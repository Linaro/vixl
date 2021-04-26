// Copyright 2021, VIXL authors
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

#if __cplusplus >= 201703L

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "test-runner.h"
#include "test-text-assembler-aarch64.h"

#define FUZZ_TEST(name) TEST_(AARCH64_REASM_FUZZ_##name)

namespace vixl {
namespace aarch64 {
namespace tasm {

static void FuzzReassemble(int step_size, int offset, int shift) {
  TextAssembler tasm;
  CodeBuffer* tasm_cb = tasm.GetBuffer();
  tasm.GetCPUFeatures()->Combine(CPUFeatures::All());

  Reassembler reasm(&tasm);
  Decoder decoder;
  Instruction buffer[kInstructionSize];
  decoder.AppendVisitor(&reasm);

  for (uint64_t i = offset << shift; i < (UINT64_C(1) << 32); i += step_size) {
    CodeBufferCheckScope guard(tasm.AsAssemblerBase(), kInstructionSize * 70);
    buffer->SetInstructionBits(static_cast<uint32_t>(i));
    decoder.Decode(buffer);

    if (reasm.err_flag) {
      std::cerr << "Error: unallocated instruction" << std::endl;
      std::cerr << "Instruction: " << reasm.str_instr << std::endl;
    } else if (tasm.GetLineError()) {
      std::cerr << "Error: text assembler parsing error" << std::endl;
      std::cerr << "Instruction: " << reasm.str_instr << std::endl;
    } else if (memcmp(buffer,
                      tasm_cb->GetStartAddress<const byte*>(),
                      kInstructionSize) != 0) {
      std::cerr << "Error: buffer mismatch" << std::endl;
      std::cerr << "Instruction: " << reasm.str_instr << std::endl;
      std::cerr << "Generated encoding: " << std::hex
                << *tasm_cb->GetStartAddress<uint32_t*>() << std::endl;
      std::cerr << "Expected encoding: " << std::hex << static_cast<uint32_t>(i)
                << std::endl;
    } else {
      std::cout << "Instruction: " << reasm.str_instr << std::endl;
    }
    std::cerr << std::endl;
    tasm_cb->Reset();
  }
}

// Number of shards used to split fuzz tests. This value isn't used in the macro
// below, so if you change this, ensure more FUZZ_SHARD instances are
// instantiated.
static const int kShardCount = 16;

// Test approximately 0.2% of the instruction space for the text assembler.
// Multiply the step size by the number of shards issued.
static const int kDisasmStep = 500 * kShardCount + 1;

// Shift the offset argument into the top-level opcode bits, which helps to
// spread the fuzz coverage across instruction classes.
static const int kOpFieldShift = 25;

#define FUZZ_SHARD(step, i, shift) \
  FUZZ_TEST(i) { FuzzReassemble(step, i, shift); }

FUZZ_SHARD(kDisasmStep, 0, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 1, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 2, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 3, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 4, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 5, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 6, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 7, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 8, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 9, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 10, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 11, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 12, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 13, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 14, kOpFieldShift)
FUZZ_SHARD(kDisasmStep, 15, kOpFieldShift)

}  // tasm
}  // namespace aarch64
}  // namespace vixl

#endif  // cpp version check