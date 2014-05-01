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

import util
import git

# Google's cpplint.py from depot_tools is the linter used here.
CPP_LINTER_RULES = '''
build/class
build/deprecated
build/endif_comment
build/forward_decl
build/include_order
build/printf_format
build/storage_class
legal/copyright
readability/boost
readability/braces
readability/casting
readability/constructors
readability/fn_size
readability/function
readability/multiline_comment
readability/multiline_string
readability/streams
readability/utf8
runtime/arrays
runtime/casting
runtime/deprecated_fn
runtime/explicit
runtime/int
runtime/memset
runtime/mutex
runtime/nonconf
runtime/printf
runtime/printf_format
runtime/references
runtime/rtti
runtime/sizeof
runtime/string
runtime/virtual
runtime/vlog
whitespace/blank_line
whitespace/braces
whitespace/comma
whitespace/comments
whitespace/end_of_line
whitespace/ending_newline
whitespace/indent
whitespace/labels
whitespace/line_length
whitespace/newline
whitespace/operators
whitespace/parens
whitespace/tab
whitespace/todo
'''.split()


def BuildOptions():
  result = argparse.ArgumentParser(description='Run the linter and unit tests.')
  result.add_argument('--verbose', '-v', action='store_true',
                      help='Print all tests output at the end.')
  result.add_argument('--notest', action='store_true',
                      help='Do not run tests. Run the linter only.')
  result.add_argument('--nolint', action='store_true',
                      help='Do not run the linter. Run the tests only.')
  result.add_argument('--noclean', action='store_true',
                      help='Do not clean before build.')
  result.add_argument('--jobs', '-j', metavar='N', type=int, default=1,
                      help='Allow N jobs at once.')
  sim_default = 'off' if platform.machine() == 'aarch64' else 'on'
  result.add_argument('--simulator', action='store', choices=['on', 'off'],
                      default=sim_default,
                      help='''Explicitly enable or disable the simulator. On
                      this system, the default is "''' + sim_default + '".')
  return result.parse_args()


def CleanBuildSystem():
  def clean(mode):
    if args.verbose: print('Cleaning ' + mode + ' mode cctest...')
    command = 'scons mode=%s simulator=%s target=cctest --clean' % \
              (mode, args.simulator)
    status, output = util.getstatusoutput(command)
    if status != 0:
      print(output)
      util.abort('Failed cleaning cctest: ' + command)
  clean('debug')
  clean('release')


def BuildRequiredObjects():
  def build(mode):
    if args.verbose: print('Building ' + mode + ' mode cctest...')
    command = 'scons mode=%s simulator=%s target=cctest -j%u' % \
              (mode, args.simulator, args.jobs)
    status, output = util.getstatusoutput(command)
    if status != 0:
      print(output)
      util.abort('Failed building cctest: ' + command)
  build('debug')
  build('release')


NOT_RUN = 'NOT RUN'
PASSED = 'PASSED'
FAILED = 'FAILED'


class Test:
  def __init__(self, name, command, get_summary = util.last_line):
    self.name = name
    self.command = command
    self.get_summary = get_summary
    self.output = NOT_RUN
    self.status = NOT_RUN
    self.summary = NOT_RUN

  def Run(self):
    if args.verbose: print('Running ' + self.name + '...')
    retcode, self.output = util.getstatusoutput(self.command)
    self.status = PASSED if retcode == 0 else FAILED
    self.summary = self.get_summary(self.output)

  def PrintOutcome(self):
    print(('%-26s : %s') % (self.name, self.summary))

  def PrintOutput(self):
    print('\n\n=== OUTPUT of: ' + self.command + '\n')
    print(self.output)


class Tester:
  def __init__(self):
    self.tests = []

  def AddTest(self, test):
    self.tests.append(test)

  def RunAll(self):
    result = PASSED
    for test in self.tests:
      test.Run()
      if test.status != PASSED: result = FAILED
      test.PrintOutcome()
    print('Presubmit tests ' + result + '.')

  def PrintFailedTestOutput(self):
    for test in self.tests:
      if test.status == FAILED:
        test.PrintOutput();


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
    CPP_EXT_REGEXP = re.compile('\.(cc|h)$')
    SIM_TRACES_REGEXP = re.compile('test-simulator-traces-a64\.h$')
    def is_linter_input(filename):
      # Don't lint the simulator traces file; it takes a very long time to check
      # and it's (mostly) generated automatically anyway.
      if SIM_TRACES_REGEXP.search(filename): return False
      # Otherwise, lint all C++ files.
      return CPP_EXT_REGEXP.search(filename) != None

    lint_args = '--filter=-,+' + ',+'.join(CPP_LINTER_RULES) + ' '
    tracked_files = git.get_tracked_files().split()
    tracked_files = filter(is_linter_input, tracked_files)
    tracked_files = ' '.join(tracked_files)
    lint = Test('cpp lint', 'cpplint.py ' + lint_args + tracked_files)
    tester.AddTest(lint)

  if not args.notest:
    if not args.noclean:
      CleanBuildSystem()
    BuildRequiredObjects()

    def command(*test_args):
      if args.verbose:
        return 'tools/test.py --verbose ' + ' '.join(test_args)
      else:
        return 'tools/test.py ' + ' '.join(test_args)

    if args.simulator == 'on':
      tester.AddTest(Test('cctest release (debugger)',
                          command('--cctest=cctest_sim', '--debugger')))
      tester.AddTest(Test('cctest debug (debugger)',
                          command('--cctest=cctest_sim_g', '--debugger')))
      tester.AddTest(Test('cctest release (simulator)',
                          command('--cctest=cctest_sim')))
      tester.AddTest(Test('cctest debug (simulator)',
                          command('--cctest=cctest_sim_g')))
    else:
      tester.AddTest(Test('cctest release', command('--cctest=cctest')))
      tester.AddTest(Test('cctest debug', command('--cctest=cctest_g')))

  tester.RunAll()

  # If tests failed, print their output.
  tester.PrintFailedTestOutput()

  if git.is_git_repository_root():
    untracked_files = git.get_untracked_files()
    if untracked_files:
      print '\nWARNING: The following files are untracked:'
      for f in untracked_files:
        print f.lstrip('?')
