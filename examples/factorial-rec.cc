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

void GenerateFactorialRec(MacroAssembler* masm) {
  // uint64_t factorial_rec(uint64_t n)
  // Argument location:
  //   n -> x0

  Label entry, input_is_zero;

  __ Bind(&entry);
  // Check for the stopping condition: the input number is null.
  __ Cbz(x0, &input_is_zero);

  __ Mov(x1, x0);
  __ Sub(x0, x0, 1);
  __ Push(x1, lr);
  __ Bl(&entry);    // Recursive call factorial_rec(n - 1).
  __ Pop(lr, x1);
  __ Mul(x0, x0, x1);
  __ Ret();

  __ Bind(&input_is_zero);
  __ Mov(x0, 1);
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
  Label factorial_rec;
  masm.Bind(&factorial_rec);
  GenerateFactorialRec(&masm);
  masm.FinalizeCode();

  // Run the example function.
  uint64_t input_val = 16;
  simulator.set_xreg(0, input_val);
  simulator.RunFrom(factorial_rec.target());
  printf("factorial(%ld) = %ld\n", input_val, simulator.xreg(0));

  return 0;
}
#endif
