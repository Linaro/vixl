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

#include <cfloat>
#include <cstdio>
#include <sstream>

#include "test-runner.h"
#include "test-utils.h"

#include "aarch64/cpu-features-auditor-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"
#include "aarch64/simulator-aarch64.h"
#include "aarch64/test-simulator-inputs-aarch64.h"
#include "aarch64/test-simulator-traces-aarch64.h"
#include "aarch64/test-utils-aarch64.h"

namespace vixl {
namespace aarch64 {

// ==== Simulator Tests ====
//
// These simulator tests check instruction behaviour against a trace taken from
// real AArch64 hardware. The same test code is used to generate the trace; the
// results are printed to stdout when the test is run with
// --generate_test_trace.
//
// The input lists and expected results are stored in test/traces. The expected
// results can be regenerated using tools/generate_simulator_traces.py. Adding a
// test for a new instruction is described at the top of
// test-simulator-traces-aarch64.h.

#define __ masm.
#define TEST(name) TEST_(AARCH64_SIM_##name)

#define SETUP() SETUP_WITH_FEATURES(CPUFeatures())

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64

#define SETUP_WITH_FEATURES(...)                 \
  MacroAssembler masm;                           \
  masm.SetCPUFeatures(CPUFeatures(__VA_ARGS__)); \
  Decoder decoder;                               \
  Simulator simulator(&decoder);                 \
  simulator.SetColouredTrace(Test::coloured_trace());

#define START()                                                         \
  masm.Reset();                                                         \
  simulator.ResetState();                                               \
  __ PushCalleeSavedRegisters();                                        \
  /* The infrastructure code hasn't been covered at the moment, e.g. */ \
  /* prologue/epilogue. Suppress tagging mis-match exception before  */ \
  /* this point. */                                                     \
  if (masm.GetCPUFeatures()->Has(CPUFeatures::kMTE)) {                  \
    __ Hlt(DebugHltOpcode::kMTEActive);                                 \
  }                                                                     \
  if (Test::trace_reg()) {                                              \
    __ Trace(LOG_STATE, TRACE_ENABLE);                                  \
  }                                                                     \
  if (Test::trace_write()) {                                            \
    __ Trace(LOG_WRITE, TRACE_ENABLE);                                  \
  }                                                                     \
  if (Test::trace_sim()) {                                              \
    __ Trace(LOG_DISASM, TRACE_ENABLE);                                 \
  }

#define END()                                          \
  if (masm.GetCPUFeatures()->Has(CPUFeatures::kMTE)) { \
    __ Hlt(DebugHltOpcode::kMTEInactive);              \
  }                                                    \
  __ Trace(LOG_ALL, TRACE_DISABLE);                    \
  __ PopCalleeSavedRegisters();                        \
  __ Ret();                                            \
  masm.FinalizeCode()

#define TRY_RUN(skipped)                                                \
  DISASSEMBLE();                                                        \
  simulator.RunFrom(masm.GetBuffer()->GetStartAddress<Instruction*>()); \
  /* The simulator can run every test. */                               \
  *skipped = false

#ifdef VIXL_ENABLE_IMPLICIT_CHECKS
// The signal handler needs access to the simulator.
Simulator* gImplicitCheckSim;

#ifdef __x86_64__
#include <signal.h>
#include <ucontext.h>
void HandleSegFault(int sig, siginfo_t* info, void* context) {
  USE(sig);
  USE(info);
  Simulator* sim = gImplicitCheckSim;

  // Did the signal come from the simulator?
  ucontext_t* uc = reinterpret_cast<ucontext_t*>(context);
  uintptr_t fault_pc = uc->uc_mcontext.gregs[REG_RIP];
  VIXL_CHECK(sim->IsSimulatedMemoryAccess(fault_pc));

  // Increment the counter (x1) each time we handle a signal.
  int64_t counter = reinterpret_cast<int64_t>(sim->ReadXRegister(1));
  sim->WriteXRegister(1, ++counter);

  // Return to the VIXL memory access continuation point, which is also the
  // next instruction, after this handler.
  uc->uc_mcontext.gregs[REG_RIP] = sim->GetSignalReturnAddress();
  // Return that the memory access failed.
  uc->uc_mcontext.gregs[REG_RAX] =
      static_cast<greg_t>(MemoryAccessResult::Failure);
}
#endif  // __x86_64__

// Start an implicit check test with a counter and start label so the number of
// faults can be counted. Note: each instruction after the start will be
// expected to fault.
#define START_IMPLICIT_CHECK()                                                \
  gImplicitCheckSim = &simulator;                                             \
  /* Set up a signal handler to count the number of faulting instructions. */ \
  struct sigaction sa;                                                        \
  sa.sa_sigaction = HandleSegFault;                                           \
  sigaction(SIGSEGV, &sa, NULL);                                              \
  START();                                                                    \
  /* Reset the counter. */                                                    \
  __ Mov(x1, 0);                                                              \
  /* Use a consistent bad address. */                                         \
  __ Mov(x15, xzr);                                                           \
  __ Mov(ip0, xzr);                                                           \
  /* Load an amount of data to load. */                                       \
  __ Mov(ip1, 4096);                                                          \
  [[maybe_unused]] MemOperand bad_memory = MemOperand(ip0);                   \
  if (masm.GetCPUFeatures()->Has(CPUFeatures::kSVE)) {                        \
    /* Turn on all lanes to ensure all loads/stores are tested. */            \
    __ Ptrue(p0.VnB());                                                       \
    __ Ptrue(p1.VnB());                                                       \
    __ Ptrue(p2.VnB());                                                       \
    __ Ptrue(p3.VnB());                                                       \
    __ Ptrue(p4.VnB());                                                       \
    __ Ptrue(p5.VnB());                                                       \
    __ Ptrue(p6.VnB());                                                       \
    __ Ptrue(p7.VnB());                                                       \
    __ Ptrue(p8.VnB());                                                       \
    __ Ptrue(p9.VnB());                                                       \
    __ Ptrue(p10.VnB());                                                      \
    __ Ptrue(p11.VnB());                                                      \
    __ Ptrue(p12.VnB());                                                      \
    __ Ptrue(p13.VnB());                                                      \
    __ Ptrue(p14.VnB());                                                      \
    __ Ptrue(p15.VnB());                                                      \
  }                                                                           \
  Label l_start, l_end;                                                       \
  __ Bind(&l_start);

#define END_IMPLICIT_CHECK() \
  __ Bind(&l_end);           \
  /* Return the counter. */  \
  __ Mov(x0, x1);            \
  END();

#define TRY_RUN_IMPLICIT_CHECK()                                              \
  bool skipped;                                                               \
  TRY_RUN(&skipped);                                                          \
  /* Implicit checks should only be used with the simulator. */               \
  VIXL_ASSERT(!skipped);                                                      \
  /* Check that each load/store instruction generated a segfault that was */  \
  /* raised and dealt with. */                                                \
  size_t result = simulator.ReadXRegister(0);                                 \
  size_t num_of_faulting_instr = masm.GetSizeOfCodeGeneratedSince(&l_start) - \
                                 masm.GetSizeOfCodeGeneratedSince(&l_end);    \
  VIXL_CHECK((result * kInstructionSize) == num_of_faulting_instr);

#endif  // VIXL_ENABLE_IMPLICIT_CHECKS

#else  // VIXL_INCLUDE_SIMULATOR_AARCH64

#define SETUP_WITH_FEATURES(...)                 \
  MacroAssembler masm;                           \
  masm.SetCPUFeatures(CPUFeatures(__VA_ARGS__)); \
  CPU::SetUp()

#define START() \
  masm.Reset(); \
  __ PushCalleeSavedRegisters()

#define END()                   \
  __ PopCalleeSavedRegisters(); \
  __ Ret();                     \
  masm.FinalizeCode()

#define TRY_RUN(skipped)                                                      \
  DISASSEMBLE();                                                              \
  /* If the test uses features that the current CPU doesn't support, don't */ \
  /* attempt to run it natively.                                           */ \
  {                                                                           \
    Decoder decoder;                                                          \
    /* TODO: Once available, use runtime feature detection. The use of  */    \
    /* AArch64LegacyBaseline is a stopgap.                              */    \
    const CPUFeatures& this_machine = CPUFeatures::AArch64LegacyBaseline();   \
    CPUFeaturesAuditor auditor(&decoder, this_machine);                       \
    CodeBuffer* buffer = masm.GetBuffer();                                    \
    decoder.Decode(buffer->GetStartAddress<Instruction*>(),                   \
                   buffer->GetEndAddress<Instruction*>());                    \
    const CPUFeatures& requirements = auditor.GetSeenFeatures();              \
    if (this_machine.Has(requirements)) {                                     \
      masm.GetBuffer()->SetExecutable();                                      \
      ExecuteMemory(buffer->GetStartAddress<byte*>(),                         \
                    masm.GetSizeOfCodeGenerated());                           \
      masm.GetBuffer()->SetWritable();                                        \
      *skipped = false;                                                       \
    } else {                                                                  \
      std::stringstream os;                                                   \
      /* Note: This message needs to match REGEXP_MISSING_FEATURES from    */ \
      /* tools/threaded_test.py.                                           */ \
      os << "SKIPPED: Missing features: { ";                                  \
      os << requirements.Without(this_machine) << " }\n";                     \
      printf("%s", os.str().c_str());                                         \
      *skipped = true;                                                        \
    }                                                                         \
  }


#endif  // VIXL_INCLUDE_SIMULATOR_AARCH64


#define DISASSEMBLE()                                             \
  if (Test::disassemble()) {                                      \
    PrintDisassembler disasm(stdout);                             \
    CodeBuffer* buffer = masm.GetBuffer();                        \
    Instruction* start = buffer->GetStartAddress<Instruction*>(); \
    Instruction* end = buffer->GetEndAddress<Instruction*>();     \
    disasm.DisassembleBuffer(start, end);                         \
  }

// The maximum number of errors to report in detail for each test.
static const unsigned kErrorReportLimit = 8;


// Overloaded versions of RawbitsToDouble and RawbitsToFloat for use in the
// templated test functions.
static float rawbits_to_fp(uint32_t bits) { return RawbitsToFloat(bits); }

static double rawbits_to_fp(uint64_t bits) { return RawbitsToDouble(bits); }

// The rawbits_to_fp functions are only used for printing decimal values so we
// just approximate FP16 as double.
static double rawbits_to_fp(uint16_t bits) {
  return FPToDouble(RawbitsToFloat16(bits), kIgnoreDefaultNaN);
}


// MacroAssembler member function pointers to pass to the test dispatchers.
typedef void (MacroAssembler::*Test1OpFPHelper_t)(const VRegister& fd,
                                                  const VRegister& fn);
typedef void (MacroAssembler::*Test2OpFPHelper_t)(const VRegister& fd,
                                                  const VRegister& fn,
                                                  const VRegister& fm);
typedef void (MacroAssembler::*Test3OpFPHelper_t)(const VRegister& fd,
                                                  const VRegister& fn,
                                                  const VRegister& fm,
                                                  const VRegister& fa);
typedef void (MacroAssembler::*TestFPCmpHelper_t)(const VRegister& fn,
                                                  const VRegister& fm);
typedef void (MacroAssembler::*TestFPCmpZeroHelper_t)(const VRegister& fn,
                                                      double value);
typedef void (MacroAssembler::*TestFPToIntHelper_t)(const Register& rd,
                                                    const VRegister& fn);
typedef void (MacroAssembler::*TestFPToFixedHelper_t)(const Register& rd,
                                                      const VRegister& fn,
                                                      int fbits);
typedef void (MacroAssembler::*TestFixedToFPHelper_t)(const VRegister& fd,
                                                      const Register& rn,
                                                      int fbits);
// TODO: 'Test2OpNEONHelper_t' and 'Test2OpFPHelper_t' can be
//       consolidated into one routine.
typedef void (MacroAssembler::*Test1OpNEONHelper_t)(const VRegister& vd,
                                                    const VRegister& vn);
typedef void (MacroAssembler::*Test2OpNEONHelper_t)(const VRegister& vd,
                                                    const VRegister& vn,
                                                    const VRegister& vm);
typedef void (MacroAssembler::*TestByElementNEONHelper_t)(const VRegister& vd,
                                                          const VRegister& vn,
                                                          const VRegister& vm,
                                                          int vm_index);
typedef void (MacroAssembler::*TestOpImmOpImmVdUpdateNEONHelper_t)(
    const VRegister& vd, int imm1, const VRegister& vn, int imm2);

// This helps using the same typename for both the function pointer
// and the array of immediates passed to helper routines.
template <typename T>
class Test2OpImmediateNEONHelper_t {
 public:
  typedef void (MacroAssembler::*mnemonic)(const VRegister& vd,
                                           const VRegister& vn,
                                           T imm);
};


// Maximum number of hex characters required to represent values of either
// templated type.
template <typename Ta, typename Tb>
static unsigned MaxHexCharCount() {
  unsigned count = static_cast<unsigned>(std::max(sizeof(Ta), sizeof(Tb)));
  return (count * 8) / 4;
}


// Standard test dispatchers.


static void Test1Op_Helper(Test1OpFPHelper_t helper,
                           uintptr_t inputs,
                           unsigned inputs_length,
                           uintptr_t results,
                           unsigned d_size,
                           unsigned n_size,
                           bool* skipped) {
  VIXL_ASSERT((d_size == kDRegSize) || (d_size == kSRegSize) ||
              (d_size == kHRegSize));
  VIXL_ASSERT((n_size == kDRegSize) || (n_size == kSRegSize) ||
              (n_size == kHRegSize));

  CPUFeatures features;
  features.Combine(CPUFeatures::kFP, CPUFeatures::kFPHalf);
  // For frint{32,64}{x,y} variants.
  features.Combine(CPUFeatures::kFrintToFixedSizedInt);
  SETUP_WITH_FEATURES(features);
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n;

  Register out = x0;
  Register inputs_base = x1;
  Register length = w2;
  Register index_n = w3;

  int n_index_shift;
  VRegister fd;
  VRegister fn;
  if (n_size == kDRegSize) {
    n_index_shift = kDRegSizeInBytesLog2;
    fn = d1;
  } else if (n_size == kSRegSize) {
    n_index_shift = kSRegSizeInBytesLog2;
    fn = s1;
  } else {
    n_index_shift = kHRegSizeInBytesLog2;
    fn = h1;
  }

  if (d_size == kDRegSize) {
    fd = d0;
  } else if (d_size == kSRegSize) {
    fd = s0;
  } else {
    fd = h0;
  }


  __ Mov(out, results);
  __ Mov(inputs_base, inputs);
  __ Mov(length, inputs_length);

  __ Mov(index_n, 0);
  __ Bind(&loop_n);
  __ Ldr(fn, MemOperand(inputs_base, index_n, UXTW, n_index_shift));

  {
    SingleEmissionCheckScope guard(&masm);
    (masm.*helper)(fd, fn);
  }
  __ Str(fd, MemOperand(out, fd.GetSizeInBytes(), PostIndex));

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}


// Test FP instructions. The inputs[] and expected[] arrays should be arrays of
// rawbits representations of doubles or floats. This ensures that exact bit
// comparisons can be performed.
template <typename Tn, typename Td>
static void Test1Op(const char* name,
                    Test1OpFPHelper_t helper,
                    const Tn inputs[],
                    unsigned inputs_length,
                    const Td expected[],
                    unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length;
  Td* results = new Td[results_length];

  const unsigned d_bits = sizeof(Td) * 8;
  const unsigned n_bits = sizeof(Tn) * 8;
  bool skipped;

  Test1Op_Helper(helper,
                 reinterpret_cast<uintptr_t>(inputs),
                 inputs_length,
                 reinterpret_cast<uintptr_t>(results),
                 d_bits,
                 n_bits,
                 &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_%s[] = {\n", d_bits, name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  0x%0*" PRIx64 ",\n",
             d_bits / 4,
             static_cast<uint64_t>(results[d]));
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++, d++) {
      if (results[d] != expected[d]) {
        if (++error_count > kErrorReportLimit) continue;

        printf("%s 0x%0*" PRIx64 " (%s %g):\n",
               name,
               n_bits / 4,
               static_cast<uint64_t>(inputs[n]),
               name,
               rawbits_to_fp(inputs[n]));
        printf("  Expected: 0x%0*" PRIx64 " (%g)\n",
               d_bits / 4,
               static_cast<uint64_t>(expected[d]),
               rawbits_to_fp(expected[d]));
        printf("  Found:    0x%0*" PRIx64 " (%g)\n",
               d_bits / 4,
               static_cast<uint64_t>(results[d]),
               rawbits_to_fp(results[d]));
        printf("\n");
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


static void Test2Op_Helper(Test2OpFPHelper_t helper,
                           uintptr_t inputs,
                           unsigned inputs_length,
                           uintptr_t results,
                           unsigned reg_size,
                           bool* skipped) {
  VIXL_ASSERT((reg_size == kDRegSize) || (reg_size == kSRegSize) ||
              (reg_size == kHRegSize));

  SETUP_WITH_FEATURES(CPUFeatures::kFP, CPUFeatures::kFPHalf);
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n, loop_m;

  Register out = x0;
  Register inputs_base = x1;
  Register length = w2;
  Register index_n = w3;
  Register index_m = w4;

  bool double_op = reg_size == kDRegSize;
  bool float_op = reg_size == kSRegSize;
  int index_shift;
  if (double_op) {
    index_shift = kDRegSizeInBytesLog2;
  } else if (float_op) {
    index_shift = kSRegSizeInBytesLog2;
  } else {
    index_shift = kHRegSizeInBytesLog2;
  }

  VRegister fd;
  VRegister fn;
  VRegister fm;

  if (double_op) {
    fd = d0;
    fn = d1;
    fm = d2;
  } else if (float_op) {
    fd = s0;
    fn = s1;
    fm = s2;
  } else {
    fd = h0;
    fn = h1;
    fm = h2;
  }

  __ Mov(out, results);
  __ Mov(inputs_base, inputs);
  __ Mov(length, inputs_length);

  __ Mov(index_n, 0);
  __ Bind(&loop_n);
  __ Ldr(fn, MemOperand(inputs_base, index_n, UXTW, index_shift));

  __ Mov(index_m, 0);
  __ Bind(&loop_m);
  __ Ldr(fm, MemOperand(inputs_base, index_m, UXTW, index_shift));

  {
    SingleEmissionCheckScope guard(&masm);
    (masm.*helper)(fd, fn, fm);
  }
  __ Str(fd, MemOperand(out, fd.GetSizeInBytes(), PostIndex));

  __ Add(index_m, index_m, 1);
  __ Cmp(index_m, inputs_length);
  __ B(lo, &loop_m);

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}


// Test FP instructions. The inputs[] and expected[] arrays should be arrays of
// rawbits representations of doubles or floats. This ensures that exact bit
// comparisons can be performed.
template <typename T>
static void Test2Op(const char* name,
                    Test2OpFPHelper_t helper,
                    const T inputs[],
                    unsigned inputs_length,
                    const T expected[],
                    unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length * inputs_length;
  T* results = new T[results_length];

  const unsigned bits = sizeof(T) * 8;
  bool skipped;

  Test2Op_Helper(helper,
                 reinterpret_cast<uintptr_t>(inputs),
                 inputs_length,
                 reinterpret_cast<uintptr_t>(results),
                 bits,
                 &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_%s[] = {\n", bits, name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  0x%0*" PRIx64 ",\n",
             bits / 4,
             static_cast<uint64_t>(results[d]));
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++) {
      for (unsigned m = 0; m < inputs_length; m++, d++) {
        if (results[d] != expected[d]) {
          if (++error_count > kErrorReportLimit) continue;

          printf("%s 0x%0*" PRIx64 ", 0x%0*" PRIx64 " (%s %g %g):\n",
                 name,
                 bits / 4,
                 static_cast<uint64_t>(inputs[n]),
                 bits / 4,
                 static_cast<uint64_t>(inputs[m]),
                 name,
                 rawbits_to_fp(inputs[n]),
                 rawbits_to_fp(inputs[m]));
          printf("  Expected: 0x%0*" PRIx64 " (%g)\n",
                 bits / 4,
                 static_cast<uint64_t>(expected[d]),
                 rawbits_to_fp(expected[d]));
          printf("  Found:    0x%0*" PRIx64 " (%g)\n",
                 bits / 4,
                 static_cast<uint64_t>(results[d]),
                 rawbits_to_fp(results[d]));
          printf("\n");
        }
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


static void Test3Op_Helper(Test3OpFPHelper_t helper,
                           uintptr_t inputs,
                           unsigned inputs_length,
                           uintptr_t results,
                           unsigned reg_size,
                           bool* skipped) {
  VIXL_ASSERT((reg_size == kDRegSize) || (reg_size == kSRegSize) ||
              (reg_size == kHRegSize));

  SETUP_WITH_FEATURES(CPUFeatures::kFP, CPUFeatures::kFPHalf);
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n, loop_m, loop_a;

  Register out = x0;
  Register inputs_base = x1;
  Register length = w2;
  Register index_n = w3;
  Register index_m = w4;
  Register index_a = w5;

  bool double_op = reg_size == kDRegSize;
  bool single_op = reg_size == kSRegSize;
  int index_shift;
  VRegister fd(0, reg_size);
  VRegister fn(1, reg_size);
  VRegister fm(2, reg_size);
  VRegister fa(3, reg_size);
  if (double_op) {
    index_shift = kDRegSizeInBytesLog2;
  } else if (single_op) {
    index_shift = kSRegSizeInBytesLog2;
  } else {
    index_shift = kHRegSizeInBytesLog2;
  }

  __ Mov(out, results);
  __ Mov(inputs_base, inputs);
  __ Mov(length, inputs_length);

  __ Mov(index_n, 0);
  __ Bind(&loop_n);
  __ Ldr(fn, MemOperand(inputs_base, index_n, UXTW, index_shift));

  __ Mov(index_m, 0);
  __ Bind(&loop_m);
  __ Ldr(fm, MemOperand(inputs_base, index_m, UXTW, index_shift));

  __ Mov(index_a, 0);
  __ Bind(&loop_a);
  __ Ldr(fa, MemOperand(inputs_base, index_a, UXTW, index_shift));

  {
    SingleEmissionCheckScope guard(&masm);
    (masm.*helper)(fd, fn, fm, fa);
  }
  __ Str(fd, MemOperand(out, fd.GetSizeInBytes(), PostIndex));

  __ Add(index_a, index_a, 1);
  __ Cmp(index_a, inputs_length);
  __ B(lo, &loop_a);

  __ Add(index_m, index_m, 1);
  __ Cmp(index_m, inputs_length);
  __ B(lo, &loop_m);

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}


// Test FP instructions. The inputs[] and expected[] arrays should be arrays of
// rawbits representations of doubles or floats. This ensures that exact bit
// comparisons can be performed.
template <typename T>
static void Test3Op(const char* name,
                    Test3OpFPHelper_t helper,
                    const T inputs[],
                    unsigned inputs_length,
                    const T expected[],
                    unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length * inputs_length * inputs_length;
  T* results = new T[results_length];

  const unsigned bits = sizeof(T) * 8;
  bool skipped;

  Test3Op_Helper(helper,
                 reinterpret_cast<uintptr_t>(inputs),
                 inputs_length,
                 reinterpret_cast<uintptr_t>(results),
                 bits,
                 &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_%s[] = {\n", bits, name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  0x%0*" PRIx64 ",\n",
             bits / 4,
             static_cast<uint64_t>(results[d]));
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++) {
      for (unsigned m = 0; m < inputs_length; m++) {
        for (unsigned a = 0; a < inputs_length; a++, d++) {
          if (results[d] != expected[d]) {
            if (++error_count > kErrorReportLimit) continue;

            printf("%s 0x%0*" PRIx64 ", 0x%0*" PRIx64 ", 0x%0*" PRIx64
                   " (%s %g %g %g):\n",
                   name,
                   bits / 4,
                   static_cast<uint64_t>(inputs[n]),
                   bits / 4,
                   static_cast<uint64_t>(inputs[m]),
                   bits / 4,
                   static_cast<uint64_t>(inputs[a]),
                   name,
                   rawbits_to_fp(inputs[n]),
                   rawbits_to_fp(inputs[m]),
                   rawbits_to_fp(inputs[a]));
            printf("  Expected: 0x%0*" PRIx64 " (%g)\n",
                   bits / 4,
                   static_cast<uint64_t>(expected[d]),
                   rawbits_to_fp(expected[d]));
            printf("  Found:    0x%0*" PRIx64 " (%g)\n",
                   bits / 4,
                   static_cast<uint64_t>(results[d]),
                   rawbits_to_fp(results[d]));
            printf("\n");
          }
        }
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


static void TestCmp_Helper(TestFPCmpHelper_t helper,
                           uintptr_t inputs,
                           unsigned inputs_length,
                           uintptr_t results,
                           unsigned reg_size,
                           bool* skipped) {
  VIXL_ASSERT((reg_size == kDRegSize) || (reg_size == kSRegSize));

  SETUP_WITH_FEATURES(CPUFeatures::kFP);
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n, loop_m;

  Register out = x0;
  Register inputs_base = x1;
  Register length = w2;
  Register index_n = w3;
  Register index_m = w4;
  Register flags = x5;

  bool double_op = reg_size == kDRegSize;
  const int index_shift =
      double_op ? kDRegSizeInBytesLog2 : kSRegSizeInBytesLog2;

  VRegister fn = double_op ? d1 : s1;
  VRegister fm = double_op ? d2 : s2;

  __ Mov(out, results);
  __ Mov(inputs_base, inputs);
  __ Mov(length, inputs_length);

  __ Mov(index_n, 0);
  __ Bind(&loop_n);
  __ Ldr(fn, MemOperand(inputs_base, index_n, UXTW, index_shift));

  __ Mov(index_m, 0);
  __ Bind(&loop_m);
  __ Ldr(fm, MemOperand(inputs_base, index_m, UXTW, index_shift));

  {
    SingleEmissionCheckScope guard(&masm);
    (masm.*helper)(fn, fm);
  }
  __ Mrs(flags, NZCV);
  __ Ubfx(flags, flags, 28, 4);
  __ Strb(flags, MemOperand(out, 1, PostIndex));

  __ Add(index_m, index_m, 1);
  __ Cmp(index_m, inputs_length);
  __ B(lo, &loop_m);

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}


// Test FP instructions. The inputs[] and expected[] arrays should be arrays of
// rawbits representations of doubles or floats. This ensures that exact bit
// comparisons can be performed.
template <typename T>
static void TestCmp(const char* name,
                    TestFPCmpHelper_t helper,
                    const T inputs[],
                    unsigned inputs_length,
                    const uint8_t expected[],
                    unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length * inputs_length;
  uint8_t* results = new uint8_t[results_length];

  const unsigned bits = sizeof(T) * 8;
  bool skipped;

  TestCmp_Helper(helper,
                 reinterpret_cast<uintptr_t>(inputs),
                 inputs_length,
                 reinterpret_cast<uintptr_t>(results),
                 bits,
                 &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint8_t kExpected_%s[] = {\n", name);
    for (unsigned d = 0; d < results_length; d++) {
      // Each NZCV result only requires 4 bits.
      VIXL_ASSERT((results[d] & 0xf) == results[d]);
      printf("  0x%" PRIx8 ",\n", results[d]);
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++) {
      for (unsigned m = 0; m < inputs_length; m++, d++) {
        if (results[d] != expected[d]) {
          if (++error_count > kErrorReportLimit) continue;

          printf("%s 0x%0*" PRIx64 ", 0x%0*" PRIx64 " (%s %g %g):\n",
                 name,
                 bits / 4,
                 static_cast<uint64_t>(inputs[n]),
                 bits / 4,
                 static_cast<uint64_t>(inputs[m]),
                 name,
                 rawbits_to_fp(inputs[n]),
                 rawbits_to_fp(inputs[m]));
          printf("  Expected: %c%c%c%c (0x%" PRIx8 ")\n",
                 (expected[d] & 0x8) ? 'N' : 'n',
                 (expected[d] & 0x4) ? 'Z' : 'z',
                 (expected[d] & 0x2) ? 'C' : 'c',
                 (expected[d] & 0x1) ? 'V' : 'v',
                 expected[d]);
          printf("  Found:    %c%c%c%c (0x%" PRIx8 ")\n",
                 (results[d] & 0x8) ? 'N' : 'n',
                 (results[d] & 0x4) ? 'Z' : 'z',
                 (results[d] & 0x2) ? 'C' : 'c',
                 (results[d] & 0x1) ? 'V' : 'v',
                 results[d]);
          printf("\n");
        }
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


static void TestCmpZero_Helper(TestFPCmpZeroHelper_t helper,
                               uintptr_t inputs,
                               unsigned inputs_length,
                               uintptr_t results,
                               unsigned reg_size,
                               bool* skipped) {
  VIXL_ASSERT((reg_size == kDRegSize) || (reg_size == kSRegSize));

  SETUP_WITH_FEATURES(CPUFeatures::kFP);
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n, loop_m;

  Register out = x0;
  Register inputs_base = x1;
  Register length = w2;
  Register index_n = w3;
  Register flags = x4;

  bool double_op = reg_size == kDRegSize;
  const int index_shift =
      double_op ? kDRegSizeInBytesLog2 : kSRegSizeInBytesLog2;

  VRegister fn = double_op ? d1 : s1;

  __ Mov(out, results);
  __ Mov(inputs_base, inputs);
  __ Mov(length, inputs_length);

  __ Mov(index_n, 0);
  __ Bind(&loop_n);
  __ Ldr(fn, MemOperand(inputs_base, index_n, UXTW, index_shift));

  {
    SingleEmissionCheckScope guard(&masm);
    (masm.*helper)(fn, 0.0);
  }
  __ Mrs(flags, NZCV);
  __ Ubfx(flags, flags, 28, 4);
  __ Strb(flags, MemOperand(out, 1, PostIndex));

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}


// Test FP instructions. The inputs[] and expected[] arrays should be arrays of
// rawbits representations of doubles or floats. This ensures that exact bit
// comparisons can be performed.
template <typename T>
static void TestCmpZero(const char* name,
                        TestFPCmpZeroHelper_t helper,
                        const T inputs[],
                        unsigned inputs_length,
                        const uint8_t expected[],
                        unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length;
  uint8_t* results = new uint8_t[results_length];

  const unsigned bits = sizeof(T) * 8;
  bool skipped;

  TestCmpZero_Helper(helper,
                     reinterpret_cast<uintptr_t>(inputs),
                     inputs_length,
                     reinterpret_cast<uintptr_t>(results),
                     bits,
                     &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint8_t kExpected_%s[] = {\n", name);
    for (unsigned d = 0; d < results_length; d++) {
      // Each NZCV result only requires 4 bits.
      VIXL_ASSERT((results[d] & 0xf) == results[d]);
      printf("  0x%" PRIx8 ",\n", results[d]);
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++, d++) {
      if (results[d] != expected[d]) {
        if (++error_count > kErrorReportLimit) continue;

        printf("%s 0x%0*" PRIx64 ", 0x%0*u (%s %g #0.0):\n",
               name,
               bits / 4,
               static_cast<uint64_t>(inputs[n]),
               bits / 4,
               0,
               name,
               rawbits_to_fp(inputs[n]));
        printf("  Expected: %c%c%c%c (0x%" PRIx8 ")\n",
               (expected[d] & 0x8) ? 'N' : 'n',
               (expected[d] & 0x4) ? 'Z' : 'z',
               (expected[d] & 0x2) ? 'C' : 'c',
               (expected[d] & 0x1) ? 'V' : 'v',
               expected[d]);
        printf("  Found:    %c%c%c%c (0x%" PRIx8 ")\n",
               (results[d] & 0x8) ? 'N' : 'n',
               (results[d] & 0x4) ? 'Z' : 'z',
               (results[d] & 0x2) ? 'C' : 'c',
               (results[d] & 0x1) ? 'V' : 'v',
               results[d]);
        printf("\n");
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


static void TestFPToFixed_Helper(TestFPToFixedHelper_t helper,
                                 uintptr_t inputs,
                                 unsigned inputs_length,
                                 uintptr_t results,
                                 unsigned d_size,
                                 unsigned n_size,
                                 bool* skipped) {
  VIXL_ASSERT((d_size == kXRegSize) || (d_size == kWRegSize));
  VIXL_ASSERT((n_size == kDRegSize) || (n_size == kSRegSize) ||
              (n_size == kHRegSize));

  SETUP_WITH_FEATURES(CPUFeatures::kFP, CPUFeatures::kFPHalf);
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n;

  Register out = x0;
  Register inputs_base = x1;
  Register length = w2;
  Register index_n = w3;

  int n_index_shift;
  if (n_size == kDRegSize) {
    n_index_shift = kDRegSizeInBytesLog2;
  } else if (n_size == kSRegSize) {
    n_index_shift = kSRegSizeInBytesLog2;
  } else {
    n_index_shift = kHRegSizeInBytesLog2;
  }

  Register rd = (d_size == kXRegSize) ? Register(x10) : Register(w10);
  VRegister fn;
  if (n_size == kDRegSize) {
    fn = d1;
  } else if (n_size == kSRegSize) {
    fn = s1;
  } else {
    fn = h1;
  }

  __ Mov(out, results);
  __ Mov(inputs_base, inputs);
  __ Mov(length, inputs_length);

  __ Mov(index_n, 0);
  __ Bind(&loop_n);
  __ Ldr(fn, MemOperand(inputs_base, index_n, UXTW, n_index_shift));

  for (unsigned fbits = 0; fbits <= d_size; ++fbits) {
    {
      SingleEmissionCheckScope guard(&masm);
      (masm.*helper)(rd, fn, fbits);
    }
    __ Str(rd, MemOperand(out, rd.GetSizeInBytes(), PostIndex));
  }

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}


static void TestFPToInt_Helper(TestFPToIntHelper_t helper,
                               uintptr_t inputs,
                               unsigned inputs_length,
                               uintptr_t results,
                               unsigned d_size,
                               unsigned n_size,
                               bool* skipped) {
  VIXL_ASSERT((d_size == kXRegSize) || (d_size == kWRegSize));
  VIXL_ASSERT((n_size == kDRegSize) || (n_size == kSRegSize) ||
              (n_size == kHRegSize));

  SETUP_WITH_FEATURES(CPUFeatures::kFP,
                      CPUFeatures::kFPHalf,
                      CPUFeatures::kJSCVT);
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n;

  Register out = x0;
  Register inputs_base = x1;
  Register length = w2;
  Register index_n = w3;

  int n_index_shift;
  if (n_size == kDRegSize) {
    n_index_shift = kDRegSizeInBytesLog2;
  } else if (n_size == kSRegSize) {
    n_index_shift = kSRegSizeInBytesLog2;
  } else {
    n_index_shift = kHRegSizeInBytesLog2;
  }

  Register rd = (d_size == kXRegSize) ? Register(x10) : Register(w10);
  VRegister fn;
  if (n_size == kDRegSize) {
    fn = d1;
  } else if (n_size == kSRegSize) {
    fn = s1;
  } else {
    fn = h1;
  }

  __ Mov(out, results);
  __ Mov(inputs_base, inputs);
  __ Mov(length, inputs_length);

  __ Mov(index_n, 0);
  __ Bind(&loop_n);
  __ Ldr(fn, MemOperand(inputs_base, index_n, UXTW, n_index_shift));

  {
    SingleEmissionCheckScope guard(&masm);
    (masm.*helper)(rd, fn);
  }
  __ Str(rd, MemOperand(out, rd.GetSizeInBytes(), PostIndex));

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}


// Test FP instructions.
//  - The inputs[] array should be an array of rawbits representations of
//    doubles or floats. This ensures that exact bit comparisons can be
//    performed.
//  - The expected[] array should be an array of signed integers.
template <typename Tn, typename Td>
static void TestFPToS(const char* name,
                      TestFPToIntHelper_t helper,
                      const Tn inputs[],
                      unsigned inputs_length,
                      const Td expected[],
                      unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length;
  Td* results = new Td[results_length];

  const unsigned d_bits = sizeof(Td) * 8;
  const unsigned n_bits = sizeof(Tn) * 8;
  bool skipped;

  TestFPToInt_Helper(helper,
                     reinterpret_cast<uintptr_t>(inputs),
                     inputs_length,
                     reinterpret_cast<uintptr_t>(results),
                     d_bits,
                     n_bits,
                     &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const int%u_t kExpected_%s[] = {\n", d_bits, name);
    // There is no simple C++ literal for INT*_MIN that doesn't produce
    // warnings, so we use an appropriate constant in that case instead.
    // Deriving int_d_min in this way (rather than just checking INT64_MIN and
    // the like) avoids warnings about comparing values with differing ranges.
    const int64_t int_d_max = (UINT64_C(1) << (d_bits - 1)) - 1;
    const int64_t int_d_min = -(int_d_max)-1;
    for (unsigned d = 0; d < results_length; d++) {
      if (results[d] == int_d_min) {
        printf("  -INT%u_C(%" PRId64 ") - 1,\n", d_bits, int_d_max);
      } else {
        // Some constants (such as those between INT32_MAX and UINT32_MAX)
        // trigger compiler warnings. To avoid these warnings, use an
        // appropriate macro to make the type explicit.
        int64_t result_int64 = static_cast<int64_t>(results[d]);
        if (result_int64 >= 0) {
          printf("  INT%u_C(%" PRId64 "),\n", d_bits, result_int64);
        } else {
          printf("  -INT%u_C(%" PRId64 "),\n", d_bits, -result_int64);
        }
      }
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++, d++) {
      if (results[d] != expected[d]) {
        if (++error_count > kErrorReportLimit) continue;

        printf("%s 0x%0*" PRIx64 " (%s %g):\n",
               name,
               n_bits / 4,
               static_cast<uint64_t>(inputs[n]),
               name,
               rawbits_to_fp(inputs[n]));
        printf("  Expected: 0x%0*" PRIx64 " (%" PRId64 ")\n",
               d_bits / 4,
               static_cast<uint64_t>(expected[d]),
               static_cast<int64_t>(expected[d]));
        printf("  Found:    0x%0*" PRIx64 " (%" PRId64 ")\n",
               d_bits / 4,
               static_cast<uint64_t>(results[d]),
               static_cast<int64_t>(results[d]));
        printf("\n");
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


// Test FP instructions.
//  - The inputs[] array should be an array of rawbits representations of
//    doubles or floats. This ensures that exact bit comparisons can be
//    performed.
//  - The expected[] array should be an array of unsigned integers.
template <typename Tn, typename Td>
static void TestFPToU(const char* name,
                      TestFPToIntHelper_t helper,
                      const Tn inputs[],
                      unsigned inputs_length,
                      const Td expected[],
                      unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length;
  Td* results = new Td[results_length];

  const unsigned d_bits = sizeof(Td) * 8;
  const unsigned n_bits = sizeof(Tn) * 8;
  bool skipped;

  TestFPToInt_Helper(helper,
                     reinterpret_cast<uintptr_t>(inputs),
                     inputs_length,
                     reinterpret_cast<uintptr_t>(results),
                     d_bits,
                     n_bits,
                     &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_%s[] = {\n", d_bits, name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  %" PRIu64 "u,\n", static_cast<uint64_t>(results[d]));
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++, d++) {
      if (results[d] != expected[d]) {
        if (++error_count > kErrorReportLimit) continue;

        printf("%s 0x%0*" PRIx64 " (%s %g):\n",
               name,
               n_bits / 4,
               static_cast<uint64_t>(inputs[n]),
               name,
               rawbits_to_fp(inputs[n]));
        printf("  Expected: 0x%0*" PRIx64 " (%" PRIu64 ")\n",
               d_bits / 4,
               static_cast<uint64_t>(expected[d]),
               static_cast<uint64_t>(expected[d]));
        printf("  Found:    0x%0*" PRIx64 " (%" PRIu64 ")\n",
               d_bits / 4,
               static_cast<uint64_t>(results[d]),
               static_cast<uint64_t>(results[d]));
        printf("\n");
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


// Test FP instructions.
//  - The inputs[] array should be an array of rawbits representations of
//    doubles or floats. This ensures that exact bit comparisons can be
//    performed.
//  - The expected[] array should be an array of signed integers.
template <typename Tn, typename Td>
static void TestFPToFixedS(const char* name,
                           TestFPToFixedHelper_t helper,
                           const Tn inputs[],
                           unsigned inputs_length,
                           const Td expected[],
                           unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned d_bits = sizeof(Td) * 8;
  const unsigned n_bits = sizeof(Tn) * 8;

  const unsigned results_length = inputs_length * (d_bits + 1);
  Td* results = new Td[results_length];

  bool skipped;

  TestFPToFixed_Helper(helper,
                       reinterpret_cast<uintptr_t>(inputs),
                       inputs_length,
                       reinterpret_cast<uintptr_t>(results),
                       d_bits,
                       n_bits,
                       &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const int%u_t kExpected_%s[] = {\n", d_bits, name);
    // There is no simple C++ literal for INT*_MIN that doesn't produce
    // warnings, so we use an appropriate constant in that case instead.
    // Deriving int_d_min in this way (rather than just checking INT64_MIN and
    // the like) avoids warnings about comparing values with differing ranges.
    const int64_t int_d_max = (UINT64_C(1) << (d_bits - 1)) - 1;
    const int64_t int_d_min = -(int_d_max)-1;
    for (unsigned d = 0; d < results_length; d++) {
      if (results[d] == int_d_min) {
        printf("  -INT%u_C(%" PRId64 ") - 1,\n", d_bits, int_d_max);
      } else {
        // Some constants (such as those between INT32_MAX and UINT32_MAX)
        // trigger compiler warnings. To avoid these warnings, use an
        // appropriate macro to make the type explicit.
        int64_t result_int64 = static_cast<int64_t>(results[d]);
        if (result_int64 >= 0) {
          printf("  INT%u_C(%" PRId64 "),\n", d_bits, result_int64);
        } else {
          printf("  -INT%u_C(%" PRId64 "),\n", d_bits, -result_int64);
        }
      }
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++) {
      for (unsigned fbits = 0; fbits <= d_bits; ++fbits, d++) {
        if (results[d] != expected[d]) {
          if (++error_count > kErrorReportLimit) continue;

          printf("%s 0x%0*" PRIx64 " #%d (%s %g #%d):\n",
                 name,
                 n_bits / 4,
                 static_cast<uint64_t>(inputs[n]),
                 fbits,
                 name,
                 rawbits_to_fp(inputs[n]),
                 fbits);
          printf("  Expected: 0x%0*" PRIx64 " (%" PRId64 ")\n",
                 d_bits / 4,
                 static_cast<uint64_t>(expected[d]),
                 static_cast<int64_t>(expected[d]));
          printf("  Found:    0x%0*" PRIx64 " (%" PRId64 ")\n",
                 d_bits / 4,
                 static_cast<uint64_t>(results[d]),
                 static_cast<int64_t>(results[d]));
          printf("\n");
        }
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


// Test FP instructions.
//  - The inputs[] array should be an array of rawbits representations of
//    doubles or floats. This ensures that exact bit comparisons can be
//    performed.
//  - The expected[] array should be an array of unsigned integers.
template <typename Tn, typename Td>
static void TestFPToFixedU(const char* name,
                           TestFPToFixedHelper_t helper,
                           const Tn inputs[],
                           unsigned inputs_length,
                           const Td expected[],
                           unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned d_bits = sizeof(Td) * 8;
  const unsigned n_bits = sizeof(Tn) * 8;

  const unsigned results_length = inputs_length * (d_bits + 1);
  Td* results = new Td[results_length];

  bool skipped;

  TestFPToFixed_Helper(helper,
                       reinterpret_cast<uintptr_t>(inputs),
                       inputs_length,
                       reinterpret_cast<uintptr_t>(results),
                       d_bits,
                       n_bits,
                       &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_%s[] = {\n", d_bits, name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  %" PRIu64 "u,\n", static_cast<uint64_t>(results[d]));
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++) {
      for (unsigned fbits = 0; fbits <= d_bits; ++fbits, d++) {
        if (results[d] != expected[d]) {
          if (++error_count > kErrorReportLimit) continue;

          printf("%s 0x%0*" PRIx64 " #%d (%s %g #%d):\n",
                 name,
                 n_bits / 4,
                 static_cast<uint64_t>(inputs[n]),
                 fbits,
                 name,
                 rawbits_to_fp(inputs[n]),
                 fbits);
          printf("  Expected: 0x%0*" PRIx64 " (%" PRIu64 ")\n",
                 d_bits / 4,
                 static_cast<uint64_t>(expected[d]),
                 static_cast<uint64_t>(expected[d]));
          printf("  Found:    0x%0*" PRIx64 " (%" PRIu64 ")\n",
                 d_bits / 4,
                 static_cast<uint64_t>(results[d]),
                 static_cast<uint64_t>(results[d]));
          printf("\n");
        }
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


// ==== Tests for instructions of the form <INST> VReg, VReg. ====


static void Test1OpNEON_Helper(Test1OpNEONHelper_t helper,
                               uintptr_t inputs_n,
                               unsigned inputs_n_length,
                               uintptr_t results,
                               VectorFormat vd_form,
                               VectorFormat vn_form,
                               bool* skipped) {
  VIXL_ASSERT(vd_form != kFormatUndefined);
  VIXL_ASSERT(vn_form != kFormatUndefined);

  CPUFeatures features;
  features.Combine(CPUFeatures::kNEON,
                   CPUFeatures::kFP,
                   CPUFeatures::kRDM,
                   CPUFeatures::kNEONHalf);
  // For frint{32,64}{x,y} variants.
  features.Combine(CPUFeatures::kFrintToFixedSizedInt);
  SETUP_WITH_FEATURES(features);
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n;

  Register out = x0;
  Register inputs_n_base = x1;
  Register inputs_n_last_16bytes = x3;
  Register index_n = x5;

  // TODO: Refactor duplicate definitions below with a VRegister::As() routine.
  const unsigned vd_bits = RegisterSizeInBitsFromFormat(vd_form);
  const unsigned vd_lane_count = LaneCountFromFormat(vd_form);

  const unsigned vn_bits = RegisterSizeInBitsFromFormat(vn_form);
  const unsigned vn_lane_count = LaneCountFromFormat(vn_form);
  const unsigned vn_lane_bytes = LaneSizeInBytesFromFormat(vn_form);
  const unsigned vn_lane_bytes_log2 = LaneSizeInBytesLog2FromFormat(vn_form);
  const unsigned vn_lane_bits = LaneSizeInBitsFromFormat(vn_form);


  // These will be either a D- or a Q-register form, with a single lane
  // (for use in scalar load and store operations).
  VRegister vd = VRegister(0, vd_bits);
  VRegister vn = v1.V16B();
  VRegister vntmp = v3.V16B();

  // These will have the correct format for use when calling 'helper'.
  VRegister vd_helper = VRegister(0, vd_bits, vd_lane_count);
  VRegister vn_helper = VRegister(1, vn_bits, vn_lane_count);

  // 'v*tmp_single' will be either 'Vt.B', 'Vt.H', 'Vt.S' or 'Vt.D'.
  VRegister vntmp_single = VRegister(3, vn_lane_bits);

  __ Mov(out, results);

  __ Mov(inputs_n_base, inputs_n);
  __ Mov(inputs_n_last_16bytes,
         inputs_n + (vn_lane_bytes * inputs_n_length) - 16);

  __ Ldr(vn, MemOperand(inputs_n_last_16bytes));

  __ Mov(index_n, 0);
  __ Bind(&loop_n);

  __ Ldr(vntmp_single,
         MemOperand(inputs_n_base, index_n, LSL, vn_lane_bytes_log2));
  __ Ext(vn, vn, vntmp, vn_lane_bytes);

  // Set the destination to zero.
  // TODO: Setting the destination to values other than zero
  //       might be a better test for instructions such as sqxtn2
  //       which may leave parts of V registers unchanged.
  __ Movi(vd.V16B(), 0);

  {
    SingleEmissionCheckScope guard(&masm);
    (masm.*helper)(vd_helper, vn_helper);
  }
  __ Str(vd, MemOperand(out, vd.GetSizeInBytes(), PostIndex));

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_n_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}


// Test NEON instructions. The inputs_*[] and expected[] arrays should be
// arrays of rawbit representation of input values. This ensures that
// exact bit comparisons can be performed.
template <typename Td, typename Tn>
static void Test1OpNEON(const char* name,
                        Test1OpNEONHelper_t helper,
                        const Tn inputs_n[],
                        unsigned inputs_n_length,
                        const Td expected[],
                        unsigned expected_length,
                        VectorFormat vd_form,
                        VectorFormat vn_form) {
  VIXL_ASSERT(inputs_n_length > 0);

  const unsigned vd_lane_count = LaneCountFromFormat(vd_form);
  const unsigned vn_lane_bytes = LaneSizeInBytesFromFormat(vn_form);
  const unsigned vn_lane_count = LaneCountFromFormat(vn_form);

  const unsigned results_length = inputs_n_length;
  Td* results = new Td[results_length * vd_lane_count];
  const unsigned lane_bit = sizeof(Td) * 8;
  const unsigned lane_len_in_hex = MaxHexCharCount<Td, Tn>();

  bool skipped;

  Test1OpNEON_Helper(helper,
                     reinterpret_cast<uintptr_t>(inputs_n),
                     inputs_n_length,
                     reinterpret_cast<uintptr_t>(results),
                     vd_form,
                     vn_form,
                     &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_NEON_%s[] = {\n", lane_bit, name);
    for (unsigned iteration = 0; iteration < results_length; iteration++) {
      printf(" ");
      // Output a separate result for each element of the result vector.
      for (unsigned lane = 0; lane < vd_lane_count; lane++) {
        unsigned index = lane + (iteration * vd_lane_count);
        printf(" 0x%0*" PRIx64 ",",
               lane_len_in_hex,
               static_cast<uint64_t>(results[index]));
      }
      printf("\n");
    }

    printf("};\n");
    printf("const unsigned kExpectedCount_NEON_%s = %u;\n",
           name,
           results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    const char* padding = "                    ";
    VIXL_ASSERT(strlen(padding) >= (lane_len_in_hex + 1));
    for (unsigned n = 0; n < inputs_n_length; n++, d++) {
      bool error_in_vector = false;

      for (unsigned lane = 0; lane < vd_lane_count; lane++) {
        unsigned output_index = (n * vd_lane_count) + lane;

        if (results[output_index] != expected[output_index]) {
          error_in_vector = true;
          break;
        }
      }

      if (error_in_vector && (++error_count <= kErrorReportLimit)) {
        printf("%s\n", name);
        printf(" Vn%.*s| Vd%.*s| Expected\n",
               lane_len_in_hex + 1,
               padding,
               lane_len_in_hex + 1,
               padding);

        const unsigned first_index_n =
            inputs_n_length - (16 / vn_lane_bytes) + n + 1;

        for (unsigned lane = 0; lane < std::max(vd_lane_count, vn_lane_count);
             lane++) {
          unsigned output_index = (n * vd_lane_count) + lane;
          unsigned input_index_n = (first_index_n + lane) % inputs_n_length;

          printf("%c0x%0*" PRIx64 " | 0x%0*" PRIx64
                 " "
                 "| 0x%0*" PRIx64 "\n",
                 results[output_index] != expected[output_index] ? '*' : ' ',
                 lane_len_in_hex,
                 static_cast<uint64_t>(inputs_n[input_index_n]),
                 lane_len_in_hex,
                 static_cast<uint64_t>(results[output_index]),
                 lane_len_in_hex,
                 static_cast<uint64_t>(expected[output_index]));
        }
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


// ==== Tests for instructions of the form <mnemonic> <V><d>, <Vn>.<T> ====
//      where <V> is one of B, H, S or D registers.
//      e.g. saddlv H1, v0.8B

// TODO: Change tests to store all lanes of the resulting V register.
//       Some tests store all 128 bits of the resulting V register to
//       check the simulator's behaviour on the rest of the register.
//       This is better than storing the affected lanes only.
//       Change any tests such as the 'Across' template to do the same.

static void Test1OpAcrossNEON_Helper(Test1OpNEONHelper_t helper,
                                     uintptr_t inputs_n,
                                     unsigned inputs_n_length,
                                     uintptr_t results,
                                     VectorFormat vd_form,
                                     VectorFormat vn_form,
                                     bool* skipped) {
  VIXL_ASSERT(vd_form != kFormatUndefined);
  VIXL_ASSERT(vn_form != kFormatUndefined);

  SETUP_WITH_FEATURES(CPUFeatures::kNEON,
                      CPUFeatures::kFP,
                      CPUFeatures::kNEONHalf);
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n;

  Register out = x0;
  Register inputs_n_base = x1;
  Register inputs_n_last_vector = x3;
  Register index_n = x5;

  // TODO: Refactor duplicate definitions below with a VRegister::As() routine.
  const unsigned vd_bits = RegisterSizeInBitsFromFormat(vd_form);
  const unsigned vn_bits = RegisterSizeInBitsFromFormat(vn_form);
  const unsigned vn_lane_count = LaneCountFromFormat(vn_form);
  const unsigned vn_lane_bytes = LaneSizeInBytesFromFormat(vn_form);
  const unsigned vn_lane_bytes_log2 = LaneSizeInBytesLog2FromFormat(vn_form);
  const unsigned vn_lane_bits = LaneSizeInBitsFromFormat(vn_form);

  // Test destructive operations by (arbitrarily) using the same register for
  // B and S lane sizes.
  bool destructive = (vd_bits == kBRegSize) || (vd_bits == kSRegSize);

  // Create two aliases for v0; the first is the destination for the tested
  // instruction, the second, the whole Q register to check the results.
  VRegister vd = VRegister(0, vd_bits);
  VRegister vdstr = VRegister(0, kQRegSize);

  VRegister vn = VRegister(1, vn_bits);
  VRegister vntmp = VRegister(3, vn_bits);

  // These will have the correct format for use when calling 'helper'.
  VRegister vd_helper = VRegister(0, vn_bits, vn_lane_count);
  VRegister vn_helper = VRegister(1, vn_bits, vn_lane_count);

  // 'v*tmp_single' will be either 'Vt.B', 'Vt.H', 'Vt.S' or 'Vt.D'.
  VRegister vntmp_single = VRegister(3, vn_lane_bits);

  // Same registers for use in the 'ext' instructions.
  VRegister vn_ext = (kDRegSize == vn_bits) ? vn.V8B() : vn.V16B();
  VRegister vntmp_ext = (kDRegSize == vn_bits) ? vntmp.V8B() : vntmp.V16B();

  __ Mov(out, results);

  __ Mov(inputs_n_base, inputs_n);
  __ Mov(inputs_n_last_vector,
         inputs_n + vn_lane_bytes * (inputs_n_length - vn_lane_count));

  __ Ldr(vn, MemOperand(inputs_n_last_vector));

  __ Mov(index_n, 0);
  __ Bind(&loop_n);

  __ Ldr(vntmp_single,
         MemOperand(inputs_n_base, index_n, LSL, vn_lane_bytes_log2));
  __ Ext(vn_ext, vn_ext, vntmp_ext, vn_lane_bytes);

  if (destructive) {
    __ Mov(vd_helper, vn_helper);
    SingleEmissionCheckScope guard(&masm);
    (masm.*helper)(vd, vd_helper);
  } else {
    SingleEmissionCheckScope guard(&masm);
    (masm.*helper)(vd, vn_helper);
  }

  __ Str(vdstr, MemOperand(out, kQRegSizeInBytes, PostIndex));

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_n_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}

// Test NEON instructions. The inputs_*[] and expected[] arrays should be
// arrays of rawbit representation of input values. This ensures that
// exact bit comparisons can be performed.
template <typename Td, typename Tn>
static void Test1OpAcrossNEON(const char* name,
                              Test1OpNEONHelper_t helper,
                              const Tn inputs_n[],
                              unsigned inputs_n_length,
                              const Td expected[],
                              unsigned expected_length,
                              VectorFormat vd_form,
                              VectorFormat vn_form) {
  VIXL_ASSERT(inputs_n_length > 0);

  const unsigned vd_lane_count = LaneCountFromFormat(vd_form);
  const unsigned vd_lanes_per_q = MaxLaneCountFromFormat(vd_form);

  const unsigned results_length = inputs_n_length;
  Td* results = new Td[results_length * vd_lanes_per_q];
  const unsigned lane_bit = sizeof(Td) * 8;
  const unsigned lane_len_in_hex = MaxHexCharCount<Td, Tn>();

  bool skipped;

  Test1OpAcrossNEON_Helper(helper,
                           reinterpret_cast<uintptr_t>(inputs_n),
                           inputs_n_length,
                           reinterpret_cast<uintptr_t>(results),
                           vd_form,
                           vn_form,
                           &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_NEON_%s[] = {\n", lane_bit, name);
    for (unsigned iteration = 0; iteration < results_length; iteration++) {
      printf(" ");
      // Output a separate result for each element of the result vector.
      for (unsigned lane = 0; lane < vd_lane_count; lane++) {
        unsigned index = lane + (iteration * vd_lanes_per_q);
        printf(" 0x%0*" PRIx64 ",",
               lane_len_in_hex,
               static_cast<uint64_t>(results[index]));
      }
      printf("\n");
    }

    printf("};\n");
    printf("const unsigned kExpectedCount_NEON_%s = %u;\n",
           name,
           results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    const char* padding = "                    ";
    VIXL_ASSERT(strlen(padding) >= (lane_len_in_hex + 1));
    for (unsigned n = 0; n < inputs_n_length; n++, d++) {
      bool error_in_vector = false;

      for (unsigned lane = 0; lane < vd_lane_count; lane++) {
        unsigned expected_index = (n * vd_lane_count) + lane;
        unsigned results_index = (n * vd_lanes_per_q) + lane;

        if (results[results_index] != expected[expected_index]) {
          error_in_vector = true;
          break;
        }
      }

      // For across operations, the remaining lanes should be zero.
      for (unsigned lane = vd_lane_count; lane < vd_lanes_per_q; lane++) {
        unsigned results_index = (n * vd_lanes_per_q) + lane;
        if (results[results_index] != 0) {
          error_in_vector = true;
          break;
        }
      }

      if (error_in_vector && (++error_count <= kErrorReportLimit)) {
        const unsigned vn_lane_count = LaneCountFromFormat(vn_form);

        printf("%s\n", name);
        printf(" Vn%.*s| Vd%.*s| Expected\n",
               lane_len_in_hex + 1,
               padding,
               lane_len_in_hex + 1,
               padding);

        // TODO: In case of an error, all tests print out as many elements as
        //       there are lanes in the output or input vectors. This way
        //       the viewer can read all the values that were needed for the
        //       operation but the output contains also unnecessary values.
        //       These prints can be improved according to the arguments
        //       passed to test functions.
        //       This output for the 'Across' category has the required
        //       modifications.
        for (unsigned lane = 0; lane < vn_lane_count; lane++) {
          unsigned results_index =
              (n * vd_lanes_per_q) + ((vn_lane_count - 1) - lane);
          unsigned input_index_n =
              (inputs_n_length - vn_lane_count + n + 1 + lane) %
              inputs_n_length;

          Td expect = 0;
          if ((vn_lane_count - 1) == lane) {
            // This is the last lane to be printed, ie. the least-significant
            // lane, so use the expected value; any other lane should be zero.
            unsigned expected_index = n * vd_lane_count;
            expect = expected[expected_index];
          }
          printf("%c0x%0*" PRIx64 " | 0x%0*" PRIx64 " | 0x%0*" PRIx64 "\n",
                 results[results_index] != expect ? '*' : ' ',
                 lane_len_in_hex,
                 static_cast<uint64_t>(inputs_n[input_index_n]),
                 lane_len_in_hex,
                 static_cast<uint64_t>(results[results_index]),
                 lane_len_in_hex,
                 static_cast<uint64_t>(expect));
        }
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


// ==== Tests for instructions of the form <INST> VReg, VReg, VReg. ====

// TODO: Iterate over inputs_d once the traces file is split.

static void Test2OpNEON_Helper(Test2OpNEONHelper_t helper,
                               uintptr_t inputs_d,
                               uintptr_t inputs_n,
                               unsigned inputs_n_length,
                               uintptr_t inputs_m,
                               unsigned inputs_m_length,
                               uintptr_t results,
                               VectorFormat vd_form,
                               VectorFormat vn_form,
                               VectorFormat vm_form,
                               bool* skipped) {
  VIXL_ASSERT(vd_form != kFormatUndefined);
  VIXL_ASSERT(vn_form != kFormatUndefined);
  VIXL_ASSERT(vm_form != kFormatUndefined);

  CPUFeatures features;
  features.Combine(CPUFeatures::kNEON, CPUFeatures::kNEONHalf);
  features.Combine(CPUFeatures::kFP);
  features.Combine(CPUFeatures::kRDM);
  features.Combine(CPUFeatures::kDotProduct);
  features.Combine(CPUFeatures::kFHM);
  SETUP_WITH_FEATURES(features);
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n, loop_m;

  Register out = x0;
  Register inputs_n_base = x1;
  Register inputs_m_base = x2;
  Register inputs_d_base = x3;
  Register inputs_n_last_16bytes = x4;
  Register inputs_m_last_16bytes = x5;
  Register index_n = x6;
  Register index_m = x7;

  // TODO: Refactor duplicate definitions below with a VRegister::As() routine.
  const unsigned vd_bits = RegisterSizeInBitsFromFormat(vd_form);
  const unsigned vd_lane_count = LaneCountFromFormat(vd_form);

  const unsigned vn_bits = RegisterSizeInBitsFromFormat(vn_form);
  const unsigned vn_lane_count = LaneCountFromFormat(vn_form);
  const unsigned vn_lane_bytes = LaneSizeInBytesFromFormat(vn_form);
  const unsigned vn_lane_bytes_log2 = LaneSizeInBytesLog2FromFormat(vn_form);
  const unsigned vn_lane_bits = LaneSizeInBitsFromFormat(vn_form);

  const unsigned vm_bits = RegisterSizeInBitsFromFormat(vm_form);
  const unsigned vm_lane_count = LaneCountFromFormat(vm_form);
  const unsigned vm_lane_bytes = LaneSizeInBytesFromFormat(vm_form);
  const unsigned vm_lane_bytes_log2 = LaneSizeInBytesLog2FromFormat(vm_form);
  const unsigned vm_lane_bits = LaneSizeInBitsFromFormat(vm_form);


  // Always load and store 128 bits regardless of the format.
  VRegister vd = v0.V16B();
  VRegister vn = v1.V16B();
  VRegister vm = v2.V16B();
  VRegister vntmp = v3.V16B();
  VRegister vmtmp = v4.V16B();
  VRegister vres = v5.V16B();

  // These will have the correct format for calling the 'helper'.
  VRegister vn_helper = VRegister(1, vn_bits, vn_lane_count);
  VRegister vm_helper = VRegister(2, vm_bits, vm_lane_count);
  VRegister vres_helper = VRegister(5, vd_bits, vd_lane_count);

  // 'v*tmp_single' will be either 'Vt.B', 'Vt.H', 'Vt.S' or 'Vt.D'.
  VRegister vntmp_single = VRegister(3, vn_lane_bits);
  VRegister vmtmp_single = VRegister(4, vm_lane_bits);

  __ Mov(out, results);

  __ Mov(inputs_d_base, inputs_d);

  __ Mov(inputs_n_base, inputs_n);
  __ Mov(inputs_n_last_16bytes, inputs_n + (inputs_n_length - 16));
  __ Mov(inputs_m_base, inputs_m);
  __ Mov(inputs_m_last_16bytes, inputs_m + (inputs_m_length - 16));

  __ Ldr(vd, MemOperand(inputs_d_base));
  __ Ldr(vn, MemOperand(inputs_n_last_16bytes));
  __ Ldr(vm, MemOperand(inputs_m_last_16bytes));

  __ Mov(index_n, 0);
  __ Bind(&loop_n);

  __ Ldr(vntmp_single,
         MemOperand(inputs_n_base, index_n, LSL, vn_lane_bytes_log2));
  __ Ext(vn, vn, vntmp, vn_lane_bytes);

  __ Mov(index_m, 0);
  __ Bind(&loop_m);

  __ Ldr(vmtmp_single,
         MemOperand(inputs_m_base, index_m, LSL, vm_lane_bytes_log2));
  __ Ext(vm, vm, vmtmp, vm_lane_bytes);

  __ Mov(vres, vd);
  {
    SingleEmissionCheckScope guard(&masm);
    (masm.*helper)(vres_helper, vn_helper, vm_helper);
  }
  __ Str(vres, MemOperand(out, vd.GetSizeInBytes(), PostIndex));

  __ Add(index_m, index_m, 1);
  __ Cmp(index_m, inputs_m_length);
  __ B(lo, &loop_m);

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_n_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}


// Test NEON instructions. The inputs_*[] and expected[] arrays should be
// arrays of rawbit representation of input values. This ensures that
// exact bit comparisons can be performed.
template <typename Td, typename Tn, typename Tm>
static void Test2OpNEON(const char* name,
                        Test2OpNEONHelper_t helper,
                        const Td inputs_d[],
                        const Tn inputs_n[],
                        unsigned inputs_n_length,
                        const Tm inputs_m[],
                        unsigned inputs_m_length,
                        const Td expected[],
                        unsigned expected_length,
                        VectorFormat vd_form,
                        VectorFormat vn_form,
                        VectorFormat vm_form) {
  VIXL_ASSERT(inputs_n_length > 0 && inputs_m_length > 0);

  const unsigned vd_lane_count = MaxLaneCountFromFormat(vd_form);

  const unsigned results_length = inputs_n_length * inputs_m_length;
  Td* results = new Td[results_length * vd_lane_count];
  const unsigned lane_bit = sizeof(Td) * 8;
  const unsigned lane_len_in_hex = MaxHexCharCount<Td, Tm>();

  bool skipped;

  Test2OpNEON_Helper(helper,
                     reinterpret_cast<uintptr_t>(inputs_d),
                     reinterpret_cast<uintptr_t>(inputs_n),
                     inputs_n_length,
                     reinterpret_cast<uintptr_t>(inputs_m),
                     inputs_m_length,
                     reinterpret_cast<uintptr_t>(results),
                     vd_form,
                     vn_form,
                     vm_form,
                     &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_NEON_%s[] = {\n", lane_bit, name);
    for (unsigned iteration = 0; iteration < results_length; iteration++) {
      printf(" ");
      // Output a separate result for each element of the result vector.
      for (unsigned lane = 0; lane < vd_lane_count; lane++) {
        unsigned index = lane + (iteration * vd_lane_count);
        printf(" 0x%0*" PRIx64 ",",
               lane_len_in_hex,
               static_cast<uint64_t>(results[index]));
      }
      printf("\n");
    }

    printf("};\n");
    printf("const unsigned kExpectedCount_NEON_%s = %u;\n",
           name,
           results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    const char* padding = "                    ";
    VIXL_ASSERT(strlen(padding) >= (lane_len_in_hex + 1));
    for (unsigned n = 0; n < inputs_n_length; n++) {
      for (unsigned m = 0; m < inputs_m_length; m++, d++) {
        bool error_in_vector = false;

        for (unsigned lane = 0; lane < vd_lane_count; lane++) {
          unsigned output_index = (n * inputs_m_length * vd_lane_count) +
                                  (m * vd_lane_count) + lane;

          if (results[output_index] != expected[output_index]) {
            error_in_vector = true;
            break;
          }
        }

        if (error_in_vector && (++error_count <= kErrorReportLimit)) {
          printf("%s\n", name);
          printf(" Vd%.*s| Vn%.*s| Vm%.*s| Vd%.*s| Expected\n",
                 lane_len_in_hex + 1,
                 padding,
                 lane_len_in_hex + 1,
                 padding,
                 lane_len_in_hex + 1,
                 padding,
                 lane_len_in_hex + 1,
                 padding);

          for (unsigned lane = 0; lane < vd_lane_count; lane++) {
            unsigned output_index = (n * inputs_m_length * vd_lane_count) +
                                    (m * vd_lane_count) + lane;
            unsigned input_index_n =
                (inputs_n_length - vd_lane_count + n + 1 + lane) %
                inputs_n_length;
            unsigned input_index_m =
                (inputs_m_length - vd_lane_count + m + 1 + lane) %
                inputs_m_length;

            printf("%c0x%0*" PRIx64 " | 0x%0*" PRIx64 " | 0x%0*" PRIx64
                   " "
                   "| 0x%0*" PRIx64 " | 0x%0*" PRIx64 "\n",
                   results[output_index] != expected[output_index] ? '*' : ' ',
                   lane_len_in_hex,
                   static_cast<uint64_t>(inputs_d[lane]),
                   lane_len_in_hex,
                   static_cast<uint64_t>(inputs_n[input_index_n]),
                   lane_len_in_hex,
                   static_cast<uint64_t>(inputs_m[input_index_m]),
                   lane_len_in_hex,
                   static_cast<uint64_t>(results[output_index]),
                   lane_len_in_hex,
                   static_cast<uint64_t>(expected[output_index]));
          }
        }
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


// ==== Tests for instructions of the form <INST> Vd, Vn, Vm[<#index>]. ====

static void TestByElementNEON_Helper(TestByElementNEONHelper_t helper,
                                     uintptr_t inputs_d,
                                     uintptr_t inputs_n,
                                     unsigned inputs_n_length,
                                     uintptr_t inputs_m,
                                     unsigned inputs_m_length,
                                     const int indices[],
                                     unsigned indices_length,
                                     uintptr_t results,
                                     VectorFormat vd_form,
                                     VectorFormat vn_form,
                                     VectorFormat vm_form,
                                     unsigned vm_subvector_count,
                                     bool* skipped) {
  VIXL_ASSERT(vd_form != kFormatUndefined);
  VIXL_ASSERT(vn_form != kFormatUndefined);
  VIXL_ASSERT(vm_form != kFormatUndefined);
  VIXL_ASSERT((vm_subvector_count != 0) && IsPowerOf2(vm_subvector_count));

  CPUFeatures features;
  features.Combine(CPUFeatures::kNEON, CPUFeatures::kNEONHalf);
  features.Combine(CPUFeatures::kFP);
  features.Combine(CPUFeatures::kRDM);
  features.Combine(CPUFeatures::kDotProduct);
  features.Combine(CPUFeatures::kFHM);
  SETUP_WITH_FEATURES(features);

  START();

  // Roll up the loop to keep the code size down.
  Label loop_n, loop_m;

  Register out = x0;
  Register inputs_n_base = x1;
  Register inputs_m_base = x2;
  Register inputs_d_base = x3;
  Register inputs_n_last_16bytes = x4;
  Register inputs_m_last_16bytes = x5;
  Register index_n = x6;
  Register index_m = x7;

  // TODO: Refactor duplicate definitions below with a VRegister::As() routine.
  const unsigned vd_bits = RegisterSizeInBitsFromFormat(vd_form);
  const unsigned vd_lane_count = LaneCountFromFormat(vd_form);

  const unsigned vn_bits = RegisterSizeInBitsFromFormat(vn_form);
  const unsigned vn_lane_count = LaneCountFromFormat(vn_form);
  const unsigned vn_lane_bytes = LaneSizeInBytesFromFormat(vn_form);
  const unsigned vn_lane_bytes_log2 = LaneSizeInBytesLog2FromFormat(vn_form);
  const unsigned vn_lane_bits = LaneSizeInBitsFromFormat(vn_form);

  const unsigned vm_bits = RegisterSizeInBitsFromFormat(vm_form);
  const unsigned vm_lane_count = LaneCountFromFormat(vm_form);
  const unsigned vm_lane_bytes = LaneSizeInBytesFromFormat(vm_form);
  const unsigned vm_lane_bytes_log2 = LaneSizeInBytesLog2FromFormat(vm_form);
  const unsigned vm_lane_bits = LaneSizeInBitsFromFormat(vm_form);

  VIXL_ASSERT((vm_bits * vm_subvector_count) <= kQRegSize);

  // Always load and store 128 bits regardless of the format.
  VRegister vd = v0.V16B();
  VRegister vn = v1.V16B();
  VRegister vm = v2.V16B();
  VRegister vntmp = v3.V16B();
  VRegister vmtmp = v4.V16B();
  VRegister vres = v5.V16B();

  // These will have the correct format for calling the 'helper'.
  VRegister vn_helper = VRegister(1, vn_bits, vn_lane_count);
  VRegister vm_helper =
      VRegister(2, vm_bits * vm_subvector_count, vm_lane_count);
  VRegister vres_helper = VRegister(5, vd_bits, vd_lane_count);

  // 'v*tmp_single' will be either 'Vt.B', 'Vt.H', 'Vt.S' or 'Vt.D'.
  VRegister vntmp_single = VRegister(3, vn_lane_bits);
  VRegister vmtmp_single = VRegister(4, vm_lane_bits);

  __ Mov(out, results);

  __ Mov(inputs_d_base, inputs_d);

  __ Mov(inputs_n_base, inputs_n);
  __ Mov(inputs_n_last_16bytes, inputs_n + (inputs_n_length - 16));
  __ Mov(inputs_m_base, inputs_m);
  __ Mov(inputs_m_last_16bytes, inputs_m + (inputs_m_length - 16));

  __ Ldr(vd, MemOperand(inputs_d_base));
  __ Ldr(vn, MemOperand(inputs_n_last_16bytes));
  __ Ldr(vm, MemOperand(inputs_m_last_16bytes));

  __ Mov(index_n, 0);
  __ Bind(&loop_n);

  __ Ldr(vntmp_single,
         MemOperand(inputs_n_base, index_n, LSL, vn_lane_bytes_log2));
  __ Ext(vn, vn, vntmp, vn_lane_bytes);

  __ Mov(index_m, 0);
  __ Bind(&loop_m);

  __ Ldr(vmtmp_single,
         MemOperand(inputs_m_base, index_m, LSL, vm_lane_bytes_log2));
  __ Ext(vm, vm, vmtmp, vm_lane_bytes);

  __ Mov(vres, vd);
  {
    for (unsigned i = 0; i < indices_length; i++) {
      {
        SingleEmissionCheckScope guard(&masm);
        (masm.*helper)(vres_helper, vn_helper, vm_helper, indices[i]);
      }
      __ Str(vres, MemOperand(out, vd.GetSizeInBytes(), PostIndex));
    }
  }

  __ Add(index_m, index_m, 1);
  __ Cmp(index_m, inputs_m_length);
  __ B(lo, &loop_m);

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_n_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}


// Test NEON instructions. The inputs_*[] and expected[] arrays should be
// arrays of rawbit representation of input values. This ensures that
// exact bit comparisons can be performed.
template <typename Td, typename Tn, typename Tm>
static void TestByElementNEON(const char* name,
                              TestByElementNEONHelper_t helper,
                              const Td inputs_d[],
                              const Tn inputs_n[],
                              unsigned inputs_n_length,
                              const Tm inputs_m[],
                              unsigned inputs_m_length,
                              const int indices[],
                              unsigned indices_length,
                              const Td expected[],
                              unsigned expected_length,
                              VectorFormat vd_form,
                              VectorFormat vn_form,
                              VectorFormat vm_form,
                              unsigned vm_subvector_count = 1) {
  VIXL_ASSERT(inputs_n_length > 0);
  VIXL_ASSERT(inputs_m_length > 0);
  VIXL_ASSERT(indices_length > 0);

  const unsigned vd_lane_count = MaxLaneCountFromFormat(vd_form);

  const unsigned results_length =
      inputs_n_length * inputs_m_length * indices_length;
  Td* results = new Td[results_length * vd_lane_count];
  const unsigned lane_bit = sizeof(Td) * 8;
  const unsigned lane_len_in_hex = MaxHexCharCount<Td, Tm>();

  bool skipped;

  TestByElementNEON_Helper(helper,
                           reinterpret_cast<uintptr_t>(inputs_d),
                           reinterpret_cast<uintptr_t>(inputs_n),
                           inputs_n_length,
                           reinterpret_cast<uintptr_t>(inputs_m),
                           inputs_m_length,
                           indices,
                           indices_length,
                           reinterpret_cast<uintptr_t>(results),
                           vd_form,
                           vn_form,
                           vm_form,
                           vm_subvector_count,
                           &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_NEON_%s[] = {\n", lane_bit, name);
    for (unsigned iteration = 0; iteration < results_length; iteration++) {
      printf(" ");
      // Output a separate result for each element of the result vector.
      for (unsigned lane = 0; lane < vd_lane_count; lane++) {
        unsigned index = lane + (iteration * vd_lane_count);
        printf(" 0x%0*" PRIx64 ",",
               lane_len_in_hex,
               static_cast<uint64_t>(results[index]));
      }
      printf("\n");
    }

    printf("};\n");
    printf("const unsigned kExpectedCount_NEON_%s = %u;\n",
           name,
           results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    const char* padding = "                    ";
    VIXL_ASSERT(strlen(padding) >= (lane_len_in_hex + 1));
    for (unsigned n = 0; n < inputs_n_length; n++) {
      for (unsigned m = 0; m < inputs_m_length; m++) {
        for (unsigned index = 0; index < indices_length; index++, d++) {
          bool error_in_vector = false;

          for (unsigned lane = 0; lane < vd_lane_count; lane++) {
            unsigned output_index =
                (n * inputs_m_length * indices_length * vd_lane_count) +
                (m * indices_length * vd_lane_count) + (index * vd_lane_count) +
                lane;

            if (results[output_index] != expected[output_index]) {
              error_in_vector = true;
              break;
            }
          }

          if (error_in_vector && (++error_count <= kErrorReportLimit)) {
            printf("%s\n", name);
            printf(" Vd%.*s| Vn%.*s| Vm%.*s| Index | Vd%.*s| Expected\n",
                   lane_len_in_hex + 1,
                   padding,
                   lane_len_in_hex + 1,
                   padding,
                   lane_len_in_hex + 1,
                   padding,
                   lane_len_in_hex + 1,
                   padding);

            for (unsigned lane = 0; lane < vd_lane_count; lane++) {
              unsigned output_index =
                  (n * inputs_m_length * indices_length * vd_lane_count) +
                  (m * indices_length * vd_lane_count) +
                  (index * vd_lane_count) + lane;
              unsigned input_index_n =
                  (inputs_n_length - vd_lane_count + n + 1 + lane) %
                  inputs_n_length;
              unsigned input_index_m =
                  (inputs_m_length - vd_lane_count + m + 1 + lane) %
                  inputs_m_length;

              printf("%c0x%0*" PRIx64 " | 0x%0*" PRIx64 " | 0x%0*" PRIx64
                     " "
                     "| [%3d] | 0x%0*" PRIx64 " | 0x%0*" PRIx64 "\n",
                     results[output_index] != expected[output_index] ? '*'
                                                                     : ' ',
                     lane_len_in_hex,
                     static_cast<uint64_t>(inputs_d[lane]),
                     lane_len_in_hex,
                     static_cast<uint64_t>(inputs_n[input_index_n]),
                     lane_len_in_hex,
                     static_cast<uint64_t>(inputs_m[input_index_m]),
                     indices[index],
                     lane_len_in_hex,
                     static_cast<uint64_t>(results[output_index]),
                     lane_len_in_hex,
                     static_cast<uint64_t>(expected[output_index]));
            }
          }
        }
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


// ==== Tests for instructions of the form <INST> VReg, VReg, #Immediate. ====


template <typename Tm>
void Test2OpImmNEON_Helper(
    typename Test2OpImmediateNEONHelper_t<Tm>::mnemonic helper,
    uintptr_t inputs_n,
    unsigned inputs_n_length,
    const Tm inputs_m[],
    unsigned inputs_m_length,
    uintptr_t results,
    VectorFormat vd_form,
    VectorFormat vn_form,
    bool* skipped) {
  VIXL_ASSERT(vd_form != kFormatUndefined && vn_form != kFormatUndefined);

  SETUP_WITH_FEATURES(CPUFeatures::kNEON,
                      CPUFeatures::kFP,
                      CPUFeatures::kNEONHalf);
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n;

  Register out = x0;
  Register inputs_n_base = x1;
  Register inputs_n_last_16bytes = x3;
  Register index_n = x5;

  // TODO: Refactor duplicate definitions below with a VRegister::As() routine.
  const unsigned vd_bits = RegisterSizeInBitsFromFormat(vd_form);
  const unsigned vd_lane_count = LaneCountFromFormat(vd_form);

  const unsigned vn_bits = RegisterSizeInBitsFromFormat(vn_form);
  const unsigned vn_lane_count = LaneCountFromFormat(vn_form);
  const unsigned vn_lane_bytes = LaneSizeInBytesFromFormat(vn_form);
  const unsigned vn_lane_bytes_log2 = LaneSizeInBytesLog2FromFormat(vn_form);
  const unsigned vn_lane_bits = LaneSizeInBitsFromFormat(vn_form);


  // These will be either a D- or a Q-register form, with a single lane
  // (for use in scalar load and store operations).
  VRegister vd = VRegister(0, vd_bits);
  VRegister vn = v1.V16B();
  VRegister vntmp = v3.V16B();

  // These will have the correct format for use when calling 'helper'.
  VRegister vd_helper = VRegister(0, vd_bits, vd_lane_count);
  VRegister vn_helper = VRegister(1, vn_bits, vn_lane_count);

  // 'v*tmp_single' will be either 'Vt.B', 'Vt.H', 'Vt.S' or 'Vt.D'.
  VRegister vntmp_single = VRegister(3, vn_lane_bits);

  __ Mov(out, results);

  __ Mov(inputs_n_base, inputs_n);
  __ Mov(inputs_n_last_16bytes,
         inputs_n + (vn_lane_bytes * inputs_n_length) - 16);

  __ Ldr(vn, MemOperand(inputs_n_last_16bytes));

  __ Mov(index_n, 0);
  __ Bind(&loop_n);

  __ Ldr(vntmp_single,
         MemOperand(inputs_n_base, index_n, LSL, vn_lane_bytes_log2));
  __ Ext(vn, vn, vntmp, vn_lane_bytes);

  // Set the destination to zero for tests such as '[r]shrn2'.
  // TODO: Setting the destination to values other than zero might be a better
  //       test for shift and accumulate instructions (srsra/ssra/usra/ursra).
  __ Movi(vd.V16B(), 0);

  {
    for (unsigned i = 0; i < inputs_m_length; i++) {
      {
        SingleEmissionCheckScope guard(&masm);
        (masm.*helper)(vd_helper, vn_helper, inputs_m[i]);
      }
      __ Str(vd, MemOperand(out, vd.GetSizeInBytes(), PostIndex));
    }
  }

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_n_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}


// Test NEON instructions. The inputs_*[] and expected[] arrays should be
// arrays of rawbit representation of input values. This ensures that
// exact bit comparisons can be performed.
template <typename Td, typename Tn, typename Tm>
static void Test2OpImmNEON(
    const char* name,
    typename Test2OpImmediateNEONHelper_t<Tm>::mnemonic helper,
    const Tn inputs_n[],
    unsigned inputs_n_length,
    const Tm inputs_m[],
    unsigned inputs_m_length,
    const Td expected[],
    unsigned expected_length,
    VectorFormat vd_form,
    VectorFormat vn_form) {
  VIXL_ASSERT(inputs_n_length > 0 && inputs_m_length > 0);

  const unsigned vd_lane_count = LaneCountFromFormat(vd_form);
  const unsigned vn_lane_bytes = LaneSizeInBytesFromFormat(vn_form);
  const unsigned vn_lane_count = LaneCountFromFormat(vn_form);

  const unsigned results_length = inputs_n_length * inputs_m_length;
  Td* results = new Td[results_length * vd_lane_count];
  const unsigned lane_bit = sizeof(Td) * 8;
  const unsigned lane_len_in_hex = MaxHexCharCount<Td, Tn>();

  bool skipped;

  Test2OpImmNEON_Helper(helper,
                        reinterpret_cast<uintptr_t>(inputs_n),
                        inputs_n_length,
                        inputs_m,
                        inputs_m_length,
                        reinterpret_cast<uintptr_t>(results),
                        vd_form,
                        vn_form,
                        &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_NEON_%s[] = {\n", lane_bit, name);
    for (unsigned iteration = 0; iteration < results_length; iteration++) {
      printf(" ");
      // Output a separate result for each element of the result vector.
      for (unsigned lane = 0; lane < vd_lane_count; lane++) {
        unsigned index = lane + (iteration * vd_lane_count);
        printf(" 0x%0*" PRIx64 ",",
               lane_len_in_hex,
               static_cast<uint64_t>(results[index]));
      }
      printf("\n");
    }

    printf("};\n");
    printf("const unsigned kExpectedCount_NEON_%s = %u;\n",
           name,
           results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    const char* padding = "                    ";
    VIXL_ASSERT(strlen(padding) >= (lane_len_in_hex + 1));
    for (unsigned n = 0; n < inputs_n_length; n++) {
      for (unsigned m = 0; m < inputs_m_length; m++, d++) {
        bool error_in_vector = false;

        for (unsigned lane = 0; lane < vd_lane_count; lane++) {
          unsigned output_index = (n * inputs_m_length * vd_lane_count) +
                                  (m * vd_lane_count) + lane;

          if (results[output_index] != expected[output_index]) {
            error_in_vector = true;
            break;
          }
        }

        if (error_in_vector && (++error_count <= kErrorReportLimit)) {
          printf("%s\n", name);
          printf(" Vn%.*s| Imm%.*s| Vd%.*s| Expected\n",
                 lane_len_in_hex + 1,
                 padding,
                 lane_len_in_hex,
                 padding,
                 lane_len_in_hex + 1,
                 padding);

          const unsigned first_index_n =
              inputs_n_length - (16 / vn_lane_bytes) + n + 1;

          for (unsigned lane = 0; lane < std::max(vd_lane_count, vn_lane_count);
               lane++) {
            unsigned output_index = (n * inputs_m_length * vd_lane_count) +
                                    (m * vd_lane_count) + lane;
            unsigned input_index_n = (first_index_n + lane) % inputs_n_length;
            unsigned input_index_m = m;

            printf("%c0x%0*" PRIx64 " | 0x%0*" PRIx64
                   " "
                   "| 0x%0*" PRIx64 " | 0x%0*" PRIx64 "\n",
                   results[output_index] != expected[output_index] ? '*' : ' ',
                   lane_len_in_hex,
                   static_cast<uint64_t>(inputs_n[input_index_n]),
                   lane_len_in_hex,
                   static_cast<uint64_t>(inputs_m[input_index_m]),
                   lane_len_in_hex,
                   static_cast<uint64_t>(results[output_index]),
                   lane_len_in_hex,
                   static_cast<uint64_t>(expected[output_index]));
          }
        }
      }
    }
    VIXL_ASSERT(d == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


// ==== Tests for instructions of the form <INST> VReg, #Imm, VReg, #Imm. ====


static void TestOpImmOpImmNEON_Helper(TestOpImmOpImmVdUpdateNEONHelper_t helper,
                                      uintptr_t inputs_d,
                                      const int inputs_imm1[],
                                      unsigned inputs_imm1_length,
                                      uintptr_t inputs_n,
                                      unsigned inputs_n_length,
                                      const int inputs_imm2[],
                                      unsigned inputs_imm2_length,
                                      uintptr_t results,
                                      VectorFormat vd_form,
                                      VectorFormat vn_form,
                                      bool* skipped) {
  VIXL_ASSERT(vd_form != kFormatUndefined);
  VIXL_ASSERT(vn_form != kFormatUndefined);

  SETUP_WITH_FEATURES(CPUFeatures::kNEON, CPUFeatures::kFP);
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n;

  Register out = x0;
  Register inputs_d_base = x1;
  Register inputs_n_base = x2;
  Register inputs_n_last_vector = x4;
  Register index_n = x6;

  // TODO: Refactor duplicate definitions below with a VRegister::As() routine.
  const unsigned vd_bits = RegisterSizeInBitsFromFormat(vd_form);
  const unsigned vd_lane_count = LaneCountFromFormat(vd_form);

  const unsigned vn_bits = RegisterSizeInBitsFromFormat(vn_form);
  const unsigned vn_lane_count = LaneCountFromFormat(vn_form);
  const unsigned vn_lane_bytes = LaneSizeInBytesFromFormat(vn_form);
  const unsigned vn_lane_bytes_log2 = LaneSizeInBytesLog2FromFormat(vn_form);
  const unsigned vn_lane_bits = LaneSizeInBitsFromFormat(vn_form);


  // These will be either a D- or a Q-register form, with a single lane
  // (for use in scalar load and store operations).
  VRegister vd = VRegister(0, vd_bits);
  VRegister vn = VRegister(1, vn_bits);
  VRegister vntmp = VRegister(4, vn_bits);
  VRegister vres = VRegister(5, vn_bits);

  VRegister vn_helper = VRegister(1, vn_bits, vn_lane_count);
  VRegister vres_helper = VRegister(5, vd_bits, vd_lane_count);

  // 'v*tmp_single' will be either 'Vt.B', 'Vt.H', 'Vt.S' or 'Vt.D'.
  VRegister vntmp_single = VRegister(4, vn_lane_bits);

  // Same registers for use in the 'ext' instructions.
  VRegister vn_ext = (kDRegSize == vn_bits) ? vn.V8B() : vn.V16B();
  VRegister vntmp_ext = (kDRegSize == vn_bits) ? vntmp.V8B() : vntmp.V16B();

  __ Mov(out, results);

  __ Mov(inputs_d_base, inputs_d);

  __ Mov(inputs_n_base, inputs_n);
  __ Mov(inputs_n_last_vector,
         inputs_n + vn_lane_bytes * (inputs_n_length - vn_lane_count));

  __ Ldr(vd, MemOperand(inputs_d_base));

  __ Ldr(vn, MemOperand(inputs_n_last_vector));

  __ Mov(index_n, 0);
  __ Bind(&loop_n);

  __ Ldr(vntmp_single,
         MemOperand(inputs_n_base, index_n, LSL, vn_lane_bytes_log2));
  __ Ext(vn_ext, vn_ext, vntmp_ext, vn_lane_bytes);

  {
    EmissionCheckScope guard(&masm,
                             kInstructionSize * inputs_imm1_length *
                                 inputs_imm2_length * 3);
    for (unsigned i = 0; i < inputs_imm1_length; i++) {
      for (unsigned j = 0; j < inputs_imm2_length; j++) {
        __ Mov(vres, vd);
        (masm.*helper)(vres_helper, inputs_imm1[i], vn_helper, inputs_imm2[j]);
        __ Str(vres, MemOperand(out, vd.GetSizeInBytes(), PostIndex));
      }
    }
  }

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_n_length);
  __ B(lo, &loop_n);

  END();
  TRY_RUN(skipped);
}


// Test NEON instructions. The inputs_*[] and expected[] arrays should be
// arrays of rawbit representation of input values. This ensures that
// exact bit comparisons can be performed.
template <typename Td, typename Tn>
static void TestOpImmOpImmNEON(const char* name,
                               TestOpImmOpImmVdUpdateNEONHelper_t helper,
                               const Td inputs_d[],
                               const int inputs_imm1[],
                               unsigned inputs_imm1_length,
                               const Tn inputs_n[],
                               unsigned inputs_n_length,
                               const int inputs_imm2[],
                               unsigned inputs_imm2_length,
                               const Td expected[],
                               unsigned expected_length,
                               VectorFormat vd_form,
                               VectorFormat vn_form) {
  VIXL_ASSERT(inputs_n_length > 0);
  VIXL_ASSERT(inputs_imm1_length > 0);
  VIXL_ASSERT(inputs_imm2_length > 0);

  const unsigned vd_lane_count = LaneCountFromFormat(vd_form);

  const unsigned results_length =
      inputs_n_length * inputs_imm1_length * inputs_imm2_length;

  Td* results = new Td[results_length * vd_lane_count];
  const unsigned lane_bit = sizeof(Td) * 8;
  const unsigned lane_len_in_hex = MaxHexCharCount<Td, Tn>();

  bool skipped;

  TestOpImmOpImmNEON_Helper(helper,
                            reinterpret_cast<uintptr_t>(inputs_d),
                            inputs_imm1,
                            inputs_imm1_length,
                            reinterpret_cast<uintptr_t>(inputs_n),
                            inputs_n_length,
                            inputs_imm2,
                            inputs_imm2_length,
                            reinterpret_cast<uintptr_t>(results),
                            vd_form,
                            vn_form,
                            &skipped);

  if (Test::generate_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_NEON_%s[] = {\n", lane_bit, name);
    for (unsigned iteration = 0; iteration < results_length; iteration++) {
      printf(" ");
      // Output a separate result for each element of the result vector.
      for (unsigned lane = 0; lane < vd_lane_count; lane++) {
        unsigned index = lane + (iteration * vd_lane_count);
        printf(" 0x%0*" PRIx64 ",",
               lane_len_in_hex,
               static_cast<uint64_t>(results[index]));
      }
      printf("\n");
    }

    printf("};\n");
    printf("const unsigned kExpectedCount_NEON_%s = %u;\n",
           name,
           results_length);
  } else if (!skipped) {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned counted_length = 0;
    const char* padding = "                    ";
    VIXL_ASSERT(strlen(padding) >= (lane_len_in_hex + 1));
    for (unsigned n = 0; n < inputs_n_length; n++) {
      for (unsigned imm1 = 0; imm1 < inputs_imm1_length; imm1++) {
        for (unsigned imm2 = 0; imm2 < inputs_imm2_length; imm2++) {
          bool error_in_vector = false;

          counted_length++;

          for (unsigned lane = 0; lane < vd_lane_count; lane++) {
            unsigned output_index =
                (n * inputs_imm1_length * inputs_imm2_length * vd_lane_count) +
                (imm1 * inputs_imm2_length * vd_lane_count) +
                (imm2 * vd_lane_count) + lane;

            if (results[output_index] != expected[output_index]) {
              error_in_vector = true;
              break;
            }
          }

          if (error_in_vector && (++error_count <= kErrorReportLimit)) {
            printf("%s\n", name);
            printf(" Vd%.*s| Imm%.*s| Vn%.*s| Imm%.*s| Vd%.*s| Expected\n",
                   lane_len_in_hex + 1,
                   padding,
                   lane_len_in_hex,
                   padding,
                   lane_len_in_hex + 1,
                   padding,
                   lane_len_in_hex,
                   padding,
                   lane_len_in_hex + 1,
                   padding);

            for (unsigned lane = 0; lane < vd_lane_count; lane++) {
              unsigned output_index =
                  (n * inputs_imm1_length * inputs_imm2_length *
                   vd_lane_count) +
                  (imm1 * inputs_imm2_length * vd_lane_count) +
                  (imm2 * vd_lane_count) + lane;
              unsigned input_index_n =
                  (inputs_n_length - vd_lane_count + n + 1 + lane) %
                  inputs_n_length;
              unsigned input_index_imm1 = imm1;
              unsigned input_index_imm2 = imm2;

              printf("%c0x%0*" PRIx64 " | 0x%0*" PRIx64 " | 0x%0*" PRIx64
                     " "
                     "| 0x%0*" PRIx64 " | 0x%0*" PRIx64 " | 0x%0*" PRIx64 "\n",
                     results[output_index] != expected[output_index] ? '*'
                                                                     : ' ',
                     lane_len_in_hex,
                     static_cast<uint64_t>(inputs_d[lane]),
                     lane_len_in_hex,
                     static_cast<uint64_t>(inputs_imm1[input_index_imm1]),
                     lane_len_in_hex,
                     static_cast<uint64_t>(inputs_n[input_index_n]),
                     lane_len_in_hex,
                     static_cast<uint64_t>(inputs_imm2[input_index_imm2]),
                     lane_len_in_hex,
                     static_cast<uint64_t>(results[output_index]),
                     lane_len_in_hex,
                     static_cast<uint64_t>(expected[output_index]));
            }
          }
        }
      }
    }
    VIXL_ASSERT(counted_length == expected_length);
    if (error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n", error_count - kErrorReportLimit);
    }
    VIXL_CHECK(error_count == 0);
  }
  delete[] results;
}


// ==== Floating-point tests. ====


// Standard floating-point test expansion for both double- and single-precision
// operations.
#define STRINGIFY(s) #s

#define CALL_TEST_FP_HELPER(mnemonic, variant, type, input) \
  Test##type(STRINGIFY(mnemonic) "_" STRINGIFY(variant),    \
             &MacroAssembler::mnemonic,                     \
             input,                                         \
             sizeof(input) / sizeof(input[0]),              \
             kExpected_##mnemonic##_##variant,              \
             kExpectedCount_##mnemonic##_##variant)

#define DEFINE_TEST_FP(mnemonic, type, input)                    \
  TEST(mnemonic##_d) {                                           \
    CALL_TEST_FP_HELPER(mnemonic, d, type, kInputDouble##input); \
  }                                                              \
  TEST(mnemonic##_s) {                                           \
    CALL_TEST_FP_HELPER(mnemonic, s, type, kInputFloat##input);  \
  }

#define DEFINE_TEST_FP_FP16(mnemonic, type, input)                \
  TEST(mnemonic##_d) {                                            \
    CALL_TEST_FP_HELPER(mnemonic, d, type, kInputDouble##input);  \
  }                                                               \
  TEST(mnemonic##_s) {                                            \
    CALL_TEST_FP_HELPER(mnemonic, s, type, kInputFloat##input);   \
  }                                                               \
  TEST(mnemonic##_h) {                                            \
    CALL_TEST_FP_HELPER(mnemonic, h, type, kInputFloat16##input); \
  }


// TODO: Test with a newer version of valgrind.
//
// Note: valgrind-3.10.0 does not properly interpret libm's fma() on x86_64.
// Therefore this test will be exiting though an ASSERT and thus leaking
// memory.
DEFINE_TEST_FP_FP16(fmadd, 3Op, Basic)
DEFINE_TEST_FP_FP16(fmsub, 3Op, Basic)
DEFINE_TEST_FP_FP16(fnmadd, 3Op, Basic)
DEFINE_TEST_FP_FP16(fnmsub, 3Op, Basic)

DEFINE_TEST_FP_FP16(fadd, 2Op, Basic)
DEFINE_TEST_FP_FP16(fdiv, 2Op, Basic)
DEFINE_TEST_FP_FP16(fmax, 2Op, Basic)
DEFINE_TEST_FP_FP16(fmaxnm, 2Op, Basic)
DEFINE_TEST_FP_FP16(fmin, 2Op, Basic)
DEFINE_TEST_FP_FP16(fminnm, 2Op, Basic)
DEFINE_TEST_FP_FP16(fmul, 2Op, Basic)
DEFINE_TEST_FP_FP16(fsub, 2Op, Basic)
DEFINE_TEST_FP_FP16(fnmul, 2Op, Basic)

DEFINE_TEST_FP_FP16(fabs, 1Op, Basic)
DEFINE_TEST_FP_FP16(fmov, 1Op, Basic)
DEFINE_TEST_FP_FP16(fneg, 1Op, Basic)
DEFINE_TEST_FP_FP16(fsqrt, 1Op, Basic)
DEFINE_TEST_FP(frint32x, 1Op, Conversions)
DEFINE_TEST_FP(frint64x, 1Op, Conversions)
DEFINE_TEST_FP(frint32z, 1Op, Conversions)
DEFINE_TEST_FP(frint64z, 1Op, Conversions)
DEFINE_TEST_FP_FP16(frinta, 1Op, Conversions)
DEFINE_TEST_FP_FP16(frinti, 1Op, Conversions)
DEFINE_TEST_FP_FP16(frintm, 1Op, Conversions)
DEFINE_TEST_FP_FP16(frintn, 1Op, Conversions)
DEFINE_TEST_FP_FP16(frintp, 1Op, Conversions)
DEFINE_TEST_FP_FP16(frintx, 1Op, Conversions)
DEFINE_TEST_FP_FP16(frintz, 1Op, Conversions)

TEST(fcmp_d) { CALL_TEST_FP_HELPER(fcmp, d, Cmp, kInputDoubleBasic); }
TEST(fcmp_s) { CALL_TEST_FP_HELPER(fcmp, s, Cmp, kInputFloatBasic); }
TEST(fcmp_dz) { CALL_TEST_FP_HELPER(fcmp, dz, CmpZero, kInputDoubleBasic); }
TEST(fcmp_sz) { CALL_TEST_FP_HELPER(fcmp, sz, CmpZero, kInputFloatBasic); }

TEST(fcvt_sd) { CALL_TEST_FP_HELPER(fcvt, sd, 1Op, kInputDoubleConversions); }
TEST(fcvt_ds) { CALL_TEST_FP_HELPER(fcvt, ds, 1Op, kInputFloatConversions); }

#define DEFINE_TEST_FP_TO_INT(mnemonic, type, input)               \
  TEST(mnemonic##_xd) {                                            \
    CALL_TEST_FP_HELPER(mnemonic, xd, type, kInputDouble##input);  \
  }                                                                \
  TEST(mnemonic##_xs) {                                            \
    CALL_TEST_FP_HELPER(mnemonic, xs, type, kInputFloat##input);   \
  }                                                                \
  TEST(mnemonic##_xh) {                                            \
    CALL_TEST_FP_HELPER(mnemonic, xh, type, kInputFloat16##input); \
  }                                                                \
  TEST(mnemonic##_wd) {                                            \
    CALL_TEST_FP_HELPER(mnemonic, wd, type, kInputDouble##input);  \
  }                                                                \
  TEST(mnemonic##_ws) {                                            \
    CALL_TEST_FP_HELPER(mnemonic, ws, type, kInputFloat##input);   \
  }                                                                \
  TEST(mnemonic##_wh) {                                            \
    CALL_TEST_FP_HELPER(mnemonic, wh, type, kInputFloat16##input); \
  }

DEFINE_TEST_FP_TO_INT(fcvtas, FPToS, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtau, FPToU, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtms, FPToS, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtmu, FPToU, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtns, FPToS, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtnu, FPToU, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtzs, FPToFixedS, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtzu, FPToFixedU, Conversions)

#define DEFINE_TEST_FP_TO_JS_INT(mnemonic, type, input)           \
  TEST(mnemonic##_wd) {                                           \
    CALL_TEST_FP_HELPER(mnemonic, wd, type, kInputDouble##input); \
  }

DEFINE_TEST_FP_TO_JS_INT(fjcvtzs, FPToS, Conversions)

// TODO: Scvtf-fixed-point
// TODO: Scvtf-integer
// TODO: Ucvtf-fixed-point
// TODO: Ucvtf-integer

// TODO: Fccmp
// TODO: Fcsel


// ==== NEON Tests. ====

#define CALL_TEST_NEON_HELPER_1Op(mnemonic, vdform, vnform, input_n) \
  Test1OpNEON(STRINGIFY(mnemonic) "_" STRINGIFY(vdform),             \
              &MacroAssembler::mnemonic,                             \
              input_n,                                               \
              (sizeof(input_n) / sizeof(input_n[0])),                \
              kExpected_NEON_##mnemonic##_##vdform,                  \
              kExpectedCount_NEON_##mnemonic##_##vdform,             \
              kFormat##vdform,                                       \
              kFormat##vnform)

#define CALL_TEST_NEON_HELPER_1OpAcross(mnemonic, vdform, vnform, input_n)   \
  Test1OpAcrossNEON(STRINGIFY(mnemonic) "_" STRINGIFY(vdform) "_" STRINGIFY( \
                        vnform),                                             \
                    &MacroAssembler::mnemonic,                               \
                    input_n,                                                 \
                    (sizeof(input_n) / sizeof(input_n[0])),                  \
                    kExpected_NEON_##mnemonic##_##vdform##_##vnform,         \
                    kExpectedCount_NEON_##mnemonic##_##vdform##_##vnform,    \
                    kFormat##vdform,                                         \
                    kFormat##vnform)

#define CALL_TEST_NEON_HELPER_2Op(mnemonic,              \
                                  vdform,                \
                                  vnform,                \
                                  vmform,                \
                                  input_d,               \
                                  input_n,               \
                                  input_m)               \
  Test2OpNEON(STRINGIFY(mnemonic) "_" STRINGIFY(vdform), \
              &MacroAssembler::mnemonic,                 \
              input_d,                                   \
              input_n,                                   \
              (sizeof(input_n) / sizeof(input_n[0])),    \
              input_m,                                   \
              (sizeof(input_m) / sizeof(input_m[0])),    \
              kExpected_NEON_##mnemonic##_##vdform,      \
              kExpectedCount_NEON_##mnemonic##_##vdform, \
              kFormat##vdform,                           \
              kFormat##vnform,                           \
              kFormat##vmform)

#define CALL_TEST_NEON_HELPER_2OpImm(mnemonic,                        \
                                     vdform,                          \
                                     vnform,                          \
                                     input_n,                         \
                                     input_m)                         \
  Test2OpImmNEON(STRINGIFY(mnemonic) "_" STRINGIFY(vdform) "_2OPIMM", \
                 &MacroAssembler::mnemonic,                           \
                 input_n,                                             \
                 (sizeof(input_n) / sizeof(input_n[0])),              \
                 input_m,                                             \
                 (sizeof(input_m) / sizeof(input_m[0])),              \
                 kExpected_NEON_##mnemonic##_##vdform##_2OPIMM,       \
                 kExpectedCount_NEON_##mnemonic##_##vdform##_2OPIMM,  \
                 kFormat##vdform,                                     \
                 kFormat##vnform)

#define CALL_TEST_NEON_HELPER_ByElement(mnemonic,                      \
                                        vdform,                        \
                                        vnform,                        \
                                        vmform,                        \
                                        input_d,                       \
                                        input_n,                       \
                                        input_m,                       \
                                        indices)                       \
  TestByElementNEON(                                                   \
      STRINGIFY(mnemonic) "_" STRINGIFY(vdform) "_" STRINGIFY(         \
          vnform) "_" STRINGIFY(vmform),                               \
      &MacroAssembler::mnemonic,                                       \
      input_d,                                                         \
      input_n,                                                         \
      (sizeof(input_n) / sizeof(input_n[0])),                          \
      input_m,                                                         \
      (sizeof(input_m) / sizeof(input_m[0])),                          \
      indices,                                                         \
      (sizeof(indices) / sizeof(indices[0])),                          \
      kExpected_NEON_##mnemonic##_##vdform##_##vnform##_##vmform,      \
      kExpectedCount_NEON_##mnemonic##_##vdform##_##vnform##_##vmform, \
      kFormat##vdform,                                                 \
      kFormat##vnform,                                                 \
      kFormat##vmform)

#define CALL_TEST_NEON_HELPER_ByElement_Dot_Product(mnemonic,           \
                                                    vdform,             \
                                                    vnform,             \
                                                    vmform,             \
                                                    input_d,            \
                                                    input_n,            \
                                                    input_m,            \
                                                    indices,            \
                                                    vm_subvector_count) \
  TestByElementNEON(                                                    \
      STRINGIFY(mnemonic) "_" STRINGIFY(vdform) "_" STRINGIFY(          \
          vnform) "_" STRINGIFY(vmform),                                \
      &MacroAssembler::mnemonic,                                        \
      input_d,                                                          \
      input_n,                                                          \
      (sizeof(input_n) / sizeof(input_n[0])),                           \
      input_m,                                                          \
      (sizeof(input_m) / sizeof(input_m[0])),                           \
      indices,                                                          \
      (sizeof(indices) / sizeof(indices[0])),                           \
      kExpected_NEON_##mnemonic##_##vdform##_##vnform##_##vmform,       \
      kExpectedCount_NEON_##mnemonic##_##vdform##_##vnform##_##vmform,  \
      kFormat##vdform,                                                  \
      kFormat##vnform,                                                  \
      kFormat##vmform,                                                  \
      vm_subvector_count)

#define CALL_TEST_NEON_HELPER_OpImmOpImm(helper,                   \
                                         mnemonic,                 \
                                         vdform,                   \
                                         vnform,                   \
                                         input_d,                  \
                                         input_imm1,               \
                                         input_n,                  \
                                         input_imm2)               \
  TestOpImmOpImmNEON(STRINGIFY(mnemonic) "_" STRINGIFY(vdform),    \
                     helper,                                       \
                     input_d,                                      \
                     input_imm1,                                   \
                     (sizeof(input_imm1) / sizeof(input_imm1[0])), \
                     input_n,                                      \
                     (sizeof(input_n) / sizeof(input_n[0])),       \
                     input_imm2,                                   \
                     (sizeof(input_imm2) / sizeof(input_imm2[0])), \
                     kExpected_NEON_##mnemonic##_##vdform,         \
                     kExpectedCount_NEON_##mnemonic##_##vdform,    \
                     kFormat##vdform,                              \
                     kFormat##vnform)

#define CALL_TEST_NEON_HELPER_2SAME(mnemonic, variant, input) \
  CALL_TEST_NEON_HELPER_1Op(mnemonic, variant, variant, input)

#define DEFINE_TEST_NEON_2SAME_8B_16B(mnemonic, input)              \
  TEST(mnemonic##_8B) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 8B, kInput8bits##input);  \
  }                                                                 \
  TEST(mnemonic##_16B) {                                            \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 16B, kInput8bits##input); \
  }

#define DEFINE_TEST_NEON_2SAME_4H_8H(mnemonic, input)               \
  TEST(mnemonic##_4H) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 4H, kInput16bits##input); \
  }                                                                 \
  TEST(mnemonic##_8H) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 8H, kInput16bits##input); \
  }

#define DEFINE_TEST_NEON_2SAME_2S_4S(mnemonic, input)               \
  TEST(mnemonic##_2S) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 2S, kInput32bits##input); \
  }                                                                 \
  TEST(mnemonic##_4S) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 4S, kInput32bits##input); \
  }

#define DEFINE_TEST_NEON_2SAME_BH(mnemonic, input) \
  DEFINE_TEST_NEON_2SAME_8B_16B(mnemonic, input)   \
  DEFINE_TEST_NEON_2SAME_4H_8H(mnemonic, input)

#define DEFINE_TEST_NEON_2SAME_NO2D(mnemonic, input) \
  DEFINE_TEST_NEON_2SAME_BH(mnemonic, input)         \
  DEFINE_TEST_NEON_2SAME_2S_4S(mnemonic, input)

#define DEFINE_TEST_NEON_2SAME(mnemonic, input)                     \
  DEFINE_TEST_NEON_2SAME_NO2D(mnemonic, input)                      \
  TEST(mnemonic##_2D) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 2D, kInput64bits##input); \
  }
#define DEFINE_TEST_NEON_2SAME_SD(mnemonic, input)                  \
  DEFINE_TEST_NEON_2SAME_2S_4S(mnemonic, input)                     \
  TEST(mnemonic##_2D) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 2D, kInput64bits##input); \
  }

#define DEFINE_TEST_NEON_2SAME_FP(mnemonic, input)                  \
  TEST(mnemonic##_2S) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 2S, kInputFloat##input);  \
  }                                                                 \
  TEST(mnemonic##_4S) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 4S, kInputFloat##input);  \
  }                                                                 \
  TEST(mnemonic##_2D) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 2D, kInputDouble##input); \
  }

#define DEFINE_TEST_NEON_2SAME_FP_FP16(mnemonic, input)              \
  DEFINE_TEST_NEON_2SAME_FP(mnemonic, input)                         \
  TEST(mnemonic##_4H) {                                              \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 4H, kInputFloat16##input); \
  }                                                                  \
  TEST(mnemonic##_8H) {                                              \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, 8H, kInputFloat16##input); \
  }

#define DEFINE_TEST_NEON_2SAME_FP_FP16_SCALAR(mnemonic, input)      \
  TEST(mnemonic##_H) {                                              \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, H, kInputFloat16##input); \
  }                                                                 \
  TEST(mnemonic##_S) {                                              \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, S, kInputFloat##input);   \
  }                                                                 \
  TEST(mnemonic##_D) {                                              \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, D, kInputDouble##input);  \
  }

#define DEFINE_TEST_NEON_2SAME_SCALAR_B(mnemonic, input)          \
  TEST(mnemonic##_B) {                                            \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, B, kInput8bits##input); \
  }
#define DEFINE_TEST_NEON_2SAME_SCALAR_H(mnemonic, input)           \
  TEST(mnemonic##_H) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, H, kInput16bits##input); \
  }
#define DEFINE_TEST_NEON_2SAME_SCALAR_S(mnemonic, input)           \
  TEST(mnemonic##_S) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, S, kInput32bits##input); \
  }
#define DEFINE_TEST_NEON_2SAME_SCALAR_D(mnemonic, input)           \
  TEST(mnemonic##_D) {                                             \
    CALL_TEST_NEON_HELPER_2SAME(mnemonic, D, kInput64bits##input); \
  }

#define DEFINE_TEST_NEON_2SAME_SCALAR(mnemonic, input) \
  DEFINE_TEST_NEON_2SAME_SCALAR_B(mnemonic, input)     \
  DEFINE_TEST_NEON_2SAME_SCALAR_H(mnemonic, input)     \
  DEFINE_TEST_NEON_2SAME_SCALAR_S(mnemonic, input)     \
  DEFINE_TEST_NEON_2SAME_SCALAR_D(mnemonic, input)

#define DEFINE_TEST_NEON_2SAME_SCALAR_SD(mnemonic, input) \
  DEFINE_TEST_NEON_2SAME_SCALAR_S(mnemonic, input)        \
  DEFINE_TEST_NEON_2SAME_SCALAR_D(mnemonic, input)


#define CALL_TEST_NEON_HELPER_ACROSS(mnemonic, vd_form, vn_form, input_n) \
  CALL_TEST_NEON_HELPER_1OpAcross(mnemonic, vd_form, vn_form, input_n)

#define DEFINE_TEST_NEON_ACROSS(mnemonic, input)                        \
  TEST(mnemonic##_B_8B) {                                               \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, B, 8B, kInput8bits##input);  \
  }                                                                     \
  TEST(mnemonic##_B_16B) {                                              \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, B, 16B, kInput8bits##input); \
  }                                                                     \
  TEST(mnemonic##_H_4H) {                                               \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, H, 4H, kInput16bits##input); \
  }                                                                     \
  TEST(mnemonic##_H_8H) {                                               \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, H, 8H, kInput16bits##input); \
  }                                                                     \
  TEST(mnemonic##_S_4S) {                                               \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, S, 4S, kInput32bits##input); \
  }

#define DEFINE_TEST_NEON_ACROSS_LONG(mnemonic, input)                   \
  TEST(mnemonic##_H_8B) {                                               \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, H, 8B, kInput8bits##input);  \
  }                                                                     \
  TEST(mnemonic##_H_16B) {                                              \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, H, 16B, kInput8bits##input); \
  }                                                                     \
  TEST(mnemonic##_S_4H) {                                               \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, S, 4H, kInput16bits##input); \
  }                                                                     \
  TEST(mnemonic##_S_8H) {                                               \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, S, 8H, kInput16bits##input); \
  }                                                                     \
  TEST(mnemonic##_D_4S) {                                               \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, D, 4S, kInput32bits##input); \
  }

#define DEFINE_TEST_NEON_ACROSS_FP(mnemonic, input)                      \
  TEST(mnemonic##_H_4H) {                                                \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, H, 4H, kInputFloat16##input); \
  }                                                                      \
  TEST(mnemonic##_H_8H) {                                                \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, H, 8H, kInputFloat16##input); \
  }                                                                      \
  TEST(mnemonic##_S_4S) {                                                \
    CALL_TEST_NEON_HELPER_ACROSS(mnemonic, S, 4S, kInputFloat##input);   \
  }

#define CALL_TEST_NEON_HELPER_2DIFF(mnemonic, vdform, vnform, input_n) \
  CALL_TEST_NEON_HELPER_1Op(mnemonic, vdform, vnform, input_n)

#define DEFINE_TEST_NEON_2DIFF_LONG(mnemonic, input)                    \
  TEST(mnemonic##_4H) {                                                 \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 4H, 8B, kInput8bits##input);  \
  }                                                                     \
  TEST(mnemonic##_8H) {                                                 \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 8H, 16B, kInput8bits##input); \
  }                                                                     \
  TEST(mnemonic##_2S) {                                                 \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 2S, 4H, kInput16bits##input); \
  }                                                                     \
  TEST(mnemonic##_4S) {                                                 \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 4S, 8H, kInput16bits##input); \
  }                                                                     \
  TEST(mnemonic##_1D) {                                                 \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 1D, 2S, kInput32bits##input); \
  }                                                                     \
  TEST(mnemonic##_2D) {                                                 \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 2D, 4S, kInput32bits##input); \
  }

#define DEFINE_TEST_NEON_2DIFF_NARROW(mnemonic, input)                      \
  TEST(mnemonic##_8B) {                                                     \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 8B, 8H, kInput16bits##input);     \
  }                                                                         \
  TEST(mnemonic##_4H) {                                                     \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 4H, 4S, kInput32bits##input);     \
  }                                                                         \
  TEST(mnemonic##_2S) {                                                     \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 2S, 2D, kInput64bits##input);     \
  }                                                                         \
  TEST(mnemonic##2_16B) {                                                   \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 16B, 8H, kInput16bits##input); \
  }                                                                         \
  TEST(mnemonic##2_8H) {                                                    \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 8H, 4S, kInput32bits##input);  \
  }                                                                         \
  TEST(mnemonic##2_4S) {                                                    \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 4S, 2D, kInput64bits##input);  \
  }

#define DEFINE_TEST_NEON_2DIFF_FP_LONG(mnemonic, input)                     \
  TEST(mnemonic##_4S) {                                                     \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 4S, 4H, kInputFloat16##input);    \
  }                                                                         \
  TEST(mnemonic##_2D) {                                                     \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 2D, 2S, kInputFloat##input);      \
  }                                                                         \
  TEST(mnemonic##2_4S) {                                                    \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 4S, 8H, kInputFloat16##input); \
  }                                                                         \
  TEST(mnemonic##2_2D) {                                                    \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 2D, 4S, kInputFloat##input);   \
  }

#define DEFINE_TEST_NEON_2DIFF_FP_NARROW(mnemonic, input)                  \
  TEST(mnemonic##_4H) {                                                    \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 4H, 4S, kInputFloat##input);     \
  }                                                                        \
  TEST(mnemonic##_2S) {                                                    \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 2S, 2D, kInputDouble##input);    \
  }                                                                        \
  TEST(mnemonic##2_8H) {                                                   \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 8H, 4S, kInputFloat##input);  \
  }                                                                        \
  TEST(mnemonic##2_4S) {                                                   \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 4S, 2D, kInputDouble##input); \
  }

#define DEFINE_TEST_NEON_2DIFF_FP_NARROW_2S(mnemonic, input)               \
  TEST(mnemonic##_2S) {                                                    \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 2S, 2D, kInputDouble##input);    \
  }                                                                        \
  TEST(mnemonic##2_4S) {                                                   \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 4S, 2D, kInputDouble##input); \
  }

#define DEFINE_TEST_NEON_2DIFF_SCALAR_NARROW(mnemonic, input)         \
  TEST(mnemonic##_B) {                                                \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, B, H, kInput16bits##input); \
  }                                                                   \
  TEST(mnemonic##_H) {                                                \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, H, S, kInput32bits##input); \
  }                                                                   \
  TEST(mnemonic##_S) {                                                \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, S, D, kInput64bits##input); \
  }

#define DEFINE_TEST_NEON_2DIFF_FP_SCALAR_SD(mnemonic, input)            \
  TEST(mnemonic##_S) {                                                  \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, S, 2S, kInputFloat##input);   \
  }                                                                     \
  TEST(mnemonic##_D) {                                                  \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, D, 2D, kInputDouble##input);  \
  }                                                                     \
  TEST(mnemonic##_H) {                                                  \
    CALL_TEST_NEON_HELPER_2DIFF(mnemonic, H, 2H, kInputFloat16##input); \
  }

#define CALL_TEST_NEON_HELPER_3SAME(mnemonic, variant, input_d, input_nm) \
  {                                                                       \
    CALL_TEST_NEON_HELPER_2Op(mnemonic,                                   \
                              variant,                                    \
                              variant,                                    \
                              variant,                                    \
                              input_d,                                    \
                              input_nm,                                   \
                              input_nm);                                  \
  }

#define DEFINE_TEST_NEON_3SAME_8B_16B(mnemonic, input)     \
  TEST(mnemonic##_8B) {                                    \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                  \
                                8B,                        \
                                kInput8bitsAccDestination, \
                                kInput8bits##input);       \
  }                                                        \
  TEST(mnemonic##_16B) {                                   \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                  \
                                16B,                       \
                                kInput8bitsAccDestination, \
                                kInput8bits##input);       \
  }

#define DEFINE_TEST_NEON_3SAME_HS(mnemonic, input)          \
  TEST(mnemonic##_4H) {                                     \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                   \
                                4H,                         \
                                kInput16bitsAccDestination, \
                                kInput16bits##input);       \
  }                                                         \
  TEST(mnemonic##_8H) {                                     \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                   \
                                8H,                         \
                                kInput16bitsAccDestination, \
                                kInput16bits##input);       \
  }                                                         \
  TEST(mnemonic##_2S) {                                     \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                   \
                                2S,                         \
                                kInput32bitsAccDestination, \
                                kInput32bits##input);       \
  }                                                         \
  TEST(mnemonic##_4S) {                                     \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                   \
                                4S,                         \
                                kInput32bitsAccDestination, \
                                kInput32bits##input);       \
  }

#define DEFINE_TEST_NEON_3SAME_NO2D(mnemonic, input) \
  DEFINE_TEST_NEON_3SAME_8B_16B(mnemonic, input)     \
  DEFINE_TEST_NEON_3SAME_HS(mnemonic, input)

#define DEFINE_TEST_NEON_3SAME(mnemonic, input)             \
  DEFINE_TEST_NEON_3SAME_NO2D(mnemonic, input)              \
  TEST(mnemonic##_2D) {                                     \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                   \
                                2D,                         \
                                kInput64bitsAccDestination, \
                                kInput64bits##input);       \
  }

#define DEFINE_TEST_NEON_3SAME_FP(mnemonic, input)           \
  TEST(mnemonic##_4H) {                                      \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                    \
                                4H,                          \
                                kInputFloat16AccDestination, \
                                kInputFloat16##input);       \
  }                                                          \
  TEST(mnemonic##_8H) {                                      \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                    \
                                8H,                          \
                                kInputFloat16AccDestination, \
                                kInputFloat16##input);       \
  }                                                          \
  TEST(mnemonic##_2S) {                                      \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                    \
                                2S,                          \
                                kInputFloatAccDestination,   \
                                kInputFloat##input);         \
  }                                                          \
  TEST(mnemonic##_4S) {                                      \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                    \
                                4S,                          \
                                kInputFloatAccDestination,   \
                                kInputFloat##input);         \
  }                                                          \
  TEST(mnemonic##_2D) {                                      \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                    \
                                2D,                          \
                                kInputDoubleAccDestination,  \
                                kInputDouble##input);        \
  }

#define DEFINE_TEST_NEON_3SAME_SCALAR_D(mnemonic, input)    \
  TEST(mnemonic##_D) {                                      \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                   \
                                D,                          \
                                kInput64bitsAccDestination, \
                                kInput64bits##input);       \
  }

#define DEFINE_TEST_NEON_3SAME_SCALAR_HS(mnemonic, input)   \
  TEST(mnemonic##_H) {                                      \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                   \
                                H,                          \
                                kInput16bitsAccDestination, \
                                kInput16bits##input);       \
  }                                                         \
  TEST(mnemonic##_S) {                                      \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                   \
                                S,                          \
                                kInput32bitsAccDestination, \
                                kInput32bits##input);       \
  }

#define DEFINE_TEST_NEON_3SAME_SCALAR(mnemonic, input)      \
  TEST(mnemonic##_B) {                                      \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                   \
                                B,                          \
                                kInput8bitsAccDestination,  \
                                kInput8bits##input);        \
  }                                                         \
  TEST(mnemonic##_H) {                                      \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                   \
                                H,                          \
                                kInput16bitsAccDestination, \
                                kInput16bits##input);       \
  }                                                         \
  TEST(mnemonic##_S) {                                      \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                   \
                                S,                          \
                                kInput32bitsAccDestination, \
                                kInput32bits##input);       \
  }                                                         \
  TEST(mnemonic##_D) {                                      \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                   \
                                D,                          \
                                kInput64bitsAccDestination, \
                                kInput64bits##input);       \
  }

#define DEFINE_TEST_NEON_3SAME_FP_SCALAR(mnemonic, input)    \
  TEST(mnemonic##_H) {                                       \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                    \
                                H,                           \
                                kInputFloat16AccDestination, \
                                kInputFloat16##input);       \
  }                                                          \
  TEST(mnemonic##_S) {                                       \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                    \
                                S,                           \
                                kInputFloatAccDestination,   \
                                kInputFloat##input);         \
  }                                                          \
  TEST(mnemonic##_D) {                                       \
    CALL_TEST_NEON_HELPER_3SAME(mnemonic,                    \
                                D,                           \
                                kInputDoubleAccDestination,  \
                                kInputDouble##input);        \
  }

#define DEFINE_TEST_NEON_FHM(mnemonic, input_d, input_n, input_m) \
  TEST(mnemonic##_2S) {                                           \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                         \
                                2S,                               \
                                2H,                               \
                                2H,                               \
                                kInputFloatAccDestination,        \
                                kInputFloat16##input_n,           \
                                kInputFloat16##input_m);          \
  }                                                               \
  TEST(mnemonic##_4S) {                                           \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                         \
                                4S,                               \
                                4H,                               \
                                4H,                               \
                                kInputFloatAccDestination,        \
                                kInputFloat16##input_n,           \
                                kInputFloat16##input_m);          \
  }

#define CALL_TEST_NEON_HELPER_3DIFF(mnemonic, \
                                    vdform,   \
                                    vnform,   \
                                    vmform,   \
                                    input_d,  \
                                    input_n,  \
                                    input_m)  \
  {                                           \
    CALL_TEST_NEON_HELPER_2Op(mnemonic,       \
                              vdform,         \
                              vnform,         \
                              vmform,         \
                              input_d,        \
                              input_n,        \
                              input_m);       \
  }

#define DEFINE_TEST_NEON_3DIFF_LONG_8H(mnemonic, input)     \
  TEST(mnemonic##_8H) {                                     \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                   \
                                8H,                         \
                                8B,                         \
                                8B,                         \
                                kInput16bitsAccDestination, \
                                kInput8bits##input,         \
                                kInput8bits##input);        \
  }                                                         \
  TEST(mnemonic##2_8H) {                                    \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2,                \
                                8H,                         \
                                16B,                        \
                                16B,                        \
                                kInput16bitsAccDestination, \
                                kInput8bits##input,         \
                                kInput8bits##input);        \
  }

#define DEFINE_TEST_NEON_3DIFF_LONG_4S(mnemonic, input)     \
  TEST(mnemonic##_4S) {                                     \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                   \
                                4S,                         \
                                4H,                         \
                                4H,                         \
                                kInput32bitsAccDestination, \
                                kInput16bits##input,        \
                                kInput16bits##input);       \
  }                                                         \
  TEST(mnemonic##2_4S) {                                    \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2,                \
                                4S,                         \
                                8H,                         \
                                8H,                         \
                                kInput32bitsAccDestination, \
                                kInput16bits##input,        \
                                kInput16bits##input);       \
  }

#define DEFINE_TEST_NEON_3DIFF_LONG_2D(mnemonic, input)     \
  TEST(mnemonic##_2D) {                                     \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                   \
                                2D,                         \
                                2S,                         \
                                2S,                         \
                                kInput64bitsAccDestination, \
                                kInput32bits##input,        \
                                kInput32bits##input);       \
  }                                                         \
  TEST(mnemonic##2_2D) {                                    \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2,                \
                                2D,                         \
                                4S,                         \
                                4S,                         \
                                kInput64bitsAccDestination, \
                                kInput32bits##input,        \
                                kInput32bits##input);       \
  }

#define DEFINE_TEST_NEON_3DIFF_LONG_SD(mnemonic, input) \
  DEFINE_TEST_NEON_3DIFF_LONG_4S(mnemonic, input)       \
  DEFINE_TEST_NEON_3DIFF_LONG_2D(mnemonic, input)

#define DEFINE_TEST_NEON_3DIFF_LONG(mnemonic, input) \
  DEFINE_TEST_NEON_3DIFF_LONG_8H(mnemonic, input)    \
  DEFINE_TEST_NEON_3DIFF_LONG_4S(mnemonic, input)    \
  DEFINE_TEST_NEON_3DIFF_LONG_2D(mnemonic, input)

#define DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_S(mnemonic, input) \
  TEST(mnemonic##_S) {                                        \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                     \
                                S,                            \
                                H,                            \
                                H,                            \
                                kInput32bitsAccDestination,   \
                                kInput16bits##input,          \
                                kInput16bits##input);         \
  }

#define DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_D(mnemonic, input) \
  TEST(mnemonic##_D) {                                        \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                     \
                                D,                            \
                                S,                            \
                                S,                            \
                                kInput64bitsAccDestination,   \
                                kInput32bits##input,          \
                                kInput32bits##input);         \
  }

#define DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_SD(mnemonic, input) \
  DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_S(mnemonic, input)        \
  DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_D(mnemonic, input)

#define DEFINE_TEST_NEON_3DIFF_WIDE(mnemonic, input)        \
  TEST(mnemonic##_8H) {                                     \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                   \
                                8H,                         \
                                8H,                         \
                                8B,                         \
                                kInput16bitsAccDestination, \
                                kInput16bits##input,        \
                                kInput8bits##input);        \
  }                                                         \
  TEST(mnemonic##_4S) {                                     \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                   \
                                4S,                         \
                                4S,                         \
                                4H,                         \
                                kInput32bitsAccDestination, \
                                kInput32bits##input,        \
                                kInput16bits##input);       \
  }                                                         \
  TEST(mnemonic##_2D) {                                     \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                   \
                                2D,                         \
                                2D,                         \
                                2S,                         \
                                kInput64bitsAccDestination, \
                                kInput64bits##input,        \
                                kInput32bits##input);       \
  }                                                         \
  TEST(mnemonic##2_8H) {                                    \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2,                \
                                8H,                         \
                                8H,                         \
                                16B,                        \
                                kInput16bitsAccDestination, \
                                kInput16bits##input,        \
                                kInput8bits##input);        \
  }                                                         \
  TEST(mnemonic##2_4S) {                                    \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2,                \
                                4S,                         \
                                4S,                         \
                                8H,                         \
                                kInput32bitsAccDestination, \
                                kInput32bits##input,        \
                                kInput16bits##input);       \
  }                                                         \
  TEST(mnemonic##2_2D) {                                    \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2,                \
                                2D,                         \
                                2D,                         \
                                4S,                         \
                                kInput64bitsAccDestination, \
                                kInput64bits##input,        \
                                kInput32bits##input);       \
  }

#define DEFINE_TEST_NEON_3DIFF_NARROW(mnemonic, input)      \
  TEST(mnemonic##_8B) {                                     \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                   \
                                8B,                         \
                                8H,                         \
                                8H,                         \
                                kInput8bitsAccDestination,  \
                                kInput16bits##input,        \
                                kInput16bits##input);       \
  }                                                         \
  TEST(mnemonic##_4H) {                                     \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                   \
                                4H,                         \
                                4S,                         \
                                4S,                         \
                                kInput16bitsAccDestination, \
                                kInput32bits##input,        \
                                kInput32bits##input);       \
  }                                                         \
  TEST(mnemonic##_2S) {                                     \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                   \
                                2S,                         \
                                2D,                         \
                                2D,                         \
                                kInput32bitsAccDestination, \
                                kInput64bits##input,        \
                                kInput64bits##input);       \
  }                                                         \
  TEST(mnemonic##2_16B) {                                   \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2,                \
                                16B,                        \
                                8H,                         \
                                8H,                         \
                                kInput8bitsAccDestination,  \
                                kInput16bits##input,        \
                                kInput16bits##input);       \
  }                                                         \
  TEST(mnemonic##2_8H) {                                    \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2,                \
                                8H,                         \
                                4S,                         \
                                4S,                         \
                                kInput16bitsAccDestination, \
                                kInput32bits##input,        \
                                kInput32bits##input);       \
  }                                                         \
  TEST(mnemonic##2_4S) {                                    \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2,                \
                                4S,                         \
                                2D,                         \
                                2D,                         \
                                kInput32bitsAccDestination, \
                                kInput64bits##input,        \
                                kInput64bits##input);       \
  }

#define DEFINE_TEST_NEON_3DIFF_DOUBLE_WIDE(mnemonic, input) \
  TEST(mnemonic##_2S) {                                     \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                   \
                                2S,                         \
                                8B,                         \
                                8B,                         \
                                kInput32bitsAccDestination, \
                                kInput8bits##input,         \
                                kInput8bits##input);        \
  }                                                         \
  TEST(mnemonic##_4S) {                                     \
    CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                   \
                                4S,                         \
                                16B,                        \
                                16B,                        \
                                kInput32bitsAccDestination, \
                                kInput8bits##input,         \
                                kInput8bits##input);        \
  }


#define CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,  \
                                     vdform,    \
                                     vnform,    \
                                     input_n,   \
                                     input_imm) \
  {                                             \
    CALL_TEST_NEON_HELPER_2OpImm(mnemonic,      \
                                 vdform,        \
                                 vnform,        \
                                 input_n,       \
                                 input_imm);    \
  }

#define DEFINE_TEST_NEON_2OPIMM(mnemonic, input, input_imm)   \
  TEST(mnemonic##_8B_2OPIMM) {                                \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                    \
                                 8B,                          \
                                 8B,                          \
                                 kInput8bits##input,          \
                                 kInput8bitsImm##input_imm);  \
  }                                                           \
  TEST(mnemonic##_16B_2OPIMM) {                               \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                    \
                                 16B,                         \
                                 16B,                         \
                                 kInput8bits##input,          \
                                 kInput8bitsImm##input_imm);  \
  }                                                           \
  TEST(mnemonic##_4H_2OPIMM) {                                \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                    \
                                 4H,                          \
                                 4H,                          \
                                 kInput16bits##input,         \
                                 kInput16bitsImm##input_imm); \
  }                                                           \
  TEST(mnemonic##_8H_2OPIMM) {                                \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                    \
                                 8H,                          \
                                 8H,                          \
                                 kInput16bits##input,         \
                                 kInput16bitsImm##input_imm); \
  }                                                           \
  TEST(mnemonic##_2S_2OPIMM) {                                \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                    \
                                 2S,                          \
                                 2S,                          \
                                 kInput32bits##input,         \
                                 kInput32bitsImm##input_imm); \
  }                                                           \
  TEST(mnemonic##_4S_2OPIMM) {                                \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                    \
                                 4S,                          \
                                 4S,                          \
                                 kInput32bits##input,         \
                                 kInput32bitsImm##input_imm); \
  }                                                           \
  TEST(mnemonic##_2D_2OPIMM) {                                \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                    \
                                 2D,                          \
                                 2D,                          \
                                 kInput64bits##input,         \
                                 kInput64bitsImm##input_imm); \
  }

#define DEFINE_TEST_NEON_2OPIMM_COPY(mnemonic, input, input_imm) \
  TEST(mnemonic##_8B_2OPIMM) {                                   \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                       \
                                 8B,                             \
                                 B,                              \
                                 kInput8bits##input,             \
                                 kInput8bitsImm##input_imm);     \
  }                                                              \
  TEST(mnemonic##_16B_2OPIMM) {                                  \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                       \
                                 16B,                            \
                                 B,                              \
                                 kInput8bits##input,             \
                                 kInput8bitsImm##input_imm);     \
  }                                                              \
  TEST(mnemonic##_4H_2OPIMM) {                                   \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                       \
                                 4H,                             \
                                 H,                              \
                                 kInput16bits##input,            \
                                 kInput16bitsImm##input_imm);    \
  }                                                              \
  TEST(mnemonic##_8H_2OPIMM) {                                   \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                       \
                                 8H,                             \
                                 H,                              \
                                 kInput16bits##input,            \
                                 kInput16bitsImm##input_imm);    \
  }                                                              \
  TEST(mnemonic##_2S_2OPIMM) {                                   \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                       \
                                 2S,                             \
                                 S,                              \
                                 kInput32bits##input,            \
                                 kInput32bitsImm##input_imm);    \
  }                                                              \
  TEST(mnemonic##_4S_2OPIMM) {                                   \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                       \
                                 4S,                             \
                                 S,                              \
                                 kInput32bits##input,            \
                                 kInput32bitsImm##input_imm);    \
  }                                                              \
  TEST(mnemonic##_2D_2OPIMM) {                                   \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                       \
                                 2D,                             \
                                 D,                              \
                                 kInput64bits##input,            \
                                 kInput64bitsImm##input_imm);    \
  }

#define DEFINE_TEST_NEON_2OPIMM_NARROW(mnemonic, input, input_imm) \
  TEST(mnemonic##_8B_2OPIMM) {                                     \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                         \
                                 8B,                               \
                                 8H,                               \
                                 kInput16bits##input,              \
                                 kInput8bitsImm##input_imm);       \
  }                                                                \
  TEST(mnemonic##_4H_2OPIMM) {                                     \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                         \
                                 4H,                               \
                                 4S,                               \
                                 kInput32bits##input,              \
                                 kInput16bitsImm##input_imm);      \
  }                                                                \
  TEST(mnemonic##_2S_2OPIMM) {                                     \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                         \
                                 2S,                               \
                                 2D,                               \
                                 kInput64bits##input,              \
                                 kInput32bitsImm##input_imm);      \
  }                                                                \
  TEST(mnemonic##2_16B_2OPIMM) {                                   \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic##2,                      \
                                 16B,                              \
                                 8H,                               \
                                 kInput16bits##input,              \
                                 kInput8bitsImm##input_imm);       \
  }                                                                \
  TEST(mnemonic##2_8H_2OPIMM) {                                    \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic##2,                      \
                                 8H,                               \
                                 4S,                               \
                                 kInput32bits##input,              \
                                 kInput16bitsImm##input_imm);      \
  }                                                                \
  TEST(mnemonic##2_4S_2OPIMM) {                                    \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic##2,                      \
                                 4S,                               \
                                 2D,                               \
                                 kInput64bits##input,              \
                                 kInput32bitsImm##input_imm);      \
  }

#define DEFINE_TEST_NEON_2OPIMM_SCALAR_NARROW(mnemonic, input, input_imm) \
  TEST(mnemonic##_B_2OPIMM) {                                             \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                \
                                 B,                                       \
                                 H,                                       \
                                 kInput16bits##input,                     \
                                 kInput8bitsImm##input_imm);              \
  }                                                                       \
  TEST(mnemonic##_H_2OPIMM) {                                             \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                \
                                 H,                                       \
                                 S,                                       \
                                 kInput32bits##input,                     \
                                 kInput16bitsImm##input_imm);             \
  }                                                                       \
  TEST(mnemonic##_S_2OPIMM) {                                             \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                \
                                 S,                                       \
                                 D,                                       \
                                 kInput64bits##input,                     \
                                 kInput32bitsImm##input_imm);             \
  }

#define DEFINE_TEST_NEON_2OPIMM_FCMP_ZERO(mnemonic, input, input_imm) \
  TEST(mnemonic##_4H_2OPIMM) {                                        \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                            \
                                 4H,                                  \
                                 4H,                                  \
                                 kInputFloat16##input,                \
                                 kInputDoubleImm##input_imm);         \
  }                                                                   \
  TEST(mnemonic##_8H_2OPIMM) {                                        \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                            \
                                 8H,                                  \
                                 8H,                                  \
                                 kInputFloat16##input,                \
                                 kInputDoubleImm##input_imm);         \
  }                                                                   \
  TEST(mnemonic##_2S_2OPIMM) {                                        \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                            \
                                 2S,                                  \
                                 2S,                                  \
                                 kInputFloat##Basic,                  \
                                 kInputDoubleImm##input_imm);         \
  }                                                                   \
  TEST(mnemonic##_4S_2OPIMM) {                                        \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                            \
                                 4S,                                  \
                                 4S,                                  \
                                 kInputFloat##input,                  \
                                 kInputDoubleImm##input_imm);         \
  }                                                                   \
  TEST(mnemonic##_2D_2OPIMM) {                                        \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                            \
                                 2D,                                  \
                                 2D,                                  \
                                 kInputDouble##input,                 \
                                 kInputDoubleImm##input_imm);         \
  }

#define DEFINE_TEST_NEON_2OPIMM_FP(mnemonic, input, input_imm) \
  TEST(mnemonic##_4H_2OPIMM) {                                 \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                     \
                                 4H,                           \
                                 4H,                           \
                                 kInputFloat16##input,         \
                                 kInput16bitsImm##input_imm);  \
  }                                                            \
  TEST(mnemonic##_8H_2OPIMM) {                                 \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                     \
                                 8H,                           \
                                 8H,                           \
                                 kInputFloat16##input,         \
                                 kInput16bitsImm##input_imm);  \
  }                                                            \
  TEST(mnemonic##_2S_2OPIMM) {                                 \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                     \
                                 2S,                           \
                                 2S,                           \
                                 kInputFloat##Basic,           \
                                 kInput32bitsImm##input_imm);  \
  }                                                            \
  TEST(mnemonic##_4S_2OPIMM) {                                 \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                     \
                                 4S,                           \
                                 4S,                           \
                                 kInputFloat##input,           \
                                 kInput32bitsImm##input_imm);  \
  }                                                            \
  TEST(mnemonic##_2D_2OPIMM) {                                 \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                     \
                                 2D,                           \
                                 2D,                           \
                                 kInputDouble##input,          \
                                 kInput64bitsImm##input_imm);  \
  }

#define DEFINE_TEST_NEON_2OPIMM_FP_SCALAR(mnemonic, input, input_imm) \
  TEST(mnemonic##_H_2OPIMM) {                                         \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                            \
                                 H,                                   \
                                 H,                                   \
                                 kInputFloat16##Basic,                \
                                 kInput16bitsImm##input_imm);         \
  }                                                                   \
  TEST(mnemonic##_S_2OPIMM) {                                         \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                            \
                                 S,                                   \
                                 S,                                   \
                                 kInputFloat##Basic,                  \
                                 kInput32bitsImm##input_imm);         \
  }                                                                   \
  TEST(mnemonic##_D_2OPIMM) {                                         \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                            \
                                 D,                                   \
                                 D,                                   \
                                 kInputDouble##input,                 \
                                 kInput64bitsImm##input_imm);         \
  }

#define DEFINE_TEST_NEON_2OPIMM_HSD(mnemonic, input, input_imm) \
  TEST(mnemonic##_4H_2OPIMM) {                                  \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                      \
                                 4H,                            \
                                 4H,                            \
                                 kInput16bits##input,           \
                                 kInput16bitsImm##input_imm);   \
  }                                                             \
  TEST(mnemonic##_8H_2OPIMM) {                                  \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                      \
                                 8H,                            \
                                 8H,                            \
                                 kInput16bits##input,           \
                                 kInput16bitsImm##input_imm);   \
  }                                                             \
  TEST(mnemonic##_2S_2OPIMM) {                                  \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                      \
                                 2S,                            \
                                 2S,                            \
                                 kInput32bits##input,           \
                                 kInput32bitsImm##input_imm);   \
  }                                                             \
  TEST(mnemonic##_4S_2OPIMM) {                                  \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                      \
                                 4S,                            \
                                 4S,                            \
                                 kInput32bits##input,           \
                                 kInput32bitsImm##input_imm);   \
  }                                                             \
  TEST(mnemonic##_2D_2OPIMM) {                                  \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                      \
                                 2D,                            \
                                 2D,                            \
                                 kInput64bits##input,           \
                                 kInput64bitsImm##input_imm);   \
  }

#define DEFINE_TEST_NEON_2OPIMM_SCALAR_D(mnemonic, input, input_imm) \
  TEST(mnemonic##_D_2OPIMM) {                                        \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                           \
                                 D,                                  \
                                 D,                                  \
                                 kInput64bits##input,                \
                                 kInput64bitsImm##input_imm);        \
  }

#define DEFINE_TEST_NEON_2OPIMM_SCALAR_HSD(mnemonic, input, input_imm) \
  TEST(mnemonic##_H_2OPIMM) {                                          \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                             \
                                 H,                                    \
                                 H,                                    \
                                 kInput16bits##input,                  \
                                 kInput16bitsImm##input_imm);          \
  }                                                                    \
  TEST(mnemonic##_S_2OPIMM) {                                          \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                             \
                                 S,                                    \
                                 S,                                    \
                                 kInput32bits##input,                  \
                                 kInput32bitsImm##input_imm);          \
  }                                                                    \
  DEFINE_TEST_NEON_2OPIMM_SCALAR_D(mnemonic, input, input_imm)

#define DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_D(mnemonic, input, input_imm) \
  TEST(mnemonic##_D_2OPIMM) {                                           \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                              \
                                 D,                                     \
                                 D,                                     \
                                 kInputDouble##input,                   \
                                 kInputDoubleImm##input_imm);           \
  }

#define DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_HSD(mnemonic, input, input_imm) \
  TEST(mnemonic##_H_2OPIMM) {                                             \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                \
                                 H,                                       \
                                 H,                                       \
                                 kInputFloat16##input,                    \
                                 kInputDoubleImm##input_imm);             \
  }                                                                       \
  TEST(mnemonic##_S_2OPIMM) {                                             \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                \
                                 S,                                       \
                                 S,                                       \
                                 kInputFloat##input,                      \
                                 kInputDoubleImm##input_imm);             \
  }                                                                       \
  DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_D(mnemonic, input, input_imm)

#define DEFINE_TEST_NEON_2OPIMM_SCALAR(mnemonic, input, input_imm) \
  TEST(mnemonic##_B_2OPIMM) {                                      \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                         \
                                 B,                                \
                                 B,                                \
                                 kInput8bits##input,               \
                                 kInput8bitsImm##input_imm);       \
  }                                                                \
  DEFINE_TEST_NEON_2OPIMM_SCALAR_HSD(mnemonic, input, input_imm)

#define DEFINE_TEST_NEON_2OPIMM_LONG(mnemonic, input, input_imm) \
  TEST(mnemonic##_8H_2OPIMM) {                                   \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                       \
                                 8H,                             \
                                 8B,                             \
                                 kInput8bits##input,             \
                                 kInput8bitsImm##input_imm);     \
  }                                                              \
  TEST(mnemonic##_4S_2OPIMM) {                                   \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                       \
                                 4S,                             \
                                 4H,                             \
                                 kInput16bits##input,            \
                                 kInput16bitsImm##input_imm);    \
  }                                                              \
  TEST(mnemonic##_2D_2OPIMM) {                                   \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                       \
                                 2D,                             \
                                 2S,                             \
                                 kInput32bits##input,            \
                                 kInput32bitsImm##input_imm);    \
  }                                                              \
  TEST(mnemonic##2_8H_2OPIMM) {                                  \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic##2,                    \
                                 8H,                             \
                                 16B,                            \
                                 kInput8bits##input,             \
                                 kInput8bitsImm##input_imm);     \
  }                                                              \
  TEST(mnemonic##2_4S_2OPIMM) {                                  \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic##2,                    \
                                 4S,                             \
                                 8H,                             \
                                 kInput16bits##input,            \
                                 kInput16bitsImm##input_imm);    \
  }                                                              \
  TEST(mnemonic##2_2D_2OPIMM) {                                  \
    CALL_TEST_NEON_HELPER_2OPIMM(mnemonic##2,                    \
                                 2D,                             \
                                 4S,                             \
                                 kInput32bits##input,            \
                                 kInput32bitsImm##input_imm);    \
  }

#define CALL_TEST_NEON_HELPER_BYELEMENT_DOT_PRODUCT(mnemonic,           \
                                                    vdform,             \
                                                    vnform,             \
                                                    vmform,             \
                                                    input_d,            \
                                                    input_n,            \
                                                    input_m,            \
                                                    indices,            \
                                                    vm_subvector_count) \
  {                                                                     \
    CALL_TEST_NEON_HELPER_ByElement_Dot_Product(mnemonic,               \
                                                vdform,                 \
                                                vnform,                 \
                                                vmform,                 \
                                                input_d,                \
                                                input_n,                \
                                                input_m,                \
                                                indices,                \
                                                vm_subvector_count);    \
  }

#define DEFINE_TEST_NEON_BYELEMENT_DOT_PRODUCT(mnemonic,               \
                                               input_d,                \
                                               input_n,                \
                                               input_m)                \
  TEST(mnemonic##_2S_8B_B) {                                           \
    CALL_TEST_NEON_HELPER_BYELEMENT_DOT_PRODUCT(mnemonic,              \
                                                2S,                    \
                                                8B,                    \
                                                B,                     \
                                                kInput32bits##input_d, \
                                                kInput8bits##input_n,  \
                                                kInput8bits##input_m,  \
                                                kInputSIndices,        \
                                                4);                    \
  }                                                                    \
  TEST(mnemonic##_4S_16B_B) {                                          \
    CALL_TEST_NEON_HELPER_BYELEMENT_DOT_PRODUCT(mnemonic,              \
                                                4S,                    \
                                                16B,                   \
                                                B,                     \
                                                kInput32bits##input_d, \
                                                kInput8bits##input_n,  \
                                                kInput8bits##input_m,  \
                                                kInputSIndices,        \
                                                4);                    \
  }

#define CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic, \
                                        vdform,   \
                                        vnform,   \
                                        vmform,   \
                                        input_d,  \
                                        input_n,  \
                                        input_m,  \
                                        indices)  \
  {                                               \
    CALL_TEST_NEON_HELPER_ByElement(mnemonic,     \
                                    vdform,       \
                                    vnform,       \
                                    vmform,       \
                                    input_d,      \
                                    input_n,      \
                                    input_m,      \
                                    indices);     \
  }

#define DEFINE_TEST_NEON_BYELEMENT(mnemonic, input_d, input_n, input_m) \
  TEST(mnemonic##_4H_4H_H) {                                            \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                           \
                                    4H,                                 \
                                    4H,                                 \
                                    H,                                  \
                                    kInput16bits##input_d,              \
                                    kInput16bits##input_n,              \
                                    kInput16bits##input_m,              \
                                    kInputHIndices);                    \
  }                                                                     \
  TEST(mnemonic##_8H_8H_H) {                                            \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                           \
                                    8H,                                 \
                                    8H,                                 \
                                    H,                                  \
                                    kInput16bits##input_d,              \
                                    kInput16bits##input_n,              \
                                    kInput16bits##input_m,              \
                                    kInputHIndices);                    \
  }                                                                     \
  TEST(mnemonic##_2S_2S_S) {                                            \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                           \
                                    2S,                                 \
                                    2S,                                 \
                                    S,                                  \
                                    kInput32bits##input_d,              \
                                    kInput32bits##input_n,              \
                                    kInput32bits##input_m,              \
                                    kInputSIndices);                    \
  }                                                                     \
  TEST(mnemonic##_4S_4S_S) {                                            \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                           \
                                    4S,                                 \
                                    4S,                                 \
                                    S,                                  \
                                    kInput32bits##input_d,              \
                                    kInput32bits##input_n,              \
                                    kInput32bits##input_m,              \
                                    kInputSIndices);                    \
  }

#define DEFINE_TEST_NEON_BYELEMENT_SCALAR(mnemonic, input_d, input_n, input_m) \
  TEST(mnemonic##_H_H_H) {                                                     \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                                  \
                                    H,                                         \
                                    H,                                         \
                                    H,                                         \
                                    kInput16bits##input_d,                     \
                                    kInput16bits##input_n,                     \
                                    kInput16bits##input_m,                     \
                                    kInputHIndices);                           \
  }                                                                            \
  TEST(mnemonic##_S_S_S) {                                                     \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                                  \
                                    S,                                         \
                                    S,                                         \
                                    S,                                         \
                                    kInput32bits##input_d,                     \
                                    kInput32bits##input_n,                     \
                                    kInput32bits##input_m,                     \
                                    kInputSIndices);                           \
  }

#define DEFINE_TEST_NEON_FP_BYELEMENT(mnemonic, input_d, input_n, input_m) \
  TEST(mnemonic##_4H_4H_H) {                                               \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                    4H,                                    \
                                    4H,                                    \
                                    H,                                     \
                                    kInputFloat16##input_d,                \
                                    kInputFloat16##input_n,                \
                                    kInputFloat16##input_m,                \
                                    kInputHIndices);                       \
  }                                                                        \
  TEST(mnemonic##_8H_8H_H) {                                               \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                    8H,                                    \
                                    8H,                                    \
                                    H,                                     \
                                    kInputFloat16##input_d,                \
                                    kInputFloat16##input_n,                \
                                    kInputFloat16##input_m,                \
                                    kInputHIndices);                       \
  }                                                                        \
  TEST(mnemonic##_2S_2S_S) {                                               \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                    2S,                                    \
                                    2S,                                    \
                                    S,                                     \
                                    kInputFloat##input_d,                  \
                                    kInputFloat##input_n,                  \
                                    kInputFloat##input_m,                  \
                                    kInputSIndices);                       \
  }                                                                        \
  TEST(mnemonic##_4S_4S_S) {                                               \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                    4S,                                    \
                                    4S,                                    \
                                    S,                                     \
                                    kInputFloat##input_d,                  \
                                    kInputFloat##input_n,                  \
                                    kInputFloat##input_m,                  \
                                    kInputSIndices);                       \
  }                                                                        \
  TEST(mnemonic##_2D_2D_D) {                                               \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                    2D,                                    \
                                    2D,                                    \
                                    D,                                     \
                                    kInputDouble##input_d,                 \
                                    kInputDouble##input_n,                 \
                                    kInputDouble##input_m,                 \
                                    kInputDIndices);                       \
  }

#define DEFINE_TEST_NEON_FHM_BYELEMENT(mnemonic, input_d, input_n, input_m) \
  TEST(mnemonic##_2S_2H_H) {                                                \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                               \
                                    2S,                                     \
                                    2H,                                     \
                                    H,                                      \
                                    kInputFloatAccDestination,              \
                                    kInputFloat16##input_n,                 \
                                    kInputFloat16##input_m,                 \
                                    kInputHIndices);                        \
  }                                                                         \
  TEST(mnemonic##_4S_4H_H) {                                                \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                               \
                                    4S,                                     \
                                    4H,                                     \
                                    H,                                      \
                                    kInputFloatAccDestination,              \
                                    kInputFloat16##input_n,                 \
                                    kInputFloat16##input_m,                 \
                                    kInputHIndices);                        \
  }

#define DEFINE_TEST_NEON_FP_BYELEMENT_SCALAR(mnemonic, inp_d, inp_n, inp_m) \
  TEST(mnemonic##_H_H_H) {                                                  \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                               \
                                    H,                                      \
                                    H,                                      \
                                    H,                                      \
                                    kInputFloat16##inp_d,                   \
                                    kInputFloat16##inp_n,                   \
                                    kInputFloat16##inp_m,                   \
                                    kInputHIndices);                        \
  }                                                                         \
  TEST(mnemonic##_S_S_S) {                                                  \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                               \
                                    S,                                      \
                                    S,                                      \
                                    S,                                      \
                                    kInputFloat##inp_d,                     \
                                    kInputFloat##inp_n,                     \
                                    kInputFloat##inp_m,                     \
                                    kInputSIndices);                        \
  }                                                                         \
  TEST(mnemonic##_D_D_D) {                                                  \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                               \
                                    D,                                      \
                                    D,                                      \
                                    D,                                      \
                                    kInputDouble##inp_d,                    \
                                    kInputDouble##inp_n,                    \
                                    kInputDouble##inp_m,                    \
                                    kInputDIndices);                        \
  }


#define DEFINE_TEST_NEON_BYELEMENT_DIFF(mnemonic, input_d, input_n, input_m) \
  TEST(mnemonic##_4S_4H_H) {                                                 \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                                \
                                    4S,                                      \
                                    4H,                                      \
                                    H,                                       \
                                    kInput32bits##input_d,                   \
                                    kInput16bits##input_n,                   \
                                    kInput16bits##input_m,                   \
                                    kInputHIndices);                         \
  }                                                                          \
  TEST(mnemonic##2_4S_8H_H) {                                                \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic##2,                             \
                                    4S,                                      \
                                    8H,                                      \
                                    H,                                       \
                                    kInput32bits##input_d,                   \
                                    kInput16bits##input_n,                   \
                                    kInput16bits##input_m,                   \
                                    kInputHIndices);                         \
  }                                                                          \
  TEST(mnemonic##_2D_2S_S) {                                                 \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                                \
                                    2D,                                      \
                                    2S,                                      \
                                    S,                                       \
                                    kInput64bits##input_d,                   \
                                    kInput32bits##input_n,                   \
                                    kInput32bits##input_m,                   \
                                    kInputSIndices);                         \
  }                                                                          \
  TEST(mnemonic##2_2D_4S_S) {                                                \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic##2,                             \
                                    2D,                                      \
                                    4S,                                      \
                                    S,                                       \
                                    kInput64bits##input_d,                   \
                                    kInput32bits##input_n,                   \
                                    kInput32bits##input_m,                   \
                                    kInputSIndices);                         \
  }

#define DEFINE_TEST_NEON_BYELEMENT_DIFF_SCALAR(mnemonic,   \
                                               input_d,    \
                                               input_n,    \
                                               input_m)    \
  TEST(mnemonic##_S_H_H) {                                 \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,              \
                                    S,                     \
                                    H,                     \
                                    H,                     \
                                    kInput32bits##input_d, \
                                    kInput16bits##input_n, \
                                    kInput16bits##input_m, \
                                    kInputHIndices);       \
  }                                                        \
  TEST(mnemonic##_D_S_S) {                                 \
    CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,              \
                                    D,                     \
                                    S,                     \
                                    S,                     \
                                    kInput64bits##input_d, \
                                    kInput32bits##input_n, \
                                    kInput32bits##input_m, \
                                    kInputSIndices);       \
  }


#define CALL_TEST_NEON_HELPER_2OP2IMM(mnemonic,                 \
                                      variant,                  \
                                      input_d,                  \
                                      input_imm1,               \
                                      input_n,                  \
                                      input_imm2)               \
  {                                                             \
    CALL_TEST_NEON_HELPER_OpImmOpImm(&MacroAssembler::mnemonic, \
                                     mnemonic,                  \
                                     variant,                   \
                                     variant,                   \
                                     input_d,                   \
                                     input_imm1,                \
                                     input_n,                   \
                                     input_imm2);               \
  }

#define DEFINE_TEST_NEON_2OP2IMM(mnemonic,                      \
                                 input_d,                       \
                                 input_imm1,                    \
                                 input_n,                       \
                                 input_imm2)                    \
  TEST(mnemonic##_B) {                                          \
    CALL_TEST_NEON_HELPER_2OP2IMM(mnemonic,                     \
                                  16B,                          \
                                  kInput8bits##input_d,         \
                                  kInput8bitsImm##input_imm1,   \
                                  kInput8bits##input_n,         \
                                  kInput8bitsImm##input_imm2);  \
  }                                                             \
  TEST(mnemonic##_H) {                                          \
    CALL_TEST_NEON_HELPER_2OP2IMM(mnemonic,                     \
                                  8H,                           \
                                  kInput16bits##input_d,        \
                                  kInput16bitsImm##input_imm1,  \
                                  kInput16bits##input_n,        \
                                  kInput16bitsImm##input_imm2); \
  }                                                             \
  TEST(mnemonic##_S) {                                          \
    CALL_TEST_NEON_HELPER_2OP2IMM(mnemonic,                     \
                                  4S,                           \
                                  kInput32bits##input_d,        \
                                  kInput32bitsImm##input_imm1,  \
                                  kInput32bits##input_n,        \
                                  kInput32bitsImm##input_imm2); \
  }                                                             \
  TEST(mnemonic##_D) {                                          \
    CALL_TEST_NEON_HELPER_2OP2IMM(mnemonic,                     \
                                  2D,                           \
                                  kInput64bits##input_d,        \
                                  kInput64bitsImm##input_imm1,  \
                                  kInput64bits##input_n,        \
                                  kInput64bitsImm##input_imm2); \
  }


// Advanced SIMD copy.
DEFINE_TEST_NEON_2OP2IMM(
    ins, Basic, LaneCountFromZero, Basic, LaneCountFromZero)
DEFINE_TEST_NEON_2OPIMM_COPY(dup, Basic, LaneCountFromZero)


// Advanced SIMD scalar copy.
DEFINE_TEST_NEON_2OPIMM_SCALAR(dup, Basic, LaneCountFromZero)


// Advanced SIMD three same.
DEFINE_TEST_NEON_3SAME_NO2D(shadd, Basic)
DEFINE_TEST_NEON_3SAME(sqadd, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(srhadd, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(shsub, Basic)
DEFINE_TEST_NEON_3SAME(sqsub, Basic)
DEFINE_TEST_NEON_3SAME(cmgt, Basic)
DEFINE_TEST_NEON_3SAME(cmge, Basic)
DEFINE_TEST_NEON_3SAME(sshl, Basic)
DEFINE_TEST_NEON_3SAME(sqshl, Basic)
DEFINE_TEST_NEON_3SAME(srshl, Basic)
DEFINE_TEST_NEON_3SAME(sqrshl, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(smax, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(smin, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(sabd, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(saba, Basic)
DEFINE_TEST_NEON_3SAME(add, Basic)
DEFINE_TEST_NEON_3SAME(cmtst, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(mla, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(mul, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(smaxp, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(sminp, Basic)
DEFINE_TEST_NEON_3SAME_HS(sqdmulh, Basic)
DEFINE_TEST_NEON_3SAME(addp, Basic)
DEFINE_TEST_NEON_3SAME_FP(fmaxnm, Basic)
DEFINE_TEST_NEON_3SAME_FP(fmla, Basic)
DEFINE_TEST_NEON_3SAME_FP(fadd, Basic)
DEFINE_TEST_NEON_3SAME_FP(fmulx, Basic)
DEFINE_TEST_NEON_3SAME_FP(fcmeq, Basic)
DEFINE_TEST_NEON_3SAME_FP(fmax, Basic)
DEFINE_TEST_NEON_3SAME_FP(frecps, Basic)
DEFINE_TEST_NEON_3SAME_8B_16B(and_, Basic)
DEFINE_TEST_NEON_3SAME_8B_16B(bic, Basic)
DEFINE_TEST_NEON_3SAME_FP(fminnm, Basic)
DEFINE_TEST_NEON_3SAME_FP(fmls, Basic)
DEFINE_TEST_NEON_3SAME_FP(fsub, Basic)
DEFINE_TEST_NEON_3SAME_FP(fmin, Basic)
DEFINE_TEST_NEON_3SAME_FP(frsqrts, Basic)
DEFINE_TEST_NEON_3SAME_8B_16B(orr, Basic)
DEFINE_TEST_NEON_3SAME_8B_16B(orn, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(uhadd, Basic)
DEFINE_TEST_NEON_3SAME(uqadd, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(urhadd, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(uhsub, Basic)
DEFINE_TEST_NEON_3SAME(uqsub, Basic)
DEFINE_TEST_NEON_3SAME(cmhi, Basic)
DEFINE_TEST_NEON_3SAME(cmhs, Basic)
DEFINE_TEST_NEON_3SAME(ushl, Basic)
DEFINE_TEST_NEON_3SAME(uqshl, Basic)
DEFINE_TEST_NEON_3SAME(urshl, Basic)
DEFINE_TEST_NEON_3SAME(uqrshl, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(umax, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(umin, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(uabd, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(uaba, Basic)
DEFINE_TEST_NEON_3SAME(sub, Basic)
DEFINE_TEST_NEON_3SAME(cmeq, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(mls, Basic)
DEFINE_TEST_NEON_3SAME_8B_16B(pmul, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(uminp, Basic)
DEFINE_TEST_NEON_3SAME_NO2D(umaxp, Basic)
DEFINE_TEST_NEON_3SAME_HS(sqrdmulh, Basic)
DEFINE_TEST_NEON_3SAME_HS(sqrdmlah, Basic)
DEFINE_TEST_NEON_3SAME_HS(sqrdmlsh, Basic)
DEFINE_TEST_NEON_3DIFF_DOUBLE_WIDE(udot, Basic)
DEFINE_TEST_NEON_3DIFF_DOUBLE_WIDE(sdot, Basic)
DEFINE_TEST_NEON_3SAME_FP(fmaxnmp, Basic)
DEFINE_TEST_NEON_3SAME_FP(faddp, Basic)
DEFINE_TEST_NEON_3SAME_FP(fmul, Basic)
DEFINE_TEST_NEON_3SAME_FP(fcmge, Basic)
DEFINE_TEST_NEON_3SAME_FP(facge, Basic)
DEFINE_TEST_NEON_3SAME_FP(fmaxp, Basic)
DEFINE_TEST_NEON_3SAME_FP(fdiv, Basic)
DEFINE_TEST_NEON_3SAME_8B_16B(eor, Basic)
DEFINE_TEST_NEON_3SAME_8B_16B(bsl, Basic)
DEFINE_TEST_NEON_3SAME_FP(fminnmp, Basic)
DEFINE_TEST_NEON_3SAME_FP(fabd, Basic)
DEFINE_TEST_NEON_3SAME_FP(fcmgt, Basic)
DEFINE_TEST_NEON_3SAME_FP(facgt, Basic)
DEFINE_TEST_NEON_3SAME_FP(fminp, Basic)
DEFINE_TEST_NEON_3SAME_8B_16B(bit, Basic)
DEFINE_TEST_NEON_3SAME_8B_16B(bif, Basic)


// Advanced SIMD scalar three same.
DEFINE_TEST_NEON_3SAME_SCALAR(sqadd, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR(sqsub, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(cmgt, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(cmge, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(sshl, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR(sqshl, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(srshl, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR(sqrshl, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(add, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(cmtst, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_HS(sqdmulh, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(fmulx, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(fcmeq, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(frecps, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(frsqrts, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(uqadd, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(uqsub, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(cmhi, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(cmhs, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(ushl, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR(uqshl, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(urshl, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR(uqrshl, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(sub, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_D(cmeq, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_HS(sqrdmulh, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_HS(sqrdmlah, Basic)
DEFINE_TEST_NEON_3SAME_SCALAR_HS(sqrdmlsh, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(fcmge, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(facge, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(fabd, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(fcmgt, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(facgt, Basic)


// Advanced SIMD FHM instructions (FMLAL, FMLSL).
// These are oddballs: they are encoded under the 3SAME group but behave
// quite differently.
DEFINE_TEST_NEON_FHM(fmlal, Basic, Basic, Basic)
DEFINE_TEST_NEON_FHM(fmlal2, Basic, Basic, Basic)
DEFINE_TEST_NEON_FHM(fmlsl, Basic, Basic, Basic)
DEFINE_TEST_NEON_FHM(fmlsl2, Basic, Basic, Basic)


// Advanced SIMD three different.
DEFINE_TEST_NEON_3DIFF_LONG(saddl, Basic)
DEFINE_TEST_NEON_3DIFF_WIDE(saddw, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(ssubl, Basic)
DEFINE_TEST_NEON_3DIFF_WIDE(ssubw, Basic)
DEFINE_TEST_NEON_3DIFF_NARROW(addhn, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(sabal, Basic)
DEFINE_TEST_NEON_3DIFF_NARROW(subhn, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(sabdl, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(smlal, Basic)
DEFINE_TEST_NEON_3DIFF_LONG_SD(sqdmlal, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(smlsl, Basic)
DEFINE_TEST_NEON_3DIFF_LONG_SD(sqdmlsl, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(smull, Basic)
DEFINE_TEST_NEON_3DIFF_LONG_SD(sqdmull, Basic)
DEFINE_TEST_NEON_3DIFF_LONG_8H(pmull, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(uaddl, Basic)
DEFINE_TEST_NEON_3DIFF_WIDE(uaddw, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(usubl, Basic)
DEFINE_TEST_NEON_3DIFF_WIDE(usubw, Basic)
DEFINE_TEST_NEON_3DIFF_NARROW(raddhn, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(uabal, Basic)
DEFINE_TEST_NEON_3DIFF_NARROW(rsubhn, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(uabdl, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(umlal, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(umlsl, Basic)
DEFINE_TEST_NEON_3DIFF_LONG(umull, Basic)


// Advanced SIMD scalar three different.
DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_SD(sqdmlal, Basic)
DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_SD(sqdmlsl, Basic)
DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_SD(sqdmull, Basic)


// Advanced SIMD scalar pairwise.
TEST(addp_SCALAR) {
  CALL_TEST_NEON_HELPER_2DIFF(addp, D, 2D, kInput64bitsBasic);
}
DEFINE_TEST_NEON_2DIFF_FP_SCALAR_SD(fmaxnmp, Basic)
DEFINE_TEST_NEON_2DIFF_FP_SCALAR_SD(faddp, Basic)
DEFINE_TEST_NEON_2DIFF_FP_SCALAR_SD(fmaxp, Basic)
DEFINE_TEST_NEON_2DIFF_FP_SCALAR_SD(fminnmp, Basic)
DEFINE_TEST_NEON_2DIFF_FP_SCALAR_SD(fminp, Basic)


// Advanced SIMD shift by immediate.
DEFINE_TEST_NEON_2OPIMM(sshr, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM(ssra, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM(srshr, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM(srsra, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM(shl, Basic, TypeWidthFromZero)
DEFINE_TEST_NEON_2OPIMM(sqshl, Basic, TypeWidthFromZero)
DEFINE_TEST_NEON_2OPIMM_NARROW(shrn, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_NARROW(rshrn, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_NARROW(sqshrn, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_NARROW(sqrshrn, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_LONG(sshll, Basic, TypeWidthFromZero)
DEFINE_TEST_NEON_2OPIMM_HSD(scvtf,
                            FixedPointConversions,
                            TypeWidthFromZeroToWidth)
DEFINE_TEST_NEON_2OPIMM_FP(fcvtzs, Conversions, TypeWidthFromZeroToWidth)
DEFINE_TEST_NEON_2OPIMM(ushr, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM(usra, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM(urshr, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM(ursra, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM(sri, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM(sli, Basic, TypeWidthFromZero)
DEFINE_TEST_NEON_2OPIMM(sqshlu, Basic, TypeWidthFromZero)
DEFINE_TEST_NEON_2OPIMM(uqshl, Basic, TypeWidthFromZero)
DEFINE_TEST_NEON_2OPIMM_NARROW(sqshrun, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_NARROW(sqrshrun, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_NARROW(uqshrn, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_NARROW(uqrshrn, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_LONG(ushll, Basic, TypeWidthFromZero)
DEFINE_TEST_NEON_2OPIMM_HSD(ucvtf,
                            FixedPointConversions,
                            TypeWidthFromZeroToWidth)
DEFINE_TEST_NEON_2OPIMM_FP(fcvtzu, Conversions, TypeWidthFromZeroToWidth)


// Advanced SIMD scalar shift by immediate..
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(sshr, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(ssra, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(srshr, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(srsra, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(shl, Basic, TypeWidthFromZero)
DEFINE_TEST_NEON_2OPIMM_SCALAR(sqshl, Basic, TypeWidthFromZero)
DEFINE_TEST_NEON_2OPIMM_SCALAR_NARROW(sqshrn, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_NARROW(sqrshrn, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_HSD(scvtf,
                                   FixedPointConversions,
                                   TypeWidthFromZeroToWidth)
DEFINE_TEST_NEON_2OPIMM_FP_SCALAR(fcvtzs, Conversions, TypeWidthFromZeroToWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(ushr, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(usra, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(urshr, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(ursra, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(sri, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(sli, Basic, TypeWidthFromZero)
DEFINE_TEST_NEON_2OPIMM_SCALAR(sqshlu, Basic, TypeWidthFromZero)
DEFINE_TEST_NEON_2OPIMM_SCALAR(uqshl, Basic, TypeWidthFromZero)
DEFINE_TEST_NEON_2OPIMM_SCALAR_NARROW(sqshrun, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_NARROW(sqrshrun, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_NARROW(uqshrn, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_NARROW(uqrshrn, Basic, TypeWidth)
DEFINE_TEST_NEON_2OPIMM_SCALAR_HSD(ucvtf,
                                   FixedPointConversions,
                                   TypeWidthFromZeroToWidth)
DEFINE_TEST_NEON_2OPIMM_FP_SCALAR(fcvtzu, Conversions, TypeWidthFromZeroToWidth)


// Advanced SIMD two-register miscellaneous.
DEFINE_TEST_NEON_2SAME_NO2D(rev64, Basic)
DEFINE_TEST_NEON_2SAME_8B_16B(rev16, Basic)
DEFINE_TEST_NEON_2DIFF_LONG(saddlp, Basic)
DEFINE_TEST_NEON_2SAME(suqadd, Basic)
DEFINE_TEST_NEON_2SAME_NO2D(cls, Basic)
DEFINE_TEST_NEON_2SAME_8B_16B(cnt, Basic)
DEFINE_TEST_NEON_2DIFF_LONG(sadalp, Basic)
DEFINE_TEST_NEON_2SAME(sqabs, Basic)
DEFINE_TEST_NEON_2OPIMM(cmgt, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM(cmeq, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM(cmlt, Basic, Zero)
DEFINE_TEST_NEON_2SAME(abs, Basic)
DEFINE_TEST_NEON_2DIFF_NARROW(xtn, Basic)
DEFINE_TEST_NEON_2DIFF_NARROW(sqxtn, Basic)
DEFINE_TEST_NEON_2DIFF_FP_NARROW(fcvtn, Conversions)
DEFINE_TEST_NEON_2DIFF_FP_LONG(fcvtl, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(frintn, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(frintm, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(fcvtns, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(fcvtms, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(fcvtas, Conversions)
// SCVTF (vector, integer) covered by SCVTF(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2OPIMM_FCMP_ZERO(fcmgt, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_FCMP_ZERO(fcmeq, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_FCMP_ZERO(fcmlt, Basic, Zero)
DEFINE_TEST_NEON_2SAME_FP_FP16(fabs, Basic)
DEFINE_TEST_NEON_2SAME_FP_FP16(frintp, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(frintz, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(fcvtps, Conversions)
// FCVTZS(vector, integer) covered by FCVTZS(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2SAME_2S_4S(urecpe, Basic)
DEFINE_TEST_NEON_2SAME_FP_FP16(frecpe, Basic)
DEFINE_TEST_NEON_2SAME_BH(rev32, Basic)
DEFINE_TEST_NEON_2DIFF_LONG(uaddlp, Basic)
DEFINE_TEST_NEON_2SAME(usqadd, Basic)
DEFINE_TEST_NEON_2SAME_NO2D(clz, Basic)
DEFINE_TEST_NEON_2DIFF_LONG(uadalp, Basic)
DEFINE_TEST_NEON_2SAME(sqneg, Basic)
DEFINE_TEST_NEON_2OPIMM(cmge, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM(cmle, Basic, Zero)
DEFINE_TEST_NEON_2SAME(neg, Basic)
DEFINE_TEST_NEON_2DIFF_NARROW(sqxtun, Basic)
DEFINE_TEST_NEON_2OPIMM_LONG(shll, Basic, SHLL)
DEFINE_TEST_NEON_2DIFF_NARROW(uqxtn, Basic)
DEFINE_TEST_NEON_2DIFF_FP_NARROW_2S(fcvtxn, Conversions)
DEFINE_TEST_NEON_2SAME_FP(frint32x, Conversions)
DEFINE_TEST_NEON_2SAME_FP(frint64x, Conversions)
DEFINE_TEST_NEON_2SAME_FP(frint32z, Conversions)
DEFINE_TEST_NEON_2SAME_FP(frint64z, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(frinta, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(frintx, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(fcvtnu, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(fcvtmu, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(fcvtau, Conversions)
// UCVTF (vector, integer) covered by UCVTF(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2SAME_8B_16B(not_, Basic)
DEFINE_TEST_NEON_2SAME_8B_16B(rbit, Basic)
DEFINE_TEST_NEON_2OPIMM_FCMP_ZERO(fcmge, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_FCMP_ZERO(fcmle, Basic, Zero)
DEFINE_TEST_NEON_2SAME_FP_FP16(fneg, Basic)
DEFINE_TEST_NEON_2SAME_FP_FP16(frinti, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16(fcvtpu, Conversions)
// FCVTZU(vector, integer) covered by FCVTZU(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2SAME_2S_4S(ursqrte, Basic)
DEFINE_TEST_NEON_2SAME_FP_FP16(frsqrte, Basic)
DEFINE_TEST_NEON_2SAME_FP_FP16(fsqrt, Basic)


// Advanced SIMD scalar two-register miscellaneous.
DEFINE_TEST_NEON_2SAME_SCALAR(suqadd, Basic)
DEFINE_TEST_NEON_2SAME_SCALAR(sqabs, Basic)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(cmgt, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(cmeq, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(cmlt, Basic, Zero)
DEFINE_TEST_NEON_2SAME_SCALAR_D(abs, Basic)
DEFINE_TEST_NEON_2DIFF_SCALAR_NARROW(sqxtn, Basic)
DEFINE_TEST_NEON_2SAME_FP_FP16_SCALAR(fcvtns, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16_SCALAR(fcvtms, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16_SCALAR(fcvtas, Conversions)
// SCVTF (vector, integer) covered by SCVTF(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_HSD(fcmgt, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_HSD(fcmeq, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_HSD(fcmlt, Basic, Zero)
DEFINE_TEST_NEON_2SAME_FP_FP16_SCALAR(fcvtps, Conversions)
// FCVTZS(vector, integer) covered by FCVTZS(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2SAME_FP_FP16_SCALAR(frecpe, Basic)
DEFINE_TEST_NEON_2SAME_FP_FP16_SCALAR(frecpx, Basic)
DEFINE_TEST_NEON_2SAME_SCALAR(usqadd, Basic)
DEFINE_TEST_NEON_2SAME_SCALAR(sqneg, Basic)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(cmge, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(cmle, Basic, Zero)
DEFINE_TEST_NEON_2SAME_SCALAR_D(neg, Basic)
DEFINE_TEST_NEON_2DIFF_SCALAR_NARROW(sqxtun, Basic)
DEFINE_TEST_NEON_2DIFF_SCALAR_NARROW(uqxtn, Basic)
TEST(fcvtxn_SCALAR) {
  CALL_TEST_NEON_HELPER_2DIFF(fcvtxn, S, D, kInputDoubleConversions);
}
DEFINE_TEST_NEON_2SAME_FP_FP16_SCALAR(fcvtnu, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16_SCALAR(fcvtmu, Conversions)
DEFINE_TEST_NEON_2SAME_FP_FP16_SCALAR(fcvtau, Conversions)
// UCVTF (vector, integer) covered by UCVTF(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_HSD(fcmge, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_HSD(fcmle, Basic, Zero)
DEFINE_TEST_NEON_2SAME_FP_FP16_SCALAR(fcvtpu, Conversions)
// FCVTZU(vector, integer) covered by FCVTZU(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2SAME_FP_FP16_SCALAR(frsqrte, Basic)


// Advanced SIMD across lanes.
DEFINE_TEST_NEON_ACROSS_LONG(saddlv, Basic)
DEFINE_TEST_NEON_ACROSS(smaxv, Basic)
DEFINE_TEST_NEON_ACROSS(sminv, Basic)
DEFINE_TEST_NEON_ACROSS(addv, Basic)
DEFINE_TEST_NEON_ACROSS_LONG(uaddlv, Basic)
DEFINE_TEST_NEON_ACROSS(umaxv, Basic)
DEFINE_TEST_NEON_ACROSS(uminv, Basic)
DEFINE_TEST_NEON_ACROSS_FP(fmaxnmv, Basic)
DEFINE_TEST_NEON_ACROSS_FP(fmaxv, Basic)
DEFINE_TEST_NEON_ACROSS_FP(fminnmv, Basic)
DEFINE_TEST_NEON_ACROSS_FP(fminv, Basic)


// Advanced SIMD permute.
DEFINE_TEST_NEON_3SAME(uzp1, Basic)
DEFINE_TEST_NEON_3SAME(trn1, Basic)
DEFINE_TEST_NEON_3SAME(zip1, Basic)
DEFINE_TEST_NEON_3SAME(uzp2, Basic)
DEFINE_TEST_NEON_3SAME(trn2, Basic)
DEFINE_TEST_NEON_3SAME(zip2, Basic)


// Advanced SIMD vector x indexed element.
DEFINE_TEST_NEON_BYELEMENT_DIFF(smlal, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_DIFF(sqdmlal, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_DIFF(smlsl, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_DIFF(sqdmlsl, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT(mul, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_DIFF(smull, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_DIFF(sqdmull, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT(sqdmulh, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT(sqrdmulh, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT(sqrdmlah, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT(sqrdmlsh, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_DOT_PRODUCT(udot, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_DOT_PRODUCT(sdot, Basic, Basic, Basic)
DEFINE_TEST_NEON_FP_BYELEMENT(fmla, Basic, Basic, Basic)
DEFINE_TEST_NEON_FP_BYELEMENT(fmls, Basic, Basic, Basic)
DEFINE_TEST_NEON_FP_BYELEMENT(fmul, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT(mla, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_DIFF(umlal, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT(mls, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_DIFF(umlsl, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_DIFF(umull, Basic, Basic, Basic)
DEFINE_TEST_NEON_FP_BYELEMENT(fmulx, Basic, Basic, Basic)


// Advanced SIMD scalar x indexed element.
DEFINE_TEST_NEON_BYELEMENT_DIFF_SCALAR(sqdmlal, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_DIFF_SCALAR(sqdmlsl, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_DIFF_SCALAR(sqdmull, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_SCALAR(sqdmulh, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_SCALAR(sqrdmulh, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_SCALAR(sqrdmlah, Basic, Basic, Basic)
DEFINE_TEST_NEON_BYELEMENT_SCALAR(sqrdmlsh, Basic, Basic, Basic)
DEFINE_TEST_NEON_FP_BYELEMENT_SCALAR(fmla, Basic, Basic, Basic)
DEFINE_TEST_NEON_FP_BYELEMENT_SCALAR(fmls, Basic, Basic, Basic)
DEFINE_TEST_NEON_FP_BYELEMENT_SCALAR(fmul, Basic, Basic, Basic)
DEFINE_TEST_NEON_FP_BYELEMENT_SCALAR(fmulx, Basic, Basic, Basic)


DEFINE_TEST_NEON_FHM_BYELEMENT(fmlal, Basic, Basic, Basic)
DEFINE_TEST_NEON_FHM_BYELEMENT(fmlal2, Basic, Basic, Basic)
DEFINE_TEST_NEON_FHM_BYELEMENT(fmlsl, Basic, Basic, Basic)
DEFINE_TEST_NEON_FHM_BYELEMENT(fmlsl2, Basic, Basic, Basic)


#ifdef VIXL_ENABLE_IMPLICIT_CHECKS
TEST(ImplicitCheck) {
  SETUP_WITH_FEATURES(CPUFeatures::kNEON);
  START_IMPLICIT_CHECK();

  EmissionCheckScope guard(&masm, masm.GetBuffer()->GetRemainingBytes());
  // Invalid memory reads.
  __ ldar(w3, bad_memory);
  __ ldar(x4, bad_memory);
  __ ldarb(w5, bad_memory);
  __ ldarb(x6, bad_memory);
  __ ldarh(w7, bad_memory);
  __ ldarh(x8, bad_memory);
  __ ldaxp(w9, w10, bad_memory);
  __ ldaxp(x11, x12, bad_memory);
  __ ldaxr(w13, bad_memory);
  __ ldaxr(x14, bad_memory);
  __ ldaxrb(w15, bad_memory);
  __ ldaxrb(x16, bad_memory);
  __ ldaxrh(w17, bad_memory);
  __ ldaxrh(x18, bad_memory);
  __ ldnp(w19, w20, bad_memory);
  __ ldnp(x21, x22, bad_memory);
  __ ldp(w23, w24, bad_memory);
  __ ldp(x25, x26, bad_memory);
  __ ldpsw(x27, x28, bad_memory);
  __ ldr(w29, bad_memory);
  __ ldr(x2, bad_memory);
  __ ldrb(w3, bad_memory);
  __ ldrb(x4, bad_memory);
  __ ldrh(w5, bad_memory);
  __ ldrh(x6, bad_memory);
  __ ldrsb(w7, bad_memory);
  __ ldrsb(x8, bad_memory);
  __ ldrsh(w9, bad_memory);
  __ ldrsh(x10, bad_memory);
  __ ldrsw(x11, bad_memory);
  __ ldur(w12, bad_memory);
  __ ldur(x13, bad_memory);
  __ ldurb(w14, bad_memory);
  __ ldurb(x15, bad_memory);
  __ ldurh(w16, bad_memory);
  __ ldurh(x17, bad_memory);
  __ ldursb(w18, bad_memory);
  __ ldursb(x19, bad_memory);
  __ ldursh(w20, bad_memory);
  __ ldursh(x21, bad_memory);
  __ ldursw(x22, bad_memory);
  __ ldxp(w23, w24, bad_memory);
  __ ldxp(x25, x26, bad_memory);
  __ ldxr(w27, bad_memory);
  __ ldxr(x28, bad_memory);
  __ ldxrb(w29, bad_memory);
  __ ldxrb(x2, bad_memory);
  __ ldxrh(w3, bad_memory);
  __ ldxrh(x4, bad_memory);

  // Invalid memory writes. Note: exclusive store instructions are not tested
  // because they can fail due to the global monitor before trying to perform a
  // memory store.
  __ stlr(w18, bad_memory);
  __ stlr(x19, bad_memory);
  __ stlrb(w20, bad_memory);
  __ stlrb(x21, bad_memory);
  __ stlrh(w22, bad_memory);
  __ stlrh(x23, bad_memory);
  __ stnp(w14, w15, bad_memory);
  __ stnp(x16, x17, bad_memory);
  __ stp(w18, w19, bad_memory);
  __ stp(x20, x21, bad_memory);
  __ str(w22, bad_memory);
  __ str(x23, bad_memory);
  __ strb(w24, bad_memory);
  __ strb(x25, bad_memory);
  __ strh(w26, bad_memory);
  __ strh(x27, bad_memory);
  __ stur(w28, bad_memory);
  __ stur(x29, bad_memory);
  __ sturb(w2, bad_memory);
  __ sturb(x3, bad_memory);
  __ sturh(w4, bad_memory);
  __ sturh(x5, bad_memory);

  END_IMPLICIT_CHECK();
  TRY_RUN_IMPLICIT_CHECK();
}

TEST(ImplicitCheckNeon) {
  SETUP_WITH_FEATURES(CPUFeatures::kNEON);
  START_IMPLICIT_CHECK();

  EmissionCheckScope guard(&masm, masm.GetBuffer()->GetRemainingBytes());
  __ ld1(v18.V16B(), v19.V16B(), v20.V16B(), v21.V16B(), bad_memory);
  __ ld1(v23.V16B(), v24.V16B(), v25.V16B(), v26.V16B(), bad_memory);
  __ ld1(v5.V16B(), v6.V16B(), v7.V16B(), v8.V16B(), bad_memory);
  __ ld1(v18.V16B(), v19.V16B(), v20.V16B(), bad_memory);
  __ ld1(v13.V16B(), v14.V16B(), v15.V16B(), bad_memory);
  __ ld1(v19.V16B(), v20.V16B(), v21.V16B(), bad_memory);
  __ ld1(v17.V16B(), v18.V16B(), bad_memory);
  __ ld1(v20.V16B(), v21.V16B(), bad_memory);
  __ ld1(v28.V16B(), v29.V16B(), bad_memory);
  __ ld1(v29.V16B(), bad_memory);
  __ ld1(v21.V16B(), bad_memory);
  __ ld1(v4.V16B(), bad_memory);
  __ ld1(v4.V1D(), v5.V1D(), v6.V1D(), v7.V1D(), bad_memory);
  __ ld1(v17.V1D(), v18.V1D(), v19.V1D(), v20.V1D(), bad_memory);
  __ ld1(v28.V1D(), v29.V1D(), v30.V1D(), v31.V1D(), bad_memory);
  __ ld1(v20.V1D(), v21.V1D(), v22.V1D(), bad_memory);
  __ ld1(v19.V1D(), v20.V1D(), v21.V1D(), bad_memory);
  __ ld1(v12.V1D(), v13.V1D(), v14.V1D(), bad_memory);
  __ ld1(v29.V1D(), v30.V1D(), bad_memory);
  __ ld1(v31.V1D(), v0.V1D(), bad_memory);
  __ ld1(v3.V1D(), v4.V1D(), bad_memory);
  __ ld1(v28.V1D(), bad_memory);
  __ ld1(v11.V1D(), bad_memory);
  __ ld1(v29.V1D(), bad_memory);
  __ ld1(v28.V2D(), v29.V2D(), v30.V2D(), v31.V2D(), bad_memory);
  __ ld1(v8.V2D(), v9.V2D(), v10.V2D(), v11.V2D(), bad_memory);
  __ ld1(v14.V2D(), v15.V2D(), v16.V2D(), v17.V2D(), bad_memory);
  __ ld1(v26.V2D(), v27.V2D(), v28.V2D(), bad_memory);
  __ ld1(v5.V2D(), v6.V2D(), v7.V2D(), bad_memory);
  __ ld1(v26.V2D(), v27.V2D(), v28.V2D(), bad_memory);
  __ ld1(v18.V2D(), v19.V2D(), bad_memory);
  __ ld1(v21.V2D(), v22.V2D(), bad_memory);
  __ ld1(v17.V2D(), v18.V2D(), bad_memory);
  __ ld1(v5.V2D(), bad_memory);
  __ ld1(v6.V2D(), bad_memory);
  __ ld1(v15.V2D(), bad_memory);
  __ ld1(v30.V2S(), v31.V2S(), v0.V2S(), v1.V2S(), bad_memory);
  __ ld1(v24.V2S(), v25.V2S(), v26.V2S(), v27.V2S(), bad_memory);
  __ ld1(v27.V2S(), v28.V2S(), v29.V2S(), v30.V2S(), bad_memory);
  __ ld1(v11.V2S(), v12.V2S(), v13.V2S(), bad_memory);
  __ ld1(v8.V2S(), v9.V2S(), v10.V2S(), bad_memory);
  __ ld1(v31.V2S(), v0.V2S(), v1.V2S(), bad_memory);
  __ ld1(v0.V2S(), v1.V2S(), bad_memory);
  __ ld1(v13.V2S(), v14.V2S(), bad_memory);
  __ ld1(v3.V2S(), v4.V2S(), bad_memory);
  __ ld1(v26.V2S(), bad_memory);
  __ ld1(v0.V2S(), bad_memory);
  __ ld1(v11.V2S(), bad_memory);
  __ ld1(v16.V4H(), v17.V4H(), v18.V4H(), v19.V4H(), bad_memory);
  __ ld1(v24.V4H(), v25.V4H(), v26.V4H(), v27.V4H(), bad_memory);
  __ ld1(v1.V4H(), v2.V4H(), v3.V4H(), v4.V4H(), bad_memory);
  __ ld1(v30.V4H(), v31.V4H(), v0.V4H(), bad_memory);
  __ ld1(v25.V4H(), v26.V4H(), v27.V4H(), bad_memory);
  __ ld1(v3.V4H(), v4.V4H(), v5.V4H(), bad_memory);
  __ ld1(v3.V4H(), v4.V4H(), bad_memory);
  __ ld1(v3.V4H(), v4.V4H(), bad_memory);
  __ ld1(v23.V4H(), v24.V4H(), bad_memory);
  __ ld1(v26.V4H(), bad_memory);
  __ ld1(v1.V4H(), bad_memory);
  __ ld1(v14.V4H(), bad_memory);
  __ ld1(v26.V4S(), v27.V4S(), v28.V4S(), v29.V4S(), bad_memory);
  __ ld1(v28.V4S(), v29.V4S(), v30.V4S(), v31.V4S(), bad_memory);
  __ ld1(v4.V4S(), v5.V4S(), v6.V4S(), v7.V4S(), bad_memory);
  __ ld1(v2.V4S(), v3.V4S(), v4.V4S(), bad_memory);
  __ ld1(v22.V4S(), v23.V4S(), v24.V4S(), bad_memory);
  __ ld1(v15.V4S(), v16.V4S(), v17.V4S(), bad_memory);
  __ ld1(v20.V4S(), v21.V4S(), bad_memory);
  __ ld1(v30.V4S(), v31.V4S(), bad_memory);
  __ ld1(v11.V4S(), v12.V4S(), bad_memory);
  __ ld1(v15.V4S(), bad_memory);
  __ ld1(v12.V4S(), bad_memory);
  __ ld1(v0.V4S(), bad_memory);
  __ ld1(v17.V8B(), v18.V8B(), v19.V8B(), v20.V8B(), bad_memory);
  __ ld1(v5.V8B(), v6.V8B(), v7.V8B(), v8.V8B(), bad_memory);
  __ ld1(v9.V8B(), v10.V8B(), v11.V8B(), v12.V8B(), bad_memory);
  __ ld1(v4.V8B(), v5.V8B(), v6.V8B(), bad_memory);
  __ ld1(v2.V8B(), v3.V8B(), v4.V8B(), bad_memory);
  __ ld1(v12.V8B(), v13.V8B(), v14.V8B(), bad_memory);
  __ ld1(v10.V8B(), v11.V8B(), bad_memory);
  __ ld1(v11.V8B(), v12.V8B(), bad_memory);
  __ ld1(v27.V8B(), v28.V8B(), bad_memory);
  __ ld1(v31.V8B(), bad_memory);
  __ ld1(v10.V8B(), bad_memory);
  __ ld1(v28.V8B(), bad_memory);
  __ ld1(v5.V8H(), v6.V8H(), v7.V8H(), v8.V8H(), bad_memory);
  __ ld1(v2.V8H(), v3.V8H(), v4.V8H(), v5.V8H(), bad_memory);
  __ ld1(v10.V8H(), v11.V8H(), v12.V8H(), v13.V8H(), bad_memory);
  __ ld1(v26.V8H(), v27.V8H(), v28.V8H(), bad_memory);
  __ ld1(v3.V8H(), v4.V8H(), v5.V8H(), bad_memory);
  __ ld1(v17.V8H(), v18.V8H(), v19.V8H(), bad_memory);
  __ ld1(v4.V8H(), v5.V8H(), bad_memory);
  __ ld1(v21.V8H(), v22.V8H(), bad_memory);
  __ ld1(v4.V8H(), v5.V8H(), bad_memory);
  __ ld1(v9.V8H(), bad_memory);
  __ ld1(v27.V8H(), bad_memory);
  __ ld1(v26.V8H(), bad_memory);
  __ ld1(v19.B(), 1, bad_memory);
  __ ld1(v12.B(), 3, bad_memory);
  __ ld1(v27.B(), 12, bad_memory);
  __ ld1(v10.D(), 1, bad_memory);
  __ ld1(v26.D(), 1, bad_memory);
  __ ld1(v7.D(), 1, bad_memory);
  __ ld1(v19.H(), 5, bad_memory);
  __ ld1(v10.H(), 1, bad_memory);
  __ ld1(v5.H(), 4, bad_memory);
  __ ld1(v21.S(), 2, bad_memory);
  __ ld1(v13.S(), 2, bad_memory);
  __ ld1(v1.S(), 2, bad_memory);
  __ ld1r(v2.V16B(), bad_memory);
  __ ld1r(v2.V16B(), bad_memory);
  __ ld1r(v22.V16B(), bad_memory);
  __ ld1r(v25.V1D(), bad_memory);
  __ ld1r(v9.V1D(), bad_memory);
  __ ld1r(v23.V1D(), bad_memory);
  __ ld1r(v19.V2D(), bad_memory);
  __ ld1r(v21.V2D(), bad_memory);
  __ ld1r(v30.V2D(), bad_memory);
  __ ld1r(v24.V2S(), bad_memory);
  __ ld1r(v26.V2S(), bad_memory);
  __ ld1r(v28.V2S(), bad_memory);
  __ ld1r(v19.V4H(), bad_memory);
  __ ld1r(v1.V4H(), bad_memory);
  __ ld1r(v21.V4H(), bad_memory);
  __ ld1r(v15.V4S(), bad_memory);
  __ ld1r(v21.V4S(), bad_memory);
  __ ld1r(v23.V4S(), bad_memory);
  __ ld1r(v26.V8B(), bad_memory);
  __ ld1r(v14.V8B(), bad_memory);
  __ ld1r(v19.V8B(), bad_memory);
  __ ld1r(v13.V8H(), bad_memory);
  __ ld1r(v30.V8H(), bad_memory);
  __ ld1r(v27.V8H(), bad_memory);
  __ ld2(v21.V16B(), v22.V16B(), bad_memory);
  __ ld2(v21.V16B(), v22.V16B(), bad_memory);
  __ ld2(v12.V16B(), v13.V16B(), bad_memory);
  __ ld2(v14.V2D(), v15.V2D(), bad_memory);
  __ ld2(v0.V2D(), v1.V2D(), bad_memory);
  __ ld2(v12.V2D(), v13.V2D(), bad_memory);
  __ ld2(v27.V2S(), v28.V2S(), bad_memory);
  __ ld2(v2.V2S(), v3.V2S(), bad_memory);
  __ ld2(v12.V2S(), v13.V2S(), bad_memory);
  __ ld2(v9.V4H(), v10.V4H(), bad_memory);
  __ ld2(v23.V4H(), v24.V4H(), bad_memory);
  __ ld2(v1.V4H(), v2.V4H(), bad_memory);
  __ ld2(v20.V4S(), v21.V4S(), bad_memory);
  __ ld2(v10.V4S(), v11.V4S(), bad_memory);
  __ ld2(v24.V4S(), v25.V4S(), bad_memory);
  __ ld2(v17.V8B(), v18.V8B(), bad_memory);
  __ ld2(v13.V8B(), v14.V8B(), bad_memory);
  __ ld2(v7.V8B(), v8.V8B(), bad_memory);
  __ ld2(v30.V8H(), v31.V8H(), bad_memory);
  __ ld2(v4.V8H(), v5.V8H(), bad_memory);
  __ ld2(v13.V8H(), v14.V8H(), bad_memory);
  __ ld2(v5.B(), v6.B(), 12, bad_memory);
  __ ld2(v16.B(), v17.B(), 7, bad_memory);
  __ ld2(v29.B(), v30.B(), 2, bad_memory);
  __ ld2(v11.D(), v12.D(), 1, bad_memory);
  __ ld2(v26.D(), v27.D(), 0, bad_memory);
  __ ld2(v25.D(), v26.D(), 0, bad_memory);
  __ ld2(v18.H(), v19.H(), 7, bad_memory);
  __ ld2(v17.H(), v18.H(), 5, bad_memory);
  __ ld2(v30.H(), v31.H(), 2, bad_memory);
  __ ld2(v29.S(), v30.S(), 3, bad_memory);
  __ ld2(v28.S(), v29.S(), 0, bad_memory);
  __ ld2(v6.S(), v7.S(), 1, bad_memory);
  __ ld2r(v26.V16B(), v27.V16B(), bad_memory);
  __ ld2r(v21.V16B(), v22.V16B(), bad_memory);
  __ ld2r(v5.V16B(), v6.V16B(), bad_memory);
  __ ld2r(v26.V1D(), v27.V1D(), bad_memory);
  __ ld2r(v14.V1D(), v15.V1D(), bad_memory);
  __ ld2r(v23.V1D(), v24.V1D(), bad_memory);
  __ ld2r(v11.V2D(), v12.V2D(), bad_memory);
  __ ld2r(v29.V2D(), v30.V2D(), bad_memory);
  __ ld2r(v15.V2D(), v16.V2D(), bad_memory);
  __ ld2r(v26.V2S(), v27.V2S(), bad_memory);
  __ ld2r(v22.V2S(), v23.V2S(), bad_memory);
  __ ld2r(v2.V2S(), v3.V2S(), bad_memory);
  __ ld2r(v2.V4H(), v3.V4H(), bad_memory);
  __ ld2r(v9.V4H(), v10.V4H(), bad_memory);
  __ ld2r(v6.V4H(), v7.V4H(), bad_memory);
  __ ld2r(v7.V4S(), v8.V4S(), bad_memory);
  __ ld2r(v19.V4S(), v20.V4S(), bad_memory);
  __ ld2r(v21.V4S(), v22.V4S(), bad_memory);
  __ ld2r(v26.V8B(), v27.V8B(), bad_memory);
  __ ld2r(v20.V8B(), v21.V8B(), bad_memory);
  __ ld2r(v11.V8B(), v12.V8B(), bad_memory);
  __ ld2r(v12.V8H(), v13.V8H(), bad_memory);
  __ ld2r(v6.V8H(), v7.V8H(), bad_memory);
  __ ld2r(v25.V8H(), v26.V8H(), bad_memory);
  __ ld3(v20.V16B(), v21.V16B(), v22.V16B(), bad_memory);
  __ ld3(v28.V16B(), v29.V16B(), v30.V16B(), bad_memory);
  __ ld3(v20.V16B(), v21.V16B(), v22.V16B(), bad_memory);
  __ ld3(v21.V2D(), v22.V2D(), v23.V2D(), bad_memory);
  __ ld3(v18.V2D(), v19.V2D(), v20.V2D(), bad_memory);
  __ ld3(v27.V2D(), v28.V2D(), v29.V2D(), bad_memory);
  __ ld3(v7.V2S(), v8.V2S(), v9.V2S(), bad_memory);
  __ ld3(v20.V2S(), v21.V2S(), v22.V2S(), bad_memory);
  __ ld3(v26.V2S(), v27.V2S(), v28.V2S(), bad_memory);
  __ ld3(v27.V4H(), v28.V4H(), v29.V4H(), bad_memory);
  __ ld3(v28.V4H(), v29.V4H(), v30.V4H(), bad_memory);
  __ ld3(v7.V4H(), v8.V4H(), v9.V4H(), bad_memory);
  __ ld3(v2.V4S(), v3.V4S(), v4.V4S(), bad_memory);
  __ ld3(v24.V4S(), v25.V4S(), v26.V4S(), bad_memory);
  __ ld3(v11.V4S(), v12.V4S(), v13.V4S(), bad_memory);
  __ ld3(v29.V8B(), v30.V8B(), v31.V8B(), bad_memory);
  __ ld3(v1.V8B(), v2.V8B(), v3.V8B(), bad_memory);
  __ ld3(v12.V8B(), v13.V8B(), v14.V8B(), bad_memory);
  __ ld3(v22.V8H(), v23.V8H(), v24.V8H(), bad_memory);
  __ ld3(v13.V8H(), v14.V8H(), v15.V8H(), bad_memory);
  __ ld3(v28.V8H(), v29.V8H(), v30.V8H(), bad_memory);
  __ ld3(v21.B(), v22.B(), v23.B(), 11, bad_memory);
  __ ld3(v5.B(), v6.B(), v7.B(), 9, bad_memory);
  __ ld3(v23.B(), v24.B(), v25.B(), 0, bad_memory);
  __ ld3(v16.D(), v17.D(), v18.D(), 0, bad_memory);
  __ ld3(v30.D(), v31.D(), v0.D(), 0, bad_memory);
  __ ld3(v28.D(), v29.D(), v30.D(), 1, bad_memory);
  __ ld3(v13.H(), v14.H(), v15.H(), 2, bad_memory);
  __ ld3(v22.H(), v23.H(), v24.H(), 7, bad_memory);
  __ ld3(v14.H(), v15.H(), v16.H(), 3, bad_memory);
  __ ld3(v22.S(), v23.S(), v24.S(), 3, bad_memory);
  __ ld3(v30.S(), v31.S(), v0.S(), 2, bad_memory);
  __ ld3(v12.S(), v13.S(), v14.S(), 1, bad_memory);
  __ ld3r(v24.V16B(), v25.V16B(), v26.V16B(), bad_memory);
  __ ld3r(v24.V16B(), v25.V16B(), v26.V16B(), bad_memory);
  __ ld3r(v3.V16B(), v4.V16B(), v5.V16B(), bad_memory);
  __ ld3r(v4.V1D(), v5.V1D(), v6.V1D(), bad_memory);
  __ ld3r(v7.V1D(), v8.V1D(), v9.V1D(), bad_memory);
  __ ld3r(v17.V1D(), v18.V1D(), v19.V1D(), bad_memory);
  __ ld3r(v16.V2D(), v17.V2D(), v18.V2D(), bad_memory);
  __ ld3r(v20.V2D(), v21.V2D(), v22.V2D(), bad_memory);
  __ ld3r(v14.V2D(), v15.V2D(), v16.V2D(), bad_memory);
  __ ld3r(v10.V2S(), v11.V2S(), v12.V2S(), bad_memory);
  __ ld3r(v0.V2S(), v1.V2S(), v2.V2S(), bad_memory);
  __ ld3r(v23.V2S(), v24.V2S(), v25.V2S(), bad_memory);
  __ ld3r(v22.V4H(), v23.V4H(), v24.V4H(), bad_memory);
  __ ld3r(v6.V4H(), v7.V4H(), v8.V4H(), bad_memory);
  __ ld3r(v7.V4H(), v8.V4H(), v9.V4H(), bad_memory);
  __ ld3r(v26.V4S(), v27.V4S(), v28.V4S(), bad_memory);
  __ ld3r(v0.V4S(), v1.V4S(), v2.V4S(), bad_memory);
  __ ld3r(v30.V4S(), v31.V4S(), v0.V4S(), bad_memory);
  __ ld3r(v2.V8B(), v3.V8B(), v4.V8B(), bad_memory);
  __ ld3r(v10.V8B(), v11.V8B(), v12.V8B(), bad_memory);
  __ ld3r(v28.V8B(), v29.V8B(), v30.V8B(), bad_memory);
  __ ld3r(v6.V8H(), v7.V8H(), v8.V8H(), bad_memory);
  __ ld3r(v29.V8H(), v30.V8H(), v31.V8H(), bad_memory);
  __ ld3r(v7.V8H(), v8.V8H(), v9.V8H(), bad_memory);
  __ ld4(v3.V16B(), v4.V16B(), v5.V16B(), v6.V16B(), bad_memory);
  __ ld4(v2.V16B(), v3.V16B(), v4.V16B(), v5.V16B(), bad_memory);
  __ ld4(v5.V16B(), v6.V16B(), v7.V16B(), v8.V16B(), bad_memory);
  __ ld4(v18.V2D(), v19.V2D(), v20.V2D(), v21.V2D(), bad_memory);
  __ ld4(v4.V2D(), v5.V2D(), v6.V2D(), v7.V2D(), bad_memory);
  __ ld4(v29.V2D(), v30.V2D(), v31.V2D(), v0.V2D(), bad_memory);
  __ ld4(v27.V2S(), v28.V2S(), v29.V2S(), v30.V2S(), bad_memory);
  __ ld4(v24.V2S(), v25.V2S(), v26.V2S(), v27.V2S(), bad_memory);
  __ ld4(v4.V2S(), v5.V2S(), v6.V2S(), v7.V2S(), bad_memory);
  __ ld4(v16.V4H(), v17.V4H(), v18.V4H(), v19.V4H(), bad_memory);
  __ ld4(v23.V4H(), v24.V4H(), v25.V4H(), v26.V4H(), bad_memory);
  __ ld4(v2.V4H(), v3.V4H(), v4.V4H(), v5.V4H(), bad_memory);
  __ ld4(v7.V4S(), v8.V4S(), v9.V4S(), v10.V4S(), bad_memory);
  __ ld4(v28.V4S(), v29.V4S(), v30.V4S(), v31.V4S(), bad_memory);
  __ ld4(v29.V4S(), v30.V4S(), v31.V4S(), v0.V4S(), bad_memory);
  __ ld4(v15.V8B(), v16.V8B(), v17.V8B(), v18.V8B(), bad_memory);
  __ ld4(v27.V8B(), v28.V8B(), v29.V8B(), v30.V8B(), bad_memory);
  __ ld4(v5.V8B(), v6.V8B(), v7.V8B(), v8.V8B(), bad_memory);
  __ ld4(v25.V8H(), v26.V8H(), v27.V8H(), v28.V8H(), bad_memory);
  __ ld4(v2.V8H(), v3.V8H(), v4.V8H(), v5.V8H(), bad_memory);
  __ ld4(v20.V8H(), v21.V8H(), v22.V8H(), v23.V8H(), bad_memory);
  __ ld4(v20.B(), v21.B(), v22.B(), v23.B(), 3, bad_memory);
  __ ld4(v12.B(), v13.B(), v14.B(), v15.B(), 3, bad_memory);
  __ ld4(v27.B(), v28.B(), v29.B(), v30.B(), 6, bad_memory);
  __ ld4(v28.D(), v29.D(), v30.D(), v31.D(), 1, bad_memory);
  __ ld4(v15.D(), v16.D(), v17.D(), v18.D(), 1, bad_memory);
  __ ld4(v16.D(), v17.D(), v18.D(), v19.D(), 1, bad_memory);
  __ ld4(v2.H(), v3.H(), v4.H(), v5.H(), 6, bad_memory);
  __ ld4(v5.H(), v6.H(), v7.H(), v8.H(), 3, bad_memory);
  __ ld4(v7.H(), v8.H(), v9.H(), v10.H(), 6, bad_memory);
  __ ld4(v6.S(), v7.S(), v8.S(), v9.S(), 1, bad_memory);
  __ ld4(v25.S(), v26.S(), v27.S(), v28.S(), 2, bad_memory);
  __ ld4(v8.S(), v9.S(), v10.S(), v11.S(), 3, bad_memory);
  __ ld4r(v14.V16B(), v15.V16B(), v16.V16B(), v17.V16B(), bad_memory);
  __ ld4r(v13.V16B(), v14.V16B(), v15.V16B(), v16.V16B(), bad_memory);
  __ ld4r(v9.V16B(), v10.V16B(), v11.V16B(), v12.V16B(), bad_memory);
  __ ld4r(v8.V1D(), v9.V1D(), v10.V1D(), v11.V1D(), bad_memory);
  __ ld4r(v4.V1D(), v5.V1D(), v6.V1D(), v7.V1D(), bad_memory);
  __ ld4r(v26.V1D(), v27.V1D(), v28.V1D(), v29.V1D(), bad_memory);
  __ ld4r(v19.V2D(), v20.V2D(), v21.V2D(), v22.V2D(), bad_memory);
  __ ld4r(v28.V2D(), v29.V2D(), v30.V2D(), v31.V2D(), bad_memory);
  __ ld4r(v15.V2D(), v16.V2D(), v17.V2D(), v18.V2D(), bad_memory);
  __ ld4r(v31.V2S(), v0.V2S(), v1.V2S(), v2.V2S(), bad_memory);
  __ ld4r(v28.V2S(), v29.V2S(), v30.V2S(), v31.V2S(), bad_memory);
  __ ld4r(v11.V2S(), v12.V2S(), v13.V2S(), v14.V2S(), bad_memory);
  __ ld4r(v19.V4H(), v20.V4H(), v21.V4H(), v22.V4H(), bad_memory);
  __ ld4r(v22.V4H(), v23.V4H(), v24.V4H(), v25.V4H(), bad_memory);
  __ ld4r(v20.V4H(), v21.V4H(), v22.V4H(), v23.V4H(), bad_memory);
  __ ld4r(v16.V4S(), v17.V4S(), v18.V4S(), v19.V4S(), bad_memory);
  __ ld4r(v25.V4S(), v26.V4S(), v27.V4S(), v28.V4S(), bad_memory);
  __ ld4r(v23.V4S(), v24.V4S(), v25.V4S(), v26.V4S(), bad_memory);
  __ ld4r(v22.V8B(), v23.V8B(), v24.V8B(), v25.V8B(), bad_memory);
  __ ld4r(v27.V8B(), v28.V8B(), v29.V8B(), v30.V8B(), bad_memory);
  __ ld4r(v29.V8B(), v30.V8B(), v31.V8B(), v0.V8B(), bad_memory);
  __ ld4r(v28.V8H(), v29.V8H(), v30.V8H(), v31.V8H(), bad_memory);
  __ ld4r(v25.V8H(), v26.V8H(), v27.V8H(), v28.V8H(), bad_memory);
  __ ld4r(v22.V8H(), v23.V8H(), v24.V8H(), v25.V8H(), bad_memory);

  __ st1(v18.V16B(), v19.V16B(), v20.V16B(), v21.V16B(), bad_memory);
  __ st1(v10.V16B(), v11.V16B(), v12.V16B(), v13.V16B(), bad_memory);
  __ st1(v27.V16B(), v28.V16B(), v29.V16B(), v30.V16B(), bad_memory);
  __ st1(v16.V16B(), v17.V16B(), v18.V16B(), bad_memory);
  __ st1(v21.V16B(), v22.V16B(), v23.V16B(), bad_memory);
  __ st1(v9.V16B(), v10.V16B(), v11.V16B(), bad_memory);
  __ st1(v7.V16B(), v8.V16B(), bad_memory);
  __ st1(v26.V16B(), v27.V16B(), bad_memory);
  __ st1(v22.V16B(), v23.V16B(), bad_memory);
  __ st1(v23.V16B(), bad_memory);
  __ st1(v28.V16B(), bad_memory);
  __ st1(v2.V16B(), bad_memory);
  __ st1(v29.V1D(), v30.V1D(), v31.V1D(), v0.V1D(), bad_memory);
  __ st1(v12.V1D(), v13.V1D(), v14.V1D(), v15.V1D(), bad_memory);
  __ st1(v30.V1D(), v31.V1D(), v0.V1D(), v1.V1D(), bad_memory);
  __ st1(v16.V1D(), v17.V1D(), v18.V1D(), bad_memory);
  __ st1(v3.V1D(), v4.V1D(), v5.V1D(), bad_memory);
  __ st1(v14.V1D(), v15.V1D(), v16.V1D(), bad_memory);
  __ st1(v18.V1D(), v19.V1D(), bad_memory);
  __ st1(v5.V1D(), v6.V1D(), bad_memory);
  __ st1(v2.V1D(), v3.V1D(), bad_memory);
  __ st1(v4.V1D(), bad_memory);
  __ st1(v27.V1D(), bad_memory);
  __ st1(v23.V1D(), bad_memory);
  __ st1(v2.V2D(), v3.V2D(), v4.V2D(), v5.V2D(), bad_memory);
  __ st1(v22.V2D(), v23.V2D(), v24.V2D(), v25.V2D(), bad_memory);
  __ st1(v28.V2D(), v29.V2D(), v30.V2D(), v31.V2D(), bad_memory);
  __ st1(v17.V2D(), v18.V2D(), v19.V2D(), bad_memory);
  __ st1(v16.V2D(), v17.V2D(), v18.V2D(), bad_memory);
  __ st1(v22.V2D(), v23.V2D(), v24.V2D(), bad_memory);
  __ st1(v21.V2D(), v22.V2D(), bad_memory);
  __ st1(v6.V2D(), v7.V2D(), bad_memory);
  __ st1(v27.V2D(), v28.V2D(), bad_memory);
  __ st1(v21.V2D(), bad_memory);
  __ st1(v29.V2D(), bad_memory);
  __ st1(v20.V2D(), bad_memory);
  __ st1(v22.V2S(), v23.V2S(), v24.V2S(), v25.V2S(), bad_memory);
  __ st1(v8.V2S(), v9.V2S(), v10.V2S(), v11.V2S(), bad_memory);
  __ st1(v15.V2S(), v16.V2S(), v17.V2S(), v18.V2S(), bad_memory);
  __ st1(v2.V2S(), v3.V2S(), v4.V2S(), bad_memory);
  __ st1(v23.V2S(), v24.V2S(), v25.V2S(), bad_memory);
  __ st1(v7.V2S(), v8.V2S(), v9.V2S(), bad_memory);
  __ st1(v28.V2S(), v29.V2S(), bad_memory);
  __ st1(v29.V2S(), v30.V2S(), bad_memory);
  __ st1(v23.V2S(), v24.V2S(), bad_memory);
  __ st1(v6.V2S(), bad_memory);
  __ st1(v11.V2S(), bad_memory);
  __ st1(v17.V2S(), bad_memory);
  __ st1(v6.V4H(), v7.V4H(), v8.V4H(), v9.V4H(), bad_memory);
  __ st1(v9.V4H(), v10.V4H(), v11.V4H(), v12.V4H(), bad_memory);
  __ st1(v25.V4H(), v26.V4H(), v27.V4H(), v28.V4H(), bad_memory);
  __ st1(v11.V4H(), v12.V4H(), v13.V4H(), bad_memory);
  __ st1(v10.V4H(), v11.V4H(), v12.V4H(), bad_memory);
  __ st1(v12.V4H(), v13.V4H(), v14.V4H(), bad_memory);
  __ st1(v13.V4H(), v14.V4H(), bad_memory);
  __ st1(v15.V4H(), v16.V4H(), bad_memory);
  __ st1(v21.V4H(), v22.V4H(), bad_memory);
  __ st1(v16.V4H(), bad_memory);
  __ st1(v8.V4H(), bad_memory);
  __ st1(v30.V4H(), bad_memory);
  __ st1(v3.V4S(), v4.V4S(), v5.V4S(), v6.V4S(), bad_memory);
  __ st1(v25.V4S(), v26.V4S(), v27.V4S(), v28.V4S(), bad_memory);
  __ st1(v5.V4S(), v6.V4S(), v7.V4S(), v8.V4S(), bad_memory);
  __ st1(v31.V4S(), v0.V4S(), v1.V4S(), bad_memory);
  __ st1(v30.V4S(), v31.V4S(), v0.V4S(), bad_memory);
  __ st1(v6.V4S(), v7.V4S(), v8.V4S(), bad_memory);
  __ st1(v17.V4S(), v18.V4S(), bad_memory);
  __ st1(v31.V4S(), v0.V4S(), bad_memory);
  __ st1(v1.V4S(), v2.V4S(), bad_memory);
  __ st1(v26.V4S(), bad_memory);
  __ st1(v15.V4S(), bad_memory);
  __ st1(v13.V4S(), bad_memory);
  __ st1(v26.V8B(), v27.V8B(), v28.V8B(), v29.V8B(), bad_memory);
  __ st1(v10.V8B(), v11.V8B(), v12.V8B(), v13.V8B(), bad_memory);
  __ st1(v15.V8B(), v16.V8B(), v17.V8B(), v18.V8B(), bad_memory);
  __ st1(v19.V8B(), v20.V8B(), v21.V8B(), bad_memory);
  __ st1(v31.V8B(), v0.V8B(), v1.V8B(), bad_memory);
  __ st1(v9.V8B(), v10.V8B(), v11.V8B(), bad_memory);
  __ st1(v12.V8B(), v13.V8B(), bad_memory);
  __ st1(v2.V8B(), v3.V8B(), bad_memory);
  __ st1(v0.V8B(), v1.V8B(), bad_memory);
  __ st1(v16.V8B(), bad_memory);
  __ st1(v25.V8B(), bad_memory);
  __ st1(v31.V8B(), bad_memory);
  __ st1(v4.V8H(), v5.V8H(), v6.V8H(), v7.V8H(), bad_memory);
  __ st1(v3.V8H(), v4.V8H(), v5.V8H(), v6.V8H(), bad_memory);
  __ st1(v26.V8H(), v27.V8H(), v28.V8H(), v29.V8H(), bad_memory);
  __ st1(v10.V8H(), v11.V8H(), v12.V8H(), bad_memory);
  __ st1(v21.V8H(), v22.V8H(), v23.V8H(), bad_memory);
  __ st1(v18.V8H(), v19.V8H(), v20.V8H(), bad_memory);
  __ st1(v26.V8H(), v27.V8H(), bad_memory);
  __ st1(v24.V8H(), v25.V8H(), bad_memory);
  __ st1(v17.V8H(), v18.V8H(), bad_memory);
  __ st1(v29.V8H(), bad_memory);
  __ st1(v19.V8H(), bad_memory);
  __ st1(v23.V8H(), bad_memory);
  __ st1(v19.B(), 15, bad_memory);
  __ st1(v25.B(), 9, bad_memory);
  __ st1(v4.B(), 8, bad_memory);
  __ st1(v13.D(), 0, bad_memory);
  __ st1(v30.D(), 0, bad_memory);
  __ st1(v3.D(), 0, bad_memory);
  __ st1(v22.H(), 0, bad_memory);
  __ st1(v31.H(), 7, bad_memory);
  __ st1(v23.H(), 3, bad_memory);
  __ st1(v0.S(), 0, bad_memory);
  __ st1(v11.S(), 3, bad_memory);
  __ st1(v24.S(), 3, bad_memory);
  __ st2(v7.V16B(), v8.V16B(), bad_memory);
  __ st2(v5.V16B(), v6.V16B(), bad_memory);
  __ st2(v18.V16B(), v19.V16B(), bad_memory);
  __ st2(v14.V2D(), v15.V2D(), bad_memory);
  __ st2(v7.V2D(), v8.V2D(), bad_memory);
  __ st2(v24.V2D(), v25.V2D(), bad_memory);
  __ st2(v22.V2S(), v23.V2S(), bad_memory);
  __ st2(v4.V2S(), v5.V2S(), bad_memory);
  __ st2(v2.V2S(), v3.V2S(), bad_memory);
  __ st2(v23.V4H(), v24.V4H(), bad_memory);
  __ st2(v8.V4H(), v9.V4H(), bad_memory);
  __ st2(v7.V4H(), v8.V4H(), bad_memory);
  __ st2(v17.V4S(), v18.V4S(), bad_memory);
  __ st2(v6.V4S(), v7.V4S(), bad_memory);
  __ st2(v26.V4S(), v27.V4S(), bad_memory);
  __ st2(v31.V8B(), v0.V8B(), bad_memory);
  __ st2(v0.V8B(), v1.V8B(), bad_memory);
  __ st2(v21.V8B(), v22.V8B(), bad_memory);
  __ st2(v7.V8H(), v8.V8H(), bad_memory);
  __ st2(v22.V8H(), v23.V8H(), bad_memory);
  __ st2(v4.V8H(), v5.V8H(), bad_memory);
  __ st2(v8.B(), v9.B(), 15, bad_memory);
  __ st2(v8.B(), v9.B(), 15, bad_memory);
  __ st2(v7.B(), v8.B(), 4, bad_memory);
  __ st2(v25.D(), v26.D(), 0, bad_memory);
  __ st2(v17.D(), v18.D(), 1, bad_memory);
  __ st2(v3.D(), v4.D(), 1, bad_memory);
  __ st2(v4.H(), v5.H(), 3, bad_memory);
  __ st2(v0.H(), v1.H(), 5, bad_memory);
  __ st2(v22.H(), v23.H(), 2, bad_memory);
  __ st2(v14.S(), v15.S(), 3, bad_memory);
  __ st2(v23.S(), v24.S(), 3, bad_memory);
  __ st2(v0.S(), v1.S(), 2, bad_memory);
  __ st3(v26.V16B(), v27.V16B(), v28.V16B(), bad_memory);
  __ st3(v21.V16B(), v22.V16B(), v23.V16B(), bad_memory);
  __ st3(v24.V16B(), v25.V16B(), v26.V16B(), bad_memory);
  __ st3(v17.V2D(), v18.V2D(), v19.V2D(), bad_memory);
  __ st3(v23.V2D(), v24.V2D(), v25.V2D(), bad_memory);
  __ st3(v10.V2D(), v11.V2D(), v12.V2D(), bad_memory);
  __ st3(v9.V2S(), v10.V2S(), v11.V2S(), bad_memory);
  __ st3(v13.V2S(), v14.V2S(), v15.V2S(), bad_memory);
  __ st3(v22.V2S(), v23.V2S(), v24.V2S(), bad_memory);
  __ st3(v31.V4H(), v0.V4H(), v1.V4H(), bad_memory);
  __ st3(v8.V4H(), v9.V4H(), v10.V4H(), bad_memory);
  __ st3(v19.V4H(), v20.V4H(), v21.V4H(), bad_memory);
  __ st3(v18.V4S(), v19.V4S(), v20.V4S(), bad_memory);
  __ st3(v25.V4S(), v26.V4S(), v27.V4S(), bad_memory);
  __ st3(v16.V4S(), v17.V4S(), v18.V4S(), bad_memory);
  __ st3(v27.V8B(), v28.V8B(), v29.V8B(), bad_memory);
  __ st3(v29.V8B(), v30.V8B(), v31.V8B(), bad_memory);
  __ st3(v30.V8B(), v31.V8B(), v0.V8B(), bad_memory);
  __ st3(v8.V8H(), v9.V8H(), v10.V8H(), bad_memory);
  __ st3(v18.V8H(), v19.V8H(), v20.V8H(), bad_memory);
  __ st3(v18.V8H(), v19.V8H(), v20.V8H(), bad_memory);
  __ st3(v31.B(), v0.B(), v1.B(), 10, bad_memory);
  __ st3(v4.B(), v5.B(), v6.B(), 5, bad_memory);
  __ st3(v5.B(), v6.B(), v7.B(), 1, bad_memory);
  __ st3(v5.D(), v6.D(), v7.D(), 0, bad_memory);
  __ st3(v6.D(), v7.D(), v8.D(), 0, bad_memory);
  __ st3(v0.D(), v1.D(), v2.D(), 0, bad_memory);
  __ st3(v31.H(), v0.H(), v1.H(), 2, bad_memory);
  __ st3(v14.H(), v15.H(), v16.H(), 5, bad_memory);
  __ st3(v21.H(), v22.H(), v23.H(), 6, bad_memory);
  __ st3(v21.S(), v22.S(), v23.S(), 0, bad_memory);
  __ st3(v11.S(), v12.S(), v13.S(), 1, bad_memory);
  __ st3(v15.S(), v16.S(), v17.S(), 0, bad_memory);
  __ st4(v22.V16B(), v23.V16B(), v24.V16B(), v25.V16B(), bad_memory);
  __ st4(v24.V16B(), v25.V16B(), v26.V16B(), v27.V16B(), bad_memory);
  __ st4(v15.V16B(), v16.V16B(), v17.V16B(), v18.V16B(), bad_memory);
  __ st4(v16.V2D(), v17.V2D(), v18.V2D(), v19.V2D(), bad_memory);
  __ st4(v17.V2D(), v18.V2D(), v19.V2D(), v20.V2D(), bad_memory);
  __ st4(v9.V2D(), v10.V2D(), v11.V2D(), v12.V2D(), bad_memory);
  __ st4(v23.V2S(), v24.V2S(), v25.V2S(), v26.V2S(), bad_memory);
  __ st4(v15.V2S(), v16.V2S(), v17.V2S(), v18.V2S(), bad_memory);
  __ st4(v24.V2S(), v25.V2S(), v26.V2S(), v27.V2S(), bad_memory);
  __ st4(v14.V4H(), v15.V4H(), v16.V4H(), v17.V4H(), bad_memory);
  __ st4(v18.V4H(), v19.V4H(), v20.V4H(), v21.V4H(), bad_memory);
  __ st4(v1.V4H(), v2.V4H(), v3.V4H(), v4.V4H(), bad_memory);
  __ st4(v13.V4S(), v14.V4S(), v15.V4S(), v16.V4S(), bad_memory);
  __ st4(v6.V4S(), v7.V4S(), v8.V4S(), v9.V4S(), bad_memory);
  __ st4(v15.V4S(), v16.V4S(), v17.V4S(), v18.V4S(), bad_memory);
  __ st4(v26.V8B(), v27.V8B(), v28.V8B(), v29.V8B(), bad_memory);
  __ st4(v25.V8B(), v26.V8B(), v27.V8B(), v28.V8B(), bad_memory);
  __ st4(v19.V8B(), v20.V8B(), v21.V8B(), v22.V8B(), bad_memory);
  __ st4(v19.V8H(), v20.V8H(), v21.V8H(), v22.V8H(), bad_memory);
  __ st4(v15.V8H(), v16.V8H(), v17.V8H(), v18.V8H(), bad_memory);
  __ st4(v31.V8H(), v0.V8H(), v1.V8H(), v2.V8H(), bad_memory);
  __ st4(v0.B(), v1.B(), v2.B(), v3.B(), 13, bad_memory);
  __ st4(v4.B(), v5.B(), v6.B(), v7.B(), 10, bad_memory);
  __ st4(v9.B(), v10.B(), v11.B(), v12.B(), 9, bad_memory);
  __ st4(v2.D(), v3.D(), v4.D(), v5.D(), 1, bad_memory);
  __ st4(v7.D(), v8.D(), v9.D(), v10.D(), 0, bad_memory);
  __ st4(v31.D(), v0.D(), v1.D(), v2.D(), 1, bad_memory);
  __ st4(v2.H(), v3.H(), v4.H(), v5.H(), 1, bad_memory);
  __ st4(v27.H(), v28.H(), v29.H(), v30.H(), 3, bad_memory);
  __ st4(v24.H(), v25.H(), v26.H(), v27.H(), 4, bad_memory);
  __ st4(v18.S(), v19.S(), v20.S(), v21.S(), 2, bad_memory);
  __ st4(v6.S(), v7.S(), v8.S(), v9.S(), 2, bad_memory);
  __ st4(v25.S(), v26.S(), v27.S(), v28.S(), 1, bad_memory);

  END_IMPLICIT_CHECK();
  TRY_RUN_IMPLICIT_CHECK();
}

TEST(ImplicitCheckSve) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                      CPUFeatures::kSVE2,
                      CPUFeatures::kNEON);
  START_IMPLICIT_CHECK();

  SVEMemOperand bad_sve_memory = SVEMemOperand(ip0);

  EmissionCheckScope guard(&masm, masm.GetBuffer()->GetRemainingBytes());
  // Simple, unpredicated loads and stores.
  __ Str(p12.VnD(), bad_sve_memory);
  __ Str(p13.VnS(), bad_sve_memory);
  __ Str(p14.VnH(), bad_sve_memory);
  __ Str(p15.VnB(), bad_sve_memory);
  __ Ldr(p8.VnD(), bad_sve_memory);
  __ Ldr(p9.VnS(), bad_sve_memory);
  __ Ldr(p10.VnH(), bad_sve_memory);
  __ Ldr(p11.VnB(), bad_sve_memory);

  __ Str(z0.VnD(), bad_sve_memory);
  __ Str(z1.VnS(), bad_sve_memory);
  __ Str(z2.VnH(), bad_sve_memory);
  __ Str(z3.VnB(), bad_sve_memory);
  __ Ldr(z20.VnD(), bad_sve_memory);
  __ Ldr(z21.VnS(), bad_sve_memory);
  __ Ldr(z22.VnH(), bad_sve_memory);
  __ Ldr(z23.VnB(), bad_sve_memory);

  // Structured accesses.
  __ St1b(z0.VnB(), p2, bad_sve_memory);
  __ St1h(z1.VnH(), p1, bad_sve_memory);
  __ St1w(z2.VnS(), p1, bad_sve_memory);
  __ St1d(z3.VnD(), p2, bad_sve_memory);
  __ Ld1b(z20.VnB(), p1.Zeroing(), bad_sve_memory);
  __ Ld1h(z21.VnH(), p2.Zeroing(), bad_sve_memory);
  __ Ld1w(z22.VnS(), p1.Zeroing(), bad_sve_memory);
  __ Ld1d(z23.VnD(), p1.Zeroing(), bad_sve_memory);

  // Structured, packed accesses.
  __ St1b(z2.VnH(), p1, bad_sve_memory);
  __ St1b(z3.VnS(), p2, bad_sve_memory);
  __ St1b(z4.VnD(), p2, bad_sve_memory);
  __ St1h(z0.VnS(), p1, bad_sve_memory);
  __ St1h(z1.VnD(), p1, bad_sve_memory);
  __ St1w(z2.VnD(), p1, bad_sve_memory);
  __ Ld1b(z20.VnH(), p1.Zeroing(), bad_sve_memory);
  __ Ld1b(z21.VnS(), p1.Zeroing(), bad_sve_memory);
  __ Ld1b(z22.VnD(), p1.Zeroing(), bad_sve_memory);
  __ Ld1h(z23.VnS(), p2.Zeroing(), bad_sve_memory);
  __ Ld1h(z24.VnD(), p2.Zeroing(), bad_sve_memory);
  __ Ld1w(z20.VnD(), p1.Zeroing(), bad_sve_memory);
  __ Ld1sb(z21.VnH(), p1.Zeroing(), bad_sve_memory);
  __ Ld1sb(z22.VnS(), p1.Zeroing(), bad_sve_memory);
  __ Ld1sb(z23.VnD(), p2.Zeroing(), bad_sve_memory);
  __ Ld1sh(z24.VnS(), p2.Zeroing(), bad_sve_memory);
  __ Ld1sh(z20.VnD(), p1.Zeroing(), bad_sve_memory);
  __ Ld1sw(z21.VnD(), p1.Zeroing(), bad_sve_memory);

  // Structured, interleaved accesses.
  __ St2b(z0.VnB(), z1.VnB(), p4, bad_sve_memory);
  __ St2h(z1.VnH(), z2.VnH(), p4, bad_sve_memory);
  __ St2w(z2.VnS(), z3.VnS(), p3, bad_sve_memory);
  __ St2d(z3.VnD(), z4.VnD(), p4, bad_sve_memory);
  __ Ld2b(z20.VnB(), z21.VnB(), p5.Zeroing(), bad_sve_memory);
  __ Ld2h(z21.VnH(), z22.VnH(), p6.Zeroing(), bad_sve_memory);
  __ Ld2w(z22.VnS(), z23.VnS(), p6.Zeroing(), bad_sve_memory);
  __ Ld2d(z23.VnD(), z24.VnD(), p5.Zeroing(), bad_sve_memory);

  __ St3b(z4.VnB(), z5.VnB(), z6.VnB(), p4, bad_sve_memory);
  __ St3h(z5.VnH(), z6.VnH(), z7.VnH(), p4, bad_sve_memory);
  __ St3w(z6.VnS(), z7.VnS(), z8.VnS(), p3, bad_sve_memory);
  __ St3d(z7.VnD(), z8.VnD(), z9.VnD(), p4, bad_sve_memory);
  __ Ld3b(z24.VnB(), z25.VnB(), z26.VnB(), p5.Zeroing(), bad_sve_memory);
  __ Ld3h(z25.VnH(), z26.VnH(), z27.VnH(), p6.Zeroing(), bad_sve_memory);
  __ Ld3w(z26.VnS(), z27.VnS(), z28.VnS(), p6.Zeroing(), bad_sve_memory);
  __ Ld3d(z27.VnD(), z28.VnD(), z29.VnD(), p5.Zeroing(), bad_sve_memory);

  __ St4b(z31.VnB(), z0.VnB(), z1.VnB(), z2.VnB(), p4, bad_sve_memory);
  __ St4h(z0.VnH(), z1.VnH(), z2.VnH(), z3.VnH(), p4, bad_sve_memory);
  __ St4w(z1.VnS(), z2.VnS(), z3.VnS(), z4.VnS(), p3, bad_sve_memory);
  __ St4d(z2.VnD(), z3.VnD(), z4.VnD(), z5.VnD(), p4, bad_sve_memory);
  __ Ld4b(z25.VnB(),
          z26.VnB(),
          z27.VnB(),
          z28.VnB(),
          p5.Zeroing(),
          bad_sve_memory);
  __ Ld4h(z26.VnH(),
          z27.VnH(),
          z28.VnH(),
          z29.VnH(),
          p6.Zeroing(),
          bad_sve_memory);
  __ Ld4w(z27.VnS(),
          z28.VnS(),
          z29.VnS(),
          z30.VnS(),
          p6.Zeroing(),
          bad_sve_memory);
  __ Ld4d(z28.VnD(),
          z29.VnD(),
          z30.VnD(),
          z31.VnD(),
          p5.Zeroing(),
          bad_sve_memory);

  END_IMPLICIT_CHECK();
  TRY_RUN_IMPLICIT_CHECK();
}

TEST(ImplicitCheckAtomics) {
  SETUP_WITH_FEATURES(CPUFeatures::kNEON, CPUFeatures::kAtomics);
  START_IMPLICIT_CHECK();

  EmissionCheckScope guard(&masm, masm.GetBuffer()->GetRemainingBytes());
#define INST_LIST(OP)                 \
  __ Ld##OP##b(w0, w0, bad_memory);   \
  __ Ld##OP##ab(w0, w1, bad_memory);  \
  __ Ld##OP##lb(w0, w2, bad_memory);  \
  __ Ld##OP##alb(w0, w3, bad_memory); \
  __ Ld##OP##h(w0, w0, bad_memory);   \
  __ Ld##OP##ah(w0, w1, bad_memory);  \
  __ Ld##OP##lh(w0, w2, bad_memory);  \
  __ Ld##OP##alh(w0, w3, bad_memory); \
  __ Ld##OP(w0, w0, bad_memory);      \
  __ Ld##OP##a(w0, w1, bad_memory);   \
  __ Ld##OP##l(w0, w2, bad_memory);   \
  __ Ld##OP##al(w0, w3, bad_memory);  \
  __ Ld##OP(x0, x0, bad_memory);      \
  __ Ld##OP##a(x0, x1, bad_memory);   \
  __ Ld##OP##l(x0, x2, bad_memory);   \
  __ Ld##OP##al(x0, x3, bad_memory);  \
  __ St##OP##b(w0, bad_memory);       \
  __ St##OP##lb(w0, bad_memory);      \
  __ St##OP##h(w0, bad_memory);       \
  __ St##OP##lh(w0, bad_memory);      \
  __ St##OP(w0, bad_memory);          \
  __ St##OP##l(w0, bad_memory);       \
  __ St##OP(x0, bad_memory);          \
  __ St##OP##l(x0, bad_memory);

  INST_LIST(add);
  INST_LIST(set);
  INST_LIST(eor);
  INST_LIST(smin);
  INST_LIST(smax);
  INST_LIST(umin);
  INST_LIST(umax);
  INST_LIST(clr);

#undef INST_LIST

  END_IMPLICIT_CHECK();
  TRY_RUN_IMPLICIT_CHECK();
}

TEST(ImplicitCheckMops) {
  SETUP_WITH_FEATURES(CPUFeatures::kNEON, CPUFeatures::kMOPS);
  START_IMPLICIT_CHECK();

  EmissionCheckScope guard(&masm, masm.GetBuffer()->GetRemainingBytes());
  __ Set(x15, ip1, ip0);
  __ Setn(x15, ip1, ip0);
  __ Setg(x15, ip1, ip0);
  __ Setgn(x15, ip1, ip0);

  __ Cpy(x15, ip0, ip1);
  __ Cpyn(x15, ip0, ip1);
  __ Cpyrn(x15, ip0, ip1);
  __ Cpywn(x15, ip0, ip1);
  __ Cpyf(x15, ip0, ip1);
  __ Cpyfn(x15, ip0, ip1);
  __ Cpyfrn(x15, ip0, ip1);
  __ Cpyfwn(x15, ip0, ip1);

  // The macro-assembler expands each instruction into prologue, main and
  // epilogue instructions where only the main instruction will fail. Increase
  // the counter to account for those additional instructions and the following
  // instructions.
  __ Mov(x0, 3);
  __ Mul(x1, x1, x0);
  __ Add(x1, x1, x0);

  END_IMPLICIT_CHECK();
  TRY_RUN_IMPLICIT_CHECK();
}
#endif  // VIXL_ENABLE_IMPLICIT_CHECKS

#undef __
#define __ masm->

#if defined(VIXL_INCLUDE_SIMULATOR_AARCH64) &&                 \
    defined(VIXL_HAS_ABI_SUPPORT) && __cplusplus >= 201103L && \
    (defined(__clang__) || GCC_VERSION_OR_NEWER(4, 9, 1))

// Generate a function that stores zero to a hard-coded address.
Instruction* GenerateStoreZero(MacroAssembler* masm, int32_t* target) {
  masm->Reset();

  UseScratchRegisterScope temps(masm);
  Register temp = temps.AcquireX();
  __ Mov(temp, reinterpret_cast<intptr_t>(target));
  __ Str(wzr, MemOperand(temp));
  __ Ret();

  masm->FinalizeCode();
  return masm->GetBuffer()->GetStartAddress<Instruction*>();
}


// Generate a function that stores the `int32_t` argument to a hard-coded
// address.
// In this example and the other below, we use the `abi` object to retrieve
// argument and return locations even though we could easily hard code them.
// This mirrors how more generic code (e.g. templated) user would use these
// mechanisms.
Instruction* GenerateStoreInput(MacroAssembler* masm, int32_t* target) {
  masm->Reset();

  ABI abi;
  Register input =
      Register(abi.GetNextParameterGenericOperand<int32_t>().GetCPURegister());

  UseScratchRegisterScope temps(masm);
  Register temp = temps.AcquireX();
  __ Mov(temp, reinterpret_cast<intptr_t>(target));
  __ Str(input, MemOperand(temp));
  __ Ret();

  masm->FinalizeCode();
  return masm->GetBuffer()->GetStartAddress<Instruction*>();
}


// A minimal implementation of a `pow` function.
Instruction* GeneratePow(MacroAssembler* masm, unsigned pow) {
  masm->Reset();

  ABI abi;
  Register input =
      Register(abi.GetNextParameterGenericOperand<int64_t>().GetCPURegister());
  Register result =
      Register(abi.GetReturnGenericOperand<int64_t>().GetCPURegister());
  UseScratchRegisterScope temps(masm);
  Register temp = temps.AcquireX();

  __ Mov(temp, 1);
  for (unsigned i = 0; i < pow; i++) {
    __ Mul(temp, temp, input);
  }
  __ Mov(result, temp);
  __ Ret();

  masm->FinalizeCode();
  return masm->GetBuffer()->GetStartAddress<Instruction*>();
}


Instruction* GenerateSum(MacroAssembler* masm) {
  masm->Reset();

  ABI abi;
  VRegister input_1 =
      VRegister(abi.GetNextParameterGenericOperand<float>().GetCPURegister());
  Register input_2 =
      Register(abi.GetNextParameterGenericOperand<int64_t>().GetCPURegister());
  VRegister input_3 =
      VRegister(abi.GetNextParameterGenericOperand<double>().GetCPURegister());
  VRegister result =
      VRegister(abi.GetReturnGenericOperand<double>().GetCPURegister());

  UseScratchRegisterScope temps(masm);
  VRegister temp = temps.AcquireD();

  __ Fcvt(input_1.D(), input_1);
  __ Scvtf(temp, input_2);
  __ Fadd(temp, temp, input_1.D());
  __ Fadd(result, temp, input_3);
  __ Ret();

  masm->FinalizeCode();
  return masm->GetBuffer()->GetStartAddress<Instruction*>();
}


TEST(RunFrom) {
  SETUP_WITH_FEATURES(CPUFeatures::kFP);

  // Run a function returning `void` and taking no argument.
  int32_t value = 0xbad;
  simulator.RunFrom(GenerateStoreZero(&masm, &value));
  VIXL_CHECK(value == 0);

  // Run a function returning `void` and taking one argument.
  int32_t argument = 0xf00d;
  simulator.RunFrom<void, int32_t>(GenerateStoreInput(&masm, &value), argument);
  VIXL_CHECK(value == 0xf00d);

  // Run a function taking one argument and returning a value.
  int64_t res_int64_t;
  res_int64_t =
      simulator.RunFrom<int64_t, int64_t>(GeneratePow(&masm, 0), 0xbad);
  VIXL_CHECK(res_int64_t == 1);
  res_int64_t = simulator.RunFrom<int64_t, int64_t>(GeneratePow(&masm, 1), 123);
  VIXL_CHECK(res_int64_t == 123);
  res_int64_t = simulator.RunFrom<int64_t, int64_t>(GeneratePow(&masm, 10), 2);
  VIXL_CHECK(res_int64_t == 1024);

  // Run a function taking multiple arguments in registers.
  double res_double =
      simulator.RunFrom<double, float, int64_t, double>(GenerateSum(&masm),
                                                        1.0,
                                                        2,
                                                        3.0);
  VIXL_CHECK(res_double == 6.0);
}

#endif


}  // namespace aarch64
}  // namespace vixl
