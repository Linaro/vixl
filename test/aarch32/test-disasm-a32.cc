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

#define COMPARE(ASM, EXP)                                                      \
  {                                                                            \
    ptrdiff_t start = masm.GetBuffer().GetCursorOffset();                      \
    masm.ASM;                                                                  \
    ptrdiff_t end = masm.GetBuffer().GetCursorOffset();                        \
    masm.FinalizeCode();                                                       \
    std::ostringstream ss;                                                     \
    TestDisassembler disassembler(ss, 0);                                      \
    if (masm.IsT32()) {                                                        \
      disassembler.DisassembleT32(masm.GetBuffer(), start, end);               \
    } else {                                                                   \
      disassembler.DisassembleA32(masm.GetBuffer(), start, end);               \
    }                                                                          \
    masm.GetBuffer().Reset();                                                  \
    if (std::string(EXP) != ss.str()) {                                        \
      printf("\nFound:\n%sExpected:\n%s", ss.str().c_str(), EXP);              \
      abort();                                                                 \
    }                                                                          \
  }

#define COMPARE_A32(ASM, EXP)                                                  \
  masm.SetT32(false);                                                          \
  COMPARE(ASM, EXP)

#define COMPARE_T32(ASM, EXP)                                                  \
  masm.SetT32(true);                                                           \
  COMPARE(ASM, EXP)


class TestDisassembler : public Disassembler {
 public:
  TestDisassembler(std::ostream& os, uint32_t pc)  // NOLINT
      : Disassembler(os, pc) {
  }

  void DisassembleA32(const CodeBuffer& buffer, ptrdiff_t start,
                      ptrdiff_t end) {
    const uint32_t* start_addr =
        buffer.GetOffsetAddress<const uint32_t*>(start);
    const uint32_t* end_addr = buffer.GetOffsetAddress<const uint32_t*>(end);
    while (start_addr < end_addr) {
      DecodeA32(*start_addr++);
    }
  }

  void DisassembleT32(const CodeBuffer& buffer, ptrdiff_t start,
                      ptrdiff_t end) {
    const uint16_t* start_addr =
        buffer.GetOffsetAddress<const uint16_t*>(start);
    const uint16_t* end_addr = buffer.GetOffsetAddress<const uint16_t*>(end);
    while (start_addr < end_addr) {
      uint32_t value = *start_addr++ << 16;
      if (value >= kLowestT32_32Opcode) value |= *start_addr++;
      DecodeT32(value);
    }
  }

  void DecodeT32(uint32_t instruction) {
    Disassembler::DecodeT32(instruction);
    os() << "\n";
  }

  void DecodeA32(uint32_t instruction) {
    Disassembler::DecodeA32(instruction);
    os() << "\n";
  }
};

TEST(macro_assembler_orn) {
  SETUP();

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

  // - Plain register form.

  // No need for temporay registers.
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

  COMPARE_A32(Adc(r0, r1, 0xbadbeef),
              "mov r0, #48879\n"
              "movt r0, #2989\n"
              "adc r0, r1, r0\n");
  COMPARE_T32(Adc(r0, r1, 0xbadbeef),
              "mov r0, #48879\n"
              "movt r0, #2989\n"
              "adc r0, r1, r0\n");
  COMPARE_A32(Add(r0, r0, 0xbadbeef),
              "mov ip, #48879\n"
              "movt ip, #2989\n"
              "add r0, ip\n");
  COMPARE_T32(Adds(r0, r0, 0xbadbeef),
              "mov ip, #48879\n"
              "movt ip, #2989\n"
              "adds r0, ip\n");

  CLEANUP();
}


}  // namespace aarch32
}  // namespace vixl
