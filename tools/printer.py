#!/usr/bin/env python2.7

# Copyright 2014, ARM Limited
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


import sys
import time
import multiprocessing


__need_newline__ = multiprocessing.Value('i', 0)
__last_overwritable_line_length__ = multiprocessing.Value('i', 0)
__print_lock__ = multiprocessing.Lock()


# If the last printed character was not a newline, print one.
def EnsureNewLine():
  global __need_newline__

  if __need_newline__.value:
    __need_newline__.value = 0
    sys.stdout.write('\n')


# Like print, but insert a newline if necessary to avoid corrupting the status
# display (provided by UpdateProgress).
def Print(string):
  global __last_overwritable_line_length__

  EnsureNewLine()
  print string
  __last_overwritable_line_length__.value = 0


def PrintOverwritableLine(string, verbose = False):
  global __need_newline__
  global __last_overwritable_line_length__

  with __print_lock__:
    if verbose:
      # In verbose mode we do not overwrite previous lines.
      EnsureNewLine()
    else:
      # Otherwise, overwrite the previous line.
      sys.stdout.write('\r')

    sys.stdout.write(string)

    # Append spaces to hide the previous line.
    new_len = len(string)
    spaces = __last_overwritable_line_length__.value - new_len
    if spaces > 0:
      sys.stdout.write(' ' * spaces)
    __last_overwritable_line_length__.value = new_len

    # We haven't printed a newline, so any subsequent print output (with verbose
    # logs or error reports) will need to print one.
    __need_newline__.value = 1


# Display the run progress:
# [time| progress|+ passed|- failed]  Name
def UpdateProgress(start_time, passed, failed, count, verbose, name,
                   prefix = ''):
  minutes, seconds = divmod(time.time() - start_time, 60)
  progress = float(passed + failed) / count * 100
  passed_colour = '\x1b[32m' if passed != 0 else ''
  failed_colour = '\x1b[31m' if failed != 0 else ''
  indicator = '[%02d:%02d| %3d%%|'
  indicator += passed_colour + '+ %d\x1b[0m|'
  indicator += failed_colour + '- %d\x1b[0m]'

  progress_string = prefix
  progress_string += indicator % (minutes, seconds, progress, passed, failed)
  progress_string += '  ' + name

  PrintOverwritableLine(progress_string, verbose)
