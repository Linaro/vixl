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

// This file holds inputs for the instructions tested by test-simulator-a64.
//
// If the input lists are updated, please run tools/generate_simulator_traces.py
// on a reference platform to regenerate the expected outputs. The outputs are
// stored in test-simulator-traces-a64.h.

#include <stdint.h>

// This header should only be used by test/test-simulator-a64.cc, so it
// doesn't need the usual header guard.
#ifdef VIXL_A64_TEST_SIMULATOR_INPUTS_A64_H_
#error This header should be inluded only once.
#endif
#define VIXL_A64_TEST_SIMULATOR_INPUTS_A64_H_

// Double values, stored as uint64_t representations. This ensures exact bit
// representation, and avoids the loss of NaNs and suchlike through C++ casts.
static const uint64_t kInputDouble[] = {
  // Simple values.
  0x0000000000000000,   // 0.0
  0x0010000000000000,   // The smallest normal value.
  0x3fdfffffffffffff,   // The value just below 0.5.
  0x3fe0000000000000,   // 0.5
  0x3fe0000000000001,   // The value just above 0.5.
  0x3fefffffffffffff,   // The value just below 1.0.
  0x3ff0000000000000,   // 1.0
  0x3ff0000000000001,   // The value just above 1.0.
  0x3ff8000000000000,   // 1.5
  0x4024000000000000,   // 10
  0x7fefffffffffffff,   // The largest finite value.

  // Infinity.
  0x7ff0000000000000,

  // NaNs.
  //  - Quiet NaNs
  0x7ff923456789abcd,
  0x7ff8000000000000,
  //  - Signalling NaNs
  0x7ff123456789abcd,
  0x7ff0000000000000,

  // Subnormals.
  //  - A recognisable bit pattern.
  0x000123456789abcd,
  //  - The largest subnormal value.
  0x000fffffffffffff,
  //  - The smallest subnormal value.
  0x0000000000000001,

  // The same values again, but negated.
  0x8000000000000000,
  0x8010000000000000,
  0xbfdfffffffffffff,
  0xbfe0000000000000,
  0xbfe0000000000001,
  0xbfefffffffffffff,
  0xbff0000000000000,
  0xbff0000000000001,
  0xbff8000000000000,
  0xc024000000000000,
  0xffefffffffffffff,
  0xfff0000000000000,
  0xfff923456789abcd,
  0xfff8000000000000,
  0xfff123456789abcd,
  0xfff0000000000000,
  0x800123456789abcd,
  0x800fffffffffffff,
  0x8000000000000001,


// TODO(jbramley): Add these inputs to the fcvt tests. They have little value in
// other instructions and they will massively inflate the trace size.
#if 0
  // Values relevant for conversion to single-precision floats.
  double_to_rawbits(FLT_MAX),
  //  - The smallest normalized float.
  double_to_rawbits(pow(2, -126)),
  //  - Normal floats that need (ties-to-even) rounding.
  //    For normalized numbers, bit 29 (0x0000000020000000) is the lowest-order
  //    bit which will fit in the float's mantissa.
  0x3ff0000000000000,
  0x3ff0000000000001,
  0x3ff0000010000000,
  0x3ff0000010000001,
  0x3ff0000020000000,
  0x3ff0000020000001,
  0x3ff0000030000000,
  0x3ff0000030000001,
  0x3ff0000040000000,
  0x3ff0000040000001,
  0x3ff0000050000000,
  0x3ff0000050000001,
  0x3ff0000060000000,
  //  - A mantissa that overflows into the exponent during rounding.
  0x3feffffff0000000,
  //  - The largest double that rounds to a normal float.
  0x47efffffefffffff,
  //  - The smallest exponent that's too big for a float.
  double_to_rawbits(pow(2, 128)),
  //  - This exponent is in range, but the value rounds to infinity.
  0x47effffff0000000,
  //  - The largest double which is too small for a subnormal float.
  0x3690000000000000,
  //  - The largest subnormal float.
  0x380fffffc0000000,
  //  - The smallest subnormal float.
  0x36a0000000000000,
  //  - Subnormal floats that need (ties-to-even) rounding.
  //    For these subnormals, bit 34 (0x0000000400000000) is the lowest-order
  //    bit which will fit in the float's mantissa.
  0x37c159e000000000,
  0x37c159e000000001,
  0x37c159e200000000,
  0x37c159e200000001,
  0x37c159e400000000,
  0x37c159e400000001,
  0x37c159e600000000,
  0x37c159e600000001,
  0x37c159e800000000,
  0x37c159e800000001,
  0x37c159ea00000000,
  0x37c159ea00000001,
  0x37c159ec00000000,
  //  - The smallest double which rounds up to become a subnormal float.
  0x3690000000000001,
#endif
};
static const size_t kInputDoubleCount =
    sizeof(kInputDouble) / sizeof(kInputDouble[0]);


// Float values, stored as uint32_t representations. This ensures exact bit
// representation, and avoids the loss of NaNs and suchlike through C++ casts.
static const uint32_t kInputFloat[] = {
  // Simple values.
  0x00000000,   // 0.0
  0x00800000,   // The smallest normal value.
  0x3effffff,   // The value just below 0.5.
  0x3f000000,   // 0.5
  0x3f000001,   // The value just above 0.5.
  0x3f7fffff,   // The value just below 1.0.
  0x3f800000,   // 1.0
  0x3f800001,   // The value just above 1.0.
  0x3fc00000,   // 1.5
  0x41200000,   // 10
  0x7f8fffff,   // The largest finite value.

  // Infinity.
  0x7f800000,

  // NaNs.
  //  - Quiet NaNs
  0x7fd23456,
  0x7fc00000,
  //  - Signalling NaNs
  0x7f923456,
  0x7f800001,

  // Subnormals.
  //  - A recognisable bit pattern.
  0x00123456,
  //  - The largest subnormal value.
  0x007fffff,
  //  - The smallest subnormal value.
  0x00000001,

  // The same values again, but negated.
  0x80000000,
  0x80800000,
  0xbeffffff,
  0xbf000000,
  0xbf000001,
  0xbf7fffff,
  0xbf800000,
  0xbf800001,
  0xbfc00000,
  0xc1200000,
  0xff8fffff,
  0xff800000,
  0xffd23456,
  0xffc00000,
  0xff923456,
  0xff800001,
  0x80123456,
  0x807fffff,
  0x80000001,
};
static const size_t kInputFloatCount =
    sizeof(kInputFloat) / sizeof(kInputFloat[0]);

