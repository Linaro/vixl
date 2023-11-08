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

TEST_SVE(neon_sha2h) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kSHA2);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0x5e0152a2);  // sha256h2 q2, q21, v1.4s
    // vl128 state = 0x6bda8984
    __ dci(0x5e1552b2);  // sha256h2 q18, q21, v21.4s
    // vl128 state = 0xe985c68a
    __ dci(0x5e055293);  // sha256h2 q19, q20, v5.4s
    // vl128 state = 0xab18a98b
    __ dci(0x5e055297);  // sha256h2 q23, q20, v5.4s
    // vl128 state = 0x896bad28
    __ dci(0x5e0752a7);  // sha256h2 q7, q21, v7.4s
    // vl128 state = 0x4e00ba08
    __ dci(0x5e175223);  // sha256h2 q3, q17, v23.4s
    // vl128 state = 0x380f3893
    __ dci(0x5e1f5262);  // sha256h2 q2, q19, v31.4s
    // vl128 state = 0xb431122d
    __ dci(0x5e1f5272);  // sha256h2 q18, q19, v31.4s
    // vl128 state = 0x18140047
    __ dci(0x5e1e4262);  // sha256h q2, q19, v30.4s
    // vl128 state = 0x721779be
    __ dci(0x5e164363);  // sha256h q3, q27, v22.4s
    // vl128 state = 0x383ad878
    __ dci(0x5e175361);  // sha256h2 q1, q27, v23.4s
    // vl128 state = 0xd985bd85
    __ dci(0x5e115360);  // sha256h2 q0, q27, v17.4s
    // vl128 state = 0xfa5e77f3
    __ dci(0x5e135270);  // sha256h2 q16, q19, v19.4s
    // vl128 state = 0x4fc1f5cc
    __ dci(0x5e195260);  // sha256h2 q0, q19, v25.4s
    // vl128 state = 0x89435952
    __ dci(0x5e1952c4);  // sha256h2 q4, q22, v25.4s
    // vl128 state = 0x93c60c86
    __ dci(0x5e1a52c6);  // sha256h2 q6, q22, v26.4s
    // vl128 state = 0xedc42105
    __ dci(0x5e1a52c4);  // sha256h2 q4, q22, v26.4s
    // vl128 state = 0xd5d638a8
    __ dci(0x5e1a4285);  // sha256h q5, q20, v26.4s
    // vl128 state = 0x9f9da446
    __ dci(0x5e1a428d);  // sha256h q13, q20, v26.4s
    // vl128 state = 0x87d49cfb
    __ dci(0x5e1b42cf);  // sha256h q15, q22, v27.4s
    // vl128 state = 0xa6802b10
    __ dci(0x5e1b43ed);  // sha256h q13, q31, v27.4s
    // vl128 state = 0x2e346937
    __ dci(0x5e0b436f);  // sha256h q15, q27, v11.4s
    // vl128 state = 0x1005f372
    __ dci(0x5e03433f);  // sha256h q31, q25, v3.4s
    // vl128 state = 0xd908918c
    __ dci(0x5e13532f);  // sha256h2 q15, q25, v19.4s
    // vl128 state = 0x31c73fe0
    __ dci(0x5e01533f);  // sha256h2 q31, q25, v1.4s
    // vl128 state = 0x84e35a20
    __ dci(0x5e03523d);  // sha256h2 q29, q17, v3.4s
    // vl128 state = 0x40da34aa
    __ dci(0x5e0b527c);  // sha256h2 q28, q19, v11.4s
    // vl128 state = 0x506a21d9
    __ dci(0x5e0f5238);  // sha256h2 q24, q17, v15.4s
    // vl128 state = 0x6a67f033
    __ dci(0x5e0d5210);  // sha256h2 q16, q16, v13.4s
    // vl128 state = 0x317e084c
    __ dci(0x5e0d5214);  // sha256h2 q20, q16, v13.4s
    // vl128 state = 0xdd0eb379
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xdd0eb379,
        0x15384d69,
        0x32bbc73a,
        0xc5879e77,
        0x9241294d,
        0xfc01bad8,
        0xf5e79af5,
        0xee66e696,
        0x535158e8,
        0x09cfa8b6,
        0x8cd83eae,
        0x93ff18b0,
        0x561444e4,
        0xa6249eea,
        0x830e4c73,
        0xb516eaae,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(neon_sha2su0) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kSHA2);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0x5e2828e3);  // sha256su0 v3.4s, v7.4s
    // vl128 state = 0xbc7a7764
    __ dci(0x5e282be1);  // sha256su0 v1.4s, v31.4s
    // vl128 state = 0x6138a856
    __ dci(0x5e282be9);  // sha256su0 v9.4s, v31.4s
    // vl128 state = 0x49c6be17
    __ dci(0x5e282beb);  // sha256su0 v11.4s, v31.4s
    // vl128 state = 0xca658743
    __ dci(0x5e2829bb);  // sha256su0 v27.4s, v13.4s
    // vl128 state = 0x1bf1d233
    __ dci(0x5e2829ba);  // sha256su0 v26.4s, v13.4s
    // vl128 state = 0xafb0c6ae
    __ dci(0x5e2829aa);  // sha256su0 v10.4s, v13.4s
    // vl128 state = 0x2182e90d
    __ dci(0x5e282b2e);  // sha256su0 v14.4s, v25.4s
    // vl128 state = 0x401d297d
    __ dci(0x5e282aaf);  // sha256su0 v15.4s, v21.4s
    // vl128 state = 0x6c01fefa
    __ dci(0x5e282aad);  // sha256su0 v13.4s, v21.4s
    // vl128 state = 0x0f4c191d
    __ dci(0x5e282a7d);  // sha256su0 v29.4s, v19.4s
    // vl128 state = 0xcf26aa1b
    __ dci(0x5e282ad9);  // sha256su0 v25.4s, v22.4s
    // vl128 state = 0xae04081e
    __ dci(0x5e282ac9);  // sha256su0 v9.4s, v22.4s
    // vl128 state = 0x08149009
    __ dci(0x5e282acb);  // sha256su0 v11.4s, v22.4s
    // vl128 state = 0xa691e487
    __ dci(0x5e282ac3);  // sha256su0 v3.4s, v22.4s
    // vl128 state = 0xd728e1b5
    __ dci(0x5e282ac7);  // sha256su0 v7.4s, v22.4s
    // vl128 state = 0x120fac30
    __ dci(0x5e282ac5);  // sha256su0 v5.4s, v22.4s
    // vl128 state = 0x88086f82
    __ dci(0x5e282ac4);  // sha256su0 v4.4s, v22.4s
    // vl128 state = 0x625160b7
    __ dci(0x5e282a65);  // sha256su0 v5.4s, v19.4s
    // vl128 state = 0x308feecd
    __ dci(0x5e282a6d);  // sha256su0 v13.4s, v19.4s
    // vl128 state = 0x65f03097
    __ dci(0x5e282a65);  // sha256su0 v5.4s, v19.4s
    // vl128 state = 0x44d9fbb6
    __ dci(0x5e282a67);  // sha256su0 v7.4s, v19.4s
    // vl128 state = 0x694fe04a
    __ dci(0x5e282a17);  // sha256su0 v23.4s, v16.4s
    // vl128 state = 0x3d5c139b
    __ dci(0x5e282a13);  // sha256su0 v19.4s, v16.4s
    // vl128 state = 0x922f40a5
    __ dci(0x5e282b3b);  // sha256su0 v27.4s, v25.4s
    // vl128 state = 0x4f9c34f2
    __ dci(0x5e282ab9);  // sha256su0 v25.4s, v21.4s
    // vl128 state = 0x18a4f581
    __ dci(0x5e282ab1);  // sha256su0 v17.4s, v21.4s
    // vl128 state = 0x69da3844
    __ dci(0x5e282ab9);  // sha256su0 v25.4s, v21.4s
    // vl128 state = 0x57f8ce0b
    __ dci(0x5e282a1d);  // sha256su0 v29.4s, v16.4s
    // vl128 state = 0xafa03001
    __ dci(0x5e282ad5);  // sha256su0 v21.4s, v22.4s
    // vl128 state = 0x029b78a8
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x029b78a8,
        0x479a8911,
        0x6bdbdb48,
        0x5ef3718b,
        0x695ce173,
        0x586543d0,
        0xd00a22be,
        0xe63a91b9,
        0x42bb89a2,
        0xea48ee79,
        0x9788ac35,
        0x1e8599a3,
        0xd0d2d6ee,
        0xfe7aaaf7,
        0x77da6831,
        0xb93fb875,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(neon_sha2su1) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kSHA2);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0x5e1e6146);  // sha256su1 v6.4s, v10.4s, v30.4s
    // vl128 state = 0x3bbf7782
    __ dci(0x5e0f6144);  // sha256su1 v4.4s, v10.4s, v15.4s
    // vl128 state = 0xf8c83149
    __ dci(0x5e0e6174);  // sha256su1 v20.4s, v11.4s, v14.4s
    // vl128 state = 0x3b8c353b
    __ dci(0x5e0e6170);  // sha256su1 v16.4s, v11.4s, v14.4s
    // vl128 state = 0x1041e30e
    __ dci(0x5e0a6131);  // sha256su1 v17.4s, v9.4s, v10.4s
    // vl128 state = 0xe4d81cd2
    __ dci(0x5e0a6135);  // sha256su1 v21.4s, v9.4s, v10.4s
    // vl128 state = 0x24869db3
    __ dci(0x5e0a6131);  // sha256su1 v17.4s, v9.4s, v10.4s
    // vl128 state = 0xfb093436
    __ dci(0x5e0a6199);  // sha256su1 v25.4s, v12.4s, v10.4s
    // vl128 state = 0x0c7939ba
    __ dci(0x5e0e639b);  // sha256su1 v27.4s, v28.4s, v14.4s
    // vl128 state = 0xa7e5c40a
    __ dci(0x5e0663ab);  // sha256su1 v11.4s, v29.4s, v6.4s
    // vl128 state = 0xc4ae571c
    __ dci(0x5e06619b);  // sha256su1 v27.4s, v12.4s, v6.4s
    // vl128 state = 0xf84ef221
    __ dci(0x5e066199);  // sha256su1 v25.4s, v12.4s, v6.4s
    // vl128 state = 0x24f98d3c
    __ dci(0x5e0e6118);  // sha256su1 v24.4s, v8.4s, v14.4s
    // vl128 state = 0xcdb43a3b
    __ dci(0x5e0f601a);  // sha256su1 v26.4s, v0.4s, v15.4s
    // vl128 state = 0x85fd37e9
    __ dci(0x5e096012);  // sha256su1 v18.4s, v0.4s, v9.4s
    // vl128 state = 0xabccd3f6
    __ dci(0x5e0c601a);  // sha256su1 v26.4s, v0.4s, v12.4s
    // vl128 state = 0x8c0232e5
    __ dci(0x5e1c602a);  // sha256su1 v10.4s, v1.4s, v28.4s
    // vl128 state = 0xcdcf37ba
    __ dci(0x5e1e622e);  // sha256su1 v14.4s, v17.4s, v30.4s
    // vl128 state = 0x25129c9a
    __ dci(0x5e1e623e);  // sha256su1 v30.4s, v17.4s, v30.4s
    // vl128 state = 0xd0a281b7
    __ dci(0x5e1e630e);  // sha256su1 v14.4s, v24.4s, v30.4s
    // vl128 state = 0x3ed92f18
    __ dci(0x5e1f639e);  // sha256su1 v30.4s, v28.4s, v31.4s
    // vl128 state = 0xda1056b9
    __ dci(0x5e0f629f);  // sha256su1 v31.4s, v20.4s, v15.4s
    // vl128 state = 0x367274fa
    __ dci(0x5e0f63bd);  // sha256su1 v29.4s, v29.4s, v15.4s
    // vl128 state = 0x46a79748
    __ dci(0x5e0f63b5);  // sha256su1 v21.4s, v29.4s, v15.4s
    // vl128 state = 0xdc427315
    __ dci(0x5e0b63f7);  // sha256su1 v23.4s, v31.4s, v11.4s
    // vl128 state = 0x91547f41
    __ dci(0x5e0263e7);  // sha256su1 v7.4s, v31.4s, v2.4s
    // vl128 state = 0x1c233ffa
    __ dci(0x5e0062f7);  // sha256su1 v23.4s, v23.4s, v0.4s
    // vl128 state = 0x8c2948a1
    __ dci(0x5e1062c7);  // sha256su1 v7.4s, v22.4s, v16.4s
    // vl128 state = 0x8b72f498
    __ dci(0x5e1062c6);  // sha256su1 v6.4s, v22.4s, v16.4s
    // vl128 state = 0x43d27746
    __ dci(0x5e1063ee);  // sha256su1 v14.4s, v31.4s, v16.4s
    // vl128 state = 0xa864e589
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xa864e589,
        0xc588dfe0,
        0x171add38,
        0x884ca9db,
        0x5f47fb6a,
        0x0bd024c5,
        0xa6921cce,
        0x01dc8899,
        0x0f5b4b19,
        0x948260c1,
        0x4d4faafe,
        0x76ee7ff7,
        0xd9a56156,
        0x63c8e138,
        0xe687f7c3,
        0x51785434,
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

TEST_SVE(neon_sha512) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kSHA512);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0xce6583cc);  // sha512h q12, q30, v5.2d
    // vl128 state = 0xecc5733a
    __ dci(0xce6586c8);  // sha512h2 q8, q22, v5.2d
    // vl128 state = 0xe05f2087
    __ dci(0xce7586e0);  // sha512h2 q0, q23, v21.2d
    // vl128 state = 0x1925555b
    __ dci(0xce7187e8);  // sha512h2 q8, q31, v17.2d
    // vl128 state = 0x891dba65
    __ dci(0xce7586ec);  // sha512h2 q12, q23, v21.2d
    // vl128 state = 0xdfbe3239
    __ dci(0xce7580fc);  // sha512h q28, q7, v21.2d
    // vl128 state = 0xba49dbc1
    __ dci(0xce7580f4);  // sha512h q20, q7, v21.2d
    // vl128 state = 0x3ad11a23
    __ dci(0xce6780f6);  // sha512h q22, q7, v7.2d
    // vl128 state = 0xcf9e1803
    __ dci(0xce6780f7);  // sha512h q23, q7, v7.2d
    // vl128 state = 0xe2baee15
    __ dci(0xce6785e7);  // sha512h2 q7, q15, v7.2d
    // vl128 state = 0x900a337c
    __ dci(0xce6f8565);  // sha512h2 q5, q11, v15.2d
    // vl128 state = 0xc6e5d7eb
    __ dci(0xce6f8424);  // sha512h2 q4, q1, v15.2d
    // vl128 state = 0xcbcb6ac1
    __ dci(0xce6b84a6);  // sha512h2 q6, q5, v11.2d
    // vl128 state = 0xa3c1a679
    __ dci(0xce7b848e);  // sha512h2 q14, q4, v27.2d
    // vl128 state = 0x47c4e54d
    __ dci(0xce7d849e);  // sha512h2 q30, q4, v29.2d
    // vl128 state = 0x9f519a29
    __ dci(0xce7f859c);  // sha512h2 q28, q12, v31.2d
    // vl128 state = 0xa4433415
    __ dci(0xce778494);  // sha512h2 q20, q4, v23.2d
    // vl128 state = 0xf03a69ec
    __ dci(0xce778484);  // sha512h2 q4, q4, v23.2d
    // vl128 state = 0x2c728333
    __ dci(0xce77850c);  // sha512h2 q12, q8, v23.2d
    // vl128 state = 0xaedc423e
    __ dci(0xce77815c);  // sha512h q28, q10, v23.2d
    // vl128 state = 0xea9346ea
    __ dci(0xce7381cc);  // sha512h q12, q14, v19.2d
    // vl128 state = 0x05ad87d1
    __ dci(0xce7a81dc);  // sha512h q28, q14, v26.2d
    // vl128 state = 0x9b1cd7b3
    __ dci(0xce7285d4);  // sha512h2 q20, q14, v18.2d
    // vl128 state = 0x154201ac
    __ dci(0xce7280d6);  // sha512h q22, q6, v18.2d
    // vl128 state = 0xd8640492
    __ dci(0xce7a81d4);  // sha512h q20, q14, v26.2d
    // vl128 state = 0x908eb258
    __ dci(0xce7281f0);  // sha512h q16, q15, v18.2d
    // vl128 state = 0x0067f162
    __ dci(0xce728572);  // sha512h2 q18, q11, v18.2d
    // vl128 state = 0xca9bc751
    __ dci(0xce728422);  // sha512h2 q2, q1, v18.2d
    // vl128 state = 0x06b7318d
    __ dci(0xce738412);  // sha512h2 q18, q0, v19.2d
    // vl128 state = 0xad019588
    __ dci(0xce718016);  // sha512h q22, q0, v17.2d
    // vl128 state = 0x55a29e9b
    __ dci(0xce718834);  // sha512su1 v20.2d, v1.2d, v17.2d
    // vl128 state = 0x953a9c7a
    __ dci(0xce738876);  // sha512su1 v22.2d, v3.2d, v19.2d
    // vl128 state = 0x4f194c71
    __ dci(0xce638826);  // sha512su1 v6.2d, v1.2d, v3.2d
    // vl128 state = 0x08e50d47
    __ dci(0xce6b886e);  // sha512su1 v14.2d, v3.2d, v11.2d
    // vl128 state = 0x4bdfb870
    __ dci(0xce6b88de);  // sha512su1 v30.2d, v6.2d, v11.2d
    // vl128 state = 0xbcf4b6c5
    __ dci(0xce7f88df);  // sha512su1 v31.2d, v6.2d, v31.2d
    // vl128 state = 0x916dede1
    __ dci(0xce6f8acf);  // sha512su1 v15.2d, v22.2d, v15.2d
    // vl128 state = 0x3b776003
    __ dci(0xce6d8bcb);  // sha512su1 v11.2d, v30.2d, v13.2d
    // vl128 state = 0x5d5cb7d9
    __ dci(0xce6d83ea);  // sha512h q10, q31, v13.2d
    // vl128 state = 0x18df9e46
    __ dci(0xce6d8328);  // sha512h q8, q25, v13.2d
    // vl128 state = 0xde5807d0
    __ dci(0xce6583b8);  // sha512h q24, q29, v5.2d
    // vl128 state = 0x861020e7
    __ dci(0xce6d83f9);  // sha512h q25, q31, v13.2d
    // vl128 state = 0x39d960f4
    __ dci(0xce6d8b78);  // sha512su1 v24.2d, v27.2d, v13.2d
    // vl128 state = 0x3afc2b5c
    __ dci(0xce6c8968);  // sha512su1 v8.2d, v11.2d, v12.2d
    // vl128 state = 0x74d44114
    __ dci(0xce6c8b49);  // sha512su1 v9.2d, v26.2d, v12.2d
    // vl128 state = 0x72e6b5cd
    __ dci(0xce6c8b39);  // sha512su1 v25.2d, v25.2d, v12.2d
    // vl128 state = 0x6aaa4658
    __ dci(0xce6c8b9d);  // sha512su1 v29.2d, v28.2d, v12.2d
    // vl128 state = 0x7c076c9b
    __ dci(0xce648b0d);  // sha512su1 v13.2d, v24.2d, v4.2d
    // vl128 state = 0x1082519d
    __ dci(0xce648385);  // sha512h q5, q28, v4.2d
    // vl128 state = 0x9ed9d190
    __ dci(0xce648715);  // sha512h2 q21, q24, v4.2d
    // vl128 state = 0xaace5a02
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xaace5a02,
        0x912905de,
        0xc62c0756,
        0xac6646d5,
        0xd3c2e6af,
        0x029ae35f,
        0xf5e83b54,
        0x49f8d50c,
        0xc5175320,
        0xb51c8ebd,
        0x2dc184b0,
        0x01e01875,
        0x28df0d5a,
        0x01d2fff2,
        0x5f5f5909,
        0x6aead9d8,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(neon_sha512su0) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kSHA512);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0xcec083f6);  // sha512su0 v22.2d, v31.2d
    // vl128 state = 0xf7a54f2b
    __ dci(0xcec083e6);  // sha512su0 v6.2d, v31.2d
    // vl128 state = 0x919c170d
    __ dci(0xcec08347);  // sha512su0 v7.2d, v26.2d
    // vl128 state = 0x8a1800d6
    __ dci(0xcec082c6);  // sha512su0 v6.2d, v22.2d
    // vl128 state = 0x353aa8bf
    __ dci(0xcec082c4);  // sha512su0 v4.2d, v22.2d
    // vl128 state = 0x87d75b6c
    __ dci(0xcec082c0);  // sha512su0 v0.2d, v22.2d
    // vl128 state = 0xf2ee6974
    __ dci(0xcec082c1);  // sha512su0 v1.2d, v22.2d
    // vl128 state = 0xf2ec1e17
    __ dci(0xcec082c0);  // sha512su0 v0.2d, v22.2d
    // vl128 state = 0x1bcca060
    __ dci(0xcec082c4);  // sha512su0 v4.2d, v22.2d
    // vl128 state = 0x67773394
    __ dci(0xcec082c5);  // sha512su0 v5.2d, v22.2d
    // vl128 state = 0xbb344c8d
    __ dci(0xcec083e1);  // sha512su0 v1.2d, v31.2d
    // vl128 state = 0x595e2eb0
    __ dci(0xcec081a5);  // sha512su0 v5.2d, v13.2d
    // vl128 state = 0x7d7f4e15
    __ dci(0xcec081a7);  // sha512su0 v7.2d, v13.2d
    // vl128 state = 0xba4b1bc6
    __ dci(0xcec081a3);  // sha512su0 v3.2d, v13.2d
    // vl128 state = 0x2c56ee6e
    __ dci(0xcec083f3);  // sha512su0 v19.2d, v31.2d
    // vl128 state = 0xefe9b855
    __ dci(0xcec08397);  // sha512su0 v23.2d, v28.2d
    // vl128 state = 0x6f0d20ba
    __ dci(0xcec08396);  // sha512su0 v22.2d, v28.2d
    // vl128 state = 0x9be77fdb
    __ dci(0xcec081b7);  // sha512su0 v23.2d, v13.2d
    // vl128 state = 0x5d981c55
    __ dci(0xcec080ff);  // sha512su0 v31.2d, v7.2d
    // vl128 state = 0x9126079f
    __ dci(0xcec080fd);  // sha512su0 v29.2d, v7.2d
    // vl128 state = 0x3199dc9e
    __ dci(0xcec081dc);  // sha512su0 v28.2d, v14.2d
    // vl128 state = 0x20fb48d7
    __ dci(0xcec081cc);  // sha512su0 v12.2d, v14.2d
    // vl128 state = 0x4ae6221a
    __ dci(0xcec08088);  // sha512su0 v8.2d, v4.2d
    // vl128 state = 0x17e8b62d
    __ dci(0xcec0808a);  // sha512su0 v10.2d, v4.2d
    // vl128 state = 0x90d73468
    __ dci(0xcec0809a);  // sha512su0 v26.2d, v4.2d
    // vl128 state = 0x1f02f97f
    __ dci(0xcec081de);  // sha512su0 v30.2d, v14.2d
    // vl128 state = 0xe5ef3e67
    __ dci(0xcec081bf);  // sha512su0 v31.2d, v13.2d
    // vl128 state = 0xd1bcc363
    __ dci(0xcec081bb);  // sha512su0 v27.2d, v13.2d
    // vl128 state = 0x8bcfab58
    __ dci(0xcec08033);  // sha512su0 v19.2d, v1.2d
    // vl128 state = 0x93fb8bad
    __ dci(0xcec080fb);  // sha512su0 v27.2d, v7.2d
    // vl128 state = 0x3598e921
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x3598e921,
        0x7e3ee16c,
        0x4856987c,
        0x193bda79,
        0x84154d6f,
        0x861f1795,
        0xb74d39b3,
        0x9653d8b3,
        0x6690a066,
        0x00a29b51,
        0xb2c795ce,
        0xcbd03b05,
        0x9fb2aaec,
        0x0216b732,
        0x96eb6864,
        0x4024f5c7,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(neon_aes) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kAES);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0x4e285a86);  // aesd v6.16b, v20.16b
    // vl128 state = 0x801bfc08
    __ dci(0x4e2858ae);  // aesd v14.16b, v5.16b
    // vl128 state = 0xbd83a757
    __ dci(0x4e2858ac);  // aesd v12.16b, v5.16b
    // vl128 state = 0x9fb1dc6b
    __ dci(0x4e2858ae);  // aesd v14.16b, v5.16b
    // vl128 state = 0xfa1fa7e4
    __ dci(0x4e28482a);  // aese v10.16b, v1.16b
    // vl128 state = 0xecfcfe2d
    __ dci(0x4e28483a);  // aese v26.16b, v1.16b
    // vl128 state = 0x05e22f07
    __ dci(0x4e28488a);  // aese v10.16b, v4.16b
    // vl128 state = 0xdd53df5f
    __ dci(0x4e28488e);  // aese v14.16b, v4.16b
    // vl128 state = 0x9d2ac50f
    __ dci(0x4e28484f);  // aese v15.16b, v2.16b
    // vl128 state = 0xf45146ab
    __ dci(0x4e28484b);  // aese v11.16b, v2.16b
    // vl128 state = 0xf1260a7c
    __ dci(0x4e28485b);  // aese v27.16b, v2.16b
    // vl128 state = 0x3a0844da
    __ dci(0x4e285819);  // aesd v25.16b, v0.16b
    // vl128 state = 0xaca89993
    __ dci(0x4e284a09);  // aese v9.16b, v16.16b
    // vl128 state = 0xef4e9a5f
    __ dci(0x4e285a4b);  // aesd v11.16b, v18.16b
    // vl128 state = 0x209a44bc
    __ dci(0x4e285a4f);  // aesd v15.16b, v18.16b
    // vl128 state = 0xc6d2d718
    __ dci(0x4e285a4d);  // aesd v13.16b, v18.16b
    // vl128 state = 0x1aceef8f
    __ dci(0x4e285a45);  // aesd v5.16b, v18.16b
    // vl128 state = 0x7ed056c6
    __ dci(0x4e285af5);  // aesd v21.16b, v23.16b
    // vl128 state = 0x429ed71e
    __ dci(0x4e285a91);  // aesd v17.16b, v20.16b
    // vl128 state = 0xd7a1f687
    __ dci(0x4e284ad9);  // aese v25.16b, v22.16b
    // vl128 state = 0x8fa44574
    __ dci(0x4e284adb);  // aese v27.16b, v22.16b
    // vl128 state = 0xd2792169
    __ dci(0x4e285afa);  // aesd v26.16b, v23.16b
    // vl128 state = 0xe502f095
    __ dci(0x4e285bbb);  // aesd v27.16b, v29.16b
    // vl128 state = 0x0e3d3238
    __ dci(0x4e285bbf);  // aesd v31.16b, v29.16b
    // vl128 state = 0x0ad06592
    __ dci(0x4e285baf);  // aesd v15.16b, v29.16b
    // vl128 state = 0xb94f3c19
    __ dci(0x4e284b3f);  // aese v31.16b, v25.16b
    // vl128 state = 0xf31a0da1
    __ dci(0x4e284917);  // aese v23.16b, v8.16b
    // vl128 state = 0x7d2d7811
    __ dci(0x4e284913);  // aese v19.16b, v8.16b
    // vl128 state = 0x41b7b854
    __ dci(0x4e284911);  // aese v17.16b, v8.16b
    // vl128 state = 0x60600536
    __ dci(0x4e2849d5);  // aese v21.16b, v14.16b
    // vl128 state = 0x3e0cc74f
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x3e0cc74f,
        0x7f17ba2e,
        0xd59f8e91,
        0x9f15a51b,
        0x11d92e66,
        0xcd53d015,
        0xbc652785,
        0x6974fa54,
        0x953d342e,
        0xf1aa56b3,
        0xde8ca1d3,
        0xba408b82,
        0x48094fa4,
        0xb757bcf1,
        0x2cc5be58,
        0x6e7a0f58,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(neon_aesmc) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kAES);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0x4e287800);  // aesimc v0.16b, v0.16b
    // vl128 state = 0x03554749
    __ dci(0x4e287a28);  // aesimc v8.16b, v17.16b
    // vl128 state = 0x59d5fedd
    __ dci(0x4e287a2a);  // aesimc v10.16b, v17.16b
    // vl128 state = 0xcda29514
    __ dci(0x4e286aae);  // aesmc v14.16b, v21.16b
    // vl128 state = 0xae8f019a
    __ dci(0x4e286abe);  // aesmc v30.16b, v21.16b
    // vl128 state = 0x7b04c6c0
    __ dci(0x4e286a0e);  // aesmc v14.16b, v16.16b
    // vl128 state = 0xaf6c5ce6
    __ dci(0x4e286a0a);  // aesmc v10.16b, v16.16b
    // vl128 state = 0xf1d7fd2b
    __ dci(0x4e286acb);  // aesmc v11.16b, v22.16b
    // vl128 state = 0x5d693c63
    __ dci(0x4e286acf);  // aesmc v15.16b, v22.16b
    // vl128 state = 0xec8971ad
    __ dci(0x4e286adf);  // aesmc v31.16b, v22.16b
    // vl128 state = 0x6389b200
    __ dci(0x4e287a9d);  // aesimc v29.16b, v20.16b
    // vl128 state = 0xd69341fb
    __ dci(0x4e28688d);  // aesmc v13.16b, v4.16b
    // vl128 state = 0x6344af95
    __ dci(0x4e2878cf);  // aesimc v15.16b, v6.16b
    // vl128 state = 0x5c58dfac
    __ dci(0x4e2878cb);  // aesimc v11.16b, v6.16b
    // vl128 state = 0x7dc9cf34
    __ dci(0x4e2878c9);  // aesimc v9.16b, v6.16b
    // vl128 state = 0xff4b3544
    __ dci(0x4e2878c1);  // aesimc v1.16b, v6.16b
    // vl128 state = 0xd1937de2
    __ dci(0x4e287871);  // aesimc v17.16b, v3.16b
    // vl128 state = 0x7cabd208
    __ dci(0x4e287815);  // aesimc v21.16b, v0.16b
    // vl128 state = 0xbc06df94
    __ dci(0x4e28685d);  // aesmc v29.16b, v2.16b
    // vl128 state = 0xfc4478bb
    __ dci(0x4e28685f);  // aesmc v31.16b, v2.16b
    // vl128 state = 0x0c72c200
    __ dci(0x4e28787e);  // aesimc v30.16b, v3.16b
    // vl128 state = 0xdd822b9d
    __ dci(0x4e28793f);  // aesimc v31.16b, v9.16b
    // vl128 state = 0x1397dcc6
    __ dci(0x4e28793b);  // aesimc v27.16b, v9.16b
    // vl128 state = 0x43f3abd6
    __ dci(0x4e28792b);  // aesimc v11.16b, v9.16b
    // vl128 state = 0xeb8ca365
    __ dci(0x4e2869bb);  // aesmc v27.16b, v13.16b
    // vl128 state = 0x0a957f4f
    __ dci(0x4e286b93);  // aesmc v19.16b, v28.16b
    // vl128 state = 0xbc5da8bd
    __ dci(0x4e286b97);  // aesmc v23.16b, v28.16b
    // vl128 state = 0xc49343cc
    __ dci(0x4e286b95);  // aesmc v21.16b, v28.16b
    // vl128 state = 0x8c80c144
    __ dci(0x4e286b51);  // aesmc v17.16b, v26.16b
    // vl128 state = 0xeda3255d
    __ dci(0x4e2869d3);  // aesmc v19.16b, v14.16b
    // vl128 state = 0x8db8a9d0
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x8db8a9d0,
        0xb13d8e1e,
        0x9f33ca70,
        0x38f7ef7a,
        0x65352b29,
        0xc4257260,
        0xf49587c2,
        0xb3f61256,
        0x8ef4a534,
        0x6e061aa9,
        0x7270527d,
        0x3e1f82f9,
        0x1fe79e60,
        0x985cab68,
        0xe77b4484,
        0xe3817f4e,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(neon_sm3) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kSM3);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 10 * kInstructionSize);
    __ dci(0xce591017);  // sm3ss1 v23.4s, v0.4s, v25.4s, v4.4s
    // vl128 state = 0xad4bba0a
    __ dci(0xce49121f);  // sm3ss1 v31.4s, v16.4s, v9.4s, v4.4s
    // vl128 state = 0x84adef21
    __ dci(0xce49121e);  // sm3ss1 v30.4s, v16.4s, v9.4s, v4.4s
    // vl128 state = 0xccfd7e5a
    __ dci(0xce49301a);  // sm3ss1 v26.4s, v0.4s, v9.4s, v12.4s
    // vl128 state = 0x60833cc7
    __ dci(0xce49720a);  // sm3ss1 v10.4s, v16.4s, v9.4s, v28.4s
    // vl128 state = 0x03f03263
    __ dci(0xce58721a);  // sm3ss1 v26.4s, v16.4s, v24.4s, v28.4s
    // vl128 state = 0x31845f40
    __ dci(0xce58702a);  // sm3ss1 v10.4s, v1.4s, v24.4s, v28.4s
    // vl128 state = 0x54c64f70
    __ dci(0xce58753a);  // sm3ss1 v26.4s, v9.4s, v24.4s, v29.4s
    // vl128 state = 0x3d5cb04f
    __ dci(0xce507518);  // sm3ss1 v24.4s, v8.4s, v16.4s, v29.4s
    // vl128 state = 0xe02de221
    __ dci(0xce406519);  // sm3ss1 v25.4s, v8.4s, v0.4s, v25.4s
    // vl128 state = 0x73d36ae8
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x73d36ae8,
        0xcbcda2db,
        0x6ee9ad3d,
        0xa6857a16,
        0xa238ec05,
        0x1bc82d1d,
        0xe4530773,
        0xfb0d092e,
        0xe62aff0a,
        0xf56a593f,
        0x3967d590,
        0xebcd14a0,
        0xa7bedcb8,
        0x867fa43c,
        0x1679eab5,
        0x0a836861,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(neon_sm3partw12) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kSM3);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0xce70c201);  // sm3partw1 v1.4s, v16.4s, v16.4s
    // vl128 state = 0x6f2069a6
    __ dci(0xce72c303);  // sm3partw1 v3.4s, v24.4s, v18.4s
    // vl128 state = 0x986fa56c
    __ dci(0xce76c381);  // sm3partw1 v1.4s, v28.4s, v22.4s
    // vl128 state = 0x5dbd953c
    __ dci(0xce7ec3b1);  // sm3partw1 v17.4s, v29.4s, v30.4s
    // vl128 state = 0xc72ccca5
    __ dci(0xce7ac1b5);  // sm3partw1 v21.4s, v13.4s, v26.4s
    // vl128 state = 0x33cdfd6a
    __ dci(0xce7ac1b7);  // sm3partw1 v23.4s, v13.4s, v26.4s
    // vl128 state = 0x4303e945
    __ dci(0xce7ac1bf);  // sm3partw1 v31.4s, v13.4s, v26.4s
    // vl128 state = 0x56acac84
    __ dci(0xce78c1fd);  // sm3partw1 v29.4s, v15.4s, v24.4s
    // vl128 state = 0x5e2a2793
    __ dci(0xce78c5df);  // sm3partw2 v31.4s, v14.4s, v24.4s
    // vl128 state = 0xf7c457f3
    __ dci(0xce70c55d);  // sm3partw2 v29.4s, v10.4s, v16.4s
    // vl128 state = 0xfa3557ac
    __ dci(0xce60c159);  // sm3partw1 v25.4s, v10.4s, v0.4s
    // vl128 state = 0xb3ae6830
    __ dci(0xce62c55b);  // sm3partw2 v27.4s, v10.4s, v2.4s
    // vl128 state = 0xa7747c70
    __ dci(0xce66c753);  // sm3partw2 v19.4s, v26.4s, v6.4s
    // vl128 state = 0xb55f5895
    __ dci(0xce67c551);  // sm3partw2 v17.4s, v10.4s, v7.4s
    // vl128 state = 0x519b1342
    __ dci(0xce65c750);  // sm3partw2 v16.4s, v26.4s, v5.4s
    // vl128 state = 0xc4e6e4b9
    __ dci(0xce61c718);  // sm3partw2 v24.4s, v24.4s, v1.4s
    // vl128 state = 0x127c483c
    __ dci(0xce61c71c);  // sm3partw2 v28.4s, v24.4s, v1.4s
    // vl128 state = 0x92783ecc
    __ dci(0xce6dc714);  // sm3partw2 v20.4s, v24.4s, v13.4s
    // vl128 state = 0xe11e87d3
    __ dci(0xce65c756);  // sm3partw2 v22.4s, v26.4s, v5.4s
    // vl128 state = 0x8b6878d0
    __ dci(0xce65c5d2);  // sm3partw2 v18.4s, v14.4s, v5.4s
    // vl128 state = 0xf2fb1e86
    __ dci(0xce64c550);  // sm3partw2 v16.4s, v10.4s, v4.4s
    // vl128 state = 0x73ad3b0f
    __ dci(0xce66c578);  // sm3partw2 v24.4s, v11.4s, v6.4s
    // vl128 state = 0x7e03900d
    __ dci(0xce76c55c);  // sm3partw2 v28.4s, v10.4s, v22.4s
    // vl128 state = 0x1d0b5df6
    __ dci(0xce76c54c);  // sm3partw2 v12.4s, v10.4s, v22.4s
    // vl128 state = 0x1a3d7a77
    __ dci(0xce7ec448);  // sm3partw2 v8.4s, v2.4s, v30.4s
    // vl128 state = 0x3ed2e4bd
    __ dci(0xce6ec409);  // sm3partw2 v9.4s, v0.4s, v14.4s
    // vl128 state = 0x826dd348
    __ dci(0xce6ec52b);  // sm3partw2 v11.4s, v9.4s, v14.4s
    // vl128 state = 0x3ff5e482
    __ dci(0xce66c72f);  // sm3partw2 v15.4s, v25.4s, v6.4s
    // vl128 state = 0x6fd24cd4
    __ dci(0xce65c73f);  // sm3partw2 v31.4s, v25.4s, v5.4s
    // vl128 state = 0xd51ac474
    __ dci(0xce67c77b);  // sm3partw2 v27.4s, v27.4s, v7.4s
    // vl128 state = 0x720d7419
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x720d7419,
        0x31445e06,
        0xd2aee240,
        0x45a27e4b,
        0xd6c46f08,
        0xcaed7f9e,
        0x734820c7,
        0x377e1f38,
        0x12e03585,
        0x1b9cbe63,
        0x1d58d49a,
        0xc160a9dc,
        0x22c2fe25,
        0x86b7af0f,
        0xfeae7bf5,
        0xf8dfcc40,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(neon_sm3tt1) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kSM3);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 20 * kInstructionSize);
    __ dci(0xce53a363);  // sm3tt1a v3.4s, v27.4s, v19.s[2]
    // vl128 state = 0xaaa8c715
    __ dci(0xce58a7a7);  // sm3tt1b v7.4s, v29.4s, v24.s[2]
    // vl128 state = 0xb99a301d
    __ dci(0xce5eb2b7);  // sm3tt1a v23.4s, v21.4s, v30.s[3]
    // vl128 state = 0xe8dabe99
    __ dci(0xce43b6ce);  // sm3tt1b v14.4s, v22.4s, v3.s[3]
    // vl128 state = 0xaa498ae5
    __ dci(0xce448027);  // sm3tt1a v7.4s, v1.4s, v4.s[0]
    // vl128 state = 0x32093547
    __ dci(0xce4286d8);  // sm3tt1b v24.4s, v22.4s, v2.s[0]
    // vl128 state = 0xe03e3a81
    __ dci(0xce44a0f3);  // sm3tt1a v19.4s, v7.4s, v4.s[2]
    // vl128 state = 0xcb555b4a
    __ dci(0xce418233);  // sm3tt1a v19.4s, v17.4s, v1.s[0]
    // vl128 state = 0x751e4f7d
    __ dci(0xce58a49f);  // sm3tt1b v31.4s, v4.4s, v24.s[2]
    // vl128 state = 0xcaff7580
    __ dci(0xce548326);  // sm3tt1a v6.4s, v25.4s, v20.s[0]
    // vl128 state = 0xc4308a78
    __ dci(0xce548124);  // sm3tt1a v4.4s, v9.4s, v20.s[0]
    // vl128 state = 0x1f1bfdfb
    __ dci(0xce5fb282);  // sm3tt1a v2.4s, v20.4s, v31.s[3]
    // vl128 state = 0xa632c0b2
    __ dci(0xce549573);  // sm3tt1b v19.4s, v11.4s, v20.s[1]
    // vl128 state = 0x7fb7c2d3
    __ dci(0xce4387ae);  // sm3tt1b v14.4s, v29.4s, v3.s[0]
    // vl128 state = 0xe8d4c534
    __ dci(0xce5094eb);  // sm3tt1b v11.4s, v7.4s, v16.s[1]
    // vl128 state = 0xf34a4fbc
    __ dci(0xce51b59f);  // sm3tt1b v31.4s, v12.4s, v17.s[3]
    // vl128 state = 0x98e388e9
    __ dci(0xce50a7bf);  // sm3tt1b v31.4s, v29.4s, v16.s[2]
    // vl128 state = 0x7cd7a6ac
    __ dci(0xce5ca52e);  // sm3tt1b v14.4s, v9.4s, v28.s[2]
    // vl128 state = 0xce9410c5
    __ dci(0xce5aa741);  // sm3tt1b v1.4s, v26.4s, v26.s[2]
    // vl128 state = 0xd83fbd58
    __ dci(0xce5e94da);  // sm3tt1b v26.4s, v6.4s, v30.s[1]
    // vl128 state = 0xc6055fe3
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xc6055fe3,
        0xa2c33f98,
        0x1cc9a227,
        0xf29eb254,
        0xd1739d6e,
        0x1c4fff34,
        0x0c182795,
        0x96e46836,
        0x43d010c9,
        0xd7c4f94c,
        0x78c387f2,
        0x4319fef3,
        0x72407eef,
        0xa77d3869,
        0x3c81c49a,
        0x68cc20ef,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(neon_sm3tt2) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32,
                          CPUFeatures::kSM3);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 20 * kInstructionSize);
    __ dci(0xce439d42);  // sm3tt2b v2.4s, v10.4s, v3.s[1]
    // vl128 state = 0x388642cc
    __ dci(0xce42b89d);  // sm3tt2a v29.4s, v4.4s, v2.s[3]
    // vl128 state = 0x66f4e60a
    __ dci(0xce4da95d);  // sm3tt2a v29.4s, v10.4s, v13.s[2]
    // vl128 state = 0x95d4651d
    __ dci(0xce49b926);  // sm3tt2a v6.4s, v9.4s, v9.s[3]
    // vl128 state = 0x826919fe
    __ dci(0xce5cae33);  // sm3tt2b v19.4s, v17.4s, v28.s[2]
    // vl128 state = 0xb5cfefb0
    __ dci(0xce478959);  // sm3tt2a v25.4s, v10.4s, v7.s[0]
    // vl128 state = 0xfe17b730
    __ dci(0xce549cc2);  // sm3tt2b v2.4s, v6.4s, v20.s[1]
    // vl128 state = 0x769a0d76
    __ dci(0xce4c9f90);  // sm3tt2b v16.4s, v28.4s, v12.s[1]
    // vl128 state = 0x8f633b95
    __ dci(0xce508d49);  // sm3tt2b v9.4s, v10.4s, v16.s[0]
    // vl128 state = 0x5eab6daa
    __ dci(0xce59ad79);  // sm3tt2b v25.4s, v11.4s, v25.s[2]
    // vl128 state = 0xfb197616
    __ dci(0xce458fd6);  // sm3tt2b v22.4s, v30.4s, v5.s[0]
    // vl128 state = 0x875ff29d
    __ dci(0xce4ab92c);  // sm3tt2a v12.4s, v9.4s, v10.s[3]
    // vl128 state = 0xad159c01
    __ dci(0xce598a1c);  // sm3tt2a v28.4s, v16.4s, v25.s[0]
    // vl128 state = 0x3da313e4
    __ dci(0xce43989f);  // sm3tt2a v31.4s, v4.4s, v3.s[1]
    // vl128 state = 0xc0a54179
    __ dci(0xce459c8a);  // sm3tt2b v10.4s, v4.4s, v5.s[1]
    // vl128 state = 0x4739cdbf
    __ dci(0xce539959);  // sm3tt2a v25.4s, v10.4s, v19.s[1]
    // vl128 state = 0xd85f84ab
    __ dci(0xce429be1);  // sm3tt2a v1.4s, v31.4s, v2.s[1]
    // vl128 state = 0x85b5871c
    __ dci(0xce5d9fe3);  // sm3tt2b v3.4s, v31.4s, v29.s[1]
    // vl128 state = 0x2be5bd95
    __ dci(0xce4ebe16);  // sm3tt2b v22.4s, v16.4s, v14.s[3]
    // vl128 state = 0x2f8146e9
    __ dci(0xce599a63);  // sm3tt2a v3.4s, v19.4s, v25.s[1]
    // vl128 state = 0xa6e513e2
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xa6e513e2,
        0x6bf4ae47,
        0x74e074db,
        0xae1a57e0,
        0x0db67f09,
        0x85332e49,
        0xc40d6565,
        0x07ed81aa,
        0xfa0e10bb,
        0x9addadfa,
        0xa9cea561,
        0xa481e17b,
        0x7c2be34e,
        0xd4cf493f,
        0x8b30cc5e,
        0xe44416d3,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

}  // namespace aarch64
}  // namespace vixl
