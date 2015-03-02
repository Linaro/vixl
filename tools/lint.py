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
import multiprocessing
import re
import subprocess
import sys

import git
import printer
import util


# Google's cpplint.py from depot_tools is the linter used here.
# These are positive rules, added to the set of rules that the linter checks.
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
  result = argparse.ArgumentParser(
      description =
      '''This tool lints the C++ files tracked by the git repository, and
      produces a summary of the errors found.''',
      # Print default values.
      formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  result.add_argument('--jobs', '-j', metavar='N', type=int, nargs='?',
                      default=1, const=multiprocessing.cpu_count(),
                      help='''Runs the tests using N jobs. If the option is set
                      but no value is provided, the script will use as many jobs
                      as it thinks useful.''')
  result.add_argument('--verbose', action='store_true',
                      help='Print verbose output.')
  return result.parse_args()



__lint_results_lock__ = multiprocessing.Lock()

# Returns the number of errors in the file linted.
def Lint(filename, lint_options, progress_prefix = '', verbose = False):
  command = ['cpplint.py', lint_options, filename]
  process = subprocess.Popen(command,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)

  # Use a lock to avoid mixing the output for different files.
  with __lint_results_lock__:
    # Process the output as the process is running, until it exits.
    LINT_ERROR_LINE_REGEXP = re.compile('\[[1-5]\]$')
    LINT_DONE_PROC_LINE_REGEXP = re.compile('Done processing')
    LINT_STATUS_LINE_REGEXP = re.compile('Total errors found')
    while True:
      retcode = process.poll()
      while True:
        line = process.stderr.readline()
        if line == '': break
        output_line = progress_prefix + line.rstrip('\r\n')

        if LINT_ERROR_LINE_REGEXP.search(line):
          printer.PrintOverwritableLine(output_line, verbose = verbose)
          printer.EnsureNewLine()
        elif LINT_DONE_PROC_LINE_REGEXP.search(line):
          printer.PrintOverwritableLine(output_line, verbose = verbose)
        elif LINT_STATUS_LINE_REGEXP.search(line):
          status_line = line

      if retcode != None: break;

    if retcode == 0:
      return 0

    # Return the number of errors in this file.
    res = re.search('\d+$', status_line)
    n_errors_str = res.string[res.start():res.end()]
    n_errors = int(n_errors_str)
    status_line = \
        progress_prefix + 'Total errors found in %s : %d' % (filename, n_errors)
    printer.PrintOverwritableLine(status_line, verbose = verbose)
    printer.EnsureNewLine()
    return n_errors


# The multiprocessing map_async function does not allow passing multiple
# arguments directly, so use a wrapper.
def LintWrapper(args):
  return Lint(*args)


# Returns the total number of errors found in the files linted.
def LintFiles(files, lint_args = CPP_LINTER_RULES, jobs = 1, verbose = False,
              progress_prefix = ''):
  lint_options = '--filter=-,+' + ',+'.join(lint_args)
  pool = multiprocessing.Pool(jobs)
  # The '.get(9999999)' is workaround to allow killing the test script with
  # ctrl+C from the shell. This bug is documented at
  # http://bugs.python.org/issue8296.
  tasks = [(f, lint_options, progress_prefix, verbose) for f in files]
  results = pool.map_async(LintWrapper, tasks).get(9999999)
  n_errors = sum(results)

  printer.PrintOverwritableLine(
      progress_prefix + 'Total errors found: %d' % n_errors)
  printer.EnsureNewLine()
  return n_errors


def IsCppLintAvailable():
    retcode, unused_output = util.getstatusoutput('which cpplint.py')
    return retcode == 0


CPP_EXT_REGEXP = re.compile('\.(cc|h)$')
SIM_TRACES_REGEXP = re.compile('trace-a64\.h$')
def is_linter_input(filename):
  # Don't lint the simulator traces file; it takes a very long time to check
  # and it's (mostly) generated automatically anyway.
  if SIM_TRACES_REGEXP.search(filename): return False
  # Otherwise, lint all C++ files.
  return CPP_EXT_REGEXP.search(filename) != None
default_tracked_files = git.get_tracked_files().split()
default_tracked_files = filter(is_linter_input, default_tracked_files)

if __name__ == '__main__':
  # Parse the arguments.
  args = BuildOptions()

  retcode = LintFiles(default_tracked_files,
                      jobs = args.jobs, verbose = args.verbose)
  sys.exit(retcode)
