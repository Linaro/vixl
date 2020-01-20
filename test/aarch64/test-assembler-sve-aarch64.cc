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

#include <sys/mman.h>
#include <unistd.h>

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "test-runner.h"
#include "test-utils.h"
#include "aarch64/test-utils-aarch64.h"

#include "aarch64/cpu-aarch64.h"
#include "aarch64/disasm-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"
#include "aarch64/simulator-aarch64.h"
#include "test-assembler-aarch64.h"

namespace vixl {
namespace aarch64 {

Test* MakeSVETest(int vl, const char* name, Test::TestFunctionWithConfig* fn) {
  // We never free this memory, but we need it to live for as long as the static
  // linked list of tests, and this is the easiest way to do it.
  Test* test = new Test(name, fn);
  test->set_sve_vl_in_bits(vl);
  return test;
}

// The TEST_SVE macro works just like the usual TEST macro, but the resulting
// function receives a `const Test& config` argument, to allow it to query the
// vector length.
#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64
// On the Simulator, run SVE tests with several vector lengths, including the
// extreme values and an intermediate value that isn't a power of two.

#define TEST_SVE(name)                                                  \
  void Test##name(Test* config);                                        \
  Test* test_##name##_list[] =                                          \
      {MakeSVETest(128, "AARCH64_ASM_" #name "_vl128", &Test##name),    \
       MakeSVETest(384, "AARCH64_ASM_" #name "_vl384", &Test##name),    \
       MakeSVETest(2048, "AARCH64_ASM_" #name "_vl2048", &Test##name)}; \
  void Test##name(Test* config)

#define SVE_SETUP_WITH_FEATURES(...) \
  SETUP_WITH_FEATURES(__VA_ARGS__);  \
  simulator.SetVectorLengthInBits(config->sve_vl_in_bits())

#else
// Otherwise, just use whatever the hardware provides.
static const int kSVEVectorLengthInBits =
    CPUFeatures::InferFromOS().Has(CPUFeatures::kSVE)
        ? CPU::ReadSVEVectorLengthInBits()
        : 0;

#define TEST_SVE(name)                                                     \
  void Test##name(Test* config);                                           \
  Test* test_##name##_vlauto = MakeSVETest(kSVEVectorLengthInBits,         \
                                           "AARCH64_ASM_" #name "_vlauto", \
                                           &Test##name);                   \
  void Test##name(Test* config)

#define SVE_SETUP_WITH_FEATURES(...) \
  SETUP_WITH_FEATURES(__VA_ARGS__);  \
  USE(config)

#endif

// Call masm->Insr repeatedly to allow test inputs to be set up concisely. This
// is optimised for call-site clarity, not generated code quality, so it doesn't
// exist in the MacroAssembler itself.
//
// Usage:
//
//    int values[] = { 42, 43, 44 };
//    InsrHelper(&masm, z0.VnS(), values);    // Sets z0.S = { ..., 42, 43, 44 }
//
// The rightmost (highest-indexed) array element maps to the lowest-numbered
// lane.
template <typename T, size_t N>
void InsrHelper(MacroAssembler* masm,
                const ZRegister& zdn,
                const T (&values)[N]) {
  for (size_t i = 0; i < N; i++) {
    masm->Insr(zdn, values[i]);
  }
}

// Conveniently initialise P registers with scalar bit patterns. The destination
// lane size is ignored. This is optimised for call-site clarity, not generated
// code quality.
//
// Usage:
//
//    Initialise(&masm, p0, 0x1234);  // Sets p0 = 0b'0001'0010'0011'0100
void Initialise(MacroAssembler* masm,
                const PRegister& pd,
                uint64_t value3,
                uint64_t value2,
                uint64_t value1,
                uint64_t value0) {
  // Generate a literal pool, as in the array form.
  UseScratchRegisterScope temps(masm);
  Register temp = temps.AcquireX();
  Label data;
  Label done;

  masm->Adr(temp, &data);
  masm->Ldr(pd, SVEMemOperand(temp));
  masm->B(&done);
  {
    ExactAssemblyScope total(masm, kPRegMaxSizeInBytes);
    masm->bind(&data);
    masm->dc64(value0);
    masm->dc64(value1);
    masm->dc64(value2);
    masm->dc64(value3);
  }
  masm->Bind(&done);
}
void Initialise(MacroAssembler* masm,
                const PRegister& pd,
                uint64_t value2,
                uint64_t value1,
                uint64_t value0) {
  Initialise(masm, pd, 0, value2, value1, value0);
}
void Initialise(MacroAssembler* masm,
                const PRegister& pd,
                uint64_t value1,
                uint64_t value0) {
  Initialise(masm, pd, 0, 0, value1, value0);
}
void Initialise(MacroAssembler* masm, const PRegister& pd, uint64_t value0) {
  Initialise(masm, pd, 0, 0, 0, value0);
}

// Conveniently initialise P registers by lane. This is optimised for call-site
// clarity, not generated code quality.
//
// Usage:
//
//     int values[] = { 0x0, 0x1, 0x2 };
//     Initialise(&masm, p0.VnS(), values);  // Sets p0 = 0b'0000'0001'0010
//
// The rightmost (highest-indexed) array element maps to the lowest-numbered
// lane. Unspecified lanes are set to 0 (inactive).
//
// Each element of the `values` array is mapped onto a lane in `pd`. The
// architecture only respects the lower bit, and writes zero the upper bits, but
// other (encodable) values can be specified if required by the test.
template <typename T, size_t N>
void Initialise(MacroAssembler* masm,
                const PRegisterWithLaneSize& pd,
                const T (&values)[N]) {
  // Turn the array into 64-bit chunks.
  uint64_t chunks[4] = {0, 0, 0, 0};
  VIXL_STATIC_ASSERT(sizeof(chunks) == kPRegMaxSizeInBytes);

  int p_bits_per_lane = pd.GetLaneSizeInBits() / kZRegBitsPerPRegBit;
  VIXL_ASSERT((64 % p_bits_per_lane) == 0);
  VIXL_ASSERT((N * p_bits_per_lane) <= kPRegMaxSize);

  uint64_t p_lane_mask = GetUintMask(p_bits_per_lane);

  VIXL_STATIC_ASSERT(N <= kPRegMaxSize);
  size_t bit = 0;
  for (int n = static_cast<int>(N - 1); n >= 0; n--) {
    VIXL_ASSERT(bit < (sizeof(chunks) * kBitsPerByte));
    uint64_t value = values[n] & p_lane_mask;
    chunks[bit / 64] |= value << (bit % 64);
    bit += p_bits_per_lane;
  }

  Initialise(masm, pd, chunks[3], chunks[2], chunks[1], chunks[0]);
}

// Ensure that basic test infrastructure works.
TEST_SVE(sve_test_infrastructure_z) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  __ Mov(x0, 0x0123456789abcdef);

  // Test basic `Insr` behaviour.
  __ Insr(z0.VnB(), 1);
  __ Insr(z0.VnB(), 2);
  __ Insr(z0.VnB(), x0);
  __ Insr(z0.VnB(), -42);
  __ Insr(z0.VnB(), 0);

  // Test array inputs.
  int z1_inputs[] = {3, 4, 5, -42, 0};
  InsrHelper(&masm, z1.VnH(), z1_inputs);

  // Test that sign-extension works as intended for various lane sizes.
  __ Dup(z2.VnD(), 0);            // Clear the register first.
  __ Insr(z2.VnB(), -42);         //                       0xd6
  __ Insr(z2.VnB(), 0xfe);        //                       0xfe
  __ Insr(z2.VnH(), -42);         //                     0xffd6
  __ Insr(z2.VnH(), 0xfedc);      //                     0xfedc
  __ Insr(z2.VnS(), -42);         //                 0xffffffd6
  __ Insr(z2.VnS(), 0xfedcba98);  //                 0xfedcba98
  // Use another register for VnD(), so we can support 128-bit Z registers.
  __ Insr(z3.VnD(), -42);                 // 0xffffffffffffffd6
  __ Insr(z3.VnD(), 0xfedcba9876543210);  // 0xfedcba9876543210

  END();

  if (CAN_RUN()) {
    RUN();

    // Test that array checks work properly on a register initialised
    // lane-by-lane.
    int z0_inputs_b[] = {0x01, 0x02, 0xef, 0xd6, 0x00};
    ASSERT_EQUAL_SVE(z0_inputs_b, z0.VnB());

    // Test that lane-by-lane checks work properly on a register initialised
    // by array.
    for (size_t i = 0; i < ArrayLength(z1_inputs); i++) {
      // The rightmost (highest-indexed) array element maps to the
      // lowest-numbered lane.
      int lane = static_cast<int>(ArrayLength(z1_inputs) - i - 1);
      ASSERT_EQUAL_SVE_LANE(z1_inputs[i], z1.VnH(), lane);
    }

    uint64_t z2_inputs_d[] = {0x0000d6feffd6fedc, 0xffffffd6fedcba98};
    ASSERT_EQUAL_SVE(z2_inputs_d, z2.VnD());
    uint64_t z3_inputs_d[] = {0xffffffffffffffd6, 0xfedcba9876543210};
    ASSERT_EQUAL_SVE(z3_inputs_d, z3.VnD());
  }
}

// Ensure that basic test infrastructure works.
TEST_SVE(sve_test_infrastructure_p) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Simple cases: move boolean (0 or 1) values.

  int p0_inputs[] = {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0};
  Initialise(&masm, p0.VnB(), p0_inputs);

  int p1_inputs[] = {1, 0, 1, 1, 0, 1, 1, 1};
  Initialise(&masm, p1.VnH(), p1_inputs);

  int p2_inputs[] = {1, 1, 0, 1};
  Initialise(&masm, p2.VnS(), p2_inputs);

  int p3_inputs[] = {0, 1};
  Initialise(&masm, p3.VnD(), p3_inputs);

  // Advanced cases: move numeric value into architecturally-ignored bits.

  // B-sized lanes get one bit in a P register, so there are no ignored bits.

  // H-sized lanes get two bits in a P register.
  int p4_inputs[] = {0x3, 0x2, 0x1, 0x0, 0x1, 0x2, 0x3};
  Initialise(&masm, p4.VnH(), p4_inputs);

  // S-sized lanes get four bits in a P register.
  int p5_inputs[] = {0xc, 0x7, 0x9, 0x6, 0xf};
  Initialise(&masm, p5.VnS(), p5_inputs);

  // D-sized lanes get eight bits in a P register.
  int p6_inputs[] = {0x81, 0xcc, 0x55};
  Initialise(&masm, p6.VnD(), p6_inputs);

  // The largest possible P register has 32 bytes.
  int p7_inputs[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
                     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
                     0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f};
  Initialise(&masm, p7.VnD(), p7_inputs);

  END();

  if (CAN_RUN()) {
    RUN();

    // Test that lane-by-lane checks work properly. The rightmost
    // (highest-indexed) array element maps to the lowest-numbered lane.
    for (size_t i = 0; i < ArrayLength(p0_inputs); i++) {
      int lane = static_cast<int>(ArrayLength(p0_inputs) - i - 1);
      ASSERT_EQUAL_SVE_LANE(p0_inputs[i], p0.VnB(), lane);
    }
    for (size_t i = 0; i < ArrayLength(p1_inputs); i++) {
      int lane = static_cast<int>(ArrayLength(p1_inputs) - i - 1);
      ASSERT_EQUAL_SVE_LANE(p1_inputs[i], p1.VnH(), lane);
    }
    for (size_t i = 0; i < ArrayLength(p2_inputs); i++) {
      int lane = static_cast<int>(ArrayLength(p2_inputs) - i - 1);
      ASSERT_EQUAL_SVE_LANE(p2_inputs[i], p2.VnS(), lane);
    }
    for (size_t i = 0; i < ArrayLength(p3_inputs); i++) {
      int lane = static_cast<int>(ArrayLength(p3_inputs) - i - 1);
      ASSERT_EQUAL_SVE_LANE(p3_inputs[i], p3.VnD(), lane);
    }

    // Test that array checks work properly on predicates initialised with a
    // possibly-different lane size.
    // 0b...11'10'01'00'01'10'11
    int p4_expected[] = {0x39, 0x1b};
    ASSERT_EQUAL_SVE(p4_expected, p4.VnD());

    ASSERT_EQUAL_SVE(p5_inputs, p5.VnS());

    // 0b...10000001'11001100'01010101
    int p6_expected[] = {2, 0, 0, 1, 3, 0, 3, 0, 1, 1, 1, 1};
    ASSERT_EQUAL_SVE(p6_expected, p6.VnH());

    // 0b...10011100'10011101'10011110'10011111
    int p7_expected[] = {1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1,
                         1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1};
    ASSERT_EQUAL_SVE(p7_expected, p7.VnB());
  }
}

// Test that writes to V registers clear the high bits of the corresponding Z
// register.
TEST_SVE(sve_v_write_clear) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kNEON,
                          CPUFeatures::kFP,
                          CPUFeatures::kSVE);
  START();

  // The Simulator has two mechansisms for writing V registers:
  //  - Write*Register, calling through to SimRegisterBase::Write.
  //  - LogicVRegister::ClearForWrite followed by one or more lane updates.
  // Try to cover both variants.

  // Prepare some known inputs.
  uint8_t data[kQRegSizeInBytes];
  for (size_t i = 0; i < kQRegSizeInBytes; i++) {
    data[i] = 42 + i;
  }
  __ Mov(x10, reinterpret_cast<uintptr_t>(data));
  __ Fmov(d30, 42.0);

  // Use Index to label the lane indices, so failures are easy to detect and
  // diagnose.
  __ Index(z0.VnB(), 0, 1);
  __ Index(z1.VnB(), 0, 1);
  __ Index(z2.VnB(), 0, 1);
  __ Index(z3.VnB(), 0, 1);
  __ Index(z4.VnB(), 0, 1);

  __ Index(z10.VnB(), 0, -1);
  __ Index(z11.VnB(), 0, -1);
  __ Index(z12.VnB(), 0, -1);
  __ Index(z13.VnB(), 0, -1);
  __ Index(z14.VnB(), 0, -1);

  // Instructions using Write*Register (and SimRegisterBase::Write).
  __ Ldr(b0, MemOperand(x10));
  __ Fcvt(h1, d30);
  __ Fmov(s2, 1.5f);
  __ Fmov(d3, d30);
  __ Ldr(q4, MemOperand(x10));

  // Instructions using LogicVRegister::ClearForWrite.
  // These also (incidentally) test that across-lane instructions correctly
  // ignore the high-order Z register lanes.
  __ Sminv(b10, v10.V16B());
  __ Addv(h11, v11.V4H());
  __ Saddlv(s12, v12.V8H());
  __ Dup(v13.V8B(), b13, kDRegSizeInBytes);
  __ Uaddl(v14.V8H(), v14.V8B(), v14.V8B());

  END();

  if (CAN_RUN()) {
    RUN();

    // Check the Q part first.
    ASSERT_EQUAL_128(0x0000000000000000, 0x000000000000002a, v0);
    ASSERT_EQUAL_128(0x0000000000000000, 0x0000000000005140, v1);  // 42.0 (f16)
    ASSERT_EQUAL_128(0x0000000000000000, 0x000000003fc00000, v2);  // 1.5 (f32)
    ASSERT_EQUAL_128(0x0000000000000000, 0x4045000000000000, v3);  // 42.0 (f64)
    ASSERT_EQUAL_128(0x3938373635343332, 0x31302f2e2d2c2b2a, v4);
    ASSERT_EQUAL_128(0x0000000000000000, 0x00000000000000f1, v10);  // -15
    //  0xf9fa + 0xfbfc + 0xfdfe + 0xff00 -> 0xf2f4
    ASSERT_EQUAL_128(0x0000000000000000, 0x000000000000f2f4, v11);
    //  0xfffff1f2 + 0xfffff3f4 + ... + 0xfffffdfe + 0xffffff00 -> 0xffffc6c8
    ASSERT_EQUAL_128(0x0000000000000000, 0x00000000ffffc6c8, v12);
    ASSERT_EQUAL_128(0x0000000000000000, 0xf8f8f8f8f8f8f8f8, v13);  // [-8] x 8
    //    [0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff, 0x0000]
    //  + [0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff, 0x0000]
    // -> [0x01f2, 0x01f4, 0x01f6, 0x01f8, 0x01fa, 0x01fc, 0x01fe, 0x0000]
    ASSERT_EQUAL_128(0x01f201f401f601f8, 0x01fa01fc01fe0000, v14);

    // Check that the upper lanes are all clear.
    for (int i = kQRegSizeInBytes; i < core.GetSVELaneCount(kBRegSize); i++) {
      ASSERT_EQUAL_SVE_LANE(0x00, z0.VnB(), i);
      ASSERT_EQUAL_SVE_LANE(0x00, z1.VnB(), i);
      ASSERT_EQUAL_SVE_LANE(0x00, z2.VnB(), i);
      ASSERT_EQUAL_SVE_LANE(0x00, z3.VnB(), i);
      ASSERT_EQUAL_SVE_LANE(0x00, z4.VnB(), i);
      ASSERT_EQUAL_SVE_LANE(0x00, z10.VnB(), i);
      ASSERT_EQUAL_SVE_LANE(0x00, z11.VnB(), i);
      ASSERT_EQUAL_SVE_LANE(0x00, z12.VnB(), i);
      ASSERT_EQUAL_SVE_LANE(0x00, z13.VnB(), i);
      ASSERT_EQUAL_SVE_LANE(0x00, z14.VnB(), i);
    }
  }
}

static void MlaMlsHelper(Test* config, unsigned lane_size_in_bits) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int zd_inputs[] = {0xbb, 0xcc, 0xdd, 0xee};
  int za_inputs[] = {-39, 1, -3, 2};
  int zn_inputs[] = {-5, -20, 9, 8};
  int zm_inputs[] = {9, -5, 4, 5};

  ZRegister zd = z0.WithLaneSize(lane_size_in_bits);
  ZRegister za = z1.WithLaneSize(lane_size_in_bits);
  ZRegister zn = z2.WithLaneSize(lane_size_in_bits);
  ZRegister zm = z3.WithLaneSize(lane_size_in_bits);

  // TODO: Use a simple `Dup` once it accepts arbitrary immediates.
  InsrHelper(&masm, zd, zd_inputs);
  InsrHelper(&masm, za, za_inputs);
  InsrHelper(&masm, zn, zn_inputs);
  InsrHelper(&masm, zm, zm_inputs);

  int p0_inputs[] = {1, 1, 0, 1};
  int p1_inputs[] = {1, 0, 1, 1};
  int p2_inputs[] = {0, 1, 1, 1};
  int p3_inputs[] = {1, 1, 1, 0};

  Initialise(&masm, p0.WithLaneSize(lane_size_in_bits), p0_inputs);
  Initialise(&masm, p1.WithLaneSize(lane_size_in_bits), p1_inputs);
  Initialise(&masm, p2.WithLaneSize(lane_size_in_bits), p2_inputs);
  Initialise(&masm, p3.WithLaneSize(lane_size_in_bits), p3_inputs);

  // The Mla macro automatically selects between mla, mad and movprfx + mla
  // based on what registers are aliased.
  ZRegister mla_da_result = z10.WithLaneSize(lane_size_in_bits);
  ZRegister mla_dn_result = z11.WithLaneSize(lane_size_in_bits);
  ZRegister mla_dm_result = z12.WithLaneSize(lane_size_in_bits);
  ZRegister mla_d_result = z13.WithLaneSize(lane_size_in_bits);

  __ Mov(mla_da_result, za);
  __ Mla(mla_da_result, p0.Merging(), mla_da_result, zn, zm);

  __ Mov(mla_dn_result, zn);
  __ Mla(mla_dn_result, p1.Merging(), za, mla_dn_result, zm);

  __ Mov(mla_dm_result, zm);
  __ Mla(mla_dm_result, p2.Merging(), za, zn, mla_dm_result);

  __ Mov(mla_d_result, zd);
  __ Mla(mla_d_result, p3.Merging(), za, zn, zm);

  // The Mls macro automatically selects between mls, msb and movprfx + mls
  // based on what registers are aliased.
  ZRegister mls_da_result = z20.WithLaneSize(lane_size_in_bits);
  ZRegister mls_dn_result = z21.WithLaneSize(lane_size_in_bits);
  ZRegister mls_dm_result = z22.WithLaneSize(lane_size_in_bits);
  ZRegister mls_d_result = z23.WithLaneSize(lane_size_in_bits);

  __ Mov(mls_da_result, za);
  __ Mls(mls_da_result, p0.Merging(), mls_da_result, zn, zm);

  __ Mov(mls_dn_result, zn);
  __ Mls(mls_dn_result, p1.Merging(), za, mls_dn_result, zm);

  __ Mov(mls_dm_result, zm);
  __ Mls(mls_dm_result, p2.Merging(), za, zn, mls_dm_result);

  __ Mov(mls_d_result, zd);
  __ Mls(mls_d_result, p3.Merging(), za, zn, zm);

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(za_inputs, z1.WithLaneSize(lane_size_in_bits));
    ASSERT_EQUAL_SVE(zn_inputs, z2.WithLaneSize(lane_size_in_bits));
    ASSERT_EQUAL_SVE(zm_inputs, z3.WithLaneSize(lane_size_in_bits));

    int mla[] = {-84, 101, 33, 42};
    int mls[] = {6, -99, -39, -38};

    int mla_da_expected[] = {mla[0], mla[1], za_inputs[2], mla[3]};
    ASSERT_EQUAL_SVE(mla_da_expected, mla_da_result);

    int mla_dn_expected[] = {mla[0], zn_inputs[1], mla[2], mla[3]};
    ASSERT_EQUAL_SVE(mla_dn_expected, mla_dn_result);

    int mla_dm_expected[] = {zm_inputs[0], mla[1], mla[2], mla[3]};
    ASSERT_EQUAL_SVE(mla_dm_expected, mla_dm_result);

    int mla_d_expected[] = {mla[0], mla[1], mla[2], zd_inputs[3]};
    ASSERT_EQUAL_SVE(mla_d_expected, mla_d_result);

    int mls_da_expected[] = {mls[0], mls[1], za_inputs[2], mls[3]};
    ASSERT_EQUAL_SVE(mls_da_expected, mls_da_result);

    int mls_dn_expected[] = {mls[0], zn_inputs[1], mls[2], mls[3]};
    ASSERT_EQUAL_SVE(mls_dn_expected, mls_dn_result);

    int mls_dm_expected[] = {zm_inputs[0], mls[1], mls[2], mls[3]};
    ASSERT_EQUAL_SVE(mls_dm_expected, mls_dm_result);

    int mls_d_expected[] = {mls[0], mls[1], mls[2], zd_inputs[3]};
    ASSERT_EQUAL_SVE(mls_d_expected, mls_d_result);
  }
}

TEST_SVE(sve_mla_mls_b) { MlaMlsHelper(config, kBRegSize); }
TEST_SVE(sve_mla_mls_h) { MlaMlsHelper(config, kHRegSize); }
TEST_SVE(sve_mla_mls_s) { MlaMlsHelper(config, kSRegSize); }
TEST_SVE(sve_mla_mls_d) { MlaMlsHelper(config, kDRegSize); }

TEST_SVE(sve_bitwise_unpredicate_logical) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t z8_inputs[] = {0xfedcba9876543210, 0x0123456789abcdef};
  InsrHelper(&masm, z8.VnD(), z8_inputs);
  uint64_t z15_inputs[] = {0xffffeeeeddddcccc, 0xccccddddeeeeffff};
  InsrHelper(&masm, z15.VnD(), z15_inputs);

  __ And(z1.VnD(), z8.VnD(), z15.VnD());
  __ Bic(z2.VnD(), z8.VnD(), z15.VnD());
  __ Eor(z3.VnD(), z8.VnD(), z15.VnD());
  __ Orr(z4.VnD(), z8.VnD(), z15.VnD());

  END();

  if (CAN_RUN()) {
    RUN();
    uint64_t z1_expected[] = {0xfedcaa8854540000, 0x0000454588aacdef};
    uint64_t z2_expected[] = {0x0000101022003210, 0x0123002201010000};
    uint64_t z3_expected[] = {0x01235476ab89fedc, 0xcdef98ba67453210};
    uint64_t z4_expected[] = {0xfffffefeffddfedc, 0xcdefddffefefffff};

    ASSERT_EQUAL_SVE(z1_expected, z1.VnD());
    ASSERT_EQUAL_SVE(z2_expected, z2.VnD());
    ASSERT_EQUAL_SVE(z3_expected, z3.VnD());
    ASSERT_EQUAL_SVE(z4_expected, z4.VnD());
  }
}

TEST_SVE(sve_predicate_logical) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // 0b...01011010'10110111
  int p10_inputs[] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1};  // Pm
  // 0b...11011001'01010010
  int p11_inputs[] = {1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0};  // Pn
  // 0b...01010101'10110010
  int p12_inputs[] = {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0};  // pg

  Initialise(&masm, p10.VnB(), p10_inputs);
  Initialise(&masm, p11.VnB(), p11_inputs);
  Initialise(&masm, p12.VnB(), p12_inputs);

  __ Ands(p0.VnB(), p12.Zeroing(), p11.VnB(), p10.VnB());
  __ Mrs(x0, NZCV);
  __ Bics(p1.VnB(), p12.Zeroing(), p11.VnB(), p10.VnB());
  __ Mrs(x1, NZCV);
  __ Eor(p2.VnB(), p12.Zeroing(), p11.VnB(), p10.VnB());
  __ Nand(p3.VnB(), p12.Zeroing(), p11.VnB(), p10.VnB());
  __ Nor(p4.VnB(), p12.Zeroing(), p11.VnB(), p10.VnB());
  __ Orn(p5.VnB(), p12.Zeroing(), p11.VnB(), p10.VnB());
  __ Orr(p6.VnB(), p12.Zeroing(), p11.VnB(), p10.VnB());
  __ Sel(p7.VnB(), p12, p11.VnB(), p10.VnB());

  END();

  if (CAN_RUN()) {
    RUN();

    // 0b...01010000'00010010
    int p0_expected[] = {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0};
    // 0b...00000001'00000000
    int p1_expected[] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    // 0b...00000001'10100000
    int p2_expected[] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0};
    // 0b...00000101'10100000
    int p3_expected[] = {0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0};
    // 0b...00000100'00000000
    int p4_expected[] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // 0b...01010101'00010010
    int p5_expected[] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0};
    // 0b...01010001'10110010
    int p6_expected[] = {0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0};
    // 0b...01011011'00010111
    int p7_expected[] = {0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1};

    ASSERT_EQUAL_SVE(p0_expected, p0.VnB());
    ASSERT_EQUAL_SVE(p1_expected, p1.VnB());
    ASSERT_EQUAL_SVE(p2_expected, p2.VnB());
    ASSERT_EQUAL_SVE(p3_expected, p3.VnB());
    ASSERT_EQUAL_SVE(p4_expected, p4.VnB());
    ASSERT_EQUAL_SVE(p5_expected, p5.VnB());
    ASSERT_EQUAL_SVE(p6_expected, p6.VnB());
    ASSERT_EQUAL_SVE(p7_expected, p7.VnB());

    ASSERT_EQUAL_32(SVEFirstFlag, w0);
    ASSERT_EQUAL_32(SVENotLastFlag, w1);
  }
}

TEST_SVE(sve_int_compare_vectors) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int z10_inputs[] = {0x00, 0x80, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xff};
  int z11_inputs[] = {0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0x7f, 0xfe};
  int p0_inputs[] = {1, 0, 1, 1, 1, 1, 1, 1};
  InsrHelper(&masm, z10.VnB(), z10_inputs);
  InsrHelper(&masm, z11.VnB(), z11_inputs);
  Initialise(&masm, p0.VnB(), p0_inputs);

  __ Cmphs(p6.VnB(), p0.Zeroing(), z10.VnB(), z11.VnB());
  __ Mrs(x6, NZCV);

  uint64_t z12_inputs[] = {0xffffffffffffffff, 0x8000000000000000};
  uint64_t z13_inputs[] = {0x0000000000000000, 0x8000000000000000};
  int p1_inputs[] = {1, 1};
  InsrHelper(&masm, z12.VnD(), z12_inputs);
  InsrHelper(&masm, z13.VnD(), z13_inputs);
  Initialise(&masm, p1.VnD(), p1_inputs);

  __ Cmphi(p7.VnD(), p1.Zeroing(), z12.VnD(), z13.VnD());
  __ Mrs(x7, NZCV);

  int z14_inputs[] = {0, 32767, -1, -32767, 0, 0, 0, 32766};
  int z15_inputs[] = {0, 0, 0, 0, 32767, -1, -32767, 32767};

  int p2_inputs[] = {1, 0, 1, 1, 1, 1, 1, 1};
  InsrHelper(&masm, z14.VnH(), z14_inputs);
  InsrHelper(&masm, z15.VnH(), z15_inputs);
  Initialise(&masm, p2.VnH(), p2_inputs);

  __ Cmpge(p8.VnH(), p2.Zeroing(), z14.VnH(), z15.VnH());
  __ Mrs(x8, NZCV);

  __ Cmpeq(p9.VnH(), p2.Zeroing(), z14.VnH(), z15.VnH());
  __ Mrs(x9, NZCV);

  int z16_inputs[] = {0, -1, 0, 0};
  int z17_inputs[] = {0, 0, 2147483647, -2147483648};
  int p3_inputs[] = {1, 1, 1, 1};
  InsrHelper(&masm, z16.VnS(), z16_inputs);
  InsrHelper(&masm, z17.VnS(), z17_inputs);
  Initialise(&masm, p3.VnS(), p3_inputs);

  __ Cmpgt(p10.VnS(), p3.Zeroing(), z16.VnS(), z17.VnS());
  __ Mrs(x10, NZCV);

  __ Cmpne(p11.VnS(), p3.Zeroing(), z16.VnS(), z17.VnS());
  __ Mrs(x11, NZCV);

  // Architectural aliases testing.
  __ Cmpls(p12.VnB(), p0.Zeroing(), z11.VnB(), z10.VnB());  // HS
  __ Cmplo(p13.VnD(), p1.Zeroing(), z13.VnD(), z12.VnD());  // HI
  __ Cmple(p14.VnH(), p2.Zeroing(), z15.VnH(), z14.VnH());  // GE
  __ Cmplt(p15.VnS(), p3.Zeroing(), z17.VnS(), z16.VnS());  // GT

  END();

  if (CAN_RUN()) {
    RUN();

    int p6_expected[] = {1, 0, 1, 1, 0, 0, 0, 1};
    for (size_t i = 0; i < ArrayLength(p6_expected); i++) {
      int lane = static_cast<int>(ArrayLength(p6_expected) - i - 1);
      ASSERT_EQUAL_SVE_LANE(p6_expected[i], p6.VnB(), lane);
    }

    int p7_expected[] = {1, 0};
    ASSERT_EQUAL_SVE(p7_expected, p7.VnD());

    int p8_expected[] = {1, 0, 0, 0, 0, 1, 1, 0};
    ASSERT_EQUAL_SVE(p8_expected, p8.VnH());

    int p9_expected[] = {1, 0, 0, 0, 0, 0, 0, 0};
    ASSERT_EQUAL_SVE(p9_expected, p9.VnH());

    int p10_expected[] = {0, 0, 0, 1};
    ASSERT_EQUAL_SVE(p10_expected, p10.VnS());

    int p11_expected[] = {0, 1, 1, 1};
    ASSERT_EQUAL_SVE(p11_expected, p11.VnS());

    // Reuse the expected results to verify the architectural aliases.
    ASSERT_EQUAL_SVE(p6_expected, p12.VnB());
    ASSERT_EQUAL_SVE(p7_expected, p13.VnD());
    ASSERT_EQUAL_SVE(p8_expected, p14.VnH());
    ASSERT_EQUAL_SVE(p10_expected, p15.VnS());

    ASSERT_EQUAL_32(SVEFirstFlag, w6);
    ASSERT_EQUAL_32(NoFlag, w7);
    ASSERT_EQUAL_32(NoFlag, w8);
    ASSERT_EQUAL_32(NoFlag, w9);
    ASSERT_EQUAL_32(SVEFirstFlag | SVENotLastFlag, w10);
  }
}

TEST_SVE(sve_int_compare_vectors_wide_elements) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int src1_inputs_1[] = {0, 1, -1, -128, 127, 100, -66};
  int src2_inputs_1[] = {0, -1};
  int mask_inputs_1[] = {1, 1, 1, 1, 1, 0, 1};
  InsrHelper(&masm, z13.VnB(), src1_inputs_1);
  InsrHelper(&masm, z19.VnD(), src2_inputs_1);
  Initialise(&masm, p0.VnB(), mask_inputs_1);

  __ Cmpge(p2.VnB(), p0.Zeroing(), z13.VnB(), z19.VnD());
  __ Mrs(x2, NZCV);
  __ Cmpgt(p3.VnB(), p0.Zeroing(), z13.VnB(), z19.VnD());
  __ Mrs(x3, NZCV);

  int src1_inputs_2[] = {0, 32767, -1, -32767, 1, 1234, 0, 32766};
  int src2_inputs_2[] = {0, -32767};
  int mask_inputs_2[] = {1, 0, 1, 1, 1, 1, 1, 1};
  InsrHelper(&masm, z13.VnH(), src1_inputs_2);
  InsrHelper(&masm, z19.VnD(), src2_inputs_2);
  Initialise(&masm, p0.VnH(), mask_inputs_2);

  __ Cmple(p4.VnH(), p0.Zeroing(), z13.VnH(), z19.VnD());
  __ Mrs(x4, NZCV);
  __ Cmplt(p5.VnH(), p0.Zeroing(), z13.VnH(), z19.VnD());
  __ Mrs(x5, NZCV);

  int src1_inputs_3[] = {0, -1, 2147483647, -2147483648};
  int src2_inputs_3[] = {0, -2147483648};
  int mask_inputs_3[] = {1, 1, 1, 1};
  InsrHelper(&masm, z13.VnS(), src1_inputs_3);
  InsrHelper(&masm, z19.VnD(), src2_inputs_3);
  Initialise(&masm, p0.VnS(), mask_inputs_3);

  __ Cmpeq(p6.VnS(), p0.Zeroing(), z13.VnS(), z19.VnD());
  __ Mrs(x6, NZCV);
  __ Cmpne(p7.VnS(), p0.Zeroing(), z13.VnS(), z19.VnD());
  __ Mrs(x7, NZCV);

  int src1_inputs_4[] = {0x00, 0x80, 0x7f, 0xff, 0x7f, 0xf0, 0x0f, 0x55};
  int src2_inputs_4[] = {0x00, 0x7f};
  int mask_inputs_4[] = {1, 1, 1, 1, 0, 1, 1, 1};
  InsrHelper(&masm, z13.VnB(), src1_inputs_4);
  InsrHelper(&masm, z19.VnD(), src2_inputs_4);
  Initialise(&masm, p0.VnB(), mask_inputs_4);

  __ Cmplo(p8.VnB(), p0.Zeroing(), z13.VnB(), z19.VnD());
  __ Mrs(x8, NZCV);
  __ Cmpls(p9.VnB(), p0.Zeroing(), z13.VnB(), z19.VnD());
  __ Mrs(x9, NZCV);

  int src1_inputs_5[] = {0x0000, 0x8000, 0x7fff, 0xffff};
  int src2_inputs_5[] = {0x8000, 0xffff};
  int mask_inputs_5[] = {1, 1, 1, 1};
  InsrHelper(&masm, z13.VnS(), src1_inputs_5);
  InsrHelper(&masm, z19.VnD(), src2_inputs_5);
  Initialise(&masm, p0.VnS(), mask_inputs_5);

  __ Cmphi(p10.VnS(), p0.Zeroing(), z13.VnS(), z19.VnD());
  __ Mrs(x10, NZCV);
  __ Cmphs(p11.VnS(), p0.Zeroing(), z13.VnS(), z19.VnD());
  __ Mrs(x11, NZCV);

  END();

  if (CAN_RUN()) {
    RUN();
    int p2_expected[] = {1, 1, 1, 0, 1, 0, 0};
    ASSERT_EQUAL_SVE(p2_expected, p2.VnB());

    int p3_expected[] = {1, 1, 0, 0, 1, 0, 0};
    ASSERT_EQUAL_SVE(p3_expected, p3.VnB());

    int p4_expected[] = {0x1, 0x0, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0};
    ASSERT_EQUAL_SVE(p4_expected, p4.VnH());

    int p5_expected[] = {0x0, 0x0, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0};
    ASSERT_EQUAL_SVE(p5_expected, p5.VnH());

    int p6_expected[] = {0x1, 0x0, 0x0, 0x1};
    ASSERT_EQUAL_SVE(p6_expected, p6.VnS());

    int p7_expected[] = {0x0, 0x1, 0x1, 0x0};
    ASSERT_EQUAL_SVE(p7_expected, p7.VnS());

    int p8_expected[] = {1, 0, 0, 0, 0, 0, 1, 1};
    ASSERT_EQUAL_SVE(p8_expected, p8.VnB());

    int p9_expected[] = {1, 0, 1, 0, 0, 0, 1, 1};
    ASSERT_EQUAL_SVE(p9_expected, p9.VnB());

    int p10_expected[] = {0x0, 0x0, 0x0, 0x0};
    ASSERT_EQUAL_SVE(p10_expected, p10.VnS());

    int p11_expected[] = {0x0, 0x1, 0x0, 0x1};
    ASSERT_EQUAL_SVE(p11_expected, p11.VnS());

    ASSERT_EQUAL_32(NoFlag, w2);
    ASSERT_EQUAL_32(NoFlag, w3);
    ASSERT_EQUAL_32(NoFlag, w4);
    ASSERT_EQUAL_32(SVENotLastFlag, w5);
    ASSERT_EQUAL_32(SVEFirstFlag, w6);
    ASSERT_EQUAL_32(SVENotLastFlag, w7);
    ASSERT_EQUAL_32(SVEFirstFlag, w8);
    ASSERT_EQUAL_32(SVEFirstFlag, w9);
    ASSERT_EQUAL_32(SVENotLastFlag | SVENoneFlag, w10);
    ASSERT_EQUAL_32(SVENotLastFlag | SVEFirstFlag, w11);
  }
}

TEST_SVE(sve_bitwise_imm) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // clang-format off
  uint64_t z21_inputs[] = {0xfedcba9876543210, 0x0123456789abcdef};
  uint32_t z22_inputs[] = {0xfedcba98, 0x76543210, 0x01234567, 0x89abcdef};
  uint16_t z23_inputs[] = {0xfedc, 0xba98, 0x7654, 0x3210,
                           0x0123, 0x4567, 0x89ab, 0xcdef};
  uint8_t z24_inputs[] = {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
                          0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
  // clang-format on

  InsrHelper(&masm, z1.VnD(), z21_inputs);
  InsrHelper(&masm, z2.VnS(), z22_inputs);
  InsrHelper(&masm, z3.VnH(), z23_inputs);
  InsrHelper(&masm, z4.VnB(), z24_inputs);

  __ And(z1.VnD(), z1.VnD(), 0x0000ffff0000ffff);
  __ And(z2.VnS(), z2.VnS(), 0xff0000ff);
  __ And(z3.VnH(), z3.VnH(), 0x0ff0);
  __ And(z4.VnB(), z4.VnB(), 0x3f);

  InsrHelper(&masm, z5.VnD(), z21_inputs);
  InsrHelper(&masm, z6.VnS(), z22_inputs);
  InsrHelper(&masm, z7.VnH(), z23_inputs);
  InsrHelper(&masm, z8.VnB(), z24_inputs);

  __ Eor(z5.VnD(), z5.VnD(), 0x0000ffff0000ffff);
  __ Eor(z6.VnS(), z6.VnS(), 0xff0000ff);
  __ Eor(z7.VnH(), z7.VnH(), 0x0ff0);
  __ Eor(z8.VnB(), z8.VnB(), 0x3f);

  InsrHelper(&masm, z9.VnD(), z21_inputs);
  InsrHelper(&masm, z10.VnS(), z22_inputs);
  InsrHelper(&masm, z11.VnH(), z23_inputs);
  InsrHelper(&masm, z12.VnB(), z24_inputs);

  __ Orr(z9.VnD(), z9.VnD(), 0x0000ffff0000ffff);
  __ Orr(z10.VnS(), z10.VnS(), 0xff0000ff);
  __ Orr(z11.VnH(), z11.VnH(), 0x0ff0);
  __ Orr(z12.VnB(), z12.VnB(), 0x3f);

  {
    // The `Dup` macro maps onto either `dup` or `dupm`, but has its own test,
    // so here we test `dupm` directly.
    ExactAssemblyScope guard(&masm, 4 * kInstructionSize);
    __ dupm(z13.VnD(), 0x7ffffff800000000);
    __ dupm(z14.VnS(), 0x7ffc7ffc);
    __ dupm(z15.VnH(), 0x3ffc);
    __ dupm(z16.VnB(), 0xc3);
  }

  END();

  if (CAN_RUN()) {
    RUN();

    // clang-format off
    uint64_t z1_expected[] = {0x0000ba9800003210, 0x000045670000cdef};
    uint32_t z2_expected[] = {0xfe000098, 0x76000010, 0x01000067, 0x890000ef};
    uint16_t z3_expected[] = {0x0ed0, 0x0a90, 0x0650, 0x0210,
                              0x0120, 0x0560, 0x09a0, 0x0de0};
    uint8_t z4_expected[] = {0x3e, 0x1c, 0x3a, 0x18, 0x36, 0x14, 0x32, 0x10,
                             0x01, 0x23, 0x05, 0x27, 0x09, 0x2b, 0x0d, 0x2f};

    ASSERT_EQUAL_SVE(z1_expected, z1.VnD());
    ASSERT_EQUAL_SVE(z2_expected, z2.VnS());
    ASSERT_EQUAL_SVE(z3_expected, z3.VnH());
    ASSERT_EQUAL_SVE(z4_expected, z4.VnB());

    uint64_t z5_expected[] = {0xfedc45677654cdef, 0x0123ba9889ab3210};
    uint32_t z6_expected[] = {0x01dcba67, 0x895432ef, 0xfe234598, 0x76abcd10};
    uint16_t z7_expected[] = {0xf12c, 0xb568, 0x79a4, 0x3de0,
                              0x0ed3, 0x4a97, 0x865b, 0xc21f};
    uint8_t z8_expected[] = {0xc1, 0xe3, 0x85, 0xa7, 0x49, 0x6b, 0x0d, 0x2f,
                             0x3e, 0x1c, 0x7a, 0x58, 0xb6, 0x94, 0xf2, 0xd0};

    ASSERT_EQUAL_SVE(z5_expected, z5.VnD());
    ASSERT_EQUAL_SVE(z6_expected, z6.VnS());
    ASSERT_EQUAL_SVE(z7_expected, z7.VnH());
    ASSERT_EQUAL_SVE(z8_expected, z8.VnB());

    uint64_t z9_expected[] = {0xfedcffff7654ffff, 0x0123ffff89abffff};
    uint32_t z10_expected[] = {0xffdcbaff, 0xff5432ff,  0xff2345ff, 0xffabcdff};
    uint16_t z11_expected[] = {0xfffc, 0xbff8, 0x7ff4, 0x3ff0,
                               0x0ff3, 0x4ff7, 0x8ffb, 0xcfff};
    uint8_t z12_expected[] = {0xff, 0xff, 0xbf, 0xbf, 0x7f, 0x7f, 0x3f, 0x3f,
                              0x3f, 0x3f, 0x7f, 0x7f, 0xbf, 0xbf, 0xff, 0xff};

    ASSERT_EQUAL_SVE(z9_expected, z9.VnD());
    ASSERT_EQUAL_SVE(z10_expected, z10.VnS());
    ASSERT_EQUAL_SVE(z11_expected, z11.VnH());
    ASSERT_EQUAL_SVE(z12_expected, z12.VnB());

    uint64_t z13_expected[] = {0x7ffffff800000000, 0x7ffffff800000000};
    uint32_t z14_expected[] = {0x7ffc7ffc, 0x7ffc7ffc, 0x7ffc7ffc, 0x7ffc7ffc};
    uint16_t z15_expected[] = {0x3ffc, 0x3ffc, 0x3ffc, 0x3ffc,
                               0x3ffc, 0x3ffc, 0x3ffc ,0x3ffc};
    ASSERT_EQUAL_SVE(z13_expected, z13.VnD());
    ASSERT_EQUAL_SVE(z14_expected, z14.VnS());
    ASSERT_EQUAL_SVE(z15_expected, z15.VnH());
    // clang-format on
  }
}

TEST_SVE(sve_dup_imm) {
  // The `Dup` macro can generate `dup`, `dupm`, and it can synthesise
  // unencodable immediates.

  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Encodable with `dup` (shift 0).
  __ Dup(z0.VnD(), -1);
  __ Dup(z1.VnS(), 0x7f);
  __ Dup(z2.VnH(), -0x80);
  __ Dup(z3.VnB(), 42);

  // Encodable with `dup` (shift 8).
  __ Dup(z4.VnD(), -42 * 256);
  __ Dup(z5.VnS(), -0x8000);
  __ Dup(z6.VnH(), 0x7f00);
  // B-sized lanes cannot take a shift of 8.

  // Encodable with `dupm` (but not `dup`).
  __ Dup(z10.VnD(), 0x3fc);
  __ Dup(z11.VnS(), -516097);  // 0xfff81fff, as a signed int.
  __ Dup(z12.VnH(), 0x0001);
  // All values that fit B-sized lanes are encodable with `dup`.

  // Cases that require immediate synthesis.
  __ Dup(z20.VnD(), 0x1234);
  __ Dup(z21.VnD(), -4242);
  __ Dup(z22.VnD(), 0xfedcba9876543210);
  __ Dup(z23.VnS(), 0x01020304);
  __ Dup(z24.VnS(), -0x01020304);
  __ Dup(z25.VnH(), 0x3c38);
  // All values that fit B-sized lanes are directly encodable.

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(0xffffffffffffffff, z0.VnD());
    ASSERT_EQUAL_SVE(0x0000007f, z1.VnS());
    ASSERT_EQUAL_SVE(0xff80, z2.VnH());
    ASSERT_EQUAL_SVE(0x2a, z3.VnB());

    ASSERT_EQUAL_SVE(0xffffffffffffd600, z4.VnD());
    ASSERT_EQUAL_SVE(0xffff8000, z5.VnS());
    ASSERT_EQUAL_SVE(0x7f00, z6.VnH());

    ASSERT_EQUAL_SVE(0x00000000000003fc, z10.VnD());
    ASSERT_EQUAL_SVE(0xfff81fff, z11.VnS());
    ASSERT_EQUAL_SVE(0x0001, z12.VnH());

    ASSERT_EQUAL_SVE(0x1234, z20.VnD());
    ASSERT_EQUAL_SVE(0xffffffffffffef6e, z21.VnD());
    ASSERT_EQUAL_SVE(0xfedcba9876543210, z22.VnD());
    ASSERT_EQUAL_SVE(0x01020304, z23.VnS());
    ASSERT_EQUAL_SVE(0xfefdfcfc, z24.VnS());
    ASSERT_EQUAL_SVE(0x3c38, z25.VnH());
  }
}

TEST_SVE(sve_inc_dec_p_scalar) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int p0_inputs[] = {0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1};
  Initialise(&masm, p0.VnB(), p0_inputs);

  int p0_b_count = 9;
  int p0_h_count = 5;
  int p0_s_count = 3;
  int p0_d_count = 2;

  // 64-bit operations preserve their high bits.
  __ Mov(x0, 0x123456780000002a);
  __ Decp(x0, p0.VnB());

  __ Mov(x1, 0x123456780000002a);
  __ Incp(x1, p0.VnH());

  // Check that saturation does not occur.
  __ Mov(x10, 1);
  __ Decp(x10, p0.VnS());

  __ Mov(x11, UINT64_MAX);
  __ Incp(x11, p0.VnD());

  __ Mov(x12, INT64_MAX);
  __ Incp(x12, p0.VnB());

  // With an all-true predicate, these instructions increment or decrement by
  // the vector length.
  __ Ptrue(p15.VnB());

  __ Mov(x20, 0x4000000000000000);
  __ Decp(x20, p15.VnB());

  __ Mov(x21, 0x4000000000000000);
  __ Incp(x21, p15.VnH());

  END();
  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_64(0x123456780000002a - p0_b_count, x0);
    ASSERT_EQUAL_64(0x123456780000002a + p0_h_count, x1);

    ASSERT_EQUAL_64(UINT64_C(1) - p0_s_count, x10);
    ASSERT_EQUAL_64(UINT64_MAX + p0_d_count, x11);
    ASSERT_EQUAL_64(static_cast<uint64_t>(INT64_MAX) + p0_b_count, x12);

    ASSERT_EQUAL_64(0x4000000000000000 - core.GetSVELaneCount(kBRegSize), x20);
    ASSERT_EQUAL_64(0x4000000000000000 + core.GetSVELaneCount(kHRegSize), x21);
  }
}

TEST_SVE(sve_sqinc_sqdec_p_scalar) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int p0_inputs[] = {0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1};
  Initialise(&masm, p0.VnB(), p0_inputs);

  int p0_b_count = 9;
  int p0_h_count = 5;
  int p0_s_count = 3;
  int p0_d_count = 2;

  uint64_t dummy_high = 0x1234567800000000;

  // 64-bit operations preserve their high bits.
  __ Mov(x0, dummy_high + 42);
  __ Sqdecp(x0, p0.VnB());

  __ Mov(x1, dummy_high + 42);
  __ Sqincp(x1, p0.VnH());

  // 32-bit operations sign-extend into their high bits.
  __ Mov(x2, dummy_high + 42);
  __ Sqdecp(x2, p0.VnS(), w2);

  __ Mov(x3, dummy_high + 42);
  __ Sqincp(x3, p0.VnD(), w3);

  __ Mov(x4, dummy_high + 1);
  __ Sqdecp(x4, p0.VnS(), w4);

  __ Mov(x5, dummy_high - 1);
  __ Sqincp(x5, p0.VnD(), w5);

  // Check that saturation behaves correctly.
  __ Mov(x10, 0x8000000000000001);  // INT64_MIN + 1
  __ Sqdecp(x10, p0.VnB());

  __ Mov(x11, dummy_high + 0x80000001);  // INT32_MIN + 1
  __ Sqdecp(x11, p0.VnH(), w11);

  __ Mov(x12, 1);
  __ Sqdecp(x12, p0.VnS());

  __ Mov(x13, dummy_high + 1);
  __ Sqdecp(x13, p0.VnD(), w13);

  __ Mov(x14, 0x7ffffffffffffffe);  // INT64_MAX - 1
  __ Sqincp(x14, p0.VnB());

  __ Mov(x15, dummy_high + 0x7ffffffe);  // INT32_MAX - 1
  __ Sqincp(x15, p0.VnH(), w15);

  // Don't use x16 and x17 since they are scratch registers by default.

  __ Mov(x18, 0xffffffffffffffff);
  __ Sqincp(x18, p0.VnS());

  __ Mov(x19, dummy_high + 0xffffffff);
  __ Sqincp(x19, p0.VnD(), w19);

  __ Mov(x20, dummy_high + 0xffffffff);
  __ Sqdecp(x20, p0.VnB(), w20);

  // With an all-true predicate, these instructions increment or decrement by
  // the vector length.
  __ Ptrue(p15.VnB());

  __ Mov(x21, 0);
  __ Sqdecp(x21, p15.VnB());

  __ Mov(x22, 0);
  __ Sqincp(x22, p15.VnH());

  __ Mov(x23, dummy_high);
  __ Sqdecp(x23, p15.VnS(), w23);

  __ Mov(x24, dummy_high);
  __ Sqincp(x24, p15.VnD(), w24);

  END();
  if (CAN_RUN()) {
    RUN();

    // 64-bit operations preserve their high bits.
    ASSERT_EQUAL_64(dummy_high + 42 - p0_b_count, x0);
    ASSERT_EQUAL_64(dummy_high + 42 + p0_h_count, x1);

    // 32-bit operations sign-extend into their high bits.
    ASSERT_EQUAL_64(42 - p0_s_count, x2);
    ASSERT_EQUAL_64(42 + p0_d_count, x3);
    ASSERT_EQUAL_64(0xffffffff00000000 | (1 - p0_s_count), x4);
    ASSERT_EQUAL_64(p0_d_count - 1, x5);

    // Check that saturation behaves correctly.
    ASSERT_EQUAL_64(INT64_MIN, x10);
    ASSERT_EQUAL_64(INT32_MIN, x11);
    ASSERT_EQUAL_64(1 - p0_s_count, x12);
    ASSERT_EQUAL_64(1 - p0_d_count, x13);
    ASSERT_EQUAL_64(INT64_MAX, x14);
    ASSERT_EQUAL_64(INT32_MAX, x15);
    ASSERT_EQUAL_64(p0_s_count - 1, x18);
    ASSERT_EQUAL_64(p0_d_count - 1, x19);
    ASSERT_EQUAL_64(-1 - p0_b_count, x20);

    // Check all-true predicates.
    ASSERT_EQUAL_64(-core.GetSVELaneCount(kBRegSize), x21);
    ASSERT_EQUAL_64(core.GetSVELaneCount(kHRegSize), x22);
    ASSERT_EQUAL_64(-core.GetSVELaneCount(kSRegSize), x23);
    ASSERT_EQUAL_64(core.GetSVELaneCount(kDRegSize), x24);
  }
}

TEST_SVE(sve_uqinc_uqdec_p_scalar) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int p0_inputs[] = {0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1};
  Initialise(&masm, p0.VnB(), p0_inputs);

  int p0_b_count = 9;
  int p0_h_count = 5;
  int p0_s_count = 3;
  int p0_d_count = 2;

  uint64_t dummy_high = 0x1234567800000000;

  // 64-bit operations preserve their high bits.
  __ Mov(x0, dummy_high + 42);
  __ Uqdecp(x0, p0.VnB());

  __ Mov(x1, dummy_high + 42);
  __ Uqincp(x1, p0.VnH());

  // 32-bit operations zero-extend into their high bits.
  __ Mov(x2, dummy_high + 42);
  __ Uqdecp(x2, p0.VnS(), w2);

  __ Mov(x3, dummy_high + 42);
  __ Uqincp(x3, p0.VnD(), w3);

  __ Mov(x4, dummy_high + 0x80000001);
  __ Uqdecp(x4, p0.VnS(), w4);

  __ Mov(x5, dummy_high + 0x7fffffff);
  __ Uqincp(x5, p0.VnD(), w5);

  // Check that saturation behaves correctly.
  __ Mov(x10, 1);
  __ Uqdecp(x10, p0.VnB(), x10);

  __ Mov(x11, dummy_high + 1);
  __ Uqdecp(x11, p0.VnH(), w11);

  __ Mov(x12, 0x8000000000000000);  // INT64_MAX + 1
  __ Uqdecp(x12, p0.VnS(), x12);

  __ Mov(x13, dummy_high + 0x80000000);  // INT32_MAX + 1
  __ Uqdecp(x13, p0.VnD(), w13);

  __ Mov(x14, 0xfffffffffffffffe);  // UINT64_MAX - 1
  __ Uqincp(x14, p0.VnB(), x14);

  __ Mov(x15, dummy_high + 0xfffffffe);  // UINT32_MAX - 1
  __ Uqincp(x15, p0.VnH(), w15);

  // Don't use x16 and x17 since they are scratch registers by default.

  __ Mov(x18, 0x7ffffffffffffffe);  // INT64_MAX - 1
  __ Uqincp(x18, p0.VnS(), x18);

  __ Mov(x19, dummy_high + 0x7ffffffe);  // INT32_MAX - 1
  __ Uqincp(x19, p0.VnD(), w19);

  // With an all-true predicate, these instructions increment or decrement by
  // the vector length.
  __ Ptrue(p15.VnB());

  __ Mov(x20, 0x4000000000000000);
  __ Uqdecp(x20, p15.VnB(), x20);

  __ Mov(x21, 0x4000000000000000);
  __ Uqincp(x21, p15.VnH(), x21);

  __ Mov(x22, dummy_high + 0x40000000);
  __ Uqdecp(x22, p15.VnS(), w22);

  __ Mov(x23, dummy_high + 0x40000000);
  __ Uqincp(x23, p15.VnD(), w23);

  END();
  if (CAN_RUN()) {
    RUN();

    // 64-bit operations preserve their high bits.
    ASSERT_EQUAL_64(dummy_high + 42 - p0_b_count, x0);
    ASSERT_EQUAL_64(dummy_high + 42 + p0_h_count, x1);

    // 32-bit operations zero-extend into their high bits.
    ASSERT_EQUAL_64(42 - p0_s_count, x2);
    ASSERT_EQUAL_64(42 + p0_d_count, x3);
    ASSERT_EQUAL_64(UINT64_C(0x80000001) - p0_s_count, x4);
    ASSERT_EQUAL_64(UINT64_C(0x7fffffff) + p0_d_count, x5);

    // Check that saturation behaves correctly.
    ASSERT_EQUAL_64(0, x10);
    ASSERT_EQUAL_64(0, x11);
    ASSERT_EQUAL_64(0x8000000000000000 - p0_s_count, x12);
    ASSERT_EQUAL_64(UINT64_C(0x80000000) - p0_d_count, x13);
    ASSERT_EQUAL_64(UINT64_MAX, x14);
    ASSERT_EQUAL_64(UINT32_MAX, x15);
    ASSERT_EQUAL_64(0x7ffffffffffffffe + p0_s_count, x18);
    ASSERT_EQUAL_64(UINT64_C(0x7ffffffe) + p0_d_count, x19);

    // Check all-true predicates.
    ASSERT_EQUAL_64(0x4000000000000000 - core.GetSVELaneCount(kBRegSize), x20);
    ASSERT_EQUAL_64(0x4000000000000000 + core.GetSVELaneCount(kHRegSize), x21);
    ASSERT_EQUAL_64(0x40000000 - core.GetSVELaneCount(kSRegSize), x22);
    ASSERT_EQUAL_64(0x40000000 + core.GetSVELaneCount(kDRegSize), x23);
  }
}

TEST_SVE(sve_inc_dec_p_vector) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // There are {5, 3, 2} active {H, S, D} lanes. B-sized lanes are ignored.
  int p0_inputs[] = {0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1};
  Initialise(&masm, p0.VnB(), p0_inputs);

  // Check that saturation does not occur.

  int64_t z0_inputs[] = {0x1234567800000042, 0, 1, INT64_MIN};
  InsrHelper(&masm, z0.VnD(), z0_inputs);

  int64_t z1_inputs[] = {0x12345678ffffff2a, 0, -1, INT64_MAX};
  InsrHelper(&masm, z1.VnD(), z1_inputs);

  int32_t z2_inputs[] = {0x12340042, 0, -1, 1, INT32_MAX, INT32_MIN};
  InsrHelper(&masm, z2.VnS(), z2_inputs);

  int16_t z3_inputs[] = {0x122a, 0, 1, -1, INT16_MIN, INT16_MAX};
  InsrHelper(&masm, z3.VnH(), z3_inputs);

  // The MacroAssembler implements non-destructive operations using movprfx.
  __ Decp(z10.VnD(), p0, z0.VnD());
  __ Decp(z11.VnD(), p0, z1.VnD());
  __ Decp(z12.VnS(), p0, z2.VnS());
  __ Decp(z13.VnH(), p0, z3.VnH());

  __ Incp(z14.VnD(), p0, z0.VnD());
  __ Incp(z15.VnD(), p0, z1.VnD());
  __ Incp(z16.VnS(), p0, z2.VnS());
  __ Incp(z17.VnH(), p0, z3.VnH());

  // Also test destructive forms.
  __ Mov(z4, z0);
  __ Mov(z5, z1);
  __ Mov(z6, z2);
  __ Mov(z7, z3);

  __ Decp(z0.VnD(), p0);
  __ Decp(z1.VnD(), p0);
  __ Decp(z2.VnS(), p0);
  __ Decp(z3.VnH(), p0);

  __ Incp(z4.VnD(), p0);
  __ Incp(z5.VnD(), p0);
  __ Incp(z6.VnS(), p0);
  __ Incp(z7.VnH(), p0);

  END();
  if (CAN_RUN()) {
    RUN();

    // z0_inputs[...] - number of active D lanes (2)
    int64_t z0_expected[] = {0x1234567800000040, -2, -1, 0x7ffffffffffffffe};
    ASSERT_EQUAL_SVE(z0_expected, z0.VnD());

    // z1_inputs[...] - number of active D lanes (2)
    int64_t z1_expected[] = {0x12345678ffffff28, -2, -3, 0x7ffffffffffffffd};
    ASSERT_EQUAL_SVE(z1_expected, z1.VnD());

    // z2_inputs[...] - number of active S lanes (3)
    int32_t z2_expected[] = {0x1234003f, -3, -4, -2, 0x7ffffffc, 0x7ffffffd};
    ASSERT_EQUAL_SVE(z2_expected, z2.VnS());

    // z3_inputs[...] - number of active H lanes (5)
    int16_t z3_expected[] = {0x1225, -5, -4, -6, 0x7ffb, 0x7ffa};
    ASSERT_EQUAL_SVE(z3_expected, z3.VnH());

    // z0_inputs[...] + number of active D lanes (2)
    uint64_t z4_expected[] = {0x1234567800000044, 2, 3, 0x8000000000000002};
    ASSERT_EQUAL_SVE(z4_expected, z4.VnD());

    // z1_inputs[...] + number of active D lanes (2)
    uint64_t z5_expected[] = {0x12345678ffffff2c, 2, 1, 0x8000000000000001};
    ASSERT_EQUAL_SVE(z5_expected, z5.VnD());

    // z2_inputs[...] + number of active S lanes (3)
    uint32_t z6_expected[] = {0x12340045, 3, 2, 4, 0x80000002, 0x80000003};
    ASSERT_EQUAL_SVE(z6_expected, z6.VnS());

    // z3_inputs[...] + number of active H lanes (5)
    uint16_t z7_expected[] = {0x122f, 5, 6, 4, 0x8005, 0x8004};
    ASSERT_EQUAL_SVE(z7_expected, z7.VnH());

    // Check that the non-destructive macros produced the same results.
    ASSERT_EQUAL_SVE(z0_expected, z10.VnD());
    ASSERT_EQUAL_SVE(z1_expected, z11.VnD());
    ASSERT_EQUAL_SVE(z2_expected, z12.VnS());
    ASSERT_EQUAL_SVE(z3_expected, z13.VnH());
    ASSERT_EQUAL_SVE(z4_expected, z14.VnD());
    ASSERT_EQUAL_SVE(z5_expected, z15.VnD());
    ASSERT_EQUAL_SVE(z6_expected, z16.VnS());
    ASSERT_EQUAL_SVE(z7_expected, z17.VnH());
  }
}

TEST_SVE(sve_inc_dec_ptrue_vector) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // With an all-true predicate, these instructions increment or decrement by
  // the vector length.
  __ Ptrue(p15.VnB());

  __ Dup(z0.VnD(), 0);
  __ Decp(z0.VnD(), p15);

  __ Dup(z1.VnS(), 0);
  __ Decp(z1.VnS(), p15);

  __ Dup(z2.VnH(), 0);
  __ Decp(z2.VnH(), p15);

  __ Dup(z3.VnD(), 0);
  __ Incp(z3.VnD(), p15);

  __ Dup(z4.VnS(), 0);
  __ Incp(z4.VnS(), p15);

  __ Dup(z5.VnH(), 0);
  __ Incp(z5.VnH(), p15);

  END();
  if (CAN_RUN()) {
    RUN();

    int d_lane_count = core.GetSVELaneCount(kDRegSize);
    int s_lane_count = core.GetSVELaneCount(kSRegSize);
    int h_lane_count = core.GetSVELaneCount(kHRegSize);

    for (int i = 0; i < d_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(-d_lane_count, z0.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(d_lane_count, z3.VnD(), i);
    }

    for (int i = 0; i < s_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(-s_lane_count, z1.VnS(), i);
      ASSERT_EQUAL_SVE_LANE(s_lane_count, z4.VnS(), i);
    }

    for (int i = 0; i < h_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(-h_lane_count, z2.VnH(), i);
      ASSERT_EQUAL_SVE_LANE(h_lane_count, z5.VnH(), i);
    }
  }
}

TEST_SVE(sve_sqinc_sqdec_p_vector) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // There are {5, 3, 2} active {H, S, D} lanes. B-sized lanes are ignored.
  int p0_inputs[] = {0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1};
  Initialise(&masm, p0.VnB(), p0_inputs);

  // Check that saturation behaves correctly.

  int64_t z0_inputs[] = {0x1234567800000042, 0, 1, INT64_MIN};
  InsrHelper(&masm, z0.VnD(), z0_inputs);

  int64_t z1_inputs[] = {0x12345678ffffff2a, 0, -1, INT64_MAX};
  InsrHelper(&masm, z1.VnD(), z1_inputs);

  int32_t z2_inputs[] = {0x12340042, 0, -1, 1, INT32_MAX, INT32_MIN};
  InsrHelper(&masm, z2.VnS(), z2_inputs);

  int16_t z3_inputs[] = {0x122a, 0, 1, -1, INT16_MIN, INT16_MAX};
  InsrHelper(&masm, z3.VnH(), z3_inputs);

  // The MacroAssembler implements non-destructive operations using movprfx.
  __ Sqdecp(z10.VnD(), p0, z0.VnD());
  __ Sqdecp(z11.VnD(), p0, z1.VnD());
  __ Sqdecp(z12.VnS(), p0, z2.VnS());
  __ Sqdecp(z13.VnH(), p0, z3.VnH());

  __ Sqincp(z14.VnD(), p0, z0.VnD());
  __ Sqincp(z15.VnD(), p0, z1.VnD());
  __ Sqincp(z16.VnS(), p0, z2.VnS());
  __ Sqincp(z17.VnH(), p0, z3.VnH());

  // Also test destructive forms.
  __ Mov(z4, z0);
  __ Mov(z5, z1);
  __ Mov(z6, z2);
  __ Mov(z7, z3);

  __ Sqdecp(z0.VnD(), p0);
  __ Sqdecp(z1.VnD(), p0);
  __ Sqdecp(z2.VnS(), p0);
  __ Sqdecp(z3.VnH(), p0);

  __ Sqincp(z4.VnD(), p0);
  __ Sqincp(z5.VnD(), p0);
  __ Sqincp(z6.VnS(), p0);
  __ Sqincp(z7.VnH(), p0);

  END();
  if (CAN_RUN()) {
    RUN();

    // z0_inputs[...] - number of active D lanes (2)
    int64_t z0_expected[] = {0x1234567800000040, -2, -1, INT64_MIN};
    ASSERT_EQUAL_SVE(z0_expected, z0.VnD());

    // z1_inputs[...] - number of active D lanes (2)
    int64_t z1_expected[] = {0x12345678ffffff28, -2, -3, 0x7ffffffffffffffd};
    ASSERT_EQUAL_SVE(z1_expected, z1.VnD());

    // z2_inputs[...] - number of active S lanes (3)
    int32_t z2_expected[] = {0x1234003f, -3, -4, -2, 0x7ffffffc, INT32_MIN};
    ASSERT_EQUAL_SVE(z2_expected, z2.VnS());

    // z3_inputs[...] - number of active H lanes (5)
    int16_t z3_expected[] = {0x1225, -5, -4, -6, INT16_MIN, 0x7ffa};
    ASSERT_EQUAL_SVE(z3_expected, z3.VnH());

    // z0_inputs[...] + number of active D lanes (2)
    uint64_t z4_expected[] = {0x1234567800000044, 2, 3, 0x8000000000000002};
    ASSERT_EQUAL_SVE(z4_expected, z4.VnD());

    // z1_inputs[...] + number of active D lanes (2)
    uint64_t z5_expected[] = {0x12345678ffffff2c, 2, 1, INT64_MAX};
    ASSERT_EQUAL_SVE(z5_expected, z5.VnD());

    // z2_inputs[...] + number of active S lanes (3)
    uint32_t z6_expected[] = {0x12340045, 3, 2, 4, INT32_MAX, 0x80000003};
    ASSERT_EQUAL_SVE(z6_expected, z6.VnS());

    // z3_inputs[...] + number of active H lanes (5)
    uint16_t z7_expected[] = {0x122f, 5, 6, 4, 0x8005, INT16_MAX};
    ASSERT_EQUAL_SVE(z7_expected, z7.VnH());

    // Check that the non-destructive macros produced the same results.
    ASSERT_EQUAL_SVE(z0_expected, z10.VnD());
    ASSERT_EQUAL_SVE(z1_expected, z11.VnD());
    ASSERT_EQUAL_SVE(z2_expected, z12.VnS());
    ASSERT_EQUAL_SVE(z3_expected, z13.VnH());
    ASSERT_EQUAL_SVE(z4_expected, z14.VnD());
    ASSERT_EQUAL_SVE(z5_expected, z15.VnD());
    ASSERT_EQUAL_SVE(z6_expected, z16.VnS());
    ASSERT_EQUAL_SVE(z7_expected, z17.VnH());
  }
}

TEST_SVE(sve_sqinc_sqdec_ptrue_vector) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // With an all-true predicate, these instructions increment or decrement by
  // the vector length.
  __ Ptrue(p15.VnB());

  __ Dup(z0.VnD(), 0);
  __ Sqdecp(z0.VnD(), p15);

  __ Dup(z1.VnS(), 0);
  __ Sqdecp(z1.VnS(), p15);

  __ Dup(z2.VnH(), 0);
  __ Sqdecp(z2.VnH(), p15);

  __ Dup(z3.VnD(), 0);
  __ Sqincp(z3.VnD(), p15);

  __ Dup(z4.VnS(), 0);
  __ Sqincp(z4.VnS(), p15);

  __ Dup(z5.VnH(), 0);
  __ Sqincp(z5.VnH(), p15);

  END();
  if (CAN_RUN()) {
    RUN();

    int d_lane_count = core.GetSVELaneCount(kDRegSize);
    int s_lane_count = core.GetSVELaneCount(kSRegSize);
    int h_lane_count = core.GetSVELaneCount(kHRegSize);

    for (int i = 0; i < d_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(-d_lane_count, z0.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(d_lane_count, z3.VnD(), i);
    }

    for (int i = 0; i < s_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(-s_lane_count, z1.VnS(), i);
      ASSERT_EQUAL_SVE_LANE(s_lane_count, z4.VnS(), i);
    }

    for (int i = 0; i < h_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(-h_lane_count, z2.VnH(), i);
      ASSERT_EQUAL_SVE_LANE(h_lane_count, z5.VnH(), i);
    }
  }
}

TEST_SVE(sve_uqinc_uqdec_p_vector) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // There are {5, 3, 2} active {H, S, D} lanes. B-sized lanes are ignored.
  int p0_inputs[] = {0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1};
  Initialise(&masm, p0.VnB(), p0_inputs);

  // Check that saturation behaves correctly.

  uint64_t z0_inputs[] = {0x1234567800000042, 0, 1, 0x8000000000000000};
  InsrHelper(&masm, z0.VnD(), z0_inputs);

  uint64_t z1_inputs[] = {0x12345678ffffff2a, 0, UINT64_MAX, INT64_MAX};
  InsrHelper(&masm, z1.VnD(), z1_inputs);

  uint32_t z2_inputs[] = {0x12340042, 0, UINT32_MAX, 1, INT32_MAX, 0x80000000};
  InsrHelper(&masm, z2.VnS(), z2_inputs);

  uint16_t z3_inputs[] = {0x122a, 0, 1, UINT16_MAX, 0x8000, INT16_MAX};
  InsrHelper(&masm, z3.VnH(), z3_inputs);

  // The MacroAssembler implements non-destructive operations using movprfx.
  __ Uqdecp(z10.VnD(), p0, z0.VnD());
  __ Uqdecp(z11.VnD(), p0, z1.VnD());
  __ Uqdecp(z12.VnS(), p0, z2.VnS());
  __ Uqdecp(z13.VnH(), p0, z3.VnH());

  __ Uqincp(z14.VnD(), p0, z0.VnD());
  __ Uqincp(z15.VnD(), p0, z1.VnD());
  __ Uqincp(z16.VnS(), p0, z2.VnS());
  __ Uqincp(z17.VnH(), p0, z3.VnH());

  // Also test destructive forms.
  __ Mov(z4, z0);
  __ Mov(z5, z1);
  __ Mov(z6, z2);
  __ Mov(z7, z3);

  __ Uqdecp(z0.VnD(), p0);
  __ Uqdecp(z1.VnD(), p0);
  __ Uqdecp(z2.VnS(), p0);
  __ Uqdecp(z3.VnH(), p0);

  __ Uqincp(z4.VnD(), p0);
  __ Uqincp(z5.VnD(), p0);
  __ Uqincp(z6.VnS(), p0);
  __ Uqincp(z7.VnH(), p0);

  END();
  if (CAN_RUN()) {
    RUN();

    // z0_inputs[...] - number of active D lanes (2)
    uint64_t z0_expected[] = {0x1234567800000040, 0, 0, 0x7ffffffffffffffe};
    ASSERT_EQUAL_SVE(z0_expected, z0.VnD());

    // z1_inputs[...] - number of active D lanes (2)
    uint64_t z1_expected[] = {0x12345678ffffff28,
                              0,
                              0xfffffffffffffffd,
                              0x7ffffffffffffffd};
    ASSERT_EQUAL_SVE(z1_expected, z1.VnD());

    // z2_inputs[...] - number of active S lanes (3)
    uint32_t z2_expected[] =
        {0x1234003f, 0, 0xfffffffc, 0, 0x7ffffffc, 0x7ffffffd};
    ASSERT_EQUAL_SVE(z2_expected, z2.VnS());

    // z3_inputs[...] - number of active H lanes (5)
    uint16_t z3_expected[] = {0x1225, 0, 0, 0xfffa, 0x7ffb, 0x7ffa};
    ASSERT_EQUAL_SVE(z3_expected, z3.VnH());

    // z0_inputs[...] + number of active D lanes (2)
    uint64_t z4_expected[] = {0x1234567800000044, 2, 3, 0x8000000000000002};
    ASSERT_EQUAL_SVE(z4_expected, z4.VnD());

    // z1_inputs[...] + number of active D lanes (2)
    uint64_t z5_expected[] = {0x12345678ffffff2c,
                              2,
                              UINT64_MAX,
                              0x8000000000000001};
    ASSERT_EQUAL_SVE(z5_expected, z5.VnD());

    // z2_inputs[...] + number of active S lanes (3)
    uint32_t z6_expected[] =
        {0x12340045, 3, UINT32_MAX, 4, 0x80000002, 0x80000003};
    ASSERT_EQUAL_SVE(z6_expected, z6.VnS());

    // z3_inputs[...] + number of active H lanes (5)
    uint16_t z7_expected[] = {0x122f, 5, 6, UINT16_MAX, 0x8005, 0x8004};
    ASSERT_EQUAL_SVE(z7_expected, z7.VnH());

    // Check that the non-destructive macros produced the same results.
    ASSERT_EQUAL_SVE(z0_expected, z10.VnD());
    ASSERT_EQUAL_SVE(z1_expected, z11.VnD());
    ASSERT_EQUAL_SVE(z2_expected, z12.VnS());
    ASSERT_EQUAL_SVE(z3_expected, z13.VnH());
    ASSERT_EQUAL_SVE(z4_expected, z14.VnD());
    ASSERT_EQUAL_SVE(z5_expected, z15.VnD());
    ASSERT_EQUAL_SVE(z6_expected, z16.VnS());
    ASSERT_EQUAL_SVE(z7_expected, z17.VnH());
  }
}

TEST_SVE(sve_uqinc_uqdec_ptrue_vector) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // With an all-true predicate, these instructions increment or decrement by
  // the vector length.
  __ Ptrue(p15.VnB());

  __ Mov(x0, 0x1234567800000000);
  __ Mov(x1, 0x12340000);
  __ Mov(x2, 0x1200);

  __ Dup(z0.VnD(), x0);
  __ Uqdecp(z0.VnD(), p15);

  __ Dup(z1.VnS(), x1);
  __ Uqdecp(z1.VnS(), p15);

  __ Dup(z2.VnH(), x2);
  __ Uqdecp(z2.VnH(), p15);

  __ Dup(z3.VnD(), x0);
  __ Uqincp(z3.VnD(), p15);

  __ Dup(z4.VnS(), x1);
  __ Uqincp(z4.VnS(), p15);

  __ Dup(z5.VnH(), x2);
  __ Uqincp(z5.VnH(), p15);

  END();
  if (CAN_RUN()) {
    RUN();

    int d_lane_count = core.GetSVELaneCount(kDRegSize);
    int s_lane_count = core.GetSVELaneCount(kSRegSize);
    int h_lane_count = core.GetSVELaneCount(kHRegSize);

    for (int i = 0; i < d_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(0x1234567800000000 - d_lane_count, z0.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0x1234567800000000 + d_lane_count, z3.VnD(), i);
    }

    for (int i = 0; i < s_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(0x12340000 - s_lane_count, z1.VnS(), i);
      ASSERT_EQUAL_SVE_LANE(0x12340000 + s_lane_count, z4.VnS(), i);
    }

    for (int i = 0; i < h_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(0x1200 - h_lane_count, z2.VnH(), i);
      ASSERT_EQUAL_SVE_LANE(0x1200 + h_lane_count, z5.VnH(), i);
    }
  }
}

TEST_SVE(sve_index) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Simple cases.
  __ Index(z0.VnB(), 0, 1);
  __ Index(z1.VnH(), 1, 1);
  __ Index(z2.VnS(), 2, 1);
  __ Index(z3.VnD(), 3, 1);

  // Synthesised immediates.
  __ Index(z4.VnB(), 42, -1);
  __ Index(z5.VnH(), -1, 42);
  __ Index(z6.VnS(), 42, 42);

  // Register arguments.
  __ Mov(x0, 42);
  __ Mov(x1, -3);
  __ Index(z10.VnD(), x0, x1);
  __ Index(z11.VnB(), w0, w1);
  // The register size should correspond to the lane size, but VIXL allows any
  // register at least as big as the lane size.
  __ Index(z12.VnB(), x0, x1);
  __ Index(z13.VnH(), w0, x1);
  __ Index(z14.VnS(), x0, w1);

  // Integer overflow.
  __ Index(z20.VnB(), UINT8_MAX - 2, 2);
  __ Index(z21.VnH(), 7, -3);
  __ Index(z22.VnS(), INT32_MAX - 2, 1);
  __ Index(z23.VnD(), INT64_MIN + 6, -7);

  END();

  if (CAN_RUN()) {
    RUN();

    int b_lane_count = core.GetSVELaneCount(kBRegSize);
    int h_lane_count = core.GetSVELaneCount(kHRegSize);
    int s_lane_count = core.GetSVELaneCount(kSRegSize);
    int d_lane_count = core.GetSVELaneCount(kDRegSize);

    uint64_t b_mask = GetUintMask(kBRegSize);
    uint64_t h_mask = GetUintMask(kHRegSize);
    uint64_t s_mask = GetUintMask(kSRegSize);
    uint64_t d_mask = GetUintMask(kDRegSize);

    // Simple cases.
    for (int i = 0; i < b_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE((0 + i) & b_mask, z0.VnB(), i);
    }
    for (int i = 0; i < h_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE((1 + i) & h_mask, z1.VnH(), i);
    }
    for (int i = 0; i < s_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE((2 + i) & s_mask, z2.VnS(), i);
    }
    for (int i = 0; i < d_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE((3 + i) & d_mask, z3.VnD(), i);
    }

    // Synthesised immediates.
    for (int i = 0; i < b_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE((42 - i) & b_mask, z4.VnB(), i);
    }
    for (int i = 0; i < h_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE((-1 + (42 * i)) & h_mask, z5.VnH(), i);
    }
    for (int i = 0; i < s_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE((42 + (42 * i)) & s_mask, z6.VnS(), i);
    }

    // Register arguments.
    for (int i = 0; i < d_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE((42 - (3 * i)) & d_mask, z10.VnD(), i);
    }
    for (int i = 0; i < b_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE((42 - (3 * i)) & b_mask, z11.VnB(), i);
    }
    for (int i = 0; i < b_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE((42 - (3 * i)) & b_mask, z12.VnB(), i);
    }
    for (int i = 0; i < h_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE((42 - (3 * i)) & h_mask, z13.VnH(), i);
    }
    for (int i = 0; i < s_lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE((42 - (3 * i)) & s_mask, z14.VnS(), i);
    }

    // Integer overflow.
    uint8_t expected_z20[] = {0x05, 0x03, 0x01, 0xff, 0xfd};
    ASSERT_EQUAL_SVE(expected_z20, z20.VnB());
    uint16_t expected_z21[] = {0xfffb, 0xfffe, 0x0001, 0x0004, 0x0007};
    ASSERT_EQUAL_SVE(expected_z21, z21.VnH());
    uint32_t expected_z22[] = {0x80000000, 0x7fffffff, 0x7ffffffe, 0x7ffffffd};
    ASSERT_EQUAL_SVE(expected_z22, z22.VnS());
    uint64_t expected_z23[] = {0x7fffffffffffffff, 0x8000000000000006};
    ASSERT_EQUAL_SVE(expected_z23, z23.VnD());
  }
}

TEST(sve_int_compare_count_and_limit_scalars) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  __ Mov(w20, 0xfffffffd);
  __ Mov(w21, 0xffffffff);

  __ Whilele(p0.VnB(), w20, w21);
  __ Mrs(x0, NZCV);
  __ Whilele(p1.VnH(), w20, w21);
  __ Mrs(x1, NZCV);

  __ Mov(w20, 0xffffffff);
  __ Mov(w21, 0x00000000);

  __ Whilelt(p2.VnS(), w20, w21);
  __ Mrs(x2, NZCV);
  __ Whilelt(p3.VnD(), w20, w21);
  __ Mrs(x3, NZCV);

  __ Mov(w20, 0xfffffffd);
  __ Mov(w21, 0xffffffff);

  __ Whilels(p4.VnB(), w20, w21);
  __ Mrs(x4, NZCV);
  __ Whilels(p5.VnH(), w20, w21);
  __ Mrs(x5, NZCV);

  __ Mov(w20, 0xffffffff);
  __ Mov(w21, 0x00000000);

  __ Whilelo(p6.VnS(), w20, w21);
  __ Mrs(x6, NZCV);
  __ Whilelo(p7.VnD(), w20, w21);
  __ Mrs(x7, NZCV);

  __ Mov(x20, 0xfffffffffffffffd);
  __ Mov(x21, 0xffffffffffffffff);

  __ Whilele(p8.VnB(), x20, x21);
  __ Mrs(x8, NZCV);
  __ Whilele(p9.VnH(), x20, x21);
  __ Mrs(x9, NZCV);

  __ Mov(x20, 0xffffffffffffffff);
  __ Mov(x21, 0x0000000000000000);

  __ Whilelt(p10.VnS(), x20, x21);
  __ Mrs(x10, NZCV);
  __ Whilelt(p11.VnD(), x20, x21);
  __ Mrs(x11, NZCV);

  __ Mov(x20, 0xfffffffffffffffd);
  __ Mov(x21, 0xffffffffffffffff);

  __ Whilels(p12.VnB(), x20, x21);
  __ Mrs(x12, NZCV);
  __ Whilels(p13.VnH(), x20, x21);
  __ Mrs(x13, NZCV);

  __ Mov(x20, 0xffffffffffffffff);
  __ Mov(x21, 0x0000000000000000);

  __ Whilelo(p14.VnS(), x20, x21);
  __ Mrs(x14, NZCV);
  __ Whilelo(p15.VnD(), x20, x21);
  __ Mrs(x15, NZCV);

  END();

  if (CAN_RUN()) {
    RUN();

    // 0b...00000000'00000111
    int p0_expected[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1};
    ASSERT_EQUAL_SVE(p0_expected, p0.VnB());

    // 0b...00000000'00010101
    int p1_expected[] = {0, 0, 0, 0, 0, 1, 1, 1};
    ASSERT_EQUAL_SVE(p1_expected, p1.VnH());

    int p2_expected[] = {0x0, 0x0, 0x0, 0x1};
    ASSERT_EQUAL_SVE(p2_expected, p2.VnS());

    int p3_expected[] = {0x00, 0x01};
    ASSERT_EQUAL_SVE(p3_expected, p3.VnD());

    // 0b...11111111'11111111
    int p4_expected[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    ASSERT_EQUAL_SVE(p4_expected, p4.VnB());

    // 0b...01010101'01010101
    int p5_expected[] = {1, 1, 1, 1, 1, 1, 1, 1};
    ASSERT_EQUAL_SVE(p5_expected, p5.VnH());

    int p6_expected[] = {0x0, 0x0, 0x0, 0x0};
    ASSERT_EQUAL_SVE(p6_expected, p6.VnS());

    int p7_expected[] = {0x00, 0x00};
    ASSERT_EQUAL_SVE(p7_expected, p7.VnD());

    // 0b...00000000'00000111
    int p8_expected[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1};
    ASSERT_EQUAL_SVE(p8_expected, p8.VnB());

    // 0b...00000000'00010101
    int p9_expected[] = {0, 0, 0, 0, 0, 1, 1, 1};
    ASSERT_EQUAL_SVE(p9_expected, p9.VnH());

    int p10_expected[] = {0x0, 0x0, 0x0, 0x1};
    ASSERT_EQUAL_SVE(p10_expected, p10.VnS());

    int p11_expected[] = {0x00, 0x01};
    ASSERT_EQUAL_SVE(p11_expected, p11.VnD());

    // 0b...11111111'11111111
    int p12_expected[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    ASSERT_EQUAL_SVE(p12_expected, p12.VnB());

    // 0b...01010101'01010101
    int p13_expected[] = {1, 1, 1, 1, 1, 1, 1, 1};
    ASSERT_EQUAL_SVE(p13_expected, p13.VnH());

    int p14_expected[] = {0x0, 0x0, 0x0, 0x0};
    ASSERT_EQUAL_SVE(p14_expected, p14.VnS());

    int p15_expected[] = {0x00, 0x00};
    ASSERT_EQUAL_SVE(p15_expected, p15.VnD());

    ASSERT_EQUAL_32(SVEFirstFlag | SVENotLastFlag, w0);
    ASSERT_EQUAL_32(SVEFirstFlag | SVENotLastFlag, w1);
    ASSERT_EQUAL_32(SVEFirstFlag | SVENotLastFlag, w2);
    ASSERT_EQUAL_32(SVEFirstFlag | SVENotLastFlag, w3);
    ASSERT_EQUAL_32(SVEFirstFlag, w4);
    ASSERT_EQUAL_32(SVEFirstFlag, w5);
    ASSERT_EQUAL_32(SVENoneFlag | SVENotLastFlag, w6);
    ASSERT_EQUAL_32(SVENoneFlag | SVENotLastFlag, w7);
    ASSERT_EQUAL_32(SVEFirstFlag | SVENotLastFlag, w8);
    ASSERT_EQUAL_32(SVEFirstFlag | SVENotLastFlag, w9);
    ASSERT_EQUAL_32(SVEFirstFlag | SVENotLastFlag, w10);
    ASSERT_EQUAL_32(SVEFirstFlag | SVENotLastFlag, w11);
    ASSERT_EQUAL_32(SVEFirstFlag, w12);
    ASSERT_EQUAL_32(SVEFirstFlag, w13);
    ASSERT_EQUAL_32(SVENoneFlag | SVENotLastFlag, w14);
    ASSERT_EQUAL_32(SVENoneFlag | SVENotLastFlag, w15);
  }
}

TEST(sve_int_compare_vectors_signed_imm) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int z13_inputs[] = {0, 1, -1, -15, 126, -127, -126, -15};
  int mask_inputs1[] = {1, 1, 1, 0, 1, 1, 1, 1};
  InsrHelper(&masm, z13.VnB(), z13_inputs);
  Initialise(&masm, p0.VnB(), mask_inputs1);

  __ Cmpeq(p2.VnB(), p0.Zeroing(), z13.VnB(), -15);
  __ Mrs(x2, NZCV);
  __ Cmpeq(p3.VnB(), p0.Zeroing(), z13.VnB(), -127);

  int z14_inputs[] = {0, 1, -1, -32767, -32766, 32767, 32766, 0};
  int mask_inputs2[] = {1, 1, 1, 0, 1, 1, 1, 1};
  InsrHelper(&masm, z14.VnH(), z14_inputs);
  Initialise(&masm, p0.VnH(), mask_inputs2);

  __ Cmpge(p4.VnH(), p0.Zeroing(), z14.VnH(), -1);
  __ Mrs(x4, NZCV);
  __ Cmpge(p5.VnH(), p0.Zeroing(), z14.VnH(), -32767);

  int z15_inputs[] = {0, 1, -1, INT_MIN};
  int mask_inputs3[] = {0, 1, 1, 1};
  InsrHelper(&masm, z15.VnS(), z15_inputs);
  Initialise(&masm, p0.VnS(), mask_inputs3);

  __ Cmpgt(p6.VnS(), p0.Zeroing(), z15.VnS(), 0);
  __ Mrs(x6, NZCV);
  __ Cmpgt(p7.VnS(), p0.Zeroing(), z15.VnS(), INT_MIN + 1);

  __ Cmplt(p8.VnS(), p0.Zeroing(), z15.VnS(), 0);
  __ Mrs(x8, NZCV);
  __ Cmplt(p9.VnS(), p0.Zeroing(), z15.VnS(), INT_MIN + 1);

  int64_t z16_inputs[] = {0, -1};
  int mask_inputs4[] = {1, 1};
  InsrHelper(&masm, z16.VnD(), z16_inputs);
  Initialise(&masm, p0.VnD(), mask_inputs4);

  __ Cmple(p10.VnD(), p0.Zeroing(), z16.VnD(), -1);
  __ Mrs(x10, NZCV);
  __ Cmple(p11.VnD(), p0.Zeroing(), z16.VnD(), LLONG_MIN);

  __ Cmpne(p12.VnD(), p0.Zeroing(), z16.VnD(), -1);
  __ Mrs(x12, NZCV);
  __ Cmpne(p13.VnD(), p0.Zeroing(), z16.VnD(), LLONG_MAX);

  END();

  if (CAN_RUN()) {
    RUN();

    int p2_expected[] = {0, 0, 0, 0, 0, 0, 0, 1};
    ASSERT_EQUAL_SVE(p2_expected, p2.VnB());

    int p3_expected[] = {0, 0, 0, 0, 0, 1, 0, 0};
    ASSERT_EQUAL_SVE(p3_expected, p3.VnB());

    int p4_expected[] = {0x1, 0x1, 0x1, 0x0, 0x0, 0x1, 0x1, 0x1};
    ASSERT_EQUAL_SVE(p4_expected, p4.VnH());

    int p5_expected[] = {0x1, 0x1, 0x1, 0x0, 0x1, 0x1, 0x1, 0x1};
    ASSERT_EQUAL_SVE(p5_expected, p5.VnH());

    int p6_expected[] = {0x0, 0x1, 0x0, 0x0};
    ASSERT_EQUAL_SVE(p6_expected, p6.VnS());

    int p7_expected[] = {0x0, 0x1, 0x1, 0x0};
    ASSERT_EQUAL_SVE(p7_expected, p7.VnS());

    int p8_expected[] = {0x0, 0x0, 0x1, 0x1};
    ASSERT_EQUAL_SVE(p8_expected, p8.VnS());

    int p9_expected[] = {0x0, 0x0, 0x0, 0x1};
    ASSERT_EQUAL_SVE(p9_expected, p9.VnS());

    int p10_expected[] = {0x00, 0x01};
    ASSERT_EQUAL_SVE(p10_expected, p10.VnD());

    int p11_expected[] = {0x00, 0x00};
    ASSERT_EQUAL_SVE(p11_expected, p11.VnD());

    int p12_expected[] = {0x01, 0x00};
    ASSERT_EQUAL_SVE(p12_expected, p12.VnD());

    int p13_expected[] = {0x01, 0x01};
    ASSERT_EQUAL_SVE(p13_expected, p13.VnD());

    ASSERT_EQUAL_32(SVENotLastFlag | SVEFirstFlag, w2);
    ASSERT_EQUAL_32(SVEFirstFlag, w4);
    ASSERT_EQUAL_32(NoFlag, w6);
    ASSERT_EQUAL_32(SVENotLastFlag | SVEFirstFlag, w8);
    ASSERT_EQUAL_32(SVENotLastFlag | SVEFirstFlag, w10);
    ASSERT_EQUAL_32(NoFlag, w12);
  }
}

TEST(sve_int_compare_vectors_unsigned_imm) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint32_t src1_inputs[] = {0xf7, 0x0f, 0x8f, 0x1f, 0x83, 0x12, 0x00, 0xf1};
  int mask_inputs1[] = {1, 1, 1, 0, 1, 1, 0, 1};
  InsrHelper(&masm, z13.VnB(), src1_inputs);
  Initialise(&masm, p0.VnB(), mask_inputs1);

  __ Cmphi(p2.VnB(), p0.Zeroing(), z13.VnB(), 0x0f);
  __ Mrs(x2, NZCV);
  __ Cmphi(p3.VnB(), p0.Zeroing(), z13.VnB(), 0xf0);

  uint32_t src2_inputs[] = {0xffff, 0x8000, 0x1fff, 0x0000, 0x1234};
  int mask_inputs2[] = {1, 1, 1, 1, 0};
  InsrHelper(&masm, z13.VnH(), src2_inputs);
  Initialise(&masm, p0.VnH(), mask_inputs2);

  __ Cmphs(p4.VnH(), p0.Zeroing(), z13.VnH(), 0x1f);
  __ Mrs(x4, NZCV);
  __ Cmphs(p5.VnH(), p0.Zeroing(), z13.VnH(), 0x1fff);

  uint32_t src3_inputs[] = {0xffffffff, 0xfedcba98, 0x0000ffff, 0x00000000};
  int mask_inputs3[] = {1, 1, 1, 1};
  InsrHelper(&masm, z13.VnS(), src3_inputs);
  Initialise(&masm, p0.VnS(), mask_inputs3);

  __ Cmplo(p6.VnS(), p0.Zeroing(), z13.VnS(), 0x3f);
  __ Mrs(x6, NZCV);
  __ Cmplo(p7.VnS(), p0.Zeroing(), z13.VnS(), 0x3f3f3f3f);

  uint64_t src4_inputs[] = {0xffffffffffffffff, 0x0000000000000000};
  int mask_inputs4[] = {1, 1};
  InsrHelper(&masm, z13.VnD(), src4_inputs);
  Initialise(&masm, p0.VnD(), mask_inputs4);

  __ Cmpls(p8.VnD(), p0.Zeroing(), z13.VnD(), 0x2f);
  __ Mrs(x8, NZCV);
  __ Cmpls(p9.VnD(), p0.Zeroing(), z13.VnD(), 0x800000000000000);

  END();

  if (CAN_RUN()) {
    RUN();

    int p2_expected[] = {1, 0, 1, 0, 1, 1, 0, 1};
    ASSERT_EQUAL_SVE(p2_expected, p2.VnB());

    int p3_expected[] = {1, 0, 0, 0, 0, 0, 0, 1};
    ASSERT_EQUAL_SVE(p3_expected, p3.VnB());

    int p4_expected[] = {0x1, 0x1, 0x1, 0x0, 0x0};
    ASSERT_EQUAL_SVE(p4_expected, p4.VnH());

    int p5_expected[] = {0x1, 0x1, 0x1, 0x0, 0x0};
    ASSERT_EQUAL_SVE(p5_expected, p5.VnH());

    int p6_expected[] = {0x0, 0x0, 0x0, 0x1};
    ASSERT_EQUAL_SVE(p6_expected, p6.VnS());

    int p7_expected[] = {0x0, 0x0, 0x1, 0x1};
    ASSERT_EQUAL_SVE(p7_expected, p7.VnS());

    int p8_expected[] = {0x00, 0x01};
    ASSERT_EQUAL_SVE(p8_expected, p8.VnD());

    int p9_expected[] = {0x00, 0x01};
    ASSERT_EQUAL_SVE(p9_expected, p9.VnD());

    ASSERT_EQUAL_32(SVEFirstFlag, w2);
    ASSERT_EQUAL_32(NoFlag, w4);
    ASSERT_EQUAL_32(SVENotLastFlag | SVEFirstFlag, w6);
    ASSERT_EQUAL_32(SVENotLastFlag | SVEFirstFlag, w8);
  }
}

TEST(sve_int_compare_conditionally_terminate_scalars) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  __ Mov(x0, 0xfedcba9887654321);
  __ Mov(x1, 0x1000100010001000);

  // Initialise Z and C. These are preserved by cterm*, and the V flag is set to
  // !C if the condition does not hold.
  __ Mov(x10, NoFlag);
  __ Msr(NZCV, x10);

  __ Ctermeq(w0, w0);
  __ Mrs(x2, NZCV);
  __ Ctermeq(x0, x1);
  __ Mrs(x3, NZCV);
  __ Ctermne(x0, x0);
  __ Mrs(x4, NZCV);
  __ Ctermne(w0, w1);
  __ Mrs(x5, NZCV);

  // As above, but with all flags initially set.
  __ Mov(x10, NZCVFlag);
  __ Msr(NZCV, x10);

  __ Ctermeq(w0, w0);
  __ Mrs(x6, NZCV);
  __ Ctermeq(x0, x1);
  __ Mrs(x7, NZCV);
  __ Ctermne(x0, x0);
  __ Mrs(x8, NZCV);
  __ Ctermne(w0, w1);
  __ Mrs(x9, NZCV);

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_32(SVEFirstFlag, w2);
    ASSERT_EQUAL_32(VFlag, w3);
    ASSERT_EQUAL_32(VFlag, w4);
    ASSERT_EQUAL_32(SVEFirstFlag, w5);

    ASSERT_EQUAL_32(SVEFirstFlag | ZCFlag, w6);
    ASSERT_EQUAL_32(ZCFlag, w7);
    ASSERT_EQUAL_32(ZCFlag, w8);
    ASSERT_EQUAL_32(SVEFirstFlag | ZCFlag, w9);
  }
}

// Work out what the architectural `PredTest` pseudocode should produce for the
// given result and governing predicate.
template <typename Tg, typename Td, int N>
static StatusFlags GetPredTestFlags(const Td (&pd)[N],
                                    const Tg (&pg)[N],
                                    int vl) {
  int first = -1;
  int last = -1;
  bool any_active = false;

  // Only consider potentially-active lanes.
  int start = (N > vl) ? (N - vl) : 0;
  for (int i = start; i < N; i++) {
    if ((pg[i] & 1) == 1) {
      // Look for the first and last active lanes.
      // Note that the 'first' lane is the one with the highest index.
      if (last < 0) last = i;
      first = i;
      // Look for any active lanes that are also active in pd.
      if ((pd[i] & 1) == 1) any_active = true;
    }
  }

  uint32_t flags = 0;
  if ((first >= 0) && ((pd[first] & 1) == 1)) flags |= SVEFirstFlag;
  if (!any_active) flags |= SVENoneFlag;
  if ((last < 0) || ((pd[last] & 1) == 0)) flags |= SVENotLastFlag;
  return static_cast<StatusFlags>(flags);
}

typedef void (MacroAssembler::*PfirstPnextFn)(const PRegisterWithLaneSize& pd,
                                              const PRegister& pg,
                                              const PRegisterWithLaneSize& pn);
template <typename Tg, typename Tn, typename Td>
static void PfirstPnextHelper(Test* config,
                              PfirstPnextFn macro,
                              unsigned lane_size_in_bits,
                              const Tg& pg_inputs,
                              const Tn& pn_inputs,
                              const Td& pd_expected) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  PRegister pg = p15;
  PRegister pn = p14;
  Initialise(&masm, pg.WithLaneSize(lane_size_in_bits), pg_inputs);
  Initialise(&masm, pn.WithLaneSize(lane_size_in_bits), pn_inputs);

  // Initialise NZCV to an impossible value, to check that we actually write it.
  __ Mov(x10, NZCVFlag);

  // If pd.Is(pn), the MacroAssembler simply passes the arguments directly to
  // the Assembler.
  __ Msr(NZCV, x10);
  __ Mov(p0, pn);
  (masm.*macro)(p0.WithLaneSize(lane_size_in_bits),
                pg,
                p0.WithLaneSize(lane_size_in_bits));
  __ Mrs(x0, NZCV);

  // The MacroAssembler supports non-destructive use.
  __ Msr(NZCV, x10);
  (masm.*macro)(p1.WithLaneSize(lane_size_in_bits),
                pg,
                pn.WithLaneSize(lane_size_in_bits));
  __ Mrs(x1, NZCV);

  // If pd.Aliases(pg) the macro requires a scratch register.
  {
    UseScratchRegisterScope temps(&masm);
    temps.Include(p13);
    __ Msr(NZCV, x10);
    __ Mov(p2, p15);
    (masm.*macro)(p2.WithLaneSize(lane_size_in_bits),
                  p2,
                  pn.WithLaneSize(lane_size_in_bits));
    __ Mrs(x2, NZCV);
  }

  END();

  if (CAN_RUN()) {
    RUN();

    // Check that the inputs weren't modified.
    ASSERT_EQUAL_SVE(pn_inputs, pn.WithLaneSize(lane_size_in_bits));
    ASSERT_EQUAL_SVE(pg_inputs, pg.WithLaneSize(lane_size_in_bits));

    // Check the primary operation.
    ASSERT_EQUAL_SVE(pd_expected, p0.WithLaneSize(lane_size_in_bits));
    ASSERT_EQUAL_SVE(pd_expected, p1.WithLaneSize(lane_size_in_bits));
    ASSERT_EQUAL_SVE(pd_expected, p2.WithLaneSize(lane_size_in_bits));

    // Check that the flags were properly set.
    StatusFlags nzcv_expected =
        GetPredTestFlags(pd_expected,
                         pg_inputs,
                         core.GetSVELaneCount(kBRegSize));
    ASSERT_EQUAL_64(nzcv_expected, x0);
    ASSERT_EQUAL_64(nzcv_expected, x1);
    ASSERT_EQUAL_64(nzcv_expected, x2);
  }
}

template <typename Tg, typename Tn, typename Td>
static void PfirstHelper(Test* config,
                         const Tg& pg_inputs,
                         const Tn& pn_inputs,
                         const Td& pd_expected) {
  PfirstPnextHelper(config,
                    &MacroAssembler::Pfirst,
                    kBRegSize,  // pfirst only accepts B-sized lanes.
                    pg_inputs,
                    pn_inputs,
                    pd_expected);
}

template <typename Tg, typename Tn, typename Td>
static void PnextHelper(Test* config,
                        unsigned lane_size_in_bits,
                        const Tg& pg_inputs,
                        const Tn& pn_inputs,
                        const Td& pd_expected) {
  PfirstPnextHelper(config,
                    &MacroAssembler::Pnext,
                    lane_size_in_bits,
                    pg_inputs,
                    pn_inputs,
                    pd_expected);
}

TEST_SVE(sve_pfirst) {
  // Provide more lanes than kPRegMinSize (to check propagation if we have a
  // large VL), but few enough to make the test easy to read.
  int in0[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int in1[] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
  int in2[] = {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
  int in3[] = {0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1};
  int in4[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  VIXL_ASSERT(ArrayLength(in0) > kPRegMinSize);

  // Pfirst finds the first active lane in pg, and activates the corresponding
  // lane in pn (if it isn't already active).

  //                             The first active lane in in1 is here. |
  //                                                                   v
  int exp10[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
  int exp12[] = {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0};
  int exp13[] = {0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1};
  int exp14[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
  PfirstHelper(config, in1, in0, exp10);
  PfirstHelper(config, in1, in2, exp12);
  PfirstHelper(config, in1, in3, exp13);
  PfirstHelper(config, in1, in4, exp14);

  //                          The first active lane in in2 is here. |
  //                                                                v
  int exp20[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0};
  int exp21[] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0};
  int exp23[] = {0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1};
  int exp24[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0};
  PfirstHelper(config, in2, in0, exp20);
  PfirstHelper(config, in2, in1, exp21);
  PfirstHelper(config, in2, in3, exp23);
  PfirstHelper(config, in2, in4, exp24);

  //                                   The first active lane in in3 is here. |
  //                                                                         v
  int exp30[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  int exp31[] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1};
  int exp32[] = {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1};
  int exp34[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  PfirstHelper(config, in3, in0, exp30);
  PfirstHelper(config, in3, in1, exp31);
  PfirstHelper(config, in3, in2, exp32);
  PfirstHelper(config, in3, in4, exp34);

  //             | The first active lane in in4 is here.
  //             v
  int exp40[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp41[] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
  int exp42[] = {1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
  int exp43[] = {1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1};
  PfirstHelper(config, in4, in0, exp40);
  PfirstHelper(config, in4, in1, exp41);
  PfirstHelper(config, in4, in2, exp42);
  PfirstHelper(config, in4, in3, exp43);

  // If pg is all inactive, the input is passed through unchanged.
  PfirstHelper(config, in0, in0, in0);
  PfirstHelper(config, in0, in1, in1);
  PfirstHelper(config, in0, in2, in2);
  PfirstHelper(config, in0, in3, in3);

  // If the values of pg and pn match, the value is passed through unchanged.
  PfirstHelper(config, in0, in0, in0);
  PfirstHelper(config, in1, in1, in1);
  PfirstHelper(config, in2, in2, in2);
  PfirstHelper(config, in3, in3, in3);
}

TEST_SVE(sve_pfirst_alias) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Check that the Simulator behaves correctly when all arguments are aliased.
  int in_b[] = {0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0};
  int in_h[] = {0, 0, 0, 0, 1, 1, 0, 0};
  int in_s[] = {0, 1, 1, 0};
  int in_d[] = {1, 1};

  Initialise(&masm, p0.VnB(), in_b);
  Initialise(&masm, p1.VnH(), in_h);
  Initialise(&masm, p2.VnS(), in_s);
  Initialise(&masm, p3.VnD(), in_d);

  // Initialise NZCV to an impossible value, to check that we actually write it.
  __ Mov(x10, NZCVFlag);

  __ Msr(NZCV, x10);
  __ Pfirst(p0.VnB(), p0.VnB(), p0.VnB());
  __ Mrs(x0, NZCV);

  __ Msr(NZCV, x10);
  __ Pfirst(p1.VnB(), p1.VnB(), p1.VnB());
  __ Mrs(x1, NZCV);

  __ Msr(NZCV, x10);
  __ Pfirst(p2.VnB(), p2.VnB(), p2.VnB());
  __ Mrs(x2, NZCV);

  __ Msr(NZCV, x10);
  __ Pfirst(p3.VnB(), p3.VnB(), p3.VnB());
  __ Mrs(x3, NZCV);

  END();

  if (CAN_RUN()) {
    RUN();

    // The first lane from pg is already active in pdn, so the P register should
    // be unchanged.
    ASSERT_EQUAL_SVE(in_b, p0.VnB());
    ASSERT_EQUAL_SVE(in_h, p1.VnH());
    ASSERT_EQUAL_SVE(in_s, p2.VnS());
    ASSERT_EQUAL_SVE(in_d, p3.VnD());

    ASSERT_EQUAL_64(SVEFirstFlag, x0);
    ASSERT_EQUAL_64(SVEFirstFlag, x1);
    ASSERT_EQUAL_64(SVEFirstFlag, x2);
    ASSERT_EQUAL_64(SVEFirstFlag, x3);
  }
}

TEST_SVE(sve_pnext_b) {
  // TODO: Once we have the infrastructure, provide more lanes than kPRegMinSize
  // (to check propagation if we have a large VL), but few enough to make the
  // test easy to read.
  // For now, we just use kPRegMinSize so that the test works anywhere.
  int in0[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int in1[] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
  int in2[] = {0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
  int in3[] = {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1};
  int in4[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  // Pnext activates the next element that is true in pg, after the last-active
  // element in pn. If all pn elements are false (as in in0), it starts looking
  // at element 0.

  // There are no active lanes in in0, so the result is simply the first active
  // lane from pg.
  int exp00[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp10[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
  int exp20[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0};
  int exp30[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  int exp40[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  //      The last active lane in in1 is here. |
  //                                           v
  int exp01[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp11[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp21[] = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp31[] = {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp41[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  //                | The last active lane in in2 is here.
  //                v
  int exp02[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp12[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp22[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp32[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp42[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  //                               | The last active lane in in3 is here.
  //                               v
  int exp03[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp13[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp23[] = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp33[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp43[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  //             | The last active lane in in4 is here.
  //             v
  int exp04[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp14[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp24[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp34[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp44[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  PnextHelper(config, kBRegSize, in0, in0, exp00);
  PnextHelper(config, kBRegSize, in1, in0, exp10);
  PnextHelper(config, kBRegSize, in2, in0, exp20);
  PnextHelper(config, kBRegSize, in3, in0, exp30);
  PnextHelper(config, kBRegSize, in4, in0, exp40);

  PnextHelper(config, kBRegSize, in0, in1, exp01);
  PnextHelper(config, kBRegSize, in1, in1, exp11);
  PnextHelper(config, kBRegSize, in2, in1, exp21);
  PnextHelper(config, kBRegSize, in3, in1, exp31);
  PnextHelper(config, kBRegSize, in4, in1, exp41);

  PnextHelper(config, kBRegSize, in0, in2, exp02);
  PnextHelper(config, kBRegSize, in1, in2, exp12);
  PnextHelper(config, kBRegSize, in2, in2, exp22);
  PnextHelper(config, kBRegSize, in3, in2, exp32);
  PnextHelper(config, kBRegSize, in4, in2, exp42);

  PnextHelper(config, kBRegSize, in0, in3, exp03);
  PnextHelper(config, kBRegSize, in1, in3, exp13);
  PnextHelper(config, kBRegSize, in2, in3, exp23);
  PnextHelper(config, kBRegSize, in3, in3, exp33);
  PnextHelper(config, kBRegSize, in4, in3, exp43);

  PnextHelper(config, kBRegSize, in0, in4, exp04);
  PnextHelper(config, kBRegSize, in1, in4, exp14);
  PnextHelper(config, kBRegSize, in2, in4, exp24);
  PnextHelper(config, kBRegSize, in3, in4, exp34);
  PnextHelper(config, kBRegSize, in4, in4, exp44);
}

TEST_SVE(sve_pnext_h) {
  // TODO: Once we have the infrastructure, provide more lanes than kPRegMinSize
  // (to check propagation if we have a large VL), but few enough to make the
  // test easy to read.
  // For now, we just use kPRegMinSize so that the test works anywhere.
  int in0[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int in1[] = {0, 0, 0, 1, 0, 2, 1, 0};
  int in2[] = {0, 1, 2, 0, 2, 0, 2, 0};
  int in3[] = {0, 0, 0, 3, 0, 0, 0, 3};
  int in4[] = {3, 0, 0, 0, 0, 0, 0, 0};

  // Pnext activates the next element that is true in pg, after the last-active
  // element in pn. If all pn elements are false (as in in0), it starts looking
  // at element 0.
  //
  // As for other SVE instructions, elements are only considered to be active if
  // the _first_ bit in each field is one. Other bits are ignored.

  // There are no active lanes in in0, so the result is simply the first active
  // lane from pg.
  int exp00[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp10[] = {0, 0, 0, 0, 0, 0, 1, 0};
  int exp20[] = {0, 1, 0, 0, 0, 0, 0, 0};
  int exp30[] = {0, 0, 0, 0, 0, 0, 0, 1};
  int exp40[] = {1, 0, 0, 0, 0, 0, 0, 0};

  //                      | The last active lane in in1 is here.
  //                      v
  int exp01[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp11[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp21[] = {0, 1, 0, 0, 0, 0, 0, 0};
  int exp31[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp41[] = {1, 0, 0, 0, 0, 0, 0, 0};

  //                | The last active lane in in2 is here.
  //                v
  int exp02[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp12[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp22[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp32[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp42[] = {1, 0, 0, 0, 0, 0, 0, 0};

  //                      | The last active lane in in3 is here.
  //                      v
  int exp03[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp13[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp23[] = {0, 1, 0, 0, 0, 0, 0, 0};
  int exp33[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp43[] = {1, 0, 0, 0, 0, 0, 0, 0};

  //             | The last active lane in in4 is here.
  //             v
  int exp04[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp14[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp24[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp34[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int exp44[] = {0, 0, 0, 0, 0, 0, 0, 0};

  PnextHelper(config, kHRegSize, in0, in0, exp00);
  PnextHelper(config, kHRegSize, in1, in0, exp10);
  PnextHelper(config, kHRegSize, in2, in0, exp20);
  PnextHelper(config, kHRegSize, in3, in0, exp30);
  PnextHelper(config, kHRegSize, in4, in0, exp40);

  PnextHelper(config, kHRegSize, in0, in1, exp01);
  PnextHelper(config, kHRegSize, in1, in1, exp11);
  PnextHelper(config, kHRegSize, in2, in1, exp21);
  PnextHelper(config, kHRegSize, in3, in1, exp31);
  PnextHelper(config, kHRegSize, in4, in1, exp41);

  PnextHelper(config, kHRegSize, in0, in2, exp02);
  PnextHelper(config, kHRegSize, in1, in2, exp12);
  PnextHelper(config, kHRegSize, in2, in2, exp22);
  PnextHelper(config, kHRegSize, in3, in2, exp32);
  PnextHelper(config, kHRegSize, in4, in2, exp42);

  PnextHelper(config, kHRegSize, in0, in3, exp03);
  PnextHelper(config, kHRegSize, in1, in3, exp13);
  PnextHelper(config, kHRegSize, in2, in3, exp23);
  PnextHelper(config, kHRegSize, in3, in3, exp33);
  PnextHelper(config, kHRegSize, in4, in3, exp43);

  PnextHelper(config, kHRegSize, in0, in4, exp04);
  PnextHelper(config, kHRegSize, in1, in4, exp14);
  PnextHelper(config, kHRegSize, in2, in4, exp24);
  PnextHelper(config, kHRegSize, in3, in4, exp34);
  PnextHelper(config, kHRegSize, in4, in4, exp44);
}

TEST_SVE(sve_pnext_s) {
  // TODO: Once we have the infrastructure, provide more lanes than kPRegMinSize
  // (to check propagation if we have a large VL), but few enough to make the
  // test easy to read.
  // For now, we just use kPRegMinSize so that the test works anywhere.
  int in0[] = {0xe, 0xc, 0x8, 0x0};
  int in1[] = {0x0, 0x2, 0x0, 0x1};
  int in2[] = {0x0, 0x1, 0xf, 0x0};
  int in3[] = {0xf, 0x0, 0x0, 0x0};

  // Pnext activates the next element that is true in pg, after the last-active
  // element in pn. If all pn elements are false (as in in0), it starts looking
  // at element 0.
  //
  // As for other SVE instructions, elements are only considered to be active if
  // the _first_ bit in each field is one. Other bits are ignored.

  // There are no active lanes in in0, so the result is simply the first active
  // lane from pg.
  int exp00[] = {0, 0, 0, 0};
  int exp10[] = {0, 0, 0, 1};
  int exp20[] = {0, 0, 1, 0};
  int exp30[] = {1, 0, 0, 0};

  //                      | The last active lane in in1 is here.
  //                      v
  int exp01[] = {0, 0, 0, 0};
  int exp11[] = {0, 0, 0, 0};
  int exp21[] = {0, 0, 1, 0};
  int exp31[] = {1, 0, 0, 0};

  //                | The last active lane in in2 is here.
  //                v
  int exp02[] = {0, 0, 0, 0};
  int exp12[] = {0, 0, 0, 0};
  int exp22[] = {0, 0, 0, 0};
  int exp32[] = {1, 0, 0, 0};

  //             | The last active lane in in3 is here.
  //             v
  int exp03[] = {0, 0, 0, 0};
  int exp13[] = {0, 0, 0, 0};
  int exp23[] = {0, 0, 0, 0};
  int exp33[] = {0, 0, 0, 0};

  PnextHelper(config, kSRegSize, in0, in0, exp00);
  PnextHelper(config, kSRegSize, in1, in0, exp10);
  PnextHelper(config, kSRegSize, in2, in0, exp20);
  PnextHelper(config, kSRegSize, in3, in0, exp30);

  PnextHelper(config, kSRegSize, in0, in1, exp01);
  PnextHelper(config, kSRegSize, in1, in1, exp11);
  PnextHelper(config, kSRegSize, in2, in1, exp21);
  PnextHelper(config, kSRegSize, in3, in1, exp31);

  PnextHelper(config, kSRegSize, in0, in2, exp02);
  PnextHelper(config, kSRegSize, in1, in2, exp12);
  PnextHelper(config, kSRegSize, in2, in2, exp22);
  PnextHelper(config, kSRegSize, in3, in2, exp32);

  PnextHelper(config, kSRegSize, in0, in3, exp03);
  PnextHelper(config, kSRegSize, in1, in3, exp13);
  PnextHelper(config, kSRegSize, in2, in3, exp23);
  PnextHelper(config, kSRegSize, in3, in3, exp33);
}

TEST_SVE(sve_pnext_d) {
  // TODO: Once we have the infrastructure, provide more lanes than kPRegMinSize
  // (to check propagation if we have a large VL), but few enough to make the
  // test easy to read.
  // For now, we just use kPRegMinSize so that the test works anywhere.
  int in0[] = {0xfe, 0xf0};
  int in1[] = {0x00, 0x55};
  int in2[] = {0x33, 0xff};

  // Pnext activates the next element that is true in pg, after the last-active
  // element in pn. If all pn elements are false (as in in0), it starts looking
  // at element 0.
  //
  // As for other SVE instructions, elements are only considered to be active if
  // the _first_ bit in each field is one. Other bits are ignored.

  // There are no active lanes in in0, so the result is simply the first active
  // lane from pg.
  int exp00[] = {0, 0};
  int exp10[] = {0, 1};
  int exp20[] = {0, 1};

  //                | The last active lane in in1 is here.
  //                v
  int exp01[] = {0, 0};
  int exp11[] = {0, 0};
  int exp21[] = {1, 0};

  //             | The last active lane in in2 is here.
  //             v
  int exp02[] = {0, 0};
  int exp12[] = {0, 0};
  int exp22[] = {0, 0};

  PnextHelper(config, kDRegSize, in0, in0, exp00);
  PnextHelper(config, kDRegSize, in1, in0, exp10);
  PnextHelper(config, kDRegSize, in2, in0, exp20);

  PnextHelper(config, kDRegSize, in0, in1, exp01);
  PnextHelper(config, kDRegSize, in1, in1, exp11);
  PnextHelper(config, kDRegSize, in2, in1, exp21);

  PnextHelper(config, kDRegSize, in0, in2, exp02);
  PnextHelper(config, kDRegSize, in1, in2, exp12);
  PnextHelper(config, kDRegSize, in2, in2, exp22);
}

TEST_SVE(sve_pnext_alias) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Check that the Simulator behaves correctly when all arguments are aliased.
  int in_b[] = {0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0};
  int in_h[] = {0, 0, 0, 0, 1, 1, 0, 0};
  int in_s[] = {0, 1, 1, 0};
  int in_d[] = {1, 1};

  Initialise(&masm, p0.VnB(), in_b);
  Initialise(&masm, p1.VnH(), in_h);
  Initialise(&masm, p2.VnS(), in_s);
  Initialise(&masm, p3.VnD(), in_d);

  // Initialise NZCV to an impossible value, to check that we actually write it.
  __ Mov(x10, NZCVFlag);

  __ Msr(NZCV, x10);
  __ Pnext(p0.VnB(), p0.VnB(), p0.VnB());
  __ Mrs(x0, NZCV);

  __ Msr(NZCV, x10);
  __ Pnext(p1.VnB(), p1.VnB(), p1.VnB());
  __ Mrs(x1, NZCV);

  __ Msr(NZCV, x10);
  __ Pnext(p2.VnB(), p2.VnB(), p2.VnB());
  __ Mrs(x2, NZCV);

  __ Msr(NZCV, x10);
  __ Pnext(p3.VnB(), p3.VnB(), p3.VnB());
  __ Mrs(x3, NZCV);

  END();

  if (CAN_RUN()) {
    RUN();

    // Since pg.Is(pdn), there can be no active lanes in pg above the last
    // active lane in pdn, so the result should always be zero.
    ASSERT_EQUAL_SVE(0, p0.VnB());
    ASSERT_EQUAL_SVE(0, p1.VnH());
    ASSERT_EQUAL_SVE(0, p2.VnS());
    ASSERT_EQUAL_SVE(0, p3.VnD());

    ASSERT_EQUAL_64(SVENoneFlag | SVENotLastFlag, x0);
    ASSERT_EQUAL_64(SVENoneFlag | SVENotLastFlag, x1);
    ASSERT_EQUAL_64(SVENoneFlag | SVENotLastFlag, x2);
    ASSERT_EQUAL_64(SVENoneFlag | SVENotLastFlag, x3);
  }
}

static void PtrueHelper(Test* config,
                        unsigned lane_size_in_bits,
                        FlagsUpdate s = LeaveFlags) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  PRegisterWithLaneSize p[kNumberOfPRegisters];
  for (unsigned i = 0; i < kNumberOfPRegisters; i++) {
    p[i] = PRegister(i).WithLaneSize(lane_size_in_bits);
  }

  // Initialise NZCV to an impossible value, to check that we actually write it.
  StatusFlags nzcv_unmodified = NZCVFlag;
  __ Mov(x20, nzcv_unmodified);

  // We don't have enough registers to conveniently test every pattern, so take
  // samples from each group.
  __ Msr(NZCV, x20);
  __ Ptrue(p[0], SVE_POW2, s);
  __ Mrs(x0, NZCV);

  __ Msr(NZCV, x20);
  __ Ptrue(p[1], SVE_VL1, s);
  __ Mrs(x1, NZCV);

  __ Msr(NZCV, x20);
  __ Ptrue(p[2], SVE_VL2, s);
  __ Mrs(x2, NZCV);

  __ Msr(NZCV, x20);
  __ Ptrue(p[3], SVE_VL5, s);
  __ Mrs(x3, NZCV);

  __ Msr(NZCV, x20);
  __ Ptrue(p[4], SVE_VL6, s);
  __ Mrs(x4, NZCV);

  __ Msr(NZCV, x20);
  __ Ptrue(p[5], SVE_VL8, s);
  __ Mrs(x5, NZCV);

  __ Msr(NZCV, x20);
  __ Ptrue(p[6], SVE_VL16, s);
  __ Mrs(x6, NZCV);

  __ Msr(NZCV, x20);
  __ Ptrue(p[7], SVE_VL64, s);
  __ Mrs(x7, NZCV);

  __ Msr(NZCV, x20);
  __ Ptrue(p[8], SVE_VL256, s);
  __ Mrs(x8, NZCV);

  {
    // We have to use the Assembler to use values not defined by
    // SVEPredicateConstraint, so call `ptrues` directly..
    typedef void (
        MacroAssembler::*AssemblePtrueFn)(const PRegisterWithLaneSize& pd,
                                          int pattern);
    AssemblePtrueFn assemble =
        (s == SetFlags) ? &MacroAssembler::ptrues : &MacroAssembler::ptrue;

    ExactAssemblyScope guard(&masm, 12 * kInstructionSize);
    __ msr(NZCV, x20);
    (masm.*assemble)(p[9], 0xe);
    __ mrs(x9, NZCV);

    __ msr(NZCV, x20);
    (masm.*assemble)(p[10], 0x16);
    __ mrs(x10, NZCV);

    __ msr(NZCV, x20);
    (masm.*assemble)(p[11], 0x1a);
    __ mrs(x11, NZCV);

    __ msr(NZCV, x20);
    (masm.*assemble)(p[12], 0x1c);
    __ mrs(x12, NZCV);
  }

  __ Msr(NZCV, x20);
  __ Ptrue(p[13], SVE_MUL4, s);
  __ Mrs(x13, NZCV);

  __ Msr(NZCV, x20);
  __ Ptrue(p[14], SVE_MUL3, s);
  __ Mrs(x14, NZCV);

  __ Msr(NZCV, x20);
  __ Ptrue(p[15], SVE_ALL, s);
  __ Mrs(x15, NZCV);

  END();

  if (CAN_RUN()) {
    RUN();

    int all = core.GetSVELaneCount(lane_size_in_bits);
    int pow2 = 1 << HighestSetBitPosition(all);
    int mul4 = all - (all % 4);
    int mul3 = all - (all % 3);

    // Check P register results.
    for (int i = 0; i < all; i++) {
      ASSERT_EQUAL_SVE_LANE(i < pow2, p[0], i);
      ASSERT_EQUAL_SVE_LANE((all >= 1) && (i < 1), p[1], i);
      ASSERT_EQUAL_SVE_LANE((all >= 2) && (i < 2), p[2], i);
      ASSERT_EQUAL_SVE_LANE((all >= 5) && (i < 5), p[3], i);
      ASSERT_EQUAL_SVE_LANE((all >= 6) && (i < 6), p[4], i);
      ASSERT_EQUAL_SVE_LANE((all >= 8) && (i < 8), p[5], i);
      ASSERT_EQUAL_SVE_LANE((all >= 16) && (i < 16), p[6], i);
      ASSERT_EQUAL_SVE_LANE((all >= 64) && (i < 64), p[7], i);
      ASSERT_EQUAL_SVE_LANE((all >= 256) && (i < 256), p[8], i);
      ASSERT_EQUAL_SVE_LANE(false, p[9], i);
      ASSERT_EQUAL_SVE_LANE(false, p[10], i);
      ASSERT_EQUAL_SVE_LANE(false, p[11], i);
      ASSERT_EQUAL_SVE_LANE(false, p[12], i);
      ASSERT_EQUAL_SVE_LANE(i < mul4, p[13], i);
      ASSERT_EQUAL_SVE_LANE(i < mul3, p[14], i);
      ASSERT_EQUAL_SVE_LANE(true, p[15], i);
    }

    // Check NZCV results.
    if (s == LeaveFlags) {
      // No flags should have been updated.
      for (int i = 0; i <= 15; i++) {
        ASSERT_EQUAL_64(nzcv_unmodified, XRegister(i));
      }
    } else {
      StatusFlags zero = static_cast<StatusFlags>(SVENoneFlag | SVENotLastFlag);
      StatusFlags nonzero = SVEFirstFlag;

      // POW2
      ASSERT_EQUAL_64(nonzero, x0);
      // VL*
      ASSERT_EQUAL_64((all >= 1) ? nonzero : zero, x1);
      ASSERT_EQUAL_64((all >= 2) ? nonzero : zero, x2);
      ASSERT_EQUAL_64((all >= 5) ? nonzero : zero, x3);
      ASSERT_EQUAL_64((all >= 6) ? nonzero : zero, x4);
      ASSERT_EQUAL_64((all >= 8) ? nonzero : zero, x5);
      ASSERT_EQUAL_64((all >= 16) ? nonzero : zero, x6);
      ASSERT_EQUAL_64((all >= 64) ? nonzero : zero, x7);
      ASSERT_EQUAL_64((all >= 256) ? nonzero : zero, x8);
      // #uimm5
      ASSERT_EQUAL_64(zero, x9);
      ASSERT_EQUAL_64(zero, x10);
      ASSERT_EQUAL_64(zero, x11);
      ASSERT_EQUAL_64(zero, x12);
      // MUL*
      ASSERT_EQUAL_64((all >= 4) ? nonzero : zero, x13);
      ASSERT_EQUAL_64((all >= 3) ? nonzero : zero, x14);
      // ALL
      ASSERT_EQUAL_64(nonzero, x15);
    }
  }
}

TEST_SVE(sve_ptrue_b) { PtrueHelper(config, kBRegSize, LeaveFlags); }
TEST_SVE(sve_ptrue_h) { PtrueHelper(config, kHRegSize, LeaveFlags); }
TEST_SVE(sve_ptrue_s) { PtrueHelper(config, kSRegSize, LeaveFlags); }
TEST_SVE(sve_ptrue_d) { PtrueHelper(config, kDRegSize, LeaveFlags); }

TEST_SVE(sve_ptrues_b) { PtrueHelper(config, kBRegSize, SetFlags); }
TEST_SVE(sve_ptrues_h) { PtrueHelper(config, kHRegSize, SetFlags); }
TEST_SVE(sve_ptrues_s) { PtrueHelper(config, kSRegSize, SetFlags); }
TEST_SVE(sve_ptrues_d) { PtrueHelper(config, kDRegSize, SetFlags); }

TEST_SVE(sve_pfalse) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Initialise non-zero inputs.
  __ Ptrue(p0.VnB());
  __ Ptrue(p1.VnH());
  __ Ptrue(p2.VnS());
  __ Ptrue(p3.VnD());

  // The instruction only supports B-sized lanes, but the lane size has no
  // logical effect, so the MacroAssembler accepts anything.
  __ Pfalse(p0.VnB());
  __ Pfalse(p1.VnH());
  __ Pfalse(p2.VnS());
  __ Pfalse(p3.VnD());

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(0, p0.VnB());
    ASSERT_EQUAL_SVE(0, p1.VnB());
    ASSERT_EQUAL_SVE(0, p2.VnB());
    ASSERT_EQUAL_SVE(0, p3.VnB());
  }
}

TEST_SVE(sve_ptest) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Initialise NZCV to a known (impossible) value.
  StatusFlags nzcv_unmodified = NZCVFlag;
  __ Mov(x0, nzcv_unmodified);
  __ Msr(NZCV, x0);

  // Construct some test inputs.
  int in2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0};
  int in3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0};
  int in4[] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  __ Pfalse(p0.VnB());
  __ Ptrue(p1.VnB());
  Initialise(&masm, p2.VnB(), in2);
  Initialise(&masm, p3.VnB(), in3);
  Initialise(&masm, p4.VnB(), in4);

  // All-inactive pg.
  __ Ptest(p0, p0.VnB());
  __ Mrs(x0, NZCV);
  __ Ptest(p0, p1.VnB());
  __ Mrs(x1, NZCV);
  __ Ptest(p0, p2.VnB());
  __ Mrs(x2, NZCV);
  __ Ptest(p0, p3.VnB());
  __ Mrs(x3, NZCV);
  __ Ptest(p0, p4.VnB());
  __ Mrs(x4, NZCV);

  // All-active pg.
  __ Ptest(p1, p0.VnB());
  __ Mrs(x5, NZCV);
  __ Ptest(p1, p1.VnB());
  __ Mrs(x6, NZCV);
  __ Ptest(p1, p2.VnB());
  __ Mrs(x7, NZCV);
  __ Ptest(p1, p3.VnB());
  __ Mrs(x8, NZCV);
  __ Ptest(p1, p4.VnB());
  __ Mrs(x9, NZCV);

  // Combinations of other inputs.
  __ Ptest(p2, p2.VnB());
  __ Mrs(x20, NZCV);
  __ Ptest(p2, p3.VnB());
  __ Mrs(x21, NZCV);
  __ Ptest(p2, p4.VnB());
  __ Mrs(x22, NZCV);
  __ Ptest(p3, p2.VnB());
  __ Mrs(x23, NZCV);
  __ Ptest(p3, p3.VnB());
  __ Mrs(x24, NZCV);
  __ Ptest(p3, p4.VnB());
  __ Mrs(x25, NZCV);
  __ Ptest(p4, p2.VnB());
  __ Mrs(x26, NZCV);
  __ Ptest(p4, p3.VnB());
  __ Mrs(x27, NZCV);
  __ Ptest(p4, p4.VnB());
  __ Mrs(x28, NZCV);

  END();

  if (CAN_RUN()) {
    RUN();

    StatusFlags zero = static_cast<StatusFlags>(SVENoneFlag | SVENotLastFlag);

    // If pg is all inactive, the value of pn is irrelevant.
    ASSERT_EQUAL_64(zero, x0);
    ASSERT_EQUAL_64(zero, x1);
    ASSERT_EQUAL_64(zero, x2);
    ASSERT_EQUAL_64(zero, x3);
    ASSERT_EQUAL_64(zero, x4);

    // All-active pg.
    ASSERT_EQUAL_64(zero, x5);          // All-inactive pn.
    ASSERT_EQUAL_64(SVEFirstFlag, x6);  // All-active pn.
    // Other pn inputs are non-zero, but the first and last lanes are inactive.
    ASSERT_EQUAL_64(SVENotLastFlag, x7);
    ASSERT_EQUAL_64(SVENotLastFlag, x8);
    ASSERT_EQUAL_64(SVENotLastFlag, x9);

    // Other inputs.
    ASSERT_EQUAL_64(SVEFirstFlag, x20);  // pg: in2, pn: in2
    ASSERT_EQUAL_64(NoFlag, x21);        // pg: in2, pn: in3
    ASSERT_EQUAL_64(zero, x22);          // pg: in2, pn: in4
    ASSERT_EQUAL_64(static_cast<StatusFlags>(SVEFirstFlag | SVENotLastFlag),
                    x23);                // pg: in3, pn: in2
    ASSERT_EQUAL_64(SVEFirstFlag, x24);  // pg: in3, pn: in3
    ASSERT_EQUAL_64(zero, x25);          // pg: in3, pn: in4
    ASSERT_EQUAL_64(zero, x26);          // pg: in4, pn: in2
    ASSERT_EQUAL_64(zero, x27);          // pg: in4, pn: in3
    ASSERT_EQUAL_64(SVEFirstFlag, x28);  // pg: in4, pn: in4
  }
}

TEST_SVE(sve_cntp) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // There are {7, 5, 2, 1} active {B, H, S, D} lanes.
  int p0_inputs[] = {0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0};
  Initialise(&masm, p0.VnB(), p0_inputs);

  // With an all-true predicate, these instructions measure the vector length.
  __ Ptrue(p10.VnB());
  __ Ptrue(p11.VnH());
  __ Ptrue(p12.VnS());
  __ Ptrue(p13.VnD());

  // `ptrue p10.b` provides an all-active pg.
  __ Cntp(x10, p10, p10.VnB());
  __ Cntp(x11, p10, p11.VnH());
  __ Cntp(x12, p10, p12.VnS());
  __ Cntp(x13, p10, p13.VnD());

  // Check that the predicate mask is applied properly.
  __ Cntp(x14, p10, p10.VnB());
  __ Cntp(x15, p11, p10.VnB());
  __ Cntp(x16, p12, p10.VnB());
  __ Cntp(x17, p13, p10.VnB());

  // Check other patterns (including some ignored bits).
  __ Cntp(x0, p10, p0.VnB());
  __ Cntp(x1, p10, p0.VnH());
  __ Cntp(x2, p10, p0.VnS());
  __ Cntp(x3, p10, p0.VnD());
  __ Cntp(x4, p0, p10.VnB());
  __ Cntp(x5, p0, p10.VnH());
  __ Cntp(x6, p0, p10.VnS());
  __ Cntp(x7, p0, p10.VnD());

  END();

  if (CAN_RUN()) {
    RUN();

    int vl_b = core.GetSVELaneCount(kBRegSize);
    int vl_h = core.GetSVELaneCount(kHRegSize);
    int vl_s = core.GetSVELaneCount(kSRegSize);
    int vl_d = core.GetSVELaneCount(kDRegSize);

    // Check all-active predicates in various combinations.
    ASSERT_EQUAL_64(vl_b, x10);
    ASSERT_EQUAL_64(vl_h, x11);
    ASSERT_EQUAL_64(vl_s, x12);
    ASSERT_EQUAL_64(vl_d, x13);

    ASSERT_EQUAL_64(vl_b, x14);
    ASSERT_EQUAL_64(vl_h, x15);
    ASSERT_EQUAL_64(vl_s, x16);
    ASSERT_EQUAL_64(vl_d, x17);

    // Check that irrelevant bits are properly ignored.
    ASSERT_EQUAL_64(7, x0);
    ASSERT_EQUAL_64(5, x1);
    ASSERT_EQUAL_64(2, x2);
    ASSERT_EQUAL_64(1, x3);

    ASSERT_EQUAL_64(7, x4);
    ASSERT_EQUAL_64(5, x5);
    ASSERT_EQUAL_64(2, x6);
    ASSERT_EQUAL_64(1, x7);
  }
}

typedef void (MacroAssembler::*CntFn)(const Register& dst,
                                      int pattern,
                                      int multiplier);

template <typename T>
void GenerateCntSequence(MacroAssembler* masm,
                         CntFn cnt,
                         T acc_value,
                         int multiplier) {
  // Initialise accumulators.
  masm->Mov(x0, acc_value);
  masm->Mov(x1, acc_value);
  masm->Mov(x2, acc_value);
  masm->Mov(x3, acc_value);
  masm->Mov(x4, acc_value);
  masm->Mov(x5, acc_value);
  masm->Mov(x6, acc_value);
  masm->Mov(x7, acc_value);
  masm->Mov(x8, acc_value);
  masm->Mov(x9, acc_value);
  masm->Mov(x10, acc_value);
  masm->Mov(x11, acc_value);
  masm->Mov(x12, acc_value);
  masm->Mov(x13, acc_value);
  masm->Mov(x14, acc_value);
  masm->Mov(x15, acc_value);
  masm->Mov(x18, acc_value);
  masm->Mov(x19, acc_value);
  masm->Mov(x20, acc_value);
  masm->Mov(x21, acc_value);

  (masm->*cnt)(Register(0, sizeof(T) * kBitsPerByte), SVE_POW2, multiplier);
  (masm->*cnt)(Register(1, sizeof(T) * kBitsPerByte), SVE_VL1, multiplier);
  (masm->*cnt)(Register(2, sizeof(T) * kBitsPerByte), SVE_VL2, multiplier);
  (masm->*cnt)(Register(3, sizeof(T) * kBitsPerByte), SVE_VL3, multiplier);
  (masm->*cnt)(Register(4, sizeof(T) * kBitsPerByte), SVE_VL4, multiplier);
  (masm->*cnt)(Register(5, sizeof(T) * kBitsPerByte), SVE_VL5, multiplier);
  (masm->*cnt)(Register(6, sizeof(T) * kBitsPerByte), SVE_VL6, multiplier);
  (masm->*cnt)(Register(7, sizeof(T) * kBitsPerByte), SVE_VL7, multiplier);
  (masm->*cnt)(Register(8, sizeof(T) * kBitsPerByte), SVE_VL8, multiplier);
  (masm->*cnt)(Register(9, sizeof(T) * kBitsPerByte), SVE_VL16, multiplier);
  (masm->*cnt)(Register(10, sizeof(T) * kBitsPerByte), SVE_VL32, multiplier);
  (masm->*cnt)(Register(11, sizeof(T) * kBitsPerByte), SVE_VL64, multiplier);
  (masm->*cnt)(Register(12, sizeof(T) * kBitsPerByte), SVE_VL128, multiplier);
  (masm->*cnt)(Register(13, sizeof(T) * kBitsPerByte), SVE_VL256, multiplier);
  (masm->*cnt)(Register(14, sizeof(T) * kBitsPerByte), 16, multiplier);
  (masm->*cnt)(Register(15, sizeof(T) * kBitsPerByte), 23, multiplier);
  (masm->*cnt)(Register(18, sizeof(T) * kBitsPerByte), 28, multiplier);
  (masm->*cnt)(Register(19, sizeof(T) * kBitsPerByte), SVE_MUL4, multiplier);
  (masm->*cnt)(Register(20, sizeof(T) * kBitsPerByte), SVE_MUL3, multiplier);
  (masm->*cnt)(Register(21, sizeof(T) * kBitsPerByte), SVE_ALL, multiplier);
}

int FixedVL(int fixed, int length) {
  VIXL_ASSERT(((fixed >= 1) && (fixed <= 8)) || (fixed == 16) ||
              (fixed == 32) || (fixed == 64) || (fixed == 128) ||
              (fixed = 256));
  return (length >= fixed) ? fixed : 0;
}

static void CntHelper(Test* config,
                      CntFn cnt,
                      int multiplier,
                      int lane_size_in_bits,
                      int64_t acc_value = 0,
                      bool is_increment = true) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();
  GenerateCntSequence(&masm, cnt, acc_value, multiplier);
  END();

  if (CAN_RUN()) {
    RUN();

    int all = core.GetSVELaneCount(lane_size_in_bits);
    int pow2 = 1 << HighestSetBitPosition(all);
    int mul4 = all - (all % 4);
    int mul3 = all - (all % 3);

    multiplier = is_increment ? multiplier : -multiplier;

    ASSERT_EQUAL_64(acc_value + (multiplier * pow2), x0);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(1, all)), x1);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(2, all)), x2);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(3, all)), x3);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(4, all)), x4);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(5, all)), x5);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(6, all)), x6);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(7, all)), x7);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(8, all)), x8);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(16, all)), x9);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(32, all)), x10);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(64, all)), x11);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(128, all)), x12);
    ASSERT_EQUAL_64(acc_value + (multiplier * FixedVL(256, all)), x13);
    ASSERT_EQUAL_64(acc_value, x14);
    ASSERT_EQUAL_64(acc_value, x15);
    ASSERT_EQUAL_64(acc_value, x18);
    ASSERT_EQUAL_64(acc_value + (multiplier * mul4), x19);
    ASSERT_EQUAL_64(acc_value + (multiplier * mul3), x20);
    ASSERT_EQUAL_64(acc_value + (multiplier * all), x21);
  }
}

static void IncHelper(Test* config,
                      CntFn cnt,
                      int multiplier,
                      int lane_size_in_bits,
                      int64_t acc_value) {
  CntHelper(config, cnt, multiplier, lane_size_in_bits, acc_value, true);
}

static void DecHelper(Test* config,
                      CntFn cnt,
                      int multiplier,
                      int lane_size_in_bits,
                      int64_t acc_value) {
  CntHelper(config, cnt, multiplier, lane_size_in_bits, acc_value, false);
}

TEST_SVE(sve_cntb) {
  CntHelper(config, &MacroAssembler::Cntb, 1, kBRegSize);
  CntHelper(config, &MacroAssembler::Cntb, 2, kBRegSize);
  CntHelper(config, &MacroAssembler::Cntb, 15, kBRegSize);
  CntHelper(config, &MacroAssembler::Cntb, 16, kBRegSize);
}

TEST_SVE(sve_cnth) {
  CntHelper(config, &MacroAssembler::Cnth, 1, kHRegSize);
  CntHelper(config, &MacroAssembler::Cnth, 2, kHRegSize);
  CntHelper(config, &MacroAssembler::Cnth, 15, kHRegSize);
  CntHelper(config, &MacroAssembler::Cnth, 16, kHRegSize);
}

TEST_SVE(sve_cntw) {
  CntHelper(config, &MacroAssembler::Cntw, 1, kWRegSize);
  CntHelper(config, &MacroAssembler::Cntw, 2, kWRegSize);
  CntHelper(config, &MacroAssembler::Cntw, 15, kWRegSize);
  CntHelper(config, &MacroAssembler::Cntw, 16, kWRegSize);
}

TEST_SVE(sve_cntd) {
  CntHelper(config, &MacroAssembler::Cntd, 1, kDRegSize);
  CntHelper(config, &MacroAssembler::Cntd, 2, kDRegSize);
  CntHelper(config, &MacroAssembler::Cntd, 15, kDRegSize);
  CntHelper(config, &MacroAssembler::Cntd, 16, kDRegSize);
}

TEST_SVE(sve_decb) {
  DecHelper(config, &MacroAssembler::Decb, 1, kBRegSize, 42);
  DecHelper(config, &MacroAssembler::Decb, 2, kBRegSize, -1);
  DecHelper(config, &MacroAssembler::Decb, 15, kBRegSize, INT64_MIN);
  DecHelper(config, &MacroAssembler::Decb, 16, kBRegSize, -42);
}

TEST_SVE(sve_dech) {
  DecHelper(config, &MacroAssembler::Dech, 1, kHRegSize, 42);
  DecHelper(config, &MacroAssembler::Dech, 2, kHRegSize, -1);
  DecHelper(config, &MacroAssembler::Dech, 15, kHRegSize, INT64_MIN);
  DecHelper(config, &MacroAssembler::Dech, 16, kHRegSize, -42);
}

TEST_SVE(sve_decw) {
  DecHelper(config, &MacroAssembler::Decw, 1, kWRegSize, 42);
  DecHelper(config, &MacroAssembler::Decw, 2, kWRegSize, -1);
  DecHelper(config, &MacroAssembler::Decw, 15, kWRegSize, INT64_MIN);
  DecHelper(config, &MacroAssembler::Decw, 16, kWRegSize, -42);
}

TEST_SVE(sve_decd) {
  DecHelper(config, &MacroAssembler::Decd, 1, kDRegSize, 42);
  DecHelper(config, &MacroAssembler::Decd, 2, kDRegSize, -1);
  DecHelper(config, &MacroAssembler::Decd, 15, kDRegSize, INT64_MIN);
  DecHelper(config, &MacroAssembler::Decd, 16, kDRegSize, -42);
}

TEST_SVE(sve_incb) {
  IncHelper(config, &MacroAssembler::Incb, 1, kBRegSize, 42);
  IncHelper(config, &MacroAssembler::Incb, 2, kBRegSize, -1);
  IncHelper(config, &MacroAssembler::Incb, 15, kBRegSize, INT64_MAX);
  IncHelper(config, &MacroAssembler::Incb, 16, kBRegSize, -42);
}

TEST_SVE(sve_inch) {
  IncHelper(config, &MacroAssembler::Inch, 1, kHRegSize, 42);
  IncHelper(config, &MacroAssembler::Inch, 2, kHRegSize, -1);
  IncHelper(config, &MacroAssembler::Inch, 15, kHRegSize, INT64_MAX);
  IncHelper(config, &MacroAssembler::Inch, 16, kHRegSize, -42);
}

TEST_SVE(sve_incw) {
  IncHelper(config, &MacroAssembler::Incw, 1, kWRegSize, 42);
  IncHelper(config, &MacroAssembler::Incw, 2, kWRegSize, -1);
  IncHelper(config, &MacroAssembler::Incw, 15, kWRegSize, INT64_MAX);
  IncHelper(config, &MacroAssembler::Incw, 16, kWRegSize, -42);
}

TEST_SVE(sve_incd) {
  IncHelper(config, &MacroAssembler::Incd, 1, kDRegSize, 42);
  IncHelper(config, &MacroAssembler::Incd, 2, kDRegSize, -1);
  IncHelper(config, &MacroAssembler::Incd, 15, kDRegSize, INT64_MAX);
  IncHelper(config, &MacroAssembler::Incd, 16, kDRegSize, -42);
}

template <typename T>
static T QAdd(T x, int y) {
  VIXL_ASSERT(y > INT_MIN);
  T result;
  T min = std::numeric_limits<T>::min();
  T max = std::numeric_limits<T>::max();
  if ((x >= 0) && (y >= 0)) {
    // For positive a and b, saturate at max.
    result = (max - x) < static_cast<T>(y) ? max : x + y;
  } else if ((y < 0) && ((x < 0) || (min == 0))) {
    // For negative b, where either a negative or T unsigned.
    result = (x - min) < static_cast<T>(-y) ? min : x + y;
  } else {
    result = x + y;
  }
  return result;
}

template <typename T>
static void QIncDecHelper(Test* config,
                          CntFn cnt,
                          int multiplier,
                          int lane_size_in_bits,
                          T acc_value,
                          bool is_increment) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();
  GenerateCntSequence(&masm, cnt, acc_value, multiplier);
  END();

  if (CAN_RUN()) {
    RUN();

    int all = core.GetSVELaneCount(lane_size_in_bits);
    int pow2 = 1 << HighestSetBitPosition(all);
    int mul4 = all - (all % 4);
    int mul3 = all - (all % 3);

    multiplier = is_increment ? multiplier : -multiplier;

    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * pow2), x0);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(1, all)), x1);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(2, all)), x2);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(3, all)), x3);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(4, all)), x4);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(5, all)), x5);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(6, all)), x6);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(7, all)), x7);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(8, all)), x8);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(16, all)), x9);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(32, all)), x10);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(64, all)), x11);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(128, all)), x12);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(256, all)), x13);
    ASSERT_EQUAL_64(acc_value, x14);
    ASSERT_EQUAL_64(acc_value, x15);
    ASSERT_EQUAL_64(acc_value, x18);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * mul4), x19);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * mul3), x20);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * all), x21);
  }
}

template <typename T>
static void QIncHelper(Test* config,
                       CntFn cnt,
                       int multiplier,
                       int lane_size_in_bits,
                       T acc_value) {
  QIncDecHelper<T>(config, cnt, multiplier, lane_size_in_bits, acc_value, true);
}

template <typename T>
static void QDecHelper(Test* config,
                       CntFn cnt,
                       int multiplier,
                       int lane_size_in_bits,
                       T acc_value) {
  QIncDecHelper<T>(config,
                   cnt,
                   multiplier,
                   lane_size_in_bits,
                   acc_value,
                   false);
}

TEST_SVE(sve_sqdecb) {
  int64_t bigneg = INT64_MIN + 42;
  int64_t bigpos = INT64_MAX - 42;
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdecb, 1, kBRegSize, 1);
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdecb, 2, kBRegSize, bigneg);
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdecb, 15, kBRegSize, 999);
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdecb, 16, kBRegSize, bigpos);
}

TEST_SVE(sve_sqdech) {
  int64_t bigneg = INT64_MIN + 42;
  int64_t bigpos = INT64_MAX - 42;
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdech, 1, kHRegSize, 1);
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdech, 2, kHRegSize, bigneg);
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdech, 15, kHRegSize, 999);
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdech, 16, kHRegSize, bigpos);
}

TEST_SVE(sve_sqdecw) {
  int64_t bigneg = INT64_MIN + 42;
  int64_t bigpos = INT64_MAX - 42;
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdecw, 1, kWRegSize, 1);
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdecw, 2, kWRegSize, bigneg);
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdecw, 15, kWRegSize, 999);
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdecw, 16, kWRegSize, bigpos);
}

TEST_SVE(sve_sqdecd) {
  int64_t bigneg = INT64_MIN + 42;
  int64_t bigpos = INT64_MAX - 42;
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdecd, 1, kDRegSize, 1);
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdecd, 2, kDRegSize, bigneg);
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdecd, 15, kDRegSize, 999);
  QDecHelper<int64_t>(config, &MacroAssembler::Sqdecd, 16, kDRegSize, bigpos);
}

TEST_SVE(sve_sqincb) {
  int64_t bigneg = INT64_MIN + 42;
  int64_t bigpos = INT64_MAX - 42;
  QIncHelper<int64_t>(config, &MacroAssembler::Sqincb, 1, kBRegSize, 1);
  QIncHelper<int64_t>(config, &MacroAssembler::Sqincb, 2, kBRegSize, bigneg);
  QIncHelper<int64_t>(config, &MacroAssembler::Sqincb, 15, kBRegSize, 999);
  QIncHelper<int64_t>(config, &MacroAssembler::Sqincb, 16, kBRegSize, bigpos);
}

TEST_SVE(sve_sqinch) {
  int64_t bigneg = INT64_MIN + 42;
  int64_t bigpos = INT64_MAX - 42;
  QIncHelper<int64_t>(config, &MacroAssembler::Sqinch, 1, kHRegSize, 1);
  QIncHelper<int64_t>(config, &MacroAssembler::Sqinch, 2, kHRegSize, bigneg);
  QIncHelper<int64_t>(config, &MacroAssembler::Sqinch, 15, kHRegSize, 999);
  QIncHelper<int64_t>(config, &MacroAssembler::Sqinch, 16, kHRegSize, bigpos);
}

TEST_SVE(sve_sqincw) {
  int64_t bigneg = INT64_MIN + 42;
  int64_t bigpos = INT64_MAX - 42;
  QIncHelper<int64_t>(config, &MacroAssembler::Sqincw, 1, kWRegSize, 1);
  QIncHelper<int64_t>(config, &MacroAssembler::Sqincw, 2, kWRegSize, bigneg);
  QIncHelper<int64_t>(config, &MacroAssembler::Sqincw, 15, kWRegSize, 999);
  QIncHelper<int64_t>(config, &MacroAssembler::Sqincw, 16, kWRegSize, bigpos);
}

TEST_SVE(sve_sqincd) {
  int64_t bigneg = INT64_MIN + 42;
  int64_t bigpos = INT64_MAX - 42;
  QIncHelper<int64_t>(config, &MacroAssembler::Sqincd, 1, kDRegSize, 1);
  QIncHelper<int64_t>(config, &MacroAssembler::Sqincd, 2, kDRegSize, bigneg);
  QIncHelper<int64_t>(config, &MacroAssembler::Sqincd, 15, kDRegSize, 999);
  QIncHelper<int64_t>(config, &MacroAssembler::Sqincd, 16, kDRegSize, bigpos);
}

TEST_SVE(sve_uqdecb) {
  int32_t big32 = UINT32_MAX - 42;
  int64_t big64 = UINT64_MAX - 42;
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdecb, 1, kBRegSize, 1);
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdecb, 2, kBRegSize, 42);
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdecb, 15, kBRegSize, 999);
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdecb, 16, kBRegSize, big32);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdecb, 1, kBRegSize, 1);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdecb, 2, kBRegSize, 42);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdecb, 15, kBRegSize, 999);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdecb, 16, kBRegSize, big64);
}

TEST_SVE(sve_uqdech) {
  int32_t big32 = UINT32_MAX - 42;
  int64_t big64 = UINT64_MAX - 42;
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdech, 1, kHRegSize, 1);
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdech, 2, kHRegSize, 42);
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdech, 15, kHRegSize, 999);
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdech, 16, kHRegSize, big32);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdech, 1, kHRegSize, 1);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdech, 2, kHRegSize, 42);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdech, 15, kHRegSize, 999);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdech, 16, kHRegSize, big64);
}

TEST_SVE(sve_uqdecw) {
  int32_t big32 = UINT32_MAX - 42;
  int64_t big64 = UINT64_MAX - 42;
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdecw, 1, kWRegSize, 1);
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdecw, 2, kWRegSize, 42);
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdecw, 15, kWRegSize, 999);
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdecw, 16, kWRegSize, big32);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdecw, 1, kWRegSize, 1);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdecw, 2, kWRegSize, 42);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdecw, 15, kWRegSize, 999);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdecw, 16, kWRegSize, big64);
}

TEST_SVE(sve_uqdecd) {
  int32_t big32 = UINT32_MAX - 42;
  int64_t big64 = UINT64_MAX - 42;
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdecd, 1, kDRegSize, 1);
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdecd, 2, kDRegSize, 42);
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdecd, 15, kDRegSize, 999);
  QDecHelper<uint32_t>(config, &MacroAssembler::Uqdecd, 16, kDRegSize, big32);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdecd, 1, kDRegSize, 1);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdecd, 2, kDRegSize, 42);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdecd, 15, kDRegSize, 999);
  QDecHelper<uint64_t>(config, &MacroAssembler::Uqdecd, 16, kDRegSize, big64);
}

TEST_SVE(sve_uqincb) {
  int32_t big32 = UINT32_MAX - 42;
  int64_t big64 = UINT64_MAX - 42;
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqincb, 1, kBRegSize, 1);
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqincb, 2, kBRegSize, 42);
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqincb, 15, kBRegSize, 999);
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqincb, 16, kBRegSize, big32);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqincb, 1, kBRegSize, 1);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqincb, 2, kBRegSize, 42);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqincb, 15, kBRegSize, 999);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqincb, 16, kBRegSize, big64);
}

TEST_SVE(sve_uqinch) {
  int32_t big32 = UINT32_MAX - 42;
  int64_t big64 = UINT64_MAX - 42;
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqinch, 1, kHRegSize, 1);
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqinch, 2, kHRegSize, 42);
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqinch, 15, kHRegSize, 999);
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqinch, 16, kHRegSize, big32);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqinch, 1, kHRegSize, 1);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqinch, 2, kHRegSize, 42);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqinch, 15, kHRegSize, 999);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqinch, 16, kHRegSize, big64);
}

TEST_SVE(sve_uqincw) {
  int32_t big32 = UINT32_MAX - 42;
  int64_t big64 = UINT64_MAX - 42;
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqincw, 1, kWRegSize, 1);
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqincw, 2, kWRegSize, 42);
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqincw, 15, kWRegSize, 999);
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqincw, 16, kWRegSize, big32);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqincw, 1, kWRegSize, 1);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqincw, 2, kWRegSize, 42);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqincw, 15, kWRegSize, 999);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqincw, 16, kWRegSize, big64);
}

TEST_SVE(sve_uqincd) {
  int32_t big32 = UINT32_MAX - 42;
  int64_t big64 = UINT64_MAX - 42;
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqincd, 1, kDRegSize, 1);
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqincd, 2, kDRegSize, 42);
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqincd, 15, kDRegSize, 999);
  QIncHelper<uint32_t>(config, &MacroAssembler::Uqincd, 16, kDRegSize, big32);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqincd, 1, kDRegSize, 1);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqincd, 2, kDRegSize, 42);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqincd, 15, kDRegSize, 999);
  QIncHelper<uint64_t>(config, &MacroAssembler::Uqincd, 16, kDRegSize, big64);
}

typedef void (MacroAssembler::*QIncDecXWFn)(const Register& dst,
                                            const Register& src,
                                            int pattern,
                                            int multiplier);

static void QIncDecXWHelper(Test* config,
                            QIncDecXWFn cnt,
                            int multiplier,
                            int lane_size_in_bits,
                            int32_t acc_value,
                            bool is_increment) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Initialise accumulators.
  __ Mov(x0, acc_value);
  __ Mov(x1, acc_value);
  __ Mov(x2, acc_value);
  __ Mov(x3, acc_value);
  __ Mov(x4, acc_value);
  __ Mov(x5, acc_value);
  __ Mov(x6, acc_value);
  __ Mov(x7, acc_value);
  __ Mov(x8, acc_value);
  __ Mov(x9, acc_value);
  __ Mov(x10, acc_value);
  __ Mov(x11, acc_value);
  __ Mov(x12, acc_value);
  __ Mov(x13, acc_value);
  __ Mov(x14, acc_value);
  __ Mov(x15, acc_value);
  __ Mov(x18, acc_value);
  __ Mov(x19, acc_value);
  __ Mov(x20, acc_value);
  __ Mov(x21, acc_value);

  (masm.*cnt)(x0, w0, SVE_POW2, multiplier);
  (masm.*cnt)(x1, w1, SVE_VL1, multiplier);
  (masm.*cnt)(x2, w2, SVE_VL2, multiplier);
  (masm.*cnt)(x3, w3, SVE_VL3, multiplier);
  (masm.*cnt)(x4, w4, SVE_VL4, multiplier);
  (masm.*cnt)(x5, w5, SVE_VL5, multiplier);
  (masm.*cnt)(x6, w6, SVE_VL6, multiplier);
  (masm.*cnt)(x7, w7, SVE_VL7, multiplier);
  (masm.*cnt)(x8, w8, SVE_VL8, multiplier);
  (masm.*cnt)(x9, w9, SVE_VL16, multiplier);
  (masm.*cnt)(x10, w10, SVE_VL32, multiplier);
  (masm.*cnt)(x11, w11, SVE_VL64, multiplier);
  (masm.*cnt)(x12, w12, SVE_VL128, multiplier);
  (masm.*cnt)(x13, w13, SVE_VL256, multiplier);
  (masm.*cnt)(x14, w14, 16, multiplier);
  (masm.*cnt)(x15, w15, 23, multiplier);
  (masm.*cnt)(x18, w18, 28, multiplier);
  (masm.*cnt)(x19, w19, SVE_MUL4, multiplier);
  (masm.*cnt)(x20, w20, SVE_MUL3, multiplier);
  (masm.*cnt)(x21, w21, SVE_ALL, multiplier);

  END();

  if (CAN_RUN()) {
    RUN();

    int all = core.GetSVELaneCount(lane_size_in_bits);
    int pow2 = 1 << HighestSetBitPosition(all);
    int mul4 = all - (all % 4);
    int mul3 = all - (all % 3);

    multiplier = is_increment ? multiplier : -multiplier;

    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * pow2), x0);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(1, all)), x1);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(2, all)), x2);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(3, all)), x3);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(4, all)), x4);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(5, all)), x5);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(6, all)), x6);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(7, all)), x7);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(8, all)), x8);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(16, all)), x9);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(32, all)), x10);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(64, all)), x11);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(128, all)), x12);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * FixedVL(256, all)), x13);
    ASSERT_EQUAL_64(acc_value, x14);
    ASSERT_EQUAL_64(acc_value, x15);
    ASSERT_EQUAL_64(acc_value, x18);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * mul4), x19);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * mul3), x20);
    ASSERT_EQUAL_64(QAdd(acc_value, multiplier * all), x21);
  }
}

static void QIncXWHelper(Test* config,
                         QIncDecXWFn cnt,
                         int multiplier,
                         int lane_size_in_bits,
                         int32_t acc_value) {
  QIncDecXWHelper(config, cnt, multiplier, lane_size_in_bits, acc_value, true);
}

static void QDecXWHelper(Test* config,
                         QIncDecXWFn cnt,
                         int multiplier,
                         int lane_size_in_bits,
                         int32_t acc_value) {
  QIncDecXWHelper(config, cnt, multiplier, lane_size_in_bits, acc_value, false);
}

TEST_SVE(sve_sqdecb_xw) {
  QDecXWHelper(config, &MacroAssembler::Sqdecb, 1, kBRegSize, 1);
  QDecXWHelper(config, &MacroAssembler::Sqdecb, 2, kBRegSize, INT32_MIN + 42);
  QDecXWHelper(config, &MacroAssembler::Sqdecb, 15, kBRegSize, 999);
  QDecXWHelper(config, &MacroAssembler::Sqdecb, 16, kBRegSize, INT32_MAX - 42);
}

TEST_SVE(sve_sqdech_xw) {
  QDecXWHelper(config, &MacroAssembler::Sqdech, 1, kHRegSize, 1);
  QDecXWHelper(config, &MacroAssembler::Sqdech, 2, kHRegSize, INT32_MIN + 42);
  QDecXWHelper(config, &MacroAssembler::Sqdech, 15, kHRegSize, 999);
  QDecXWHelper(config, &MacroAssembler::Sqdech, 16, kHRegSize, INT32_MAX - 42);
}

TEST_SVE(sve_sqdecw_xw) {
  QDecXWHelper(config, &MacroAssembler::Sqdecw, 1, kWRegSize, 1);
  QDecXWHelper(config, &MacroAssembler::Sqdecw, 2, kWRegSize, INT32_MIN + 42);
  QDecXWHelper(config, &MacroAssembler::Sqdecw, 15, kWRegSize, 999);
  QDecXWHelper(config, &MacroAssembler::Sqdecw, 16, kWRegSize, INT32_MAX - 42);
}

TEST_SVE(sve_sqdecd_xw) {
  QDecXWHelper(config, &MacroAssembler::Sqdecd, 1, kDRegSize, 1);
  QDecXWHelper(config, &MacroAssembler::Sqdecd, 2, kDRegSize, INT32_MIN + 42);
  QDecXWHelper(config, &MacroAssembler::Sqdecd, 15, kDRegSize, 999);
  QDecXWHelper(config, &MacroAssembler::Sqdecd, 16, kDRegSize, INT32_MAX - 42);
}

TEST_SVE(sve_sqincb_xw) {
  QIncXWHelper(config, &MacroAssembler::Sqincb, 1, kBRegSize, 1);
  QIncXWHelper(config, &MacroAssembler::Sqincb, 2, kBRegSize, INT32_MIN + 42);
  QIncXWHelper(config, &MacroAssembler::Sqincb, 15, kBRegSize, 999);
  QIncXWHelper(config, &MacroAssembler::Sqincb, 16, kBRegSize, INT32_MAX - 42);
}

TEST_SVE(sve_sqinch_xw) {
  QIncXWHelper(config, &MacroAssembler::Sqinch, 1, kHRegSize, 1);
  QIncXWHelper(config, &MacroAssembler::Sqinch, 2, kHRegSize, INT32_MIN + 42);
  QIncXWHelper(config, &MacroAssembler::Sqinch, 15, kHRegSize, 999);
  QIncXWHelper(config, &MacroAssembler::Sqinch, 16, kHRegSize, INT32_MAX - 42);
}

TEST_SVE(sve_sqincw_xw) {
  QIncXWHelper(config, &MacroAssembler::Sqincw, 1, kWRegSize, 1);
  QIncXWHelper(config, &MacroAssembler::Sqincw, 2, kWRegSize, INT32_MIN + 42);
  QIncXWHelper(config, &MacroAssembler::Sqincw, 15, kWRegSize, 999);
  QIncXWHelper(config, &MacroAssembler::Sqincw, 16, kWRegSize, INT32_MAX - 42);
}

TEST_SVE(sve_sqincd_xw) {
  QIncXWHelper(config, &MacroAssembler::Sqincd, 1, kDRegSize, 1);
  QIncXWHelper(config, &MacroAssembler::Sqincd, 2, kDRegSize, INT32_MIN + 42);
  QIncXWHelper(config, &MacroAssembler::Sqincd, 15, kDRegSize, 999);
  QIncXWHelper(config, &MacroAssembler::Sqincd, 16, kDRegSize, INT32_MAX - 42);
}

typedef void (MacroAssembler::*IncDecZFn)(const ZRegister& dst,
                                          int pattern,
                                          int multiplier);
typedef void (MacroAssembler::*AddSubFn)(const ZRegister& dst,
                                         const ZRegister& src1,
                                         const ZRegister& src2);

static void IncDecZHelper(Test* config,
                          IncDecZFn fn,
                          CntFn cnt,
                          AddSubFn addsub,
                          int multiplier,
                          int lane_size_in_bits) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t acc_inputs[] = {0x7766554433221100,
                           0xffffffffffffffff,
                           0x0000000000000000,
                           0xffffffff0000ffff,
                           0x7fffffffffffffff,
                           0x8000000000000000,
                           0x7fffffff7fff7fff,
                           0x8000000080008000};

  for (unsigned i = 0; i < kNumberOfZRegisters; i++) {
    for (int j = 0; j < 4; j++) {
      InsrHelper(&masm, ZRegister(i, kDRegSize), acc_inputs);
    }
  }
  for (unsigned i = 0; i < 15; i++) {
    __ Mov(XRegister(i), 0);
  }

  (masm.*fn)(z16.WithLaneSize(lane_size_in_bits), SVE_POW2, multiplier);
  (masm.*fn)(z17.WithLaneSize(lane_size_in_bits), SVE_VL1, multiplier);
  (masm.*fn)(z18.WithLaneSize(lane_size_in_bits), SVE_VL2, multiplier);
  (masm.*fn)(z19.WithLaneSize(lane_size_in_bits), SVE_VL3, multiplier);
  (masm.*fn)(z20.WithLaneSize(lane_size_in_bits), SVE_VL4, multiplier);
  (masm.*fn)(z21.WithLaneSize(lane_size_in_bits), SVE_VL7, multiplier);
  (masm.*fn)(z22.WithLaneSize(lane_size_in_bits), SVE_VL8, multiplier);
  (masm.*fn)(z23.WithLaneSize(lane_size_in_bits), SVE_VL16, multiplier);
  (masm.*fn)(z24.WithLaneSize(lane_size_in_bits), SVE_VL64, multiplier);
  (masm.*fn)(z25.WithLaneSize(lane_size_in_bits), SVE_VL256, multiplier);
  (masm.*fn)(z26.WithLaneSize(lane_size_in_bits), 16, multiplier);
  (masm.*fn)(z27.WithLaneSize(lane_size_in_bits), 28, multiplier);
  (masm.*fn)(z28.WithLaneSize(lane_size_in_bits), SVE_MUL3, multiplier);
  (masm.*fn)(z29.WithLaneSize(lane_size_in_bits), SVE_MUL4, multiplier);
  (masm.*fn)(z30.WithLaneSize(lane_size_in_bits), SVE_ALL, multiplier);

  // Perform computation using alternative instructions.
  (masm.*cnt)(x0, SVE_POW2, multiplier);
  (masm.*cnt)(x1, SVE_VL1, multiplier);
  (masm.*cnt)(x2, SVE_VL2, multiplier);
  (masm.*cnt)(x3, SVE_VL3, multiplier);
  (masm.*cnt)(x4, SVE_VL4, multiplier);
  (masm.*cnt)(x5, SVE_VL7, multiplier);
  (masm.*cnt)(x6, SVE_VL8, multiplier);
  (masm.*cnt)(x7, SVE_VL16, multiplier);
  (masm.*cnt)(x8, SVE_VL64, multiplier);
  (masm.*cnt)(x9, SVE_VL256, multiplier);
  (masm.*cnt)(x10, 16, multiplier);
  (masm.*cnt)(x11, 28, multiplier);
  (masm.*cnt)(x12, SVE_MUL3, multiplier);
  (masm.*cnt)(x13, SVE_MUL4, multiplier);
  (masm.*cnt)(x14, SVE_ALL, multiplier);

  ZRegister zscratch = z15.WithLaneSize(lane_size_in_bits);
  for (unsigned i = 0; i < 15; i++) {
    ZRegister zsrcdst = ZRegister(i, lane_size_in_bits);
    Register x = Register(i, kXRegSize);
    __ Dup(zscratch, x);
    (masm.*addsub)(zsrcdst, zsrcdst, zscratch);
  }

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(z0, z16);
    ASSERT_EQUAL_SVE(z1, z17);
    ASSERT_EQUAL_SVE(z2, z18);
    ASSERT_EQUAL_SVE(z3, z19);
    ASSERT_EQUAL_SVE(z4, z20);
    ASSERT_EQUAL_SVE(z5, z21);
    ASSERT_EQUAL_SVE(z6, z22);
    ASSERT_EQUAL_SVE(z7, z23);
    ASSERT_EQUAL_SVE(z8, z24);
    ASSERT_EQUAL_SVE(z9, z25);
    ASSERT_EQUAL_SVE(z10, z26);
    ASSERT_EQUAL_SVE(z11, z27);
    ASSERT_EQUAL_SVE(z12, z28);
    ASSERT_EQUAL_SVE(z13, z29);
    ASSERT_EQUAL_SVE(z14, z30);
  }
}

TEST_SVE(sve_inc_dec_vec) {
  CntFn cnth = &MacroAssembler::Cnth;
  CntFn cntw = &MacroAssembler::Cntw;
  CntFn cntd = &MacroAssembler::Cntd;
  AddSubFn sub = &MacroAssembler::Sub;
  AddSubFn add = &MacroAssembler::Add;
  for (int mult = 1; mult <= 16; mult += 5) {
    IncDecZHelper(config, &MacroAssembler::Dech, cnth, sub, mult, kHRegSize);
    IncDecZHelper(config, &MacroAssembler::Decw, cntw, sub, mult, kSRegSize);
    IncDecZHelper(config, &MacroAssembler::Decd, cntd, sub, mult, kDRegSize);
    IncDecZHelper(config, &MacroAssembler::Inch, cnth, add, mult, kHRegSize);
    IncDecZHelper(config, &MacroAssembler::Incw, cntw, add, mult, kSRegSize);
    IncDecZHelper(config, &MacroAssembler::Incd, cntd, add, mult, kDRegSize);
  }
}

TEST_SVE(sve_unsigned_sat_inc_dec_vec) {
  CntFn cnth = &MacroAssembler::Cnth;
  CntFn cntw = &MacroAssembler::Cntw;
  CntFn cntd = &MacroAssembler::Cntd;
  AddSubFn sub = &MacroAssembler::Uqsub;
  AddSubFn add = &MacroAssembler::Uqadd;
  for (int mult = 1; mult <= 16; mult += 5) {
    IncDecZHelper(config, &MacroAssembler::Uqdech, cnth, sub, mult, kHRegSize);
    IncDecZHelper(config, &MacroAssembler::Uqdecw, cntw, sub, mult, kSRegSize);
    IncDecZHelper(config, &MacroAssembler::Uqdecd, cntd, sub, mult, kDRegSize);
    IncDecZHelper(config, &MacroAssembler::Uqinch, cnth, add, mult, kHRegSize);
    IncDecZHelper(config, &MacroAssembler::Uqincw, cntw, add, mult, kSRegSize);
    IncDecZHelper(config, &MacroAssembler::Uqincd, cntd, add, mult, kDRegSize);
  }
}

TEST_SVE(sve_signed_sat_inc_dec_vec) {
  CntFn cnth = &MacroAssembler::Cnth;
  CntFn cntw = &MacroAssembler::Cntw;
  CntFn cntd = &MacroAssembler::Cntd;
  AddSubFn sub = &MacroAssembler::Sqsub;
  AddSubFn add = &MacroAssembler::Sqadd;
  for (int mult = 1; mult <= 16; mult += 5) {
    IncDecZHelper(config, &MacroAssembler::Sqdech, cnth, sub, mult, kHRegSize);
    IncDecZHelper(config, &MacroAssembler::Sqdecw, cntw, sub, mult, kSRegSize);
    IncDecZHelper(config, &MacroAssembler::Sqdecd, cntd, sub, mult, kDRegSize);
    IncDecZHelper(config, &MacroAssembler::Sqinch, cnth, add, mult, kHRegSize);
    IncDecZHelper(config, &MacroAssembler::Sqincw, cntw, add, mult, kSRegSize);
    IncDecZHelper(config, &MacroAssembler::Sqincd, cntd, add, mult, kDRegSize);
  }
}

typedef void (MacroAssembler::*ArithPredicatedFn)(const ZRegister& zd,
                                                  const PRegisterM& pg,
                                                  const ZRegister& zn,
                                                  const ZRegister& zm);

template <typename Td, typename Tg, typename Tn>
static void IntBinArithHelper(Test* config,
                              ArithPredicatedFn macro,
                              unsigned lane_size_in_bits,
                              const Tg& pg_inputs,
                              const Tn& zn_inputs,
                              const Tn& zm_inputs,
                              const Td& zd_expected) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  ZRegister src_a = z31.WithLaneSize(lane_size_in_bits);
  ZRegister src_b = z27.WithLaneSize(lane_size_in_bits);
  InsrHelper(&masm, src_a, zn_inputs);
  InsrHelper(&masm, src_b, zm_inputs);

  Initialise(&masm, p0.WithLaneSize(lane_size_in_bits), pg_inputs);

  ZRegister zd_1 = z0.WithLaneSize(lane_size_in_bits);
  ZRegister zd_2 = z1.WithLaneSize(lane_size_in_bits);
  ZRegister zd_3 = z2.WithLaneSize(lane_size_in_bits);

  // `instr` zd(dst), zd(src_a), zn(src_b)
  __ Mov(zd_1, src_a);
  (masm.*macro)(zd_1, p0.Merging(), zd_1, src_b);

  // `instr` zd(dst), zm(src_a), zd(src_b)
  // Based on whether zd and zm registers are aliased, the macro of instructions
  // (`Instr`) swaps the order of operands if it has the commutative property,
  // otherwise, transfer to the reversed `Instr`, such as subr and divr.
  __ Mov(zd_2, src_b);
  (masm.*macro)(zd_2, p0.Merging(), src_a, zd_2);

  // `instr` zd(dst), zm(src_a), zn(src_b)
  // The macro of instructions (`Instr`) automatically selects between `instr`
  // and movprfx + `instr` based on whether zd and zn registers are aliased.
  // A generated movprfx instruction is predicated that using the same
  // governing predicate register. In order to keep the result constant,
  // initialize the destination register first.
  __ Mov(zd_3, src_a);
  (masm.*macro)(zd_3, p0.Merging(), src_a, src_b);

  END();

  if (CAN_RUN()) {
    RUN();
    ASSERT_EQUAL_SVE(zd_expected, zd_1);

    for (size_t i = 0; i < ArrayLength(zd_expected); i++) {
      int lane = static_cast<int>(ArrayLength(zd_expected) - i - 1);
      if (!core.HasSVELane(zd_1, lane)) break;
      if ((pg_inputs[i] & 1) != 0) {
        ASSERT_EQUAL_SVE_LANE(zd_expected[i], zd_1, lane);
      } else {
        ASSERT_EQUAL_SVE_LANE(zn_inputs[i], zd_1, lane);
      }
    }

    ASSERT_EQUAL_SVE(zd_expected, zd_3);
  }
}

TEST_SVE(sve_binary_arithmetic_predicated_add) {
  // clang-format off
  unsigned zn_b[] = {0x00, 0x01, 0x10, 0x81, 0xff, 0x0f, 0x01, 0x7f};

  unsigned zm_b[] = {0x00, 0x01, 0x10, 0x00, 0x81, 0x80, 0xff, 0xff};

  unsigned zn_h[] = {0x0000, 0x0123, 0x1010, 0x8181, 0xffff, 0x0f0f, 0x0101, 0x7f7f};

  unsigned zm_h[] = {0x0000, 0x0123, 0x1010, 0x0000, 0x8181, 0x8080, 0xffff, 0xffff};

  unsigned zn_s[] = {0x00000000, 0x01234567, 0x10101010, 0x81818181,
                     0xffffffff, 0x0f0f0f0f, 0x01010101, 0x7f7f7f7f};

  unsigned zm_s[] = {0x00000000, 0x01234567, 0x10101010, 0x00000000,
                     0x81818181, 0x80808080, 0xffffffff, 0xffffffff};

  uint64_t zn_d[] = {0x0000000000000000, 0x0123456789abcdef,
                     0x1010101010101010, 0x8181818181818181,
                     0xffffffffffffffff, 0x0f0f0f0f0f0f0f0f,
                     0x0101010101010101, 0x7f7f7f7fffffffff};

  uint64_t zm_d[] = {0x0000000000000000, 0x0123456789abcdef,
                     0x1010101010101010, 0x0000000000000000,
                     0x8181818181818181, 0x8080808080808080,
                     0xffffffffffffffff, 0xffffffffffffffff};

  int pg_b[] = {1, 1, 1, 0, 1, 1, 1, 0};
  int pg_h[] = {1, 1, 0, 1, 1, 1, 0, 1};
  int pg_s[] = {1, 0, 1, 1, 1, 0, 1, 1};
  int pg_d[] = {0, 1, 1, 1, 0, 1, 1, 1};

  unsigned add_exp_b[] = {0x00, 0x02, 0x20, 0x81, 0x80, 0x8f, 0x00, 0x7f};

  unsigned add_exp_h[] = {0x0000, 0x0246, 0x1010, 0x8181,
                          0x8180, 0x8f8f, 0x0101, 0x7f7e};

  unsigned add_exp_s[] = {0x00000000, 0x01234567, 0x20202020, 0x81818181,
                          0x81818180, 0x0f0f0f0f, 0x01010100, 0x7f7f7f7e};

  uint64_t add_exp_d[] = {0x0000000000000000, 0x02468acf13579bde,
                          0x2020202020202020, 0x8181818181818181,
                          0xffffffffffffffff, 0x8f8f8f8f8f8f8f8f,
                          0x0101010101010100, 0x7f7f7f7ffffffffe};

  ArithPredicatedFn fn = &MacroAssembler::Add;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, add_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, add_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, add_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, add_exp_d);

  unsigned sub_exp_b[] = {0x00, 0x00, 0x00, 0x81, 0x7e, 0x8f, 0x02, 0x7f};

  unsigned sub_exp_h[] = {0x0000, 0x0000, 0x1010, 0x8181,
                          0x7e7e, 0x8e8f, 0x0101, 0x7f80};

  unsigned sub_exp_s[] = {0x00000000, 0x01234567, 0x00000000, 0x81818181,
                          0x7e7e7e7e, 0x0f0f0f0f, 0x01010102, 0x7f7f7f80};

  uint64_t sub_exp_d[] = {0x0000000000000000, 0x0000000000000000,
                          0x0000000000000000, 0x8181818181818181,
                          0xffffffffffffffff, 0x8e8e8e8e8e8e8e8f,
                          0x0101010101010102, 0x7f7f7f8000000000};

  fn = &MacroAssembler::Sub;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, sub_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, sub_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, sub_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, sub_exp_d);
  // clang-format on
}

TEST_SVE(sve_binary_arithmetic_predicated_umin_umax_uabd) {
  // clang-format off
  unsigned zn_b[] = {0x00, 0xff, 0x0f, 0xff, 0xf0, 0x98, 0x55, 0x67};

  unsigned zm_b[] = {0x01, 0x00, 0x0e, 0xfe, 0xfe, 0xab, 0xcd, 0x78};

  unsigned zn_h[] = {0x0000, 0xffff, 0x00ff, 0xffff,
                     0xff00, 0xba98, 0x5555, 0x4567};

  unsigned zm_h[] = {0x0001, 0x0000, 0x00ee, 0xfffe,
                     0xfe00, 0xabab, 0xcdcd, 0x5678};

  unsigned zn_s[] = {0x00000000, 0xffffffff, 0x0000ffff, 0xffffffff,
                     0xffff0000, 0xfedcba98, 0x55555555, 0x01234567};

  unsigned zm_s[] = {0x00000001, 0x00000000, 0x0000eeee, 0xfffffffe,
                     0xfffe0000, 0xabababab, 0xcdcdcdcd, 0x12345678};

  uint64_t zn_d[] = {0x0000000000000000, 0xffffffffffffffff,
                     0x5555555555555555, 0x0000000001234567};

  uint64_t zm_d[] = {0x0000000000000001, 0x0000000000000000,
                     0xcdcdcdcdcdcdcdcd, 0x0000000012345678};

  int pg_b[] = {1, 1, 1, 0, 1, 1, 1, 0};
  int pg_h[] = {1, 1, 0, 1, 1, 1, 0, 1};
  int pg_s[] = {1, 0, 1, 1, 1, 0, 1, 1};
  int pg_d[] = {1, 0, 1, 1};

  unsigned umax_exp_b[] = {0x01, 0xff, 0x0f, 0xff, 0xfe, 0xab, 0xcd, 0x67};

  unsigned umax_exp_h[] = {0x0001, 0xffff, 0x00ff, 0xffff,
                           0xff00, 0xba98, 0x5555, 0x5678};

  unsigned umax_exp_s[] = {0x00000001, 0xffffffff, 0x0000ffff, 0xffffffff,
                           0xffff0000, 0xfedcba98, 0xcdcdcdcd, 0x12345678};

  uint64_t umax_exp_d[] = {0x0000000000000001, 0xffffffffffffffff,
                           0xcdcdcdcdcdcdcdcd, 0x0000000012345678};

  ArithPredicatedFn fn = &MacroAssembler::Umax;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, umax_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, umax_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, umax_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, umax_exp_d);

  unsigned umin_exp_b[] = {0x00, 0x00, 0x0e, 0xff, 0xf0, 0x98, 0x55, 0x67};

  unsigned umin_exp_h[] = {0x0000, 0x0000, 0x00ff, 0xfffe,
                           0xfe00, 0xabab, 0x5555, 0x4567};

  unsigned umin_exp_s[] = {0x00000000, 0xffffffff, 0x0000eeee, 0xfffffffe,
                           0xfffe0000, 0xfedcba98, 0x55555555, 0x01234567};

  uint64_t umin_exp_d[] = {0x0000000000000000, 0xffffffffffffffff,
                           0x5555555555555555, 0x0000000001234567};
  fn = &MacroAssembler::Umin;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, umin_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, umin_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, umin_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, umin_exp_d);

  unsigned uabd_exp_b[] = {0x01, 0xff, 0x01, 0xff, 0x0e, 0x13, 0x78, 0x67};

  unsigned uabd_exp_h[] = {0x0001, 0xffff, 0x00ff, 0x0001,
                           0x0100, 0x0eed, 0x5555, 0x1111};

  unsigned uabd_exp_s[] = {0x00000001, 0xffffffff, 0x00001111, 0x00000001,
                           0x00010000, 0xfedcba98, 0x78787878, 0x11111111};

  uint64_t uabd_exp_d[] = {0x0000000000000001, 0xffffffffffffffff,
                           0x7878787878787878, 0x0000000011111111};

  fn = &MacroAssembler::Uabd;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, uabd_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, uabd_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, uabd_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, uabd_exp_d);
  // clang-format on
}

TEST_SVE(sve_binary_arithmetic_predicated_smin_smax_sabd) {
  // clang-format off
  int zn_b[] = {0, -128, -128, -128, -128, 127, 127, 1};

  int zm_b[] = {-1, 0, -1, -127, 127, 126, -1, 0};

  int zn_h[] = {0, INT16_MIN, INT16_MIN, INT16_MIN,
                INT16_MIN, INT16_MAX, INT16_MAX, 1};

  int zm_h[] = {-1, 0, -1, INT16_MIN + 1,
                INT16_MAX, INT16_MAX - 1, -1, 0};

  int zn_s[] = {0, INT32_MIN, INT32_MIN, INT32_MIN,
                INT32_MIN, INT32_MAX, INT32_MAX, 1};

  int zm_s[] = {-1, 0, -1, -INT32_MAX,
                INT32_MAX, INT32_MAX - 1, -1, 0};

  int64_t zn_d[] = {0, INT64_MIN, INT64_MIN, INT64_MIN,
                    INT64_MIN, INT64_MAX, INT64_MAX, 1};

  int64_t zm_d[] = {-1, 0, -1, INT64_MIN + 1,
                    INT64_MAX, INT64_MAX - 1, -1, 0};

  int pg_b[] = {1, 1, 1, 0, 1, 1, 1, 0};
  int pg_h[] = {1, 1, 0, 1, 1, 1, 0, 1};
  int pg_s[] = {1, 0, 1, 1, 1, 0, 1, 1};
  int pg_d[] = {0, 1, 1, 1, 0, 1, 1, 1};

  int smax_exp_b[] = {0, 0, -1, -128, 127, 127, 127, 1};

  int smax_exp_h[] = {0, 0, INT16_MIN, INT16_MIN + 1,
                      INT16_MAX, INT16_MAX, INT16_MAX, 1};

  int smax_exp_s[] = {0, INT32_MIN, -1, INT32_MIN + 1,
                      INT32_MAX, INT32_MAX, INT32_MAX, 1};

  int64_t smax_exp_d[] = {0, 0, -1, INT64_MIN + 1,
                          INT64_MIN, INT64_MAX, INT64_MAX, 1};

  ArithPredicatedFn fn = &MacroAssembler::Smax;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, smax_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, smax_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, smax_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, smax_exp_d);

  int smin_exp_b[] = {-1, -128, -128, -128, -128, 126, -1, 1};

  int smin_exp_h[] = {-1, INT16_MIN, INT16_MIN, INT16_MIN,
                      INT16_MIN, INT16_MAX - 1, INT16_MAX, 0};

  int smin_exp_s[] = {-1, INT32_MIN, INT32_MIN, INT32_MIN,
                      INT32_MIN, INT32_MAX, -1, 0};

  int64_t smin_exp_d[] = {0, INT64_MIN, INT64_MIN, INT64_MIN,
                          INT64_MIN, INT64_MAX - 1, -1, 0};

  fn = &MacroAssembler::Smin;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, smin_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, smin_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, smin_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, smin_exp_d);

  unsigned sabd_exp_b[] = {1, 128, 127, 128, 255, 1, 128, 1};

  unsigned sabd_exp_h[] = {1, 0x8000, 0x8000, 1, 0xffff, 1, 0x7fff, 1};

  unsigned sabd_exp_s[] = {1, 0x80000000, 0x7fffffff, 1,
                           0xffffffff, 0x7fffffff, 0x80000000, 1};

  uint64_t sabd_exp_d[] = {0, 0x8000000000000000, 0x7fffffffffffffff, 1,
                           0x8000000000000000, 1, 0x8000000000000000, 1};

  fn = &MacroAssembler::Sabd;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, sabd_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, sabd_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, sabd_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, sabd_exp_d);
  // clang-format on
}

TEST_SVE(sve_binary_arithmetic_predicated_mul_umulh) {
  // clang-format off
  unsigned zn_b[] = {0x00, 0x01, 0x20, 0x08, 0x80, 0xff, 0x55, 0xaa};

  unsigned zm_b[] = {0x7f, 0xcd, 0x80, 0xff, 0x55, 0xaa, 0x00, 0x08};

  unsigned zn_h[] = {0x0000, 0x0001, 0x0020, 0x0800,
                     0x8000, 0xff00, 0x5555, 0xaaaa};

  unsigned zm_h[] = {0x007f, 0x00cd, 0x0800, 0xffff,
                     0x5555, 0xaaaa, 0x0001, 0x1234};

  unsigned zn_s[] = {0x00000000, 0x00000001, 0x00200020, 0x08000800,
                     0x12345678, 0xffffffff, 0x55555555, 0xaaaaaaaa};

  unsigned zm_s[] = {0x00000000, 0x00000001, 0x00200020, 0x08000800,
                     0x12345678, 0x22223333, 0x55556666, 0x77778888};

  uint64_t zn_d[] = {0x0000000000000000, 0x5555555555555555,
                     0xffffffffffffffff, 0xaaaaaaaaaaaaaaaa};

  uint64_t zm_d[] = {0x0000000000000000, 0x1111111133333333,
                     0xddddddddeeeeeeee, 0xaaaaaaaaaaaaaaaa};

  int pg_b[] = {0, 1, 1, 1, 0, 1, 1, 1};
  int pg_h[] = {1, 0, 1, 1, 1, 0, 1, 1};
  int pg_s[] = {1, 1, 0, 1, 1, 1, 0, 1};
  int pg_d[] = {1, 1, 0, 1};

  unsigned mul_exp_b[] = {0x00, 0xcd, 0x00, 0xf8, 0x80, 0x56, 0x00, 0x50};

  unsigned mul_exp_h[] = {0x0000, 0x0001, 0x0000, 0xf800,
                          0x8000, 0xff00, 0x5555, 0x9e88};

  unsigned mul_exp_s[] = {0x00000000, 0x00000001, 0x00200020, 0x00400000,
                          0x1df4d840, 0xddddcccd, 0x55555555, 0xb05afa50};

  uint64_t mul_exp_d[] = {0x0000000000000000, 0xa4fa4fa4eeeeeeef,
                          0xffffffffffffffff, 0x38e38e38e38e38e4};

  ArithPredicatedFn fn = &MacroAssembler::Mul;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, mul_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, mul_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, mul_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, mul_exp_d);

  unsigned umulh_exp_b[] = {0x00, 0x00, 0x10, 0x07, 0x80, 0xa9, 0x00, 0x05};

  unsigned umulh_exp_h[] = {0x0000, 0x0001, 0x0001, 0x07ff,
                            0x2aaa, 0xff00, 0x0000, 0x0c22};

  unsigned umulh_exp_s[] = {0x00000000, 0x00000000, 0x00200020, 0x00400080,
                            0x014b66dc, 0x22223332, 0x55555555, 0x4fa505af};

  uint64_t umulh_exp_d[] = {0x0000000000000000, 0x05b05b05bbbbbbbb,
                            0xffffffffffffffff, 0x71c71c71c71c71c6};

  fn = &MacroAssembler::Umulh;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, umulh_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, umulh_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, umulh_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, umulh_exp_d);
  // clang-format on
}

TEST_SVE(sve_binary_arithmetic_predicated_smulh) {
  // clang-format off
  int zn_b[] = {0, 1, -1, INT8_MIN, INT8_MAX, -1, 100, -3};

  int zm_b[] = {0, INT8_MIN, INT8_MIN, INT8_MAX, INT8_MAX, -1, 2, 66};

  int zn_h[] = {0, 1, -1, INT16_MIN, INT16_MAX, -1, 10000, -3};

  int zm_h[] = {0, INT16_MIN, INT16_MIN, INT16_MAX, INT16_MAX, -1, 2, 6666};

  int zn_s[] = {0, 1, -1, INT32_MIN, INT32_MAX, -1, 100000000, -3};

  int zm_s[] = {0, INT32_MIN, INT32_MIN, INT32_MAX, INT32_MAX, -1, 2, 66666666};

  int64_t zn_d[] = {0, -1, INT64_MIN, INT64_MAX};

  int64_t zm_d[] = {INT64_MIN, INT64_MAX, INT64_MIN, INT64_MAX};

  int pg_b[] = {0, 1, 1, 1, 0, 1, 1, 1};
  int pg_h[] = {1, 0, 1, 1, 1, 0, 1, 1};
  int pg_s[] = {1, 1, 0, 1, 1, 1, 0, 1};
  int pg_d[] = {1, 1, 0, 1};

  int exp_b[] = {0, -1, 0, -64, INT8_MAX, 0, 0, -1};

  int exp_h[] = {0, 1, 0, -16384, 16383, -1, 0, -1};

  int exp_s[] = {0, -1, -1, -1073741824, 1073741823, 0, 100000000, -1};

  int64_t exp_d[] = {0, -1, INT64_MIN, 4611686018427387903};

  ArithPredicatedFn fn = &MacroAssembler::Smulh;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, exp_d);
  // clang-format on
}

TEST_SVE(sve_binary_arithmetic_predicated_logical) {
  // clang-format off
  unsigned zn_b[] = {0x00, 0x01, 0x20, 0x08, 0x80, 0xff, 0x55, 0xaa};
  unsigned zm_b[] = {0x7f, 0xcd, 0x80, 0xff, 0x55, 0xaa, 0x00, 0x08};

  unsigned zn_h[] = {0x0000, 0x0001, 0x2020, 0x0008,
                     0x8000, 0xffff, 0x5555, 0xaaaa};
  unsigned zm_h[] = {0x7fff, 0xabcd, 0x8000, 0xffff,
                     0x5555, 0xaaaa, 0x0000, 0x0800};

  unsigned zn_s[] = {0x00000001, 0x20200008, 0x8000ffff, 0x5555aaaa};
  unsigned zm_s[] = {0x7fffabcd, 0x8000ffff, 0x5555aaaa, 0x00000800};

  uint64_t zn_d[] = {0xfedcba9876543210, 0x0123456789abcdef,
                     0x0001200880ff55aa, 0x0022446688aaccee};
  uint64_t zm_d[] = {0xffffeeeeddddcccc, 0xccccddddeeeeffff,
                     0x7fcd80ff55aa0008, 0x1133557799bbddff};

  int pg_b[] = {0, 1, 1, 1, 0, 1, 1, 1};
  int pg_h[] = {1, 0, 1, 1, 1, 0, 1, 1};
  int pg_s[] = {1, 1, 1, 0};
  int pg_d[] = {1, 1, 0, 1};

  unsigned and_exp_b[] = {0x00, 0x01, 0x00, 0x08, 0x80, 0xaa, 0x00, 0x08};

  unsigned and_exp_h[] = {0x0000, 0x0001, 0x0000, 0x0008,
                          0x0000, 0xffff, 0x0000, 0x0800};

  unsigned and_exp_s[] = {0x00000001, 0x00000008, 0x0000aaaa, 0x5555aaaa};

  uint64_t and_exp_d[] = {0xfedcaa8854540000, 0x0000454588aacdef,
                          0x0001200880ff55aa, 0x0022446688aaccee};

  ArithPredicatedFn fn = &MacroAssembler::And;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, and_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, and_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, and_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, and_exp_d);

  unsigned bic_exp_b[] = {0x00, 0x00, 0x20, 0x00, 0x80, 0x55, 0x55, 0xa2};

  unsigned bic_exp_h[] = {0x0000, 0x0001, 0x2020, 0x0000,
                          0x8000, 0xffff, 0x5555, 0xa2aa};

  unsigned bic_exp_s[] = {0x00000000, 0x20200000, 0x80005555, 0x5555aaaa};

  uint64_t bic_exp_d[] = {0x0000101022003210, 0x0123002201010000,
                          0x0001200880ff55aa, 0x0000000000000000};

  fn = &MacroAssembler::Bic;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, bic_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, bic_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, bic_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, bic_exp_d);

  unsigned eor_exp_b[] = {0x00, 0xcc, 0xa0, 0xf7, 0x80, 0x55, 0x55, 0xa2};

  unsigned eor_exp_h[] = {0x7fff, 0x0001, 0xa020, 0xfff7,
                          0xd555, 0xffff, 0x5555, 0xa2aa};

  unsigned eor_exp_s[] = {0x7fffabcc, 0xa020fff7, 0xd5555555, 0x5555aaaa};

  uint64_t eor_exp_d[] = {0x01235476ab89fedc, 0xcdef98ba67453210,
                          0x0001200880ff55aa, 0x1111111111111111};

  fn = &MacroAssembler::Eor;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, eor_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, eor_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, eor_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, eor_exp_d);

  unsigned orr_exp_b[] = {0x00, 0xcd, 0xa0, 0xff, 0x80, 0xff, 0x55, 0xaa};

  unsigned orr_exp_h[] = {0x7fff, 0x0001, 0xa020, 0xffff,
                          0xd555, 0xffff, 0x5555, 0xaaaa};

  unsigned orr_exp_s[] = {0x7fffabcd, 0xa020ffff, 0xd555ffff, 0x5555aaaa};

  uint64_t orr_exp_d[] = {0xfffffefeffddfedc, 0xcdefddffefefffff,
                          0x0001200880ff55aa, 0x1133557799bbddff};

  fn = &MacroAssembler::Orr;
  IntBinArithHelper(config, fn, kBRegSize, pg_b, zn_b, zm_b, orr_exp_b);
  IntBinArithHelper(config, fn, kHRegSize, pg_h, zn_h, zm_h, orr_exp_h);
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, orr_exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, orr_exp_d);
  // clang-format on
}

TEST_SVE(sve_binary_arithmetic_predicated_sdiv) {
  // clang-format off
  int zn_s[] = {0, 1, -1, 2468,
                INT32_MIN, INT32_MAX, INT32_MIN, INT32_MAX,
                -11111111, 87654321, 0, 0};

  int zm_s[] = {1, -1, 1, 1234,
                -1, INT32_MIN, 1, -1,
                22222222, 80000000, -1, 0};

  int64_t zn_d[] = {0, 1, -1, 2468,
                    INT64_MIN, INT64_MAX, INT64_MIN, INT64_MAX,
                    -11111111, 87654321, 0, 0};

  int64_t zm_d[] = {1, -1, 1, 1234,
                    -1, INT64_MIN, 1, -1,
                    22222222, 80000000, -1, 0};

  int pg_s[] = {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0};
  int pg_d[] = {0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1};

  int exp_s[] = {0, 1, -1, 2,
                 INT32_MIN, 0, INT32_MIN, -INT32_MAX,
                 0, 1, 0, 0};

  int64_t exp_d[] = {0, -1, -1, 2,
                     INT64_MIN, INT64_MAX, INT64_MIN, -INT64_MAX,
                     0, 1, 0, 0};

  ArithPredicatedFn fn = &MacroAssembler::Sdiv;
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, exp_d);
  // clang-format on
}

TEST_SVE(sve_binary_arithmetic_predicated_udiv) {
  // clang-format off
  unsigned zn_s[] = {0x00000000, 0x00000001, 0xffffffff, 0x80000000,
                     0xffffffff, 0x80000000, 0xffffffff, 0x0000f000};

  unsigned zm_s[] = {0x00000001, 0xffffffff, 0x80000000, 0x00000002,
                     0x00000000, 0x00000001, 0x00008000, 0xf0000000};

  uint64_t zn_d[] = {0x0000000000000000, 0x0000000000000001,
                     0xffffffffffffffff, 0x8000000000000000,
                     0xffffffffffffffff, 0x8000000000000000,
                     0xffffffffffffffff, 0xf0000000f0000000};

  uint64_t zm_d[] = {0x0000000000000001, 0xffffffff00000000,
                     0x8000000000000000, 0x0000000000000002,
                     0x8888888888888888, 0x0000000000000001,
                     0x0000000080000000, 0x00000000f0000000};

  int pg_s[] = {1, 1, 0, 1, 1, 0, 1, 1};
  int pg_d[] = {1, 0, 1, 1, 1, 1, 0, 1};

  unsigned exp_s[] = {0x00000000, 0x00000000, 0xffffffff, 0x40000000,
                      0x00000000, 0x80000000, 0x0001ffff, 0x00000000};

  uint64_t exp_d[] = {0x0000000000000000, 0x0000000000000001,
                      0x0000000000000001, 0x4000000000000000,
                      0x0000000000000001, 0x8000000000000000,
                      0xffffffffffffffff, 0x0000000100000001};

  ArithPredicatedFn fn = &MacroAssembler::Udiv;
  IntBinArithHelper(config, fn, kSRegSize, pg_s, zn_s, zm_s, exp_s);
  IntBinArithHelper(config, fn, kDRegSize, pg_d, zn_d, zm_d, exp_d);
  // clang-format on
}

typedef void (MacroAssembler::*ArithFn)(const ZRegister& zd,
                                        const ZRegister& zn,
                                        const ZRegister& zm);

template <typename T>
static void IntArithHelper(Test* config,
                           ArithFn macro,
                           unsigned lane_size_in_bits,
                           const T& zn_inputs,
                           const T& zm_inputs,
                           const T& zd_expected) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  ZRegister zn = z31.WithLaneSize(lane_size_in_bits);
  ZRegister zm = z27.WithLaneSize(lane_size_in_bits);
  InsrHelper(&masm, zn, zn_inputs);
  InsrHelper(&masm, zm, zm_inputs);

  ZRegister zd = z0.WithLaneSize(lane_size_in_bits);
  (masm.*macro)(zd, zn, zm);

  END();

  if (CAN_RUN()) {
    RUN();
    ASSERT_EQUAL_SVE(zd_expected, zd);
  }
}

TEST_SVE(sve_arithmetic_unpredicated_add_sqadd_uqadd) {
  // clang-format off
  unsigned in_b[] = {0x81, 0x7f, 0x10, 0xaa, 0x55, 0xff, 0xf0};
  unsigned in_h[] = {0x8181, 0x7f7f, 0x1010, 0xaaaa, 0x5555, 0xffff, 0xf0f0};
  unsigned in_s[] = {0x80018181, 0x7fff7f7f, 0x10001010, 0xaaaaaaaa, 0xf000f0f0};
  uint64_t in_d[] = {0x8000000180018181, 0x7fffffff7fff7f7f,
                      0x1000000010001010, 0xf0000000f000f0f0};

  ArithFn fn = &MacroAssembler::Add;

  unsigned add_exp_b[] = {0x02, 0xfe, 0x20, 0x54, 0xaa, 0xfe, 0xe0};
  unsigned add_exp_h[] = {0x0302, 0xfefe, 0x2020, 0x5554, 0xaaaa, 0xfffe, 0xe1e0};
  unsigned add_exp_s[] = {0x00030302, 0xfffefefe, 0x20002020, 0x55555554, 0xe001e1e0};
  uint64_t add_exp_d[] = {0x0000000300030302, 0xfffffffefffefefe,
                          0x2000000020002020, 0xe0000001e001e1e0};

  IntArithHelper(config, fn, kBRegSize, in_b, in_b, add_exp_b);
  IntArithHelper(config, fn, kHRegSize, in_h, in_h, add_exp_h);
  IntArithHelper(config, fn, kSRegSize, in_s, in_s, add_exp_s);
  IntArithHelper(config, fn, kDRegSize, in_d, in_d, add_exp_d);

  fn = &MacroAssembler::Sqadd;

  unsigned sqadd_exp_b[] = {0x80, 0x7f, 0x20, 0x80, 0x7f, 0xfe, 0xe0};
  unsigned sqadd_exp_h[] = {0x8000, 0x7fff, 0x2020, 0x8000, 0x7fff, 0xfffe, 0xe1e0};
  unsigned sqadd_exp_s[] = {0x80000000, 0x7fffffff, 0x20002020, 0x80000000, 0xe001e1e0};
  uint64_t sqadd_exp_d[] = {0x8000000000000000, 0x7fffffffffffffff,
                            0x2000000020002020, 0xe0000001e001e1e0};

  IntArithHelper(config, fn, kBRegSize, in_b, in_b, sqadd_exp_b);
  IntArithHelper(config, fn, kHRegSize, in_h, in_h, sqadd_exp_h);
  IntArithHelper(config, fn, kSRegSize, in_s, in_s, sqadd_exp_s);
  IntArithHelper(config, fn, kDRegSize, in_d, in_d, sqadd_exp_d);

  fn = &MacroAssembler::Uqadd;

  unsigned uqadd_exp_b[] = {0xff, 0xfe, 0x20, 0xff, 0xaa, 0xff, 0xff};
  unsigned uqadd_exp_h[] = {0xffff, 0xfefe, 0x2020, 0xffff, 0xaaaa, 0xffff, 0xffff};
  unsigned uqadd_exp_s[] = {0xffffffff, 0xfffefefe, 0x20002020, 0xffffffff, 0xffffffff};
  uint64_t uqadd_exp_d[] = {0xffffffffffffffff, 0xfffffffefffefefe,
                            0x2000000020002020, 0xffffffffffffffff};

  IntArithHelper(config, fn, kBRegSize, in_b, in_b, uqadd_exp_b);
  IntArithHelper(config, fn, kHRegSize, in_h, in_h, uqadd_exp_h);
  IntArithHelper(config, fn, kSRegSize, in_s, in_s, uqadd_exp_s);
  IntArithHelper(config, fn, kDRegSize, in_d, in_d, uqadd_exp_d);
  // clang-format on
}

TEST_SVE(sve_arithmetic_unpredicated_sub_sqsub_uqsub) {
  // clang-format off

  unsigned ins1_b[] = {0x81, 0x7f, 0x7e, 0xaa};
  unsigned ins2_b[] = {0x10, 0xf0, 0xf0, 0x55};

  unsigned ins1_h[] = {0x8181, 0x7f7f, 0x7e7e, 0xaaaa};
  unsigned ins2_h[] = {0x1010, 0xf0f0, 0xf0f0, 0x5555};

  unsigned ins1_s[] = {0x80018181, 0x7fff7f7f, 0x7eee7e7e, 0xaaaaaaaa};
  unsigned ins2_s[] = {0x10001010, 0xf000f0f0, 0xf000f0f0, 0x55555555};

  uint64_t ins1_d[] = {0x8000000180018181, 0x7fffffff7fff7f7f,
                       0x7eeeeeee7eee7e7e, 0xaaaaaaaaaaaaaaaa};
  uint64_t ins2_d[] = {0x1000000010001010, 0xf0000000f000f0f0,
                       0xf0000000f000f0f0, 0x5555555555555555};

  ArithFn fn = &MacroAssembler::Sub;

  unsigned ins1_sub_ins2_exp_b[] = {0x71, 0x8f, 0x8e, 0x55};
  unsigned ins1_sub_ins2_exp_h[] = {0x7171, 0x8e8f, 0x8d8e, 0x5555};
  unsigned ins1_sub_ins2_exp_s[] = {0x70017171, 0x8ffe8e8f, 0x8eed8d8e, 0x55555555};
  uint64_t ins1_sub_ins2_exp_d[] = {0x7000000170017171, 0x8ffffffe8ffe8e8f,
                                    0x8eeeeeed8eed8d8e, 0x5555555555555555};

  IntArithHelper(config, fn, kBRegSize, ins1_b, ins2_b, ins1_sub_ins2_exp_b);
  IntArithHelper(config, fn, kHRegSize, ins1_h, ins2_h, ins1_sub_ins2_exp_h);
  IntArithHelper(config, fn, kSRegSize, ins1_s, ins2_s, ins1_sub_ins2_exp_s);
  IntArithHelper(config, fn, kDRegSize, ins1_d, ins2_d, ins1_sub_ins2_exp_d);

  unsigned ins2_sub_ins1_exp_b[] = {0x8f, 0x71, 0x72, 0xab};
  unsigned ins2_sub_ins1_exp_h[] = {0x8e8f, 0x7171, 0x7272, 0xaaab};
  unsigned ins2_sub_ins1_exp_s[] = {0x8ffe8e8f, 0x70017171, 0x71127272, 0xaaaaaaab};
  uint64_t ins2_sub_ins1_exp_d[] = {0x8ffffffe8ffe8e8f, 0x7000000170017171,
                                    0x7111111271127272, 0xaaaaaaaaaaaaaaab};

  IntArithHelper(config, fn, kBRegSize, ins2_b, ins1_b, ins2_sub_ins1_exp_b);
  IntArithHelper(config, fn, kHRegSize, ins2_h, ins1_h, ins2_sub_ins1_exp_h);
  IntArithHelper(config, fn, kSRegSize, ins2_s, ins1_s, ins2_sub_ins1_exp_s);
  IntArithHelper(config, fn, kDRegSize, ins2_d, ins1_d, ins2_sub_ins1_exp_d);

  fn = &MacroAssembler::Sqsub;

  unsigned ins1_sqsub_ins2_exp_b[] = {0x80, 0x7f, 0x7f, 0x80};
  unsigned ins1_sqsub_ins2_exp_h[] = {0x8000, 0x7fff, 0x7fff, 0x8000};
  unsigned ins1_sqsub_ins2_exp_s[] = {0x80000000, 0x7fffffff, 0x7fffffff, 0x80000000};
  uint64_t ins1_sqsub_ins2_exp_d[] = {0x8000000000000000, 0x7fffffffffffffff,
                                      0x7fffffffffffffff, 0x8000000000000000};

  IntArithHelper(config, fn, kBRegSize, ins1_b, ins2_b, ins1_sqsub_ins2_exp_b);
  IntArithHelper(config, fn, kHRegSize, ins1_h, ins2_h, ins1_sqsub_ins2_exp_h);
  IntArithHelper(config, fn, kSRegSize, ins1_s, ins2_s, ins1_sqsub_ins2_exp_s);
  IntArithHelper(config, fn, kDRegSize, ins1_d, ins2_d, ins1_sqsub_ins2_exp_d);

  unsigned ins2_sqsub_ins1_exp_b[] = {0x7f, 0x80, 0x80, 0x7f};
  unsigned ins2_sqsub_ins1_exp_h[] = {0x7fff, 0x8000, 0x8000, 0x7fff};
  unsigned ins2_sqsub_ins1_exp_s[] = {0x7fffffff, 0x80000000, 0x80000000, 0x7fffffff};
  uint64_t ins2_sqsub_ins1_exp_d[] = {0x7fffffffffffffff, 0x8000000000000000,
                                      0x8000000000000000, 0x7fffffffffffffff};

  IntArithHelper(config, fn, kBRegSize, ins2_b, ins1_b, ins2_sqsub_ins1_exp_b);
  IntArithHelper(config, fn, kHRegSize, ins2_h, ins1_h, ins2_sqsub_ins1_exp_h);
  IntArithHelper(config, fn, kSRegSize, ins2_s, ins1_s, ins2_sqsub_ins1_exp_s);
  IntArithHelper(config, fn, kDRegSize, ins2_d, ins1_d, ins2_sqsub_ins1_exp_d);

  fn = &MacroAssembler::Uqsub;

  unsigned ins1_uqsub_ins2_exp_b[] = {0x71, 0x00, 0x00, 0x55};
  unsigned ins1_uqsub_ins2_exp_h[] = {0x7171, 0x0000, 0x0000, 0x5555};
  unsigned ins1_uqsub_ins2_exp_s[] = {0x70017171, 0x00000000, 0x00000000, 0x55555555};
  uint64_t ins1_uqsub_ins2_exp_d[] = {0x7000000170017171, 0x0000000000000000,
                                      0x0000000000000000, 0x5555555555555555};

  IntArithHelper(config, fn, kBRegSize, ins1_b, ins2_b, ins1_uqsub_ins2_exp_b);
  IntArithHelper(config, fn, kHRegSize, ins1_h, ins2_h, ins1_uqsub_ins2_exp_h);
  IntArithHelper(config, fn, kSRegSize, ins1_s, ins2_s, ins1_uqsub_ins2_exp_s);
  IntArithHelper(config, fn, kDRegSize, ins1_d, ins2_d, ins1_uqsub_ins2_exp_d);

  unsigned ins2_uqsub_ins1_exp_b[] = {0x00, 0x71, 0x72, 0x00};
  unsigned ins2_uqsub_ins1_exp_h[] = {0x0000, 0x7171, 0x7272, 0x0000};
  unsigned ins2_uqsub_ins1_exp_s[] = {0x00000000, 0x70017171, 0x71127272, 0x00000000};
  uint64_t ins2_uqsub_ins1_exp_d[] = {0x0000000000000000, 0x7000000170017171,
                                      0x7111111271127272, 0x0000000000000000};

  IntArithHelper(config, fn, kBRegSize, ins2_b, ins1_b, ins2_uqsub_ins1_exp_b);
  IntArithHelper(config, fn, kHRegSize, ins2_h, ins1_h, ins2_uqsub_ins1_exp_h);
  IntArithHelper(config, fn, kSRegSize, ins2_s, ins1_s, ins2_uqsub_ins1_exp_s);
  IntArithHelper(config, fn, kDRegSize, ins2_d, ins1_d, ins2_uqsub_ins1_exp_d);
  // clang-format on
}

TEST_SVE(sve_rdvl) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Encodable multipliers.
  __ Rdvl(x0, 0);
  __ Rdvl(x1, 1);
  __ Rdvl(x2, 2);
  __ Rdvl(x3, 31);
  __ Rdvl(x4, -1);
  __ Rdvl(x5, -2);
  __ Rdvl(x6, -32);

  // For unencodable multipliers, the MacroAssembler uses a sequence of
  // instructions.
  __ Rdvl(x10, 32);
  __ Rdvl(x11, -33);
  __ Rdvl(x12, 42);
  __ Rdvl(x13, -42);

  // The maximum value of VL is 256 (bytes), so the multiplier is limited to the
  // range [INT64_MIN/256, INT64_MAX/256], to ensure that no signed overflow
  // occurs in the macro.
  __ Rdvl(x14, 0x007fffffffffffff);
  __ Rdvl(x15, -0x0080000000000000);

  END();

  if (CAN_RUN()) {
    RUN();

    uint64_t vl = config->sve_vl_in_bytes();

    ASSERT_EQUAL_64(vl * 0, x0);
    ASSERT_EQUAL_64(vl * 1, x1);
    ASSERT_EQUAL_64(vl * 2, x2);
    ASSERT_EQUAL_64(vl * 31, x3);
    ASSERT_EQUAL_64(vl * -1, x4);
    ASSERT_EQUAL_64(vl * -2, x5);
    ASSERT_EQUAL_64(vl * -32, x6);

    ASSERT_EQUAL_64(vl * 32, x10);
    ASSERT_EQUAL_64(vl * -33, x11);
    ASSERT_EQUAL_64(vl * 42, x12);
    ASSERT_EQUAL_64(vl * -42, x13);

    ASSERT_EQUAL_64(vl * 0x007fffffffffffff, x14);
    ASSERT_EQUAL_64(vl * 0xff80000000000000, x15);
  }
}

TEST_SVE(sve_rdpl) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // There is no `rdpl` instruction, so the MacroAssembler maps `Rdpl` onto
  // Addpl(xd, xzr, ...).

  // Encodable multipliers (as `addvl`).
  __ Rdpl(x0, 0);
  __ Rdpl(x1, 8);
  __ Rdpl(x2, 248);
  __ Rdpl(x3, -8);
  __ Rdpl(x4, -256);

  // Encodable multipliers (as `movz` + `addpl`).
  __ Rdpl(x7, 31);
  __ Rdpl(x8, -31);

  // For unencodable multipliers, the MacroAssembler uses a sequence of
  // instructions.
  __ Rdpl(x10, 42);
  __ Rdpl(x11, -42);

  // The maximum value of VL is 256 (bytes), so the multiplier is limited to the
  // range [INT64_MIN/256, INT64_MAX/256], to ensure that no signed overflow
  // occurs in the macro.
  __ Rdpl(x12, 0x007fffffffffffff);
  __ Rdpl(x13, -0x0080000000000000);

  END();

  if (CAN_RUN()) {
    RUN();

    uint64_t vl = config->sve_vl_in_bytes();
    VIXL_ASSERT((vl % kZRegBitsPerPRegBit) == 0);
    uint64_t pl = vl / kZRegBitsPerPRegBit;

    ASSERT_EQUAL_64(pl * 0, x0);
    ASSERT_EQUAL_64(pl * 8, x1);
    ASSERT_EQUAL_64(pl * 248, x2);
    ASSERT_EQUAL_64(pl * -8, x3);
    ASSERT_EQUAL_64(pl * -256, x4);

    ASSERT_EQUAL_64(pl * 31, x7);
    ASSERT_EQUAL_64(pl * -31, x8);

    ASSERT_EQUAL_64(pl * 42, x10);
    ASSERT_EQUAL_64(pl * -42, x11);

    ASSERT_EQUAL_64(pl * 0x007fffffffffffff, x12);
    ASSERT_EQUAL_64(pl * 0xff80000000000000, x13);
  }
}

TEST_SVE(sve_addvl) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t base = 0x1234567800000000;
  __ Mov(x30, base);

  // Encodable multipliers.
  __ Addvl(x0, x30, 0);
  __ Addvl(x1, x30, 1);
  __ Addvl(x2, x30, 31);
  __ Addvl(x3, x30, -1);
  __ Addvl(x4, x30, -32);

  // For unencodable multipliers, the MacroAssembler uses `Rdvl` and `Add`.
  __ Addvl(x5, x30, 32);
  __ Addvl(x6, x30, -33);

  // Test the limits of the multiplier supported by the `Rdvl` macro.
  __ Addvl(x7, x30, 0x007fffffffffffff);
  __ Addvl(x8, x30, -0x0080000000000000);

  // Check that xzr behaves correctly.
  __ Addvl(x9, xzr, 8);
  __ Addvl(x10, xzr, 42);

  // Check that sp behaves correctly with encodable and unencodable multipliers.
  __ Addvl(sp, sp, -5);
  __ Addvl(sp, sp, -37);
  __ Addvl(x11, sp, -2);
  __ Addvl(sp, x11, 2);
  __ Addvl(x12, sp, -42);

  // Restore the value of sp.
  __ Addvl(sp, x11, 39);
  __ Addvl(sp, sp, 5);

  // Adjust x11 and x12 to make the test sp-agnostic.
  __ Sub(x11, sp, x11);
  __ Sub(x12, sp, x12);

  // Check cases where xd.Is(xn). This stresses scratch register allocation.
  __ Mov(x20, x30);
  __ Mov(x21, x30);
  __ Mov(x22, x30);
  __ Addvl(x20, x20, 4);
  __ Addvl(x21, x21, 42);
  __ Addvl(x22, x22, -0x0080000000000000);

  END();

  if (CAN_RUN()) {
    RUN();

    uint64_t vl = config->sve_vl_in_bytes();

    ASSERT_EQUAL_64(base + (vl * 0), x0);
    ASSERT_EQUAL_64(base + (vl * 1), x1);
    ASSERT_EQUAL_64(base + (vl * 31), x2);
    ASSERT_EQUAL_64(base + (vl * -1), x3);
    ASSERT_EQUAL_64(base + (vl * -32), x4);

    ASSERT_EQUAL_64(base + (vl * 32), x5);
    ASSERT_EQUAL_64(base + (vl * -33), x6);

    ASSERT_EQUAL_64(base + (vl * 0x007fffffffffffff), x7);
    ASSERT_EQUAL_64(base + (vl * 0xff80000000000000), x8);

    ASSERT_EQUAL_64(vl * 8, x9);
    ASSERT_EQUAL_64(vl * 42, x10);

    ASSERT_EQUAL_64(vl * 44, x11);
    ASSERT_EQUAL_64(vl * 84, x12);

    ASSERT_EQUAL_64(base + (vl * 4), x20);
    ASSERT_EQUAL_64(base + (vl * 42), x21);
    ASSERT_EQUAL_64(base + (vl * 0xff80000000000000), x22);

    ASSERT_EQUAL_64(base, x30);
  }
}

TEST_SVE(sve_addpl) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t base = 0x1234567800000000;
  __ Mov(x30, base);

  // Encodable multipliers.
  __ Addpl(x0, x30, 0);
  __ Addpl(x1, x30, 1);
  __ Addpl(x2, x30, 31);
  __ Addpl(x3, x30, -1);
  __ Addpl(x4, x30, -32);

  // For unencodable multipliers, the MacroAssembler uses `Addvl` if it can, or
  // it falls back to `Rdvl` and `Add`.
  __ Addpl(x5, x30, 32);
  __ Addpl(x6, x30, -33);

  // Test the limits of the multiplier supported by the `Rdvl` macro.
  __ Addpl(x7, x30, 0x007fffffffffffff);
  __ Addpl(x8, x30, -0x0080000000000000);

  // Check that xzr behaves correctly.
  __ Addpl(x9, xzr, 8);
  __ Addpl(x10, xzr, 42);

  // Check that sp behaves correctly with encodable and unencodable multipliers.
  __ Addpl(sp, sp, -5);
  __ Addpl(sp, sp, -37);
  __ Addpl(x11, sp, -2);
  __ Addpl(sp, x11, 2);
  __ Addpl(x12, sp, -42);

  // Restore the value of sp.
  __ Addpl(sp, x11, 39);
  __ Addpl(sp, sp, 5);

  // Adjust x11 and x12 to make the test sp-agnostic.
  __ Sub(x11, sp, x11);
  __ Sub(x12, sp, x12);

  // Check cases where xd.Is(xn). This stresses scratch register allocation.
  __ Mov(x20, x30);
  __ Mov(x21, x30);
  __ Mov(x22, x30);
  __ Addpl(x20, x20, 4);
  __ Addpl(x21, x21, 42);
  __ Addpl(x22, x22, -0x0080000000000000);

  END();

  if (CAN_RUN()) {
    RUN();

    uint64_t vl = config->sve_vl_in_bytes();
    VIXL_ASSERT((vl % kZRegBitsPerPRegBit) == 0);
    uint64_t pl = vl / kZRegBitsPerPRegBit;

    ASSERT_EQUAL_64(base + (pl * 0), x0);
    ASSERT_EQUAL_64(base + (pl * 1), x1);
    ASSERT_EQUAL_64(base + (pl * 31), x2);
    ASSERT_EQUAL_64(base + (pl * -1), x3);
    ASSERT_EQUAL_64(base + (pl * -32), x4);

    ASSERT_EQUAL_64(base + (pl * 32), x5);
    ASSERT_EQUAL_64(base + (pl * -33), x6);

    ASSERT_EQUAL_64(base + (pl * 0x007fffffffffffff), x7);
    ASSERT_EQUAL_64(base + (pl * 0xff80000000000000), x8);

    ASSERT_EQUAL_64(pl * 8, x9);
    ASSERT_EQUAL_64(pl * 42, x10);

    ASSERT_EQUAL_64(pl * 44, x11);
    ASSERT_EQUAL_64(pl * 84, x12);

    ASSERT_EQUAL_64(base + (pl * 4), x20);
    ASSERT_EQUAL_64(base + (pl * 42), x21);
    ASSERT_EQUAL_64(base + (pl * 0xff80000000000000), x22);

    ASSERT_EQUAL_64(base, x30);
  }
}

TEST_SVE(sve_calculate_sve_address) {
  // Shadow the `MacroAssembler` type so that the test macros work without
  // modification.
  typedef CalculateSVEAddressMacroAssembler MacroAssembler;

  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();  // NOLINT(clang-diagnostic-local-type-template-args)

  uint64_t base = 0x1234567800000000;
  __ Mov(x28, base);
  __ Mov(x29, 48);
  __ Mov(x30, -48);

  // Simple scalar (or equivalent) cases.

  __ CalculateSVEAddress(x0, SVEMemOperand(x28));
  __ CalculateSVEAddress(x1, SVEMemOperand(x28, 0));
  __ CalculateSVEAddress(x2, SVEMemOperand(x28, 0, SVE_MUL_VL));
  __ CalculateSVEAddress(x3, SVEMemOperand(x28, 0, SVE_MUL_VL), 3);
  __ CalculateSVEAddress(x4, SVEMemOperand(x28, xzr));
  __ CalculateSVEAddress(x5, SVEMemOperand(x28, xzr, LSL, 42));

  // scalar-plus-immediate

  // Unscaled immediates, handled with `Add`.
  __ CalculateSVEAddress(x6, SVEMemOperand(x28, 42));
  __ CalculateSVEAddress(x7, SVEMemOperand(x28, -42));
  // Scaled immediates, handled with `Addvl` or `Addpl`.
  __ CalculateSVEAddress(x8, SVEMemOperand(x28, 31, SVE_MUL_VL), 0);
  __ CalculateSVEAddress(x9, SVEMemOperand(x28, -32, SVE_MUL_VL), 0);
  // Out of `addvl` or `addpl` range.
  __ CalculateSVEAddress(x10, SVEMemOperand(x28, 42, SVE_MUL_VL), 0);
  __ CalculateSVEAddress(x11, SVEMemOperand(x28, -42, SVE_MUL_VL), 0);
  // As above, for VL-based accesses smaller than a Z register.
  VIXL_STATIC_ASSERT(kZRegBitsPerPRegBitLog2 == 3);
  __ CalculateSVEAddress(x12, SVEMemOperand(x28, -32 * 8, SVE_MUL_VL), 3);
  __ CalculateSVEAddress(x13, SVEMemOperand(x28, -42 * 8, SVE_MUL_VL), 3);
  __ CalculateSVEAddress(x14, SVEMemOperand(x28, -32 * 4, SVE_MUL_VL), 2);
  __ CalculateSVEAddress(x15, SVEMemOperand(x28, -42 * 4, SVE_MUL_VL), 2);
  __ CalculateSVEAddress(x18, SVEMemOperand(x28, -32 * 2, SVE_MUL_VL), 1);
  __ CalculateSVEAddress(x19, SVEMemOperand(x28, -42 * 2, SVE_MUL_VL), 1);

  // scalar-plus-scalar

  __ CalculateSVEAddress(x20, SVEMemOperand(x28, x29));
  __ CalculateSVEAddress(x21, SVEMemOperand(x28, x30));
  __ CalculateSVEAddress(x22, SVEMemOperand(x28, x29, LSL, 8));
  __ CalculateSVEAddress(x23, SVEMemOperand(x28, x30, LSL, 8));

  // In-place updates, to stress scratch register allocation.

  __ Mov(x24, 0xabcd000000000000);
  __ Mov(x25, 0xabcd101100000000);
  __ Mov(x26, 0xabcd202200000000);
  __ Mov(x27, 0xabcd303300000000);
  __ Mov(x28, 0xabcd404400000000);
  __ Mov(x29, 0xabcd505500000000);

  __ CalculateSVEAddress(x24, SVEMemOperand(x24));
  __ CalculateSVEAddress(x25, SVEMemOperand(x25, 0x42));
  __ CalculateSVEAddress(x26, SVEMemOperand(x26, 3, SVE_MUL_VL), 0);
  __ CalculateSVEAddress(x27, SVEMemOperand(x27, 0x42, SVE_MUL_VL), 3);
  __ CalculateSVEAddress(x28, SVEMemOperand(x28, x30));
  __ CalculateSVEAddress(x29, SVEMemOperand(x29, x30, LSL, 4));

  END();

  if (CAN_RUN()) {
    RUN();

    uint64_t vl = config->sve_vl_in_bytes();
    VIXL_ASSERT((vl % kZRegBitsPerPRegBit) == 0);
    uint64_t pl = vl / kZRegBitsPerPRegBit;

    // Simple scalar (or equivalent) cases.
    ASSERT_EQUAL_64(base, x0);
    ASSERT_EQUAL_64(base, x1);
    ASSERT_EQUAL_64(base, x2);
    ASSERT_EQUAL_64(base, x3);
    ASSERT_EQUAL_64(base, x4);
    ASSERT_EQUAL_64(base, x5);

    // scalar-plus-immediate
    ASSERT_EQUAL_64(base + 42, x6);
    ASSERT_EQUAL_64(base - 42, x7);
    ASSERT_EQUAL_64(base + (31 * vl), x8);
    ASSERT_EQUAL_64(base - (32 * vl), x9);
    ASSERT_EQUAL_64(base + (42 * vl), x10);
    ASSERT_EQUAL_64(base - (42 * vl), x11);
    ASSERT_EQUAL_64(base - (32 * vl), x12);
    ASSERT_EQUAL_64(base - (42 * vl), x13);
    ASSERT_EQUAL_64(base - (32 * vl), x14);
    ASSERT_EQUAL_64(base - (42 * vl), x15);
    ASSERT_EQUAL_64(base - (32 * vl), x18);
    ASSERT_EQUAL_64(base - (42 * vl), x19);

    // scalar-plus-scalar
    ASSERT_EQUAL_64(base + 48, x20);
    ASSERT_EQUAL_64(base - 48, x21);
    ASSERT_EQUAL_64(base + (48 << 8), x22);
    ASSERT_EQUAL_64(base - (48 << 8), x23);

    // In-place updates.
    ASSERT_EQUAL_64(0xabcd000000000000, x24);
    ASSERT_EQUAL_64(0xabcd101100000000 + 0x42, x25);
    ASSERT_EQUAL_64(0xabcd202200000000 + (3 * vl), x26);
    ASSERT_EQUAL_64(0xabcd303300000000 + (0x42 * pl), x27);
    ASSERT_EQUAL_64(0xabcd404400000000 - 48, x28);
    ASSERT_EQUAL_64(0xabcd505500000000 - (48 << 4), x29);
  }
}

TEST_SVE(sve_permute_vector_unpredicated) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE, CPUFeatures::kNEON);
  START();

  // Initialise registers with known values first.
  __ Dup(z1.VnB(), 0x11);
  __ Dup(z2.VnB(), 0x22);
  __ Dup(z3.VnB(), 0x33);
  __ Dup(z4.VnB(), 0x44);

  __ Mov(x0, 0x0123456789abcdef);
  __ Fmov(d0, RawbitsToDouble(0x7ffaaaaa22223456));
  __ Insr(z1.VnS(), w0);
  __ Insr(z2.VnD(), x0);
  __ Insr(z3.VnH(), h0);
  __ Insr(z4.VnD(), d0);

  uint64_t inputs[] = {0xfedcba9876543210,
                       0x0123456789abcdef,
                       0x8f8e8d8c8b8a8988,
                       0x8786858483828180};

  // Initialize a distinguishable value throughout the register first.
  __ Dup(z9.VnB(), 0xff);
  InsrHelper(&masm, z9.VnD(), inputs);

  __ Rev(z5.VnB(), z9.VnB());
  __ Rev(z6.VnH(), z9.VnH());
  __ Rev(z7.VnS(), z9.VnS());
  __ Rev(z8.VnD(), z9.VnD());

  int index[7] = {22, 7, 7, 3, 1, 1, 63};
  // Broadcasting an data within the input array.
  __ Dup(z10.VnB(), z9.VnB(), index[0]);
  __ Dup(z11.VnH(), z9.VnH(), index[1]);
  __ Dup(z12.VnS(), z9.VnS(), index[2]);
  __ Dup(z13.VnD(), z9.VnD(), index[3]);
  __ Dup(z14.VnQ(), z9.VnQ(), index[4]);
  // Test dst == src
  __ Mov(z15, z9);
  __ Dup(z15.VnS(), z15.VnS(), index[5]);
  // Selecting an data beyond the input array.
  __ Dup(z16.VnB(), z9.VnB(), index[6]);

  END();

  if (CAN_RUN()) {
    RUN();

    // Insr
    uint64_t z1_expected[] = {0x1111111111111111, 0x1111111189abcdef};
    uint64_t z2_expected[] = {0x2222222222222222, 0x0123456789abcdef};
    uint64_t z3_expected[] = {0x3333333333333333, 0x3333333333333456};
    uint64_t z4_expected[] = {0x4444444444444444, 0x7ffaaaaa22223456};
    ASSERT_EQUAL_SVE(z1_expected, z1.VnD());
    ASSERT_EQUAL_SVE(z2_expected, z2.VnD());
    ASSERT_EQUAL_SVE(z3_expected, z3.VnD());
    ASSERT_EQUAL_SVE(z4_expected, z4.VnD());

    // Rev
    int lane_count = core.GetSVELaneCount(kBRegSize);
    for (int i = 0; i < lane_count; i++) {
      uint64_t expected =
          core.zreg_lane(z5.GetCode(), kBRegSize, lane_count - i - 1);
      uint64_t input = core.zreg_lane(z9.GetCode(), kBRegSize, i);
      ASSERT_EQUAL_64(expected, input);
    }

    lane_count = core.GetSVELaneCount(kHRegSize);
    for (int i = 0; i < lane_count; i++) {
      uint64_t expected =
          core.zreg_lane(z6.GetCode(), kHRegSize, lane_count - i - 1);
      uint64_t input = core.zreg_lane(z9.GetCode(), kHRegSize, i);
      ASSERT_EQUAL_64(expected, input);
    }

    lane_count = core.GetSVELaneCount(kSRegSize);
    for (int i = 0; i < lane_count; i++) {
      uint64_t expected =
          core.zreg_lane(z7.GetCode(), kSRegSize, lane_count - i - 1);
      uint64_t input = core.zreg_lane(z9.GetCode(), kSRegSize, i);
      ASSERT_EQUAL_64(expected, input);
    }

    lane_count = core.GetSVELaneCount(kDRegSize);
    for (int i = 0; i < lane_count; i++) {
      uint64_t expected =
          core.zreg_lane(z8.GetCode(), kDRegSize, lane_count - i - 1);
      uint64_t input = core.zreg_lane(z9.GetCode(), kDRegSize, i);
      ASSERT_EQUAL_64(expected, input);
    }

    // Dup
    unsigned vl = config->sve_vl_in_bits();
    lane_count = core.GetSVELaneCount(kBRegSize);
    uint64_t expected_z10 = (vl > (index[0] * kBRegSize)) ? 0x23 : 0;
    for (int i = 0; i < lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(expected_z10, z10.VnB(), i);
    }

    lane_count = core.GetSVELaneCount(kHRegSize);
    uint64_t expected_z11 = (vl > (index[1] * kHRegSize)) ? 0x8f8e : 0;
    for (int i = 0; i < lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(expected_z11, z11.VnH(), i);
    }

    lane_count = core.GetSVELaneCount(kSRegSize);
    uint64_t expected_z12 = (vl > (index[2] * kSRegSize)) ? 0xfedcba98 : 0;
    for (int i = 0; i < lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(expected_z12, z12.VnS(), i);
    }

    lane_count = core.GetSVELaneCount(kDRegSize);
    uint64_t expected_z13 =
        (vl > (index[3] * kDRegSize)) ? 0xfedcba9876543210 : 0;
    for (int i = 0; i < lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(expected_z13, z13.VnD(), i);
    }

    lane_count = core.GetSVELaneCount(kDRegSize);
    uint64_t expected_z14_lo = 0;
    uint64_t expected_z14_hi = 0;
    if (vl > (index[4] * kQRegSize)) {
      expected_z14_lo = 0x0123456789abcdef;
      expected_z14_hi = 0xfedcba9876543210;
    }
    for (int i = 0; i < lane_count; i += 2) {
      ASSERT_EQUAL_SVE_LANE(expected_z14_lo, z14.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(expected_z14_hi, z14.VnD(), i + 1);
    }

    lane_count = core.GetSVELaneCount(kSRegSize);
    uint64_t expected_z15 = (vl > (index[5] * kSRegSize)) ? 0x87868584 : 0;
    for (int i = 0; i < lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(expected_z15, z15.VnS(), i);
    }

    lane_count = core.GetSVELaneCount(kBRegSize);
    uint64_t expected_z16 = (vl > (index[6] * kBRegSize)) ? 0xff : 0;
    for (int i = 0; i < lane_count; i++) {
      ASSERT_EQUAL_SVE_LANE(expected_z16, z16.VnB(), i);
    }
  }
}

TEST_SVE(sve_permute_vector_unpredicated_unpack_vector_elements) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t z9_inputs[] = {0xfedcba9876543210,
                          0x0123456789abcdef,
                          0x8f8e8d8c8b8a8988,
                          0x8786858483828180};
  InsrHelper(&masm, z9.VnD(), z9_inputs);

  __ Sunpkhi(z10.VnH(), z9.VnB());
  __ Sunpkhi(z11.VnS(), z9.VnH());
  __ Sunpkhi(z12.VnD(), z9.VnS());

  __ Sunpklo(z13.VnH(), z9.VnB());
  __ Sunpklo(z14.VnS(), z9.VnH());
  __ Sunpklo(z15.VnD(), z9.VnS());

  __ Uunpkhi(z16.VnH(), z9.VnB());
  __ Uunpkhi(z17.VnS(), z9.VnH());
  __ Uunpkhi(z18.VnD(), z9.VnS());

  __ Uunpklo(z19.VnH(), z9.VnB());
  __ Uunpklo(z20.VnS(), z9.VnH());
  __ Uunpklo(z21.VnD(), z9.VnS());

  // Test unpacking with same source and destination.
  __ Mov(z22, z9);
  __ Sunpklo(z22.VnH(), z22.VnB());
  __ Mov(z23, z9);
  __ Uunpklo(z23.VnH(), z23.VnB());

  END();

  if (CAN_RUN()) {
    RUN();

    // Suunpkhi
    int lane_count = core.GetSVELaneCount(kHRegSize);
    for (int i = lane_count - 1; i >= 0; i--) {
      uint16_t expected = core.zreg_lane<uint16_t>(z10.GetCode(), i);
      uint8_t b_lane = core.zreg_lane<uint8_t>(z9.GetCode(), i + lane_count);
      uint16_t input = SignExtend<int16_t>(b_lane, kBRegSize);
      ASSERT_EQUAL_64(expected, input);
    }

    lane_count = core.GetSVELaneCount(kSRegSize);
    for (int i = lane_count - 1; i >= 0; i--) {
      uint32_t expected = core.zreg_lane<uint32_t>(z11.GetCode(), i);
      uint16_t h_lane = core.zreg_lane<uint16_t>(z9.GetCode(), i + lane_count);
      uint32_t input = SignExtend<int32_t>(h_lane, kHRegSize);
      ASSERT_EQUAL_64(expected, input);
    }

    lane_count = core.GetSVELaneCount(kDRegSize);
    for (int i = lane_count - 1; i >= 0; i--) {
      uint64_t expected = core.zreg_lane<uint64_t>(z12.GetCode(), i);
      uint32_t s_lane = core.zreg_lane<uint32_t>(z9.GetCode(), i + lane_count);
      uint64_t input = SignExtend<int64_t>(s_lane, kSRegSize);
      ASSERT_EQUAL_64(expected, input);
    }

    // Suunpklo
    lane_count = core.GetSVELaneCount(kHRegSize);
    for (int i = lane_count - 1; i >= 0; i--) {
      uint16_t expected = core.zreg_lane<uint16_t>(z13.GetCode(), i);
      uint8_t b_lane = core.zreg_lane<uint8_t>(z9.GetCode(), i);
      uint16_t input = SignExtend<int16_t>(b_lane, kBRegSize);
      ASSERT_EQUAL_64(expected, input);
    }

    lane_count = core.GetSVELaneCount(kSRegSize);
    for (int i = lane_count - 1; i >= 0; i--) {
      uint32_t expected = core.zreg_lane<uint32_t>(z14.GetCode(), i);
      uint16_t h_lane = core.zreg_lane<uint16_t>(z9.GetCode(), i);
      uint32_t input = SignExtend<int32_t>(h_lane, kHRegSize);
      ASSERT_EQUAL_64(expected, input);
    }

    lane_count = core.GetSVELaneCount(kDRegSize);
    for (int i = lane_count - 1; i >= 0; i--) {
      uint64_t expected = core.zreg_lane<uint64_t>(z15.GetCode(), i);
      uint32_t s_lane = core.zreg_lane<uint32_t>(z9.GetCode(), i);
      uint64_t input = SignExtend<int64_t>(s_lane, kSRegSize);
      ASSERT_EQUAL_64(expected, input);
    }

    // Uuunpkhi
    lane_count = core.GetSVELaneCount(kHRegSize);
    for (int i = lane_count - 1; i >= 0; i--) {
      uint16_t expected = core.zreg_lane<uint16_t>(z16.GetCode(), i);
      uint16_t input = core.zreg_lane<uint8_t>(z9.GetCode(), i + lane_count);
      ASSERT_EQUAL_64(expected, input);
    }

    lane_count = core.GetSVELaneCount(kSRegSize);
    for (int i = lane_count - 1; i >= 0; i--) {
      uint32_t expected = core.zreg_lane<uint32_t>(z17.GetCode(), i);
      uint32_t input = core.zreg_lane<uint16_t>(z9.GetCode(), i + lane_count);
      ASSERT_EQUAL_64(expected, input);
    }

    lane_count = core.GetSVELaneCount(kDRegSize);
    for (int i = lane_count - 1; i >= 0; i--) {
      uint64_t expected = core.zreg_lane<uint64_t>(z18.GetCode(), i);
      uint64_t input = core.zreg_lane<uint32_t>(z9.GetCode(), i + lane_count);
      ASSERT_EQUAL_64(expected, input);
    }

    // Uuunpklo
    lane_count = core.GetSVELaneCount(kHRegSize);
    for (int i = lane_count - 1; i >= 0; i--) {
      uint16_t expected = core.zreg_lane<uint16_t>(z19.GetCode(), i);
      uint16_t input = core.zreg_lane<uint8_t>(z9.GetCode(), i);
      ASSERT_EQUAL_64(expected, input);
    }

    lane_count = core.GetSVELaneCount(kSRegSize);
    for (int i = lane_count - 1; i >= 0; i--) {
      uint32_t expected = core.zreg_lane<uint32_t>(z20.GetCode(), i);
      uint32_t input = core.zreg_lane<uint16_t>(z9.GetCode(), i);
      ASSERT_EQUAL_64(expected, input);
    }

    lane_count = core.GetSVELaneCount(kDRegSize);
    for (int i = lane_count - 1; i >= 0; i--) {
      uint64_t expected = core.zreg_lane<uint64_t>(z21.GetCode(), i);
      uint64_t input = core.zreg_lane<uint32_t>(z9.GetCode(), i);
      ASSERT_EQUAL_64(expected, input);
    }

    ASSERT_EQUAL_SVE(z13, z22);
    ASSERT_EQUAL_SVE(z19, z23);
  }
}

TEST_SVE(sve_cnot_not) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in[] = {0x0000000000000000, 0x00000000e1c30000, 0x123456789abcdef0};

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         1,                      1,                      0
  // For S lanes:         1,          1,          1,          0,          0
  // For H lanes:   0,    1,    0,    1,    1,    1,    0,    0,    1,    0
  int pg_in[] = {1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0};
  Initialise(&masm, p0.VnB(), pg_in);
  PRegisterM pg = p0.Merging();

  // These are merging operations, so we have to initialise the result register.
  // We use a mixture of constructive and destructive operations.

  InsrHelper(&masm, z31.VnD(), in);
  // Make a copy so we can check that constructive operations preserve zn.
  __ Mov(z30, z31);

  // For constructive operations, use a different initial result value.
  __ Index(z29.VnB(), 0, -1);

  __ Mov(z0, z31);
  __ Cnot(z0.VnB(), pg, z0.VnB());  // destructive
  __ Mov(z1, z29);
  __ Cnot(z1.VnH(), pg, z31.VnH());
  __ Mov(z2, z31);
  __ Cnot(z2.VnS(), pg, z2.VnS());  // destructive
  __ Mov(z3, z29);
  __ Cnot(z3.VnD(), pg, z31.VnD());

  __ Mov(z4, z29);
  __ Not(z4.VnB(), pg, z31.VnB());
  __ Mov(z5, z31);
  __ Not(z5.VnH(), pg, z5.VnH());  // destructive
  __ Mov(z6, z29);
  __ Not(z6.VnS(), pg, z31.VnS());
  __ Mov(z7, z31);
  __ Not(z7.VnD(), pg, z7.VnD());  // destructive

  END();

  if (CAN_RUN()) {
    RUN();

    // Check that constructive operations preserve their inputs.
    ASSERT_EQUAL_SVE(z30, z31);

    // clang-format off

    // Cnot (B) destructive
    uint64_t expected_z0[] =
    // pg:  0 0 0 0 1 0 1 1     1 0 0 1 0 1 1 1     0 0 1 0 1 1 1 0
        {0x0000000001000101, 0x01000001e1000101, 0x12340078000000f0};
    ASSERT_EQUAL_SVE(expected_z0, z0.VnD());

    // Cnot (H)
    uint64_t expected_z1[] =
    // pg:    0   0   0   1       0   1   1   1       0   0   1   0
        {0xe9eaebecedee0001, 0xf1f2000100000001, 0xf9fafbfc0000ff00};
    ASSERT_EQUAL_SVE(expected_z1, z1.VnD());

    // Cnot (S) destructive
    uint64_t expected_z2[] =
    // pg:        0       1           1       1           0       0
        {0x0000000000000001, 0x0000000100000000, 0x123456789abcdef0};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());

    // Cnot (D)
    uint64_t expected_z3[] =
    // pg:                1                   1                   0
        {0x0000000000000001, 0x0000000000000000, 0xf9fafbfcfdfeff00};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());

    // Not (B)
    uint64_t expected_z4[] =
    // pg:  0 0 0 0 1 0 1 1     1 0 0 1 0 1 1 1     0 0 1 0 1 1 1 0
        {0xe9eaebecffeeffff, 0xfff2f3fff53cffff, 0xf9faa9fc65432100};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());

    // Not (H) destructive
    uint64_t expected_z5[] =
    // pg:    0   0   0   1       0   1   1   1       0   0   1   0
        {0x000000000000ffff, 0x0000ffff1e3cffff, 0x123456786543def0};
    ASSERT_EQUAL_SVE(expected_z5, z5.VnD());

    // Not (S)
    uint64_t expected_z6[] =
    // pg:        0       1           1       1           0       0
        {0xe9eaebecffffffff, 0xffffffff1e3cffff, 0xf9fafbfcfdfeff00};
    ASSERT_EQUAL_SVE(expected_z6, z6.VnD());

    // Not (D) destructive
    uint64_t expected_z7[] =
    // pg:                1                   1                   0
        {0xffffffffffffffff, 0xffffffff1e3cffff, 0x123456789abcdef0};
    ASSERT_EQUAL_SVE(expected_z7, z7.VnD());

    // clang-format on
  }
}

TEST_SVE(sve_fabs_fneg) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Include FP64, FP32 and FP16 signalling NaNs. Most FP operations quieten
  // NaNs, but fabs and fneg do not.
  uint64_t in[] = {0xc04500004228d140,  // Recognisable (+/-42) values.
                   0xfff00000ff80fc01,  // Signalling NaNs.
                   0x123456789abcdef0};

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         1,                      1,                      0
  // For S lanes:         1,          1,          1,          0,          0
  // For H lanes:   0,    1,    0,    1,    1,    1,    0,    0,    1,    0
  int pg_in[] = {1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0};
  Initialise(&masm, p0.VnB(), pg_in);
  PRegisterM pg = p0.Merging();

  // These are merging operations, so we have to initialise the result register.
  // We use a mixture of constructive and destructive operations.

  InsrHelper(&masm, z31.VnD(), in);
  // Make a copy so we can check that constructive operations preserve zn.
  __ Mov(z30, z31);

  // For constructive operations, use a different initial result value.
  __ Index(z29.VnB(), 0, -1);

  __ Mov(z0, z29);
  __ Fabs(z0.VnH(), pg, z31.VnH());
  __ Mov(z1, z31);
  __ Fabs(z1.VnS(), pg, z1.VnS());  // destructive
  __ Mov(z2, z29);
  __ Fabs(z2.VnD(), pg, z31.VnD());

  __ Mov(z3, z31);
  __ Fneg(z3.VnH(), pg, z3.VnH());  // destructive
  __ Mov(z4, z29);
  __ Fneg(z4.VnS(), pg, z31.VnS());
  __ Mov(z5, z31);
  __ Fneg(z5.VnD(), pg, z5.VnD());  // destructive

  END();

  if (CAN_RUN()) {
    RUN();

    // Check that constructive operations preserve their inputs.
    ASSERT_EQUAL_SVE(z30, z31);

    // clang-format off

    // Fabs (H)
    uint64_t expected_z0[] =
    // pg:    0   0   0   1       0   1   1   1       0   0   1   0
        {0xe9eaebecedee5140, 0xf1f200007f807c01, 0xf9fafbfc1abcff00};
    ASSERT_EQUAL_SVE(expected_z0, z0.VnD());

    // Fabs (S) destructive
    uint64_t expected_z1[] =
    // pg:        0       1           1       1           0       0
        {0xc04500004228d140, 0x7ff000007f80fc01, 0x123456789abcdef0};
    ASSERT_EQUAL_SVE(expected_z1, z1.VnD());

    // Fabs (D)
    uint64_t expected_z2[] =
    // pg:                1                   1                   0
        {0x404500004228d140, 0x7ff00000ff80fc01, 0xf9fafbfcfdfeff00};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());

    // Fneg (H) destructive
    uint64_t expected_z3[] =
    // pg:    0   0   0   1       0   1   1   1       0   0   1   0
        {0xc045000042285140, 0xfff080007f807c01, 0x123456781abcdef0};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());

    // Fneg (S)
    uint64_t expected_z4[] =
    // pg:        0       1           1       1           0       0
        {0xe9eaebecc228d140, 0x7ff000007f80fc01, 0xf9fafbfcfdfeff00};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());

    // Fneg (D) destructive
    uint64_t expected_z5[] =
    // pg:                1                   1                   0
        {0x404500004228d140, 0x7ff00000ff80fc01, 0x123456789abcdef0};
    ASSERT_EQUAL_SVE(expected_z5, z5.VnD());

    // clang-format on
  }
}

TEST_SVE(sve_cls_clz_cnt) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in[] = {0x0000000000000000, 0xfefcf8f0e1c3870f, 0x123456789abcdef0};

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         1,                      1,                      0
  // For S lanes:         1,          1,          1,          0,          0
  // For H lanes:   0,    1,    0,    1,    1,    1,    0,    0,    1,    0
  int pg_in[] = {1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0};
  Initialise(&masm, p0.VnB(), pg_in);
  PRegisterM pg = p0.Merging();

  // These are merging operations, so we have to initialise the result register.
  // We use a mixture of constructive and destructive operations.

  InsrHelper(&masm, z31.VnD(), in);
  // Make a copy so we can check that constructive operations preserve zn.
  __ Mov(z30, z31);

  // For constructive operations, use a different initial result value.
  __ Index(z29.VnB(), 0, -1);

  __ Mov(z0, z29);
  __ Cls(z0.VnB(), pg, z31.VnB());
  __ Mov(z1, z31);
  __ Clz(z1.VnH(), pg, z1.VnH());  // destructive
  __ Mov(z2, z29);
  __ Cnt(z2.VnS(), pg, z31.VnS());
  __ Mov(z3, z31);
  __ Cnt(z3.VnD(), pg, z3.VnD());  // destructive

  END();

  if (CAN_RUN()) {
    RUN();
    // Check that non-destructive operations preserve their inputs.
    ASSERT_EQUAL_SVE(z30, z31);

    // clang-format off

    // cls (B)
    uint8_t expected_z0[] =
    // pg:  0     0     0     0     1     0     1     1
    // pg:  1     0     0     1     0     1     1     1
    // pg:  0     0     1     0     1     1     1     0
        {0xe9, 0xea, 0xeb, 0xec,    7, 0xee,    7,    7,
            6, 0xf2, 0xf3,    3, 0xf5,    1,    0,    3,
         0xf9, 0xfa,    0, 0xfc,    0,    0,    1, 0x00};
    ASSERT_EQUAL_SVE(expected_z0, z0.VnB());

    // clz (H) destructive
    uint16_t expected_z1[] =
    // pg:    0       0       0       1
    // pg:    0       1       1       1
    // pg:    0       0       1       0
        {0x0000, 0x0000, 0x0000,     16,
         0xfefc,      0,      0,      0,
         0x1234, 0x5678,      0, 0xdef0};
    ASSERT_EQUAL_SVE(expected_z1, z1.VnH());

    // cnt (S)
    uint32_t expected_z2[] =
    // pg:        0           1
    // pg:        1           1
    // pg:        0           0
        {0xe9eaebec,          0,
                 22,         16,
         0xf9fafbfc, 0xfdfeff00};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnS());

    // cnt (D) destructive
    uint64_t expected_z3[] =
    // pg:                1                   1                   0
        {                 0,                 38, 0x123456789abcdef0};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());

    // clang-format on
  }
}

TEST_SVE(sve_sxt) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in[] = {0x01f203f405f607f8, 0xfefcf8f0e1c3870f, 0x123456789abcdef0};

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         1,                      1,                      0
  // For S lanes:         1,          1,          1,          0,          0
  // For H lanes:   0,    1,    0,    1,    1,    1,    0,    0,    1,    0
  int pg_in[] = {1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0};
  Initialise(&masm, p0.VnB(), pg_in);
  PRegisterM pg = p0.Merging();

  // These are merging operations, so we have to initialise the result register.
  // We use a mixture of constructive and destructive operations.

  InsrHelper(&masm, z31.VnD(), in);
  // Make a copy so we can check that constructive operations preserve zn.
  __ Mov(z30, z31);

  // For constructive operations, use a different initial result value.
  __ Index(z29.VnB(), 0, -1);

  __ Mov(z0, z31);
  __ Sxtb(z0.VnH(), pg, z0.VnH());  // destructive
  __ Mov(z1, z29);
  __ Sxtb(z1.VnS(), pg, z31.VnS());
  __ Mov(z2, z31);
  __ Sxtb(z2.VnD(), pg, z2.VnD());  // destructive
  __ Mov(z3, z29);
  __ Sxth(z3.VnS(), pg, z31.VnS());
  __ Mov(z4, z31);
  __ Sxth(z4.VnD(), pg, z4.VnD());  // destructive
  __ Mov(z5, z29);
  __ Sxtw(z5.VnD(), pg, z31.VnD());

  END();

  if (CAN_RUN()) {
    RUN();
    // Check that constructive operations preserve their inputs.
    ASSERT_EQUAL_SVE(z30, z31);

    // clang-format off

    // Sxtb (H) destructive
    uint64_t expected_z0[] =
    // pg:    0   0   0   1       0   1   1   1       0   0   1   0
        {0x01f203f405f6fff8, 0xfefcfff0ffc3000f, 0x12345678ffbcdef0};
    ASSERT_EQUAL_SVE(expected_z0, z0.VnD());

    // Sxtb (S)
    uint64_t expected_z1[] =
    // pg:        0       1           1       1           0       0
        {0xe9eaebecfffffff8, 0xfffffff00000000f, 0xf9fafbfcfdfeff00};
    ASSERT_EQUAL_SVE(expected_z1, z1.VnD());

    // Sxtb (D) destructive
    uint64_t expected_z2[] =
    // pg:                1                   1                   0
        {0xfffffffffffffff8, 0x000000000000000f, 0x123456789abcdef0};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());

    // Sxth (S)
    uint64_t expected_z3[] =
    // pg:        0       1           1       1           0       0
        {0xe9eaebec000007f8, 0xfffff8f0ffff870f, 0xf9fafbfcfdfeff00};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());

    // Sxth (D) destructive
    uint64_t expected_z4[] =
    // pg:                1                   1                   0
        {0x00000000000007f8, 0xffffffffffff870f, 0x123456789abcdef0};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());

    // Sxtw (D)
    uint64_t expected_z5[] =
    // pg:                1                   1                   0
        {0x0000000005f607f8, 0xffffffffe1c3870f, 0xf9fafbfcfdfeff00};
    ASSERT_EQUAL_SVE(expected_z5, z5.VnD());

    // clang-format on
  }
}

TEST_SVE(sve_uxt) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in[] = {0x01f203f405f607f8, 0xfefcf8f0e1c3870f, 0x123456789abcdef0};

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         1,                      1,                      0
  // For S lanes:         1,          1,          1,          0,          0
  // For H lanes:   0,    1,    0,    1,    1,    1,    0,    0,    1,    0
  int pg_in[] = {1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0};
  Initialise(&masm, p0.VnB(), pg_in);
  PRegisterM pg = p0.Merging();

  // These are merging operations, so we have to initialise the result register.
  // We use a mixture of constructive and destructive operations.

  InsrHelper(&masm, z31.VnD(), in);
  // Make a copy so we can check that constructive operations preserve zn.
  __ Mov(z30, z31);

  // For constructive operations, use a different initial result value.
  __ Index(z29.VnB(), 0, -1);

  __ Mov(z0, z29);
  __ Uxtb(z0.VnH(), pg, z31.VnH());
  __ Mov(z1, z31);
  __ Uxtb(z1.VnS(), pg, z1.VnS());  // destructive
  __ Mov(z2, z29);
  __ Uxtb(z2.VnD(), pg, z31.VnD());
  __ Mov(z3, z31);
  __ Uxth(z3.VnS(), pg, z3.VnS());  // destructive
  __ Mov(z4, z29);
  __ Uxth(z4.VnD(), pg, z31.VnD());
  __ Mov(z5, z31);
  __ Uxtw(z5.VnD(), pg, z5.VnD());  // destructive

  END();

  if (CAN_RUN()) {
    RUN();
    // clang-format off

    // Uxtb (H)
    uint64_t expected_z0[] =
    // pg:    0   0   0   1       0   1   1   1       0   0   1   0
        {0xe9eaebecedee00f8, 0xf1f200f000c3000f, 0xf9fafbfc00bcff00};
    ASSERT_EQUAL_SVE(expected_z0, z0.VnD());

    // Uxtb (S) destructive
    uint64_t expected_z1[] =
    // pg:        0       1           1       1           0       0
        {0x01f203f4000000f8, 0x000000f00000000f, 0x123456789abcdef0};
    ASSERT_EQUAL_SVE(expected_z1, z1.VnD());

    // Uxtb (D)
    uint64_t expected_z2[] =
    // pg:                1                   1                   0
        {0x00000000000000f8, 0x000000000000000f, 0xf9fafbfcfdfeff00};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());

    // Uxth (S) destructive
    uint64_t expected_z3[] =
    // pg:        0       1           1       1           0       0
        {0x01f203f4000007f8, 0x0000f8f00000870f, 0x123456789abcdef0};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());

    // Uxth (D)
    uint64_t expected_z4[] =
    // pg:                1                   1                   0
        {0x00000000000007f8, 0x000000000000870f, 0xf9fafbfcfdfeff00};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());

    // Uxtw (D) destructive
    uint64_t expected_z5[] =
    // pg:                1                   1                   0
        {0x0000000005f607f8, 0x00000000e1c3870f, 0x123456789abcdef0};
    ASSERT_EQUAL_SVE(expected_z5, z5.VnD());

    // clang-format on
  }
}

TEST_SVE(sve_abs_neg) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in[] = {0x01f203f405f607f8, 0xfefcf8f0e1c3870f, 0x123456789abcdef0};

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         1,                      1,                      0
  // For S lanes:         1,          1,          1,          0,          0
  // For H lanes:   0,    1,    0,    1,    1,    1,    0,    0,    1,    0
  int pg_in[] = {1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0};
  Initialise(&masm, p0.VnB(), pg_in);
  PRegisterM pg = p0.Merging();

  InsrHelper(&masm, z31.VnD(), in);

  // These are merging operations, so we have to initialise the result register.
  // We use a mixture of constructive and destructive operations.

  InsrHelper(&masm, z31.VnD(), in);
  // Make a copy so we can check that constructive operations preserve zn.
  __ Mov(z30, z31);

  // For constructive operations, use a different initial result value.
  __ Index(z29.VnB(), 0, -1);

  __ Mov(z0, z31);
  __ Abs(z0.VnD(), pg, z0.VnD());  // destructive
  __ Mov(z1, z29);
  __ Abs(z1.VnB(), pg, z31.VnB());

  __ Mov(z2, z31);
  __ Neg(z2.VnH(), pg, z2.VnH());  // destructive
  __ Mov(z3, z29);
  __ Neg(z3.VnS(), pg, z31.VnS());

  // The unpredicated form of `Neg` is implemented using `subr`.
  __ Mov(z4, z31);
  __ Neg(z4.VnB(), z4.VnB());  // destructive
  __ Mov(z5, z29);
  __ Neg(z5.VnD(), z31.VnD());

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(z30, z31);

    // clang-format off

    // Abs (D) destructive
    uint64_t expected_z0[] =
    // pg:                1                   1                   0
        {0x01f203f405f607f8, 0x0103070f1e3c78f1, 0x123456789abcdef0};
    ASSERT_EQUAL_SVE(expected_z0, z0.VnD());

    // Abs (B)
    uint64_t expected_z1[] =
    // pg:  0 0 0 0 1 0 1 1     1 0 0 1 0 1 1 1     0 0 1 0 1 1 1 0
        {0xe9eaebec05ee0708, 0x02f2f310f53d790f, 0xf9fa56fc66442200};
    ASSERT_EQUAL_SVE(expected_z1, z1.VnD());

    // Neg (H) destructive
    uint64_t expected_z2[] =
    // pg:    0   0   0   1       0   1   1   1       0   0   1   0
        {0x01f203f405f6f808, 0xfefc07101e3d78f1, 0x123456786544def0};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());

    // Neg (S)
    uint64_t expected_z3[] =
    // pg:        0       1           1       1           0       0
        {0xe9eaebecfa09f808, 0x010307101e3c78f1, 0xf9fafbfcfdfeff00};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());

    // Neg (B) destructive, unpredicated
    uint64_t expected_z4[] =
        {0xff0efd0cfb0af908, 0x020408101f3d79f1, 0xeeccaa8866442210};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());

    // Neg (D) unpredicated
    uint64_t expected_z5[] =
        {0xfe0dfc0bfa09f808, 0x0103070f1e3c78f1, 0xedcba98765432110};
    ASSERT_EQUAL_SVE(expected_z5, z5.VnD());

    // clang-format on
  }
}

TEST_SVE(sve_cpy) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE, CPUFeatures::kNEON);
  START();

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         0,                      1,                      1
  // For S lanes:         0,          1,          1,          0,          1
  // For H lanes:   1,    0,    0,    1,    0,    1,    1,    0,    0,    1
  int pg_in[] = {1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1};

  PRegisterM pg = p7.Merging();
  Initialise(&masm, pg.VnB(), pg_in);

  // These are merging operations, so we have to initialise the result registers
  // for each operation.
  for (unsigned i = 0; i < kNumberOfZRegisters; i++) {
    __ Index(ZRegister(i, kBRegSize), 0, -1);
  }

  // Recognisable values to copy.
  __ Mov(x0, 0xdeadbeefdeadbe42);
  __ Mov(x1, 0xdeadbeefdead8421);
  __ Mov(x2, 0xdeadbeef80042001);
  __ Mov(x3, 0x8000000420000001);

  // Use NEON moves, to avoid testing SVE `cpy` against itself.
  __ Dup(v28.V2D(), x0);
  __ Dup(v29.V2D(), x1);
  __ Dup(v30.V2D(), x2);
  __ Dup(v31.V2D(), x3);

  // Register forms (CPY_z_p_r)
  __ Cpy(z0.VnB(), pg, w0);
  __ Cpy(z1.VnH(), pg, x1);  // X registers are accepted for small lanes.
  __ Cpy(z2.VnS(), pg, w2);
  __ Cpy(z3.VnD(), pg, x3);

  // VRegister forms (CPY_z_p_v)
  __ Cpy(z4.VnB(), pg, b28);
  __ Cpy(z5.VnH(), pg, h29);
  __ Cpy(z6.VnS(), pg, s30);
  __ Cpy(z7.VnD(), pg, d31);

  // Check that we can copy the stack pointer.
  __ Mov(x10, sp);
  __ Mov(sp, 0xabcabcabcabcabca);  // Set sp to a known value.
  __ Cpy(z16.VnB(), pg, sp);
  __ Cpy(z17.VnH(), pg, wsp);
  __ Cpy(z18.VnS(), pg, wsp);
  __ Cpy(z19.VnD(), pg, sp);
  __ Mov(sp, x10);  // Restore sp.

  END();

  if (CAN_RUN()) {
    RUN();
    // clang-format off

    uint64_t expected_b[] =
    // pg:  0 0 0 0 1 1 1 0     1 0 0 1 1 0 1 1     0 1 0 0 0 0 0 1
        {0xe9eaebec424242f0, 0x42f2f34242f64242, 0xf942fbfcfdfeff42};
    ASSERT_EQUAL_SVE(expected_b, z0.VnD());
    ASSERT_EQUAL_SVE(expected_b, z4.VnD());

    uint64_t expected_h[] =
    // pg:    0   0   1   0       0   1   0   1       1   0   0   1
        {0xe9eaebec8421eff0, 0xf1f28421f5f68421, 0x8421fbfcfdfe8421};
    ASSERT_EQUAL_SVE(expected_h, z1.VnD());
    ASSERT_EQUAL_SVE(expected_h, z5.VnD());

    uint64_t expected_s[] =
    // pg:        0       0           1       1           0       1
        {0xe9eaebecedeeeff0, 0x8004200180042001, 0xf9fafbfc80042001};
    ASSERT_EQUAL_SVE(expected_s, z2.VnD());
    ASSERT_EQUAL_SVE(expected_s, z6.VnD());

    uint64_t expected_d[] =
    // pg:                0                   1                   1
        {0xe9eaebecedeeeff0, 0x8000000420000001, 0x8000000420000001};
    ASSERT_EQUAL_SVE(expected_d, z3.VnD());
    ASSERT_EQUAL_SVE(expected_d, z7.VnD());


    uint64_t expected_b_sp[] =
    // pg:  0 0 0 0 1 1 1 0     1 0 0 1 1 0 1 1     0 1 0 0 0 0 0 1
        {0xe9eaebeccacacaf0, 0xcaf2f3cacaf6caca, 0xf9cafbfcfdfeffca};
    ASSERT_EQUAL_SVE(expected_b_sp, z16.VnD());

    uint64_t expected_h_sp[] =
    // pg:    0   0   1   0       0   1   0   1       1   0   0   1
        {0xe9eaebecabcaeff0, 0xf1f2abcaf5f6abca, 0xabcafbfcfdfeabca};
    ASSERT_EQUAL_SVE(expected_h_sp, z17.VnD());

    uint64_t expected_s_sp[] =
    // pg:        0       0           1       1           0       1
        {0xe9eaebecedeeeff0, 0xcabcabcacabcabca, 0xf9fafbfccabcabca};
    ASSERT_EQUAL_SVE(expected_s_sp, z18.VnD());

    uint64_t expected_d_sp[] =
    // pg:                0                   1                   1
        {0xe9eaebecedeeeff0, 0xabcabcabcabcabca, 0xabcabcabcabcabca};
    ASSERT_EQUAL_SVE(expected_d_sp, z19.VnD());

    // clang-format on
  }
}

TEST_SVE(sve_cpy_imm) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         0,                      1,                      1
  // For S lanes:         0,          1,          1,          0,          1
  // For H lanes:   1,    0,    0,    1,    0,    1,    1,    0,    0,    1
  int pg_in[] = {1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1};

  PRegister pg = p7;
  Initialise(&masm, pg.VnB(), pg_in);

  // These are (mostly) merging operations, so we have to initialise the result
  // registers for each operation.
  for (unsigned i = 0; i < kNumberOfZRegisters; i++) {
    __ Index(ZRegister(i, kBRegSize), 0, -1);
  }

  // Encodable integer forms (CPY_z_p_i)
  __ Cpy(z0.VnB(), pg.Merging(), 0);
  __ Cpy(z1.VnB(), pg.Zeroing(), 42);
  __ Cpy(z2.VnB(), pg.Merging(), -42);
  __ Cpy(z3.VnB(), pg.Zeroing(), 0xff);
  __ Cpy(z4.VnH(), pg.Merging(), 127);
  __ Cpy(z5.VnS(), pg.Zeroing(), -128);
  __ Cpy(z6.VnD(), pg.Merging(), -1);

  // Forms encodable using fcpy.
  __ Cpy(z7.VnH(), pg.Merging(), Float16ToRawbits(Float16(-31.0)));
  __ Cpy(z8.VnS(), pg.Zeroing(), FloatToRawbits(2.0f));
  __ Cpy(z9.VnD(), pg.Merging(), DoubleToRawbits(-4.0));

  // Other forms use a scratch register.
  __ Cpy(z10.VnH(), pg.Merging(), 0xff);
  __ Cpy(z11.VnD(), pg.Zeroing(), 0x0123456789abcdef);

  END();

  if (CAN_RUN()) {
    RUN();
    // clang-format off

    uint64_t expected_z0[] =
    // pg:  0 0 0 0 1 1 1 0     1 0 0 1 1 0 1 1     0 1 0 0 0 0 0 1
        {0xe9eaebec000000f0, 0x00f2f30000f60000, 0xf900fbfcfdfeff00};
    ASSERT_EQUAL_SVE(expected_z0, z0.VnD());

    uint64_t expected_z1[] =
    // pg:  0 0 0 0 1 1 1 0     1 0 0 1 1 0 1 1     0 1 0 0 0 0 0 1
        {0x000000002a2a2a00, 0x2a00002a2a002a2a, 0x002a00000000002a};
    ASSERT_EQUAL_SVE(expected_z1, z1.VnD());

    uint64_t expected_z2[] =
    // pg:  0 0 0 0 1 1 1 0     1 0 0 1 1 0 1 1     0 1 0 0 0 0 0 1
        {0xe9eaebecd6d6d6f0, 0xd6f2f3d6d6f6d6d6, 0xf9d6fbfcfdfeffd6};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());

    uint64_t expected_z3[] =
    // pg:  0 0 0 0 1 1 1 0     1 0 0 1 1 0 1 1     0 1 0 0 0 0 0 1
        {0x00000000ffffff00, 0xff0000ffff00ffff, 0x00ff0000000000ff};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());

    uint64_t expected_z4[] =
    // pg:    0   0   1   0       0   1   0   1       1   0   0   1
        {0xe9eaebec007feff0, 0xf1f2007ff5f6007f, 0x007ffbfcfdfe007f};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());

    uint64_t expected_z5[] =
    // pg:        0       0           1       1           0       1
        {0x0000000000000000, 0xffffff80ffffff80, 0x00000000ffffff80};
    ASSERT_EQUAL_SVE(expected_z5, z5.VnD());

    uint64_t expected_z6[] =
    // pg:                0                   1                   1
        {0xe9eaebecedeeeff0, 0xffffffffffffffff, 0xffffffffffffffff};
    ASSERT_EQUAL_SVE(expected_z6, z6.VnD());

    uint64_t expected_z7[] =
    // pg:    0   0   1   0       0   1   0   1       1   0   0   1
        {0xe9eaebeccfc0eff0, 0xf1f2cfc0f5f6cfc0, 0xcfc0fbfcfdfecfc0};
    ASSERT_EQUAL_SVE(expected_z7, z7.VnD());

    uint64_t expected_z8[] =
    // pg:        0       0           1       1           0       1
        {0x0000000000000000, 0x4000000040000000, 0x0000000040000000};
    ASSERT_EQUAL_SVE(expected_z8, z8.VnD());

    uint64_t expected_z9[] =
    // pg:                0                   1                   1
        {0xe9eaebecedeeeff0, 0xc010000000000000, 0xc010000000000000};
    ASSERT_EQUAL_SVE(expected_z9, z9.VnD());

    uint64_t expected_z10[] =
    // pg:    0   0   1   0       0   1   0   1       1   0   0   1
        {0xe9eaebec00ffeff0, 0xf1f200fff5f600ff, 0x00fffbfcfdfe00ff};
    ASSERT_EQUAL_SVE(expected_z10, z10.VnD());

    uint64_t expected_z11[] =
    // pg:                0                   1                   1
        {0x0000000000000000, 0x0123456789abcdef, 0x0123456789abcdef};
    ASSERT_EQUAL_SVE(expected_z11, z11.VnD());

    // clang-format on
  }
}

TEST_SVE(sve_fcpy_imm) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         0,                      1,                      1
  // For S lanes:         0,          1,          1,          0,          1
  // For H lanes:   1,    0,    0,    1,    0,    1,    1,    0,    0,    1
  int pg_in[] = {1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1};

  PRegister pg = p7;
  Initialise(&masm, pg.VnB(), pg_in);

  // These are (mostly) merging operations, so we have to initialise the result
  // registers for each operation.
  for (unsigned i = 0; i < kNumberOfZRegisters; i++) {
    __ Index(ZRegister(i, kBRegSize), 0, -1);
  }

  // Encodable floating-point forms (FCPY_z_p_i)
  __ Fcpy(z1.VnH(), pg.Merging(), Float16(1.0));
  __ Fcpy(z2.VnH(), pg.Merging(), -2.0f);
  __ Fcpy(z3.VnH(), pg.Merging(), 3.0);
  __ Fcpy(z4.VnS(), pg.Merging(), Float16(-4.0));
  __ Fcpy(z5.VnS(), pg.Merging(), 5.0f);
  __ Fcpy(z6.VnS(), pg.Merging(), 6.0);
  __ Fcpy(z7.VnD(), pg.Merging(), Float16(7.0));
  __ Fcpy(z8.VnD(), pg.Merging(), 8.0f);
  __ Fcpy(z9.VnD(), pg.Merging(), -9.0);

  // Unencodable immediates.
  __ Fcpy(z10.VnS(), pg.Merging(), 0.0);
  __ Fcpy(z11.VnH(), pg.Merging(), Float16(42.0));
  __ Fcpy(z12.VnD(), pg.Merging(), RawbitsToDouble(0x7ff0000012340000));  // NaN
  __ Fcpy(z13.VnH(), pg.Merging(), kFP64NegativeInfinity);

  END();

  if (CAN_RUN()) {
    RUN();
    // clang-format off

    // 1.0 as FP16: 0x3c00
    uint64_t expected_z1[] =
    // pg:    0   0   1   0       0   1   0   1       1   0   0   1
        {0xe9eaebec3c00eff0, 0xf1f23c00f5f63c00, 0x3c00fbfcfdfe3c00};
    ASSERT_EQUAL_SVE(expected_z1, z1.VnD());

    // -2.0 as FP16: 0xc000
    uint64_t expected_z2[] =
    // pg:    0   0   1   0       0   1   0   1       1   0   0   1
        {0xe9eaebecc000eff0, 0xf1f2c000f5f6c000, 0xc000fbfcfdfec000};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());

    // 3.0 as FP16: 0x4200
    uint64_t expected_z3[] =
    // pg:    0   0   1   0       0   1   0   1       1   0   0   1
        {0xe9eaebec4200eff0, 0xf1f24200f5f64200, 0x4200fbfcfdfe4200};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());

    // -4.0 as FP32: 0xc0800000
    uint64_t expected_z4[] =
    // pg:        0       0           1       1           0       1
        {0xe9eaebecedeeeff0, 0xc0800000c0800000, 0xf9fafbfcc0800000};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());

    // 5.0 as FP32: 0x40a00000
    uint64_t expected_z5[] =
    // pg:        0       0           1       1           0       1
        {0xe9eaebecedeeeff0, 0x40a0000040a00000, 0xf9fafbfc40a00000};
    ASSERT_EQUAL_SVE(expected_z5, z5.VnD());

    // 6.0 as FP32: 0x40c00000
    uint64_t expected_z6[] =
    // pg:        0       0           1       1           0       1
        {0xe9eaebecedeeeff0, 0x40c0000040c00000, 0xf9fafbfc40c00000};
    ASSERT_EQUAL_SVE(expected_z6, z6.VnD());

    // 7.0 as FP64: 0x401c000000000000
    uint64_t expected_z7[] =
    // pg:                0                   1                   1
        {0xe9eaebecedeeeff0, 0x401c000000000000, 0x401c000000000000};
    ASSERT_EQUAL_SVE(expected_z7, z7.VnD());

    // 8.0 as FP64: 0x4020000000000000
    uint64_t expected_z8[] =
    // pg:                0                   1                   1
        {0xe9eaebecedeeeff0, 0x4020000000000000, 0x4020000000000000};
    ASSERT_EQUAL_SVE(expected_z8, z8.VnD());

    // -9.0 as FP64: 0xc022000000000000
    uint64_t expected_z9[] =
    // pg:                0                   1                   1
        {0xe9eaebecedeeeff0, 0xc022000000000000, 0xc022000000000000};
    ASSERT_EQUAL_SVE(expected_z9, z9.VnD());

    // 0.0 as FP32: 0x00000000
    uint64_t expected_z10[] =
    // pg:        0       0           1       1           0       1
        {0xe9eaebecedeeeff0, 0x0000000000000000, 0xf9fafbfc00000000};
    ASSERT_EQUAL_SVE(expected_z10, z10.VnD());

    // 42.0 as FP16: 0x5140
    uint64_t expected_z11[] =
    // pg:    0   0   1   0       0   1   0   1       1   0   0   1
        {0xe9eaebec5140eff0, 0xf1f25140f5f65140, 0x5140fbfcfdfe5140};
    ASSERT_EQUAL_SVE(expected_z11, z11.VnD());

    // Signalling NaN (with payload): 0x7ff0000012340000
    uint64_t expected_z12[] =
    // pg:                0                   1                   1
        {0xe9eaebecedeeeff0, 0x7ff0000012340000, 0x7ff0000012340000};
    ASSERT_EQUAL_SVE(expected_z12, z12.VnD());

    // -infinity as FP16: 0xfc00
    uint64_t expected_z13[] =
    // pg:    0   0   1   0       0   1   0   1       1   0   0   1
        {0xe9eaebecfc00eff0, 0xf1f2fc00f5f6fc00, 0xfc00fbfcfdfefc00};
    ASSERT_EQUAL_SVE(expected_z13, z13.VnD());

    // clang-format on
  }
}

TEST_SVE(sve_permute_vector_unpredicated_table_lookup) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t table_inputs[] = {0xffeeddccbbaa9988, 0x7766554433221100};

  int index_b[] = {255, 255, 11, 10, 15, 14, 13, 12, 1, 0, 4, 3, 7, 6, 5, 4};

  int index_h[] = {5, 6, 7, 8, 2, 3, 6, 4};

  int index_s[] = {1, 3, 2, 31, -1};

  int index_d[] = {31, 1};

  // Initialize the register with a value that doesn't existed in the table.
  __ Dup(z9.VnB(), 0x1f);
  InsrHelper(&masm, z9.VnD(), table_inputs);

  ZRegister ind_b = z0.WithLaneSize(kBRegSize);
  ZRegister ind_h = z1.WithLaneSize(kHRegSize);
  ZRegister ind_s = z2.WithLaneSize(kSRegSize);
  ZRegister ind_d = z3.WithLaneSize(kDRegSize);

  InsrHelper(&masm, ind_b, index_b);
  InsrHelper(&masm, ind_h, index_h);
  InsrHelper(&masm, ind_s, index_s);
  InsrHelper(&masm, ind_d, index_d);

  __ Tbl(z26.VnB(), z9.VnB(), ind_b);

  __ Tbl(z27.VnH(), z9.VnH(), ind_h);

  __ Tbl(z28.VnS(), z9.VnS(), ind_s);

  __ Tbl(z29.VnD(), z9.VnD(), ind_d);

  END();

  if (CAN_RUN()) {
    RUN();

    // clang-format off
    unsigned z26_expected[] = {0x1f, 0x1f, 0xbb, 0xaa, 0xff, 0xee, 0xdd, 0xcc,
                               0x11, 0x00, 0x44, 0x33, 0x77, 0x66, 0x55, 0x44};

    unsigned z27_expected[] = {0xbbaa, 0xddcc, 0xffee, 0x1f1f,
                               0x5544, 0x7766, 0xddcc, 0x9988};

    unsigned z28_expected[] =
       {0x77665544, 0xffeeddcc, 0xbbaa9988, 0x1f1f1f1f, 0x1f1f1f1f};

    uint64_t z29_expected[] = {0x1f1f1f1f1f1f1f1f, 0xffeeddccbbaa9988};
    // clang-format on

    unsigned vl = config->sve_vl_in_bits();
    for (size_t i = 0; i < ArrayLength(index_b); i++) {
      int lane = static_cast<int>(ArrayLength(index_b) - i - 1);
      if (!core.HasSVELane(z26.VnB(), lane)) break;
      uint64_t expected = (vl > (index_b[i] * kBRegSize)) ? z26_expected[i] : 0;
      ASSERT_EQUAL_SVE_LANE(expected, z26.VnB(), lane);
    }

    for (size_t i = 0; i < ArrayLength(index_h); i++) {
      int lane = static_cast<int>(ArrayLength(index_h) - i - 1);
      if (!core.HasSVELane(z27.VnH(), lane)) break;
      uint64_t expected = (vl > (index_h[i] * kHRegSize)) ? z27_expected[i] : 0;
      ASSERT_EQUAL_SVE_LANE(expected, z27.VnH(), lane);
    }

    for (size_t i = 0; i < ArrayLength(index_s); i++) {
      int lane = static_cast<int>(ArrayLength(index_s) - i - 1);
      if (!core.HasSVELane(z28.VnS(), lane)) break;
      uint64_t expected = (vl > (index_s[i] * kSRegSize)) ? z28_expected[i] : 0;
      ASSERT_EQUAL_SVE_LANE(expected, z28.VnS(), lane);
    }

    for (size_t i = 0; i < ArrayLength(index_d); i++) {
      int lane = static_cast<int>(ArrayLength(index_d) - i - 1);
      if (!core.HasSVELane(z29.VnD(), lane)) break;
      uint64_t expected = (vl > (index_d[i] * kDRegSize)) ? z29_expected[i] : 0;
      ASSERT_EQUAL_SVE_LANE(expected, z29.VnD(), lane);
    }
  }
}

TEST_SVE(ldr_str_z_bi) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int vl = config->sve_vl_in_bytes();

  // The immediate can address [-256, 255] times the VL, so allocate enough
  // space to exceed that in both directions.
  int data_size = vl * 1024;

  uint8_t* data = new uint8_t[data_size];
  memset(data, 0, data_size);

  // Set the base half-way through the buffer so we can use negative indices.
  __ Mov(x0, reinterpret_cast<uintptr_t>(&data[data_size / 2]));

  __ Index(z1.VnB(), 1, 3);
  __ Index(z2.VnB(), 2, 5);
  __ Index(z3.VnB(), 3, 7);
  __ Index(z4.VnB(), 4, 11);
  __ Index(z5.VnB(), 5, 13);
  __ Index(z6.VnB(), 6, 2);
  __ Index(z7.VnB(), 7, 3);
  __ Index(z8.VnB(), 8, 5);
  __ Index(z9.VnB(), 9, 7);

  // Encodable cases.
  __ Str(z1, SVEMemOperand(x0));
  __ Str(z2, SVEMemOperand(x0, 2, SVE_MUL_VL));
  __ Str(z3, SVEMemOperand(x0, -3, SVE_MUL_VL));
  __ Str(z4, SVEMemOperand(x0, 255, SVE_MUL_VL));
  __ Str(z5, SVEMemOperand(x0, -256, SVE_MUL_VL));

  // Cases that fall back on `CalculateSVEAddress`.
  __ Str(z6, SVEMemOperand(x0, 6 * vl));
  __ Str(z7, SVEMemOperand(x0, -7 * vl));
  __ Str(z8, SVEMemOperand(x0, 314, SVE_MUL_VL));
  __ Str(z9, SVEMemOperand(x0, -314, SVE_MUL_VL));

  // Corresponding loads.
  __ Ldr(z11, SVEMemOperand(x0, xzr));  // Test xzr operand.
  __ Ldr(z12, SVEMemOperand(x0, 2, SVE_MUL_VL));
  __ Ldr(z13, SVEMemOperand(x0, -3, SVE_MUL_VL));
  __ Ldr(z14, SVEMemOperand(x0, 255, SVE_MUL_VL));
  __ Ldr(z15, SVEMemOperand(x0, -256, SVE_MUL_VL));

  __ Ldr(z16, SVEMemOperand(x0, 6 * vl));
  __ Ldr(z17, SVEMemOperand(x0, -7 * vl));
  __ Ldr(z18, SVEMemOperand(x0, 314, SVE_MUL_VL));
  __ Ldr(z19, SVEMemOperand(x0, -314, SVE_MUL_VL));

  END();

  if (CAN_RUN()) {
    RUN();

    uint8_t* expected = new uint8_t[data_size];
    memset(expected, 0, data_size);
    uint8_t* middle = &expected[data_size / 2];

    for (int i = 0; i < vl; i++) {
      middle[i] = (1 + (3 * i)) & 0xff;                 // z1
      middle[(2 * vl) + i] = (2 + (5 * i)) & 0xff;      // z2
      middle[(-3 * vl) + i] = (3 + (7 * i)) & 0xff;     // z3
      middle[(255 * vl) + i] = (4 + (11 * i)) & 0xff;   // z4
      middle[(-256 * vl) + i] = (5 + (13 * i)) & 0xff;  // z5
      middle[(6 * vl) + i] = (6 + (2 * i)) & 0xff;      // z6
      middle[(-7 * vl) + i] = (7 + (3 * i)) & 0xff;     // z7
      middle[(314 * vl) + i] = (8 + (5 * i)) & 0xff;    // z8
      middle[(-314 * vl) + i] = (9 + (7 * i)) & 0xff;   // z9
    }

    ASSERT_EQUAL_MEMORY(expected, data, data_size, middle - expected);

    ASSERT_EQUAL_SVE(z1, z11);
    ASSERT_EQUAL_SVE(z2, z12);
    ASSERT_EQUAL_SVE(z3, z13);
    ASSERT_EQUAL_SVE(z4, z14);
    ASSERT_EQUAL_SVE(z5, z15);
    ASSERT_EQUAL_SVE(z6, z16);
    ASSERT_EQUAL_SVE(z7, z17);
    ASSERT_EQUAL_SVE(z8, z18);
    ASSERT_EQUAL_SVE(z9, z19);

    delete[] expected;
  }
  delete[] data;
}

TEST_SVE(ldr_str_p_bi) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int vl = config->sve_vl_in_bytes();
  VIXL_ASSERT((vl % kZRegBitsPerPRegBit) == 0);
  int pl = vl / kZRegBitsPerPRegBit;

  // The immediate can address [-256, 255] times the PL, so allocate enough
  // space to exceed that in both directions.
  int data_size = pl * 1024;

  uint8_t* data = new uint8_t[data_size];
  memset(data, 0, data_size);

  // Set the base half-way through the buffer so we can use negative indices.
  __ Mov(x0, reinterpret_cast<uintptr_t>(&data[data_size / 2]));

  uint64_t pattern[4] = {0x1010101011101111,
                         0x0010111011000101,
                         0x1001101110010110,
                         0x1010110101100011};
  for (int i = 8; i <= 15; i++) {
    // Initialise p8-p15 with a conveniently-recognisable, non-zero pattern.
    Initialise(&masm,
               PRegister(i),
               pattern[3] * i,
               pattern[2] * i,
               pattern[1] * i,
               pattern[0] * i);
  }

  // Encodable cases.
  __ Str(p8, SVEMemOperand(x0));
  __ Str(p9, SVEMemOperand(x0, 2, SVE_MUL_VL));
  __ Str(p10, SVEMemOperand(x0, -3, SVE_MUL_VL));
  __ Str(p11, SVEMemOperand(x0, 255, SVE_MUL_VL));

  // Cases that fall back on `CalculateSVEAddress`.
  __ Str(p12, SVEMemOperand(x0, 6 * pl));
  __ Str(p13, SVEMemOperand(x0, -7 * pl));
  __ Str(p14, SVEMemOperand(x0, 314, SVE_MUL_VL));
  __ Str(p15, SVEMemOperand(x0, -314, SVE_MUL_VL));

  // Corresponding loads.
  __ Ldr(p0, SVEMemOperand(x0));
  __ Ldr(p1, SVEMemOperand(x0, 2, SVE_MUL_VL));
  __ Ldr(p2, SVEMemOperand(x0, -3, SVE_MUL_VL));
  __ Ldr(p3, SVEMemOperand(x0, 255, SVE_MUL_VL));

  __ Ldr(p4, SVEMemOperand(x0, 6 * pl));
  __ Ldr(p5, SVEMemOperand(x0, -7 * pl));
  __ Ldr(p6, SVEMemOperand(x0, 314, SVE_MUL_VL));
  __ Ldr(p7, SVEMemOperand(x0, -314, SVE_MUL_VL));

  END();

  if (CAN_RUN()) {
    RUN();

    uint8_t* expected = new uint8_t[data_size];
    memset(expected, 0, data_size);
    uint8_t* middle = &expected[data_size / 2];

    for (int i = 0; i < pl; i++) {
      int bit_index = (i % sizeof(pattern[0])) * kBitsPerByte;
      size_t index = i / sizeof(pattern[0]);
      VIXL_ASSERT(index < ArrayLength(pattern));
      uint64_t byte = (pattern[index] >> bit_index) & 0xff;
      // Each byte of `pattern` can be multiplied by 15 without carry.
      VIXL_ASSERT((byte * 15) <= 0xff);

      middle[i] = byte * 8;                 // p8
      middle[(2 * pl) + i] = byte * 9;      // p9
      middle[(-3 * pl) + i] = byte * 10;    // p10
      middle[(255 * pl) + i] = byte * 11;   // p11
      middle[(6 * pl) + i] = byte * 12;     // p12
      middle[(-7 * pl) + i] = byte * 13;    // p13
      middle[(314 * pl) + i] = byte * 14;   // p14
      middle[(-314 * pl) + i] = byte * 15;  // p15
    }

    ASSERT_EQUAL_MEMORY(expected, data, data_size, middle - expected);

    ASSERT_EQUAL_SVE(p0, p8);
    ASSERT_EQUAL_SVE(p1, p9);
    ASSERT_EQUAL_SVE(p2, p10);
    ASSERT_EQUAL_SVE(p3, p11);
    ASSERT_EQUAL_SVE(p4, p12);
    ASSERT_EQUAL_SVE(p5, p13);
    ASSERT_EQUAL_SVE(p6, p14);
    ASSERT_EQUAL_SVE(p7, p15);

    delete[] expected;
  }
  delete[] data;
}

template <typename T>
static void MemoryWrite(uint8_t* base, int64_t offset, int64_t index, T data) {
  memcpy(base + offset + (index * sizeof(data)), &data, sizeof(data));
}

TEST_SVE(sve_ld1_st1_contiguous) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int vl = config->sve_vl_in_bytes();

  // The immediate can address [-8, 7] times the VL, so allocate enough space to
  // exceed that in both directions.
  int data_size = vl * 128;

  uint8_t* data = new uint8_t[data_size];
  memset(data, 0, data_size);

  // Set the base half-way through the buffer so we can use negative indeces.
  __ Mov(x0, reinterpret_cast<uintptr_t>(&data[data_size / 2]));

  // Encodable scalar-plus-immediate cases.
  __ Index(z1.VnB(), 1, -3);
  __ Ptrue(p1.VnB());
  __ St1b(z1.VnB(), p1, SVEMemOperand(x0));

  __ Index(z2.VnH(), -2, 5);
  __ Ptrue(p2.VnH(), SVE_MUL3);
  __ St1b(z2.VnH(), p2, SVEMemOperand(x0, 7, SVE_MUL_VL));

  __ Index(z3.VnS(), 3, -7);
  __ Ptrue(p3.VnS(), SVE_POW2);
  __ St1h(z3.VnS(), p3, SVEMemOperand(x0, -8, SVE_MUL_VL));

  // Encodable scalar-plus-scalar cases.
  __ Index(z4.VnD(), -4, 11);
  __ Ptrue(p4.VnD(), SVE_VL3);
  __ Addvl(x1, x0, 8);  // Try not to overlap with VL-dependent cases.
  __ Mov(x2, 17);
  __ St1b(z4.VnD(), p4, SVEMemOperand(x1, x2));

  __ Index(z5.VnD(), 6, -2);
  __ Ptrue(p5.VnD(), SVE_VL16);
  __ Addvl(x3, x0, 10);  // Try not to overlap with VL-dependent cases.
  __ Mov(x4, 6);
  __ St1d(z5.VnD(), p5, SVEMemOperand(x3, x4, LSL, 3));

  // Unencodable cases fall back on `CalculateSVEAddress`.
  __ Index(z6.VnS(), -7, 3);
  // Setting SVE_ALL on B lanes checks that the Simulator ignores irrelevant
  // predicate bits when handling larger lanes.
  __ Ptrue(p6.VnB(), SVE_ALL);
  __ St1w(z6.VnS(), p6, SVEMemOperand(x0, 42, SVE_MUL_VL));

  __ Index(z7.VnD(), 32, -11);
  __ Ptrue(p7.VnD(), SVE_MUL4);
  __ St1w(z7.VnD(), p7, SVEMemOperand(x0, 22, SVE_MUL_VL));

  // Corresponding loads.
  __ Ld1b(z8.VnB(), p1.Zeroing(), SVEMemOperand(x0));
  __ Ld1b(z9.VnH(), p2.Zeroing(), SVEMemOperand(x0, 7, SVE_MUL_VL));
  __ Ld1h(z10.VnS(), p3.Zeroing(), SVEMemOperand(x0, -8, SVE_MUL_VL));
  __ Ld1b(z11.VnD(), p4.Zeroing(), SVEMemOperand(x1, x2));
  __ Ld1d(z12.VnD(), p5.Zeroing(), SVEMemOperand(x3, x4, LSL, 3));
  __ Ld1w(z13.VnS(), p6.Zeroing(), SVEMemOperand(x0, 42, SVE_MUL_VL));

  __ Ld1sb(z14.VnH(), p2.Zeroing(), SVEMemOperand(x0, 7, SVE_MUL_VL));
  __ Ld1sh(z15.VnS(), p3.Zeroing(), SVEMemOperand(x0, -8, SVE_MUL_VL));
  __ Ld1sb(z16.VnD(), p4.Zeroing(), SVEMemOperand(x1, x2));
  __ Ld1sw(z17.VnD(), p7.Zeroing(), SVEMemOperand(x0, 22, SVE_MUL_VL));

  // We can test ld1 by comparing the value loaded with the value stored. In
  // most cases, there are two complications:
  //  - Loads have zeroing predication, so we have to clear the inactive
  //    elements on our reference.
  //  - We have to replicate any sign- or zero-extension.

  // Ld1b(z8.VnB(), ...)
  __ Dup(z18.VnB(), 0);
  __ Mov(z18.VnB(), p1.Merging(), z1.VnB());

  // Ld1b(z9.VnH(), ...)
  __ Dup(z19.VnH(), 0);
  __ Uxtb(z19.VnH(), p2.Merging(), z2.VnH());

  // Ld1h(z10.VnS(), ...)
  __ Dup(z20.VnS(), 0);
  __ Uxth(z20.VnS(), p3.Merging(), z3.VnS());

  // Ld1b(z11.VnD(), ...)
  __ Dup(z21.VnD(), 0);
  __ Uxtb(z21.VnD(), p4.Merging(), z4.VnD());

  // Ld1d(z12.VnD(), ...)
  __ Dup(z22.VnD(), 0);
  __ Mov(z22.VnD(), p5.Merging(), z5.VnD());

  // Ld1w(z13.VnS(), ...)
  __ Dup(z23.VnS(), 0);
  __ Mov(z23.VnS(), p6.Merging(), z6.VnS());

  // Ld1sb(z14.VnH(), ...)
  __ Dup(z24.VnH(), 0);
  __ Sxtb(z24.VnH(), p2.Merging(), z2.VnH());

  // Ld1sh(z15.VnS(), ...)
  __ Dup(z25.VnS(), 0);
  __ Sxth(z25.VnS(), p3.Merging(), z3.VnS());

  // Ld1sb(z16.VnD(), ...)
  __ Dup(z26.VnD(), 0);
  __ Sxtb(z26.VnD(), p4.Merging(), z4.VnD());

  // Ld1sw(z17.VnD(), ...)
  __ Dup(z27.VnD(), 0);
  __ Sxtw(z27.VnD(), p7.Merging(), z7.VnD());

  END();

  if (CAN_RUN()) {
    RUN();

    uint8_t* expected = new uint8_t[data_size];
    memset(expected, 0, data_size);
    uint8_t* middle = &expected[data_size / 2];

    int vl_b = vl / kBRegSizeInBytes;
    int vl_h = vl / kHRegSizeInBytes;
    int vl_s = vl / kSRegSizeInBytes;
    int vl_d = vl / kDRegSizeInBytes;

    // Encodable cases.

    // st1b { z1.b }, SVE_ALL
    for (int i = 0; i < vl_b; i++) {
      MemoryWrite(middle, 0, i, static_cast<uint8_t>(1 - (3 * i)));
    }

    // st1b { z2.h }, SVE_MUL3
    int vl_h_mul3 = vl_h - (vl_h % 3);
    for (int i = 0; i < vl_h_mul3; i++) {
      int64_t offset = 7 * static_cast<int>(vl / (kHRegSize / kBRegSize));
      MemoryWrite(middle, offset, i, static_cast<uint8_t>(-2 + (5 * i)));
    }

    // st1h { z3.s }, SVE_POW2
    int vl_s_pow2 = 1 << HighestSetBitPosition(vl_s);
    for (int i = 0; i < vl_s_pow2; i++) {
      int64_t offset = -8 * static_cast<int>(vl / (kSRegSize / kHRegSize));
      MemoryWrite(middle, offset, i, static_cast<uint16_t>(3 - (7 * i)));
    }

    // st1b { z4.d }, SVE_VL3
    if (vl_d >= 3) {
      for (int i = 0; i < 3; i++) {
        MemoryWrite(middle,
                    (8 * vl) + 17,
                    i,
                    static_cast<uint8_t>(-4 + (11 * i)));
      }
    }

    // st1d { z5.d }, SVE_VL16
    if (vl_d >= 16) {
      for (int i = 0; i < 16; i++) {
        MemoryWrite(middle,
                    (10 * vl) + (6 * kDRegSizeInBytes),
                    i,
                    static_cast<uint64_t>(6 - (2 * i)));
      }
    }

    // Unencodable cases.

    // st1w { z6.s }, SVE_ALL
    for (int i = 0; i < vl_s; i++) {
      MemoryWrite(middle, 42 * vl, i, static_cast<uint32_t>(-7 + (3 * i)));
    }

    // st1w { z7.d }, SVE_MUL4
    int vl_d_mul4 = vl_d - (vl_d % 4);
    for (int i = 0; i < vl_d_mul4; i++) {
      int64_t offset = 22 * static_cast<int>(vl / (kDRegSize / kWRegSize));
      MemoryWrite(middle, offset, i, static_cast<uint32_t>(32 + (-11 * i)));
    }

    ASSERT_EQUAL_MEMORY(expected, data, data_size, middle - expected);

    // Check that we loaded back the expected values.

    ASSERT_EQUAL_SVE(z18, z8);
    ASSERT_EQUAL_SVE(z19, z9);
    ASSERT_EQUAL_SVE(z20, z10);
    ASSERT_EQUAL_SVE(z21, z11);
    ASSERT_EQUAL_SVE(z22, z12);
    ASSERT_EQUAL_SVE(z23, z13);
    ASSERT_EQUAL_SVE(z24, z14);
    ASSERT_EQUAL_SVE(z25, z15);
    ASSERT_EQUAL_SVE(z26, z16);
    ASSERT_EQUAL_SVE(z27, z17);

    delete[] expected;
  }
  delete[] data;
}

TEST_SVE(sve_ld2_st2_scalar_plus_imm) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int vl = config->sve_vl_in_bytes();

  // The immediate can address [-16, 14] times the VL, so allocate enough space
  // to exceed that in both directions.
  int data_size = vl * 128;

  uint8_t* data = new uint8_t[data_size];
  memset(data, 0, data_size);

  // Set the base half-way through the buffer so we can use negative indeces.
  __ Mov(x0, reinterpret_cast<uintptr_t>(&data[data_size / 2]));

  __ Index(z14.VnB(), 1, -3);
  __ Index(z15.VnB(), 2, -3);
  __ Ptrue(p0.VnB());
  __ St2b(z14.VnB(), z15.VnB(), p0, SVEMemOperand(x0));

  __ Index(z16.VnH(), -2, 5);
  __ Index(z17.VnH(), -3, 5);
  __ Ptrue(p1.VnH(), SVE_MUL3);
  __ St2h(z16.VnH(), z17.VnH(), p1, SVEMemOperand(x0, 8, SVE_MUL_VL));

  // Wrap around from z31 to z0.
  __ Index(z31.VnS(), 3, -7);
  __ Index(z0.VnS(), 4, -7);
  __ Ptrue(p2.VnS(), SVE_POW2);
  __ St2w(z31.VnS(), z0.VnS(), p2, SVEMemOperand(x0, -12, SVE_MUL_VL));

  __ Index(z18.VnD(), -7, 3);
  __ Index(z19.VnD(), -8, 3);
  // Sparse predication, including some irrelevant bits (0xe). To make the
  // results easy to check, activate each lane <n> where n is a multiple of 5.
  Initialise(&masm,
             p3,
             0xeee10000000001ee,
             0xeeeeeee100000000,
             0x01eeeeeeeee10000,
             0x000001eeeeeeeee1);
  __ St2d(z18.VnD(), z19.VnD(), p3, SVEMemOperand(x0, 14, SVE_MUL_VL));

  // We can test ld2 by comparing the values loaded with the values stored.
  // There are two complications:
  //  - Loads have zeroing predication, so we have to clear the inactive
  //    elements on our reference.
  //  - We want to test both loads and stores that span { z31, z0 }, so we have
  //    to move some values around.
  //
  // Registers z4-z11 will hold as-stored values (with inactive elements
  // cleared). Registers z20-z27 will hold the values that were loaded.

  // Ld2b(z14.VnB(), z15.VnB(), ...)
  __ Dup(z4.VnB(), 0);
  __ Dup(z5.VnB(), 0);
  __ Mov(z4.VnB(), p0.Merging(), z14.VnB());
  __ Mov(z5.VnB(), p0.Merging(), z15.VnB());

  // Ld2h(z16.VnH(), z17.VnH(), ...)
  __ Dup(z6.VnH(), 0);
  __ Dup(z7.VnH(), 0);
  __ Mov(z6.VnH(), p1.Merging(), z16.VnH());
  __ Mov(z7.VnH(), p1.Merging(), z17.VnH());

  // Ld2w(z31.VnS(), z0.VnS(), ...)
  __ Dup(z8.VnS(), 0);
  __ Dup(z9.VnS(), 0);
  __ Mov(z8.VnS(), p2.Merging(), z31.VnS());
  __ Mov(z9.VnS(), p2.Merging(), z0.VnS());

  // Ld2d(z18.VnD(), z19.VnD(), ...)
  __ Dup(z10.VnD(), 0);
  __ Dup(z11.VnD(), 0);
  __ Mov(z10.VnD(), p3.Merging(), z18.VnD());
  __ Mov(z11.VnD(), p3.Merging(), z19.VnD());

  // Wrap around from z31 to z0, moving the results elsewhere to avoid overlap.
  __ Ld2b(z31.VnB(), z0.VnB(), p0.Zeroing(), SVEMemOperand(x0));
  __ Mov(z20, z31);
  __ Mov(z21, z0);

  __ Ld2h(z22.VnH(), z23.VnH(), p1.Zeroing(), SVEMemOperand(x0, 8, SVE_MUL_VL));
  __ Ld2w(z24.VnS(),
          z25.VnS(),
          p2.Zeroing(),
          SVEMemOperand(x0, -12, SVE_MUL_VL));
  __ Ld2d(z26.VnD(),
          z27.VnD(),
          p3.Zeroing(),
          SVEMemOperand(x0, 14, SVE_MUL_VL));

  END();

  if (CAN_RUN()) {
    RUN();

    uint8_t* expected = new uint8_t[data_size];
    memset(expected, 0, data_size);
    uint8_t* middle = &expected[data_size / 2];

    int vl_b = vl / kBRegSizeInBytes;
    int vl_h = vl / kHRegSizeInBytes;
    int vl_s = vl / kSRegSizeInBytes;
    int vl_d = vl / kDRegSizeInBytes;

    int reg_count = 2;

    // st2b { z14.b, z15.b }, SVE_ALL
    for (int i = 0; i < vl_b; i++) {
      uint8_t lane0 = 1 - (3 * i);
      uint8_t lane1 = 2 - (3 * i);
      MemoryWrite(middle, 0, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, 0, (i * reg_count) + 1, lane1);
    }

    // st2h { z16.h, z17.h }, SVE_MUL3
    int vl_h_mul3 = vl_h - (vl_h % 3);
    for (int i = 0; i < vl_h_mul3; i++) {
      int64_t offset = 8 * vl;
      uint16_t lane0 = -2 + (5 * i);
      uint16_t lane1 = -3 + (5 * i);
      MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
    }

    // st2w { z31.s, z0.s }, SVE_POW2
    int vl_s_pow2 = 1 << HighestSetBitPosition(vl_s);
    for (int i = 0; i < vl_s_pow2; i++) {
      int64_t offset = -12 * vl;
      uint32_t lane0 = 3 - (7 * i);
      uint32_t lane1 = 4 - (7 * i);
      MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
    }

    // st2d { z18.d, z19.d }, ((i % 5) == 0)
    for (int i = 0; i < vl_d; i++) {
      if ((i % 5) == 0) {
        int64_t offset = 14 * vl;
        uint64_t lane0 = -7 + (3 * i);
        uint64_t lane1 = -8 + (3 * i);
        MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
        MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
      }
    }

    ASSERT_EQUAL_MEMORY(expected, data, data_size, middle - expected);

    // Check that we loaded back the expected values.

    // st2b/ld2b
    ASSERT_EQUAL_SVE(z4, z20);
    ASSERT_EQUAL_SVE(z5, z21);

    // st2h/ld2h
    ASSERT_EQUAL_SVE(z6, z22);
    ASSERT_EQUAL_SVE(z7, z23);

    // st2w/ld2w
    ASSERT_EQUAL_SVE(z8, z24);
    ASSERT_EQUAL_SVE(z9, z25);

    // st2d/ld2d
    ASSERT_EQUAL_SVE(z10, z26);
    ASSERT_EQUAL_SVE(z11, z27);

    delete[] expected;
  }
  delete[] data;
}

TEST_SVE(sve_ld2_st2_scalar_plus_scalar) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int vl = config->sve_vl_in_bytes();

  // Allocate plenty of space to enable indexing in both directions.
  int data_size = vl * 128;

  uint8_t* data = new uint8_t[data_size];
  memset(data, 0, data_size);

  // Set the base half-way through the buffer so we can use negative indeces.
  __ Mov(x0, reinterpret_cast<uintptr_t>(&data[data_size / 2]));

  __ Index(z10.VnB(), -4, 11);
  __ Index(z11.VnB(), -5, 11);
  __ Ptrue(p7.VnB(), SVE_MUL4);
  __ Mov(x1, 0);
  __ St2b(z10.VnB(), z11.VnB(), p7, SVEMemOperand(x0, x1));

  __ Index(z12.VnH(), 6, -2);
  __ Index(z13.VnH(), 7, -2);
  __ Ptrue(p6.VnH(), SVE_VL16);
  __ Rdvl(x2, 3);  // Make offsets VL-dependent so we can avoid overlap.
  __ St2h(z12.VnH(), z13.VnH(), p6, SVEMemOperand(x0, x2, LSL, 1));

  __ Index(z14.VnS(), -7, 3);
  __ Index(z15.VnS(), -8, 3);
  // Sparse predication, including some irrelevant bits (0xe). To make the
  // results easy to check, activate each lane <n> where n is a multiple of 5.
  Initialise(&masm,
             p5,
             0xeee1000010000100,
             0x001eeee100001000,
             0x0100001eeee10000,
             0x10000100001eeee1);
  __ Rdvl(x3, -3);
  __ St2w(z14.VnS(), z15.VnS(), p5, SVEMemOperand(x0, x3, LSL, 2));

  // Wrap around from z31 to z0.
  __ Index(z31.VnD(), 32, -11);
  __ Index(z0.VnD(), 33, -11);
  __ Ptrue(p4.VnD(), SVE_MUL3);
  __ Rdvl(x4, 1);
  __ St2d(z31.VnD(), z0.VnD(), p4, SVEMemOperand(x0, x4, LSL, 3));

  // We can test ld2 by comparing the values loaded with the values stored.
  // There are two complications:
  //  - Loads have zeroing predication, so we have to clear the inactive
  //    elements on our reference.
  //  - We want to test both loads and stores that span { z31, z0 }, so we have
  //    to move some values around.
  //
  // Registers z4-z11 will hold as-stored values (with inactive elements
  // cleared). Registers z20-z27 will hold the values that were loaded.

  // Ld2b(z20.VnB(), z21.VnB(), ...)
  __ Dup(z4.VnB(), 0);
  __ Dup(z5.VnB(), 0);
  __ Mov(z4.VnB(), p7.Merging(), z10.VnB());
  __ Mov(z5.VnB(), p7.Merging(), z11.VnB());

  // Ld2h(z22.VnH(), z23.VnH(), ...)
  __ Dup(z6.VnH(), 0);
  __ Dup(z7.VnH(), 0);
  __ Mov(z6.VnH(), p6.Merging(), z12.VnH());
  __ Mov(z7.VnH(), p6.Merging(), z13.VnH());

  // Ld2w(z24.VnS(), z25.VnS(), ...)
  __ Dup(z8.VnS(), 0);
  __ Dup(z9.VnS(), 0);
  __ Mov(z8.VnS(), p5.Merging(), z14.VnS());
  __ Mov(z9.VnS(), p5.Merging(), z15.VnS());

  // Ld2d(z31.VnD(), z0.VnD(), ...)
  __ Dup(z10.VnD(), 0);
  __ Dup(z11.VnD(), 0);
  __ Mov(z10.VnD(), p4.Merging(), z31.VnD());
  __ Mov(z11.VnD(), p4.Merging(), z0.VnD());

  // Wrap around from z31 to z0, moving the results elsewhere to avoid overlap.
  __ Ld2b(z31.VnB(), z0.VnB(), p7.Zeroing(), SVEMemOperand(x0, x1));
  __ Mov(z20, z31);
  __ Mov(z21, z0);

  __ Ld2h(z22.VnH(), z23.VnH(), p6.Zeroing(), SVEMemOperand(x0, x2, LSL, 1));
  __ Ld2w(z24.VnS(), z25.VnS(), p5.Zeroing(), SVEMemOperand(x0, x3, LSL, 2));
  __ Ld2d(z26.VnD(), z27.VnD(), p4.Zeroing(), SVEMemOperand(x0, x4, LSL, 3));

  END();

  if (CAN_RUN()) {
    RUN();

    uint8_t* expected = new uint8_t[data_size];
    memset(expected, 0, data_size);
    uint8_t* middle = &expected[data_size / 2];

    int vl_b = vl / kBRegSizeInBytes;
    int vl_h = vl / kHRegSizeInBytes;
    int vl_s = vl / kSRegSizeInBytes;
    int vl_d = vl / kDRegSizeInBytes;

    int reg_count = 2;

    // st2b { z10.b, z11.b }, SVE_MUL4
    int vl_b_mul4 = vl_b - (vl_b % 4);
    for (int i = 0; i < vl_b_mul4; i++) {
      uint8_t lane0 = -4 + (11 * i);
      uint8_t lane1 = -5 + (11 * i);
      MemoryWrite(middle, 0, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, 0, (i * reg_count) + 1, lane1);
    }

    // st2h { z12.h, z13.h }, SVE_VL16
    if (vl_h >= 16) {
      for (int i = 0; i < 16; i++) {
        int64_t offset = (3 << kHRegSizeInBytesLog2) * vl;
        uint16_t lane0 = 6 - (2 * i);
        uint16_t lane1 = 7 - (2 * i);
        MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
        MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
      }
    }

    // st2w { z14.s, z15.s }, ((i % 5) == 0)
    for (int i = 0; i < vl_s; i++) {
      if ((i % 5) == 0) {
        int64_t offset = -(3 << kSRegSizeInBytesLog2) * vl;
        uint32_t lane0 = -7 + (3 * i);
        uint32_t lane1 = -8 + (3 * i);
        MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
        MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
      }
    }

    // st2d { z31.b, z0.b }, SVE_MUL3
    int vl_d_mul3 = vl_d - (vl_d % 3);
    for (int i = 0; i < vl_d_mul3; i++) {
      int64_t offset = (1 << kDRegSizeInBytesLog2) * vl;
      uint64_t lane0 = 32 - (11 * i);
      uint64_t lane1 = 33 - (11 * i);
      MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
    }

    ASSERT_EQUAL_MEMORY(expected, data, data_size, middle - expected);

    // Check that we loaded back the expected values.

    // st2b/ld2b
    ASSERT_EQUAL_SVE(z4, z20);
    ASSERT_EQUAL_SVE(z5, z21);

    // st2h/ld2h
    ASSERT_EQUAL_SVE(z6, z22);
    ASSERT_EQUAL_SVE(z7, z23);

    // st2w/ld2w
    ASSERT_EQUAL_SVE(z8, z24);
    ASSERT_EQUAL_SVE(z9, z25);

    // st2d/ld2d
    ASSERT_EQUAL_SVE(z10, z26);
    ASSERT_EQUAL_SVE(z11, z27);

    delete[] expected;
  }
  delete[] data;
}

TEST_SVE(sve_ld3_st3_scalar_plus_imm) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int vl = config->sve_vl_in_bytes();

  // The immediate can address [-24, 21] times the VL, so allocate enough space
  // to exceed that in both directions.
  int data_size = vl * 128;

  uint8_t* data = new uint8_t[data_size];
  memset(data, 0, data_size);

  // Set the base half-way through the buffer so we can use negative indeces.
  __ Mov(x0, reinterpret_cast<uintptr_t>(&data[data_size / 2]));

  // We can test ld3 by comparing the values loaded with the values stored.
  // There are two complications:
  //  - Loads have zeroing predication, so we have to clear the inactive
  //    elements on our reference.
  //  - We want to test both loads and stores that span { z31, z0 }, so we have
  //    to move some values around.
  //
  // Registers z4-z15 will hold as-stored values (with inactive elements
  // cleared). Registers z16-z27 will hold the values that were loaded.

  __ Index(z10.VnB(), 1, -3);
  __ Index(z11.VnB(), 2, -3);
  __ Index(z12.VnB(), 3, -3);
  __ Ptrue(p0.VnB());
  __ St3b(z10.VnB(), z11.VnB(), z12.VnB(), p0, SVEMemOperand(x0));
  // Save the stored values for ld3 tests.
  __ Dup(z4.VnB(), 0);
  __ Dup(z5.VnB(), 0);
  __ Dup(z6.VnB(), 0);
  __ Mov(z4.VnB(), p0.Merging(), z10.VnB());
  __ Mov(z5.VnB(), p0.Merging(), z11.VnB());
  __ Mov(z6.VnB(), p0.Merging(), z12.VnB());

  // Wrap around from z31 to z0.
  __ Index(z31.VnH(), -2, 5);
  __ Index(z0.VnH(), -3, 5);
  __ Index(z1.VnH(), -4, 5);
  __ Ptrue(p1.VnH(), SVE_MUL3);
  __ St3h(z31.VnH(), z0.VnH(), z1.VnH(), p1, SVEMemOperand(x0, 9, SVE_MUL_VL));
  // Save the stored values for ld3 tests.
  __ Dup(z7.VnH(), 0);
  __ Dup(z8.VnH(), 0);
  __ Dup(z9.VnH(), 0);
  __ Mov(z7.VnH(), p1.Merging(), z31.VnH());
  __ Mov(z8.VnH(), p1.Merging(), z0.VnH());
  __ Mov(z9.VnH(), p1.Merging(), z1.VnH());

  __ Index(z30.VnS(), 3, -7);
  __ Index(z31.VnS(), 4, -7);
  __ Index(z0.VnS(), 5, -7);
  __ Ptrue(p2.VnS(), SVE_POW2);
  __ St3w(z30.VnS(),
          z31.VnS(),
          z0.VnS(),
          p2,
          SVEMemOperand(x0, -12, SVE_MUL_VL));
  // Save the stored values for ld3 tests.
  __ Dup(z10.VnS(), 0);
  __ Dup(z11.VnS(), 0);
  __ Dup(z12.VnS(), 0);
  __ Mov(z10.VnS(), p2.Merging(), z30.VnS());
  __ Mov(z11.VnS(), p2.Merging(), z31.VnS());
  __ Mov(z12.VnS(), p2.Merging(), z0.VnS());

  __ Index(z0.VnD(), -7, 3);
  __ Index(z1.VnD(), -8, 3);
  __ Index(z2.VnD(), -9, 3);
  // Sparse predication, including some irrelevant bits (0xee). To make the
  // results easy to check, activate each lane <n> where n is a multiple of 5.
  Initialise(&masm,
             p3,
             0xeee10000000001ee,
             0xeeeeeee100000000,
             0x01eeeeeeeee10000,
             0x000001eeeeeeeee1);
  __ St3d(z0.VnD(), z1.VnD(), z2.VnD(), p3, SVEMemOperand(x0, 15, SVE_MUL_VL));
  // Save the stored values for ld3 tests.
  __ Dup(z13.VnD(), 0);
  __ Dup(z14.VnD(), 0);
  __ Dup(z15.VnD(), 0);
  __ Mov(z13.VnD(), p3.Merging(), z0.VnD());
  __ Mov(z14.VnD(), p3.Merging(), z1.VnD());
  __ Mov(z15.VnD(), p3.Merging(), z2.VnD());

  // Corresponding loads.
  // Wrap around from z31 to z0, moving the results elsewhere to avoid overlap.
  __ Ld3b(z31.VnB(), z0.VnB(), z1.VnB(), p0.Zeroing(), SVEMemOperand(x0));
  __ Mov(z16, z31);
  __ Mov(z17, z0);
  __ Mov(z18, z1);
  __ Ld3h(z30.VnH(),
          z31.VnH(),
          z0.VnH(),
          p1.Zeroing(),
          SVEMemOperand(x0, 9, SVE_MUL_VL));
  __ Mov(z19, z30);
  __ Mov(z20, z31);
  __ Mov(z21, z0);
  __ Ld3w(z22.VnS(),
          z23.VnS(),
          z24.VnS(),
          p2.Zeroing(),
          SVEMemOperand(x0, -12, SVE_MUL_VL));
  __ Ld3d(z25.VnD(),
          z26.VnD(),
          z27.VnD(),
          p3.Zeroing(),
          SVEMemOperand(x0, 15, SVE_MUL_VL));

  END();

  if (CAN_RUN()) {
    RUN();

    uint8_t* expected = new uint8_t[data_size];
    memset(expected, 0, data_size);
    uint8_t* middle = &expected[data_size / 2];

    int vl_b = vl / kBRegSizeInBytes;
    int vl_h = vl / kHRegSizeInBytes;
    int vl_s = vl / kSRegSizeInBytes;
    int vl_d = vl / kDRegSizeInBytes;

    int reg_count = 3;

    // st3b { z10.b, z11.b, z12.b }, SVE_ALL
    for (int i = 0; i < vl_b; i++) {
      uint8_t lane0 = 1 - (3 * i);
      uint8_t lane1 = 2 - (3 * i);
      uint8_t lane2 = 3 - (3 * i);
      MemoryWrite(middle, 0, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, 0, (i * reg_count) + 1, lane1);
      MemoryWrite(middle, 0, (i * reg_count) + 2, lane2);
    }

    // st3h { z31.h, z0.h, z1.h }, SVE_MUL3
    int vl_h_mul3 = vl_h - (vl_h % 3);
    for (int i = 0; i < vl_h_mul3; i++) {
      int64_t offset = 9 * vl;
      uint16_t lane0 = -2 + (5 * i);
      uint16_t lane1 = -3 + (5 * i);
      uint16_t lane2 = -4 + (5 * i);
      MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
      MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
    }

    // st3w { z30.s, z31.s, z0.s }, SVE_POW2
    int vl_s_pow2 = 1 << HighestSetBitPosition(vl_s);
    for (int i = 0; i < vl_s_pow2; i++) {
      int64_t offset = -12 * vl;
      uint32_t lane0 = 3 - (7 * i);
      uint32_t lane1 = 4 - (7 * i);
      uint32_t lane2 = 5 - (7 * i);
      MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
      MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
    }

    // st3d { z0.d, z1.d, z2.d }, ((i % 5) == 0)
    for (int i = 0; i < vl_d; i++) {
      if ((i % 5) == 0) {
        int64_t offset = 15 * vl;
        uint64_t lane0 = -7 + (3 * i);
        uint64_t lane1 = -8 + (3 * i);
        uint64_t lane2 = -9 + (3 * i);
        MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
        MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
        MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
      }
    }

    ASSERT_EQUAL_MEMORY(expected, data, data_size, middle - expected);

    // Check that we loaded back the expected values.

    // st3b/ld3b
    ASSERT_EQUAL_SVE(z4, z16);
    ASSERT_EQUAL_SVE(z5, z17);
    ASSERT_EQUAL_SVE(z6, z18);

    // st3h/ld3h
    ASSERT_EQUAL_SVE(z7, z19);
    ASSERT_EQUAL_SVE(z8, z20);
    ASSERT_EQUAL_SVE(z9, z21);

    // st3w/ld3w
    ASSERT_EQUAL_SVE(z10, z22);
    ASSERT_EQUAL_SVE(z11, z23);
    ASSERT_EQUAL_SVE(z12, z24);

    // st3d/ld3d
    ASSERT_EQUAL_SVE(z13, z25);
    ASSERT_EQUAL_SVE(z14, z26);
    ASSERT_EQUAL_SVE(z15, z27);

    delete[] expected;
  }
  delete[] data;
}

TEST_SVE(sve_ld3_st3_scalar_plus_scalar) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int vl = config->sve_vl_in_bytes();

  // Allocate plenty of space to enable indexing in both directions.
  int data_size = vl * 128;

  uint8_t* data = new uint8_t[data_size];
  memset(data, 0, data_size);

  // Set the base half-way through the buffer so we can use negative indeces.
  __ Mov(x0, reinterpret_cast<uintptr_t>(&data[data_size / 2]));

  // We can test ld3 by comparing the values loaded with the values stored.
  // There are two complications:
  //  - Loads have zeroing predication, so we have to clear the inactive
  //    elements on our reference.
  //  - We want to test both loads and stores that span { z31, z0 }, so we have
  //    to move some values around.
  //
  // Registers z4-z15 will hold as-stored values (with inactive elements
  // cleared). Registers z16-z27 will hold the values that were loaded.

  __ Index(z10.VnB(), -4, 11);
  __ Index(z11.VnB(), -5, 11);
  __ Index(z12.VnB(), -6, 11);
  __ Ptrue(p7.VnB(), SVE_MUL4);
  __ Rdvl(x1, -1);  // Make offsets VL-dependent so we can avoid overlap.
  __ St3b(z10.VnB(), z11.VnB(), z12.VnB(), p7, SVEMemOperand(x0, x1, LSL, 0));
  // Save the stored values for ld3 tests.
  __ Dup(z4.VnB(), 0);
  __ Dup(z5.VnB(), 0);
  __ Dup(z6.VnB(), 0);
  __ Mov(z4.VnB(), p7.Merging(), z10.VnB());
  __ Mov(z5.VnB(), p7.Merging(), z11.VnB());
  __ Mov(z6.VnB(), p7.Merging(), z12.VnB());

  __ Index(z13.VnH(), 6, -2);
  __ Index(z14.VnH(), 7, -2);
  __ Index(z15.VnH(), 8, -2);
  __ Ptrue(p6.VnH(), SVE_VL16);
  __ Rdvl(x2, 5);  // (5 * vl) << 1 = 10 * vl
  __ St3h(z13.VnH(), z14.VnH(), z15.VnH(), p6, SVEMemOperand(x0, x2, LSL, 1));
  // Save the stored values for ld3 tests.
  __ Dup(z7.VnH(), 0);
  __ Dup(z8.VnH(), 0);
  __ Dup(z9.VnH(), 0);
  __ Mov(z7.VnH(), p6.Merging(), z13.VnH());
  __ Mov(z8.VnH(), p6.Merging(), z14.VnH());
  __ Mov(z9.VnH(), p6.Merging(), z15.VnH());

  // Wrap around from z31 to z0.
  __ Index(z30.VnS(), -7, 3);
  __ Index(z31.VnS(), -8, 3);
  __ Index(z0.VnS(), -9, 3);
  // Sparse predication, including some irrelevant bits (0xe). To make the
  // results easy to check, activate each lane <n> where n is a multiple of 5.
  Initialise(&masm,
             p5,
             0xeee1000010000100,
             0x001eeee100001000,
             0x0100001eeee10000,
             0x10000100001eeee1);
  __ Rdvl(x3, -5);  // -(5 * vl) << 2 = -20 * vl
  __ St3w(z30.VnS(), z31.VnS(), z0.VnS(), p5, SVEMemOperand(x0, x3, LSL, 2));
  // Save the stored values for ld3 tests.
  __ Dup(z10.VnS(), 0);
  __ Dup(z11.VnS(), 0);
  __ Dup(z12.VnS(), 0);
  __ Mov(z10.VnS(), p5.Merging(), z30.VnS());
  __ Mov(z11.VnS(), p5.Merging(), z31.VnS());
  __ Mov(z12.VnS(), p5.Merging(), z0.VnS());

  __ Index(z31.VnD(), 32, -11);
  __ Index(z0.VnD(), 33, -11);
  __ Index(z1.VnD(), 34, -11);
  __ Ptrue(p4.VnD(), SVE_MUL3);
  __ Rdvl(x4, -1);  // -(1 * vl) << 3 = -8 * vl
  __ St3d(z31.VnD(), z0.VnD(), z1.VnD(), p4, SVEMemOperand(x0, x4, LSL, 3));
  // Save the stored values for ld3 tests.
  __ Dup(z13.VnD(), 0);
  __ Dup(z14.VnD(), 0);
  __ Dup(z15.VnD(), 0);
  __ Mov(z13.VnD(), p4.Merging(), z31.VnD());
  __ Mov(z14.VnD(), p4.Merging(), z0.VnD());
  __ Mov(z15.VnD(), p4.Merging(), z1.VnD());

  // Corresponding loads.
  // Wrap around from z31 to z0, moving the results elsewhere to avoid overlap.
  __ Ld3b(z31.VnB(),
          z0.VnB(),
          z1.VnB(),
          p7.Zeroing(),
          SVEMemOperand(x0, x1, LSL, 0));
  __ Mov(z16, z31);
  __ Mov(z17, z0);
  __ Mov(z18, z1);
  __ Ld3h(z30.VnH(),
          z31.VnH(),
          z0.VnH(),
          p6.Zeroing(),
          SVEMemOperand(x0, x2, LSL, 1));
  __ Mov(z19, z30);
  __ Mov(z20, z31);
  __ Mov(z21, z0);
  __ Ld3w(z22.VnS(),
          z23.VnS(),
          z24.VnS(),
          p5.Zeroing(),
          SVEMemOperand(x0, x3, LSL, 2));
  __ Ld3d(z25.VnD(),
          z26.VnD(),
          z27.VnD(),
          p4.Zeroing(),
          SVEMemOperand(x0, x4, LSL, 3));

  END();

  if (CAN_RUN()) {
    RUN();

    uint8_t* expected = new uint8_t[data_size];
    memset(expected, 0, data_size);
    uint8_t* middle = &expected[data_size / 2];

    int vl_b = vl / kBRegSizeInBytes;
    int vl_h = vl / kHRegSizeInBytes;
    int vl_s = vl / kSRegSizeInBytes;
    int vl_d = vl / kDRegSizeInBytes;

    int reg_count = 3;

    // st3b { z10.b, z11.b, z12.b }, SVE_MUL4
    int vl_b_mul4 = vl_b - (vl_b % 4);
    for (int i = 0; i < vl_b_mul4; i++) {
      int64_t offset = -(1 << kBRegSizeInBytesLog2) * vl;
      uint8_t lane0 = -4 + (11 * i);
      uint8_t lane1 = -5 + (11 * i);
      uint8_t lane2 = -6 + (11 * i);
      MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
      MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
    }

    // st3h { z13.h, z14.h, z15.h }, SVE_VL16
    if (vl_h >= 16) {
      for (int i = 0; i < 16; i++) {
        int64_t offset = (5 << kHRegSizeInBytesLog2) * vl;
        uint16_t lane0 = 6 - (2 * i);
        uint16_t lane1 = 7 - (2 * i);
        uint16_t lane2 = 8 - (2 * i);
        MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
        MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
        MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
      }
    }

    // st3w { z30.s, z31.s, z0.s }, ((i % 5) == 0)
    for (int i = 0; i < vl_s; i++) {
      if ((i % 5) == 0) {
        int64_t offset = -(5 << kSRegSizeInBytesLog2) * vl;
        uint32_t lane0 = -7 + (3 * i);
        uint32_t lane1 = -8 + (3 * i);
        uint32_t lane2 = -9 + (3 * i);
        MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
        MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
        MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
      }
    }

    // st3d { z31.d, z0.d, z1.d }, SVE_MUL3
    int vl_d_mul3 = vl_d - (vl_d % 3);
    for (int i = 0; i < vl_d_mul3; i++) {
      int64_t offset = -(1 << kDRegSizeInBytesLog2) * vl;
      uint64_t lane0 = 32 - (11 * i);
      uint64_t lane1 = 33 - (11 * i);
      uint64_t lane2 = 34 - (11 * i);
      MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
      MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
    }

    ASSERT_EQUAL_MEMORY(expected, data, data_size, middle - expected);

    // Check that we loaded back the expected values.

    // st3b/ld3b
    ASSERT_EQUAL_SVE(z4, z16);
    ASSERT_EQUAL_SVE(z5, z17);
    ASSERT_EQUAL_SVE(z6, z18);

    // st3h/ld3h
    ASSERT_EQUAL_SVE(z7, z19);
    ASSERT_EQUAL_SVE(z8, z20);
    ASSERT_EQUAL_SVE(z9, z21);

    // st3w/ld3w
    ASSERT_EQUAL_SVE(z10, z22);
    ASSERT_EQUAL_SVE(z11, z23);
    ASSERT_EQUAL_SVE(z12, z24);

    // st3d/ld3d
    ASSERT_EQUAL_SVE(z13, z25);
    ASSERT_EQUAL_SVE(z14, z26);
    ASSERT_EQUAL_SVE(z15, z27);

    delete[] expected;
  }
  delete[] data;
}

TEST_SVE(sve_ld4_st4_scalar_plus_imm) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int vl = config->sve_vl_in_bytes();

  // The immediate can address [-24, 21] times the VL, so allocate enough space
  // to exceed that in both directions.
  int data_size = vl * 128;

  uint8_t* data = new uint8_t[data_size];
  memset(data, 0, data_size);

  // Set the base half-way through the buffer so we can use negative indeces.
  __ Mov(x0, reinterpret_cast<uintptr_t>(&data[data_size / 2]));

  // We can test ld4 by comparing the values loaded with the values stored.
  // There are two complications:
  //  - Loads have zeroing predication, so we have to clear the inactive
  //    elements on our reference.
  //  - We want to test both loads and stores that span { z31, z0 }, so we have
  //    to move some values around.
  //
  // Registers z3-z18 will hold as-stored values (with inactive elements
  // cleared). Registers z19-z31 and z0-z2 will hold the values that were
  // loaded.

  __ Index(z10.VnB(), 1, -7);
  __ Index(z11.VnB(), 2, -7);
  __ Index(z12.VnB(), 3, -7);
  __ Index(z13.VnB(), 4, -7);
  __ Ptrue(p0.VnB());
  __ St4b(z10.VnB(), z11.VnB(), z12.VnB(), z13.VnB(), p0, SVEMemOperand(x0));
  // Save the stored values for ld4 tests.
  __ Dup(z3.VnB(), 0);
  __ Dup(z4.VnB(), 0);
  __ Dup(z5.VnB(), 0);
  __ Dup(z6.VnB(), 0);
  __ Mov(z3.VnB(), p0.Merging(), z10.VnB());
  __ Mov(z4.VnB(), p0.Merging(), z11.VnB());
  __ Mov(z5.VnB(), p0.Merging(), z12.VnB());
  __ Mov(z6.VnB(), p0.Merging(), z13.VnB());

  // Wrap around from z31 to z0.
  __ Index(z31.VnH(), -2, 5);
  __ Index(z0.VnH(), -3, 5);
  __ Index(z1.VnH(), -4, 5);
  __ Index(z2.VnH(), -5, 5);
  __ Ptrue(p1.VnH(), SVE_MUL3);
  __ St4h(z31.VnH(),
          z0.VnH(),
          z1.VnH(),
          z2.VnH(),
          p1,
          SVEMemOperand(x0, 4, SVE_MUL_VL));
  // Save the stored values for ld4 tests.
  __ Dup(z7.VnH(), 0);
  __ Dup(z8.VnH(), 0);
  __ Dup(z9.VnH(), 0);
  __ Dup(z10.VnH(), 0);
  __ Mov(z7.VnH(), p1.Merging(), z31.VnH());
  __ Mov(z8.VnH(), p1.Merging(), z0.VnH());
  __ Mov(z9.VnH(), p1.Merging(), z1.VnH());
  __ Mov(z10.VnH(), p1.Merging(), z2.VnH());

  // Wrap around from z31 to z0.
  __ Index(z29.VnS(), 2, -7);
  __ Index(z30.VnS(), 3, -7);
  __ Index(z31.VnS(), 4, -7);
  __ Index(z0.VnS(), 5, -7);
  __ Ptrue(p2.VnS(), SVE_POW2);
  __ St4w(z29.VnS(),
          z30.VnS(),
          z31.VnS(),
          z0.VnS(),
          p2,
          SVEMemOperand(x0, -12, SVE_MUL_VL));
  // Save the stored values for ld4 tests.
  __ Dup(z11.VnS(), 0);
  __ Dup(z12.VnS(), 0);
  __ Dup(z13.VnS(), 0);
  __ Dup(z14.VnS(), 0);
  __ Mov(z11.VnS(), p2.Merging(), z29.VnS());
  __ Mov(z12.VnS(), p2.Merging(), z30.VnS());
  __ Mov(z13.VnS(), p2.Merging(), z31.VnS());
  __ Mov(z14.VnS(), p2.Merging(), z0.VnS());

  __ Index(z20.VnD(), -7, 8);
  __ Index(z21.VnD(), -8, 8);
  __ Index(z22.VnD(), -9, 8);
  __ Index(z23.VnD(), -10, 8);
  // Sparse predication, including some irrelevant bits (0xee). To make the
  // results easy to check, activate each lane <n> where n is a multiple of 5.
  Initialise(&masm,
             p3,
             0xeee10000000001ee,
             0xeeeeeee100000000,
             0x01eeeeeeeee10000,
             0x000001eeeeeeeee1);
  __ St4d(z20.VnD(),
          z21.VnD(),
          z22.VnD(),
          z23.VnD(),
          p3,
          SVEMemOperand(x0, 16, SVE_MUL_VL));
  // Save the stored values for ld4 tests.
  __ Dup(z15.VnD(), 0);
  __ Dup(z16.VnD(), 0);
  __ Dup(z17.VnD(), 0);
  __ Dup(z18.VnD(), 0);
  __ Mov(z15.VnD(), p3.Merging(), z20.VnD());
  __ Mov(z16.VnD(), p3.Merging(), z21.VnD());
  __ Mov(z17.VnD(), p3.Merging(), z22.VnD());
  __ Mov(z18.VnD(), p3.Merging(), z23.VnD());

  // Corresponding loads.
  // Wrap around from z31 to z0, moving the results elsewhere to avoid overlap.
  __ Ld4b(z31.VnB(),
          z0.VnB(),
          z1.VnB(),
          z2.VnB(),
          p0.Zeroing(),
          SVEMemOperand(x0));
  __ Mov(z19, z31);
  __ Mov(z20, z0);
  __ Mov(z21, z1);
  __ Mov(z22, z2);
  __ Ld4h(z23.VnH(),
          z24.VnH(),
          z25.VnH(),
          z26.VnH(),
          p1.Zeroing(),
          SVEMemOperand(x0, 4, SVE_MUL_VL));
  __ Ld4w(z27.VnS(),
          z28.VnS(),
          z29.VnS(),
          z30.VnS(),
          p2.Zeroing(),
          SVEMemOperand(x0, -12, SVE_MUL_VL));
  // Wrap around from z31 to z0.
  __ Ld4d(z31.VnD(),
          z0.VnD(),
          z1.VnD(),
          z2.VnD(),
          p3.Zeroing(),
          SVEMemOperand(x0, 16, SVE_MUL_VL));

  END();

  if (CAN_RUN()) {
    RUN();

    uint8_t* expected = new uint8_t[data_size];
    memset(expected, 0, data_size);
    uint8_t* middle = &expected[data_size / 2];

    int vl_b = vl / kBRegSizeInBytes;
    int vl_h = vl / kHRegSizeInBytes;
    int vl_s = vl / kSRegSizeInBytes;
    int vl_d = vl / kDRegSizeInBytes;

    int reg_count = 4;

    // st2b { z10.b, z11.b, z12.b, z13.b }, SVE_ALL
    for (int i = 0; i < vl_b; i++) {
      uint8_t lane0 = 1 - (7 * i);
      uint8_t lane1 = 2 - (7 * i);
      uint8_t lane2 = 3 - (7 * i);
      uint8_t lane3 = 4 - (7 * i);
      MemoryWrite(middle, 0, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, 0, (i * reg_count) + 1, lane1);
      MemoryWrite(middle, 0, (i * reg_count) + 2, lane2);
      MemoryWrite(middle, 0, (i * reg_count) + 3, lane3);
    }

    // st4h { z31.h, z0.h, z1.h, z2.h }, SVE_MUL3
    int vl_h_mul3 = vl_h - (vl_h % 3);
    for (int i = 0; i < vl_h_mul3; i++) {
      int64_t offset = 4 * vl;
      uint16_t lane0 = -2 + (5 * i);
      uint16_t lane1 = -3 + (5 * i);
      uint16_t lane2 = -4 + (5 * i);
      uint16_t lane3 = -5 + (5 * i);
      MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
      MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
      MemoryWrite(middle, offset, (i * reg_count) + 3, lane3);
    }

    // st4w { z29.s, z30.s, z31.s, z0.s }, SVE_POW2
    int vl_s_pow2 = 1 << HighestSetBitPosition(vl_s);
    for (int i = 0; i < vl_s_pow2; i++) {
      int64_t offset = -12 * vl;
      uint32_t lane0 = 2 - (7 * i);
      uint32_t lane1 = 3 - (7 * i);
      uint32_t lane2 = 4 - (7 * i);
      uint32_t lane3 = 5 - (7 * i);
      MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
      MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
      MemoryWrite(middle, offset, (i * reg_count) + 3, lane3);
    }

    // st4d { z20.d, z21.d, z22.d, z23.d }, ((i % 5) == 0)
    for (int i = 0; i < vl_d; i++) {
      if ((i % 5) == 0) {
        int64_t offset = 16 * vl;
        uint64_t lane0 = -7 + (8 * i);
        uint64_t lane1 = -8 + (8 * i);
        uint64_t lane2 = -9 + (8 * i);
        uint64_t lane3 = -10 + (8 * i);
        MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
        MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
        MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
        MemoryWrite(middle, offset, (i * reg_count) + 3, lane3);
      }
    }

    ASSERT_EQUAL_MEMORY(expected, data, data_size, middle - expected);

    // Check that we loaded back the expected values.

    // st4b/ld4b
    ASSERT_EQUAL_SVE(z3, z19);
    ASSERT_EQUAL_SVE(z4, z20);
    ASSERT_EQUAL_SVE(z5, z21);
    ASSERT_EQUAL_SVE(z6, z22);

    // st4h/ld4h
    ASSERT_EQUAL_SVE(z7, z23);
    ASSERT_EQUAL_SVE(z8, z24);
    ASSERT_EQUAL_SVE(z9, z25);
    ASSERT_EQUAL_SVE(z10, z26);

    // st4w/ld4w
    ASSERT_EQUAL_SVE(z11, z27);
    ASSERT_EQUAL_SVE(z12, z28);
    ASSERT_EQUAL_SVE(z13, z29);
    ASSERT_EQUAL_SVE(z14, z30);

    // st4d/ld4d
    ASSERT_EQUAL_SVE(z15, z31);
    ASSERT_EQUAL_SVE(z16, z0);
    ASSERT_EQUAL_SVE(z17, z1);
    ASSERT_EQUAL_SVE(z18, z2);

    delete[] expected;
  }
  delete[] data;
}

TEST_SVE(sve_ld4_st4_scalar_plus_scalar) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int vl = config->sve_vl_in_bytes();

  // Allocate plenty of space to enable indexing in both directions.
  int data_size = vl * 128;

  uint8_t* data = new uint8_t[data_size];
  memset(data, 0, data_size);

  // Set the base half-way through the buffer so we can use negative indeces.
  __ Mov(x0, reinterpret_cast<uintptr_t>(&data[data_size / 2]));

  // We can test ld4 by comparing the values loaded with the values stored.
  // There are two complications:
  //  - Loads have zeroing predication, so we have to clear the inactive
  //    elements on our reference.
  //  - We want to test both loads and stores that span { z31, z0 }, so we have
  //    to move some values around.
  //
  // Registers z3-z18 will hold as-stored values (with inactive elements
  // cleared). Registers z19-z31 and z0-z2 will hold the values that were
  // loaded.

  __ Index(z19.VnB(), -4, 11);
  __ Index(z20.VnB(), -5, 11);
  __ Index(z21.VnB(), -6, 11);
  __ Index(z22.VnB(), -7, 11);
  __ Ptrue(p7.VnB(), SVE_MUL4);
  __ Rdvl(x1, -1);  // Make offsets VL-dependent so we can avoid overlap.
  __ St4b(z19.VnB(),
          z20.VnB(),
          z21.VnB(),
          z22.VnB(),
          p7,
          SVEMemOperand(x0, x1, LSL, 0));
  // Save the stored values for ld4 tests.
  __ Dup(z3.VnB(), 0);
  __ Dup(z4.VnB(), 0);
  __ Dup(z5.VnB(), 0);
  __ Dup(z6.VnB(), 0);
  __ Mov(z3.VnB(), p7.Merging(), z19.VnB());
  __ Mov(z4.VnB(), p7.Merging(), z20.VnB());
  __ Mov(z5.VnB(), p7.Merging(), z21.VnB());
  __ Mov(z6.VnB(), p7.Merging(), z22.VnB());

  __ Index(z23.VnH(), 6, -2);
  __ Index(z24.VnH(), 7, -2);
  __ Index(z25.VnH(), 8, -2);
  __ Index(z26.VnH(), 9, -2);
  __ Ptrue(p6.VnH(), SVE_VL16);
  __ Rdvl(x2, 7);  // (7 * vl) << 1 = 14 * vl
  __ St4h(z23.VnH(),
          z24.VnH(),
          z25.VnH(),
          z26.VnH(),
          p6,
          SVEMemOperand(x0, x2, LSL, 1));
  // Save the stored values for ld4 tests.
  __ Dup(z7.VnH(), 0);
  __ Dup(z8.VnH(), 0);
  __ Dup(z9.VnH(), 0);
  __ Dup(z10.VnH(), 0);
  __ Mov(z7.VnH(), p6.Merging(), z23.VnH());
  __ Mov(z8.VnH(), p6.Merging(), z24.VnH());
  __ Mov(z9.VnH(), p6.Merging(), z25.VnH());
  __ Mov(z10.VnH(), p6.Merging(), z26.VnH());

  // Wrap around from z31 to z0.
  __ Index(z29.VnS(), -6, 7);
  __ Index(z30.VnS(), -7, 7);
  __ Index(z31.VnS(), -8, 7);
  __ Index(z0.VnS(), -9, 7);
  // Sparse predication, including some irrelevant bits (0xe). To make the
  // results easy to check, activate each lane <n> where n is a multiple of 5.
  Initialise(&masm,
             p5,
             0xeee1000010000100,
             0x001eeee100001000,
             0x0100001eeee10000,
             0x10000100001eeee1);
  __ Rdvl(x3, -5);  // -(5 * vl) << 2 = -20 * vl
  __ St4w(z29.VnS(),
          z30.VnS(),
          z31.VnS(),
          z0.VnS(),
          p5,
          SVEMemOperand(x0, x3, LSL, 2));
  // Save the stored values for ld4 tests.
  __ Dup(z11.VnS(), 0);
  __ Dup(z12.VnS(), 0);
  __ Dup(z13.VnS(), 0);
  __ Dup(z14.VnS(), 0);
  __ Mov(z11.VnS(), p5.Merging(), z29.VnS());
  __ Mov(z12.VnS(), p5.Merging(), z30.VnS());
  __ Mov(z13.VnS(), p5.Merging(), z31.VnS());
  __ Mov(z14.VnS(), p5.Merging(), z0.VnS());

  __ Index(z31.VnD(), 32, -11);
  __ Index(z0.VnD(), 33, -11);
  __ Index(z1.VnD(), 34, -11);
  __ Index(z2.VnD(), 35, -11);
  __ Ptrue(p4.VnD(), SVE_MUL3);
  __ Rdvl(x4, -1);  // -(1 * vl) << 3 = -8 *vl
  __ St4d(z31.VnD(),
          z0.VnD(),
          z1.VnD(),
          z2.VnD(),
          p4,
          SVEMemOperand(x0, x4, LSL, 3));
  // Save the stored values for ld4 tests.
  __ Dup(z15.VnD(), 0);
  __ Dup(z16.VnD(), 0);
  __ Dup(z17.VnD(), 0);
  __ Dup(z18.VnD(), 0);
  __ Mov(z15.VnD(), p4.Merging(), z31.VnD());
  __ Mov(z16.VnD(), p4.Merging(), z0.VnD());
  __ Mov(z17.VnD(), p4.Merging(), z1.VnD());
  __ Mov(z18.VnD(), p4.Merging(), z2.VnD());

  // Corresponding loads.
  // Wrap around from z31 to z0, moving the results elsewhere to avoid overlap.
  __ Ld4b(z31.VnB(),
          z0.VnB(),
          z1.VnB(),
          z2.VnB(),
          p7.Zeroing(),
          SVEMemOperand(x0, x1, LSL, 0));
  __ Mov(z19, z31);
  __ Mov(z20, z0);
  __ Mov(z21, z1);
  __ Mov(z22, z2);
  __ Ld4h(z23.VnH(),
          z24.VnH(),
          z25.VnH(),
          z26.VnH(),
          p6.Zeroing(),
          SVEMemOperand(x0, x2, LSL, 1));
  __ Ld4w(z27.VnS(),
          z28.VnS(),
          z29.VnS(),
          z30.VnS(),
          p5.Zeroing(),
          SVEMemOperand(x0, x3, LSL, 2));
  // Wrap around from z31 to z0.
  __ Ld4d(z31.VnD(),
          z0.VnD(),
          z1.VnD(),
          z2.VnD(),
          p4.Zeroing(),
          SVEMemOperand(x0, x4, LSL, 3));

  END();

  if (CAN_RUN()) {
    RUN();

    uint8_t* expected = new uint8_t[data_size];
    memset(expected, 0, data_size);
    uint8_t* middle = &expected[data_size / 2];

    int vl_b = vl / kBRegSizeInBytes;
    int vl_h = vl / kHRegSizeInBytes;
    int vl_s = vl / kSRegSizeInBytes;
    int vl_d = vl / kDRegSizeInBytes;

    int reg_count = 4;

    // st4b { z19.b, z20.b, z21.b, z22.b }, SVE_MUL4
    int vl_b_mul4 = vl_b - (vl_b % 4);
    for (int i = 0; i < vl_b_mul4; i++) {
      int64_t offset = -(1 << kBRegSizeInBytesLog2) * vl;
      uint8_t lane0 = -4 + (11 * i);
      uint8_t lane1 = -5 + (11 * i);
      uint8_t lane2 = -6 + (11 * i);
      uint8_t lane3 = -7 + (11 * i);
      MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
      MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
      MemoryWrite(middle, offset, (i * reg_count) + 3, lane3);
    }

    // st4h { z22.h, z23.h, z24.h, z25.h }, SVE_VL16
    if (vl_h >= 16) {
      for (int i = 0; i < 16; i++) {
        int64_t offset = (7 << kHRegSizeInBytesLog2) * vl;
        uint16_t lane0 = 6 - (2 * i);
        uint16_t lane1 = 7 - (2 * i);
        uint16_t lane2 = 8 - (2 * i);
        uint16_t lane3 = 9 - (2 * i);
        MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
        MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
        MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
        MemoryWrite(middle, offset, (i * reg_count) + 3, lane3);
      }
    }

    // st4w { z29.s, z30.s, z31.s, z0.s }, ((i % 5) == 0)
    for (int i = 0; i < vl_s; i++) {
      if ((i % 5) == 0) {
        int64_t offset = -(5 << kSRegSizeInBytesLog2) * vl;
        uint32_t lane0 = -6 + (7 * i);
        uint32_t lane1 = -7 + (7 * i);
        uint32_t lane2 = -8 + (7 * i);
        uint32_t lane3 = -9 + (7 * i);
        MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
        MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
        MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
        MemoryWrite(middle, offset, (i * reg_count) + 3, lane3);
      }
    }

    // st4d { z31.d, z0.d, z1.d, z2.d }, SVE_MUL3
    int vl_d_mul3 = vl_d - (vl_d % 3);
    for (int i = 0; i < vl_d_mul3; i++) {
      int64_t offset = -(1 << kDRegSizeInBytesLog2) * vl;
      uint64_t lane0 = 32 - (11 * i);
      uint64_t lane1 = 33 - (11 * i);
      uint64_t lane2 = 34 - (11 * i);
      uint64_t lane3 = 35 - (11 * i);
      MemoryWrite(middle, offset, (i * reg_count) + 0, lane0);
      MemoryWrite(middle, offset, (i * reg_count) + 1, lane1);
      MemoryWrite(middle, offset, (i * reg_count) + 2, lane2);
      MemoryWrite(middle, offset, (i * reg_count) + 3, lane3);
    }

    ASSERT_EQUAL_MEMORY(expected, data, data_size, middle - expected);

    // Check that we loaded back the expected values.

    // st4b/ld4b
    ASSERT_EQUAL_SVE(z3, z19);
    ASSERT_EQUAL_SVE(z4, z20);
    ASSERT_EQUAL_SVE(z5, z21);
    ASSERT_EQUAL_SVE(z6, z22);

    // st4h/ld4h
    ASSERT_EQUAL_SVE(z7, z23);
    ASSERT_EQUAL_SVE(z8, z24);
    ASSERT_EQUAL_SVE(z9, z25);
    ASSERT_EQUAL_SVE(z10, z26);

    // st4w/ld4w
    ASSERT_EQUAL_SVE(z11, z27);
    ASSERT_EQUAL_SVE(z12, z28);
    ASSERT_EQUAL_SVE(z13, z29);
    ASSERT_EQUAL_SVE(z14, z30);

    // st4d/ld4d
    ASSERT_EQUAL_SVE(z15, z31);
    ASSERT_EQUAL_SVE(z16, z0);
    ASSERT_EQUAL_SVE(z17, z1);
    ASSERT_EQUAL_SVE(z18, z2);

    delete[] expected;
  }
  delete[] data;
}

TEST_SVE(sve_ld234_st234_scalar_plus_scalar_sp) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Check that the simulator correctly interprets rn == 31 as sp.
  // The indexing logic is the same regardless so we just check one load and
  // store of each type.

  // There are no pre- or post-indexing modes, so reserve space first.
  __ ClaimVL(2 + 3 + 4);

  __ Index(z0.VnB(), 42, 2);
  __ Index(z1.VnB(), 43, 2);
  __ Ptrue(p0.VnB(), SVE_VL7);
  __ Rdvl(x0, 0);
  __ St2b(z0.VnB(), z1.VnB(), p0, SVEMemOperand(sp, x0));

  __ Index(z4.VnH(), 42, 3);
  __ Index(z5.VnH(), 43, 3);
  __ Index(z6.VnH(), 44, 3);
  __ Ptrue(p1.VnH(), SVE_POW2);
  __ Rdvl(x1, 2);
  __ Lsr(x1, x1, 1);
  __ St3h(z4.VnH(), z5.VnH(), z6.VnH(), p1, SVEMemOperand(sp, x1, LSL, 1));

  __ Index(z8.VnS(), 42, 4);
  __ Index(z9.VnS(), 43, 4);
  __ Index(z10.VnS(), 44, 4);
  __ Index(z11.VnS(), 45, 4);
  __ Ptrue(p2.VnS());
  __ Rdvl(x2, 2 + 3);
  __ Lsr(x2, x2, 2);
  __ St4w(z8.VnS(),
          z9.VnS(),
          z10.VnS(),
          z11.VnS(),
          p2,
          SVEMemOperand(sp, x2, LSL, 2));

  // Corresponding loads.
  // We have to explicitly zero inactive lanes in the reference values because
  // loads have zeroing predication.
  __ Dup(z12.VnB(), 0);
  __ Dup(z13.VnB(), 0);
  __ Mov(z12.VnB(), p0.Merging(), z0.VnB());
  __ Mov(z13.VnB(), p0.Merging(), z1.VnB());
  __ Ld2b(z0.VnB(), z1.VnB(), p0.Zeroing(), SVEMemOperand(sp, x0));

  __ Dup(z16.VnH(), 0);
  __ Dup(z17.VnH(), 0);
  __ Dup(z18.VnH(), 0);
  __ Mov(z16.VnH(), p1.Merging(), z4.VnH());
  __ Mov(z17.VnH(), p1.Merging(), z5.VnH());
  __ Mov(z18.VnH(), p1.Merging(), z6.VnH());
  __ Ld3h(z4.VnH(),
          z5.VnH(),
          z6.VnH(),
          p1.Zeroing(),
          SVEMemOperand(sp, x1, LSL, 1));

  __ Dup(z20.VnS(), 0);
  __ Dup(z21.VnS(), 0);
  __ Dup(z22.VnS(), 0);
  __ Dup(z23.VnS(), 0);
  __ Mov(z20.VnS(), p2.Merging(), z8.VnS());
  __ Mov(z21.VnS(), p2.Merging(), z9.VnS());
  __ Mov(z22.VnS(), p2.Merging(), z10.VnS());
  __ Mov(z23.VnS(), p2.Merging(), z11.VnS());
  __ Ld4w(z8.VnS(),
          z9.VnS(),
          z10.VnS(),
          z11.VnS(),
          p2.Zeroing(),
          SVEMemOperand(sp, x2, LSL, 2));

  __ DropVL(2 + 3 + 4);

  END();

  if (CAN_RUN()) {
    RUN();

    // The most likely failure mode is the that simulator reads sp as xzr and
    // crashes on execution. We already test the address calculations separately
    // and sp doesn't change this, so just test that we load the values we
    // stored.

    // st2b/ld2b
    ASSERT_EQUAL_SVE(z0, z12);
    ASSERT_EQUAL_SVE(z1, z13);

    // st3h/ld3h
    ASSERT_EQUAL_SVE(z4, z16);
    ASSERT_EQUAL_SVE(z5, z17);
    ASSERT_EQUAL_SVE(z6, z18);

    // st4h/ld4h
    ASSERT_EQUAL_SVE(z8, z20);
    ASSERT_EQUAL_SVE(z9, z21);
    ASSERT_EQUAL_SVE(z10, z22);
    ASSERT_EQUAL_SVE(z11, z23);
  }
}

TEST_SVE(sve_ld234_st234_scalar_plus_imm_sp) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Check that the simulator correctly interprets rn == 31 as sp.
  // The indexing logic is the same regardless so we just check one load and
  // store of each type.

  // There are no pre- or post-indexing modes, so reserve space first.
  // Note that the stores fill in an order that allows each immediate to be a
  // multiple of the number of registers.
  __ ClaimVL(4 + 2 + 3);

  __ Index(z0.VnB(), 42, 2);
  __ Index(z1.VnB(), 43, 2);
  __ Ptrue(p0.VnB(), SVE_POW2);
  __ St2b(z0.VnB(), z1.VnB(), p0, SVEMemOperand(sp, 4, SVE_MUL_VL));

  __ Index(z4.VnH(), 42, 3);
  __ Index(z5.VnH(), 43, 3);
  __ Index(z6.VnH(), 44, 3);
  __ Ptrue(p1.VnH(), SVE_VL7);
  __ St3h(z4.VnH(), z5.VnH(), z6.VnH(), p1, SVEMemOperand(sp, 6, SVE_MUL_VL));

  __ Index(z8.VnS(), 42, 4);
  __ Index(z9.VnS(), 43, 4);
  __ Index(z10.VnS(), 44, 4);
  __ Index(z11.VnS(), 45, 4);
  __ Ptrue(p2.VnS());
  __ St4w(z8.VnS(), z9.VnS(), z10.VnS(), z11.VnS(), p2, SVEMemOperand(sp));

  // Corresponding loads.
  // We have to explicitly zero inactive lanes in the reference values because
  // loads have zeroing predication.
  __ Dup(z12.VnB(), 0);
  __ Dup(z13.VnB(), 0);
  __ Mov(z12.VnB(), p0.Merging(), z0.VnB());
  __ Mov(z13.VnB(), p0.Merging(), z1.VnB());
  __ Ld2b(z0.VnB(), z1.VnB(), p0.Zeroing(), SVEMemOperand(sp, 4, SVE_MUL_VL));

  __ Dup(z16.VnH(), 0);
  __ Dup(z17.VnH(), 0);
  __ Dup(z18.VnH(), 0);
  __ Mov(z16.VnH(), p1.Merging(), z4.VnH());
  __ Mov(z17.VnH(), p1.Merging(), z5.VnH());
  __ Mov(z18.VnH(), p1.Merging(), z6.VnH());
  __ Ld3h(z4.VnH(),
          z5.VnH(),
          z6.VnH(),
          p1.Zeroing(),
          SVEMemOperand(sp, 6, SVE_MUL_VL));

  __ Dup(z20.VnS(), 0);
  __ Dup(z21.VnS(), 0);
  __ Dup(z22.VnS(), 0);
  __ Dup(z23.VnS(), 0);
  __ Mov(z20.VnS(), p2.Merging(), z8.VnS());
  __ Mov(z21.VnS(), p2.Merging(), z9.VnS());
  __ Mov(z22.VnS(), p2.Merging(), z10.VnS());
  __ Mov(z23.VnS(), p2.Merging(), z11.VnS());
  __ Ld4w(z8.VnS(),
          z9.VnS(),
          z10.VnS(),
          z11.VnS(),
          p2.Zeroing(),
          SVEMemOperand(sp));

  __ DropVL(4 + 2 + 3);

  END();

  if (CAN_RUN()) {
    RUN();

    // The most likely failure mode is the that simulator reads sp as xzr and
    // crashes on execution. We already test the address calculations separately
    // and sp doesn't change this, so just test that we load the values we
    // stored.
    // TODO: Actually do this, once loads are implemented.
  }
}

TEST_SVE(sve_ldff1_scalar_plus_scalar) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int vl = config->sve_vl_in_bytes();
  size_t page_size = sysconf(_SC_PAGE_SIZE);
  VIXL_ASSERT(page_size > static_cast<size_t>(vl));

  // Allocate two pages, then mprotect the second one to make it inaccessible.
  uintptr_t data = reinterpret_cast<uintptr_t>(mmap(NULL,
                                                    page_size * 2,
                                                    PROT_READ | PROT_WRITE,
                                                    MAP_PRIVATE | MAP_ANONYMOUS,
                                                    -1,
                                                    0));
  mprotect(reinterpret_cast<void*>(data + page_size), page_size, PROT_NONE);

  // Fill the accessible page with arbitrary data.
  for (size_t i = 0; i < page_size; i++) {
    // Reverse bits so we get a mixture of positive and negative values.
    uint8_t byte = ReverseBits(static_cast<uint8_t>(i));
    memcpy(reinterpret_cast<void*>(data + i), &byte, 1);
  }

  __ Mov(x20, data);

  PRegister all = p7;
  __ Ptrue(all.VnB());

  size_t offset_modifier = 0;

  // The highest adddress at which a load stopped. Every FF load should fault at
  // `data + page_size`, so this value should not exceed that value. However,
  // the architecture allows fault-tolerant loads to fault arbitrarily, so the
  // real value may be lower.
  //
  // This is used to check that the `mprotect` above really does make the second
  // page inaccessible, and that the resulting FFR from each load reflects that.
  Register limit = x22;
  __ Mov(limit, 0);

  // If the FFR grows unexpectedly, we increment this register by the
  // difference. FFR should never grow, except when explicitly set.
  Register ffr_grow_count = x23;
  __ Mov(ffr_grow_count, 0);

#define VIXL_EMIT_LDFF1_TEST(LDFF1, M_SIZE, Zt, E_SIZE, LD1, ZtRef)            \
  do {                                                                         \
    /* Set the offset so that the load is guaranteed to start in the */        \
    /* accessible page, but end in the inaccessible one. */                    \
    VIXL_ASSERT((page_size % k##M_SIZE##RegSizeInBytes) == 0);                 \
    VIXL_ASSERT((vl % k##M_SIZE##RegSizeInBytes) == 0);                        \
    size_t elements_per_page = page_size / k##M_SIZE##RegSizeInBytes;          \
    size_t elements_per_access = vl / k##E_SIZE##RegSizeInBytes;               \
    size_t min_offset = (elements_per_page - elements_per_access) + 1;         \
    size_t max_offset = elements_per_page - 1;                                 \
    size_t offset =                                                            \
        min_offset + (offset_modifier % (max_offset - min_offset + 1));        \
    offset_modifier++;                                                         \
    __ Mov(x21, offset);                                                       \
    __ Setffr();                                                               \
    __ LDFF1(Zt.Vn##E_SIZE(),                                                  \
             all.Zeroing(),                                                    \
             SVEMemOperand(x20, x21, LSL, k##M_SIZE##RegSizeInBytesLog2));     \
    __ Rdffrs(p0.VnB(), all.Zeroing());                                        \
    /* Execute another LDFF1 with no offset, so that every element could be */ \
    /* read. It should respect FFR, and load no more than we loaded the */     \
    /* first time. */                                                          \
    __ LDFF1(ZtRef.Vn##E_SIZE(), all.Zeroing(), SVEMemOperand(x20));           \
    __ Rdffrs(p1.VnB(), all.Zeroing());                                        \
    __ Cntp(x0, all, p1.VnB());                                                \
    __ Uqdecp(x0, p0.VnB());                                                   \
    __ Add(ffr_grow_count, ffr_grow_count, x0);                                \
    /* Use the FFR to predicate the normal load. If it wasn't properly set, */ \
    /* the normal load will abort. */                                          \
    __ LD1(ZtRef.Vn##E_SIZE(),                                                 \
           p0.Zeroing(),                                                       \
           SVEMemOperand(x20, x21, LSL, k##M_SIZE##RegSizeInBytesLog2));       \
    /* Work out the address after the one that was just accessed. */           \
    __ Incp(x21, p0.Vn##E_SIZE());                                             \
    __ Add(x0, x20, Operand(x21, LSL, k##M_SIZE##RegSizeInBytesLog2));         \
    __ Cmp(limit, x0);                                                         \
    __ Csel(limit, limit, x0, hs);                                             \
    /* Clear lanes inactive in FFR. These have an undefined result. */         \
    /* TODO: Use the 'Not' and 'Mov' aliases once they are implemented. */     \
    __ Eor(p0.Vn##E_SIZE(), all.Zeroing(), p0.Vn##E_SIZE(), all.Vn##E_SIZE()); \
    __ Cpy(Zt.Vn##E_SIZE(), p0.Merging(), 0);                                  \
  } while (0)

  VIXL_EMIT_LDFF1_TEST(Ldff1b, B, z0, B, Ld1b, z16);
  VIXL_EMIT_LDFF1_TEST(Ldff1b, B, z1, H, Ld1b, z17);
  VIXL_EMIT_LDFF1_TEST(Ldff1b, B, z2, S, Ld1b, z18);
  VIXL_EMIT_LDFF1_TEST(Ldff1b, B, z3, D, Ld1b, z19);

  VIXL_EMIT_LDFF1_TEST(Ldff1h, H, z4, H, Ld1h, z20);
  VIXL_EMIT_LDFF1_TEST(Ldff1h, H, z5, S, Ld1h, z21);
  VIXL_EMIT_LDFF1_TEST(Ldff1h, H, z6, D, Ld1h, z22);

  VIXL_EMIT_LDFF1_TEST(Ldff1w, S, z7, S, Ld1w, z23);
  VIXL_EMIT_LDFF1_TEST(Ldff1w, S, z8, D, Ld1w, z24);

  VIXL_EMIT_LDFF1_TEST(Ldff1d, D, z9, D, Ld1d, z25);

  VIXL_EMIT_LDFF1_TEST(Ldff1sb, B, z10, H, Ld1sb, z26);
  VIXL_EMIT_LDFF1_TEST(Ldff1sb, B, z11, S, Ld1sb, z27);
  VIXL_EMIT_LDFF1_TEST(Ldff1sb, B, z12, D, Ld1sb, z28);

  VIXL_EMIT_LDFF1_TEST(Ldff1sh, H, z13, S, Ld1sh, z29);
  VIXL_EMIT_LDFF1_TEST(Ldff1sh, H, z14, D, Ld1sh, z30);

  VIXL_EMIT_LDFF1_TEST(Ldff1sw, S, z15, D, Ld1sw, z31);

#undef VIXL_EMIT_LDFF1_TEST

  END();

  if (CAN_RUN()) {
    RUN();

    uintptr_t expected_limit = data + page_size;
    uintptr_t measured_limit = core.xreg(limit.GetCode());
    VIXL_CHECK(measured_limit <= expected_limit);
    if (measured_limit < expected_limit) {
      // We can't fail the test for this case, but a warning is helpful for
      // manually-run tests.
      printf(
          "WARNING: All fault-tolerant loads detected faults before the\n"
          "expected limit. This is architecturally possible, but improbable,\n"
          "and could be a symptom of another problem.\n");
    }

    ASSERT_EQUAL_64(0, ffr_grow_count);

    // Ldff1b
    ASSERT_EQUAL_SVE(z0.VnB(), z16.VnB());
    ASSERT_EQUAL_SVE(z1.VnH(), z17.VnH());
    ASSERT_EQUAL_SVE(z2.VnS(), z18.VnS());
    ASSERT_EQUAL_SVE(z3.VnD(), z19.VnD());

    // Ldff1h
    ASSERT_EQUAL_SVE(z4.VnH(), z20.VnH());
    ASSERT_EQUAL_SVE(z5.VnS(), z21.VnS());
    ASSERT_EQUAL_SVE(z6.VnD(), z22.VnD());

    // Ldff1w
    ASSERT_EQUAL_SVE(z7.VnS(), z23.VnS());
    ASSERT_EQUAL_SVE(z8.VnD(), z24.VnD());

    // Ldff1d
    ASSERT_EQUAL_SVE(z9.VnD(), z25.VnD());

    // Ldff1sb
    ASSERT_EQUAL_SVE(z10.VnH(), z26.VnH());
    ASSERT_EQUAL_SVE(z11.VnS(), z27.VnS());
    ASSERT_EQUAL_SVE(z12.VnD(), z28.VnD());

    // Ldff1sh
    ASSERT_EQUAL_SVE(z13.VnS(), z29.VnS());
    ASSERT_EQUAL_SVE(z14.VnD(), z30.VnD());

    // Ldff1sw
    ASSERT_EQUAL_SVE(z15.VnD(), z31.VnD());
  }

  munmap(reinterpret_cast<void*>(data), page_size * 2);
}

// Test gather loads by comparing them with the result of a set of equivalent
// scalar loads.
template <typename F>
static void GatherLoadHelper(Test* config,
                             unsigned msize_in_bits,
                             unsigned esize_in_bits,
                             F sve_ld1,
                             bool is_signed) {
  // SVE supports 32- and 64-bit addressing for gather loads.
  VIXL_ASSERT((esize_in_bits == kSRegSize) || (esize_in_bits == kDRegSize));
  static const unsigned kMaxLaneCount = kZRegMaxSize / kSRegSize;

  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  unsigned msize_in_bytes = msize_in_bits / kBitsPerByte;
  unsigned esize_in_bytes = esize_in_bits / kBitsPerByte;
  int vl = config->sve_vl_in_bytes();

  // Use a fixed seed for nrand48() so that test runs are reproducible.
  unsigned short seed[3] = {1, 2, 3};  // NOLINT(runtime/int)

  // Fill a buffer with arbitrary data.
  size_t buffer_size = vl * 64;
  uint64_t data = reinterpret_cast<uintptr_t>(malloc(buffer_size));
  for (size_t i = 0; i < buffer_size; i++) {
    uint8_t byte = nrand48(seed) & 0xff;
    memcpy(reinterpret_cast<void*>(data + i), &byte, 1);
  }

  // Vectors of random addresses and offsets into the buffer.
  uint64_t addresses[kMaxLaneCount];
  uint64_t offsets[kMaxLaneCount];
  uint64_t max_address = 0;
  for (unsigned i = 0; i < kMaxLaneCount; i++) {
    uint64_t rnd = nrand48(seed);
    // Limit the range to the set of completely-accessible elements in memory.
    offsets[i] = rnd % (buffer_size - msize_in_bytes);
    addresses[i] = data + offsets[i];
    max_address = std::max(max_address, addresses[i]);
  }

  // Maximised offsets, to ensure that the address calculation is modulo-2^64,
  // and that the vector addresses are not sign-extended.
  uint64_t uint_e_max = (esize_in_bits == kDRegSize) ? UINT64_MAX : UINT32_MAX;
  uint64_t maxed_offsets[kMaxLaneCount];
  uint64_t maxed_offsets_imm = max_address - uint_e_max;
  for (unsigned i = 0; i < kMaxLaneCount; i++) {
    maxed_offsets[i] = addresses[i] - maxed_offsets_imm;
  }

  ZRegister zn = z0.WithLaneSize(esize_in_bits);
  ZRegister zt_addresses = z1.WithLaneSize(esize_in_bits);
  ZRegister zt_offsets = z2.WithLaneSize(esize_in_bits);
  ZRegister zt_maxed = z3.WithLaneSize(esize_in_bits);
  ZRegister zt_ref = z4.WithLaneSize(esize_in_bits);

  PRegisterZ pg = p0.Zeroing();
  Initialise(&masm,
             pg,
             0x9abcdef012345678,
             0xabcdef0123456789,
             0xf4f3f1f0fefdfcfa,
             0xf9f8f6f5f3f2f0ff);

  // Execute each load.

  if (esize_in_bits == kDRegSize) {
    // Only test `addresses` if we can use 64-bit pointers. InsrHelper will fail
    // if any value won't fit in a lane of zn.
    InsrHelper(&masm, zn, addresses);
    (masm.*sve_ld1)(zt_addresses, pg, SVEMemOperand(zn));
  }

  InsrHelper(&masm, zn, offsets);
  (masm.*sve_ld1)(zt_offsets, pg, SVEMemOperand(zn, data));

  InsrHelper(&masm, zn, maxed_offsets);
  (masm.*sve_ld1)(zt_maxed, pg, SVEMemOperand(zn, maxed_offsets_imm));

  // TODO: Also test scalar-plus-vector SVEMemOperands.
  // TODO: Also test first-fault loads.

  // Generate a reference result using scalar loads.

  ZRegister lane_numbers = z10.WithLaneSize(esize_in_bits);
  __ Index(lane_numbers, 0, 1);
  __ Dup(zt_ref, 0);
  for (unsigned i = 0; i < (vl / esize_in_bytes); i++) {
    __ Mov(x0, addresses[ArrayLength(addresses) - i - 1]);
    Register rt(0, esize_in_bits);
    if (is_signed) {
      switch (msize_in_bits) {
        case kBRegSize:
          __ Ldrsb(rt, MemOperand(x0));
          break;
        case kHRegSize:
          __ Ldrsh(rt, MemOperand(x0));
          break;
        case kWRegSize:
          __ Ldrsw(rt, MemOperand(x0));
          break;
      }
    } else {
      switch (msize_in_bits) {
        case kBRegSize:
          __ Ldrb(rt, MemOperand(x0));
          break;
        case kHRegSize:
          __ Ldrh(rt, MemOperand(x0));
          break;
        case kWRegSize:
          __ Ldr(rt.W(), MemOperand(x0));
          break;
        case kXRegSize:
          __ Ldr(rt, MemOperand(x0));
          break;
      }
    }

    // Emulate predication.
    __ Cmpeq(p7.WithLaneSize(esize_in_bits), pg, lane_numbers, i);
    __ Cpy(zt_ref, p7.Merging(), rt);
  }

  END();

  if (CAN_RUN()) {
    RUN();

    if (esize_in_bits == kDRegSize) {
      ASSERT_EQUAL_SVE(zt_ref, zt_addresses);
    }
    ASSERT_EQUAL_SVE(zt_ref, zt_offsets);
    ASSERT_EQUAL_SVE(zt_ref, zt_maxed);
  }

  free(reinterpret_cast<void*>(data));
}

TEST_SVE(sve_ld1b_64bit_vector_plus_immediate) {
  GatherLoadHelper(config, kBRegSize, kDRegSize, &MacroAssembler::Ld1b, false);
}

TEST_SVE(sve_ld1h_64bit_vector_plus_immediate) {
  GatherLoadHelper(config, kHRegSize, kDRegSize, &MacroAssembler::Ld1h, false);
}

TEST_SVE(sve_ld1w_64bit_vector_plus_immediate) {
  GatherLoadHelper(config, kSRegSize, kDRegSize, &MacroAssembler::Ld1w, false);
}

TEST_SVE(sve_ld1d_64bit_vector_plus_immediate) {
  GatherLoadHelper(config, kDRegSize, kDRegSize, &MacroAssembler::Ld1d, false);
}

TEST_SVE(sve_ld1sb_64bit_vector_plus_immediate) {
  GatherLoadHelper(config, kBRegSize, kDRegSize, &MacroAssembler::Ld1sb, true);
}

TEST_SVE(sve_ld1sh_64bit_vector_plus_immediate) {
  GatherLoadHelper(config, kHRegSize, kDRegSize, &MacroAssembler::Ld1sh, true);
}

TEST_SVE(sve_ld1sw_64bit_vector_plus_immediate) {
  GatherLoadHelper(config, kSRegSize, kDRegSize, &MacroAssembler::Ld1sw, true);
}

TEST_SVE(sve_ld1b_32bit_vector_plus_immediate) {
  GatherLoadHelper(config, kBRegSize, kSRegSize, &MacroAssembler::Ld1b, false);
}

TEST_SVE(sve_ld1h_32bit_vector_plus_immediate) {
  GatherLoadHelper(config, kHRegSize, kSRegSize, &MacroAssembler::Ld1h, false);
}

TEST_SVE(sve_ld1w_32bit_vector_plus_immediate) {
  GatherLoadHelper(config, kSRegSize, kSRegSize, &MacroAssembler::Ld1w, false);
}

TEST_SVE(sve_ld1sb_32bit_vector_plus_immediate) {
  GatherLoadHelper(config, kBRegSize, kSRegSize, &MacroAssembler::Ld1sb, true);
}

TEST_SVE(sve_ld1sh_32bit_vector_plus_immediate) {
  GatherLoadHelper(config, kHRegSize, kSRegSize, &MacroAssembler::Ld1sh, true);
}

typedef void (MacroAssembler::*IntWideImmFn)(const ZRegister& zd,
                                             const ZRegister& zn,
                                             const IntegerOperand imm);

template <typename F, typename Td, typename Tn>
static void IntWideImmHelper(Test* config,
                             F macro,
                             unsigned lane_size_in_bits,
                             const Tn& zn_inputs,
                             IntegerOperand imm,
                             const Td& zd_expected) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  ZRegister zd1 = z0.WithLaneSize(lane_size_in_bits);
  InsrHelper(&masm, zd1, zn_inputs);

  // Also test with a different zn, to test the movprfx case.
  ZRegister zn = z1.WithLaneSize(lane_size_in_bits);
  InsrHelper(&masm, zn, zn_inputs);
  ZRegister zd2 = z2.WithLaneSize(lane_size_in_bits);
  ZRegister zn_copy = z3.WithSameLaneSizeAs(zn);

  // Make a copy so we can check that constructive operations preserve zn.
  __ Mov(zn_copy, zn);

  {
    UseScratchRegisterScope temps(&masm);
    // The MacroAssembler needs a P scratch register for some of these macros,
    // and it doesn't have one by default.
    temps.Include(p3);

    (masm.*macro)(zd1, zd1, imm);
    (masm.*macro)(zd2, zn, imm);
  }

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(zd_expected, zd1);

    // Check the result from `instr` with movprfx is the same as
    // the immediate version.
    ASSERT_EQUAL_SVE(zd_expected, zd2);

    ASSERT_EQUAL_SVE(zn_copy, zn);
  }
}

TEST_SVE(sve_int_wide_imm_unpredicated_smax) {
  int in_b[] = {0, -128, 127, -127, 126, 1, -1, 55};
  int in_h[] = {0, -128, 127, INT16_MIN, INT16_MAX, 1, -1, 5555};
  int in_s[] = {0, -128, 127, INT32_MIN, INT32_MAX, 1, -1, 555555};
  int64_t in_d[] = {1, 10, 10000, 1000000};

  IntWideImmFn fn = &MacroAssembler::Smax;

  int exp_b_1[] = {0, -1, 127, -1, 126, 1, -1, 55};
  int exp_h_1[] = {127, 127, 127, 127, INT16_MAX, 127, 127, 5555};
  int exp_s_1[] = {0, -128, 127, -128, INT32_MAX, 1, -1, 555555};
  int64_t exp_d_1[] = {99, 99, 10000, 1000000};

  IntWideImmHelper(config, fn, kBRegSize, in_b, -1, exp_b_1);
  IntWideImmHelper(config, fn, kHRegSize, in_h, 127, exp_h_1);
  IntWideImmHelper(config, fn, kSRegSize, in_s, -128, exp_s_1);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 99, exp_d_1);

  int exp_h_2[] = {0, -128, 127, -255, INT16_MAX, 1, -1, 5555};
  int exp_s_2[] = {2048, 2048, 2048, 2048, INT32_MAX, 2048, 2048, 555555};
  int64_t exp_d_2[] = {INT16_MAX, INT16_MAX, INT16_MAX, 1000000};

  // The immediate is in the range [-128, 127], but the macro is able to
  // synthesise unencodable immediates.
  // B-sized lanes cannot take an immediate out of the range [-128, 127].
  IntWideImmHelper(config, fn, kHRegSize, in_h, -255, exp_h_2);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 2048, exp_s_2);
  IntWideImmHelper(config, fn, kDRegSize, in_d, INT16_MAX, exp_d_2);
}

TEST_SVE(sve_int_wide_imm_unpredicated_smin) {
  int in_b[] = {0, -128, 127, -127, 126, 1, -1, 55};
  int in_h[] = {0, -128, 127, INT16_MIN, INT16_MAX, 1, -1, 5555};
  int in_s[] = {0, -128, 127, INT32_MIN, INT32_MAX, 1, -1, 555555};
  int64_t in_d[] = {1, 10, 10000, 1000000};

  IntWideImmFn fn = &MacroAssembler::Smin;

  int exp_b_1[] = {-1, -128, -1, -127, -1, -1, -1, -1};
  int exp_h_1[] = {0, -128, 127, INT16_MIN, 127, 1, -1, 127};
  int exp_s_1[] = {-128, -128, -128, INT32_MIN, -128, -128, -128, -128};
  int64_t exp_d_1[] = {1, 10, 99, 99};

  IntWideImmHelper(config, fn, kBRegSize, in_b, -1, exp_b_1);
  IntWideImmHelper(config, fn, kHRegSize, in_h, 127, exp_h_1);
  IntWideImmHelper(config, fn, kSRegSize, in_s, -128, exp_s_1);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 99, exp_d_1);

  int exp_h_2[] = {-255, -255, -255, INT16_MIN, -255, -255, -255, -255};
  int exp_s_2[] = {0, -128, 127, INT32_MIN, 2048, 1, -1, 2048};
  int64_t exp_d_2[] = {1, 10, 10000, INT16_MAX};

  // The immediate is in the range [-128, 127], but the macro is able to
  // synthesise unencodable immediates.
  // B-sized lanes cannot take an immediate out of the range [-128, 127].
  IntWideImmHelper(config, fn, kHRegSize, in_h, -255, exp_h_2);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 2048, exp_s_2);
  IntWideImmHelper(config, fn, kDRegSize, in_d, INT16_MAX, exp_d_2);
}

TEST_SVE(sve_int_wide_imm_unpredicated_umax) {
  int in_b[] = {0, 255, 127, 0x80, 1, 55};
  int in_h[] = {0, 255, 127, INT16_MAX, 1, 5555};
  int in_s[] = {0, 0xff, 0x7f, INT32_MAX, 1, 555555};
  int64_t in_d[] = {1, 10, 10000, 1000000};

  IntWideImmFn fn = &MacroAssembler::Umax;

  int exp_b_1[] = {17, 255, 127, 0x80, 17, 55};
  int exp_h_1[] = {127, 255, 127, INT16_MAX, 127, 5555};
  int exp_s_1[] = {255, 255, 255, INT32_MAX, 255, 555555};
  int64_t exp_d_1[] = {99, 99, 10000, 1000000};

  IntWideImmHelper(config, fn, kBRegSize, in_b, 17, exp_b_1);
  IntWideImmHelper(config, fn, kHRegSize, in_h, 0x7f, exp_h_1);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 0xff, exp_s_1);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 99, exp_d_1);

  int exp_h_2[] = {511, 511, 511, INT16_MAX, 511, 5555};
  int exp_s_2[] = {2048, 2048, 2048, INT32_MAX, 2048, 555555};
  int64_t exp_d_2[] = {INT16_MAX, INT16_MAX, INT16_MAX, 1000000};

  // The immediate is in the range [0, 255], but the macro is able to
  // synthesise unencodable immediates.
  // B-sized lanes cannot take an immediate out of the range [0, 255].
  IntWideImmHelper(config, fn, kHRegSize, in_h, 511, exp_h_2);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 2048, exp_s_2);
  IntWideImmHelper(config, fn, kDRegSize, in_d, INT16_MAX, exp_d_2);
}

TEST_SVE(sve_int_wide_imm_unpredicated_umin) {
  int in_b[] = {0, 255, 127, 0x80, 1, 55};
  int in_h[] = {0, 255, 127, INT16_MAX, 1, 5555};
  int in_s[] = {0, 0xff, 0x7f, INT32_MAX, 1, 555555};
  int64_t in_d[] = {1, 10, 10000, 1000000};

  IntWideImmFn fn = &MacroAssembler::Umin;

  int exp_b_1[] = {0, 17, 17, 17, 1, 17};
  int exp_h_1[] = {0, 127, 127, 127, 1, 127};
  int exp_s_1[] = {0, 255, 127, 255, 1, 255};
  int64_t exp_d_1[] = {1, 10, 99, 99};

  IntWideImmHelper(config, fn, kBRegSize, in_b, 17, exp_b_1);
  IntWideImmHelper(config, fn, kHRegSize, in_h, 0x7f, exp_h_1);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 255, exp_s_1);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 99, exp_d_1);

  int exp_h_2[] = {0, 255, 127, 511, 1, 511};
  int exp_s_2[] = {0, 255, 127, 2048, 1, 2048};
  int64_t exp_d_2[] = {1, 10, 10000, INT16_MAX};

  // The immediate is in the range [0, 255], but the macro is able to
  // synthesise unencodable immediates.
  // B-sized lanes cannot take an immediate out of the range [0, 255].
  IntWideImmHelper(config, fn, kHRegSize, in_h, 511, exp_h_2);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 2048, exp_s_2);
  IntWideImmHelper(config, fn, kDRegSize, in_d, INT16_MAX, exp_d_2);
}

TEST_SVE(sve_int_wide_imm_unpredicated_mul) {
  int in_b[] = {11, -1, 7, -3};
  int in_h[] = {111, -1, 17, -123};
  int in_s[] = {11111, -1, 117, -12345};
  int64_t in_d[] = {0x7fffffff, 0x80000000};

  IntWideImmFn fn = &MacroAssembler::Mul;

  int exp_b_1[] = {66, -6, 42, -18};
  int exp_h_1[] = {-14208, 128, -2176, 15744};
  int exp_s_1[] = {11111 * 127, -127, 117 * 127, -12345 * 127};
  int64_t exp_d_1[] = {0xfffffffe, 0x100000000};

  IntWideImmHelper(config, fn, kBRegSize, in_b, 6, exp_b_1);
  IntWideImmHelper(config, fn, kHRegSize, in_h, -128, exp_h_1);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127, exp_s_1);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 2, exp_d_1);

  int exp_h_2[] = {-28305, 255, -4335, 31365};
  int exp_s_2[] = {22755328, -2048, 239616, -25282560};
  int64_t exp_d_2[] = {0x00000063ffffff38, 0x0000006400000000};

  // The immediate is in the range [-128, 127], but the macro is able to
  // synthesise unencodable immediates.
  // B-sized lanes cannot take an immediate out of the range [0, 255].
  IntWideImmHelper(config, fn, kHRegSize, in_h, -255, exp_h_2);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 2048, exp_s_2);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 200, exp_d_2);

  // Integer overflow on multiplication.
  unsigned exp_b_3[] = {0x75, 0x81, 0x79, 0x83};

  IntWideImmHelper(config, fn, kBRegSize, in_b, 0x7f, exp_b_3);
}

TEST_SVE(sve_int_wide_imm_unpredicated_add) {
  unsigned in_b[] = {0x81, 0x7f, 0x10, 0xff};
  unsigned in_h[] = {0x8181, 0x7f7f, 0x1010, 0xaaaa};
  unsigned in_s[] = {0x80018181, 0x7fff7f7f, 0xaaaaaaaa, 0xf000f0f0};
  uint64_t in_d[] = {0x8000000180018181, 0x7fffffff7fff7f7f};

  IntWideImmFn fn = &MacroAssembler::Add;

  unsigned exp_b_1[] = {0x02, 0x00, 0x91, 0x80};
  unsigned exp_h_1[] = {0x8191, 0x7f8f, 0x1020, 0xaaba};
  unsigned exp_s_1[] = {0x80018200, 0x7fff7ffe, 0xaaaaab29, 0xf000f16f};
  uint64_t exp_d_1[] = {0x8000000180018280, 0x7fffffff7fff807e};

  // Encodable with `add` (shift 0).
  IntWideImmHelper(config, fn, kBRegSize, in_b, 0x81, exp_b_1);
  IntWideImmHelper(config, fn, kHRegSize, in_h, 16, exp_h_1);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127, exp_s_1);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 0xff, exp_d_1);

  unsigned exp_h_2[] = {0x9181, 0x8f7f, 0x2010, 0xbaaa};
  unsigned exp_s_2[] = {0x80020081, 0x7ffffe7f, 0xaaab29aa, 0xf0016ff0};
  uint64_t exp_d_2[] = {0x8000000180028081, 0x7fffffff80007e7f};

  // Encodable with `add` (shift 8).
  // B-sized lanes cannot take a shift of 8.
  IntWideImmHelper(config, fn, kHRegSize, in_h, 16 << 8, exp_h_2);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127 << 8, exp_s_2);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 0xff << 8, exp_d_2);

  unsigned exp_s_3[] = {0x80808181, 0x807e7f7f, 0xab29aaaa, 0xf07ff0f0};

  // The macro is able to synthesise unencodable immediates.
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127 << 16, exp_s_3);

  unsigned exp_b_4[] = {0x61, 0x5f, 0xf0, 0xdf};
  unsigned exp_h_4[] = {0x6181, 0x5f7f, 0xf010, 0x8aaa};
  unsigned exp_s_4[] = {0x00018181, 0xffff7f7f, 0x2aaaaaaa, 0x7000f0f0};
  uint64_t exp_d_4[] = {0x8000000180018180, 0x7fffffff7fff7f7e};

  // Negative immediates use `sub`.
  IntWideImmHelper(config, fn, kBRegSize, in_b, -0x20, exp_b_4);
  IntWideImmHelper(config, fn, kHRegSize, in_h, -0x2000, exp_h_4);
  IntWideImmHelper(config, fn, kSRegSize, in_s, INT32_MIN, exp_s_4);
  IntWideImmHelper(config, fn, kDRegSize, in_d, -1, exp_d_4);
}

TEST_SVE(sve_int_wide_imm_unpredicated_sqadd) {
  unsigned in_b[] = {0x81, 0x7f, 0x10, 0xff};
  unsigned in_h[] = {0x8181, 0x7f7f, 0x1010, 0xaaaa};
  unsigned in_s[] = {0x80018181, 0x7fff7f7f, 0xaaaaaaaa, 0xf000f0f0};
  uint64_t in_d[] = {0x8000000180018181, 0x7fffffff7fff7f7f};

  IntWideImmFn fn = &MacroAssembler::Sqadd;

  unsigned exp_b_1[] = {0x02, 0x7f, 0x7f, 0x7f};
  unsigned exp_h_1[] = {0x8191, 0x7f8f, 0x1020, 0xaaba};
  unsigned exp_s_1[] = {0x80018200, 0x7fff7ffe, 0xaaaaab29, 0xf000f16f};
  uint64_t exp_d_1[] = {0x8000000180018280, 0x7fffffff7fff807e};

  // Encodable with `sqadd` (shift 0).
  // Note that encodable immediates are unsigned, even for signed saturation.
  IntWideImmHelper(config, fn, kBRegSize, in_b, 129, exp_b_1);
  IntWideImmHelper(config, fn, kHRegSize, in_h, 16, exp_h_1);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127, exp_s_1);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 255, exp_d_1);

  unsigned exp_h_2[] = {0x9181, 0x7fff, 0x2010, 0xbaaa};
  unsigned exp_s_2[] = {0x80020081, 0x7ffffe7f, 0xaaab29aa, 0xf0016ff0};
  uint64_t exp_d_2[] = {0x8000000180028081, 0x7fffffff80007e7f};

  // Encodable with `sqadd` (shift 8).
  // B-sized lanes cannot take a shift of 8.
  IntWideImmHelper(config, fn, kHRegSize, in_h, 16 << 8, exp_h_2);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127 << 8, exp_s_2);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 0xff << 8, exp_d_2);
}

TEST_SVE(sve_int_wide_imm_unpredicated_uqadd) {
  unsigned in_b[] = {0x81, 0x7f, 0x10, 0xff};
  unsigned in_h[] = {0x8181, 0x7f7f, 0x1010, 0xaaaa};
  unsigned in_s[] = {0x80018181, 0x7fff7f7f, 0xaaaaaaaa, 0xf000f0f0};
  uint64_t in_d[] = {0x8000000180018181, 0x7fffffff7fff7f7f};

  IntWideImmFn fn = &MacroAssembler::Uqadd;

  unsigned exp_b_1[] = {0xff, 0xff, 0x91, 0xff};
  unsigned exp_h_1[] = {0x8191, 0x7f8f, 0x1020, 0xaaba};
  unsigned exp_s_1[] = {0x80018200, 0x7fff7ffe, 0xaaaaab29, 0xf000f16f};
  uint64_t exp_d_1[] = {0x8000000180018280, 0x7fffffff7fff807e};

  // Encodable with `uqadd` (shift 0).
  IntWideImmHelper(config, fn, kBRegSize, in_b, 0x81, exp_b_1);
  IntWideImmHelper(config, fn, kHRegSize, in_h, 16, exp_h_1);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127, exp_s_1);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 0xff, exp_d_1);

  unsigned exp_h_2[] = {0x9181, 0x8f7f, 0x2010, 0xbaaa};
  unsigned exp_s_2[] = {0x80020081, 0x7ffffe7f, 0xaaab29aa, 0xf0016ff0};
  uint64_t exp_d_2[] = {0x8000000180028081, 0x7fffffff80007e7f};

  // Encodable with `uqadd` (shift 8).
  // B-sized lanes cannot take a shift of 8.
  IntWideImmHelper(config, fn, kHRegSize, in_h, 16 << 8, exp_h_2);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127 << 8, exp_s_2);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 0xff << 8, exp_d_2);
}

TEST_SVE(sve_int_wide_imm_unpredicated_sub) {
  unsigned in_b[] = {0x81, 0x7f, 0x10, 0xff};
  unsigned in_h[] = {0x8181, 0x7f7f, 0x1010, 0xaaaa};
  unsigned in_s[] = {0x80018181, 0x7fff7f7f, 0xaaaaaaaa, 0xf000f0f0};
  uint64_t in_d[] = {0x8000000180018181, 0x7fffffff7fff7f7f};

  IntWideImmFn fn = &MacroAssembler::Sub;

  unsigned exp_b_1[] = {0x00, 0xfe, 0x8f, 0x7e};
  unsigned exp_h_1[] = {0x8171, 0x7f6f, 0x1000, 0xaa9a};
  unsigned exp_s_1[] = {0x80018102, 0x7fff7f00, 0xaaaaaa2b, 0xf000f071};
  uint64_t exp_d_1[] = {0x8000000180018082, 0x7fffffff7fff7e80};

  // Encodable with `sub` (shift 0).
  IntWideImmHelper(config, fn, kBRegSize, in_b, 0x81, exp_b_1);
  IntWideImmHelper(config, fn, kHRegSize, in_h, 16, exp_h_1);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127, exp_s_1);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 0xff, exp_d_1);

  unsigned exp_h_2[] = {0x7181, 0x6f7f, 0x0010, 0x9aaa};
  unsigned exp_s_2[] = {0x80010281, 0x7fff007f, 0xaaaa2baa, 0xf00071f0};
  uint64_t exp_d_2[] = {0x8000000180008281, 0x7fffffff7ffe807f};

  // Encodable with `sub` (shift 8).
  // B-sized lanes cannot take a shift of 8.
  IntWideImmHelper(config, fn, kHRegSize, in_h, 16 << 8, exp_h_2);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127 << 8, exp_s_2);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 0xff << 8, exp_d_2);

  unsigned exp_s_3[] = {0x7f828181, 0x7f807f7f, 0xaa2baaaa, 0xef81f0f0};

  // The macro is able to synthesise unencodable immediates.
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127 << 16, exp_s_3);

  unsigned exp_b_4[] = {0xa1, 0x9f, 0x30, 0x1f};
  unsigned exp_h_4[] = {0xa181, 0x9f7f, 0x3010, 0xcaaa};
  unsigned exp_s_4[] = {0x00018181, 0xffff7f7f, 0x2aaaaaaa, 0x7000f0f0};
  uint64_t exp_d_4[] = {0x8000000180018182, 0x7fffffff7fff7f80};

  // Negative immediates use `add`.
  IntWideImmHelper(config, fn, kBRegSize, in_b, -0x20, exp_b_4);
  IntWideImmHelper(config, fn, kHRegSize, in_h, -0x2000, exp_h_4);
  IntWideImmHelper(config, fn, kSRegSize, in_s, INT32_MIN, exp_s_4);
  IntWideImmHelper(config, fn, kDRegSize, in_d, -1, exp_d_4);
}

TEST_SVE(sve_int_wide_imm_unpredicated_sqsub) {
  unsigned in_b[] = {0x81, 0x7f, 0x10, 0xff};
  unsigned in_h[] = {0x8181, 0x7f7f, 0x1010, 0xaaaa};
  unsigned in_s[] = {0x80018181, 0x7fff7f7f, 0xaaaaaaaa, 0xf000f0f0};
  uint64_t in_d[] = {0x8000000180018181, 0x7fffffff7fff7f7f};

  IntWideImmFn fn = &MacroAssembler::Sqsub;

  unsigned exp_b_1[] = {0x80, 0xfe, 0x8f, 0x80};
  unsigned exp_h_1[] = {0x8171, 0x7f6f, 0x1000, 0xaa9a};
  unsigned exp_s_1[] = {0x80018102, 0x7fff7f00, 0xaaaaaa2b, 0xf000f071};
  uint64_t exp_d_1[] = {0x8000000180018082, 0x7fffffff7fff7e80};

  // Encodable with `sqsub` (shift 0).
  // Note that encodable immediates are unsigned, even for signed saturation.
  IntWideImmHelper(config, fn, kBRegSize, in_b, 129, exp_b_1);
  IntWideImmHelper(config, fn, kHRegSize, in_h, 16, exp_h_1);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127, exp_s_1);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 255, exp_d_1);

  unsigned exp_h_2[] = {0x8000, 0x6f7f, 0x0010, 0x9aaa};
  unsigned exp_s_2[] = {0x80010281, 0x7fff007f, 0xaaaa2baa, 0xf00071f0};
  uint64_t exp_d_2[] = {0x8000000180008281, 0x7fffffff7ffe807f};

  // Encodable with `sqsub` (shift 8).
  // B-sized lanes cannot take a shift of 8.
  IntWideImmHelper(config, fn, kHRegSize, in_h, 16 << 8, exp_h_2);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127 << 8, exp_s_2);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 0xff << 8, exp_d_2);
}

TEST_SVE(sve_int_wide_imm_unpredicated_uqsub) {
  unsigned in_b[] = {0x81, 0x7f, 0x10, 0xff};
  unsigned in_h[] = {0x8181, 0x7f7f, 0x1010, 0xaaaa};
  unsigned in_s[] = {0x80018181, 0x7fff7f7f, 0xaaaaaaaa, 0xf000f0f0};
  uint64_t in_d[] = {0x8000000180018181, 0x7fffffff7fff7f7f};

  IntWideImmFn fn = &MacroAssembler::Uqsub;

  unsigned exp_b_1[] = {0x00, 0x00, 0x00, 0x7e};
  unsigned exp_h_1[] = {0x8171, 0x7f6f, 0x1000, 0xaa9a};
  unsigned exp_s_1[] = {0x80018102, 0x7fff7f00, 0xaaaaaa2b, 0xf000f071};
  uint64_t exp_d_1[] = {0x8000000180018082, 0x7fffffff7fff7e80};

  // Encodable with `uqsub` (shift 0).
  IntWideImmHelper(config, fn, kBRegSize, in_b, 0x81, exp_b_1);
  IntWideImmHelper(config, fn, kHRegSize, in_h, 16, exp_h_1);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127, exp_s_1);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 0xff, exp_d_1);

  unsigned exp_h_2[] = {0x7181, 0x6f7f, 0x0010, 0x9aaa};
  unsigned exp_s_2[] = {0x80010281, 0x7fff007f, 0xaaaa2baa, 0xf00071f0};
  uint64_t exp_d_2[] = {0x8000000180008281, 0x7fffffff7ffe807f};

  // Encodable with `uqsub` (shift 8).
  // B-sized lanes cannot take a shift of 8.
  IntWideImmHelper(config, fn, kHRegSize, in_h, 16 << 8, exp_h_2);
  IntWideImmHelper(config, fn, kSRegSize, in_s, 127 << 8, exp_s_2);
  IntWideImmHelper(config, fn, kDRegSize, in_d, 0xff << 8, exp_d_2);
}

TEST_SVE(sve_int_wide_imm_unpredicated_subr) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Encodable with `subr` (shift 0).
  __ Index(z0.VnD(), 1, 1);
  __ Sub(z0.VnD(), 100, z0.VnD());
  __ Index(z1.VnS(), 0x7f, 1);
  __ Sub(z1.VnS(), 0xf7, z1.VnS());
  __ Index(z2.VnH(), 0xaaaa, 0x2222);
  __ Sub(z2.VnH(), 0x80, z2.VnH());
  __ Index(z3.VnB(), 133, 1);
  __ Sub(z3.VnB(), 255, z3.VnB());

  // Encodable with `subr` (shift 8).
  __ Index(z4.VnD(), 256, -1);
  __ Sub(z4.VnD(), 42 * 256, z4.VnD());
  __ Index(z5.VnS(), 0x7878, 1);
  __ Sub(z5.VnS(), 0x8000, z5.VnS());
  __ Index(z6.VnH(), 0x30f0, -1);
  __ Sub(z6.VnH(), 0x7f00, z6.VnH());
  // B-sized lanes cannot take a shift of 8.

  // Select with movprfx.
  __ Index(z31.VnD(), 256, 4001);
  __ Sub(z7.VnD(), 42 * 256, z31.VnD());

  // Out of immediate encodable range of `sub`.
  __ Index(z30.VnS(), 0x11223344, 1);
  __ Sub(z8.VnS(), 0x88776655, z30.VnS());

  END();

  if (CAN_RUN()) {
    RUN();

    int expected_z0[] = {87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99};
    ASSERT_EQUAL_SVE(expected_z0, z0.VnD());

    int expected_z1[] = {0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78};
    ASSERT_EQUAL_SVE(expected_z1, z1.VnS());

    int expected_z2[] = {0xab2c, 0xcd4e, 0xef70, 0x1192, 0x33b4, 0x55d6};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnH());

    int expected_z3[] = {0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnB());

    int expected_z4[] = {10502, 10501, 10500, 10499, 10498, 10497, 10496};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());

    int expected_z5[] = {0x0783, 0x0784, 0x0785, 0x0786, 0x0787, 0x0788};
    ASSERT_EQUAL_SVE(expected_z5, z5.VnS());

    int expected_z6[] = {0x4e15, 0x4e14, 0x4e13, 0x4e12, 0x4e11, 0x4e10};
    ASSERT_EQUAL_SVE(expected_z6, z6.VnH());

    int expected_z7[] = {-13510, -9509, -5508, -1507, 2494, 6495, 10496};
    ASSERT_EQUAL_SVE(expected_z7, z7.VnD());

    int expected_z8[] = {0x7755330e, 0x7755330f, 0x77553310, 0x77553311};
    ASSERT_EQUAL_SVE(expected_z8, z8.VnS());
  }
}

TEST_SVE(sve_int_wide_imm_unpredicated_fdup) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Immediates which can be encoded in the instructions.
  __ Fdup(z0.VnH(), RawbitsToFloat16(0xc500));
  __ Fdup(z1.VnS(), Float16(2.0));
  __ Fdup(z2.VnD(), Float16(3.875));
  __ Fdup(z3.VnH(), 8.0f);
  __ Fdup(z4.VnS(), -4.75f);
  __ Fdup(z5.VnD(), 0.5f);
  __ Fdup(z6.VnH(), 1.0);
  __ Fdup(z7.VnS(), 2.125);
  __ Fdup(z8.VnD(), -13.0);

  // Immediates which cannot be encoded in the instructions.
  __ Fdup(z10.VnH(), Float16(0.0));
  __ Fdup(z11.VnH(), kFP16PositiveInfinity);
  __ Fdup(z12.VnS(), 255.0f);
  __ Fdup(z13.VnS(), kFP32NegativeInfinity);
  __ Fdup(z14.VnD(), 12.3456);
  __ Fdup(z15.VnD(), kFP64PositiveInfinity);

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(0xc500, z0.VnH());
    ASSERT_EQUAL_SVE(0x40000000, z1.VnS());
    ASSERT_EQUAL_SVE(0x400f000000000000, z2.VnD());
    ASSERT_EQUAL_SVE(0x4800, z3.VnH());
    ASSERT_EQUAL_SVE(FloatToRawbits(-4.75f), z4.VnS());
    ASSERT_EQUAL_SVE(DoubleToRawbits(0.5), z5.VnD());
    ASSERT_EQUAL_SVE(0x3c00, z6.VnH());
    ASSERT_EQUAL_SVE(FloatToRawbits(2.125f), z7.VnS());
    ASSERT_EQUAL_SVE(DoubleToRawbits(-13.0), z8.VnD());

    ASSERT_EQUAL_SVE(0x0000, z10.VnH());
    ASSERT_EQUAL_SVE(Float16ToRawbits(kFP16PositiveInfinity), z11.VnH());
    ASSERT_EQUAL_SVE(FloatToRawbits(255.0), z12.VnS());
    ASSERT_EQUAL_SVE(FloatToRawbits(kFP32NegativeInfinity), z13.VnS());
    ASSERT_EQUAL_SVE(DoubleToRawbits(12.3456), z14.VnD());
    ASSERT_EQUAL_SVE(DoubleToRawbits(kFP64PositiveInfinity), z15.VnD());
  }
}

TEST_SVE(sve_andv_eorv_orv) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in[] = {0x8899aabbccddeeff, 0x7777555533331111, 0x123456789abcdef0};
  InsrHelper(&masm, z31.VnD(), in);

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         1,                      1,                      0
  // For S lanes:         1,          1,          1,          0,          0
  // For H lanes:   0,    1,    0,    1,    1,    1,    0,    0,    1,    0
  int pg_in[] = {1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0};
  Initialise(&masm, p0.VnB(), pg_in);

  // Make a copy so we can check that constructive operations preserve zn.
  __ Mov(z0, z31);
  __ Andv(b0, p0, z0.VnB());  // destructive
  __ Andv(h1, p0, z31.VnH());
  __ Mov(z2, z31);
  __ Andv(s2, p0, z2.VnS());  // destructive
  __ Andv(d3, p0, z31.VnD());

  __ Eorv(b4, p0, z31.VnB());
  __ Mov(z5, z31);
  __ Eorv(h5, p0, z5.VnH());  // destructive
  __ Eorv(s6, p0, z31.VnS());
  __ Mov(z7, z31);
  __ Eorv(d7, p0, z7.VnD());  // destructive

  __ Mov(z8, z31);
  __ Orv(b8, p0, z8.VnB());  // destructive
  __ Orv(h9, p0, z31.VnH());
  __ Mov(z10, z31);
  __ Orv(s10, p0, z10.VnS());  // destructive
  __ Orv(d11, p0, z31.VnD());

  END();

  if (CAN_RUN()) {
    RUN();

    if (static_cast<int>(ArrayLength(pg_in)) >= config->sve_vl_in_bytes()) {
      ASSERT_EQUAL_64(0x10, d0);
      ASSERT_EQUAL_64(0x1010, d1);
      ASSERT_EQUAL_64(0x33331111, d2);
      ASSERT_EQUAL_64(0x7777555533331111, d3);
      ASSERT_EQUAL_64(0xbf, d4);
      ASSERT_EQUAL_64(0xedcb, d5);
      ASSERT_EQUAL_64(0x44444444, d6);
      ASSERT_EQUAL_64(0x7777555533331111, d7);
      ASSERT_EQUAL_64(0xff, d8);
      ASSERT_EQUAL_64(0xffff, d9);
      ASSERT_EQUAL_64(0x77775555, d10);
      ASSERT_EQUAL_64(0x7777555533331111, d11);
    } else {
      ASSERT_EQUAL_64(0, d0);
      ASSERT_EQUAL_64(0x0010, d1);
      ASSERT_EQUAL_64(0x00110011, d2);
      ASSERT_EQUAL_64(0x0011001100110011, d3);
      ASSERT_EQUAL_64(0x62, d4);
      ASSERT_EQUAL_64(0x0334, d5);
      ASSERT_EQUAL_64(0x8899aabb, d6);
      ASSERT_EQUAL_64(0xffeeffeeffeeffee, d7);
      ASSERT_EQUAL_64(0xff, d8);
      ASSERT_EQUAL_64(0xffff, d9);
      ASSERT_EQUAL_64(0xffffffff, d10);
      ASSERT_EQUAL_64(0xffffffffffffffff, d11);
    }

    // Check the upper lanes above the top of the V register are all clear.
    for (int i = 1; i < core.GetSVELaneCount(kDRegSize); i++) {
      ASSERT_EQUAL_SVE_LANE(0, z0.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z1.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z2.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z3.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z4.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z5.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z6.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z7.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z8.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z9.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z10.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z11.VnD(), i);
    }
  }
}


TEST_SVE(sve_saddv_uaddv) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in[] = {0x8899aabbccddeeff, 0x8182838485868788, 0x0807060504030201};
  InsrHelper(&masm, z31.VnD(), in);

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         1,                      1,                      0
  // For S lanes:         1,          1,          1,          0,          0
  // For H lanes:   0,    1,    0,    1,    1,    1,    0,    0,    1,    0
  int pg_in[] = {1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0};
  Initialise(&masm, p0.VnB(), pg_in);

  // Make a copy so we can check that constructive operations preserve zn.
  __ Mov(z0, z31);
  __ Saddv(b0, p0, z0.VnB());  // destructive
  __ Saddv(h1, p0, z31.VnH());
  __ Mov(z2, z31);
  __ Saddv(s2, p0, z2.VnS());  // destructive

  __ Uaddv(b4, p0, z31.VnB());
  __ Mov(z5, z31);
  __ Uaddv(h5, p0, z5.VnH());  // destructive
  __ Uaddv(s6, p0, z31.VnS());
  __ Mov(z7, z31);
  __ Uaddv(d7, p0, z7.VnD());  // destructive

  END();

  if (CAN_RUN()) {
    RUN();

    if (static_cast<int>(ArrayLength(pg_in)) >= config->sve_vl_in_bytes()) {
      // Saddv
      ASSERT_EQUAL_64(0xfffffffffffffda9, d0);
      ASSERT_EQUAL_64(0xfffffffffffe9495, d1);
      ASSERT_EQUAL_64(0xffffffff07090b0c, d2);
      // Uaddv
      ASSERT_EQUAL_64(0x00000000000002a9, d4);
      ASSERT_EQUAL_64(0x0000000000019495, d5);
      ASSERT_EQUAL_64(0x0000000107090b0c, d6);
      ASSERT_EQUAL_64(0x8182838485868788, d7);
    } else {
      // Saddv
      ASSERT_EQUAL_64(0xfffffffffffffd62, d0);
      ASSERT_EQUAL_64(0xfffffffffffe8394, d1);
      ASSERT_EQUAL_64(0xfffffffed3e6fa0b, d2);
      // Uaddv
      ASSERT_EQUAL_64(0x0000000000000562, d4);
      ASSERT_EQUAL_64(0x0000000000028394, d5);
      ASSERT_EQUAL_64(0x00000001d3e6fa0b, d6);
      ASSERT_EQUAL_64(0x0a1c2e4052647687, d7);
    }

    // Check the upper lanes above the top of the V register are all clear.
    for (int i = 1; i < core.GetSVELaneCount(kDRegSize); i++) {
      ASSERT_EQUAL_SVE_LANE(0, z0.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z1.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z2.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z4.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z5.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z6.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z7.VnD(), i);
    }
  }
}


TEST_SVE(sve_sminv_uminv) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in[] = {0xfffa5555aaaaaaaa, 0x0011223344aafe80, 0x00112233aabbfc00};
  InsrHelper(&masm, z31.VnD(), in);

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         1,                      0,                      1
  // For S lanes:         1,          1,          0,          0,          1
  // For H lanes:   1,    1,    0,    1,    1,    0,    0,    0,    1,    1
  int pg_in[] = {1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1};
  Initialise(&masm, p0.VnB(), pg_in);

  // Make a copy so we can check that constructive operations preserve zn.
  __ Mov(z0, z31);
  __ Sminv(b0, p0, z0.VnB());  // destructive
  __ Sminv(h1, p0, z31.VnH());
  __ Mov(z2, z31);
  __ Sminv(s2, p0, z2.VnS());  // destructive
  __ Sminv(d3, p0, z31.VnD());

  __ Uminv(b4, p0, z31.VnB());
  __ Mov(z5, z31);
  __ Uminv(h5, p0, z5.VnH());  // destructive
  __ Uminv(s6, p0, z31.VnS());
  __ Mov(z7, z31);
  __ Uminv(d7, p0, z7.VnD());  // destructive

  END();

  if (CAN_RUN()) {
    RUN();

    if (static_cast<int>(ArrayLength(pg_in)) >= config->sve_vl_in_bytes()) {
      // Sminv
      ASSERT_EQUAL_64(0xaa, d0);
      ASSERT_EQUAL_64(0xaabb, d1);
      ASSERT_EQUAL_64(0xaabbfc00, d2);
      ASSERT_EQUAL_64(0x00112233aabbfc00, d3);  // The smaller lane is inactive.
      // Uminv
      ASSERT_EQUAL_64(0, d4);
      ASSERT_EQUAL_64(0x2233, d5);
      ASSERT_EQUAL_64(0x112233, d6);
      ASSERT_EQUAL_64(0x00112233aabbfc00, d7);  // The smaller lane is inactive.
    } else {
      // Sminv
      ASSERT_EQUAL_64(0xaa, d0);
      ASSERT_EQUAL_64(0xaaaa, d1);
      ASSERT_EQUAL_64(0xaaaaaaaa, d2);
      ASSERT_EQUAL_64(0xfffa5555aaaaaaaa, d3);
      // Uminv
      ASSERT_EQUAL_64(0, d4);
      ASSERT_EQUAL_64(0x2233, d5);
      ASSERT_EQUAL_64(0x112233, d6);
      ASSERT_EQUAL_64(0x00112233aabbfc00, d7);
    }

    // Check the upper lanes above the top of the V register are all clear.
    for (int i = 1; i < core.GetSVELaneCount(kDRegSize); i++) {
      ASSERT_EQUAL_SVE_LANE(0, z0.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z1.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z2.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z3.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z4.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z5.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z6.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z7.VnD(), i);
    }
  }
}

TEST_SVE(sve_smaxv_umaxv) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in[] = {0xfffa5555aaaaaaaa, 0x0011223344aafe80, 0x00112233aabbfc00};
  InsrHelper(&masm, z31.VnD(), in);

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         1,                      0,                      1
  // For S lanes:         1,          1,          0,          0,          1
  // For H lanes:   1,    1,    0,    1,    1,    0,    0,    0,    1,    1
  int pg_in[] = {1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1};
  Initialise(&masm, p0.VnB(), pg_in);

  // Make a copy so we can check that constructive operations preserve zn.
  __ Mov(z0, z31);
  __ Smaxv(b0, p0, z0.VnB());  // destructive
  __ Smaxv(h1, p0, z31.VnH());
  __ Mov(z2, z31);
  __ Smaxv(s2, p0, z2.VnS());  // destructive
  __ Smaxv(d3, p0, z31.VnD());

  __ Umaxv(b4, p0, z31.VnB());
  __ Mov(z5, z31);
  __ Umaxv(h5, p0, z5.VnH());  // destructive
  __ Umaxv(s6, p0, z31.VnS());
  __ Mov(z7, z31);
  __ Umaxv(d7, p0, z7.VnD());  // destructive

  END();

  if (CAN_RUN()) {
    RUN();

    if (static_cast<int>(ArrayLength(pg_in)) >= config->sve_vl_in_bytes()) {
      // Smaxv
      ASSERT_EQUAL_64(0x33, d0);
      ASSERT_EQUAL_64(0x44aa, d1);
      ASSERT_EQUAL_64(0x112233, d2);
      ASSERT_EQUAL_64(0x112233aabbfc00, d3);
      // Umaxv
      ASSERT_EQUAL_64(0xfe, d4);
      ASSERT_EQUAL_64(0xfc00, d5);
      ASSERT_EQUAL_64(0xaabbfc00, d6);
      ASSERT_EQUAL_64(0x112233aabbfc00, d7);
    } else {
      // Smaxv
      ASSERT_EQUAL_64(0x33, d0);
      ASSERT_EQUAL_64(0x44aa, d1);
      ASSERT_EQUAL_64(0x112233, d2);
      ASSERT_EQUAL_64(0x00112233aabbfc00, d3);
      // Umaxv
      ASSERT_EQUAL_64(0xfe, d4);
      ASSERT_EQUAL_64(0xfc00, d5);
      ASSERT_EQUAL_64(0xaabbfc00, d6);
      ASSERT_EQUAL_64(0xfffa5555aaaaaaaa, d7);
    }

    // Check the upper lanes above the top of the V register are all clear.
    for (int i = 1; i < core.GetSVELaneCount(kDRegSize); i++) {
      ASSERT_EQUAL_SVE_LANE(0, z0.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z1.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z2.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z3.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z4.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z5.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z6.VnD(), i);
      ASSERT_EQUAL_SVE_LANE(0, z7.VnD(), i);
    }
  }
}

typedef void (MacroAssembler::*SdotUdotFn)(const ZRegister& zd,
                                           const ZRegister& za,
                                           const ZRegister& zn,
                                           const ZRegister& zm);

template <typename Td, typename Ts, typename Te>
static void SdotUdotHelper(Test* config,
                           SdotUdotFn macro,
                           unsigned lane_size_in_bits,
                           const Td& zd_inputs,
                           const Td& za_inputs,
                           const Ts& zn_inputs,
                           const Ts& zm_inputs,
                           const Te& zd_expected,
                           const Te& zdnm_expected) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  ZRegister zd = z0.WithLaneSize(lane_size_in_bits);
  ZRegister za = z1.WithLaneSize(lane_size_in_bits);
  ZRegister zn = z2.WithLaneSize(lane_size_in_bits / 4);
  ZRegister zm = z3.WithLaneSize(lane_size_in_bits / 4);

  InsrHelper(&masm, zd, zd_inputs);
  InsrHelper(&masm, za, za_inputs);
  InsrHelper(&masm, zn, zn_inputs);
  InsrHelper(&masm, zm, zm_inputs);

  // The Dot macro handles arbitrarily-aliased registers in the argument list.
  ZRegister da_result = z10.WithLaneSize(lane_size_in_bits);
  ZRegister dn_result = z11.WithLaneSize(lane_size_in_bits);
  ZRegister dm_result = z12.WithLaneSize(lane_size_in_bits);
  ZRegister dnm_result = z13.WithLaneSize(lane_size_in_bits);
  ZRegister d_result = z14.WithLaneSize(lane_size_in_bits);

  __ Mov(da_result, za);
  // zda = zda + (zn . zm)
  (masm.*macro)(da_result, da_result, zn, zm);

  __ Mov(dn_result, zn);
  // zdn = za + (zdn . zm)
  (masm.*macro)(dn_result, za, dn_result.WithSameLaneSizeAs(zn), zm);

  __ Mov(dm_result, zm);
  // zdm = za + (zn . zdm)
  (masm.*macro)(dm_result, za, zn, dm_result.WithSameLaneSizeAs(zm));

  __ Mov(d_result, zd);
  // zd = za + (zn . zm)
  (masm.*macro)(d_result, za, zn, zm);

  __ Mov(dnm_result, zn);
  // zdnm = za + (zdmn . zdnm)
  (masm.*macro)(dnm_result,
                za,
                dnm_result.WithSameLaneSizeAs(zn),
                dnm_result.WithSameLaneSizeAs(zm));

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(za_inputs, z1.WithLaneSize(lane_size_in_bits));
    ASSERT_EQUAL_SVE(zn_inputs, z2.WithLaneSize(lane_size_in_bits / 4));
    ASSERT_EQUAL_SVE(zm_inputs, z3.WithLaneSize(lane_size_in_bits / 4));

    ASSERT_EQUAL_SVE(zd_expected, da_result);
    ASSERT_EQUAL_SVE(zd_expected, dn_result);
    ASSERT_EQUAL_SVE(zd_expected, dm_result);
    ASSERT_EQUAL_SVE(zd_expected, d_result);

    ASSERT_EQUAL_SVE(zdnm_expected, dnm_result);
  }
}

TEST_SVE(sve_sdot) {
  int zd_inputs[] = {0x33, 0xee, 0xff};
  int za_inputs[] = {INT32_MAX, -3, 2};
  int zn_inputs[] = {-128, -128, -128, -128, 9, -1, 1, 30, -5, -20, 9, 8};
  int zm_inputs[] = {-128, -128, -128, -128, -19, 15, 6, 0, 9, -5, 4, 5};

  // zd_expected[] = za_inputs[] + (zn_inputs[] . zm_inputs[])
  int32_t zd_expected_s[] = {-2147418113, -183, 133};  // 0x8000ffff
  int64_t zd_expected_d[] = {2147549183, -183, 133};   // 0x8000ffff

  // zdnm_expected[] = za_inputs[] + (zn_inputs[] . zn_inputs[])
  int32_t zdnm_expected_s[] = {-2147418113, 980, 572};
  int64_t zdnm_expected_d[] = {2147549183, 980, 572};

  SdotUdotHelper(config,
                 &MacroAssembler::Sdot,
                 kSRegSize,
                 zd_inputs,
                 za_inputs,
                 zn_inputs,
                 zm_inputs,
                 zd_expected_s,
                 zdnm_expected_s);
  SdotUdotHelper(config,
                 &MacroAssembler::Sdot,
                 kDRegSize,
                 zd_inputs,
                 za_inputs,
                 zn_inputs,
                 zm_inputs,
                 zd_expected_d,
                 zdnm_expected_d);
}

TEST_SVE(sve_udot) {
  int zd_inputs[] = {0x33, 0xee, 0xff};
  int za_inputs[] = {INT32_MAX, -3, 2};
  int zn_inputs[] = {-128, -128, -128, -128, 9, -1, 1, 30, -5, -20, 9, 8};
  int zm_inputs[] = {-128, -128, -128, -128, -19, 15, 6, 0, 9, -5, 4, 5};

  // zd_expected[] = za_inputs[] + (zn_inputs[] . zm_inputs[])
  uint32_t zd_expected_s[] = {0x8000ffff, 0x00001749, 0x0000f085};
  uint64_t zd_expected_d[] = {0x000000047c00ffff,
                              0x000000000017ff49,
                              0x00000000fff00085};

  // zdnm_expected[] = za_inputs[] + (zn_inputs[] . zn_inputs[])
  uint32_t zdnm_expected_s[] = {0x8000ffff, 0x000101d4, 0x0001d03c};
  uint64_t zdnm_expected_d[] = {0x000000047c00ffff,
                                0x00000000fffe03d4,
                                0x00000001ffce023c};

  SdotUdotHelper(config,
                 &MacroAssembler::Udot,
                 kSRegSize,
                 zd_inputs,
                 za_inputs,
                 zn_inputs,
                 zm_inputs,
                 zd_expected_s,
                 zdnm_expected_s);
  SdotUdotHelper(config,
                 &MacroAssembler::Udot,
                 kDRegSize,
                 zd_inputs,
                 za_inputs,
                 zn_inputs,
                 zm_inputs,
                 zd_expected_d,
                 zdnm_expected_d);
}

template <typename T, size_t N>
static void FPToRawbitsWithSize(const T (&inputs)[N],
                                uint64_t* outputs,
                                unsigned size_in_bits) {
  for (size_t i = 0; i < N; i++) {
    outputs[i] = vixl::FPToRawbitsWithSize(size_in_bits, inputs[i]);
  }
}

template <typename Ti, typename Te, size_t N>
static void FPBinArithHelper(Test* config,
                             ArithFn macro,
                             int lane_size_in_bits,
                             const Ti (&zn_inputs)[N],
                             const Ti (&zm_inputs)[N],
                             const Te (&zd_expected)[N]) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);

  START();

  ZRegister zd = z29.WithLaneSize(lane_size_in_bits);
  ZRegister zn = z30.WithLaneSize(lane_size_in_bits);
  ZRegister zm = z31.WithLaneSize(lane_size_in_bits);

  uint64_t zn_rawbits[N];
  uint64_t zm_rawbits[N];

  FPToRawbitsWithSize(zn_inputs, zn_rawbits, lane_size_in_bits);
  FPToRawbitsWithSize(zm_inputs, zm_rawbits, lane_size_in_bits);

  InsrHelper(&masm, zn, zn_rawbits);
  InsrHelper(&masm, zm, zm_rawbits);

  (masm.*macro)(zd, zn, zm);

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(zd_expected, zd);
  }
}

TEST_SVE(sve_fp_arithmetic_unpredicated_fadd) {
  double zn_inputs[] = {24.0,
                        5.5,
                        0.0,
                        3.875,
                        2.125,
                        kFP64PositiveInfinity,
                        kFP64NegativeInfinity};

  double zm_inputs[] = {1024.0, 2048.0, 0.1, -4.75, 12.34, 255.0, -13.0};

  ArithFn fn = &MacroAssembler::Fadd;

  uint16_t expected_h[] = {Float16ToRawbits(Float16(1048.0)),
                           Float16ToRawbits(Float16(2053.5)),
                           Float16ToRawbits(Float16(0.1)),
                           Float16ToRawbits(Float16(-0.875)),
                           Float16ToRawbits(Float16(14.465)),
                           Float16ToRawbits(kFP16PositiveInfinity),
                           Float16ToRawbits(kFP16NegativeInfinity)};

  FPBinArithHelper(config, fn, kHRegSize, zn_inputs, zm_inputs, expected_h);

  uint32_t expected_s[] = {FloatToRawbits(1048.0f),
                           FloatToRawbits(2053.5f),
                           FloatToRawbits(0.1f),
                           FloatToRawbits(-0.875f),
                           FloatToRawbits(14.465f),
                           FloatToRawbits(kFP32PositiveInfinity),
                           FloatToRawbits(kFP32NegativeInfinity)};

  FPBinArithHelper(config, fn, kSRegSize, zn_inputs, zm_inputs, expected_s);

  uint64_t expected_d[] = {DoubleToRawbits(1048.0),
                           DoubleToRawbits(2053.5),
                           DoubleToRawbits(0.1),
                           DoubleToRawbits(-0.875),
                           DoubleToRawbits(14.465),
                           DoubleToRawbits(kFP64PositiveInfinity),
                           DoubleToRawbits(kFP64NegativeInfinity)};

  FPBinArithHelper(config, fn, kDRegSize, zn_inputs, zm_inputs, expected_d);
}

TEST_SVE(sve_fp_arithmetic_unpredicated_fsub) {
  double zn_inputs[] = {24.0,
                        5.5,
                        0.0,
                        3.875,
                        2.125,
                        kFP64PositiveInfinity,
                        kFP64NegativeInfinity};

  double zm_inputs[] = {1024.0, 2048.0, 0.1, -4.75, 12.34, 255.0, -13.0};

  ArithFn fn = &MacroAssembler::Fsub;

  uint16_t expected_h[] = {Float16ToRawbits(Float16(-1000.0)),
                           Float16ToRawbits(Float16(-2042.5)),
                           Float16ToRawbits(Float16(-0.1)),
                           Float16ToRawbits(Float16(8.625)),
                           Float16ToRawbits(Float16(-10.215)),
                           Float16ToRawbits(kFP16PositiveInfinity),
                           Float16ToRawbits(kFP16NegativeInfinity)};

  FPBinArithHelper(config, fn, kHRegSize, zn_inputs, zm_inputs, expected_h);

  uint32_t expected_s[] = {FloatToRawbits(-1000.0),
                           FloatToRawbits(-2042.5),
                           FloatToRawbits(-0.1),
                           FloatToRawbits(8.625),
                           FloatToRawbits(-10.215),
                           FloatToRawbits(kFP32PositiveInfinity),
                           FloatToRawbits(kFP32NegativeInfinity)};

  FPBinArithHelper(config, fn, kSRegSize, zn_inputs, zm_inputs, expected_s);

  uint64_t expected_d[] = {DoubleToRawbits(-1000.0),
                           DoubleToRawbits(-2042.5),
                           DoubleToRawbits(-0.1),
                           DoubleToRawbits(8.625),
                           DoubleToRawbits(-10.215),
                           DoubleToRawbits(kFP64PositiveInfinity),
                           DoubleToRawbits(kFP64NegativeInfinity)};

  FPBinArithHelper(config, fn, kDRegSize, zn_inputs, zm_inputs, expected_d);
}

TEST_SVE(sve_fp_arithmetic_unpredicated_fmul) {
  double zn_inputs[] = {24.0,
                        5.5,
                        0.0,
                        3.875,
                        2.125,
                        kFP64PositiveInfinity,
                        kFP64NegativeInfinity};

  double zm_inputs[] = {1024.0, 2048.0, 0.1, -4.75, 12.34, 255.0, -13.0};

  ArithFn fn = &MacroAssembler::Fmul;

  uint16_t expected_h[] = {Float16ToRawbits(Float16(24576.0)),
                           Float16ToRawbits(Float16(11264.0)),
                           Float16ToRawbits(Float16(0.0)),
                           Float16ToRawbits(Float16(-18.4)),
                           Float16ToRawbits(Float16(26.23)),
                           Float16ToRawbits(kFP16PositiveInfinity),
                           Float16ToRawbits(kFP16PositiveInfinity)};

  FPBinArithHelper(config, fn, kHRegSize, zn_inputs, zm_inputs, expected_h);

  uint32_t expected_s[] = {FloatToRawbits(24576.0),
                           FloatToRawbits(11264.0),
                           FloatToRawbits(0.0),
                           FloatToRawbits(-18.40625),
                           FloatToRawbits(26.2225),
                           FloatToRawbits(kFP32PositiveInfinity),
                           FloatToRawbits(kFP32PositiveInfinity)};

  FPBinArithHelper(config, fn, kSRegSize, zn_inputs, zm_inputs, expected_s);

  uint64_t expected_d[] = {DoubleToRawbits(24576.0),
                           DoubleToRawbits(11264.0),
                           DoubleToRawbits(0.0),
                           DoubleToRawbits(-18.40625),
                           DoubleToRawbits(26.2225),
                           DoubleToRawbits(kFP64PositiveInfinity),
                           DoubleToRawbits(kFP64PositiveInfinity)};

  FPBinArithHelper(config, fn, kDRegSize, zn_inputs, zm_inputs, expected_d);
}

typedef void (MacroAssembler::*FPArithPredicatedFn)(
    const ZRegister& zd,
    const PRegisterM& pg,
    const ZRegister& zn,
    const ZRegister& zm,
    FPMacroNaNPropagationOption nan_option);

template <typename Ti, typename Te, size_t N>
static void FPBinArithHelper(
    Test* config,
    FPArithPredicatedFn macro,
    unsigned lane_size_in_bits,
    const Ti (&zd_inputs)[N],
    const int (&pg_inputs)[N],
    const Ti (&zn_inputs)[N],
    const Ti (&zm_inputs)[N],
    const Te (&zd_expected)[N],
    FPMacroNaNPropagationOption nan_option = FastNaNPropagation) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Avoid choosing default scratch registers.
  ZRegister zd = z26.WithLaneSize(lane_size_in_bits);
  ZRegister zn = z27.WithLaneSize(lane_size_in_bits);
  ZRegister zm = z28.WithLaneSize(lane_size_in_bits);

  uint64_t zn_inputs_rawbits[N];
  uint64_t zm_inputs_rawbits[N];
  uint64_t zd_inputs_rawbits[N];

  FPToRawbitsWithSize(zn_inputs, zn_inputs_rawbits, lane_size_in_bits);
  FPToRawbitsWithSize(zm_inputs, zm_inputs_rawbits, lane_size_in_bits);
  FPToRawbitsWithSize(zd_inputs, zd_inputs_rawbits, lane_size_in_bits);

  InsrHelper(&masm, zn, zn_inputs_rawbits);
  InsrHelper(&masm, zm, zm_inputs_rawbits);
  InsrHelper(&masm, zd, zd_inputs_rawbits);

  PRegisterWithLaneSize pg = p0.WithLaneSize(lane_size_in_bits);
  Initialise(&masm, pg, pg_inputs);

  // `instr` zdn, pg, zdn, zm
  ZRegister dn_result = z0.WithLaneSize(lane_size_in_bits);
  __ Mov(dn_result, zn);
  (masm.*macro)(dn_result, pg.Merging(), dn_result, zm, nan_option);

  // Based on whether zd and zm registers are aliased, the macro of instructions
  // (`Instr`) swaps the order of operands if it has the commutative property,
  // otherwise, transfer to the reversed `Instr`, such as fdivr.
  // `instr` zdm, pg, zn, zdm
  ZRegister dm_result = z1.WithLaneSize(lane_size_in_bits);
  __ Mov(dm_result, zm);
  (masm.*macro)(dm_result, pg.Merging(), zn, dm_result, nan_option);

  // The macro of instructions (`Instr`) automatically selects between `instr`
  // and movprfx + `instr` based on whether zd and zn registers are aliased.
  // A generated movprfx instruction is predicated that using the same
  // governing predicate register. In order to keep the result constant,
  // initialize the destination register first.
  // `instr` zd, pg, zn, zm
  ZRegister d_result = z2.WithLaneSize(lane_size_in_bits);
  __ Mov(d_result, zd);
  (masm.*macro)(d_result, pg.Merging(), zn, zm, nan_option);

  END();

  if (CAN_RUN()) {
    RUN();

    for (size_t i = 0; i < ArrayLength(zd_expected); i++) {
      int lane = static_cast<int>(ArrayLength(zd_expected) - i - 1);
      if (!core.HasSVELane(dn_result, lane)) break;
      if ((pg_inputs[i] & 1) != 0) {
        ASSERT_EQUAL_SVE_LANE(zd_expected[i], dn_result, lane);
      } else {
        ASSERT_EQUAL_SVE_LANE(zn_inputs_rawbits[i], dn_result, lane);
      }
    }

    for (size_t i = 0; i < ArrayLength(zd_expected); i++) {
      int lane = static_cast<int>(ArrayLength(zd_expected) - i - 1);
      if (!core.HasSVELane(dm_result, lane)) break;
      if ((pg_inputs[i] & 1) != 0) {
        ASSERT_EQUAL_SVE_LANE(zd_expected[i], dm_result, lane);
      } else {
        ASSERT_EQUAL_SVE_LANE(zm_inputs_rawbits[i], dm_result, lane);
      }
    }

    ASSERT_EQUAL_SVE(zd_expected, d_result);
  }
}

TEST_SVE(sve_binary_arithmetic_predicated_fdiv) {
  // The inputs are shared with different precision tests.
  double zd_in[] = {0.1, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9};

  double zn_in[] = {24.0,
                    24.0,
                    -2.0,
                    -2.0,
                    5.5,
                    5.5,
                    kFP64PositiveInfinity,
                    kFP64PositiveInfinity,
                    kFP64NegativeInfinity,
                    kFP64NegativeInfinity};

  double zm_in[] = {-2.0, -2.0, 24.0, 24.0, 0.5, 0.5, 0.65, 0.65, 24.0, 24.0};

  int pg_in[] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};

  uint16_t exp_h[] = {Float16ToRawbits(Float16(0.1)),
                      Float16ToRawbits(Float16(-12.0)),
                      Float16ToRawbits(Float16(2.2)),
                      Float16ToRawbits(Float16(-0.0833)),
                      Float16ToRawbits(Float16(4.4)),
                      Float16ToRawbits(Float16(11.0)),
                      Float16ToRawbits(Float16(6.6)),
                      Float16ToRawbits(kFP16PositiveInfinity),
                      Float16ToRawbits(Float16(8.8)),
                      Float16ToRawbits(kFP16NegativeInfinity)};

  FPBinArithHelper(config,
                   &MacroAssembler::Fdiv,
                   kHRegSize,
                   zd_in,
                   pg_in,
                   zn_in,
                   zm_in,
                   exp_h);

  uint32_t exp_s[] = {FloatToRawbits(0.1),
                      FloatToRawbits(-12.0),
                      FloatToRawbits(2.2),
                      0xbdaaaaab,
                      FloatToRawbits(4.4),
                      FloatToRawbits(11.0),
                      FloatToRawbits(6.6),
                      FloatToRawbits(kFP32PositiveInfinity),
                      FloatToRawbits(8.8),
                      FloatToRawbits(kFP32NegativeInfinity)};

  FPBinArithHelper(config,
                   &MacroAssembler::Fdiv,
                   kSRegSize,
                   zd_in,
                   pg_in,
                   zn_in,
                   zm_in,
                   exp_s);

  uint64_t exp_d[] = {DoubleToRawbits(0.1),
                      DoubleToRawbits(-12.0),
                      DoubleToRawbits(2.2),
                      0xbfb5555555555555,
                      DoubleToRawbits(4.4),
                      DoubleToRawbits(11.0),
                      DoubleToRawbits(6.6),
                      DoubleToRawbits(kFP64PositiveInfinity),
                      DoubleToRawbits(8.8),
                      DoubleToRawbits(kFP64NegativeInfinity)};

  FPBinArithHelper(config,
                   &MacroAssembler::Fdiv,
                   kDRegSize,
                   zd_in,
                   pg_in,
                   zn_in,
                   zm_in,
                   exp_d);
}

TEST_SVE(sve_select) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in0[] = {0x01f203f405f607f8, 0xfefcf8f0e1c3870f, 0x123456789abcdef0};
  uint64_t in1[] = {0xaaaaaaaaaaaaaaaa, 0xaaaaaaaaaaaaaaaa, 0xaaaaaaaaaaaaaaaa};

  // For simplicity, we re-use the same pg for various lane sizes.
  // For D lanes:         1,                      1,                      0
  // For S lanes:         1,          1,          1,          0,          0
  // For H lanes:   0,    1,    0,    1,    1,    1,    0,    0,    1,    0
  int pg_in[] = {1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0};
  Initialise(&masm, p0.VnB(), pg_in);
  PRegisterM pg = p0.Merging();

  InsrHelper(&masm, z30.VnD(), in0);
  InsrHelper(&masm, z31.VnD(), in1);

  __ Sel(z0.VnB(), pg, z30.VnB(), z31.VnB());
  __ Sel(z1.VnH(), pg, z30.VnH(), z31.VnH());
  __ Sel(z2.VnS(), pg, z30.VnS(), z31.VnS());
  __ Sel(z3.VnD(), pg, z30.VnD(), z31.VnD());

  END();

  if (CAN_RUN()) {
    RUN();

    uint64_t expected_z0[] = {0xaaaaaaaa05aa07f8,
                              0xfeaaaaf0aac3870f,
                              0xaaaa56aa9abcdeaa};
    ASSERT_EQUAL_SVE(expected_z0, z0.VnD());

    uint64_t expected_z1[] = {0xaaaaaaaaaaaa07f8,
                              0xaaaaf8f0e1c3870f,
                              0xaaaaaaaa9abcaaaa};
    ASSERT_EQUAL_SVE(expected_z1, z1.VnD());

    uint64_t expected_z2[] = {0xaaaaaaaa05f607f8,
                              0xfefcf8f0e1c3870f,
                              0xaaaaaaaaaaaaaaaa};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());

    uint64_t expected_z3[] = {0x01f203f405f607f8,
                              0xfefcf8f0e1c3870f,
                              0xaaaaaaaaaaaaaaaa};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());
  }
}

// Execute a number of instructions which all use ProcessNaNs, and check that
// they all propagate NaNs correctly.
template <typename Ti, typename Te, size_t N>
static void ProcessNaNsHelper(Test* config,
                              int lane_size_in_bits,
                              const Ti (&zn_inputs)[N],
                              const Ti (&zm_inputs)[N],
                              const Te (&zd_expected)[N],
                              FPMacroNaNPropagationOption nan_option) {
  ArithFn unpredicated_macro[] = {&MacroAssembler::Fadd,
                                  &MacroAssembler::Fsub,
                                  &MacroAssembler::Fmul};

  for (size_t i = 0; i < ArrayLength(unpredicated_macro); i++) {
    FPBinArithHelper(config,
                     unpredicated_macro[i],
                     lane_size_in_bits,
                     zn_inputs,
                     zm_inputs,
                     zd_expected);
  }

  FPArithPredicatedFn predicated_macro[] = {&MacroAssembler::Fdiv,
                                            &MacroAssembler::Fmax,
                                            &MacroAssembler::Fmin};
  int pg_inputs[N];
  // With an all-true predicate, this helper aims to compare with special
  // numbers.
  for (size_t i = 0; i < N; i++) {
    pg_inputs[i] = 1;
  }

  for (size_t i = 0; i < ArrayLength(predicated_macro); i++) {
    FPBinArithHelper(config,
                     predicated_macro[i],
                     lane_size_in_bits,
                     // With an all-true predicate, the value in zd is
                     // irrelevant to the operations.
                     zn_inputs,
                     pg_inputs,
                     zn_inputs,
                     zm_inputs,
                     zd_expected,
                     nan_option);
  }
}

TEST_SVE(sve_process_nans_double) {
  // Use non-standard NaNs to check that the payload bits are preserved.
  double sn = RawbitsToDouble(0x7ff5555511111111);
  double sm = RawbitsToDouble(0x7ff5555522222222);
  double qn = RawbitsToDouble(0x7ffaaaaa11111111);
  double qm = RawbitsToDouble(0x7ffaaaaa22222222);
  VIXL_ASSERT(IsSignallingNaN(sn));
  VIXL_ASSERT(IsSignallingNaN(sm));
  VIXL_ASSERT(IsQuietNaN(qn));
  VIXL_ASSERT(IsQuietNaN(qm));

  // The input NaNs after passing through ProcessNaN.
  uint64_t sn_proc = 0x7ffd555511111111;
  uint64_t sm_proc = 0x7ffd555522222222;
  uint64_t qn_proc = DoubleToRawbits(qn);
  uint64_t qm_proc = DoubleToRawbits(qm);

  // Quiet NaNs are propagated.
  double zn_inputs_1[] = {qn, 0.0, 0.0, qm, qn, qm};
  double zm_inputs_1[] = {0.0, qn, qm, 0.0, qm, qn};
  uint64_t zd_expected_1[] =
      {qn_proc, qn_proc, qm_proc, qm_proc, qn_proc, qm_proc};
  ProcessNaNsHelper(config,
                    kDRegSize,
                    zn_inputs_1,
                    zm_inputs_1,
                    zd_expected_1,
                    StrictNaNPropagation);

  // Signalling NaNs are propagated.
  double zn_inputs_2[] = {sn, 0.0, 0.0, sm, sn, sm};
  double zm_inputs_2[] = {0.0, sn, sm, 0.0, sm, sn};
  uint64_t zd_expected_2[] =
      {sn_proc, sn_proc, sm_proc, sm_proc, sn_proc, sm_proc};
  ProcessNaNsHelper(config,
                    kDRegSize,
                    zn_inputs_2,
                    zm_inputs_2,
                    zd_expected_2,
                    StrictNaNPropagation);

  // Signalling NaNs take precedence over quiet NaNs.
  double zn_inputs_3[] = {sn, qn, sn, sn, qn};
  double zm_inputs_3[] = {qm, sm, sm, qn, sn};
  uint64_t zd_expected_3[] = {sn_proc, sm_proc, sn_proc, sn_proc, sn_proc};
  ProcessNaNsHelper(config,
                    kDRegSize,
                    zn_inputs_3,
                    zm_inputs_3,
                    zd_expected_3,
                    StrictNaNPropagation);
}

TEST_SVE(sve_process_nans_float) {
  // Use non-standard NaNs to check that the payload bits are preserved.
  float sn = RawbitsToFloat(0x7f951111);
  float sm = RawbitsToFloat(0x7f952222);
  float qn = RawbitsToFloat(0x7fea1111);
  float qm = RawbitsToFloat(0x7fea2222);
  VIXL_ASSERT(IsSignallingNaN(sn));
  VIXL_ASSERT(IsSignallingNaN(sm));
  VIXL_ASSERT(IsQuietNaN(qn));
  VIXL_ASSERT(IsQuietNaN(qm));

  // The input NaNs after passing through ProcessNaN.
  uint32_t sn_proc = 0x7fd51111;
  uint32_t sm_proc = 0x7fd52222;
  uint32_t qn_proc = FloatToRawbits(qn);
  uint32_t qm_proc = FloatToRawbits(qm);

  // Quiet NaNs are propagated.
  float zn_inputs_1[] = {qn, 0.0f, 0.0f, qm, qn, qm};
  float zm_inputs_1[] = {0.0f, qn, qm, 0.0f, qm, qn};
  uint64_t zd_expected_1[] =
      {qn_proc, qn_proc, qm_proc, qm_proc, qn_proc, qm_proc};
  ProcessNaNsHelper(config,
                    kSRegSize,
                    zn_inputs_1,
                    zm_inputs_1,
                    zd_expected_1,
                    StrictNaNPropagation);

  // Signalling NaNs are propagated.
  float zn_inputs_2[] = {sn, 0.0f, 0.0f, sm, sn, sm};
  float zm_inputs_2[] = {0.0f, sn, sm, 0.0f, sm, sn};
  uint64_t zd_expected_2[] =
      {sn_proc, sn_proc, sm_proc, sm_proc, sn_proc, sm_proc};
  ProcessNaNsHelper(config,
                    kSRegSize,
                    zn_inputs_2,
                    zm_inputs_2,
                    zd_expected_2,
                    StrictNaNPropagation);

  // Signalling NaNs take precedence over quiet NaNs.
  float zn_inputs_3[] = {sn, qn, sn, sn, qn};
  float zm_inputs_3[] = {qm, sm, sm, qn, sn};
  uint64_t zd_expected_3[] = {sn_proc, sm_proc, sn_proc, sn_proc, sn_proc};
  ProcessNaNsHelper(config,
                    kSRegSize,
                    zn_inputs_3,
                    zm_inputs_3,
                    zd_expected_3,
                    StrictNaNPropagation);
}

TEST_SVE(sve_process_nans_half) {
  // Use non-standard NaNs to check that the payload bits are preserved.
  Float16 sn(RawbitsToFloat16(0x7c11));
  Float16 sm(RawbitsToFloat16(0xfc22));
  Float16 qn(RawbitsToFloat16(0x7e33));
  Float16 qm(RawbitsToFloat16(0xfe44));
  VIXL_ASSERT(IsSignallingNaN(sn));
  VIXL_ASSERT(IsSignallingNaN(sm));
  VIXL_ASSERT(IsQuietNaN(qn));
  VIXL_ASSERT(IsQuietNaN(qm));

  // The input NaNs after passing through ProcessNaN.
  uint16_t sn_proc = 0x7e11;
  uint16_t sm_proc = 0xfe22;
  uint16_t qn_proc = Float16ToRawbits(qn);
  uint16_t qm_proc = Float16ToRawbits(qm);

  // Quiet NaNs are propagated.
  Float16 zn_inputs_1[] = {qn, Float16(0.0), Float16(0.0), qm, qn, qm};
  Float16 zm_inputs_1[] = {Float16(0.0), qn, qm, Float16(0.0), qm, qn};
  uint64_t zd_expected_1[] =
      {qn_proc, qn_proc, qm_proc, qm_proc, qn_proc, qm_proc};
  ProcessNaNsHelper(config,
                    kHRegSize,
                    zn_inputs_1,
                    zm_inputs_1,
                    zd_expected_1,
                    StrictNaNPropagation);

  // Signalling NaNs are propagated.
  Float16 zn_inputs_2[] = {sn, Float16(0.0), Float16(0.0), sm, sn, sm};
  Float16 zm_inputs_2[] = {Float16(0.0), sn, sm, Float16(0.0), sm, sn};
  uint64_t zd_expected_2[] =
      {sn_proc, sn_proc, sm_proc, sm_proc, sn_proc, sm_proc};
  ProcessNaNsHelper(config,
                    kHRegSize,
                    zn_inputs_2,
                    zm_inputs_2,
                    zd_expected_2,
                    StrictNaNPropagation);

  // Signalling NaNs take precedence over quiet NaNs.
  Float16 zn_inputs_3[] = {sn, qn, sn, sn, qn};
  Float16 zm_inputs_3[] = {qm, sm, sm, qn, sn};
  uint64_t zd_expected_3[] = {sn_proc, sm_proc, sn_proc, sn_proc, sn_proc};
  ProcessNaNsHelper(config,
                    kHRegSize,
                    zn_inputs_3,
                    zm_inputs_3,
                    zd_expected_3,
                    StrictNaNPropagation);
}

TEST_SVE(sve_binary_arithmetic_predicated_fmax_fmin_h) {
  double zd_inputs[] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
  double zn_inputs[] = {-2.1,
                        8.5,
                        225.5,
                        0.0,
                        8.8,
                        -4.75,
                        kFP64PositiveInfinity,
                        kFP64NegativeInfinity};
  double zm_inputs[] = {-2.0,
                        -13.0,
                        24.0,
                        0.01,
                        0.5,
                        300.75,
                        kFP64NegativeInfinity,
                        kFP64PositiveInfinity};
  int pg_inputs[] = {1, 1, 0, 1, 0, 1, 1, 1};

  uint16_t zd_expected_max[] = {Float16ToRawbits(Float16(-2.0)),
                                Float16ToRawbits(Float16(8.5)),
                                Float16ToRawbits(Float16(3.3)),
                                Float16ToRawbits(Float16(0.01)),
                                Float16ToRawbits(Float16(5.5)),
                                Float16ToRawbits(Float16(300.75)),
                                Float16ToRawbits(kFP16PositiveInfinity),
                                Float16ToRawbits(kFP16PositiveInfinity)};
  FPBinArithHelper(config,
                   &MacroAssembler::Fmax,
                   kHRegSize,
                   zd_inputs,
                   pg_inputs,
                   zn_inputs,
                   zm_inputs,
                   zd_expected_max);

  uint16_t zd_expected_min[] = {Float16ToRawbits(Float16(-2.1)),
                                Float16ToRawbits(Float16(-13.0)),
                                Float16ToRawbits(Float16(3.3)),
                                Float16ToRawbits(Float16(0.0)),
                                Float16ToRawbits(Float16(5.5)),
                                Float16ToRawbits(Float16(-4.75)),
                                Float16ToRawbits(kFP16NegativeInfinity),
                                Float16ToRawbits(kFP16NegativeInfinity)};
  FPBinArithHelper(config,
                   &MacroAssembler::Fmin,
                   kHRegSize,
                   zd_inputs,
                   pg_inputs,
                   zn_inputs,
                   zm_inputs,
                   zd_expected_min);
}

TEST_SVE(sve_binary_arithmetic_predicated_fmax_fmin_s) {
  double zd_inputs[] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
  double zn_inputs[] = {-2.1,
                        8.5,
                        225.5,
                        0.0,
                        8.8,
                        -4.75,
                        kFP64PositiveInfinity,
                        kFP64NegativeInfinity};
  double zm_inputs[] = {-2.0,
                        -13.0,
                        24.0,
                        0.01,
                        0.5,
                        300.75,
                        kFP64NegativeInfinity,
                        kFP64PositiveInfinity};
  int pg_inputs[] = {1, 1, 0, 1, 0, 1, 1, 1};

  uint32_t zd_expected_max[] = {FloatToRawbits(-2.0),
                                FloatToRawbits(8.5),
                                FloatToRawbits(3.3),
                                FloatToRawbits(0.01),
                                FloatToRawbits(5.5),
                                FloatToRawbits(300.75),
                                FloatToRawbits(kFP32PositiveInfinity),
                                FloatToRawbits(kFP32PositiveInfinity)};
  FPBinArithHelper(config,
                   &MacroAssembler::Fmax,
                   kSRegSize,
                   zd_inputs,
                   pg_inputs,
                   zn_inputs,
                   zm_inputs,
                   zd_expected_max);

  uint32_t zd_expected_min[] = {FloatToRawbits(-2.1),
                                FloatToRawbits(-13.0),
                                FloatToRawbits(3.3),
                                FloatToRawbits(0.0),
                                FloatToRawbits(5.5),
                                FloatToRawbits(-4.75),
                                FloatToRawbits(kFP32NegativeInfinity),
                                FloatToRawbits(kFP32NegativeInfinity)};
  FPBinArithHelper(config,
                   &MacroAssembler::Fmin,
                   kSRegSize,
                   zd_inputs,
                   pg_inputs,
                   zn_inputs,
                   zm_inputs,
                   zd_expected_min);
}

TEST_SVE(sve_binary_arithmetic_predicated_fmax_fmin_d) {
  double zd_inputs[] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
  double zn_inputs[] = {-2.1,
                        8.5,
                        225.5,
                        0.0,
                        8.8,
                        -4.75,
                        kFP64PositiveInfinity,
                        kFP64NegativeInfinity};
  double zm_inputs[] = {-2.0,
                        -13.0,
                        24.0,
                        0.01,
                        0.5,
                        300.75,
                        kFP64NegativeInfinity,
                        kFP64PositiveInfinity};
  int pg_inputs[] = {1, 1, 0, 1, 0, 1, 1, 1};

  uint64_t zd_expected_max[] = {DoubleToRawbits(-2.0),
                                DoubleToRawbits(8.5),
                                DoubleToRawbits(3.3),
                                DoubleToRawbits(0.01),
                                DoubleToRawbits(5.5),
                                DoubleToRawbits(300.75),
                                DoubleToRawbits(kFP64PositiveInfinity),
                                DoubleToRawbits(kFP64PositiveInfinity)};
  FPBinArithHelper(config,
                   &MacroAssembler::Fmax,
                   kDRegSize,
                   zd_inputs,
                   pg_inputs,
                   zn_inputs,
                   zm_inputs,
                   zd_expected_max);

  uint64_t zd_expected_min[] = {DoubleToRawbits(-2.1),
                                DoubleToRawbits(-13.0),
                                DoubleToRawbits(3.3),
                                DoubleToRawbits(0.0),
                                DoubleToRawbits(5.5),
                                DoubleToRawbits(-4.75),
                                DoubleToRawbits(kFP64NegativeInfinity),
                                DoubleToRawbits(kFP64NegativeInfinity)};
  FPBinArithHelper(config,
                   &MacroAssembler::Fmin,
                   kDRegSize,
                   zd_inputs,
                   pg_inputs,
                   zn_inputs,
                   zm_inputs,
                   zd_expected_min);
}

template <typename T, size_t N>
static void BitwiseShiftImmHelper(Test* config,
                                  int lane_size_in_bits,
                                  const T (&zn_inputs)[N],
                                  int shift) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  ZRegister zd_asr = z25.WithLaneSize(lane_size_in_bits);
  ZRegister zd_lsr = z26.WithLaneSize(lane_size_in_bits);
  ZRegister zd_lsl = z27.WithLaneSize(lane_size_in_bits);
  ZRegister zn = z28.WithLaneSize(lane_size_in_bits);

  InsrHelper(&masm, zn, zn_inputs);

  __ Asr(zd_asr, zn, shift);
  __ Lsr(zd_lsr, zn, shift);
  __ Lsl(zd_lsl, zn, shift);

  END();

  if (CAN_RUN()) {
    RUN();

    const uint64_t mask = GetUintMask(lane_size_in_bits);
    for (int i = 0; i < static_cast<int>(N); i++) {
      int lane = N - i - 1;
      if (!core.HasSVELane(zd_asr, lane)) break;
      bool is_negative = (zn_inputs[i] & GetSignMask(lane_size_in_bits)) != 0;
      uint64_t result;
      if (shift >= lane_size_in_bits) {
        result = is_negative ? mask : 0;
      } else {
        result = zn_inputs[i] >> shift;
        if (is_negative) {
          result |= mask << (lane_size_in_bits - shift);
          result &= mask;
        }
      }
      ASSERT_EQUAL_SVE_LANE(result, zd_asr, lane);
    }

    for (int i = 0; i < static_cast<int>(N); i++) {
      int lane = N - i - 1;
      if (!core.HasSVELane(zd_lsr, lane)) break;
      uint64_t result =
          (shift >= lane_size_in_bits) ? 0 : zn_inputs[i] >> shift;
      ASSERT_EQUAL_SVE_LANE(result, zd_lsr, lane);
    }

    for (int i = 0; i < static_cast<int>(N); i++) {
      int lane = N - i - 1;
      if (!core.HasSVELane(zd_lsl, lane)) break;
      uint64_t result = (shift >= lane_size_in_bits) ? 0 : zn_inputs[i]
                                                               << shift;
      ASSERT_EQUAL_SVE_LANE(result & mask, zd_lsl, lane);
    }
  }
}

TEST_SVE(sve_bitwise_shift_imm_unpredicated) {
  uint64_t inputs_b[] = {0xfe, 0xdc, 0xba, 0x98, 0xff, 0x55, 0xaa, 0x80};
  int shift_b[] = {1, 3, 5, 8};
  for (size_t i = 0; i < ArrayLength(shift_b); i++) {
    BitwiseShiftImmHelper(config, kBRegSize, inputs_b, shift_b[i]);
  }

  uint64_t inputs_h[] = {0xfedc, 0xfa55, 0x0011, 0x2233};
  int shift_h[] = {1, 8, 11, 16};
  for (size_t i = 0; i < ArrayLength(shift_h); i++) {
    BitwiseShiftImmHelper(config, kHRegSize, inputs_h, shift_h[i]);
  }

  uint64_t inputs_s[] = {0xfedcba98, 0xfffa55aa, 0x00112233};
  int shift_s[] = {1, 9, 17, 32};
  for (size_t i = 0; i < ArrayLength(shift_s); i++) {
    BitwiseShiftImmHelper(config, kSRegSize, inputs_s, shift_s[i]);
  }

  uint64_t inputs_d[] = {0xfedcba98fedcba98,
                         0xfffa5555aaaaaaaa,
                         0x0011223344aafe80};
  int shift_d[] = {1, 23, 45, 64};
  for (size_t i = 0; i < ArrayLength(shift_d); i++) {
    BitwiseShiftImmHelper(config, kDRegSize, inputs_d, shift_d[i]);
  }
}

template <typename T, typename R, size_t N>
static void BitwiseShiftWideElementsHelper(Test* config,
                                           Shift shift_type,
                                           int lane_size_in_bits,
                                           const T (&zn_inputs)[N],
                                           const R& zm_inputs,
                                           const T (&zd_expected)[N]) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  ArithFn macro;
  // Since logical shift left and right by the current lane size width is equal
  // to 0, so initialize the array to 0 for convenience.
  uint64_t zd_expected_max_shift_amount[N] = {0};
  switch (shift_type) {
    case ASR: {
      macro = &MacroAssembler::Asr;
      uint64_t mask = GetUintMask(lane_size_in_bits);
      for (size_t i = 0; i < ArrayLength(zn_inputs); i++) {
        bool is_negative = (zn_inputs[i] & GetSignMask(lane_size_in_bits)) != 0;
        zd_expected_max_shift_amount[i] = is_negative ? mask : 0;
      }
      break;
    }
    case LSR:
      macro = &MacroAssembler::Lsr;
      break;
    case LSL:
      macro = &MacroAssembler::Lsl;
      break;
    default:
      VIXL_UNIMPLEMENTED();
      macro = NULL;
      break;
  }

  ZRegister zd = z26.WithLaneSize(lane_size_in_bits);
  ZRegister zn = z27.WithLaneSize(lane_size_in_bits);
  ZRegister zm = z28.WithLaneSize(kDRegSize);

  InsrHelper(&masm, zn, zn_inputs);
  InsrHelper(&masm, zm, zm_inputs);

  (masm.*macro)(zd, zn, zm);

  ZRegister zm_max_shift_amount = z25.WithLaneSize(kDRegSize);
  ZRegister zd_max_shift_amount = z24.WithLaneSize(lane_size_in_bits);

  __ Dup(zm_max_shift_amount, lane_size_in_bits);
  (masm.*macro)(zd_max_shift_amount, zn, zm_max_shift_amount);

  ZRegister zm_out_of_range = z23.WithLaneSize(kDRegSize);
  ZRegister zd_out_of_range = z22.WithLaneSize(lane_size_in_bits);

  __ Dup(zm_out_of_range, GetUintMask(lane_size_in_bits));
  (masm.*macro)(zd_out_of_range, zn, zm_out_of_range);

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(zd_expected, zd);
    ASSERT_EQUAL_SVE(zd_expected_max_shift_amount, zd_max_shift_amount);
    ASSERT_EQUAL_SVE(zd_max_shift_amount, zd_out_of_range);
  }
}

TEST_SVE(sve_bitwise_shift_wide_elements_unpredicated_asr) {
  // clang-format off
  uint64_t inputs_b[] = {0xfe, 0xdc, 0xba, 0x98, 0xff, 0x55, 0xaa, 0x80,
                         0xfe, 0xdc, 0xba, 0x98, 0xff, 0x55, 0xaa, 0x80};
  int shift_b[] = {1, 3};
  uint64_t expected_b[] = {0xff, 0xee, 0xdd, 0xcc, 0xff, 0x2a, 0xd5, 0xc0,
                           0xff, 0xfb, 0xf7, 0xf3, 0xff, 0x0a, 0xf5, 0xf0};
  BitwiseShiftWideElementsHelper(config,
                                 ASR,
                                 kBRegSize,
                                 inputs_b,
                                 shift_b,
                                 expected_b);

  uint64_t inputs_h[] = {0xfedc, 0xfa55, 0x0011, 0x2233,
                         0xfedc, 0xfa55, 0x0011, 0x2233,
                         0xfedc, 0xfa55, 0x0011, 0x2233};
  int shift_h[] = {1, 8, 11};
  uint64_t expected_h[] = {0xff6e, 0xfd2a, 0x0008, 0x1119,
                           0xfffe, 0xfffa, 0x0000, 0x0022,
                           0xffff, 0xffff, 0x0000, 0x0004};
  BitwiseShiftWideElementsHelper(config,
                                 ASR,
                                 kHRegSize,
                                 inputs_h,
                                 shift_h,
                                 expected_h);

  uint64_t inputs_s[] =
      {0xfedcba98, 0xfffa55aa, 0x00112233, 0x01234567, 0xaaaaaaaa, 0x88888888};
  int shift_s[] = {1, 9, 23};
  uint64_t expected_s[] =
      {0xff6e5d4c, 0xfffd2ad5, 0x00000891, 0x000091a2, 0xffffff55, 0xffffff11};
  BitwiseShiftWideElementsHelper(config,
                                 ASR,
                                 kSRegSize,
                                 inputs_s,
                                 shift_s,
                                 expected_s);
  // clang-format on
}

TEST_SVE(sve_bitwise_shift_wide_elements_unpredicated_lsr) {
  // clang-format off
  uint64_t inputs_b[] = {0xfe, 0xdc, 0xba, 0x98, 0xff, 0x55, 0xaa, 0x80,
                         0xfe, 0xdc, 0xba, 0x98, 0xff, 0x55, 0xaa, 0x80};
  int shift_b[] = {1, 3};
  uint64_t expected_b[] = {0x7f, 0x6e, 0x5d, 0x4c, 0x7f, 0x2a, 0x55, 0x40,
                           0x1f, 0x1b, 0x17, 0x13, 0x1f, 0x0a, 0x15, 0x10};

  BitwiseShiftWideElementsHelper(config,
                                 LSR,
                                 kBRegSize,
                                 inputs_b,
                                 shift_b,
                                 expected_b);

  uint64_t inputs_h[] = {0xfedc, 0xfa55, 0x0011, 0x2233,
                         0xfedc, 0xfa55, 0x0011, 0x2233,
                         0xfedc, 0xfa55, 0x0011, 0x2233};
  int shift_h[] = {1, 8, 11};
  uint64_t expected_h[] = {0x7f6e, 0x7d2a, 0x0008, 0x1119,
                           0x00fe, 0x00fa, 0x0000, 0x0022,
                           0x001f, 0x001f, 0x0000, 0x0004};
  BitwiseShiftWideElementsHelper(config,
                                 LSR,
                                 kHRegSize,
                                 inputs_h,
                                 shift_h,
                                 expected_h);

  uint64_t inputs_s[] =
      {0xfedcba98, 0xfffa55aa, 0x00112233, 0x01234567, 0xaaaaaaaa, 0x88888888};
  int shift_s[] = {1, 9, 23};
  uint64_t expected_s[] =
      {0x7f6e5d4c, 0x7ffd2ad5, 0x00000891, 0x000091a2, 0x00000155, 0x00000111};
  BitwiseShiftWideElementsHelper(config,
                                 LSR,
                                 kSRegSize,
                                 inputs_s,
                                 shift_s,
                                 expected_s);
  // clang-format on
}

TEST_SVE(sve_bitwise_shift_wide_elements_unpredicated_lsl) {
  // clang-format off
  uint64_t inputs_b[] = {0xfe, 0xdc, 0xba, 0x98, 0xff, 0x55, 0xaa, 0x80,
                         0xfe, 0xdc, 0xba, 0x98, 0xff, 0x55, 0xaa, 0x80};
  int shift_b[] = {1, 5};

  uint64_t expected_b[] = {0xfc, 0xb8, 0x74, 0x30, 0xfe, 0xaa, 0x54, 0x00,
                           0xc0, 0x80, 0x40, 0x00, 0xe0, 0xa0, 0x40, 0x00};

  BitwiseShiftWideElementsHelper(config,
                                 LSL,
                                 kBRegSize,
                                 inputs_b,
                                 shift_b,
                                 expected_b);
  uint64_t inputs_h[] = {0xfedc, 0xfa55, 0x0011, 0x2233,
                         0xfedc, 0xfa55, 0x0011, 0x2233,
                         0xfedc, 0xfa55, 0x0011, 0x2233};
  int shift_h[] = {1, 2, 14};

  uint64_t expected_h[] = {0xfdb8, 0xf4aa, 0x0022, 0x4466,
                           0xfb70, 0xe954, 0x0044, 0x88cc,
                           0x0000, 0x4000, 0x4000, 0xc000};
  BitwiseShiftWideElementsHelper(config,
                                 LSL,
                                 kHRegSize,
                                 inputs_h,
                                 shift_h,
                                 expected_h);
  uint64_t inputs_s[] =
      {0xfedcba98, 0xfffa55aa, 0x00112233, 0x01234567, 0xaaaaaaaa, 0x88888888};
  int shift_s[] = {1, 19, 26};
  uint64_t expected_s[] =
      {0xfdb97530, 0xfff4ab54, 0x11980000, 0x2b380000, 0xa8000000, 0x20000000};
  BitwiseShiftWideElementsHelper(config,
                                 LSL,
                                 kSRegSize,
                                 inputs_s,
                                 shift_s,
                                 expected_s);
  // clang-format on
}

TEST_SVE(sve_setffr) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  __ Ptrue(p15.VnB());
  __ Setffr();
  __ Rdffr(p14.VnB());

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(p14.VnB(), p15.VnB());
  }
}

static void WrffrHelper(Test* config, unsigned active_lanes) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  int inputs[kPRegMaxSize] = {0};
  VIXL_ASSERT(active_lanes <= kPRegMaxSize);
  for (unsigned i = 0; i < active_lanes; i++) {
    // The rightmost (highest-indexed) array element maps to the lowest-numbered
    // lane.
    inputs[kPRegMaxSize - i - 1] = 1;
  }

  Initialise(&masm, p1.VnB(), inputs);
  __ Wrffr(p1.VnB());
  __ Rdffr(p2.VnB());

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(p1.VnB(), p2.VnB());
  }
}

TEST_SVE(sve_wrffr) {
  int active_lanes_inputs[] = {0, 1, 7, 10, 32, 48, kPRegMaxSize};
  for (size_t i = 0; i < ArrayLength(active_lanes_inputs); i++) {
    WrffrHelper(config, active_lanes_inputs[i]);
  }
}

template <size_t N>
static void RdffrHelper(Test* config,
                        size_t active_lanes,
                        const int (&pg_inputs)[N]) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  VIXL_ASSERT(active_lanes <= kPRegMaxSize);

  // The rightmost (highest-indexed) array element maps to the lowest-numbered
  // lane.
  int pd[kPRegMaxSize] = {0};
  for (unsigned i = 0; i < active_lanes; i++) {
    pd[kPRegMaxSize - i - 1] = 1;
  }

  int pg[kPRegMaxSize] = {0};
  for (unsigned i = 0; i < N; i++) {
    pg[kPRegMaxSize - i - 1] = pg_inputs[i];
  }

  int pd_expected[kPRegMaxSize] = {0};
  for (unsigned i = 0; i < std::min(active_lanes, N); i++) {
    int lane = kPRegMaxSize - i - 1;
    pd_expected[lane] = pd[lane] & pg[lane];
  }

  Initialise(&masm, p0.VnB(), pg);
  Initialise(&masm, p1.VnB(), pd);

  // The unpredicated form of rdffr has been tested in `WrffrHelper`.
  __ Wrffr(p1.VnB());
  __ Rdffr(p14.VnB(), p0.Zeroing());
  __ Rdffrs(p13.VnB(), p0.Zeroing());
  __ Mrs(x8, NZCV);

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(pd_expected, p14.VnB());
    ASSERT_EQUAL_SVE(pd_expected, p13.VnB());
    StatusFlags nzcv_expected =
        GetPredTestFlags(pd_expected, pg, core.GetSVELaneCount(kBRegSize));
    ASSERT_EQUAL_64(nzcv_expected, x8);
  }
}

TEST_SVE(sve_rdffr_rdffrs) {
  // clang-format off
  int active_lanes_inputs[] = {0, 1, 15, 26, 39, 47, kPRegMaxSize};
  int pg_inputs_0[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int pg_inputs_1[] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
  int pg_inputs_2[] = {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
  int pg_inputs_3[] = {0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1};
  int pg_inputs_4[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  // clang-format on

  for (size_t i = 0; i < ArrayLength(active_lanes_inputs); i++) {
    RdffrHelper(config, active_lanes_inputs[i], pg_inputs_0);
    RdffrHelper(config, active_lanes_inputs[i], pg_inputs_1);
    RdffrHelper(config, active_lanes_inputs[i], pg_inputs_2);
    RdffrHelper(config, active_lanes_inputs[i], pg_inputs_3);
    RdffrHelper(config, active_lanes_inputs[i], pg_inputs_4);
  }
}

typedef void (MacroAssembler::*BrkpFn)(const PRegisterWithLaneSize& pd,
                                       const PRegisterZ& pg,
                                       const PRegisterWithLaneSize& pn,
                                       const PRegisterWithLaneSize& pm);

template <typename Tg, typename Tn, typename Td>
static void BrkpaBrkpbHelper(Test* config,
                             BrkpFn macro,
                             BrkpFn macro_set_flags,
                             const Tg& pg_inputs,
                             const Tn& pn_inputs,
                             const Tn& pm_inputs,
                             const Td& pd_expected) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  PRegister pg = p15;
  PRegister pn = p14;
  PRegister pm = p13;
  Initialise(&masm, pg.VnB(), pg_inputs);
  Initialise(&masm, pn.VnB(), pn_inputs);
  Initialise(&masm, pm.VnB(), pm_inputs);

  // Initialise NZCV to an impossible value, to check that we actually write it.
  __ Mov(x10, NZCVFlag);
  __ Msr(NZCV, x10);

  (masm.*macro_set_flags)(p0.VnB(), pg.Zeroing(), pn.VnB(), pm.VnB());
  __ Mrs(x0, NZCV);

  (masm.*macro)(p1.VnB(), pg.Zeroing(), pn.VnB(), pm.VnB());

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(pd_expected, p0.VnB());

    // Check that the flags were properly set.
    StatusFlags nzcv_expected =
        GetPredTestFlags(pd_expected,
                         pg_inputs,
                         core.GetSVELaneCount(kBRegSize));
    ASSERT_EQUAL_64(nzcv_expected, x0);
    ASSERT_EQUAL_SVE(p0.VnB(), p1.VnB());
  }
}

template <typename Tg, typename Tn, typename Td>
static void BrkpaHelper(Test* config,
                        const Tg& pg_inputs,
                        const Tn& pn_inputs,
                        const Tn& pm_inputs,
                        const Td& pd_expected) {
  BrkpaBrkpbHelper(config,
                   &MacroAssembler::Brkpa,
                   &MacroAssembler::Brkpas,
                   pg_inputs,
                   pn_inputs,
                   pm_inputs,
                   pd_expected);
}

template <typename Tg, typename Tn, typename Td>
static void BrkpbHelper(Test* config,
                        const Tg& pg_inputs,
                        const Tn& pn_inputs,
                        const Tn& pm_inputs,
                        const Td& pd_expected) {
  BrkpaBrkpbHelper(config,
                   &MacroAssembler::Brkpb,
                   &MacroAssembler::Brkpbs,
                   pg_inputs,
                   pn_inputs,
                   pm_inputs,
                   pd_expected);
}

TEST_SVE(sve_brkpb) {
  // clang-format off
  // The last active element of `pn` are `true` in all vector length configurations.
  //                                | boundary of 128-bits VL.
  //                                v
  int pg_1[] =      {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};
  int pg_2[] =      {1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};
  int pg_3[] =      {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1};

  //                 | highest-numbered lane                lowest-numbered lane |
  //                 v                                                           v
  int pn_1[] =      {1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
  int pn_2[] =      {1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
  int pn_3[] =      {1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1};

  int pm_1[] =      {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
  int pm_2[] =      {0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int pm_3[] =      {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};

  //                                                                    | first active
  //                                                                    v
  int exp_1_1_1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
  //                                            | first active
  //                                            v
  int exp_1_2_2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};
  //                                                                    | first active
  //                                                                    v
  int exp_1_3_3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};

  BrkpbHelper(config, pg_1, pn_1, pm_1, exp_1_1_1);
  BrkpbHelper(config, pg_1, pn_2, pm_2, exp_1_2_2);
  BrkpbHelper(config, pg_1, pn_3, pm_3, exp_1_3_3);

  //                                               | first active
  //                                               v
  int exp_2_1_2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};
  //                                            | first active
  //                                            v
  int exp_2_2_3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};
  //                                                                 | first active
  //                                                                 v
  int exp_2_3_1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1};
  BrkpbHelper(config, pg_2, pn_1, pm_2, exp_2_1_2);
  BrkpbHelper(config, pg_2, pn_2, pm_3, exp_2_2_3);
  BrkpbHelper(config, pg_2, pn_3, pm_1, exp_2_3_1);

  //                                                                    | first active
  //                                                                    v
  int exp_3_1_3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1};
  //                                                                    | first active
  //                                                                    v
  int exp_3_2_1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1};
  //                                      | first active
  //                                      v
  int exp_3_3_2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1};
  BrkpbHelper(config, pg_3, pn_1, pm_3, exp_3_1_3);
  BrkpbHelper(config, pg_3, pn_2, pm_1, exp_3_2_1);
  BrkpbHelper(config, pg_3, pn_3, pm_2, exp_3_3_2);

  // The last active element of `pn` are `false` in all vector length configurations.
  //                       | last active lane when VL > 128 bits.
  //                       v
  //                                   | last active lane when VL == 128 bits.
  //                                   v
  int pg_4[] =      {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1};
  int exp_4_x_x[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  BrkpbHelper(config, pg_4, pn_1, pm_1, exp_4_x_x);
  BrkpbHelper(config, pg_4, pn_2, pm_2, exp_4_x_x);
  BrkpbHelper(config, pg_4, pn_3, pm_3, exp_4_x_x);
  // clang-format on
}

TEST_SVE(sve_brkpa) {
  // clang-format off
  // The last active element of `pn` are `true` in all vector length configurations.
  //                                | boundary of 128-bits VL.
  //                                v
  int pg_1[] =      {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};
  int pg_2[] =      {1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};
  int pg_3[] =      {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1};

  //                 | highest-numbered lane                lowest-numbered lane |
  //                 v                                                           v
  int pn_1[] =      {1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
  int pn_2[] =      {1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
  int pn_3[] =      {1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1};

  int pm_1[] =      {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
  int pm_2[] =      {0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int pm_3[] =      {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};

  //                                                                    | first active
  //                                                                    v
  int exp_1_1_1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0};
  //                                            | first active
  //                                            v
  int exp_1_2_2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};
  //                                                                    | first active
  //                                                                    v
  int exp_1_3_3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0};

  BrkpaHelper(config, pg_1, pn_1, pm_1, exp_1_1_1);
  BrkpaHelper(config, pg_1, pn_2, pm_2, exp_1_2_2);
  BrkpaHelper(config, pg_1, pn_3, pm_3, exp_1_3_3);

  //                                               | first active
  //                                               v
  int exp_2_1_2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};
  //                                            | first active
  //                                            v
  int exp_2_2_3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};
  //                                                                 | first active
  //                                                                 v
  int exp_2_3_1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1};
  BrkpaHelper(config, pg_2, pn_1, pm_2, exp_2_1_2);
  BrkpaHelper(config, pg_2, pn_2, pm_3, exp_2_2_3);
  BrkpaHelper(config, pg_2, pn_3, pm_1, exp_2_3_1);

  //                                                                    | first active
  //                                                                    v
  int exp_3_1_3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1};
  //                                                                    | first active
  //                                                                    v
  int exp_3_2_1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1};
  //                                      | first active
  //                                      v
  int exp_3_3_2[] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1};
  BrkpaHelper(config, pg_3, pn_1, pm_3, exp_3_1_3);
  BrkpaHelper(config, pg_3, pn_2, pm_1, exp_3_2_1);
  BrkpaHelper(config, pg_3, pn_3, pm_2, exp_3_3_2);

  // The last active element of `pn` are `false` in all vector length configurations.
  //                       | last active lane when VL > 128 bits.
  //                       v
  //                                   | last active lane when VL == 128 bits.
  //                                   v
  int pg_4[] =      {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1};
  int exp_4_x_x[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  BrkpaHelper(config, pg_4, pn_1, pm_1, exp_4_x_x);
  BrkpaHelper(config, pg_4, pn_2, pm_2, exp_4_x_x);
  BrkpaHelper(config, pg_4, pn_3, pm_3, exp_4_x_x);
  // clang-format on
}

TEST_SVE(sve_rbit) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t inputs[] = {0xaaaaaaaa55555555, 0xaaaa5555aa55aa55};
  InsrHelper(&masm, z0.VnD(), inputs);

  __ Ptrue(p1.VnB());
  int pred[] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1};
  Initialise(&masm, p2.VnB(), pred);

  __ Rbit(z0.VnB(), p1.Merging(), z0.VnB());
  __ Rbit(z0.VnB(), p1.Merging(), z0.VnB());

  __ Rbit(z1.VnB(), p1.Merging(), z0.VnB());
  __ Rbit(z2.VnH(), p1.Merging(), z0.VnH());
  __ Rbit(z3.VnS(), p1.Merging(), z0.VnS());
  __ Rbit(z4.VnD(), p1.Merging(), z0.VnD());

  __ Dup(z5.VnB(), 0x42);
  __ Rbit(z5.VnB(), p2.Merging(), z0.VnB());
  __ Dup(z6.VnB(), 0x42);
  __ Rbit(z6.VnS(), p2.Merging(), z0.VnS());

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(inputs, z0.VnD());

    uint64_t expected_z1[] = {0x55555555aaaaaaaa, 0x5555aaaa55aa55aa};
    ASSERT_EQUAL_SVE(expected_z1, z1.VnD());
    uint64_t expected_z2[] = {0x55555555aaaaaaaa, 0x5555aaaaaa55aa55};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());
    uint64_t expected_z3[] = {0x55555555aaaaaaaa, 0xaaaa5555aa55aa55};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());
    uint64_t expected_z4[] = {0xaaaaaaaa55555555, 0xaa55aa55aaaa5555};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());
    uint64_t expected_z5[] = {0x4255425542aa42aa, 0x4255424242aa42aa};
    ASSERT_EQUAL_SVE(expected_z5, z5.VnD());
    uint64_t expected_z6[] = {0x55555555aaaaaaaa, 0x42424242aa55aa55};
    ASSERT_EQUAL_SVE(expected_z6, z6.VnD());
  }
}

TEST_SVE(sve_rev_bhw) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t inputs[] = {0xaaaaaaaa55555555, 0xaaaa5555aa55aa55};
  InsrHelper(&masm, z0.VnD(), inputs);

  __ Ptrue(p1.VnB());
  int pred[] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1};
  Initialise(&masm, p2.VnB(), pred);

  __ Revb(z1.VnH(), p1.Merging(), z0.VnH());
  __ Revb(z2.VnS(), p1.Merging(), z0.VnS());
  __ Revb(z3.VnD(), p1.Merging(), z0.VnD());
  __ Revh(z4.VnS(), p1.Merging(), z0.VnS());
  __ Revh(z5.VnD(), p1.Merging(), z0.VnD());
  __ Revw(z6.VnD(), p1.Merging(), z0.VnD());

  __ Dup(z7.VnB(), 0x42);
  __ Revb(z7.VnH(), p2.Merging(), z0.VnH());
  __ Dup(z8.VnB(), 0x42);
  __ Revh(z8.VnS(), p2.Merging(), z0.VnS());

  END();

  if (CAN_RUN()) {
    RUN();

    uint64_t expected_z1[] = {0xaaaaaaaa55555555, 0xaaaa555555aa55aa};
    ASSERT_EQUAL_SVE(expected_z1, z1.VnD());
    uint64_t expected_z2[] = {0xaaaaaaaa55555555, 0x5555aaaa55aa55aa};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());
    uint64_t expected_z3[] = {0x55555555aaaaaaaa, 0x55aa55aa5555aaaa};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());
    uint64_t expected_z4[] = {0xaaaaaaaa55555555, 0x5555aaaaaa55aa55};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());
    uint64_t expected_z5[] = {0x55555555aaaaaaaa, 0xaa55aa555555aaaa};
    ASSERT_EQUAL_SVE(expected_z5, z5.VnD());
    uint64_t expected_z6[] = {0x55555555aaaaaaaa, 0xaa55aa55aaaa5555};
    ASSERT_EQUAL_SVE(expected_z6, z6.VnD());
    uint64_t expected_z7[] = {0xaaaaaaaa55555555, 0xaaaa424255aa55aa};
    ASSERT_EQUAL_SVE(expected_z7, z7.VnD());
    uint64_t expected_z8[] = {0xaaaaaaaa55555555, 0x42424242aa55aa55};
    ASSERT_EQUAL_SVE(expected_z8, z8.VnD());
  }
}

TEST_SVE(sve_ftssel) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in[] = {0x1111777766665555, 0xaaaabbbbccccdddd};
  uint64_t q[] = {0x0001000300000002, 0x0001000200000003};
  InsrHelper(&masm, z0.VnD(), in);
  InsrHelper(&masm, z1.VnD(), q);

  __ Ftssel(z2.VnH(), z0.VnH(), z1.VnH());
  __ Ftssel(z3.VnS(), z0.VnS(), z1.VnS());
  __ Ftssel(z4.VnD(), z0.VnD(), z1.VnD());

  END();

  if (CAN_RUN()) {
    RUN();

    uint64_t expected_z2[] = {0x3c00bc006666d555, 0x3c003bbbccccbc00};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());
    uint64_t expected_z3[] = {0xbf800000e6665555, 0x2aaabbbbbf800000};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());
    uint64_t expected_z4[] = {0x9111777766665555, 0xbff0000000000000};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());
  }
}

TEST_SVE(sve_fexpa) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in0[] = {0x3ff0000000000000, 0x3ff0000000011001};
  uint64_t in1[] = {0x3ff000000002200f, 0xbff000000003301f};
  uint64_t in2[] = {0xbff000000004403f, 0x3ff0000000055040};
  uint64_t in3[] = {0x3f800000bf800001, 0x3f80000f3f80001f};
  uint64_t in4[] = {0x3f80002f3f82203f, 0xbf8000403f833041};
  uint64_t in5[] = {0x3c003c01bc00bc07, 0x3c08bc0f3c1fbc20};
  InsrHelper(&masm, z0.VnD(), in0);
  InsrHelper(&masm, z1.VnD(), in1);
  InsrHelper(&masm, z2.VnD(), in2);
  InsrHelper(&masm, z3.VnD(), in3);
  InsrHelper(&masm, z4.VnD(), in4);
  InsrHelper(&masm, z5.VnD(), in5);

  __ Fexpa(z6.VnD(), z0.VnD());
  __ Fexpa(z7.VnD(), z1.VnD());
  __ Fexpa(z8.VnD(), z2.VnD());
  __ Fexpa(z9.VnS(), z3.VnS());
  __ Fexpa(z10.VnS(), z4.VnS());
  __ Fexpa(z11.VnH(), z5.VnH());

  END();

  if (CAN_RUN()) {
    RUN();
    uint64_t expected_z6[] = {0x0000000000000000, 0x44002c9a3e778061};
    ASSERT_EQUAL_SVE(expected_z6, z6.VnD());
    uint64_t expected_z7[] = {0x0802d285a6e4030b, 0x4c06623882552225};
    ASSERT_EQUAL_SVE(expected_z7, z7.VnD());
    uint64_t expected_z8[] = {0x100fa7c1819e90d8, 0x5410000000000000};
    ASSERT_EQUAL_SVE(expected_z8, z8.VnD());
    uint64_t expected_z9[] = {0x00000000000164d2, 0x0016942d003311c4};
    ASSERT_EQUAL_SVE(expected_z9, z9.VnD());
    uint64_t expected_z10[] = {0x0054f35b407d3e0c, 0x00800000608164d2};
    ASSERT_EQUAL_SVE(expected_z10, z10.VnD());
    uint64_t expected_z11[] = {0x00000016000000a8, 0x00c2018903d40400};
    ASSERT_EQUAL_SVE(expected_z11, z11.VnD());
  }
}

TEST_SVE(sve_rev_p) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  Initialise(&masm,
             p0.VnB(),
             0xabcdabcdabcdabcd,
             0xabcdabcdabcdabcd,
             0xabcdabcdabcdabcd,
             0xabcdabcdabcdabcd);

  __ Rev(p1.VnB(), p0.VnB());
  __ Rev(p2.VnH(), p0.VnH());
  __ Rev(p3.VnS(), p0.VnS());
  __ Rev(p4.VnD(), p0.VnD());

  END();

  if (CAN_RUN()) {
    RUN();

    int p1_expected[] = {1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1};
    ASSERT_EQUAL_SVE(p1_expected, p1.VnB());
    int p2_expected[] = {0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p2_expected, p2.VnB());
    int p3_expected[] = {1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p3_expected, p3.VnB());
    int p4_expected[] = {1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1};
    ASSERT_EQUAL_SVE(p4_expected, p4.VnB());
  }
}

TEST_SVE(sve_trn_p_bh) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  Initialise(&masm, p0.VnB(), 0xa5a55a5a);
  __ Pfalse(p1.VnB());

  __ Trn1(p2.VnB(), p0.VnB(), p0.VnB());
  __ Trn2(p3.VnB(), p0.VnB(), p0.VnB());
  __ Trn1(p4.VnB(), p1.VnB(), p0.VnB());
  __ Trn2(p5.VnB(), p1.VnB(), p0.VnB());
  __ Trn1(p6.VnB(), p0.VnB(), p1.VnB());
  __ Trn2(p7.VnB(), p0.VnB(), p1.VnB());

  __ Trn1(p8.VnH(), p0.VnH(), p0.VnH());
  __ Trn2(p9.VnH(), p0.VnH(), p0.VnH());
  __ Trn1(p10.VnH(), p1.VnH(), p0.VnH());
  __ Trn2(p11.VnH(), p1.VnH(), p0.VnH());
  __ Trn1(p12.VnH(), p0.VnH(), p1.VnH());
  __ Trn2(p13.VnH(), p0.VnH(), p1.VnH());

  END();

  if (CAN_RUN()) {
    RUN();
    int p2_expected[] = {1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0};
    int p3_expected[] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1};
    ASSERT_EQUAL_SVE(p2_expected, p2.VnB());
    ASSERT_EQUAL_SVE(p3_expected, p3.VnB());

    int p4_expected[] = {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
    int p5_expected[] = {0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p4_expected, p4.VnB());
    ASSERT_EQUAL_SVE(p5_expected, p5.VnB());

    int p6_expected[] = {0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0};
    int p7_expected[] = {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1};
    ASSERT_EQUAL_SVE(p6_expected, p6.VnB());
    ASSERT_EQUAL_SVE(p7_expected, p7.VnB());

    int p8_expected[] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0};
    int p9_expected[] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p8_expected, p8.VnB());
    ASSERT_EQUAL_SVE(p9_expected, p9.VnB());

    int p10_expected[] = {0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0};
    int p11_expected[] = {0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0};
    ASSERT_EQUAL_SVE(p10_expected, p10.VnB());
    ASSERT_EQUAL_SVE(p11_expected, p11.VnB());

    int p12_expected[] = {0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0};
    int p13_expected[] = {0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0};
    ASSERT_EQUAL_SVE(p12_expected, p12.VnB());
    ASSERT_EQUAL_SVE(p13_expected, p13.VnB());
  }
}

TEST_SVE(sve_trn_p_sd) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  Initialise(&masm, p0.VnB(), 0x55a55aaa);
  __ Pfalse(p1.VnB());

  __ Trn1(p2.VnS(), p0.VnS(), p0.VnS());
  __ Trn2(p3.VnS(), p0.VnS(), p0.VnS());
  __ Trn1(p4.VnS(), p1.VnS(), p0.VnS());
  __ Trn2(p5.VnS(), p1.VnS(), p0.VnS());
  __ Trn1(p6.VnS(), p0.VnS(), p1.VnS());
  __ Trn2(p7.VnS(), p0.VnS(), p1.VnS());

  __ Trn1(p8.VnD(), p0.VnD(), p0.VnD());
  __ Trn2(p9.VnD(), p0.VnD(), p0.VnD());
  __ Trn1(p10.VnD(), p1.VnD(), p0.VnD());
  __ Trn2(p11.VnD(), p1.VnD(), p0.VnD());
  __ Trn1(p12.VnD(), p0.VnD(), p1.VnD());
  __ Trn2(p13.VnD(), p0.VnD(), p1.VnD());

  END();

  if (CAN_RUN()) {
    RUN();
    int p2_expected[] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    int p3_expected[] = {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p2_expected, p2.VnB());
    ASSERT_EQUAL_SVE(p3_expected, p3.VnB());

    int p4_expected[] = {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
    int p5_expected[] = {0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
    ASSERT_EQUAL_SVE(p4_expected, p4.VnB());
    ASSERT_EQUAL_SVE(p5_expected, p5.VnB());

    int p6_expected[] = {0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0};
    int p7_expected[] = {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p6_expected, p6.VnB());
    ASSERT_EQUAL_SVE(p7_expected, p7.VnB());

    int p8_expected[] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    int p9_expected[] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p8_expected, p8.VnB());
    ASSERT_EQUAL_SVE(p9_expected, p9.VnB());

    int p10_expected[] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int p11_expected[] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    ASSERT_EQUAL_SVE(p10_expected, p10.VnB());
    ASSERT_EQUAL_SVE(p11_expected, p11.VnB());

    int p12_expected[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    int p13_expected[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p12_expected, p12.VnB());
    ASSERT_EQUAL_SVE(p13_expected, p13.VnB());
  }
}

TEST_SVE(sve_zip_p_bh) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  Initialise(&masm,
             p0.VnB(),
             0x5a5a5a5a5a5a5a5a,
             0x5a5a5a5a5a5a5a5a,
             0x5a5a5a5a5a5a5a5a,
             0x5a5a5a5a5a5a5a5a);
  __ Pfalse(p1.VnB());

  __ Zip1(p2.VnB(), p0.VnB(), p0.VnB());
  __ Zip2(p3.VnB(), p0.VnB(), p0.VnB());
  __ Zip1(p4.VnB(), p1.VnB(), p0.VnB());
  __ Zip2(p5.VnB(), p1.VnB(), p0.VnB());
  __ Zip1(p6.VnB(), p0.VnB(), p1.VnB());
  __ Zip2(p7.VnB(), p0.VnB(), p1.VnB());

  __ Zip1(p8.VnH(), p0.VnH(), p0.VnH());
  __ Zip2(p9.VnH(), p0.VnH(), p0.VnH());
  __ Zip1(p10.VnH(), p1.VnH(), p0.VnH());
  __ Zip2(p11.VnH(), p1.VnH(), p0.VnH());
  __ Zip1(p12.VnH(), p0.VnH(), p1.VnH());
  __ Zip2(p13.VnH(), p0.VnH(), p1.VnH());

  END();

  if (CAN_RUN()) {
    RUN();
    int p2_expected[] = {0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0};
    int p3_expected[] = {0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0};
    ASSERT_EQUAL_SVE(p2_expected, p2.VnB());
    ASSERT_EQUAL_SVE(p3_expected, p3.VnB());

    int p4_expected[] = {0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0};
    int p5_expected[] = {0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0};
    ASSERT_EQUAL_SVE(p4_expected, p4.VnB());
    ASSERT_EQUAL_SVE(p5_expected, p5.VnB());

    int p6_expected[] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0};
    int p7_expected[] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0};
    ASSERT_EQUAL_SVE(p6_expected, p6.VnB());
    ASSERT_EQUAL_SVE(p7_expected, p7.VnB());

    int p8_expected[] = {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0};
    int p9_expected[] = {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p8_expected, p8.VnB());
    ASSERT_EQUAL_SVE(p9_expected, p9.VnB());

    int p10_expected[] = {0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
    int p11_expected[] = {0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
    ASSERT_EQUAL_SVE(p10_expected, p10.VnB());
    ASSERT_EQUAL_SVE(p11_expected, p11.VnB());

    int p12_expected[] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0};
    int p13_expected[] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0};
    ASSERT_EQUAL_SVE(p12_expected, p12.VnB());
    ASSERT_EQUAL_SVE(p13_expected, p13.VnB());
  }
}

TEST_SVE(sve_zip_p_sd) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  Initialise(&masm,
             p0.VnB(),
             0x5a5a5a5a5a5a5a5a,
             0x5a5a5a5a5a5a5a5a,
             0x5a5a5a5a5a5a5a5a,
             0x5a5a5a5a5a5a5a5a);
  __ Pfalse(p1.VnB());

  __ Zip1(p2.VnS(), p0.VnS(), p0.VnS());
  __ Zip2(p3.VnS(), p0.VnS(), p0.VnS());
  __ Zip1(p4.VnS(), p1.VnS(), p0.VnS());
  __ Zip2(p5.VnS(), p1.VnS(), p0.VnS());
  __ Zip1(p6.VnS(), p0.VnS(), p1.VnS());
  __ Zip2(p7.VnS(), p0.VnS(), p1.VnS());

  __ Zip1(p8.VnD(), p0.VnD(), p0.VnD());
  __ Zip2(p9.VnD(), p0.VnD(), p0.VnD());
  __ Zip1(p10.VnD(), p1.VnD(), p0.VnD());
  __ Zip2(p11.VnD(), p1.VnD(), p0.VnD());
  __ Zip1(p12.VnD(), p0.VnD(), p1.VnD());
  __ Zip2(p13.VnD(), p0.VnD(), p1.VnD());

  END();

  if (CAN_RUN()) {
    RUN();
    int p2_expected[] = {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0};
    int p3_expected[] = {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p2_expected, p2.VnB());
    ASSERT_EQUAL_SVE(p3_expected, p3.VnB());

    int p4_expected[] = {0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
    int p5_expected[] = {0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
    ASSERT_EQUAL_SVE(p4_expected, p4.VnB());
    ASSERT_EQUAL_SVE(p5_expected, p5.VnB());

    int p6_expected[] = {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0};
    int p7_expected[] = {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p6_expected, p6.VnB());
    ASSERT_EQUAL_SVE(p7_expected, p7.VnB());

    int p8_expected[] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0};
    int p9_expected[] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p8_expected, p8.VnB());
    ASSERT_EQUAL_SVE(p9_expected, p9.VnB());

    int p10_expected[] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int p11_expected[] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    ASSERT_EQUAL_SVE(p10_expected, p10.VnB());
    ASSERT_EQUAL_SVE(p11_expected, p11.VnB());

    int p12_expected[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0};
    int p13_expected[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0};
    ASSERT_EQUAL_SVE(p12_expected, p12.VnB());
    ASSERT_EQUAL_SVE(p13_expected, p13.VnB());
  }
}

TEST_SVE(sve_uzp_p) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  Initialise(&masm,
             p0.VnB(),
             0xf0f0ff00ffff0000,
             0x4242424242424242,
             0x5a5a5a5a5a5a5a5a,
             0x0123456789abcdef);
  __ Rev(p1.VnB(), p0.VnB());

  __ Zip1(p2.VnB(), p0.VnB(), p1.VnB());
  __ Zip2(p3.VnB(), p0.VnB(), p1.VnB());
  __ Uzp1(p4.VnB(), p2.VnB(), p3.VnB());
  __ Uzp2(p5.VnB(), p2.VnB(), p3.VnB());

  __ Zip1(p2.VnH(), p0.VnH(), p1.VnH());
  __ Zip2(p3.VnH(), p0.VnH(), p1.VnH());
  __ Uzp1(p6.VnH(), p2.VnH(), p3.VnH());
  __ Uzp2(p7.VnH(), p2.VnH(), p3.VnH());

  __ Zip1(p2.VnS(), p0.VnS(), p1.VnS());
  __ Zip2(p3.VnS(), p0.VnS(), p1.VnS());
  __ Uzp1(p8.VnS(), p2.VnS(), p3.VnS());
  __ Uzp2(p9.VnS(), p2.VnS(), p3.VnS());

  __ Zip1(p2.VnD(), p0.VnD(), p1.VnD());
  __ Zip2(p3.VnD(), p0.VnD(), p1.VnD());
  __ Uzp1(p10.VnD(), p2.VnD(), p3.VnD());
  __ Uzp2(p11.VnD(), p2.VnD(), p3.VnD());

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(p0, p4);
    ASSERT_EQUAL_SVE(p1, p5);
    ASSERT_EQUAL_SVE(p0, p6);
    ASSERT_EQUAL_SVE(p1, p7);
    ASSERT_EQUAL_SVE(p0, p8);
    ASSERT_EQUAL_SVE(p1, p9);
    ASSERT_EQUAL_SVE(p0, p10);
    ASSERT_EQUAL_SVE(p1, p11);
  }
}

TEST_SVE(sve_punpk) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  Initialise(&masm,
             p0.VnB(),
             0xf0a0f0a0f0a0f0a0,
             0xf0a0f0a0f0a0f0a0,
             0xa0f0a0f0a0f0a0f0,
             0xa0f0a0f0a0f0a0f0);
  __ Punpklo(p1.VnH(), p0.VnB());
  __ Punpkhi(p2.VnH(), p0.VnB());

  END();

  if (CAN_RUN()) {
    RUN();

    int p1_expected[] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    int p2_expected[] = {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    ASSERT_EQUAL_SVE(p1_expected, p1.VnB());
    ASSERT_EQUAL_SVE(p2_expected, p2.VnB());
  }
}

typedef void (MacroAssembler::*BrkFn)(const PRegisterWithLaneSize& pd,
                                      const PRegister& pg,
                                      const PRegisterWithLaneSize& pn);

typedef void (MacroAssembler::*BrksFn)(const PRegisterWithLaneSize& pd,
                                       const PRegisterZ& pg,
                                       const PRegisterWithLaneSize& pn);

template <typename T, size_t N>
static void BrkaBrkbHelper(Test* config,
                           BrkFn macro,
                           BrksFn macro_set_flags,
                           const T (&pd_inputs)[N],
                           const T (&pg_inputs)[N],
                           const T (&pn_inputs)[N],
                           const T (&pd_z_expected)[N]) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  PRegister pg = p10;
  PRegister pn = p9;
  PRegister pd_z = p0;
  PRegister pd_z_s = p1;
  PRegister pd_m = p2;
  Initialise(&masm, pg.VnB(), pg_inputs);
  Initialise(&masm, pn.VnB(), pn_inputs);
  Initialise(&masm, pd_m.VnB(), pd_inputs);

  // Initialise NZCV to an impossible value, to check that we actually write it.
  __ Mov(x10, NZCVFlag);
  __ Msr(NZCV, x10);

  (masm.*macro)(pd_z.VnB(), pg.Zeroing(), pn.VnB());
  (masm.*macro_set_flags)(pd_z_s.VnB(), pg.Zeroing(), pn.VnB());
  __ Mrs(x0, NZCV);

  (masm.*macro)(pd_m.VnB(), pg.Merging(), pn.VnB());

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(pd_z_expected, pd_z.VnB());

    // Check that the flags were properly set.
    StatusFlags nzcv_expected =
        GetPredTestFlags(pd_z_expected,
                         pg_inputs,
                         core.GetSVELaneCount(kBRegSize));
    ASSERT_EQUAL_64(nzcv_expected, x0);
    ASSERT_EQUAL_SVE(pd_z.VnB(), pd_z_s.VnB());

    T pd_m_expected[N];
    // Set expected `pd` result on merging predication.
    for (size_t i = 0; i < N; i++) {
      pd_m_expected[i] = pg_inputs[i] ? pd_z_expected[i] : pd_inputs[i];
    }
    ASSERT_EQUAL_SVE(pd_m_expected, pd_m.VnB());
  }
}

template <typename T>
static void BrkaHelper(Test* config,
                       const T& pd_inputs,
                       const T& pg_inputs,
                       const T& pn_inputs,
                       const T& pd_expected) {
  BrkaBrkbHelper(config,
                 &MacroAssembler::Brka,
                 &MacroAssembler::Brkas,
                 pd_inputs,
                 pg_inputs,
                 pn_inputs,
                 pd_expected);
}

TEST_SVE(sve_brka) {
  // clang-format off
  //                              | boundary of 128-bits VL.
  //                              v
  int pd[] =      {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  //               | highest-numbered lane                lowest-numbered lane |
  //               v                                                           v
  int pg_1[] =    {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};
  int pg_2[] =    {1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};

  int pn_1[] =    {1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
  int pn_2[] =    {1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int pn_3[] =    {1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1};

  //                                                                  | first break
  //                                                                  v
  int exp_1_1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0};
  //                              | first break
  //                              v
  int exp_1_2[] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};
  //                                                      | first break
  //                                                      v
  int exp_1_3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};

  BrkaHelper(config, pd, pg_1, pn_1, exp_1_1);
  BrkaHelper(config, pd, pg_1, pn_2, exp_1_2);
  BrkaHelper(config, pd, pg_1, pn_3, exp_1_3);

  //                                                               | first break
  //                                                               v
  int exp_2_1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1};
  //                                       | first break
  //                                       v
  int exp_2_2[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};
  //                                                                           | first break
  //                                                                           v
  int exp_2_3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  BrkaHelper(config, pd, pg_2, pn_1, exp_2_1);
  BrkaHelper(config, pd, pg_2, pn_2, exp_2_2);
  BrkaHelper(config, pd, pg_2, pn_3, exp_2_3);

  // The all-inactive zeroing predicate sets destination predicate all-false.
  int pg_3[] =    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp_3_x[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  BrkaHelper(config, pd, pg_3, pn_1, exp_3_x);
  BrkaHelper(config, pd, pg_3, pn_2, exp_3_x);
  BrkaHelper(config, pd, pg_3, pn_3, exp_3_x);
  // clang-format on
}

template <typename T>
static void BrkbHelper(Test* config,
                       const T& pd_inputs,
                       const T& pg_inputs,
                       const T& pn_inputs,
                       const T& pd_expected) {
  BrkaBrkbHelper(config,
                 &MacroAssembler::Brkb,
                 &MacroAssembler::Brkbs,
                 pd_inputs,
                 pg_inputs,
                 pn_inputs,
                 pd_expected);
}

TEST_SVE(sve_brkb) {
  // clang-format off
  //                              | boundary of 128-bits VL.
  //                              v
  int pd[] =      {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  //               | highest-numbered lane                lowest-numbered lane |
  //               v                                                           v
  int pg_1[] =    {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};
  int pg_2[] =    {1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};

  int pn_1[] =    {1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0};
  int pn_2[] =    {1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int pn_3[] =    {1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1};

  //                                                                  | first break
  //                                                                  v
  int exp_1_1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
  //                              | first break
  //                              v
  int exp_1_2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};
  //                                                      | first break
  //                                                      v
  int exp_1_3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0};

  BrkbHelper(config, pd, pg_1, pn_1, exp_1_1);
  BrkbHelper(config, pd, pg_1, pn_2, exp_1_2);
  BrkbHelper(config, pd, pg_1, pn_3, exp_1_3);

  //                                                               | first break
  //                                                               v
  int exp_2_1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1};
  //                                       | first break
  //                                       v
  int exp_2_2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};
  //                                                                           | first break
  //                                                                           v
  int exp_2_3[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  BrkbHelper(config, pd, pg_2, pn_1, exp_2_1);
  BrkbHelper(config, pd, pg_2, pn_2, exp_2_2);
  BrkbHelper(config, pd, pg_2, pn_3, exp_2_3);

  // The all-inactive zeroing predicate sets destination predicate all-false.
  int pg_3[] =    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int exp_3_x[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  BrkbHelper(config, pd, pg_3, pn_1, exp_3_x);
  BrkbHelper(config, pd, pg_3, pn_2, exp_3_x);
  BrkbHelper(config, pd, pg_3, pn_3, exp_3_x);
  // clang-format on
}

typedef void (MacroAssembler::*BrknFn)(const PRegisterWithLaneSize& pd,
                                       const PRegisterZ& pg,
                                       const PRegisterWithLaneSize& pn,
                                       const PRegisterWithLaneSize& pm);

typedef void (MacroAssembler::*BrknsFn)(const PRegisterWithLaneSize& pd,
                                        const PRegisterZ& pg,
                                        const PRegisterWithLaneSize& pn,
                                        const PRegisterWithLaneSize& pm);

enum BrknDstPredicateState { kAllFalse, kUnchanged };

template <typename T, size_t N>
static void BrknHelper(Test* config,
                       BrknFn macro,
                       BrknsFn macro_set_flags,
                       const T (&pd_inputs)[N],
                       const T (&pg_inputs)[N],
                       const T (&pn_inputs)[N],
                       const T (&pm_inputs)[N],
                       BrknDstPredicateState expected_pd_state) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  PRegister pg = p10;
  PRegister pn = p9;
  PRegister pm = p8;
  PRegister pdm = p0;
  PRegister pd = p1;
  PRegister pd_s = p2;
  Initialise(&masm, pg.VnB(), pg_inputs);
  Initialise(&masm, pn.VnB(), pn_inputs);
  Initialise(&masm, pm.VnB(), pm_inputs);
  Initialise(&masm, pdm.VnB(), pm_inputs);
  Initialise(&masm, pd.VnB(), pd_inputs);
  Initialise(&masm, pd_s.VnB(), pd_inputs);

  // Initialise NZCV to an impossible value, to check that we actually write it.
  __ Mov(x10, NZCVFlag);
  __ Msr(NZCV, x10);

  (masm.*macro)(pdm.VnB(), pg.Zeroing(), pn.VnB(), pdm.VnB());
  // !pd.Aliases(pm).
  (masm.*macro)(pd.VnB(), pg.Zeroing(), pn.VnB(), pm.VnB());
  (masm.*macro_set_flags)(pd_s.VnB(), pg.Zeroing(), pn.VnB(), pm.VnB());
  __ Mrs(x0, NZCV);

  END();

  if (CAN_RUN()) {
    RUN();

    T all_false[N] = {0};
    if (expected_pd_state == kAllFalse) {
      ASSERT_EQUAL_SVE(all_false, pd.VnB());
    } else {
      ASSERT_EQUAL_SVE(pm_inputs, pd.VnB());
    }
    ASSERT_EQUAL_SVE(pm_inputs, pm.VnB());

    // Check that the flags were properly set.
    StatusFlags nzcv_expected =
        GetPredTestFlags((expected_pd_state == kAllFalse) ? all_false
                                                          : pm_inputs,
                         pg_inputs,
                         core.GetSVELaneCount(kBRegSize));
    ASSERT_EQUAL_64(nzcv_expected, x0);
    ASSERT_EQUAL_SVE(pd.VnB(), pdm.VnB());
    ASSERT_EQUAL_SVE(pd.VnB(), pd_s.VnB());
  }
}

TEST_SVE(sve_brkn) {
  // clang-format off
  int pd[] =   {1, 0, 0, 1, 0, 1, 1, 0, 1, 0};
  int pm[] =   {0, 1, 1, 1, 1, 0, 0, 1, 0, 1};

  int pg_1[] =  {1, 1, 0, 0, 1, 0, 1, 1, 0, 0};
  int pg_2[] =  {0, 0, 0, 1, 1, 1, 0, 0, 1, 1};
  int pg_3[] =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // all-false

  int pn_1[] =  {1, 0, 0, 0, 0, 1, 1, 0, 0, 0};
  int pn_2[] =  {0, 1, 0, 1, 0, 0, 0, 0, 0, 0};
  int pn_3[] =  {0, 0, 0, 0, 1, 1, 0, 0, 1, 1};

  BrknHelper(config, &MacroAssembler::Brkn, &MacroAssembler::Brkns, pd, pg_1, pn_1, pm, kUnchanged);
  BrknHelper(config, &MacroAssembler::Brkn, &MacroAssembler::Brkns, pd, pg_1, pn_2, pm, kAllFalse);
  BrknHelper(config, &MacroAssembler::Brkn, &MacroAssembler::Brkns, pd, pg_1, pn_3, pm, kAllFalse);

  BrknHelper(config, &MacroAssembler::Brkn, &MacroAssembler::Brkns, pd, pg_2, pn_1, pm, kAllFalse);
  BrknHelper(config, &MacroAssembler::Brkn, &MacroAssembler::Brkns, pd, pg_2, pn_2, pm, kUnchanged);
  BrknHelper(config, &MacroAssembler::Brkn, &MacroAssembler::Brkns, pd, pg_2, pn_3, pm, kAllFalse);

  BrknHelper(config, &MacroAssembler::Brkn, &MacroAssembler::Brkns, pd, pg_3, pn_1, pm, kAllFalse);
  BrknHelper(config, &MacroAssembler::Brkn, &MacroAssembler::Brkns, pd, pg_3, pn_2, pm, kAllFalse);
  BrknHelper(config, &MacroAssembler::Brkn, &MacroAssembler::Brkns, pd, pg_3, pn_3, pm, kAllFalse);
  // clang-format on
}

TEST_SVE(sve_trn) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in0[] = {0xffeeddccbbaa9988, 0x7766554433221100};
  uint64_t in1[] = {0xaa55aa55aa55aa55, 0x55aa55aa55aa55aa};
  InsrHelper(&masm, z0.VnD(), in0);
  InsrHelper(&masm, z1.VnD(), in1);

  __ Trn1(z2.VnB(), z0.VnB(), z1.VnB());
  __ Trn2(z3.VnB(), z0.VnB(), z1.VnB());
  __ Trn1(z4.VnH(), z0.VnH(), z1.VnH());
  __ Trn2(z5.VnH(), z0.VnH(), z1.VnH());
  __ Trn1(z6.VnS(), z0.VnS(), z1.VnS());
  __ Trn2(z7.VnS(), z0.VnS(), z1.VnS());
  __ Trn1(z8.VnD(), z0.VnD(), z1.VnD());
  __ Trn2(z9.VnD(), z0.VnD(), z1.VnD());

  END();

  if (CAN_RUN()) {
    RUN();
    uint64_t expected_z2[] = {0x55ee55cc55aa5588, 0xaa66aa44aa22aa00};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());
    uint64_t expected_z3[] = {0xaaffaaddaabbaa99, 0x5577555555335511};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());
    uint64_t expected_z4[] = {0xaa55ddccaa559988, 0x55aa554455aa1100};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());
    uint64_t expected_z5[] = {0xaa55ffeeaa55bbaa, 0x55aa776655aa3322};
    ASSERT_EQUAL_SVE(expected_z5, z5.VnD());
    uint64_t expected_z6[] = {0xaa55aa55bbaa9988, 0x55aa55aa33221100};
    ASSERT_EQUAL_SVE(expected_z6, z6.VnD());
    uint64_t expected_z7[] = {0xaa55aa55ffeeddcc, 0x55aa55aa77665544};
    ASSERT_EQUAL_SVE(expected_z7, z7.VnD());
    uint64_t expected_z8[] = {0x55aa55aa55aa55aa, 0x7766554433221100};
    ASSERT_EQUAL_SVE(expected_z8, z8.VnD());
    uint64_t expected_z9[] = {0xaa55aa55aa55aa55, 0xffeeddccbbaa9988};
    ASSERT_EQUAL_SVE(expected_z9, z9.VnD());
  }
}

TEST_SVE(sve_zip_uzp) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  __ Dup(z0.VnD(), 0xffeeddccbbaa9988);
  __ Insr(z0.VnD(), 0x7766554433221100);
  __ Dup(z1.VnD(), 0xaa55aa55aa55aa55);
  __ Insr(z1.VnD(), 0x55aa55aa55aa55aa);

  __ Zip1(z2.VnB(), z0.VnB(), z1.VnB());
  __ Zip2(z3.VnB(), z0.VnB(), z1.VnB());
  __ Zip1(z4.VnH(), z0.VnH(), z1.VnH());
  __ Zip2(z5.VnH(), z0.VnH(), z1.VnH());
  __ Zip1(z6.VnS(), z0.VnS(), z1.VnS());
  __ Zip2(z7.VnS(), z0.VnS(), z1.VnS());
  __ Zip1(z8.VnD(), z0.VnD(), z1.VnD());
  __ Zip2(z9.VnD(), z0.VnD(), z1.VnD());

  __ Uzp1(z10.VnB(), z2.VnB(), z3.VnB());
  __ Uzp2(z11.VnB(), z2.VnB(), z3.VnB());
  __ Uzp1(z12.VnH(), z4.VnH(), z5.VnH());
  __ Uzp2(z13.VnH(), z4.VnH(), z5.VnH());
  __ Uzp1(z14.VnS(), z6.VnS(), z7.VnS());
  __ Uzp2(z15.VnS(), z6.VnS(), z7.VnS());
  __ Uzp1(z16.VnD(), z8.VnD(), z9.VnD());
  __ Uzp2(z17.VnD(), z8.VnD(), z9.VnD());

  END();

  if (CAN_RUN()) {
    RUN();
    uint64_t expected_z2[] = {0x5577aa665555aa44, 0x5533aa225511aa00};
    ASSERT_EQUAL_SVE(expected_z2, z2.VnD());
    uint64_t expected_z3[] = {0xaaff55eeaadd55cc, 0xaabb55aaaa995588};
    ASSERT_EQUAL_SVE(expected_z3, z3.VnD());
    uint64_t expected_z4[] = {0x55aa776655aa5544, 0x55aa332255aa1100};
    ASSERT_EQUAL_SVE(expected_z4, z4.VnD());
    uint64_t expected_z5[] = {0xaa55ffeeaa55ddcc, 0xaa55bbaaaa559988};
    ASSERT_EQUAL_SVE(expected_z5, z5.VnD());
    uint64_t expected_z6[] = {0x55aa55aa77665544, 0x55aa55aa33221100};
    ASSERT_EQUAL_SVE(expected_z6, z6.VnD());
    uint64_t expected_z7[] = {0xaa55aa55ffeeddcc, 0xaa55aa55bbaa9988};
    ASSERT_EQUAL_SVE(expected_z7, z7.VnD());
    uint64_t expected_z8[] = {0x55aa55aa55aa55aa, 0x7766554433221100};
    ASSERT_EQUAL_SVE(expected_z8, z8.VnD());
    uint64_t expected_z9[] = {0xaa55aa55aa55aa55, 0xffeeddccbbaa9988};
    ASSERT_EQUAL_SVE(expected_z9, z9.VnD());

    // Check uzp is the opposite of zip.
    ASSERT_EQUAL_SVE(z0.VnD(), z10.VnD());
    ASSERT_EQUAL_SVE(z1.VnD(), z11.VnD());
    ASSERT_EQUAL_SVE(z0.VnD(), z12.VnD());
    ASSERT_EQUAL_SVE(z1.VnD(), z13.VnD());
    ASSERT_EQUAL_SVE(z0.VnD(), z14.VnD());
    ASSERT_EQUAL_SVE(z1.VnD(), z15.VnD());
    ASSERT_EQUAL_SVE(z0.VnD(), z16.VnD());
    ASSERT_EQUAL_SVE(z1.VnD(), z17.VnD());
  }
}

TEST_SVE(sve_fpmul_index) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in0[] = {0x3ff000003f803c00, 0xbff00000bf80bc00};
  uint64_t in1[] = {0x3ff012343ff03c76, 0xbff01234bff0bc76};

  InsrHelper(&masm, z0.VnD(), in0);
  InsrHelper(&masm, z1.VnD(), in1);

  __ Fmul(z2.VnH(), z1.VnH(), z0.VnH(), 0);
  __ Fmul(z3.VnH(), z1.VnH(), z0.VnH(), 1);
  __ Fmul(z4.VnH(), z1.VnH(), z0.VnH(), 4);
  __ Fmul(z5.VnH(), z1.VnH(), z0.VnH(), 7);

  __ Fmul(z6.VnS(), z1.VnS(), z0.VnS(), 0);
  __ Fmul(z7.VnS(), z1.VnS(), z0.VnS(), 1);
  __ Fmul(z8.VnS(), z1.VnS(), z0.VnS(), 2);
  __ Fmul(z9.VnS(), z1.VnS(), z0.VnS(), 3);

  __ Fmul(z10.VnD(), z1.VnD(), z0.VnD(), 0);
  __ Fmul(z11.VnD(), z1.VnD(), z0.VnD(), 1);

  // Compute the results using other instructions.
  __ Dup(z12.VnH(), z0.VnH(), 0);
  __ Fmul(z12.VnH(), z1.VnH(), z12.VnH());
  __ Dup(z13.VnH(), z0.VnH(), 1);
  __ Fmul(z13.VnH(), z1.VnH(), z13.VnH());
  __ Dup(z14.VnH(), z0.VnH(), 4);
  __ Fmul(z14.VnH(), z1.VnH(), z14.VnH());
  __ Dup(z15.VnH(), z0.VnH(), 7);
  __ Fmul(z15.VnH(), z1.VnH(), z15.VnH());

  __ Dup(z16.VnS(), z0.VnS(), 0);
  __ Fmul(z16.VnS(), z1.VnS(), z16.VnS());
  __ Dup(z17.VnS(), z0.VnS(), 1);
  __ Fmul(z17.VnS(), z1.VnS(), z17.VnS());
  __ Dup(z18.VnS(), z0.VnS(), 2);
  __ Fmul(z18.VnS(), z1.VnS(), z18.VnS());
  __ Dup(z19.VnS(), z0.VnS(), 3);
  __ Fmul(z19.VnS(), z1.VnS(), z19.VnS());

  __ Dup(z20.VnD(), z0.VnD(), 0);
  __ Fmul(z20.VnD(), z1.VnD(), z20.VnD());
  __ Dup(z21.VnD(), z0.VnD(), 1);
  __ Fmul(z21.VnD(), z1.VnD(), z21.VnD());

  END();

  if (CAN_RUN()) {
    RUN();
    ASSERT_EQUAL_SVE(z12.VnH(), z2.VnH());
    ASSERT_EQUAL_SVE(z13.VnH(), z3.VnH());
    ASSERT_EQUAL_SVE(z14.VnH(), z4.VnH());
    ASSERT_EQUAL_SVE(z15.VnH(), z5.VnH());
    ASSERT_EQUAL_SVE(z16.VnS(), z6.VnS());
    ASSERT_EQUAL_SVE(z17.VnS(), z7.VnS());
    ASSERT_EQUAL_SVE(z18.VnS(), z8.VnS());
    ASSERT_EQUAL_SVE(z19.VnS(), z9.VnS());
    ASSERT_EQUAL_SVE(z20.VnD(), z10.VnD());
    ASSERT_EQUAL_SVE(z21.VnD(), z11.VnD());
  }
}

TEST_SVE(sve_ftmad) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  uint64_t in_h0[] = {0x7c027e01fc02fe01,
                      0x3c003c00bc00bc00,
                      0x3c003c00bc00bc00};
  uint64_t in_h1[] = {0xfe01fc027e017e01,
                      0x3c00bc003c00bc00,
                      0x3c00bc003c00bc00};
  uint64_t in_s0[] = {0x7f800002ffc00001,
                      0x3f8000003f800000,
                      0xbf800000bf800000};
  uint64_t in_s1[] = {0xffc00001ffc00001,
                      0x3f800000bf800000,
                      0x3f800000bf800000};
  uint64_t in_d0[] = {0x7ff8000000000001,
                      0x3ff0000000000000,
                      0xbff0000000000000};
  uint64_t in_d1[] = {0xfff0000000000002,
                      0xbff0000000000000,
                      0x3ff0000000000000};
  InsrHelper(&masm, z0.VnD(), in_h0);
  InsrHelper(&masm, z1.VnD(), in_h1);
  InsrHelper(&masm, z2.VnD(), in_s0);
  InsrHelper(&masm, z3.VnD(), in_s1);
  InsrHelper(&masm, z4.VnD(), in_d0);
  InsrHelper(&masm, z5.VnD(), in_d1);

  __ Mov(z6, z0);
  __ Ftmad(z6.VnH(), z6.VnH(), z1.VnH(), 0);
  __ Mov(z7, z0);
  __ Ftmad(z7.VnH(), z7.VnH(), z1.VnH(), 1);
  __ Mov(z8, z0);
  __ Ftmad(z8.VnH(), z8.VnH(), z1.VnH(), 2);

  __ Mov(z9, z2);
  __ Ftmad(z9.VnS(), z9.VnS(), z3.VnS(), 0);
  __ Mov(z10, z2);
  __ Ftmad(z10.VnS(), z10.VnS(), z3.VnS(), 3);
  __ Mov(z11, z2);
  __ Ftmad(z11.VnS(), z11.VnS(), z3.VnS(), 4);

  __ Mov(z12, z4);
  __ Ftmad(z12.VnD(), z12.VnD(), z5.VnD(), 0);
  __ Mov(z13, z4);
  __ Ftmad(z13.VnD(), z13.VnD(), z5.VnD(), 5);
  __ Mov(z14, z4);
  __ Ftmad(z14.VnD(), z14.VnD(), z5.VnD(), 7);

  END();

  if (CAN_RUN()) {
    RUN();
    uint64_t expected_z6[] = {0x7e027e02fe02fe01,
                              0x4000400000000000,
                              0x4000400000000000};
    ASSERT_EQUAL_SVE(expected_z6, z6.VnD());
    uint64_t expected_z7[] = {0x7e027e02fe02fe01,
                              0x3aab3800bcabbe00,
                              0x3aab3800bcabbe00};
    ASSERT_EQUAL_SVE(expected_z7, z7.VnD());
    uint64_t expected_z8[] = {0x7e027e02fe02fe01,
                              0x3c083c2abbefbbac,
                              0x3c083c2abbefbbac};
    ASSERT_EQUAL_SVE(expected_z8, z8.VnD());
    uint64_t expected_z9[] = {0x7fc00002ffc00001,
                              0x4000000040000000,
                              0x0000000000000000};
    ASSERT_EQUAL_SVE(expected_z9, z9.VnD());
    uint64_t expected_z10[] = {0x7fc00002ffc00001,
                               0x3f7ff2ff3f7fa4fc,
                               0xbf800680bf802d82};
    ASSERT_EQUAL_SVE(expected_z10, z10.VnD());
    uint64_t expected_z11[] = {0x7fc00002ffc00001,
                               0x3f8000173f8000cd,
                               0xbf7fffd2bf7ffe66};
    ASSERT_EQUAL_SVE(expected_z11, z11.VnD());
    uint64_t expected_z12[] = {0x7ff8000000000002,
                               0x4000000000000000,
                               0x0000000000000000};
    ASSERT_EQUAL_SVE(expected_z12, z12.VnD());
    uint64_t expected_z13[] = {0x7ff8000000000002,
                               0x3fefffff6c0d846c,
                               0xbff0000006b978ae};
    ASSERT_EQUAL_SVE(expected_z13, z13.VnD());
    uint64_t expected_z14[] = {0x7ff8000000000002,
                               0x3feffffffffe708a,
                               0xbff0000000000000};
    ASSERT_EQUAL_SVE(expected_z14, z14.VnD());
  }
}

typedef void (MacroAssembler::*FcvtFn)(const ZRegister& zd,
                                       const PRegisterM& pg,
                                       const ZRegister& zn);

template <typename F, size_t N>
static void TestFcvtzsFcvtzuHelper(
    Test* config,
    FcvtFn macro,
    int dst_type_size_in_bits,
    int src_type_size_in_bits,
    const F (&zn_inputs)[N],
    const int (&pg_inputs)[N],
    const uint64_t (&zd_expected_all_active)[N]) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // If the input and result types have a different size, the instruction
  // options on elements of the largest specified type is determined by the
  // larger type.
  int lane_size_in_bits =
      std::max(dst_type_size_in_bits, src_type_size_in_bits);

  ZRegister zd_all_active = z25;
  ZRegister zd_merged = z26;
  ZRegister zn = z27;

  uint64_t zn_rawbits[N];
  FPToRawbitsWithSize(zn_inputs, zn_rawbits, src_type_size_in_bits);
  InsrHelper(&masm, zn.WithLaneSize(lane_size_in_bits), zn_rawbits);

  PRegisterWithLaneSize pg_all_active = p0.WithLaneSize(lane_size_in_bits);
  __ Ptrue(pg_all_active);

  // Test floating-point conversions with all lanes actived.
  (masm.*macro)(zd_all_active.WithLaneSize(dst_type_size_in_bits),
                pg_all_active.Merging(),
                zn.WithLaneSize(src_type_size_in_bits));

  PRegisterWithLaneSize pg_merged = p1.WithLaneSize(lane_size_in_bits);
  Initialise(&masm, pg_merged, pg_inputs);

  __ Dup(zd_merged.VnD(), 0x0bad0bad0bad0bad);

  // Use the same `zn` inputs to test floating-point conversions but partial
  // lanes are set inactive.
  (masm.*macro)(zd_merged.WithLaneSize(dst_type_size_in_bits),
                pg_merged.Merging(),
                zn.WithLaneSize(src_type_size_in_bits));

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(zd_expected_all_active,
                     zd_all_active.WithLaneSize(lane_size_in_bits));

    uint64_t zd_expected_merged[N];
    for (unsigned i = 0; i < N; i++) {
      zd_expected_merged[i] =
          pg_inputs[i] ? zd_expected_all_active[i]
                       : 0x0bad0bad0bad0bad & GetUintMask(lane_size_in_bits);
    }
    ASSERT_EQUAL_SVE(zd_expected_merged,
                     zd_merged.WithLaneSize(lane_size_in_bits));
  }
}

TEST_SVE(fcvtzs_fcvtzu_float16) {
  // clang-format off
  const double h_max_float16 = kHMaxInt;        // Largest float16 == INT16_MAX.
  const double h_min_float16 = -h_max_float16;  // Smallest float16 > INT16_MIN.
  const double largest_float16 = 0xffe0;        // 65504
  const double smallest_float16 = -largest_float16;
  const double h_max_int_sub_one = kHMaxInt - 1;
  const double h_min_int_add_one = kHMinInt + 1;

  double zn_inputs[] = {1.0,
                        1.1,
                        1.5,
                        -1.5,
                        h_max_float16,
                        h_min_float16,
                        largest_float16,
                        smallest_float16,
                        kFP64PositiveInfinity,
                        kFP64NegativeInfinity,
                        h_max_int_sub_one,
                        h_min_int_add_one};

  int pg_inputs[] = {0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0};

  uint64_t expected_fcvtzs_fp162h[] = {1,
                                       1,
                                       1,
                                       0xffff,
                                       0x7fff,
                                       0x8000,
                                       0x7fff,
                                       0x8000,
                                       0x7fff,
                                       0x8000,
                                       0x7fff,
                                       0x8000};

  uint64_t expected_fcvtzu_fp162h[] = {1,
                                       1,
                                       1,
                                       0,
                                       0x8000,
                                       0,
                                       0xffe0,
                                       0,
                                       0xffff,
                                       0,
                                       0x8000,
                                       0};

  // Float16 to 16-bit integers.
  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzs,
                         kHRegSize,
                         kHRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzs_fp162h);

  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzu,
                         kHRegSize,
                         kHRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzu_fp162h);

  uint64_t expected_fcvtzs_fp162w[] = {1,
                                       1,
                                       1,
                                       0xffffffff,
                                       0x8000,
                                       0xffff8000,
                                       0xffe0,
                                       0xffff0020,
                                       0x7fffffff,
                                       0x80000000,
                                       0x8000,
                                       0xffff8000};

  uint64_t expected_fcvtzu_fp162w[] = {1,
                                       1,
                                       1,
                                       0,
                                       0x8000,
                                       0,
                                       0xffe0,
                                       0,
                                       0xffffffff,
                                       0,
                                       0x8000,
                                       0};

  // Float16 to 32-bit integers.
  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzs,
                         kSRegSize,
                         kHRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzs_fp162w);

  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzu,
                         kSRegSize,
                         kHRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzu_fp162w);

  uint64_t expected_fcvtzs_fp162x[] = {1,
                                       1,
                                       1,
                                       0xffffffffffffffff,
                                       0x8000,
                                       0xffffffffffff8000,
                                       0xffe0,
                                       0xffffffffffff0020,
                                       0x7fffffffffffffff,
                                       0x8000000000000000,
                                       0x8000,
                                       0xffffffffffff8000};

  uint64_t expected_fcvtzu_fp162x[] = {1,
                                       1,
                                       1,
                                       0,
                                       0x8000,
                                       0,
                                       0xffe0,
                                       0,
                                       0xffffffffffffffff,
                                       0,
                                       0x8000,
                                       0};

  // Float16 to 64-bit integers.
  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzs,
                         kDRegSize,
                         kHRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzs_fp162x);

  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzu,
                         kDRegSize,
                         kHRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzu_fp162x);
  // clang-format on
}

TEST_SVE(fcvtzs_fcvtzu_float) {
  const double w_max_float = 0x7fffff80;          // Largest float < INT32_MAX.
  const double w_min_float = -w_max_float;        // Smallest float > INT32_MIN.
  const double x_max_float = 0x7fffff8000000000;  // Largest float < INT64_MAX.
  const double x_min_float = -x_max_float;        // Smallest float > INT64_MIN.
  const double w_max_int_sub_one = kWMaxInt - 1;
  const double w_min_int_add_one = kWMinInt + 1;
  const double x_max_int_sub_one = kXMaxInt - 1;
  const double x_min_int_add_one = kXMinInt + 1;

  // clang-format off
  double zn_inputs[] = {1.0,
                        1.1,
                        1.5,
                        -1.5,
                        w_max_float,
                        w_min_float,
                        x_max_float,
                        x_min_float,
                        kFP64PositiveInfinity,
                        kFP64NegativeInfinity,
                        w_max_int_sub_one,
                        w_min_int_add_one,
                        x_max_int_sub_one,
                        x_min_int_add_one};

  int pg_inputs[] = {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0};

  uint64_t expected_fcvtzs_s2w[] = {1,
                                    1,
                                    1,
                                    0xffffffff,
                                    0x7fffff80,
                                    0x80000080,
                                    0x7fffffff,
                                    0x80000000,
                                    0x7fffffff,
                                    0x80000000,
                                    0x7fffffff,
                                    0x80000000,
                                    0x7fffffff,
                                    0x80000000};

  uint64_t expected_fcvtzu_s2w[] =  {1,
                                     1,
                                     1,
                                     0,
                                     0x7fffff80,
                                     0,
                                     0xffffffff,
                                     0,
                                     0xffffffff,
                                     0,
                                     0x80000000,
                                     0,
                                     0xffffffff,
                                     0};

  // Float to 32-bit integers.
  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzs,
                         kSRegSize,
                         kSRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzs_s2w);

  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzu,
                         kSRegSize,
                         kSRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzu_s2w);

  uint64_t expected_fcvtzs_s2x[] = {1,
                                    1,
                                    1,
                                    0xffffffffffffffff,
                                    0x7fffff80,
                                    0xffffffff80000080,
                                    0x7fffff8000000000,
                                    0x8000008000000000,
                                    0x7fffffffffffffff,
                                    0x8000000000000000,
                                    0x80000000,
                                    0xffffffff80000000,
                                    0x7fffffffffffffff,
                                    0x8000000000000000};

  uint64_t expected_fcvtzu_s2x[] = {1,
                                    1,
                                    1,
                                    0,
                                    0x7fffff80,
                                    0,
                                    0x7fffff8000000000,
                                    0,
                                    0xffffffffffffffff,
                                    0,
                                    0x0000000080000000,
                                    0,
                                    0x8000000000000000,
                                    0};

  // Float to 64-bit integers.
  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzs,
                         kDRegSize,
                         kSRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzs_s2x);

  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzu,
                         kDRegSize,
                         kSRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzu_s2x);
  // clang-format on
}

TEST_SVE(fcvtzs_fcvtzu_double) {
  // clang-format off
  const double w_max_float = 0x7fffff80;          // Largest float < INT32_MAX.
  const double w_min_float = -w_max_float;        // Smallest float > INT32_MIN.
  const double x_max_float = 0x7fffff8000000000;  // Largest float < INT64_MAX.
  const double x_min_float = -x_max_float;        // Smallest float > INT64_MIN.
  const double w_max_double = kWMaxInt;           // Largest double == INT32_MAX.
  const double w_min_double = -w_max_double;      // Smallest double > INT32_MIN.
  const double x_max_double = 0x7ffffffffffffc00; // Largest double < INT64_MAX.
  const double x_min_double = -x_max_double;      // Smallest double > INT64_MIN.
  const double w_max_int_sub_one = kWMaxInt - 1;
  const double w_min_int_add_one = kWMinInt + 1;
  const double x_max_int_sub_one = kXMaxInt - 1;
  const double x_min_int_add_one = kXMinInt + 1;

  double zn_inputs[] = {1.0,
                        1.1,
                        1.5,
                        -1.5,
                        w_max_float,
                        w_min_float,
                        x_max_float,
                        x_min_float,
                        w_max_double,
                        w_min_double,
                        x_max_double,
                        x_min_double,
                        kFP64PositiveInfinity,
                        kFP64NegativeInfinity,
                        w_max_int_sub_one,
                        w_min_int_add_one,
                        x_max_int_sub_one,
                        x_min_int_add_one};

  int pg_inputs[] = {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0};

  uint64_t expected_fcvtzs_d2w[] = {1,
                                    1,
                                    1,
                                    0xffffffffffffffff,
                                    0x7fffff80,
                                    0xffffffff80000080,
                                    0x7fffffff,
                                    0xffffffff80000000,
                                    0x7fffffff,
                                    0xffffffff80000001,
                                    0x7fffffff,
                                    0xffffffff80000000,
                                    0x7fffffff,
                                    0xffffffff80000000,
                                    0x7ffffffe,
                                    0xffffffff80000001,
                                    0x7fffffff,
                                    0xffffffff80000000};

  uint64_t expected_fcvtzu_d2w[] = {1,
                                    1,
                                    1,
                                    0,
                                    0x7fffff80,
                                    0,
                                    0xffffffff,
                                    0,
                                    0x7fffffff,
                                    0,
                                    0xffffffff,
                                    0,
                                    0xffffffff,
                                    0,
                                    0x7ffffffe,
                                    0,
                                    0xffffffff,
                                    0};

  // Double to 32-bit integers.
  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzs,
                         kSRegSize,
                         kDRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzs_d2w);

  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzu,
                         kSRegSize,
                         kDRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzu_d2w);

  uint64_t expected_fcvtzs_d2x[] = {1,
                                    1,
                                    1,
                                    0xffffffffffffffff,
                                    0x7fffff80,
                                    0xffffffff80000080,
                                    0x7fffff8000000000,
                                    0x8000008000000000,
                                    0x7fffffff,
                                    0xffffffff80000001,
                                    0x7ffffffffffffc00,
                                    0x8000000000000400,
                                    0x7fffffffffffffff,
                                    0x8000000000000000,
                                    0x7ffffffe,
                                    0xffffffff80000001,
                                    0x7fffffffffffffff,
                                    0x8000000000000000};

  uint64_t expected_fcvtzu_d2x[] = {1,
                                    1,
                                    1,
                                    0,
                                    0x7fffff80,
                                    0,
                                    0x7fffff8000000000,
                                    0,
                                    0x7fffffff,
                                    0,
                                    0x7ffffffffffffc00,
                                    0,
                                    0xffffffffffffffff,
                                    0,
                                    0x000000007ffffffe,
                                    0,
                                    0x8000000000000000,
                                    0};

  // Double to 64-bit integers.
  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzs,
                         kDRegSize,
                         kDRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzs_d2x);

  TestFcvtzsFcvtzuHelper(config,
                         &MacroAssembler::Fcvtzu,
                         kDRegSize,
                         kDRegSize,
                         zn_inputs,
                         pg_inputs,
                         expected_fcvtzu_d2x);
  // clang-format on
}

struct CvtfTestDataSet {
  uint64_t int_value;
  uint64_t scvtf_result;
  uint64_t ucvtf_result;
};

template <size_t N>
static void TestUScvtfHelper(Test* config,
                             int dst_type_size_in_bits,
                             int src_type_size_in_bits,
                             const int (&pg_inputs)[N],
                             const CvtfTestDataSet (&data_set)[N]) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Unpack the data from the array of struct into individual arrays that can
  // simplify the testing.
  uint64_t zn_inputs[N];
  uint64_t expected_zd_scvtf_all_active[N];
  uint64_t expected_zd_ucvtf_all_active[N];
  for (size_t i = 0; i < N; i++) {
    zn_inputs[i] = data_set[i].int_value;
    expected_zd_scvtf_all_active[i] = data_set[i].scvtf_result;
    expected_zd_ucvtf_all_active[i] = data_set[i].ucvtf_result;
  }

  // If the input and result types have a different size, the instruction
  // operates on elements of the largest specified type.
  int lane_size_in_bits =
      std::max(dst_type_size_in_bits, src_type_size_in_bits);

  ZRegister zd_scvtf_all_active = z25;
  ZRegister zd_ucvtf_all_active = z26;
  ZRegister zn = z27;
  InsrHelper(&masm, zn.WithLaneSize(lane_size_in_bits), zn_inputs);

  PRegisterWithLaneSize pg_all_active = p0.WithLaneSize(lane_size_in_bits);
  __ Ptrue(pg_all_active);

  // Test integer conversions with all lanes actived.
  __ Scvtf(zd_scvtf_all_active.WithLaneSize(dst_type_size_in_bits),
           pg_all_active.Merging(),
           zn.WithLaneSize(src_type_size_in_bits));
  __ Ucvtf(zd_ucvtf_all_active.WithLaneSize(dst_type_size_in_bits),
           pg_all_active.Merging(),
           zn.WithLaneSize(src_type_size_in_bits));

  ZRegister zd_scvtf_merged = z23;
  ZRegister zd_ucvtf_merged = z24;

  PRegisterWithLaneSize pg_merged = p1.WithLaneSize(lane_size_in_bits);
  Initialise(&masm, pg_merged, pg_inputs);

  uint64_t snan;
  switch (lane_size_in_bits) {
    case kHRegSize:
      snan = 0x7c11;
      break;
    case kSRegSize:
      snan = 0x7f951111;
      break;
    case kDRegSize:
      snan = 0x7ff5555511111111;
      break;
  }
  __ Dup(zd_scvtf_merged.WithLaneSize(lane_size_in_bits), snan);
  __ Dup(zd_ucvtf_merged.WithLaneSize(lane_size_in_bits), snan);

  // Use the same `zn` inputs to test integer conversions but some lanes are set
  // inactive.
  __ Scvtf(zd_scvtf_merged.WithLaneSize(dst_type_size_in_bits),
           pg_merged.Merging(),
           zn.WithLaneSize(src_type_size_in_bits));
  __ Ucvtf(zd_ucvtf_merged.WithLaneSize(dst_type_size_in_bits),
           pg_merged.Merging(),
           zn.WithLaneSize(src_type_size_in_bits));

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_SVE(expected_zd_scvtf_all_active,
                     zd_scvtf_all_active.WithLaneSize(lane_size_in_bits));
    ASSERT_EQUAL_SVE(expected_zd_ucvtf_all_active,
                     zd_ucvtf_all_active.WithLaneSize(lane_size_in_bits));

    uint64_t expected_zd_scvtf_merged[N];
    for (size_t i = 0; i < N; i++) {
      expected_zd_scvtf_merged[i] =
          pg_inputs[i] ? expected_zd_scvtf_all_active[i] : snan;
    }
    ASSERT_EQUAL_SVE(expected_zd_scvtf_merged,
                     zd_scvtf_merged.WithLaneSize(lane_size_in_bits));

    uint64_t expected_zd_ucvtf_merged[N];
    for (size_t i = 0; i < N; i++) {
      expected_zd_ucvtf_merged[i] =
          pg_inputs[i] ? expected_zd_ucvtf_all_active[i] : snan;
    }
    ASSERT_EQUAL_SVE(expected_zd_ucvtf_merged,
                     zd_ucvtf_merged.WithLaneSize(lane_size_in_bits));
  }
}

TEST_SVE(scvtf_ucvtf_h_s_d_to_float16) {
  // clang-format off
  CvtfTestDataSet data_set_1[] = {
    // Simple conversions of positive numbers which require no rounding; the
    // results should not depened on the rounding mode, and ucvtf and scvtf should
    // produce the same result.
    {0x0000, 0x0000, 0x0000},
    {0x0001, 0x3c00, 0x3c00},
    {0x0010, 0x4c00, 0x4c00},
    {0x0080, 0x5800, 0x5800},
    {0x0400, 0x6400, 0x6400},
    // Conversions which require rounding.
    {0x4000, 0x7400, 0x7400},
    {0x4001, 0x7400, 0x7400},
    // Round up to produce a result that's too big for the input to represent.
    {0x7ff0, 0x77ff, 0x77ff},
    {0x7ff1, 0x77ff, 0x77ff},
    {0x7ffe, 0x7800, 0x7800},
    {0x7fff, 0x7800, 0x7800}};
  int pg_1[] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
  TestUScvtfHelper(config, kHRegSize, kDRegSize, pg_1, data_set_1);
  TestUScvtfHelper(config, kHRegSize, kSRegSize, pg_1, data_set_1);
  TestUScvtfHelper(config, kHRegSize, kHRegSize, pg_1, data_set_1);

  CvtfTestDataSet data_set_2[] = {
    // Test mantissa extremities.
    {0x0401, 0x6401, 0x6401},
    {0x4020, 0x7402, 0x7402},
    // The largest int16_t that fits in a float16.
    {0xffef, 0xcc40, 0x7bff},
    // Values that would be negative if treated as an int16_t.
    {0xff00, 0xdc00, 0x7bf8},
    {0x8000, 0xf800, 0x7800},
    {0x8100, 0xf7f0, 0x7808},
    // Check for bit pattern reproduction.
    {0x0123, 0x5c8c, 0x5c8c},
    {0x0cde, 0x6a6f, 0x6a6f},
    // Simple conversions of negative int64_t values. These require no rounding,
    // and the results should not depend on the rounding mode.
    {0xf800, 0xe800, 0x7bc0},
    {0xfc00, 0xe400, 0x7be0},
    {0xc000, 0xf400, 0x7a00},
    // Check rounding of negative int16_t values.
    {0x8ffe, 0xf700, 0x7880},
    {0x8fff, 0xf700, 0x7880},
    {0xffee, 0xcc80, 0x7bff},
    {0xffef, 0xcc40, 0x7bff}};
  int pg_2[] = {1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1};
  // `32-bit to float16` and `64-bit to float16` of above tests has been tested
  // in `ucvtf` of `16-bit to float16`.
  TestUScvtfHelper(config, kHRegSize, kHRegSize, pg_2, data_set_2);
  // clang-format on
}

TEST_SVE(scvtf_ucvtf_s_to_float) {
  // clang-format off
  int dst_lane_size = kSRegSize;
  int src_lane_size = kSRegSize;

  // Simple conversions of positive numbers which require no rounding; the
  // results should not depened on the rounding mode, and ucvtf and scvtf should
  // produce the same result.
  CvtfTestDataSet data_set_1[] = {
    {0x00000000, 0x00000000, 0x00000000},
    {0x00000001, 0x3f800000, 0x3f800000},
    {0x00004000, 0x46800000, 0x46800000},
    {0x00010000, 0x47800000, 0x47800000},
    {0x40000000, 0x4e800000, 0x4e800000}};
  int pg_1[] = {1, 0, 1, 0, 0};
  TestUScvtfHelper(config, dst_lane_size, src_lane_size, pg_1, data_set_1);

  CvtfTestDataSet data_set_2[] = {
    // Test mantissa extremities.
    {0x00800001, 0x4b000001, 0x4b000001},
    {0x40400000, 0x4e808000, 0x4e808000},
    // The largest int32_t that fits in a double.
    {0x7fffff80, 0x4effffff, 0x4effffff},
    // Values that would be negative if treated as an int32_t.
    {0xffffffff, 0xbf800000, 0x4f800000},
    {0xffffff00, 0xc3800000, 0x4f7fffff},
    {0x80000000, 0xcf000000, 0x4f000000},
    {0x80000001, 0xcf000000, 0x4f000000},
    // Check for bit pattern reproduction.
    {0x089abcde, 0x4d09abce, 0x4d09abce},
    {0x12345678, 0x4d91a2b4, 0x4d91a2b4}};
  int pg_2[] = {1, 0, 1, 0, 1, 1, 1, 0, 0};
  TestUScvtfHelper(config, dst_lane_size, src_lane_size, pg_2, data_set_2);

  // Simple conversions of negative int32_t values. These require no rounding,
  // and the results should not depend on the rounding mode.
  CvtfTestDataSet data_set_3[] = {
    {0xffffc000, 0xc6800000, 0x4f7fffc0},
    {0xffff0000, 0xc7800000, 0x4f7fff00},
    {0xc0000000, 0xce800000, 0x4f400000},
    // Conversions which require rounding.
    {0x72800000, 0x4ee50000, 0x4ee50000},
    {0x72800001, 0x4ee50000, 0x4ee50000},
    {0x73000000, 0x4ee60000, 0x4ee60000},
    // Check rounding of negative int32_t values.
    {0x80000140, 0xcefffffe, 0x4f000001},
    {0x80000141, 0xcefffffd, 0x4f000001},
    {0x80000180, 0xcefffffd, 0x4f000002},
    // Round up to produce a result that's too big for the input to represent.
    {0x7fffffc0, 0x4f000000, 0x4f000000},
    {0x7fffffff, 0x4f000000, 0x4f000000}};
  int pg_3[] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0};
  TestUScvtfHelper(config, dst_lane_size, src_lane_size, pg_3, data_set_3);
  // clang-format on
}

TEST_SVE(scvtf_ucvtf_d_to_float) {
  // clang-format off
  int dst_lane_size = kSRegSize;
  int src_lane_size = kDRegSize;

  // Simple conversions of positive numbers which require no rounding; the
  // results should not depened on the rounding mode, and ucvtf and scvtf should
  // produce the same result.
  CvtfTestDataSet data_set_1[] = {
    {0x0000000000000000, 0x00000000, 0x00000000},
    {0x0000000000000001, 0x3f800000, 0x3f800000},
    {0x0000000040000000, 0x4e800000, 0x4e800000},
    {0x0000000100000000, 0x4f800000, 0x4f800000},
    {0x4000000000000000, 0x5e800000, 0x5e800000}};
  int pg_1[] = {1, 1, 0, 1, 0};
  TestUScvtfHelper(config, dst_lane_size, src_lane_size, pg_1, data_set_1);

  CvtfTestDataSet data_set_2[] = {
    // Test mantissa extremities.
    {0x0010000000000001, 0x59800000, 0x59800000},
    {0x4008000000000000, 0x5e801000, 0x5e801000},
    // The largest int32_t that fits in a float.
    {0x000000007fffff80, 0x4effffff, 0x4effffff},
    // Values that would be negative if treated as an int32_t.
    {0x00000000ffffffff, 0x4f800000, 0x4f800000},
    {0x00000000ffffff00, 0x4f7fffff, 0x4f7fffff},
    {0x0000000080000000, 0x4f000000, 0x4f000000},
    {0x0000000080000100, 0x4f000001, 0x4f000001},
    // The largest int64_t that fits in a float.
    {0x7fffff8000000000, 0x5effffff, 0x5effffff},
    // Check for bit pattern reproduction.
    {0x0123456789abcde0, 0x5b91a2b4, 0x5b91a2b4},
    {0x0000000000876543, 0x4b076543, 0x4b076543}};
  int pg_2[] = {1, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  TestUScvtfHelper(config, dst_lane_size, src_lane_size, pg_2, data_set_2);

  CvtfTestDataSet data_set_3[] = {
    // Simple conversions of negative int64_t values. These require no rounding,
    // and the results should not depend on the rounding mode.
    {0xffffffffc0000000, 0xce800000, 0x5f800000},
    {0xffffffff00000000, 0xcf800000, 0x5f800000},
    {0xc000000000000000, 0xde800000, 0x5f400000},
    // Conversions which require rounding.
    {0x0000800002800000, 0x57000002, 0x57000002},
    {0x0000800002800001, 0x57000003, 0x57000003},
    {0x0000800003000000, 0x57000003, 0x57000003},
    // Check rounding of negative int64_t values.
    {0x8000014000000000, 0xdefffffe, 0x5f000001},
    {0x8000014000000001, 0xdefffffd, 0x5f000001},
    {0x8000018000000000, 0xdefffffd, 0x5f000002},
    // Round up to produce a result that's too big for the input to represent.
    {0x00000000ffffff80, 0x4f800000, 0x4f800000},
    {0x00000000ffffffff, 0x4f800000, 0x4f800000},
    {0xffffff8000000000, 0xd3000000, 0x5f800000},
    {0xffffffffffffffff, 0xbf800000, 0x5f800000}};
  int pg_3[] = {0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1};
  TestUScvtfHelper(config, dst_lane_size, src_lane_size, pg_3, data_set_3);
  // clang-format on
}

TEST_SVE(scvtf_ucvtf_d_to_double) {
  // clang-format off
  int dst_lane_size = kDRegSize;
  int src_lane_size = kDRegSize;

  // Simple conversions of positive numbers which require no rounding; the
  // results should not depened on the rounding mode, and ucvtf and scvtf should
  // produce the same result.
  CvtfTestDataSet data_set_1[] = {
    {0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
    {0x0000000000000001, 0x3ff0000000000000, 0x3ff0000000000000},
    {0x0000000040000000, 0x41d0000000000000, 0x41d0000000000000},
    {0x0000000100000000, 0x41f0000000000000, 0x41f0000000000000},
    {0x4000000000000000, 0x43d0000000000000, 0x43d0000000000000}};
  int pg_1[] = {0, 1, 1, 0, 0};
  TestUScvtfHelper(config, dst_lane_size, src_lane_size, pg_1, data_set_1);

  CvtfTestDataSet data_set_2[] = {
    // Test mantissa extremities.
    {0x0010000000000001, 0x4330000000000001, 0x4330000000000001},
    {0x4008000000000000, 0x43d0020000000000, 0x43d0020000000000},
    // The largest int32_t that fits in a double.
    {0x000000007fffffff, 0x41dfffffffc00000, 0x41dfffffffc00000},
    // Values that would be negative if treated as an int32_t.
    {0x00000000ffffffff, 0x41efffffffe00000, 0x41efffffffe00000},
    {0x0000000080000000, 0x41e0000000000000, 0x41e0000000000000},
    {0x0000000080000001, 0x41e0000000200000, 0x41e0000000200000},
    // The largest int64_t that fits in a double.
    {0x7ffffffffffffc00, 0x43dfffffffffffff, 0x43dfffffffffffff},
    // Check for bit pattern reproduction.
    {0x0123456789abcde0, 0x43723456789abcde, 0x43723456789abcde},
    {0x0000000012345678, 0x41b2345678000000, 0x41b2345678000000}};
  int pg_2[] = {1, 1, 1, 1, 1, 0, 0, 0, 0};
  TestUScvtfHelper(config, dst_lane_size, src_lane_size, pg_2, data_set_2);

  CvtfTestDataSet data_set_3[] = {
    // Simple conversions of negative int64_t values. These require no rounding,
    // and the results should not depend on the rounding mode.
    {0xffffffffc0000000, 0xc1d0000000000000, 0x43effffffff80000},
    {0xffffffff00000000, 0xc1f0000000000000, 0x43efffffffe00000},
    {0xc000000000000000, 0xc3d0000000000000, 0x43e8000000000000},
    // Conversions which require rounding.
    {0x1000000000000280, 0x43b0000000000002, 0x43b0000000000002},
    {0x1000000000000281, 0x43b0000000000003, 0x43b0000000000003},
    {0x1000000000000300, 0x43b0000000000003, 0x43b0000000000003},
    // Check rounding of negative int64_t values.
    {0x8000000000000a00, 0xc3dffffffffffffe, 0x43e0000000000001},
    {0x8000000000000a01, 0xc3dffffffffffffd, 0x43e0000000000001},
    {0x8000000000000c00, 0xc3dffffffffffffd, 0x43e0000000000002},
    // Round up to produce a result that's too big for the input to represent.
    {0x7ffffffffffffe00, 0x43e0000000000000, 0x43e0000000000000},
    {0x7fffffffffffffff, 0x43e0000000000000, 0x43e0000000000000},
    {0xfffffffffffffc00, 0xc090000000000000, 0x43f0000000000000},
    {0xffffffffffffffff, 0xbff0000000000000, 0x43f0000000000000}};
  int pg_3[] = {1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0};
  TestUScvtfHelper(config, dst_lane_size, src_lane_size, pg_3, data_set_3);
  // clang-format on
}

TEST_SVE(scvtf_ucvtf_s_to_double) {
  // clang-format off
  int dst_lane_size = kDRegSize;
  int src_lane_size = kSRegSize;

  // Simple conversions of positive numbers which require no rounding; the
  // results should not depened on the rounding mode, and ucvtf and scvtf should
  // produce the same result.
  CvtfTestDataSet data_set_1[] = {
    {0x00000000, 0x0000000000000000, 0x0000000000000000},
    {0x00000001, 0x3ff0000000000000, 0x3ff0000000000000},
    {0x00004000, 0x40d0000000000000, 0x40d0000000000000},
    {0x00010000, 0x40f0000000000000, 0x40f0000000000000},
    {0x40000000, 0x41d0000000000000, 0x41d0000000000000}};
  int pg_1[] = {1, 0, 0, 0, 1};
  TestUScvtfHelper(config, dst_lane_size, src_lane_size, pg_1, data_set_1);

  CvtfTestDataSet data_set_2[] = {
    // Test mantissa extremities.
    {0x40000400, 0x41d0000100000000, 0x41d0000100000000},
    // The largest int32_t that fits in a double.
    {0x7fffffff, 0x41dfffffffc00000, 0x41dfffffffc00000},
    // Values that would be negative if treated as an int32_t.
    {0xffffffff, 0xbff0000000000000, 0x41efffffffe00000},
    {0x80000000, 0xc1e0000000000000, 0x41e0000000000000},
    {0x80000001, 0xc1dfffffffc00000, 0x41e0000000200000},
    // Check for bit pattern reproduction.
    {0x089abcde, 0x41a13579bc000000, 0x41a13579bc000000},
    {0x12345678, 0x41b2345678000000, 0x41b2345678000000},
    // Simple conversions of negative int32_t values. These require no rounding,
    // and the results should not depend on the rounding mode.
    {0xffffc000, 0xc0d0000000000000, 0x41effff800000000},
    {0xffff0000, 0xc0f0000000000000, 0x41efffe000000000},
    {0xc0000000, 0xc1d0000000000000, 0x41e8000000000000}};
  int pg_2[] = {1, 0, 1, 0, 0, 1, 1, 0, 1, 1};
  TestUScvtfHelper(config, dst_lane_size, src_lane_size, pg_2, data_set_2);

  // Note that IEEE 754 double-precision format has 52-bits fraction, so all
  // 32-bits integers are representable in double.
  // clang-format on
}

TEST_SVE(sve_fadda) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kFP,
                          CPUFeatures::kFPHalf);
  START();

  __ Ptrue(p0.VnB());
  __ Pfalse(p1.VnB());
  __ Zip1(p1.VnH(), p0.VnH(), p1.VnH());

  __ Index(z0.VnS(), 3, 3);
  __ Scvtf(z0.VnS(), p0.Merging(), z0.VnS());
  __ Fmov(s2, 2.0);
  __ Fadda(s2, p0, s2, z0.VnS());

  __ Index(z0.VnD(), -7, -7);
  __ Scvtf(z0.VnD(), p0.Merging(), z0.VnD());
  __ Fmov(d3, 3.0);
  __ Fadda(d3, p0, d3, z0.VnD());

  __ Index(z0.VnH(), 1, 1);
  __ Scvtf(z0.VnH(), p0.Merging(), z0.VnH());
  __ Fmov(h4, 0);
  __ Fadda(h4, p1, h4, z0.VnH());
  END();

  if (CAN_RUN()) {
    RUN();
    // Sum of 1 .. n is n+1 * n/2, ie. n(n+1)/2.
    int n = core.GetSVELaneCount(kSRegSize);
    ASSERT_EQUAL_FP32(2 + 3 * ((n + 1) * (n / 2)), s2);

    n /= 2;  // Half as many lanes.
    ASSERT_EQUAL_FP64(3 + -7 * ((n + 1) * (n / 2)), d3);

    // Sum of first n odd numbers is n^2.
    n = core.GetSVELaneCount(kHRegSize) / 2;  // Half are odd numbers.
    ASSERT_EQUAL_FP16(Float16(n * n), h4);
  }
}

}  // namespace aarch64
}  // namespace vixl
