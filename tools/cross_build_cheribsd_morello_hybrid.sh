#!/bin/sh

# Copyright 2020, VIXL authors
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

# Cross-build VIXL for cheribuild's "cheribsd-morello-hybrid" target.
#
# https://github.com/CTSRD-CHERI/cheribuild

if [ "$#" -lt 1 ]; then
  echo "Usage: tools/cross_build_cheribsd_morello_hybrid.sh <cheri-root> [scons arguments ...]"
  echo ""
  echo "By default, cheribuild puts <cheri-root> at '~/cheri'."
  exit 1
fi

CHERI=$1
shift

CLANG_PREFIX=$CHERI/output/morello-sdk/bin/aarch64-unknown-freebsd-
SYSROOT=$CHERI/output/morello-sdk/sysroot-morello-hybrid

export CCFLAGS="--sysroot=$SYSROOT -iwithsysroot/usr/include/c++/v1 -target aarch64-unknown-freebsd13 -march=morello"
export LINKFLAGS="$CCFLAGS"

export CXX=${CLANG_PREFIX}clang++
export AR=${CLANG_PREFIX}ar
export RANLIB=${CLANG_PREFIX}ranlib
export CC=${CLANG_PREFIX}clang
export LD=${CLANG_PREFIX}ld

OK=1
if [ ! -x "`which $CXX`" ]; then
  echo "Error: $CXX does not exist or is not executable."
  OK=0
fi
if [ ! -x "`which $AR`" ]; then
  echo "Error: $AR does not exist or is not executable."
  OK=0
fi
if [ ! -x "`which $RANLIB`" ]; then
  echo "Error: $RANLIB does not exist or is not executable."
  OK=0
fi
if [ ! -x "`which $CC`" ]; then
  echo "Error: $CC does not exist or is not executable."
  OK=0
fi
if [ ! -x "`which $LD`" ]; then
  echo "Error: $LD does not exist or is not executable."
  OK=0
fi
if [ $OK -ne 1 ]; then
  exit 1
fi

scons $@
