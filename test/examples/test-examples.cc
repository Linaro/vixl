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

#include "vixl/a64/macro-assembler-a64.h"
#include "vixl/a64/debugger-a64.h"
#include "vixl/a64/simulator-a64.h"
#include "examples.h"
#include "non-const-visitor.h"
#include "custom-disassembler.h"
#include "../test-utils-a64.h"

#include "../test-runner.h"

#define TEST(name) TEST_(EXAMPLE_##name)

using namespace vixl;


TEST(custom_disassembler) {
  TestCustomDisassembler();
}


// The tests below only work with the simulator.
#ifdef USE_SIMULATOR

#define ARRAY_SIZE(Array) (sizeof(Array) / sizeof((Array)[0]))
#define BUF_SIZE (4096)
#define __ masm->

uint64_t FactorialC(uint64_t n) {
  uint64_t result = 1;

  while (n != 0) {
    result *= n;
    n--;
  }

  return result;
}

// Multiply two column-major 4x4 matrices of 32 bit floating point values.
// Return a column-major 4x4 matrix of 32 bit floating point values in 'C'.
void MatrixMultiplyC(float C[16], float A[16], float B[16]) {
  C[ 0] = A[ 0]*B[ 0] + A[ 4]*B[ 1] + A[ 8]*B[ 2] + A[12]*B[ 3];
  C[ 1] = A[ 1]*B[ 0] + A[ 5]*B[ 1] + A[ 9]*B[ 2] + A[13]*B[ 3];
  C[ 2] = A[ 2]*B[ 0] + A[ 6]*B[ 1] + A[10]*B[ 2] + A[14]*B[ 3];
  C[ 3] = A[ 3]*B[ 0] + A[ 7]*B[ 1] + A[11]*B[ 2] + A[15]*B[ 3];

  C[ 4] = A[ 0]*B[ 4] + A[ 4]*B[ 5] + A[ 8]*B[ 6] + A[12]*B[ 7];
  C[ 5] = A[ 1]*B[ 4] + A[ 5]*B[ 5] + A[ 9]*B[ 6] + A[13]*B[ 7];
  C[ 6] = A[ 2]*B[ 4] + A[ 6]*B[ 5] + A[10]*B[ 6] + A[14]*B[ 7];
  C[ 7] = A[ 3]*B[ 4] + A[ 7]*B[ 5] + A[11]*B[ 6] + A[15]*B[ 7];

  C[ 8] = A[ 0]*B[ 8] + A[ 4]*B[ 9] + A[ 8]*B[10] + A[12]*B[11];
  C[ 9] = A[ 1]*B[ 8] + A[ 5]*B[ 9] + A[ 9]*B[10] + A[13]*B[11];
  C[10] = A[ 2]*B[ 8] + A[ 6]*B[ 9] + A[10]*B[10] + A[14]*B[11];
  C[11] = A[ 3]*B[ 8] + A[ 7]*B[ 9] + A[11]*B[10] + A[15]*B[11];

  C[12] = A[ 0]*B[12] + A[ 4]*B[13] + A[ 8]*B[14] + A[12]*B[15];
  C[13] = A[ 1]*B[12] + A[ 5]*B[13] + A[ 9]*B[14] + A[13]*B[15];
  C[14] = A[ 2]*B[12] + A[ 6]*B[13] + A[10]*B[14] + A[14]*B[15];
  C[15] = A[ 3]*B[12] + A[ 7]*B[13] + A[11]*B[14] + A[15]*B[15];
}

double Add3DoubleC(double x, double y, double z) {
  return x + y + z;
}

double Add4DoubleC(uint64_t a, double b, uint64_t c, double d) {
  return static_cast<double>(a) + b + static_cast<double>(c) + d;
}

uint32_t SumArrayC(uint8_t* array, uint32_t size) {
  uint32_t result = 0;

  for (uint32_t i = 0; i < size; ++i) {
    result += array[i];
  }

  return result;
}


void GenerateTestWrapper(MacroAssembler* masm, RegisterDump *regs) {
  __ Push(xzr, lr);
  __ Blr(x15);
  regs->Dump(masm);
  __ Pop(lr, xzr);
  __ Ret();
}


#define TEST_FUNCTION(Func)                                             \
  do {                                                                  \
    int64_t saved_xregs[13];                                            \
    saved_xregs[0] = simulator.xreg(19);                                \
    saved_xregs[1] = simulator.xreg(20);                                \
    saved_xregs[2] = simulator.xreg(21);                                \
    saved_xregs[3] = simulator.xreg(22);                                \
    saved_xregs[4] = simulator.xreg(23);                                \
    saved_xregs[5] = simulator.xreg(24);                                \
    saved_xregs[6] = simulator.xreg(25);                                \
    saved_xregs[7] = simulator.xreg(26);                                \
    saved_xregs[8] = simulator.xreg(27);                                \
    saved_xregs[9] = simulator.xreg(28);                                \
    saved_xregs[10] = simulator.xreg(29);                               \
    saved_xregs[11] = simulator.xreg(30);                               \
    saved_xregs[12] = simulator.xreg(31);                               \
                                                                        \
    uint64_t saved_dregs[8];                                            \
    saved_dregs[0] = simulator.dreg_bits(8);                            \
    saved_dregs[1] = simulator.dreg_bits(9);                            \
    saved_dregs[2] = simulator.dreg_bits(10);                           \
    saved_dregs[3] = simulator.dreg_bits(11);                           \
    saved_dregs[4] = simulator.dreg_bits(12);                           \
    saved_dregs[5] = simulator.dreg_bits(13);                           \
    saved_dregs[6] = simulator.dreg_bits(14);                           \
    saved_dregs[7] = simulator.dreg_bits(15);                           \
                                                                        \
    simulator.set_xreg(15, masm.GetLabelAddress<uint64_t>(&Func));      \
    simulator.RunFrom(masm.GetLabelAddress<Instruction*>(&test));       \
                                                                        \
    assert(saved_xregs[0] == simulator.xreg(19));                       \
    assert(saved_xregs[1] == simulator.xreg(20));                       \
    assert(saved_xregs[2] == simulator.xreg(21));                       \
    assert(saved_xregs[3] == simulator.xreg(22));                       \
    assert(saved_xregs[4] == simulator.xreg(23));                       \
    assert(saved_xregs[5] == simulator.xreg(24));                       \
    assert(saved_xregs[6] == simulator.xreg(25));                       \
    assert(saved_xregs[7] == simulator.xreg(26));                       \
    assert(saved_xregs[8] == simulator.xreg(27));                       \
    assert(saved_xregs[9] == simulator.xreg(28));                       \
    assert(saved_xregs[10] == simulator.xreg(29));                      \
    assert(saved_xregs[11] == simulator.xreg(30));                      \
    assert(saved_xregs[12] == simulator.xreg(31));                      \
                                                                        \
    assert(saved_dregs[0] == simulator.dreg_bits(8));                   \
    assert(saved_dregs[1] == simulator.dreg_bits(9));                   \
    assert(saved_dregs[2] == simulator.dreg_bits(10));                  \
    assert(saved_dregs[3] == simulator.dreg_bits(11));                  \
    assert(saved_dregs[4] == simulator.dreg_bits(12));                  \
    assert(saved_dregs[5] == simulator.dreg_bits(13));                  \
    assert(saved_dregs[6] == simulator.dreg_bits(14));                  \
    assert(saved_dregs[7] == simulator.dreg_bits(15));                  \
                                                                        \
  } while (0)

#define START()                                             \
  MacroAssembler masm(BUF_SIZE);                            \
  Decoder decoder;                                          \
  Debugger simulator(&decoder);                             \
  simulator.set_coloured_trace(Test::coloured_trace());   \
  PrintDisassembler* pdis = NULL;                           \
  Instrument* inst = NULL;                                  \
  if (Test::trace_sim()) {                                \
    pdis = new PrintDisassembler(stdout);                   \
    decoder.PrependVisitor(pdis);                           \
  }                                                         \
  if (Test::instruction_stats()) {                        \
    inst = new Instrument("vixl_stats.csv", 10);            \
    inst->Enable();                                         \
    decoder.AppendVisitor(inst);                            \
  }                                                         \
  RegisterDump regs;                                        \
                                                            \
  Label test;                                               \
  masm.Bind(&test);                                         \
  GenerateTestWrapper(&masm, &regs);                        \
  masm.FinalizeCode()



#define FACTORIAL_DOTEST(N)                                             \
  do {                                                                  \
    simulator.ResetState();                                             \
    simulator.set_xreg(0, N);                                           \
    TEST_FUNCTION(factorial);                                           \
    assert(static_cast<uint64_t>(regs.xreg(0)) == FactorialC(N));       \
  } while (0)

TEST(factorial) {
  START();

  Label factorial;
  masm.Bind(&factorial);
  GenerateFactorial(&masm);
  masm.FinalizeCode();

  FACTORIAL_DOTEST(0);
  FACTORIAL_DOTEST(1);
  FACTORIAL_DOTEST(5);
  FACTORIAL_DOTEST(10);
  FACTORIAL_DOTEST(20);
  FACTORIAL_DOTEST(25);
}


#define FACTORIAL_REC_DOTEST(N)                                         \
  do {                                                                  \
    simulator.ResetState();                                             \
    simulator.set_xreg(0, N);                                           \
    TEST_FUNCTION(factorial_rec);                                       \
    assert(static_cast<uint64_t>(regs.xreg(0)) == FactorialC(N));       \
  } while (0)

TEST(factorial_rec) {
  START();

  Label factorial_rec;
  masm.Bind(&factorial_rec);
  GenerateFactorialRec(&masm);
  masm.FinalizeCode();

  FACTORIAL_REC_DOTEST(0);
  FACTORIAL_REC_DOTEST(1);
  FACTORIAL_REC_DOTEST(5);
  FACTORIAL_REC_DOTEST(10);
  FACTORIAL_REC_DOTEST(20);
  FACTORIAL_REC_DOTEST(25);
}

TEST(neon_matrix_multiply) {
  START();

  Label neon_matrix_multiply;
  masm.Bind(&neon_matrix_multiply);
  GenerateNEONMatrixMultiply(&masm);
  masm.FinalizeCode();

  {
    const int kRowSize = 4;
    const int kColSize = 4;
    const int kLength = kRowSize * kColSize;

    float mat1[kLength], mat2[kLength], expected[kLength], output[kLength];

    // Fill the two input matrices with some 32 bit floating point values.

    mat1[0] =   1.0f; mat1[4] =   2.0f; mat1[ 8] =   3.0f; mat1[12] =   4.0f;
    mat1[1] = 52.03f; mat1[5] = 12.24f; mat1[ 9] = 53.56f; mat1[13] = 22.22f;
    mat1[2] =  4.43f; mat1[6] =  5.00f; mat1[10] =  7.00f; mat1[14] =  3.11f;
    mat1[3] = 43.47f; mat1[7] = 10.97f; mat1[11] = 37.78f; mat1[15] = 90.91f;

    mat2[0] =   1.0f; mat2[4] = 11.24f; mat2[ 8] = 21.00f; mat2[12] = 21.31f;
    mat2[1] =   2.0f; mat2[5] =  2.24f; mat2[ 9] =  8.56f; mat2[13] = 52.03f;
    mat2[2] =   3.0f; mat2[6] = 51.00f; mat2[10] = 21.00f; mat2[14] = 33.11f;
    mat2[3] =   4.0f; mat2[7] =  0.00f; mat2[11] = 84.00f; mat2[15] =  1.97f;

    MatrixMultiplyC(expected, mat1, mat2);

    simulator.ResetState();
    simulator.set_xreg(0, reinterpret_cast<uintptr_t>(output));
    simulator.set_xreg(1, reinterpret_cast<uintptr_t>(mat1));
    simulator.set_xreg(2, reinterpret_cast<uintptr_t>(mat2));
    TEST_FUNCTION(neon_matrix_multiply);

    // Check that the results match what is expected.
    for (int i = 0; i < kLength; i++) {
      assert(output[i] == expected[i]);
    }
  }
}

TEST(add2_vectors) {
  START();

  // Create and initialize the assembler and the simulator.
  Label add2_vectors;
  masm.Bind(&add2_vectors);
  GenerateAdd2Vectors(&masm);
  masm.FinalizeCode();

  // Initialize input data for the example function.
  uint8_t A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 200};
  uint8_t B[] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, \
                 30, 31, 50};
  uint8_t D[ARRAY_SIZE(A)];
  uintptr_t A_addr = reinterpret_cast<uintptr_t>(A);
  uintptr_t B_addr = reinterpret_cast<uintptr_t>(B);

  // Check whether number of elements in vectors match.
  VIXL_STATIC_ASSERT(ARRAY_SIZE(A) == ARRAY_SIZE(B));
  VIXL_STATIC_ASSERT(ARRAY_SIZE(A) == ARRAY_SIZE(D));

  // Compute vector sum for comparison later.
  for (unsigned i = 0; i < ARRAY_SIZE(A); i++) {
    D[i] = A[i] + B[i];
  }

  // Set up simulator and run example function.
  simulator.ResetState();
  simulator.set_xreg(0, A_addr);
  simulator.set_xreg(1, B_addr);
  simulator.set_xreg(2, ARRAY_SIZE(A));
  TEST_FUNCTION(add2_vectors);

  // Compare vectors to ensure sums are equal.
  for (unsigned i = 0; i < ARRAY_SIZE(A); i++) {
    assert(A[i] == D[i]);
  }
}

#define ADD3_DOUBLE_DOTEST(A, B, C)                                     \
  do {                                                                  \
    simulator.ResetState();                                             \
    simulator.set_dreg(0, A);                                           \
    simulator.set_dreg(1, B);                                           \
    simulator.set_dreg(2, C);                                           \
    TEST_FUNCTION(add3_double);                                         \
    assert(regs.dreg(0) == Add3DoubleC(A, B, C));                       \
  } while (0)

TEST(add3_double) {
  START();

  Label add3_double;
  masm.Bind(&add3_double);
  GenerateAdd3Double(&masm);
  masm.FinalizeCode();

  ADD3_DOUBLE_DOTEST(0.0, 0.0, 0.0);
  ADD3_DOUBLE_DOTEST(457.698, 14.36, 2.00025);
  ADD3_DOUBLE_DOTEST(-45.55, -98.9, -0.354);
  ADD3_DOUBLE_DOTEST(.55, .9, .12);
}


#define ADD4_DOUBLE_DOTEST(A, B, C, D)                                  \
  do {                                                                  \
    simulator.ResetState();                                             \
    simulator.set_xreg(0, A);                                           \
    simulator.set_dreg(0, B);                                           \
    simulator.set_xreg(1, C);                                           \
    simulator.set_dreg(1, D);                                           \
    TEST_FUNCTION(add4_double);                                         \
    assert(regs.dreg(0) == Add4DoubleC(A, B, C, D));                    \
  } while (0)

TEST(add4_double) {
  START();

  Label add4_double;
  masm.Bind(&add4_double);
  GenerateAdd4Double(&masm);
  masm.FinalizeCode();

  ADD4_DOUBLE_DOTEST(0, 0, 0, 0);
  ADD4_DOUBLE_DOTEST(4, 3.287, 6, 13.48);
  ADD4_DOUBLE_DOTEST(56, 665.368, 0, -4932.4697);
  ADD4_DOUBLE_DOTEST(56, 0, 546, 0);
  ADD4_DOUBLE_DOTEST(0, 0.658, 0, 0.00000011540026);
}


#define SUM_ARRAY_DOTEST(Array)                                         \
  do {                                                                  \
    simulator.ResetState();                                             \
    uintptr_t addr = reinterpret_cast<uintptr_t>(Array);                \
    simulator.set_xreg(0, addr);                                        \
    simulator.set_xreg(1, ARRAY_SIZE(Array));                           \
    TEST_FUNCTION(sum_array);                                           \
    assert(regs.xreg(0) == SumArrayC(Array, ARRAY_SIZE(Array)));        \
  } while (0)

TEST(sum_array) {
  START();

  Label sum_array;
  masm.Bind(&sum_array);
  GenerateSumArray(&masm);
  masm.FinalizeCode();

  uint8_t data1[] = { 4, 9, 13, 3, 2, 6, 5 };
  SUM_ARRAY_DOTEST(data1);

  uint8_t data2[] = { 42 };
  SUM_ARRAY_DOTEST(data2);

  uint8_t data3[1000];
  for (unsigned int i = 0; i < ARRAY_SIZE(data3); ++i)
    data3[i] = 255;
  SUM_ARRAY_DOTEST(data3);
}


#define ABS_DOTEST(X)                                                   \
  do {                                                                  \
    simulator.ResetState();                                             \
    simulator.set_xreg(0, X);                                           \
    TEST_FUNCTION(func_abs);                                            \
    assert(regs.xreg(0) == abs(X));                                     \
  } while (0)

TEST(abs) {
  START();

  Label func_abs;
  masm.Bind(&func_abs);
  GenerateAbs(&masm);
  masm.FinalizeCode();

  ABS_DOTEST(-42);
  ABS_DOTEST(0);
  ABS_DOTEST(545);
  ABS_DOTEST(-428751489);
}


TEST(crc32) {
  START();

  Label crc32;
  masm.Bind(&crc32);
  GenerateCrc32(&masm);
  masm.FinalizeCode();

  const char *msg = "Hello World!";
  uintptr_t msg_addr = reinterpret_cast<uintptr_t>(msg);
  size_t msg_size = strlen(msg);
  int64_t chksum = INT64_C(0xe3d6e35c);
  simulator.set_xreg(0, msg_addr);
  simulator.set_xreg(1, msg_size);
  TEST_FUNCTION(crc32);
  assert(regs.xreg(0) == chksum);
}


TEST(swap4) {
  START();

  Label swap4;
  masm.Bind(&swap4);
  GenerateSwap4(&masm);
  masm.FinalizeCode();

  int64_t a = 15;
  int64_t b = 26;
  int64_t c = 46;
  int64_t d = 79;

  simulator.set_xreg(0, a);
  simulator.set_xreg(1, b);
  simulator.set_xreg(2, c);
  simulator.set_xreg(3, d);
  TEST_FUNCTION(swap4);
  assert(regs.xreg(0) == d);
  assert(regs.xreg(1) == c);
  assert(regs.xreg(2) == b);
  assert(regs.xreg(3) == a);
}


TEST(swap_int32) {
  START();

  Label swap_int32;
  masm.Bind(&swap_int32);
  GenerateSwapInt32(&masm);
  masm.FinalizeCode();

  int32_t x = 168;
  int32_t y = 246;
  simulator.set_wreg(0, x);
  simulator.set_wreg(1, y);
  TEST_FUNCTION(swap_int32);
  assert(regs.wreg(0) == y);
  assert(regs.wreg(1) == x);
}


#define CHECKBOUNDS_DOTEST(Value, Low, High)                            \
  do {                                                                  \
    simulator.ResetState();                                             \
    simulator.set_xreg(0, Value);                                       \
    simulator.set_xreg(1, Low);                                         \
    simulator.set_xreg(2, High);                                        \
    TEST_FUNCTION(check_bounds);                                        \
    assert(regs.xreg(0) == ((Low <= Value) && (Value <= High)));        \
  } while (0)

TEST(check_bounds) {
  START();

  Label check_bounds;
  masm.Bind(&check_bounds);
  GenerateCheckBounds(&masm);
  masm.FinalizeCode();

  CHECKBOUNDS_DOTEST(0, 100, 200);
  CHECKBOUNDS_DOTEST(58, 100, 200);
  CHECKBOUNDS_DOTEST(99, 100, 200);
  CHECKBOUNDS_DOTEST(100, 100, 200);
  CHECKBOUNDS_DOTEST(101, 100, 200);
  CHECKBOUNDS_DOTEST(150, 100, 200);
  CHECKBOUNDS_DOTEST(199, 100, 200);
  CHECKBOUNDS_DOTEST(200, 100, 200);
  CHECKBOUNDS_DOTEST(201, 100, 200);
}


#define GETTING_STARTED_DOTEST(Value)                           \
  do {                                                          \
    simulator.ResetState();                                     \
    simulator.set_xreg(0, Value);                               \
    TEST_FUNCTION(demo_function);                               \
    assert(regs.xreg(0) == (Value & 0x1122334455667788));       \
  } while (0)

TEST(getting_started) {
  START();

  Label demo_function;
  masm.Bind(&demo_function);
  GenerateDemoFunction(&masm);
  masm.FinalizeCode();

  GETTING_STARTED_DOTEST(0x8899aabbccddeeff);
  GETTING_STARTED_DOTEST(0x1122334455667788);
  GETTING_STARTED_DOTEST(0x0000000000000000);
  GETTING_STARTED_DOTEST(0xffffffffffffffff);
  GETTING_STARTED_DOTEST(0x5a5a5a5a5a5a5a5a);
}


TEST(non_const_visitor) {
  byte assm_buf[BUF_SIZE];
  MacroAssembler masm(assm_buf, BUF_SIZE);

  Label code_start, code_end;
  masm.Bind(&code_start);
  GenerateNonConstVisitorTestCode(&masm);
  masm.Bind(&code_end);
  masm.FinalizeCode();
  Instruction* instr_start = masm.GetLabelAddress<Instruction*>(&code_start);
  Instruction* instr_end = masm.GetLabelAddress<Instruction*>(&code_end);

  int64_t res_orig = RunNonConstVisitorTestGeneratedCode(instr_start);

  ModifyNonConstVisitorTestGeneratedCode(instr_start, instr_end);

  int64_t res_mod = RunNonConstVisitorTestGeneratedCode(instr_start);
  assert(res_orig == -res_mod);
}

#endif  // USE_SIMULATOR
