// Copyright 2022, VIXL authors
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

#define TEST(name) TEST_(AARCH64_IOR_##name)

#define SETUP()                                       \
  MacroAssembler masm;                                \
  masm.GetCPUFeatures()->Combine(CPUFeatures::All()); \
  masm.SetGenerateSimulatorCode(true);                \
  Decoder decoder;                                    \
  Disassembler disasm;                                \
  IOReporter ior;                                     \
  decoder.AppendVisitor(&disasm);                     \
  decoder.AppendVisitor(&ior)

#define CLEANUP()

#define CMP_IO(ASM, DST, SRC)                                              \
  do {                                                                     \
    masm.Reset();                                                          \
    ior.Reset();                                                           \
    {                                                                      \
      ExactAssemblyScope guard(&masm, kInstructionSize);                   \
      masm.ASM;                                                            \
    }                                                                      \
    masm.FinalizeCode();                                                   \
    decoder.Decode(masm.GetBuffer()->GetStartAddress<Instruction*>());     \
    std::unordered_set<CPURegister> dst = ior.GetDestinationRegisters();   \
    std::unordered_set<CPURegister> dst_expected(DST);                     \
    std::unordered_set<CPURegister> src = ior.GetSourceRegisters();        \
    std::unordered_set<CPURegister> src_expected(SRC);                     \
    if ((dst != dst_expected) || (src != src_expected)) {                  \
      uint32_t encoding = *masm.GetBuffer()->GetStartAddress<uint32_t*>(); \
      printf("\nInstruction: %s (0x%08" PRIx32 ")\n",                      \
             disasm.GetOutput(),                                           \
             encoding);                                                    \
      if (dst != dst_expected) {                                           \
        printf(                                                            \
            "Mismatched destination registers.\n"                          \
            "Expected: %s\nFound:    %s\n",                                \
            UnorderedSetAsString(dst_expected).c_str(),                    \
            UnorderedSetAsString(dst).c_str());                            \
      }                                                                    \
      if (src != src_expected) {                                           \
        printf(                                                            \
            "Mismatched source registers.\n"                               \
            "Expected: %s\nFound:    %s\n",                                \
            UnorderedSetAsString(src_expected).c_str(),                    \
            UnorderedSetAsString(src).c_str());                            \
      }                                                                    \
      abort();                                                             \
    }                                                                      \
  } while (0)

static std::string UnorderedSetAsString(
    const std::unordered_set<vixl::aarch64::CPURegister>& s) {
  std::string result;
  for (const vixl::aarch64::CPURegister& i : s) {
    result += i.GetArchitecturalName();
    result += " ";
  }
  return result;
}
