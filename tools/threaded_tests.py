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

import multiprocessing
import re
import signal
import subprocess
import sys
import time

import printer
import util

# Catch SIGINT to gracefully exit when ctrl+C is pressed.
def SigIntHandler(signal, frame):
  sys.exit(1)

signal.signal(signal.SIGINT, SigIntHandler)


# Scan matching tests and return a test manifest.
def GetTests(runner, filters = []):
  rc, output = util.getstatusoutput(runner +  ' --list')
  if rc != 0: util.abort('Failed to list all tests')

  tests = output.split()
  for f in filters:
    print f
    tests = filter(re.compile(f).search, tests)

  return tests


# Shared state for multiprocessing. Ideally the context should be passed with
# arguments, but constraints from the multiprocessing module prevent us from
# doing so: the shared variables (multiprocessing.Value) must be global, or no
# work is started. So we abstract some additional state into global variables to
# simplify the implementation.
# Read-write variables for the workers.
n_tests_passed = multiprocessing.Value('i', 0)
n_tests_failed = multiprocessing.Value('i', 0)
# Read-only for workers.
test_runner = None
test_runner_runtime_options = None
test_runner_under_valgrind = False
n_tests = None
start_time = None
progress_prefix = None


def RunTest(test):
  command = [test_runner, test] + test_runner_runtime_options
  if test_runner_under_valgrind:
    command = ['valgrind'] + command

  p = subprocess.Popen(command,
                       stdout=subprocess.PIPE,
                       stderr=subprocess.STDOUT)
  p_out, p_err = p.communicate()
  rc = p.poll()

  if rc == 0:
    with n_tests_passed.get_lock(): n_tests_passed.value += 1
  else:
    with n_tests_failed.get_lock(): n_tests_failed.value += 1

  printer.__print_lock__.acquire()

  printer.UpdateProgress(start_time,
                         n_tests_passed.value,
                         n_tests_failed.value,
                         n_tests,
                         test,
                         prevent_next_overwrite = (rc != 0),
                         has_lock = True,
                         prefix = progress_prefix)

  if rc != 0:
    printer.Print('FAILED: ' + test, has_lock = True)
    printer.Print(printer.COLOUR_RED + ' '.join(command) + printer.NO_COLOUR,
                  has_lock = True)
    printer.Print(p_out, has_lock = True)

  printer.__print_lock__.release()


# Run the specified tests.
# This function won't run in parallel due to constraints from the
# multiprocessing module.
__run_tests_lock__ = multiprocessing.Lock()
def RunTests(test_runner_command, filters, runtime_options,
             under_valgrind = False,
             jobs = 1, prefix = ''):
  global test_runner
  global test_runner_runtime_options
  global test_runner_under_valgrind
  global n_tests
  global start_time
  global progress_prefix

  tests = GetTests(test_runner_command, filters)

  if n_tests == 0:
    printer.Print('No tests to run.')
    return 0

  with __run_tests_lock__:

    # Initialisation.
    start_time = time.time()
    test_runner = test_runner_command
    test_runner_runtime_options = runtime_options
    test_runner_under_valgrind = under_valgrind
    n_tests = len(tests)
    n_tests_passed.value = 0
    n_tests_failed.value = 0
    progress_prefix = prefix

    pool = multiprocessing.Pool(jobs)
    # The '.get(9999999)' is a workaround to allow killing the test script with
    # ctrl+C from the shell. This bug is documented at
    # http://bugs.python.org/issue8296.
    work = pool.map_async(RunTest, tests).get(9999999)
    pool.close()
    pool.join()

    printer.UpdateProgress(start_time,
                           n_tests_passed.value,
                           n_tests_failed.value,
                           n_tests,
                           '== Done ==',
                           prevent_next_overwrite = True,
                           prefix = progress_prefix)

  # `0` indicates success
  return n_tests_failed.value
