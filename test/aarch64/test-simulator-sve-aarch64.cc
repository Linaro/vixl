// Copyright 2021, VIXL authors
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
#include "test-utils.h"

#include "aarch64/cpu-aarch64.h"
#include "aarch64/disasm-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"
#include "aarch64/simulator-aarch64.h"
#include "aarch64/test-utils-aarch64.h"
#include "test-assembler-aarch64.h"

#define TEST_SVE(name) TEST_SVE_INNER("SIM", name)

namespace vixl {
namespace aarch64 {

TEST_SVE(sve_matmul) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVEI8MM,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x45179979);  // smmla z25.s, z11.b, z23.b
    // vl128 state = 0xf1ca8a4d
    __ dci(0x45179b51);  // smmla z17.s, z26.b, z23.b
    // vl128 state = 0x4458ad10
    __ dci(0x45d79b53);  // ummla z19.s, z26.b, z23.b
    // vl128 state = 0x43d4d064
    __ dci(0x45d69b17);  // ummla z23.s, z24.b, z22.b
    // vl128 state = 0x601e77c8
    __ dci(0x45c69b33);  // ummla z19.s, z25.b, z6.b
    // vl128 state = 0x561b4e22
    __ dci(0x45c49b1b);  // ummla z27.s, z24.b, z4.b
    // vl128 state = 0x89b65d78
    __ dci(0x45dc9b1a);  // ummla z26.s, z24.b, z28.b
    // vl128 state = 0x85c9e62d
    __ dci(0x45d99b1b);  // ummla z27.s, z24.b, z25.b
    // vl128 state = 0x3fc74134
    __ dci(0x45d99b19);  // ummla z25.s, z24.b, z25.b
    // vl128 state = 0xa2fa347b
    __ dci(0x45d99b1b);  // ummla z27.s, z24.b, z25.b
    // vl128 state = 0xb9854782
    __ dci(0x45899b1a);  // usmmla z26.s, z24.b, z9.b
    // vl128 state = 0x7fd376d8
    __ dci(0x45099b8a);  // smmla z10.s, z28.b, z9.b
    // vl128 state = 0xb41d8433
    __ dci(0x45019bcb);  // smmla z11.s, z30.b, z1.b
    // vl128 state = 0xc9c0e80d
    __ dci(0x45019bdb);  // smmla z27.s, z30.b, z1.b
    // vl128 state = 0xf1130e02
    __ dci(0x45019b6b);  // smmla z11.s, z27.b, z1.b
    // vl128 state = 0x282d3dc7
    __ dci(0x45019b6f);  // smmla z15.s, z27.b, z1.b
    // vl128 state = 0x34570238
    __ dci(0x45859b6b);  // usmmla z11.s, z27.b, z5.b
    // vl128 state = 0xc451206a
    __ dci(0x45919b6a);  // usmmla z10.s, z27.b, z17.b
    // vl128 state = 0xa58e2ea8
    __ dci(0x45909a62);  // usmmla z2.s, z19.b, z16.b
    // vl128 state = 0x7b5f948d
    __ dci(0x45809a52);  // usmmla z18.s, z18.b, z0.b
    // vl128 state = 0xf746260d
    __ dci(0x45889b53);  // usmmla z19.s, z26.b, z8.b
    // vl128 state = 0xc31cc539
    __ dci(0x45809a57);  // usmmla z23.s, z18.b, z0.b
    // vl128 state = 0x736bb3ee
    __ dci(0x45809a96);  // usmmla z22.s, z20.b, z0.b
    // vl128 state = 0xbb05fef6
    __ dci(0x45809a92);  // usmmla z18.s, z20.b, z0.b
    // vl128 state = 0xbc594372
    __ dci(0x45809a82);  // usmmla z2.s, z20.b, z0.b
    // vl128 state = 0x87c5a584
    __ dci(0x45829ad2);  // usmmla z18.s, z22.b, z2.b
    // vl128 state = 0xa413f733
    __ dci(0x45889ad6);  // usmmla z22.s, z22.b, z8.b
    // vl128 state = 0x87ec445d
    __ dci(0x45c898d2);  // ummla z18.s, z6.b, z8.b
    // vl128 state = 0x3ca8a6e5
    __ dci(0x450898d0);  // smmla z16.s, z6.b, z8.b
    // vl128 state = 0x4300d87b
    __ dci(0x45189ad8);  // smmla z24.s, z22.b, z24.b
    // vl128 state = 0x38be2e8a
    __ dci(0x451c9bd9);  // smmla z25.s, z30.b, z28.b
    // vl128 state = 0x8a3e6103
    __ dci(0x45989bc9);  // usmmla z9.s, z30.b, z24.b
    // vl128 state = 0xc728e586
    __ dci(0x451c9bd9);  // smmla z25.s, z30.b, z28.b
    // vl128 state = 0x4cb44c0e
    __ dci(0x459c99d1);  // usmmla z17.s, z14.b, z28.b
    // vl128 state = 0x84ebcb36
    __ dci(0x459c99d5);  // usmmla z21.s, z14.b, z28.b
    // vl128 state = 0x8813d2e2
    __ dci(0x451c999d);  // smmla z29.s, z12.b, z28.b
    // vl128 state = 0x8f26ee51
    __ dci(0x451c999f);  // smmla z31.s, z12.b, z28.b
    // vl128 state = 0x5d626fd0
    __ dci(0x459e998f);  // usmmla z15.s, z12.b, z30.b
    // vl128 state = 0x6b64cc8f
    __ dci(0x459f991f);  // usmmla z31.s, z8.b, z31.b
    // vl128 state = 0x41648186
    __ dci(0x4587991e);  // usmmla z30.s, z8.b, z7.b
    // vl128 state = 0x701525ec
    __ dci(0x45079816);  // smmla z22.s, z0.b, z7.b
    // vl128 state = 0x61a2d024
    __ dci(0x450f9897);  // smmla z23.s, z4.b, z15.b
    // vl128 state = 0x82ba6bd5
    __ dci(0x450b98d3);  // smmla z19.s, z6.b, z11.b
    // vl128 state = 0xa842bbde
    __ dci(0x450b98db);  // smmla z27.s, z6.b, z11.b
    // vl128 state = 0x9977677a
    __ dci(0x451f98d3);  // smmla z19.s, z6.b, z31.b
    // vl128 state = 0xe6d6c2ef
    __ dci(0x451b9adb);  // smmla z27.s, z22.b, z27.b
    // vl128 state = 0xa535453f
    __ dci(0x450b98d9);  // smmla z25.s, z6.b, z11.b
    // vl128 state = 0xeda3f381
    __ dci(0x458b9adb);  // usmmla z27.s, z22.b, z11.b
    // vl128 state = 0xd72dbdef
    __ dci(0x45cb98da);  // ummla z26.s, z6.b, z11.b
    // vl128 state = 0xfae4975b
    __ dci(0x45c999d2);  // ummla z18.s, z14.b, z9.b
    // vl128 state = 0x0aa6e1f6
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x0aa6e1f6,
        0xba2d4547,
        0x0e72a647,
        0x15b8fc1b,
        0x92eddc98,
        0xe0c72bcf,
        0x36b4e3ba,
        0x1041114e,
        0x4d44ebd4,
        0xfe0e3cbf,
        0x81c43455,
        0x678617c5,
        0xf72fac1f,
        0xabdcd4e4,
        0x108864bd,
        0x035f6eca,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve_fmatmul_s) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVEF32MM,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 20 * kInstructionSize);
    __ dci(0x64a1e6ee);  // fmmla z14.s, z23.s, z1.s
    // vl128 state = 0x9db41bef
    __ dci(0x64b1e7fe);  // fmmla z30.s, z31.s, z17.s
    // vl128 state = 0xc1535e55
    __ dci(0x64b9e7d6);  // fmmla z22.s, z30.s, z25.s
    // vl128 state = 0xc65aad35
    __ dci(0x64bde6c6);  // fmmla z6.s, z22.s, z29.s
    // vl128 state = 0x68387c22
    __ dci(0x64b9e4c2);  // fmmla z2.s, z6.s, z25.s
    // vl128 state = 0xcf08b3a4
    __ dci(0x64b9e543);  // fmmla z3.s, z10.s, z25.s
    // vl128 state = 0x969bbe77
    __ dci(0x64b9e553);  // fmmla z19.s, z10.s, z25.s
    // vl128 state = 0xc3f514e1
    __ dci(0x64b9e557);  // fmmla z23.s, z10.s, z25.s
    // vl128 state = 0x4b351c29
    __ dci(0x64b9e773);  // fmmla z19.s, z27.s, z25.s
    // vl128 state = 0x5e026315
    __ dci(0x64bbe757);  // fmmla z23.s, z26.s, z27.s
    // vl128 state = 0x61684fe6
    __ dci(0x64bbe755);  // fmmla z21.s, z26.s, z27.s
    // vl128 state = 0x719b4ce0
    __ dci(0x64bfe554);  // fmmla z20.s, z10.s, z31.s
    // vl128 state = 0xdf3d2a1c
    __ dci(0x64bfe550);  // fmmla z16.s, z10.s, z31.s
    // vl128 state = 0x3279aab8
    __ dci(0x64bfe714);  // fmmla z20.s, z24.s, z31.s
    // vl128 state = 0x0b985869
    __ dci(0x64b7e756);  // fmmla z22.s, z26.s, z23.s
    // vl128 state = 0x14230587
    __ dci(0x64b7e737);  // fmmla z23.s, z25.s, z23.s
    // vl128 state = 0x2cb88e7f
    __ dci(0x64bfe767);  // fmmla z7.s, z27.s, z31.s
    // vl128 state = 0xb5ec0c65
    __ dci(0x64bfe777);  // fmmla z23.s, z27.s, z31.s
    // vl128 state = 0xb5e5eab0
    __ dci(0x64bfe715);  // fmmla z21.s, z24.s, z31.s
    // vl128 state = 0xd0491fb5
    __ dci(0x64b7e797);  // fmmla z23.s, z28.s, z23.s
    // vl128 state = 0x98a55a30
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x98a55a30,
        0x590b7715,
        0x4562ccf3,
        0x1f8653a6,
        0x5fe174d5,
        0xb300dcb8,
        0x3cefa79e,
        0xa22484c7,
        0x380697ec,
        0xde9e699b,
        0x99d21870,
        0x456cb46b,
        0x207d2615,
        0xecaf9678,
        0x0949e2d2,
        0xa764c43f,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

}  // namespace aarch64
}  // namespace vixl
