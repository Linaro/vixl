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
  M(Adc)                       \
  M(Adcs)                      \
  M(Add)                       \
  M(Adds)                      \
  M(And)                       \
  M(Ands)                      \
  M(Bic)                       \
  M(Bics)                      \
  M(Eor)                       \
  M(Eors)                      \
  M(Orn)                       \
  M(Orns)                      \
  M(Orr)                       \
  M(Orrs)                      \
  M(Rsb)                       \
  M(Rsbs)                      \
  M(Sbc)                       \
  M(Sbcs)                      \
  M(Sub)                       \
  M(Subs)


// Values to be passed to the assembler to produce the instruction under test.
struct Operands {
  Condition cond;
  Register rd;
  Register rn;
  uint32_t immediate;
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
    {{al, r13, r14, 0x02ac0000},
     "",
     "al r13 r14 0x02ac0000",
     "al_r13_r14_0x02ac0000"},
    {{al, r10, r1, 0x00156000},
     "",
     "al r10 r1 0x00156000",
     "al_r10_r1_0x00156000"},
    {{al, r10, r0, 0x000003fc},
     "",
     "al r10 r0 0x000003fc",
     "al_r10_r0_0x000003fc"},
    {{al, r1, r11, 0x2ac00000},
     "",
     "al r1 r11 0x2ac00000",
     "al_r1_r11_0x2ac00000"},
    {{al, r8, r6, 0x00156000}, "", "al r8 r6 0x00156000", "al_r8_r6_"
                                                          "0x00156000"},
    {{al, r7, r12, 0x00ff0000},
     "",
     "al r7 r12 0x00ff0000",
     "al_r7_r12_0x00ff0000"},
    {{al, r12, r3, 0x00ff0000},
     "",
     "al r12 r3 0x00ff0000",
     "al_r12_r3_0x00ff0000"},
    {{al, r4, r7, 0x0000ff00}, "", "al r4 r7 0x0000ff00", "al_r4_r7_"
                                                          "0x0000ff00"},
    {{al, r11, r13, 0x0ab00000},
     "",
     "al r11 r13 0x0ab00000",
     "al_r11_r13_0x0ab00000"},
    {{al, r6, r12, 0xff00ff00},
     "",
     "al r6 r12 0xff00ff00",
     "al_r6_r12_0xff00ff00"},
    {{al, r12, r8, 0x003fc000},
     "",
     "al r12 r8 0x003fc000",
     "al_r12_r8_0x003fc000"},
    {{al, r5, r12, 0x00ab00ab},
     "",
     "al r5 r12 0x00ab00ab",
     "al_r5_r12_0x00ab00ab"},
    {{al, r7, r6, 0x00ab00ab}, "", "al r7 r6 0x00ab00ab", "al_r7_r6_"
                                                          "0x00ab00ab"},
    {{al, r0, r1, 0x00ab00ab}, "", "al r0 r1 0x00ab00ab", "al_r0_r1_"
                                                          "0x00ab00ab"},
    {{al, r9, r9, 0x000001fe}, "", "al r9 r9 0x000001fe", "al_r9_r9_"
                                                          "0x000001fe"},
    {{al, r2, r8, 0xab00ab00}, "", "al r2 r8 0xab00ab00", "al_r2_r8_"
                                                          "0xab00ab00"},
    {{al, r9, r10, 0x00ff0000},
     "",
     "al r9 r10 0x00ff0000",
     "al_r9_r10_0x00ff0000"},
    {{al, r8, r8, 0x55800000}, "", "al r8 r8 0x55800000", "al_r8_r8_"
                                                          "0x55800000"},
    {{al, r6, r7, 0x00ab00ab}, "", "al r6 r7 0x00ab00ab", "al_r6_r7_"
                                                          "0x00ab00ab"},
    {{al, r5, r9, 0xff000000}, "", "al r5 r9 0xff000000", "al_r5_r9_"
                                                          "0xff000000"},
    {{al, r8, r8, 0x00ab0000}, "", "al r8 r8 0x00ab0000", "al_r8_r8_"
                                                          "0x00ab0000"},
    {{al, r5, r8, 0xab00ab00}, "", "al r5 r8 0xab00ab00", "al_r5_r8_"
                                                          "0xab00ab00"},
    {{al, r0, r12, 0xab000000},
     "",
     "al r0 r12 0xab000000",
     "al_r0_r12_0xab000000"},
    {{al, r13, r11, 0xab000000},
     "",
     "al r13 r11 0xab000000",
     "al_r13_r11_0xab000000"},
    {{al, r14, r3, 0xab00ab00},
     "",
     "al r14 r3 0xab00ab00",
     "al_r14_r3_0xab00ab00"},
    {{al, r0, r1, 0x0003fc00}, "", "al r0 r1 0x0003fc00", "al_r0_r1_"
                                                          "0x0003fc00"},
    {{al, r14, r13, 0x0ab00000},
     "",
     "al r14 r13 0x0ab00000",
     "al_r14_r13_0x0ab00000"},
    {{al, r6, r0, 0x0002ac00}, "", "al r6 r0 0x0002ac00", "al_r6_r0_"
                                                          "0x0002ac00"},
    {{al, r6, r8, 0x55800000}, "", "al r6 r8 0x55800000", "al_r6_r8_"
                                                          "0x55800000"},
    {{al, r2, r14, 0x01560000},
     "",
     "al r2 r14 0x01560000",
     "al_r2_r14_0x01560000"},
    {{al, r5, r13, 0x03fc0000},
     "",
     "al r5 r13 0x03fc0000",
     "al_r5_r13_0x03fc0000"},
    {{al, r7, r6, 0x00000ab0}, "", "al r7 r6 0x00000ab0", "al_r7_r6_"
                                                          "0x00000ab0"},
    {{al, r3, r14, 0x007f8000},
     "",
     "al r3 r14 0x007f8000",
     "al_r3_r14_0x007f8000"},
    {{al, r9, r4, 0x00558000}, "", "al r9 r4 0x00558000", "al_r9_r4_"
                                                          "0x00558000"},
    {{al, r10, r11, 0x00002ac0},
     "",
     "al r10 r11 0x00002ac0",
     "al_r10_r11_0x00002ac0"},
    {{al, r1, r5, 0x003fc000}, "", "al r1 r5 0x003fc000", "al_r1_r5_"
                                                          "0x003fc000"},
    {{al, r7, r7, 0x00003fc0}, "", "al r7 r7 0x00003fc0", "al_r7_r7_"
                                                          "0x00003fc0"},
    {{al, r5, r3, 0x000007f8}, "", "al r5 r3 0x000007f8", "al_r5_r3_"
                                                          "0x000007f8"},
    {{al, r4, r3, 0x00001560}, "", "al r4 r3 0x00001560", "al_r4_r3_"
                                                          "0x00001560"},
    {{al, r5, r3, 0x03fc0000}, "", "al r5 r3 0x03fc0000", "al_r5_r3_"
                                                          "0x03fc0000"},
    {{al, r2, r6, 0x55800000}, "", "al r2 r6 0x55800000", "al_r2_r6_"
                                                          "0x55800000"},
    {{al, r13, r5, 0x0000ab00},
     "",
     "al r13 r5 0x0000ab00",
     "al_r13_r5_0x0000ab00"},
    {{al, r0, r11, 0xab00ab00},
     "",
     "al r0 r11 0xab00ab00",
     "al_r0_r11_0xab00ab00"},
    {{al, r14, r12, 0x00ff00ff},
     "",
     "al r14 r12 0x00ff00ff",
     "al_r14_r12_0x00ff00ff"},
    {{al, r13, r8, 0x7f800000},
     "",
     "al r13 r8 0x7f800000",
     "al_r13_r8_0x7f800000"},
    {{al, r1, r2, 0x15600000}, "", "al r1 r2 0x15600000", "al_r1_r2_"
                                                          "0x15600000"},
    {{al, r7, r6, 0xab000000}, "", "al r7 r6 0xab000000", "al_r7_r6_"
                                                          "0xab000000"},
    {{al, r1, r9, 0x00000ff0}, "", "al r1 r9 0x00000ff0", "al_r1_r9_"
                                                          "0x00000ff0"},
    {{al, r12, r8, 0x0007f800},
     "",
     "al r12 r8 0x0007f800",
     "al_r12_r8_0x0007f800"},
    {{al, r0, r8, 0x00ab0000}, "", "al r0 r8 0x00ab0000", "al_r0_r8_"
                                                          "0x00ab0000"},
    {{al, r11, r11, 0x000000ff},
     "",
     "al r11 r11 0x000000ff",
     "al_r11_r11_0x000000ff"},
    {{al, r12, r13, 0xff000000},
     "",
     "al r12 r13 0xff000000",
     "al_r12_r13_0xff000000"},
    {{al, r1, r3, 0x0ab00000}, "", "al r1 r3 0x0ab00000", "al_r1_r3_"
                                                          "0x0ab00000"},
    {{al, r2, r10, 0x0001fe00},
     "",
     "al r2 r10 0x0001fe00",
     "al_r2_r10_0x0001fe00"},
    {{al, r14, r2, 0x01fe0000},
     "",
     "al r14 r2 0x01fe0000",
     "al_r14_r2_0x01fe0000"},
    {{al, r3, r4, 0x000000ff}, "", "al r3 r4 0x000000ff", "al_r3_r4_"
                                                          "0x000000ff"},
    {{al, r3, r13, 0x00000558},
     "",
     "al r3 r13 0x00000558",
     "al_r3_r13_0x00000558"},
    {{al, r13, r10, 0x00055800},
     "",
     "al r13 r10 0x00055800",
     "al_r13_r10_0x00055800"},
    {{al, r1, r10, 0xff000000},
     "",
     "al r1 r10 0xff000000",
     "al_r1_r10_0xff000000"},
    {{al, r0, r7, 0x2ac00000}, "", "al r0 r7 0x2ac00000", "al_r0_r7_"
                                                          "0x2ac00000"},
    {{al, r12, r1, 0xab000000},
     "",
     "al r12 r1 0xab000000",
     "al_r12_r1_0xab000000"},
    {{al, r9, r14, 0x00003fc0},
     "",
     "al r9 r14 0x00003fc0",
     "al_r9_r14_0x00003fc0"},
    {{al, r7, r2, 0x2ac00000}, "", "al r7 r2 0x2ac00000", "al_r7_r2_"
                                                          "0x2ac00000"},
    {{al, r14, r4, 0x00001fe0},
     "",
     "al r14 r4 0x00001fe0",
     "al_r14_r4_0x00001fe0"},
    {{al, r12, r8, 0x00007f80},
     "",
     "al r12 r8 0x00007f80",
     "al_r12_r8_0x00007f80"},
    {{al, r7, r10, 0x00000ab0},
     "",
     "al r7 r10 0x00000ab0",
     "al_r7_r10_0x00000ab0"},
    {{al, r13, r6, 0x00ab0000},
     "",
     "al r13 r6 0x00ab0000",
     "al_r13_r6_0x00ab0000"},
    {{al, r7, r9, 0x0000ff00}, "", "al r7 r9 0x0000ff00", "al_r7_r9_"
                                                          "0x0000ff00"},
    {{al, r2, r12, 0xff00ff00},
     "",
     "al r2 r12 0xff00ff00",
     "al_r2_r12_0xff00ff00"},
    {{al, r1, r6, 0x00000156}, "", "al r1 r6 0x00000156", "al_r1_r6_"
                                                          "0x00000156"},
    {{al, r7, r5, 0x03fc0000}, "", "al r7 r5 0x03fc0000", "al_r7_r5_"
                                                          "0x03fc0000"},
    {{al, r2, r9, 0x01fe0000}, "", "al r2 r9 0x01fe0000", "al_r2_r9_"
                                                          "0x01fe0000"},
    {{al, r10, r12, 0x00002ac0},
     "",
     "al r10 r12 0x00002ac0",
     "al_r10_r12_0x00002ac0"},
    {{al, r14, r10, 0x7f800000},
     "",
     "al r14 r10 0x7f800000",
     "al_r14_r10_0x7f800000"},
    {{al, r2, r8, 0x02ac0000}, "", "al r2 r8 0x02ac0000", "al_r2_r8_"
                                                          "0x02ac0000"},
    {{al, r4, r9, 0x000001fe}, "", "al r4 r9 0x000001fe", "al_r4_r9_"
                                                          "0x000001fe"},
    {{al, r10, r10, 0x000001fe},
     "",
     "al r10 r10 0x000001fe",
     "al_r10_r10_0x000001fe"},
    {{al, r6, r6, 0x3fc00000}, "", "al r6 r6 0x3fc00000", "al_r6_r6_"
                                                          "0x3fc00000"},
    {{al, r4, r12, 0x000003fc},
     "",
     "al r4 r12 0x000003fc",
     "al_r4_r12_0x000003fc"},
    {{al, r0, r2, 0x0000ff00}, "", "al r0 r2 0x0000ff00", "al_r0_r2_"
                                                          "0x0000ff00"},
    {{al, r9, r0, 0x003fc000}, "", "al r9 r0 0x003fc000", "al_r9_r0_"
                                                          "0x003fc000"},
    {{al, r7, r4, 0x000002ac}, "", "al r7 r4 0x000002ac", "al_r7_r4_"
                                                          "0x000002ac"},
    {{al, r6, r6, 0x7f800000}, "", "al r6 r6 0x7f800000", "al_r6_r6_"
                                                          "0x7f800000"},
    {{al, r6, r8, 0x00015600}, "", "al r6 r8 0x00015600", "al_r6_r8_"
                                                          "0x00015600"},
    {{al, r10, r0, 0x00000ff0},
     "",
     "al r10 r0 0x00000ff0",
     "al_r10_r0_0x00000ff0"},
    {{al, r8, r1, 0xffffffff}, "", "al r8 r1 0xffffffff", "al_r8_r1_"
                                                          "0xffffffff"},
    {{al, r3, r7, 0x00ab00ab}, "", "al r3 r7 0x00ab00ab", "al_r3_r7_"
                                                          "0x00ab00ab"},
    {{al, r8, r11, 0x01fe0000},
     "",
     "al r8 r11 0x01fe0000",
     "al_r8_r11_0x01fe0000"},
    {{al, r3, r1, 0x00ff0000}, "", "al r3 r1 0x00ff0000", "al_r3_r1_"
                                                          "0x00ff0000"},
    {{al, r5, r4, 0x000001fe}, "", "al r5 r4 0x000001fe", "al_r5_r4_"
                                                          "0x000001fe"},
    {{al, r7, r10, 0x00000558},
     "",
     "al r7 r10 0x00000558",
     "al_r7_r10_0x00000558"},
    {{al, r8, r13, 0x00001560},
     "",
     "al r8 r13 0x00001560",
     "al_r8_r13_0x00001560"},
    {{al, r9, r4, 0x00002ac0}, "", "al r9 r4 0x00002ac0", "al_r9_r4_"
                                                          "0x00002ac0"},
    {{al, r9, r7, 0x03fc0000}, "", "al r9 r7 0x03fc0000", "al_r9_r7_"
                                                          "0x03fc0000"},
    {{al, r11, r12, 0x2ac00000},
     "",
     "al r11 r12 0x2ac00000",
     "al_r11_r12_0x2ac00000"},
    {{al, r13, r10, 0x00001fe0},
     "",
     "al r13 r10 0x00001fe0",
     "al_r13_r10_0x00001fe0"},
    {{al, r11, r10, 0x00558000},
     "",
     "al r11 r10 0x00558000",
     "al_r11_r10_0x00558000"},
    {{al, r3, r2, 0x000000ab}, "", "al r3 r2 0x000000ab", "al_r3_r2_"
                                                          "0x000000ab"},
    {{al, r0, r8, 0x00000ab0}, "", "al r0 r8 0x00000ab0", "al_r0_r8_"
                                                          "0x00000ab0"},
    {{al, r9, r7, 0xab000000}, "", "al r9 r7 0xab000000", "al_r9_r7_"
                                                          "0xab000000"},
    {{al, r11, r7, 0x0ff00000},
     "",
     "al r11 r7 0x0ff00000",
     "al_r11_r7_0x0ff00000"},
    {{al, r10, r2, 0x7f800000},
     "",
     "al r10 r2 0x7f800000",
     "al_r10_r2_0x7f800000"},
    {{al, r3, r1, 0x05580000}, "", "al r3 r1 0x05580000", "al_r3_r1_"
                                                          "0x05580000"},
    {{al, r1, r4, 0x0ab00000}, "", "al r1 r4 0x0ab00000", "al_r1_r4_"
                                                          "0x0ab00000"},
    {{al, r4, r9, 0x00005580}, "", "al r4 r9 0x00005580", "al_r4_r9_"
                                                          "0x00005580"},
    {{al, r3, r2, 0x001fe000}, "", "al r3 r2 0x001fe000", "al_r3_r2_"
                                                          "0x001fe000"},
    {{al, r14, r6, 0x00000156},
     "",
     "al r14 r6 0x00000156",
     "al_r14_r6_0x00000156"},
    {{al, r14, r3, 0x00000ab0},
     "",
     "al r14 r3 0x00000ab0",
     "al_r14_r3_0x00000ab0"},
    {{al, r12, r13, 0x000001fe},
     "",
     "al r12 r13 0x000001fe",
     "al_r12_r13_0x000001fe"},
    {{al, r12, r10, 0x1fe00000},
     "",
     "al r12 r10 0x1fe00000",
     "al_r12_r10_0x1fe00000"},
    {{al, r0, r9, 0x2ac00000}, "", "al r0 r9 0x2ac00000", "al_r0_r9_"
                                                          "0x2ac00000"},
    {{al, r11, r6, 0x00000156},
     "",
     "al r11 r6 0x00000156",
     "al_r11_r6_0x00000156"},
    {{al, r2, r4, 0x3fc00000}, "", "al r2 r4 0x3fc00000", "al_r2_r4_"
                                                          "0x3fc00000"},
    {{al, r8, r13, 0x00002ac0},
     "",
     "al r8 r13 0x00002ac0",
     "al_r8_r13_0x00002ac0"},
    {{al, r1, r5, 0x00ff00ff}, "", "al r1 r5 0x00ff00ff", "al_r1_r5_"
                                                          "0x00ff00ff"},
    {{al, r6, r1, 0x0007f800}, "", "al r6 r1 0x0007f800", "al_r6_r1_"
                                                          "0x0007f800"},
    {{al, r5, r1, 0x00001fe0}, "", "al r5 r1 0x00001fe0", "al_r5_r1_"
                                                          "0x00001fe0"},
    {{al, r8, r11, 0xab00ab00},
     "",
     "al r8 r11 0xab00ab00",
     "al_r8_r11_0xab00ab00"},
    {{al, r5, r0, 0xff00ff00}, "", "al r5 r0 0xff00ff00", "al_r5_r0_"
                                                          "0xff00ff00"},
    {{al, r14, r13, 0x000000ab},
     "",
     "al r14 r13 0x000000ab",
     "al_r14_r13_0x000000ab"},
    {{al, r2, r4, 0x05580000}, "", "al r2 r4 0x05580000", "al_r2_r4_"
                                                          "0x05580000"},
    {{al, r14, r10, 0x07f80000},
     "",
     "al r14 r10 0x07f80000",
     "al_r14_r10_0x07f80000"},
    {{al, r10, r3, 0x55800000},
     "",
     "al r10 r3 0x55800000",
     "al_r10_r3_0x55800000"},
    {{al, r0, r11, 0x7f800000},
     "",
     "al r0 r11 0x7f800000",
     "al_r0_r11_0x7f800000"},
    {{al, r3, r12, 0xffffffff},
     "",
     "al r3 r12 0xffffffff",
     "al_r3_r12_0xffffffff"},
    {{al, r2, r3, 0x00000558}, "", "al r2 r3 0x00000558", "al_r2_r3_"
                                                          "0x00000558"},
    {{al, r2, r2, 0x0003fc00}, "", "al r2 r2 0x0003fc00", "al_r2_r2_"
                                                          "0x0003fc00"},
    {{al, r14, r10, 0x15600000},
     "",
     "al r14 r10 0x15600000",
     "al_r14_r10_0x15600000"},
    {{al, r3, r13, 0x00000156},
     "",
     "al r3 r13 0x00000156",
     "al_r3_r13_0x00000156"},
    {{al, r10, r5, 0x1fe00000},
     "",
     "al r10 r5 0x1fe00000",
     "al_r10_r5_0x1fe00000"},
    {{al, r1, r5, 0x00055800}, "", "al r1 r5 0x00055800", "al_r1_r5_"
                                                          "0x00055800"},
    {{al, r8, r6, 0xff000000}, "", "al r8 r6 0xff000000", "al_r8_r6_"
                                                          "0xff000000"},
    {{al, r3, r7, 0x002ac000}, "", "al r3 r7 0x002ac000", "al_r3_r7_"
                                                          "0x002ac000"},
    {{al, r6, r4, 0x00ff00ff}, "", "al r6 r4 0x00ff00ff", "al_r6_r4_"
                                                          "0x00ff00ff"},
    {{al, r0, r8, 0x0007f800}, "", "al r0 r8 0x0007f800", "al_r0_r8_"
                                                          "0x0007f800"},
    {{al, r0, r3, 0xff000000}, "", "al r0 r3 0xff000000", "al_r0_r3_"
                                                          "0xff000000"},
    {{al, r11, r1, 0xabababab},
     "",
     "al r11 r1 0xabababab",
     "al_r11_r1_0xabababab"},
    {{al, r14, r10, 0x000001fe},
     "",
     "al r14 r10 0x000001fe",
     "al_r14_r10_0x000001fe"},
    {{al, r4, r11, 0x002ac000},
     "",
     "al r4 r11 0x002ac000",
     "al_r4_r11_0x002ac000"},
    {{al, r11, r12, 0x000000ab},
     "",
     "al r11 r12 0x000000ab",
     "al_r11_r12_0x000000ab"},
    {{al, r3, r4, 0x003fc000}, "", "al r3 r4 0x003fc000", "al_r3_r4_"
                                                          "0x003fc000"},
    {{al, r3, r13, 0x0ff00000},
     "",
     "al r3 r13 0x0ff00000",
     "al_r3_r13_0x0ff00000"},
    {{al, r5, r4, 0x00001fe0}, "", "al r5 r4 0x00001fe0", "al_r5_r4_"
                                                          "0x00001fe0"},
    {{al, r6, r12, 0x002ac000},
     "",
     "al r6 r12 0x002ac000",
     "al_r6_r12_0x002ac000"},
    {{al, r13, r13, 0x1fe00000},
     "",
     "al r13 r13 0x1fe00000",
     "al_r13_r13_0x1fe00000"},
    {{al, r0, r8, 0x01560000}, "", "al r0 r8 0x01560000", "al_r0_r8_"
                                                          "0x01560000"},
    {{al, r9, r7, 0x00055800}, "", "al r9 r7 0x00055800", "al_r9_r7_"
                                                          "0x00055800"},
    {{al, r6, r0, 0x00000156}, "", "al r6 r0 0x00000156", "al_r6_r0_"
                                                          "0x00000156"},
    {{al, r14, r12, 0x00055800},
     "",
     "al r14 r12 0x00055800",
     "al_r14_r12_0x00055800"},
    {{al, r14, r0, 0xab00ab00},
     "",
     "al r14 r0 0xab00ab00",
     "al_r14_r0_0xab00ab00"},
    {{al, r14, r2, 0x00ab0000},
     "",
     "al r14 r2 0x00ab0000",
     "al_r14_r2_0x00ab0000"},
    {{al, r0, r3, 0x000000ab}, "", "al r0 r3 0x000000ab", "al_r0_r3_"
                                                          "0x000000ab"},
    {{al, r13, r4, 0x003fc000},
     "",
     "al r13 r4 0x003fc000",
     "al_r13_r4_0x003fc000"},
    {{al, r4, r2, 0x00001560}, "", "al r4 r2 0x00001560", "al_r4_r2_"
                                                          "0x00001560"},
    {{al, r14, r4, 0x2ac00000},
     "",
     "al r14 r4 0x2ac00000",
     "al_r14_r4_0x2ac00000"},
    {{al, r4, r11, 0x000003fc},
     "",
     "al r4 r11 0x000003fc",
     "al_r4_r11_0x000003fc"},
    {{al, r6, r8, 0x001fe000}, "", "al r6 r8 0x001fe000", "al_r6_r8_"
                                                          "0x001fe000"},
    {{al, r12, r14, 0x00000558},
     "",
     "al r12 r14 0x00000558",
     "al_r12_r14_0x00000558"},
    {{al, r0, r13, 0x0ff00000},
     "",
     "al r0 r13 0x0ff00000",
     "al_r0_r13_0x0ff00000"},
    {{al, r3, r11, 0xabababab},
     "",
     "al r3 r11 0xabababab",
     "al_r3_r11_0xabababab"},
    {{al, r4, r1, 0x000001fe}, "", "al r4 r1 0x000001fe", "al_r4_r1_"
                                                          "0x000001fe"},
    {{al, r0, r5, 0x000002ac}, "", "al r0 r5 0x000002ac", "al_r0_r5_"
                                                          "0x000002ac"},
    {{al, r8, r5, 0x0003fc00}, "", "al r8 r5 0x0003fc00", "al_r8_r5_"
                                                          "0x0003fc00"},
    {{al, r7, r13, 0x0002ac00},
     "",
     "al r7 r13 0x0002ac00",
     "al_r7_r13_0x0002ac00"},
    {{al, r10, r6, 0x00015600},
     "",
     "al r10 r6 0x00015600",
     "al_r10_r6_0x00015600"},
    {{al, r12, r10, 0x00ff0000},
     "",
     "al r12 r10 0x00ff0000",
     "al_r12_r10_0x00ff0000"},
    {{al, r12, r12, 0x00005580},
     "",
     "al r12 r12 0x00005580",
     "al_r12_r12_0x00005580"},
    {{al, r0, r4, 0x02ac0000}, "", "al r0 r4 0x02ac0000", "al_r0_r4_"
                                                          "0x02ac0000"},
    {{al, r9, r9, 0x02ac0000}, "", "al r9 r9 0x02ac0000", "al_r9_r9_"
                                                          "0x02ac0000"},
    {{al, r7, r4, 0x00000558}, "", "al r7 r4 0x00000558", "al_r7_r4_"
                                                          "0x00000558"},
    {{al, r12, r14, 0x07f80000},
     "",
     "al r12 r14 0x07f80000",
     "al_r12_r14_0x07f80000"},
    {{al, r7, r2, 0xab00ab00}, "", "al r7 r2 0xab00ab00", "al_r7_r2_"
                                                          "0xab00ab00"},
    {{al, r1, r12, 0xff000000},
     "",
     "al r1 r12 0xff000000",
     "al_r1_r12_0xff000000"},
    {{al, r8, r0, 0x7f800000}, "", "al r8 r0 0x7f800000", "al_r8_r0_"
                                                          "0x7f800000"},
    {{al, r7, r0, 0x00000ab0}, "", "al r7 r0 0x00000ab0", "al_r7_r0_"
                                                          "0x00000ab0"},
    {{al, r1, r0, 0x00005580}, "", "al r1 r0 0x00005580", "al_r1_r0_"
                                                          "0x00005580"},
    {{al, r14, r1, 0x001fe000},
     "",
     "al r14 r1 0x001fe000",
     "al_r14_r1_0x001fe000"},
    {{al, r13, r13, 0x0002ac00},
     "",
     "al r13 r13 0x0002ac00",
     "al_r13_r13_0x0002ac00"},
    {{al, r8, r12, 0x0002ac00},
     "",
     "al r8 r12 0x0002ac00",
     "al_r8_r12_0x0002ac00"},
    {{al, r10, r10, 0x00ff00ff},
     "",
     "al r10 r10 0x00ff00ff",
     "al_r10_r10_0x00ff00ff"},
    {{al, r4, r4, 0x002ac000}, "", "al r4 r4 0x002ac000", "al_r4_r4_"
                                                          "0x002ac000"},
    {{al, r12, r5, 0x000ab000},
     "",
     "al r12 r5 0x000ab000",
     "al_r12_r5_0x000ab000"},
    {{al, r1, r2, 0x000003fc}, "", "al r1 r2 0x000003fc", "al_r1_r2_"
                                                          "0x000003fc"},
    {{al, r10, r11, 0x001fe000},
     "",
     "al r10 r11 0x001fe000",
     "al_r10_r11_0x001fe000"},
    {{al, r11, r2, 0x05580000},
     "",
     "al r11 r2 0x05580000",
     "al_r11_r2_0x05580000"},
    {{al, r2, r6, 0x000000ab}, "", "al r2 r6 0x000000ab", "al_r2_r6_"
                                                          "0x000000ab"},
    {{al, r6, r3, 0x0000ff00}, "", "al r6 r3 0x0000ff00", "al_r6_r3_"
                                                          "0x0000ff00"},
    {{al, r13, r0, 0x00156000},
     "",
     "al r13 r0 0x00156000",
     "al_r13_r0_0x00156000"},
    {{al, r2, r9, 0x00002ac0}, "", "al r2 r9 0x00002ac0", "al_r2_r9_"
                                                          "0x00002ac0"},
    {{al, r11, r7, 0x00055800},
     "",
     "al r11 r7 0x00055800",
     "al_r11_r7_0x00055800"},
    {{al, r10, r9, 0x00001fe0},
     "",
     "al r10 r9 0x00001fe0",
     "al_r10_r9_0x00001fe0"},
    {{al, r10, r11, 0x00156000},
     "",
     "al r10 r11 0x00156000",
     "al_r10_r11_0x00156000"},
    {{al, r12, r10, 0xff00ff00},
     "",
     "al r12 r10 0xff00ff00",
     "al_r12_r10_0xff00ff00"},
    {{al, r7, r14, 0x00ab00ab},
     "",
     "al r7 r14 0x00ab00ab",
     "al_r7_r14_0x00ab00ab"},
    {{al, r14, r7, 0x002ac000},
     "",
     "al r14 r7 0x002ac000",
     "al_r14_r7_0x002ac000"},
    {{al, r5, r6, 0x000ff000}, "", "al r5 r6 0x000ff000", "al_r5_r6_"
                                                          "0x000ff000"},
    {{al, r8, r1, 0xff000000}, "", "al r8 r1 0xff000000", "al_r8_r1_"
                                                          "0xff000000"},
    {{al, r8, r0, 0x000002ac}, "", "al r8 r0 0x000002ac", "al_r8_r0_"
                                                          "0x000002ac"},
    {{al, r12, r6, 0x00002ac0},
     "",
     "al r12 r6 0x00002ac0",
     "al_r12_r6_0x00002ac0"},
    {{al, r14, r2, 0x3fc00000},
     "",
     "al r14 r2 0x3fc00000",
     "al_r14_r2_0x3fc00000"},
    {{al, r3, r3, 0x01560000}, "", "al r3 r3 0x01560000", "al_r3_r3_"
                                                          "0x01560000"},
    {{al, r3, r12, 0x0001fe00},
     "",
     "al r3 r12 0x0001fe00",
     "al_r3_r12_0x0001fe00"},
    {{al, r8, r10, 0x000002ac},
     "",
     "al r8 r10 0x000002ac",
     "al_r8_r10_0x000002ac"},
    {{al, r9, r9, 0x002ac000}, "", "al r9 r9 0x002ac000", "al_r9_r9_"
                                                          "0x002ac000"},
    {{al, r0, r6, 0x00156000}, "", "al r0 r6 0x00156000", "al_r0_r6_"
                                                          "0x00156000"},
    {{al, r14, r7, 0x0ff00000},
     "",
     "al r14 r7 0x0ff00000",
     "al_r14_r7_0x0ff00000"},
    {{al, r1, r3, 0x00005580}, "", "al r1 r3 0x00005580", "al_r1_r3_"
                                                          "0x00005580"},
    {{al, r14, r7, 0x000001fe},
     "",
     "al r14 r7 0x000001fe",
     "al_r14_r7_0x000001fe"},
    {{al, r9, r5, 0x03fc0000}, "", "al r9 r5 0x03fc0000", "al_r9_r5_"
                                                          "0x03fc0000"},
    {{al, r7, r14, 0x002ac000},
     "",
     "al r7 r14 0x002ac000",
     "al_r7_r14_0x002ac000"},
    {{al, r8, r9, 0x00000558}, "", "al r8 r9 0x00000558", "al_r8_r9_"
                                                          "0x00000558"},
    {{al, r14, r1, 0x007f8000},
     "",
     "al r14 r1 0x007f8000",
     "al_r14_r1_0x007f8000"},
    {{al, r11, r0, 0xab00ab00},
     "",
     "al r11 r0 0xab00ab00",
     "al_r11_r0_0xab00ab00"},
    {{al, r11, r8, 0x00000156},
     "",
     "al r11 r8 0x00000156",
     "al_r11_r8_0x00000156"},
    {{al, r4, r10, 0x00055800},
     "",
     "al r4 r10 0x00055800",
     "al_r4_r10_0x00055800"},
    {{al, r2, r7, 0x00007f80}, "", "al r2 r7 0x00007f80", "al_r2_r7_"
                                                          "0x00007f80"},
    {{al, r0, r6, 0x00558000}, "", "al r0 r6 0x00558000", "al_r0_r6_"
                                                          "0x00558000"},
    {{al, r4, r2, 0x00558000}, "", "al r4 r2 0x00558000", "al_r4_r2_"
                                                          "0x00558000"},
    {{al, r2, r3, 0x0007f800}, "", "al r2 r3 0x0007f800", "al_r2_r3_"
                                                          "0x0007f800"},
    {{al, r14, r14, 0xab00ab00},
     "",
     "al r14 r14 0xab00ab00",
     "al_r14_r14_0xab00ab00"},
    {{al, r0, r13, 0x000000ff},
     "",
     "al r0 r13 0x000000ff",
     "al_r0_r13_0x000000ff"},
    {{al, r10, r9, 0xab00ab00},
     "",
     "al r10 r9 0xab00ab00",
     "al_r10_r9_0xab00ab00"},
    {{al, r1, r1, 0x3fc00000}, "", "al r1 r1 0x3fc00000", "al_r1_r1_"
                                                          "0x3fc00000"},
    {{al, r8, r6, 0x002ac000}, "", "al r8 r6 0x002ac000", "al_r8_r6_"
                                                          "0x002ac000"},
    {{al, r12, r4, 0x55800000},
     "",
     "al r12 r4 0x55800000",
     "al_r12_r4_0x55800000"},
    {{al, r6, r10, 0x2ac00000},
     "",
     "al r6 r10 0x2ac00000",
     "al_r6_r10_0x2ac00000"},
    {{al, r7, r9, 0x001fe000}, "", "al r7 r9 0x001fe000", "al_r7_r9_"
                                                          "0x001fe000"},
    {{al, r4, r12, 0x00005580},
     "",
     "al r4 r12 0x00005580",
     "al_r4_r12_0x00005580"},
    {{al, r9, r8, 0x0ab00000}, "", "al r9 r8 0x0ab00000", "al_r9_r8_"
                                                          "0x0ab00000"},
    {{al, r2, r4, 0xff00ff00}, "", "al r2 r4 0xff00ff00", "al_r2_r4_"
                                                          "0xff00ff00"},
    {{al, r8, r14, 0x00001fe0},
     "",
     "al r8 r14 0x00001fe0",
     "al_r8_r14_0x00001fe0"},
    {{al, r5, r3, 0x003fc000}, "", "al r5 r3 0x003fc000", "al_r5_r3_"
                                                          "0x003fc000"},
    {{al, r2, r10, 0x00ff00ff},
     "",
     "al r2 r10 0x00ff00ff",
     "al_r2_r10_0x00ff00ff"},
    {{al, r11, r12, 0x15600000},
     "",
     "al r11 r12 0x15600000",
     "al_r11_r12_0x15600000"},
    {{al, r1, r5, 0x00002ac0}, "", "al r1 r5 0x00002ac0", "al_r1_r5_"
                                                          "0x00002ac0"},
    {{al, r3, r7, 0x2ac00000}, "", "al r3 r7 0x2ac00000", "al_r3_r7_"
                                                          "0x2ac00000"},
    {{al, r5, r1, 0xffffffff}, "", "al r5 r1 0xffffffff", "al_r5_r1_"
                                                          "0xffffffff"},
    {{al, r4, r10, 0xff00ff00},
     "",
     "al r4 r10 0xff00ff00",
     "al_r4_r10_0xff00ff00"},
    {{al, r1, r2, 0x00001fe0}, "", "al r1 r2 0x00001fe0", "al_r1_r2_"
                                                          "0x00001fe0"},
    {{al, r5, r14, 0x000000ff},
     "",
     "al r5 r14 0x000000ff",
     "al_r5_r14_0x000000ff"},
    {{al, r14, r0, 0x000ab000},
     "",
     "al r14 r0 0x000ab000",
     "al_r14_r0_0x000ab000"},
    {{al, r10, r3, 0x00ab0000},
     "",
     "al r10 r3 0x00ab0000",
     "al_r10_r3_0x00ab0000"},
    {{al, r10, r12, 0x03fc0000},
     "",
     "al r10 r12 0x03fc0000",
     "al_r10_r12_0x03fc0000"},
    {{al, r8, r11, 0x0007f800},
     "",
     "al r8 r11 0x0007f800",
     "al_r8_r11_0x0007f800"},
    {{al, r9, r13, 0x0001fe00},
     "",
     "al r9 r13 0x0001fe00",
     "al_r9_r13_0x0001fe00"},
    {{al, r12, r13, 0x02ac0000},
     "",
     "al r12 r13 0x02ac0000",
     "al_r12_r13_0x02ac0000"},
    {{al, r3, r9, 0x00ab00ab}, "", "al r3 r9 0x00ab00ab", "al_r3_r9_"
                                                          "0x00ab00ab"},
    {{al, r10, r1, 0x3fc00000},
     "",
     "al r10 r1 0x3fc00000",
     "al_r10_r1_0x3fc00000"},
    {{al, r6, r8, 0x00000558}, "", "al r6 r8 0x00000558", "al_r6_r8_"
                                                          "0x00000558"},
    {{al, r6, r12, 0x0000ab00},
     "",
     "al r6 r12 0x0000ab00",
     "al_r6_r12_0x0000ab00"},
    {{al, r14, r13, 0x000ab000},
     "",
     "al r14 r13 0x000ab000",
     "al_r14_r13_0x000ab000"},
    {{al, r1, r5, 0x1fe00000}, "", "al r1 r5 0x1fe00000", "al_r1_r5_"
                                                          "0x1fe00000"},
    {{al, r11, r3, 0x02ac0000},
     "",
     "al r11 r3 0x02ac0000",
     "al_r11_r3_0x02ac0000"},
    {{al, r9, r5, 0x55800000}, "", "al r9 r5 0x55800000", "al_r9_r5_"
                                                          "0x55800000"},
    {{al, r5, r5, 0x000ab000}, "", "al r5 r5 0x000ab000", "al_r5_r5_"
                                                          "0x000ab000"},
    {{al, r0, r12, 0x003fc000},
     "",
     "al r0 r12 0x003fc000",
     "al_r0_r12_0x003fc000"},
    {{al, r10, r4, 0x0000ab00},
     "",
     "al r10 r4 0x0000ab00",
     "al_r10_r4_0x0000ab00"},
    {{al, r3, r2, 0x0000ff00}, "", "al r3 r2 0x0000ff00", "al_r3_r2_"
                                                          "0x0000ff00"},
    {{al, r14, r8, 0x3fc00000},
     "",
     "al r14 r8 0x3fc00000",
     "al_r14_r8_0x3fc00000"},
    {{al, r10, r13, 0x05580000},
     "",
     "al r10 r13 0x05580000",
     "al_r10_r13_0x05580000"},
    {{al, r4, r13, 0x00156000},
     "",
     "al r4 r13 0x00156000",
     "al_r4_r13_0x00156000"},
    {{al, r7, r2, 0x000002ac}, "", "al r7 r2 0x000002ac", "al_r7_r2_"
                                                          "0x000002ac"},
    {{al, r5, r10, 0x000002ac},
     "",
     "al r5 r10 0x000002ac",
     "al_r5_r10_0x000002ac"},
    {{al, r7, r0, 0xab000000}, "", "al r7 r0 0xab000000", "al_r7_r0_"
                                                          "0xab000000"},
    {{al, r1, r10, 0x000002ac},
     "",
     "al r1 r10 0x000002ac",
     "al_r1_r10_0x000002ac"},
    {{al, r11, r9, 0x00002ac0},
     "",
     "al r11 r9 0x00002ac0",
     "al_r11_r9_0x00002ac0"},
    {{al, r4, r0, 0x000001fe}, "", "al r4 r0 0x000001fe", "al_r4_r0_"
                                                          "0x000001fe"},
    {{al, r11, r9, 0x0003fc00},
     "",
     "al r11 r9 0x0003fc00",
     "al_r11_r9_0x0003fc00"},
    {{al, r8, r3, 0x00005580}, "", "al r8 r3 0x00005580", "al_r8_r3_"
                                                          "0x00005580"},
    {{al, r4, r4, 0xffffffff}, "", "al r4 r4 0xffffffff", "al_r4_r4_"
                                                          "0xffffffff"},
    {{al, r1, r9, 0x00000558}, "", "al r1 r9 0x00000558", "al_r1_r9_"
                                                          "0x00000558"},
    {{al, r9, r2, 0x00ab0000}, "", "al r9 r2 0x00ab0000", "al_r9_r2_"
                                                          "0x00ab0000"},
    {{al, r11, r6, 0x00003fc0},
     "",
     "al r11 r6 0x00003fc0",
     "al_r11_r6_0x00003fc0"},
    {{al, r11, r11, 0x01fe0000},
     "",
     "al r11 r11 0x01fe0000",
     "al_r11_r11_0x01fe0000"},
    {{al, r6, r10, 0x0001fe00},
     "",
     "al r6 r10 0x0001fe00",
     "al_r6_r10_0x0001fe00"},
    {{al, r8, r3, 0x00000156}, "", "al r8 r3 0x00000156", "al_r8_r3_"
                                                          "0x00000156"},
    {{al, r12, r12, 0x0002ac00},
     "",
     "al r12 r12 0x0002ac00",
     "al_r12_r12_0x0002ac00"},
    {{al, r8, r6, 0x7f800000}, "", "al r8 r6 0x7f800000", "al_r8_r6_"
                                                          "0x7f800000"},
    {{al, r5, r13, 0x000002ac},
     "",
     "al r5 r13 0x000002ac",
     "al_r5_r13_0x000002ac"},
    {{al, r5, r13, 0x15600000},
     "",
     "al r5 r13 0x15600000",
     "al_r5_r13_0x15600000"},
    {{al, r8, r8, 0x000000ab}, "", "al r8 r8 0x000000ab", "al_r8_r8_"
                                                          "0x000000ab"},
    {{al, r12, r14, 0x00156000},
     "",
     "al r12 r14 0x00156000",
     "al_r12_r14_0x00156000"},
    {{al, r1, r7, 0x003fc000}, "", "al r1 r7 0x003fc000", "al_r1_r7_"
                                                          "0x003fc000"},
    {{al, r8, r0, 0x00003fc0}, "", "al r8 r0 0x00003fc0", "al_r8_r0_"
                                                          "0x00003fc0"},
    {{al, r14, r11, 0x0007f800},
     "",
     "al r14 r11 0x0007f800",
     "al_r14_r11_0x0007f800"},
    {{al, r3, r8, 0x00ab00ab}, "", "al r3 r8 0x00ab00ab", "al_r3_r8_"
                                                          "0x00ab00ab"},
    {{al, r14, r8, 0x55800000},
     "",
     "al r14 r8 0x55800000",
     "al_r14_r8_0x55800000"},
    {{al, r7, r8, 0x000ff000}, "", "al r7 r8 0x000ff000", "al_r7_r8_"
                                                          "0x000ff000"},
    {{al, r4, r11, 0x01fe0000},
     "",
     "al r4 r11 0x01fe0000",
     "al_r4_r11_0x01fe0000"},
    {{al, r2, r4, 0x01560000}, "", "al r2 r4 0x01560000", "al_r2_r4_"
                                                          "0x01560000"},
    {{al, r4, r3, 0xffffffff}, "", "al r4 r3 0xffffffff", "al_r4_r3_"
                                                          "0xffffffff"},
    {{al, r7, r8, 0xab000000}, "", "al r7 r8 0xab000000", "al_r7_r8_"
                                                          "0xab000000"},
    {{al, r0, r13, 0x00000ab0},
     "",
     "al r0 r13 0x00000ab0",
     "al_r0_r13_0x00000ab0"},
    {{al, r1, r2, 0x000001fe}, "", "al r1 r2 0x000001fe", "al_r1_r2_"
                                                          "0x000001fe"},
    {{al, r8, r14, 0x02ac0000},
     "",
     "al r8 r14 0x02ac0000",
     "al_r8_r14_0x02ac0000"},
    {{al, r4, r5, 0x00558000}, "", "al r4 r5 0x00558000", "al_r4_r5_"
                                                          "0x00558000"},
    {{al, r6, r7, 0xff00ff00}, "", "al r6 r7 0xff00ff00", "al_r6_r7_"
                                                          "0xff00ff00"},
    {{al, r8, r12, 0x001fe000},
     "",
     "al r8 r12 0x001fe000",
     "al_r8_r12_0x001fe000"},
    {{al, r6, r4, 0x07f80000}, "", "al r6 r4 0x07f80000", "al_r6_r4_"
                                                          "0x07f80000"},
    {{al, r4, r0, 0x00001fe0}, "", "al r4 r0 0x00001fe0", "al_r4_r0_"
                                                          "0x00001fe0"},
    {{al, r14, r3, 0xff00ff00},
     "",
     "al r14 r3 0xff00ff00",
     "al_r14_r3_0xff00ff00"},
    {{al, r0, r6, 0xab000000}, "", "al r0 r6 0xab000000", "al_r0_r6_"
                                                          "0xab000000"},
    {{al, r12, r13, 0x00000ab0},
     "",
     "al r12 r13 0x00000ab0",
     "al_r12_r13_0x00000ab0"},
    {{al, r12, r8, 0x00000558},
     "",
     "al r12 r8 0x00000558",
     "al_r12_r8_0x00000558"},
    {{al, r3, r12, 0x0003fc00},
     "",
     "al r3 r12 0x0003fc00",
     "al_r3_r12_0x0003fc00"},
    {{al, r2, r11, 0x7f800000},
     "",
     "al r2 r11 0x7f800000",
     "al_r2_r11_0x7f800000"},
    {{al, r10, r4, 0x15600000},
     "",
     "al r10 r4 0x15600000",
     "al_r10_r4_0x15600000"},
    {{al, r8, r7, 0x0ab00000}, "", "al r8 r7 0x0ab00000", "al_r8_r7_"
                                                          "0x0ab00000"},
    {{al, r10, r6, 0x000000ff},
     "",
     "al r10 r6 0x000000ff",
     "al_r10_r6_0x000000ff"},
    {{al, r3, r4, 0xff00ff00}, "", "al r3 r4 0xff00ff00", "al_r3_r4_"
                                                          "0xff00ff00"},
    {{al, r14, r10, 0x00ab0000},
     "",
     "al r14 r10 0x00ab0000",
     "al_r14_r10_0x00ab0000"},
    {{al, r8, r3, 0x0002ac00}, "", "al r8 r3 0x0002ac00", "al_r8_r3_"
                                                          "0x0002ac00"},
    {{al, r8, r8, 0x00000558}, "", "al r8 r8 0x00000558", "al_r8_r8_"
                                                          "0x00000558"},
    {{al, r12, r4, 0x00015600},
     "",
     "al r12 r4 0x00015600",
     "al_r12_r4_0x00015600"},
    {{al, r8, r1, 0x002ac000}, "", "al r8 r1 0x002ac000", "al_r8_r1_"
                                                          "0x002ac000"},
    {{al, r8, r5, 0x000000ab}, "", "al r8 r5 0x000000ab", "al_r8_r5_"
                                                          "0x000000ab"},
    {{al, r6, r6, 0x000000ab}, "", "al r6 r6 0x000000ab", "al_r6_r6_"
                                                          "0x000000ab"},
    {{al, r5, r7, 0x00002ac0}, "", "al r5 r7 0x00002ac0", "al_r5_r7_"
                                                          "0x00002ac0"},
    {{al, r11, r4, 0x00000ff0},
     "",
     "al r11 r4 0x00000ff0",
     "al_r11_r4_0x00000ff0"},
    {{al, r9, r9, 0x00000ff0}, "", "al r9 r9 0x00000ff0", "al_r9_r9_"
                                                          "0x00000ff0"},
    {{al, r0, r8, 0x00ff0000}, "", "al r0 r8 0x00ff0000", "al_r0_r8_"
                                                          "0x00ff0000"},
    {{al, r9, r11, 0x000000ab},
     "",
     "al r9 r11 0x000000ab",
     "al_r9_r11_0x000000ab"},
    {{al, r7, r5, 0x000000ff}, "", "al r7 r5 0x000000ff", "al_r7_r5_"
                                                          "0x000000ff"},
    {{al, r14, r0, 0x15600000},
     "",
     "al r14 r0 0x15600000",
     "al_r14_r0_0x15600000"},
    {{al, r10, r9, 0x00000156},
     "",
     "al r10 r9 0x00000156",
     "al_r10_r9_0x00000156"},
    {{al, r3, r7, 0x00ff0000}, "", "al r3 r7 0x00ff0000", "al_r3_r7_"
                                                          "0x00ff0000"},
    {{al, r6, r11, 0xab00ab00},
     "",
     "al r6 r11 0xab00ab00",
     "al_r6_r11_0xab00ab00"},
    {{al, r5, r2, 0x002ac000}, "", "al r5 r2 0x002ac000", "al_r5_r2_"
                                                          "0x002ac000"},
    {{al, r9, r14, 0x55800000},
     "",
     "al r9 r14 0x55800000",
     "al_r9_r14_0x55800000"},
    {{al, r10, r13, 0x15600000},
     "",
     "al r10 r13 0x15600000",
     "al_r10_r13_0x15600000"},
    {{al, r13, r7, 0x0ff00000},
     "",
     "al r13 r7 0x0ff00000",
     "al_r13_r7_0x0ff00000"},
    {{al, r12, r5, 0xffffffff},
     "",
     "al r12 r5 0xffffffff",
     "al_r12_r5_0xffffffff"},
    {{al, r8, r10, 0x00000156},
     "",
     "al r8 r10 0x00000156",
     "al_r8_r10_0x00000156"},
    {{al, r7, r6, 0x00005580}, "", "al r7 r6 0x00005580", "al_r7_r6_"
                                                          "0x00005580"},
    {{al, r6, r6, 0x0ab00000}, "", "al r6 r6 0x0ab00000", "al_r6_r6_"
                                                          "0x0ab00000"},
    {{al, r3, r7, 0x01fe0000}, "", "al r3 r7 0x01fe0000", "al_r3_r7_"
                                                          "0x01fe0000"},
    {{al, r14, r9, 0x00558000},
     "",
     "al r14 r9 0x00558000",
     "al_r14_r9_0x00558000"},
    {{al, r3, r13, 0x000007f8},
     "",
     "al r3 r13 0x000007f8",
     "al_r3_r13_0x000007f8"},
    {{al, r10, r2, 0x00055800},
     "",
     "al r10 r2 0x00055800",
     "al_r10_r2_0x00055800"},
    {{al, r5, r14, 0x00005580},
     "",
     "al r5 r14 0x00005580",
     "al_r5_r14_0x00005580"},
    {{al, r9, r12, 0xab000000},
     "",
     "al r9 r12 0xab000000",
     "al_r9_r12_0xab000000"},
    {{al, r2, r14, 0x00000156},
     "",
     "al r2 r14 0x00000156",
     "al_r2_r14_0x00000156"},
    {{al, r6, r10, 0x000ff000},
     "",
     "al r6 r10 0x000ff000",
     "al_r6_r10_0x000ff000"},
    {{al, r6, r7, 0x000007f8}, "", "al r6 r7 0x000007f8", "al_r6_r7_"
                                                          "0x000007f8"},
    {{al, r8, r3, 0x7f800000}, "", "al r8 r3 0x7f800000", "al_r8_r3_"
                                                          "0x7f800000"},
    {{al, r0, r12, 0x15600000},
     "",
     "al r0 r12 0x15600000",
     "al_r0_r12_0x15600000"},
    {{al, r1, r6, 0x00558000}, "", "al r1 r6 0x00558000", "al_r1_r6_"
                                                          "0x00558000"},
    {{al, r3, r8, 0x55800000}, "", "al r3 r8 0x55800000", "al_r3_r8_"
                                                          "0x55800000"},
    {{al, r1, r14, 0x000003fc},
     "",
     "al r1 r14 0x000003fc",
     "al_r1_r14_0x000003fc"},
    {{al, r0, r2, 0x0ab00000}, "", "al r0 r2 0x0ab00000", "al_r0_r2_"
                                                          "0x0ab00000"},
    {{al, r10, r12, 0x00000156},
     "",
     "al r10 r12 0x00000156",
     "al_r10_r12_0x00000156"},
    {{al, r12, r14, 0x03fc0000},
     "",
     "al r12 r14 0x03fc0000",
     "al_r12_r14_0x03fc0000"},
    {{al, r2, r5, 0x0001fe00}, "", "al r2 r5 0x0001fe00", "al_r2_r5_"
                                                          "0x0001fe00"},
    {{al, r5, r11, 0x000ab000},
     "",
     "al r5 r11 0x000ab000",
     "al_r5_r11_0x000ab000"},
    {{al, r14, r14, 0x0001fe00},
     "",
     "al r14 r14 0x0001fe00",
     "al_r14_r14_0x0001fe00"},
    {{al, r13, r2, 0x00003fc0},
     "",
     "al r13 r2 0x00003fc0",
     "al_r13_r2_0x00003fc0"},
    {{al, r0, r8, 0xab000000}, "", "al r0 r8 0xab000000", "al_r0_r8_"
                                                          "0xab000000"},
    {{al, r12, r0, 0x000000ab},
     "",
     "al r12 r0 0x000000ab",
     "al_r12_r0_0x000000ab"},
    {{al, r11, r10, 0x002ac000},
     "",
     "al r11 r10 0x002ac000",
     "al_r11_r10_0x002ac000"},
    {{al, r12, r11, 0x00ab0000},
     "",
     "al r12 r11 0x00ab0000",
     "al_r12_r11_0x00ab0000"},
    {{al, r2, r9, 0x0ff00000}, "", "al r2 r9 0x0ff00000", "al_r2_r9_"
                                                          "0x0ff00000"},
    {{al, r7, r4, 0x000001fe}, "", "al r7 r4 0x000001fe", "al_r7_r4_"
                                                          "0x000001fe"},
    {{al, r7, r6, 0x0000ff00}, "", "al r7 r6 0x0000ff00", "al_r7_r6_"
                                                          "0x0000ff00"},
    {{al, r11, r14, 0x05580000},
     "",
     "al r11 r14 0x05580000",
     "al_r11_r14_0x05580000"},
    {{al, r6, r10, 0x00000558},
     "",
     "al r6 r10 0x00000558",
     "al_r6_r10_0x00000558"},
    {{al, r11, r6, 0x0001fe00},
     "",
     "al r11 r6 0x0001fe00",
     "al_r11_r6_0x0001fe00"},
    {{al, r11, r12, 0xab00ab00},
     "",
     "al r11 r12 0xab00ab00",
     "al_r11_r12_0xab00ab00"},
    {{al, r1, r8, 0x7f800000}, "", "al r1 r8 0x7f800000", "al_r1_r8_"
                                                          "0x7f800000"},
    {{al, r4, r3, 0x0000ff00}, "", "al r4 r3 0x0000ff00", "al_r4_r3_"
                                                          "0x0000ff00"},
    {{al, r5, r4, 0x00ff00ff}, "", "al r5 r4 0x00ff00ff", "al_r5_r4_"
                                                          "0x00ff00ff"},
    {{al, r12, r11, 0x2ac00000},
     "",
     "al r12 r11 0x2ac00000",
     "al_r12_r11_0x2ac00000"},
    {{al, r1, r6, 0xab00ab00}, "", "al r1 r6 0xab00ab00", "al_r1_r6_"
                                                          "0xab00ab00"},
    {{al, r6, r3, 0x000000ab}, "", "al r6 r3 0x000000ab", "al_r6_r3_"
                                                          "0x000000ab"},
    {{al, r2, r11, 0x0007f800},
     "",
     "al r2 r11 0x0007f800",
     "al_r2_r11_0x0007f800"},
    {{al, r3, r0, 0x00001560}, "", "al r3 r0 0x00001560", "al_r3_r0_"
                                                          "0x00001560"},
    {{al, r1, r14, 0x00000558},
     "",
     "al r1 r14 0x00000558",
     "al_r1_r14_0x00000558"},
    {{al, r10, r8, 0x00558000},
     "",
     "al r10 r8 0x00558000",
     "al_r10_r8_0x00558000"},
    {{al, r0, r8, 0x000ff000}, "", "al r0 r8 0x000ff000", "al_r0_r8_"
                                                          "0x000ff000"},
    {{al, r13, r6, 0x007f8000},
     "",
     "al r13 r6 0x007f8000",
     "al_r13_r6_0x007f8000"},
    {{al, r3, r10, 0x000002ac},
     "",
     "al r3 r10 0x000002ac",
     "al_r3_r10_0x000002ac"},
    {{al, r12, r2, 0x0003fc00},
     "",
     "al r12 r2 0x0003fc00",
     "al_r12_r2_0x0003fc00"},
    {{al, r5, r5, 0x02ac0000}, "", "al r5 r5 0x02ac0000", "al_r5_r5_"
                                                          "0x02ac0000"},
    {{al, r11, r12, 0x001fe000},
     "",
     "al r11 r12 0x001fe000",
     "al_r11_r12_0x001fe000"},
    {{al, r0, r14, 0x001fe000},
     "",
     "al r0 r14 0x001fe000",
     "al_r0_r14_0x001fe000"},
    {{al, r0, r14, 0x02ac0000},
     "",
     "al r0 r14 0x02ac0000",
     "al_r0_r14_0x02ac0000"},
    {{al, r6, r7, 0x0ff00000}, "", "al r6 r7 0x0ff00000", "al_r6_r7_"
                                                          "0x0ff00000"},
    {{al, r10, r13, 0x00000156},
     "",
     "al r10 r13 0x00000156",
     "al_r10_r13_0x00000156"},
    {{al, r3, r7, 0x000007f8}, "", "al r3 r7 0x000007f8", "al_r3_r7_"
                                                          "0x000007f8"},
    {{al, r4, r10, 0x000000ab},
     "",
     "al r4 r10 0x000000ab",
     "al_r4_r10_0x000000ab"},
    {{al, r0, r6, 0x00000558}, "", "al r0 r6 0x00000558", "al_r0_r6_"
                                                          "0x00000558"},
    {{al, r1, r1, 0x05580000}, "", "al r1 r1 0x05580000", "al_r1_r1_"
                                                          "0x05580000"},
    {{al, r8, r2, 0x00001560}, "", "al r8 r2 0x00001560", "al_r8_r2_"
                                                          "0x00001560"},
    {{al, r9, r5, 0x0001fe00}, "", "al r9 r5 0x0001fe00", "al_r9_r5_"
                                                          "0x0001fe00"},
    {{al, r13, r9, 0x0ab00000},
     "",
     "al r13 r9 0x0ab00000",
     "al_r13_r9_0x0ab00000"},
    {{al, r13, r9, 0x00007f80},
     "",
     "al r13 r9 0x00007f80",
     "al_r13_r9_0x00007f80"},
    {{al, r10, r5, 0x0000ab00},
     "",
     "al r10 r5 0x0000ab00",
     "al_r10_r5_0x0000ab00"},
    {{al, r6, r13, 0x007f8000},
     "",
     "al r6 r13 0x007f8000",
     "al_r6_r13_0x007f8000"},
    {{al, r5, r9, 0x000ab000}, "", "al r5 r9 0x000ab000", "al_r5_r9_"
                                                          "0x000ab000"},
    {{al, r4, r4, 0x000000ab}, "", "al r4 r4 0x000000ab", "al_r4_r4_"
                                                          "0x000000ab"},
    {{al, r13, r5, 0xab00ab00},
     "",
     "al r13 r5 0xab00ab00",
     "al_r13_r5_0xab00ab00"},
    {{al, r12, r3, 0x00005580},
     "",
     "al r12 r3 0x00005580",
     "al_r12_r3_0x00005580"},
    {{al, r0, r10, 0x55800000},
     "",
     "al r0 r10 0x55800000",
     "al_r0_r10_0x55800000"},
    {{al, r2, r8, 0x00ab00ab}, "", "al r2 r8 0x00ab00ab", "al_r2_r8_"
                                                          "0x00ab00ab"},
    {{al, r11, r5, 0x0003fc00},
     "",
     "al r11 r5 0x0003fc00",
     "al_r11_r5_0x0003fc00"},
    {{al, r11, r0, 0x00ab0000},
     "",
     "al r11 r0 0x00ab0000",
     "al_r11_r0_0x00ab0000"},
    {{al, r10, r2, 0x000002ac},
     "",
     "al r10 r2 0x000002ac",
     "al_r10_r2_0x000002ac"},
    {{al, r11, r12, 0x00055800},
     "",
     "al r11 r12 0x00055800",
     "al_r11_r12_0x00055800"},
    {{al, r5, r13, 0x00000ff0},
     "",
     "al r5 r13 0x00000ff0",
     "al_r5_r13_0x00000ff0"},
    {{al, r4, r14, 0x15600000},
     "",
     "al r4 r14 0x15600000",
     "al_r4_r14_0x15600000"},
    {{al, r10, r1, 0x00003fc0},
     "",
     "al r10 r1 0x00003fc0",
     "al_r10_r1_0x00003fc0"},
    {{al, r14, r8, 0xff000000},
     "",
     "al r14 r8 0xff000000",
     "al_r14_r8_0xff000000"},
    {{al, r12, r0, 0x00ff0000},
     "",
     "al r12 r0 0x00ff0000",
     "al_r12_r0_0x00ff0000"},
    {{al, r4, r5, 0x3fc00000}, "", "al r4 r5 0x3fc00000", "al_r4_r5_"
                                                          "0x3fc00000"},
    {{al, r14, r10, 0x3fc00000},
     "",
     "al r14 r10 0x3fc00000",
     "al_r14_r10_0x3fc00000"},
    {{al, r10, r1, 0x00015600},
     "",
     "al r10 r1 0x00015600",
     "al_r10_r1_0x00015600"},
    {{al, r4, r3, 0xff000000}, "", "al r4 r3 0xff000000", "al_r4_r3_"
                                                          "0xff000000"},
    {{al, r10, r10, 0x02ac0000},
     "",
     "al r10 r10 0x02ac0000",
     "al_r10_r10_0x02ac0000"},
    {{al, r9, r9, 0x000ff000}, "", "al r9 r9 0x000ff000", "al_r9_r9_"
                                                          "0x000ff000"},
    {{al, r13, r7, 0x0002ac00},
     "",
     "al r13 r7 0x0002ac00",
     "al_r13_r7_0x0002ac00"},
    {{al, r7, r8, 0x00001fe0}, "", "al r7 r8 0x00001fe0", "al_r7_r8_"
                                                          "0x00001fe0"},
    {{al, r2, r4, 0x00001560}, "", "al r2 r4 0x00001560", "al_r2_r4_"
                                                          "0x00001560"},
    {{al, r13, r7, 0x00156000},
     "",
     "al r13 r7 0x00156000",
     "al_r13_r7_0x00156000"},
    {{al, r9, r9, 0x000003fc}, "", "al r9 r9 0x000003fc", "al_r9_r9_"
                                                          "0x000003fc"},
    {{al, r0, r3, 0x000ab000}, "", "al r0 r3 0x000ab000", "al_r0_r3_"
                                                          "0x000ab000"},
    {{al, r10, r12, 0x0000ab00},
     "",
     "al r10 r12 0x0000ab00",
     "al_r10_r12_0x0000ab00"},
    {{al, r1, r13, 0x00002ac0},
     "",
     "al r1 r13 0x00002ac0",
     "al_r1_r13_0x00002ac0"},
    {{al, r3, r10, 0x001fe000},
     "",
     "al r3 r10 0x001fe000",
     "al_r3_r10_0x001fe000"},
    {{al, r4, r12, 0x00ff00ff},
     "",
     "al r4 r12 0x00ff00ff",
     "al_r4_r12_0x00ff00ff"},
    {{al, r12, r5, 0x003fc000},
     "",
     "al r12 r5 0x003fc000",
     "al_r12_r5_0x003fc000"},
    {{al, r11, r2, 0x0001fe00},
     "",
     "al r11 r2 0x0001fe00",
     "al_r11_r2_0x0001fe00"},
    {{al, r8, r6, 0x0007f800}, "", "al r8 r6 0x0007f800", "al_r8_r6_"
                                                          "0x0007f800"},
    {{al, r11, r1, 0x000000ff},
     "",
     "al r11 r1 0x000000ff",
     "al_r11_r1_0x000000ff"},
    {{al, r5, r2, 0x007f8000}, "", "al r5 r2 0x007f8000", "al_r5_r2_"
                                                          "0x007f8000"},
    {{al, r8, r10, 0xab000000},
     "",
     "al r8 r10 0xab000000",
     "al_r8_r10_0xab000000"},
    {{al, r10, r3, 0x000ff000},
     "",
     "al r10 r3 0x000ff000",
     "al_r10_r3_0x000ff000"},
    {{al, r6, r0, 0x00ff0000}, "", "al r6 r0 0x00ff0000", "al_r6_r0_"
                                                          "0x00ff0000"},
    {{al, r7, r14, 0x0ff00000},
     "",
     "al r7 r14 0x0ff00000",
     "al_r7_r14_0x0ff00000"},
    {{al, r8, r3, 0x00001560}, "", "al r8 r3 0x00001560", "al_r8_r3_"
                                                          "0x00001560"},
    {{al, r13, r9, 0x00000558},
     "",
     "al r13 r9 0x00000558",
     "al_r13_r9_0x00000558"},
    {{al, r8, r7, 0x00001fe0}, "", "al r8 r7 0x00001fe0", "al_r8_r7_"
                                                          "0x00001fe0"},
    {{al, r13, r3, 0x0003fc00},
     "",
     "al r13 r3 0x0003fc00",
     "al_r13_r3_0x0003fc00"},
    {{al, r4, r14, 0x000000ab},
     "",
     "al r4 r14 0x000000ab",
     "al_r4_r14_0x000000ab"},
    {{al, r14, r7, 0x000000ab},
     "",
     "al r14 r7 0x000000ab",
     "al_r14_r7_0x000000ab"},
    {{al, r11, r9, 0x00558000},
     "",
     "al r11 r9 0x00558000",
     "al_r11_r9_0x00558000"},
    {{al, r3, r10, 0x0000ff00},
     "",
     "al r3 r10 0x0000ff00",
     "al_r3_r10_0x0000ff00"},
    {{al, r4, r12, 0x003fc000},
     "",
     "al r4 r12 0x003fc000",
     "al_r4_r12_0x003fc000"},
    {{al, r11, r1, 0x002ac000},
     "",
     "al r11 r1 0x002ac000",
     "al_r11_r1_0x002ac000"},
    {{al, r12, r0, 0x7f800000},
     "",
     "al r12 r0 0x7f800000",
     "al_r12_r0_0x7f800000"},
    {{al, r3, r9, 0x00003fc0}, "", "al r3 r9 0x00003fc0", "al_r3_r9_"
                                                          "0x00003fc0"},
    {{al, r6, r6, 0x0ff00000}, "", "al r6 r6 0x0ff00000", "al_r6_r6_"
                                                          "0x0ff00000"},
    {{al, r1, r11, 0xff000000},
     "",
     "al r1 r11 0xff000000",
     "al_r1_r11_0xff000000"},
    {{al, r2, r10, 0x0007f800},
     "",
     "al r2 r10 0x0007f800",
     "al_r2_r10_0x0007f800"},
    {{al, r12, r10, 0x000002ac},
     "",
     "al r12 r10 0x000002ac",
     "al_r12_r10_0x000002ac"},
    {{al, r10, r8, 0x000003fc},
     "",
     "al r10 r8 0x000003fc",
     "al_r10_r8_0x000003fc"},
    {{al, r9, r0, 0x55800000}, "", "al r9 r0 0x55800000", "al_r9_r0_"
                                                          "0x55800000"},
    {{al, r8, r7, 0x1fe00000}, "", "al r8 r7 0x1fe00000", "al_r8_r7_"
                                                          "0x1fe00000"},
    {{al, r4, r0, 0x15600000}, "", "al r4 r0 0x15600000", "al_r4_r0_"
                                                          "0x15600000"},
    {{al, r4, r0, 0xff00ff00}, "", "al r4 r0 0xff00ff00", "al_r4_r0_"
                                                          "0xff00ff00"},
    {{al, r1, r14, 0x00007f80},
     "",
     "al r1 r14 0x00007f80",
     "al_r1_r14_0x00007f80"},
    {{al, r7, r3, 0x00ff00ff}, "", "al r7 r3 0x00ff00ff", "al_r7_r3_"
                                                          "0x00ff00ff"},
    {{al, r10, r2, 0x00001560},
     "",
     "al r10 r2 0x00001560",
     "al_r10_r2_0x00001560"},
    {{al, r0, r14, 0xabababab},
     "",
     "al r0 r14 0xabababab",
     "al_r0_r14_0xabababab"},
    {{al, r3, r4, 0x007f8000}, "", "al r3 r4 0x007f8000", "al_r3_r4_"
                                                          "0x007f8000"},
    {{al, r0, r2, 0x003fc000}, "", "al r0 r2 0x003fc000", "al_r0_r2_"
                                                          "0x003fc000"},
    {{al, r13, r6, 0x0002ac00},
     "",
     "al r13 r6 0x0002ac00",
     "al_r13_r6_0x0002ac00"},
    {{al, r11, r5, 0x00001fe0},
     "",
     "al r11 r5 0x00001fe0",
     "al_r11_r5_0x00001fe0"},
    {{al, r1, r13, 0x00005580},
     "",
     "al r1 r13 0x00005580",
     "al_r1_r13_0x00005580"},
    {{al, r13, r8, 0x000007f8},
     "",
     "al r13 r8 0x000007f8",
     "al_r13_r8_0x000007f8"},
    {{al, r6, r4, 0x0ab00000}, "", "al r6 r4 0x0ab00000", "al_r6_r4_"
                                                          "0x0ab00000"},
    {{al, r14, r10, 0x1fe00000},
     "",
     "al r14 r10 0x1fe00000",
     "al_r14_r10_0x1fe00000"},
    {{al, r7, r6, 0xff00ff00}, "", "al r7 r6 0xff00ff00", "al_r7_r6_"
                                                          "0xff00ff00"},
    {{al, r11, r5, 0xffffffff},
     "",
     "al r11 r5 0xffffffff",
     "al_r11_r5_0xffffffff"},
    {{al, r0, r12, 0xffffffff},
     "",
     "al r0 r12 0xffffffff",
     "al_r0_r12_0xffffffff"},
    {{al, r12, r2, 0x15600000},
     "",
     "al r12 r2 0x15600000",
     "al_r12_r2_0x15600000"},
    {{al, r3, r12, 0x000ff000},
     "",
     "al r3 r12 0x000ff000",
     "al_r3_r12_0x000ff000"},
    {{al, r6, r8, 0x00055800}, "", "al r6 r8 0x00055800", "al_r6_r8_"
                                                          "0x00055800"},
    {{al, r12, r7, 0x05580000},
     "",
     "al r12 r7 0x05580000",
     "al_r12_r7_0x05580000"},
    {{al, r8, r5, 0x007f8000}, "", "al r8 r5 0x007f8000", "al_r8_r5_"
                                                          "0x007f8000"},
    {{al, r4, r1, 0x000ab000}, "", "al r4 r1 0x000ab000", "al_r4_r1_"
                                                          "0x000ab000"},
    {{al, r13, r12, 0x02ac0000},
     "",
     "al r13 r12 0x02ac0000",
     "al_r13_r12_0x02ac0000"},
    {{al, r9, r8, 0x000000ff}, "", "al r9 r8 0x000000ff", "al_r9_r8_"
                                                          "0x000000ff"},
    {{al, r1, r11, 0x00005580},
     "",
     "al r1 r11 0x00005580",
     "al_r1_r11_0x00005580"},
    {{al, r10, r12, 0x02ac0000},
     "",
     "al r10 r12 0x02ac0000",
     "al_r10_r12_0x02ac0000"},
    {{al, r7, r9, 0x00ab00ab}, "", "al r7 r9 0x00ab00ab", "al_r7_r9_"
                                                          "0x00ab00ab"},
    {{al, r0, r5, 0x0000ab00}, "", "al r0 r5 0x0000ab00", "al_r0_r5_"
                                                          "0x0000ab00"},
    {{al, r13, r9, 0x00558000},
     "",
     "al r13 r9 0x00558000",
     "al_r13_r9_0x00558000"},
    {{al, r0, r1, 0x002ac000}, "", "al r0 r1 0x002ac000", "al_r0_r1_"
                                                          "0x002ac000"},
    {{al, r14, r1, 0x00000ab0},
     "",
     "al r14 r1 0x00000ab0",
     "al_r14_r1_0x00000ab0"},
    {{al, r2, r2, 0x00000558}, "", "al r2 r2 0x00000558", "al_r2_r2_"
                                                          "0x00000558"},
    {{al, r10, r13, 0x00ab00ab},
     "",
     "al r10 r13 0x00ab00ab",
     "al_r10_r13_0x00ab00ab"},
    {{al, r4, r6, 0x00001560}, "", "al r4 r6 0x00001560", "al_r4_r6_"
                                                          "0x00001560"},
    {{al, r10, r0, 0x00156000},
     "",
     "al r10 r0 0x00156000",
     "al_r10_r0_0x00156000"},
    {{al, r10, r13, 0x00156000},
     "",
     "al r10 r13 0x00156000",
     "al_r10_r13_0x00156000"},
    {{al, r11, r2, 0x001fe000},
     "",
     "al r11 r2 0x001fe000",
     "al_r11_r2_0x001fe000"},
    {{al, r4, r5, 0x2ac00000}, "", "al r4 r5 0x2ac00000", "al_r4_r5_"
                                                          "0x2ac00000"},
    {{al, r8, r8, 0x02ac0000}, "", "al r8 r8 0x02ac0000", "al_r8_r8_"
                                                          "0x02ac0000"},
    {{al, r9, r1, 0x7f800000}, "", "al r9 r1 0x7f800000", "al_r9_r1_"
                                                          "0x7f800000"},
    {{al, r8, r9, 0xff00ff00}, "", "al r8 r9 0xff00ff00", "al_r8_r9_"
                                                          "0xff00ff00"},
    {{al, r12, r7, 0x00ff00ff},
     "",
     "al r12 r7 0x00ff00ff",
     "al_r12_r7_0x00ff00ff"},
    {{al, r9, r10, 0x00156000},
     "",
     "al r9 r10 0x00156000",
     "al_r9_r10_0x00156000"}};

// These headers each contain an array of `TestResult` with the reference output
// values. The reference arrays are names `kReference{mnemonic}`.
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-adc.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-adcs.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-add.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-adds.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-and.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-ands.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-bic.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-bics.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-eor.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-eors.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-orn.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-orns.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-orr.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-orrs.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-rsb.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-rsbs.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-sbc.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-sbcs.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-sub.h"
#include "aarch32/traces/assembler-cond-rd-rn-operand-const-t32-subs.h"


// The maximum number of errors to report in detail for each test.
static const unsigned kErrorReportLimit = 8;

typedef void (MacroAssembler::*Fn)(Condition cond,
                                   Register rd,
                                   Register rn,
                                   const Operand& op);

static void TestHelper(Fn instruction,
                       const char* mnemonic,
                       const TestResult reference[]) {
  unsigned total_error_count = 0;
  MacroAssembler masm(BUF_SIZE);

  masm.UseT32();

  for (unsigned i = 0; i < ARRAY_SIZE(kTests); i++) {
    // Values to pass to the macro-assembler.
    Condition cond = kTests[i].operands.cond;
    Register rd = kTests[i].operands.rd;
    Register rn = kTests[i].operands.rn;
    uint32_t immediate = kTests[i].operands.immediate;
    Operand op(immediate);

    uint32_t start = masm.GetCursorOffset();
    (masm.*instruction)(cond, rd, rn, op);
    uint32_t end = masm.GetCursorOffset();

    const byte* result_ptr =
        masm.GetBuffer().GetOffsetAddress<const byte*>(start);
    uint32_t result_size = end - start;

    if (Test::generate_test_trace()) {
      // Print the result bytes.
      printf("static const byte kInstruction_%s_%s[] = {\n",
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
      printf(" // %s %s %s\n};\n",
             kTests[i].expect_instruction_before,
             mnemonic,
             kTests[i].operands_description);
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
    printf("static const TestResult kReference%s[] = {\n", mnemonic);
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
  }
}

// Instantiate tests for each instruction in the list.
#define TEST(mnemonic)                                                      \
  static void Test_##mnemonic() {                                           \
    TestHelper(&MacroAssembler::mnemonic, #mnemonic, kReference##mnemonic); \
  }                                                                         \
  static Test test_##mnemonic(                                              \
      "AARCH32_ASSEMBLER_COND_RD_RN_OPERAND_CONST_T32_" #mnemonic,          \
      &Test_##mnemonic);
FOREACH_INSTRUCTION(TEST)
#undef TEST

}  // aarch32
}  // vixl
