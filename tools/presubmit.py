#!/usr/bin/env python2.7

# Copyright 2013, ARM Limited
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

import os
import sys
import argparse
import re
import platform
import subprocess
import multiprocessing

import git
import printer
import test
import util


def BuildOptions():
  result = argparse.ArgumentParser(
          description='Run the linter and unit tests.',
          # Print default values.
          formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  result.add_argument('--verbose', '-v', action='store_true',
                      help='Print all tests output at the end.')
  result.add_argument('--notest', action='store_true',
                      help='Do not run tests. Run the linter only.')
  result.add_argument('--nolint', action='store_true',
                      help='Do not run the linter. Run the tests only.')
  result.add_argument('--noclean', action='store_true',
                      help='Do not clean before build.')
  result.add_argument('--jobs', '-j', metavar='N', type=int, nargs='?',
                      default=1, const=multiprocessing.cpu_count(),
                      help='''Runs the tests using N jobs. If the option is set
                      but no value is provided, the script will use as many jobs
                      as it thinks useful.''')
  sim_default = 'off' if platform.machine() == 'aarch64' else 'on'
  result.add_argument('--simulator', action='store', choices=['on', 'off'],
                      default=sim_default,
                      help='Explicitly enable or disable the simulator.')
  return result.parse_args()


def CleanBuildSystem():
  def clean(mode):
    if args.verbose: print('Cleaning ' + mode + ' mode test...')
    command = 'scons mode=%s simulator=%s all --clean' % \
              (mode, args.simulator)
    status, output = util.getstatusoutput(command)
    if status != 0:
      print(output)
      util.abort('Failed cleaning test: ' + command)
  clean('debug')
  clean('release')


def BuildEverything():
  def build(mode):
    if args.verbose: print('Building ' + mode + ' mode test...')
    command = 'scons mode=%s simulator=%s all -j%u' % \
              (mode, args.simulator, args.jobs)
    status, output = util.getstatusoutput(command)
    if status != 0:
      print(output)
      util.abort('Failed building test: ' + command)
  build('debug')
  build('release')


NOT_RUN = 'NOT RUN'
PASSED = 'PASSED'
FAILED = 'FAILED'

class Test:
  def __init__(self, name):
    self.name = name
    self.status = NOT_RUN

  def name_prefix(self):
    return '%-26s : ' % self.name


class Tester:
  def __init__(self):
    self.tests = []

  def AddTest(self, test):
    self.tests.append(test)

  def RunAll(self):
    result = PASSED
    for test in self.tests:
      if args.verbose: print('Running ' + test.name + '...')
      test.Run()
      if test.status != PASSED: result = FAILED
    print('Presubmit tests ' + result + '.')


class VIXLTest(Test):
  def __init__(self, mode, simulator, debugger = False, verbose = False):
    if not mode in ['release', 'debug']:
      print 'Invalid mode.'
      sys.exit(1)

    self.debugger = debugger
    self.verbose = verbose

    name = 'test ' + mode
    if simulator:
      name += ' (%s)' % ('debugger' if debugger else 'simulator')
    Test.__init__(self, name)

    self.exe = './test-runner'
    if simulator:
        self.exe += '_sim'
    if mode == 'debug':
      self.exe += '_g'

  def Run(self):
    manifest = test.ReadManifest(self.exe, [], self.debugger,
                                 False, self.verbose)
    retcode = test.RunTests(manifest, jobs = args.jobs,
                            verbose = self.verbose, debugger = self.debugger,
                            progress_prefix = self.name_prefix())
    printer.EnsureNewLine()
    self.status = PASSED if retcode == 0 else FAILED


class LintTest(Test):
  def __init__(self):
    name = 'cpp lint'
    Test.__init__(self, name)

  def Run(self):
    if not lint.IsCppLintAvailable():
      self.status = FAILED
      print self.name_prefix() + FAILED + '''
cpplint.py not found. Please ensure the depot tools are installed and in your
PATH. See http://dev.chromium.org/developers/how-tos/install-depot-tools for
details.'''
      return

    n_errors = lint.LintFiles(lint.default_tracked_files,
                              jobs = args.jobs, verbose = args.verbose,
                              progress_prefix = self.name_prefix())

    self.status = PASSED if n_errors == 0 else FAILED


class BenchTest(Test):
  def __init__(self, mode, simulator):
    name = 'benchmarks ' + mode
    Test.__init__(self, name)
    self.exe_suffix = ''
    if simulator:
      self.exe_suffix += '_sim'
    if mode == 'debug':
      self.exe_suffix += '_g'

  def Run(self):
    benchmarks = ['bench-dataop', 'bench-branch', 'bench-branch-link']
    self.status = PASSED
    for bench in benchmarks:
      command = './' + bench + self.exe_suffix
      (rc, out) = util.getstatusoutput(command)
      if rc != 0:
        self.status = FAILED
        print self.name_prefix() + 'Failed to run `' + command + '`'
    print self.name_prefix() + self.status



if __name__ == '__main__':
  original_dir = os.path.abspath('.')
  # $ROOT/tools/presubmit.py
  root_dir = os.path.dirname(os.path.dirname(os.path.abspath(sys.argv[0])))
  os.chdir(root_dir)
  args = BuildOptions()

  if not args.nolint and not git.is_git_repository_root():
    print 'WARNING: This is not a Git repository. The linter will not run.'
    args.nolint = True

  tester = Tester()
  if not args.nolint:
    import lint
    tester.AddTest(LintTest())

  if not args.notest:
    if not args.noclean:
      CleanBuildSystem()
    BuildEverything()

    if args.simulator == 'on':
      #                        mode,      sim,   debugger, verbose
      tester.AddTest(VIXLTest('release',  True,  True,     args.verbose))
      tester.AddTest(VIXLTest('debug',    True,  True,     args.verbose))
      tester.AddTest(VIXLTest('release',  True,  False,    args.verbose))
      tester.AddTest(VIXLTest('debug',    True,  False,    args.verbose))
      tester.AddTest(BenchTest('release', True))
      tester.AddTest(BenchTest('debug',   True))
    else:
      tester.AddTest(VIXLTest('release',  False, False,    args.verbose))
      tester.AddTest(VIXLTest('debug',    False, False,    args.verbose))
      tester.AddTest(BenchTest('release', False))
      tester.AddTest(BenchTest('debug',   False))

  tester.RunAll()

  if git.is_git_repository_root():
    untracked_files = git.get_untracked_files()
    if untracked_files:
      print '\nWARNING: The following files are untracked:'
      for f in untracked_files:
        print f.lstrip('?')
