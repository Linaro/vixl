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
import time
import util


from printer import EnsureNewLine, Print, UpdateProgress


def BuildOptions():
  result = argparse.ArgumentParser(description =
      '''This tool runs each test reported by $CCTEST --list (and filtered as
         specified). A summary will be printed, and detailed test output will be
         stored in log/$CCTEST.''')
  result.add_argument('filters', metavar='filter', nargs='*',
                      help='Run tests matching all of the (regexp) filters.')
  result.add_argument('--cctest', action='store', required=True,
                      help='The cctest executable to run.')
  result.add_argument('--coloured_trace', action='store_true',
                      help='''Pass --coloured_trace to cctest. This will put
                              colour codes in the log files. The coloured output
                              can be viewed by "less -R", for example.''')
  result.add_argument('--coverage', action='store_true',
                      help='Run coverage tests.')
  result.add_argument('--debugger', action='store_true',
                      help='''Pass --debugger to cctest, so that the debugger is
                              used instead of the simulator. This has no effect
                              when running natively.''')
  result.add_argument('--verbose', action='store_true',
                      help='Print verbose output.')
  return result.parse_args()


def VerbosePrint(string):
  if args.verbose:
    Print(string)


# A class representing an individual test.
class Test:
  def __init__(self, name):
    self.name = name
    self.logpath = os.path.join('log', os.path.basename(args.cctest))
    if args.debugger:
      basename = name + '_debugger'
    else:
      basename = name
    self.logout = os.path.join(self.logpath, basename + '.stdout')
    self.logerr = os.path.join(self.logpath, basename + '.stderr')

  # Run the test.
  # Use a thread to be able to control the test.
  def Run(self):
    command = [args.cctest, '--trace_sim', '--trace_reg', self.name]
    if args.coloured_trace:
      command.append('--coloured_trace')

    VerbosePrint('==== Running ' + self.name + '... ====')
    sys.stdout.flush()

    process = subprocess.Popen(command,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    # Get the output and return status of the test.
    stdout, stderr = process.communicate()
    retcode = process.poll()

    # Write stdout and stderr to the log.
    if not os.path.exists(self.logpath): os.makedirs(self.logpath)
    with open(self.logout, 'w') as f: f.write(stdout)
    with open(self.logerr, 'w') as f: f.write(stderr)

    if retcode == 0:
      # Success.
      # We normally only print the command on failure, but with --verbose we
      # should also print it on success.
      VerbosePrint('COMMAND: ' + ' '.join(command))
      VerbosePrint('LOG (stdout): ' + self.logout)
      VerbosePrint('LOG (stderr): ' + self.logerr + '\n')
    else:
      # Failure.
      Print('--- FAILED ' + self.name + ' ---')
      Print('COMMAND: ' + ' '.join(command))
      Print('LOG (stdout): ' + self.logout)
      Print('LOG (stderr): ' + self.logerr + '\n')

    return retcode


# Scan matching tests and return a test manifest.
def ReadManifest(filters):
  status, output = util.getstatusoutput(args.cctest +  ' --list')
  if status != 0: util.abort('Failed to list all tests')

  names = output.split()
  for f in filters:
    names = filter(re.compile(f).search, names)

  return map(Test, names)


# Run all tests in the manifest.
def RunTests(manifest):
  count = len(manifest)
  passed = 0
  failed = 0

  if count == 0:
    Print('No tests to run.')
    return 0

  Print('Running %d tests...' % (count))
  start_time = time.time()

  for test in manifest:
    # Update the progress counter with the name of the test we're about to run.
    UpdateProgress(start_time, passed, failed, count, args.verbose, test.name)
    retcode = test.Run()
    # Update the counters and progress indicator.
    if retcode == 0:
      passed += 1
    else:
      failed += 1
  UpdateProgress(start_time, passed, failed, count, args.verbose, '== Done ==')

  return failed     # 0 indicates success.


if __name__ == '__main__':
  # $ROOT/tools/test.py
  root_dir = os.path.dirname(os.path.dirname(os.path.abspath(sys.argv[0])))

  # Parse the arguments.
  args = BuildOptions()

  # Find a valid path to args.cctest (in case it doesn't begin with './').
  args.cctest = os.path.join('.', args.cctest)

  if not os.access(args.cctest, os.X_OK):
    print "'" + args.cctest + "' is not executable or does not exist."
    sys.exit(1)

  # List all matching tests.
  manifest = ReadManifest(args.filters)

  # Delete coverage data files.
  if args.coverage:
    status, output = util.getstatusoutput('find obj/coverage -name "*.gcda" -exec rm {} \;')

  # Run the tests.
  status = RunTests(manifest)
  EnsureNewLine()

  # Print coverage information.
  if args.coverage:
    cmd = 'tggcov -R summary_all,untested_functions_per_file obj/coverage/src/a64'
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    print(stdout)

  sys.exit(status)

