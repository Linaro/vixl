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

#include "macro-assembler-aarch64.h"

namespace vixl {
namespace aarch64 {


void MacroAssembler::Insr(const ZRegister& zdn, uint64_t imm) {
  VIXL_ASSERT(allow_macro_instructions_);

  // TODO: It might be useful to add an overload, so that calls like
  // `Insr(zdn.VnB(), 0xffffffffffffffff)` can be distinguished from more
  // reasonable calls like `Insr(zdn.VnB(), -1)`. A naive overload (taking
  // int64_t) results in overload ambiguity. A template-based approach would
  // require C++11's std::is_signed.
  unsigned lane_size = zdn.GetLaneSizeInBits();
  VIXL_ASSERT(IsUintN(lane_size, imm) ||
              IsIntN(lane_size, RawbitsToInt64(imm)));

  if (imm == 0) {
    SingleEmissionCheckScope guard(this);
    insr(zdn, xzr);
    return;
  }

  UseScratchRegisterScope temps(this);
  VIXL_ASSERT(lane_size <= kXRegSize);
  Register scratch =
      (lane_size > kWRegSize) ? temps.AcquireX() : temps.AcquireW();
  // TODO: There are many cases where we could optimise immediates, such as by
  // detecting repeating patterns or FP immediates. We should optimise and
  // abstract this for use in other SVE mov-immediate-like macros.
  Mov(scratch, imm);

  SingleEmissionCheckScope guard(this);
  insr(zdn, scratch);
}


}  // namespace aarch64
}  // namespace vixl
