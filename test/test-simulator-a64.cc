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
typedef void (MacroAssembler::*TestFixedToFPHelper_t)(const FPRegister& fd,
                                                      const Register& rn,
                                                      unsigned fbits);

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

  (masm.*helper)(fd, fn);
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

  if (Cctest::sim_test_trace()) {
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

  if (Cctest::sim_test_trace()) {
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

  if (Cctest::sim_test_trace()) {
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

  (masm.*helper)(fn, fm);
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

  if (Cctest::sim_test_trace()) {
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

  (masm.*helper)(fn, 0.0);
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

  if (Cctest::sim_test_trace()) {
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

  (masm.*helper)(rd, fn);
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

  if (Cctest::sim_test_trace()) {
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
        printf("  %" PRId64 ",\n", static_cast<int64_t>(results[d]));
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

  TestFPToInt_Helper(helper, reinterpret_cast<uintptr_t>(inputs), inputs_length,
                     reinterpret_cast<uintptr_t>(results), d_bits, n_bits);

  if (Cctest::sim_test_trace()) {
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


// Floating-point tests.


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

DEFINE_TEST_FP(fabs, 1Op, Basic)
DEFINE_TEST_FP(fmov, 1Op, Basic)
DEFINE_TEST_FP(fneg, 1Op, Basic)
DEFINE_TEST_FP(fsqrt, 1Op, Basic)
DEFINE_TEST_FP(frinta, 1Op, Conversions)
DEFINE_TEST_FP(frintn, 1Op, Conversions)
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
DEFINE_TEST_FP_TO_INT(fcvtzs, FPToS, Conversions)
DEFINE_TEST_FP_TO_INT(fcvtzu, FPToU, Conversions)

// TODO(jbramley): Scvtf-fixed-point
// TODO(jbramley): Scvtf-integer
// TODO(jbramley): Ucvtf-fixed-point
// TODO(jbramley): Ucvtf-integer

// TODO(jbramley): Fccmp
// TODO(jbramley): Fcsel

}  // namespace vixl
