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

#include "examples.h"

#define BUF_SIZE (4096)
#define __ masm->

void GenerateSwap4(MacroAssembler* masm) {
  // VIXL's macro assembler provides some functions to manipulate the stack.
  // This example shows some of these functions.
  __ Claim(16);
  __ Poke(x0, 0);
  __ Poke(x1, 8);
  __ Push(x3, x2);

  __ Pop(x1, x0);
  __ Peek(x3, 0);
  __ Peek(x2, 8);
  __ Drop(16);

  __ Ret();
}


#ifndef TEST_EXAMPLES
int main(void) {
  // Create and initialize the assembler and the simulator.
  byte assm_buf[BUF_SIZE];
  MacroAssembler masm(assm_buf, BUF_SIZE);
  Decoder decoder;
  Simulator simulator(&decoder);

  // Generate the code for the example function.
  Label swap4;
  masm.Bind(&swap4);
  GenerateSwap4(&masm);
  masm.FinalizeCode();

  // Run the example function.
  simulator.set_xreg(0, 0x1111111111111111);
  simulator.set_xreg(1, 0x2222222222222222);
  simulator.set_xreg(2, 0x3333333333333333);
  simulator.set_xreg(3, 0x4444444444444444);

  printf("Before swap4:\n"
         "x0 = 0x%" PRIx64 "\n"
         "x1 = 0x%" PRIx64 "\n"
         "x2 = 0x%" PRIx64 "\n"
         "x3 = 0x%" PRIx64 "\n",
         simulator.xreg(0), simulator.xreg(1),
         simulator.xreg(2), simulator.xreg(3));

  simulator.RunFrom(swap4.target());

  printf("After swap4:\n"
         "x0 = 0x%" PRIx64 "\n"
         "x1 = 0x%" PRIx64 "\n"
         "x2 = 0x%" PRIx64 "\n"
         "x3 = 0x%" PRIx64 "\n",
         simulator.xreg(0), simulator.xreg(1),
         simulator.xreg(2), simulator.xreg(3));

  return 0;
}
#endif
