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
// test/aarch32/config/template-simulator-aarch32.cc.in template file using
// tools/generate_tests.py.
//
// PLEASE DO NOT EDIT.
// -----------------------------------------------------------------------------


#include "test-runner.h"

#include "test-utils.h"
#include "test-utils-aarch32.h"

#include "aarch32/assembler-aarch32.h"
#include "aarch32/disasm-aarch32.h"
#include "aarch32/macro-assembler-aarch32.h"

#define __ masm.
#define BUF_SIZE (4096)

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH32
// Run tests with the simulator.

#define SETUP() MacroAssembler masm(BUF_SIZE)

#define START() masm.GetBuffer()->Reset()

#define END() \
  __ Hlt(0);  \
  __ FinalizeCode();

// TODO: Run the tests in the simulator.
#define RUN()

#else  // ifdef VIXL_INCLUDE_SIMULATOR_AARCH32.

#define SETUP()                  \
  MacroAssembler masm(BUF_SIZE); \
  UseScratchRegisterScope harness_scratch;

#define START()                 \
  harness_scratch.Open(&masm);  \
  harness_scratch.ExcludeAll(); \
  masm.GetBuffer()->Reset();    \
  __ Push(r4);                  \
  __ Push(r5);                  \
  __ Push(r6);                  \
  __ Push(r7);                  \
  __ Push(r8);                  \
  __ Push(r9);                  \
  __ Push(r10);                 \
  __ Push(r11);                 \
  __ Push(lr);                  \
  harness_scratch.Include(ip);

#define END()                  \
  harness_scratch.Exclude(ip); \
  __ Pop(lr);                  \
  __ Pop(r11);                 \
  __ Pop(r10);                 \
  __ Pop(r9);                  \
  __ Pop(r8);                  \
  __ Pop(r7);                  \
  __ Pop(r6);                  \
  __ Pop(r5);                  \
  __ Pop(r4);                  \
  __ Bx(lr);                   \
  __ FinalizeCode();           \
  harness_scratch.Close();

#define RUN()                                                 \
  {                                                           \
    int pcs_offset = masm.IsUsingT32() ? 1 : 0;               \
    masm.GetBuffer()->SetExecutable();                        \
    ExecuteMemory(masm.GetBuffer()->GetStartAddress<byte*>(), \
                  masm.GetSizeOfCodeGenerated(),              \
                  pcs_offset);                                \
    masm.GetBuffer()->SetWritable();                          \
  }

#endif  // ifdef VIXL_INCLUDE_SIMULATOR_AARCH32

namespace vixl {
namespace aarch32 {

// List of instruction encodings:
#define FOREACH_INSTRUCTION(M) \
  M(Mov)                       \
  M(Movt)


// The following definitions are defined again in each generated test, therefore
// we need to place them in an anonymous namespace. It expresses that they are
// local to this file only, and the compiler is not allowed to share these types
// across test files during template instantiation. Specifically, `Operands` and
// `Inputs` have various layouts across generated tests so they absolutely
// cannot be shared.

#ifdef VIXL_INCLUDE_TARGET_T32
namespace {

// Values to be passed to the assembler to produce the instruction under test.
struct Operands {
  Condition cond;
  Register rd;
  uint32_t immediate;
};

// Input data to feed to the instruction.
struct Inputs {
  uint32_t apsr;
  uint32_t rd;
};

// This structure contains all input data needed to test one specific encoding.
// It used to generate a loop over an instruction.
struct TestLoopData {
  // The `operands` fields represents the values to pass to the assembler to
  // produce the instruction.
  Operands operands;
  // Description of the operands, used for error reporting.
  const char* operands_description;
  // Unique identifier, used for generating traces.
  const char* identifier;
  // Array of values to be fed to the instruction.
  size_t input_size;
  const Inputs* inputs;
};

static const Inputs kCondition[] = {{NFlag, 0xabababab},
                                    {ZFlag, 0xabababab},
                                    {CFlag, 0xabababab},
                                    {VFlag, 0xabababab},
                                    {NZFlag, 0xabababab},
                                    {NCFlag, 0xabababab},
                                    {NVFlag, 0xabababab},
                                    {ZCFlag, 0xabababab},
                                    {ZVFlag, 0xabababab},
                                    {CVFlag, 0xabababab},
                                    {NZCFlag, 0xabababab},
                                    {NZVFlag, 0xabababab},
                                    {NCVFlag, 0xabababab},
                                    {ZCVFlag, 0xabababab},
                                    {NZCVFlag, 0xabababab}};

static const Inputs kModifiedImmediate[] =
    {{NoFlag, 0x00000000}, {NoFlag, 0x00000001}, {NoFlag, 0x00000002},
     {NoFlag, 0x00000020}, {NoFlag, 0x0000007d}, {NoFlag, 0x0000007e},
     {NoFlag, 0x0000007f}, {NoFlag, 0x00007ffd}, {NoFlag, 0x00007ffe},
     {NoFlag, 0x00007fff}, {NoFlag, 0x33333333}, {NoFlag, 0x55555555},
     {NoFlag, 0x7ffffffd}, {NoFlag, 0x7ffffffe}, {NoFlag, 0x7fffffff},
     {NoFlag, 0x80000000}, {NoFlag, 0x80000001}, {NoFlag, 0xaaaaaaaa},
     {NoFlag, 0xcccccccc}, {NoFlag, 0xffff8000}, {NoFlag, 0xffff8001},
     {NoFlag, 0xffff8002}, {NoFlag, 0xffff8003}, {NoFlag, 0xffffff80},
     {NoFlag, 0xffffff81}, {NoFlag, 0xffffff82}, {NoFlag, 0xffffff83},
     {NoFlag, 0xffffffe0}, {NoFlag, 0xfffffffd}, {NoFlag, 0xfffffffe},
     {NoFlag, 0xffffffff}};


// A loop will be generated for each element of this array.
const TestLoopData kTests[] = {{{eq, r0, 0x0},
                                "eq r0 0x0",
                                "Condition_eq_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{ne, r0, 0x0},
                                "ne r0 0x0",
                                "Condition_ne_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{cs, r0, 0x0},
                                "cs r0 0x0",
                                "Condition_cs_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{cc, r0, 0x0},
                                "cc r0 0x0",
                                "Condition_cc_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{mi, r0, 0x0},
                                "mi r0 0x0",
                                "Condition_mi_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{pl, r0, 0x0},
                                "pl r0 0x0",
                                "Condition_pl_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{vs, r0, 0x0},
                                "vs r0 0x0",
                                "Condition_vs_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{vc, r0, 0x0},
                                "vc r0 0x0",
                                "Condition_vc_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{hi, r0, 0x0},
                                "hi r0 0x0",
                                "Condition_hi_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{ls, r0, 0x0},
                                "ls r0 0x0",
                                "Condition_ls_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{ge, r0, 0x0},
                                "ge r0 0x0",
                                "Condition_ge_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{lt, r0, 0x0},
                                "lt r0 0x0",
                                "Condition_lt_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{gt, r0, 0x0},
                                "gt r0 0x0",
                                "Condition_gt_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{le, r0, 0x0},
                                "le r0 0x0",
                                "Condition_le_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{al, r0, 0x0},
                                "al r0 0x0",
                                "Condition_al_r0_0x0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{al, r0, 0x0000},
                                "al r0 0x0000",
                                "ModifiedImmediate_al_r0_0x0000",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x0001},
                                "al r0 0x0001",
                                "ModifiedImmediate_al_r0_0x0001",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x0002},
                                "al r0 0x0002",
                                "ModifiedImmediate_al_r0_0x0002",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x0020},
                                "al r0 0x0020",
                                "ModifiedImmediate_al_r0_0x0020",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x007d},
                                "al r0 0x007d",
                                "ModifiedImmediate_al_r0_0x007d",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x007e},
                                "al r0 0x007e",
                                "ModifiedImmediate_al_r0_0x007e",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x007f},
                                "al r0 0x007f",
                                "ModifiedImmediate_al_r0_0x007f",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x7ffd},
                                "al r0 0x7ffd",
                                "ModifiedImmediate_al_r0_0x7ffd",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x7ffe},
                                "al r0 0x7ffe",
                                "ModifiedImmediate_al_r0_0x7ffe",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x7fff},
                                "al r0 0x7fff",
                                "ModifiedImmediate_al_r0_0x7fff",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x3333},
                                "al r0 0x3333",
                                "ModifiedImmediate_al_r0_0x3333",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x5555},
                                "al r0 0x5555",
                                "ModifiedImmediate_al_r0_0x5555",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0xaaaa},
                                "al r0 0xaaaa",
                                "ModifiedImmediate_al_r0_0xaaaa",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0xcccc},
                                "al r0 0xcccc",
                                "ModifiedImmediate_al_r0_0xcccc",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x8000},
                                "al r0 0x8000",
                                "ModifiedImmediate_al_r0_0x8000",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x8001},
                                "al r0 0x8001",
                                "ModifiedImmediate_al_r0_0x8001",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x8002},
                                "al r0 0x8002",
                                "ModifiedImmediate_al_r0_0x8002",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0x8003},
                                "al r0 0x8003",
                                "ModifiedImmediate_al_r0_0x8003",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0xff80},
                                "al r0 0xff80",
                                "ModifiedImmediate_al_r0_0xff80",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0xff81},
                                "al r0 0xff81",
                                "ModifiedImmediate_al_r0_0xff81",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0xff82},
                                "al r0 0xff82",
                                "ModifiedImmediate_al_r0_0xff82",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0xff83},
                                "al r0 0xff83",
                                "ModifiedImmediate_al_r0_0xff83",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0xffe0},
                                "al r0 0xffe0",
                                "ModifiedImmediate_al_r0_0xffe0",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0xfffd},
                                "al r0 0xfffd",
                                "ModifiedImmediate_al_r0_0xfffd",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0xfffe},
                                "al r0 0xfffe",
                                "ModifiedImmediate_al_r0_0xfffe",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate},
                               {{al, r0, 0xffff},
                                "al r0 0xffff",
                                "ModifiedImmediate_al_r0_0xffff",
                                ARRAY_SIZE(kModifiedImmediate),
                                kModifiedImmediate}};

// We record all inputs to the instructions as outputs. This way, we also check
// that what shouldn't change didn't change.
struct TestResult {
  size_t output_size;
  const Inputs* outputs;
};

// These headers each contain an array of `TestResult` with the reference output
// values. The reference arrays are names `kReference{mnemonic}`.
#include "aarch32/traces/simulator-cond-rd-operand-imm16-mov-t32.h"
#include "aarch32/traces/simulator-cond-rd-operand-imm16-movt-t32.h"


// The maximum number of errors to report in detail for each test.
const unsigned kErrorReportLimit = 8;

typedef void (MacroAssembler::*Fn)(Condition cond,
                                   Register rd,
                                   const Operand& op);

void TestHelper(Fn instruction,
                const char* mnemonic,
                const TestResult reference[]) {
  SETUP();
  masm.UseT32();
  START();

  // Data to compare to `reference`.
  TestResult* results[ARRAY_SIZE(kTests)];

  // Test cases for memory bound instructions may allocate a buffer and save its
  // address in this array.
  byte* scratch_memory_buffers[ARRAY_SIZE(kTests)];

  // Generate a loop for each element in `kTests`. Each loop tests one specific
  // instruction.
  for (unsigned i = 0; i < ARRAY_SIZE(kTests); i++) {
    // Allocate results on the heap for this test.
    results[i] = new TestResult;
    results[i]->outputs = new Inputs[kTests[i].input_size];
    results[i]->output_size = kTests[i].input_size;

    size_t input_stride = sizeof(kTests[i].inputs[0]) * kTests[i].input_size;
    VIXL_ASSERT(IsUint32(input_stride));

    scratch_memory_buffers[i] = NULL;

    Label loop;
    UseScratchRegisterScope scratch_registers(&masm);
    // Include all registers from r0 ro r12.
    scratch_registers.Include(RegisterList(0x1fff));

    // Values to pass to the macro-assembler.
    Condition cond = kTests[i].operands.cond;
    Register rd = kTests[i].operands.rd;
    uint32_t immediate = kTests[i].operands.immediate;
    Operand op(immediate);
    scratch_registers.Exclude(rd);

    // Allocate reserved registers for our own use.
    Register input_ptr = scratch_registers.Acquire();
    Register input_end = scratch_registers.Acquire();
    Register result_ptr = scratch_registers.Acquire();

    // Initialize `input_ptr` to the first element and `input_end` the address
    // after the array.
    __ Mov(input_ptr, Operand::From(kTests[i].inputs));
    __ Add(input_end, input_ptr, static_cast<uint32_t>(input_stride));
    __ Mov(result_ptr, Operand::From(results[i]->outputs));
    __ Bind(&loop);

    {
      UseScratchRegisterScope temp_registers(&masm);
      Register nzcv_bits = temp_registers.Acquire();
      Register saved_q_bit = temp_registers.Acquire();
      // Save the `Q` bit flag.
      __ Mrs(saved_q_bit, APSR);
      __ And(saved_q_bit, saved_q_bit, QFlag);
      // Set the `NZCV` and `Q` flags together.
      __ Ldr(nzcv_bits, MemOperand(input_ptr, offsetof(Inputs, apsr)));
      __ Orr(nzcv_bits, nzcv_bits, saved_q_bit);
      __ Msr(APSR_nzcvq, nzcv_bits);
    }
    __ Ldr(rd, MemOperand(input_ptr, offsetof(Inputs, rd)));

    (masm.*instruction)(cond, rd, op);

    {
      UseScratchRegisterScope temp_registers(&masm);
      Register nzcv_bits = temp_registers.Acquire();
      __ Mrs(nzcv_bits, APSR);
      // Only record the NZCV bits.
      __ And(nzcv_bits, nzcv_bits, NZCVFlag);
      __ Str(nzcv_bits, MemOperand(result_ptr, offsetof(Inputs, apsr)));
    }
    __ Str(rd, MemOperand(result_ptr, offsetof(Inputs, rd)));

    // Advance the result pointer.
    __ Add(result_ptr, result_ptr, Operand::From(sizeof(kTests[i].inputs[0])));
    // Loop back until `input_ptr` is lower than `input_base`.
    __ Add(input_ptr, input_ptr, Operand::From(sizeof(kTests[i].inputs[0])));
    __ Cmp(input_ptr, input_end);
    __ B(ne, &loop);
  }

  END();

  RUN();

  if (Test::generate_test_trace()) {
    // Print the results.
    for (size_t i = 0; i < ARRAY_SIZE(kTests); i++) {
      printf("const Inputs kOutputs_%s_%s[] = {\n",
             mnemonic,
             kTests[i].identifier);
      for (size_t j = 0; j < results[i]->output_size; j++) {
        printf("  { ");
        printf("0x%08" PRIx32, results[i]->outputs[j].apsr);
        printf(", ");
        printf("0x%08" PRIx32, results[i]->outputs[j].rd);
        printf(" },\n");
      }
      printf("};\n");
    }
    printf("const TestResult kReference%s[] = {\n", mnemonic);
    for (size_t i = 0; i < ARRAY_SIZE(kTests); i++) {
      printf("  {\n");
      printf("    ARRAY_SIZE(kOutputs_%s_%s),\n",
             mnemonic,
             kTests[i].identifier);
      printf("    kOutputs_%s_%s,\n", mnemonic, kTests[i].identifier);
      printf("  },\n");
    }
    printf("};\n");
  } else if (kCheckSimulatorTestResults) {
    // Check the results.
    unsigned total_error_count = 0;
    for (size_t i = 0; i < ARRAY_SIZE(kTests); i++) {
      bool instruction_has_errors = false;
      for (size_t j = 0; j < kTests[i].input_size; j++) {
        uint32_t apsr = results[i]->outputs[j].apsr;
        uint32_t rd = results[i]->outputs[j].rd;
        uint32_t apsr_input = kTests[i].inputs[j].apsr;
        uint32_t rd_input = kTests[i].inputs[j].rd;
        uint32_t apsr_ref = reference[i].outputs[j].apsr;
        uint32_t rd_ref = reference[i].outputs[j].rd;

        if (((apsr != apsr_ref) || (rd != rd_ref)) &&
            (++total_error_count <= kErrorReportLimit)) {
          // Print the instruction once even if it triggered multiple failures.
          if (!instruction_has_errors) {
            printf("Error(s) when testing \"%s %s\":\n",
                   mnemonic,
                   kTests[i].operands_description);
            instruction_has_errors = true;
          }
          // Print subsequent errors.
          printf("  Input:    ");
          printf("0x%08" PRIx32, apsr_input);
          printf(", ");
          printf("0x%08" PRIx32, rd_input);
          printf("\n");
          printf("  Expected: ");
          printf("0x%08" PRIx32, apsr_ref);
          printf(", ");
          printf("0x%08" PRIx32, rd_ref);
          printf("\n");
          printf("  Found:    ");
          printf("0x%08" PRIx32, apsr);
          printf(", ");
          printf("0x%08" PRIx32, rd);
          printf("\n\n");
        }
      }
    }

    if (total_error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n",
             total_error_count - kErrorReportLimit);
    }
    VIXL_CHECK(total_error_count == 0);
  } else {
    VIXL_WARNING("Assembled the code, but did not run anything.\n");
  }

  for (size_t i = 0; i < ARRAY_SIZE(kTests); i++) {
    delete[] results[i]->outputs;
    delete results[i];
    delete[] scratch_memory_buffers[i];
  }
}

// Instantiate tests for each instruction in the list.
// TODO: Remove this limitation by having a sandboxing mechanism.
#if defined(VIXL_HOST_POINTER_32)
#define TEST(mnemonic)                                                      \
  void Test_##mnemonic() {                                                  \
    TestHelper(&MacroAssembler::mnemonic, #mnemonic, kReference##mnemonic); \
  }                                                                         \
  Test test_##mnemonic("AARCH32_SIMULATOR_COND_RD_OPERAND_IMM16_" #mnemonic \
                       "_T32",                                              \
                       &Test_##mnemonic);
#else
#define TEST(mnemonic)                                                      \
  void Test_##mnemonic() {                                                  \
    VIXL_WARNING("This test can only run on a 32-bit host.\n");             \
    USE(TestHelper);                                                        \
  }                                                                         \
  Test test_##mnemonic("AARCH32_SIMULATOR_COND_RD_OPERAND_IMM16_" #mnemonic \
                       "_T32",                                              \
                       &Test_##mnemonic);
#endif

FOREACH_INSTRUCTION(TEST)
#undef TEST

}  // namespace
#endif

}  // namespace aarch32
}  // namespace vixl
