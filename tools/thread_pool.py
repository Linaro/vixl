#!/usr/bin/env python3

# Copyright 2019, VIXL authors
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

import multiprocessing
import signal
import sys

# Catch SIGINT to gracefully exit when ctrl+C is pressed.
def SigIntHandler(signal, frame):
  sys.exit(1)

signal.signal(signal.SIGINT, SigIntHandler)

# This function can't run in parallel due to constraints from the
# multiprocessing module.
__run_tests_lock__ = multiprocessing.Lock()
def Multithread(function, list_of_args, num_threads=1, init_function=None):
  with __run_tests_lock__:
    if init_function:
      if not init_function():
        # Init failed: early exit
        return

    pool = multiprocessing.Pool(num_threads)
    # The '.get(9999999)' is a workaround to allow killing the test script with
    # ctrl+C from the shell. This bug is documented at
    # http://bugs.python.org/issue8296.
    pool.map_async(function, list_of_args).get(9999999)
    pool.close()
    pool.join()
