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

import os
import sys
import argparse
import re
import util

def BuildOptions(root):
  result = argparse.ArgumentParser(description = 'Simulator test generator.')
  result.add_argument('--cctest', action='store', default=root+'/cctest',
                      help='The cctest executable to run.')
  result.add_argument('--out', action='store',
                      default='test/test-simulator-traces-a64.h')
  return result.parse_args()


if __name__ == '__main__':
  # $ROOT/tools/generate_simulator_traces.py
  root_dir = os.path.dirname(os.path.dirname(os.path.abspath(sys.argv[0])))
  os.chdir(root_dir)

  args = BuildOptions(root_dir)

  # Run each simulator test (SIM_*) with the --sim_test_trace option, and use
  # the output to update the traces header (from --out). The existing traces are
  # wholly replaced, but some boilerplate code exists in the header, so we find
  # the start of the traces, truncate the file to that point, then add the new
  # (updated) trace data.

  # Find the output section of the traces file.
  marker = [
    '// ---------------------------------------------------------------------',
    '// Expected outputs.',
    '// Everything below this point is automatically generated.',
    '//',
    '// PLEASE DO NOT EDIT ANYTHING BELOW THIS COMMENT.',
    '// ---------------------------------------------------------------------',
    ]
  matched = 0
  f = open(args.out, 'r+')
  # Use readline (rather than for..in) so we can truncate at the right place.
  line = f.readline();
  while line:
    if line.strip() == marker[matched]:
      matched = matched + 1
      if matched == len(marker):
        f.truncate()
        break
    else:
      matched = 0
    line = f.readline()

  if matched != len(marker):
    util.abort('Failed to find output section in ' + args.out + '.')

  # Find the simulator tests.
  status, output = util.getstatusoutput(args.cctest + ' --list')
  if status != 0: util.abort('Failed to list all tests')
  tests = filter(lambda t: 'SIM_' in t, output.split())
  tests.sort()

  # Run each test.
  for test in tests:
    cmd = ' '.join([args.cctest, '--sim_test_trace', test])
    status, output = util.getstatusoutput(cmd)
    if status != 0: util.abort('Failed to run ' + cmd + '.')

    f.write('\n\n' + output)

  f.write('\n\n')
  f.close()
