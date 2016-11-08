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

#define __ masm.
#define TEST(name)  TEST_(AARCH32_ASM_##name)

#define BUF_SIZE (4096)

#define ASSERT_LITERAL_POOL_SIZE(size) \
    do { VIXL_CHECK(__ GetLiteralPoolSize() == size); } while (false)

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH32
// No simulator yet.

#define SETUP() \
  MacroAssembler masm(BUF_SIZE);

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
  MacroAssembler masm(BUF_SIZE);

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
  __ Mov(r0, 0);                                                               \
  __ Msr(APSR_nzcvq, r0);

#define END()                                                                  \
  core.Dump(&masm);                                                            \
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

#define START_T32()                                                            \
  __ UseT32();                                                                 \
  START();

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


TEST(adr) {
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


// This test will emit the veneer in the middle of a macro-assembler
// instruction.
// The Mov(r1, 0x7ff80000) can't be emitted with only one instruction.
// Only one instruction doesn't, here, emit the veneer. Therefore, the
// veneer emission is only triggered when we are in the Delegate.
TEST(mov_and_veneer) {
  SETUP();

  START_T32();
  Label end;
  __ Mov(r0, 1);
  __ Cbz(r0, &end);
  // Generate enough instructions to have, after this loop, only one more
  // instruction that can be generated (Mov(r1, 0x7ff80000) needs two assembler
  // instructions).
  while (masm.GetMarginBeforeVeneerEmission() >
         static_cast<int32_t>(kMaxInstructionSizeInBytes)) {
    VIXL_CHECK(!masm.VeneerPoolIsEmpty());
    __ Mov(r1, r0);
  }
  VIXL_CHECK(!masm.VeneerPoolIsEmpty());
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


// This test will emit the literal pool in the middle of a macro-assembler
// instruction.
// The Mov(r2, 0x7ff80000) can't be emitted with only one instruction.
// Only one instruction doesn't, here, emit the pool. Therefore, the
// pool emission is only triggered when we are in the Delegate.
TEST(mov_and_literal) {
  SETUP();

  START();
  __ Ldrd(r0, r1, 0x1234567890abcdef);
  // Generate enough instructions to have, after this loop, only one more
  // instruction that can be generated (Mov(r2, 0x7ff80000) needs two assembler
  // instructions).
  while (masm.GetMarginBeforePoolEmission() >
         static_cast<int32_t>(kMaxInstructionSizeInBytes)) {
    VIXL_CHECK(!masm.LiteralPoolIsEmpty());
    __ Mov(r2, r0);
  }
  VIXL_CHECK(!masm.LiteralPoolIsEmpty());
  Label check;
  __ Bind(&check);
  __ Mov(r2, 0x7ff80000);
  VIXL_CHECK(masm.LiteralPoolIsEmpty());
  VIXL_CHECK(masm.GetSizeOfCodeGeneratedSince(&check) >
             2 * kMaxInstructionSizeInBytes);
  END();

  RUN();

  ASSERT_EQUAL_32(0x90abcdef, r0);
  ASSERT_EQUAL_32(0x12345678, r1);
  ASSERT_EQUAL_32(0x7ff80000, r2);

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


TEST(emit_literal) {
  SETUP();

  START();
  // Make sure the pool is empty.
  masm.EmitLiteralPool(MacroAssembler::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  // Create one literal pool entry.
  __ Ldrd(r0, r1, 0x1234567890abcdef);
  ASSERT_LITERAL_POOL_SIZE(8);

  // Emit code up to the maximum literal load range and ensure the pool
  // has not been emitted.
  static const int ldrd_size = 255 - 4;
  ptrdiff_t end = masm.GetBuffer()->GetCursorOffset() + ldrd_size;
  while (masm.GetBuffer()->GetCursorOffset() < end) {
    // Random instruction that does not affect the test, ie
    // an instruction that does not depend on a literal.
    // Avoid Nop as it can be skipped by the macro-assembler.
    __ Mov(r5, 0);
  }
  // Check that the pool has not been emited along the way.
  ASSERT_LITERAL_POOL_SIZE(8);
  // This extra instruction should trigger an emit of the pool.
  __ Mov(r5, 0);
  // The pool should have been emitted.
  ASSERT_LITERAL_POOL_SIZE(0);

  StringLiteral big_literal(std::string(260, 'x').c_str());
  __ Adr(r4, &big_literal);
  // This add will overflow the literal pool and force a rewind.
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

  Label past_literal0;
  Literal<uint32_t> literal0(static_cast<uint32_t>(0x12345678),
                             RawLiteral::kManuallyPlaced);
  __ Ldr(r0, &literal0);
  __ B(&past_literal0);
  __ Place(&literal0);
  __ Bind(&past_literal0);
  __ Ldr(r1, &literal0);

  ASSERT_LITERAL_POOL_SIZE(0);

  Label past_literal1;
  Literal<uint64_t> cafebeefdeadbaba(0xcafebeefdeadbaba,
                                     RawLiteral::kManuallyPlaced);
  __ B(&past_literal1);
  __ Place(&cafebeefdeadbaba);
  __ Bind(&past_literal1);
  __ Ldrd(r8, r9, &cafebeefdeadbaba);
  __ Ldrd(r2, r3, &cafebeefdeadbaba);
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


TEST(too_far_cbz) {
  SETUP();

  START_T32();
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


TEST(close_cbz) {
  SETUP();

  START_T32();
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


TEST(close_cbz2) {
  SETUP();

  START_T32();
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


TEST(not_close_cbz) {
  SETUP();

  START_T32();
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


TEST(veneers) {
  SETUP();

  START_T32();
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
TEST(veneers_labels_sort) {
  SETUP();

  START_T32();
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
                bool is_using_t32) {
  SETUP();

  if (is_using_t32) {
    START_T32();
  } else {
    START();
  }

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

  __ Default(switch_);
  __ Mov(r0, -1);

  __ EndSwitch(switch_);


  END();

  RUN();

  if (case_index < 4) {
    ASSERT_EQUAL_32(1 << case_index, r0);
  } else {
    ASSERT_EQUAL_32(-1, r0);
  }
}


TEST(switch_case_a32_8) {
  for (int i = 0; i < 8; i++) {
    JumpTable8bitOffset switch_(6);
    SwitchCase(&switch_, i, false);
  }
}


TEST(switch_case_a32_16) {
  for (int i = 0; i < 5; i++) {
    JumpTable16bitOffset switch_(5);
    SwitchCase(&switch_, i, false);
  }
}


TEST(switch_case_a32_32) {
  for (int i = 0; i < 5; i++) {
    JumpTable32bitOffset switch_(5);
    SwitchCase(&switch_, i, false);
  }
}


TEST(switch_case_t32_8) {
  for (int i = 0; i < 5; i++) {
    JumpTable8bitOffset switch_(5);
    SwitchCase(&switch_, i, true);
  }
}


TEST(switch_case_t32_16) {
  for (int i = 0; i < 5; i++) {
    JumpTable16bitOffset switch_(5);
    SwitchCase(&switch_, i, true);
  }
}


TEST(switch_case_t32_32) {
  for (int i = 0; i < 5; i++) {
    JumpTable32bitOffset switch_(5);
    SwitchCase(&switch_, i, true);
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

  START_T32();
  __ Mov(r0, 0xdead);
  __ Mov(r1, 0xdead);
  __ Mov(r2, 0xdead);
  __ Mov(r3, 0xb);
  __ Msr(APSR_nzcvqg, 0);
  __ Mrs(r0, APSR);
  __ Msr(APSR_nzcvqg, 0xffffffff);
  __ Mrs(r1, APSR);
  // Only modify nzcvq => keep previous g.
  __ Msr(APSR_nzcvq, Operand(r3, LSL, 28));
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
  __ Mov(r12, 0xdeadcccc);
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

  START();
  {
    UseScratchRegisterScope temps(&masm);
    temps.Include(VRegisterList(q0, q1, q2, q3));

    QRegister should_be_q0 = temps.AcquireQ();
    VRegister should_be_d2 = temps.AcquireV(64);
    SRegister should_be_s6 = temps.AcquireS();
    VRegister should_be_q2 = temps.AcquireV(128);
    DRegister should_be_d6 = temps.AcquireD();

    __ Mov(r0, 0x12);
    __ Mov(r1, 0x34);
    __ Mov(r2, 0x56);
    __ Mov(r3, 0x78);
    __ Mov(r4, 0x9a);

    __ Vdup(Untyped32, should_be_q0, r0);
    __ Vdup(Untyped32, should_be_d2.D(), r1);
    __ Vmov(should_be_s6, r2);
    __ Vdup(Untyped32, should_be_q2.Q(), r3);
    __ Vdup(Untyped32, should_be_d6, r4);
  }
  END();

  RUN();

  ASSERT_EQUAL_128(0x0000001200000012, 0x0000001200000012, q0);
  ASSERT_EQUAL_64(0x0000003400000034, d2);
  ASSERT_EQUAL_32(0x00000056, s6);
  ASSERT_EQUAL_128(0x0000007800000078, 0x0000007800000078, q2);
  ASSERT_EQUAL_64(0x0000009a0000009a, d6);

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


TEST(set_isa_constructors) {
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


TEST(set_isa_empty) {
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


TEST(set_isa_noop) {
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


}  // namespace aarch32
}  // namespace vixl
