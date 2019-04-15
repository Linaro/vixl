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


TEST(is_plain_register) {
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


}  // namespace aarch64
}  // namespace vixl
