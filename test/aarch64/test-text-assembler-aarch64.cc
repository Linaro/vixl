// Copyright 2020, VIXL authors
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

#if __cplusplus >= 201703L

#include "test-text-assembler-aarch64.h"

namespace vixl {
namespace aarch64 {
namespace tasm {

TEST(arithmetic) {
  SETUP();

  __ Mov(x17, 0xc001c001c001c001);
  __ Mov(x2, 3);
  __ Sub(x1, x0, 0);
  __ Add(x1, x1, 10);
  __ Add(x1, x1, Operand(x2, LSL, 2));
  __ Add(x1, x1, x0);
  __ Orr(x1, x1, 2);
  __ Ret();

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64
  RUN();
#endif
  REASSEMBLE_AND_COMPARE();
}

TEST(sve_predicate_registers) {
  SETUP();

  __ Ptrue(p2.VnB());
  __ Ptrue(p3.VnB(), SVE_VL7);
  __ Ptrue(p4.VnS(), SVE_ALL);
  __ Ptrue(p5.VnS(), SVE_VL7);
  __ Not(p3.VnB(), p2.Zeroing(), p3.VnB());
  __ Ret();

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64
  RUN();
#endif
  REASSEMBLE_AND_COMPARE();
}
}
}
}  // namespace vixl::aarch64::tasm

#endif  // __cplusplus >= 201703L