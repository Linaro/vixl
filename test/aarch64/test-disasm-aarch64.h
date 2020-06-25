// Copyright 2019, VIXL authors
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

namespace vixl {
namespace aarch64 {

#define TEST(name) TEST_(AARCH64_DISASM_##name)

#define SETUP_COMMON()                                        \
  MacroAssembler masm;                                        \
  /* Use PositionDependentCode to allow the likes of adrp. */ \
  masm.SetPic(PositionDependentCode);                         \
  masm.GetCPUFeatures()->Combine(CPUFeatures::All());         \
  Decoder decoder;                                            \
  Disassembler disasm;                                        \
  decoder.AppendVisitor(&disasm)

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64
// Run tests with the simulator.
#define SETUP()   \
  SETUP_COMMON(); \
  masm.SetGenerateSimulatorCode(true)

#else  // ifdef VIXL_INCLUDE_SIMULATOR_AARCH64.
#define SETUP()   \
  SETUP_COMMON(); \
  masm.SetGenerateSimulatorCode(false)

#endif  // ifdef VIXL_INCLUDE_SIMULATOR_AARCH64.

// A conservative limit for the size of the code that we generate in these
// tests.
#define MAX_SIZE_GENERATED 1024

#define DISASSEMBLE()                                                       \
  do {                                                                      \
    printf("----\n");                                                       \
    PrintDisassembler print_disasm(stdout);                                 \
    Instruction* start = masm.GetBuffer()->GetStartAddress<Instruction*>(); \
    Instruction* end = masm.GetBuffer()->GetEndAddress<Instruction*>();     \
    print_disasm.DisassembleBuffer(start, end, masm.GetISAMap());           \
  } while (0)

#define COMPARE_ISA(ASM, ISA, EXP)                                          \
  do {                                                                      \
    masm.Reset();                                                           \
    {                                                                       \
      ISAScope isa(&masm, ISA);                                             \
      ExactAssemblyScope guard(&masm,                                       \
                               MAX_SIZE_GENERATED,                          \
                               ExactAssemblyScope::kMaximumSize);           \
      masm.ASM;                                                             \
    }                                                                       \
    masm.FinalizeCode();                                                    \
    decoder.Decode(masm.GetBuffer()->GetStartAddress<Instruction*>(), ISA); \
    uint32_t encoding = *masm.GetBuffer()->GetStartAddress<uint32_t*>();    \
    if (strcmp(disasm.GetOutput(), EXP) != 0) {                             \
      printf("\nEncoding: %08" PRIx32 "\nExpected: %s\nFound:    %s\n",     \
             encoding,                                                      \
             EXP,                                                           \
             disasm.GetOutput());                                           \
      abort();                                                              \
    }                                                                       \
    if (Test::disassemble()) DISASSEMBLE();                                 \
  } while (0)

#define COMPARE_PREFIX_ISA(ASM, ISA, EXP)                                   \
  do {                                                                      \
    masm.Reset();                                                           \
    {                                                                       \
      ISAScope isa(&masm, ISA);                                             \
      ExactAssemblyScope guard(&masm,                                       \
                               MAX_SIZE_GENERATED,                          \
                               ExactAssemblyScope::kMaximumSize);           \
      masm.ASM;                                                             \
    }                                                                       \
    masm.FinalizeCode();                                                    \
    decoder.Decode(masm.GetBuffer()->GetStartAddress<Instruction*>(), ISA); \
    uint32_t encoding = *masm.GetBuffer()->GetStartAddress<uint32_t*>();    \
    if (strncmp(disasm.GetOutput(), EXP, strlen(EXP)) != 0) {               \
      printf("\nEncoding: %08" PRIx32 "\nExpected: %s...\nFound:    %s\n",  \
             encoding,                                                      \
             EXP,                                                           \
             disasm.GetOutput());                                           \
      abort();                                                              \
    }                                                                       \
    if (Test::disassemble()) DISASSEMBLE();                                 \
  } while (0)

#define COMPARE_MACRO_BASE(ASM, ISA, EXP)                   \
  masm.Reset();                                             \
  {                                                         \
    ISAScope isa(&masm, ISA);                               \
    masm.ASM;                                               \
    masm.FinalizeCode();                                    \
  }                                                         \
  std::string res;                                          \
                                                            \
  Instruction* instruction =                                \
      masm.GetBuffer()->GetStartAddress<Instruction*>();    \
  Instruction* end = masm.GetCursorAddress<Instruction*>(); \
  while (instruction != end) {                              \
    decoder.Decode(instruction, ISA);                       \
    res.append(disasm.GetOutput());                         \
    instruction = instruction->GetNextInstruction();        \
    if (instruction != end) {                               \
      res.append("\n");                                     \
    }                                                       \
  }

#define COMPARE_MACRO_ISA(ASM, ISA, EXP)                        \
  do {                                                          \
    COMPARE_MACRO_BASE(ASM, ISA, EXP)                           \
    if (strcmp(res.c_str(), EXP) != 0) {                        \
      printf("Expected: %s\nFound:    %s\n", EXP, res.c_str()); \
      abort();                                                  \
    }                                                           \
    if (Test::disassemble()) DISASSEMBLE();                     \
  } while (0)

#define COMPARE_MACRO_PREFIX_ISA(ASM, ISA, EXP)                             \
  do {                                                                      \
    COMPARE_MACRO_BASE(ASM, ISA, EXP)                                       \
    if (strncmp(res.c_str(), EXP, strlen(EXP)) != 0) {                      \
      printf("Expected (prefix): %s...\nFound:    %s\n", EXP, res.c_str()); \
      abort();                                                              \
    }                                                                       \
    if (Test::disassemble()) DISASSEMBLE();                                 \
  } while (0)


// Convenience wrappers to avoid verbose ISA specification.

// A64
#define COMPARE_A64(ASM, EXP) COMPARE_ISA(ASM, ISA::A64, EXP)
#define COMPARE_PREFIX_A64(ASM, EXP) COMPARE_PREFIX_ISA(ASM, ISA::A64, EXP)
#define COMPARE_MACRO_A64(ASM, EXP) COMPARE_MACRO_ISA(ASM, ISA::A64, EXP)
#define COMPARE_MACRO_PREFIX_A64(ASM, EXP) \
  COMPARE_MACRO_PREFIX_ISA(ASM, ISA::A64, EXP)

// C64
#define COMPARE_C64(ASM, EXP) COMPARE_ISA(ASM, ISA::C64, EXP)
#define COMPARE_PREFIX_C64(ASM, EXP) COMPARE_PREFIX_ISA(ASM, ISA::C64, EXP)
#define COMPARE_MACRO_C64(ASM, EXP) COMPARE_MACRO_ISA(ASM, ISA::C64, EXP)
#define COMPARE_MACRO_PREFIX_C64(ASM, EXP) \
  COMPARE_MACRO_PREFIX_ISA(ASM, ISA::C64, EXP)

// Instructions common to both A64 and C64.
// TODO: Also check that the encodings are the same.
#define COMPARE_MORELLO(ASM, EXP) \
  COMPARE_A64(ASM, EXP);          \
  COMPARE_C64(ASM, EXP)
#define COMPARE_PREFIX_MORELLO(ASM, EXP) \
  COMPARE_PREFIX_A64(ASM, EXP);          \
  COMPARE_PREFIX_C64(ASM, EXP)
#define COMPARE_MACRO_MORELLO(ASM, EXP) \
  COMPARE_MACRO_A64(ASM, EXP);          \
  COMPARE_MACRO_C64(ASM, EXP)
#define COMPARE_MACRO_PREFIX_MORELLO(ASM, EXP) \
  COMPARE_MACRO_PREFIX_A64(ASM, EXP);          \
  COMPARE_MACRO_PREFIX_C64(ASM, EXP)

// To avoid changing many existing tests, operate on A64 by default.
// TODO: Make this work in both A64 and C64, then remove the _MORELLO variants.
#define COMPARE(ASM, EXP) COMPARE_A64(ASM, EXP)
#define COMPARE_PREFIX(ASM, EXP) COMPARE_PREFIX_A64(ASM, EXP)
#define COMPARE_MACRO(ASM, EXP) COMPARE_MACRO_A64(ASM, EXP)
#define COMPARE_MACRO_PREFIX(ASM, EXP) COMPARE_MACRO_PREFIX_A64(ASM, EXP)

// Unallocated encodings disassemble as "unallocated", followed by an optional
// instruction class in parentheses. The instruction classes are unstable, so we
// only check the prefix.
#define CHECK_UNALLOCATED_MORELLO(ASM) \
  COMPARE_PREFIX_MORELLO(ASM, "unallocated")

#define CLEANUP()

// Some tests need to do things that aren't possible in a single call. This
// MacroAssembler replacement allows that, via callbacks.
class DisasmTestUtilMacroAssembler : public MacroAssembler {
 public:
  // Call `fn` with only the specified features enabled.
  // Disassembler tests, by default, run with all features enabled. This allows
  // tests to be more specific, in case the absence of a feature can result in
  // different behaviour.
  void WithCPUFeatures(std::function<void()> fn, CPUFeatures cpu) {
    CPUFeaturesScope scope(this);
    scope.SetCPUFeatures(cpu);
    fn();
  }

  // Call `fn` with labels bound immediately before and after it, and bound in
  // the specified ISA.
  void WithLabels(std::function<void(Label*, Label*)> fn, ISA label_isa) {
    Label before;
    Label after;
    {
      vixl::aarch64::ISAScope isa(this, label_isa);
      bind(&before);
    }
    fn(&before, &after);
    {
      vixl::aarch64::ISAScope isa(this, label_isa);
      bind(&after);
    }
  }

  void WithA64LabelBefore(std::function<void(Label*)> fn) {
    WithLabels([fn](Label* before, Label*) { fn(before); }, ISA::A64);
  }
  void WithC64LabelBefore(std::function<void(Label*)> fn) {
    WithLabels([fn](Label* before, Label*) { fn(before); }, ISA::C64);
  }
  void WithDataLabelBefore(std::function<void(Label*)> fn) {
    WithLabels([fn](Label* before, Label*) { fn(before); }, ISA::Data);
  }
  void WithA64LabelAfter(std::function<void(Label*)> fn) {
    WithLabels([fn](Label*, Label* after) { fn(after); }, ISA::A64);
  }
  void WithC64LabelAfter(std::function<void(Label*)> fn) {
    WithLabels([fn](Label*, Label* after) { fn(after); }, ISA::C64);
  }
  void WithDataLabelAfter(std::function<void(Label*)> fn) {
    WithLabels([fn](Label*, Label* after) { fn(after); }, ISA::Data);
  }
};

}  // namespace aarch64
}  // namespace vixl
