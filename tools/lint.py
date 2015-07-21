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
import signal
import subprocess
import sys

import config
import git
import printer
import util


# Catch SIGINT to gracefully exit when ctrl+C is pressed.
def sigint_handler(signal, frame):
  sys.exit(1)
signal.signal(signal.SIGINT, sigint_handler)

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
  return result.parse_args()



__lint_results_lock__ = multiprocessing.Lock()

# Returns the number of errors in the file linted.
def Lint(filename, progress_prefix = ''):
  command = ['cpplint.py', filename]
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
          printer.PrintOverwritableLine(output_line,
                                        type = printer.LINE_TYPE_LINTER)
          printer.EnsureNewLine()
        elif LINT_DONE_PROC_LINE_REGEXP.search(line):
          printer.PrintOverwritableLine(output_line,
                                        type = printer.LINE_TYPE_LINTER)
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
    printer.PrintOverwritableLine(status_line, type = printer.LINE_TYPE_LINTER)
    printer.EnsureNewLine()
    return n_errors


# The multiprocessing map_async function does not allow passing multiple
# arguments directly, so use a wrapper.
def LintWrapper(args):
  # Run under a try-catch  to avoid flooding the output when the script is
  # interrupted from the keyboard with ctrl+C.
  try:
    return Lint(*args)
  except:
    sys.exit(1)


# Returns the total number of errors found in the files linted.
def LintFiles(files, jobs = 1, progress_prefix = ''):
  if not IsCppLintAvailable():
    print(
      printer.COLOUR_RED + \
      ("cpplint.py not found. Please ensure the depot"
       " tools are installed and in your PATH. See"
       " http://dev.chromium.org/developers/how-tos/install-depot-tools for"
       " details.") + \
      printer.NO_COLOUR)
    return -1

  pool = multiprocessing.Pool(jobs)
  # The '.get(9999999)' is workaround to allow killing the test script with
  # ctrl+C from the shell. This bug is documented at
  # http://bugs.python.org/issue8296.
  tasks = [(f, progress_prefix) for f in files]
  # Run under a try-catch  to avoid flooding the output when the script is
  # interrupted from the keyboard with ctrl+C.
  try:
    results = pool.map_async(LintWrapper, tasks).get(9999999)
    pool.close()
    pool.join()
  except KeyboardInterrupt:
    pool.terminate()
    sys.exit(1)
  n_errors = sum(results)

  printer.PrintOverwritableLine(
      progress_prefix + 'Total errors found: %d' % n_errors)
  printer.EnsureNewLine()
  return n_errors


def IsCppLintAvailable():
    retcode, unused_output = util.getstatusoutput('which cpplint.py')
    return retcode == 0


CPP_EXT_REGEXP = re.compile('\.(cc|h)$')
def is_linter_input(filename):
  # lint all C++ files.
  return CPP_EXT_REGEXP.search(filename) != None

def GetDefaultTrackedFiles():
  if git.is_git_repository_root(config.dir_root):
    default_tracked_files = git.get_tracked_files().split()
    default_tracked_files = filter(is_linter_input, default_tracked_files)
    return 0, default_tracked_files
  else:
    printer.Print(printer.COLOUR_ORANGE + 'WARNING: This script is not run ' \
                  'from its Git repository. The linter will not run.' + \
                  printer.NO_COLOUR)
    return 1, []

if __name__ == '__main__':
  # Parse the arguments.
  args = BuildOptions()

  retcode, default_tracked_files = GetDefaultTrackedFiles()
  if retcode:
    sys.exit(retcode)
  retcode = LintFiles(default_tracked_files,
                      jobs = args.jobs)
  sys.exit(retcode)
