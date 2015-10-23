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
#include <float.h>

#include "test-runner.h"
#include "test-utils-a64.h"
#include "test-simulator-inputs-a64.h"
#include "test-simulator-traces-a64.h"
#include "vixl/a64/macro-assembler-a64.h"
#include "vixl/a64/simulator-a64.h"

namespace vixl {

// ==== Simulator Tests ====
//
// These simulator tests check instruction behaviour against a trace taken from
// real AArch64 hardware. The same test code is used to generate the trace; the
// results are printed to stdout when the test is run with --sim_test_trace.
//
// The input lists and expected results are stored in test/traces. The expected
// results can be regenerated using tools/generate_simulator_traces.py. Adding
// a test for a new instruction is described at the top of
// test-simulator-traces-a64.h.

#define __ masm.
#define TEST(name)  TEST_(SIM_##name)

#define BUF_SIZE (256)

#ifdef VIXL_INCLUDE_SIMULATOR

#define SETUP()                                                               \
  MacroAssembler masm(BUF_SIZE);                                              \
  Decoder decoder;                                                            \
  Simulator* simulator = Test::run_debugger() ? new Debugger(&decoder)        \
                                              : new Simulator(&decoder);      \
  simulator->set_coloured_trace(Test::coloured_trace());                      \
  simulator->set_instruction_stats(Test::instruction_stats());                \

#define START()                                                               \
  masm.Reset();                                                               \
  simulator->ResetState();                                                    \
  __ PushCalleeSavedRegisters();                                              \
  if (Test::trace_reg()) {                                                    \
    __ Trace(LOG_STATE, TRACE_ENABLE);                                        \
  }                                                                           \
  if (Test::trace_write()) {                                                  \
    __ Trace(LOG_WRITE, TRACE_ENABLE);                                        \
  }                                                                           \
  if (Test::trace_sim()) {                                                    \
    __ Trace(LOG_DISASM, TRACE_ENABLE);                                       \
  }                                                                           \
  if (Test::instruction_stats()) {                                            \
    __ EnableInstrumentation();                                               \
  }

#define END()                                                                 \
  if (Test::instruction_stats()) {                                            \
    __ DisableInstrumentation();                                              \
  }                                                                           \
  __ Trace(LOG_ALL, TRACE_DISABLE);                                           \
  __ PopCalleeSavedRegisters();                                               \
  __ Ret();                                                                   \
  masm.FinalizeCode()

#define RUN()                                                                 \
  simulator->RunFrom(masm.GetStartAddress<Instruction*>())

#define TEARDOWN()                                                            \
  delete simulator;

#else     // VIXL_INCLUDE_SIMULATOR

#define SETUP()                                                               \
  MacroAssembler masm(BUF_SIZE);                                              \
  CPU::SetUp()

#define START()                                                               \
  masm.Reset();                                                               \
  __ PushCalleeSavedRegisters()

#define END()                                                                 \
  __ PopCalleeSavedRegisters();                                               \
  __ Ret();                                                                   \
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

#endif    // VIXL_INCLUDE_SIMULATOR


// The maximum number of errors to report in detail for each test.
static const unsigned kErrorReportLimit = 8;


// Overloaded versions of rawbits_to_double and rawbits_to_float for use in the
// templated test functions.
static float rawbits_to_fp(uint32_t bits) {
  return rawbits_to_float(bits);
}

static double rawbits_to_fp(uint64_t bits) {
  return rawbits_to_double(bits);
}


// MacroAssembler member function pointers to pass to the test dispatchers.
typedef void (MacroAssembler::*Test1OpFPHelper_t)(const FPRegister& fd,
                                                  const FPRegister& fn);
typedef void (MacroAssembler::*Test2OpFPHelper_t)(const FPRegister& fd,
                                                  const FPRegister& fn,
                                                  const FPRegister& fm);
typedef void (MacroAssembler::*Test3OpFPHelper_t)(const FPRegister& fd,
                                                  const FPRegister& fn,
                                                  const FPRegister& fm,
                                                  const FPRegister& fa);
typedef void (MacroAssembler::*TestFPCmpHelper_t)(const FPRegister& fn,
                                                  const FPRegister& fm);
typedef void (MacroAssembler::*TestFPCmpZeroHelper_t)(const FPRegister& fn,
                                                      double value);
typedef void (MacroAssembler::*TestFPToIntHelper_t)(const Register& rd,
                                                    const FPRegister& fn);
typedef void (MacroAssembler::*TestFPToFixedHelper_t)(const Register& rd,
                                                      const FPRegister& fn,
                                                      int fbits);
typedef void (MacroAssembler::*TestFixedToFPHelper_t)(const FPRegister& fd,
                                                      const Register& rn,
                                                      int fbits);
// TODO: 'Test2OpNEONHelper_t' and 'Test2OpFPHelper_t' can be
//       consolidated into one routine.
typedef void (MacroAssembler::*Test1OpNEONHelper_t)(
  const VRegister& vd, const VRegister& vn);
typedef void (MacroAssembler::*Test2OpNEONHelper_t)(
  const VRegister& vd, const VRegister& vn, const VRegister& vm);
typedef void (MacroAssembler::*TestByElementNEONHelper_t)(
  const VRegister& vd, const VRegister& vn, const VRegister& vm, int vm_index);
typedef void (MacroAssembler::*TestOpImmOpImmVdUpdateNEONHelper_t)(
  const VRegister& vd, int imm1, const VRegister& vn, int imm2);

// This helps using the same typename for both the function pointer
// and the array of immediates passed to helper routines.
template <typename T>
class Test2OpImmediateNEONHelper_t {
 public:
    typedef void (MacroAssembler::*mnemonic)(
      const VRegister& vd, const VRegister& vn, T imm);
};


// Maximum number of hex characters required to represent values of either
// templated type.
template <typename Ta, typename Tb>
static unsigned MaxHexCharCount() {
  unsigned count = static_cast<unsigned>(std::max(sizeof(Ta), sizeof(Tb)));
  return (count * 8) / 4;
}


// Standard test dispatchers.


static void Test1Op_Helper(Test1OpFPHelper_t helper, uintptr_t inputs,
                           unsigned inputs_length, uintptr_t results,
                           unsigned d_size, unsigned n_size) {
  VIXL_ASSERT((d_size == kDRegSize) || (d_size == kSRegSize));
  VIXL_ASSERT((n_size == kDRegSize) || (n_size == kSRegSize));

  SETUP();
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n;

  Register out = x0;
  Register inputs_base = x1;
  Register length = w2;
  Register index_n = w3;

  const int n_index_shift =
      (n_size == kDRegSize) ? kDRegSizeInBytesLog2 : kSRegSizeInBytesLog2;

  FPRegister fd = (d_size == kDRegSize) ? d0 : s0;
  FPRegister fn = (n_size == kDRegSize) ? d1 : s1;

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
  __ Str(fd, MemOperand(out, fd.SizeInBytes(), PostIndex));

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_length);
  __ B(lo, &loop_n);

  END();
  RUN();
  TEARDOWN();
}


// Test FP instructions. The inputs[] and expected[] arrays should be arrays of
// rawbits representations of doubles or floats. This ensures that exact bit
// comparisons can be performed.
template <typename Tn, typename Td>
static void Test1Op(const char * name, Test1OpFPHelper_t helper,
                    const Tn inputs[], unsigned inputs_length,
                    const Td expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length;
  Td * results = new Td[results_length];

  const unsigned d_bits = sizeof(Td) * 8;
  const unsigned n_bits = sizeof(Tn) * 8;

  Test1Op_Helper(helper, reinterpret_cast<uintptr_t>(inputs), inputs_length,
                 reinterpret_cast<uintptr_t>(results), d_bits, n_bits);

  if (Test::sim_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_%s[] = {\n", d_bits, name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  0x%0*" PRIx64 ",\n",
             d_bits / 4, static_cast<uint64_t>(results[d]));
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++, d++) {
      if (results[d] != expected[d]) {
        if (++error_count > kErrorReportLimit) continue;

        printf("%s 0x%0*" PRIx64 " (%s %g):\n",
               name, n_bits / 4, static_cast<uint64_t>(inputs[n]),
               name, rawbits_to_fp(inputs[n]));
        printf("  Expected: 0x%0*" PRIx64 " (%g)\n",
               d_bits / 4, static_cast<uint64_t>(expected[d]),
               rawbits_to_fp(expected[d]));
        printf("  Found:    0x%0*" PRIx64 " (%g)\n",
               d_bits / 4, static_cast<uint64_t>(results[d]),
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
                           uintptr_t inputs, unsigned inputs_length,
                           uintptr_t results, unsigned reg_size) {
  VIXL_ASSERT((reg_size == kDRegSize) || (reg_size == kSRegSize));

  SETUP();
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n, loop_m;

  Register out = x0;
  Register inputs_base = x1;
  Register length = w2;
  Register index_n = w3;
  Register index_m = w4;

  bool double_op = reg_size == kDRegSize;
  const int index_shift =
      double_op ? kDRegSizeInBytesLog2 : kSRegSizeInBytesLog2;

  FPRegister fd = double_op ? d0 : s0;
  FPRegister fn = double_op ? d1 : s1;
  FPRegister fm = double_op ? d2 : s2;

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
    __ Str(fd, MemOperand(out, fd.SizeInBytes(), PostIndex));

  __ Add(index_m, index_m, 1);
  __ Cmp(index_m, inputs_length);
  __ B(lo, &loop_m);

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_length);
  __ B(lo, &loop_n);

  END();
  RUN();
  TEARDOWN();
}


// Test FP instructions. The inputs[] and expected[] arrays should be arrays of
// rawbits representations of doubles or floats. This ensures that exact bit
// comparisons can be performed.
template <typename T>
static void Test2Op(const char * name, Test2OpFPHelper_t helper,
                    const T inputs[], unsigned inputs_length,
                    const T expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length * inputs_length;
  T * results = new T[results_length];

  const unsigned bits = sizeof(T) * 8;

  Test2Op_Helper(helper, reinterpret_cast<uintptr_t>(inputs), inputs_length,
                 reinterpret_cast<uintptr_t>(results), bits);

  if (Test::sim_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_%s[] = {\n", bits, name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  0x%0*" PRIx64 ",\n",
             bits / 4, static_cast<uint64_t>(results[d]));
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else {
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
                 bits / 4, static_cast<uint64_t>(inputs[n]),
                 bits / 4, static_cast<uint64_t>(inputs[m]),
                 name,
                 rawbits_to_fp(inputs[n]),
                 rawbits_to_fp(inputs[m]));
          printf("  Expected: 0x%0*" PRIx64 " (%g)\n",
                 bits / 4, static_cast<uint64_t>(expected[d]),
                 rawbits_to_fp(expected[d]));
          printf("  Found:    0x%0*" PRIx64 " (%g)\n",
                 bits / 4, static_cast<uint64_t>(results[d]),
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
                           uintptr_t inputs, unsigned inputs_length,
                           uintptr_t results, unsigned reg_size) {
  VIXL_ASSERT((reg_size == kDRegSize) || (reg_size == kSRegSize));

  SETUP();
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
  const int index_shift =
      double_op ? kDRegSizeInBytesLog2 : kSRegSizeInBytesLog2;

  FPRegister fd = double_op ? d0 : s0;
  FPRegister fn = double_op ? d1 : s1;
  FPRegister fm = double_op ? d2 : s2;
  FPRegister fa = double_op ? d3 : s3;

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
  __ Str(fd, MemOperand(out, fd.SizeInBytes(), PostIndex));

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
  RUN();
  TEARDOWN();
}


// Test FP instructions. The inputs[] and expected[] arrays should be arrays of
// rawbits representations of doubles or floats. This ensures that exact bit
// comparisons can be performed.
template <typename T>
static void Test3Op(const char * name, Test3OpFPHelper_t helper,
                    const T inputs[], unsigned inputs_length,
                    const T expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length * inputs_length * inputs_length;
  T * results = new T[results_length];

  const unsigned bits = sizeof(T) * 8;

  Test3Op_Helper(helper, reinterpret_cast<uintptr_t>(inputs), inputs_length,
                 reinterpret_cast<uintptr_t>(results), bits);

  if (Test::sim_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_%s[] = {\n", bits, name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  0x%0*" PRIx64 ",\n",
             bits / 4, static_cast<uint64_t>(results[d]));
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else {
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
                   bits / 4, static_cast<uint64_t>(inputs[n]),
                   bits / 4, static_cast<uint64_t>(inputs[m]),
                   bits / 4, static_cast<uint64_t>(inputs[a]),
                   name,
                   rawbits_to_fp(inputs[n]),
                   rawbits_to_fp(inputs[m]),
                   rawbits_to_fp(inputs[a]));
            printf("  Expected: 0x%0*" PRIx64 " (%g)\n",
                   bits / 4, static_cast<uint64_t>(expected[d]),
                   rawbits_to_fp(expected[d]));
            printf("  Found:    0x%0*" PRIx64 " (%g)\n",
                   bits / 4, static_cast<uint64_t>(results[d]),
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
                           uintptr_t inputs, unsigned inputs_length,
                           uintptr_t results, unsigned reg_size) {
  VIXL_ASSERT((reg_size == kDRegSize) || (reg_size == kSRegSize));

  SETUP();
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

  FPRegister fn = double_op ? d1 : s1;
  FPRegister fm = double_op ? d2 : s2;

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
  RUN();
  TEARDOWN();
}


// Test FP instructions. The inputs[] and expected[] arrays should be arrays of
// rawbits representations of doubles or floats. This ensures that exact bit
// comparisons can be performed.
template <typename T>
static void TestCmp(const char * name, TestFPCmpHelper_t helper,
                    const T inputs[], unsigned inputs_length,
                    const uint8_t expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length * inputs_length;
  uint8_t * results = new uint8_t[results_length];

  const unsigned bits = sizeof(T) * 8;

  TestCmp_Helper(helper, reinterpret_cast<uintptr_t>(inputs), inputs_length,
                 reinterpret_cast<uintptr_t>(results), bits);

  if (Test::sim_test_trace()) {
    // Print the results.
    printf("const uint8_t kExpected_%s[] = {\n", name);
    for (unsigned d = 0; d < results_length; d++) {
      // Each NZCV result only requires 4 bits.
      VIXL_ASSERT((results[d] & 0xf) == results[d]);
      printf("  0x%" PRIx8 ",\n", results[d]);
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else {
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
                 bits / 4, static_cast<uint64_t>(inputs[n]),
                 bits / 4, static_cast<uint64_t>(inputs[m]),
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
                               uintptr_t inputs, unsigned inputs_length,
                               uintptr_t results, unsigned reg_size) {
  VIXL_ASSERT((reg_size == kDRegSize) || (reg_size == kSRegSize));

  SETUP();
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

  FPRegister fn = double_op ? d1 : s1;

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
  RUN();
  TEARDOWN();
}


// Test FP instructions. The inputs[] and expected[] arrays should be arrays of
// rawbits representations of doubles or floats. This ensures that exact bit
// comparisons can be performed.
template <typename T>
static void TestCmpZero(const char * name, TestFPCmpZeroHelper_t helper,
                        const T inputs[], unsigned inputs_length,
                        const uint8_t expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length;
  uint8_t * results = new uint8_t[results_length];

  const unsigned bits = sizeof(T) * 8;

  TestCmpZero_Helper(helper, reinterpret_cast<uintptr_t>(inputs), inputs_length,
                     reinterpret_cast<uintptr_t>(results), bits);

  if (Test::sim_test_trace()) {
    // Print the results.
    printf("const uint8_t kExpected_%s[] = {\n", name);
    for (unsigned d = 0; d < results_length; d++) {
      // Each NZCV result only requires 4 bits.
      VIXL_ASSERT((results[d] & 0xf) == results[d]);
      printf("  0x%" PRIx8 ",\n", results[d]);
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++, d++) {
      if (results[d] != expected[d]) {
        if (++error_count > kErrorReportLimit) continue;

        printf("%s 0x%0*" PRIx64 ", 0x%0*u (%s %g #0.0):\n",
               name,
               bits / 4, static_cast<uint64_t>(inputs[n]),
               bits / 4, 0,
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
                                 uintptr_t inputs, unsigned inputs_length,
                                 uintptr_t results,
                                 unsigned d_size, unsigned n_size) {
  VIXL_ASSERT((d_size == kXRegSize) || (d_size == kWRegSize));
  VIXL_ASSERT((n_size == kDRegSize) || (n_size == kSRegSize));

  SETUP();
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n;

  Register out = x0;
  Register inputs_base = x1;
  Register length = w2;
  Register index_n = w3;

  const int n_index_shift =
      (n_size == kDRegSize) ? kDRegSizeInBytesLog2 : kSRegSizeInBytesLog2;

  Register rd = (d_size == kXRegSize) ? x10 : w10;
  FPRegister fn = (n_size == kDRegSize) ? d1 : s1;

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
    __ Str(rd, MemOperand(out, rd.SizeInBytes(), PostIndex));
  }

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_length);
  __ B(lo, &loop_n);

  END();
  RUN();
  TEARDOWN();
}


static void TestFPToInt_Helper(TestFPToIntHelper_t helper, uintptr_t inputs,
                               unsigned inputs_length, uintptr_t results,
                               unsigned d_size, unsigned n_size) {
  VIXL_ASSERT((d_size == kXRegSize) || (d_size == kWRegSize));
  VIXL_ASSERT((n_size == kDRegSize) || (n_size == kSRegSize));

  SETUP();
  START();

  // Roll up the loop to keep the code size down.
  Label loop_n;

  Register out = x0;
  Register inputs_base = x1;
  Register length = w2;
  Register index_n = w3;

  const int n_index_shift =
      (n_size == kDRegSize) ? kDRegSizeInBytesLog2 : kSRegSizeInBytesLog2;

  Register rd = (d_size == kXRegSize) ? x10 : w10;
  FPRegister fn = (n_size == kDRegSize) ? d1 : s1;

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
  __ Str(rd, MemOperand(out, rd.SizeInBytes(), PostIndex));

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_length);
  __ B(lo, &loop_n);

  END();
  RUN();
  TEARDOWN();
}


// Test FP instructions.
//  - The inputs[] array should be an array of rawbits representations of
//    doubles or floats. This ensures that exact bit comparisons can be
//    performed.
//  - The expected[] array should be an array of signed integers.
template <typename Tn, typename Td>
static void TestFPToS(const char * name, TestFPToIntHelper_t helper,
                      const Tn inputs[], unsigned inputs_length,
                      const Td expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length;
  Td * results = new Td[results_length];

  const unsigned d_bits = sizeof(Td) * 8;
  const unsigned n_bits = sizeof(Tn) * 8;

  TestFPToInt_Helper(helper, reinterpret_cast<uintptr_t>(inputs), inputs_length,
                     reinterpret_cast<uintptr_t>(results), d_bits, n_bits);

  if (Test::sim_test_trace()) {
    // Print the results.
    printf("const int%u_t kExpected_%s[] = {\n", d_bits, name);
    // There is no simple C++ literal for INT*_MIN that doesn't produce
    // warnings, so we use an appropriate constant in that case instead.
    // Deriving int_d_min in this way (rather than just checking INT64_MIN and
    // the like) avoids warnings about comparing values with differing ranges.
    const int64_t int_d_max = (UINT64_C(1) << (d_bits - 1)) - 1;
    const int64_t int_d_min = -(int_d_max) - 1;
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
  } else {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++, d++) {
      if (results[d] != expected[d]) {
        if (++error_count > kErrorReportLimit) continue;

        printf("%s 0x%0*" PRIx64 " (%s %g):\n",
               name, n_bits / 4, static_cast<uint64_t>(inputs[n]),
               name, rawbits_to_fp(inputs[n]));
        printf("  Expected: 0x%0*" PRIx64 " (%" PRId64 ")\n",
               d_bits / 4, static_cast<uint64_t>(expected[d]),
               static_cast<int64_t>(expected[d]));
        printf("  Found:    0x%0*" PRIx64 " (%" PRId64 ")\n",
               d_bits / 4, static_cast<uint64_t>(results[d]),
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
static void TestFPToU(const char * name, TestFPToIntHelper_t helper,
                      const Tn inputs[], unsigned inputs_length,
                      const Td expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned results_length = inputs_length;
  Td * results = new Td[results_length];

  const unsigned d_bits = sizeof(Td) * 8;
  const unsigned n_bits = sizeof(Tn) * 8;

  TestFPToInt_Helper(helper,
                     reinterpret_cast<uintptr_t>(inputs), inputs_length,
                     reinterpret_cast<uintptr_t>(results), d_bits, n_bits);

  if (Test::sim_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_%s[] = {\n", d_bits, name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  %" PRIu64 "u,\n", static_cast<uint64_t>(results[d]));
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++, d++) {
      if (results[d] != expected[d]) {
        if (++error_count > kErrorReportLimit) continue;

        printf("%s 0x%0*" PRIx64 " (%s %g):\n",
               name, n_bits / 4, static_cast<uint64_t>(inputs[n]),
               name, rawbits_to_fp(inputs[n]));
        printf("  Expected: 0x%0*" PRIx64 " (%" PRIu64 ")\n",
               d_bits / 4, static_cast<uint64_t>(expected[d]),
               static_cast<uint64_t>(expected[d]));
        printf("  Found:    0x%0*" PRIx64 " (%" PRIu64 ")\n",
               d_bits / 4, static_cast<uint64_t>(results[d]),
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
static void TestFPToFixedS(const char * name, TestFPToFixedHelper_t helper,
                           const Tn inputs[], unsigned inputs_length,
                           const Td expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned d_bits = sizeof(Td) * 8;
  const unsigned n_bits = sizeof(Tn) * 8;

  const unsigned results_length = inputs_length * (d_bits + 1);
  Td * results = new Td[results_length];

  TestFPToFixed_Helper(helper,
                       reinterpret_cast<uintptr_t>(inputs), inputs_length,
                       reinterpret_cast<uintptr_t>(results), d_bits, n_bits);

  if (Test::sim_test_trace()) {
    // Print the results.
    printf("const int%u_t kExpected_%s[] = {\n", d_bits, name);
    // There is no simple C++ literal for INT*_MIN that doesn't produce
    // warnings, so we use an appropriate constant in that case instead.
    // Deriving int_d_min in this way (rather than just checking INT64_MIN and
    // the like) avoids warnings about comparing values with differing ranges.
    const int64_t int_d_max = (UINT64_C(1) << (d_bits - 1)) - 1;
    const int64_t int_d_min = -(int_d_max) - 1;
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
  } else {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++) {
      for (unsigned fbits = 0; fbits <= d_bits; ++fbits, d++) {
        if (results[d] != expected[d]) {
          if (++error_count > kErrorReportLimit) continue;

          printf("%s 0x%0*" PRIx64 " #%d (%s %g #%d):\n",
                 name, n_bits / 4, static_cast<uint64_t>(inputs[n]), fbits,
                 name, rawbits_to_fp(inputs[n]), fbits);
          printf("  Expected: 0x%0*" PRIx64 " (%" PRId64 ")\n",
                 d_bits / 4, static_cast<uint64_t>(expected[d]),
                 static_cast<int64_t>(expected[d]));
          printf("  Found:    0x%0*" PRIx64 " (%" PRId64 ")\n",
                 d_bits / 4, static_cast<uint64_t>(results[d]),
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
static void TestFPToFixedU(const char * name, TestFPToFixedHelper_t helper,
                           const Tn inputs[], unsigned inputs_length,
                           const Td expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  const unsigned d_bits = sizeof(Td) * 8;
  const unsigned n_bits = sizeof(Tn) * 8;

  const unsigned results_length = inputs_length * (d_bits + 1);
  Td * results = new Td[results_length];

  TestFPToFixed_Helper(helper,
                       reinterpret_cast<uintptr_t>(inputs), inputs_length,
                       reinterpret_cast<uintptr_t>(results), d_bits, n_bits);

  if (Test::sim_test_trace()) {
    // Print the results.
    printf("const uint%u_t kExpected_%s[] = {\n", d_bits, name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  %" PRIu64 "u,\n", static_cast<uint64_t>(results[d]));
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s = %u;\n", name, results_length);
  } else {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++) {
      for (unsigned fbits = 0; fbits <= d_bits; ++fbits, d++) {
        if (results[d] != expected[d]) {
          if (++error_count > kErrorReportLimit) continue;

          printf("%s 0x%0*" PRIx64 " #%d (%s %g #%d):\n",
                 name, n_bits / 4, static_cast<uint64_t>(inputs[n]), fbits,
                 name, rawbits_to_fp(inputs[n]), fbits);
          printf("  Expected: 0x%0*" PRIx64 " (%" PRIu64 ")\n",
                 d_bits / 4, static_cast<uint64_t>(expected[d]),
                 static_cast<uint64_t>(expected[d]));
          printf("  Found:    0x%0*" PRIx64 " (%" PRIu64 ")\n",
                 d_bits / 4, static_cast<uint64_t>(results[d]),
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
                               uintptr_t inputs_n, unsigned inputs_n_length,
                               uintptr_t results,
                               VectorFormat vd_form,
                               VectorFormat vn_form) {
  VIXL_ASSERT(vd_form != kFormatUndefined);
  VIXL_ASSERT(vn_form != kFormatUndefined);

  SETUP();
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

  __ Ldr(vntmp_single, MemOperand(inputs_n_base, index_n, LSL,
                                  vn_lane_bytes_log2));
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
  __ Str(vd, MemOperand(out, vd.SizeInBytes(), PostIndex));

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_n_length);
  __ B(lo, &loop_n);

  END();
  RUN();
  TEARDOWN();
}


// Test NEON instructions. The inputs_*[] and expected[] arrays should be
// arrays of rawbit representation of input values. This ensures that
// exact bit comparisons can be performed.
template <typename Td, typename Tn>
static void Test1OpNEON(const char * name, Test1OpNEONHelper_t helper,
                        const Tn inputs_n[], unsigned inputs_n_length,
                        const Td expected[], unsigned expected_length,
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

  Test1OpNEON_Helper(helper,
                     reinterpret_cast<uintptr_t>(inputs_n),
                     inputs_n_length,
                     reinterpret_cast<uintptr_t>(results),
                     vd_form, vn_form);

  if (Test::sim_test_trace()) {
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
  } else {
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
                lane_len_in_hex+1, padding,
                lane_len_in_hex+1, padding);

        const unsigned first_index_n =
          inputs_n_length - (16 / vn_lane_bytes) + n + 1;

        for (unsigned lane = 0;
             lane < std::max(vd_lane_count, vn_lane_count);
             lane++) {
          unsigned output_index = (n * vd_lane_count) + lane;
          unsigned input_index_n = (first_index_n + lane) % inputs_n_length;

          printf("%c0x%0*" PRIx64 " | 0x%0*" PRIx64 " "
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
                                     VectorFormat vn_form) {
  VIXL_ASSERT(vd_form != kFormatUndefined);
  VIXL_ASSERT(vn_form != kFormatUndefined);

  SETUP();
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


  // These will be either a D- or a Q-register form, with a single lane
  // (for use in scalar load and store operations).
  VRegister vd = VRegister(0, vd_bits);
  VRegister vn = VRegister(1, vn_bits);
  VRegister vntmp = VRegister(3, vn_bits);

  // These will have the correct format for use when calling 'helper'.
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

  __ Ldr(vntmp_single, MemOperand(inputs_n_base, index_n, LSL,
                                  vn_lane_bytes_log2));
  __ Ext(vn_ext, vn_ext, vntmp_ext, vn_lane_bytes);

  // Set the destination to zero for tests such as '[r]shrn2'.
  // TODO: Setting the destination to values other than zero
  //       might be a better test for instructions such as sqxtn2
  //       which may leave parts of V registers unchanged.
  __ Movi(vd.V16B(), 0);

  {
    SingleEmissionCheckScope guard(&masm);
    (masm.*helper)(vd, vn_helper);
  }
  __ Str(vd, MemOperand(out, vd.SizeInBytes(), PostIndex));

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_n_length);
  __ B(lo, &loop_n);

  END();
  RUN();
  TEARDOWN();
}

// Test NEON instructions. The inputs_*[] and expected[] arrays should be
// arrays of rawbit representation of input values. This ensures that
// exact bit comparisons can be performed.
template <typename Td, typename Tn>
static void Test1OpAcrossNEON(const char * name, Test1OpNEONHelper_t helper,
                              const Tn inputs_n[], unsigned inputs_n_length,
                              const Td expected[], unsigned expected_length,
                              VectorFormat vd_form,
                              VectorFormat vn_form) {
  VIXL_ASSERT(inputs_n_length > 0);

  const unsigned vd_lane_count = LaneCountFromFormat(vd_form);

  const unsigned results_length = inputs_n_length;
  Td* results = new Td[results_length * vd_lane_count];
  const unsigned lane_bit = sizeof(Td) * 8;
  const unsigned lane_len_in_hex = MaxHexCharCount<Td, Tn>();

  Test1OpAcrossNEON_Helper(helper,
                           reinterpret_cast<uintptr_t>(inputs_n),
                           inputs_n_length,
                           reinterpret_cast<uintptr_t>(results),
                           vd_form, vn_form);

  if (Test::sim_test_trace()) {
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
  } else {
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
        const unsigned vn_lane_count = LaneCountFromFormat(vn_form);

        printf("%s\n", name);
        printf(" Vn%.*s| Vd%.*s| Expected\n",
                lane_len_in_hex+1, padding,
                lane_len_in_hex+1, padding);

        // TODO: In case of an error, all tests print out as many elements as
        //       there are lanes in the output or input vectors. This way
        //       the viewer can read all the values that were needed for the
        //       operation but the output contains also unnecessary values.
        //       These prints can be improved according to the arguments
        //       passed to test functions.
        //       This output for the 'Across' category has the required
        //       modifications.
        for (unsigned lane = 0; lane < vn_lane_count; lane++) {
          unsigned output_index = n * vd_lane_count;
          unsigned input_index_n = (inputs_n_length - vn_lane_count +
              n + 1 + lane) % inputs_n_length;

          if (vn_lane_count-1 == lane) {  // Is this the last lane?
            // Print the result element(s) in the last lane only.
            printf("%c0x%0*" PRIx64 " | 0x%0*" PRIx64 " "
                  "| 0x%0*" PRIx64 "\n",
                  results[output_index] != expected[output_index] ? '*' : ' ',
                  lane_len_in_hex,
                  static_cast<uint64_t>(inputs_n[input_index_n]),
                  lane_len_in_hex,
                  static_cast<uint64_t>(results[output_index]),
                  lane_len_in_hex,
                  static_cast<uint64_t>(expected[output_index]));
          } else {
            printf(" 0x%0*" PRIx64 " |   %.*s|   %.*s\n",
                  lane_len_in_hex,
                  static_cast<uint64_t>(inputs_n[input_index_n]),
                  lane_len_in_hex+1, padding,
                  lane_len_in_hex+1, padding);
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


// ==== Tests for instructions of the form <INST> VReg, VReg, VReg. ====

// TODO: Iterate over inputs_d once the traces file is split.

static void Test2OpNEON_Helper(Test2OpNEONHelper_t helper,
                               uintptr_t inputs_d,
                               uintptr_t inputs_n, unsigned inputs_n_length,
                               uintptr_t inputs_m, unsigned inputs_m_length,
                               uintptr_t results,
                               VectorFormat vd_form,
                               VectorFormat vn_form,
                               VectorFormat vm_form) {
  VIXL_ASSERT(vd_form != kFormatUndefined);
  VIXL_ASSERT(vn_form != kFormatUndefined);
  VIXL_ASSERT(vm_form != kFormatUndefined);

  SETUP();
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

  __ Ldr(vntmp_single, MemOperand(inputs_n_base, index_n, LSL,
                                  vn_lane_bytes_log2));
  __ Ext(vn, vn, vntmp, vn_lane_bytes);

  __ Mov(index_m, 0);
  __ Bind(&loop_m);

  __ Ldr(vmtmp_single, MemOperand(inputs_m_base, index_m, LSL,
                                  vm_lane_bytes_log2));
  __ Ext(vm, vm, vmtmp, vm_lane_bytes);

  __ Mov(vres, vd);
  {
    SingleEmissionCheckScope guard(&masm);
    (masm.*helper)(vres_helper, vn_helper, vm_helper);
  }
  __ Str(vres, MemOperand(out, vd.SizeInBytes(), PostIndex));

  __ Add(index_m, index_m, 1);
  __ Cmp(index_m, inputs_m_length);
  __ B(lo, &loop_m);

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_n_length);
  __ B(lo, &loop_n);

  END();
  RUN();
  TEARDOWN();
}


// Test NEON instructions. The inputs_*[] and expected[] arrays should be
// arrays of rawbit representation of input values. This ensures that
// exact bit comparisons can be performed.
template <typename Td, typename Tn, typename Tm>
static void Test2OpNEON(const char * name, Test2OpNEONHelper_t helper,
                        const Td inputs_d[],
                        const Tn inputs_n[], unsigned inputs_n_length,
                        const Tm inputs_m[], unsigned inputs_m_length,
                        const Td expected[], unsigned expected_length,
                        VectorFormat vd_form,
                        VectorFormat vn_form,
                        VectorFormat vm_form) {
  VIXL_ASSERT(inputs_n_length > 0 && inputs_m_length > 0);

  const unsigned vd_lane_count = MaxLaneCountFromFormat(vd_form);

  const unsigned results_length = inputs_n_length * inputs_m_length;
  Td* results = new Td[results_length * vd_lane_count];
  const unsigned lane_bit = sizeof(Td) * 8;
  const unsigned lane_len_in_hex = MaxHexCharCount<Td, Tm>();

  Test2OpNEON_Helper(helper,
                     reinterpret_cast<uintptr_t>(inputs_d),
                     reinterpret_cast<uintptr_t>(inputs_n), inputs_n_length,
                     reinterpret_cast<uintptr_t>(inputs_m), inputs_m_length,
                     reinterpret_cast<uintptr_t>(results),
                     vd_form, vn_form, vm_form);

  if (Test::sim_test_trace()) {
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
  } else {
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
                 lane_len_in_hex+1, padding,
                 lane_len_in_hex+1, padding,
                 lane_len_in_hex+1, padding,
                 lane_len_in_hex+1, padding);

          for (unsigned lane = 0; lane < vd_lane_count; lane++) {
            unsigned output_index = (n * inputs_m_length * vd_lane_count) +
                (m * vd_lane_count) + lane;
            unsigned input_index_n = (inputs_n_length - vd_lane_count +
                n + 1 + lane) % inputs_n_length;
            unsigned input_index_m = (inputs_m_length - vd_lane_count +
                m + 1 + lane) % inputs_m_length;

            printf("%c0x%0*" PRIx64 " | 0x%0*" PRIx64 " | 0x%0*" PRIx64 " "
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
                                     VectorFormat vm_form) {
  VIXL_ASSERT(vd_form != kFormatUndefined);
  VIXL_ASSERT(vn_form != kFormatUndefined);
  VIXL_ASSERT(vm_form != kFormatUndefined);

  SETUP();
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

  __ Ldr(vntmp_single, MemOperand(inputs_n_base, index_n, LSL,
                                  vn_lane_bytes_log2));
  __ Ext(vn, vn, vntmp, vn_lane_bytes);

  __ Mov(index_m, 0);
  __ Bind(&loop_m);

  __ Ldr(vmtmp_single, MemOperand(inputs_m_base, index_m, LSL,
                                  vm_lane_bytes_log2));
  __ Ext(vm, vm, vmtmp, vm_lane_bytes);

  __ Mov(vres, vd);
  {
    for (unsigned i = 0; i < indices_length; i++) {
      {
        SingleEmissionCheckScope guard(&masm);
        (masm.*helper)(vres_helper, vn_helper, vm_helper, indices[i]);
      }
      __ Str(vres, MemOperand(out, vd.SizeInBytes(), PostIndex));
    }
  }

  __ Add(index_m, index_m, 1);
  __ Cmp(index_m, inputs_m_length);
  __ B(lo, &loop_m);

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_n_length);
  __ B(lo, &loop_n);

  END();
  RUN();
  TEARDOWN();
}



// Test NEON instructions. The inputs_*[] and expected[] arrays should be
// arrays of rawbit representation of input values. This ensures that
// exact bit comparisons can be performed.
template <typename Td, typename Tn, typename Tm>
static void TestByElementNEON(const char *name,
                              TestByElementNEONHelper_t helper,
                              const Td inputs_d[],
                              const Tn inputs_n[], unsigned inputs_n_length,
                              const Tm inputs_m[], unsigned inputs_m_length,
                              const int indices[], unsigned indices_length,
                              const Td expected[], unsigned expected_length,
                              VectorFormat vd_form,
                              VectorFormat vn_form,
                              VectorFormat vm_form) {
  VIXL_ASSERT(inputs_n_length > 0);
  VIXL_ASSERT(inputs_m_length > 0);
  VIXL_ASSERT(indices_length > 0);

  const unsigned vd_lane_count = MaxLaneCountFromFormat(vd_form);

  const unsigned results_length = inputs_n_length * inputs_m_length *
                                  indices_length;
  Td* results = new Td[results_length * vd_lane_count];
  const unsigned lane_bit = sizeof(Td) * 8;
  const unsigned lane_len_in_hex = MaxHexCharCount<Td, Tm>();

  TestByElementNEON_Helper(helper,
    reinterpret_cast<uintptr_t>(inputs_d),
    reinterpret_cast<uintptr_t>(inputs_n), inputs_n_length,
    reinterpret_cast<uintptr_t>(inputs_m), inputs_m_length,
    indices, indices_length,
    reinterpret_cast<uintptr_t>(results),
    vd_form, vn_form, vm_form);

  if (Test::sim_test_trace()) {
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
  } else {
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
                (m * indices_length * vd_lane_count) +
                (index * vd_lane_count) + lane;

            if (results[output_index] != expected[output_index]) {
              error_in_vector = true;
              break;
            }
          }

          if (error_in_vector && (++error_count <= kErrorReportLimit)) {
            printf("%s\n", name);
            printf(" Vd%.*s| Vn%.*s| Vm%.*s| Index | Vd%.*s| Expected\n",
                  lane_len_in_hex+1, padding,
                  lane_len_in_hex+1, padding,
                  lane_len_in_hex+1, padding,
                  lane_len_in_hex+1, padding);

            for (unsigned lane = 0; lane < vd_lane_count; lane++) {
              unsigned output_index =
                  (n * inputs_m_length * indices_length * vd_lane_count) +
                  (m * indices_length * vd_lane_count) +
                  (index * vd_lane_count) + lane;
              unsigned input_index_n = (inputs_n_length - vd_lane_count +
                  n + 1 + lane) % inputs_n_length;
              unsigned input_index_m = (inputs_m_length - vd_lane_count +
                  m + 1 + lane) % inputs_m_length;

              printf("%c0x%0*" PRIx64 " | 0x%0*" PRIx64 " | 0x%0*" PRIx64 " "
                "| [%3d] | 0x%0*" PRIx64 " | 0x%0*" PRIx64 "\n",
                results[output_index] != expected[output_index] ? '*' : ' ',
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
    VectorFormat vn_form) {
  VIXL_ASSERT(vd_form != kFormatUndefined &&
              vn_form != kFormatUndefined);

  SETUP();
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

  __ Ldr(vntmp_single, MemOperand(inputs_n_base, index_n, LSL,
                                  vn_lane_bytes_log2));
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
      __ Str(vd, MemOperand(out, vd.SizeInBytes(), PostIndex));
    }
  }

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_n_length);
  __ B(lo, &loop_n);

  END();
  RUN();
  TEARDOWN();
}


// Test NEON instructions. The inputs_*[] and expected[] arrays should be
// arrays of rawbit representation of input values. This ensures that
// exact bit comparisons can be performed.
template <typename Td, typename Tn, typename Tm>
static void Test2OpImmNEON(
    const char * name,
    typename Test2OpImmediateNEONHelper_t<Tm>::mnemonic helper,
    const Tn inputs_n[], unsigned inputs_n_length,
    const Tm inputs_m[], unsigned inputs_m_length,
    const Td expected[], unsigned expected_length,
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

  Test2OpImmNEON_Helper(helper,
                        reinterpret_cast<uintptr_t>(inputs_n), inputs_n_length,
                        inputs_m, inputs_m_length,
                        reinterpret_cast<uintptr_t>(results),
                        vd_form, vn_form);

  if (Test::sim_test_trace()) {
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
  } else {
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
                 lane_len_in_hex+1, padding,
                 lane_len_in_hex, padding,
                 lane_len_in_hex+1, padding);

        const unsigned first_index_n =
          inputs_n_length - (16 / vn_lane_bytes) + n + 1;

        for (unsigned lane = 0;
             lane < std::max(vd_lane_count, vn_lane_count);
             lane++) {
            unsigned output_index = (n * inputs_m_length * vd_lane_count) +
                (m * vd_lane_count) + lane;
            unsigned input_index_n = (first_index_n + lane) % inputs_n_length;
            unsigned input_index_m = m;

            printf("%c0x%0*" PRIx64 " | 0x%0*" PRIx64 " "
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


static void TestOpImmOpImmNEON_Helper(
  TestOpImmOpImmVdUpdateNEONHelper_t helper,
  uintptr_t inputs_d,
  const int inputs_imm1[], unsigned inputs_imm1_length,
  uintptr_t inputs_n, unsigned inputs_n_length,
  const int inputs_imm2[], unsigned inputs_imm2_length,
  uintptr_t results,
  VectorFormat vd_form, VectorFormat vn_form) {
  VIXL_ASSERT(vd_form != kFormatUndefined);
  VIXL_ASSERT(vn_form != kFormatUndefined);

  SETUP();
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

  __ Ldr(vntmp_single, MemOperand(inputs_n_base, index_n, LSL,
                                  vn_lane_bytes_log2));
  __ Ext(vn_ext, vn_ext, vntmp_ext, vn_lane_bytes);

  {
    EmissionCheckScope guard(&masm,
        kInstructionSize * inputs_imm1_length * inputs_imm2_length * 3);
    for (unsigned i = 0; i < inputs_imm1_length; i++) {
      for (unsigned j = 0; j < inputs_imm2_length; j++) {
        __ Mov(vres, vd);
        (masm.*helper)(vres_helper, inputs_imm1[i], vn_helper, inputs_imm2[j]);
        __ Str(vres, MemOperand(out, vd.SizeInBytes(), PostIndex));
      }
    }
  }

  __ Add(index_n, index_n, 1);
  __ Cmp(index_n, inputs_n_length);
  __ B(lo, &loop_n);

  END();
  RUN();
  TEARDOWN();
}


// Test NEON instructions. The inputs_*[] and expected[] arrays should be
// arrays of rawbit representation of input values. This ensures that
// exact bit comparisons can be performed.
template <typename Td, typename Tn>
static void TestOpImmOpImmNEON(const char * name,
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

  const unsigned results_length = inputs_n_length *
      inputs_imm1_length * inputs_imm2_length;

  Td* results = new Td[results_length * vd_lane_count];
  const unsigned lane_bit = sizeof(Td) * 8;
  const unsigned lane_len_in_hex = MaxHexCharCount<Td, Tn>();

  TestOpImmOpImmNEON_Helper(helper,
                            reinterpret_cast<uintptr_t>(inputs_d),
                            inputs_imm1,
                            inputs_imm1_length,
                            reinterpret_cast<uintptr_t>(inputs_n),
                            inputs_n_length,
                            inputs_imm2,
                            inputs_imm2_length,
                            reinterpret_cast<uintptr_t>(results),
                            vd_form, vn_form);

  if (Test::sim_test_trace()) {
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
  } else {
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
                (n * inputs_imm1_length *
                 inputs_imm2_length * vd_lane_count) +
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
                   lane_len_in_hex+1, padding,
                   lane_len_in_hex, padding,
                   lane_len_in_hex+1, padding,
                   lane_len_in_hex, padding,
                   lane_len_in_hex+1, padding);

            for (unsigned lane = 0; lane < vd_lane_count; lane++) {
              unsigned output_index =
                (n * inputs_imm1_length *
                 inputs_imm2_length * vd_lane_count) +
                (imm1 * inputs_imm2_length * vd_lane_count) +
                (imm2 * vd_lane_count) + lane;
              unsigned input_index_n = (inputs_n_length - vd_lane_count +
                  n + 1 + lane) % inputs_n_length;
              unsigned input_index_imm1 = imm1;
              unsigned input_index_imm2 = imm2;

              printf("%c0x%0*" PRIx64 " | 0x%0*" PRIx64 " | 0x%0*" PRIx64 " "
                "| 0x%0*" PRIx64 " | 0x%0*" PRIx64 " | 0x%0*" PRIx64 "\n",
                results[output_index] !=
                  expected[output_index] ? '*' : ' ',
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

#define CALL_TEST_FP_HELPER(mnemonic, variant, type, input)         \
    Test##type(STRINGIFY(mnemonic) "_" STRINGIFY(variant),          \
               &MacroAssembler::mnemonic,                           \
               input, sizeof(input) / sizeof(input[0]),             \
               kExpected_##mnemonic##_##variant,                    \
               kExpectedCount_##mnemonic##_##variant)

#define DEFINE_TEST_FP(mnemonic, type, input)                       \
    TEST(mnemonic##_d) {                                            \
      CALL_TEST_FP_HELPER(mnemonic, d, type, kInputDouble##input);  \
    }                                                               \
    TEST(mnemonic##_s) {                                            \
      CALL_TEST_FP_HELPER(mnemonic, s, type, kInputFloat##input);   \
    }

// TODO: Test with a newer version of valgrind.
//
// Note: valgrind-3.10.0 does not properly interpret libm's fma() on x86_64.
// Therefore this test will be exiting though an ASSERT and thus leaking
// memory.
DEFINE_TEST_FP(fmadd, 3Op, Basic)
DEFINE_TEST_FP(fmsub, 3Op, Basic)
DEFINE_TEST_FP(fnmadd, 3Op, Basic)
DEFINE_TEST_FP(fnmsub, 3Op, Basic)

DEFINE_TEST_FP(fadd, 2Op, Basic)
DEFINE_TEST_FP(fdiv, 2Op, Basic)
DEFINE_TEST_FP(fmax, 2Op, Basic)
DEFINE_TEST_FP(fmaxnm, 2Op, Basic)
DEFINE_TEST_FP(fmin, 2Op, Basic)
DEFINE_TEST_FP(fminnm, 2Op, Basic)
DEFINE_TEST_FP(fmul, 2Op, Basic)
DEFINE_TEST_FP(fsub, 2Op, Basic)
DEFINE_TEST_FP(fnmul, 2Op, Basic)

DEFINE_TEST_FP(fabs, 1Op, Basic)
DEFINE_TEST_FP(fmov, 1Op, Basic)
DEFINE_TEST_FP(fneg, 1Op, Basic)
DEFINE_TEST_FP(fsqrt, 1Op, Basic)
DEFINE_TEST_FP(frinta, 1Op, Conversions)
DEFINE_TEST_FP(frinti, 1Op, Conversions)
DEFINE_TEST_FP(frintm, 1Op, Conversions)
DEFINE_TEST_FP(frintn, 1Op, Conversions)
DEFINE_TEST_FP(frintp, 1Op, Conversions)
DEFINE_TEST_FP(frintx, 1Op, Conversions)
DEFINE_TEST_FP(frintz, 1Op, Conversions)

TEST(fcmp_d) { CALL_TEST_FP_HELPER(fcmp, d, Cmp, kInputDoubleBasic); }
TEST(fcmp_s) { CALL_TEST_FP_HELPER(fcmp, s, Cmp, kInputFloatBasic); }
TEST(fcmp_dz) { CALL_TEST_FP_HELPER(fcmp, dz, CmpZero, kInputDoubleBasic); }
TEST(fcmp_sz) { CALL_TEST_FP_HELPER(fcmp, sz, CmpZero, kInputFloatBasic); }

TEST(fcvt_sd) { CALL_TEST_FP_HELPER(fcvt, sd, 1Op, kInputDoubleConversions); }
TEST(fcvt_ds) { CALL_TEST_FP_HELPER(fcvt, ds, 1Op, kInputFloatConversions); }

#define DEFINE_TEST_FP_TO_INT(mnemonic, type, input)                \
    TEST(mnemonic##_xd) {                                           \
      CALL_TEST_FP_HELPER(mnemonic, xd, type, kInputDouble##input); \
    }                                                               \
    TEST(mnemonic##_xs) {                                           \
      CALL_TEST_FP_HELPER(mnemonic, xs, type, kInputFloat##input);  \
    }                                                               \
    TEST(mnemonic##_wd) {                                           \
      CALL_TEST_FP_HELPER(mnemonic, wd, type, kInputDouble##input); \
    }                                                               \
    TEST(mnemonic##_ws) {                                           \
      CALL_TEST_FP_HELPER(mnemonic, ws, type, kInputFloat##input);  \
    }

DEFINE_TEST_FP_TO_INT(fcvtas, FPToS, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtau, FPToU, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtms, FPToS, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtmu, FPToU, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtns, FPToS, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtnu, FPToU, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtzs, FPToFixedS, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtzu, FPToFixedU, Conversions)

// TODO: Scvtf-fixed-point
// TODO: Scvtf-integer
// TODO: Ucvtf-fixed-point
// TODO: Ucvtf-integer

// TODO: Fccmp
// TODO: Fcsel


// ==== NEON Tests. ====

#define CALL_TEST_NEON_HELPER_1Op(mnemonic,                                  \
                                  vdform, vnform,                            \
                                  input_n)                                   \
    Test1OpNEON(STRINGIFY(mnemonic) "_" STRINGIFY(vdform),                   \
                &MacroAssembler::mnemonic,                                   \
                input_n,                                                     \
                (sizeof(input_n) / sizeof(input_n[0])),                      \
                kExpected_NEON_##mnemonic##_##vdform,                        \
                kExpectedCount_NEON_##mnemonic##_##vdform,                   \
                kFormat##vdform,                                             \
                kFormat##vnform)

#define CALL_TEST_NEON_HELPER_1OpAcross(mnemonic,                            \
                                        vdform, vnform,                      \
                                        input_n)                             \
    Test1OpAcrossNEON(STRINGIFY(mnemonic) "_" STRINGIFY(vdform)              \
                                          "_" STRINGIFY(vnform),             \
                      &MacroAssembler::mnemonic,                             \
                      input_n,                                               \
                      (sizeof(input_n) / sizeof(input_n[0])),                \
                      kExpected_NEON_##mnemonic##_##vdform##_##vnform,       \
                      kExpectedCount_NEON_##mnemonic##_##vdform##_##vnform,  \
                      kFormat##vdform,                                       \
                      kFormat##vnform)

#define CALL_TEST_NEON_HELPER_2Op(mnemonic,                                  \
                                  vdform, vnform, vmform,                    \
                                  input_d, input_n, input_m)                 \
    Test2OpNEON(STRINGIFY(mnemonic) "_" STRINGIFY(vdform),                   \
                &MacroAssembler::mnemonic,                                   \
                input_d,                                                     \
                input_n,                                                     \
                (sizeof(input_n) / sizeof(input_n[0])),                      \
                input_m,                                                     \
                (sizeof(input_m) / sizeof(input_m[0])),                      \
                kExpected_NEON_##mnemonic##_##vdform,                        \
                kExpectedCount_NEON_##mnemonic##_##vdform,                   \
                kFormat##vdform,                                             \
                kFormat##vnform,                                             \
                kFormat##vmform)

#define CALL_TEST_NEON_HELPER_2OpImm(mnemonic,                               \
                                     vdform, vnform,                         \
                                     input_n, input_m)                       \
    Test2OpImmNEON(STRINGIFY(mnemonic) "_" STRINGIFY(vdform) "_2OPIMM",      \
                   &MacroAssembler::mnemonic,                                \
                   input_n,                                                  \
                   (sizeof(input_n) / sizeof(input_n[0])),                   \
                   input_m,                                                  \
                   (sizeof(input_m) / sizeof(input_m[0])),                   \
                   kExpected_NEON_##mnemonic##_##vdform##_2OPIMM,            \
                   kExpectedCount_NEON_##mnemonic##_##vdform##_2OPIMM,       \
                   kFormat##vdform,                                          \
                   kFormat##vnform)

#define CALL_TEST_NEON_HELPER_ByElement(mnemonic,                            \
                                        vdform, vnform, vmform,              \
                                        input_d, input_n, input_m, indices)  \
    TestByElementNEON(STRINGIFY(mnemonic) "_" STRINGIFY(vdform)              \
        "_" STRINGIFY(vnform) "_" STRINGIFY(vmform),                         \
        &MacroAssembler::mnemonic,                                           \
        input_d,                                                             \
        input_n,                                                             \
        (sizeof(input_n) / sizeof(input_n[0])),                              \
        input_m,                                                             \
        (sizeof(input_m) / sizeof(input_m[0])),                              \
        indices,                                                             \
        (sizeof(indices) / sizeof(indices[0])),                              \
        kExpected_NEON_##mnemonic##_##vdform##_##vnform##_##vmform,          \
        kExpectedCount_NEON_##mnemonic##_##vdform##_##vnform##_##vmform,     \
        kFormat##vdform,                                                     \
        kFormat##vnform,                                                     \
        kFormat##vmform)

#define CALL_TEST_NEON_HELPER_OpImmOpImm(helper,                             \
                                         mnemonic,                           \
                                         vdform, vnform,                     \
                                         input_d, input_imm1,                \
                                         input_n, input_imm2)                \
    TestOpImmOpImmNEON(STRINGIFY(mnemonic) "_" STRINGIFY(vdform),            \
                       helper,                                               \
                       input_d,                                              \
                       input_imm1,                                           \
                       (sizeof(input_imm1) / sizeof(input_imm1[0])),         \
                       input_n,                                              \
                       (sizeof(input_n) / sizeof(input_n[0])),               \
                       input_imm2,                                           \
                       (sizeof(input_imm2) / sizeof(input_imm2[0])),         \
                       kExpected_NEON_##mnemonic##_##vdform,                 \
                       kExpectedCount_NEON_##mnemonic##_##vdform,            \
                       kFormat##vdform,                                      \
                       kFormat##vnform)

#define CALL_TEST_NEON_HELPER_2SAME(mnemonic, variant, input)                \
    CALL_TEST_NEON_HELPER_1Op(mnemonic,                                      \
                              variant, variant,                              \
                              input)

#define DEFINE_TEST_NEON_2SAME_8B_16B(mnemonic, input)                       \
    TEST(mnemonic##_8B) {                                                    \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, 8B, kInput8bits##input);         \
    }                                                                        \
    TEST(mnemonic##_16B) {                                                   \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, 16B, kInput8bits##input);        \
    }

#define DEFINE_TEST_NEON_2SAME_4H_8H(mnemonic, input)                        \
    TEST(mnemonic##_4H) {                                                    \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, 4H, kInput16bits##input);        \
    }                                                                        \
    TEST(mnemonic##_8H) {                                                    \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, 8H, kInput16bits##input);        \
    }

#define DEFINE_TEST_NEON_2SAME_2S_4S(mnemonic, input)                        \
    TEST(mnemonic##_2S) {                                                    \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, 2S, kInput32bits##input);        \
    }                                                                        \
    TEST(mnemonic##_4S) {                                                    \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, 4S, kInput32bits##input);        \
    }

#define DEFINE_TEST_NEON_2SAME_BH(mnemonic, input)                           \
    DEFINE_TEST_NEON_2SAME_8B_16B(mnemonic, input)                           \
    DEFINE_TEST_NEON_2SAME_4H_8H(mnemonic, input)

#define DEFINE_TEST_NEON_2SAME_NO2D(mnemonic, input)                         \
    DEFINE_TEST_NEON_2SAME_BH(mnemonic, input)                               \
    DEFINE_TEST_NEON_2SAME_2S_4S(mnemonic, input)

#define DEFINE_TEST_NEON_2SAME(mnemonic, input)                              \
    DEFINE_TEST_NEON_2SAME_NO2D(mnemonic, input)                             \
    TEST(mnemonic##_2D) {                                                    \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, 2D, kInput64bits##input);        \
    }
#define DEFINE_TEST_NEON_2SAME_SD(mnemonic, input)                           \
    DEFINE_TEST_NEON_2SAME_2S_4S(mnemonic, input)                            \
    TEST(mnemonic##_2D) {                                                    \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, 2D, kInput64bits##input);        \
    }

#define DEFINE_TEST_NEON_2SAME_FP(mnemonic, input)                           \
    TEST(mnemonic##_2S) {                                                    \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, 2S, kInputFloat##input);         \
    }                                                                        \
    TEST(mnemonic##_4S) {                                                    \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, 4S, kInputFloat##input);         \
    }                                                                        \
    TEST(mnemonic##_2D) {                                                    \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, 2D, kInputDouble##input);        \
    }

#define DEFINE_TEST_NEON_2SAME_FP_SCALAR(mnemonic, input)                    \
    TEST(mnemonic##_S) {                                                     \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, S, kInputFloat##input);          \
    }                                                                        \
    TEST(mnemonic##_D) {                                                     \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, D, kInputDouble##input);         \
    }

#define DEFINE_TEST_NEON_2SAME_SCALAR_B(mnemonic, input)                     \
    TEST(mnemonic##_B) {                                                     \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, B, kInput8bits##input);          \
    }
#define DEFINE_TEST_NEON_2SAME_SCALAR_H(mnemonic, input)                     \
    TEST(mnemonic##_H) {                                                     \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, H, kInput16bits##input);         \
    }
#define DEFINE_TEST_NEON_2SAME_SCALAR_S(mnemonic, input)                     \
    TEST(mnemonic##_S) {                                                     \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, S, kInput32bits##input);         \
    }
#define DEFINE_TEST_NEON_2SAME_SCALAR_D(mnemonic, input)                     \
    TEST(mnemonic##_D) {                                                     \
      CALL_TEST_NEON_HELPER_2SAME(mnemonic, D, kInput64bits##input);         \
    }

#define DEFINE_TEST_NEON_2SAME_SCALAR(mnemonic, input)                       \
    DEFINE_TEST_NEON_2SAME_SCALAR_B(mnemonic, input)                         \
    DEFINE_TEST_NEON_2SAME_SCALAR_H(mnemonic, input)                         \
    DEFINE_TEST_NEON_2SAME_SCALAR_S(mnemonic, input)                         \
    DEFINE_TEST_NEON_2SAME_SCALAR_D(mnemonic, input)

#define DEFINE_TEST_NEON_2SAME_SCALAR_SD(mnemonic, input)                    \
    DEFINE_TEST_NEON_2SAME_SCALAR_S(mnemonic, input)                         \
    DEFINE_TEST_NEON_2SAME_SCALAR_D(mnemonic, input)


#define CALL_TEST_NEON_HELPER_ACROSS(mnemonic, vd_form, vn_form, input_n)    \
    CALL_TEST_NEON_HELPER_1OpAcross(mnemonic,                                \
                                    vd_form, vn_form,                        \
                                    input_n)

#define DEFINE_TEST_NEON_ACROSS(mnemonic, input)                             \
    TEST(mnemonic##_B_8B) {                                                  \
      CALL_TEST_NEON_HELPER_ACROSS(mnemonic, B, 8B, kInput8bits##input);     \
    }                                                                        \
    TEST(mnemonic##_B_16B) {                                                 \
      CALL_TEST_NEON_HELPER_ACROSS(mnemonic, B, 16B, kInput8bits##input);    \
    }                                                                        \
    TEST(mnemonic##_H_4H) {                                                  \
      CALL_TEST_NEON_HELPER_ACROSS(mnemonic, H, 4H, kInput16bits##input);    \
    }                                                                        \
    TEST(mnemonic##_H_8H) {                                                  \
      CALL_TEST_NEON_HELPER_ACROSS(mnemonic, H, 8H, kInput16bits##input);    \
    }                                                                        \
    TEST(mnemonic##_S_4S) {                                                  \
      CALL_TEST_NEON_HELPER_ACROSS(mnemonic, S, 4S, kInput32bits##input);    \
    }

#define DEFINE_TEST_NEON_ACROSS_LONG(mnemonic, input)                        \
    TEST(mnemonic##_H_8B) {                                                  \
      CALL_TEST_NEON_HELPER_ACROSS(mnemonic, H, 8B, kInput8bits##input);     \
    }                                                                        \
    TEST(mnemonic##_H_16B) {                                                 \
      CALL_TEST_NEON_HELPER_ACROSS(mnemonic, H, 16B, kInput8bits##input);    \
    }                                                                        \
    TEST(mnemonic##_S_4H) {                                                  \
      CALL_TEST_NEON_HELPER_ACROSS(mnemonic, S, 4H, kInput16bits##input);    \
    }                                                                        \
    TEST(mnemonic##_S_8H) {                                                  \
      CALL_TEST_NEON_HELPER_ACROSS(mnemonic, S, 8H, kInput16bits##input);    \
    }                                                                        \
    TEST(mnemonic##_D_4S) {                                                  \
      CALL_TEST_NEON_HELPER_ACROSS(mnemonic, D, 4S, kInput32bits##input);    \
    }

#define DEFINE_TEST_NEON_ACROSS_FP(mnemonic, input)                          \
    TEST(mnemonic##_S_4S) {                                                  \
      CALL_TEST_NEON_HELPER_ACROSS(mnemonic, S, 4S, kInputFloat##input);     \
    }

#define CALL_TEST_NEON_HELPER_2DIFF(mnemonic,                                \
                                    vdform, vnform,                          \
                                    input_n)                                 \
    CALL_TEST_NEON_HELPER_1Op(mnemonic,                                      \
                              vdform, vnform,                                \
                              input_n)

#define DEFINE_TEST_NEON_2DIFF_LONG(mnemonic, input)                         \
    TEST(mnemonic##_4H) {                                                    \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 4H, 8B, kInput8bits##input);     \
    }                                                                        \
    TEST(mnemonic##_8H) {                                                    \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 8H, 16B, kInput8bits##input);    \
    }                                                                        \
    TEST(mnemonic##_2S) {                                                    \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 2S, 4H, kInput16bits##input);    \
    }                                                                        \
    TEST(mnemonic##_4S) {                                                    \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 4S, 8H, kInput16bits##input);    \
    }                                                                        \
    TEST(mnemonic##_1D) {                                                    \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 1D, 2S, kInput32bits##input);    \
    }                                                                        \
    TEST(mnemonic##_2D) {                                                    \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 2D, 4S, kInput32bits##input);    \
    }

#define DEFINE_TEST_NEON_2DIFF_NARROW(mnemonic, input)                       \
    TEST(mnemonic##_8B) {                                                    \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 8B, 8H, kInput16bits##input);    \
    }                                                                        \
    TEST(mnemonic##_4H) {                                                    \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 4H, 4S, kInput32bits##input);    \
    }                                                                        \
    TEST(mnemonic##_2S) {                                                    \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 2S, 2D, kInput64bits##input);    \
    }                                                                        \
    TEST(mnemonic##2_16B) {                                                  \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 16B, 8H, kInput16bits##input);\
    }                                                                        \
    TEST(mnemonic##2_8H) {                                                   \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 8H, 4S, kInput32bits##input); \
    }                                                                        \
    TEST(mnemonic##2_4S) {                                                   \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 4S, 2D, kInput64bits##input); \
    }

#define DEFINE_TEST_NEON_2DIFF_FP_LONG(mnemonic, input)                      \
    TEST(mnemonic##_4S) {                                                    \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 4S, 4H, kInputFloat16##input);   \
    }                                                                        \
    TEST(mnemonic##_2D) {                                                    \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 2D, 2S, kInputFloat##input);     \
    }                                                                        \
    TEST(mnemonic##2_4S) {                                                   \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 4S, 8H, kInputFloat16##input);\
    }                                                                        \
    TEST(mnemonic##2_2D) {                                                   \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 2D, 4S, kInputFloat##input);  \
    }

#define DEFINE_TEST_NEON_2DIFF_FP_NARROW(mnemonic, input)                    \
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

#define DEFINE_TEST_NEON_2DIFF_FP_NARROW_2S(mnemonic, input)                 \
    TEST(mnemonic##_2S) {                                                    \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, 2S, 2D, kInputDouble##input);    \
    }                                                                        \
    TEST(mnemonic##2_4S) {                                                   \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic##2, 4S, 2D, kInputDouble##input); \
    }

#define DEFINE_TEST_NEON_2DIFF_SCALAR_NARROW(mnemonic, input)                \
    TEST(mnemonic##_B) {                                                     \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, B, H, kInput16bits##input);      \
    }                                                                        \
    TEST(mnemonic##_H) {                                                     \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, H, S, kInput32bits##input);      \
    }                                                                        \
    TEST(mnemonic##_S) {                                                     \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, S, D, kInput64bits##input);      \
    }

#define DEFINE_TEST_NEON_2DIFF_FP_SCALAR_SD(mnemonic, input)                 \
    TEST(mnemonic##_S) {                                                     \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, S, 2S, kInputFloat##input);      \
    }                                                                        \
    TEST(mnemonic##_D) {                                                     \
      CALL_TEST_NEON_HELPER_2DIFF(mnemonic, D, 2D, kInputDouble##input);     \
    }

#define CALL_TEST_NEON_HELPER_3SAME(mnemonic, variant, input_d, input_nm) {  \
    CALL_TEST_NEON_HELPER_2Op(mnemonic,                                      \
                              variant, variant, variant,                     \
                              input_d, input_nm, input_nm);                  \
    }

#define DEFINE_TEST_NEON_3SAME_8B_16B(mnemonic, input)                       \
    TEST(mnemonic##_8B) {                                                    \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, 8B,                              \
                                  kInput8bitsAccDestination,                 \
                                  kInput8bits##input);                       \
    }                                                                        \
    TEST(mnemonic##_16B) {                                                   \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, 16B,                             \
                                  kInput8bitsAccDestination,                 \
                                  kInput8bits##input);                       \
    }                                                                        \

#define DEFINE_TEST_NEON_3SAME_HS(mnemonic, input)                           \
    TEST(mnemonic##_4H) {                                                    \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, 4H,                              \
                                  kInput16bitsAccDestination,                \
                                  kInput16bits##input);                      \
    }                                                                        \
    TEST(mnemonic##_8H) {                                                    \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, 8H,                              \
                                  kInput16bitsAccDestination,                \
                                  kInput16bits##input);                      \
    }                                                                        \
    TEST(mnemonic##_2S) {                                                    \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, 2S,                              \
                                  kInput32bitsAccDestination,                \
                                  kInput32bits##input);                      \
    }                                                                        \
    TEST(mnemonic##_4S) {                                                    \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, 4S,                              \
                                  kInput32bitsAccDestination,                \
                                  kInput32bits##input);                      \
    }

#define DEFINE_TEST_NEON_3SAME_NO2D(mnemonic, input)                         \
    DEFINE_TEST_NEON_3SAME_8B_16B(mnemonic, input)                           \
    DEFINE_TEST_NEON_3SAME_HS(mnemonic, input)

#define DEFINE_TEST_NEON_3SAME(mnemonic, input)                              \
    DEFINE_TEST_NEON_3SAME_NO2D(mnemonic, input)                             \
    TEST(mnemonic##_2D) {                                                    \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, 2D,                              \
                                  kInput64bitsAccDestination,                \
                                  kInput64bits##input);                      \
    }

#define DEFINE_TEST_NEON_3SAME_FP(mnemonic, input)                           \
    TEST(mnemonic##_2S) {                                                    \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, 2S,                              \
                                  kInputFloatAccDestination,                 \
                                  kInputFloat##input);                       \
    }                                                                        \
    TEST(mnemonic##_4S) {                                                    \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, 4S,                              \
                                  kInputFloatAccDestination,                 \
                                  kInputFloat##input);                       \
    }                                                                        \
    TEST(mnemonic##_2D) {                                                    \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, 2D,                              \
                                  kInputDoubleAccDestination,                \
                                  kInputDouble##input);                      \
    }

#define DEFINE_TEST_NEON_3SAME_SCALAR_D(mnemonic, input)                     \
    TEST(mnemonic##_D) {                                                     \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, D,                               \
                                  kInput64bitsAccDestination,                \
                                  kInput64bits##input);                      \
    }

#define DEFINE_TEST_NEON_3SAME_SCALAR_HS(mnemonic, input)                    \
    TEST(mnemonic##_H) {                                                     \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, H,                               \
                                  kInput16bitsAccDestination,                \
                                  kInput16bits##input);                      \
    }                                                                        \
    TEST(mnemonic##_S) {                                                     \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, S,                               \
                                  kInput32bitsAccDestination,                \
                                  kInput32bits##input);                      \
    }                                                                        \

#define DEFINE_TEST_NEON_3SAME_SCALAR(mnemonic, input)                       \
    TEST(mnemonic##_B) {                                                     \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, B,                               \
                                  kInput8bitsAccDestination,                 \
                                  kInput8bits##input);                       \
    }                                                                        \
    TEST(mnemonic##_H) {                                                     \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, H,                               \
                                  kInput16bitsAccDestination,                \
                                  kInput16bits##input);                      \
    }                                                                        \
    TEST(mnemonic##_S) {                                                     \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, S,                               \
                                  kInput32bitsAccDestination,                \
                                  kInput32bits##input);                      \
    }                                                                        \
    TEST(mnemonic##_D) {                                                     \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, D,                               \
                                  kInput64bitsAccDestination,                \
                                  kInput64bits##input);                      \
    }

#define DEFINE_TEST_NEON_3SAME_FP_SCALAR(mnemonic, input)                    \
    TEST(mnemonic##_S) {                                                     \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, S,                               \
                                  kInputFloatAccDestination,                 \
                                  kInputFloat##input);                       \
    }                                                                        \
    TEST(mnemonic##_D) {                                                     \
      CALL_TEST_NEON_HELPER_3SAME(mnemonic, D,                               \
                                  kInputDoubleAccDestination,                \
                                  kInputDouble##input);                      \
    }

#define CALL_TEST_NEON_HELPER_3DIFF(mnemonic,                                \
                                    vdform, vnform, vmform,                  \
                                    input_d, input_n, input_m) {             \
    CALL_TEST_NEON_HELPER_2Op(mnemonic,                                      \
                              vdform, vnform, vmform,                        \
                              input_d, input_n, input_m);                    \
    }

#define DEFINE_TEST_NEON_3DIFF_LONG_8H(mnemonic, input)                      \
    TEST(mnemonic##_8H) {                                                    \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic, 8H, 8B, 8B,                      \
                                  kInput16bitsAccDestination,                \
                                  kInput8bits##input, kInput8bits##input);   \
    }                                                                        \
    TEST(mnemonic##2_8H) {                                                   \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2, 8H, 16B, 16B,                 \
                                  kInput16bitsAccDestination,                \
                                  kInput8bits##input, kInput8bits##input);   \
    }

#define DEFINE_TEST_NEON_3DIFF_LONG_4S(mnemonic, input)                      \
    TEST(mnemonic##_4S) {                                                    \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic, 4S, 4H, 4H,                      \
                                  kInput32bitsAccDestination,                \
                                  kInput16bits##input, kInput16bits##input); \
    }                                                                        \
    TEST(mnemonic##2_4S) {                                                   \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2, 4S, 8H, 8H,                   \
                                  kInput32bitsAccDestination,                \
                                  kInput16bits##input, kInput16bits##input); \
    }

#define DEFINE_TEST_NEON_3DIFF_LONG_2D(mnemonic, input)                      \
    TEST(mnemonic##_2D) {                                                    \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic, 2D, 2S, 2S,                      \
                                  kInput64bitsAccDestination,                \
                                  kInput32bits##input, kInput32bits##input); \
    }                                                                        \
    TEST(mnemonic##2_2D) {                                                   \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2, 2D, 4S, 4S,                   \
                                  kInput64bitsAccDestination,                \
                                  kInput32bits##input, kInput32bits##input); \
    }

#define DEFINE_TEST_NEON_3DIFF_LONG_SD(mnemonic, input)                      \
    DEFINE_TEST_NEON_3DIFF_LONG_4S(mnemonic, input)                          \
    DEFINE_TEST_NEON_3DIFF_LONG_2D(mnemonic, input)

#define DEFINE_TEST_NEON_3DIFF_LONG(mnemonic, input)                         \
    DEFINE_TEST_NEON_3DIFF_LONG_8H(mnemonic, input)                          \
    DEFINE_TEST_NEON_3DIFF_LONG_4S(mnemonic, input)                          \
    DEFINE_TEST_NEON_3DIFF_LONG_2D(mnemonic, input)

#define DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_S(mnemonic, input)                \
    TEST(mnemonic##_S) {                                                     \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic, S, H, H,                         \
                                  kInput32bitsAccDestination,                \
                                  kInput16bits##input,                       \
                                  kInput16bits##input);                      \
    }

#define DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_D(mnemonic, input)                \
    TEST(mnemonic##_D) {                                                     \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic, D, S, S,                         \
                                  kInput64bitsAccDestination,                \
                                  kInput32bits##input,                       \
                                  kInput32bits##input);                      \
    }

#define DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_SD(mnemonic, input)               \
    DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_S(mnemonic, input)                    \
    DEFINE_TEST_NEON_3DIFF_SCALAR_LONG_D(mnemonic, input)

#define DEFINE_TEST_NEON_3DIFF_WIDE(mnemonic, input)                         \
    TEST(mnemonic##_8H) {                                                    \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic, 8H, 8H, 8B,                      \
                                  kInput16bitsAccDestination,                \
                                  kInput16bits##input, kInput8bits##input);  \
    }                                                                        \
    TEST(mnemonic##_4S) {                                                    \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic, 4S, 4S, 4H,                      \
                                  kInput32bitsAccDestination,                \
                                  kInput32bits##input, kInput16bits##input); \
    }                                                                        \
    TEST(mnemonic##_2D) {                                                    \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic, 2D, 2D, 2S,                      \
                                  kInput64bitsAccDestination,                \
                                  kInput64bits##input, kInput32bits##input); \
    }                                                                        \
    TEST(mnemonic##2_8H) {                                                   \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2, 8H, 8H, 16B,                  \
                                  kInput16bitsAccDestination,                \
                                  kInput16bits##input, kInput8bits##input);  \
    }                                                                        \
    TEST(mnemonic##2_4S) {                                                   \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2, 4S, 4S, 8H,                   \
                                  kInput32bitsAccDestination,                \
                                  kInput32bits##input, kInput16bits##input); \
    }                                                                        \
    TEST(mnemonic##2_2D) {                                                   \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2, 2D, 2D, 4S,                   \
                                  kInput64bitsAccDestination,                \
                                  kInput64bits##input, kInput32bits##input); \
    }

#define DEFINE_TEST_NEON_3DIFF_NARROW(mnemonic, input)                       \
    TEST(mnemonic##_8B) {                                                    \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic, 8B, 8H, 8H,                      \
                                  kInput8bitsAccDestination,                 \
                                  kInput16bits##input, kInput16bits##input); \
    }                                                                        \
    TEST(mnemonic##_4H) {                                                    \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic, 4H, 4S, 4S,                      \
                                  kInput16bitsAccDestination,                \
                                  kInput32bits##input, kInput32bits##input); \
    }                                                                        \
    TEST(mnemonic##_2S) {                                                    \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic, 2S, 2D, 2D,                      \
                                  kInput32bitsAccDestination,                \
                                  kInput64bits##input, kInput64bits##input); \
    }                                                                        \
    TEST(mnemonic##2_16B) {                                                  \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2, 16B, 8H, 8H,                  \
                                  kInput8bitsAccDestination,                 \
                                  kInput16bits##input, kInput16bits##input); \
    }                                                                        \
    TEST(mnemonic##2_8H) {                                                   \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2, 8H, 4S, 4S,                   \
                                  kInput16bitsAccDestination,                \
                                  kInput32bits##input, kInput32bits##input); \
    }                                                                        \
    TEST(mnemonic##2_4S) {                                                   \
      CALL_TEST_NEON_HELPER_3DIFF(mnemonic##2, 4S, 2D, 2D,                   \
                                  kInput32bitsAccDestination,                \
                                  kInput64bits##input, kInput64bits##input); \
    }

#define CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                               \
                                     vdform, vnform,                         \
                                     input_n,                                \
                                     input_imm) {                            \
    CALL_TEST_NEON_HELPER_2OpImm(mnemonic,                                   \
                                 vdform, vnform,                             \
                                 input_n, input_imm);                        \
    }

#define DEFINE_TEST_NEON_2OPIMM(mnemonic, input, input_imm)                  \
    TEST(mnemonic##_8B_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   8B, 8B,                                   \
                                   kInput8bits##input,                       \
                                   kInput8bitsImm##input_imm);               \
    }                                                                        \
    TEST(mnemonic##_16B_2OPIMM) {                                            \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   16B, 16B,                                 \
                                   kInput8bits##input,                       \
                                   kInput8bitsImm##input_imm);               \
    }                                                                        \
    TEST(mnemonic##_4H_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   4H, 4H,                                   \
                                   kInput16bits##input,                      \
                                   kInput16bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_8H_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   8H, 8H,                                   \
                                   kInput16bits##input,                      \
                                   kInput16bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_2S_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   2S, 2S,                                   \
                                   kInput32bits##input,                      \
                                   kInput32bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_4S_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   4S, 4S,                                   \
                                   kInput32bits##input,                      \
                                   kInput32bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_2D_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   2D, 2D,                                   \
                                   kInput64bits##input,                      \
                                   kInput64bitsImm##input_imm);              \
    }

#define DEFINE_TEST_NEON_2OPIMM_COPY(mnemonic, input, input_imm)             \
    TEST(mnemonic##_8B_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   8B, B,                                    \
                                   kInput8bits##input,                       \
                                   kInput8bitsImm##input_imm);               \
    }                                                                        \
    TEST(mnemonic##_16B_2OPIMM) {                                            \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   16B, B,                                   \
                                   kInput8bits##input,                       \
                                   kInput8bitsImm##input_imm);               \
    }                                                                        \
    TEST(mnemonic##_4H_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   4H, H,                                    \
                                   kInput16bits##input,                      \
                                   kInput16bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_8H_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   8H, H,                                    \
                                   kInput16bits##input,                      \
                                   kInput16bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_2S_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   2S, S,                                    \
                                   kInput32bits##input,                      \
                                   kInput32bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_4S_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   4S, S,                                    \
                                   kInput32bits##input,                      \
                                   kInput32bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_2D_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   2D, D,                                    \
                                   kInput64bits##input,                      \
                                   kInput64bitsImm##input_imm);              \
    }

#define DEFINE_TEST_NEON_2OPIMM_NARROW(mnemonic, input, input_imm)           \
    TEST(mnemonic##_8B_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   8B, 8H,                                   \
                                   kInput16bits##input,                      \
                                   kInput8bitsImm##input_imm);               \
    }                                                                        \
    TEST(mnemonic##_4H_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   4H, 4S,                                   \
                                   kInput32bits##input,                      \
                                   kInput16bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_2S_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   2S, 2D,                                   \
                                   kInput64bits##input,                      \
                                   kInput32bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##2_16B_2OPIMM) {                                           \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic##2,                              \
                                   16B, 8H,                                  \
                                   kInput16bits##input,                      \
                                   kInput8bitsImm##input_imm);               \
    }                                                                        \
    TEST(mnemonic##2_8H_2OPIMM) {                                            \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic##2,                              \
                                   8H, 4S,                                   \
                                   kInput32bits##input,                      \
                                   kInput16bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##2_4S_2OPIMM) {                                            \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic##2,                              \
                                   4S, 2D,                                   \
                                   kInput64bits##input,                      \
                                   kInput32bitsImm##input_imm);              \
    }

#define DEFINE_TEST_NEON_2OPIMM_SCALAR_NARROW(mnemonic, input, input_imm)    \
    TEST(mnemonic##_B_2OPIMM) {                                              \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   B, H,                                     \
                                   kInput16bits##input,                      \
                                   kInput8bitsImm##input_imm);               \
    }                                                                        \
    TEST(mnemonic##_H_2OPIMM) {                                              \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   H, S,                                     \
                                   kInput32bits##input,                      \
                                   kInput16bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_S_2OPIMM) {                                              \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   S, D,                                     \
                                   kInput64bits##input,                      \
                                   kInput32bitsImm##input_imm);              \
    }

#define DEFINE_TEST_NEON_2OPIMM_FCMP_ZERO(mnemonic, input, input_imm)        \
    TEST(mnemonic##_2S_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(                                          \
          mnemonic,                                                          \
          2S, 2S,                                                            \
          kInputFloat##Basic,                                                \
          kInputDoubleImm##input_imm)                                        \
    }                                                                        \
    TEST(mnemonic##_4S_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(                                          \
          mnemonic,                                                          \
          4S, 4S,                                                            \
          kInputFloat##input,                                                \
          kInputDoubleImm##input_imm);                                       \
    }                                                                        \
    TEST(mnemonic##_2D_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(                                          \
          mnemonic,                                                          \
          2D, 2D,                                                            \
          kInputDouble##input,                                               \
          kInputDoubleImm##input_imm);                                       \
    }

#define DEFINE_TEST_NEON_2OPIMM_FP(mnemonic, input, input_imm)               \
    TEST(mnemonic##_2S_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(                                          \
          mnemonic,                                                          \
          2S, 2S,                                                            \
          kInputFloat##Basic,                                                \
          kInput32bitsImm##input_imm)                                        \
    }                                                                        \
    TEST(mnemonic##_4S_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(                                          \
          mnemonic,                                                          \
          4S, 4S,                                                            \
          kInputFloat##input,                                                \
          kInput32bitsImm##input_imm)                                        \
    }                                                                        \
    TEST(mnemonic##_2D_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(                                          \
          mnemonic,                                                          \
          2D, 2D,                                                            \
          kInputDouble##input,                                               \
          kInput64bitsImm##input_imm)                                        \
    }

#define DEFINE_TEST_NEON_2OPIMM_FP_SCALAR(mnemonic, input, input_imm)        \
    TEST(mnemonic##_S_2OPIMM) {                                              \
      CALL_TEST_NEON_HELPER_2OPIMM(                                          \
          mnemonic,                                                          \
          S, S,                                                              \
          kInputFloat##Basic,                                                \
          kInput32bitsImm##input_imm)                                        \
    }                                                                        \
    TEST(mnemonic##_D_2OPIMM) {                                              \
      CALL_TEST_NEON_HELPER_2OPIMM(                                          \
          mnemonic,                                                          \
          D, D,                                                              \
          kInputDouble##input,                                               \
          kInput64bitsImm##input_imm)                                        \
    }

#define DEFINE_TEST_NEON_2OPIMM_SD(mnemonic, input, input_imm)               \
    TEST(mnemonic##_2S_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   2S, 2S,                                   \
                                   kInput32bits##input,                      \
                                   kInput32bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_4S_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   4S, 4S,                                   \
                                   kInput32bits##input,                      \
                                   kInput32bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_2D_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   2D, 2D,                                   \
                                   kInput64bits##input,                      \
                                   kInput64bitsImm##input_imm);              \
    }

#define DEFINE_TEST_NEON_2OPIMM_SCALAR_D(mnemonic, input, input_imm)         \
    TEST(mnemonic##_D_2OPIMM) {                                              \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   D, D,                                     \
                                   kInput64bits##input,                      \
                                   kInput64bitsImm##input_imm);              \
    }

#define DEFINE_TEST_NEON_2OPIMM_SCALAR_SD(mnemonic, input, input_imm)        \
    TEST(mnemonic##_S_2OPIMM) {                                              \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   S, S,                                     \
                                   kInput32bits##input,                      \
                                   kInput32bitsImm##input_imm);              \
    }                                                                        \
    DEFINE_TEST_NEON_2OPIMM_SCALAR_D(mnemonic, input, input_imm)

#define DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_D(mnemonic, input, input_imm)      \
    TEST(mnemonic##_D_2OPIMM) {                                              \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   D, D,                                     \
                                   kInputDouble##input,                      \
                                   kInputDoubleImm##input_imm);              \
    }

#define DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_SD(mnemonic, input, input_imm)     \
    TEST(mnemonic##_S_2OPIMM) {                                              \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   S, S,                                     \
                                   kInputFloat##input,                       \
                                   kInputDoubleImm##input_imm);              \
    }                                                                        \
    DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_D(mnemonic, input, input_imm)

#define DEFINE_TEST_NEON_2OPIMM_SCALAR(mnemonic, input, input_imm)           \
    TEST(mnemonic##_B_2OPIMM) {                                              \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   B, B,                                     \
                                   kInput8bits##input,                       \
                                   kInput8bitsImm##input_imm);               \
    }                                                                        \
    TEST(mnemonic##_H_2OPIMM) {                                              \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   H, H,                                     \
                                   kInput16bits##input,                      \
                                   kInput16bitsImm##input_imm);              \
    }                                                                        \
    DEFINE_TEST_NEON_2OPIMM_SCALAR_SD(mnemonic, input, input_imm)

#define DEFINE_TEST_NEON_2OPIMM_LONG(mnemonic, input, input_imm)             \
    TEST(mnemonic##_8H_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   8H, 8B,                                   \
                                   kInput8bits##input,                       \
                                   kInput8bitsImm##input_imm);               \
    }                                                                        \
    TEST(mnemonic##_4S_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   4S, 4H,                                   \
                                   kInput16bits##input,                      \
                                   kInput16bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##_2D_2OPIMM) {                                             \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic,                                 \
                                   2D, 2S,                                   \
                                   kInput32bits##input,                      \
                                   kInput32bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##2_8H_2OPIMM) {                                            \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic##2,                              \
                                   8H, 16B,                                  \
                                   kInput8bits##input,                       \
                                   kInput8bitsImm##input_imm);               \
    }                                                                        \
    TEST(mnemonic##2_4S_2OPIMM) {                                            \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic##2,                              \
                                   4S, 8H,                                   \
                                   kInput16bits##input,                      \
                                   kInput16bitsImm##input_imm);              \
    }                                                                        \
    TEST(mnemonic##2_2D_2OPIMM) {                                            \
      CALL_TEST_NEON_HELPER_2OPIMM(mnemonic##2,                              \
                                   2D, 4S,                                   \
                                   kInput32bits##input,                      \
                                   kInput32bitsImm##input_imm);              \
    }

#define CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                            \
                                        vdform, vnform, vmform,              \
                                        input_d, input_n,                    \
                                        input_m, indices) {                  \
    CALL_TEST_NEON_HELPER_ByElement(mnemonic,                                \
                                    vdform, vnform, vmform,                  \
                                    input_d, input_n,                        \
                                    input_m, indices);                       \
    }

#define DEFINE_TEST_NEON_BYELEMENT(mnemonic, input_d, input_n, input_m)      \
    TEST(mnemonic##_4H_4H_H) {                                               \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      4H, 4H, H,                             \
                                      kInput16bits##input_d,                 \
                                      kInput16bits##input_n,                 \
                                      kInput16bits##input_m,                 \
                                      kInputHIndices);                       \
    }                                                                        \
    TEST(mnemonic##_8H_8H_H) {                                               \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      8H, 8H, H,                             \
                                      kInput16bits##input_d,                 \
                                      kInput16bits##input_n,                 \
                                      kInput16bits##input_m,                 \
                                      kInputHIndices);                       \
    }                                                                        \
    TEST(mnemonic##_2S_2S_S) {                                               \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      2S, 2S, S,                             \
                                      kInput32bits##input_d,                 \
                                      kInput32bits##input_n,                 \
                                      kInput32bits##input_m,                 \
                                      kInputSIndices);                       \
    }                                                                        \
    TEST(mnemonic##_4S_4S_S) {                                               \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      4S, 4S, S,                             \
                                      kInput32bits##input_d,                 \
                                      kInput32bits##input_n,                 \
                                      kInput32bits##input_m,                 \
                                      kInputSIndices);                       \
    }

#define DEFINE_TEST_NEON_BYELEMENT_SCALAR(mnemonic,                          \
                                          input_d, input_n, input_m)         \
    TEST(mnemonic##_H_H_H) {                                                 \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      H, H, H,                               \
                                      kInput16bits##input_d,                 \
                                      kInput16bits##input_n,                 \
                                      kInput16bits##input_m,                 \
                                      kInputHIndices);                       \
    }                                                                        \
    TEST(mnemonic##_S_S_S) {                                                 \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      S, S, S,                               \
                                      kInput32bits##input_d,                 \
                                      kInput32bits##input_n,                 \
                                      kInput32bits##input_m,                 \
                                      kInputSIndices);                       \
    }

#define DEFINE_TEST_NEON_FP_BYELEMENT(mnemonic, input_d, input_n, input_m)   \
    TEST(mnemonic##_2S_2S_S) {                                               \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      2S, 2S, S,                             \
                                      kInputFloat##input_d,                  \
                                      kInputFloat##input_n,                  \
                                      kInputFloat##input_m,                  \
                                      kInputSIndices);                       \
    }                                                                        \
    TEST(mnemonic##_4S_4S_S) {                                               \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      4S, 4S, S,                             \
                                      kInputFloat##input_d,                  \
                                      kInputFloat##input_n,                  \
                                      kInputFloat##input_m,                  \
                                      kInputSIndices);                       \
    }                                                                        \
    TEST(mnemonic##_2D_2D_D) {                                               \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      2D, 2D, D,                             \
                                      kInputDouble##input_d,                 \
                                      kInputDouble##input_n,                 \
                                      kInputDouble##input_m,                 \
                                      kInputDIndices);                       \
    }                                                                        \

#define DEFINE_TEST_NEON_FP_BYELEMENT_SCALAR(mnemonic, inp_d, inp_n, inp_m)  \
    TEST(mnemonic##_S_S_S) {                                                 \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      S, S, S,                               \
                                      kInputFloat##inp_d,                    \
                                      kInputFloat##inp_n,                    \
                                      kInputFloat##inp_m,                    \
                                      kInputSIndices);                       \
    }                                                                        \
    TEST(mnemonic##_D_D_D) {                                                 \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      D, D, D,                               \
                                      kInputDouble##inp_d,                   \
                                      kInputDouble##inp_n,                   \
                                      kInputDouble##inp_m,                   \
                                      kInputDIndices);                       \
    }                                                                        \


#define DEFINE_TEST_NEON_BYELEMENT_DIFF(mnemonic, input_d, input_n, input_m) \
    TEST(mnemonic##_4S_4H_H) {                                               \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      4S, 4H, H,                             \
                                      kInput32bits##input_d,                 \
                                      kInput16bits##input_n,                 \
                                      kInput16bits##input_m,                 \
                                      kInputHIndices);                       \
    }                                                                        \
    TEST(mnemonic##2_4S_8H_H) {                                              \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic##2,                           \
                                      4S, 8H, H,                             \
                                      kInput32bits##input_d,                 \
                                      kInput16bits##input_n,                 \
                                      kInput16bits##input_m,                 \
                                      kInputHIndices);                       \
    }                                                                        \
    TEST(mnemonic##_2D_2S_S) {                                               \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      2D, 2S, S,                             \
                                      kInput64bits##input_d,                 \
                                      kInput32bits##input_n,                 \
                                      kInput32bits##input_m,                 \
                                      kInputSIndices);                       \
    }                                                                        \
    TEST(mnemonic##2_2D_4S_S) {                                              \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic##2,                           \
                                      2D, 4S, S,                             \
                                      kInput64bits##input_d,                 \
                                      kInput32bits##input_n,                 \
                                      kInput32bits##input_m,                 \
                                      kInputSIndices);                       \
    }

#define DEFINE_TEST_NEON_BYELEMENT_DIFF_SCALAR(mnemonic,                     \
                                               input_d, input_n, input_m)    \
    TEST(mnemonic##_S_H_H) {                                                 \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      S, H, H,                               \
                                      kInput32bits##input_d,                 \
                                      kInput16bits##input_n,                 \
                                      kInput16bits##input_m,                 \
                                      kInputHIndices);                       \
    }                                                                        \
    TEST(mnemonic##_D_S_S) {                                                 \
      CALL_TEST_NEON_HELPER_BYELEMENT(mnemonic,                              \
                                      D, S, S,                               \
                                      kInput64bits##input_d,                 \
                                      kInput32bits##input_n,                 \
                                      kInput32bits##input_m,                 \
                                      kInputSIndices);                       \
    }


#define CALL_TEST_NEON_HELPER_2OP2IMM(mnemonic,                              \
                                      variant,                               \
                                      input_d,                               \
                                      input_imm1,                            \
                                      input_n,                               \
                                      input_imm2) {                          \
    CALL_TEST_NEON_HELPER_OpImmOpImm(&MacroAssembler::mnemonic,              \
                                     mnemonic,                               \
                                     variant, variant,                       \
                                     input_d, input_imm1,                    \
                                     input_n, input_imm2);                   \
    }

#define DEFINE_TEST_NEON_2OP2IMM(mnemonic,                                   \
                                 input_d, input_imm1,                        \
                                 input_n, input_imm2)                        \
    TEST(mnemonic##_B) {                                                     \
      CALL_TEST_NEON_HELPER_2OP2IMM(mnemonic,                                \
                                    16B,                                     \
                                    kInput8bits##input_d,                    \
                                    kInput8bitsImm##input_imm1,              \
                                    kInput8bits##input_n,                    \
                                    kInput8bitsImm##input_imm2);             \
    }                                                                        \
    TEST(mnemonic##_H) {                                                     \
      CALL_TEST_NEON_HELPER_2OP2IMM(mnemonic,                                \
                                    8H,                                      \
                                    kInput16bits##input_d,                   \
                                    kInput16bitsImm##input_imm1,             \
                                    kInput16bits##input_n,                   \
                                    kInput16bitsImm##input_imm2);            \
    }                                                                        \
    TEST(mnemonic##_S) {                                                     \
      CALL_TEST_NEON_HELPER_2OP2IMM(mnemonic,                                \
                                    4S,                                      \
                                    kInput32bits##input_d,                   \
                                    kInput32bitsImm##input_imm1,             \
                                    kInput32bits##input_n,                   \
                                    kInput32bitsImm##input_imm2);            \
    }                                                                        \
    TEST(mnemonic##_D) {                                                     \
      CALL_TEST_NEON_HELPER_2OP2IMM(mnemonic,                                \
                                    2D,                                      \
                                    kInput64bits##input_d,                   \
                                    kInput64bitsImm##input_imm1,             \
                                    kInput64bits##input_n,                   \
                                    kInput64bitsImm##input_imm2);            \
    }


// Advanced SIMD copy.
DEFINE_TEST_NEON_2OP2IMM(ins,
                         Basic, LaneCountFromZero,
                         Basic, LaneCountFromZero)
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
DEFINE_TEST_NEON_3SAME_FP_SCALAR(fcmge, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(facge, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(fabd, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(fcmgt, Basic)
DEFINE_TEST_NEON_3SAME_FP_SCALAR(facgt, Basic)


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
DEFINE_TEST_NEON_2OPIMM_SD(scvtf, FixedPointConversions, \
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
DEFINE_TEST_NEON_2OPIMM_SD(ucvtf, FixedPointConversions, \
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
DEFINE_TEST_NEON_2OPIMM_SCALAR_SD(scvtf, FixedPointConversions, \
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
DEFINE_TEST_NEON_2OPIMM_SCALAR_SD(ucvtf, FixedPointConversions, \
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
DEFINE_TEST_NEON_2SAME_FP(frintn, Conversions)
DEFINE_TEST_NEON_2SAME_FP(frintm, Conversions)
DEFINE_TEST_NEON_2SAME_FP(fcvtns, Conversions)
DEFINE_TEST_NEON_2SAME_FP(fcvtms, Conversions)
DEFINE_TEST_NEON_2SAME_FP(fcvtas, Conversions)
// SCVTF (vector, integer) covered by SCVTF(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2OPIMM_FCMP_ZERO(fcmgt, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_FCMP_ZERO(fcmeq, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_FCMP_ZERO(fcmlt, Basic, Zero)
DEFINE_TEST_NEON_2SAME_FP(fabs, Basic)
DEFINE_TEST_NEON_2SAME_FP(frintp, Conversions)
DEFINE_TEST_NEON_2SAME_FP(frintz, Conversions)
DEFINE_TEST_NEON_2SAME_FP(fcvtps, Conversions)
// FCVTZS(vector, integer) covered by FCVTZS(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2SAME_2S_4S(urecpe, Basic)
DEFINE_TEST_NEON_2SAME_FP(frecpe, Basic)
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
DEFINE_TEST_NEON_2SAME_FP(frinta, Conversions)
DEFINE_TEST_NEON_2SAME_FP(frintx, Conversions)
DEFINE_TEST_NEON_2SAME_FP(fcvtnu, Conversions)
DEFINE_TEST_NEON_2SAME_FP(fcvtmu, Conversions)
DEFINE_TEST_NEON_2SAME_FP(fcvtau, Conversions)
// UCVTF (vector, integer) covered by UCVTF(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2SAME_8B_16B(not_, Basic)
DEFINE_TEST_NEON_2SAME_8B_16B(rbit, Basic)
DEFINE_TEST_NEON_2OPIMM_FCMP_ZERO(fcmge, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_FCMP_ZERO(fcmle, Basic, Zero)
DEFINE_TEST_NEON_2SAME_FP(fneg, Basic)
DEFINE_TEST_NEON_2SAME_FP(frinti, Conversions)
DEFINE_TEST_NEON_2SAME_FP(fcvtpu, Conversions)
// FCVTZU(vector, integer) covered by FCVTZU(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2SAME_2S_4S(ursqrte, Basic)
DEFINE_TEST_NEON_2SAME_FP(frsqrte, Basic)
DEFINE_TEST_NEON_2SAME_FP(fsqrt, Basic)


// Advanced SIMD scalar two-register miscellaneous.
DEFINE_TEST_NEON_2SAME_SCALAR(suqadd, Basic)
DEFINE_TEST_NEON_2SAME_SCALAR(sqabs, Basic)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(cmgt, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(cmeq, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_SCALAR_D(cmlt, Basic, Zero)
DEFINE_TEST_NEON_2SAME_SCALAR_D(abs, Basic)
DEFINE_TEST_NEON_2DIFF_SCALAR_NARROW(sqxtn, Basic)
DEFINE_TEST_NEON_2SAME_FP_SCALAR(fcvtns, Conversions)
DEFINE_TEST_NEON_2SAME_FP_SCALAR(fcvtms, Conversions)
DEFINE_TEST_NEON_2SAME_FP_SCALAR(fcvtas, Conversions)
// SCVTF (vector, integer) covered by SCVTF(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_SD(fcmgt, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_SD(fcmeq, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_SD(fcmlt, Basic, Zero)
DEFINE_TEST_NEON_2SAME_FP_SCALAR(fcvtps, Conversions)
// FCVTZS(vector, integer) covered by FCVTZS(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2SAME_FP_SCALAR(frecpe, Basic)
DEFINE_TEST_NEON_2SAME_FP_SCALAR(frecpx, Basic)
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
DEFINE_TEST_NEON_2SAME_FP_SCALAR(fcvtnu, Conversions)
DEFINE_TEST_NEON_2SAME_FP_SCALAR(fcvtmu, Conversions)
DEFINE_TEST_NEON_2SAME_FP_SCALAR(fcvtau, Conversions)
// UCVTF (vector, integer) covered by UCVTF(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_SD(fcmge, Basic, Zero)
DEFINE_TEST_NEON_2OPIMM_FP_SCALAR_SD(fcmle, Basic, Zero)
DEFINE_TEST_NEON_2SAME_FP_SCALAR(fcvtpu, Conversions)
// FCVTZU(vector, integer) covered by FCVTZU(vector, fixed point) with fbits 0.
DEFINE_TEST_NEON_2SAME_FP_SCALAR(frsqrte, Basic)


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
DEFINE_TEST_NEON_FP_BYELEMENT_SCALAR(fmla, Basic, Basic, Basic)
DEFINE_TEST_NEON_FP_BYELEMENT_SCALAR(fmls, Basic, Basic, Basic)
DEFINE_TEST_NEON_FP_BYELEMENT_SCALAR(fmul, Basic, Basic, Basic)
DEFINE_TEST_NEON_FP_BYELEMENT_SCALAR(fmulx, Basic, Basic, Basic)

}  // namespace vixl
