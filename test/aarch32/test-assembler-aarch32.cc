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

#include <cstdio>
#include <string>
#include <iostream>

#include "test-runner.h"
#include "test-utils.h"
#include "aarch32/test-utils-aarch32.h"

#include "aarch32/macro-assembler-aarch32.h"
#include "aarch32/disasm-aarch32.h"

namespace vixl {
namespace aarch32 {

#define STRINGIFY(x) #x

// Tests declared with this macro will be run twice: once targeting A32 and
// once targeting T32.
#define TEST(Name)                                                \
void Test##Name##Impl(InstructionSet isa);                        \
void Test##Name() {                                               \
  Test##Name##Impl(A32);                                          \
  printf(" > A32 done\n");                                        \
  Test##Name##Impl(T32);                                          \
  printf(" > T32 done\n");                                        \
}                                                                 \
Test test_##Name(STRINGIFY(AARCH32_ASM_##Name), &Test##Name);     \
void Test##Name##Impl(InstructionSet isa __attribute__((unused)))

// Test declared with this macro will only target A32.
#define TEST_A32(Name)                                            \
void Test##Name##Impl(InstructionSet isa);                        \
void Test##Name() {                                               \
  Test##Name##Impl(A32);                                          \
}                                                                 \
Test test_##Name(STRINGIFY(AARCH32_A32_##Name), &Test##Name);     \
void Test##Name##Impl(InstructionSet isa __attribute__((unused)))

// Tests declared with this macro will only target T32.
#define TEST_T32(Name)                                            \
void Test##Name##Impl(InstructionSet isa);                        \
void Test##Name() {                                               \
  Test##Name##Impl(T32);                                          \
}                                                                 \
Test test_##Name(STRINGIFY(AARCH32_T32_##Name), &Test##Name);     \
void Test##Name##Impl(InstructionSet isa __attribute__((unused)))

// Tests declared with this macro are not expected to use any provided test
// helpers such as SETUP, RUN, etc.
#define TEST_NOASM(Name)                                   \
void Test##Name();                                         \
Test test_##Name(STRINGIFY(AARCH32_##Name), &Test##Name);  \
void Test##Name()

#define __ masm.
#define BUF_SIZE (4096)

#define ASSERT_LITERAL_POOL_SIZE(size) \
    do { VIXL_CHECK(__ GetLiteralPoolSize() == size); } while (false)

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH32
// No simulator yet.

#define SETUP()                                             \
  MacroAssembler masm(BUF_SIZE, isa);                       \

#define START() \
  masm.GetBuffer()->Reset();

#define END()                                                                  \
  __ Hlt(0);                                                                   \
  __ FinalizeCode();

#define RUN()                                                                  \
  DISASSEMBLE();

#define TEARDOWN()

#else  // ifdef VIXL_INCLUDE_SIMULATOR_AARCH32.

#define SETUP()                                                                \
  RegisterDump core;                                                           \
  MacroAssembler masm(BUF_SIZE, isa);                                          \

#define START()                                                                \
  masm.GetBuffer()->Reset();                                                   \
  __ Push(r4);                                                                 \
  __ Push(r5);                                                                 \
  __ Push(r6);                                                                 \
  __ Push(r7);                                                                 \
  __ Push(r8);                                                                 \
  __ Push(r9);                                                                 \
  __ Push(r10);                                                                \
  __ Push(r11);                                                                \
  __ Push(r12);                                                                \
  __ Push(lr);                                                                 \
  __ Mov(r0, 0);                                                               \
  __ Msr(APSR_nzcvq, r0);

#define END()                                                                  \
  core.Dump(&masm);                                                            \
  __ Pop(lr);                                                                  \
  __ Pop(r12);                                                                 \
  __ Pop(r11);                                                                 \
  __ Pop(r10);                                                                 \
  __ Pop(r9);                                                                  \
  __ Pop(r8);                                                                  \
  __ Pop(r7);                                                                  \
  __ Pop(r6);                                                                  \
  __ Pop(r5);                                                                  \
  __ Pop(r4);                                                                  \
  __ Bx(lr);                                                                   \
  __ FinalizeCode();

// Execute the generated code from the MacroAssembler's automatic code buffer.
// Note the offset for ExecuteMemory since the PCS requires that
// the address be odd in the case of branching to T32 code.
#define RUN()                                                                  \
  DISASSEMBLE();                                                               \
  {                                                                            \
    int pcs_offset = masm.IsUsingT32() ? 1 : 0;                                \
    masm.GetBuffer()->SetExecutable();                                         \
    ExecuteMemory(masm.GetBuffer()->GetStartAddress<byte*>(),                  \
                  masm.GetSizeOfCodeGenerated(),                               \
                  pcs_offset);                                                 \
    masm.GetBuffer()->SetWritable();                                           \
  }

#define TEARDOWN()

#endif  // ifdef VIXL_INCLUDE_SIMULATOR_AARCH32

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH32
// No simulator yet. We can't test the results.

#define ASSERT_EQUAL_32(expected, result)

#define ASSERT_EQUAL_64(expected, result)

#define ASSERT_EQUAL_128(expected_h, expected_l, result)

#define ASSERT_EQUAL_FP32(expected, result)

#define ASSERT_EQUAL_FP64(expected, result)

#define ASSERT_EQUAL_NZCV(expected)

#else

#define ASSERT_EQUAL_32(expected, result)                                      \
  VIXL_CHECK(Equal32(expected, &core, result))

#define ASSERT_EQUAL_64(expected, result)                                      \
  VIXL_CHECK(Equal64(expected, &core, result))

#define ASSERT_EQUAL_128(expected_h, expected_l, result)                       \
  VIXL_CHECK(Equal128(expected_h, expected_l, &core, result))

#define ASSERT_EQUAL_FP32(expected, result)                                    \
  VIXL_CHECK(EqualFP32(expected, &core, result))

#define ASSERT_EQUAL_FP64(expected, result)                                    \
  VIXL_CHECK(EqualFP64(expected, &core, result))

#define ASSERT_EQUAL_NZCV(expected)                                            \
  VIXL_CHECK(EqualNzcv(expected, core.flags_nzcv()))

#endif

#define DISASSEMBLE() \
  if (Test::disassemble()) {                                                   \
    PrintDisassembler dis(std::cout, 0);                                       \
    if (masm.IsUsingT32()) {                                                   \
      dis.DisassembleT32Buffer(masm.GetBuffer()->GetStartAddress<uint16_t*>(), \
                               masm.GetCursorOffset());                        \
    } else {                                                                   \
      dis.DisassembleA32Buffer(masm.GetBuffer()->GetStartAddress<uint32_t*>(), \
                               masm.GetCursorOffset());                        \
    }                                                                          \
  }

// TODO: Add SBC to the ADC tests.


TEST(adc_shift) {
  SETUP();

  START();
  // Initialize registers.
  __ Mov(r0, 0);
  __ Mov(r1, 1);
  __ Mov(r2, 0x01234567);
  __ Mov(r3, 0xfedcba98);

  // Clear the C flag.
  __ Adds(r0, r0, 0);

  __ Adc(r4, r2, r3);
  __ Adc(r5, r0, Operand(r1, LSL, 30));
  __ Adc(r6, r0, Operand(r2, LSR, 16));
  __ Adc(r7, r2, Operand(r3, ASR, 4));
  __ Adc(r8, r2, Operand(r3, ROR, 8));
  __ Adc(r9, r2, Operand(r3, RRX));
  END();

  RUN();

  ASSERT_EQUAL_32(0xffffffff, r4);
  ASSERT_EQUAL_32(INT32_C(1) << 30, r5);
  ASSERT_EQUAL_32(0x00000123, r6);
  ASSERT_EQUAL_32(0x01111110, r7);
  ASSERT_EQUAL_32(0x9a222221, r8);
  ASSERT_EQUAL_32(0x8091a2b3, r9);

  START();
  // Initialize registers.
  __ Mov(r0, 0);
  __ Mov(r1, 1);
  __ Mov(r2, 0x01234567);
  __ Mov(r3, 0xfedcba98);
  __ Mov(r4, 0xffffffff);

  // Set the C flag.
  __ Adds(r0, r4, r1);

  __ Adc(r5, r2, r3);
  __ Adc(r6, r0, Operand(r1, LSL, 30));
  __ Adc(r7, r0, Operand(r2, LSR, 16));
  __ Adc(r8, r2, Operand(r3, ASR, 4));
  __ Adc(r9, r2, Operand(r3, ROR, 8));
  __ Adc(r10, r2, Operand(r3, RRX));
  END();

  RUN();

  ASSERT_EQUAL_32(0xffffffff + 1, r5);
  ASSERT_EQUAL_32((INT32_C(1) << 30) + 1, r6);
  ASSERT_EQUAL_32(0x00000123 + 1, r7);
  ASSERT_EQUAL_32(0x01111110 + 1, r8);
  ASSERT_EQUAL_32(0x9a222221 + 1, r9);
  ASSERT_EQUAL_32(0x0091a2b3 + 1, r10);

  // Check that adc correctly sets the condition flags.
  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0xffffffff);
  __ Mov(r2, 1);

  // Clear the C flag.
  __ Adds(r0, r0, 0);
  __ Adcs(r3, r2, r1);
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);
  ASSERT_EQUAL_32(0, r3);

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0x80000000);
  __ Mov(r2, 1);

  // Clear the C flag.
  __ Adds(r0, r0, 0);
  __ Adcs(r3, r2, Operand(r1, ASR, 31));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);
  ASSERT_EQUAL_32(0, r3);

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0x80000000);
  __ Mov(r2, 0xffffffff);

  // Clear the C flag.
  __ Adds(r0, r0, 0);
  __ Adcs(r3, r2, Operand(r1, LSR, 31));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);
  ASSERT_EQUAL_32(0, r3);

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0x07ffffff);
  __ Mov(r2, 0x10);

  // Clear the C flag.
  __ Adds(r0, r0, 0);
  __ Adcs(r3, r2, Operand(r1, LSL, 4));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NVFlag);
  ASSERT_EQUAL_32(0x080000000, r3);

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0xffffff00);
  __ Mov(r2, 0xff000001);

  // Clear the C flag.
  __ Adds(r0, r0, 0);
  __ Adcs(r3, r2, Operand(r1, ROR, 8));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);
  ASSERT_EQUAL_32(0, r3);

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0xffffffff);
  __ Mov(r2, 0x1);

  // Clear the C flag, forcing RRX to insert 0 in r1's most significant bit.
  __ Adds(r0, r0, 0);
  __ Adcs(r3, r2, Operand(r1, RRX));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NVFlag);
  ASSERT_EQUAL_32(0x80000000, r3);

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0xffffffff);
  __ Mov(r2, 0x1);

  // Set the C flag, forcing RRX to insert 1 in r1's most significant bit.
  __ Adds(r0, r1, r2);
  __ Adcs(r3, r2, Operand(r1, RRX));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(CFlag);
  ASSERT_EQUAL_32(1, r3);

  TEARDOWN();
}


TEST(adc_wide_imm) {
  SETUP();

  START();
  __ Mov(r0, 0);

  // Clear the C flag.
  __ Adds(r0, r0, 0);

  __ Adc(r1, r0, 0x12345678);
  __ Adc(r2, r0, 0xffffffff);

  // Set the C flag.
  __ Cmp(r0, r0);

  __ Adc(r3, r0, 0x12345678);
  __ Adc(r4, r0, 0xffffffff);
  END();

  RUN();

  ASSERT_EQUAL_32(0x12345678, r1);
  ASSERT_EQUAL_32(0xffffffff, r2);
  ASSERT_EQUAL_32(0x12345678 + 1, r3);
  ASSERT_EQUAL_32(0, r4);

  TEARDOWN();
}


// TODO: Add SUB tests to the ADD tests.


TEST(add_imm) {
  SETUP();

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0x1111);
  __ Mov(r2, 0xffffffff);
  __ Mov(r3, 0x80000000);

  __ Add(r4, r0, 0x12);
  __ Add(r5, r1, 0x120000);
  __ Add(r6, r0, 0xab << 12);
  __ Add(r7, r2, 1);

  END();

  RUN();

  ASSERT_EQUAL_32(0x12, r4);
  ASSERT_EQUAL_32(0x121111, r5);
  ASSERT_EQUAL_32(0xab000, r6);
  ASSERT_EQUAL_32(0x0, r7);

  TEARDOWN();
}


TEST(add_wide_imm) {
  SETUP();

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 1);

  __ Add(r2, r0, 0x12345678);
  __ Add(r3, r1, 0xffff);
  END();

  RUN();

  ASSERT_EQUAL_32(0x12345678, r2);
  ASSERT_EQUAL_32(0x00010000, r3);

  TEARDOWN();
}


TEST(add_shifted) {
  SETUP();

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0x01234567);
  __ Mov(r2, 0x76543210);
  __ Mov(r3, 0xffffffff);

  __ Add(r4, r1, r2);
  __ Add(r5, r0, Operand(r1, LSL, 8));
  __ Add(r6, r0, Operand(r1, LSR, 8));
  __ Add(r7, r0, Operand(r1, ASR, 8));
  __ Add(r8, r3, Operand(r1, ROR, 8));

  // Set the C flag.
  __ Adds(r0, r3, 1);
  __ Add(r9, r3, Operand(r1, RRX));

  // Clear the C flag.
  __ Adds(r0, r0, 0);
  __ Add(r10, r3, Operand(r1, RRX));

  END();

  RUN();

  ASSERT_EQUAL_32(0x77777777, r4);
  ASSERT_EQUAL_32(0x23456700, r5);
  ASSERT_EQUAL_32(0x00012345, r6);
  ASSERT_EQUAL_32(0x00012345, r7);
  ASSERT_EQUAL_32(0x67012344, r8);
  ASSERT_EQUAL_32(0x8091a2b2, r9);
  ASSERT_EQUAL_32(0x0091a2b2, r10);

  TEARDOWN();
}


TEST(and_) {
  SETUP();

  START();
  __ Mov(r0, 0x0000fff0);
  __ Mov(r1, 0xf00000ff);
  __ Mov(r2, 0xffffffff);

  __ And(r3, r0, r1);
  __ And(r4, r0, Operand(r1, LSL, 4));
  __ And(r5, r0, Operand(r1, LSR, 1));
  __ And(r6, r0, Operand(r1, ASR, 20));
  __ And(r7, r0, Operand(r1, ROR, 28));
  __ And(r8, r0, 0xff);

  // Set the C flag.
  __ Adds(r9, r2, 1);
  __ And(r9, r1, Operand(r1, RRX));

  // Clear the C flag.
  __ Adds(r10, r0, 0);
  __ And(r10, r1, Operand(r1, RRX));
  END();

  RUN();

  ASSERT_EQUAL_32(0x000000f0, r3);
  ASSERT_EQUAL_32(0x00000ff0, r4);
  ASSERT_EQUAL_32(0x00000070, r5);
  ASSERT_EQUAL_32(0x0000ff00, r6);
  ASSERT_EQUAL_32(0x00000ff0, r7);
  ASSERT_EQUAL_32(0x000000f0, r8);
  ASSERT_EQUAL_32(0xf000007f, r9);
  ASSERT_EQUAL_32(0x7000007f, r10);

  TEARDOWN();
}


TEST(ands) {
  SETUP();

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0xf00000ff);

  __ Ands(r0, r1, r1);
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NFlag);
  ASSERT_EQUAL_32(0xf00000ff, r0);

  START();
  __ Mov(r0, 0x00fff000);
  __ Mov(r1, 0xf00000ff);

  __ Ands(r0, r0, Operand(r1, LSL, 4));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);
  ASSERT_EQUAL_32(0x00000000, r0);

  START();
  __ Mov(r0, 0x0000fff0);
  __ Mov(r1, 0xf00000ff);

  __ Ands(r0, r0, Operand(r1, LSR, 4));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);
  ASSERT_EQUAL_32(0x00000000, r0);

  START();
  __ Mov(r0, 0xf000fff0);
  __ Mov(r1, 0xf00000ff);

  __ Ands(r0, r0, Operand(r1, ASR, 4));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NCFlag);
  ASSERT_EQUAL_32(0xf0000000, r0);

  START();
  __ Mov(r0, 0x80000000);
  __ Mov(r1, 0x00000001);

  __ Ands(r0, r0, Operand(r1, ROR, 1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NCFlag);
  ASSERT_EQUAL_32(0x80000000, r0);

  START();
  __ Mov(r0, 0x80000000);
  __ Mov(r1, 0x80000001);

  // Clear the C flag, forcing RRX to insert 0 in r1's most significant bit.
  __ Adds(r2, r0, 0);
  __ Ands(r2, r0, Operand(r1, RRX));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);
  ASSERT_EQUAL_32(0, r2);

  START();
  __ Mov(r0, 0x80000000);
  __ Mov(r1, 0x80000001);
  __ Mov(r2, 0xffffffff);

  // Set the C flag, forcing RRX to insert 1 in r1's most significant bit.
  __ Adds(r2, r2, 1);
  __ Ands(r2, r0, Operand(r1, RRX));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NCFlag);
  ASSERT_EQUAL_32(0x80000000, r2);

  START();
  __ Mov(r0, 0xfff0);

  __ Ands(r0, r0, 0xf);
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZFlag);
  ASSERT_EQUAL_32(0x00000000, r0);

  START();
  __ Mov(r0, 0xff000000);

  __ Ands(r0, r0, 0x80000000);
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NCFlag);
  ASSERT_EQUAL_32(0x80000000, r0);

  TEARDOWN();
}


// TODO: fix this test in T32.
TEST_A32(adr) {
  SETUP();

  Label label_1, label_2, label_3, label_4;

  START();
  __ Mov(r0, 0x0);
  __ Adr(r1, &label_3);   // Set to zero to indicate success.

  __ Adr(r2, &label_1);   // Multiple forward references to the same label.
  __ Adr(r3, &label_1);
  __ Adr(r4, &label_1);

  __ Bind(&label_2);
  __ Eor(r5, r2, r3);  // Ensure that r2,r3 and r4 are identical.
  __ Eor(r6, r2, r4);
  __ Mov(r0, r5);
  __ Mov(r0, r6);
  __ Bx(r2);  // label_1, label_3

  __ Bind(&label_3);
  __ Adr(r2, &label_3);   // Self-reference (offset 0).
  __ Eor(r1, r1, r2);
  __ Adr(r2, &label_4);   // Simple forward reference.
  __ Bx(r2);  // label_4

  __ Bind(&label_1);
  __ Adr(r2, &label_3);   // Multiple reverse references to the same label.
  __ Adr(r3, &label_3);
  __ Adr(r4, &label_3);
  __ Adr(r5, &label_2);   // Simple reverse reference.
  __ Bx(r5);  // label_2

  __ Bind(&label_4);
  END();

  RUN();

  ASSERT_EQUAL_32(0x0, r0);
  ASSERT_EQUAL_32(0x0, r1);

  TEARDOWN();
}


TEST(shift_imm) {
  SETUP();

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0xfedcba98);
  __ Mov(r2, 0xffffffff);

  __ Lsl(r3, r1, 4);
  __ Lsr(r4, r1, 8);
  __ Asr(r5, r1, 16);
  __ Ror(r6, r1, 20);
  END();

  RUN();

  ASSERT_EQUAL_32(0xedcba980, r3);
  ASSERT_EQUAL_32(0x00fedcba, r4);
  ASSERT_EQUAL_32(0xfffffedc, r5);
  ASSERT_EQUAL_32(0xcba98fed, r6);

  TEARDOWN();
}


TEST(shift_reg) {
  SETUP();

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0xfedcba98);
  __ Mov(r2, 0xffffffff);

  __ Add(r9, r0, 4);
  __ Lsl(r3, r1, r9);

  __ Add(r9, r0, 8);
  __ Lsr(r4, r1, r9);

  __ Add(r9, r0, 16);
  __ Asr(r5, r1, r9);

  __ Add(r9, r0, 20);
  __ Ror(r6, r1, r9);

  // Set the C flag.
  __ Adds(r7, r2, 1);
  __ Rrx(r7, r1);

  // Clear the C flag.
  __ Adds(r8, r0, 0);
  __ Rrx(r8, r1);
  END();

  RUN();

  ASSERT_EQUAL_32(0xedcba980, r3);
  ASSERT_EQUAL_32(0x00fedcba, r4);
  ASSERT_EQUAL_32(0xfffffedc, r5);
  ASSERT_EQUAL_32(0xcba98fed, r6);
  ASSERT_EQUAL_32(0xff6e5d4c, r7);
  ASSERT_EQUAL_32(0x7f6e5d4c, r8);

  TEARDOWN();
}


TEST(branch_cond) {
  SETUP();

  Label done, wrong;

  START();
  __ Mov(r0, 0x0);
  __ Mov(r1, 0x1);
  __ Mov(r2, 0x80000000);
  // TODO: Use r0 instead of r3 when r0 becomes available.
  __ Mov(r3, 0x1);

  // For each 'cmp' instruction below, condition codes other than the ones
  // following it would branch.

  __ Cmp(r1, 0);
  __ B(eq, &wrong);
  __ B(lo, &wrong);
  __ B(mi, &wrong);
  __ B(vs, &wrong);
  __ B(ls, &wrong);
  __ B(lt, &wrong);
  __ B(le, &wrong);
  Label ok_1;
  __ B(ne, &ok_1);
  // TODO: Use __ Mov(r0, 0x0) instead.
  __ Add(r3, r0, 0x0);
  __ Bind(&ok_1);

  __ Cmp(r1, 1);
  __ B(ne, &wrong);
  __ B(lo, &wrong);
  __ B(mi, &wrong);
  __ B(vs, &wrong);
  __ B(hi, &wrong);
  __ B(lt, &wrong);
  __ B(gt, &wrong);
  Label ok_2;
  __ B(pl, &ok_2);
  // TODO: Use __ Mov(r0, 0x0) instead.
  __ Add(r3, r0, 0x0);
  __ Bind(&ok_2);

  __ Cmp(r1, 2);
  __ B(eq, &wrong);
  __ B(hs, &wrong);
  __ B(pl, &wrong);
  __ B(vs, &wrong);
  __ B(hi, &wrong);
  __ B(ge, &wrong);
  __ B(gt, &wrong);
  Label ok_3;
  __ B(vc, &ok_3);
  // TODO: Use __ Mov(r0, 0x0) instead.
  __ Add(r3, r0, 0x0);
  __ Bind(&ok_3);

  __ Cmp(r2, 1);
  __ B(eq, &wrong);
  __ B(lo, &wrong);
  __ B(mi, &wrong);
  __ B(vc, &wrong);
  __ B(ls, &wrong);
  __ B(ge, &wrong);
  __ B(gt, &wrong);
  Label ok_4;
  __ B(le, &ok_4);
  // TODO: Use __ Mov(r0, 0x0) instead.
  __ Add(r3, r0, 0x0);
  __ Bind(&ok_4);

  Label ok_5;
  __ B(&ok_5);
  // TODO: Use __ Mov(r0, 0x0) instead.
  __ Add(r3, r0, 0x0);
  __ Bind(&ok_5);

  __ B(&done);

  __ Bind(&wrong);
  // TODO: Use __ Mov(r0, 0x0) instead.
  __ Add(r3, r0, 0x0);

  __ Bind(&done);
  END();

  RUN();

  // TODO: Use r0.
  ASSERT_EQUAL_32(0x1, r3);

  TEARDOWN();
}


TEST(bfc_bfi) {
  SETUP();

  START();
  __ Mov(r0, 0xffffffff);
  __ Mov(r1, 0x01234567);
  __ Mov(r2, 0x0);

  __ Bfc(r0, 0, 3);
  __ Bfc(r0, 16, 5);

  __ Bfi(r2, r1, 0, 8);
  __ Bfi(r2, r1, 16, 16);
  END();

  RUN();

  ASSERT_EQUAL_32(0xffe0fff8, r0);
  ASSERT_EQUAL_32(0x45670067, r2);

  TEARDOWN();
}


TEST(bic) {
  SETUP();

  START();
  __ Mov(r0, 0xfff0);
  __ Mov(r1, 0xf00000ff);
  __ Mov(r2, 0xffffffff);

  __ Bic(r3, r0, r1);
  __ Bic(r4, r0, Operand(r1, LSL, 4));
  __ Bic(r5, r0, Operand(r1, LSR, 1));
  __ Bic(r6, r0, Operand(r1, ASR, 20));
  __ Bic(r7, r0, Operand(r1, ROR, 28));
  __ Bic(r8, r0, 0x1f);

  // Set the C flag.
  __ Adds(r9, r2, 1);
  __ Bic(r9, r1, Operand(r1, RRX));

  // Clear the C flag.
  __ Adds(r10, r0, 0);
  __ Bic(r10, r1, Operand(r1, RRX));
  END();

  RUN();

  ASSERT_EQUAL_32(0x0000ff00, r3);
  ASSERT_EQUAL_32(0x0000f000, r4);
  ASSERT_EQUAL_32(0x0000ff80, r5);
  ASSERT_EQUAL_32(0x000000f0, r6);
  ASSERT_EQUAL_32(0x0000f000, r7);
  ASSERT_EQUAL_32(0x0000ffe0, r8);
  ASSERT_EQUAL_32(0x00000080, r9);
  ASSERT_EQUAL_32(0x80000080, r10);

  TEARDOWN();
}


TEST(bics) {
  SETUP();

  START();
  __ Mov(r0, 0);
  __ Mov(r1, 0xf00000ff);

  __ Bics(r0, r1, r1);
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZFlag);
  ASSERT_EQUAL_32(0, r0);

  START();
  __ Mov(r0, 0x00fff000);
  __ Mov(r1, 0x0fffff00);

  __ Bics(r0, r0, Operand(r1, LSL, 4));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZFlag);
  ASSERT_EQUAL_32(0x00000000, r0);

  START();
  __ Mov(r0, 0x0000fff0);
  __ Mov(r1, 0x0fffff00);

  __ Bics(r0, r0, Operand(r1, LSR, 4));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZFlag);
  ASSERT_EQUAL_32(0x00000000, r0);

  START();
  __ Mov(r0, 0xf000fff0);
  __ Mov(r1, 0x0fffff00);

  __ Bics(r0, r0, Operand(r1, ASR, 4));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NFlag);
  ASSERT_EQUAL_32(0xf0000000, r0);

  START();
  __ Mov(r0, 0x80000000);
  __ Mov(r1, 0xfffffffe);

  __ Bics(r0, r0, Operand(r1, ROR, 1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NFlag);
  ASSERT_EQUAL_32(0x80000000, r0);

  START();
  __ Mov(r0, 0x80000000);
  __ Mov(r1, 0x80000001);

  // Clear the C flag, forcing RRX to insert 0 in r1's most significant bit.
  __ Adds(r2, r0, 0);
  __ Bics(r2, r0, Operand(r1, RRX));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NCFlag);
  ASSERT_EQUAL_32(0x80000000, r2);

  START();
  __ Mov(r0, 0x80000000);
  __ Mov(r1, 0x80000001);
  __ Mov(r2, 0xffffffff);

  // Set the C flag, forcing RRX to insert 1 in r1's most significant bit.
  __ Adds(r2, r2, 1);
  __ Bics(r2, r0, Operand(r1, RRX));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);
  ASSERT_EQUAL_32(0, r2);

  START();
  __ Mov(r0, 0xf000);

  __ Bics(r0, r0, 0xf000);
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZFlag);
  ASSERT_EQUAL_32(0x00000000, r0);

  START();
  __ Mov(r0, 0xff000000);

  __ Bics(r0, r0, 0x7fffffff);
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NFlag);
  ASSERT_EQUAL_32(0x80000000, r0);

  TEARDOWN();
}


TEST_T32(veneer_pool_in_delegate) {
  SETUP();

  START();

  Label end;

  VIXL_CHECK(masm.VeneerPoolIsEmpty());
  VIXL_CHECK(masm.LiteralPoolIsEmpty());

  __ Mov(r0, 1);
  __ Cbz(r0, &end);

  VIXL_CHECK(!masm.VeneerPoolIsEmpty());
  VIXL_CHECK(masm.LiteralPoolIsEmpty());

  // Generate enough code to have, after the loop, a margin of only one 16-bit
  // instruction that can be generated before we need to generate the veneer
  // pool.
  // Use `CodeBufferCheckScope` and the assembler to generate the code.
  int32_t space =
      masm.GetMarginBeforeVeneerEmission() - k16BitT32InstructionSizeInBytes;
  {
    AssemblerAccurateScope scope(&masm,
                                 space,
                                 CodeBufferCheckScope::kExactSize);
    while (space > 0) {
      __ nop();
      space -= k16BitT32InstructionSizeInBytes;
    }
  }

  // We should not have emitted the veneer pool at this point.
  VIXL_CHECK(!masm.VeneerPoolIsEmpty());
  VIXL_CHECK(masm.LiteralPoolIsEmpty());
  VIXL_CHECK(
      masm.GetMarginBeforeVeneerEmission() == k16BitT32InstructionSizeInBytes);

  // Now generate `Mov(r1, 0x12345678)`. It needs to 16-bit assembler
  // instructions, so it has to go through the `MacroAssembler` delegate. Since
  // there is only margin for one instruction to be generated, the pool will
  // have to be generated from within the `MacroAssembler` delegate. That should
  // not fire.
  Label check;
  __ Bind(&check);
  __ Mov(r1, 0x12345678);
  VIXL_CHECK(masm.GetSizeOfCodeGeneratedSince(&check) >
             2 * kMaxInstructionSizeInBytes);
  __ Bind(&end);

  END();

  RUN();

  ASSERT_EQUAL_32(0x12345678, r1);

  TEARDOWN();
}


TEST_T32(literal_pool_in_delegate) {
  SETUP();

  START();

  PrintDisassembler disasm(std::cout);

  VIXL_CHECK(masm.VeneerPoolIsEmpty());
  VIXL_CHECK(masm.LiteralPoolIsEmpty());

  __ Ldrd(r0, r1, 0x1234567890abcdef);

  VIXL_CHECK(masm.VeneerPoolIsEmpty());
  VIXL_CHECK(!masm.LiteralPoolIsEmpty());

  // Generate enough code to have, after the loop, a margin of only one 16-bit
  // instruction that can be generated before we need to generate the literal
  // pool.
  // Use `CodeBufferCheckScope` and the assembler to generate the code.
  int32_t space = masm.GetMarginBeforeLiteralEmission() -
      2 * k16BitT32InstructionSizeInBytes;
  {
    AssemblerAccurateScope scope(&masm,
                                 space,
                                 CodeBufferCheckScope::kExactSize);
    while (space > 0) {
      __ nop();
      space -= k16BitT32InstructionSizeInBytes;
    }
  }

  // We should not have emitted the literal pool at this point.
  VIXL_CHECK(masm.VeneerPoolIsEmpty());
  VIXL_CHECK(!masm.LiteralPoolIsEmpty());
  VIXL_CHECK(masm.GetMarginBeforeLiteralEmission() ==
             2 * k16BitT32InstructionSizeInBytes);

  // Now generate `Mov(r1, 0x12345678)`. It needs to 16-bit assembler
  // instructions, so it has to go through the `MacroAssembler` delegate. Since
  // there is only margin for one instruction to be generated, the pool will
  // have to be generated from within the `MacroAssembler` delegate. That should
  // not fire.
  Label check;
  __ Bind(&check);
  __ Mov(r1, 0x12345678);
  VIXL_CHECK(masm.GetSizeOfCodeGeneratedSince(&check) >
             2 * kMaxInstructionSizeInBytes);

  VIXL_CHECK(masm.VeneerPoolIsEmpty());
  VIXL_CHECK(masm.LiteralPoolIsEmpty());

  END();

  RUN();

  ASSERT_EQUAL_32(0x12345678, r1);

  TEARDOWN();
}


TEST(emit_single_literal) {
  SETUP();

  START();
  // Make sure the pool is empty.
  masm.EmitLiteralPool(MacroAssembler::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  // Create one literal pool entry.
  __ Ldrd(r0, r1, 0x1234567890abcdef);
  ASSERT_LITERAL_POOL_SIZE(8);
  __ Vldr(s0, 1.0);
  __ Vldr(d1, 2.0);
  __ Vmov(d2, 4.1);
  __ Vmov(s8, 8.2);
  ASSERT_LITERAL_POOL_SIZE(20);
  END();

  RUN();

  // Check that the literals loaded correctly.
  ASSERT_EQUAL_32(0x90abcdef, r0);
  ASSERT_EQUAL_32(0x12345678, r1);
  ASSERT_EQUAL_FP32(1.0f, s0);
  ASSERT_EQUAL_FP64(2.0, d1);
  ASSERT_EQUAL_FP64(4.1, d2);
  ASSERT_EQUAL_FP32(8.2f, s8);

  TEARDOWN();
}


#undef __
#define __ masm->


void EmitLdrdLiteralTest(MacroAssembler* masm) {
  const int ldrd_range = masm->IsUsingA32() ? 255 : 1020;
  // We want to emit code up to the maximum literal load range and ensure the
  // pool has not been emitted. Compute the limit (end).
  ptrdiff_t end =
      AlignDown(
          // Align down the PC to 4 bytes as the instruction does when it's
          // executed.
          // The PC will be the cursor offset plus the architecture state PC
          // offset.
          AlignDown(masm->GetBuffer()->GetCursorOffset() +
                    masm->GetArchitectureStatePCOffset(), 4) +
          // Maximum range allowed to access the constant.
          ldrd_range -
          // A branch will be generated before the pool.
          kMaxInstructionSizeInBytes,
          // AlignDown to 4 byte as the literals will be 4 byte aligned.
          4);

  // Create one literal pool entry.
  __ Ldrd(r0, r1, 0x1234567890abcdef);
  ASSERT_LITERAL_POOL_SIZE(8);

  int32_t margin = masm->GetMarginBeforeLiteralEmission();
  {
    AssemblerAccurateScope scope(masm,
                                 margin,
                                 CodeBufferCheckScope::kExactSize);
    // Opening the scope should not have triggered the emission of the literal
    // pool.
    VIXL_CHECK(!masm->LiteralPoolIsEmpty());
    while (masm->GetCursorOffset() < end) {
      __ nop();
    }
    VIXL_CHECK(masm->GetCursorOffset() == end);
  }

  // Check that the pool has not been emited along the way.
  ASSERT_LITERAL_POOL_SIZE(8);
  // This extra instruction should trigger an emit of the pool.
  __ Nop();
  // The pool should have been emitted.
  ASSERT_LITERAL_POOL_SIZE(0);
}


#undef __
#define __ masm.


TEST(emit_literal) {
  SETUP();

  START();

  // Make sure the pool is empty.
  masm.EmitLiteralPool(MacroAssembler::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  EmitLdrdLiteralTest(&masm);

  const int ldrd_range = masm.IsUsingA32() ? 255 : 1020;
  const int string_size = AlignUp(ldrd_range + kMaxInstructionSizeInBytes, 4);
  std::string test_string(string_size, 'x');
  StringLiteral big_literal(test_string.c_str());
  __ Adr(r4, &big_literal);
  // This add will overflow the literal pool and force a rewind.
  // That means that the string will be generated then, then Ldrd and the
  // ldrd's value will be alone in the pool.
  __ Ldrd(r2, r3, 0xcafebeefdeadbaba);
  ASSERT_LITERAL_POOL_SIZE(8);

  masm.EmitLiteralPool(MacroAssembler::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);
  __ Ldr(r4, MemOperand(r4));  // Load the first 4 characters in r4.
  END();

  RUN();

  // Check that the literals loaded correctly.
  ASSERT_EQUAL_32(0x90abcdef, r0);
  ASSERT_EQUAL_32(0x12345678, r1);
  ASSERT_EQUAL_32(0xdeadbaba, r2);
  ASSERT_EQUAL_32(0xcafebeef, r3);
  ASSERT_EQUAL_32(0x78787878, r4);

  TEARDOWN();
}

TEST_T32(emit_literal_unaligned) {
  SETUP();

  START();

  // Make sure the pool is empty.
  masm.EmitLiteralPool(MacroAssembler::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  // Generate a nop to break the 4 bytes alignment.
  __ Nop();

  EmitLdrdLiteralTest(&masm);

  END();

  RUN();

  // Check that the literals loaded correctly.
  ASSERT_EQUAL_32(0x90abcdef, r0);
  ASSERT_EQUAL_32(0x12345678, r1);

  TEARDOWN();
}


TEST(literal_multiple_uses) {
  SETUP();

  START();
  Literal<int32_t> lit(42);
  __ Ldr(r0, &lit);
  ASSERT_LITERAL_POOL_SIZE(4);

  // Multiple uses of the same literal object should not make the
  // pool grow.
  __ Ldrb(r1, &lit);
  __ Ldrsb(r2, &lit);
  __ Ldrh(r3, &lit);
  __ Ldrsh(r4, &lit);
  ASSERT_LITERAL_POOL_SIZE(4);

  END();

  RUN();

  ASSERT_EQUAL_32(42, r0);
  ASSERT_EQUAL_32(42, r1);
  ASSERT_EQUAL_32(42, r2);
  ASSERT_EQUAL_32(42, r3);
  ASSERT_EQUAL_32(42, r4);

  TEARDOWN();
}


// A test with two loads literal which go out of range at the same time.
TEST_A32(ldr_literal_range_same_time) {
  SETUP();

  START();
  const int ldrd_range = 255;
  // We need to take into account the jump over the pool.
  const int ldrd_padding = ldrd_range - kA32InstructionSizeInBytes;
  const int ldr_range = 4095;
  // We need to take into account the ldrd padding and the ldrd instruction.
  const int ldr_padding = ldr_range - ldrd_padding - kA32InstructionSizeInBytes;

  __ Ldr(r1, 0x12121212);
  ASSERT_LITERAL_POOL_SIZE(4);

  for (unsigned int i = 0; i < ldr_padding / kA32InstructionSizeInBytes; ++i) {
    __ Mov(r0, 0);
  }

  __ Ldrd(r2, r3, 0x1234567890abcdef);
  ASSERT_LITERAL_POOL_SIZE(12);

  for (unsigned int i = 0; i < ldrd_padding / kA32InstructionSizeInBytes; ++i) {
    __ Mov(r0, 0);
  }
  ASSERT_LITERAL_POOL_SIZE(12);

  // This mov will put the two loads literal out of range and will force
  // the literal pool emission.
  __ Mov(r0, 0);
  ASSERT_LITERAL_POOL_SIZE(0);
  END();

  RUN();

  ASSERT_EQUAL_32(0x12121212, r1);
  ASSERT_EQUAL_32(0x90abcdef, r2);
  ASSERT_EQUAL_32(0x12345678, r3);

  TEARDOWN();
}


TEST(ldr_literal_mix_types) {
  SETUP();

  START();
  Literal<uint64_t> l0(0x1234567890abcdef);
  Literal<int32_t> l1(0x12345678);
  Literal<uint16_t> l2(1234);
  Literal<int16_t> l3(-678);
  Literal<uint8_t> l4(42);
  Literal<int8_t> l5(-12);

  __ Ldrd(r0, r1, &l0);
  __ Ldr(r2, &l1);
  __ Ldrh(r3, &l2);
  __ Ldrsh(r4, &l3);
  __ Ldrb(r5, &l4);
  __ Ldrsb(r6, &l5);
  ASSERT_LITERAL_POOL_SIZE(28);

  END();

  RUN();

  ASSERT_EQUAL_32(0x90abcdef, r0);
  ASSERT_EQUAL_32(0x12345678, r1);
  ASSERT_EQUAL_32(0x12345678, r2);
  ASSERT_EQUAL_32(1234, r3);
  ASSERT_EQUAL_32(-678, r4);
  ASSERT_EQUAL_32(42, r5);
  ASSERT_EQUAL_32(-12, r6);

  TEARDOWN();
}


struct LdrLiteralRangeTest {
  void (MacroAssembler::*instruction)(Register, RawLiteral*);
  Register result_reg;
  int a32_range;
  int t32_range;
  uint32_t literal_value;
  uint32_t test_value;
};


const LdrLiteralRangeTest kLdrLiteralRangeTestData[] = {
  {&MacroAssembler::Ldr, r1, 4095, 4095, 0x12345678, 0x12345678 },
  {&MacroAssembler::Ldrh, r2, 255, 4095, 0xabcdefff, 0x0000efff },
  {&MacroAssembler::Ldrsh, r3, 255, 4095, 0x00008765, 0xffff8765 },
  {&MacroAssembler::Ldrb, r4, 4095, 4095, 0x12345678, 0x00000078 },
  {&MacroAssembler::Ldrsb, r5, 255, 4095, 0x00000087, 0xffffff87 }
};


void GenerateLdrLiteralTriggerPoolEmission(InstructionSet isa,
                                           bool unaligned_ldr) {
  SETUP();

  for (size_t i = 0; i < ARRAY_SIZE(kLdrLiteralRangeTestData); ++i) {
    const LdrLiteralRangeTest& test = kLdrLiteralRangeTestData[i];

    START();

    if (unaligned_ldr) {
      // Generate a nop to break the 4-byte alignment.
      __ Nop();
      VIXL_ASSERT((masm.GetBuffer()->GetCursorOffset() % 4) == 2);
    }

    __ Ldr(r6, 0x12345678);
    ASSERT_LITERAL_POOL_SIZE(4);

    // In A32 mode we can fit one more instruction before being forced to emit
    // the pool. However the newly added literal will be to far for the ldr
    // instruction forcing the pool to be emitted earlier. So we need to make
    // sure that we need to stop one instruction before the margin on A32 for
    // this test to work as expected.
    int32_t margin_offset = masm.IsUsingA32() ? kA32InstructionSizeInBytes : 0;

    size_t expected_pool_size = 4;
    while ((masm.GetMarginBeforeLiteralEmission() - margin_offset) >=
	   static_cast<int32_t>(kMaxInstructionSizeInBytes)) {
      __ Ldr(r7, 0x90abcdef);
      // Each ldr instruction will force a new literal value to be added
      // to the pool. Check that the literal pool grows accordingly.
      expected_pool_size += 4;
      ASSERT_LITERAL_POOL_SIZE(expected_pool_size);
    }

    // This ldr will force the literal pool to be emitted before emitting
    // the load and will create a new pool for the new literal used by this ldr.
    Literal<uint32_t> literal(test.literal_value);
    (masm.*test.instruction)(test.result_reg, &literal);
    ASSERT_LITERAL_POOL_SIZE(4);

    END();

    RUN();

    ASSERT_EQUAL_32(0x12345678, r6);
    ASSERT_EQUAL_32(0x90abcdef, r7);
    ASSERT_EQUAL_32(test.test_value, test.result_reg);
  }

  TEARDOWN();
}


TEST(ldr_literal_trigger_pool_emission) {
  GenerateLdrLiteralTriggerPoolEmission(isa, false);
}


TEST_T32(ldr_literal_trigger_pool_emission_unaligned) {
  GenerateLdrLiteralTriggerPoolEmission(isa, true);
}


void GenerateLdrLiteralRangeTest(InstructionSet isa, bool unaligned_ldr) {
  SETUP();

  for (size_t i = 0; i < ARRAY_SIZE(kLdrLiteralRangeTestData); ++i) {
    const LdrLiteralRangeTest& test = kLdrLiteralRangeTestData[i];

    START();

    // Make sure the pool is empty.
    masm.EmitLiteralPool(MacroAssembler::kBranchRequired);
    ASSERT_LITERAL_POOL_SIZE(0);

    if (unaligned_ldr) {
      // Generate a nop to break the 4-byte alignment.
      __ Nop();
      VIXL_ASSERT((masm.GetBuffer()->GetCursorOffset() % 4) == 2);
    }

    Literal<uint32_t> literal(test.literal_value);
    (masm.*test.instruction)(test.result_reg, &literal);
    ASSERT_LITERAL_POOL_SIZE(4);

    // Generate enough instruction so that we go out of range for the load
    // literal we just emitted.
    ptrdiff_t end =
        masm.GetBuffer()->GetCursorOffset() +
        ((masm.IsUsingA32()) ? test.a32_range : test.t32_range);
    while (masm.GetBuffer()->GetCursorOffset() < end) {
      __ Mov(r0, 0);
    }

    // The literal pool should have been emitted now.
    VIXL_CHECK(literal.IsBound());
    ASSERT_LITERAL_POOL_SIZE(0);

    END();

    RUN();

    ASSERT_EQUAL_32(test.test_value, test.result_reg);
  }

  TEARDOWN();
}


TEST(ldr_literal_range) {
  GenerateLdrLiteralRangeTest(isa, false);
}


TEST_T32(ldr_literal_range_unaligned) {
  GenerateLdrLiteralRangeTest(isa, true);
}


TEST(string_literal) {
  SETUP();

  START();
  // Make sure the pool is empty.
  masm.EmitLiteralPool(MacroAssembler::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  StringLiteral hello_string("hello");

  __ Ldrb(r1, &hello_string);

  __ Adr(r0, &hello_string);
  __ Ldrb(r2, MemOperand(r0));
  END();

  RUN();

  ASSERT_EQUAL_32('h', r1);
  ASSERT_EQUAL_32('h', r2);

  TEARDOWN();
}


TEST(custom_literal_in_pool) {
  SETUP();

  START();
  // Make sure the pool is empty.
  masm.EmitLiteralPool(MacroAssembler::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  Literal<uint32_t> l0(static_cast<uint32_t>(0x12345678));
  __ Ldr(r0, &l0);
  masm.EmitLiteralPool(MacroAssembler::kBranchRequired);
  __ Ldr(r1, &l0);
  ASSERT_LITERAL_POOL_SIZE(0);

  Literal<uint64_t> cafebeefdeadbaba(0xcafebeefdeadbaba);
  __ Ldrd(r8, r9, &cafebeefdeadbaba);
  masm.EmitLiteralPool(MacroAssembler::kBranchRequired);
  __ Ldrd(r2, r3, &cafebeefdeadbaba);
  ASSERT_LITERAL_POOL_SIZE(0);

  Literal<uint32_t> l1(0x09abcdef);
  __ Adr(r4, &l1);
  __ Ldr(r4, MemOperand(r4));
  masm.EmitLiteralPool();
  __ Adr(r5, &l1);
  __ Ldr(r5, MemOperand(r5));
  ASSERT_LITERAL_POOL_SIZE(0);

  END();

  RUN();

  // Check that the literals loaded correctly.
  ASSERT_EQUAL_32(0x12345678, r0);
  ASSERT_EQUAL_32(0x12345678, r1);
  ASSERT_EQUAL_32(0xdeadbaba, r2);
  ASSERT_EQUAL_32(0xcafebeef, r3);
  ASSERT_EQUAL_32(0xdeadbaba, r8);
  ASSERT_EQUAL_32(0xcafebeef, r9);
  ASSERT_EQUAL_32(0x09abcdef, r4);
  ASSERT_EQUAL_32(0x09abcdef, r5);
}


TEST(custom_literal_place) {
  SETUP();

  START();
  // Make sure the pool is empty.
  masm.EmitLiteralPool(MacroAssembler::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  Literal<uint64_t> l0(0xcafebeefdeadbaba, RawLiteral::kManuallyPlaced);
  Literal<int32_t> l1(0x12345678, RawLiteral::kManuallyPlaced);
  Literal<uint16_t>l2(4567, RawLiteral::kManuallyPlaced);
  Literal<int16_t> l3(-4567, RawLiteral::kManuallyPlaced);
  Literal<uint8_t> l4(123, RawLiteral::kManuallyPlaced);
  Literal<int8_t> l5(-123, RawLiteral::kManuallyPlaced);

  __ Ldrd(r0, r1, &l0);
  __ Ldr(r2, &l1);
  __ Ldrh(r3, &l2);
  __ Ldrsh(r4, &l3);
  __ Ldrb(r5, &l4);
  __ Ldrsb(r6, &l5);

  ASSERT_LITERAL_POOL_SIZE(0);

  // Manually generate a literal pool.
  Label after_pool;
  __ B(&after_pool);
  __ Place(&l0);
  __ Place(&l1);
  __ Place(&l2);
  __ Place(&l3);
  __ Place(&l4);
  __ Place(&l5);
  __ Bind(&after_pool);

  UseScratchRegisterScope temps(&masm);
  Register temp = temps.Acquire();
  VIXL_CHECK(temp.Is(r12));

  __ Ldrd(r8, r9, &l0);
  __ Ldr(r7, &l1);
  __ Ldrh(r10, &l2);
  __ Ldrsh(r11, &l3);
  __ Ldrb(temp, &l4);
  // We don't use any function call so we can use lr as an extra register.
  __ Ldrsb(lr, &l5);

  ASSERT_LITERAL_POOL_SIZE(0);

  END();

  RUN();

  // Check that the literals loaded correctly.
  ASSERT_EQUAL_32(0xdeadbaba, r0);
  ASSERT_EQUAL_32(0xcafebeef, r1);
  ASSERT_EQUAL_32(0x12345678, r2);
  ASSERT_EQUAL_32(4567, r3);
  ASSERT_EQUAL_32(-4567, r4);
  ASSERT_EQUAL_32(123, r5);
  ASSERT_EQUAL_32(-123, r6);

  ASSERT_EQUAL_32(0xdeadbaba, r8);
  ASSERT_EQUAL_32(0xcafebeef, r9);
  ASSERT_EQUAL_32(0x12345678, r7);
  ASSERT_EQUAL_32(4567, r10);
  ASSERT_EQUAL_32(-4567, r11);
  ASSERT_EQUAL_32(123, temp);
  ASSERT_EQUAL_32(-123, lr);

  TEARDOWN();
}


TEST(custom_literal_place_shared) {
  SETUP();

  for (size_t i = 0; i < ARRAY_SIZE(kLdrLiteralRangeTestData); ++i) {
    const LdrLiteralRangeTest& test = kLdrLiteralRangeTestData[i];

    START();

    // Make sure the pool is empty.
    masm.EmitLiteralPool(MacroAssembler::kBranchRequired);
    ASSERT_LITERAL_POOL_SIZE(0);

    Literal<uint32_t> before(test.literal_value, RawLiteral::kManuallyPlaced);
    Literal<uint32_t> after(test.literal_value, RawLiteral::kManuallyPlaced);

    VIXL_CHECK(!before.IsBound());
    VIXL_CHECK(!after.IsBound());

    // Manually generate a pool.
    Label end_of_pool_before;
    __ B(&end_of_pool_before);
    __ Place(&before);
    __ Bind(&end_of_pool_before);

    ASSERT_LITERAL_POOL_SIZE(0);
    VIXL_CHECK(before.IsBound());
    VIXL_CHECK(!after.IsBound());

  // Load the entries several times to test that literals can be shared.
    for (int i = 0; i < 20; i++) {
      (masm.*test.instruction)(r0, &before);
      (masm.*test.instruction)(r1, &after);
    }

    ASSERT_LITERAL_POOL_SIZE(0);
    VIXL_CHECK(before.IsBound());
    VIXL_CHECK(!after.IsBound());

    // Manually generate a pool.
    Label end_of_pool_after;
    __ B(&end_of_pool_after);
    __ Place(&after);
    __ Bind(&end_of_pool_after);

    ASSERT_LITERAL_POOL_SIZE(0);
    VIXL_CHECK(before.IsBound());
    VIXL_CHECK(after.IsBound());

    END();

    RUN();

    ASSERT_EQUAL_32(test.test_value, r0);
    ASSERT_EQUAL_32(test.test_value, r1);
  }

  TEARDOWN();
}


TEST(emit_big_pool) {
  SETUP();

  START();
  // Make sure the pool is empty.
  ASSERT_LITERAL_POOL_SIZE(0);

  Label start;
  __ Bind(&start);
  for (int i = 1000; i > 0; --i) {
    __ Ldr(r0, i);
  }

  VIXL_ASSERT(masm.GetSizeOfCodeGeneratedSince(&start) == 4000);

  ASSERT_LITERAL_POOL_SIZE(4000);
  END();

  RUN();

  // Check that the literals loaded correctly.
  ASSERT_EQUAL_32(1, r0);

  TEARDOWN();
}


TEST_T32(too_far_cbz) {
  SETUP();

  START();
  Label start;
  Label end;
  Label exit;
  __ Mov(r0, 0);
  __ B(&start);
  __ Bind(&end);
  __ Mov(r0, 1);
  __ B(&exit);
  __ Bind(&start);
  // Cbz is only defined for forward jump. Check that it will work (substituted
  // by Cbnz/B).
  __ Cbz(r0, &end);
  __ Bind(&exit);
  END();

  RUN();

  ASSERT_EQUAL_32(1, r0);
}


TEST_T32(close_cbz) {
  SETUP();

  START();
  Label first;
  Label second;
  __ Mov(r0, 0);
  __ Mov(r1, 0);
  __ Mov(r2, 0);
  __ Cbz(r0, &first);
  __ Bind(&first);
  __ Mov(r1, 1);
  __ Cbnz(r0, &second);
  __ Bind(&second);
  __ Mov(r2, 2);
  END();

  RUN();

  ASSERT_EQUAL_32(0, r0);
  ASSERT_EQUAL_32(1, r1);
  ASSERT_EQUAL_32(2, r2);
}


TEST_T32(close_cbz2) {
  SETUP();

  START();
  Label first;
  Label second;
  __ Mov(r0, 0);
  __ Mov(r1, 0);
  __ Mov(r2, 0);
  __ Cmp(r0, 0);
  __ B(ne, &first);
  __ B(gt, &second);
  __ Cbz(r0, &first);
  __ Bind(&first);
  __ Mov(r1, 1);
  __ Cbnz(r0, &second);
  __ Bind(&second);
  __ Mov(r2, 2);
  END();

  RUN();

  ASSERT_EQUAL_32(0, r0);
  ASSERT_EQUAL_32(1, r1);
  ASSERT_EQUAL_32(2, r2);
}


TEST_T32(not_close_cbz) {
  SETUP();

  START();
  Label first;
  Label second;
  __ Cbz(r0, &first);
  __ B(ne, &first);
  __ Bind(&first);
  __ Cbnz(r0, &second);
  __ B(gt, &second);
  __ Bind(&second);
  END();

  RUN();
}


TEST_T32(veneers) {
  SETUP();

  START();
  Label zero;
  Label exit;
  __ Mov(r0, 0);
  // Create one literal pool entry.
  __ Ldr(r1, 0x12345678);
  ASSERT_LITERAL_POOL_SIZE(4);
  __ Cbz(r0, &zero);
  __ Mov(r0, 1);
  __ B(&exit);
  for (int i = 32; i > 0; i--) {
    __ Mov(r1, 0);
  }
  // Assert that the literal pool has been generated with the veneers.
  ASSERT_LITERAL_POOL_SIZE(0);
  __ Bind(&zero);
  __ Mov(r0, 2);
  __ Bind(&exit);
  END();

  RUN();

  ASSERT_EQUAL_32(2, r0);
  ASSERT_EQUAL_32(0x12345678, r1);
}


// This test checks that veneers are sorted. If not, the test failed as the
// veneer for "exit" is emitted before the veneer for "zero" and the "zero"
// veneer is out of range for Cbz.
TEST_T32(veneers_labels_sort) {
  SETUP();

  START();
  Label start;
  Label zero;
  Label exit;
  __ Movs(r0, 0);
  __ B(ne, &exit);
  __ B(&start);
  for (int i = 1048400; i > 0; i -= 4) {
    __ Mov(r1, 0);
  }
  __ Bind(&start);
  __ Cbz(r0, &zero);
  __ Mov(r0, 1);
  __ B(&exit);
  for (int i = 32; i > 0; i--) {
    __ Mov(r1, 0);
  }
  __ Bind(&zero);
  __ Mov(r0, 2);
  __ Bind(&exit);
  END();

  RUN();

  ASSERT_EQUAL_32(2, r0);
}

// Check that a label bound within the assembler is effectively removed from
// the veneer pool.
TEST_T32(veneer_bind) {
  SETUP();
  Label target;
  __ Cbz(r0, &target);
  __ Nop();

  {
    // Bind the target label using the `Assembler`.
    AssemblerAccurateScope aas(&masm,
                               kMaxInstructionSizeInBytes,
                               CodeBufferCheckScope::kMaximumSize);
    __ bind(&target);
    __ nop();
  }

  VIXL_CHECK(target.IsBound());
  VIXL_CHECK(masm.VeneerPoolIsEmpty());

  END();
}

// This test check that we can update a Literal after usage.
TEST(literal_update) {
  SETUP();

  START();
  Label exit;
  Literal<uint32_t>* a32 =
      new Literal<uint32_t>(0xabcdef01, RawLiteral::kDeletedOnPoolDestruction);
  Literal<uint64_t>* a64 =
      new Literal<uint64_t>(
          UINT64_C(0xabcdef01abcdef01), RawLiteral::kDeletedOnPoolDestruction);
  __ Ldr(r0, a32);
  __ Ldrd(r2, r3, a64);
  __ EmitLiteralPool();
  Literal<uint32_t>* b32 =
      new Literal<uint32_t>(0x10fedcba, RawLiteral::kDeletedOnPoolDestruction);
  Literal<uint64_t>* b64 =
      new Literal<uint64_t>(
          UINT64_C(0x10fedcba10fedcba), RawLiteral::kDeletedOnPoolDestruction);
  __ Ldr(r1, b32);
  __ Ldrd(r4, r5, b64);
  // Update literals' values. "a32" and "a64" are already emitted. "b32" and
  // "b64" will only be emitted when "END()" will be called.
  a32->UpdateValue(0x12345678, masm.GetBuffer());
  a64->UpdateValue(UINT64_C(0x13579bdf02468ace), masm.GetBuffer());
  b32->UpdateValue(0x87654321, masm.GetBuffer());
  b64->UpdateValue(UINT64_C(0x1032547698badcfe), masm.GetBuffer());
  END();

  RUN();

  ASSERT_EQUAL_32(0x12345678, r0);
  ASSERT_EQUAL_32(0x87654321, r1);
  ASSERT_EQUAL_32(0x02468ace, r2);
  ASSERT_EQUAL_32(0x13579bdf, r3);
  ASSERT_EQUAL_32(0x98badcfe, r4);
  ASSERT_EQUAL_32(0x10325476, r5);
}


void SwitchCase(JumpTableBase* switch_, uint32_t case_index,
                InstructionSet isa, bool bind_default = true) {
  SETUP();

  START();

  __ Mov(r0, case_index);
  __ Mov(r1, case_index);
  __ Switch(r1, switch_);

  __ Case(switch_, 0);
  __ Mov(r0, 1);
  __ Break(switch_);

  __ Case(switch_, 1);
  __ Mov(r0, 2);
  __ Break(switch_);

  __ Case(switch_, 2);
  __ Mov(r0, 4);
  __ Break(switch_);

  __ Case(switch_, 3);
  __ Mov(r0, 8);
  __ Break(switch_);

  if (bind_default) {
    __ Default(switch_);
    __ Mov(r0, -1);
  }

  __ EndSwitch(switch_);


  END();

  RUN();

  if (case_index < 4) {
    ASSERT_EQUAL_32(1 << case_index, r0);
  } else if (bind_default) {
    ASSERT_EQUAL_32(-1, r0);
  } else {
    ASSERT_EQUAL_32(case_index, r0);
  }
}


TEST(switch_case_8) {
  for (int i = 0; i < 5; i++) {
    JumpTable8bitOffset switch_(5);
    SwitchCase(&switch_, i, isa);
  }
}


TEST(switch_case_16) {
  for (int i = 0; i < 5; i++) {
    JumpTable16bitOffset switch_(5);
    SwitchCase(&switch_, i, isa);
  }
}


TEST(switch_case_32) {
  for (int i = 0; i < 5; i++) {
    JumpTable32bitOffset switch_(5);
    SwitchCase(&switch_, i, isa);
  }
}


TEST(switch_case_8_omit_default) {
  for (int i = 0; i < 5; i++) {
    JumpTable8bitOffset switch_(5);
    SwitchCase(&switch_, i, isa, false);
  }
}


TEST(switch_case_16_omit_default) {
  for (int i = 0; i < 5; i++) {
    JumpTable16bitOffset switch_(5);
    SwitchCase(&switch_, i, isa, false);
  }
}


TEST(switch_case_32_omit_default) {
  for (int i = 0; i < 5; i++) {
    JumpTable32bitOffset switch_(5);
    SwitchCase(&switch_, i, isa, false);
  }
}

TEST(claim_peek_poke) {
  SETUP();

  START();

  Label start;
  __ Bind(&start);
  __ Claim(0);
  __ Drop(0);
  VIXL_CHECK((masm.GetCursorOffset() - start.GetLocation()) == 0);

  __ Claim(32);
  __ Ldr(r0, 0xcafe0000);
  __ Ldr(r1, 0xcafe0001);
  __ Ldr(r2, 0xcafe0002);
  __ Poke(r0, 0);
  __ Poke(r1, 4);
  __ Poke(r2, 8);
  __ Peek(r2, 0);
  __ Peek(r0, 4);
  __ Peek(r1, 8);
  __ Drop(32);

  END();

  RUN();

  ASSERT_EQUAL_32(0xcafe0001, r0);
  ASSERT_EQUAL_32(0xcafe0002, r1);
  ASSERT_EQUAL_32(0xcafe0000, r2);

  TEARDOWN();
}


TEST(msr_i) {
  SETUP();

  START();
  __ Mov(r0, 0xdead);
  __ Mov(r1, 0xdead);
  __ Mov(r2, 0xdead);
  __ Mov(r3, 0xb);
  __ Msr(APSR_nzcvqg, 0);
  __ Mrs(r0, APSR);
  __ Msr(APSR_nzcvqg, 0xffffffff);
  __ Mrs(r1, APSR);
  // Only modify nzcvq => keep previous g.
  __ Lsl(r4, r3, 28);
  __ Msr(APSR_nzcvq, r4);
  __ Mrs(r2, APSR);
  END();

  RUN();

  ASSERT_EQUAL_32(0x10, r0);
  ASSERT_EQUAL_32(0xf80f0010, r1);
  ASSERT_EQUAL_32(0xb00f0010, r2);

  TEARDOWN();
}


TEST(printf) {
  SETUP();

  START();
  __ Mov(r0, 0xb00e0000);
  __ Msr(APSR_nzcvqg, r0);
  __ Mov(r0, sp);
  __ Printf("sp=%x\n", r0);
//  __ Printf("Hello world!\n");
  __ Mov(r0, 0x1234);
  __ Mov(r1, 0x5678);
  StringLiteral literal("extra string");
  __ Adr(r2, &literal);
  __ Mov(r3, 5);
  __ Mov(r4, 0xdead4444);
  __ Mov(r5, 0xdead5555);
  __ Mov(r6, 0xdead6666);
  __ Mov(r7, 0xdead7777);
  __ Mov(r8, 0xdead8888);
  __ Mov(r9, 0xdead9999);
  __ Mov(r10, 0xdeadaaaa);
  __ Mov(r11, 0xdeadbbbb);
  __ Vldr(d0, 1.2345);
  __ Vldr(d1, 2.9876);
  __ Vldr(s4, 1.3333);
  __ Vldr(s5, 3.21);
  __ Vldr(d3, 3.333);
  __ Vldr(d4, 4.444);
  __ Vldr(d5, 5.555);
  __ Vldr(d6, 6.666);
  __ Vldr(d7, 7.777);
  __ Vldr(d8, 8.888);
  __ Vldr(d9, 9.999);
  __ Vldr(d10, 10.000);
  __ Vldr(d11, 11.111);
  __ Vldr(d12, 12.222);
  __ Vldr(d13, 13.333);
  __ Vldr(d14, 14.444);
  __ Vldr(d15, 15.555);
  __ Vldr(d16, 16.666);
  __ Vldr(d17, 17.777);
  __ Vldr(d18, 18.888);
  __ Vldr(d19, 19.999);
  __ Vldr(d20, 20.000);
  __ Vldr(d21, 21.111);
  __ Vldr(d22, 22.222);
  __ Vldr(d23, 23.333);
  __ Vldr(d24, 24.444);
  __ Vldr(d25, 25.555);
  __ Vldr(d26, 26.666);
  __ Vldr(d27, 27.777);
  __ Vldr(d28, 28.888);
  __ Vldr(d29, 29.999);
  __ Vldr(d30, 30.000);
  __ Vldr(d31, 31.111);
  {
    UseScratchRegisterScope temps(&masm);
    // For effective use as an inspection tool, Printf must work without any
    // scratch registers.
    VIXL_CHECK(r12.Is(temps.Acquire()));
    __ Mov(r12, 0xdeadcccc);
    VIXL_CHECK(masm.GetScratchRegisterList()->IsEmpty());

    __ Printf("%% r0=%x r1=%x str=<%.*s>\n", r0, r1, r3, r2);
    __ Printf("r0=%d r1=%d str=<%s>\n", r0, r1, r2);
    __ Printf("d0=%g\n", d0);
    __ Printf("s4=%g\n", s4);
    __ Printf("d0=%g d1=%g s4=%g s5=%g\n", d0, d1, s4, s5);
    __ Printf("d0=%g r0=%x s4=%g r1=%x\n", d0, r0, s4, r1);
    __ Printf("r0=%x d0=%g r1=%x s4=%g\n", r0, d0, r1, s4);
    __ Mov(r0, sp);
    __ Printf("sp=%x\n", r0);
    __ Mrs(r0, APSR);
    // Only keep R/W fields.
    __ Mov(r2, 0xf80f0200);
    __ And(r0, r0, r2);
  }
  END();

  RUN();

  ASSERT_EQUAL_32(0xb00e0000, r0);
  ASSERT_EQUAL_32(0x5678, r1);
  ASSERT_EQUAL_32(5, r3);
  ASSERT_EQUAL_32(0xdead4444, r4);
  ASSERT_EQUAL_32(0xdead5555, r5);
  ASSERT_EQUAL_32(0xdead6666, r6);
  ASSERT_EQUAL_32(0xdead7777, r7);
  ASSERT_EQUAL_32(0xdead8888, r8);
  ASSERT_EQUAL_32(0xdead9999, r9);
  ASSERT_EQUAL_32(0xdeadaaaa, r10);
  ASSERT_EQUAL_32(0xdeadbbbb, r11);
  ASSERT_EQUAL_32(0xdeadcccc, r12);
  ASSERT_EQUAL_FP64(1.2345, d0);
  ASSERT_EQUAL_FP64(2.9876, d1);
  ASSERT_EQUAL_FP32(1.3333, s4);
  ASSERT_EQUAL_FP32(3.21, s5);
  ASSERT_EQUAL_FP64(4.444, d4);
  ASSERT_EQUAL_FP64(5.555, d5);
  ASSERT_EQUAL_FP64(6.666, d6);
  ASSERT_EQUAL_FP64(7.777, d7);
  ASSERT_EQUAL_FP64(8.888, d8);
  ASSERT_EQUAL_FP64(9.999, d9);
  ASSERT_EQUAL_FP64(10.000, d10);
  ASSERT_EQUAL_FP64(11.111, d11);
  ASSERT_EQUAL_FP64(12.222, d12);
  ASSERT_EQUAL_FP64(13.333, d13);
  ASSERT_EQUAL_FP64(14.444, d14);
  ASSERT_EQUAL_FP64(15.555, d15);
  ASSERT_EQUAL_FP64(16.666, d16);
  ASSERT_EQUAL_FP64(17.777, d17);
  ASSERT_EQUAL_FP64(18.888, d18);
  ASSERT_EQUAL_FP64(19.999, d19);
  ASSERT_EQUAL_FP64(20.000, d20);
  ASSERT_EQUAL_FP64(21.111, d21);
  ASSERT_EQUAL_FP64(22.222, d22);
  ASSERT_EQUAL_FP64(23.333, d23);
  ASSERT_EQUAL_FP64(24.444, d24);
  ASSERT_EQUAL_FP64(25.555, d25);
  ASSERT_EQUAL_FP64(26.666, d26);
  ASSERT_EQUAL_FP64(27.777, d27);
  ASSERT_EQUAL_FP64(28.888, d28);
  ASSERT_EQUAL_FP64(29.999, d29);
  ASSERT_EQUAL_FP64(30.000, d30);
  ASSERT_EQUAL_FP64(31.111, d31);

  TEARDOWN();
}

TEST(printf2) {
  SETUP();

  START();
  __ Mov(r0, 0x1234);
  __ Mov(r1, 0x5678);
  __ Vldr(d0, 1.2345);
  __ Vldr(s2, 2.9876);
  __ Printf("d0=%g d1=%g r0=%x r1=%x\n", d0, s2, r0, r1);
  END();

  RUN();

  TEARDOWN();
}


TEST(use_scratch_register_scope_v_registers) {
  SETUP();
  {
    UseScratchRegisterScope temps(&masm);
    temps.Include(VRegisterList(q0, q1, q2, q3));

    // This test assumes that low-numbered registers are allocated first. The
    // implementation is allowed to use a different strategy; if it does, the
    // test will need to be updated.
    // TODO: Write more flexible (and thorough) tests.

    VIXL_CHECK(q0.Is(temps.AcquireQ()));
    VIXL_CHECK(!temps.IsAvailable(q0));
    VIXL_CHECK(!temps.IsAvailable(d0));
    VIXL_CHECK(!temps.IsAvailable(d1));
    VIXL_CHECK(!temps.IsAvailable(s0));
    VIXL_CHECK(!temps.IsAvailable(s1));
    VIXL_CHECK(!temps.IsAvailable(s2));
    VIXL_CHECK(!temps.IsAvailable(s3));

    VIXL_CHECK(d2.Is(temps.AcquireV(64)));
    VIXL_CHECK(!temps.IsAvailable(q1));
    VIXL_CHECK(!temps.IsAvailable(d2));
    VIXL_CHECK(temps.IsAvailable(d3));
    VIXL_CHECK(!temps.IsAvailable(s4));
    VIXL_CHECK(!temps.IsAvailable(s5));
    VIXL_CHECK(temps.IsAvailable(s6));
    VIXL_CHECK(temps.IsAvailable(s7));

    VIXL_CHECK(s6.Is(temps.AcquireS()));
    VIXL_CHECK(!temps.IsAvailable(d3));
    VIXL_CHECK(!temps.IsAvailable(s6));
    VIXL_CHECK(temps.IsAvailable(s7));

    VIXL_CHECK(q2.Is(temps.AcquireV(128)));
    VIXL_CHECK(!temps.IsAvailable(q2));
    VIXL_CHECK(!temps.IsAvailable(d4));
    VIXL_CHECK(!temps.IsAvailable(d5));
    VIXL_CHECK(!temps.IsAvailable(s8));
    VIXL_CHECK(!temps.IsAvailable(s9));
    VIXL_CHECK(!temps.IsAvailable(s10));
    VIXL_CHECK(!temps.IsAvailable(s11));
    VIXL_CHECK(temps.IsAvailable(s7));

    VIXL_CHECK(d6.Is(temps.AcquireD()));
    VIXL_CHECK(!temps.IsAvailable(q3));
    VIXL_CHECK(!temps.IsAvailable(d6));
    VIXL_CHECK(temps.IsAvailable(d7));
    VIXL_CHECK(!temps.IsAvailable(s12));
    VIXL_CHECK(!temps.IsAvailable(s13));
    VIXL_CHECK(temps.IsAvailable(s14));
    VIXL_CHECK(temps.IsAvailable(s15));
    VIXL_CHECK(temps.IsAvailable(s7));

    VIXL_CHECK(s7.Is(temps.AcquireS()));
  }
  TEARDOWN();
}


TEST(scratch_register_scope_include_exclude) {
  SETUP();
  {
    UseScratchRegisterScope temps(&masm);
    temps.Include(r0, r1, r2, r3);
    temps.Include(s0, s1, d1, q1);

    VIXL_CHECK(temps.IsAvailable(r0));
    VIXL_CHECK(temps.IsAvailable(r1));
    VIXL_CHECK(temps.IsAvailable(r2));
    VIXL_CHECK(temps.IsAvailable(r3));

    VIXL_CHECK(temps.IsAvailable(s0));

    VIXL_CHECK(temps.IsAvailable(s1));

    VIXL_CHECK(temps.IsAvailable(d1));
    VIXL_CHECK(temps.IsAvailable(s2));
    VIXL_CHECK(temps.IsAvailable(s3));

    VIXL_CHECK(temps.IsAvailable(q1));
    VIXL_CHECK(temps.IsAvailable(d2));
    VIXL_CHECK(temps.IsAvailable(d3));
    VIXL_CHECK(temps.IsAvailable(s4));
    VIXL_CHECK(temps.IsAvailable(s5));
    VIXL_CHECK(temps.IsAvailable(s6));
    VIXL_CHECK(temps.IsAvailable(s7));

    // Test local exclusion.
    {
      UseScratchRegisterScope local_temps(&masm);
      local_temps.Exclude(r1, r2);
      local_temps.Exclude(s1, q1);

      VIXL_CHECK(temps.IsAvailable(r0));
      VIXL_CHECK(!temps.IsAvailable(r1));
      VIXL_CHECK(!temps.IsAvailable(r2));
      VIXL_CHECK(temps.IsAvailable(r3));

      VIXL_CHECK(temps.IsAvailable(s0));

      VIXL_CHECK(!temps.IsAvailable(s1));

      VIXL_CHECK(temps.IsAvailable(d1));
      VIXL_CHECK(temps.IsAvailable(s2));
      VIXL_CHECK(temps.IsAvailable(s3));

      VIXL_CHECK(!temps.IsAvailable(q1));
      VIXL_CHECK(!temps.IsAvailable(d2));
      VIXL_CHECK(!temps.IsAvailable(d3));
      VIXL_CHECK(!temps.IsAvailable(s4));
      VIXL_CHECK(!temps.IsAvailable(s5));
      VIXL_CHECK(!temps.IsAvailable(s6));
      VIXL_CHECK(!temps.IsAvailable(s7));
    }

    // This time, exclude part of included registers, making sure the entire
    // register gets excluded.
    {
      UseScratchRegisterScope local_temps(&masm);
      local_temps.Exclude(s2, d3);

      VIXL_CHECK(temps.IsAvailable(r0));
      VIXL_CHECK(temps.IsAvailable(r1));
      VIXL_CHECK(temps.IsAvailable(r2));
      VIXL_CHECK(temps.IsAvailable(r3));

      VIXL_CHECK(temps.IsAvailable(s0));

      VIXL_CHECK(temps.IsAvailable(s1));

      // Excluding s2 should exclude d1 but not s3.
      VIXL_CHECK(!temps.IsAvailable(d1));
      VIXL_CHECK(!temps.IsAvailable(s2));
      VIXL_CHECK(temps.IsAvailable(s3));

      // Excluding d3 should exclude q1, s7 and s6 but not d2, s5, s4.
      VIXL_CHECK(!temps.IsAvailable(q1));
      VIXL_CHECK(temps.IsAvailable(d2));
      VIXL_CHECK(!temps.IsAvailable(d3));
      VIXL_CHECK(temps.IsAvailable(s4));
      VIXL_CHECK(temps.IsAvailable(s5));
      VIXL_CHECK(!temps.IsAvailable(s6));
      VIXL_CHECK(!temps.IsAvailable(s7));
    }

    // Make sure the initial state was restored.

    VIXL_CHECK(temps.IsAvailable(r0));
    VIXL_CHECK(temps.IsAvailable(r1));
    VIXL_CHECK(temps.IsAvailable(r2));
    VIXL_CHECK(temps.IsAvailable(r3));

    VIXL_CHECK(temps.IsAvailable(s0));

    VIXL_CHECK(temps.IsAvailable(s1));

    VIXL_CHECK(temps.IsAvailable(d1));
    VIXL_CHECK(temps.IsAvailable(s2));
    VIXL_CHECK(temps.IsAvailable(s3));

    VIXL_CHECK(temps.IsAvailable(q1));
    VIXL_CHECK(temps.IsAvailable(d2));
    VIXL_CHECK(temps.IsAvailable(d3));
    VIXL_CHECK(temps.IsAvailable(s4));
    VIXL_CHECK(temps.IsAvailable(s5));
    VIXL_CHECK(temps.IsAvailable(s6));
    VIXL_CHECK(temps.IsAvailable(s7));
  }
  TEARDOWN();
}


template<typename T>
void CheckInstructionSetA32(const T& assm) {
  VIXL_CHECK(assm.IsUsingA32());
  VIXL_CHECK(!assm.IsUsingT32());
  VIXL_CHECK(assm.GetInstructionSetInUse() == A32);
}


template<typename T>
void CheckInstructionSetT32(const T& assm) {
  VIXL_CHECK(assm.IsUsingT32());
  VIXL_CHECK(!assm.IsUsingA32());
  VIXL_CHECK(assm.GetInstructionSetInUse() == T32);
}


TEST_NOASM(set_isa_constructors) {
  byte buffer[1024];

  // A32 by default.
  CheckInstructionSetA32(Assembler());
  CheckInstructionSetA32(Assembler(1024));
  CheckInstructionSetA32(Assembler(buffer, sizeof(buffer)));
  // Explicit A32.
  CheckInstructionSetA32(Assembler(A32));
  CheckInstructionSetA32(Assembler(1024, A32));
  CheckInstructionSetA32(Assembler(buffer, sizeof(buffer), A32));
  // Explicit T32.
  CheckInstructionSetT32(Assembler(T32));
  CheckInstructionSetT32(Assembler(1024, T32));
  CheckInstructionSetT32(Assembler(buffer, sizeof(buffer), T32));

  // A32 by default.
  CheckInstructionSetA32(MacroAssembler());
  CheckInstructionSetA32(MacroAssembler(1024));
  CheckInstructionSetA32(MacroAssembler(buffer, sizeof(buffer)));
  // Explicit A32.
  CheckInstructionSetA32(MacroAssembler(A32));
  CheckInstructionSetA32(MacroAssembler(1024, A32));
  CheckInstructionSetA32(MacroAssembler(buffer, sizeof(buffer), A32));
  // Explicit T32.
  CheckInstructionSetT32(MacroAssembler(T32));
  CheckInstructionSetT32(MacroAssembler(1024, T32));
  CheckInstructionSetT32(MacroAssembler(buffer, sizeof(buffer), T32));
}


TEST_NOASM(set_isa_empty) {
  // It is possible to change the instruction set if no instructions have yet
  // been generated.
  Assembler assm;
  CheckInstructionSetA32(assm);
  assm.UseT32();
  CheckInstructionSetT32(assm);
  assm.UseA32();
  CheckInstructionSetA32(assm);
  assm.UseInstructionSet(T32);
  CheckInstructionSetT32(assm);
  assm.UseInstructionSet(A32);
  CheckInstructionSetA32(assm);

  MacroAssembler masm;
  CheckInstructionSetA32(masm);
  masm.UseT32();
  CheckInstructionSetT32(masm);
  masm.UseA32();
  CheckInstructionSetA32(masm);
  masm.UseInstructionSet(T32);
  CheckInstructionSetT32(masm);
  masm.UseInstructionSet(A32);
  CheckInstructionSetA32(masm);
}


TEST_NOASM(set_isa_noop) {
  // It is possible to call a no-op UseA32/T32 or UseInstructionSet even if
  // one or more instructions have been generated.
  {
    Assembler assm(A32);
    CheckInstructionSetA32(assm);
    assm.bx(lr);
    VIXL_ASSERT(assm.GetCursorOffset() > 0);
    CheckInstructionSetA32(assm);
    assm.UseA32();
    CheckInstructionSetA32(assm);
    assm.UseInstructionSet(A32);
    CheckInstructionSetA32(assm);
    assm.FinalizeCode();
  }
  {
    Assembler assm(T32);
    CheckInstructionSetT32(assm);
    assm.bx(lr);
    VIXL_ASSERT(assm.GetCursorOffset() > 0);
    CheckInstructionSetT32(assm);
    assm.UseT32();
    CheckInstructionSetT32(assm);
    assm.UseInstructionSet(T32);
    CheckInstructionSetT32(assm);
    assm.FinalizeCode();
  }
  {
    MacroAssembler masm(A32);
    CheckInstructionSetA32(masm);
    masm.Bx(lr);
    VIXL_ASSERT(masm.GetCursorOffset() > 0);
    CheckInstructionSetA32(masm);
    masm.UseA32();
    CheckInstructionSetA32(masm);
    masm.UseInstructionSet(A32);
    CheckInstructionSetA32(masm);
    masm.FinalizeCode();
  }
  {
    MacroAssembler masm(T32);
    CheckInstructionSetT32(masm);
    masm.Bx(lr);
    VIXL_ASSERT(masm.GetCursorOffset() > 0);
    CheckInstructionSetT32(masm);
    masm.UseT32();
    CheckInstructionSetT32(masm);
    masm.UseInstructionSet(T32);
    CheckInstructionSetT32(masm);
    masm.FinalizeCode();
  }
}


TEST(logical_arithmetic_identities) {
  SETUP();

  START();

  Label blob_1;
  __ Bind(&blob_1);
  __ Add(r0, r0, 0);
  __ And(r0, r0, 0xffffffff);
  __ Bic(r0, r0, 0);
  __ Eor(r0, r0, 0);
  __ Orn(r0, r0, 0xffffffff);
  __ Orr(r0, r0, 0);
  __ Sub(r0, r0, 0);
  VIXL_ASSERT(masm.GetSizeOfCodeGeneratedSince(&blob_1) == 0);

  Label blob_2;
  __ Bind(&blob_2);
  __ Adds(r0, r0, 0);
  VIXL_ASSERT(masm.GetSizeOfCodeGeneratedSince(&blob_2) != 0);

  Label blob_3;
  __ Bind(&blob_3);
  __ Ands(r0, r0, 0);
  VIXL_ASSERT(masm.GetSizeOfCodeGeneratedSince(&blob_3) != 0);

  Label blob_4;
  __ Bind(&blob_4);
  __ Bics(r0, r0, 0);
  VIXL_ASSERT(masm.GetSizeOfCodeGeneratedSince(&blob_4) != 0);

  Label blob_5;
  __ Bind(&blob_5);
  __ Eors(r0, r0, 0);
  VIXL_ASSERT(masm.GetSizeOfCodeGeneratedSince(&blob_5) != 0);

  Label blob_6;
  __ Bind(&blob_6);
  __ Orns(r0, r0, 0);
  VIXL_ASSERT(masm.GetSizeOfCodeGeneratedSince(&blob_6) != 0);

  Label blob_7;
  __ Bind(&blob_7);
  __ Orrs(r0, r0, 0);
  VIXL_ASSERT(masm.GetSizeOfCodeGeneratedSince(&blob_7) != 0);

  Label blob_8;
  __ Bind(&blob_8);
  __ Subs(r0, r0, 0);
  VIXL_ASSERT(masm.GetSizeOfCodeGeneratedSince(&blob_8) != 0);

  __ Mov(r0, 0xbad);
  __ And(r1, r0, 0);
  __ Bic(r2, r0, 0xffffffff);
  __ Eor(r3, r0, 0xffffffff);
  __ Orn(r4, r0, 0);
  __ Orr(r5, r0, 0xffffffff);

  END();

  RUN();

  ASSERT_EQUAL_32(0xbad, r0);
  ASSERT_EQUAL_32(0, r1);
  ASSERT_EQUAL_32(0, r2);
  ASSERT_EQUAL_32(~0xbad, r3);
  ASSERT_EQUAL_32(0xffffffff, r4);
  ASSERT_EQUAL_32(0xffffffff, r5);

  TEARDOWN();
}


TEST(scratch_register_checks) {
  // It is unsafe for users to use registers that the MacroAssembler is also
  // using as scratch registers. This test checks the MacroAssembler's checking
  // mechanism itself.
  SETUP();
  {
    UseScratchRegisterScope temps(&masm);
    // 'ip' is a scratch register by default.
    VIXL_CHECK(
        masm.GetScratchRegisterList()->GetList() == (1u << ip.GetCode()));
    VIXL_CHECK(temps.IsAvailable(ip));

    // Integer registers have no complicated aliasing so
    // masm.AliasesAvailableScratchRegister(reg) == temps.IsAvailable(reg).
    for (unsigned i = 0; i < kNumberOfRegisters; i++) {
      Register reg(i);
      VIXL_CHECK(masm.AliasesAvailableScratchRegister(reg) ==
                 temps.IsAvailable(reg));
    }
  }

  TEARDOWN();
}


TEST(scratch_register_checks_v) {
  // It is unsafe for users to use registers that the MacroAssembler is also
  // using as scratch registers. This test checks the MacroAssembler's checking
  // mechanism itself.
  SETUP();
  {
    UseScratchRegisterScope temps(&masm);
    // There is no default floating-point scratch register. Add temps of various
    // sizes to check handling of aliased registers.
    VIXL_CHECK(masm.GetScratchVRegisterList()->GetList() == 0);
    temps.Include(q15);
    temps.Include(d15);
    temps.Include(s15);
    temps.Include(d4);
    temps.Include(d5);
    temps.Include(s24);
    temps.Include(s25);
    temps.Include(s26);
    temps.Include(s27);
    temps.Include(q0);
    // See VRegisterList for details of the list encoding.
    VIXL_CHECK(masm.GetScratchVRegisterList()->GetList() ==
               UINT64_C(0xf0000000cf008f0f));
    //                    |       ||  || |
    //                   q15    d15|  || q0
    //                        s24-s27 |d4-d5
    //                               s15

    // Simple checks: Included registers are available.
    VIXL_CHECK(temps.IsAvailable(q15));
    VIXL_CHECK(temps.IsAvailable(d15));
    VIXL_CHECK(temps.IsAvailable(s15));
    VIXL_CHECK(temps.IsAvailable(d4));
    VIXL_CHECK(temps.IsAvailable(d5));
    VIXL_CHECK(temps.IsAvailable(s24));
    VIXL_CHECK(temps.IsAvailable(s25));
    VIXL_CHECK(temps.IsAvailable(s26));
    VIXL_CHECK(temps.IsAvailable(s27));
    VIXL_CHECK(temps.IsAvailable(q0));

    // Each available S register should mark the corresponding D and Q registers
    // as aliasing an available scratch register.
    for (unsigned s = 0; s < kNumberOfSRegisters; s++) {
      if (temps.IsAvailable(SRegister(s))) {
        VIXL_CHECK(masm.AliasesAvailableScratchRegister(SRegister(s)));
        VIXL_CHECK(masm.AliasesAvailableScratchRegister(DRegister(s / 2)));
        VIXL_CHECK(masm.AliasesAvailableScratchRegister(QRegister(s / 4)));
      } else {
        // AliasesAvailableScratchRegiters == IsAvailable for S registers.
        VIXL_CHECK(!masm.AliasesAvailableScratchRegister(SRegister(s)));
      }
    }

    // Similar checks for high D registers.
    unsigned first_high_d_register = kNumberOfSRegisters / 2;
    for (unsigned d = first_high_d_register; d < kMaxNumberOfDRegisters; d++) {
      if (temps.IsAvailable(DRegister(d))) {
        VIXL_CHECK(masm.AliasesAvailableScratchRegister(DRegister(d)));
        VIXL_CHECK(masm.AliasesAvailableScratchRegister(QRegister(d / 2)));
      } else {
        // AliasesAvailableScratchRegiters == IsAvailable for high D registers.
        VIXL_CHECK(!masm.AliasesAvailableScratchRegister(DRegister(d)));
      }
    }
  }
  TEARDOWN();
}


TEST(nop) {
  SETUP();

  Label start;
  __ Bind(&start);
  __ Nop();
  size_t nop_size = (isa == T32) ? k16BitT32InstructionSizeInBytes
                                 : kA32InstructionSizeInBytes;
  // `MacroAssembler::Nop` must generate at least one nop.
  VIXL_CHECK(masm.GetSizeOfCodeGeneratedSince(&start) >= nop_size);

  masm.FinalizeCode();

  TEARDOWN();
}


// Check that `GetMarginBeforeLiteralEmission()` is precise.
TEST(literal_pool_margin) {
  SETUP();

  START();

  VIXL_CHECK(masm.VeneerPoolIsEmpty());
  VIXL_CHECK(masm.LiteralPoolIsEmpty());

  // Create a single literal.
  __ Ldrd(r0, r1, 0x1234567890abcdef);

  VIXL_CHECK(!masm.LiteralPoolIsEmpty());

  // Generate code to fill all the margin we have before generating the literal
  // pool.
  int32_t margin = masm.GetMarginBeforeLiteralEmission();
  int32_t end = masm.GetCursorOffset() + margin;
  {
    AssemblerAccurateScope scope(&masm,
                                 margin,
                                 CodeBufferCheckScope::kExactSize);
    // Opening the scope should not have triggered the emission of the literal
    // pool.
    VIXL_CHECK(!masm.LiteralPoolIsEmpty());
    while (masm.GetCursorOffset() < end) {
      __ nop();
    }
    VIXL_CHECK(masm.GetCursorOffset() == end);
  }

  // There should be no margin left to emit the literal pool.
  VIXL_CHECK(!masm.LiteralPoolIsEmpty());
  VIXL_CHECK(masm.GetMarginBeforeLiteralEmission() == 0);

  // So emitting a single instruction should force emission of the pool.
  __ Nop();
  VIXL_CHECK(masm.LiteralPoolIsEmpty());
  END();

  RUN();

  // Check that the literals loaded correctly.
  ASSERT_EQUAL_32(0x90abcdef, r0);
  ASSERT_EQUAL_32(0x12345678, r1);

  TEARDOWN();
}


// Check that `GetMarginBeforeVeneerEmission()` is precise.
TEST(veneer_pool_margin) {
  SETUP();

  START();

  VIXL_CHECK(masm.VeneerPoolIsEmpty());
  VIXL_CHECK(masm.LiteralPoolIsEmpty());

  // Create a single veneer.
  Label target;
  __ B(eq, &target);

  VIXL_CHECK(!masm.VeneerPoolIsEmpty());

  // Generate code to fill all the margin we have before generating the veneer
  // pool.
  int32_t margin = masm.GetMarginBeforeVeneerEmission();
  int32_t end = masm.GetCursorOffset() + margin;
  {
    AssemblerAccurateScope scope(&masm,
                                 margin,
                                 CodeBufferCheckScope::kExactSize);
    // Opening the scope should not have triggered the emission of the veneer
    // pool.
    VIXL_CHECK(!masm.VeneerPoolIsEmpty());
    while (masm.GetCursorOffset() < end) {
      __ nop();
    }
    VIXL_CHECK(masm.GetCursorOffset() == end);
  }
  // There should be no margin left to emit the veneer pool.
  VIXL_CHECK(masm.GetMarginBeforeVeneerEmission() == 0);

  // So emitting a single instruction should force emission of the pool.
  // We cannot simply check that the veneer pool is empty, because the veneer
  // emitted for the CBZ instruction above is itself tracked by the veneer
  // mechanisms. Instead, check that some 'unexpected' code is generated.
  Label check;
  __ Bind(&check);
  {
    AssemblerAccurateScope scope(&masm,
                                 2,
                                 AssemblerAccurateScope::kMaximumSize);
    // Do not actually generate any code.
  }
  VIXL_CHECK(masm.GetSizeOfCodeGeneratedSince(&check) > 0);
  __ Bind(&target);
  VIXL_CHECK(masm.VeneerPoolIsEmpty());

  END();

  RUN();

  TEARDOWN();
}


TEST(code_buffer_precise_growth) {
  static const int kBaseBufferSize = 16;
  MacroAssembler masm(kBaseBufferSize, T32);

  VIXL_CHECK(masm.GetBuffer()->GetCapacity() == kBaseBufferSize);

  {
    // Fill the buffer with nops.
    AssemblerAccurateScope scope(&masm,
                                 kBaseBufferSize,
                                 CodeBufferCheckScope::kExactSize);
    for (int i = 0; i < kBaseBufferSize; i += k16BitT32InstructionSizeInBytes) {
      __ nop();
    }
  }

  // The buffer should not have grown yet.
  VIXL_CHECK(masm.GetBuffer()->GetCapacity() == kBaseBufferSize);

  // Generating a single instruction should force the buffer to grow.
  __ Nop();

  VIXL_CHECK(masm.GetBuffer()->GetCapacity() > kBaseBufferSize);

  masm.FinalizeCode();
}


TEST_T32(out_of_space_immediately_before_PerformEnsureEmit) {
  static const int kBaseBufferSize = 16;
  MacroAssembler masm(kBaseBufferSize, T32);

  VIXL_CHECK(masm.GetBuffer()->GetCapacity() == kBaseBufferSize);

  VIXL_CHECK(masm.VeneerPoolIsEmpty());
  VIXL_CHECK(masm.LiteralPoolIsEmpty());

  // Create a veneer.
  Label target;
  __ Cbz(r0, &target);

  VIXL_CHECK(!masm.VeneerPoolIsEmpty());

  VIXL_CHECK(IsUint32(masm.GetBuffer()->GetRemainingBytes()));
  uint32_t space = static_cast<uint32_t>(masm.GetBuffer()->GetRemainingBytes());
  {
    // Fill the buffer with nops.
    AssemblerAccurateScope scope(&masm,
                                 space,
                                 CodeBufferCheckScope::kExactSize);
    for (uint32_t i = 0; i < space; i += k16BitT32InstructionSizeInBytes) {
      __ nop();
    }
  }

  VIXL_CHECK(!masm.VeneerPoolIsEmpty());

  // The buffer should not have grown yet, and there should be no space left.
  VIXL_CHECK(masm.GetBuffer()->GetCapacity() == kBaseBufferSize);
  VIXL_CHECK(masm.GetBuffer()->GetRemainingBytes() == 0);

  // Force emission of the veneer, at a point where there is no space available
  // in the buffer.
  int32_t past_cbz_range = masm.GetMarginBeforeVeneerEmission() + 1;
  masm.EnsureEmitFor(past_cbz_range);

  __ Bind(&target);

  VIXL_CHECK(masm.VeneerPoolIsEmpty());

  masm.FinalizeCode();
}


}  // namespace aarch32
}  // namespace vixl
