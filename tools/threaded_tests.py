# Copyright 2015, VIXL authors
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

from known_test_failures import FilterKnownTestFailures
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

def RunTest(test):
  p = subprocess.Popen(test.command,
                       stdout=subprocess.PIPE,
                       stderr=subprocess.STDOUT)
  p_out, p_err = p.communicate()
  rc = p.poll()

  if rc == 0:
    with Test.n_tests_passed.get_lock(): Test.n_tests_passed.value += 1
  else:
    with Test.n_tests_failed.get_lock(): Test.n_tests_failed.value += 1

  printer.__print_lock__.acquire()

  printer.UpdateProgress(test.shared.start_time,
                         Test.n_tests_passed.value,
                         Test.n_tests_failed.value,
                         test.shared.n_tests,
                         test.name,
                         prevent_next_overwrite = (rc != 0),
                         has_lock = True,
                         prefix = test.shared.progress_prefix)

  if rc != 0:
    printer.Print('FAILED: ' + test.name, has_lock = True)
    printer.Print(printer.COLOUR_RED + ' '.join(test.command) + printer.NO_COLOUR,
                  has_lock = True)
    printer.Print(p_out, has_lock = True)

  printer.__print_lock__.release()

class Test(object):
  # Shared state for multiprocessing. Ideally the context should be passed with
  # arguments, but constraints from the multiprocessing module prevent us from
  # doing so: the shared variables (multiprocessing.Value) must be either global
  # or static, or no work is started.
  n_tests_passed = multiprocessing.Value('i', 0)
  n_tests_failed = multiprocessing.Value('i', 0)

  def __init__(self, test_runner, test, runtime_options, use_valgrind, shared):
    self.command = [test_runner, test] + runtime_options
    self.name = test
    self.shared = shared
    if use_valgrind:
      self.command = ['valgrind'] + self.command

class TestQueue(object):
  def __init__(self, under_valgrind = False, prefix = ''):
    self.progress_prefix = prefix
    self.under_valgrind = under_valgrind
    self.queue = []

  def Add(self, test_runner_command, filters, runtime_options):
    tests = GetTests(test_runner_command, filters)
    tests = FilterKnownTestFailures(tests, under_valgrind = self.under_valgrind)

    if len(tests) == 0:
      printer.Print('No tests to run.')
      return

    for test in tests:
      self.queue.append(Test(test_runner_command, test,
                             runtime_options, self.under_valgrind, self))

  # Run the specified tests.
  # This function won't run in parallel due to constraints from the
  # multiprocessing module.
  __run_tests_lock__ = multiprocessing.Lock()
  def Run(self, jobs):
    with TestQueue.__run_tests_lock__:
      # Initialisation.
      self.start_time = time.time()
      self.n_tests = len(self.queue)
      if self.n_tests == 0:
        printer.Print('No tests to run.')
        return 0
      Test.n_tests_passed.value = 0
      Test.n_tests_failed.value = 0

      pool = multiprocessing.Pool(jobs)
      # The '.get(9999999)' is a workaround to allow killing the test script with
      # ctrl+C from the shell. This bug is documented at
      # http://bugs.python.org/issue8296.
      work = pool.map_async(RunTest, self.queue).get(9999999)
      pool.close()
      pool.join()

      printer.UpdateProgress(self.start_time,
                             Test.n_tests_passed.value,
                             Test.n_tests_failed.value,
                             self.n_tests,
                             '== Done ==',
                             prevent_next_overwrite = True,
                             prefix = self.progress_prefix)

    # Empty the queue now that the tests have been run.
    self.queue = []
    # `0` indicates success
    return Test.n_tests_failed.value

