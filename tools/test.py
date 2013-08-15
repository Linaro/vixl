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
import subprocess
import threading
import time
import util


def BuildOptions():
  result = argparse.ArgumentParser(description = 'Unit test tool')
  result.add_argument('name_filters', metavar='name filters', nargs='*',
                      help='Tests matching any of the regexp filters will be run.')
  result.add_argument('--mode', action='store', choices=['release', 'debug', 'coverage'],
                      default='release', help='Build mode')
  result.add_argument('--simulator', action='store', choices=['on', 'off'],
                      default='on', help='Use the builtin a64 simulator')
  result.add_argument('--timeout', action='store', type=int, default=5,
                      help='Timeout (in seconds) for each cctest (5sec default).')
  result.add_argument('--nobuild', action='store_true',
                      help='Do not (re)build the tests')
  result.add_argument('--jobs', '-j', metavar='N', type=int, default=1,
                      help='Allow N jobs at once.')
  return result.parse_args()


def BuildRequiredObjects(arguments):
  status, output = util.getstatusoutput('scons ' +
                                        'mode=' + arguments.mode + ' ' +
                                        'simulator=' + arguments.simulator + ' ' +
                                        'target=cctest ' +
                                        '--jobs=' + str(arguments.jobs))

  if status != 0:
    print(output)
    util.abort('Failed bulding cctest')


# Display the run progress:
# [time| progress|+ passed|- failed]
def UpdateProgress(start_time, passed, failed, card):
  minutes, seconds = divmod(time.time() - start_time, 60)
  progress = float(passed + failed) / card * 100
  passed_colour = '\x1b[32m' if passed != 0 else ''
  failed_colour = '\x1b[31m' if failed != 0 else ''
  indicator = '\r[%02d:%02d| %3d%%|' + passed_colour + '+ %d\x1b[0m|' + failed_colour + '- %d\x1b[0m]'
  sys.stdout.write(indicator % (minutes, seconds, progress, passed, failed))


def PrintError(s):
  # Print the error on a new line.
  sys.stdout.write('\n')
  print(s)
  sys.stdout.flush()


# List all tests matching any of the provided filters.
def ListTests(cctest, filters):
  status, output = util.getstatusoutput(cctest +  ' --list')
  if status != 0: util.abort('Failed to list all tests')

  available_tests = output.split()
  if filters:
    filters = map(re.compile, filters)
    def is_selected(test_name):
      for e in filters:
        if e.search(test_name):
          return True
      return False

    return filter(is_selected, available_tests)
  else:
    return available_tests


# A class representing a cctest.
class CCtest:
  cctest = None

  def __init__(self, name, options = None):
    self.name = name
    self.options = options
    self.process = None
    self.stdout = None
    self.stderr = None

  def Command(self):
    command = '%s %s' % (CCtest.cctest, self.name)
    if self.options is not None:
      command = '%s %s' % (command, ' '.join(self.options))

    return command

  # Run the test.
  # Use a thread to be able to control the test.
  def Run(self, arguments):
    command = [CCtest.cctest, self.name]
    if self.options is not None:
      command += self.options

    def execute():
      self.process = subprocess.Popen(command,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.PIPE)
      self.stdout, self.stderr = self.process.communicate()

    thread = threading.Thread(target=execute)
    retcode = -1
    # Append spaces to hide the previous test name if longer.
    sys.stdout.write('  ' + self.name + ' ' * 20)
    sys.stdout.flush()
    # Start the test with a timeout.
    thread.start()
    thread.join(arguments.timeout)
    if thread.is_alive():
      # Too slow! Terminate.
      PrintError('### TIMEOUT %s\nCOMMAND:\n%s' % (self.name, self.Command()))
      # If timeout was too small the thread may not have run and self.process
      # is still None. Therefore check.
      if (self.process):
        self.process.terminate()
      # Allow 1 second to terminate. Else, exterminate!
      thread.join(1)
      if thread.is_alive():
        thread.kill()
        thread.join()
      # retcode is already set for failure.
    else:
      # Check the return status of the test.
      retcode = self.process.poll()
      if retcode != 0:
        PrintError('### FAILED %s\nSTDERR:\n%s\nSTDOUT:\n%s\nCOMMAND:\n%s'
                   % (self.name, self.stderr.decode(), self.stdout.decode(),
                      self.Command()))

    return retcode


# Run all tests in the list 'tests'.
def RunTests(cctest, tests, arguments):
  CCtest.cctest = cctest
  card = len(tests)
  passed = 0
  failed = 0

  if card == 0:
    print('No test to run')
    return 0

  # When the simulator is on the tests are ran twice: with and without the
  # debugger.
  if arguments.simulator:
    card *= 2

  print('Running %d tests... (timeout = %ds)' % (card, arguments.timeout))
  start_time = time.time()

  # Initialize the progress indicator.
  UpdateProgress(start_time, 0, 0, card)
  for e in tests:
    variants = [CCtest(e)]
    if arguments.simulator: variants.append(CCtest(e, ['--debugger']))
    for v in variants:
      retcode = v.Run(arguments)
      # Update the counters and progress indicator.
      if retcode == 0:
        passed += 1
      else:
        failed += 1
    UpdateProgress(start_time, passed, failed, card)

  return failed


if __name__ == '__main__':
  original_dir = os.path.abspath('.')
  # $ROOT/tools/test.py
  root_dir = os.path.dirname(os.path.dirname(os.path.abspath(sys.argv[0])))
  os.chdir(root_dir)

  # Parse the arguments and build the executable.
  args = BuildOptions()
  if not args.nobuild:
    BuildRequiredObjects(args)

  # The test binary.
  cctest = os.path.join(root_dir, 'cctest')
  if args.simulator == 'on':
    cctest += '_sim'
  if args.mode == 'debug':
    cctest += '_g'
  elif args.mode == 'coverage':
    cctest += '_gcov'

  # List available tests.
  tests = ListTests(cctest, args.name_filters)

  # Delete coverage data files.
  if args.mode == 'coverage':
    status, output = util.getstatusoutput('find obj/coverage -name "*.gcda" -exec rm {} \;')

  # Run the tests.
  status = RunTests(cctest, tests, args)
  sys.stdout.write('\n')

  # Print coverage information.
  if args.mode == 'coverage':
    cmd = 'tggcov -R summary_all,untested_functions_per_file obj/coverage/src/aarch64'
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    print(stdout)

  # Restore original directory.
  os.chdir(original_dir)

  sys.exit(status)

