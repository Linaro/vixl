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

import collections
import multiprocessing
import re
import subprocess
import time

from known_test_failures import FilterKnownTestFailures
import printer
import thread_pool
import util

REGEXP_MISSING_FEATURES = "Missing features: { ([^,}]+(, [^,}]+)*) }"

# Scan matching tests and return a test manifest.
def GetTests(runner, filters = []):
  rc, output = util.getstatusoutput(runner +  ' --list')
  if rc != 0: util.abort('Failed to list all tests')

  tests = output.split()
  for f in filters:
    tests = filter(re.compile(f).search, tests)

  return tests

def RunTest(test):
  p = subprocess.Popen(test.command,
                       stdout=subprocess.PIPE,
                       stderr=subprocess.STDOUT)
  p_out, p_err = p.communicate()
  rc = p.poll()

  if rc == 0:
    skipped = False
    lines = p_out.split('\n')
    skipped_id = "SKIPPED: "
    for i in range(len(lines)):
      if lines[i].startswith(skipped_id):
        skipped = True
        reason = lines[i][len(skipped_id):]
        with Test.n_tests_skipped.get_lock():
          Test.n_tests_skipped.value += 1
          test.shared.tests_skipped.setdefault(reason, 0)
          test.shared.tests_skipped[reason] += 1
        break
    if not skipped:
      with Test.n_tests_passed.get_lock(): Test.n_tests_passed.value += 1
  else:
    with Test.n_tests_failed.get_lock(): Test.n_tests_failed.value += 1

  printer.__print_lock__.acquire()

  printer.UpdateProgress(test.shared.start_time,
                         Test.n_tests_passed.value,
                         Test.n_tests_failed.value,
                         test.shared.n_tests,
                         Test.n_tests_skipped.value,
                         test.shared.n_known_failures,
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
  n_tests_skipped = multiprocessing.Value('i', 0)
  manager = multiprocessing.Manager()

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
    self.tests_skipped = Test.manager.dict()
    self.n_known_failures = 0
    self.known_failures = collections.Counter()

  def Add(self, test_runner_command, filters, runtime_options):
    tests = GetTests(test_runner_command, filters)
    n_tests_total = len(tests)
    tests, skipped  = FilterKnownTestFailures(tests, under_valgrind = self.under_valgrind)
    for n_tests, reason in skipped:
      if n_tests > 0:
          self.n_known_failures += n_tests
          self.known_failures[reason] += n_tests

    if len(tests) == 0:
      printer.Print('No tests to run.')
      return

    for test in tests:
      self.queue.append(Test(test_runner_command, test,
                             runtime_options, self.under_valgrind, self))

  # Run the specified tests.
  def Run(self, jobs, verbose):
    def InitGlobals():
      # Initialisation.
      self.start_time = time.time()
      self.n_tests = len(self.queue)
      if self.n_tests == 0:
        printer.Print('No tests to run.')
        return False
      Test.n_tests_passed.value = 0
      Test.n_tests_failed.value = 0
      Test.n_tests_skipped.value = 0
      self.tests_skipped.clear()
      return True

    thread_pool.Multithread(RunTest, self.queue, jobs, InitGlobals)

    printer.UpdateProgress(self.start_time,
                           Test.n_tests_passed.value,
                           Test.n_tests_failed.value,
                           self.n_tests,
                           Test.n_tests_skipped.value,
                           self.n_known_failures,
                           '== Done ==',
                           prevent_next_overwrite = True,
                           prefix = self.progress_prefix)
    n_tests_features = 0
    features = set()
    for reason, n_tests in self.tests_skipped.items():
      m = re.match(REGEXP_MISSING_FEATURES, reason)
      if m:
        if verbose:
          printer.Print("%d tests skipped because the following features are not "
                        "available '%s'" % (n_tests, m.group(1)))
        else:
          n_tests_features += n_tests
          features.update(m.group(1).split(', '))
      else:
        printer.Print("%d tests skipped because '%s'" % (n_tests, reason))

    n_tests_other = 0
    if n_tests_features > 0 :
      printer.Print("%d tests skipped because the CPU does not support "
                    "the following features: '%s'" %
                    (n_tests_features, ", ".join(features)))

    for reason, n_tests in self.known_failures.items():
        printer.Print("%d tests skipped because '%s'" % (n_tests, reason))

    # Empty the queue now that the tests have been run.
    self.queue = []
    # `0` indicates success
    return Test.n_tests_failed.value

