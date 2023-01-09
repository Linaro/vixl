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

#include "cpu-features.h"
#include "examples.h"

using namespace vixl;
using namespace vixl::aarch64;

#define __ masm->

// Capinfo (*)()
void GenerateDDCInfo(MacroAssembler* masm) {
  __ Mrs(c0, DDC);
  GenerateNewCapinfo(masm, "DDC");
}

#ifndef TEST_EXAMPLES
int main(void) {
  CPUFeatures cpu_req(CPUFeatures::kMorello);
  MacroAssembler masm;
  masm.SetCPUFeatures(cpu_req);
#if VIXL_HOST_IS_MORELLO
  masm.SetISA(vixl::aarch64::ISA::Host);
#endif

  // Capability literal pools (used by `GenerateNewCapinfo`) are weakly
  // position-dependent, because capabilities must be 16-byte aligned.
  masm.SetFixedCodeAddressBits(kCRegSizeInBytesLog2);

  Label fn_label;
  masm.Bind(&fn_label);
  GenerateDDCInfo(&masm);
  masm.FinalizeCode();
  CodeBuffer* buffer = masm.GetBuffer();

  ptrdiff_t fn_offset = fn_label.GetLocation() + fn_label.GetInterworkOffset();
  auto fn = buffer->GetOffsetAddress<Capinfo (*)()>(fn_offset);

  if (CanRunNatively(cpu_req)) {
    buffer->SetExecutable();
    Capinfo info = fn();
    info.Print();
  }
}
#endif
