#!/usr/bin/env python2.7

# Copyright 2014, ARM Limited
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
import time
import util


from printer import EnsureNewLine, Print, UpdateProgress


def BuildOptions():
  result = argparse.ArgumentParser(
      description =
      '''This tool runs each test reported by $TEST --list (and filtered as
         specified). A summary will be printed, and detailed test output will be
         stored in log/$TEST.''',
      # Print default values.
      formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  result.add_argument('filters', metavar='filter', nargs='*',
                      help='Run tests matching all of the (regexp) filters.')
  result.add_argument('--runner', action='store', required=True,
                      help='The test executable to run.')
  result.add_argument('--coloured_trace', action='store_true',
                      help='''Pass --coloured_trace to the test runner. This
                              will put colour codes in the log files. The
                              coloured output can be viewed by "less -R", for
                              example.''')
  result.add_argument('--debugger', action='store_true',
                      help='''Pass --debugger to test, so that the debugger is
                              used instead of the simulator. This has no effect
                              when running natively.''')
  result.add_argument('--verbose', action='store_true',
                      help='Print verbose output.')
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


def VerbosePrint(verbose, string):
  if verbose:
    Print(string)


# A class representing an individual test.
class Test:
  def __init__(self, name, runner, debugger, coloured_trace, verbose):
    self.name = name
    self.runner = runner
    self.debugger = debugger
    self.coloured_trace = coloured_trace
    self.verbose = verbose
    self.logpath = os.path.join('log', os.path.basename(self.runner))
    if self.debugger:
      basename = name + '_debugger'
    else:
      basename = name
    self.logout = os.path.join(self.logpath, basename + '.stdout')
    self.logerr = os.path.join(self.logpath, basename + '.stderr')
    if not os.path.exists(self.logpath): os.makedirs(self.logpath)

  # Run the test.
  # Use a thread to be able to control the test.
  def Run(self):
    command = \
        [self.runner, '--trace_sim', '--trace_reg', '--trace_write', self.name]
    if self.coloured_trace:
      command.append('--coloured_trace')
    if self.debugger:
      command.append('--debugger')

    VerbosePrint(self.verbose, '==== Running ' + self.name + '... ====')
    sys.stdout.flush()

    process = subprocess.Popen(command,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    # Get the output and return status of the test.
    stdout, stderr = process.communicate()
    retcode = process.poll()

    # Write stdout and stderr to the log.
    with open(self.logout, 'w') as f: f.write(stdout)
    with open(self.logerr, 'w') as f: f.write(stderr)

    if retcode == 0:
      # Success.
      # We normally only print the command on failure, but with --verbose we
      # should also print it on success.
      VerbosePrint(self.verbose, 'COMMAND: ' + ' '.join(command))
      VerbosePrint(self.verbose, 'LOG (stdout): ' + self.logout)
      VerbosePrint(self.verbose, 'LOG (stderr): ' + self.logerr + '\n')
    else:
      # Failure.
      Print('--- FAILED ' + self.name + ' ---')
      Print('COMMAND: ' + ' '.join(command))
      Print('LOG (stdout): ' + self.logout)
      Print('LOG (stderr): ' + self.logerr + '\n')

    return retcode


# Scan matching tests and return a test manifest.
def ReadManifest(runner, filters = [],
                 debugger = False, coloured_trace = False, verbose = False):
  status, output = util.getstatusoutput(runner +  ' --list')
  if status != 0: util.abort('Failed to list all tests')

  names = output.split()
  for f in filters:
    names = filter(re.compile(f).search, names)

  return map(lambda x:
      Test(x, runner, debugger, coloured_trace, verbose), names)


# Shared state for multiprocessing. Ideally the context should be passed with
# arguments, but constraints from the multiprocessing module prevent us from
# doing so: the shared variables (multiprocessing.Value) must be global, or no
# work is started. So we abstract some additional state into global variables to
# simplify the implementation.
# Read-write variables for the workers.
n_tests_passed = multiprocessing.Value('i', 0)
n_tests_failed = multiprocessing.Value('i', 0)
# Read-only for workers.
n_tests = None
start_time = None
verbose_test_run = None
test_suite_name = ''

def RunTest(test):
  UpdateProgress(start_time, n_tests_passed.value, n_tests_failed.value,
                 n_tests, verbose_test_run, test.name, test_suite_name)
  # Run the test and update the statistics.
  retcode = test.Run()
  if retcode == 0:
    with n_tests_passed.get_lock(): n_tests_passed.value += 1
  else:
    with n_tests_failed.get_lock(): n_tests_failed.value += 1


# Run all tests in the manifest.
# This function won't run in parallel due to constraints from the
# multiprocessing module.
__run_tests_lock__ = multiprocessing.Lock()
def RunTests(manifest, jobs = 1, verbose = False, debugger = False,
             progress_prefix = ''):
  global n_tests
  global start_time
  global verbose_test_run
  global test_suite_name

  with __run_tests_lock__:

    # Reset the counters.
    n_tests_passed.value = 0
    n_tests_failed.value = 0

    verbose_test_run = verbose
    test_suite_name = progress_prefix

    n_tests = len(manifest)

    if n_tests == 0:
      Print('No tests to run.')
      return 0

    VerbosePrint(verbose, 'Running %d tests...' % (n_tests))
    start_time = time.time()

    pool = multiprocessing.Pool(jobs)
    # The '.get(9999999)' is workaround to allow killing the test script with
    # ctrl+C from the shell. This bug is documented at
    # http://bugs.python.org/issue8296.
    work = pool.map_async(RunTest, manifest).get(9999999)

    done_message = '== Done =='
    UpdateProgress(start_time, n_tests_passed.value, n_tests_failed.value,
                   n_tests, verbose, done_message, progress_prefix)

    return n_tests_failed.value # 0 indicates success


if __name__ == '__main__':
  # $ROOT/tools/test.py
  root_dir = os.path.dirname(os.path.dirname(os.path.abspath(sys.argv[0])))

  # Parse the arguments.
  args = BuildOptions()

  # Find a valid path to args.runner (in case it doesn't begin with './').
  args.runner = os.path.join('.', args.runner)

  if not os.access(args.runner, os.X_OK):
    print "'" + args.test + "' is not executable or does not exist."
    sys.exit(1)

  # List all matching tests.
  manifest = ReadManifest(args.runner, args.filters,
                          args.debugger, args.coloured_trace, args.verbose)

  # Run the tests.
  status = RunTests(manifest, jobs=args.jobs,
                    verbose=args.verbose, debugger=args.debugger)
  EnsureNewLine()

  sys.exit(status)

