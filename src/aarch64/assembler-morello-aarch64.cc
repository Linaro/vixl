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

#include "assembler-aarch64.h"

namespace vixl {
namespace aarch64 {

void Assembler::bx(ptrdiff_t offset_in_bytes) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(offset_in_bytes == kInstructionSize);
  USE(offset_in_bytes);
  Emit(BX);
}

void Assembler::bx(Label* label) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // Link the label. Binding the label won't update the instruction because it
  // only supports an offset of 4, but it will check that the ISA and offset are
  // correct.
  ptrdiff_t offset = LinkAndGetByteOffsetTo(label);
  USE(offset);
  if (label->IsBound()) {
    // This occurs if BindToOffset binds the label ahead of the branch.
    VIXL_ASSERT(offset == kInstructionSize);
  } else {
    VIXL_ASSERT(offset == 0);
  }
  // `bx` unconditionally switches ISA.
  label->EnsureISA(vixl::aarch64::ExchangeISA(GetISA()));
  bx(kInstructionSize);
}

}  // namespace aarch64
}  // namespace vixl
