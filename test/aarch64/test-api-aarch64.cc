// Copyright 2017, VIXL authors
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

#include <cstdio>
#include <cstring>
#include <string>

#include "test-runner.h"
#include "test-utils.h"
#include "aarch64/test-utils-aarch64.h"

#include "aarch64/macro-assembler-aarch64.h"

#define __ masm.
#define TEST(name) TEST_(AARCH64_API_##name)


namespace vixl {
namespace aarch64 {

// Check compiler intrinsics helpers.

TEST(count_leading_sign_bits) {
  class Helper {
   public:
    static void Check(int64_t value, int non_sign_bits) {
      VIXL_ASSERT((0 <= non_sign_bits) && (non_sign_bits < 64));

      for (int width = 1; width <= 64; width *= 2) {
        // Note that leading_sign_bits does not include the topmost bit.
        int leading_sign_bits = width - non_sign_bits - 1;
        if (leading_sign_bits < 0) continue;

        int64_t result = CountLeadingSignBits(value, width);
        int64_t fallback_result = CountLeadingSignBitsFallBack(value, width);
        VIXL_CHECK(result == leading_sign_bits);
        VIXL_CHECK(fallback_result == leading_sign_bits);
      }
    }
  };

  // Basic positive (and zero) cases. Sign bits are all zeroes.
  Helper::Check(0, 0);  // 0b++++
  Helper::Check(1, 1);  // 0b+++1
  Helper::Check(2, 2);  // 0b++10
  Helper::Check(3, 2);  // 0b++11
  Helper::Check(4, 3);  // 0b+100

  // Basic negative cases. Sign bits are all ones.
  Helper::Check(-1, 0);  // 0b----
  Helper::Check(-2, 1);  // 0b---0
  Helper::Check(-3, 2);  // 0b--01
  Helper::Check(-4, 2);  // 0b--00
  Helper::Check(-5, 3);  // 0b-011

  // Boundary conditions.
  Helper::Check(INT8_MAX, 7);
  Helper::Check(INT8_MIN, 7);
  Helper::Check(static_cast<int64_t>(INT8_MAX) + 1, 8);
  Helper::Check(static_cast<int64_t>(INT8_MIN) - 1, 8);

  Helper::Check(INT16_MAX, 15);
  Helper::Check(INT16_MIN, 15);
  Helper::Check(static_cast<int64_t>(INT16_MAX) + 1, 16);
  Helper::Check(static_cast<int64_t>(INT16_MIN) - 1, 16);

  Helper::Check(INT32_MAX, 31);
  Helper::Check(INT32_MIN, 31);
  Helper::Check(static_cast<int64_t>(INT32_MAX) + 1, 32);
  Helper::Check(static_cast<int64_t>(INT32_MIN) - 1, 32);

  Helper::Check(INT64_MAX, 63);
  Helper::Check(INT64_MIN, 63);

  // Check automatic width detection.
  VIXL_CHECK(CountLeadingSignBits(static_cast<int8_t>(42)) == 1);  // 0b00101010
  VIXL_CHECK(CountLeadingSignBits(static_cast<int16_t>(42)) == 9);
  VIXL_CHECK(CountLeadingSignBits(static_cast<int32_t>(42)) == 25);
  VIXL_CHECK(CountLeadingSignBits(static_cast<int64_t>(42)) == 57);
}

// Check SimFloat16 class mechanics.
TEST(float16_operators) {
  ::vixl::internal::SimFloat16 f1 = kFP16DefaultNaN;
  ::vixl::internal::SimFloat16 f2 = kFP16DefaultNaN;
  ::vixl::internal::SimFloat16 f3 = kFP16PositiveInfinity;
  ::vixl::internal::SimFloat16 f4 = kFP16NegativeInfinity;
  VIXL_CHECK(!(f1 == f2));
  VIXL_CHECK(f1 != f2);
  VIXL_CHECK(!(f3 == f4));
  VIXL_CHECK(f3 != f4);
  VIXL_CHECK(::vixl::internal::SimFloat16(kFP16PositiveZero) ==
             ::vixl::internal::SimFloat16(kFP16NegativeZero));
  VIXL_CHECK(!(::vixl::internal::SimFloat16(kFP16PositiveZero) !=
               ::vixl::internal::SimFloat16(kFP16NegativeZero)));
}

TEST(rawbits_conversions) {
  VIXL_CHECK(RawbitsToInt64(0x0) == 0x0);
  VIXL_CHECK(RawbitsToInt64(0x123) == 0x123);
  VIXL_CHECK(RawbitsToInt64(INT64_MAX) == INT64_MAX);
  VIXL_CHECK(RawbitsToInt64(UINT64_C(0xffffffffffffffff)) == -1);
  VIXL_CHECK(RawbitsToInt64(UINT64_C(0x8000000000000000)) == INT64_MIN);
  VIXL_CHECK(RawbitsToInt64(UINT64_C(0x8000000000000001)) == -INT64_MAX);

  VIXL_CHECK(RawbitsToInt32(0x0) == 0x0);
  VIXL_CHECK(RawbitsToInt32(0x123) == 0x123);
  VIXL_CHECK(RawbitsToInt32(INT32_MAX) == INT32_MAX);
  VIXL_CHECK(RawbitsToInt32(UINT32_C(0xffffffff)) == -1);
  VIXL_CHECK(RawbitsToInt32(UINT32_C(0x80000000)) == INT32_MIN);
  VIXL_CHECK(RawbitsToInt32(UINT32_C(0x80000001)) == -INT32_MAX);
}

// Check moved FP constants are still accessible via the AArch64 namespace.
TEST(float_constants_scope) {
  VIXL_CHECK(vixl::aarch64::kFP64PositiveInfinity ==
             vixl::kFP64PositiveInfinity);
  VIXL_CHECK(vixl::aarch64::kFP64NegativeInfinity ==
             vixl::kFP64NegativeInfinity);
  VIXL_CHECK(vixl::aarch64::kFP32PositiveInfinity ==
             vixl::kFP32PositiveInfinity);
  VIXL_CHECK(vixl::aarch64::kFP32NegativeInfinity ==
             vixl::kFP32NegativeInfinity);
  VIXL_CHECK(Float16ToRawbits(vixl::aarch64::kFP16PositiveInfinity) ==
             Float16ToRawbits(vixl::aarch64::kFP16PositiveInfinity));
  VIXL_CHECK(Float16ToRawbits(vixl::aarch64::kFP16NegativeInfinity) ==
             Float16ToRawbits(vixl::aarch64::kFP16NegativeInfinity));
  VIXL_CHECK(DoubleToRawbits(vixl::aarch64::kFP64DefaultNaN) ==
             DoubleToRawbits(vixl::kFP64DefaultNaN));
  VIXL_CHECK(FloatToRawbits(vixl::aarch64::kFP32DefaultNaN) ==
             FloatToRawbits(vixl::kFP32DefaultNaN));
  VIXL_CHECK(IsNaN(vixl::aarch64::kFP16DefaultNaN) ==
             IsNaN(vixl::kFP16DefaultNaN));
  VIXL_CHECK(vixl::aarch64::kDoubleExponentBits == vixl::kDoubleExponentBits);
  VIXL_CHECK(vixl::aarch64::kDoubleMantissaBits == vixl::kDoubleMantissaBits);
  VIXL_CHECK(vixl::aarch64::kFloatExponentBits == vixl::kFloatExponentBits);
  VIXL_CHECK(vixl::aarch64::kFloatMantissaBits == vixl::kFloatMantissaBits);
  VIXL_CHECK(vixl::aarch64::kFloat16ExponentBits == vixl::kFloat16ExponentBits);
  VIXL_CHECK(vixl::aarch64::kFloat16MantissaBits == vixl::kFloat16MantissaBits);
}


TEST(register_bit) {
  VIXL_CHECK(x0.GetBit() == (UINT64_C(1) << 0));
  VIXL_CHECK(x1.GetBit() == (UINT64_C(1) << 1));
  VIXL_CHECK(x10.GetBit() == (UINT64_C(1) << 10));

  // AAPCS64 definitions.
  VIXL_CHECK(lr.GetBit() == (UINT64_C(1) << kLinkRegCode));

  // Fixed (hardware) definitions.
  VIXL_CHECK(xzr.GetBit() == (UINT64_C(1) << kZeroRegCode));

  // Internal ABI definitions.
  VIXL_CHECK(sp.GetBit() == (UINT64_C(1) << kSPRegInternalCode));
  VIXL_CHECK(sp.GetBit() != xzr.GetBit());

  // xn.GetBit() == wn.GetBit() at all times, for the same n.
  VIXL_CHECK(x0.GetBit() == w0.GetBit());
  VIXL_CHECK(x1.GetBit() == w1.GetBit());
  VIXL_CHECK(x10.GetBit() == w10.GetBit());
  VIXL_CHECK(xzr.GetBit() == wzr.GetBit());
  VIXL_CHECK(sp.GetBit() == wsp.GetBit());
}


TEST(noreg) {
  VIXL_CHECK(NoReg.Is(NoVReg));
  VIXL_CHECK(NoVReg.Is(NoReg));

  VIXL_CHECK(NoVReg.Is(NoReg));
  VIXL_CHECK(NoReg.Is(NoVReg));

  VIXL_CHECK(NoReg.Is(NoCPUReg));
  VIXL_CHECK(NoCPUReg.Is(NoReg));

  VIXL_CHECK(NoVReg.Is(NoCPUReg));
  VIXL_CHECK(NoCPUReg.Is(NoVReg));

  VIXL_CHECK(NoVReg.Is(NoCPUReg));
  VIXL_CHECK(NoCPUReg.Is(NoVReg));

  VIXL_CHECK(NoReg.IsNone());
  VIXL_CHECK(NoVReg.IsNone());
  VIXL_CHECK(NoCPUReg.IsNone());
}


TEST(isvalid) {
  VIXL_CHECK(!NoReg.IsValid());
  VIXL_CHECK(!NoVReg.IsValid());
  VIXL_CHECK(!NoCPUReg.IsValid());

  VIXL_CHECK(x0.IsValid());
  VIXL_CHECK(w0.IsValid());
  VIXL_CHECK(x30.IsValid());
  VIXL_CHECK(w30.IsValid());
  VIXL_CHECK(xzr.IsValid());
  VIXL_CHECK(wzr.IsValid());

  VIXL_CHECK(sp.IsValid());
  VIXL_CHECK(wsp.IsValid());

  VIXL_CHECK(d0.IsValid());
  VIXL_CHECK(s0.IsValid());
  VIXL_CHECK(d31.IsValid());
  VIXL_CHECK(s31.IsValid());

  VIXL_CHECK(x0.IsValidRegister());
  VIXL_CHECK(w0.IsValidRegister());
  VIXL_CHECK(xzr.IsValidRegister());
  VIXL_CHECK(wzr.IsValidRegister());
  VIXL_CHECK(sp.IsValidRegister());
  VIXL_CHECK(wsp.IsValidRegister());
  VIXL_CHECK(!x0.IsValidVRegister());
  VIXL_CHECK(!w0.IsValidVRegister());
  VIXL_CHECK(!xzr.IsValidVRegister());
  VIXL_CHECK(!wzr.IsValidVRegister());
  VIXL_CHECK(!sp.IsValidVRegister());
  VIXL_CHECK(!wsp.IsValidVRegister());
  VIXL_CHECK(!x0.IsValidFPRegister());
  VIXL_CHECK(!w0.IsValidFPRegister());
  VIXL_CHECK(!xzr.IsValidFPRegister());
  VIXL_CHECK(!wzr.IsValidFPRegister());
  VIXL_CHECK(!sp.IsValidFPRegister());
  VIXL_CHECK(!wsp.IsValidFPRegister());

  VIXL_CHECK(q0.IsValidVRegister());
  VIXL_CHECK(!q0.IsValidFPRegister());
  VIXL_CHECK(!q0.IsValidRegister());

  VIXL_CHECK(d0.IsValidVRegister());
  VIXL_CHECK(d0.IsValidFPRegister());
  VIXL_CHECK(!d0.IsValidRegister());

  VIXL_CHECK(s0.IsValidVRegister());
  VIXL_CHECK(s0.IsValidFPRegister());
  VIXL_CHECK(!s0.IsValidRegister());

  VIXL_CHECK(h0.IsValidVRegister());
  VIXL_CHECK(h0.IsValidFPRegister());
  VIXL_CHECK(!h0.IsValidRegister());

  VIXL_CHECK(b0.IsValidVRegister());
  VIXL_CHECK(!b0.IsValidFPRegister());
  VIXL_CHECK(!b0.IsValidRegister());

  // IsValidFPRegister() is only true for scalar types.
  VIXL_CHECK(q0.V2D().IsValidVRegister());
  VIXL_CHECK(!q0.V2D().IsValidFPRegister());
  VIXL_CHECK(d0.V2S().IsValidVRegister());
  VIXL_CHECK(!d0.V2S().IsValidFPRegister());
  VIXL_CHECK(s0.V2H().IsValidVRegister());
  VIXL_CHECK(!s0.V2H().IsValidFPRegister());
}


TEST(isvalid_cpu) {
  // As 'isvalid', but using CPURegister types where possible. This shouldn't
  // make any difference.
  VIXL_CHECK(!static_cast<CPURegister>(NoReg).IsValid());
  VIXL_CHECK(!static_cast<CPURegister>(NoVReg).IsValid());
  VIXL_CHECK(!static_cast<CPURegister>(NoCPUReg).IsValid());

  VIXL_CHECK(static_cast<CPURegister>(x0).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(w0).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(x30).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(w30).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(xzr).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(wzr).IsValid());

  VIXL_CHECK(static_cast<CPURegister>(sp).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(wsp).IsValid());

  VIXL_CHECK(static_cast<CPURegister>(d0).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(s0).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(d31).IsValid());
  VIXL_CHECK(static_cast<CPURegister>(s31).IsValid());

  VIXL_CHECK(static_cast<CPURegister>(x0).IsValidRegister());
  VIXL_CHECK(static_cast<CPURegister>(w0).IsValidRegister());
  VIXL_CHECK(static_cast<CPURegister>(xzr).IsValidRegister());
  VIXL_CHECK(static_cast<CPURegister>(wzr).IsValidRegister());
  VIXL_CHECK(static_cast<CPURegister>(sp).IsValidRegister());
  VIXL_CHECK(static_cast<CPURegister>(wsp).IsValidRegister());
  VIXL_CHECK(!static_cast<CPURegister>(x0).IsValidVRegister());
  VIXL_CHECK(!static_cast<CPURegister>(w0).IsValidVRegister());
  VIXL_CHECK(!static_cast<CPURegister>(xzr).IsValidVRegister());
  VIXL_CHECK(!static_cast<CPURegister>(wzr).IsValidVRegister());
  VIXL_CHECK(!static_cast<CPURegister>(sp).IsValidVRegister());
  VIXL_CHECK(!static_cast<CPURegister>(wsp).IsValidVRegister());
  VIXL_CHECK(!static_cast<CPURegister>(x0).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(w0).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(xzr).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(wzr).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(sp).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(wsp).IsValidFPRegister());

  VIXL_CHECK(static_cast<CPURegister>(q0).IsValidVRegister());
  VIXL_CHECK(!static_cast<CPURegister>(q0).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(q0).IsValidRegister());

  VIXL_CHECK(static_cast<CPURegister>(d0).IsValidVRegister());
  VIXL_CHECK(static_cast<CPURegister>(d0).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(d0).IsValidRegister());

  VIXL_CHECK(static_cast<CPURegister>(s0).IsValidVRegister());
  VIXL_CHECK(static_cast<CPURegister>(s0).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(s0).IsValidRegister());

  VIXL_CHECK(static_cast<CPURegister>(h0).IsValidVRegister());
  VIXL_CHECK(static_cast<CPURegister>(h0).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(h0).IsValidRegister());

  VIXL_CHECK(static_cast<CPURegister>(b0).IsValidVRegister());
  VIXL_CHECK(!static_cast<CPURegister>(b0).IsValidFPRegister());
  VIXL_CHECK(!static_cast<CPURegister>(b0).IsValidRegister());
}


TEST(areconsecutive) {
  VIXL_CHECK(AreConsecutive(b0, NoVReg));
  VIXL_CHECK(AreConsecutive(b1, b2));
  VIXL_CHECK(AreConsecutive(b3, b4, b5));
  VIXL_CHECK(AreConsecutive(b6, b7, b8, b9));
  VIXL_CHECK(AreConsecutive(h10, NoVReg));
  VIXL_CHECK(AreConsecutive(h11, h12));
  VIXL_CHECK(AreConsecutive(h13, h14, h15));
  VIXL_CHECK(AreConsecutive(h16, h17, h18, h19));
  VIXL_CHECK(AreConsecutive(s20, NoVReg));
  VIXL_CHECK(AreConsecutive(s21, s22));
  VIXL_CHECK(AreConsecutive(s23, s24, s25));
  VIXL_CHECK(AreConsecutive(s26, s27, s28, s29));
  VIXL_CHECK(AreConsecutive(d30, NoVReg));
  VIXL_CHECK(AreConsecutive(d31, d0));
  VIXL_CHECK(AreConsecutive(d1, d2, d3));
  VIXL_CHECK(AreConsecutive(d4, d5, d6, d7));
  VIXL_CHECK(AreConsecutive(q8, NoVReg));
  VIXL_CHECK(AreConsecutive(q9, q10));
  VIXL_CHECK(AreConsecutive(q11, q12, q13));
  VIXL_CHECK(AreConsecutive(q14, q15, q16, q17));
  VIXL_CHECK(AreConsecutive(v18, NoVReg));
  VIXL_CHECK(AreConsecutive(v19, v20));
  VIXL_CHECK(AreConsecutive(v21, v22, v23));
  VIXL_CHECK(AreConsecutive(v24, v25, v26, v27));
  VIXL_CHECK(AreConsecutive(b29, h30));
  VIXL_CHECK(AreConsecutive(s31, d0, q1));
  VIXL_CHECK(AreConsecutive(v2, b3, h4, s5));

  VIXL_CHECK(!AreConsecutive(b0, b2));
  VIXL_CHECK(!AreConsecutive(h1, h0));
  VIXL_CHECK(!AreConsecutive(s31, s1));
  VIXL_CHECK(!AreConsecutive(d12, d12));
  VIXL_CHECK(!AreConsecutive(q31, q1));

  VIXL_CHECK(!AreConsecutive(b0, b1, b3));
  VIXL_CHECK(!AreConsecutive(h4, h5, h6, h6));
  VIXL_CHECK(!AreConsecutive(d11, d13, NoVReg, d14));
  VIXL_CHECK(!AreConsecutive(d15, d16, d18, NoVReg));
  VIXL_CHECK(!AreConsecutive(b26, b28, NoVReg, b29));
  VIXL_CHECK(!AreConsecutive(s28, s30, NoVReg, NoVReg));

  VIXL_CHECK(AreConsecutive(q19, NoVReg, NoVReg, q22));
  VIXL_CHECK(AreConsecutive(v23, NoVReg, v25, NoVReg));
  VIXL_CHECK(AreConsecutive(b26, b27, NoVReg, NoVReg));
  VIXL_CHECK(AreConsecutive(h28, NoVReg, NoVReg, NoVReg));
  VIXL_CHECK(AreConsecutive(s30, s31, NoVReg, s2));
  VIXL_CHECK(AreConsecutive(d3, NoVReg, d6, d7));
}


TEST(sve_p_registers) {
  enum Qualification { kNone, kZeroing, kMerging, kWithLaneSize };
  class Helper {
   public:
    static Qualification GetQualification(PRegister) { return kNone; }
    static Qualification GetQualification(PRegisterZ) { return kZeroing; }
    static Qualification GetQualification(PRegisterM) { return kMerging; }
    static Qualification GetQualification(PRegisterWithLaneSize) {
      return kWithLaneSize;
    }
  };

  VIXL_CHECK(kNumberOfPRegisters == 16);
  VIXL_CHECK(p0.GetCode() == 0);
  VIXL_CHECK(p15.GetCode() == 15);
  VIXL_CHECK(p14.VnB().GetLaneSizeInBits() == kBRegSize);
  VIXL_CHECK(p14.VnH().GetLaneSizeInBits() == kHRegSize);
  VIXL_CHECK(p14.VnS().GetLaneSizeInBits() == kSRegSize);
  VIXL_CHECK(p14.VnD().GetLaneSizeInBits() == kDRegSize);
  VIXL_CHECK(p14.VnB().GetLaneSizeInBytes() == kBRegSizeInBytes);
  VIXL_CHECK(p14.VnH().GetLaneSizeInBytes() == kHRegSizeInBytes);
  VIXL_CHECK(p14.VnS().GetLaneSizeInBytes() == kSRegSizeInBytes);
  VIXL_CHECK(p14.VnD().GetLaneSizeInBytes() == kDRegSizeInBytes);
  VIXL_CHECK(Helper::GetQualification(p1) == kNone);
  VIXL_CHECK(Helper::GetQualification(p2.Zeroing()) == kZeroing);
  VIXL_CHECK(Helper::GetQualification(p3.Merging()) == kMerging);
  VIXL_CHECK(Helper::GetQualification(p4.VnB()) == kWithLaneSize);
  VIXL_CHECK(Helper::GetQualification(p5.VnH()) == kWithLaneSize);
  VIXL_CHECK(Helper::GetQualification(p6.VnS()) == kWithLaneSize);
  VIXL_CHECK(Helper::GetQualification(p7.VnD()) == kWithLaneSize);
}


TEST(sve_z_registers) {
  VIXL_CHECK(z0.GetCode() == 0);
  VIXL_CHECK(z31.GetCode() == 31);

  VIXL_CHECK(z0.Is(z0));
  VIXL_CHECK(!z0.Is(z1));
  VIXL_CHECK(!z0.Is(v0));
  VIXL_CHECK(!z0.Is(b0));
  VIXL_CHECK(!z0.Is(q0));

  VIXL_CHECK(AreAliased(z5, z5));
  VIXL_CHECK(AreAliased(z5, b5));
  VIXL_CHECK(AreAliased(b5, z5));
  VIXL_CHECK(AreAliased(z5, z5.B()));
  VIXL_CHECK(AreAliased(z5, z5.VnB()));

  VIXL_CHECK(!AreAliased(z6, z7));
  VIXL_CHECK(!AreAliased(b6, z7));
  VIXL_CHECK(!AreAliased(x7, z7));
}


TEST(sve_z_registers_vs_neon) {
  // There are three related register variants to consider in VIXL's API:
  //
  //    "b0": NEON: The least-significant byte of v0.
  //    "v0.B": NEON: v0, with an unspecified number of byte-sized lanes.
  //    "z0.B": SVE: z0, with an unspecified number of byte-sized lanes.
  //
  // The first two cases are indistinguishable in VIXL; both are obtained using
  // something like `v0.B()`. This is fine for NEON because there is no
  // ambiguity in practice; the "v0.B" form is always used with an index that
  // makes the meaning clear.

  VIXL_ASSERT(v6.B().Is(b6));
  VIXL_ASSERT(v7.H().Is(h7));
  VIXL_ASSERT(v8.S().Is(s8));
  VIXL_ASSERT(v9.D().Is(d9));

  VIXL_ASSERT(z6.B().Is(b6));
  VIXL_ASSERT(z7.H().Is(h7));
  VIXL_ASSERT(z8.S().Is(s8));
  VIXL_ASSERT(z9.D().Is(d9));

  // We cannot use the same approach for SVE's "z0.B" because, for example,
  // `Add(VRegister, ...)` and `Add(ZRegister, ...)` generate different
  // instructions.

  // Test that the variants can be distinguished with `Is`.
  VIXL_CHECK(!z6.VnB().Is(b6));
  VIXL_CHECK(!z7.VnH().Is(h7));
  VIXL_CHECK(!z8.VnS().Is(s8));
  VIXL_CHECK(!z9.VnD().Is(d9));

  VIXL_CHECK(!z6.VnB().Is(v6.B()));
  VIXL_CHECK(!z7.VnH().Is(v7.H()));
  VIXL_CHECK(!z8.VnS().Is(v8.S()));
  VIXL_CHECK(!z9.VnD().Is(v9.D()));

  VIXL_CHECK(!z6.VnB().Is(z6.B()));
  VIXL_CHECK(!z7.VnH().Is(z7.H()));
  VIXL_CHECK(!z8.VnS().Is(z8.S()));
  VIXL_CHECK(!z9.VnD().Is(z9.D()));

  // Test that the variants can be distinguished at compile-time using
  // overloading. VIXL's API relies on this.
  enum Variant { kNEON, kSVE, kUnknown };
  class Helper {
   public:
    static Variant GetVariant(ZRegister) { return kSVE; }
    static Variant GetVariant(VRegister) { return kNEON; }
    static Variant GetVariant(CPURegister) { return kUnknown; }
  };
  VIXL_CHECK(Helper::GetVariant(z10.VnB()) == kSVE);
  VIXL_CHECK(Helper::GetVariant(z11.VnH()) == kSVE);
  VIXL_CHECK(Helper::GetVariant(z12.VnS()) == kSVE);
  VIXL_CHECK(Helper::GetVariant(z13.VnD()) == kSVE);

  VIXL_CHECK(Helper::GetVariant(v10.B()) == kNEON);
  VIXL_CHECK(Helper::GetVariant(v11.H()) == kNEON);
  VIXL_CHECK(Helper::GetVariant(v12.S()) == kNEON);
  VIXL_CHECK(Helper::GetVariant(v13.D()) == kNEON);

  VIXL_CHECK(Helper::GetVariant(v10.V16B()) == kNEON);
  VIXL_CHECK(Helper::GetVariant(v11.V8H()) == kNEON);
  VIXL_CHECK(Helper::GetVariant(v12.V4S()) == kNEON);
  VIXL_CHECK(Helper::GetVariant(v13.V2D()) == kNEON);

  VIXL_CHECK(Helper::GetVariant(b10) == kNEON);
  VIXL_CHECK(Helper::GetVariant(h11) == kNEON);
  VIXL_CHECK(Helper::GetVariant(s12) == kNEON);
  VIXL_CHECK(Helper::GetVariant(d13) == kNEON);
}


TEST(move_immediate_helpers) {
  // Using these helpers to query information (without generating code) should
  // not crash.
  MacroAssembler::MoveImmediateHelper(NULL, x0, 0x12345678);
  MacroAssembler::OneInstrMoveImmediateHelper(NULL, x1, 0xabcdef);
}


TEST(generic_operand_helpers) {
  GenericOperand invalid_1;
  GenericOperand invalid_2;
  GenericOperand reg(x3);
  GenericOperand mem(MemOperand(sp, 8), kXRegSizeInBytes);

  VIXL_CHECK(!invalid_1.IsValid());
  VIXL_CHECK(!invalid_2.IsValid());

  VIXL_CHECK(invalid_1.Equals(invalid_1));
  VIXL_CHECK(invalid_2.Equals(invalid_2));
  VIXL_CHECK(reg.Equals(reg));
  VIXL_CHECK(mem.Equals(mem));

  VIXL_CHECK(invalid_1.Equals(invalid_2));
  VIXL_CHECK(invalid_2.Equals(invalid_1));

  VIXL_CHECK(!invalid_1.Equals(reg));
  VIXL_CHECK(!invalid_1.Equals(mem));
  VIXL_CHECK(!reg.Equals(invalid_1));
  VIXL_CHECK(!reg.Equals(invalid_2));
  VIXL_CHECK(!reg.Equals(mem));
  VIXL_CHECK(!mem.Equals(invalid_1));
  VIXL_CHECK(!mem.Equals(reg));
}


TEST(static_register_types) {
  // [WX]Register implicitly casts to Register.
  XRegister x_x0(0);
  WRegister w_w0(0);
  Register r_x0 = x_x0;
  Register r_w0 = w_w0;
  VIXL_CHECK(r_x0.Is(x_x0));
  VIXL_CHECK(x_x0.Is(r_x0));
  VIXL_CHECK(r_w0.Is(w_w0));
  VIXL_CHECK(w_w0.Is(r_w0));

  // Register explicitly casts to [WX]Register.
  Register r_x1(1, kXRegSize);
  Register r_w1(1, kWRegSize);
  XRegister x_x1(r_x1);
  WRegister w_w1(r_w1);
  VIXL_CHECK(r_x1.Is(x_x1));
  VIXL_CHECK(x_x1.Is(r_x1));
  VIXL_CHECK(r_w1.Is(w_w1));
  VIXL_CHECK(w_w1.Is(r_w1));

  // [WX]Register implicitly casts to CPURegister.
  XRegister x_x2(2);
  WRegister w_w2(2);
  CPURegister cpu_x2 = x_x2;
  CPURegister cpu_w2 = w_w2;
  VIXL_CHECK(cpu_x2.Is(x_x2));
  VIXL_CHECK(x_x2.Is(cpu_x2));
  VIXL_CHECK(cpu_w2.Is(w_w2));
  VIXL_CHECK(w_w2.Is(cpu_w2));
}


TEST(operand_is_plain_register) {
  VIXL_CHECK(Operand(x0).IsPlainRegister());
  VIXL_CHECK(Operand(x1, LSL, 0).IsPlainRegister());
  VIXL_CHECK(Operand(x2, LSR, 0).IsPlainRegister());
  VIXL_CHECK(Operand(x3, ASR, 0).IsPlainRegister());
  VIXL_CHECK(Operand(x4, ROR, 0).IsPlainRegister());
  VIXL_CHECK(Operand(x5, UXTX).IsPlainRegister());
  VIXL_CHECK(Operand(x6, SXTX).IsPlainRegister());
  VIXL_CHECK(Operand(w7).IsPlainRegister());
  VIXL_CHECK(Operand(w8, LSL, 0).IsPlainRegister());
  VIXL_CHECK(Operand(w9, LSR, 0).IsPlainRegister());
  VIXL_CHECK(Operand(w10, ASR, 0).IsPlainRegister());
  VIXL_CHECK(Operand(w11, ROR, 0).IsPlainRegister());

  VIXL_CHECK(!Operand(x0, LSL, 1).IsPlainRegister());
  VIXL_CHECK(!Operand(x1, LSR, 2).IsPlainRegister());
  VIXL_CHECK(!Operand(x2, ASR, 3).IsPlainRegister());
  VIXL_CHECK(!Operand(x3, ROR, 4).IsPlainRegister());
  VIXL_CHECK(!Operand(x5, UXTX, 1).IsPlainRegister());
  VIXL_CHECK(!Operand(x6, SXTX, 2).IsPlainRegister());
  VIXL_CHECK(!Operand(w7, LSL, 1).IsPlainRegister());
  VIXL_CHECK(!Operand(w8, LSR, 2).IsPlainRegister());
  VIXL_CHECK(!Operand(w9, ASR, 3).IsPlainRegister());
  VIXL_CHECK(!Operand(w10, ROR, 4).IsPlainRegister());
  VIXL_CHECK(!Operand(w11, UXTB).IsPlainRegister());
  VIXL_CHECK(!Operand(w12, SXTB).IsPlainRegister());
  VIXL_CHECK(!Operand(w13, UXTH).IsPlainRegister());
  VIXL_CHECK(!Operand(w14, SXTH).IsPlainRegister());
  // UXTW and SXTW could be treated as plain registers in 32-bit contexts, but
  // the Operand class doesn't know the context so it has to return false.
  VIXL_CHECK(!Operand(w15, UXTW).IsPlainRegister());
  VIXL_CHECK(!Operand(w16, SXTW).IsPlainRegister());
}


TEST(memoperand_is_plain_register) {
  VIXL_CHECK(MemOperand(x0).IsPlainRegister());
  VIXL_CHECK(MemOperand(sp).IsPlainRegister());
  VIXL_CHECK(MemOperand(x1, 0).IsPlainRegister());

  VIXL_CHECK(!MemOperand(x2, xzr).IsPlainRegister());
  VIXL_CHECK(!MemOperand(x3, xzr, SXTX).IsPlainRegister());
  VIXL_CHECK(!MemOperand(x4, xzr, SXTX, 2).IsPlainRegister());
  VIXL_CHECK(!MemOperand(x5, wzr, UXTW).IsPlainRegister());
  VIXL_CHECK(!MemOperand(x6, wzr, UXTW, 3).IsPlainRegister());

  VIXL_CHECK(!MemOperand(x7, 0, PostIndex).IsPlainRegister());
  VIXL_CHECK(!MemOperand(x8, 0, PreIndex).IsPlainRegister());
  VIXL_CHECK(!MemOperand(x9, xzr, PostIndex).IsPlainRegister());

  VIXL_CHECK(!MemOperand(x20, 1).IsPlainRegister());
  VIXL_CHECK(!MemOperand(x21, x30).IsPlainRegister());
}

TEST(memoperand_is_plain_register_or_equivalent) {
  VIXL_CHECK(MemOperand(x0).IsEquivalentToPlainRegister());
  VIXL_CHECK(MemOperand(sp).IsEquivalentToPlainRegister());
  VIXL_CHECK(MemOperand(x1, 0).IsEquivalentToPlainRegister());

  VIXL_CHECK(MemOperand(x2, xzr).IsEquivalentToPlainRegister());
  VIXL_CHECK(MemOperand(x3, xzr, SXTX).IsEquivalentToPlainRegister());
  VIXL_CHECK(MemOperand(x4, xzr, SXTX, 2).IsEquivalentToPlainRegister());
  VIXL_CHECK(MemOperand(x5, wzr, UXTW).IsEquivalentToPlainRegister());
  VIXL_CHECK(MemOperand(x6, wzr, UXTW, 3).IsEquivalentToPlainRegister());

  VIXL_CHECK(MemOperand(x7, 0, PostIndex).IsEquivalentToPlainRegister());
  VIXL_CHECK(MemOperand(x8, 0, PreIndex).IsEquivalentToPlainRegister());
  VIXL_CHECK(MemOperand(x9, xzr, PostIndex).IsEquivalentToPlainRegister());

  VIXL_CHECK(!MemOperand(x20, 1).IsEquivalentToPlainRegister());
  VIXL_CHECK(!MemOperand(x21, x30).IsEquivalentToPlainRegister());
}

TEST(scratch_scope_basic_v) {
  MacroAssembler masm;
  // v31 is the only V scratch register available by default.
  {
    UseScratchRegisterScope temps(&masm);
    VRegister temp = temps.AcquireVRegisterOfSize(kQRegSize);
    VIXL_CHECK(temp.Aliases(v31));
  }
  {
    UseScratchRegisterScope temps(&masm);
    VRegister temp = temps.AcquireVRegisterOfSize(kDRegSize);
    VIXL_CHECK(temp.Aliases(v31));
  }
  {
    UseScratchRegisterScope temps(&masm);
    VRegister temp = temps.AcquireVRegisterOfSize(kSRegSize);
    VIXL_CHECK(temp.Aliases(v31));
  }
}

TEST(scratch_scope_basic_z) {
  MacroAssembler masm;
  // z31 is the only Z scratch register available by default.
  {
    UseScratchRegisterScope temps(&masm);
    VIXL_CHECK(temps.IsAvailable(v31));
    VIXL_CHECK(temps.IsAvailable(z31));
    ZRegisterNoLaneSize temp = temps.AcquireZ();
    VIXL_CHECK(temp.Is(z31));
    // Check that allocating a Z register properly reserves the corresponding V
    // register.
    VIXL_CHECK(!temps.IsAvailable(v31));
    VIXL_CHECK(!temps.IsAvailable(z31));
  }
  // Check that the destructor restored the acquired register.
  UseScratchRegisterScope temps(&masm);
  VIXL_CHECK(temps.IsAvailable(v31));
  VIXL_CHECK(temps.IsAvailable(z31));
}

// The tests below only work with the simulator.
#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64
// Testing data transfer to register on each element access type and size.
#define REG_ACCESSOR_UINT_DOTEST(INPUT, EXPECTED, ELEM, TYPE)                  \
  VIXL_STATIC_ASSERT(sizeof(INPUT) == sizeof(EXPECTED));                       \
  do {                                                                         \
    simulator.ResetState();                                                    \
    simulator.SetVectorLengthInBits(sizeof(INPUT) << 3);                       \
    LogicVRegister reg(simulator.ReadVRegister(0));                            \
    for (int i = 0; i < simulator.LaneCountFromFormat(kFormatVn##ELEM); i++) { \
      reg.SetUint(kFormatVn##ELEM, i, 0);                                      \
      if (i % 2 == 0) {                                                        \
        reg.SetUint(kFormatVn##ELEM, i, reinterpret_cast<TYPE*>(INPUT)[i]);    \
      }                                                                        \
      VIXL_CHECK(static_cast<TYPE>(EXPECTED[i]) ==                             \
                 static_cast<TYPE>(reg.Uint(kFormatVn##ELEM, i)));             \
    }                                                                          \
  } while (0)

#define REG_ACCESSOR_INT_DOTEST(INPUT, EXPECTED, ELEM, TYPE)                   \
  VIXL_STATIC_ASSERT(sizeof(INPUT) == sizeof(EXPECTED));                       \
  do {                                                                         \
    simulator.ResetState();                                                    \
    simulator.SetVectorLengthInBits(sizeof(INPUT) << 3);                       \
    LogicVRegister reg(simulator.ReadVRegister(0));                            \
    for (int i = 0; i < simulator.LaneCountFromFormat(kFormatVn##ELEM); i++) { \
      reg.SetInt(kFormatVn##ELEM, i, 0);                                       \
      if (i % 2 == 0) {                                                        \
        reg.SetInt(kFormatVn##ELEM, i, reinterpret_cast<TYPE*>(INPUT)[i]);     \
      }                                                                        \
      VIXL_CHECK(static_cast<TYPE>(EXPECTED[i]) ==                             \
                 static_cast<TYPE>(reg.Int(kFormatVn##ELEM, i)));              \
    }                                                                          \
  } while (0)

TEST(sve_logic_z_register) {
  // Initialize a 256-bit vector for the example function.
  // clang-format off
  uint8_t input[32] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d, 0x8f,
                       0x91, 0x93, 0x95, 0x97, 0x99, 0x9b, 0x9d, 0x9f,
                       0xa1, 0xa3, 0xa5, 0xa7, 0xa9, 0xab, 0xad, 0xaf,
                       0xb1, 0xb3, 0xb5, 0xb7, 0xb9, 0xbb, 0xbd, 0xbf};

  uint64_t D2D[4] = {0x8f8d8b8987858381, 0x0000000000000000,
                     0xafadaba9a7a5a3a1, 0x0000000000000000};

  uint32_t W2W[8] = {0x87858381, 0x0000000, 0x97959391, 0x00000000,
                     0xa7a5a3a1, 0x0000000, 0xb7b5b3b1, 0x00000000};

  uint16_t H2H[16] = {0x8381, 0x0000, 0x8b89, 0x0000, 0x9391, 0x0000, 0x9b99, 0x0000,
                      0xa3a1, 0x0000, 0xaba9, 0x0000, 0xb3b1, 0x0000, 0xbbb9, 0x0000};

  uint8_t B2B[32] = {0x81, 0x00, 0x85, 0x00, 0x89, 0x00, 0x8d, 0x00,
                     0x91, 0x00, 0x95, 0x00, 0x99, 0x00, 0x9d, 0x00,
                     0xa1, 0x00, 0xa5, 0x00, 0xa9, 0x00, 0xad, 0x00,
                     0xb1, 0x00, 0xb5, 0x00, 0xb9, 0x00, 0xbd, 0x00};

  uint64_t W2D[4] = {0x0000000087858381, 0x0000000000000000,
                    0x0000000097959391, 0x0000000000000000};

  uint64_t SW2D[4] = {0xffffffff87858381, 0x0000000000000000,
                     0xffffffff97959391, 0x0000000000000000};

  uint32_t H2W[8] = {0x00008381, 0x00000000, 0x00008b89, 0x00000000,
                    0x00009391, 0x00000000, 0x00009b99, 0x00000000};

  uint32_t SH2W[8] = {0xffff8381, 0x00000000, 0xffff8b89, 0x00000000,
                     0xffff9391, 0x00000000, 0xffff9b99, 0x00000000};

  uint16_t B2H[16] = {0x0081, 0x0000, 0x0085, 0x0000, 0x0089, 0x0000, 0x008d, 0x0000,
                     0x0091, 0x0000, 0x0095, 0x0000, 0x0099, 0x0000, 0x009d, 0x0000};

  uint16_t SB2H[16] = {0xff81, 0x0000, 0xff85, 0x0000, 0xff89, 0x0000, 0xff8d, 0x0000,
                      0xff91, 0x0000, 0xff95, 0x0000, 0xff99, 0x0000, 0xff9d, 0x0000};
  // clang-format on

  MacroAssembler masm;
  Decoder decoder;
  Simulator simulator(&decoder);
  // Test packed access.
  REG_ACCESSOR_UINT_DOTEST(input, D2D, D, uint64_t);
  REG_ACCESSOR_UINT_DOTEST(input, W2W, S, uint32_t);
  REG_ACCESSOR_UINT_DOTEST(input, H2H, H, uint16_t);
  REG_ACCESSOR_UINT_DOTEST(input, B2B, B, uint8_t);

  // Test unpacked access, either sign-extended or zero-extended.
  REG_ACCESSOR_UINT_DOTEST(input, B2H, H, uint8_t);
  REG_ACCESSOR_UINT_DOTEST(input, W2D, D, uint32_t);
  REG_ACCESSOR_UINT_DOTEST(input, H2W, S, uint16_t);
  REG_ACCESSOR_INT_DOTEST(input, SB2H, H, int8_t);
  REG_ACCESSOR_INT_DOTEST(input, SW2D, D, int32_t);
  REG_ACCESSOR_INT_DOTEST(input, SH2W, S, int16_t);
}

TEST(sve_logic_p_register) {
  uint16_t input1[2] = {0x8381, 0x8785};
  uint16_t input2[2] = {0x8b89, 0x8f8d};
  uint16_t input3[2] = {0xa9a8, 0xafaa};
  uint16_t expected[2] = {0x8180, 0x8780};
  uint16_t result[2] = {0};

  MacroAssembler masm;
  Decoder decoder;
  Simulator simulator(&decoder);
  simulator.ResetState();
  simulator.SetVectorLengthInBits(256);

  LogicPRegister Pg(simulator.ReadPRegister(5));
  LogicPRegister Pm(simulator.ReadPRegister(4));
  LogicPRegister Pn(simulator.ReadPRegister(3));
  LogicPRegister Pd(simulator.ReadPRegister(2));
  int p_size_in_bytes = simulator.GetVectorLengthInBytes() / 8;
  int p_size_in_chunks = p_size_in_bytes / sizeof(LogicPRegister::ChunkType);
  for (int i = 0; i < p_size_in_chunks; i++) {
    Pm.SetChunk(i, input1[i]);
    Pn.SetChunk(i, input2[i]);
    Pg.SetChunk(i, input3[i]);
  }

  for (int i = 0; i < p_size_in_chunks; i++) {
    uint16_t chunk1 = Pm.GetChunk(i);
    uint16_t chunk2 = Pn.GetChunk(i);
    uint16_t mask = Pg.GetChunk(i);
    VIXL_CHECK(input1[i] == chunk1);
    VIXL_CHECK(input2[i] == chunk2);
    VIXL_CHECK(input3[i] == mask);
    result[i] = (chunk1 & chunk2) & mask;
    VIXL_CHECK(result[i] == expected[i]);
    Pd.SetChunk(i, result[i]);
  }

  VIXL_CHECK(Pd.IsActive(kFormatVnD, 3) == true);
  VIXL_CHECK(Pd.IsActive(kFormatVnS, 6) == true);
  VIXL_CHECK(Pd.IsActive(kFormatVnS, 5) == false);
  VIXL_CHECK(Pd.IsActive(kFormatVnH, 6) == false);
  VIXL_CHECK(Pd.IsActive(kFormatVnH, 5) == false);
  VIXL_CHECK(Pd.IsActive(kFormatVnB, 7) == true);
  VIXL_CHECK(Pd.IsActive(kFormatVnB, 6) == false);

  Pd.SetActive(kFormatVnD, 3, false);
  Pd.SetActive(kFormatVnS, 5, true);
  Pd.SetActive(kFormatVnH, 5, true);
  Pd.SetActive(kFormatVnB, 6, true);

  VIXL_CHECK(Pd.IsActive(kFormatVnD, 3) == false);
  VIXL_CHECK(Pd.IsActive(kFormatVnS, 6) == false);
  VIXL_CHECK(Pd.IsActive(kFormatVnS, 5) == true);
  VIXL_CHECK(Pd.IsActive(kFormatVnH, 6) == false);
  VIXL_CHECK(Pd.IsActive(kFormatVnH, 5) == true);
  VIXL_CHECK(Pd.IsActive(kFormatVnB, 7) == true);
  VIXL_CHECK(Pd.IsActive(kFormatVnB, 6) == true);
}

TEST(sve_first_none_last_active) {
  uint16_t mask[2] = {0x8785, 0x8381};
  uint16_t bits[2] = {0xaca8, 0xacaa};

  MacroAssembler masm;
  Decoder decoder;
  Simulator simulator(&decoder);
  simulator.ResetState();
  simulator.SetVectorLengthInBits(256);

  LogicPRegister Pg(simulator.ReadPRegister(5));
  LogicPRegister Pm(simulator.ReadPRegister(4));
  int p_size_in_bytes = simulator.GetVectorLengthInBytes() / 8;
  int p_size_in_chunks = p_size_in_bytes / sizeof(LogicPRegister::ChunkType);
  for (int i = 0; i < p_size_in_chunks; i++) {
    Pm.SetChunk(i, mask[i]);
    Pg.SetChunk(i, bits[i]);
  }

  VIXL_CHECK(simulator.IsFirstActive(kFormatVnD, Pg, Pm) == false);
  VIXL_CHECK(simulator.IsFirstActive(kFormatVnS, Pg, Pm) == false);
  VIXL_CHECK(simulator.IsFirstActive(kFormatVnH, Pg, Pm) == true);
  VIXL_CHECK(simulator.IsFirstActive(kFormatVnB, Pg, Pm) == false);

  VIXL_CHECK(simulator.AreNoneActive(kFormatVnD, Pg, Pm) == true);
  VIXL_CHECK(simulator.AreNoneActive(kFormatVnS, Pg, Pm) == true);
  VIXL_CHECK(simulator.AreNoneActive(kFormatVnH, Pg, Pm) == false);
  VIXL_CHECK(simulator.AreNoneActive(kFormatVnB, Pg, Pm) == false);

  VIXL_CHECK(simulator.IsLastActive(kFormatVnD, Pg, Pm) == false);
  VIXL_CHECK(simulator.IsLastActive(kFormatVnS, Pg, Pm) == false);
  VIXL_CHECK(simulator.IsLastActive(kFormatVnH, Pg, Pm) == false);
  VIXL_CHECK(simulator.IsLastActive(kFormatVnB, Pg, Pm) == true);
}
#undef REG_ACCESSOR_UINT_DOTEST
#endif
}  // namespace aarch64
}  // namespace vixl
