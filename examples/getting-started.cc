// Copyright 2013, ARM Limited
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

#include "a64/simulator-a64.h"
#include "a64/macro-assembler-a64.h"

#define BUF_SIZE (4096)
#define __ masm->

using namespace vixl;

void GenerateDemoFunction(MacroAssembler *masm) {
  // uint64_t demo_function(uint64_t x)
  __ Ldr(x1, 0x1122334455667788);
  __ And(x0, x0, x1);
  __ Ret();
}


#ifndef TEST_EXAMPLES
int main() {
  byte assm_buf[BUF_SIZE];
  MacroAssembler masm(assm_buf, BUF_SIZE);
  Decoder decoder;
  Simulator simulator(&decoder);

  Label demo_function;
  masm.Bind(&demo_function);
  GenerateDemoFunction(&masm);
  masm.FinalizeCode();

  simulator.set_xreg(0, 0x8899aabbccddeeff);
  simulator.RunFrom(demo_function.target());
  printf("x0 = %" PRIx64 "\n", simulator.xreg(0));

  return 0;
}
#endif
