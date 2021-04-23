// Copyright 2020, VIXL authors
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

#include <cstring>
#include <regex>

#include "assembler-base-vixl.h"
#include "tasm/tasm-assembler.h"
#include "test-runner.h"
#include "aarch64/disasm-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"
#include "aarch64/simulator-aarch64.h"

namespace vixl {
namespace aarch64 {
namespace tasm {

// Currently this class only passes whole disassembled instruction to
// TextAssembler. It can be used as a visitor node in Decoder object.
class Reassembler : public Disassembler {
 public:
  explicit Reassembler(TextAssembler* tasm) : tasm_(tasm) {}

 protected:
  virtual void ProcessOutput(const Instruction* instr) override {
    USE(instr);
    std::regex imm_reg = std::regex("(.*)\\s\\(\\d+\\)");
    std::regex addr_reg = std::regex("(.*)\\s\\(addr 0x[\\da-fA-F]+\\)");
    std::string str_instr = GetOutput();
    std::smatch reg_match;
    // Discard disassembler hex immediate value translation in arithmetic
    // instructions, for example #0xff (255).
    // TODO: handle more cases with other instructions classes.
    if (std::regex_match(str_instr, reg_match, imm_reg)) {
      tasm_->Assemble(reg_match.str(1));
    } else if (std::regex_match(str_instr, reg_match, addr_reg)) {
      tasm_->Assemble(reg_match.str(1));
    } else {
      tasm_->Assemble(str_instr);
    }
  }

 private:
  TextAssembler* tasm_;
};

#define __ masm.

#define TEST(name) TEST_(AARCH64_REASM_##name)

#define SETUP()                                       \
  MacroAssembler masm;                                \
  masm.GetCPUFeatures()->Combine(CPUFeatures::All()); \
  CodeBuffer* masm_cb = masm.GetBuffer()

#define RUN()                                                    \
  {                                                              \
    Decoder decoder;                                             \
    Simulator simulator(&decoder);                               \
    masm.FinalizeCode();                                         \
    simulator.RunFrom(masm_cb->GetStartAddress<Instruction*>()); \
  }

#define TASM_SETUP()                                     \
  TextAssembler tasm;                                    \
  CodeBuffer* tasm_cb = tasm.GetBuffer();                \
  CodeBufferCheckScope guard(tasm.AsAssemblerBase(),     \
                             masm_cb->GetSizeInBytes()); \
  tasm.GetCPUFeatures()->Combine(CPUFeatures::All())

#define REASSEMBLE()                                       \
  TASM_SETUP();                                            \
  Reassembler reasm(&tasm);                                \
  Decoder decoder;                                         \
  decoder.AppendVisitor(&reasm);                           \
  decoder.Decode(masm_cb->GetStartAddress<Instruction*>(), \
                 masm_cb->GetEndAddress<Instruction*>());  \
  tasm.FinalizeCode()

#define REASSEMBLE_AND_COMPARE()                        \
  REASSEMBLE();                                         \
  bool equal = true;                                    \
  size_t cap = masm_cb->GetCapacity();                  \
  if (cap == tasm_cb->GetCapacity()) {                  \
    if (memcmp(masm_cb->GetStartAddress<const byte*>(), \
               tasm_cb->GetStartAddress<const byte*>(), \
               cap) != 0) {                             \
      equal = false;                                    \
    }                                                   \
  } else {                                              \
    equal = false;                                      \
  }                                                     \
  VIXL_CHECK(equal)
}
}
}  // namespace vixl::aarch64::tasm