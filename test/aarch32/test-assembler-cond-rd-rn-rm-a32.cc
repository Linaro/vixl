// Copyright 2016, VIXL authors
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
// test/aarch32/config/template-assembler-aarch32.cc.in template file using
// tools/generate_tests.py.
//
// PLEASE DO NOT EDIT.
// -----------------------------------------------------------------------------

#include "test-runner.h"

#include "test-utils.h"
#include "test-utils-aarch32.h"

#include "aarch32/assembler-aarch32.h"
#include "aarch32/macro-assembler-aarch32.h"

#define BUF_SIZE (4096)

namespace vixl {
namespace aarch32 {

// List of instruction mnemonics.
#define FOREACH_INSTRUCTION(M) \
  M(Mul)                       \
  M(Muls)                      \
  M(Qadd16)                    \
  M(Qadd8)                     \
  M(Qasx)                      \
  M(Qsax)                      \
  M(Qsub16)                    \
  M(Qsub8)                     \
  M(Sdiv)                      \
  M(Shadd16)                   \
  M(Shadd8)                    \
  M(Shasx)                     \
  M(Shsax)                     \
  M(Shsub16)                   \
  M(Shsub8)                    \
  M(Smmul)                     \
  M(Smmulr)                    \
  M(Smuad)                     \
  M(Smuadx)                    \
  M(Smulbb)                    \
  M(Smulbt)                    \
  M(Smultb)                    \
  M(Smultt)                    \
  M(Smulwb)                    \
  M(Smulwt)                    \
  M(Smusd)                     \
  M(Smusdx)                    \
  M(Udiv)                      \
  M(Uhadd16)                   \
  M(Uhadd8)                    \
  M(Uhasx)                     \
  M(Uhsax)                     \
  M(Uhsub16)                   \
  M(Uhsub8)                    \
  M(Uqadd16)                   \
  M(Uqadd8)                    \
  M(Uqasx)                     \
  M(Uqsax)                     \
  M(Uqsub16)                   \
  M(Uqsub8)                    \
  M(Usad8)                     \
  M(Sadd16)                    \
  M(Sadd8)                     \
  M(Sasx)                      \
  M(Sel)                       \
  M(Ssax)                      \
  M(Ssub16)                    \
  M(Ssub8)                     \
  M(Uadd16)                    \
  M(Uadd8)                     \
  M(Uasx)                      \
  M(Usax)                      \
  M(Usub16)                    \
  M(Usub8)                     \
  M(Qadd)                      \
  M(Qdadd)                     \
  M(Qdsub)                     \
  M(Qsub)

// Values to be passed to the assembler to produce the instruction under test.
struct Operands {
  Condition cond;
  Register rd;
  Register rn;
  Register rm;
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
    {{hi, r1, r9, r5}, "", "hi r1 r9 r5", "hi_r1_r9_r5"},
    {{pl, r8, r6, r2}, "", "pl r8 r6 r2", "pl_r8_r6_r2"},
    {{hi, r5, r8, r2}, "", "hi r5 r8 r2", "hi_r5_r8_r2"},
    {{vc, r9, r2, r7}, "", "vc r9 r2 r7", "vc_r9_r2_r7"},
    {{lt, r4, r6, r3}, "", "lt r4 r6 r3", "lt_r4_r6_r3"},
    {{le, r11, r6, r2}, "", "le r11 r6 r2", "le_r11_r6_r2"},
    {{cc, r8, r14, r4}, "", "cc r8 r14 r4", "cc_r8_r14_r4"},
    {{le, r5, r14, r6}, "", "le r5 r14 r6", "le_r5_r14_r6"},
    {{lt, r6, r1, r0}, "", "lt r6 r1 r0", "lt_r6_r1_r0"},
    {{lt, r5, r0, r9}, "", "lt r5 r0 r9", "lt_r5_r0_r9"},
    {{le, r8, r12, r7}, "", "le r8 r12 r7", "le_r8_r12_r7"},
    {{eq, r7, r14, r6}, "", "eq r7 r14 r6", "eq_r7_r14_r6"},
    {{cs, r7, r4, r6}, "", "cs r7 r4 r6", "cs_r7_r4_r6"},
    {{gt, r9, r6, r9}, "", "gt r9 r6 r9", "gt_r9_r6_r9"},
    {{ne, r13, r9, r1}, "", "ne r13 r9 r1", "ne_r13_r9_r1"},
    {{ge, r13, r1, r13}, "", "ge r13 r1 r13", "ge_r13_r1_r13"},
    {{ls, r8, r10, r2}, "", "ls r8 r10 r2", "ls_r8_r10_r2"},
    {{hi, r0, r13, r5}, "", "hi r0 r13 r5", "hi_r0_r13_r5"},
    {{pl, r13, r7, r8}, "", "pl r13 r7 r8", "pl_r13_r7_r8"},
    {{ge, r4, r13, r11}, "", "ge r4 r13 r11", "ge_r4_r13_r11"},
    {{cs, r5, r10, r5}, "", "cs r5 r10 r5", "cs_r5_r10_r5"},
    {{cs, r5, r4, r3}, "", "cs r5 r4 r3", "cs_r5_r4_r3"},
    {{ls, r6, r14, r8}, "", "ls r6 r14 r8", "ls_r6_r14_r8"},
    {{vs, r3, r8, r6}, "", "vs r3 r8 r6", "vs_r3_r8_r6"},
    {{vc, r7, r12, r3}, "", "vc r7 r12 r3", "vc_r7_r12_r3"},
    {{ge, r1, r4, r1}, "", "ge r1 r4 r1", "ge_r1_r4_r1"},
    {{cc, r4, r7, r10}, "", "cc r4 r7 r10", "cc_r4_r7_r10"},
    {{cc, r2, r0, r13}, "", "cc r2 r0 r13", "cc_r2_r0_r13"},
    {{vs, r9, r6, r8}, "", "vs r9 r6 r8", "vs_r9_r6_r8"},
    {{cs, r14, r11, r13}, "", "cs r14 r11 r13", "cs_r14_r11_r13"},
    {{pl, r5, r8, r4}, "", "pl r5 r8 r4", "pl_r5_r8_r4"},
    {{pl, r2, r3, r7}, "", "pl r2 r3 r7", "pl_r2_r3_r7"},
    {{cs, r7, r12, r14}, "", "cs r7 r12 r14", "cs_r7_r12_r14"},
    {{hi, r6, r6, r1}, "", "hi r6 r6 r1", "hi_r6_r6_r1"},
    {{cc, r6, r9, r6}, "", "cc r6 r9 r6", "cc_r6_r9_r6"},
    {{ne, r12, r12, r0}, "", "ne r12 r12 r0", "ne_r12_r12_r0"},
    {{cc, r9, r3, r8}, "", "cc r9 r3 r8", "cc_r9_r3_r8"},
    {{mi, r13, r6, r1}, "", "mi r13 r6 r1", "mi_r13_r6_r1"},
    {{lt, r4, r8, r6}, "", "lt r4 r8 r6", "lt_r4_r8_r6"},
    {{hi, r11, r5, r9}, "", "hi r11 r5 r9", "hi_r11_r5_r9"},
    {{cc, r6, r10, r6}, "", "cc r6 r10 r6", "cc_r6_r10_r6"},
    {{eq, r10, r10, r5}, "", "eq r10 r10 r5", "eq_r10_r10_r5"},
    {{al, r5, r4, r11}, "", "al r5 r4 r11", "al_r5_r4_r11"},
    {{pl, r11, r11, r2}, "", "pl r11 r11 r2", "pl_r11_r11_r2"},
    {{ls, r6, r14, r12}, "", "ls r6 r14 r12", "ls_r6_r14_r12"},
    {{vc, r7, r7, r2}, "", "vc r7 r7 r2", "vc_r7_r7_r2"},
    {{eq, r10, r8, r4}, "", "eq r10 r8 r4", "eq_r10_r8_r4"},
    {{al, r14, r7, r2}, "", "al r14 r7 r2", "al_r14_r7_r2"},
    {{cs, r3, r11, r10}, "", "cs r3 r11 r10", "cs_r3_r11_r10"},
    {{ls, r11, r4, r0}, "", "ls r11 r4 r0", "ls_r11_r4_r0"},
    {{hi, r11, r8, r9}, "", "hi r11 r8 r9", "hi_r11_r8_r9"},
    {{vs, r2, r14, r13}, "", "vs r2 r14 r13", "vs_r2_r14_r13"},
    {{al, r1, r13, r9}, "", "al r1 r13 r9", "al_r1_r13_r9"},
    {{eq, r3, r9, r13}, "", "eq r3 r9 r13", "eq_r3_r9_r13"},
    {{ge, r10, r3, r13}, "", "ge r10 r3 r13", "ge_r10_r3_r13"},
    {{pl, r8, r5, r10}, "", "pl r8 r5 r10", "pl_r8_r5_r10"},
    {{vc, r8, r11, r6}, "", "vc r8 r11 r6", "vc_r8_r11_r6"},
    {{eq, r0, r0, r5}, "", "eq r0 r0 r5", "eq_r0_r0_r5"},
    {{ne, r6, r5, r8}, "", "ne r6 r5 r8", "ne_r6_r5_r8"},
    {{hi, r5, r13, r3}, "", "hi r5 r13 r3", "hi_r5_r13_r3"},
    {{ne, r11, r14, r14}, "", "ne r11 r14 r14", "ne_r11_r14_r14"},
    {{mi, r1, r0, r6}, "", "mi r1 r0 r6", "mi_r1_r0_r6"},
    {{le, r14, r8, r2}, "", "le r14 r8 r2", "le_r14_r8_r2"},
    {{eq, r9, r6, r5}, "", "eq r9 r6 r5", "eq_r9_r6_r5"},
    {{eq, r11, r0, r13}, "", "eq r11 r0 r13", "eq_r11_r0_r13"},
    {{pl, r4, r5, r14}, "", "pl r4 r5 r14", "pl_r4_r5_r14"},
    {{cs, r13, r5, r13}, "", "cs r13 r5 r13", "cs_r13_r5_r13"},
    {{mi, r0, r13, r8}, "", "mi r0 r13 r8", "mi_r0_r13_r8"},
    {{lt, r2, r13, r3}, "", "lt r2 r13 r3", "lt_r2_r13_r3"},
    {{ls, r8, r1, r11}, "", "ls r8 r1 r11", "ls_r8_r1_r11"},
    {{vc, r14, r11, r8}, "", "vc r14 r11 r8", "vc_r14_r11_r8"},
    {{lt, r4, r13, r12}, "", "lt r4 r13 r12", "lt_r4_r13_r12"},
    {{eq, r2, r1, r14}, "", "eq r2 r1 r14", "eq_r2_r1_r14"},
    {{eq, r9, r4, r14}, "", "eq r9 r4 r14", "eq_r9_r4_r14"},
    {{hi, r10, r6, r13}, "", "hi r10 r6 r13", "hi_r10_r6_r13"},
    {{ge, r12, r9, r4}, "", "ge r12 r9 r4", "ge_r12_r9_r4"},
    {{le, r9, r11, r14}, "", "le r9 r11 r14", "le_r9_r11_r14"},
    {{ls, r0, r9, r5}, "", "ls r0 r9 r5", "ls_r0_r9_r5"},
    {{mi, r2, r3, r8}, "", "mi r2 r3 r8", "mi_r2_r3_r8"},
    {{ne, r14, r10, r14}, "", "ne r14 r10 r14", "ne_r14_r10_r14"},
    {{eq, r6, r2, r10}, "", "eq r6 r2 r10", "eq_r6_r2_r10"},
    {{lt, r11, r0, r12}, "", "lt r11 r0 r12", "lt_r11_r0_r12"},
    {{ne, r1, r12, r10}, "", "ne r1 r12 r10", "ne_r1_r12_r10"},
    {{cc, r1, r0, r2}, "", "cc r1 r0 r2", "cc_r1_r0_r2"},
    {{al, r5, r5, r7}, "", "al r5 r5 r7", "al_r5_r5_r7"},
    {{hi, r7, r13, r1}, "", "hi r7 r13 r1", "hi_r7_r13_r1"},
    {{cs, r4, r4, r9}, "", "cs r4 r4 r9", "cs_r4_r4_r9"},
    {{eq, r14, r4, r14}, "", "eq r14 r4 r14", "eq_r14_r4_r14"},
    {{vs, r10, r5, r14}, "", "vs r10 r5 r14", "vs_r10_r5_r14"},
    {{gt, r4, r3, r11}, "", "gt r4 r3 r11", "gt_r4_r3_r11"},
    {{ne, r14, r10, r12}, "", "ne r14 r10 r12", "ne_r14_r10_r12"},
    {{vs, r2, r11, r0}, "", "vs r2 r11 r0", "vs_r2_r11_r0"},
    {{ge, r5, r12, r7}, "", "ge r5 r12 r7", "ge_r5_r12_r7"},
    {{mi, r7, r14, r6}, "", "mi r7 r14 r6", "mi_r7_r14_r6"},
    {{gt, r8, r3, r8}, "", "gt r8 r3 r8", "gt_r8_r3_r8"},
    {{hi, r9, r14, r3}, "", "hi r9 r14 r3", "hi_r9_r14_r3"},
    {{vc, r2, r11, r2}, "", "vc r2 r11 r2", "vc_r2_r11_r2"},
    {{hi, r11, r7, r12}, "", "hi r11 r7 r12", "hi_r11_r7_r12"},
    {{cs, r6, r4, r11}, "", "cs r6 r4 r11", "cs_r6_r4_r11"},
    {{cs, r12, r5, r9}, "", "cs r12 r5 r9", "cs_r12_r5_r9"},
    {{ls, r5, r10, r5}, "", "ls r5 r10 r5", "ls_r5_r10_r5"},
    {{ls, r0, r9, r13}, "", "ls r0 r9 r13", "ls_r0_r9_r13"},
    {{lt, r3, r3, r5}, "", "lt r3 r3 r5", "lt_r3_r3_r5"},
    {{mi, r0, r12, r8}, "", "mi r0 r12 r8", "mi_r0_r12_r8"},
    {{pl, r3, r12, r12}, "", "pl r3 r12 r12", "pl_r3_r12_r12"},
    {{eq, r8, r12, r5}, "", "eq r8 r12 r5", "eq_r8_r12_r5"},
    {{cc, r7, r8, r1}, "", "cc r7 r8 r1", "cc_r7_r8_r1"},
    {{hi, r2, r13, r10}, "", "hi r2 r13 r10", "hi_r2_r13_r10"},
    {{al, r7, r10, r10}, "", "al r7 r10 r10", "al_r7_r10_r10"},
    {{vc, r1, r12, r2}, "", "vc r1 r12 r2", "vc_r1_r12_r2"},
    {{cc, r8, r5, r8}, "", "cc r8 r5 r8", "cc_r8_r5_r8"},
    {{ls, r3, r7, r9}, "", "ls r3 r7 r9", "ls_r3_r7_r9"},
    {{al, r8, r10, r8}, "", "al r8 r10 r8", "al_r8_r10_r8"},
    {{lt, r4, r12, r10}, "", "lt r4 r12 r10", "lt_r4_r12_r10"},
    {{ge, r10, r5, r11}, "", "ge r10 r5 r11", "ge_r10_r5_r11"},
    {{ls, r3, r14, r4}, "", "ls r3 r14 r4", "ls_r3_r14_r4"},
    {{hi, r3, r6, r12}, "", "hi r3 r6 r12", "hi_r3_r6_r12"},
    {{hi, r6, r0, r4}, "", "hi r6 r0 r4", "hi_r6_r0_r4"},
    {{al, r11, r6, r0}, "", "al r11 r6 r0", "al_r11_r6_r0"},
    {{mi, r3, r1, r9}, "", "mi r3 r1 r9", "mi_r3_r1_r9"},
    {{mi, r12, r13, r0}, "", "mi r12 r13 r0", "mi_r12_r13_r0"},
    {{le, r1, r2, r5}, "", "le r1 r2 r5", "le_r1_r2_r5"},
    {{hi, r4, r3, r14}, "", "hi r4 r3 r14", "hi_r4_r3_r14"},
    {{eq, r6, r11, r11}, "", "eq r6 r11 r11", "eq_r6_r11_r11"},
    {{cc, r14, r11, r14}, "", "cc r14 r11 r14", "cc_r14_r11_r14"},
    {{hi, r4, r10, r0}, "", "hi r4 r10 r0", "hi_r4_r10_r0"},
    {{cc, r7, r11, r1}, "", "cc r7 r11 r1", "cc_r7_r11_r1"},
    {{mi, r14, r6, r10}, "", "mi r14 r6 r10", "mi_r14_r6_r10"},
    {{eq, r2, r0, r11}, "", "eq r2 r0 r11", "eq_r2_r0_r11"},
    {{mi, r13, r5, r12}, "", "mi r13 r5 r12", "mi_r13_r5_r12"},
    {{eq, r2, r12, r5}, "", "eq r2 r12 r5", "eq_r2_r12_r5"},
    {{le, r12, r0, r2}, "", "le r12 r0 r2", "le_r12_r0_r2"},
    {{vc, r10, r10, r9}, "", "vc r10 r10 r9", "vc_r10_r10_r9"},
    {{ls, r11, r11, r8}, "", "ls r11 r11 r8", "ls_r11_r11_r8"},
    {{hi, r10, r11, r9}, "", "hi r10 r11 r9", "hi_r10_r11_r9"},
    {{vs, r7, r12, r14}, "", "vs r7 r12 r14", "vs_r7_r12_r14"},
    {{gt, r11, r14, r12}, "", "gt r11 r14 r12", "gt_r11_r14_r12"},
    {{vs, r0, r12, r8}, "", "vs r0 r12 r8", "vs_r0_r12_r8"},
    {{al, r0, r5, r7}, "", "al r0 r5 r7", "al_r0_r5_r7"},
    {{hi, r5, r13, r8}, "", "hi r5 r13 r8", "hi_r5_r13_r8"},
    {{le, r9, r9, r7}, "", "le r9 r9 r7", "le_r9_r9_r7"},
    {{cc, r4, r9, r5}, "", "cc r4 r9 r5", "cc_r4_r9_r5"},
    {{vs, r8, r1, r3}, "", "vs r8 r1 r3", "vs_r8_r1_r3"},
    {{cc, r0, r10, r12}, "", "cc r0 r10 r12", "cc_r0_r10_r12"},
    {{eq, r7, r14, r0}, "", "eq r7 r14 r0", "eq_r7_r14_r0"},
    {{vs, r12, r9, r11}, "", "vs r12 r9 r11", "vs_r12_r9_r11"},
    {{gt, r5, r9, r11}, "", "gt r5 r9 r11", "gt_r5_r9_r11"},
    {{cs, r14, r13, r7}, "", "cs r14 r13 r7", "cs_r14_r13_r7"},
    {{mi, r11, r3, r10}, "", "mi r11 r3 r10", "mi_r11_r3_r10"},
    {{hi, r11, r8, r12}, "", "hi r11 r8 r12", "hi_r11_r8_r12"},
    {{cs, r3, r8, r13}, "", "cs r3 r8 r13", "cs_r3_r8_r13"},
    {{pl, r10, r12, r6}, "", "pl r10 r12 r6", "pl_r10_r12_r6"},
    {{vc, r7, r3, r2}, "", "vc r7 r3 r2", "vc_r7_r3_r2"},
    {{mi, r9, r0, r8}, "", "mi r9 r0 r8", "mi_r9_r0_r8"},
    {{eq, r2, r13, r7}, "", "eq r2 r13 r7", "eq_r2_r13_r7"},
    {{ne, r2, r14, r0}, "", "ne r2 r14 r0", "ne_r2_r14_r0"},
    {{vs, r4, r10, r0}, "", "vs r4 r10 r0", "vs_r4_r10_r0"},
    {{ls, r0, r2, r2}, "", "ls r0 r2 r2", "ls_r0_r2_r2"},
    {{cc, r1, r6, r0}, "", "cc r1 r6 r0", "cc_r1_r6_r0"},
    {{lt, r12, r0, r8}, "", "lt r12 r0 r8", "lt_r12_r0_r8"},
    {{cc, r9, r3, r14}, "", "cc r9 r3 r14", "cc_r9_r3_r14"},
    {{vs, r7, r9, r1}, "", "vs r7 r9 r1", "vs_r7_r9_r1"},
    {{eq, r11, r9, r14}, "", "eq r11 r9 r14", "eq_r11_r9_r14"},
    {{pl, r6, r10, r4}, "", "pl r6 r10 r4", "pl_r6_r10_r4"},
    {{ne, r8, r5, r6}, "", "ne r8 r5 r6", "ne_r8_r5_r6"},
    {{cs, r0, r6, r2}, "", "cs r0 r6 r2", "cs_r0_r6_r2"},
    {{eq, r11, r12, r4}, "", "eq r11 r12 r4", "eq_r11_r12_r4"},
    {{lt, r14, r3, r14}, "", "lt r14 r3 r14", "lt_r14_r3_r14"},
    {{le, r7, r12, r14}, "", "le r7 r12 r14", "le_r7_r12_r14"},
    {{hi, r2, r9, r9}, "", "hi r2 r9 r9", "hi_r2_r9_r9"},
    {{ne, r8, r1, r0}, "", "ne r8 r1 r0", "ne_r8_r1_r0"},
    {{cc, r5, r11, r2}, "", "cc r5 r11 r2", "cc_r5_r11_r2"},
    {{hi, r0, r1, r2}, "", "hi r0 r1 r2", "hi_r0_r1_r2"},
    {{al, r4, r9, r4}, "", "al r4 r9 r4", "al_r4_r9_r4"},
    {{cs, r12, r7, r14}, "", "cs r12 r7 r14", "cs_r12_r7_r14"},
    {{cc, r4, r12, r10}, "", "cc r4 r12 r10", "cc_r4_r12_r10"},
    {{al, r3, r5, r10}, "", "al r3 r5 r10", "al_r3_r5_r10"},
    {{mi, r5, r3, r7}, "", "mi r5 r3 r7", "mi_r5_r3_r7"},
    {{ls, r10, r6, r2}, "", "ls r10 r6 r2", "ls_r10_r6_r2"},
    {{mi, r0, r12, r11}, "", "mi r0 r12 r11", "mi_r0_r12_r11"},
    {{vc, r12, r5, r6}, "", "vc r12 r5 r6", "vc_r12_r5_r6"},
    {{cs, r3, r9, r4}, "", "cs r3 r9 r4", "cs_r3_r9_r4"},
    {{ls, r4, r9, r11}, "", "ls r4 r9 r11", "ls_r4_r9_r11"},
    {{le, r14, r8, r13}, "", "le r14 r8 r13", "le_r14_r8_r13"},
    {{gt, r4, r10, r8}, "", "gt r4 r10 r8", "gt_r4_r10_r8"},
    {{al, r6, r9, r9}, "", "al r6 r9 r9", "al_r6_r9_r9"},
    {{ne, r8, r5, r12}, "", "ne r8 r5 r12", "ne_r8_r5_r12"},
    {{ne, r0, r4, r8}, "", "ne r0 r4 r8", "ne_r0_r4_r8"},
    {{mi, r7, r13, r3}, "", "mi r7 r13 r3", "mi_r7_r13_r3"},
    {{cc, r11, r7, r0}, "", "cc r11 r7 r0", "cc_r11_r7_r0"},
    {{hi, r1, r0, r12}, "", "hi r1 r0 r12", "hi_r1_r0_r12"},
    {{lt, r8, r9, r3}, "", "lt r8 r9 r3", "lt_r8_r9_r3"},
    {{al, r0, r2, r1}, "", "al r0 r2 r1", "al_r0_r2_r1"},
    {{vs, r4, r3, r14}, "", "vs r4 r3 r14", "vs_r4_r3_r14"},
    {{ge, r2, r11, r1}, "", "ge r2 r11 r1", "ge_r2_r11_r1"},
    {{lt, r12, r9, r6}, "", "lt r12 r9 r6", "lt_r12_r9_r6"},
    {{ls, r8, r2, r7}, "", "ls r8 r2 r7", "ls_r8_r2_r7"},
    {{le, r8, r13, r3}, "", "le r8 r13 r3", "le_r8_r13_r3"},
    {{eq, r11, r13, r14}, "", "eq r11 r13 r14", "eq_r11_r13_r14"},
    {{lt, r1, r6, r13}, "", "lt r1 r6 r13", "lt_r1_r6_r13"},
    {{cs, r3, r8, r11}, "", "cs r3 r8 r11", "cs_r3_r8_r11"},
    {{pl, r12, r5, r4}, "", "pl r12 r5 r4", "pl_r12_r5_r4"},
    {{eq, r8, r7, r2}, "", "eq r8 r7 r2", "eq_r8_r7_r2"},
    {{ls, r2, r12, r2}, "", "ls r2 r12 r2", "ls_r2_r12_r2"},
    {{le, r14, r2, r3}, "", "le r14 r2 r3", "le_r14_r2_r3"},
    {{ge, r10, r11, r6}, "", "ge r10 r11 r6", "ge_r10_r11_r6"},
    {{hi, r0, r2, r2}, "", "hi r0 r2 r2", "hi_r0_r2_r2"},
    {{ge, r2, r0, r2}, "", "ge r2 r0 r2", "ge_r2_r0_r2"},
    {{vs, r11, r14, r0}, "", "vs r11 r14 r0", "vs_r11_r14_r0"},
    {{lt, r2, r0, r1}, "", "lt r2 r0 r1", "lt_r2_r0_r1"},
    {{cs, r2, r5, r11}, "", "cs r2 r5 r11", "cs_r2_r5_r11"},
    {{ls, r7, r14, r5}, "", "ls r7 r14 r5", "ls_r7_r14_r5"},
    {{pl, r0, r0, r3}, "", "pl r0 r0 r3", "pl_r0_r0_r3"},
    {{ge, r6, r8, r8}, "", "ge r6 r8 r8", "ge_r6_r8_r8"},
    {{le, r11, r1, r10}, "", "le r11 r1 r10", "le_r11_r1_r10"},
    {{vs, r5, r2, r7}, "", "vs r5 r2 r7", "vs_r5_r2_r7"},
    {{ne, r4, r4, r8}, "", "ne r4 r4 r8", "ne_r4_r4_r8"},
    {{cc, r9, r14, r13}, "", "cc r9 r14 r13", "cc_r9_r14_r13"},
    {{hi, r14, r6, r3}, "", "hi r14 r6 r3", "hi_r14_r6_r3"},
    {{al, r0, r8, r0}, "", "al r0 r8 r0", "al_r0_r8_r0"},
    {{lt, r6, r11, r1}, "", "lt r6 r11 r1", "lt_r6_r11_r1"},
    {{ge, r7, r6, r12}, "", "ge r7 r6 r12", "ge_r7_r6_r12"},
    {{cs, r4, r6, r14}, "", "cs r4 r6 r14", "cs_r4_r6_r14"},
    {{cs, r7, r6, r7}, "", "cs r7 r6 r7", "cs_r7_r6_r7"},
    {{cs, r3, r7, r10}, "", "cs r3 r7 r10", "cs_r3_r7_r10"},
    {{ne, r0, r2, r1}, "", "ne r0 r2 r1", "ne_r0_r2_r1"},
    {{vs, r9, r10, r13}, "", "vs r9 r10 r13", "vs_r9_r10_r13"},
    {{vc, r11, r14, r12}, "", "vc r11 r14 r12", "vc_r11_r14_r12"},
    {{ge, r14, r8, r7}, "", "ge r14 r8 r7", "ge_r14_r8_r7"},
    {{lt, r13, r0, r11}, "", "lt r13 r0 r11", "lt_r13_r0_r11"},
    {{lt, r14, r13, r4}, "", "lt r14 r13 r4", "lt_r14_r13_r4"},
    {{al, r1, r10, r9}, "", "al r1 r10 r9", "al_r1_r10_r9"},
    {{ge, r11, r14, r11}, "", "ge r11 r14 r11", "ge_r11_r14_r11"},
    {{cs, r11, r4, r11}, "", "cs r11 r4 r11", "cs_r11_r4_r11"},
    {{ge, r0, r14, r7}, "", "ge r0 r14 r7", "ge_r0_r14_r7"},
    {{mi, r1, r2, r9}, "", "mi r1 r2 r9", "mi_r1_r2_r9"},
    {{eq, r5, r12, r3}, "", "eq r5 r12 r3", "eq_r5_r12_r3"},
    {{ge, r1, r5, r12}, "", "ge r1 r5 r12", "ge_r1_r5_r12"},
    {{lt, r10, r11, r4}, "", "lt r10 r11 r4", "lt_r10_r11_r4"},
    {{le, r1, r1, r5}, "", "le r1 r1 r5", "le_r1_r1_r5"},
    {{al, r9, r1, r8}, "", "al r9 r1 r8", "al_r9_r1_r8"},
    {{ne, r6, r8, r4}, "", "ne r6 r8 r4", "ne_r6_r8_r4"},
    {{ge, r12, r2, r9}, "", "ge r12 r2 r9", "ge_r12_r2_r9"},
    {{pl, r4, r3, r10}, "", "pl r4 r3 r10", "pl_r4_r3_r10"},
    {{eq, r14, r4, r11}, "", "eq r14 r4 r11", "eq_r14_r4_r11"},
    {{cc, r9, r7, r6}, "", "cc r9 r7 r6", "cc_r9_r7_r6"},
    {{ge, r12, r4, r5}, "", "ge r12 r4 r5", "ge_r12_r4_r5"},
    {{hi, r2, r3, r4}, "", "hi r2 r3 r4", "hi_r2_r3_r4"},
    {{cs, r0, r3, r1}, "", "cs r0 r3 r1", "cs_r0_r3_r1"},
    {{hi, r6, r2, r8}, "", "hi r6 r2 r8", "hi_r6_r2_r8"},
    {{cc, r3, r14, r13}, "", "cc r3 r14 r13", "cc_r3_r14_r13"},
    {{gt, r11, r4, r7}, "", "gt r11 r4 r7", "gt_r11_r4_r7"},
    {{hi, r5, r0, r12}, "", "hi r5 r0 r12", "hi_r5_r0_r12"},
    {{gt, r0, r14, r14}, "", "gt r0 r14 r14", "gt_r0_r14_r14"},
    {{hi, r9, r0, r10}, "", "hi r9 r0 r10", "hi_r9_r0_r10"},
    {{vc, r7, r11, r8}, "", "vc r7 r11 r8", "vc_r7_r11_r8"},
    {{pl, r11, r9, r6}, "", "pl r11 r9 r6", "pl_r11_r9_r6"},
    {{al, r3, r3, r7}, "", "al r3 r3 r7", "al_r3_r3_r7"},
    {{mi, r5, r7, r9}, "", "mi r5 r7 r9", "mi_r5_r7_r9"},
    {{cc, r11, r2, r4}, "", "cc r11 r2 r4", "cc_r11_r2_r4"},
    {{cc, r9, r13, r10}, "", "cc r9 r13 r10", "cc_r9_r13_r10"},
    {{al, r5, r2, r6}, "", "al r5 r2 r6", "al_r5_r2_r6"},
    {{ge, r9, r4, r6}, "", "ge r9 r4 r6", "ge_r9_r4_r6"},
    {{ls, r3, r3, r4}, "", "ls r3 r3 r4", "ls_r3_r3_r4"},
    {{ge, r14, r1, r8}, "", "ge r14 r1 r8", "ge_r14_r1_r8"},
    {{ls, r7, r12, r7}, "", "ls r7 r12 r7", "ls_r7_r12_r7"},
    {{al, r11, r10, r5}, "", "al r11 r10 r5", "al_r11_r10_r5"},
    {{al, r7, r4, r6}, "", "al r7 r4 r6", "al_r7_r4_r6"},
    {{vs, r12, r4, r10}, "", "vs r12 r4 r10", "vs_r12_r4_r10"},
    {{eq, r4, r4, r4}, "", "eq r4 r4 r4", "eq_r4_r4_r4"},
    {{vs, r6, r6, r12}, "", "vs r6 r6 r12", "vs_r6_r6_r12"},
    {{pl, r9, r3, r5}, "", "pl r9 r3 r5", "pl_r9_r3_r5"},
    {{eq, r6, r5, r13}, "", "eq r6 r5 r13", "eq_r6_r5_r13"},
    {{cc, r8, r2, r12}, "", "cc r8 r2 r12", "cc_r8_r2_r12"},
    {{le, r4, r2, r0}, "", "le r4 r2 r0", "le_r4_r2_r0"},
    {{lt, r7, r9, r8}, "", "lt r7 r9 r8", "lt_r7_r9_r8"},
    {{le, r4, r7, r11}, "", "le r4 r7 r11", "le_r4_r7_r11"},
    {{eq, r5, r7, r5}, "", "eq r5 r7 r5", "eq_r5_r7_r5"},
    {{vc, r10, r7, r12}, "", "vc r10 r7 r12", "vc_r10_r7_r12"},
    {{eq, r7, r10, r6}, "", "eq r7 r10 r6", "eq_r7_r10_r6"},
    {{pl, r1, r12, r2}, "", "pl r1 r12 r2", "pl_r1_r12_r2"},
    {{le, r14, r6, r6}, "", "le r14 r6 r6", "le_r14_r6_r6"},
    {{ne, r3, r8, r8}, "", "ne r3 r8 r8", "ne_r3_r8_r8"},
    {{eq, r4, r12, r8}, "", "eq r4 r12 r8", "eq_r4_r12_r8"},
    {{ge, r11, r2, r3}, "", "ge r11 r2 r3", "ge_r11_r2_r3"},
    {{hi, r12, r6, r11}, "", "hi r12 r6 r11", "hi_r12_r6_r11"},
    {{cs, r4, r5, r10}, "", "cs r4 r5 r10", "cs_r4_r5_r10"},
    {{ge, r10, r2, r10}, "", "ge r10 r2 r10", "ge_r10_r2_r10"},
    {{ge, r5, r14, r6}, "", "ge r5 r14 r6", "ge_r5_r14_r6"},
    {{gt, r13, r7, r5}, "", "gt r13 r7 r5", "gt_r13_r7_r5"},
    {{ge, r13, r4, r12}, "", "ge r13 r4 r12", "ge_r13_r4_r12"},
    {{lt, r8, r10, r14}, "", "lt r8 r10 r14", "lt_r8_r10_r14"},
    {{le, r4, r3, r13}, "", "le r4 r3 r13", "le_r4_r3_r13"},
    {{pl, r0, r9, r0}, "", "pl r0 r9 r0", "pl_r0_r9_r0"},
    {{eq, r2, r3, r1}, "", "eq r2 r3 r1", "eq_r2_r3_r1"},
    {{vc, r0, r0, r3}, "", "vc r0 r0 r3", "vc_r0_r0_r3"},
    {{mi, r10, r8, r11}, "", "mi r10 r8 r11", "mi_r10_r8_r11"},
    {{mi, r5, r14, r14}, "", "mi r5 r14 r14", "mi_r5_r14_r14"},
    {{gt, r5, r11, r2}, "", "gt r5 r11 r2", "gt_r5_r11_r2"},
    {{al, r4, r7, r11}, "", "al r4 r7 r11", "al_r4_r7_r11"}};

// These headers each contain an array of `TestResult` with the reference output
// values. The reference arrays are names `kReference{mnemonic}`.
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-mul.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-muls.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-qadd16.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-qadd8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-qasx.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-qsax.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-qsub16.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-qsub8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-sdiv.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-shadd16.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-shadd8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-shasx.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-shsax.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-shsub16.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-shsub8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-smmul.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-smmulr.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-smuad.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-smuadx.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-smulbb.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-smulbt.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-smultb.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-smultt.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-smulwb.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-smulwt.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-smusd.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-smusdx.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-udiv.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uhadd16.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uhadd8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uhasx.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uhsax.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uhsub16.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uhsub8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uqadd16.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uqadd8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uqasx.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uqsax.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uqsub16.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uqsub8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-usad8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-sadd16.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-sadd8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-sasx.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-sel.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-ssax.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-ssub16.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-ssub8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uadd16.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uadd8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-uasx.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-usax.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-usub16.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-usub8.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-qadd.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-qdadd.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-qdsub.h"
#include "aarch32/traces/assembler-cond-rd-rn-rm-a32-qsub.h"

// The maximum number of errors to report in detail for each test.
static const unsigned kErrorReportLimit = 8;

typedef void (MacroAssembler::*Fn)(Condition cond, Register rd, Register rn,
                                   Register rm);

static void TestHelper(Fn instruction, const char* mnemonic,
                       const TestResult reference[]) {
  unsigned total_error_count = 0;
  MacroAssembler masm(BUF_SIZE);

  masm.UseA32();

  for (unsigned i = 0; i < ARRAY_SIZE(kTests); i++) {
    // Values to pass to the macro-assembler.
    Condition cond = kTests[i].operands.cond;
    Register rd = kTests[i].operands.rd;
    Register rn = kTests[i].operands.rn;
    Register rm = kTests[i].operands.rm;

    uint32_t start = masm.GetCursorOffset();
    (masm.*instruction)(cond, rd, rn, rm);
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
      "AARCH32_ASSEMBLER_COND_RD_RN_RM_A32_" #mnemonic, &Test_##mnemonic);
FOREACH_INSTRUCTION(TEST)
#undef TEST

}  // aarch32
}  // vixl
