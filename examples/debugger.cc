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

// This is an interactive example, not to be used for testing.
#ifndef TEST_EXAMPLES

#define BUF_SIZE (4096)
#define __ masm->

// The aim is to let the user "play" with the debugger. Brk will trigger the
// debugger shell.
void GenerateBreak(MacroAssembler* masm) {
  Label hop;
  __ Brk();
  __ Nop();
  __ B(&hop);
  __ Nop();
  __ Bind(&hop);
  __ Mov(x1, 123);
  __ Mov(x2, 456);
  __ Add(x0, x1, x2);
  __ Ret();
}


int main(void) {
  // Create and initialize the assembler and the debugger.
  byte assm_buf[BUF_SIZE];
  MacroAssembler masm(assm_buf, BUF_SIZE);
  Decoder decoder;
  Debugger debugger(&decoder);

  // Generate the code for the example function.
  Label start;
  masm.Bind(&start);
  GenerateBreak(&masm);
  masm.FinalizeCode();

  // Run the example function.
  debugger.RunFrom(start.target());
  printf("Debugger example run\n");

  return 0;
}
#endif
