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

import os
import sys
import argparse
import re
import util

copyright_header = """// Copyright 2015, ARM Limited
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of ARM Limited nor the names of its contributors may be
//     used to endorse or promote products derived from this software without
//     specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""

master_trace_header = """
// This file holds the expected results for the instructions tested by
// test-simulator-a64.
//
// If you update input lists in test-simulator-inputs-a64.h, or add a new test
// to test-simulator-a64.cc, please run tools/generate_simulator_traces.py on a
// reference platform to regenerate this file and trace files.
//

#ifndef VIXL_TEST_SIMULATOR_TRACES_A64_H_
#define VIXL_TEST_SIMULATOR_TRACES_A64_H_

#include <stdint.h>

// To add a new simulator test to test-simulator-a64.cc, add dummy array(s)
// below to build test-simulator-a64 for reference platform. Then, run
// tools/generate_simulator_traces.py on a reference platform to regenerate this
// file and traces files.

// ---------------------------------------------------------------------
// ADD DUMMY ARRAYS FOR NEW SIMULATOR TEST HERE.
// ---------------------------------------------------------------------
const uint64_t kExpected_dummy_64[] = { 0 };
const size_t kExpectedCount_dummy_64 = 0;

const uint32_t kExpected_dummy_32[] = { 0 };
const size_t kExpectedCount_dummy_32 = 0;

// ---------------------------------------------------------------------
// Simulator test trace output files.
// ---------------------------------------------------------------------
"""
master_trace_footer = """
#endif  // VIXL_TEST_SIMULATOR_TRACES_A64_H_
"""

trace_header = """
// ---------------------------------------------------------------------
// This file is auto generated using tools/generate_simulator_traces.py.
//
// PLEASE DO NOT EDIT.
// ---------------------------------------------------------------------
"""

def BuildOptions(root):
  result = argparse.ArgumentParser(description = 'Simulator test generator.')
  result.add_argument('--runner', action='store', default=root+'/test-runner',
                      help='The test executable to run.')
  result.add_argument('--out', action='store',
                      default='test/test-simulator-traces-a64.h')
  return result.parse_args()


if __name__ == '__main__':
  # $ROOT/tools/generate_simulator_traces.py
  root_dir = os.path.dirname(os.path.dirname(os.path.abspath(sys.argv[0])))
  os.chdir(root_dir)

  args = BuildOptions(root_dir)

  # Run each simulator test (SIM_*) with the --sim_test_trace option, and use
  # the output to create the traces header (from --out). In addition, the
  # test-simulator-traces-a64.h file, the master trace file, which includes all
  # other trace files is generated.

  # Create master trace file.
  master_trace_f = open(args.out, 'w')
  master_trace_f.write(copyright_header)
  master_trace_f.write(master_trace_header)
  master_trace_f.write('\n\n')

  # Find the simulator tests.
  status, output = util.getstatusoutput(args.runner + ' --list')
  if status != 0: util.abort('Failed to list all tests')
  tests = filter(lambda t: 'SIM_' in t, output.split())
  tests.sort()

  for test in tests:
    # Run each test.
    print 'Generating trace for ' + test;
    cmd = ' '.join([args.runner, '--sim_test_trace', test])
    status, output = util.getstatusoutput(cmd)
    if status != 0: util.abort('Failed to run ' + cmd + '.')

    # Create a new trace header file.
    trace_filename = test.lower().replace('_', '-') + "-trace-a64.h"
    trace_f =  open("test/traces/a64/" + trace_filename, 'w')
    trace_f.write(copyright_header)
    trace_f.write(trace_header)
    trace_f.write('\n')
    trace_f.write("#ifndef VIXL_" + test.upper() + "_TRACE_A64_H_\n")
    trace_f.write("#define VIXL_" + test.upper() + "_TRACE_A64_H_\n")
    trace_f.write('\n')
    trace_f.write(output)
    trace_f.write('\n')
    trace_f.write('\n' + "#endif  // VIXL_" + test.upper() + "_TRACE_A64_H_" + '\n')
    trace_f.close()

    # Update master trace file.
    master_trace_f.write('#include \"traces/a64/' + trace_filename + '\"\n')

# Close master trace file.
  master_trace_f.write(master_trace_footer)
  master_trace_f.close()
  print 'Trace generation COMPLETE'
