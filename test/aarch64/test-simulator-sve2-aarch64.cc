// Copyright 2020, VIXL authors
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

#include <sys/mman.h>
#include <unistd.h>

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>

#include "test-runner.h"
#include "test-utils.h"
#include "aarch64/test-utils-aarch64.h"

#include "aarch64/cpu-aarch64.h"
#include "aarch64/disasm-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"
#include "aarch64/simulator-aarch64.h"
#include "test-assembler-aarch64.h"

#define TEST_SVE(name) TEST_SVE_INNER("SIM", name)

namespace vixl {
namespace aarch64 {

TEST_SVE(sve2_halving_arithmetic) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x441182b2);  // uhadd z18.b, p0/m, z18.b, z21.b
    // vl128 state = 0x8ac2942a
    __ dci(0x441382f3);  // uhsub z19.b, p0/m, z19.b, z23.b
    // vl128 state = 0x0e0db643
    __ dci(0x449383fb);  // uhsub z27.s, p0/m, z27.s, z31.s
    // vl128 state = 0x6a97fc8c
    __ dci(0x441283fa);  // shsub z26.b, p0/m, z26.b, z31.b
    // vl128 state = 0x48a5fd5f
    __ dci(0x44928372);  // shsub z18.s, p0/m, z18.s, z27.s
    // vl128 state = 0x7c670d36
    __ dci(0x44d2827a);  // shsub z26.d, p0/m, z26.d, z19.d
    // vl128 state = 0x3a15c66f
    __ dci(0x4492823b);  // shsub z27.s, p0/m, z27.s, z17.s
    // vl128 state = 0xe407c826
    __ dci(0x44978239);  // uhsubr z25.s, p0/m, z25.s, z17.s
    // vl128 state = 0xf7157dae
    __ dci(0x4493827d);  // uhsub z29.s, p0/m, z29.s, z19.s
    // vl128 state = 0xcebff22f
    __ dci(0x449782f9);  // uhsubr z25.s, p0/m, z25.s, z23.s
    // vl128 state = 0xbe691139
    __ dci(0x44978231);  // uhsubr z17.s, p0/m, z17.s, z17.s
    // vl128 state = 0x59b2af72
    __ dci(0x44578233);  // uhsubr z19.h, p0/m, z19.h, z17.h
    // vl128 state = 0xd7fad727
    __ dci(0x44578312);  // uhsubr z18.h, p0/m, z18.h, z24.h
    // vl128 state = 0x87b5d00a
    __ dci(0x44578610);  // uhsubr z16.h, p1/m, z16.h, z16.h
    // vl128 state = 0xbaae097d
    __ dci(0x44578618);  // uhsubr z24.h, p1/m, z24.h, z16.h
    // vl128 state = 0x3887509e
    __ dci(0x44168608);  // shsubr z8.b, p1/m, z8.b, z16.b
    // vl128 state = 0xc16dc63b
    __ dci(0x44128700);  // shsub z0.b, p1/m, z0.b, z24.b
    // vl128 state = 0x3eddcd6d
    __ dci(0x44528f02);  // shsub z2.h, p3/m, z2.h, z24.h
    // vl128 state = 0x2e7ffa0d
    __ dci(0x44538f40);  // uhsub z0.h, p3/m, z0.h, z26.h
    // vl128 state = 0x1f68bee5
    __ dci(0x44538342);  // uhsub z2.h, p0/m, z2.h, z26.h
    // vl128 state = 0x2a368049
    __ dci(0x44538040);  // uhsub z0.h, p0/m, z0.h, z2.h
    // vl128 state = 0x0537f844
    __ dci(0x44568044);  // shsubr z4.h, p0/m, z4.h, z2.h
    // vl128 state = 0x0dfac1b2
    __ dci(0x445688cc);  // shsubr z12.h, p2/m, z12.h, z6.h
    // vl128 state = 0xbefa909b
    __ dci(0x44d288dc);  // shsub z28.d, p2/m, z28.d, z6.d
    // vl128 state = 0xbadc14bb
    __ dci(0x44d288d8);  // shsub z24.d, p2/m, z24.d, z6.d
    // vl128 state = 0x518130c0
    __ dci(0x44d088f0);  // shadd z16.d, p2/m, z16.d, z7.d
    // vl128 state = 0xb01856bd
    __ dci(0x44d08cd2);  // shadd z18.d, p3/m, z18.d, z6.d
    // vl128 state = 0xbbcfeaa2
    __ dci(0x44d484d0);  // srhadd z16.d, p1/m, z16.d, z6.d
    // vl128 state = 0xefe1d416
    __ dci(0x44d496d1);  // srhadd z17.d, p5/m, z17.d, z22.d
    // vl128 state = 0xceb574b8
    __ dci(0x44d196d5);  // uhadd z21.d, p5/m, z21.d, z22.d
    // vl128 state = 0x46cdd268
    __ dci(0x44d496dd);  // srhadd z29.d, p5/m, z29.d, z22.d
    // vl128 state = 0x21a81b6a
    __ dci(0x4494969c);  // srhadd z28.s, p5/m, z28.s, z20.s
    // vl128 state = 0x2316cb04
    __ dci(0x4494968c);  // srhadd z12.s, p5/m, z12.s, z20.s
    // vl128 state = 0x6248cc0a
    __ dci(0x4415968d);  // urhadd z13.b, p5/m, z13.b, z20.b
    // vl128 state = 0x6edd11e0
    __ dci(0x44119e8c);  // uhadd z12.b, p7/m, z12.b, z20.b
    // vl128 state = 0x81841eb6
    __ dci(0x4491968d);  // uhadd z13.s, p5/m, z13.s, z20.s
    // vl128 state = 0x02b8b893
    __ dci(0x44118685);  // uhadd z5.b, p1/m, z5.b, z20.b
    // vl128 state = 0x707db891
    __ dci(0x44138e8d);  // uhsub z13.b, p3/m, z13.b, z20.b
    // vl128 state = 0x2caa64dd
    __ dci(0x44139e0c);  // uhsub z12.b, p7/m, z12.b, z16.b
    // vl128 state = 0xe34695ef
    __ dci(0x44128e0d);  // shsub z13.b, p3/m, z13.b, z16.b
    // vl128 state = 0x477197dd
    __ dci(0x44129a1d);  // shsub z29.b, p6/m, z29.b, z16.b
    // vl128 state = 0x19cebaa2
    __ dci(0x44129a19);  // shsub z25.b, p6/m, z25.b, z16.b
    // vl128 state = 0x0d62dca4
    __ dci(0x44129249);  // shsub z9.b, p4/m, z9.b, z18.b
    // vl128 state = 0x327e81e3
    __ dci(0x44129248);  // shsub z8.b, p4/m, z8.b, z18.b
    // vl128 state = 0x28ec9bf8
    __ dci(0x44169269);  // shsubr z9.b, p4/m, z9.b, z19.b
    // vl128 state = 0x652ca8c9
    __ dci(0x44168661);  // shsubr z1.b, p1/m, z1.b, z19.b
    // vl128 state = 0x46fcb15a
    __ dci(0x44168420);  // shsubr z0.b, p1/m, z0.b, z1.b
    // vl128 state = 0x7151e02b
    __ dci(0x44168428);  // shsubr z8.b, p1/m, z8.b, z1.b
    // vl128 state = 0x4c8921f6
    __ dci(0x44148409);  // srhadd z9.b, p1/m, z9.b, z0.b
    // vl128 state = 0xd0d2fc1c
    __ dci(0x44148641);  // srhadd z1.b, p1/m, z1.b, z18.b
    // vl128 state = 0xc821f381
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xc821f381,
        0xc0ad3b7c,
        0x4eb4ba1b,
        0xdc8e061a,
        0x64675a15,
        0x923703bf,
        0x6944c0db,
        0x7ac89bae,
        0x8fa4c45f,
        0xf64c8b4c,
        0x8ba751b7,
        0x2fe8832e,
        0xc6b8000d,
        0x864ba0ff,
        0xded22c04,
        0x213cf65e,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_sli_sri) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x4509f07f);  // sri z31.b, z3.b, #7
    // vl128 state = 0x509a7a2d
    __ dci(0x454bf07e);  // sri z30.s, z3.s, #21
    // vl128 state = 0xc973a4e8
    __ dci(0x450bf17a);  // sri z26.b, z11.b, #5
    // vl128 state = 0xa9dcbcf5
    __ dci(0x450ef17b);  // sri z27.b, z11.b, #2
    // vl128 state = 0xd56761c1
    __ dci(0x458ef1f9);  // sri z25.d, z15.d, #50
    // vl128 state = 0xdd84a538
    __ dci(0x459ff1fb);  // sri z27.d, z15.d, #33
    // vl128 state = 0x4e2dbf4a
    __ dci(0x459ff5df);  // sli z31.d, z14.d, #31
    // vl128 state = 0x46d9563e
    __ dci(0x45d7f5cf);  // sli z15.d, z14.d, #55
    // vl128 state = 0xf4fcf912
    __ dci(0x4593f5ce);  // sli z14.d, z14.d, #19
    // vl128 state = 0xcef34d18
    __ dci(0x4593f1fe);  // sri z30.d, z15.d, #45
    // vl128 state = 0x69509e94
    __ dci(0x4581f1ff);  // sri z31.d, z15.d, #63
    // vl128 state = 0x09cd0cf7
    __ dci(0x45c1f1bd);  // sri z29.d, z13.d, #31
    // vl128 state = 0xfc095f8b
    __ dci(0x45c1f03c);  // sri z28.d, z1.d, #31
    // vl128 state = 0x0ca836f0
    __ dci(0x45c1f4b4);  // sli z20.d, z5.d, #33
    // vl128 state = 0x678be6b3
    __ dci(0x45c1f5f0);  // sli z16.d, z15.d, #33
    // vl128 state = 0x7a743b56
    __ dci(0x45c7f5f2);  // sli z18.d, z15.d, #39
    // vl128 state = 0x0bbc4117
    __ dci(0x45c7f5e2);  // sli z2.d, z15.d, #39
    // vl128 state = 0x13e1a7ae
    __ dci(0x45c7f1a0);  // sri z0.d, z13.d, #25
    // vl128 state = 0x8014a497
    __ dci(0x4597f1b0);  // sri z16.d, z13.d, #41
    // vl128 state = 0x5f7994a8
    __ dci(0x4593f5b1);  // sli z17.d, z13.d, #19
    // vl128 state = 0x125f37b5
    __ dci(0x4591f5f0);  // sli z16.d, z15.d, #17
    // vl128 state = 0x26f1fdf2
    __ dci(0x4581f5d2);  // sli z18.d, z14.d, #1
    // vl128 state = 0x5b0baccc
    __ dci(0x4541f5d6);  // sli z22.s, z14.s, #1
    // vl128 state = 0x74f04ecb
    __ dci(0x4551f1d4);  // sri z20.s, z14.s, #15
    // vl128 state = 0xc43d0586
    __ dci(0x4553f150);  // sri z16.s, z10.s, #13
    // vl128 state = 0xce8c688a
    __ dci(0x4557f171);  // sri z17.s, z11.s, #9
    // vl128 state = 0x03a5b3b0
    __ dci(0x4513f175);  // sri z21.h, z11.h, #13
    // vl128 state = 0x392ab48e
    __ dci(0x4551f177);  // sri z23.s, z11.s, #15
    // vl128 state = 0xa886dbc8
    __ dci(0x4551f17f);  // sri z31.s, z11.s, #15
    // vl128 state = 0x37c804bc
    __ dci(0x4551f16f);  // sri z15.s, z11.s, #15
    // vl128 state = 0x17e99d67
    __ dci(0x4550f067);  // sri z7.s, z3.s, #16
    // vl128 state = 0xb0bd981a
    __ dci(0x4550f077);  // sri z23.s, z3.s, #16
    // vl128 state = 0x5f643b3e
    __ dci(0x4551f0f5);  // sri z21.s, z7.s, #15
    // vl128 state = 0xa0b83a32
    __ dci(0x4551f09d);  // sri z29.s, z4.s, #15
    // vl128 state = 0x890807a1
    __ dci(0x4552f08d);  // sri z13.s, z4.s, #14
    // vl128 state = 0x81cb8fa4
    __ dci(0x4512f01d);  // sri z29.h, z0.h, #14
    // vl128 state = 0x62751a54
    __ dci(0x4552f419);  // sli z25.s, z0.s, #18
    // vl128 state = 0xfd7c0337
    __ dci(0x4542f49b);  // sli z27.s, z4.s, #2
    // vl128 state = 0x0089e534
    __ dci(0x454af09a);  // sri z26.s, z4.s, #22
    // vl128 state = 0xea87d159
    __ dci(0x45caf0d8);  // sri z24.d, z6.d, #22
    // vl128 state = 0x3c44b845
    __ dci(0x45c2f2dc);  // sri z28.d, z22.d, #30
    // vl128 state = 0x9b8c17a7
    __ dci(0x45caf25d);  // sri z29.d, z18.d, #22
    // vl128 state = 0x3e2c1797
    __ dci(0x45caf0dc);  // sri z28.d, z6.d, #22
    // vl128 state = 0xbf933754
    __ dci(0x458af1cc);  // sri z12.d, z14.d, #54
    // vl128 state = 0x93e91a23
    __ dci(0x4586f1cd);  // sri z13.d, z14.d, #58
    // vl128 state = 0x0f7c6faa
    __ dci(0x458ef0cc);  // sri z12.d, z6.d, #50
    // vl128 state = 0x1d771f71
    __ dci(0x458ef00d);  // sri z13.d, z0.d, #50
    // vl128 state = 0x29a23da7
    __ dci(0x450ef05d);  // sri z29.b, z2.b, #2
    // vl128 state = 0x74fd2038
    __ dci(0x450cf00d);  // sri z13.b, z0.b, #4
    // vl128 state = 0x075bc166
    __ dci(0x450cf00c);  // sri z12.b, z0.b, #4
    // vl128 state = 0xfd3d290f
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xfd3d290f,
        0x8dd0bdab,
        0xa25ba843,
        0x484543ed,
        0x22df2f4f,
        0xb62769dc,
        0x795e30f7,
        0xe49948e7,
        0xd4ceb676,
        0xbf2d359a,
        0xcf4331a9,
        0x8cce4eef,
        0x4fbaec97,
        0x4fec4d88,
        0x3efc521d,
        0xffef31d1,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_srshr_urshr) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x04cc9074);  // srshr z20.d, p4/m, z20.d, #29
    // vl128 state = 0xecefbcaa
    __ dci(0x04cc9236);  // srshr z22.d, p4/m, z22.d, #15
    // vl128 state = 0x7eef75c3
    __ dci(0x04cd927e);  // urshr z30.d, p4/m, z30.d, #13
    // vl128 state = 0xf5ab0a43
    __ dci(0x04cd9e76);  // urshr z22.d, p7/m, z22.d, #13
    // vl128 state = 0x67a9d15a
    __ dci(0x04cd9a57);  // urshr z23.d, p6/m, z23.d, #14
    // vl128 state = 0xf1591f3f
    __ dci(0x044d9247);  // urshr z7.s, p4/m, z7.s, #14
    // vl128 state = 0xcb770d03
    __ dci(0x044d9245);  // urshr z5.s, p4/m, z5.s, #14
    // vl128 state = 0x7a225c92
    __ dci(0x044d9241);  // urshr z1.s, p4/m, z1.s, #14
    // vl128 state = 0x31e4f59a
    __ dci(0x044d8200);  // urshr z0.s, p0/m, z0.s, #16
    // vl128 state = 0x7c0c67fa
    __ dci(0x044d8330);  // urshr z16.s, p0/m, z16.s, #7
    // vl128 state = 0x2aaa996d
    __ dci(0x044d8340);  // urshr z0.s, p0/m, z0.s, #6
    // vl128 state = 0x1999a541
    __ dci(0x044d8104);  // urshr z4.s, p0/m, z4.s, #24
    // vl128 state = 0xbebc22f3
    __ dci(0x044d8526);  // urshr z6.s, p1/m, z6.s, #23
    // vl128 state = 0x5e9c818d
    __ dci(0x04cd8502);  // urshr z2.d, p1/m, z2.d, #24
    // vl128 state = 0x9cd88e00
    __ dci(0x048d9506);  // urshr z6.d, p5/m, z6.d, #56
    // vl128 state = 0xff60a16e
    __ dci(0x048d9504);  // urshr z4.d, p5/m, z4.d, #56
    // vl128 state = 0xfae64bf4
    __ dci(0x048d8705);  // urshr z5.d, p1/m, z5.d, #40
    // vl128 state = 0xbd7bc8bb
    __ dci(0x048d9307);  // urshr z7.d, p4/m, z7.d, #40
    // vl128 state = 0x22e58729
    __ dci(0x048c9323);  // srshr z3.d, p4/m, z3.d, #39
    // vl128 state = 0x1a2b90d1
    __ dci(0x048c8721);  // srshr z1.d, p1/m, z1.d, #39
    // vl128 state = 0xf31798ea
    __ dci(0x04cc8f20);  // srshr z0.d, p3/m, z0.d, #7
    // vl128 state = 0x3a159e41
    __ dci(0x04cc87b0);  // srshr z16.d, p1/m, z16.d, #3
    // vl128 state = 0x461819c6
    __ dci(0x04cc8778);  // srshr z24.d, p1/m, z24.d, #5
    // vl128 state = 0x52c8c945
    __ dci(0x048c8730);  // srshr z16.d, p1/m, z16.d, #39
    // vl128 state = 0xa6724c16
    __ dci(0x040c8534);  // srshr z20.b, p1/m, z20.b, #7
    // vl128 state = 0xfeae5ea1
    __ dci(0x040c957c);  // srshr z28.b, p5/m, z28.b, #5
    // vl128 state = 0xe55cac9f
    __ dci(0x048c9554);  // srshr z20.d, p5/m, z20.d, #54
    // vl128 state = 0x41ccbe50
    __ dci(0x048c8156);  // srshr z22.d, p0/m, z22.d, #54
    // vl128 state = 0xfef5c71e
    __ dci(0x040c8957);  // srshr z23.b, p2/m, z23.b, #6
    // vl128 state = 0xac8cf177
    __ dci(0x040c8bd5);  // srshr z21.h, p2/m, z21.h, #2
    // vl128 state = 0xfe7005fe
    __ dci(0x040c8354);  // srshr z20.h, p0/m, z20.h, #6
    // vl128 state = 0x1daa6598
    __ dci(0x040c931c);  // srshr z28.h, p4/m, z28.h, #8
    // vl128 state = 0x8c7f2675
    __ dci(0x040c9798);  // srshr z24.h, p5/m, z24.h, #4
    // vl128 state = 0x2349e927
    __ dci(0x044c97ba);  // srshr z26.s, p5/m, z26.s, #3
    // vl128 state = 0xf3670053
    __ dci(0x040c9faa);  // srshr z10.h, p7/m, z10.h, #3
    // vl128 state = 0x61333578
    __ dci(0x044d9fae);  // urshr z14.s, p7/m, z14.s, #3
    // vl128 state = 0xdb1232a3
    __ dci(0x044d8f8f);  // urshr z15.s, p3/m, z15.s, #4
    // vl128 state = 0xb1b4bda1
    __ dci(0x044d8f87);  // urshr z7.s, p3/m, z7.s, #4
    // vl128 state = 0xba636ab8
    __ dci(0x044d9d97);  // urshr z23.s, p7/m, z23.s, #20
    // vl128 state = 0x8ab01b49
    __ dci(0x040d9593);  // urshr z19.b, p5/m, z19.b, #4
    // vl128 state = 0x20ee49b4
    __ dci(0x040d959b);  // urshr z27.b, p5/m, z27.b, #4
    // vl128 state = 0xe34dcf2e
    __ dci(0x044c959a);  // srshr z26.s, p5/m, z26.s, #20
    // vl128 state = 0x65bafb28
    __ dci(0x044d9492);  // urshr z18.s, p5/m, z18.s, #28
    // vl128 state = 0xcbed1382
    __ dci(0x044c8493);  // srshr z19.s, p1/m, z19.s, #28
    // vl128 state = 0xa54fb84c
    __ dci(0x044c8cc3);  // srshr z3.s, p3/m, z3.s, #26
    // vl128 state = 0x257267ee
    __ dci(0x044c8c0b);  // srshr z11.s, p3/m, z11.s, #32
    // vl128 state = 0xd494a3e8
    __ dci(0x044c8c6f);  // srshr z15.s, p3/m, z15.s, #29
    // vl128 state = 0x63621477
    __ dci(0x044c9c2e);  // srshr z14.s, p7/m, z14.s, #31
    // vl128 state = 0x4cb2e888
    __ dci(0x04cc943e);  // srshr z30.d, p5/m, z30.d, #31
    // vl128 state = 0x8e580ba2
    __ dci(0x04cd953f);  // urshr z31.d, p5/m, z31.d, #23
    // vl128 state = 0x7678cc05
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x7678cc05,
        0x37f2893a,
        0xce2a105d,
        0x5a03f5a3,
        0x81444dfc,
        0x5581c0c1,
        0xfee622cc,
        0x0f6796a5,
        0xf151a5fd,
        0x13e9be9c,
        0x9685f8b5,
        0xa6827285,
        0x7ad6d004,
        0xba7989ae,
        0x96fe2826,
        0xd1ddc17e,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_sqshl_uqshl) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x044f86aa);  // sqshlu z10.s, p1/m, z10.s, #21
    // vl128 state = 0x37777991
    __ dci(0x044f8482);  // sqshlu z2.s, p1/m, z2.s, #4
    // vl128 state = 0x8119dd5a
    __ dci(0x048f8480);  // sqshlu z0.d, p1/m, z0.d, #4
    // vl128 state = 0x8966cd23
    __ dci(0x04cf8c82);  // sqshlu z2.d, p3/m, z2.d, #36
    // vl128 state = 0x71b53135
    __ dci(0x044f8892);  // sqshlu z18.s, p2/m, z18.s, #4
    // vl128 state = 0x44e0e9a7
    __ dci(0x04cf8996);  // sqshlu z22.d, p2/m, z22.d, #44
    // vl128 state = 0x4e4b77b9
    __ dci(0x04cf9194);  // sqshlu z20.d, p4/m, z20.d, #44
    // vl128 state = 0x66d72728
    __ dci(0x04cf9b9c);  // sqshlu z28.d, p6/m, z28.d, #60
    // vl128 state = 0xa80f62ce
    __ dci(0x04c79f8c);  // uqshl z12.d, p7/m, z12.d, #60
    // vl128 state = 0x87a3a8c0
    __ dci(0x04469f88);  // sqshl z8.s, p7/m, z8.s, #28
    // vl128 state = 0x3db302cb
    __ dci(0x04469f8a);  // sqshl z10.s, p7/m, z10.s, #28
    // vl128 state = 0x2d66bbb2
    __ dci(0x04469a8e);  // sqshl z14.s, p6/m, z14.s, #20
    // vl128 state = 0x39524732
    __ dci(0x04c69a1e);  // sqshl z30.d, p6/m, z30.d, #48
    // vl128 state = 0x39d71433
    __ dci(0x04c68a9a);  // sqshl z26.d, p2/m, z26.d, #52
    // vl128 state = 0x58771cfb
    __ dci(0x04469a8a);  // sqshl z10.s, p6/m, z10.s, #20
    // vl128 state = 0xa773fcc9
    __ dci(0x04c68a88);  // sqshl z8.d, p2/m, z8.d, #52
    // vl128 state = 0x9dce801c
    __ dci(0x04469a89);  // sqshl z9.s, p6/m, z9.s, #20
    // vl128 state = 0x4141302f
    __ dci(0x04479b81);  // uqshl z1.s, p6/m, z1.s, #28
    // vl128 state = 0x369084f9
    __ dci(0x044f9f91);  // sqshlu z17.s, p7/m, z17.s, #28
    // vl128 state = 0x1570bb90
    __ dci(0x04479e90);  // uqshl z16.s, p7/m, z16.s, #20
    // vl128 state = 0x27765662
    __ dci(0x044f9f94);  // sqshlu z20.s, p7/m, z20.s, #28
    // vl128 state = 0xe99bcbb9
    __ dci(0x04479795);  // uqshl z21.s, p5/m, z21.s, #28
    // vl128 state = 0xb36c3b9f
    __ dci(0x04479754);  // uqshl z20.s, p5/m, z20.s, #26
    // vl128 state = 0x435e0256
    __ dci(0x04479750);  // uqshl z16.s, p5/m, z16.s, #26
    // vl128 state = 0x485471e9
    __ dci(0x04479740);  // uqshl z0.s, p5/m, z0.s, #26
    // vl128 state = 0x170e10cb
    __ dci(0x04079544);  // uqshl z4.b, p5/m, z4.b, #2
    // vl128 state = 0x026fe32a
    __ dci(0x04c79546);  // uqshl z6.d, p5/m, z6.d, #42
    // vl128 state = 0x9a92b063
    __ dci(0x04c78504);  // uqshl z4.d, p1/m, z4.d, #40
    // vl128 state = 0x4e9a105e
    __ dci(0x04879500);  // uqshl z0.d, p5/m, z0.d, #8
    // vl128 state = 0x958b4d28
    __ dci(0x04879908);  // uqshl z8.d, p6/m, z8.d, #8
    // vl128 state = 0x420ff82d
    __ dci(0x04879318);  // uqshl z24.d, p4/m, z24.d, #24
    // vl128 state = 0x88002097
    __ dci(0x0487931a);  // uqshl z26.d, p4/m, z26.d, #24
    // vl128 state = 0x3047401c
    __ dci(0x0486938a);  // sqshl z10.d, p4/m, z10.d, #28
    // vl128 state = 0x5b2b7938
    __ dci(0x04069188);  // sqshl z8.b, p4/m, z8.b, #4
    // vl128 state = 0xb92dd260
    __ dci(0x04469389);  // sqshl z9.s, p4/m, z9.s, #28
    // vl128 state = 0xdc6370c3
    __ dci(0x0447918b);  // uqshl z11.s, p4/m, z11.s, #12
    // vl128 state = 0x5e6198f0
    __ dci(0x0447913b);  // uqshl z27.s, p4/m, z27.s, #9
    // vl128 state = 0x935ed2a3
    __ dci(0x0447915f);  // uqshl z31.s, p4/m, z31.s, #10
    // vl128 state = 0x76271654
    __ dci(0x0406915d);  // sqshl z29.b, p4/m, z29.b, #2
    // vl128 state = 0x46a71ae3
    __ dci(0x0486911f);  // sqshl z31.d, p4/m, z31.d, #8
    // vl128 state = 0x2c7320a6
    __ dci(0x0486911d);  // sqshl z29.d, p4/m, z29.d, #8
    // vl128 state = 0x4aa0022d
    __ dci(0x04869b1f);  // sqshl z31.d, p6/m, z31.d, #24
    // vl128 state = 0x2de081d7
    __ dci(0x04069317);  // sqshl z23.h, p4/m, z23.h, #8
    // vl128 state = 0x879c9ead
    __ dci(0x0447931f);  // uqshl z31.s, p4/m, z31.s, #24
    // vl128 state = 0x51070552
    __ dci(0x04479b9e);  // uqshl z30.s, p6/m, z30.s, #28
    // vl128 state = 0x8cc26b2b
    __ dci(0x04479adf);  // uqshl z31.s, p6/m, z31.s, #22
    // vl128 state = 0x8f4512d3
    __ dci(0x04479adb);  // uqshl z27.s, p6/m, z27.s, #22
    // vl128 state = 0x3d44e050
    __ dci(0x04079a99);  // uqshl z25.h, p6/m, z25.h, #4
    // vl128 state = 0xede0c288
    __ dci(0x04079a89);  // uqshl z9.h, p6/m, z9.h, #4
    // vl128 state = 0x928beed6
    __ dci(0x04879acb);  // uqshl z11.d, p6/m, z11.d, #22
    // vl128 state = 0x6945e18a
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x6945e18a,
        0x0e954f70,
        0x3d269eb2,
        0xefeb5acb,
        0xfb27cb0c,
        0x651a1aea,
        0x07011083,
        0xd425418b,
        0xa0e026c6,
        0x407c416e,
        0x14e25761,
        0x21eef576,
        0xc6ad09eb,
        0x3642006b,
        0xdebec165,
        0x24ae8a32,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_unsigned_sat_round_shift) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 100 * kInstructionSize);
    __ dci(0x44cb84cb);  // uqrshl z11.d, p1/m, z11.d, z6.d
    // vl128 state = 0x9794ef4a
    __ dci(0x444b85db);  // uqrshl z27.h, p1/m, z27.h, z14.h
    // vl128 state = 0xda137fcc
    __ dci(0x444b874b);  // uqrshl z11.h, p1/m, z11.h, z26.h
    // vl128 state = 0xafc1533b
    __ dci(0x444b87fb);  // uqrshl z27.h, p1/m, z27.h, z31.h
    // vl128 state = 0x228890a2
    __ dci(0x444b87f3);  // uqrshl z19.h, p1/m, z19.h, z31.h
    // vl128 state = 0x5cb0d356
    __ dci(0x444385f1);  // urshl z17.h, p1/m, z17.h, z15.h
    // vl128 state = 0xbb6b6d1d
    __ dci(0x444795f3);  // urshlr z19.h, p5/m, z19.h, z15.h
    // vl128 state = 0x98b43358
    __ dci(0x44479552);  // urshlr z18.h, p5/m, z18.h, z10.h
    // vl128 state = 0x472880b2
    __ dci(0x44c79502);  // urshlr z2.d, p5/m, z2.d, z8.d
    // vl128 state = 0x0995d86f
    __ dci(0x44879406);  // urshlr z6.s, p5/m, z6.s, z0.s
    // vl128 state = 0x405211cd
    __ dci(0x44079436);  // urshlr z22.b, p5/m, z22.b, z1.b
    // vl128 state = 0x563647b0
    __ dci(0x44078c34);  // urshlr z20.b, p3/m, z20.b, z1.b
    // vl128 state = 0x2eacf2d3
    __ dci(0x440f843c);  // uqrshlr z28.b, p1/m, z28.b, z1.b
    // vl128 state = 0x56f472ce
    __ dci(0x440f8cbe);  // uqrshlr z30.b, p3/m, z30.b, z5.b
    // vl128 state = 0x910ce8d0
    __ dci(0x44078eba);  // urshlr z26.b, p3/m, z26.b, z21.b
    // vl128 state = 0xc47b6482
    __ dci(0x44078ebe);  // urshlr z30.b, p3/m, z30.b, z21.b
    // vl128 state = 0xff805975
    __ dci(0x440f86b6);  // uqrshlr z22.b, p1/m, z22.b, z21.b
    // vl128 state = 0x132fe792
    __ dci(0x444b86b7);  // uqrshl z23.h, p1/m, z23.h, z21.h
    // vl128 state = 0xabd3d85c
    __ dci(0x440b84a7);  // uqrshl z7.b, p1/m, z7.b, z5.b
    // vl128 state = 0x8f718992
    __ dci(0x440b8085);  // uqrshl z5.b, p0/m, z5.b, z4.b
    // vl128 state = 0x1b05e694
    __ dci(0x440b8687);  // uqrshl z7.b, p1/m, z7.b, z20.b
    // vl128 state = 0xd9a0c225
    __ dci(0x440986cf);  // uqshl z15.b, p1/m, z15.b, z22.b
    // vl128 state = 0x98be170a
    __ dci(0x440b87ce);  // uqrshl z14.b, p1/m, z14.b, z30.b
    // vl128 state = 0x0993d862
    __ dci(0x440b838c);  // uqrshl z12.b, p0/m, z12.b, z28.b
    // vl128 state = 0xbc95a037
    __ dci(0x440b839c);  // uqrshl z28.b, p0/m, z28.b, z28.b
    // vl128 state = 0x558159d9
    __ dci(0x444b8314);  // uqrshl z20.h, p0/m, z20.h, z24.h
    // vl128 state = 0x53798c6b
    __ dci(0x44498b1c);  // uqshl z28.h, p2/m, z28.h, z24.h
    // vl128 state = 0x83db6a7c
    __ dci(0x44498b0c);  // uqshl z12.h, p2/m, z12.h, z24.h
    // vl128 state = 0x62bda6cb
    __ dci(0x44438b0e);  // urshl z14.h, p2/m, z14.h, z24.h
    // vl128 state = 0xc04356eb
    __ dci(0x44438986);  // urshl z6.h, p2/m, z6.h, z12.h
    // vl128 state = 0x0e2e6682
    __ dci(0x444389e4);  // urshl z4.h, p2/m, z4.h, z15.h
    // vl128 state = 0xbb28cacd
    __ dci(0x444391f4);  // urshl z20.h, p4/m, z20.h, z15.h
    // vl128 state = 0x5349f37a
    __ dci(0x444391f6);  // urshl z22.h, p4/m, z22.h, z15.h
    // vl128 state = 0x99e66890
    __ dci(0x44c39177);  // urshl z23.d, p4/m, z23.d, z11.d
    // vl128 state = 0x2d48a891
    __ dci(0x44c79573);  // urshlr z19.d, p5/m, z19.d, z11.d
    // vl128 state = 0xd26e94f9
    __ dci(0x04c79d63);  // uqshl z3.d, p7/m, z3.d, #43
    // vl128 state = 0x54801050
    __ dci(0x04c78c67);  // uqshl z7.d, p3/m, z7.d, #35
    // vl128 state = 0xde9f357a
    __ dci(0x04878c43);  // uqshl z3.d, p3/m, z3.d, #2
    // vl128 state = 0x59e5d53c
    __ dci(0x44878c0b);  // urshlr z11.s, p3/m, z11.s, z0.s
    // vl128 state = 0x8cfa7532
    __ dci(0x44878c03);  // urshlr z3.s, p3/m, z3.s, z0.s
    // vl128 state = 0xdb4e86b6
    __ dci(0x44878d42);  // urshlr z2.s, p3/m, z2.s, z10.s
    // vl128 state = 0x07467a7c
    __ dci(0x44878d4a);  // urshlr z10.s, p3/m, z10.s, z10.s
    // vl128 state = 0x6a4ad81c
    __ dci(0x44879948);  // urshlr z8.s, p6/m, z8.s, z10.s
    // vl128 state = 0x91d7bdc0
    __ dci(0x44879949);  // urshlr z9.s, p6/m, z9.s, z10.s
    // vl128 state = 0x2fe3b819
    __ dci(0x44879bcb);  // urshlr z11.s, p6/m, z11.s, z30.s
    // vl128 state = 0x5c121b68
    __ dci(0x04879b4f);  // uqshl z15.d, p6/m, z15.d, #26
    // vl128 state = 0xe678f4f7
    __ dci(0x44879bdf);  // urshlr z31.s, p6/m, z31.s, z30.s
    // vl128 state = 0x6593da76
    __ dci(0x4487935e);  // urshlr z30.s, p4/m, z30.s, z26.s
    // vl128 state = 0xb558ba57
    __ dci(0x440f9356);  // uqrshlr z22.b, p4/m, z22.b, z26.b
    // vl128 state = 0x45d1775e
    __ dci(0x440f93f7);  // uqrshlr z23.b, p4/m, z23.b, z31.b
    // vl128 state = 0x20974795
    __ dci(0x448793f5);  // urshlr z21.s, p4/m, z21.s, z31.s
    // vl128 state = 0xeb0bc2ab
    __ dci(0x448383fd);  // urshl z29.s, p0/m, z29.s, z31.s
    // vl128 state = 0x74557d81
    __ dci(0x448b82f9);  // uqrshl z25.s, p0/m, z25.s, z23.s
    // vl128 state = 0x34518418
    __ dci(0x448f82b8);  // uqrshlr z24.s, p0/m, z24.s, z21.s
    // vl128 state = 0x93e637f3
    __ dci(0x448f82bc);  // uqrshlr z28.s, p0/m, z28.s, z21.s
    // vl128 state = 0x6e35e56a
    __ dci(0x448f83fe);  // uqrshlr z30.s, p0/m, z30.s, z31.s
    // vl128 state = 0xf3c59bb1
    __ dci(0x448d83ae);  // uqshlr z14.s, p0/m, z14.s, z29.s
    // vl128 state = 0x95b401a3
    __ dci(0x448d83aa);  // uqshlr z10.s, p0/m, z10.s, z29.s
    // vl128 state = 0x56ec65b0
    __ dci(0x448993ae);  // uqshl z14.s, p4/m, z14.s, z29.s
    // vl128 state = 0x28f6e4c6
    __ dci(0x448993a6);  // uqshl z6.s, p4/m, z6.s, z29.s
    // vl128 state = 0x9ed5eaf3
    __ dci(0x44c991a4);  // uqshl z4.d, p4/m, z4.d, z13.d
    // vl128 state = 0xa8512b00
    __ dci(0x44c991a5);  // uqshl z5.d, p4/m, z5.d, z13.d
    // vl128 state = 0x49a10780
    __ dci(0x44c991a1);  // uqshl z1.d, p4/m, z1.d, z13.d
    // vl128 state = 0x465a2cb4
    __ dci(0x444b91a0);  // uqrshl z0.h, p4/m, z0.h, z13.h
    // vl128 state = 0x8f6dad8e
    __ dci(0x444b91a1);  // uqrshl z1.h, p4/m, z1.h, z13.h
    // vl128 state = 0x50dec3f8
    __ dci(0x440391a3);  // urshl z3.b, p4/m, z3.b, z13.b
    // vl128 state = 0xab2b5ad7
    __ dci(0x448393a7);  // urshl z7.s, p4/m, z7.s, z29.s
    // vl128 state = 0x2ffd164f
    __ dci(0x448393af);  // urshl z15.s, p4/m, z15.s, z29.s
    // vl128 state = 0x43a7959b
    __ dci(0x448393ab);  // urshl z11.s, p4/m, z11.s, z29.s
    // vl128 state = 0xf9526723
    __ dci(0x448f93af);  // uqrshlr z15.s, p4/m, z15.s, z29.s
    // vl128 state = 0xf9081b27
    __ dci(0x448f93ae);  // uqrshlr z14.s, p4/m, z14.s, z29.s
    // vl128 state = 0x3a4f693e
    __ dci(0x048793aa);  // uqshl z10.d, p4/m, z10.d, #29
    // vl128 state = 0xbba37d9a
    __ dci(0x04c79388);  // uqshl z8.d, p4/m, z8.d, #60
    // vl128 state = 0x3b3f5fa4
    __ dci(0x04c79380);  // uqshl z0.d, p4/m, z0.d, #60
    // vl128 state = 0xdac48ac2
    __ dci(0x04878390);  // uqshl z16.d, p0/m, z16.d, #28
    // vl128 state = 0xe3c8148f
    __ dci(0x44878794);  // urshlr z20.s, p1/m, z20.s, z28.s
    // vl128 state = 0xee2179ec
    __ dci(0x04878384);  // uqshl z4.d, p0/m, z4.d, #28
    // vl128 state = 0xc6a3796c
    __ dci(0x048787ac);  // uqshl z12.d, p1/m, z12.d, #29
    // vl128 state = 0x18e0fd43
    __ dci(0x04c786ae);  // uqshl z14.d, p1/m, z14.d, #53
    // vl128 state = 0x9292503e
    __ dci(0x04c786be);  // uqshl z30.d, p1/m, z30.d, #53
    // vl128 state = 0xc1ebe042
    __ dci(0x44c782b6);  // urshlr z22.d, p0/m, z22.d, z21.d
    // vl128 state = 0x0badc025
    __ dci(0x44c78a3e);  // urshlr z30.d, p2/m, z30.d, z17.d
    // vl128 state = 0x51b3b5ac
    __ dci(0x04c78b3a);  // uqshl z26.d, p2/m, z26.d, #57
    // vl128 state = 0x334f52f8
    __ dci(0x04c78832);  // uqshl z18.d, p2/m, z18.d, #33
    // vl128 state = 0xf95df0b7
    __ dci(0x44cf8833);  // uqrshlr z19.d, p2/m, z19.d, z1.d
    // vl128 state = 0xda88a00a
    __ dci(0x44cf9811);  // uqrshlr z17.d, p6/m, z17.d, z0.d
    // vl128 state = 0x1e642a4c
    __ dci(0x44cf9c41);  // uqrshlr z1.d, p7/m, z1.d, z2.d
    // vl128 state = 0xeb7fe4bd
    __ dci(0x444f8c45);  // uqrshlr z5.h, p3/m, z5.h, z2.h
    // vl128 state = 0x5a82d833
    __ dci(0x44cf844d);  // uqrshlr z13.d, p1/m, z13.d, z2.d
    // vl128 state = 0x595d42a4
    __ dci(0x44c7841d);  // urshlr z29.d, p1/m, z29.d, z0.d
    // vl128 state = 0x0b433688
    __ dci(0x44c7805f);  // urshlr z31.d, p0/m, z31.d, z2.d
    // vl128 state = 0x14b8c29a
    __ dci(0x44cf807b);  // uqrshlr z27.d, p0/m, z27.d, z3.d
    // vl128 state = 0x12a76015
    __ dci(0x44c780eb);  // urshlr z11.d, p0/m, z11.d, z7.d
    // vl128 state = 0x73fa7d24
    __ dci(0x44c794e3);  // urshlr z3.d, p5/m, z3.d, z7.d
    // vl128 state = 0x0a01c859
    __ dci(0x04c795eb);  // uqshl z11.d, p5/m, z11.d, #47
    // vl128 state = 0x0e7024fd
    __ dci(0x04c795e9);  // uqshl z9.d, p5/m, z9.d, #47
    // vl128 state = 0x9ca5cb63
    __ dci(0x04c795f9);  // uqshl z25.d, p5/m, z25.d, #47
    // vl128 state = 0x4c60da07
    __ dci(0x04c795fb);  // uqshl z27.d, p5/m, z27.d, #47
    // vl128 state = 0x71114c19
    __ dci(0x04c799f3);  // uqshl z19.d, p6/m, z19.d, #47
    // vl128 state = 0x32d71e12
    __ dci(0x04c79997);  // uqshl z23.d, p6/m, z23.d, #44
    // vl128 state = 0xab0c9051
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xab0c9051,
        0xc2455013,
        0x6e4b3f1e,
        0x631ce7ed,
        0x031e4f7f,
        0xa2be23bd,
        0x2f5f74b0,
        0x9e60f1ea,
        0xb1080595,
        0x953020c9,
        0x7a5bfffb,
        0xf0a27817,
        0x83904886,
        0x04620572,
        0xbcd5c8c9,
        0x3d4abe12,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_signed_sat_round_shift) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 100 * kInstructionSize);
    __ dci(0x048687c6);  // sqshl z6.d, p1/m, z6.d, #30
    // vl128 state = 0xe81d8487
    __ dci(0x048687c4);  // sqshl z4.d, p1/m, z4.d, #30
    // vl128 state = 0x47cc69b1
    __ dci(0x04868385);  // sqshl z5.d, p0/m, z5.d, #28
    // vl128 state = 0xec4cab7b
    __ dci(0x0486838d);  // sqshl z13.d, p0/m, z13.d, #28
    // vl128 state = 0x23b07ac8
    __ dci(0x048681a9);  // sqshl z9.d, p0/m, z9.d, #13
    // vl128 state = 0xace4253d
    __ dci(0x04068139);  // sqshl z25.b, p0/m, z25.b, #1
    // vl128 state = 0xf8f14a80
    __ dci(0x440681b8);  // srshlr z24.b, p0/m, z24.b, z13.b
    // vl128 state = 0xa79d8fc1
    __ dci(0x4406803a);  // srshlr z26.b, p0/m, z26.b, z1.b
    // vl128 state = 0xed9bb777
    __ dci(0x4406808a);  // srshlr z10.b, p0/m, z10.b, z4.b
    // vl128 state = 0xbd1dfa2f
    __ dci(0x440688da);  // srshlr z26.b, p2/m, z26.b, z6.b
    // vl128 state = 0x8f9b61e6
    __ dci(0x448680db);  // srshlr z27.s, p0/m, z27.s, z6.s
    // vl128 state = 0x0a16f551
    __ dci(0x440684d3);  // srshlr z19.b, p1/m, z19.b, z6.b
    // vl128 state = 0x0a764f12
    __ dci(0x448694c3);  // srshlr z3.s, p5/m, z3.s, z6.s
    // vl128 state = 0x8d6f5613
    __ dci(0x448e9cc7);  // sqrshlr z7.s, p7/m, z7.s, z6.s
    // vl128 state = 0xaf7b559b
    __ dci(0x448e9ef7);  // sqrshlr z23.s, p7/m, z23.s, z23.s
    // vl128 state = 0x086d6430
    __ dci(0x448e9673);  // sqrshlr z19.s, p5/m, z19.s, z19.s
    // vl128 state = 0x4a9a5736
    __ dci(0x448a8663);  // sqrshl z3.s, p1/m, z3.s, z19.s
    // vl128 state = 0x19adf50e
    __ dci(0x440a8e6b);  // sqrshl z11.b, p3/m, z11.b, z19.b
    // vl128 state = 0x4a01719c
    __ dci(0x44028eef);  // srshl z15.b, p3/m, z15.b, z23.b
    // vl128 state = 0x1af6d72e
    __ dci(0x44028e8b);  // srshl z11.b, p3/m, z11.b, z20.b
    // vl128 state = 0xeca2061d
    __ dci(0x44828f8f);  // srshl z15.s, p3/m, z15.s, z28.s
    // vl128 state = 0x61059832
    __ dci(0x44828f87);  // srshl z7.s, p3/m, z7.s, z28.s
    // vl128 state = 0x5e4d94cc
    __ dci(0x44828a97);  // srshl z23.s, p2/m, z23.s, z20.s
    // vl128 state = 0xf5095aa8
    __ dci(0x44828a93);  // srshl z19.s, p2/m, z19.s, z20.s
    // vl128 state = 0x155ff234
    __ dci(0x44868a11);  // srshlr z17.s, p2/m, z17.s, z16.s
    // vl128 state = 0xf2844c7f
    __ dci(0x44c68a90);  // srshlr z16.d, p2/m, z16.d, z20.d
    // vl128 state = 0xcf9f9508
    __ dci(0x44c68a80);  // srshlr z0.d, p2/m, z0.d, z20.d
    // vl128 state = 0xd476915b
    __ dci(0x44868a02);  // srshlr z2.s, p2/m, z2.s, z16.s
    // vl128 state = 0x9acbc986
    __ dci(0x44868a12);  // srshlr z18.s, p2/m, z18.s, z16.s
    // vl128 state = 0xaf9e1114
    __ dci(0x4486921a);  // srshlr z26.s, p4/m, z26.s, z16.s
    // vl128 state = 0x9d188add
    __ dci(0x4486909e);  // srshlr z30.s, p4/m, z30.s, z4.s
    // vl128 state = 0xb41018d5
    __ dci(0x448c9096);  // sqshlr z22.s, p4/m, z22.s, z4.s
    // vl128 state = 0x4ab51dea
    __ dci(0x448890b4);  // sqshl z20.s, p4/m, z20.s, z5.s
    // vl128 state = 0x600dcc36
    __ dci(0x448884bc);  // sqshl z28.s, p1/m, z28.s, z5.s
    // vl128 state = 0x84f37050
    __ dci(0x44c88434);  // sqshl z20.d, p1/m, z20.d, z1.d
    // vl128 state = 0x1f19ce5a
    __ dci(0x44cc8536);  // sqshlr z22.d, p1/m, z22.d, z9.d
    // vl128 state = 0xa51d3f31
    __ dci(0x448c8517);  // sqshlr z23.s, p1/m, z23.s, z8.s
    // vl128 state = 0x8d431292
    __ dci(0x448c8133);  // sqshlr z19.s, p0/m, z19.s, z9.s
    // vl128 state = 0xdd59917f
    __ dci(0x448c8b23);  // sqshlr z3.s, p2/m, z3.s, z25.s
    // vl128 state = 0xfcdae7d4
    __ dci(0x448c8b21);  // sqshlr z1.s, p2/m, z1.s, z25.s
    // vl128 state = 0x0f1239a5
    __ dci(0x448c8b29);  // sqshlr z9.s, p2/m, z9.s, z25.s
    // vl128 state = 0xf6d1f180
    __ dci(0x448c8b2b);  // sqshlr z11.s, p2/m, z11.s, z25.s
    // vl128 state = 0xe7a1af08
    __ dci(0x448c8b89);  // sqshlr z9.s, p2/m, z9.s, z28.s
    // vl128 state = 0xa72666cb
    __ dci(0x448c9bcb);  // sqshlr z11.s, p6/m, z11.s, z30.s
    // vl128 state = 0x9cae5fd7
    __ dci(0x44869bca);  // srshlr z10.s, p6/m, z10.s, z30.s
    // vl128 state = 0xda133b76
    __ dci(0x04869b8e);  // sqshl z14.d, p6/m, z14.d, #28
    // vl128 state = 0xf8eb71c2
    __ dci(0x44869bca);  // srshlr z10.s, p6/m, z10.s, z30.s
    // vl128 state = 0xbe561563
    __ dci(0x44869ae2);  // srshlr z2.s, p6/m, z2.s, z23.s
    // vl128 state = 0x0c286f7e
    __ dci(0x44869a46);  // srshlr z6.s, p6/m, z6.s, z18.s
    // vl128 state = 0x59da6464
    __ dci(0x44869a47);  // srshlr z7.s, p6/m, z7.s, z18.s
    // vl128 state = 0x908e5664
    __ dci(0x4486920f);  // srshlr z15.s, p4/m, z15.s, z16.s
    // vl128 state = 0x213d23db
    __ dci(0x44869a87);  // srshlr z7.s, p6/m, z7.s, z20.s
    // vl128 state = 0xd81ea7fb
    __ dci(0x44469a86);  // srshlr z6.h, p6/m, z6.h, z20.h
    // vl128 state = 0x27d44726
    __ dci(0x44029a82);  // srshl z2.b, p6/m, z2.b, z20.b
    // vl128 state = 0x2187127f
    __ dci(0x44069aa0);  // srshlr z0.b, p6/m, z0.b, z21.b
    // vl128 state = 0x68ba9323
    __ dci(0x444692b0);  // srshlr z16.h, p4/m, z16.h, z21.h
    // vl128 state = 0x148619ff
    __ dci(0x44468ab2);  // srshlr z18.h, p2/m, z18.h, z21.h
    // vl128 state = 0xae93eae6
    __ dci(0x444698b6);  // srshlr z22.h, p6/m, z22.h, z5.h
    // vl128 state = 0x0b875035
    __ dci(0x44469934);  // srshlr z20.h, p6/m, z20.h, z9.h
    // vl128 state = 0x559132ed
    __ dci(0x0406993c);  // sqshl z28.b, p6/m, z28.b, #1
    // vl128 state = 0xec1782e4
    __ dci(0x4406912c);  // srshlr z12.b, p4/m, z12.b, z9.b
    // vl128 state = 0x089d32a4
    __ dci(0x440291ae);  // srshl z14.b, p4/m, z14.b, z13.b
    // vl128 state = 0xde257893
    __ dci(0x44829126);  // srshl z6.s, p4/m, z6.s, z9.s
    // vl128 state = 0x318d27ef
    __ dci(0x448a8127);  // sqrshl z7.s, p0/m, z7.s, z9.s
    // vl128 state = 0x1bc564fc
    __ dci(0x448e8165);  // sqrshlr z5.s, p0/m, z5.s, z11.s
    // vl128 state = 0xa5e5c696
    __ dci(0x44869161);  // srshlr z1.s, p4/m, z1.s, z11.s
    // vl128 state = 0xd64b6830
    __ dci(0x44829120);  // srshl z0.s, p4/m, z0.s, z9.s
    // vl128 state = 0x107ca84d
    __ dci(0x44829124);  // srshl z4.s, p4/m, z4.s, z9.s
    // vl128 state = 0xcd5688f3
    __ dci(0x4482912c);  // srshl z12.s, p4/m, z12.s, z9.s
    // vl128 state = 0x88dee210
    __ dci(0x44829128);  // srshl z8.s, p4/m, z8.s, z9.s
    // vl128 state = 0xfe8611fa
    __ dci(0x44c69120);  // srshlr z0.d, p4/m, z0.d, z9.d
    // vl128 state = 0xe8b8cabd
    __ dci(0x44ce9168);  // sqrshlr z8.d, p4/m, z8.d, z11.d
    // vl128 state = 0x269af804
    __ dci(0x448e9069);  // sqrshlr z9.s, p4/m, z9.s, z3.s
    // vl128 state = 0x7d425704
    __ dci(0x448e8461);  // sqrshlr z1.s, p1/m, z1.s, z3.s
    // vl128 state = 0x1577bd67
    __ dci(0x448e8460);  // sqrshlr z0.s, p1/m, z0.s, z3.s
    // vl128 state = 0x6966617f
    __ dci(0x448a8428);  // sqrshl z8.s, p1/m, z8.s, z1.s
    // vl128 state = 0x6c9cc508
    __ dci(0x44ca8409);  // sqrshl z9.d, p1/m, z9.d, z0.d
    // vl128 state = 0xb3ea2e65
    __ dci(0x44c68408);  // srshlr z8.d, p1/m, z8.d, z0.d
    // vl128 state = 0x1aef7620
    __ dci(0x44c6840a);  // srshlr z10.d, p1/m, z10.d, z0.d
    // vl128 state = 0x63f2c5a3
    __ dci(0x44cc840e);  // sqshlr z14.d, p1/m, z14.d, z0.d
    // vl128 state = 0xb54a8f94
    __ dci(0x44cc8e1e);  // sqshlr z30.d, p3/m, z30.d, z16.d
    // vl128 state = 0xe247e0a3
    __ dci(0x44c68e1a);  // srshlr z26.d, p3/m, z26.d, z16.d
    // vl128 state = 0xfb8bf060
    __ dci(0x44c28a0a);  // srshl z10.d, p2/m, z10.d, z16.d
    // vl128 state = 0x829643e3
    __ dci(0x44c68e0e);  // srshlr z14.d, p3/m, z14.d, z16.d
    // vl128 state = 0x8bd62d7b
    __ dci(0x44c6881e);  // srshlr z30.d, p2/m, z30.d, z0.d
    // vl128 state = 0x4d8caca2
    __ dci(0x44869816);  // srshlr z22.s, p6/m, z22.s, z0.s
    // vl128 state = 0x027f41ac
    __ dci(0x44029817);  // srshl z23.b, p6/m, z23.b, z0.b
    // vl128 state = 0xab9c9627
    __ dci(0x4402993f);  // srshl z31.b, p6/m, z31.b, z9.b
    // vl128 state = 0x42a71056
    __ dci(0x4406991e);  // srshlr z30.b, p6/m, z30.b, z8.b
    // vl128 state = 0xdcdf1396
    __ dci(0x44068d1f);  // srshlr z31.b, p3/m, z31.b, z8.b
    // vl128 state = 0x84fa5cac
    __ dci(0x44068d1d);  // srshlr z29.b, p3/m, z29.b, z8.b
    // vl128 state = 0x1239cdae
    __ dci(0x44468d2d);  // srshlr z13.h, p3/m, z13.h, z9.h
    // vl128 state = 0xae689b2f
    __ dci(0x4446850f);  // srshlr z15.h, p1/m, z15.h, z8.h
    // vl128 state = 0x6330c9c2
    __ dci(0x4446910e);  // srshlr z14.h, p4/m, z14.h, z8.h
    // vl128 state = 0x326ffb9f
    __ dci(0x4446940f);  // srshlr z15.h, p5/m, z15.h, z0.h
    // vl128 state = 0x3f48f466
    __ dci(0x44468487);  // srshlr z7.h, p1/m, z7.h, z4.h
    // vl128 state = 0x0d3b6c65
    __ dci(0x444694b7);  // srshlr z23.h, p5/m, z23.h, z5.h
    // vl128 state = 0x5ef21cd8
    __ dci(0x44469c93);  // srshlr z19.h, p7/m, z19.h, z4.h
    // vl128 state = 0x413d5573
    __ dci(0x44069e92);  // srshlr z18.b, p7/m, z18.b, z20.b
    // vl128 state = 0xac59d0c3
    __ dci(0x44469693);  // srshlr z19.h, p5/m, z19.h, z20.h
    // vl128 state = 0xb3969968
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xb3969968,
        0x8ba60941,
        0x53937d52,
        0xe6737b5d,
        0x8649cf1f,
        0xb7ee12ca,
        0x6fd03bd4,
        0x4a82eb52,
        0xc0d52997,
        0xb52a263f,
        0x70599fa2,
        0x68cd2ef1,
        0x57b84410,
        0x1072dde9,
        0xe39a23c8,
        0xeded9f88,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_usra) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x450ce41d);  // usra z29.b, z0.b, #4
    // vl128 state = 0x57e84943
    __ dci(0x450ce635);  // usra z21.b, z17.b, #4
    // vl128 state = 0xc2696a7c
    __ dci(0x45cce637);  // usra z23.d, z17.d, #20
    // vl128 state = 0x97aec47c
    __ dci(0x458cee35);  // ursra z21.d, z17.d, #52
    // vl128 state = 0xab24864c
    __ dci(0x450eee25);  // ursra z5.b, z17.b, #2
    // vl128 state = 0x8aab49c9
    __ dci(0x458eef21);  // ursra z1.d, z25.d, #50
    // vl128 state = 0x3db09e7f
    __ dci(0x458fef65);  // ursra z5.d, z27.d, #49
    // vl128 state = 0xa9905ae3
    __ dci(0x459fef41);  // ursra z1.d, z26.d, #33
    // vl128 state = 0x624c2e4d
    __ dci(0x459fe549);  // usra z9.d, z10.d, #33
    // vl128 state = 0x5a158f70
    __ dci(0x459de561);  // usra z1.d, z11.d, #35
    // vl128 state = 0xf24ffa83
    __ dci(0x451ce565);  // usra z5.h, z11.h, #4
    // vl128 state = 0x0213f9c7
    __ dci(0x4519e564);  // usra z4.h, z11.h, #7
    // vl128 state = 0x8903ccf3
    __ dci(0x4589e56c);  // usra z12.d, z11.d, #55
    // vl128 state = 0x3c0f6e72
    __ dci(0x4589e56e);  // usra z14.d, z11.d, #55
    // vl128 state = 0x5d9787fc
    __ dci(0x4589e56c);  // usra z12.d, z11.d, #55
    // vl128 state = 0x3bc6fced
    __ dci(0x458bed64);  // ursra z4.d, z11.d, #53
    // vl128 state = 0x966476e2
    __ dci(0x45dbed65);  // ursra z5.d, z11.d, #5
    // vl128 state = 0xf85c4247
    __ dci(0x455bedf5);  // ursra z21.s, z15.s, #5
    // vl128 state = 0xd342f9ae
    __ dci(0x450bedfd);  // ursra z29.b, z15.b, #5
    // vl128 state = 0xc03cb476
    __ dci(0x4549edf9);  // ursra z25.s, z15.s, #23
    // vl128 state = 0x5649b073
    __ dci(0x4549ede9);  // ursra z9.s, z15.s, #23
    // vl128 state = 0xce5a7dbb
    __ dci(0x4549ed59);  // ursra z25.s, z10.s, #23
    // vl128 state = 0x8c98ee08
    __ dci(0x4549ed5d);  // ursra z29.s, z10.s, #23
    // vl128 state = 0xd991a574
    __ dci(0x45cded59);  // ursra z25.d, z10.d, #19
    // vl128 state = 0xebc24746
    __ dci(0x45d9ed58);  // ursra z24.d, z10.d, #7
    // vl128 state = 0x145d5970
    __ dci(0x45d8ec50);  // ursra z16.d, z2.d, #8
    // vl128 state = 0x8f65850c
    __ dci(0x45c8ec60);  // ursra z0.d, z3.d, #24
    // vl128 state = 0xe510a1b4
    __ dci(0x45c0ed61);  // ursra z1.d, z11.d, #32
    // vl128 state = 0xfef468e1
    __ dci(0x45c8ec65);  // ursra z5.d, z3.d, #24
    // vl128 state = 0xa6754589
    __ dci(0x45c0e464);  // usra z4.d, z3.d, #32
    // vl128 state = 0x2b4cd23a
    __ dci(0x45c0e4a5);  // usra z5.d, z5.d, #32
    // vl128 state = 0xfa58fea0
    __ dci(0x45c0e4a1);  // usra z1.d, z5.d, #32
    // vl128 state = 0x015c4435
    __ dci(0x45c0e4b1);  // usra z17.d, z5.d, #32
    // vl128 state = 0x67271050
    __ dci(0x45c2ecb3);  // ursra z19.d, z5.d, #30
    // vl128 state = 0x1d3631c3
    __ dci(0x45c0ece3);  // ursra z3.d, z7.d, #32
    // vl128 state = 0x646e0e43
    __ dci(0x45caece7);  // ursra z7.d, z7.d, #22
    // vl128 state = 0x104bf393
    __ dci(0x458aeee3);  // ursra z3.d, z23.d, #54
    // vl128 state = 0xbac8c54b
    __ dci(0x454aeee1);  // ursra z1.s, z23.s, #22
    // vl128 state = 0x5c2a40db
    __ dci(0x4508eee9);  // ursra z9.b, z23.b, #8
    // vl128 state = 0xe117d81a
    __ dci(0x4518ece1);  // ursra z1.h, z7.h, #8
    // vl128 state = 0xeb43265d
    __ dci(0x451cede0);  // ursra z0.h, z15.h, #4
    // vl128 state = 0xd5c8d09e
    __ dci(0x4598edf0);  // ursra z16.d, z15.d, #40
    // vl128 state = 0x0c060220
    __ dci(0x451cede0);  // ursra z0.h, z15.h, #4
    // vl128 state = 0x0ea52d2d
    __ dci(0x459cefe8);  // ursra z8.d, z31.d, #36
    // vl128 state = 0xa6a7e977
    __ dci(0x459ce5f8);  // usra z24.d, z15.d, #36
    // vl128 state = 0xb0192caf
    __ dci(0x458cedfa);  // ursra z26.d, z15.d, #52
    // vl128 state = 0x154fce29
    __ dci(0x458cedfe);  // ursra z30.d, z15.d, #52
    // vl128 state = 0x369cc3e1
    __ dci(0x450cedb6);  // ursra z22.b, z13.b, #4
    // vl128 state = 0xf613cb4b
    __ dci(0x450cedb4);  // ursra z20.b, z13.b, #4
    // vl128 state = 0xd075c8a9
    __ dci(0x458eeda4);  // ursra z4.d, z13.d, #50
    // vl128 state = 0xc9366682
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xc9366682,
        0xaf202cff,
        0x0e90a7c4,
        0xa8c89f40,
        0xc7bb56ad,
        0xa203dd34,
        0xf3b3a749,
        0xf16c9d5f,
        0x9929dea8,
        0xd652c693,
        0xe76f701b,
        0xe2fe20a3,
        0x07182afb,
        0x816b928f,
        0x52baf33f,
        0x9ef46875,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_ssra) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x450ce01d);  // ssra z29.b, z0.b, #4
    // vl128 state = 0xdf461c2b
    __ dci(0x450ce235);  // ssra z21.b, z17.b, #4
    // vl128 state = 0xd28868a9
    __ dci(0x45cce237);  // ssra z23.d, z17.d, #20
    // vl128 state = 0x874fc6a9
    __ dci(0x458cea35);  // srsra z21.d, z17.d, #52
    // vl128 state = 0xb848785b
    __ dci(0x450eea25);  // srsra z5.b, z17.b, #2
    // vl128 state = 0x8bca62e4
    __ dci(0x458eeb21);  // srsra z1.d, z25.d, #50
    // vl128 state = 0x3cd1b552
    __ dci(0x458feb65);  // srsra z5.d, z27.d, #49
    // vl128 state = 0xd78844fb
    __ dci(0x459feb41);  // srsra z1.d, z26.d, #33
    // vl128 state = 0xa948dc2f
    __ dci(0x459fe149);  // ssra z9.d, z10.d, #33
    // vl128 state = 0x709a83f1
    __ dci(0x459de161);  // ssra z1.d, z11.d, #35
    // vl128 state = 0x1c21e4f6
    __ dci(0x451ce165);  // ssra z5.h, z11.h, #4
    // vl128 state = 0x72288f41
    __ dci(0x4519e164);  // ssra z4.h, z11.h, #7
    // vl128 state = 0x9a8c4c8c
    __ dci(0x4589e16c);  // ssra z12.d, z11.d, #55
    // vl128 state = 0x872585d4
    __ dci(0x4589e16e);  // ssra z14.d, z11.d, #55
    // vl128 state = 0xd237aaa0
    __ dci(0x4589e16c);  // ssra z12.d, z11.d, #55
    // vl128 state = 0x1c828333
    __ dci(0x458be964);  // srsra z4.d, z11.d, #53
    // vl128 state = 0xc190178f
    __ dci(0x45dbe965);  // srsra z5.d, z11.d, #5
    // vl128 state = 0xe9e81bda
    __ dci(0x455be9f5);  // srsra z21.s, z15.s, #5
    // vl128 state = 0x8e58c7a1
    __ dci(0x450be9fd);  // srsra z29.b, z15.b, #5
    // vl128 state = 0x904b404b
    __ dci(0x4549e9f9);  // srsra z25.s, z15.s, #23
    // vl128 state = 0x35a60481
    __ dci(0x4549e9e9);  // srsra z9.s, z15.s, #23
    // vl128 state = 0x6911448b
    __ dci(0x4549e959);  // srsra z25.s, z10.s, #23
    // vl128 state = 0xdb384324
    __ dci(0x4549e95d);  // srsra z29.s, z10.s, #23
    // vl128 state = 0x16acd8ee
    __ dci(0x45cde959);  // srsra z25.d, z10.d, #19
    // vl128 state = 0x56bf7bda
    __ dci(0x45d9e958);  // srsra z24.d, z10.d, #7
    // vl128 state = 0x6a713fa6
    __ dci(0x45d8e850);  // srsra z16.d, z2.d, #8
    // vl128 state = 0xa6394cf3
    __ dci(0x45c8e860);  // srsra z0.d, z3.d, #24
    // vl128 state = 0x829c3d2a
    __ dci(0x45c0e961);  // srsra z1.d, z11.d, #32
    // vl128 state = 0x006d1904
    __ dci(0x45c8e865);  // srsra z5.d, z3.d, #24
    // vl128 state = 0xcc7dffaf
    __ dci(0x45c0e064);  // ssra z4.d, z3.d, #32
    // vl128 state = 0xc9eaddd0
    __ dci(0x45c0e0a5);  // ssra z5.d, z5.d, #32
    // vl128 state = 0x643145e1
    __ dci(0x45c0e0a1);  // ssra z1.d, z5.d, #32
    // vl128 state = 0x03f4c42e
    __ dci(0x45c0e0b1);  // ssra z17.d, z5.d, #32
    // vl128 state = 0x5a8cff35
    __ dci(0x45c2e8b3);  // srsra z19.d, z5.d, #30
    // vl128 state = 0x3ee63e9f
    __ dci(0x45c0e8e3);  // srsra z3.d, z7.d, #32
    // vl128 state = 0x687d943b
    __ dci(0x45cae8e7);  // srsra z7.d, z7.d, #22
    // vl128 state = 0xf5a19cb2
    __ dci(0x458aeae3);  // srsra z3.d, z23.d, #54
    // vl128 state = 0xd1371248
    __ dci(0x454aeae1);  // srsra z1.s, z23.s, #22
    // vl128 state = 0xdb83ef8b
    __ dci(0x455ae8e9);  // srsra z9.s, z7.s, #6
    // vl128 state = 0xc831a54c
    __ dci(0x455ee9e8);  // srsra z8.s, z15.s, #2
    // vl128 state = 0x4342b823
    __ dci(0x45dae9f8);  // srsra z24.d, z15.d, #6
    // vl128 state = 0x52a7151a
    __ dci(0x455ee9e8);  // srsra z8.s, z15.s, #2
    // vl128 state = 0xde8110e0
    __ dci(0x45deebe0);  // srsra z0.d, z31.d, #2
    // vl128 state = 0xd2b28e81
    __ dci(0x45dee1f0);  // ssra z16.d, z15.d, #2
    // vl128 state = 0x56d1c366
    __ dci(0x45cee9f2);  // srsra z18.d, z15.d, #18
    // vl128 state = 0x53537689
    __ dci(0x45cee9f6);  // srsra z22.d, z15.d, #18
    // vl128 state = 0x5e410508
    __ dci(0x454ee9be);  // srsra z30.s, z13.s, #18
    // vl128 state = 0x06245094
    __ dci(0x454ee9bc);  // srsra z28.s, z13.s, #18
    // vl128 state = 0xb92b3929
    __ dci(0x45cce9ac);  // srsra z12.d, z13.d, #20
    // vl128 state = 0xfe6a2830
    __ dci(0x45cde93c);  // srsra z28.d, z9.d, #19
    // vl128 state = 0x737461a1
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x737461a1,
        0xe1ef707c,
        0x9760ba4e,
        0x782dd4cd,
        0xe793d0c2,
        0x991e0de7,
        0x34627e21,
        0x76c89433,
        0x96c9f4ce,
        0x38ec4b6f,
        0x7aee3ec7,
        0x665f9b94,
        0x8e166fc3,
        0xb4461fac,
        0x215de9dc,
        0xc23ef1f9,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_sat_arith) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x44df9df9);  // uqsubr z25.d, p7/m, z25.d, z15.d
    // vl128 state = 0x7670ac87
    __ dci(0x445f9db1);  // uqsubr z17.h, p7/m, z17.h, z13.h
    // vl128 state = 0x3c5b39fe
    __ dci(0x441f99a1);  // uqsubr z1.b, p6/m, z1.b, z13.b
    // vl128 state = 0x5df43635
    __ dci(0x441d9ba0);  // usqadd z0.b, p6/m, z0.b, z29.b
    // vl128 state = 0x737bc7a5
    __ dci(0x441d9ba8);  // usqadd z8.b, p6/m, z8.b, z29.b
    // vl128 state = 0xba69890b
    __ dci(0x441d9bb8);  // usqadd z24.b, p6/m, z24.b, z29.b
    // vl128 state = 0x3f81c19d
    __ dci(0x441d8b30);  // usqadd z16.b, p2/m, z16.b, z25.b
    // vl128 state = 0x076c5fc1
    __ dci(0x441d8a14);  // usqadd z20.b, p2/m, z20.b, z16.b
    // vl128 state = 0x67df29dd
    __ dci(0x449d8215);  // usqadd z21.s, p0/m, z21.s, z16.s
    // vl128 state = 0x663b236f
    __ dci(0x449d8205);  // usqadd z5.s, p0/m, z5.s, z16.s
    // vl128 state = 0xe58d41d0
    __ dci(0x449d8201);  // usqadd z1.s, p0/m, z1.s, z16.s
    // vl128 state = 0x82f89d40
    __ dci(0x449c8a09);  // suqadd z9.s, p2/m, z9.s, z16.s
    // vl128 state = 0xa0218390
    __ dci(0x44dd8a0d);  // usqadd z13.d, p2/m, z13.d, z16.d
    // vl128 state = 0xfab22f04
    __ dci(0x44d98a2c);  // uqadd z12.d, p2/m, z12.d, z17.d
    // vl128 state = 0x70911fc9
    __ dci(0x44598a0d);  // uqadd z13.h, p2/m, z13.h, z16.h
    // vl128 state = 0xcc12ec49
    __ dci(0x44d99a05);  // uqadd z5.d, p6/m, z5.d, z16.d
    // vl128 state = 0x31fef46f
    __ dci(0x44d99004);  // uqadd z4.d, p4/m, z4.d, z0.d
    // vl128 state = 0xf81448db
    __ dci(0x44d98020);  // uqadd z0.d, p0/m, z0.d, z1.d
    // vl128 state = 0xe6fe9d31
    __ dci(0x44d980e1);  // uqadd z1.d, p0/m, z1.d, z7.d
    // vl128 state = 0x76fecfc2
    __ dci(0x44d981c0);  // uqadd z0.d, p0/m, z0.d, z14.d
    // vl128 state = 0x4066a558
    __ dci(0x44d98161);  // uqadd z1.d, p0/m, z1.d, z11.d
    // vl128 state = 0x0d3a1487
    __ dci(0x44d98031);  // uqadd z17.d, p0/m, z17.d, z1.d
    // vl128 state = 0x061b4aed
    __ dci(0x44d98039);  // uqadd z25.d, p0/m, z25.d, z1.d
    // vl128 state = 0x02172a17
    __ dci(0x44d98029);  // uqadd z9.d, p0/m, z9.d, z1.d
    // vl128 state = 0xebe138b3
    __ dci(0x44d8800d);  // sqadd z13.d, p0/m, z13.d, z0.d
    // vl128 state = 0x73f0114b
    __ dci(0x44d8828f);  // sqadd z15.d, p0/m, z15.d, z20.d
    // vl128 state = 0x7a8689e0
    __ dci(0x44d8829f);  // sqadd z31.d, p0/m, z31.d, z20.d
    // vl128 state = 0x0800ae49
    __ dci(0x44d88e8f);  // sqadd z15.d, p3/m, z15.d, z20.d
    // vl128 state = 0x9b733fff
    __ dci(0x44d88e8b);  // sqadd z11.d, p3/m, z11.d, z20.d
    // vl128 state = 0x6d01eb90
    __ dci(0x44d88e8f);  // sqadd z15.d, p3/m, z15.d, z20.d
    // vl128 state = 0x337692b3
    __ dci(0x44d8968e);  // sqadd z14.d, p5/m, z14.d, z20.d
    // vl128 state = 0xcd4478b6
    __ dci(0x44d886ca);  // sqadd z10.d, p1/m, z10.d, z22.d
    // vl128 state = 0x335fd099
    __ dci(0x44dc87ce);  // suqadd z14.d, p1/m, z14.d, z30.d
    // vl128 state = 0x0d3b6403
    __ dci(0x44de8fcf);  // sqsubr z15.d, p3/m, z15.d, z30.d
    // vl128 state = 0x41a1073f
    __ dci(0x449e9fcd);  // sqsubr z13.s, p7/m, z13.s, z30.s
    // vl128 state = 0x5a4b1c22
    __ dci(0x445e9fcf);  // sqsubr z15.h, p7/m, z15.h, z30.h
    // vl128 state = 0x5a08ccf1
    __ dci(0x441e9ece);  // sqsubr z14.b, p7/m, z14.b, z22.b
    // vl128 state = 0x3f3c700c
    __ dci(0x441e8cde);  // sqsubr z30.b, p3/m, z30.b, z6.b
    // vl128 state = 0x3b32b296
    __ dci(0x441e88fa);  // sqsubr z26.b, p2/m, z26.b, z7.b
    // vl128 state = 0x7a6472e3
    __ dci(0x441f98f8);  // uqsubr z24.b, p6/m, z24.b, z7.b
    // vl128 state = 0x1d72f5ea
    __ dci(0x441f98fc);  // uqsubr z28.b, p6/m, z28.b, z7.b
    // vl128 state = 0x0245804b
    __ dci(0x441b9afe);  // uqsub z30.b, p6/m, z30.b, z23.b
    // vl128 state = 0x8c7ac3d7
    __ dci(0x441b9afc);  // uqsub z28.b, p6/m, z28.b, z23.b
    // vl128 state = 0xa96d65cb
    __ dci(0x449b9a74);  // uqsub z20.s, p6/m, z20.s, z19.s
    // vl128 state = 0x261eb58f
    __ dci(0x449a9b75);  // sqsub z21.s, p6/m, z21.s, z27.s
    // vl128 state = 0x3464e3e5
    __ dci(0x449a9b7d);  // sqsub z29.s, p6/m, z29.s, z27.s
    // vl128 state = 0xfe3ab427
    __ dci(0x445a9b79);  // sqsub z25.h, p6/m, z25.h, z27.h
    // vl128 state = 0x609eef3a
    __ dci(0x445a9b7d);  // sqsub z29.h, p6/m, z29.h, z27.h
    // vl128 state = 0x0e6d6940
    __ dci(0x445e9b5f);  // sqsubr z31.h, p6/m, z31.h, z26.h
    // vl128 state = 0x60a375e7
    __ dci(0x441e8b5b);  // sqsubr z27.b, p2/m, z27.b, z26.b
    // vl128 state = 0xea9bd16f
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xea9bd16f,
        0x1296119e,
        0x00aaf6dc,
        0xb6ce0579,
        0xdb3d0829,
        0x119f52d0,
        0xf697dcd8,
        0x2c46a66c,
        0x7d838497,
        0x6cd68fb3,
        0xf98a5c79,
        0x51685054,
        0xa9494104,
        0x8d012936,
        0x32726258,
        0x091f1956,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_pair_arith) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 64 * kInstructionSize);
    __ dci(0x4414b214);  // smaxp z20.b, p4/m, z20.b, z16.b
    // vl128 state = 0x90adc6c9
    __ dci(0x4414ba5c);  // smaxp z28.b, p6/m, z28.b, z18.b
    // vl128 state = 0x0e41b2b9
    __ dci(0x4454ba0c);  // smaxp z12.h, p6/m, z12.h, z16.h
    // vl128 state = 0x472160b8
    __ dci(0x4454ba64);  // smaxp z4.h, p6/m, z4.h, z19.h
    // vl128 state = 0x4f485ba3
    __ dci(0x44d4bb65);  // smaxp z5.d, p6/m, z5.d, z27.d
    // vl128 state = 0x432f5185
    __ dci(0x4456bb64);  // sminp z4.h, p6/m, z4.h, z27.h
    // vl128 state = 0x01bd324a
    __ dci(0x4455bb74);  // umaxp z20.h, p6/m, z20.h, z27.h
    // vl128 state = 0xaf795389
    __ dci(0x4451bb35);  // addp z21.h, p6/m, z21.h, z25.h
    // vl128 state = 0x5f4be111
    __ dci(0x4451ab71);  // addp z17.h, p2/m, z17.h, z27.h
    // vl128 state = 0xc16a8d03
    __ dci(0x4451ba75);  // addp z21.h, p6/m, z21.h, z19.h
    // vl128 state = 0x8cd36853
    __ dci(0x4451b225);  // addp z5.h, p4/m, z5.h, z17.h
    // vl128 state = 0xea3d5389
    __ dci(0x4455b627);  // umaxp z7.h, p5/m, z7.h, z17.h
    // vl128 state = 0xbb42a8e1
    __ dci(0x4415b426);  // umaxp z6.b, p5/m, z6.b, z1.b
    // vl128 state = 0x485ca761
    __ dci(0x4415b224);  // umaxp z4.b, p4/m, z4.b, z17.b
    // vl128 state = 0x6bcfd641
    __ dci(0x4455b02c);  // umaxp z12.h, p4/m, z12.h, z1.h
    // vl128 state = 0x84485a9f
    __ dci(0x4455a12d);  // umaxp z13.h, p0/m, z13.h, z9.h
    // vl128 state = 0xed43519f
    __ dci(0x4455b33d);  // umaxp z29.h, p4/m, z29.h, z25.h
    // vl128 state = 0xcc0b7c40
    __ dci(0x4455b7b9);  // umaxp z25.h, p5/m, z25.h, z29.h
    // vl128 state = 0xe1c14517
    __ dci(0x4454b6b8);  // smaxp z24.h, p5/m, z24.h, z21.h
    // vl128 state = 0x4c5e9f3c
    __ dci(0x44d4b4bc);  // smaxp z28.d, p5/m, z28.d, z5.d
    // vl128 state = 0x7530a2f7
    __ dci(0x44d4b4bd);  // smaxp z29.d, p5/m, z29.d, z5.d
    // vl128 state = 0x37e61b68
    __ dci(0x44d4b5ed);  // smaxp z13.d, p5/m, z13.d, z15.d
    // vl128 state = 0xb592b6e9
    __ dci(0x4455b5fd);  // umaxp z29.h, p5/m, z29.h, z15.h
    // vl128 state = 0xe7f9e492
    __ dci(0x4415b57f);  // umaxp z31.b, p5/m, z31.b, z11.b
    // vl128 state = 0xe4e7b644
    __ dci(0x4411b5fe);  // addp z30.b, p5/m, z30.b, z15.b
    // vl128 state = 0x4bfe144d
    __ dci(0x4411a576);  // addp z22.b, p1/m, z22.b, z11.b
    // vl128 state = 0xb1813df8
    __ dci(0x4455a566);  // umaxp z6.h, p1/m, z6.h, z11.h
    // vl128 state = 0x4aa8b50e
    __ dci(0x4455adf6);  // umaxp z22.h, p3/m, z22.h, z15.h
    // vl128 state = 0xfc13568a
    __ dci(0x4454acfe);  // smaxp z30.h, p3/m, z30.h, z7.h
    // vl128 state = 0x3aac7365
    __ dci(0x4454acff);  // smaxp z31.h, p3/m, z31.h, z7.h
    // vl128 state = 0x610991cf
    __ dci(0x44d4a8fb);  // smaxp z27.d, p2/m, z27.d, z7.d
    // vl128 state = 0x36581f26
    __ dci(0x4456a8f3);  // sminp z19.h, p2/m, z19.h, z7.h
    // vl128 state = 0x249bb813
    __ dci(0x4457a8b1);  // uminp z17.h, p2/m, z17.h, z5.h
    // vl128 state = 0xd48d6d88
    __ dci(0x4457a8b5);  // uminp z21.h, p2/m, z21.h, z5.h
    // vl128 state = 0x1628fb6e
    __ dci(0x4456a8f7);  // sminp z23.h, p2/m, z23.h, z7.h
    // vl128 state = 0x0bd3c76b
    __ dci(0x4456a89f);  // sminp z31.h, p2/m, z31.h, z4.h
    // vl128 state = 0xf09d21e4
    __ dci(0x4456aa0f);  // sminp z15.h, p2/m, z15.h, z16.h
    // vl128 state = 0xd2a92168
    __ dci(0x4456b807);  // sminp z7.h, p6/m, z7.h, z0.h
    // vl128 state = 0x009d0ac8
    __ dci(0x4456bc26);  // sminp z6.h, p7/m, z6.h, z1.h
    // vl128 state = 0x716ddc73
    __ dci(0x4456beae);  // sminp z14.h, p7/m, z14.h, z21.h
    // vl128 state = 0x35a4d900
    __ dci(0x4416b6ac);  // sminp z12.b, p5/m, z12.b, z21.b
    // vl128 state = 0x7929e077
    __ dci(0x4416b6bc);  // sminp z28.b, p5/m, z28.b, z21.b
    // vl128 state = 0x259195ca
    __ dci(0x4417b694);  // uminp z20.b, p5/m, z20.b, z20.b
    // vl128 state = 0x5cc3927b
    __ dci(0x4417b684);  // uminp z4.b, p5/m, z4.b, z20.b
    // vl128 state = 0x2e7c4b88
    __ dci(0x4415b6a0);  // umaxp z0.b, p5/m, z0.b, z21.b
    // vl128 state = 0x1478d524
    __ dci(0x4415a690);  // umaxp z16.b, p1/m, z16.b, z20.b
    // vl128 state = 0xc3ac4a89
    __ dci(0x4415b614);  // umaxp z20.b, p5/m, z20.b, z16.b
    // vl128 state = 0xb94a5aeb
    __ dci(0x4415b675);  // umaxp z21.b, p5/m, z21.b, z19.b
    // vl128 state = 0xabeed92b
    __ dci(0x4415a63d);  // umaxp z29.b, p1/m, z29.b, z17.b
    // vl128 state = 0xe36835ea
    __ dci(0x4415a63c);  // umaxp z28.b, p1/m, z28.b, z17.b
    // vl128 state = 0x087002bb
    __ dci(0x4455a61d);  // umaxp z29.h, p1/m, z29.h, z16.h
    // vl128 state = 0x17388ea4
    __ dci(0x4451ae1f);  // addp z31.h, p3/m, z31.h, z16.h
    // vl128 state = 0x86ee7dbe
    __ dci(0x4451ae1b);  // addp z27.h, p3/m, z27.h, z16.h
    // vl128 state = 0x9846169e
    __ dci(0x4451bc0b);  // addp z11.h, p7/m, z11.h, z0.h
    // vl128 state = 0x5dc31eb0
    __ dci(0x4455bc4f);  // umaxp z15.h, p7/m, z15.h, z2.h
    // vl128 state = 0x9ec9086c
    __ dci(0x4455bf47);  // umaxp z7.h, p7/m, z7.h, z26.h
    // vl128 state = 0xf3a2766b
    __ dci(0x44d5b743);  // umaxp z3.d, p5/m, z3.d, z26.d
    // vl128 state = 0x1ce44f7e
    __ dci(0x44d5b7e2);  // umaxp z2.d, p5/m, z2.d, z31.d
    // vl128 state = 0xf121f7c0
    __ dci(0x44d5b7e0);  // umaxp z0.d, p5/m, z0.d, z31.d
    // vl128 state = 0x4ac0d4f3
    __ dci(0x44d5b670);  // umaxp z16.d, p5/m, z16.d, z19.d
    // vl128 state = 0xdb0d62f5
    __ dci(0x44d1b272);  // addp z18.d, p4/m, z18.d, z19.d
    // vl128 state = 0x34b0c018
    __ dci(0x44d1be76);  // addp z22.d, p7/m, z22.d, z19.d
    // vl128 state = 0x1673f380
    __ dci(0x44d1b772);  // addp z18.d, p5/m, z18.d, z27.d
    // vl128 state = 0xe3e67205
    __ dci(0x44d1b162);  // addp z2.d, p4/m, z2.d, z11.d
    // vl128 state = 0x42907adc
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x42907adc,
        0xee2f21f5,
        0xcbfa0af4,
        0x42e7c862,
        0x10ef537f,
        0x83461e96,
        0x2dca0c37,
        0xf2080504,
        0xf615d956,
        0x1732775a,
        0x491fec07,
        0xf9e33ada,
        0x324435d7,
        0x08a9c2ca,
        0x87ce3994,
        0x338adb5d,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_extract_narrow) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 64 * kInstructionSize);
    __ dci(0x45284000);  // sqxtnb z0.b, z0.h
    // vl128 state = 0x874f147b
    __ dci(0x45284228);  // sqxtnb z8.b, z17.h
    // vl128 state = 0xf694d31e
    __ dci(0x45284820);  // uqxtnb z0.b, z1.h
    // vl128 state = 0x5d25df42
    __ dci(0x45304821);  // uqxtnb z1.h, z1.s
    // vl128 state = 0x87eb933f
    __ dci(0x45304823);  // uqxtnb z3.h, z1.s
    // vl128 state = 0x137eddc9
    __ dci(0x45604822);  // uqxtnb z2.s, z1.d
    // vl128 state = 0x26e237a3
    __ dci(0x45604d26);  // uqxtnt z6.s, z9.d
    // vl128 state = 0x72bcf361
    __ dci(0x45304d2e);  // uqxtnt z14.h, z9.s
    // vl128 state = 0x5bcdd232
    __ dci(0x45304d3e);  // uqxtnt z30.h, z9.s
    // vl128 state = 0x9a695f7e
    __ dci(0x453049bc);  // uqxtnb z28.h, z13.s
    // vl128 state = 0x9c2fa230
    __ dci(0x453049b8);  // uqxtnb z24.h, z13.s
    // vl128 state = 0xb590179f
    __ dci(0x45304979);  // uqxtnb z25.h, z11.s
    // vl128 state = 0xc8987735
    __ dci(0x4530497d);  // uqxtnb z29.h, z11.s
    // vl128 state = 0x380f8730
    __ dci(0x4530496d);  // uqxtnb z13.h, z11.s
    // vl128 state = 0x45bf22d4
    __ dci(0x45304565);  // sqxtnt z5.h, z11.s
    // vl128 state = 0xd9237f41
    __ dci(0x45304f75);  // uqxtnt z21.h, z27.s
    // vl128 state = 0x0726a49b
    __ dci(0x45304f71);  // uqxtnt z17.h, z27.s
    // vl128 state = 0xcbc547e0
    __ dci(0x45304f73);  // uqxtnt z19.h, z27.s
    // vl128 state = 0x0b16d843
    __ dci(0x45284f72);  // uqxtnt z18.b, z27.h
    // vl128 state = 0xea84ff1f
    __ dci(0x45284f7a);  // uqxtnt z26.b, z27.h
    // vl128 state = 0x4bdb094d
    __ dci(0x45284fca);  // uqxtnt z10.b, z30.h
    // vl128 state = 0x5986f190
    __ dci(0x45284b8b);  // uqxtnb z11.b, z28.h
    // vl128 state = 0xb40f0b26
    __ dci(0x45284bef);  // uqxtnb z15.b, z31.h
    // vl128 state = 0x7abef2b5
    __ dci(0x45284fae);  // uqxtnt z14.b, z29.h
    // vl128 state = 0x79503b36
    __ dci(0x45284fac);  // uqxtnt z12.b, z29.h
    // vl128 state = 0x481a6879
    __ dci(0x45284eed);  // uqxtnt z13.b, z23.h
    // vl128 state = 0x32da844c
    __ dci(0x45284ee9);  // uqxtnt z9.b, z23.h
    // vl128 state = 0xb8438ca7
    __ dci(0x45284ef9);  // uqxtnt z25.b, z23.h
    // vl128 state = 0x4aa26674
    __ dci(0x45284cd1);  // uqxtnt z17.b, z6.h
    // vl128 state = 0xc5411d78
    __ dci(0x45284cd5);  // uqxtnt z21.b, z6.h
    // vl128 state = 0xee446689
    __ dci(0x45284ad4);  // uqxtnb z20.b, z22.h
    // vl128 state = 0x66ef53ef
    __ dci(0x45604adc);  // uqxtnb z28.s, z22.d
    // vl128 state = 0xa894f4d4
    __ dci(0x45604ade);  // uqxtnb z30.s, z22.d
    // vl128 state = 0x50215eb8
    __ dci(0x456040dc);  // sqxtnb z28.s, z6.d
    // vl128 state = 0x5ee8464d
    __ dci(0x456048f4);  // uqxtnb z20.s, z7.d
    // vl128 state = 0xee2ca07b
    __ dci(0x45604c75);  // uqxtnt z21.s, z3.d
    // vl128 state = 0x0e81e7e0
    __ dci(0x45604cb1);  // uqxtnt z17.s, z5.d
    // vl128 state = 0x5c448cac
    __ dci(0x45604e33);  // uqxtnt z19.s, z17.d
    // vl128 state = 0xcd0d561e
    __ dci(0x45604e23);  // uqxtnt z3.s, z17.d
    // vl128 state = 0x7b8b2204
    __ dci(0x45604cab);  // uqxtnt z11.s, z5.d
    // vl128 state = 0x418cec7f
    __ dci(0x45604caa);  // uqxtnt z10.s, z5.d
    // vl128 state = 0x37064bb6
    __ dci(0x45604efa);  // uqxtnt z26.s, z23.d
    // vl128 state = 0xc83ef05d
    __ dci(0x456046db);  // sqxtnt z27.s, z22.d
    // vl128 state = 0xe30a1f0f
    __ dci(0x456046da);  // sqxtnt z26.s, z22.d
    // vl128 state = 0xe10b92fa
    __ dci(0x4560424a);  // sqxtnb z10.s, z18.d
    // vl128 state = 0x2396410c
    __ dci(0x45604a08);  // uqxtnb z8.s, z16.d
    // vl128 state = 0xf4ae5ad5
    __ dci(0x45304a00);  // uqxtnb z0.h, z16.s
    // vl128 state = 0x26bbb3d1
    __ dci(0x45304828);  // uqxtnb z8.h, z1.s
    // vl128 state = 0x57d91166
    __ dci(0x4530422c);  // sqxtnb z12.h, z17.s
    // vl128 state = 0x5548e0b4
    __ dci(0x45305324);  // sqxtunb z4.h, z25.s
    // vl128 state = 0xf7eb8d9c
    __ dci(0x45305325);  // sqxtunb z5.h, z25.s
    // vl128 state = 0xcf294303
    __ dci(0x45305321);  // sqxtunb z1.h, z25.s
    // vl128 state = 0x6c7597d6
    __ dci(0x453057a9);  // sqxtunt z9.h, z29.s
    // vl128 state = 0xe7be4fd5
    __ dci(0x453043b9);  // sqxtnb z25.h, z29.s
    // vl128 state = 0x376f3f76
    __ dci(0x453043bb);  // sqxtnb z27.h, z29.s
    // vl128 state = 0xf8389159
    __ dci(0x4530431a);  // sqxtnb z26.h, z24.s
    // vl128 state = 0x8ca15413
    __ dci(0x45304312);  // sqxtnb z18.h, z24.s
    // vl128 state = 0x2a6d8b90
    __ dci(0x4530491a);  // uqxtnb z26.h, z8.s
    // vl128 state = 0x7119ff0d
    __ dci(0x4530413b);  // sqxtnb z27.h, z9.s
    // vl128 state = 0x884748db
    __ dci(0x4530482b);  // uqxtnb z11.h, z1.s
    // vl128 state = 0x43296aec
    __ dci(0x4530483b);  // uqxtnb z27.h, z1.s
    // vl128 state = 0xdb9908f0
    __ dci(0x45304979);  // uqxtnb z25.h, z11.s
    // vl128 state = 0xef30bfc8
    __ dci(0x453049d1);  // uqxtnb z17.h, z14.s
    // vl128 state = 0xb46173d8
    __ dci(0x456049d3);  // uqxtnb z19.s, z14.d
    // vl128 state = 0xcb8c3b83
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xcb8c3b83,
        0x92fb7f98,
        0xb7ec6385,
        0x81de8602,
        0xd970d431,
        0x2fe61431,
        0x359b1355,
        0xdeec900e,
        0xfd0c7d7d,
        0x62e89b19,
        0x43039424,
        0xdd42efc9,
        0x861010f1,
        0x82d68f37,
        0x3761a1d0,
        0xbcf3c5c9,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_eorbt_eortb) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x451892b8);  // eorbt z24.b, z21.b, z24.b
    // vl128 state = 0xc3f2b082
    __ dci(0x455893ba);  // eorbt z26.h, z29.h, z24.h
    // vl128 state = 0xc7421198
    __ dci(0x455892f8);  // eorbt z24.h, z23.h, z24.h
    // vl128 state = 0x4e155b96
    __ dci(0x455092bc);  // eorbt z28.h, z21.h, z16.h
    // vl128 state = 0x09393ad0
    __ dci(0x455893be);  // eorbt z30.h, z29.h, z24.h
    // vl128 state = 0x6d660844
    __ dci(0x4558922e);  // eorbt z14.h, z17.h, z24.h
    // vl128 state = 0x84f1ff20
    __ dci(0x45d892aa);  // eorbt z10.d, z21.d, z24.d
    // vl128 state = 0x568612d4
    __ dci(0x454892a8);  // eorbt z8.h, z21.h, z8.h
    // vl128 state = 0x699a3e24
    __ dci(0x45c890ac);  // eorbt z12.d, z5.d, z8.d
    // vl128 state = 0x17bb6d9b
    __ dci(0x45c990ed);  // eorbt z13.d, z7.d, z9.d
    // vl128 state = 0xee5be73f
    __ dci(0x45c892fd);  // eorbt z29.d, z23.d, z8.d
    // vl128 state = 0x141c47ed
    __ dci(0x45c892f9);  // eorbt z25.d, z23.d, z8.d
    // vl128 state = 0xc3259593
    __ dci(0x45c892f8);  // eorbt z24.d, z23.d, z8.d
    // vl128 state = 0x3bca0bcc
    __ dci(0x45c892e8);  // eorbt z8.d, z23.d, z8.d
    // vl128 state = 0x4714ab64
    __ dci(0x454a92ea);  // eorbt z10.h, z23.h, z10.h
    // vl128 state = 0x51360c73
    __ dci(0x454092e2);  // eorbt z2.h, z23.h, z0.h
    // vl128 state = 0xe33859fe
    __ dci(0x454092f2);  // eorbt z18.h, z23.h, z0.h
    // vl128 state = 0xa0d81168
    __ dci(0x4550927a);  // eorbt z26.h, z19.h, z16.h
    // vl128 state = 0xe4983274
    __ dci(0x4551923b);  // eorbt z27.h, z17.h, z17.h
    // vl128 state = 0x8e89eab7
    __ dci(0x45d3923f);  // eorbt z31.d, z17.d, z19.d
    // vl128 state = 0x472bd288
    __ dci(0x4553921d);  // eorbt z29.h, z16.h, z19.h
    // vl128 state = 0x61090ed4
    __ dci(0x4553932d);  // eorbt z13.h, z25.h, z19.h
    // vl128 state = 0x3ef228eb
    __ dci(0x4513912c);  // eorbt z12.b, z9.b, z19.b
    // vl128 state = 0x96d4505c
    __ dci(0x4551912d);  // eorbt z13.h, z9.h, z17.h
    // vl128 state = 0x1c32baef
    __ dci(0x45119029);  // eorbt z9.b, z1.b, z17.b
    // vl128 state = 0xa138f554
    __ dci(0x45149028);  // eorbt z8.b, z1.b, z20.b
    // vl128 state = 0xf0681d9a
    __ dci(0x459490aa);  // eorbt z10.s, z5.s, z20.s
    // vl128 state = 0xbd4b30f5
    __ dci(0x458590a8);  // eorbt z8.s, z5.s, z5.s
    // vl128 state = 0x45c5b437
    __ dci(0x4585948c);  // eortb z12.s, z4.s, z5.s
    // vl128 state = 0x22f90a7b
    __ dci(0x45cd949c);  // eortb z28.d, z4.d, z13.d
    // vl128 state = 0x5e4584ca
    __ dci(0x4589949d);  // eortb z29.s, z4.s, z9.s
    // vl128 state = 0x65ac913e
    __ dci(0x458990ad);  // eorbt z13.s, z5.s, z9.s
    // vl128 state = 0x4f13d973
    __ dci(0x459b90ac);  // eorbt z12.s, z5.s, z27.s
    // vl128 state = 0xd13bb801
    __ dci(0x45db90ee);  // eorbt z14.d, z7.d, z27.d
    // vl128 state = 0xf24115d0
    __ dci(0x45db916f);  // eorbt z15.d, z11.d, z27.d
    // vl128 state = 0x04f38375
    __ dci(0x45db95e7);  // eortb z7.d, z15.d, z27.d
    // vl128 state = 0xe1046ae5
    __ dci(0x45db94a3);  // eortb z3.d, z5.d, z27.d
    // vl128 state = 0xaaeae67e
    __ dci(0x45dd94a1);  // eortb z1.d, z5.d, z29.d
    // vl128 state = 0xd67f6823
    __ dci(0x45dd94b1);  // eortb z17.d, z5.d, z29.d
    // vl128 state = 0xf172245b
    __ dci(0x45dd90f3);  // eorbt z19.d, z7.d, z29.d
    // vl128 state = 0xc99195b8
    __ dci(0x458d90e3);  // eorbt z3.s, z7.s, z13.s
    // vl128 state = 0xe1a146cf
    __ dci(0x458994e2);  // eortb z2.s, z7.s, z9.s
    // vl128 state = 0x8038f273
    __ dci(0x458b94a3);  // eortb z3.s, z5.s, z11.s
    // vl128 state = 0x50bda372
    __ dci(0x459b9481);  // eortb z1.s, z4.s, z27.s
    // vl128 state = 0xe8d53012
    __ dci(0x455b9485);  // eortb z5.h, z4.h, z27.h
    // vl128 state = 0xdba33ea5
    __ dci(0x454b9087);  // eorbt z7.h, z4.h, z11.h
    // vl128 state = 0xff7f1815
    __ dci(0x45499003);  // eorbt z3.h, z0.h, z9.h
    // vl128 state = 0x5d6e0104
    __ dci(0x454d9022);  // eorbt z2.h, z1.h, z13.h
    // vl128 state = 0xe9161cfe
    __ dci(0x45099026);  // eorbt z6.b, z1.b, z9.b
    // vl128 state = 0x48126fb9
    __ dci(0x454b9024);  // eorbt z4.h, z1.h, z11.h
    // vl128 state = 0x53cbfc46
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x53cbfc46,
        0x0f81a01e,
        0xf97c4e96,
        0x745e9ed6,
        0x4487a0a1,
        0x7ad79509,
        0x53577280,
        0x1e589717,
        0xaaa96af0,
        0x4f2b0884,
        0x24d2cd1c,
        0x4d89438d,
        0x9b327a12,
        0xeabfd558,
        0xb63e33f1,
        0xebd7d9ca,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_saturating_multiply_add_high_vector) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 40 * kInstructionSize);
    __ dci(0x44d9721a);  // sqrdmlah z26.d, z16.d, z25.d
    // vl128 state = 0xe7f26fd2
    __ dci(0x44dd761b);  // sqrdmlsh z27.d, z16.d, z29.d
    // vl128 state = 0x627ffb65
    __ dci(0x44d4760b);  // sqrdmlsh z11.d, z16.d, z20.d
    // vl128 state = 0x727ac13a
    __ dci(0x44947709);  // sqrdmlsh z9.s, z24.s, z20.s
    // vl128 state = 0x47045ddf
    __ dci(0x4494770b);  // sqrdmlsh z11.s, z24.s, z20.s
    // vl128 state = 0xbd00a91d
    __ dci(0x44d4773b);  // sqrdmlsh z27.d, z25.d, z20.d
    // vl128 state = 0x399e7643
    __ dci(0x44dc7639);  // sqrdmlsh z25.d, z17.d, z28.d
    // vl128 state = 0x81b1ebaf
    __ dci(0x44dc763b);  // sqrdmlsh z27.d, z17.d, z28.d
    // vl128 state = 0xff9c1896
    __ dci(0x44d4743f);  // sqrdmlsh z31.d, z1.d, z20.d
    // vl128 state = 0x13e84bd0
    __ dci(0x449c742f);  // sqrdmlsh z15.s, z1.s, z28.s
    // vl128 state = 0x717eee33
    __ dci(0x449c7487);  // sqrdmlsh z7.s, z4.s, z28.s
    // vl128 state = 0x5d55ba17
    __ dci(0x449c7485);  // sqrdmlsh z5.s, z4.s, z28.s
    // vl128 state = 0x1a595359
    __ dci(0x448e7481);  // sqrdmlsh z1.s, z4.s, z14.s
    // vl128 state = 0x42eb77bc
    __ dci(0x448e7480);  // sqrdmlsh z0.s, z4.s, z14.s
    // vl128 state = 0x532dc129
    __ dci(0x449c7488);  // sqrdmlsh z8.s, z4.s, z28.s
    // vl128 state = 0x2dee895d
    __ dci(0x441c758a);  // sqrdmlsh z10.b, z12.b, z28.b
    // vl128 state = 0x4ef6b432
    __ dci(0x441475ae);  // sqrdmlsh z14.b, z13.b, z20.b
    // vl128 state = 0x72437e89
    __ dci(0x440075ac);  // sqrdmlsh z12.b, z13.b, z0.b
    // vl128 state = 0x4f9e9633
    __ dci(0x440171bc);  // sqrdmlah z28.b, z13.b, z1.b
    // vl128 state = 0x5fb9f8f4
    __ dci(0x440171b8);  // sqrdmlah z24.b, z13.b, z1.b
    // vl128 state = 0x951cd941
    __ dci(0x441971b9);  // sqrdmlah z25.b, z13.b, z25.b
    // vl128 state = 0x29e34291
    __ dci(0x440970bb);  // sqrdmlah z27.b, z5.b, z9.b
    // vl128 state = 0x3b65ce2d
    __ dci(0x441870ba);  // sqrdmlah z26.b, z5.b, z24.b
    // vl128 state = 0x5390fc4e
    __ dci(0x441270b8);  // sqrdmlah z24.b, z5.b, z18.b
    // vl128 state = 0x3db64e12
    __ dci(0x44927090);  // sqrdmlah z16.s, z4.s, z18.s
    // vl128 state = 0x26a4e6ad
    __ dci(0x44937292);  // sqrdmlah z18.s, z20.s, z19.s
    // vl128 state = 0x6ec4687e
    __ dci(0x4491721a);  // sqrdmlah z26.s, z16.s, z17.s
    // vl128 state = 0x98640c48
    __ dci(0x44907318);  // sqrdmlah z24.s, z24.s, z16.s
    // vl128 state = 0x71e464cd
    __ dci(0x4490737c);  // sqrdmlah z28.s, z27.s, z16.s
    // vl128 state = 0x72b30439
    __ dci(0x4488736c);  // sqrdmlah z12.s, z27.s, z8.s
    // vl128 state = 0xb0004b01
    __ dci(0x44817364);  // sqrdmlah z4.s, z27.s, z1.s
    // vl128 state = 0xedd8d96e
    __ dci(0x44c37365);  // sqrdmlah z5.d, z27.d, z3.d
    // vl128 state = 0xb2ce3f95
    __ dci(0x44c373d5);  // sqrdmlah z21.d, z30.d, z3.d
    // vl128 state = 0x1b27c597
    __ dci(0x44c373dd);  // sqrdmlah z29.d, z30.d, z3.d
    // vl128 state = 0x459d64db
    __ dci(0x444377df);  // sqrdmlsh z31.h, z30.h, z3.h
    // vl128 state = 0x0a86a0bb
    __ dci(0x444277fb);  // sqrdmlsh z27.h, z31.h, z2.h
    // vl128 state = 0x0a7b442d
    __ dci(0x440275eb);  // sqrdmlsh z11.b, z15.b, z2.b
    // vl128 state = 0x47d1f4d8
    __ dci(0x440276ea);  // sqrdmlsh z10.b, z23.b, z2.b
    // vl128 state = 0x18ade1e2
    __ dci(0x440276eb);  // sqrdmlsh z11.b, z23.b, z2.b
    // vl128 state = 0x34498c16
    __ dci(0x440677ef);  // sqrdmlsh z15.b, z31.b, z6.b
    // vl128 state = 0x76b6ce44
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x76b6ce44,
        0x9aed065f,
        0x2dc82c4b,
        0x39dff095,
        0x67905033,
        0x4f1a64f8,
        0x19b9f1d2,
        0x365818b7,
        0xd6d4d404,
        0x955d5613,
        0x8d01f38f,
        0x045e51c7,
        0x40946c78,
        0x7ce16d49,
        0x328f8df9,
        0xb7f6df30,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_integer_pairwise_add_accumulate_long) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 40 * kInstructionSize);
    __ dci(0x4445b4e3);  // uadalp z3.h, p5/m, z7.b
    // vl128 state = 0x3ad015af
    __ dci(0x4445b4e1);  // uadalp z1.h, p5/m, z7.b
    // vl128 state = 0x3f53978b
    __ dci(0x4445bc65);  // uadalp z5.h, p7/m, z3.b
    // vl128 state = 0xf3340744
    __ dci(0x4445be35);  // uadalp z21.h, p7/m, z17.b
    // vl128 state = 0xb6f81377
    __ dci(0x4445be9d);  // uadalp z29.h, p7/m, z20.b
    // vl128 state = 0xaf772b37
    __ dci(0x4444bc9c);  // sadalp z28.h, p7/m, z4.b
    // vl128 state = 0x591be304
    __ dci(0x4444bc9d);  // sadalp z29.h, p7/m, z4.b
    // vl128 state = 0x406d9d34
    __ dci(0x4444ba99);  // sadalp z25.h, p6/m, z20.b
    // vl128 state = 0xb455880f
    __ dci(0x44c4ba09);  // sadalp z9.d, p6/m, z16.s
    // vl128 state = 0x5ef8e2ed
    __ dci(0x44c4ba01);  // sadalp z1.d, p6/m, z16.s
    // vl128 state = 0xca2ccf0d
    __ dci(0x44c4ba11);  // sadalp z17.d, p6/m, z16.s
    // vl128 state = 0x33bb9903
    __ dci(0x4484bb15);  // sadalp z21.s, p6/m, z24.h
    // vl128 state = 0x3964a356
    __ dci(0x4484b957);  // sadalp z23.s, p6/m, z10.h
    // vl128 state = 0x1e1426d2
    __ dci(0x4484b953);  // sadalp z19.s, p6/m, z10.h
    // vl128 state = 0x83e2e1a6
    __ dci(0x4484b943);  // sadalp z3.s, p6/m, z10.h
    // vl128 state = 0x24335149
    __ dci(0x4484b102);  // sadalp z2.s, p4/m, z8.h
    // vl128 state = 0x8bde109a
    __ dci(0x4484bd06);  // sadalp z6.s, p7/m, z8.h
    // vl128 state = 0x5abf30eb
    __ dci(0x4484bdc2);  // sadalp z2.s, p7/m, z14.h
    // vl128 state = 0xcb199381
    __ dci(0x4485b5c6);  // uadalp z6.s, p5/m, z14.h
    // vl128 state = 0x5f3819ad
    __ dci(0x4485b5c2);  // uadalp z2.s, p5/m, z14.h
    // vl128 state = 0x5f6d69e4
    __ dci(0x4485b5ca);  // uadalp z10.s, p5/m, z14.h
    // vl128 state = 0x1a0d7053
    __ dci(0x4485b15a);  // uadalp z26.s, p4/m, z10.h
    // vl128 state = 0x9081b6cd
    __ dci(0x44c5b95e);  // uadalp z30.d, p6/m, z10.s
    // vl128 state = 0x6b15107e
    __ dci(0x44c5a14e);  // uadalp z14.d, p0/m, z10.s
    // vl128 state = 0x4a127dc2
    __ dci(0x4445a1c6);  // uadalp z6.h, p0/m, z14.b
    // vl128 state = 0x06902399
    __ dci(0x4445a1ce);  // uadalp z14.h, p0/m, z14.b
    // vl128 state = 0x1789be4a
    __ dci(0x4444a9de);  // sadalp z30.h, p2/m, z14.b
    // vl128 state = 0x86732543
    __ dci(0x4444adff);  // sadalp z31.h, p3/m, z15.b
    // vl128 state = 0xe326faef
    __ dci(0x4444bdb7);  // sadalp z23.h, p7/m, z13.b
    // vl128 state = 0x46d5f328
    __ dci(0x4444bda7);  // sadalp z7.h, p7/m, z13.b
    // vl128 state = 0x5cf7a973
    __ dci(0x4445bd25);  // uadalp z5.h, p7/m, z9.b
    // vl128 state = 0xdf8cbb97
    __ dci(0x4485bd35);  // uadalp z21.s, p7/m, z9.h
    // vl128 state = 0x330c3d35
    __ dci(0x4485bc17);  // uadalp z23.s, p7/m, z0.h
    // vl128 state = 0x6ebfa4fe
    __ dci(0x4485bc15);  // uadalp z21.s, p7/m, z0.h
    // vl128 state = 0x52f18385
    __ dci(0x4485be91);  // uadalp z17.s, p7/m, z20.h
    // vl128 state = 0x82fa2d85
    __ dci(0x4485be53);  // uadalp z19.s, p7/m, z18.h
    // vl128 state = 0xa7d6098b
    __ dci(0x4485aa52);  // uadalp z18.s, p2/m, z18.h
    // vl128 state = 0xfe8faafa
    __ dci(0x4485ae13);  // uadalp z19.s, p3/m, z16.h
    // vl128 state = 0xf2465f31
    __ dci(0x4485b617);  // uadalp z23.s, p5/m, z16.h
    // vl128 state = 0xed6be8ed
    __ dci(0x4485bc13);  // uadalp z19.s, p7/m, z0.h
    // vl128 state = 0xb2f95c3d
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xb2f95c3d,
        0xa4189170,
        0xed9e7f9e,
        0xfca732cb,
        0x4c94b2d7,
        0x92a2fb21,
        0xbca62a5c,
        0x9aec54d6,
        0x8df82b02,
        0x50c18764,
        0xd27e5a0e,
        0x1a538cc6,
        0x538b673e,
        0x37e4b499,
        0x7160cbd5,
        0x113951bc,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_pmul_mul_vector_unpredicated) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0x04a56309);  // mul z9.s, z24.s, z5.s
    // vl128 state = 0x0ef461d5
    __ dci(0x04a56148);  // mul z8.s, z10.s, z5.s
    // vl128 state = 0xce9f1381
    __ dci(0x04a161d8);  // mul z24.s, z14.s, z1.s
    // vl128 state = 0x2a14ff8c
    __ dci(0x04a16179);  // mul z25.s, z11.s, z1.s
    // vl128 state = 0x88a0241b
    __ dci(0x04b36171);  // mul z17.s, z11.s, z19.s
    // vl128 state = 0x23aea8a6
    __ dci(0x04fb6170);  // mul z16.d, z11.d, z27.d
    // vl128 state = 0x58eaa46d
    __ dci(0x04fb6171);  // mul z17.d, z11.d, z27.d
    // vl128 state = 0xc733a399
    __ dci(0x04fb6350);  // mul z16.d, z26.d, z27.d
    // vl128 state = 0x2806af41
    __ dci(0x04eb6372);  // mul z18.d, z27.d, z11.d
    // vl128 state = 0x5ec775d1
    __ dci(0x04eb6376);  // mul z22.d, z27.d, z11.d
    // vl128 state = 0x40d03f0d
    __ dci(0x04ed637e);  // mul z30.d, z27.d, z13.d
    // vl128 state = 0xe3a61d56
    __ dci(0x04e8637f);  // mul z31.d, z27.d, z8.d
    // vl128 state = 0x2eb4313f
    __ dci(0x04a86337);  // mul z23.s, z25.s, z8.s
    // vl128 state = 0xc68e329e
    __ dci(0x04a86336);  // mul z22.s, z25.s, z8.s
    // vl128 state = 0x177b1a43
    __ dci(0x04ac63be);  // mul z30.s, z29.s, z12.s
    // vl128 state = 0xaaa415dd
    __ dci(0x04ac63d6);  // mul z22.s, z30.s, z12.s
    // vl128 state = 0xaeb212b8
    __ dci(0x042c67d2);  // pmul z18.b, z30.b, z12.b
    // vl128 state = 0xa11be1c8
    __ dci(0x042c65f3);  // pmul z19.b, z15.b, z12.b
    // vl128 state = 0x8dd03a21
    __ dci(0x042e65d2);  // pmul z18.b, z14.b, z14.b
    // vl128 state = 0x83ef9a66
    __ dci(0x042f6550);  // pmul z16.b, z10.b, z15.b
    // vl128 state = 0x6a495368
    __ dci(0x042e6754);  // pmul z20.b, z26.b, z14.b
    // vl128 state = 0x0b6c3ccf
    __ dci(0x042e6750);  // pmul z16.b, z26.b, z14.b
    // vl128 state = 0xa745457f
    __ dci(0x042e6600);  // pmul z0.b, z16.b, z14.b
    // vl128 state = 0x92fe8b9d
    __ dci(0x042e6602);  // pmul z2.b, z16.b, z14.b
    // vl128 state = 0xda39ebe2
    __ dci(0x043f6600);  // pmul z0.b, z16.b, z31.b
    // vl128 state = 0xcc36d223
    __ dci(0x042b6608);  // pmul z8.b, z16.b, z11.b
    // vl128 state = 0x8b94d25a
    __ dci(0x042a6700);  // pmul z0.b, z24.b, z10.b
    // vl128 state = 0x0118ccba
    __ dci(0x042a6710);  // pmul z16.b, z24.b, z10.b
    // vl128 state = 0x4b38543b
    __ dci(0x042a6714);  // pmul z20.b, z24.b, z10.b
    // vl128 state = 0xa54e126f
    __ dci(0x042a6716);  // pmul z22.b, z24.b, z10.b
    // vl128 state = 0x61ad87c9
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x61ad87c9,
        0x82df488f,
        0xc0d7c1a4,
        0x4f86e761,
        0x8d651d7b,
        0x294cf55a,
        0x060ab34c,
        0x1db0e99c,
        0x4b0b59d7,
        0xcee6dfd1,
        0x29575669,
        0x5c1c7922,
        0x4b1957ed,
        0x8bc5712b,
        0x6ac59fdc,
        0x048ce1b5,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_smulh_umulh_vector_unpredicated) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0x04e46c3b);  // umulh z27.d, z1.d, z4.d
    // vl128 state = 0xfb66ba83
    __ dci(0x04ac6c3a);  // umulh z26.s, z1.s, z12.s
    // vl128 state = 0x45cdb9a2
    __ dci(0x04a86e32);  // umulh z18.s, z17.s, z8.s
    // vl128 state = 0x4ad150dc
    __ dci(0x04a86a7a);  // smulh z26.s, z19.s, z8.s
    // vl128 state = 0xbf08e2cb
    __ dci(0x04e86b7b);  // smulh z27.d, z27.d, z8.d
    // vl128 state = 0x51ad0655
    __ dci(0x04ee6b73);  // smulh z19.d, z27.d, z14.d
    // vl128 state = 0xf764bda9
    __ dci(0x04ec6f7b);  // umulh z27.d, z27.d, z12.d
    // vl128 state = 0xc90f20ef
    __ dci(0x04ac6f3a);  // umulh z26.s, z25.s, z12.s
    // vl128 state = 0x9ec08333
    __ dci(0x04ac6f32);  // umulh z18.s, z25.s, z12.s
    // vl128 state = 0x3620406c
    __ dci(0x042e6f3a);  // umulh z26.b, z25.b, z14.b
    // vl128 state = 0x4e18467a
    __ dci(0x042a6b2a);  // smulh z10.b, z25.b, z10.b
    // vl128 state = 0x13c7cd6f
    __ dci(0x042a6b2b);  // smulh z11.b, z25.b, z10.b
    // vl128 state = 0x16a44c1b
    __ dci(0x043a6b03);  // smulh z3.b, z24.b, z26.b
    // vl128 state = 0x9f8f203b
    __ dci(0x047a690b);  // smulh z11.h, z8.h, z26.h
    // vl128 state = 0xce0aa45e
    __ dci(0x047a690a);  // smulh z10.h, z8.h, z26.h
    // vl128 state = 0xb667d59b
    __ dci(0x0479690e);  // smulh z14.h, z8.h, z25.h
    // vl128 state = 0xd76639b7
    __ dci(0x046d690c);  // smulh z12.h, z8.h, z13.h
    // vl128 state = 0x736b227e
    __ dci(0x042f690e);  // smulh z14.b, z8.b, z15.b
    // vl128 state = 0xc0804df9
    __ dci(0x042f69ac);  // smulh z12.b, z13.b, z15.b
    // vl128 state = 0x8a5509f5
    __ dci(0x042f696e);  // smulh z14.b, z11.b, z15.b
    // vl128 state = 0x761f9cf8
    __ dci(0x042e6b6a);  // smulh z10.b, z27.b, z14.b
    // vl128 state = 0x3b5f2705
    __ dci(0x042e6b6e);  // smulh z14.b, z27.b, z14.b
    // vl128 state = 0x53b23a0a
    __ dci(0x04366b6f);  // smulh z15.b, z27.b, z22.b
    // vl128 state = 0x5bd53ce9
    __ dci(0x04766f7f);  // umulh z31.h, z27.h, z22.h
    // vl128 state = 0x701bec8f
    __ dci(0x04746fef);  // umulh z15.h, z31.h, z20.h
    // vl128 state = 0x29697c8c
    __ dci(0x04706dee);  // umulh z14.h, z15.h, z16.h
    // vl128 state = 0x2088f1c2
    __ dci(0x04706c7e);  // umulh z30.h, z3.h, z16.h
    // vl128 state = 0x56224145
    __ dci(0x04306c2e);  // umulh z14.b, z1.b, z16.b
    // vl128 state = 0x2ba58c9c
    __ dci(0x04b06e2a);  // umulh z10.s, z17.s, z16.s
    // vl128 state = 0xb933d058
    __ dci(0x04b56e2e);  // umulh z14.s, z17.s, z21.s
    // vl128 state = 0x184daee9
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x184daee9,
        0x19454232,
        0xa56823a3,
        0xe334897a,
        0xcaa988e1,
        0x614cbf4f,
        0xfaa384e4,
        0x4b45e885,
        0xef930ead,
        0x49304b9a,
        0x4f1d830e,
        0xa41c1a95,
        0xa1ea8d07,
        0x62ca97b4,
        0x15f52cac,
        0xc190cd57,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

}  // namespace aarch64
}  // namespace vixl
