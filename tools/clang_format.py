#!/usr/bin/env python2.7

# Copyright 2016, VIXL authors
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
import fnmatch
import multiprocessing
import os
import re
import signal
import subprocess
import sys
import tempfile

import config
import git
import printer
import util

CLANG_FORMAT_VERSION_MAJOR = 3
CLANG_FORMAT_VERSION_MINOR = 8

DEFAULT_CLANG_FORMAT = \
    'clang-format-{}.{}'.format(CLANG_FORMAT_VERSION_MAJOR,
                                CLANG_FORMAT_VERSION_MINOR)

is_output_redirected = not sys.stdout.isatty()

# Catch SIGINT to gracefully exit when ctrl+C is pressed.
def sigint_handler(signal, frame):
  sys.exit(1)
signal.signal(signal.SIGINT, sigint_handler)

def BuildOptions():
  parser = argparse.ArgumentParser(
    description = '''This tool runs `clang-format` on C++ files.
    If no files are provided on the command-line, all C++ source files in `src`,
    `sample`, and `benchmarks` are processed.
    When available, `colordiff` is automatically used to clour the output.''',
    # Print default values.
    formatter_class = argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument('files', nargs = '*')
  parser.add_argument('--clang-format', default=DEFAULT_CLANG_FORMAT,
                      help='Path to clang-format.')
  parser.add_argument('--in-place', '-i',
                      action = 'store_true', default = False,
                      help = 'Edit files in place.')
  parser.add_argument('--jobs', '-j', metavar = 'N', type = int, nargs = '?',
                      default = multiprocessing.cpu_count(),
                      const = multiprocessing.cpu_count(),
                      help = '''Runs the tests using N jobs. If the option is set
                      but no value is provided, the script will use as many jobs
                      as it thinks useful.''')
  return parser.parse_args()


def ClangFormatIsAvailable(clang_format):
  if not util.IsCommandAvailable(clang_format):
    return False
  version = subprocess.check_output([clang_format, '-version'])
  m = re.search("^clang-format version (\d)\.(\d)\.\d.*$",
                version.decode(), re.M)
  major, minor = m.groups()
  return int(major) == CLANG_FORMAT_VERSION_MAJOR and \
      int(minor) == CLANG_FORMAT_VERSION_MINOR


# Returns 0 if the file is correctly formatted, or 1 otherwise.
def ClangFormat(filename, clang_format, in_place = False, progress_prefix = ''):
  rc = 0
  printer.PrintOverwritableLine('Processing %s' % filename,
                                type = printer.LINE_TYPE_LINTER)

  cmd_format = [clang_format, filename]
  temp_file, temp_file_name = tempfile.mkstemp(prefix = 'clang_format_')
  cmd_format_string = '$ ' + ' '.join(cmd_format) + ' > %s' % temp_file_name
  p_format = subprocess.Popen(cmd_format,
                              stdout = temp_file, stderr = subprocess.STDOUT)

  rc += p_format.wait()

  cmd_diff = ['diff', '--unified', filename, temp_file_name]
  cmd_diff_string = '$ ' + ' '.join(cmd_diff)
  p_diff = subprocess.Popen(cmd_diff,
                            stdout = subprocess.PIPE, stderr = subprocess.STDOUT)

  if util.IsCommandAvailable('colordiff') and not is_output_redirected:
    p_colordiff = subprocess.Popen(
            ['colordiff', '--unified'],
            stdin = p_diff.stdout,
            stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
    out, unused = p_colordiff.communicate()
  else:
    out, unused = p_diff.communicate()

  rc += p_diff.wait()

  if in_place:
      cmd_format = [clang_format, '-i', filename]
      p_format = subprocess.Popen(cmd_format,
                                  stdout=temp_file, stderr=subprocess.STDOUT)

  if rc != 0:
    printer.Print('Incorrectly formatted file: ' + filename + '\n' + \
                  cmd_format_string + '\n' + \
                  cmd_diff_string + '\n' + \
                  out)

  os.remove(temp_file_name)

  return 0 if rc == 0 else 1


# The multiprocessing map_async function does not allow passing multiple
# arguments directly, so use a wrapper.
def ClangFormatWrapper(args):
  # Run under a try-catch  to avoid flooding the output when the script is
  # interrupted from the keyboard with ctrl+C.
  try:
    return ClangFormat(*args)
  except:
    sys.exit(1)


# Returns the total number of files incorrectly formatted.
def ClangFormatFiles(files, clang_format, in_place = False, jobs = 1,
                     progress_prefix = ''):
  if not ClangFormatIsAvailable(clang_format):
    error_message = "`{}` version {}.{} not found. Please ensure it " \
                    "is installed, in your PATH and the correct version." \
                    .format(clang_format,
                            CLANG_FORMAT_VERSION_MAJOR,
                            CLANG_FORMAT_VERSION_MINOR)
    print(printer.COLOUR_RED + error_message + printer.NO_COLOUR)
    return -1

  pool = multiprocessing.Pool(jobs)
  # The '.get(9999999)' is workaround to allow killing the test script with
  # ctrl+C from the shell. This bug is documented at
  # http://bugs.python.org/issue8296.
  tasks = [(f, clang_format, in_place, progress_prefix) for f in files]
  # Run under a try-catch  to avoid flooding the output when the script is
  # interrupted from the keyboard with ctrl+C.
  try:
    results = pool.map_async(ClangFormatWrapper, tasks).get(9999999)
    pool.close()
    pool.join()
  except KeyboardInterrupt:
    pool.terminate()
    sys.exit(1)
  rc = sum(results)

  printer.PrintOverwritableLine(
      progress_prefix + '%d files are incorrectly formatted.' % rc,
      type = printer.LINE_TYPE_LINTER)
  printer.EnsureNewLine()
  return rc


def Find(path, filters = ['*'], excluded_dir = ""):
  files_found = []

  def NameMatchesAnyFilter(name, ff):
    for f in ff:
      if fnmatch.fnmatch(name, f):
        return True
    return False

  for root, dirs, files in os.walk(path):
    files_found += [
        os.path.join(root, fn)
        for fn in files
        # Include files which names match "filters".
        # Exclude files for which the base directory is "excluded_dir".
        if NameMatchesAnyFilter(os.path.relpath(fn), filters) and \
            not os.path.dirname(os.path.join(root, fn)).endswith(excluded_dir)
    ]
  return files_found


def GetCppSourceFilesToFormat():
  sources = []
  source_dirs = [config.dir_aarch32_benchmarks,
                 config.dir_aarch32_examples,
                 config.dir_aarch64_benchmarks,
                 config.dir_aarch64_examples,
                 config.dir_tests,
                 config.dir_src_vixl ]
  for directory in source_dirs:
    sources += Find(directory, ['*.h', '*.cc'], 'traces')
  return sources


if __name__ == '__main__':
  # Parse the arguments.
  args = BuildOptions()
  files = args.files
  if not files:
    files = GetCppSourceFilesToFormat()

  rc = ClangFormatFiles(files, clang_format = args.clang_format,
                        in_place = args.in_place, jobs = args.jobs)

  sys.exit(rc)
