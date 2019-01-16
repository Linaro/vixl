// Copyright 2019, VIXL authors
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


#include <cstdio>
#include <cstring>
#include <string>

#include "test-runner.h"

#include "aarch64/disasm-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"
#include "test-disasm-aarch64.h"

namespace vixl {
namespace aarch64 {

TEST(sve) {
  SETUP();

  // TODO: Replace these tests when the disassembler is more capable.
  COMPARE_PREFIX(asrr(z0.VnB(), p7.Merging(), z0.VnB(), z1.VnB()),
                 "asrr z0.b, p7/m, z0.b, z1.b");
  COMPARE_PREFIX(fcmeq(p6.VnD(), p7.Zeroing(), z0.VnD(), z1.VnD()),
                 "fcmeq p6.d, p7/z, z0.d, z1.d");
  COMPARE_PREFIX(mla(z0.VnB(), p7.Merging(), z0.VnB(), z1.VnB()),
                 "mla z0.b, p7/m, z0.b, z1.b");
  COMPARE_PREFIX(mla(z1.VnS(), p7.Merging(), z1.VnS(), z0.VnS()),
                 "mla z1.s, p7/m, z1.s, z0.s");
  COMPARE_PREFIX(asr(z1.VnB(), z0.VnB()), "asr z1.<T>, z0.<T>, #<const>");
  COMPARE_PREFIX(lsl(z0.VnH(), z1.VnH()), "lsl z0.<T>, z1.<T>, #<const>");
  COMPARE_PREFIX(lsr(z1.VnS(), z0.VnS()), "lsr z1.<T>, z0.<T>, #<const>");
  COMPARE_PREFIX(and_(p6.VnB(), p7.Zeroing(), p6.VnB(), p7.VnB()),
                 "and p6.b, p7/z, p6.b, p7.b");
  COMPARE_PREFIX(rev(p7.VnB(), p6.VnB()), "rev p7.b, p6.b");
  COMPARE_PREFIX(splice(z0.VnH(), p7, z0.VnH(), z1.VnH()),
                 "splice z0.h, p7, z0.h, z1.h");
  COMPARE_PREFIX(fnmad(z0.VnD(), p6.Merging(), z1.VnD(), z0.VnD()),
                 "fnmad z0.d, p6/m, z1.d, z0.d");

  CLEANUP();
}

}  // namespace aarch64
}  // namespace vixl
