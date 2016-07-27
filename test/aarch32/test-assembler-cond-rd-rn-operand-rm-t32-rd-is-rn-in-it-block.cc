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
#define FOREACH_INSTRUCTION(M) M(Add)

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
    {{ls, r9, r9, r9}, "It ls;", "ls r9 r9 r9", "ls_r9_r9_r9"},
    {{cc, r1, r1, r9}, "It cc;", "cc r1 r1 r9", "cc_r1_r1_r9"},
    {{cc, r4, r4, r9}, "It cc;", "cc r4 r4 r9", "cc_r4_r4_r9"},
    {{vs, r13, r13, r3}, "It vs;", "vs r13 r13 r3", "vs_r13_r13_r3"},
    {{hi, r5, r5, r11}, "It hi;", "hi r5 r5 r11", "hi_r5_r5_r11"},
    {{le, r13, r13, r6}, "It le;", "le r13 r13 r6", "le_r13_r13_r6"},
    {{mi, r2, r2, r12}, "It mi;", "mi r2 r2 r12", "mi_r2_r2_r12"},
    {{hi, r10, r10, r14}, "It hi;", "hi r10 r10 r14", "hi_r10_r10_r14"},
    {{mi, r1, r1, r5}, "It mi;", "mi r1 r1 r5", "mi_r1_r1_r5"},
    {{vc, r10, r10, r8}, "It vc;", "vc r10 r10 r8", "vc_r10_r10_r8"},
    {{ge, r7, r7, r7}, "It ge;", "ge r7 r7 r7", "ge_r7_r7_r7"},
    {{eq, r14, r14, r3}, "It eq;", "eq r14 r14 r3", "eq_r14_r14_r3"},
    {{ge, r1, r1, r6}, "It ge;", "ge r1 r1 r6", "ge_r1_r1_r6"},
    {{cc, r8, r8, r14}, "It cc;", "cc r8 r8 r14", "cc_r8_r8_r14"},
    {{lt, r9, r9, r11}, "It lt;", "lt r9 r9 r11", "lt_r9_r9_r11"},
    {{vs, r1, r1, r0}, "It vs;", "vs r1 r1 r0", "vs_r1_r1_r0"},
    {{cc, r14, r14, r1}, "It cc;", "cc r14 r14 r1", "cc_r14_r14_r1"},
    {{cs, r8, r8, r1}, "It cs;", "cs r8 r8 r1", "cs_r8_r8_r1"},
    {{gt, r11, r11, r9}, "It gt;", "gt r11 r11 r9", "gt_r11_r11_r9"},
    {{hi, r9, r9, r2}, "It hi;", "hi r9 r9 r2", "hi_r9_r9_r2"},
    {{ne, r7, r7, r4}, "It ne;", "ne r7 r7 r4", "ne_r7_r7_r4"},
    {{ge, r0, r0, r14}, "It ge;", "ge r0 r0 r14", "ge_r0_r0_r14"},
    {{eq, r10, r10, r12}, "It eq;", "eq r10 r10 r12", "eq_r10_r10_r12"},
    {{lt, r0, r0, r12}, "It lt;", "lt r0 r0 r12", "lt_r0_r0_r12"},
    {{lt, r3, r3, r7}, "It lt;", "lt r3 r3 r7", "lt_r3_r3_r7"},
    {{vs, r1, r1, r8}, "It vs;", "vs r1 r1 r8", "vs_r1_r1_r8"},
    {{cs, r5, r5, r10}, "It cs;", "cs r5 r5 r10", "cs_r5_r5_r10"},
    {{gt, r0, r0, r2}, "It gt;", "gt r0 r0 r2", "gt_r0_r0_r2"},
    {{cs, r1, r1, r6}, "It cs;", "cs r1 r1 r6", "cs_r1_r1_r6"},
    {{ls, r3, r3, r6}, "It ls;", "ls r3 r3 r6", "ls_r3_r3_r6"},
    {{vc, r2, r2, r8}, "It vc;", "vc r2 r2 r8", "vc_r2_r2_r8"},
    {{mi, r7, r7, r0}, "It mi;", "mi r7 r7 r0", "mi_r7_r7_r0"},
    {{pl, r9, r9, r7}, "It pl;", "pl r9 r9 r7", "pl_r9_r9_r7"},
    {{cs, r0, r0, r12}, "It cs;", "cs r0 r0 r12", "cs_r0_r0_r12"},
    {{gt, r3, r3, r8}, "It gt;", "gt r3 r3 r8", "gt_r3_r3_r8"},
    {{le, r14, r14, r2}, "It le;", "le r14 r14 r2", "le_r14_r14_r2"},
    {{pl, r0, r0, r6}, "It pl;", "pl r0 r0 r6", "pl_r0_r0_r6"},
    {{mi, r5, r5, r12}, "It mi;", "mi r5 r5 r12", "mi_r5_r5_r12"},
    {{vs, r1, r1, r6}, "It vs;", "vs r1 r1 r6", "vs_r1_r1_r6"},
    {{cs, r4, r4, r0}, "It cs;", "cs r4 r4 r0", "cs_r4_r4_r0"},
    {{ge, r8, r8, r6}, "It ge;", "ge r8 r8 r6", "ge_r8_r8_r6"},
    {{vc, r8, r8, r3}, "It vc;", "vc r8 r8 r3", "vc_r8_r8_r3"},
    {{vc, r9, r9, r7}, "It vc;", "vc r9 r9 r7", "vc_r9_r9_r7"},
    {{eq, r5, r5, r9}, "It eq;", "eq r5 r5 r9", "eq_r5_r5_r9"},
    {{ge, r12, r12, r8}, "It ge;", "ge r12 r12 r8", "ge_r12_r12_r8"},
    {{vs, r8, r8, r7}, "It vs;", "vs r8 r8 r7", "vs_r8_r8_r7"},
    {{pl, r0, r0, r4}, "It pl;", "pl r0 r0 r4", "pl_r0_r0_r4"},
    {{gt, r0, r0, r1}, "It gt;", "gt r0 r0 r1", "gt_r0_r0_r1"},
    {{ne, r13, r13, r10}, "It ne;", "ne r13 r13 r10", "ne_r13_r13_r10"},
    {{ne, r8, r8, r14}, "It ne;", "ne r8 r8 r14", "ne_r8_r8_r14"},
    {{cs, r2, r2, r1}, "It cs;", "cs r2 r2 r1", "cs_r2_r2_r1"},
    {{vs, r7, r7, r3}, "It vs;", "vs r7 r7 r3", "vs_r7_r7_r3"},
    {{le, r1, r1, r14}, "It le;", "le r1 r1 r14", "le_r1_r1_r14"},
    {{eq, r4, r4, r4}, "It eq;", "eq r4 r4 r4", "eq_r4_r4_r4"},
    {{ls, r1, r1, r4}, "It ls;", "ls r1 r1 r4", "ls_r1_r1_r4"},
    {{vs, r7, r7, r0}, "It vs;", "vs r7 r7 r0", "vs_r7_r7_r0"},
    {{pl, r5, r5, r10}, "It pl;", "pl r5 r5 r10", "pl_r5_r5_r10"},
    {{eq, r8, r8, r1}, "It eq;", "eq r8 r8 r1", "eq_r8_r8_r1"},
    {{pl, r8, r8, r10}, "It pl;", "pl r8 r8 r10", "pl_r8_r8_r10"},
    {{mi, r13, r13, r11}, "It mi;", "mi r13 r13 r11", "mi_r13_r13_r11"},
    {{lt, r6, r6, r0}, "It lt;", "lt r6 r6 r0", "lt_r6_r6_r0"},
    {{vs, r4, r4, r1}, "It vs;", "vs r4 r4 r1", "vs_r4_r4_r1"},
    {{vc, r13, r13, r6}, "It vc;", "vc r13 r13 r6", "vc_r13_r13_r6"},
    {{ge, r12, r12, r12}, "It ge;", "ge r12 r12 r12", "ge_r12_r12_r12"},
    {{cc, r6, r6, r14}, "It cc;", "cc r6 r6 r14", "cc_r6_r6_r14"},
    {{cc, r10, r10, r14}, "It cc;", "cc r10 r10 r14", "cc_r10_r10_r14"},
    {{cs, r9, r9, r10}, "It cs;", "cs r9 r9 r10", "cs_r9_r9_r10"},
    {{ge, r10, r10, r3}, "It ge;", "ge r10 r10 r3", "ge_r10_r10_r3"},
    {{cc, r1, r1, r3}, "It cc;", "cc r1 r1 r3", "cc_r1_r1_r3"},
    {{vs, r11, r11, r12}, "It vs;", "vs r11 r11 r12", "vs_r11_r11_r12"},
    {{cs, r10, r10, r14}, "It cs;", "cs r10 r10 r14", "cs_r10_r10_r14"},
    {{lt, r5, r5, r11}, "It lt;", "lt r5 r5 r11", "lt_r5_r5_r11"},
    {{ge, r6, r6, r9}, "It ge;", "ge r6 r6 r9", "ge_r6_r6_r9"},
    {{ne, r2, r2, r14}, "It ne;", "ne r2 r2 r14", "ne_r2_r2_r14"},
    {{ne, r0, r0, r8}, "It ne;", "ne r0 r0 r8", "ne_r0_r0_r8"},
    {{cs, r3, r3, r14}, "It cs;", "cs r3 r3 r14", "cs_r3_r3_r14"},
    {{vc, r14, r14, r5}, "It vc;", "vc r14 r14 r5", "vc_r14_r14_r5"},
    {{pl, r11, r11, r4}, "It pl;", "pl r11 r11 r4", "pl_r11_r11_r4"},
    {{ne, r13, r13, r9}, "It ne;", "ne r13 r13 r9", "ne_r13_r13_r9"},
    {{ls, r11, r11, r6}, "It ls;", "ls r11 r11 r6", "ls_r11_r11_r6"},
    {{vs, r2, r2, r9}, "It vs;", "vs r2 r2 r9", "vs_r2_r2_r9"},
    {{ge, r6, r6, r8}, "It ge;", "ge r6 r6 r8", "ge_r6_r6_r8"},
    {{ls, r7, r7, r6}, "It ls;", "ls r7 r7 r6", "ls_r7_r7_r6"},
    {{mi, r10, r10, r5}, "It mi;", "mi r10 r10 r5", "mi_r10_r10_r5"},
    {{ls, r1, r1, r6}, "It ls;", "ls r1 r1 r6", "ls_r1_r1_r6"},
    {{le, r13, r13, r4}, "It le;", "le r13 r13 r4", "le_r13_r13_r4"},
    {{le, r0, r0, r5}, "It le;", "le r0 r0 r5", "le_r0_r0_r5"},
    {{ls, r4, r4, r12}, "It ls;", "ls r4 r4 r12", "ls_r4_r4_r12"},
    {{pl, r7, r7, r2}, "It pl;", "pl r7 r7 r2", "pl_r7_r7_r2"},
    {{hi, r3, r3, r9}, "It hi;", "hi r3 r3 r9", "hi_r3_r3_r9"},
    {{mi, r4, r4, r9}, "It mi;", "mi r4 r4 r9", "mi_r4_r4_r9"},
    {{lt, r4, r4, r4}, "It lt;", "lt r4 r4 r4", "lt_r4_r4_r4"},
    {{eq, r12, r12, r12}, "It eq;", "eq r12 r12 r12", "eq_r12_r12_r12"},
    {{cc, r5, r5, r4}, "It cc;", "cc r5 r5 r4", "cc_r5_r5_r4"},
    {{eq, r12, r12, r7}, "It eq;", "eq r12 r12 r7", "eq_r12_r12_r7"},
    {{eq, r7, r7, r5}, "It eq;", "eq r7 r7 r5", "eq_r7_r7_r5"},
    {{lt, r14, r14, r2}, "It lt;", "lt r14 r14 r2", "lt_r14_r14_r2"},
    {{mi, r12, r12, r2}, "It mi;", "mi r12 r12 r2", "mi_r12_r12_r2"},
    {{gt, r12, r12, r14}, "It gt;", "gt r12 r12 r14", "gt_r12_r12_r14"},
    {{mi, r9, r9, r4}, "It mi;", "mi r9 r9 r4", "mi_r9_r9_r4"},
    {{mi, r5, r5, r10}, "It mi;", "mi r5 r5 r10", "mi_r5_r5_r10"},
    {{ne, r3, r3, r14}, "It ne;", "ne r3 r3 r14", "ne_r3_r3_r14"},
    {{lt, r10, r10, r4}, "It lt;", "lt r10 r10 r4", "lt_r10_r10_r4"},
    {{cc, r0, r0, r12}, "It cc;", "cc r0 r0 r12", "cc_r0_r0_r12"},
    {{ge, r1, r1, r10}, "It ge;", "ge r1 r1 r10", "ge_r1_r1_r10"},
    {{gt, r11, r11, r10}, "It gt;", "gt r11 r11 r10", "gt_r11_r11_r10"},
    {{vs, r10, r10, r3}, "It vs;", "vs r10 r10 r3", "vs_r10_r10_r3"},
    {{cs, r6, r6, r5}, "It cs;", "cs r6 r6 r5", "cs_r6_r6_r5"},
    {{lt, r13, r13, r3}, "It lt;", "lt r13 r13 r3", "lt_r13_r13_r3"},
    {{cs, r6, r6, r4}, "It cs;", "cs r6 r6 r4", "cs_r6_r6_r4"},
    {{vs, r1, r1, r7}, "It vs;", "vs r1 r1 r7", "vs_r1_r1_r7"},
    {{eq, r3, r3, r12}, "It eq;", "eq r3 r3 r12", "eq_r3_r3_r12"},
    {{ls, r5, r5, r2}, "It ls;", "ls r5 r5 r2", "ls_r5_r5_r2"},
    {{vs, r4, r4, r12}, "It vs;", "vs r4 r4 r12", "vs_r4_r4_r12"},
    {{mi, r5, r5, r11}, "It mi;", "mi r5 r5 r11", "mi_r5_r5_r11"},
    {{le, r9, r9, r12}, "It le;", "le r9 r9 r12", "le_r9_r9_r12"},
    {{hi, r9, r9, r9}, "It hi;", "hi r9 r9 r9", "hi_r9_r9_r9"},
    {{ne, r11, r11, r5}, "It ne;", "ne r11 r11 r5", "ne_r11_r11_r5"},
    {{gt, r5, r5, r1}, "It gt;", "gt r5 r5 r1", "gt_r5_r5_r1"},
    {{ne, r12, r12, r3}, "It ne;", "ne r12 r12 r3", "ne_r12_r12_r3"},
    {{pl, r8, r8, r1}, "It pl;", "pl r8 r8 r1", "pl_r8_r8_r1"},
    {{ge, r6, r6, r5}, "It ge;", "ge r6 r6 r5", "ge_r6_r6_r5"},
    {{vs, r8, r8, r9}, "It vs;", "vs r8 r8 r9", "vs_r8_r8_r9"},
    {{vc, r14, r14, r11}, "It vc;", "vc r14 r14 r11", "vc_r14_r14_r11"},
    {{gt, r5, r5, r0}, "It gt;", "gt r5 r5 r0", "gt_r5_r5_r0"},
    {{gt, r2, r2, r1}, "It gt;", "gt r2 r2 r1", "gt_r2_r2_r1"},
    {{pl, r6, r6, r0}, "It pl;", "pl r6 r6 r0", "pl_r6_r6_r0"},
    {{cc, r7, r7, r2}, "It cc;", "cc r7 r7 r2", "cc_r7_r7_r2"},
    {{vc, r7, r7, r12}, "It vc;", "vc r7 r7 r12", "vc_r7_r7_r12"},
    {{cc, r13, r13, r6}, "It cc;", "cc r13 r13 r6", "cc_r13_r13_r6"},
    {{vs, r5, r5, r10}, "It vs;", "vs r5 r5 r10", "vs_r5_r5_r10"},
    {{vc, r12, r12, r3}, "It vc;", "vc r12 r12 r3", "vc_r12_r12_r3"},
    {{ge, r0, r0, r4}, "It ge;", "ge r0 r0 r4", "ge_r0_r0_r4"},
    {{eq, r2, r2, r12}, "It eq;", "eq r2 r2 r12", "eq_r2_r2_r12"},
    {{cc, r1, r1, r2}, "It cc;", "cc r1 r1 r2", "cc_r1_r1_r2"},
    {{cs, r4, r4, r8}, "It cs;", "cs r4 r4 r8", "cs_r4_r4_r8"},
    {{ls, r5, r5, r6}, "It ls;", "ls r5 r5 r6", "ls_r5_r5_r6"},
    {{pl, r12, r12, r8}, "It pl;", "pl r12 r12 r8", "pl_r12_r12_r8"},
    {{ls, r12, r12, r0}, "It ls;", "ls r12 r12 r0", "ls_r12_r12_r0"},
    {{le, r8, r8, r1}, "It le;", "le r8 r8 r1", "le_r8_r8_r1"},
    {{pl, r12, r12, r7}, "It pl;", "pl r12 r12 r7", "pl_r12_r12_r7"},
    {{cc, r7, r7, r4}, "It cc;", "cc r7 r7 r4", "cc_r7_r7_r4"},
    {{hi, r12, r12, r9}, "It hi;", "hi r12 r12 r9", "hi_r12_r12_r9"},
    {{pl, r4, r4, r7}, "It pl;", "pl r4 r4 r7", "pl_r4_r4_r7"},
    {{cc, r14, r14, r8}, "It cc;", "cc r14 r14 r8", "cc_r14_r14_r8"},
    {{hi, r14, r14, r14}, "It hi;", "hi r14 r14 r14", "hi_r14_r14_r14"},
    {{pl, r5, r5, r7}, "It pl;", "pl r5 r5 r7", "pl_r5_r5_r7"},
    {{le, r5, r5, r11}, "It le;", "le r5 r5 r11", "le_r5_r5_r11"},
    {{eq, r8, r8, r0}, "It eq;", "eq r8 r8 r0", "eq_r8_r8_r0"},
    {{hi, r8, r8, r6}, "It hi;", "hi r8 r8 r6", "hi_r8_r8_r6"},
    {{hi, r2, r2, r7}, "It hi;", "hi r2 r2 r7", "hi_r2_r2_r7"},
    {{gt, r9, r9, r1}, "It gt;", "gt r9 r9 r1", "gt_r9_r9_r1"},
    {{vs, r6, r6, r5}, "It vs;", "vs r6 r6 r5", "vs_r6_r6_r5"},
    {{cc, r1, r1, r14}, "It cc;", "cc r1 r1 r14", "cc_r1_r1_r14"},
    {{eq, r2, r2, r0}, "It eq;", "eq r2 r2 r0", "eq_r2_r2_r0"},
    {{vc, r8, r8, r14}, "It vc;", "vc r8 r8 r14", "vc_r8_r8_r14"},
    {{pl, r6, r6, r9}, "It pl;", "pl r6 r6 r9", "pl_r6_r6_r9"},
    {{hi, r11, r11, r11}, "It hi;", "hi r11 r11 r11", "hi_r11_r11_r11"},
    {{ls, r0, r0, r11}, "It ls;", "ls r0 r0 r11", "ls_r0_r0_r11"},
    {{ne, r9, r9, r1}, "It ne;", "ne r9 r9 r1", "ne_r9_r9_r1"},
    {{lt, r13, r13, r1}, "It lt;", "lt r13 r13 r1", "lt_r13_r13_r1"},
    {{eq, r3, r3, r14}, "It eq;", "eq r3 r3 r14", "eq_r3_r3_r14"},
    {{vs, r14, r14, r5}, "It vs;", "vs r14 r14 r5", "vs_r14_r14_r5"},
    {{ne, r11, r11, r4}, "It ne;", "ne r11 r11 r4", "ne_r11_r11_r4"},
    {{gt, r13, r13, r0}, "It gt;", "gt r13 r13 r0", "gt_r13_r13_r0"},
    {{mi, r11, r11, r4}, "It mi;", "mi r11 r11 r4", "mi_r11_r11_r4"},
    {{cs, r10, r10, r4}, "It cs;", "cs r10 r10 r4", "cs_r10_r10_r4"},
    {{hi, r14, r14, r11}, "It hi;", "hi r14 r14 r11", "hi_r14_r14_r11"},
    {{cs, r11, r11, r4}, "It cs;", "cs r11 r11 r4", "cs_r11_r11_r4"},
    {{eq, r0, r0, r10}, "It eq;", "eq r0 r0 r10", "eq_r0_r0_r10"},
    {{cs, r7, r7, r10}, "It cs;", "cs r7 r7 r10", "cs_r7_r7_r10"},
    {{ne, r5, r5, r9}, "It ne;", "ne r5 r5 r9", "ne_r5_r5_r9"},
    {{pl, r3, r3, r7}, "It pl;", "pl r3 r3 r7", "pl_r3_r3_r7"},
    {{lt, r1, r1, r0}, "It lt;", "lt r1 r1 r0", "lt_r1_r1_r0"},
    {{vs, r12, r12, r9}, "It vs;", "vs r12 r12 r9", "vs_r12_r12_r9"},
    {{mi, r8, r8, r4}, "It mi;", "mi r8 r8 r4", "mi_r8_r8_r4"},
    {{lt, r0, r0, r7}, "It lt;", "lt r0 r0 r7", "lt_r0_r0_r7"},
    {{vc, r2, r2, r2}, "It vc;", "vc r2 r2 r2", "vc_r2_r2_r2"},
    {{le, r1, r1, r4}, "It le;", "le r1 r1 r4", "le_r1_r1_r4"},
    {{vs, r3, r3, r3}, "It vs;", "vs r3 r3 r3", "vs_r3_r3_r3"},
    {{gt, r2, r2, r7}, "It gt;", "gt r2 r2 r7", "gt_r2_r2_r7"},
    {{vc, r7, r7, r6}, "It vc;", "vc r7 r7 r6", "vc_r7_r7_r6"},
    {{vs, r2, r2, r5}, "It vs;", "vs r2 r2 r5", "vs_r2_r2_r5"},
    {{ls, r14, r14, r5}, "It ls;", "ls r14 r14 r5", "ls_r14_r14_r5"},
    {{eq, r5, r5, r8}, "It eq;", "eq r5 r5 r8", "eq_r5_r5_r8"},
    {{cc, r3, r3, r11}, "It cc;", "cc r3 r3 r11", "cc_r3_r3_r11"},
    {{mi, r11, r11, r9}, "It mi;", "mi r11 r11 r9", "mi_r11_r11_r9"},
    {{eq, r7, r7, r0}, "It eq;", "eq r7 r7 r0", "eq_r7_r7_r0"},
    {{eq, r5, r5, r0}, "It eq;", "eq r5 r5 r0", "eq_r5_r5_r0"},
    {{ne, r7, r7, r8}, "It ne;", "ne r7 r7 r8", "ne_r7_r7_r8"},
    {{lt, r3, r3, r6}, "It lt;", "lt r3 r3 r6", "lt_r3_r3_r6"},
    {{hi, r2, r2, r4}, "It hi;", "hi r2 r2 r4", "hi_r2_r2_r4"},
    {{vs, r1, r1, r11}, "It vs;", "vs r1 r1 r11", "vs_r1_r1_r11"},
    {{cs, r3, r3, r9}, "It cs;", "cs r3 r3 r9", "cs_r3_r3_r9"},
    {{gt, r4, r4, r7}, "It gt;", "gt r4 r4 r7", "gt_r4_r4_r7"},
    {{ge, r1, r1, r9}, "It ge;", "ge r1 r1 r9", "ge_r1_r1_r9"},
    {{cc, r9, r9, r5}, "It cc;", "cc r9 r9 r5", "cc_r9_r9_r5"},
    {{mi, r6, r6, r0}, "It mi;", "mi r6 r6 r0", "mi_r6_r6_r0"},
    {{hi, r2, r2, r0}, "It hi;", "hi r2 r2 r0", "hi_r2_r2_r0"},
    {{hi, r8, r8, r4}, "It hi;", "hi r8 r8 r4", "hi_r8_r8_r4"},
    {{hi, r9, r9, r1}, "It hi;", "hi r9 r9 r1", "hi_r9_r9_r1"},
    {{ls, r7, r7, r11}, "It ls;", "ls r7 r7 r11", "ls_r7_r7_r11"},
    {{eq, r1, r1, r2}, "It eq;", "eq r1 r1 r2", "eq_r1_r1_r2"},
    {{pl, r10, r10, r8}, "It pl;", "pl r10 r10 r8", "pl_r10_r10_r8"},
    {{hi, r13, r13, r14}, "It hi;", "hi r13 r13 r14", "hi_r13_r13_r14"},
    {{pl, r5, r5, r2}, "It pl;", "pl r5 r5 r2", "pl_r5_r5_r2"},
    {{hi, r8, r8, r12}, "It hi;", "hi r8 r8 r12", "hi_r8_r8_r12"},
    {{mi, r0, r0, r10}, "It mi;", "mi r0 r0 r10", "mi_r0_r0_r10"},
    {{mi, r4, r4, r0}, "It mi;", "mi r4 r4 r0", "mi_r4_r4_r0"},
    {{pl, r13, r13, r3}, "It pl;", "pl r13 r13 r3", "pl_r13_r13_r3"},
    {{cs, r9, r9, r9}, "It cs;", "cs r9 r9 r9", "cs_r9_r9_r9"},
    {{gt, r11, r11, r3}, "It gt;", "gt r11 r11 r3", "gt_r11_r11_r3"},
    {{mi, r12, r12, r3}, "It mi;", "mi r12 r12 r3", "mi_r12_r12_r3"},
    {{vc, r0, r0, r11}, "It vc;", "vc r0 r0 r11", "vc_r0_r0_r11"},
    {{vc, r4, r4, r5}, "It vc;", "vc r4 r4 r5", "vc_r4_r4_r5"},
    {{hi, r10, r10, r3}, "It hi;", "hi r10 r10 r3", "hi_r10_r10_r3"},
    {{hi, r11, r11, r8}, "It hi;", "hi r11 r11 r8", "hi_r11_r11_r8"},
    {{le, r5, r5, r7}, "It le;", "le r5 r5 r7", "le_r5_r5_r7"},
    {{pl, r3, r3, r4}, "It pl;", "pl r3 r3 r4", "pl_r3_r3_r4"},
    {{vs, r12, r12, r8}, "It vs;", "vs r12 r12 r8", "vs_r12_r12_r8"},
    {{ne, r5, r5, r12}, "It ne;", "ne r5 r5 r12", "ne_r5_r5_r12"},
    {{ls, r5, r5, r3}, "It ls;", "ls r5 r5 r3", "ls_r5_r5_r3"},
    {{le, r11, r11, r7}, "It le;", "le r11 r11 r7", "le_r11_r11_r7"},
    {{vc, r4, r4, r11}, "It vc;", "vc r4 r4 r11", "vc_r4_r4_r11"},
    {{mi, r11, r11, r2}, "It mi;", "mi r11 r11 r2", "mi_r11_r11_r2"},
    {{gt, r0, r0, r9}, "It gt;", "gt r0 r0 r9", "gt_r0_r0_r9"},
    {{cs, r9, r9, r11}, "It cs;", "cs r9 r9 r11", "cs_r9_r9_r11"},
    {{ge, r7, r7, r1}, "It ge;", "ge r7 r7 r1", "ge_r7_r7_r1"},
    {{ne, r13, r13, r3}, "It ne;", "ne r13 r13 r3", "ne_r13_r13_r3"},
    {{gt, r0, r0, r3}, "It gt;", "gt r0 r0 r3", "gt_r0_r0_r3"},
    {{ne, r10, r10, r7}, "It ne;", "ne r10 r10 r7", "ne_r10_r10_r7"},
    {{vs, r5, r5, r14}, "It vs;", "vs r5 r5 r14", "vs_r5_r5_r14"},
    {{ge, r4, r4, r14}, "It ge;", "ge r4 r4 r14", "ge_r4_r4_r14"},
    {{ge, r9, r9, r12}, "It ge;", "ge r9 r9 r12", "ge_r9_r9_r12"},
    {{pl, r4, r4, r8}, "It pl;", "pl r4 r4 r8", "pl_r4_r4_r8"},
    {{ls, r10, r10, r3}, "It ls;", "ls r10 r10 r3", "ls_r10_r10_r3"},
    {{vc, r8, r8, r12}, "It vc;", "vc r8 r8 r12", "vc_r8_r8_r12"},
    {{hi, r2, r2, r8}, "It hi;", "hi r2 r2 r8", "hi_r2_r2_r8"},
    {{ne, r1, r1, r2}, "It ne;", "ne r1 r1 r2", "ne_r1_r1_r2"},
    {{pl, r13, r13, r2}, "It pl;", "pl r13 r13 r2", "pl_r13_r13_r2"},
    {{vc, r5, r5, r2}, "It vc;", "vc r5 r5 r2", "vc_r5_r5_r2"},
    {{eq, r4, r4, r7}, "It eq;", "eq r4 r4 r7", "eq_r4_r4_r7"},
    {{vc, r12, r12, r7}, "It vc;", "vc r12 r12 r7", "vc_r12_r12_r7"},
    {{vs, r6, r6, r9}, "It vs;", "vs r6 r6 r9", "vs_r6_r6_r9"},
    {{ne, r6, r6, r10}, "It ne;", "ne r6 r6 r10", "ne_r6_r6_r10"},
    {{vc, r10, r10, r14}, "It vc;", "vc r10 r10 r14", "vc_r10_r10_r14"},
    {{ne, r13, r13, r2}, "It ne;", "ne r13 r13 r2", "ne_r13_r13_r2"},
    {{vc, r1, r1, r10}, "It vc;", "vc r1 r1 r10", "vc_r1_r1_r10"},
    {{eq, r11, r11, r9}, "It eq;", "eq r11 r11 r9", "eq_r11_r11_r9"},
    {{cs, r5, r5, r12}, "It cs;", "cs r5 r5 r12", "cs_r5_r5_r12"},
    {{gt, r8, r8, r2}, "It gt;", "gt r8 r8 r2", "gt_r8_r8_r2"},
    {{cs, r0, r0, r11}, "It cs;", "cs r0 r0 r11", "cs_r0_r0_r11"},
    {{cs, r0, r0, r4}, "It cs;", "cs r0 r0 r4", "cs_r0_r0_r4"},
    {{mi, r1, r1, r9}, "It mi;", "mi r1 r1 r9", "mi_r1_r1_r9"},
    {{vc, r14, r14, r9}, "It vc;", "vc r14 r14 r9", "vc_r14_r14_r9"},
    {{ne, r4, r4, r1}, "It ne;", "ne r4 r4 r1", "ne_r4_r4_r1"},
    {{gt, r11, r11, r0}, "It gt;", "gt r11 r11 r0", "gt_r11_r11_r0"},
    {{ls, r9, r9, r5}, "It ls;", "ls r9 r9 r5", "ls_r9_r9_r5"},
    {{mi, r14, r14, r1}, "It mi;", "mi r14 r14 r1", "mi_r14_r14_r1"},
    {{cc, r14, r14, r3}, "It cc;", "cc r14 r14 r3", "cc_r14_r14_r3"},
    {{gt, r1, r1, r0}, "It gt;", "gt r1 r1 r0", "gt_r1_r1_r0"},
    {{ge, r14, r14, r8}, "It ge;", "ge r14 r14 r8", "ge_r14_r14_r8"},
    {{lt, r14, r14, r8}, "It lt;", "lt r14 r14 r8", "lt_r14_r14_r8"},
    {{ne, r10, r10, r2}, "It ne;", "ne r10 r10 r2", "ne_r10_r10_r2"},
    {{gt, r9, r9, r3}, "It gt;", "gt r9 r9 r3", "gt_r9_r9_r3"},
    {{pl, r13, r13, r5}, "It pl;", "pl r13 r13 r5", "pl_r13_r13_r5"},
    {{pl, r0, r0, r5}, "It pl;", "pl r0 r0 r5", "pl_r0_r0_r5"},
    {{hi, r6, r6, r4}, "It hi;", "hi r6 r6 r4", "hi_r6_r6_r4"},
    {{gt, r1, r1, r10}, "It gt;", "gt r1 r1 r10", "gt_r1_r1_r10"},
    {{vs, r12, r12, r7}, "It vs;", "vs r12 r12 r7", "vs_r12_r12_r7"},
    {{cc, r3, r3, r3}, "It cc;", "cc r3 r3 r3", "cc_r3_r3_r3"},
    {{vs, r2, r2, r11}, "It vs;", "vs r2 r2 r11", "vs_r2_r2_r11"},
    {{mi, r2, r2, r4}, "It mi;", "mi r2 r2 r4", "mi_r2_r2_r4"},
    {{cc, r4, r4, r11}, "It cc;", "cc r4 r4 r11", "cc_r4_r4_r11"},
    {{mi, r7, r7, r11}, "It mi;", "mi r7 r7 r11", "mi_r7_r7_r11"},
    {{cc, r2, r2, r7}, "It cc;", "cc r2 r2 r7", "cc_r2_r2_r7"},
    {{ge, r4, r4, r9}, "It ge;", "ge r4 r4 r9", "ge_r4_r4_r9"},
    {{cc, r6, r6, r4}, "It cc;", "cc r6 r6 r4", "cc_r6_r6_r4"},
    {{cc, r14, r14, r7}, "It cc;", "cc r14 r14 r7", "cc_r14_r14_r7"},
    {{gt, r12, r12, r6}, "It gt;", "gt r12 r12 r6", "gt_r12_r12_r6"},
    {{eq, r11, r11, r2}, "It eq;", "eq r11 r11 r2", "eq_r11_r11_r2"},
    {{vs, r4, r4, r3}, "It vs;", "vs r4 r4 r3", "vs_r4_r4_r3"},
    {{vs, r10, r10, r6}, "It vs;", "vs r10 r10 r6", "vs_r10_r10_r6"},
    {{ls, r7, r7, r5}, "It ls;", "ls r7 r7 r5", "ls_r7_r7_r5"},
    {{lt, r1, r1, r14}, "It lt;", "lt r1 r1 r14", "lt_r1_r1_r14"},
    {{lt, r11, r11, r5}, "It lt;", "lt r11 r11 r5", "lt_r11_r11_r5"},
    {{gt, r0, r0, r5}, "It gt;", "gt r0 r0 r5", "gt_r0_r0_r5"},
    {{pl, r11, r11, r9}, "It pl;", "pl r11 r11 r9", "pl_r11_r11_r9"},
    {{cs, r1, r1, r14}, "It cs;", "cs r1 r1 r14", "cs_r1_r1_r14"},
    {{le, r9, r9, r6}, "It le;", "le r9 r9 r6", "le_r9_r9_r6"},
    {{gt, r2, r2, r9}, "It gt;", "gt r2 r2 r9", "gt_r2_r2_r9"},
    {{vs, r12, r12, r12}, "It vs;", "vs r12 r12 r12", "vs_r12_r12_r12"},
    {{eq, r12, r12, r11}, "It eq;", "eq r12 r12 r11", "eq_r12_r12_r11"},
    {{ne, r6, r6, r12}, "It ne;", "ne r6 r6 r12", "ne_r6_r6_r12"},
    {{ne, r4, r4, r6}, "It ne;", "ne r4 r4 r6", "ne_r4_r4_r6"},
    {{vc, r0, r0, r8}, "It vc;", "vc r0 r0 r8", "vc_r0_r0_r8"},
    {{ls, r0, r0, r1}, "It ls;", "ls r0 r0 r1", "ls_r0_r0_r1"},
    {{vc, r14, r14, r0}, "It vc;", "vc r14 r14 r0", "vc_r14_r14_r0"},
    {{lt, r6, r6, r14}, "It lt;", "lt r6 r6 r14", "lt_r6_r6_r14"},
    {{hi, r10, r10, r12}, "It hi;", "hi r10 r10 r12", "hi_r10_r10_r12"},
    {{ne, r9, r9, r2}, "It ne;", "ne r9 r9 r2", "ne_r9_r9_r2"},
    {{cs, r5, r5, r14}, "It cs;", "cs r5 r5 r14", "cs_r5_r5_r14"},
    {{ls, r0, r0, r8}, "It ls;", "ls r0 r0 r8", "ls_r0_r0_r8"},
    {{le, r5, r5, r10}, "It le;", "le r5 r5 r10", "le_r5_r5_r10"},
    {{gt, r12, r12, r10}, "It gt;", "gt r12 r12 r10", "gt_r12_r12_r10"},
    {{ls, r1, r1, r9}, "It ls;", "ls r1 r1 r9", "ls_r1_r1_r9"},
    {{ge, r0, r0, r5}, "It ge;", "ge r0 r0 r5", "ge_r0_r0_r5"},
    {{mi, r6, r6, r12}, "It mi;", "mi r6 r6 r12", "mi_r6_r6_r12"},
    {{cs, r2, r2, r7}, "It cs;", "cs r2 r2 r7", "cs_r2_r2_r7"},
    {{ls, r7, r7, r2}, "It ls;", "ls r7 r7 r2", "ls_r7_r7_r2"},
    {{eq, r11, r11, r6}, "It eq;", "eq r11 r11 r6", "eq_r11_r11_r6"},
    {{le, r9, r9, r3}, "It le;", "le r9 r9 r3", "le_r9_r9_r3"},
    {{mi, r8, r8, r11}, "It mi;", "mi r8 r8 r11", "mi_r8_r8_r11"},
    {{cs, r4, r4, r5}, "It cs;", "cs r4 r4 r5", "cs_r4_r4_r5"},
    {{mi, r11, r11, r8}, "It mi;", "mi r11 r11 r8", "mi_r11_r11_r8"},
    {{ge, r11, r11, r8}, "It ge;", "ge r11 r11 r8", "ge_r11_r11_r8"},
    {{mi, r13, r13, r6}, "It mi;", "mi r13 r13 r6", "mi_r13_r13_r6"},
    {{cc, r8, r8, r8}, "It cc;", "cc r8 r8 r8", "cc_r8_r8_r8"},
    {{eq, r11, r11, r14}, "It eq;", "eq r11 r11 r14", "eq_r11_r11_r14"},
    {{lt, r6, r6, r1}, "It lt;", "lt r6 r6 r1", "lt_r6_r6_r1"},
    {{mi, r13, r13, r9}, "It mi;", "mi r13 r13 r9", "mi_r13_r13_r9"},
    {{cs, r9, r9, r0}, "It cs;", "cs r9 r9 r0", "cs_r9_r9_r0"},
    {{gt, r5, r5, r2}, "It gt;", "gt r5 r5 r2", "gt_r5_r5_r2"},
    {{lt, r8, r8, r2}, "It lt;", "lt r8 r8 r2", "lt_r8_r8_r2"},
    {{pl, r5, r5, r0}, "It pl;", "pl r5 r5 r0", "pl_r5_r5_r0"},
    {{vs, r1, r1, r12}, "It vs;", "vs r1 r1 r12", "vs_r1_r1_r12"},
    {{vs, r9, r9, r4}, "It vs;", "vs r9 r9 r4", "vs_r9_r9_r4"},
    {{ls, r8, r8, r9}, "It ls;", "ls r8 r8 r9", "ls_r8_r8_r9"},
    {{lt, r9, r9, r4}, "It lt;", "lt r9 r9 r4", "lt_r9_r9_r4"},
    {{ne, r3, r3, r5}, "It ne;", "ne r3 r3 r5", "ne_r3_r3_r5"},
    {{hi, r4, r4, r10}, "It hi;", "hi r4 r4 r10", "hi_r4_r4_r10"},
    {{vc, r1, r1, r9}, "It vc;", "vc r1 r1 r9", "vc_r1_r1_r9"},
    {{ge, r9, r9, r5}, "It ge;", "ge r9 r9 r5", "ge_r9_r9_r5"},
    {{hi, r0, r0, r12}, "It hi;", "hi r0 r0 r12", "hi_r0_r0_r12"},
    {{mi, r13, r13, r7}, "It mi;", "mi r13 r13 r7", "mi_r13_r13_r7"},
    {{eq, r3, r3, r10}, "It eq;", "eq r3 r3 r10", "eq_r3_r3_r10"},
    {{vc, r9, r9, r0}, "It vc;", "vc r9 r9 r0", "vc_r9_r9_r0"},
    {{mi, r2, r2, r1}, "It mi;", "mi r2 r2 r1", "mi_r2_r2_r1"},
    {{eq, r13, r13, r8}, "It eq;", "eq r13 r13 r8", "eq_r13_r13_r8"},
    {{ge, r12, r12, r1}, "It ge;", "ge r12 r12 r1", "ge_r12_r12_r1"},
    {{eq, r14, r14, r5}, "It eq;", "eq r14 r14 r5", "eq_r14_r14_r5"},
    {{pl, r9, r9, r6}, "It pl;", "pl r9 r9 r6", "pl_r9_r9_r6"},
    {{ge, r1, r1, r5}, "It ge;", "ge r1 r1 r5", "ge_r1_r1_r5"},
    {{vc, r2, r2, r7}, "It vc;", "vc r2 r2 r7", "vc_r2_r2_r7"},
    {{pl, r6, r6, r6}, "It pl;", "pl r6 r6 r6", "pl_r6_r6_r6"},
    {{le, r13, r13, r5}, "It le;", "le r13 r13 r5", "le_r13_r13_r5"},
    {{ls, r14, r14, r11}, "It ls;", "ls r14 r14 r11", "ls_r14_r14_r11"},
    {{mi, r13, r13, r0}, "It mi;", "mi r13 r13 r0", "mi_r13_r13_r0"},
    {{le, r4, r4, r11}, "It le;", "le r4 r4 r11", "le_r4_r4_r11"},
    {{pl, r5, r5, r8}, "It pl;", "pl r5 r5 r8", "pl_r5_r5_r8"},
    {{vs, r13, r13, r7}, "It vs;", "vs r13 r13 r7", "vs_r13_r13_r7"},
    {{lt, r5, r5, r8}, "It lt;", "lt r5 r5 r8", "lt_r5_r5_r8"},
    {{cs, r3, r3, r0}, "It cs;", "cs r3 r3 r0", "cs_r3_r3_r0"},
    {{vs, r2, r2, r10}, "It vs;", "vs r2 r2 r10", "vs_r2_r2_r10"},
    {{lt, r10, r10, r12}, "It lt;", "lt r10 r10 r12", "lt_r10_r10_r12"},
    {{ge, r10, r10, r6}, "It ge;", "ge r10 r10 r6", "ge_r10_r10_r6"},
    {{hi, r3, r3, r6}, "It hi;", "hi r3 r3 r6", "hi_r3_r3_r6"},
    {{vs, r11, r11, r9}, "It vs;", "vs r11 r11 r9", "vs_r11_r11_r9"},
    {{gt, r10, r10, r1}, "It gt;", "gt r10 r10 r1", "gt_r10_r10_r1"},
    {{mi, r12, r12, r12}, "It mi;", "mi r12 r12 r12", "mi_r12_r12_r12"},
    {{pl, r9, r9, r8}, "It pl;", "pl r9 r9 r8", "pl_r9_r9_r8"},
    {{ls, r1, r1, r5}, "It ls;", "ls r1 r1 r5", "ls_r1_r1_r5"},
    {{le, r8, r8, r14}, "It le;", "le r8 r8 r14", "le_r8_r8_r14"},
    {{ne, r0, r0, r5}, "It ne;", "ne r0 r0 r5", "ne_r0_r0_r5"},
    {{mi, r12, r12, r0}, "It mi;", "mi r12 r12 r0", "mi_r12_r12_r0"},
    {{mi, r14, r14, r6}, "It mi;", "mi r14 r14 r6", "mi_r14_r14_r6"},
    {{lt, r10, r10, r11}, "It lt;", "lt r10 r10 r11", "lt_r10_r10_r11"},
    {{vc, r4, r4, r0}, "It vc;", "vc r4 r4 r0", "vc_r4_r4_r0"},
    {{ge, r13, r13, r8}, "It ge;", "ge r13 r13 r8", "ge_r13_r13_r8"},
    {{ne, r2, r2, r7}, "It ne;", "ne r2 r2 r7", "ne_r2_r2_r7"},
    {{lt, r11, r11, r3}, "It lt;", "lt r11 r11 r3", "lt_r11_r11_r3"},
    {{vs, r14, r14, r2}, "It vs;", "vs r14 r14 r2", "vs_r14_r14_r2"},
    {{ge, r8, r8, r10}, "It ge;", "ge r8 r8 r10", "ge_r8_r8_r10"},
    {{le, r2, r2, r10}, "It le;", "le r2 r2 r10", "le_r2_r2_r10"},
    {{ne, r11, r11, r14}, "It ne;", "ne r11 r11 r14", "ne_r11_r11_r14"},
    {{vc, r14, r14, r4}, "It vc;", "vc r14 r14 r4", "vc_r14_r14_r4"},
    {{vc, r0, r0, r9}, "It vc;", "vc r0 r0 r9", "vc_r0_r0_r9"},
    {{ls, r14, r14, r2}, "It ls;", "ls r14 r14 r2", "ls_r14_r14_r2"},
    {{ge, r9, r9, r3}, "It ge;", "ge r9 r9 r3", "ge_r9_r9_r3"},
    {{gt, r8, r8, r10}, "It gt;", "gt r8 r8 r10", "gt_r8_r8_r10"},
    {{mi, r0, r0, r3}, "It mi;", "mi r0 r0 r3", "mi_r0_r0_r3"},
    {{mi, r10, r10, r0}, "It mi;", "mi r10 r10 r0", "mi_r10_r10_r0"},
    {{gt, r6, r6, r10}, "It gt;", "gt r6 r6 r10", "gt_r6_r6_r10"},
    {{pl, r6, r6, r8}, "It pl;", "pl r6 r6 r8", "pl_r6_r6_r8"},
    {{cc, r11, r11, r14}, "It cc;", "cc r11 r11 r14", "cc_r11_r11_r14"},
    {{le, r6, r6, r9}, "It le;", "le r6 r6 r9", "le_r6_r6_r9"},
    {{mi, r8, r8, r6}, "It mi;", "mi r8 r8 r6", "mi_r8_r8_r6"},
    {{mi, r10, r10, r9}, "It mi;", "mi r10 r10 r9", "mi_r10_r10_r9"},
    {{cc, r5, r5, r0}, "It cc;", "cc r5 r5 r0", "cc_r5_r5_r0"},
    {{hi, r0, r0, r4}, "It hi;", "hi r0 r0 r4", "hi_r0_r0_r4"},
    {{ls, r8, r8, r5}, "It ls;", "ls r8 r8 r5", "ls_r8_r8_r5"},
    {{hi, r9, r9, r10}, "It hi;", "hi r9 r9 r10", "hi_r9_r9_r10"},
    {{le, r11, r11, r12}, "It le;", "le r11 r11 r12", "le_r11_r11_r12"},
    {{le, r2, r2, r7}, "It le;", "le r2 r2 r7", "le_r2_r2_r7"},
    {{ls, r10, r10, r14}, "It ls;", "ls r10 r10 r14", "ls_r10_r10_r14"},
    {{ne, r4, r4, r11}, "It ne;", "ne r4 r4 r11", "ne_r4_r4_r11"},
    {{gt, r8, r8, r8}, "It gt;", "gt r8 r8 r8", "gt_r8_r8_r8"},
    {{eq, r4, r4, r1}, "It eq;", "eq r4 r4 r1", "eq_r4_r4_r1"},
    {{lt, r6, r6, r12}, "It lt;", "lt r6 r6 r12", "lt_r6_r6_r12"},
    {{hi, r11, r11, r0}, "It hi;", "hi r11 r11 r0", "hi_r11_r11_r0"},
    {{hi, r2, r2, r2}, "It hi;", "hi r2 r2 r2", "hi_r2_r2_r2"},
    {{pl, r5, r5, r12}, "It pl;", "pl r5 r5 r12", "pl_r5_r5_r12"},
    {{cc, r7, r7, r6}, "It cc;", "cc r7 r7 r6", "cc_r7_r7_r6"},
    {{gt, r4, r4, r5}, "It gt;", "gt r4 r4 r5", "gt_r4_r4_r5"},
    {{vc, r10, r10, r12}, "It vc;", "vc r10 r10 r12", "vc_r10_r10_r12"},
    {{eq, r13, r13, r4}, "It eq;", "eq r13 r13 r4", "eq_r13_r13_r4"},
    {{pl, r2, r2, r1}, "It pl;", "pl r2 r2 r1", "pl_r2_r2_r1"},
    {{vs, r12, r12, r14}, "It vs;", "vs r12 r12 r14", "vs_r12_r12_r14"},
    {{ge, r2, r2, r0}, "It ge;", "ge r2 r2 r0", "ge_r2_r2_r0"},
    {{lt, r3, r3, r9}, "It lt;", "lt r3 r3 r9", "lt_r3_r3_r9"},
    {{hi, r7, r7, r12}, "It hi;", "hi r7 r7 r12", "hi_r7_r7_r12"},
    {{cc, r0, r0, r5}, "It cc;", "cc r0 r0 r5", "cc_r0_r0_r5"},
    {{mi, r4, r4, r5}, "It mi;", "mi r4 r4 r5", "mi_r4_r4_r5"},
    {{lt, r13, r13, r11}, "It lt;", "lt r13 r13 r11", "lt_r13_r13_r11"},
    {{vs, r5, r5, r11}, "It vs;", "vs r5 r5 r11", "vs_r5_r5_r11"},
    {{vs, r10, r10, r11}, "It vs;", "vs r10 r10 r11", "vs_r10_r10_r11"},
    {{cs, r7, r7, r7}, "It cs;", "cs r7 r7 r7", "cs_r7_r7_r7"},
    {{le, r2, r2, r11}, "It le;", "le r2 r2 r11", "le_r2_r2_r11"},
    {{hi, r14, r14, r2}, "It hi;", "hi r14 r14 r2", "hi_r14_r14_r2"},
    {{cs, r12, r12, r8}, "It cs;", "cs r12 r12 r8", "cs_r12_r12_r8"},
    {{ne, r0, r0, r11}, "It ne;", "ne r0 r0 r11", "ne_r0_r0_r11"},
    {{vc, r14, r14, r10}, "It vc;", "vc r14 r14 r10", "vc_r14_r14_r10"},
    {{vc, r5, r5, r10}, "It vc;", "vc r5 r5 r10", "vc_r5_r5_r10"},
    {{cc, r10, r10, r3}, "It cc;", "cc r10 r10 r3", "cc_r10_r10_r3"},
    {{vs, r10, r10, r1}, "It vs;", "vs r10 r10 r1", "vs_r10_r10_r1"},
    {{vc, r1, r1, r12}, "It vc;", "vc r1 r1 r12", "vc_r1_r1_r12"},
    {{vc, r7, r7, r8}, "It vc;", "vc r7 r7 r8", "vc_r7_r7_r8"},
    {{lt, r13, r13, r12}, "It lt;", "lt r13 r13 r12", "lt_r13_r13_r12"},
    {{ls, r1, r1, r14}, "It ls;", "ls r1 r1 r14", "ls_r1_r1_r14"},
    {{gt, r9, r9, r0}, "It gt;", "gt r9 r9 r0", "gt_r9_r9_r0"},
    {{cc, r3, r3, r1}, "It cc;", "cc r3 r3 r1", "cc_r3_r3_r1"},
    {{eq, r8, r8, r8}, "It eq;", "eq r8 r8 r8", "eq_r8_r8_r8"},
    {{pl, r14, r14, r3}, "It pl;", "pl r14 r14 r3", "pl_r14_r14_r3"},
    {{eq, r12, r12, r9}, "It eq;", "eq r12 r12 r9", "eq_r12_r12_r9"},
    {{ne, r14, r14, r11}, "It ne;", "ne r14 r14 r11", "ne_r14_r14_r11"},
    {{mi, r1, r1, r1}, "It mi;", "mi r1 r1 r1", "mi_r1_r1_r1"},
    {{vc, r8, r8, r0}, "It vc;", "vc r8 r8 r0", "vc_r8_r8_r0"},
    {{eq, r0, r0, r11}, "It eq;", "eq r0 r0 r11", "eq_r0_r0_r11"},
    {{ge, r6, r6, r12}, "It ge;", "ge r6 r6 r12", "ge_r6_r6_r12"},
    {{pl, r12, r12, r14}, "It pl;", "pl r12 r12 r14", "pl_r12_r12_r14"},
    {{cc, r1, r1, r0}, "It cc;", "cc r1 r1 r0", "cc_r1_r1_r0"},
    {{gt, r11, r11, r1}, "It gt;", "gt r11 r11 r1", "gt_r11_r11_r1"},
    {{gt, r7, r7, r6}, "It gt;", "gt r7 r7 r6", "gt_r7_r7_r6"},
    {{le, r9, r9, r4}, "It le;", "le r9 r9 r4", "le_r9_r9_r4"},
    {{gt, r13, r13, r2}, "It gt;", "gt r13 r13 r2", "gt_r13_r13_r2"},
    {{mi, r6, r6, r4}, "It mi;", "mi r6 r6 r4", "mi_r6_r6_r4"},
    {{eq, r14, r14, r6}, "It eq;", "eq r14 r14 r6", "eq_r14_r14_r6"},
    {{ge, r1, r1, r12}, "It ge;", "ge r1 r1 r12", "ge_r1_r1_r12"},
    {{lt, r5, r5, r14}, "It lt;", "lt r5 r5 r14", "lt_r5_r5_r14"},
    {{cc, r11, r11, r0}, "It cc;", "cc r11 r11 r0", "cc_r11_r11_r0"},
    {{mi, r12, r12, r1}, "It mi;", "mi r12 r12 r1", "mi_r12_r12_r1"},
    {{ge, r11, r11, r2}, "It ge;", "ge r11 r11 r2", "ge_r11_r11_r2"},
    {{ge, r13, r13, r1}, "It ge;", "ge r13 r13 r1", "ge_r13_r13_r1"},
    {{le, r9, r9, r7}, "It le;", "le r9 r9 r7", "le_r9_r9_r7"},
    {{cs, r11, r11, r1}, "It cs;", "cs r11 r11 r1", "cs_r11_r11_r1"},
    {{pl, r3, r3, r5}, "It pl;", "pl r3 r3 r5", "pl_r3_r3_r5"},
    {{eq, r8, r8, r7}, "It eq;", "eq r8 r8 r7", "eq_r8_r8_r7"},
    {{vs, r11, r11, r6}, "It vs;", "vs r11 r11 r6", "vs_r11_r11_r6"},
    {{cc, r6, r6, r7}, "It cc;", "cc r6 r6 r7", "cc_r6_r6_r7"},
    {{mi, r6, r6, r7}, "It mi;", "mi r6 r6 r7", "mi_r6_r6_r7"},
    {{mi, r9, r9, r2}, "It mi;", "mi r9 r9 r2", "mi_r9_r9_r2"},
    {{vs, r14, r14, r6}, "It vs;", "vs r14 r14 r6", "vs_r14_r14_r6"},
    {{ge, r5, r5, r1}, "It ge;", "ge r5 r5 r1", "ge_r5_r5_r1"},
    {{hi, r1, r1, r11}, "It hi;", "hi r1 r1 r11", "hi_r1_r1_r11"},
    {{mi, r14, r14, r3}, "It mi;", "mi r14 r14 r3", "mi_r14_r14_r3"},
    {{mi, r3, r3, r5}, "It mi;", "mi r3 r3 r5", "mi_r3_r3_r5"},
    {{lt, r13, r13, r14}, "It lt;", "lt r13 r13 r14", "lt_r13_r13_r14"},
    {{hi, r9, r9, r14}, "It hi;", "hi r9 r9 r14", "hi_r9_r9_r14"},
    {{gt, r9, r9, r6}, "It gt;", "gt r9 r9 r6", "gt_r9_r9_r6"},
    {{vc, r1, r1, r14}, "It vc;", "vc r1 r1 r14", "vc_r1_r1_r14"},
    {{cc, r10, r10, r7}, "It cc;", "cc r10 r10 r7", "cc_r10_r10_r7"},
    {{pl, r14, r14, r9}, "It pl;", "pl r14 r14 r9", "pl_r14_r14_r9"},
    {{cc, r7, r7, r7}, "It cc;", "cc r7 r7 r7", "cc_r7_r7_r7"},
    {{ge, r2, r2, r3}, "It ge;", "ge r2 r2 r3", "ge_r2_r2_r3"},
    {{cs, r8, r8, r7}, "It cs;", "cs r8 r8 r7", "cs_r8_r8_r7"},
    {{eq, r1, r1, r9}, "It eq;", "eq r1 r1 r9", "eq_r1_r1_r9"},
    {{ls, r9, r9, r2}, "It ls;", "ls r9 r9 r2", "ls_r9_r9_r2"},
    {{ls, r10, r10, r5}, "It ls;", "ls r10 r10 r5", "ls_r10_r10_r5"},
    {{ge, r1, r1, r1}, "It ge;", "ge r1 r1 r1", "ge_r1_r1_r1"},
    {{pl, r12, r12, r6}, "It pl;", "pl r12 r12 r6", "pl_r12_r12_r6"},
    {{lt, r11, r11, r7}, "It lt;", "lt r11 r11 r7", "lt_r11_r11_r7"},
    {{ge, r5, r5, r6}, "It ge;", "ge r5 r5 r6", "ge_r5_r5_r6"},
    {{ls, r14, r14, r14}, "It ls;", "ls r14 r14 r14", "ls_r14_r14_r14"},
    {{cs, r13, r13, r5}, "It cs;", "cs r13 r13 r5", "cs_r13_r13_r5"},
    {{vs, r2, r2, r1}, "It vs;", "vs r2 r2 r1", "vs_r2_r2_r1"},
    {{pl, r10, r10, r3}, "It pl;", "pl r10 r10 r3", "pl_r10_r10_r3"},
    {{cs, r9, r9, r6}, "It cs;", "cs r9 r9 r6", "cs_r9_r9_r6"},
    {{ge, r13, r13, r14}, "It ge;", "ge r13 r13 r14", "ge_r13_r13_r14"},
    {{cc, r14, r14, r6}, "It cc;", "cc r14 r14 r6", "cc_r14_r14_r6"},
    {{ge, r9, r9, r10}, "It ge;", "ge r9 r9 r10", "ge_r9_r9_r10"},
    {{gt, r6, r6, r5}, "It gt;", "gt r6 r6 r5", "gt_r6_r6_r5"},
    {{le, r11, r11, r3}, "It le;", "le r11 r11 r3", "le_r11_r11_r3"},
    {{cc, r2, r2, r1}, "It cc;", "cc r2 r2 r1", "cc_r2_r2_r1"},
    {{ne, r13, r13, r7}, "It ne;", "ne r13 r13 r7", "ne_r13_r13_r7"},
    {{vs, r11, r11, r8}, "It vs;", "vs r11 r11 r8", "vs_r11_r11_r8"},
    {{hi, r3, r3, r11}, "It hi;", "hi r3 r3 r11", "hi_r3_r3_r11"},
    {{vc, r13, r13, r1}, "It vc;", "vc r13 r13 r1", "vc_r13_r13_r1"},
    {{vs, r4, r4, r6}, "It vs;", "vs r4 r4 r6", "vs_r4_r4_r6"},
    {{pl, r5, r5, r9}, "It pl;", "pl r5 r5 r9", "pl_r5_r5_r9"},
    {{cc, r7, r7, r10}, "It cc;", "cc r7 r7 r10", "cc_r7_r7_r10"}};

// These headers each contain an array of `TestResult` with the reference output
// values. The reference arrays are names `kReference{mnemonic}`.
#include "aarch32/traces/assembler-cond-rd-rn-operand-rm-t32-rd-is-rn-in-it-block-add.h"

// The maximum number of errors to report in detail for each test.
static const unsigned kErrorReportLimit = 8;

typedef void (MacroAssembler::*Fn)(Condition cond, Register rd, Register rn,
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
    Register rm = kTests[i].operands.rm;
    Operand op(rm);

    uint32_t start = masm.GetCursorOffset();
    (masm.*instruction)(cond, rd, rn, op);
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
      "AARCH32_ASSEMBLER_COND_RD_RN_OPERAND_RM_T32_RD_IS_RN_IN_IT_"         \
      "BLOCK_" #mnemonic,                                                   \
      &Test_##mnemonic);
FOREACH_INSTRUCTION(TEST)
#undef TEST

}  // aarch32
}  // vixl
