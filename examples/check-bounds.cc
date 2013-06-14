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

void GenerateCheckBounds(MacroAssembler* masm) {
  // uint64_t check_bounds(uint64_t value, uint64_t low, uint64_t high)
  // Argument locations:
  //   value -> x0
  //   low   -> x1
  //   high  -> x2

  // First we compare 'value' with the 'low' bound. If x1 <= x0 the N flag will
  // be cleared. This configuration can be checked with the 'pl' condition.
  __ Cmp(x0, x1);

  // Now we will compare 'value' and 'high' (x0 and x2) but only if the 'pl'
  // condition is verified. If the condition is not verified, we will clear
  // all the flags except the carry one (C flag).
  __ Ccmp(x0, x2, CFlag, pl);

  // We set x0 to 1 only if the 'ls' condition is satisfied.
  // 'ls' performs the following test: !(C==1 && Z==0). If the previous
  // comparison has been skipped we have C==1 and Z==0, so the 'ls' test
  // will fail and x0 will be set to 0.
  // Otherwise if the previous comparison occurred, x0 will be set to 1
  // only if x0 is less than or equal to x2.
  __ Cset(x0, ls);

  __ Ret();
}


#ifndef TEST_EXAMPLES
void run_function(Simulator *simulator, Label *function,
                  uint64_t value, uint64_t low, uint64_t high) {
  simulator->set_xreg(0, value);
  simulator->set_xreg(1, low);
  simulator->set_xreg(2, high);

  simulator->RunFrom(function->target());
  printf("%ld %s between %ld and %ld\n", value,
         simulator->xreg(0) ? "is" : "is not",
         low, high);

  simulator->ResetState();
}

int main(void) {
  // Create and initialize the assembler and the simulator.
  byte assm_buf[BUF_SIZE];
  MacroAssembler masm(assm_buf, BUF_SIZE);
  Decoder decoder;
  Simulator simulator(&decoder);

  // Generate the code for the example function.
  Label check_bounds;
  masm.Bind(&check_bounds);
  GenerateCheckBounds(&masm);
  masm.FinalizeCode();

  // Run the example function.
  run_function(&simulator, &check_bounds, 546, 50, 1000);
  run_function(&simulator, &check_bounds, 62, 100, 200);
  run_function(&simulator, &check_bounds, 200, 100, 200);

  return 0;
}
#endif
