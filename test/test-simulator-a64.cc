// Copyright 2014, ARM Limited
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

#include "cctest.h"
#include "test-utils-a64.h"
#include "test-simulator-inputs-a64.h"
#include "test-simulator-traces-a64.h"
#include "a64/macro-assembler-a64.h"
#include "a64/simulator-a64.h"

namespace vixl {

// ==== Simulator Tests ====
//
// These simulator tests check instruction behaviour against a trace taken from
// real AArch64 hardware. The same test code is used to generate the trace; the
// results are printed to stdout when the test is run with --sim_test_trace.
//
// The input lists and expected results are stored in
// test/test-simulator-traces-a64.h. The expected results can be regenerated
// using tools/generate_simulator_traces.py.

#define __ masm.
#define TEST(name)  TEST_(SIM_##name)

#define BUF_SIZE (256)

#ifdef USE_SIMULATOR

#define SETUP()                                                               \
  byte* buf = new byte[BUF_SIZE];                                             \
  MacroAssembler masm(buf, BUF_SIZE);                                         \
  Decoder decoder;                                                            \
  Simulator* simulator = NULL;                                                \
  if (Cctest::run_debugger()) {                                               \
    simulator = new Debugger(&decoder);                                       \
  } else {                                                                    \
    simulator = new Simulator(&decoder);                                      \
    simulator->set_disasm_trace(Cctest::trace_sim());                         \
  }                                                                           \
  simulator->set_coloured_trace(Cctest::coloured_trace());                    \
  simulator->set_instruction_stats(Cctest::instruction_stats());

#define START()                                                               \
  masm.Reset();                                                               \
  simulator->ResetState();                                                    \
  __ PushCalleeSavedRegisters();                                              \
  if (Cctest::run_debugger()) {                                               \
    if (Cctest::trace_reg()) {                                                \
      __ Trace(LOG_STATE, TRACE_ENABLE);                                      \
    }                                                                         \
    if (Cctest::trace_sim()) {                                                \
      __ Trace(LOG_DISASM, TRACE_ENABLE);                                     \
    }                                                                         \
  }                                                                           \
  if (Cctest::instruction_stats()) {                                          \
    __ EnableInstrumentation();                                               \
  }

#define END()                                                                 \
  if (Cctest::instruction_stats()) {                                          \
    __ DisableInstrumentation();                                              \
  }                                                                           \
  if (Cctest::run_debugger()) {                                               \
    __ Trace(LOG_ALL, TRACE_DISABLE);                                         \
  }                                                                           \
  __ PopCalleeSavedRegisters();                                               \
  __ Ret();                                                                   \
  masm.FinalizeCode()

#define RUN()                                                                 \
  simulator->RunFrom(reinterpret_cast<Instruction*>(buf))

#define TEARDOWN()                                                            \
  delete simulator;                                                           \
  delete[] buf;

#else     // USE_SIMULATOR

#define SETUP()                                                               \
  byte* buf = new byte[BUF_SIZE];                                             \
  MacroAssembler masm(buf, BUF_SIZE);                                         \
  CPU::SetUp()

#define START()                                                               \
  masm.Reset();                                                               \
  __ PushCalleeSavedRegisters()

#define END()                                                                 \
  __ PopCalleeSavedRegisters();                                               \
  __ Ret();                                                                   \
  masm.FinalizeCode()

#define RUN()                                                                 \
  CPU::EnsureIAndDCacheCoherency(buf, BUF_SIZE);                              \
  {                                                                           \
    void (*test_function)(void);                                              \
    VIXL_ASSERT(sizeof(buf) == sizeof(test_function));                        \
    memcpy(&test_function, &buf, sizeof(buf));                                \
    test_function();                                                          \
  }

#define TEARDOWN()                                                            \
  delete[] buf;

#endif    // USE_SIMULATOR


// The maximum number of errors to report in detail for each test.
static const unsigned kErrorReportLimit = 8;


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

// Standard test dispatchers.


static void Test2Op_Helper(Test2OpFPHelper_t helper,
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

  (masm.*helper)(fd, fn, fm);
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


// Test FP instructions using doubles. The inputs[] and expected[] arrays should
// be arrays of rawbits representations of doubles. This ensures that exact bit
// comparisons can be performed.
static void Test2Op(const char * name, Test2OpFPHelper_t helper,
                    const uint64_t inputs[], unsigned inputs_length,
                    const uint64_t expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  static unsigned results_length = inputs_length * inputs_length;
  uint64_t * results = new uint64_t[results_length];

  Test2Op_Helper(helper, reinterpret_cast<uintptr_t>(inputs), inputs_length,
                 reinterpret_cast<uintptr_t>(results), kDRegSize);

  if (Cctest::sim_test_trace()) {
    // Print the results.
    printf("const uint64_t kExpected_%s[] = {\n", name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  0x%016" PRIx64 ",\n", results[d]);
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s =\n"
           "    sizeof(kExpected_%s) / sizeof(kExpected_%s[0]);\n",
           name, name, name);
  } else {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++) {
      for (unsigned m = 0; m < inputs_length; m++, d++) {
        if (results[d] != expected[d]) {
          if (++error_count > kErrorReportLimit) continue;

          printf("%s 0x%016" PRIx64 ", 0x%016" PRIx64 " (%s %g, %g):\n",
                 name, inputs[n], inputs[m],
                 name,
                 rawbits_to_double(inputs[n]),
                 rawbits_to_double(inputs[m]));
          printf("  Expected: 0x%016" PRIx64 " (%g)\n",
                 expected[d], rawbits_to_double(expected[d]));
          printf("  Found:    0x%016" PRIx64 " (%g)\n",
                 results[d], rawbits_to_double(results[d]));
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


// Test FP instructions using floats. The inputs[] and expected[] arrays should
// be arrays of rawbits representations of floats. This ensures that exact bit
// comparisons can be performed.
static void Test2Op(const char * name, Test2OpFPHelper_t helper,
                    const uint32_t inputs[], unsigned inputs_length,
                    const uint32_t expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  static unsigned results_length = inputs_length * inputs_length;
  uint32_t * results = new uint32_t[results_length];

  Test2Op_Helper(helper, reinterpret_cast<uintptr_t>(inputs), inputs_length,
                 reinterpret_cast<uintptr_t>(results), kSRegSize);

  if (Cctest::sim_test_trace()) {
    // Print the results.
    printf("const uint32_t kExpected_%s[] = {\n", name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  0x%08" PRIx32 ",\n", results[d]);
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s =\n"
           "    sizeof(kExpected_%s) / sizeof(kExpected_%s[0]);\n",
           name, name, name);
  } else {
    // Check the results.
    VIXL_CHECK(expected_length == results_length);
    unsigned error_count = 0;
    unsigned d = 0;
    for (unsigned n = 0; n < inputs_length; n++) {
      for (unsigned m = 0; m < inputs_length; m++, d++) {
        if (results[d] != expected[d]) {
          if (++error_count > kErrorReportLimit) continue;

          printf("%s 0x%08" PRIx32 ", 0x%08" PRIx32 " (%s %g, %g):\n",
                 name, inputs[n], inputs[m],
                 name,
                 rawbits_to_float(inputs[n]),
                 rawbits_to_float(inputs[m]));
          printf("  Expected: 0x%08" PRIx32 " (%g)\n",
                 expected[d], rawbits_to_float(expected[d]));
          printf("  Found:    0x%08" PRIx32 " (%g)\n",
                 results[d], rawbits_to_float(results[d]));
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

  (masm.*helper)(fd, fn, fm, fa);
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


// Test FP instructions using doubles. The inputs[] and expected[] arrays should
// be arrays of rawbits representations of doubles. This ensures that exact bit
// comparisons can be performed.
static void Test3Op(const char * name, Test3OpFPHelper_t helper,
                    const uint64_t inputs[], unsigned inputs_length,
                    const uint64_t expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  static unsigned results_length =
      inputs_length * inputs_length * inputs_length;
  uint64_t * results = new uint64_t[results_length];

  Test3Op_Helper(helper, reinterpret_cast<uintptr_t>(inputs), inputs_length,
                 reinterpret_cast<uintptr_t>(results), kDRegSize);

  if (Cctest::sim_test_trace()) {
    // Print the results.
    printf("const uint64_t kExpected_%s[] = {\n", name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  0x%016" PRIx64 ",\n", results[d]);
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s =\n"
           "    sizeof(kExpected_%s) / sizeof(kExpected_%s[0]);\n",
           name, name, name);
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

            printf("%s 0x%016" PRIx64 ", 0x%016" PRIx64 ", 0x%016" PRIx64 " "
                   "(%s %g, %g, %g):\n",
                   name, inputs[n], inputs[m], inputs[a],
                   name,
                   rawbits_to_double(inputs[n]),
                   rawbits_to_double(inputs[m]),
                   rawbits_to_double(inputs[a]));
            printf("  Expected: 0x%016" PRIx64 " (%g)\n",
                   expected[d], rawbits_to_double(expected[d]));
            printf("  Found:    0x%016" PRIx64 " (%g)\n",
                   results[d], rawbits_to_double(results[d]));
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


// Test FP instructions using floats. The inputs[] and expected[] arrays should
// be arrays of rawbits representations of floats. This ensures that exact bit
// comparisons can be performed.
static void Test3Op(const char * name, Test3OpFPHelper_t helper,
                    const uint32_t inputs[], unsigned inputs_length,
                    const uint32_t expected[], unsigned expected_length) {
  VIXL_ASSERT(inputs_length > 0);

  static unsigned results_length =
      inputs_length * inputs_length * inputs_length;
  uint32_t * results = new uint32_t[results_length];

  Test3Op_Helper(helper, reinterpret_cast<uintptr_t>(inputs), inputs_length,
                 reinterpret_cast<uintptr_t>(results), kSRegSize);

  if (Cctest::sim_test_trace()) {
    // Print the results.
    printf("const uint32_t kExpected_%s[] = {\n", name);
    for (unsigned d = 0; d < results_length; d++) {
      printf("  0x%08" PRIx32 ",\n", results[d]);
    }
    printf("};\n");
    printf("const unsigned kExpectedCount_%s =\n"
           "    sizeof(kExpected_%s) / sizeof(kExpected_%s[0]);\n",
           name, name, name);
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

            printf("%s 0x%08" PRIx32 ", 0x%08" PRIx32 ", 0x%08" PRIx32 " "
                   "(%s %g, %g, %g):\n",
                   name, inputs[n], inputs[m], inputs[a],
                   name,
                   rawbits_to_float(inputs[n]),
                   rawbits_to_float(inputs[m]),
                   rawbits_to_float(inputs[a]));
            printf("  Expected: 0x%08" PRIx32 " (%g)\n",
                   expected[d], rawbits_to_float(expected[d]));
            printf("  Found:    0x%08" PRIx32 " (%g)\n",
                   results[d], rawbits_to_float(results[d]));
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


// Floating-point tests.


// Standard floating-point test expansion for both double- and single-precision
// operations.
#define STRINGIFY(s) #s
#define DEFINE_TEST_FP(mnemonic, type)                  \
    TEST(mnemonic##_d) {                                \
      Test##type(STRINGIFY(mnemonic) "_d",              \
                 &MacroAssembler::mnemonic,             \
                 kInputDouble, kInputDoubleCount,       \
                 kExpected_##mnemonic##_d,              \
                 kExpectedCount_##mnemonic##_d);        \
    }                                                   \
                                                        \
    TEST(mnemonic##_s) {                                \
      Test##type(STRINGIFY(mnemonic) "_s",              \
                 &MacroAssembler::mnemonic,             \
                 kInputFloat, kInputFloatCount,         \
                 kExpected_##mnemonic##_s,              \
                 kExpectedCount_##mnemonic##_s);        \
    }


// TODO(jbramley): Fabs

DEFINE_TEST_FP(fadd, 2Op)

// TODO(jbramley): Fccmp
// TODO(jbramley): Fcmp
// TODO(jbramley): Fcsel
// TODO(jbramley): Fcvt
// TODO(jbramley): Fcvt-to-integer
// TODO(jbramley): Fcvt-to-fixed-point

DEFINE_TEST_FP(fdiv, 2Op)
DEFINE_TEST_FP(fmadd, 3Op)
DEFINE_TEST_FP(fmax, 2Op)
DEFINE_TEST_FP(fmaxnm, 2Op)
DEFINE_TEST_FP(fmin, 2Op)
DEFINE_TEST_FP(fminnm, 2Op)

// TODO(jbramley): Fmov

DEFINE_TEST_FP(fmsub, 3Op)
DEFINE_TEST_FP(fmul, 2Op)

// TODO(jbramley): Fneg

DEFINE_TEST_FP(fnmadd, 3Op)
DEFINE_TEST_FP(fnmsub, 3Op)

// TODO(jbramley): Frint-to-integer
// TODO(jbramley): Fsqrt

DEFINE_TEST_FP(fsub, 2Op)

// TODO(jbramley): Scvtf-fixed-point
// TODO(jbramley): Scvtf-integer
// TODO(jbramley): Ucvtf-fixed-point
// TODO(jbramley): Ucvtf-integer


}  // namespace vixl
