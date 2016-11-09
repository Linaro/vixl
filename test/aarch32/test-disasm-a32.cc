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

#include <sstream>
#include <string>
#include <list>

#include "test-runner.h"
#include "test-utils.h"

#include "aarch32/macro-assembler-aarch32.h"
#include "aarch32/disasm-aarch32.h"

namespace vixl {
namespace aarch32 {

#define __ masm.
#define TEST(name)  TEST_(AARCH32_DISASM_##name)

#define BUF_SIZE (4096)

#define SETUP()                   \
  MacroAssembler masm(BUF_SIZE);

#define CLEANUP()

#define START_COMPARE()                                                        \
  {                                                                            \
    int32_t start = masm.GetCursorOffset();

#define END_COMPARE(EXP)                                                       \
    int32_t end = masm.GetCursorOffset();                                      \
    masm.FinalizeCode();                                                       \
    std::ostringstream ss;                                                     \
    TestDisassembler disassembler(ss, 0);                                      \
    if (masm.IsUsingT32()) {                                                   \
      disassembler.DisassembleT32(*masm.GetBuffer(), start, end);              \
    } else {                                                                   \
      disassembler.DisassembleA32(*masm.GetBuffer(), start, end);              \
    }                                                                          \
    masm.GetBuffer()->Reset();                                                 \
    if (Test::disassemble()) {                                                 \
      printf("%s", ss.str().c_str());                                          \
    }                                                                          \
    if (std::string(EXP) != ss.str()) {                                        \
      printf("\nFound:\n%sExpected:\n%s", ss.str().c_str(), EXP);              \
      abort();                                                                 \
    }                                                                          \
  }

#define COMPARE_A32(ASM, EXP)                                                  \
  masm.UseA32();                                                               \
  START_COMPARE()                                                              \
  masm.ASM;                                                                    \
  END_COMPARE(EXP)

#define COMPARE_T32(ASM, EXP)                                                  \
  masm.UseT32();                                                               \
  START_COMPARE()                                                              \
  masm.ASM;                                                                    \
  END_COMPARE(EXP)

#define COMPARE_BOTH(ASM, EXP)                                                 \
  COMPARE_A32(ASM, EXP)                                                        \
  COMPARE_T32(ASM, EXP)


class TestDisassembler : public PrintDisassembler {
 public:
  TestDisassembler(std::ostream& os, uint32_t pc)  // NOLINT(runtime/references)
      : PrintDisassembler(os, pc) {
  }

  virtual void PrintCodeAddress(uint32_t code_address) VIXL_OVERRIDE {
    USE(code_address);
  }

  virtual void PrintOpcode16(uint32_t opcode) VIXL_OVERRIDE {
    USE(opcode);
  }

  virtual void PrintOpcode32(uint32_t opcode) VIXL_OVERRIDE {
    USE(opcode);
  }

  void DisassembleA32(const CodeBuffer& buffer, ptrdiff_t start,
                      ptrdiff_t end) {
    DisassembleA32Buffer(buffer.GetOffsetAddress<const uint32_t*>(start),
                         end - start);
  }

  void DisassembleT32(const CodeBuffer& buffer, ptrdiff_t start,
                      ptrdiff_t end) {
    DisassembleT32Buffer(buffer.GetOffsetAddress<const uint16_t*>(start),
                         end - start);
  }
};


TEST(t32_disassembler_limit1) {
  SETUP();

  masm.UseT32();
  START_COMPARE()
  masm.Add(r10, r11, r12);
  masm.GetBuffer()->Emit16(kLowestT32_32Opcode >> 16);
  END_COMPARE("add r10, r11, ip\n"
              "?\n");

  CLEANUP();
}


TEST(t32_disassembler_limit2) {
  SETUP();

  masm.UseT32();
  START_COMPARE()
  masm.Add(r10, r11, r12);
  masm.Add(r0, r0, r1);
  END_COMPARE("add r10, r11, ip\n"
              "add r0, r1\n");

  CLEANUP();
}


TEST(macro_assembler_orn) {
  SETUP();

  // - Identities.

  COMPARE_BOTH(Orn(r0, r1, 0),
               "mvn r0, #0\n");
  COMPARE_BOTH(Orn(r0, r0, 0xffffffff),
               "");

  // - Immediate form. This form does not need macro-assembler support
  //   for T32.

  // Use r0 as the temporary register.
  COMPARE_A32(Orn(r0, r1, 1),
              "mvn r0, #1\n"
              "orr r0, r1, r0\n");
  // Use ip as the temporary register.
  COMPARE_A32(Orn(r0, r0, 1),
              "mvn ip, #1\n"
              "orr r0, ip\n");

  //  - Too large immediate form.

  // TODO: optimize this.
  COMPARE_A32(Orn(r0, r1, 0x00ffffff),
              "mvn ip, #4278190080\n"
              "mvn r0, ip\n"
              "orr r0, r1, r0\n");
  COMPARE_T32(Orn(r0, r1, 0x00ffffff),
              "orr r0, r1, #0xff000000\n");

  COMPARE_A32(Orn(r0, r1, 0xabcd2345),
              "mov ip, #9029\n"
              "movt ip, #43981\n"
              "mvn r0, ip\n"
              "orr r0, r1, r0\n");
  COMPARE_T32(Orn(r0, r1, 0xabcd2345),
              "mov r0, #9029\n"
              "movt r0, #43981\n"
              "orn r0, r1, r0\n");

  // - Plain register form. This form does not need macro-assembler
  //   support for T32.

  // Use r0 as the temporary register.
  COMPARE_A32(Orn(r0, r1, r2),
              "mvn r0, r2\n"
              "orr r0, r1, r0\n");
  // Use r1 as the temporary register.
  COMPARE_A32(Orn(r0, r0, r1),
              "mvn r1, r1\n"
              "orr r0, r1\n");
  // Use r0 as the temporary register.
  COMPARE_A32(Orn(r0, r1, r0),
              "mvn r0, r0\n"
              "orr r0, r1, r0\n");
  // Use ip as the temporary register.
  COMPARE_A32(Orn(r0, r0, r0),
              "mvn ip, r0\n"
              "orr r0, ip\n");

  // - Shifted register form. This form does not need macro-assembler
  //   support for T32.

  // Use r0 as the temporary register.
  COMPARE_A32(Orn(r0, r1, Operand(r2, LSL, 1)),
              "mvn r0, r2, lsl #1\n"
              "orr r0, r1, r0\n");
  // Use r2 as the temporary register.
  COMPARE_A32(Orns(r0, r0, Operand(r2, LSR, 2)),
              "mvn r2, r2, lsr #2\n"
              "orrs r0, r2\n");

  // - Register shifted register form.

  // Use r0 as the temporary register.
  COMPARE_A32(Orn(r0, r1, Operand(r2, LSL, r3)),
              "mvn r0, r2, lsl r3\n"
              "orr r0, r1, r0\n");
  COMPARE_T32(Orn(r0, r1, Operand(r2, LSL, r3)),
              "lsl r0, r2, r3\n"
              "orn r0, r1, r0\n");
  // Use r2 as the temporary register.
  COMPARE_A32(Orns(r0, r0, Operand(r2, LSR, r3)),
              "mvn r2, r2, lsr r3\n"
              "orrs r0, r2\n");
  COMPARE_T32(Orns(r0, r0, Operand(r2, LSR, r3)),
              "lsr r2, r3\n"
              "orns r0, r2\n");
  // Use r3 as the temporary register.
  COMPARE_A32(Orn(r0, r0, Operand(r0, ASR, r3)),
              "mvn r3, r0, asr r3\n"
              "orr r0, r3\n");
  COMPARE_T32(Orn(r0, r0, Operand(r0, ASR, r3)),
              "asr r3, r0, r3\n"
              "orn r0, r3\n");
  CLEANUP();
}


TEST(macro_assembler_t32_rsc) {
  SETUP();

  // - Immediate form. We can always re-use `rn`.

  // No need for temporay registers.
  COMPARE_T32(Rsc(r0, r1, 1),
              "mvn r1, r1\n"
              "adc r0, r1, #1\n");
  // No need for temporay registers.
  COMPARE_T32(Rscs(r0, r0, 2),
              "mvn r0, r0\n"
              "adcs r0, #2\n");

  //  - Too large immediate form.

  // TODO: optimize this.
  COMPARE_A32(Rsc(r0, r1, 0x00ffffff),
              "mvn r0, #4278190080\n"
              "rsc r0, r1, r0\n");
  COMPARE_T32(Rscs(r0, r1, 0x00ffffff),
              "mvn r1, r1\n"
              "mvn r0, #4278190080\n"
              "adcs r0, r1, r0\n");

  COMPARE_A32(Rsc(r0, r1, 0xabcd2345),
              "mov r0, #9029\n"
              "movt r0, #43981\n"
              "rsc r0, r1, r0\n");
  COMPARE_T32(Rscs(r0, r1, 0xabcd2345),
              "mvn r1, r1\n"
              "mov r0, #9029\n"
              "movt r0, #43981\n"
              "adcs r0, r1, r0\n");

  // - Plain register form.

  // No need for temporary registers.
  COMPARE_T32(Rscs(r0, r1, r2),
              "mvn r1, r1\n"
              "adcs r0, r1, r2\n");
  // Use r0 as the temporary register.
  COMPARE_T32(Rscs(r0, r1, r1),
              "mvn r0, r1\n"
              "adcs r0, r1\n");
  // Use ip as the temporary register.
  COMPARE_T32(Rscs(r0, r0, r0),
              "mvn ip, r0\n"
              "adcs r0, ip, r0\n");

  // - Shifted register form.

  // No need for temporay registers.
  COMPARE_T32(Rsc(r0, r1, Operand(r2, LSL, 1)),
              "mvn r1, r1\n"
              "adc r0, r1, r2, lsl #1\n");
  // No need for temporay registers.
  COMPARE_T32(Rscs(r0, r1, Operand(r0, LSR, 2)),
              "mvn r1, r1\n"
              "adcs r0, r1, r0, lsr #2\n");
  // Use r0 as the temporary register.
  COMPARE_T32(Rsc(r0, r1, Operand(r1, ASR, 3)),
              "mvn r0, r1\n"
              "adc r0, r1, asr #3\n");
  // Use ip as the temporary register.
  COMPARE_T32(Rscs(r0, r0, Operand(r0, ROR, 4)),
              "mvn ip, r0\n"
              "adcs r0, ip, r0, ror #4\n");

  // - Register shifted register form. The macro-assembler handles this form in
  //   two steps. First, a shift instruction is generated from the operand. And
  //   finally the operation is reduced to its plain register form.

  COMPARE_T32(Rsc(r0, r1, Operand(r2, LSL, r3)),
              "lsl r0, r2, r3\n"
              "mvn r1, r1\n"
              "adc r0, r1, r0\n");
  // Use r0 as the temporary register.
  COMPARE_T32(Rscs(r0, r1, Operand(r1, LSR, r3)),
              "lsr r0, r1, r3\n"
              "mvn r1, r1\n"
              "adcs r0, r1, r0\n");
  // Use r2 as the temporary register.
  COMPARE_T32(Rsc(r0, r0, Operand(r2, ASR, r3)),
              "asr r2, r3\n"
              "mvn r0, r0\n"
              "adc r0, r2\n");
  // Use r3 as the temporary register.
  COMPARE_T32(Rscs(r0, r0, Operand(r0, ROR, r3)),
              "ror r3, r0, r3\n"
              "mvn r0, r0\n"
              "adcs r0, r3\n");
  // Use ip as the temporary register.
  COMPARE_T32(Rsc(r0, r0, Operand(r0, LSL, r0)),
              "lsl ip, r0, r0\n"
              "mvn r0, r0\n"
              "adc r0, ip\n");

  CLEANUP();
}


TEST(macro_assembler_t32_register_shift_register) {
  SETUP();

  COMPARE_T32(Adc(r0, r1, Operand(r2, LSL, r3)),
              "lsl r0, r2, r3\n"
              "adc r0, r1, r0\n");
  COMPARE_T32(Adcs(r0, r0, Operand(r2, LSR, r3)),
              "lsr r2, r3\n"
              "adcs r0, r2\n");
  COMPARE_T32(Add(r0, r0, Operand(r0, ASR, r3)),
              "asr r3, r0, r3\n"
              "add r0, r3\n");
  COMPARE_T32(Adds(r0, r0, Operand(r0, ROR, r0)),
              "ror ip, r0, r0\n"
              "adds r0, ip\n");

  CLEANUP();
}


TEST(macro_assembler_wide_immediate) {
  SETUP();

  COMPARE_BOTH(Adc(r0, r1, 0xbadbeef),
              "mov r0, #48879\n"
              "movt r0, #2989\n"
              "adc r0, r1, r0\n");

  COMPARE_BOTH(Add(r0, r0, 0xbadbeef),
              "mov ip, #48879\n"
              "movt ip, #2989\n"
              "add r0, ip\n");

  COMPARE_BOTH(Mov(r0, 0xbadbeef),
              "mov r0, #48879\n"
              "movt r0, #2989\n");
  COMPARE_A32(Mov(eq, r0, 0xbadbeef),
              "moveq r0, #48879\n"
              "movteq r0, #2989\n");
  COMPARE_T32(Mov(eq, r0, 0xbadbeef),
              "bne 0x0000000a\n"
              "mov r0, #48879\n"
              "movt r0, #2989\n");

  COMPARE_BOTH(Movs(r0, 0xbadbeef),
              "mov r0, #48879\n"
              "movt r0, #2989\n"
              "tst r0, r0\n");
  COMPARE_A32(Movs(eq, r0, 0xbadbeef),
              "moveq r0, #48879\n"
              "movteq r0, #2989\n"
              "tsteq r0, r0\n");
  COMPARE_T32(Movs(eq, r0, 0xbadbeef),
              "bne 0x0000000c\n"
              "mov r0, #48879\n"
              "movt r0, #2989\n"
              "tst r0, r0\n");

  COMPARE_BOTH(Mov(pc, 0xbadbeef),
              "mov ip, #48879\n"
              "movt ip, #2989\n"
              "bx ip\n");
  COMPARE_A32(Mov(eq, pc, 0xbadbeef),
              "mov ip, #48879\n"
              "movt ip, #2989\n"
              "bxeq ip\n");
  COMPARE_T32(Mov(eq, pc, 0xbadbeef),
              "bne 0x0000000c\n"
              "mov ip, #48879\n"
              "movt ip, #2989\n"
              "bx ip\n");

  CLEANUP();
}


TEST(macro_assembler_And) {
  SETUP();

  // Identities.
  COMPARE_BOTH(And(r0, r1, 0),
               "mov r0, #0\n");
  COMPARE_BOTH(And(r0, r0, 0xffffffff),
               "");
  CLEANUP();
}


TEST(macro_assembler_Bic) {
  SETUP();

  // Identities.
  COMPARE_BOTH(Bic(r0, r1, 0xffffffff),
               "mov r0, #0\n");
  COMPARE_BOTH(Bic(r0, r0, 0),
               "");
  CLEANUP();
}


TEST(macro_assembler_Orr) {
  SETUP();

  // Identities.
  COMPARE_BOTH(Orr(r0, r1, 0xffffffff),
               "mvn r0, #0\n");
  COMPARE_BOTH(Orr(r0, r0, 0),
               "");
  CLEANUP();
}


TEST(macro_assembler_InstructionCondSizeRROp) {
  SETUP();

  // Negate the immediate.
  COMPARE_T32(Orr(r0, r1, 0x00ffffff),
              "orn r0, r1, #0xff000000\n");
  COMPARE_T32(Orrs(r0, r1, 0x00ffffff),
              "orns r0, r1, #0xff000000\n");
  CLEANUP();
}


TEST(macro_assembler_InstructionCondRO) {
  SETUP();

  COMPARE_BOTH(Teq(r0, 0xbadbeef),
               "mov ip, #48879\n"
               "movt ip, #2989\n"
               "teq r0, ip\n");

  CLEANUP();
}


}  // namespace aarch32
}  // namespace vixl
