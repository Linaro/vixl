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
  M(cmn)                       \
  M(cmp)                       \
  M(mov)                       \
  M(movs)                      \
  M(mvn)                       \
  M(mvns)                      \
  M(teq)                       \
  M(tst)


// The following definitions are defined again in each generated test, therefore
// we need to place them in an anomymous namespace. It expresses that they are
// local to this file only, and the compiler is not allowed to share these types
// across test files during template instantiation. Specifically, `Operands` has
// various layouts across generated tests so it absolutely cannot be shared.

namespace {

// Values to be passed to the assembler to produce the instruction under test.
struct Operands {
  Condition cond;
  Register rd;
  uint32_t immediate;
};

// This structure contains all data needed to test one specific
// instruction.
struct TestData {
  // The `operands` field represents what to pass to the assembler to
  // produce the instruction.
  Operands operands;
  // True if we need to generate an IT instruction for this test to be valid.
  bool in_it_block;
  // The condition to give the IT instruction, this will be set to "al" by
  // default.
  Condition it_condition;
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
const TestData kTests[] = {
    {{vc, r12, 0x00000ff0}, false, al, "vc r12 0x00000ff0", "vc_r12_"
                                                            "0x00000ff0"},
    {{ge, r0, 0x000000ab}, false, al, "ge r0 0x000000ab", "ge_r0_0x000000ab"},
    {{mi, r2, 0x000002ac}, false, al, "mi r2 0x000002ac", "mi_r2_0x000002ac"},
    {{pl, r13, 0x03fc0000}, false, al, "pl r13 0x03fc0000", "pl_r13_"
                                                            "0x03fc0000"},
    {{hi, r6, 0x0000ff00}, false, al, "hi r6 0x0000ff00", "hi_r6_0x0000ff00"},
    {{ls, r4, 0x000ff000}, false, al, "ls r4 0x000ff000", "ls_r4_0x000ff000"},
    {{al, r12, 0x0002ac00}, false, al, "al r12 0x0002ac00", "al_r12_"
                                                            "0x0002ac00"},
    {{ge, r15, 0xac000002}, false, al, "ge r15 0xac000002", "ge_r15_"
                                                            "0xac000002"},
    {{cc, r11, 0x000003fc}, false, al, "cc r11 0x000003fc", "cc_r11_"
                                                            "0x000003fc"},
    {{cs, r13, 0x002ac000}, false, al, "cs r13 0x002ac000", "cs_r13_"
                                                            "0x002ac000"},
    {{cc, r8, 0x0000ab00}, false, al, "cc r8 0x0000ab00", "cc_r8_0x0000ab00"},
    {{lt, r12, 0x000ff000}, false, al, "lt r12 0x000ff000", "lt_r12_"
                                                            "0x000ff000"},
    {{hi, r6, 0x00ff0000}, false, al, "hi r6 0x00ff0000", "hi_r6_0x00ff0000"},
    {{hi, r12, 0x00002ac0}, false, al, "hi r12 0x00002ac0", "hi_r12_"
                                                            "0x00002ac0"},
    {{vc, r13, 0x000000ab}, false, al, "vc r13 0x000000ab", "vc_r13_"
                                                            "0x000000ab"},
    {{cc, r11, 0x0000ff00}, false, al, "cc r11 0x0000ff00", "cc_r11_"
                                                            "0x0000ff00"},
    {{al, r10, 0x0ab00000}, false, al, "al r10 0x0ab00000", "al_r10_"
                                                            "0x0ab00000"},
    {{cc, r1, 0x000003fc}, false, al, "cc r1 0x000003fc", "cc_r1_0x000003fc"},
    {{ge, r9, 0x02ac0000}, false, al, "ge r9 0x02ac0000", "ge_r9_0x02ac0000"},
    {{vs, r7, 0xf000000f}, false, al, "vs r7 0xf000000f", "vs_r7_0xf000000f"},
    {{ne, r7, 0xc000002a}, false, al, "ne r7 0xc000002a", "ne_r7_0xc000002a"},
    {{ge, r3, 0x000000ff}, false, al, "ge r3 0x000000ff", "ge_r3_0x000000ff"},
    {{lt, r13, 0xab000000}, false, al, "lt r13 0xab000000", "lt_r13_"
                                                            "0xab000000"},
    {{cs, r10, 0x0003fc00}, false, al, "cs r10 0x0003fc00", "cs_r10_"
                                                            "0x0003fc00"},
    {{hi, r9, 0x0000ff00}, false, al, "hi r9 0x0000ff00", "hi_r9_0x0000ff00"},
    {{gt, r11, 0x000003fc}, false, al, "gt r11 0x000003fc", "gt_r11_"
                                                            "0x000003fc"},
    {{cc, r0, 0x00003fc0}, false, al, "cc r0 0x00003fc0", "cc_r0_0x00003fc0"},
    {{mi, r11, 0x002ac000}, false, al, "mi r11 0x002ac000", "mi_r11_"
                                                            "0x002ac000"},
    {{eq, r6, 0x00000ff0}, false, al, "eq r6 0x00000ff0", "eq_r6_0x00000ff0"},
    {{vs, r3, 0x0000ab00}, false, al, "vs r3 0x0000ab00", "vs_r3_0x0000ab00"},
    {{gt, r1, 0x003fc000}, false, al, "gt r1 0x003fc000", "gt_r1_0x003fc000"},
    {{ne, r2, 0x0002ac00}, false, al, "ne r2 0x0002ac00", "ne_r2_0x0002ac00"},
    {{cc, r3, 0x0ff00000}, false, al, "cc r3 0x0ff00000", "cc_r3_0x0ff00000"},
    {{vc, r7, 0x2ac00000}, false, al, "vc r7 0x2ac00000", "vc_r7_0x2ac00000"},
    {{al, r15, 0x000003fc}, false, al, "al r15 0x000003fc", "al_r15_"
                                                            "0x000003fc"},
    {{al, r12, 0xac000002}, false, al, "al r12 0xac000002", "al_r12_"
                                                            "0xac000002"},
    {{ge, r6, 0x00000000}, false, al, "ge r6 0x00000000", "ge_r6_0x00000000"},
    {{cs, r10, 0x000002ac}, false, al, "cs r10 0x000002ac", "cs_r10_"
                                                            "0x000002ac"},
    {{ls, r14, 0x0ab00000}, false, al, "ls r14 0x0ab00000", "ls_r14_"
                                                            "0x0ab00000"},
    {{le, r0, 0xac000002}, false, al, "le r0 0xac000002", "le_r0_0xac000002"},
    {{lt, r3, 0xc000003f}, false, al, "lt r3 0xc000003f", "lt_r3_0xc000003f"},
    {{cs, r4, 0x000000ab}, false, al, "cs r4 0x000000ab", "cs_r4_0x000000ab"},
    {{pl, r0, 0x00ab0000}, false, al, "pl r0 0x00ab0000", "pl_r0_0x00ab0000"},
    {{hi, r6, 0xc000002a}, false, al, "hi r6 0xc000002a", "hi_r6_0xc000002a"},
    {{vc, r4, 0x00000ab0}, false, al, "vc r4 0x00000ab0", "vc_r4_0x00000ab0"},
    {{cc, r10, 0xab000000}, false, al, "cc r10 0xab000000", "cc_r10_"
                                                            "0xab000000"},
    {{vs, r9, 0x03fc0000}, false, al, "vs r9 0x03fc0000", "vs_r9_0x03fc0000"},
    {{vc, r11, 0x00000ab0}, false, al, "vc r11 0x00000ab0", "vc_r11_"
                                                            "0x00000ab0"},
    {{le, r11, 0x00000ab0}, false, al, "le r11 0x00000ab0", "le_r11_"
                                                            "0x00000ab0"},
    {{ne, r10, 0x0003fc00}, false, al, "ne r10 0x0003fc00", "ne_r10_"
                                                            "0x0003fc00"},
    {{vc, r10, 0x03fc0000}, false, al, "vc r10 0x03fc0000", "vc_r10_"
                                                            "0x03fc0000"},
    {{vc, r4, 0xc000002a}, false, al, "vc r4 0xc000002a", "vc_r4_0xc000002a"},
    {{mi, r0, 0x000000ab}, false, al, "mi r0 0x000000ab", "mi_r0_0x000000ab"},
    {{al, r13, 0x00000ff0}, false, al, "al r13 0x00000ff0", "al_r13_"
                                                            "0x00000ff0"},
    {{hi, r5, 0x00002ac0}, false, al, "hi r5 0x00002ac0", "hi_r5_0x00002ac0"},
    {{cc, r5, 0xfc000003}, false, al, "cc r5 0xfc000003", "cc_r5_0xfc000003"},
    {{ls, r9, 0x00000ab0}, false, al, "ls r9 0x00000ab0", "ls_r9_0x00000ab0"},
    {{eq, r11, 0x00000ab0}, false, al, "eq r11 0x00000ab0", "eq_r11_"
                                                            "0x00000ab0"},
    {{vc, r6, 0x003fc000}, false, al, "vc r6 0x003fc000", "vc_r6_0x003fc000"},
    {{le, r13, 0x00ff0000}, false, al, "le r13 0x00ff0000", "le_r13_"
                                                            "0x00ff0000"},
    {{pl, r6, 0xc000003f}, false, al, "pl r6 0xc000003f", "pl_r6_0xc000003f"},
    {{hi, r6, 0x00000ab0}, false, al, "hi r6 0x00000ab0", "hi_r6_0x00000ab0"},
    {{gt, r3, 0x0000ab00}, false, al, "gt r3 0x0000ab00", "gt_r3_0x0000ab00"},
    {{ge, r1, 0x00000ab0}, false, al, "ge r1 0x00000ab0", "ge_r1_0x00000ab0"},
    {{ge, r3, 0x00000ff0}, false, al, "ge r3 0x00000ff0", "ge_r3_0x00000ff0"},
    {{cs, r15, 0xf000000f}, false, al, "cs r15 0xf000000f", "cs_r15_"
                                                            "0xf000000f"},
    {{al, r12, 0x000000ab}, false, al, "al r12 0x000000ab", "al_r12_"
                                                            "0x000000ab"},
    {{ls, r0, 0xac000002}, false, al, "ls r0 0xac000002", "ls_r0_0xac000002"},
    {{vc, r9, 0x0ab00000}, false, al, "vc r9 0x0ab00000", "vc_r9_0x0ab00000"},
    {{gt, r4, 0x0ab00000}, false, al, "gt r4 0x0ab00000", "gt_r4_0x0ab00000"},
    {{al, r4, 0x0002ac00}, false, al, "al r4 0x0002ac00", "al_r4_0x0002ac00"},
    {{mi, r1, 0x00000000}, false, al, "mi r1 0x00000000", "mi_r1_0x00000000"},
    {{ge, r14, 0x000000ab}, false, al, "ge r14 0x000000ab", "ge_r14_"
                                                            "0x000000ab"},
    {{ge, r14, 0x00000000}, false, al, "ge r14 0x00000000", "ge_r14_"
                                                            "0x00000000"},
    {{le, r6, 0x2ac00000}, false, al, "le r6 0x2ac00000", "le_r6_0x2ac00000"},
    {{pl, r7, 0xf000000f}, false, al, "pl r7 0xf000000f", "pl_r7_0xf000000f"},
    {{gt, r14, 0x00002ac0}, false, al, "gt r14 0x00002ac0", "gt_r14_"
                                                            "0x00002ac0"},
    {{eq, r5, 0x000000ff}, false, al, "eq r5 0x000000ff", "eq_r5_0x000000ff"},
    {{gt, r2, 0xab000000}, false, al, "gt r2 0xab000000", "gt_r2_0xab000000"},
    {{ne, r1, 0x0002ac00}, false, al, "ne r1 0x0002ac00", "ne_r1_0x0002ac00"},
    {{ls, r1, 0x00000ff0}, false, al, "ls r1 0x00000ff0", "ls_r1_0x00000ff0"},
    {{cc, r10, 0xff000000}, false, al, "cc r10 0xff000000", "cc_r10_"
                                                            "0xff000000"},
    {{vs, r10, 0x02ac0000}, false, al, "vs r10 0x02ac0000", "vs_r10_"
                                                            "0x02ac0000"},
    {{ge, r15, 0x000ab000}, false, al, "ge r15 0x000ab000", "ge_r15_"
                                                            "0x000ab000"},
    {{le, r0, 0xab000000}, false, al, "le r0 0xab000000", "le_r0_0xab000000"},
    {{ls, r2, 0x00000ff0}, false, al, "ls r2 0x00000ff0", "ls_r2_0x00000ff0"},
    {{hi, r15, 0x00000000}, false, al, "hi r15 0x00000000", "hi_r15_"
                                                            "0x00000000"},
    {{ge, r9, 0xff000000}, false, al, "ge r9 0xff000000", "ge_r9_0xff000000"},
    {{al, r4, 0x03fc0000}, false, al, "al r4 0x03fc0000", "al_r4_0x03fc0000"},
    {{vs, r15, 0x00000ab0}, false, al, "vs r15 0x00000ab0", "vs_r15_"
                                                            "0x00000ab0"},
    {{cs, r12, 0x002ac000}, false, al, "cs r12 0x002ac000", "cs_r12_"
                                                            "0x002ac000"},
    {{vs, r3, 0x00000ff0}, false, al, "vs r3 0x00000ff0", "vs_r3_0x00000ff0"},
    {{cc, r3, 0xb000000a}, false, al, "cc r3 0xb000000a", "cc_r3_0xb000000a"},
    {{ge, r10, 0x002ac000}, false, al, "ge r10 0x002ac000", "ge_r10_"
                                                            "0x002ac000"},
    {{vs, r7, 0xc000003f}, false, al, "vs r7 0xc000003f", "vs_r7_0xc000003f"},
    {{pl, r2, 0x002ac000}, false, al, "pl r2 0x002ac000", "pl_r2_0x002ac000"},
    {{lt, r15, 0x0ff00000}, false, al, "lt r15 0x0ff00000", "lt_r15_"
                                                            "0x0ff00000"},
    {{al, r5, 0x00000000}, false, al, "al r5 0x00000000", "al_r5_0x00000000"},
    {{le, r5, 0x3fc00000}, false, al, "le r5 0x3fc00000", "le_r5_0x3fc00000"},
    {{pl, r0, 0x02ac0000}, false, al, "pl r0 0x02ac0000", "pl_r0_0x02ac0000"},
    {{pl, r2, 0x00ab0000}, false, al, "pl r2 0x00ab0000", "pl_r2_0x00ab0000"},
    {{vc, r15, 0xab000000}, false, al, "vc r15 0xab000000", "vc_r15_"
                                                            "0xab000000"},
    {{le, r12, 0x003fc000}, false, al, "le r12 0x003fc000", "le_r12_"
                                                            "0x003fc000"},
    {{ne, r2, 0x00000ff0}, false, al, "ne r2 0x00000ff0", "ne_r2_0x00000ff0"},
    {{ls, r15, 0x000003fc}, false, al, "ls r15 0x000003fc", "ls_r15_"
                                                            "0x000003fc"},
    {{vs, r7, 0x0ff00000}, false, al, "vs r7 0x0ff00000", "vs_r7_0x0ff00000"},
    {{ls, r5, 0x0000ab00}, false, al, "ls r5 0x0000ab00", "ls_r5_0x0000ab00"},
    {{lt, r9, 0x00ab0000}, false, al, "lt r9 0x00ab0000", "lt_r9_0x00ab0000"},
    {{vc, r13, 0x00003fc0}, false, al, "vc r13 0x00003fc0", "vc_r13_"
                                                            "0x00003fc0"},
    {{al, r1, 0xf000000f}, false, al, "al r1 0xf000000f", "al_r1_0xf000000f"},
    {{ne, r15, 0x000002ac}, false, al, "ne r15 0x000002ac", "ne_r15_"
                                                            "0x000002ac"},
    {{hi, r0, 0x2ac00000}, false, al, "hi r0 0x2ac00000", "hi_r0_0x2ac00000"},
    {{ls, r3, 0x03fc0000}, false, al, "ls r3 0x03fc0000", "ls_r3_0x03fc0000"},
    {{mi, r9, 0x0002ac00}, false, al, "mi r9 0x0002ac00", "mi_r9_0x0002ac00"},
    {{ls, r15, 0x000002ac}, false, al, "ls r15 0x000002ac", "ls_r15_"
                                                            "0x000002ac"},
    {{al, r1, 0x00002ac0}, false, al, "al r1 0x00002ac0", "al_r1_0x00002ac0"},
    {{ls, r12, 0x2ac00000}, false, al, "ls r12 0x2ac00000", "ls_r12_"
                                                            "0x2ac00000"},
    {{lt, r10, 0xc000002a}, false, al, "lt r10 0xc000002a", "lt_r10_"
                                                            "0xc000002a"},
    {{eq, r1, 0xc000002a}, false, al, "eq r1 0xc000002a", "eq_r1_0xc000002a"},
    {{ne, r8, 0xab000000}, false, al, "ne r8 0xab000000", "ne_r8_0xab000000"},
    {{lt, r10, 0xb000000a}, false, al, "lt r10 0xb000000a", "lt_r10_"
                                                            "0xb000000a"},
    {{le, r7, 0x00002ac0}, false, al, "le r7 0x00002ac0", "le_r7_0x00002ac0"},
    {{cs, r8, 0xf000000f}, false, al, "cs r8 0xf000000f", "cs_r8_0xf000000f"},
    {{cs, r9, 0xff000000}, false, al, "cs r9 0xff000000", "cs_r9_0xff000000"},
    {{vs, r3, 0x003fc000}, false, al, "vs r3 0x003fc000", "vs_r3_0x003fc000"},
    {{ge, r6, 0x003fc000}, false, al, "ge r6 0x003fc000", "ge_r6_0x003fc000"},
    {{le, r14, 0xab000000}, false, al, "le r14 0xab000000", "le_r14_"
                                                            "0xab000000"},
    {{mi, r4, 0xc000002a}, false, al, "mi r4 0xc000002a", "mi_r4_0xc000002a"},
    {{pl, r0, 0x00000ab0}, false, al, "pl r0 0x00000ab0", "pl_r0_0x00000ab0"},
    {{mi, r7, 0x000ff000}, false, al, "mi r7 0x000ff000", "mi_r7_0x000ff000"},
    {{cs, r1, 0x000ab000}, false, al, "cs r1 0x000ab000", "cs_r1_0x000ab000"},
    {{lt, r10, 0xac000002}, false, al, "lt r10 0xac000002", "lt_r10_"
                                                            "0xac000002"},
    {{eq, r1, 0xb000000a}, false, al, "eq r1 0xb000000a", "eq_r1_0xb000000a"},
    {{ne, r8, 0x2ac00000}, false, al, "ne r8 0x2ac00000", "ne_r8_0x2ac00000"},
    {{lt, r14, 0x2ac00000}, false, al, "lt r14 0x2ac00000", "lt_r14_"
                                                            "0x2ac00000"},
    {{eq, r12, 0x000000ff}, false, al, "eq r12 0x000000ff", "eq_r12_"
                                                            "0x000000ff"},
    {{pl, r15, 0x0000ab00}, false, al, "pl r15 0x0000ab00", "pl_r15_"
                                                            "0x0000ab00"},
    {{vs, r11, 0x0000ff00}, false, al, "vs r11 0x0000ff00", "vs_r11_"
                                                            "0x0000ff00"},
    {{eq, r11, 0x000ab000}, false, al, "eq r11 0x000ab000", "eq_r11_"
                                                            "0x000ab000"},
    {{gt, r5, 0x2ac00000}, false, al, "gt r5 0x2ac00000", "gt_r5_0x2ac00000"},
    {{ls, r8, 0x2ac00000}, false, al, "ls r8 0x2ac00000", "ls_r8_0x2ac00000"},
    {{ge, r1, 0x00003fc0}, false, al, "ge r1 0x00003fc0", "ge_r1_0x00003fc0"},
    {{vs, r7, 0xac000002}, false, al, "vs r7 0xac000002", "vs_r7_0xac000002"},
    {{vc, r8, 0x00ff0000}, false, al, "vc r8 0x00ff0000", "vc_r8_0x00ff0000"},
    {{eq, r11, 0x00002ac0}, false, al, "eq r11 0x00002ac0", "eq_r11_"
                                                            "0x00002ac0"},
    {{vs, r3, 0x0ff00000}, false, al, "vs r3 0x0ff00000", "vs_r3_0x0ff00000"},
    {{hi, r7, 0x3fc00000}, false, al, "hi r7 0x3fc00000", "hi_r7_0x3fc00000"},
    {{cc, r3, 0x0003fc00}, false, al, "cc r3 0x0003fc00", "cc_r3_0x0003fc00"},
    {{hi, r9, 0xf000000f}, false, al, "hi r9 0xf000000f", "hi_r9_0xf000000f"},
    {{cs, r6, 0x00003fc0}, false, al, "cs r6 0x00003fc0", "cs_r6_0x00003fc0"},
    {{ne, r12, 0x0000ff00}, false, al, "ne r12 0x0000ff00", "ne_r12_"
                                                            "0x0000ff00"},
    {{vc, r10, 0xff000000}, false, al, "vc r10 0xff000000", "vc_r10_"
                                                            "0xff000000"},
    {{lt, r12, 0x3fc00000}, false, al, "lt r12 0x3fc00000", "lt_r12_"
                                                            "0x3fc00000"},
    {{cs, r1, 0x000002ac}, false, al, "cs r1 0x000002ac", "cs_r1_0x000002ac"},
    {{ls, r1, 0x00003fc0}, false, al, "ls r1 0x00003fc0", "ls_r1_0x00003fc0"},
    {{pl, r0, 0x0000ab00}, false, al, "pl r0 0x0000ab00", "pl_r0_0x0000ab00"},
    {{cc, r14, 0x0000ab00}, false, al, "cc r14 0x0000ab00", "cc_r14_"
                                                            "0x0000ab00"},
    {{ne, r8, 0x0003fc00}, false, al, "ne r8 0x0003fc00", "ne_r8_0x0003fc00"},
    {{pl, r7, 0x000ff000}, false, al, "pl r7 0x000ff000", "pl_r7_0x000ff000"},
    {{vc, r2, 0x0000ff00}, false, al, "vc r2 0x0000ff00", "vc_r2_0x0000ff00"},
    {{vc, r4, 0x02ac0000}, false, al, "vc r4 0x02ac0000", "vc_r4_0x02ac0000"},
    {{ne, r14, 0x00000ab0}, false, al, "ne r14 0x00000ab0", "ne_r14_"
                                                            "0x00000ab0"},
    {{al, r11, 0x0000ab00}, false, al, "al r11 0x0000ab00", "al_r11_"
                                                            "0x0000ab00"},
    {{mi, r12, 0x00000ab0}, false, al, "mi r12 0x00000ab0", "mi_r12_"
                                                            "0x00000ab0"},
    {{hi, r12, 0xff000000}, false, al, "hi r12 0xff000000", "hi_r12_"
                                                            "0xff000000"},
    {{ge, r7, 0x002ac000}, false, al, "ge r7 0x002ac000", "ge_r7_0x002ac000"},
    {{ge, r14, 0xf000000f}, false, al, "ge r14 0xf000000f", "ge_r14_"
                                                            "0xf000000f"},
    {{ge, r2, 0x000002ac}, false, al, "ge r2 0x000002ac", "ge_r2_0x000002ac"},
    {{vs, r12, 0xff000000}, false, al, "vs r12 0xff000000", "vs_r12_"
                                                            "0xff000000"},
    {{le, r1, 0x002ac000}, false, al, "le r1 0x002ac000", "le_r1_0x002ac000"},
    {{pl, r13, 0x02ac0000}, false, al, "pl r13 0x02ac0000", "pl_r13_"
                                                            "0x02ac0000"},
    {{cs, r8, 0x00ab0000}, false, al, "cs r8 0x00ab0000", "cs_r8_0x00ab0000"},
    {{mi, r8, 0x00000000}, false, al, "mi r8 0x00000000", "mi_r8_0x00000000"},
    {{hi, r0, 0x00000000}, false, al, "hi r0 0x00000000", "hi_r0_0x00000000"},
    {{vs, r1, 0x0ff00000}, false, al, "vs r1 0x0ff00000", "vs_r1_0x0ff00000"},
    {{al, r15, 0x02ac0000}, false, al, "al r15 0x02ac0000", "al_r15_"
                                                            "0x02ac0000"},
    {{gt, r13, 0x2ac00000}, false, al, "gt r13 0x2ac00000", "gt_r13_"
                                                            "0x2ac00000"},
    {{hi, r11, 0x000ab000}, false, al, "hi r11 0x000ab000", "hi_r11_"
                                                            "0x000ab000"},
    {{ge, r4, 0x000ff000}, false, al, "ge r4 0x000ff000", "ge_r4_0x000ff000"},
    {{ge, r12, 0x000000ab}, false, al, "ge r12 0x000000ab", "ge_r12_"
                                                            "0x000000ab"},
    {{vc, r10, 0x003fc000}, false, al, "vc r10 0x003fc000", "vc_r10_"
                                                            "0x003fc000"},
    {{al, r2, 0x000002ac}, false, al, "al r2 0x000002ac", "al_r2_0x000002ac"},
    {{le, r7, 0x00000000}, false, al, "le r7 0x00000000", "le_r7_0x00000000"},
    {{ne, r2, 0xb000000a}, false, al, "ne r2 0xb000000a", "ne_r2_0xb000000a"},
    {{hi, r2, 0xf000000f}, false, al, "hi r2 0xf000000f", "hi_r2_0xf000000f"},
    {{mi, r13, 0x00ff0000}, false, al, "mi r13 0x00ff0000", "mi_r13_"
                                                            "0x00ff0000"},
    {{al, r3, 0x0ff00000}, false, al, "al r3 0x0ff00000", "al_r3_0x0ff00000"},
    {{ls, r2, 0xc000003f}, false, al, "ls r2 0xc000003f", "ls_r2_0xc000003f"},
    {{al, r14, 0x2ac00000}, false, al, "al r14 0x2ac00000", "al_r14_"
                                                            "0x2ac00000"},
    {{al, r13, 0x002ac000}, false, al, "al r13 0x002ac000", "al_r13_"
                                                            "0x002ac000"},
    {{eq, r2, 0x0ff00000}, false, al, "eq r2 0x0ff00000", "eq_r2_0x0ff00000"},
    {{mi, r10, 0x3fc00000}, false, al, "mi r10 0x3fc00000", "mi_r10_"
                                                            "0x3fc00000"},
    {{ge, r11, 0x0000ab00}, false, al, "ge r11 0x0000ab00", "ge_r11_"
                                                            "0x0000ab00"},
    {{gt, r2, 0x000ab000}, false, al, "gt r2 0x000ab000", "gt_r2_0x000ab000"},
    {{lt, r11, 0x00ff0000}, false, al, "lt r11 0x00ff0000", "lt_r11_"
                                                            "0x00ff0000"},
    {{mi, r2, 0xf000000f}, false, al, "mi r2 0xf000000f", "mi_r2_0xf000000f"},
    {{ge, r9, 0x00000000}, false, al, "ge r9 0x00000000", "ge_r9_0x00000000"},
    {{lt, r1, 0x000000ab}, false, al, "lt r1 0x000000ab", "lt_r1_0x000000ab"},
    {{cc, r2, 0x000ab000}, false, al, "cc r2 0x000ab000", "cc_r2_0x000ab000"},
    {{ne, r6, 0x000003fc}, false, al, "ne r6 0x000003fc", "ne_r6_0x000003fc"},
    {{eq, r10, 0x000002ac}, false, al, "eq r10 0x000002ac", "eq_r10_"
                                                            "0x000002ac"},
    {{cc, r14, 0x000ff000}, false, al, "cc r14 0x000ff000", "cc_r14_"
                                                            "0x000ff000"},
    {{ne, r12, 0x0000ab00}, false, al, "ne r12 0x0000ab00", "ne_r12_"
                                                            "0x0000ab00"},
    {{lt, r12, 0x003fc000}, false, al, "lt r12 0x003fc000", "lt_r12_"
                                                            "0x003fc000"},
    {{vs, r13, 0xf000000f}, false, al, "vs r13 0xf000000f", "vs_r13_"
                                                            "0xf000000f"},
    {{ne, r5, 0x00003fc0}, false, al, "ne r5 0x00003fc0", "ne_r5_0x00003fc0"},
    {{ne, r11, 0x02ac0000}, false, al, "ne r11 0x02ac0000", "ne_r11_"
                                                            "0x02ac0000"},
    {{cs, r3, 0xc000002a}, false, al, "cs r3 0xc000002a", "cs_r3_0xc000002a"},
    {{gt, r9, 0xc000003f}, false, al, "gt r9 0xc000003f", "gt_r9_0xc000003f"},
    {{vc, r4, 0x03fc0000}, false, al, "vc r4 0x03fc0000", "vc_r4_0x03fc0000"},
    {{vs, r5, 0x0ff00000}, false, al, "vs r5 0x0ff00000", "vs_r5_0x0ff00000"},
    {{ls, r8, 0xab000000}, false, al, "ls r8 0xab000000", "ls_r8_0xab000000"},
    {{gt, r1, 0x0003fc00}, false, al, "gt r1 0x0003fc00", "gt_r1_0x0003fc00"},
    {{ls, r13, 0x0003fc00}, false, al, "ls r13 0x0003fc00", "ls_r13_"
                                                            "0x0003fc00"},
    {{ne, r3, 0x000ff000}, false, al, "ne r3 0x000ff000", "ne_r3_0x000ff000"},
    {{cs, r5, 0x000ff000}, false, al, "cs r5 0x000ff000", "cs_r5_0x000ff000"},
    {{eq, r9, 0x0000ff00}, false, al, "eq r9 0x0000ff00", "eq_r9_0x0000ff00"},
    {{gt, r0, 0xab000000}, false, al, "gt r0 0xab000000", "gt_r0_0xab000000"},
    {{hi, r4, 0xf000000f}, false, al, "hi r4 0xf000000f", "hi_r4_0xf000000f"},
    {{ne, r3, 0x00002ac0}, false, al, "ne r3 0x00002ac0", "ne_r3_0x00002ac0"},
    {{hi, r5, 0xc000002a}, false, al, "hi r5 0xc000002a", "hi_r5_0xc000002a"},
    {{vs, r5, 0x000003fc}, false, al, "vs r5 0x000003fc", "vs_r5_0x000003fc"},
    {{ls, r4, 0x00ab0000}, false, al, "ls r4 0x00ab0000", "ls_r4_0x00ab0000"},
    {{lt, r6, 0xc000003f}, false, al, "lt r6 0xc000003f", "lt_r6_0xc000003f"},
    {{lt, r14, 0x00000ab0}, false, al, "lt r14 0x00000ab0", "lt_r14_"
                                                            "0x00000ab0"},
    {{ge, r0, 0xac000002}, false, al, "ge r0 0xac000002", "ge_r0_0xac000002"},
    {{le, r12, 0xac000002}, false, al, "le r12 0xac000002", "le_r12_"
                                                            "0xac000002"},
    {{cs, r11, 0x000ff000}, false, al, "cs r11 0x000ff000", "cs_r11_"
                                                            "0x000ff000"},
    {{cs, r7, 0xab000000}, false, al, "cs r7 0xab000000", "cs_r7_0xab000000"},
    {{lt, r2, 0x00ab0000}, false, al, "lt r2 0x00ab0000", "lt_r2_0x00ab0000"},
    {{lt, r3, 0xf000000f}, false, al, "lt r3 0xf000000f", "lt_r3_0xf000000f"},
    {{le, r14, 0x0003fc00}, false, al, "le r14 0x0003fc00", "le_r14_"
                                                            "0x0003fc00"},
    {{hi, r2, 0x0ff00000}, false, al, "hi r2 0x0ff00000", "hi_r2_0x0ff00000"},
    {{ge, r10, 0xf000000f}, false, al, "ge r10 0xf000000f", "ge_r10_"
                                                            "0xf000000f"},
    {{ls, r11, 0x000003fc}, false, al, "ls r11 0x000003fc", "ls_r11_"
                                                            "0x000003fc"},
    {{vc, r0, 0x0003fc00}, false, al, "vc r0 0x0003fc00", "vc_r0_0x0003fc00"},
    {{vc, r4, 0x0000ff00}, false, al, "vc r4 0x0000ff00", "vc_r4_0x0000ff00"},
    {{cs, r0, 0x2ac00000}, false, al, "cs r0 0x2ac00000", "cs_r0_0x2ac00000"},
    {{ne, r1, 0x000ab000}, false, al, "ne r1 0x000ab000", "ne_r1_0x000ab000"},
    {{mi, r6, 0x000ff000}, false, al, "mi r6 0x000ff000", "mi_r6_0x000ff000"},
    {{al, r1, 0x000002ac}, false, al, "al r1 0x000002ac", "al_r1_0x000002ac"},
    {{al, r11, 0x0000ff00}, false, al, "al r11 0x0000ff00", "al_r11_"
                                                            "0x0000ff00"},
    {{cs, r0, 0x000ff000}, false, al, "cs r0 0x000ff000", "cs_r0_0x000ff000"},
    {{hi, r5, 0xab000000}, false, al, "hi r5 0xab000000", "hi_r5_0xab000000"},
    {{eq, r2, 0x0000ff00}, false, al, "eq r2 0x0000ff00", "eq_r2_0x0000ff00"},
    {{hi, r9, 0xab000000}, false, al, "hi r9 0xab000000", "hi_r9_0xab000000"},
    {{gt, r4, 0xff000000}, false, al, "gt r4 0xff000000", "gt_r4_0xff000000"},
    {{ne, r2, 0x003fc000}, false, al, "ne r2 0x003fc000", "ne_r2_0x003fc000"},
    {{eq, r4, 0x0000ff00}, false, al, "eq r4 0x0000ff00", "eq_r4_0x0000ff00"},
    {{vc, r13, 0x002ac000}, false, al, "vc r13 0x002ac000", "vc_r13_"
                                                            "0x002ac000"},
    {{pl, r5, 0xff000000}, false, al, "pl r5 0xff000000", "pl_r5_0xff000000"},
    {{cs, r13, 0x00000000}, false, al, "cs r13 0x00000000", "cs_r13_"
                                                            "0x00000000"},
    {{lt, r8, 0x00002ac0}, false, al, "lt r8 0x00002ac0", "lt_r8_0x00002ac0"},
    {{le, r1, 0x3fc00000}, false, al, "le r1 0x3fc00000", "le_r1_0x3fc00000"},
    {{vs, r13, 0x000ff000}, false, al, "vs r13 0x000ff000", "vs_r13_"
                                                            "0x000ff000"},
    {{vc, r8, 0x0ab00000}, false, al, "vc r8 0x0ab00000", "vc_r8_0x0ab00000"},
    {{lt, r7, 0x000000ff}, false, al, "lt r7 0x000000ff", "lt_r7_0x000000ff"},
    {{eq, r7, 0x000000ff}, false, al, "eq r7 0x000000ff", "eq_r7_0x000000ff"},
    {{ne, r6, 0x00002ac0}, false, al, "ne r6 0x00002ac0", "ne_r6_0x00002ac0"},
    {{eq, r10, 0x0000ab00}, false, al, "eq r10 0x0000ab00", "eq_r10_"
                                                            "0x0000ab00"},
    {{mi, r10, 0x00ab0000}, false, al, "mi r10 0x00ab0000", "mi_r10_"
                                                            "0x00ab0000"},
    {{hi, r7, 0x0000ff00}, false, al, "hi r7 0x0000ff00", "hi_r7_0x0000ff00"},
    {{vc, r7, 0x000000ab}, false, al, "vc r7 0x000000ab", "vc_r7_0x000000ab"},
    {{ge, r6, 0x03fc0000}, false, al, "ge r6 0x03fc0000", "ge_r6_0x03fc0000"},
    {{ge, r5, 0x0002ac00}, false, al, "ge r5 0x0002ac00", "ge_r5_0x0002ac00"},
    {{eq, r3, 0x0000ab00}, false, al, "eq r3 0x0000ab00", "eq_r3_0x0000ab00"},
    {{gt, r3, 0xc000002a}, false, al, "gt r3 0xc000002a", "gt_r3_0xc000002a"},
    {{lt, r10, 0x0ab00000}, false, al, "lt r10 0x0ab00000", "lt_r10_"
                                                            "0x0ab00000"},
    {{pl, r6, 0x00003fc0}, false, al, "pl r6 0x00003fc0", "pl_r6_0x00003fc0"},
    {{pl, r3, 0x00000ff0}, false, al, "pl r3 0x00000ff0", "pl_r3_0x00000ff0"},
    {{cc, r8, 0x0002ac00}, false, al, "cc r8 0x0002ac00", "cc_r8_0x0002ac00"},
    {{ls, r14, 0x0002ac00}, false, al, "ls r14 0x0002ac00", "ls_r14_"
                                                            "0x0002ac00"},
    {{hi, r11, 0xab000000}, false, al, "hi r11 0xab000000", "hi_r11_"
                                                            "0xab000000"},
    {{mi, r2, 0x0ab00000}, false, al, "mi r2 0x0ab00000", "mi_r2_0x0ab00000"},
    {{al, r6, 0x00000000}, false, al, "al r6 0x00000000", "al_r6_0x00000000"},
    {{al, r13, 0x000003fc}, false, al, "al r13 0x000003fc", "al_r13_"
                                                            "0x000003fc"},
    {{ge, r4, 0xb000000a}, false, al, "ge r4 0xb000000a", "ge_r4_0xb000000a"},
    {{mi, r14, 0xc000002a}, false, al, "mi r14 0xc000002a", "mi_r14_"
                                                            "0xc000002a"},
    {{vc, r6, 0xf000000f}, false, al, "vc r6 0xf000000f", "vc_r6_0xf000000f"},
    {{eq, r3, 0x03fc0000}, false, al, "eq r3 0x03fc0000", "eq_r3_0x03fc0000"},
    {{vc, r3, 0xf000000f}, false, al, "vc r3 0xf000000f", "vc_r3_0xf000000f"},
    {{lt, r12, 0xb000000a}, false, al, "lt r12 0xb000000a", "lt_r12_"
                                                            "0xb000000a"},
    {{vs, r2, 0xff000000}, false, al, "vs r2 0xff000000", "vs_r2_0xff000000"},
    {{ls, r2, 0x003fc000}, false, al, "ls r2 0x003fc000", "ls_r2_0x003fc000"},
    {{gt, r11, 0xab000000}, false, al, "gt r11 0xab000000", "gt_r11_"
                                                            "0xab000000"},
    {{ne, r15, 0x00002ac0}, false, al, "ne r15 0x00002ac0", "ne_r15_"
                                                            "0x00002ac0"},
    {{ne, r4, 0x0002ac00}, false, al, "ne r4 0x0002ac00", "ne_r4_0x0002ac00"},
    {{al, r0, 0x000ab000}, false, al, "al r0 0x000ab000", "al_r0_0x000ab000"},
    {{le, r9, 0xc000003f}, false, al, "le r9 0xc000003f", "le_r9_0xc000003f"},
    {{le, r5, 0x000ab000}, false, al, "le r5 0x000ab000", "le_r5_0x000ab000"},
    {{mi, r4, 0x000000ff}, false, al, "mi r4 0x000000ff", "mi_r4_0x000000ff"},
    {{vc, r12, 0x0002ac00}, false, al, "vc r12 0x0002ac00", "vc_r12_"
                                                            "0x0002ac00"},
    {{eq, r11, 0x0000ab00}, false, al, "eq r11 0x0000ab00", "eq_r11_"
                                                            "0x0000ab00"},
    {{al, r4, 0x000000ff}, false, al, "al r4 0x000000ff", "al_r4_0x000000ff"},
    {{ls, r10, 0xfc000003}, false, al, "ls r10 0xfc000003", "ls_r10_"
                                                            "0xfc000003"},
    {{vs, r6, 0x000000ab}, false, al, "vs r6 0x000000ab", "vs_r6_0x000000ab"},
    {{vs, r9, 0x02ac0000}, false, al, "vs r9 0x02ac0000", "vs_r9_0x02ac0000"},
    {{ge, r6, 0x00000ab0}, false, al, "ge r6 0x00000ab0", "ge_r6_0x00000ab0"},
    {{le, r8, 0x00002ac0}, false, al, "le r8 0x00002ac0", "le_r8_0x00002ac0"},
    {{mi, r15, 0xac000002}, false, al, "mi r15 0xac000002", "mi_r15_"
                                                            "0xac000002"},
    {{hi, r10, 0x0003fc00}, false, al, "hi r10 0x0003fc00", "hi_r10_"
                                                            "0x0003fc00"},
    {{ge, r2, 0x03fc0000}, false, al, "ge r2 0x03fc0000", "ge_r2_0x03fc0000"},
    {{mi, r13, 0x00000ff0}, false, al, "mi r13 0x00000ff0", "mi_r13_"
                                                            "0x00000ff0"},
    {{vc, r12, 0xff000000}, false, al, "vc r12 0xff000000", "vc_r12_"
                                                            "0xff000000"},
    {{cc, r0, 0xc000003f}, false, al, "cc r0 0xc000003f", "cc_r0_0xc000003f"},
    {{pl, r11, 0x000003fc}, false, al, "pl r11 0x000003fc", "pl_r11_"
                                                            "0x000003fc"},
    {{gt, r1, 0x0002ac00}, false, al, "gt r1 0x0002ac00", "gt_r1_0x0002ac00"},
    {{pl, r14, 0xb000000a}, false, al, "pl r14 0xb000000a", "pl_r14_"
                                                            "0xb000000a"},
    {{mi, r5, 0x2ac00000}, false, al, "mi r5 0x2ac00000", "mi_r5_0x2ac00000"},
    {{lt, r15, 0x03fc0000}, false, al, "lt r15 0x03fc0000", "lt_r15_"
                                                            "0x03fc0000"},
    {{ne, r0, 0x00000ab0}, false, al, "ne r0 0x00000ab0", "ne_r0_0x00000ab0"},
    {{mi, r7, 0x000003fc}, false, al, "mi r7 0x000003fc", "mi_r7_0x000003fc"},
    {{ls, r7, 0xab000000}, false, al, "ls r7 0xab000000", "ls_r7_0xab000000"},
    {{lt, r7, 0x002ac000}, false, al, "lt r7 0x002ac000", "lt_r7_0x002ac000"},
    {{cc, r14, 0x00000000}, false, al, "cc r14 0x00000000", "cc_r14_"
                                                            "0x00000000"},
    {{le, r7, 0x2ac00000}, false, al, "le r7 0x2ac00000", "le_r7_0x2ac00000"},
    {{mi, r8, 0xff000000}, false, al, "mi r8 0xff000000", "mi_r8_0xff000000"},
    {{ls, r4, 0xc000003f}, false, al, "ls r4 0xc000003f", "ls_r4_0xc000003f"},
    {{al, r8, 0x000ab000}, false, al, "al r8 0x000ab000", "al_r8_0x000ab000"},
    {{ge, r5, 0xab000000}, false, al, "ge r5 0xab000000", "ge_r5_0xab000000"},
    {{pl, r9, 0x000000ab}, false, al, "pl r9 0x000000ab", "pl_r9_0x000000ab"},
    {{cs, r8, 0x002ac000}, false, al, "cs r8 0x002ac000", "cs_r8_0x002ac000"},
    {{hi, r3, 0x0ff00000}, false, al, "hi r3 0x0ff00000", "hi_r3_0x0ff00000"},
    {{ls, r10, 0x000003fc}, false, al, "ls r10 0x000003fc", "ls_r10_"
                                                            "0x000003fc"},
    {{le, r14, 0x00ab0000}, false, al, "le r14 0x00ab0000", "le_r14_"
                                                            "0x00ab0000"},
    {{hi, r5, 0x0003fc00}, false, al, "hi r5 0x0003fc00", "hi_r5_0x0003fc00"},
    {{cs, r14, 0x0ff00000}, false, al, "cs r14 0x0ff00000", "cs_r14_"
                                                            "0x0ff00000"},
    {{le, r2, 0x0002ac00}, false, al, "le r2 0x0002ac00", "le_r2_0x0002ac00"},
    {{cc, r10, 0x00000000}, false, al, "cc r10 0x00000000", "cc_r10_"
                                                            "0x00000000"},
    {{vs, r13, 0xc000002a}, false, al, "vs r13 0xc000002a", "vs_r13_"
                                                            "0xc000002a"},
    {{ls, r1, 0x003fc000}, false, al, "ls r1 0x003fc000", "ls_r1_0x003fc000"},
    {{cc, r15, 0x0003fc00}, false, al, "cc r15 0x0003fc00", "cc_r15_"
                                                            "0x0003fc00"},
    {{eq, r6, 0x03fc0000}, false, al, "eq r6 0x03fc0000", "eq_r6_0x03fc0000"},
    {{cs, r12, 0x0000ab00}, false, al, "cs r12 0x0000ab00", "cs_r12_"
                                                            "0x0000ab00"},
    {{eq, r10, 0x0003fc00}, false, al, "eq r10 0x0003fc00", "eq_r10_"
                                                            "0x0003fc00"},
    {{mi, r1, 0xf000000f}, false, al, "mi r1 0xf000000f", "mi_r1_0xf000000f"},
    {{pl, r0, 0x000ab000}, false, al, "pl r0 0x000ab000", "pl_r0_0x000ab000"},
    {{eq, r3, 0x3fc00000}, false, al, "eq r3 0x3fc00000", "eq_r3_0x3fc00000"},
    {{cs, r5, 0x0003fc00}, false, al, "cs r5 0x0003fc00", "cs_r5_0x0003fc00"},
    {{hi, r8, 0x03fc0000}, false, al, "hi r8 0x03fc0000", "hi_r8_0x03fc0000"},
    {{gt, r5, 0x00ff0000}, false, al, "gt r5 0x00ff0000", "gt_r5_0x00ff0000"},
    {{mi, r12, 0x000ab000}, false, al, "mi r12 0x000ab000", "mi_r12_"
                                                            "0x000ab000"},
    {{ge, r12, 0xf000000f}, false, al, "ge r12 0xf000000f", "ge_r12_"
                                                            "0xf000000f"},
    {{ne, r3, 0xff000000}, false, al, "ne r3 0xff000000", "ne_r3_0xff000000"},
    {{ne, r10, 0x0002ac00}, false, al, "ne r10 0x0002ac00", "ne_r10_"
                                                            "0x0002ac00"},
    {{pl, r14, 0x0000ab00}, false, al, "pl r14 0x0000ab00", "pl_r14_"
                                                            "0x0000ab00"},
    {{mi, r12, 0x000000ff}, false, al, "mi r12 0x000000ff", "mi_r12_"
                                                            "0x000000ff"},
    {{lt, r15, 0x00000000}, false, al, "lt r15 0x00000000", "lt_r15_"
                                                            "0x00000000"},
    {{vc, r9, 0xff000000}, false, al, "vc r9 0xff000000", "vc_r9_0xff000000"},
    {{eq, r1, 0x0000ff00}, false, al, "eq r1 0x0000ff00", "eq_r1_0x0000ff00"},
    {{lt, r4, 0x000ff000}, false, al, "lt r4 0x000ff000", "lt_r4_0x000ff000"},
    {{ne, r10, 0x00000ff0}, false, al, "ne r10 0x00000ff0", "ne_r10_"
                                                            "0x00000ff0"},
    {{al, r4, 0x000ab000}, false, al, "al r4 0x000ab000", "al_r4_0x000ab000"},
    {{ne, r7, 0x000003fc}, false, al, "ne r7 0x000003fc", "ne_r7_0x000003fc"},
    {{cs, r5, 0x003fc000}, false, al, "cs r5 0x003fc000", "cs_r5_0x003fc000"},
    {{mi, r4, 0x0000ab00}, false, al, "mi r4 0x0000ab00", "mi_r4_0x0000ab00"},
    {{gt, r3, 0x03fc0000}, false, al, "gt r3 0x03fc0000", "gt_r3_0x03fc0000"},
    {{gt, r2, 0x00000ab0}, false, al, "gt r2 0x00000ab0", "gt_r2_0x00000ab0"},
    {{ne, r9, 0x0ab00000}, false, al, "ne r9 0x0ab00000", "ne_r9_0x0ab00000"},
    {{pl, r10, 0x000ab000}, false, al, "pl r10 0x000ab000", "pl_r10_"
                                                            "0x000ab000"},
    {{vs, r9, 0x0ab00000}, false, al, "vs r9 0x0ab00000", "vs_r9_0x0ab00000"},
    {{vc, r10, 0xfc000003}, false, al, "vc r10 0xfc000003", "vc_r10_"
                                                            "0xfc000003"},
    {{ne, r9, 0x00000000}, false, al, "ne r9 0x00000000", "ne_r9_0x00000000"},
    {{hi, r14, 0x000ff000}, false, al, "hi r14 0x000ff000", "hi_r14_"
                                                            "0x000ff000"},
    {{al, r9, 0xac000002}, false, al, "al r9 0xac000002", "al_r9_0xac000002"},
    {{mi, r11, 0xf000000f}, false, al, "mi r11 0xf000000f", "mi_r11_"
                                                            "0xf000000f"},
    {{ls, r1, 0x03fc0000}, false, al, "ls r1 0x03fc0000", "ls_r1_0x03fc0000"},
    {{le, r14, 0xc000003f}, false, al, "le r14 0xc000003f", "le_r14_"
                                                            "0xc000003f"},
    {{vs, r2, 0x00000ff0}, false, al, "vs r2 0x00000ff0", "vs_r2_0x00000ff0"},
    {{cc, r0, 0xab000000}, false, al, "cc r0 0xab000000", "cc_r0_0xab000000"},
    {{mi, r10, 0x00000000}, false, al, "mi r10 0x00000000", "mi_r10_"
                                                            "0x00000000"},
    {{ne, r11, 0x03fc0000}, false, al, "ne r11 0x03fc0000", "ne_r11_"
                                                            "0x03fc0000"},
    {{ne, r7, 0x00000ff0}, false, al, "ne r7 0x00000ff0", "ne_r7_0x00000ff0"},
    {{hi, r7, 0x0003fc00}, false, al, "hi r7 0x0003fc00", "hi_r7_0x0003fc00"},
    {{vc, r7, 0x000ab000}, false, al, "vc r7 0x000ab000", "vc_r7_0x000ab000"},
    {{al, r2, 0xc000002a}, false, al, "al r2 0xc000002a", "al_r2_0xc000002a"},
    {{ls, r15, 0x00003fc0}, false, al, "ls r15 0x00003fc0", "ls_r15_"
                                                            "0x00003fc0"},
    {{cc, r4, 0x000000ab}, false, al, "cc r4 0x000000ab", "cc_r4_0x000000ab"},
    {{gt, r7, 0x02ac0000}, false, al, "gt r7 0x02ac0000", "gt_r7_0x02ac0000"},
    {{cc, r12, 0x00003fc0}, false, al, "cc r12 0x00003fc0", "cc_r12_"
                                                            "0x00003fc0"},
    {{gt, r12, 0xc000002a}, false, al, "gt r12 0xc000002a", "gt_r12_"
                                                            "0xc000002a"},
    {{gt, r12, 0xfc000003}, false, al, "gt r12 0xfc000003", "gt_r12_"
                                                            "0xfc000003"},
    {{hi, r9, 0x2ac00000}, false, al, "hi r9 0x2ac00000", "hi_r9_0x2ac00000"},
    {{hi, r10, 0x00ab0000}, false, al, "hi r10 0x00ab0000", "hi_r10_"
                                                            "0x00ab0000"},
    {{cc, r0, 0x0000ff00}, false, al, "cc r0 0x0000ff00", "cc_r0_0x0000ff00"},
    {{vc, r8, 0x0ff00000}, false, al, "vc r8 0x0ff00000", "vc_r8_0x0ff00000"},
    {{cc, r12, 0x002ac000}, false, al, "cc r12 0x002ac000", "cc_r12_"
                                                            "0x002ac000"},
    {{gt, r15, 0x002ac000}, false, al, "gt r15 0x002ac000", "gt_r15_"
                                                            "0x002ac000"},
    {{vs, r7, 0x03fc0000}, false, al, "vs r7 0x03fc0000", "vs_r7_0x03fc0000"},
    {{pl, r5, 0xab000000}, false, al, "pl r5 0xab000000", "pl_r5_0xab000000"},
    {{al, r7, 0x000000ab}, false, al, "al r7 0x000000ab", "al_r7_0x000000ab"},
    {{vs, r10, 0xff000000}, false, al, "vs r10 0xff000000", "vs_r10_"
                                                            "0xff000000"},
    {{cc, r13, 0xf000000f}, false, al, "cc r13 0xf000000f", "cc_r13_"
                                                            "0xf000000f"},
    {{cs, r2, 0x002ac000}, false, al, "cs r2 0x002ac000", "cs_r2_0x002ac000"},
    {{mi, r11, 0x0003fc00}, false, al, "mi r11 0x0003fc00", "mi_r11_"
                                                            "0x0003fc00"},
    {{ls, r11, 0x003fc000}, false, al, "ls r11 0x003fc000", "ls_r11_"
                                                            "0x003fc000"},
    {{eq, r4, 0x003fc000}, false, al, "eq r4 0x003fc000", "eq_r4_0x003fc000"},
    {{ls, r15, 0xac000002}, false, al, "ls r15 0xac000002", "ls_r15_"
                                                            "0xac000002"},
    {{ne, r3, 0x3fc00000}, false, al, "ne r3 0x3fc00000", "ne_r3_0x3fc00000"},
    {{hi, r2, 0x00ff0000}, false, al, "hi r2 0x00ff0000", "hi_r2_0x00ff0000"},
    {{ge, r3, 0x000ff000}, false, al, "ge r3 0x000ff000", "ge_r3_0x000ff000"},
    {{lt, r12, 0x02ac0000}, false, al, "lt r12 0x02ac0000", "lt_r12_"
                                                            "0x02ac0000"},
    {{lt, r9, 0x003fc000}, false, al, "lt r9 0x003fc000", "lt_r9_0x003fc000"},
    {{lt, r8, 0x000ab000}, false, al, "lt r8 0x000ab000", "lt_r8_0x000ab000"},
    {{vc, r7, 0xac000002}, false, al, "vc r7 0xac000002", "vc_r7_0xac000002"},
    {{cs, r12, 0x0002ac00}, false, al, "cs r12 0x0002ac00", "cs_r12_"
                                                            "0x0002ac00"},
    {{cc, r11, 0xc000002a}, false, al, "cc r11 0xc000002a", "cc_r11_"
                                                            "0xc000002a"},
    {{ne, r14, 0x003fc000}, false, al, "ne r14 0x003fc000", "ne_r14_"
                                                            "0x003fc000"},
    {{lt, r12, 0x0ab00000}, false, al, "lt r12 0x0ab00000", "lt_r12_"
                                                            "0x0ab00000"},
    {{mi, r4, 0x03fc0000}, false, al, "mi r4 0x03fc0000", "mi_r4_0x03fc0000"},
    {{ge, r6, 0x000ff000}, false, al, "ge r6 0x000ff000", "ge_r6_0x000ff000"},
    {{ls, r14, 0x00ff0000}, false, al, "ls r14 0x00ff0000", "ls_r14_"
                                                            "0x00ff0000"},
    {{pl, r13, 0x002ac000}, false, al, "pl r13 0x002ac000", "pl_r13_"
                                                            "0x002ac000"},
    {{vc, r3, 0x00000000}, false, al, "vc r3 0x00000000", "vc_r3_0x00000000"},
    {{le, r7, 0x00ff0000}, false, al, "le r7 0x00ff0000", "le_r7_0x00ff0000"},
    {{ne, r0, 0xac000002}, false, al, "ne r0 0xac000002", "ne_r0_0xac000002"},
    {{le, r10, 0xff000000}, false, al, "le r10 0xff000000", "le_r10_"
                                                            "0xff000000"},
    {{mi, r8, 0x3fc00000}, false, al, "mi r8 0x3fc00000", "mi_r8_0x3fc00000"},
    {{vc, r15, 0x0003fc00}, false, al, "vc r15 0x0003fc00", "vc_r15_"
                                                            "0x0003fc00"},
    {{mi, r15, 0x003fc000}, false, al, "mi r15 0x003fc000", "mi_r15_"
                                                            "0x003fc000"},
    {{vc, r1, 0x003fc000}, false, al, "vc r1 0x003fc000", "vc_r1_0x003fc000"},
    {{hi, r0, 0xff000000}, false, al, "hi r0 0xff000000", "hi_r0_0xff000000"},
    {{pl, r10, 0xf000000f}, false, al, "pl r10 0xf000000f", "pl_r10_"
                                                            "0xf000000f"},
    {{le, r9, 0x0ff00000}, false, al, "le r9 0x0ff00000", "le_r9_0x0ff00000"},
    {{ne, r8, 0x0000ab00}, false, al, "ne r8 0x0000ab00", "ne_r8_0x0000ab00"},
    {{vs, r10, 0x0ab00000}, false, al, "vs r10 0x0ab00000", "vs_r10_"
                                                            "0x0ab00000"},
    {{eq, r2, 0x02ac0000}, false, al, "eq r2 0x02ac0000", "eq_r2_0x02ac0000"},
    {{cs, r8, 0xac000002}, false, al, "cs r8 0xac000002", "cs_r8_0xac000002"},
    {{eq, r14, 0x0000ff00}, false, al, "eq r14 0x0000ff00", "eq_r14_"
                                                            "0x0000ff00"},
    {{ne, r7, 0x03fc0000}, false, al, "ne r7 0x03fc0000", "ne_r7_0x03fc0000"},
    {{hi, r10, 0x0002ac00}, false, al, "hi r10 0x0002ac00", "hi_r10_"
                                                            "0x0002ac00"},
    {{mi, r13, 0xf000000f}, false, al, "mi r13 0xf000000f", "mi_r13_"
                                                            "0xf000000f"},
    {{lt, r1, 0x002ac000}, false, al, "lt r1 0x002ac000", "lt_r1_0x002ac000"},
    {{ge, r13, 0x03fc0000}, false, al, "ge r13 0x03fc0000", "ge_r13_"
                                                            "0x03fc0000"},
    {{lt, r9, 0x0ff00000}, false, al, "lt r9 0x0ff00000", "lt_r9_0x0ff00000"},
    {{al, r13, 0xc000002a}, false, al, "al r13 0xc000002a", "al_r13_"
                                                            "0xc000002a"},
    {{cc, r12, 0x03fc0000}, false, al, "cc r12 0x03fc0000", "cc_r12_"
                                                            "0x03fc0000"},
    {{al, r11, 0xfc000003}, false, al, "al r11 0xfc000003", "al_r11_"
                                                            "0xfc000003"},
    {{le, r3, 0x2ac00000}, false, al, "le r3 0x2ac00000", "le_r3_0x2ac00000"},
    {{le, r4, 0x000000ff}, false, al, "le r4 0x000000ff", "le_r4_0x000000ff"},
    {{ge, r8, 0x000000ab}, false, al, "ge r8 0x000000ab", "ge_r8_0x000000ab"},
    {{cc, r14, 0x2ac00000}, false, al, "cc r14 0x2ac00000", "cc_r14_"
                                                            "0x2ac00000"},
    {{vs, r15, 0x2ac00000}, false, al, "vs r15 0x2ac00000", "vs_r15_"
                                                            "0x2ac00000"},
    {{lt, r7, 0x000000ab}, false, al, "lt r7 0x000000ab", "lt_r7_0x000000ab"},
    {{vc, r5, 0x003fc000}, false, al, "vc r5 0x003fc000", "vc_r5_0x003fc000"},
    {{le, r12, 0x0003fc00}, false, al, "le r12 0x0003fc00", "le_r12_"
                                                            "0x0003fc00"},
    {{eq, r3, 0x00003fc0}, false, al, "eq r3 0x00003fc0", "eq_r3_0x00003fc0"},
    {{cs, r15, 0x00000000}, false, al, "cs r15 0x00000000", "cs_r15_"
                                                            "0x00000000"},
    {{pl, r11, 0x0000ff00}, false, al, "pl r11 0x0000ff00", "pl_r11_"
                                                            "0x0000ff00"},
    {{mi, r1, 0xb000000a}, false, al, "mi r1 0xb000000a", "mi_r1_0xb000000a"},
    {{pl, r4, 0x0ab00000}, false, al, "pl r4 0x0ab00000", "pl_r4_0x0ab00000"},
    {{vc, r9, 0x00000000}, false, al, "vc r9 0x00000000", "vc_r9_0x00000000"},
    {{ne, r0, 0x0ff00000}, false, al, "ne r0 0x0ff00000", "ne_r0_0x0ff00000"},
    {{le, r11, 0x00000000}, false, al, "le r11 0x00000000", "le_r11_"
                                                            "0x00000000"},
    {{gt, r15, 0x0ab00000}, false, al, "gt r15 0x0ab00000", "gt_r15_"
                                                            "0x0ab00000"},
    {{vs, r12, 0xab000000}, false, al, "vs r12 0xab000000", "vs_r12_"
                                                            "0xab000000"},
    {{vc, r4, 0x0ff00000}, false, al, "vc r4 0x0ff00000", "vc_r4_0x0ff00000"},
    {{vc, r10, 0xac000002}, false, al, "vc r10 0xac000002", "vc_r10_"
                                                            "0xac000002"},
    {{lt, r14, 0xc000002a}, false, al, "lt r14 0xc000002a", "lt_r14_"
                                                            "0xc000002a"},
    {{ne, r2, 0xf000000f}, false, al, "ne r2 0xf000000f", "ne_r2_0xf000000f"},
    {{le, r1, 0x00002ac0}, false, al, "le r1 0x00002ac0", "le_r1_0x00002ac0"},
    {{eq, r10, 0xc000003f}, false, al, "eq r10 0xc000003f", "eq_r10_"
                                                            "0xc000003f"},
    {{ne, r11, 0x0000ab00}, false, al, "ne r11 0x0000ab00", "ne_r11_"
                                                            "0x0000ab00"},
    {{le, r7, 0x0ab00000}, false, al, "le r7 0x0ab00000", "le_r7_0x0ab00000"},
    {{ne, r10, 0x3fc00000}, false, al, "ne r10 0x3fc00000", "ne_r10_"
                                                            "0x3fc00000"},
    {{al, r9, 0x00002ac0}, false, al, "al r9 0x00002ac0", "al_r9_0x00002ac0"},
    {{pl, r3, 0x0000ff00}, false, al, "pl r3 0x0000ff00", "pl_r3_0x0000ff00"},
    {{gt, r10, 0x000000ff}, false, al, "gt r10 0x000000ff", "gt_r10_"
                                                            "0x000000ff"},
    {{cs, r3, 0x2ac00000}, false, al, "cs r3 0x2ac00000", "cs_r3_0x2ac00000"},
    {{le, r7, 0x000000ff}, false, al, "le r7 0x000000ff", "le_r7_0x000000ff"},
    {{ls, r6, 0x000002ac}, false, al, "ls r6 0x000002ac", "ls_r6_0x000002ac"},
    {{vc, r12, 0x03fc0000}, false, al, "vc r12 0x03fc0000", "vc_r12_"
                                                            "0x03fc0000"},
    {{cs, r4, 0x0003fc00}, false, al, "cs r4 0x0003fc00", "cs_r4_0x0003fc00"},
    {{vs, r12, 0xc000003f}, false, al, "vs r12 0xc000003f", "vs_r12_"
                                                            "0xc000003f"},
    {{hi, r13, 0x000000ab}, false, al, "hi r13 0x000000ab", "hi_r13_"
                                                            "0x000000ab"},
    {{le, r12, 0x00000000}, false, al, "le r12 0x00000000", "le_r12_"
                                                            "0x00000000"},
    {{al, r2, 0x000000ab}, false, al, "al r2 0x000000ab", "al_r2_0x000000ab"},
    {{lt, r8, 0x03fc0000}, false, al, "lt r8 0x03fc0000", "lt_r8_0x03fc0000"},
    {{lt, r15, 0x0002ac00}, false, al, "lt r15 0x0002ac00", "lt_r15_"
                                                            "0x0002ac00"},
    {{gt, r2, 0x00ff0000}, false, al, "gt r2 0x00ff0000", "gt_r2_0x00ff0000"},
    {{al, r14, 0x00003fc0}, false, al, "al r14 0x00003fc0", "al_r14_"
                                                            "0x00003fc0"},
    {{lt, r14, 0x00003fc0}, false, al, "lt r14 0x00003fc0", "lt_r14_"
                                                            "0x00003fc0"},
    {{vc, r15, 0xb000000a}, false, al, "vc r15 0xb000000a", "vc_r15_"
                                                            "0xb000000a"},
    {{ls, r15, 0xc000002a}, false, al, "ls r15 0xc000002a", "ls_r15_"
                                                            "0xc000002a"},
    {{pl, r9, 0x2ac00000}, false, al, "pl r9 0x2ac00000", "pl_r9_0x2ac00000"},
    {{vc, r13, 0x0000ab00}, false, al, "vc r13 0x0000ab00", "vc_r13_"
                                                            "0x0000ab00"},
    {{ge, r7, 0xfc000003}, false, al, "ge r7 0xfc000003", "ge_r7_0xfc000003"},
    {{mi, r10, 0x000ab000}, false, al, "mi r10 0x000ab000", "mi_r10_"
                                                            "0x000ab000"},
    {{ge, r8, 0x000000ff}, false, al, "ge r8 0x000000ff", "ge_r8_0x000000ff"},
    {{vc, r7, 0x00ff0000}, false, al, "vc r7 0x00ff0000", "vc_r7_0x00ff0000"},
    {{al, r0, 0x03fc0000}, false, al, "al r0 0x03fc0000", "al_r0_0x03fc0000"},
    {{ne, r5, 0xab000000}, false, al, "ne r5 0xab000000", "ne_r5_0xab000000"},
    {{lt, r9, 0x00002ac0}, false, al, "lt r9 0x00002ac0", "lt_r9_0x00002ac0"},
    {{ge, r4, 0x0002ac00}, false, al, "ge r4 0x0002ac00", "ge_r4_0x0002ac00"},
    {{lt, r1, 0x00000000}, false, al, "lt r1 0x00000000", "lt_r1_0x00000000"},
    {{cc, r9, 0xc000003f}, false, al, "cc r9 0xc000003f", "cc_r9_0xc000003f"},
    {{mi, r3, 0xc000002a}, false, al, "mi r3 0xc000002a", "mi_r3_0xc000002a"},
    {{vc, r15, 0xff000000}, false, al, "vc r15 0xff000000", "vc_r15_"
                                                            "0xff000000"},
    {{cs, r0, 0xfc000003}, false, al, "cs r0 0xfc000003", "cs_r0_0xfc000003"},
    {{eq, r9, 0x00ff0000}, false, al, "eq r9 0x00ff0000", "eq_r9_0x00ff0000"},
    {{lt, r13, 0xac000002}, false, al, "lt r13 0xac000002", "lt_r13_"
                                                            "0xac000002"},
    {{pl, r11, 0xac000002}, false, al, "pl r11 0xac000002", "pl_r11_"
                                                            "0xac000002"},
    {{cc, r9, 0x00ab0000}, false, al, "cc r9 0x00ab0000", "cc_r9_0x00ab0000"},
    {{lt, r6, 0x0002ac00}, false, al, "lt r6 0x0002ac00", "lt_r6_0x0002ac00"},
    {{al, r0, 0xab000000}, false, al, "al r0 0xab000000", "al_r0_0xab000000"},
    {{le, r15, 0xff000000}, false, al, "le r15 0xff000000", "le_r15_"
                                                            "0xff000000"},
    {{ge, r6, 0xb000000a}, false, al, "ge r6 0xb000000a", "ge_r6_0xb000000a"},
    {{pl, r3, 0xb000000a}, false, al, "pl r3 0xb000000a", "pl_r3_0xb000000a"},
    {{le, r10, 0x000002ac}, false, al, "le r10 0x000002ac", "le_r10_"
                                                            "0x000002ac"},
    {{al, r15, 0x2ac00000}, false, al, "al r15 0x2ac00000", "al_r15_"
                                                            "0x2ac00000"},
    {{vc, r10, 0x00000000}, false, al, "vc r10 0x00000000", "vc_r10_"
                                                            "0x00000000"},
    {{ge, r10, 0x000002ac}, false, al, "ge r10 0x000002ac", "ge_r10_"
                                                            "0x000002ac"},
    {{lt, r14, 0x002ac000}, false, al, "lt r14 0x002ac000", "lt_r14_"
                                                            "0x002ac000"},
    {{cc, r6, 0x0003fc00}, false, al, "cc r6 0x0003fc00", "cc_r6_0x0003fc00"},
    {{gt, r12, 0xff000000}, false, al, "gt r12 0xff000000", "gt_r12_"
                                                            "0xff000000"},
    {{ls, r10, 0x00000ff0}, false, al, "ls r10 0x00000ff0", "ls_r10_"
                                                            "0x00000ff0"},
    {{vs, r8, 0xb000000a}, false, al, "vs r8 0xb000000a", "vs_r8_0xb000000a"},
    {{ge, r2, 0x000003fc}, false, al, "ge r2 0x000003fc", "ge_r2_0x000003fc"},
    {{ls, r1, 0xab000000}, false, al, "ls r1 0xab000000", "ls_r1_0xab000000"},
    {{ge, r5, 0x000003fc}, false, al, "ge r5 0x000003fc", "ge_r5_0x000003fc"},
    {{le, r2, 0x2ac00000}, false, al, "le r2 0x2ac00000", "le_r2_0x2ac00000"},
    {{ls, r3, 0x00002ac0}, false, al, "ls r3 0x00002ac0", "ls_r3_0x00002ac0"},
    {{le, r5, 0x003fc000}, false, al, "le r5 0x003fc000", "le_r5_0x003fc000"},
    {{cc, r0, 0x00002ac0}, false, al, "cc r0 0x00002ac0", "cc_r0_0x00002ac0"},
    {{lt, r5, 0x00000000}, false, al, "lt r5 0x00000000", "lt_r5_0x00000000"},
    {{gt, r12, 0x00000ab0}, false, al, "gt r12 0x00000ab0", "gt_r12_"
                                                            "0x00000ab0"},
    {{le, r15, 0x00ab0000}, false, al, "le r15 0x00ab0000", "le_r15_"
                                                            "0x00ab0000"},
    {{eq, r1, 0x02ac0000}, false, al, "eq r1 0x02ac0000", "eq_r1_0x02ac0000"},
    {{eq, r15, 0xc000002a}, false, al, "eq r15 0xc000002a", "eq_r15_"
                                                            "0xc000002a"},
    {{eq, r8, 0xff000000}, false, al, "eq r8 0xff000000", "eq_r8_0xff000000"},
    {{mi, r15, 0x0000ff00}, false, al, "mi r15 0x0000ff00", "mi_r15_"
                                                            "0x0000ff00"},
    {{le, r5, 0xab000000}, false, al, "le r5 0xab000000", "le_r5_0xab000000"},
    {{cc, r1, 0x000002ac}, false, al, "cc r1 0x000002ac", "cc_r1_0x000002ac"},
    {{pl, r5, 0x00003fc0}, false, al, "pl r5 0x00003fc0", "pl_r5_0x00003fc0"},
    {{pl, r8, 0x00000ab0}, false, al, "pl r8 0x00000ab0", "pl_r8_0x00000ab0"},
    {{gt, r3, 0x2ac00000}, false, al, "gt r3 0x2ac00000", "gt_r3_0x2ac00000"},
    {{ne, r3, 0x0ff00000}, false, al, "ne r3 0x0ff00000", "ne_r3_0x0ff00000"},
    {{ls, r11, 0x00002ac0}, false, al, "ls r11 0x00002ac0", "ls_r11_"
                                                            "0x00002ac0"},
    {{cs, r6, 0x000ff000}, false, al, "cs r6 0x000ff000", "cs_r6_0x000ff000"},
    {{cc, r11, 0x002ac000}, false, al, "cc r11 0x002ac000", "cc_r11_"
                                                            "0x002ac000"},
    {{gt, r6, 0xf000000f}, false, al, "gt r6 0xf000000f", "gt_r6_0xf000000f"},
    {{pl, r12, 0xab000000}, false, al, "pl r12 0xab000000", "pl_r12_"
                                                            "0xab000000"},
    {{vs, r4, 0x000002ac}, false, al, "vs r4 0x000002ac", "vs_r4_0x000002ac"},
    {{lt, r5, 0x0003fc00}, false, al, "lt r5 0x0003fc00", "lt_r5_0x0003fc00"},
    {{vs, r12, 0xc000002a}, false, al, "vs r12 0xc000002a", "vs_r12_"
                                                            "0xc000002a"},
    {{cc, r7, 0x3fc00000}, false, al, "cc r7 0x3fc00000", "cc_r7_0x3fc00000"},
    {{le, r4, 0x0ff00000}, false, al, "le r4 0x0ff00000", "le_r4_0x0ff00000"},
    {{ne, r14, 0x000000ff}, false, al, "ne r14 0x000000ff", "ne_r14_"
                                                            "0x000000ff"},
    {{cs, r14, 0xac000002}, false, al, "cs r14 0xac000002", "cs_r14_"
                                                            "0xac000002"},
    {{ge, r15, 0x000ff000}, false, al, "ge r15 0x000ff000", "ge_r15_"
                                                            "0x000ff000"},
    {{le, r12, 0x0000ff00}, false, al, "le r12 0x0000ff00", "le_r12_"
                                                            "0x0000ff00"},
    {{al, r3, 0x00ff0000}, false, al, "al r3 0x00ff0000", "al_r3_0x00ff0000"},
    {{ls, r0, 0x000ff000}, false, al, "ls r0 0x000ff000", "ls_r0_0x000ff000"},
    {{ne, r3, 0x000000ab}, false, al, "ne r3 0x000000ab", "ne_r3_0x000000ab"},
    {{eq, r13, 0x003fc000}, false, al, "eq r13 0x003fc000", "eq_r13_"
                                                            "0x003fc000"},
    {{eq, r14, 0x0ab00000}, false, al, "eq r14 0x0ab00000", "eq_r14_"
                                                            "0x0ab00000"},
    {{ge, r12, 0x0002ac00}, false, al, "ge r12 0x0002ac00", "ge_r12_"
                                                            "0x0002ac00"},
    {{mi, r10, 0x00003fc0}, false, al, "mi r10 0x00003fc0", "mi_r10_"
                                                            "0x00003fc0"},
    {{cs, r8, 0x0002ac00}, false, al, "cs r8 0x0002ac00", "cs_r8_0x0002ac00"},
    {{lt, r3, 0x000ff000}, false, al, "lt r3 0x000ff000", "lt_r3_0x000ff000"},
    {{cc, r3, 0xf000000f}, false, al, "cc r3 0xf000000f", "cc_r3_0xf000000f"},
    {{ne, r10, 0x00000ab0}, false, al, "ne r10 0x00000ab0", "ne_r10_"
                                                            "0x00000ab0"},
    {{cc, r14, 0xb000000a}, false, al, "cc r14 0xb000000a", "cc_r14_"
                                                            "0xb000000a"},
    {{cs, r0, 0x3fc00000}, false, al, "cs r0 0x3fc00000", "cs_r0_0x3fc00000"},
    {{vs, r6, 0x00003fc0}, false, al, "vs r6 0x00003fc0", "vs_r6_0x00003fc0"},
    {{ge, r15, 0x00000000}, false, al, "ge r15 0x00000000", "ge_r15_"
                                                            "0x00000000"},
    {{pl, r14, 0x00003fc0}, false, al, "pl r14 0x00003fc0", "pl_r14_"
                                                            "0x00003fc0"},
    {{al, r3, 0x3fc00000}, false, al, "al r3 0x3fc00000", "al_r3_0x3fc00000"},
    {{gt, r9, 0x0003fc00}, false, al, "gt r9 0x0003fc00", "gt_r9_0x0003fc00"},
    {{ne, r12, 0x00000000}, false, al, "ne r12 0x00000000", "ne_r12_"
                                                            "0x00000000"},
    {{ge, r9, 0x0000ab00}, false, al, "ge r9 0x0000ab00", "ge_r9_0x0000ab00"},
    {{eq, r6, 0xac000002}, false, al, "eq r6 0xac000002", "eq_r6_0xac000002"},
    {{ge, r2, 0x00ff0000}, false, al, "ge r2 0x00ff0000", "ge_r2_0x00ff0000"},
    {{eq, r3, 0x00000ff0}, false, al, "eq r3 0x00000ff0", "eq_r3_0x00000ff0"},
    {{al, r15, 0x00000ab0}, false, al, "al r15 0x00000ab0", "al_r15_"
                                                            "0x00000ab0"},
    {{eq, r8, 0x0000ff00}, false, al, "eq r8 0x0000ff00", "eq_r8_0x0000ff00"},
    {{hi, r2, 0x000003fc}, false, al, "hi r2 0x000003fc", "hi_r2_0x000003fc"},
    {{ls, r3, 0x0000ff00}, false, al, "ls r3 0x0000ff00", "ls_r3_0x0000ff00"},
    {{le, r12, 0x00000ab0}, false, al, "le r12 0x00000ab0", "le_r12_"
                                                            "0x00000ab0"},
    {{le, r15, 0xb000000a}, false, al, "le r15 0xb000000a", "le_r15_"
                                                            "0xb000000a"},
    {{vc, r7, 0x003fc000}, false, al, "vc r7 0x003fc000", "vc_r7_0x003fc000"},
    {{vc, r2, 0xac000002}, false, al, "vc r2 0xac000002", "vc_r2_0xac000002"},
    {{hi, r10, 0x003fc000}, false, al, "hi r10 0x003fc000", "hi_r10_"
                                                            "0x003fc000"},
    {{ge, r5, 0x000ff000}, false, al, "ge r5 0x000ff000", "ge_r5_0x000ff000"},
    {{hi, r9, 0x0000ab00}, false, al, "hi r9 0x0000ab00", "hi_r9_0x0000ab00"},
    {{gt, r10, 0x2ac00000}, false, al, "gt r10 0x2ac00000", "gt_r10_"
                                                            "0x2ac00000"},
    {{lt, r12, 0x00002ac0}, false, al, "lt r12 0x00002ac0", "lt_r12_"
                                                            "0x00002ac0"},
    {{ls, r14, 0xab000000}, false, al, "ls r14 0xab000000", "ls_r14_"
                                                            "0xab000000"},
    {{cs, r0, 0x00003fc0}, false, al, "cs r0 0x00003fc0", "cs_r0_0x00003fc0"},
    {{vc, r5, 0x00000ff0}, false, al, "vc r5 0x00000ff0", "vc_r5_0x00000ff0"},
    {{cs, r2, 0x0ab00000}, false, al, "cs r2 0x0ab00000", "cs_r2_0x0ab00000"},
    {{al, r5, 0x0000ff00}, false, al, "al r5 0x0000ff00", "al_r5_0x0000ff00"},
    {{hi, r6, 0x003fc000}, false, al, "hi r6 0x003fc000", "hi_r6_0x003fc000"},
    {{eq, r0, 0xfc000003}, false, al, "eq r0 0xfc000003", "eq_r0_0xfc000003"},
    {{mi, r14, 0x00003fc0}, false, al, "mi r14 0x00003fc0", "mi_r14_"
                                                            "0x00003fc0"},
    {{pl, r7, 0x000002ac}, false, al, "pl r7 0x000002ac", "pl_r7_0x000002ac"},
    {{ls, r5, 0xfc000003}, false, al, "ls r5 0xfc000003", "ls_r5_0xfc000003"},
    {{cs, r15, 0x000003fc}, false, al, "cs r15 0x000003fc", "cs_r15_"
                                                            "0x000003fc"},
    {{ls, r3, 0xab000000}, false, al, "ls r3 0xab000000", "ls_r3_0xab000000"},
    {{lt, r5, 0xc000003f}, false, al, "lt r5 0xc000003f", "lt_r5_0xc000003f"},
    {{mi, r8, 0xab000000}, false, al, "mi r8 0xab000000", "mi_r8_0xab000000"},
    {{ge, r15, 0x000000ab}, false, al, "ge r15 0x000000ab", "ge_r15_"
                                                            "0x000000ab"},
    {{hi, r0, 0x000ff000}, false, al, "hi r0 0x000ff000", "hi_r0_0x000ff000"},
    {{lt, r14, 0x00000000}, false, al, "lt r14 0x00000000", "lt_r14_"
                                                            "0x00000000"},
    {{ne, r11, 0xc000003f}, false, al, "ne r11 0xc000003f", "ne_r11_"
                                                            "0xc000003f"},
    {{al, r3, 0x000002ac}, false, al, "al r3 0x000002ac", "al_r3_0x000002ac"},
    {{vs, r4, 0x0000ab00}, false, al, "vs r4 0x0000ab00", "vs_r4_0x0000ab00"},
    {{eq, r12, 0x000ab000}, false, al, "eq r12 0x000ab000", "eq_r12_"
                                                            "0x000ab000"},
    {{gt, r15, 0x00000ff0}, false, al, "gt r15 0x00000ff0", "gt_r15_"
                                                            "0x00000ff0"},
    {{mi, r1, 0x00002ac0}, false, al, "mi r1 0x00002ac0", "mi_r1_0x00002ac0"},
    {{eq, r15, 0x0000ff00}, false, al, "eq r15 0x0000ff00", "eq_r15_"
                                                            "0x0000ff00"},
    {{pl, r5, 0xfc000003}, false, al, "pl r5 0xfc000003", "pl_r5_0xfc000003"},
    {{ge, r8, 0xc000002a}, false, al, "ge r8 0xc000002a", "ge_r8_0xc000002a"},
    {{ne, r0, 0x00002ac0}, false, al, "ne r0 0x00002ac0", "ne_r0_0x00002ac0"},
    {{le, r5, 0x000000ff}, false, al, "le r5 0x000000ff", "le_r5_0x000000ff"},
    {{cs, r9, 0x000003fc}, false, al, "cs r9 0x000003fc", "cs_r9_0x000003fc"},
    {{cc, r3, 0x02ac0000}, false, al, "cc r3 0x02ac0000", "cc_r3_0x02ac0000"},
    {{ne, r1, 0x0ff00000}, false, al, "ne r1 0x0ff00000", "ne_r1_0x0ff00000"},
    {{le, r2, 0x000ff000}, false, al, "le r2 0x000ff000", "le_r2_0x000ff000"},
    {{vc, r11, 0x00ab0000}, false, al, "vc r11 0x00ab0000", "vc_r11_"
                                                            "0x00ab0000"},
    {{eq, r11, 0x0003fc00}, false, al, "eq r11 0x0003fc00", "eq_r11_"
                                                            "0x0003fc00"},
    {{vc, r12, 0x2ac00000}, false, al, "vc r12 0x2ac00000", "vc_r12_"
                                                            "0x2ac00000"},
    {{al, r10, 0xac000002}, false, al, "al r10 0xac000002", "al_r10_"
                                                            "0xac000002"},
    {{gt, r6, 0x0000ab00}, false, al, "gt r6 0x0000ab00", "gt_r6_0x0000ab00"},
    {{al, r4, 0x02ac0000}, false, al, "al r4 0x02ac0000", "al_r4_0x02ac0000"},
    {{cs, r14, 0x00000ab0}, false, al, "cs r14 0x00000ab0", "cs_r14_"
                                                            "0x00000ab0"},
    {{hi, r13, 0x03fc0000}, false, al, "hi r13 0x03fc0000", "hi_r13_"
                                                            "0x03fc0000"},
    {{ne, r1, 0x000002ac}, false, al, "ne r1 0x000002ac", "ne_r1_0x000002ac"},
    {{vs, r1, 0xf000000f}, false, al, "vs r1 0xf000000f", "vs_r1_0xf000000f"},
    {{ge, r7, 0x2ac00000}, false, al, "ge r7 0x2ac00000", "ge_r7_0x2ac00000"},
    {{mi, r1, 0x000002ac}, false, al, "mi r1 0x000002ac", "mi_r1_0x000002ac"},
    {{gt, r6, 0x00002ac0}, false, al, "gt r6 0x00002ac0", "gt_r6_0x00002ac0"},
    {{al, r15, 0x000000ff}, false, al, "al r15 0x000000ff", "al_r15_"
                                                            "0x000000ff"},
    {{vs, r0, 0x000000ff}, false, al, "vs r0 0x000000ff", "vs_r0_0x000000ff"},
    {{vc, r2, 0x00ff0000}, false, al, "vc r2 0x00ff0000", "vc_r2_0x00ff0000"},
    {{ls, r15, 0x00000ab0}, false, al, "ls r15 0x00000ab0", "ls_r15_"
                                                            "0x00000ab0"},
    {{pl, r8, 0xab000000}, false, al, "pl r8 0xab000000", "pl_r8_0xab000000"},
    {{ge, r13, 0x2ac00000}, false, al, "ge r13 0x2ac00000", "ge_r13_"
                                                            "0x2ac00000"},
    {{vc, r6, 0x0000ab00}, false, al, "vc r6 0x0000ab00", "vc_r6_0x0000ab00"},
    {{mi, r7, 0x000000ff}, false, al, "mi r7 0x000000ff", "mi_r7_0x000000ff"},
    {{al, r13, 0x00000ab0}, false, al, "al r13 0x00000ab0", "al_r13_"
                                                            "0x00000ab0"},
    {{gt, r14, 0x02ac0000}, false, al, "gt r14 0x02ac0000", "gt_r14_"
                                                            "0x02ac0000"},
    {{le, r8, 0x00000ff0}, false, al, "le r8 0x00000ff0", "le_r8_0x00000ff0"},
    {{lt, r12, 0x0000ff00}, false, al, "lt r12 0x0000ff00", "lt_r12_"
                                                            "0x0000ff00"},
    {{gt, r11, 0x0003fc00}, false, al, "gt r11 0x0003fc00", "gt_r11_"
                                                            "0x0003fc00"},
    {{vs, r0, 0xff000000}, false, al, "vs r0 0xff000000", "vs_r0_0xff000000"},
    {{ne, r3, 0x2ac00000}, false, al, "ne r3 0x2ac00000", "ne_r3_0x2ac00000"},
    {{cs, r9, 0x00002ac0}, false, al, "cs r9 0x00002ac0", "cs_r9_0x00002ac0"},
    {{cs, r10, 0x003fc000}, false, al, "cs r10 0x003fc000", "cs_r10_"
                                                            "0x003fc000"},
    {{ls, r12, 0x002ac000}, false, al, "ls r12 0x002ac000", "ls_r12_"
                                                            "0x002ac000"},
    {{gt, r6, 0x3fc00000}, false, al, "gt r6 0x3fc00000", "gt_r6_0x3fc00000"},
    {{le, r2, 0x000000ff}, false, al, "le r2 0x000000ff", "le_r2_0x000000ff"},
    {{le, r1, 0x000000ab}, false, al, "le r1 0x000000ab", "le_r1_0x000000ab"},
    {{cc, r14, 0x000000ff}, false, al, "cc r14 0x000000ff", "cc_r14_"
                                                            "0x000000ff"},
    {{mi, r6, 0x002ac000}, false, al, "mi r6 0x002ac000", "mi_r6_0x002ac000"},
    {{hi, r7, 0xf000000f}, false, al, "hi r7 0xf000000f", "hi_r7_0xf000000f"},
    {{ge, r1, 0xf000000f}, false, al, "ge r1 0xf000000f", "ge_r1_0xf000000f"},
    {{cs, r15, 0xc000003f}, false, al, "cs r15 0xc000003f", "cs_r15_"
                                                            "0xc000003f"},
    {{le, r14, 0x000000ff}, false, al, "le r14 0x000000ff", "le_r14_"
                                                            "0x000000ff"},
    {{ls, r10, 0x0000ab00}, false, al, "ls r10 0x0000ab00", "ls_r10_"
                                                            "0x0000ab00"},
    {{pl, r15, 0x000000ab}, false, al, "pl r15 0x000000ab", "pl_r15_"
                                                            "0x000000ab"},
    {{mi, r7, 0x0000ff00}, false, al, "mi r7 0x0000ff00", "mi_r7_0x0000ff00"},
    {{cs, r0, 0x003fc000}, false, al, "cs r0 0x003fc000", "cs_r0_0x003fc000"},
    {{eq, r5, 0xb000000a}, false, al, "eq r5 0xb000000a", "eq_r5_0xb000000a"},
    {{ls, r7, 0x00ff0000}, false, al, "ls r7 0x00ff0000", "ls_r7_0x00ff0000"},
    {{pl, r8, 0xff000000}, false, al, "pl r8 0xff000000", "pl_r8_0xff000000"},
    {{hi, r15, 0x0ff00000}, false, al, "hi r15 0x0ff00000", "hi_r15_"
                                                            "0x0ff00000"},
    {{mi, r7, 0x00ff0000}, false, al, "mi r7 0x00ff0000", "mi_r7_0x00ff0000"},
    {{vs, r9, 0x0ff00000}, false, al, "vs r9 0x0ff00000", "vs_r9_0x0ff00000"},
    {{pl, r7, 0xc000002a}, false, al, "pl r7 0xc000002a", "pl_r7_0xc000002a"},
    {{mi, r6, 0x0002ac00}, false, al, "mi r6 0x0002ac00", "mi_r6_0x0002ac00"},
    {{al, r0, 0x00003fc0}, false, al, "al r0 0x00003fc0", "al_r0_0x00003fc0"},
    {{hi, r15, 0xc000003f}, false, al, "hi r15 0xc000003f", "hi_r15_"
                                                            "0xc000003f"},
    {{gt, r15, 0xfc000003}, false, al, "gt r15 0xfc000003", "gt_r15_"
                                                            "0xfc000003"},
    {{mi, r7, 0x00000ff0}, false, al, "mi r7 0x00000ff0", "mi_r7_0x00000ff0"},
    {{gt, r4, 0x00ff0000}, false, al, "gt r4 0x00ff0000", "gt_r4_0x00ff0000"},
    {{vc, r6, 0xfc000003}, false, al, "vc r6 0xfc000003", "vc_r6_0xfc000003"},
    {{pl, r11, 0x002ac000}, false, al, "pl r11 0x002ac000", "pl_r11_"
                                                            "0x002ac000"},
    {{gt, r1, 0xc000002a}, false, al, "gt r1 0xc000002a", "gt_r1_0xc000002a"},
    {{vc, r1, 0x0003fc00}, false, al, "vc r1 0x0003fc00", "vc_r1_0x0003fc00"},
    {{gt, r11, 0x003fc000}, false, al, "gt r11 0x003fc000", "gt_r11_"
                                                            "0x003fc000"},
    {{hi, r11, 0x00000ff0}, false, al, "hi r11 0x00000ff0", "hi_r11_"
                                                            "0x00000ff0"},
    {{le, r9, 0x00000ab0}, false, al, "le r9 0x00000ab0", "le_r9_0x00000ab0"},
    {{ne, r1, 0x000000ff}, false, al, "ne r1 0x000000ff", "ne_r1_0x000000ff"},
    {{cc, r8, 0x00000ff0}, false, al, "cc r8 0x00000ff0", "cc_r8_0x00000ff0"},
    {{vc, r12, 0xf000000f}, false, al, "vc r12 0xf000000f", "vc_r12_"
                                                            "0xf000000f"},
    {{gt, r10, 0x000003fc}, false, al, "gt r10 0x000003fc", "gt_r10_"
                                                            "0x000003fc"},
    {{cc, r5, 0x0ff00000}, false, al, "cc r5 0x0ff00000", "cc_r5_0x0ff00000"},
    {{cs, r12, 0x03fc0000}, false, al, "cs r12 0x03fc0000", "cs_r12_"
                                                            "0x03fc0000"},
    {{pl, r5, 0x000ab000}, false, al, "pl r5 0x000ab000", "pl_r5_0x000ab000"},
    {{vs, r5, 0x0000ff00}, false, al, "vs r5 0x0000ff00", "vs_r5_0x0000ff00"},
    {{vs, r3, 0x02ac0000}, false, al, "vs r3 0x02ac0000", "vs_r3_0x02ac0000"},
    {{cc, r6, 0x003fc000}, false, al, "cc r6 0x003fc000", "cc_r6_0x003fc000"},
    {{hi, r11, 0x000002ac}, false, al, "hi r11 0x000002ac", "hi_r11_"
                                                            "0x000002ac"},
    {{cs, r0, 0xb000000a}, false, al, "cs r0 0xb000000a", "cs_r0_0xb000000a"},
    {{ge, r12, 0xfc000003}, false, al, "ge r12 0xfc000003", "ge_r12_"
                                                            "0xfc000003"},
    {{hi, r1, 0x002ac000}, false, al, "hi r1 0x002ac000", "hi_r1_0x002ac000"},
    {{ls, r8, 0x000ab000}, false, al, "ls r8 0x000ab000", "ls_r8_0x000ab000"},
    {{cc, r4, 0x00000000}, false, al, "cc r4 0x00000000", "cc_r4_0x00000000"},
    {{le, r9, 0xf000000f}, false, al, "le r9 0xf000000f", "le_r9_0xf000000f"},
    {{vs, r14, 0x03fc0000}, false, al, "vs r14 0x03fc0000", "vs_r14_"
                                                            "0x03fc0000"},
    {{lt, r1, 0x0000ab00}, false, al, "lt r1 0x0000ab00", "lt_r1_0x0000ab00"},
    {{ge, r6, 0x0002ac00}, false, al, "ge r6 0x0002ac00", "ge_r6_0x0002ac00"},
    {{ne, r10, 0x000ff000}, false, al, "ne r10 0x000ff000", "ne_r10_"
                                                            "0x000ff000"},
    {{vs, r4, 0x000ab000}, false, al, "vs r4 0x000ab000", "vs_r4_0x000ab000"},
    {{hi, r0, 0xfc000003}, false, al, "hi r0 0xfc000003", "hi_r0_0xfc000003"},
    {{lt, r6, 0x00000ff0}, false, al, "lt r6 0x00000ff0", "lt_r6_0x00000ff0"},
    {{cc, r14, 0xf000000f}, false, al, "cc r14 0xf000000f", "cc_r14_"
                                                            "0xf000000f"},
    {{ne, r5, 0x00000ab0}, false, al, "ne r5 0x00000ab0", "ne_r5_0x00000ab0"},
    {{gt, r7, 0x0003fc00}, false, al, "gt r7 0x0003fc00", "gt_r7_0x0003fc00"},
    {{lt, r12, 0xab000000}, false, al, "lt r12 0xab000000", "lt_r12_"
                                                            "0xab000000"},
    {{cs, r3, 0x000ab000}, false, al, "cs r3 0x000ab000", "cs_r3_0x000ab000"},
    {{gt, r9, 0x00000ab0}, false, al, "gt r9 0x00000ab0", "gt_r9_0x00000ab0"},
    {{hi, r13, 0x0ab00000}, false, al, "hi r13 0x0ab00000", "hi_r13_"
                                                            "0x0ab00000"},
    {{gt, r8, 0x0003fc00}, false, al, "gt r8 0x0003fc00", "gt_r8_0x0003fc00"},
    {{gt, r6, 0x000003fc}, false, al, "gt r6 0x000003fc", "gt_r6_0x000003fc"},
    {{hi, r14, 0xfc000003}, false, al, "hi r14 0xfc000003", "hi_r14_"
                                                            "0xfc000003"},
    {{lt, r3, 0xff000000}, false, al, "lt r3 0xff000000", "lt_r3_0xff000000"},
    {{ls, r9, 0xb000000a}, false, al, "ls r9 0xb000000a", "ls_r9_0xb000000a"},
    {{pl, r6, 0xab000000}, false, al, "pl r6 0xab000000", "pl_r6_0xab000000"},
    {{al, r0, 0x3fc00000}, false, al, "al r0 0x3fc00000", "al_r0_0x3fc00000"},
    {{al, r14, 0x000ab000}, false, al, "al r14 0x000ab000", "al_r14_"
                                                            "0x000ab000"},
    {{pl, r14, 0xf000000f}, false, al, "pl r14 0xf000000f", "pl_r14_"
                                                            "0xf000000f"},
    {{lt, r13, 0x000002ac}, false, al, "lt r13 0x000002ac", "lt_r13_"
                                                            "0x000002ac"},
    {{al, r7, 0x000ab000}, false, al, "al r7 0x000ab000", "al_r7_0x000ab000"},
    {{eq, r6, 0xff000000}, false, al, "eq r6 0xff000000", "eq_r6_0xff000000"},
    {{lt, r3, 0x003fc000}, false, al, "lt r3 0x003fc000", "lt_r3_0x003fc000"},
    {{le, r15, 0x0ab00000}, false, al, "le r15 0x0ab00000", "le_r15_"
                                                            "0x0ab00000"},
    {{le, r1, 0x00000ab0}, false, al, "le r1 0x00000ab0", "le_r1_0x00000ab0"},
    {{cc, r8, 0x000002ac}, false, al, "cc r8 0x000002ac", "cc_r8_0x000002ac"},
    {{cs, r11, 0xac000002}, false, al, "cs r11 0xac000002", "cs_r11_"
                                                            "0xac000002"},
    {{ls, r0, 0x0002ac00}, false, al, "ls r0 0x0002ac00", "ls_r0_0x0002ac00"},
    {{mi, r7, 0x003fc000}, false, al, "mi r7 0x003fc000", "mi_r7_0x003fc000"},
    {{al, r6, 0x000ab000}, false, al, "al r6 0x000ab000", "al_r6_0x000ab000"},
    {{eq, r7, 0x03fc0000}, false, al, "eq r7 0x03fc0000", "eq_r7_0x03fc0000"},
    {{ne, r4, 0xff000000}, false, al, "ne r4 0xff000000", "ne_r4_0xff000000"},
    {{eq, r15, 0x002ac000}, false, al, "eq r15 0x002ac000", "eq_r15_"
                                                            "0x002ac000"},
    {{vc, r14, 0x000002ac}, false, al, "vc r14 0x000002ac", "vc_r14_"
                                                            "0x000002ac"},
    {{ls, r12, 0x0000ff00}, false, al, "ls r12 0x0000ff00", "ls_r12_"
                                                            "0x0000ff00"},
    {{gt, r14, 0x0002ac00}, false, al, "gt r14 0x0002ac00", "gt_r14_"
                                                            "0x0002ac00"},
    {{vs, r2, 0x0000ab00}, false, al, "vs r2 0x0000ab00", "vs_r2_0x0000ab00"},
    {{al, r3, 0x0000ab00}, false, al, "al r3 0x0000ab00", "al_r3_0x0000ab00"},
    {{hi, r6, 0x0ab00000}, false, al, "hi r6 0x0ab00000", "hi_r6_0x0ab00000"},
    {{ne, r12, 0xff000000}, false, al, "ne r12 0xff000000", "ne_r12_"
                                                            "0xff000000"},
    {{ne, r0, 0x02ac0000}, false, al, "ne r0 0x02ac0000", "ne_r0_0x02ac0000"},
    {{ls, r12, 0x00003fc0}, false, al, "ls r12 0x00003fc0", "ls_r12_"
                                                            "0x00003fc0"},
    {{cs, r8, 0x00000ab0}, false, al, "cs r8 0x00000ab0", "cs_r8_0x00000ab0"},
    {{mi, r0, 0x000003fc}, false, al, "mi r0 0x000003fc", "mi_r0_0x000003fc"},
    {{vc, r15, 0xc000003f}, false, al, "vc r15 0xc000003f", "vc_r15_"
                                                            "0xc000003f"},
    {{al, r9, 0x000002ac}, false, al, "al r9 0x000002ac", "al_r9_0x000002ac"},
    {{hi, r14, 0x00000000}, false, al, "hi r14 0x00000000", "hi_r14_"
                                                            "0x00000000"},
    {{hi, r3, 0x0003fc00}, false, al, "hi r3 0x0003fc00", "hi_r3_0x0003fc00"},
    {{vs, r7, 0xb000000a}, false, al, "vs r7 0xb000000a", "vs_r7_0xb000000a"},
    {{pl, r14, 0x002ac000}, false, al, "pl r14 0x002ac000", "pl_r14_"
                                                            "0x002ac000"},
    {{al, r15, 0xff000000}, false, al, "al r15 0xff000000", "al_r15_"
                                                            "0xff000000"},
    {{al, r6, 0x03fc0000}, false, al, "al r6 0x03fc0000", "al_r6_0x03fc0000"},
    {{pl, r8, 0x00000000}, false, al, "pl r8 0x00000000", "pl_r8_0x00000000"},
    {{lt, r5, 0x0000ab00}, false, al, "lt r5 0x0000ab00", "lt_r5_0x0000ab00"},
    {{al, r5, 0x0ff00000}, false, al, "al r5 0x0ff00000", "al_r5_0x0ff00000"},
    {{vc, r13, 0x000000ff}, false, al, "vc r13 0x000000ff", "vc_r13_"
                                                            "0x000000ff"},
    {{ls, r6, 0x0ab00000}, false, al, "ls r6 0x0ab00000", "ls_r6_0x0ab00000"},
    {{gt, r7, 0x000000ab}, false, al, "gt r7 0x000000ab", "gt_r7_0x000000ab"},
    {{ge, r15, 0x0000ab00}, false, al, "ge r15 0x0000ab00", "ge_r15_"
                                                            "0x0000ab00"},
    {{vs, r12, 0x00000ff0}, false, al, "vs r12 0x00000ff0", "vs_r12_"
                                                            "0x00000ff0"},
    {{al, r11, 0x00002ac0}, false, al, "al r11 0x00002ac0", "al_r11_"
                                                            "0x00002ac0"},
    {{ne, r14, 0x002ac000}, false, al, "ne r14 0x002ac000", "ne_r14_"
                                                            "0x002ac000"},
    {{ne, r8, 0x000ab000}, false, al, "ne r8 0x000ab000", "ne_r8_0x000ab000"},
    {{cs, r4, 0x03fc0000}, false, al, "cs r4 0x03fc0000", "cs_r4_0x03fc0000"},
    {{cs, r7, 0x00003fc0}, false, al, "cs r7 0x00003fc0", "cs_r7_0x00003fc0"},
    {{cs, r10, 0x000ff000}, false, al, "cs r10 0x000ff000", "cs_r10_"
                                                            "0x000ff000"},
    {{hi, r4, 0x00ff0000}, false, al, "hi r4 0x00ff0000", "hi_r4_0x00ff0000"},
    {{pl, r5, 0x3fc00000}, false, al, "pl r5 0x3fc00000", "pl_r5_0x3fc00000"},
    {{pl, r4, 0x0ff00000}, false, al, "pl r4 0x0ff00000", "pl_r4_0x0ff00000"},
    {{eq, r0, 0x00000ff0}, false, al, "eq r0 0x00000ff0", "eq_r0_0x00000ff0"},
    {{le, r4, 0xc000002a}, false, al, "le r4 0xc000002a", "le_r4_0xc000002a"},
    {{le, r2, 0xfc000003}, false, al, "le r2 0xfc000003", "le_r2_0xfc000003"},
    {{ls, r0, 0x00000ab0}, false, al, "ls r0 0x00000ab0", "ls_r0_0x00000ab0"},
    {{vs, r3, 0x00ff0000}, false, al, "vs r3 0x00ff0000", "vs_r3_0x00ff0000"},
    {{eq, r4, 0x2ac00000}, false, al, "eq r4 0x2ac00000", "eq_r4_0x2ac00000"},
    {{ne, r12, 0x0ff00000}, false, al, "ne r12 0x0ff00000", "ne_r12_"
                                                            "0x0ff00000"},
    {{al, r12, 0x00ff0000}, false, al, "al r12 0x00ff0000", "al_r12_"
                                                            "0x00ff0000"},
    {{lt, r2, 0x00003fc0}, false, al, "lt r2 0x00003fc0", "lt_r2_0x00003fc0"},
    {{cs, r4, 0x00003fc0}, false, al, "cs r4 0x00003fc0", "cs_r4_0x00003fc0"},
    {{gt, r11, 0xc000002a}, false, al, "gt r11 0xc000002a", "gt_r11_"
                                                            "0xc000002a"},
    {{ne, r4, 0x0ff00000}, false, al, "ne r4 0x0ff00000", "ne_r4_0x0ff00000"},
    {{hi, r13, 0x00ab0000}, false, al, "hi r13 0x00ab0000", "hi_r13_"
                                                            "0x00ab0000"},
    {{ge, r3, 0x000ab000}, false, al, "ge r3 0x000ab000", "ge_r3_0x000ab000"},
    {{vs, r12, 0xfc000003}, false, al, "vs r12 0xfc000003", "vs_r12_"
                                                            "0xfc000003"},
    {{hi, r6, 0x03fc0000}, false, al, "hi r6 0x03fc0000", "hi_r6_0x03fc0000"},
    {{cs, r9, 0x00000ab0}, false, al, "cs r9 0x00000ab0", "cs_r9_0x00000ab0"},
    {{ne, r6, 0x003fc000}, false, al, "ne r6 0x003fc000", "ne_r6_0x003fc000"},
    {{vc, r15, 0x00000000}, false, al, "vc r15 0x00000000", "vc_r15_"
                                                            "0x00000000"},
    {{hi, r0, 0x000003fc}, false, al, "hi r0 0x000003fc", "hi_r0_0x000003fc"},
    {{vs, r15, 0x03fc0000}, false, al, "vs r15 0x03fc0000", "vs_r15_"
                                                            "0x03fc0000"},
    {{cs, r0, 0x00ff0000}, false, al, "cs r0 0x00ff0000", "cs_r0_0x00ff0000"},
    {{lt, r10, 0x0000ab00}, false, al, "lt r10 0x0000ab00", "lt_r10_"
                                                            "0x0000ab00"},
    {{hi, r15, 0xab000000}, false, al, "hi r15 0xab000000", "hi_r15_"
                                                            "0xab000000"},
    {{cs, r12, 0x2ac00000}, false, al, "cs r12 0x2ac00000", "cs_r12_"
                                                            "0x2ac00000"},
    {{hi, r1, 0x000003fc}, false, al, "hi r1 0x000003fc", "hi_r1_0x000003fc"},
    {{lt, r9, 0x00000000}, false, al, "lt r9 0x00000000", "lt_r9_0x00000000"},
    {{vs, r12, 0x003fc000}, false, al, "vs r12 0x003fc000", "vs_r12_"
                                                            "0x003fc000"},
    {{al, r11, 0x000ab000}, false, al, "al r11 0x000ab000", "al_r11_"
                                                            "0x000ab000"},
    {{ls, r10, 0x0ab00000}, false, al, "ls r10 0x0ab00000", "ls_r10_"
                                                            "0x0ab00000"},
    {{vc, r8, 0xff000000}, false, al, "vc r8 0xff000000", "vc_r8_0xff000000"},
    {{vc, r11, 0x2ac00000}, false, al, "vc r11 0x2ac00000", "vc_r11_"
                                                            "0x2ac00000"},
    {{hi, r0, 0x00000ff0}, false, al, "hi r0 0x00000ff0", "hi_r0_0x00000ff0"},
    {{hi, r14, 0x0ab00000}, false, al, "hi r14 0x0ab00000", "hi_r14_"
                                                            "0x0ab00000"},
    {{al, r2, 0x03fc0000}, false, al, "al r2 0x03fc0000", "al_r2_0x03fc0000"},
    {{vs, r4, 0xc000003f}, false, al, "vs r4 0xc000003f", "vs_r4_0xc000003f"},
    {{vs, r0, 0x0000ff00}, false, al, "vs r0 0x0000ff00", "vs_r0_0x0000ff00"},
    {{cc, r6, 0x000ab000}, false, al, "cc r6 0x000ab000", "cc_r6_0x000ab000"},
    {{hi, r13, 0x000ab000}, false, al, "hi r13 0x000ab000", "hi_r13_"
                                                            "0x000ab000"},
    {{ls, r2, 0x00000000}, false, al, "ls r2 0x00000000", "ls_r2_0x00000000"},
    {{eq, r6, 0x000ff000}, false, al, "eq r6 0x000ff000", "eq_r6_0x000ff000"},
    {{pl, r2, 0x000002ac}, false, al, "pl r2 0x000002ac", "pl_r2_0x000002ac"},
    {{mi, r3, 0xab000000}, false, al, "mi r3 0xab000000", "mi_r3_0xab000000"},
    {{ge, r1, 0x0000ab00}, false, al, "ge r1 0x0000ab00", "ge_r1_0x0000ab00"},
    {{ne, r4, 0x3fc00000}, false, al, "ne r4 0x3fc00000", "ne_r4_0x3fc00000"},
    {{cs, r11, 0x00ff0000}, false, al, "cs r11 0x00ff0000", "cs_r11_"
                                                            "0x00ff0000"},
    {{hi, r4, 0x2ac00000}, false, al, "hi r4 0x2ac00000", "hi_r4_0x2ac00000"},
    {{vs, r7, 0x000002ac}, false, al, "vs r7 0x000002ac", "vs_r7_0x000002ac"},
    {{vs, r14, 0x00ff0000}, false, al, "vs r14 0x00ff0000", "vs_r14_"
                                                            "0x00ff0000"},
    {{eq, r5, 0x00003fc0}, false, al, "eq r5 0x00003fc0", "eq_r5_0x00003fc0"},
    {{le, r10, 0x00003fc0}, false, al, "le r10 0x00003fc0", "le_r10_"
                                                            "0x00003fc0"},
    {{vs, r11, 0x00ff0000}, false, al, "vs r11 0x00ff0000", "vs_r11_"
                                                            "0x00ff0000"},
    {{pl, r12, 0x0002ac00}, false, al, "pl r12 0x0002ac00", "pl_r12_"
                                                            "0x0002ac00"},
    {{lt, r6, 0x002ac000}, false, al, "lt r6 0x002ac000", "lt_r6_0x002ac000"},
    {{mi, r13, 0x0003fc00}, false, al, "mi r13 0x0003fc00", "mi_r13_"
                                                            "0x0003fc00"},
    {{le, r15, 0xac000002}, false, al, "le r15 0xac000002", "le_r15_"
                                                            "0xac000002"},
    {{vc, r5, 0x0ff00000}, false, al, "vc r5 0x0ff00000", "vc_r5_0x0ff00000"},
    {{mi, r5, 0x000ab000}, false, al, "mi r5 0x000ab000", "mi_r5_0x000ab000"},
    {{le, r3, 0xb000000a}, false, al, "le r3 0xb000000a", "le_r3_0xb000000a"},
    {{vs, r14, 0x0003fc00}, false, al, "vs r14 0x0003fc00", "vs_r14_"
                                                            "0x0003fc00"},
    {{vs, r13, 0xc000003f}, false, al, "vs r13 0xc000003f", "vs_r13_"
                                                            "0xc000003f"},
    {{hi, r13, 0xab000000}, false, al, "hi r13 0xab000000", "hi_r13_"
                                                            "0xab000000"},
    {{pl, r7, 0x0000ab00}, false, al, "pl r7 0x0000ab00", "pl_r7_0x0000ab00"},
    {{al, r8, 0xc000003f}, false, al, "al r8 0xc000003f", "al_r8_0xc000003f"},
    {{gt, r2, 0x02ac0000}, false, al, "gt r2 0x02ac0000", "gt_r2_0x02ac0000"},
    {{lt, r5, 0x000000ff}, false, al, "lt r5 0x000000ff", "lt_r5_0x000000ff"},
    {{hi, r14, 0xff000000}, false, al, "hi r14 0xff000000", "hi_r14_"
                                                            "0xff000000"},
    {{gt, r4, 0x0ff00000}, false, al, "gt r4 0x0ff00000", "gt_r4_0x0ff00000"},
    {{ls, r14, 0x00000ab0}, false, al, "ls r14 0x00000ab0", "ls_r14_"
                                                            "0x00000ab0"},
    {{vc, r13, 0xc000003f}, false, al, "vc r13 0xc000003f", "vc_r13_"
                                                            "0xc000003f"},
    {{lt, r4, 0x00ff0000}, false, al, "lt r4 0x00ff0000", "lt_r4_0x00ff0000"},
    {{vc, r6, 0x000000ff}, false, al, "vc r6 0x000000ff", "vc_r6_0x000000ff"},
    {{cs, r3, 0x000002ac}, false, al, "cs r3 0x000002ac", "cs_r3_0x000002ac"},
    {{ls, r11, 0xff000000}, false, al, "ls r11 0xff000000", "ls_r11_"
                                                            "0xff000000"},
    {{ls, r10, 0x00003fc0}, false, al, "ls r10 0x00003fc0", "ls_r10_"
                                                            "0x00003fc0"},
    {{ge, r15, 0xf000000f}, false, al, "ge r15 0xf000000f", "ge_r15_"
                                                            "0xf000000f"},
    {{le, r2, 0x0000ff00}, false, al, "le r2 0x0000ff00", "le_r2_0x0000ff00"},
    {{al, r3, 0x003fc000}, false, al, "al r3 0x003fc000", "al_r3_0x003fc000"},
    {{hi, r3, 0x00ff0000}, false, al, "hi r3 0x00ff0000", "hi_r3_0x00ff0000"},
    {{ne, r1, 0x002ac000}, false, al, "ne r1 0x002ac000", "ne_r1_0x002ac000"},
    {{eq, r15, 0x2ac00000}, false, al, "eq r15 0x2ac00000", "eq_r15_"
                                                            "0x2ac00000"},
    {{cs, r9, 0xc000002a}, false, al, "cs r9 0xc000002a", "cs_r9_0xc000002a"},
    {{lt, r0, 0x00002ac0}, false, al, "lt r0 0x00002ac0", "lt_r0_0x00002ac0"},
    {{cs, r12, 0x000000ff}, false, al, "cs r12 0x000000ff", "cs_r12_"
                                                            "0x000000ff"},
    {{al, r6, 0x0002ac00}, false, al, "al r6 0x0002ac00", "al_r6_0x0002ac00"},
    {{vs, r10, 0x003fc000}, false, al, "vs r10 0x003fc000", "vs_r10_"
                                                            "0x003fc000"},
    {{al, r15, 0xc000003f}, false, al, "al r15 0xc000003f", "al_r15_"
                                                            "0xc000003f"},
    {{ne, r7, 0x002ac000}, false, al, "ne r7 0x002ac000", "ne_r7_0x002ac000"},
    {{eq, r6, 0x00000ab0}, false, al, "eq r6 0x00000ab0", "eq_r6_0x00000ab0"},
    {{cc, r1, 0x000ab000}, false, al, "cc r1 0x000ab000", "cc_r1_0x000ab000"},
    {{gt, r0, 0x00000ab0}, false, al, "gt r0 0x00000ab0", "gt_r0_0x00000ab0"},
    {{al, r6, 0x000002ac}, false, al, "al r6 0x000002ac", "al_r6_0x000002ac"},
    {{vc, r13, 0x0003fc00}, false, al, "vc r13 0x0003fc00", "vc_r13_"
                                                            "0x0003fc00"},
    {{cc, r12, 0x003fc000}, false, al, "cc r12 0x003fc000", "cc_r12_"
                                                            "0x003fc000"},
    {{le, r1, 0x00003fc0}, false, al, "le r1 0x00003fc0", "le_r1_0x00003fc0"},
    {{pl, r10, 0x00002ac0}, false, al, "pl r10 0x00002ac0", "pl_r10_"
                                                            "0x00002ac0"},
    {{hi, r3, 0x000ff000}, false, al, "hi r3 0x000ff000", "hi_r3_0x000ff000"},
    {{vc, r12, 0x00003fc0}, false, al, "vc r12 0x00003fc0", "vc_r12_"
                                                            "0x00003fc0"},
    {{gt, r5, 0x000003fc}, false, al, "gt r5 0x000003fc", "gt_r5_0x000003fc"},
    {{ge, r15, 0x0002ac00}, false, al, "ge r15 0x0002ac00", "ge_r15_"
                                                            "0x0002ac00"},
    {{vs, r15, 0x02ac0000}, false, al, "vs r15 0x02ac0000", "vs_r15_"
                                                            "0x02ac0000"},
    {{vs, r15, 0x000003fc}, false, al, "vs r15 0x000003fc", "vs_r15_"
                                                            "0x000003fc"},
    {{cc, r10, 0x0ff00000}, false, al, "cc r10 0x0ff00000", "cc_r10_"
                                                            "0x0ff00000"},
    {{vc, r10, 0x3fc00000}, false, al, "vc r10 0x3fc00000", "vc_r10_"
                                                            "0x3fc00000"},
    {{pl, r15, 0x00000ab0}, false, al, "pl r15 0x00000ab0", "pl_r15_"
                                                            "0x00000ab0"},
    {{hi, r4, 0x00002ac0}, false, al, "hi r4 0x00002ac0", "hi_r4_0x00002ac0"},
    {{ge, r0, 0x00ff0000}, false, al, "ge r0 0x00ff0000", "ge_r0_0x00ff0000"},
    {{gt, r8, 0x000002ac}, false, al, "gt r8 0x000002ac", "gt_r8_0x000002ac"},
    {{cc, r14, 0x02ac0000}, false, al, "cc r14 0x02ac0000", "cc_r14_"
                                                            "0x02ac0000"},
    {{ge, r3, 0xac000002}, false, al, "ge r3 0xac000002", "ge_r3_0xac000002"},
    {{ne, r12, 0x003fc000}, false, al, "ne r12 0x003fc000", "ne_r12_"
                                                            "0x003fc000"},
    {{gt, r13, 0xf000000f}, false, al, "gt r13 0xf000000f", "gt_r13_"
                                                            "0xf000000f"},
    {{al, r11, 0x0ab00000}, false, al, "al r11 0x0ab00000", "al_r11_"
                                                            "0x0ab00000"},
    {{ge, r12, 0x2ac00000}, false, al, "ge r12 0x2ac00000", "ge_r12_"
                                                            "0x2ac00000"},
    {{al, r11, 0x00000ff0}, false, al, "al r11 0x00000ff0", "al_r11_"
                                                            "0x00000ff0"},
    {{cs, r11, 0xc000002a}, false, al, "cs r11 0xc000002a", "cs_r11_"
                                                            "0xc000002a"},
    {{gt, r8, 0x0002ac00}, false, al, "gt r8 0x0002ac00", "gt_r8_0x0002ac00"},
    {{hi, r14, 0xab000000}, false, al, "hi r14 0xab000000", "hi_r14_"
                                                            "0xab000000"},
    {{lt, r1, 0x000ab000}, false, al, "lt r1 0x000ab000", "lt_r1_0x000ab000"},
    {{ge, r13, 0x002ac000}, false, al, "ge r13 0x002ac000", "ge_r13_"
                                                            "0x002ac000"},
    {{pl, r11, 0x0003fc00}, false, al, "pl r11 0x0003fc00", "pl_r11_"
                                                            "0x0003fc00"},
    {{hi, r8, 0x000002ac}, false, al, "hi r8 0x000002ac", "hi_r8_0x000002ac"},
    {{ge, r4, 0x0000ab00}, false, al, "ge r4 0x0000ab00", "ge_r4_0x0000ab00"},
    {{cc, r4, 0x3fc00000}, false, al, "cc r4 0x3fc00000", "cc_r4_0x3fc00000"},
    {{mi, r4, 0x0003fc00}, false, al, "mi r4 0x0003fc00", "mi_r4_0x0003fc00"},
    {{vs, r13, 0x000000ff}, false, al, "vs r13 0x000000ff", "vs_r13_"
                                                            "0x000000ff"},
    {{le, r1, 0xb000000a}, false, al, "le r1 0xb000000a", "le_r1_0xb000000a"},
    {{vs, r10, 0x000000ff}, false, al, "vs r10 0x000000ff", "vs_r10_"
                                                            "0x000000ff"},
    {{cc, r3, 0xab000000}, false, al, "cc r3 0xab000000", "cc_r3_0xab000000"},
    {{lt, r3, 0x0000ff00}, false, al, "lt r3 0x0000ff00", "lt_r3_0x0000ff00"},
    {{hi, r12, 0xfc000003}, false, al, "hi r12 0xfc000003", "hi_r12_"
                                                            "0xfc000003"},
    {{le, r15, 0xfc000003}, false, al, "le r15 0xfc000003", "le_r15_"
                                                            "0xfc000003"},
    {{hi, r4, 0x02ac0000}, false, al, "hi r4 0x02ac0000", "hi_r4_0x02ac0000"},
    {{hi, r11, 0x0ab00000}, false, al, "hi r11 0x0ab00000", "hi_r11_"
                                                            "0x0ab00000"},
    {{pl, r15, 0x0002ac00}, false, al, "pl r15 0x0002ac00", "pl_r15_"
                                                            "0x0002ac00"},
    {{pl, r10, 0x0000ab00}, false, al, "pl r10 0x0000ab00", "pl_r10_"
                                                            "0x0000ab00"},
    {{ne, r6, 0xab000000}, false, al, "ne r6 0xab000000", "ne_r6_0xab000000"},
    {{cs, r4, 0x02ac0000}, false, al, "cs r4 0x02ac0000", "cs_r4_0x02ac0000"},
    {{mi, r1, 0xfc000003}, false, al, "mi r1 0xfc000003", "mi_r1_0xfc000003"},
    {{hi, r8, 0xab000000}, false, al, "hi r8 0xab000000", "hi_r8_0xab000000"},
    {{gt, r3, 0x3fc00000}, false, al, "gt r3 0x3fc00000", "gt_r3_0x3fc00000"},
    {{ne, r13, 0x0000ff00}, false, al, "ne r13 0x0000ff00", "ne_r13_"
                                                            "0x0000ff00"},
    {{vc, r2, 0x0003fc00}, false, al, "vc r2 0x0003fc00", "vc_r2_0x0003fc00"},
    {{al, r14, 0x000000ab}, false, al, "al r14 0x000000ab", "al_r14_"
                                                            "0x000000ab"},
    {{pl, r15, 0x000ff000}, false, al, "pl r15 0x000ff000", "pl_r15_"
                                                            "0x000ff000"},
    {{vs, r5, 0x00002ac0}, false, al, "vs r5 0x00002ac0", "vs_r5_0x00002ac0"},
    {{eq, r0, 0x0ab00000}, false, al, "eq r0 0x0ab00000", "eq_r0_0x0ab00000"},
    {{lt, r15, 0xfc000003}, false, al, "lt r15 0xfc000003", "lt_r15_"
                                                            "0xfc000003"},
    {{vs, r9, 0x3fc00000}, false, al, "vs r9 0x3fc00000", "vs_r9_0x3fc00000"},
    {{vc, r14, 0x00003fc0}, false, al, "vc r14 0x00003fc0", "vc_r14_"
                                                            "0x00003fc0"},
    {{lt, r3, 0x0003fc00}, false, al, "lt r3 0x0003fc00", "lt_r3_0x0003fc00"},
    {{cs, r12, 0x000000ab}, false, al, "cs r12 0x000000ab", "cs_r12_"
                                                            "0x000000ab"},
    {{al, r14, 0x00002ac0}, false, al, "al r14 0x00002ac0", "al_r14_"
                                                            "0x00002ac0"},
    {{le, r5, 0x0000ab00}, false, al, "le r5 0x0000ab00", "le_r5_0x0000ab00"},
    {{pl, r13, 0xc000002a}, false, al, "pl r13 0xc000002a", "pl_r13_"
                                                            "0xc000002a"},
    {{eq, r1, 0x00002ac0}, false, al, "eq r1 0x00002ac0", "eq_r1_0x00002ac0"},
    {{cs, r9, 0xc000003f}, false, al, "cs r9 0xc000003f", "cs_r9_0xc000003f"},
    {{ne, r9, 0x000ff000}, false, al, "ne r9 0x000ff000", "ne_r9_0x000ff000"},
    {{gt, r11, 0x00ab0000}, false, al, "gt r11 0x00ab0000", "gt_r11_"
                                                            "0x00ab0000"},
    {{hi, r5, 0xff000000}, false, al, "hi r5 0xff000000", "hi_r5_0xff000000"},
    {{le, r5, 0x0ab00000}, false, al, "le r5 0x0ab00000", "le_r5_0x0ab00000"},
    {{gt, r11, 0x00000ff0}, false, al, "gt r11 0x00000ff0", "gt_r11_"
                                                            "0x00000ff0"},
    {{al, r14, 0x0ff00000}, false, al, "al r14 0x0ff00000", "al_r14_"
                                                            "0x0ff00000"},
    {{lt, r10, 0x0ff00000}, false, al, "lt r10 0x0ff00000", "lt_r10_"
                                                            "0x0ff00000"},
    {{pl, r13, 0x003fc000}, false, al, "pl r13 0x003fc000", "pl_r13_"
                                                            "0x003fc000"},
    {{eq, r5, 0xab000000}, false, al, "eq r5 0xab000000", "eq_r5_0xab000000"},
    {{eq, r7, 0x00002ac0}, false, al, "eq r7 0x00002ac0", "eq_r7_0x00002ac0"},
    {{eq, r7, 0x002ac000}, false, al, "eq r7 0x002ac000", "eq_r7_0x002ac000"},
    {{gt, r12, 0x00003fc0}, false, al, "gt r12 0x00003fc0", "gt_r12_"
                                                            "0x00003fc0"},
    {{mi, r14, 0xfc000003}, false, al, "mi r14 0xfc000003", "mi_r14_"
                                                            "0xfc000003"},
    {{al, r8, 0x0000ab00}, false, al, "al r8 0x0000ab00", "al_r8_0x0000ab00"},
    {{ge, r4, 0x00ab0000}, false, al, "ge r4 0x00ab0000", "ge_r4_0x00ab0000"},
    {{vs, r13, 0xb000000a}, false, al, "vs r13 0xb000000a", "vs_r13_"
                                                            "0xb000000a"},
    {{vc, r15, 0xac000002}, false, al, "vc r15 0xac000002", "vc_r15_"
                                                            "0xac000002"},
    {{cs, r13, 0x000002ac}, false, al, "cs r13 0x000002ac", "cs_r13_"
                                                            "0x000002ac"},
    {{vs, r11, 0x00000ab0}, false, al, "vs r11 0x00000ab0", "vs_r11_"
                                                            "0x00000ab0"},
    {{lt, r8, 0x00000ff0}, false, al, "lt r8 0x00000ff0", "lt_r8_0x00000ff0"},
    {{eq, r13, 0x00ab0000}, false, al, "eq r13 0x00ab0000", "eq_r13_"
                                                            "0x00ab0000"},
    {{lt, r11, 0xf000000f}, false, al, "lt r11 0xf000000f", "lt_r11_"
                                                            "0xf000000f"},
    {{ge, r6, 0x0003fc00}, false, al, "ge r6 0x0003fc00", "ge_r6_0x0003fc00"},
    {{al, r10, 0x3fc00000}, false, al, "al r10 0x3fc00000", "al_r10_"
                                                            "0x3fc00000"},
    {{le, r15, 0x0003fc00}, false, al, "le r15 0x0003fc00", "le_r15_"
                                                            "0x0003fc00"},
    {{le, r13, 0x000003fc}, false, al, "le r13 0x000003fc", "le_r13_"
                                                            "0x000003fc"},
    {{lt, r4, 0x00003fc0}, false, al, "lt r4 0x00003fc0", "lt_r4_0x00003fc0"},
    {{eq, r3, 0x00002ac0}, false, al, "eq r3 0x00002ac0", "eq_r3_0x00002ac0"},
    {{vs, r3, 0x000000ff}, false, al, "vs r3 0x000000ff", "vs_r3_0x000000ff"},
    {{vs, r1, 0x000000ff}, false, al, "vs r1 0x000000ff", "vs_r1_0x000000ff"},
    {{lt, r4, 0x0000ab00}, false, al, "lt r4 0x0000ab00", "lt_r4_0x0000ab00"},
    {{pl, r2, 0xc000003f}, false, al, "pl r2 0xc000003f", "pl_r2_0xc000003f"},
    {{al, r5, 0xff000000}, false, al, "al r5 0xff000000", "al_r5_0xff000000"},
    {{le, r14, 0x03fc0000}, false, al, "le r14 0x03fc0000", "le_r14_"
                                                            "0x03fc0000"},
    {{cs, r14, 0x000000ab}, false, al, "cs r14 0x000000ab", "cs_r14_"
                                                            "0x000000ab"},
    {{ne, r11, 0xfc000003}, false, al, "ne r11 0xfc000003", "ne_r11_"
                                                            "0xfc000003"},
    {{hi, r8, 0x00003fc0}, false, al, "hi r8 0x00003fc0", "hi_r8_0x00003fc0"},
    {{lt, r7, 0x00ff0000}, false, al, "lt r7 0x00ff0000", "lt_r7_0x00ff0000"},
    {{le, r14, 0x00003fc0}, false, al, "le r14 0x00003fc0", "le_r14_"
                                                            "0x00003fc0"},
    {{al, r8, 0x00ff0000}, false, al, "al r8 0x00ff0000", "al_r8_0x00ff0000"},
    {{vc, r6, 0x000ab000}, false, al, "vc r6 0x000ab000", "vc_r6_0x000ab000"},
    {{ge, r6, 0x00000ff0}, false, al, "ge r6 0x00000ff0", "ge_r6_0x00000ff0"},
    {{le, r3, 0x00ff0000}, false, al, "le r3 0x00ff0000", "le_r3_0x00ff0000"},
    {{cs, r8, 0x0000ab00}, false, al, "cs r8 0x0000ab00", "cs_r8_0x0000ab00"},
    {{vc, r2, 0x0000ab00}, false, al, "vc r2 0x0000ab00", "vc_r2_0x0000ab00"},
    {{mi, r5, 0x00003fc0}, false, al, "mi r5 0x00003fc0", "mi_r5_0x00003fc0"},
    {{hi, r11, 0x00ff0000}, false, al, "hi r11 0x00ff0000", "hi_r11_"
                                                            "0x00ff0000"},
    {{cs, r2, 0x000000ff}, false, al, "cs r2 0x000000ff", "cs_r2_0x000000ff"},
    {{eq, r2, 0x03fc0000}, false, al, "eq r2 0x03fc0000", "eq_r2_0x03fc0000"},
    {{eq, r3, 0x003fc000}, false, al, "eq r3 0x003fc000", "eq_r3_0x003fc000"},
    {{lt, r10, 0x002ac000}, false, al, "lt r10 0x002ac000", "lt_r10_"
                                                            "0x002ac000"},
    {{lt, r7, 0x000ff000}, false, al, "lt r7 0x000ff000", "lt_r7_0x000ff000"},
    {{eq, r0, 0x03fc0000}, false, al, "eq r0 0x03fc0000", "eq_r0_0x03fc0000"},
    {{vs, r5, 0x00000ab0}, false, al, "vs r5 0x00000ab0", "vs_r5_0x00000ab0"},
    {{vc, r9, 0x0000ab00}, false, al, "vc r9 0x0000ab00", "vc_r9_0x0000ab00"},
    {{vc, r5, 0x0ab00000}, false, al, "vc r5 0x0ab00000", "vc_r5_0x0ab00000"},
    {{cs, r15, 0x000000ab}, false, al, "cs r15 0x000000ab", "cs_r15_"
                                                            "0x000000ab"},
    {{cc, r12, 0x00000000}, false, al, "cc r12 0x00000000", "cc_r12_"
                                                            "0x00000000"},
    {{lt, r12, 0x000ab000}, false, al, "lt r12 0x000ab000", "lt_r12_"
                                                            "0x000ab000"},
    {{mi, r13, 0xfc000003}, false, al, "mi r13 0xfc000003", "mi_r13_"
                                                            "0xfc000003"},
    {{cs, r8, 0xff000000}, false, al, "cs r8 0xff000000", "cs_r8_0xff000000"},
    {{cs, r0, 0x00000000}, false, al, "cs r0 0x00000000", "cs_r0_0x00000000"},
    {{gt, r6, 0x003fc000}, false, al, "gt r6 0x003fc000", "gt_r6_0x003fc000"},
    {{ls, r7, 0xb000000a}, false, al, "ls r7 0xb000000a", "ls_r7_0xb000000a"},
    {{ne, r7, 0x00002ac0}, false, al, "ne r7 0x00002ac0", "ne_r7_0x00002ac0"},
    {{ls, r13, 0x003fc000}, false, al, "ls r13 0x003fc000", "ls_r13_"
                                                            "0x003fc000"},
    {{vs, r5, 0x00ab0000}, false, al, "vs r5 0x00ab0000", "vs_r5_0x00ab0000"},
    {{hi, r7, 0x002ac000}, false, al, "hi r7 0x002ac000", "hi_r7_0x002ac000"},
    {{vc, r4, 0x000002ac}, false, al, "vc r4 0x000002ac", "vc_r4_0x000002ac"},
    {{al, r10, 0x00002ac0},
     false,
     al,
     "al r10 0x00002ac0",
     "al_r10_0x00002ac0"}};

// These headers each contain an array of `TestResult` with the reference output
// values. The reference arrays are names `kReference{mnemonic}`.
#include "aarch32/traces/assembler-cond-rd-operand-const-a32-cmn.h"
#include "aarch32/traces/assembler-cond-rd-operand-const-a32-cmp.h"
#include "aarch32/traces/assembler-cond-rd-operand-const-a32-mov.h"
#include "aarch32/traces/assembler-cond-rd-operand-const-a32-movs.h"
#include "aarch32/traces/assembler-cond-rd-operand-const-a32-mvn.h"
#include "aarch32/traces/assembler-cond-rd-operand-const-a32-mvns.h"
#include "aarch32/traces/assembler-cond-rd-operand-const-a32-teq.h"
#include "aarch32/traces/assembler-cond-rd-operand-const-a32-tst.h"


// The maximum number of errors to report in detail for each test.
const unsigned kErrorReportLimit = 8;

typedef void (MacroAssembler::*Fn)(Condition cond,
                                   Register rd,
                                   const Operand& op);

void TestHelper(Fn instruction,
                const char* mnemonic,
                const TestResult reference[]) {
  unsigned total_error_count = 0;
  MacroAssembler masm(BUF_SIZE);

  masm.UseA32();

  for (unsigned i = 0; i < ARRAY_SIZE(kTests); i++) {
    // Values to pass to the macro-assembler.
    Condition cond = kTests[i].operands.cond;
    Register rd = kTests[i].operands.rd;
    uint32_t immediate = kTests[i].operands.immediate;
    Operand op(immediate);

    int32_t start = masm.GetCursorOffset();
    {
      // We never generate more that 4 bytes, as IT instructions are only
      // allowed for narrow encodings.
      ExactAssemblyScope scope(&masm, 4, ExactAssemblyScope::kMaximumSize);
      if (kTests[i].in_it_block) {
        masm.it(kTests[i].it_condition);
      }
      (masm.*instruction)(cond, rd, op);
    }
    int32_t end = masm.GetCursorOffset();

    const byte* result_ptr =
        masm.GetBuffer()->GetOffsetAddress<const byte*>(start);
    VIXL_ASSERT(start < end);
    uint32_t result_size = end - start;

    if (Test::generate_test_trace()) {
      // Print the result bytes.
      printf("const byte kInstruction_%s_%s[] = {\n",
             mnemonic,
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
      if (kTests[i].in_it_block) {
        printf(" // It %s; %s %s\n};\n",
               kTests[i].it_condition.GetName(),
               mnemonic,
               kTests[i].operands_description);
      } else {
        printf(" // %s %s\n};\n", mnemonic, kTests[i].operands_description);
      }
    } else {
      // Check we've emitted the exact same encoding as present in the
      // trace file. Only print up to `kErrorReportLimit` errors.
      if (((result_size != reference[i].size) ||
           (memcmp(result_ptr, reference[i].encoding, reference[i].size) !=
            0)) &&
          (++total_error_count <= kErrorReportLimit)) {
        printf("Error when testing \"%s\" with operands \"%s\":\n",
               mnemonic,
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
    printf("const TestResult kReference%s[] = {\n", mnemonic);
    for (unsigned i = 0; i < ARRAY_SIZE(kTests); i++) {
      printf("  {\n");
      printf("    ARRAY_SIZE(kInstruction_%s_%s),\n",
             mnemonic,
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
    // Crash if the test failed.
    VIXL_CHECK(total_error_count == 0);
  }
}

// Instantiate tests for each instruction in the list.
#define TEST(mnemonic)                                                      \
  void Test_##mnemonic() {                                                  \
    TestHelper(&MacroAssembler::mnemonic, #mnemonic, kReference##mnemonic); \
  }                                                                         \
  Test test_##mnemonic(                                                     \
      "AARCH32_ASSEMBLER_COND_RD_OPERAND_CONST_A32_" #mnemonic,             \
      &Test_##mnemonic);
FOREACH_INSTRUCTION(TEST)
#undef TEST

}  // namespace

}  // namespace aarch32
}  // namespace vixl
