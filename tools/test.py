#!/usr/bin/env python2.7

# Copyright 2015, ARM Limited
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   * Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#   * Neither the name of ARM Limited nor the names of its contributors may be
#     used to endorse or promote products derived from this software without
#     specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import argparse
import fcntl
import git
import itertools
import multiprocessing
import os
from os.path import join
import platform
import re
import subprocess
import sys
import time

import config
import lint
import printer
import test
import threaded_tests
import util


dir_root = config.dir_root

def Optionify(name):
  return '--' + name


# The options that can be tested are abstracted to provide an easy way to add
# new ones.
# Environment options influence the environment. They can be used for example to
# set the compiler used.
# Build options are options passed to scons, with a syntax like `scons opt=val`
# Runtime options are options passed to the test program.
# See the definition of `test_options` below.

# 'all' is a special value for the options. If specified, all other values of
# the option are tested.
class TestOption(object):
  type_environment = 'type_environment'
  type_build = 'type_build'
  type_run = 'type_run'

  def __init__(self, option_type, name, help,
               val_test_choices, val_test_default = None,
               # If unset, the user can pass any value.
               strict_choices = True):
    self.name = name
    self.option_type = option_type
    self.help = help
    self.val_test_choices = val_test_choices
    self.strict_choices = strict_choices
    if val_test_default is not None:
      self.val_test_default = val_test_default
    else:
      self.val_test_default = val_test_choices[0]

  def ArgList(self, to_test):
    res = []
    if to_test == 'all':
      for value in self.val_test_choices:
        if value != 'all':
          res.append(self.GetOptionString(value))
    else:
      for value in to_test:
        res.append(self.GetOptionString(value))
    return res

class EnvironmentOption(TestOption):
  option_type = TestOption.type_environment
  def __init__(self, name, environment_variable_name, help,
               val_test_choices, val_test_default = None,
               strict_choices = True):
    super(EnvironmentOption, self).__init__(EnvironmentOption.option_type,
                                      name,
                                      help,
                                      val_test_choices,
                                      val_test_default,
                                      strict_choices = strict_choices)
    self.environment_variable_name = environment_variable_name

  def GetOptionString(self, value):
    return self.environment_variable_name + '=' + value


class BuildOption(TestOption):
  option_type = TestOption.type_build
  def __init__(self, name, help,
               val_test_choices, val_test_default = None,
               strict_choices = True):
    super(BuildOption, self).__init__(BuildOption.option_type,
                                      name,
                                      help,
                                      val_test_choices,
                                      val_test_default,
                                      strict_choices = strict_choices)
  def GetOptionString(self, value):
    return self.name + '=' + value


class RuntimeOption(TestOption):
  option_type = TestOption.type_run
  def __init__(self, name, help,
               val_test_choices, val_test_default = None):
    super(RuntimeOption, self).__init__(RuntimeOption.option_type,
                                        name,
                                        help,
                                        val_test_choices,
                                        val_test_default)
  def GetOptionString(self, value):
    if value == 'on':
      return Optionify(self.name)
    else:
      return None



environment_option_compiler = \
  EnvironmentOption('compiler', 'CXX', 'Test for the specified compilers.',
                    val_test_choices=['all'] + config.tested_compilers,
                    strict_choices = False)
test_environment_options = [
  environment_option_compiler
]

build_option_mode = \
  BuildOption('mode', 'Test with the specified build modes.',
              val_test_choices=['all'] + config.build_options_modes)
build_option_standard = \
  BuildOption('std', 'Test with the specified C++ standard.',
              val_test_choices=['all'] + config.tested_cpp_standards,
              strict_choices = False)
test_build_options = [
  build_option_mode,
  build_option_standard
]

runtime_option_debugger = \
  RuntimeOption('debugger',
                '''Test with the specified configurations for the debugger.
                Note that this is only tested if we are using the simulator.''',
                val_test_choices=['all', 'on', 'off'])
test_runtime_options = [
  runtime_option_debugger
]

test_options = \
  test_environment_options + test_build_options + test_runtime_options


def BuildOptions():
  args = argparse.ArgumentParser(
    description =
    '''This tool runs all tests matching the speficied filters for multiple
    environment, build options, and runtime options configurations.''',
    # Print default values.
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)

  args.add_argument('filters', metavar='filter', nargs='*',
                    help='Run tests matching all of the (regexp) filters.')

  # We automatically build the script options from the options to be tested.
  test_arguments = args.add_argument_group(
    'Test options',
    'These options indicate what should be tested')
  for option in test_options:
    choices = option.val_test_choices if option.strict_choices else None
    help = option.help
    if not option.strict_choices:
      help += ' Supported values: {' + ','.join(option.val_test_choices) + '}'
    test_arguments.add_argument(Optionify(option.name),
                                nargs='+',
                                choices=choices,
                                default=option.val_test_default,
                                help=help,
                                action='store')

  general_arguments = args.add_argument_group('General options')
  general_arguments.add_argument('--fast', action='store_true',
                                 help='''Skip the lint tests, and run only with
                                 one compiler, in one mode, with one C++
                                 standard, and with an appropriate default for
                                 runtime options. The compiler, mode, and C++
                                 standard used are the first ones provided to
                                 the script or in the default arguments.''')
  general_arguments.add_argument(
    '--jobs', '-j', metavar='N', type=int, nargs='?',
    default=multiprocessing.cpu_count(),
    const=multiprocessing.cpu_count(),
    help='''Runs the tests using N jobs. If the option is set but no value is
    provided, the script will use as many jobs as it thinks useful.''')
  general_arguments.add_argument('--nobench', action='store_true',
                                 help='Do not run benchmarks.')
  general_arguments.add_argument('--nolint', action='store_true',
                                 help='Do not run the linter.')
  general_arguments.add_argument('--notest', action='store_true',
                                 help='Do not run tests.')
  sim_default = 'off' if platform.machine() == 'aarch64' else 'on'
  general_arguments.add_argument(
    '--simulator', action='store', choices=['on', 'off'],
    default=sim_default,
    help='Explicitly enable or disable the simulator.')
  general_arguments.add_argument(
    '--under_valgrind', action='store_true',
    help='''Run the test-runner commands under Valgrind.
            Note that a few tests are known to fail because of
            issues in Valgrind''')
  return args.parse_args()


def RunCommand(command, environment_options = None):
  # Create a copy of the environment. We do not want to pollute the environment
  # of future commands run.
  environment = os.environ
  # Configure the environment.
  # TODO: We currently pass the options as strings, so we need to parse them. We
  # should instead pass them as a data structure and build the string option
  # later. `environment_options` looks like `['CXX=compiler', 'OPT=val']`.
  if environment_options:
    for option in environment_options:
      opt, val = option.split('=')
      environment[opt] = val

  printable_command = ''
  if environment_options:
    printable_command += ' '.join(environment_options) + ' '
  printable_command += ' '.join(command)

  printable_command_orange = \
    printer.COLOUR_ORANGE + printable_command + printer.NO_COLOUR
  printer.PrintOverwritableLine(printable_command_orange)
  sys.stdout.flush()

  # Start a process for the command.
  # Interleave `stderr` and `stdout`.
  p = subprocess.Popen(command,
                       stdout=subprocess.PIPE,
                       stderr=subprocess.STDOUT,
                       env=environment)

  # We want to be able to display a continuously updated 'work indicator' while
  # the process is running. Since the process can hang if the `stdout` pipe is
  # full, we need to pull from it regularly. We cannot do so via the
  # `readline()` function because it is blocking, and would thus cause the
  # indicator to not be updated properly. So use file control mechanisms
  # instead.
  indicator = ' (still working: %d seconds elapsed)'

  # Mark the process output as non-blocking.
  flags = fcntl.fcntl(p.stdout, fcntl.F_GETFL)
  fcntl.fcntl(p.stdout, fcntl.F_SETFL, flags | os.O_NONBLOCK)

  t_start = time.time()
  t_last_indication = t_start
  process_output = ''

  # Keep looping as long as the process is running.
  while p.poll() is None:
    # Avoid polling too often.
    time.sleep(0.1)
    # Update the progress indicator.
    t_current = time.time()
    if (t_current - t_start >= 2) and (t_current - t_last_indication >= 1):
      printer.PrintOverwritableLine(
        printable_command_orange + indicator % int(t_current - t_start))
      sys.stdout.flush()
      t_last_indication = t_current
    # Pull from the process output.
    while True:
      try:
        line = os.read(p.stdout.fileno(), 1024)
      except OSError:
        line = ''
        break
      if line == '': break
      process_output += line

  # The process has exited. Don't forget to retrieve the rest of its output.
  out, err = p.communicate()
  rc = p.poll()
  process_output += out

  if rc == 0:
    printer.Print(printer.COLOUR_GREEN + printable_command + printer.NO_COLOUR)
  else:
    printer.Print(printer.COLOUR_RED + printable_command + printer.NO_COLOUR)
    printer.Print(process_output)
  return rc


def RunLinter():
  rc, default_tracked_files = lint.GetDefaultTrackedFiles()
  if rc:
    return rc
  return lint.LintFiles(map(lambda x: join(dir_root, x), default_tracked_files),
                        jobs = args.jobs, progress_prefix = 'cpp lint: ')



def BuildAll(build_options, jobs):
  scons_command = ["scons", "-C", dir_root, 'all', '-j', str(jobs)]
  scons_command += list(build_options)
  return RunCommand(scons_command, list(environment_options))


def RunBenchmarks():
  rc = 0
  benchmark_names = util.ListCCFilesWithoutExt(config.dir_benchmarks)
  for bench in benchmark_names:
    rc |= RunCommand(
      [os.path.realpath(join(config.dir_build_latest, 'benchmarks', bench))])
  return rc


def PrintStatus(success):
  printer.Print('\n$ ' + ' '.join(sys.argv))
  if success:
    printer.Print('SUCCESS')
  else:
    printer.Print('FAILURE')



if __name__ == '__main__':
  util.require_program('scons')
  rc = 0

  args = BuildOptions()

  if args.under_valgrind:
    util.require_program('valgrind')

  if args.fast:
    def SetFast(option, specified, default):
      option.val_test_choices = \
        [default[0] if specified == 'all' else specified[0]]
    SetFast(environment_option_compiler, args.compiler, config.tested_compilers)
    SetFast(build_option_mode, args.mode, config.build_options_modes)
    SetFast(build_option_standard, args.std, config.tested_cpp_standards)
    SetFast(runtime_option_debugger, args.debugger, ['on', 'off'])

  if not args.nolint and not args.fast:
    rc |= RunLinter()

  # Don't try to test the debugger if we are not running with the simulator.
  if not args.simulator:
    test_runtime_options = \
      filter(lambda x: x.name != 'debugger', test_runtime_options)

  # List all combinations of options that will be tested.
  def ListCombinations(args, options):
    opts_list = map(lambda opt : opt.ArgList(args.__dict__[opt.name]), options)
    return list(itertools.product(*opts_list))
  test_env_combinations = ListCombinations(args, test_environment_options)
  test_build_combinations = ListCombinations(args, test_build_options)
  test_runtime_combinations = ListCombinations(args, test_runtime_options)

  for environment_options in test_env_combinations:
    for build_options in test_build_combinations:
      # Avoid going through the build stage if we are not using the build
      # result.
      if not (args.notest and args.nobench):
        build_rc = BuildAll(build_options, args.jobs)
        # Don't run the tests for this configuration if the build failed.
        if build_rc != 0:
          rc |= build_rc
          continue

      # Use the realpath of the test executable so that the commands printed
      # can be copy-pasted and run.
      test_executable = os.path.realpath(
        join(config.dir_build_latest, 'test', 'test-runner'))

      if not args.notest:
        printer.Print(test_executable)

      for runtime_options in test_runtime_combinations:
        if not args.notest:
          runtime_options = [x for x in runtime_options if x is not None]
          prefix = '  ' + ' '.join(runtime_options) + '  '
          rc |= threaded_tests.RunTests(test_executable,
                                        args.filters,
                                        list(runtime_options),
                                        args.under_valgrind,
                                        jobs = args.jobs, prefix = prefix)

      if not args.nobench:
        rc |= RunBenchmarks()

  PrintStatus(rc == 0)
