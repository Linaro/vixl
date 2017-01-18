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

import os

# These paths describe the structure of the repository.
dir_tools              = os.path.dirname(os.path.realpath(__file__))
dir_root               = os.path.abspath(os.path.join(dir_tools, '..'))
dir_build              = os.path.join(dir_root, 'obj')
dir_build_latest       = os.path.join(dir_build, 'latest')
dir_src_vixl           = os.path.join(dir_root, 'src')
dir_tests              = os.path.join(dir_root, 'test')
dir_aarch64_benchmarks = os.path.join(dir_root, 'benchmarks', 'aarch64')
dir_aarch32_benchmarks = os.path.join(dir_root, 'benchmarks', 'aarch32')
dir_aarch64_examples   = os.path.join(dir_root, 'examples', 'aarch64')
dir_aarch32_examples   = os.path.join(dir_root, 'examples', 'aarch32')
dir_aarch64_traces     = os.path.join(dir_tests, 'aarch64', 'traces')
dir_aarch32_traces     = os.path.join(dir_tests, 'aarch32', 'traces')

# The following set of options are tested in all combinations. The order is
# important; the _first_ option in each list is used as the basis for the
# independently-tested options below.

# The full list of available build modes.
build_options_modes = ['debug', 'release']
# The list of C++ standard to test for.
tested_cpp_standards = ['c++98', 'c++11']
# The list of compilers tested.
tested_compilers = ['clang++', 'g++']

# The following sets of options are tested independently, and are not combined
# with one another. Each one in turn is appended to the first combination of the
# options above.

# The list of target arch/isa options to test with. Do not list 'all' as an
# option since it is the default.
build_options_target = ['a32', 't32', 'a32,t32', 'a64', 'a64,a32', 'a64,t32']
# Negative testing is off by default, so do not list 'off' as an option.
build_options_negative_testing = ['on']
