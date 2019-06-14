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

// Conveniently initialise P registers. This is optimised for call-site clarity,
// not generated code quality.
//
// Usage:
//
//     int values[] = { 0x0, 0x1, 0x2 };
//     Initialise(&masm, p0.VnS(), values);  // Sets p0 = 0b'0000'0001'0010
//
// The rightmost (highest-indexed) array element maps to the lowest-numbered
// lane.
//
// Each element of the `values` array is mapped onto a lane in `pd`. The
// architecture only respects the lower bit, and writes zero the upper bits, but
// other (encodable) values can be specified if required by the test.
template <typename T, size_t N>
void Initialise(MacroAssembler* masm,
                const PRegisterWithLaneSize& pd,
                const T (&values)[N]) {
  UseScratchRegisterScope temps(masm);
  Register temp = temps.AcquireX();
  Label data;
  Label done;

  // There is no 'insr' for P registers. The easiest way to initialise one with
  // an arbitrary value is to load it from a literal pool.

  int p_bits_per_lane = pd.GetLaneSizeInBits() / kZRegBitsPerPRegBit;
  VIXL_ASSERT((N * p_bits_per_lane) <= kPRegMaxSize);
  uint64_t p_lane_mask = GetUintMask(p_bits_per_lane);

  // For most lane sizes, each value contributes less than a byte. We need to
  // pack them into chunks which we can store directly. It's sensible for the
  // chunk to be the same size as an instruction because we need to pad to an
  // instruction boundary anyway.
  typedef Instr Chunk;
  const size_t kChunkSizeInBits = sizeof(Chunk) * kBitsPerByte;
  VIXL_ASSERT((kPRegMaxSize % kChunkSizeInBits) == 0);
  const size_t kPRegMaxSizeInChunks = kPRegMaxSize / kChunkSizeInBits;

  masm->Adr(temp, &data);
  // TODO: Use `Ldr(pd, MemOperand(temp))` once available.
  masm->Ldr(PRegister(pd.GetCode()), temp);
  masm->B(&done);
  {
    ExactAssemblyScope total(masm, kPRegMaxSizeInBytes);
    masm->bind(&data);
    // Put the last-specified value at the lowest address.
    int values_index = N - 1;
    for (size_t c = 0; c < kPRegMaxSizeInChunks; c++) {
      Chunk chunk = 0;
      // Whilst we still have values left, use them to populate the chunk.
      for (size_t chunk_bit = 0;
           (chunk_bit < kChunkSizeInBits) && (values_index >= 0);
           chunk_bit += p_bits_per_lane) {
        Chunk value = values[values_index] & p_lane_mask;
        VIXL_ASSERT(static_cast<T>(value) == values[values_index]);
        chunk |= value << chunk_bit;
        values_index--;
      }
      masm->dc(chunk);
    }
  }
  masm->Bind(&done);
}

// Ensure that basic test infrastructure works.
TEST(sve_test_infrastructure_z) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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
TEST(sve_test_infrastructure_p) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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
TEST(sve_v_write_clear) {
  SETUP_WITH_FEATURES(CPUFeatures::kNEON, CPUFeatures::kFP, CPUFeatures::kSVE);
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

  // Use Index to label the lane indeces, so failures are easy to detect and
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

static void MlaMlsHelper(unsigned lane_size_in_bits) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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

TEST(sve_mla_mls_b) { MlaMlsHelper(kBRegSize); }
TEST(sve_mla_mls_h) { MlaMlsHelper(kHRegSize); }
TEST(sve_mla_mls_s) { MlaMlsHelper(kSRegSize); }
TEST(sve_mla_mls_d) { MlaMlsHelper(kDRegSize); }

TEST(sve_bitwise_unpredicate_logical) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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

TEST(sve_predicate_logical) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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

TEST(sve_int_compare_vectors) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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

TEST(sve_int_compare_vectors_wide_elements) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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

TEST(sve_bitwise_imm) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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

TEST(sve_dup_imm) {
  // The `Dup` macro can generate `dup`, `dupm`, and it can synthesise
  // unencodable immediates.

  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // Encodable with `dup` (shift 0).
  __ Dup(z0.VnD(), -1);
  __ Dup(z1.VnS(), 0x7f);
  __ Dup(z2.VnH(), -0x80);
  __ Dup(z3.VnB(), 42);

  // Encodable with `dup` (shift 8).
  // TODO: Enable these once we have Simulator support.
  // __ Dup(z4.VnD(), -42 * 256);
  // __ Dup(z5.VnS(), -0x8000);
  // __ Dup(z6.VnH(), 0x7f00);
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

    // TODO: Enable these once we have Simulator support.
    // ASSERT_EQUAL_SVE(0x0000000000003c00, z4.VnD());
    // ASSERT_EQUAL_SVE(0xffff8000, z5.VnS());
    // ASSERT_EQUAL_SVE(0x7f00, z6.VnH());

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

TEST(sve_inc_dec_p_scalar) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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

  // TODO: Use Ptrue() for this, once it is implemented.
  int p15_inputs[kPRegMaxSize / kZRegBitsPerPRegBit];
  for (size_t i = 0; i < sizeof(p15_inputs) / sizeof(p15_inputs[0]); i++) {
    p15_inputs[i] = 1;
  }
  Initialise(&masm, p15.VnB(), p15_inputs);

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

TEST(sve_sqinc_sqdec_p_scalar) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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
  __ Sqdecp(x10, p0.VnB(), x10);

  __ Mov(x11, dummy_high + 0x80000001);  // INT32_MIN + 1
  __ Sqdecp(x11, p0.VnH(), w11);

  __ Mov(x12, 1);
  __ Sqdecp(x12, p0.VnS(), x12);

  __ Mov(x13, dummy_high + 1);
  __ Sqdecp(x13, p0.VnD(), w13);

  __ Mov(x14, 0x7ffffffffffffffe);  // INT64_MAX - 1
  __ Sqincp(x14, p0.VnB(), x14);

  __ Mov(x15, dummy_high + 0x7ffffffe);  // INT32_MAX - 1
  __ Sqincp(x15, p0.VnH(), w15);

  // Don't use x16 and x17 since they are scratch registers by default.

  __ Mov(x18, 0xffffffffffffffff);
  __ Sqincp(x18, p0.VnS(), x18);

  __ Mov(x19, dummy_high + 0xffffffff);
  __ Sqincp(x19, p0.VnD(), w19);

  __ Mov(x20, dummy_high + 0xffffffff);
  __ Sqdecp(x20, p0.VnB(), w20);

  // With an all-true predicate, these instructions increment or decrement by
  // the vector length.

  // TODO: Use Ptrue() for this, once it is implemented.
  int p15_inputs[kPRegMaxSize / kZRegBitsPerPRegBit];
  for (size_t i = 0; i < sizeof(p15_inputs) / sizeof(p15_inputs[0]); i++) {
    p15_inputs[i] = 1;
  }
  Initialise(&masm, p15.VnB(), p15_inputs);

  __ Mov(x21, 0);
  __ Sqdecp(x21, p15.VnB(), x21);

  __ Mov(x22, 0);
  __ Sqincp(x22, p15.VnH(), x22);

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

TEST(sve_uqinc_uqdec_p_scalar) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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

  // TODO: Use Ptrue() for this, once it is implemented.
  int p15_inputs[kPRegMaxSize / kZRegBitsPerPRegBit];
  for (size_t i = 0; i < sizeof(p15_inputs) / sizeof(p15_inputs[0]); i++) {
    p15_inputs[i] = 1;
  }
  Initialise(&masm, p15.VnB(), p15_inputs);

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

TEST(sve_inc_dec_p_vector) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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

TEST(sve_inc_dec_ptrue_vector) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // With an all-true predicate, these instructions increment or decrement by
  // the vector length.

  // TODO: Use Ptrue() for this, once it is implemented.
  int p15_inputs[kPRegMaxSize / kZRegBitsPerPRegBit];
  for (size_t i = 0; i < sizeof(p15_inputs) / sizeof(p15_inputs[0]); i++) {
    p15_inputs[i] = 1;
  }
  Initialise(&masm, p15.VnB(), p15_inputs);

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

TEST(sve_sqinc_sqdec_p_vector) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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

TEST(sve_sqinc_sqdec_ptrue_vector) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // With an all-true predicate, these instructions increment or decrement by
  // the vector length.

  // TODO: Use Ptrue() for this, once it is implemented.
  int p15_inputs[kPRegMaxSize / kZRegBitsPerPRegBit];
  for (size_t i = 0; i < sizeof(p15_inputs) / sizeof(p15_inputs[0]); i++) {
    p15_inputs[i] = 1;
  }
  Initialise(&masm, p15.VnB(), p15_inputs);

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

TEST(sve_uqinc_uqdec_p_vector) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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

TEST(sve_uqinc_uqdec_ptrue_vector) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
  START();

  // With an all-true predicate, these instructions increment or decrement by
  // the vector length.

  // TODO: Use Ptrue() for this, once it is implemented.
  int p15_inputs[kPRegMaxSize / kZRegBitsPerPRegBit];
  for (size_t i = 0; i < sizeof(p15_inputs) / sizeof(p15_inputs[0]); i++) {
    p15_inputs[i] = 1;
  }
  Initialise(&masm, p15.VnB(), p15_inputs);

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

TEST(sve_index) {
  SETUP_WITH_FEATURES(CPUFeatures::kSVE);
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

  __ Ctermeq(w0, w0);
  __ Mrs(x2, NZCV);
  __ Ctermeq(x0, x1);
  __ Mrs(x3, NZCV);
  __ Ctermne(x0, x0);
  __ Mrs(x4, NZCV);
  __ Ctermne(w0, w1);
  __ Mrs(x5, NZCV);

  END();

  if (CAN_RUN()) {
    RUN();

    ASSERT_EQUAL_32(SVEFirstFlag, w2);
    ASSERT_EQUAL_32(VFlag, w3);
    ASSERT_EQUAL_32(VFlag, w4);
    ASSERT_EQUAL_32(SVEFirstFlag, w5);
  }
}

}  // namespace aarch64
}  // namespace vixl
