// Copyright 2016, ARM Limited
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

// -----------------------------------------------------------------------------
// This file is auto generated from the
// test/a32/config/template-assembler-a32.cc.in template file using
// tools/generate_tests.py.
//
// PLEASE DO NOT EDIT.
// -----------------------------------------------------------------------------

#include "test-runner.h"

#include "test-utils.h"
#include "test-utils-a32.h"

#include "a32/assembler-a32.h"
#include "a32/macro-assembler-a32.h"

#define BUF_SIZE (4096)

namespace vixl {
namespace aarch32 {

// List of instruction mnemonics.
#define FOREACH_INSTRUCTION(M) M(Movs)

// Values to be passed to the assembler to produce the instruction under test.
struct Operands {
  Condition cond;
  Register rd;
  Register rn;
  ShiftType shift;
  Register rs;
};

// This structure contains all data needed to test one specific
// instruction.
struct TestData {
  // The `operands` field represents what to pass to the assembler to
  // produce the instruction.
  Operands operands;
  // Optionally expect the MacroAssembler to have generated an extra
  // instruction. This is used when the instruction needs to be in an IT block.
  const char* expect_instruction_before;
  // Description of the operands, used for error reporting.
  const char* operands_description;
  // Unique identifier, used for generating traces.
  const char* identifier;
};

struct TestResult {
  size_t size;
  const byte* encoding;
};

// Each element of this array produce one instruction encoding.
static const TestData kTests[] = {
    {{al, r0, r0, LSL, r0}, "", "al r0 r0 LSL r0", "al_r0_r0_LSL_r0"},
    {{al, r0, r0, LSL, r1}, "", "al r0 r0 LSL r1", "al_r0_r0_LSL_r1"},
    {{al, r0, r0, LSL, r2}, "", "al r0 r0 LSL r2", "al_r0_r0_LSL_r2"},
    {{al, r0, r0, LSL, r3}, "", "al r0 r0 LSL r3", "al_r0_r0_LSL_r3"},
    {{al, r0, r0, LSL, r4}, "", "al r0 r0 LSL r4", "al_r0_r0_LSL_r4"},
    {{al, r0, r0, LSL, r5}, "", "al r0 r0 LSL r5", "al_r0_r0_LSL_r5"},
    {{al, r0, r0, LSL, r6}, "", "al r0 r0 LSL r6", "al_r0_r0_LSL_r6"},
    {{al, r0, r0, LSL, r7}, "", "al r0 r0 LSL r7", "al_r0_r0_LSL_r7"},
    {{al, r0, r0, LSR, r0}, "", "al r0 r0 LSR r0", "al_r0_r0_LSR_r0"},
    {{al, r0, r0, LSR, r1}, "", "al r0 r0 LSR r1", "al_r0_r0_LSR_r1"},
    {{al, r0, r0, LSR, r2}, "", "al r0 r0 LSR r2", "al_r0_r0_LSR_r2"},
    {{al, r0, r0, LSR, r3}, "", "al r0 r0 LSR r3", "al_r0_r0_LSR_r3"},
    {{al, r0, r0, LSR, r4}, "", "al r0 r0 LSR r4", "al_r0_r0_LSR_r4"},
    {{al, r0, r0, LSR, r5}, "", "al r0 r0 LSR r5", "al_r0_r0_LSR_r5"},
    {{al, r0, r0, LSR, r6}, "", "al r0 r0 LSR r6", "al_r0_r0_LSR_r6"},
    {{al, r0, r0, LSR, r7}, "", "al r0 r0 LSR r7", "al_r0_r0_LSR_r7"},
    {{al, r0, r0, ASR, r0}, "", "al r0 r0 ASR r0", "al_r0_r0_ASR_r0"},
    {{al, r0, r0, ASR, r1}, "", "al r0 r0 ASR r1", "al_r0_r0_ASR_r1"},
    {{al, r0, r0, ASR, r2}, "", "al r0 r0 ASR r2", "al_r0_r0_ASR_r2"},
    {{al, r0, r0, ASR, r3}, "", "al r0 r0 ASR r3", "al_r0_r0_ASR_r3"},
    {{al, r0, r0, ASR, r4}, "", "al r0 r0 ASR r4", "al_r0_r0_ASR_r4"},
    {{al, r0, r0, ASR, r5}, "", "al r0 r0 ASR r5", "al_r0_r0_ASR_r5"},
    {{al, r0, r0, ASR, r6}, "", "al r0 r0 ASR r6", "al_r0_r0_ASR_r6"},
    {{al, r0, r0, ASR, r7}, "", "al r0 r0 ASR r7", "al_r0_r0_ASR_r7"},
    {{al, r0, r0, ROR, r0}, "", "al r0 r0 ROR r0", "al_r0_r0_ROR_r0"},
    {{al, r0, r0, ROR, r1}, "", "al r0 r0 ROR r1", "al_r0_r0_ROR_r1"},
    {{al, r0, r0, ROR, r2}, "", "al r0 r0 ROR r2", "al_r0_r0_ROR_r2"},
    {{al, r0, r0, ROR, r3}, "", "al r0 r0 ROR r3", "al_r0_r0_ROR_r3"},
    {{al, r0, r0, ROR, r4}, "", "al r0 r0 ROR r4", "al_r0_r0_ROR_r4"},
    {{al, r0, r0, ROR, r5}, "", "al r0 r0 ROR r5", "al_r0_r0_ROR_r5"},
    {{al, r0, r0, ROR, r6}, "", "al r0 r0 ROR r6", "al_r0_r0_ROR_r6"},
    {{al, r0, r0, ROR, r7}, "", "al r0 r0 ROR r7", "al_r0_r0_ROR_r7"},
    {{al, r1, r1, LSL, r0}, "", "al r1 r1 LSL r0", "al_r1_r1_LSL_r0"},
    {{al, r1, r1, LSL, r1}, "", "al r1 r1 LSL r1", "al_r1_r1_LSL_r1"},
    {{al, r1, r1, LSL, r2}, "", "al r1 r1 LSL r2", "al_r1_r1_LSL_r2"},
    {{al, r1, r1, LSL, r3}, "", "al r1 r1 LSL r3", "al_r1_r1_LSL_r3"},
    {{al, r1, r1, LSL, r4}, "", "al r1 r1 LSL r4", "al_r1_r1_LSL_r4"},
    {{al, r1, r1, LSL, r5}, "", "al r1 r1 LSL r5", "al_r1_r1_LSL_r5"},
    {{al, r1, r1, LSL, r6}, "", "al r1 r1 LSL r6", "al_r1_r1_LSL_r6"},
    {{al, r1, r1, LSL, r7}, "", "al r1 r1 LSL r7", "al_r1_r1_LSL_r7"},
    {{al, r1, r1, LSR, r0}, "", "al r1 r1 LSR r0", "al_r1_r1_LSR_r0"},
    {{al, r1, r1, LSR, r1}, "", "al r1 r1 LSR r1", "al_r1_r1_LSR_r1"},
    {{al, r1, r1, LSR, r2}, "", "al r1 r1 LSR r2", "al_r1_r1_LSR_r2"},
    {{al, r1, r1, LSR, r3}, "", "al r1 r1 LSR r3", "al_r1_r1_LSR_r3"},
    {{al, r1, r1, LSR, r4}, "", "al r1 r1 LSR r4", "al_r1_r1_LSR_r4"},
    {{al, r1, r1, LSR, r5}, "", "al r1 r1 LSR r5", "al_r1_r1_LSR_r5"},
    {{al, r1, r1, LSR, r6}, "", "al r1 r1 LSR r6", "al_r1_r1_LSR_r6"},
    {{al, r1, r1, LSR, r7}, "", "al r1 r1 LSR r7", "al_r1_r1_LSR_r7"},
    {{al, r1, r1, ASR, r0}, "", "al r1 r1 ASR r0", "al_r1_r1_ASR_r0"},
    {{al, r1, r1, ASR, r1}, "", "al r1 r1 ASR r1", "al_r1_r1_ASR_r1"},
    {{al, r1, r1, ASR, r2}, "", "al r1 r1 ASR r2", "al_r1_r1_ASR_r2"},
    {{al, r1, r1, ASR, r3}, "", "al r1 r1 ASR r3", "al_r1_r1_ASR_r3"},
    {{al, r1, r1, ASR, r4}, "", "al r1 r1 ASR r4", "al_r1_r1_ASR_r4"},
    {{al, r1, r1, ASR, r5}, "", "al r1 r1 ASR r5", "al_r1_r1_ASR_r5"},
    {{al, r1, r1, ASR, r6}, "", "al r1 r1 ASR r6", "al_r1_r1_ASR_r6"},
    {{al, r1, r1, ASR, r7}, "", "al r1 r1 ASR r7", "al_r1_r1_ASR_r7"},
    {{al, r1, r1, ROR, r0}, "", "al r1 r1 ROR r0", "al_r1_r1_ROR_r0"},
    {{al, r1, r1, ROR, r1}, "", "al r1 r1 ROR r1", "al_r1_r1_ROR_r1"},
    {{al, r1, r1, ROR, r2}, "", "al r1 r1 ROR r2", "al_r1_r1_ROR_r2"},
    {{al, r1, r1, ROR, r3}, "", "al r1 r1 ROR r3", "al_r1_r1_ROR_r3"},
    {{al, r1, r1, ROR, r4}, "", "al r1 r1 ROR r4", "al_r1_r1_ROR_r4"},
    {{al, r1, r1, ROR, r5}, "", "al r1 r1 ROR r5", "al_r1_r1_ROR_r5"},
    {{al, r1, r1, ROR, r6}, "", "al r1 r1 ROR r6", "al_r1_r1_ROR_r6"},
    {{al, r1, r1, ROR, r7}, "", "al r1 r1 ROR r7", "al_r1_r1_ROR_r7"},
    {{al, r2, r2, LSL, r0}, "", "al r2 r2 LSL r0", "al_r2_r2_LSL_r0"},
    {{al, r2, r2, LSL, r1}, "", "al r2 r2 LSL r1", "al_r2_r2_LSL_r1"},
    {{al, r2, r2, LSL, r2}, "", "al r2 r2 LSL r2", "al_r2_r2_LSL_r2"},
    {{al, r2, r2, LSL, r3}, "", "al r2 r2 LSL r3", "al_r2_r2_LSL_r3"},
    {{al, r2, r2, LSL, r4}, "", "al r2 r2 LSL r4", "al_r2_r2_LSL_r4"},
    {{al, r2, r2, LSL, r5}, "", "al r2 r2 LSL r5", "al_r2_r2_LSL_r5"},
    {{al, r2, r2, LSL, r6}, "", "al r2 r2 LSL r6", "al_r2_r2_LSL_r6"},
    {{al, r2, r2, LSL, r7}, "", "al r2 r2 LSL r7", "al_r2_r2_LSL_r7"},
    {{al, r2, r2, LSR, r0}, "", "al r2 r2 LSR r0", "al_r2_r2_LSR_r0"},
    {{al, r2, r2, LSR, r1}, "", "al r2 r2 LSR r1", "al_r2_r2_LSR_r1"},
    {{al, r2, r2, LSR, r2}, "", "al r2 r2 LSR r2", "al_r2_r2_LSR_r2"},
    {{al, r2, r2, LSR, r3}, "", "al r2 r2 LSR r3", "al_r2_r2_LSR_r3"},
    {{al, r2, r2, LSR, r4}, "", "al r2 r2 LSR r4", "al_r2_r2_LSR_r4"},
    {{al, r2, r2, LSR, r5}, "", "al r2 r2 LSR r5", "al_r2_r2_LSR_r5"},
    {{al, r2, r2, LSR, r6}, "", "al r2 r2 LSR r6", "al_r2_r2_LSR_r6"},
    {{al, r2, r2, LSR, r7}, "", "al r2 r2 LSR r7", "al_r2_r2_LSR_r7"},
    {{al, r2, r2, ASR, r0}, "", "al r2 r2 ASR r0", "al_r2_r2_ASR_r0"},
    {{al, r2, r2, ASR, r1}, "", "al r2 r2 ASR r1", "al_r2_r2_ASR_r1"},
    {{al, r2, r2, ASR, r2}, "", "al r2 r2 ASR r2", "al_r2_r2_ASR_r2"},
    {{al, r2, r2, ASR, r3}, "", "al r2 r2 ASR r3", "al_r2_r2_ASR_r3"},
    {{al, r2, r2, ASR, r4}, "", "al r2 r2 ASR r4", "al_r2_r2_ASR_r4"},
    {{al, r2, r2, ASR, r5}, "", "al r2 r2 ASR r5", "al_r2_r2_ASR_r5"},
    {{al, r2, r2, ASR, r6}, "", "al r2 r2 ASR r6", "al_r2_r2_ASR_r6"},
    {{al, r2, r2, ASR, r7}, "", "al r2 r2 ASR r7", "al_r2_r2_ASR_r7"},
    {{al, r2, r2, ROR, r0}, "", "al r2 r2 ROR r0", "al_r2_r2_ROR_r0"},
    {{al, r2, r2, ROR, r1}, "", "al r2 r2 ROR r1", "al_r2_r2_ROR_r1"},
    {{al, r2, r2, ROR, r2}, "", "al r2 r2 ROR r2", "al_r2_r2_ROR_r2"},
    {{al, r2, r2, ROR, r3}, "", "al r2 r2 ROR r3", "al_r2_r2_ROR_r3"},
    {{al, r2, r2, ROR, r4}, "", "al r2 r2 ROR r4", "al_r2_r2_ROR_r4"},
    {{al, r2, r2, ROR, r5}, "", "al r2 r2 ROR r5", "al_r2_r2_ROR_r5"},
    {{al, r2, r2, ROR, r6}, "", "al r2 r2 ROR r6", "al_r2_r2_ROR_r6"},
    {{al, r2, r2, ROR, r7}, "", "al r2 r2 ROR r7", "al_r2_r2_ROR_r7"},
    {{al, r3, r3, LSL, r0}, "", "al r3 r3 LSL r0", "al_r3_r3_LSL_r0"},
    {{al, r3, r3, LSL, r1}, "", "al r3 r3 LSL r1", "al_r3_r3_LSL_r1"},
    {{al, r3, r3, LSL, r2}, "", "al r3 r3 LSL r2", "al_r3_r3_LSL_r2"},
    {{al, r3, r3, LSL, r3}, "", "al r3 r3 LSL r3", "al_r3_r3_LSL_r3"},
    {{al, r3, r3, LSL, r4}, "", "al r3 r3 LSL r4", "al_r3_r3_LSL_r4"},
    {{al, r3, r3, LSL, r5}, "", "al r3 r3 LSL r5", "al_r3_r3_LSL_r5"},
    {{al, r3, r3, LSL, r6}, "", "al r3 r3 LSL r6", "al_r3_r3_LSL_r6"},
    {{al, r3, r3, LSL, r7}, "", "al r3 r3 LSL r7", "al_r3_r3_LSL_r7"},
    {{al, r3, r3, LSR, r0}, "", "al r3 r3 LSR r0", "al_r3_r3_LSR_r0"},
    {{al, r3, r3, LSR, r1}, "", "al r3 r3 LSR r1", "al_r3_r3_LSR_r1"},
    {{al, r3, r3, LSR, r2}, "", "al r3 r3 LSR r2", "al_r3_r3_LSR_r2"},
    {{al, r3, r3, LSR, r3}, "", "al r3 r3 LSR r3", "al_r3_r3_LSR_r3"},
    {{al, r3, r3, LSR, r4}, "", "al r3 r3 LSR r4", "al_r3_r3_LSR_r4"},
    {{al, r3, r3, LSR, r5}, "", "al r3 r3 LSR r5", "al_r3_r3_LSR_r5"},
    {{al, r3, r3, LSR, r6}, "", "al r3 r3 LSR r6", "al_r3_r3_LSR_r6"},
    {{al, r3, r3, LSR, r7}, "", "al r3 r3 LSR r7", "al_r3_r3_LSR_r7"},
    {{al, r3, r3, ASR, r0}, "", "al r3 r3 ASR r0", "al_r3_r3_ASR_r0"},
    {{al, r3, r3, ASR, r1}, "", "al r3 r3 ASR r1", "al_r3_r3_ASR_r1"},
    {{al, r3, r3, ASR, r2}, "", "al r3 r3 ASR r2", "al_r3_r3_ASR_r2"},
    {{al, r3, r3, ASR, r3}, "", "al r3 r3 ASR r3", "al_r3_r3_ASR_r3"},
    {{al, r3, r3, ASR, r4}, "", "al r3 r3 ASR r4", "al_r3_r3_ASR_r4"},
    {{al, r3, r3, ASR, r5}, "", "al r3 r3 ASR r5", "al_r3_r3_ASR_r5"},
    {{al, r3, r3, ASR, r6}, "", "al r3 r3 ASR r6", "al_r3_r3_ASR_r6"},
    {{al, r3, r3, ASR, r7}, "", "al r3 r3 ASR r7", "al_r3_r3_ASR_r7"},
    {{al, r3, r3, ROR, r0}, "", "al r3 r3 ROR r0", "al_r3_r3_ROR_r0"},
    {{al, r3, r3, ROR, r1}, "", "al r3 r3 ROR r1", "al_r3_r3_ROR_r1"},
    {{al, r3, r3, ROR, r2}, "", "al r3 r3 ROR r2", "al_r3_r3_ROR_r2"},
    {{al, r3, r3, ROR, r3}, "", "al r3 r3 ROR r3", "al_r3_r3_ROR_r3"},
    {{al, r3, r3, ROR, r4}, "", "al r3 r3 ROR r4", "al_r3_r3_ROR_r4"},
    {{al, r3, r3, ROR, r5}, "", "al r3 r3 ROR r5", "al_r3_r3_ROR_r5"},
    {{al, r3, r3, ROR, r6}, "", "al r3 r3 ROR r6", "al_r3_r3_ROR_r6"},
    {{al, r3, r3, ROR, r7}, "", "al r3 r3 ROR r7", "al_r3_r3_ROR_r7"},
    {{al, r4, r4, LSL, r0}, "", "al r4 r4 LSL r0", "al_r4_r4_LSL_r0"},
    {{al, r4, r4, LSL, r1}, "", "al r4 r4 LSL r1", "al_r4_r4_LSL_r1"},
    {{al, r4, r4, LSL, r2}, "", "al r4 r4 LSL r2", "al_r4_r4_LSL_r2"},
    {{al, r4, r4, LSL, r3}, "", "al r4 r4 LSL r3", "al_r4_r4_LSL_r3"},
    {{al, r4, r4, LSL, r4}, "", "al r4 r4 LSL r4", "al_r4_r4_LSL_r4"},
    {{al, r4, r4, LSL, r5}, "", "al r4 r4 LSL r5", "al_r4_r4_LSL_r5"},
    {{al, r4, r4, LSL, r6}, "", "al r4 r4 LSL r6", "al_r4_r4_LSL_r6"},
    {{al, r4, r4, LSL, r7}, "", "al r4 r4 LSL r7", "al_r4_r4_LSL_r7"},
    {{al, r4, r4, LSR, r0}, "", "al r4 r4 LSR r0", "al_r4_r4_LSR_r0"},
    {{al, r4, r4, LSR, r1}, "", "al r4 r4 LSR r1", "al_r4_r4_LSR_r1"},
    {{al, r4, r4, LSR, r2}, "", "al r4 r4 LSR r2", "al_r4_r4_LSR_r2"},
    {{al, r4, r4, LSR, r3}, "", "al r4 r4 LSR r3", "al_r4_r4_LSR_r3"},
    {{al, r4, r4, LSR, r4}, "", "al r4 r4 LSR r4", "al_r4_r4_LSR_r4"},
    {{al, r4, r4, LSR, r5}, "", "al r4 r4 LSR r5", "al_r4_r4_LSR_r5"},
    {{al, r4, r4, LSR, r6}, "", "al r4 r4 LSR r6", "al_r4_r4_LSR_r6"},
    {{al, r4, r4, LSR, r7}, "", "al r4 r4 LSR r7", "al_r4_r4_LSR_r7"},
    {{al, r4, r4, ASR, r0}, "", "al r4 r4 ASR r0", "al_r4_r4_ASR_r0"},
    {{al, r4, r4, ASR, r1}, "", "al r4 r4 ASR r1", "al_r4_r4_ASR_r1"},
    {{al, r4, r4, ASR, r2}, "", "al r4 r4 ASR r2", "al_r4_r4_ASR_r2"},
    {{al, r4, r4, ASR, r3}, "", "al r4 r4 ASR r3", "al_r4_r4_ASR_r3"},
    {{al, r4, r4, ASR, r4}, "", "al r4 r4 ASR r4", "al_r4_r4_ASR_r4"},
    {{al, r4, r4, ASR, r5}, "", "al r4 r4 ASR r5", "al_r4_r4_ASR_r5"},
    {{al, r4, r4, ASR, r6}, "", "al r4 r4 ASR r6", "al_r4_r4_ASR_r6"},
    {{al, r4, r4, ASR, r7}, "", "al r4 r4 ASR r7", "al_r4_r4_ASR_r7"},
    {{al, r4, r4, ROR, r0}, "", "al r4 r4 ROR r0", "al_r4_r4_ROR_r0"},
    {{al, r4, r4, ROR, r1}, "", "al r4 r4 ROR r1", "al_r4_r4_ROR_r1"},
    {{al, r4, r4, ROR, r2}, "", "al r4 r4 ROR r2", "al_r4_r4_ROR_r2"},
    {{al, r4, r4, ROR, r3}, "", "al r4 r4 ROR r3", "al_r4_r4_ROR_r3"},
    {{al, r4, r4, ROR, r4}, "", "al r4 r4 ROR r4", "al_r4_r4_ROR_r4"},
    {{al, r4, r4, ROR, r5}, "", "al r4 r4 ROR r5", "al_r4_r4_ROR_r5"},
    {{al, r4, r4, ROR, r6}, "", "al r4 r4 ROR r6", "al_r4_r4_ROR_r6"},
    {{al, r4, r4, ROR, r7}, "", "al r4 r4 ROR r7", "al_r4_r4_ROR_r7"},
    {{al, r5, r5, LSL, r0}, "", "al r5 r5 LSL r0", "al_r5_r5_LSL_r0"},
    {{al, r5, r5, LSL, r1}, "", "al r5 r5 LSL r1", "al_r5_r5_LSL_r1"},
    {{al, r5, r5, LSL, r2}, "", "al r5 r5 LSL r2", "al_r5_r5_LSL_r2"},
    {{al, r5, r5, LSL, r3}, "", "al r5 r5 LSL r3", "al_r5_r5_LSL_r3"},
    {{al, r5, r5, LSL, r4}, "", "al r5 r5 LSL r4", "al_r5_r5_LSL_r4"},
    {{al, r5, r5, LSL, r5}, "", "al r5 r5 LSL r5", "al_r5_r5_LSL_r5"},
    {{al, r5, r5, LSL, r6}, "", "al r5 r5 LSL r6", "al_r5_r5_LSL_r6"},
    {{al, r5, r5, LSL, r7}, "", "al r5 r5 LSL r7", "al_r5_r5_LSL_r7"},
    {{al, r5, r5, LSR, r0}, "", "al r5 r5 LSR r0", "al_r5_r5_LSR_r0"},
    {{al, r5, r5, LSR, r1}, "", "al r5 r5 LSR r1", "al_r5_r5_LSR_r1"},
    {{al, r5, r5, LSR, r2}, "", "al r5 r5 LSR r2", "al_r5_r5_LSR_r2"},
    {{al, r5, r5, LSR, r3}, "", "al r5 r5 LSR r3", "al_r5_r5_LSR_r3"},
    {{al, r5, r5, LSR, r4}, "", "al r5 r5 LSR r4", "al_r5_r5_LSR_r4"},
    {{al, r5, r5, LSR, r5}, "", "al r5 r5 LSR r5", "al_r5_r5_LSR_r5"},
    {{al, r5, r5, LSR, r6}, "", "al r5 r5 LSR r6", "al_r5_r5_LSR_r6"},
    {{al, r5, r5, LSR, r7}, "", "al r5 r5 LSR r7", "al_r5_r5_LSR_r7"},
    {{al, r5, r5, ASR, r0}, "", "al r5 r5 ASR r0", "al_r5_r5_ASR_r0"},
    {{al, r5, r5, ASR, r1}, "", "al r5 r5 ASR r1", "al_r5_r5_ASR_r1"},
    {{al, r5, r5, ASR, r2}, "", "al r5 r5 ASR r2", "al_r5_r5_ASR_r2"},
    {{al, r5, r5, ASR, r3}, "", "al r5 r5 ASR r3", "al_r5_r5_ASR_r3"},
    {{al, r5, r5, ASR, r4}, "", "al r5 r5 ASR r4", "al_r5_r5_ASR_r4"},
    {{al, r5, r5, ASR, r5}, "", "al r5 r5 ASR r5", "al_r5_r5_ASR_r5"},
    {{al, r5, r5, ASR, r6}, "", "al r5 r5 ASR r6", "al_r5_r5_ASR_r6"},
    {{al, r5, r5, ASR, r7}, "", "al r5 r5 ASR r7", "al_r5_r5_ASR_r7"},
    {{al, r5, r5, ROR, r0}, "", "al r5 r5 ROR r0", "al_r5_r5_ROR_r0"},
    {{al, r5, r5, ROR, r1}, "", "al r5 r5 ROR r1", "al_r5_r5_ROR_r1"},
    {{al, r5, r5, ROR, r2}, "", "al r5 r5 ROR r2", "al_r5_r5_ROR_r2"},
    {{al, r5, r5, ROR, r3}, "", "al r5 r5 ROR r3", "al_r5_r5_ROR_r3"},
    {{al, r5, r5, ROR, r4}, "", "al r5 r5 ROR r4", "al_r5_r5_ROR_r4"},
    {{al, r5, r5, ROR, r5}, "", "al r5 r5 ROR r5", "al_r5_r5_ROR_r5"},
    {{al, r5, r5, ROR, r6}, "", "al r5 r5 ROR r6", "al_r5_r5_ROR_r6"},
    {{al, r5, r5, ROR, r7}, "", "al r5 r5 ROR r7", "al_r5_r5_ROR_r7"},
    {{al, r6, r6, LSL, r0}, "", "al r6 r6 LSL r0", "al_r6_r6_LSL_r0"},
    {{al, r6, r6, LSL, r1}, "", "al r6 r6 LSL r1", "al_r6_r6_LSL_r1"},
    {{al, r6, r6, LSL, r2}, "", "al r6 r6 LSL r2", "al_r6_r6_LSL_r2"},
    {{al, r6, r6, LSL, r3}, "", "al r6 r6 LSL r3", "al_r6_r6_LSL_r3"},
    {{al, r6, r6, LSL, r4}, "", "al r6 r6 LSL r4", "al_r6_r6_LSL_r4"},
    {{al, r6, r6, LSL, r5}, "", "al r6 r6 LSL r5", "al_r6_r6_LSL_r5"},
    {{al, r6, r6, LSL, r6}, "", "al r6 r6 LSL r6", "al_r6_r6_LSL_r6"},
    {{al, r6, r6, LSL, r7}, "", "al r6 r6 LSL r7", "al_r6_r6_LSL_r7"},
    {{al, r6, r6, LSR, r0}, "", "al r6 r6 LSR r0", "al_r6_r6_LSR_r0"},
    {{al, r6, r6, LSR, r1}, "", "al r6 r6 LSR r1", "al_r6_r6_LSR_r1"},
    {{al, r6, r6, LSR, r2}, "", "al r6 r6 LSR r2", "al_r6_r6_LSR_r2"},
    {{al, r6, r6, LSR, r3}, "", "al r6 r6 LSR r3", "al_r6_r6_LSR_r3"},
    {{al, r6, r6, LSR, r4}, "", "al r6 r6 LSR r4", "al_r6_r6_LSR_r4"},
    {{al, r6, r6, LSR, r5}, "", "al r6 r6 LSR r5", "al_r6_r6_LSR_r5"},
    {{al, r6, r6, LSR, r6}, "", "al r6 r6 LSR r6", "al_r6_r6_LSR_r6"},
    {{al, r6, r6, LSR, r7}, "", "al r6 r6 LSR r7", "al_r6_r6_LSR_r7"},
    {{al, r6, r6, ASR, r0}, "", "al r6 r6 ASR r0", "al_r6_r6_ASR_r0"},
    {{al, r6, r6, ASR, r1}, "", "al r6 r6 ASR r1", "al_r6_r6_ASR_r1"},
    {{al, r6, r6, ASR, r2}, "", "al r6 r6 ASR r2", "al_r6_r6_ASR_r2"},
    {{al, r6, r6, ASR, r3}, "", "al r6 r6 ASR r3", "al_r6_r6_ASR_r3"},
    {{al, r6, r6, ASR, r4}, "", "al r6 r6 ASR r4", "al_r6_r6_ASR_r4"},
    {{al, r6, r6, ASR, r5}, "", "al r6 r6 ASR r5", "al_r6_r6_ASR_r5"},
    {{al, r6, r6, ASR, r6}, "", "al r6 r6 ASR r6", "al_r6_r6_ASR_r6"},
    {{al, r6, r6, ASR, r7}, "", "al r6 r6 ASR r7", "al_r6_r6_ASR_r7"},
    {{al, r6, r6, ROR, r0}, "", "al r6 r6 ROR r0", "al_r6_r6_ROR_r0"},
    {{al, r6, r6, ROR, r1}, "", "al r6 r6 ROR r1", "al_r6_r6_ROR_r1"},
    {{al, r6, r6, ROR, r2}, "", "al r6 r6 ROR r2", "al_r6_r6_ROR_r2"},
    {{al, r6, r6, ROR, r3}, "", "al r6 r6 ROR r3", "al_r6_r6_ROR_r3"},
    {{al, r6, r6, ROR, r4}, "", "al r6 r6 ROR r4", "al_r6_r6_ROR_r4"},
    {{al, r6, r6, ROR, r5}, "", "al r6 r6 ROR r5", "al_r6_r6_ROR_r5"},
    {{al, r6, r6, ROR, r6}, "", "al r6 r6 ROR r6", "al_r6_r6_ROR_r6"},
    {{al, r6, r6, ROR, r7}, "", "al r6 r6 ROR r7", "al_r6_r6_ROR_r7"},
    {{al, r7, r7, LSL, r0}, "", "al r7 r7 LSL r0", "al_r7_r7_LSL_r0"},
    {{al, r7, r7, LSL, r1}, "", "al r7 r7 LSL r1", "al_r7_r7_LSL_r1"},
    {{al, r7, r7, LSL, r2}, "", "al r7 r7 LSL r2", "al_r7_r7_LSL_r2"},
    {{al, r7, r7, LSL, r3}, "", "al r7 r7 LSL r3", "al_r7_r7_LSL_r3"},
    {{al, r7, r7, LSL, r4}, "", "al r7 r7 LSL r4", "al_r7_r7_LSL_r4"},
    {{al, r7, r7, LSL, r5}, "", "al r7 r7 LSL r5", "al_r7_r7_LSL_r5"},
    {{al, r7, r7, LSL, r6}, "", "al r7 r7 LSL r6", "al_r7_r7_LSL_r6"},
    {{al, r7, r7, LSL, r7}, "", "al r7 r7 LSL r7", "al_r7_r7_LSL_r7"},
    {{al, r7, r7, LSR, r0}, "", "al r7 r7 LSR r0", "al_r7_r7_LSR_r0"},
    {{al, r7, r7, LSR, r1}, "", "al r7 r7 LSR r1", "al_r7_r7_LSR_r1"},
    {{al, r7, r7, LSR, r2}, "", "al r7 r7 LSR r2", "al_r7_r7_LSR_r2"},
    {{al, r7, r7, LSR, r3}, "", "al r7 r7 LSR r3", "al_r7_r7_LSR_r3"},
    {{al, r7, r7, LSR, r4}, "", "al r7 r7 LSR r4", "al_r7_r7_LSR_r4"},
    {{al, r7, r7, LSR, r5}, "", "al r7 r7 LSR r5", "al_r7_r7_LSR_r5"},
    {{al, r7, r7, LSR, r6}, "", "al r7 r7 LSR r6", "al_r7_r7_LSR_r6"},
    {{al, r7, r7, LSR, r7}, "", "al r7 r7 LSR r7", "al_r7_r7_LSR_r7"},
    {{al, r7, r7, ASR, r0}, "", "al r7 r7 ASR r0", "al_r7_r7_ASR_r0"},
    {{al, r7, r7, ASR, r1}, "", "al r7 r7 ASR r1", "al_r7_r7_ASR_r1"},
    {{al, r7, r7, ASR, r2}, "", "al r7 r7 ASR r2", "al_r7_r7_ASR_r2"},
    {{al, r7, r7, ASR, r3}, "", "al r7 r7 ASR r3", "al_r7_r7_ASR_r3"},
    {{al, r7, r7, ASR, r4}, "", "al r7 r7 ASR r4", "al_r7_r7_ASR_r4"},
    {{al, r7, r7, ASR, r5}, "", "al r7 r7 ASR r5", "al_r7_r7_ASR_r5"},
    {{al, r7, r7, ASR, r6}, "", "al r7 r7 ASR r6", "al_r7_r7_ASR_r6"},
    {{al, r7, r7, ASR, r7}, "", "al r7 r7 ASR r7", "al_r7_r7_ASR_r7"},
    {{al, r7, r7, ROR, r0}, "", "al r7 r7 ROR r0", "al_r7_r7_ROR_r0"},
    {{al, r7, r7, ROR, r1}, "", "al r7 r7 ROR r1", "al_r7_r7_ROR_r1"},
    {{al, r7, r7, ROR, r2}, "", "al r7 r7 ROR r2", "al_r7_r7_ROR_r2"},
    {{al, r7, r7, ROR, r3}, "", "al r7 r7 ROR r3", "al_r7_r7_ROR_r3"},
    {{al, r7, r7, ROR, r4}, "", "al r7 r7 ROR r4", "al_r7_r7_ROR_r4"},
    {{al, r7, r7, ROR, r5}, "", "al r7 r7 ROR r5", "al_r7_r7_ROR_r5"},
    {{al, r7, r7, ROR, r6}, "", "al r7 r7 ROR r6", "al_r7_r7_ROR_r6"},
    {{al, r7, r7, ROR, r7}, "", "al r7 r7 ROR r7", "al_r7_r7_ROR_r7"}};

// These headers each contain an array of `TestResult` with the reference output
// values. The reference arrays are names `kReference{mnemonic}`.
#include "a32/traces/assembler-cond-rd-operand-rn-shift-rs-t32-narrow-out-it-block-movs.h"

// The maximum number of errors to report in detail for each test.
static const unsigned kErrorReportLimit = 8;

typedef void (MacroAssembler::*Fn)(Condition cond, Register rd,
                                   const Operand& op);

static void TestHelper(Fn instruction, const char* mnemonic,
                       const TestResult reference[]) {
  unsigned total_error_count = 0;
  MacroAssembler masm(BUF_SIZE);

  masm.SetT32(true);

  for (unsigned i = 0; i < ARRAY_SIZE(kTests); i++) {
    // Values to pass to the macro-assembler.
    Condition cond = kTests[i].operands.cond;
    Register rd = kTests[i].operands.rd;
    Register rn = kTests[i].operands.rn;
    ShiftType shift = kTests[i].operands.shift;
    Register rs = kTests[i].operands.rs;
    Operand op(rn, shift, rs);

    uint32_t start = masm.GetCursorOffset();
    (masm.*instruction)(cond, rd, op);
    uint32_t end = masm.GetCursorOffset();

    const byte* result_ptr =
        masm.GetBuffer().GetOffsetAddress<const byte*>(start);
    uint32_t result_size = end - start;

    if (Test::generate_test_trace()) {
      // Print the result bytes.
      printf("static const byte kInstruction_%s_%s[] = {\n", mnemonic,
             kTests[i].identifier);
      for (uint32_t j = 0; j < result_size; j++) {
        if (j == 0) {
          printf("  0x%02" PRIx8, result_ptr[j]);
        } else {
          printf(", 0x%02" PRIx8, result_ptr[j]);
        }
      }
      // This comment is meant to be used by external tools to validate
      // the encoding. We can parse the comment to figure out what
      // instruction this corresponds to.
      printf(" // %s %s %s\n};\n", kTests[i].expect_instruction_before,
             mnemonic, kTests[i].operands_description);
    } else {
      // Check we've emitted the exact same encoding as present in the
      // trace file. Only print up to `kErrorReportLimit` errors.
      if (((result_size != reference[i].size) ||
           (memcmp(result_ptr, reference[i].encoding, reference[i].size) !=
            0)) &&
          (++total_error_count <= kErrorReportLimit)) {
        printf("Error when testing \"%s\" with operands \"%s\":\n", mnemonic,
               kTests[i].operands_description);
        printf("  Expected: ");
        for (uint32_t j = 0; j < reference[i].size; j++) {
          if (j == 0) {
            printf("0x%02" PRIx8, reference[i].encoding[j]);
          } else {
            printf(", 0x%02" PRIx8, reference[i].encoding[j]);
          }
        }
        printf("\n");
        printf("  Found:    ");
        for (uint32_t j = 0; j < result_size; j++) {
          if (j == 0) {
            printf("0x%02" PRIx8, result_ptr[j]);
          } else {
            printf(", 0x%02" PRIx8, result_ptr[j]);
          }
        }
        printf("\n");
      }
    }
  }

  masm.FinalizeCode();

  if (Test::generate_test_trace()) {
    // Finalize the trace file by writing the final `TestResult` array
    // which links all generated instruction encodings.
    printf("static const TestResult kReference%s[] = {\n", mnemonic);
    for (unsigned i = 0; i < ARRAY_SIZE(kTests); i++) {
      printf("  {\n");
      printf("    ARRAY_SIZE(kInstruction_%s_%s),\n", mnemonic,
             kTests[i].identifier);
      printf("    kInstruction_%s_%s,\n", mnemonic, kTests[i].identifier);
      printf("  },\n");
    }
    printf("};\n");
  } else {
    if (total_error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n",
             total_error_count - kErrorReportLimit);
    }
  }
}

// Instantiate tests for each instruction in the list.
#define TEST(mnemonic)                                                      \
  static void Test_##mnemonic() {                                           \
    TestHelper(&MacroAssembler::mnemonic, #mnemonic, kReference##mnemonic); \
  }                                                                         \
  static Test test_##mnemonic(                                              \
      "AARCH32_ASSEMBLER_COND_RD_OPERAND_RN_SHIFT_RS_T32_NARROW_OUT_IT_"    \
      "BLOCK_" #mnemonic,                                                   \
      &Test_##mnemonic);
FOREACH_INSTRUCTION(TEST)
#undef TEST

}  // aarch32
}  // vixl
