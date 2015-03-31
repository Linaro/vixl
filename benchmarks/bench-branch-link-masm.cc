// Copyright 2015, ARM Limited
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

#include "vixl/a64/macro-assembler-a64.h"
#include "vixl/a64/instructions-a64.h"
#include "vixl/globals.h"

using namespace vixl;

static const int kDefaultIterationsCount = 100000;

// This program focuses on the emission of branches and veneers.
int main(int argc, char* argv[]) {
  int iterations = 0;

  switch (argc) {
    case 1: iterations = kDefaultIterationsCount; break;
    case 2: iterations = atoi(argv[1]); break;
    default:
      printf("Usage: %s [#iterations]\n", argv[0]);
      exit(1);
  }

  size_t base_buf_size = iterations * 4 * kInstructionSize;
  MacroAssembler masm(base_buf_size);

#define __ masm.

  Label target_1, target_2, target_3, target_4;
  for (int i = 0; i < iterations; i++) {
    __ B(&target_1);
  }
  __ Bind(&target_1);
  for (int i = 0; i < iterations; i++) {
    __ B(eq, &target_2);
  }
  __ Bind(&target_2);
  for (int i = 0; i < iterations; i++) {
    __ Cbz(x2, &target_3);
  }
  __ Bind(&target_3);
  for (int i = 0; i < iterations; i++) {
    __ Tbz(x3, 2, &target_4);
  }
  __ Bind(&target_4);

  masm.FinalizeCode();

  return 0;
}
