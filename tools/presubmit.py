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
  return result.parse_args()


def CleanBuildSystem():
  status, output = util.getstatusoutput('scons mode=release --clean')
  if status != 0: util.abort('Failed to clean in release mode.')
  status, output = util.getstatusoutput('scons mode=debug --clean')
  if status != 0: util.abort('Failed to clean in debug mode.')


class Test:
  def __init__(self, name, command, get_summary):
    self.name = name
    self.command = command
    self.get_summary = get_summary
    self.output = 'NOT RUN'
    self.status = 'NOT RUN'
    self.summary = 'NOT RUN'

  def Run(self):
    retcode, self.output = util.getstatusoutput(self.command)
    self.status = 'PASS' if retcode == 0 else 'FAILED'
    self.summary = self.get_summary(self.output)

  def PrintOutcome(self):
    print(('%s :'.ljust(20 - len(self.name)) + '%s\n' + ' ' * 18 + '%s')
          %(self.name, self.status, self.summary))

  def PrintOutput(self):
    print('\n\n### OUTPUT of ' + self.name)
    print(self.output)


class Tester:
  def __init__(self):
    self.tests = []

  def AddTest(self, test):
    self.tests.append(test)

  def RunAll(self, verbose):
    for test in self.tests:
      test.Run()
      test.PrintOutcome()

    if verbose:
      for test in self.tests:
        test.PrintOutput()


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
    lint_args = '--filter=-,+' + ',+'.join(CPP_LINTER_RULES) + ' '
    tracked_files = git.get_tracked_files().split()
    tracked_files = filter(util.is_cpp_filename, tracked_files)
    tracked_files = ' '.join(tracked_files)
    lint = Test('cpp lint',
                'cpplint.py ' + lint_args + tracked_files,
                util.last_line)
    tester.AddTest(lint)
  if not args.notest:
    release = Test('cctest release',
                   './tools/test.py --mode=release --jobs=%d' % args.jobs,
                   util.last_line)
    debug = Test('cctest debug',
                 './tools/test.py --mode=debug --jobs=%d' % args.jobs,
                 util.last_line)
    tester.AddTest(release)
    tester.AddTest(debug)

  if not args.noclean:
    CleanBuildSystem()

  tester.RunAll(args.verbose)

  # If the linter failed, print its output. We don't do the same for the debug
  # or release tests because they're easy to run by themselves. In verbose mode,
  # the output is printed automatically in tester.RunAll().
  if not args.nolint and lint.status == 'FAILED' and not args.verbose:
    lint.PrintOutput()

  if git.is_git_repository_root():
    untracked_files = git.get_untracked_files()
    if untracked_files:
      print '\nWARNING: The following files are untracked:'
      for f in untracked_files:
        print f.lstrip('?')

  # Restore original directory.
  os.chdir(original_dir)
