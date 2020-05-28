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
// Test infrastructure.

namespace vixl {
namespace aarch64 {

#define __ masm->
#define TEST(name) TEST_(AARCH64_CPUFeatures_##name)

class CPUFeaturesTest {
 public:
  CPUFeaturesTest(const CPUFeatures& features, const char* description)
      : description_(description),
        features_(features),
        auditor_(&decoder_, features) {}

  void Run() {
    auditor_.ResetSeenFeatures();

    // Positive test: the instruction must assemble with the specified features.
    RunWithFeatures(features_);
    // Positive test: extra features are ignored.
    RunWithFeatures(CPUFeatures::All());
#ifdef VIXL_DEBUG
#ifdef VIXL_NEGATIVE_TESTING
    // Negative tests: any one missing feature causes the Assembler to fail in
    // debug mode.
    std::vector<CPUFeatures> errors;
    for (CPUFeatures::const_iterator it = features_.begin();
         it != features_.end();
         ++it) {
      try {
        CPUFeatures reduced = features_.Without(*it);
        RunWithFeatures(reduced);
        errors.push_back(reduced);
      } catch (const std::runtime_error&) {
      }
    }
    if (!errors.empty()) {
      std::cerr << "Negative CPUFeatures test failed for:\n";
      std::cerr << "  " << description_ << ";";
      std::cerr << "  // Requires: {" << features_ << "}\n";
      std::cerr << "with feature sets:\n";
      for (std::vector<CPUFeatures>::iterator it = errors.begin();
           it != errors.end();
           ++it) {
        std::cerr << "  {" << *it << "}\n";
      }
      abort();
    }
#endif  // VIXL_NEGATIVE_TESTING
#else   // VIXL_DEBUG
    // In release mode, the {Macro}Assembler doesn't check CPUFeatures.
    RunWithFeatures(CPUFeatures::None());
#endif  // VIXL_DEBUG

    // Check that the CPUFeaturesAuditor detected the correct features.
    VIXL_CHECK(auditor_.GetSeenFeatures() == features_);
  }

  virtual void GenerateTestInstruction(MacroAssembler* masm) const = 0;

 protected:
  const char* description_;

 private:
  CPUFeatures features_;

  Decoder decoder_;
  CPUFeaturesAuditor auditor_;

  // Use a separate context (and MacroAssembler) for each test, because the
  // MacroAssembler does not guarantee any particular exception safety in
  // negative testing mode.
  void RunWithFeatures(const CPUFeatures& features) {
    // Use PositionDependentCode to allow the likes of adrp.
    MacroAssembler masm(PositionDependentCode);
    masm.SetCPUFeatures(features);
    {
      SingleEmissionCheckScope guard(&masm);
      GenerateTestInstruction(&masm);
    }
    masm.FinalizeCode();

    // Pass the generated code through the CPUFeaturesAuditor.
    VIXL_ASSERT(masm.GetBuffer()->GetSizeInBytes() == kInstructionSize);
    decoder_.Decode(masm.GetInstructionAt(0));

    // Check that the CPUFeaturesAuditor detected the correct features for this
    // instruction. A simple assertion would do, but printing the missing or
    // superfluous features is useful for debugging.
    if (auditor_.GetInstructionFeatures() != features_) {
      CPUFeatures missing =
          features_.Without(auditor_.GetInstructionFeatures());
      if (missing.Count() > 0) {
        std::cout << "Error: the auditor should have detected CPUFeatures { "
                  << missing << " }\n";
      }
      CPUFeatures extra = auditor_.GetInstructionFeatures().Without(features_);
      if (extra.Count() > 0) {
        std::cout << "Error: the auditor detected superfluous CPUFeatures { "
                  << extra << " }\n";
      }
      VIXL_ABORT();
    }
  }
};

#define STRINGIFY(x) #x

#define TEST_TEMPLATE_COMMON(FEATURES, ISA, NAME, ASM)                         \
  /* TODO: Do something with 'ISA'. */                                         \
  TEST(NAME) {                                                                 \
    class TestCase : public CPUFeaturesTest {                                  \
     public:                                                                   \
      explicit TestCase(const CPUFeatures& features)                           \
          : CPUFeaturesTest(features, STRINGIFY(ASM)) {}                       \
                                                                               \
      void GenerateTestInstruction(MacroAssembler* masm) const VIXL_OVERRIDE { \
        /* Some tests need a label. */                                         \
        Label label;                                                           \
        __ bind(&label);                                                       \
        __ ASM;                                                                \
      }                                                                        \
    };                                                                         \
    TestCase test((FEATURES));                                                 \
    test.Run();                                                                \
  }

#define TEST_TEMPLATE_A64(FEATURES, NAME, ASM) \
  TEST_TEMPLATE_COMMON(FEATURES, A64, A64_##NAME, ASM)
#define TEST_TEMPLATE_C64(FEATURES, NAME, ASM) \
  TEST_TEMPLATE_COMMON(FEATURES, C64, C64_##NAME, ASM)

#define TEST_TEMPLATE(FEATURES, NAME, ASM) \
  TEST_TEMPLATE_A64(FEATURES, NAME, ASM)   \
  TEST_TEMPLATE_C64(FEATURES, NAME, ASM)


}  // namespace aarch64
}  // namespace vixl
