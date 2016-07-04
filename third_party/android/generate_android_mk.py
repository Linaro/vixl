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
import glob
import os

def get_source_files(dir_root, path):
  sources = glob.glob(os.path.join(dir_root, path))
  sources = map(lambda p : os.path.relpath(p, dir_root), sources)
  sources.sort()
  return sources

dir_android = os.path.dirname(os.path.realpath(__file__))
dir_root = os.path.join(dir_android, '..', '..')

target_mk_default = os.path.abspath(os.path.join(dir_root, 'Android.mk'))


parser = argparse.ArgumentParser(
  description = \
    'Generate an `Android.mk` to compile VIXL within Android.',
  formatter_class=argparse.ArgumentDefaultsHelpFormatter
)
parser.add_argument('-o', '--output',
                    default=os.path.relpath(target_mk_default, os.getcwd()),
                    help='Target file.')
args = parser.parse_args()


common_sources = get_source_files(dir_root, os.path.join('src', '*.cc'))
aarch32_sources = get_source_files(dir_root, os.path.join('src', 'aarch32', '*.cc'))
aarch64_sources = get_source_files(dir_root, os.path.join('src', 'aarch64', '*.cc'))

test_common_sources = get_source_files(dir_root, os.path.join('test', '*.cc'))
test_aarch64_sources = get_source_files(dir_root, os.path.join('test', 'aarch64', '*.cc'))
test_aarch32_sources = get_source_files(dir_root, os.path.join('test', 'aarch32', '*.cc'))
test_sources = test_common_sources + test_aarch32_sources + test_aarch64_sources

test_sources.sort()

android_mk_template = os.path.join(dir_android, 'Android.mk.template')
with open(android_mk_template, 'r') as template_file:
  template = template_file.read()


template = template.format(vixl_common=' \\\n  '.join(common_sources),
                           vixl_aarch64_sources=' \\\n  '.join(aarch64_sources),
                           vixl_aarch32_sources=' \\\n  '.join(aarch32_sources),
                           vixl_test_files=' \\\n  '.join(test_sources))


with open(args.output, 'w') as android_mk:
  android_mk.write(template)

