#!/usr/bin/env python2.7

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

import argparse
import hashlib
import multiprocessing
import os
import pickle
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
  parser = argparse.ArgumentParser(
      description =
      '''This tool lints C++ files and produces a summary of the errors found.
      If no files are provided on the command-line, all C++ source files in the
      repository are processed.
      Results are cached to speed up the process.
      ''',
      # Print default values.
      formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument('files', nargs = '*')
  parser.add_argument('--jobs', '-j', metavar='N', type=int, nargs='?',
                      default=multiprocessing.cpu_count(),
                      const=multiprocessing.cpu_count(),
                      help='''Runs the tests using N jobs. If the option is set
                      but no value is provided, the script will use as many jobs
                      as it thinks useful.''')
  parser.add_argument('--no-cache',
                      action='store_true', default=False,
                      help='Do not use cached lint results.')
  return parser.parse_args()



__lint_results_lock__ = multiprocessing.Lock()

# Returns a tuple (filename, number of lint errors).
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
      return (filename, 0)

    # Return the number of errors in this file.
    res = re.search('\d+$', status_line)
    n_errors_str = res.string[res.start():res.end()]
    n_errors = int(n_errors_str)
    status_line = \
        progress_prefix + 'Total errors found in %s : %d' % (filename, n_errors)
    printer.PrintOverwritableLine(status_line, type = printer.LINE_TYPE_LINTER)
    printer.EnsureNewLine()

    return (filename, n_errors)


# The multiprocessing map_async function does not allow passing multiple
# arguments directly, so use a wrapper.
def LintWrapper(args):
  # Run under a try-catch  to avoid flooding the output when the script is
  # interrupted from the keyboard with ctrl+C.
  try:
    return Lint(*args)
  except:
    sys.exit(1)


def ShouldLint(filename, cached_results):
  filename = os.path.realpath(filename)
  if filename not in cached_results:
    return True
  with open(filename, 'rb') as f:
    file_hash = hashlib.md5(f.read()).hexdigest()
  return file_hash != cached_results[filename]


# Returns the total number of errors found in the files linted.
# `cached_results` must be a dictionary, with the format:
#     { 'filename': file_hash, 'other_filename': other_hash, ... }
# If not `None`, `cached_results` is used to avoid re-linting files, and new
# results are stored in it.
def LintFiles(files,
              jobs = 1,
              progress_prefix = '',
              cached_results = None):
  if not IsCppLintAvailable():
    print(
      printer.COLOUR_RED + \
      ("cpplint.py not found. Please ensure the depot"
       " tools are installed and in your PATH. See"
       " http://dev.chromium.org/developers/how-tos/install-depot-tools for"
       " details.") + \
      printer.NO_COLOUR)
    return -1

  # Filter out directories.
  files = filter(os.path.isfile, files)

  # Filter out files for which we have a cached correct result.
  if cached_results is not None and len(cached_results) != 0:
    n_input_files = len(files)
    files = filter(lambda f: ShouldLint(f, cached_results), files)
    n_skipped_files = n_input_files - len(files)
    if n_skipped_files != 0:
      printer.Print(
        progress_prefix +
        'Skipping %d correct files that were already processed.' %
        n_skipped_files)

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

  n_errors = sum(map(lambda (filename, errors): errors, results))

  if cached_results is not None:
    for filename, errors in results:
      if errors == 0:
        with open(filename, 'rb') as f:
          filename = os.path.realpath(filename)
          file_hash = hashlib.md5(f.read()).hexdigest()
          cached_results[filename] = file_hash


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


def GetDefaultFilesToLint():
  if git.is_git_repository_root(config.dir_root):
    default_tracked_files = git.get_tracked_files().split()
    default_tracked_files = filter(is_linter_input, default_tracked_files)
    return 0, default_tracked_files
  else:
    printer.Print(printer.COLOUR_ORANGE + 'WARNING: This script is not run ' \
                  'from its Git repository. The linter will not run.' + \
                  printer.NO_COLOUR)
    return 1, []


cached_results_pkl_filename = \
  os.path.join(config.dir_tools, '.cached_lint_results.pkl')


def ReadCachedResults():
  cached_results = {}
  if os.path.isfile(cached_results_pkl_filename):
    with open(cached_results_pkl_filename, 'rb') as pkl_file:
      cached_results = pickle.load(pkl_file)
  return cached_results


def CacheResults(results):
  with open(cached_results_pkl_filename, 'wb') as pkl_file:
    pickle.dump(results, pkl_file)


def RunLinter(files, jobs=1, progress_prefix='', cached=True):
  results = {} if not cached else ReadCachedResults()

  rc = LintFiles(files,
                 jobs=jobs,
                 progress_prefix=progress_prefix,
                 cached_results=results)

  CacheResults(results)
  return rc



if __name__ == '__main__':
  # Parse the arguments.
  args = BuildOptions()

  files = args.files
  if not files:
    retcode, files = GetDefaultFilesToLint()
    if retcode:
      sys.exit(retcode)

  cached = not args.no_cache
  retcode = RunLinter(files, jobs=args.jobs, cached=cached)

  sys.exit(retcode)
