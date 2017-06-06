// Copyright 2016, VIXL authors
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

#include "test-runner.h"

#include "utils-vixl.h"

#define TEST(name) TEST_(API_##name)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

namespace vixl {

// Describe the result of a test. Should IsUintN() and IsIntN() return true or
// false for N and X?
template <typename T>
struct UintIntTest {
  bool is_uintn;
  bool is_intn;
  unsigned n;
  T x;
};

// Test IsUintN() and IsIntN() against various values and integral types.
TEST(IsUint_IsInt) {
  UintIntTest<uint32_t> test_little_values_unsigned[] = {
      {true, true, 1, UINT32_C(0x0)},   {true, false, 1, UINT32_C(0x1)},
      {false, false, 1, UINT32_C(0x2)}, {false, false, 1, UINT32_C(0x3)},
      {false, false, 1, UINT32_C(0x4)}, {false, false, 1, UINT32_C(0x5)},
      {false, false, 1, UINT32_C(0x6)}, {false, false, 1, UINT32_C(0x7)},
      {false, false, 1, UINT32_C(0x8)}, {false, false, 1, UINT32_C(0x9)},
      {false, false, 1, UINT32_C(0xa)}, {false, false, 1, UINT32_C(0xb)},
      {false, false, 1, UINT32_C(0xc)}, {false, false, 1, UINT32_C(0xd)},
      {false, false, 1, UINT32_C(0xe)}, {false, false, 1, UINT32_C(0xf)},

      {true, true, 2, UINT32_C(0x0)},   {true, true, 2, UINT32_C(0x1)},
      {true, false, 2, UINT32_C(0x2)},  {true, false, 2, UINT32_C(0x3)},
      {false, false, 2, UINT32_C(0x4)}, {false, false, 2, UINT32_C(0x5)},
      {false, false, 2, UINT32_C(0x6)}, {false, false, 2, UINT32_C(0x7)},
      {false, false, 2, UINT32_C(0x8)}, {false, false, 2, UINT32_C(0x9)},
      {false, false, 2, UINT32_C(0xa)}, {false, false, 2, UINT32_C(0xb)},
      {false, false, 2, UINT32_C(0xc)}, {false, false, 2, UINT32_C(0xd)},
      {false, false, 2, UINT32_C(0xe)}, {false, false, 2, UINT32_C(0xf)},
  };

  UintIntTest<int32_t> test_little_values_signed[] = {
      {true, true, 1, INT32_C(0)},    {true, false, 1, INT32_C(1)},
      {false, false, 1, INT32_C(2)},  {false, false, 1, INT32_C(3)},
      {false, false, 1, INT32_C(4)},  {false, false, 1, INT32_C(5)},
      {false, false, 1, INT32_C(6)},  {false, false, 1, INT32_C(7)},
      {false, true, 1, INT32_C(-1)},  {false, false, 1, INT32_C(-2)},
      {false, false, 1, INT32_C(-3)}, {false, false, 1, INT32_C(-4)},
      {false, false, 1, INT32_C(-5)}, {false, false, 1, INT32_C(-6)},
      {false, false, 1, INT32_C(-7)}, {false, false, 1, INT32_C(-8)},

      {true, true, 2, INT32_C(0)},    {true, true, 2, INT32_C(1)},
      {true, false, 2, INT32_C(2)},   {true, false, 2, INT32_C(3)},
      {false, false, 2, INT32_C(4)},  {false, false, 2, INT32_C(5)},
      {false, false, 2, INT32_C(6)},  {false, false, 2, INT32_C(7)},
      {false, true, 2, INT32_C(-1)},  {false, true, 2, INT32_C(-2)},
      {false, false, 2, INT32_C(-3)}, {false, false, 2, INT32_C(-4)},
      {false, false, 2, INT32_C(-5)}, {false, false, 2, INT32_C(-6)},
      {false, false, 2, INT32_C(-7)}, {false, false, 2, INT32_C(-8)},
  };

  UintIntTest<uint32_t> test_u16[] = {
      {true, true, 16, UINT32_C(0x0)},
      {true, false, 16, UINT32_C(0xabcd)},
      {true, false, 16, UINT32_C(0x8000)},
      {true, false, 16, UINT32_C(0xffff)},
      {false, false, 16, UINT32_C(0x10000)},
      {false, false, 16, UINT32_C(0xffff0000)},
      {false, false, 16, UINT32_C(0xffff8000)},
      {false, false, 16, UINT32_C(0xffffffff)},
  };

  UintIntTest<int32_t> test_i16[] = {
      {true, true, 16, INT32_C(0x0)},
      {true, false, 16, INT32_C(0xabcd)},
      {true, false, 16, INT32_C(0x8000)},
      {true, false, 16, INT32_C(0xffff)},
      {false, false, 16, INT32_C(0x10000)},
      {true, true, 16, INT32_C(42)},
      {false, true, 16, INT32_C(-42)},
      {false, true, 16, INT32_C(-1)},
  };

  UintIntTest<uint64_t> test_u32[] = {
      {true, true, 32, UINT64_C(0x0)},
      {true, false, 32, UINT64_C(0xabcdabcd)},
      {true, false, 32, UINT64_C(0x80000000)},
      {true, false, 32, UINT64_C(0xffffffff)},
  };

  UintIntTest<int64_t> test_i32[] = {
      {true, true, 32, INT64_C(0)},
      {true, true, 32, INT64_C(42)},
      {false, true, 32, INT64_C(-42)},
      {false, true, 32, INT64_C(-1)},
      {true, true, 32, INT64_C(2147483647)},    // (1 << (32 - 1)) - 1
      {false, true, 32, INT64_C(-2147483648)},  // -(1 << (32 - 1))
  };

  UintIntTest<uint64_t> test_unsigned_higher_than_32[] = {
      {false, false, 54, UINT64_C(0xabcdef9012345678)},
      {true, false, 33, UINT64_C(0x100000000)},
      {true, false, 62, UINT64_C(0x3fffffffffffffff)},
      {true, false, 63, UINT64_C(0x7fffffffffffffff)},
  };

  UintIntTest<int64_t> test_signed_higher_than_32[] = {
      {true, true, 54, INT64_C(9007199254740991)},   // (1 << (54 - 1)) - 1
      {true, false, 54, INT64_C(9007199254740992)},  // 1 << (54 - 1)
      {true, true, 33, INT64_C(4294967295)},         // (1 << (33 - 1) - 1)
      {false, true, 33, INT64_C(-4294967296)},       // -(1 << (33 - 1))
  };

#define TEST_LIST(M)              \
  M(test_little_values_unsigned)  \
  M(test_little_values_signed)    \
  M(test_u16)                     \
  M(test_i16)                     \
  M(test_u32)                     \
  M(test_i32)                     \
  M(test_unsigned_higher_than_32) \
  M(test_signed_higher_than_32)


#define TEST_UINT(test_vector)                                  \
  for (unsigned i = 0; i < ARRAY_SIZE(test_vector); i++) {      \
    if (test_vector[i].is_uintn) {                              \
      VIXL_CHECK(IsUintN(test_vector[i].n, test_vector[i].x));  \
    } else {                                                    \
      VIXL_CHECK(!IsUintN(test_vector[i].n, test_vector[i].x)); \
    }                                                           \
  }

#define TEST_INT(test_vector)                                  \
  for (unsigned i = 0; i < ARRAY_SIZE(test_vector); i++) {     \
    if (test_vector[i].is_intn) {                              \
      VIXL_CHECK(IsIntN(test_vector[i].n, test_vector[i].x));  \
    } else {                                                   \
      VIXL_CHECK(!IsIntN(test_vector[i].n, test_vector[i].x)); \
    }                                                          \
  }

  TEST_LIST(TEST_UINT)
  TEST_LIST(TEST_INT)

#undef TEST_UINT
#undef TEST_INT

#undef TEST_LIST
}

}  // namespace vixl
