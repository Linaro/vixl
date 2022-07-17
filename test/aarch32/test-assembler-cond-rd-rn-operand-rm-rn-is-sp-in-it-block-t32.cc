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
#define FOREACH_INSTRUCTION(M) M(add)


// The following definitions are defined again in each generated test, therefore
// we need to place them in an anonymous namespace. It expresses that they are
// local to this file only, and the compiler is not allowed to share these types
// across test files during template instantiation. Specifically, `Operands` has
// various layouts across generated tests so it absolutely cannot be shared.

#ifdef VIXL_INCLUDE_TARGET_T32
namespace {

// Values to be passed to the assembler to produce the instruction under test.
struct Operands {
  Condition cond;
  Register rd;
  Register rn;
  Register rm;
};

// This structure contains all data needed to test one specific
// instruction.
struct TestData {
  // The `operands` field represents what to pass to the assembler to
  // produce the instruction.
  Operands operands;
  // True if we need to generate an IT instruction for this test to be valid.
  bool in_it_block;
  // The condition to give the IT instruction, this will be set to "al" by
  // default.
  Condition it_condition;
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
const TestData kTests[] =
    {{{eq, r0, r13, r0}, true, eq, "eq r0 r13 r0", "eq_r0_r13_r0"},
     {{eq, r1, r13, r1}, true, eq, "eq r1 r13 r1", "eq_r1_r13_r1"},
     {{eq, r2, r13, r2}, true, eq, "eq r2 r13 r2", "eq_r2_r13_r2"},
     {{eq, r3, r13, r3}, true, eq, "eq r3 r13 r3", "eq_r3_r13_r3"},
     {{eq, r4, r13, r4}, true, eq, "eq r4 r13 r4", "eq_r4_r13_r4"},
     {{eq, r5, r13, r5}, true, eq, "eq r5 r13 r5", "eq_r5_r13_r5"},
     {{eq, r6, r13, r6}, true, eq, "eq r6 r13 r6", "eq_r6_r13_r6"},
     {{eq, r7, r13, r7}, true, eq, "eq r7 r13 r7", "eq_r7_r13_r7"},
     {{ne, r0, r13, r0}, true, ne, "ne r0 r13 r0", "ne_r0_r13_r0"},
     {{ne, r1, r13, r1}, true, ne, "ne r1 r13 r1", "ne_r1_r13_r1"},
     {{ne, r2, r13, r2}, true, ne, "ne r2 r13 r2", "ne_r2_r13_r2"},
     {{ne, r3, r13, r3}, true, ne, "ne r3 r13 r3", "ne_r3_r13_r3"},
     {{ne, r4, r13, r4}, true, ne, "ne r4 r13 r4", "ne_r4_r13_r4"},
     {{ne, r5, r13, r5}, true, ne, "ne r5 r13 r5", "ne_r5_r13_r5"},
     {{ne, r6, r13, r6}, true, ne, "ne r6 r13 r6", "ne_r6_r13_r6"},
     {{ne, r7, r13, r7}, true, ne, "ne r7 r13 r7", "ne_r7_r13_r7"},
     {{cs, r0, r13, r0}, true, cs, "cs r0 r13 r0", "cs_r0_r13_r0"},
     {{cs, r1, r13, r1}, true, cs, "cs r1 r13 r1", "cs_r1_r13_r1"},
     {{cs, r2, r13, r2}, true, cs, "cs r2 r13 r2", "cs_r2_r13_r2"},
     {{cs, r3, r13, r3}, true, cs, "cs r3 r13 r3", "cs_r3_r13_r3"},
     {{cs, r4, r13, r4}, true, cs, "cs r4 r13 r4", "cs_r4_r13_r4"},
     {{cs, r5, r13, r5}, true, cs, "cs r5 r13 r5", "cs_r5_r13_r5"},
     {{cs, r6, r13, r6}, true, cs, "cs r6 r13 r6", "cs_r6_r13_r6"},
     {{cs, r7, r13, r7}, true, cs, "cs r7 r13 r7", "cs_r7_r13_r7"},
     {{cc, r0, r13, r0}, true, cc, "cc r0 r13 r0", "cc_r0_r13_r0"},
     {{cc, r1, r13, r1}, true, cc, "cc r1 r13 r1", "cc_r1_r13_r1"},
     {{cc, r2, r13, r2}, true, cc, "cc r2 r13 r2", "cc_r2_r13_r2"},
     {{cc, r3, r13, r3}, true, cc, "cc r3 r13 r3", "cc_r3_r13_r3"},
     {{cc, r4, r13, r4}, true, cc, "cc r4 r13 r4", "cc_r4_r13_r4"},
     {{cc, r5, r13, r5}, true, cc, "cc r5 r13 r5", "cc_r5_r13_r5"},
     {{cc, r6, r13, r6}, true, cc, "cc r6 r13 r6", "cc_r6_r13_r6"},
     {{cc, r7, r13, r7}, true, cc, "cc r7 r13 r7", "cc_r7_r13_r7"},
     {{mi, r0, r13, r0}, true, mi, "mi r0 r13 r0", "mi_r0_r13_r0"},
     {{mi, r1, r13, r1}, true, mi, "mi r1 r13 r1", "mi_r1_r13_r1"},
     {{mi, r2, r13, r2}, true, mi, "mi r2 r13 r2", "mi_r2_r13_r2"},
     {{mi, r3, r13, r3}, true, mi, "mi r3 r13 r3", "mi_r3_r13_r3"},
     {{mi, r4, r13, r4}, true, mi, "mi r4 r13 r4", "mi_r4_r13_r4"},
     {{mi, r5, r13, r5}, true, mi, "mi r5 r13 r5", "mi_r5_r13_r5"},
     {{mi, r6, r13, r6}, true, mi, "mi r6 r13 r6", "mi_r6_r13_r6"},
     {{mi, r7, r13, r7}, true, mi, "mi r7 r13 r7", "mi_r7_r13_r7"},
     {{pl, r0, r13, r0}, true, pl, "pl r0 r13 r0", "pl_r0_r13_r0"},
     {{pl, r1, r13, r1}, true, pl, "pl r1 r13 r1", "pl_r1_r13_r1"},
     {{pl, r2, r13, r2}, true, pl, "pl r2 r13 r2", "pl_r2_r13_r2"},
     {{pl, r3, r13, r3}, true, pl, "pl r3 r13 r3", "pl_r3_r13_r3"},
     {{pl, r4, r13, r4}, true, pl, "pl r4 r13 r4", "pl_r4_r13_r4"},
     {{pl, r5, r13, r5}, true, pl, "pl r5 r13 r5", "pl_r5_r13_r5"},
     {{pl, r6, r13, r6}, true, pl, "pl r6 r13 r6", "pl_r6_r13_r6"},
     {{pl, r7, r13, r7}, true, pl, "pl r7 r13 r7", "pl_r7_r13_r7"},
     {{vs, r0, r13, r0}, true, vs, "vs r0 r13 r0", "vs_r0_r13_r0"},
     {{vs, r1, r13, r1}, true, vs, "vs r1 r13 r1", "vs_r1_r13_r1"},
     {{vs, r2, r13, r2}, true, vs, "vs r2 r13 r2", "vs_r2_r13_r2"},
     {{vs, r3, r13, r3}, true, vs, "vs r3 r13 r3", "vs_r3_r13_r3"},
     {{vs, r4, r13, r4}, true, vs, "vs r4 r13 r4", "vs_r4_r13_r4"},
     {{vs, r5, r13, r5}, true, vs, "vs r5 r13 r5", "vs_r5_r13_r5"},
     {{vs, r6, r13, r6}, true, vs, "vs r6 r13 r6", "vs_r6_r13_r6"},
     {{vs, r7, r13, r7}, true, vs, "vs r7 r13 r7", "vs_r7_r13_r7"},
     {{vc, r0, r13, r0}, true, vc, "vc r0 r13 r0", "vc_r0_r13_r0"},
     {{vc, r1, r13, r1}, true, vc, "vc r1 r13 r1", "vc_r1_r13_r1"},
     {{vc, r2, r13, r2}, true, vc, "vc r2 r13 r2", "vc_r2_r13_r2"},
     {{vc, r3, r13, r3}, true, vc, "vc r3 r13 r3", "vc_r3_r13_r3"},
     {{vc, r4, r13, r4}, true, vc, "vc r4 r13 r4", "vc_r4_r13_r4"},
     {{vc, r5, r13, r5}, true, vc, "vc r5 r13 r5", "vc_r5_r13_r5"},
     {{vc, r6, r13, r6}, true, vc, "vc r6 r13 r6", "vc_r6_r13_r6"},
     {{vc, r7, r13, r7}, true, vc, "vc r7 r13 r7", "vc_r7_r13_r7"},
     {{hi, r0, r13, r0}, true, hi, "hi r0 r13 r0", "hi_r0_r13_r0"},
     {{hi, r1, r13, r1}, true, hi, "hi r1 r13 r1", "hi_r1_r13_r1"},
     {{hi, r2, r13, r2}, true, hi, "hi r2 r13 r2", "hi_r2_r13_r2"},
     {{hi, r3, r13, r3}, true, hi, "hi r3 r13 r3", "hi_r3_r13_r3"},
     {{hi, r4, r13, r4}, true, hi, "hi r4 r13 r4", "hi_r4_r13_r4"},
     {{hi, r5, r13, r5}, true, hi, "hi r5 r13 r5", "hi_r5_r13_r5"},
     {{hi, r6, r13, r6}, true, hi, "hi r6 r13 r6", "hi_r6_r13_r6"},
     {{hi, r7, r13, r7}, true, hi, "hi r7 r13 r7", "hi_r7_r13_r7"},
     {{ls, r0, r13, r0}, true, ls, "ls r0 r13 r0", "ls_r0_r13_r0"},
     {{ls, r1, r13, r1}, true, ls, "ls r1 r13 r1", "ls_r1_r13_r1"},
     {{ls, r2, r13, r2}, true, ls, "ls r2 r13 r2", "ls_r2_r13_r2"},
     {{ls, r3, r13, r3}, true, ls, "ls r3 r13 r3", "ls_r3_r13_r3"},
     {{ls, r4, r13, r4}, true, ls, "ls r4 r13 r4", "ls_r4_r13_r4"},
     {{ls, r5, r13, r5}, true, ls, "ls r5 r13 r5", "ls_r5_r13_r5"},
     {{ls, r6, r13, r6}, true, ls, "ls r6 r13 r6", "ls_r6_r13_r6"},
     {{ls, r7, r13, r7}, true, ls, "ls r7 r13 r7", "ls_r7_r13_r7"},
     {{ge, r0, r13, r0}, true, ge, "ge r0 r13 r0", "ge_r0_r13_r0"},
     {{ge, r1, r13, r1}, true, ge, "ge r1 r13 r1", "ge_r1_r13_r1"},
     {{ge, r2, r13, r2}, true, ge, "ge r2 r13 r2", "ge_r2_r13_r2"},
     {{ge, r3, r13, r3}, true, ge, "ge r3 r13 r3", "ge_r3_r13_r3"},
     {{ge, r4, r13, r4}, true, ge, "ge r4 r13 r4", "ge_r4_r13_r4"},
     {{ge, r5, r13, r5}, true, ge, "ge r5 r13 r5", "ge_r5_r13_r5"},
     {{ge, r6, r13, r6}, true, ge, "ge r6 r13 r6", "ge_r6_r13_r6"},
     {{ge, r7, r13, r7}, true, ge, "ge r7 r13 r7", "ge_r7_r13_r7"},
     {{lt, r0, r13, r0}, true, lt, "lt r0 r13 r0", "lt_r0_r13_r0"},
     {{lt, r1, r13, r1}, true, lt, "lt r1 r13 r1", "lt_r1_r13_r1"},
     {{lt, r2, r13, r2}, true, lt, "lt r2 r13 r2", "lt_r2_r13_r2"},
     {{lt, r3, r13, r3}, true, lt, "lt r3 r13 r3", "lt_r3_r13_r3"},
     {{lt, r4, r13, r4}, true, lt, "lt r4 r13 r4", "lt_r4_r13_r4"},
     {{lt, r5, r13, r5}, true, lt, "lt r5 r13 r5", "lt_r5_r13_r5"},
     {{lt, r6, r13, r6}, true, lt, "lt r6 r13 r6", "lt_r6_r13_r6"},
     {{lt, r7, r13, r7}, true, lt, "lt r7 r13 r7", "lt_r7_r13_r7"},
     {{gt, r0, r13, r0}, true, gt, "gt r0 r13 r0", "gt_r0_r13_r0"},
     {{gt, r1, r13, r1}, true, gt, "gt r1 r13 r1", "gt_r1_r13_r1"},
     {{gt, r2, r13, r2}, true, gt, "gt r2 r13 r2", "gt_r2_r13_r2"},
     {{gt, r3, r13, r3}, true, gt, "gt r3 r13 r3", "gt_r3_r13_r3"},
     {{gt, r4, r13, r4}, true, gt, "gt r4 r13 r4", "gt_r4_r13_r4"},
     {{gt, r5, r13, r5}, true, gt, "gt r5 r13 r5", "gt_r5_r13_r5"},
     {{gt, r6, r13, r6}, true, gt, "gt r6 r13 r6", "gt_r6_r13_r6"},
     {{gt, r7, r13, r7}, true, gt, "gt r7 r13 r7", "gt_r7_r13_r7"},
     {{le, r0, r13, r0}, true, le, "le r0 r13 r0", "le_r0_r13_r0"},
     {{le, r1, r13, r1}, true, le, "le r1 r13 r1", "le_r1_r13_r1"},
     {{le, r2, r13, r2}, true, le, "le r2 r13 r2", "le_r2_r13_r2"},
     {{le, r3, r13, r3}, true, le, "le r3 r13 r3", "le_r3_r13_r3"},
     {{le, r4, r13, r4}, true, le, "le r4 r13 r4", "le_r4_r13_r4"},
     {{le, r5, r13, r5}, true, le, "le r5 r13 r5", "le_r5_r13_r5"},
     {{le, r6, r13, r6}, true, le, "le r6 r13 r6", "le_r6_r13_r6"},
     {{le, r7, r13, r7}, true, le, "le r7 r13 r7", "le_r7_r13_r7"}};

// These headers each contain an array of `TestResult` with the reference output
// values. The reference arrays are names `kReference{mnemonic}`.
#include "aarch32/traces/assembler-cond-rd-rn-operand-rm-rn-is-sp-in-it-block-add-t32.h"


// The maximum number of errors to report in detail for each test.
const unsigned kErrorReportLimit = 8;

typedef void (MacroAssembler::*Fn)(Condition cond,
                                   Register rd,
                                   Register rn,
                                   const Operand& op);

void TestHelper(Fn instruction,
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
    Register rm = kTests[i].operands.rm;
    Operand op(rm);

    int32_t start = masm.GetCursorOffset();
    {
      // We never generate more that 4 bytes, as IT instructions are only
      // allowed for narrow encodings.
      ExactAssemblyScope scope(&masm, 4, ExactAssemblyScope::kMaximumSize);
      if (kTests[i].in_it_block) {
        masm.it(kTests[i].it_condition);
      }
      (masm.*instruction)(cond, rd, rn, op);
    }
    int32_t end = masm.GetCursorOffset();

    const byte* result_ptr =
        masm.GetBuffer()->GetOffsetAddress<const byte*>(start);
    VIXL_ASSERT(start < end);
    uint32_t result_size = end - start;

    if (Test::generate_test_trace()) {
      // Print the result bytes.
      printf("const byte kInstruction_%s_%s[] = {\n",
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
      if (kTests[i].in_it_block) {
        printf(" // It %s; %s %s\n};\n",
               kTests[i].it_condition.GetName(),
               mnemonic,
               kTests[i].operands_description);
      } else {
        printf(" // %s %s\n};\n", mnemonic, kTests[i].operands_description);
      }
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
    printf("const TestResult kReference%s[] = {\n", mnemonic);
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
    // Crash if the test failed.
    VIXL_CHECK(total_error_count == 0);
  }
}

// Instantiate tests for each instruction in the list.
#define TEST(mnemonic)                                                      \
  void Test_##mnemonic() {                                                  \
    TestHelper(&MacroAssembler::mnemonic, #mnemonic, kReference##mnemonic); \
  }                                                                         \
  Test test_##mnemonic(                                                     \
      "AARCH32_ASSEMBLER_COND_RD_RN_OPERAND_RM_RN_IS_SP_IN_IT_"             \
      "BLOCK_" #mnemonic "_T32",                                            \
      &Test_##mnemonic);
FOREACH_INSTRUCTION(TEST)
#undef TEST

}  // namespace
#endif

}  // namespace aarch32
}  // namespace vixl
