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

// Below here, there are tests for Neon instructions. As these forms of test
// check the entire register state, they also need SVE features.

TEST_SVE(neon_pmull) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kPmull1Q);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 40 * kInstructionSize);
    __ dci(0x4e20e000);  // pmull2 v0.8h, v0.16b, v0.16b
    // vl128 state = 0x5eba4d4f
    __ dci(0x4e20e228);  // pmull2 v8.8h, v17.16b, v0.16b
    // vl128 state = 0x86bceb87
    __ dci(0x4ee0e22a);  // pmull2 v10.1q, v17.2d, v0.2d
    // vl128 state = 0x1332fe02
    __ dci(0x0ee8e222);  // pmull v2.1q, v17.1d, v8.1d
    // vl128 state = 0xd357dc7b
    __ dci(0x4eece226);  // pmull2 v6.1q, v17.2d, v12.2d
    // vl128 state = 0xdff409ad
    __ dci(0x0eece276);  // pmull v22.1q, v19.1d, v12.1d
    // vl128 state = 0xd8af1dc6
    __ dci(0x0eede232);  // pmull v18.1q, v17.1d, v13.1d
    // vl128 state = 0x41e6ed0e
    __ dci(0x0efde216);  // pmull v22.1q, v16.1d, v29.1d
    // vl128 state = 0x1f10365f
    __ dci(0x0effe23e);  // pmull v30.1q, v17.1d, v31.1d
    // vl128 state = 0x9779ece5
    __ dci(0x0ee7e23f);  // pmull v31.1q, v17.1d, v7.1d
    // vl128 state = 0x11fc8ce9
    __ dci(0x0ee2e23e);  // pmull v30.1q, v17.1d, v2.1d
    // vl128 state = 0x101d5a6f
    __ dci(0x0ee2e23c);  // pmull v28.1q, v17.1d, v2.1d
    // vl128 state = 0xcc4fe26e
    __ dci(0x0eeae27d);  // pmull v29.1q, v19.1d, v10.1d
    // vl128 state = 0xc84be9f4
    __ dci(0x4eeae24d);  // pmull2 v13.1q, v18.2d, v10.2d
    // vl128 state = 0x2fc540b4
    __ dci(0x4eeae25d);  // pmull2 v29.1q, v18.2d, v10.2d
    // vl128 state = 0x1b2d99cd
    __ dci(0x4eeae2ed);  // pmull2 v13.1q, v23.2d, v10.2d
    // vl128 state = 0x8a278b95
    __ dci(0x4eeae2e9);  // pmull2 v9.1q, v23.2d, v10.2d
    // vl128 state = 0x3359b4c8
    __ dci(0x4efee2e8);  // pmull2 v8.1q, v23.2d, v30.2d
    // vl128 state = 0x5c25ed31
    __ dci(0x4effe3e0);  // pmull2 v0.1q, v31.2d, v31.2d
    // vl128 state = 0x28ff67d1
    __ dci(0x4eefe3d0);  // pmull2 v16.1q, v30.2d, v15.2d
    // vl128 state = 0x1543436d
    __ dci(0x4ee7e2d1);  // pmull2 v17.1q, v22.2d, v7.2d
    // vl128 state = 0x71b8bc90
    __ dci(0x4eefe3d5);  // pmull2 v21.1q, v30.2d, v15.2d
    // vl128 state = 0x3d35ca02
    __ dci(0x4eefe314);  // pmull2 v20.1q, v24.2d, v15.2d
    // vl128 state = 0x40e8fade
    __ dci(0x4eefe310);  // pmull2 v16.1q, v24.2d, v15.2d
    // vl128 state = 0xb8affb87
    __ dci(0x4eefe300);  // pmull2 v0.1q, v24.2d, v15.2d
    // vl128 state = 0x4824ee5c
    __ dci(0x4eede350);  // pmull2 v16.1q, v26.2d, v13.2d
    // vl128 state = 0x39202868
    __ dci(0x4ee7e354);  // pmull2 v20.1q, v26.2d, v7.2d
    // vl128 state = 0xc8fde340
    __ dci(0x4e27e356);  // pmull2 v22.8h, v26.16b, v7.16b
    // vl128 state = 0x0f02316b
    __ dci(0x4e37e15e);  // pmull2 v30.8h, v10.16b, v23.16b
    // vl128 state = 0xced4f8bd
    __ dci(0x4e33e05f);  // pmull2 v31.8h, v2.16b, v19.16b
    // vl128 state = 0x0c76bdb3
    __ dci(0x0e23e05e);  // pmull v30.8h, v2.8b, v3.8b
    // vl128 state = 0x0e36962b
    __ dci(0x4e23e25f);  // pmull2 v31.8h, v18.16b, v3.16b
    // vl128 state = 0x11a8dcc3
    __ dci(0x4e23e25b);  // pmull2 v27.8h, v18.16b, v3.16b
    // vl128 state = 0xf01bfe16
    __ dci(0x4e23e259);  // pmull2 v25.8h, v18.16b, v3.16b
    // vl128 state = 0xea351afe
    __ dci(0x4e22e2c9);  // pmull2 v9.8h, v22.16b, v2.16b
    // vl128 state = 0x16e933ef
    __ dci(0x4e3ae2c8);  // pmull2 v8.8h, v22.16b, v26.16b
    // vl128 state = 0x02528a2a
    __ dci(0x4e32e249);  // pmull2 v9.8h, v18.16b, v18.16b
    // vl128 state = 0xe7e20633
    __ dci(0x4e36e20d);  // pmull2 v13.8h, v16.16b, v22.16b
    // vl128 state = 0x6f231732
    __ dci(0x4e36e205);  // pmull2 v5.8h, v16.16b, v22.16b
    // vl128 state = 0x423eb7ea
    __ dci(0x4e22e20d);  // pmull2 v13.8h, v16.16b, v2.16b
    // vl128 state = 0xfc0d1c14
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xfc0d1c14,
        0x4cb040a3,
        0x4b913ebe,
        0xfa35b836,
        0x78745d20,
        0x6666b09a,
        0xee2868f4,
        0x1936a795,
        0x1025244a,
        0xe8551950,
        0xae73af02,
        0x0fdd5fc7,
        0x22e9827b,
        0x384ce1ac,
        0xc833cbeb,
        0x255baab5,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(neon_sha1_2reg) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kSHA1);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0x5e280800);  // sha1h s0, s0
    // vl128 state = 0xc388d4f8
    __ dci(0x5e280a28);  // sha1h s8, s17
    // vl128 state = 0x5c88b904
    __ dci(0x5e280a2a);  // sha1h s10, s17
    // vl128 state = 0x6f63c596
    __ dci(0x5e281aae);  // sha1su1 v14.4s, v21.4s
    // vl128 state = 0x85e1119d
    __ dci(0x5e281abe);  // sha1su1 v30.4s, v21.4s
    // vl128 state = 0x9b814260
    __ dci(0x5e281a0e);  // sha1su1 v14.4s, v16.4s
    // vl128 state = 0x8ccca0ab
    __ dci(0x5e281a0a);  // sha1su1 v10.4s, v16.4s
    // vl128 state = 0x42262836
    __ dci(0x5e281acb);  // sha1su1 v11.4s, v22.4s
    // vl128 state = 0xabcde33d
    __ dci(0x5e281acf);  // sha1su1 v15.4s, v22.4s
    // vl128 state = 0xdf44e7be
    __ dci(0x5e281adf);  // sha1su1 v31.4s, v22.4s
    // vl128 state = 0x48c332a3
    __ dci(0x5e280a9d);  // sha1h s29, s20
    // vl128 state = 0x56bafe13
    __ dci(0x5e28188d);  // sha1su1 v13.4s, v4.4s
    // vl128 state = 0x218eb351
    __ dci(0x5e2808cf);  // sha1h s15, s6
    // vl128 state = 0xc1720d9f
    __ dci(0x5e2808cb);  // sha1h s11, s6
    // vl128 state = 0x67119e1c
    __ dci(0x5e2808c9);  // sha1h s9, s6
    // vl128 state = 0x31f69637
    __ dci(0x5e2808c1);  // sha1h s1, s6
    // vl128 state = 0x214a25ff
    __ dci(0x5e280871);  // sha1h s17, s3
    // vl128 state = 0xa5e88b55
    __ dci(0x5e280815);  // sha1h s21, s0
    // vl128 state = 0xc8c91e29
    __ dci(0x5e28185d);  // sha1su1 v29.4s, v2.4s
    // vl128 state = 0x5582c6a8
    __ dci(0x5e28185f);  // sha1su1 v31.4s, v2.4s
    // vl128 state = 0xd3288a61
    __ dci(0x5e28087e);  // sha1h s30, s3
    // vl128 state = 0x350b39c2
    __ dci(0x5e28093f);  // sha1h s31, s9
    // vl128 state = 0xbdc1ac98
    __ dci(0x5e28093b);  // sha1h s27, s9
    // vl128 state = 0x62f828bf
    __ dci(0x5e28092b);  // sha1h s11, s9
    // vl128 state = 0xc8f2f671
    __ dci(0x5e2819bb);  // sha1su1 v27.4s, v13.4s
    // vl128 state = 0x24ec8c34
    __ dci(0x5e281b93);  // sha1su1 v19.4s, v28.4s
    // vl128 state = 0x71e188de
    __ dci(0x5e281b97);  // sha1su1 v23.4s, v28.4s
    // vl128 state = 0x22490375
    __ dci(0x5e281b95);  // sha1su1 v21.4s, v28.4s
    // vl128 state = 0x016b70d1
    __ dci(0x5e281b51);  // sha1su1 v17.4s, v26.4s
    // vl128 state = 0xa6252086
    __ dci(0x5e2819d3);  // sha1su1 v19.4s, v14.4s
    // vl128 state = 0x78683885
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x78683885,
        0x59574c2a,
        0x449978bf,
        0x0ddab775,
        0x1a043ef3,
        0xf501e2e7,
        0xa219e725,
        0xf17f57c8,
        0x4ccdbf99,
        0x419d4fc3,
        0x7302571d,
        0xd6bee170,
        0x7d81c301,
        0xbaa7d729,
        0xf33f0bc4,
        0xff8b070a,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(neon_sha1_3reg) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kSHA1);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0x5e1f02bd);  // sha1c q29, s21, v31.4s
    // vl128 state = 0xec2a37ad
    __ dci(0x5e0810af);  // sha1p q15, s5, v8.4s
    // vl128 state = 0x3fe9252a
    __ dci(0x5e122227);  // sha1m q7, s17, v18.4s
    // vl128 state = 0x4465789e
    __ dci(0x5e0b039d);  // sha1c q29, s28, v11.4s
    // vl128 state = 0x2186488a
    __ dci(0x5e1a03e9);  // sha1c q9, s31, v26.4s
    // vl128 state = 0x9eddf8e3
    __ dci(0x5e0c138c);  // sha1p q12, s28, v12.4s
    // vl128 state = 0x0ca7cd3d
    __ dci(0x5e1f1316);  // sha1p q22, s24, v31.4s
    // vl128 state = 0xb80a61c0
    __ dci(0x5e052204);  // sha1m q4, s16, v5.4s
    // vl128 state = 0x941821ca
    __ dci(0x5e0a00d6);  // sha1c q22, s6, v10.4s
    // vl128 state = 0x5e71ccae
    __ dci(0x5e0e032e);  // sha1c q14, s25, v14.4s
    // vl128 state = 0x7ed4486a
    __ dci(0x5e1d1098);  // sha1p q24, s4, v29.4s
    // vl128 state = 0x0978a637
    __ dci(0x5e0400d9);  // sha1c q25, s6, v4.4s
    // vl128 state = 0x34c8609e
    __ dci(0x5e1a330e);  // sha1su0 v14.4s, v24.4s, v26.4s
    // vl128 state = 0xcb078fad
    __ dci(0x5e1e30f5);  // sha1su0 v21.4s, v7.4s, v30.4s
    // vl128 state = 0x885200be
    __ dci(0x5e1e32e1);  // sha1su0 v1.4s, v23.4s, v30.4s
    // vl128 state = 0xabc6a188
    __ dci(0x5e0733d3);  // sha1su0 v19.4s, v30.4s, v7.4s
    // vl128 state = 0x37a4fe6f
    __ dci(0x5e0b22e6);  // sha1m q6, s23, v11.4s
    // vl128 state = 0x68b788d2
    __ dci(0x5e011210);  // sha1p q16, s16, v1.4s
    // vl128 state = 0x6b36b092
    __ dci(0x5e1702e1);  // sha1c q1, s23, v23.4s
    // vl128 state = 0x74ef56f5
    __ dci(0x5e1e30f6);  // sha1su0 v22.4s, v7.4s, v30.4s
    // vl128 state = 0x5a150dfd
    __ dci(0x5e1b3348);  // sha1su0 v8.4s, v26.4s, v27.4s
    // vl128 state = 0xe0a45d9c
    __ dci(0x5e0a3041);  // sha1su0 v1.4s, v2.4s, v10.4s
    // vl128 state = 0x6ba02d02
    __ dci(0x5e17119a);  // sha1p q26, s12, v23.4s
    // vl128 state = 0x3bf511fc
    __ dci(0x5e0b32c7);  // sha1su0 v7.4s, v22.4s, v11.4s
    // vl128 state = 0xf5c513b6
    __ dci(0x5e063016);  // sha1su0 v22.4s, v0.4s, v6.4s
    // vl128 state = 0x3eb44b28
    __ dci(0x5e05323c);  // sha1su0 v28.4s, v17.4s, v5.4s
    // vl128 state = 0x7c2d3adf
    __ dci(0x5e1d132a);  // sha1p q10, s25, v29.4s
    // vl128 state = 0x2b0963c4
    __ dci(0x5e13003c);  // sha1c q28, s1, v19.4s
    // vl128 state = 0x4a582d00
    __ dci(0x5e13322c);  // sha1su0 v12.4s, v17.4s, v19.4s
    // vl128 state = 0x7bb2cc8c
    __ dci(0x5e032330);  // sha1m q16, s25, v3.4s
    // vl128 state = 0x2a8b4c0d
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x2a8b4c0d,
        0x114e25bb,
        0x4f035af9,
        0x23db7966,
        0x3d106b42,
        0x62651fcf,
        0x44c20879,
        0xadf71d73,
        0xe6858f82,
        0x93a74ae5,
        0xc270310e,
        0x3d07058c,
        0x69f83d0e,
        0x28c5813b,
        0xbb9de2c1,
        0xe06b94cd,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(neon_sha3) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kSHA3);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 60 * kInstructionSize);
    __ dci(0xce608c00);  // rax1 v0.2d, v0.2d, v0.2d
    // vl128 state = 0x960c2b9f
    __ dci(0xce608e28);  // rax1 v8.2d, v17.2d, v0.2d
    // vl128 state = 0x89ea3f7b
    __ dci(0xce618e6c);  // rax1 v12.2d, v19.2d, v1.2d
    // vl128 state = 0xa7801384
    __ dci(0xce718e48);  // rax1 v8.2d, v18.2d, v17.2d
    // vl128 state = 0x4477d70d
    __ dci(0xce738e60);  // rax1 v0.2d, v19.2d, v19.2d
    // vl128 state = 0xdee66854
    __ dci(0xce6b8e61);  // rax1 v1.2d, v19.2d, v11.2d
    // vl128 state = 0x2e383dc2
    __ dci(0xce6e8e60);  // rax1 v0.2d, v19.2d, v14.2d
    // vl128 state = 0xa022bb6d
    __ dci(0xce6e8e62);  // rax1 v2.2d, v19.2d, v14.2d
    // vl128 state = 0x923f5d32
    __ dci(0xce668e23);  // rax1 v3.2d, v17.2d, v6.2d
    // vl128 state = 0xc2c6ca00
    __ dci(0xce260e33);  // bcax v19.16b, v17.16b, v6.16b, v3.16b
    // vl128 state = 0x517e85e9
    __ dci(0xce260e23);  // bcax v3.16b, v17.16b, v6.16b, v3.16b
    // vl128 state = 0xbcf4c332
    __ dci(0xce260e93);  // bcax v19.16b, v20.16b, v6.16b, v3.16b
    // vl128 state = 0x5d9d51ef
    __ dci(0xce260a11);  // bcax v17.16b, v16.16b, v6.16b, v2.16b
    // vl128 state = 0x69ce0099
    __ dci(0xce260a15);  // bcax v21.16b, v16.16b, v6.16b, v2.16b
    // vl128 state = 0x9a2cdc9f
    __ dci(0xce244a11);  // bcax v17.16b, v16.16b, v4.16b, v18.16b
    // vl128 state = 0x27eeff29
    __ dci(0xce304a10);  // bcax v16.16b, v16.16b, v16.16b, v18.16b
    // vl128 state = 0x6d586875
    __ dci(0xce314b18);  // bcax v24.16b, v24.16b, v17.16b, v18.16b
    // vl128 state = 0xe38b6054
    __ dci(0xce214b28);  // bcax v8.16b, v25.16b, v1.16b, v18.16b
    // vl128 state = 0x27a3f5f6
    __ dci(0xce294f38);  // bcax v24.16b, v25.16b, v9.16b, v19.16b
    // vl128 state = 0x7d7ffa9b
    __ dci(0xce214e39);  // bcax v25.16b, v17.16b, v1.16b, v19.16b
    // vl128 state = 0x936374f0
    __ dci(0xce216a3d);  // bcax v29.16b, v17.16b, v1.16b, v26.16b
    // vl128 state = 0x1c5136d5
    __ dci(0xce296b39);  // bcax v25.16b, v25.16b, v9.16b, v26.16b
    // vl128 state = 0x75cd7131
    __ dci(0xce216338);  // bcax v24.16b, v25.16b, v1.16b, v24.16b
    // vl128 state = 0xcc747626
    __ dci(0xce2163f9);  // bcax v25.16b, v31.16b, v1.16b, v24.16b
    // vl128 state = 0x9409c8bc
    __ dci(0xce2043f1);  // bcax v17.16b, v31.16b, v0.16b, v16.16b
    // vl128 state = 0x8db3a0c8
    __ dci(0xce2043f5);  // bcax v21.16b, v31.16b, v0.16b, v16.16b
    // vl128 state = 0xa55f8d7d
    __ dci(0xce2043e5);  // bcax v5.16b, v31.16b, v0.16b, v16.16b
    // vl128 state = 0xe1960c7a
    __ dci(0xce224be7);  // bcax v7.16b, v31.16b, v2.16b, v18.16b
    // vl128 state = 0xc9599bde
    __ dci(0xce204bb7);  // bcax v23.16b, v29.16b, v0.16b, v18.16b
    // vl128 state = 0x7176d08d
    __ dci(0xce004b9f);  // eor3 v31.16b, v28.16b, v0.16b, v18.16b
    // vl128 state = 0x10620821
    __ dci(0xce000baf);  // eor3 v15.16b, v29.16b, v0.16b, v2.16b
    // vl128 state = 0x0aba0288
    __ dci(0xce0a0bab);  // eor3 v11.16b, v29.16b, v10.16b, v2.16b
    // vl128 state = 0xe6517156
    __ dci(0xce0e1baf);  // eor3 v15.16b, v29.16b, v14.16b, v6.16b
    // vl128 state = 0x6b7021fb
    __ dci(0xce0e3fa7);  // eor3 v7.16b, v29.16b, v14.16b, v15.16b
    // vl128 state = 0x05761b1f
    __ dci(0xce0e2fe5);  // eor3 v5.16b, v31.16b, v14.16b, v11.16b
    // vl128 state = 0xe01822c6
    __ dci(0xce2e2fc7);  // bcax v7.16b, v30.16b, v14.16b, v11.16b
    // vl128 state = 0xdc6444d7
    __ dci(0xce3e2dcf);  // bcax v15.16b, v14.16b, v30.16b, v11.16b
    // vl128 state = 0xa5ecad2e
    __ dci(0xce3e3fdf);  // bcax v31.16b, v30.16b, v30.16b, v15.16b
    // vl128 state = 0x2124dc42
    __ dci(0xce3a3ede);  // bcax v30.16b, v22.16b, v26.16b, v15.16b
    // vl128 state = 0x57f77204
    __ dci(0xce3a2e9c);  // bcax v28.16b, v20.16b, v26.16b, v11.16b
    // vl128 state = 0x6e8d303d
    __ dci(0xce3a2294);  // bcax v20.16b, v20.16b, v26.16b, v8.16b
    // vl128 state = 0xdb53d42c
    __ dci(0xce38029c);  // bcax v28.16b, v20.16b, v24.16b, v0.16b
    // vl128 state = 0x258d49b8
    __ dci(0xce38088c);  // bcax v12.16b, v4.16b, v24.16b, v2.16b
    // vl128 state = 0xe751a348
    __ dci(0xce28008e);  // bcax v14.16b, v4.16b, v8.16b, v0.16b
    // vl128 state = 0x8ce0aa1a
    __ dci(0xce28008a);  // bcax v10.16b, v4.16b, v8.16b, v0.16b
    // vl128 state = 0x1fdf89a5
    __ dci(0xce280088);  // bcax v8.16b, v4.16b, v8.16b, v0.16b
    // vl128 state = 0xcc51f5e1
    __ dci(0xce2a1089);  // bcax v9.16b, v4.16b, v10.16b, v4.16b
    // vl128 state = 0xdaf766b0
    __ dci(0xce0b1081);  // eor3 v1.16b, v4.16b, v11.16b, v4.16b
    // vl128 state = 0x2da7deb5
    __ dci(0xce0a1011);  // eor3 v17.16b, v0.16b, v10.16b, v4.16b
    // vl128 state = 0xcc86f5d4
    __ dci(0xce121010);  // eor3 v16.16b, v0.16b, v18.16b, v4.16b
    // vl128 state = 0xfb722105
    __ dci(0xce921118);  // xar v24.2d, v8.2d, v18.2d, #4
    // vl128 state = 0x9a7752e3
    __ dci(0xce9a1199);  // xar v25.2d, v12.2d, v26.2d, #4
    // vl128 state = 0x83a251c2
    __ dci(0xce9e11dd);  // xar v29.2d, v14.2d, v30.2d, #4
    // vl128 state = 0x1e31c9d5
    __ dci(0xce9e915c);  // xar v28.2d, v10.2d, v30.2d, #36
    // vl128 state = 0x0e421d73
    __ dci(0xce1e115d);  // eor3 v29.16b, v10.16b, v30.16b, v4.16b
    // vl128 state = 0xb5a8c677
    __ dci(0xce3e515c);  // bcax v28.16b, v10.16b, v30.16b, v20.16b
    // vl128 state = 0x21587300
    __ dci(0xce3e5154);  // bcax v20.16b, v10.16b, v30.16b, v20.16b
    // vl128 state = 0x9459c629
    __ dci(0xce3e1056);  // bcax v22.16b, v2.16b, v30.16b, v4.16b
    // vl128 state = 0xdb02263a
    __ dci(0xce2a105e);  // bcax v30.16b, v2.16b, v10.16b, v4.16b
    // vl128 state = 0xc9d210aa
    __ dci(0xce3a5056);  // bcax v22.16b, v2.16b, v26.16b, v20.16b
    // vl128 state = 0x4cc56293
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x4cc56293,
        0xee8bac03,
        0xc1253ac9,
        0x9fe5aa0f,
        0x43df27f4,
        0x19f03be6,
        0xd26c928b,
        0x7b9da4c4,
        0xe13149a7,
        0x9fa11ed9,
        0xe02cc4dd,
        0x7848dfe7,
        0x5ed1726f,
        0x983e0123,
        0x34166240,
        0xc4ee172f,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}


}  // namespace aarch64
}  // namespace vixl
