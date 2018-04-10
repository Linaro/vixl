// Copyright 2018, VIXL authors
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

#ifndef VIXL_AARCH64_UTILS_AARCH64_H_
#define VIXL_AARCH64_UTILS_AARCH64_H_

#include "instructions-aarch64.h"

namespace vixl {
namespace aarch64 {

enum UseDefaultNaN { kUseDefaultNaN, kIgnoreDefaultNaN };

// Assemble the specified IEEE-754 components into the target type and apply
// appropriate rounding.
//  sign:     0 = positive, 1 = negative
//  exponent: Unbiased IEEE-754 exponent.
//  mantissa: The mantissa of the input. The top bit (which is not encoded for
//            normal IEEE-754 values) must not be omitted. This bit has the
//            value 'pow(2, exponent)'.
//
// The input value is assumed to be a normalized value. That is, the input may
// not be infinity or NaN. If the source value is subnormal, it must be
// normalized before calling this function such that the highest set bit in the
// mantissa has the value 'pow(2, exponent)'.
//
// Callers should use FPRoundToFloat or FPRoundToDouble directly, rather than
// calling a templated FPRound.
template <class T, int ebits, int mbits>
T FPRound(int64_t sign,
          int64_t exponent,
          uint64_t mantissa,
          FPRounding round_mode) {
  VIXL_ASSERT((sign == 0) || (sign == 1));

  // Only FPTieEven and FPRoundOdd rounding modes are implemented.
  VIXL_ASSERT((round_mode == FPTieEven) || (round_mode == FPRoundOdd));

  // Rounding can promote subnormals to normals, and normals to infinities. For
  // example, a double with exponent 127 (FLT_MAX_EXP) would appear to be
  // encodable as a float, but rounding based on the low-order mantissa bits
  // could make it overflow. With ties-to-even rounding, this value would become
  // an infinity.

  // ---- Rounding Method ----
  //
  // The exponent is irrelevant in the rounding operation, so we treat the
  // lowest-order bit that will fit into the result ('onebit') as having
  // the value '1'. Similarly, the highest-order bit that won't fit into
  // the result ('halfbit') has the value '0.5'. The 'point' sits between
  // 'onebit' and 'halfbit':
  //
  //            These bits fit into the result.
  //               |---------------------|
  //  mantissa = 0bxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  //                                     ||
  //                                    / |
  //                                   /  halfbit
  //                               onebit
  //
  // For subnormal outputs, the range of representable bits is smaller and
  // the position of onebit and halfbit depends on the exponent of the
  // input, but the method is otherwise similar.
  //
  //   onebit(frac)
  //     |
  //     | halfbit(frac)          halfbit(adjusted)
  //     | /                      /
  //     | |                      |
  //  0b00.0 (exact)      -> 0b00.0 (exact)                    -> 0b00
  //  0b00.0...           -> 0b00.0...                         -> 0b00
  //  0b00.1 (exact)      -> 0b00.0111..111                    -> 0b00
  //  0b00.1...           -> 0b00.1...                         -> 0b01
  //  0b01.0 (exact)      -> 0b01.0 (exact)                    -> 0b01
  //  0b01.0...           -> 0b01.0...                         -> 0b01
  //  0b01.1 (exact)      -> 0b01.1 (exact)                    -> 0b10
  //  0b01.1...           -> 0b01.1...                         -> 0b10
  //  0b10.0 (exact)      -> 0b10.0 (exact)                    -> 0b10
  //  0b10.0...           -> 0b10.0...                         -> 0b10
  //  0b10.1 (exact)      -> 0b10.0111..111                    -> 0b10
  //  0b10.1...           -> 0b10.1...                         -> 0b11
  //  0b11.0 (exact)      -> 0b11.0 (exact)                    -> 0b11
  //  ...                   /             |                      /   |
  //                       /              |                     /    |
  //                                                           /     |
  // adjusted = frac - (halfbit(mantissa) & ~onebit(frac));   /      |
  //
  //                   mantissa = (mantissa >> shift) + halfbit(adjusted);

  static const int mantissa_offset = 0;
  static const int exponent_offset = mantissa_offset + mbits;
  static const int sign_offset = exponent_offset + ebits;
  VIXL_ASSERT(sign_offset == (sizeof(T) * 8 - 1));

  // Bail out early for zero inputs.
  if (mantissa == 0) {
    return static_cast<T>(sign << sign_offset);
  }

  // If all bits in the exponent are set, the value is infinite or NaN.
  // This is true for all binary IEEE-754 formats.
  static const int infinite_exponent = (1 << ebits) - 1;
  static const int max_normal_exponent = infinite_exponent - 1;

  // Apply the exponent bias to encode it for the result. Doing this early makes
  // it easy to detect values that will be infinite or subnormal.
  exponent += max_normal_exponent >> 1;

  if (exponent > max_normal_exponent) {
    // Overflow: the input is too large for the result type to represent.
    if (round_mode == FPTieEven) {
      // FPTieEven rounding mode handles overflows using infinities.
      exponent = infinite_exponent;
      mantissa = 0;
    } else {
      VIXL_ASSERT(round_mode == FPRoundOdd);
      // FPRoundOdd rounding mode handles overflows using the largest magnitude
      // normal number.
      exponent = max_normal_exponent;
      mantissa = (UINT64_C(1) << exponent_offset) - 1;
    }
    return static_cast<T>((sign << sign_offset) |
                          (exponent << exponent_offset) |
                          (mantissa << mantissa_offset));
  }

  // Calculate the shift required to move the top mantissa bit to the proper
  // place in the destination type.
  const int highest_significant_bit = 63 - CountLeadingZeros(mantissa);
  int shift = highest_significant_bit - mbits;

  if (exponent <= 0) {
    // The output will be subnormal (before rounding).
    // For subnormal outputs, the shift must be adjusted by the exponent. The +1
    // is necessary because the exponent of a subnormal value (encoded as 0) is
    // the same as the exponent of the smallest normal value (encoded as 1).
    shift += -exponent + 1;

    // Handle inputs that would produce a zero output.
    //
    // Shifts higher than highest_significant_bit+1 will always produce a zero
    // result. A shift of exactly highest_significant_bit+1 might produce a
    // non-zero result after rounding.
    if (shift > (highest_significant_bit + 1)) {
      if (round_mode == FPTieEven) {
        // The result will always be +/-0.0.
        return static_cast<T>(sign << sign_offset);
      } else {
        VIXL_ASSERT(round_mode == FPRoundOdd);
        VIXL_ASSERT(mantissa != 0);
        // For FPRoundOdd, if the mantissa is too small to represent and
        // non-zero return the next "odd" value.
        return static_cast<T>((sign << sign_offset) | 1);
      }
    }

    // Properly encode the exponent for a subnormal output.
    exponent = 0;
  } else {
    // Clear the topmost mantissa bit, since this is not encoded in IEEE-754
    // normal values.
    mantissa &= ~(UINT64_C(1) << highest_significant_bit);
  }

  if (shift > 0) {
    if (round_mode == FPTieEven) {
      // We have to shift the mantissa to the right. Some precision is lost, so
      // we need to apply rounding.
      uint64_t onebit_mantissa = (mantissa >> (shift)) & 1;
      uint64_t halfbit_mantissa = (mantissa >> (shift - 1)) & 1;
      uint64_t adjustment = (halfbit_mantissa & ~onebit_mantissa);
      uint64_t adjusted = mantissa - adjustment;
      T halfbit_adjusted = (adjusted >> (shift - 1)) & 1;

      T result =
          static_cast<T>((sign << sign_offset) | (exponent << exponent_offset) |
                         ((mantissa >> shift) << mantissa_offset));

      // A very large mantissa can overflow during rounding. If this happens,
      // the exponent should be incremented and the mantissa set to 1.0
      // (encoded as 0). Applying halfbit_adjusted after assembling the float
      // has the nice side-effect that this case is handled for free.
      //
      // This also handles cases where a very large finite value overflows to
      // infinity, or where a very large subnormal value overflows to become
      // normal.
      return result + halfbit_adjusted;
    } else {
      VIXL_ASSERT(round_mode == FPRoundOdd);
      // If any bits at position halfbit or below are set, onebit (ie. the
      // bottom bit of the resulting mantissa) must be set.
      uint64_t fractional_bits = mantissa & ((UINT64_C(1) << shift) - 1);
      if (fractional_bits != 0) {
        mantissa |= UINT64_C(1) << shift;
      }

      return static_cast<T>((sign << sign_offset) |
                            (exponent << exponent_offset) |
                            ((mantissa >> shift) << mantissa_offset));
    }
  } else {
    // We have to shift the mantissa to the left (or not at all). The input
    // mantissa is exactly representable in the output mantissa, so apply no
    // rounding correction.
    return static_cast<T>((sign << sign_offset) |
                          (exponent << exponent_offset) |
                          ((mantissa << -shift) << mantissa_offset));
  }
}


// See FPRound for a description of this function.
inline double FPRoundToDouble(int64_t sign,
                              int64_t exponent,
                              uint64_t mantissa,
                              FPRounding round_mode) {
  int64_t bits =
      FPRound<int64_t, kDoubleExponentBits, kDoubleMantissaBits>(sign,
                                                                 exponent,
                                                                 mantissa,
                                                                 round_mode);
  return RawbitsToDouble(bits);
}


// See FPRound for a description of this function.
inline float16 FPRoundToFloat16(int64_t sign,
                                int64_t exponent,
                                uint64_t mantissa,
                                FPRounding round_mode) {
  return FPRound<float16,
                 kFloat16ExponentBits,
                 kFloat16MantissaBits>(sign, exponent, mantissa, round_mode);
}


// See FPRound for a description of this function.
static inline float FPRoundToFloat(int64_t sign,
                                   int64_t exponent,
                                   uint64_t mantissa,
                                   FPRounding round_mode) {
  int32_t bits =
      FPRound<int32_t, kFloatExponentBits, kFloatMantissaBits>(sign,
                                                               exponent,
                                                               mantissa,
                                                               round_mode);
  return RawbitsToFloat(bits);
}


float FPToFloat(float16 value, UseDefaultNaN DN, bool* exception = NULL);
float FPToFloat(double value,
                FPRounding round_mode,
                UseDefaultNaN DN,
                bool* exception = NULL);

double FPToDouble(float value, UseDefaultNaN DN, bool* exception = NULL);

float16 FPToFloat16(float value,
                    FPRounding round_mode,
                    UseDefaultNaN DN,
                    bool* exception = NULL);

float16 FPToFloat16(double value,
                    FPRounding round_mode,
                    UseDefaultNaN DN,
                    bool* exception = NULL);
}  // namespace aarch64
}  // namespace vixl

#endif  // VIXL_AARCH64_UTILS_AARCH64_H_
