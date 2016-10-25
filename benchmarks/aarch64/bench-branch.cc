// Copyright 2015, VIXL authors
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

#include <sys/time.h>
#include "globals-vixl.h"

#include "aarch64/instructions-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"

using namespace vixl;
using namespace vixl::aarch64;

static const int kDefaultInstructionCount = 100000;

// This program focuses on emitting branch instructions.
//
// This code will emit a given number of branch immediate to the next
// instructions in a fixed size buffer, looping over the buffer if necessary.
// This code therefore focuses on Emit and label binding/patching.
int main(int argc, char* argv[]) {
  int instructions = 0;

  switch (argc) {
    case 1:
      instructions = kDefaultInstructionCount;
      break;
    case 2:
      instructions = atoi(argv[1]);
      break;
    default:
      printf("Usage: %s [#instructions]\n", argv[0]);
      exit(1);
  }

  const int buffer_size = 256 * KBytes;
  const int buffer_instruction_count = buffer_size / kInstructionSize;
  timeval start;
  gettimeofday(&start, NULL);
  MacroAssembler masm(buffer_size);

#define __ masm.
  // We emit a branch to the next instruction.

  int rounds = instructions / buffer_instruction_count;
  for (int i = 0; i < rounds; ++i) {
    {
      ExactAssemblyScope scope(&masm, buffer_size);
      for (int j = 0; j < buffer_instruction_count; ++j) {
        Label target;
        __ b(&target);
        __ bind(&target);
      }
    }
    masm.Reset();
  }

  int remaining = instructions % buffer_instruction_count;
  {
    ExactAssemblyScope scope(&masm, remaining * kInstructionSize);
    for (int i = 0; i < remaining; ++i) {
      Label target;
      __ b(&target);
      __ bind(&target);
    }
  }

  masm.FinalizeCode();
  timeval end;
  gettimeofday(&end, NULL);
  double delta = (end.tv_sec - start.tv_sec) +
                 static_cast<double>(end.tv_usec - start.tv_usec) / 1000000;
  printf("A64: time for %d instructions: %gs\n", instructions, delta);

  return 0;
}
