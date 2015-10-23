// Copyright 2015, ARM Limited
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <cmath>

#include "test-runner.h"
#include "test-utils-a64.h"
#include "vixl/a64/macro-assembler-a64.h"
#include "vixl/a64/simulator-a64.h"
#include "vixl/a64/debugger-a64.h"
#include "vixl/a64/disasm-a64.h"
#include "vixl/a64/cpu-a64.h"

namespace vixl {

// Test infrastructure.
//
// Tests are functions which accept no parameters and have no return values.
// The testing code should not perform an explicit return once completed. For
// example to test the mov immediate instruction a very simple test would be:
//
//   TEST(mov_x0_one) {
//     SETUP();
//
//     START();
//     __ mov(x0, Operand(1));
//     END();
//
//     RUN();
//
//     ASSERT_EQUAL_64(1, x0);
//
//     TEARDOWN();
//   }
//
// Within a START ... END block all registers but sp can be modified. sp has to
// be explicitly saved/restored. The END() macro replaces the function return
// so it may appear multiple times in a test if the test has multiple exit
// points.
//
// Once the test has been run all integer and floating point registers as well
// as flags are accessible through a RegisterDump instance, see
// utils-a64.cc for more info on RegisterDump.
//
// We provide some helper assert to handle common cases:
//
//   ASSERT_EQUAL_32(int32_t, int_32t)
//   ASSERT_EQUAL_FP32(float, float)
//   ASSERT_EQUAL_32(int32_t, W register)
//   ASSERT_EQUAL_FP32(float, S register)
//   ASSERT_EQUAL_64(int64_t, int_64t)
//   ASSERT_EQUAL_FP64(double, double)
//   ASSERT_EQUAL_64(int64_t, X register)
//   ASSERT_EQUAL_64(X register, X register)
//   ASSERT_EQUAL_FP64(double, D register)
//
// e.g. ASSERT_EQUAL_64(0.5, d30);
//
// If more advanced computation is required before the assert then access the
// RegisterDump named core directly:
//
//   ASSERT_EQUAL_64(0x1234, core->reg_x0() & 0xffff);


#define __ masm.
#define TEST(name)  TEST_(ASM_##name)

#define BUF_SIZE (4096)

#ifdef VIXL_INCLUDE_SIMULATOR
// Run tests with the simulator.

#define SETUP()                                                                \
  MacroAssembler masm(BUF_SIZE);                                               \
  SETUP_COMMON()

#define SETUP_CUSTOM(size, pic)                                                \
  byte* buf = new byte[size + BUF_SIZE];                                       \
  MacroAssembler masm(buf, size + BUF_SIZE, pic);                              \
  SETUP_COMMON()

#define SETUP_COMMON()                                                         \
  masm.SetAllowSimulatorInstructions(true);                                    \
  Decoder decoder;                                                             \
  Simulator* simulator = Test::run_debugger() ? new Debugger(&decoder)         \
                                              : new Simulator(&decoder);       \
  simulator->set_coloured_trace(Test::coloured_trace());                       \
  simulator->set_instruction_stats(Test::instruction_stats());                 \
  RegisterDump core

// This is a convenience macro to avoid creating a scope for every assembler
// function called. It will still assert the buffer hasn't been exceeded.
#define ALLOW_ASM()                                                            \
  CodeBufferCheckScope guard(&masm, masm.BufferCapacity())

#define START()                                                                \
  masm.Reset();                                                                \
  simulator->ResetState();                                                     \
  __ PushCalleeSavedRegisters();                                               \
  if (Test::trace_reg()) {                                                     \
    __ Trace(LOG_STATE, TRACE_ENABLE);                                         \
  }                                                                            \
  if (Test::trace_write()) {                                                   \
    __ Trace(LOG_WRITE, TRACE_ENABLE);                                         \
  }                                                                            \
  if (Test::trace_sim()) {                                                     \
    __ Trace(LOG_DISASM, TRACE_ENABLE);                                        \
  }                                                                            \
  if (Test::instruction_stats()) {                                             \
    __ EnableInstrumentation();                                                \
  }

#define END()                                                                  \
  if (Test::instruction_stats()) {                                             \
    __ DisableInstrumentation();                                               \
  }                                                                            \
  __ Trace(LOG_ALL, TRACE_DISABLE);                                            \
  core.Dump(&masm);                                                            \
  __ PopCalleeSavedRegisters();                                                \
  __ Ret();                                                                    \
  masm.FinalizeCode()

#define RUN()                                                                  \
  simulator->RunFrom(masm.GetStartAddress<Instruction*>())

#define TEARDOWN() TEARDOWN_COMMON()

#define TEARDOWN_CUSTOM()                                                      \
  delete[] buf;                                                                \
  TEARDOWN_COMMON()

#define TEARDOWN_COMMON()                                                      \
  delete simulator;

#else  // ifdef VIXL_INCLUDE_SIMULATOR.
// Run the test on real hardware or models.
#define SETUP()                                                                \
  MacroAssembler masm(BUF_SIZE);                                               \
  SETUP_COMMON()

#define SETUP_CUSTOM(size, pic)                                                \
  byte* buf = new byte[size + BUF_SIZE];                                       \
  MacroAssembler masm(buf, size + BUF_SIZE, pic);                              \
  SETUP_COMMON()

#define SETUP_COMMON()                                                         \
  masm.SetAllowSimulatorInstructions(false);                                   \
  RegisterDump core;                                                           \
  CPU::SetUp()

// This is a convenience macro to avoid creating a scope for every assembler
// function called. It will still assert the buffer hasn't been exceeded.
#define ALLOW_ASM()                                                            \
  CodeBufferCheckScope guard(&masm, masm.BufferCapacity())

#define START()                                                                \
  masm.Reset();                                                                \
  __ PushCalleeSavedRegisters()

#define END()                                                                  \
  core.Dump(&masm);                                                            \
  __ PopCalleeSavedRegisters();                                                \
  __ Ret();                                                                    \
  masm.FinalizeCode()

#define RUN()                                                                  \
  {                                                                            \
    byte* buffer_start = masm.GetStartAddress<byte*>();                        \
    size_t buffer_length = masm.CursorOffset();                                \
    void (*test_function)(void);                                               \
                                                                               \
    CPU::EnsureIAndDCacheCoherency(buffer_start, buffer_length);               \
    VIXL_STATIC_ASSERT(sizeof(buffer_start) == sizeof(test_function));         \
    memcpy(&test_function, &buffer_start, sizeof(buffer_start));               \
    test_function();                                                           \
  }

#define TEARDOWN()

#define TEARDOWN_CUSTOM()                                                      \
  delete[] buf;                                                                \

#endif  // ifdef VIXL_INCLUDE_SIMULATOR.

#define ASSERT_EQUAL_NZCV(expected)                                            \
  assert(EqualNzcv(expected, core.flags_nzcv()))

#define ASSERT_EQUAL_REGISTERS(expected)                                       \
  assert(EqualRegisters(&expected, &core))

#define ASSERT_EQUAL_32(expected, result)                                      \
  assert(Equal32(static_cast<uint32_t>(expected), &core, result))

#define ASSERT_EQUAL_FP32(expected, result)                                    \
  assert(EqualFP32(expected, &core, result))

#define ASSERT_EQUAL_64(expected, result)                                      \
  assert(Equal64(expected, &core, result))

#define ASSERT_EQUAL_FP64(expected, result)                                    \
  assert(EqualFP64(expected, &core, result))

#define ASSERT_EQUAL_128(expected_h, expected_l, result)                       \
  assert(Equal128(expected_h, expected_l, &core, result))

#define ASSERT_LITERAL_POOL_SIZE(expected)                                     \
  assert((expected + kInstructionSize) == (masm.LiteralPoolSize()))


TEST(stack_ops) {
  SETUP();

  START();
  // save sp.
  __ Mov(x29, sp);

  // Set the sp to a known value.
  __ Mov(sp, 0x1004);
  __ Mov(x0, sp);

  // Add immediate to the sp, and move the result to a normal register.
  __ Add(sp, sp, 0x50);
  __ Mov(x1, sp);

  // Add extended to the sp, and move the result to a normal register.
  __ Mov(x17, 0xfff);
  __ Add(sp, sp, Operand(x17, SXTB));
  __ Mov(x2, sp);

  // Create an sp using a logical instruction, and move to normal register.
  __ Orr(sp, xzr, 0x1fff);
  __ Mov(x3, sp);

  // Write wsp using a logical instruction.
  __ Orr(wsp, wzr, 0xfffffff8);
  __ Mov(x4, sp);

  // Write sp, and read back wsp.
  __ Orr(sp, xzr, 0xfffffff8);
  __ Mov(w5, wsp);

  //  restore sp.
  __ Mov(sp, x29);
  END();

  RUN();

  ASSERT_EQUAL_64(0x1004, x0);
  ASSERT_EQUAL_64(0x1054, x1);
  ASSERT_EQUAL_64(0x1053, x2);
  ASSERT_EQUAL_64(0x1fff, x3);
  ASSERT_EQUAL_64(0xfffffff8, x4);
  ASSERT_EQUAL_64(0xfffffff8, x5);

  TEARDOWN();
}


TEST(mvn) {
  SETUP();

  START();
  __ Mvn(w0, 0xfff);
  __ Mvn(x1, 0xfff);
  __ Mvn(w2, Operand(w0, LSL, 1));
  __ Mvn(x3, Operand(x1, LSL, 2));
  __ Mvn(w4, Operand(w0, LSR, 3));
  __ Mvn(x5, Operand(x1, LSR, 4));
  __ Mvn(w6, Operand(w0, ASR, 11));
  __ Mvn(x7, Operand(x1, ASR, 12));
  __ Mvn(w8, Operand(w0, ROR, 13));
  __ Mvn(x9, Operand(x1, ROR, 14));
  __ Mvn(w10, Operand(w2, UXTB));
  __ Mvn(x11, Operand(x2, SXTB, 1));
  __ Mvn(w12, Operand(w2, UXTH, 2));
  __ Mvn(x13, Operand(x2, SXTH, 3));
  __ Mvn(x14, Operand(w2, UXTW, 4));
  __ Mvn(x15, Operand(w2, SXTW, 4));
  END();

  RUN();

  ASSERT_EQUAL_64(0xfffff000, x0);
  ASSERT_EQUAL_64(0xfffffffffffff000, x1);
  ASSERT_EQUAL_64(0x00001fff, x2);
  ASSERT_EQUAL_64(0x0000000000003fff, x3);
  ASSERT_EQUAL_64(0xe00001ff, x4);
  ASSERT_EQUAL_64(0xf0000000000000ff, x5);
  ASSERT_EQUAL_64(0x00000001, x6);
  ASSERT_EQUAL_64(0x0000000000000000, x7);
  ASSERT_EQUAL_64(0x7ff80000, x8);
  ASSERT_EQUAL_64(0x3ffc000000000000, x9);
  ASSERT_EQUAL_64(0xffffff00, x10);
  ASSERT_EQUAL_64(0x0000000000000001, x11);
  ASSERT_EQUAL_64(0xffff8003, x12);
  ASSERT_EQUAL_64(0xffffffffffff0007, x13);
  ASSERT_EQUAL_64(0xfffffffffffe000f, x14);
  ASSERT_EQUAL_64(0xfffffffffffe000f, x15);

  TEARDOWN();
}


TEST(mov_imm_w) {
  SETUP();

  START();
  __ Mov(w0, 0xffffffff);
  __ Mov(w1, 0xffff1234);
  __ Mov(w2, 0x1234ffff);
  __ Mov(w3, 0x00000000);
  __ Mov(w4, 0x00001234);
  __ Mov(w5, 0x12340000);
  __ Mov(w6, 0x12345678);
  __ Mov(w7, (int32_t)0x80000000);
  __ Mov(w8, (int32_t)0xffff0000);
  __ Mov(w9, kWMinInt);
  END();

  RUN();

  ASSERT_EQUAL_64(0xffffffff, x0);
  ASSERT_EQUAL_64(0xffff1234, x1);
  ASSERT_EQUAL_64(0x1234ffff, x2);
  ASSERT_EQUAL_64(0x00000000, x3);
  ASSERT_EQUAL_64(0x00001234, x4);
  ASSERT_EQUAL_64(0x12340000, x5);
  ASSERT_EQUAL_64(0x12345678, x6);
  ASSERT_EQUAL_64(0x80000000, x7);
  ASSERT_EQUAL_64(0xffff0000, x8);
  ASSERT_EQUAL_32(kWMinInt, w9);

  TEARDOWN();
}


TEST(mov_imm_x) {
  SETUP();

  START();
  __ Mov(x0, 0xffffffffffffffff);
  __ Mov(x1, 0xffffffffffff1234);
  __ Mov(x2, 0xffffffff12345678);
  __ Mov(x3, 0xffff1234ffff5678);
  __ Mov(x4, 0x1234ffffffff5678);
  __ Mov(x5, 0x1234ffff5678ffff);
  __ Mov(x6, 0x12345678ffffffff);
  __ Mov(x7, 0x1234ffffffffffff);
  __ Mov(x8, 0x123456789abcffff);
  __ Mov(x9, 0x12345678ffff9abc);
  __ Mov(x10, 0x1234ffff56789abc);
  __ Mov(x11, 0xffff123456789abc);
  __ Mov(x12, 0x0000000000000000);
  __ Mov(x13, 0x0000000000001234);
  __ Mov(x14, 0x0000000012345678);
  __ Mov(x15, 0x0000123400005678);
  __ Mov(x18, 0x1234000000005678);
  __ Mov(x19, 0x1234000056780000);
  __ Mov(x20, 0x1234567800000000);
  __ Mov(x21, 0x1234000000000000);
  __ Mov(x22, 0x123456789abc0000);
  __ Mov(x23, 0x1234567800009abc);
  __ Mov(x24, 0x1234000056789abc);
  __ Mov(x25, 0x0000123456789abc);
  __ Mov(x26, 0x123456789abcdef0);
  __ Mov(x27, 0xffff000000000001);
  __ Mov(x28, 0x8000ffff00000000);
  END();

  RUN();

  ASSERT_EQUAL_64(0xffffffffffff1234, x1);
  ASSERT_EQUAL_64(0xffffffff12345678, x2);
  ASSERT_EQUAL_64(0xffff1234ffff5678, x3);
  ASSERT_EQUAL_64(0x1234ffffffff5678, x4);
  ASSERT_EQUAL_64(0x1234ffff5678ffff, x5);
  ASSERT_EQUAL_64(0x12345678ffffffff, x6);
  ASSERT_EQUAL_64(0x1234ffffffffffff, x7);
  ASSERT_EQUAL_64(0x123456789abcffff, x8);
  ASSERT_EQUAL_64(0x12345678ffff9abc, x9);
  ASSERT_EQUAL_64(0x1234ffff56789abc, x10);
  ASSERT_EQUAL_64(0xffff123456789abc, x11);
  ASSERT_EQUAL_64(0x0000000000000000, x12);
  ASSERT_EQUAL_64(0x0000000000001234, x13);
  ASSERT_EQUAL_64(0x0000000012345678, x14);
  ASSERT_EQUAL_64(0x0000123400005678, x15);
  ASSERT_EQUAL_64(0x1234000000005678, x18);
  ASSERT_EQUAL_64(0x1234000056780000, x19);
  ASSERT_EQUAL_64(0x1234567800000000, x20);
  ASSERT_EQUAL_64(0x1234000000000000, x21);
  ASSERT_EQUAL_64(0x123456789abc0000, x22);
  ASSERT_EQUAL_64(0x1234567800009abc, x23);
  ASSERT_EQUAL_64(0x1234000056789abc, x24);
  ASSERT_EQUAL_64(0x0000123456789abc, x25);
  ASSERT_EQUAL_64(0x123456789abcdef0, x26);
  ASSERT_EQUAL_64(0xffff000000000001, x27);
  ASSERT_EQUAL_64(0x8000ffff00000000, x28);


  TEARDOWN();
}


TEST(mov) {
  SETUP();
  ALLOW_ASM();

  START();
  __ Mov(x0, 0xffffffffffffffff);
  __ Mov(x1, 0xffffffffffffffff);
  __ Mov(x2, 0xffffffffffffffff);
  __ Mov(x3, 0xffffffffffffffff);

  __ Mov(x0, 0x0123456789abcdef);

  __ movz(x1, UINT64_C(0xabcd) << 16);
  __ movk(x2, UINT64_C(0xabcd) << 32);
  __ movn(x3, UINT64_C(0xabcd) << 48);

  __ Mov(x4, 0x0123456789abcdef);
  __ Mov(x5, x4);

  __ Mov(w6, -1);

  // Test that moves back to the same register have the desired effect. This
  // is a no-op for X registers, and a truncation for W registers.
  __ Mov(x7, 0x0123456789abcdef);
  __ Mov(x7, x7);
  __ Mov(x8, 0x0123456789abcdef);
  __ Mov(w8, w8);
  __ Mov(x9, 0x0123456789abcdef);
  __ Mov(x9, Operand(x9));
  __ Mov(x10, 0x0123456789abcdef);
  __ Mov(w10, Operand(w10));

  __ Mov(w11, 0xfff);
  __ Mov(x12, 0xfff);
  __ Mov(w13, Operand(w11, LSL, 1));
  __ Mov(x14, Operand(x12, LSL, 2));
  __ Mov(w15, Operand(w11, LSR, 3));
  __ Mov(x18, Operand(x12, LSR, 4));
  __ Mov(w19, Operand(w11, ASR, 11));
  __ Mov(x20, Operand(x12, ASR, 12));
  __ Mov(w21, Operand(w11, ROR, 13));
  __ Mov(x22, Operand(x12, ROR, 14));
  __ Mov(w23, Operand(w13, UXTB));
  __ Mov(x24, Operand(x13, SXTB, 1));
  __ Mov(w25, Operand(w13, UXTH, 2));
  __ Mov(x26, Operand(x13, SXTH, 3));
  __ Mov(x27, Operand(w13, UXTW, 4));

  __ Mov(x28, 0x0123456789abcdef);
  __ Mov(w28, w28, kDiscardForSameWReg);
  END();

  RUN();

  ASSERT_EQUAL_64(0x0123456789abcdef, x0);
  ASSERT_EQUAL_64(0x00000000abcd0000, x1);
  ASSERT_EQUAL_64(0xffffabcdffffffff, x2);
  ASSERT_EQUAL_64(0x5432ffffffffffff, x3);
  ASSERT_EQUAL_64(x4, x5);
  ASSERT_EQUAL_32(-1, w6);
  ASSERT_EQUAL_64(0x0123456789abcdef, x7);
  ASSERT_EQUAL_32(0x89abcdef, w8);
  ASSERT_EQUAL_64(0x0123456789abcdef, x9);
  ASSERT_EQUAL_32(0x89abcdef, w10);
  ASSERT_EQUAL_64(0x00000fff, x11);
  ASSERT_EQUAL_64(0x0000000000000fff, x12);
  ASSERT_EQUAL_64(0x00001ffe, x13);
  ASSERT_EQUAL_64(0x0000000000003ffc, x14);
  ASSERT_EQUAL_64(0x000001ff, x15);
  ASSERT_EQUAL_64(0x00000000000000ff, x18);
  ASSERT_EQUAL_64(0x00000001, x19);
  ASSERT_EQUAL_64(0x0000000000000000, x20);
  ASSERT_EQUAL_64(0x7ff80000, x21);
  ASSERT_EQUAL_64(0x3ffc000000000000, x22);
  ASSERT_EQUAL_64(0x000000fe, x23);
  ASSERT_EQUAL_64(0xfffffffffffffffc, x24);
  ASSERT_EQUAL_64(0x00007ff8, x25);
  ASSERT_EQUAL_64(0x000000000000fff0, x26);
  ASSERT_EQUAL_64(0x000000000001ffe0, x27);
  ASSERT_EQUAL_64(0x0123456789abcdef, x28);

  TEARDOWN();
}


TEST(orr) {
  SETUP();

  START();
  __ Mov(x0, 0xf0f0);
  __ Mov(x1, 0xf00000ff);

  __ Orr(x2, x0, Operand(x1));
  __ Orr(w3, w0, Operand(w1, LSL, 28));
  __ Orr(x4, x0, Operand(x1, LSL, 32));
  __ Orr(x5, x0, Operand(x1, LSR, 4));
  __ Orr(w6, w0, Operand(w1, ASR, 4));
  __ Orr(x7, x0, Operand(x1, ASR, 4));
  __ Orr(w8, w0, Operand(w1, ROR, 12));
  __ Orr(x9, x0, Operand(x1, ROR, 12));
  __ Orr(w10, w0, 0xf);
  __ Orr(x11, x0, 0xf0000000f0000000);
  END();

  RUN();

  ASSERT_EQUAL_64(0x00000000f000f0ff, x2);
  ASSERT_EQUAL_64(0xf000f0f0, x3);
  ASSERT_EQUAL_64(0xf00000ff0000f0f0, x4);
  ASSERT_EQUAL_64(0x000000000f00f0ff, x5);
  ASSERT_EQUAL_64(0xff00f0ff, x6);
  ASSERT_EQUAL_64(0x000000000f00f0ff, x7);
  ASSERT_EQUAL_64(0x0ffff0f0, x8);
  ASSERT_EQUAL_64(0x0ff00000000ff0f0, x9);
  ASSERT_EQUAL_64(0x0000f0ff, x10);
  ASSERT_EQUAL_64(0xf0000000f000f0f0, x11);

  TEARDOWN();
}


TEST(orr_extend) {
  SETUP();

  START();
  __ Mov(x0, 1);
  __ Mov(x1, 0x8000000080008080);
  __ Orr(w6, w0, Operand(w1, UXTB));
  __ Orr(x7, x0, Operand(x1, UXTH, 1));
  __ Orr(w8, w0, Operand(w1, UXTW, 2));
  __ Orr(x9, x0, Operand(x1, UXTX, 3));
  __ Orr(w10, w0, Operand(w1, SXTB));
  __ Orr(x11, x0, Operand(x1, SXTH, 1));
  __ Orr(x12, x0, Operand(x1, SXTW, 2));
  __ Orr(x13, x0, Operand(x1, SXTX, 3));
  END();

  RUN();

  ASSERT_EQUAL_64(0x00000081, x6);
  ASSERT_EQUAL_64(0x0000000000010101, x7);
  ASSERT_EQUAL_64(0x00020201, x8);
  ASSERT_EQUAL_64(0x0000000400040401, x9);
  ASSERT_EQUAL_64(0xffffff81, x10);
  ASSERT_EQUAL_64(0xffffffffffff0101, x11);
  ASSERT_EQUAL_64(0xfffffffe00020201, x12);
  ASSERT_EQUAL_64(0x0000000400040401, x13);

  TEARDOWN();
}


TEST(bitwise_wide_imm) {
  SETUP();

  START();
  __ Mov(x0, 0);
  __ Mov(x1, 0xf0f0f0f0f0f0f0f0);

  __ Orr(x10, x0, 0x1234567890abcdef);
  __ Orr(w11, w1, 0x90abcdef);

  __ Orr(w12, w0, kWMinInt);
  __ Eor(w13, w0, kWMinInt);
  END();

  RUN();

  ASSERT_EQUAL_64(0, x0);
  ASSERT_EQUAL_64(0xf0f0f0f0f0f0f0f0, x1);
  ASSERT_EQUAL_64(0x1234567890abcdef, x10);
  ASSERT_EQUAL_64(0x00000000f0fbfdff, x11);
  ASSERT_EQUAL_32(kWMinInt, w12);
  ASSERT_EQUAL_32(kWMinInt, w13);

  TEARDOWN();
}


TEST(orn) {
  SETUP();

  START();
  __ Mov(x0, 0xf0f0);
  __ Mov(x1, 0xf00000ff);

  __ Orn(x2, x0, Operand(x1));
  __ Orn(w3, w0, Operand(w1, LSL, 4));
  __ Orn(x4, x0, Operand(x1, LSL, 4));
  __ Orn(x5, x0, Operand(x1, LSR, 1));
  __ Orn(w6, w0, Operand(w1, ASR, 1));
  __ Orn(x7, x0, Operand(x1, ASR, 1));
  __ Orn(w8, w0, Operand(w1, ROR, 16));
  __ Orn(x9, x0, Operand(x1, ROR, 16));
  __ Orn(w10, w0, 0x0000ffff);
  __ Orn(x11, x0, 0x0000ffff0000ffff);
  END();

  RUN();

  ASSERT_EQUAL_64(0xffffffff0ffffff0, x2);
  ASSERT_EQUAL_64(0xfffff0ff, x3);
  ASSERT_EQUAL_64(0xfffffff0fffff0ff, x4);
  ASSERT_EQUAL_64(0xffffffff87fffff0, x5);
  ASSERT_EQUAL_64(0x07fffff0, x6);
  ASSERT_EQUAL_64(0xffffffff87fffff0, x7);
  ASSERT_EQUAL_64(0xff00ffff, x8);
  ASSERT_EQUAL_64(0xff00ffffffffffff, x9);
  ASSERT_EQUAL_64(0xfffff0f0, x10);
  ASSERT_EQUAL_64(0xffff0000fffff0f0, x11);

  TEARDOWN();
}


TEST(orn_extend) {
  SETUP();

  START();
  __ Mov(x0, 1);
  __ Mov(x1, 0x8000000080008081);
  __ Orn(w6, w0, Operand(w1, UXTB));
  __ Orn(x7, x0, Operand(x1, UXTH, 1));
  __ Orn(w8, w0, Operand(w1, UXTW, 2));
  __ Orn(x9, x0, Operand(x1, UXTX, 3));
  __ Orn(w10, w0, Operand(w1, SXTB));
  __ Orn(x11, x0, Operand(x1, SXTH, 1));
  __ Orn(x12, x0, Operand(x1, SXTW, 2));
  __ Orn(x13, x0, Operand(x1, SXTX, 3));
  END();

  RUN();

  ASSERT_EQUAL_64(0xffffff7f, x6);
  ASSERT_EQUAL_64(0xfffffffffffefefd, x7);
  ASSERT_EQUAL_64(0xfffdfdfb, x8);
  ASSERT_EQUAL_64(0xfffffffbfffbfbf7, x9);
  ASSERT_EQUAL_64(0x0000007f, x10);
  ASSERT_EQUAL_64(0x000000000000fefd, x11);
  ASSERT_EQUAL_64(0x00000001fffdfdfb, x12);
  ASSERT_EQUAL_64(0xfffffffbfffbfbf7, x13);

  TEARDOWN();
}


TEST(and_) {
  SETUP();

  START();
  __ Mov(x0, 0xfff0);
  __ Mov(x1, 0xf00000ff);

  __ And(x2, x0, Operand(x1));
  __ And(w3, w0, Operand(w1, LSL, 4));
  __ And(x4, x0, Operand(x1, LSL, 4));
  __ And(x5, x0, Operand(x1, LSR, 1));
  __ And(w6, w0, Operand(w1, ASR, 20));
  __ And(x7, x0, Operand(x1, ASR, 20));
  __ And(w8, w0, Operand(w1, ROR, 28));
  __ And(x9, x0, Operand(x1, ROR, 28));
  __ And(w10, w0, Operand(0xff00));
  __ And(x11, x0, Operand(0xff));
  END();

  RUN();

  ASSERT_EQUAL_64(0x000000f0, x2);
  ASSERT_EQUAL_64(0x00000ff0, x3);
  ASSERT_EQUAL_64(0x00000ff0, x4);
  ASSERT_EQUAL_64(0x00000070, x5);
  ASSERT_EQUAL_64(0x0000ff00, x6);
  ASSERT_EQUAL_64(0x00000f00, x7);
  ASSERT_EQUAL_64(0x00000ff0, x8);
  ASSERT_EQUAL_64(0x00000000, x9);
  ASSERT_EQUAL_64(0x0000ff00, x10);
  ASSERT_EQUAL_64(0x000000f0, x11);

  TEARDOWN();
}


TEST(and_extend) {
  SETUP();

  START();
  __ Mov(x0, 0xffffffffffffffff);
  __ Mov(x1, 0x8000000080008081);
  __ And(w6, w0, Operand(w1, UXTB));
  __ And(x7, x0, Operand(x1, UXTH, 1));
  __ And(w8, w0, Operand(w1, UXTW, 2));
  __ And(x9, x0, Operand(x1, UXTX, 3));
  __ And(w10, w0, Operand(w1, SXTB));
  __ And(x11, x0, Operand(x1, SXTH, 1));
  __ And(x12, x0, Operand(x1, SXTW, 2));
  __ And(x13, x0, Operand(x1, SXTX, 3));
  END();

  RUN();

  ASSERT_EQUAL_64(0x00000081, x6);
  ASSERT_EQUAL_64(0x0000000000010102, x7);
  ASSERT_EQUAL_64(0x00020204, x8);
  ASSERT_EQUAL_64(0x0000000400040408, x9);
  ASSERT_EQUAL_64(0xffffff81, x10);
  ASSERT_EQUAL_64(0xffffffffffff0102, x11);
  ASSERT_EQUAL_64(0xfffffffe00020204, x12);
  ASSERT_EQUAL_64(0x0000000400040408, x13);

  TEARDOWN();
}


TEST(ands) {
  SETUP();

  START();
  __ Mov(x1, 0xf00000ff);
  __ Ands(w0, w1, Operand(w1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NFlag);
  ASSERT_EQUAL_64(0xf00000ff, x0);

  START();
  __ Mov(x0, 0xfff0);
  __ Mov(x1, 0xf00000ff);
  __ Ands(w0, w0, Operand(w1, LSR, 4));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZFlag);
  ASSERT_EQUAL_64(0x00000000, x0);

  START();
  __ Mov(x0, 0x8000000000000000);
  __ Mov(x1, 0x00000001);
  __ Ands(x0, x0, Operand(x1, ROR, 1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NFlag);
  ASSERT_EQUAL_64(0x8000000000000000, x0);

  START();
  __ Mov(x0, 0xfff0);
  __ Ands(w0, w0, Operand(0xf));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZFlag);
  ASSERT_EQUAL_64(0x00000000, x0);

  START();
  __ Mov(x0, 0xff000000);
  __ Ands(w0, w0, Operand(0x80000000));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NFlag);
  ASSERT_EQUAL_64(0x80000000, x0);

  TEARDOWN();
}


TEST(bic) {
  SETUP();

  START();
  __ Mov(x0, 0xfff0);
  __ Mov(x1, 0xf00000ff);

  __ Bic(x2, x0, Operand(x1));
  __ Bic(w3, w0, Operand(w1, LSL, 4));
  __ Bic(x4, x0, Operand(x1, LSL, 4));
  __ Bic(x5, x0, Operand(x1, LSR, 1));
  __ Bic(w6, w0, Operand(w1, ASR, 20));
  __ Bic(x7, x0, Operand(x1, ASR, 20));
  __ Bic(w8, w0, Operand(w1, ROR, 28));
  __ Bic(x9, x0, Operand(x1, ROR, 24));
  __ Bic(x10, x0, Operand(0x1f));
  __ Bic(x11, x0, Operand(0x100));

  // Test bic into sp when the constant cannot be encoded in the immediate
  // field.
  // Use x20 to preserve sp. We check for the result via x21 because the
  // test infrastructure requires that sp be restored to its original value.
  __ Mov(x20, sp);
  __ Mov(x0, 0xffffff);
  __ Bic(sp, x0, Operand(0xabcdef));
  __ Mov(x21, sp);
  __ Mov(sp, x20);
  END();

  RUN();

  ASSERT_EQUAL_64(0x0000ff00, x2);
  ASSERT_EQUAL_64(0x0000f000, x3);
  ASSERT_EQUAL_64(0x0000f000, x4);
  ASSERT_EQUAL_64(0x0000ff80, x5);
  ASSERT_EQUAL_64(0x000000f0, x6);
  ASSERT_EQUAL_64(0x0000f0f0, x7);
  ASSERT_EQUAL_64(0x0000f000, x8);
  ASSERT_EQUAL_64(0x0000ff00, x9);
  ASSERT_EQUAL_64(0x0000ffe0, x10);
  ASSERT_EQUAL_64(0x0000fef0, x11);

  ASSERT_EQUAL_64(0x543210, x21);

  TEARDOWN();
}


TEST(bic_extend) {
  SETUP();

  START();
  __ Mov(x0, 0xffffffffffffffff);
  __ Mov(x1, 0x8000000080008081);
  __ Bic(w6, w0, Operand(w1, UXTB));
  __ Bic(x7, x0, Operand(x1, UXTH, 1));
  __ Bic(w8, w0, Operand(w1, UXTW, 2));
  __ Bic(x9, x0, Operand(x1, UXTX, 3));
  __ Bic(w10, w0, Operand(w1, SXTB));
  __ Bic(x11, x0, Operand(x1, SXTH, 1));
  __ Bic(x12, x0, Operand(x1, SXTW, 2));
  __ Bic(x13, x0, Operand(x1, SXTX, 3));
  END();

  RUN();

  ASSERT_EQUAL_64(0xffffff7e, x6);
  ASSERT_EQUAL_64(0xfffffffffffefefd, x7);
  ASSERT_EQUAL_64(0xfffdfdfb, x8);
  ASSERT_EQUAL_64(0xfffffffbfffbfbf7, x9);
  ASSERT_EQUAL_64(0x0000007e, x10);
  ASSERT_EQUAL_64(0x000000000000fefd, x11);
  ASSERT_EQUAL_64(0x00000001fffdfdfb, x12);
  ASSERT_EQUAL_64(0xfffffffbfffbfbf7, x13);

  TEARDOWN();
}


TEST(bics) {
  SETUP();

  START();
  __ Mov(x1, 0xffff);
  __ Bics(w0, w1, Operand(w1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZFlag);
  ASSERT_EQUAL_64(0x00000000, x0);

  START();
  __ Mov(x0, 0xffffffff);
  __ Bics(w0, w0, Operand(w0, LSR, 1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NFlag);
  ASSERT_EQUAL_64(0x80000000, x0);

  START();
  __ Mov(x0, 0x8000000000000000);
  __ Mov(x1, 0x00000001);
  __ Bics(x0, x0, Operand(x1, ROR, 1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZFlag);
  ASSERT_EQUAL_64(0x00000000, x0);

  START();
  __ Mov(x0, 0xffffffffffffffff);
  __ Bics(x0, x0, 0x7fffffffffffffff);
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NFlag);
  ASSERT_EQUAL_64(0x8000000000000000, x0);

  START();
  __ Mov(w0, 0xffff0000);
  __ Bics(w0, w0, 0xfffffff0);
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZFlag);
  ASSERT_EQUAL_64(0x00000000, x0);

  TEARDOWN();
}


TEST(eor) {
  SETUP();

  START();
  __ Mov(x0, 0xfff0);
  __ Mov(x1, 0xf00000ff);

  __ Eor(x2, x0, Operand(x1));
  __ Eor(w3, w0, Operand(w1, LSL, 4));
  __ Eor(x4, x0, Operand(x1, LSL, 4));
  __ Eor(x5, x0, Operand(x1, LSR, 1));
  __ Eor(w6, w0, Operand(w1, ASR, 20));
  __ Eor(x7, x0, Operand(x1, ASR, 20));
  __ Eor(w8, w0, Operand(w1, ROR, 28));
  __ Eor(x9, x0, Operand(x1, ROR, 28));
  __ Eor(w10, w0, 0xff00ff00);
  __ Eor(x11, x0, 0xff00ff00ff00ff00);
  END();

  RUN();

  ASSERT_EQUAL_64(0x00000000f000ff0f, x2);
  ASSERT_EQUAL_64(0x0000f000, x3);
  ASSERT_EQUAL_64(0x0000000f0000f000, x4);
  ASSERT_EQUAL_64(0x000000007800ff8f, x5);
  ASSERT_EQUAL_64(0xffff00f0, x6);
  ASSERT_EQUAL_64(0x000000000000f0f0, x7);
  ASSERT_EQUAL_64(0x0000f00f, x8);
  ASSERT_EQUAL_64(0x00000ff00000ffff, x9);
  ASSERT_EQUAL_64(0xff0000f0, x10);
  ASSERT_EQUAL_64(0xff00ff00ff0000f0, x11);

  TEARDOWN();
}

TEST(eor_extend) {
  SETUP();

  START();
  __ Mov(x0, 0x1111111111111111);
  __ Mov(x1, 0x8000000080008081);
  __ Eor(w6, w0, Operand(w1, UXTB));
  __ Eor(x7, x0, Operand(x1, UXTH, 1));
  __ Eor(w8, w0, Operand(w1, UXTW, 2));
  __ Eor(x9, x0, Operand(x1, UXTX, 3));
  __ Eor(w10, w0, Operand(w1, SXTB));
  __ Eor(x11, x0, Operand(x1, SXTH, 1));
  __ Eor(x12, x0, Operand(x1, SXTW, 2));
  __ Eor(x13, x0, Operand(x1, SXTX, 3));
  END();

  RUN();

  ASSERT_EQUAL_64(0x11111190, x6);
  ASSERT_EQUAL_64(0x1111111111101013, x7);
  ASSERT_EQUAL_64(0x11131315, x8);
  ASSERT_EQUAL_64(0x1111111511151519, x9);
  ASSERT_EQUAL_64(0xeeeeee90, x10);
  ASSERT_EQUAL_64(0xeeeeeeeeeeee1013, x11);
  ASSERT_EQUAL_64(0xeeeeeeef11131315, x12);
  ASSERT_EQUAL_64(0x1111111511151519, x13);

  TEARDOWN();
}


TEST(eon) {
  SETUP();

  START();
  __ Mov(x0, 0xfff0);
  __ Mov(x1, 0xf00000ff);

  __ Eon(x2, x0, Operand(x1));
  __ Eon(w3, w0, Operand(w1, LSL, 4));
  __ Eon(x4, x0, Operand(x1, LSL, 4));
  __ Eon(x5, x0, Operand(x1, LSR, 1));
  __ Eon(w6, w0, Operand(w1, ASR, 20));
  __ Eon(x7, x0, Operand(x1, ASR, 20));
  __ Eon(w8, w0, Operand(w1, ROR, 28));
  __ Eon(x9, x0, Operand(x1, ROR, 28));
  __ Eon(w10, w0, 0x03c003c0);
  __ Eon(x11, x0, 0x0000100000001000);
  END();

  RUN();

  ASSERT_EQUAL_64(0xffffffff0fff00f0, x2);
  ASSERT_EQUAL_64(0xffff0fff, x3);
  ASSERT_EQUAL_64(0xfffffff0ffff0fff, x4);
  ASSERT_EQUAL_64(0xffffffff87ff0070, x5);
  ASSERT_EQUAL_64(0x0000ff0f, x6);
  ASSERT_EQUAL_64(0xffffffffffff0f0f, x7);
  ASSERT_EQUAL_64(0xffff0ff0, x8);
  ASSERT_EQUAL_64(0xfffff00fffff0000, x9);
  ASSERT_EQUAL_64(0xfc3f03cf, x10);
  ASSERT_EQUAL_64(0xffffefffffff100f, x11);

  TEARDOWN();
}


TEST(eon_extend) {
  SETUP();

  START();
  __ Mov(x0, 0x1111111111111111);
  __ Mov(x1, 0x8000000080008081);
  __ Eon(w6, w0, Operand(w1, UXTB));
  __ Eon(x7, x0, Operand(x1, UXTH, 1));
  __ Eon(w8, w0, Operand(w1, UXTW, 2));
  __ Eon(x9, x0, Operand(x1, UXTX, 3));
  __ Eon(w10, w0, Operand(w1, SXTB));
  __ Eon(x11, x0, Operand(x1, SXTH, 1));
  __ Eon(x12, x0, Operand(x1, SXTW, 2));
  __ Eon(x13, x0, Operand(x1, SXTX, 3));
  END();

  RUN();

  ASSERT_EQUAL_64(0xeeeeee6f, x6);
  ASSERT_EQUAL_64(0xeeeeeeeeeeefefec, x7);
  ASSERT_EQUAL_64(0xeeececea, x8);
  ASSERT_EQUAL_64(0xeeeeeeeaeeeaeae6, x9);
  ASSERT_EQUAL_64(0x1111116f, x10);
  ASSERT_EQUAL_64(0x111111111111efec, x11);
  ASSERT_EQUAL_64(0x11111110eeececea, x12);
  ASSERT_EQUAL_64(0xeeeeeeeaeeeaeae6, x13);

  TEARDOWN();
}


TEST(mul) {
  SETUP();

  START();
  __ Mov(x25, 0);
  __ Mov(x26, 1);
  __ Mov(x18, 0xffffffff);
  __ Mov(x19, 0xffffffffffffffff);

  __ Mul(w0, w25, w25);
  __ Mul(w1, w25, w26);
  __ Mul(w2, w26, w18);
  __ Mul(w3, w18, w19);
  __ Mul(x4, x25, x25);
  __ Mul(x5, x26, x18);
  __ Mul(x6, x18, x19);
  __ Mul(x7, x19, x19);
  __ Smull(x8, w26, w18);
  __ Smull(x9, w18, w18);
  __ Smull(x10, w19, w19);
  __ Mneg(w11, w25, w25);
  __ Mneg(w12, w25, w26);
  __ Mneg(w13, w26, w18);
  __ Mneg(w14, w18, w19);
  __ Mneg(x20, x25, x25);
  __ Mneg(x21, x26, x18);
  __ Mneg(x22, x18, x19);
  __ Mneg(x23, x19, x19);
  END();

  RUN();

  ASSERT_EQUAL_64(0, x0);
  ASSERT_EQUAL_64(0, x1);
  ASSERT_EQUAL_64(0xffffffff, x2);
  ASSERT_EQUAL_64(1, x3);
  ASSERT_EQUAL_64(0, x4);
  ASSERT_EQUAL_64(0xffffffff, x5);
  ASSERT_EQUAL_64(0xffffffff00000001, x6);
  ASSERT_EQUAL_64(1, x7);
  ASSERT_EQUAL_64(0xffffffffffffffff, x8);
  ASSERT_EQUAL_64(1, x9);
  ASSERT_EQUAL_64(1, x10);
  ASSERT_EQUAL_64(0, x11);
  ASSERT_EQUAL_64(0, x12);
  ASSERT_EQUAL_64(1, x13);
  ASSERT_EQUAL_64(0xffffffff, x14);
  ASSERT_EQUAL_64(0, x20);
  ASSERT_EQUAL_64(0xffffffff00000001, x21);
  ASSERT_EQUAL_64(0xffffffff, x22);
  ASSERT_EQUAL_64(0xffffffffffffffff, x23);

  TEARDOWN();
}


static void SmullHelper(int64_t expected, int64_t a, int64_t b) {
  SETUP();
  START();
  __ Mov(w0, a);
  __ Mov(w1, b);
  __ Smull(x2, w0, w1);
  END();
  RUN();
  ASSERT_EQUAL_64(expected, x2);
  TEARDOWN();
}


TEST(smull) {
  SmullHelper(0, 0, 0);
  SmullHelper(1, 1, 1);
  SmullHelper(-1, -1, 1);
  SmullHelper(1, -1, -1);
  SmullHelper(0xffffffff80000000, 0x80000000, 1);
  SmullHelper(0x0000000080000000, 0x00010000, 0x00008000);
}


TEST(madd) {
  SETUP();

  START();
  __ Mov(x16, 0);
  __ Mov(x17, 1);
  __ Mov(x18, 0xffffffff);
  __ Mov(x19, 0xffffffffffffffff);

  __ Madd(w0, w16, w16, w16);
  __ Madd(w1, w16, w16, w17);
  __ Madd(w2, w16, w16, w18);
  __ Madd(w3, w16, w16, w19);
  __ Madd(w4, w16, w17, w17);
  __ Madd(w5, w17, w17, w18);
  __ Madd(w6, w17, w17, w19);
  __ Madd(w7, w17, w18, w16);
  __ Madd(w8, w17, w18, w18);
  __ Madd(w9, w18, w18, w17);
  __ Madd(w10, w18, w19, w18);
  __ Madd(w11, w19, w19, w19);

  __ Madd(x12, x16, x16, x16);
  __ Madd(x13, x16, x16, x17);
  __ Madd(x14, x16, x16, x18);
  __ Madd(x15, x16, x16, x19);
  __ Madd(x20, x16, x17, x17);
  __ Madd(x21, x17, x17, x18);
  __ Madd(x22, x17, x17, x19);
  __ Madd(x23, x17, x18, x16);
  __ Madd(x24, x17, x18, x18);
  __ Madd(x25, x18, x18, x17);
  __ Madd(x26, x18, x19, x18);
  __ Madd(x27, x19, x19, x19);

  END();

  RUN();

  ASSERT_EQUAL_64(0, x0);
  ASSERT_EQUAL_64(1, x1);
  ASSERT_EQUAL_64(0xffffffff, x2);
  ASSERT_EQUAL_64(0xffffffff, x3);
  ASSERT_EQUAL_64(1, x4);
  ASSERT_EQUAL_64(0, x5);
  ASSERT_EQUAL_64(0, x6);
  ASSERT_EQUAL_64(0xffffffff, x7);
  ASSERT_EQUAL_64(0xfffffffe, x8);
  ASSERT_EQUAL_64(2, x9);
  ASSERT_EQUAL_64(0, x10);
  ASSERT_EQUAL_64(0, x11);

  ASSERT_EQUAL_64(0, x12);
  ASSERT_EQUAL_64(1, x13);
  ASSERT_EQUAL_64(0x00000000ffffffff, x14);
  ASSERT_EQUAL_64(0xffffffffffffffff, x15);
  ASSERT_EQUAL_64(1, x20);
  ASSERT_EQUAL_64(0x0000000100000000, x21);
  ASSERT_EQUAL_64(0, x22);
  ASSERT_EQUAL_64(0x00000000ffffffff, x23);
  ASSERT_EQUAL_64(0x00000001fffffffe, x24);
  ASSERT_EQUAL_64(0xfffffffe00000002, x25);
  ASSERT_EQUAL_64(0, x26);
  ASSERT_EQUAL_64(0, x27);

  TEARDOWN();
}


TEST(msub) {
  SETUP();

  START();
  __ Mov(x16, 0);
  __ Mov(x17, 1);
  __ Mov(x18, 0xffffffff);
  __ Mov(x19, 0xffffffffffffffff);

  __ Msub(w0, w16, w16, w16);
  __ Msub(w1, w16, w16, w17);
  __ Msub(w2, w16, w16, w18);
  __ Msub(w3, w16, w16, w19);
  __ Msub(w4, w16, w17, w17);
  __ Msub(w5, w17, w17, w18);
  __ Msub(w6, w17, w17, w19);
  __ Msub(w7, w17, w18, w16);
  __ Msub(w8, w17, w18, w18);
  __ Msub(w9, w18, w18, w17);
  __ Msub(w10, w18, w19, w18);
  __ Msub(w11, w19, w19, w19);

  __ Msub(x12, x16, x16, x16);
  __ Msub(x13, x16, x16, x17);
  __ Msub(x14, x16, x16, x18);
  __ Msub(x15, x16, x16, x19);
  __ Msub(x20, x16, x17, x17);
  __ Msub(x21, x17, x17, x18);
  __ Msub(x22, x17, x17, x19);
  __ Msub(x23, x17, x18, x16);
  __ Msub(x24, x17, x18, x18);
  __ Msub(x25, x18, x18, x17);
  __ Msub(x26, x18, x19, x18);
  __ Msub(x27, x19, x19, x19);

  END();

  RUN();

  ASSERT_EQUAL_64(0, x0);
  ASSERT_EQUAL_64(1, x1);
  ASSERT_EQUAL_64(0xffffffff, x2);
  ASSERT_EQUAL_64(0xffffffff, x3);
  ASSERT_EQUAL_64(1, x4);
  ASSERT_EQUAL_64(0xfffffffe, x5);
  ASSERT_EQUAL_64(0xfffffffe, x6);
  ASSERT_EQUAL_64(1, x7);
  ASSERT_EQUAL_64(0, x8);
  ASSERT_EQUAL_64(0, x9);
  ASSERT_EQUAL_64(0xfffffffe, x10);
  ASSERT_EQUAL_64(0xfffffffe, x11);

  ASSERT_EQUAL_64(0, x12);
  ASSERT_EQUAL_64(1, x13);
  ASSERT_EQUAL_64(0x00000000ffffffff, x14);
  ASSERT_EQUAL_64(0xffffffffffffffff, x15);
  ASSERT_EQUAL_64(1, x20);
  ASSERT_EQUAL_64(0x00000000fffffffe, x21);
  ASSERT_EQUAL_64(0xfffffffffffffffe, x22);
  ASSERT_EQUAL_64(0xffffffff00000001, x23);
  ASSERT_EQUAL_64(0, x24);
  ASSERT_EQUAL_64(0x0000000200000000, x25);
  ASSERT_EQUAL_64(0x00000001fffffffe, x26);
  ASSERT_EQUAL_64(0xfffffffffffffffe, x27);

  TEARDOWN();
}


TEST(smulh) {
  SETUP();

  START();
  __ Mov(x20, 0);
  __ Mov(x21, 1);
  __ Mov(x22, 0x0000000100000000);
  __ Mov(x23, 0x0000000012345678);
  __ Mov(x24, 0x0123456789abcdef);
  __ Mov(x25, 0x0000000200000000);
  __ Mov(x26, 0x8000000000000000);
  __ Mov(x27, 0xffffffffffffffff);
  __ Mov(x28, 0x5555555555555555);
  __ Mov(x29, 0xaaaaaaaaaaaaaaaa);

  __ Smulh(x0, x20, x24);
  __ Smulh(x1, x21, x24);
  __ Smulh(x2, x22, x23);
  __ Smulh(x3, x22, x24);
  __ Smulh(x4, x24, x25);
  __ Smulh(x5, x23, x27);
  __ Smulh(x6, x26, x26);
  __ Smulh(x7, x26, x27);
  __ Smulh(x8, x27, x27);
  __ Smulh(x9, x28, x28);
  __ Smulh(x10, x28, x29);
  __ Smulh(x11, x29, x29);
  END();

  RUN();

  ASSERT_EQUAL_64(0, x0);
  ASSERT_EQUAL_64(0, x1);
  ASSERT_EQUAL_64(0, x2);
  ASSERT_EQUAL_64(0x0000000001234567, x3);
  ASSERT_EQUAL_64(0x0000000002468acf, x4);
  ASSERT_EQUAL_64(0xffffffffffffffff, x5);
  ASSERT_EQUAL_64(0x4000000000000000, x6);
  ASSERT_EQUAL_64(0, x7);
  ASSERT_EQUAL_64(0, x8);
  ASSERT_EQUAL_64(0x1c71c71c71c71c71, x9);
  ASSERT_EQUAL_64(0xe38e38e38e38e38e, x10);
  ASSERT_EQUAL_64(0x1c71c71c71c71c72, x11);

  TEARDOWN();
}


TEST(umulh) {
  SETUP();

  START();
  __ Mov(x20, 0);
  __ Mov(x21, 1);
  __ Mov(x22, 0x0000000100000000);
  __ Mov(x23, 0x0000000012345678);
  __ Mov(x24, 0x0123456789abcdef);
  __ Mov(x25, 0x0000000200000000);
  __ Mov(x26, 0x8000000000000000);
  __ Mov(x27, 0xffffffffffffffff);
  __ Mov(x28, 0x5555555555555555);
  __ Mov(x29, 0xaaaaaaaaaaaaaaaa);

  __ Umulh(x0, x20, x24);
  __ Umulh(x1, x21, x24);
  __ Umulh(x2, x22, x23);
  __ Umulh(x3, x22, x24);
  __ Umulh(x4, x24, x25);
  __ Umulh(x5, x23, x27);
  __ Umulh(x6, x26, x26);
  __ Umulh(x7, x26, x27);
  __ Umulh(x8, x27, x27);
  __ Umulh(x9, x28, x28);
  __ Umulh(x10, x28, x29);
  __ Umulh(x11, x29, x29);
  END();

  RUN();

  ASSERT_EQUAL_64(0, x0);
  ASSERT_EQUAL_64(0, x1);
  ASSERT_EQUAL_64(0, x2);
  ASSERT_EQUAL_64(0x0000000001234567, x3);
  ASSERT_EQUAL_64(0x0000000002468acf, x4);
  ASSERT_EQUAL_64(0x0000000012345677, x5);
  ASSERT_EQUAL_64(0x4000000000000000, x6);
  ASSERT_EQUAL_64(0x7fffffffffffffff, x7);
  ASSERT_EQUAL_64(0xfffffffffffffffe, x8);
  ASSERT_EQUAL_64(0x1c71c71c71c71c71, x9);
  ASSERT_EQUAL_64(0x38e38e38e38e38e3, x10);
  ASSERT_EQUAL_64(0x71c71c71c71c71c6, x11);

  TEARDOWN();
}


TEST(smaddl_umaddl_umull) {
  SETUP();

  START();
  __ Mov(x17, 1);
  __ Mov(x18, 0x00000000ffffffff);
  __ Mov(x19, 0xffffffffffffffff);
  __ Mov(x20, 4);
  __ Mov(x21, 0x0000000200000000);

  __ Smaddl(x9, w17, w18, x20);
  __ Smaddl(x10, w18, w18, x20);
  __ Smaddl(x11, w19, w19, x20);
  __ Smaddl(x12, w19, w19, x21);
  __ Umaddl(x13, w17, w18, x20);
  __ Umaddl(x14, w18, w18, x20);
  __ Umaddl(x15, w19, w19, x20);
  __ Umaddl(x22, w19, w19, x21);
  __ Umull(x24, w19, w19);
  __ Umull(x25, w17, w18);
  END();

  RUN();

  ASSERT_EQUAL_64(3, x9);
  ASSERT_EQUAL_64(5, x10);
  ASSERT_EQUAL_64(5, x11);
  ASSERT_EQUAL_64(0x0000000200000001, x12);
  ASSERT_EQUAL_64(0x0000000100000003, x13);
  ASSERT_EQUAL_64(0xfffffffe00000005, x14);
  ASSERT_EQUAL_64(0xfffffffe00000005, x15);
  ASSERT_EQUAL_64(1, x22);
  ASSERT_EQUAL_64(0xfffffffe00000001, x24);
  ASSERT_EQUAL_64(0x00000000ffffffff, x25);

  TEARDOWN();
}


TEST(smsubl_umsubl) {
  SETUP();

  START();
  __ Mov(x17, 1);
  __ Mov(x18, 0x00000000ffffffff);
  __ Mov(x19, 0xffffffffffffffff);
  __ Mov(x20, 4);
  __ Mov(x21, 0x0000000200000000);

  __ Smsubl(x9, w17, w18, x20);
  __ Smsubl(x10, w18, w18, x20);
  __ Smsubl(x11, w19, w19, x20);
  __ Smsubl(x12, w19, w19, x21);
  __ Umsubl(x13, w17, w18, x20);
  __ Umsubl(x14, w18, w18, x20);
  __ Umsubl(x15, w19, w19, x20);
  __ Umsubl(x22, w19, w19, x21);
  END();

  RUN();

  ASSERT_EQUAL_64(5, x9);
  ASSERT_EQUAL_64(3, x10);
  ASSERT_EQUAL_64(3, x11);
  ASSERT_EQUAL_64(0x00000001ffffffff, x12);
  ASSERT_EQUAL_64(0xffffffff00000005, x13);
  ASSERT_EQUAL_64(0x0000000200000003, x14);
  ASSERT_EQUAL_64(0x0000000200000003, x15);
  ASSERT_EQUAL_64(0x00000003ffffffff, x22);

  TEARDOWN();
}


TEST(div) {
  SETUP();

  START();
  __ Mov(x16, 1);
  __ Mov(x17, 0xffffffff);
  __ Mov(x18, 0xffffffffffffffff);
  __ Mov(x19, 0x80000000);
  __ Mov(x20, 0x8000000000000000);
  __ Mov(x21, 2);

  __ Udiv(w0, w16, w16);
  __ Udiv(w1, w17, w16);
  __ Sdiv(w2, w16, w16);
  __ Sdiv(w3, w16, w17);
  __ Sdiv(w4, w17, w18);

  __ Udiv(x5, x16, x16);
  __ Udiv(x6, x17, x18);
  __ Sdiv(x7, x16, x16);
  __ Sdiv(x8, x16, x17);
  __ Sdiv(x9, x17, x18);

  __ Udiv(w10, w19, w21);
  __ Sdiv(w11, w19, w21);
  __ Udiv(x12, x19, x21);
  __ Sdiv(x13, x19, x21);
  __ Udiv(x14, x20, x21);
  __ Sdiv(x15, x20, x21);

  __ Udiv(w22, w19, w17);
  __ Sdiv(w23, w19, w17);
  __ Udiv(x24, x20, x18);
  __ Sdiv(x25, x20, x18);

  __ Udiv(x26, x16, x21);
  __ Sdiv(x27, x16, x21);
  __ Udiv(x28, x18, x21);
  __ Sdiv(x29, x18, x21);

  __ Mov(x17, 0);
  __ Udiv(w18, w16, w17);
  __ Sdiv(w19, w16, w17);
  __ Udiv(x20, x16, x17);
  __ Sdiv(x21, x16, x17);
  END();

  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(0xffffffff, x1);
  ASSERT_EQUAL_64(1, x2);
  ASSERT_EQUAL_64(0xffffffff, x3);
  ASSERT_EQUAL_64(1, x4);
  ASSERT_EQUAL_64(1, x5);
  ASSERT_EQUAL_64(0, x6);
  ASSERT_EQUAL_64(1, x7);
  ASSERT_EQUAL_64(0, x8);
  ASSERT_EQUAL_64(0xffffffff00000001, x9);
  ASSERT_EQUAL_64(0x40000000, x10);
  ASSERT_EQUAL_64(0xc0000000, x11);
  ASSERT_EQUAL_64(0x0000000040000000, x12);
  ASSERT_EQUAL_64(0x0000000040000000, x13);
  ASSERT_EQUAL_64(0x4000000000000000, x14);
  ASSERT_EQUAL_64(0xc000000000000000, x15);
  ASSERT_EQUAL_64(0, x22);
  ASSERT_EQUAL_64(0x80000000, x23);
  ASSERT_EQUAL_64(0, x24);
  ASSERT_EQUAL_64(0x8000000000000000, x25);
  ASSERT_EQUAL_64(0, x26);
  ASSERT_EQUAL_64(0, x27);
  ASSERT_EQUAL_64(0x7fffffffffffffff, x28);
  ASSERT_EQUAL_64(0, x29);
  ASSERT_EQUAL_64(0, x18);
  ASSERT_EQUAL_64(0, x19);
  ASSERT_EQUAL_64(0, x20);
  ASSERT_EQUAL_64(0, x21);

  TEARDOWN();
}


TEST(rbit_rev) {
  SETUP();

  START();
  __ Mov(x24, 0xfedcba9876543210);
  __ Rbit(w0, w24);
  __ Rbit(x1, x24);
  __ Rev16(w2, w24);
  __ Rev16(x3, x24);
  __ Rev(w4, w24);
  __ Rev32(x5, x24);
  __ Rev(x6, x24);
  END();

  RUN();

  ASSERT_EQUAL_64(0x084c2a6e, x0);
  ASSERT_EQUAL_64(0x084c2a6e195d3b7f, x1);
  ASSERT_EQUAL_64(0x54761032, x2);
  ASSERT_EQUAL_64(0xdcfe98ba54761032, x3);
  ASSERT_EQUAL_64(0x10325476, x4);
  ASSERT_EQUAL_64(0x98badcfe10325476, x5);
  ASSERT_EQUAL_64(0x1032547698badcfe, x6);

  TEARDOWN();
}


TEST(clz_cls) {
  SETUP();

  START();
  __ Mov(x24, 0x0008000000800000);
  __ Mov(x25, 0xff800000fff80000);
  __ Mov(x26, 0);
  __ Clz(w0, w24);
  __ Clz(x1, x24);
  __ Clz(w2, w25);
  __ Clz(x3, x25);
  __ Clz(w4, w26);
  __ Clz(x5, x26);
  __ Cls(w6, w24);
  __ Cls(x7, x24);
  __ Cls(w8, w25);
  __ Cls(x9, x25);
  __ Cls(w10, w26);
  __ Cls(x11, x26);
  END();

  RUN();

  ASSERT_EQUAL_64(8, x0);
  ASSERT_EQUAL_64(12, x1);
  ASSERT_EQUAL_64(0, x2);
  ASSERT_EQUAL_64(0, x3);
  ASSERT_EQUAL_64(32, x4);
  ASSERT_EQUAL_64(64, x5);
  ASSERT_EQUAL_64(7, x6);
  ASSERT_EQUAL_64(11, x7);
  ASSERT_EQUAL_64(12, x8);
  ASSERT_EQUAL_64(8, x9);
  ASSERT_EQUAL_64(31, x10);
  ASSERT_EQUAL_64(63, x11);

  TEARDOWN();
}


TEST(label) {
  SETUP();

  Label label_1, label_2, label_3, label_4;

  START();
  __ Mov(x0, 0x1);
  __ Mov(x1, 0x0);
  __ Mov(x22, lr);    // Save lr.

  __ B(&label_1);
  __ B(&label_1);
  __ B(&label_1);     // Multiple branches to the same label.
  __ Mov(x0, 0x0);
  __ Bind(&label_2);
  __ B(&label_3);     // Forward branch.
  __ Mov(x0, 0x0);
  __ Bind(&label_1);
  __ B(&label_2);     // Backward branch.
  __ Mov(x0, 0x0);
  __ Bind(&label_3);
  __ Bl(&label_4);
  END();

  __ Bind(&label_4);
  __ Mov(x1, 0x1);
  __ Mov(lr, x22);
  END();

  RUN();

  ASSERT_EQUAL_64(0x1, x0);
  ASSERT_EQUAL_64(0x1, x1);

  TEARDOWN();
}


TEST(label_2) {
  SETUP();

  Label label_1, label_2, label_3;
  Label first_jump_to_3;

  START();
  __ Mov(x0, 0x0);

  __ B(&label_1);
  ptrdiff_t offset_2 = masm.CursorOffset();
  __ Orr(x0, x0, 1 << 1);
  __ B(&label_3);
  ptrdiff_t offset_1 = masm.CursorOffset();
  __ Orr(x0, x0, 1 << 0);
  __ B(&label_2);
  ptrdiff_t offset_3 = masm.CursorOffset();
  __ Tbz(x0, 2, &first_jump_to_3);
  __ Orr(x0, x0, 1 << 3);
  __ Bind(&first_jump_to_3);
  __ Orr(x0, x0, 1 << 2);
  __ Tbz(x0, 3, &label_3);

  // Labels 1, 2, and 3 are bound before the current buffer offset. Branches to
  // label_1 and label_2 branch respectively forward and backward. Branches to
  // label 3 include both forward and backward branches.
  masm.BindToOffset(&label_1, offset_1);
  masm.BindToOffset(&label_2, offset_2);
  masm.BindToOffset(&label_3, offset_3);

  END();

  RUN();

  ASSERT_EQUAL_64(0xf, x0);

  TEARDOWN();
}


TEST(adr) {
  SETUP();

  Label label_1, label_2, label_3, label_4;

  START();
  __ Mov(x0, 0x0);        // Set to non-zero to indicate failure.
  __ Adr(x1, &label_3);   // Set to zero to indicate success.

  __ Adr(x2, &label_1);   // Multiple forward references to the same label.
  __ Adr(x3, &label_1);
  __ Adr(x4, &label_1);

  __ Bind(&label_2);
  __ Eor(x5, x2, Operand(x3));  // Ensure that x2,x3 and x4 are identical.
  __ Eor(x6, x2, Operand(x4));
  __ Orr(x0, x0, Operand(x5));
  __ Orr(x0, x0, Operand(x6));
  __ Br(x2);  // label_1, label_3

  __ Bind(&label_3);
  __ Adr(x2, &label_3);   // Self-reference (offset 0).
  __ Eor(x1, x1, Operand(x2));
  __ Adr(x2, &label_4);   // Simple forward reference.
  __ Br(x2);  // label_4

  __ Bind(&label_1);
  __ Adr(x2, &label_3);   // Multiple reverse references to the same label.
  __ Adr(x3, &label_3);
  __ Adr(x4, &label_3);
  __ Adr(x5, &label_2);   // Simple reverse reference.
  __ Br(x5);  // label_2

  __ Bind(&label_4);
  END();

  RUN();

  ASSERT_EQUAL_64(0x0, x0);
  ASSERT_EQUAL_64(0x0, x1);

  TEARDOWN();
}


// Simple adrp tests: check that labels are linked and handled properly.
// This is similar to the adr test, but all the adrp instructions are put on the
// same page so that they return the same value.
TEST(adrp) {
  Label start;
  Label label_1, label_2, label_3;

  SETUP_CUSTOM(2 * kPageSize, PageOffsetDependentCode);
  START();

  // Waste space until the start of a page.
  {
    InstructionAccurateScope scope(&masm,
                                   kPageSize / kInstructionSize,
                                   InstructionAccurateScope::kMaximumSize);
    const uintptr_t kPageOffsetMask = kPageSize - 1;
    while ((masm.GetCursorAddress<uintptr_t>() & kPageOffsetMask) != 0) {
      __ b(&start);
    }
    __ bind(&start);
  }

  // Simple forward reference.
  __ Adrp(x0, &label_2);

  __ Bind(&label_1);

  // Multiple forward references to the same label.
  __ Adrp(x1, &label_3);
  __ Adrp(x2, &label_3);
  __ Adrp(x3, &label_3);

  __ Bind(&label_2);

  // Self-reference (offset 0).
  __ Adrp(x4, &label_2);

  __ Bind(&label_3);

  // Simple reverse reference.
  __ Adrp(x5, &label_1);

  // Multiple reverse references to the same label.
  __ Adrp(x6, &label_2);
  __ Adrp(x7, &label_2);
  __ Adrp(x8, &label_2);

  VIXL_ASSERT(masm.SizeOfCodeGeneratedSince(&start) < kPageSize);
  END();
  RUN();

  uint64_t expected = reinterpret_cast<uint64_t>(
      AlignDown(masm.GetLabelAddress<uint64_t*>(&start), kPageSize));
  ASSERT_EQUAL_64(expected, x0);
  ASSERT_EQUAL_64(expected, x1);
  ASSERT_EQUAL_64(expected, x2);
  ASSERT_EQUAL_64(expected, x3);
  ASSERT_EQUAL_64(expected, x4);
  ASSERT_EQUAL_64(expected, x5);
  ASSERT_EQUAL_64(expected, x6);
  ASSERT_EQUAL_64(expected, x7);
  ASSERT_EQUAL_64(expected, x8);

  TEARDOWN_CUSTOM();
}


static void AdrpPageBoundaryHelper(unsigned offset_into_page) {
  VIXL_ASSERT(offset_into_page < kPageSize);
  VIXL_ASSERT((offset_into_page % kInstructionSize) == 0);

  const uintptr_t kPageOffsetMask = kPageSize - 1;

  // The test label is always bound on page 0. Adrp instructions are generated
  // on pages from kStartPage to kEndPage (inclusive).
  const int kStartPage = -16;
  const int kEndPage = 16;
  const int kMaxCodeSize = (kEndPage - kStartPage + 2) * kPageSize;

  SETUP_CUSTOM(kMaxCodeSize, PageOffsetDependentCode);
  START();

  Label test;
  Label start;

  {
    InstructionAccurateScope scope(&masm,
                                   kMaxCodeSize / kInstructionSize,
                                   InstructionAccurateScope::kMaximumSize);
    // Initialize NZCV with `eq` flags.
    __ cmp(wzr, wzr);
    // Waste space until the start of a page.
    while ((masm.GetCursorAddress<uintptr_t>() & kPageOffsetMask) != 0) {
      __ b(&start);
    }

    // The first page.
    VIXL_STATIC_ASSERT(kStartPage < 0);
    {
      InstructionAccurateScope scope_page(&masm, kPageSize / kInstructionSize);
      __ bind(&start);
      __ adrp(x0, &test);
      __ adrp(x1, &test);
      for (size_t i = 2; i < (kPageSize / kInstructionSize); i += 2) {
        __ ccmp(x0, x1, NoFlag, eq);
        __ adrp(x1, &test);
      }
    }

    // Subsequent pages.
    VIXL_STATIC_ASSERT(kEndPage >= 0);
    for (int page = (kStartPage + 1); page <= kEndPage; page++) {
      InstructionAccurateScope scope_page(&masm, kPageSize / kInstructionSize);
      if (page == 0) {
        for (size_t i = 0; i < (kPageSize / kInstructionSize);) {
          if (i++ == (offset_into_page / kInstructionSize)) __ bind(&test);
          __ ccmp(x0, x1, NoFlag, eq);
          if (i++ == (offset_into_page / kInstructionSize)) __ bind(&test);
          __ adrp(x1, &test);
        }
      } else {
        for (size_t i = 0; i < (kPageSize / kInstructionSize); i += 2) {
          __ ccmp(x0, x1, NoFlag, eq);
          __ adrp(x1, &test);
        }
      }
    }
  }

  // Every adrp instruction pointed to the same label (`test`), so they should
  // all have produced the same result.

  END();
  RUN();

  uintptr_t expected =
      AlignDown(masm.GetLabelAddress<uintptr_t>(&test), kPageSize);
  ASSERT_EQUAL_64(expected, x0);
  ASSERT_EQUAL_64(expected, x1);
  ASSERT_EQUAL_NZCV(ZCFlag);

  TEARDOWN_CUSTOM();
}


// Test that labels are correctly referenced by adrp across page boundaries.
TEST(adrp_page_boundaries) {
  VIXL_STATIC_ASSERT(kPageSize == 4096);
  AdrpPageBoundaryHelper(kInstructionSize * 0);
  AdrpPageBoundaryHelper(kInstructionSize * 1);
  AdrpPageBoundaryHelper(kInstructionSize * 512);
  AdrpPageBoundaryHelper(kInstructionSize * 1022);
  AdrpPageBoundaryHelper(kInstructionSize * 1023);
}


static void AdrpOffsetHelper(int64_t offset) {
  const size_t kPageOffsetMask = kPageSize - 1;
  const int kMaxCodeSize = 2 * kPageSize;

  SETUP_CUSTOM(kMaxCodeSize, PageOffsetDependentCode);
  START();

  Label page;

  {
    InstructionAccurateScope scope(&masm,
                                   kMaxCodeSize / kInstructionSize,
                                   InstructionAccurateScope::kMaximumSize);
    // Initialize NZCV with `eq` flags.
    __ cmp(wzr, wzr);
    // Waste space until the start of a page.
    while ((masm.GetCursorAddress<uintptr_t>() & kPageOffsetMask) != 0) {
      __ b(&page);
    }
    __ bind(&page);

    {
      int imm21 = static_cast<int>(offset);
      InstructionAccurateScope scope_page(&masm, kPageSize / kInstructionSize);
      // Every adrp instruction on this page should return the same value.
      __ adrp(x0, imm21);
      __ adrp(x1, imm21);
      for (size_t i = 2; i < kPageSize / kInstructionSize; i += 2) {
        __ ccmp(x0, x1, NoFlag, eq);
        __ adrp(x1, imm21);
      }
    }
  }

  END();
  RUN();

  uintptr_t expected =
      masm.GetLabelAddress<uintptr_t>(&page) + (kPageSize * offset);
  ASSERT_EQUAL_64(expected, x0);
  ASSERT_EQUAL_64(expected, x1);
  ASSERT_EQUAL_NZCV(ZCFlag);

  TEARDOWN_CUSTOM();
}


// Check that adrp produces the correct result for a specific offset.
TEST(adrp_offset) {
  AdrpOffsetHelper(0);
  AdrpOffsetHelper(1);
  AdrpOffsetHelper(-1);
  AdrpOffsetHelper(4);
  AdrpOffsetHelper(-4);
  AdrpOffsetHelper(0x000fffff);
  AdrpOffsetHelper(-0x000fffff);
  AdrpOffsetHelper(-0x00100000);
}


TEST(branch_cond) {
  SETUP();
  ALLOW_ASM();

  Label done, wrong;

  START();
  __ Mov(x0, 0x1);
  __ Mov(x1, 0x1);
  __ Mov(x2, 0x8000000000000000);

  // For each 'cmp' instruction below, condition codes other than the ones
  // following it would branch.

  __ Cmp(x1, 0);
  __ B(&wrong, eq);
  __ B(&wrong, lo);
  __ B(&wrong, mi);
  __ B(&wrong, vs);
  __ B(&wrong, ls);
  __ B(&wrong, lt);
  __ B(&wrong, le);
  Label ok_1;
  __ B(&ok_1, ne);
  __ Mov(x0, 0x0);
  __ Bind(&ok_1);

  __ Cmp(x1, 1);
  __ B(&wrong, ne);
  __ B(&wrong, lo);
  __ B(&wrong, mi);
  __ B(&wrong, vs);
  __ B(&wrong, hi);
  __ B(&wrong, lt);
  __ B(&wrong, gt);
  Label ok_2;
  __ B(&ok_2, pl);
  __ Mov(x0, 0x0);
  __ Bind(&ok_2);

  __ Cmp(x1, 2);
  __ B(&wrong, eq);
  __ B(&wrong, hs);
  __ B(&wrong, pl);
  __ B(&wrong, vs);
  __ B(&wrong, hi);
  __ B(&wrong, ge);
  __ B(&wrong, gt);
  Label ok_3;
  __ B(&ok_3, vc);
  __ Mov(x0, 0x0);
  __ Bind(&ok_3);

  __ Cmp(x2, 1);
  __ B(&wrong, eq);
  __ B(&wrong, lo);
  __ B(&wrong, mi);
  __ B(&wrong, vc);
  __ B(&wrong, ls);
  __ B(&wrong, ge);
  __ B(&wrong, gt);
  Label ok_4;
  __ B(&ok_4, le);
  __ Mov(x0, 0x0);
  __ Bind(&ok_4);

  // The MacroAssembler does not allow al as a branch condition.
  Label ok_5;
  __ b(&ok_5, al);
  __ Mov(x0, 0x0);
  __ Bind(&ok_5);

  // The MacroAssembler does not allow nv as a branch condition.
  Label ok_6;
  __ b(&ok_6, nv);
  __ Mov(x0, 0x0);
  __ Bind(&ok_6);

  __ B(&done);

  __ Bind(&wrong);
  __ Mov(x0, 0x0);

  __ Bind(&done);
  END();

  RUN();

  ASSERT_EQUAL_64(0x1, x0);

  TEARDOWN();
}


TEST(branch_to_reg) {
  SETUP();

  // Test br.
  Label fn1, after_fn1;

  START();
  __ Mov(x29, lr);

  __ Mov(x1, 0);
  __ B(&after_fn1);

  __ Bind(&fn1);
  __ Mov(x0, lr);
  __ Mov(x1, 42);
  __ Br(x0);

  __ Bind(&after_fn1);
  __ Bl(&fn1);

  // Test blr.
  Label fn2, after_fn2;

  __ Mov(x2, 0);
  __ B(&after_fn2);

  __ Bind(&fn2);
  __ Mov(x0, lr);
  __ Mov(x2, 84);
  __ Blr(x0);

  __ Bind(&after_fn2);
  __ Bl(&fn2);
  __ Mov(x3, lr);

  __ Mov(lr, x29);
  END();

  RUN();

  ASSERT_EQUAL_64(core.xreg(3) + kInstructionSize, x0);
  ASSERT_EQUAL_64(42, x1);
  ASSERT_EQUAL_64(84, x2);

  TEARDOWN();
}


TEST(compare_branch) {
  SETUP();

  START();
  __ Mov(x0, 0);
  __ Mov(x1, 0);
  __ Mov(x2, 0);
  __ Mov(x3, 0);
  __ Mov(x4, 0);
  __ Mov(x5, 0);
  __ Mov(x16, 0);
  __ Mov(x17, 42);

  Label zt, zt_end;
  __ Cbz(w16, &zt);
  __ B(&zt_end);
  __ Bind(&zt);
  __ Mov(x0, 1);
  __ Bind(&zt_end);

  Label zf, zf_end;
  __ Cbz(x17, &zf);
  __ B(&zf_end);
  __ Bind(&zf);
  __ Mov(x1, 1);
  __ Bind(&zf_end);

  Label nzt, nzt_end;
  __ Cbnz(w17, &nzt);
  __ B(&nzt_end);
  __ Bind(&nzt);
  __ Mov(x2, 1);
  __ Bind(&nzt_end);

  Label nzf, nzf_end;
  __ Cbnz(x16, &nzf);
  __ B(&nzf_end);
  __ Bind(&nzf);
  __ Mov(x3, 1);
  __ Bind(&nzf_end);

  __ Mov(x18, 0xffffffff00000000);

  Label a, a_end;
  __ Cbz(w18, &a);
  __ B(&a_end);
  __ Bind(&a);
  __ Mov(x4, 1);
  __ Bind(&a_end);

  Label b, b_end;
  __ Cbnz(w18, &b);
  __ B(&b_end);
  __ Bind(&b);
  __ Mov(x5, 1);
  __ Bind(&b_end);

  END();

  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(0, x1);
  ASSERT_EQUAL_64(1, x2);
  ASSERT_EQUAL_64(0, x3);
  ASSERT_EQUAL_64(1, x4);
  ASSERT_EQUAL_64(0, x5);

  TEARDOWN();
}


TEST(test_branch) {
  SETUP();

  START();
  __ Mov(x0, 0);
  __ Mov(x1, 0);
  __ Mov(x2, 0);
  __ Mov(x3, 0);
  __ Mov(x16, 0xaaaaaaaaaaaaaaaa);

  Label bz, bz_end;
  __ Tbz(w16, 0, &bz);
  __ B(&bz_end);
  __ Bind(&bz);
  __ Mov(x0, 1);
  __ Bind(&bz_end);

  Label bo, bo_end;
  __ Tbz(x16, 63, &bo);
  __ B(&bo_end);
  __ Bind(&bo);
  __ Mov(x1, 1);
  __ Bind(&bo_end);

  Label nbz, nbz_end;
  __ Tbnz(x16, 61, &nbz);
  __ B(&nbz_end);
  __ Bind(&nbz);
  __ Mov(x2, 1);
  __ Bind(&nbz_end);

  Label nbo, nbo_end;
  __ Tbnz(w16, 2, &nbo);
  __ B(&nbo_end);
  __ Bind(&nbo);
  __ Mov(x3, 1);
  __ Bind(&nbo_end);
  END();

  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(0, x1);
  ASSERT_EQUAL_64(1, x2);
  ASSERT_EQUAL_64(0, x3);

  TEARDOWN();
}


TEST(branch_type) {
  SETUP();

  Label fail, done;

  START();
  __ Mov(x0, 0x0);
  __ Mov(x10, 0x7);
  __ Mov(x11, 0x0);

  // Test non taken branches.
  __ Cmp(x10, 0x7);
  __ B(&fail, ne);
  __ B(&fail, never);
  __ B(&fail, reg_zero, x10);
  __ B(&fail, reg_not_zero, x11);
  __ B(&fail, reg_bit_clear, x10, 0);
  __ B(&fail, reg_bit_set, x10, 3);

  // Test taken branches.
  Label l1, l2, l3, l4, l5;
  __ Cmp(x10, 0x7);
  __ B(&l1, eq);
  __ B(&fail);
  __ Bind(&l1);
  __ B(&l2, always);
  __ B(&fail);
  __ Bind(&l2);
  __ B(&l3, reg_not_zero, x10);
  __ B(&fail);
  __ Bind(&l3);
  __ B(&l4, reg_bit_clear, x10, 15);
  __ B(&fail);
  __ Bind(&l4);
  __ B(&l5, reg_bit_set, x10, 1);
  __ B(&fail);
  __ Bind(&l5);

  __ B(&done);

  __ Bind(&fail);
  __ Mov(x0, 0x1);

  __ Bind(&done);

  END();

  RUN();

  ASSERT_EQUAL_64(0x0, x0);

  TEARDOWN();
}


TEST(ldr_str_offset) {
  SETUP();

  uint64_t src[2] = {0xfedcba9876543210, 0x0123456789abcdef};
  uint64_t dst[5] = {0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, dst_base);
  __ Ldr(w0, MemOperand(x17));
  __ Str(w0, MemOperand(x18));
  __ Ldr(w1, MemOperand(x17, 4));
  __ Str(w1, MemOperand(x18, 12));
  __ Ldr(x2, MemOperand(x17, 8));
  __ Str(x2, MemOperand(x18, 16));
  __ Ldrb(w3, MemOperand(x17, 1));
  __ Strb(w3, MemOperand(x18, 25));
  __ Ldrh(w4, MemOperand(x17, 2));
  __ Strh(w4, MemOperand(x18, 33));
  END();

  RUN();

  ASSERT_EQUAL_64(0x76543210, x0);
  ASSERT_EQUAL_64(0x76543210, dst[0]);
  ASSERT_EQUAL_64(0xfedcba98, x1);
  ASSERT_EQUAL_64(0xfedcba9800000000, dst[1]);
  ASSERT_EQUAL_64(0x0123456789abcdef, x2);
  ASSERT_EQUAL_64(0x0123456789abcdef, dst[2]);
  ASSERT_EQUAL_64(0x32, x3);
  ASSERT_EQUAL_64(0x3200, dst[3]);
  ASSERT_EQUAL_64(0x7654, x4);
  ASSERT_EQUAL_64(0x765400, dst[4]);
  ASSERT_EQUAL_64(src_base, x17);
  ASSERT_EQUAL_64(dst_base, x18);

  TEARDOWN();
}


TEST(ldr_str_wide) {
  SETUP();

  uint32_t src[8192];
  uint32_t dst[8192];
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);
  memset(src, 0xaa, 8192 * sizeof(src[0]));
  memset(dst, 0xaa, 8192 * sizeof(dst[0]));
  src[0] = 0;
  src[6144] = 6144;
  src[8191] = 8191;

  START();
  __ Mov(x22, src_base);
  __ Mov(x23, dst_base);
  __ Mov(x24, src_base);
  __ Mov(x25, dst_base);
  __ Mov(x26, src_base);
  __ Mov(x27, dst_base);

  __ Ldr(w0, MemOperand(x22, 8191 * sizeof(src[0])));
  __ Str(w0, MemOperand(x23, 8191 * sizeof(dst[0])));
  __ Ldr(w1, MemOperand(x24, 4096 * sizeof(src[0]), PostIndex));
  __ Str(w1, MemOperand(x25, 4096 * sizeof(dst[0]), PostIndex));
  __ Ldr(w2, MemOperand(x26, 6144 * sizeof(src[0]), PreIndex));
  __ Str(w2, MemOperand(x27, 6144 * sizeof(dst[0]), PreIndex));
  END();

  RUN();

  ASSERT_EQUAL_32(8191, w0);
  ASSERT_EQUAL_32(8191, dst[8191]);
  ASSERT_EQUAL_64(src_base, x22);
  ASSERT_EQUAL_64(dst_base, x23);
  ASSERT_EQUAL_32(0, w1);
  ASSERT_EQUAL_32(0, dst[0]);
  ASSERT_EQUAL_64(src_base + 4096 * sizeof(src[0]), x24);
  ASSERT_EQUAL_64(dst_base + 4096 * sizeof(dst[0]), x25);
  ASSERT_EQUAL_32(6144, w2);
  ASSERT_EQUAL_32(6144, dst[6144]);
  ASSERT_EQUAL_64(src_base + 6144 * sizeof(src[0]), x26);
  ASSERT_EQUAL_64(dst_base + 6144 * sizeof(dst[0]), x27);

  TEARDOWN();
}


TEST(ldr_str_preindex) {
  SETUP();

  uint64_t src[2] = {0xfedcba9876543210, 0x0123456789abcdef};
  uint64_t dst[6] = {0, 0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, dst_base);
  __ Mov(x19, src_base);
  __ Mov(x20, dst_base);
  __ Mov(x21, src_base + 16);
  __ Mov(x22, dst_base + 40);
  __ Mov(x23, src_base);
  __ Mov(x24, dst_base);
  __ Mov(x25, src_base);
  __ Mov(x26, dst_base);
  __ Ldr(w0, MemOperand(x17, 4, PreIndex));
  __ Str(w0, MemOperand(x18, 12, PreIndex));
  __ Ldr(x1, MemOperand(x19, 8, PreIndex));
  __ Str(x1, MemOperand(x20, 16, PreIndex));
  __ Ldr(w2, MemOperand(x21, -4, PreIndex));
  __ Str(w2, MemOperand(x22, -4, PreIndex));
  __ Ldrb(w3, MemOperand(x23, 1, PreIndex));
  __ Strb(w3, MemOperand(x24, 25, PreIndex));
  __ Ldrh(w4, MemOperand(x25, 3, PreIndex));
  __ Strh(w4, MemOperand(x26, 41, PreIndex));
  END();

  RUN();

  ASSERT_EQUAL_64(0xfedcba98, x0);
  ASSERT_EQUAL_64(0xfedcba9800000000, dst[1]);
  ASSERT_EQUAL_64(0x0123456789abcdef, x1);
  ASSERT_EQUAL_64(0x0123456789abcdef, dst[2]);
  ASSERT_EQUAL_64(0x01234567, x2);
  ASSERT_EQUAL_64(0x0123456700000000, dst[4]);
  ASSERT_EQUAL_64(0x32, x3);
  ASSERT_EQUAL_64(0x3200, dst[3]);
  ASSERT_EQUAL_64(0x9876, x4);
  ASSERT_EQUAL_64(0x987600, dst[5]);
  ASSERT_EQUAL_64(src_base + 4, x17);
  ASSERT_EQUAL_64(dst_base + 12, x18);
  ASSERT_EQUAL_64(src_base + 8, x19);
  ASSERT_EQUAL_64(dst_base + 16, x20);
  ASSERT_EQUAL_64(src_base + 12, x21);
  ASSERT_EQUAL_64(dst_base + 36, x22);
  ASSERT_EQUAL_64(src_base + 1, x23);
  ASSERT_EQUAL_64(dst_base + 25, x24);
  ASSERT_EQUAL_64(src_base + 3, x25);
  ASSERT_EQUAL_64(dst_base + 41, x26);

  TEARDOWN();
}


TEST(ldr_str_postindex) {
  SETUP();

  uint64_t src[2] = {0xfedcba9876543210, 0x0123456789abcdef};
  uint64_t dst[6] = {0, 0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, src_base + 4);
  __ Mov(x18, dst_base + 12);
  __ Mov(x19, src_base + 8);
  __ Mov(x20, dst_base + 16);
  __ Mov(x21, src_base + 8);
  __ Mov(x22, dst_base + 32);
  __ Mov(x23, src_base + 1);
  __ Mov(x24, dst_base + 25);
  __ Mov(x25, src_base + 3);
  __ Mov(x26, dst_base + 41);
  __ Ldr(w0, MemOperand(x17, 4, PostIndex));
  __ Str(w0, MemOperand(x18, 12, PostIndex));
  __ Ldr(x1, MemOperand(x19, 8, PostIndex));
  __ Str(x1, MemOperand(x20, 16, PostIndex));
  __ Ldr(x2, MemOperand(x21, -8, PostIndex));
  __ Str(x2, MemOperand(x22, -32, PostIndex));
  __ Ldrb(w3, MemOperand(x23, 1, PostIndex));
  __ Strb(w3, MemOperand(x24, 5, PostIndex));
  __ Ldrh(w4, MemOperand(x25, -3, PostIndex));
  __ Strh(w4, MemOperand(x26, -41, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_64(0xfedcba98, x0);
  ASSERT_EQUAL_64(0xfedcba9800000000, dst[1]);
  ASSERT_EQUAL_64(0x0123456789abcdef, x1);
  ASSERT_EQUAL_64(0x0123456789abcdef, dst[2]);
  ASSERT_EQUAL_64(0x0123456789abcdef, x2);
  ASSERT_EQUAL_64(0x0123456789abcdef, dst[4]);
  ASSERT_EQUAL_64(0x32, x3);
  ASSERT_EQUAL_64(0x3200, dst[3]);
  ASSERT_EQUAL_64(0x9876, x4);
  ASSERT_EQUAL_64(0x987600, dst[5]);
  ASSERT_EQUAL_64(src_base + 8, x17);
  ASSERT_EQUAL_64(dst_base + 24, x18);
  ASSERT_EQUAL_64(src_base + 16, x19);
  ASSERT_EQUAL_64(dst_base + 32, x20);
  ASSERT_EQUAL_64(src_base, x21);
  ASSERT_EQUAL_64(dst_base, x22);
  ASSERT_EQUAL_64(src_base + 2, x23);
  ASSERT_EQUAL_64(dst_base + 30, x24);
  ASSERT_EQUAL_64(src_base, x25);
  ASSERT_EQUAL_64(dst_base, x26);

  TEARDOWN();
}


TEST(ldr_str_largeindex) {
  SETUP();

  // This value won't fit in the immediate offset field of ldr/str instructions.
  int largeoffset = 0xabcdef;

  int64_t data[3] = { 0x1122334455667788, 0, 0 };
  uint64_t base_addr = reinterpret_cast<uintptr_t>(data);
  uint64_t drifted_addr = base_addr - largeoffset;

  // This test checks that we we can use large immediate offsets when
  // using PreIndex or PostIndex addressing mode of the MacroAssembler
  // Ldr/Str instructions.

  START();
  __ Mov(x19, drifted_addr);
  __ Ldr(x0, MemOperand(x19, largeoffset, PreIndex));

  __ Mov(x20, base_addr);
  __ Ldr(x1, MemOperand(x20, largeoffset, PostIndex));

  __ Mov(x21, drifted_addr);
  __ Str(x0, MemOperand(x21, largeoffset + 8, PreIndex));

  __ Mov(x22, base_addr + 16);
  __ Str(x0, MemOperand(x22, largeoffset, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_64(0x1122334455667788, data[0]);
  ASSERT_EQUAL_64(0x1122334455667788, data[1]);
  ASSERT_EQUAL_64(0x1122334455667788, data[2]);
  ASSERT_EQUAL_64(0x1122334455667788, x0);
  ASSERT_EQUAL_64(0x1122334455667788, x1);

  ASSERT_EQUAL_64(base_addr, x19);
  ASSERT_EQUAL_64(base_addr + largeoffset, x20);
  ASSERT_EQUAL_64(base_addr + 8, x21);
  ASSERT_EQUAL_64(base_addr + 16 + largeoffset, x22);

  TEARDOWN();
}


TEST(load_signed) {
  SETUP();

  uint32_t src[2] = {0x80008080, 0x7fff7f7f};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x24, src_base);
  __ Ldrsb(w0, MemOperand(x24));
  __ Ldrsb(w1, MemOperand(x24, 4));
  __ Ldrsh(w2, MemOperand(x24));
  __ Ldrsh(w3, MemOperand(x24, 4));
  __ Ldrsb(x4, MemOperand(x24));
  __ Ldrsb(x5, MemOperand(x24, 4));
  __ Ldrsh(x6, MemOperand(x24));
  __ Ldrsh(x7, MemOperand(x24, 4));
  __ Ldrsw(x8, MemOperand(x24));
  __ Ldrsw(x9, MemOperand(x24, 4));
  END();

  RUN();

  ASSERT_EQUAL_64(0xffffff80, x0);
  ASSERT_EQUAL_64(0x0000007f, x1);
  ASSERT_EQUAL_64(0xffff8080, x2);
  ASSERT_EQUAL_64(0x00007f7f, x3);
  ASSERT_EQUAL_64(0xffffffffffffff80, x4);
  ASSERT_EQUAL_64(0x000000000000007f, x5);
  ASSERT_EQUAL_64(0xffffffffffff8080, x6);
  ASSERT_EQUAL_64(0x0000000000007f7f, x7);
  ASSERT_EQUAL_64(0xffffffff80008080, x8);
  ASSERT_EQUAL_64(0x000000007fff7f7f, x9);

  TEARDOWN();
}


TEST(load_store_regoffset) {
  SETUP();

  uint32_t src[3] = {1, 2, 3};
  uint32_t dst[4] = {0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x16, src_base);
  __ Mov(x17, dst_base);
  __ Mov(x18, src_base + 3 * sizeof(src[0]));
  __ Mov(x19, dst_base + 3 * sizeof(dst[0]));
  __ Mov(x20, dst_base + 4 * sizeof(dst[0]));
  __ Mov(x24, 0);
  __ Mov(x25, 4);
  __ Mov(x26, -4);
  __ Mov(x27, 0xfffffffc);  // 32-bit -4.
  __ Mov(x28, 0xfffffffe);  // 32-bit -2.
  __ Mov(x29, 0xffffffff);  // 32-bit -1.

  __ Ldr(w0, MemOperand(x16, x24));
  __ Ldr(x1, MemOperand(x16, x25));
  __ Ldr(w2, MemOperand(x18, x26));
  __ Ldr(w3, MemOperand(x18, x27, SXTW));
  __ Ldr(w4, MemOperand(x18, x28, SXTW, 2));
  __ Str(w0, MemOperand(x17, x24));
  __ Str(x1, MemOperand(x17, x25));
  __ Str(w2, MemOperand(x20, x29, SXTW, 2));
  END();

  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(0x0000000300000002, x1);
  ASSERT_EQUAL_64(3, x2);
  ASSERT_EQUAL_64(3, x3);
  ASSERT_EQUAL_64(2, x4);
  ASSERT_EQUAL_32(1, dst[0]);
  ASSERT_EQUAL_32(2, dst[1]);
  ASSERT_EQUAL_32(3, dst[2]);
  ASSERT_EQUAL_32(3, dst[3]);

  TEARDOWN();
}


TEST(load_store_float) {
  SETUP();

  float src[3] = {1.0, 2.0, 3.0};
  float dst[3] = {0.0, 0.0, 0.0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, dst_base);
  __ Mov(x19, src_base);
  __ Mov(x20, dst_base);
  __ Mov(x21, src_base);
  __ Mov(x22, dst_base);
  __ Ldr(s0, MemOperand(x17, sizeof(src[0])));
  __ Str(s0, MemOperand(x18, sizeof(dst[0]), PostIndex));
  __ Ldr(s1, MemOperand(x19, sizeof(src[0]), PostIndex));
  __ Str(s1, MemOperand(x20, 2 * sizeof(dst[0]), PreIndex));
  __ Ldr(s2, MemOperand(x21, 2 * sizeof(src[0]), PreIndex));
  __ Str(s2, MemOperand(x22, sizeof(dst[0])));
  END();

  RUN();

  ASSERT_EQUAL_FP32(2.0, s0);
  ASSERT_EQUAL_FP32(2.0, dst[0]);
  ASSERT_EQUAL_FP32(1.0, s1);
  ASSERT_EQUAL_FP32(1.0, dst[2]);
  ASSERT_EQUAL_FP32(3.0, s2);
  ASSERT_EQUAL_FP32(3.0, dst[1]);
  ASSERT_EQUAL_64(src_base, x17);
  ASSERT_EQUAL_64(dst_base + sizeof(dst[0]), x18);
  ASSERT_EQUAL_64(src_base + sizeof(src[0]), x19);
  ASSERT_EQUAL_64(dst_base + 2 * sizeof(dst[0]), x20);
  ASSERT_EQUAL_64(src_base + 2 * sizeof(src[0]), x21);
  ASSERT_EQUAL_64(dst_base, x22);

  TEARDOWN();
}


TEST(load_store_double) {
  SETUP();

  double src[3] = {1.0, 2.0, 3.0};
  double dst[3] = {0.0, 0.0, 0.0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, dst_base);
  __ Mov(x19, src_base);
  __ Mov(x20, dst_base);
  __ Mov(x21, src_base);
  __ Mov(x22, dst_base);
  __ Ldr(d0, MemOperand(x17, sizeof(src[0])));
  __ Str(d0, MemOperand(x18, sizeof(dst[0]), PostIndex));
  __ Ldr(d1, MemOperand(x19, sizeof(src[0]), PostIndex));
  __ Str(d1, MemOperand(x20, 2 * sizeof(dst[0]), PreIndex));
  __ Ldr(d2, MemOperand(x21, 2 * sizeof(src[0]), PreIndex));
  __ Str(d2, MemOperand(x22, sizeof(dst[0])));
  END();

  RUN();

  ASSERT_EQUAL_FP64(2.0, d0);
  ASSERT_EQUAL_FP64(2.0, dst[0]);
  ASSERT_EQUAL_FP64(1.0, d1);
  ASSERT_EQUAL_FP64(1.0, dst[2]);
  ASSERT_EQUAL_FP64(3.0, d2);
  ASSERT_EQUAL_FP64(3.0, dst[1]);
  ASSERT_EQUAL_64(src_base, x17);
  ASSERT_EQUAL_64(dst_base + sizeof(dst[0]), x18);
  ASSERT_EQUAL_64(src_base + sizeof(src[0]), x19);
  ASSERT_EQUAL_64(dst_base + 2 * sizeof(dst[0]), x20);
  ASSERT_EQUAL_64(src_base + 2 * sizeof(src[0]), x21);
  ASSERT_EQUAL_64(dst_base, x22);

  TEARDOWN();
}


TEST(load_store_b) {
  SETUP();

  uint8_t src[3] = {0x12, 0x23, 0x34};
  uint8_t dst[3] = {0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, dst_base);
  __ Mov(x19, src_base);
  __ Mov(x20, dst_base);
  __ Mov(x21, src_base);
  __ Mov(x22, dst_base);
  __ Ldr(b0, MemOperand(x17, sizeof(src[0])));
  __ Str(b0, MemOperand(x18, sizeof(dst[0]), PostIndex));
  __ Ldr(b1, MemOperand(x19, sizeof(src[0]), PostIndex));
  __ Str(b1, MemOperand(x20, 2 * sizeof(dst[0]), PreIndex));
  __ Ldr(b2, MemOperand(x21, 2 * sizeof(src[0]), PreIndex));
  __ Str(b2, MemOperand(x22, sizeof(dst[0])));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x23, q0);
  ASSERT_EQUAL_64(0x23, dst[0]);
  ASSERT_EQUAL_128(0, 0x12, q1);
  ASSERT_EQUAL_64(0x12, dst[2]);
  ASSERT_EQUAL_128(0, 0x34, q2);
  ASSERT_EQUAL_64(0x34, dst[1]);
  ASSERT_EQUAL_64(src_base, x17);
  ASSERT_EQUAL_64(dst_base + sizeof(dst[0]), x18);
  ASSERT_EQUAL_64(src_base + sizeof(src[0]), x19);
  ASSERT_EQUAL_64(dst_base + 2 * sizeof(dst[0]), x20);
  ASSERT_EQUAL_64(src_base + 2 * sizeof(src[0]), x21);
  ASSERT_EQUAL_64(dst_base, x22);

  TEARDOWN();
}


TEST(load_store_h) {
  SETUP();

  uint16_t src[3] = {0x1234, 0x2345, 0x3456};
  uint16_t dst[3] = {0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, dst_base);
  __ Mov(x19, src_base);
  __ Mov(x20, dst_base);
  __ Mov(x21, src_base);
  __ Mov(x22, dst_base);
  __ Ldr(h0, MemOperand(x17, sizeof(src[0])));
  __ Str(h0, MemOperand(x18, sizeof(dst[0]), PostIndex));
  __ Ldr(h1, MemOperand(x19, sizeof(src[0]), PostIndex));
  __ Str(h1, MemOperand(x20, 2 * sizeof(dst[0]), PreIndex));
  __ Ldr(h2, MemOperand(x21, 2 * sizeof(src[0]), PreIndex));
  __ Str(h2, MemOperand(x22, sizeof(dst[0])));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x2345, q0);
  ASSERT_EQUAL_64(0x2345, dst[0]);
  ASSERT_EQUAL_128(0, 0x1234, q1);
  ASSERT_EQUAL_64(0x1234, dst[2]);
  ASSERT_EQUAL_128(0, 0x3456, q2);
  ASSERT_EQUAL_64(0x3456, dst[1]);
  ASSERT_EQUAL_64(src_base, x17);
  ASSERT_EQUAL_64(dst_base + sizeof(dst[0]), x18);
  ASSERT_EQUAL_64(src_base + sizeof(src[0]), x19);
  ASSERT_EQUAL_64(dst_base + 2 * sizeof(dst[0]), x20);
  ASSERT_EQUAL_64(src_base + 2 * sizeof(src[0]), x21);
  ASSERT_EQUAL_64(dst_base, x22);

  TEARDOWN();
}


TEST(load_store_q) {
  SETUP();

  uint8_t src[48] = {0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe,
                     0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                     0x21, 0x43, 0x65, 0x87, 0xa9, 0xcb, 0xed, 0x0f,
                     0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
                     0x24, 0x46, 0x68, 0x8a, 0xac, 0xce, 0xe0, 0x02,
                     0x42, 0x64, 0x86, 0xa8, 0xca, 0xec, 0x0e, 0x20};

  uint64_t dst[6] = {0, 0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, dst_base);
  __ Mov(x19, src_base);
  __ Mov(x20, dst_base);
  __ Mov(x21, src_base);
  __ Mov(x22, dst_base);
  __ Ldr(q0, MemOperand(x17, 16));
  __ Str(q0, MemOperand(x18, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));
  __ Str(q1, MemOperand(x20, 32, PreIndex));
  __ Ldr(q2, MemOperand(x21, 32, PreIndex));
  __ Str(q2, MemOperand(x22, 16));
  END();

  RUN();

  ASSERT_EQUAL_128(0xf0debc9a78563412, 0x0fedcba987654321, q0);
  ASSERT_EQUAL_64(0x0fedcba987654321, dst[0]);
  ASSERT_EQUAL_64(0xf0debc9a78563412, dst[1]);
  ASSERT_EQUAL_128(0xefcdab8967452301, 0xfedcba9876543210, q1);
  ASSERT_EQUAL_64(0xfedcba9876543210, dst[4]);
  ASSERT_EQUAL_64(0xefcdab8967452301, dst[5]);
  ASSERT_EQUAL_128(0x200eeccaa8866442, 0x02e0ceac8a684624, q2);
  ASSERT_EQUAL_64(0x02e0ceac8a684624, dst[2]);
  ASSERT_EQUAL_64(0x200eeccaa8866442, dst[3]);
  ASSERT_EQUAL_64(src_base, x17);
  ASSERT_EQUAL_64(dst_base + 16, x18);
  ASSERT_EQUAL_64(src_base + 16, x19);
  ASSERT_EQUAL_64(dst_base + 32, x20);
  ASSERT_EQUAL_64(src_base + 32, x21);
  ASSERT_EQUAL_64(dst_base, x22);

  TEARDOWN();
}


TEST(load_store_v_regoffset) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uint8_t dst[64];
  memset(dst, 0, sizeof(dst));

  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, src_base + 16);
  __ Mov(x18, 1);
  __ Mov(w19, -1);
  __ Mov(x20, dst_base - 1);

  __ Ldr(b0, MemOperand(x17, x18));
  __ Ldr(b1, MemOperand(x17, x19, SXTW));

  __ Ldr(h2, MemOperand(x17, x18));
  __ Ldr(h3, MemOperand(x17, x18, UXTW, 1));
  __ Ldr(h4, MemOperand(x17, x19, SXTW, 1));
  __ Ldr(h5, MemOperand(x17, x18, LSL, 1));

  __ Ldr(s16, MemOperand(x17, x18));
  __ Ldr(s17, MemOperand(x17, x18, UXTW, 2));
  __ Ldr(s18, MemOperand(x17, x19, SXTW, 2));
  __ Ldr(s19, MemOperand(x17, x18, LSL, 2));

  __ Ldr(d20, MemOperand(x17, x18));
  __ Ldr(d21, MemOperand(x17, x18, UXTW, 3));
  __ Ldr(d22, MemOperand(x17, x19, SXTW, 3));
  __ Ldr(d23, MemOperand(x17, x18, LSL, 3));

  __ Ldr(q24, MemOperand(x17, x18));
  __ Ldr(q25, MemOperand(x17, x18, UXTW, 4));
  __ Ldr(q26, MemOperand(x17, x19, SXTW, 4));
  __ Ldr(q27, MemOperand(x17, x18, LSL, 4));

  // Store [bhsdq]27 to adjacent memory locations, then load again to check.
  __ Str(b27, MemOperand(x20, x18));
  __ Str(h27, MemOperand(x20, x18, UXTW, 1));
  __ Add(x20, x20, 8);
  __ Str(s27, MemOperand(x20, x19, SXTW, 2));
  __ Sub(x20, x20, 8);
  __ Str(d27, MemOperand(x20, x18, LSL, 3));
  __ Add(x20, x20, 32);
  __ Str(q27, MemOperand(x20, x19, SXTW, 4));

  __ Sub(x20, x20, 32);
  __ Ldr(q6, MemOperand(x20, x18));
  __ Ldr(q7, MemOperand(x20, x18, LSL, 4));

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x11, q0);
  ASSERT_EQUAL_128(0, 0x0f, q1);
  ASSERT_EQUAL_128(0, 0x1211, q2);
  ASSERT_EQUAL_128(0, 0x1312, q3);
  ASSERT_EQUAL_128(0, 0x0f0e, q4);
  ASSERT_EQUAL_128(0, 0x1312, q5);
  ASSERT_EQUAL_128(0, 0x14131211, q16);
  ASSERT_EQUAL_128(0, 0x17161514, q17);
  ASSERT_EQUAL_128(0, 0x0f0e0d0c, q18);
  ASSERT_EQUAL_128(0, 0x17161514, q19);
  ASSERT_EQUAL_128(0, 0x1817161514131211, q20);
  ASSERT_EQUAL_128(0, 0x1f1e1d1c1b1a1918, q21);
  ASSERT_EQUAL_128(0, 0x0f0e0d0c0b0a0908, q22);
  ASSERT_EQUAL_128(0, 0x1f1e1d1c1b1a1918, q23);
  ASSERT_EQUAL_128(0x201f1e1d1c1b1a19, 0x1817161514131211, q24);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726252423222120, q25);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q26);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726252423222120, q27);
  ASSERT_EQUAL_128(0x2027262524232221, 0x2023222120212020, q6);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726252423222120, q7);

  TEARDOWN();
}


TEST(neon_ld1_d) {
  SETUP();

  uint8_t src[32 + 5];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Ldr(q2, MemOperand(x17));  // Initialise top 64-bits of Q register.
  __ Ld1(v2.V8B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1(v3.V8B(), v4.V8B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1(v5.V4H(), v6.V4H(), v7.V4H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1(v16.V2S(), v17.V2S(), v18.V2S(), v19.V2S(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1(v30.V2S(), v31.V2S(), v0.V2S(), v1.V2S(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1(v20.V1D(), v21.V1D(), v22.V1D(), v23.V1D(), MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x0706050403020100, q2);
  ASSERT_EQUAL_128(0, 0x0807060504030201, q3);
  ASSERT_EQUAL_128(0, 0x100f0e0d0c0b0a09, q4);
  ASSERT_EQUAL_128(0, 0x0908070605040302, q5);
  ASSERT_EQUAL_128(0, 0x11100f0e0d0c0b0a, q6);
  ASSERT_EQUAL_128(0, 0x1918171615141312, q7);
  ASSERT_EQUAL_128(0, 0x0a09080706050403, q16);
  ASSERT_EQUAL_128(0, 0x1211100f0e0d0c0b, q17);
  ASSERT_EQUAL_128(0, 0x1a19181716151413, q18);
  ASSERT_EQUAL_128(0, 0x2221201f1e1d1c1b, q19);
  ASSERT_EQUAL_128(0, 0x0b0a090807060504, q30);
  ASSERT_EQUAL_128(0, 0x131211100f0e0d0c, q31);
  ASSERT_EQUAL_128(0, 0x1b1a191817161514, q0);
  ASSERT_EQUAL_128(0, 0x232221201f1e1d1c, q1);
  ASSERT_EQUAL_128(0, 0x0c0b0a0908070605, q20);
  ASSERT_EQUAL_128(0, 0x14131211100f0e0d, q21);
  ASSERT_EQUAL_128(0, 0x1c1b1a1918171615, q22);
  ASSERT_EQUAL_128(0, 0x24232221201f1e1d, q23);

  TEARDOWN();
}


TEST(neon_ld1_d_postindex) {
  SETUP();

  uint8_t src[32 + 5];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base + 1);
  __ Mov(x19, src_base + 2);
  __ Mov(x20, src_base + 3);
  __ Mov(x21, src_base + 4);
  __ Mov(x22, src_base + 5);
  __ Mov(x23, 1);
  __ Ldr(q2, MemOperand(x17));  // Initialise top 64-bits of Q register.
  __ Ld1(v2.V8B(), MemOperand(x17, x23, PostIndex));
  __ Ld1(v3.V8B(), v4.V8B(), MemOperand(x18, 16, PostIndex));
  __ Ld1(v5.V4H(), v6.V4H(), v7.V4H(), MemOperand(x19, 24, PostIndex));
  __ Ld1(v16.V2S(), v17.V2S(), v18.V2S(), v19.V2S(),
         MemOperand(x20, 32, PostIndex));
  __ Ld1(v30.V2S(), v31.V2S(), v0.V2S(), v1.V2S(),
         MemOperand(x21, 32, PostIndex));
  __ Ld1(v20.V1D(), v21.V1D(), v22.V1D(), v23.V1D(),
         MemOperand(x22, 32, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x0706050403020100, q2);
  ASSERT_EQUAL_128(0, 0x0807060504030201, q3);
  ASSERT_EQUAL_128(0, 0x100f0e0d0c0b0a09, q4);
  ASSERT_EQUAL_128(0, 0x0908070605040302, q5);
  ASSERT_EQUAL_128(0, 0x11100f0e0d0c0b0a, q6);
  ASSERT_EQUAL_128(0, 0x1918171615141312, q7);
  ASSERT_EQUAL_128(0, 0x0a09080706050403, q16);
  ASSERT_EQUAL_128(0, 0x1211100f0e0d0c0b, q17);
  ASSERT_EQUAL_128(0, 0x1a19181716151413, q18);
  ASSERT_EQUAL_128(0, 0x2221201f1e1d1c1b, q19);
  ASSERT_EQUAL_128(0, 0x0b0a090807060504, q30);
  ASSERT_EQUAL_128(0, 0x131211100f0e0d0c, q31);
  ASSERT_EQUAL_128(0, 0x1b1a191817161514, q0);
  ASSERT_EQUAL_128(0, 0x232221201f1e1d1c, q1);
  ASSERT_EQUAL_128(0, 0x0c0b0a0908070605, q20);
  ASSERT_EQUAL_128(0, 0x14131211100f0e0d, q21);
  ASSERT_EQUAL_128(0, 0x1c1b1a1918171615, q22);
  ASSERT_EQUAL_128(0, 0x24232221201f1e1d, q23);
  ASSERT_EQUAL_64(src_base + 1, x17);
  ASSERT_EQUAL_64(src_base + 1 + 16, x18);
  ASSERT_EQUAL_64(src_base + 2 + 24, x19);
  ASSERT_EQUAL_64(src_base + 3 + 32, x20);
  ASSERT_EQUAL_64(src_base + 4 + 32, x21);
  ASSERT_EQUAL_64(src_base + 5 + 32, x22);

  TEARDOWN();
}


TEST(neon_ld1_q) {
  SETUP();

  uint8_t src[64 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Ld1(v2.V16B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1(v3.V16B(), v4.V16B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1(v5.V8H(), v6.V8H(), v7.V8H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1(v16.V4S(), v17.V4S(), v18.V4S(), v19.V4S(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1(v30.V2D(), v31.V2D(), v0.V2D(), v1.V2D(), MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q2);
  ASSERT_EQUAL_128(0x100f0e0d0c0b0a09, 0x0807060504030201, q3);
  ASSERT_EQUAL_128(0x201f1e1d1c1b1a19, 0x1817161514131211, q4);
  ASSERT_EQUAL_128(0x11100f0e0d0c0b0a, 0x0908070605040302, q5);
  ASSERT_EQUAL_128(0x21201f1e1d1c1b1a, 0x1918171615141312, q6);
  ASSERT_EQUAL_128(0x31302f2e2d2c2b2a, 0x2928272625242322, q7);
  ASSERT_EQUAL_128(0x1211100f0e0d0c0b, 0x0a09080706050403, q16);
  ASSERT_EQUAL_128(0x2221201f1e1d1c1b, 0x1a19181716151413, q17);
  ASSERT_EQUAL_128(0x3231302f2e2d2c2b, 0x2a29282726252423, q18);
  ASSERT_EQUAL_128(0x4241403f3e3d3c3b, 0x3a39383736353433, q19);
  ASSERT_EQUAL_128(0x131211100f0e0d0c, 0x0b0a090807060504, q30);
  ASSERT_EQUAL_128(0x232221201f1e1d1c, 0x1b1a191817161514, q31);
  ASSERT_EQUAL_128(0x333231302f2e2d2c, 0x2b2a292827262524, q0);
  ASSERT_EQUAL_128(0x434241403f3e3d3c, 0x3b3a393837363534, q1);

  TEARDOWN();
}


TEST(neon_ld1_q_postindex) {
  SETUP();

  uint8_t src[64 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base + 1);
  __ Mov(x19, src_base + 2);
  __ Mov(x20, src_base + 3);
  __ Mov(x21, src_base + 4);
  __ Mov(x22, 1);
  __ Ld1(v2.V16B(), MemOperand(x17, x22, PostIndex));
  __ Ld1(v3.V16B(), v4.V16B(), MemOperand(x18, 32, PostIndex));
  __ Ld1(v5.V8H(), v6.V8H(), v7.V8H(), MemOperand(x19, 48, PostIndex));
  __ Ld1(v16.V4S(), v17.V4S(), v18.V4S(), v19.V4S(),
         MemOperand(x20, 64, PostIndex));
  __ Ld1(v30.V2D(), v31.V2D(), v0.V2D(), v1.V2D(),
         MemOperand(x21, 64, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q2);
  ASSERT_EQUAL_128(0x100f0e0d0c0b0a09, 0x0807060504030201, q3);
  ASSERT_EQUAL_128(0x201f1e1d1c1b1a19, 0x1817161514131211, q4);
  ASSERT_EQUAL_128(0x11100f0e0d0c0b0a, 0x0908070605040302, q5);
  ASSERT_EQUAL_128(0x21201f1e1d1c1b1a, 0x1918171615141312, q6);
  ASSERT_EQUAL_128(0x31302f2e2d2c2b2a, 0x2928272625242322, q7);
  ASSERT_EQUAL_128(0x1211100f0e0d0c0b, 0x0a09080706050403, q16);
  ASSERT_EQUAL_128(0x2221201f1e1d1c1b, 0x1a19181716151413, q17);
  ASSERT_EQUAL_128(0x3231302f2e2d2c2b, 0x2a29282726252423, q18);
  ASSERT_EQUAL_128(0x4241403f3e3d3c3b, 0x3a39383736353433, q19);
  ASSERT_EQUAL_128(0x131211100f0e0d0c, 0x0b0a090807060504, q30);
  ASSERT_EQUAL_128(0x232221201f1e1d1c, 0x1b1a191817161514, q31);
  ASSERT_EQUAL_128(0x333231302f2e2d2c, 0x2b2a292827262524, q0);
  ASSERT_EQUAL_128(0x434241403f3e3d3c, 0x3b3a393837363534, q1);
  ASSERT_EQUAL_64(src_base + 1, x17);
  ASSERT_EQUAL_64(src_base + 1 + 32, x18);
  ASSERT_EQUAL_64(src_base + 2 + 48, x19);
  ASSERT_EQUAL_64(src_base + 3 + 64, x20);
  ASSERT_EQUAL_64(src_base + 4 + 64, x21);

  TEARDOWN();
}


TEST(neon_ld1_lane) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();

  // Test loading whole register by element.
  __ Mov(x17, src_base);
  for (int i = 15; i >= 0; i--) {
    __ Ld1(v0.B(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  __ Mov(x17, src_base);
  for (int i = 7; i >= 0; i--) {
    __ Ld1(v1.H(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  __ Mov(x17, src_base);
  for (int i = 3; i >= 0; i--) {
    __ Ld1(v2.S(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  __ Mov(x17, src_base);
  for (int i = 1; i >= 0; i--) {
    __ Ld1(v3.D(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  // Test loading a single element into an initialised register.
  __ Mov(x17, src_base);
  __ Ldr(q4, MemOperand(x17));
  __ Ld1(v4.B(), 4, MemOperand(x17));
  __ Ldr(q5, MemOperand(x17));
  __ Ld1(v5.H(), 3, MemOperand(x17));
  __ Ldr(q6, MemOperand(x17));
  __ Ld1(v6.S(), 2, MemOperand(x17));
  __ Ldr(q7, MemOperand(x17));
  __ Ld1(v7.D(), 1, MemOperand(x17));

  END();

  RUN();

  ASSERT_EQUAL_128(0x0001020304050607, 0x08090a0b0c0d0e0f, q0);
  ASSERT_EQUAL_128(0x0100020103020403, 0x0504060507060807, q1);
  ASSERT_EQUAL_128(0x0302010004030201, 0x0504030206050403, q2);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0807060504030201, q3);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050003020100, q4);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0100050403020100, q5);
  ASSERT_EQUAL_128(0x0f0e0d0c03020100, 0x0706050403020100, q6);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0706050403020100, q7);

  TEARDOWN();
}

TEST(neon_ld2_d) {
  SETUP();

  uint8_t src[64 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Ld2(v2.V8B(), v3.V8B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld2(v4.V8B(), v5.V8B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld2(v6.V4H(), v7.V4H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld2(v31.V2S(), v0.V2S(), MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x0e0c0a0806040200, q2);
  ASSERT_EQUAL_128(0, 0x0f0d0b0907050301, q3);
  ASSERT_EQUAL_128(0, 0x0f0d0b0907050301, q4);
  ASSERT_EQUAL_128(0, 0x100e0c0a08060402, q5);
  ASSERT_EQUAL_128(0, 0x0f0e0b0a07060302, q6);
  ASSERT_EQUAL_128(0, 0x11100d0c09080504, q7);
  ASSERT_EQUAL_128(0, 0x0e0d0c0b06050403, q31);
  ASSERT_EQUAL_128(0, 0x1211100f0a090807, q0);

  TEARDOWN();
}

TEST(neon_ld2_d_postindex) {
  SETUP();

  uint8_t src[32 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base + 1);
  __ Mov(x19, src_base + 2);
  __ Mov(x20, src_base + 3);
  __ Mov(x21, src_base + 4);
  __ Mov(x22, 1);
  __ Ld2(v2.V8B(), v3.V8B(), MemOperand(x17, x22, PostIndex));
  __ Ld2(v4.V8B(), v5.V8B(), MemOperand(x18, 16, PostIndex));
  __ Ld2(v5.V4H(), v6.V4H(), MemOperand(x19, 16, PostIndex));
  __ Ld2(v16.V2S(), v17.V2S(), MemOperand(x20, 16, PostIndex));
  __ Ld2(v31.V2S(), v0.V2S(), MemOperand(x21, 16, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x0e0c0a0806040200, q2);
  ASSERT_EQUAL_128(0, 0x0f0d0b0907050301, q3);
  ASSERT_EQUAL_128(0, 0x0f0d0b0907050301, q4);
  ASSERT_EQUAL_128(0, 0x0f0e0b0a07060302, q5);
  ASSERT_EQUAL_128(0, 0x11100d0c09080504, q6);
  ASSERT_EQUAL_128(0, 0x0e0d0c0b06050403, q16);
  ASSERT_EQUAL_128(0, 0x1211100f0a090807, q17);
  ASSERT_EQUAL_128(0, 0x0f0e0d0c07060504, q31);
  ASSERT_EQUAL_128(0, 0x131211100b0a0908, q0);

  ASSERT_EQUAL_64(src_base + 1, x17);
  ASSERT_EQUAL_64(src_base + 1 + 16, x18);
  ASSERT_EQUAL_64(src_base + 2 + 16, x19);
  ASSERT_EQUAL_64(src_base + 3 + 16, x20);
  ASSERT_EQUAL_64(src_base + 4 + 16, x21);

  TEARDOWN();
}


TEST(neon_ld2_q) {
  SETUP();

  uint8_t src[64 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Ld2(v2.V16B(), v3.V16B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld2(v4.V16B(), v5.V16B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld2(v6.V8H(), v7.V8H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld2(v16.V4S(), v17.V4S(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld2(v31.V2D(), v0.V2D(), MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0x1e1c1a1816141210, 0x0e0c0a0806040200, q2);
  ASSERT_EQUAL_128(0x1f1d1b1917151311, 0x0f0d0b0907050301, q3);
  ASSERT_EQUAL_128(0x1f1d1b1917151311, 0x0f0d0b0907050301, q4);
  ASSERT_EQUAL_128(0x201e1c1a18161412, 0x100e0c0a08060402, q5);
  ASSERT_EQUAL_128(0x1f1e1b1a17161312, 0x0f0e0b0a07060302, q6);
  ASSERT_EQUAL_128(0x21201d1c19181514, 0x11100d0c09080504, q7);
  ASSERT_EQUAL_128(0x1e1d1c1b16151413, 0x0e0d0c0b06050403, q16);
  ASSERT_EQUAL_128(0x2221201f1a191817, 0x1211100f0a090807, q17);
  ASSERT_EQUAL_128(0x1b1a191817161514, 0x0b0a090807060504, q31);
  ASSERT_EQUAL_128(0x232221201f1e1d1c, 0x131211100f0e0d0c, q0);

  TEARDOWN();
}


TEST(neon_ld2_q_postindex) {
  SETUP();

  uint8_t src[64 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base + 1);
  __ Mov(x19, src_base + 2);
  __ Mov(x20, src_base + 3);
  __ Mov(x21, src_base + 4);
  __ Mov(x22, 1);
  __ Ld2(v2.V16B(), v3.V16B(), MemOperand(x17, x22, PostIndex));
  __ Ld2(v4.V16B(), v5.V16B(), MemOperand(x18, 32, PostIndex));
  __ Ld2(v6.V8H(), v7.V8H(), MemOperand(x19, 32, PostIndex));
  __ Ld2(v16.V4S(), v17.V4S(), MemOperand(x20, 32, PostIndex));
  __ Ld2(v31.V2D(), v0.V2D(), MemOperand(x21, 32, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0x1e1c1a1816141210, 0x0e0c0a0806040200, q2);
  ASSERT_EQUAL_128(0x1f1d1b1917151311, 0x0f0d0b0907050301, q3);
  ASSERT_EQUAL_128(0x1f1d1b1917151311, 0x0f0d0b0907050301, q4);
  ASSERT_EQUAL_128(0x201e1c1a18161412, 0x100e0c0a08060402, q5);
  ASSERT_EQUAL_128(0x1f1e1b1a17161312, 0x0f0e0b0a07060302, q6);
  ASSERT_EQUAL_128(0x21201d1c19181514, 0x11100d0c09080504, q7);
  ASSERT_EQUAL_128(0x1e1d1c1b16151413, 0x0e0d0c0b06050403, q16);
  ASSERT_EQUAL_128(0x2221201f1a191817, 0x1211100f0a090807, q17);
  ASSERT_EQUAL_128(0x1b1a191817161514, 0x0b0a090807060504, q31);
  ASSERT_EQUAL_128(0x232221201f1e1d1c, 0x131211100f0e0d0c, q0);



  ASSERT_EQUAL_64(src_base + 1, x17);
  ASSERT_EQUAL_64(src_base + 1 + 32, x18);
  ASSERT_EQUAL_64(src_base + 2 + 32, x19);
  ASSERT_EQUAL_64(src_base + 3 + 32, x20);
  ASSERT_EQUAL_64(src_base + 4 + 32, x21);

  TEARDOWN();
}


TEST(neon_ld2_lane) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();

  // Test loading whole register by element.
  __ Mov(x17, src_base);
  for (int i = 15; i >= 0; i--) {
    __ Ld2(v0.B(), v1.B(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  __ Mov(x17, src_base);
  for (int i = 7; i >= 0; i--) {
    __ Ld2(v2.H(), v3.H(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  __ Mov(x17, src_base);
  for (int i = 3; i >= 0; i--) {
    __ Ld2(v4.S(), v5.S(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  __ Mov(x17, src_base);
  for (int i = 1; i >= 0; i--) {
    __ Ld2(v6.D(), v7.D(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  // Test loading a single element into an initialised register.
  __ Mov(x17, src_base);
  __ Mov(x4, x17);
  __ Ldr(q8, MemOperand(x4, 16, PostIndex));
  __ Ldr(q9, MemOperand(x4));
  __ Ld2(v8.B(), v9.B(), 4, MemOperand(x17));
  __ Mov(x5, x17);
  __ Ldr(q10, MemOperand(x5, 16, PostIndex));
  __ Ldr(q11, MemOperand(x5));
  __ Ld2(v10.H(), v11.H(), 3, MemOperand(x17));
  __ Mov(x6, x17);
  __ Ldr(q12, MemOperand(x6, 16, PostIndex));
  __ Ldr(q13, MemOperand(x6));
  __ Ld2(v12.S(), v13.S(), 2, MemOperand(x17));
  __ Mov(x7, x17);
  __ Ldr(q14, MemOperand(x7, 16, PostIndex));
  __ Ldr(q15, MemOperand(x7));
  __ Ld2(v14.D(), v15.D(), 1, MemOperand(x17));

  END();

  RUN();

  ASSERT_EQUAL_128(0x0001020304050607, 0x08090a0b0c0d0e0f, q0);
  ASSERT_EQUAL_128(0x0102030405060708, 0x090a0b0c0d0e0f10, q1);
  ASSERT_EQUAL_128(0x0100020103020403, 0x0504060507060807, q2);
  ASSERT_EQUAL_128(0x0302040305040605, 0x0706080709080a09, q3);
  ASSERT_EQUAL_128(0x0302010004030201, 0x0504030206050403, q4);
  ASSERT_EQUAL_128(0x0706050408070605, 0x090807060a090807, q5);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0807060504030201, q6);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x100f0e0d0c0b0a09, q7);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050003020100, q8);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716150113121110, q9);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0100050403020100, q10);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x0302151413121110, q11);
  ASSERT_EQUAL_128(0x0f0e0d0c03020100, 0x0706050403020100, q12);
  ASSERT_EQUAL_128(0x1f1e1d1c07060504, 0x1716151413121110, q13);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0706050403020100, q14);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x1716151413121110, q15);

  TEARDOWN();
}


TEST(neon_ld2_lane_postindex) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Mov(x19, src_base);
  __ Mov(x20, src_base);
  __ Mov(x21, src_base);
  __ Mov(x22, src_base);
  __ Mov(x23, src_base);
  __ Mov(x24, src_base);

  // Test loading whole register by element.
  for (int i = 15; i >= 0; i--) {
    __ Ld2(v0.B(), v1.B(), i, MemOperand(x17, 2, PostIndex));
  }

  for (int i = 7; i >= 0; i--) {
    __ Ld2(v2.H(), v3.H(), i, MemOperand(x18, 4, PostIndex));
  }

  for (int i = 3; i >= 0; i--) {
    __ Ld2(v4.S(), v5.S(), i, MemOperand(x19, 8, PostIndex));
  }

  for (int i = 1; i >= 0; i--) {
    __ Ld2(v6.D(), v7.D(), i, MemOperand(x20, 16, PostIndex));
  }

  // Test loading a single element into an initialised register.
  __ Mov(x25, 1);
  __ Mov(x4, x21);
  __ Ldr(q8, MemOperand(x4, 16, PostIndex));
  __ Ldr(q9, MemOperand(x4));
  __ Ld2(v8.B(), v9.B(), 4, MemOperand(x21, x25, PostIndex));
  __ Add(x25, x25, 1);

  __ Mov(x5, x22);
  __ Ldr(q10, MemOperand(x5, 16, PostIndex));
  __ Ldr(q11, MemOperand(x5));
  __ Ld2(v10.H(), v11.H(), 3, MemOperand(x22, x25, PostIndex));
  __ Add(x25, x25, 1);

  __ Mov(x6, x23);
  __ Ldr(q12, MemOperand(x6, 16, PostIndex));
  __ Ldr(q13, MemOperand(x6));
  __ Ld2(v12.S(), v13.S(), 2, MemOperand(x23, x25, PostIndex));
  __ Add(x25, x25, 1);

  __ Mov(x7, x24);
  __ Ldr(q14, MemOperand(x7, 16, PostIndex));
  __ Ldr(q15, MemOperand(x7));
  __ Ld2(v14.D(), v15.D(), 1, MemOperand(x24, x25, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x00020406080a0c0e, 0x10121416181a1c1e, q0);
  ASSERT_EQUAL_128(0x01030507090b0d0f, 0x11131517191b1d1f, q1);
  ASSERT_EQUAL_128(0x0100050409080d0c, 0x1110151419181d1c, q2);
  ASSERT_EQUAL_128(0x030207060b0a0f0e, 0x131217161b1a1f1e, q3);
  ASSERT_EQUAL_128(0x030201000b0a0908, 0x131211101b1a1918, q4);
  ASSERT_EQUAL_128(0x070605040f0e0d0c, 0x171615141f1e1d1c, q5);
  ASSERT_EQUAL_128(0x0706050403020100, 0x1716151413121110, q6);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x1f1e1d1c1b1a1918, q7);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050003020100, q8);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716150113121110, q9);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0100050403020100, q10);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x0302151413121110, q11);
  ASSERT_EQUAL_128(0x0f0e0d0c03020100, 0x0706050403020100, q12);
  ASSERT_EQUAL_128(0x1f1e1d1c07060504, 0x1716151413121110, q13);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0706050403020100, q14);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x1716151413121110, q15);




  ASSERT_EQUAL_64(src_base + 32, x17);
  ASSERT_EQUAL_64(src_base + 32, x18);
  ASSERT_EQUAL_64(src_base + 32, x19);
  ASSERT_EQUAL_64(src_base + 32, x20);
  ASSERT_EQUAL_64(src_base + 1, x21);
  ASSERT_EQUAL_64(src_base + 2, x22);
  ASSERT_EQUAL_64(src_base + 3, x23);
  ASSERT_EQUAL_64(src_base + 4, x24);

  TEARDOWN();
}


TEST(neon_ld2_alllanes) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base + 1);
  __ Mov(x18, 1);
  __ Ld2r(v0.V8B(), v1.V8B(), MemOperand(x17));
  __ Add(x17, x17, 2);
  __ Ld2r(v2.V16B(), v3.V16B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld2r(v4.V4H(), v5.V4H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld2r(v6.V8H(), v7.V8H(), MemOperand(x17));
  __ Add(x17, x17, 4);
  __ Ld2r(v8.V2S(), v9.V2S(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld2r(v10.V4S(), v11.V4S(), MemOperand(x17));
  __ Add(x17, x17, 8);
  __ Ld2r(v12.V2D(), v13.V2D(), MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x0101010101010101, q0);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0202020202020202, q1);
  ASSERT_EQUAL_128(0x0303030303030303, 0x0303030303030303, q2);
  ASSERT_EQUAL_128(0x0404040404040404, 0x0404040404040404, q3);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0504050405040504, q4);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0706070607060706, q5);
  ASSERT_EQUAL_128(0x0605060506050605, 0x0605060506050605, q6);
  ASSERT_EQUAL_128(0x0807080708070807, 0x0807080708070807, q7);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0c0b0a090c0b0a09, q8);
  ASSERT_EQUAL_128(0x0000000000000000, 0x100f0e0d100f0e0d, q9);
  ASSERT_EQUAL_128(0x0d0c0b0a0d0c0b0a, 0x0d0c0b0a0d0c0b0a, q10);
  ASSERT_EQUAL_128(0x11100f0e11100f0e, 0x11100f0e11100f0e, q11);
  ASSERT_EQUAL_128(0x1918171615141312, 0x1918171615141312, q12);
  ASSERT_EQUAL_128(0x21201f1e1d1c1b1a, 0x21201f1e1d1c1b1a, q13);

  TEARDOWN();
}


TEST(neon_ld2_alllanes_postindex) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base + 1);
  __ Mov(x18, 1);
  __ Ld2r(v0.V8B(), v1.V8B(), MemOperand(x17, 2, PostIndex));
  __ Ld2r(v2.V16B(), v3.V16B(), MemOperand(x17, x18, PostIndex));
  __ Ld2r(v4.V4H(), v5.V4H(), MemOperand(x17, x18, PostIndex));
  __ Ld2r(v6.V8H(), v7.V8H(), MemOperand(x17, 4, PostIndex));
  __ Ld2r(v8.V2S(), v9.V2S(), MemOperand(x17, x18, PostIndex));
  __ Ld2r(v10.V4S(), v11.V4S(), MemOperand(x17, 8, PostIndex));
  __ Ld2r(v12.V2D(), v13.V2D(), MemOperand(x17, 16, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x0101010101010101, q0);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0202020202020202, q1);
  ASSERT_EQUAL_128(0x0303030303030303, 0x0303030303030303, q2);
  ASSERT_EQUAL_128(0x0404040404040404, 0x0404040404040404, q3);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0504050405040504, q4);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0706070607060706, q5);
  ASSERT_EQUAL_128(0x0605060506050605, 0x0605060506050605, q6);
  ASSERT_EQUAL_128(0x0807080708070807, 0x0807080708070807, q7);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0c0b0a090c0b0a09, q8);
  ASSERT_EQUAL_128(0x0000000000000000, 0x100f0e0d100f0e0d, q9);
  ASSERT_EQUAL_128(0x0d0c0b0a0d0c0b0a, 0x0d0c0b0a0d0c0b0a, q10);
  ASSERT_EQUAL_128(0x11100f0e11100f0e, 0x11100f0e11100f0e, q11);
  ASSERT_EQUAL_128(0x1918171615141312, 0x1918171615141312, q12);
  ASSERT_EQUAL_128(0x21201f1e1d1c1b1a, 0x21201f1e1d1c1b1a, q13);
  ASSERT_EQUAL_64(src_base + 34, x17);

  TEARDOWN();
}


TEST(neon_ld3_d) {
  SETUP();

  uint8_t src[64 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Ld3(v2.V8B(), v3.V8B(), v4.V8B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld3(v5.V8B(), v6.V8B(), v7.V8B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld3(v8.V4H(), v9.V4H(), v10.V4H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld3(v31.V2S(), v0.V2S(), v1.V2S(), MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x15120f0c09060300, q2);
  ASSERT_EQUAL_128(0, 0x1613100d0a070401, q3);
  ASSERT_EQUAL_128(0, 0x1714110e0b080502, q4);
  ASSERT_EQUAL_128(0, 0x1613100d0a070401, q5);
  ASSERT_EQUAL_128(0, 0x1714110e0b080502, q6);
  ASSERT_EQUAL_128(0, 0x1815120f0c090603, q7);
  ASSERT_EQUAL_128(0, 0x15140f0e09080302, q8);
  ASSERT_EQUAL_128(0, 0x171611100b0a0504, q9);
  ASSERT_EQUAL_128(0, 0x191813120d0c0706, q10);
  ASSERT_EQUAL_128(0, 0x1211100f06050403, q31);
  ASSERT_EQUAL_128(0, 0x161514130a090807, q0);
  ASSERT_EQUAL_128(0, 0x1a1918170e0d0c0b, q1);

  TEARDOWN();
}


TEST(neon_ld3_d_postindex) {
  SETUP();

  uint8_t src[32 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base + 1);
  __ Mov(x19, src_base + 2);
  __ Mov(x20, src_base + 3);
  __ Mov(x21, src_base + 4);
  __ Mov(x22, 1);
  __ Ld3(v2.V8B(), v3.V8B(), v4.V8B(), MemOperand(x17, x22, PostIndex));
  __ Ld3(v5.V8B(), v6.V8B(), v7.V8B(), MemOperand(x18, 24, PostIndex));
  __ Ld3(v8.V4H(), v9.V4H(), v10.V4H(), MemOperand(x19, 24, PostIndex));
  __ Ld3(v11.V2S(), v12.V2S(), v13.V2S(), MemOperand(x20, 24, PostIndex));
  __ Ld3(v31.V2S(), v0.V2S(), v1.V2S(), MemOperand(x21, 24, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x15120f0c09060300, q2);
  ASSERT_EQUAL_128(0, 0x1613100d0a070401, q3);
  ASSERT_EQUAL_128(0, 0x1714110e0b080502, q4);
  ASSERT_EQUAL_128(0, 0x1613100d0a070401, q5);
  ASSERT_EQUAL_128(0, 0x1714110e0b080502, q6);
  ASSERT_EQUAL_128(0, 0x1815120f0c090603, q7);
  ASSERT_EQUAL_128(0, 0x15140f0e09080302, q8);
  ASSERT_EQUAL_128(0, 0x171611100b0a0504, q9);
  ASSERT_EQUAL_128(0, 0x191813120d0c0706, q10);
  ASSERT_EQUAL_128(0, 0x1211100f06050403, q11);
  ASSERT_EQUAL_128(0, 0x161514130a090807, q12);
  ASSERT_EQUAL_128(0, 0x1a1918170e0d0c0b, q13);
  ASSERT_EQUAL_128(0, 0x1312111007060504, q31);
  ASSERT_EQUAL_128(0, 0x171615140b0a0908, q0);
  ASSERT_EQUAL_128(0, 0x1b1a19180f0e0d0c, q1);

  ASSERT_EQUAL_64(src_base + 1, x17);
  ASSERT_EQUAL_64(src_base + 1 + 24, x18);
  ASSERT_EQUAL_64(src_base + 2 + 24, x19);
  ASSERT_EQUAL_64(src_base + 3 + 24, x20);
  ASSERT_EQUAL_64(src_base + 4 + 24, x21);

  TEARDOWN();
}


TEST(neon_ld3_q) {
  SETUP();

  uint8_t src[64 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Ld3(v2.V16B(), v3.V16B(), v4.V16B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld3(v5.V16B(), v6.V16B(), v7.V16B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld3(v8.V8H(), v9.V8H(), v10.V8H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld3(v11.V4S(), v12.V4S(), v13.V4S(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld3(v31.V2D(), v0.V2D(), v1.V2D(), MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0x2d2a2724211e1b18, 0x15120f0c09060300, q2);
  ASSERT_EQUAL_128(0x2e2b2825221f1c19, 0x1613100d0a070401, q3);
  ASSERT_EQUAL_128(0x2f2c292623201d1a, 0x1714110e0b080502, q4);
  ASSERT_EQUAL_128(0x2e2b2825221f1c19, 0x1613100d0a070401, q5);
  ASSERT_EQUAL_128(0x2f2c292623201d1a, 0x1714110e0b080502, q6);
  ASSERT_EQUAL_128(0x302d2a2724211e1b, 0x1815120f0c090603, q7);
  ASSERT_EQUAL_128(0x2d2c272621201b1a, 0x15140f0e09080302, q8);
  ASSERT_EQUAL_128(0x2f2e292823221d1c, 0x171611100b0a0504, q9);
  ASSERT_EQUAL_128(0x31302b2a25241f1e, 0x191813120d0c0706, q10);
  ASSERT_EQUAL_128(0x2a2928271e1d1c1b, 0x1211100f06050403, q11);
  ASSERT_EQUAL_128(0x2e2d2c2b2221201f, 0x161514130a090807, q12);
  ASSERT_EQUAL_128(0x3231302f26252423, 0x1a1918170e0d0c0b, q13);
  ASSERT_EQUAL_128(0x232221201f1e1d1c, 0x0b0a090807060504, q31);
  ASSERT_EQUAL_128(0x2b2a292827262524, 0x131211100f0e0d0c, q0);
  ASSERT_EQUAL_128(0x333231302f2e2d2c, 0x1b1a191817161514, q1);

  TEARDOWN();
}


TEST(neon_ld3_q_postindex) {
  SETUP();

  uint8_t src[64 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base + 1);
  __ Mov(x19, src_base + 2);
  __ Mov(x20, src_base + 3);
  __ Mov(x21, src_base + 4);
  __ Mov(x22, 1);

  __ Ld3(v2.V16B(), v3.V16B(), v4.V16B(), MemOperand(x17, x22, PostIndex));
  __ Ld3(v5.V16B(), v6.V16B(), v7.V16B(), MemOperand(x18, 48, PostIndex));
  __ Ld3(v8.V8H(), v9.V8H(), v10.V8H(), MemOperand(x19, 48, PostIndex));
  __ Ld3(v11.V4S(), v12.V4S(), v13.V4S(), MemOperand(x20, 48, PostIndex));
  __ Ld3(v31.V2D(), v0.V2D(), v1.V2D(), MemOperand(x21, 48, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0x2d2a2724211e1b18, 0x15120f0c09060300, q2);
  ASSERT_EQUAL_128(0x2e2b2825221f1c19, 0x1613100d0a070401, q3);
  ASSERT_EQUAL_128(0x2f2c292623201d1a, 0x1714110e0b080502, q4);
  ASSERT_EQUAL_128(0x2e2b2825221f1c19, 0x1613100d0a070401, q5);
  ASSERT_EQUAL_128(0x2f2c292623201d1a, 0x1714110e0b080502, q6);
  ASSERT_EQUAL_128(0x302d2a2724211e1b, 0x1815120f0c090603, q7);
  ASSERT_EQUAL_128(0x2d2c272621201b1a, 0x15140f0e09080302, q8);
  ASSERT_EQUAL_128(0x2f2e292823221d1c, 0x171611100b0a0504, q9);
  ASSERT_EQUAL_128(0x31302b2a25241f1e, 0x191813120d0c0706, q10);
  ASSERT_EQUAL_128(0x2a2928271e1d1c1b, 0x1211100f06050403, q11);
  ASSERT_EQUAL_128(0x2e2d2c2b2221201f, 0x161514130a090807, q12);
  ASSERT_EQUAL_128(0x3231302f26252423, 0x1a1918170e0d0c0b, q13);
  ASSERT_EQUAL_128(0x232221201f1e1d1c, 0x0b0a090807060504, q31);
  ASSERT_EQUAL_128(0x2b2a292827262524, 0x131211100f0e0d0c, q0);
  ASSERT_EQUAL_128(0x333231302f2e2d2c, 0x1b1a191817161514, q1);

  ASSERT_EQUAL_64(src_base + 1, x17);
  ASSERT_EQUAL_64(src_base + 1 + 48, x18);
  ASSERT_EQUAL_64(src_base + 2 + 48, x19);
  ASSERT_EQUAL_64(src_base + 3 + 48, x20);
  ASSERT_EQUAL_64(src_base + 4 + 48, x21);

  TEARDOWN();
}


TEST(neon_ld3_lane) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();

  // Test loading whole register by element.
  __ Mov(x17, src_base);
  for (int i = 15; i >= 0; i--) {
    __ Ld3(v0.B(), v1.B(), v2.B(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  __ Mov(x17, src_base);
  for (int i = 7; i >= 0; i--) {
    __ Ld3(v3.H(), v4.H(), v5.H(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  __ Mov(x17, src_base);
  for (int i = 3; i >= 0; i--) {
    __ Ld3(v6.S(), v7.S(), v8.S(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  __ Mov(x17, src_base);
  for (int i = 1; i >= 0; i--) {
    __ Ld3(v9.D(), v10.D(), v11.D(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  // Test loading a single element into an initialised register.
  __ Mov(x17, src_base);
  __ Mov(x4, x17);
  __ Ldr(q12, MemOperand(x4, 16, PostIndex));
  __ Ldr(q13, MemOperand(x4, 16, PostIndex));
  __ Ldr(q14, MemOperand(x4));
  __ Ld3(v12.B(), v13.B(), v14.B(), 4, MemOperand(x17));
  __ Mov(x5, x17);
  __ Ldr(q15, MemOperand(x5, 16, PostIndex));
  __ Ldr(q16, MemOperand(x5, 16, PostIndex));
  __ Ldr(q17, MemOperand(x5));
  __ Ld3(v15.H(), v16.H(), v17.H(), 3, MemOperand(x17));
  __ Mov(x6, x17);
  __ Ldr(q18, MemOperand(x6, 16, PostIndex));
  __ Ldr(q19, MemOperand(x6, 16, PostIndex));
  __ Ldr(q20, MemOperand(x6));
  __ Ld3(v18.S(), v19.S(), v20.S(), 2, MemOperand(x17));
  __ Mov(x7, x17);
  __ Ldr(q21, MemOperand(x7, 16, PostIndex));
  __ Ldr(q22, MemOperand(x7, 16, PostIndex));
  __ Ldr(q23, MemOperand(x7));
  __ Ld3(v21.D(), v22.D(), v23.D(), 1, MemOperand(x17));

  END();

  RUN();

  ASSERT_EQUAL_128(0x0001020304050607, 0x08090a0b0c0d0e0f, q0);
  ASSERT_EQUAL_128(0x0102030405060708, 0x090a0b0c0d0e0f10, q1);
  ASSERT_EQUAL_128(0x0203040506070809, 0x0a0b0c0d0e0f1011, q2);
  ASSERT_EQUAL_128(0x0100020103020403, 0x0504060507060807, q3);
  ASSERT_EQUAL_128(0x0302040305040605, 0x0706080709080a09, q4);
  ASSERT_EQUAL_128(0x0504060507060807, 0x09080a090b0a0c0b, q5);
  ASSERT_EQUAL_128(0x0302010004030201, 0x0504030206050403, q6);
  ASSERT_EQUAL_128(0x0706050408070605, 0x090807060a090807, q7);
  ASSERT_EQUAL_128(0x0b0a09080c0b0a09, 0x0d0c0b0a0e0d0c0b, q8);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0807060504030201, q9);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x100f0e0d0c0b0a09, q10);
  ASSERT_EQUAL_128(0x1716151413121110, 0x1817161514131211, q11);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050003020100, q12);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716150113121110, q13);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726250223222120, q14);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0100050403020100, q15);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x0302151413121110, q16);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x0504252423222120, q17);

  TEARDOWN();
}


TEST(neon_ld3_lane_postindex) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();

  // Test loading whole register by element.
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Mov(x19, src_base);
  __ Mov(x20, src_base);
  __ Mov(x21, src_base);
  __ Mov(x22, src_base);
  __ Mov(x23, src_base);
  __ Mov(x24, src_base);
  for (int i = 15; i >= 0; i--) {
    __ Ld3(v0.B(), v1.B(), v2.B(), i, MemOperand(x17, 3, PostIndex));
  }

  for (int i = 7; i >= 0; i--) {
    __ Ld3(v3.H(), v4.H(), v5.H(), i, MemOperand(x18, 6, PostIndex));
  }

  for (int i = 3; i >= 0; i--) {
    __ Ld3(v6.S(), v7.S(), v8.S(), i, MemOperand(x19, 12, PostIndex));
  }

  for (int i = 1; i >= 0; i--) {
    __ Ld3(v9.D(), v10.D(), v11.D(), i, MemOperand(x20, 24, PostIndex));
  }


  // Test loading a single element into an initialised register.
  __ Mov(x25, 1);
  __ Mov(x4, x21);
  __ Ldr(q12, MemOperand(x4, 16, PostIndex));
  __ Ldr(q13, MemOperand(x4, 16, PostIndex));
  __ Ldr(q14, MemOperand(x4));
  __ Ld3(v12.B(), v13.B(), v14.B(), 4, MemOperand(x21, x25, PostIndex));
  __ Add(x25, x25, 1);

  __ Mov(x5, x22);
  __ Ldr(q15, MemOperand(x5, 16, PostIndex));
  __ Ldr(q16, MemOperand(x5, 16, PostIndex));
  __ Ldr(q17, MemOperand(x5));
  __ Ld3(v15.H(), v16.H(), v17.H(), 3, MemOperand(x22, x25, PostIndex));
  __ Add(x25, x25, 1);

  __ Mov(x6, x23);
  __ Ldr(q18, MemOperand(x6, 16, PostIndex));
  __ Ldr(q19, MemOperand(x6, 16, PostIndex));
  __ Ldr(q20, MemOperand(x6));
  __ Ld3(v18.S(), v19.S(), v20.S(), 2, MemOperand(x23, x25, PostIndex));
  __ Add(x25, x25, 1);

  __ Mov(x7, x24);
  __ Ldr(q21, MemOperand(x7, 16, PostIndex));
  __ Ldr(q22, MemOperand(x7, 16, PostIndex));
  __ Ldr(q23, MemOperand(x7));
  __ Ld3(v21.D(), v22.D(), v23.D(), 1, MemOperand(x24, x25, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x000306090c0f1215, 0x181b1e2124272a2d, q0);
  ASSERT_EQUAL_128(0x0104070a0d101316, 0x191c1f2225282b2e, q1);
  ASSERT_EQUAL_128(0x0205080b0e111417, 0x1a1d202326292c2f, q2);
  ASSERT_EQUAL_128(0x010007060d0c1312, 0x19181f1e25242b2a, q3);
  ASSERT_EQUAL_128(0x030209080f0e1514, 0x1b1a212027262d2c, q4);
  ASSERT_EQUAL_128(0x05040b0a11101716, 0x1d1c232229282f2e, q5);
  ASSERT_EQUAL_128(0x030201000f0e0d0c, 0x1b1a191827262524, q6);
  ASSERT_EQUAL_128(0x0706050413121110, 0x1f1e1d1c2b2a2928, q7);
  ASSERT_EQUAL_128(0x0b0a090817161514, 0x232221202f2e2d2c, q8);
  ASSERT_EQUAL_128(0x0706050403020100, 0x1f1e1d1c1b1a1918, q9);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x2726252423222120, q10);
  ASSERT_EQUAL_128(0x1716151413121110, 0x2f2e2d2c2b2a2928, q11);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050003020100, q12);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716150113121110, q13);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726250223222120, q14);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0100050403020100, q15);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x0302151413121110, q16);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x0504252423222120, q17);
  ASSERT_EQUAL_128(0x0f0e0d0c03020100, 0x0706050403020100, q18);
  ASSERT_EQUAL_128(0x1f1e1d1c07060504, 0x1716151413121110, q19);
  ASSERT_EQUAL_128(0x2f2e2d2c0b0a0908, 0x2726252423222120, q20);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0706050403020100, q21);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x1716151413121110, q22);
  ASSERT_EQUAL_128(0x1716151413121110, 0x2726252423222120, q23);

  ASSERT_EQUAL_64(src_base + 48, x17);
  ASSERT_EQUAL_64(src_base + 48, x18);
  ASSERT_EQUAL_64(src_base + 48, x19);
  ASSERT_EQUAL_64(src_base + 48, x20);
  ASSERT_EQUAL_64(src_base + 1, x21);
  ASSERT_EQUAL_64(src_base + 2, x22);
  ASSERT_EQUAL_64(src_base + 3, x23);
  ASSERT_EQUAL_64(src_base + 4, x24);

  TEARDOWN();
}


TEST(neon_ld3_alllanes) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base + 1);
  __ Mov(x18, 1);
  __ Ld3r(v0.V8B(), v1.V8B(), v2.V8B(), MemOperand(x17));
  __ Add(x17, x17, 3);
  __ Ld3r(v3.V16B(), v4.V16B(), v5.V16B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld3r(v6.V4H(), v7.V4H(), v8.V4H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld3r(v9.V8H(), v10.V8H(), v11.V8H(), MemOperand(x17));
  __ Add(x17, x17, 6);
  __ Ld3r(v12.V2S(), v13.V2S(), v14.V2S(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld3r(v15.V4S(), v16.V4S(), v17.V4S(), MemOperand(x17));
  __ Add(x17, x17, 12);
  __ Ld3r(v18.V2D(), v19.V2D(), v20.V2D(), MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x0101010101010101, q0);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0202020202020202, q1);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0303030303030303, q2);
  ASSERT_EQUAL_128(0x0404040404040404, 0x0404040404040404, q3);
  ASSERT_EQUAL_128(0x0505050505050505, 0x0505050505050505, q4);
  ASSERT_EQUAL_128(0x0606060606060606, 0x0606060606060606, q5);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0605060506050605, q6);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0807080708070807, q7);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0a090a090a090a09, q8);
  ASSERT_EQUAL_128(0x0706070607060706, 0x0706070607060706, q9);
  ASSERT_EQUAL_128(0x0908090809080908, 0x0908090809080908, q10);
  ASSERT_EQUAL_128(0x0b0a0b0a0b0a0b0a, 0x0b0a0b0a0b0a0b0a, q11);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0f0e0d0c0f0e0d0c, q12);
  ASSERT_EQUAL_128(0x0000000000000000, 0x1312111013121110, q13);
  ASSERT_EQUAL_128(0x0000000000000000, 0x1716151417161514, q14);
  ASSERT_EQUAL_128(0x100f0e0d100f0e0d, 0x100f0e0d100f0e0d, q15);
  ASSERT_EQUAL_128(0x1413121114131211, 0x1413121114131211, q16);
  ASSERT_EQUAL_128(0x1817161518171615, 0x1817161518171615, q17);
  ASSERT_EQUAL_128(0x201f1e1d1c1b1a19, 0x201f1e1d1c1b1a19, q18);
  ASSERT_EQUAL_128(0x2827262524232221, 0x2827262524232221, q19);
  ASSERT_EQUAL_128(0x302f2e2d2c2b2a29, 0x302f2e2d2c2b2a29, q20);

  TEARDOWN();
}


TEST(neon_ld3_alllanes_postindex) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  __ Mov(x17, src_base + 1);
  __ Mov(x18, 1);

  START();
  __ Mov(x17, src_base + 1);
  __ Mov(x18, 1);
  __ Ld3r(v0.V8B(), v1.V8B(), v2.V8B(), MemOperand(x17, 3, PostIndex));
  __ Ld3r(v3.V16B(), v4.V16B(), v5.V16B(), MemOperand(x17, x18, PostIndex));
  __ Ld3r(v6.V4H(), v7.V4H(), v8.V4H(), MemOperand(x17, x18, PostIndex));
  __ Ld3r(v9.V8H(), v10.V8H(), v11.V8H(), MemOperand(x17, 6, PostIndex));
  __ Ld3r(v12.V2S(), v13.V2S(), v14.V2S(), MemOperand(x17, x18, PostIndex));
  __ Ld3r(v15.V4S(), v16.V4S(), v17.V4S(), MemOperand(x17, 12, PostIndex));
  __ Ld3r(v18.V2D(), v19.V2D(), v20.V2D(), MemOperand(x17, 24, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x0101010101010101, q0);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0202020202020202, q1);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0303030303030303, q2);
  ASSERT_EQUAL_128(0x0404040404040404, 0x0404040404040404, q3);
  ASSERT_EQUAL_128(0x0505050505050505, 0x0505050505050505, q4);
  ASSERT_EQUAL_128(0x0606060606060606, 0x0606060606060606, q5);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0605060506050605, q6);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0807080708070807, q7);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0a090a090a090a09, q8);
  ASSERT_EQUAL_128(0x0706070607060706, 0x0706070607060706, q9);
  ASSERT_EQUAL_128(0x0908090809080908, 0x0908090809080908, q10);
  ASSERT_EQUAL_128(0x0b0a0b0a0b0a0b0a, 0x0b0a0b0a0b0a0b0a, q11);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0f0e0d0c0f0e0d0c, q12);
  ASSERT_EQUAL_128(0x0000000000000000, 0x1312111013121110, q13);
  ASSERT_EQUAL_128(0x0000000000000000, 0x1716151417161514, q14);
  ASSERT_EQUAL_128(0x100f0e0d100f0e0d, 0x100f0e0d100f0e0d, q15);
  ASSERT_EQUAL_128(0x1413121114131211, 0x1413121114131211, q16);
  ASSERT_EQUAL_128(0x1817161518171615, 0x1817161518171615, q17);
  ASSERT_EQUAL_128(0x201f1e1d1c1b1a19, 0x201f1e1d1c1b1a19, q18);
  ASSERT_EQUAL_128(0x2827262524232221, 0x2827262524232221, q19);
  ASSERT_EQUAL_128(0x302f2e2d2c2b2a29, 0x302f2e2d2c2b2a29, q20);

  TEARDOWN();
}


TEST(neon_ld4_d) {
  SETUP();

  uint8_t src[64 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Ld4(v2.V8B(), v3.V8B(), v4.V8B(), v5.V8B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld4(v6.V8B(), v7.V8B(), v8.V8B(), v9.V8B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld4(v10.V4H(), v11.V4H(), v12.V4H(), v13.V4H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld4(v30.V2S(), v31.V2S(), v0.V2S(), v1.V2S(), MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x1c1814100c080400, q2);
  ASSERT_EQUAL_128(0, 0x1d1915110d090501, q3);
  ASSERT_EQUAL_128(0, 0x1e1a16120e0a0602, q4);
  ASSERT_EQUAL_128(0, 0x1f1b17130f0b0703, q5);
  ASSERT_EQUAL_128(0, 0x1d1915110d090501, q6);
  ASSERT_EQUAL_128(0, 0x1e1a16120e0a0602, q7);
  ASSERT_EQUAL_128(0, 0x1f1b17130f0b0703, q8);
  ASSERT_EQUAL_128(0, 0x201c1814100c0804, q9);
  ASSERT_EQUAL_128(0, 0x1b1a13120b0a0302, q10);
  ASSERT_EQUAL_128(0, 0x1d1c15140d0c0504, q11);
  ASSERT_EQUAL_128(0, 0x1f1e17160f0e0706, q12);
  ASSERT_EQUAL_128(0, 0x2120191811100908, q13);
  ASSERT_EQUAL_128(0, 0x1615141306050403, q30);
  ASSERT_EQUAL_128(0, 0x1a1918170a090807, q31);
  ASSERT_EQUAL_128(0, 0x1e1d1c1b0e0d0c0b, q0);
  ASSERT_EQUAL_128(0, 0x2221201f1211100f, q1);

  TEARDOWN();
}


TEST(neon_ld4_d_postindex) {
  SETUP();

  uint8_t src[32 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base + 1);
  __ Mov(x19, src_base + 2);
  __ Mov(x20, src_base + 3);
  __ Mov(x21, src_base + 4);
  __ Mov(x22, 1);
  __ Ld4(v2.V8B(), v3.V8B(), v4.V8B(), v5.V8B(),
         MemOperand(x17, x22, PostIndex));
  __ Ld4(v6.V8B(), v7.V8B(), v8.V8B(), v9.V8B(),
         MemOperand(x18, 32, PostIndex));
  __ Ld4(v10.V4H(), v11.V4H(), v12.V4H(), v13.V4H(),
         MemOperand(x19, 32, PostIndex));
  __ Ld4(v14.V2S(), v15.V2S(), v16.V2S(), v17.V2S(),
         MemOperand(x20, 32, PostIndex));
  __ Ld4(v30.V2S(), v31.V2S(), v0.V2S(), v1.V2S(),
         MemOperand(x21, 32, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x1c1814100c080400, q2);
  ASSERT_EQUAL_128(0, 0x1d1915110d090501, q3);
  ASSERT_EQUAL_128(0, 0x1e1a16120e0a0602, q4);
  ASSERT_EQUAL_128(0, 0x1f1b17130f0b0703, q5);
  ASSERT_EQUAL_128(0, 0x1d1915110d090501, q6);
  ASSERT_EQUAL_128(0, 0x1e1a16120e0a0602, q7);
  ASSERT_EQUAL_128(0, 0x1f1b17130f0b0703, q8);
  ASSERT_EQUAL_128(0, 0x201c1814100c0804, q9);
  ASSERT_EQUAL_128(0, 0x1b1a13120b0a0302, q10);
  ASSERT_EQUAL_128(0, 0x1d1c15140d0c0504, q11);
  ASSERT_EQUAL_128(0, 0x1f1e17160f0e0706, q12);
  ASSERT_EQUAL_128(0, 0x2120191811100908, q13);
  ASSERT_EQUAL_128(0, 0x1615141306050403, q14);
  ASSERT_EQUAL_128(0, 0x1a1918170a090807, q15);
  ASSERT_EQUAL_128(0, 0x1e1d1c1b0e0d0c0b, q16);
  ASSERT_EQUAL_128(0, 0x2221201f1211100f, q17);
  ASSERT_EQUAL_128(0, 0x1716151407060504, q30);
  ASSERT_EQUAL_128(0, 0x1b1a19180b0a0908, q31);
  ASSERT_EQUAL_128(0, 0x1f1e1d1c0f0e0d0c, q0);
  ASSERT_EQUAL_128(0, 0x2322212013121110, q1);


  ASSERT_EQUAL_64(src_base + 1, x17);
  ASSERT_EQUAL_64(src_base + 1 + 32, x18);
  ASSERT_EQUAL_64(src_base + 2 + 32, x19);
  ASSERT_EQUAL_64(src_base + 3 + 32, x20);
  ASSERT_EQUAL_64(src_base + 4 + 32, x21);
  TEARDOWN();
}


TEST(neon_ld4_q) {
  SETUP();

  uint8_t src[64 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Ld4(v2.V16B(), v3.V16B(), v4.V16B(), v5.V16B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld4(v6.V16B(), v7.V16B(), v8.V16B(), v9.V16B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld4(v10.V8H(), v11.V8H(), v12.V8H(), v13.V8H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld4(v14.V4S(), v15.V4S(), v16.V4S(), v17.V4S(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld4(v18.V2D(), v19.V2D(), v20.V2D(), v21.V2D(), MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0x3c3834302c282420, 0x1c1814100c080400, q2);
  ASSERT_EQUAL_128(0x3d3935312d292521, 0x1d1915110d090501, q3);
  ASSERT_EQUAL_128(0x3e3a36322e2a2622, 0x1e1a16120e0a0602, q4);
  ASSERT_EQUAL_128(0x3f3b37332f2b2723, 0x1f1b17130f0b0703, q5);
  ASSERT_EQUAL_128(0x3d3935312d292521, 0x1d1915110d090501, q6);
  ASSERT_EQUAL_128(0x3e3a36322e2a2622, 0x1e1a16120e0a0602, q7);
  ASSERT_EQUAL_128(0x3f3b37332f2b2723, 0x1f1b17130f0b0703, q8);
  ASSERT_EQUAL_128(0x403c3834302c2824, 0x201c1814100c0804, q9);
  ASSERT_EQUAL_128(0x3b3a33322b2a2322, 0x1b1a13120b0a0302, q10);
  ASSERT_EQUAL_128(0x3d3c35342d2c2524, 0x1d1c15140d0c0504, q11);
  ASSERT_EQUAL_128(0x3f3e37362f2e2726, 0x1f1e17160f0e0706, q12);
  ASSERT_EQUAL_128(0x4140393831302928, 0x2120191811100908, q13);
  ASSERT_EQUAL_128(0x3635343326252423, 0x1615141306050403, q14);
  ASSERT_EQUAL_128(0x3a3938372a292827, 0x1a1918170a090807, q15);
  ASSERT_EQUAL_128(0x3e3d3c3b2e2d2c2b, 0x1e1d1c1b0e0d0c0b, q16);
  ASSERT_EQUAL_128(0x4241403f3231302f, 0x2221201f1211100f, q17);
  ASSERT_EQUAL_128(0x2b2a292827262524, 0x0b0a090807060504, q18);
  ASSERT_EQUAL_128(0x333231302f2e2d2c, 0x131211100f0e0d0c, q19);
  ASSERT_EQUAL_128(0x3b3a393837363534, 0x1b1a191817161514, q20);
  ASSERT_EQUAL_128(0x434241403f3e3d3c, 0x232221201f1e1d1c, q21);
  TEARDOWN();
}


TEST(neon_ld4_q_postindex) {
  SETUP();

  uint8_t src[64 + 4];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base + 1);
  __ Mov(x19, src_base + 2);
  __ Mov(x20, src_base + 3);
  __ Mov(x21, src_base + 4);
  __ Mov(x22, 1);

  __ Ld4(v2.V16B(), v3.V16B(), v4.V16B(), v5.V16B(),
         MemOperand(x17, x22, PostIndex));
  __ Ld4(v6.V16B(), v7.V16B(), v8.V16B(), v9.V16B(),
         MemOperand(x18, 64, PostIndex));
  __ Ld4(v10.V8H(), v11.V8H(), v12.V8H(), v13.V8H(),
         MemOperand(x19, 64, PostIndex));
  __ Ld4(v14.V4S(), v15.V4S(), v16.V4S(), v17.V4S(),
         MemOperand(x20, 64, PostIndex));
  __ Ld4(v30.V2D(), v31.V2D(), v0.V2D(), v1.V2D(),
         MemOperand(x21, 64, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0x3c3834302c282420, 0x1c1814100c080400, q2);
  ASSERT_EQUAL_128(0x3d3935312d292521, 0x1d1915110d090501, q3);
  ASSERT_EQUAL_128(0x3e3a36322e2a2622, 0x1e1a16120e0a0602, q4);
  ASSERT_EQUAL_128(0x3f3b37332f2b2723, 0x1f1b17130f0b0703, q5);
  ASSERT_EQUAL_128(0x3d3935312d292521, 0x1d1915110d090501, q6);
  ASSERT_EQUAL_128(0x3e3a36322e2a2622, 0x1e1a16120e0a0602, q7);
  ASSERT_EQUAL_128(0x3f3b37332f2b2723, 0x1f1b17130f0b0703, q8);
  ASSERT_EQUAL_128(0x403c3834302c2824, 0x201c1814100c0804, q9);
  ASSERT_EQUAL_128(0x3b3a33322b2a2322, 0x1b1a13120b0a0302, q10);
  ASSERT_EQUAL_128(0x3d3c35342d2c2524, 0x1d1c15140d0c0504, q11);
  ASSERT_EQUAL_128(0x3f3e37362f2e2726, 0x1f1e17160f0e0706, q12);
  ASSERT_EQUAL_128(0x4140393831302928, 0x2120191811100908, q13);
  ASSERT_EQUAL_128(0x3635343326252423, 0x1615141306050403, q14);
  ASSERT_EQUAL_128(0x3a3938372a292827, 0x1a1918170a090807, q15);
  ASSERT_EQUAL_128(0x3e3d3c3b2e2d2c2b, 0x1e1d1c1b0e0d0c0b, q16);
  ASSERT_EQUAL_128(0x4241403f3231302f, 0x2221201f1211100f, q17);
  ASSERT_EQUAL_128(0x2b2a292827262524, 0x0b0a090807060504, q30);
  ASSERT_EQUAL_128(0x333231302f2e2d2c, 0x131211100f0e0d0c, q31);
  ASSERT_EQUAL_128(0x3b3a393837363534, 0x1b1a191817161514, q0);
  ASSERT_EQUAL_128(0x434241403f3e3d3c, 0x232221201f1e1d1c, q1);



  ASSERT_EQUAL_64(src_base + 1, x17);
  ASSERT_EQUAL_64(src_base + 1 + 64, x18);
  ASSERT_EQUAL_64(src_base + 2 + 64, x19);
  ASSERT_EQUAL_64(src_base + 3 + 64, x20);
  ASSERT_EQUAL_64(src_base + 4 + 64, x21);

  TEARDOWN();
}


TEST(neon_ld4_lane) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();

  // Test loading whole register by element.
  __ Mov(x17, src_base);
  for (int i = 15; i >= 0; i--) {
    __ Ld4(v0.B(), v1.B(), v2.B(), v3.B(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  __ Mov(x17, src_base);
  for (int i = 7; i >= 0; i--) {
    __ Ld4(v4.H(), v5.H(), v6.H(), v7.H(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  __ Mov(x17, src_base);
  for (int i = 3; i >= 0; i--) {
    __ Ld4(v8.S(), v9.S(), v10.S(), v11.S(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  __ Mov(x17, src_base);
  for (int i = 1; i >= 0; i--) {
    __ Ld4(v12.D(), v13.D(), v14.D(), v15.D(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }

  // Test loading a single element into an initialised register.
  __ Mov(x17, src_base);
  __ Mov(x4, x17);
  __ Ldr(q16, MemOperand(x4, 16, PostIndex));
  __ Ldr(q17, MemOperand(x4, 16, PostIndex));
  __ Ldr(q18, MemOperand(x4, 16, PostIndex));
  __ Ldr(q19, MemOperand(x4));
  __ Ld4(v16.B(), v17.B(), v18.B(), v19.B(), 4, MemOperand(x17));

  __ Mov(x5, x17);
  __ Ldr(q20, MemOperand(x5, 16, PostIndex));
  __ Ldr(q21, MemOperand(x5, 16, PostIndex));
  __ Ldr(q22, MemOperand(x5, 16, PostIndex));
  __ Ldr(q23, MemOperand(x5));
  __ Ld4(v20.H(), v21.H(), v22.H(), v23.H(), 3, MemOperand(x17));

  __ Mov(x6, x17);
  __ Ldr(q24, MemOperand(x6, 16, PostIndex));
  __ Ldr(q25, MemOperand(x6, 16, PostIndex));
  __ Ldr(q26, MemOperand(x6, 16, PostIndex));
  __ Ldr(q27, MemOperand(x6));
  __ Ld4(v24.S(), v25.S(), v26.S(), v27.S(), 2, MemOperand(x17));

  __ Mov(x7, x17);
  __ Ldr(q28, MemOperand(x7, 16, PostIndex));
  __ Ldr(q29, MemOperand(x7, 16, PostIndex));
  __ Ldr(q30, MemOperand(x7, 16, PostIndex));
  __ Ldr(q31, MemOperand(x7));
  __ Ld4(v28.D(), v29.D(), v30.D(), v31.D(), 1, MemOperand(x17));

  END();

  RUN();

  ASSERT_EQUAL_128(0x0001020304050607, 0x08090a0b0c0d0e0f, q0);
  ASSERT_EQUAL_128(0x0102030405060708, 0x090a0b0c0d0e0f10, q1);
  ASSERT_EQUAL_128(0x0203040506070809, 0x0a0b0c0d0e0f1011, q2);
  ASSERT_EQUAL_128(0x030405060708090a, 0x0b0c0d0e0f101112, q3);
  ASSERT_EQUAL_128(0x0100020103020403, 0x0504060507060807, q4);
  ASSERT_EQUAL_128(0x0302040305040605, 0x0706080709080a09, q5);
  ASSERT_EQUAL_128(0x0504060507060807, 0x09080a090b0a0c0b, q6);
  ASSERT_EQUAL_128(0x0706080709080a09, 0x0b0a0c0b0d0c0e0d, q7);
  ASSERT_EQUAL_128(0x0302010004030201, 0x0504030206050403, q8);
  ASSERT_EQUAL_128(0x0706050408070605, 0x090807060a090807, q9);
  ASSERT_EQUAL_128(0x0b0a09080c0b0a09, 0x0d0c0b0a0e0d0c0b, q10);
  ASSERT_EQUAL_128(0x0f0e0d0c100f0e0d, 0x11100f0e1211100f, q11);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0807060504030201, q12);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x100f0e0d0c0b0a09, q13);
  ASSERT_EQUAL_128(0x1716151413121110, 0x1817161514131211, q14);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x201f1e1d1c1b1a19, q15);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050003020100, q16);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716150113121110, q17);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726250223222120, q18);
  ASSERT_EQUAL_128(0x3f3e3d3c3b3a3938, 0x3736350333323130, q19);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0100050403020100, q20);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x0302151413121110, q21);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x0504252423222120, q22);
  ASSERT_EQUAL_128(0x3f3e3d3c3b3a3938, 0x0706353433323130, q23);
  ASSERT_EQUAL_128(0x0f0e0d0c03020100, 0x0706050403020100, q24);
  ASSERT_EQUAL_128(0x1f1e1d1c07060504, 0x1716151413121110, q25);
  ASSERT_EQUAL_128(0x2f2e2d2c0b0a0908, 0x2726252423222120, q26);
  ASSERT_EQUAL_128(0x3f3e3d3c0f0e0d0c, 0x3736353433323130, q27);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0706050403020100, q28);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x1716151413121110, q29);
  ASSERT_EQUAL_128(0x1716151413121110, 0x2726252423222120, q30);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x3736353433323130, q31);

  TEARDOWN();
}



TEST(neon_ld4_lane_postindex) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();

  // Test loading whole register by element.
  __ Mov(x17, src_base);
  for (int i = 15; i >= 0; i--) {
    __ Ld4(v0.B(), v1.B(), v2.B(), v3.B(), i,
           MemOperand(x17, 4, PostIndex));
  }

  __ Mov(x18, src_base);
  for (int i = 7; i >= 0; i--) {
    __ Ld4(v4.H(), v5.H(), v6.H(), v7.H(), i,
           MemOperand(x18, 8, PostIndex));
  }

  __ Mov(x19, src_base);
  for (int i = 3; i >= 0; i--) {
    __ Ld4(v8.S(), v9.S(), v10.S(), v11.S(), i,
           MemOperand(x19, 16, PostIndex));
  }

  __ Mov(x20, src_base);
  for (int i = 1; i >= 0; i--) {
    __ Ld4(v12.D(), v13.D(), v14.D(), v15.D(), i,
           MemOperand(x20, 32, PostIndex));
  }

  // Test loading a single element into an initialised register.
  __ Mov(x25, 1);
  __ Mov(x21, src_base);
  __ Mov(x22, src_base);
  __ Mov(x23, src_base);
  __ Mov(x24, src_base);

  __ Mov(x4, x21);
  __ Ldr(q16, MemOperand(x4, 16, PostIndex));
  __ Ldr(q17, MemOperand(x4, 16, PostIndex));
  __ Ldr(q18, MemOperand(x4, 16, PostIndex));
  __ Ldr(q19, MemOperand(x4));
  __ Ld4(v16.B(), v17.B(), v18.B(), v19.B(), 4,
         MemOperand(x21, x25, PostIndex));
  __ Add(x25, x25, 1);

  __ Mov(x5, x22);
  __ Ldr(q20, MemOperand(x5, 16, PostIndex));
  __ Ldr(q21, MemOperand(x5, 16, PostIndex));
  __ Ldr(q22, MemOperand(x5, 16, PostIndex));
  __ Ldr(q23, MemOperand(x5));
  __ Ld4(v20.H(), v21.H(), v22.H(), v23.H(), 3,
         MemOperand(x22, x25, PostIndex));
  __ Add(x25, x25, 1);

  __ Mov(x6, x23);
  __ Ldr(q24, MemOperand(x6, 16, PostIndex));
  __ Ldr(q25, MemOperand(x6, 16, PostIndex));
  __ Ldr(q26, MemOperand(x6, 16, PostIndex));
  __ Ldr(q27, MemOperand(x6));
  __ Ld4(v24.S(), v25.S(), v26.S(), v27.S(), 2,
         MemOperand(x23, x25, PostIndex));
  __ Add(x25, x25, 1);

  __ Mov(x7, x24);
  __ Ldr(q28, MemOperand(x7, 16, PostIndex));
  __ Ldr(q29, MemOperand(x7, 16, PostIndex));
  __ Ldr(q30, MemOperand(x7, 16, PostIndex));
  __ Ldr(q31, MemOperand(x7));
  __ Ld4(v28.D(), v29.D(), v30.D(), v31.D(), 1,
         MemOperand(x24, x25, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x0004080c1014181c, 0x2024282c3034383c, q0);
  ASSERT_EQUAL_128(0x0105090d1115191d, 0x2125292d3135393d, q1);
  ASSERT_EQUAL_128(0x02060a0e12161a1e, 0x22262a2e32363a3e, q2);
  ASSERT_EQUAL_128(0x03070b0f13171b1f, 0x23272b2f33373b3f, q3);
  ASSERT_EQUAL_128(0x0100090811101918, 0x2120292831303938, q4);
  ASSERT_EQUAL_128(0x03020b0a13121b1a, 0x23222b2a33323b3a, q5);
  ASSERT_EQUAL_128(0x05040d0c15141d1c, 0x25242d2c35343d3c, q6);
  ASSERT_EQUAL_128(0x07060f0e17161f1e, 0x27262f2e37363f3e, q7);
  ASSERT_EQUAL_128(0x0302010013121110, 0x2322212033323130, q8);
  ASSERT_EQUAL_128(0x0706050417161514, 0x2726252437363534, q9);
  ASSERT_EQUAL_128(0x0b0a09081b1a1918, 0x2b2a29283b3a3938, q10);
  ASSERT_EQUAL_128(0x0f0e0d0c1f1e1d1c, 0x2f2e2d2c3f3e3d3c, q11);
  ASSERT_EQUAL_128(0x0706050403020100, 0x2726252423222120, q12);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x2f2e2d2c2b2a2928, q13);
  ASSERT_EQUAL_128(0x1716151413121110, 0x3736353433323130, q14);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x3f3e3d3c3b3a3938, q15);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050003020100, q16);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716150113121110, q17);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726250223222120, q18);
  ASSERT_EQUAL_128(0x3f3e3d3c3b3a3938, 0x3736350333323130, q19);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0100050403020100, q20);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x0302151413121110, q21);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x0504252423222120, q22);
  ASSERT_EQUAL_128(0x3f3e3d3c3b3a3938, 0x0706353433323130, q23);
  ASSERT_EQUAL_128(0x0f0e0d0c03020100, 0x0706050403020100, q24);
  ASSERT_EQUAL_128(0x1f1e1d1c07060504, 0x1716151413121110, q25);
  ASSERT_EQUAL_128(0x2f2e2d2c0b0a0908, 0x2726252423222120, q26);
  ASSERT_EQUAL_128(0x3f3e3d3c0f0e0d0c, 0x3736353433323130, q27);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0706050403020100, q28);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x1716151413121110, q29);
  ASSERT_EQUAL_128(0x1716151413121110, 0x2726252423222120, q30);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x3736353433323130, q31);

  ASSERT_EQUAL_64(src_base + 64, x17);
  ASSERT_EQUAL_64(src_base + 64, x18);
  ASSERT_EQUAL_64(src_base + 64, x19);
  ASSERT_EQUAL_64(src_base + 64, x20);
  ASSERT_EQUAL_64(src_base + 1, x21);
  ASSERT_EQUAL_64(src_base + 2, x22);
  ASSERT_EQUAL_64(src_base + 3, x23);
  ASSERT_EQUAL_64(src_base + 4, x24);

  TEARDOWN();
}


TEST(neon_ld4_alllanes) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base + 1);
  __ Mov(x18, 1);
  __ Ld4r(v0.V8B(), v1.V8B(), v2.V8B(), v3.V8B(), MemOperand(x17));
  __ Add(x17, x17, 4);
  __ Ld4r(v4.V16B(), v5.V16B(), v6.V16B(), v7.V16B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld4r(v8.V4H(), v9.V4H(), v10.V4H(), v11.V4H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld4r(v12.V8H(), v13.V8H(), v14.V8H(), v15.V8H(), MemOperand(x17));
  __ Add(x17, x17, 8);
  __ Ld4r(v16.V2S(), v17.V2S(), v18.V2S(), v19.V2S(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld4r(v20.V4S(), v21.V4S(), v22.V4S(), v23.V4S(), MemOperand(x17));
  __ Add(x17, x17, 16);
  __ Ld4r(v24.V2D(), v25.V2D(), v26.V2D(), v27.V2D(), MemOperand(x17));


  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x0101010101010101, q0);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0202020202020202, q1);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0303030303030303, q2);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0404040404040404, q3);
  ASSERT_EQUAL_128(0x0505050505050505, 0x0505050505050505, q4);
  ASSERT_EQUAL_128(0x0606060606060606, 0x0606060606060606, q5);
  ASSERT_EQUAL_128(0x0707070707070707, 0x0707070707070707, q6);
  ASSERT_EQUAL_128(0x0808080808080808, 0x0808080808080808, q7);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0706070607060706, q8);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0908090809080908, q9);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0b0a0b0a0b0a0b0a, q10);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0d0c0d0c0d0c0d0c, q11);
  ASSERT_EQUAL_128(0x0807080708070807, 0x0807080708070807, q12);
  ASSERT_EQUAL_128(0x0a090a090a090a09, 0x0a090a090a090a09, q13);
  ASSERT_EQUAL_128(0x0c0b0c0b0c0b0c0b, 0x0c0b0c0b0c0b0c0b, q14);
  ASSERT_EQUAL_128(0x0e0d0e0d0e0d0e0d, 0x0e0d0e0d0e0d0e0d, q15);
  ASSERT_EQUAL_128(0x0000000000000000, 0x1211100f1211100f, q16);
  ASSERT_EQUAL_128(0x0000000000000000, 0x1615141316151413, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x1a1918171a191817, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x1e1d1c1b1e1d1c1b, q19);
  ASSERT_EQUAL_128(0x1312111013121110, 0x1312111013121110, q20);
  ASSERT_EQUAL_128(0x1716151417161514, 0x1716151417161514, q21);
  ASSERT_EQUAL_128(0x1b1a19181b1a1918, 0x1b1a19181b1a1918, q22);
  ASSERT_EQUAL_128(0x1f1e1d1c1f1e1d1c, 0x1f1e1d1c1f1e1d1c, q23);
  ASSERT_EQUAL_128(0x2726252423222120, 0x2726252423222120, q24);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2f2e2d2c2b2a2928, q25);
  ASSERT_EQUAL_128(0x3736353433323130, 0x3736353433323130, q26);
  ASSERT_EQUAL_128(0x3f3e3d3c3b3a3938, 0x3f3e3d3c3b3a3938, q27);

  TEARDOWN();
}


TEST(neon_ld4_alllanes_postindex) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  __ Mov(x17, src_base + 1);
  __ Mov(x18, 1);

  START();
  __ Mov(x17, src_base + 1);
  __ Mov(x18, 1);
  __ Ld4r(v0.V8B(), v1.V8B(), v2.V8B(), v3.V8B(),
          MemOperand(x17, 4, PostIndex));
  __ Ld4r(v4.V16B(), v5.V16B(), v6.V16B(), v7.V16B(),
          MemOperand(x17, x18, PostIndex));
  __ Ld4r(v8.V4H(), v9.V4H(), v10.V4H(), v11.V4H(),
          MemOperand(x17, x18, PostIndex));
  __ Ld4r(v12.V8H(), v13.V8H(), v14.V8H(), v15.V8H(),
          MemOperand(x17, 8, PostIndex));
  __ Ld4r(v16.V2S(), v17.V2S(), v18.V2S(), v19.V2S(),
          MemOperand(x17, x18, PostIndex));
  __ Ld4r(v20.V4S(), v21.V4S(), v22.V4S(), v23.V4S(),
          MemOperand(x17, 16, PostIndex));
  __ Ld4r(v24.V2D(), v25.V2D(), v26.V2D(), v27.V2D(),
          MemOperand(x17, 32, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x0101010101010101, q0);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0202020202020202, q1);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0303030303030303, q2);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0404040404040404, q3);
  ASSERT_EQUAL_128(0x0505050505050505, 0x0505050505050505, q4);
  ASSERT_EQUAL_128(0x0606060606060606, 0x0606060606060606, q5);
  ASSERT_EQUAL_128(0x0707070707070707, 0x0707070707070707, q6);
  ASSERT_EQUAL_128(0x0808080808080808, 0x0808080808080808, q7);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0706070607060706, q8);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0908090809080908, q9);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0b0a0b0a0b0a0b0a, q10);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0d0c0d0c0d0c0d0c, q11);
  ASSERT_EQUAL_128(0x0807080708070807, 0x0807080708070807, q12);
  ASSERT_EQUAL_128(0x0a090a090a090a09, 0x0a090a090a090a09, q13);
  ASSERT_EQUAL_128(0x0c0b0c0b0c0b0c0b, 0x0c0b0c0b0c0b0c0b, q14);
  ASSERT_EQUAL_128(0x0e0d0e0d0e0d0e0d, 0x0e0d0e0d0e0d0e0d, q15);
  ASSERT_EQUAL_128(0x0000000000000000, 0x1211100f1211100f, q16);
  ASSERT_EQUAL_128(0x0000000000000000, 0x1615141316151413, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x1a1918171a191817, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x1e1d1c1b1e1d1c1b, q19);
  ASSERT_EQUAL_128(0x1312111013121110, 0x1312111013121110, q20);
  ASSERT_EQUAL_128(0x1716151417161514, 0x1716151417161514, q21);
  ASSERT_EQUAL_128(0x1b1a19181b1a1918, 0x1b1a19181b1a1918, q22);
  ASSERT_EQUAL_128(0x1f1e1d1c1f1e1d1c, 0x1f1e1d1c1f1e1d1c, q23);
  ASSERT_EQUAL_128(0x2726252423222120, 0x2726252423222120, q24);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2f2e2d2c2b2a2928, q25);
  ASSERT_EQUAL_128(0x3736353433323130, 0x3736353433323130, q26);
  ASSERT_EQUAL_128(0x3f3e3d3c3b3a3938, 0x3f3e3d3c3b3a3938, q27);
  ASSERT_EQUAL_64(src_base + 64, x17);

  TEARDOWN();
}


TEST(neon_st1_lane) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, -16);
  __ Ldr(q0, MemOperand(x17));

  for (int i = 15; i >= 0; i--) {
    __ St1(v0.B(), i, MemOperand(x17));
    __ Add(x17, x17, 1);
  }
  __ Ldr(q1, MemOperand(x17, x18));

  for (int i = 7; i >= 0; i--) {
    __ St1(v0.H(), i, MemOperand(x17));
    __ Add(x17, x17, 2);
  }
  __ Ldr(q2, MemOperand(x17, x18));

  for (int i = 3; i >= 0; i--) {
    __ St1(v0.S(), i, MemOperand(x17));
    __ Add(x17, x17, 4);
  }
  __ Ldr(q3, MemOperand(x17, x18));

  for (int i = 1; i >= 0; i--) {
    __ St1(v0.D(), i, MemOperand(x17));
    __ Add(x17, x17, 8);
  }
  __ Ldr(q4, MemOperand(x17, x18));

  END();

  RUN();

  ASSERT_EQUAL_128(0x0001020304050607, 0x08090a0b0c0d0e0f, q1);
  ASSERT_EQUAL_128(0x0100030205040706, 0x09080b0a0d0c0f0e, q2);
  ASSERT_EQUAL_128(0x0302010007060504, 0x0b0a09080f0e0d0c, q3);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0f0e0d0c0b0a0908, q4);

  TEARDOWN();
}


TEST(neon_st2_lane) {
  SETUP();

  // Struct size * addressing modes * element sizes * vector size.
  uint8_t dst[2 * 2 * 4 * 16];
  memset(dst, 0, sizeof(dst));
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, dst_base);
  __ Mov(x18, dst_base);
  __ Movi(v0.V2D(), 0x0001020304050607, 0x08090a0b0c0d0e0f);
  __ Movi(v1.V2D(), 0x1011121314151617, 0x18191a1b1c1d1e1f);

  // Test B stores with and without post index.
  for (int i = 15; i >= 0; i--) {
    __ St2(v0.B(), v1.B(), i, MemOperand(x18));
    __ Add(x18, x18, 2);
  }
  for (int i = 15; i >= 0; i--) {
    __ St2(v0.B(), v1.B(), i, MemOperand(x18, 2, PostIndex));
  }
  __ Ldr(q2, MemOperand(x17, 0 * 16));
  __ Ldr(q3, MemOperand(x17, 1 * 16));
  __ Ldr(q4, MemOperand(x17, 2 * 16));
  __ Ldr(q5, MemOperand(x17, 3 * 16));

  // Test H stores with and without post index.
  __ Mov(x0, 4);
  for (int i = 7; i >= 0; i--) {
    __ St2(v0.H(), v1.H(), i, MemOperand(x18));
    __ Add(x18, x18, 4);
  }
  for (int i = 7; i >= 0; i--) {
    __ St2(v0.H(), v1.H(), i, MemOperand(x18, x0, PostIndex));
  }
  __ Ldr(q6, MemOperand(x17, 4 * 16));
  __ Ldr(q7, MemOperand(x17, 5 * 16));
  __ Ldr(q16, MemOperand(x17, 6 * 16));
  __ Ldr(q17, MemOperand(x17, 7 * 16));

  // Test S stores with and without post index.
  for (int i = 3; i >= 0; i--) {
    __ St2(v0.S(), v1.S(), i, MemOperand(x18));
    __ Add(x18, x18, 8);
  }
  for (int i = 3; i >= 0; i--) {
    __ St2(v0.S(), v1.S(), i, MemOperand(x18, 8, PostIndex));
  }
  __ Ldr(q18, MemOperand(x17, 8 * 16));
  __ Ldr(q19, MemOperand(x17, 9 * 16));
  __ Ldr(q20, MemOperand(x17, 10 * 16));
  __ Ldr(q21, MemOperand(x17, 11 * 16));

  // Test D stores with and without post index.
  __ Mov(x0, 16);
  __ St2(v0.D(), v1.D(), 1, MemOperand(x18));
  __ Add(x18, x18, 16);
  __ St2(v0.D(), v1.D(), 0, MemOperand(x18, 16, PostIndex));
  __ St2(v0.D(), v1.D(), 1, MemOperand(x18, x0, PostIndex));
  __ St2(v0.D(), v1.D(), 0, MemOperand(x18, x0, PostIndex));
  __ Ldr(q22, MemOperand(x17, 12 * 16));
  __ Ldr(q23, MemOperand(x17, 13 * 16));
  __ Ldr(q24, MemOperand(x17, 14 * 16));
  __ Ldr(q25, MemOperand(x17, 15 * 16));
  END();

  RUN();

  ASSERT_EQUAL_128(0x1707160615051404, 0x1303120211011000, q2);
  ASSERT_EQUAL_128(0x1f0f1e0e1d0d1c0c, 0x1b0b1a0a19091808, q3);
  ASSERT_EQUAL_128(0x1707160615051404, 0x1303120211011000, q4);
  ASSERT_EQUAL_128(0x1f0f1e0e1d0d1c0c, 0x1b0b1a0a19091808, q5);

  ASSERT_EQUAL_128(0x1617060714150405, 0x1213020310110001, q6);
  ASSERT_EQUAL_128(0x1e1f0e0f1c1d0c0d, 0x1a1b0a0b18190809, q7);
  ASSERT_EQUAL_128(0x1617060714150405, 0x1213020310110001, q16);
  ASSERT_EQUAL_128(0x1e1f0e0f1c1d0c0d, 0x1a1b0a0b18190809, q17);

  ASSERT_EQUAL_128(0x1415161704050607, 0x1011121300010203, q18);
  ASSERT_EQUAL_128(0x1c1d1e1f0c0d0e0f, 0x18191a1b08090a0b, q19);
  ASSERT_EQUAL_128(0x1415161704050607, 0x1011121300010203, q20);
  ASSERT_EQUAL_128(0x1c1d1e1f0c0d0e0f, 0x18191a1b08090a0b, q21);

  ASSERT_EQUAL_128(0x1011121314151617, 0x0001020304050607, q22);
  ASSERT_EQUAL_128(0x18191a1b1c1d1e1f, 0x08090a0b0c0d0e0f, q23);
  ASSERT_EQUAL_128(0x1011121314151617, 0x0001020304050607, q22);
  ASSERT_EQUAL_128(0x18191a1b1c1d1e1f, 0x08090a0b0c0d0e0f, q23);

  TEARDOWN();
}


TEST(neon_st3_lane) {
  SETUP();

  // Struct size * addressing modes * element sizes * vector size.
  uint8_t dst[3 * 2 * 4 * 16];
  memset(dst, 0, sizeof(dst));
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, dst_base);
  __ Mov(x18, dst_base);
  __ Movi(v0.V2D(), 0x0001020304050607, 0x08090a0b0c0d0e0f);
  __ Movi(v1.V2D(), 0x1011121314151617, 0x18191a1b1c1d1e1f);
  __ Movi(v2.V2D(), 0x2021222324252627, 0x28292a2b2c2d2e2f);

  // Test B stores with and without post index.
  for (int i = 15; i >= 0; i--) {
    __ St3(v0.B(), v1.B(), v2.B(), i, MemOperand(x18));
    __ Add(x18, x18, 3);
  }
  for (int i = 15; i >= 0; i--) {
    __ St3(v0.B(), v1.B(), v2.B(), i, MemOperand(x18, 3, PostIndex));
  }
  __ Ldr(q3, MemOperand(x17, 0 * 16));
  __ Ldr(q4, MemOperand(x17, 1 * 16));
  __ Ldr(q5, MemOperand(x17, 2 * 16));
  __ Ldr(q6, MemOperand(x17, 3 * 16));
  __ Ldr(q7, MemOperand(x17, 4 * 16));
  __ Ldr(q16, MemOperand(x17, 5 * 16));

  // Test H stores with and without post index.
  __ Mov(x0, 6);
  for (int i = 7; i >= 0; i--) {
    __ St3(v0.H(), v1.H(), v2.H(), i, MemOperand(x18));
    __ Add(x18, x18, 6);
  }
  for (int i = 7; i >= 0; i--) {
    __ St3(v0.H(), v1.H(), v2.H(), i, MemOperand(x18, x0, PostIndex));
  }
  __ Ldr(q17, MemOperand(x17, 6 * 16));
  __ Ldr(q18, MemOperand(x17, 7 * 16));
  __ Ldr(q19, MemOperand(x17, 8 * 16));
  __ Ldr(q20, MemOperand(x17, 9 * 16));
  __ Ldr(q21, MemOperand(x17, 10 * 16));
  __ Ldr(q22, MemOperand(x17, 11 * 16));

  // Test S stores with and without post index.
  for (int i = 3; i >= 0; i--) {
    __ St3(v0.S(), v1.S(), v2.S(), i, MemOperand(x18));
    __ Add(x18, x18, 12);
  }
  for (int i = 3; i >= 0; i--) {
    __ St3(v0.S(), v1.S(), v2.S(), i, MemOperand(x18, 12, PostIndex));
  }
  __ Ldr(q23, MemOperand(x17, 12 * 16));
  __ Ldr(q24, MemOperand(x17, 13 * 16));
  __ Ldr(q25, MemOperand(x17, 14 * 16));
  __ Ldr(q26, MemOperand(x17, 15 * 16));
  __ Ldr(q27, MemOperand(x17, 16 * 16));
  __ Ldr(q28, MemOperand(x17, 17 * 16));

  // Test D stores with and without post index.
  __ Mov(x0, 24);
  __ St3(v0.D(), v1.D(), v2.D(), 1, MemOperand(x18));
  __ Add(x18, x18, 24);
  __ St3(v0.D(), v1.D(), v2.D(), 0, MemOperand(x18, 24, PostIndex));
  __ St3(v0.D(), v1.D(), v2.D(), 1, MemOperand(x18, x0, PostIndex));
  __ Ldr(q29, MemOperand(x17, 18 * 16));
  __ Ldr(q30, MemOperand(x17, 19 * 16));
  __ Ldr(q31, MemOperand(x17, 20 * 16));
  END();

  RUN();

  ASSERT_EQUAL_128(0x0524140423130322, 0x1202211101201000, q3);
  ASSERT_EQUAL_128(0x1a0a291909281808, 0x2717072616062515, q4);
  ASSERT_EQUAL_128(0x2f1f0f2e1e0e2d1d, 0x0d2c1c0c2b1b0b2a, q5);
  ASSERT_EQUAL_128(0x0524140423130322, 0x1202211101201000, q6);
  ASSERT_EQUAL_128(0x1a0a291909281808, 0x2717072616062515, q7);
  ASSERT_EQUAL_128(0x2f1f0f2e1e0e2d1d, 0x0d2c1c0c2b1b0b2a, q16);

  ASSERT_EQUAL_128(0x1415040522231213, 0x0203202110110001, q17);
  ASSERT_EQUAL_128(0x0a0b282918190809, 0x2627161706072425, q18);
  ASSERT_EQUAL_128(0x2e2f1e1f0e0f2c2d, 0x1c1d0c0d2a2b1a1b, q19);
  ASSERT_EQUAL_128(0x1415040522231213, 0x0203202110110001, q20);
  ASSERT_EQUAL_128(0x0a0b282918190809, 0x2627161706072425, q21);
  ASSERT_EQUAL_128(0x2e2f1e1f0e0f2c2d, 0x1c1d0c0d2a2b1a1b, q22);

  ASSERT_EQUAL_128(0x0405060720212223, 0x1011121300010203, q23);
  ASSERT_EQUAL_128(0x18191a1b08090a0b, 0x2425262714151617, q24);
  ASSERT_EQUAL_128(0x2c2d2e2f1c1d1e1f, 0x0c0d0e0f28292a2b, q25);
  ASSERT_EQUAL_128(0x0405060720212223, 0x1011121300010203, q26);
  ASSERT_EQUAL_128(0x18191a1b08090a0b, 0x2425262714151617, q27);
  ASSERT_EQUAL_128(0x2c2d2e2f1c1d1e1f, 0x0c0d0e0f28292a2b, q28);

  TEARDOWN();
}


TEST(neon_st4_lane) {
  SETUP();

  // Struct size * element sizes * vector size.
  uint8_t dst[4 * 4 * 16];
  memset(dst, 0, sizeof(dst));
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, dst_base);
  __ Mov(x18, dst_base);
  __ Movi(v0.V2D(), 0x0001020304050607, 0x08090a0b0c0d0e0f);
  __ Movi(v1.V2D(), 0x1011121314151617, 0x18191a1b1c1d1e1f);
  __ Movi(v2.V2D(), 0x2021222324252627, 0x28292a2b2c2d2e2f);
  __ Movi(v3.V2D(), 0x2021222324252627, 0x28292a2b2c2d2e2f);

  // Test B stores without post index.
  for (int i = 15; i >= 0; i--) {
    __ St4(v0.B(), v1.B(), v2.B(), v3.B(), i, MemOperand(x18));
    __ Add(x18, x18, 4);
  }
  __ Ldr(q4, MemOperand(x17, 0 * 16));
  __ Ldr(q5, MemOperand(x17, 1 * 16));
  __ Ldr(q6, MemOperand(x17, 2 * 16));
  __ Ldr(q7, MemOperand(x17, 3 * 16));

  // Test H stores with post index.
  __ Mov(x0, 8);
  for (int i = 7; i >= 0; i--) {
    __ St4(v0.H(), v1.H(), v2.H(), v3.H(), i, MemOperand(x18, x0, PostIndex));
  }
  __ Ldr(q16, MemOperand(x17, 4 * 16));
  __ Ldr(q17, MemOperand(x17, 5 * 16));
  __ Ldr(q18, MemOperand(x17, 6 * 16));
  __ Ldr(q19, MemOperand(x17, 7 * 16));

  // Test S stores without post index.
  for (int i = 3; i >= 0; i--) {
    __ St4(v0.S(), v1.S(), v2.S(), v3.S(), i, MemOperand(x18));
    __ Add(x18, x18, 16);
  }
  __ Ldr(q20, MemOperand(x17, 8 * 16));
  __ Ldr(q21, MemOperand(x17, 9 * 16));
  __ Ldr(q22, MemOperand(x17, 10 * 16));
  __ Ldr(q23, MemOperand(x17, 11 * 16));

  // Test D stores with post index.
  __ Mov(x0, 32);
  __ St4(v0.D(), v1.D(), v2.D(), v3.D(), 0, MemOperand(x18, 32, PostIndex));
  __ St4(v0.D(), v1.D(), v2.D(), v3.D(), 1, MemOperand(x18, x0, PostIndex));

  __ Ldr(q24, MemOperand(x17, 12 * 16));
  __ Ldr(q25, MemOperand(x17, 13 * 16));
  __ Ldr(q26, MemOperand(x17, 14 * 16));
  __ Ldr(q27, MemOperand(x17, 15 * 16));
  END();

  RUN();

  ASSERT_EQUAL_128(0x2323130322221202, 0x2121110120201000, q4);
  ASSERT_EQUAL_128(0x2727170726261606, 0x2525150524241404, q5);
  ASSERT_EQUAL_128(0x2b2b1b0b2a2a1a0a, 0x2929190928281808, q6);
  ASSERT_EQUAL_128(0x2f2f1f0f2e2e1e0e, 0x2d2d1d0d2c2c1c0c, q7);

  ASSERT_EQUAL_128(0x2223222312130203, 0x2021202110110001, q16);
  ASSERT_EQUAL_128(0x2627262716170607, 0x2425242514150405, q17);
  ASSERT_EQUAL_128(0x2a2b2a2b1a1b0a0b, 0x2829282918190809, q18);
  ASSERT_EQUAL_128(0x2e2f2e2f1e1f0e0f, 0x2c2d2c2d1c1d0c0d, q19);

  ASSERT_EQUAL_128(0x2021222320212223, 0x1011121300010203, q20);
  ASSERT_EQUAL_128(0x2425262724252627, 0x1415161704050607, q21);
  ASSERT_EQUAL_128(0x28292a2b28292a2b, 0x18191a1b08090a0b, q22);
  ASSERT_EQUAL_128(0x2c2d2e2f2c2d2e2f, 0x1c1d1e1f0c0d0e0f, q23);

  ASSERT_EQUAL_128(0x18191a1b1c1d1e1f, 0x08090a0b0c0d0e0f, q24);
  ASSERT_EQUAL_128(0x28292a2b2c2d2e2f, 0x28292a2b2c2d2e2f, q25);
  ASSERT_EQUAL_128(0x1011121314151617, 0x0001020304050607, q26);
  ASSERT_EQUAL_128(0x2021222324252627, 0x2021222324252627, q27);

  TEARDOWN();
}


TEST(neon_ld1_lane_postindex) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Mov(x19, src_base);
  __ Mov(x20, src_base);
  __ Mov(x21, src_base);
  __ Mov(x22, src_base);
  __ Mov(x23, src_base);
  __ Mov(x24, src_base);

  // Test loading whole register by element.
  for (int i = 15; i >= 0; i--) {
    __ Ld1(v0.B(), i, MemOperand(x17, 1, PostIndex));
  }

  for (int i = 7; i >= 0; i--) {
    __ Ld1(v1.H(), i, MemOperand(x18, 2, PostIndex));
  }

  for (int i = 3; i >= 0; i--) {
    __ Ld1(v2.S(), i, MemOperand(x19, 4, PostIndex));
  }

  for (int i = 1; i >= 0; i--) {
    __ Ld1(v3.D(), i, MemOperand(x20, 8, PostIndex));
  }

  // Test loading a single element into an initialised register.
  __ Mov(x25, 1);
  __ Ldr(q4, MemOperand(x21));
  __ Ld1(v4.B(), 4, MemOperand(x21, x25, PostIndex));
  __ Add(x25, x25, 1);

  __ Ldr(q5, MemOperand(x22));
  __ Ld1(v5.H(), 3, MemOperand(x22, x25, PostIndex));
  __ Add(x25, x25, 1);

  __ Ldr(q6, MemOperand(x23));
  __ Ld1(v6.S(), 2, MemOperand(x23, x25, PostIndex));
  __ Add(x25, x25, 1);

  __ Ldr(q7, MemOperand(x24));
  __ Ld1(v7.D(), 1, MemOperand(x24, x25, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x0001020304050607, 0x08090a0b0c0d0e0f, q0);
  ASSERT_EQUAL_128(0x0100030205040706, 0x09080b0a0d0c0f0e, q1);
  ASSERT_EQUAL_128(0x0302010007060504, 0x0b0a09080f0e0d0c, q2);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0f0e0d0c0b0a0908, q3);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050003020100, q4);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0100050403020100, q5);
  ASSERT_EQUAL_128(0x0f0e0d0c03020100, 0x0706050403020100, q6);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0706050403020100, q7);
  ASSERT_EQUAL_64(src_base + 16, x17);
  ASSERT_EQUAL_64(src_base + 16, x18);
  ASSERT_EQUAL_64(src_base + 16, x19);
  ASSERT_EQUAL_64(src_base + 16, x20);
  ASSERT_EQUAL_64(src_base + 1, x21);
  ASSERT_EQUAL_64(src_base + 2, x22);
  ASSERT_EQUAL_64(src_base + 3, x23);
  ASSERT_EQUAL_64(src_base + 4, x24);

  TEARDOWN();
}


TEST(neon_st1_lane_postindex) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, -16);
  __ Ldr(q0, MemOperand(x17));

  for (int i = 15; i >= 0; i--) {
    __ St1(v0.B(), i, MemOperand(x17, 1, PostIndex));
  }
  __ Ldr(q1, MemOperand(x17, x18));

  for (int i = 7; i >= 0; i--) {
    __ St1(v0.H(), i, MemOperand(x17, 2, PostIndex));
  }
  __ Ldr(q2, MemOperand(x17, x18));

  for (int i = 3; i >= 0; i--) {
    __ St1(v0.S(), i, MemOperand(x17, 4, PostIndex));
  }
  __ Ldr(q3, MemOperand(x17, x18));

  for (int i = 1; i >= 0; i--) {
    __ St1(v0.D(), i, MemOperand(x17, 8, PostIndex));
  }
  __ Ldr(q4, MemOperand(x17, x18));

  END();

  RUN();

  ASSERT_EQUAL_128(0x0001020304050607, 0x08090a0b0c0d0e0f, q1);
  ASSERT_EQUAL_128(0x0100030205040706, 0x09080b0a0d0c0f0e, q2);
  ASSERT_EQUAL_128(0x0302010007060504, 0x0b0a09080f0e0d0c, q3);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0f0e0d0c0b0a0908, q4);

  TEARDOWN();
}


TEST(neon_ld1_alllanes) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base + 1);
  __ Ld1r(v0.V8B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1r(v1.V16B(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1r(v2.V4H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1r(v3.V8H(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1r(v4.V2S(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1r(v5.V4S(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1r(v6.V1D(), MemOperand(x17));
  __ Add(x17, x17, 1);
  __ Ld1r(v7.V2D(), MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x0101010101010101, q0);
  ASSERT_EQUAL_128(0x0202020202020202, 0x0202020202020202, q1);
  ASSERT_EQUAL_128(0, 0x0403040304030403, q2);
  ASSERT_EQUAL_128(0x0504050405040504, 0x0504050405040504, q3);
  ASSERT_EQUAL_128(0, 0x0807060508070605, q4);
  ASSERT_EQUAL_128(0x0908070609080706, 0x0908070609080706, q5);
  ASSERT_EQUAL_128(0, 0x0e0d0c0b0a090807, q6);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0f0e0d0c0b0a0908, q7);

  TEARDOWN();
}


TEST(neon_ld1_alllanes_postindex) {
  SETUP();

  uint8_t src[64];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base + 1);
  __ Mov(x18, 1);
  __ Ld1r(v0.V8B(), MemOperand(x17, 1, PostIndex));
  __ Ld1r(v1.V16B(), MemOperand(x17, x18, PostIndex));
  __ Ld1r(v2.V4H(), MemOperand(x17, x18, PostIndex));
  __ Ld1r(v3.V8H(), MemOperand(x17, 2, PostIndex));
  __ Ld1r(v4.V2S(), MemOperand(x17, x18, PostIndex));
  __ Ld1r(v5.V4S(), MemOperand(x17, 4, PostIndex));
  __ Ld1r(v6.V2D(), MemOperand(x17, 8, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x0101010101010101, q0);
  ASSERT_EQUAL_128(0x0202020202020202, 0x0202020202020202, q1);
  ASSERT_EQUAL_128(0, 0x0403040304030403, q2);
  ASSERT_EQUAL_128(0x0504050405040504, 0x0504050405040504, q3);
  ASSERT_EQUAL_128(0, 0x0908070609080706, q4);
  ASSERT_EQUAL_128(0x0a0908070a090807, 0x0a0908070a090807, q5);
  ASSERT_EQUAL_128(0x1211100f0e0d0c0b, 0x1211100f0e0d0c0b, q6);
  ASSERT_EQUAL_64(src_base + 19, x17);

  TEARDOWN();
}


TEST(neon_st1_d) {
  SETUP();

  uint8_t src[14 * kDRegSizeInBytes];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));
  __ Ldr(q2, MemOperand(x17, 16, PostIndex));
  __ Ldr(q3, MemOperand(x17, 16, PostIndex));
  __ Mov(x17, src_base);

  __ St1(v0.V8B(), MemOperand(x17));
  __ Ldr(d16, MemOperand(x17, 8, PostIndex));

  __ St1(v0.V8B(), v1.V8B(), MemOperand(x17));
  __ Ldr(q17, MemOperand(x17, 16, PostIndex));

  __ St1(v0.V4H(), v1.V4H(), v2.V4H(), MemOperand(x17));
  __ Ldr(d18, MemOperand(x17, 8, PostIndex));
  __ Ldr(d19, MemOperand(x17, 8, PostIndex));
  __ Ldr(d20, MemOperand(x17, 8, PostIndex));

  __ St1(v0.V2S(), v1.V2S(), v2.V2S(), v3.V2S(), MemOperand(x17));
  __ Ldr(q21, MemOperand(x17, 16, PostIndex));
  __ Ldr(q22, MemOperand(x17, 16, PostIndex));

  __ St1(v0.V1D(), v1.V1D(), v2.V1D(), v3.V1D(), MemOperand(x17));
  __ Ldr(q23, MemOperand(x17, 16, PostIndex));
  __ Ldr(q24, MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q0);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716151413121110, q1);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726252423222120, q2);
  ASSERT_EQUAL_128(0x3f3e3d3c3b3a3938, 0x3736353433323130, q3);
  ASSERT_EQUAL_128(0, 0x0706050403020100, q16);
  ASSERT_EQUAL_128(0x1716151413121110, 0x0706050403020100, q17);
  ASSERT_EQUAL_128(0, 0x0706050403020100, q18);
  ASSERT_EQUAL_128(0, 0x1716151413121110, q19);
  ASSERT_EQUAL_128(0, 0x2726252423222120, q20);
  ASSERT_EQUAL_128(0x1716151413121110, 0x0706050403020100, q21);
  ASSERT_EQUAL_128(0x3736353433323130, 0x2726252423222120, q22);
  ASSERT_EQUAL_128(0x1716151413121110, 0x0706050403020100, q23);
  ASSERT_EQUAL_128(0x3736353433323130, 0x2726252423222120, q24);

  TEARDOWN();
}


TEST(neon_st1_d_postindex) {
  SETUP();

  uint8_t src[64 + 14 * kDRegSizeInBytes];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, -8);
  __ Mov(x19, -16);
  __ Mov(x20, -24);
  __ Mov(x21, -32);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));
  __ Ldr(q2, MemOperand(x17, 16, PostIndex));
  __ Ldr(q3, MemOperand(x17, 16, PostIndex));
  __ Mov(x17, src_base);

  __ St1(v0.V8B(), MemOperand(x17, 8, PostIndex));
  __ Ldr(d16, MemOperand(x17, x18));

  __ St1(v0.V8B(), v1.V8B(), MemOperand(x17, 16, PostIndex));
  __ Ldr(q17, MemOperand(x17, x19));

  __ St1(v0.V4H(), v1.V4H(), v2.V4H(), MemOperand(x17, 24, PostIndex));
  __ Ldr(d18, MemOperand(x17, x20));
  __ Ldr(d19, MemOperand(x17, x19));
  __ Ldr(d20, MemOperand(x17, x18));

  __ St1(v0.V2S(), v1.V2S(), v2.V2S(), v3.V2S(),
         MemOperand(x17, 32, PostIndex));
  __ Ldr(q21, MemOperand(x17, x21));
  __ Ldr(q22, MemOperand(x17, x19));

  __ St1(v0.V1D(), v1.V1D(), v2.V1D(), v3.V1D(),
         MemOperand(x17, 32, PostIndex));
  __ Ldr(q23, MemOperand(x17, x21));
  __ Ldr(q24, MemOperand(x17, x19));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x0706050403020100, q16);
  ASSERT_EQUAL_128(0x1716151413121110, 0x0706050403020100, q17);
  ASSERT_EQUAL_128(0, 0x0706050403020100, q18);
  ASSERT_EQUAL_128(0, 0x1716151413121110, q19);
  ASSERT_EQUAL_128(0, 0x2726252423222120, q20);
  ASSERT_EQUAL_128(0x1716151413121110, 0x0706050403020100, q21);
  ASSERT_EQUAL_128(0x3736353433323130, 0x2726252423222120, q22);
  ASSERT_EQUAL_128(0x1716151413121110, 0x0706050403020100, q23);
  ASSERT_EQUAL_128(0x3736353433323130, 0x2726252423222120, q24);

  TEARDOWN();
}


TEST(neon_st1_q) {
  SETUP();

  uint8_t src[64 + 160];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));
  __ Ldr(q2, MemOperand(x17, 16, PostIndex));
  __ Ldr(q3, MemOperand(x17, 16, PostIndex));

  __ St1(v0.V16B(), MemOperand(x17));
  __ Ldr(q16, MemOperand(x17, 16, PostIndex));

  __ St1(v0.V8H(), v1.V8H(), MemOperand(x17));
  __ Ldr(q17, MemOperand(x17, 16, PostIndex));
  __ Ldr(q18, MemOperand(x17, 16, PostIndex));

  __ St1(v0.V4S(), v1.V4S(), v2.V4S(), MemOperand(x17));
  __ Ldr(q19, MemOperand(x17, 16, PostIndex));
  __ Ldr(q20, MemOperand(x17, 16, PostIndex));
  __ Ldr(q21, MemOperand(x17, 16, PostIndex));

  __ St1(v0.V2D(), v1.V2D(), v2.V2D(), v3.V2D(), MemOperand(x17));
  __ Ldr(q22, MemOperand(x17, 16, PostIndex));
  __ Ldr(q23, MemOperand(x17, 16, PostIndex));
  __ Ldr(q24, MemOperand(x17, 16, PostIndex));
  __ Ldr(q25, MemOperand(x17));
  END();

  RUN();

  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q16);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q17);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716151413121110, q18);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q19);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716151413121110, q20);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726252423222120, q21);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q22);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716151413121110, q23);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726252423222120, q24);
  ASSERT_EQUAL_128(0x3f3e3d3c3b3a3938, 0x3736353433323130, q25);

  TEARDOWN();
}


TEST(neon_st1_q_postindex) {
  SETUP();

  uint8_t src[64 + 160];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, -16);
  __ Mov(x19, -32);
  __ Mov(x20, -48);
  __ Mov(x21, -64);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));
  __ Ldr(q2, MemOperand(x17, 16, PostIndex));
  __ Ldr(q3, MemOperand(x17, 16, PostIndex));

  __ St1(v0.V16B(), MemOperand(x17, 16, PostIndex));
  __ Ldr(q16, MemOperand(x17, x18));

  __ St1(v0.V8H(), v1.V8H(), MemOperand(x17, 32, PostIndex));
  __ Ldr(q17, MemOperand(x17, x19));
  __ Ldr(q18, MemOperand(x17, x18));

  __ St1(v0.V4S(), v1.V4S(), v2.V4S(), MemOperand(x17, 48, PostIndex));
  __ Ldr(q19, MemOperand(x17, x20));
  __ Ldr(q20, MemOperand(x17, x19));
  __ Ldr(q21, MemOperand(x17, x18));

  __ St1(v0.V2D(), v1.V2D(), v2.V2D(), v3.V2D(),
         MemOperand(x17, 64, PostIndex));
  __ Ldr(q22, MemOperand(x17, x21));
  __ Ldr(q23, MemOperand(x17, x20));
  __ Ldr(q24, MemOperand(x17, x19));
  __ Ldr(q25, MemOperand(x17, x18));

  END();

  RUN();

  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q16);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q17);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716151413121110, q18);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q19);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716151413121110, q20);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726252423222120, q21);
  ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q22);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x1716151413121110, q23);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726252423222120, q24);
  ASSERT_EQUAL_128(0x3f3e3d3c3b3a3938, 0x3736353433323130, q25);

  TEARDOWN();
}


TEST(neon_st2_d) {
  SETUP();

  uint8_t src[4*16];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));

  __ St2(v0.V8B(), v1.V8B(), MemOperand(x18));
  __ Add(x18, x18, 22);
  __ St2(v0.V4H(), v1.V4H(), MemOperand(x18));
  __ Add(x18, x18, 11);
  __ St2(v0.V2S(), v1.V2S(), MemOperand(x18));

  __ Mov(x19, src_base);
  __ Ldr(q0, MemOperand(x19, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));
  __ Ldr(q2, MemOperand(x19, 16, PostIndex));
  __ Ldr(q3, MemOperand(x19, 16, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x1707160615051404, 0x1303120211011000, q0);
  ASSERT_EQUAL_128(0x0504131203021110, 0x0100151413121110, q1);
  ASSERT_EQUAL_128(0x1615140706050413, 0x1211100302010014, q2);
  ASSERT_EQUAL_128(0x3f3e3d3c3b3a3938, 0x3736353433323117, q3);

  TEARDOWN();
}


TEST(neon_st2_d_postindex) {
  SETUP();

  uint8_t src[4*16];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x22, 5);
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));

  __ St2(v0.V8B(), v1.V8B(), MemOperand(x18, x22, PostIndex));
  __ St2(v0.V4H(), v1.V4H(), MemOperand(x18, 16, PostIndex));
  __ St2(v0.V2S(), v1.V2S(), MemOperand(x18));


  __ Mov(x19, src_base);
  __ Ldr(q0, MemOperand(x19, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));
  __ Ldr(q2, MemOperand(x19, 16, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x1405041312030211, 0x1001000211011000, q0);
  ASSERT_EQUAL_128(0x0605041312111003, 0x0201001716070615, q1);
  ASSERT_EQUAL_128(0x2f2e2d2c2b2a2928, 0x2726251716151407, q2);

  TEARDOWN();
}


TEST(neon_st2_q) {
  SETUP();

  uint8_t src[5*16];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));

  __ St2(v0.V16B(), v1.V16B(), MemOperand(x18));
  __ Add(x18, x18, 8);
  __ St2(v0.V8H(), v1.V8H(), MemOperand(x18));
  __ Add(x18, x18, 22);
  __ St2(v0.V4S(), v1.V4S(), MemOperand(x18));
  __ Add(x18, x18, 2);
  __ St2(v0.V2D(), v1.V2D(), MemOperand(x18));

  __ Mov(x19, src_base);
  __ Ldr(q0, MemOperand(x19, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));
  __ Ldr(q2, MemOperand(x19, 16, PostIndex));
  __ Ldr(q3, MemOperand(x19, 16, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x1312030211100100, 0x1303120211011000, q0);
  ASSERT_EQUAL_128(0x01000b0a19180908, 0x1716070615140504, q1);
  ASSERT_EQUAL_128(0x1716151413121110, 0x0706050403020100, q2);
  ASSERT_EQUAL_128(0x1f1e1d1c1b1a1918, 0x0f0e0d0c0b0a0908, q3);
  TEARDOWN();
}


TEST(neon_st2_q_postindex) {
  SETUP();

  uint8_t src[5*16];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x22, 5);
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));

  __ St2(v0.V16B(), v1.V16B(), MemOperand(x18, x22, PostIndex));
  __ St2(v0.V8H(), v1.V8H(), MemOperand(x18, 32, PostIndex));
  __ St2(v0.V4S(), v1.V4S(), MemOperand(x18, x22, PostIndex));
  __ St2(v0.V2D(), v1.V2D(), MemOperand(x18));

  __ Mov(x19, src_base);
  __ Ldr(q0, MemOperand(x19, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));
  __ Ldr(q2, MemOperand(x19, 16, PostIndex));
  __ Ldr(q3, MemOperand(x19, 16, PostIndex));
  __ Ldr(q4, MemOperand(x19, 16, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x1405041312030211, 0x1001000211011000, q0);
  ASSERT_EQUAL_128(0x1c0d0c1b1a0b0a19, 0x1809081716070615, q1);
  ASSERT_EQUAL_128(0x0504030201001003, 0x0201001f1e0f0e1d, q2);
  ASSERT_EQUAL_128(0x0d0c0b0a09081716, 0x1514131211100706, q3);
  ASSERT_EQUAL_128(0x4f4e4d4c4b4a1f1e, 0x1d1c1b1a19180f0e, q4);

  TEARDOWN();
}


TEST(neon_st3_d) {
  SETUP();

  uint8_t src[3*16];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));
  __ Ldr(q2, MemOperand(x17, 16, PostIndex));

  __ St3(v0.V8B(), v1.V8B(), v2.V8B(), MemOperand(x18));
  __ Add(x18, x18, 3);
  __ St3(v0.V4H(), v1.V4H(), v2.V4H(), MemOperand(x18));
  __ Add(x18, x18, 2);
  __ St3(v0.V2S(), v1.V2S(), v2.V2S(), MemOperand(x18));


  __ Mov(x19, src_base);
  __ Ldr(q0, MemOperand(x19, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x2221201312111003, 0x0201000100201000, q0);
  ASSERT_EQUAL_128(0x1f1e1d2726252417, 0x1615140706050423, q1);

  TEARDOWN();
}


TEST(neon_st3_d_postindex) {
  SETUP();

  uint8_t src[4*16];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x22, 5);
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));
  __ Ldr(q2, MemOperand(x17, 16, PostIndex));

  __ St3(v0.V8B(), v1.V8B(), v2.V8B(), MemOperand(x18, x22, PostIndex));
  __ St3(v0.V4H(), v1.V4H(), v2.V4H(), MemOperand(x18, 24, PostIndex));
  __ St3(v0.V2S(), v1.V2S(), v2.V2S(), MemOperand(x18));


  __ Mov(x19, src_base);
  __ Ldr(q0, MemOperand(x19, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));
  __ Ldr(q2, MemOperand(x19, 16, PostIndex));
  __ Ldr(q3, MemOperand(x19, 16, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x2213120302212011, 0x1001001101201000, q0);
  ASSERT_EQUAL_128(0x0201002726171607, 0x0625241514050423, q1);
  ASSERT_EQUAL_128(0x1615140706050423, 0x2221201312111003, q2);
  ASSERT_EQUAL_128(0x3f3e3d3c3b3a3938, 0x3736352726252417, q3);

  TEARDOWN();
}


TEST(neon_st3_q) {
  SETUP();

  uint8_t src[6*16];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));
  __ Ldr(q2, MemOperand(x17, 16, PostIndex));

  __ St3(v0.V16B(), v1.V16B(), v2.V16B(), MemOperand(x18));
  __ Add(x18, x18, 5);
  __ St3(v0.V8H(), v1.V8H(), v2.V8H(), MemOperand(x18));
  __ Add(x18, x18, 12);
  __ St3(v0.V4S(), v1.V4S(), v2.V4S(), MemOperand(x18));
  __ Add(x18, x18, 22);
  __ St3(v0.V2D(), v1.V2D(), v2.V2D(), MemOperand(x18));

  __ Mov(x19, src_base);
  __ Ldr(q0, MemOperand(x19, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));
  __ Ldr(q2, MemOperand(x19, 16, PostIndex));
  __ Ldr(q3, MemOperand(x19, 16, PostIndex));
  __ Ldr(q4, MemOperand(x19, 16, PostIndex));
  __ Ldr(q5, MemOperand(x19, 16, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x2213120302212011, 0x1001001101201000, q0);
  ASSERT_EQUAL_128(0x0605042322212013, 0x1211100302010023, q1);
  ASSERT_EQUAL_128(0x1007060504030201, 0x0025241716151407, q2);
  ASSERT_EQUAL_128(0x0827262524232221, 0x2017161514131211, q3);
  ASSERT_EQUAL_128(0x281f1e1d1c1b1a19, 0x180f0e0d0c0b0a09, q4);
  ASSERT_EQUAL_128(0x5f5e5d5c5b5a5958, 0x572f2e2d2c2b2a29, q5);

  TEARDOWN();
}


TEST(neon_st3_q_postindex) {
  SETUP();

  uint8_t src[7*16];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x22, 5);
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));
  __ Ldr(q2, MemOperand(x17, 16, PostIndex));

  __ St3(v0.V16B(), v1.V16B(), v2.V16B(), MemOperand(x18, x22, PostIndex));
  __ St3(v0.V8H(), v1.V8H(), v2.V8H(), MemOperand(x18, 48, PostIndex));
  __ St3(v0.V4S(), v1.V4S(), v2.V4S(), MemOperand(x18, x22, PostIndex));
  __ St3(v0.V2D(), v1.V2D(), v2.V2D(), MemOperand(x18));

  __ Mov(x19, src_base);
  __ Ldr(q0, MemOperand(x19, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));
  __ Ldr(q2, MemOperand(x19, 16, PostIndex));
  __ Ldr(q3, MemOperand(x19, 16, PostIndex));
  __ Ldr(q4, MemOperand(x19, 16, PostIndex));
  __ Ldr(q5, MemOperand(x19, 16, PostIndex));
  __ Ldr(q6, MemOperand(x19, 16, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x2213120302212011, 0x1001001101201000, q0);
  ASSERT_EQUAL_128(0x1809082726171607, 0x0625241514050423, q1);
  ASSERT_EQUAL_128(0x0e2d2c1d1c0d0c2b, 0x2a1b1a0b0a292819, q2);
  ASSERT_EQUAL_128(0x0504030201001003, 0x0201002f2e1f1e0f, q3);
  ASSERT_EQUAL_128(0x2524232221201716, 0x1514131211100706, q4);
  ASSERT_EQUAL_128(0x1d1c1b1a19180f0e, 0x0d0c0b0a09082726, q5);
  ASSERT_EQUAL_128(0x6f6e6d6c6b6a2f2e, 0x2d2c2b2a29281f1e, q6);

  TEARDOWN();
}


TEST(neon_st4_d) {
  SETUP();

  uint8_t src[4*16];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));
  __ Ldr(q2, MemOperand(x17, 16, PostIndex));
  __ Ldr(q3, MemOperand(x17, 16, PostIndex));

  __ St4(v0.V8B(), v1.V8B(), v2.V8B(), v3.V8B(), MemOperand(x18));
  __ Add(x18, x18, 12);
  __ St4(v0.V4H(), v1.V4H(), v2.V4H(), v3.V4H(), MemOperand(x18));
  __ Add(x18, x18, 15);
  __ St4(v0.V2S(), v1.V2S(), v2.V2S(), v3.V2S(), MemOperand(x18));


  __ Mov(x19, src_base);
  __ Ldr(q0, MemOperand(x19, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));
  __ Ldr(q2, MemOperand(x19, 16, PostIndex));
  __ Ldr(q3, MemOperand(x19, 16, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x1110010032221202, 0X3121110130201000, q0);
  ASSERT_EQUAL_128(0x1003020100322322, 0X1312030231302120, q1);
  ASSERT_EQUAL_128(0x1407060504333231, 0X3023222120131211, q2);
  ASSERT_EQUAL_128(0x3f3e3d3c3b373635, 0x3427262524171615, q3);

  TEARDOWN();
}


TEST(neon_st4_d_postindex) {
  SETUP();

  uint8_t src[5*16];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x22, 5);
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));
  __ Ldr(q2, MemOperand(x17, 16, PostIndex));
  __ Ldr(q3, MemOperand(x17, 16, PostIndex));

  __ St4(v0.V8B(), v1.V8B(), v2.V8B(), v3.V8B(),
         MemOperand(x18, x22, PostIndex));
  __ St4(v0.V4H(), v1.V4H(), v2.V4H(), v3.V4H(),
         MemOperand(x18, 32, PostIndex));
  __ St4(v0.V2S(), v1.V2S(), v2.V2S(), v3.V2S(),
         MemOperand(x18));


  __ Mov(x19, src_base);
  __ Ldr(q0, MemOperand(x19, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));
  __ Ldr(q2, MemOperand(x19, 16, PostIndex));
  __ Ldr(q3, MemOperand(x19, 16, PostIndex));
  __ Ldr(q4, MemOperand(x19, 16, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x1203023130212011, 0x1001000130201000, q0);
  ASSERT_EQUAL_128(0x1607063534252415, 0x1405043332232213, q1);
  ASSERT_EQUAL_128(0x2221201312111003, 0x0201003736272617, q2);
  ASSERT_EQUAL_128(0x2625241716151407, 0x0605043332313023, q3);
  ASSERT_EQUAL_128(0x4f4e4d4c4b4a4948, 0x4746453736353427, q4);

  TEARDOWN();
}


TEST(neon_st4_q) {
  SETUP();

  uint8_t src[7*16];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));
  __ Ldr(q2, MemOperand(x17, 16, PostIndex));
  __ Ldr(q3, MemOperand(x17, 16, PostIndex));

  __ St4(v0.V16B(), v1.V16B(), v2.V16B(), v3.V16B(), MemOperand(x18));
  __ Add(x18, x18, 5);
  __ St4(v0.V8H(), v1.V8H(), v2.V8H(), v3.V8H(), MemOperand(x18));
  __ Add(x18, x18, 12);
  __ St4(v0.V4S(), v1.V4S(), v2.V4S(), v3.V4S(), MemOperand(x18));
  __ Add(x18, x18, 22);
  __ St4(v0.V2D(), v1.V2D(), v2.V2D(), v3.V2D(), MemOperand(x18));
  __ Add(x18, x18, 10);

  __ Mov(x19, src_base);
  __ Ldr(q0, MemOperand(x19, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));
  __ Ldr(q2, MemOperand(x19, 16, PostIndex));
  __ Ldr(q3, MemOperand(x19, 16, PostIndex));
  __ Ldr(q4, MemOperand(x19, 16, PostIndex));
  __ Ldr(q5, MemOperand(x19, 16, PostIndex));
  __ Ldr(q6, MemOperand(x19, 16, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x1203023130212011, 0x1001000130201000, q0);
  ASSERT_EQUAL_128(0x3231302322212013, 0x1211100302010013, q1);
  ASSERT_EQUAL_128(0x1007060504030201, 0x0015140706050433, q2);
  ASSERT_EQUAL_128(0x3027262524232221, 0x2017161514131211, q3);
  ASSERT_EQUAL_128(0x180f0e0d0c0b0a09, 0x0837363534333231, q4);
  ASSERT_EQUAL_128(0x382f2e2d2c2b2a29, 0x281f1e1d1c1b1a19, q5);
  ASSERT_EQUAL_128(0x6f6e6d6c6b6a6968, 0x673f3e3d3c3b3a39, q6);

  TEARDOWN();
}


TEST(neon_st4_q_postindex) {
  SETUP();

  uint8_t src[9*16];
  for (unsigned i = 0; i < sizeof(src); i++) {
    src[i] = i;
  }
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x22, 5);
  __ Mov(x17, src_base);
  __ Mov(x18, src_base);
  __ Ldr(q0, MemOperand(x17, 16, PostIndex));
  __ Ldr(q1, MemOperand(x17, 16, PostIndex));
  __ Ldr(q2, MemOperand(x17, 16, PostIndex));
  __ Ldr(q3, MemOperand(x17, 16, PostIndex));

  __ St4(v0.V16B(), v1.V16B(), v2.V16B(), v3.V16B(),
         MemOperand(x18, x22, PostIndex));
  __ St4(v0.V8H(), v1.V8H(), v2.V8H(), v3.V8H(),
         MemOperand(x18, 64, PostIndex));
  __ St4(v0.V4S(), v1.V4S(), v2.V4S(), v3.V4S(),
         MemOperand(x18, x22, PostIndex));
  __ St4(v0.V2D(), v1.V2D(), v2.V2D(), v3.V2D(),
         MemOperand(x18));

  __ Mov(x19, src_base);
  __ Ldr(q0, MemOperand(x19, 16, PostIndex));
  __ Ldr(q1, MemOperand(x19, 16, PostIndex));
  __ Ldr(q2, MemOperand(x19, 16, PostIndex));
  __ Ldr(q3, MemOperand(x19, 16, PostIndex));
  __ Ldr(q4, MemOperand(x19, 16, PostIndex));
  __ Ldr(q5, MemOperand(x19, 16, PostIndex));
  __ Ldr(q6, MemOperand(x19, 16, PostIndex));
  __ Ldr(q7, MemOperand(x19, 16, PostIndex));
  __ Ldr(q8, MemOperand(x19, 16, PostIndex));

  END();

  RUN();

  ASSERT_EQUAL_128(0x1203023130212011, 0x1001000130201000, q0);
  ASSERT_EQUAL_128(0x1607063534252415, 0x1405043332232213, q1);
  ASSERT_EQUAL_128(0x1a0b0a3938292819, 0x1809083736272617, q2);
  ASSERT_EQUAL_128(0x1e0f0e3d3c2d2c1d, 0x1c0d0c3b3a2b2a1b, q3);
  ASSERT_EQUAL_128(0x0504030201001003, 0x0201003f3e2f2e1f, q4);
  ASSERT_EQUAL_128(0x2524232221201716, 0x1514131211100706, q5);
  ASSERT_EQUAL_128(0x0d0c0b0a09083736, 0x3534333231302726, q6);
  ASSERT_EQUAL_128(0x2d2c2b2a29281f1e, 0x1d1c1b1a19180f0e, q7);
  ASSERT_EQUAL_128(0x8f8e8d8c8b8a3f3e, 0x3d3c3b3a39382f2e, q8);

  TEARDOWN();
}


TEST(ldp_stp_float) {
  SETUP();

  float src[2] = {1.0, 2.0};
  float dst[3] = {0.0, 0.0, 0.0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x16, src_base);
  __ Mov(x17, dst_base);
  __ Ldp(s31, s0, MemOperand(x16, 2 * sizeof(src[0]), PostIndex));
  __ Stp(s0, s31, MemOperand(x17, sizeof(dst[1]), PreIndex));
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s31);
  ASSERT_EQUAL_FP32(2.0, s0);
  ASSERT_EQUAL_FP32(0.0, dst[0]);
  ASSERT_EQUAL_FP32(2.0, dst[1]);
  ASSERT_EQUAL_FP32(1.0, dst[2]);
  ASSERT_EQUAL_64(src_base + 2 * sizeof(src[0]), x16);
  ASSERT_EQUAL_64(dst_base + sizeof(dst[1]), x17);

  TEARDOWN();
}


TEST(ldp_stp_double) {
  SETUP();

  double src[2] = {1.0, 2.0};
  double dst[3] = {0.0, 0.0, 0.0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x16, src_base);
  __ Mov(x17, dst_base);
  __ Ldp(d31, d0, MemOperand(x16, 2 * sizeof(src[0]), PostIndex));
  __ Stp(d0, d31, MemOperand(x17, sizeof(dst[1]), PreIndex));
  END();

  RUN();

  ASSERT_EQUAL_FP64(1.0, d31);
  ASSERT_EQUAL_FP64(2.0, d0);
  ASSERT_EQUAL_FP64(0.0, dst[0]);
  ASSERT_EQUAL_FP64(2.0, dst[1]);
  ASSERT_EQUAL_FP64(1.0, dst[2]);
  ASSERT_EQUAL_64(src_base + 2 * sizeof(src[0]), x16);
  ASSERT_EQUAL_64(dst_base + sizeof(dst[1]), x17);

  TEARDOWN();
}


TEST(ldp_stp_quad) {
  SETUP();

  uint64_t src[4] = {0x0123456789abcdef, 0xaaaaaaaa55555555,
                     0xfedcba9876543210, 0x55555555aaaaaaaa};
  uint64_t dst[6] = {0, 0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x16, src_base);
  __ Mov(x17, dst_base);
  __ Ldp(q31, q0, MemOperand(x16, 4 * sizeof(src[0]), PostIndex));
  __ Stp(q0, q31, MemOperand(x17, 2 * sizeof(dst[1]), PreIndex));
  END();

  RUN();

  ASSERT_EQUAL_128(0xaaaaaaaa55555555, 0x0123456789abcdef, q31);
  ASSERT_EQUAL_128(0x55555555aaaaaaaa, 0xfedcba9876543210, q0);
  ASSERT_EQUAL_64(0, dst[0]);
  ASSERT_EQUAL_64(0, dst[1]);
  ASSERT_EQUAL_64(0xfedcba9876543210, dst[2]);
  ASSERT_EQUAL_64(0x55555555aaaaaaaa, dst[3]);
  ASSERT_EQUAL_64(0x0123456789abcdef, dst[4]);
  ASSERT_EQUAL_64(0xaaaaaaaa55555555, dst[5]);
  ASSERT_EQUAL_64(src_base + 4 * sizeof(src[0]), x16);
  ASSERT_EQUAL_64(dst_base + 2 * sizeof(dst[1]), x17);

  TEARDOWN();
}


TEST(ldp_stp_offset) {
  SETUP();

  uint64_t src[3] = {0x0011223344556677, 0x8899aabbccddeeff,
                     0xffeeddccbbaa9988};
  uint64_t dst[7] = {0, 0, 0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x16, src_base);
  __ Mov(x17, dst_base);
  __ Mov(x18, src_base + 24);
  __ Mov(x19, dst_base + 56);
  __ Ldp(w0, w1, MemOperand(x16));
  __ Ldp(w2, w3, MemOperand(x16, 4));
  __ Ldp(x4, x5, MemOperand(x16, 8));
  __ Ldp(w6, w7, MemOperand(x18, -12));
  __ Ldp(x8, x9, MemOperand(x18, -16));
  __ Stp(w0, w1, MemOperand(x17));
  __ Stp(w2, w3, MemOperand(x17, 8));
  __ Stp(x4, x5, MemOperand(x17, 16));
  __ Stp(w6, w7, MemOperand(x19, -24));
  __ Stp(x8, x9, MemOperand(x19, -16));
  END();

  RUN();

  ASSERT_EQUAL_64(0x44556677, x0);
  ASSERT_EQUAL_64(0x00112233, x1);
  ASSERT_EQUAL_64(0x0011223344556677, dst[0]);
  ASSERT_EQUAL_64(0x00112233, x2);
  ASSERT_EQUAL_64(0xccddeeff, x3);
  ASSERT_EQUAL_64(0xccddeeff00112233, dst[1]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x4);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, dst[2]);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, x5);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, dst[3]);
  ASSERT_EQUAL_64(0x8899aabb, x6);
  ASSERT_EQUAL_64(0xbbaa9988, x7);
  ASSERT_EQUAL_64(0xbbaa99888899aabb, dst[4]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x8);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, dst[5]);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, x9);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, dst[6]);
  ASSERT_EQUAL_64(src_base, x16);
  ASSERT_EQUAL_64(dst_base, x17);
  ASSERT_EQUAL_64(src_base + 24, x18);
  ASSERT_EQUAL_64(dst_base + 56, x19);

  TEARDOWN();
}


TEST(ldp_stp_offset_wide) {
  SETUP();

  uint64_t src[3] = {0x0011223344556677, 0x8899aabbccddeeff,
                     0xffeeddccbbaa9988};
  uint64_t dst[7] = {0, 0, 0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);
  // Move base too far from the array to force multiple instructions
  // to be emitted.
  const int64_t base_offset = 1024;

  START();
  __ Mov(x20, src_base - base_offset);
  __ Mov(x21, dst_base - base_offset);
  __ Mov(x18, src_base + base_offset + 24);
  __ Mov(x19, dst_base + base_offset + 56);
  __ Ldp(w0, w1, MemOperand(x20, base_offset));
  __ Ldp(w2, w3, MemOperand(x20, base_offset + 4));
  __ Ldp(x4, x5, MemOperand(x20, base_offset + 8));
  __ Ldp(w6, w7, MemOperand(x18, -12 - base_offset));
  __ Ldp(x8, x9, MemOperand(x18, -16 - base_offset));
  __ Stp(w0, w1, MemOperand(x21, base_offset));
  __ Stp(w2, w3, MemOperand(x21, base_offset + 8));
  __ Stp(x4, x5, MemOperand(x21, base_offset + 16));
  __ Stp(w6, w7, MemOperand(x19, -24 - base_offset));
  __ Stp(x8, x9, MemOperand(x19, -16 - base_offset));
  END();

  RUN();

  ASSERT_EQUAL_64(0x44556677, x0);
  ASSERT_EQUAL_64(0x00112233, x1);
  ASSERT_EQUAL_64(0x0011223344556677, dst[0]);
  ASSERT_EQUAL_64(0x00112233, x2);
  ASSERT_EQUAL_64(0xccddeeff, x3);
  ASSERT_EQUAL_64(0xccddeeff00112233, dst[1]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x4);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, dst[2]);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, x5);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, dst[3]);
  ASSERT_EQUAL_64(0x8899aabb, x6);
  ASSERT_EQUAL_64(0xbbaa9988, x7);
  ASSERT_EQUAL_64(0xbbaa99888899aabb, dst[4]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x8);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, dst[5]);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, x9);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, dst[6]);
  ASSERT_EQUAL_64(src_base - base_offset, x20);
  ASSERT_EQUAL_64(dst_base - base_offset, x21);
  ASSERT_EQUAL_64(src_base + base_offset + 24, x18);
  ASSERT_EQUAL_64(dst_base + base_offset + 56, x19);

  TEARDOWN();
}


TEST(ldnp_stnp_offset) {
  SETUP();

  uint64_t src[4] = {0x0011223344556677, 0x8899aabbccddeeff,
                     0xffeeddccbbaa9988, 0x7766554433221100};
  uint64_t dst[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x16, src_base);
  __ Mov(x17, dst_base);
  __ Mov(x18, src_base + 24);
  __ Mov(x19, dst_base + 64);
  __ Mov(x20, src_base + 32);

  // Ensure address set up has happened before executing non-temporal ops.
  __ Dmb(InnerShareable, BarrierAll);

  __ Ldnp(w0, w1, MemOperand(x16));
  __ Ldnp(w2, w3, MemOperand(x16, 4));
  __ Ldnp(x4, x5, MemOperand(x16, 8));
  __ Ldnp(w6, w7, MemOperand(x18, -12));
  __ Ldnp(x8, x9, MemOperand(x18, -16));
  __ Ldnp(q16, q17, MemOperand(x16));
  __ Ldnp(q19, q18, MemOperand(x20, -32));
  __ Stnp(w0, w1, MemOperand(x17));
  __ Stnp(w2, w3, MemOperand(x17, 8));
  __ Stnp(x4, x5, MemOperand(x17, 16));
  __ Stnp(w6, w7, MemOperand(x19, -32));
  __ Stnp(x8, x9, MemOperand(x19, -24));
  __ Stnp(q17, q16, MemOperand(x19));
  __ Stnp(q18, q19, MemOperand(x19, 32));
  END();

  RUN();

  ASSERT_EQUAL_64(0x44556677, x0);
  ASSERT_EQUAL_64(0x00112233, x1);
  ASSERT_EQUAL_64(0x0011223344556677, dst[0]);
  ASSERT_EQUAL_64(0x00112233, x2);
  ASSERT_EQUAL_64(0xccddeeff, x3);
  ASSERT_EQUAL_64(0xccddeeff00112233, dst[1]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x4);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, dst[2]);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, x5);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, dst[3]);
  ASSERT_EQUAL_64(0x8899aabb, x6);
  ASSERT_EQUAL_64(0xbbaa9988, x7);
  ASSERT_EQUAL_64(0xbbaa99888899aabb, dst[4]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x8);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, dst[5]);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, x9);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, dst[6]);
  ASSERT_EQUAL_128(0x8899aabbccddeeff, 0x0011223344556677, q16);
  ASSERT_EQUAL_128(0x7766554433221100, 0xffeeddccbbaa9988, q17);
  ASSERT_EQUAL_128(0x7766554433221100, 0xffeeddccbbaa9988, q18);
  ASSERT_EQUAL_128(0x8899aabbccddeeff, 0x0011223344556677, q19);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, dst[8]);
  ASSERT_EQUAL_64(0x7766554433221100, dst[9]);
  ASSERT_EQUAL_64(0x0011223344556677, dst[10]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, dst[11]);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, dst[12]);
  ASSERT_EQUAL_64(0x7766554433221100, dst[13]);
  ASSERT_EQUAL_64(0x0011223344556677, dst[14]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, dst[15]);
  ASSERT_EQUAL_64(src_base, x16);
  ASSERT_EQUAL_64(dst_base, x17);
  ASSERT_EQUAL_64(src_base + 24, x18);
  ASSERT_EQUAL_64(dst_base + 64, x19);
  ASSERT_EQUAL_64(src_base + 32, x20);

  TEARDOWN();
}


TEST(ldnp_stnp_offset_float) {
  SETUP();

  float src[3] = {1.2, 2.3, 3.4};
  float dst[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x16, src_base);
  __ Mov(x17, dst_base);
  __ Mov(x18, src_base + 12);
  __ Mov(x19, dst_base + 24);

  // Ensure address set up has happened before executing non-temporal ops.
  __ Dmb(InnerShareable, BarrierAll);

  __ Ldnp(s0, s1, MemOperand(x16));
  __ Ldnp(s2, s3, MemOperand(x16, 4));
  __ Ldnp(s5, s4, MemOperand(x18, -8));
  __ Stnp(s1, s0, MemOperand(x17));
  __ Stnp(s3, s2, MemOperand(x17, 8));
  __ Stnp(s4, s5, MemOperand(x19, -8));
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.2, s0);
  ASSERT_EQUAL_FP32(2.3, s1);
  ASSERT_EQUAL_FP32(2.3, dst[0]);
  ASSERT_EQUAL_FP32(1.2, dst[1]);
  ASSERT_EQUAL_FP32(2.3, s2);
  ASSERT_EQUAL_FP32(3.4, s3);
  ASSERT_EQUAL_FP32(3.4, dst[2]);
  ASSERT_EQUAL_FP32(2.3, dst[3]);
  ASSERT_EQUAL_FP32(3.4, s4);
  ASSERT_EQUAL_FP32(2.3, s5);
  ASSERT_EQUAL_FP32(3.4, dst[4]);
  ASSERT_EQUAL_FP32(2.3, dst[5]);
  ASSERT_EQUAL_64(src_base, x16);
  ASSERT_EQUAL_64(dst_base, x17);
  ASSERT_EQUAL_64(src_base + 12, x18);
  ASSERT_EQUAL_64(dst_base + 24, x19);

  TEARDOWN();
}


TEST(ldnp_stnp_offset_double) {
  SETUP();

  double src[3] = {1.2, 2.3, 3.4};
  double dst[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x16, src_base);
  __ Mov(x17, dst_base);
  __ Mov(x18, src_base + 24);
  __ Mov(x19, dst_base + 48);

  // Ensure address set up has happened before executing non-temporal ops.
  __ Dmb(InnerShareable, BarrierAll);

  __ Ldnp(d0, d1, MemOperand(x16));
  __ Ldnp(d2, d3, MemOperand(x16, 8));
  __ Ldnp(d5, d4, MemOperand(x18, -16));
  __ Stnp(d1, d0, MemOperand(x17));
  __ Stnp(d3, d2, MemOperand(x17, 16));
  __ Stnp(d4, d5, MemOperand(x19, -16));
  END();

  RUN();

  ASSERT_EQUAL_FP64(1.2, d0);
  ASSERT_EQUAL_FP64(2.3, d1);
  ASSERT_EQUAL_FP64(2.3, dst[0]);
  ASSERT_EQUAL_FP64(1.2, dst[1]);
  ASSERT_EQUAL_FP64(2.3, d2);
  ASSERT_EQUAL_FP64(3.4, d3);
  ASSERT_EQUAL_FP64(3.4, dst[2]);
  ASSERT_EQUAL_FP64(2.3, dst[3]);
  ASSERT_EQUAL_FP64(3.4, d4);
  ASSERT_EQUAL_FP64(2.3, d5);
  ASSERT_EQUAL_FP64(3.4, dst[4]);
  ASSERT_EQUAL_FP64(2.3, dst[5]);
  ASSERT_EQUAL_64(src_base, x16);
  ASSERT_EQUAL_64(dst_base, x17);
  ASSERT_EQUAL_64(src_base + 24, x18);
  ASSERT_EQUAL_64(dst_base + 48, x19);

  TEARDOWN();
}


TEST(ldp_stp_preindex) {
  SETUP();

  uint64_t src[3] = {0x0011223344556677, 0x8899aabbccddeeff,
                     0xffeeddccbbaa9988};
  uint64_t dst[5] = {0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x16, src_base);
  __ Mov(x17, dst_base);
  __ Mov(x18, dst_base + 16);
  __ Ldp(w0, w1, MemOperand(x16, 4, PreIndex));
  __ Mov(x19, x16);
  __ Ldp(w2, w3, MemOperand(x16, -4, PreIndex));
  __ Stp(w2, w3, MemOperand(x17, 4, PreIndex));
  __ Mov(x20, x17);
  __ Stp(w0, w1, MemOperand(x17, -4, PreIndex));
  __ Ldp(x4, x5, MemOperand(x16, 8, PreIndex));
  __ Mov(x21, x16);
  __ Ldp(x6, x7, MemOperand(x16, -8, PreIndex));
  __ Stp(x7, x6, MemOperand(x18, 8, PreIndex));
  __ Mov(x22, x18);
  __ Stp(x5, x4, MemOperand(x18, -8, PreIndex));
  END();

  RUN();

  ASSERT_EQUAL_64(0x00112233, x0);
  ASSERT_EQUAL_64(0xccddeeff, x1);
  ASSERT_EQUAL_64(0x44556677, x2);
  ASSERT_EQUAL_64(0x00112233, x3);
  ASSERT_EQUAL_64(0xccddeeff00112233, dst[0]);
  ASSERT_EQUAL_64(0x0000000000112233, dst[1]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x4);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, x5);
  ASSERT_EQUAL_64(0x0011223344556677, x6);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x7);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, dst[2]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, dst[3]);
  ASSERT_EQUAL_64(0x0011223344556677, dst[4]);
  ASSERT_EQUAL_64(src_base, x16);
  ASSERT_EQUAL_64(dst_base, x17);
  ASSERT_EQUAL_64(dst_base + 16, x18);
  ASSERT_EQUAL_64(src_base + 4, x19);
  ASSERT_EQUAL_64(dst_base + 4, x20);
  ASSERT_EQUAL_64(src_base + 8, x21);
  ASSERT_EQUAL_64(dst_base + 24, x22);

  TEARDOWN();
}


TEST(ldp_stp_preindex_wide) {
  SETUP();

  uint64_t src[3] = {0x0011223344556677, 0x8899aabbccddeeff,
                     0xffeeddccbbaa9988};
  uint64_t dst[5] = {0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);
  // Move base too far from the array to force multiple instructions
  // to be emitted.
  const int64_t base_offset = 1024;

  START();
  __ Mov(x24, src_base - base_offset);
  __ Mov(x25, dst_base + base_offset);
  __ Mov(x18, dst_base + base_offset + 16);
  __ Ldp(w0, w1, MemOperand(x24, base_offset + 4, PreIndex));
  __ Mov(x19, x24);
  __ Mov(x24, src_base - base_offset + 4);
  __ Ldp(w2, w3, MemOperand(x24, base_offset - 4, PreIndex));
  __ Stp(w2, w3, MemOperand(x25, 4 - base_offset , PreIndex));
  __ Mov(x20, x25);
  __ Mov(x25, dst_base + base_offset + 4);
  __ Mov(x24, src_base - base_offset);
  __ Stp(w0, w1, MemOperand(x25, -4 - base_offset, PreIndex));
  __ Ldp(x4, x5, MemOperand(x24, base_offset + 8, PreIndex));
  __ Mov(x21, x24);
  __ Mov(x24, src_base - base_offset + 8);
  __ Ldp(x6, x7, MemOperand(x24, base_offset - 8, PreIndex));
  __ Stp(x7, x6, MemOperand(x18, 8 - base_offset, PreIndex));
  __ Mov(x22, x18);
  __ Mov(x18, dst_base + base_offset + 16 + 8);
  __ Stp(x5, x4, MemOperand(x18, -8 - base_offset, PreIndex));
  END();

  RUN();

  ASSERT_EQUAL_64(0x00112233, x0);
  ASSERT_EQUAL_64(0xccddeeff, x1);
  ASSERT_EQUAL_64(0x44556677, x2);
  ASSERT_EQUAL_64(0x00112233, x3);
  ASSERT_EQUAL_64(0xccddeeff00112233, dst[0]);
  ASSERT_EQUAL_64(0x0000000000112233, dst[1]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x4);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, x5);
  ASSERT_EQUAL_64(0x0011223344556677, x6);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x7);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, dst[2]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, dst[3]);
  ASSERT_EQUAL_64(0x0011223344556677, dst[4]);
  ASSERT_EQUAL_64(src_base, x24);
  ASSERT_EQUAL_64(dst_base, x25);
  ASSERT_EQUAL_64(dst_base + 16, x18);
  ASSERT_EQUAL_64(src_base + 4, x19);
  ASSERT_EQUAL_64(dst_base + 4, x20);
  ASSERT_EQUAL_64(src_base + 8, x21);
  ASSERT_EQUAL_64(dst_base + 24, x22);

  TEARDOWN();
}


TEST(ldp_stp_postindex) {
  SETUP();

  uint64_t src[4] = {0x0011223344556677, 0x8899aabbccddeeff,
                     0xffeeddccbbaa9988, 0x7766554433221100};
  uint64_t dst[5] = {0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x16, src_base);
  __ Mov(x17, dst_base);
  __ Mov(x18, dst_base + 16);
  __ Ldp(w0, w1, MemOperand(x16, 4, PostIndex));
  __ Mov(x19, x16);
  __ Ldp(w2, w3, MemOperand(x16, -4, PostIndex));
  __ Stp(w2, w3, MemOperand(x17, 4, PostIndex));
  __ Mov(x20, x17);
  __ Stp(w0, w1, MemOperand(x17, -4, PostIndex));
  __ Ldp(x4, x5, MemOperand(x16, 8, PostIndex));
  __ Mov(x21, x16);
  __ Ldp(x6, x7, MemOperand(x16, -8, PostIndex));
  __ Stp(x7, x6, MemOperand(x18, 8, PostIndex));
  __ Mov(x22, x18);
  __ Stp(x5, x4, MemOperand(x18, -8, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_64(0x44556677, x0);
  ASSERT_EQUAL_64(0x00112233, x1);
  ASSERT_EQUAL_64(0x00112233, x2);
  ASSERT_EQUAL_64(0xccddeeff, x3);
  ASSERT_EQUAL_64(0x4455667700112233, dst[0]);
  ASSERT_EQUAL_64(0x0000000000112233, dst[1]);
  ASSERT_EQUAL_64(0x0011223344556677, x4);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x5);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x6);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, x7);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, dst[2]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, dst[3]);
  ASSERT_EQUAL_64(0x0011223344556677, dst[4]);
  ASSERT_EQUAL_64(src_base, x16);
  ASSERT_EQUAL_64(dst_base, x17);
  ASSERT_EQUAL_64(dst_base + 16, x18);
  ASSERT_EQUAL_64(src_base + 4, x19);
  ASSERT_EQUAL_64(dst_base + 4, x20);
  ASSERT_EQUAL_64(src_base + 8, x21);
  ASSERT_EQUAL_64(dst_base + 24, x22);

  TEARDOWN();
}


TEST(ldp_stp_postindex_wide) {
  SETUP();

  uint64_t src[4] = {0x0011223344556677, 0x8899aabbccddeeff,
                     0xffeeddccbbaa9988, 0x7766554433221100};
  uint64_t dst[5] = {0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);
  // Move base too far from the array to force multiple instructions
  // to be emitted.
  const int64_t base_offset = 1024;

  START();
  __ Mov(x24, src_base);
  __ Mov(x25, dst_base);
  __ Mov(x18, dst_base + 16);
  __ Ldp(w0, w1, MemOperand(x24, base_offset + 4, PostIndex));
  __ Mov(x19, x24);
  __ Sub(x24, x24, base_offset);
  __ Ldp(w2, w3, MemOperand(x24, base_offset - 4, PostIndex));
  __ Stp(w2, w3, MemOperand(x25, 4 - base_offset, PostIndex));
  __ Mov(x20, x25);
  __ Sub(x24, x24, base_offset);
  __ Add(x25, x25, base_offset);
  __ Stp(w0, w1, MemOperand(x25, -4 - base_offset, PostIndex));
  __ Ldp(x4, x5, MemOperand(x24, base_offset + 8, PostIndex));
  __ Mov(x21, x24);
  __ Sub(x24, x24, base_offset);
  __ Ldp(x6, x7, MemOperand(x24, base_offset - 8, PostIndex));
  __ Stp(x7, x6, MemOperand(x18, 8 - base_offset, PostIndex));
  __ Mov(x22, x18);
  __ Add(x18, x18, base_offset);
  __ Stp(x5, x4, MemOperand(x18, -8 - base_offset, PostIndex));
  END();

  RUN();

  ASSERT_EQUAL_64(0x44556677, x0);
  ASSERT_EQUAL_64(0x00112233, x1);
  ASSERT_EQUAL_64(0x00112233, x2);
  ASSERT_EQUAL_64(0xccddeeff, x3);
  ASSERT_EQUAL_64(0x4455667700112233, dst[0]);
  ASSERT_EQUAL_64(0x0000000000112233, dst[1]);
  ASSERT_EQUAL_64(0x0011223344556677, x4);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x5);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, x6);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, x7);
  ASSERT_EQUAL_64(0xffeeddccbbaa9988, dst[2]);
  ASSERT_EQUAL_64(0x8899aabbccddeeff, dst[3]);
  ASSERT_EQUAL_64(0x0011223344556677, dst[4]);
  ASSERT_EQUAL_64(src_base + base_offset, x24);
  ASSERT_EQUAL_64(dst_base - base_offset, x25);
  ASSERT_EQUAL_64(dst_base - base_offset + 16, x18);
  ASSERT_EQUAL_64(src_base + base_offset + 4, x19);
  ASSERT_EQUAL_64(dst_base - base_offset + 4, x20);
  ASSERT_EQUAL_64(src_base + base_offset + 8, x21);
  ASSERT_EQUAL_64(dst_base - base_offset + 24, x22);

  TEARDOWN();
}


TEST(ldp_sign_extend) {
  SETUP();

  uint32_t src[2] = {0x80000000, 0x7fffffff};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);

  START();
  __ Mov(x24, src_base);
  __ Ldpsw(x0, x1, MemOperand(x24));
  END();

  RUN();

  ASSERT_EQUAL_64(0xffffffff80000000, x0);
  ASSERT_EQUAL_64(0x000000007fffffff, x1);

  TEARDOWN();
}


TEST(ldur_stur) {
  SETUP();

  int64_t src[2] = {0x0123456789abcdef, 0x0123456789abcdef};
  int64_t dst[5] = {0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, dst_base);
  __ Mov(x19, src_base + 16);
  __ Mov(x20, dst_base + 32);
  __ Mov(x21, dst_base + 40);
  __ Ldr(w0, MemOperand(x17, 1));
  __ Str(w0, MemOperand(x18, 2));
  __ Ldr(x1, MemOperand(x17, 3));
  __ Str(x1, MemOperand(x18, 9));
  __ Ldr(w2, MemOperand(x19, -9));
  __ Str(w2, MemOperand(x20, -5));
  __ Ldrb(w3, MemOperand(x19, -1));
  __ Strb(w3, MemOperand(x21, -1));
  END();

  RUN();

  ASSERT_EQUAL_64(0x6789abcd, x0);
  ASSERT_EQUAL_64(0x00006789abcd0000, dst[0]);
  ASSERT_EQUAL_64(0xabcdef0123456789, x1);
  ASSERT_EQUAL_64(0xcdef012345678900, dst[1]);
  ASSERT_EQUAL_64(0x000000ab, dst[2]);
  ASSERT_EQUAL_64(0xabcdef01, x2);
  ASSERT_EQUAL_64(0x00abcdef01000000, dst[3]);
  ASSERT_EQUAL_64(0x00000001, x3);
  ASSERT_EQUAL_64(0x0100000000000000, dst[4]);
  ASSERT_EQUAL_64(src_base, x17);
  ASSERT_EQUAL_64(dst_base, x18);
  ASSERT_EQUAL_64(src_base + 16, x19);
  ASSERT_EQUAL_64(dst_base + 32, x20);

  TEARDOWN();
}


TEST(ldur_stur_fp) {
  SETUP();

  int64_t src[3] = {0x0123456789abcdef, 0x0123456789abcdef,
                    0x0123456789abcdef};
  int64_t dst[5] = {0, 0, 0, 0, 0};
  uintptr_t src_base = reinterpret_cast<uintptr_t>(src);
  uintptr_t dst_base = reinterpret_cast<uintptr_t>(dst);

  START();
  __ Mov(x17, src_base);
  __ Mov(x18, dst_base);
  __ Ldr(b0, MemOperand(x17));
  __ Str(b0, MemOperand(x18));
  __ Ldr(h1, MemOperand(x17, 1));
  __ Str(h1, MemOperand(x18, 1));
  __ Ldr(s2, MemOperand(x17, 2));
  __ Str(s2, MemOperand(x18, 3));
  __ Ldr(d3, MemOperand(x17, 3));
  __ Str(d3, MemOperand(x18, 7));
  __ Ldr(q4, MemOperand(x17, 4));
  __ Str(q4, MemOperand(x18, 15));
  END();

  RUN();

  ASSERT_EQUAL_128(0, 0xef, q0);
  ASSERT_EQUAL_128(0, 0xabcd, q1);
  ASSERT_EQUAL_128(0, 0x456789ab, q2);
  ASSERT_EQUAL_128(0, 0xabcdef0123456789, q3);
  ASSERT_EQUAL_128(0x89abcdef01234567, 0x89abcdef01234567, q4);
  ASSERT_EQUAL_64(0x89456789ababcdef, dst[0]);
  ASSERT_EQUAL_64(0x67abcdef01234567, dst[1]);
  ASSERT_EQUAL_64(0x6789abcdef012345, dst[2]);
  ASSERT_EQUAL_64(0x0089abcdef012345, dst[3]);

  TEARDOWN();
}


TEST(ldr_literal) {
  SETUP();

  START();
  __ Ldr(x2, 0x1234567890abcdef);
  __ Ldr(w3, 0xfedcba09);
  __ Ldrsw(x4, 0x7fffffff);
  __ Ldrsw(x5, 0x80000000);
  __ Ldr(q11, 0x1234000056780000, 0xabcd0000ef000000);
  __ Ldr(d13, 1.234);
  __ Ldr(s25, 2.5);
  END();

  RUN();

  ASSERT_EQUAL_64(0x1234567890abcdef, x2);
  ASSERT_EQUAL_64(0xfedcba09, x3);
  ASSERT_EQUAL_64(0x7fffffff, x4);
  ASSERT_EQUAL_64(0xffffffff80000000, x5);
  ASSERT_EQUAL_128(0x1234000056780000, 0xabcd0000ef000000, q11);
  ASSERT_EQUAL_FP64(1.234, d13);
  ASSERT_EQUAL_FP32(2.5, s25);

  TEARDOWN();
}


TEST(ldr_literal_range) {
  SETUP();

  START();
  // Make sure the pool is empty;
  masm.EmitLiteralPool(LiteralPool::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  // Create some literal pool entries.
  __ Ldr(x0, 0x1234567890abcdef);
  __ Ldr(w1, 0xfedcba09);
  __ Ldrsw(x2, 0x7fffffff);
  __ Ldrsw(x3, 0x80000000);
  __ Ldr(q2, 0x1234000056780000, 0xabcd0000ef000000);
  __ Ldr(d0, 1.234);
  __ Ldr(s1, 2.5);
  ASSERT_LITERAL_POOL_SIZE(48);

  // Emit more code than the maximum literal load range to ensure the pool
  // should be emitted.
  const ptrdiff_t end = masm.CursorOffset() + 2 * kMaxLoadLiteralRange;
  while (masm.CursorOffset() < end) {
    __ Nop();
  }

  // The pool should have been emitted.
  ASSERT_LITERAL_POOL_SIZE(0);

  // These loads should be after the pool (and will require a new one).
  __ Ldr(x4, 0x34567890abcdef12);
  __ Ldr(w5, 0xdcba09fe);
  __ Ldrsw(x6, 0x7fffffff);
  __ Ldrsw(x7, 0x80000000);
  __ Ldr(q6, 0x1234000056780000, 0xabcd0000ef000000);
  __ Ldr(d4, 123.4);
  __ Ldr(s5, 250.0);
  ASSERT_LITERAL_POOL_SIZE(48);
  END();

  RUN();

  // Check that the literals loaded correctly.
  ASSERT_EQUAL_64(0x1234567890abcdef, x0);
  ASSERT_EQUAL_64(0xfedcba09, x1);
  ASSERT_EQUAL_64(0x7fffffff, x2);
  ASSERT_EQUAL_64(0xffffffff80000000, x3);
  ASSERT_EQUAL_128(0x1234000056780000, 0xabcd0000ef000000, q2);
  ASSERT_EQUAL_FP64(1.234, d0);
  ASSERT_EQUAL_FP32(2.5, s1);
  ASSERT_EQUAL_64(0x34567890abcdef12, x4);
  ASSERT_EQUAL_64(0xdcba09fe, x5);
  ASSERT_EQUAL_64(0x7fffffff, x6);
  ASSERT_EQUAL_64(0xffffffff80000000, x7);
  ASSERT_EQUAL_128(0x1234000056780000, 0xabcd0000ef000000, q6);
  ASSERT_EQUAL_FP64(123.4, d4);
  ASSERT_EQUAL_FP32(250.0, s5);

  TEARDOWN();
}


TEST(ldr_literal_values_q) {
  SETUP();

  static const uint64_t kHalfValues[] = {
    0x8000000000000000, 0x7fffffffffffffff, 0x0000000000000000,
    0xffffffffffffffff, 0x00ff00ff00ff00ff, 0x1234567890abcdef
  };
  const int card = sizeof(kHalfValues) / sizeof(kHalfValues[0]);
  const Register& ref_low64 = x1;
  const Register& ref_high64 = x2;
  const Register& loaded_low64 = x3;
  const Register& loaded_high64 = x4;
  const VRegister& tgt = q0;

  START();
  __ Mov(x0, 0);

  for (int i = 0; i < card; i++) {
    __ Mov(ref_low64, kHalfValues[i]);
    for (int j = 0; j < card; j++) {
      __ Mov(ref_high64, kHalfValues[j]);
      __ Ldr(tgt, kHalfValues[j], kHalfValues[i]);
      __ Mov(loaded_low64, tgt.V2D(), 0);
      __ Mov(loaded_high64, tgt.V2D(), 1);
      __ Cmp(loaded_low64, ref_low64);
      __ Ccmp(loaded_high64, ref_high64, NoFlag, eq);
      __ Cset(x0, ne);
    }
  }
  END();

  RUN();

  // If one of the values differs, the trace can be used to identify which one.
  ASSERT_EQUAL_64(0, x0);

  TEARDOWN();
}


template <typename T>
void LoadIntValueHelper(T values[], int card) {
  SETUP();

  const bool is_32bits = (sizeof(T) == 4);
  const Register& tgt1 = is_32bits ? w1 : x1;
  const Register& tgt2 = is_32bits ? w2 : x2;

  START();
  __ Mov(x0, 0);

  // If one of the values differ then x0 will be one.
  for (int i = 0; i < card; ++i) {
    __ Mov(tgt1, values[i]);
    __ Ldr(tgt2, values[i]);
    __ Cmp(tgt1, tgt2);
    __ Cset(x0, ne);
  }
  END();

  RUN();

  // If one of the values differs, the trace can be used to identify which one.
  ASSERT_EQUAL_64(0, x0);

  TEARDOWN();
}


TEST(ldr_literal_values_x) {
  static const uint64_t kValues[] = {
    0x8000000000000000, 0x7fffffffffffffff, 0x0000000000000000,
    0xffffffffffffffff, 0x00ff00ff00ff00ff, 0x1234567890abcdef
  };

  LoadIntValueHelper(kValues, sizeof(kValues) / sizeof(kValues[0]));
}


TEST(ldr_literal_values_w) {
  static const uint32_t kValues[] = {
    0x80000000, 0x7fffffff, 0x00000000, 0xffffffff, 0x00ff00ff, 0x12345678,
    0x90abcdef
  };

  LoadIntValueHelper(kValues, sizeof(kValues) / sizeof(kValues[0]));
}


template <typename T>
void LoadFPValueHelper(T values[], int card) {
  SETUP();

  const bool is_32bits = (sizeof(T) == 4);
  const FPRegister& fp_tgt = is_32bits ? s2 : d2;
  const Register& tgt1 = is_32bits ? w1 : x1;
  const Register& tgt2 = is_32bits ? w2 : x2;

  START();
  __ Mov(x0, 0);

  // If one of the values differ then x0 will be one.
  for (int i = 0; i < card; ++i) {
    __ Mov(tgt1, is_32bits ? float_to_rawbits(values[i])
                           : double_to_rawbits(values[i]));
    __ Ldr(fp_tgt, values[i]);
    __ Fmov(tgt2, fp_tgt);
    __ Cmp(tgt1, tgt2);
    __ Cset(x0, ne);
  }
  END();

  RUN();

  // If one of the values differs, the trace can be used to identify which one.
  ASSERT_EQUAL_64(0, x0);

  TEARDOWN();
}

TEST(ldr_literal_values_d) {
  static const double kValues[] = {
    -0.0, 0.0, -1.0, 1.0, -1e10, 1e10
  };

  LoadFPValueHelper(kValues, sizeof(kValues) / sizeof(kValues[0]));
}


TEST(ldr_literal_values_s) {
  static const float kValues[] = {
    -0.0, 0.0, -1.0, 1.0, -1e10, 1e10
  };

  LoadFPValueHelper(kValues, sizeof(kValues) / sizeof(kValues[0]));
}


TEST(ldr_literal_custom) {
  SETUP();
  ALLOW_ASM();

  Label end_of_pool_before;
  Label end_of_pool_after;
  Literal<uint64_t> before_x(0x1234567890abcdef);
  Literal<uint32_t> before_w(0xfedcba09);
  Literal<uint32_t> before_sx(0x80000000);
  Literal<uint64_t> before_q(0x1234000056780000, 0xabcd0000ef000000);
  Literal<double> before_d(1.234);
  Literal<float> before_s(2.5);
  Literal<uint64_t> after_x(0x1234567890abcdef);
  Literal<uint32_t> after_w(0xfedcba09);
  Literal<uint32_t> after_sx(0x80000000);
  Literal<uint64_t> after_q(0x1234000056780000, 0xabcd0000ef000000);
  Literal<double> after_d(1.234);
  Literal<float> after_s(2.5);

  START();

  // Manually generate a pool.
  __ B(&end_of_pool_before);
  __ place(&before_x);
  __ place(&before_w);
  __ place(&before_sx);
  __ place(&before_q);
  __ place(&before_d);
  __ place(&before_s);
  __ Bind(&end_of_pool_before);

  __ ldr(x2, &before_x);
  __ ldr(w3, &before_w);
  __ ldrsw(x5, &before_sx);
  __ ldr(q11, &before_q);
  __ ldr(d13, &before_d);
  __ ldr(s25, &before_s);

  __ ldr(x6, &after_x);
  __ ldr(w7, &after_w);
  __ ldrsw(x8, &after_sx);
  __ ldr(q18, &after_q);
  __ ldr(d14, &after_d);
  __ ldr(s26, &after_s);

  // Manually generate a pool.
  __ B(&end_of_pool_after);
  __ place(&after_x);
  __ place(&after_w);
  __ place(&after_sx);
  __ place(&after_q);
  __ place(&after_d);
  __ place(&after_s);
  __ Bind(&end_of_pool_after);

  END();

  RUN();

  ASSERT_EQUAL_64(0x1234567890abcdef, x2);
  ASSERT_EQUAL_64(0xfedcba09, x3);
  ASSERT_EQUAL_64(0xffffffff80000000, x5);
  ASSERT_EQUAL_128(0x1234000056780000, 0xabcd0000ef000000, q11);
  ASSERT_EQUAL_FP64(1.234, d13);
  ASSERT_EQUAL_FP32(2.5, s25);

  ASSERT_EQUAL_64(0x1234567890abcdef, x6);
  ASSERT_EQUAL_64(0xfedcba09, x7);
  ASSERT_EQUAL_64(0xffffffff80000000, x8);
  ASSERT_EQUAL_128(0x1234000056780000, 0xabcd0000ef000000, q18);
  ASSERT_EQUAL_FP64(1.234, d14);
  ASSERT_EQUAL_FP32(2.5, s26);

  TEARDOWN();
}


TEST(ldr_literal_custom_shared) {
  SETUP();
  ALLOW_ASM();

  Label end_of_pool_before;
  Label end_of_pool_after;
  Literal<uint64_t> before_x(0x1234567890abcdef);
  Literal<uint32_t> before_w(0xfedcba09);
  Literal<uint64_t> before_q(0x1234000056780000, 0xabcd0000ef000000);
  Literal<double> before_d(1.234);
  Literal<float> before_s(2.5);
  Literal<uint64_t> after_x(0x1234567890abcdef);
  Literal<uint32_t> after_w(0xfedcba09);
  Literal<uint64_t> after_q(0x1234000056780000, 0xabcd0000ef000000);
  Literal<double> after_d(1.234);
  Literal<float> after_s(2.5);

  START();

  // Manually generate a pool.
  __ B(&end_of_pool_before);
  __ place(&before_x);
  __ place(&before_w);
  __ place(&before_q);
  __ place(&before_d);
  __ place(&before_s);
  __ Bind(&end_of_pool_before);

  // Load the entries several times to test that literals can be shared.
  for (int i = 0; i < 50; i++) {
    __ ldr(x2, &before_x);
    __ ldr(w3, &before_w);
    __ ldrsw(x5, &before_w);    // Re-use before_w.
    __ ldr(q11, &before_q);
    __ ldr(d13, &before_d);
    __ ldr(s25, &before_s);

    __ ldr(x6, &after_x);
    __ ldr(w7, &after_w);
    __ ldrsw(x8, &after_w);     // Re-use after_w.
    __ ldr(q18, &after_q);
    __ ldr(d14, &after_d);
    __ ldr(s26, &after_s);
  }

  // Manually generate a pool.
  __ B(&end_of_pool_after);
  __ place(&after_x);
  __ place(&after_w);
  __ place(&after_q);
  __ place(&after_d);
  __ place(&after_s);
  __ Bind(&end_of_pool_after);

  END();

  RUN();

  ASSERT_EQUAL_64(0x1234567890abcdef, x2);
  ASSERT_EQUAL_64(0xfedcba09, x3);
  ASSERT_EQUAL_64(0xfffffffffedcba09, x5);
  ASSERT_EQUAL_128(0x1234000056780000, 0xabcd0000ef000000, q11);
  ASSERT_EQUAL_FP64(1.234, d13);
  ASSERT_EQUAL_FP32(2.5, s25);

  ASSERT_EQUAL_64(0x1234567890abcdef, x6);
  ASSERT_EQUAL_64(0xfedcba09, x7);
  ASSERT_EQUAL_64(0xfffffffffedcba09, x8);
  ASSERT_EQUAL_128(0x1234000056780000, 0xabcd0000ef000000, q18);
  ASSERT_EQUAL_FP64(1.234, d14);
  ASSERT_EQUAL_FP32(2.5, s26);

  TEARDOWN();
}


TEST(prfm_offset) {
  SETUP();

  START();
  // The address used in prfm doesn't have to be valid.
  __ Mov(x0, 0x0123456789abcdef);

  for (int i = 0; i < (1 << ImmPrefetchOperation_width); i++) {
    // Unallocated prefetch operations are ignored, so test all of them.
    PrefetchOperation op = static_cast<PrefetchOperation>(i);

    __ Prfm(op, MemOperand(x0));
    __ Prfm(op, MemOperand(x0, 8));
    __ Prfm(op, MemOperand(x0, 32760));
    __ Prfm(op, MemOperand(x0, 32768));

    __ Prfm(op, MemOperand(x0, 1));
    __ Prfm(op, MemOperand(x0, 9));
    __ Prfm(op, MemOperand(x0, 255));
    __ Prfm(op, MemOperand(x0, 257));
    __ Prfm(op, MemOperand(x0, -1));
    __ Prfm(op, MemOperand(x0, -9));
    __ Prfm(op, MemOperand(x0, -255));
    __ Prfm(op, MemOperand(x0, -257));

    __ Prfm(op, MemOperand(x0, 0xfedcba9876543210));
  }

  END();
  RUN();
  TEARDOWN();
}


TEST(prfm_regoffset) {
  SETUP();

  START();
  // The address used in prfm doesn't have to be valid.
  __ Mov(x0, 0x0123456789abcdef);

  CPURegList inputs(CPURegister::kRegister, kXRegSize, 10, 18);
  __ Mov(x10, 0);
  __ Mov(x11, 1);
  __ Mov(x12, 8);
  __ Mov(x13, 255);
  __ Mov(x14, -0);
  __ Mov(x15, -1);
  __ Mov(x16, -8);
  __ Mov(x17, -255);
  __ Mov(x18, 0xfedcba9876543210);

  for (int i = 0; i < (1 << ImmPrefetchOperation_width); i++) {
    // Unallocated prefetch operations are ignored, so test all of them.
    PrefetchOperation op = static_cast<PrefetchOperation>(i);

    CPURegList loop = inputs;
    while (!loop.IsEmpty()) {
      Register input(loop.PopLowestIndex());
      __ Prfm(op, MemOperand(x0, input));
      __ Prfm(op, MemOperand(x0, input, UXTW));
      __ Prfm(op, MemOperand(x0, input, UXTW, 3));
      __ Prfm(op, MemOperand(x0, input, LSL));
      __ Prfm(op, MemOperand(x0, input, LSL, 3));
      __ Prfm(op, MemOperand(x0, input, SXTW));
      __ Prfm(op, MemOperand(x0, input, SXTW, 3));
      __ Prfm(op, MemOperand(x0, input, SXTX));
      __ Prfm(op, MemOperand(x0, input, SXTX, 3));
    }
  }

  END();
  RUN();
  TEARDOWN();
}


TEST(prfm_literal_imm19) {
  SETUP();
  ALLOW_ASM();
  START();

  for (int i = 0; i < (1 << ImmPrefetchOperation_width); i++) {
    // Unallocated prefetch operations are ignored, so test all of them.
    PrefetchOperation op = static_cast<PrefetchOperation>(i);

    // The address used in prfm doesn't have to be valid.
    __ prfm(op, 0);
    __ prfm(op, 1);
    __ prfm(op, -1);
    __ prfm(op, 1000);
    __ prfm(op, -1000);
    __ prfm(op, 0x3ffff);
    __ prfm(op, -0x40000);
  }

  END();
  RUN();
  TEARDOWN();
}


TEST(prfm_literal) {
  SETUP();
  ALLOW_ASM();

  Label end_of_pool_before;
  Label end_of_pool_after;
  Literal<uint64_t> before(0);
  Literal<uint64_t> after(0);

  START();

  // Manually generate a pool.
  __ B(&end_of_pool_before);
  __ place(&before);
  __ Bind(&end_of_pool_before);

  for (int i = 0; i < (1 << ImmPrefetchOperation_width); i++) {
    // Unallocated prefetch operations are ignored, so test all of them.
    PrefetchOperation op = static_cast<PrefetchOperation>(i);

    CodeBufferCheckScope guard(&masm, 2 * kInstructionSize);
    __ prfm(op, &before);
    __ prfm(op, &after);
  }

  // Manually generate a pool.
  __ B(&end_of_pool_after);
  __ place(&after);
  __ Bind(&end_of_pool_after);

  END();
  RUN();
  TEARDOWN();
}


TEST(prfm_wide) {
  SETUP();

  START();
  // The address used in prfm doesn't have to be valid.
  __ Mov(x0, 0x0123456789abcdef);

  for (int i = 0; i < (1 << ImmPrefetchOperation_width); i++) {
    // Unallocated prefetch operations are ignored, so test all of them.
    PrefetchOperation op = static_cast<PrefetchOperation>(i);

    __ Prfm(op, MemOperand(x0, 0x40000));
    __ Prfm(op, MemOperand(x0, -0x40001));
    __ Prfm(op, MemOperand(x0, UINT64_C(0x5555555555555555)));
    __ Prfm(op, MemOperand(x0, UINT64_C(0xfedcba9876543210)));
  }

  END();
  RUN();
  TEARDOWN();
}


TEST(load_prfm_literal) {
  // Test literals shared between both prfm and ldr.
  SETUP();
  ALLOW_ASM();

  Label end_of_pool_before;
  Label end_of_pool_after;
  Literal<uint64_t> before_x(0x1234567890abcdef);
  Literal<uint32_t> before_w(0xfedcba09);
  Literal<uint32_t> before_sx(0x80000000);
  Literal<double> before_d(1.234);
  Literal<float> before_s(2.5);
  Literal<uint64_t> after_x(0x1234567890abcdef);
  Literal<uint32_t> after_w(0xfedcba09);
  Literal<uint32_t> after_sx(0x80000000);
  Literal<double> after_d(1.234);
  Literal<float> after_s(2.5);

  START();

  // Manually generate a pool.
  __ B(&end_of_pool_before);
  __ place(&before_x);
  __ place(&before_w);
  __ place(&before_sx);
  __ place(&before_d);
  __ place(&before_s);
  __ Bind(&end_of_pool_before);

  for (int i = 0; i < (1 << ImmPrefetchOperation_width); i++) {
    // Unallocated prefetch operations are ignored, so test all of them.
    PrefetchOperation op = static_cast<PrefetchOperation>(i);

    __ prfm(op, &before_x);
    __ prfm(op, &before_w);
    __ prfm(op, &before_sx);
    __ prfm(op, &before_d);
    __ prfm(op, &before_s);

    __ prfm(op, &after_x);
    __ prfm(op, &after_w);
    __ prfm(op, &after_sx);
    __ prfm(op, &after_d);
    __ prfm(op, &after_s);
  }

  __ ldr(x2, &before_x);
  __ ldr(w3, &before_w);
  __ ldrsw(x5, &before_sx);
  __ ldr(d13, &before_d);
  __ ldr(s25, &before_s);

  __ ldr(x6, &after_x);
  __ ldr(w7, &after_w);
  __ ldrsw(x8, &after_sx);
  __ ldr(d14, &after_d);
  __ ldr(s26, &after_s);

  // Manually generate a pool.
  __ B(&end_of_pool_after);
  __ place(&after_x);
  __ place(&after_w);
  __ place(&after_sx);
  __ place(&after_d);
  __ place(&after_s);
  __ Bind(&end_of_pool_after);

  END();

  RUN();

  ASSERT_EQUAL_64(0x1234567890abcdef, x2);
  ASSERT_EQUAL_64(0xfedcba09, x3);
  ASSERT_EQUAL_64(0xffffffff80000000, x5);
  ASSERT_EQUAL_FP64(1.234, d13);
  ASSERT_EQUAL_FP32(2.5, s25);

  ASSERT_EQUAL_64(0x1234567890abcdef, x6);
  ASSERT_EQUAL_64(0xfedcba09, x7);
  ASSERT_EQUAL_64(0xffffffff80000000, x8);
  ASSERT_EQUAL_FP64(1.234, d14);
  ASSERT_EQUAL_FP32(2.5, s26);

  TEARDOWN();
}


TEST(add_sub_imm) {
  SETUP();

  START();
  __ Mov(x0, 0x0);
  __ Mov(x1, 0x1111);
  __ Mov(x2, 0xffffffffffffffff);
  __ Mov(x3, 0x8000000000000000);

  __ Add(x10, x0, Operand(0x123));
  __ Add(x11, x1, Operand(0x122000));
  __ Add(x12, x0, Operand(0xabc << 12));
  __ Add(x13, x2, Operand(1));

  __ Add(w14, w0, Operand(0x123));
  __ Add(w15, w1, Operand(0x122000));
  __ Add(w16, w0, Operand(0xabc << 12));
  __ Add(w17, w2, Operand(1));

  __ Sub(x20, x0, Operand(0x1));
  __ Sub(x21, x1, Operand(0x111));
  __ Sub(x22, x1, Operand(0x1 << 12));
  __ Sub(x23, x3, Operand(1));

  __ Sub(w24, w0, Operand(0x1));
  __ Sub(w25, w1, Operand(0x111));
  __ Sub(w26, w1, Operand(0x1 << 12));
  __ Sub(w27, w3, Operand(1));
  END();

  RUN();

  ASSERT_EQUAL_64(0x123, x10);
  ASSERT_EQUAL_64(0x123111, x11);
  ASSERT_EQUAL_64(0xabc000, x12);
  ASSERT_EQUAL_64(0x0, x13);

  ASSERT_EQUAL_32(0x123, w14);
  ASSERT_EQUAL_32(0x123111, w15);
  ASSERT_EQUAL_32(0xabc000, w16);
  ASSERT_EQUAL_32(0x0, w17);

  ASSERT_EQUAL_64(0xffffffffffffffff, x20);
  ASSERT_EQUAL_64(0x1000, x21);
  ASSERT_EQUAL_64(0x111, x22);
  ASSERT_EQUAL_64(0x7fffffffffffffff, x23);

  ASSERT_EQUAL_32(0xffffffff, w24);
  ASSERT_EQUAL_32(0x1000, w25);
  ASSERT_EQUAL_32(0x111, w26);
  ASSERT_EQUAL_32(0xffffffff, w27);

  TEARDOWN();
}


TEST(add_sub_wide_imm) {
  SETUP();

  START();
  __ Mov(x0, 0x0);
  __ Mov(x1, 0x1);

  __ Add(x10, x0, Operand(0x1234567890abcdef));
  __ Add(x11, x1, Operand(0xffffffff));

  __ Add(w12, w0, Operand(0x12345678));
  __ Add(w13, w1, Operand(0xffffffff));

  __ Add(w18, w0, Operand(kWMinInt));
  __ Sub(w19, w0, Operand(kWMinInt));

  __ Sub(x20, x0, Operand(0x1234567890abcdef));
  __ Sub(w21, w0, Operand(0x12345678));

  END();

  RUN();

  ASSERT_EQUAL_64(0x1234567890abcdef, x10);
  ASSERT_EQUAL_64(0x100000000, x11);

  ASSERT_EQUAL_32(0x12345678, w12);
  ASSERT_EQUAL_64(0x0, x13);

  ASSERT_EQUAL_32(kWMinInt, w18);
  ASSERT_EQUAL_32(kWMinInt, w19);

  ASSERT_EQUAL_64(-0x1234567890abcdef, x20);
  ASSERT_EQUAL_32(-0x12345678, w21);

  TEARDOWN();
}


TEST(add_sub_shifted) {
  SETUP();

  START();
  __ Mov(x0, 0);
  __ Mov(x1, 0x0123456789abcdef);
  __ Mov(x2, 0xfedcba9876543210);
  __ Mov(x3, 0xffffffffffffffff);

  __ Add(x10, x1, Operand(x2));
  __ Add(x11, x0, Operand(x1, LSL, 8));
  __ Add(x12, x0, Operand(x1, LSR, 8));
  __ Add(x13, x0, Operand(x1, ASR, 8));
  __ Add(x14, x0, Operand(x2, ASR, 8));
  __ Add(w15, w0, Operand(w1, ASR, 8));
  __ Add(w18, w3, Operand(w1, ROR, 8));
  __ Add(x19, x3, Operand(x1, ROR, 8));

  __ Sub(x20, x3, Operand(x2));
  __ Sub(x21, x3, Operand(x1, LSL, 8));
  __ Sub(x22, x3, Operand(x1, LSR, 8));
  __ Sub(x23, x3, Operand(x1, ASR, 8));
  __ Sub(x24, x3, Operand(x2, ASR, 8));
  __ Sub(w25, w3, Operand(w1, ASR, 8));
  __ Sub(w26, w3, Operand(w1, ROR, 8));
  __ Sub(x27, x3, Operand(x1, ROR, 8));
  END();

  RUN();

  ASSERT_EQUAL_64(0xffffffffffffffff, x10);
  ASSERT_EQUAL_64(0x23456789abcdef00, x11);
  ASSERT_EQUAL_64(0x000123456789abcd, x12);
  ASSERT_EQUAL_64(0x000123456789abcd, x13);
  ASSERT_EQUAL_64(0xfffedcba98765432, x14);
  ASSERT_EQUAL_64(0xff89abcd, x15);
  ASSERT_EQUAL_64(0xef89abcc, x18);
  ASSERT_EQUAL_64(0xef0123456789abcc, x19);

  ASSERT_EQUAL_64(0x0123456789abcdef, x20);
  ASSERT_EQUAL_64(0xdcba9876543210ff, x21);
  ASSERT_EQUAL_64(0xfffedcba98765432, x22);
  ASSERT_EQUAL_64(0xfffedcba98765432, x23);
  ASSERT_EQUAL_64(0x000123456789abcd, x24);
  ASSERT_EQUAL_64(0x00765432, x25);
  ASSERT_EQUAL_64(0x10765432, x26);
  ASSERT_EQUAL_64(0x10fedcba98765432, x27);

  TEARDOWN();
}


TEST(add_sub_extended) {
  SETUP();

  START();
  __ Mov(x0, 0);
  __ Mov(x1, 0x0123456789abcdef);
  __ Mov(x2, 0xfedcba9876543210);
  __ Mov(w3, 0x80);

  __ Add(x10, x0, Operand(x1, UXTB, 0));
  __ Add(x11, x0, Operand(x1, UXTB, 1));
  __ Add(x12, x0, Operand(x1, UXTH, 2));
  __ Add(x13, x0, Operand(x1, UXTW, 4));

  __ Add(x14, x0, Operand(x1, SXTB, 0));
  __ Add(x15, x0, Operand(x1, SXTB, 1));
  __ Add(x16, x0, Operand(x1, SXTH, 2));
  __ Add(x17, x0, Operand(x1, SXTW, 3));
  __ Add(x18, x0, Operand(x2, SXTB, 0));
  __ Add(x19, x0, Operand(x2, SXTB, 1));
  __ Add(x20, x0, Operand(x2, SXTH, 2));
  __ Add(x21, x0, Operand(x2, SXTW, 3));

  __ Add(x22, x1, Operand(x2, SXTB, 1));
  __ Sub(x23, x1, Operand(x2, SXTB, 1));

  __ Add(w24, w1, Operand(w2, UXTB, 2));
  __ Add(w25, w0, Operand(w1, SXTB, 0));
  __ Add(w26, w0, Operand(w1, SXTB, 1));
  __ Add(w27, w2, Operand(w1, SXTW, 3));

  __ Add(w28, w0, Operand(w1, SXTW, 3));
  __ Add(x29, x0, Operand(w1, SXTW, 3));

  __ Sub(x30, x0, Operand(w3, SXTB, 1));
  END();

  RUN();

  ASSERT_EQUAL_64(0xef, x10);
  ASSERT_EQUAL_64(0x1de, x11);
  ASSERT_EQUAL_64(0x337bc, x12);
  ASSERT_EQUAL_64(0x89abcdef0, x13);

  ASSERT_EQUAL_64(0xffffffffffffffef, x14);
  ASSERT_EQUAL_64(0xffffffffffffffde, x15);
  ASSERT_EQUAL_64(0xffffffffffff37bc, x16);
  ASSERT_EQUAL_64(0xfffffffc4d5e6f78, x17);
  ASSERT_EQUAL_64(0x10, x18);
  ASSERT_EQUAL_64(0x20, x19);
  ASSERT_EQUAL_64(0xc840, x20);
  ASSERT_EQUAL_64(0x3b2a19080, x21);

  ASSERT_EQUAL_64(0x0123456789abce0f, x22);
  ASSERT_EQUAL_64(0x0123456789abcdcf, x23);

  ASSERT_EQUAL_32(0x89abce2f, w24);
  ASSERT_EQUAL_32(0xffffffef, w25);
  ASSERT_EQUAL_32(0xffffffde, w26);
  ASSERT_EQUAL_32(0xc3b2a188, w27);

  ASSERT_EQUAL_32(0x4d5e6f78, w28);
  ASSERT_EQUAL_64(0xfffffffc4d5e6f78, x29);

  ASSERT_EQUAL_64(256, x30);

  TEARDOWN();
}


TEST(add_sub_negative) {
  SETUP();

  START();
  __ Mov(x0, 0);
  __ Mov(x1, 4687);
  __ Mov(x2, 0x1122334455667788);
  __ Mov(w3, 0x11223344);
  __ Mov(w4, 400000);

  __ Add(x10, x0, -42);
  __ Add(x11, x1, -687);
  __ Add(x12, x2, -0x88);

  __ Sub(x13, x0, -600);
  __ Sub(x14, x1, -313);
  __ Sub(x15, x2, -0x555);

  __ Add(w19, w3, -0x344);
  __ Add(w20, w4, -2000);

  __ Sub(w21, w3, -0xbc);
  __ Sub(w22, w4, -2000);
  END();

  RUN();

  ASSERT_EQUAL_64(-42, x10);
  ASSERT_EQUAL_64(4000, x11);
  ASSERT_EQUAL_64(0x1122334455667700, x12);

  ASSERT_EQUAL_64(600, x13);
  ASSERT_EQUAL_64(5000, x14);
  ASSERT_EQUAL_64(0x1122334455667cdd, x15);

  ASSERT_EQUAL_32(0x11223000, w19);
  ASSERT_EQUAL_32(398000, w20);

  ASSERT_EQUAL_32(0x11223400, w21);
  ASSERT_EQUAL_32(402000, w22);

  TEARDOWN();
}


TEST(add_sub_zero) {
  SETUP();

  START();
  __ Mov(x0, 0);
  __ Mov(x1, 0);
  __ Mov(x2, 0);

  Label blob1;
  __ Bind(&blob1);
  __ Add(x0, x0, 0);
  __ Sub(x1, x1, 0);
  __ Sub(x2, x2, xzr);
  VIXL_CHECK(__ SizeOfCodeGeneratedSince(&blob1) == 0);

  Label blob2;
  __ Bind(&blob2);
  __ Add(w3, w3, 0);
  VIXL_CHECK(__ SizeOfCodeGeneratedSince(&blob2) != 0);

  Label blob3;
  __ Bind(&blob3);
  __ Sub(w3, w3, wzr);
  VIXL_CHECK(__ SizeOfCodeGeneratedSince(&blob3) != 0);

  END();

  RUN();

  ASSERT_EQUAL_64(0, x0);
  ASSERT_EQUAL_64(0, x1);
  ASSERT_EQUAL_64(0, x2);

  TEARDOWN();
}


TEST(claim_drop_zero) {
  SETUP();

  START();

  Label start;
  __ Bind(&start);
  __ Claim(Operand(0));
  __ Drop(Operand(0));
  __ Claim(Operand(xzr));
  __ Drop(Operand(xzr));
  VIXL_CHECK(__ SizeOfCodeGeneratedSince(&start) == 0);

  END();

  RUN();

  TEARDOWN();
}


TEST(neg) {
  SETUP();

  START();
  __ Mov(x0, 0xf123456789abcdef);

  // Immediate.
  __ Neg(x1, 0x123);
  __ Neg(w2, 0x123);

  // Shifted.
  __ Neg(x3, Operand(x0, LSL, 1));
  __ Neg(w4, Operand(w0, LSL, 2));
  __ Neg(x5, Operand(x0, LSR, 3));
  __ Neg(w6, Operand(w0, LSR, 4));
  __ Neg(x7, Operand(x0, ASR, 5));
  __ Neg(w8, Operand(w0, ASR, 6));

  // Extended.
  __ Neg(w9, Operand(w0, UXTB));
  __ Neg(x10, Operand(x0, SXTB, 1));
  __ Neg(w11, Operand(w0, UXTH, 2));
  __ Neg(x12, Operand(x0, SXTH, 3));
  __ Neg(w13, Operand(w0, UXTW, 4));
  __ Neg(x14, Operand(x0, SXTW, 4));
  END();

  RUN();

  ASSERT_EQUAL_64(0xfffffffffffffedd, x1);
  ASSERT_EQUAL_64(0xfffffedd, x2);
  ASSERT_EQUAL_64(0x1db97530eca86422, x3);
  ASSERT_EQUAL_64(0xd950c844, x4);
  ASSERT_EQUAL_64(0xe1db97530eca8643, x5);
  ASSERT_EQUAL_64(0xf7654322, x6);
  ASSERT_EQUAL_64(0x0076e5d4c3b2a191, x7);
  ASSERT_EQUAL_64(0x01d950c9, x8);
  ASSERT_EQUAL_64(0xffffff11, x9);
  ASSERT_EQUAL_64(0x0000000000000022, x10);
  ASSERT_EQUAL_64(0xfffcc844, x11);
  ASSERT_EQUAL_64(0x0000000000019088, x12);
  ASSERT_EQUAL_64(0x65432110, x13);
  ASSERT_EQUAL_64(0x0000000765432110, x14);

  TEARDOWN();
}


template<typename T, typename Op>
static void AdcsSbcsHelper(Op op, T left, T right, int carry,
                           T expected, StatusFlags expected_flags) {
  int reg_size = sizeof(T) * 8;
  Register left_reg(0, reg_size);
  Register right_reg(1, reg_size);
  Register result_reg(2, reg_size);

  SETUP();
  START();

  __ Mov(left_reg, left);
  __ Mov(right_reg, right);
  __ Mov(x10, (carry ? CFlag : NoFlag));

  __ Msr(NZCV, x10);
  (masm.*op)(result_reg, left_reg, right_reg);

  END();
  RUN();

  ASSERT_EQUAL_64(left, left_reg.X());
  ASSERT_EQUAL_64(right, right_reg.X());
  ASSERT_EQUAL_64(expected, result_reg.X());
  ASSERT_EQUAL_NZCV(expected_flags);

  TEARDOWN();
}


TEST(adcs_sbcs_x) {
  uint64_t inputs[] = {
    0x0000000000000000, 0x0000000000000001,
    0x7ffffffffffffffe, 0x7fffffffffffffff,
    0x8000000000000000, 0x8000000000000001,
    0xfffffffffffffffe, 0xffffffffffffffff,
  };
  static const size_t input_count = sizeof(inputs) / sizeof(inputs[0]);

  struct Expected {
    uint64_t carry0_result;
    StatusFlags carry0_flags;
    uint64_t carry1_result;
    StatusFlags carry1_flags;
  };

  static const Expected expected_adcs_x[input_count][input_count] = {
    {{0x0000000000000000, ZFlag, 0x0000000000000001, NoFlag},
     {0x0000000000000001, NoFlag, 0x0000000000000002, NoFlag},
     {0x7ffffffffffffffe, NoFlag, 0x7fffffffffffffff, NoFlag},
     {0x7fffffffffffffff, NoFlag, 0x8000000000000000, NVFlag},
     {0x8000000000000000, NFlag, 0x8000000000000001, NFlag},
     {0x8000000000000001, NFlag, 0x8000000000000002, NFlag},
     {0xfffffffffffffffe, NFlag, 0xffffffffffffffff, NFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag}},
    {{0x0000000000000001, NoFlag, 0x0000000000000002, NoFlag},
     {0x0000000000000002, NoFlag, 0x0000000000000003, NoFlag},
     {0x7fffffffffffffff, NoFlag, 0x8000000000000000, NVFlag},
     {0x8000000000000000, NVFlag, 0x8000000000000001, NVFlag},
     {0x8000000000000001, NFlag, 0x8000000000000002, NFlag},
     {0x8000000000000002, NFlag, 0x8000000000000003, NFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0x0000000000000000, ZCFlag, 0x0000000000000001, CFlag}},
    {{0x7ffffffffffffffe, NoFlag, 0x7fffffffffffffff, NoFlag},
     {0x7fffffffffffffff, NoFlag, 0x8000000000000000, NVFlag},
     {0xfffffffffffffffc, NVFlag, 0xfffffffffffffffd, NVFlag},
     {0xfffffffffffffffd, NVFlag, 0xfffffffffffffffe, NVFlag},
     {0xfffffffffffffffe, NFlag, 0xffffffffffffffff, NFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0x7ffffffffffffffc, CFlag, 0x7ffffffffffffffd, CFlag},
     {0x7ffffffffffffffd, CFlag, 0x7ffffffffffffffe, CFlag}},
    {{0x7fffffffffffffff, NoFlag, 0x8000000000000000, NVFlag},
     {0x8000000000000000, NVFlag, 0x8000000000000001, NVFlag},
     {0xfffffffffffffffd, NVFlag, 0xfffffffffffffffe, NVFlag},
     {0xfffffffffffffffe, NVFlag, 0xffffffffffffffff, NVFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0x0000000000000000, ZCFlag, 0x0000000000000001, CFlag},
     {0x7ffffffffffffffd, CFlag, 0x7ffffffffffffffe, CFlag},
     {0x7ffffffffffffffe, CFlag, 0x7fffffffffffffff, CFlag}},
    {{0x8000000000000000, NFlag, 0x8000000000000001, NFlag},
     {0x8000000000000001, NFlag, 0x8000000000000002, NFlag},
     {0xfffffffffffffffe, NFlag, 0xffffffffffffffff, NFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0x0000000000000000, ZCVFlag, 0x0000000000000001, CVFlag},
     {0x0000000000000001, CVFlag, 0x0000000000000002, CVFlag},
     {0x7ffffffffffffffe, CVFlag, 0x7fffffffffffffff, CVFlag},
     {0x7fffffffffffffff, CVFlag, 0x8000000000000000, NCFlag}},
    {{0x8000000000000001, NFlag, 0x8000000000000002, NFlag},
     {0x8000000000000002, NFlag, 0x8000000000000003, NFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0x0000000000000000, ZCFlag, 0x0000000000000001, CFlag},
     {0x0000000000000001, CVFlag, 0x0000000000000002, CVFlag},
     {0x0000000000000002, CVFlag, 0x0000000000000003, CVFlag},
     {0x7fffffffffffffff, CVFlag, 0x8000000000000000, NCFlag},
     {0x8000000000000000, NCFlag, 0x8000000000000001, NCFlag}},
    {{0xfffffffffffffffe, NFlag, 0xffffffffffffffff, NFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0x7ffffffffffffffc, CFlag, 0x7ffffffffffffffd, CFlag},
     {0x7ffffffffffffffd, CFlag, 0x7ffffffffffffffe, CFlag},
     {0x7ffffffffffffffe, CVFlag, 0x7fffffffffffffff, CVFlag},
     {0x7fffffffffffffff, CVFlag, 0x8000000000000000, NCFlag},
     {0xfffffffffffffffc, NCFlag, 0xfffffffffffffffd, NCFlag},
     {0xfffffffffffffffd, NCFlag, 0xfffffffffffffffe, NCFlag}},
    {{0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0x0000000000000000, ZCFlag, 0x0000000000000001, CFlag},
     {0x7ffffffffffffffd, CFlag, 0x7ffffffffffffffe, CFlag},
     {0x7ffffffffffffffe, CFlag, 0x7fffffffffffffff, CFlag},
     {0x7fffffffffffffff, CVFlag, 0x8000000000000000, NCFlag},
     {0x8000000000000000, NCFlag, 0x8000000000000001, NCFlag},
     {0xfffffffffffffffd, NCFlag, 0xfffffffffffffffe, NCFlag},
     {0xfffffffffffffffe, NCFlag, 0xffffffffffffffff, NCFlag}}
  };

  static const Expected expected_sbcs_x[input_count][input_count] = {
    {{0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0xfffffffffffffffe, NFlag, 0xffffffffffffffff, NFlag},
     {0x8000000000000001, NFlag, 0x8000000000000002, NFlag},
     {0x8000000000000000, NFlag, 0x8000000000000001, NFlag},
     {0x7fffffffffffffff, NoFlag, 0x8000000000000000, NVFlag},
     {0x7ffffffffffffffe, NoFlag, 0x7fffffffffffffff, NoFlag},
     {0x0000000000000001, NoFlag, 0x0000000000000002, NoFlag},
     {0x0000000000000000, ZFlag, 0x0000000000000001, NoFlag}},
    {{0x0000000000000000, ZCFlag, 0x0000000000000001, CFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0x8000000000000002, NFlag, 0x8000000000000003, NFlag},
     {0x8000000000000001, NFlag, 0x8000000000000002, NFlag},
     {0x8000000000000000, NVFlag, 0x8000000000000001, NVFlag},
     {0x7fffffffffffffff, NoFlag, 0x8000000000000000, NVFlag},
     {0x0000000000000002, NoFlag, 0x0000000000000003, NoFlag},
     {0x0000000000000001, NoFlag, 0x0000000000000002, NoFlag}},
    {{0x7ffffffffffffffd, CFlag, 0x7ffffffffffffffe, CFlag},
     {0x7ffffffffffffffc, CFlag, 0x7ffffffffffffffd, CFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0xfffffffffffffffe, NFlag, 0xffffffffffffffff, NFlag},
     {0xfffffffffffffffd, NVFlag, 0xfffffffffffffffe, NVFlag},
     {0xfffffffffffffffc, NVFlag, 0xfffffffffffffffd, NVFlag},
     {0x7fffffffffffffff, NoFlag, 0x8000000000000000, NVFlag},
     {0x7ffffffffffffffe, NoFlag, 0x7fffffffffffffff, NoFlag}},
    {{0x7ffffffffffffffe, CFlag, 0x7fffffffffffffff, CFlag},
     {0x7ffffffffffffffd, CFlag, 0x7ffffffffffffffe, CFlag},
     {0x0000000000000000, ZCFlag, 0x0000000000000001, CFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0xfffffffffffffffe, NVFlag, 0xffffffffffffffff, NVFlag},
     {0xfffffffffffffffd, NVFlag, 0xfffffffffffffffe, NVFlag},
     {0x8000000000000000, NVFlag, 0x8000000000000001, NVFlag},
     {0x7fffffffffffffff, NoFlag, 0x8000000000000000, NVFlag}},
    {{0x7fffffffffffffff, CVFlag, 0x8000000000000000, NCFlag},
     {0x7ffffffffffffffe, CVFlag, 0x7fffffffffffffff, CVFlag},
     {0x0000000000000001, CVFlag, 0x0000000000000002, CVFlag},
     {0x0000000000000000, ZCVFlag, 0x0000000000000001, CVFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0xfffffffffffffffe, NFlag, 0xffffffffffffffff, NFlag},
     {0x8000000000000001, NFlag, 0x8000000000000002, NFlag},
     {0x8000000000000000, NFlag, 0x8000000000000001, NFlag}},
    {{0x8000000000000000, NCFlag, 0x8000000000000001, NCFlag},
     {0x7fffffffffffffff, CVFlag, 0x8000000000000000, NCFlag},
     {0x0000000000000002, CVFlag, 0x0000000000000003, CVFlag},
     {0x0000000000000001, CVFlag, 0x0000000000000002, CVFlag},
     {0x0000000000000000, ZCFlag, 0x0000000000000001, CFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0x8000000000000002, NFlag, 0x8000000000000003, NFlag},
     {0x8000000000000001, NFlag, 0x8000000000000002, NFlag}},
    {{0xfffffffffffffffd, NCFlag, 0xfffffffffffffffe, NCFlag},
     {0xfffffffffffffffc, NCFlag, 0xfffffffffffffffd, NCFlag},
     {0x7fffffffffffffff, CVFlag, 0x8000000000000000, NCFlag},
     {0x7ffffffffffffffe, CVFlag, 0x7fffffffffffffff, CVFlag},
     {0x7ffffffffffffffd, CFlag, 0x7ffffffffffffffe, CFlag},
     {0x7ffffffffffffffc, CFlag, 0x7ffffffffffffffd, CFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag},
     {0xfffffffffffffffe, NFlag, 0xffffffffffffffff, NFlag}},
    {{0xfffffffffffffffe, NCFlag, 0xffffffffffffffff, NCFlag},
     {0xfffffffffffffffd, NCFlag, 0xfffffffffffffffe, NCFlag},
     {0x8000000000000000, NCFlag, 0x8000000000000001, NCFlag},
     {0x7fffffffffffffff, CVFlag, 0x8000000000000000, NCFlag},
     {0x7ffffffffffffffe, CFlag, 0x7fffffffffffffff, CFlag},
     {0x7ffffffffffffffd, CFlag, 0x7ffffffffffffffe, CFlag},
     {0x0000000000000000, ZCFlag, 0x0000000000000001, CFlag},
     {0xffffffffffffffff, NFlag, 0x0000000000000000, ZCFlag}}
  };

  for (size_t left = 0; left < input_count; left++) {
    for (size_t right = 0; right < input_count; right++) {
      const Expected & expected = expected_adcs_x[left][right];
      AdcsSbcsHelper(&MacroAssembler::Adcs, inputs[left], inputs[right], 0,
                     expected.carry0_result, expected.carry0_flags);
      AdcsSbcsHelper(&MacroAssembler::Adcs, inputs[left], inputs[right], 1,
                     expected.carry1_result, expected.carry1_flags);
    }
  }

  for (size_t left = 0; left < input_count; left++) {
    for (size_t right = 0; right < input_count; right++) {
      const Expected & expected = expected_sbcs_x[left][right];
      AdcsSbcsHelper(&MacroAssembler::Sbcs, inputs[left], inputs[right], 0,
                     expected.carry0_result, expected.carry0_flags);
      AdcsSbcsHelper(&MacroAssembler::Sbcs, inputs[left], inputs[right], 1,
                     expected.carry1_result, expected.carry1_flags);
    }
  }
}


TEST(adcs_sbcs_w) {
  uint32_t inputs[] = {
    0x00000000, 0x00000001, 0x7ffffffe, 0x7fffffff,
    0x80000000, 0x80000001, 0xfffffffe, 0xffffffff,
  };
  static const size_t input_count = sizeof(inputs) / sizeof(inputs[0]);

  struct Expected {
    uint32_t carry0_result;
    StatusFlags carry0_flags;
    uint32_t carry1_result;
    StatusFlags carry1_flags;
  };

  static const Expected expected_adcs_w[input_count][input_count] = {
    {{0x00000000, ZFlag, 0x00000001, NoFlag},
     {0x00000001, NoFlag, 0x00000002, NoFlag},
     {0x7ffffffe, NoFlag, 0x7fffffff, NoFlag},
     {0x7fffffff, NoFlag, 0x80000000, NVFlag},
     {0x80000000, NFlag, 0x80000001, NFlag},
     {0x80000001, NFlag, 0x80000002, NFlag},
     {0xfffffffe, NFlag, 0xffffffff, NFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag}},
    {{0x00000001, NoFlag, 0x00000002, NoFlag},
     {0x00000002, NoFlag, 0x00000003, NoFlag},
     {0x7fffffff, NoFlag, 0x80000000, NVFlag},
     {0x80000000, NVFlag, 0x80000001, NVFlag},
     {0x80000001, NFlag, 0x80000002, NFlag},
     {0x80000002, NFlag, 0x80000003, NFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0x00000000, ZCFlag, 0x00000001, CFlag}},
    {{0x7ffffffe, NoFlag, 0x7fffffff, NoFlag},
     {0x7fffffff, NoFlag, 0x80000000, NVFlag},
     {0xfffffffc, NVFlag, 0xfffffffd, NVFlag},
     {0xfffffffd, NVFlag, 0xfffffffe, NVFlag},
     {0xfffffffe, NFlag, 0xffffffff, NFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0x7ffffffc, CFlag, 0x7ffffffd, CFlag},
     {0x7ffffffd, CFlag, 0x7ffffffe, CFlag}},
    {{0x7fffffff, NoFlag, 0x80000000, NVFlag},
     {0x80000000, NVFlag, 0x80000001, NVFlag},
     {0xfffffffd, NVFlag, 0xfffffffe, NVFlag},
     {0xfffffffe, NVFlag, 0xffffffff, NVFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0x00000000, ZCFlag, 0x00000001, CFlag},
     {0x7ffffffd, CFlag, 0x7ffffffe, CFlag},
     {0x7ffffffe, CFlag, 0x7fffffff, CFlag}},
    {{0x80000000, NFlag, 0x80000001, NFlag},
     {0x80000001, NFlag, 0x80000002, NFlag},
     {0xfffffffe, NFlag, 0xffffffff, NFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0x00000000, ZCVFlag, 0x00000001, CVFlag},
     {0x00000001, CVFlag, 0x00000002, CVFlag},
     {0x7ffffffe, CVFlag, 0x7fffffff, CVFlag},
     {0x7fffffff, CVFlag, 0x80000000, NCFlag}},
    {{0x80000001, NFlag, 0x80000002, NFlag},
     {0x80000002, NFlag, 0x80000003, NFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0x00000000, ZCFlag, 0x00000001, CFlag},
     {0x00000001, CVFlag, 0x00000002, CVFlag},
     {0x00000002, CVFlag, 0x00000003, CVFlag},
     {0x7fffffff, CVFlag, 0x80000000, NCFlag},
     {0x80000000, NCFlag, 0x80000001, NCFlag}},
    {{0xfffffffe, NFlag, 0xffffffff, NFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0x7ffffffc, CFlag, 0x7ffffffd, CFlag},
     {0x7ffffffd, CFlag, 0x7ffffffe, CFlag},
     {0x7ffffffe, CVFlag, 0x7fffffff, CVFlag},
     {0x7fffffff, CVFlag, 0x80000000, NCFlag},
     {0xfffffffc, NCFlag, 0xfffffffd, NCFlag},
     {0xfffffffd, NCFlag, 0xfffffffe, NCFlag}},
    {{0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0x00000000, ZCFlag, 0x00000001, CFlag},
     {0x7ffffffd, CFlag, 0x7ffffffe, CFlag},
     {0x7ffffffe, CFlag, 0x7fffffff, CFlag},
     {0x7fffffff, CVFlag, 0x80000000, NCFlag},
     {0x80000000, NCFlag, 0x80000001, NCFlag},
     {0xfffffffd, NCFlag, 0xfffffffe, NCFlag},
     {0xfffffffe, NCFlag, 0xffffffff, NCFlag}}
  };

  static const Expected expected_sbcs_w[input_count][input_count] = {
    {{0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0xfffffffe, NFlag, 0xffffffff, NFlag},
     {0x80000001, NFlag, 0x80000002, NFlag},
     {0x80000000, NFlag, 0x80000001, NFlag},
     {0x7fffffff, NoFlag, 0x80000000, NVFlag},
     {0x7ffffffe, NoFlag, 0x7fffffff, NoFlag},
     {0x00000001, NoFlag, 0x00000002, NoFlag},
     {0x00000000, ZFlag, 0x00000001, NoFlag}},
    {{0x00000000, ZCFlag, 0x00000001, CFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0x80000002, NFlag, 0x80000003, NFlag},
     {0x80000001, NFlag, 0x80000002, NFlag},
     {0x80000000, NVFlag, 0x80000001, NVFlag},
     {0x7fffffff, NoFlag, 0x80000000, NVFlag},
     {0x00000002, NoFlag, 0x00000003, NoFlag},
     {0x00000001, NoFlag, 0x00000002, NoFlag}},
    {{0x7ffffffd, CFlag, 0x7ffffffe, CFlag},
     {0x7ffffffc, CFlag, 0x7ffffffd, CFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0xfffffffe, NFlag, 0xffffffff, NFlag},
     {0xfffffffd, NVFlag, 0xfffffffe, NVFlag},
     {0xfffffffc, NVFlag, 0xfffffffd, NVFlag},
     {0x7fffffff, NoFlag, 0x80000000, NVFlag},
     {0x7ffffffe, NoFlag, 0x7fffffff, NoFlag}},
    {{0x7ffffffe, CFlag, 0x7fffffff, CFlag},
     {0x7ffffffd, CFlag, 0x7ffffffe, CFlag},
     {0x00000000, ZCFlag, 0x00000001, CFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0xfffffffe, NVFlag, 0xffffffff, NVFlag},
     {0xfffffffd, NVFlag, 0xfffffffe, NVFlag},
     {0x80000000, NVFlag, 0x80000001, NVFlag},
     {0x7fffffff, NoFlag, 0x80000000, NVFlag}},
    {{0x7fffffff, CVFlag, 0x80000000, NCFlag},
     {0x7ffffffe, CVFlag, 0x7fffffff, CVFlag},
     {0x00000001, CVFlag, 0x00000002, CVFlag},
     {0x00000000, ZCVFlag, 0x00000001, CVFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0xfffffffe, NFlag, 0xffffffff, NFlag},
     {0x80000001, NFlag, 0x80000002, NFlag},
     {0x80000000, NFlag, 0x80000001, NFlag}},
    {{0x80000000, NCFlag, 0x80000001, NCFlag},
     {0x7fffffff, CVFlag, 0x80000000, NCFlag},
     {0x00000002, CVFlag, 0x00000003, CVFlag},
     {0x00000001, CVFlag, 0x00000002, CVFlag},
     {0x00000000, ZCFlag, 0x00000001, CFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0x80000002, NFlag, 0x80000003, NFlag},
     {0x80000001, NFlag, 0x80000002, NFlag}},
    {{0xfffffffd, NCFlag, 0xfffffffe, NCFlag},
     {0xfffffffc, NCFlag, 0xfffffffd, NCFlag},
     {0x7fffffff, CVFlag, 0x80000000, NCFlag},
     {0x7ffffffe, CVFlag, 0x7fffffff, CVFlag},
     {0x7ffffffd, CFlag, 0x7ffffffe, CFlag},
     {0x7ffffffc, CFlag, 0x7ffffffd, CFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag},
     {0xfffffffe, NFlag, 0xffffffff, NFlag}},
    {{0xfffffffe, NCFlag, 0xffffffff, NCFlag},
     {0xfffffffd, NCFlag, 0xfffffffe, NCFlag},
     {0x80000000, NCFlag, 0x80000001, NCFlag},
     {0x7fffffff, CVFlag, 0x80000000, NCFlag},
     {0x7ffffffe, CFlag, 0x7fffffff, CFlag},
     {0x7ffffffd, CFlag, 0x7ffffffe, CFlag},
     {0x00000000, ZCFlag, 0x00000001, CFlag},
     {0xffffffff, NFlag, 0x00000000, ZCFlag}}
  };

  for (size_t left = 0; left < input_count; left++) {
    for (size_t right = 0; right < input_count; right++) {
      const Expected & expected = expected_adcs_w[left][right];
      AdcsSbcsHelper(&MacroAssembler::Adcs, inputs[left], inputs[right], 0,
                     expected.carry0_result, expected.carry0_flags);
      AdcsSbcsHelper(&MacroAssembler::Adcs, inputs[left], inputs[right], 1,
                     expected.carry1_result, expected.carry1_flags);
    }
  }

  for (size_t left = 0; left < input_count; left++) {
    for (size_t right = 0; right < input_count; right++) {
      const Expected & expected = expected_sbcs_w[left][right];
      AdcsSbcsHelper(&MacroAssembler::Sbcs, inputs[left], inputs[right], 0,
                     expected.carry0_result, expected.carry0_flags);
      AdcsSbcsHelper(&MacroAssembler::Sbcs, inputs[left], inputs[right], 1,
                     expected.carry1_result, expected.carry1_flags);
    }
  }
}


TEST(adc_sbc_shift) {
  SETUP();

  START();
  __ Mov(x0, 0);
  __ Mov(x1, 1);
  __ Mov(x2, 0x0123456789abcdef);
  __ Mov(x3, 0xfedcba9876543210);
  __ Mov(x4, 0xffffffffffffffff);

  // Clear the C flag.
  __ Adds(x0, x0, Operand(0));

  __ Adc(x5, x2, Operand(x3));
  __ Adc(x6, x0, Operand(x1, LSL, 60));
  __ Sbc(x7, x4, Operand(x3, LSR, 4));
  __ Adc(x8, x2, Operand(x3, ASR, 4));
  __ Adc(x9, x2, Operand(x3, ROR, 8));

  __ Adc(w10, w2, Operand(w3));
  __ Adc(w11, w0, Operand(w1, LSL, 30));
  __ Sbc(w12, w4, Operand(w3, LSR, 4));
  __ Adc(w13, w2, Operand(w3, ASR, 4));
  __ Adc(w14, w2, Operand(w3, ROR, 8));

  // Set the C flag.
  __ Cmp(w0, Operand(w0));

  __ Adc(x18, x2, Operand(x3));
  __ Adc(x19, x0, Operand(x1, LSL, 60));
  __ Sbc(x20, x4, Operand(x3, LSR, 4));
  __ Adc(x21, x2, Operand(x3, ASR, 4));
  __ Adc(x22, x2, Operand(x3, ROR, 8));

  __ Adc(w23, w2, Operand(w3));
  __ Adc(w24, w0, Operand(w1, LSL, 30));
  __ Sbc(w25, w4, Operand(w3, LSR, 4));
  __ Adc(w26, w2, Operand(w3, ASR, 4));
  __ Adc(w27, w2, Operand(w3, ROR, 8));
  END();

  RUN();

  ASSERT_EQUAL_64(0xffffffffffffffff, x5);
  ASSERT_EQUAL_64(INT64_C(1) << 60, x6);
  ASSERT_EQUAL_64(0xf0123456789abcdd, x7);
  ASSERT_EQUAL_64(0x0111111111111110, x8);
  ASSERT_EQUAL_64(0x1222222222222221, x9);

  ASSERT_EQUAL_32(0xffffffff, w10);
  ASSERT_EQUAL_32(INT32_C(1) << 30, w11);
  ASSERT_EQUAL_32(0xf89abcdd, w12);
  ASSERT_EQUAL_32(0x91111110, w13);
  ASSERT_EQUAL_32(0x9a222221, w14);

  ASSERT_EQUAL_64(0xffffffffffffffff + 1, x18);
  ASSERT_EQUAL_64((INT64_C(1) << 60) + 1, x19);
  ASSERT_EQUAL_64(0xf0123456789abcdd + 1, x20);
  ASSERT_EQUAL_64(0x0111111111111110 + 1, x21);
  ASSERT_EQUAL_64(0x1222222222222221 + 1, x22);

  ASSERT_EQUAL_32(0xffffffff + 1, w23);
  ASSERT_EQUAL_32((INT32_C(1) << 30) + 1, w24);
  ASSERT_EQUAL_32(0xf89abcdd + 1, w25);
  ASSERT_EQUAL_32(0x91111110 + 1, w26);
  ASSERT_EQUAL_32(0x9a222221 + 1, w27);

  TEARDOWN();
}


TEST(adc_sbc_extend) {
  SETUP();

  START();
  // Clear the C flag.
  __ Adds(x0, x0, Operand(0));

  __ Mov(x0, 0);
  __ Mov(x1, 1);
  __ Mov(x2, 0x0123456789abcdef);

  __ Adc(x10, x1, Operand(w2, UXTB, 1));
  __ Adc(x11, x1, Operand(x2, SXTH, 2));
  __ Sbc(x12, x1, Operand(w2, UXTW, 4));
  __ Adc(x13, x1, Operand(x2, UXTX, 4));

  __ Adc(w14, w1, Operand(w2, UXTB, 1));
  __ Adc(w15, w1, Operand(w2, SXTH, 2));
  __ Adc(w9, w1, Operand(w2, UXTW, 4));

  // Set the C flag.
  __ Cmp(w0, Operand(w0));

  __ Adc(x20, x1, Operand(w2, UXTB, 1));
  __ Adc(x21, x1, Operand(x2, SXTH, 2));
  __ Sbc(x22, x1, Operand(w2, UXTW, 4));
  __ Adc(x23, x1, Operand(x2, UXTX, 4));

  __ Adc(w24, w1, Operand(w2, UXTB, 1));
  __ Adc(w25, w1, Operand(w2, SXTH, 2));
  __ Adc(w26, w1, Operand(w2, UXTW, 4));
  END();

  RUN();

  ASSERT_EQUAL_64(0x1df, x10);
  ASSERT_EQUAL_64(0xffffffffffff37bd, x11);
  ASSERT_EQUAL_64(0xfffffff765432110, x12);
  ASSERT_EQUAL_64(0x123456789abcdef1, x13);

  ASSERT_EQUAL_32(0x1df, w14);
  ASSERT_EQUAL_32(0xffff37bd, w15);
  ASSERT_EQUAL_32(0x9abcdef1, w9);

  ASSERT_EQUAL_64(0x1df + 1, x20);
  ASSERT_EQUAL_64(0xffffffffffff37bd + 1, x21);
  ASSERT_EQUAL_64(0xfffffff765432110 + 1, x22);
  ASSERT_EQUAL_64(0x123456789abcdef1 + 1, x23);

  ASSERT_EQUAL_32(0x1df + 1, w24);
  ASSERT_EQUAL_32(0xffff37bd + 1, w25);
  ASSERT_EQUAL_32(0x9abcdef1 + 1, w26);

  // Check that adc correctly sets the condition flags.
  START();
  __ Mov(x0, 0xff);
  __ Mov(x1, 0xffffffffffffffff);
  // Clear the C flag.
  __ Adds(x0, x0, Operand(0));
  __ Adcs(x10, x0, Operand(x1, SXTX, 1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(CFlag);

  START();
  __ Mov(x0, 0x7fffffffffffffff);
  __ Mov(x1, 1);
  // Clear the C flag.
  __ Adds(x0, x0, Operand(0));
  __ Adcs(x10, x0, Operand(x1, UXTB, 2));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NVFlag);

  START();
  __ Mov(x0, 0x7fffffffffffffff);
  // Clear the C flag.
  __ Adds(x0, x0, Operand(0));
  __ Adcs(x10, x0, Operand(1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NVFlag);

  TEARDOWN();
}


TEST(adc_sbc_wide_imm) {
  SETUP();

  START();
  __ Mov(x0, 0);

  // Clear the C flag.
  __ Adds(x0, x0, Operand(0));

  __ Adc(x7, x0, Operand(0x1234567890abcdef));
  __ Adc(w8, w0, Operand(0xffffffff));
  __ Sbc(x9, x0, Operand(0x1234567890abcdef));
  __ Sbc(w10, w0, Operand(0xffffffff));
  __ Ngc(x11, Operand(0xffffffff00000000));
  __ Ngc(w12, Operand(0xffff0000));

  // Set the C flag.
  __ Cmp(w0, Operand(w0));

  __ Adc(x18, x0, Operand(0x1234567890abcdef));
  __ Adc(w19, w0, Operand(0xffffffff));
  __ Sbc(x20, x0, Operand(0x1234567890abcdef));
  __ Sbc(w21, w0, Operand(0xffffffff));
  __ Ngc(x22, Operand(0xffffffff00000000));
  __ Ngc(w23, Operand(0xffff0000));
  END();

  RUN();

  ASSERT_EQUAL_64(0x1234567890abcdef, x7);
  ASSERT_EQUAL_64(0xffffffff, x8);
  ASSERT_EQUAL_64(0xedcba9876f543210, x9);
  ASSERT_EQUAL_64(0, x10);
  ASSERT_EQUAL_64(0xffffffff, x11);
  ASSERT_EQUAL_64(0xffff, x12);

  ASSERT_EQUAL_64(0x1234567890abcdef + 1, x18);
  ASSERT_EQUAL_64(0, x19);
  ASSERT_EQUAL_64(0xedcba9876f543211, x20);
  ASSERT_EQUAL_64(1, x21);
  ASSERT_EQUAL_64(0x0000000100000000, x22);
  ASSERT_EQUAL_64(0x0000000000010000, x23);

  TEARDOWN();
}

TEST(flags) {
  SETUP();

  START();
  __ Mov(x0, 0);
  __ Mov(x1, 0x1111111111111111);
  __ Neg(x10, Operand(x0));
  __ Neg(x11, Operand(x1));
  __ Neg(w12, Operand(w1));
  // Clear the C flag.
  __ Adds(x0, x0, Operand(0));
  __ Ngc(x13, Operand(x0));
  // Set the C flag.
  __ Cmp(x0, Operand(x0));
  __ Ngc(w14, Operand(w0));
  END();

  RUN();

  ASSERT_EQUAL_64(0, x10);
  ASSERT_EQUAL_64(-0x1111111111111111, x11);
  ASSERT_EQUAL_32(-0x11111111, w12);
  ASSERT_EQUAL_64(-1, x13);
  ASSERT_EQUAL_32(0, w14);

  START();
  __ Mov(x0, 0);
  __ Cmp(x0, Operand(x0));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);

  START();
  __ Mov(w0, 0);
  __ Cmp(w0, Operand(w0));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);

  START();
  __ Mov(x0, 0);
  __ Mov(x1, 0x1111111111111111);
  __ Cmp(x0, Operand(x1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NFlag);

  START();
  __ Mov(w0, 0);
  __ Mov(w1, 0x11111111);
  __ Cmp(w0, Operand(w1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NFlag);

  START();
  __ Mov(x1, 0x1111111111111111);
  __ Cmp(x1, Operand(0));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(CFlag);

  START();
  __ Mov(w1, 0x11111111);
  __ Cmp(w1, Operand(0));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(CFlag);

  START();
  __ Mov(x0, 1);
  __ Mov(x1, 0x7fffffffffffffff);
  __ Cmn(x1, Operand(x0));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NVFlag);

  START();
  __ Mov(w0, 1);
  __ Mov(w1, 0x7fffffff);
  __ Cmn(w1, Operand(w0));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NVFlag);

  START();
  __ Mov(x0, 1);
  __ Mov(x1, 0xffffffffffffffff);
  __ Cmn(x1, Operand(x0));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);

  START();
  __ Mov(w0, 1);
  __ Mov(w1, 0xffffffff);
  __ Cmn(w1, Operand(w0));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);

  START();
  __ Mov(w0, 0);
  __ Mov(w1, 1);
  // Clear the C flag.
  __ Adds(w0, w0, Operand(0));
  __ Ngcs(w0, Operand(w1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(NFlag);

  START();
  __ Mov(w0, 0);
  __ Mov(w1, 0);
  // Set the C flag.
  __ Cmp(w0, Operand(w0));
  __ Ngcs(w0, Operand(w1));
  END();

  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);

  TEARDOWN();
}


TEST(cmp_shift) {
  SETUP();

  START();
  __ Mov(x18, 0xf0000000);
  __ Mov(x19, 0xf000000010000000);
  __ Mov(x20, 0xf0000000f0000000);
  __ Mov(x21, 0x7800000078000000);
  __ Mov(x22, 0x3c0000003c000000);
  __ Mov(x23, 0x8000000780000000);
  __ Mov(x24, 0x0000000f00000000);
  __ Mov(x25, 0x00000003c0000000);
  __ Mov(x26, 0x8000000780000000);
  __ Mov(x27, 0xc0000003);

  __ Cmp(w20, Operand(w21, LSL, 1));
  __ Mrs(x0, NZCV);

  __ Cmp(x20, Operand(x22, LSL, 2));
  __ Mrs(x1, NZCV);

  __ Cmp(w19, Operand(w23, LSR, 3));
  __ Mrs(x2, NZCV);

  __ Cmp(x18, Operand(x24, LSR, 4));
  __ Mrs(x3, NZCV);

  __ Cmp(w20, Operand(w25, ASR, 2));
  __ Mrs(x4, NZCV);

  __ Cmp(x20, Operand(x26, ASR, 3));
  __ Mrs(x5, NZCV);

  __ Cmp(w27, Operand(w22, ROR, 28));
  __ Mrs(x6, NZCV);

  __ Cmp(x20, Operand(x21, ROR, 31));
  __ Mrs(x7, NZCV);
  END();

  RUN();

  ASSERT_EQUAL_32(ZCFlag, w0);
  ASSERT_EQUAL_32(ZCFlag, w1);
  ASSERT_EQUAL_32(ZCFlag, w2);
  ASSERT_EQUAL_32(ZCFlag, w3);
  ASSERT_EQUAL_32(ZCFlag, w4);
  ASSERT_EQUAL_32(ZCFlag, w5);
  ASSERT_EQUAL_32(ZCFlag, w6);
  ASSERT_EQUAL_32(ZCFlag, w7);

  TEARDOWN();
}


TEST(cmp_extend) {
  SETUP();

  START();
  __ Mov(w20, 0x2);
  __ Mov(w21, 0x1);
  __ Mov(x22, 0xffffffffffffffff);
  __ Mov(x23, 0xff);
  __ Mov(x24, 0xfffffffffffffffe);
  __ Mov(x25, 0xffff);
  __ Mov(x26, 0xffffffff);

  __ Cmp(w20, Operand(w21, LSL, 1));
  __ Mrs(x0, NZCV);

  __ Cmp(x22, Operand(x23, SXTB, 0));
  __ Mrs(x1, NZCV);

  __ Cmp(x24, Operand(x23, SXTB, 1));
  __ Mrs(x2, NZCV);

  __ Cmp(x24, Operand(x23, UXTB, 1));
  __ Mrs(x3, NZCV);

  __ Cmp(w22, Operand(w25, UXTH));
  __ Mrs(x4, NZCV);

  __ Cmp(x22, Operand(x25, SXTH));
  __ Mrs(x5, NZCV);

  __ Cmp(x22, Operand(x26, UXTW));
  __ Mrs(x6, NZCV);

  __ Cmp(x24, Operand(x26, SXTW, 1));
  __ Mrs(x7, NZCV);
  END();

  RUN();

  ASSERT_EQUAL_32(ZCFlag, w0);
  ASSERT_EQUAL_32(ZCFlag, w1);
  ASSERT_EQUAL_32(ZCFlag, w2);
  ASSERT_EQUAL_32(NCFlag, w3);
  ASSERT_EQUAL_32(NCFlag, w4);
  ASSERT_EQUAL_32(ZCFlag, w5);
  ASSERT_EQUAL_32(NCFlag, w6);
  ASSERT_EQUAL_32(ZCFlag, w7);

  TEARDOWN();
}


TEST(ccmp) {
  SETUP();
  ALLOW_ASM();

  START();
  __ Mov(w16, 0);
  __ Mov(w17, 1);
  __ Cmp(w16, w16);
  __ Ccmp(w16, w17, NCFlag, eq);
  __ Mrs(x0, NZCV);

  __ Cmp(w16, w16);
  __ Ccmp(w16, w17, NCFlag, ne);
  __ Mrs(x1, NZCV);

  __ Cmp(x16, x16);
  __ Ccmn(x16, 2, NZCVFlag, eq);
  __ Mrs(x2, NZCV);

  __ Cmp(x16, x16);
  __ Ccmn(x16, 2, NZCVFlag, ne);
  __ Mrs(x3, NZCV);

  // The MacroAssembler does not allow al as a condition.
  __ ccmp(x16, x16, NZCVFlag, al);
  __ Mrs(x4, NZCV);

  // The MacroAssembler does not allow nv as a condition.
  __ ccmp(x16, x16, NZCVFlag, nv);
  __ Mrs(x5, NZCV);

  END();

  RUN();

  ASSERT_EQUAL_32(NFlag, w0);
  ASSERT_EQUAL_32(NCFlag, w1);
  ASSERT_EQUAL_32(NoFlag, w2);
  ASSERT_EQUAL_32(NZCVFlag, w3);
  ASSERT_EQUAL_32(ZCFlag, w4);
  ASSERT_EQUAL_32(ZCFlag, w5);

  TEARDOWN();
}


TEST(ccmp_wide_imm) {
  SETUP();

  START();
  __ Mov(w20, 0);

  __ Cmp(w20, Operand(w20));
  __ Ccmp(w20, Operand(0x12345678), NZCVFlag, eq);
  __ Mrs(x0, NZCV);

  __ Cmp(w20, Operand(w20));
  __ Ccmp(x20, Operand(0xffffffffffffffff), NZCVFlag, eq);
  __ Mrs(x1, NZCV);
  END();

  RUN();

  ASSERT_EQUAL_32(NFlag, w0);
  ASSERT_EQUAL_32(NoFlag, w1);

  TEARDOWN();
}


TEST(ccmp_shift_extend) {
  SETUP();

  START();
  __ Mov(w20, 0x2);
  __ Mov(w21, 0x1);
  __ Mov(x22, 0xffffffffffffffff);
  __ Mov(x23, 0xff);
  __ Mov(x24, 0xfffffffffffffffe);

  __ Cmp(w20, Operand(w20));
  __ Ccmp(w20, Operand(w21, LSL, 1), NZCVFlag, eq);
  __ Mrs(x0, NZCV);

  __ Cmp(w20, Operand(w20));
  __ Ccmp(x22, Operand(x23, SXTB, 0), NZCVFlag, eq);
  __ Mrs(x1, NZCV);

  __ Cmp(w20, Operand(w20));
  __ Ccmp(x24, Operand(x23, SXTB, 1), NZCVFlag, eq);
  __ Mrs(x2, NZCV);

  __ Cmp(w20, Operand(w20));
  __ Ccmp(x24, Operand(x23, UXTB, 1), NZCVFlag, eq);
  __ Mrs(x3, NZCV);

  __ Cmp(w20, Operand(w20));
  __ Ccmp(x24, Operand(x23, UXTB, 1), NZCVFlag, ne);
  __ Mrs(x4, NZCV);
  END();

  RUN();

  ASSERT_EQUAL_32(ZCFlag, w0);
  ASSERT_EQUAL_32(ZCFlag, w1);
  ASSERT_EQUAL_32(ZCFlag, w2);
  ASSERT_EQUAL_32(NCFlag, w3);
  ASSERT_EQUAL_32(NZCVFlag, w4);

  TEARDOWN();
}


TEST(csel) {
  SETUP();
  ALLOW_ASM();

  START();
  __ Mov(x16, 0);
  __ Mov(x24, 0x0000000f0000000f);
  __ Mov(x25, 0x0000001f0000001f);

  __ Cmp(w16, Operand(0));
  __ Csel(w0, w24, w25, eq);
  __ Csel(w1, w24, w25, ne);
  __ Csinc(w2, w24, w25, mi);
  __ Csinc(w3, w24, w25, pl);

  // The MacroAssembler does not allow al or nv as a condition.
  __ csel(w13, w24, w25, al);
  __ csel(x14, x24, x25, nv);

  __ Cmp(x16, Operand(1));
  __ Csinv(x4, x24, x25, gt);
  __ Csinv(x5, x24, x25, le);
  __ Csneg(x6, x24, x25, hs);
  __ Csneg(x7, x24, x25, lo);

  __ Cset(w8, ne);
  __ Csetm(w9, ne);
  __ Cinc(x10, x25, ne);
  __ Cinv(x11, x24, ne);
  __ Cneg(x12, x24, ne);

  // The MacroAssembler does not allow al or nv as a condition.
  __ csel(w15, w24, w25, al);
  __ csel(x17, x24, x25, nv);

  END();

  RUN();

  ASSERT_EQUAL_64(0x0000000f, x0);
  ASSERT_EQUAL_64(0x0000001f, x1);
  ASSERT_EQUAL_64(0x00000020, x2);
  ASSERT_EQUAL_64(0x0000000f, x3);
  ASSERT_EQUAL_64(0xffffffe0ffffffe0, x4);
  ASSERT_EQUAL_64(0x0000000f0000000f, x5);
  ASSERT_EQUAL_64(0xffffffe0ffffffe1, x6);
  ASSERT_EQUAL_64(0x0000000f0000000f, x7);
  ASSERT_EQUAL_64(0x00000001, x8);
  ASSERT_EQUAL_64(0xffffffff, x9);
  ASSERT_EQUAL_64(0x0000001f00000020, x10);
  ASSERT_EQUAL_64(0xfffffff0fffffff0, x11);
  ASSERT_EQUAL_64(0xfffffff0fffffff1, x12);
  ASSERT_EQUAL_64(0x0000000f, x13);
  ASSERT_EQUAL_64(0x0000000f0000000f, x14);
  ASSERT_EQUAL_64(0x0000000f, x15);
  ASSERT_EQUAL_64(0x0000000f0000000f, x17);

  TEARDOWN();
}


TEST(csel_imm) {
  SETUP();

  START();
  __ Mov(x18, 0);
  __ Mov(x19, 0x80000000);
  __ Mov(x20, 0x8000000000000000);

  __ Cmp(x18, Operand(0));
  __ Csel(w0, w19, -2, ne);
  __ Csel(w1, w19, -1, ne);
  __ Csel(w2, w19, 0, ne);
  __ Csel(w3, w19, 1, ne);
  __ Csel(w4, w19, 2, ne);
  __ Csel(w5, w19, Operand(w19, ASR, 31), ne);
  __ Csel(w6, w19, Operand(w19, ROR, 1), ne);
  __ Csel(w7, w19, 3, eq);

  __ Csel(x8, x20, -2, ne);
  __ Csel(x9, x20, -1, ne);
  __ Csel(x10, x20, 0, ne);
  __ Csel(x11, x20, 1, ne);
  __ Csel(x12, x20, 2, ne);
  __ Csel(x13, x20, Operand(x20, ASR, 63), ne);
  __ Csel(x14, x20, Operand(x20, ROR, 1), ne);
  __ Csel(x15, x20, 3, eq);

  END();

  RUN();

  ASSERT_EQUAL_32(-2, w0);
  ASSERT_EQUAL_32(-1, w1);
  ASSERT_EQUAL_32(0, w2);
  ASSERT_EQUAL_32(1, w3);
  ASSERT_EQUAL_32(2, w4);
  ASSERT_EQUAL_32(-1, w5);
  ASSERT_EQUAL_32(0x40000000, w6);
  ASSERT_EQUAL_32(0x80000000, w7);

  ASSERT_EQUAL_64(-2, x8);
  ASSERT_EQUAL_64(-1, x9);
  ASSERT_EQUAL_64(0, x10);
  ASSERT_EQUAL_64(1, x11);
  ASSERT_EQUAL_64(2, x12);
  ASSERT_EQUAL_64(-1, x13);
  ASSERT_EQUAL_64(0x4000000000000000, x14);
  ASSERT_EQUAL_64(0x8000000000000000, x15);

  TEARDOWN();
}


TEST(lslv) {
  SETUP();
  ALLOW_ASM();

  uint64_t value = 0x0123456789abcdef;
  int shift[] = {1, 3, 5, 9, 17, 33};

  START();
  __ Mov(x0, value);
  __ Mov(w1, shift[0]);
  __ Mov(w2, shift[1]);
  __ Mov(w3, shift[2]);
  __ Mov(w4, shift[3]);
  __ Mov(w5, shift[4]);
  __ Mov(w6, shift[5]);

  // The MacroAssembler does not allow zr as an argument.
  __ lslv(x0, x0, xzr);

  __ Lsl(x16, x0, x1);
  __ Lsl(x17, x0, x2);
  __ Lsl(x18, x0, x3);
  __ Lsl(x19, x0, x4);
  __ Lsl(x20, x0, x5);
  __ Lsl(x21, x0, x6);

  __ Lsl(w22, w0, w1);
  __ Lsl(w23, w0, w2);
  __ Lsl(w24, w0, w3);
  __ Lsl(w25, w0, w4);
  __ Lsl(w26, w0, w5);
  __ Lsl(w27, w0, w6);
  END();

  RUN();

  ASSERT_EQUAL_64(value, x0);
  ASSERT_EQUAL_64(value << (shift[0] & 63), x16);
  ASSERT_EQUAL_64(value << (shift[1] & 63), x17);
  ASSERT_EQUAL_64(value << (shift[2] & 63), x18);
  ASSERT_EQUAL_64(value << (shift[3] & 63), x19);
  ASSERT_EQUAL_64(value << (shift[4] & 63), x20);
  ASSERT_EQUAL_64(value << (shift[5] & 63), x21);
  ASSERT_EQUAL_32(value << (shift[0] & 31), w22);
  ASSERT_EQUAL_32(value << (shift[1] & 31), w23);
  ASSERT_EQUAL_32(value << (shift[2] & 31), w24);
  ASSERT_EQUAL_32(value << (shift[3] & 31), w25);
  ASSERT_EQUAL_32(value << (shift[4] & 31), w26);
  ASSERT_EQUAL_32(value << (shift[5] & 31), w27);

  TEARDOWN();
}


TEST(lsrv) {
  SETUP();
  ALLOW_ASM();

  uint64_t value = 0x0123456789abcdef;
  int shift[] = {1, 3, 5, 9, 17, 33};

  START();
  __ Mov(x0, value);
  __ Mov(w1, shift[0]);
  __ Mov(w2, shift[1]);
  __ Mov(w3, shift[2]);
  __ Mov(w4, shift[3]);
  __ Mov(w5, shift[4]);
  __ Mov(w6, shift[5]);

  // The MacroAssembler does not allow zr as an argument.
  __ lsrv(x0, x0, xzr);

  __ Lsr(x16, x0, x1);
  __ Lsr(x17, x0, x2);
  __ Lsr(x18, x0, x3);
  __ Lsr(x19, x0, x4);
  __ Lsr(x20, x0, x5);
  __ Lsr(x21, x0, x6);

  __ Lsr(w22, w0, w1);
  __ Lsr(w23, w0, w2);
  __ Lsr(w24, w0, w3);
  __ Lsr(w25, w0, w4);
  __ Lsr(w26, w0, w5);
  __ Lsr(w27, w0, w6);
  END();

  RUN();

  ASSERT_EQUAL_64(value, x0);
  ASSERT_EQUAL_64(value >> (shift[0] & 63), x16);
  ASSERT_EQUAL_64(value >> (shift[1] & 63), x17);
  ASSERT_EQUAL_64(value >> (shift[2] & 63), x18);
  ASSERT_EQUAL_64(value >> (shift[3] & 63), x19);
  ASSERT_EQUAL_64(value >> (shift[4] & 63), x20);
  ASSERT_EQUAL_64(value >> (shift[5] & 63), x21);

  value &= 0xffffffff;
  ASSERT_EQUAL_32(value >> (shift[0] & 31), w22);
  ASSERT_EQUAL_32(value >> (shift[1] & 31), w23);
  ASSERT_EQUAL_32(value >> (shift[2] & 31), w24);
  ASSERT_EQUAL_32(value >> (shift[3] & 31), w25);
  ASSERT_EQUAL_32(value >> (shift[4] & 31), w26);
  ASSERT_EQUAL_32(value >> (shift[5] & 31), w27);

  TEARDOWN();
}


TEST(asrv) {
  SETUP();
  ALLOW_ASM();

  int64_t value = 0xfedcba98fedcba98;
  int shift[] = {1, 3, 5, 9, 17, 33};

  START();
  __ Mov(x0, value);
  __ Mov(w1, shift[0]);
  __ Mov(w2, shift[1]);
  __ Mov(w3, shift[2]);
  __ Mov(w4, shift[3]);
  __ Mov(w5, shift[4]);
  __ Mov(w6, shift[5]);

  // The MacroAssembler does not allow zr as an argument.
  __ asrv(x0, x0, xzr);

  __ Asr(x16, x0, x1);
  __ Asr(x17, x0, x2);
  __ Asr(x18, x0, x3);
  __ Asr(x19, x0, x4);
  __ Asr(x20, x0, x5);
  __ Asr(x21, x0, x6);

  __ Asr(w22, w0, w1);
  __ Asr(w23, w0, w2);
  __ Asr(w24, w0, w3);
  __ Asr(w25, w0, w4);
  __ Asr(w26, w0, w5);
  __ Asr(w27, w0, w6);
  END();

  RUN();

  ASSERT_EQUAL_64(value, x0);
  ASSERT_EQUAL_64(value >> (shift[0] & 63), x16);
  ASSERT_EQUAL_64(value >> (shift[1] & 63), x17);
  ASSERT_EQUAL_64(value >> (shift[2] & 63), x18);
  ASSERT_EQUAL_64(value >> (shift[3] & 63), x19);
  ASSERT_EQUAL_64(value >> (shift[4] & 63), x20);
  ASSERT_EQUAL_64(value >> (shift[5] & 63), x21);

  int32_t value32 = static_cast<int32_t>(value & 0xffffffff);
  ASSERT_EQUAL_32(value32 >> (shift[0] & 31), w22);
  ASSERT_EQUAL_32(value32 >> (shift[1] & 31), w23);
  ASSERT_EQUAL_32(value32 >> (shift[2] & 31), w24);
  ASSERT_EQUAL_32(value32 >> (shift[3] & 31), w25);
  ASSERT_EQUAL_32(value32 >> (shift[4] & 31), w26);
  ASSERT_EQUAL_32(value32 >> (shift[5] & 31), w27);

  TEARDOWN();
}


TEST(rorv) {
  SETUP();
  ALLOW_ASM();

  uint64_t value = 0x0123456789abcdef;
  int shift[] = {4, 8, 12, 16, 24, 36};

  START();
  __ Mov(x0, value);
  __ Mov(w1, shift[0]);
  __ Mov(w2, shift[1]);
  __ Mov(w3, shift[2]);
  __ Mov(w4, shift[3]);
  __ Mov(w5, shift[4]);
  __ Mov(w6, shift[5]);

  // The MacroAssembler does not allow zr as an argument.
  __ rorv(x0, x0, xzr);

  __ Ror(x16, x0, x1);
  __ Ror(x17, x0, x2);
  __ Ror(x18, x0, x3);
  __ Ror(x19, x0, x4);
  __ Ror(x20, x0, x5);
  __ Ror(x21, x0, x6);

  __ Ror(w22, w0, w1);
  __ Ror(w23, w0, w2);
  __ Ror(w24, w0, w3);
  __ Ror(w25, w0, w4);
  __ Ror(w26, w0, w5);
  __ Ror(w27, w0, w6);
  END();

  RUN();

  ASSERT_EQUAL_64(value, x0);
  ASSERT_EQUAL_64(0xf0123456789abcde, x16);
  ASSERT_EQUAL_64(0xef0123456789abcd, x17);
  ASSERT_EQUAL_64(0xdef0123456789abc, x18);
  ASSERT_EQUAL_64(0xcdef0123456789ab, x19);
  ASSERT_EQUAL_64(0xabcdef0123456789, x20);
  ASSERT_EQUAL_64(0x789abcdef0123456, x21);
  ASSERT_EQUAL_32(0xf89abcde, w22);
  ASSERT_EQUAL_32(0xef89abcd, w23);
  ASSERT_EQUAL_32(0xdef89abc, w24);
  ASSERT_EQUAL_32(0xcdef89ab, w25);
  ASSERT_EQUAL_32(0xabcdef89, w26);
  ASSERT_EQUAL_32(0xf89abcde, w27);

  TEARDOWN();
}


TEST(bfm) {
  SETUP();
  ALLOW_ASM();

  START();
  __ Mov(x1, 0x0123456789abcdef);

  __ Mov(x10, 0x8888888888888888);
  __ Mov(x11, 0x8888888888888888);
  __ Mov(x12, 0x8888888888888888);
  __ Mov(x13, 0x8888888888888888);
  __ Mov(w20, 0x88888888);
  __ Mov(w21, 0x88888888);

  // There are no macro instruction for bfm.
  __ Bfm(x10, x1, 16, 31);
  __ Bfm(x11, x1, 32, 15);

  __ Bfm(w20, w1, 16, 23);
  __ Bfm(w21, w1, 24, 15);

  // Aliases.
  __ Bfi(x12, x1, 16, 8);
  __ Bfxil(x13, x1, 16, 8);
  END();

  RUN();


  ASSERT_EQUAL_64(0x88888888888889ab, x10);
  ASSERT_EQUAL_64(0x8888cdef88888888, x11);

  ASSERT_EQUAL_32(0x888888ab, w20);
  ASSERT_EQUAL_32(0x88cdef88, w21);

  ASSERT_EQUAL_64(0x8888888888ef8888, x12);
  ASSERT_EQUAL_64(0x88888888888888ab, x13);

  TEARDOWN();
}


TEST(sbfm) {
  SETUP();
  ALLOW_ASM();

  START();
  __ Mov(x1, 0x0123456789abcdef);
  __ Mov(x2, 0xfedcba9876543210);

  // There are no macro instruction for sbfm.
  __ Sbfm(x10, x1, 16, 31);
  __ Sbfm(x11, x1, 32, 15);
  __ Sbfm(x12, x1, 32, 47);
  __ Sbfm(x13, x1, 48, 35);

  __ Sbfm(w14, w1, 16, 23);
  __ Sbfm(w15, w1, 24, 15);
  __ Sbfm(w16, w2, 16, 23);
  __ Sbfm(w17, w2, 24, 15);

  // Aliases.
  __ Asr(x18, x1, 32);
  __ Asr(x19, x2, 32);
  __ Sbfiz(x20, x1, 8, 16);
  __ Sbfiz(x21, x2, 8, 16);
  __ Sbfx(x22, x1, 8, 16);
  __ Sbfx(x23, x2, 8, 16);
  __ Sxtb(x24, w1);
  __ Sxtb(x25, x2);
  __ Sxth(x26, w1);
  __ Sxth(x27, x2);
  __ Sxtw(x28, w1);
  __ Sxtw(x29, x2);
  END();

  RUN();


  ASSERT_EQUAL_64(0xffffffffffff89ab, x10);
  ASSERT_EQUAL_64(0xffffcdef00000000, x11);
  ASSERT_EQUAL_64(0x0000000000004567, x12);
  ASSERT_EQUAL_64(0x000789abcdef0000, x13);

  ASSERT_EQUAL_32(0xffffffab, w14);
  ASSERT_EQUAL_32(0xffcdef00, w15);
  ASSERT_EQUAL_32(0x00000054, w16);
  ASSERT_EQUAL_32(0x00321000, w17);

  ASSERT_EQUAL_64(0x0000000001234567, x18);
  ASSERT_EQUAL_64(0xfffffffffedcba98, x19);
  ASSERT_EQUAL_64(0xffffffffffcdef00, x20);
  ASSERT_EQUAL_64(0x0000000000321000, x21);
  ASSERT_EQUAL_64(0xffffffffffffabcd, x22);
  ASSERT_EQUAL_64(0x0000000000005432, x23);
  ASSERT_EQUAL_64(0xffffffffffffffef, x24);
  ASSERT_EQUAL_64(0x0000000000000010, x25);
  ASSERT_EQUAL_64(0xffffffffffffcdef, x26);
  ASSERT_EQUAL_64(0x0000000000003210, x27);
  ASSERT_EQUAL_64(0xffffffff89abcdef, x28);
  ASSERT_EQUAL_64(0x0000000076543210, x29);

  TEARDOWN();
}


TEST(ubfm) {
  SETUP();
  ALLOW_ASM();

  START();
  __ Mov(x1, 0x0123456789abcdef);
  __ Mov(x2, 0xfedcba9876543210);

  __ Mov(x10, 0x8888888888888888);
  __ Mov(x11, 0x8888888888888888);

  // There are no macro instruction for ubfm.
  __ Ubfm(x10, x1, 16, 31);
  __ Ubfm(x11, x1, 32, 15);
  __ Ubfm(x12, x1, 32, 47);
  __ Ubfm(x13, x1, 48, 35);

  __ Ubfm(w25, w1, 16, 23);
  __ Ubfm(w26, w1, 24, 15);
  __ Ubfm(w27, w2, 16, 23);
  __ Ubfm(w28, w2, 24, 15);

  // Aliases
  __ Lsl(x15, x1, 63);
  __ Lsl(x16, x1, 0);
  __ Lsr(x17, x1, 32);
  __ Ubfiz(x18, x1, 8, 16);
  __ Ubfx(x19, x1, 8, 16);
  __ Uxtb(x20, x1);
  __ Uxth(x21, x1);
  __ Uxtw(x22, x1);
  END();

  RUN();

  ASSERT_EQUAL_64(0x00000000000089ab, x10);
  ASSERT_EQUAL_64(0x0000cdef00000000, x11);
  ASSERT_EQUAL_64(0x0000000000004567, x12);
  ASSERT_EQUAL_64(0x000789abcdef0000, x13);

  ASSERT_EQUAL_32(0x000000ab, w25);
  ASSERT_EQUAL_32(0x00cdef00, w26);
  ASSERT_EQUAL_32(0x00000054, w27);
  ASSERT_EQUAL_32(0x00321000, w28);

  ASSERT_EQUAL_64(0x8000000000000000, x15);
  ASSERT_EQUAL_64(0x0123456789abcdef, x16);
  ASSERT_EQUAL_64(0x0000000001234567, x17);
  ASSERT_EQUAL_64(0x0000000000cdef00, x18);
  ASSERT_EQUAL_64(0x000000000000abcd, x19);
  ASSERT_EQUAL_64(0x00000000000000ef, x20);
  ASSERT_EQUAL_64(0x000000000000cdef, x21);
  ASSERT_EQUAL_64(0x0000000089abcdef, x22);

  TEARDOWN();
}


TEST(extr) {
  SETUP();

  START();
  __ Mov(x1, 0x0123456789abcdef);
  __ Mov(x2, 0xfedcba9876543210);

  __ Extr(w10, w1, w2, 0);
  __ Extr(w11, w1, w2, 1);
  __ Extr(x12, x2, x1, 2);

  __ Ror(w13, w1, 0);
  __ Ror(w14, w2, 17);
  __ Ror(w15, w1, 31);
  __ Ror(x18, x2, 0);
  __ Ror(x19, x2, 1);
  __ Ror(x20, x1, 63);
  END();

  RUN();

  ASSERT_EQUAL_64(0x76543210, x10);
  ASSERT_EQUAL_64(0xbb2a1908, x11);
  ASSERT_EQUAL_64(0x0048d159e26af37b, x12);
  ASSERT_EQUAL_64(0x89abcdef, x13);
  ASSERT_EQUAL_64(0x19083b2a, x14);
  ASSERT_EQUAL_64(0x13579bdf, x15);
  ASSERT_EQUAL_64(0xfedcba9876543210, x18);
  ASSERT_EQUAL_64(0x7f6e5d4c3b2a1908, x19);
  ASSERT_EQUAL_64(0x02468acf13579bde, x20);

  TEARDOWN();
}


TEST(fmov_imm) {
  SETUP();

  START();
  __ Fmov(s11, 1.0);
  __ Fmov(d22, -13.0);
  __ Fmov(s1, 255.0);
  __ Fmov(d2, 12.34567);
  __ Fmov(s3, 0.0);
  __ Fmov(d4, 0.0);
  __ Fmov(s5, kFP32PositiveInfinity);
  __ Fmov(d6, kFP64NegativeInfinity);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s11);
  ASSERT_EQUAL_FP64(-13.0, d22);
  ASSERT_EQUAL_FP32(255.0, s1);
  ASSERT_EQUAL_FP64(12.34567, d2);
  ASSERT_EQUAL_FP32(0.0, s3);
  ASSERT_EQUAL_FP64(0.0, d4);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s5);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d6);

  TEARDOWN();
}


TEST(fmov_reg) {
  SETUP();

  START();
  __ Fmov(s20, 1.0);
  __ Fmov(w10, s20);
  __ Fmov(s30, w10);
  __ Fmov(s5, s20);
  __ Fmov(d1, -13.0);
  __ Fmov(x1, d1);
  __ Fmov(d2, x1);
  __ Fmov(d4, d1);
  __ Fmov(d6, rawbits_to_double(0x0123456789abcdef));
  __ Fmov(s6, s6);

  __ Fmov(d0, 0.0);
  __ Fmov(v0.D(), 1, x1);
  __ Fmov(x2, v0.D(), 1);

  END();

  RUN();

  ASSERT_EQUAL_32(float_to_rawbits(1.0), w10);
  ASSERT_EQUAL_FP32(1.0, s30);
  ASSERT_EQUAL_FP32(1.0, s5);
  ASSERT_EQUAL_64(double_to_rawbits(-13.0), x1);
  ASSERT_EQUAL_FP64(-13.0, d2);
  ASSERT_EQUAL_FP64(-13.0, d4);
  ASSERT_EQUAL_FP32(rawbits_to_float(0x89abcdef), s6);
  ASSERT_EQUAL_128(double_to_rawbits(-13.0), 0x0000000000000000, q0);
  ASSERT_EQUAL_64(double_to_rawbits(-13.0), x2);
  TEARDOWN();
}


TEST(fadd) {
  SETUP();

  START();
  __ Fmov(s14, -0.0f);
  __ Fmov(s15, kFP32PositiveInfinity);
  __ Fmov(s16, kFP32NegativeInfinity);
  __ Fmov(s17, 3.25f);
  __ Fmov(s18, 1.0f);
  __ Fmov(s19, 0.0f);

  __ Fmov(d26, -0.0);
  __ Fmov(d27, kFP64PositiveInfinity);
  __ Fmov(d28, kFP64NegativeInfinity);
  __ Fmov(d29, 0.0);
  __ Fmov(d30, -2.0);
  __ Fmov(d31, 2.25);

  __ Fadd(s0, s17, s18);
  __ Fadd(s1, s18, s19);
  __ Fadd(s2, s14, s18);
  __ Fadd(s3, s15, s18);
  __ Fadd(s4, s16, s18);
  __ Fadd(s5, s15, s16);
  __ Fadd(s6, s16, s15);

  __ Fadd(d7, d30, d31);
  __ Fadd(d8, d29, d31);
  __ Fadd(d9, d26, d31);
  __ Fadd(d10, d27, d31);
  __ Fadd(d11, d28, d31);
  __ Fadd(d12, d27, d28);
  __ Fadd(d13, d28, d27);
  END();

  RUN();

  ASSERT_EQUAL_FP32(4.25, s0);
  ASSERT_EQUAL_FP32(1.0, s1);
  ASSERT_EQUAL_FP32(1.0, s2);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s3);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s4);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s5);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s6);
  ASSERT_EQUAL_FP64(0.25, d7);
  ASSERT_EQUAL_FP64(2.25, d8);
  ASSERT_EQUAL_FP64(2.25, d9);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d10);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d11);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d12);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d13);

  TEARDOWN();
}


TEST(fsub) {
  SETUP();

  START();
  __ Fmov(s14, -0.0f);
  __ Fmov(s15, kFP32PositiveInfinity);
  __ Fmov(s16, kFP32NegativeInfinity);
  __ Fmov(s17, 3.25f);
  __ Fmov(s18, 1.0f);
  __ Fmov(s19, 0.0f);

  __ Fmov(d26, -0.0);
  __ Fmov(d27, kFP64PositiveInfinity);
  __ Fmov(d28, kFP64NegativeInfinity);
  __ Fmov(d29, 0.0);
  __ Fmov(d30, -2.0);
  __ Fmov(d31, 2.25);

  __ Fsub(s0, s17, s18);
  __ Fsub(s1, s18, s19);
  __ Fsub(s2, s14, s18);
  __ Fsub(s3, s18, s15);
  __ Fsub(s4, s18, s16);
  __ Fsub(s5, s15, s15);
  __ Fsub(s6, s16, s16);

  __ Fsub(d7, d30, d31);
  __ Fsub(d8, d29, d31);
  __ Fsub(d9, d26, d31);
  __ Fsub(d10, d31, d27);
  __ Fsub(d11, d31, d28);
  __ Fsub(d12, d27, d27);
  __ Fsub(d13, d28, d28);
  END();

  RUN();

  ASSERT_EQUAL_FP32(2.25, s0);
  ASSERT_EQUAL_FP32(1.0, s1);
  ASSERT_EQUAL_FP32(-1.0, s2);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s3);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s4);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s5);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s6);
  ASSERT_EQUAL_FP64(-4.25, d7);
  ASSERT_EQUAL_FP64(-2.25, d8);
  ASSERT_EQUAL_FP64(-2.25, d9);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d10);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d11);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d12);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d13);

  TEARDOWN();
}


TEST(fmul) {
  SETUP();

  START();
  __ Fmov(s14, -0.0f);
  __ Fmov(s15, kFP32PositiveInfinity);
  __ Fmov(s16, kFP32NegativeInfinity);
  __ Fmov(s17, 3.25f);
  __ Fmov(s18, 2.0f);
  __ Fmov(s19, 0.0f);
  __ Fmov(s20, -2.0f);

  __ Fmov(d26, -0.0);
  __ Fmov(d27, kFP64PositiveInfinity);
  __ Fmov(d28, kFP64NegativeInfinity);
  __ Fmov(d29, 0.0);
  __ Fmov(d30, -2.0);
  __ Fmov(d31, 2.25);

  __ Fmul(s0, s17, s18);
  __ Fmul(s1, s18, s19);
  __ Fmul(s2, s14, s14);
  __ Fmul(s3, s15, s20);
  __ Fmul(s4, s16, s20);
  __ Fmul(s5, s15, s19);
  __ Fmul(s6, s19, s16);

  __ Fmul(d7, d30, d31);
  __ Fmul(d8, d29, d31);
  __ Fmul(d9, d26, d26);
  __ Fmul(d10, d27, d30);
  __ Fmul(d11, d28, d30);
  __ Fmul(d12, d27, d29);
  __ Fmul(d13, d29, d28);
  END();

  RUN();

  ASSERT_EQUAL_FP32(6.5, s0);
  ASSERT_EQUAL_FP32(0.0, s1);
  ASSERT_EQUAL_FP32(0.0, s2);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s3);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s4);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s5);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s6);
  ASSERT_EQUAL_FP64(-4.5, d7);
  ASSERT_EQUAL_FP64(0.0, d8);
  ASSERT_EQUAL_FP64(0.0, d9);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d10);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d11);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d12);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d13);

  TEARDOWN();
}


static void FmaddFmsubHelper(double n, double m, double a,
                             double fmadd, double fmsub,
                             double fnmadd, double fnmsub) {
  SETUP();
  START();

  __ Fmov(d0, n);
  __ Fmov(d1, m);
  __ Fmov(d2, a);
  __ Fmadd(d28, d0, d1, d2);
  __ Fmsub(d29, d0, d1, d2);
  __ Fnmadd(d30, d0, d1, d2);
  __ Fnmsub(d31, d0, d1, d2);

  END();
  RUN();

  ASSERT_EQUAL_FP64(fmadd, d28);
  ASSERT_EQUAL_FP64(fmsub, d29);
  ASSERT_EQUAL_FP64(fnmadd, d30);
  ASSERT_EQUAL_FP64(fnmsub, d31);

  TEARDOWN();
}


TEST(fmadd_fmsub_double) {
  // It's hard to check the result of fused operations because the only way to
  // calculate the result is using fma, which is what the simulator uses anyway.

  // Basic operation.
  FmaddFmsubHelper(1.0, 2.0, 3.0, 5.0, 1.0, -5.0, -1.0);
  FmaddFmsubHelper(-1.0, 2.0, 3.0, 1.0, 5.0, -1.0, -5.0);

  // Check the sign of exact zeroes.
  //               n     m     a     fmadd  fmsub  fnmadd fnmsub
  FmaddFmsubHelper(-0.0, +0.0, -0.0, -0.0,  +0.0,  +0.0,  +0.0);
  FmaddFmsubHelper(+0.0, +0.0, -0.0, +0.0,  -0.0,  +0.0,  +0.0);
  FmaddFmsubHelper(+0.0, +0.0, +0.0, +0.0,  +0.0,  -0.0,  +0.0);
  FmaddFmsubHelper(-0.0, +0.0, +0.0, +0.0,  +0.0,  +0.0,  -0.0);
  FmaddFmsubHelper(+0.0, -0.0, -0.0, -0.0,  +0.0,  +0.0,  +0.0);
  FmaddFmsubHelper(-0.0, -0.0, -0.0, +0.0,  -0.0,  +0.0,  +0.0);
  FmaddFmsubHelper(-0.0, -0.0, +0.0, +0.0,  +0.0,  -0.0,  +0.0);
  FmaddFmsubHelper(+0.0, -0.0, +0.0, +0.0,  +0.0,  +0.0,  -0.0);

  // Check NaN generation.
  FmaddFmsubHelper(kFP64PositiveInfinity, 0.0, 42.0,
                   kFP64DefaultNaN, kFP64DefaultNaN,
                   kFP64DefaultNaN, kFP64DefaultNaN);
  FmaddFmsubHelper(0.0, kFP64PositiveInfinity, 42.0,
                   kFP64DefaultNaN, kFP64DefaultNaN,
                   kFP64DefaultNaN, kFP64DefaultNaN);
  FmaddFmsubHelper(kFP64PositiveInfinity, 1.0, kFP64PositiveInfinity,
                   kFP64PositiveInfinity,   //  inf + ( inf * 1) = inf
                   kFP64DefaultNaN,         //  inf + (-inf * 1) = NaN
                   kFP64NegativeInfinity,   // -inf + (-inf * 1) = -inf
                   kFP64DefaultNaN);        // -inf + ( inf * 1) = NaN
  FmaddFmsubHelper(kFP64NegativeInfinity, 1.0, kFP64PositiveInfinity,
                   kFP64DefaultNaN,         //  inf + (-inf * 1) = NaN
                   kFP64PositiveInfinity,   //  inf + ( inf * 1) = inf
                   kFP64DefaultNaN,         // -inf + ( inf * 1) = NaN
                   kFP64NegativeInfinity);  // -inf + (-inf * 1) = -inf
}


static void FmaddFmsubHelper(float n, float m, float a,
                             float fmadd, float fmsub,
                             float fnmadd, float fnmsub) {
  SETUP();
  START();

  __ Fmov(s0, n);
  __ Fmov(s1, m);
  __ Fmov(s2, a);
  __ Fmadd(s28, s0, s1, s2);
  __ Fmsub(s29, s0, s1, s2);
  __ Fnmadd(s30, s0, s1, s2);
  __ Fnmsub(s31, s0, s1, s2);

  END();
  RUN();

  ASSERT_EQUAL_FP32(fmadd, s28);
  ASSERT_EQUAL_FP32(fmsub, s29);
  ASSERT_EQUAL_FP32(fnmadd, s30);
  ASSERT_EQUAL_FP32(fnmsub, s31);

  TEARDOWN();
}


TEST(fmadd_fmsub_float) {
  // It's hard to check the result of fused operations because the only way to
  // calculate the result is using fma, which is what the simulator uses anyway.

  // Basic operation.
  FmaddFmsubHelper(1.0f, 2.0f, 3.0f, 5.0f, 1.0f, -5.0f, -1.0f);
  FmaddFmsubHelper(-1.0f, 2.0f, 3.0f, 1.0f, 5.0f, -1.0f, -5.0f);

  // Check the sign of exact zeroes.
  //               n      m      a      fmadd  fmsub  fnmadd fnmsub
  FmaddFmsubHelper(-0.0f, +0.0f, -0.0f, -0.0f, +0.0f, +0.0f, +0.0f);
  FmaddFmsubHelper(+0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, +0.0f);
  FmaddFmsubHelper(+0.0f, +0.0f, +0.0f, +0.0f, +0.0f, -0.0f, +0.0f);
  FmaddFmsubHelper(-0.0f, +0.0f, +0.0f, +0.0f, +0.0f, +0.0f, -0.0f);
  FmaddFmsubHelper(+0.0f, -0.0f, -0.0f, -0.0f, +0.0f, +0.0f, +0.0f);
  FmaddFmsubHelper(-0.0f, -0.0f, -0.0f, +0.0f, -0.0f, +0.0f, +0.0f);
  FmaddFmsubHelper(-0.0f, -0.0f, +0.0f, +0.0f, +0.0f, -0.0f, +0.0f);
  FmaddFmsubHelper(+0.0f, -0.0f, +0.0f, +0.0f, +0.0f, +0.0f, -0.0f);

  // Check NaN generation.
  FmaddFmsubHelper(kFP32PositiveInfinity, 0.0f, 42.0f,
                   kFP32DefaultNaN, kFP32DefaultNaN,
                   kFP32DefaultNaN, kFP32DefaultNaN);
  FmaddFmsubHelper(0.0f, kFP32PositiveInfinity, 42.0f,
                   kFP32DefaultNaN, kFP32DefaultNaN,
                   kFP32DefaultNaN, kFP32DefaultNaN);
  FmaddFmsubHelper(kFP32PositiveInfinity, 1.0f, kFP32PositiveInfinity,
                   kFP32PositiveInfinity,   //  inf + ( inf * 1) = inf
                   kFP32DefaultNaN,         //  inf + (-inf * 1) = NaN
                   kFP32NegativeInfinity,   // -inf + (-inf * 1) = -inf
                   kFP32DefaultNaN);        // -inf + ( inf * 1) = NaN
  FmaddFmsubHelper(kFP32NegativeInfinity, 1.0f, kFP32PositiveInfinity,
                   kFP32DefaultNaN,         //  inf + (-inf * 1) = NaN
                   kFP32PositiveInfinity,   //  inf + ( inf * 1) = inf
                   kFP32DefaultNaN,         // -inf + ( inf * 1) = NaN
                   kFP32NegativeInfinity);  // -inf + (-inf * 1) = -inf
}


TEST(fmadd_fmsub_double_nans) {
  // Make sure that NaN propagation works correctly.
  double s1 = rawbits_to_double(0x7ff5555511111111);
  double s2 = rawbits_to_double(0x7ff5555522222222);
  double sa = rawbits_to_double(0x7ff55555aaaaaaaa);
  double q1 = rawbits_to_double(0x7ffaaaaa11111111);
  double q2 = rawbits_to_double(0x7ffaaaaa22222222);
  double qa = rawbits_to_double(0x7ffaaaaaaaaaaaaa);
  VIXL_ASSERT(IsSignallingNaN(s1));
  VIXL_ASSERT(IsSignallingNaN(s2));
  VIXL_ASSERT(IsSignallingNaN(sa));
  VIXL_ASSERT(IsQuietNaN(q1));
  VIXL_ASSERT(IsQuietNaN(q2));
  VIXL_ASSERT(IsQuietNaN(qa));

  // The input NaNs after passing through ProcessNaN.
  double s1_proc = rawbits_to_double(0x7ffd555511111111);
  double s2_proc = rawbits_to_double(0x7ffd555522222222);
  double sa_proc = rawbits_to_double(0x7ffd5555aaaaaaaa);
  double q1_proc = q1;
  double q2_proc = q2;
  double qa_proc = qa;
  VIXL_ASSERT(IsQuietNaN(s1_proc));
  VIXL_ASSERT(IsQuietNaN(s2_proc));
  VIXL_ASSERT(IsQuietNaN(sa_proc));
  VIXL_ASSERT(IsQuietNaN(q1_proc));
  VIXL_ASSERT(IsQuietNaN(q2_proc));
  VIXL_ASSERT(IsQuietNaN(qa_proc));

  // Negated NaNs as it would be done on ARMv8 hardware.
  double s1_proc_neg = rawbits_to_double(0xfffd555511111111);
  double sa_proc_neg = rawbits_to_double(0xfffd5555aaaaaaaa);
  double q1_proc_neg = rawbits_to_double(0xfffaaaaa11111111);
  double qa_proc_neg = rawbits_to_double(0xfffaaaaaaaaaaaaa);
  VIXL_ASSERT(IsQuietNaN(s1_proc_neg));
  VIXL_ASSERT(IsQuietNaN(sa_proc_neg));
  VIXL_ASSERT(IsQuietNaN(q1_proc_neg));
  VIXL_ASSERT(IsQuietNaN(qa_proc_neg));

  // Quiet NaNs are propagated.
  FmaddFmsubHelper(q1, 0, 0, q1_proc, q1_proc_neg, q1_proc_neg, q1_proc);
  FmaddFmsubHelper(0, q2, 0, q2_proc, q2_proc, q2_proc, q2_proc);
  FmaddFmsubHelper(0, 0, qa, qa_proc, qa_proc, qa_proc_neg, qa_proc_neg);
  FmaddFmsubHelper(q1, q2, 0, q1_proc, q1_proc_neg, q1_proc_neg, q1_proc);
  FmaddFmsubHelper(0, q2, qa, qa_proc, qa_proc, qa_proc_neg, qa_proc_neg);
  FmaddFmsubHelper(q1, 0, qa, qa_proc, qa_proc, qa_proc_neg, qa_proc_neg);
  FmaddFmsubHelper(q1, q2, qa, qa_proc, qa_proc, qa_proc_neg, qa_proc_neg);

  // Signalling NaNs are propagated, and made quiet.
  FmaddFmsubHelper(s1, 0, 0, s1_proc, s1_proc_neg, s1_proc_neg, s1_proc);
  FmaddFmsubHelper(0, s2, 0, s2_proc, s2_proc, s2_proc, s2_proc);
  FmaddFmsubHelper(0, 0, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);
  FmaddFmsubHelper(s1, s2, 0, s1_proc, s1_proc_neg, s1_proc_neg, s1_proc);
  FmaddFmsubHelper(0, s2, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);
  FmaddFmsubHelper(s1, 0, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);
  FmaddFmsubHelper(s1, s2, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);

  // Signalling NaNs take precedence over quiet NaNs.
  FmaddFmsubHelper(s1, q2, qa, s1_proc, s1_proc_neg, s1_proc_neg, s1_proc);
  FmaddFmsubHelper(q1, s2, qa, s2_proc, s2_proc, s2_proc, s2_proc);
  FmaddFmsubHelper(q1, q2, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);
  FmaddFmsubHelper(s1, s2, qa, s1_proc, s1_proc_neg, s1_proc_neg, s1_proc);
  FmaddFmsubHelper(q1, s2, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);
  FmaddFmsubHelper(s1, q2, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);
  FmaddFmsubHelper(s1, s2, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);

  // A NaN generated by the intermediate op1 * op2 overrides a quiet NaN in a.
  FmaddFmsubHelper(0, kFP64PositiveInfinity, qa,
                   kFP64DefaultNaN, kFP64DefaultNaN,
                   kFP64DefaultNaN, kFP64DefaultNaN);
  FmaddFmsubHelper(kFP64PositiveInfinity, 0, qa,
                   kFP64DefaultNaN, kFP64DefaultNaN,
                   kFP64DefaultNaN, kFP64DefaultNaN);
  FmaddFmsubHelper(0, kFP64NegativeInfinity, qa,
                   kFP64DefaultNaN, kFP64DefaultNaN,
                   kFP64DefaultNaN, kFP64DefaultNaN);
  FmaddFmsubHelper(kFP64NegativeInfinity, 0, qa,
                   kFP64DefaultNaN, kFP64DefaultNaN,
                   kFP64DefaultNaN, kFP64DefaultNaN);
}


TEST(fmadd_fmsub_float_nans) {
  // Make sure that NaN propagation works correctly.
  float s1 = rawbits_to_float(0x7f951111);
  float s2 = rawbits_to_float(0x7f952222);
  float sa = rawbits_to_float(0x7f95aaaa);
  float q1 = rawbits_to_float(0x7fea1111);
  float q2 = rawbits_to_float(0x7fea2222);
  float qa = rawbits_to_float(0x7feaaaaa);
  VIXL_ASSERT(IsSignallingNaN(s1));
  VIXL_ASSERT(IsSignallingNaN(s2));
  VIXL_ASSERT(IsSignallingNaN(sa));
  VIXL_ASSERT(IsQuietNaN(q1));
  VIXL_ASSERT(IsQuietNaN(q2));
  VIXL_ASSERT(IsQuietNaN(qa));

  // The input NaNs after passing through ProcessNaN.
  float s1_proc = rawbits_to_float(0x7fd51111);
  float s2_proc = rawbits_to_float(0x7fd52222);
  float sa_proc = rawbits_to_float(0x7fd5aaaa);
  float q1_proc = q1;
  float q2_proc = q2;
  float qa_proc = qa;
  VIXL_ASSERT(IsQuietNaN(s1_proc));
  VIXL_ASSERT(IsQuietNaN(s2_proc));
  VIXL_ASSERT(IsQuietNaN(sa_proc));
  VIXL_ASSERT(IsQuietNaN(q1_proc));
  VIXL_ASSERT(IsQuietNaN(q2_proc));
  VIXL_ASSERT(IsQuietNaN(qa_proc));

  // Negated NaNs as it would be done on ARMv8 hardware.
  float s1_proc_neg = rawbits_to_float(0xffd51111);
  float sa_proc_neg = rawbits_to_float(0xffd5aaaa);
  float q1_proc_neg = rawbits_to_float(0xffea1111);
  float qa_proc_neg = rawbits_to_float(0xffeaaaaa);
  VIXL_ASSERT(IsQuietNaN(s1_proc_neg));
  VIXL_ASSERT(IsQuietNaN(sa_proc_neg));
  VIXL_ASSERT(IsQuietNaN(q1_proc_neg));
  VIXL_ASSERT(IsQuietNaN(qa_proc_neg));

  // Quiet NaNs are propagated.
  FmaddFmsubHelper(q1, 0, 0, q1_proc, q1_proc_neg, q1_proc_neg, q1_proc);
  FmaddFmsubHelper(0, q2, 0, q2_proc, q2_proc, q2_proc, q2_proc);
  FmaddFmsubHelper(0, 0, qa, qa_proc, qa_proc, qa_proc_neg, qa_proc_neg);
  FmaddFmsubHelper(q1, q2, 0, q1_proc, q1_proc_neg, q1_proc_neg, q1_proc);
  FmaddFmsubHelper(0, q2, qa, qa_proc, qa_proc, qa_proc_neg, qa_proc_neg);
  FmaddFmsubHelper(q1, 0, qa, qa_proc, qa_proc, qa_proc_neg, qa_proc_neg);
  FmaddFmsubHelper(q1, q2, qa, qa_proc, qa_proc, qa_proc_neg, qa_proc_neg);

  // Signalling NaNs are propagated, and made quiet.
  FmaddFmsubHelper(s1, 0, 0, s1_proc, s1_proc_neg, s1_proc_neg, s1_proc);
  FmaddFmsubHelper(0, s2, 0, s2_proc, s2_proc, s2_proc, s2_proc);
  FmaddFmsubHelper(0, 0, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);
  FmaddFmsubHelper(s1, s2, 0, s1_proc, s1_proc_neg, s1_proc_neg, s1_proc);
  FmaddFmsubHelper(0, s2, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);
  FmaddFmsubHelper(s1, 0, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);
  FmaddFmsubHelper(s1, s2, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);

  // Signalling NaNs take precedence over quiet NaNs.
  FmaddFmsubHelper(s1, q2, qa, s1_proc, s1_proc_neg, s1_proc_neg, s1_proc);
  FmaddFmsubHelper(q1, s2, qa, s2_proc, s2_proc, s2_proc, s2_proc);
  FmaddFmsubHelper(q1, q2, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);
  FmaddFmsubHelper(s1, s2, qa, s1_proc, s1_proc_neg, s1_proc_neg, s1_proc);
  FmaddFmsubHelper(q1, s2, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);
  FmaddFmsubHelper(s1, q2, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);
  FmaddFmsubHelper(s1, s2, sa, sa_proc, sa_proc, sa_proc_neg, sa_proc_neg);

  // A NaN generated by the intermediate op1 * op2 overrides a quiet NaN in a.
  FmaddFmsubHelper(0, kFP32PositiveInfinity, qa,
                   kFP32DefaultNaN, kFP32DefaultNaN,
                   kFP32DefaultNaN, kFP32DefaultNaN);
  FmaddFmsubHelper(kFP32PositiveInfinity, 0, qa,
                   kFP32DefaultNaN, kFP32DefaultNaN,
                   kFP32DefaultNaN, kFP32DefaultNaN);
  FmaddFmsubHelper(0, kFP32NegativeInfinity, qa,
                   kFP32DefaultNaN, kFP32DefaultNaN,
                   kFP32DefaultNaN, kFP32DefaultNaN);
  FmaddFmsubHelper(kFP32NegativeInfinity, 0, qa,
                   kFP32DefaultNaN, kFP32DefaultNaN,
                   kFP32DefaultNaN, kFP32DefaultNaN);
}


TEST(fdiv) {
  SETUP();

  START();
  __ Fmov(s14, -0.0f);
  __ Fmov(s15, kFP32PositiveInfinity);
  __ Fmov(s16, kFP32NegativeInfinity);
  __ Fmov(s17, 3.25f);
  __ Fmov(s18, 2.0f);
  __ Fmov(s19, 2.0f);
  __ Fmov(s20, -2.0f);

  __ Fmov(d26, -0.0);
  __ Fmov(d27, kFP64PositiveInfinity);
  __ Fmov(d28, kFP64NegativeInfinity);
  __ Fmov(d29, 0.0);
  __ Fmov(d30, -2.0);
  __ Fmov(d31, 2.25);

  __ Fdiv(s0, s17, s18);
  __ Fdiv(s1, s18, s19);
  __ Fdiv(s2, s14, s18);
  __ Fdiv(s3, s18, s15);
  __ Fdiv(s4, s18, s16);
  __ Fdiv(s5, s15, s16);
  __ Fdiv(s6, s14, s14);

  __ Fdiv(d7, d31, d30);
  __ Fdiv(d8, d29, d31);
  __ Fdiv(d9, d26, d31);
  __ Fdiv(d10, d31, d27);
  __ Fdiv(d11, d31, d28);
  __ Fdiv(d12, d28, d27);
  __ Fdiv(d13, d29, d29);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.625f, s0);
  ASSERT_EQUAL_FP32(1.0f, s1);
  ASSERT_EQUAL_FP32(-0.0f, s2);
  ASSERT_EQUAL_FP32(0.0f, s3);
  ASSERT_EQUAL_FP32(-0.0f, s4);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s5);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s6);
  ASSERT_EQUAL_FP64(-1.125, d7);
  ASSERT_EQUAL_FP64(0.0, d8);
  ASSERT_EQUAL_FP64(-0.0, d9);
  ASSERT_EQUAL_FP64(0.0, d10);
  ASSERT_EQUAL_FP64(-0.0, d11);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d12);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d13);

  TEARDOWN();
}


static float MinMaxHelper(float n,
                          float m,
                          bool min,
                          float quiet_nan_substitute = 0.0) {
  const uint64_t kFP32QuietNaNMask = 0x00400000;
  uint32_t raw_n = float_to_rawbits(n);
  uint32_t raw_m = float_to_rawbits(m);

  if (std::isnan(n) && ((raw_n & kFP32QuietNaNMask) == 0)) {
    // n is signalling NaN.
    return rawbits_to_float(raw_n | kFP32QuietNaNMask);
  } else if (std::isnan(m) && ((raw_m & kFP32QuietNaNMask) == 0)) {
    // m is signalling NaN.
    return rawbits_to_float(raw_m | kFP32QuietNaNMask);
  } else if (quiet_nan_substitute == 0.0) {
    if (std::isnan(n)) {
      // n is quiet NaN.
      return n;
    } else if (std::isnan(m)) {
      // m is quiet NaN.
      return m;
    }
  } else {
    // Substitute n or m if one is quiet, but not both.
    if (std::isnan(n) && !std::isnan(m)) {
      // n is quiet NaN: replace with substitute.
      n = quiet_nan_substitute;
    } else if (!std::isnan(n) && std::isnan(m)) {
      // m is quiet NaN: replace with substitute.
      m = quiet_nan_substitute;
    }
  }

  if ((n == 0.0) && (m == 0.0) &&
      (copysign(1.0, n) != copysign(1.0, m))) {
    return min ? -0.0 : 0.0;
  }

  return min ? fminf(n, m) : fmaxf(n, m);
}


static double MinMaxHelper(double n,
                           double m,
                           bool min,
                           double quiet_nan_substitute = 0.0) {
  const uint64_t kFP64QuietNaNMask = 0x0008000000000000;
  uint64_t raw_n = double_to_rawbits(n);
  uint64_t raw_m = double_to_rawbits(m);

  if (std::isnan(n) && ((raw_n & kFP64QuietNaNMask) == 0)) {
    // n is signalling NaN.
    return rawbits_to_double(raw_n | kFP64QuietNaNMask);
  } else if (std::isnan(m) && ((raw_m & kFP64QuietNaNMask) == 0)) {
    // m is signalling NaN.
    return rawbits_to_double(raw_m | kFP64QuietNaNMask);
  } else if (quiet_nan_substitute == 0.0) {
    if (std::isnan(n)) {
      // n is quiet NaN.
      return n;
    } else if (std::isnan(m)) {
      // m is quiet NaN.
      return m;
    }
  } else {
    // Substitute n or m if one is quiet, but not both.
    if (std::isnan(n) && !std::isnan(m)) {
      // n is quiet NaN: replace with substitute.
      n = quiet_nan_substitute;
    } else if (!std::isnan(n) && std::isnan(m)) {
      // m is quiet NaN: replace with substitute.
      m = quiet_nan_substitute;
    }
  }

  if ((n == 0.0) && (m == 0.0) &&
      (copysign(1.0, n) != copysign(1.0, m))) {
    return min ? -0.0 : 0.0;
  }

  return min ? fmin(n, m) : fmax(n, m);
}


static void FminFmaxDoubleHelper(double n, double m, double min, double max,
                                 double minnm, double maxnm) {
  SETUP();

  START();
  __ Fmov(d0, n);
  __ Fmov(d1, m);
  __ Fmin(d28, d0, d1);
  __ Fmax(d29, d0, d1);
  __ Fminnm(d30, d0, d1);
  __ Fmaxnm(d31, d0, d1);
  END();

  RUN();

  ASSERT_EQUAL_FP64(min, d28);
  ASSERT_EQUAL_FP64(max, d29);
  ASSERT_EQUAL_FP64(minnm, d30);
  ASSERT_EQUAL_FP64(maxnm, d31);

  TEARDOWN();
}


TEST(fmax_fmin_d) {
  // Use non-standard NaNs to check that the payload bits are preserved.
  double snan = rawbits_to_double(0x7ff5555512345678);
  double qnan = rawbits_to_double(0x7ffaaaaa87654321);

  double snan_processed = rawbits_to_double(0x7ffd555512345678);
  double qnan_processed = qnan;

  VIXL_ASSERT(IsSignallingNaN(snan));
  VIXL_ASSERT(IsQuietNaN(qnan));
  VIXL_ASSERT(IsQuietNaN(snan_processed));
  VIXL_ASSERT(IsQuietNaN(qnan_processed));

  // Bootstrap tests.
  FminFmaxDoubleHelper(0, 0, 0, 0, 0, 0);
  FminFmaxDoubleHelper(0, 1, 0, 1, 0, 1);
  FminFmaxDoubleHelper(kFP64PositiveInfinity, kFP64NegativeInfinity,
                       kFP64NegativeInfinity, kFP64PositiveInfinity,
                       kFP64NegativeInfinity, kFP64PositiveInfinity);
  FminFmaxDoubleHelper(snan, 0,
                       snan_processed, snan_processed,
                       snan_processed, snan_processed);
  FminFmaxDoubleHelper(0, snan,
                       snan_processed, snan_processed,
                       snan_processed, snan_processed);
  FminFmaxDoubleHelper(qnan, 0,
                       qnan_processed, qnan_processed,
                       0, 0);
  FminFmaxDoubleHelper(0, qnan,
                       qnan_processed, qnan_processed,
                       0, 0);
  FminFmaxDoubleHelper(qnan, snan,
                       snan_processed, snan_processed,
                       snan_processed, snan_processed);
  FminFmaxDoubleHelper(snan, qnan,
                       snan_processed, snan_processed,
                       snan_processed, snan_processed);

  // Iterate over all combinations of inputs.
  double inputs[] = { DBL_MAX, DBL_MIN, 1.0, 0.0,
                      -DBL_MAX, -DBL_MIN, -1.0, -0.0,
                      kFP64PositiveInfinity, kFP64NegativeInfinity,
                      kFP64QuietNaN, kFP64SignallingNaN };

  const int count = sizeof(inputs) / sizeof(inputs[0]);

  for (int in = 0; in < count; in++) {
    double n = inputs[in];
    for (int im = 0; im < count; im++) {
      double m = inputs[im];
      FminFmaxDoubleHelper(n, m,
                           MinMaxHelper(n, m, true),
                           MinMaxHelper(n, m, false),
                           MinMaxHelper(n, m, true, kFP64PositiveInfinity),
                           MinMaxHelper(n, m, false, kFP64NegativeInfinity));
    }
  }
}


static void FminFmaxFloatHelper(float n, float m, float min, float max,
                                float minnm, float maxnm) {
  SETUP();

  START();
  __ Fmov(s0, n);
  __ Fmov(s1, m);
  __ Fmin(s28, s0, s1);
  __ Fmax(s29, s0, s1);
  __ Fminnm(s30, s0, s1);
  __ Fmaxnm(s31, s0, s1);
  END();

  RUN();

  ASSERT_EQUAL_FP32(min, s28);
  ASSERT_EQUAL_FP32(max, s29);
  ASSERT_EQUAL_FP32(minnm, s30);
  ASSERT_EQUAL_FP32(maxnm, s31);

  TEARDOWN();
}


TEST(fmax_fmin_s) {
  // Use non-standard NaNs to check that the payload bits are preserved.
  float snan = rawbits_to_float(0x7f951234);
  float qnan = rawbits_to_float(0x7fea8765);

  float snan_processed = rawbits_to_float(0x7fd51234);
  float qnan_processed = qnan;

  VIXL_ASSERT(IsSignallingNaN(snan));
  VIXL_ASSERT(IsQuietNaN(qnan));
  VIXL_ASSERT(IsQuietNaN(snan_processed));
  VIXL_ASSERT(IsQuietNaN(qnan_processed));

  // Bootstrap tests.
  FminFmaxFloatHelper(0, 0, 0, 0, 0, 0);
  FminFmaxFloatHelper(0, 1, 0, 1, 0, 1);
  FminFmaxFloatHelper(kFP32PositiveInfinity, kFP32NegativeInfinity,
                      kFP32NegativeInfinity, kFP32PositiveInfinity,
                      kFP32NegativeInfinity, kFP32PositiveInfinity);
  FminFmaxFloatHelper(snan, 0,
                      snan_processed, snan_processed,
                      snan_processed, snan_processed);
  FminFmaxFloatHelper(0, snan,
                      snan_processed, snan_processed,
                      snan_processed, snan_processed);
  FminFmaxFloatHelper(qnan, 0,
                      qnan_processed, qnan_processed,
                      0, 0);
  FminFmaxFloatHelper(0, qnan,
                      qnan_processed, qnan_processed,
                      0, 0);
  FminFmaxFloatHelper(qnan, snan,
                      snan_processed, snan_processed,
                      snan_processed, snan_processed);
  FminFmaxFloatHelper(snan, qnan,
                      snan_processed, snan_processed,
                      snan_processed, snan_processed);

  // Iterate over all combinations of inputs.
  float inputs[] = { FLT_MAX, FLT_MIN, 1.0, 0.0,
                     -FLT_MAX, -FLT_MIN, -1.0, -0.0,
                     kFP32PositiveInfinity, kFP32NegativeInfinity,
                     kFP32QuietNaN, kFP32SignallingNaN };

  const int count = sizeof(inputs) / sizeof(inputs[0]);

  for (int in = 0; in < count; in++) {
    float n = inputs[in];
    for (int im = 0; im < count; im++) {
      float m = inputs[im];
      FminFmaxFloatHelper(n, m,
                          MinMaxHelper(n, m, true),
                          MinMaxHelper(n, m, false),
                          MinMaxHelper(n, m, true, kFP32PositiveInfinity),
                          MinMaxHelper(n, m, false, kFP32NegativeInfinity));
    }
  }
}


TEST(fccmp) {
  SETUP();
  ALLOW_ASM();

  START();
  __ Fmov(s16, 0.0);
  __ Fmov(s17, 0.5);
  __ Fmov(d18, -0.5);
  __ Fmov(d19, -1.0);
  __ Mov(x20, 0);
  __ Mov(x21, 0x7ff0000000000001);  // Double precision NaN.
  __ Fmov(d21, x21);
  __ Mov(w22, 0x7f800001);  // Single precision NaN.
  __ Fmov(s22, w22);

  __ Cmp(x20, 0);
  __ Fccmp(s16, s16, NoFlag, eq);
  __ Mrs(x0, NZCV);

  __ Cmp(x20, 0);
  __ Fccmp(s16, s16, VFlag, ne);
  __ Mrs(x1, NZCV);

  __ Cmp(x20, 0);
  __ Fccmp(s16, s17, CFlag, ge);
  __ Mrs(x2, NZCV);

  __ Cmp(x20, 0);
  __ Fccmp(s16, s17, CVFlag, lt);
  __ Mrs(x3, NZCV);

  __ Cmp(x20, 0);
  __ Fccmp(d18, d18, ZFlag, le);
  __ Mrs(x4, NZCV);

  __ Cmp(x20, 0);
  __ Fccmp(d18, d18, ZVFlag, gt);
  __ Mrs(x5, NZCV);

  __ Cmp(x20, 0);
  __ Fccmp(d18, d19, ZCVFlag, ls);
  __ Mrs(x6, NZCV);

  __ Cmp(x20, 0);
  __ Fccmp(d18, d19, NFlag, hi);
  __ Mrs(x7, NZCV);

  // The Macro Assembler does not allow al or nv as condition.
  __ fccmp(s16, s16, NFlag, al);
  __ Mrs(x8, NZCV);

  __ fccmp(d18, d18, NFlag, nv);
  __ Mrs(x9, NZCV);

  __ Cmp(x20, 0);
  __ Fccmpe(s16, s16, NoFlag, eq);
  __ Mrs(x10, NZCV);

  __ Cmp(x20, 0);
  __ Fccmpe(d18, d19, ZCVFlag, ls);
  __ Mrs(x11, NZCV);

  __ Cmp(x20, 0);
  __ Fccmpe(d21, d21, NoFlag, eq);
  __ Mrs(x12, NZCV);

  __ Cmp(x20, 0);
  __ Fccmpe(s22, s22, NoFlag, eq);
  __ Mrs(x13, NZCV);
  END();

  RUN();

  ASSERT_EQUAL_32(ZCFlag, w0);
  ASSERT_EQUAL_32(VFlag, w1);
  ASSERT_EQUAL_32(NFlag, w2);
  ASSERT_EQUAL_32(CVFlag, w3);
  ASSERT_EQUAL_32(ZCFlag, w4);
  ASSERT_EQUAL_32(ZVFlag, w5);
  ASSERT_EQUAL_32(CFlag, w6);
  ASSERT_EQUAL_32(NFlag, w7);
  ASSERT_EQUAL_32(ZCFlag, w8);
  ASSERT_EQUAL_32(ZCFlag, w9);
  ASSERT_EQUAL_32(ZCFlag, w10);
  ASSERT_EQUAL_32(CFlag, w11);
  ASSERT_EQUAL_32(CVFlag, w12);
  ASSERT_EQUAL_32(CVFlag, w13);

  TEARDOWN();
}


TEST(fcmp) {
  SETUP();

  START();

  // Some of these tests require a floating-point scratch register assigned to
  // the macro assembler, but most do not.
  {
    UseScratchRegisterScope temps(&masm);
    temps.ExcludeAll();
    temps.Include(ip0, ip1);

    __ Fmov(s8, 0.0);
    __ Fmov(s9, 0.5);
    __ Mov(w18, 0x7f800001);  // Single precision NaN.
    __ Fmov(s18, w18);

    __ Fcmp(s8, s8);
    __ Mrs(x0, NZCV);
    __ Fcmp(s8, s9);
    __ Mrs(x1, NZCV);
    __ Fcmp(s9, s8);
    __ Mrs(x2, NZCV);
    __ Fcmp(s8, s18);
    __ Mrs(x3, NZCV);
    __ Fcmp(s18, s18);
    __ Mrs(x4, NZCV);
    __ Fcmp(s8, 0.0);
    __ Mrs(x5, NZCV);
    temps.Include(d0);
    __ Fcmp(s8, 255.0);
    temps.Exclude(d0);
    __ Mrs(x6, NZCV);

    __ Fmov(d19, 0.0);
    __ Fmov(d20, 0.5);
    __ Mov(x21, 0x7ff0000000000001);  // Double precision NaN.
    __ Fmov(d21, x21);

    __ Fcmp(d19, d19);
    __ Mrs(x10, NZCV);
    __ Fcmp(d19, d20);
    __ Mrs(x11, NZCV);
    __ Fcmp(d20, d19);
    __ Mrs(x12, NZCV);
    __ Fcmp(d19, d21);
    __ Mrs(x13, NZCV);
    __ Fcmp(d21, d21);
    __ Mrs(x14, NZCV);
    __ Fcmp(d19, 0.0);
    __ Mrs(x15, NZCV);
    temps.Include(d0);
    __ Fcmp(d19, 12.3456);
    temps.Exclude(d0);
    __ Mrs(x16, NZCV);

    __ Fcmpe(s8, s8);
    __ Mrs(x22, NZCV);
    __ Fcmpe(s8, 0.0);
    __ Mrs(x23, NZCV);
    __ Fcmpe(d19, d19);
    __ Mrs(x24, NZCV);
    __ Fcmpe(d19, 0.0);
    __ Mrs(x25, NZCV);
    __ Fcmpe(s18, s18);
    __ Mrs(x26, NZCV);
    __ Fcmpe(d21, d21);
    __ Mrs(x27, NZCV);
  }

  END();

  RUN();

  ASSERT_EQUAL_32(ZCFlag, w0);
  ASSERT_EQUAL_32(NFlag, w1);
  ASSERT_EQUAL_32(CFlag, w2);
  ASSERT_EQUAL_32(CVFlag, w3);
  ASSERT_EQUAL_32(CVFlag, w4);
  ASSERT_EQUAL_32(ZCFlag, w5);
  ASSERT_EQUAL_32(NFlag, w6);
  ASSERT_EQUAL_32(ZCFlag, w10);
  ASSERT_EQUAL_32(NFlag, w11);
  ASSERT_EQUAL_32(CFlag, w12);
  ASSERT_EQUAL_32(CVFlag, w13);
  ASSERT_EQUAL_32(CVFlag, w14);
  ASSERT_EQUAL_32(ZCFlag, w15);
  ASSERT_EQUAL_32(NFlag, w16);
  ASSERT_EQUAL_32(ZCFlag, w22);
  ASSERT_EQUAL_32(ZCFlag, w23);
  ASSERT_EQUAL_32(ZCFlag, w24);
  ASSERT_EQUAL_32(ZCFlag, w25);
  ASSERT_EQUAL_32(CVFlag, w26);
  ASSERT_EQUAL_32(CVFlag, w27);

  TEARDOWN();
}


TEST(fcsel) {
  SETUP();
  ALLOW_ASM();

  START();
  __ Mov(x16, 0);
  __ Fmov(s16, 1.0);
  __ Fmov(s17, 2.0);
  __ Fmov(d18, 3.0);
  __ Fmov(d19, 4.0);

  __ Cmp(x16, 0);
  __ Fcsel(s0, s16, s17, eq);
  __ Fcsel(s1, s16, s17, ne);
  __ Fcsel(d2, d18, d19, eq);
  __ Fcsel(d3, d18, d19, ne);
  // The Macro Assembler does not allow al or nv as condition.
  __ fcsel(s4, s16, s17, al);
  __ fcsel(d5, d18, d19, nv);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s0);
  ASSERT_EQUAL_FP32(2.0, s1);
  ASSERT_EQUAL_FP64(3.0, d2);
  ASSERT_EQUAL_FP64(4.0, d3);
  ASSERT_EQUAL_FP32(1.0, s4);
  ASSERT_EQUAL_FP64(3.0, d5);

  TEARDOWN();
}


TEST(fneg) {
  SETUP();

  START();
  __ Fmov(s16, 1.0);
  __ Fmov(s17, 0.0);
  __ Fmov(s18, kFP32PositiveInfinity);
  __ Fmov(d19, 1.0);
  __ Fmov(d20, 0.0);
  __ Fmov(d21, kFP64PositiveInfinity);

  __ Fneg(s0, s16);
  __ Fneg(s1, s0);
  __ Fneg(s2, s17);
  __ Fneg(s3, s2);
  __ Fneg(s4, s18);
  __ Fneg(s5, s4);
  __ Fneg(d6, d19);
  __ Fneg(d7, d6);
  __ Fneg(d8, d20);
  __ Fneg(d9, d8);
  __ Fneg(d10, d21);
  __ Fneg(d11, d10);
  END();

  RUN();

  ASSERT_EQUAL_FP32(-1.0, s0);
  ASSERT_EQUAL_FP32(1.0, s1);
  ASSERT_EQUAL_FP32(-0.0, s2);
  ASSERT_EQUAL_FP32(0.0, s3);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s4);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s5);
  ASSERT_EQUAL_FP64(-1.0, d6);
  ASSERT_EQUAL_FP64(1.0, d7);
  ASSERT_EQUAL_FP64(-0.0, d8);
  ASSERT_EQUAL_FP64(0.0, d9);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d10);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d11);

  TEARDOWN();
}


TEST(fabs) {
  SETUP();

  START();
  __ Fmov(s16, -1.0);
  __ Fmov(s17, -0.0);
  __ Fmov(s18, kFP32NegativeInfinity);
  __ Fmov(d19, -1.0);
  __ Fmov(d20, -0.0);
  __ Fmov(d21, kFP64NegativeInfinity);

  __ Fabs(s0, s16);
  __ Fabs(s1, s0);
  __ Fabs(s2, s17);
  __ Fabs(s3, s18);
  __ Fabs(d4, d19);
  __ Fabs(d5, d4);
  __ Fabs(d6, d20);
  __ Fabs(d7, d21);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s0);
  ASSERT_EQUAL_FP32(1.0, s1);
  ASSERT_EQUAL_FP32(0.0, s2);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s3);
  ASSERT_EQUAL_FP64(1.0, d4);
  ASSERT_EQUAL_FP64(1.0, d5);
  ASSERT_EQUAL_FP64(0.0, d6);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d7);

  TEARDOWN();
}


TEST(fsqrt) {
  SETUP();

  START();
  __ Fmov(s16, 0.0);
  __ Fmov(s17, 1.0);
  __ Fmov(s18, 0.25);
  __ Fmov(s19, 65536.0);
  __ Fmov(s20, -0.0);
  __ Fmov(s21, kFP32PositiveInfinity);
  __ Fmov(s22, -1.0);
  __ Fmov(d23, 0.0);
  __ Fmov(d24, 1.0);
  __ Fmov(d25, 0.25);
  __ Fmov(d26, 4294967296.0);
  __ Fmov(d27, -0.0);
  __ Fmov(d28, kFP64PositiveInfinity);
  __ Fmov(d29, -1.0);

  __ Fsqrt(s0, s16);
  __ Fsqrt(s1, s17);
  __ Fsqrt(s2, s18);
  __ Fsqrt(s3, s19);
  __ Fsqrt(s4, s20);
  __ Fsqrt(s5, s21);
  __ Fsqrt(s6, s22);
  __ Fsqrt(d7, d23);
  __ Fsqrt(d8, d24);
  __ Fsqrt(d9, d25);
  __ Fsqrt(d10, d26);
  __ Fsqrt(d11, d27);
  __ Fsqrt(d12, d28);
  __ Fsqrt(d13, d29);
  END();

  RUN();

  ASSERT_EQUAL_FP32(0.0, s0);
  ASSERT_EQUAL_FP32(1.0, s1);
  ASSERT_EQUAL_FP32(0.5, s2);
  ASSERT_EQUAL_FP32(256.0, s3);
  ASSERT_EQUAL_FP32(-0.0, s4);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s5);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s6);
  ASSERT_EQUAL_FP64(0.0, d7);
  ASSERT_EQUAL_FP64(1.0, d8);
  ASSERT_EQUAL_FP64(0.5, d9);
  ASSERT_EQUAL_FP64(65536.0, d10);
  ASSERT_EQUAL_FP64(-0.0, d11);
  ASSERT_EQUAL_FP64(kFP32PositiveInfinity, d12);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d13);

  TEARDOWN();
}


TEST(frinta) {
  SETUP();

  START();
  __ Fmov(s16, 1.0);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, 1.9);
  __ Fmov(s20, 2.5);
  __ Fmov(s21, -1.5);
  __ Fmov(s22, -2.5);
  __ Fmov(s23, kFP32PositiveInfinity);
  __ Fmov(s24, kFP32NegativeInfinity);
  __ Fmov(s25, 0.0);
  __ Fmov(s26, -0.0);
  __ Fmov(s27, -0.2);

  __ Frinta(s0, s16);
  __ Frinta(s1, s17);
  __ Frinta(s2, s18);
  __ Frinta(s3, s19);
  __ Frinta(s4, s20);
  __ Frinta(s5, s21);
  __ Frinta(s6, s22);
  __ Frinta(s7, s23);
  __ Frinta(s8, s24);
  __ Frinta(s9, s25);
  __ Frinta(s10, s26);
  __ Frinta(s11, s27);

  __ Fmov(d16, 1.0);
  __ Fmov(d17, 1.1);
  __ Fmov(d18, 1.5);
  __ Fmov(d19, 1.9);
  __ Fmov(d20, 2.5);
  __ Fmov(d21, -1.5);
  __ Fmov(d22, -2.5);
  __ Fmov(d23, kFP32PositiveInfinity);
  __ Fmov(d24, kFP32NegativeInfinity);
  __ Fmov(d25, 0.0);
  __ Fmov(d26, -0.0);
  __ Fmov(d27, -0.2);

  __ Frinta(d12, d16);
  __ Frinta(d13, d17);
  __ Frinta(d14, d18);
  __ Frinta(d15, d19);
  __ Frinta(d16, d20);
  __ Frinta(d17, d21);
  __ Frinta(d18, d22);
  __ Frinta(d19, d23);
  __ Frinta(d20, d24);
  __ Frinta(d21, d25);
  __ Frinta(d22, d26);
  __ Frinta(d23, d27);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s0);
  ASSERT_EQUAL_FP32(1.0, s1);
  ASSERT_EQUAL_FP32(2.0, s2);
  ASSERT_EQUAL_FP32(2.0, s3);
  ASSERT_EQUAL_FP32(3.0, s4);
  ASSERT_EQUAL_FP32(-2.0, s5);
  ASSERT_EQUAL_FP32(-3.0, s6);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s7);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s8);
  ASSERT_EQUAL_FP32(0.0, s9);
  ASSERT_EQUAL_FP32(-0.0, s10);
  ASSERT_EQUAL_FP32(-0.0, s11);
  ASSERT_EQUAL_FP64(1.0, d12);
  ASSERT_EQUAL_FP64(1.0, d13);
  ASSERT_EQUAL_FP64(2.0, d14);
  ASSERT_EQUAL_FP64(2.0, d15);
  ASSERT_EQUAL_FP64(3.0, d16);
  ASSERT_EQUAL_FP64(-2.0, d17);
  ASSERT_EQUAL_FP64(-3.0, d18);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d19);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d20);
  ASSERT_EQUAL_FP64(0.0, d21);
  ASSERT_EQUAL_FP64(-0.0, d22);
  ASSERT_EQUAL_FP64(-0.0, d23);

  TEARDOWN();
}


TEST(frinti) {
  // VIXL only supports the round-to-nearest FPCR mode, so this test has the
  // same results as frintn.
  SETUP();

  START();
  __ Fmov(s16, 1.0);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, 1.9);
  __ Fmov(s20, 2.5);
  __ Fmov(s21, -1.5);
  __ Fmov(s22, -2.5);
  __ Fmov(s23, kFP32PositiveInfinity);
  __ Fmov(s24, kFP32NegativeInfinity);
  __ Fmov(s25, 0.0);
  __ Fmov(s26, -0.0);
  __ Fmov(s27, -0.2);

  __ Frinti(s0, s16);
  __ Frinti(s1, s17);
  __ Frinti(s2, s18);
  __ Frinti(s3, s19);
  __ Frinti(s4, s20);
  __ Frinti(s5, s21);
  __ Frinti(s6, s22);
  __ Frinti(s7, s23);
  __ Frinti(s8, s24);
  __ Frinti(s9, s25);
  __ Frinti(s10, s26);
  __ Frinti(s11, s27);

  __ Fmov(d16, 1.0);
  __ Fmov(d17, 1.1);
  __ Fmov(d18, 1.5);
  __ Fmov(d19, 1.9);
  __ Fmov(d20, 2.5);
  __ Fmov(d21, -1.5);
  __ Fmov(d22, -2.5);
  __ Fmov(d23, kFP32PositiveInfinity);
  __ Fmov(d24, kFP32NegativeInfinity);
  __ Fmov(d25, 0.0);
  __ Fmov(d26, -0.0);
  __ Fmov(d27, -0.2);

  __ Frinti(d12, d16);
  __ Frinti(d13, d17);
  __ Frinti(d14, d18);
  __ Frinti(d15, d19);
  __ Frinti(d16, d20);
  __ Frinti(d17, d21);
  __ Frinti(d18, d22);
  __ Frinti(d19, d23);
  __ Frinti(d20, d24);
  __ Frinti(d21, d25);
  __ Frinti(d22, d26);
  __ Frinti(d23, d27);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s0);
  ASSERT_EQUAL_FP32(1.0, s1);
  ASSERT_EQUAL_FP32(2.0, s2);
  ASSERT_EQUAL_FP32(2.0, s3);
  ASSERT_EQUAL_FP32(2.0, s4);
  ASSERT_EQUAL_FP32(-2.0, s5);
  ASSERT_EQUAL_FP32(-2.0, s6);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s7);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s8);
  ASSERT_EQUAL_FP32(0.0, s9);
  ASSERT_EQUAL_FP32(-0.0, s10);
  ASSERT_EQUAL_FP32(-0.0, s11);
  ASSERT_EQUAL_FP64(1.0, d12);
  ASSERT_EQUAL_FP64(1.0, d13);
  ASSERT_EQUAL_FP64(2.0, d14);
  ASSERT_EQUAL_FP64(2.0, d15);
  ASSERT_EQUAL_FP64(2.0, d16);
  ASSERT_EQUAL_FP64(-2.0, d17);
  ASSERT_EQUAL_FP64(-2.0, d18);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d19);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d20);
  ASSERT_EQUAL_FP64(0.0, d21);
  ASSERT_EQUAL_FP64(-0.0, d22);
  ASSERT_EQUAL_FP64(-0.0, d23);

  TEARDOWN();
}


TEST(frintm) {
  SETUP();

  START();
  __ Fmov(s16, 1.0);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, 1.9);
  __ Fmov(s20, 2.5);
  __ Fmov(s21, -1.5);
  __ Fmov(s22, -2.5);
  __ Fmov(s23, kFP32PositiveInfinity);
  __ Fmov(s24, kFP32NegativeInfinity);
  __ Fmov(s25, 0.0);
  __ Fmov(s26, -0.0);
  __ Fmov(s27, -0.2);

  __ Frintm(s0, s16);
  __ Frintm(s1, s17);
  __ Frintm(s2, s18);
  __ Frintm(s3, s19);
  __ Frintm(s4, s20);
  __ Frintm(s5, s21);
  __ Frintm(s6, s22);
  __ Frintm(s7, s23);
  __ Frintm(s8, s24);
  __ Frintm(s9, s25);
  __ Frintm(s10, s26);
  __ Frintm(s11, s27);

  __ Fmov(d16, 1.0);
  __ Fmov(d17, 1.1);
  __ Fmov(d18, 1.5);
  __ Fmov(d19, 1.9);
  __ Fmov(d20, 2.5);
  __ Fmov(d21, -1.5);
  __ Fmov(d22, -2.5);
  __ Fmov(d23, kFP32PositiveInfinity);
  __ Fmov(d24, kFP32NegativeInfinity);
  __ Fmov(d25, 0.0);
  __ Fmov(d26, -0.0);
  __ Fmov(d27, -0.2);

  __ Frintm(d12, d16);
  __ Frintm(d13, d17);
  __ Frintm(d14, d18);
  __ Frintm(d15, d19);
  __ Frintm(d16, d20);
  __ Frintm(d17, d21);
  __ Frintm(d18, d22);
  __ Frintm(d19, d23);
  __ Frintm(d20, d24);
  __ Frintm(d21, d25);
  __ Frintm(d22, d26);
  __ Frintm(d23, d27);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s0);
  ASSERT_EQUAL_FP32(1.0, s1);
  ASSERT_EQUAL_FP32(1.0, s2);
  ASSERT_EQUAL_FP32(1.0, s3);
  ASSERT_EQUAL_FP32(2.0, s4);
  ASSERT_EQUAL_FP32(-2.0, s5);
  ASSERT_EQUAL_FP32(-3.0, s6);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s7);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s8);
  ASSERT_EQUAL_FP32(0.0, s9);
  ASSERT_EQUAL_FP32(-0.0, s10);
  ASSERT_EQUAL_FP32(-1.0, s11);
  ASSERT_EQUAL_FP64(1.0, d12);
  ASSERT_EQUAL_FP64(1.0, d13);
  ASSERT_EQUAL_FP64(1.0, d14);
  ASSERT_EQUAL_FP64(1.0, d15);
  ASSERT_EQUAL_FP64(2.0, d16);
  ASSERT_EQUAL_FP64(-2.0, d17);
  ASSERT_EQUAL_FP64(-3.0, d18);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d19);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d20);
  ASSERT_EQUAL_FP64(0.0, d21);
  ASSERT_EQUAL_FP64(-0.0, d22);
  ASSERT_EQUAL_FP64(-1.0, d23);

  TEARDOWN();
}


TEST(frintn) {
  SETUP();

  START();
  __ Fmov(s16, 1.0);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, 1.9);
  __ Fmov(s20, 2.5);
  __ Fmov(s21, -1.5);
  __ Fmov(s22, -2.5);
  __ Fmov(s23, kFP32PositiveInfinity);
  __ Fmov(s24, kFP32NegativeInfinity);
  __ Fmov(s25, 0.0);
  __ Fmov(s26, -0.0);
  __ Fmov(s27, -0.2);

  __ Frintn(s0, s16);
  __ Frintn(s1, s17);
  __ Frintn(s2, s18);
  __ Frintn(s3, s19);
  __ Frintn(s4, s20);
  __ Frintn(s5, s21);
  __ Frintn(s6, s22);
  __ Frintn(s7, s23);
  __ Frintn(s8, s24);
  __ Frintn(s9, s25);
  __ Frintn(s10, s26);
  __ Frintn(s11, s27);

  __ Fmov(d16, 1.0);
  __ Fmov(d17, 1.1);
  __ Fmov(d18, 1.5);
  __ Fmov(d19, 1.9);
  __ Fmov(d20, 2.5);
  __ Fmov(d21, -1.5);
  __ Fmov(d22, -2.5);
  __ Fmov(d23, kFP32PositiveInfinity);
  __ Fmov(d24, kFP32NegativeInfinity);
  __ Fmov(d25, 0.0);
  __ Fmov(d26, -0.0);
  __ Fmov(d27, -0.2);

  __ Frintn(d12, d16);
  __ Frintn(d13, d17);
  __ Frintn(d14, d18);
  __ Frintn(d15, d19);
  __ Frintn(d16, d20);
  __ Frintn(d17, d21);
  __ Frintn(d18, d22);
  __ Frintn(d19, d23);
  __ Frintn(d20, d24);
  __ Frintn(d21, d25);
  __ Frintn(d22, d26);
  __ Frintn(d23, d27);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s0);
  ASSERT_EQUAL_FP32(1.0, s1);
  ASSERT_EQUAL_FP32(2.0, s2);
  ASSERT_EQUAL_FP32(2.0, s3);
  ASSERT_EQUAL_FP32(2.0, s4);
  ASSERT_EQUAL_FP32(-2.0, s5);
  ASSERT_EQUAL_FP32(-2.0, s6);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s7);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s8);
  ASSERT_EQUAL_FP32(0.0, s9);
  ASSERT_EQUAL_FP32(-0.0, s10);
  ASSERT_EQUAL_FP32(-0.0, s11);
  ASSERT_EQUAL_FP64(1.0, d12);
  ASSERT_EQUAL_FP64(1.0, d13);
  ASSERT_EQUAL_FP64(2.0, d14);
  ASSERT_EQUAL_FP64(2.0, d15);
  ASSERT_EQUAL_FP64(2.0, d16);
  ASSERT_EQUAL_FP64(-2.0, d17);
  ASSERT_EQUAL_FP64(-2.0, d18);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d19);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d20);
  ASSERT_EQUAL_FP64(0.0, d21);
  ASSERT_EQUAL_FP64(-0.0, d22);
  ASSERT_EQUAL_FP64(-0.0, d23);

  TEARDOWN();
}


TEST(frintp) {
  SETUP();

  START();
  __ Fmov(s16, 1.0);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, 1.9);
  __ Fmov(s20, 2.5);
  __ Fmov(s21, -1.5);
  __ Fmov(s22, -2.5);
  __ Fmov(s23, kFP32PositiveInfinity);
  __ Fmov(s24, kFP32NegativeInfinity);
  __ Fmov(s25, 0.0);
  __ Fmov(s26, -0.0);
  __ Fmov(s27, -0.2);

  __ Frintp(s0, s16);
  __ Frintp(s1, s17);
  __ Frintp(s2, s18);
  __ Frintp(s3, s19);
  __ Frintp(s4, s20);
  __ Frintp(s5, s21);
  __ Frintp(s6, s22);
  __ Frintp(s7, s23);
  __ Frintp(s8, s24);
  __ Frintp(s9, s25);
  __ Frintp(s10, s26);
  __ Frintp(s11, s27);

  __ Fmov(d16, 1.0);
  __ Fmov(d17, 1.1);
  __ Fmov(d18, 1.5);
  __ Fmov(d19, 1.9);
  __ Fmov(d20, 2.5);
  __ Fmov(d21, -1.5);
  __ Fmov(d22, -2.5);
  __ Fmov(d23, kFP32PositiveInfinity);
  __ Fmov(d24, kFP32NegativeInfinity);
  __ Fmov(d25, 0.0);
  __ Fmov(d26, -0.0);
  __ Fmov(d27, -0.2);

  __ Frintp(d12, d16);
  __ Frintp(d13, d17);
  __ Frintp(d14, d18);
  __ Frintp(d15, d19);
  __ Frintp(d16, d20);
  __ Frintp(d17, d21);
  __ Frintp(d18, d22);
  __ Frintp(d19, d23);
  __ Frintp(d20, d24);
  __ Frintp(d21, d25);
  __ Frintp(d22, d26);
  __ Frintp(d23, d27);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s0);
  ASSERT_EQUAL_FP32(2.0, s1);
  ASSERT_EQUAL_FP32(2.0, s2);
  ASSERT_EQUAL_FP32(2.0, s3);
  ASSERT_EQUAL_FP32(3.0, s4);
  ASSERT_EQUAL_FP32(-1.0, s5);
  ASSERT_EQUAL_FP32(-2.0, s6);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s7);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s8);
  ASSERT_EQUAL_FP32(0.0, s9);
  ASSERT_EQUAL_FP32(-0.0, s10);
  ASSERT_EQUAL_FP32(-0.0, s11);
  ASSERT_EQUAL_FP64(1.0, d12);
  ASSERT_EQUAL_FP64(2.0, d13);
  ASSERT_EQUAL_FP64(2.0, d14);
  ASSERT_EQUAL_FP64(2.0, d15);
  ASSERT_EQUAL_FP64(3.0, d16);
  ASSERT_EQUAL_FP64(-1.0, d17);
  ASSERT_EQUAL_FP64(-2.0, d18);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d19);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d20);
  ASSERT_EQUAL_FP64(0.0, d21);
  ASSERT_EQUAL_FP64(-0.0, d22);
  ASSERT_EQUAL_FP64(-0.0, d23);

  TEARDOWN();
}


TEST(frintx) {
  // VIXL only supports the round-to-nearest FPCR mode, and it doesn't support
  // FP exceptions, so this test has the same results as frintn (and frinti).
  SETUP();

  START();
  __ Fmov(s16, 1.0);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, 1.9);
  __ Fmov(s20, 2.5);
  __ Fmov(s21, -1.5);
  __ Fmov(s22, -2.5);
  __ Fmov(s23, kFP32PositiveInfinity);
  __ Fmov(s24, kFP32NegativeInfinity);
  __ Fmov(s25, 0.0);
  __ Fmov(s26, -0.0);
  __ Fmov(s27, -0.2);

  __ Frintx(s0, s16);
  __ Frintx(s1, s17);
  __ Frintx(s2, s18);
  __ Frintx(s3, s19);
  __ Frintx(s4, s20);
  __ Frintx(s5, s21);
  __ Frintx(s6, s22);
  __ Frintx(s7, s23);
  __ Frintx(s8, s24);
  __ Frintx(s9, s25);
  __ Frintx(s10, s26);
  __ Frintx(s11, s27);

  __ Fmov(d16, 1.0);
  __ Fmov(d17, 1.1);
  __ Fmov(d18, 1.5);
  __ Fmov(d19, 1.9);
  __ Fmov(d20, 2.5);
  __ Fmov(d21, -1.5);
  __ Fmov(d22, -2.5);
  __ Fmov(d23, kFP32PositiveInfinity);
  __ Fmov(d24, kFP32NegativeInfinity);
  __ Fmov(d25, 0.0);
  __ Fmov(d26, -0.0);
  __ Fmov(d27, -0.2);

  __ Frintx(d12, d16);
  __ Frintx(d13, d17);
  __ Frintx(d14, d18);
  __ Frintx(d15, d19);
  __ Frintx(d16, d20);
  __ Frintx(d17, d21);
  __ Frintx(d18, d22);
  __ Frintx(d19, d23);
  __ Frintx(d20, d24);
  __ Frintx(d21, d25);
  __ Frintx(d22, d26);
  __ Frintx(d23, d27);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s0);
  ASSERT_EQUAL_FP32(1.0, s1);
  ASSERT_EQUAL_FP32(2.0, s2);
  ASSERT_EQUAL_FP32(2.0, s3);
  ASSERT_EQUAL_FP32(2.0, s4);
  ASSERT_EQUAL_FP32(-2.0, s5);
  ASSERT_EQUAL_FP32(-2.0, s6);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s7);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s8);
  ASSERT_EQUAL_FP32(0.0, s9);
  ASSERT_EQUAL_FP32(-0.0, s10);
  ASSERT_EQUAL_FP32(-0.0, s11);
  ASSERT_EQUAL_FP64(1.0, d12);
  ASSERT_EQUAL_FP64(1.0, d13);
  ASSERT_EQUAL_FP64(2.0, d14);
  ASSERT_EQUAL_FP64(2.0, d15);
  ASSERT_EQUAL_FP64(2.0, d16);
  ASSERT_EQUAL_FP64(-2.0, d17);
  ASSERT_EQUAL_FP64(-2.0, d18);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d19);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d20);
  ASSERT_EQUAL_FP64(0.0, d21);
  ASSERT_EQUAL_FP64(-0.0, d22);
  ASSERT_EQUAL_FP64(-0.0, d23);

  TEARDOWN();
}


TEST(frintz) {
  SETUP();

  START();
  __ Fmov(s16, 1.0);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, 1.9);
  __ Fmov(s20, 2.5);
  __ Fmov(s21, -1.5);
  __ Fmov(s22, -2.5);
  __ Fmov(s23, kFP32PositiveInfinity);
  __ Fmov(s24, kFP32NegativeInfinity);
  __ Fmov(s25, 0.0);
  __ Fmov(s26, -0.0);

  __ Frintz(s0, s16);
  __ Frintz(s1, s17);
  __ Frintz(s2, s18);
  __ Frintz(s3, s19);
  __ Frintz(s4, s20);
  __ Frintz(s5, s21);
  __ Frintz(s6, s22);
  __ Frintz(s7, s23);
  __ Frintz(s8, s24);
  __ Frintz(s9, s25);
  __ Frintz(s10, s26);

  __ Fmov(d16, 1.0);
  __ Fmov(d17, 1.1);
  __ Fmov(d18, 1.5);
  __ Fmov(d19, 1.9);
  __ Fmov(d20, 2.5);
  __ Fmov(d21, -1.5);
  __ Fmov(d22, -2.5);
  __ Fmov(d23, kFP32PositiveInfinity);
  __ Fmov(d24, kFP32NegativeInfinity);
  __ Fmov(d25, 0.0);
  __ Fmov(d26, -0.0);

  __ Frintz(d11, d16);
  __ Frintz(d12, d17);
  __ Frintz(d13, d18);
  __ Frintz(d14, d19);
  __ Frintz(d15, d20);
  __ Frintz(d16, d21);
  __ Frintz(d17, d22);
  __ Frintz(d18, d23);
  __ Frintz(d19, d24);
  __ Frintz(d20, d25);
  __ Frintz(d21, d26);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s0);
  ASSERT_EQUAL_FP32(1.0, s1);
  ASSERT_EQUAL_FP32(1.0, s2);
  ASSERT_EQUAL_FP32(1.0, s3);
  ASSERT_EQUAL_FP32(2.0, s4);
  ASSERT_EQUAL_FP32(-1.0, s5);
  ASSERT_EQUAL_FP32(-2.0, s6);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s7);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s8);
  ASSERT_EQUAL_FP32(0.0, s9);
  ASSERT_EQUAL_FP32(-0.0, s10);
  ASSERT_EQUAL_FP64(1.0, d11);
  ASSERT_EQUAL_FP64(1.0, d12);
  ASSERT_EQUAL_FP64(1.0, d13);
  ASSERT_EQUAL_FP64(1.0, d14);
  ASSERT_EQUAL_FP64(2.0, d15);
  ASSERT_EQUAL_FP64(-1.0, d16);
  ASSERT_EQUAL_FP64(-2.0, d17);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d18);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d19);
  ASSERT_EQUAL_FP64(0.0, d20);
  ASSERT_EQUAL_FP64(-0.0, d21);

  TEARDOWN();
}


TEST(fcvt_ds) {
  SETUP();

  START();
  __ Fmov(s16, 1.0);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, 1.9);
  __ Fmov(s20, 2.5);
  __ Fmov(s21, -1.5);
  __ Fmov(s22, -2.5);
  __ Fmov(s23, kFP32PositiveInfinity);
  __ Fmov(s24, kFP32NegativeInfinity);
  __ Fmov(s25, 0.0);
  __ Fmov(s26, -0.0);
  __ Fmov(s27, FLT_MAX);
  __ Fmov(s28, FLT_MIN);
  __ Fmov(s29, rawbits_to_float(0x7fc12345));   // Quiet NaN.
  __ Fmov(s30, rawbits_to_float(0x7f812345));   // Signalling NaN.

  __ Fcvt(d0, s16);
  __ Fcvt(d1, s17);
  __ Fcvt(d2, s18);
  __ Fcvt(d3, s19);
  __ Fcvt(d4, s20);
  __ Fcvt(d5, s21);
  __ Fcvt(d6, s22);
  __ Fcvt(d7, s23);
  __ Fcvt(d8, s24);
  __ Fcvt(d9, s25);
  __ Fcvt(d10, s26);
  __ Fcvt(d11, s27);
  __ Fcvt(d12, s28);
  __ Fcvt(d13, s29);
  __ Fcvt(d14, s30);
  END();

  RUN();

  ASSERT_EQUAL_FP64(1.0f, d0);
  ASSERT_EQUAL_FP64(1.1f, d1);
  ASSERT_EQUAL_FP64(1.5f, d2);
  ASSERT_EQUAL_FP64(1.9f, d3);
  ASSERT_EQUAL_FP64(2.5f, d4);
  ASSERT_EQUAL_FP64(-1.5f, d5);
  ASSERT_EQUAL_FP64(-2.5f, d6);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d7);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d8);
  ASSERT_EQUAL_FP64(0.0f, d9);
  ASSERT_EQUAL_FP64(-0.0f, d10);
  ASSERT_EQUAL_FP64(FLT_MAX, d11);
  ASSERT_EQUAL_FP64(FLT_MIN, d12);

  // Check that the NaN payload is preserved according to A64 conversion rules:
  //  - The sign bit is preserved.
  //  - The top bit of the mantissa is forced to 1 (making it a quiet NaN).
  //  - The remaining mantissa bits are copied until they run out.
  //  - The low-order bits that haven't already been assigned are set to 0.
  ASSERT_EQUAL_FP64(rawbits_to_double(0x7ff82468a0000000), d13);
  ASSERT_EQUAL_FP64(rawbits_to_double(0x7ff82468a0000000), d14);

  TEARDOWN();
}


TEST(fcvt_sd) {
  // Test simple conversions here. Complex behaviour (such as rounding
  // specifics) are tested in the simulator tests.

  SETUP();

  START();
  __ Fmov(d16, 1.0);
  __ Fmov(d17, 1.1);
  __ Fmov(d18, 1.5);
  __ Fmov(d19, 1.9);
  __ Fmov(d20, 2.5);
  __ Fmov(d21, -1.5);
  __ Fmov(d22, -2.5);
  __ Fmov(d23, kFP32PositiveInfinity);
  __ Fmov(d24, kFP32NegativeInfinity);
  __ Fmov(d25, 0.0);
  __ Fmov(d26, -0.0);
  __ Fmov(d27, FLT_MAX);
  __ Fmov(d28, FLT_MIN);
  __ Fmov(d29, rawbits_to_double(0x7ff82468a0000000));   // Quiet NaN.
  __ Fmov(d30, rawbits_to_double(0x7ff02468a0000000));   // Signalling NaN.

  __ Fcvt(s0, d16);
  __ Fcvt(s1, d17);
  __ Fcvt(s2, d18);
  __ Fcvt(s3, d19);
  __ Fcvt(s4, d20);
  __ Fcvt(s5, d21);
  __ Fcvt(s6, d22);
  __ Fcvt(s7, d23);
  __ Fcvt(s8, d24);
  __ Fcvt(s9, d25);
  __ Fcvt(s10, d26);
  __ Fcvt(s11, d27);
  __ Fcvt(s12, d28);
  __ Fcvt(s13, d29);
  __ Fcvt(s14, d30);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0f, s0);
  ASSERT_EQUAL_FP32(1.1f, s1);
  ASSERT_EQUAL_FP32(1.5f, s2);
  ASSERT_EQUAL_FP32(1.9f, s3);
  ASSERT_EQUAL_FP32(2.5f, s4);
  ASSERT_EQUAL_FP32(-1.5f, s5);
  ASSERT_EQUAL_FP32(-2.5f, s6);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s7);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s8);
  ASSERT_EQUAL_FP32(0.0f, s9);
  ASSERT_EQUAL_FP32(-0.0f, s10);
  ASSERT_EQUAL_FP32(FLT_MAX, s11);
  ASSERT_EQUAL_FP32(FLT_MIN, s12);

  // Check that the NaN payload is preserved according to A64 conversion rules:
  //  - The sign bit is preserved.
  //  - The top bit of the mantissa is forced to 1 (making it a quiet NaN).
  //  - The remaining mantissa bits are copied until they run out.
  //  - The low-order bits that haven't already been assigned are set to 0.
  ASSERT_EQUAL_FP32(rawbits_to_float(0x7fc12345), s13);
  ASSERT_EQUAL_FP32(rawbits_to_float(0x7fc12345), s14);

  TEARDOWN();
}


TEST(fcvt_half) {
  SETUP();

  START();
  Label done;
  {
    // Check all exact conversions from half to float and back.
    Label ok, fail;
    __ Mov(w0, 0);
    for (int i = 0; i < 0xffff; i += 3) {
      if ((i & 0x7c00) == 0x7c00) continue;
      __ Mov(w1, i);
      __ Fmov(s1, w1);
      __ Fcvt(s2, h1);
      __ Fcvt(h2, s2);
      __ Fmov(w2, s2);
      __ Cmp(w1, w2);
      __ B(&fail, ne);
    }
    __ B(&ok);
    __ Bind(&fail);
    __ Mov(w0, 1);
    __ B(&done);
    __ Bind(&ok);
  }
  {
    // Check all exact conversions from half to double and back.
    Label ok, fail;
    for (int i = 0; i < 0xffff; i += 3) {
      if ((i & 0x7c00) == 0x7c00) continue;
      __ Mov(w1, i);
      __ Fmov(s1, w1);
      __ Fcvt(d2, h1);
      __ Fcvt(h2, d2);
      __ Mov(w2, v2.S(), 0);
      __ Cmp(w1, w2);
      __ B(&fail, ne);
    }
    __ B(&ok);
    __ Bind(&fail);
    __ Mov(w0, 2);
    __ Bind(&ok);
  }
  __ Bind(&done);

  // Check some other interesting values.
  __ Fmov(s0, kFP32PositiveInfinity);
  __ Fmov(s1, kFP32NegativeInfinity);
  __ Fmov(s2, 65504);       // Max half precision.
  __ Fmov(s3, 6.10352e-5);  // Min positive normal.
  __ Fmov(s4, 6.09756e-5);  // Max subnormal.
  __ Fmov(s5, 5.96046e-8);  // Min positive subnormal.
  __ Fmov(s6, 5e-9);        // Not representable -> zero.
  __ Fmov(s7, -0.0);
  __ Fcvt(h0, s0);
  __ Fcvt(h1, s1);
  __ Fcvt(h2, s2);
  __ Fcvt(h3, s3);
  __ Fcvt(h4, s4);
  __ Fcvt(h5, s5);
  __ Fcvt(h6, s6);
  __ Fcvt(h7, s7);

  __ Fmov(d20, kFP64PositiveInfinity);
  __ Fmov(d21, kFP64NegativeInfinity);
  __ Fmov(d22, 65504);       // Max half precision.
  __ Fmov(d23, 6.10352e-5);  // Min positive normal.
  __ Fmov(d24, 6.09756e-5);  // Max subnormal.
  __ Fmov(d25, 5.96046e-8);  // Min positive subnormal.
  __ Fmov(d26, 5e-9);        // Not representable -> zero.
  __ Fmov(d27, -0.0);
  __ Fcvt(h20, d20);
  __ Fcvt(h21, d21);
  __ Fcvt(h22, d22);
  __ Fcvt(h23, d23);
  __ Fcvt(h24, d24);
  __ Fcvt(h25, d25);
  __ Fcvt(h26, d26);
  __ Fcvt(h27, d27);
  END();

  RUN();

  ASSERT_EQUAL_32(0, w0);  // 1 => float failed, 2 => double failed.
  ASSERT_EQUAL_128(0, kFP16PositiveInfinity, q0);
  ASSERT_EQUAL_128(0, kFP16NegativeInfinity, q1);
  ASSERT_EQUAL_128(0, 0x7bff, q2);
  ASSERT_EQUAL_128(0, 0x0400, q3);
  ASSERT_EQUAL_128(0, 0x03ff, q4);
  ASSERT_EQUAL_128(0, 0x0001, q5);
  ASSERT_EQUAL_128(0, 0, q6);
  ASSERT_EQUAL_128(0, 0x8000, q7);
  ASSERT_EQUAL_128(0, kFP16PositiveInfinity, q20);
  ASSERT_EQUAL_128(0, kFP16NegativeInfinity, q21);
  ASSERT_EQUAL_128(0, 0x7bff, q22);
  ASSERT_EQUAL_128(0, 0x0400, q23);
  ASSERT_EQUAL_128(0, 0x03ff, q24);
  ASSERT_EQUAL_128(0, 0x0001, q25);
  ASSERT_EQUAL_128(0, 0, q26);
  ASSERT_EQUAL_128(0, 0x8000, q27);
  TEARDOWN();
}


TEST(fcvtas) {
  SETUP();

  START();
  __ Fmov(s0, 1.0);
  __ Fmov(s1, 1.1);
  __ Fmov(s2, 2.5);
  __ Fmov(s3, -2.5);
  __ Fmov(s4, kFP32PositiveInfinity);
  __ Fmov(s5, kFP32NegativeInfinity);
  __ Fmov(s6, 0x7fffff80);  // Largest float < INT32_MAX.
  __ Fneg(s7, s6);          // Smallest float > INT32_MIN.
  __ Fmov(d8, 1.0);
  __ Fmov(d9, 1.1);
  __ Fmov(d10, 2.5);
  __ Fmov(d11, -2.5);
  __ Fmov(d12, kFP64PositiveInfinity);
  __ Fmov(d13, kFP64NegativeInfinity);
  __ Fmov(d14, kWMaxInt - 1);
  __ Fmov(d15, kWMinInt + 1);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 2.5);
  __ Fmov(s19, -2.5);
  __ Fmov(s20, kFP32PositiveInfinity);
  __ Fmov(s21, kFP32NegativeInfinity);
  __ Fmov(s22, 0x7fffff8000000000);     // Largest float < INT64_MAX.
  __ Fneg(s23, s22);                    // Smallest float > INT64_MIN.
  __ Fmov(d24, 1.1);
  __ Fmov(d25, 2.5);
  __ Fmov(d26, -2.5);
  __ Fmov(d27, kFP64PositiveInfinity);
  __ Fmov(d28, kFP64NegativeInfinity);
  __ Fmov(d29, 0x7ffffffffffffc00);     // Largest double < INT64_MAX.
  __ Fneg(d30, d29);                    // Smallest double > INT64_MIN.

  __ Fcvtas(w0, s0);
  __ Fcvtas(w1, s1);
  __ Fcvtas(w2, s2);
  __ Fcvtas(w3, s3);
  __ Fcvtas(w4, s4);
  __ Fcvtas(w5, s5);
  __ Fcvtas(w6, s6);
  __ Fcvtas(w7, s7);
  __ Fcvtas(w8, d8);
  __ Fcvtas(w9, d9);
  __ Fcvtas(w10, d10);
  __ Fcvtas(w11, d11);
  __ Fcvtas(w12, d12);
  __ Fcvtas(w13, d13);
  __ Fcvtas(w14, d14);
  __ Fcvtas(w15, d15);
  __ Fcvtas(x17, s17);
  __ Fcvtas(x18, s18);
  __ Fcvtas(x19, s19);
  __ Fcvtas(x20, s20);
  __ Fcvtas(x21, s21);
  __ Fcvtas(x22, s22);
  __ Fcvtas(x23, s23);
  __ Fcvtas(x24, d24);
  __ Fcvtas(x25, d25);
  __ Fcvtas(x26, d26);
  __ Fcvtas(x27, d27);
  __ Fcvtas(x28, d28);
  __ Fcvtas(x29, d29);
  __ Fcvtas(x30, d30);
  END();

  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(1, x1);
  ASSERT_EQUAL_64(3, x2);
  ASSERT_EQUAL_64(0xfffffffd, x3);
  ASSERT_EQUAL_64(0x7fffffff, x4);
  ASSERT_EQUAL_64(0x80000000, x5);
  ASSERT_EQUAL_64(0x7fffff80, x6);
  ASSERT_EQUAL_64(0x80000080, x7);
  ASSERT_EQUAL_64(1, x8);
  ASSERT_EQUAL_64(1, x9);
  ASSERT_EQUAL_64(3, x10);
  ASSERT_EQUAL_64(0xfffffffd, x11);
  ASSERT_EQUAL_64(0x7fffffff, x12);
  ASSERT_EQUAL_64(0x80000000, x13);
  ASSERT_EQUAL_64(0x7ffffffe, x14);
  ASSERT_EQUAL_64(0x80000001, x15);
  ASSERT_EQUAL_64(1, x17);
  ASSERT_EQUAL_64(3, x18);
  ASSERT_EQUAL_64(0xfffffffffffffffd, x19);
  ASSERT_EQUAL_64(0x7fffffffffffffff, x20);
  ASSERT_EQUAL_64(0x8000000000000000, x21);
  ASSERT_EQUAL_64(0x7fffff8000000000, x22);
  ASSERT_EQUAL_64(0x8000008000000000, x23);
  ASSERT_EQUAL_64(1, x24);
  ASSERT_EQUAL_64(3, x25);
  ASSERT_EQUAL_64(0xfffffffffffffffd, x26);
  ASSERT_EQUAL_64(0x7fffffffffffffff, x27);
  ASSERT_EQUAL_64(0x8000000000000000, x28);
  ASSERT_EQUAL_64(0x7ffffffffffffc00, x29);
  ASSERT_EQUAL_64(0x8000000000000400, x30);

  TEARDOWN();
}


TEST(fcvtau) {
  SETUP();

  START();
  __ Fmov(s0, 1.0);
  __ Fmov(s1, 1.1);
  __ Fmov(s2, 2.5);
  __ Fmov(s3, -2.5);
  __ Fmov(s4, kFP32PositiveInfinity);
  __ Fmov(s5, kFP32NegativeInfinity);
  __ Fmov(s6, 0xffffff00);  // Largest float < UINT32_MAX.
  __ Fmov(d8, 1.0);
  __ Fmov(d9, 1.1);
  __ Fmov(d10, 2.5);
  __ Fmov(d11, -2.5);
  __ Fmov(d12, kFP64PositiveInfinity);
  __ Fmov(d13, kFP64NegativeInfinity);
  __ Fmov(d14, 0xfffffffe);
  __ Fmov(s16, 1.0);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 2.5);
  __ Fmov(s19, -2.5);
  __ Fmov(s20, kFP32PositiveInfinity);
  __ Fmov(s21, kFP32NegativeInfinity);
  __ Fmov(s22, 0xffffff0000000000);  // Largest float < UINT64_MAX.
  __ Fmov(d24, 1.1);
  __ Fmov(d25, 2.5);
  __ Fmov(d26, -2.5);
  __ Fmov(d27, kFP64PositiveInfinity);
  __ Fmov(d28, kFP64NegativeInfinity);
  __ Fmov(d29, 0xfffffffffffff800);  // Largest double < UINT64_MAX.
  __ Fmov(s30, 0x100000000);

  __ Fcvtau(w0, s0);
  __ Fcvtau(w1, s1);
  __ Fcvtau(w2, s2);
  __ Fcvtau(w3, s3);
  __ Fcvtau(w4, s4);
  __ Fcvtau(w5, s5);
  __ Fcvtau(w6, s6);
  __ Fcvtau(w8, d8);
  __ Fcvtau(w9, d9);
  __ Fcvtau(w10, d10);
  __ Fcvtau(w11, d11);
  __ Fcvtau(w12, d12);
  __ Fcvtau(w13, d13);
  __ Fcvtau(w14, d14);
  __ Fcvtau(w15, d15);
  __ Fcvtau(x16, s16);
  __ Fcvtau(x17, s17);
  __ Fcvtau(x18, s18);
  __ Fcvtau(x19, s19);
  __ Fcvtau(x20, s20);
  __ Fcvtau(x21, s21);
  __ Fcvtau(x22, s22);
  __ Fcvtau(x24, d24);
  __ Fcvtau(x25, d25);
  __ Fcvtau(x26, d26);
  __ Fcvtau(x27, d27);
  __ Fcvtau(x28, d28);
  __ Fcvtau(x29, d29);
  __ Fcvtau(w30, s30);
  END();

  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(1, x1);
  ASSERT_EQUAL_64(3, x2);
  ASSERT_EQUAL_64(0, x3);
  ASSERT_EQUAL_64(0xffffffff, x4);
  ASSERT_EQUAL_64(0, x5);
  ASSERT_EQUAL_64(0xffffff00, x6);
  ASSERT_EQUAL_64(1, x8);
  ASSERT_EQUAL_64(1, x9);
  ASSERT_EQUAL_64(3, x10);
  ASSERT_EQUAL_64(0, x11);
  ASSERT_EQUAL_64(0xffffffff, x12);
  ASSERT_EQUAL_64(0, x13);
  ASSERT_EQUAL_64(0xfffffffe, x14);
  ASSERT_EQUAL_64(1, x16);
  ASSERT_EQUAL_64(1, x17);
  ASSERT_EQUAL_64(3, x18);
  ASSERT_EQUAL_64(0, x19);
  ASSERT_EQUAL_64(0xffffffffffffffff, x20);
  ASSERT_EQUAL_64(0, x21);
  ASSERT_EQUAL_64(0xffffff0000000000, x22);
  ASSERT_EQUAL_64(1, x24);
  ASSERT_EQUAL_64(3, x25);
  ASSERT_EQUAL_64(0, x26);
  ASSERT_EQUAL_64(0xffffffffffffffff, x27);
  ASSERT_EQUAL_64(0, x28);
  ASSERT_EQUAL_64(0xfffffffffffff800, x29);
  ASSERT_EQUAL_64(0xffffffff, x30);

  TEARDOWN();
}


TEST(fcvtms) {
  SETUP();

  START();
  __ Fmov(s0, 1.0);
  __ Fmov(s1, 1.1);
  __ Fmov(s2, 1.5);
  __ Fmov(s3, -1.5);
  __ Fmov(s4, kFP32PositiveInfinity);
  __ Fmov(s5, kFP32NegativeInfinity);
  __ Fmov(s6, 0x7fffff80);                    // Largest float < INT32_MAX.
  __ Fneg(s7, s6);                            // Smallest float > INT32_MIN.
  __ Fmov(d8, 1.0);
  __ Fmov(d9, 1.1);
  __ Fmov(d10, 1.5);
  __ Fmov(d11, -1.5);
  __ Fmov(d12, kFP64PositiveInfinity);
  __ Fmov(d13, kFP64NegativeInfinity);
  __ Fmov(d14, kWMaxInt - 1);
  __ Fmov(d15, kWMinInt + 1);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, -1.5);
  __ Fmov(s20, kFP32PositiveInfinity);
  __ Fmov(s21, kFP32NegativeInfinity);
  __ Fmov(s22, 0x7fffff8000000000);           // Largest float < INT64_MAX.
  __ Fneg(s23, s22);                          // Smallest float > INT64_MIN.
  __ Fmov(d24, 1.1);
  __ Fmov(d25, 1.5);
  __ Fmov(d26, -1.5);
  __ Fmov(d27, kFP64PositiveInfinity);
  __ Fmov(d28, kFP64NegativeInfinity);
  __ Fmov(d29, 0x7ffffffffffffc00);           // Largest double < INT64_MAX.
  __ Fneg(d30, d29);                          // Smallest double > INT64_MIN.

  __ Fcvtms(w0, s0);
  __ Fcvtms(w1, s1);
  __ Fcvtms(w2, s2);
  __ Fcvtms(w3, s3);
  __ Fcvtms(w4, s4);
  __ Fcvtms(w5, s5);
  __ Fcvtms(w6, s6);
  __ Fcvtms(w7, s7);
  __ Fcvtms(w8, d8);
  __ Fcvtms(w9, d9);
  __ Fcvtms(w10, d10);
  __ Fcvtms(w11, d11);
  __ Fcvtms(w12, d12);
  __ Fcvtms(w13, d13);
  __ Fcvtms(w14, d14);
  __ Fcvtms(w15, d15);
  __ Fcvtms(x17, s17);
  __ Fcvtms(x18, s18);
  __ Fcvtms(x19, s19);
  __ Fcvtms(x20, s20);
  __ Fcvtms(x21, s21);
  __ Fcvtms(x22, s22);
  __ Fcvtms(x23, s23);
  __ Fcvtms(x24, d24);
  __ Fcvtms(x25, d25);
  __ Fcvtms(x26, d26);
  __ Fcvtms(x27, d27);
  __ Fcvtms(x28, d28);
  __ Fcvtms(x29, d29);
  __ Fcvtms(x30, d30);
  END();

  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(1, x1);
  ASSERT_EQUAL_64(1, x2);
  ASSERT_EQUAL_64(0xfffffffe, x3);
  ASSERT_EQUAL_64(0x7fffffff, x4);
  ASSERT_EQUAL_64(0x80000000, x5);
  ASSERT_EQUAL_64(0x7fffff80, x6);
  ASSERT_EQUAL_64(0x80000080, x7);
  ASSERT_EQUAL_64(1, x8);
  ASSERT_EQUAL_64(1, x9);
  ASSERT_EQUAL_64(1, x10);
  ASSERT_EQUAL_64(0xfffffffe, x11);
  ASSERT_EQUAL_64(0x7fffffff, x12);
  ASSERT_EQUAL_64(0x80000000, x13);
  ASSERT_EQUAL_64(0x7ffffffe, x14);
  ASSERT_EQUAL_64(0x80000001, x15);
  ASSERT_EQUAL_64(1, x17);
  ASSERT_EQUAL_64(1, x18);
  ASSERT_EQUAL_64(0xfffffffffffffffe, x19);
  ASSERT_EQUAL_64(0x7fffffffffffffff, x20);
  ASSERT_EQUAL_64(0x8000000000000000, x21);
  ASSERT_EQUAL_64(0x7fffff8000000000, x22);
  ASSERT_EQUAL_64(0x8000008000000000, x23);
  ASSERT_EQUAL_64(1, x24);
  ASSERT_EQUAL_64(1, x25);
  ASSERT_EQUAL_64(0xfffffffffffffffe, x26);
  ASSERT_EQUAL_64(0x7fffffffffffffff, x27);
  ASSERT_EQUAL_64(0x8000000000000000, x28);
  ASSERT_EQUAL_64(0x7ffffffffffffc00, x29);
  ASSERT_EQUAL_64(0x8000000000000400, x30);

  TEARDOWN();
}


TEST(fcvtmu) {
  SETUP();

  START();
  __ Fmov(s0, 1.0);
  __ Fmov(s1, 1.1);
  __ Fmov(s2, 1.5);
  __ Fmov(s3, -1.5);
  __ Fmov(s4, kFP32PositiveInfinity);
  __ Fmov(s5, kFP32NegativeInfinity);
  __ Fmov(s6, 0x7fffff80);                    // Largest float < INT32_MAX.
  __ Fneg(s7, s6);                            // Smallest float > INT32_MIN.
  __ Fmov(d8, 1.0);
  __ Fmov(d9, 1.1);
  __ Fmov(d10, 1.5);
  __ Fmov(d11, -1.5);
  __ Fmov(d12, kFP64PositiveInfinity);
  __ Fmov(d13, kFP64NegativeInfinity);
  __ Fmov(d14, kWMaxInt - 1);
  __ Fmov(d15, kWMinInt + 1);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, -1.5);
  __ Fmov(s20, kFP32PositiveInfinity);
  __ Fmov(s21, kFP32NegativeInfinity);
  __ Fmov(s22, 0x7fffff8000000000);           // Largest float < INT64_MAX.
  __ Fneg(s23, s22);                          // Smallest float > INT64_MIN.
  __ Fmov(d24, 1.1);
  __ Fmov(d25, 1.5);
  __ Fmov(d26, -1.5);
  __ Fmov(d27, kFP64PositiveInfinity);
  __ Fmov(d28, kFP64NegativeInfinity);
  __ Fmov(d29, 0x7ffffffffffffc00);           // Largest double < INT64_MAX.
  __ Fneg(d30, d29);                          // Smallest double > INT64_MIN.

  __ Fcvtmu(w0, s0);
  __ Fcvtmu(w1, s1);
  __ Fcvtmu(w2, s2);
  __ Fcvtmu(w3, s3);
  __ Fcvtmu(w4, s4);
  __ Fcvtmu(w5, s5);
  __ Fcvtmu(w6, s6);
  __ Fcvtmu(w7, s7);
  __ Fcvtmu(w8, d8);
  __ Fcvtmu(w9, d9);
  __ Fcvtmu(w10, d10);
  __ Fcvtmu(w11, d11);
  __ Fcvtmu(w12, d12);
  __ Fcvtmu(w13, d13);
  __ Fcvtmu(w14, d14);
  __ Fcvtmu(x17, s17);
  __ Fcvtmu(x18, s18);
  __ Fcvtmu(x19, s19);
  __ Fcvtmu(x20, s20);
  __ Fcvtmu(x21, s21);
  __ Fcvtmu(x22, s22);
  __ Fcvtmu(x23, s23);
  __ Fcvtmu(x24, d24);
  __ Fcvtmu(x25, d25);
  __ Fcvtmu(x26, d26);
  __ Fcvtmu(x27, d27);
  __ Fcvtmu(x28, d28);
  __ Fcvtmu(x29, d29);
  __ Fcvtmu(x30, d30);
  END();

  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(1, x1);
  ASSERT_EQUAL_64(1, x2);
  ASSERT_EQUAL_64(0, x3);
  ASSERT_EQUAL_64(0xffffffff, x4);
  ASSERT_EQUAL_64(0, x5);
  ASSERT_EQUAL_64(0x7fffff80, x6);
  ASSERT_EQUAL_64(0, x7);
  ASSERT_EQUAL_64(1, x8);
  ASSERT_EQUAL_64(1, x9);
  ASSERT_EQUAL_64(1, x10);
  ASSERT_EQUAL_64(0, x11);
  ASSERT_EQUAL_64(0xffffffff, x12);
  ASSERT_EQUAL_64(0, x13);
  ASSERT_EQUAL_64(0x7ffffffe, x14);
  ASSERT_EQUAL_64(1, x17);
  ASSERT_EQUAL_64(1, x18);
  ASSERT_EQUAL_64(0, x19);
  ASSERT_EQUAL_64(0xffffffffffffffff, x20);
  ASSERT_EQUAL_64(0, x21);
  ASSERT_EQUAL_64(0x7fffff8000000000, x22);
  ASSERT_EQUAL_64(0, x23);
  ASSERT_EQUAL_64(1, x24);
  ASSERT_EQUAL_64(1, x25);
  ASSERT_EQUAL_64(0, x26);
  ASSERT_EQUAL_64(0xffffffffffffffff, x27);
  ASSERT_EQUAL_64(0, x28);
  ASSERT_EQUAL_64(0x7ffffffffffffc00, x29);
  ASSERT_EQUAL_64(0, x30);

  TEARDOWN();
}


TEST(fcvtns) {
  SETUP();

  START();
  __ Fmov(s0, 1.0);
  __ Fmov(s1, 1.1);
  __ Fmov(s2, 1.5);
  __ Fmov(s3, -1.5);
  __ Fmov(s4, kFP32PositiveInfinity);
  __ Fmov(s5, kFP32NegativeInfinity);
  __ Fmov(s6, 0x7fffff80);                    // Largest float < INT32_MAX.
  __ Fneg(s7, s6);                            // Smallest float > INT32_MIN.
  __ Fmov(d8, 1.0);
  __ Fmov(d9, 1.1);
  __ Fmov(d10, 1.5);
  __ Fmov(d11, -1.5);
  __ Fmov(d12, kFP64PositiveInfinity);
  __ Fmov(d13, kFP64NegativeInfinity);
  __ Fmov(d14, kWMaxInt - 1);
  __ Fmov(d15, kWMinInt + 1);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, -1.5);
  __ Fmov(s20, kFP32PositiveInfinity);
  __ Fmov(s21, kFP32NegativeInfinity);
  __ Fmov(s22, 0x7fffff8000000000);           // Largest float < INT64_MAX.
  __ Fneg(s23, s22);                          // Smallest float > INT64_MIN.
  __ Fmov(d24, 1.1);
  __ Fmov(d25, 1.5);
  __ Fmov(d26, -1.5);
  __ Fmov(d27, kFP64PositiveInfinity);
  __ Fmov(d28, kFP64NegativeInfinity);
  __ Fmov(d29, 0x7ffffffffffffc00);           // Largest double < INT64_MAX.
  __ Fneg(d30, d29);                          // Smallest double > INT64_MIN.

  __ Fcvtns(w0, s0);
  __ Fcvtns(w1, s1);
  __ Fcvtns(w2, s2);
  __ Fcvtns(w3, s3);
  __ Fcvtns(w4, s4);
  __ Fcvtns(w5, s5);
  __ Fcvtns(w6, s6);
  __ Fcvtns(w7, s7);
  __ Fcvtns(w8, d8);
  __ Fcvtns(w9, d9);
  __ Fcvtns(w10, d10);
  __ Fcvtns(w11, d11);
  __ Fcvtns(w12, d12);
  __ Fcvtns(w13, d13);
  __ Fcvtns(w14, d14);
  __ Fcvtns(w15, d15);
  __ Fcvtns(x17, s17);
  __ Fcvtns(x18, s18);
  __ Fcvtns(x19, s19);
  __ Fcvtns(x20, s20);
  __ Fcvtns(x21, s21);
  __ Fcvtns(x22, s22);
  __ Fcvtns(x23, s23);
  __ Fcvtns(x24, d24);
  __ Fcvtns(x25, d25);
  __ Fcvtns(x26, d26);
  __ Fcvtns(x27, d27);
  __ Fcvtns(x28, d28);
  __ Fcvtns(x29, d29);
  __ Fcvtns(x30, d30);
  END();

  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(1, x1);
  ASSERT_EQUAL_64(2, x2);
  ASSERT_EQUAL_64(0xfffffffe, x3);
  ASSERT_EQUAL_64(0x7fffffff, x4);
  ASSERT_EQUAL_64(0x80000000, x5);
  ASSERT_EQUAL_64(0x7fffff80, x6);
  ASSERT_EQUAL_64(0x80000080, x7);
  ASSERT_EQUAL_64(1, x8);
  ASSERT_EQUAL_64(1, x9);
  ASSERT_EQUAL_64(2, x10);
  ASSERT_EQUAL_64(0xfffffffe, x11);
  ASSERT_EQUAL_64(0x7fffffff, x12);
  ASSERT_EQUAL_64(0x80000000, x13);
  ASSERT_EQUAL_64(0x7ffffffe, x14);
  ASSERT_EQUAL_64(0x80000001, x15);
  ASSERT_EQUAL_64(1, x17);
  ASSERT_EQUAL_64(2, x18);
  ASSERT_EQUAL_64(0xfffffffffffffffe, x19);
  ASSERT_EQUAL_64(0x7fffffffffffffff, x20);
  ASSERT_EQUAL_64(0x8000000000000000, x21);
  ASSERT_EQUAL_64(0x7fffff8000000000, x22);
  ASSERT_EQUAL_64(0x8000008000000000, x23);
  ASSERT_EQUAL_64(1, x24);
  ASSERT_EQUAL_64(2, x25);
  ASSERT_EQUAL_64(0xfffffffffffffffe, x26);
  ASSERT_EQUAL_64(0x7fffffffffffffff, x27);
  ASSERT_EQUAL_64(0x8000000000000000, x28);
  ASSERT_EQUAL_64(0x7ffffffffffffc00, x29);
  ASSERT_EQUAL_64(0x8000000000000400, x30);

  TEARDOWN();
}


TEST(fcvtnu) {
  SETUP();

  START();
  __ Fmov(s0, 1.0);
  __ Fmov(s1, 1.1);
  __ Fmov(s2, 1.5);
  __ Fmov(s3, -1.5);
  __ Fmov(s4, kFP32PositiveInfinity);
  __ Fmov(s5, kFP32NegativeInfinity);
  __ Fmov(s6, 0xffffff00);  // Largest float < UINT32_MAX.
  __ Fmov(d8, 1.0);
  __ Fmov(d9, 1.1);
  __ Fmov(d10, 1.5);
  __ Fmov(d11, -1.5);
  __ Fmov(d12, kFP64PositiveInfinity);
  __ Fmov(d13, kFP64NegativeInfinity);
  __ Fmov(d14, 0xfffffffe);
  __ Fmov(s16, 1.0);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, -1.5);
  __ Fmov(s20, kFP32PositiveInfinity);
  __ Fmov(s21, kFP32NegativeInfinity);
  __ Fmov(s22, 0xffffff0000000000);  // Largest float < UINT64_MAX.
  __ Fmov(d24, 1.1);
  __ Fmov(d25, 1.5);
  __ Fmov(d26, -1.5);
  __ Fmov(d27, kFP64PositiveInfinity);
  __ Fmov(d28, kFP64NegativeInfinity);
  __ Fmov(d29, 0xfffffffffffff800);  // Largest double < UINT64_MAX.
  __ Fmov(s30, 0x100000000);

  __ Fcvtnu(w0, s0);
  __ Fcvtnu(w1, s1);
  __ Fcvtnu(w2, s2);
  __ Fcvtnu(w3, s3);
  __ Fcvtnu(w4, s4);
  __ Fcvtnu(w5, s5);
  __ Fcvtnu(w6, s6);
  __ Fcvtnu(w8, d8);
  __ Fcvtnu(w9, d9);
  __ Fcvtnu(w10, d10);
  __ Fcvtnu(w11, d11);
  __ Fcvtnu(w12, d12);
  __ Fcvtnu(w13, d13);
  __ Fcvtnu(w14, d14);
  __ Fcvtnu(w15, d15);
  __ Fcvtnu(x16, s16);
  __ Fcvtnu(x17, s17);
  __ Fcvtnu(x18, s18);
  __ Fcvtnu(x19, s19);
  __ Fcvtnu(x20, s20);
  __ Fcvtnu(x21, s21);
  __ Fcvtnu(x22, s22);
  __ Fcvtnu(x24, d24);
  __ Fcvtnu(x25, d25);
  __ Fcvtnu(x26, d26);
  __ Fcvtnu(x27, d27);
  __ Fcvtnu(x28, d28);
  __ Fcvtnu(x29, d29);
  __ Fcvtnu(w30, s30);
  END();

  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(1, x1);
  ASSERT_EQUAL_64(2, x2);
  ASSERT_EQUAL_64(0, x3);
  ASSERT_EQUAL_64(0xffffffff, x4);
  ASSERT_EQUAL_64(0, x5);
  ASSERT_EQUAL_64(0xffffff00, x6);
  ASSERT_EQUAL_64(1, x8);
  ASSERT_EQUAL_64(1, x9);
  ASSERT_EQUAL_64(2, x10);
  ASSERT_EQUAL_64(0, x11);
  ASSERT_EQUAL_64(0xffffffff, x12);
  ASSERT_EQUAL_64(0, x13);
  ASSERT_EQUAL_64(0xfffffffe, x14);
  ASSERT_EQUAL_64(1, x16);
  ASSERT_EQUAL_64(1, x17);
  ASSERT_EQUAL_64(2, x18);
  ASSERT_EQUAL_64(0, x19);
  ASSERT_EQUAL_64(0xffffffffffffffff, x20);
  ASSERT_EQUAL_64(0, x21);
  ASSERT_EQUAL_64(0xffffff0000000000, x22);
  ASSERT_EQUAL_64(1, x24);
  ASSERT_EQUAL_64(2, x25);
  ASSERT_EQUAL_64(0, x26);
  ASSERT_EQUAL_64(0xffffffffffffffff, x27);
  ASSERT_EQUAL_64(0, x28);
  ASSERT_EQUAL_64(0xfffffffffffff800, x29);
  ASSERT_EQUAL_64(0xffffffff, x30);

  TEARDOWN();
}


TEST(fcvtzs) {
  SETUP();

  START();
  __ Fmov(s0, 1.0);
  __ Fmov(s1, 1.1);
  __ Fmov(s2, 1.5);
  __ Fmov(s3, -1.5);
  __ Fmov(s4, kFP32PositiveInfinity);
  __ Fmov(s5, kFP32NegativeInfinity);
  __ Fmov(s6, 0x7fffff80);                    // Largest float < INT32_MAX.
  __ Fneg(s7, s6);                            // Smallest float > INT32_MIN.
  __ Fmov(d8, 1.0);
  __ Fmov(d9, 1.1);
  __ Fmov(d10, 1.5);
  __ Fmov(d11, -1.5);
  __ Fmov(d12, kFP64PositiveInfinity);
  __ Fmov(d13, kFP64NegativeInfinity);
  __ Fmov(d14, kWMaxInt - 1);
  __ Fmov(d15, kWMinInt + 1);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, -1.5);
  __ Fmov(s20, kFP32PositiveInfinity);
  __ Fmov(s21, kFP32NegativeInfinity);
  __ Fmov(s22, 0x7fffff8000000000);           // Largest float < INT64_MAX.
  __ Fneg(s23, s22);                          // Smallest float > INT64_MIN.
  __ Fmov(d24, 1.1);
  __ Fmov(d25, 1.5);
  __ Fmov(d26, -1.5);
  __ Fmov(d27, kFP64PositiveInfinity);
  __ Fmov(d28, kFP64NegativeInfinity);
  __ Fmov(d29, 0x7ffffffffffffc00);           // Largest double < INT64_MAX.
  __ Fneg(d30, d29);                          // Smallest double > INT64_MIN.

  __ Fcvtzs(w0, s0);
  __ Fcvtzs(w1, s1);
  __ Fcvtzs(w2, s2);
  __ Fcvtzs(w3, s3);
  __ Fcvtzs(w4, s4);
  __ Fcvtzs(w5, s5);
  __ Fcvtzs(w6, s6);
  __ Fcvtzs(w7, s7);
  __ Fcvtzs(w8, d8);
  __ Fcvtzs(w9, d9);
  __ Fcvtzs(w10, d10);
  __ Fcvtzs(w11, d11);
  __ Fcvtzs(w12, d12);
  __ Fcvtzs(w13, d13);
  __ Fcvtzs(w14, d14);
  __ Fcvtzs(w15, d15);
  __ Fcvtzs(x17, s17);
  __ Fcvtzs(x18, s18);
  __ Fcvtzs(x19, s19);
  __ Fcvtzs(x20, s20);
  __ Fcvtzs(x21, s21);
  __ Fcvtzs(x22, s22);
  __ Fcvtzs(x23, s23);
  __ Fcvtzs(x24, d24);
  __ Fcvtzs(x25, d25);
  __ Fcvtzs(x26, d26);
  __ Fcvtzs(x27, d27);
  __ Fcvtzs(x28, d28);
  __ Fcvtzs(x29, d29);
  __ Fcvtzs(x30, d30);
  END();

  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(1, x1);
  ASSERT_EQUAL_64(1, x2);
  ASSERT_EQUAL_64(0xffffffff, x3);
  ASSERT_EQUAL_64(0x7fffffff, x4);
  ASSERT_EQUAL_64(0x80000000, x5);
  ASSERT_EQUAL_64(0x7fffff80, x6);
  ASSERT_EQUAL_64(0x80000080, x7);
  ASSERT_EQUAL_64(1, x8);
  ASSERT_EQUAL_64(1, x9);
  ASSERT_EQUAL_64(1, x10);
  ASSERT_EQUAL_64(0xffffffff, x11);
  ASSERT_EQUAL_64(0x7fffffff, x12);
  ASSERT_EQUAL_64(0x80000000, x13);
  ASSERT_EQUAL_64(0x7ffffffe, x14);
  ASSERT_EQUAL_64(0x80000001, x15);
  ASSERT_EQUAL_64(1, x17);
  ASSERT_EQUAL_64(1, x18);
  ASSERT_EQUAL_64(0xffffffffffffffff, x19);
  ASSERT_EQUAL_64(0x7fffffffffffffff, x20);
  ASSERT_EQUAL_64(0x8000000000000000, x21);
  ASSERT_EQUAL_64(0x7fffff8000000000, x22);
  ASSERT_EQUAL_64(0x8000008000000000, x23);
  ASSERT_EQUAL_64(1, x24);
  ASSERT_EQUAL_64(1, x25);
  ASSERT_EQUAL_64(0xffffffffffffffff, x26);
  ASSERT_EQUAL_64(0x7fffffffffffffff, x27);
  ASSERT_EQUAL_64(0x8000000000000000, x28);
  ASSERT_EQUAL_64(0x7ffffffffffffc00, x29);
  ASSERT_EQUAL_64(0x8000000000000400, x30);

  TEARDOWN();
}

TEST(fcvtzu) {
  SETUP();

  START();
  __ Fmov(s0, 1.0);
  __ Fmov(s1, 1.1);
  __ Fmov(s2, 1.5);
  __ Fmov(s3, -1.5);
  __ Fmov(s4, kFP32PositiveInfinity);
  __ Fmov(s5, kFP32NegativeInfinity);
  __ Fmov(s6, 0x7fffff80);                    // Largest float < INT32_MAX.
  __ Fneg(s7, s6);                            // Smallest float > INT32_MIN.
  __ Fmov(d8, 1.0);
  __ Fmov(d9, 1.1);
  __ Fmov(d10, 1.5);
  __ Fmov(d11, -1.5);
  __ Fmov(d12, kFP64PositiveInfinity);
  __ Fmov(d13, kFP64NegativeInfinity);
  __ Fmov(d14, kWMaxInt - 1);
  __ Fmov(d15, kWMinInt + 1);
  __ Fmov(s17, 1.1);
  __ Fmov(s18, 1.5);
  __ Fmov(s19, -1.5);
  __ Fmov(s20, kFP32PositiveInfinity);
  __ Fmov(s21, kFP32NegativeInfinity);
  __ Fmov(s22, 0x7fffff8000000000);           // Largest float < INT64_MAX.
  __ Fneg(s23, s22);                          // Smallest float > INT64_MIN.
  __ Fmov(d24, 1.1);
  __ Fmov(d25, 1.5);
  __ Fmov(d26, -1.5);
  __ Fmov(d27, kFP64PositiveInfinity);
  __ Fmov(d28, kFP64NegativeInfinity);
  __ Fmov(d29, 0x7ffffffffffffc00);           // Largest double < INT64_MAX.
  __ Fneg(d30, d29);                          // Smallest double > INT64_MIN.

  __ Fcvtzu(w0, s0);
  __ Fcvtzu(w1, s1);
  __ Fcvtzu(w2, s2);
  __ Fcvtzu(w3, s3);
  __ Fcvtzu(w4, s4);
  __ Fcvtzu(w5, s5);
  __ Fcvtzu(w6, s6);
  __ Fcvtzu(w7, s7);
  __ Fcvtzu(w8, d8);
  __ Fcvtzu(w9, d9);
  __ Fcvtzu(w10, d10);
  __ Fcvtzu(w11, d11);
  __ Fcvtzu(w12, d12);
  __ Fcvtzu(w13, d13);
  __ Fcvtzu(w14, d14);
  __ Fcvtzu(x17, s17);
  __ Fcvtzu(x18, s18);
  __ Fcvtzu(x19, s19);
  __ Fcvtzu(x20, s20);
  __ Fcvtzu(x21, s21);
  __ Fcvtzu(x22, s22);
  __ Fcvtzu(x23, s23);
  __ Fcvtzu(x24, d24);
  __ Fcvtzu(x25, d25);
  __ Fcvtzu(x26, d26);
  __ Fcvtzu(x27, d27);
  __ Fcvtzu(x28, d28);
  __ Fcvtzu(x29, d29);
  __ Fcvtzu(x30, d30);
  END();

  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(1, x1);
  ASSERT_EQUAL_64(1, x2);
  ASSERT_EQUAL_64(0, x3);
  ASSERT_EQUAL_64(0xffffffff, x4);
  ASSERT_EQUAL_64(0, x5);
  ASSERT_EQUAL_64(0x7fffff80, x6);
  ASSERT_EQUAL_64(0, x7);
  ASSERT_EQUAL_64(1, x8);
  ASSERT_EQUAL_64(1, x9);
  ASSERT_EQUAL_64(1, x10);
  ASSERT_EQUAL_64(0, x11);
  ASSERT_EQUAL_64(0xffffffff, x12);
  ASSERT_EQUAL_64(0, x13);
  ASSERT_EQUAL_64(0x7ffffffe, x14);
  ASSERT_EQUAL_64(1, x17);
  ASSERT_EQUAL_64(1, x18);
  ASSERT_EQUAL_64(0, x19);
  ASSERT_EQUAL_64(0xffffffffffffffff, x20);
  ASSERT_EQUAL_64(0, x21);
  ASSERT_EQUAL_64(0x7fffff8000000000, x22);
  ASSERT_EQUAL_64(0, x23);
  ASSERT_EQUAL_64(1, x24);
  ASSERT_EQUAL_64(1, x25);
  ASSERT_EQUAL_64(0, x26);
  ASSERT_EQUAL_64(0xffffffffffffffff, x27);
  ASSERT_EQUAL_64(0, x28);
  ASSERT_EQUAL_64(0x7ffffffffffffc00, x29);
  ASSERT_EQUAL_64(0, x30);

  TEARDOWN();
}


TEST(neon_fcvtl) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x000080007efffeff, 0x3100b1007c00fc00);
  __ Movi(v1.V2D(), 0x03ff83ff00038003, 0x000180017c01fc01);
  __ Movi(v2.V2D(), 0x3e200000be200000, 0x7f800000ff800000);
  __ Movi(v3.V2D(), 0x0000000080000000, 0x7f8fffffff8fffff);
  __ Movi(v4.V2D(), 0x7fcfffffffcfffff, 0x0000000180000001);
  __ Fcvtl(v16.V4S(), v0.V4H());
  __ Fcvtl2(v17.V4S(), v0.V8H());
  __ Fcvtl(v18.V4S(), v1.V4H());
  __ Fcvtl2(v19.V4S(), v1.V8H());

  __ Fcvtl(v20.V2D(), v2.V2S());
  __ Fcvtl2(v21.V2D(), v2.V4S());
  __ Fcvtl(v22.V2D(), v3.V2S());
  __ Fcvtl2(v23.V2D(), v3.V4S());
  __ Fcvtl(v24.V2D(), v4.V2S());
  __ Fcvtl2(v25.V2D(), v4.V4S());

  END();

  RUN();
  ASSERT_EQUAL_128(0x3e200000be200000, 0x7f800000ff800000, q16);
  ASSERT_EQUAL_128(0x0000000080000000, 0x7fdfe000ffdfe000, q17);
  ASSERT_EQUAL_128(0x33800000b3800000, 0x7fc02000ffc02000, q18);
  ASSERT_EQUAL_128(0x387fc000b87fc000, 0x34400000b4400000, q19);
  ASSERT_EQUAL_128(0x7ff0000000000000, 0xfff0000000000000, q20);
  ASSERT_EQUAL_128(0x3fc4000000000000, 0xbfc4000000000000, q21);
  ASSERT_EQUAL_128(0x7ff9ffffe0000000, 0xfff9ffffe0000000, q22);
  ASSERT_EQUAL_128(0x0000000000000000, 0x8000000000000000, q23);
  ASSERT_EQUAL_128(0x36a0000000000000, 0xb6a0000000000000, q24);
  ASSERT_EQUAL_128(0x7ff9ffffe0000000, 0xfff9ffffe0000000, q25);
  TEARDOWN();
}


TEST(neon_fcvtn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x3e200000be200000, 0x7f800000ff800000);
  __ Movi(v1.V2D(), 0x0000000080000000, 0x7f8fffffff8fffff);
  __ Movi(v2.V2D(), 0x7fcfffffffcfffff, 0x0000000180000001);
  __ Movi(v3.V2D(), 0x3fc4000000000000, 0xbfc4000000000000);
  __ Movi(v4.V2D(), 0x7ff0000000000000, 0xfff0000000000000);
  __ Movi(v5.V2D(), 0x0000000000000000, 0x8000000000000000);
  __ Movi(v6.V2D(), 0x7ff0ffffffffffff, 0xfff0ffffffffffff);
  __ Movi(v7.V2D(), 0x7ff8ffffffffffff, 0xfff8ffffffffffff);
  __ Movi(v8.V2D(), 0x0000000000000001, 0x8000000000000001);

  __ Fcvtn(v16.V4H(), v0.V4S());
  __ Fcvtn2(v16.V8H(), v1.V4S());
  __ Fcvtn(v17.V4H(), v2.V4S());
  __ Fcvtn(v18.V2S(), v3.V2D());
  __ Fcvtn2(v18.V4S(), v4.V2D());
  __ Fcvtn(v19.V2S(), v5.V2D());
  __ Fcvtn2(v19.V4S(), v6.V2D());
  __ Fcvtn(v20.V2S(), v7.V2D());
  __ Fcvtn2(v20.V4S(), v8.V2D());
  END();

  RUN();
  ASSERT_EQUAL_128(0x000080007e7ffe7f, 0x3100b1007c00fc00, q16);
  ASSERT_EQUAL_128(0, 0x7e7ffe7f00008000, q17);
  ASSERT_EQUAL_128(0x7f800000ff800000, 0x3e200000be200000, q18);
  ASSERT_EQUAL_128(0x7fc7ffffffc7ffff, 0x0000000080000000, q19);
  ASSERT_EQUAL_128(0x0000000080000000, 0x7fc7ffffffc7ffff, q20);
  TEARDOWN();
}


TEST(neon_fcvtxn) {
  SETUP();

  START();
  __ Movi(v0.V2D(), 0x3e200000be200000, 0x7f800000ff800000);
  __ Movi(v1.V2D(), 0x0000000080000000, 0x7f8fffffff8fffff);
  __ Movi(v2.V2D(), 0x7fcfffffffcfffff, 0x0000000180000001);
  __ Movi(v3.V2D(), 0x3fc4000000000000, 0xbfc4000000000000);
  __ Movi(v4.V2D(), 0x7ff0000000000000, 0xfff0000000000000);
  __ Movi(v5.V2D(), 0x0000000000000000, 0x8000000000000000);
  __ Movi(v6.V2D(), 0x7ff0ffffffffffff, 0xfff0ffffffffffff);
  __ Movi(v7.V2D(), 0x7ff8ffffffffffff, 0xfff8ffffffffffff);
  __ Movi(v8.V2D(), 0x0000000000000001, 0x8000000000000001);
  __ Movi(v9.V2D(), 0x41ed000000000000, 0x41efffffffefffff);
  __ Fcvtxn(v16.V2S(), v0.V2D());
  __ Fcvtxn2(v16.V4S(), v1.V2D());
  __ Fcvtxn(v17.V2S(), v2.V2D());
  __ Fcvtxn2(v17.V4S(), v3.V2D());
  __ Fcvtxn(v18.V2S(), v4.V2D());
  __ Fcvtxn2(v18.V4S(), v5.V2D());
  __ Fcvtxn(v19.V2S(), v6.V2D());
  __ Fcvtxn2(v19.V4S(), v7.V2D());
  __ Fcvtxn(v20.V2S(), v8.V2D());
  __ Fcvtxn2(v20.V4S(), v9.V2D());
  __ Fcvtxn(s21, d0);
  END();

  RUN();
  ASSERT_EQUAL_128(0x000000017f7fffff, 0x310000057f7fffff, q16);
  ASSERT_EQUAL_128(0x3e200000be200000, 0x7f7fffff00000001, q17);
  ASSERT_EQUAL_128(0x0000000080000000, 0x7f800000ff800000, q18);
  ASSERT_EQUAL_128(0x7fc7ffffffc7ffff, 0x7fc7ffffffc7ffff, q19);
  ASSERT_EQUAL_128(0x4f6800004f7fffff, 0x0000000180000001, q20);
  ASSERT_EQUAL_128(0, 0x7f7fffff, q21);
  TEARDOWN();
}


// Test that scvtf and ucvtf can convert the 64-bit input into the expected
// value. All possible values of 'fbits' are tested. The expected value is
// modified accordingly in each case.
//
// The expected value is specified as the bit encoding of the expected double
// produced by scvtf (expected_scvtf_bits) as well as ucvtf
// (expected_ucvtf_bits).
//
// Where the input value is representable by int32_t or uint32_t, conversions
// from W registers will also be tested.
static void TestUScvtfHelper(uint64_t in,
                             uint64_t expected_scvtf_bits,
                             uint64_t expected_ucvtf_bits) {
  uint64_t u64 = in;
  uint32_t u32 = u64 & 0xffffffff;
  int64_t s64 = static_cast<int64_t>(in);
  int32_t s32 = s64 & 0x7fffffff;

  bool cvtf_s32 = (s64 == s32);
  bool cvtf_u32 = (u64 == u32);

  double results_scvtf_x[65];
  double results_ucvtf_x[65];
  double results_scvtf_w[33];
  double results_ucvtf_w[33];

  SETUP();
  START();

  __ Mov(x0, reinterpret_cast<uintptr_t>(results_scvtf_x));
  __ Mov(x1, reinterpret_cast<uintptr_t>(results_ucvtf_x));
  __ Mov(x2, reinterpret_cast<uintptr_t>(results_scvtf_w));
  __ Mov(x3, reinterpret_cast<uintptr_t>(results_ucvtf_w));

  __ Mov(x10, s64);

  // Corrupt the top word, in case it is accidentally used during W-register
  // conversions.
  __ Mov(x11, 0x5555555555555555);
  __ Bfi(x11, x10, 0, kWRegSize);

  // Test integer conversions.
  __ Scvtf(d0, x10);
  __ Ucvtf(d1, x10);
  __ Scvtf(d2, w11);
  __ Ucvtf(d3, w11);
  __ Str(d0, MemOperand(x0));
  __ Str(d1, MemOperand(x1));
  __ Str(d2, MemOperand(x2));
  __ Str(d3, MemOperand(x3));

  // Test all possible values of fbits.
  for (int fbits = 1; fbits <= 32; fbits++) {
    __ Scvtf(d0, x10, fbits);
    __ Ucvtf(d1, x10, fbits);
    __ Scvtf(d2, w11, fbits);
    __ Ucvtf(d3, w11, fbits);
    __ Str(d0, MemOperand(x0, fbits * kDRegSizeInBytes));
    __ Str(d1, MemOperand(x1, fbits * kDRegSizeInBytes));
    __ Str(d2, MemOperand(x2, fbits * kDRegSizeInBytes));
    __ Str(d3, MemOperand(x3, fbits * kDRegSizeInBytes));
  }

  // Conversions from W registers can only handle fbits values <= 32, so just
  // test conversions from X registers for 32 < fbits <= 64.
  for (int fbits = 33; fbits <= 64; fbits++) {
    __ Scvtf(d0, x10, fbits);
    __ Ucvtf(d1, x10, fbits);
    __ Str(d0, MemOperand(x0, fbits * kDRegSizeInBytes));
    __ Str(d1, MemOperand(x1, fbits * kDRegSizeInBytes));
  }

  END();
  RUN();

  // Check the results.
  double expected_scvtf_base = rawbits_to_double(expected_scvtf_bits);
  double expected_ucvtf_base = rawbits_to_double(expected_ucvtf_bits);

  for (int fbits = 0; fbits <= 32; fbits++) {
    double expected_scvtf = expected_scvtf_base / std::pow(2, fbits);
    double expected_ucvtf = expected_ucvtf_base / std::pow(2, fbits);
    ASSERT_EQUAL_FP64(expected_scvtf, results_scvtf_x[fbits]);
    ASSERT_EQUAL_FP64(expected_ucvtf, results_ucvtf_x[fbits]);
    if (cvtf_s32) ASSERT_EQUAL_FP64(expected_scvtf, results_scvtf_w[fbits]);
    if (cvtf_u32) ASSERT_EQUAL_FP64(expected_ucvtf, results_ucvtf_w[fbits]);
  }
  for (int fbits = 33; fbits <= 64; fbits++) {
    double expected_scvtf = expected_scvtf_base / std::pow(2, fbits);
    double expected_ucvtf = expected_ucvtf_base / std::pow(2, fbits);
    ASSERT_EQUAL_FP64(expected_scvtf, results_scvtf_x[fbits]);
    ASSERT_EQUAL_FP64(expected_ucvtf, results_ucvtf_x[fbits]);
  }

  TEARDOWN();
}


TEST(scvtf_ucvtf_double) {
  // Simple conversions of positive numbers which require no rounding; the
  // results should not depened on the rounding mode, and ucvtf and scvtf should
  // produce the same result.
  TestUScvtfHelper(0x0000000000000000, 0x0000000000000000, 0x0000000000000000);
  TestUScvtfHelper(0x0000000000000001, 0x3ff0000000000000, 0x3ff0000000000000);
  TestUScvtfHelper(0x0000000040000000, 0x41d0000000000000, 0x41d0000000000000);
  TestUScvtfHelper(0x0000000100000000, 0x41f0000000000000, 0x41f0000000000000);
  TestUScvtfHelper(0x4000000000000000, 0x43d0000000000000, 0x43d0000000000000);
  // Test mantissa extremities.
  TestUScvtfHelper(0x4000000000000400, 0x43d0000000000001, 0x43d0000000000001);
  // The largest int32_t that fits in a double.
  TestUScvtfHelper(0x000000007fffffff, 0x41dfffffffc00000, 0x41dfffffffc00000);
  // Values that would be negative if treated as an int32_t.
  TestUScvtfHelper(0x00000000ffffffff, 0x41efffffffe00000, 0x41efffffffe00000);
  TestUScvtfHelper(0x0000000080000000, 0x41e0000000000000, 0x41e0000000000000);
  TestUScvtfHelper(0x0000000080000001, 0x41e0000000200000, 0x41e0000000200000);
  // The largest int64_t that fits in a double.
  TestUScvtfHelper(0x7ffffffffffffc00, 0x43dfffffffffffff, 0x43dfffffffffffff);
  // Check for bit pattern reproduction.
  TestUScvtfHelper(0x0123456789abcde0, 0x43723456789abcde, 0x43723456789abcde);
  TestUScvtfHelper(0x0000000012345678, 0x41b2345678000000, 0x41b2345678000000);

  // Simple conversions of negative int64_t values. These require no rounding,
  // and the results should not depend on the rounding mode.
  TestUScvtfHelper(0xffffffffc0000000, 0xc1d0000000000000, 0x43effffffff80000);
  TestUScvtfHelper(0xffffffff00000000, 0xc1f0000000000000, 0x43efffffffe00000);
  TestUScvtfHelper(0xc000000000000000, 0xc3d0000000000000, 0x43e8000000000000);

  // Conversions which require rounding.
  TestUScvtfHelper(0x1000000000000000, 0x43b0000000000000, 0x43b0000000000000);
  TestUScvtfHelper(0x1000000000000001, 0x43b0000000000000, 0x43b0000000000000);
  TestUScvtfHelper(0x1000000000000080, 0x43b0000000000000, 0x43b0000000000000);
  TestUScvtfHelper(0x1000000000000081, 0x43b0000000000001, 0x43b0000000000001);
  TestUScvtfHelper(0x1000000000000100, 0x43b0000000000001, 0x43b0000000000001);
  TestUScvtfHelper(0x1000000000000101, 0x43b0000000000001, 0x43b0000000000001);
  TestUScvtfHelper(0x1000000000000180, 0x43b0000000000002, 0x43b0000000000002);
  TestUScvtfHelper(0x1000000000000181, 0x43b0000000000002, 0x43b0000000000002);
  TestUScvtfHelper(0x1000000000000200, 0x43b0000000000002, 0x43b0000000000002);
  TestUScvtfHelper(0x1000000000000201, 0x43b0000000000002, 0x43b0000000000002);
  TestUScvtfHelper(0x1000000000000280, 0x43b0000000000002, 0x43b0000000000002);
  TestUScvtfHelper(0x1000000000000281, 0x43b0000000000003, 0x43b0000000000003);
  TestUScvtfHelper(0x1000000000000300, 0x43b0000000000003, 0x43b0000000000003);
  // Check rounding of negative int64_t values (and large uint64_t values).
  TestUScvtfHelper(0x8000000000000000, 0xc3e0000000000000, 0x43e0000000000000);
  TestUScvtfHelper(0x8000000000000001, 0xc3e0000000000000, 0x43e0000000000000);
  TestUScvtfHelper(0x8000000000000200, 0xc3e0000000000000, 0x43e0000000000000);
  TestUScvtfHelper(0x8000000000000201, 0xc3dfffffffffffff, 0x43e0000000000000);
  TestUScvtfHelper(0x8000000000000400, 0xc3dfffffffffffff, 0x43e0000000000000);
  TestUScvtfHelper(0x8000000000000401, 0xc3dfffffffffffff, 0x43e0000000000001);
  TestUScvtfHelper(0x8000000000000600, 0xc3dffffffffffffe, 0x43e0000000000001);
  TestUScvtfHelper(0x8000000000000601, 0xc3dffffffffffffe, 0x43e0000000000001);
  TestUScvtfHelper(0x8000000000000800, 0xc3dffffffffffffe, 0x43e0000000000001);
  TestUScvtfHelper(0x8000000000000801, 0xc3dffffffffffffe, 0x43e0000000000001);
  TestUScvtfHelper(0x8000000000000a00, 0xc3dffffffffffffe, 0x43e0000000000001);
  TestUScvtfHelper(0x8000000000000a01, 0xc3dffffffffffffd, 0x43e0000000000001);
  TestUScvtfHelper(0x8000000000000c00, 0xc3dffffffffffffd, 0x43e0000000000002);
  // Round up to produce a result that's too big for the input to represent.
  TestUScvtfHelper(0x7ffffffffffffe00, 0x43e0000000000000, 0x43e0000000000000);
  TestUScvtfHelper(0x7fffffffffffffff, 0x43e0000000000000, 0x43e0000000000000);
  TestUScvtfHelper(0xfffffffffffffc00, 0xc090000000000000, 0x43f0000000000000);
  TestUScvtfHelper(0xffffffffffffffff, 0xbff0000000000000, 0x43f0000000000000);
}


// The same as TestUScvtfHelper, but convert to floats.
static void TestUScvtf32Helper(uint64_t in,
                               uint32_t expected_scvtf_bits,
                               uint32_t expected_ucvtf_bits) {
  uint64_t u64 = in;
  uint32_t u32 = u64 & 0xffffffff;
  int64_t s64 = static_cast<int64_t>(in);
  int32_t s32 = s64 & 0x7fffffff;

  bool cvtf_s32 = (s64 == s32);
  bool cvtf_u32 = (u64 == u32);

  float results_scvtf_x[65];
  float results_ucvtf_x[65];
  float results_scvtf_w[33];
  float results_ucvtf_w[33];

  SETUP();
  START();

  __ Mov(x0, reinterpret_cast<uintptr_t>(results_scvtf_x));
  __ Mov(x1, reinterpret_cast<uintptr_t>(results_ucvtf_x));
  __ Mov(x2, reinterpret_cast<uintptr_t>(results_scvtf_w));
  __ Mov(x3, reinterpret_cast<uintptr_t>(results_ucvtf_w));

  __ Mov(x10, s64);

  // Corrupt the top word, in case it is accidentally used during W-register
  // conversions.
  __ Mov(x11, 0x5555555555555555);
  __ Bfi(x11, x10, 0, kWRegSize);

  // Test integer conversions.
  __ Scvtf(s0, x10);
  __ Ucvtf(s1, x10);
  __ Scvtf(s2, w11);
  __ Ucvtf(s3, w11);
  __ Str(s0, MemOperand(x0));
  __ Str(s1, MemOperand(x1));
  __ Str(s2, MemOperand(x2));
  __ Str(s3, MemOperand(x3));

  // Test all possible values of fbits.
  for (int fbits = 1; fbits <= 32; fbits++) {
    __ Scvtf(s0, x10, fbits);
    __ Ucvtf(s1, x10, fbits);
    __ Scvtf(s2, w11, fbits);
    __ Ucvtf(s3, w11, fbits);
    __ Str(s0, MemOperand(x0, fbits * kSRegSizeInBytes));
    __ Str(s1, MemOperand(x1, fbits * kSRegSizeInBytes));
    __ Str(s2, MemOperand(x2, fbits * kSRegSizeInBytes));
    __ Str(s3, MemOperand(x3, fbits * kSRegSizeInBytes));
  }

  // Conversions from W registers can only handle fbits values <= 32, so just
  // test conversions from X registers for 32 < fbits <= 64.
  for (int fbits = 33; fbits <= 64; fbits++) {
    __ Scvtf(s0, x10, fbits);
    __ Ucvtf(s1, x10, fbits);
    __ Str(s0, MemOperand(x0, fbits * kSRegSizeInBytes));
    __ Str(s1, MemOperand(x1, fbits * kSRegSizeInBytes));
  }

  END();
  RUN();

  // Check the results.
  float expected_scvtf_base = rawbits_to_float(expected_scvtf_bits);
  float expected_ucvtf_base = rawbits_to_float(expected_ucvtf_bits);

  for (int fbits = 0; fbits <= 32; fbits++) {
    float expected_scvtf = expected_scvtf_base / std::pow(2.0f, fbits);
    float expected_ucvtf = expected_ucvtf_base / std::pow(2.0f, fbits);
    ASSERT_EQUAL_FP32(expected_scvtf, results_scvtf_x[fbits]);
    ASSERT_EQUAL_FP32(expected_ucvtf, results_ucvtf_x[fbits]);
    if (cvtf_s32) ASSERT_EQUAL_FP32(expected_scvtf, results_scvtf_w[fbits]);
    if (cvtf_u32) ASSERT_EQUAL_FP32(expected_ucvtf, results_ucvtf_w[fbits]);
  }
  for (int fbits = 33; fbits <= 64; fbits++) {
    float expected_scvtf = expected_scvtf_base / std::pow(2.0f, fbits);
    float expected_ucvtf = expected_ucvtf_base / std::pow(2.0f, fbits);
    ASSERT_EQUAL_FP32(expected_scvtf, results_scvtf_x[fbits]);
    ASSERT_EQUAL_FP32(expected_ucvtf, results_ucvtf_x[fbits]);
  }

  TEARDOWN();
}


TEST(scvtf_ucvtf_float) {
  // Simple conversions of positive numbers which require no rounding; the
  // results should not depened on the rounding mode, and ucvtf and scvtf should
  // produce the same result.
  TestUScvtf32Helper(0x0000000000000000, 0x00000000, 0x00000000);
  TestUScvtf32Helper(0x0000000000000001, 0x3f800000, 0x3f800000);
  TestUScvtf32Helper(0x0000000040000000, 0x4e800000, 0x4e800000);
  TestUScvtf32Helper(0x0000000100000000, 0x4f800000, 0x4f800000);
  TestUScvtf32Helper(0x4000000000000000, 0x5e800000, 0x5e800000);
  // Test mantissa extremities.
  TestUScvtf32Helper(0x0000000000800001, 0x4b000001, 0x4b000001);
  TestUScvtf32Helper(0x4000008000000000, 0x5e800001, 0x5e800001);
  // The largest int32_t that fits in a float.
  TestUScvtf32Helper(0x000000007fffff80, 0x4effffff, 0x4effffff);
  // Values that would be negative if treated as an int32_t.
  TestUScvtf32Helper(0x00000000ffffff00, 0x4f7fffff, 0x4f7fffff);
  TestUScvtf32Helper(0x0000000080000000, 0x4f000000, 0x4f000000);
  TestUScvtf32Helper(0x0000000080000100, 0x4f000001, 0x4f000001);
  // The largest int64_t that fits in a float.
  TestUScvtf32Helper(0x7fffff8000000000, 0x5effffff, 0x5effffff);
  // Check for bit pattern reproduction.
  TestUScvtf32Helper(0x0000000000876543, 0x4b076543, 0x4b076543);

  // Simple conversions of negative int64_t values. These require no rounding,
  // and the results should not depend on the rounding mode.
  TestUScvtf32Helper(0xfffffc0000000000, 0xd4800000, 0x5f7ffffc);
  TestUScvtf32Helper(0xc000000000000000, 0xde800000, 0x5f400000);

  // Conversions which require rounding.
  TestUScvtf32Helper(0x0000800000000000, 0x57000000, 0x57000000);
  TestUScvtf32Helper(0x0000800000000001, 0x57000000, 0x57000000);
  TestUScvtf32Helper(0x0000800000800000, 0x57000000, 0x57000000);
  TestUScvtf32Helper(0x0000800000800001, 0x57000001, 0x57000001);
  TestUScvtf32Helper(0x0000800001000000, 0x57000001, 0x57000001);
  TestUScvtf32Helper(0x0000800001000001, 0x57000001, 0x57000001);
  TestUScvtf32Helper(0x0000800001800000, 0x57000002, 0x57000002);
  TestUScvtf32Helper(0x0000800001800001, 0x57000002, 0x57000002);
  TestUScvtf32Helper(0x0000800002000000, 0x57000002, 0x57000002);
  TestUScvtf32Helper(0x0000800002000001, 0x57000002, 0x57000002);
  TestUScvtf32Helper(0x0000800002800000, 0x57000002, 0x57000002);
  TestUScvtf32Helper(0x0000800002800001, 0x57000003, 0x57000003);
  TestUScvtf32Helper(0x0000800003000000, 0x57000003, 0x57000003);
  // Check rounding of negative int64_t values (and large uint64_t values).
  TestUScvtf32Helper(0x8000000000000000, 0xdf000000, 0x5f000000);
  TestUScvtf32Helper(0x8000000000000001, 0xdf000000, 0x5f000000);
  TestUScvtf32Helper(0x8000004000000000, 0xdf000000, 0x5f000000);
  TestUScvtf32Helper(0x8000004000000001, 0xdeffffff, 0x5f000000);
  TestUScvtf32Helper(0x8000008000000000, 0xdeffffff, 0x5f000000);
  TestUScvtf32Helper(0x8000008000000001, 0xdeffffff, 0x5f000001);
  TestUScvtf32Helper(0x800000c000000000, 0xdefffffe, 0x5f000001);
  TestUScvtf32Helper(0x800000c000000001, 0xdefffffe, 0x5f000001);
  TestUScvtf32Helper(0x8000010000000000, 0xdefffffe, 0x5f000001);
  TestUScvtf32Helper(0x8000010000000001, 0xdefffffe, 0x5f000001);
  TestUScvtf32Helper(0x8000014000000000, 0xdefffffe, 0x5f000001);
  TestUScvtf32Helper(0x8000014000000001, 0xdefffffd, 0x5f000001);
  TestUScvtf32Helper(0x8000018000000000, 0xdefffffd, 0x5f000002);
  // Round up to produce a result that's too big for the input to represent.
  TestUScvtf32Helper(0x000000007fffffc0, 0x4f000000, 0x4f000000);
  TestUScvtf32Helper(0x000000007fffffff, 0x4f000000, 0x4f000000);
  TestUScvtf32Helper(0x00000000ffffff80, 0x4f800000, 0x4f800000);
  TestUScvtf32Helper(0x00000000ffffffff, 0x4f800000, 0x4f800000);
  TestUScvtf32Helper(0x7fffffc000000000, 0x5f000000, 0x5f000000);
  TestUScvtf32Helper(0x7fffffffffffffff, 0x5f000000, 0x5f000000);
  TestUScvtf32Helper(0xffffff8000000000, 0xd3000000, 0x5f800000);
  TestUScvtf32Helper(0xffffffffffffffff, 0xbf800000, 0x5f800000);
}


TEST(system_mrs) {
  SETUP();

  START();
  __ Mov(w0, 0);
  __ Mov(w1, 1);
  __ Mov(w2, 0x80000000);

  // Set the Z and C flags.
  __ Cmp(w0, w0);
  __ Mrs(x3, NZCV);

  // Set the N flag.
  __ Cmp(w0, w1);
  __ Mrs(x4, NZCV);

  // Set the Z, C and V flags.
  __ Adds(w0, w2, w2);
  __ Mrs(x5, NZCV);

  // Read the default FPCR.
  __ Mrs(x6, FPCR);
  END();

  RUN();

  // NZCV
  ASSERT_EQUAL_32(ZCFlag, w3);
  ASSERT_EQUAL_32(NFlag, w4);
  ASSERT_EQUAL_32(ZCVFlag, w5);

  // FPCR
  // The default FPCR on Linux-based platforms is 0.
  ASSERT_EQUAL_32(0, w6);

  TEARDOWN();
}


TEST(system_msr) {
  // All FPCR fields that must be implemented: AHP, DN, FZ, RMode
  const uint64_t fpcr_core = 0x07c00000;

  // All FPCR fields (including fields which may be read-as-zero):
  //  Stride, Len
  //  IDE, IXE, UFE, OFE, DZE, IOE
  const uint64_t fpcr_all = fpcr_core | 0x00379f00;

  SETUP();

  START();
  __ Mov(w0, 0);
  __ Mov(w1, 0x7fffffff);

  __ Mov(x7, 0);

  __ Mov(x10, NVFlag);
  __ Cmp(w0, w0);     // Set Z and C.
  __ Msr(NZCV, x10);  // Set N and V.
  // The Msr should have overwritten every flag set by the Cmp.
  __ Cinc(x7, x7, mi);  // N
  __ Cinc(x7, x7, ne);  // !Z
  __ Cinc(x7, x7, lo);  // !C
  __ Cinc(x7, x7, vs);  // V

  __ Mov(x10, ZCFlag);
  __ Cmn(w1, w1);     // Set N and V.
  __ Msr(NZCV, x10);  // Set Z and C.
  // The Msr should have overwritten every flag set by the Cmn.
  __ Cinc(x7, x7, pl);  // !N
  __ Cinc(x7, x7, eq);  // Z
  __ Cinc(x7, x7, hs);  // C
  __ Cinc(x7, x7, vc);  // !V

  // All core FPCR fields must be writable.
  __ Mov(x8, fpcr_core);
  __ Msr(FPCR, x8);
  __ Mrs(x8, FPCR);

  // All FPCR fields, including optional ones. This part of the test doesn't
  // achieve much other than ensuring that supported fields can be cleared by
  // the next test.
  __ Mov(x9, fpcr_all);
  __ Msr(FPCR, x9);
  __ Mrs(x9, FPCR);
  __ And(x9, x9, fpcr_core);

  // The undefined bits must ignore writes.
  // It's conceivable that a future version of the architecture could use these
  // fields (making this test fail), but in the meantime this is a useful test
  // for the simulator.
  __ Mov(x10, ~fpcr_all);
  __ Msr(FPCR, x10);
  __ Mrs(x10, FPCR);

  END();

  RUN();

  // We should have incremented x7 (from 0) exactly 8 times.
  ASSERT_EQUAL_64(8, x7);

  ASSERT_EQUAL_64(fpcr_core, x8);
  ASSERT_EQUAL_64(fpcr_core, x9);
  ASSERT_EQUAL_64(0, x10);

  TEARDOWN();
}


TEST(system_nop) {
  SETUP();
  RegisterDump before;

  START();
  before.Dump(&masm);
  __ Nop();
  END();

  RUN();

  ASSERT_EQUAL_REGISTERS(before);
  ASSERT_EQUAL_NZCV(before.flags_nzcv());

  TEARDOWN();
}


TEST(zero_dest) {
  SETUP();
  ALLOW_ASM();
  RegisterDump before;

  START();
  // Preserve the stack pointer, in case we clobber it.
  __ Mov(x30, sp);
  // Initialize the other registers used in this test.
  uint64_t literal_base = 0x0100001000100101;
  __ Mov(x0, 0);
  __ Mov(x1, literal_base);
  for (unsigned i = 2; i < x30.code(); i++) {
    __ Add(Register::XRegFromCode(i), Register::XRegFromCode(i-1), x1);
  }
  before.Dump(&masm);

  // All of these instructions should be NOPs in these forms, but have
  // alternate forms which can write into the stack pointer.
  __ add(xzr, x0, x1);
  __ add(xzr, x1, xzr);
  __ add(xzr, xzr, x1);

  __ and_(xzr, x0, x2);
  __ and_(xzr, x2, xzr);
  __ and_(xzr, xzr, x2);

  __ bic(xzr, x0, x3);
  __ bic(xzr, x3, xzr);
  __ bic(xzr, xzr, x3);

  __ eon(xzr, x0, x4);
  __ eon(xzr, x4, xzr);
  __ eon(xzr, xzr, x4);

  __ eor(xzr, x0, x5);
  __ eor(xzr, x5, xzr);
  __ eor(xzr, xzr, x5);

  __ orr(xzr, x0, x6);
  __ orr(xzr, x6, xzr);
  __ orr(xzr, xzr, x6);

  __ sub(xzr, x0, x7);
  __ sub(xzr, x7, xzr);
  __ sub(xzr, xzr, x7);

  // Swap the saved stack pointer with the real one. If sp was written
  // during the test, it will show up in x30. This is done because the test
  // framework assumes that sp will be valid at the end of the test.
  __ Mov(x29, x30);
  __ Mov(x30, sp);
  __ Mov(sp, x29);
  // We used x29 as a scratch register, so reset it to make sure it doesn't
  // trigger a test failure.
  __ Add(x29, x28, x1);
  END();

  RUN();

  ASSERT_EQUAL_REGISTERS(before);
  ASSERT_EQUAL_NZCV(before.flags_nzcv());

  TEARDOWN();
}


TEST(zero_dest_setflags) {
  SETUP();
  ALLOW_ASM();
  RegisterDump before;

  START();
  // Preserve the stack pointer, in case we clobber it.
  __ Mov(x30, sp);
  // Initialize the other registers used in this test.
  uint64_t literal_base = 0x0100001000100101;
  __ Mov(x0, 0);
  __ Mov(x1, literal_base);
  for (int i = 2; i < 30; i++) {
    __ Add(Register::XRegFromCode(i), Register::XRegFromCode(i-1), x1);
  }
  before.Dump(&masm);

  // All of these instructions should only write to the flags in these forms,
  // but have alternate forms which can write into the stack pointer.
  __ adds(xzr, x0, Operand(x1, UXTX));
  __ adds(xzr, x1, Operand(xzr, UXTX));
  __ adds(xzr, x1, 1234);
  __ adds(xzr, x0, x1);
  __ adds(xzr, x1, xzr);
  __ adds(xzr, xzr, x1);

  __ ands(xzr, x2, ~0xf);
  __ ands(xzr, xzr, ~0xf);
  __ ands(xzr, x0, x2);
  __ ands(xzr, x2, xzr);
  __ ands(xzr, xzr, x2);

  __ bics(xzr, x3, ~0xf);
  __ bics(xzr, xzr, ~0xf);
  __ bics(xzr, x0, x3);
  __ bics(xzr, x3, xzr);
  __ bics(xzr, xzr, x3);

  __ subs(xzr, x0, Operand(x3, UXTX));
  __ subs(xzr, x3, Operand(xzr, UXTX));
  __ subs(xzr, x3, 1234);
  __ subs(xzr, x0, x3);
  __ subs(xzr, x3, xzr);
  __ subs(xzr, xzr, x3);

  // Swap the saved stack pointer with the real one. If sp was written
  // during the test, it will show up in x30. This is done because the test
  // framework assumes that sp will be valid at the end of the test.
  __ Mov(x29, x30);
  __ Mov(x30, sp);
  __ Mov(sp, x29);
  // We used x29 as a scratch register, so reset it to make sure it doesn't
  // trigger a test failure.
  __ Add(x29, x28, x1);
  END();

  RUN();

  ASSERT_EQUAL_REGISTERS(before);

  TEARDOWN();
}


TEST(register_bit) {
  // No code generation takes place in this test, so no need to setup and
  // teardown.

  // Simple tests.
  assert(x0.Bit() == (UINT64_C(1) << 0));
  assert(x1.Bit() == (UINT64_C(1) << 1));
  assert(x10.Bit() == (UINT64_C(1) << 10));

  // AAPCS64 definitions.
  assert(lr.Bit() == (UINT64_C(1) << kLinkRegCode));

  // Fixed (hardware) definitions.
  assert(xzr.Bit() == (UINT64_C(1) << kZeroRegCode));

  // Internal ABI definitions.
  assert(sp.Bit() == (UINT64_C(1) << kSPRegInternalCode));
  assert(sp.Bit() != xzr.Bit());

  // xn.Bit() == wn.Bit() at all times, for the same n.
  assert(x0.Bit() == w0.Bit());
  assert(x1.Bit() == w1.Bit());
  assert(x10.Bit() == w10.Bit());
  assert(xzr.Bit() == wzr.Bit());
  assert(sp.Bit() == wsp.Bit());
}


TEST(stack_pointer_override) {
  // This test generates some stack maintenance code, but the test only checks
  // the reported state.
  SETUP();
  START();

  // The default stack pointer in VIXL is sp.
  assert(sp.Is(__ StackPointer()));
  __ SetStackPointer(x0);
  assert(x0.Is(__ StackPointer()));
  __ SetStackPointer(x28);
  assert(x28.Is(__ StackPointer()));
  __ SetStackPointer(sp);
  assert(sp.Is(__ StackPointer()));

  END();
  RUN();
  TEARDOWN();
}


TEST(peek_poke_simple) {
  SETUP();
  START();

  static const RegList x0_to_x3 = x0.Bit() | x1.Bit() | x2.Bit() | x3.Bit();
  static const RegList x10_to_x13 = x10.Bit() | x11.Bit() |
                                    x12.Bit() | x13.Bit();

  // The literal base is chosen to have two useful properties:
  //  * When multiplied by small values (such as a register index), this value
  //    is clearly readable in the result.
  //  * The value is not formed from repeating fixed-size smaller values, so it
  //    can be used to detect endianness-related errors.
  uint64_t literal_base = 0x0100001000100101;

  // Initialize the registers.
  __ Mov(x0, literal_base);
  __ Add(x1, x0, x0);
  __ Add(x2, x1, x0);
  __ Add(x3, x2, x0);

  __ Claim(32);

  // Simple exchange.
  //  After this test:
  //    x0-x3 should be unchanged.
  //    w10-w13 should contain the lower words of x0-x3.
  __ Poke(x0, 0);
  __ Poke(x1, 8);
  __ Poke(x2, 16);
  __ Poke(x3, 24);
  Clobber(&masm, x0_to_x3);
  __ Peek(x0, 0);
  __ Peek(x1, 8);
  __ Peek(x2, 16);
  __ Peek(x3, 24);

  __ Poke(w0, 0);
  __ Poke(w1, 4);
  __ Poke(w2, 8);
  __ Poke(w3, 12);
  Clobber(&masm, x10_to_x13);
  __ Peek(w10, 0);
  __ Peek(w11, 4);
  __ Peek(w12, 8);
  __ Peek(w13, 12);

  __ Drop(32);

  END();
  RUN();

  ASSERT_EQUAL_64(literal_base * 1, x0);
  ASSERT_EQUAL_64(literal_base * 2, x1);
  ASSERT_EQUAL_64(literal_base * 3, x2);
  ASSERT_EQUAL_64(literal_base * 4, x3);

  ASSERT_EQUAL_64((literal_base * 1) & 0xffffffff, x10);
  ASSERT_EQUAL_64((literal_base * 2) & 0xffffffff, x11);
  ASSERT_EQUAL_64((literal_base * 3) & 0xffffffff, x12);
  ASSERT_EQUAL_64((literal_base * 4) & 0xffffffff, x13);

  TEARDOWN();
}


TEST(peek_poke_unaligned) {
  SETUP();
  START();

  // The literal base is chosen to have two useful properties:
  //  * When multiplied by small values (such as a register index), this value
  //    is clearly readable in the result.
  //  * The value is not formed from repeating fixed-size smaller values, so it
  //    can be used to detect endianness-related errors.
  uint64_t literal_base = 0x0100001000100101;

  // Initialize the registers.
  __ Mov(x0, literal_base);
  __ Add(x1, x0, x0);
  __ Add(x2, x1, x0);
  __ Add(x3, x2, x0);
  __ Add(x4, x3, x0);
  __ Add(x5, x4, x0);
  __ Add(x6, x5, x0);

  __ Claim(32);

  // Unaligned exchanges.
  //  After this test:
  //    x0-x6 should be unchanged.
  //    w10-w12 should contain the lower words of x0-x2.
  __ Poke(x0, 1);
  Clobber(&masm, x0.Bit());
  __ Peek(x0, 1);
  __ Poke(x1, 2);
  Clobber(&masm, x1.Bit());
  __ Peek(x1, 2);
  __ Poke(x2, 3);
  Clobber(&masm, x2.Bit());
  __ Peek(x2, 3);
  __ Poke(x3, 4);
  Clobber(&masm, x3.Bit());
  __ Peek(x3, 4);
  __ Poke(x4, 5);
  Clobber(&masm, x4.Bit());
  __ Peek(x4, 5);
  __ Poke(x5, 6);
  Clobber(&masm, x5.Bit());
  __ Peek(x5, 6);
  __ Poke(x6, 7);
  Clobber(&masm, x6.Bit());
  __ Peek(x6, 7);

  __ Poke(w0, 1);
  Clobber(&masm, w10.Bit());
  __ Peek(w10, 1);
  __ Poke(w1, 2);
  Clobber(&masm, w11.Bit());
  __ Peek(w11, 2);
  __ Poke(w2, 3);
  Clobber(&masm, w12.Bit());
  __ Peek(w12, 3);

  __ Drop(32);

  END();
  RUN();

  ASSERT_EQUAL_64(literal_base * 1, x0);
  ASSERT_EQUAL_64(literal_base * 2, x1);
  ASSERT_EQUAL_64(literal_base * 3, x2);
  ASSERT_EQUAL_64(literal_base * 4, x3);
  ASSERT_EQUAL_64(literal_base * 5, x4);
  ASSERT_EQUAL_64(literal_base * 6, x5);
  ASSERT_EQUAL_64(literal_base * 7, x6);

  ASSERT_EQUAL_64((literal_base * 1) & 0xffffffff, x10);
  ASSERT_EQUAL_64((literal_base * 2) & 0xffffffff, x11);
  ASSERT_EQUAL_64((literal_base * 3) & 0xffffffff, x12);

  TEARDOWN();
}


TEST(peek_poke_endianness) {
  SETUP();
  START();

  // The literal base is chosen to have two useful properties:
  //  * When multiplied by small values (such as a register index), this value
  //    is clearly readable in the result.
  //  * The value is not formed from repeating fixed-size smaller values, so it
  //    can be used to detect endianness-related errors.
  uint64_t literal_base = 0x0100001000100101;

  // Initialize the registers.
  __ Mov(x0, literal_base);
  __ Add(x1, x0, x0);

  __ Claim(32);

  // Endianness tests.
  //  After this section:
  //    x4 should match x0[31:0]:x0[63:32]
  //    w5 should match w1[15:0]:w1[31:16]
  __ Poke(x0, 0);
  __ Poke(x0, 8);
  __ Peek(x4, 4);

  __ Poke(w1, 0);
  __ Poke(w1, 4);
  __ Peek(w5, 2);

  __ Drop(32);

  END();
  RUN();

  uint64_t x0_expected = literal_base * 1;
  uint64_t x1_expected = literal_base * 2;
  uint64_t x4_expected = (x0_expected << 32) | (x0_expected >> 32);
  uint64_t x5_expected = ((x1_expected << 16) & 0xffff0000) |
                         ((x1_expected >> 16) & 0x0000ffff);

  ASSERT_EQUAL_64(x0_expected, x0);
  ASSERT_EQUAL_64(x1_expected, x1);
  ASSERT_EQUAL_64(x4_expected, x4);
  ASSERT_EQUAL_64(x5_expected, x5);

  TEARDOWN();
}


TEST(peek_poke_mixed) {
  SETUP();
  START();

  // Acquire all temps from the MacroAssembler. They are used arbitrarily below.
  UseScratchRegisterScope temps(&masm);
  temps.ExcludeAll();

  // The literal base is chosen to have two useful properties:
  //  * When multiplied by small values (such as a register index), this value
  //    is clearly readable in the result.
  //  * The value is not formed from repeating fixed-size smaller values, so it
  //    can be used to detect endianness-related errors.
  uint64_t literal_base = 0x0100001000100101;

  // Initialize the registers.
  __ Mov(x0, literal_base);
  __ Add(x1, x0, x0);
  __ Add(x2, x1, x0);
  __ Add(x3, x2, x0);

  __ Claim(32);

  // Mix with other stack operations.
  //  After this section:
  //    x0-x3 should be unchanged.
  //    x6 should match x1[31:0]:x0[63:32]
  //    w7 should match x1[15:0]:x0[63:48]
  __ Poke(x1, 8);
  __ Poke(x0, 0);
  {
    VIXL_ASSERT(__ StackPointer().Is(sp));
    __ Mov(x4, __ StackPointer());
    __ SetStackPointer(x4);

    __ Poke(wzr, 0);    // Clobber the space we're about to drop.
    __ Drop(4);
    __ Peek(x6, 0);
    __ Claim(8);
    __ Peek(w7, 10);
    __ Poke(x3, 28);
    __ Poke(xzr, 0);    // Clobber the space we're about to drop.
    __ Drop(8);
    __ Poke(x2, 12);
    __ Push(w0);

    __ Mov(sp, __ StackPointer());
    __ SetStackPointer(sp);
  }

  __ Pop(x0, x1, x2, x3);

  END();
  RUN();

  uint64_t x0_expected = literal_base * 1;
  uint64_t x1_expected = literal_base * 2;
  uint64_t x2_expected = literal_base * 3;
  uint64_t x3_expected = literal_base * 4;
  uint64_t x6_expected = (x1_expected << 32) | (x0_expected >> 32);
  uint64_t x7_expected = ((x1_expected << 16) & 0xffff0000) |
                         ((x0_expected >> 48) & 0x0000ffff);

  ASSERT_EQUAL_64(x0_expected, x0);
  ASSERT_EQUAL_64(x1_expected, x1);
  ASSERT_EQUAL_64(x2_expected, x2);
  ASSERT_EQUAL_64(x3_expected, x3);
  ASSERT_EQUAL_64(x6_expected, x6);
  ASSERT_EQUAL_64(x7_expected, x7);

  TEARDOWN();
}


TEST(peek_poke_reglist) {
  SETUP();
  START();

  // Acquire all temps from the MacroAssembler. They are used arbitrarily below.
  UseScratchRegisterScope temps(&masm);
  temps.ExcludeAll();

  // The literal base is chosen to have two useful properties:
  //  * When multiplied by small values (such as a register index), this value
  //    is clearly readable in the result.
  //  * The value is not formed from repeating fixed-size smaller values, so it
  //    can be used to detect endianness-related errors.
  uint64_t base = 0x0100001000100101;

  // Initialize the registers.
  __ Mov(x1, base);
  __ Add(x2, x1, x1);
  __ Add(x3, x2, x1);
  __ Add(x4, x3, x1);

  CPURegList list_1(x1, x2, x3, x4);
  CPURegList list_2(x11, x12, x13, x14);
  int list_1_size = list_1.TotalSizeInBytes();

  __ Claim(2 * list_1_size);

  __ PokeCPURegList(list_1, 0);
  __ PokeXRegList(list_1.list(), list_1_size);
  __ PeekCPURegList(list_2, 2 * kXRegSizeInBytes);
  __ PeekXRegList(x15.Bit(), kWRegSizeInBytes);
  __ PeekWRegList(w16.Bit() | w17.Bit(), 3 * kXRegSizeInBytes);

  __ Drop(2 * list_1_size);


  uint64_t base_d = 0x1010010001000010;

  // Initialize the registers.
  __ Mov(x1, base_d);
  __ Add(x2, x1, x1);
  __ Add(x3, x2, x1);
  __ Add(x4, x3, x1);
  __ Fmov(d1, x1);
  __ Fmov(d2, x2);
  __ Fmov(d3, x3);
  __ Fmov(d4, x4);

  CPURegList list_d_1(d1, d2, d3, d4);
  CPURegList list_d_2(d11, d12, d13, d14);
  int list_d_1_size = list_d_1.TotalSizeInBytes();

  __ Claim(2 * list_d_1_size);

  __ PokeCPURegList(list_d_1, 0);
  __ PokeDRegList(list_d_1.list(), list_d_1_size);
  __ PeekCPURegList(list_d_2, 2 * kDRegSizeInBytes);
  __ PeekDRegList(d15.Bit(), kSRegSizeInBytes);
  __ PeekSRegList(s16.Bit() | s17.Bit(), 3 * kDRegSizeInBytes);

  __ Drop(2 * list_d_1_size);


  END();
  RUN();

  ASSERT_EQUAL_64(3 * base, x11);
  ASSERT_EQUAL_64(4 * base, x12);
  ASSERT_EQUAL_64(1 * base, x13);
  ASSERT_EQUAL_64(2 * base, x14);
  ASSERT_EQUAL_64(((1 * base) >> kWRegSize) | ((2 * base) << kWRegSize), x15);
  ASSERT_EQUAL_64(2 * base, x14);
  ASSERT_EQUAL_32((4 * base) & kWRegMask, w16);
  ASSERT_EQUAL_32((4 * base) >> kWRegSize, w17);

  ASSERT_EQUAL_FP64(rawbits_to_double(3 * base_d), d11);
  ASSERT_EQUAL_FP64(rawbits_to_double(4 * base_d), d12);
  ASSERT_EQUAL_FP64(rawbits_to_double(1 * base_d), d13);
  ASSERT_EQUAL_FP64(rawbits_to_double(2 * base_d), d14);
  ASSERT_EQUAL_FP64(
      rawbits_to_double((base_d >> kSRegSize) | ((2 * base_d) << kSRegSize)),
      d15);
  ASSERT_EQUAL_FP64(rawbits_to_double(2 * base_d), d14);
  ASSERT_EQUAL_FP32(rawbits_to_float((4 * base_d) & kSRegMask), s16);
  ASSERT_EQUAL_FP32(rawbits_to_float((4 * base_d) >> kSRegSize), s17);

  TEARDOWN();
}


TEST(load_store_reglist) {
  SETUP();
  START();

  // The literal base is chosen to have two useful properties:
  //  * When multiplied by small values (such as a register index), this value
  //    is clearly readable in the result.
  //  * The value is not formed from repeating fixed-size smaller values, so it
  //    can be used to detect endianness-related errors.
  uint64_t high_base = UINT32_C(0x01000010);
  uint64_t low_base =  UINT32_C(0x00100101);
  uint64_t base = (high_base << 32) | low_base;
  uint64_t array[21];
  memset(array, 0, sizeof(array));

  // Initialize the registers.
  __ Mov(x1, base);
  __ Add(x2, x1, x1);
  __ Add(x3, x2, x1);
  __ Add(x4, x3, x1);
  __ Fmov(d1, x1);
  __ Fmov(d2, x2);
  __ Fmov(d3, x3);
  __ Fmov(d4, x4);
  __ Fmov(d5, x1);
  __ Fmov(d6, x2);
  __ Fmov(d7, x3);
  __ Fmov(d8, x4);

  Register reg_base = x20;
  Register reg_index = x21;
  int size_stored = 0;

  __ Mov(reg_base, reinterpret_cast<uintptr_t>(&array));

  // Test aligned accesses.
  CPURegList list_src(w1, w2, w3, w4);
  CPURegList list_dst(w11, w12, w13, w14);
  CPURegList list_fp_src_1(d1, d2, d3, d4);
  CPURegList list_fp_dst_1(d11, d12, d13, d14);

  __ StoreCPURegList(list_src, MemOperand(reg_base, 0 * sizeof(uint64_t)));
  __ LoadCPURegList(list_dst, MemOperand(reg_base, 0 * sizeof(uint64_t)));
  size_stored += 4 * kWRegSizeInBytes;

  __ Mov(reg_index, size_stored);
  __ StoreCPURegList(list_src, MemOperand(reg_base, reg_index));
  __ LoadCPURegList(list_dst, MemOperand(reg_base, reg_index));
  size_stored += 4 * kWRegSizeInBytes;

  __ StoreCPURegList(list_fp_src_1, MemOperand(reg_base, size_stored));
  __ LoadCPURegList(list_fp_dst_1, MemOperand(reg_base, size_stored));
  size_stored += 4 * kDRegSizeInBytes;

  __ Mov(reg_index, size_stored);
  __ StoreCPURegList(list_fp_src_1, MemOperand(reg_base, reg_index));
  __ LoadCPURegList(list_fp_dst_1, MemOperand(reg_base, reg_index));
  size_stored += 4 * kDRegSizeInBytes;

  // Test unaligned accesses.
  CPURegList list_fp_src_2(d5, d6, d7, d8);
  CPURegList list_fp_dst_2(d15, d16, d17, d18);

  __ Str(wzr, MemOperand(reg_base, size_stored));
  size_stored += 1 * kWRegSizeInBytes;
  __ StoreCPURegList(list_fp_src_2, MemOperand(reg_base, size_stored));
  __ LoadCPURegList(list_fp_dst_2, MemOperand(reg_base, size_stored));
  size_stored += 4 * kDRegSizeInBytes;

  __ Mov(reg_index, size_stored);
  __ StoreCPURegList(list_fp_src_2, MemOperand(reg_base, reg_index));
  __ LoadCPURegList(list_fp_dst_2, MemOperand(reg_base, reg_index));

  END();
  RUN();

  VIXL_CHECK(array[0] == (1 * low_base) + (2 * low_base << kWRegSize));
  VIXL_CHECK(array[1] == (3 * low_base) + (4 * low_base << kWRegSize));
  VIXL_CHECK(array[2] == (1 * low_base) + (2 * low_base << kWRegSize));
  VIXL_CHECK(array[3] == (3 * low_base) + (4 * low_base << kWRegSize));
  VIXL_CHECK(array[4] == 1 * base);
  VIXL_CHECK(array[5] == 2 * base);
  VIXL_CHECK(array[6] == 3 * base);
  VIXL_CHECK(array[7] == 4 * base);
  VIXL_CHECK(array[8] == 1 * base);
  VIXL_CHECK(array[9] == 2 * base);
  VIXL_CHECK(array[10] == 3 * base);
  VIXL_CHECK(array[11] == 4 * base);
  VIXL_CHECK(array[12] == ((1 * low_base) << kSRegSize));
  VIXL_CHECK(array[13] == (((2 * low_base) << kSRegSize) | (1 * high_base)));
  VIXL_CHECK(array[14] == (((3 * low_base) << kSRegSize) | (2 * high_base)));
  VIXL_CHECK(array[15] == (((4 * low_base) << kSRegSize) | (3 * high_base)));
  VIXL_CHECK(array[16] == (((1 * low_base) << kSRegSize) | (4 * high_base)));
  VIXL_CHECK(array[17] == (((2 * low_base) << kSRegSize) | (1 * high_base)));
  VIXL_CHECK(array[18] == (((3 * low_base) << kSRegSize) | (2 * high_base)));
  VIXL_CHECK(array[19] == (((4 * low_base) << kSRegSize) | (3 * high_base)));
  VIXL_CHECK(array[20] == (4 * high_base));

  ASSERT_EQUAL_64(1 * low_base, x11);
  ASSERT_EQUAL_64(2 * low_base, x12);
  ASSERT_EQUAL_64(3 * low_base, x13);
  ASSERT_EQUAL_64(4 * low_base, x14);
  ASSERT_EQUAL_FP64(rawbits_to_double(1 * base), d11);
  ASSERT_EQUAL_FP64(rawbits_to_double(2 * base), d12);
  ASSERT_EQUAL_FP64(rawbits_to_double(3 * base), d13);
  ASSERT_EQUAL_FP64(rawbits_to_double(4 * base), d14);
  ASSERT_EQUAL_FP64(rawbits_to_double(1 * base), d15);
  ASSERT_EQUAL_FP64(rawbits_to_double(2 * base), d16);
  ASSERT_EQUAL_FP64(rawbits_to_double(3 * base), d17);
  ASSERT_EQUAL_FP64(rawbits_to_double(4 * base), d18);

  TEARDOWN();
}


// This enum is used only as an argument to the push-pop test helpers.
enum PushPopMethod {
  // Push or Pop using the Push and Pop methods, with blocks of up to four
  // registers. (Smaller blocks will be used if necessary.)
  PushPopByFour,

  // Use Push<Size>RegList and Pop<Size>RegList to transfer the registers.
  PushPopRegList
};


// The maximum number of registers that can be used by the PushPopXReg* tests,
// where a reg_count field is provided.
static int const kPushPopXRegMaxRegCount = -1;

// Test a simple push-pop pattern:
//  * Claim <claim> bytes to set the stack alignment.
//  * Push <reg_count> registers with size <reg_size>.
//  * Clobber the register contents.
//  * Pop <reg_count> registers to restore the original contents.
//  * Drop <claim> bytes to restore the original stack pointer.
//
// Different push and pop methods can be specified independently to test for
// proper word-endian behaviour.
static void PushPopXRegSimpleHelper(int reg_count,
                                    int claim,
                                    int reg_size,
                                    PushPopMethod push_method,
                                    PushPopMethod pop_method) {
  SETUP();

  START();

  // Arbitrarily pick a register to use as a stack pointer.
  const Register& stack_pointer = x20;
  const RegList allowed = ~stack_pointer.Bit();
  if (reg_count == kPushPopXRegMaxRegCount) {
    reg_count = CountSetBits(allowed, kNumberOfRegisters);
  }
  // Work out which registers to use, based on reg_size.
  Register r[kNumberOfRegisters];
  Register x[kNumberOfRegisters];
  RegList list = PopulateRegisterArray(NULL, x, r, reg_size, reg_count,
                                       allowed);

  // Acquire all temps from the MacroAssembler. They are used arbitrarily below.
  UseScratchRegisterScope temps(&masm);
  temps.ExcludeAll();

  // The literal base is chosen to have two useful properties:
  //  * When multiplied by small values (such as a register index), this value
  //    is clearly readable in the result.
  //  * The value is not formed from repeating fixed-size smaller values, so it
  //    can be used to detect endianness-related errors.
  uint64_t literal_base = 0x0100001000100101;

  {
    VIXL_ASSERT(__ StackPointer().Is(sp));
    __ Mov(stack_pointer, __ StackPointer());
    __ SetStackPointer(stack_pointer);

    int i;

    // Initialize the registers.
    for (i = 0; i < reg_count; i++) {
      // Always write into the X register, to ensure that the upper word is
      // properly ignored by Push when testing W registers.
      __ Mov(x[i], literal_base * i);
    }

    // Claim memory first, as requested.
    __ Claim(claim);

    switch (push_method) {
      case PushPopByFour:
        // Push high-numbered registers first (to the highest addresses).
        for (i = reg_count; i >= 4; i -= 4) {
          __ Push(r[i-1], r[i-2], r[i-3], r[i-4]);
        }
        // Finish off the leftovers.
        switch (i) {
          case 3:  __ Push(r[2], r[1], r[0]); break;
          case 2:  __ Push(r[1], r[0]);       break;
          case 1:  __ Push(r[0]);             break;
          default: VIXL_ASSERT(i == 0);            break;
        }
        break;
      case PushPopRegList:
        __ PushSizeRegList(list, reg_size);
        break;
    }

    // Clobber all the registers, to ensure that they get repopulated by Pop.
    Clobber(&masm, list);

    switch (pop_method) {
      case PushPopByFour:
        // Pop low-numbered registers first (from the lowest addresses).
        for (i = 0; i <= (reg_count-4); i += 4) {
          __ Pop(r[i], r[i+1], r[i+2], r[i+3]);
        }
        // Finish off the leftovers.
        switch (reg_count - i) {
          case 3:  __ Pop(r[i], r[i+1], r[i+2]); break;
          case 2:  __ Pop(r[i], r[i+1]);         break;
          case 1:  __ Pop(r[i]);                 break;
          default: VIXL_ASSERT(i == reg_count);       break;
        }
        break;
      case PushPopRegList:
        __ PopSizeRegList(list, reg_size);
        break;
    }

    // Drop memory to restore stack_pointer.
    __ Drop(claim);

    __ Mov(sp, __ StackPointer());
    __ SetStackPointer(sp);
  }

  END();

  RUN();

  // Check that the register contents were preserved.
  // Always use ASSERT_EQUAL_64, even when testing W registers, so we can test
  // that the upper word was properly cleared by Pop.
  literal_base &= (0xffffffffffffffff >> (64-reg_size));
  for (int i = 0; i < reg_count; i++) {
    if (x[i].Is(xzr)) {
      ASSERT_EQUAL_64(0, x[i]);
    } else {
      ASSERT_EQUAL_64(literal_base * i, x[i]);
    }
  }

  TEARDOWN();
}


TEST(push_pop_xreg_simple_32) {
  for (int claim = 0; claim <= 8; claim++) {
    for (int count = 0; count <= 8; count++) {
      PushPopXRegSimpleHelper(count, claim, kWRegSize,
                              PushPopByFour, PushPopByFour);
      PushPopXRegSimpleHelper(count, claim, kWRegSize,
                              PushPopByFour, PushPopRegList);
      PushPopXRegSimpleHelper(count, claim, kWRegSize,
                              PushPopRegList, PushPopByFour);
      PushPopXRegSimpleHelper(count, claim, kWRegSize,
                              PushPopRegList, PushPopRegList);
    }
    // Test with the maximum number of registers.
    PushPopXRegSimpleHelper(kPushPopXRegMaxRegCount,
                            claim, kWRegSize, PushPopByFour, PushPopByFour);
    PushPopXRegSimpleHelper(kPushPopXRegMaxRegCount,
                            claim, kWRegSize, PushPopByFour, PushPopRegList);
    PushPopXRegSimpleHelper(kPushPopXRegMaxRegCount,
                            claim, kWRegSize, PushPopRegList, PushPopByFour);
    PushPopXRegSimpleHelper(kPushPopXRegMaxRegCount,
                            claim, kWRegSize, PushPopRegList, PushPopRegList);
  }
}


TEST(push_pop_xreg_simple_64) {
  for (int claim = 0; claim <= 8; claim++) {
    for (int count = 0; count <= 8; count++) {
      PushPopXRegSimpleHelper(count, claim, kXRegSize,
                              PushPopByFour, PushPopByFour);
      PushPopXRegSimpleHelper(count, claim, kXRegSize,
                              PushPopByFour, PushPopRegList);
      PushPopXRegSimpleHelper(count, claim, kXRegSize,
                              PushPopRegList, PushPopByFour);
      PushPopXRegSimpleHelper(count, claim, kXRegSize,
                              PushPopRegList, PushPopRegList);
    }
    // Test with the maximum number of registers.
    PushPopXRegSimpleHelper(kPushPopXRegMaxRegCount,
                            claim, kXRegSize, PushPopByFour, PushPopByFour);
    PushPopXRegSimpleHelper(kPushPopXRegMaxRegCount,
                            claim, kXRegSize, PushPopByFour, PushPopRegList);
    PushPopXRegSimpleHelper(kPushPopXRegMaxRegCount,
                            claim, kXRegSize, PushPopRegList, PushPopByFour);
    PushPopXRegSimpleHelper(kPushPopXRegMaxRegCount,
                            claim, kXRegSize, PushPopRegList, PushPopRegList);
  }
}


// The maximum number of registers that can be used by the PushPopFPXReg* tests,
// where a reg_count field is provided.
static int const kPushPopFPXRegMaxRegCount = -1;

// Test a simple push-pop pattern:
//  * Claim <claim> bytes to set the stack alignment.
//  * Push <reg_count> FP registers with size <reg_size>.
//  * Clobber the register contents.
//  * Pop <reg_count> FP registers to restore the original contents.
//  * Drop <claim> bytes to restore the original stack pointer.
//
// Different push and pop methods can be specified independently to test for
// proper word-endian behaviour.
static void PushPopFPXRegSimpleHelper(int reg_count,
                                      int claim,
                                      int reg_size,
                                      PushPopMethod push_method,
                                      PushPopMethod pop_method) {
  SETUP();

  START();

  // We can use any floating-point register. None of them are reserved for
  // debug code, for example.
  static RegList const allowed = ~0;
  if (reg_count == kPushPopFPXRegMaxRegCount) {
    reg_count = CountSetBits(allowed, kNumberOfFPRegisters);
  }
  // Work out which registers to use, based on reg_size.
  FPRegister v[kNumberOfRegisters];
  FPRegister d[kNumberOfRegisters];
  RegList list = PopulateFPRegisterArray(NULL, d, v, reg_size, reg_count,
                                         allowed);

  // Arbitrarily pick a register to use as a stack pointer.
  const Register& stack_pointer = x10;

  // Acquire all temps from the MacroAssembler. They are used arbitrarily below.
  UseScratchRegisterScope temps(&masm);
  temps.ExcludeAll();

  // The literal base is chosen to have two useful properties:
  //  * When multiplied (using an integer) by small values (such as a register
  //    index), this value is clearly readable in the result.
  //  * The value is not formed from repeating fixed-size smaller values, so it
  //    can be used to detect endianness-related errors.
  //  * It is never a floating-point NaN, and will therefore always compare
  //    equal to itself.
  uint64_t literal_base = 0x0100001000100101;

  {
    VIXL_ASSERT(__ StackPointer().Is(sp));
    __ Mov(stack_pointer, __ StackPointer());
    __ SetStackPointer(stack_pointer);

    int i;

    // Initialize the registers, using X registers to load the literal.
    __ Mov(x0, 0);
    __ Mov(x1, literal_base);
    for (i = 0; i < reg_count; i++) {
      // Always write into the D register, to ensure that the upper word is
      // properly ignored by Push when testing S registers.
      __ Fmov(d[i], x0);
      // Calculate the next literal.
      __ Add(x0, x0, x1);
    }

    // Claim memory first, as requested.
    __ Claim(claim);

    switch (push_method) {
      case PushPopByFour:
        // Push high-numbered registers first (to the highest addresses).
        for (i = reg_count; i >= 4; i -= 4) {
          __ Push(v[i-1], v[i-2], v[i-3], v[i-4]);
        }
        // Finish off the leftovers.
        switch (i) {
          case 3:  __ Push(v[2], v[1], v[0]); break;
          case 2:  __ Push(v[1], v[0]);       break;
          case 1:  __ Push(v[0]);             break;
          default: VIXL_ASSERT(i == 0);            break;
        }
        break;
      case PushPopRegList:
        __ PushSizeRegList(list, reg_size, CPURegister::kVRegister);
        break;
    }

    // Clobber all the registers, to ensure that they get repopulated by Pop.
    ClobberFP(&masm, list);

    switch (pop_method) {
      case PushPopByFour:
        // Pop low-numbered registers first (from the lowest addresses).
        for (i = 0; i <= (reg_count-4); i += 4) {
          __ Pop(v[i], v[i+1], v[i+2], v[i+3]);
        }
        // Finish off the leftovers.
        switch (reg_count - i) {
          case 3:  __ Pop(v[i], v[i+1], v[i+2]); break;
          case 2:  __ Pop(v[i], v[i+1]);         break;
          case 1:  __ Pop(v[i]);                 break;
          default: VIXL_ASSERT(i == reg_count);       break;
        }
        break;
      case PushPopRegList:
        __ PopSizeRegList(list, reg_size, CPURegister::kVRegister);
        break;
    }

    // Drop memory to restore the stack pointer.
    __ Drop(claim);

    __ Mov(sp, __ StackPointer());
    __ SetStackPointer(sp);
  }

  END();

  RUN();

  // Check that the register contents were preserved.
  // Always use ASSERT_EQUAL_FP64, even when testing S registers, so we can
  // test that the upper word was properly cleared by Pop.
  literal_base &= (0xffffffffffffffff >> (64-reg_size));
  for (int i = 0; i < reg_count; i++) {
    uint64_t literal = literal_base * i;
    double expected;
    memcpy(&expected, &literal, sizeof(expected));
    ASSERT_EQUAL_FP64(expected, d[i]);
  }

  TEARDOWN();
}


TEST(push_pop_fp_xreg_simple_32) {
  for (int claim = 0; claim <= 8; claim++) {
    for (int count = 0; count <= 8; count++) {
      PushPopFPXRegSimpleHelper(count, claim, kSRegSize,
                                PushPopByFour, PushPopByFour);
      PushPopFPXRegSimpleHelper(count, claim, kSRegSize,
                                PushPopByFour, PushPopRegList);
      PushPopFPXRegSimpleHelper(count, claim, kSRegSize,
                                PushPopRegList, PushPopByFour);
      PushPopFPXRegSimpleHelper(count, claim, kSRegSize,
                                PushPopRegList, PushPopRegList);
    }
    // Test with the maximum number of registers.
    PushPopFPXRegSimpleHelper(kPushPopFPXRegMaxRegCount, claim, kSRegSize,
                              PushPopByFour, PushPopByFour);
    PushPopFPXRegSimpleHelper(kPushPopFPXRegMaxRegCount, claim, kSRegSize,
                              PushPopByFour, PushPopRegList);
    PushPopFPXRegSimpleHelper(kPushPopFPXRegMaxRegCount, claim, kSRegSize,
                              PushPopRegList, PushPopByFour);
    PushPopFPXRegSimpleHelper(kPushPopFPXRegMaxRegCount, claim, kSRegSize,
                              PushPopRegList, PushPopRegList);
  }
}


TEST(push_pop_fp_xreg_simple_64) {
  for (int claim = 0; claim <= 8; claim++) {
    for (int count = 0; count <= 8; count++) {
      PushPopFPXRegSimpleHelper(count, claim, kDRegSize,
                                PushPopByFour, PushPopByFour);
      PushPopFPXRegSimpleHelper(count, claim, kDRegSize,
                                PushPopByFour, PushPopRegList);
      PushPopFPXRegSimpleHelper(count, claim, kDRegSize,
                                PushPopRegList, PushPopByFour);
      PushPopFPXRegSimpleHelper(count, claim, kDRegSize,
                                PushPopRegList, PushPopRegList);
    }
    // Test with the maximum number of registers.
    PushPopFPXRegSimpleHelper(kPushPopFPXRegMaxRegCount, claim, kDRegSize,
                              PushPopByFour, PushPopByFour);
    PushPopFPXRegSimpleHelper(kPushPopFPXRegMaxRegCount, claim, kDRegSize,
                              PushPopByFour, PushPopRegList);
    PushPopFPXRegSimpleHelper(kPushPopFPXRegMaxRegCount, claim, kDRegSize,
                              PushPopRegList, PushPopByFour);
    PushPopFPXRegSimpleHelper(kPushPopFPXRegMaxRegCount, claim, kDRegSize,
                              PushPopRegList, PushPopRegList);
  }
}


// Push and pop data using an overlapping combination of Push/Pop and
// RegList-based methods.
static void PushPopXRegMixedMethodsHelper(int claim, int reg_size) {
  SETUP();

  // Arbitrarily pick a register to use as a stack pointer.
  const Register& stack_pointer = x5;
  const RegList allowed = ~stack_pointer.Bit();
  // Work out which registers to use, based on reg_size.
  Register r[10];
  Register x[10];
  PopulateRegisterArray(NULL, x, r, reg_size, 10, allowed);

  // Calculate some handy register lists.
  RegList r0_to_r3 = 0;
  for (int i = 0; i <= 3; i++) {
    r0_to_r3 |= x[i].Bit();
  }
  RegList r4_to_r5 = 0;
  for (int i = 4; i <= 5; i++) {
    r4_to_r5 |= x[i].Bit();
  }
  RegList r6_to_r9 = 0;
  for (int i = 6; i <= 9; i++) {
    r6_to_r9 |= x[i].Bit();
  }

  // Acquire all temps from the MacroAssembler. They are used arbitrarily below.
  UseScratchRegisterScope temps(&masm);
  temps.ExcludeAll();

  // The literal base is chosen to have two useful properties:
  //  * When multiplied by small values (such as a register index), this value
  //    is clearly readable in the result.
  //  * The value is not formed from repeating fixed-size smaller values, so it
  //    can be used to detect endianness-related errors.
  uint64_t literal_base = 0x0100001000100101;

  START();
  {
    VIXL_ASSERT(__ StackPointer().Is(sp));
    __ Mov(stack_pointer, __ StackPointer());
    __ SetStackPointer(stack_pointer);

    // Claim memory first, as requested.
    __ Claim(claim);

    __ Mov(x[3], literal_base * 3);
    __ Mov(x[2], literal_base * 2);
    __ Mov(x[1], literal_base * 1);
    __ Mov(x[0], literal_base * 0);

    __ PushSizeRegList(r0_to_r3, reg_size);
    __ Push(r[3], r[2]);

    Clobber(&masm, r0_to_r3);
    __ PopSizeRegList(r0_to_r3, reg_size);

    __ Push(r[2], r[1], r[3], r[0]);

    Clobber(&masm, r4_to_r5);
    __ Pop(r[4], r[5]);
    Clobber(&masm, r6_to_r9);
    __ Pop(r[6], r[7], r[8], r[9]);

    // Drop memory to restore stack_pointer.
    __ Drop(claim);

    __ Mov(sp, __ StackPointer());
    __ SetStackPointer(sp);
  }

  END();

  RUN();

  // Always use ASSERT_EQUAL_64, even when testing W registers, so we can test
  // that the upper word was properly cleared by Pop.
  literal_base &= (0xffffffffffffffff >> (64-reg_size));

  ASSERT_EQUAL_64(literal_base * 3, x[9]);
  ASSERT_EQUAL_64(literal_base * 2, x[8]);
  ASSERT_EQUAL_64(literal_base * 0, x[7]);
  ASSERT_EQUAL_64(literal_base * 3, x[6]);
  ASSERT_EQUAL_64(literal_base * 1, x[5]);
  ASSERT_EQUAL_64(literal_base * 2, x[4]);

  TEARDOWN();
}


TEST(push_pop_xreg_mixed_methods_64) {
  for (int claim = 0; claim <= 8; claim++) {
    PushPopXRegMixedMethodsHelper(claim, kXRegSize);
  }
}


TEST(push_pop_xreg_mixed_methods_32) {
  for (int claim = 0; claim <= 8; claim++) {
    PushPopXRegMixedMethodsHelper(claim, kWRegSize);
  }
}


// Push and pop data using overlapping X- and W-sized quantities.
static void PushPopXRegWXOverlapHelper(int reg_count, int claim) {
  SETUP();

  // Arbitrarily pick a register to use as a stack pointer.
  const Register& stack_pointer = x10;
  const RegList allowed = ~stack_pointer.Bit();
  if (reg_count == kPushPopXRegMaxRegCount) {
    reg_count = CountSetBits(allowed, kNumberOfRegisters);
  }
  // Work out which registers to use, based on reg_size.
  Register w[kNumberOfRegisters];
  Register x[kNumberOfRegisters];
  RegList list = PopulateRegisterArray(w, x, NULL, 0, reg_count, allowed);

  // The number of W-sized slots we expect to pop. When we pop, we alternate
  // between W and X registers, so we need reg_count*1.5 W-sized slots.
  int const requested_w_slots = reg_count + reg_count / 2;

  // Track what _should_ be on the stack, using W-sized slots.
  static int const kMaxWSlots = kNumberOfRegisters + kNumberOfRegisters / 2;
  uint32_t stack[kMaxWSlots];
  for (int i = 0; i < kMaxWSlots; i++) {
    stack[i] = 0xdeadbeef;
  }

  // Acquire all temps from the MacroAssembler. They are used arbitrarily below.
  UseScratchRegisterScope temps(&masm);
  temps.ExcludeAll();

  // The literal base is chosen to have two useful properties:
  //  * When multiplied by small values (such as a register index), this value
  //    is clearly readable in the result.
  //  * The value is not formed from repeating fixed-size smaller values, so it
  //    can be used to detect endianness-related errors.
  static uint64_t const literal_base = 0x0100001000100101;
  static uint64_t const literal_base_hi = literal_base >> 32;
  static uint64_t const literal_base_lo = literal_base & 0xffffffff;
  static uint64_t const literal_base_w = literal_base & 0xffffffff;

  START();
  {
    VIXL_ASSERT(__ StackPointer().Is(sp));
    __ Mov(stack_pointer, __ StackPointer());
    __ SetStackPointer(stack_pointer);

    // Initialize the registers.
    for (int i = 0; i < reg_count; i++) {
      // Always write into the X register, to ensure that the upper word is
      // properly ignored by Push when testing W registers.
      __ Mov(x[i], literal_base * i);
    }

    // Claim memory first, as requested.
    __ Claim(claim);

    // The push-pop pattern is as follows:
    // Push:           Pop:
    //  x[0](hi)   ->   w[0]
    //  x[0](lo)   ->   x[1](hi)
    //  w[1]       ->   x[1](lo)
    //  w[1]       ->   w[2]
    //  x[2](hi)   ->   x[2](hi)
    //  x[2](lo)   ->   x[2](lo)
    //  x[2](hi)   ->   w[3]
    //  x[2](lo)   ->   x[4](hi)
    //  x[2](hi)   ->   x[4](lo)
    //  x[2](lo)   ->   w[5]
    //  w[3]       ->   x[5](hi)
    //  w[3]       ->   x[6](lo)
    //  w[3]       ->   w[7]
    //  w[3]       ->   x[8](hi)
    //  x[4](hi)   ->   x[8](lo)
    //  x[4](lo)   ->   w[9]
    // ... pattern continues ...
    //
    // That is, registers are pushed starting with the lower numbers,
    // alternating between x and w registers, and pushing i%4+1 copies of each,
    // where i is the register number.
    // Registers are popped starting with the higher numbers one-by-one,
    // alternating between x and w registers, but only popping one at a time.
    //
    // This pattern provides a wide variety of alignment effects and overlaps.

    // ---- Push ----

    int active_w_slots = 0;
    for (int i = 0; active_w_slots < requested_w_slots; i++) {
      VIXL_ASSERT(i < reg_count);
      // In order to test various arguments to PushMultipleTimes, and to try to
      // exercise different alignment and overlap effects, we push each
      // register a different number of times.
      int times = i % 4 + 1;
      if (i & 1) {
        // Push odd-numbered registers as W registers.
        __ PushMultipleTimes(times, w[i]);
        // Fill in the expected stack slots.
        for (int j = 0; j < times; j++) {
          if (w[i].Is(wzr)) {
            // The zero register always writes zeroes.
            stack[active_w_slots++] = 0;
          } else {
            stack[active_w_slots++] = literal_base_w * i;
          }
        }
      } else {
        // Push even-numbered registers as X registers.
        __ PushMultipleTimes(times, x[i]);
        // Fill in the expected stack slots.
        for (int j = 0; j < times; j++) {
          if (x[i].Is(xzr)) {
            // The zero register always writes zeroes.
            stack[active_w_slots++] = 0;
            stack[active_w_slots++] = 0;
          } else {
            stack[active_w_slots++] = literal_base_hi * i;
            stack[active_w_slots++] = literal_base_lo * i;
          }
        }
      }
    }
    // Because we were pushing several registers at a time, we probably pushed
    // more than we needed to.
    if (active_w_slots > requested_w_slots) {
      __ Drop((active_w_slots - requested_w_slots) * kWRegSizeInBytes);
      // Bump the number of active W-sized slots back to where it should be,
      // and fill the empty space with a dummy value.
      do {
        stack[active_w_slots--] = 0xdeadbeef;
      } while (active_w_slots > requested_w_slots);
    }

    // ---- Pop ----

    Clobber(&masm, list);

    // If popping an even number of registers, the first one will be X-sized.
    // Otherwise, the first one will be W-sized.
    bool next_is_64 = !(reg_count & 1);
    for (int i = reg_count-1; i >= 0; i--) {
      if (next_is_64) {
        __ Pop(x[i]);
        active_w_slots -= 2;
      } else {
        __ Pop(w[i]);
        active_w_slots -= 1;
      }
      next_is_64 = !next_is_64;
    }
    VIXL_ASSERT(active_w_slots == 0);

    // Drop memory to restore stack_pointer.
    __ Drop(claim);

    __ Mov(sp, __ StackPointer());
    __ SetStackPointer(sp);
  }

  END();

  RUN();

  int slot = 0;
  for (int i = 0; i < reg_count; i++) {
    // Even-numbered registers were written as W registers.
    // Odd-numbered registers were written as X registers.
    bool expect_64 = (i & 1);
    uint64_t expected;

    if (expect_64) {
      uint64_t hi = stack[slot++];
      uint64_t lo = stack[slot++];
      expected = (hi << 32) | lo;
    } else {
      expected = stack[slot++];
    }

    // Always use ASSERT_EQUAL_64, even when testing W registers, so we can
    // test that the upper word was properly cleared by Pop.
    if (x[i].Is(xzr)) {
      ASSERT_EQUAL_64(0, x[i]);
    } else {
      ASSERT_EQUAL_64(expected, x[i]);
    }
  }
  VIXL_ASSERT(slot == requested_w_slots);

  TEARDOWN();
}


TEST(push_pop_xreg_wx_overlap) {
  for (int claim = 0; claim <= 8; claim++) {
    for (int count = 1; count <= 8; count++) {
      PushPopXRegWXOverlapHelper(count, claim);
    }
    // Test with the maximum number of registers.
    PushPopXRegWXOverlapHelper(kPushPopXRegMaxRegCount, claim);
  }
}


TEST(push_pop_sp) {
  SETUP();

  START();

  VIXL_ASSERT(sp.Is(__ StackPointer()));

  // Acquire all temps from the MacroAssembler. They are used arbitrarily below.
  UseScratchRegisterScope temps(&masm);
  temps.ExcludeAll();

  __ Mov(x3, 0x3333333333333333);
  __ Mov(x2, 0x2222222222222222);
  __ Mov(x1, 0x1111111111111111);
  __ Mov(x0, 0x0000000000000000);
  __ Claim(2 * kXRegSizeInBytes);
  __ PushXRegList(x0.Bit() | x1.Bit() | x2.Bit() | x3.Bit());
  __ Push(x3, x2);
  __ PopXRegList(x0.Bit() | x1.Bit() | x2.Bit() | x3.Bit());
  __ Push(x2, x1, x3, x0);
  __ Pop(x4, x5);
  __ Pop(x6, x7, x8, x9);

  __ Claim(2 * kXRegSizeInBytes);
  __ PushWRegList(w0.Bit() | w1.Bit() | w2.Bit() | w3.Bit());
  __ Push(w3, w1, w2, w0);
  __ PopWRegList(w10.Bit() | w11.Bit() | w12.Bit() | w13.Bit());
  __ Pop(w14, w15, w16, w17);

  __ Claim(2 * kXRegSizeInBytes);
  __ Push(w2, w2, w1, w1);
  __ Push(x3, x3);
  __ Pop(w18, w19, w20, w21);
  __ Pop(x22, x23);

  __ Claim(2 * kXRegSizeInBytes);
  __ PushXRegList(x1.Bit() | x22.Bit());
  __ PopXRegList(x24.Bit() | x26.Bit());

  __ Claim(2 * kXRegSizeInBytes);
  __ PushWRegList(w1.Bit() | w2.Bit() | w4.Bit() | w22.Bit());
  __ PopWRegList(w25.Bit() | w27.Bit() | w28.Bit() | w29.Bit());

  __ Claim(2 * kXRegSizeInBytes);
  __ PushXRegList(0);
  __ PopXRegList(0);
  __ PushXRegList(0xffffffff);
  __ PopXRegList(0xffffffff);
  __ Drop(12 * kXRegSizeInBytes);
  END();

  RUN();

  ASSERT_EQUAL_64(0x1111111111111111, x3);
  ASSERT_EQUAL_64(0x0000000000000000, x2);
  ASSERT_EQUAL_64(0x3333333333333333, x1);
  ASSERT_EQUAL_64(0x2222222222222222, x0);
  ASSERT_EQUAL_64(0x3333333333333333, x9);
  ASSERT_EQUAL_64(0x2222222222222222, x8);
  ASSERT_EQUAL_64(0x0000000000000000, x7);
  ASSERT_EQUAL_64(0x3333333333333333, x6);
  ASSERT_EQUAL_64(0x1111111111111111, x5);
  ASSERT_EQUAL_64(0x2222222222222222, x4);

  ASSERT_EQUAL_32(0x11111111U, w13);
  ASSERT_EQUAL_32(0x33333333U, w12);
  ASSERT_EQUAL_32(0x00000000U, w11);
  ASSERT_EQUAL_32(0x22222222U, w10);
  ASSERT_EQUAL_32(0x11111111U, w17);
  ASSERT_EQUAL_32(0x00000000U, w16);
  ASSERT_EQUAL_32(0x33333333U, w15);
  ASSERT_EQUAL_32(0x22222222U, w14);

  ASSERT_EQUAL_32(0x11111111U, w18);
  ASSERT_EQUAL_32(0x11111111U, w19);
  ASSERT_EQUAL_32(0x11111111U, w20);
  ASSERT_EQUAL_32(0x11111111U, w21);
  ASSERT_EQUAL_64(0x3333333333333333, x22);
  ASSERT_EQUAL_64(0x0000000000000000, x23);

  ASSERT_EQUAL_64(0x3333333333333333, x24);
  ASSERT_EQUAL_64(0x3333333333333333, x26);

  ASSERT_EQUAL_32(0x33333333U, w25);
  ASSERT_EQUAL_32(0x00000000U, w27);
  ASSERT_EQUAL_32(0x22222222U, w28);
  ASSERT_EQUAL_32(0x33333333U, w29);
  TEARDOWN();
}


TEST(noreg) {
  // This test doesn't generate any code, but it verifies some invariants
  // related to NoReg.
  VIXL_CHECK(NoReg.Is(NoFPReg));
  VIXL_CHECK(NoFPReg.Is(NoReg));

  VIXL_CHECK(NoVReg.Is(NoReg));
  VIXL_CHECK(NoReg.Is(NoVReg));

  VIXL_CHECK(NoReg.Is(NoCPUReg));
  VIXL_CHECK(NoCPUReg.Is(NoReg));

  VIXL_CHECK(NoFPReg.Is(NoCPUReg));
  VIXL_CHECK(NoCPUReg.Is(NoFPReg));

  VIXL_CHECK(NoVReg.Is(NoCPUReg));
  VIXL_CHECK(NoCPUReg.Is(NoVReg));

  VIXL_CHECK(NoReg.IsNone());
  VIXL_CHECK(NoFPReg.IsNone());
  VIXL_CHECK(NoVReg.IsNone());
  VIXL_CHECK(NoCPUReg.IsNone());
}


TEST(isvalid) {
  // This test doesn't generate any code, but it verifies some invariants
  // related to IsValid().
  VIXL_CHECK(!NoReg.IsValid());
  VIXL_CHECK(!NoFPReg.IsValid());
  VIXL_CHECK(!NoVReg.IsValid());
  VIXL_CHECK(!NoCPUReg.IsValid());

  VIXL_CHECK(x0.IsValid());
  VIXL_CHECK(w0.IsValid());
  VIXL_CHECK(x30.IsValid());
  VIXL_CHECK(w30.IsValid());
  VIXL_CHECK(xzr.IsValid());
  VIXL_CHECK(wzr.IsValid());

  VIXL_CHECK(sp.IsValid());
  VIXL_CHECK(wsp.IsValid());

  VIXL_CHECK(d0.IsValid());
  VIXL_CHECK(s0.IsValid());
  VIXL_CHECK(d31.IsValid());
  VIXL_CHECK(s31.IsValid());

  VIXL_CHECK(x0.IsValidRegister());
  VIXL_CHECK(w0.IsValidRegister());
  VIXL_CHECK(xzr.IsValidRegister());
  VIXL_CHECK(wzr.IsValidRegister());
  VIXL_CHECK(sp.IsValidRegister());
  VIXL_CHECK(wsp.IsValidRegister());
  VIXL_CHECK(!x0.IsValidFPRegister());
  VIXL_CHECK(!w0.IsValidFPRegister());
  VIXL_CHECK(!xzr.IsValidFPRegister());
  VIXL_CHECK(!wzr.IsValidFPRegister());
  VIXL_CHECK(!sp.IsValidFPRegister());
  VIXL_CHECK(!wsp.IsValidFPRegister());

  VIXL_CHECK(d0.IsValidFPRegister());
  VIXL_CHECK(s0.IsValidFPRegister());
  VIXL_CHECK(!d0.IsValidRegister());
  VIXL_CHECK(!s0.IsValidRegister());

  // Test the same as before, but using CPURegister types. This shouldn't make
  // any difference.
  VIXL_CHECK(static_cast<CPURegister>(x0).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(w0).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(x30).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(w30).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(xzr).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(wzr).IsValid());

  VIXL_CHECK(static_cast<CPURegister>(sp).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(wsp).IsValid());

  VIXL_CHECK(static_cast<CPURegister>(d0).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(s0).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(d31).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(s31).IsValid());

  VIXL_CHECK(static_cast<CPURegister>(x0).IsValidRegister());
  VIXL_CHECK(static_cast<CPURegister>(w0).IsValidRegister());
  VIXL_CHECK(static_cast<CPURegister>(xzr).IsValidRegister());
  VIXL_CHECK(static_cast<CPURegister>(wzr).IsValidRegister());
  VIXL_CHECK(static_cast<CPURegister>(sp).IsValidRegister());
  VIXL_CHECK(static_cast<CPURegister>(wsp).IsValidRegister());
  VIXL_CHECK(!static_cast<CPURegister>(x0).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(w0).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(xzr).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(wzr).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(sp).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(wsp).IsValidFPRegister());

  VIXL_CHECK(static_cast<CPURegister>(d0).IsValidFPRegister());
  VIXL_CHECK(static_cast<CPURegister>(s0).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(d0).IsValidRegister());
  VIXL_CHECK(!static_cast<CPURegister>(s0).IsValidRegister());
}


TEST(printf) {
  SETUP();
  START();

  char const * test_plain_string = "Printf with no arguments.\n";
  char const * test_substring = "'This is a substring.'";
  RegisterDump before;

  // Initialize x29 to the value of the stack pointer. We will use x29 as a
  // temporary stack pointer later, and initializing it in this way allows the
  // RegisterDump check to pass.
  __ Mov(x29, __ StackPointer());

  // Test simple integer arguments.
  __ Mov(x0, 1234);
  __ Mov(x1, 0x1234);

  // Test simple floating-point arguments.
  __ Fmov(d0, 1.234);

  // Test pointer (string) arguments.
  __ Mov(x2, reinterpret_cast<uintptr_t>(test_substring));

  // Test the maximum number of arguments, and sign extension.
  __ Mov(w3, 0xffffffff);
  __ Mov(w4, 0xffffffff);
  __ Mov(x5, 0xffffffffffffffff);
  __ Mov(x6, 0xffffffffffffffff);
  __ Fmov(s1, 1.234);
  __ Fmov(s2, 2.345);
  __ Fmov(d3, 3.456);
  __ Fmov(d4, 4.567);

  // Test printing callee-saved registers.
  __ Mov(x28, 0x123456789abcdef);
  __ Fmov(d10, 42.0);

  // Test with three arguments.
  __ Mov(x10, 3);
  __ Mov(x11, 40);
  __ Mov(x12, 500);

  // A single character.
  __ Mov(w13, 'x');

  // Check that we don't clobber any registers.
  before.Dump(&masm);

  __ Printf(test_plain_string);   // NOLINT(runtime/printf)
  __ Printf("x0: %" PRId64 ", x1: 0x%08" PRIx64 "\n", x0, x1);
  __ Printf("w5: %" PRId32 ", x5: %" PRId64"\n", w5, x5);
  __ Printf("d0: %f\n", d0);
  __ Printf("Test %%s: %s\n", x2);
  __ Printf("w3(uint32): %" PRIu32 "\nw4(int32): %" PRId32 "\n"
            "x5(uint64): %" PRIu64 "\nx6(int64): %" PRId64 "\n",
            w3, w4, x5, x6);
  __ Printf("%%f: %f\n%%g: %g\n%%e: %e\n%%E: %E\n", s1, s2, d3, d4);
  __ Printf("0x%" PRIx32 ", 0x%" PRIx64 "\n", w28, x28);
  __ Printf("%g\n", d10);
  __ Printf("%%%%%s%%%c%%\n", x2, w13);

  // Print the stack pointer (sp).
  __ Printf("StackPointer(sp): 0x%016" PRIx64 ", 0x%08" PRIx32 "\n",
            __ StackPointer(), __ StackPointer().W());

  // Test with a different stack pointer.
  const Register old_stack_pointer = __ StackPointer();
  __ Mov(x29, old_stack_pointer);
  __ SetStackPointer(x29);
  // Print the stack pointer (not sp).
  __ Printf("StackPointer(not sp): 0x%016" PRIx64 ", 0x%08" PRIx32 "\n",
            __ StackPointer(), __ StackPointer().W());
  __ Mov(old_stack_pointer, __ StackPointer());
  __ SetStackPointer(old_stack_pointer);

  // Test with three arguments.
  __ Printf("3=%u, 4=%u, 5=%u\n", x10, x11, x12);

  // Mixed argument types.
  __ Printf("w3: %" PRIu32 ", s1: %f, x5: %" PRIu64 ", d3: %f\n",
            w3, s1, x5, d3);
  __ Printf("s1: %f, d3: %f, w3: %" PRId32 ", x5: %" PRId64 "\n",
            s1, d3, w3, x5);

  END();
  RUN();

  // We cannot easily test the output of the Printf sequences, and because
  // Printf preserves all registers by default, we can't look at the number of
  // bytes that were printed. However, the printf_no_preserve test should check
  // that, and here we just test that we didn't clobber any registers.
  ASSERT_EQUAL_REGISTERS(before);

  TEARDOWN();
}


TEST(printf_no_preserve) {
  SETUP();
  START();

  char const * test_plain_string = "Printf with no arguments.\n";
  char const * test_substring = "'This is a substring.'";

  __ PrintfNoPreserve(test_plain_string);
  __ Mov(x19, x0);

  // Test simple integer arguments.
  __ Mov(x0, 1234);
  __ Mov(x1, 0x1234);
  __ PrintfNoPreserve("x0: %" PRId64", x1: 0x%08" PRIx64 "\n", x0, x1);
  __ Mov(x20, x0);

  // Test simple floating-point arguments.
  __ Fmov(d0, 1.234);
  __ PrintfNoPreserve("d0: %f\n", d0);
  __ Mov(x21, x0);

  // Test pointer (string) arguments.
  __ Mov(x2, reinterpret_cast<uintptr_t>(test_substring));
  __ PrintfNoPreserve("Test %%s: %s\n", x2);
  __ Mov(x22, x0);

  // Test the maximum number of arguments, and sign extension.
  __ Mov(w3, 0xffffffff);
  __ Mov(w4, 0xffffffff);
  __ Mov(x5, 0xffffffffffffffff);
  __ Mov(x6, 0xffffffffffffffff);
  __ PrintfNoPreserve("w3(uint32): %" PRIu32 "\nw4(int32): %" PRId32 "\n"
                      "x5(uint64): %" PRIu64 "\nx6(int64): %" PRId64 "\n",
                      w3, w4, x5, x6);
  __ Mov(x23, x0);

  __ Fmov(s1, 1.234);
  __ Fmov(s2, 2.345);
  __ Fmov(d3, 3.456);
  __ Fmov(d4, 4.567);
  __ PrintfNoPreserve("%%f: %f\n%%g: %g\n%%e: %e\n%%E: %E\n", s1, s2, d3, d4);
  __ Mov(x24, x0);

  // Test printing callee-saved registers.
  __ Mov(x28, 0x123456789abcdef);
  __ PrintfNoPreserve("0x%" PRIx32 ", 0x%" PRIx64 "\n", w28, x28);
  __ Mov(x25, x0);

  __ Fmov(d10, 42.0);
  __ PrintfNoPreserve("%g\n", d10);
  __ Mov(x26, x0);

  // Test with a different stack pointer.
  const Register old_stack_pointer = __ StackPointer();
  __ Mov(x29, old_stack_pointer);
  __ SetStackPointer(x29);
  // Print the stack pointer (not sp).
  __ PrintfNoPreserve(
      "StackPointer(not sp): 0x%016" PRIx64 ", 0x%08" PRIx32 "\n",
      __ StackPointer(), __ StackPointer().W());
  __ Mov(x27, x0);
  __ Mov(old_stack_pointer, __ StackPointer());
  __ SetStackPointer(old_stack_pointer);

  // Test with three arguments.
  __ Mov(x3, 3);
  __ Mov(x4, 40);
  __ Mov(x5, 500);
  __ PrintfNoPreserve("3=%u, 4=%u, 5=%u\n", x3, x4, x5);
  __ Mov(x28, x0);

  // Mixed argument types.
  __ Mov(w3, 0xffffffff);
  __ Fmov(s1, 1.234);
  __ Mov(x5, 0xffffffffffffffff);
  __ Fmov(d3, 3.456);
  __ PrintfNoPreserve("w3: %" PRIu32 ", s1: %f, x5: %" PRIu64 ", d3: %f\n",
                      w3, s1, x5, d3);
  __ Mov(x29, x0);

  END();
  RUN();

  // We cannot easily test the exact output of the Printf sequences, but we can
  // use the return code to check that the string length was correct.

  // Printf with no arguments.
  ASSERT_EQUAL_64(strlen(test_plain_string), x19);
  // x0: 1234, x1: 0x00001234
  ASSERT_EQUAL_64(25, x20);
  // d0: 1.234000
  ASSERT_EQUAL_64(13, x21);
  // Test %s: 'This is a substring.'
  ASSERT_EQUAL_64(32, x22);
  // w3(uint32): 4294967295
  // w4(int32): -1
  // x5(uint64): 18446744073709551615
  // x6(int64): -1
  ASSERT_EQUAL_64(23 + 14 + 33 + 14, x23);
  // %f: 1.234000
  // %g: 2.345
  // %e: 3.456000e+00
  // %E: 4.567000E+00
  ASSERT_EQUAL_64(13 + 10 + 17 + 17, x24);
  // 0x89abcdef, 0x123456789abcdef
  ASSERT_EQUAL_64(30, x25);
  // 42
  ASSERT_EQUAL_64(3, x26);
  // StackPointer(not sp): 0x00007fb037ae2370, 0x37ae2370
  // Note: This is an example value, but the field width is fixed here so the
  // string length is still predictable.
  ASSERT_EQUAL_64(53, x27);
  // 3=3, 4=40, 5=500
  ASSERT_EQUAL_64(17, x28);
  // w3: 4294967295, s1: 1.234000, x5: 18446744073709551615, d3: 3.456000
  ASSERT_EQUAL_64(69, x29);

  TEARDOWN();
}


#ifndef VIXL_INCLUDE_SIMULATOR
TEST(trace) {
  // The Trace helper should not generate any code unless the simulator (or
  // debugger) is being used.
  SETUP();
  START();

  Label start;
  __ Bind(&start);
  __ Trace(LOG_ALL, TRACE_ENABLE);
  __ Trace(LOG_ALL, TRACE_DISABLE);
  VIXL_CHECK(__ SizeOfCodeGeneratedSince(&start) == 0);

  END();
  TEARDOWN();
}
#endif


#ifndef VIXL_INCLUDE_SIMULATOR
TEST(log) {
  // The Log helper should not generate any code unless the simulator (or
  // debugger) is being used.
  SETUP();
  START();

  Label start;
  __ Bind(&start);
  __ Log(LOG_ALL);
  VIXL_CHECK(__ SizeOfCodeGeneratedSince(&start) == 0);

  END();
  TEARDOWN();
}
#endif


TEST(instruction_accurate_scope) {
  SETUP();
  START();

  // By default macro instructions are allowed.
  VIXL_ASSERT(masm.AllowMacroInstructions());
  {
    InstructionAccurateScope scope1(&masm, 2);
    VIXL_ASSERT(!masm.AllowMacroInstructions());
    __ nop();
    {
      InstructionAccurateScope scope2(&masm, 1);
      VIXL_ASSERT(!masm.AllowMacroInstructions());
      __ nop();
    }
    VIXL_ASSERT(!masm.AllowMacroInstructions());
  }
  VIXL_ASSERT(masm.AllowMacroInstructions());

  {
    InstructionAccurateScope scope(&masm, 2);
    __ add(x0, x0, x0);
    __ sub(x0, x0, x0);
  }

  END();
  RUN();
  TEARDOWN();
}


TEST(blr_lr) {
  // A simple test to check that the simulator correcty handle "blr lr".
  SETUP();

  START();
  Label target;
  Label end;

  __ Mov(x0, 0x0);
  __ Adr(lr, &target);

  __ Blr(lr);
  __ Mov(x0, 0xdeadbeef);
  __ B(&end);

  __ Bind(&target);
  __ Mov(x0, 0xc001c0de);

  __ Bind(&end);
  END();

  RUN();

  ASSERT_EQUAL_64(0xc001c0de, x0);

  TEARDOWN();
}


TEST(barriers) {
  // Generate all supported barriers, this is just a smoke test
  SETUP();

  START();

  // DMB
  __ Dmb(FullSystem, BarrierAll);
  __ Dmb(FullSystem, BarrierReads);
  __ Dmb(FullSystem, BarrierWrites);
  __ Dmb(FullSystem, BarrierOther);

  __ Dmb(InnerShareable, BarrierAll);
  __ Dmb(InnerShareable, BarrierReads);
  __ Dmb(InnerShareable, BarrierWrites);
  __ Dmb(InnerShareable, BarrierOther);

  __ Dmb(NonShareable, BarrierAll);
  __ Dmb(NonShareable, BarrierReads);
  __ Dmb(NonShareable, BarrierWrites);
  __ Dmb(NonShareable, BarrierOther);

  __ Dmb(OuterShareable, BarrierAll);
  __ Dmb(OuterShareable, BarrierReads);
  __ Dmb(OuterShareable, BarrierWrites);
  __ Dmb(OuterShareable, BarrierOther);

  // DSB
  __ Dsb(FullSystem, BarrierAll);
  __ Dsb(FullSystem, BarrierReads);
  __ Dsb(FullSystem, BarrierWrites);
  __ Dsb(FullSystem, BarrierOther);

  __ Dsb(InnerShareable, BarrierAll);
  __ Dsb(InnerShareable, BarrierReads);
  __ Dsb(InnerShareable, BarrierWrites);
  __ Dsb(InnerShareable, BarrierOther);

  __ Dsb(NonShareable, BarrierAll);
  __ Dsb(NonShareable, BarrierReads);
  __ Dsb(NonShareable, BarrierWrites);
  __ Dsb(NonShareable, BarrierOther);

  __ Dsb(OuterShareable, BarrierAll);
  __ Dsb(OuterShareable, BarrierReads);
  __ Dsb(OuterShareable, BarrierWrites);
  __ Dsb(OuterShareable, BarrierOther);

  // ISB
  __ Isb();

  END();

  RUN();

  TEARDOWN();
}


TEST(process_nan_double) {
  // Make sure that NaN propagation works correctly.
  double sn = rawbits_to_double(0x7ff5555511111111);
  double qn = rawbits_to_double(0x7ffaaaaa11111111);
  VIXL_ASSERT(IsSignallingNaN(sn));
  VIXL_ASSERT(IsQuietNaN(qn));

  // The input NaNs after passing through ProcessNaN.
  double sn_proc = rawbits_to_double(0x7ffd555511111111);
  double qn_proc = qn;
  VIXL_ASSERT(IsQuietNaN(sn_proc));
  VIXL_ASSERT(IsQuietNaN(qn_proc));

  SETUP();
  START();

  // Execute a number of instructions which all use ProcessNaN, and check that
  // they all handle the NaN correctly.
  __ Fmov(d0, sn);
  __ Fmov(d10, qn);

  // Operations that always propagate NaNs unchanged, even signalling NaNs.
  //   - Signalling NaN
  __ Fmov(d1, d0);
  __ Fabs(d2, d0);
  __ Fneg(d3, d0);
  //   - Quiet NaN
  __ Fmov(d11, d10);
  __ Fabs(d12, d10);
  __ Fneg(d13, d10);

  // Operations that use ProcessNaN.
  //   - Signalling NaN
  __ Fsqrt(d4, d0);
  __ Frinta(d5, d0);
  __ Frintn(d6, d0);
  __ Frintz(d7, d0);
  //   - Quiet NaN
  __ Fsqrt(d14, d10);
  __ Frinta(d15, d10);
  __ Frintn(d16, d10);
  __ Frintz(d17, d10);

  // The behaviour of fcvt is checked in TEST(fcvt_sd).

  END();
  RUN();

  uint64_t qn_raw = double_to_rawbits(qn);
  uint64_t sn_raw = double_to_rawbits(sn);

  //   - Signalling NaN
  ASSERT_EQUAL_FP64(sn, d1);
  ASSERT_EQUAL_FP64(rawbits_to_double(sn_raw & ~kDSignMask), d2);
  ASSERT_EQUAL_FP64(rawbits_to_double(sn_raw ^ kDSignMask), d3);
  //   - Quiet NaN
  ASSERT_EQUAL_FP64(qn, d11);
  ASSERT_EQUAL_FP64(rawbits_to_double(qn_raw & ~kDSignMask), d12);
  ASSERT_EQUAL_FP64(rawbits_to_double(qn_raw ^ kDSignMask), d13);

  //   - Signalling NaN
  ASSERT_EQUAL_FP64(sn_proc, d4);
  ASSERT_EQUAL_FP64(sn_proc, d5);
  ASSERT_EQUAL_FP64(sn_proc, d6);
  ASSERT_EQUAL_FP64(sn_proc, d7);
  //   - Quiet NaN
  ASSERT_EQUAL_FP64(qn_proc, d14);
  ASSERT_EQUAL_FP64(qn_proc, d15);
  ASSERT_EQUAL_FP64(qn_proc, d16);
  ASSERT_EQUAL_FP64(qn_proc, d17);

  TEARDOWN();
}


TEST(process_nan_float) {
  // Make sure that NaN propagation works correctly.
  float sn = rawbits_to_float(0x7f951111);
  float qn = rawbits_to_float(0x7fea1111);
  VIXL_ASSERT(IsSignallingNaN(sn));
  VIXL_ASSERT(IsQuietNaN(qn));

  // The input NaNs after passing through ProcessNaN.
  float sn_proc = rawbits_to_float(0x7fd51111);
  float qn_proc = qn;
  VIXL_ASSERT(IsQuietNaN(sn_proc));
  VIXL_ASSERT(IsQuietNaN(qn_proc));

  SETUP();
  START();

  // Execute a number of instructions which all use ProcessNaN, and check that
  // they all handle the NaN correctly.
  __ Fmov(s0, sn);
  __ Fmov(s10, qn);

  // Operations that always propagate NaNs unchanged, even signalling NaNs.
  //   - Signalling NaN
  __ Fmov(s1, s0);
  __ Fabs(s2, s0);
  __ Fneg(s3, s0);
  //   - Quiet NaN
  __ Fmov(s11, s10);
  __ Fabs(s12, s10);
  __ Fneg(s13, s10);

  // Operations that use ProcessNaN.
  //   - Signalling NaN
  __ Fsqrt(s4, s0);
  __ Frinta(s5, s0);
  __ Frintn(s6, s0);
  __ Frintz(s7, s0);
  //   - Quiet NaN
  __ Fsqrt(s14, s10);
  __ Frinta(s15, s10);
  __ Frintn(s16, s10);
  __ Frintz(s17, s10);

  // The behaviour of fcvt is checked in TEST(fcvt_sd).

  END();
  RUN();

  uint32_t qn_raw = float_to_rawbits(qn);
  uint32_t sn_raw = float_to_rawbits(sn);

  //   - Signalling NaN
  ASSERT_EQUAL_FP32(sn, s1);
  ASSERT_EQUAL_FP32(rawbits_to_float(sn_raw & ~kSSignMask), s2);
  ASSERT_EQUAL_FP32(rawbits_to_float(sn_raw ^ kSSignMask), s3);
  //   - Quiet NaN
  ASSERT_EQUAL_FP32(qn, s11);
  ASSERT_EQUAL_FP32(rawbits_to_float(qn_raw & ~kSSignMask), s12);
  ASSERT_EQUAL_FP32(rawbits_to_float(qn_raw ^ kSSignMask), s13);

  //   - Signalling NaN
  ASSERT_EQUAL_FP32(sn_proc, s4);
  ASSERT_EQUAL_FP32(sn_proc, s5);
  ASSERT_EQUAL_FP32(sn_proc, s6);
  ASSERT_EQUAL_FP32(sn_proc, s7);
  //   - Quiet NaN
  ASSERT_EQUAL_FP32(qn_proc, s14);
  ASSERT_EQUAL_FP32(qn_proc, s15);
  ASSERT_EQUAL_FP32(qn_proc, s16);
  ASSERT_EQUAL_FP32(qn_proc, s17);

  TEARDOWN();
}


static void ProcessNaNsHelper(double n, double m, double expected) {
  VIXL_ASSERT(std::isnan(n) || std::isnan(m));
  VIXL_ASSERT(std::isnan(expected));

  SETUP();
  START();

  // Execute a number of instructions which all use ProcessNaNs, and check that
  // they all propagate NaNs correctly.
  __ Fmov(d0, n);
  __ Fmov(d1, m);

  __ Fadd(d2, d0, d1);
  __ Fsub(d3, d0, d1);
  __ Fmul(d4, d0, d1);
  __ Fdiv(d5, d0, d1);
  __ Fmax(d6, d0, d1);
  __ Fmin(d7, d0, d1);

  END();
  RUN();

  ASSERT_EQUAL_FP64(expected, d2);
  ASSERT_EQUAL_FP64(expected, d3);
  ASSERT_EQUAL_FP64(expected, d4);
  ASSERT_EQUAL_FP64(expected, d5);
  ASSERT_EQUAL_FP64(expected, d6);
  ASSERT_EQUAL_FP64(expected, d7);

  TEARDOWN();
}


TEST(process_nans_double) {
  // Make sure that NaN propagation works correctly.
  double sn = rawbits_to_double(0x7ff5555511111111);
  double sm = rawbits_to_double(0x7ff5555522222222);
  double qn = rawbits_to_double(0x7ffaaaaa11111111);
  double qm = rawbits_to_double(0x7ffaaaaa22222222);
  VIXL_ASSERT(IsSignallingNaN(sn));
  VIXL_ASSERT(IsSignallingNaN(sm));
  VIXL_ASSERT(IsQuietNaN(qn));
  VIXL_ASSERT(IsQuietNaN(qm));

  // The input NaNs after passing through ProcessNaN.
  double sn_proc = rawbits_to_double(0x7ffd555511111111);
  double sm_proc = rawbits_to_double(0x7ffd555522222222);
  double qn_proc = qn;
  double qm_proc = qm;
  VIXL_ASSERT(IsQuietNaN(sn_proc));
  VIXL_ASSERT(IsQuietNaN(sm_proc));
  VIXL_ASSERT(IsQuietNaN(qn_proc));
  VIXL_ASSERT(IsQuietNaN(qm_proc));

  // Quiet NaNs are propagated.
  ProcessNaNsHelper(qn, 0, qn_proc);
  ProcessNaNsHelper(0, qm, qm_proc);
  ProcessNaNsHelper(qn, qm, qn_proc);

  // Signalling NaNs are propagated, and made quiet.
  ProcessNaNsHelper(sn, 0, sn_proc);
  ProcessNaNsHelper(0, sm, sm_proc);
  ProcessNaNsHelper(sn, sm, sn_proc);

  // Signalling NaNs take precedence over quiet NaNs.
  ProcessNaNsHelper(sn, qm, sn_proc);
  ProcessNaNsHelper(qn, sm, sm_proc);
  ProcessNaNsHelper(sn, sm, sn_proc);
}


static void ProcessNaNsHelper(float n, float m, float expected) {
  VIXL_ASSERT(std::isnan(n) || std::isnan(m));
  VIXL_ASSERT(std::isnan(expected));

  SETUP();
  START();

  // Execute a number of instructions which all use ProcessNaNs, and check that
  // they all propagate NaNs correctly.
  __ Fmov(s0, n);
  __ Fmov(s1, m);

  __ Fadd(s2, s0, s1);
  __ Fsub(s3, s0, s1);
  __ Fmul(s4, s0, s1);
  __ Fdiv(s5, s0, s1);
  __ Fmax(s6, s0, s1);
  __ Fmin(s7, s0, s1);

  END();
  RUN();

  ASSERT_EQUAL_FP32(expected, s2);
  ASSERT_EQUAL_FP32(expected, s3);
  ASSERT_EQUAL_FP32(expected, s4);
  ASSERT_EQUAL_FP32(expected, s5);
  ASSERT_EQUAL_FP32(expected, s6);
  ASSERT_EQUAL_FP32(expected, s7);

  TEARDOWN();
}


TEST(process_nans_float) {
  // Make sure that NaN propagation works correctly.
  float sn = rawbits_to_float(0x7f951111);
  float sm = rawbits_to_float(0x7f952222);
  float qn = rawbits_to_float(0x7fea1111);
  float qm = rawbits_to_float(0x7fea2222);
  VIXL_ASSERT(IsSignallingNaN(sn));
  VIXL_ASSERT(IsSignallingNaN(sm));
  VIXL_ASSERT(IsQuietNaN(qn));
  VIXL_ASSERT(IsQuietNaN(qm));

  // The input NaNs after passing through ProcessNaN.
  float sn_proc = rawbits_to_float(0x7fd51111);
  float sm_proc = rawbits_to_float(0x7fd52222);
  float qn_proc = qn;
  float qm_proc = qm;
  VIXL_ASSERT(IsQuietNaN(sn_proc));
  VIXL_ASSERT(IsQuietNaN(sm_proc));
  VIXL_ASSERT(IsQuietNaN(qn_proc));
  VIXL_ASSERT(IsQuietNaN(qm_proc));

  // Quiet NaNs are propagated.
  ProcessNaNsHelper(qn, 0, qn_proc);
  ProcessNaNsHelper(0, qm, qm_proc);
  ProcessNaNsHelper(qn, qm, qn_proc);

  // Signalling NaNs are propagated, and made quiet.
  ProcessNaNsHelper(sn, 0, sn_proc);
  ProcessNaNsHelper(0, sm, sm_proc);
  ProcessNaNsHelper(sn, sm, sn_proc);

  // Signalling NaNs take precedence over quiet NaNs.
  ProcessNaNsHelper(sn, qm, sn_proc);
  ProcessNaNsHelper(qn, sm, sm_proc);
  ProcessNaNsHelper(sn, sm, sn_proc);
}


static void DefaultNaNHelper(float n, float m, float a) {
  VIXL_ASSERT(std::isnan(n) || std::isnan(m) || std::isnan(a));

  bool test_1op = std::isnan(n);
  bool test_2op = std::isnan(n) || std::isnan(m);

  SETUP();
  START();

  // Enable Default-NaN mode in the FPCR.
  __ Mrs(x0, FPCR);
  __ Orr(x1, x0, DN_mask);
  __ Msr(FPCR, x1);

  // Execute a number of instructions which all use ProcessNaNs, and check that
  // they all produce the default NaN.
  __ Fmov(s0, n);
  __ Fmov(s1, m);
  __ Fmov(s2, a);

  if (test_1op) {
    // Operations that always propagate NaNs unchanged, even signalling NaNs.
    __ Fmov(s10, s0);
    __ Fabs(s11, s0);
    __ Fneg(s12, s0);

    // Operations that use ProcessNaN.
    __ Fsqrt(s13, s0);
    __ Frinta(s14, s0);
    __ Frintn(s15, s0);
    __ Frintz(s16, s0);

    // Fcvt usually has special NaN handling, but it respects default-NaN mode.
    __ Fcvt(d17, s0);
  }

  if (test_2op) {
    __ Fadd(s18, s0, s1);
    __ Fsub(s19, s0, s1);
    __ Fmul(s20, s0, s1);
    __ Fdiv(s21, s0, s1);
    __ Fmax(s22, s0, s1);
    __ Fmin(s23, s0, s1);
  }

  __ Fmadd(s24, s0, s1, s2);
  __ Fmsub(s25, s0, s1, s2);
  __ Fnmadd(s26, s0, s1, s2);
  __ Fnmsub(s27, s0, s1, s2);

  // Restore FPCR.
  __ Msr(FPCR, x0);

  END();
  RUN();

  if (test_1op) {
    uint32_t n_raw = float_to_rawbits(n);
    ASSERT_EQUAL_FP32(n, s10);
    ASSERT_EQUAL_FP32(rawbits_to_float(n_raw & ~kSSignMask), s11);
    ASSERT_EQUAL_FP32(rawbits_to_float(n_raw ^ kSSignMask), s12);
    ASSERT_EQUAL_FP32(kFP32DefaultNaN, s13);
    ASSERT_EQUAL_FP32(kFP32DefaultNaN, s14);
    ASSERT_EQUAL_FP32(kFP32DefaultNaN, s15);
    ASSERT_EQUAL_FP32(kFP32DefaultNaN, s16);
    ASSERT_EQUAL_FP64(kFP64DefaultNaN, d17);
  }

  if (test_2op) {
    ASSERT_EQUAL_FP32(kFP32DefaultNaN, s18);
    ASSERT_EQUAL_FP32(kFP32DefaultNaN, s19);
    ASSERT_EQUAL_FP32(kFP32DefaultNaN, s20);
    ASSERT_EQUAL_FP32(kFP32DefaultNaN, s21);
    ASSERT_EQUAL_FP32(kFP32DefaultNaN, s22);
    ASSERT_EQUAL_FP32(kFP32DefaultNaN, s23);
  }

  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s24);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s25);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s26);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s27);

  TEARDOWN();
}


TEST(default_nan_float) {
  float sn = rawbits_to_float(0x7f951111);
  float sm = rawbits_to_float(0x7f952222);
  float sa = rawbits_to_float(0x7f95aaaa);
  float qn = rawbits_to_float(0x7fea1111);
  float qm = rawbits_to_float(0x7fea2222);
  float qa = rawbits_to_float(0x7feaaaaa);
  VIXL_ASSERT(IsSignallingNaN(sn));
  VIXL_ASSERT(IsSignallingNaN(sm));
  VIXL_ASSERT(IsSignallingNaN(sa));
  VIXL_ASSERT(IsQuietNaN(qn));
  VIXL_ASSERT(IsQuietNaN(qm));
  VIXL_ASSERT(IsQuietNaN(qa));

  //   - Signalling NaNs
  DefaultNaNHelper(sn, 0.0f, 0.0f);
  DefaultNaNHelper(0.0f, sm, 0.0f);
  DefaultNaNHelper(0.0f, 0.0f, sa);
  DefaultNaNHelper(sn, sm, 0.0f);
  DefaultNaNHelper(0.0f, sm, sa);
  DefaultNaNHelper(sn, 0.0f, sa);
  DefaultNaNHelper(sn, sm, sa);
  //   - Quiet NaNs
  DefaultNaNHelper(qn, 0.0f, 0.0f);
  DefaultNaNHelper(0.0f, qm, 0.0f);
  DefaultNaNHelper(0.0f, 0.0f, qa);
  DefaultNaNHelper(qn, qm, 0.0f);
  DefaultNaNHelper(0.0f, qm, qa);
  DefaultNaNHelper(qn, 0.0f, qa);
  DefaultNaNHelper(qn, qm, qa);
  //   - Mixed NaNs
  DefaultNaNHelper(qn, sm, sa);
  DefaultNaNHelper(sn, qm, sa);
  DefaultNaNHelper(sn, sm, qa);
  DefaultNaNHelper(qn, qm, sa);
  DefaultNaNHelper(sn, qm, qa);
  DefaultNaNHelper(qn, sm, qa);
  DefaultNaNHelper(qn, qm, qa);
}


static void DefaultNaNHelper(double n, double m, double a) {
  VIXL_ASSERT(std::isnan(n) || std::isnan(m) || std::isnan(a));

  bool test_1op = std::isnan(n);
  bool test_2op = std::isnan(n) || std::isnan(m);

  SETUP();
  START();

  // Enable Default-NaN mode in the FPCR.
  __ Mrs(x0, FPCR);
  __ Orr(x1, x0, DN_mask);
  __ Msr(FPCR, x1);

  // Execute a number of instructions which all use ProcessNaNs, and check that
  // they all produce the default NaN.
  __ Fmov(d0, n);
  __ Fmov(d1, m);
  __ Fmov(d2, a);

  if (test_1op) {
    // Operations that always propagate NaNs unchanged, even signalling NaNs.
    __ Fmov(d10, d0);
    __ Fabs(d11, d0);
    __ Fneg(d12, d0);

    // Operations that use ProcessNaN.
    __ Fsqrt(d13, d0);
    __ Frinta(d14, d0);
    __ Frintn(d15, d0);
    __ Frintz(d16, d0);

    // Fcvt usually has special NaN handling, but it respects default-NaN mode.
    __ Fcvt(s17, d0);
  }

  if (test_2op) {
    __ Fadd(d18, d0, d1);
    __ Fsub(d19, d0, d1);
    __ Fmul(d20, d0, d1);
    __ Fdiv(d21, d0, d1);
    __ Fmax(d22, d0, d1);
    __ Fmin(d23, d0, d1);
  }

  __ Fmadd(d24, d0, d1, d2);
  __ Fmsub(d25, d0, d1, d2);
  __ Fnmadd(d26, d0, d1, d2);
  __ Fnmsub(d27, d0, d1, d2);

  // Restore FPCR.
  __ Msr(FPCR, x0);

  END();
  RUN();

  if (test_1op) {
    uint64_t n_raw = double_to_rawbits(n);
    ASSERT_EQUAL_FP64(n, d10);
    ASSERT_EQUAL_FP64(rawbits_to_double(n_raw & ~kDSignMask), d11);
    ASSERT_EQUAL_FP64(rawbits_to_double(n_raw ^ kDSignMask), d12);
    ASSERT_EQUAL_FP64(kFP64DefaultNaN, d13);
    ASSERT_EQUAL_FP64(kFP64DefaultNaN, d14);
    ASSERT_EQUAL_FP64(kFP64DefaultNaN, d15);
    ASSERT_EQUAL_FP64(kFP64DefaultNaN, d16);
    ASSERT_EQUAL_FP32(kFP32DefaultNaN, s17);
  }

  if (test_2op) {
    ASSERT_EQUAL_FP64(kFP64DefaultNaN, d18);
    ASSERT_EQUAL_FP64(kFP64DefaultNaN, d19);
    ASSERT_EQUAL_FP64(kFP64DefaultNaN, d20);
    ASSERT_EQUAL_FP64(kFP64DefaultNaN, d21);
    ASSERT_EQUAL_FP64(kFP64DefaultNaN, d22);
    ASSERT_EQUAL_FP64(kFP64DefaultNaN, d23);
  }

  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d24);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d25);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d26);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d27);

  TEARDOWN();
}


TEST(default_nan_double) {
  double sn = rawbits_to_double(0x7ff5555511111111);
  double sm = rawbits_to_double(0x7ff5555522222222);
  double sa = rawbits_to_double(0x7ff55555aaaaaaaa);
  double qn = rawbits_to_double(0x7ffaaaaa11111111);
  double qm = rawbits_to_double(0x7ffaaaaa22222222);
  double qa = rawbits_to_double(0x7ffaaaaaaaaaaaaa);
  VIXL_ASSERT(IsSignallingNaN(sn));
  VIXL_ASSERT(IsSignallingNaN(sm));
  VIXL_ASSERT(IsSignallingNaN(sa));
  VIXL_ASSERT(IsQuietNaN(qn));
  VIXL_ASSERT(IsQuietNaN(qm));
  VIXL_ASSERT(IsQuietNaN(qa));

  //   - Signalling NaNs
  DefaultNaNHelper(sn, 0.0, 0.0);
  DefaultNaNHelper(0.0, sm, 0.0);
  DefaultNaNHelper(0.0, 0.0, sa);
  DefaultNaNHelper(sn, sm, 0.0);
  DefaultNaNHelper(0.0, sm, sa);
  DefaultNaNHelper(sn, 0.0, sa);
  DefaultNaNHelper(sn, sm, sa);
  //   - Quiet NaNs
  DefaultNaNHelper(qn, 0.0, 0.0);
  DefaultNaNHelper(0.0, qm, 0.0);
  DefaultNaNHelper(0.0, 0.0, qa);
  DefaultNaNHelper(qn, qm, 0.0);
  DefaultNaNHelper(0.0, qm, qa);
  DefaultNaNHelper(qn, 0.0, qa);
  DefaultNaNHelper(qn, qm, qa);
  //   - Mixed NaNs
  DefaultNaNHelper(qn, sm, sa);
  DefaultNaNHelper(sn, qm, sa);
  DefaultNaNHelper(sn, sm, qa);
  DefaultNaNHelper(qn, qm, sa);
  DefaultNaNHelper(sn, qm, qa);
  DefaultNaNHelper(qn, sm, qa);
  DefaultNaNHelper(qn, qm, qa);
}


TEST(ldar_stlr) {
  // The middle value is read, modified, and written. The padding exists only to
  // check for over-write.
  uint8_t b[] = {0, 0x12, 0};
  uint16_t h[] = {0, 0x1234, 0};
  uint32_t w[] = {0, 0x12345678, 0};
  uint64_t x[] = {0, 0x123456789abcdef0, 0};

  SETUP();
  START();

  __ Mov(x10, reinterpret_cast<uintptr_t>(&b[1]));
  __ Ldarb(w0, MemOperand(x10));
  __ Add(w0, w0, 1);
  __ Stlrb(w0, MemOperand(x10));

  __ Mov(x10, reinterpret_cast<uintptr_t>(&h[1]));
  __ Ldarh(w0, MemOperand(x10));
  __ Add(w0, w0, 1);
  __ Stlrh(w0, MemOperand(x10));

  __ Mov(x10, reinterpret_cast<uintptr_t>(&w[1]));
  __ Ldar(w0, MemOperand(x10));
  __ Add(w0, w0, 1);
  __ Stlr(w0, MemOperand(x10));

  __ Mov(x10, reinterpret_cast<uintptr_t>(&x[1]));
  __ Ldar(x0, MemOperand(x10));
  __ Add(x0, x0, 1);
  __ Stlr(x0, MemOperand(x10));

  END();
  RUN();

  ASSERT_EQUAL_32(0x13, b[1]);
  ASSERT_EQUAL_32(0x1235, h[1]);
  ASSERT_EQUAL_32(0x12345679, w[1]);
  ASSERT_EQUAL_64(0x123456789abcdef1, x[1]);

  // Check for over-write.
  ASSERT_EQUAL_32(0, b[0]);
  ASSERT_EQUAL_32(0, b[2]);
  ASSERT_EQUAL_32(0, h[0]);
  ASSERT_EQUAL_32(0, h[2]);
  ASSERT_EQUAL_32(0, w[0]);
  ASSERT_EQUAL_32(0, w[2]);
  ASSERT_EQUAL_64(0, x[0]);
  ASSERT_EQUAL_64(0, x[2]);

  TEARDOWN();
}


TEST(ldxr_stxr) {
  // The middle value is read, modified, and written. The padding exists only to
  // check for over-write.
  uint8_t b[] = {0, 0x12, 0};
  uint16_t h[] = {0, 0x1234, 0};
  uint32_t w[] = {0, 0x12345678, 0};
  uint64_t x[] = {0, 0x123456789abcdef0, 0};

  // As above, but get suitably-aligned values for ldxp and stxp.
  uint32_t wp_data[] = {0, 0, 0, 0, 0};
  uint32_t * wp = AlignUp(wp_data + 1, kWRegSizeInBytes * 2) - 1;
  wp[1] = 0x12345678;           // wp[1] is 64-bit-aligned.
  wp[2] = 0x87654321;
  uint64_t xp_data[] = {0, 0, 0, 0, 0};
  uint64_t * xp = AlignUp(xp_data + 1, kXRegSizeInBytes * 2) - 1;
  xp[1] = 0x123456789abcdef0;   // xp[1] is 128-bit-aligned.
  xp[2] = 0x0fedcba987654321;

  SETUP();
  START();

  __ Mov(x10, reinterpret_cast<uintptr_t>(&b[1]));
  Label try_b;
  __ Bind(&try_b);
  __ Ldxrb(w0, MemOperand(x10));
  __ Add(w0, w0, 1);
  __ Stxrb(w5, w0, MemOperand(x10));
  __ Cbnz(w5, &try_b);

  __ Mov(x10, reinterpret_cast<uintptr_t>(&h[1]));
  Label try_h;
  __ Bind(&try_h);
  __ Ldxrh(w0, MemOperand(x10));
  __ Add(w0, w0, 1);
  __ Stxrh(w5, w0, MemOperand(x10));
  __ Cbnz(w5, &try_h);

  __ Mov(x10, reinterpret_cast<uintptr_t>(&w[1]));
  Label try_w;
  __ Bind(&try_w);
  __ Ldxr(w0, MemOperand(x10));
  __ Add(w0, w0, 1);
  __ Stxr(w5, w0, MemOperand(x10));
  __ Cbnz(w5, &try_w);

  __ Mov(x10, reinterpret_cast<uintptr_t>(&x[1]));
  Label try_x;
  __ Bind(&try_x);
  __ Ldxr(x0, MemOperand(x10));
  __ Add(x0, x0, 1);
  __ Stxr(w5, x0, MemOperand(x10));
  __ Cbnz(w5, &try_x);

  __ Mov(x10, reinterpret_cast<uintptr_t>(&wp[1]));
  Label try_wp;
  __ Bind(&try_wp);
  __ Ldxp(w0, w1, MemOperand(x10));
  __ Add(w0, w0, 1);
  __ Add(w1, w1, 1);
  __ Stxp(w5, w0, w1, MemOperand(x10));
  __ Cbnz(w5, &try_wp);

  __ Mov(x10, reinterpret_cast<uintptr_t>(&xp[1]));
  Label try_xp;
  __ Bind(&try_xp);
  __ Ldxp(x0, x1, MemOperand(x10));
  __ Add(x0, x0, 1);
  __ Add(x1, x1, 1);
  __ Stxp(w5, x0, x1, MemOperand(x10));
  __ Cbnz(w5, &try_xp);

  END();
  RUN();

  ASSERT_EQUAL_32(0x13, b[1]);
  ASSERT_EQUAL_32(0x1235, h[1]);
  ASSERT_EQUAL_32(0x12345679, w[1]);
  ASSERT_EQUAL_64(0x123456789abcdef1, x[1]);
  ASSERT_EQUAL_32(0x12345679, wp[1]);
  ASSERT_EQUAL_32(0x87654322, wp[2]);
  ASSERT_EQUAL_64(0x123456789abcdef1, xp[1]);
  ASSERT_EQUAL_64(0x0fedcba987654322, xp[2]);

  // Check for over-write.
  ASSERT_EQUAL_32(0, b[0]);
  ASSERT_EQUAL_32(0, b[2]);
  ASSERT_EQUAL_32(0, h[0]);
  ASSERT_EQUAL_32(0, h[2]);
  ASSERT_EQUAL_32(0, w[0]);
  ASSERT_EQUAL_32(0, w[2]);
  ASSERT_EQUAL_64(0, x[0]);
  ASSERT_EQUAL_64(0, x[2]);
  ASSERT_EQUAL_32(0, wp[0]);
  ASSERT_EQUAL_32(0, wp[3]);
  ASSERT_EQUAL_64(0, xp[0]);
  ASSERT_EQUAL_64(0, xp[3]);

  TEARDOWN();
}


TEST(ldaxr_stlxr) {
  // The middle value is read, modified, and written. The padding exists only to
  // check for over-write.
  uint8_t b[] = {0, 0x12, 0};
  uint16_t h[] = {0, 0x1234, 0};
  uint32_t w[] = {0, 0x12345678, 0};
  uint64_t x[] = {0, 0x123456789abcdef0, 0};

  // As above, but get suitably-aligned values for ldxp and stxp.
  uint32_t wp_data[] = {0, 0, 0, 0, 0};
  uint32_t * wp = AlignUp(wp_data + 1, kWRegSizeInBytes * 2) - 1;
  wp[1] = 0x12345678;           // wp[1] is 64-bit-aligned.
  wp[2] = 0x87654321;
  uint64_t xp_data[] = {0, 0, 0, 0, 0};
  uint64_t * xp = AlignUp(xp_data + 1, kXRegSizeInBytes * 2) - 1;
  xp[1] = 0x123456789abcdef0;   // xp[1] is 128-bit-aligned.
  xp[2] = 0x0fedcba987654321;

  SETUP();
  START();

  __ Mov(x10, reinterpret_cast<uintptr_t>(&b[1]));
  Label try_b;
  __ Bind(&try_b);
  __ Ldaxrb(w0, MemOperand(x10));
  __ Add(w0, w0, 1);
  __ Stlxrb(w5, w0, MemOperand(x10));
  __ Cbnz(w5, &try_b);

  __ Mov(x10, reinterpret_cast<uintptr_t>(&h[1]));
  Label try_h;
  __ Bind(&try_h);
  __ Ldaxrh(w0, MemOperand(x10));
  __ Add(w0, w0, 1);
  __ Stlxrh(w5, w0, MemOperand(x10));
  __ Cbnz(w5, &try_h);

  __ Mov(x10, reinterpret_cast<uintptr_t>(&w[1]));
  Label try_w;
  __ Bind(&try_w);
  __ Ldaxr(w0, MemOperand(x10));
  __ Add(w0, w0, 1);
  __ Stlxr(w5, w0, MemOperand(x10));
  __ Cbnz(w5, &try_w);

  __ Mov(x10, reinterpret_cast<uintptr_t>(&x[1]));
  Label try_x;
  __ Bind(&try_x);
  __ Ldaxr(x0, MemOperand(x10));
  __ Add(x0, x0, 1);
  __ Stlxr(w5, x0, MemOperand(x10));
  __ Cbnz(w5, &try_x);

  __ Mov(x10, reinterpret_cast<uintptr_t>(&wp[1]));
  Label try_wp;
  __ Bind(&try_wp);
  __ Ldaxp(w0, w1, MemOperand(x10));
  __ Add(w0, w0, 1);
  __ Add(w1, w1, 1);
  __ Stlxp(w5, w0, w1, MemOperand(x10));
  __ Cbnz(w5, &try_wp);

  __ Mov(x10, reinterpret_cast<uintptr_t>(&xp[1]));
  Label try_xp;
  __ Bind(&try_xp);
  __ Ldaxp(x0, x1, MemOperand(x10));
  __ Add(x0, x0, 1);
  __ Add(x1, x1, 1);
  __ Stlxp(w5, x0, x1, MemOperand(x10));
  __ Cbnz(w5, &try_xp);

  END();
  RUN();

  ASSERT_EQUAL_32(0x13, b[1]);
  ASSERT_EQUAL_32(0x1235, h[1]);
  ASSERT_EQUAL_32(0x12345679, w[1]);
  ASSERT_EQUAL_64(0x123456789abcdef1, x[1]);
  ASSERT_EQUAL_32(0x12345679, wp[1]);
  ASSERT_EQUAL_32(0x87654322, wp[2]);
  ASSERT_EQUAL_64(0x123456789abcdef1, xp[1]);
  ASSERT_EQUAL_64(0x0fedcba987654322, xp[2]);

  // Check for over-write.
  ASSERT_EQUAL_32(0, b[0]);
  ASSERT_EQUAL_32(0, b[2]);
  ASSERT_EQUAL_32(0, h[0]);
  ASSERT_EQUAL_32(0, h[2]);
  ASSERT_EQUAL_32(0, w[0]);
  ASSERT_EQUAL_32(0, w[2]);
  ASSERT_EQUAL_64(0, x[0]);
  ASSERT_EQUAL_64(0, x[2]);
  ASSERT_EQUAL_32(0, wp[0]);
  ASSERT_EQUAL_32(0, wp[3]);
  ASSERT_EQUAL_64(0, xp[0]);
  ASSERT_EQUAL_64(0, xp[3]);

  TEARDOWN();
}


TEST(clrex) {
  // This data should never be written.
  uint64_t data[] = {0, 0, 0};
  uint64_t * data_aligned = AlignUp(data, kXRegSizeInBytes * 2);

  SETUP();
  START();

  __ Mov(x10, reinterpret_cast<uintptr_t>(data_aligned));
  __ Mov(w6, 0);

  __ Ldxrb(w0, MemOperand(x10));
  __ Clrex();
  __ Add(w0, w0, 1);
  __ Stxrb(w5, w0, MemOperand(x10));
  __ Add(w6, w6, w5);

  __ Ldxrh(w0, MemOperand(x10));
  __ Clrex();
  __ Add(w0, w0, 1);
  __ Stxrh(w5, w0, MemOperand(x10));
  __ Add(w6, w6, w5);

  __ Ldxr(w0, MemOperand(x10));
  __ Clrex();
  __ Add(w0, w0, 1);
  __ Stxr(w5, w0, MemOperand(x10));
  __ Add(w6, w6, w5);

  __ Ldxr(x0, MemOperand(x10));
  __ Clrex();
  __ Add(x0, x0, 1);
  __ Stxr(w5, x0, MemOperand(x10));
  __ Add(w6, w6, w5);

  __ Ldxp(w0, w1, MemOperand(x10));
  __ Clrex();
  __ Add(w0, w0, 1);
  __ Add(w1, w1, 1);
  __ Stxp(w5, w0, w1, MemOperand(x10));
  __ Add(w6, w6, w5);

  __ Ldxp(x0, x1, MemOperand(x10));
  __ Clrex();
  __ Add(x0, x0, 1);
  __ Add(x1, x1, 1);
  __ Stxp(w5, x0, x1, MemOperand(x10));
  __ Add(w6, w6, w5);

  // Acquire-release variants.

  __ Ldaxrb(w0, MemOperand(x10));
  __ Clrex();
  __ Add(w0, w0, 1);
  __ Stlxrb(w5, w0, MemOperand(x10));
  __ Add(w6, w6, w5);

  __ Ldaxrh(w0, MemOperand(x10));
  __ Clrex();
  __ Add(w0, w0, 1);
  __ Stlxrh(w5, w0, MemOperand(x10));
  __ Add(w6, w6, w5);

  __ Ldaxr(w0, MemOperand(x10));
  __ Clrex();
  __ Add(w0, w0, 1);
  __ Stlxr(w5, w0, MemOperand(x10));
  __ Add(w6, w6, w5);

  __ Ldaxr(x0, MemOperand(x10));
  __ Clrex();
  __ Add(x0, x0, 1);
  __ Stlxr(w5, x0, MemOperand(x10));
  __ Add(w6, w6, w5);

  __ Ldaxp(w0, w1, MemOperand(x10));
  __ Clrex();
  __ Add(w0, w0, 1);
  __ Add(w1, w1, 1);
  __ Stlxp(w5, w0, w1, MemOperand(x10));
  __ Add(w6, w6, w5);

  __ Ldaxp(x0, x1, MemOperand(x10));
  __ Clrex();
  __ Add(x0, x0, 1);
  __ Add(x1, x1, 1);
  __ Stlxp(w5, x0, x1, MemOperand(x10));
  __ Add(w6, w6, w5);

  END();
  RUN();

  // None of the 12 store-exclusives should have succeeded.
  ASSERT_EQUAL_32(12, w6);

  ASSERT_EQUAL_64(0, data[0]);
  ASSERT_EQUAL_64(0, data[1]);
  ASSERT_EQUAL_64(0, data[2]);

  TEARDOWN();
}


#ifdef VIXL_INCLUDE_SIMULATOR
// Check that the simulator occasionally makes store-exclusive fail.
TEST(ldxr_stxr_fail) {
  uint64_t data[] = {0, 0, 0};
  uint64_t * data_aligned = AlignUp(data, kXRegSizeInBytes * 2);

  // Impose a hard limit on the number of attempts, so the test cannot hang.
  static const uint64_t kWatchdog = 10000;
  Label done;

  SETUP();
  START();

  __ Mov(x10, reinterpret_cast<uintptr_t>(data_aligned));
  __ Mov(x11, kWatchdog);

  // This loop is the opposite of what we normally do with ldxr and stxr; we
  // keep trying until we fail (or the watchdog counter runs out).
  Label try_b;
  __ Bind(&try_b);
  __ Ldxrb(w0, MemOperand(x10));
  __ Stxrb(w5, w0, MemOperand(x10));
  // Check the watchdog counter.
  __ Sub(x11, x11, 1);
  __ Cbz(x11, &done);
  // Check the exclusive-store result.
  __ Cbz(w5, &try_b);

  Label try_h;
  __ Bind(&try_h);
  __ Ldxrh(w0, MemOperand(x10));
  __ Stxrh(w5, w0, MemOperand(x10));
  __ Sub(x11, x11, 1);
  __ Cbz(x11, &done);
  __ Cbz(w5, &try_h);

  Label try_w;
  __ Bind(&try_w);
  __ Ldxr(w0, MemOperand(x10));
  __ Stxr(w5, w0, MemOperand(x10));
  __ Sub(x11, x11, 1);
  __ Cbz(x11, &done);
  __ Cbz(w5, &try_w);

  Label try_x;
  __ Bind(&try_x);
  __ Ldxr(x0, MemOperand(x10));
  __ Stxr(w5, x0, MemOperand(x10));
  __ Sub(x11, x11, 1);
  __ Cbz(x11, &done);
  __ Cbz(w5, &try_x);

  Label try_wp;
  __ Bind(&try_wp);
  __ Ldxp(w0, w1, MemOperand(x10));
  __ Stxp(w5, w0, w1, MemOperand(x10));
  __ Sub(x11, x11, 1);
  __ Cbz(x11, &done);
  __ Cbz(w5, &try_wp);

  Label try_xp;
  __ Bind(&try_xp);
  __ Ldxp(x0, x1, MemOperand(x10));
  __ Stxp(w5, x0, x1, MemOperand(x10));
  __ Sub(x11, x11, 1);
  __ Cbz(x11, &done);
  __ Cbz(w5, &try_xp);

  __ Bind(&done);
  // Trigger an error if x11 (watchdog) is zero.
  __ Cmp(x11, 0);
  __ Cset(x12, eq);

  END();
  RUN();

  // Check that the watchdog counter didn't run out.
  ASSERT_EQUAL_64(0, x12);

  TEARDOWN();
}
#endif


#ifdef VIXL_INCLUDE_SIMULATOR
// Check that the simulator occasionally makes store-exclusive fail.
TEST(ldaxr_stlxr_fail) {
  uint64_t data[] = {0, 0, 0};
  uint64_t * data_aligned = AlignUp(data, kXRegSizeInBytes * 2);

  // Impose a hard limit on the number of attempts, so the test cannot hang.
  static const uint64_t kWatchdog = 10000;
  Label done;

  SETUP();
  START();

  __ Mov(x10, reinterpret_cast<uintptr_t>(data_aligned));
  __ Mov(x11, kWatchdog);

  // This loop is the opposite of what we normally do with ldxr and stxr; we
  // keep trying until we fail (or the watchdog counter runs out).
  Label try_b;
  __ Bind(&try_b);
  __ Ldxrb(w0, MemOperand(x10));
  __ Stxrb(w5, w0, MemOperand(x10));
  // Check the watchdog counter.
  __ Sub(x11, x11, 1);
  __ Cbz(x11, &done);
  // Check the exclusive-store result.
  __ Cbz(w5, &try_b);

  Label try_h;
  __ Bind(&try_h);
  __ Ldaxrh(w0, MemOperand(x10));
  __ Stlxrh(w5, w0, MemOperand(x10));
  __ Sub(x11, x11, 1);
  __ Cbz(x11, &done);
  __ Cbz(w5, &try_h);

  Label try_w;
  __ Bind(&try_w);
  __ Ldaxr(w0, MemOperand(x10));
  __ Stlxr(w5, w0, MemOperand(x10));
  __ Sub(x11, x11, 1);
  __ Cbz(x11, &done);
  __ Cbz(w5, &try_w);

  Label try_x;
  __ Bind(&try_x);
  __ Ldaxr(x0, MemOperand(x10));
  __ Stlxr(w5, x0, MemOperand(x10));
  __ Sub(x11, x11, 1);
  __ Cbz(x11, &done);
  __ Cbz(w5, &try_x);

  Label try_wp;
  __ Bind(&try_wp);
  __ Ldaxp(w0, w1, MemOperand(x10));
  __ Stlxp(w5, w0, w1, MemOperand(x10));
  __ Sub(x11, x11, 1);
  __ Cbz(x11, &done);
  __ Cbz(w5, &try_wp);

  Label try_xp;
  __ Bind(&try_xp);
  __ Ldaxp(x0, x1, MemOperand(x10));
  __ Stlxp(w5, x0, x1, MemOperand(x10));
  __ Sub(x11, x11, 1);
  __ Cbz(x11, &done);
  __ Cbz(w5, &try_xp);

  __ Bind(&done);
  // Trigger an error if x11 (watchdog) is zero.
  __ Cmp(x11, 0);
  __ Cset(x12, eq);

  END();
  RUN();

  // Check that the watchdog counter didn't run out.
  ASSERT_EQUAL_64(0, x12);

  TEARDOWN();
}
#endif


TEST(load_store_tagged_immediate_offset) {
  uint64_t tags[] = { 0x00, 0x1, 0x55, 0xff };
  int tag_count = sizeof(tags) / sizeof(tags[0]);

  const int kMaxDataLength = 160;

  for (int i = 0; i < tag_count; i++) {
    unsigned char src[kMaxDataLength];
    uint64_t src_raw = reinterpret_cast<uint64_t>(src);
    uint64_t src_tag = tags[i];
    uint64_t src_tagged = CPU::SetPointerTag(src_raw, src_tag);

    for (int k = 0; k < kMaxDataLength; k++) {
      src[k] = k + 1;
    }

    for (int j = 0; j < tag_count; j++) {
      unsigned char dst[kMaxDataLength];
      uint64_t dst_raw = reinterpret_cast<uint64_t>(dst);
      uint64_t dst_tag = tags[j];
      uint64_t dst_tagged = CPU::SetPointerTag(dst_raw, dst_tag);

      memset(dst, 0, kMaxDataLength);

      SETUP();
      ALLOW_ASM();
      START();

      __ Mov(x0, src_tagged);
      __ Mov(x1, dst_tagged);

      int offset = 0;

      // Scaled-immediate offsets.
      __ ldp(q0, q1, MemOperand(x0, offset));
      __ stp(q0, q1, MemOperand(x1, offset));
      offset += 2 * kQRegSizeInBytes;

      __ ldp(x2, x3, MemOperand(x0, offset));
      __ stp(x2, x3, MemOperand(x1, offset));
      offset += 2 * kXRegSizeInBytes;

      __ ldpsw(x2, x3, MemOperand(x0, offset));
      __ stp(w2, w3, MemOperand(x1, offset));
      offset += 2 * kWRegSizeInBytes;

      __ ldp(d0, d1, MemOperand(x0, offset));
      __ stp(d0, d1, MemOperand(x1, offset));
      offset += 2 * kDRegSizeInBytes;

      __ ldp(w2, w3, MemOperand(x0, offset));
      __ stp(w2, w3, MemOperand(x1, offset));
      offset += 2 * kWRegSizeInBytes;

      __ ldp(s0, s1, MemOperand(x0, offset));
      __ stp(s0, s1, MemOperand(x1, offset));
      offset += 2 * kSRegSizeInBytes;

      __ ldr(x2, MemOperand(x0, offset), RequireScaledOffset);
      __ str(x2, MemOperand(x1, offset), RequireScaledOffset);
      offset += kXRegSizeInBytes;

      __ ldr(d0, MemOperand(x0, offset), RequireScaledOffset);
      __ str(d0, MemOperand(x1, offset), RequireScaledOffset);
      offset += kDRegSizeInBytes;

      __ ldr(w2, MemOperand(x0, offset), RequireScaledOffset);
      __ str(w2, MemOperand(x1, offset), RequireScaledOffset);
      offset += kWRegSizeInBytes;

      __ ldr(s0, MemOperand(x0, offset), RequireScaledOffset);
      __ str(s0, MemOperand(x1, offset), RequireScaledOffset);
      offset += kSRegSizeInBytes;

      __ ldrh(w2, MemOperand(x0, offset), RequireScaledOffset);
      __ strh(w2, MemOperand(x1, offset), RequireScaledOffset);
      offset += 2;

      __ ldrsh(w2, MemOperand(x0, offset), RequireScaledOffset);
      __ strh(w2, MemOperand(x1, offset), RequireScaledOffset);
      offset += 2;

      __ ldrb(w2, MemOperand(x0, offset), RequireScaledOffset);
      __ strb(w2, MemOperand(x1, offset), RequireScaledOffset);
      offset += 1;

      __ ldrsb(w2, MemOperand(x0, offset), RequireScaledOffset);
      __ strb(w2, MemOperand(x1, offset), RequireScaledOffset);
      offset += 1;

      // Unscaled-immediate offsets.

      __ ldur(x2, MemOperand(x0, offset), RequireUnscaledOffset);
      __ stur(x2, MemOperand(x1, offset), RequireUnscaledOffset);
      offset += kXRegSizeInBytes;

      __ ldur(d0, MemOperand(x0, offset), RequireUnscaledOffset);
      __ stur(d0, MemOperand(x1, offset), RequireUnscaledOffset);
      offset += kDRegSizeInBytes;

      __ ldur(w2, MemOperand(x0, offset), RequireUnscaledOffset);
      __ stur(w2, MemOperand(x1, offset), RequireUnscaledOffset);
      offset += kWRegSizeInBytes;

      __ ldur(s0, MemOperand(x0, offset), RequireUnscaledOffset);
      __ stur(s0, MemOperand(x1, offset), RequireUnscaledOffset);
      offset += kSRegSizeInBytes;

      __ ldurh(w2, MemOperand(x0, offset), RequireUnscaledOffset);
      __ sturh(w2, MemOperand(x1, offset), RequireUnscaledOffset);
      offset += 2;

      __ ldursh(w2, MemOperand(x0, offset), RequireUnscaledOffset);
      __ sturh(w2, MemOperand(x1, offset), RequireUnscaledOffset);
      offset += 2;

      __ ldurb(w2, MemOperand(x0, offset), RequireUnscaledOffset);
      __ sturb(w2, MemOperand(x1, offset), RequireUnscaledOffset);
      offset += 1;

      __ ldursb(w2, MemOperand(x0, offset), RequireUnscaledOffset);
      __ sturb(w2, MemOperand(x1, offset), RequireUnscaledOffset);
      offset += 1;

      // Extract the tag (so we can test that it was preserved correctly).
      __ Ubfx(x0, x0, kAddressTagOffset, kAddressTagWidth);
      __ Ubfx(x1, x1, kAddressTagOffset, kAddressTagWidth);

      VIXL_ASSERT(kMaxDataLength >= offset);

      END();
      RUN();

      ASSERT_EQUAL_64(src_tag, x0);
      ASSERT_EQUAL_64(dst_tag, x1);

      for (int k = 0; k < offset; k++) {
        VIXL_CHECK(src[k] == dst[k]);
      }

      TEARDOWN();
    }
  }
}


TEST(load_store_tagged_immediate_preindex) {
  uint64_t tags[] = { 0x00, 0x1, 0x55, 0xff };
  int tag_count = sizeof(tags) / sizeof(tags[0]);

  const int kMaxDataLength = 128;

  for (int i = 0; i < tag_count; i++) {
    unsigned char src[kMaxDataLength];
    uint64_t src_raw = reinterpret_cast<uint64_t>(src);
    uint64_t src_tag = tags[i];
    uint64_t src_tagged = CPU::SetPointerTag(src_raw, src_tag);

    for (int k = 0; k < kMaxDataLength; k++) {
      src[k] = k + 1;
    }

    for (int j = 0; j < tag_count; j++) {
      unsigned char dst[kMaxDataLength];
      uint64_t dst_raw = reinterpret_cast<uint64_t>(dst);
      uint64_t dst_tag = tags[j];
      uint64_t dst_tagged = CPU::SetPointerTag(dst_raw, dst_tag);

      for (int k = 0; k < kMaxDataLength; k++) {
        dst[k] = 0;
      }

      SETUP();
      ALLOW_ASM();
      START();

      // Each MemOperand must apply a pre-index equal to the size of the
      // previous access.

      // Start with a non-zero preindex.
      int preindex = 62 * kXRegSizeInBytes;
      int data_length = 0;

      __ Mov(x0, src_tagged - preindex);
      __ Mov(x1, dst_tagged - preindex);

      __ ldp(q0, q1, MemOperand(x0, preindex, PreIndex));
      __ stp(q0, q1, MemOperand(x1, preindex, PreIndex));
      preindex = 2 * kQRegSizeInBytes;
      data_length = preindex;

      __ ldp(x2, x3, MemOperand(x0, preindex, PreIndex));
      __ stp(x2, x3, MemOperand(x1, preindex, PreIndex));
      preindex = 2 * kXRegSizeInBytes;
      data_length += preindex;

      __ ldpsw(x2, x3, MemOperand(x0, preindex, PreIndex));
      __ stp(w2, w3, MemOperand(x1, preindex, PreIndex));
      preindex = 2 * kWRegSizeInBytes;
      data_length += preindex;

      __ ldp(d0, d1, MemOperand(x0, preindex, PreIndex));
      __ stp(d0, d1, MemOperand(x1, preindex, PreIndex));
      preindex = 2 * kDRegSizeInBytes;
      data_length += preindex;

      __ ldp(w2, w3, MemOperand(x0, preindex, PreIndex));
      __ stp(w2, w3, MemOperand(x1, preindex, PreIndex));
      preindex = 2 * kWRegSizeInBytes;
      data_length += preindex;

      __ ldp(s0, s1, MemOperand(x0, preindex, PreIndex));
      __ stp(s0, s1, MemOperand(x1, preindex, PreIndex));
      preindex = 2 * kSRegSizeInBytes;
      data_length += preindex;

      __ ldr(x2, MemOperand(x0, preindex, PreIndex));
      __ str(x2, MemOperand(x1, preindex, PreIndex));
      preindex = kXRegSizeInBytes;
      data_length += preindex;

      __ ldr(d0, MemOperand(x0, preindex, PreIndex));
      __ str(d0, MemOperand(x1, preindex, PreIndex));
      preindex = kDRegSizeInBytes;
      data_length += preindex;

      __ ldr(w2, MemOperand(x0, preindex, PreIndex));
      __ str(w2, MemOperand(x1, preindex, PreIndex));
      preindex = kWRegSizeInBytes;
      data_length += preindex;

      __ ldr(s0, MemOperand(x0, preindex, PreIndex));
      __ str(s0, MemOperand(x1, preindex, PreIndex));
      preindex = kSRegSizeInBytes;
      data_length += preindex;

      __ ldrh(w2, MemOperand(x0, preindex, PreIndex));
      __ strh(w2, MemOperand(x1, preindex, PreIndex));
      preindex = 2;
      data_length += preindex;

      __ ldrsh(w2, MemOperand(x0, preindex, PreIndex));
      __ strh(w2, MemOperand(x1, preindex, PreIndex));
      preindex = 2;
      data_length += preindex;

      __ ldrb(w2, MemOperand(x0, preindex, PreIndex));
      __ strb(w2, MemOperand(x1, preindex, PreIndex));
      preindex = 1;
      data_length += preindex;

      __ ldrsb(w2, MemOperand(x0, preindex, PreIndex));
      __ strb(w2, MemOperand(x1, preindex, PreIndex));
      preindex = 1;
      data_length += preindex;

      VIXL_ASSERT(kMaxDataLength >= data_length);

      END();
      RUN();

      // Check that the preindex was correctly applied in each operation, and
      // that the tag was preserved.
      ASSERT_EQUAL_64(src_tagged + data_length - preindex, x0);
      ASSERT_EQUAL_64(dst_tagged + data_length - preindex, x1);

      for (int k = 0; k < data_length; k++) {
        VIXL_CHECK(src[k] == dst[k]);
      }

      TEARDOWN();
    }
  }
}


TEST(load_store_tagged_immediate_postindex) {
  uint64_t tags[] = { 0x00, 0x1, 0x55, 0xff };
  int tag_count = sizeof(tags) / sizeof(tags[0]);

  const int kMaxDataLength = 128;

  for (int i = 0; i < tag_count; i++) {
    unsigned char src[kMaxDataLength];
    uint64_t src_raw = reinterpret_cast<uint64_t>(src);
    uint64_t src_tag = tags[i];
    uint64_t src_tagged = CPU::SetPointerTag(src_raw, src_tag);

    for (int k = 0; k < kMaxDataLength; k++) {
      src[k] = k + 1;
    }

    for (int j = 0; j < tag_count; j++) {
      unsigned char dst[kMaxDataLength];
      uint64_t dst_raw = reinterpret_cast<uint64_t>(dst);
      uint64_t dst_tag = tags[j];
      uint64_t dst_tagged = CPU::SetPointerTag(dst_raw, dst_tag);

      for (int k = 0; k < kMaxDataLength; k++) {
        dst[k] = 0;
      }

      SETUP();
      ALLOW_ASM();
      START();

      int postindex = 2 * kXRegSizeInBytes;
      int data_length = 0;

      __ Mov(x0, src_tagged);
      __ Mov(x1, dst_tagged);

      __ ldp(x2, x3, MemOperand(x0, postindex, PostIndex));
      __ stp(x2, x3, MemOperand(x1, postindex, PostIndex));
      data_length = postindex;

      postindex = 2 * kQRegSizeInBytes;
      __ ldp(q0, q1, MemOperand(x0, postindex, PostIndex));
      __ stp(q0, q1, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      postindex = 2 * kWRegSizeInBytes;
      __ ldpsw(x2, x3, MemOperand(x0, postindex, PostIndex));
      __ stp(w2, w3, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      postindex = 2 * kDRegSizeInBytes;
      __ ldp(d0, d1, MemOperand(x0, postindex, PostIndex));
      __ stp(d0, d1, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      postindex = 2 * kWRegSizeInBytes;
      __ ldp(w2, w3, MemOperand(x0, postindex, PostIndex));
      __ stp(w2, w3, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      postindex = 2 * kSRegSizeInBytes;
      __ ldp(s0, s1, MemOperand(x0, postindex, PostIndex));
      __ stp(s0, s1, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      postindex = kXRegSizeInBytes;
      __ ldr(x2, MemOperand(x0, postindex, PostIndex));
      __ str(x2, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      postindex = kDRegSizeInBytes;
      __ ldr(d0, MemOperand(x0, postindex, PostIndex));
      __ str(d0, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      postindex = kWRegSizeInBytes;
      __ ldr(w2, MemOperand(x0, postindex, PostIndex));
      __ str(w2, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      postindex = kSRegSizeInBytes;
      __ ldr(s0, MemOperand(x0, postindex, PostIndex));
      __ str(s0, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      postindex = 2;
      __ ldrh(w2, MemOperand(x0, postindex, PostIndex));
      __ strh(w2, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      postindex = 2;
      __ ldrsh(w2, MemOperand(x0, postindex, PostIndex));
      __ strh(w2, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      postindex = 1;
      __ ldrb(w2, MemOperand(x0, postindex, PostIndex));
      __ strb(w2, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      postindex = 1;
      __ ldrsb(w2, MemOperand(x0, postindex, PostIndex));
      __ strb(w2, MemOperand(x1, postindex, PostIndex));
      data_length += postindex;

      VIXL_ASSERT(kMaxDataLength >= data_length);

      END();
      RUN();

      // Check that the postindex was correctly applied in each operation, and
      // that the tag was preserved.
      ASSERT_EQUAL_64(src_tagged + data_length, x0);
      ASSERT_EQUAL_64(dst_tagged + data_length, x1);

      for (int k = 0; k < data_length; k++) {
        VIXL_CHECK(src[k] == dst[k]);
      }

      TEARDOWN();
    }
  }
}


TEST(load_store_tagged_register_offset) {
  uint64_t tags[] = { 0x00, 0x1, 0x55, 0xff };
  int tag_count = sizeof(tags) / sizeof(tags[0]);

  const int kMaxDataLength = 128;

  for (int i = 0; i < tag_count; i++) {
    unsigned char src[kMaxDataLength];
    uint64_t src_raw = reinterpret_cast<uint64_t>(src);
    uint64_t src_tag = tags[i];
    uint64_t src_tagged = CPU::SetPointerTag(src_raw, src_tag);

    for (int k = 0; k < kMaxDataLength; k++) {
      src[k] = k + 1;
    }

    for (int j = 0; j < tag_count; j++) {
      unsigned char dst[kMaxDataLength];
      uint64_t dst_raw = reinterpret_cast<uint64_t>(dst);
      uint64_t dst_tag = tags[j];
      uint64_t dst_tagged = CPU::SetPointerTag(dst_raw, dst_tag);

      // Also tag the offset register; the operation should still succeed.
      for (int o = 0; o < tag_count; o++) {
        uint64_t offset_base = CPU::SetPointerTag(UINT64_C(0), tags[o]);
        int data_length = 0;

        for (int k = 0; k < kMaxDataLength; k++) {
          dst[k] = 0;
        }

        SETUP();
        ALLOW_ASM();
        START();

        __ Mov(x0, src_tagged);
        __ Mov(x1, dst_tagged);

        __ Mov(x10, offset_base + data_length);
        __ ldr(x2, MemOperand(x0, x10));
        __ str(x2, MemOperand(x1, x10));
        data_length += kXRegSizeInBytes;

        __ Mov(x10, offset_base + data_length);
        __ ldr(d0, MemOperand(x0, x10));
        __ str(d0, MemOperand(x1, x10));
        data_length += kDRegSizeInBytes;

        __ Mov(x10, offset_base + data_length);
        __ ldr(w2, MemOperand(x0, x10));
        __ str(w2, MemOperand(x1, x10));
        data_length += kWRegSizeInBytes;

        __ Mov(x10, offset_base + data_length);
        __ ldr(s0, MemOperand(x0, x10));
        __ str(s0, MemOperand(x1, x10));
        data_length += kSRegSizeInBytes;

        __ Mov(x10, offset_base + data_length);
        __ ldrh(w2, MemOperand(x0, x10));
        __ strh(w2, MemOperand(x1, x10));
        data_length += 2;

        __ Mov(x10, offset_base + data_length);
        __ ldrsh(w2, MemOperand(x0, x10));
        __ strh(w2, MemOperand(x1, x10));
        data_length += 2;

        __ Mov(x10, offset_base + data_length);
        __ ldrb(w2, MemOperand(x0, x10));
        __ strb(w2, MemOperand(x1, x10));
        data_length += 1;

        __ Mov(x10, offset_base + data_length);
        __ ldrsb(w2, MemOperand(x0, x10));
        __ strb(w2, MemOperand(x1, x10));
        data_length += 1;

        VIXL_ASSERT(kMaxDataLength >= data_length);

        END();
        RUN();

        // Check that the postindex was correctly applied in each operation, and
        // that the tag was preserved.
        ASSERT_EQUAL_64(src_tagged, x0);
        ASSERT_EQUAL_64(dst_tagged, x1);
        ASSERT_EQUAL_64(offset_base + data_length - 1, x10);

        for (int k = 0; k < data_length; k++) {
          VIXL_CHECK(src[k] == dst[k]);
        }

        TEARDOWN();
      }
    }
  }
}


TEST(load_store_tagged_register_postindex) {
  uint64_t src[] = { 0x0706050403020100, 0x0f0e0d0c0b0a0908 };
  uint64_t tags[] = { 0x00, 0x1, 0x55, 0xff };
  int tag_count = sizeof(tags) / sizeof(tags[0]);

  for (int j = 0; j < tag_count; j++) {
    for (int i = 0; i < tag_count; i++) {
      SETUP();
      uint64_t src_base = reinterpret_cast<uint64_t>(src);
      uint64_t src_tagged = CPU::SetPointerTag(src_base, tags[i]);
      uint64_t offset_tagged = CPU::SetPointerTag(UINT64_C(0), tags[j]);

      START();
      __ Mov(x10, src_tagged);
      __ Mov(x11, offset_tagged);
      __ Ld1(v0.V16B(), MemOperand(x10, x11, PostIndex));
      // TODO: add other instructions (ld2-4, st1-4) as they become available.
      END();

      RUN();

      ASSERT_EQUAL_128(0x0f0e0d0c0b0a0908, 0x0706050403020100, q0);
      ASSERT_EQUAL_64(src_tagged + offset_tagged, x10);

      TEARDOWN();
    }
  }
}


TEST(branch_tagged) {
  SETUP();
  START();

  Label loop, loop_entry, done;
  __ Adr(x0, &loop);
  __ Mov(x1, 0);
  __ B(&loop_entry);

  __ Bind(&loop);
  __ Add(x1, x1, 1);  // Count successful jumps.

  // Advance to the next tag, then bail out if we've come back around to tag 0.
  __ Add(x0, x0, UINT64_C(1) << kAddressTagOffset);
  __ Tst(x0, kAddressTagMask);
  __ B(eq, &done);

  __ Bind(&loop_entry);
  __ Br(x0);

  __ Bind(&done);

  END();
  RUN();

  ASSERT_EQUAL_64(1 << kAddressTagWidth, x1);

  TEARDOWN();
}


TEST(branch_and_link_tagged) {
  SETUP();
  START();

  Label loop, loop_entry, done;
  __ Adr(x0, &loop);
  __ Mov(x1, 0);
  __ B(&loop_entry);

  __ Bind(&loop);

  // Bail out (before counting a successful jump) if lr appears to be tagged.
  __ Tst(lr, kAddressTagMask);
  __ B(ne, &done);

  __ Add(x1, x1, 1);  // Count successful jumps.

  // Advance to the next tag, then bail out if we've come back around to tag 0.
  __ Add(x0, x0, UINT64_C(1) << kAddressTagOffset);
  __ Tst(x0, kAddressTagMask);
  __ B(eq, &done);

  __ Bind(&loop_entry);
  __ Blr(x0);

  __ Bind(&done);

  END();
  RUN();

  ASSERT_EQUAL_64(1 << kAddressTagWidth, x1);

  TEARDOWN();
}


TEST(branch_tagged_and_adr_adrp) {
  SETUP_CUSTOM(BUF_SIZE, PageOffsetDependentCode);
  START();

  Label loop, loop_entry, done;
  __ Adr(x0, &loop);
  __ Mov(x1, 0);
  __ B(&loop_entry);

  __ Bind(&loop);

  // Bail out (before counting a successful jump) if `adr x10, ...` is tagged.
  __ Adr(x10, &done);
  __ Tst(x10, kAddressTagMask);
  __ B(ne, &done);

  // Bail out (before counting a successful jump) if `adrp x11, ...` is tagged.
  __ Adrp(x11, &done);
  __ Tst(x11, kAddressTagMask);
  __ B(ne, &done);

  __ Add(x1, x1, 1);  // Count successful iterations.

  // Advance to the next tag, then bail out if we've come back around to tag 0.
  __ Add(x0, x0, UINT64_C(1) << kAddressTagOffset);
  __ Tst(x0, kAddressTagMask);
  __ B(eq, &done);

  __ Bind(&loop_entry);
  __ Br(x0);

  __ Bind(&done);

  END();
  RUN();

  ASSERT_EQUAL_64(1 << kAddressTagWidth, x1);

  TEARDOWN_CUSTOM();
}

TEST(neon_3same_addp) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0xff00aa5500ff55aa, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(),  0x000055aaff55ff00, 0xaa55ff55555500ff);
  __ Addp(v16.V16B(), v0.V16B(), v1.V16B());

  END();

  RUN();
  ASSERT_EQUAL_128(0x00ff54ffff54aaff, 0xffffffffffffffff, q16);
  TEARDOWN();
}

TEST(neon_3same_sqdmulh_sqrdmulh) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0000000000000000, 0x0000040004008000);
  __ Movi(v1.V2D(), 0x0000000000000000, 0x0000002000108000);
  __ Movi(v2.V2D(), 0x0400000080000000, 0x0400000080000000);
  __ Movi(v3.V2D(), 0x0000002080000000, 0x0000001080000000);

  __ Sqdmulh(v16.V4H(), v0.V4H(), v1.V4H());
  __ Sqdmulh(v17.V4S(), v2.V4S(), v3.V4S());
  __ Sqdmulh(h18, h0, h1);
  __ Sqdmulh(s19, s2, s3);

  __ Sqrdmulh(v20.V4H(), v0.V4H(), v1.V4H());
  __ Sqrdmulh(v21.V4S(), v2.V4S(), v3.V4S());
  __ Sqrdmulh(h22, h0, h1);
  __ Sqrdmulh(s23, s2, s3);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000100007fff, q16);
  ASSERT_EQUAL_128(0x000000017fffffff, 0x000000007fffffff, q17);
  ASSERT_EQUAL_128(0, 0x7fff, q18);
  ASSERT_EQUAL_128(0, 0x7fffffff, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000100017fff, q20);
  ASSERT_EQUAL_128(0x000000017fffffff, 0x000000017fffffff, q21);
  ASSERT_EQUAL_128(0, 0x7fff, q22);
  ASSERT_EQUAL_128(0, 0x7fffffff, q23);
  TEARDOWN();
}

TEST(neon_byelement_sqdmulh_sqrdmulh) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0000000000000000, 0x0000040004008000);
  __ Movi(v1.V2D(), 0x0000000000000000, 0x0000002000108000);
  __ Movi(v2.V2D(), 0x0400000080000000, 0x0400000080000000);
  __ Movi(v3.V2D(), 0x0000002080000000, 0x0000001080000000);

  __ Sqdmulh(v16.V4H(), v0.V4H(), v1.H(), 1);
  __ Sqdmulh(v17.V4S(), v2.V4S(), v3.S(), 1);
  __ Sqdmulh(h18, h0, v1.H(), 0);
  __ Sqdmulh(s19, s2, v3.S(), 0);

  __ Sqrdmulh(v20.V4H(), v0.V4H(), v1.H(), 1);
  __ Sqrdmulh(v21.V4S(), v2.V4S(), v3.S(), 1);
  __ Sqrdmulh(h22, h0, v1.H(), 0);
  __ Sqrdmulh(s23, s2, v3.S(), 0);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000000000fff0, q16);
  ASSERT_EQUAL_128(0x00000000fffffff0, 0x00000000fffffff0, q17);
  ASSERT_EQUAL_128(0, 0x7fff, q18);
  ASSERT_EQUAL_128(0, 0x7fffffff, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000010001fff0, q20);
  ASSERT_EQUAL_128(0x00000001fffffff0, 0x00000001fffffff0, q21);
  ASSERT_EQUAL_128(0, 0x7fff, q22);
  ASSERT_EQUAL_128(0, 0x7fffffff, q23);
  TEARDOWN();
}


TEST(neon_2regmisc_saddlp) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);

  __ Saddlp(v16.V8H(), v0.V16B());
  __ Saddlp(v17.V4H(), v0.V8B());

  __ Saddlp(v18.V4S(), v0.V8H());
  __ Saddlp(v19.V2S(), v0.V4H());

  __ Saddlp(v20.V2D(), v0.V4S());
  __ Saddlp(v21.V1D(), v0.V2S());

  END();

  RUN();
  ASSERT_EQUAL_128(0x0080ffffff010080, 0xff01ffff0080ff01, q16);
  ASSERT_EQUAL_128(0x0000000000000000, 0xff01ffff0080ff01, q17);
  ASSERT_EQUAL_128(0x0000800000000081, 0xffff7f81ffff8200, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffff7f81ffff8200, q19);
  ASSERT_EQUAL_128(0x0000000000818000, 0xffffffff82017f81, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffff82017f81, q21);
  TEARDOWN();
}

TEST(neon_2regmisc_uaddlp) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);

  __ Uaddlp(v16.V8H(), v0.V16B());
  __ Uaddlp(v17.V4H(), v0.V8B());

  __ Uaddlp(v18.V4S(), v0.V8H());
  __ Uaddlp(v19.V2S(), v0.V4H());

  __ Uaddlp(v20.V2D(), v0.V4S());
  __ Uaddlp(v21.V1D(), v0.V2S());

  END();

  RUN();
  ASSERT_EQUAL_128(0x008000ff01010080, 0x010100ff00800101, q16);
  ASSERT_EQUAL_128(0x0000000000000000, 0x010100ff00800101, q17);
  ASSERT_EQUAL_128(0x0000800000010081, 0x00017f8100008200, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00017f8100008200, q19);
  ASSERT_EQUAL_128(0x0000000100818000, 0x0000000082017f81, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000082017f81, q21);
  TEARDOWN();
}

TEST(neon_2regmisc_sadalp) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Mov(v16.V16B(), v1.V16B());
  __ Mov(v17.V16B(), v1.V16B());
  __ Sadalp(v16.V8H(), v0.V16B());
  __ Sadalp(v17.V4H(), v0.V8B());

  __ Mov(v18.V16B(), v2.V16B());
  __ Mov(v19.V16B(), v2.V16B());
  __ Sadalp(v18.V4S(), v1.V8H());
  __ Sadalp(v19.V2S(), v1.V4H());

  __ Mov(v20.V16B(), v3.V16B());
  __ Mov(v21.V16B(), v4.V16B());
  __ Sadalp(v20.V2D(), v2.V4S());
  __ Sadalp(v21.V1D(), v2.V2S());

  END();

  RUN();
  ASSERT_EQUAL_128(0x80808000ff000080, 0xff00ffff00817f00, q16);
  ASSERT_EQUAL_128(0x0000000000000000, 0xff00ffff00817f00, q17);
  ASSERT_EQUAL_128(0x7fff0001fffffffe, 0xffffffff80007fff, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffff80007fff, q19);
  ASSERT_EQUAL_128(0x7fffffff80000000, 0x800000007ffffffe, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000007fffffff, q21);
  TEARDOWN();
}

TEST(neon_2regmisc_uadalp) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Mov(v16.V16B(), v1.V16B());
  __ Mov(v17.V16B(), v1.V16B());
  __ Uadalp(v16.V8H(), v0.V16B());
  __ Uadalp(v17.V4H(), v0.V8B());

  __ Mov(v18.V16B(), v2.V16B());
  __ Mov(v19.V16B(), v2.V16B());
  __ Uadalp(v18.V4S(), v1.V8H());
  __ Uadalp(v19.V2S(), v1.V4H());

  __ Mov(v20.V16B(), v3.V16B());
  __ Mov(v21.V16B(), v4.V16B());
  __ Uadalp(v20.V2D(), v2.V4S());
  __ Uadalp(v21.V1D(), v2.V2S());

  END();

  RUN();
  ASSERT_EQUAL_128(0x8080810001000080, 0x010000ff00818100, q16);
  ASSERT_EQUAL_128(0x0000000000000000, 0x010000ff00818100, q17);
  ASSERT_EQUAL_128(0x800100010000fffe, 0x0000ffff80007fff, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000ffff80007fff, q19);
  ASSERT_EQUAL_128(0x8000000180000000, 0x800000007ffffffe, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000007fffffff, q21);
  TEARDOWN();
}

TEST(neon_3same_mul) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0xff00aa5500ff55aa, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(),  0x000055aaff55ff00, 0xaa55ff55555500ff);
  __ Movi(v16.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);
  __ Movi(v17.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);

  __ Mla(v16.V16B(), v0.V16B(), v1.V16B());
  __ Mls(v17.V16B(), v0.V16B(), v1.V16B());
  __ Mul(v18.V16B(), v0.V16B(), v1.V16B());

  END();

  RUN();
  ASSERT_EQUAL_128(0x0102757605b1b208, 0x5f0a61450db90f56, q16);
  ASSERT_EQUAL_128(0x01029192055b5c08, 0xb30ab5d30d630faa, q17);
  ASSERT_EQUAL_128(0x0000727200abab00, 0x5600563900ab0056, q18);
  TEARDOWN();
}



TEST(neon_3same_absdiff) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0xff00aa5500ff55aa, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(),  0x000055aaff55ff00, 0xaa55ff55555500ff);
  __ Movi(v16.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);
  __ Movi(v17.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);

  __ Saba(v16.V16B(), v0.V16B(), v1.V16B());
  __ Uaba(v17.V16B(), v0.V16B(), v1.V16B());
  __ Sabd(v18.V16B(), v0.V16B(), v1.V16B());
  __ Uabd(v19.V16B(), v0.V16B(), v1.V16B());

  END();

  RUN();
  ASSERT_EQUAL_128(0x0202aeaf065c5d5e, 0x5e5f600c62646455, q16);
  ASSERT_EQUAL_128(0x0002585904b0b1b2, 0x5e5f600c62b86455, q17);
  ASSERT_EQUAL_128(0x0100abab01565656, 0x5555550055565555, q18);
  ASSERT_EQUAL_128(0xff005555ffaaaaaa, 0x5555550055aa5555, q19);
  TEARDOWN();
}


TEST(neon_byelement_mul) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0xff00aa5500ff55aa, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(),  0x000155aaff55ff00, 0xaa55ff55555500ff);


  __ Mul(v16.V4H(), v0.V4H(), v1.H(), 0);
  __ Mul(v17.V8H(), v0.V8H(), v1.H(), 7);
  __ Mul(v18.V2S(), v0.V2S(), v1.S(), 0);
  __ Mul(v19.V4S(), v0.V4S(), v1.S(), 3);

  __ Movi(v20.V2D(), 0x0000000000000000, 0x0001000200030004);
  __ Movi(v21.V2D(), 0x0005000600070008, 0x0001000200030004);
  __ Mla(v20.V4H(), v0.V4H(), v1.H(), 0);
  __ Mla(v21.V8H(), v0.V8H(), v1.H(), 7);

  __ Movi(v22.V2D(), 0x0000000000000000, 0x0000000200000004);
  __ Movi(v23.V2D(), 0x0000000600000008, 0x0000000200000004);
  __ Mla(v22.V2S(), v0.V2S(), v1.S(), 0);
  __ Mla(v23.V4S(), v0.V4S(), v1.S(), 3);

  __ Movi(v24.V2D(), 0x0000000000000000, 0x0100aaabfe015456);
  __ Movi(v25.V2D(), 0xff00aa5500ff55aa, 0xff00aa5500ff55aa);
  __ Mls(v24.V4H(), v0.V4H(), v1.H(), 0);
  __ Mls(v25.V8H(), v0.V8H(), v1.H(), 7);

  __ Movi(v26.V2D(), 0x0000000000000000, 0xc8e2aaabe1c85456);
  __ Movi(v27.V2D(), 0x39545572c6aa54e4, 0x39545572c6aa54e4);
  __ Mls(v26.V2S(), v0.V2S(), v1.S(), 0);
  __ Mls(v27.V4S(), v0.V4S(), v1.S(), 3);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x0100aaabfe015456, q16);
  ASSERT_EQUAL_128(0xff00aa5500ff55aa, 0xff00aa5500ff55aa, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0xc8e2aaabe1c85456, q18);
  ASSERT_EQUAL_128(0x39545572c6aa54e4, 0x39545572c6aa54e4, q19);

  ASSERT_EQUAL_128(0x0000000000000000, 0x0101aaadfe04545a, q20);
  ASSERT_EQUAL_128(0xff05aa5b010655b2, 0xff01aa57010255ae, q21);
  ASSERT_EQUAL_128(0x0000000000000000, 0xc8e2aaade1c8545a, q22);
  ASSERT_EQUAL_128(0x39545578c6aa54ec, 0x39545574c6aa54e8, q23);

  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q24);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q25);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q26);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q27);
  TEARDOWN();
}


TEST(neon_byelement_mull) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0xaa55ff55555500ff, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(),  0x000155aaff55ff00, 0xaa55ff55555500ff);


  __ Smull(v16.V4S(),  v0.V4H(), v1.H(), 7);
  __ Smull2(v17.V4S(), v0.V8H(), v1.H(), 0);
  __ Umull(v18.V4S(),  v0.V4H(), v1.H(), 7);
  __ Umull2(v19.V4S(), v0.V8H(), v1.H(), 0);

  __ Movi(v20.V2D(), 0x0000000100000002, 0x0000000200000001);
  __ Movi(v21.V2D(), 0x0000000100000002, 0x0000000200000001);
  __ Movi(v22.V2D(), 0x0000000100000002, 0x0000000200000001);
  __ Movi(v23.V2D(), 0x0000000100000002, 0x0000000200000001);

  __ Smlal(v20.V4S(),  v0.V4H(), v1.H(), 7);
  __ Smlal2(v21.V4S(), v0.V8H(), v1.H(), 0);
  __ Umlal(v22.V4S(),  v0.V4H(), v1.H(), 7);
  __ Umlal2(v23.V4S(), v0.V8H(), v1.H(), 0);

  __ Movi(v24.V2D(), 0xffffff00ffffaa55, 0x000000ff000055aa);
  __ Movi(v25.V2D(), 0xffaaaaabffff55ab, 0x0054ffab0000fe01);
  __ Movi(v26.V2D(), 0x0000ff000000aa55, 0x000000ff000055aa);
  __ Movi(v27.V2D(), 0x00a9aaab00fe55ab, 0x0054ffab0000fe01);

  __ Smlsl(v24.V4S(),  v0.V4H(), v1.H(), 7);
  __ Smlsl2(v25.V4S(), v0.V8H(), v1.H(), 0);
  __ Umlsl(v26.V4S(),  v0.V4H(), v1.H(), 7);
  __ Umlsl2(v27.V4S(), v0.V8H(), v1.H(), 0);

  END();

  RUN();

  ASSERT_EQUAL_128(0xffffff00ffffaa55, 0x000000ff000055aa, q16);
  ASSERT_EQUAL_128(0xffaaaaabffff55ab, 0x0054ffab0000fe01, q17);
  ASSERT_EQUAL_128(0x0000ff000000aa55, 0x000000ff000055aa, q18);
  ASSERT_EQUAL_128(0x00a9aaab00fe55ab, 0x0054ffab0000fe01, q19);

  ASSERT_EQUAL_128(0xffffff01ffffaa57, 0x00000101000055ab, q20);
  ASSERT_EQUAL_128(0xffaaaaacffff55ad, 0x0054ffad0000fe02, q21);
  ASSERT_EQUAL_128(0x0000ff010000aa57, 0x00000101000055ab, q22);
  ASSERT_EQUAL_128(0x00a9aaac00fe55ad, 0x0054ffad0000fe02, q23);

  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q24);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q25);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q26);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q27);

  TEARDOWN();
}


TEST(neon_byelement_sqdmull) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaa55ff55555500ff, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(), 0x000155aaff55ff00, 0xaa55ff55555500ff);

  __ Sqdmull(v16.V4S(), v0.V4H(), v1.H(), 7);
  __ Sqdmull2(v17.V4S(), v0.V8H(), v1.H(), 0);
  __ Sqdmull(s18, h0, v1.H(), 7);

  __ Movi(v20.V2D(), 0x0000000100000002, 0x0000000200000001);
  __ Movi(v21.V2D(), 0x0000000100000002, 0x0000000200000001);
  __ Movi(v22.V2D(), 0x0000000100000002, 0x0000000200000001);

  __ Sqdmlal(v20.V4S(), v0.V4H(), v1.H(), 7);
  __ Sqdmlal2(v21.V4S(), v0.V8H(), v1.H(), 0);
  __ Sqdmlal(s22, h0, v1.H(), 7);

  __ Movi(v24.V2D(), 0xfffffe00ffff54aa, 0x000001fe0000ab54);
  __ Movi(v25.V2D(), 0xff555556fffeab56, 0x00a9ff560001fc02);
  __ Movi(v26.V2D(), 0x0000000000000000, 0x000000000000ab54);

  __ Sqdmlsl(v24.V4S(), v0.V4H(), v1.H(), 7);
  __ Sqdmlsl2(v25.V4S(), v0.V8H(), v1.H(), 0);
  __ Sqdmlsl(s26, h0, v1.H(), 7);

  END();

  RUN();

  ASSERT_EQUAL_128(0xfffffe00ffff54aa, 0x000001fe0000ab54, q16);
  ASSERT_EQUAL_128(0xff555556fffeab56, 0x00a9ff560001fc02, q17);
  ASSERT_EQUAL_128(0, 0x0000ab54, q18);

  ASSERT_EQUAL_128(0xfffffe01ffff54ac, 0x000002000000ab55, q20);
  ASSERT_EQUAL_128(0xff555557fffeab58, 0x00a9ff580001fc03, q21);
  ASSERT_EQUAL_128(0, 0x0000ab55, q22);

  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q24);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q25);
  ASSERT_EQUAL_128(0, 0x00000000, q26);

  TEARDOWN();
}


TEST(neon_3diff_absdiff) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0xff00aa5500ff55ab, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(),  0x000055aaff55ff00, 0xaa55ff55555500ff);
  __ Movi(v16.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);
  __ Movi(v17.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);
  __ Movi(v18.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);
  __ Movi(v19.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);

  __ Sabal(v16.V8H(), v0.V8B(), v1.V8B());
  __ Uabal(v17.V8H(), v0.V8B(), v1.V8B());
  __ Sabal2(v18.V8H(), v0.V16B(), v1.V16B());
  __ Uabal2(v19.V8H(), v0.V16B(), v1.V16B());

  END();

  RUN();
  ASSERT_EQUAL_128(0x01570359055b0708, 0x095f0b620d630f55, q16);
  ASSERT_EQUAL_128(0x01570359055b0708, 0x095f0bb60d630f55, q17);
  ASSERT_EQUAL_128(0x0103030405b107b3, 0x090b0b620d640f55, q18);
  ASSERT_EQUAL_128(0x02010304055b075d, 0x0a090bb60db80fab, q19);
  TEARDOWN();
}


TEST(neon_3diff_sqdmull) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0x7fff7fff80008000, 0x80007fff7fff8000);
  __ Movi(v1.V2D(),  0x80007fff7fff8000, 0x7fff7fff80008000);
  __ Movi(v2.V2D(),  0x800000007fffffff, 0x7fffffff80000000);
  __ Movi(v3.V2D(),  0x8000000080000000, 0x8000000080000000);

  __ Sqdmull(v16.V4S(),  v0.V4H(), v1.V4H());
  __ Sqdmull2(v17.V4S(), v0.V8H(), v1.V8H());
  __ Sqdmull(v18.V2D(),  v2.V2S(), v3.V2S());
  __ Sqdmull2(v19.V2D(), v2.V4S(), v3.V4S());
  __ Sqdmull(s20, h0, h1);
  __ Sqdmull(d21, s2, s3);

  END();

  RUN();
  ASSERT_EQUAL_128(0x800100007ffe0002, 0x800100007fffffff, q16);
  ASSERT_EQUAL_128(0x800100007ffe0002, 0x800100007fffffff, q17);
  ASSERT_EQUAL_128(0x8000000100000000, 0x7fffffffffffffff, q18);
  ASSERT_EQUAL_128(0x7fffffffffffffff, 0x8000000100000000, q19);
  ASSERT_EQUAL_128(0, 0x7fffffff, q20);
  ASSERT_EQUAL_128(0, 0x7fffffffffffffff, q21);
  TEARDOWN();
}


TEST(neon_3diff_sqdmlal) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0x7fff7fff80008000, 0x80007fff7fff8000);
  __ Movi(v1.V2D(),  0x80007fff7fff8000, 0x7fff7fff80008000);
  __ Movi(v2.V2D(),  0x800000007fffffff, 0x7fffffff80000000);
  __ Movi(v3.V2D(),  0x8000000080000000, 0x8000000080000000);

  __ Movi(v16.V2D(), 0xffffffff00000001, 0x8fffffff00000001);
  __ Movi(v17.V2D(), 0x00000001ffffffff, 0x00000001ffffffff);
  __ Movi(v18.V2D(), 0x8000000000000001, 0x0000000000000001);
  __ Movi(v19.V2D(), 0xffffffffffffffff, 0x7fffffffffffffff);
  __ Movi(v20.V2D(), 0, 0x00000001);
  __ Movi(v21.V2D(), 0, 0x00000001);

  __ Sqdmlal(v16.V4S(),  v0.V4H(), v1.V4H());
  __ Sqdmlal2(v17.V4S(), v0.V8H(), v1.V8H());
  __ Sqdmlal(v18.V2D(),  v2.V2S(), v3.V2S());
  __ Sqdmlal2(v19.V2D(), v2.V4S(), v3.V4S());
  __ Sqdmlal(s20, h0, h1);
  __ Sqdmlal(d21, s2, s3);

  END();

  RUN();
  ASSERT_EQUAL_128(0x8000ffff7ffe0003, 0x800000007fffffff, q16);
  ASSERT_EQUAL_128(0x800100017ffe0001, 0x800100017ffffffe, q17);
  ASSERT_EQUAL_128(0x8000000000000000, 0x7fffffffffffffff, q18);
  ASSERT_EQUAL_128(0x7ffffffffffffffe, 0x00000000ffffffff, q19);
  ASSERT_EQUAL_128(0, 0x7fffffff, q20);
  ASSERT_EQUAL_128(0, 0x7fffffffffffffff, q21);
  TEARDOWN();
}


TEST(neon_3diff_sqdmlsl) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0x7fff7fff80008000, 0x80007fff7fff8000);
  __ Movi(v1.V2D(),  0x80007fff7fff8000, 0x7fff7fff80008000);
  __ Movi(v2.V2D(),  0x800000007fffffff, 0x7fffffff80000000);
  __ Movi(v3.V2D(),  0x8000000080000000, 0x8000000080000000);

  __ Movi(v16.V2D(), 0xffffffff00000001, 0x7ffffffe80000001);
  __ Movi(v17.V2D(), 0x00000001ffffffff, 0x7ffffffe00000001);
  __ Movi(v18.V2D(), 0x8000000000000001, 0x8000000000000001);
  __ Movi(v19.V2D(), 0xfffffffffffffffe, 0x7fffffffffffffff);
  __ Movi(v20.V2D(), 0, 0x00000001);
  __ Movi(v21.V2D(), 0, 0x00000001);

  __ Sqdmlsl(v16.V4S(),  v0.V4H(), v1.V4H());
  __ Sqdmlsl2(v17.V4S(), v0.V8H(), v1.V8H());
  __ Sqdmlsl(v18.V2D(),  v2.V2S(), v3.V2S());
  __ Sqdmlsl2(v19.V2D(), v2.V4S(), v3.V4S());
  __ Sqdmlsl(s20, h0, h1);
  __ Sqdmlsl(d21, s2, s3);

  END();

  RUN();
  ASSERT_EQUAL_128(0x7ffeffff8001ffff, 0x7fffffff80000000, q16);
  ASSERT_EQUAL_128(0x7fff00018001fffd, 0x7fffffff80000002, q17);
  ASSERT_EQUAL_128(0xffffffff00000001, 0x8000000000000000, q18);
  ASSERT_EQUAL_128(0x8000000000000000, 0x7fffffffffffffff, q19);
  ASSERT_EQUAL_128(0, 0x80000002, q20);
  ASSERT_EQUAL_128(0, 0x8000000000000002, q21);

  TEARDOWN();
}


TEST(neon_3diff_mla) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0xff00aa5500ff55ab, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(),  0x000055aaff55ff00, 0xaa55ff55555500ff);
  __ Movi(v16.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);
  __ Movi(v17.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);
  __ Movi(v18.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);
  __ Movi(v19.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);

  __ Smlal(v16.V8H(), v0.V8B(), v1.V8B());
  __ Umlal(v17.V8H(), v0.V8B(), v1.V8B());
  __ Smlal2(v18.V8H(), v0.V16B(), v1.V16B());
  __ Umlal2(v19.V8H(), v0.V16B(), v1.V16B());

  END();

  RUN();
  ASSERT_EQUAL_128(0x01580304055c2341, 0x090a0ab70d0e0f56, q16);
  ASSERT_EQUAL_128(0xaa580304ae5c2341, 0x090a5fb70d0eb856, q17);
  ASSERT_EQUAL_128(0x01020304e878ea7a, 0x090a0ab70cb90f00, q18);
  ASSERT_EQUAL_128(0x010203043d783f7a, 0x090a5fb761b90f00, q19);
  TEARDOWN();
}


TEST(neon_3diff_mls) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0xff00aa5500ff55ab, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(),  0x000055aaff55ff00, 0xaa55ff55555500ff);
  __ Movi(v16.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);
  __ Movi(v17.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);
  __ Movi(v18.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);
  __ Movi(v19.V2D(), 0x0102030405060708, 0x090a0b0c0d0e0f00);

  __ Smlsl(v16.V8H(), v0.V8B(), v1.V8B());
  __ Umlsl(v17.V8H(), v0.V8B(), v1.V8B());
  __ Smlsl2(v18.V8H(), v0.V16B(), v1.V16B());
  __ Umlsl2(v19.V8H(), v0.V16B(), v1.V16B());

  END();

  RUN();
  ASSERT_EQUAL_128(0x00ac030404b0eacf, 0x090a0b610d0e0eaa, q16);
  ASSERT_EQUAL_128(0x57ac03045bb0eacf, 0x090ab6610d0e65aa, q17);
  ASSERT_EQUAL_128(0x0102030421942396, 0x090a0b610d630f00, q18);
  ASSERT_EQUAL_128(0x01020304cc94ce96, 0x090ab661b8630f00, q19);
  TEARDOWN();
}


TEST(neon_3same_compare) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xff00aa5500ff55aa, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(), 0x000055aaff55ff00, 0xaa55ff55555500ff);

  __ Cmeq(v16.V16B(), v0.V16B(), v0.V16B());
  __ Cmeq(v17.V16B(), v0.V16B(), v1.V16B());
  __ Cmge(v18.V16B(), v0.V16B(), v0.V16B());
  __ Cmge(v19.V16B(), v0.V16B(), v1.V16B());
  __ Cmgt(v20.V16B(), v0.V16B(), v0.V16B());
  __ Cmgt(v21.V16B(), v0.V16B(), v1.V16B());
  __ Cmhi(v22.V16B(), v0.V16B(), v0.V16B());
  __ Cmhi(v23.V16B(), v0.V16B(), v1.V16B());
  __ Cmhs(v24.V16B(), v0.V16B(), v0.V16B());
  __ Cmhs(v25.V16B(), v0.V16B(), v1.V16B());

  END();

  RUN();
  ASSERT_EQUAL_128(0xffffffffffffffff, 0xffffffffffffffff, q16);
  ASSERT_EQUAL_128(0x00ff000000000000, 0x000000ff00000000, q17);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0xffffffffffffffff, q18);
  ASSERT_EQUAL_128(0x00ff00ffff00ff00, 0xff0000ff0000ff00, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q20);
  ASSERT_EQUAL_128(0x000000ffff00ff00, 0xff0000000000ff00, q21);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q22);
  ASSERT_EQUAL_128(0xff00ff0000ff00ff, 0xff00000000ffff00, q23);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0xffffffffffffffff, q24);
  ASSERT_EQUAL_128(0xffffff0000ff00ff, 0xff0000ff00ffff00, q25);
  TEARDOWN();
}


TEST(neon_3same_scalar_compare) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xff00aa5500ff55aa, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(), 0x000055aaff55ff00, 0xaa55ff55555500ff);

  __ Cmeq(d16, d0, d0);
  __ Cmeq(d17, d0, d1);
  __ Cmeq(d18, d1, d0);
  __ Cmge(d19, d0, d0);
  __ Cmge(d20, d0, d1);
  __ Cmge(d21, d1, d0);
  __ Cmgt(d22, d0, d0);
  __ Cmgt(d23, d0, d1);
  __ Cmhi(d24, d0, d0);
  __ Cmhi(d25, d0, d1);
  __ Cmhs(d26, d0, d0);
  __ Cmhs(d27, d0, d1);
  __ Cmhs(d28, d1, d0);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q16);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q17);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q18);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q19);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q20);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q21);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q22);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q23);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q24);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q25);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q26);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q27);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q28);

  TEARDOWN();
}

TEST(neon_2regmisc_fcmeq) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0000000000000000, 0x0000000000000000);  // Zero.
  __ Movi(v1.V2D(), 0xffffffffffffffff, 0xffffffffffffffff);  // Nan.
  __ Movi(v2.V2D(), 0xbf800000bf800000, 0xbf800000bf800000);  // < 0.
  __ Movi(v3.V2D(), 0x3f8000003f800000, 0x3f8000003f800000);  // > 0.

  __ Fcmeq(s16, s0, 0.0);
  __ Fcmeq(s17, s1, 0.0);
  __ Fcmeq(s18, s2, 0.0);
  __ Fcmeq(d19, d0, 0.0);
  __ Fcmeq(d20, d1, 0.0);
  __ Fcmeq(d21, d2, 0.0);
  __ Fcmeq(v22.V2S(), v0.V2S(), 0.0);
  __ Fcmeq(v23.V4S(), v1.V4S(), 0.0);
  __ Fcmeq(v24.V2D(), v1.V2D(), 0.0);
  __ Fcmeq(v25.V2D(), v2.V2D(), 0.0);

  END();

  RUN();
  ASSERT_EQUAL_128(0, 0xffffffff, q16);
  ASSERT_EQUAL_128(0, 0x00000000, q17);
  ASSERT_EQUAL_128(0, 0x00000000, q18);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q19);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q20);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q21);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q22);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q24);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q25);
  TEARDOWN();
}

TEST(neon_2regmisc_fcmge) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0000000000000000, 0x0000000000000000);  // Zero.
  __ Movi(v1.V2D(), 0xffffffffffffffff, 0xffffffffffffffff);  // Nan.
  __ Movi(v2.V2D(), 0xbf800000bf800000, 0xbf800000bf800000);  // < 0.
  __ Movi(v3.V2D(), 0x3f8000003f800000, 0x3f8000003f800000);  // > 0.

  __ Fcmge(s16, s0, 0.0);
  __ Fcmge(s17, s1, 0.0);
  __ Fcmge(s18, s2, 0.0);
  __ Fcmge(d19, d0, 0.0);
  __ Fcmge(d20, d1, 0.0);
  __ Fcmge(d21, d3, 0.0);
  __ Fcmge(v22.V2S(), v0.V2S(), 0.0);
  __ Fcmge(v23.V4S(), v1.V4S(), 0.0);
  __ Fcmge(v24.V2D(), v1.V2D(), 0.0);
  __ Fcmge(v25.V2D(), v3.V2D(), 0.0);

  END();

  RUN();
  ASSERT_EQUAL_128(0, 0xffffffff, q16);
  ASSERT_EQUAL_128(0, 0x00000000, q17);
  ASSERT_EQUAL_128(0, 0x00000000, q18);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q19);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q20);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q21);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q22);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q24);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0xffffffffffffffff, q25);
  TEARDOWN();
}


TEST(neon_2regmisc_fcmgt) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0000000000000000, 0x0000000000000000);  // Zero.
  __ Movi(v1.V2D(), 0xffffffffffffffff, 0xffffffffffffffff);  // Nan.
  __ Movi(v2.V2D(), 0xbf800000bf800000, 0xbf800000bf800000);  // < 0.
  __ Movi(v3.V2D(), 0x3f8000003f800000, 0x3f8000003f800000);  // > 0.

  __ Fcmgt(s16, s0, 0.0);
  __ Fcmgt(s17, s1, 0.0);
  __ Fcmgt(s18, s2, 0.0);
  __ Fcmgt(d19, d0, 0.0);
  __ Fcmgt(d20, d1, 0.0);
  __ Fcmgt(d21, d3, 0.0);
  __ Fcmgt(v22.V2S(), v0.V2S(), 0.0);
  __ Fcmgt(v23.V4S(), v1.V4S(), 0.0);
  __ Fcmgt(v24.V2D(), v1.V2D(), 0.0);
  __ Fcmgt(v25.V2D(), v3.V2D(), 0.0);

  END();

  RUN();
  ASSERT_EQUAL_128(0, 0x00000000, q16);
  ASSERT_EQUAL_128(0, 0x00000000, q17);
  ASSERT_EQUAL_128(0, 0x00000000, q18);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q19);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q20);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q21);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q22);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q24);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0xffffffffffffffff, q25);
  TEARDOWN();
}

TEST(neon_2regmisc_fcmle) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0000000000000000, 0x0000000000000000);  // Zero.
  __ Movi(v1.V2D(), 0xffffffffffffffff, 0xffffffffffffffff);  // Nan.
  __ Movi(v2.V2D(), 0xbf800000bf800000, 0xbf800000bf800000);  // < 0.
  __ Movi(v3.V2D(), 0x3f8000003f800000, 0x3f8000003f800000);  // > 0.

  __ Fcmle(s16, s0, 0.0);
  __ Fcmle(s17, s1, 0.0);
  __ Fcmle(s18, s3, 0.0);
  __ Fcmle(d19, d0, 0.0);
  __ Fcmle(d20, d1, 0.0);
  __ Fcmle(d21, d2, 0.0);
  __ Fcmle(v22.V2S(), v0.V2S(), 0.0);
  __ Fcmle(v23.V4S(), v1.V4S(), 0.0);
  __ Fcmle(v24.V2D(), v1.V2D(), 0.0);
  __ Fcmle(v25.V2D(), v2.V2D(), 0.0);

  END();

  RUN();
  ASSERT_EQUAL_128(0, 0xffffffff, q16);
  ASSERT_EQUAL_128(0, 0x00000000, q17);
  ASSERT_EQUAL_128(0, 0x00000000, q18);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q19);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q20);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q21);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q22);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q24);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0xffffffffffffffff, q25);
  TEARDOWN();
}


TEST(neon_2regmisc_fcmlt) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0000000000000000, 0x0000000000000000);  // Zero.
  __ Movi(v1.V2D(), 0xffffffffffffffff, 0xffffffffffffffff);  // Nan.
  __ Movi(v2.V2D(), 0xbf800000bf800000, 0xbf800000bf800000);  // < 0.
  __ Movi(v3.V2D(), 0x3f8000003f800000, 0x3f8000003f800000);  // > 0.

  __ Fcmlt(s16, s0, 0.0);
  __ Fcmlt(s17, s1, 0.0);
  __ Fcmlt(s18, s3, 0.0);
  __ Fcmlt(d19, d0, 0.0);
  __ Fcmlt(d20, d1, 0.0);
  __ Fcmlt(d21, d2, 0.0);
  __ Fcmlt(v22.V2S(), v0.V2S(), 0.0);
  __ Fcmlt(v23.V4S(), v1.V4S(), 0.0);
  __ Fcmlt(v24.V2D(), v1.V2D(), 0.0);
  __ Fcmlt(v25.V2D(), v2.V2D(), 0.0);

  END();

  RUN();
  ASSERT_EQUAL_128(0, 0x00000000, q16);
  ASSERT_EQUAL_128(0, 0x00000000, q17);
  ASSERT_EQUAL_128(0, 0x00000000, q18);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q19);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q20);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q21);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q22);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q24);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0xffffffffffffffff, q25);
  TEARDOWN();
}

TEST(neon_2regmisc_cmeq) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0001000200030004, 0x0000000000000000);
  __ Movi(v1.V2D(), 0x000055aaff55ff00, 0x0000ff55555500ff);

  __ Cmeq(v16.V8B(),  v1.V8B(),  0);
  __ Cmeq(v17.V16B(), v1.V16B(), 0);
  __ Cmeq(v18.V4H(),  v1.V4H(),  0);
  __ Cmeq(v19.V8H(),  v1.V8H(),  0);
  __ Cmeq(v20.V2S(),  v0.V2S(),  0);
  __ Cmeq(v21.V4S(),  v0.V4S(),  0);
  __ Cmeq(d22,  d0,  0);
  __ Cmeq(d23,  d1,  0);
  __ Cmeq(v24.V2D(),  v0.V2D(),  0);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0xffff00000000ff00, q16);
  ASSERT_EQUAL_128(0xffff0000000000ff, 0xffff00000000ff00, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffff000000000000, q18);
  ASSERT_EQUAL_128(0xffff000000000000, 0xffff000000000000, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q21);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q22);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q24);
  TEARDOWN();
}


TEST(neon_2regmisc_cmge) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xff01000200030004, 0x0000000000000000);
  __ Movi(v1.V2D(), 0x000055aaff55ff00, 0x0000ff55555500ff);

  __ Cmge(v16.V8B(),  v1.V8B(),  0);
  __ Cmge(v17.V16B(), v1.V16B(), 0);
  __ Cmge(v18.V4H(),  v1.V4H(),  0);
  __ Cmge(v19.V8H(),  v1.V8H(),  0);
  __ Cmge(v20.V2S(),  v0.V2S(),  0);
  __ Cmge(v21.V4S(),  v0.V4S(),  0);
  __ Cmge(d22,  d0,  0);
  __ Cmge(d23,  d1,  0);
  __ Cmge(v24.V2D(),  v0.V2D(),  0);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0xffff00ffffffff00, q16);
  ASSERT_EQUAL_128(0xffffff0000ff00ff, 0xffff00ffffffff00, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffff0000ffffffff, q18);
  ASSERT_EQUAL_128(0xffffffff00000000, 0xffff0000ffffffff, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q20);
  ASSERT_EQUAL_128(0x00000000ffffffff, 0xffffffffffffffff, q21);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q22);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q24);
  TEARDOWN();
}


TEST(neon_2regmisc_cmlt) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0001000200030004, 0xff00000000000000);
  __ Movi(v1.V2D(), 0x000055aaff55ff00, 0x0000ff55555500ff);

  __ Cmlt(v16.V8B(),  v1.V8B(),  0);
  __ Cmlt(v17.V16B(), v1.V16B(), 0);
  __ Cmlt(v18.V4H(),  v1.V4H(),  0);
  __ Cmlt(v19.V8H(),  v1.V8H(),  0);
  __ Cmlt(v20.V2S(),  v1.V2S(),  0);
  __ Cmlt(v21.V4S(),  v1.V4S(),  0);
  __ Cmlt(d22,  d0,  0);
  __ Cmlt(d23,  d1,  0);
  __ Cmlt(v24.V2D(),  v0.V2D(),  0);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000ff00000000ff, q16);
  ASSERT_EQUAL_128(0x000000ffff00ff00, 0x0000ff00000000ff, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000ffff00000000, q18);
  ASSERT_EQUAL_128(0x00000000ffffffff, 0x0000ffff00000000, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q20);
  ASSERT_EQUAL_128(0x00000000ffffffff, 0x0000000000000000, q21);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q22);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q24);
  TEARDOWN();
}


TEST(neon_2regmisc_cmle) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0001000200030004, 0x0000000000000000);
  __ Movi(v1.V2D(), 0x000055aaff55ff00, 0x0000ff55555500ff);

  __ Cmle(v16.V8B(),  v1.V8B(),  0);
  __ Cmle(v17.V16B(), v1.V16B(), 0);
  __ Cmle(v18.V4H(),  v1.V4H(),  0);
  __ Cmle(v19.V8H(),  v1.V8H(),  0);
  __ Cmle(v20.V2S(),  v1.V2S(),  0);
  __ Cmle(v21.V4S(),  v1.V4S(),  0);
  __ Cmle(d22,  d0,  0);
  __ Cmle(d23,  d1,  0);
  __ Cmle(v24.V2D(),  v0.V2D(),  0);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffff000000ffff, q16);
  ASSERT_EQUAL_128(0xffff00ffff00ffff, 0xffffff000000ffff, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffff00000000, q18);
  ASSERT_EQUAL_128(0xffff0000ffffffff, 0xffffffff00000000, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q20);
  ASSERT_EQUAL_128(0x00000000ffffffff, 0x0000000000000000, q21);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q22);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q24);
  TEARDOWN();
}


TEST(neon_2regmisc_cmgt) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0001000200030004, 0x0000000000000000);
  __ Movi(v1.V2D(), 0x000055aaff55ff00, 0x0000ff55555500ff);

  __ Cmgt(v16.V8B(),  v1.V8B(),  0);
  __ Cmgt(v17.V16B(), v1.V16B(), 0);
  __ Cmgt(v18.V4H(),  v1.V4H(),  0);
  __ Cmgt(v19.V8H(),  v1.V8H(),  0);
  __ Cmgt(v20.V2S(),  v0.V2S(),  0);
  __ Cmgt(v21.V4S(),  v0.V4S(),  0);
  __ Cmgt(d22,  d0,  0);
  __ Cmgt(d23,  d1,  0);
  __ Cmgt(v24.V2D(),  v0.V2D(),  0);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000ffffff0000, q16);
  ASSERT_EQUAL_128(0x0000ff0000ff0000, 0x000000ffffff0000, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00000000ffffffff, q18);
  ASSERT_EQUAL_128(0x0000ffff00000000, 0x00000000ffffffff, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q20);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0x0000000000000000, q21);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q22);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffffffffffffffff, q23);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0x0000000000000000, q24);
  TEARDOWN();
}


TEST(neon_2regmisc_neg) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Neg(v16.V8B(),  v0.V8B());
  __ Neg(v17.V16B(), v0.V16B());
  __ Neg(v18.V4H(),  v1.V4H());
  __ Neg(v19.V8H(),  v1.V8H());
  __ Neg(v20.V2S(),  v2.V2S());
  __ Neg(v21.V4S(),  v2.V4S());
  __ Neg(d22, d3);
  __ Neg(v23.V2D(),  v3.V2D());
  __ Neg(v24.V2D(),  v4.V2D());

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x807f0100ff81807f, q16);
  ASSERT_EQUAL_128(0x81ff00017f8081ff, 0x807f0100ff81807f, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00010000ffff8001, q18);
  ASSERT_EQUAL_128(0x80007fff00010000, 0x00010000ffff8001, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000080000001, q20);
  ASSERT_EQUAL_128(0x8000000000000001, 0x0000000080000001, q21);
  ASSERT_EQUAL_128(0x0000000000000000, 0x8000000000000001, q22);
  ASSERT_EQUAL_128(0x7fffffffffffffff, 0x8000000000000001, q23);
  ASSERT_EQUAL_128(0x8000000000000000, 0x0000000000000000, q24);

  TEARDOWN();
}


TEST(neon_2regmisc_sqneg) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Sqneg(v16.V8B(),  v0.V8B());
  __ Sqneg(v17.V16B(), v0.V16B());
  __ Sqneg(v18.V4H(),  v1.V4H());
  __ Sqneg(v19.V8H(),  v1.V8H());
  __ Sqneg(v20.V2S(),  v2.V2S());
  __ Sqneg(v21.V4S(),  v2.V4S());
  __ Sqneg(v22.V2D(),  v3.V2D());
  __ Sqneg(v23.V2D(),  v4.V2D());

  __ Sqneg(b24, b0);
  __ Sqneg(h25, h1);
  __ Sqneg(s26, s2);
  __ Sqneg(d27, d3);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x7f7f0100ff817f7f, q16);
  ASSERT_EQUAL_128(0x81ff00017f7f81ff, 0x7f7f0100ff817f7f, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00010000ffff8001, q18);
  ASSERT_EQUAL_128(0x7fff7fff00010000, 0x00010000ffff8001, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000080000001, q20);
  ASSERT_EQUAL_128(0x7fffffff00000001, 0x0000000080000001, q21);
  ASSERT_EQUAL_128(0x7fffffffffffffff, 0x8000000000000001, q22);
  ASSERT_EQUAL_128(0x7fffffffffffffff, 0x0000000000000000, q23);

  ASSERT_EQUAL_128(0, 0x7f, q24);
  ASSERT_EQUAL_128(0, 0x8001, q25);
  ASSERT_EQUAL_128(0, 0x80000001, q26);
  ASSERT_EQUAL_128(0, 0x8000000000000001, q27);

  TEARDOWN();
}


TEST(neon_2regmisc_abs) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Abs(v16.V8B(),  v0.V8B());
  __ Abs(v17.V16B(), v0.V16B());
  __ Abs(v18.V4H(),  v1.V4H());
  __ Abs(v19.V8H(),  v1.V8H());
  __ Abs(v20.V2S(),  v2.V2S());
  __ Abs(v21.V4S(),  v2.V4S());
  __ Abs(d22, d3);
  __ Abs(v23.V2D(),  v3.V2D());
  __ Abs(v24.V2D(),  v4.V2D());

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x807f0100017f807f, q16);
  ASSERT_EQUAL_128(0x7f0100017f807f01, 0x807f0100017f807f, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0001000000017fff, q18);
  ASSERT_EQUAL_128(0x80007fff00010000, 0x0001000000017fff, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000007fffffff, q20);
  ASSERT_EQUAL_128(0x8000000000000001, 0x000000007fffffff, q21);
  ASSERT_EQUAL_128(0x0000000000000000, 0x7fffffffffffffff, q22);
  ASSERT_EQUAL_128(0x7fffffffffffffff, 0x7fffffffffffffff, q23);
  ASSERT_EQUAL_128(0x8000000000000000, 0x0000000000000000, q24);

  TEARDOWN();
}


TEST(neon_2regmisc_sqabs) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Sqabs(v16.V8B(),  v0.V8B());
  __ Sqabs(v17.V16B(), v0.V16B());
  __ Sqabs(v18.V4H(),  v1.V4H());
  __ Sqabs(v19.V8H(),  v1.V8H());
  __ Sqabs(v20.V2S(),  v2.V2S());
  __ Sqabs(v21.V4S(),  v2.V4S());
  __ Sqabs(v22.V2D(),  v3.V2D());
  __ Sqabs(v23.V2D(),  v4.V2D());

  __ Sqabs(b24, b0);
  __ Sqabs(h25, h1);
  __ Sqabs(s26, s2);
  __ Sqabs(d27, d3);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x7f7f0100017f7f7f, q16);
  ASSERT_EQUAL_128(0x7f0100017f7f7f01, 0x7f7f0100017f7f7f, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0001000000017fff, q18);
  ASSERT_EQUAL_128(0x7fff7fff00010000, 0x0001000000017fff, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000007fffffff, q20);
  ASSERT_EQUAL_128(0x7fffffff00000001, 0x000000007fffffff, q21);
  ASSERT_EQUAL_128(0x7fffffffffffffff, 0x7fffffffffffffff, q22);
  ASSERT_EQUAL_128(0x7fffffffffffffff, 0x0000000000000000, q23);

  ASSERT_EQUAL_128(0, 0x7f, q24);
  ASSERT_EQUAL_128(0, 0x7fff, q25);
  ASSERT_EQUAL_128(0, 0x7fffffff, q26);
  ASSERT_EQUAL_128(0, 0x7fffffffffffffff, q27);

  TEARDOWN();
}

TEST(neon_2regmisc_suqadd) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x017f8081ff00017f, 0x010080ff7f0180ff);

  __ Movi(v2.V2D(), 0x80008001ffff0000, 0xffff000000017ffd);
  __ Movi(v3.V2D(), 0xffff000080008001, 0x00017fffffff0001);

  __ Movi(v4.V2D(), 0x80000000fffffffe, 0xfffffff17ffffffe);
  __ Movi(v5.V2D(), 0xffffffff80000000, 0x7fffffff00000002);

  __ Movi(v6.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v7.V2D(), 0x8000000000000000, 0x8000000000000002);

  __ Mov(v16.V2D(), v0.V2D());
  __ Mov(v17.V2D(), v0.V2D());
  __ Mov(v18.V2D(), v2.V2D());
  __ Mov(v19.V2D(), v2.V2D());
  __ Mov(v20.V2D(), v4.V2D());
  __ Mov(v21.V2D(), v4.V2D());
  __ Mov(v22.V2D(), v6.V2D());

  __ Mov(v23.V2D(), v0.V2D());
  __ Mov(v24.V2D(), v2.V2D());
  __ Mov(v25.V2D(), v4.V2D());
  __ Mov(v26.V2D(), v6.V2D());

  __ Suqadd(v16.V8B(), v1.V8B());
  __ Suqadd(v17.V16B(), v1.V16B());
  __ Suqadd(v18.V4H(), v3.V4H());
  __ Suqadd(v19.V8H(), v3.V8H());
  __ Suqadd(v20.V2S(), v5.V2S());
  __ Suqadd(v21.V4S(), v5.V4S());
  __ Suqadd(v22.V2D(), v7.V2D());

  __ Suqadd(b23, b1);
  __ Suqadd(h24, h3);
  __ Suqadd(s25, s5);
  __ Suqadd(d26, d7);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x81817f7f7f7f007f, q16);
  ASSERT_EQUAL_128(0x7f7f7f7f7f807f7f, 0x81817f7f7f7f007f, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00007fff7fff7ffe, q18);
  ASSERT_EQUAL_128(0x7fff80017fff7fff, 0x00007fff7fff7ffe, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x7ffffff07fffffff, q20);
  ASSERT_EQUAL_128(0x7fffffff7ffffffe, 0x7ffffff07fffffff, q21);
  ASSERT_EQUAL_128(0x0000000000000001, 0x7fffffffffffffff, q22);

  ASSERT_EQUAL_128(0, 0x7f, q23);
  ASSERT_EQUAL_128(0, 0x7ffe, q24);
  ASSERT_EQUAL_128(0, 0x7fffffff, q25);
  ASSERT_EQUAL_128(0, 0x7fffffffffffffff, q26);
  TEARDOWN();
}

TEST(neon_2regmisc_usqadd) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f7ffe);
  __ Movi(v1.V2D(), 0x017f8081ff00017f, 0x010080ff7f018002);

  __ Movi(v2.V2D(), 0x80008001fffe0000, 0xffff000000017ffd);
  __ Movi(v3.V2D(), 0xffff000000028001, 0x00017fffffff0001);

  __ Movi(v4.V2D(), 0x80000000fffffffe, 0x00000001fffffffe);
  __ Movi(v5.V2D(), 0xffffffff80000000, 0xfffffffe00000002);

  __ Movi(v6.V2D(), 0x8000000000000002, 0x7fffffffffffffff);
  __ Movi(v7.V2D(), 0x7fffffffffffffff, 0x8000000000000000);

  __ Mov(v16.V2D(), v0.V2D());
  __ Mov(v17.V2D(), v0.V2D());
  __ Mov(v18.V2D(), v2.V2D());
  __ Mov(v19.V2D(), v2.V2D());
  __ Mov(v20.V2D(), v4.V2D());
  __ Mov(v21.V2D(), v4.V2D());
  __ Mov(v22.V2D(), v6.V2D());

  __ Mov(v23.V2D(), v0.V2D());
  __ Mov(v24.V2D(), v2.V2D());
  __ Mov(v25.V2D(), v4.V2D());
  __ Mov(v26.V2D(), v6.V2D());

  __ Usqadd(v16.V8B(), v1.V8B());
  __ Usqadd(v17.V16B(), v1.V16B());
  __ Usqadd(v18.V4H(), v3.V4H());
  __ Usqadd(v19.V8H(), v3.V8H());
  __ Usqadd(v20.V2S(), v5.V2S());
  __ Usqadd(v21.V4S(), v5.V4S());
  __ Usqadd(v22.V2D(), v7.V2D());

  __ Usqadd(b23, b1);
  __ Usqadd(h24, h3);
  __ Usqadd(s25, s5);
  __ Usqadd(d26, d7);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x81817f00808000ff, q16);
  ASSERT_EQUAL_128(0x8080008080808080, 0x81817f00808000ff, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffff7fff00007ffe, q18);
  ASSERT_EQUAL_128(0x7fff8001ffff0000, 0xffff7fff00007ffe, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00000000ffffffff, q20);
  ASSERT_EQUAL_128(0x7fffffff7ffffffe, 0x00000000ffffffff, q21);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0x0000000000000000, q22);

  ASSERT_EQUAL_128(0, 0xff, q23);
  ASSERT_EQUAL_128(0, 0x7ffe, q24);
  ASSERT_EQUAL_128(0, 0xffffffff, q25);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q26);
  TEARDOWN();
}


TEST(system_sys) {
  SETUP();
  const char* msg = "SYS test!";
  uintptr_t msg_addr = reinterpret_cast<uintptr_t>(msg);

  START();
  __ Mov(x4, msg_addr);
  __ Sys(3, 0x7, 0x5, 1, x4);
  __ Mov(x3, x4);
  __ Sys(3, 0x7, 0xa, 1, x3);
  __ Mov(x2, x3);
  __ Sys(3, 0x7, 0xb, 1, x2);
  __ Mov(x1, x2);
  __ Sys(3, 0x7, 0xe, 1, x1);
  // TODO: Add tests to check ZVA equivalent.
  END();

  RUN();

  TEARDOWN();
}


TEST(system_ic) {
  SETUP();
  const char* msg = "IC test!";
  uintptr_t msg_addr = reinterpret_cast<uintptr_t>(msg);

  START();
  __ Mov(x11, msg_addr);
  __ Ic(IVAU, x11);
  END();

  RUN();

  TEARDOWN();
}


TEST(system_dc) {
  SETUP();
  const char* msg = "DC test!";
  uintptr_t msg_addr = reinterpret_cast<uintptr_t>(msg);

  START();
  __ Mov(x20, msg_addr);
  __ Dc(CVAC, x20);
  __ Mov(x21, x20);
  __ Dc(CVAU, x21);
  __ Mov(x22, x21);
  __ Dc(CIVAC, x22);
  // TODO: Add tests to check ZVA.
  END();

  RUN();

  TEARDOWN();
}


TEST(neon_2regmisc_xtn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Xtn(v16.V8B(),   v0.V8H());
  __ Xtn2(v16.V16B(), v1.V8H());
  __ Xtn(v17.V4H(),   v1.V4S());
  __ Xtn2(v17.V8H(),  v2.V4S());
  __ Xtn(v18.V2S(),   v3.V2D());
  __ Xtn2(v18.V4S(),  v4.V2D());

  END();

  RUN();
  ASSERT_EQUAL_128(0x0001ff00ff0001ff, 0x01ff800181007f81, q16);
  ASSERT_EQUAL_128(0x0000ffff0000ffff, 0x8001000000007fff, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00000001ffffffff, q18);
  TEARDOWN();
}


TEST(neon_2regmisc_sqxtn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f01007a81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Sqxtn(v16.V8B(),   v0.V8H());
  __ Sqxtn2(v16.V16B(), v1.V8H());
  __ Sqxtn(v17.V4H(),   v1.V4S());
  __ Sqxtn2(v17.V8H(),  v2.V4S());
  __ Sqxtn(v18.V2S(),   v3.V2D());
  __ Sqxtn2(v18.V4S(),  v4.V2D());
  __ Sqxtn(b19,  h0);
  __ Sqxtn(h20,  s0);
  __ Sqxtn(s21,  d0);

  END();

  RUN();
  ASSERT_EQUAL_128(0x8080ff00ff00017f, 0x7f7a807f80807f80, q16);
  ASSERT_EQUAL_128(0x8000ffff00007fff, 0x8000800080007fff, q17);
  ASSERT_EQUAL_128(0x8000000000000000, 0x800000007fffffff, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000080, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000007fff, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000080000000, q21);
  TEARDOWN();
}


TEST(neon_2regmisc_uqxtn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f01007a81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Uqxtn(v16.V8B(),   v0.V8H());
  __ Uqxtn2(v16.V16B(), v1.V8H());
  __ Uqxtn(v17.V4H(),   v1.V4S());
  __ Uqxtn2(v17.V8H(),  v2.V4S());
  __ Uqxtn(v18.V2S(),   v3.V2D());
  __ Uqxtn2(v18.V4S(),  v4.V2D());
  __ Uqxtn(b19,  h0);
  __ Uqxtn(h20,  s0);
  __ Uqxtn(s21,  d0);

  END();

  RUN();
  ASSERT_EQUAL_128(0xffffff00ff0001ff, 0xff7affffffffffff, q16);
  ASSERT_EQUAL_128(0xffffffff0000ffff, 0xffffffffffffffff, q17);
  ASSERT_EQUAL_128(0xffffffff00000000, 0xffffffffffffffff, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00000000000000ff, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000000000ffff, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00000000ffffffff, q21);
  TEARDOWN();
}


TEST(neon_2regmisc_sqxtun) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f01007a81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Sqxtun(v16.V8B(),   v0.V8H());
  __ Sqxtun2(v16.V16B(), v1.V8H());
  __ Sqxtun(v17.V4H(),   v1.V4S());
  __ Sqxtun2(v17.V8H(),  v2.V4S());
  __ Sqxtun(v18.V2S(),   v3.V2D());
  __ Sqxtun2(v18.V4S(),  v4.V2D());
  __ Sqxtun(b19,  h0);
  __ Sqxtun(h20,  s0);
  __ Sqxtun(s21,  d0);

  END();

  RUN();
  ASSERT_EQUAL_128(0x00000000000001ff, 0xff7a00ff0000ff00, q16);
  ASSERT_EQUAL_128(0x000000000000ffff, 0x000000000000ffff, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00000000ffffffff, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000000000ffff, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q21);
  TEARDOWN();
}

TEST(neon_3same_and) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xff00aa5500ff55aa, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(), 0x00aa55aaff55ff00, 0xaa55ff00555500ff);

  __ And(v16.V16B(), v0.V16B(), v0.V16B());  // self test
  __ And(v17.V16B(), v0.V16B(), v1.V16B());  // all combinations
  __ And(v24.V8B(), v0.V8B(), v0.V8B());  // self test
  __ And(v25.V8B(), v0.V8B(), v1.V8B());  // all combinations
  END();

  RUN();
  ASSERT_EQUAL_128(0xff00aa5500ff55aa, 0xff00aa5500ff55aa, q16);
  ASSERT_EQUAL_128(0x0000000000555500, 0xaa00aa00005500aa, q17);
  ASSERT_EQUAL_128(0, 0xff00aa5500ff55aa, q24);
  ASSERT_EQUAL_128(0, 0xaa00aa00005500aa, q25);
  TEARDOWN();
}

TEST(neon_3same_bic) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xff00aa5500ff55aa, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(), 0x00ffaa00aa55aaff, 0xffff005500ff00ff);

  __ Bic(v16.V16B(), v0.V16B(), v0.V16B());  // self test
  __ Bic(v17.V16B(), v0.V16B(), v1.V16B());  // all combinations
  __ Bic(v24.V8B(), v0.V8B(), v0.V8B());  // self test
  __ Bic(v25.V8B(), v0.V8B(), v1.V8B());  // all combinations
  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q16);
  ASSERT_EQUAL_128(0xff00005500aa5500, 0x0000aa0000005500, q17);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q24);
  ASSERT_EQUAL_128(0, 0x0000aa0000005500, q25);
  TEARDOWN();
}

TEST(neon_3same_orr) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xff00aa5500ff55aa, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(), 0x00aa55aaff55ff00, 0xaa55ff00555500ff);

  __ Orr(v16.V16B(), v0.V16B(), v0.V16B());  // self test
  __ Orr(v17.V16B(), v0.V16B(), v1.V16B());  // all combinations
  __ Orr(v24.V8B(), v0.V8B(), v0.V8B());  // self test
  __ Orr(v25.V8B(), v0.V8B(), v1.V8B());  // all combinations
  END();

  RUN();
  ASSERT_EQUAL_128(0xff00aa5500ff55aa, 0xff00aa5500ff55aa, q16);
  ASSERT_EQUAL_128(0xffaaffffffffffaa, 0xff55ff5555ff55ff, q17);
  ASSERT_EQUAL_128(0, 0xff00aa5500ff55aa, q24);
  ASSERT_EQUAL_128(0, 0xff55ff5555ff55ff, q25);
  TEARDOWN();
}

TEST(neon_3same_mov) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xff00aa5500ff55aa, 0xff00aa5500ff55aa);

  __ Mov(v16.V16B(), v0.V16B());
  __ Mov(v17.V8H(), v0.V8H());
  __ Mov(v18.V4S(), v0.V4S());
  __ Mov(v19.V2D(), v0.V2D());

  __ Mov(v24.V8B(), v0.V8B());
  __ Mov(v25.V4H(), v0.V4H());
  __ Mov(v26.V2S(), v0.V2S());
  END();

  RUN();

  ASSERT_EQUAL_128(0xff00aa5500ff55aa, 0xff00aa5500ff55aa, q16);
  ASSERT_EQUAL_128(0xff00aa5500ff55aa, 0xff00aa5500ff55aa, q17);
  ASSERT_EQUAL_128(0xff00aa5500ff55aa, 0xff00aa5500ff55aa, q18);
  ASSERT_EQUAL_128(0xff00aa5500ff55aa, 0xff00aa5500ff55aa, q19);

  ASSERT_EQUAL_128(0x0, 0xff00aa5500ff55aa, q24);
  ASSERT_EQUAL_128(0x0, 0xff00aa5500ff55aa, q25);
  ASSERT_EQUAL_128(0x0, 0xff00aa5500ff55aa, q26);

  TEARDOWN();
}

TEST(neon_3same_orn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xff00aa5500ff55aa, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(), 0x00aa55aaff55ff00, 0xaa55ff00555500ff);

  __ Orn(v16.V16B(), v0.V16B(), v0.V16B());  // self test
  __ Orn(v17.V16B(), v0.V16B(), v1.V16B());  // all combinations
  __ Orn(v24.V8B(), v0.V8B(), v0.V8B());  // self test
  __ Orn(v25.V8B(), v0.V8B(), v1.V8B());  // all combinations
  END();

  RUN();
  ASSERT_EQUAL_128(0xffffffffffffffff, 0xffffffffffffffff, q16);
  ASSERT_EQUAL_128(0xff55aa5500ff55ff, 0xffaaaaffaaffffaa, q17);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q24);
  ASSERT_EQUAL_128(0, 0xffaaaaffaaffffaa, q25);
  TEARDOWN();
}

TEST(neon_3same_eor) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xff00aa5500ff55aa, 0xff00aa5500ff55aa);
  __ Movi(v1.V2D(), 0x00ffaa00aa55aaff, 0xffff005500ff00ff);

  __ Eor(v16.V16B(), v0.V16B(), v0.V16B());  // self test
  __ Eor(v17.V16B(), v0.V16B(), v1.V16B());  // all combinations
  __ Eor(v24.V8B(), v0.V8B(), v0.V8B());  // self test
  __ Eor(v25.V8B(), v0.V8B(), v1.V8B());  // all combinations
  END();

  RUN();
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q16);
  ASSERT_EQUAL_128(0xffff0055aaaaff55, 0x00ffaa0000005555, q17);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q24);
  ASSERT_EQUAL_128(0, 0x00ffaa0000005555, q25);
  TEARDOWN();
}

TEST(neon_3same_bif) {
  SETUP();

  START();

  __ Movi(v16.V2D(), 0xffff0000ff00ffff, 0xffff00000000aaaa);
  __ Movi(v0.V2D(), 0xff00ff00ff005555, 0xaaaa5555aaaaaaaa);
  __ Movi(v1.V2D(), 0x00ff00ffff0055aa, 0x55aa55aa55aa55aa);

  __ Movi(v17.V2D(), 0x5555aa55cccccccc, 0x33333333f0f0f0f0);
  __ Movi(v2.V2D(), 0x555555aaff00ff00, 0xff00ff00ff00ff00);
  __ Movi(v3.V2D(), 0xaa55aa5500ffff00, 0x00ffff0000ffff00);

  __ Movi(v18.V2D(), 0, 0xf0f0f0f00f0f0f0f);
  __ Movi(v4.V2D(), 0, 0xf0f0f0f0f0f0f0f0);
  __ Movi(v5.V2D(), 0, 0x00ffff0000ffff00);

  __ Bif(v16.V16B(), v0.V16B(), v1.V16B());
  __ Bif(v17.V16B(), v2.V16B(), v3.V16B());
  __ Bif(v18.V8B(), v4.V8B(), v5.V8B());
  END();

  RUN();

  ASSERT_EQUAL_128(0xffffff00ff0055ff, 0xffaa0055aa00aaaa, q16);
  ASSERT_EQUAL_128(0x5555ffffffcccc00, 0xff333300fff0f000, q17);
  ASSERT_EQUAL_128(0, 0xf0f0f0f0f00f0ff0, q18);
  TEARDOWN();
}

TEST(neon_3same_bit) {
  SETUP();

  START();

  __ Movi(v16.V2D(), 0xffff0000ff00ffff, 0xffff00000000aaaa);
  __ Movi(v0.V2D(), 0xff00ff00ff005555, 0xaaaa5555aaaaaaaa);
  __ Movi(v1.V2D(), 0x00ff00ffff0055aa, 0x55aa55aa55aa55aa);

  __ Movi(v17.V2D(), 0x5555aa55cccccccc, 0x33333333f0f0f0f0);
  __ Movi(v2.V2D(), 0x555555aaff00ff00, 0xff00ff00ff00ff00);
  __ Movi(v3.V2D(), 0xaa55aa5500ffff00, 0x00ffff0000ffff00);

  __ Movi(v18.V2D(), 0, 0xf0f0f0f00f0f0f0f);
  __ Movi(v4.V2D(), 0, 0xf0f0f0f0f0f0f0f0);
  __ Movi(v5.V2D(), 0, 0x00ffff0000ffff00);

  __ Bit(v16.V16B(), v0.V16B(), v1.V16B());
  __ Bit(v17.V16B(), v2.V16B(), v3.V16B());
  __ Bit(v18.V8B(), v4.V8B(), v5.V8B());
  END();

  RUN();

  ASSERT_EQUAL_128(0xff000000ff00ff55, 0xaaff550000aaaaaa, q16);
  ASSERT_EQUAL_128(0x55550000cc00ffcc, 0x3300ff33f000fff0, q17);
  ASSERT_EQUAL_128(0, 0xf0f0f0f00ff0f00f, q18);
  TEARDOWN();
}

TEST(neon_3same_bsl) {
  SETUP();

  START();

  __ Movi(v16.V2D(), 0xffff0000ff00ffff, 0xffff00000000aaaa);
  __ Movi(v0.V2D(), 0xff00ff00ff005555, 0xaaaa5555aaaaaaaa);
  __ Movi(v1.V2D(), 0x00ff00ffff0055aa, 0x55aa55aa55aa55aa);

  __ Movi(v17.V2D(), 0x5555aa55cccccccc, 0x33333333f0f0f0f0);
  __ Movi(v2.V2D(), 0x555555aaff00ff00, 0xff00ff00ff00ff00);
  __ Movi(v3.V2D(), 0xaa55aa5500ffff00, 0x00ffff0000ffff00);

  __ Movi(v18.V2D(), 0, 0xf0f0f0f00f0f0f0f);
  __ Movi(v4.V2D(), 0, 0xf0f0f0f0f0f0f0f0);
  __ Movi(v5.V2D(), 0, 0x00ffff0000ffff00);

  __ Bsl(v16.V16B(), v0.V16B(), v1.V16B());
  __ Bsl(v17.V16B(), v2.V16B(), v3.V16B());
  __ Bsl(v18.V8B(), v4.V8B(), v5.V8B());
  END();

  RUN();

  ASSERT_EQUAL_128(0xff0000ffff005555, 0xaaaa55aa55aaffaa, q16);
  ASSERT_EQUAL_128(0xff550000cc33ff00, 0x33ccff00f00fff00, q17);
  ASSERT_EQUAL_128(0, 0xf0fffff000f0f000, q18);
  TEARDOWN();
}


TEST(neon_3same_smax) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaa55555555, 0xffffffff0000aa55);
  __ Movi(v1.V2D(), 0x55aa5555aaaaaaaa, 0x00000000ffaa55ff);

  __ Smax(v16.V8B(), v0.V8B(), v1.V8B());
  __ Smax(v18.V4H(), v0.V4H(), v1.V4H());
  __ Smax(v20.V2S(), v0.V2S(), v1.V2S());

  __ Smax(v17.V16B(), v0.V16B(), v1.V16B());
  __ Smax(v19.V8H(), v0.V8H(), v1.V8H());
  __ Smax(v21.V4S(), v0.V4S(), v1.V4S());
  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0x0000000000005555, q16);
  ASSERT_EQUAL_128(0x0, 0x00000000000055ff, q18);
  ASSERT_EQUAL_128(0x0, 0x000000000000aa55, q20);
  ASSERT_EQUAL_128(0x55aa555555555555, 0x0000000000005555, q17);
  ASSERT_EQUAL_128(0x55aa555555555555, 0x00000000000055ff, q19);
  ASSERT_EQUAL_128(0x55aa555555555555, 0x000000000000aa55, q21);
  TEARDOWN();
}


TEST(neon_3same_smaxp) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaa55555555, 0xffffffff0000aa55);
  __ Movi(v1.V2D(), 0x55aa5555aaaaaaaa, 0x00000000ffaa55ff);

  __ Smaxp(v16.V8B(), v0.V8B(), v1.V8B());
  __ Smaxp(v18.V4H(), v0.V4H(), v1.V4H());
  __ Smaxp(v20.V2S(), v0.V2S(), v1.V2S());

  __ Smaxp(v17.V16B(), v0.V16B(), v1.V16B());
  __ Smaxp(v19.V8H(), v0.V8H(), v1.V8H());
  __ Smaxp(v21.V4S(), v0.V4S(), v1.V4S());
  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0x0000ff55ffff0055, q16);
  ASSERT_EQUAL_128(0x0, 0x000055ffffff0000, q18);
  ASSERT_EQUAL_128(0x0, 0x000000000000aa55, q20);
  ASSERT_EQUAL_128(0x5555aaaa0000ff55, 0xaaaa5555ffff0055, q17);
  ASSERT_EQUAL_128(0x55aaaaaa000055ff, 0xaaaa5555ffff0000, q19);
  ASSERT_EQUAL_128(0x55aa555500000000, 0x555555550000aa55, q21);
  TEARDOWN();
}


TEST(neon_addp_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0011223344aafe80, 0x00112233aabbfc00);
  __ Movi(v1.V2D(), 0x55aa5555aaaaaaaa, 0x00000000ffaa55ff);
  __ Movi(v2.V2D(), 0xaaaaaaaa66555555, 0xffffffff0000aa00);

  __ Addp(d16, v0.V2D());
  __ Addp(d17, v1.V2D());
  __ Addp(d18, v2.V2D());

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0x00224466ef66fa80, q16);
  ASSERT_EQUAL_128(0x0, 0x55aa5556aa5500a9, q17);
  ASSERT_EQUAL_128(0x0, 0xaaaaaaa96655ff55, q18);
  TEARDOWN();
}

TEST(neon_acrosslanes_addv) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0011223344aafe80, 0x00112233aabbfc00);
  __ Movi(v1.V2D(), 0x55aa5555aaaaaaaa, 0x00000000ffaa55ff);
  __ Movi(v2.V2D(), 0xaaaaaaaa66555555, 0xffffffff0000aa00);

  __ Addv(b16, v0.V8B());
  __ Addv(b17, v0.V16B());
  __ Addv(h18, v1.V4H());
  __ Addv(h19, v1.V8H());
  __ Addv(s20, v2.V4S());

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0xc7, q16);
  ASSERT_EQUAL_128(0x0, 0x99, q17);
  ASSERT_EQUAL_128(0x0, 0x55a9, q18);
  ASSERT_EQUAL_128(0x0, 0x55fc, q19);
  ASSERT_EQUAL_128(0x0, 0x1100a9fe, q20);
  TEARDOWN();
}


TEST(neon_acrosslanes_saddlv) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0011223344aafe80, 0x00112233aabbfc00);
  __ Movi(v1.V2D(), 0x55aa5555aaaaaaaa, 0x00000000ffaa55ff);
  __ Movi(v2.V2D(), 0xaaaaaaaa66555555, 0xffffffff0000aa00);

  __ Saddlv(h16, v0.V8B());
  __ Saddlv(h17, v0.V16B());
  __ Saddlv(s18, v1.V4H());
  __ Saddlv(s19, v1.V8H());
  __ Saddlv(d20, v2.V4S());

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0xffc7, q16);
  ASSERT_EQUAL_128(0x0, 0xff99, q17);
  ASSERT_EQUAL_128(0x0, 0x000055a9, q18);
  ASSERT_EQUAL_128(0x0, 0x000055fc, q19);
  ASSERT_EQUAL_128(0x0, 0x0000001100a9fe, q20);
  TEARDOWN();
}


TEST(neon_acrosslanes_uaddlv) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0011223344aafe80, 0x00112233aabbfc00);
  __ Movi(v1.V2D(), 0x55aa5555aaaaaaaa, 0x00000000ffaa55ff);
  __ Movi(v2.V2D(), 0xaaaaaaaa66555555, 0xffffffff0000aa00);

  __ Uaddlv(h16, v0.V8B());
  __ Uaddlv(h17, v0.V16B());
  __ Uaddlv(s18, v1.V4H());
  __ Uaddlv(s19, v1.V8H());
  __ Uaddlv(d20, v2.V4S());

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0x02c7, q16);
  ASSERT_EQUAL_128(0x0, 0x0599, q17);
  ASSERT_EQUAL_128(0x0, 0x000155a9, q18);
  ASSERT_EQUAL_128(0x0, 0x000355fc, q19);
  ASSERT_EQUAL_128(0x0, 0x000000021100a9fe, q20);
  TEARDOWN();
}


TEST(neon_acrosslanes_smaxv) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0011223344aafe80, 0x00112233aabbfc00);
  __ Movi(v1.V2D(), 0x55aa5555aaaaaaaa, 0x00000000ffaa55ff);
  __ Movi(v2.V2D(), 0xaaaaaaaa66555555, 0xffffffff0000aa00);

  __ Smaxv(b16, v0.V8B());
  __ Smaxv(b17, v0.V16B());
  __ Smaxv(h18, v1.V4H());
  __ Smaxv(h19, v1.V8H());
  __ Smaxv(s20, v2.V4S());

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0x33, q16);
  ASSERT_EQUAL_128(0x0, 0x44, q17);
  ASSERT_EQUAL_128(0x0, 0x55ff, q18);
  ASSERT_EQUAL_128(0x0, 0x55ff, q19);
  ASSERT_EQUAL_128(0x0, 0x66555555, q20);
  TEARDOWN();
}


TEST(neon_acrosslanes_sminv) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0011223344aafe80, 0x00112233aabbfc00);
  __ Movi(v1.V2D(), 0xfffa5555aaaaaaaa, 0x00000000ffaa55ff);
  __ Movi(v2.V2D(), 0xaaaaaaaa66555555, 0xffffffff0000aa00);

  __ Sminv(b16, v0.V8B());
  __ Sminv(b17, v0.V16B());
  __ Sminv(h18, v1.V4H());
  __ Sminv(h19, v1.V8H());
  __ Sminv(s20, v2.V4S());

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0xaa, q16);
  ASSERT_EQUAL_128(0x0, 0x80, q17);
  ASSERT_EQUAL_128(0x0, 0xffaa, q18);
  ASSERT_EQUAL_128(0x0, 0xaaaa, q19);
  ASSERT_EQUAL_128(0x0, 0xaaaaaaaa, q20);
  TEARDOWN();
}

TEST(neon_acrosslanes_umaxv) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0011223344aafe80, 0x00112233aabbfc00);
  __ Movi(v1.V2D(), 0x55aa5555aaaaffab, 0x00000000ffaa55ff);
  __ Movi(v2.V2D(), 0xaaaaaaaa66555555, 0xffffffff0000aa00);

  __ Umaxv(b16, v0.V8B());
  __ Umaxv(b17, v0.V16B());
  __ Umaxv(h18, v1.V4H());
  __ Umaxv(h19, v1.V8H());
  __ Umaxv(s20, v2.V4S());

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0xfc, q16);
  ASSERT_EQUAL_128(0x0, 0xfe, q17);
  ASSERT_EQUAL_128(0x0, 0xffaa, q18);
  ASSERT_EQUAL_128(0x0, 0xffab, q19);
  ASSERT_EQUAL_128(0x0, 0xffffffff, q20);
  TEARDOWN();
}


TEST(neon_acrosslanes_uminv) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0011223344aafe80, 0x02112233aabbfc01);
  __ Movi(v1.V2D(), 0xfffa5555aaaa0000, 0x00010003ffaa55ff);
  __ Movi(v2.V2D(), 0xaaaaaaaa66555555, 0xffffffff0000aa00);

  __ Uminv(b16, v0.V8B());
  __ Uminv(b17, v0.V16B());
  __ Uminv(h18, v1.V4H());
  __ Uminv(h19, v1.V8H());
  __ Uminv(s20, v2.V4S());

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0x01, q16);
  ASSERT_EQUAL_128(0x0, 0x00, q17);
  ASSERT_EQUAL_128(0x0, 0x0001, q18);
  ASSERT_EQUAL_128(0x0, 0x0000, q19);
  ASSERT_EQUAL_128(0x0, 0x0000aa00, q20);
  TEARDOWN();
}


TEST(neon_3same_smin) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaa55555555, 0xffffffff0000aa55);
  __ Movi(v1.V2D(), 0x55aa5555aaaaaaaa, 0x00000000ffaa55ff);

  __ Smin(v16.V8B(), v0.V8B(), v1.V8B());
  __ Smin(v18.V4H(), v0.V4H(), v1.V4H());
  __ Smin(v20.V2S(), v0.V2S(), v1.V2S());

  __ Smin(v17.V16B(), v0.V16B(), v1.V16B());
  __ Smin(v19.V8H(), v0.V8H(), v1.V8H());
  __ Smin(v21.V4S(), v0.V4S(), v1.V4S());
  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0xffffffffffaaaaff, q16);
  ASSERT_EQUAL_128(0x0, 0xffffffffffaaaa55, q18);
  ASSERT_EQUAL_128(0x0, 0xffffffffffaa55ff, q20);
  ASSERT_EQUAL_128(0xaaaaaaaaaaaaaaaa, 0xffffffffffaaaaff, q17);
  ASSERT_EQUAL_128(0xaaaaaaaaaaaaaaaa, 0xffffffffffaaaa55, q19);
  ASSERT_EQUAL_128(0xaaaaaaaaaaaaaaaa, 0xffffffffffaa55ff, q21);
  TEARDOWN();
}


TEST(neon_3same_umax) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaa55555555, 0xffffffff0000aa55);
  __ Movi(v1.V2D(), 0x55aa5555aaaaaaaa, 0x00000000ffaa55ff);

  __ Umax(v16.V8B(), v0.V8B(), v1.V8B());
  __ Umax(v18.V4H(), v0.V4H(), v1.V4H());
  __ Umax(v20.V2S(), v0.V2S(), v1.V2S());

  __ Umax(v17.V16B(), v0.V16B(), v1.V16B());
  __ Umax(v19.V8H(), v0.V8H(), v1.V8H());
  __ Umax(v21.V4S(), v0.V4S(), v1.V4S());
  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0xffffffffffaaaaff, q16);
  ASSERT_EQUAL_128(0x0, 0xffffffffffaaaa55, q18);
  ASSERT_EQUAL_128(0x0, 0xffffffffffaa55ff, q20);
  ASSERT_EQUAL_128(0xaaaaaaaaaaaaaaaa, 0xffffffffffaaaaff, q17);
  ASSERT_EQUAL_128(0xaaaaaaaaaaaaaaaa, 0xffffffffffaaaa55, q19);
  ASSERT_EQUAL_128(0xaaaaaaaaaaaaaaaa, 0xffffffffffaa55ff, q21);
  TEARDOWN();
}


TEST(neon_3same_umin) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaa55555555, 0xffffffff0000aa55);
  __ Movi(v1.V2D(), 0x55aa5555aaaaaaaa, 0x00000000ffaa55ff);

  __ Umin(v16.V8B(), v0.V8B(), v1.V8B());
  __ Umin(v18.V4H(), v0.V4H(), v1.V4H());
  __ Umin(v20.V2S(), v0.V2S(), v1.V2S());

  __ Umin(v17.V16B(), v0.V16B(), v1.V16B());
  __ Umin(v19.V8H(), v0.V8H(), v1.V8H());
  __ Umin(v21.V4S(), v0.V4S(), v1.V4S());
  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0x0000000000005555, q16);
  ASSERT_EQUAL_128(0x0, 0x00000000000055ff, q18);
  ASSERT_EQUAL_128(0x0, 0x000000000000aa55, q20);
  ASSERT_EQUAL_128(0x55aa555555555555, 0x0000000000005555, q17);
  ASSERT_EQUAL_128(0x55aa555555555555, 0x00000000000055ff, q19);
  ASSERT_EQUAL_128(0x55aa555555555555, 0x000000000000aa55, q21);
  TEARDOWN();
}


TEST(neon_2regmisc_mvn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x00ff00ffff0055aa, 0x55aa55aa55aa55aa);

  __ Mvn(v16.V16B(), v0.V16B());
  __ Mvn(v17.V8H(), v0.V8H());
  __ Mvn(v18.V4S(), v0.V4S());
  __ Mvn(v19.V2D(), v0.V2D());

  __ Mvn(v24.V8B(), v0.V8B());
  __ Mvn(v25.V4H(), v0.V4H());
  __ Mvn(v26.V2S(), v0.V2S());

  END();

  RUN();

  ASSERT_EQUAL_128(0xff00ff0000ffaa55, 0xaa55aa55aa55aa55, q16);
  ASSERT_EQUAL_128(0xff00ff0000ffaa55, 0xaa55aa55aa55aa55, q17);
  ASSERT_EQUAL_128(0xff00ff0000ffaa55, 0xaa55aa55aa55aa55, q18);
  ASSERT_EQUAL_128(0xff00ff0000ffaa55, 0xaa55aa55aa55aa55, q19);

  ASSERT_EQUAL_128(0x0, 0xaa55aa55aa55aa55, q24);
  ASSERT_EQUAL_128(0x0, 0xaa55aa55aa55aa55, q25);
  ASSERT_EQUAL_128(0x0, 0xaa55aa55aa55aa55, q26);
  TEARDOWN();
}


TEST(neon_2regmisc_not) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x00ff00ffff0055aa, 0x55aa55aa55aa55aa);
  __ Movi(v1.V2D(), 0, 0x00ffff0000ffff00);

  __ Not(v16.V16B(), v0.V16B());
  __ Not(v17.V8B(), v1.V8B());
  END();

  RUN();

  ASSERT_EQUAL_128(0xff00ff0000ffaa55, 0xaa55aa55aa55aa55, q16);
  ASSERT_EQUAL_128(0x0, 0xff0000ffff0000ff, q17);
  TEARDOWN();
}

TEST(neon_2regmisc_cls_clz_cnt) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0001020304050607, 0x08090a0b0c0d0e0f);
  __ Movi(v1.V2D(), 0xfedcba9876543210, 0x0123456789abcdef);

  __ Cls(v16.V8B() , v1.V8B());
  __ Cls(v17.V16B(), v1.V16B());
  __ Cls(v18.V4H() , v1.V4H());
  __ Cls(v19.V8H() , v1.V8H());
  __ Cls(v20.V2S() , v1.V2S());
  __ Cls(v21.V4S() , v1.V4S());

  __ Clz(v22.V8B() , v0.V8B());
  __ Clz(v23.V16B(), v0.V16B());
  __ Clz(v24.V4H() , v0.V4H());
  __ Clz(v25.V8H() , v0.V8H());
  __ Clz(v26.V2S() , v0.V2S());
  __ Clz(v27.V4S() , v0.V4S());

  __ Cnt(v28.V8B() , v0.V8B());
  __ Cnt(v29.V16B(), v1.V16B());

  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x0601000000000102, q16);
  ASSERT_EQUAL_128(0x0601000000000102, 0x0601000000000102, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0006000000000001, q18);
  ASSERT_EQUAL_128(0x0006000000000001, 0x0006000000000001, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000600000000, q20);
  ASSERT_EQUAL_128(0x0000000600000000, 0x0000000600000000, q21);

  ASSERT_EQUAL_128(0x0000000000000000, 0x0404040404040404, q22);
  ASSERT_EQUAL_128(0x0807060605050505, 0x0404040404040404, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0004000400040004, q24);
  ASSERT_EQUAL_128(0x000f000600050005, 0x0004000400040004, q25);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000400000004, q26);
  ASSERT_EQUAL_128(0x0000000f00000005, 0x0000000400000004, q27);

  ASSERT_EQUAL_128(0x0000000000000000, 0x0102020302030304, q28);
  ASSERT_EQUAL_128(0x0705050305030301, 0x0103030503050507, q29);

  TEARDOWN();
}

TEST(neon_2regmisc_rev) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0001020304050607, 0x08090a0b0c0d0e0f);
  __ Movi(v1.V2D(), 0xfedcba9876543210, 0x0123456789abcdef);

  __ Rev16(v16.V8B() , v0.V8B());
  __ Rev16(v17.V16B(), v0.V16B());

  __ Rev32(v18.V8B() , v0.V8B());
  __ Rev32(v19.V16B(), v0.V16B());
  __ Rev32(v20.V4H() , v0.V4H());
  __ Rev32(v21.V8H() , v0.V8H());

  __ Rev64(v22.V8B() , v0.V8B());
  __ Rev64(v23.V16B(), v0.V16B());
  __ Rev64(v24.V4H() , v0.V4H());
  __ Rev64(v25.V8H() , v0.V8H());
  __ Rev64(v26.V2S() , v0.V2S());
  __ Rev64(v27.V4S() , v0.V4S());

  __ Rbit(v28.V8B() , v1.V8B());
  __ Rbit(v29.V16B(), v1.V16B());

  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x09080b0a0d0c0f0e, q16);
  ASSERT_EQUAL_128(0x0100030205040706, 0x09080b0a0d0c0f0e, q17);

  ASSERT_EQUAL_128(0x0000000000000000, 0x0b0a09080f0e0d0c, q18);
  ASSERT_EQUAL_128(0x0302010007060504, 0x0b0a09080f0e0d0c, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0a0b08090e0f0c0d, q20);
  ASSERT_EQUAL_128(0x0203000106070405, 0x0a0b08090e0f0c0d, q21);

  ASSERT_EQUAL_128(0x0000000000000000, 0x0f0e0d0c0b0a0908, q22);
  ASSERT_EQUAL_128(0x0706050403020100, 0x0f0e0d0c0b0a0908, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0e0f0c0d0a0b0809, q24);
  ASSERT_EQUAL_128(0x0607040502030001, 0x0e0f0c0d0a0b0809, q25);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0c0d0e0f08090a0b, q26);
  ASSERT_EQUAL_128(0x0405060700010203, 0x0c0d0e0f08090a0b, q27);

  ASSERT_EQUAL_128(0x0000000000000000, 0x80c4a2e691d5b3f7, q28);
  ASSERT_EQUAL_128(0x7f3b5d196e2a4c08, 0x80c4a2e691d5b3f7, q29);

  TEARDOWN();
}


TEST(neon_sli) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0001020304050607, 0x08090a0b0c0d0e0f);
  __ Movi(v1.V2D(), 0xfedcba9876543210, 0x0123456789abcdef);

  __ Mov(v16.V2D(),  v0.V2D());
  __ Mov(v17.V2D(),  v0.V2D());
  __ Mov(v18.V2D(),  v0.V2D());
  __ Mov(v19.V2D(),  v0.V2D());
  __ Mov(v20.V2D(),  v0.V2D());
  __ Mov(v21.V2D(),  v0.V2D());
  __ Mov(v22.V2D(),  v0.V2D());
  __ Mov(v23.V2D(),  v0.V2D());

  __ Sli(v16.V8B(),  v1.V8B(),  4);
  __ Sli(v17.V16B(), v1.V16B(), 7);
  __ Sli(v18.V4H(),  v1.V4H(),  8);
  __ Sli(v19.V8H(),  v1.V8H(), 15);
  __ Sli(v20.V2S(),  v1.V2S(),  0);
  __ Sli(v21.V4S(),  v1.V4S(), 31);
  __ Sli(v22.V2D(),  v1.V2D(), 48);

  __ Sli(d23,  d1, 48);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x18395a7b9cbddeff, q16);
  ASSERT_EQUAL_128(0x0001020304050607, 0x88898a8b8c8d8e8f, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x2309670bab0def0f, q18);
  ASSERT_EQUAL_128(0x0001020304050607, 0x88098a0b8c0d8e0f, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0123456789abcdef, q20);
  ASSERT_EQUAL_128(0x0001020304050607, 0x88090a0b8c0d0e0f, q21);
  ASSERT_EQUAL_128(0x3210020304050607, 0xcdef0a0b0c0d0e0f, q22);

  ASSERT_EQUAL_128(0x0000000000000000, 0xcdef0a0b0c0d0e0f, q23);


  TEARDOWN();
}


TEST(neon_sri) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0001020304050607, 0x08090a0b0c0d0e0f);
  __ Movi(v1.V2D(), 0xfedcba9876543210, 0x0123456789abcdef);

  __ Mov(v16.V2D(),  v0.V2D());
  __ Mov(v17.V2D(),  v0.V2D());
  __ Mov(v18.V2D(),  v0.V2D());
  __ Mov(v19.V2D(),  v0.V2D());
  __ Mov(v20.V2D(),  v0.V2D());
  __ Mov(v21.V2D(),  v0.V2D());
  __ Mov(v22.V2D(),  v0.V2D());
  __ Mov(v23.V2D(),  v0.V2D());

  __ Sri(v16.V8B(),  v1.V8B(),  4);
  __ Sri(v17.V16B(), v1.V16B(), 7);
  __ Sri(v18.V4H(),  v1.V4H(),  8);
  __ Sri(v19.V8H(),  v1.V8H(), 15);
  __ Sri(v20.V2S(),  v1.V2S(),  1);
  __ Sri(v21.V4S(),  v1.V4S(), 31);
  __ Sri(v22.V2D(),  v1.V2D(), 48);

  __ Sri(d23,  d1, 48);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x00020406080a0c0e, q16);
  ASSERT_EQUAL_128(0x0101030304040606, 0x08080a0a0d0d0f0f, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x08010a450c890ecd, q18);
  ASSERT_EQUAL_128(0x0001020304040606, 0x08080a0a0c0d0e0f, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0091a2b344d5e6f7, q20);
  ASSERT_EQUAL_128(0x0001020304050606, 0x08090a0a0c0d0e0f, q21);
  ASSERT_EQUAL_128(0x000102030405fedc, 0x08090a0b0c0d0123, q22);

  ASSERT_EQUAL_128(0x0000000000000000, 0x08090a0b0c0d0123, q23);


  TEARDOWN();
}


TEST(neon_shrn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Shrn(v16.V8B(),   v0.V8H(), 8);
  __ Shrn2(v16.V16B(), v1.V8H(), 1);
  __ Shrn(v17.V4H(),   v1.V4S(), 16);
  __ Shrn2(v17.V8H(),  v2.V4S(), 1);
  __ Shrn(v18.V2S(),   v3.V2D(), 32);
  __ Shrn2(v18.V4S(),  v3.V2D(), 1);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0000ff00ff0000ff, 0x7f00817f80ff0180, q16);
  ASSERT_EQUAL_128(0x0000ffff0000ffff, 0x8000ffffffff0001, q17);
  ASSERT_EQUAL_128(0x00000000ffffffff, 0x800000007fffffff, q18);
  TEARDOWN();
}


TEST(neon_rshrn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Rshrn(v16.V8B(),   v0.V8H(), 8);
  __ Rshrn2(v16.V16B(), v1.V8H(), 1);
  __ Rshrn(v17.V4H(),   v1.V4S(), 16);
  __ Rshrn2(v17.V8H(),  v2.V4S(), 1);
  __ Rshrn(v18.V2S(),   v3.V2D(), 32);
  __ Rshrn2(v18.V4S(),  v3.V2D(), 1);

  END();

  RUN();
  ASSERT_EQUAL_128(0x0001000000000100, 0x7f01827f81ff0181, q16);
  ASSERT_EQUAL_128(0x0000000000000000, 0x8001ffffffff0001, q17);
  ASSERT_EQUAL_128(0x0000000100000000, 0x8000000080000000, q18);
  TEARDOWN();
}


TEST(neon_uqshrn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Uqshrn(v16.V8B(),   v0.V8H(), 8);
  __ Uqshrn2(v16.V16B(), v1.V8H(), 1);
  __ Uqshrn(v17.V4H(),   v1.V4S(), 16);
  __ Uqshrn2(v17.V8H(),  v2.V4S(), 1);
  __ Uqshrn(v18.V2S(),   v3.V2D(), 32);
  __ Uqshrn2(v18.V4S(),  v3.V2D(), 1);

  __ Uqshrn(b19, h0, 8);
  __ Uqshrn(h20, s1, 16);
  __ Uqshrn(s21, d3, 32);

  END();

  RUN();
  ASSERT_EQUAL_128(0xffffff00ff0000ff, 0x7f00817f80ff0180, q16);
  ASSERT_EQUAL_128(0xffffffff0000ffff, 0x8000ffffffff0001, q17);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0x800000007fffffff, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000080, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000001, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000007fffffff, q21);
  TEARDOWN();
}


TEST(neon_uqrshrn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Uqrshrn(v16.V8B(),   v0.V8H(), 8);
  __ Uqrshrn2(v16.V16B(), v1.V8H(), 1);
  __ Uqrshrn(v17.V4H(),   v1.V4S(), 16);
  __ Uqrshrn2(v17.V8H(),  v2.V4S(), 1);
  __ Uqrshrn(v18.V2S(),   v3.V2D(), 32);
  __ Uqrshrn2(v18.V4S(),  v3.V2D(), 1);

  __ Uqrshrn(b19, h0, 8);
  __ Uqrshrn(h20, s1, 16);
  __ Uqrshrn(s21, d3, 32);

  END();

  RUN();
  ASSERT_EQUAL_128(0xffffff00ff0001ff, 0x7f01827f81ff0181, q16);
  ASSERT_EQUAL_128(0xffffffff0000ffff, 0x8001ffffffff0001, q17);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0x8000000080000000, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000081, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000001, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000080000000, q21);
  TEARDOWN();
}


TEST(neon_sqshrn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Sqshrn(v16.V8B(),   v0.V8H(), 8);
  __ Sqshrn2(v16.V16B(), v1.V8H(), 1);
  __ Sqshrn(v17.V4H(),   v1.V4S(), 16);
  __ Sqshrn2(v17.V8H(),  v2.V4S(), 1);
  __ Sqshrn(v18.V2S(),   v3.V2D(), 32);
  __ Sqshrn2(v18.V4S(),  v3.V2D(), 1);

  __ Sqshrn(b19, h0, 8);
  __ Sqshrn(h20, s1, 16);
  __ Sqshrn(s21, d3, 32);

  END();

  RUN();
  ASSERT_EQUAL_128(0x8080ff00ff00007f, 0x7f00817f80ff0180, q16);
  ASSERT_EQUAL_128(0x8000ffff00007fff, 0x8000ffffffff0001, q17);
  ASSERT_EQUAL_128(0x800000007fffffff, 0x800000007fffffff, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000080, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000001, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000007fffffff, q21);
  TEARDOWN();
}


TEST(neon_sqrshrn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Sqrshrn(v16.V8B(),   v0.V8H(), 8);
  __ Sqrshrn2(v16.V16B(), v1.V8H(), 1);
  __ Sqrshrn(v17.V4H(),   v1.V4S(), 16);
  __ Sqrshrn2(v17.V8H(),  v2.V4S(), 1);
  __ Sqrshrn(v18.V2S(),   v3.V2D(), 32);
  __ Sqrshrn2(v18.V4S(),  v3.V2D(), 1);

  __ Sqrshrn(b19, h0, 8);
  __ Sqrshrn(h20, s1, 16);
  __ Sqrshrn(s21, d3, 32);

  END();

  RUN();
  ASSERT_EQUAL_128(0x808000000000017f, 0x7f01827f81ff0181, q16);
  ASSERT_EQUAL_128(0x8000000000007fff, 0x8001ffffffff0001, q17);
  ASSERT_EQUAL_128(0x800000007fffffff, 0x800000007fffffff, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000081, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000001, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000007fffffff, q21);
  TEARDOWN();
}


TEST(neon_sqshrun) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Sqshrun(v16.V8B(),   v0.V8H(), 8);
  __ Sqshrun2(v16.V16B(), v1.V8H(), 1);
  __ Sqshrun(v17.V4H(),   v1.V4S(), 16);
  __ Sqshrun2(v17.V8H(),  v2.V4S(), 1);
  __ Sqshrun(v18.V2S(),   v3.V2D(), 32);
  __ Sqshrun2(v18.V4S(),  v3.V2D(), 1);

  __ Sqshrun(b19, h0, 8);
  __ Sqshrun(h20, s1, 16);
  __ Sqshrun(s21, d3, 32);

  END();

  RUN();
  ASSERT_EQUAL_128(0x00000000000000ff, 0x7f00007f00000100, q16);
  ASSERT_EQUAL_128(0x000000000000ffff, 0x0000000000000001, q17);
  ASSERT_EQUAL_128(0x00000000ffffffff, 0x000000007fffffff, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000001, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000007fffffff, q21);
  TEARDOWN();
}


TEST(neon_sqrshrun) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Sqrshrun(v16.V8B(),   v0.V8H(), 8);
  __ Sqrshrun2(v16.V16B(), v1.V8H(), 1);
  __ Sqrshrun(v17.V4H(),   v1.V4S(), 16);
  __ Sqrshrun2(v17.V8H(),  v2.V4S(), 1);
  __ Sqrshrun(v18.V2S(),   v3.V2D(), 32);
  __ Sqrshrun2(v18.V4S(),  v3.V2D(), 1);

  __ Sqrshrun(b19, h0, 8);
  __ Sqrshrun(h20, s1, 16);
  __ Sqrshrun(s21, d3, 32);

  END();

  RUN();
  ASSERT_EQUAL_128(0x00000000000001ff, 0x7f01007f00000100, q16);
  ASSERT_EQUAL_128(0x000000000000ffff, 0x0000000000000001, q17);
  ASSERT_EQUAL_128(0x00000000ffffffff, 0x0000000080000000, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000001, q20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000080000000, q21);
  TEARDOWN();
}

TEST(neon_modimm_bic) {
  SETUP();

  START();

  __ Movi(v16.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v17.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v18.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v19.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v20.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v21.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v22.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v23.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v24.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v25.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v26.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v27.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);

  __ Bic(v16.V4H(), 0x00, 0);
  __ Bic(v17.V4H(), 0xff, 8);
  __ Bic(v18.V8H(), 0x00, 0);
  __ Bic(v19.V8H(), 0xff, 8);

  __ Bic(v20.V2S(), 0x00, 0);
  __ Bic(v21.V2S(), 0xff, 8);
  __ Bic(v22.V2S(), 0x00, 16);
  __ Bic(v23.V2S(), 0xff, 24);

  __ Bic(v24.V4S(), 0xff, 0);
  __ Bic(v25.V4S(), 0x00, 8);
  __ Bic(v26.V4S(), 0xff, 16);
  __ Bic(v27.V4S(), 0x00, 24);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0x5555ffff0000aaaa, q16);
  ASSERT_EQUAL_128(0x0, 0x005500ff000000aa, q17);
  ASSERT_EQUAL_128(0x00aaff55ff0055aa, 0x5555ffff0000aaaa, q18);
  ASSERT_EQUAL_128(0x00aa0055000000aa, 0x005500ff000000aa, q19);

  ASSERT_EQUAL_128(0x0, 0x5555ffff0000aaaa, q20);
  ASSERT_EQUAL_128(0x0, 0x555500ff000000aa, q21);
  ASSERT_EQUAL_128(0x0, 0x5555ffff0000aaaa, q22);
  ASSERT_EQUAL_128(0x0, 0x0055ffff0000aaaa, q23);

  ASSERT_EQUAL_128(0x00aaff00ff005500, 0x5555ff000000aa00, q24);
  ASSERT_EQUAL_128(0x00aaff55ff0055aa, 0x5555ffff0000aaaa, q25);
  ASSERT_EQUAL_128(0x0000ff55ff0055aa, 0x5500ffff0000aaaa, q26);
  ASSERT_EQUAL_128(0x00aaff55ff0055aa, 0x5555ffff0000aaaa, q27);

  TEARDOWN();
}


TEST(neon_modimm_movi_16bit_any) {
  SETUP();

  START();

  __ Movi(v0.V4H(), 0xabab);
  __ Movi(v1.V4H(), 0xab00);
  __ Movi(v2.V4H(), 0xabff);
  __ Movi(v3.V8H(), 0x00ab);
  __ Movi(v4.V8H(), 0xffab);
  __ Movi(v5.V8H(), 0xabcd);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0xabababababababab, q0);
  ASSERT_EQUAL_128(0x0, 0xab00ab00ab00ab00, q1);
  ASSERT_EQUAL_128(0x0, 0xabffabffabffabff, q2);
  ASSERT_EQUAL_128(0x00ab00ab00ab00ab, 0x00ab00ab00ab00ab, q3);
  ASSERT_EQUAL_128(0xffabffabffabffab, 0xffabffabffabffab, q4);
  ASSERT_EQUAL_128(0xabcdabcdabcdabcd, 0xabcdabcdabcdabcd, q5);

  TEARDOWN();
}


TEST(neon_modimm_movi_32bit_any) {
  SETUP();

  START();

  __ Movi(v0.V2S(), 0x000000ab);
  __ Movi(v1.V2S(), 0x0000ab00);
  __ Movi(v2.V4S(), 0x00ab0000);
  __ Movi(v3.V4S(), 0xab000000);

  __ Movi(v4.V2S(), 0xffffffab);
  __ Movi(v5.V2S(), 0xffffabff);
  __ Movi(v6.V4S(), 0xffabffff);
  __ Movi(v7.V4S(), 0xabffffff);

  __ Movi(v16.V2S(), 0x0000abff);
  __ Movi(v17.V2S(), 0x00abffff);
  __ Movi(v18.V4S(), 0xffab0000);
  __ Movi(v19.V4S(), 0xffffab00);

  __ Movi(v20.V4S(), 0xabababab);
  __ Movi(v21.V4S(), 0xabcdabcd);
  __ Movi(v22.V4S(), 0xabcdef01);
  __ Movi(v23.V4S(), 0x00ffff00);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0x000000ab000000ab, q0);
  ASSERT_EQUAL_128(0x0, 0x0000ab000000ab00, q1);
  ASSERT_EQUAL_128(0x00ab000000ab0000, 0x00ab000000ab0000, q2);
  ASSERT_EQUAL_128(0xab000000ab000000, 0xab000000ab000000, q3);

  ASSERT_EQUAL_128(0x0, 0xffffffabffffffab, q4);
  ASSERT_EQUAL_128(0x0, 0xffffabffffffabff, q5);
  ASSERT_EQUAL_128(0xffabffffffabffff, 0xffabffffffabffff, q6);
  ASSERT_EQUAL_128(0xabffffffabffffff, 0xabffffffabffffff, q7);

  ASSERT_EQUAL_128(0x0, 0x0000abff0000abff, q16);
  ASSERT_EQUAL_128(0x0, 0x00abffff00abffff, q17);
  ASSERT_EQUAL_128(0xffab0000ffab0000, 0xffab0000ffab0000, q18);
  ASSERT_EQUAL_128(0xffffab00ffffab00, 0xffffab00ffffab00, q19);

  ASSERT_EQUAL_128(0xabababababababab, 0xabababababababab, q20);
  ASSERT_EQUAL_128(0xabcdabcdabcdabcd, 0xabcdabcdabcdabcd, q21);
  ASSERT_EQUAL_128(0xabcdef01abcdef01, 0xabcdef01abcdef01, q22);
  ASSERT_EQUAL_128(0x00ffff0000ffff00, 0x00ffff0000ffff00, q23);
  TEARDOWN();
}


TEST(neon_modimm_movi_64bit_any) {
  SETUP();

  START();

  __ Movi(v0.V1D(), 0x00ffff0000ffffff);
  __ Movi(v1.V2D(), 0xabababababababab);
  __ Movi(v2.V2D(), 0xabcdabcdabcdabcd);
  __ Movi(v3.V2D(), 0xabcdef01abcdef01);
  __ Movi(v4.V1D(), 0xabcdef0123456789);
  __ Movi(v5.V2D(), 0xabcdef0123456789);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0x00ffff0000ffffff, q0);
  ASSERT_EQUAL_128(0xabababababababab, 0xabababababababab, q1);
  ASSERT_EQUAL_128(0xabcdabcdabcdabcd, 0xabcdabcdabcdabcd, q2);
  ASSERT_EQUAL_128(0xabcdef01abcdef01, 0xabcdef01abcdef01, q3);
  ASSERT_EQUAL_128(0x0, 0xabcdef0123456789, q4);
  ASSERT_EQUAL_128(0xabcdef0123456789, 0xabcdef0123456789, q5);

  TEARDOWN();
}


TEST(neon_modimm_movi) {
  SETUP();

  START();

  __ Movi(v0.V8B(),  0xaa);
  __ Movi(v1.V16B(), 0x55);

  __ Movi(d2,       0x00ffff0000ffffff);
  __ Movi(v3.V2D(), 0x00ffff0000ffffff);

  __ Movi(v16.V4H(), 0x00, LSL, 0);
  __ Movi(v17.V4H(), 0xff, LSL, 8);
  __ Movi(v18.V8H(), 0x00, LSL, 0);
  __ Movi(v19.V8H(), 0xff, LSL, 8);

  __ Movi(v20.V2S(), 0x00, LSL, 0);
  __ Movi(v21.V2S(), 0xff, LSL, 8);
  __ Movi(v22.V2S(), 0x00, LSL, 16);
  __ Movi(v23.V2S(), 0xff, LSL, 24);

  __ Movi(v24.V4S(), 0xff, LSL, 0);
  __ Movi(v25.V4S(), 0x00, LSL, 8);
  __ Movi(v26.V4S(), 0xff, LSL, 16);
  __ Movi(v27.V4S(), 0x00, LSL, 24);

  __ Movi(v28.V2S(), 0xaa, MSL, 8);
  __ Movi(v29.V2S(), 0x55, MSL, 16);
  __ Movi(v30.V4S(), 0xff, MSL, 8);
  __ Movi(v31.V4S(), 0x00, MSL, 16);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0xaaaaaaaaaaaaaaaa, q0);
  ASSERT_EQUAL_128(0x5555555555555555, 0x5555555555555555, q1);

  ASSERT_EQUAL_128(0x0, 0x00ffff0000ffffff, q2);
  ASSERT_EQUAL_128(0x00ffff0000ffffff, 0x00ffff0000ffffff, q3);

  ASSERT_EQUAL_128(0x0, 0x0000000000000000, q16);
  ASSERT_EQUAL_128(0x0, 0xff00ff00ff00ff00, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q18);
  ASSERT_EQUAL_128(0xff00ff00ff00ff00, 0xff00ff00ff00ff00, q19);

  ASSERT_EQUAL_128(0x0, 0x0000000000000000, q20);
  ASSERT_EQUAL_128(0x0, 0x0000ff000000ff00, q21);
  ASSERT_EQUAL_128(0x0, 0x0000000000000000, q22);
  ASSERT_EQUAL_128(0x0, 0xff000000ff000000, q23);

  ASSERT_EQUAL_128(0x000000ff000000ff, 0x000000ff000000ff, q24);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q25);
  ASSERT_EQUAL_128(0x00ff000000ff0000, 0x00ff000000ff0000, q26);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000000000, q27);

  ASSERT_EQUAL_128(0x0, 0x0000aaff0000aaff, q28);
  ASSERT_EQUAL_128(0x0, 0x0055ffff0055ffff, q29);
  ASSERT_EQUAL_128(0x0000ffff0000ffff, 0x0000ffff0000ffff, q30);
  ASSERT_EQUAL_128(0x0000ffff0000ffff, 0x0000ffff0000ffff, q31);

  TEARDOWN();
}


TEST(neon_modimm_mvni) {
  SETUP();

  START();

  __ Mvni(v16.V4H(), 0x00, LSL, 0);
  __ Mvni(v17.V4H(), 0xff, LSL, 8);
  __ Mvni(v18.V8H(), 0x00, LSL, 0);
  __ Mvni(v19.V8H(), 0xff, LSL, 8);

  __ Mvni(v20.V2S(), 0x00, LSL, 0);
  __ Mvni(v21.V2S(), 0xff, LSL, 8);
  __ Mvni(v22.V2S(), 0x00, LSL, 16);
  __ Mvni(v23.V2S(), 0xff, LSL, 24);

  __ Mvni(v24.V4S(), 0xff, LSL, 0);
  __ Mvni(v25.V4S(), 0x00, LSL, 8);
  __ Mvni(v26.V4S(), 0xff, LSL, 16);
  __ Mvni(v27.V4S(), 0x00, LSL, 24);

  __ Mvni(v28.V2S(), 0xaa, MSL, 8);
  __ Mvni(v29.V2S(), 0x55, MSL, 16);
  __ Mvni(v30.V4S(), 0xff, MSL, 8);
  __ Mvni(v31.V4S(), 0x00, MSL, 16);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0xffffffffffffffff, q16);
  ASSERT_EQUAL_128(0x0, 0x00ff00ff00ff00ff, q17);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0xffffffffffffffff, q18);
  ASSERT_EQUAL_128(0x00ff00ff00ff00ff, 0x00ff00ff00ff00ff, q19);

  ASSERT_EQUAL_128(0x0, 0xffffffffffffffff, q20);
  ASSERT_EQUAL_128(0x0, 0xffff00ffffff00ff, q21);
  ASSERT_EQUAL_128(0x0, 0xffffffffffffffff, q22);
  ASSERT_EQUAL_128(0x0, 0x00ffffff00ffffff, q23);

  ASSERT_EQUAL_128(0xffffff00ffffff00, 0xffffff00ffffff00, q24);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0xffffffffffffffff, q25);
  ASSERT_EQUAL_128(0xff00ffffff00ffff, 0xff00ffffff00ffff, q26);
  ASSERT_EQUAL_128(0xffffffffffffffff, 0xffffffffffffffff, q27);

  ASSERT_EQUAL_128(0x0, 0xffff5500ffff5500, q28);
  ASSERT_EQUAL_128(0x0, 0xffaa0000ffaa0000, q29);
  ASSERT_EQUAL_128(0xffff0000ffff0000, 0xffff0000ffff0000, q30);
  ASSERT_EQUAL_128(0xffff0000ffff0000, 0xffff0000ffff0000, q31);

  TEARDOWN();
}


TEST(neon_modimm_orr) {
  SETUP();

  START();

  __ Movi(v16.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v17.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v18.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v19.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v20.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v21.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v22.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v23.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v24.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v25.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v26.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);
  __ Movi(v27.V2D(), 0x00aaff55ff0055aa, 0x5555ffff0000aaaa);

  __ Orr(v16.V4H(), 0x00, 0);
  __ Orr(v17.V4H(), 0xff, 8);
  __ Orr(v18.V8H(), 0x00, 0);
  __ Orr(v19.V8H(), 0xff, 8);

  __ Orr(v20.V2S(), 0x00, 0);
  __ Orr(v21.V2S(), 0xff, 8);
  __ Orr(v22.V2S(), 0x00, 16);
  __ Orr(v23.V2S(), 0xff, 24);

  __ Orr(v24.V4S(), 0xff, 0);
  __ Orr(v25.V4S(), 0x00, 8);
  __ Orr(v26.V4S(), 0xff, 16);
  __ Orr(v27.V4S(), 0x00, 24);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0, 0x5555ffff0000aaaa, q16);
  ASSERT_EQUAL_128(0x0, 0xff55ffffff00ffaa, q17);
  ASSERT_EQUAL_128(0x00aaff55ff0055aa, 0x5555ffff0000aaaa, q18);
  ASSERT_EQUAL_128(0xffaaff55ff00ffaa, 0xff55ffffff00ffaa, q19);

  ASSERT_EQUAL_128(0x0, 0x5555ffff0000aaaa, q20);
  ASSERT_EQUAL_128(0x0, 0x5555ffff0000ffaa, q21);
  ASSERT_EQUAL_128(0x0, 0x5555ffff0000aaaa, q22);
  ASSERT_EQUAL_128(0x0, 0xff55ffffff00aaaa, q23);

  ASSERT_EQUAL_128(0x00aaffffff0055ff, 0x5555ffff0000aaff, q24);
  ASSERT_EQUAL_128(0x00aaff55ff0055aa, 0x5555ffff0000aaaa, q25);
  ASSERT_EQUAL_128(0x00ffff55ffff55aa, 0x55ffffff00ffaaaa, q26);
  ASSERT_EQUAL_128(0x00aaff55ff0055aa, 0x5555ffff0000aaaa, q27);

  TEARDOWN();
}


// TODO: add arbitrary values once load literal to Q registers is supported.
TEST(neon_modimm_fmov) {
  SETUP();

  // Immediates which can be encoded in the instructions.
  const float kOne = 1.0f;
  const float kPointFive = 0.5f;
  const double kMinusThirteen = -13.0;
  // Immediates which cannot be encoded in the instructions.
  const float kNonImmFP32 = 255.0f;
  const double kNonImmFP64 = 12.3456;

  START();
  __ Fmov(v11.V2S(), kOne);
  __ Fmov(v12.V4S(), kPointFive);
  __ Fmov(v22.V2D(), kMinusThirteen);
  __ Fmov(v13.V2S(), kNonImmFP32);
  __ Fmov(v14.V4S(), kNonImmFP32);
  __ Fmov(v23.V2D(), kNonImmFP64);
  __ Fmov(v1.V2S(), 0.0);
  __ Fmov(v2.V4S(), 0.0);
  __ Fmov(v3.V2D(), 0.0);
  __ Fmov(v4.V2S(), kFP32PositiveInfinity);
  __ Fmov(v5.V4S(), kFP32PositiveInfinity);
  __ Fmov(v6.V2D(), kFP64PositiveInfinity);
  END();

  RUN();

  const uint64_t kOne1S = float_to_rawbits(1.0);
  const uint64_t kOne2S = (kOne1S << 32) | kOne1S;
  const uint64_t kPointFive1S = float_to_rawbits(0.5);
  const uint64_t kPointFive2S = (kPointFive1S << 32) | kPointFive1S;
  const uint64_t kMinusThirteen1D = double_to_rawbits(-13.0);
  const uint64_t kNonImmFP321S = float_to_rawbits(kNonImmFP32);
  const uint64_t kNonImmFP322S = (kNonImmFP321S << 32) | kNonImmFP321S;
  const uint64_t kNonImmFP641D = double_to_rawbits(kNonImmFP64);
  const uint64_t kFP32Inf1S = float_to_rawbits(kFP32PositiveInfinity);
  const uint64_t kFP32Inf2S = (kFP32Inf1S << 32) | kFP32Inf1S;
  const uint64_t kFP64Inf1D = double_to_rawbits(kFP64PositiveInfinity);

  ASSERT_EQUAL_128(0x0, kOne2S, q11);
  ASSERT_EQUAL_128(kPointFive2S, kPointFive2S, q12);
  ASSERT_EQUAL_128(kMinusThirteen1D, kMinusThirteen1D, q22);
  ASSERT_EQUAL_128(0x0, kNonImmFP322S, q13);
  ASSERT_EQUAL_128(kNonImmFP322S, kNonImmFP322S, q14);
  ASSERT_EQUAL_128(kNonImmFP641D, kNonImmFP641D, q23);
  ASSERT_EQUAL_128(0x0, 0x0, q1);
  ASSERT_EQUAL_128(0x0, 0x0, q2);
  ASSERT_EQUAL_128(0x0, 0x0, q3);
  ASSERT_EQUAL_128(0x0, kFP32Inf2S, q4);
  ASSERT_EQUAL_128(kFP32Inf2S, kFP32Inf2S, q5);
  ASSERT_EQUAL_128(kFP64Inf1D, kFP64Inf1D, q6);

  TEARDOWN();
}


TEST(neon_perm) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0001020304050607, 0x08090a0b0c0d0e0f);
  __ Movi(v1.V2D(), 0x1011121314151617, 0x18191a1b1c1d1e1f);

  __ Trn1(v16.V16B(), v0.V16B(), v1.V16B());
  __ Trn2(v17.V16B(), v0.V16B(), v1.V16B());
  __ Zip1(v18.V16B(), v0.V16B(), v1.V16B());
  __ Zip2(v19.V16B(), v0.V16B(), v1.V16B());
  __ Uzp1(v20.V16B(), v0.V16B(), v1.V16B());
  __ Uzp2(v21.V16B(), v0.V16B(), v1.V16B());

  END();

  RUN();

  ASSERT_EQUAL_128(0x1101130315051707, 0x19091b0b1d0d1f0f, q16);
  ASSERT_EQUAL_128(0x1000120214041606, 0x18081a0a1c0c1e0e, q17);
  ASSERT_EQUAL_128(0x180819091a0a1b0b, 0x1c0c1d0d1e0e1f0f, q18);
  ASSERT_EQUAL_128(0x1000110112021303, 0x1404150516061707, q19);
  ASSERT_EQUAL_128(0x11131517191b1d1f, 0x01030507090b0d0f, q20);
  ASSERT_EQUAL_128(0x10121416181a1c1e, 0x00020406080a0c0e, q21);

  TEARDOWN();
}


TEST(neon_copy_dup_element) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0011223344556677, 0x8899aabbccddeeff);
  __ Movi(v1.V2D(), 0xffeddccbbaae9988, 0x7766554433221100);
  __ Movi(v2.V2D(), 0xffeddccbbaae9988, 0x0011223344556677);
  __ Movi(v3.V2D(), 0x7766554433221100, 0x8899aabbccddeeff);
  __ Movi(v4.V2D(), 0x7766554433221100, 0x0123456789abcdef);
  __ Movi(v5.V2D(), 0x0011223344556677, 0x0123456789abcdef);

  __ Dup(v16.V16B(), v0.B(), 0);
  __ Dup(v17.V8H(),  v1.H(), 7);
  __ Dup(v18.V4S(),  v1.S(), 3);
  __ Dup(v19.V2D(),  v0.D(), 0);

  __ Dup(v20.V8B(), v0.B(), 0);
  __ Dup(v21.V4H(), v1.H(), 7);
  __ Dup(v22.V2S(), v1.S(), 3);

  __ Dup(v23.B(), v0.B(), 0);
  __ Dup(v24.H(), v1.H(), 7);
  __ Dup(v25.S(), v1.S(), 3);
  __ Dup(v26.D(), v0.D(), 0);

  __ Dup(v2.V16B(), v2.B(), 0);
  __ Dup(v3.V8H(),  v3.H(), 7);
  __ Dup(v4.V4S(),  v4.S(), 0);
  __ Dup(v5.V2D(),  v5.D(), 1);

  END();

  RUN();

  ASSERT_EQUAL_128(0xffffffffffffffff, 0xffffffffffffffff, q16);
  ASSERT_EQUAL_128(0xffedffedffedffed, 0xffedffedffedffed, q17);
  ASSERT_EQUAL_128(0xffeddccbffeddccb, 0xffeddccbffeddccb, q18);
  ASSERT_EQUAL_128(0x8899aabbccddeeff, 0x8899aabbccddeeff, q19);

  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q20);
  ASSERT_EQUAL_128(0, 0xffedffedffedffed, q21);
  ASSERT_EQUAL_128(0, 0xffeddccbffeddccb, q22);

  ASSERT_EQUAL_128(0, 0x00000000000000ff, q23);
  ASSERT_EQUAL_128(0, 0x000000000000ffed, q24);
  ASSERT_EQUAL_128(0, 0x00000000ffeddccb, q25);
  ASSERT_EQUAL_128(0, 0x8899aabbccddeeff, q26);

  ASSERT_EQUAL_128(0x7777777777777777, 0x7777777777777777, q2);
  ASSERT_EQUAL_128(0x7766776677667766, 0x7766776677667766, q3);
  ASSERT_EQUAL_128(0x89abcdef89abcdef, 0x89abcdef89abcdef, q4);
  ASSERT_EQUAL_128(0x0011223344556677, 0x0011223344556677, q5);
  TEARDOWN();
}


TEST(neon_copy_dup_general) {
  SETUP();

  START();

  __ Mov(x0, 0x0011223344556677);

  __ Dup(v16.V16B(), w0);
  __ Dup(v17.V8H(),  w0);
  __ Dup(v18.V4S(),  w0);
  __ Dup(v19.V2D(),  x0);

  __ Dup(v20.V8B(), w0);
  __ Dup(v21.V4H(), w0);
  __ Dup(v22.V2S(), w0);

  __ Dup(v2.V16B(), wzr);
  __ Dup(v3.V8H(),  wzr);
  __ Dup(v4.V4S(),  wzr);
  __ Dup(v5.V2D(),  xzr);

  END();

  RUN();

  ASSERT_EQUAL_128(0x7777777777777777, 0x7777777777777777, q16);
  ASSERT_EQUAL_128(0x6677667766776677, 0x6677667766776677, q17);
  ASSERT_EQUAL_128(0x4455667744556677, 0x4455667744556677, q18);
  ASSERT_EQUAL_128(0x0011223344556677, 0x0011223344556677, q19);

  ASSERT_EQUAL_128(0, 0x7777777777777777, q20);
  ASSERT_EQUAL_128(0, 0x6677667766776677, q21);
  ASSERT_EQUAL_128(0, 0x4455667744556677, q22);

  ASSERT_EQUAL_128(0, 0, q2);
  ASSERT_EQUAL_128(0, 0, q3);
  ASSERT_EQUAL_128(0, 0, q4);
  ASSERT_EQUAL_128(0, 0, q5);
  TEARDOWN();
}


TEST(neon_copy_ins_element) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0x0011223344556677, 0x8899aabbccddeeff);
  __ Movi(v1.V2D(),  0xffeddccbbaae9988, 0x7766554433221100);
  __ Movi(v16.V2D(), 0x0123456789abcdef, 0xfedcba9876543210);
  __ Movi(v17.V2D(), 0xfedcba9876543210, 0x0123456789abcdef);
  __ Movi(v18.V2D(), 0x0011223344556677, 0x8899aabbccddeeff);
  __ Movi(v19.V2D(), 0x0011223344556677, 0x8899aabbccddeeff);

  __ Movi(v2.V2D(), 0, 0x0011223344556677);
  __ Movi(v3.V2D(), 0, 0x8899aabbccddeeff);
  __ Movi(v4.V2D(), 0, 0x0123456789abcdef);
  __ Movi(v5.V2D(), 0, 0x0123456789abcdef);

  __ Ins(v16.V16B(), 15, v0.V16B(), 0);
  __ Ins(v17.V8H(),  0,  v1.V8H(), 7);
  __ Ins(v18.V4S(),  3,  v1.V4S(), 0);
  __ Ins(v19.V2D(),  1,  v0.V2D(), 0);

  __ Ins(v2.V16B(), 2, v2.V16B(), 0);
  __ Ins(v3.V8H(),  0,  v3.V8H(), 7);
  __ Ins(v4.V4S(),  3,  v4.V4S(), 0);
  __ Ins(v5.V2D(),  0,  v5.V2D(), 1);

  END();

  RUN();

  ASSERT_EQUAL_128(0xff23456789abcdef, 0xfedcba9876543210, q16);
  ASSERT_EQUAL_128(0xfedcba9876543210, 0x0123456789abffed, q17);
  ASSERT_EQUAL_128(0x3322110044556677, 0x8899aabbccddeeff, q18);
  ASSERT_EQUAL_128(0x8899aabbccddeeff, 0x8899aabbccddeeff, q19);

  ASSERT_EQUAL_128(0, 0x0011223344776677, q2);
  ASSERT_EQUAL_128(0, 0x8899aabbccdd0000, q3);
  ASSERT_EQUAL_128(0x89abcdef00000000, 0x0123456789abcdef, q4);
  ASSERT_EQUAL_128(0, 0, q5);
  TEARDOWN();
}


TEST(neon_copy_mov_element) {
  SETUP();

  START();

  __ Movi(v0.V2D(),  0x0011223344556677, 0x8899aabbccddeeff);
  __ Movi(v1.V2D(),  0xffeddccbbaae9988, 0x7766554433221100);
  __ Movi(v16.V2D(), 0x0123456789abcdef, 0xfedcba9876543210);
  __ Movi(v17.V2D(), 0xfedcba9876543210, 0x0123456789abcdef);
  __ Movi(v18.V2D(), 0x0011223344556677, 0x8899aabbccddeeff);
  __ Movi(v19.V2D(), 0x0011223344556677, 0x8899aabbccddeeff);

  __ Movi(v2.V2D(), 0, 0x0011223344556677);
  __ Movi(v3.V2D(), 0, 0x8899aabbccddeeff);
  __ Movi(v4.V2D(), 0, 0x0123456789abcdef);
  __ Movi(v5.V2D(), 0, 0x0123456789abcdef);

  __ Mov(v16.V16B(), 15, v0.V16B(), 0);
  __ Mov(v17.V8H(),  0,  v1.V8H(), 7);
  __ Mov(v18.V4S(),  3,  v1.V4S(), 0);
  __ Mov(v19.V2D(),  1,  v0.V2D(), 0);

  __ Mov(v2.V16B(), 2, v2.V16B(), 0);
  __ Mov(v3.V8H(),  0,  v3.V8H(), 7);
  __ Mov(v4.V4S(),  3,  v4.V4S(), 0);
  __ Mov(v5.V2D(),  0,  v5.V2D(), 1);

  END();

  RUN();

  ASSERT_EQUAL_128(0xff23456789abcdef, 0xfedcba9876543210, q16);
  ASSERT_EQUAL_128(0xfedcba9876543210, 0x0123456789abffed, q17);
  ASSERT_EQUAL_128(0x3322110044556677, 0x8899aabbccddeeff, q18);
  ASSERT_EQUAL_128(0x8899aabbccddeeff, 0x8899aabbccddeeff, q19);

  ASSERT_EQUAL_128(0, 0x0011223344776677, q2);
  ASSERT_EQUAL_128(0, 0x8899aabbccdd0000, q3);
  ASSERT_EQUAL_128(0x89abcdef00000000, 0x0123456789abcdef, q4);
  ASSERT_EQUAL_128(0, 0, q5);
  TEARDOWN();
}


TEST(neon_copy_smov) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0123456789abcdef, 0xfedcba9876543210);

  __ Smov(w0, v0.B(),  7);
  __ Smov(w1, v0.B(), 15);

  __ Smov(w2, v0.H(),  0);
  __ Smov(w3, v0.H(),  3);

  __ Smov(x4, v0.B(),  7);
  __ Smov(x5, v0.B(),  15);

  __ Smov(x6, v0.H(),  0);
  __ Smov(x7, v0.H(),  3);

  __ Smov(x16, v0.S(),  0);
  __ Smov(x17, v0.S(),  1);

  END();

  RUN();

  ASSERT_EQUAL_32(0xfffffffe, w0);
  ASSERT_EQUAL_32(0x00000001, w1);
  ASSERT_EQUAL_32(0x00003210, w2);
  ASSERT_EQUAL_32(0xfffffedc, w3);
  ASSERT_EQUAL_64(0xfffffffffffffffe, x4);
  ASSERT_EQUAL_64(0x0000000000000001, x5);
  ASSERT_EQUAL_64(0x0000000000003210, x6);
  ASSERT_EQUAL_64(0xfffffffffffffedc, x7);
  ASSERT_EQUAL_64(0x0000000076543210, x16);
  ASSERT_EQUAL_64(0xfffffffffedcba98, x17);

  TEARDOWN();
}


TEST(neon_copy_umov_mov) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0123456789abcdef, 0xfedcba9876543210);

  __ Umov(w0, v0.B(), 15);
  __ Umov(w1, v0.H(),  0);
  __ Umov(w2, v0.S(),  3);
  __ Umov(x3, v0.D(),  1);

  __ Mov(w4, v0.S(),  3);
  __ Mov(x5, v0.D(),  1);

  END();

  RUN();

  ASSERT_EQUAL_32(0x00000001, w0);
  ASSERT_EQUAL_32(0x00003210, w1);
  ASSERT_EQUAL_32(0x01234567, w2);
  ASSERT_EQUAL_64(0x0123456789abcdef, x3);
  ASSERT_EQUAL_32(0x01234567, w4);
  ASSERT_EQUAL_64(0x0123456789abcdef, x5);

  TEARDOWN();
}


TEST(neon_copy_ins_general) {
  SETUP();

  START();

  __ Mov(x0, 0x0011223344556677);
  __ Movi(v16.V2D(), 0x0123456789abcdef, 0xfedcba9876543210);
  __ Movi(v17.V2D(), 0xfedcba9876543210, 0x0123456789abcdef);
  __ Movi(v18.V2D(), 0x0011223344556677, 0x8899aabbccddeeff);
  __ Movi(v19.V2D(), 0x0011223344556677, 0x8899aabbccddeeff);

  __ Movi(v2.V2D(), 0, 0x0011223344556677);
  __ Movi(v3.V2D(), 0, 0x8899aabbccddeeff);
  __ Movi(v4.V2D(), 0, 0x0123456789abcdef);
  __ Movi(v5.V2D(), 0, 0x0123456789abcdef);

  __ Ins(v16.V16B(), 15, w0);
  __ Ins(v17.V8H(),  0,  w0);
  __ Ins(v18.V4S(),  3,  w0);
  __ Ins(v19.V2D(),  0,  x0);

  __ Ins(v2.V16B(), 2, w0);
  __ Ins(v3.V8H(),  0, w0);
  __ Ins(v4.V4S(),  3, w0);
  __ Ins(v5.V2D(),  1, x0);

  END();

  RUN();

  ASSERT_EQUAL_128(0x7723456789abcdef, 0xfedcba9876543210, q16);
  ASSERT_EQUAL_128(0xfedcba9876543210, 0x0123456789ab6677, q17);
  ASSERT_EQUAL_128(0x4455667744556677, 0x8899aabbccddeeff, q18);
  ASSERT_EQUAL_128(0x0011223344556677, 0x0011223344556677, q19);

  ASSERT_EQUAL_128(0, 0x0011223344776677, q2);
  ASSERT_EQUAL_128(0, 0x8899aabbccdd6677, q3);
  ASSERT_EQUAL_128(0x4455667700000000, 0x0123456789abcdef, q4);
  ASSERT_EQUAL_128(0x0011223344556677, 0x0123456789abcdef, q5);
  TEARDOWN();
}


TEST(neon_extract_ext) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0011223344556677, 0x8899aabbccddeeff);
  __ Movi(v1.V2D(), 0xffeddccbbaae9988, 0x7766554433221100);

  __ Movi(v2.V2D(), 0, 0x0011223344556677);
  __ Movi(v3.V2D(), 0, 0x8899aabbccddeeff);

  __ Ext(v16.V16B(), v0.V16B(), v1.V16B(), 0);
  __ Ext(v17.V16B(), v0.V16B(), v1.V16B(), 15);
  __ Ext(v1.V16B(), v0.V16B(), v1.V16B(), 8);  // Dest is same as one Src
  __ Ext(v0.V16B(), v0.V16B(), v0.V16B(), 8);  // All reg are the same

  __ Ext(v18.V8B(), v2.V8B(), v3.V8B(), 0);
  __ Ext(v19.V8B(), v2.V8B(), v3.V8B(), 7);
  __ Ext(v2.V8B(), v2.V8B(), v3.V8B(), 4);     // Dest is same as one Src
  __ Ext(v3.V8B(), v3.V8B(), v3.V8B(), 4);     // All reg are the same

  END();

  RUN();

  ASSERT_EQUAL_128(0x0011223344556677, 0x8899aabbccddeeff, q16);
  ASSERT_EQUAL_128(0xeddccbbaae998877, 0x6655443322110000, q17);
  ASSERT_EQUAL_128(0x7766554433221100, 0x0011223344556677, q1);
  ASSERT_EQUAL_128(0x8899aabbccddeeff, 0x0011223344556677, q0);

  ASSERT_EQUAL_128(0, 0x0011223344556677, q18);
  ASSERT_EQUAL_128(0, 0x99aabbccddeeff00, q19);
  ASSERT_EQUAL_128(0, 0xccddeeff00112233, q2);
  ASSERT_EQUAL_128(0, 0xccddeeff8899aabb, q3);
  TEARDOWN();
}


TEST(neon_3different_uaddl) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0000000000000000, 0x0000000000000000);
  __ Movi(v1.V2D(), 0, 0x00010280810e0fff);
  __ Movi(v2.V2D(), 0, 0x0101010101010101);

  __ Movi(v3.V2D(), 0x0000000000000000, 0x0000000000000000);
  __ Movi(v4.V2D(), 0x0000000000000000, 0x0000000000000000);
  __ Movi(v5.V2D(), 0, 0x0000000180008001);
  __ Movi(v6.V2D(), 0, 0x000e000ff000ffff);
  __ Movi(v7.V2D(), 0, 0x0001000100010001);

  __ Movi(v16.V2D(), 0x0000000000000000, 0x0000000000000000);
  __ Movi(v17.V2D(), 0x0000000000000000, 0x0000000000000000);
  __ Movi(v18.V2D(), 0, 0x0000000000000001);
  __ Movi(v19.V2D(), 0, 0x80000001ffffffff);
  __ Movi(v20.V2D(), 0, 0x0000000100000001);

  __ Uaddl(v0.V8H(), v1.V8B(), v2.V8B());

  __ Uaddl(v3.V4S(), v5.V4H(), v7.V4H());
  __ Uaddl(v4.V4S(), v6.V4H(), v7.V4H());

  __ Uaddl(v16.V2D(), v18.V2S(), v20.V2S());
  __ Uaddl(v17.V2D(), v19.V2S(), v20.V2S());


  END();

  RUN();

  ASSERT_EQUAL_128(0x0001000200030081, 0x0082000f00100100, q0);
  ASSERT_EQUAL_128(0x0000000100000002, 0x0000800100008002, q3);
  ASSERT_EQUAL_128(0x0000000f00000010, 0x0000f00100010000, q4);
  ASSERT_EQUAL_128(0x0000000000000001, 0x0000000000000002, q16);
  ASSERT_EQUAL_128(0x0000000080000002, 0x0000000100000000, q17);
  TEARDOWN();
}


TEST(neon_3different_addhn_subhn) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Addhn(v16.V8B(),   v0.V8H(), v1.V8H());
  __ Addhn2(v16.V16B(), v2.V8H(), v3.V8H());
  __ Raddhn(v17.V8B(),   v0.V8H(), v1.V8H());
  __ Raddhn2(v17.V16B(), v2.V8H(), v3.V8H());
  __ Subhn(v18.V8B(),   v0.V8H(), v1.V8H());
  __ Subhn2(v18.V16B(), v2.V8H(), v3.V8H());
  __ Rsubhn(v19.V8B(),   v0.V8H(), v1.V8H());
  __ Rsubhn2(v19.V16B(), v2.V8H(), v3.V8H());

  END();

  RUN();

  ASSERT_EQUAL_128(0x0000ff007fff7fff, 0xff81817f80ff0100, q16);
  ASSERT_EQUAL_128(0x0000000080008000, 0xff81817f81ff0201, q17);
  ASSERT_EQUAL_128(0x0000ffff80008000, 0xff80817f80ff0100, q18);
  ASSERT_EQUAL_128(0x0000000080008000, 0xff81827f81ff0101, q19);
  TEARDOWN();
}

TEST(neon_d_only_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaaaaaaaaaa, 0xf0000000f000f0f0);
  __ Movi(v1.V2D(), 0x5555555555555555, 0x7fffffff7fff7f7f);
  __ Movi(v2.V2D(), 0xaaaaaaaaaaaaaaaa, 0x1000000010001010);
  __ Movi(v3.V2D(), 0xffffffffffffffff, 2);
  __ Movi(v4.V2D(), 0xffffffffffffffff, -2);

  __ Add(d16, d0, d0);
  __ Add(d17, d1, d1);
  __ Add(d18, d2, d2);
  __ Sub(d19, d0, d0);
  __ Sub(d20, d0, d1);
  __ Sub(d21, d1, d0);
  __ Ushl(d22, d0, d3);
  __ Ushl(d23, d0, d4);
  __ Sshl(d24, d0, d3);
  __ Sshl(d25, d0, d4);
  __ Ushr(d26, d0, 1);
  __ Sshr(d27, d0, 3);
  __ Shl(d28, d0, 0);
  __ Shl(d29, d0, 16);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0xe0000001e001e1e0, q16);
  ASSERT_EQUAL_128(0, 0xfffffffefffefefe, q17);
  ASSERT_EQUAL_128(0, 0x2000000020002020, q18);
  ASSERT_EQUAL_128(0, 0, q19);
  ASSERT_EQUAL_128(0, 0x7000000170017171, q20);
  ASSERT_EQUAL_128(0, 0x8ffffffe8ffe8e8f, q21);
  ASSERT_EQUAL_128(0, 0xc0000003c003c3c0, q22);
  ASSERT_EQUAL_128(0, 0x3c0000003c003c3c, q23);
  ASSERT_EQUAL_128(0, 0xc0000003c003c3c0, q24);
  ASSERT_EQUAL_128(0, 0xfc0000003c003c3c, q25);
  ASSERT_EQUAL_128(0, 0x7800000078007878, q26);
  ASSERT_EQUAL_128(0, 0xfe0000001e001e1e, q27);
  ASSERT_EQUAL_128(0, 0xf0000000f000f0f0, q28);
  ASSERT_EQUAL_128(0, 0x0000f000f0f00000, q29);

  TEARDOWN();
}


TEST(neon_sqshl_imm_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0, 0x7f);
  __ Movi(v1.V2D(), 0x0, 0x80);
  __ Movi(v2.V2D(), 0x0, 0x01);
  __ Sqshl(b16, b0, 1);
  __ Sqshl(b17, b1, 1);
  __ Sqshl(b18, b2, 1);

  __ Movi(v0.V2D(), 0x0, 0x7fff);
  __ Movi(v1.V2D(), 0x0, 0x8000);
  __ Movi(v2.V2D(), 0x0, 0x0001);
  __ Sqshl(h19, h0, 1);
  __ Sqshl(h20, h1, 1);
  __ Sqshl(h21, h2, 1);

  __ Movi(v0.V2D(), 0x0, 0x7fffffff);
  __ Movi(v1.V2D(), 0x0, 0x80000000);
  __ Movi(v2.V2D(), 0x0, 0x00000001);
  __ Sqshl(s22, s0, 1);
  __ Sqshl(s23, s1, 1);
  __ Sqshl(s24, s2, 1);

  __ Movi(v0.V2D(), 0x0, 0x7fffffffffffffff);
  __ Movi(v1.V2D(), 0x0, 0x8000000000000000);
  __ Movi(v2.V2D(), 0x0, 0x0000000000000001);
  __ Sqshl(d25, d0, 1);
  __ Sqshl(d26, d1, 1);
  __ Sqshl(d27, d2, 1);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x7f, q16);
  ASSERT_EQUAL_128(0, 0x80, q17);
  ASSERT_EQUAL_128(0, 0x02, q18);

  ASSERT_EQUAL_128(0, 0x7fff, q19);
  ASSERT_EQUAL_128(0, 0x8000, q20);
  ASSERT_EQUAL_128(0, 0x0002, q21);

  ASSERT_EQUAL_128(0, 0x7fffffff, q22);
  ASSERT_EQUAL_128(0, 0x80000000, q23);
  ASSERT_EQUAL_128(0, 0x00000002, q24);

  ASSERT_EQUAL_128(0, 0x7fffffffffffffff, q25);
  ASSERT_EQUAL_128(0, 0x8000000000000000, q26);
  ASSERT_EQUAL_128(0, 0x0000000000000002, q27);

  TEARDOWN();
}


TEST(neon_uqshl_imm_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0, 0x7f);
  __ Movi(v1.V2D(), 0x0, 0x80);
  __ Movi(v2.V2D(), 0x0, 0x01);
  __ Uqshl(b16, b0, 1);
  __ Uqshl(b17, b1, 1);
  __ Uqshl(b18, b2, 1);

  __ Movi(v0.V2D(), 0x0, 0x7fff);
  __ Movi(v1.V2D(), 0x0, 0x8000);
  __ Movi(v2.V2D(), 0x0, 0x0001);
  __ Uqshl(h19, h0, 1);
  __ Uqshl(h20, h1, 1);
  __ Uqshl(h21, h2, 1);

  __ Movi(v0.V2D(), 0x0, 0x7fffffff);
  __ Movi(v1.V2D(), 0x0, 0x80000000);
  __ Movi(v2.V2D(), 0x0, 0x00000001);
  __ Uqshl(s22, s0, 1);
  __ Uqshl(s23, s1, 1);
  __ Uqshl(s24, s2, 1);

  __ Movi(v0.V2D(), 0x0, 0x7fffffffffffffff);
  __ Movi(v1.V2D(), 0x0, 0x8000000000000000);
  __ Movi(v2.V2D(), 0x0, 0x0000000000000001);
  __ Uqshl(d25, d0, 1);
  __ Uqshl(d26, d1, 1);
  __ Uqshl(d27, d2, 1);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0xfe, q16);
  ASSERT_EQUAL_128(0, 0xff, q17);
  ASSERT_EQUAL_128(0, 0x02, q18);

  ASSERT_EQUAL_128(0, 0xfffe, q19);
  ASSERT_EQUAL_128(0, 0xffff, q20);
  ASSERT_EQUAL_128(0, 0x0002, q21);

  ASSERT_EQUAL_128(0, 0xfffffffe, q22);
  ASSERT_EQUAL_128(0, 0xffffffff, q23);
  ASSERT_EQUAL_128(0, 0x00000002, q24);

  ASSERT_EQUAL_128(0, 0xfffffffffffffffe, q25);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q26);
  ASSERT_EQUAL_128(0, 0x0000000000000002, q27);

  TEARDOWN();
}


TEST(neon_sqshlu_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x0, 0x7f);
  __ Movi(v1.V2D(), 0x0, 0x80);
  __ Movi(v2.V2D(), 0x0, 0x01);
  __ Sqshlu(b16, b0, 2);
  __ Sqshlu(b17, b1, 2);
  __ Sqshlu(b18, b2, 2);

  __ Movi(v0.V2D(), 0x0, 0x7fff);
  __ Movi(v1.V2D(), 0x0, 0x8000);
  __ Movi(v2.V2D(), 0x0, 0x0001);
  __ Sqshlu(h19, h0, 2);
  __ Sqshlu(h20, h1, 2);
  __ Sqshlu(h21, h2, 2);

  __ Movi(v0.V2D(), 0x0, 0x7fffffff);
  __ Movi(v1.V2D(), 0x0, 0x80000000);
  __ Movi(v2.V2D(), 0x0, 0x00000001);
  __ Sqshlu(s22, s0, 2);
  __ Sqshlu(s23, s1, 2);
  __ Sqshlu(s24, s2, 2);

  __ Movi(v0.V2D(), 0x0, 0x7fffffffffffffff);
  __ Movi(v1.V2D(), 0x0, 0x8000000000000000);
  __ Movi(v2.V2D(), 0x0, 0x0000000000000001);
  __ Sqshlu(d25, d0, 2);
  __ Sqshlu(d26, d1, 2);
  __ Sqshlu(d27, d2, 2);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0xff, q16);
  ASSERT_EQUAL_128(0, 0x00, q17);
  ASSERT_EQUAL_128(0, 0x04, q18);

  ASSERT_EQUAL_128(0, 0xffff, q19);
  ASSERT_EQUAL_128(0, 0x0000, q20);
  ASSERT_EQUAL_128(0, 0x0004, q21);

  ASSERT_EQUAL_128(0, 0xffffffff, q22);
  ASSERT_EQUAL_128(0, 0x00000000, q23);
  ASSERT_EQUAL_128(0, 0x00000004, q24);

  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q25);
  ASSERT_EQUAL_128(0, 0x0000000000000000, q26);
  ASSERT_EQUAL_128(0, 0x0000000000000004, q27);

  TEARDOWN();
}


TEST(neon_sshll) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);

  __ Sshll(v16.V8H(), v0.V8B(),  4);
  __ Sshll2(v17.V8H(), v0.V16B(), 4);

  __ Sshll(v18.V4S(),  v1.V4H(), 8);
  __ Sshll2(v19.V4S(),  v1.V8H(), 8);

  __ Sshll(v20.V2D(),  v2.V2S(), 16);
  __ Sshll2(v21.V2D(),  v2.V4S(), 16);

  END();

  RUN();

  ASSERT_EQUAL_128(0xf800f810fff00000, 0x001007f0f800f810, q16);
  ASSERT_EQUAL_128(0x07f000100000fff0, 0xf810f80007f00010, q17);
  ASSERT_EQUAL_128(0xffffff0000000000, 0x00000100007fff00, q18);
  ASSERT_EQUAL_128(0xff800000ff800100, 0xffffff0000000000, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00007fffffff0000, q20);
  ASSERT_EQUAL_128(0xffff800000000000, 0xffffffffffff0000, q21);
  TEARDOWN();
}

TEST(neon_shll) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);

  __ Shll(v16.V8H(), v0.V8B(),  8);
  __ Shll2(v17.V8H(), v0.V16B(), 8);

  __ Shll(v18.V4S(),  v1.V4H(), 16);
  __ Shll2(v19.V4S(),  v1.V8H(), 16);

  __ Shll(v20.V2D(),  v2.V2S(), 32);
  __ Shll2(v21.V2D(),  v2.V4S(), 32);

  END();

  RUN();

  ASSERT_EQUAL_128(0x80008100ff000000, 0x01007f0080008100, q16);
  ASSERT_EQUAL_128(0x7f0001000000ff00, 0x810080007f000100, q17);
  ASSERT_EQUAL_128(0xffff000000000000, 0x000100007fff0000, q18);
  ASSERT_EQUAL_128(0x8000000080010000, 0xffff000000000000, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x7fffffff00000000, q20);
  ASSERT_EQUAL_128(0x8000000000000000, 0xffffffff00000000, q21);
  TEARDOWN();
}

TEST(neon_ushll) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);

  __ Ushll(v16.V8H(), v0.V8B(),  4);
  __ Ushll2(v17.V8H(), v0.V16B(), 4);

  __ Ushll(v18.V4S(),  v1.V4H(), 8);
  __ Ushll2(v19.V4S(),  v1.V8H(), 8);

  __ Ushll(v20.V2D(),  v2.V2S(), 16);
  __ Ushll2(v21.V2D(),  v2.V4S(), 16);

  END();

  RUN();

  ASSERT_EQUAL_128(0x080008100ff00000, 0x001007f008000810, q16);
  ASSERT_EQUAL_128(0x07f0001000000ff0, 0x0810080007f00010, q17);
  ASSERT_EQUAL_128(0x00ffff0000000000, 0x00000100007fff00, q18);
  ASSERT_EQUAL_128(0x0080000000800100, 0x00ffff0000000000, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00007fffffff0000, q20);
  ASSERT_EQUAL_128(0x0000800000000000, 0x0000ffffffff0000, q21);
  TEARDOWN();
}


TEST(neon_sxtl) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);

  __ Sxtl(v16.V8H(), v0.V8B());
  __ Sxtl2(v17.V8H(), v0.V16B());

  __ Sxtl(v18.V4S(),  v1.V4H());
  __ Sxtl2(v19.V4S(),  v1.V8H());

  __ Sxtl(v20.V2D(),  v2.V2S());
  __ Sxtl2(v21.V2D(),  v2.V4S());

  END();

  RUN();

  ASSERT_EQUAL_128(0xff80ff81ffff0000, 0x0001007fff80ff81, q16);
  ASSERT_EQUAL_128(0x007f00010000ffff, 0xff81ff80007f0001, q17);
  ASSERT_EQUAL_128(0xffffffff00000000, 0x0000000100007fff, q18);
  ASSERT_EQUAL_128(0xffff8000ffff8001, 0xffffffff00000000, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000007fffffff, q20);
  ASSERT_EQUAL_128(0xffffffff80000000, 0xffffffffffffffff, q21);
  TEARDOWN();
}


TEST(neon_uxtl) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);

  __ Uxtl(v16.V8H(), v0.V8B());
  __ Uxtl2(v17.V8H(), v0.V16B());

  __ Uxtl(v18.V4S(),  v1.V4H());
  __ Uxtl2(v19.V4S(),  v1.V8H());

  __ Uxtl(v20.V2D(),  v2.V2S());
  __ Uxtl2(v21.V2D(),  v2.V4S());

  END();

  RUN();

  ASSERT_EQUAL_128(0x0080008100ff0000, 0x0001007f00800081, q16);
  ASSERT_EQUAL_128(0x007f0001000000ff, 0x00810080007f0001, q17);
  ASSERT_EQUAL_128(0x0000ffff00000000, 0x0000000100007fff, q18);
  ASSERT_EQUAL_128(0x0000800000008001, 0x0000ffff00000000, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x000000007fffffff, q20);
  ASSERT_EQUAL_128(0x0000000080000000, 0x00000000ffffffff, q21);
  TEARDOWN();
}


TEST(neon_ssra) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Mov(v16.V2D(),   v0.V2D());
  __ Mov(v17.V2D(),   v0.V2D());
  __ Mov(v18.V2D(),   v1.V2D());
  __ Mov(v19.V2D(),   v1.V2D());
  __ Mov(v20.V2D(),   v2.V2D());
  __ Mov(v21.V2D(),   v2.V2D());
  __ Mov(v22.V2D(),   v3.V2D());
  __ Mov(v23.V2D(),   v4.V2D());
  __ Mov(v24.V2D(),   v3.V2D());
  __ Mov(v25.V2D(),   v4.V2D());

  __ Ssra(v16.V8B(),  v0.V8B(),  4);
  __ Ssra(v17.V16B(), v0.V16B(), 4);

  __ Ssra(v18.V4H(),  v1.V4H(), 8);
  __ Ssra(v19.V8H(),  v1.V8H(), 8);

  __ Ssra(v20.V2S(),  v2.V2S(), 16);
  __ Ssra(v21.V4S(),  v2.V4S(), 16);

  __ Ssra(v22.V2D(),  v3.V2D(), 32);
  __ Ssra(v23.V2D(),  v4.V2D(), 32);

  __ Ssra(d24,  d3, 48);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x7879fe0001867879, q16);
  ASSERT_EQUAL_128(0x860100fe79788601, 0x7879fe0001867879, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0xfffe00000001807e, q18);
  ASSERT_EQUAL_128(0x7f807f81fffe0000, 0xfffe00000001807e, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000080007ffe, q20);
  ASSERT_EQUAL_128(0x7fff8000fffffffe, 0x0000000080007ffe, q21);
  ASSERT_EQUAL_128(0x7fffffff80000001, 0x800000007ffffffe, q22);
  ASSERT_EQUAL_128(0x7fffffff80000000, 0x0000000000000000, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0x8000000000007ffe, q24);
  TEARDOWN();
}

TEST(neon_srsra) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Mov(v16.V2D(),   v0.V2D());
  __ Mov(v17.V2D(),   v0.V2D());
  __ Mov(v18.V2D(),   v1.V2D());
  __ Mov(v19.V2D(),   v1.V2D());
  __ Mov(v20.V2D(),   v2.V2D());
  __ Mov(v21.V2D(),   v2.V2D());
  __ Mov(v22.V2D(),   v3.V2D());
  __ Mov(v23.V2D(),   v4.V2D());
  __ Mov(v24.V2D(),   v3.V2D());
  __ Mov(v25.V2D(),   v4.V2D());

  __ Srsra(v16.V8B(),  v0.V8B(),  4);
  __ Srsra(v17.V16B(), v0.V16B(), 4);

  __ Srsra(v18.V4H(),  v1.V4H(), 8);
  __ Srsra(v19.V8H(),  v1.V8H(), 8);

  __ Srsra(v20.V2S(),  v2.V2S(), 16);
  __ Srsra(v21.V4S(),  v2.V4S(), 16);

  __ Srsra(v22.V2D(),  v3.V2D(), 32);
  __ Srsra(v23.V2D(),  v4.V2D(), 32);

  __ Srsra(d24,  d3, 48);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x7879ff0001877879, q16);
  ASSERT_EQUAL_128(0x870100ff79788701, 0x7879ff0001877879, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0xffff00000001807f, q18);
  ASSERT_EQUAL_128(0x7f807f81ffff0000, 0xffff00000001807f, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000080007fff, q20);
  ASSERT_EQUAL_128(0x7fff8000ffffffff, 0x0000000080007fff, q21);
  ASSERT_EQUAL_128(0x7fffffff80000001, 0x800000007fffffff, q22);
  ASSERT_EQUAL_128(0x7fffffff80000000, 0x0000000000000000, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0x8000000000007fff, q24);

  TEARDOWN();
}

TEST(neon_usra) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Mov(v16.V2D(),   v0.V2D());
  __ Mov(v17.V2D(),   v0.V2D());
  __ Mov(v18.V2D(),   v1.V2D());
  __ Mov(v19.V2D(),   v1.V2D());
  __ Mov(v20.V2D(),   v2.V2D());
  __ Mov(v21.V2D(),   v2.V2D());
  __ Mov(v22.V2D(),   v3.V2D());
  __ Mov(v23.V2D(),   v4.V2D());
  __ Mov(v24.V2D(),   v3.V2D());
  __ Mov(v25.V2D(),   v4.V2D());

  __ Usra(v16.V8B(),  v0.V8B(),  4);
  __ Usra(v17.V16B(), v0.V16B(), 4);

  __ Usra(v18.V4H(),  v1.V4H(), 8);
  __ Usra(v19.V8H(),  v1.V8H(), 8);

  __ Usra(v20.V2S(),  v2.V2S(), 16);
  __ Usra(v21.V4S(),  v2.V4S(), 16);

  __ Usra(v22.V2D(),  v3.V2D(), 32);
  __ Usra(v23.V2D(),  v4.V2D(), 32);

  __ Usra(d24,  d3, 48);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x88890e0001868889, q16);
  ASSERT_EQUAL_128(0x8601000e89888601, 0x88890e0001868889, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00fe00000001807e, q18);
  ASSERT_EQUAL_128(0x8080808100fe0000, 0x00fe00000001807e, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000080007ffe, q20);
  ASSERT_EQUAL_128(0x800080000000fffe, 0x0000000080007ffe, q21);
  ASSERT_EQUAL_128(0x8000000080000001, 0x800000007ffffffe, q22);
  ASSERT_EQUAL_128(0x8000000080000000, 0x0000000000000000, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0x8000000000007ffe, q24);

  TEARDOWN();
}

TEST(neon_ursra) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0x7f0100ff81807f01, 0x8081ff00017f8081);
  __ Movi(v1.V2D(), 0x80008001ffff0000, 0xffff000000017fff);
  __ Movi(v2.V2D(), 0x80000000ffffffff, 0x000000007fffffff);
  __ Movi(v3.V2D(), 0x8000000000000001, 0x7fffffffffffffff);
  __ Movi(v4.V2D(), 0x8000000000000000, 0x0000000000000000);

  __ Mov(v16.V2D(),   v0.V2D());
  __ Mov(v17.V2D(),   v0.V2D());
  __ Mov(v18.V2D(),   v1.V2D());
  __ Mov(v19.V2D(),   v1.V2D());
  __ Mov(v20.V2D(),   v2.V2D());
  __ Mov(v21.V2D(),   v2.V2D());
  __ Mov(v22.V2D(),   v3.V2D());
  __ Mov(v23.V2D(),   v4.V2D());
  __ Mov(v24.V2D(),   v3.V2D());
  __ Mov(v25.V2D(),   v4.V2D());

  __ Ursra(v16.V8B(),  v0.V8B(),  4);
  __ Ursra(v17.V16B(), v0.V16B(), 4);

  __ Ursra(v18.V4H(),  v1.V4H(), 8);
  __ Ursra(v19.V8H(),  v1.V8H(), 8);

  __ Ursra(v20.V2S(),  v2.V2S(), 16);
  __ Ursra(v21.V4S(),  v2.V4S(), 16);

  __ Ursra(v22.V2D(),  v3.V2D(), 32);
  __ Ursra(v23.V2D(),  v4.V2D(), 32);

  __ Ursra(d24,  d3, 48);

  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x88890f0001878889, q16);
  ASSERT_EQUAL_128(0x8701000f89888701, 0x88890f0001878889, q17);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00ff00000001807f, q18);
  ASSERT_EQUAL_128(0x8080808100ff0000, 0x00ff00000001807f, q19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0000000080007fff, q20);
  ASSERT_EQUAL_128(0x800080000000ffff, 0x0000000080007fff, q21);
  ASSERT_EQUAL_128(0x8000000080000001, 0x800000007fffffff, q22);
  ASSERT_EQUAL_128(0x8000000080000000, 0x0000000000000000, q23);
  ASSERT_EQUAL_128(0x0000000000000000, 0x8000000000007fff, q24);
  TEARDOWN();
}


TEST(neon_uqshl_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaaaaaaaaaa, 0xf0000000f000f0f0);
  __ Movi(v1.V2D(), 0x5555555555555555, 0x7fffffff7fff7f7f);
  __ Movi(v2.V2D(), 0xaaaaaaaaaaaaaaaa, 0x0000000000000001);
  __ Movi(v3.V2D(), 0xaaaaaaaaaaaaaaaa, 0xffffffffffffffff);

  __ Uqshl(b16, b0, b2);
  __ Uqshl(b17, b0, b3);
  __ Uqshl(b18, b1, b2);
  __ Uqshl(b19, b1, b3);
  __ Uqshl(h20, h0, h2);
  __ Uqshl(h21, h0, h3);
  __ Uqshl(h22, h1, h2);
  __ Uqshl(h23, h1, h3);
  __ Uqshl(s24, s0, s2);
  __ Uqshl(s25, s0, s3);
  __ Uqshl(s26, s1, s2);
  __ Uqshl(s27, s1, s3);
  __ Uqshl(d28, d0, d2);
  __ Uqshl(d29, d0, d3);
  __ Uqshl(d30, d1, d2);
  __ Uqshl(d31, d1, d3);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0xff, q16);
  ASSERT_EQUAL_128(0, 0x78, q17);
  ASSERT_EQUAL_128(0, 0xfe, q18);
  ASSERT_EQUAL_128(0, 0x3f, q19);
  ASSERT_EQUAL_128(0, 0xffff, q20);
  ASSERT_EQUAL_128(0, 0x7878, q21);
  ASSERT_EQUAL_128(0, 0xfefe, q22);
  ASSERT_EQUAL_128(0, 0x3fbf, q23);
  ASSERT_EQUAL_128(0, 0xffffffff, q24);
  ASSERT_EQUAL_128(0, 0x78007878, q25);
  ASSERT_EQUAL_128(0, 0xfffefefe, q26);
  ASSERT_EQUAL_128(0, 0x3fffbfbf, q27);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q28);
  ASSERT_EQUAL_128(0, 0x7800000078007878, q29);
  ASSERT_EQUAL_128(0, 0xfffffffefffefefe, q30);
  ASSERT_EQUAL_128(0, 0x3fffffffbfffbfbf, q31);

  TEARDOWN();
}


TEST(neon_sqshl_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaaaaaaaaaa, 0xbfffffffbfffbfbf);
  __ Movi(v1.V2D(), 0x5555555555555555, 0x4000000040004040);
  __ Movi(v2.V2D(), 0xaaaaaaaaaaaaaaaa, 0x0000000000000001);
  __ Movi(v3.V2D(), 0xaaaaaaaaaaaaaaaa, 0xffffffffffffffff);

  __ Sqshl(b16, b0, b2);
  __ Sqshl(b17, b0, b3);
  __ Sqshl(b18, b1, b2);
  __ Sqshl(b19, b1, b3);
  __ Sqshl(h20, h0, h2);
  __ Sqshl(h21, h0, h3);
  __ Sqshl(h22, h1, h2);
  __ Sqshl(h23, h1, h3);
  __ Sqshl(s24, s0, s2);
  __ Sqshl(s25, s0, s3);
  __ Sqshl(s26, s1, s2);
  __ Sqshl(s27, s1, s3);
  __ Sqshl(d28, d0, d2);
  __ Sqshl(d29, d0, d3);
  __ Sqshl(d30, d1, d2);
  __ Sqshl(d31, d1, d3);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x80, q16);
  ASSERT_EQUAL_128(0, 0xdf, q17);
  ASSERT_EQUAL_128(0, 0x7f, q18);
  ASSERT_EQUAL_128(0, 0x20, q19);
  ASSERT_EQUAL_128(0, 0x8000, q20);
  ASSERT_EQUAL_128(0, 0xdfdf, q21);
  ASSERT_EQUAL_128(0, 0x7fff, q22);
  ASSERT_EQUAL_128(0, 0x2020, q23);
  ASSERT_EQUAL_128(0, 0x80000000, q24);
  ASSERT_EQUAL_128(0, 0xdfffdfdf, q25);
  ASSERT_EQUAL_128(0, 0x7fffffff, q26);
  ASSERT_EQUAL_128(0, 0x20002020, q27);
  ASSERT_EQUAL_128(0, 0x8000000000000000, q28);
  ASSERT_EQUAL_128(0, 0xdfffffffdfffdfdf, q29);
  ASSERT_EQUAL_128(0, 0x7fffffffffffffff, q30);
  ASSERT_EQUAL_128(0, 0x2000000020002020, q31);

  TEARDOWN();
}


TEST(neon_urshl_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaaaaaaaaaa, 0xf0000000f000f0f0);
  __ Movi(v1.V2D(), 0x5555555555555555, 0x7fffffff7fff7f7f);
  __ Movi(v2.V2D(), 0xaaaaaaaaaaaaaaaa, 0x0000000000000001);
  __ Movi(v3.V2D(), 0xaaaaaaaaaaaaaaaa, 0xffffffffffffffff);

  __ Urshl(d28, d0, d2);
  __ Urshl(d29, d0, d3);
  __ Urshl(d30, d1, d2);
  __ Urshl(d31, d1, d3);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0xe0000001e001e1e0, q28);
  ASSERT_EQUAL_128(0, 0x7800000078007878, q29);
  ASSERT_EQUAL_128(0, 0xfffffffefffefefe, q30);
  ASSERT_EQUAL_128(0, 0x3fffffffbfffbfc0, q31);

  TEARDOWN();
}


TEST(neon_srshl_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaaaaaaaaaa, 0xbfffffffbfffbfbf);
  __ Movi(v1.V2D(), 0x5555555555555555, 0x4000000040004040);
  __ Movi(v2.V2D(), 0xaaaaaaaaaaaaaaaa, 0x0000000000000001);
  __ Movi(v3.V2D(), 0xaaaaaaaaaaaaaaaa, 0xffffffffffffffff);

  __ Srshl(d28, d0, d2);
  __ Srshl(d29, d0, d3);
  __ Srshl(d30, d1, d2);
  __ Srshl(d31, d1, d3);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x7fffffff7fff7f7e, q28);
  ASSERT_EQUAL_128(0, 0xdfffffffdfffdfe0, q29);
  ASSERT_EQUAL_128(0, 0x8000000080008080, q30);
  ASSERT_EQUAL_128(0, 0x2000000020002020, q31);

  TEARDOWN();
}


TEST(neon_uqrshl_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaaaaaaaaaa, 0xf0000000f000f0f0);
  __ Movi(v1.V2D(), 0x5555555555555555, 0x7fffffff7fff7f7f);
  __ Movi(v2.V2D(), 0xaaaaaaaaaaaaaaaa, 0x0000000000000001);
  __ Movi(v3.V2D(), 0xaaaaaaaaaaaaaaaa, 0xffffffffffffffff);

  __ Uqrshl(b16, b0, b2);
  __ Uqrshl(b17, b0, b3);
  __ Uqrshl(b18, b1, b2);
  __ Uqrshl(b19, b1, b3);
  __ Uqrshl(h20, h0, h2);
  __ Uqrshl(h21, h0, h3);
  __ Uqrshl(h22, h1, h2);
  __ Uqrshl(h23, h1, h3);
  __ Uqrshl(s24, s0, s2);
  __ Uqrshl(s25, s0, s3);
  __ Uqrshl(s26, s1, s2);
  __ Uqrshl(s27, s1, s3);
  __ Uqrshl(d28, d0, d2);
  __ Uqrshl(d29, d0, d3);
  __ Uqrshl(d30, d1, d2);
  __ Uqrshl(d31, d1, d3);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0xff, q16);
  ASSERT_EQUAL_128(0, 0x78, q17);
  ASSERT_EQUAL_128(0, 0xfe, q18);
  ASSERT_EQUAL_128(0, 0x40, q19);
  ASSERT_EQUAL_128(0, 0xffff, q20);
  ASSERT_EQUAL_128(0, 0x7878, q21);
  ASSERT_EQUAL_128(0, 0xfefe, q22);
  ASSERT_EQUAL_128(0, 0x3fc0, q23);
  ASSERT_EQUAL_128(0, 0xffffffff, q24);
  ASSERT_EQUAL_128(0, 0x78007878, q25);
  ASSERT_EQUAL_128(0, 0xfffefefe, q26);
  ASSERT_EQUAL_128(0, 0x3fffbfc0, q27);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q28);
  ASSERT_EQUAL_128(0, 0x7800000078007878, q29);
  ASSERT_EQUAL_128(0, 0xfffffffefffefefe, q30);
  ASSERT_EQUAL_128(0, 0x3fffffffbfffbfc0, q31);

  TEARDOWN();
}


TEST(neon_sqrshl_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaaaaaaaaaa, 0xbfffffffbfffbfbf);
  __ Movi(v1.V2D(), 0x5555555555555555, 0x4000000040004040);
  __ Movi(v2.V2D(), 0xaaaaaaaaaaaaaaaa, 0x0000000000000001);
  __ Movi(v3.V2D(), 0xaaaaaaaaaaaaaaaa, 0xffffffffffffffff);

  __ Sqrshl(b16, b0, b2);
  __ Sqrshl(b17, b0, b3);
  __ Sqrshl(b18, b1, b2);
  __ Sqrshl(b19, b1, b3);
  __ Sqrshl(h20, h0, h2);
  __ Sqrshl(h21, h0, h3);
  __ Sqrshl(h22, h1, h2);
  __ Sqrshl(h23, h1, h3);
  __ Sqrshl(s24, s0, s2);
  __ Sqrshl(s25, s0, s3);
  __ Sqrshl(s26, s1, s2);
  __ Sqrshl(s27, s1, s3);
  __ Sqrshl(d28, d0, d2);
  __ Sqrshl(d29, d0, d3);
  __ Sqrshl(d30, d1, d2);
  __ Sqrshl(d31, d1, d3);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x80, q16);
  ASSERT_EQUAL_128(0, 0xe0, q17);
  ASSERT_EQUAL_128(0, 0x7f, q18);
  ASSERT_EQUAL_128(0, 0x20, q19);
  ASSERT_EQUAL_128(0, 0x8000, q20);
  ASSERT_EQUAL_128(0, 0xdfe0, q21);
  ASSERT_EQUAL_128(0, 0x7fff, q22);
  ASSERT_EQUAL_128(0, 0x2020, q23);
  ASSERT_EQUAL_128(0, 0x80000000, q24);
  ASSERT_EQUAL_128(0, 0xdfffdfe0, q25);
  ASSERT_EQUAL_128(0, 0x7fffffff, q26);
  ASSERT_EQUAL_128(0, 0x20002020, q27);
  ASSERT_EQUAL_128(0, 0x8000000000000000, q28);
  ASSERT_EQUAL_128(0, 0xdfffffffdfffdfe0, q29);
  ASSERT_EQUAL_128(0, 0x7fffffffffffffff, q30);
  ASSERT_EQUAL_128(0, 0x2000000020002020, q31);

  TEARDOWN();
}


TEST(neon_uqadd_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaaaaaaaaaa, 0xf0000000f000f0f0);
  __ Movi(v1.V2D(), 0x5555555555555555, 0x7fffffff7fff7f7f);
  __ Movi(v2.V2D(), 0xaaaaaaaaaaaaaaaa, 0x1000000010001010);

  __ Uqadd(b16, b0, b0);
  __ Uqadd(b17, b1, b1);
  __ Uqadd(b18, b2, b2);
  __ Uqadd(h19, h0, h0);
  __ Uqadd(h20, h1, h1);
  __ Uqadd(h21, h2, h2);
  __ Uqadd(s22, s0, s0);
  __ Uqadd(s23, s1, s1);
  __ Uqadd(s24, s2, s2);
  __ Uqadd(d25, d0, d0);
  __ Uqadd(d26, d1, d1);
  __ Uqadd(d27, d2, d2);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0xff, q16);
  ASSERT_EQUAL_128(0, 0xfe, q17);
  ASSERT_EQUAL_128(0, 0x20, q18);
  ASSERT_EQUAL_128(0, 0xffff, q19);
  ASSERT_EQUAL_128(0, 0xfefe, q20);
  ASSERT_EQUAL_128(0, 0x2020, q21);
  ASSERT_EQUAL_128(0, 0xffffffff, q22);
  ASSERT_EQUAL_128(0, 0xfffefefe, q23);
  ASSERT_EQUAL_128(0, 0x20002020, q24);
  ASSERT_EQUAL_128(0, 0xffffffffffffffff, q25);
  ASSERT_EQUAL_128(0, 0xfffffffefffefefe, q26);
  ASSERT_EQUAL_128(0, 0x2000000020002020, q27);

  TEARDOWN();
}


TEST(neon_sqadd_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaaaaaaaaaa, 0x8000000180018181);
  __ Movi(v1.V2D(), 0x5555555555555555, 0x7fffffff7fff7f7f);
  __ Movi(v2.V2D(), 0xaaaaaaaaaaaaaaaa, 0x1000000010001010);

  __ Sqadd(b16, b0, b0);
  __ Sqadd(b17, b1, b1);
  __ Sqadd(b18, b2, b2);
  __ Sqadd(h19, h0, h0);
  __ Sqadd(h20, h1, h1);
  __ Sqadd(h21, h2, h2);
  __ Sqadd(s22, s0, s0);
  __ Sqadd(s23, s1, s1);
  __ Sqadd(s24, s2, s2);
  __ Sqadd(d25, d0, d0);
  __ Sqadd(d26, d1, d1);
  __ Sqadd(d27, d2, d2);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0x80, q16);
  ASSERT_EQUAL_128(0, 0x7f, q17);
  ASSERT_EQUAL_128(0, 0x20, q18);
  ASSERT_EQUAL_128(0, 0x8000, q19);
  ASSERT_EQUAL_128(0, 0x7fff, q20);
  ASSERT_EQUAL_128(0, 0x2020, q21);
  ASSERT_EQUAL_128(0, 0x80000000, q22);
  ASSERT_EQUAL_128(0, 0x7fffffff, q23);
  ASSERT_EQUAL_128(0, 0x20002020, q24);
  ASSERT_EQUAL_128(0, 0x8000000000000000, q25);
  ASSERT_EQUAL_128(0, 0x7fffffffffffffff, q26);
  ASSERT_EQUAL_128(0, 0x2000000020002020, q27);

  TEARDOWN();
}


TEST(neon_uqsub_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaaaaaaaaaa, 0xf0000000f000f0f0);
  __ Movi(v1.V2D(), 0x5555555555555555, 0x7fffffff7fff7f7f);

  __ Uqsub(b16, b0, b0);
  __ Uqsub(b17, b0, b1);
  __ Uqsub(b18, b1, b0);
  __ Uqsub(h19, h0, h0);
  __ Uqsub(h20, h0, h1);
  __ Uqsub(h21, h1, h0);
  __ Uqsub(s22, s0, s0);
  __ Uqsub(s23, s0, s1);
  __ Uqsub(s24, s1, s0);
  __ Uqsub(d25, d0, d0);
  __ Uqsub(d26, d0, d1);
  __ Uqsub(d27, d1, d0);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0, q16);
  ASSERT_EQUAL_128(0, 0x71, q17);
  ASSERT_EQUAL_128(0, 0, q18);

  ASSERT_EQUAL_128(0, 0, q19);
  ASSERT_EQUAL_128(0, 0x7171, q20);
  ASSERT_EQUAL_128(0, 0, q21);

  ASSERT_EQUAL_128(0, 0, q22);
  ASSERT_EQUAL_128(0, 0x70017171, q23);
  ASSERT_EQUAL_128(0, 0, q24);

  ASSERT_EQUAL_128(0, 0, q25);
  ASSERT_EQUAL_128(0, 0x7000000170017171, q26);
  ASSERT_EQUAL_128(0, 0, q27);

  TEARDOWN();
}


TEST(neon_sqsub_scalar) {
  SETUP();

  START();

  __ Movi(v0.V2D(), 0xaaaaaaaaaaaaaaaa, 0xf0000000f000f0f0);
  __ Movi(v1.V2D(), 0x5555555555555555, 0x7eeeeeee7eee7e7e);

  __ Sqsub(b16, b0, b0);
  __ Sqsub(b17, b0, b1);
  __ Sqsub(b18, b1, b0);
  __ Sqsub(h19, h0, h0);
  __ Sqsub(h20, h0, h1);
  __ Sqsub(h21, h1, h0);
  __ Sqsub(s22, s0, s0);
  __ Sqsub(s23, s0, s1);
  __ Sqsub(s24, s1, s0);
  __ Sqsub(d25, d0, d0);
  __ Sqsub(d26, d0, d1);
  __ Sqsub(d27, d1, d0);

  END();

  RUN();

  ASSERT_EQUAL_128(0, 0, q16);
  ASSERT_EQUAL_128(0, 0x80, q17);
  ASSERT_EQUAL_128(0, 0x7f, q18);

  ASSERT_EQUAL_128(0, 0, q19);
  ASSERT_EQUAL_128(0, 0x8000, q20);
  ASSERT_EQUAL_128(0, 0x7fff, q21);

  ASSERT_EQUAL_128(0, 0, q22);
  ASSERT_EQUAL_128(0, 0x80000000, q23);
  ASSERT_EQUAL_128(0, 0x7fffffff, q24);

  ASSERT_EQUAL_128(0, 0, q25);
  ASSERT_EQUAL_128(0, 0x8000000000000000, q26);
  ASSERT_EQUAL_128(0, 0x7fffffffffffffff, q27);

  TEARDOWN();
}


TEST(neon_fmla_fmls) {
  SETUP();

  START();
  __ Movi(v0.V2D(), 0x3f80000040000000, 0x4100000000000000);
  __ Movi(v1.V2D(), 0x400000003f800000, 0x000000003f800000);
  __ Movi(v2.V2D(), 0x3f800000ffffffff, 0x7f800000ff800000);
  __ Mov(v16.V16B(), v0.V16B());
  __ Mov(v17.V16B(), v0.V16B());
  __ Mov(v18.V16B(), v0.V16B());
  __ Mov(v19.V16B(), v0.V16B());
  __ Mov(v20.V16B(), v0.V16B());
  __ Mov(v21.V16B(), v0.V16B());

  __ Fmla(v16.V2S(), v1.V2S(), v2.V2S());
  __ Fmla(v17.V4S(), v1.V4S(), v2.V4S());
  __ Fmla(v18.V2D(), v1.V2D(), v2.V2D());
  __ Fmls(v19.V2S(), v1.V2S(), v2.V2S());
  __ Fmls(v20.V4S(), v1.V4S(), v2.V4S());
  __ Fmls(v21.V2D(), v1.V2D(), v2.V2D());
  END();

  RUN();

  ASSERT_EQUAL_128(0x0000000000000000, 0x7fc00000ff800000, q16);
  ASSERT_EQUAL_128(0x40400000ffffffff, 0x7fc00000ff800000, q17);
  ASSERT_EQUAL_128(0x3f9800015f8003f7, 0x41000000000000fe, q18);
  ASSERT_EQUAL_128(0x0000000000000000, 0x7fc000007f800000, q19);
  ASSERT_EQUAL_128(0xbf800000ffffffff, 0x7fc000007f800000, q20);
  ASSERT_EQUAL_128(0xbf8000023f0007ee, 0x40fffffffffffe04, q21);

  TEARDOWN();
}


TEST(neon_fmulx_scalar) {
  SETUP();

  START();
  __ Fmov(s0, 2.0);
  __ Fmov(s1, 0.5);
  __ Fmov(s2, 0.0);
  __ Fmov(s3, -0.0);
  __ Fmov(s4, kFP32PositiveInfinity);
  __ Fmov(s5, kFP32NegativeInfinity);
  __ Fmulx(s16, s0, s1);
  __ Fmulx(s17, s2, s4);
  __ Fmulx(s18, s2, s5);
  __ Fmulx(s19, s3, s4);
  __ Fmulx(s20, s3, s5);

  __ Fmov(d21, 2.0);
  __ Fmov(d22, 0.5);
  __ Fmov(d23, 0.0);
  __ Fmov(d24, -0.0);
  __ Fmov(d25, kFP64PositiveInfinity);
  __ Fmov(d26, kFP64NegativeInfinity);
  __ Fmulx(d27, d21, d22);
  __ Fmulx(d28, d23, d25);
  __ Fmulx(d29, d23, d26);
  __ Fmulx(d30, d24, d25);
  __ Fmulx(d31, d24, d26);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s16);
  ASSERT_EQUAL_FP32(2.0, s17);
  ASSERT_EQUAL_FP32(-2.0, s18);
  ASSERT_EQUAL_FP32(-2.0, s19);
  ASSERT_EQUAL_FP32(2.0, s20);
  ASSERT_EQUAL_FP64(1.0, d27);
  ASSERT_EQUAL_FP64(2.0, d28);
  ASSERT_EQUAL_FP64(-2.0, d29);
  ASSERT_EQUAL_FP64(-2.0, d30);
  ASSERT_EQUAL_FP64(2.0, d31);

  TEARDOWN();
}


TEST(crc32b) {
  SETUP();
  START();

  __ Mov(w0, 0);
  __ Mov(w1, 0);
  __ Crc32b(w10, w0, w1);

  __ Mov(w0, 0x1);
  __ Mov(w1, 0x138);
  __ Crc32b(w11, w0, w1);

  __ Mov(w0, 0x1);
  __ Mov(w1, 0x38);
  __ Crc32b(w12, w0, w1);

  __ Mov(w0, 0);
  __ Mov(w1, 128);
  __ Crc32b(w13, w0, w1);

  __ Mov(w0, UINT32_MAX);
  __ Mov(w1, 255);
  __ Crc32b(w14, w0, w1);

  __ Mov(w0, 0x00010001);
  __ Mov(w1, 0x10001000);
  __ Crc32b(w15, w0, w1);

  END();
  RUN();

  ASSERT_EQUAL_64(0x0,        x10);
  ASSERT_EQUAL_64(0x5f058808, x11);
  ASSERT_EQUAL_64(0x5f058808, x12);
  ASSERT_EQUAL_64(0xedb88320, x13);
  ASSERT_EQUAL_64(0x00ffffff, x14);
  ASSERT_EQUAL_64(0x77073196, x15);

  TEARDOWN();
}


TEST(crc32h) {
  SETUP();
  START();

  __ Mov(w0, 0);
  __ Mov(w1, 0);
  __ Crc32h(w10, w0, w1);

  __ Mov(w0, 0x1);
  __ Mov(w1, 0x10038);
  __ Crc32h(w11, w0, w1);

  __ Mov(w0, 0x1);
  __ Mov(w1, 0x38);
  __ Crc32h(w12, w0, w1);

  __ Mov(w0, 0);
  __ Mov(w1, 128);
  __ Crc32h(w13, w0, w1);

  __ Mov(w0, UINT32_MAX);
  __ Mov(w1, 255);
  __ Crc32h(w14, w0, w1);

  __ Mov(w0, 0x00010001);
  __ Mov(w1, 0x10001000);
  __ Crc32h(w15, w0, w1);

  END();
  RUN();

  ASSERT_EQUAL_64(0x0,        x10);
  ASSERT_EQUAL_64(0x0e848dba, x11);
  ASSERT_EQUAL_64(0x0e848dba, x12);
  ASSERT_EQUAL_64(0x3b83984b, x13);
  ASSERT_EQUAL_64(0x2d021072, x14);
  ASSERT_EQUAL_64(0x04ac2124, x15);

  TEARDOWN();
}


TEST(crc32w) {
  SETUP();
  START();

  __ Mov(w0, 0);
  __ Mov(w1, 0);
  __ Crc32w(w10, w0, w1);

  __ Mov(w0, 0x1);
  __ Mov(w1, 0x80000031);
  __ Crc32w(w11, w0, w1);

  __ Mov(w0, 0);
  __ Mov(w1, 128);
  __ Crc32w(w13, w0, w1);

  __ Mov(w0, UINT32_MAX);
  __ Mov(w1, 255);
  __ Crc32w(w14, w0, w1);

  __ Mov(w0, 0x00010001);
  __ Mov(w1, 0x10001000);
  __ Crc32w(w15, w0, w1);

  END();
  RUN();

  ASSERT_EQUAL_64(0x0,        x10);
  ASSERT_EQUAL_64(0x1d937b81, x11);
  ASSERT_EQUAL_64(0xed59b63b, x13);
  ASSERT_EQUAL_64(0x00be2612, x14);
  ASSERT_EQUAL_64(0xa036e530, x15);

  TEARDOWN();
}


TEST(crc32x) {
  SETUP();
  START();

  __ Mov(w0, 0);
  __ Mov(x1, 0);
  __ Crc32x(w10, w0, x1);

  __ Mov(w0, 0x1);
  __ Mov(x1, UINT64_C(0x0000000800000031));
  __ Crc32x(w11, w0, x1);

  __ Mov(w0, 0);
  __ Mov(x1, 128);
  __ Crc32x(w13, w0, x1);

  __ Mov(w0, UINT32_MAX);
  __ Mov(x1, 255);
  __ Crc32x(w14, w0, x1);

  __ Mov(w0, 0x00010001);
  __ Mov(x1, UINT64_C(0x1000100000000000));
  __ Crc32x(w15, w0, x1);

  END();
  RUN();

  ASSERT_EQUAL_64(0x0,        x10);
  ASSERT_EQUAL_64(0x40797b92, x11);
  ASSERT_EQUAL_64(0x533b85da, x13);
  ASSERT_EQUAL_64(0xbc962670, x14);
  ASSERT_EQUAL_64(0x0667602f, x15);

  TEARDOWN();
}


TEST(crc32cb) {
  SETUP();
  START();

  __ Mov(w0, 0);
  __ Mov(w1, 0);
  __ Crc32cb(w10, w0, w1);

  __ Mov(w0, 0x1);
  __ Mov(w1, 0x138);
  __ Crc32cb(w11, w0, w1);

  __ Mov(w0, 0x1);
  __ Mov(w1, 0x38);
  __ Crc32cb(w12, w0, w1);

  __ Mov(w0, 0);
  __ Mov(w1, 128);
  __ Crc32cb(w13, w0, w1);

  __ Mov(w0, UINT32_MAX);
  __ Mov(w1, 255);
  __ Crc32cb(w14, w0, w1);

  __ Mov(w0, 0x00010001);
  __ Mov(w1, 0x10001000);
  __ Crc32cb(w15, w0, w1);

  END();
  RUN();

  ASSERT_EQUAL_64(0x0,        x10);
  ASSERT_EQUAL_64(0x4851927d, x11);
  ASSERT_EQUAL_64(0x4851927d, x12);
  ASSERT_EQUAL_64(0x82f63b78, x13);
  ASSERT_EQUAL_64(0x00ffffff, x14);
  ASSERT_EQUAL_64(0xf26b8203, x15);

  TEARDOWN();
}


TEST(crc32ch) {
  SETUP();
  START();

  __ Mov(w0, 0);
  __ Mov(w1, 0);
  __ Crc32ch(w10, w0, w1);

  __ Mov(w0, 0x1);
  __ Mov(w1, 0x10038);
  __ Crc32ch(w11, w0, w1);

  __ Mov(w0, 0x1);
  __ Mov(w1, 0x38);
  __ Crc32ch(w12, w0, w1);

  __ Mov(w0, 0);
  __ Mov(w1, 128);
  __ Crc32ch(w13, w0, w1);

  __ Mov(w0, UINT32_MAX);
  __ Mov(w1, 255);
  __ Crc32ch(w14, w0, w1);

  __ Mov(w0, 0x00010001);
  __ Mov(w1, 0x10001000);
  __ Crc32ch(w15, w0, w1);

  END();
  RUN();

  ASSERT_EQUAL_64(0x0,        x10);
  ASSERT_EQUAL_64(0xcef8494c, x11);
  ASSERT_EQUAL_64(0xcef8494c, x12);
  ASSERT_EQUAL_64(0xfbc3faf9, x13);
  ASSERT_EQUAL_64(0xad7dacae, x14);
  ASSERT_EQUAL_64(0x03fc5f19, x15);

  TEARDOWN();
}


TEST(crc32cw) {
  SETUP();
  START();

  __ Mov(w0, 0);
  __ Mov(w1, 0);
  __ Crc32cw(w10, w0, w1);

  __ Mov(w0, 0x1);
  __ Mov(w1, 0x80000031);
  __ Crc32cw(w11, w0, w1);

  __ Mov(w0, 0);
  __ Mov(w1, 128);
  __ Crc32cw(w13, w0, w1);

  __ Mov(w0, UINT32_MAX);
  __ Mov(w1, 255);
  __ Crc32cw(w14, w0, w1);

  __ Mov(w0, 0x00010001);
  __ Mov(w1, 0x10001000);
  __ Crc32cw(w15, w0, w1);

  END();
  RUN();

  ASSERT_EQUAL_64(0x0,        x10);
  ASSERT_EQUAL_64(0xbcb79ece, x11);
  ASSERT_EQUAL_64(0x52a0c93f, x13);
  ASSERT_EQUAL_64(0x9f9b5c7a, x14);
  ASSERT_EQUAL_64(0xae1b882a, x15);

  TEARDOWN();
}


TEST(crc32cx) {
  SETUP();
  START();

  __ Mov(w0, 0);
  __ Mov(x1, 0);
  __ Crc32cx(w10, w0, x1);

  __ Mov(w0, 0x1);
  __ Mov(x1, UINT64_C(0x0000000800000031));
  __ Crc32cx(w11, w0, x1);

  __ Mov(w0, 0);
  __ Mov(x1, 128);
  __ Crc32cx(w13, w0, x1);

  __ Mov(w0, UINT32_MAX);
  __ Mov(x1, 255);
  __ Crc32cx(w14, w0, x1);

  __ Mov(w0, 0x00010001);
  __ Mov(x1, UINT64_C(0x1000100000000000));
  __ Crc32cx(w15, w0, x1);

  END();
  RUN();

  ASSERT_EQUAL_64(0x0,        x10);
  ASSERT_EQUAL_64(0x7f320fcb, x11);
  ASSERT_EQUAL_64(0x34019664, x13);
  ASSERT_EQUAL_64(0x6cc27dd0, x14);
  ASSERT_EQUAL_64(0xc6f0acdb, x15);

  TEARDOWN();
}


TEST(neon_fabd_scalar) {
  SETUP();

  START();
  __ Fmov(s0, 2.0);
  __ Fmov(s1, 0.5);
  __ Fmov(s2, 0.0);
  __ Fmov(s3, -0.0);
  __ Fmov(s4, kFP32PositiveInfinity);
  __ Fmov(s5, kFP32NegativeInfinity);
  __ Fabd(s16, s1, s0);
  __ Fabd(s17, s2, s3);
  __ Fabd(s18, s2, s5);
  __ Fabd(s19, s3, s4);
  __ Fabd(s20, s3, s5);

  __ Fmov(d21, 2.0);
  __ Fmov(d22, 0.5);
  __ Fmov(d23, 0.0);
  __ Fmov(d24, -0.0);
  __ Fmov(d25, kFP64PositiveInfinity);
  __ Fmov(d26, kFP64NegativeInfinity);
  __ Fabd(d27, d21, d22);
  __ Fabd(d28, d23, d24);
  __ Fabd(d29, d23, d26);
  __ Fabd(d30, d24, d25);
  __ Fabd(d31, d24, d26);
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.5, s16);
  ASSERT_EQUAL_FP32(0.0, s17);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s18);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s19);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s20);
  ASSERT_EQUAL_FP64(1.5, d27);
  ASSERT_EQUAL_FP64(0.0, d28);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d29);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d30);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d31);

  TEARDOWN();
}


TEST(neon_faddp_scalar) {
  SETUP();

  START();
  __ Movi(d0, 0x3f80000040000000);
  __ Movi(d1, 0xff8000007f800000);
  __ Movi(d2, 0x0000000080000000);
  __ Faddp(s0, v0.V2S());
  __ Faddp(s1, v1.V2S());
  __ Faddp(s2, v2.V2S());

  __ Movi(v3.V2D(), 0xc000000000000000, 0x4000000000000000);
  __ Movi(v4.V2D(), 0xfff8000000000000, 0x7ff8000000000000);
  __ Movi(v5.V2D(), 0x0000000000000000, 0x8000000000000000);
  __ Faddp(d3, v3.V2D());
  __ Faddp(d4, v4.V2D());
  __ Faddp(d5, v5.V2D());
  END();

  RUN();

  ASSERT_EQUAL_FP32(3.0, s0);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s1);
  ASSERT_EQUAL_FP32(0.0, s2);
  ASSERT_EQUAL_FP64(0.0, d3);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d4);
  ASSERT_EQUAL_FP64(0.0, d5);

  TEARDOWN();
}


TEST(neon_fmaxp_scalar) {
  SETUP();

  START();
  __ Movi(d0, 0x3f80000040000000);
  __ Movi(d1, 0xff8000007f800000);
  __ Movi(d2, 0x7fc00000ff800000);
  __ Fmaxp(s0, v0.V2S());
  __ Fmaxp(s1, v1.V2S());
  __ Fmaxp(s2, v2.V2S());

  __ Movi(v3.V2D(), 0x3ff0000000000000, 0x4000000000000000);
  __ Movi(v4.V2D(), 0xfff0000000000000, 0x7ff0000000000000);
  __ Movi(v5.V2D(), 0x7ff0000000000000, 0x7ff8000000000000);
  __ Fmaxp(d3, v3.V2D());
  __ Fmaxp(d4, v4.V2D());
  __ Fmaxp(d5, v5.V2D());
  END();

  RUN();

  ASSERT_EQUAL_FP32(2.0, s0);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s1);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s2);
  ASSERT_EQUAL_FP64(2.0, d3);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d4);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d5);

  TEARDOWN();
}


TEST(neon_fmaxnmp_scalar) {
  SETUP();

  START();
  __ Movi(d0, 0x3f80000040000000);
  __ Movi(d1, 0xff8000007f800000);
  __ Movi(d2, 0x7fc00000ff800000);
  __ Fmaxnmp(s0, v0.V2S());
  __ Fmaxnmp(s1, v1.V2S());
  __ Fmaxnmp(s2, v2.V2S());

  __ Movi(v3.V2D(), 0x3ff0000000000000, 0x4000000000000000);
  __ Movi(v4.V2D(), 0xfff0000000000000, 0x7ff0000000000000);
  __ Movi(v5.V2D(), 0x7ff8000000000000, 0xfff0000000000000);
  __ Fmaxnmp(d3, v3.V2D());
  __ Fmaxnmp(d4, v4.V2D());
  __ Fmaxnmp(d5, v5.V2D());
  END();

  RUN();

  ASSERT_EQUAL_FP32(2.0, s0);
  ASSERT_EQUAL_FP32(kFP32PositiveInfinity, s1);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s2);
  ASSERT_EQUAL_FP64(2.0, d3);
  ASSERT_EQUAL_FP64(kFP64PositiveInfinity, d4);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d5);

  TEARDOWN();
}


TEST(neon_fminp_scalar) {
  SETUP();

  START();
  __ Movi(d0, 0x3f80000040000000);
  __ Movi(d1, 0xff8000007f800000);
  __ Movi(d2, 0x7fc00000ff800000);
  __ Fminp(s0, v0.V2S());
  __ Fminp(s1, v1.V2S());
  __ Fminp(s2, v2.V2S());

  __ Movi(v3.V2D(), 0x3ff0000000000000, 0x4000000000000000);
  __ Movi(v4.V2D(), 0xfff0000000000000, 0x7ff0000000000000);
  __ Movi(v5.V2D(), 0x7ff0000000000000, 0x7ff8000000000000);
  __ Fminp(d3, v3.V2D());
  __ Fminp(d4, v4.V2D());
  __ Fminp(d5, v5.V2D());
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s0);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s1);
  ASSERT_EQUAL_FP32(kFP32DefaultNaN, s2);
  ASSERT_EQUAL_FP64(1.0, d3);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d4);
  ASSERT_EQUAL_FP64(kFP64DefaultNaN, d5);

  TEARDOWN();
}


TEST(neon_fminnmp_scalar) {
  SETUP();

  START();
  __ Movi(d0, 0x3f80000040000000);
  __ Movi(d1, 0xff8000007f800000);
  __ Movi(d2, 0x7fc00000ff800000);
  __ Fminnmp(s0, v0.V2S());
  __ Fminnmp(s1, v1.V2S());
  __ Fminnmp(s2, v2.V2S());

  __ Movi(v3.V2D(), 0x3ff0000000000000, 0x4000000000000000);
  __ Movi(v4.V2D(), 0xfff0000000000000, 0x7ff0000000000000);
  __ Movi(v5.V2D(), 0x7ff8000000000000, 0xfff0000000000000);
  __ Fminnmp(d3, v3.V2D());
  __ Fminnmp(d4, v4.V2D());
  __ Fminnmp(d5, v5.V2D());
  END();

  RUN();

  ASSERT_EQUAL_FP32(1.0, s0);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s1);
  ASSERT_EQUAL_FP32(kFP32NegativeInfinity, s2);
  ASSERT_EQUAL_FP64(1.0, d3);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d4);
  ASSERT_EQUAL_FP64(kFP64NegativeInfinity, d5);

  TEARDOWN();
}


TEST(neon_tbl) {
  SETUP();

  START();
  __ Movi(v30.V2D(), 0xbf561e188b1280e9, 0xbd542b8cbd24e8e8);
  __ Movi(v31.V2D(), 0xb5e9883d2c88a46d, 0x12276d5b614c915e);
  __ Movi(v0.V2D(), 0xc45b7782bc5ecd72, 0x5dd4fe5a4bc6bf5e);
  __ Movi(v1.V2D(), 0x1e3254094bd1746a, 0xf099ecf50e861c80);

  __ Movi(v4.V2D(), 0xf80c030100031f16, 0x00070504031201ff);
  __ Movi(v5.V2D(), 0x1f01001afc14202a, 0x2a081e1b0c02020c);
  __ Movi(v6.V2D(), 0x353f1a13022a2360, 0x2c464a00203a0a33);
  __ Movi(v7.V2D(), 0x64801a1c054cf30d, 0x793a2c052e213739);

  __ Movi(v8.V2D(), 0xb7f60ad7d7d88f13, 0x13eefc240496e842);
  __ Movi(v9.V2D(), 0x1be199c7c69b47ec, 0x8e4b9919f6eed443);
  __ Movi(v10.V2D(), 0x9bd2e1654c69e48f, 0x2143d089e426c6d2);
  __ Movi(v11.V2D(), 0xc31dbdc4a0393065, 0x1ecc2077caaf64d8);
  __ Movi(v12.V2D(), 0x29b24463967bc6eb, 0xdaf59970df01c93b);
  __ Movi(v13.V2D(), 0x3e20a4a4cb6813f4, 0x20a5832713dae669);
  __ Movi(v14.V2D(), 0xc5ff9a94041b1fdf, 0x2f46cde38cba2682);
  __ Movi(v15.V2D(), 0xd8cc5b0e61f387e6, 0xe69d6d314971e8fd);

  __ Tbl(v8.V16B(), v1.V16B(), v4.V16B());
  __ Tbl(v9.V16B(), v0.V16B(), v1.V16B(), v5.V16B());
  __ Tbl(v10.V16B(), v31.V16B(), v0.V16B(), v1.V16B(), v6.V16B());
  __ Tbl(v11.V16B(), v30.V16B(), v31.V16B(), v0.V16B(), v1.V16B(), v7.V16B());
  __ Tbl(v12.V8B(), v1.V16B(), v4.V8B());
  __ Tbl(v13.V8B(), v0.V16B(), v1.V16B(), v5.V8B());
  __ Tbl(v14.V8B(), v31.V16B(), v0.V16B(), v1.V16B(), v6.V8B());
  __ Tbl(v15.V8B(), v30.V16B(), v31.V16B(), v0.V16B(), v1.V16B(), v7.V8B());

  __ Movi(v16.V2D(), 0xb7f60ad7d7d88f13, 0x13eefc240496e842);
  __ Movi(v17.V2D(), 0x1be199c7c69b47ec, 0x8e4b9919f6eed443);
  __ Movi(v18.V2D(), 0x9bd2e1654c69e48f, 0x2143d089e426c6d2);
  __ Movi(v19.V2D(), 0xc31dbdc4a0393065, 0x1ecc2077caaf64d8);
  __ Movi(v20.V2D(), 0x29b24463967bc6eb, 0xdaf59970df01c93b);
  __ Movi(v21.V2D(), 0x3e20a4a4cb6813f4, 0x20a5832713dae669);
  __ Movi(v22.V2D(), 0xc5ff9a94041b1fdf, 0x2f46cde38cba2682);
  __ Movi(v23.V2D(), 0xd8cc5b0e61f387e6, 0xe69d6d314971e8fd);

  __ Tbx(v16.V16B(), v1.V16B(), v4.V16B());
  __ Tbx(v17.V16B(), v0.V16B(), v1.V16B(), v5.V16B());
  __ Tbx(v18.V16B(), v31.V16B(), v0.V16B(), v1.V16B(), v6.V16B());
  __ Tbx(v19.V16B(), v30.V16B(), v31.V16B(), v0.V16B(), v1.V16B(), v7.V16B());
  __ Tbx(v20.V8B(), v1.V16B(), v4.V8B());
  __ Tbx(v21.V8B(), v0.V16B(), v1.V16B(), v5.V8B());
  __ Tbx(v22.V8B(), v31.V16B(), v0.V16B(), v1.V16B(), v6.V8B());
  __ Tbx(v23.V8B(), v30.V16B(), v31.V16B(), v0.V16B(), v1.V16B(), v7.V8B());
  END();

  RUN();

  ASSERT_EQUAL_128(0x00090e1c800e0000, 0x80f0ecf50e001c00, v8);
  ASSERT_EQUAL_128(0x1ebf5ed100f50000, 0x0072324b82c6c682, v9);
  ASSERT_EQUAL_128(0x00005e4b4cd10e00, 0x0900005e80008800, v10);
  ASSERT_EQUAL_128(0x0000883d2b00001e, 0x00d1822b5bbff074, v11);
  ASSERT_EQUAL_128(0x0000000000000000, 0x80f0ecf50e001c00, v12);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0072324b82c6c682, v13);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0900005e80008800, v14);
  ASSERT_EQUAL_128(0x0000000000000000, 0x00d1822b5bbff074, v15);

  ASSERT_EQUAL_128(0xb7090e1c800e8f13, 0x80f0ecf50e961c42, v16);
  ASSERT_EQUAL_128(0x1ebf5ed1c6f547ec, 0x8e72324b82c6c682, v17);
  ASSERT_EQUAL_128(0x9bd25e4b4cd10e8f, 0x0943d05e802688d2, v18);
  ASSERT_EQUAL_128(0xc31d883d2b39301e, 0x1ed1822b5bbff074, v19);
  ASSERT_EQUAL_128(0x0000000000000000, 0x80f0ecf50e011c3b, v20);
  ASSERT_EQUAL_128(0x0000000000000000, 0x2072324b82c6c682, v21);
  ASSERT_EQUAL_128(0x0000000000000000, 0x0946cd5e80ba8882, v22);
  ASSERT_EQUAL_128(0x0000000000000000, 0xe6d1822b5bbff074, v23);

  TEARDOWN();
}


TEST(regress_cmp_shift_imm) {
  SETUP();

  START();

  __ Mov(x0, 0x3d720c8d);
  __ Cmp(x0, Operand(0x3d720c8d));

  END();
  RUN();

  ASSERT_EQUAL_NZCV(ZCFlag);

  TEARDOWN();
}


TEST(compute_address) {
  SETUP();

  START();
  int64_t base_address = INT64_C(0x123000000abc);
  int64_t reg_offset = INT64_C(0x1087654321);
  Register base = x0;
  Register offset = x1;

  __ Mov(base, base_address);
  __ Mov(offset, reg_offset);


  __ ComputeAddress(x2, MemOperand(base, 0));
  __ ComputeAddress(x3, MemOperand(base, 8));
  __ ComputeAddress(x4, MemOperand(base, -100));

  __ ComputeAddress(x5, MemOperand(base, offset));
  __ ComputeAddress(x6, MemOperand(base, offset, LSL, 2));
  __ ComputeAddress(x7, MemOperand(base, offset, LSL, 4));
  __ ComputeAddress(x8, MemOperand(base, offset, LSL, 8));

  __ ComputeAddress(x9, MemOperand(base, offset, SXTW));
  __ ComputeAddress(x10, MemOperand(base, offset, UXTW, 1));
  __ ComputeAddress(x11, MemOperand(base, offset, SXTW, 2));
  __ ComputeAddress(x12, MemOperand(base, offset, UXTW, 3));

  END();

  RUN();

  ASSERT_EQUAL_64(base_address, base);

  ASSERT_EQUAL_64(INT64_C(0x123000000abc), x2);
  ASSERT_EQUAL_64(INT64_C(0x123000000ac4), x3);
  ASSERT_EQUAL_64(INT64_C(0x123000000a58), x4);

  ASSERT_EQUAL_64(INT64_C(0x124087654ddd), x5);
  ASSERT_EQUAL_64(INT64_C(0x12721d951740), x6);
  ASSERT_EQUAL_64(INT64_C(0x133876543ccc), x7);
  ASSERT_EQUAL_64(INT64_C(0x22b765432bbc), x8);

  ASSERT_EQUAL_64(INT64_C(0x122f87654ddd), x9);
  ASSERT_EQUAL_64(INT64_C(0x12310eca90fe), x10);
  ASSERT_EQUAL_64(INT64_C(0x122e1d951740), x11);
  ASSERT_EQUAL_64(INT64_C(0x12343b2a23c4), x12);

  TEARDOWN();
}


TEST(far_branch_backward) {
  // Test that the MacroAssembler correctly resolves backward branches to labels
  // that are outside the immediate range of branch instructions.
  // Take into account that backward branches can reach one instruction further
  // than forward branches.
  const int overflow_size = kInstructionSize +
    std::max(Instruction::ImmBranchForwardRange(TestBranchType),
             std::max(Instruction::ImmBranchForwardRange(CompareBranchType),
                      Instruction::ImmBranchForwardRange(CondBranchType)));

  SETUP();
  START();

  Label done, fail;
  Label test_tbz, test_cbz, test_bcond;
  Label success_tbz, success_cbz, success_bcond;

  __ Mov(x0, 0);
  __ Mov(x1, 1);
  __ Mov(x10, 0);

  __ B(&test_tbz);
  __ Bind(&success_tbz);
  __ Orr(x0, x0, 1 << 0);
  __ B(&test_cbz);
  __ Bind(&success_cbz);
  __ Orr(x0, x0, 1 << 1);
  __ B(&test_bcond);
  __ Bind(&success_bcond);
  __ Orr(x0, x0, 1 << 2);

  __ B(&done);

  // Generate enough code to overflow the immediate range of the three types of
  // branches below.
  for (unsigned i = 0; i < overflow_size / kInstructionSize; ++i) {
    if (i % 100 == 0) {
      // If we do land in this code, we do not want to execute so many nops
      // before reaching the end of test (especially if tracing is activated).
      __ B(&fail);
    } else {
      __ Nop();
    }
  }
  __ B(&fail);

  __ Bind(&test_tbz);
  __ Tbz(x10, 7, &success_tbz);
  __ Bind(&test_cbz);
  __ Cbz(x10, &success_cbz);
  __ Bind(&test_bcond);
  __ Cmp(x10, 0);
  __ B(eq, &success_bcond);

  // For each out-of-range branch instructions, at least two instructions should
  // have been generated.
  VIXL_CHECK(masm.SizeOfCodeGeneratedSince(&test_tbz) >= 7 * kInstructionSize);

  __ Bind(&fail);
  __ Mov(x1, 0);
  __ Bind(&done);

  END();
  RUN();

  ASSERT_EQUAL_64(0x7, x0);
  ASSERT_EQUAL_64(0x1, x1);

  TEARDOWN();
}


TEST(single_veneer) {
  SETUP();
  START();

  const int max_range = Instruction::ImmBranchForwardRange(TestBranchType);

  Label success, fail, done;

  __ Mov(x0, 0);
  __ Mov(x1, 1);
  __ Mov(x10, 0);

  __ Tbz(x10, 7, &success);

  // Generate enough code to overflow the immediate range of the `tbz`.
  for (unsigned i = 0; i < max_range / kInstructionSize + 1; ++i) {
    if (i % 100 == 0) {
      // If we do land in this code, we do not want to execute so many nops
      // before reaching the end of test (especially if tracing is activated).
      __ B(&fail);
    } else {
      __ Nop();
    }
  }
  __ B(&fail);

  __ Bind(&success);
  __ Mov(x0, 1);

  __ B(&done);
  __ Bind(&fail);
  __ Mov(x1, 0);
  __ Bind(&done);

  END();
  RUN();

  ASSERT_EQUAL_64(1, x0);
  ASSERT_EQUAL_64(1, x1);

  TEARDOWN();
}


TEST(simple_veneers) {
  // Test that the MacroAssembler correctly emits veneers for forward branches
  // to labels that are outside the immediate range of branch instructions.
  const int max_range =
    std::max(Instruction::ImmBranchForwardRange(TestBranchType),
             std::max(Instruction::ImmBranchForwardRange(CompareBranchType),
                      Instruction::ImmBranchForwardRange(CondBranchType)));

  SETUP();
  START();

  Label done, fail;
  Label test_tbz, test_cbz, test_bcond;
  Label success_tbz, success_cbz, success_bcond;

  __ Mov(x0, 0);
  __ Mov(x1, 1);
  __ Mov(x10, 0);

  __ Bind(&test_tbz);
  __ Tbz(x10, 7, &success_tbz);
  __ Bind(&test_cbz);
  __ Cbz(x10, &success_cbz);
  __ Bind(&test_bcond);
  __ Cmp(x10, 0);
  __ B(eq, &success_bcond);

  // Generate enough code to overflow the immediate range of the three types of
  // branches below.
  for (unsigned i = 0; i < max_range / kInstructionSize + 1; ++i) {
    if (i % 100 == 0) {
      // If we do land in this code, we do not want to execute so many nops
      // before reaching the end of test (especially if tracing is activated).
      __ B(&fail);
    } else {
      __ Nop();
    }
  }
  __ B(&fail);

  __ Bind(&success_tbz);
  __ Orr(x0, x0, 1 << 0);
  __ B(&test_cbz);
  __ Bind(&success_cbz);
  __ Orr(x0, x0, 1 << 1);
  __ B(&test_bcond);
  __ Bind(&success_bcond);
  __ Orr(x0, x0, 1 << 2);

  __ B(&done);
  __ Bind(&fail);
  __ Mov(x1, 0);
  __ Bind(&done);

  END();
  RUN();

  ASSERT_EQUAL_64(0x7, x0);
  ASSERT_EQUAL_64(0x1, x1);

  TEARDOWN();
}


TEST(veneers_stress) {
  SETUP();
  START();

  // This is a code generation test stressing the emission of veneers. The code
  // generated is not executed.

  Label target;
  const unsigned max_range = Instruction::ImmBranchForwardRange(CondBranchType);
  const unsigned iterations =
      (max_range + max_range / 4) / (4 * kInstructionSize);
  for (unsigned i = 0; i < iterations; i++) {
    __ B(&target);
    __ B(eq, &target);
    __ Cbz(x0, &target);
    __ Tbz(x0, 0, &target);
  }
  __ Bind(&target);

  END();
  TEARDOWN();
}


TEST(veneers_two_out_of_range) {
  SETUP();
  START();

  // This is a code generation test. The code generated is not executed.
  // Ensure that the MacroAssembler considers unresolved branches to chose when
  // a veneer pool should be emitted. We generate two branches that go out of
  // range at the same offset. When the MacroAssembler decides to emit the
  // veneer pool, the emission of a first veneer should not cause the other
  // branch to go out of range.

  int range_cbz = Instruction::ImmBranchForwardRange(CompareBranchType);
  int range_tbz = Instruction::ImmBranchForwardRange(TestBranchType);
  int max_target = static_cast<int>(masm.CursorOffset()) + range_cbz;

  Label done;

  // We use different labels to prevent the MacroAssembler from sharing veneers.
  Label target_cbz, target_tbz;

  __ Cbz(x0, &target_cbz);
  while (masm.CursorOffset() < max_target - range_tbz) {
    __ Nop();
  }
  __ Tbz(x0, 0, &target_tbz);
  while (masm.CursorOffset() < max_target) {
    __ Nop();
  }

  // This additional nop makes the branches go out of range.
  __ Nop();

  __ Bind(&target_cbz);
  __ Bind(&target_tbz);

  END();
  TEARDOWN();
}


TEST(veneers_hanging) {
  SETUP();
  START();

  // This is a code generation test. The code generated is not executed.
  // Ensure that the MacroAssembler considers unresolved branches to chose when
  // a veneer pool should be emitted. This is similar to the
  // 'veneers_two_out_of_range' test. We try to trigger the following situation:
  //   b.eq label
  //   b.eq label
  //   ...
  //   nop
  //   ...
  //   cbz x0, label
  //   cbz x0, label
  //   ...
  //   tbz x0, 0 label
  //   nop
  //   ...
  //   nop    <- From here the `b.eq` and `cbz` instructions run out of range,
  //             so a literal pool is required.
  //   veneer
  //   veneer
  //   veneer <- The `tbz` runs out of range somewhere in the middle of the
  //   veneer    veneer pool.
  //   veneer

  const int range_bcond = Instruction::ImmBranchForwardRange(CondBranchType);
  const int range_cbz = Instruction::ImmBranchForwardRange(CompareBranchType);
  const int range_tbz = Instruction::ImmBranchForwardRange(TestBranchType);
  const int max_target = static_cast<int>(masm.CursorOffset()) + range_bcond;

  Label done;
  const int n_bcond = 100;
  const int n_cbz = 100;
  const int n_tbz = 1;
  const int kNTotalBranches = n_bcond + n_cbz + n_tbz;

  // We use different labels to prevent the MacroAssembler from sharing veneers.
  Label labels[kNTotalBranches];
  for (int i = 0; i < kNTotalBranches; i++) {
    new(&labels[i]) Label();
  }

  for (int i = 0; i < n_bcond; i++) {
    __ B(eq, &labels[i]);
  }

  while (masm.CursorOffset() < max_target - range_cbz) {
    __ Nop();
  }

  for (int i = 0; i < n_cbz; i++) {
    __ Cbz(x0, &labels[n_bcond + i]);
  }

  // Ensure the 'tbz' will go out of range after some of the previously
  // generated branches.
  int margin = (n_bcond / 2) * kInstructionSize;
  while (masm.CursorOffset() < max_target - range_tbz + margin) {
    __ Nop();
  }

  __ Tbz(x0, 0, &labels[n_bcond + n_cbz]);

  while (masm.CursorOffset() < max_target) {
    __ Nop();
  }

  // This additional nop makes the 'b.eq' and 'cbz' instructions go out of range
  // and forces the emission of a veneer pool. The 'tbz' is not yet out of
  // range, but will go out of range while veneers are emitted for the other
  // branches.
  // The MacroAssembler should ensure that veneers are correctly emitted for all
  // the branches, including the 'tbz'. Checks will fail if the target of a
  // branch is out of range.
  __ Nop();

  for (int i = 0; i < kNTotalBranches; i++) {
    __ Bind(&labels[i]);
  }

  END();
  TEARDOWN();
}


TEST(collision_literal_veneer_pools) {
  SETUP();
  START();

  // This is a code generation test. The code generated is not executed.

  // Make sure the literal pool is empty;
  masm.EmitLiteralPool(LiteralPool::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  // We chose the offsets below to (try to) trigger the following situation:
  // buffer offset
  //              0:   tbz x0, 0, target_tbz ----------------------------------.
  //              4:   nop                                                     |
  //                   ...                                                     |
  //                   nop                                                     |
  //    literal gen:   ldr s0, [pc + ...]   ; load from `pool start + 0`       |
  //                   ldr s0, [pc + ...]   ; load from `pool start + 4`       |
  //                   ...                                                     |
  //                   ldr s0, [pc + ...]                                      |
  //     pool start:   floating-point literal (0.1)                            |
  //                   floating-point literal (1.1)                            |
  //                   ...                                                     |
  //                   floating-point literal (<n>.1)     <-----tbz-max-range--'
  //                   floating-point literal (<n+1>.1)
  //                   ...

  const int range_tbz = Instruction::ImmBranchForwardRange(TestBranchType);
  const int max_target = static_cast<int>(masm.CursorOffset()) + range_tbz;

  const size_t target_literal_pool_size = 100 * kInstructionSize;
  const int offset_start_literal_gen =
      target_literal_pool_size + target_literal_pool_size / 2;


  Label target_tbz;

  __ Tbz(x0, 0, &target_tbz);
  VIXL_CHECK(masm.NumberOfPotentialVeneers() == 1);
  while (masm.CursorOffset() < max_target - offset_start_literal_gen) {
    __ Nop();
  }
  VIXL_CHECK(masm.NumberOfPotentialVeneers() == 1);

  for (int i = 0; i < 100; i++) {
    // Use a different value to force one literal pool entry per iteration.
    __ Ldr(s0, i + 0.1);
  }
  VIXL_CHECK(masm.LiteralPoolSize() >= target_literal_pool_size);

  // Force emission of a literal pool.
  masm.EmitLiteralPool(LiteralPool::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  // The branch should not have gone out of range during the emission of the
  // literal pool.
  __ Bind(&target_tbz);

  VIXL_CHECK(masm.NumberOfPotentialVeneers() == 0);

  END();
  TEARDOWN();
}


TEST(ldr_literal_explicit) {
  SETUP();

  START();
  Literal<int64_t> automatically_placed_literal(1, masm.GetLiteralPool());
  Literal<int64_t> manually_placed_literal(2);
  {
    CodeBufferCheckScope scope(&masm,
                               kInstructionSize + sizeof(int64_t),
                               CodeBufferCheckScope::kCheck,
                               CodeBufferCheckScope::kExactSize);
    Label over_literal;
    __ b(&over_literal);
    __ place(&manually_placed_literal);
    __ bind(&over_literal);
  }
  __ Ldr(x1, &manually_placed_literal);
  __ Ldr(x2, &automatically_placed_literal);
  __ Add(x0, x1, x2);
  END();

  RUN();

  ASSERT_EQUAL_64(3, x0);

  TEARDOWN();
}


TEST(ldr_literal_automatically_placed) {
  SETUP();

  START();

  // We start with an empty literal pool.
  ASSERT_LITERAL_POOL_SIZE(0);

  // Create a literal that should be placed by the literal pool.
  Literal<int64_t> explicit_literal(2, masm.GetLiteralPool());
  // It should not appear in the literal pool until its first use.
  ASSERT_LITERAL_POOL_SIZE(0);

  // Check that using standard literals does not break the use of explicitly
  // created literals.
  __ Ldr(d1, 1.1);
  ASSERT_LITERAL_POOL_SIZE(8);
  masm.EmitLiteralPool(LiteralPool::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  __ Ldr(x2, &explicit_literal);
  ASSERT_LITERAL_POOL_SIZE(8);
  masm.EmitLiteralPool(LiteralPool::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  __ Ldr(d3, 3.3);
  ASSERT_LITERAL_POOL_SIZE(8);
  masm.EmitLiteralPool(LiteralPool::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  // Re-use our explicitly created literal. It has already been placed, so it
  // should not impact the literal pool.
  __ Ldr(x4, &explicit_literal);
  ASSERT_LITERAL_POOL_SIZE(0);

  END();

  RUN();

  ASSERT_EQUAL_FP64(1.1, d1);
  ASSERT_EQUAL_64(2, x2);
  ASSERT_EQUAL_FP64(3.3, d3);
  ASSERT_EQUAL_64(2, x4);

  TEARDOWN();
}


TEST(literal_update_overwrite) {
  SETUP();

  START();

  ASSERT_LITERAL_POOL_SIZE(0);
  LiteralPool* literal_pool = masm.GetLiteralPool();

  Literal<int32_t> lit_32_update_before_pool(0xbad, literal_pool);
  Literal<int32_t> lit_32_update_after_pool(0xbad, literal_pool);
  Literal<int64_t> lit_64_update_before_pool(0xbad, literal_pool);
  Literal<int64_t> lit_64_update_after_pool(0xbad, literal_pool);

  ASSERT_LITERAL_POOL_SIZE(0);

  lit_32_update_before_pool.UpdateValue(32);
  lit_64_update_before_pool.UpdateValue(64);

  __ Ldr(w1, &lit_32_update_before_pool);
  __ Ldr(x2, &lit_64_update_before_pool);
  __ Ldr(w3, &lit_32_update_after_pool);
  __ Ldr(x4, &lit_64_update_after_pool);

  masm.EmitLiteralPool(LiteralPool::kBranchRequired);

  VIXL_ASSERT(lit_32_update_after_pool.IsPlaced());
  VIXL_ASSERT(lit_64_update_after_pool.IsPlaced());
  lit_32_update_after_pool.UpdateValue(128, &masm);
  lit_64_update_after_pool.UpdateValue(256, &masm);

  END();

  RUN();

  ASSERT_EQUAL_64(32, x1);
  ASSERT_EQUAL_64(64, x2);
  ASSERT_EQUAL_64(128, x3);
  ASSERT_EQUAL_64(256, x4);

  TEARDOWN();
}


TEST(literal_deletion_policies) {
  SETUP();

  START();

  // We cannot check exactly when the deletion of the literals occur, but we
  // check that usage of the deletion policies is not broken.

  ASSERT_LITERAL_POOL_SIZE(0);
  LiteralPool* literal_pool = masm.GetLiteralPool();

  Literal<int32_t> lit_manual(0xbad, literal_pool);
  Literal<int32_t>* lit_deleted_on_placement =
      new Literal<int32_t>(0xbad,
                           literal_pool,
                           RawLiteral::kDeletedOnPlacementByPool);
  Literal<int32_t>* lit_deleted_on_pool_destruction =
      new Literal<int32_t>(0xbad,
                           literal_pool,
                           RawLiteral::kDeletedOnPoolDestruction);

  ASSERT_LITERAL_POOL_SIZE(0);

  lit_manual.UpdateValue(32);
  lit_deleted_on_placement->UpdateValue(64);

  __ Ldr(w1, &lit_manual);
  __ Ldr(w2, lit_deleted_on_placement);
  __ Ldr(w3, lit_deleted_on_pool_destruction);

  masm.EmitLiteralPool(LiteralPool::kBranchRequired);

  VIXL_ASSERT(lit_manual.IsPlaced());
  VIXL_ASSERT(lit_deleted_on_pool_destruction->IsPlaced());
  lit_deleted_on_pool_destruction->UpdateValue(128, &masm);

  END();

  RUN();

  ASSERT_EQUAL_64(32, x1);
  ASSERT_EQUAL_64(64, x2);
  ASSERT_EQUAL_64(128, x3);

  TEARDOWN();
}


}  // namespace vixl
