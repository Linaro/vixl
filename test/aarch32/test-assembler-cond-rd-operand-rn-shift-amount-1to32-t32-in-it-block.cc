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
#define FOREACH_INSTRUCTION(M) M(Mov)

// Values to be passed to the assembler to produce the instruction under test.
struct Operands {
  Condition cond;
  Register rd;
  Register rn;
  ShiftType shift;
  uint32_t amount;
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
    {{mi, r4, r6, ASR, 28}, "It mi;", "mi r4 r6 ASR 28", "mi_r4_r6_ASR_28"},
    {{cs, r6, r7, ASR, 4}, "It cs;", "cs r6 r7 ASR 4", "cs_r6_r7_ASR_4"},
    {{le, r6, r7, LSR, 7}, "It le;", "le r6 r7 LSR 7", "le_r6_r7_LSR_7"},
    {{vc, r1, r0, LSR, 20}, "It vc;", "vc r1 r0 LSR 20", "vc_r1_r0_LSR_20"},
    {{cs, r6, r5, ASR, 29}, "It cs;", "cs r6 r5 ASR 29", "cs_r6_r5_ASR_29"},
    {{vc, r1, r5, LSR, 7}, "It vc;", "vc r1 r5 LSR 7", "vc_r1_r5_LSR_7"},
    {{ge, r5, r4, LSR, 8}, "It ge;", "ge r5 r4 LSR 8", "ge_r5_r4_LSR_8"},
    {{cc, r4, r3, LSR, 3}, "It cc;", "cc r4 r3 LSR 3", "cc_r4_r3_LSR_3"},
    {{cc, r5, r1, LSR, 12}, "It cc;", "cc r5 r1 LSR 12", "cc_r5_r1_LSR_12"},
    {{ls, r4, r4, LSR, 16}, "It ls;", "ls r4 r4 LSR 16", "ls_r4_r4_LSR_16"},
    {{eq, r3, r6, ASR, 10}, "It eq;", "eq r3 r6 ASR 10", "eq_r3_r6_ASR_10"},
    {{gt, r1, r4, LSR, 22}, "It gt;", "gt r1 r4 LSR 22", "gt_r1_r4_LSR_22"},
    {{hi, r2, r3, LSR, 5}, "It hi;", "hi r2 r3 LSR 5", "hi_r2_r3_LSR_5"},
    {{lt, r1, r5, ASR, 5}, "It lt;", "lt r1 r5 ASR 5", "lt_r1_r5_ASR_5"},
    {{cs, r6, r0, ASR, 12}, "It cs;", "cs r6 r0 ASR 12", "cs_r6_r0_ASR_12"},
    {{mi, r0, r4, LSR, 21}, "It mi;", "mi r0 r4 LSR 21", "mi_r0_r4_LSR_21"},
    {{eq, r7, r0, LSR, 1}, "It eq;", "eq r7 r0 LSR 1", "eq_r7_r0_LSR_1"},
    {{vc, r5, r7, ASR, 28}, "It vc;", "vc r5 r7 ASR 28", "vc_r5_r7_ASR_28"},
    {{lt, r2, r0, LSR, 24}, "It lt;", "lt r2 r0 LSR 24", "lt_r2_r0_LSR_24"},
    {{gt, r6, r4, LSR, 11}, "It gt;", "gt r6 r4 LSR 11", "gt_r6_r4_LSR_11"},
    {{lt, r1, r4, LSR, 17}, "It lt;", "lt r1 r4 LSR 17", "lt_r1_r4_LSR_17"},
    {{hi, r1, r6, ASR, 2}, "It hi;", "hi r1 r6 ASR 2", "hi_r1_r6_ASR_2"},
    {{hi, r0, r4, ASR, 7}, "It hi;", "hi r0 r4 ASR 7", "hi_r0_r4_ASR_7"},
    {{ne, r1, r2, ASR, 24}, "It ne;", "ne r1 r2 ASR 24", "ne_r1_r2_ASR_24"},
    {{lt, r3, r6, LSR, 6}, "It lt;", "lt r3 r6 LSR 6", "lt_r3_r6_LSR_6"},
    {{gt, r6, r4, ASR, 14}, "It gt;", "gt r6 r4 ASR 14", "gt_r6_r4_ASR_14"},
    {{gt, r1, r2, LSR, 26}, "It gt;", "gt r1 r2 LSR 26", "gt_r1_r2_LSR_26"},
    {{ne, r2, r3, ASR, 29}, "It ne;", "ne r2 r3 ASR 29", "ne_r2_r3_ASR_29"},
    {{le, r7, r0, ASR, 23}, "It le;", "le r7 r0 ASR 23", "le_r7_r0_ASR_23"},
    {{gt, r3, r7, LSR, 27}, "It gt;", "gt r3 r7 LSR 27", "gt_r3_r7_LSR_27"},
    {{lt, r0, r7, ASR, 26}, "It lt;", "lt r0 r7 ASR 26", "lt_r0_r7_ASR_26"},
    {{ge, r1, r6, ASR, 16}, "It ge;", "ge r1 r6 ASR 16", "ge_r1_r6_ASR_16"},
    {{ls, r5, r3, LSR, 7}, "It ls;", "ls r5 r3 LSR 7", "ls_r5_r3_LSR_7"},
    {{mi, r5, r5, LSR, 4}, "It mi;", "mi r5 r5 LSR 4", "mi_r5_r5_LSR_4"},
    {{eq, r7, r7, ASR, 5}, "It eq;", "eq r7 r7 ASR 5", "eq_r7_r7_ASR_5"},
    {{le, r5, r4, ASR, 32}, "It le;", "le r5 r4 ASR 32", "le_r5_r4_ASR_32"},
    {{pl, r7, r7, LSR, 9}, "It pl;", "pl r7 r7 LSR 9", "pl_r7_r7_LSR_9"},
    {{ne, r4, r1, LSR, 14}, "It ne;", "ne r4 r1 LSR 14", "ne_r4_r1_LSR_14"},
    {{ge, r0, r2, LSR, 2}, "It ge;", "ge r0 r2 LSR 2", "ge_r0_r2_LSR_2"},
    {{cs, r1, r3, ASR, 17}, "It cs;", "cs r1 r3 ASR 17", "cs_r1_r3_ASR_17"},
    {{cc, r1, r7, LSR, 9}, "It cc;", "cc r1 r7 LSR 9", "cc_r1_r7_LSR_9"},
    {{hi, r3, r3, ASR, 7}, "It hi;", "hi r3 r3 ASR 7", "hi_r3_r3_ASR_7"},
    {{cs, r0, r6, LSR, 32}, "It cs;", "cs r0 r6 LSR 32", "cs_r0_r6_LSR_32"},
    {{lt, r1, r5, ASR, 27}, "It lt;", "lt r1 r5 ASR 27", "lt_r1_r5_ASR_27"},
    {{le, r0, r2, LSR, 21}, "It le;", "le r0 r2 LSR 21", "le_r0_r2_LSR_21"},
    {{cc, r7, r1, LSR, 29}, "It cc;", "cc r7 r1 LSR 29", "cc_r7_r1_LSR_29"},
    {{cc, r5, r2, LSR, 3}, "It cc;", "cc r5 r2 LSR 3", "cc_r5_r2_LSR_3"},
    {{le, r7, r3, LSR, 3}, "It le;", "le r7 r3 LSR 3", "le_r7_r3_LSR_3"},
    {{le, r3, r4, LSR, 3}, "It le;", "le r3 r4 LSR 3", "le_r3_r4_LSR_3"},
    {{vs, r6, r4, LSR, 5}, "It vs;", "vs r6 r4 LSR 5", "vs_r6_r4_LSR_5"},
    {{gt, r0, r0, ASR, 24}, "It gt;", "gt r0 r0 ASR 24", "gt_r0_r0_ASR_24"},
    {{vc, r1, r3, ASR, 20}, "It vc;", "vc r1 r3 ASR 20", "vc_r1_r3_ASR_20"},
    {{lt, r6, r0, LSR, 25}, "It lt;", "lt r6 r0 LSR 25", "lt_r6_r0_LSR_25"},
    {{vs, r0, r6, ASR, 9}, "It vs;", "vs r0 r6 ASR 9", "vs_r0_r6_ASR_9"},
    {{lt, r0, r1, LSR, 16}, "It lt;", "lt r0 r1 LSR 16", "lt_r0_r1_LSR_16"},
    {{mi, r5, r5, ASR, 3}, "It mi;", "mi r5 r5 ASR 3", "mi_r5_r5_ASR_3"},
    {{cs, r3, r3, ASR, 23}, "It cs;", "cs r3 r3 ASR 23", "cs_r3_r3_ASR_23"},
    {{hi, r7, r4, ASR, 1}, "It hi;", "hi r7 r4 ASR 1", "hi_r7_r4_ASR_1"},
    {{lt, r0, r4, ASR, 28}, "It lt;", "lt r0 r4 ASR 28", "lt_r0_r4_ASR_28"},
    {{ne, r7, r0, LSR, 28}, "It ne;", "ne r7 r0 LSR 28", "ne_r7_r0_LSR_28"},
    {{cc, r7, r3, ASR, 16}, "It cc;", "cc r7 r3 ASR 16", "cc_r7_r3_ASR_16"},
    {{cc, r1, r1, ASR, 11}, "It cc;", "cc r1 r1 ASR 11", "cc_r1_r1_ASR_11"},
    {{pl, r2, r1, ASR, 2}, "It pl;", "pl r2 r1 ASR 2", "pl_r2_r1_ASR_2"},
    {{mi, r5, r2, ASR, 16}, "It mi;", "mi r5 r2 ASR 16", "mi_r5_r2_ASR_16"},
    {{vc, r2, r3, ASR, 14}, "It vc;", "vc r2 r3 ASR 14", "vc_r2_r3_ASR_14"},
    {{hi, r7, r6, ASR, 12}, "It hi;", "hi r7 r6 ASR 12", "hi_r7_r6_ASR_12"},
    {{pl, r6, r3, ASR, 12}, "It pl;", "pl r6 r3 ASR 12", "pl_r6_r3_ASR_12"},
    {{cc, r5, r2, LSR, 13}, "It cc;", "cc r5 r2 LSR 13", "cc_r5_r2_LSR_13"},
    {{vs, r7, r5, ASR, 22}, "It vs;", "vs r7 r5 ASR 22", "vs_r7_r5_ASR_22"},
    {{vc, r3, r4, LSR, 18}, "It vc;", "vc r3 r4 LSR 18", "vc_r3_r4_LSR_18"},
    {{vs, r3, r1, LSR, 22}, "It vs;", "vs r3 r1 LSR 22", "vs_r3_r1_LSR_22"},
    {{vs, r0, r4, ASR, 27}, "It vs;", "vs r0 r4 ASR 27", "vs_r0_r4_ASR_27"},
    {{le, r6, r5, LSR, 3}, "It le;", "le r6 r5 LSR 3", "le_r6_r5_LSR_3"},
    {{hi, r1, r4, ASR, 18}, "It hi;", "hi r1 r4 ASR 18", "hi_r1_r4_ASR_18"},
    {{le, r6, r6, ASR, 14}, "It le;", "le r6 r6 ASR 14", "le_r6_r6_ASR_14"},
    {{ge, r1, r6, ASR, 32}, "It ge;", "ge r1 r6 ASR 32", "ge_r1_r6_ASR_32"},
    {{le, r4, r4, ASR, 25}, "It le;", "le r4 r4 ASR 25", "le_r4_r4_ASR_25"},
    {{cc, r2, r1, ASR, 3}, "It cc;", "cc r2 r1 ASR 3", "cc_r2_r1_ASR_3"},
    {{ge, r5, r4, ASR, 20}, "It ge;", "ge r5 r4 ASR 20", "ge_r5_r4_ASR_20"},
    {{mi, r5, r5, LSR, 30}, "It mi;", "mi r5 r5 LSR 30", "mi_r5_r5_LSR_30"},
    {{hi, r1, r7, ASR, 10}, "It hi;", "hi r1 r7 ASR 10", "hi_r1_r7_ASR_10"},
    {{lt, r4, r3, ASR, 1}, "It lt;", "lt r4 r3 ASR 1", "lt_r4_r3_ASR_1"},
    {{ne, r4, r3, ASR, 28}, "It ne;", "ne r4 r3 ASR 28", "ne_r4_r3_ASR_28"},
    {{vs, r1, r1, ASR, 6}, "It vs;", "vs r1 r1 ASR 6", "vs_r1_r1_ASR_6"},
    {{lt, r3, r1, ASR, 16}, "It lt;", "lt r3 r1 ASR 16", "lt_r3_r1_ASR_16"},
    {{eq, r7, r4, ASR, 7}, "It eq;", "eq r7 r4 ASR 7", "eq_r7_r4_ASR_7"},
    {{le, r2, r6, LSR, 3}, "It le;", "le r2 r6 LSR 3", "le_r2_r6_LSR_3"},
    {{ls, r0, r3, ASR, 32}, "It ls;", "ls r0 r3 ASR 32", "ls_r0_r3_ASR_32"},
    {{pl, r7, r7, ASR, 12}, "It pl;", "pl r7 r7 ASR 12", "pl_r7_r7_ASR_12"},
    {{pl, r0, r4, LSR, 25}, "It pl;", "pl r0 r4 LSR 25", "pl_r0_r4_LSR_25"},
    {{ge, r3, r7, ASR, 29}, "It ge;", "ge r3 r7 ASR 29", "ge_r3_r7_ASR_29"},
    {{cc, r7, r7, LSR, 6}, "It cc;", "cc r7 r7 LSR 6", "cc_r7_r7_LSR_6"},
    {{ls, r4, r5, ASR, 23}, "It ls;", "ls r4 r5 ASR 23", "ls_r4_r5_ASR_23"},
    {{mi, r4, r3, LSR, 9}, "It mi;", "mi r4 r3 LSR 9", "mi_r4_r3_LSR_9"},
    {{pl, r7, r4, LSR, 11}, "It pl;", "pl r7 r4 LSR 11", "pl_r7_r4_LSR_11"},
    {{pl, r7, r6, LSR, 15}, "It pl;", "pl r7 r6 LSR 15", "pl_r7_r6_LSR_15"},
    {{cc, r5, r5, ASR, 1}, "It cc;", "cc r5 r5 ASR 1", "cc_r5_r5_ASR_1"},
    {{vc, r5, r2, LSR, 31}, "It vc;", "vc r5 r2 LSR 31", "vc_r5_r2_LSR_31"},
    {{pl, r5, r3, ASR, 12}, "It pl;", "pl r5 r3 ASR 12", "pl_r5_r3_ASR_12"},
    {{vc, r1, r5, LSR, 18}, "It vc;", "vc r1 r5 LSR 18", "vc_r1_r5_LSR_18"},
    {{vs, r6, r6, LSR, 24}, "It vs;", "vs r6 r6 LSR 24", "vs_r6_r6_LSR_24"},
    {{cs, r6, r5, ASR, 4}, "It cs;", "cs r6 r5 ASR 4", "cs_r6_r5_ASR_4"},
    {{le, r2, r0, LSR, 11}, "It le;", "le r2 r0 LSR 11", "le_r2_r0_LSR_11"},
    {{eq, r5, r6, LSR, 18}, "It eq;", "eq r5 r6 LSR 18", "eq_r5_r6_LSR_18"},
    {{vs, r3, r2, ASR, 14}, "It vs;", "vs r3 r2 ASR 14", "vs_r3_r2_ASR_14"},
    {{pl, r7, r3, ASR, 1}, "It pl;", "pl r7 r3 ASR 1", "pl_r7_r3_ASR_1"},
    {{cc, r3, r0, ASR, 14}, "It cc;", "cc r3 r0 ASR 14", "cc_r3_r0_ASR_14"},
    {{cc, r7, r1, LSR, 1}, "It cc;", "cc r7 r1 LSR 1", "cc_r7_r1_LSR_1"},
    {{hi, r7, r3, ASR, 11}, "It hi;", "hi r7 r3 ASR 11", "hi_r7_r3_ASR_11"},
    {{vs, r7, r6, ASR, 9}, "It vs;", "vs r7 r6 ASR 9", "vs_r7_r6_ASR_9"},
    {{gt, r5, r7, LSR, 7}, "It gt;", "gt r5 r7 LSR 7", "gt_r5_r7_LSR_7"},
    {{cc, r2, r6, ASR, 13}, "It cc;", "cc r2 r6 ASR 13", "cc_r2_r6_ASR_13"},
    {{vs, r5, r1, LSR, 9}, "It vs;", "vs r5 r1 LSR 9", "vs_r5_r1_LSR_9"},
    {{ne, r5, r0, ASR, 17}, "It ne;", "ne r5 r0 ASR 17", "ne_r5_r0_ASR_17"},
    {{hi, r2, r4, ASR, 25}, "It hi;", "hi r2 r4 ASR 25", "hi_r2_r4_ASR_25"},
    {{ne, r6, r3, ASR, 20}, "It ne;", "ne r6 r3 ASR 20", "ne_r6_r3_ASR_20"},
    {{eq, r5, r0, LSR, 26}, "It eq;", "eq r5 r0 LSR 26", "eq_r5_r0_LSR_26"},
    {{ne, r7, r7, LSR, 1}, "It ne;", "ne r7 r7 LSR 1", "ne_r7_r7_LSR_1"},
    {{ne, r6, r1, ASR, 10}, "It ne;", "ne r6 r1 ASR 10", "ne_r6_r1_ASR_10"},
    {{ne, r0, r3, ASR, 32}, "It ne;", "ne r0 r3 ASR 32", "ne_r0_r3_ASR_32"},
    {{lt, r0, r1, ASR, 15}, "It lt;", "lt r0 r1 ASR 15", "lt_r0_r1_ASR_15"},
    {{eq, r5, r7, ASR, 26}, "It eq;", "eq r5 r7 ASR 26", "eq_r5_r7_ASR_26"},
    {{vs, r4, r6, ASR, 23}, "It vs;", "vs r4 r6 ASR 23", "vs_r4_r6_ASR_23"},
    {{le, r0, r1, LSR, 28}, "It le;", "le r0 r1 LSR 28", "le_r0_r1_LSR_28"},
    {{ge, r3, r1, ASR, 8}, "It ge;", "ge r3 r1 ASR 8", "ge_r3_r1_ASR_8"},
    {{ne, r7, r6, ASR, 9}, "It ne;", "ne r7 r6 ASR 9", "ne_r7_r6_ASR_9"},
    {{ne, r1, r3, ASR, 21}, "It ne;", "ne r1 r3 ASR 21", "ne_r1_r3_ASR_21"},
    {{cc, r4, r0, LSR, 32}, "It cc;", "cc r4 r0 LSR 32", "cc_r4_r0_LSR_32"},
    {{vc, r7, r3, ASR, 15}, "It vc;", "vc r7 r3 ASR 15", "vc_r7_r3_ASR_15"},
    {{gt, r7, r4, ASR, 14}, "It gt;", "gt r7 r4 ASR 14", "gt_r7_r4_ASR_14"},
    {{eq, r0, r3, ASR, 12}, "It eq;", "eq r0 r3 ASR 12", "eq_r0_r3_ASR_12"},
    {{ne, r3, r4, ASR, 29}, "It ne;", "ne r3 r4 ASR 29", "ne_r3_r4_ASR_29"},
    {{gt, r3, r7, ASR, 21}, "It gt;", "gt r3 r7 ASR 21", "gt_r3_r7_ASR_21"},
    {{le, r0, r4, ASR, 31}, "It le;", "le r0 r4 ASR 31", "le_r0_r4_ASR_31"},
    {{eq, r7, r4, ASR, 10}, "It eq;", "eq r7 r4 ASR 10", "eq_r7_r4_ASR_10"},
    {{ne, r7, r1, LSR, 32}, "It ne;", "ne r7 r1 LSR 32", "ne_r7_r1_LSR_32"},
    {{cs, r5, r5, LSR, 1}, "It cs;", "cs r5 r5 LSR 1", "cs_r5_r5_LSR_1"},
    {{cc, r3, r3, ASR, 32}, "It cc;", "cc r3 r3 ASR 32", "cc_r3_r3_ASR_32"},
    {{le, r4, r5, ASR, 20}, "It le;", "le r4 r5 ASR 20", "le_r4_r5_ASR_20"},
    {{cc, r3, r7, ASR, 10}, "It cc;", "cc r3 r7 ASR 10", "cc_r3_r7_ASR_10"},
    {{mi, r2, r5, LSR, 6}, "It mi;", "mi r2 r5 LSR 6", "mi_r2_r5_LSR_6"},
    {{le, r1, r7, LSR, 5}, "It le;", "le r1 r7 LSR 5", "le_r1_r7_LSR_5"},
    {{ls, r4, r4, ASR, 7}, "It ls;", "ls r4 r4 ASR 7", "ls_r4_r4_ASR_7"},
    {{mi, r7, r3, ASR, 23}, "It mi;", "mi r7 r3 ASR 23", "mi_r7_r3_ASR_23"},
    {{vs, r2, r7, ASR, 1}, "It vs;", "vs r2 r7 ASR 1", "vs_r2_r7_ASR_1"},
    {{mi, r0, r4, ASR, 2}, "It mi;", "mi r0 r4 ASR 2", "mi_r0_r4_ASR_2"},
    {{ge, r0, r1, ASR, 1}, "It ge;", "ge r0 r1 ASR 1", "ge_r0_r1_ASR_1"},
    {{pl, r7, r1, LSR, 11}, "It pl;", "pl r7 r1 LSR 11", "pl_r7_r1_LSR_11"},
    {{mi, r2, r6, ASR, 1}, "It mi;", "mi r2 r6 ASR 1", "mi_r2_r6_ASR_1"},
    {{hi, r4, r7, ASR, 1}, "It hi;", "hi r4 r7 ASR 1", "hi_r4_r7_ASR_1"},
    {{hi, r1, r3, LSR, 18}, "It hi;", "hi r1 r3 LSR 18", "hi_r1_r3_LSR_18"},
    {{cc, r1, r0, LSR, 13}, "It cc;", "cc r1 r0 LSR 13", "cc_r1_r0_LSR_13"},
    {{gt, r5, r2, LSR, 9}, "It gt;", "gt r5 r2 LSR 9", "gt_r5_r2_LSR_9"},
    {{vs, r6, r1, LSR, 24}, "It vs;", "vs r6 r1 LSR 24", "vs_r6_r1_LSR_24"},
    {{mi, r3, r1, ASR, 15}, "It mi;", "mi r3 r1 ASR 15", "mi_r3_r1_ASR_15"},
    {{cs, r5, r5, ASR, 14}, "It cs;", "cs r5 r5 ASR 14", "cs_r5_r5_ASR_14"},
    {{vs, r2, r6, ASR, 32}, "It vs;", "vs r2 r6 ASR 32", "vs_r2_r6_ASR_32"},
    {{eq, r6, r3, LSR, 24}, "It eq;", "eq r6 r3 LSR 24", "eq_r6_r3_LSR_24"},
    {{vs, r6, r3, LSR, 2}, "It vs;", "vs r6 r3 LSR 2", "vs_r6_r3_LSR_2"},
    {{vc, r4, r3, LSR, 31}, "It vc;", "vc r4 r3 LSR 31", "vc_r4_r3_LSR_31"},
    {{ge, r5, r2, LSR, 26}, "It ge;", "ge r5 r2 LSR 26", "ge_r5_r2_LSR_26"},
    {{gt, r6, r5, LSR, 10}, "It gt;", "gt r6 r5 LSR 10", "gt_r6_r5_LSR_10"},
    {{cc, r7, r2, ASR, 28}, "It cc;", "cc r7 r2 ASR 28", "cc_r7_r2_ASR_28"},
    {{gt, r1, r5, LSR, 26}, "It gt;", "gt r1 r5 LSR 26", "gt_r1_r5_LSR_26"},
    {{eq, r3, r6, LSR, 24}, "It eq;", "eq r3 r6 LSR 24", "eq_r3_r6_LSR_24"},
    {{le, r7, r5, ASR, 12}, "It le;", "le r7 r5 ASR 12", "le_r7_r5_ASR_12"},
    {{eq, r5, r1, LSR, 6}, "It eq;", "eq r5 r1 LSR 6", "eq_r5_r1_LSR_6"},
    {{vc, r5, r3, ASR, 10}, "It vc;", "vc r5 r3 ASR 10", "vc_r5_r3_ASR_10"},
    {{le, r1, r7, LSR, 2}, "It le;", "le r1 r7 LSR 2", "le_r1_r7_LSR_2"},
    {{vc, r0, r2, LSR, 19}, "It vc;", "vc r0 r2 LSR 19", "vc_r0_r2_LSR_19"},
    {{eq, r1, r5, ASR, 22}, "It eq;", "eq r1 r5 ASR 22", "eq_r1_r5_ASR_22"},
    {{eq, r0, r7, LSR, 26}, "It eq;", "eq r0 r7 LSR 26", "eq_r0_r7_LSR_26"},
    {{ge, r2, r5, LSR, 28}, "It ge;", "ge r2 r5 LSR 28", "ge_r2_r5_LSR_28"},
    {{ne, r1, r4, ASR, 9}, "It ne;", "ne r1 r4 ASR 9", "ne_r1_r4_ASR_9"},
    {{ls, r7, r2, ASR, 26}, "It ls;", "ls r7 r2 ASR 26", "ls_r7_r2_ASR_26"},
    {{lt, r2, r6, LSR, 11}, "It lt;", "lt r2 r6 LSR 11", "lt_r2_r6_LSR_11"},
    {{ne, r5, r5, ASR, 31}, "It ne;", "ne r5 r5 ASR 31", "ne_r5_r5_ASR_31"},
    {{eq, r0, r3, ASR, 24}, "It eq;", "eq r0 r3 ASR 24", "eq_r0_r3_ASR_24"},
    {{cc, r2, r5, ASR, 10}, "It cc;", "cc r2 r5 ASR 10", "cc_r2_r5_ASR_10"},
    {{ne, r5, r2, ASR, 3}, "It ne;", "ne r5 r2 ASR 3", "ne_r5_r2_ASR_3"},
    {{mi, r2, r6, LSR, 24}, "It mi;", "mi r2 r6 LSR 24", "mi_r2_r6_LSR_24"},
    {{vs, r4, r6, ASR, 28}, "It vs;", "vs r4 r6 ASR 28", "vs_r4_r6_ASR_28"},
    {{mi, r1, r1, ASR, 5}, "It mi;", "mi r1 r1 ASR 5", "mi_r1_r1_ASR_5"},
    {{mi, r3, r7, LSR, 5}, "It mi;", "mi r3 r7 LSR 5", "mi_r3_r7_LSR_5"},
    {{vs, r4, r1, LSR, 10}, "It vs;", "vs r4 r1 LSR 10", "vs_r4_r1_LSR_10"},
    {{cc, r2, r6, LSR, 14}, "It cc;", "cc r2 r6 LSR 14", "cc_r2_r6_LSR_14"},
    {{mi, r1, r2, LSR, 18}, "It mi;", "mi r1 r2 LSR 18", "mi_r1_r2_LSR_18"},
    {{gt, r6, r7, LSR, 23}, "It gt;", "gt r6 r7 LSR 23", "gt_r6_r7_LSR_23"},
    {{cc, r0, r7, ASR, 26}, "It cc;", "cc r0 r7 ASR 26", "cc_r0_r7_ASR_26"},
    {{eq, r2, r6, LSR, 9}, "It eq;", "eq r2 r6 LSR 9", "eq_r2_r6_LSR_9"},
    {{pl, r5, r1, ASR, 19}, "It pl;", "pl r5 r1 ASR 19", "pl_r5_r1_ASR_19"},
    {{ge, r4, r4, ASR, 9}, "It ge;", "ge r4 r4 ASR 9", "ge_r4_r4_ASR_9"},
    {{cc, r4, r5, ASR, 9}, "It cc;", "cc r4 r5 ASR 9", "cc_r4_r5_ASR_9"},
    {{lt, r4, r3, ASR, 30}, "It lt;", "lt r4 r3 ASR 30", "lt_r4_r3_ASR_30"},
    {{ge, r0, r3, ASR, 18}, "It ge;", "ge r0 r3 ASR 18", "ge_r0_r3_ASR_18"},
    {{ls, r3, r7, LSR, 7}, "It ls;", "ls r3 r7 LSR 7", "ls_r3_r7_LSR_7"},
    {{eq, r2, r0, LSR, 10}, "It eq;", "eq r2 r0 LSR 10", "eq_r2_r0_LSR_10"},
    {{cc, r1, r3, LSR, 5}, "It cc;", "cc r1 r3 LSR 5", "cc_r1_r3_LSR_5"},
    {{hi, r6, r5, ASR, 1}, "It hi;", "hi r6 r5 ASR 1", "hi_r6_r5_ASR_1"},
    {{lt, r4, r4, ASR, 13}, "It lt;", "lt r4 r4 ASR 13", "lt_r4_r4_ASR_13"},
    {{pl, r1, r7, LSR, 12}, "It pl;", "pl r1 r7 LSR 12", "pl_r1_r7_LSR_12"},
    {{cc, r5, r5, LSR, 14}, "It cc;", "cc r5 r5 LSR 14", "cc_r5_r5_LSR_14"},
    {{gt, r5, r7, LSR, 9}, "It gt;", "gt r5 r7 LSR 9", "gt_r5_r7_LSR_9"},
    {{ne, r7, r0, ASR, 3}, "It ne;", "ne r7 r0 ASR 3", "ne_r7_r0_ASR_3"},
    {{vs, r1, r6, LSR, 29}, "It vs;", "vs r1 r6 LSR 29", "vs_r1_r6_LSR_29"},
    {{eq, r2, r6, ASR, 24}, "It eq;", "eq r2 r6 ASR 24", "eq_r2_r6_ASR_24"},
    {{ls, r4, r5, ASR, 27}, "It ls;", "ls r4 r5 ASR 27", "ls_r4_r5_ASR_27"},
    {{ls, r5, r2, ASR, 19}, "It ls;", "ls r5 r2 ASR 19", "ls_r5_r2_ASR_19"},
    {{hi, r2, r5, LSR, 12}, "It hi;", "hi r2 r5 LSR 12", "hi_r2_r5_LSR_12"},
    {{gt, r1, r7, LSR, 25}, "It gt;", "gt r1 r7 LSR 25", "gt_r1_r7_LSR_25"},
    {{vc, r4, r6, LSR, 13}, "It vc;", "vc r4 r6 LSR 13", "vc_r4_r6_LSR_13"},
    {{ge, r5, r1, ASR, 13}, "It ge;", "ge r5 r1 ASR 13", "ge_r5_r1_ASR_13"},
    {{cc, r3, r4, LSR, 24}, "It cc;", "cc r3 r4 LSR 24", "cc_r3_r4_LSR_24"},
    {{eq, r2, r3, LSR, 28}, "It eq;", "eq r2 r3 LSR 28", "eq_r2_r3_LSR_28"},
    {{hi, r2, r5, ASR, 31}, "It hi;", "hi r2 r5 ASR 31", "hi_r2_r5_ASR_31"},
    {{ne, r2, r1, ASR, 3}, "It ne;", "ne r2 r1 ASR 3", "ne_r2_r1_ASR_3"},
    {{gt, r6, r2, ASR, 12}, "It gt;", "gt r6 r2 ASR 12", "gt_r6_r2_ASR_12"},
    {{ls, r0, r1, LSR, 15}, "It ls;", "ls r0 r1 LSR 15", "ls_r0_r1_LSR_15"},
    {{ls, r1, r2, ASR, 25}, "It ls;", "ls r1 r2 ASR 25", "ls_r1_r2_ASR_25"},
    {{vc, r0, r3, ASR, 13}, "It vc;", "vc r0 r3 ASR 13", "vc_r0_r3_ASR_13"},
    {{vc, r3, r1, ASR, 26}, "It vc;", "vc r3 r1 ASR 26", "vc_r3_r1_ASR_26"},
    {{vc, r1, r1, LSR, 19}, "It vc;", "vc r1 r1 LSR 19", "vc_r1_r1_LSR_19"},
    {{le, r5, r0, LSR, 31}, "It le;", "le r5 r0 LSR 31", "le_r5_r0_LSR_31"},
    {{eq, r3, r6, ASR, 8}, "It eq;", "eq r3 r6 ASR 8", "eq_r3_r6_ASR_8"},
    {{le, r2, r3, ASR, 25}, "It le;", "le r2 r3 ASR 25", "le_r2_r3_ASR_25"},
    {{ge, r1, r0, ASR, 32}, "It ge;", "ge r1 r0 ASR 32", "ge_r1_r0_ASR_32"},
    {{le, r3, r4, ASR, 30}, "It le;", "le r3 r4 ASR 30", "le_r3_r4_ASR_30"},
    {{ge, r3, r0, ASR, 19}, "It ge;", "ge r3 r0 ASR 19", "ge_r3_r0_ASR_19"},
    {{ls, r4, r6, ASR, 12}, "It ls;", "ls r4 r6 ASR 12", "ls_r4_r6_ASR_12"},
    {{hi, r1, r0, ASR, 15}, "It hi;", "hi r1 r0 ASR 15", "hi_r1_r0_ASR_15"},
    {{ge, r5, r7, LSR, 27}, "It ge;", "ge r5 r7 LSR 27", "ge_r5_r7_LSR_27"},
    {{cs, r2, r6, LSR, 1}, "It cs;", "cs r2 r6 LSR 1", "cs_r2_r6_LSR_1"},
    {{vc, r6, r5, ASR, 25}, "It vc;", "vc r6 r5 ASR 25", "vc_r6_r5_ASR_25"},
    {{gt, r0, r6, LSR, 24}, "It gt;", "gt r0 r6 LSR 24", "gt_r0_r6_LSR_24"},
    {{vc, r6, r5, ASR, 15}, "It vc;", "vc r6 r5 ASR 15", "vc_r6_r5_ASR_15"},
    {{vs, r6, r0, ASR, 6}, "It vs;", "vs r6 r0 ASR 6", "vs_r6_r0_ASR_6"},
    {{ge, r7, r4, LSR, 22}, "It ge;", "ge r7 r4 LSR 22", "ge_r7_r4_LSR_22"},
    {{gt, r0, r3, LSR, 21}, "It gt;", "gt r0 r3 LSR 21", "gt_r0_r3_LSR_21"},
    {{ls, r0, r4, ASR, 11}, "It ls;", "ls r0 r4 ASR 11", "ls_r0_r4_ASR_11"},
    {{vs, r0, r5, ASR, 1}, "It vs;", "vs r0 r5 ASR 1", "vs_r0_r5_ASR_1"},
    {{ls, r2, r0, LSR, 9}, "It ls;", "ls r2 r0 LSR 9", "ls_r2_r0_LSR_9"},
    {{le, r0, r0, LSR, 32}, "It le;", "le r0 r0 LSR 32", "le_r0_r0_LSR_32"},
    {{lt, r6, r6, ASR, 19}, "It lt;", "lt r6 r6 ASR 19", "lt_r6_r6_ASR_19"},
    {{ge, r0, r6, ASR, 7}, "It ge;", "ge r0 r6 ASR 7", "ge_r0_r6_ASR_7"},
    {{mi, r4, r7, LSR, 13}, "It mi;", "mi r4 r7 LSR 13", "mi_r4_r7_LSR_13"},
    {{ne, r0, r7, ASR, 30}, "It ne;", "ne r0 r7 ASR 30", "ne_r0_r7_ASR_30"},
    {{cs, r0, r5, ASR, 17}, "It cs;", "cs r0 r5 ASR 17", "cs_r0_r5_ASR_17"},
    {{ge, r6, r5, ASR, 10}, "It ge;", "ge r6 r5 ASR 10", "ge_r6_r5_ASR_10"},
    {{gt, r5, r7, LSR, 29}, "It gt;", "gt r5 r7 LSR 29", "gt_r5_r7_LSR_29"},
    {{ne, r1, r5, LSR, 21}, "It ne;", "ne r1 r5 LSR 21", "ne_r1_r5_LSR_21"},
    {{pl, r4, r5, ASR, 3}, "It pl;", "pl r4 r5 ASR 3", "pl_r4_r5_ASR_3"},
    {{vs, r5, r5, ASR, 26}, "It vs;", "vs r5 r5 ASR 26", "vs_r5_r5_ASR_26"},
    {{mi, r4, r0, LSR, 4}, "It mi;", "mi r4 r0 LSR 4", "mi_r4_r0_LSR_4"},
    {{eq, r7, r6, ASR, 15}, "It eq;", "eq r7 r6 ASR 15", "eq_r7_r6_ASR_15"},
    {{ls, r4, r1, LSR, 28}, "It ls;", "ls r4 r1 LSR 28", "ls_r4_r1_LSR_28"},
    {{le, r0, r5, LSR, 19}, "It le;", "le r0 r5 LSR 19", "le_r0_r5_LSR_19"},
    {{lt, r0, r4, LSR, 26}, "It lt;", "lt r0 r4 LSR 26", "lt_r0_r4_LSR_26"},
    {{hi, r7, r5, ASR, 13}, "It hi;", "hi r7 r5 ASR 13", "hi_r7_r5_ASR_13"},
    {{gt, r5, r5, ASR, 2}, "It gt;", "gt r5 r5 ASR 2", "gt_r5_r5_ASR_2"},
    {{ls, r5, r3, LSR, 1}, "It ls;", "ls r5 r3 LSR 1", "ls_r5_r3_LSR_1"},
    {{mi, r7, r5, LSR, 22}, "It mi;", "mi r7 r5 LSR 22", "mi_r7_r5_LSR_22"},
    {{ge, r5, r7, LSR, 1}, "It ge;", "ge r5 r7 LSR 1", "ge_r5_r7_LSR_1"},
    {{vs, r4, r1, ASR, 5}, "It vs;", "vs r4 r1 ASR 5", "vs_r4_r1_ASR_5"},
    {{vs, r2, r4, LSR, 27}, "It vs;", "vs r2 r4 LSR 27", "vs_r2_r4_LSR_27"},
    {{hi, r0, r3, ASR, 25}, "It hi;", "hi r0 r3 ASR 25", "hi_r0_r3_ASR_25"},
    {{mi, r3, r5, LSR, 5}, "It mi;", "mi r3 r5 LSR 5", "mi_r3_r5_LSR_5"},
    {{vc, r6, r5, ASR, 32}, "It vc;", "vc r6 r5 ASR 32", "vc_r6_r5_ASR_32"},
    {{ne, r0, r7, LSR, 12}, "It ne;", "ne r0 r7 LSR 12", "ne_r0_r7_LSR_12"},
    {{hi, r1, r7, ASR, 1}, "It hi;", "hi r1 r7 ASR 1", "hi_r1_r7_ASR_1"},
    {{ge, r2, r2, LSR, 20}, "It ge;", "ge r2 r2 LSR 20", "ge_r2_r2_LSR_20"},
    {{hi, r3, r2, LSR, 14}, "It hi;", "hi r3 r2 LSR 14", "hi_r3_r2_LSR_14"},
    {{ls, r1, r0, ASR, 1}, "It ls;", "ls r1 r0 ASR 1", "ls_r1_r0_ASR_1"},
    {{eq, r3, r7, LSR, 9}, "It eq;", "eq r3 r7 LSR 9", "eq_r3_r7_LSR_9"},
    {{eq, r0, r7, ASR, 6}, "It eq;", "eq r0 r7 ASR 6", "eq_r0_r7_ASR_6"},
    {{le, r5, r1, LSR, 6}, "It le;", "le r5 r1 LSR 6", "le_r5_r1_LSR_6"},
    {{hi, r2, r5, LSR, 1}, "It hi;", "hi r2 r5 LSR 1", "hi_r2_r5_LSR_1"},
    {{lt, r1, r2, LSR, 24}, "It lt;", "lt r1 r2 LSR 24", "lt_r1_r2_LSR_24"},
    {{hi, r2, r2, LSR, 27}, "It hi;", "hi r2 r2 LSR 27", "hi_r2_r2_LSR_27"},
    {{ls, r3, r4, ASR, 3}, "It ls;", "ls r3 r4 ASR 3", "ls_r3_r4_ASR_3"},
    {{gt, r7, r7, ASR, 8}, "It gt;", "gt r7 r7 ASR 8", "gt_r7_r7_ASR_8"},
    {{eq, r4, r2, LSR, 10}, "It eq;", "eq r4 r2 LSR 10", "eq_r4_r2_LSR_10"},
    {{lt, r6, r3, ASR, 9}, "It lt;", "lt r6 r3 ASR 9", "lt_r6_r3_ASR_9"},
    {{ge, r7, r3, LSR, 26}, "It ge;", "ge r7 r3 LSR 26", "ge_r7_r3_LSR_26"},
    {{lt, r7, r1, ASR, 1}, "It lt;", "lt r7 r1 ASR 1", "lt_r7_r1_ASR_1"},
    {{ne, r3, r5, LSR, 19}, "It ne;", "ne r3 r5 LSR 19", "ne_r3_r5_LSR_19"},
    {{ge, r5, r6, LSR, 32}, "It ge;", "ge r5 r6 LSR 32", "ge_r5_r6_LSR_32"},
    {{lt, r4, r0, ASR, 3}, "It lt;", "lt r4 r0 ASR 3", "lt_r4_r0_ASR_3"},
    {{eq, r3, r6, ASR, 26}, "It eq;", "eq r3 r6 ASR 26", "eq_r3_r6_ASR_26"},
    {{lt, r4, r7, ASR, 2}, "It lt;", "lt r4 r7 ASR 2", "lt_r4_r7_ASR_2"},
    {{pl, r7, r1, LSR, 15}, "It pl;", "pl r7 r1 LSR 15", "pl_r7_r1_LSR_15"},
    {{le, r4, r4, ASR, 4}, "It le;", "le r4 r4 ASR 4", "le_r4_r4_ASR_4"},
    {{vs, r6, r6, ASR, 22}, "It vs;", "vs r6 r6 ASR 22", "vs_r6_r6_ASR_22"},
    {{cs, r3, r6, LSR, 12}, "It cs;", "cs r3 r6 LSR 12", "cs_r3_r6_LSR_12"},
    {{eq, r4, r4, LSR, 11}, "It eq;", "eq r4 r4 LSR 11", "eq_r4_r4_LSR_11"},
    {{pl, r3, r6, LSR, 3}, "It pl;", "pl r3 r6 LSR 3", "pl_r3_r6_LSR_3"},
    {{ne, r5, r4, LSR, 13}, "It ne;", "ne r5 r4 LSR 13", "ne_r5_r4_LSR_13"},
    {{lt, r3, r2, LSR, 30}, "It lt;", "lt r3 r2 LSR 30", "lt_r3_r2_LSR_30"},
    {{hi, r7, r0, LSR, 18}, "It hi;", "hi r7 r0 LSR 18", "hi_r7_r0_LSR_18"},
    {{le, r5, r5, ASR, 8}, "It le;", "le r5 r5 ASR 8", "le_r5_r5_ASR_8"},
    {{pl, r0, r1, LSR, 21}, "It pl;", "pl r0 r1 LSR 21", "pl_r0_r1_LSR_21"},
    {{ne, r3, r5, ASR, 3}, "It ne;", "ne r3 r5 ASR 3", "ne_r3_r5_ASR_3"},
    {{gt, r2, r4, LSR, 3}, "It gt;", "gt r2 r4 LSR 3", "gt_r2_r4_LSR_3"},
    {{vs, r2, r5, ASR, 31}, "It vs;", "vs r2 r5 ASR 31", "vs_r2_r5_ASR_31"},
    {{vs, r0, r0, ASR, 13}, "It vs;", "vs r0 r0 ASR 13", "vs_r0_r0_ASR_13"},
    {{eq, r0, r7, LSR, 12}, "It eq;", "eq r0 r7 LSR 12", "eq_r0_r7_LSR_12"},
    {{cs, r3, r5, LSR, 23}, "It cs;", "cs r3 r5 LSR 23", "cs_r3_r5_LSR_23"},
    {{eq, r3, r3, ASR, 1}, "It eq;", "eq r3 r3 ASR 1", "eq_r3_r3_ASR_1"},
    {{cc, r7, r3, LSR, 29}, "It cc;", "cc r7 r3 LSR 29", "cc_r7_r3_LSR_29"},
    {{cs, r3, r7, ASR, 30}, "It cs;", "cs r3 r7 ASR 30", "cs_r3_r7_ASR_30"},
    {{lt, r6, r6, LSR, 13}, "It lt;", "lt r6 r6 LSR 13", "lt_r6_r6_LSR_13"},
    {{pl, r2, r6, LSR, 5}, "It pl;", "pl r2 r6 LSR 5", "pl_r2_r6_LSR_5"},
    {{gt, r7, r6, LSR, 23}, "It gt;", "gt r7 r6 LSR 23", "gt_r7_r6_LSR_23"},
    {{ls, r4, r6, ASR, 20}, "It ls;", "ls r4 r6 ASR 20", "ls_r4_r6_ASR_20"},
    {{mi, r7, r7, LSR, 24}, "It mi;", "mi r7 r7 LSR 24", "mi_r7_r7_LSR_24"},
    {{cc, r3, r4, ASR, 23}, "It cc;", "cc r3 r4 ASR 23", "cc_r3_r4_ASR_23"},
    {{hi, r6, r6, ASR, 31}, "It hi;", "hi r6 r6 ASR 31", "hi_r6_r6_ASR_31"},
    {{lt, r3, r2, ASR, 7}, "It lt;", "lt r3 r2 ASR 7", "lt_r3_r2_ASR_7"},
    {{gt, r4, r5, LSR, 13}, "It gt;", "gt r4 r5 LSR 13", "gt_r4_r5_LSR_13"},
    {{ge, r5, r1, ASR, 28}, "It ge;", "ge r5 r1 ASR 28", "ge_r5_r1_ASR_28"},
    {{cc, r6, r7, ASR, 1}, "It cc;", "cc r6 r7 ASR 1", "cc_r6_r7_ASR_1"},
    {{lt, r6, r6, LSR, 15}, "It lt;", "lt r6 r6 LSR 15", "lt_r6_r6_LSR_15"},
    {{pl, r7, r7, LSR, 18}, "It pl;", "pl r7 r7 LSR 18", "pl_r7_r7_LSR_18"},
    {{hi, r7, r2, LSR, 23}, "It hi;", "hi r7 r2 LSR 23", "hi_r7_r2_LSR_23"},
    {{vs, r1, r0, LSR, 3}, "It vs;", "vs r1 r0 LSR 3", "vs_r1_r0_LSR_3"},
    {{ge, r0, r2, LSR, 20}, "It ge;", "ge r0 r2 LSR 20", "ge_r0_r2_LSR_20"},
    {{pl, r0, r0, LSR, 20}, "It pl;", "pl r0 r0 LSR 20", "pl_r0_r0_LSR_20"},
    {{cc, r2, r6, LSR, 5}, "It cc;", "cc r2 r6 LSR 5", "cc_r2_r6_LSR_5"},
    {{ls, r5, r3, LSR, 32}, "It ls;", "ls r5 r3 LSR 32", "ls_r5_r3_LSR_32"},
    {{vc, r2, r6, LSR, 6}, "It vc;", "vc r2 r6 LSR 6", "vc_r2_r6_LSR_6"},
    {{ne, r4, r0, LSR, 14}, "It ne;", "ne r4 r0 LSR 14", "ne_r4_r0_LSR_14"},
    {{ls, r2, r5, ASR, 16}, "It ls;", "ls r2 r5 ASR 16", "ls_r2_r5_ASR_16"},
    {{ne, r1, r7, LSR, 11}, "It ne;", "ne r1 r7 LSR 11", "ne_r1_r7_LSR_11"},
    {{le, r3, r3, ASR, 10}, "It le;", "le r3 r3 ASR 10", "le_r3_r3_ASR_10"},
    {{vs, r6, r4, LSR, 20}, "It vs;", "vs r6 r4 LSR 20", "vs_r6_r4_LSR_20"},
    {{le, r3, r3, ASR, 2}, "It le;", "le r3 r3 ASR 2", "le_r3_r3_ASR_2"},
    {{hi, r6, r2, LSR, 24}, "It hi;", "hi r6 r2 LSR 24", "hi_r6_r2_LSR_24"},
    {{ge, r1, r6, ASR, 1}, "It ge;", "ge r1 r6 ASR 1", "ge_r1_r6_ASR_1"},
    {{ne, r2, r7, LSR, 23}, "It ne;", "ne r2 r7 LSR 23", "ne_r2_r7_LSR_23"},
    {{hi, r2, r0, ASR, 26}, "It hi;", "hi r2 r0 ASR 26", "hi_r2_r0_ASR_26"},
    {{cs, r6, r6, LSR, 11}, "It cs;", "cs r6 r6 LSR 11", "cs_r6_r6_LSR_11"},
    {{le, r0, r2, ASR, 7}, "It le;", "le r0 r2 ASR 7", "le_r0_r2_ASR_7"},
    {{mi, r6, r7, LSR, 12}, "It mi;", "mi r6 r7 LSR 12", "mi_r6_r7_LSR_12"},
    {{ne, r3, r7, ASR, 10}, "It ne;", "ne r3 r7 ASR 10", "ne_r3_r7_ASR_10"},
    {{ge, r3, r4, LSR, 16}, "It ge;", "ge r3 r4 LSR 16", "ge_r3_r4_LSR_16"},
    {{pl, r5, r4, LSR, 19}, "It pl;", "pl r5 r4 LSR 19", "pl_r5_r4_LSR_19"},
    {{lt, r3, r0, ASR, 30}, "It lt;", "lt r3 r0 ASR 30", "lt_r3_r0_ASR_30"},
    {{ge, r6, r2, LSR, 19}, "It ge;", "ge r6 r2 LSR 19", "ge_r6_r2_LSR_19"},
    {{cs, r6, r7, ASR, 29}, "It cs;", "cs r6 r7 ASR 29", "cs_r6_r7_ASR_29"},
    {{le, r5, r7, ASR, 14}, "It le;", "le r5 r7 ASR 14", "le_r5_r7_ASR_14"},
    {{le, r7, r5, LSR, 1}, "It le;", "le r7 r5 LSR 1", "le_r7_r5_LSR_1"},
    {{hi, r7, r4, LSR, 18}, "It hi;", "hi r7 r4 LSR 18", "hi_r7_r4_LSR_18"},
    {{ne, r7, r3, LSR, 13}, "It ne;", "ne r7 r3 LSR 13", "ne_r7_r3_LSR_13"},
    {{cc, r2, r0, ASR, 21}, "It cc;", "cc r2 r0 ASR 21", "cc_r2_r0_ASR_21"},
    {{ge, r2, r3, LSR, 15}, "It ge;", "ge r2 r3 LSR 15", "ge_r2_r3_LSR_15"},
    {{mi, r2, r7, ASR, 15}, "It mi;", "mi r2 r7 ASR 15", "mi_r2_r7_ASR_15"},
    {{mi, r4, r0, LSR, 19}, "It mi;", "mi r4 r0 LSR 19", "mi_r4_r0_LSR_19"},
    {{eq, r7, r3, LSR, 14}, "It eq;", "eq r7 r3 LSR 14", "eq_r7_r3_LSR_14"},
    {{cs, r0, r5, ASR, 24}, "It cs;", "cs r0 r5 ASR 24", "cs_r0_r5_ASR_24"},
    {{cs, r1, r6, ASR, 16}, "It cs;", "cs r1 r6 ASR 16", "cs_r1_r6_ASR_16"},
    {{cs, r3, r2, ASR, 32}, "It cs;", "cs r3 r2 ASR 32", "cs_r3_r2_ASR_32"},
    {{cs, r4, r5, ASR, 32}, "It cs;", "cs r4 r5 ASR 32", "cs_r4_r5_ASR_32"},
    {{mi, r2, r4, LSR, 18}, "It mi;", "mi r2 r4 LSR 18", "mi_r2_r4_LSR_18"},
    {{le, r0, r5, ASR, 18}, "It le;", "le r0 r5 ASR 18", "le_r0_r5_ASR_18"},
    {{ge, r7, r7, LSR, 17}, "It ge;", "ge r7 r7 LSR 17", "ge_r7_r7_LSR_17"},
    {{hi, r3, r1, LSR, 4}, "It hi;", "hi r3 r1 LSR 4", "hi_r3_r1_LSR_4"},
    {{hi, r0, r0, LSR, 22}, "It hi;", "hi r0 r0 LSR 22", "hi_r0_r0_LSR_22"},
    {{cs, r4, r0, LSR, 24}, "It cs;", "cs r4 r0 LSR 24", "cs_r4_r0_LSR_24"},
    {{cs, r2, r0, LSR, 17}, "It cs;", "cs r2 r0 LSR 17", "cs_r2_r0_LSR_17"},
    {{ls, r6, r4, LSR, 18}, "It ls;", "ls r6 r4 LSR 18", "ls_r6_r4_LSR_18"},
    {{pl, r1, r1, LSR, 10}, "It pl;", "pl r1 r1 LSR 10", "pl_r1_r1_LSR_10"},
    {{ne, r5, r2, ASR, 26}, "It ne;", "ne r5 r2 ASR 26", "ne_r5_r2_ASR_26"},
    {{vs, r0, r4, ASR, 21}, "It vs;", "vs r0 r4 ASR 21", "vs_r0_r4_ASR_21"},
    {{ls, r4, r6, ASR, 14}, "It ls;", "ls r4 r6 ASR 14", "ls_r4_r6_ASR_14"},
    {{ge, r1, r6, ASR, 2}, "It ge;", "ge r1 r6 ASR 2", "ge_r1_r6_ASR_2"},
    {{hi, r5, r1, LSR, 7}, "It hi;", "hi r5 r1 LSR 7", "hi_r5_r1_LSR_7"},
    {{mi, r0, r2, ASR, 22}, "It mi;", "mi r0 r2 ASR 22", "mi_r0_r2_ASR_22"},
    {{gt, r0, r7, LSR, 7}, "It gt;", "gt r0 r7 LSR 7", "gt_r0_r7_LSR_7"},
    {{ge, r6, r0, LSR, 4}, "It ge;", "ge r6 r0 LSR 4", "ge_r6_r0_LSR_4"},
    {{ne, r1, r3, ASR, 14}, "It ne;", "ne r1 r3 ASR 14", "ne_r1_r3_ASR_14"},
    {{mi, r6, r1, LSR, 29}, "It mi;", "mi r6 r1 LSR 29", "mi_r6_r1_LSR_29"},
    {{gt, r5, r2, LSR, 19}, "It gt;", "gt r5 r2 LSR 19", "gt_r5_r2_LSR_19"},
    {{ge, r5, r4, ASR, 3}, "It ge;", "ge r5 r4 ASR 3", "ge_r5_r4_ASR_3"},
    {{le, r6, r2, ASR, 9}, "It le;", "le r6 r2 ASR 9", "le_r6_r2_ASR_9"},
    {{cc, r3, r5, LSR, 1}, "It cc;", "cc r3 r5 LSR 1", "cc_r3_r5_LSR_1"},
    {{ne, r3, r3, ASR, 3}, "It ne;", "ne r3 r3 ASR 3", "ne_r3_r3_ASR_3"},
    {{ge, r6, r0, ASR, 14}, "It ge;", "ge r6 r0 ASR 14", "ge_r6_r0_ASR_14"},
    {{cc, r3, r1, ASR, 20}, "It cc;", "cc r3 r1 ASR 20", "cc_r3_r1_ASR_20"},
    {{gt, r6, r2, LSR, 28}, "It gt;", "gt r6 r2 LSR 28", "gt_r6_r2_LSR_28"},
    {{ge, r1, r3, LSR, 6}, "It ge;", "ge r1 r3 LSR 6", "ge_r1_r3_LSR_6"},
    {{cs, r6, r1, LSR, 20}, "It cs;", "cs r6 r1 LSR 20", "cs_r6_r1_LSR_20"},
    {{vs, r7, r1, ASR, 14}, "It vs;", "vs r7 r1 ASR 14", "vs_r7_r1_ASR_14"},
    {{ls, r1, r1, ASR, 6}, "It ls;", "ls r1 r1 ASR 6", "ls_r1_r1_ASR_6"},
    {{gt, r1, r2, LSR, 3}, "It gt;", "gt r1 r2 LSR 3", "gt_r1_r2_LSR_3"},
    {{ne, r0, r5, LSR, 4}, "It ne;", "ne r0 r5 LSR 4", "ne_r0_r5_LSR_4"},
    {{cc, r7, r4, LSR, 1}, "It cc;", "cc r7 r4 LSR 1", "cc_r7_r4_LSR_1"},
    {{ge, r4, r3, LSR, 14}, "It ge;", "ge r4 r3 LSR 14", "ge_r4_r3_LSR_14"},
    {{ne, r6, r6, LSR, 15}, "It ne;", "ne r6 r6 LSR 15", "ne_r6_r6_LSR_15"},
    {{cs, r4, r4, LSR, 15}, "It cs;", "cs r4 r4 LSR 15", "cs_r4_r4_LSR_15"},
    {{vc, r7, r0, LSR, 8}, "It vc;", "vc r7 r0 LSR 8", "vc_r7_r0_LSR_8"},
    {{cc, r6, r0, ASR, 27}, "It cc;", "cc r6 r0 ASR 27", "cc_r6_r0_ASR_27"},
    {{eq, r0, r6, ASR, 25}, "It eq;", "eq r0 r6 ASR 25", "eq_r0_r6_ASR_25"},
    {{hi, r5, r1, LSR, 18}, "It hi;", "hi r5 r1 LSR 18", "hi_r5_r1_LSR_18"},
    {{hi, r4, r6, ASR, 19}, "It hi;", "hi r4 r6 ASR 19", "hi_r4_r6_ASR_19"},
    {{eq, r6, r1, LSR, 4}, "It eq;", "eq r6 r1 LSR 4", "eq_r6_r1_LSR_4"},
    {{le, r4, r5, LSR, 16}, "It le;", "le r4 r5 LSR 16", "le_r4_r5_LSR_16"},
    {{hi, r1, r6, ASR, 15}, "It hi;", "hi r1 r6 ASR 15", "hi_r1_r6_ASR_15"},
    {{hi, r0, r0, ASR, 11}, "It hi;", "hi r0 r0 ASR 11", "hi_r0_r0_ASR_11"},
    {{cs, r4, r2, LSR, 23}, "It cs;", "cs r4 r2 LSR 23", "cs_r4_r2_LSR_23"},
    {{ls, r0, r3, LSR, 9}, "It ls;", "ls r0 r3 LSR 9", "ls_r0_r3_LSR_9"},
    {{hi, r1, r2, LSR, 9}, "It hi;", "hi r1 r2 LSR 9", "hi_r1_r2_LSR_9"},
    {{ge, r5, r2, LSR, 32}, "It ge;", "ge r5 r2 LSR 32", "ge_r5_r2_LSR_32"},
    {{ne, r0, r6, LSR, 23}, "It ne;", "ne r0 r6 LSR 23", "ne_r0_r6_LSR_23"},
    {{lt, r3, r4, LSR, 3}, "It lt;", "lt r3 r4 LSR 3", "lt_r3_r4_LSR_3"},
    {{gt, r1, r6, LSR, 32}, "It gt;", "gt r1 r6 LSR 32", "gt_r1_r6_LSR_32"},
    {{vc, r0, r4, ASR, 32}, "It vc;", "vc r0 r4 ASR 32", "vc_r0_r4_ASR_32"},
    {{ne, r7, r1, ASR, 14}, "It ne;", "ne r7 r1 ASR 14", "ne_r7_r1_ASR_14"},
    {{cs, r2, r0, ASR, 19}, "It cs;", "cs r2 r0 ASR 19", "cs_r2_r0_ASR_19"},
    {{eq, r3, r4, LSR, 24}, "It eq;", "eq r3 r4 LSR 24", "eq_r3_r4_LSR_24"},
    {{ge, r1, r2, LSR, 10}, "It ge;", "ge r1 r2 LSR 10", "ge_r1_r2_LSR_10"},
    {{eq, r4, r5, ASR, 30}, "It eq;", "eq r4 r5 ASR 30", "eq_r4_r5_ASR_30"},
    {{le, r5, r0, ASR, 3}, "It le;", "le r5 r0 ASR 3", "le_r5_r0_ASR_3"},
    {{ls, r2, r2, LSR, 7}, "It ls;", "ls r2 r2 LSR 7", "ls_r2_r2_LSR_7"},
    {{le, r1, r1, LSR, 24}, "It le;", "le r1 r1 LSR 24", "le_r1_r1_LSR_24"},
    {{le, r6, r6, ASR, 12}, "It le;", "le r6 r6 ASR 12", "le_r6_r6_ASR_12"},
    {{vc, r7, r6, ASR, 19}, "It vc;", "vc r7 r6 ASR 19", "vc_r7_r6_ASR_19"},
    {{mi, r4, r1, ASR, 16}, "It mi;", "mi r4 r1 ASR 16", "mi_r4_r1_ASR_16"},
    {{ne, r7, r4, ASR, 9}, "It ne;", "ne r7 r4 ASR 9", "ne_r7_r4_ASR_9"},
    {{lt, r6, r5, LSR, 23}, "It lt;", "lt r6 r5 LSR 23", "lt_r6_r5_LSR_23"},
    {{ls, r0, r4, LSR, 13}, "It ls;", "ls r0 r4 LSR 13", "ls_r0_r4_LSR_13"},
    {{cs, r3, r2, LSR, 8}, "It cs;", "cs r3 r2 LSR 8", "cs_r3_r2_LSR_8"},
    {{ls, r3, r2, LSR, 17}, "It ls;", "ls r3 r2 LSR 17", "ls_r3_r2_LSR_17"},
    {{lt, r6, r0, ASR, 5}, "It lt;", "lt r6 r0 ASR 5", "lt_r6_r0_ASR_5"},
    {{gt, r1, r1, ASR, 7}, "It gt;", "gt r1 r1 ASR 7", "gt_r1_r1_ASR_7"},
    {{ls, r2, r3, LSR, 3}, "It ls;", "ls r2 r3 LSR 3", "ls_r2_r3_LSR_3"},
    {{lt, r2, r0, LSR, 5}, "It lt;", "lt r2 r0 LSR 5", "lt_r2_r0_LSR_5"},
    {{mi, r2, r4, ASR, 3}, "It mi;", "mi r2 r4 ASR 3", "mi_r2_r4_ASR_3"},
    {{vs, r6, r0, ASR, 20}, "It vs;", "vs r6 r0 ASR 20", "vs_r6_r0_ASR_20"},
    {{cc, r4, r5, LSR, 12}, "It cc;", "cc r4 r5 LSR 12", "cc_r4_r5_LSR_12"},
    {{cc, r6, r5, ASR, 12}, "It cc;", "cc r6 r5 ASR 12", "cc_r6_r5_ASR_12"},
    {{ls, r4, r3, LSR, 19}, "It ls;", "ls r4 r3 LSR 19", "ls_r4_r3_LSR_19"},
    {{cc, r5, r3, ASR, 1}, "It cc;", "cc r5 r3 ASR 1", "cc_r5_r3_ASR_1"},
    {{hi, r1, r5, ASR, 31}, "It hi;", "hi r1 r5 ASR 31", "hi_r1_r5_ASR_31"},
    {{hi, r0, r6, LSR, 21}, "It hi;", "hi r0 r6 LSR 21", "hi_r0_r6_LSR_21"},
    {{vc, r2, r6, ASR, 27}, "It vc;", "vc r2 r6 ASR 27", "vc_r2_r6_ASR_27"},
    {{ge, r1, r2, LSR, 30}, "It ge;", "ge r1 r2 LSR 30", "ge_r1_r2_LSR_30"},
    {{gt, r1, r5, ASR, 12}, "It gt;", "gt r1 r5 ASR 12", "gt_r1_r5_ASR_12"},
    {{pl, r0, r5, LSR, 23}, "It pl;", "pl r0 r5 LSR 23", "pl_r0_r5_LSR_23"},
    {{lt, r7, r6, ASR, 13}, "It lt;", "lt r7 r6 ASR 13", "lt_r7_r6_ASR_13"},
    {{vc, r2, r3, ASR, 6}, "It vc;", "vc r2 r3 ASR 6", "vc_r2_r3_ASR_6"},
    {{ge, r5, r4, LSR, 9}, "It ge;", "ge r5 r4 LSR 9", "ge_r5_r4_LSR_9"},
    {{le, r5, r5, LSR, 4}, "It le;", "le r5 r5 LSR 4", "le_r5_r5_LSR_4"},
    {{mi, r7, r5, ASR, 32}, "It mi;", "mi r7 r5 ASR 32", "mi_r7_r5_ASR_32"},
    {{pl, r1, r0, LSR, 30}, "It pl;", "pl r1 r0 LSR 30", "pl_r1_r0_LSR_30"},
    {{eq, r1, r0, LSR, 30}, "It eq;", "eq r1 r0 LSR 30", "eq_r1_r0_LSR_30"},
    {{vc, r0, r3, LSR, 31}, "It vc;", "vc r0 r3 LSR 31", "vc_r0_r3_LSR_31"},
    {{eq, r1, r4, LSR, 7}, "It eq;", "eq r1 r4 LSR 7", "eq_r1_r4_LSR_7"},
    {{hi, r4, r3, LSR, 14}, "It hi;", "hi r4 r3 LSR 14", "hi_r4_r3_LSR_14"},
    {{gt, r7, r7, LSR, 10}, "It gt;", "gt r7 r7 LSR 10", "gt_r7_r7_LSR_10"},
    {{vc, r0, r2, ASR, 32}, "It vc;", "vc r0 r2 ASR 32", "vc_r0_r2_ASR_32"},
    {{cs, r4, r4, ASR, 29}, "It cs;", "cs r4 r4 ASR 29", "cs_r4_r4_ASR_29"},
    {{mi, r2, r3, ASR, 31}, "It mi;", "mi r2 r3 ASR 31", "mi_r2_r3_ASR_31"},
    {{gt, r7, r2, LSR, 6}, "It gt;", "gt r7 r2 LSR 6", "gt_r7_r2_LSR_6"},
    {{ls, r7, r7, LSR, 32}, "It ls;", "ls r7 r7 LSR 32", "ls_r7_r7_LSR_32"},
    {{le, r5, r5, LSR, 24}, "It le;", "le r5 r5 LSR 24", "le_r5_r5_LSR_24"},
    {{hi, r4, r2, ASR, 24}, "It hi;", "hi r4 r2 ASR 24", "hi_r4_r2_ASR_24"},
    {{ne, r2, r2, ASR, 30}, "It ne;", "ne r2 r2 ASR 30", "ne_r2_r2_ASR_30"},
    {{pl, r3, r1, LSR, 27}, "It pl;", "pl r3 r1 LSR 27", "pl_r3_r1_LSR_27"},
    {{vs, r4, r4, LSR, 30}, "It vs;", "vs r4 r4 LSR 30", "vs_r4_r4_LSR_30"},
    {{pl, r2, r7, ASR, 23}, "It pl;", "pl r2 r7 ASR 23", "pl_r2_r7_ASR_23"},
    {{ge, r6, r2, ASR, 18}, "It ge;", "ge r6 r2 ASR 18", "ge_r6_r2_ASR_18"},
    {{pl, r5, r5, LSR, 22}, "It pl;", "pl r5 r5 LSR 22", "pl_r5_r5_LSR_22"},
    {{lt, r2, r0, ASR, 7}, "It lt;", "lt r2 r0 ASR 7", "lt_r2_r0_ASR_7"},
    {{gt, r6, r1, LSR, 23}, "It gt;", "gt r6 r1 LSR 23", "gt_r6_r1_LSR_23"},
    {{gt, r3, r0, ASR, 23}, "It gt;", "gt r3 r0 ASR 23", "gt_r3_r0_ASR_23"},
    {{lt, r2, r0, LSR, 11}, "It lt;", "lt r2 r0 LSR 11", "lt_r2_r0_LSR_11"},
    {{vs, r4, r5, ASR, 26}, "It vs;", "vs r4 r5 ASR 26", "vs_r4_r5_ASR_26"},
    {{eq, r4, r2, ASR, 10}, "It eq;", "eq r4 r2 ASR 10", "eq_r4_r2_ASR_10"},
    {{cc, r7, r3, ASR, 21}, "It cc;", "cc r7 r3 ASR 21", "cc_r7_r3_ASR_21"},
    {{le, r1, r1, LSR, 26}, "It le;", "le r1 r1 LSR 26", "le_r1_r1_LSR_26"},
    {{ge, r4, r5, ASR, 14}, "It ge;", "ge r4 r5 ASR 14", "ge_r4_r5_ASR_14"},
    {{le, r4, r6, ASR, 25}, "It le;", "le r4 r6 ASR 25", "le_r4_r6_ASR_25"},
    {{pl, r0, r3, ASR, 11}, "It pl;", "pl r0 r3 ASR 11", "pl_r0_r3_ASR_11"},
    {{hi, r5, r2, LSR, 15}, "It hi;", "hi r5 r2 LSR 15", "hi_r5_r2_LSR_15"},
    {{eq, r3, r7, LSR, 14}, "It eq;", "eq r3 r7 LSR 14", "eq_r3_r7_LSR_14"},
    {{ne, r3, r5, ASR, 25}, "It ne;", "ne r3 r5 ASR 25", "ne_r3_r5_ASR_25"},
    {{gt, r6, r3, ASR, 6}, "It gt;", "gt r6 r3 ASR 6", "gt_r6_r3_ASR_6"},
    {{vs, r2, r4, LSR, 21}, "It vs;", "vs r2 r4 LSR 21", "vs_r2_r4_LSR_21"},
    {{lt, r1, r7, LSR, 24}, "It lt;", "lt r1 r7 LSR 24", "lt_r1_r7_LSR_24"},
    {{hi, r4, r5, LSR, 14}, "It hi;", "hi r4 r5 LSR 14", "hi_r4_r5_LSR_14"},
    {{le, r4, r0, LSR, 25}, "It le;", "le r4 r0 LSR 25", "le_r4_r0_LSR_25"},
    {{ls, r2, r0, ASR, 14}, "It ls;", "ls r2 r0 ASR 14", "ls_r2_r0_ASR_14"},
    {{le, r3, r1, LSR, 27}, "It le;", "le r3 r1 LSR 27", "le_r3_r1_LSR_27"},
    {{ne, r5, r4, ASR, 30}, "It ne;", "ne r5 r4 ASR 30", "ne_r5_r4_ASR_30"},
    {{cc, r0, r0, ASR, 31}, "It cc;", "cc r0 r0 ASR 31", "cc_r0_r0_ASR_31"},
    {{mi, r6, r0, ASR, 16}, "It mi;", "mi r6 r0 ASR 16", "mi_r6_r0_ASR_16"},
    {{mi, r1, r3, ASR, 8}, "It mi;", "mi r1 r3 ASR 8", "mi_r1_r3_ASR_8"},
    {{ge, r7, r1, ASR, 6}, "It ge;", "ge r7 r1 ASR 6", "ge_r7_r1_ASR_6"},
    {{ge, r6, r0, LSR, 30}, "It ge;", "ge r6 r0 LSR 30", "ge_r6_r0_LSR_30"},
    {{pl, r7, r3, LSR, 32}, "It pl;", "pl r7 r3 LSR 32", "pl_r7_r3_LSR_32"},
    {{cs, r1, r6, LSR, 13}, "It cs;", "cs r1 r6 LSR 13", "cs_r1_r6_LSR_13"}};

// These headers each contain an array of `TestResult` with the reference output
// values. The reference arrays are names `kReference{mnemonic}`.
#include "aarch32/traces/assembler-cond-rd-operand-rn-shift-amount-1to32-t32-in-it-block-mov.h"

// The maximum number of errors to report in detail for each test.
static const unsigned kErrorReportLimit = 8;

typedef void (MacroAssembler::*Fn)(Condition cond, Register rd,
                                   const Operand& op);

static void TestHelper(Fn instruction, const char* mnemonic,
                       const TestResult reference[]) {
  unsigned total_error_count = 0;
  MacroAssembler masm(BUF_SIZE);

  masm.UseT32();

  for (unsigned i = 0; i < ARRAY_SIZE(kTests); i++) {
    // Values to pass to the macro-assembler.
    Condition cond = kTests[i].operands.cond;
    Register rd = kTests[i].operands.rd;
    Register rn = kTests[i].operands.rn;
    ShiftType shift = kTests[i].operands.shift;
    uint32_t amount = kTests[i].operands.amount;
    Operand op(rn, shift, amount);

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
      "AARCH32_ASSEMBLER_COND_RD_OPERAND_RN_SHIFT_AMOUNT_1TO32_T32_IN_IT_"  \
      "BLOCK_" #mnemonic,                                                   \
      &Test_##mnemonic);
FOREACH_INSTRUCTION(TEST)
#undef TEST

}  // aarch32
}  // vixl
