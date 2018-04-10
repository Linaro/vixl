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

#include "utils-aarch64.h"

namespace vixl {
namespace aarch64 {

float FPToFloat(float16 value, UseDefaultNaN DN, bool* exception) {
  uint32_t sign = value >> 15;
  uint32_t exponent =
      ExtractUnsignedBitfield32(kFloat16MantissaBits + kFloat16ExponentBits - 1,
                                kFloat16MantissaBits,
                                value);
  uint32_t mantissa =
      ExtractUnsignedBitfield32(kFloat16MantissaBits - 1, 0, value);

  switch (Float16Classify(value)) {
    case FP_ZERO:
      return (sign == 0) ? 0.0f : -0.0f;

    case FP_INFINITE:
      return (sign == 0) ? kFP32PositiveInfinity : kFP32NegativeInfinity;

    case FP_SUBNORMAL: {
      // Calculate shift required to put mantissa into the most-significant bits
      // of the destination mantissa.
      int shift = CountLeadingZeros(mantissa << (32 - 10));

      // Shift mantissa and discard implicit '1'.
      mantissa <<= (kFloatMantissaBits - kFloat16MantissaBits) + shift + 1;
      mantissa &= (1 << kFloatMantissaBits) - 1;

      // Adjust the exponent for the shift applied, and rebias.
      exponent = exponent - shift + (-15 + 127);
      break;
    }

    case FP_NAN:
      if (IsSignallingNaN(value)) {
        if (exception != NULL) {
          *exception = true;
        }
      }
      if (DN == kUseDefaultNaN) return kFP32DefaultNaN;

      // Convert NaNs as the processor would:
      //  - The sign is propagated.
      //  - The payload (mantissa) is transferred entirely, except that the top
      //    bit is forced to '1', making the result a quiet NaN. The unused
      //    (low-order) payload bits are set to 0.
      exponent = (1 << kFloatExponentBits) - 1;

      // Increase bits in mantissa, making low-order bits 0.
      mantissa <<= (kFloatMantissaBits - kFloat16MantissaBits);
      mantissa |= 1 << 22;  // Force a quiet NaN.
      break;

    case FP_NORMAL:
      // Increase bits in mantissa, making low-order bits 0.
      mantissa <<= (kFloatMantissaBits - kFloat16MantissaBits);

      // Change exponent bias.
      exponent += (-15 + 127);
      break;

    default:
      VIXL_UNREACHABLE();
  }
  return RawbitsToFloat((sign << 31) | (exponent << kFloatMantissaBits) |
                        mantissa);
}


float FPToFloat(double value,
                FPRounding round_mode,
                UseDefaultNaN DN,
                bool* exception) {
  // Only the FPTieEven rounding mode is implemented.
  VIXL_ASSERT((round_mode == FPTieEven) || (round_mode == FPRoundOdd));
  USE(round_mode);

  switch (std::fpclassify(value)) {
    case FP_NAN: {
      if (IsSignallingNaN(value)) {
        if (exception != NULL) {
          *exception = true;
        }
      }
      if (DN == kUseDefaultNaN) return kFP32DefaultNaN;

      // Convert NaNs as the processor would:
      //  - The sign is propagated.
      //  - The payload (mantissa) is transferred as much as possible, except
      //    that the top bit is forced to '1', making the result a quiet NaN.
      uint64_t raw = DoubleToRawbits(value);

      uint32_t sign = raw >> 63;
      uint32_t exponent = (1 << 8) - 1;
      uint32_t payload =
          static_cast<uint32_t>(ExtractUnsignedBitfield64(50, 52 - 23, raw));
      payload |= (1 << 22);  // Force a quiet NaN.

      return RawbitsToFloat((sign << 31) | (exponent << 23) | payload);
    }

    case FP_ZERO:
    case FP_INFINITE: {
      // In a C++ cast, any value representable in the target type will be
      // unchanged. This is always the case for +/-0.0 and infinities.
      return static_cast<float>(value);
    }

    case FP_NORMAL:
    case FP_SUBNORMAL: {
      // Convert double-to-float as the processor would, assuming that FPCR.FZ
      // (flush-to-zero) is not set.
      uint64_t raw = DoubleToRawbits(value);
      // Extract the IEEE-754 double components.
      uint32_t sign = raw >> 63;
      // Extract the exponent and remove the IEEE-754 encoding bias.
      int32_t exponent =
          static_cast<int32_t>(ExtractUnsignedBitfield64(62, 52, raw)) - 1023;
      // Extract the mantissa and add the implicit '1' bit.
      uint64_t mantissa = ExtractUnsignedBitfield64(51, 0, raw);
      if (std::fpclassify(value) == FP_NORMAL) {
        mantissa |= (UINT64_C(1) << 52);
      }
      return FPRoundToFloat(sign, exponent, mantissa, round_mode);
    }
  }

  VIXL_UNREACHABLE();
  return value;
}


double FPToDouble(float value, UseDefaultNaN DN, bool* exception) {
  switch (std::fpclassify(value)) {
    case FP_NAN: {
      if (IsSignallingNaN(value)) {
        if (exception != NULL) {
          *exception = true;
        }
      }
      if (DN == kUseDefaultNaN) return kFP64DefaultNaN;

      // Convert NaNs as the processor would:
      //  - The sign is propagated.
      //  - The payload (mantissa) is transferred entirely, except that the top
      //    bit is forced to '1', making the result a quiet NaN. The unused
      //    (low-order) payload bits are set to 0.
      uint32_t raw = FloatToRawbits(value);

      uint64_t sign = raw >> 31;
      uint64_t exponent = (1 << 11) - 1;
      uint64_t payload = ExtractUnsignedBitfield64(21, 0, raw);
      payload <<= (52 - 23);           // The unused low-order bits should be 0.
      payload |= (UINT64_C(1) << 51);  // Force a quiet NaN.

      return RawbitsToDouble((sign << 63) | (exponent << 52) | payload);
    }

    case FP_ZERO:
    case FP_NORMAL:
    case FP_SUBNORMAL:
    case FP_INFINITE: {
      // All other inputs are preserved in a standard cast, because every value
      // representable using an IEEE-754 float is also representable using an
      // IEEE-754 double.
      return static_cast<double>(value);
    }
  }

  VIXL_UNREACHABLE();
  return static_cast<double>(value);
}


float16 FPToFloat16(float value,
                    FPRounding round_mode,
                    UseDefaultNaN DN,
                    bool* exception) {
  // Only the FPTieEven rounding mode is implemented.
  VIXL_ASSERT(round_mode == FPTieEven);
  USE(round_mode);

  uint32_t raw = FloatToRawbits(value);
  int32_t sign = raw >> 31;
  int32_t exponent = ExtractUnsignedBitfield32(30, 23, raw) - 127;
  uint32_t mantissa = ExtractUnsignedBitfield32(22, 0, raw);

  switch (std::fpclassify(value)) {
    case FP_NAN: {
      if (IsSignallingNaN(value)) {
        if (exception != NULL) {
          *exception = true;
        }
      }
      if (DN == kUseDefaultNaN) return kFP16DefaultNaN;

      // Convert NaNs as the processor would:
      //  - The sign is propagated.
      //  - The payload (mantissa) is transferred as much as possible, except
      //    that the top bit is forced to '1', making the result a quiet NaN.
      float16 result =
          (sign == 0) ? kFP16PositiveInfinity : kFP16NegativeInfinity;
      result |= mantissa >> (kFloatMantissaBits - kFloat16MantissaBits);
      result |= (1 << 9);  // Force a quiet NaN;
      return result;
    }

    case FP_ZERO:
      return (sign == 0) ? 0 : 0x8000;

    case FP_INFINITE:
      return (sign == 0) ? kFP16PositiveInfinity : kFP16NegativeInfinity;

    case FP_NORMAL:
    case FP_SUBNORMAL: {
      // Convert float-to-half as the processor would, assuming that FPCR.FZ
      // (flush-to-zero) is not set.

      // Add the implicit '1' bit to the mantissa.
      mantissa += (1 << 23);
      return FPRoundToFloat16(sign, exponent, mantissa, round_mode);
    }
  }

  VIXL_UNREACHABLE();
  return 0;
}


float16 FPToFloat16(double value,
                    FPRounding round_mode,
                    UseDefaultNaN DN,
                    bool* exception) {
  // Only the FPTieEven rounding mode is implemented.
  VIXL_ASSERT(round_mode == FPTieEven);
  USE(round_mode);

  uint64_t raw = DoubleToRawbits(value);
  int32_t sign = raw >> 63;
  int64_t exponent = ExtractUnsignedBitfield64(62, 52, raw) - 1023;
  uint64_t mantissa = ExtractUnsignedBitfield64(51, 0, raw);

  switch (std::fpclassify(value)) {
    case FP_NAN: {
      if (IsSignallingNaN(value)) {
        if (exception != NULL) {
          *exception = true;
        }
      }
      if (DN == kUseDefaultNaN) return kFP16DefaultNaN;

      // Convert NaNs as the processor would:
      //  - The sign is propagated.
      //  - The payload (mantissa) is transferred as much as possible, except
      //    that the top bit is forced to '1', making the result a quiet NaN.
      float16 result =
          (sign == 0) ? kFP16PositiveInfinity : kFP16NegativeInfinity;
      result |= mantissa >> (kDoubleMantissaBits - kFloat16MantissaBits);
      result |= (1 << 9);  // Force a quiet NaN;
      return result;
    }

    case FP_ZERO:
      return (sign == 0) ? 0 : 0x8000;

    case FP_INFINITE:
      return (sign == 0) ? kFP16PositiveInfinity : kFP16NegativeInfinity;
    case FP_NORMAL:
    case FP_SUBNORMAL: {
      // Convert double-to-half as the processor would, assuming that FPCR.FZ
      // (flush-to-zero) is not set.

      // Add the implicit '1' bit to the mantissa.
      mantissa += (UINT64_C(1) << 52);
      return FPRoundToFloat16(sign, exponent, mantissa, round_mode);
    }
  }

  VIXL_UNREACHABLE();
  return 0;
}
}  // namespace aarch64
}  // namespace vixl
