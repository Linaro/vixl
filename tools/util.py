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

import glob
import os
import re
import shlex
import subprocess
import sys


def ListCCFilesWithoutExt(path):
  return map(lambda x : os.path.splitext(os.path.basename(x))[0],
             glob.glob(os.path.join(path, '*.cc')))


def abort(message):
  print('ABORTING: ' + message)
  sys.exit(1)


# Emulate python3 subprocess.getstatusoutput.
def getstatusoutput(command, shell=False):
  try:
    args = shlex.split(command)
    output = subprocess.check_output(args, stderr=subprocess.STDOUT, shell=shell)
    return 0, output.rstrip('\n')
  except subprocess.CalledProcessError as e:
    return e.returncode, e.output.rstrip('\n')


def IsCommandAvailable(command):
    retcode, unused_output = getstatusoutput('which %s' % command)
    return retcode == 0


def ensure_dir(path_name):
  if not os.path.exists(path_name):
    os.makedirs(path_name)


# Check that the specified program is available.
def require_program(program_name):
  rc, out = getstatusoutput('which %s' % program_name)
  if rc != 0:
    print('ERROR: The required program %s was not found.' % program_name)
    sys.exit(rc)

def relrealpath(path, start=os.getcwd()):
  return os.path.relpath(os.path.realpath(path), start)

# Query the target architecture of the compiler. The 'target' architecture of
# the compiler used to build VIXL is considered to be the 'host' architecture of
# VIXL itself.
def GetHostArch(cxx):
  # Instruct the compiler to dump all its preprocessor macros.
  dump = subprocess.Popen([cxx, '-E', '-dM', '-'], stdin=subprocess.PIPE,
                          stdout=subprocess.PIPE)
  out, _ = dump.communicate()
  macro_list = [
    # Extract the macro name.
    match.group(1)
    for match in [
      # Capture macro name.
      re.search('#define (.*?) 1', macro)
      for macro in out.split('\n')
    ]
    # Filter out non-matches.
    if match
  ]
  if "__x86_64__" in macro_list:
    return "x86_64"
  elif "__arm__" in macro_list:
    return "aarch32"
  elif "__aarch64__" in macro_list:
    return "aarch64"
  else:
    raise Exception("Unsupported archtecture")
