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
#define INPUT_DOUBLE_BASIC                                                    \
  /* Simple values. */                                                        \
  0x0000000000000000,   /* 0.0                        */                      \
  0x0010000000000000,   /* The smallest normal value. */                      \
  0x3fdfffffffffffff,   /* The value just below 0.5.  */                      \
  0x3fe0000000000000,   /* 0.5                        */                      \
  0x3fe0000000000001,   /* The value just above 0.5.  */                      \
  0x3fefffffffffffff,   /* The value just below 1.0.  */                      \
  0x3ff0000000000000,   /* 1.0                        */                      \
  0x3ff0000000000001,   /* The value just above 1.0.  */                      \
  0x3ff8000000000000,   /* 1.5                        */                      \
  0x4024000000000000,   /* 10                         */                      \
  0x7fefffffffffffff,   /* The largest finite value.  */                      \
                                                                              \
  /* Infinity. */                                                             \
  0x7ff0000000000000,                                                         \
                                                                              \
  /* NaNs. */                                                                 \
  /*  - Quiet NaNs */                                                         \
  0x7ff923456789abcd,                                                         \
  0x7ff8000000000000,                                                         \
  /*  - Signalling NaNs */                                                    \
  0x7ff123456789abcd,                                                         \
  0x7ff0000000000000,                                                         \
                                                                              \
  /* Subnormals. */                                                           \
  /*  - A recognisable bit pattern. */                                        \
  0x000123456789abcd,                                                         \
  /*  - The largest subnormal value. */                                       \
  0x000fffffffffffff,                                                         \
  /*  - The smallest subnormal value. */                                      \
  0x0000000000000001,                                                         \
                                                                              \
  /* The same values again, but negated. */                                   \
  0x8000000000000000,                                                         \
  0x8010000000000000,                                                         \
  0xbfdfffffffffffff,                                                         \
  0xbfe0000000000000,                                                         \
  0xbfe0000000000001,                                                         \
  0xbfefffffffffffff,                                                         \
  0xbff0000000000000,                                                         \
  0xbff0000000000001,                                                         \
  0xbff8000000000000,                                                         \
  0xc024000000000000,                                                         \
  0xffefffffffffffff,                                                         \
  0xfff0000000000000,                                                         \
  0xfff923456789abcd,                                                         \
  0xfff8000000000000,                                                         \
  0xfff123456789abcd,                                                         \
  0xfff0000000000000,                                                         \
  0x800123456789abcd,                                                         \
  0x800fffffffffffff,                                                         \
  0x8000000000000001,


// Extra inputs. Passing these to 3- or 2-op instructions makes the trace file
// very large, so these should only be used with 1-op instructions.
#define INPUT_DOUBLE_CONVERSIONS                                              \
  /* Values relevant for conversions to single-precision floats. */           \
  0x47efffff00000000,                                                         \
  /*  - The smallest normalized float. */                                     \
  0x3810000000000000,                                                         \
  /*  - Normal floats that need (ties-to-even) rounding.           */         \
  /*    For normalized numbers, bit 29 (0x0000000020000000) is the */         \
  /*    lowest-order bit which will fit in the float's mantissa.   */         \
  0x3ff0000000000000,                                                         \
  0x3ff0000000000001,                                                         \
  0x3ff0000010000000,                                                         \
  0x3ff0000010000001,                                                         \
  0x3ff0000020000000,                                                         \
  0x3ff0000020000001,                                                         \
  0x3ff0000030000000,                                                         \
  0x3ff0000030000001,                                                         \
  0x3ff0000040000000,                                                         \
  0x3ff0000040000001,                                                         \
  0x3ff0000050000000,                                                         \
  0x3ff0000050000001,                                                         \
  0x3ff0000060000000,                                                         \
  /*  - A mantissa that overflows into the exponent during rounding. */       \
  0x3feffffff0000000,                                                         \
  /*  - The largest double that rounds to a normal float. */                  \
  0x47efffffefffffff,                                                         \
  /*  - The smallest exponent that's too big for a float. */                  \
  0x47f0000000000000,                                                         \
  /*  - This exponent is in range, but the value rounds to infinity. */       \
  0x47effffff0000000,                                                         \
  /*  - The largest double which is too small for a subnormal float. */       \
  0x3690000000000000,                                                         \
  /*  - The largest subnormal float. */                                       \
  0x380fffffc0000000,                                                         \
  /*  - The smallest subnormal float. */                                      \
  0x36a0000000000000,                                                         \
  /*  - Subnormal floats that need (ties-to-even) rounding.      */           \
  /*    For these subnormals, bit 34 (0x0000000400000000) is the */           \
  /*    lowest-order bit which will fit in the float's mantissa. */           \
  0x37c159e000000000,                                                         \
  0x37c159e000000001,                                                         \
  0x37c159e200000000,                                                         \
  0x37c159e200000001,                                                         \
  0x37c159e400000000,                                                         \
  0x37c159e400000001,                                                         \
  0x37c159e600000000,                                                         \
  0x37c159e600000001,                                                         \
  0x37c159e800000000,                                                         \
  0x37c159e800000001,                                                         \
  0x37c159ea00000000,                                                         \
  0x37c159ea00000001,                                                         \
  0x37c159ec00000000,                                                         \
  /*  - The smallest double which rounds up to become a subnormal float. */   \
  0x3690000000000001,                                                         \
                                                                              \
  /* The same values again, but negated. */                                   \
  0xc7efffff00000000,                                                         \
  0xb810000000000000,                                                         \
  0xbff0000000000000,                                                         \
  0xbff0000000000001,                                                         \
  0xbff0000010000000,                                                         \
  0xbff0000010000001,                                                         \
  0xbff0000020000000,                                                         \
  0xbff0000020000001,                                                         \
  0xbff0000030000000,                                                         \
  0xbff0000030000001,                                                         \
  0xbff0000040000000,                                                         \
  0xbff0000040000001,                                                         \
  0xbff0000050000000,                                                         \
  0xbff0000050000001,                                                         \
  0xbff0000060000000,                                                         \
  0xbfeffffff0000000,                                                         \
  0xc7efffffefffffff,                                                         \
  0xc7f0000000000000,                                                         \
  0xc7effffff0000000,                                                         \
  0xb690000000000000,                                                         \
  0xb80fffffc0000000,                                                         \
  0xb6a0000000000000,                                                         \
  0xb7c159e000000000,                                                         \
  0xb7c159e000000001,                                                         \
  0xb7c159e200000000,                                                         \
  0xb7c159e200000001,                                                         \
  0xb7c159e400000000,                                                         \
  0xb7c159e400000001,                                                         \
  0xb7c159e600000000,                                                         \
  0xb7c159e600000001,                                                         \
  0xb7c159e800000000,                                                         \
  0xb7c159e800000001,                                                         \
  0xb7c159ea00000000,                                                         \
  0xb7c159ea00000001,                                                         \
  0xb7c159ec00000000,                                                         \
  0xb690000000000001,                                                         \
                                                                              \
  /* Values relevant for conversions to integers (frint).           */        \
  /*  - The lowest-order mantissa bit has value 1.                  */        \
  0x4330000000000000,                                                         \
  0x4330000000000001,                                                         \
  0x4330000000000002,                                                         \
  0x4330000000000003,                                                         \
  0x433fedcba9876543,                                                         \
  0x433ffffffffffffc,                                                         \
  0x433ffffffffffffd,                                                         \
  0x433ffffffffffffe,                                                         \
  0x433fffffffffffff,                                                         \
  /*  - The lowest-order mantissa bit has value 0.5.                */        \
  0x4320000000000000,                                                         \
  0x4320000000000001,                                                         \
  0x4320000000000002,                                                         \
  0x4320000000000003,                                                         \
  0x432fedcba9876543,                                                         \
  0x432ffffffffffffc,                                                         \
  0x432ffffffffffffd,                                                         \
  0x432ffffffffffffe,                                                         \
  0x432fffffffffffff,                                                         \
  /*  - The lowest-order mantissa bit has value 0.25.               */        \
  0x4310000000000000,                                                         \
  0x4310000000000001,                                                         \
  0x4310000000000002,                                                         \
  0x4310000000000003,                                                         \
  0x431fedcba9876543,                                                         \
  0x431ffffffffffffc,                                                         \
  0x431ffffffffffffd,                                                         \
  0x431ffffffffffffe,                                                         \
  0x431fffffffffffff,                                                         \
                                                                              \
  /* The same values again, but negated. */                                   \
  0xc330000000000000,                                                         \
  0xc330000000000001,                                                         \
  0xc330000000000002,                                                         \
  0xc330000000000003,                                                         \
  0xc33fedcba9876543,                                                         \
  0xc33ffffffffffffc,                                                         \
  0xc33ffffffffffffd,                                                         \
  0xc33ffffffffffffe,                                                         \
  0xc33fffffffffffff,                                                         \
  0xc320000000000000,                                                         \
  0xc320000000000001,                                                         \
  0xc320000000000002,                                                         \
  0xc320000000000003,                                                         \
  0xc32fedcba9876543,                                                         \
  0xc32ffffffffffffc,                                                         \
  0xc32ffffffffffffd,                                                         \
  0xc32ffffffffffffe,                                                         \
  0xc32fffffffffffff,                                                         \
  0xc310000000000000,                                                         \
  0xc310000000000001,                                                         \
  0xc310000000000002,                                                         \
  0xc310000000000003,                                                         \
  0xc31fedcba9876543,                                                         \
  0xc31ffffffffffffc,                                                         \
  0xc31ffffffffffffd,                                                         \
  0xc31ffffffffffffe,                                                         \
  0xc31fffffffffffff,                                                         \
                                                                              \
  /* Values relevant for conversions to integers (fcvt).    */                \
  0xc3e0000000000001,   /* The value just below INT64_MIN.          */        \
  0xc3e0000000000000,   /* INT64_MIN                                */        \
  0xc3dfffffffffffff,   /* The value just above INT64_MIN.          */        \
  0x43dfffffffffffff,   /* The value just below INT64_MAX.          */        \
                        /* INT64_MAX is not representable.          */        \
  0x43e0000000000000,   /* The value just above INT64_MAX.          */        \
                                                                              \
  0x43efffffffffffff,   /* The value just below UINT64_MAX.         */        \
                        /* UINT64_MAX is not representable.         */        \
  0x43f0000000000000,   /* The value just above UINT64_MAX.         */        \
                                                                              \
  0xc1e0000000200001,   /* The value just below INT32_MIN - 1.0.    */        \
  0xc1e0000000200000,   /* INT32_MIN - 1.0                          */        \
  0xc1e00000001fffff,   /* The value just above INT32_MIN - 1.0.    */        \
  0xc1e0000000100001,   /* The value just below INT32_MIN - 0.5.    */        \
  0xc1e0000000100000,   /* INT32_MIN - 0.5                          */        \
  0xc1e00000000fffff,   /* The value just above INT32_MIN - 0.5.    */        \
  0xc1e0000000000001,   /* The value just below INT32_MIN.          */        \
  0xc1e0000000000000,   /* INT32_MIN                                */        \
  0xc1dfffffffffffff,   /* The value just above INT32_MIN.          */        \
  0xc1dfffffffe00001,   /* The value just below INT32_MIN + 0.5.    */        \
  0xc1dfffffffe00000,   /* INT32_MIN + 0.5                          */        \
  0xc1dfffffffdfffff,   /* The value just above INT32_MIN + 0.5.    */        \
                                                                              \
  0x41dfffffff7fffff,   /* The value just below INT32_MAX - 1.0.    */        \
  0x41dfffffff800000,   /* INT32_MAX - 1.0                          */        \
  0x41dfffffff800001,   /* The value just above INT32_MAX - 1.0.    */        \
  0x41dfffffff9fffff,   /* The value just below INT32_MAX - 0.5.    */        \
  0x41dfffffffa00000,   /* INT32_MAX - 0.5                          */        \
  0x41dfffffffa00001,   /* The value just above INT32_MAX - 0.5.    */        \
  0x41dfffffffbfffff,   /* The value just below INT32_MAX.          */        \
  0x41dfffffffc00000,   /* INT32_MAX                                */        \
  0x41dfffffffc00001,   /* The value just above INT32_MAX.          */        \
  0x41dfffffffdfffff,   /* The value just below INT32_MAX + 0.5.    */        \
  0x41dfffffffe00000,   /* INT32_MAX + 0.5                          */        \
  0x41dfffffffe00001,   /* The value just above INT32_MAX + 0.5.    */        \
                                                                              \
  0x41efffffffbfffff,   /* The value just below UINT32_MAX - 1.0.   */        \
  0x41efffffffc00000,   /* UINT32_MAX - 1.0                         */        \
  0x41efffffffc00001,   /* The value just above UINT32_MAX - 1.0.   */        \
  0x41efffffffcfffff,   /* The value just below UINT32_MAX - 0.5.   */        \
  0x41efffffffd00000,   /* UINT32_MAX - 0.5                         */        \
  0x41efffffffd00001,   /* The value just above UINT32_MAX - 0.5.   */        \
  0x41efffffffdfffff,   /* The value just below UINT32_MAX.         */        \
  0x41efffffffe00000,   /* UINT32_MAX                               */        \
  0x41efffffffe00001,   /* The value just above UINT32_MAX.         */        \
  0x41efffffffefffff,   /* The value just below UINT32_MAX + 0.5.   */        \
  0x41effffffff00000,   /* UINT32_MAX + 0.5                         */        \
  0x41effffffff00001,   /* The value just above UINT32_MAX + 0.5.   */


// Float values, stored as uint32_t representations. This ensures exact bit
// representation, and avoids the loss of NaNs and suchlike through C++ casts.
#define INPUT_FLOAT_BASIC                                                     \
  /* Simple values. */                                                        \
  0x00000000,   /* 0.0                        */                              \
  0x00800000,   /* The smallest normal value. */                              \
  0x3effffff,   /* The value just below 0.5.  */                              \
  0x3f000000,   /* 0.5                        */                              \
  0x3f000001,   /* The value just above 0.5.  */                              \
  0x3f7fffff,   /* The value just below 1.0.  */                              \
  0x3f800000,   /* 1.0                        */                              \
  0x3f800001,   /* The value just above 1.0.  */                              \
  0x3fc00000,   /* 1.5                        */                              \
  0x41200000,   /* 10                         */                              \
  0x7f8fffff,   /* The largest finite value.  */                              \
                                                                              \
  /* Infinity. */                                                             \
  0x7f800000,                                                                 \
                                                                              \
  /* NaNs. */                                                                 \
  /*  - Quiet NaNs */                                                         \
  0x7fd23456,                                                                 \
  0x7fc00000,                                                                 \
  /*  - Signalling NaNs */                                                    \
  0x7f923456,                                                                 \
  0x7f800001,                                                                 \
                                                                              \
  /* Subnormals. */                                                           \
  /*  - A recognisable bit pattern. */                                        \
  0x00123456,                                                                 \
  /*  - The largest subnormal value. */                                       \
  0x007fffff,                                                                 \
  /*  - The smallest subnormal value. */                                      \
  0x00000001,                                                                 \
                                                                              \
  /* The same values again, but negated. */                                   \
  0x80000000,                                                                 \
  0x80800000,                                                                 \
  0xbeffffff,                                                                 \
  0xbf000000,                                                                 \
  0xbf000001,                                                                 \
  0xbf7fffff,                                                                 \
  0xbf800000,                                                                 \
  0xbf800001,                                                                 \
  0xbfc00000,                                                                 \
  0xc1200000,                                                                 \
  0xff8fffff,                                                                 \
  0xff800000,                                                                 \
  0xffd23456,                                                                 \
  0xffc00000,                                                                 \
  0xff923456,                                                                 \
  0xff800001,                                                                 \
  0x80123456,                                                                 \
  0x807fffff,                                                                 \
  0x80000001,


// Extra inputs. Passing these to 3- or 2-op instructions makes the trace file
// very large, so these should only be used with 1-op instructions.
#define INPUT_FLOAT_CONVERSIONS                                               \
  /* Values relevant for conversions to integers (frint).           */        \
  /*  - The lowest-order mantissa bit has value 1.                  */        \
  0x4b000000,                                                                 \
  0x4b000001,                                                                 \
  0x4b000002,                                                                 \
  0x4b000003,                                                                 \
  0x4b765432,                                                                 \
  0x4b7ffffc,                                                                 \
  0x4b7ffffd,                                                                 \
  0x4b7ffffe,                                                                 \
  0x4b7fffff,                                                                 \
  /*  - The lowest-order mantissa bit has value 0.5.                */        \
  0x4a800000,                                                                 \
  0x4a800001,                                                                 \
  0x4a800002,                                                                 \
  0x4a800003,                                                                 \
  0x4af65432,                                                                 \
  0x4afffffc,                                                                 \
  0x4afffffd,                                                                 \
  0x4afffffe,                                                                 \
  0x4affffff,                                                                 \
  /*  - The lowest-order mantissa bit has value 0.25.               */        \
  0x4a000000,                                                                 \
  0x4a000001,                                                                 \
  0x4a000002,                                                                 \
  0x4a000003,                                                                 \
  0x4a765432,                                                                 \
  0x4a7ffffc,                                                                 \
  0x4a7ffffd,                                                                 \
  0x4a7ffffe,                                                                 \
  0x4a7fffff,                                                                 \
                                                                              \
  /* The same values again, but negated. */                                   \
  0xcb000000,                                                                 \
  0xcb000001,                                                                 \
  0xcb000002,                                                                 \
  0xcb000003,                                                                 \
  0xcb765432,                                                                 \
  0xcb7ffffc,                                                                 \
  0xcb7ffffd,                                                                 \
  0xcb7ffffe,                                                                 \
  0xcb7fffff,                                                                 \
  0xca800000,                                                                 \
  0xca800001,                                                                 \
  0xca800002,                                                                 \
  0xca800003,                                                                 \
  0xcaf65432,                                                                 \
  0xcafffffc,                                                                 \
  0xcafffffd,                                                                 \
  0xcafffffe,                                                                 \
  0xcaffffff,                                                                 \
  0xca000000,                                                                 \
  0xca000001,                                                                 \
  0xca000002,                                                                 \
  0xca000003,                                                                 \
  0xca765432,                                                                 \
  0xca7ffffc,                                                                 \
  0xca7ffffd,                                                                 \
  0xca7ffffe,                                                                 \
  0xca7fffff,                                                                 \
                                                                              \
  /* Values relevant for conversions to integers (fcvt).            */        \
  0xdf000001,   /* The value just below INT64_MIN.                  */        \
  0xdf000000,   /* INT64_MIN                                        */        \
  0xdeffffff,   /* The value just above INT64_MIN.                  */        \
  0x5effffff,   /* The value just below INT64_MAX.                  */        \
                /* INT64_MAX is not representable.                  */        \
  0x5f000000,   /* The value just above INT64_MAX.                  */        \
                                                                              \
  0x5f7fffff,   /* The value just below UINT64_MAX.                 */        \
                /* UINT64_MAX is not representable.                 */        \
  0x5f800000,   /* The value just above UINT64_MAX.                 */        \
                                                                              \
  0xcf000001,   /* The value just below INT32_MIN.                  */        \
  0xcf000000,   /* INT32_MIN                                        */        \
  0xceffffff,   /* The value just above INT32_MIN.                  */        \
  0x4effffff,   /* The value just below INT32_MAX.                  */        \
                /* INT32_MAX is not representable.                  */        \
  0x4f000000,   /* The value just above INT32_MAX.                  */


// For most 2- and 3-op instructions, use only basic inputs. Because every
// combination is tested, the length of the output trace is very sensitive to
// the length of this list.
static const uint64_t kInputDoubleBasic[] = { INPUT_DOUBLE_BASIC };
static const uint32_t kInputFloatBasic[] = { INPUT_FLOAT_BASIC };

// For conversions, include several extra inputs.
static const uint64_t kInputDoubleConversions[] = {
  INPUT_DOUBLE_BASIC
  INPUT_DOUBLE_CONVERSIONS
};
static const uint32_t kInputFloatConversions[] = {
  INPUT_FLOAT_BASIC
  INPUT_FLOAT_CONVERSIONS
};

