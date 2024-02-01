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

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <sys/mman.h>
#include <unistd.h>

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
    // vl128 state = 0xc0474f3f
    __ dci(0x44dd761b);  // sqrdmlsh z27.d, z16.d, z29.d
    // vl128 state = 0x102712ac
    __ dci(0x44d4760b);  // sqrdmlsh z11.d, z16.d, z20.d
    // vl128 state = 0xe8666aa6
    __ dci(0x44947709);  // sqrdmlsh z9.s, z24.s, z20.s
    // vl128 state = 0xdd18f643
    __ dci(0x4494770b);  // sqrdmlsh z11.s, z24.s, z20.s
    // vl128 state = 0xac4a4d4c
    __ dci(0x44d4773b);  // sqrdmlsh z27.d, z25.d, z20.d
    // vl128 state = 0x1a5447d4
    __ dci(0x44dc7639);  // sqrdmlsh z25.d, z17.d, z28.d
    // vl128 state = 0xf547ac30
    __ dci(0x44dc763b);  // sqrdmlsh z27.d, z17.d, z28.d
    // vl128 state = 0xb42d177a
    __ dci(0x44d4743f);  // sqrdmlsh z31.d, z1.d, z20.d
    // vl128 state = 0xd0da2c6b
    __ dci(0x449c742f);  // sqrdmlsh z15.s, z1.s, z28.s
    // vl128 state = 0xb24c8988
    __ dci(0x449c7487);  // sqrdmlsh z7.s, z4.s, z28.s
    // vl128 state = 0x9e67ddac
    __ dci(0x449c7485);  // sqrdmlsh z5.s, z4.s, z28.s
    // vl128 state = 0xd96b34e2
    __ dci(0x448e7481);  // sqrdmlsh z1.s, z4.s, z14.s
    // vl128 state = 0x81d91007
    __ dci(0x448e7480);  // sqrdmlsh z0.s, z4.s, z14.s
    // vl128 state = 0x901fa692
    __ dci(0x449c7488);  // sqrdmlsh z8.s, z4.s, z28.s
    // vl128 state = 0xeedceee6
    __ dci(0x441c758a);  // sqrdmlsh z10.b, z12.b, z28.b
    // vl128 state = 0x8dc4d389
    __ dci(0x441475ae);  // sqrdmlsh z14.b, z13.b, z20.b
    // vl128 state = 0xb1711932
    __ dci(0x440075ac);  // sqrdmlsh z12.b, z13.b, z0.b
    // vl128 state = 0x8cacf188
    __ dci(0x440171bc);  // sqrdmlah z28.b, z13.b, z1.b
    // vl128 state = 0x9c8b9f4f
    __ dci(0x440171b8);  // sqrdmlah z24.b, z13.b, z1.b
    // vl128 state = 0x562ebefa
    __ dci(0x441971b9);  // sqrdmlah z25.b, z13.b, z25.b
    // vl128 state = 0x1ef60d31
    __ dci(0x440970bb);  // sqrdmlah z27.b, z5.b, z9.b
    // vl128 state = 0x69bd18ee
    __ dci(0x441870ba);  // sqrdmlah z26.b, z5.b, z24.b
    // vl128 state = 0x525b1f84
    __ dci(0x441270b8);  // sqrdmlah z24.b, z5.b, z18.b
    // vl128 state = 0x3c7dadd8
    __ dci(0x44927090);  // sqrdmlah z16.s, z4.s, z18.s
    // vl128 state = 0x276f0567
    __ dci(0x44937292);  // sqrdmlah z18.s, z20.s, z19.s
    // vl128 state = 0x6f0f8bb4
    __ dci(0x4491721a);  // sqrdmlah z26.s, z16.s, z17.s
    // vl128 state = 0x28eb737a
    __ dci(0x44d3721b);  // sqrdmlah z27.d, z16.d, z19.d
    // vl128 state = 0xa3bd1133
    __ dci(0x44d372ab);  // sqrdmlah z11.d, z21.d, z19.d
    // vl128 state = 0x6e81e8fd
    __ dci(0x44d372a3);  // sqrdmlah z3.d, z21.d, z19.d
    // vl128 state = 0x55730750
    __ dci(0x445376a1);  // sqrdmlsh z1.h, z21.h, z19.h
    // vl128 state = 0x7c7afd6d
    __ dci(0x44527685);  // sqrdmlsh z5.h, z20.h, z18.h
    // vl128 state = 0x1c9dc1a1
    __ dci(0x44127495);  // sqrdmlsh z21.b, z4.b, z18.b
    // vl128 state = 0xf2e07e92
    __ dci(0x44127794);  // sqrdmlsh z20.b, z28.b, z18.b
    // vl128 state = 0xc5a2e589
    __ dci(0x44527695);  // sqrdmlsh z21.h, z20.h, z18.h
    // vl128 state = 0x417df395
    __ dci(0x445274dd);  // sqrdmlsh z29.h, z6.h, z18.h
    // vl128 state = 0x2e223308
    __ dci(0x445774df);  // sqrdmlsh z31.h, z6.h, z23.h
    // vl128 state = 0x99047839
    __ dci(0x445775fe);  // sqrdmlsh z30.h, z15.h, z23.h
    // vl128 state = 0x34a4be39
    __ dci(0x445175ff);  // sqrdmlsh z31.h, z15.h, z17.h
    // vl128 state = 0x714b9d66
    __ dci(0x44517557);  // sqrdmlsh z23.h, z10.h, z17.h
    // vl128 state = 0x2aa51ff4
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x2aa51ff4,
        0xde163ba0,
        0x8b237661,
        0x30086cf2,
        0xabf248f0,
        0xcc183608,
        0xa4103141,
        0x521ebe39,
        0xd746470e,
        0x141a51a4,
        0x695a47fd,
        0x0a74d701,
        0xd14bae63,
        0xf967aadb,
        0xdaed8896,
        0x7ba556cb,
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

TEST_SVE(sve2_arith_interleaved_long) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x459289bd);  // ssublbt z29.s, z13.h, z18.h
    // vl128 state = 0xe2e0965a
    __ dci(0x459289bf);  // ssublbt z31.s, z13.h, z18.h
    // vl128 state = 0x64e3e1a3
    __ dci(0x45d689be);  // ssublbt z30.d, z13.s, z22.s
    // vl128 state = 0x02711ec2
    __ dci(0x45d68916);  // ssublbt z22.d, z8.s, z22.s
    // vl128 state = 0x7ff6f63f
    __ dci(0x45968957);  // ssublbt z23.s, z10.h, z22.h
    // vl128 state = 0xa9aace7f
    __ dci(0x45968a55);  // ssublbt z21.s, z18.h, z22.h
    // vl128 state = 0x6007d46c
    __ dci(0x45868251);  // saddlbt z17.s, z18.h, z6.h
    // vl128 state = 0xecea329d
    __ dci(0x45868230);  // saddlbt z16.s, z17.h, z6.h
    // vl128 state = 0xa16880b8
    __ dci(0x45868231);  // saddlbt z17.s, z17.h, z6.h
    // vl128 state = 0xcff73a01
    __ dci(0x458c8235);  // saddlbt z21.s, z17.h, z12.h
    // vl128 state = 0xf6486b24
    __ dci(0x458c8231);  // saddlbt z17.s, z17.h, z12.h
    // vl128 state = 0xa5612e07
    __ dci(0x459c8021);  // saddlbt z1.s, z1.h, z28.h
    // vl128 state = 0xd71ab1e8
    __ dci(0x458c8009);  // saddlbt z9.s, z0.h, z12.h
    // vl128 state = 0xaf74bd16
    __ dci(0x459e800b);  // saddlbt z11.s, z0.h, z30.h
    // vl128 state = 0x96dee616
    __ dci(0x45928003);  // saddlbt z3.s, z0.h, z18.h
    // vl128 state = 0x652e9cca
    __ dci(0x45d28207);  // saddlbt z7.d, z16.s, z18.s
    // vl128 state = 0xc6b07290
    __ dci(0x45da8225);  // saddlbt z5.d, z17.s, z26.s
    // vl128 state = 0x8c74a35d
    __ dci(0x45da830d);  // saddlbt z13.d, z24.s, z26.s
    // vl128 state = 0xff620001
    __ dci(0x45cb8309);  // saddlbt z9.d, z24.s, z11.s
    // vl128 state = 0x2147f374
    __ dci(0x45ca8119);  // saddlbt z25.d, z8.s, z10.s
    // vl128 state = 0x6f961936
    __ dci(0x45ce831d);  // saddlbt z29.d, z24.s, z14.s
    // vl128 state = 0xaa91e68a
    __ dci(0x45ce8135);  // saddlbt z21.d, z9.s, z14.s
    // vl128 state = 0xa5635d0e
    __ dci(0x458e8331);  // saddlbt z17.s, z25.h, z14.h
    // vl128 state = 0xa0705ea7
    __ dci(0x458e8030);  // saddlbt z16.s, z1.h, z14.h
    // vl128 state = 0x397dc4d5
    __ dci(0x458e8271);  // saddlbt z17.s, z19.h, z14.h
    // vl128 state = 0x5e975082
    __ dci(0x458a82e1);  // saddlbt z1.s, z23.h, z10.h
    // vl128 state = 0x048f8dea
    __ dci(0x458a8240);  // saddlbt z0.s, z18.h, z10.h
    // vl128 state = 0xd9104514
    __ dci(0x458a8e50);  // ssubltb z16.s, z18.h, z10.h
    // vl128 state = 0x6afbf8b6
    __ dci(0x45988e58);  // ssubltb z24.s, z18.h, z24.h
    // vl128 state = 0xfe44a2f8
    __ dci(0x45d08e59);  // ssubltb z25.d, z18.s, z16.s
    // vl128 state = 0x050fb0ab
    __ dci(0x45d08e58);  // ssubltb z24.d, z18.s, z16.s
    // vl128 state = 0xc9160f61
    __ dci(0x45d08259);  // saddlbt z25.d, z18.s, z16.s
    // vl128 state = 0x70ae0c4a
    __ dci(0x45d08b51);  // ssublbt z17.d, z26.s, z16.s
    // vl128 state = 0xe627770c
    __ dci(0x45d08970);  // ssublbt z16.d, z11.s, z16.s
    // vl128 state = 0x445fd924
    __ dci(0x45d28d74);  // ssubltb z20.d, z11.s, z18.s
    // vl128 state = 0x8c7dd6c0
    __ dci(0x45c28d56);  // ssubltb z22.d, z10.s, z2.s
    // vl128 state = 0x925de210
    __ dci(0x45c28d52);  // ssubltb z18.d, z10.s, z2.s
    // vl128 state = 0x28b67c05
    __ dci(0x45c48d5a);  // ssubltb z26.d, z10.s, z4.s
    // vl128 state = 0x48e8377c
    __ dci(0x45c18d5b);  // ssubltb z27.d, z10.s, z1.s
    // vl128 state = 0xb46af33e
    __ dci(0x45818d13);  // ssubltb z19.s, z8.h, z1.h
    // vl128 state = 0x12fada0b
    __ dci(0x45818d12);  // ssubltb z18.s, z8.h, z1.h
    // vl128 state = 0xeaeea3cd
    __ dci(0x45858d9a);  // ssubltb z26.s, z12.h, z5.h
    // vl128 state = 0x6d466bd8
    __ dci(0x45858df2);  // ssubltb z18.s, z15.h, z5.h
    // vl128 state = 0x60c67411
    __ dci(0x45c58d62);  // ssubltb z2.d, z11.s, z5.s
    // vl128 state = 0xec3b40ed
    __ dci(0x45c58b72);  // ssublbt z18.d, z27.s, z5.s
    // vl128 state = 0x5b421b0a
    __ dci(0x45858a76);  // ssublbt z22.s, z19.h, z5.h
    // vl128 state = 0x8a0f26e9
    __ dci(0x45878877);  // ssublbt z23.s, z3.h, z7.h
    // vl128 state = 0xc224293b
    __ dci(0x458f8073);  // saddlbt z19.s, z3.h, z15.h
    // vl128 state = 0x9f5c0b50
    __ dci(0x45878051);  // saddlbt z17.s, z2.h, z7.h
    // vl128 state = 0x2ae674c9
    __ dci(0x45838841);  // ssublbt z1.s, z2.h, z3.h
    // vl128 state = 0x1dff4e20
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x1dff4e20,
        0x3d2c11df,
        0x64caeccf,
        0x7940c227,
        0xf5f59485,
        0x7ad48c48,
        0xcde4523b,
        0xcb5849f0,
        0x1e7e9722,
        0x8049333f,
        0x40d95eb3,
        0x628a428d,
        0x1cf123f2,
        0x8d377510,
        0x44a03b91,
        0xabe90e98,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_sqabs_sqneg) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x4448b23a);  // sqabs z26.h, p4/m, z17.h
    // vl128 state = 0x4aadd589
    __ dci(0x4448b23e);  // sqabs z30.h, p4/m, z17.h
    // vl128 state = 0x86da455e
    __ dci(0x4448a21c);  // sqabs z28.h, p0/m, z16.h
    // vl128 state = 0x4eecab5c
    __ dci(0x4408a298);  // sqabs z24.b, p0/m, z20.b
    // vl128 state = 0xf81ee16e
    __ dci(0x4408a0dc);  // sqabs z28.b, p0/m, z6.b
    // vl128 state = 0x84b94ec5
    __ dci(0x4408a0de);  // sqabs z30.b, p0/m, z6.b
    // vl128 state = 0x626db033
    __ dci(0x4408a19c);  // sqabs z28.b, p0/m, z12.b
    // vl128 state = 0x181303a1
    __ dci(0x4408a3d4);  // sqabs z20.b, p0/m, z30.b
    // vl128 state = 0xf4e93ff3
    __ dci(0x4489a3dc);  // sqneg z28.s, p0/m, z30.s
    // vl128 state = 0xffe7a865
    __ dci(0x4409a1d4);  // sqneg z20.b, p0/m, z14.b
    // vl128 state = 0x6a27d8fe
    __ dci(0x4408a3d0);  // sqabs z16.b, p0/m, z30.b
    // vl128 state = 0x9ffc0414
    __ dci(0x44c8a3d8);  // sqabs z24.d, p0/m, z30.d
    // vl128 state = 0xd59acd78
    __ dci(0x44c8b3fa);  // sqabs z26.d, p4/m, z31.d
    // vl128 state = 0x8853f8ac
    __ dci(0x44c8a2fb);  // sqabs z27.d, p0/m, z23.d
    // vl128 state = 0x439e9079
    __ dci(0x44c8a2f9);  // sqabs z25.d, p0/m, z23.d
    // vl128 state = 0xbaaa56a6
    __ dci(0x4488a2db);  // sqabs z27.s, p0/m, z22.s
    // vl128 state = 0x328cbd5a
    __ dci(0x4488a2df);  // sqabs z31.s, p0/m, z22.s
    // vl128 state = 0x4a74b2da
    __ dci(0x4488a2cf);  // sqabs z15.s, p0/m, z22.s
    // vl128 state = 0x52af62a6
    __ dci(0x4488a04b);  // sqabs z11.s, p0/m, z2.s
    // vl128 state = 0xa45aef42
    __ dci(0x4488a02f);  // sqabs z15.s, p0/m, z1.s
    // vl128 state = 0x0b5444ed
    __ dci(0x4489a06d);  // sqneg z13.s, p0/m, z3.s
    // vl128 state = 0x6f0912d5
    __ dci(0x4489a449);  // sqneg z9.s, p1/m, z2.s
    // vl128 state = 0x669ac78a
    __ dci(0x4489a50b);  // sqneg z11.s, p1/m, z8.s
    // vl128 state = 0x58ae27ee
    __ dci(0x4488a71b);  // sqabs z27.s, p1/m, z24.s
    // vl128 state = 0xa54925f9
    __ dci(0x4408a519);  // sqabs z25.b, p1/m, z8.b
    // vl128 state = 0x45c13095
    __ dci(0x4408a158);  // sqabs z24.b, p0/m, z10.b
    // vl128 state = 0x2d6d547a
    __ dci(0x4488a168);  // sqabs z8.s, p0/m, z11.s
    // vl128 state = 0xc976b77b
    __ dci(0x44c9a16c);  // sqneg z12.d, p0/m, z11.d
    // vl128 state = 0x766e750f
    __ dci(0x44c9a17c);  // sqneg z28.d, p0/m, z11.d
    // vl128 state = 0xbf22858d
    __ dci(0x44c9a878);  // sqneg z24.d, p2/m, z3.d
    // vl128 state = 0xe563a474
    __ dci(0x44c9a8d9);  // sqneg z25.d, p2/m, z6.d
    // vl128 state = 0x573c2648
    __ dci(0x44c9b85b);  // sqneg z27.d, p6/m, z2.d
    // vl128 state = 0x03cdf714
    __ dci(0x4449b87f);  // sqneg z31.h, p6/m, z3.h
    // vl128 state = 0xff4e2cb1
    __ dci(0x4449b81d);  // sqneg z29.h, p6/m, z0.h
    // vl128 state = 0xaab7065e
    __ dci(0x4449a895);  // sqneg z21.h, p2/m, z4.h
    // vl128 state = 0x60d4a6d3
    __ dci(0x4449a825);  // sqneg z5.h, p2/m, z1.h
    // vl128 state = 0x3bed34e4
    __ dci(0x4449a821);  // sqneg z1.h, p2/m, z1.h
    // vl128 state = 0xaa750880
    __ dci(0x4449a820);  // sqneg z0.h, p2/m, z1.h
    // vl128 state = 0xfca9d635
    __ dci(0x4449a822);  // sqneg z2.h, p2/m, z1.h
    // vl128 state = 0x8a92f3e7
    __ dci(0x4449ae23);  // sqneg z3.h, p3/m, z17.h
    // vl128 state = 0xc2db1ac5
    __ dci(0x4449af73);  // sqneg z19.h, p3/m, z27.h
    // vl128 state = 0x386f5f27
    __ dci(0x4449af77);  // sqneg z23.h, p3/m, z27.h
    // vl128 state = 0xff4fd505
    __ dci(0x4489af67);  // sqneg z7.s, p3/m, z27.s
    // vl128 state = 0x4c897605
    __ dci(0x4489ad25);  // sqneg z5.s, p3/m, z9.s
    // vl128 state = 0xcc73333a
    __ dci(0x4409ad07);  // sqneg z7.b, p3/m, z8.b
    // vl128 state = 0x58d37b50
    __ dci(0x4489ad85);  // sqneg z5.s, p3/m, z12.s
    // vl128 state = 0x2a142b9d
    __ dci(0x44c9a984);  // sqneg z4.d, p2/m, z12.d
    // vl128 state = 0x006fd35a
    __ dci(0x44c9a926);  // sqneg z6.d, p2/m, z9.d
    // vl128 state = 0x06c05c5d
    __ dci(0x4449ab2e);  // sqneg z14.h, p2/m, z25.h
    // vl128 state = 0xe41a6fc4
    __ dci(0x4449ab3e);  // sqneg z30.h, p2/m, z25.h
    // vl128 state = 0x6e574bec
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x6e574bec,
        0xec677945,
        0xe7357ba7,
        0xbbf92859,
        0x3f42d943,
        0xe2db0bb1,
        0x704d1161,
        0xc0e1f809,
        0x887dd5e7,
        0x452b8b80,
        0xcf455511,
        0x821ad0bc,
        0xb98b1eac,
        0x49ae6871,
        0x16b2e0a6,
        0xaba4d260,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_urecpe_ursqrte) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 20 * kInstructionSize);
    __ dci(0x4481bee8);  // ursqrte z8.s, p7/m, z23.s
    // vl128 state = 0x38c317d5
    __ dci(0x4480bea9);  // urecpe z9.s, p7/m, z21.s
    // vl128 state = 0x8412e46d
    __ dci(0x4481bfab);  // ursqrte z11.s, p7/m, z29.s
    // vl128 state = 0xae6c2805
    __ dci(0x4481b9a3);  // ursqrte z3.s, p6/m, z13.s
    // vl128 state = 0x114331ab
    __ dci(0x4481aba2);  // ursqrte z2.s, p2/m, z29.s
    // vl128 state = 0x88f2308d
    __ dci(0x4480abe6);  // urecpe z6.s, p2/m, z31.s
    // vl128 state = 0x328b45b8
    __ dci(0x4480afa2);  // urecpe z2.s, p3/m, z29.s
    // vl128 state = 0x7b67ded4
    __ dci(0x4480ae23);  // urecpe z3.s, p3/m, z17.s
    // vl128 state = 0x48d1ac45
    __ dci(0x4481aa27);  // ursqrte z7.s, p2/m, z17.s
    // vl128 state = 0x475f61b6
    __ dci(0x4481a325);  // ursqrte z5.s, p0/m, z25.s
    // vl128 state = 0xfbf0b767
    __ dci(0x4481a321);  // ursqrte z1.s, p0/m, z25.s
    // vl128 state = 0x31481484
    __ dci(0x4481ab05);  // ursqrte z5.s, p2/m, z24.s
    // vl128 state = 0x5aca5e43
    __ dci(0x4481a995);  // ursqrte z21.s, p2/m, z12.s
    // vl128 state = 0xe3b96378
    __ dci(0x4481bb91);  // ursqrte z17.s, p6/m, z28.s
    // vl128 state = 0x9d469964
    __ dci(0x4481b199);  // ursqrte z25.s, p4/m, z12.s
    // vl128 state = 0xbbabbb9d
    __ dci(0x4481a989);  // ursqrte z9.s, p2/m, z12.s
    // vl128 state = 0xf83e651c
    __ dci(0x4481b18b);  // ursqrte z11.s, p4/m, z12.s
    // vl128 state = 0x70a808da
    __ dci(0x4480b089);  // urecpe z9.s, p4/m, z4.s
    // vl128 state = 0x427916ac
    __ dci(0x4480b2c1);  // urecpe z1.s, p4/m, z22.s
    // vl128 state = 0xbf35be88
    __ dci(0x4480aad1);  // urecpe z17.s, p2/m, z22.s
    // vl128 state = 0xaf69727b
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xaf69727b,
        0x7fda1a01,
        0xd299e078,
        0x9a794a84,
        0x47a453c1,
        0xecc67cf0,
        0x04122ec2,
        0x82dd5669,
        0xcb2bb910,
        0xcc73c54c,
        0x4660030f,
        0x7c42b056,
        0x498a73b1,
        0x1de89fad,
        0x5411c616,
        0x9f378bac,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_arith_long) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x45573eac);  // uabdlt z12.h, z21.b, z23.b
    // vl128 state = 0x2ee2e7d4
    __ dci(0x45573c84);  // uabdlt z4.h, z4.b, z23.b
    // vl128 state = 0x33413c6f
    __ dci(0x45571d8c);  // usublt z12.h, z12.b, z23.b
    // vl128 state = 0xb95ffb7e
    __ dci(0x45971d8e);  // usublt z14.s, z12.h, z23.h
    // vl128 state = 0xac4d0015
    __ dci(0x45d7158c);  // ssublt z12.d, z12.s, z23.s
    // vl128 state = 0xe5341703
    __ dci(0x4557119c);  // ssublb z28.h, z12.b, z23.b
    // vl128 state = 0x744f8598
    __ dci(0x45d5118c);  // ssublb z12.d, z12.s, z21.s
    // vl128 state = 0x120c8bf7
    __ dci(0x45551088);  // ssublb z8.h, z4.b, z21.b
    // vl128 state = 0xbf53c9ed
    __ dci(0x455410cc);  // ssublb z12.h, z6.b, z20.b
    // vl128 state = 0x2642a908
    __ dci(0x454414c8);  // ssublt z8.h, z6.b, z4.b
    // vl128 state = 0x0682c7d0
    __ dci(0x454510c9);  // ssublb z9.h, z6.b, z5.b
    // vl128 state = 0x1966420e
    __ dci(0x455510ed);  // ssublb z13.h, z7.b, z21.b
    // vl128 state = 0xdd0ec707
    __ dci(0x455508ef);  // uaddlb z15.h, z7.b, z21.b
    // vl128 state = 0x0756dbf9
    __ dci(0x455502e7);  // saddlb z7.h, z23.b, z21.b
    // vl128 state = 0xb991e688
    __ dci(0x455d06f7);  // saddlt z23.h, z23.b, z29.b
    // vl128 state = 0x55399de0
    __ dci(0x455f06df);  // saddlt z31.h, z22.b, z31.b
    // vl128 state = 0x3379dce4
    __ dci(0x45de06db);  // saddlt z27.d, z22.s, z30.s
    // vl128 state = 0xebf6b857
    __ dci(0x45c606da);  // saddlt z26.d, z22.s, z6.s
    // vl128 state = 0x7625ec15
    __ dci(0x45c306db);  // saddlt z27.d, z22.s, z3.s
    // vl128 state = 0x549988fd
    __ dci(0x455306d3);  // saddlt z19.h, z22.b, z19.b
    // vl128 state = 0xb645cb0f
    __ dci(0x455306d1);  // saddlt z17.h, z22.b, z19.b
    // vl128 state = 0x20a70427
    __ dci(0x455306d3);  // saddlt z19.h, z22.b, z19.b
    // vl128 state = 0xd263ec78
    __ dci(0x45510edb);  // uaddlt z27.h, z22.b, z17.b
    // vl128 state = 0xeecd9b44
    __ dci(0x45510bdf);  // uaddlb z31.h, z30.b, z17.b
    // vl128 state = 0x0577c3d4
    __ dci(0x45d10b4f);  // uaddlb z15.d, z26.s, z17.s
    // vl128 state = 0xca18b475
    __ dci(0x45810b47);  // uaddlb z7.s, z26.h, z1.h
    // vl128 state = 0xdfe68417
    __ dci(0x45811bc3);  // usublb z3.s, z30.h, z1.h
    // vl128 state = 0x96fe0360
    __ dci(0x45891b82);  // usublb z2.s, z28.h, z9.h
    // vl128 state = 0x7e58a9d5
    __ dci(0x4589398a);  // uabdlb z10.s, z12.h, z9.h
    // vl128 state = 0xd7612435
    __ dci(0x458919ab);  // usublb z11.s, z13.h, z9.h
    // vl128 state = 0x8842dbca
    __ dci(0x45cb19af);  // usublb z15.d, z13.s, z11.s
    // vl128 state = 0xfcac3d0f
    __ dci(0x45cb19bf);  // usublb z31.d, z13.s, z11.s
    // vl128 state = 0x7b4952d6
    __ dci(0x45cb190f);  // usublb z15.d, z8.s, z11.s
    // vl128 state = 0xb41cb8a3
    __ dci(0x45cb1d8d);  // usublt z13.d, z12.s, z11.s
    // vl128 state = 0x9197543e
    __ dci(0x45cb1d89);  // usublt z9.d, z12.s, z11.s
    // vl128 state = 0x3cc7e16c
    __ dci(0x454b0d8b);  // uaddlt z11.h, z12.b, z11.b
    // vl128 state = 0x5c52744d
    __ dci(0x45cb1d8a);  // usublt z10.d, z12.s, z11.s
    // vl128 state = 0x24c91c53
    __ dci(0x454f1d8e);  // usublt z14.h, z12.b, z15.b
    // vl128 state = 0x0091f2f1
    __ dci(0x455b1d8f);  // usublt z15.h, z12.b, z27.b
    // vl128 state = 0x521f94f7
    __ dci(0x455a1c87);  // usublt z7.h, z4.b, z26.b
    // vl128 state = 0xa0631870
    __ dci(0x454a1cb7);  // usublt z23.h, z5.b, z10.b
    // vl128 state = 0x089384c7
    __ dci(0x454218a7);  // usublb z7.h, z5.b, z2.b
    // vl128 state = 0xe8c3c063
    __ dci(0x454a19a6);  // usublb z6.h, z13.b, z10.b
    // vl128 state = 0x7a9f53ab
    __ dci(0x454a3da2);  // uabdlt z2.h, z13.b, z10.b
    // vl128 state = 0x68d5f375
    __ dci(0x45423ca6);  // uabdlt z6.h, z5.b, z2.b
    // vl128 state = 0x2c980ff7
    __ dci(0x454a34a7);  // sabdlt z7.h, z5.b, z10.b
    // vl128 state = 0xe38196aa
    __ dci(0x454a3466);  // sabdlt z6.h, z3.b, z10.b
    // vl128 state = 0x86c5bcb2
    __ dci(0x454b146e);  // ssublt z14.h, z3.b, z11.b
    // vl128 state = 0xf8527375
    __ dci(0x454b146a);  // ssublt z10.h, z3.b, z11.b
    // vl128 state = 0xf4bfb710
    __ dci(0x454b147a);  // ssublt z26.h, z3.b, z11.b
    // vl128 state = 0xe1000ccf
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xe1000ccf,
        0xd320fd27,
        0x356a62d9,
        0xc6245994,
        0x78aeec8a,
        0xb5d0402b,
        0x06684b9e,
        0x6033f51d,
        0xd174ee86,
        0x80baaecc,
        0x2c9b263c,
        0x3fba551a,
        0x489fb8b7,
        0x862c9b27,
        0xc0549096,
        0xa927d570,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_arith_wide) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x45494683);  // saddwt z3.h, z20.h, z9.b
    // vl128 state = 0x9a3fc71a
    __ dci(0x45494687);  // saddwt z7.h, z20.h, z9.b
    // vl128 state = 0xb016cb2f
    __ dci(0x454b46d7);  // saddwt z23.h, z22.h, z11.b
    // vl128 state = 0x5ce3d8a0
    __ dci(0x455b56d5);  // ssubwt z21.h, z22.h, z27.b
    // vl128 state = 0xbace5453
    __ dci(0x455b567d);  // ssubwt z29.h, z19.h, z27.b
    // vl128 state = 0x1f510928
    __ dci(0x455b506d);  // ssubwb z13.h, z3.h, z27.b
    // vl128 state = 0x19ea553e
    __ dci(0x4559502f);  // ssubwb z15.h, z1.h, z25.b
    // vl128 state = 0x4d88e5db
    __ dci(0x45d95427);  // ssubwt z7.d, z1.d, z25.s
    // vl128 state = 0x069804b6
    __ dci(0x45d95426);  // ssubwt z6.d, z1.d, z25.s
    // vl128 state = 0xfe46cf10
    __ dci(0x45db5c36);  // usubwt z22.d, z1.d, z27.s
    // vl128 state = 0xad3c8120
    __ dci(0x45d95d37);  // usubwt z23.d, z9.d, z25.s
    // vl128 state = 0x833d76fb
    __ dci(0x45d55d27);  // usubwt z7.d, z9.d, z21.s
    // vl128 state = 0xc536845d
    __ dci(0x45d44d25);  // uaddwt z5.d, z9.d, z20.s
    // vl128 state = 0x21f5a29c
    __ dci(0x45dc4927);  // uaddwb z7.d, z9.d, z28.s
    // vl128 state = 0xfe67da2a
    __ dci(0x455c490f);  // uaddwb z15.h, z8.h, z28.b
    // vl128 state = 0x5ec5d506
    __ dci(0x455c490b);  // uaddwb z11.h, z8.h, z28.b
    // vl128 state = 0x74b7d2fc
    __ dci(0x45584923);  // uaddwb z3.h, z9.h, z24.b
    // vl128 state = 0xa785f3c3
    __ dci(0x45584922);  // uaddwb z2.h, z9.h, z24.b
    // vl128 state = 0x373049c0
    __ dci(0x45584940);  // uaddwb z0.h, z10.h, z24.b
    // vl128 state = 0xbf385483
    __ dci(0x45da4944);  // uaddwb z4.d, z10.d, z26.s
    // vl128 state = 0x94cd3b86
    __ dci(0x45524945);  // uaddwb z5.h, z10.h, z18.b
    // vl128 state = 0x8535094f
    __ dci(0x4540494d);  // uaddwb z13.h, z10.h, z0.b
    // vl128 state = 0x328abbdb
    __ dci(0x45c04909);  // uaddwb z9.d, z8.d, z0.s
    // vl128 state = 0x253064cb
    __ dci(0x45c8498d);  // uaddwb z13.d, z12.d, z8.s
    // vl128 state = 0xa1b39fe0
    __ dci(0x45c0418f);  // saddwb z15.d, z12.d, z0.s
    // vl128 state = 0xa72048d9
    __ dci(0x45d84187);  // saddwb z7.d, z12.d, z24.s
    // vl128 state = 0x4c8a23ac
    __ dci(0x45dc5197);  // ssubwb z23.d, z12.d, z28.s
    // vl128 state = 0x352a3d60
    __ dci(0x45dc5d93);  // usubwt z19.d, z12.d, z28.s
    // vl128 state = 0x404b9e8b
    __ dci(0x45dd5592);  // ssubwt z18.d, z12.d, z29.s
    // vl128 state = 0xf46cc758
    __ dci(0x45dd5550);  // ssubwt z16.d, z10.d, z29.s
    // vl128 state = 0x171ebd36
    __ dci(0x45cd55d4);  // ssubwt z20.d, z14.d, z13.s
    // vl128 state = 0x4f2ef46f
    __ dci(0x45dd5dd5);  // usubwt z21.d, z14.d, z29.s
    // vl128 state = 0x0c9ab301
    __ dci(0x45dd5dc5);  // usubwt z5.d, z14.d, z29.s
    // vl128 state = 0x67a10e22
    __ dci(0x454d5dd5);  // usubwt z21.h, z14.h, z13.b
    // vl128 state = 0xb4bd21c0
    __ dci(0x454d4dfd);  // uaddwt z29.h, z15.h, z13.b
    // vl128 state = 0x8df5f90f
    __ dci(0x45494fed);  // uaddwt z13.h, z31.h, z9.b
    // vl128 state = 0x913f7aa4
    __ dci(0x45cb4fef);  // uaddwt z15.d, z31.d, z11.s
    // vl128 state = 0xa23d1307
    __ dci(0x454b47ff);  // saddwt z31.h, z31.h, z11.b
    // vl128 state = 0x026ff306
    __ dci(0x454747f7);  // saddwt z23.h, z31.h, z7.b
    // vl128 state = 0x9abf0566
    __ dci(0x45c743f6);  // saddwb z22.d, z31.d, z7.s
    // vl128 state = 0x27031d0e
    __ dci(0x45c74b66);  // uaddwb z6.d, z27.d, z7.s
    // vl128 state = 0xc6f3a976
    __ dci(0x45474be4);  // uaddwb z4.h, z31.h, z7.b
    // vl128 state = 0xededea24
    __ dci(0x454349e0);  // uaddwb z0.h, z15.h, z3.b
    // vl128 state = 0xf1092d40
    __ dci(0x454359c1);  // usubwb z1.h, z14.h, z3.b
    // vl128 state = 0x2d96f026
    __ dci(0x45535983);  // usubwb z3.h, z12.h, z19.b
    // vl128 state = 0x5a9cab0c
    __ dci(0x45535981);  // usubwb z1.h, z12.h, z19.b
    // vl128 state = 0x7f8d695f
    __ dci(0x45535a83);  // usubwb z3.h, z20.h, z19.b
    // vl128 state = 0xb0ae0f62
    __ dci(0x45d35e81);  // usubwt z1.d, z20.d, z19.s
    // vl128 state = 0xfe7e227b
    __ dci(0x45d25ec9);  // usubwt z9.d, z22.d, z18.s
    // vl128 state = 0xed9dd734
    __ dci(0x45d35e88);  // usubwt z8.d, z20.d, z19.s
    // vl128 state = 0x943f8d24
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x943f8d24,
        0xfe956248,
        0xfefddb40,
        0x4d92bfb3,
        0x01dcd5b1,
        0x29a23c92,
        0xb7587530,
        0xa56fa28c,
        0xa0f8590d,
        0xa6b883a4,
        0x2e50d1fd,
        0x8e976f55,
        0xb21bd3b1,
        0x0c3586e5,
        0xe3d7e7e6,
        0xb1e0e34f,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_shift_long) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x4518aafc);  // ushllb z28.s, z23.h, #8
    // vl128 state = 0x07dfb216
    __ dci(0x4518afec);  // ushllt z12.s, z31.h, #8
    // vl128 state = 0xe3c5d68c
    __ dci(0x4518adc4);  // ushllt z4.s, z14.h, #8
    // vl128 state = 0xce8721fc
    __ dci(0x4518a1c5);  // sshllb z5.s, z14.h, #8
    // vl128 state = 0x71820bae
    __ dci(0x4508a9cd);  // ushllb z13.h, z14.b, #0
    // vl128 state = 0xfdc3f7b3
    __ dci(0x4508ad9d);  // ushllt z29.h, z12.b, #0
    // vl128 state = 0x93c1f606
    __ dci(0x4508a795);  // sshllt z21.h, z28.b, #0
    // vl128 state = 0x15ebcb72
    __ dci(0x450caf94);  // ushllt z20.h, z28.b, #4
    // vl128 state = 0x76c630f5
    __ dci(0x4508afd6);  // ushllt z22.h, z30.b, #0
    // vl128 state = 0xa9c6dfbc
    __ dci(0x4509aed7);  // ushllt z23.h, z22.b, #1
    // vl128 state = 0xa5942073
    __ dci(0x4508ae55);  // ushllt z21.h, z18.b, #0
    // vl128 state = 0xe4348777
    __ dci(0x450cac51);  // ushllt z17.h, z2.b, #4
    // vl128 state = 0x91c6e6ea
    __ dci(0x450ca870);  // ushllb z16.h, z3.b, #4
    // vl128 state = 0x40393ae8
    __ dci(0x450ca031);  // sshllb z17.h, z1.b, #4
    // vl128 state = 0x8b9526e8
    __ dci(0x450aa030);  // sshllb z16.h, z1.b, #2
    // vl128 state = 0xd3d0857a
    __ dci(0x450aa031);  // sshllb z17.h, z1.b, #2
    // vl128 state = 0xbdd18de2
    __ dci(0x450ba233);  // sshllb z19.h, z17.b, #3
    // vl128 state = 0x5e5f6f2a
    __ dci(0x4509a263);  // sshllb z3.h, z19.b, #1
    // vl128 state = 0xa3b5427b
    __ dci(0x450da673);  // sshllt z19.h, z19.b, #5
    // vl128 state = 0x97472b22
    __ dci(0x451da477);  // sshllt z23.s, z3.h, #13
    // vl128 state = 0xe6da4012
    __ dci(0x451da5f6);  // sshllt z22.s, z15.h, #13
    // vl128 state = 0x11630552
    __ dci(0x450da5b4);  // sshllt z20.h, z13.b, #5
    // vl128 state = 0xe9a4cad0
    __ dci(0x450da5d5);  // sshllt z21.h, z14.b, #5
    // vl128 state = 0x750d4143
    __ dci(0x450fa4d7);  // sshllt z23.h, z6.b, #7
    // vl128 state = 0xc441984c
    __ dci(0x451ba4df);  // sshllt z31.s, z6.h, #11
    // vl128 state = 0x9a3899af
    __ dci(0x451ba4db);  // sshllt z27.s, z6.h, #11
    // vl128 state = 0xbb6684bb
    __ dci(0x451ba4bf);  // sshllt z31.s, z5.h, #11
    // vl128 state = 0x45a2cf1e
    __ dci(0x451aa49b);  // sshllt z27.s, z4.h, #10
    // vl128 state = 0xac10df2f
    __ dci(0x451aa49f);  // sshllt z31.s, z4.h, #10
    // vl128 state = 0x9cecdbd8
    __ dci(0x451aa89b);  // ushllb z27.s, z4.h, #10
    // vl128 state = 0x73fca806
    __ dci(0x4518aa9f);  // ushllb z31.s, z20.h, #8
    // vl128 state = 0xf58883fb
    __ dci(0x451aaab7);  // ushllb z23.s, z21.h, #10
    // vl128 state = 0xf9476b16
    __ dci(0x4508aaa7);  // ushllb z7.h, z21.b, #0
    // vl128 state = 0x6f65ea0e
    __ dci(0x4508ae2f);  // ushllt z15.h, z17.b, #0
    // vl128 state = 0x574341e2
    __ dci(0x4509ac27);  // ushllt z7.h, z1.b, #1
    // vl128 state = 0xe373d23c
    __ dci(0x450dae25);  // ushllt z5.h, z17.b, #5
    // vl128 state = 0xc6ad882b
    __ dci(0x4509aea7);  // ushllt z7.h, z21.b, #1
    // vl128 state = 0xfce8617d
    __ dci(0x4509adb7);  // ushllt z23.h, z13.b, #1
    // vl128 state = 0x30f63baf
    __ dci(0x4549ade7);  // ushllt z7.d, z15.s, #9
    // vl128 state = 0x20522e02
    __ dci(0x4549adf7);  // ushllt z23.d, z15.s, #9
    // vl128 state = 0x18c6aade
    __ dci(0x4548aff6);  // ushllt z22.d, z31.s, #8
    // vl128 state = 0x3ad49ec9
    __ dci(0x4548affe);  // ushllt z30.d, z31.s, #8
    // vl128 state = 0x828be22f
    __ dci(0x4548adda);  // ushllt z26.d, z14.s, #8
    // vl128 state = 0xb4997aa9
    __ dci(0x4544add2);  // ushllt z18.d, z14.s, #4
    // vl128 state = 0x6e7feb55
    __ dci(0x454cad42);  // ushllt z2.d, z10.s, #12
    // vl128 state = 0xb8ff410d
    __ dci(0x450dad40);  // ushllt z0.h, z10.b, #5
    // vl128 state = 0x806bb38f
    __ dci(0x4515ad50);  // ushllt z16.s, z10.h, #5
    // vl128 state = 0x6bd247ad
    __ dci(0x4557ad51);  // ushllt z17.d, z10.s, #23
    // vl128 state = 0xc0959f27
    __ dci(0x4557ad41);  // ushllt z1.d, z10.s, #23
    // vl128 state = 0xf0176482
    __ dci(0x4557ad40);  // ushllt z0.d, z10.s, #23
    // vl128 state = 0xd5c958bf
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xd5c958bf,
        0xb7546431,
        0xee4f6b9f,
        0x74f31aeb,
        0x98282a7a,
        0xf2423509,
        0xe3ae7c5c,
        0xe544e7ba,
        0x7d52fba5,
        0x1520b68d,
        0xee539501,
        0x1a65ba45,
        0x0d4c2383,
        0x9f4a30c5,
        0xca6662a2,
        0x64dc5f23,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_shift_narrow) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x456b1458);  // shrnt z24.s, z2.d, #21
    // vl128 state = 0x70323182
    __ dci(0x456b145c);  // shrnt z28.s, z2.d, #21
    // vl128 state = 0x1d620da3
    __ dci(0x45291454);  // shrnt z20.b, z2.h, #7
    // vl128 state = 0x8e6d3a55
    __ dci(0x4539141c);  // shrnt z28.h, z0.s, #7
    // vl128 state = 0xbc19c1cc
    __ dci(0x453914b8);  // shrnt z24.h, z5.s, #7
    // vl128 state = 0x0bd4d1e8
    __ dci(0x453b14f9);  // shrnt z25.h, z7.s, #5
    // vl128 state = 0x15622295
    __ dci(0x453315fd);  // shrnt z29.h, z15.s, #13
    // vl128 state = 0x45bf3b94
    __ dci(0x45331d75);  // rshrnt z21.h, z11.s, #13
    // vl128 state = 0xbb3574e6
    __ dci(0x45331945);  // rshrnb z5.h, z10.s, #13
    // vl128 state = 0x7b72be5f
    __ dci(0x45331941);  // rshrnb z1.h, z10.s, #13
    // vl128 state = 0x073cdf1a
    __ dci(0x45331949);  // rshrnb z9.h, z10.s, #13
    // vl128 state = 0x3ecd1bf9
    __ dci(0x453b1979);  // rshrnb z25.h, z11.s, #5
    // vl128 state = 0x19f7734e
    __ dci(0x453b11f1);  // shrnb z17.h, z15.s, #5
    // vl128 state = 0x47a3f036
    __ dci(0x453711f9);  // shrnb z25.h, z15.s, #9
    // vl128 state = 0xff283fe4
    __ dci(0x453315f8);  // shrnt z24.h, z15.s, #13
    // vl128 state = 0x1c19f8fb
    __ dci(0x453319f0);  // rshrnb z16.h, z15.s, #13
    // vl128 state = 0x3be08052
    __ dci(0x453b1972);  // rshrnb z18.h, z11.s, #5
    // vl128 state = 0xc5ae76a0
    __ dci(0x453b1962);  // rshrnb z2.h, z11.s, #5
    // vl128 state = 0x75ec3872
    __ dci(0x453b1c60);  // rshrnt z0.h, z3.s, #5
    // vl128 state = 0x9b372229
    __ dci(0x45331c44);  // rshrnt z4.h, z2.s, #13
    // vl128 state = 0xe4e22904
    __ dci(0x45371c0c);  // rshrnt z12.h, z0.s, #9
    // vl128 state = 0x12bc6f4b
    __ dci(0x45331d08);  // rshrnt z8.h, z8.s, #13
    // vl128 state = 0x3ef95245
    __ dci(0x45331c98);  // rshrnt z24.h, z4.s, #13
    // vl128 state = 0x0a4a0d68
    __ dci(0x45731e99);  // rshrnt z25.s, z20.d, #13
    // vl128 state = 0xa01ca6c8
    __ dci(0x457b1a98);  // rshrnb z24.s, z20.d, #5
    // vl128 state = 0x73a50e30
    __ dci(0x452b1a9c);  // rshrnb z28.b, z20.h, #5
    // vl128 state = 0xbad3deda
    __ dci(0x452b1818);  // rshrnb z24.b, z0.h, #5
    // vl128 state = 0x579b3c8f
    __ dci(0x452b181a);  // rshrnb z26.b, z0.h, #5
    // vl128 state = 0xa2b0bf7c
    __ dci(0x452b181b);  // rshrnb z27.b, z0.h, #5
    // vl128 state = 0x7bebdf9e
    __ dci(0x45291a1a);  // rshrnb z26.b, z16.h, #7
    // vl128 state = 0x3f90e1b7
    __ dci(0x45681a12);  // rshrnb z18.s, z16.d, #24
    // vl128 state = 0x57e6295e
    __ dci(0x45681290);  // shrnb z16.s, z20.d, #24
    // vl128 state = 0xa53f48b5
    __ dci(0x45281091);  // shrnb z17.b, z4.h, #8
    // vl128 state = 0x65179ab4
    __ dci(0x45281401);  // shrnt z1.b, z0.h, #8
    // vl128 state = 0x3cc490ba
    __ dci(0x45281c83);  // rshrnt z3.b, z4.h, #8
    // vl128 state = 0x3bc34e69
    __ dci(0x45281c93);  // rshrnt z19.b, z4.h, #8
    // vl128 state = 0x6dded0bb
    __ dci(0x45681cb7);  // rshrnt z23.s, z5.d, #24
    // vl128 state = 0x378f83c0
    __ dci(0x45291cb6);  // rshrnt z22.b, z5.h, #7
    // vl128 state = 0x7e4d1c44
    __ dci(0x45391eb2);  // rshrnt z18.h, z21.s, #7
    // vl128 state = 0x66c0b784
    __ dci(0x45281ea2);  // rshrnt z2.b, z21.h, #8
    // vl128 state = 0x62df2c82
    __ dci(0x452c1fa0);  // rshrnt z0.b, z29.h, #4
    // vl128 state = 0xd79ee307
    __ dci(0x456c1ba2);  // rshrnb z2.s, z29.d, #20
    // vl128 state = 0x8ebb2251
    __ dci(0x45641ab2);  // rshrnb z18.s, z21.d, #28
    // vl128 state = 0x77ec053a
    __ dci(0x456c12ba);  // shrnb z26.s, z21.d, #20
    // vl128 state = 0xcf94b608
    __ dci(0x452812b8);  // shrnb z24.b, z21.h, #8
    // vl128 state = 0x3e067a62
    __ dci(0x4568123a);  // shrnb z26.s, z17.d, #24
    // vl128 state = 0xe451de0f
    __ dci(0x456c1338);  // shrnb z24.s, z25.d, #20
    // vl128 state = 0x4042d707
    __ dci(0x456813b9);  // shrnb z25.s, z29.d, #24
    // vl128 state = 0x5184a2aa
    __ dci(0x456812e9);  // shrnb z9.s, z23.d, #24
    // vl128 state = 0x246344b8
    __ dci(0x456812e1);  // shrnb z1.s, z23.d, #24
    // vl128 state = 0x76866e79
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x76866e79,
        0x42b52927,
        0x84a0bfcc,
        0xf8226fc2,
        0x444f6df5,
        0x2f8dcd68,
        0x5a48278a,
        0x1cdd7f2f,
        0x7816d36c,
        0xebae972f,
        0xa02adfbe,
        0xc93cde0f,
        0xce43287b,
        0x777d6ce0,
        0x9d3be904,
        0x3e059dd2,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_shift_narrow_usat) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x457a3207);  // uqshrnb z7.s, z16.d, #6
    // vl128 state = 0x4b40d14e
    __ dci(0x457a3206);  // uqshrnb z6.s, z16.d, #6
    // vl128 state = 0x4dbc0377
    __ dci(0x457a3204);  // uqshrnb z4.s, z16.d, #6
    // vl128 state = 0xa6fbc7f9
    __ dci(0x457e3a14);  // uqrshrnb z20.s, z16.d, #2
    // vl128 state = 0x9e9414a9
    __ dci(0x457b3a15);  // uqrshrnb z21.s, z16.d, #5
    // vl128 state = 0xe8824afd
    __ dci(0x457b3ab7);  // uqrshrnb z23.s, z21.d, #5
    // vl128 state = 0x81ce1be6
    __ dci(0x457b3ab6);  // uqrshrnb z22.s, z21.d, #5
    // vl128 state = 0x5e343a1e
    __ dci(0x457f3af7);  // uqrshrnb z23.s, z23.d, #1
    // vl128 state = 0x09a5c3a0
    __ dci(0x457b38ff);  // uqrshrnb z31.s, z7.d, #5
    // vl128 state = 0xb50710bf
    __ dci(0x453338fe);  // uqrshrnb z30.h, z7.s, #13
    // vl128 state = 0xfc719c85
    __ dci(0x453338ee);  // uqrshrnb z14.h, z7.s, #13
    // vl128 state = 0x157d826a
    __ dci(0x453b386a);  // uqrshrnb z10.h, z3.s, #5
    // vl128 state = 0x9c735771
    __ dci(0x452f386e);  // uqrshrnb z14.b, z3.h, #1
    // vl128 state = 0xe03bb4a4
    __ dci(0x452f3aea);  // uqrshrnb z10.b, z23.h, #1
    // vl128 state = 0xa841b415
    __ dci(0x452f38ba);  // uqrshrnb z26.b, z5.h, #1
    // vl128 state = 0x55302a6d
    __ dci(0x452f3878);  // uqrshrnb z24.b, z3.h, #1
    // vl128 state = 0x73bee182
    __ dci(0x453f385c);  // uqrshrnb z28.h, z2.s, #1
    // vl128 state = 0x75f81ccc
    __ dci(0x453f397d);  // uqrshrnb z29.h, z11.s, #1
    // vl128 state = 0x856fecc9
    __ dci(0x457d397c);  // uqrshrnb z28.s, z11.d, #3
    // vl128 state = 0x4b144bf2
    __ dci(0x457f3878);  // uqrshrnb z24.s, z3.d, #1
    // vl128 state = 0x7ea5dad3
    __ dci(0x457b3c7a);  // uqrshrnt z26.s, z3.d, #5
    // vl128 state = 0xa7d48543
    __ dci(0x45633c72);  // uqrshrnt z18.s, z3.d, #29
    // vl128 state = 0x18f647a7
    __ dci(0x45613d76);  // uqrshrnt z22.s, z11.d, #31
    // vl128 state = 0x96d4081b
    __ dci(0x45693972);  // uqrshrnb z18.s, z11.d, #23
    // vl128 state = 0xa8369e83
    __ dci(0x45693d53);  // uqrshrnt z19.s, z10.d, #23
    // vl128 state = 0x7553ff55
    __ dci(0x45713d51);  // uqrshrnt z17.s, z10.d, #15
    // vl128 state = 0x52a52ecc
    __ dci(0x45713d99);  // uqrshrnt z25.s, z12.d, #15
    // vl128 state = 0x4de78f7b
    __ dci(0x45753f9d);  // uqrshrnt z29.s, z28.d, #11
    // vl128 state = 0x0f8948cd
    __ dci(0x45753f8d);  // uqrshrnt z13.s, z28.d, #11
    // vl128 state = 0x7f2c1b05
    __ dci(0x45753685);  // uqshrnt z5.s, z20.d, #11
    // vl128 state = 0xbe6f6ea9
    __ dci(0x457d3784);  // uqshrnt z4.s, z28.d, #3
    // vl128 state = 0x716e1acd
    __ dci(0x453c3785);  // uqshrnt z5.h, z28.s, #4
    // vl128 state = 0x828a3cbb
    __ dci(0x453837a4);  // uqshrnt z4.h, z29.s, #8
    // vl128 state = 0x125ddc3c
    __ dci(0x457a37a6);  // uqshrnt z6.s, z29.d, #6
    // vl128 state = 0x8c5c5d4c
    __ dci(0x453a37e4);  // uqshrnt z4.h, z31.s, #6
    // vl128 state = 0xdea9801f
    __ dci(0x453f37ec);  // uqshrnt z12.h, z31.s, #1
    // vl128 state = 0x6caa6537
    __ dci(0x457f37dc);  // uqshrnt z28.s, z30.d, #1
    // vl128 state = 0x66c0c05d
    __ dci(0x45773fde);  // uqrshrnt z30.s, z30.d, #9
    // vl128 state = 0xf8d495e2
    __ dci(0x45653fda);  // uqrshrnt z26.s, z30.d, #27
    // vl128 state = 0xb543c017
    __ dci(0x45613ffb);  // uqrshrnt z27.s, z31.d, #31
    // vl128 state = 0x58a69fb4
    __ dci(0x45613feb);  // uqrshrnt z11.s, z31.d, #31
    // vl128 state = 0xb5a04d48
    __ dci(0x45653fca);  // uqrshrnt z10.s, z30.d, #27
    // vl128 state = 0xd2d445e0
    __ dci(0x45753fe8);  // uqrshrnt z8.s, z31.d, #11
    // vl128 state = 0x67d89d28
    __ dci(0x457537ca);  // uqshrnt z10.s, z30.d, #11
    // vl128 state = 0xcaa2b6dc
    __ dci(0x457d35ce);  // uqshrnt z14.s, z14.d, #3
    // vl128 state = 0x9da6b10f
    __ dci(0x452d35de);  // uqshrnt z30.b, z14.h, #3
    // vl128 state = 0xda8663db
    __ dci(0x452d314e);  // uqshrnb z14.b, z10.h, #3
    // vl128 state = 0x761992a9
    __ dci(0x453d304f);  // uqshrnb z15.h, z2.s, #3
    // vl128 state = 0x71587e6a
    __ dci(0x453d386e);  // uqrshrnb z14.h, z3.s, #3
    // vl128 state = 0xc6118398
    __ dci(0x453538ec);  // uqrshrnb z12.h, z7.s, #11
    // vl128 state = 0x5e542c3a
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x5e542c3a,
        0xd9128c5a,
        0x73f430ed,
        0x160c07da,
        0x7bff9561,
        0x4b2d6335,
        0x3738197c,
        0x2b624a48,
        0xbb257999,
        0x0d5d8614,
        0xb031d1fc,
        0x60f2fce2,
        0x92770ad6,
        0x6e33aa78,
        0x8752089b,
        0x37b56a40,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_shift_narrow_ssat) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x456c0875);  // sqrshrunb z21.s, z3.d, #20
    // vl128 state = 0x1446427d
    __ dci(0x456c0877);  // sqrshrunb z23.s, z3.d, #20
    // vl128 state = 0xd839ea94
    __ dci(0x456c0876);  // sqrshrunb z22.s, z3.d, #20
    // vl128 state = 0xe4dd3104
    __ dci(0x456e0c77);  // sqrshrunt z23.s, z3.d, #18
    // vl128 state = 0xd86dd8aa
    __ dci(0x456e0a73);  // sqrshrunb z19.s, z19.d, #18
    // vl128 state = 0x7aacf973
    __ dci(0x456c0e72);  // sqrshrunt z18.s, z19.d, #20
    // vl128 state = 0x6e7b28b8
    __ dci(0x456c2c62);  // sqrshrnt z2.s, z3.d, #20
    // vl128 state = 0x242e0a5e
    __ dci(0x456c24f2);  // sqshrnt z18.s, z7.d, #20
    // vl128 state = 0xf9c993ec
    __ dci(0x456c2570);  // sqshrnt z16.s, z11.d, #20
    // vl128 state = 0x087c4fc1
    __ dci(0x456e2478);  // sqshrnt z24.s, z3.d, #18
    // vl128 state = 0x33fdae0c
    __ dci(0x456e2c30);  // sqrshrnt z16.s, z1.d, #18
    // vl128 state = 0x0c957ea2
    __ dci(0x456e2d78);  // sqrshrnt z24.s, z11.d, #18
    // vl128 state = 0x0792e58a
    __ dci(0x456f2970);  // sqrshrnb z16.s, z11.d, #17
    // vl128 state = 0xe7169693
    __ dci(0x456b2938);  // sqrshrnb z24.s, z9.d, #21
    // vl128 state = 0x1372a92d
    __ dci(0x45692979);  // sqrshrnb z25.s, z11.d, #23
    // vl128 state = 0xc1c31387
    __ dci(0x4563297d);  // sqrshrnb z29.s, z11.d, #29
    // vl128 state = 0x50a08538
    __ dci(0x45632975);  // sqrshrnb z21.s, z11.d, #29
    // vl128 state = 0xda962f25
    __ dci(0x456309f1);  // sqrshrunb z17.s, z15.d, #29
    // vl128 state = 0xe149814e
    __ dci(0x457308f3);  // sqrshrunb z19.s, z7.d, #13
    // vl128 state = 0x6d5ea38b
    __ dci(0x457329fb);  // sqrshrnb z27.s, z15.d, #13
    // vl128 state = 0xee932acb
    __ dci(0x457721f3);  // sqshrnb z19.s, z15.d, #9
    // vl128 state = 0x7e05914b
    __ dci(0x45732171);  // sqshrnb z17.s, z11.d, #13
    // vl128 state = 0xe4bf82a4
    __ dci(0x45722070);  // sqshrnb z16.s, z3.d, #14
    // vl128 state = 0xdfc01530
    __ dci(0x456a2078);  // sqshrnb z24.s, z3.d, #22
    // vl128 state = 0x6b48fc15
    __ dci(0x452a287c);  // sqrshrnb z28.b, z3.h, #6
    // vl128 state = 0x45e86048
    __ dci(0x45282c78);  // sqrshrnt z24.b, z3.h, #8
    // vl128 state = 0xb8dc83dd
    __ dci(0x45602c68);  // sqrshrnt z8.s, z3.d, #32
    // vl128 state = 0xda536cf8
    __ dci(0x45602678);  // sqshrnt z24.s, z19.d, #32
    // vl128 state = 0xb548f79b
    __ dci(0x45682e70);  // sqrshrnt z16.s, z19.d, #24
    // vl128 state = 0xd564dd2d
    __ dci(0x45682260);  // sqshrnb z0.s, z19.d, #24
    // vl128 state = 0x7b901f9b
    __ dci(0x45682642);  // sqshrnt z2.s, z18.d, #24
    // vl128 state = 0x1d4fe6f4
    __ dci(0x45680606);  // sqshrunt z6.s, z16.d, #24
    // vl128 state = 0xe82d65a2
    __ dci(0x45680282);  // sqshrunb z2.s, z20.d, #24
    // vl128 state = 0x8a1ae6f6
    __ dci(0x45680283);  // sqshrunb z3.s, z20.d, #24
    // vl128 state = 0x5e345dcf
    __ dci(0x4568238b);  // sqshrnb z11.s, z28.d, #24
    // vl128 state = 0x31f54470
    __ dci(0x45682383);  // sqshrnb z3.s, z28.d, #24
    // vl128 state = 0x6b48975d
    __ dci(0x45682682);  // sqshrnt z2.s, z20.d, #24
    // vl128 state = 0xa9fba153
    __ dci(0x45782e8a);  // sqrshrnt z10.s, z20.d, #8
    // vl128 state = 0x0fe3100f
    __ dci(0x45780eba);  // sqrshrunt z26.s, z21.d, #8
    // vl128 state = 0x1a392151
    __ dci(0x45700e32);  // sqrshrunt z18.s, z17.d, #16
    // vl128 state = 0x08cea935
    __ dci(0x45700e42);  // sqrshrunt z2.s, z18.d, #16
    // vl128 state = 0x353f24b1
    __ dci(0x45782e52);  // sqrshrnt z18.s, z18.d, #8
    // vl128 state = 0xe06219d0
    __ dci(0x45782e42);  // sqrshrnt z2.s, z18.d, #8
    // vl128 state = 0xbb4c6d3b
    __ dci(0x45742e46);  // sqrshrnt z6.s, z18.d, #12
    // vl128 state = 0x77e7393c
    __ dci(0x45642ec7);  // sqrshrnt z7.s, z22.d, #28
    // vl128 state = 0x5201634c
    __ dci(0x45642a97);  // sqrshrnb z23.s, z20.d, #28
    // vl128 state = 0x49c32fc1
    __ dci(0x45640b87);  // sqrshrunb z7.s, z28.d, #28
    // vl128 state = 0xdd09d56d
    __ dci(0x45640f0f);  // sqrshrunt z15.s, z24.d, #28
    // vl128 state = 0x50f7d144
    __ dci(0x45600e0e);  // sqrshrunt z14.s, z16.d, #32
    // vl128 state = 0xd6bbd38a
    __ dci(0x45620a0f);  // sqrshrunb z15.s, z16.d, #30
    // vl128 state = 0x141e2991
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x141e2991,
        0x8cb951d0,
        0x74337526,
        0x515534c6,
        0xe3789189,
        0xfee7d505,
        0xfaae7ee8,
        0x71a110a3,
        0x6469dcda,
        0xe61425fc,
        0x6840f618,
        0xbc1b116d,
        0xaad97378,
        0x5d91b661,
        0x9eb84163,
        0xf8ca1e37,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_aba_long) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x45c2ca3e);  // uabalb z30.d, z17.s, z2.s
    // vl128 state = 0xac47a81c
    __ dci(0x45caca7f);  // uabalb z31.d, z19.s, z10.s
    // vl128 state = 0x10cd4e69
    __ dci(0x455aca7e);  // uabalb z30.h, z19.b, z26.b
    // vl128 state = 0x8fba3755
    __ dci(0x45daca5f);  // uabalb z31.d, z18.s, z26.s
    // vl128 state = 0x8c18257c
    __ dci(0x45d8ca1d);  // uabalb z29.d, z16.s, z24.s
    // vl128 state = 0xe6eef5ec
    __ dci(0x45d8ce95);  // uabalt z21.d, z20.s, z24.s
    // vl128 state = 0x2368baee
    __ dci(0x4598ce14);  // uabalt z20.s, z16.h, z24.h
    // vl128 state = 0xc9281174
    __ dci(0x4598ce04);  // uabalt z4.s, z16.h, z24.h
    // vl128 state = 0xa0b5fc24
    __ dci(0x45d8ce40);  // uabalt z0.d, z18.s, z24.s
    // vl128 state = 0xb3ef6f1d
    __ dci(0x45daca44);  // uabalb z4.d, z18.s, z26.s
    // vl128 state = 0xcfa3666b
    __ dci(0x45dace00);  // uabalt z0.d, z16.s, z26.s
    // vl128 state = 0x27bb4ba9
    __ dci(0x459ece04);  // uabalt z4.s, z16.h, z30.h
    // vl128 state = 0xb6628d3e
    __ dci(0x458ece80);  // uabalt z0.s, z20.h, z14.h
    // vl128 state = 0xe8db526e
    __ dci(0x458ec482);  // sabalt z2.s, z4.h, z14.h
    // vl128 state = 0x73cd8386
    __ dci(0x45cec4a3);  // sabalt z3.d, z5.s, z14.s
    // vl128 state = 0xba1c4507
    __ dci(0x45cec8a1);  // uabalb z1.d, z5.s, z14.s
    // vl128 state = 0x851cd798
    __ dci(0x458ec0a9);  // sabalb z9.s, z5.h, z14.h
    // vl128 state = 0xc85973b8
    __ dci(0x45c6c0ab);  // sabalb z11.d, z5.s, z6.s
    // vl128 state = 0x84072419
    __ dci(0x4544c0a9);  // sabalb z9.h, z5.b, z4.b
    // vl128 state = 0x533a377a
    __ dci(0x4550c0a1);  // sabalb z1.h, z5.b, z16.b
    // vl128 state = 0x5a216f3a
    __ dci(0x4550c0b1);  // sabalb z17.h, z5.b, z16.b
    // vl128 state = 0x9957b992
    __ dci(0x4552c095);  // sabalb z21.h, z4.b, z18.b
    // vl128 state = 0x666bd8db
    __ dci(0x4543c094);  // sabalb z20.h, z4.b, z3.b
    // vl128 state = 0xd66d3d52
    __ dci(0x4543c095);  // sabalb z21.h, z4.b, z3.b
    // vl128 state = 0x5d47b643
    __ dci(0x4543c385);  // sabalb z5.h, z28.b, z3.b
    // vl128 state = 0x55fc0a65
    __ dci(0x4543c38d);  // sabalb z13.h, z28.b, z3.b
    // vl128 state = 0xbb5ccc0f
    __ dci(0x45c3c19d);  // sabalb z29.d, z12.s, z3.s
    // vl128 state = 0xb3dedffd
    __ dci(0x45d3c595);  // sabalt z21.d, z12.s, z19.s
    // vl128 state = 0xd80597a1
    __ dci(0x45d2c185);  // sabalb z5.d, z12.s, z18.s
    // vl128 state = 0x29a9fafc
    __ dci(0x45d2c0b5);  // sabalb z21.d, z5.s, z18.s
    // vl128 state = 0x85dc16cb
    __ dci(0x45d2c0bd);  // sabalb z29.d, z5.s, z18.s
    // vl128 state = 0xc38b621d
    __ dci(0x45d2cab9);  // uabalb z25.d, z21.s, z18.s
    // vl128 state = 0x3801ad51
    __ dci(0x45d0ca9b);  // uabalb z27.d, z20.s, z16.s
    // vl128 state = 0xd5cc0a31
    __ dci(0x45d0ca39);  // uabalb z25.d, z17.s, z16.s
    // vl128 state = 0x272488a9
    __ dci(0x45d0ca3d);  // uabalb z29.d, z17.s, z16.s
    // vl128 state = 0xea109c4b
    __ dci(0x4550ce3c);  // uabalt z28.h, z17.b, z16.b
    // vl128 state = 0x5a9bdb39
    __ dci(0x4559ce38);  // uabalt z24.h, z17.b, z25.b
    // vl128 state = 0xd90984c9
    __ dci(0x455bcf39);  // uabalt z25.h, z25.b, z27.b
    // vl128 state = 0x6c0884ed
    __ dci(0x455bceb1);  // uabalt z17.h, z21.b, z27.b
    // vl128 state = 0x2f01a6ad
    __ dci(0x455bceb3);  // uabalt z19.h, z21.b, z27.b
    // vl128 state = 0x72a428e1
    __ dci(0x455bceb1);  // uabalt z17.h, z21.b, z27.b
    // vl128 state = 0x27adcf54
    __ dci(0x4559ce21);  // uabalt z1.h, z17.b, z25.b
    // vl128 state = 0xf1899dea
    __ dci(0x45d9ce05);  // uabalt z5.d, z16.s, z25.s
    // vl128 state = 0x41e92a5c
    __ dci(0x45dbc604);  // sabalt z4.d, z16.s, z27.s
    // vl128 state = 0x96021962
    __ dci(0x45d3c634);  // sabalt z20.d, z17.s, z19.s
    // vl128 state = 0x4795c9e2
    __ dci(0x45dbc235);  // sabalb z21.d, z17.s, z27.s
    // vl128 state = 0x6e2eccdb
    __ dci(0x45dbc07d);  // sabalb z29.d, z3.s, z27.s
    // vl128 state = 0x2c2e3625
    __ dci(0x459bc87c);  // uabalb z28.s, z3.h, z27.h
    // vl128 state = 0x618669ad
    __ dci(0x459bc878);  // uabalb z24.s, z3.h, z27.h
    // vl128 state = 0x2d1a9a08
    __ dci(0x4593cc79);  // uabalt z25.s, z3.h, z19.h
    // vl128 state = 0xdb6575df
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xdb6575df,
        0x691c09fc,
        0x6d969d30,
        0x83db67a7,
        0x8ca1109d,
        0x5175b8ff,
        0xade3cb1b,
        0x1c7b0422,
        0x1199a415,
        0xd1c715e8,
        0x2053b361,
        0x577c4450,
        0x1557204a,
        0xe994b21a,
        0xec34be56,
        0x1c9e0136,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_add_sub_carry) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x4548d4a1);  // adclt z1.d, z5.d, z8.d
    // vl128 state = 0xde78ceb3
    __ dci(0x4588d4a5);  // sbclt z5.s, z5.s, z8.s
    // vl128 state = 0x35dc8534
    __ dci(0x4589d421);  // sbclt z1.s, z1.s, z9.s
    // vl128 state = 0xa72d158b
    __ dci(0x45d9d423);  // sbclt z3.d, z1.d, z25.d
    // vl128 state = 0x197181b9
    __ dci(0x45dfd433);  // sbclt z19.d, z1.d, z31.d
    // vl128 state = 0xaad0d32d
    __ dci(0x4597d437);  // sbclt z23.s, z1.s, z23.s
    // vl128 state = 0xb1c42b7d
    __ dci(0x4597d436);  // sbclt z22.s, z1.s, z23.s
    // vl128 state = 0x6c51a28c
    __ dci(0x4587d537);  // sbclt z23.s, z9.s, z7.s
    // vl128 state = 0x525b5cf8
    __ dci(0x4586d727);  // sbclt z7.s, z25.s, z6.s
    // vl128 state = 0x33942ff9
    __ dci(0x45c6d625);  // sbclt z5.d, z17.d, z6.d
    // vl128 state = 0x24de09b4
    __ dci(0x45c2d6b5);  // sbclt z21.d, z21.d, z2.d
    // vl128 state = 0xabc0063f
    __ dci(0x4546d6b7);  // adclt z23.d, z21.d, z6.d
    // vl128 state = 0x52765e95
    __ dci(0x45c7d6a7);  // sbclt z7.d, z21.d, z7.d
    // vl128 state = 0x7045d250
    __ dci(0x4547d4a5);  // adclt z5.d, z5.d, z7.d
    // vl128 state = 0xb20f5c2a
    __ dci(0x4517d4a1);  // adclt z1.s, z5.s, z23.s
    // vl128 state = 0x5c2c9c29
    __ dci(0x4507d5a5);  // adclt z5.s, z13.s, z7.s
    // vl128 state = 0x788b25f0
    __ dci(0x4507d5ad);  // adclt z13.s, z13.s, z7.s
    // vl128 state = 0xf27eff1e
    __ dci(0x4507d0ac);  // adclb z12.s, z5.s, z7.s
    // vl128 state = 0xc0b629de
    __ dci(0x450ed0ad);  // adclb z13.s, z5.s, z14.s
    // vl128 state = 0x3e15df94
    __ dci(0x458ad0a9);  // sbclb z9.s, z5.s, z10.s
    // vl128 state = 0x68f64c82
    __ dci(0x4582d2ad);  // sbclb z13.s, z21.s, z2.s
    // vl128 state = 0x882379e1
    __ dci(0x4502d3af);  // adclb z15.s, z29.s, z2.s
    // vl128 state = 0x6901994e
    __ dci(0x450ad32b);  // adclb z11.s, z25.s, z10.s
    // vl128 state = 0xa67e9382
    __ dci(0x4582d329);  // sbclb z9.s, z25.s, z2.s
    // vl128 state = 0x9451d0c4
    __ dci(0x4592d22b);  // sbclb z11.s, z17.s, z18.s
    // vl128 state = 0xc19da52e
    __ dci(0x459ad2a3);  // sbclb z3.s, z21.s, z26.s
    // vl128 state = 0x91065b69
    __ dci(0x451ad233);  // adclb z19.s, z17.s, z26.s
    // vl128 state = 0xe3fdc4a5
    __ dci(0x450bd232);  // adclb z18.s, z17.s, z11.s
    // vl128 state = 0x168abbff
    __ dci(0x450ad2b6);  // adclb z22.s, z21.s, z10.s
    // vl128 state = 0x64d0c940
    __ dci(0x4582d2b4);  // sbclb z20.s, z21.s, z2.s
    // vl128 state = 0x37307824
    __ dci(0x4582d6e4);  // sbclt z4.s, z23.s, z2.s
    // vl128 state = 0xd35e02f7
    __ dci(0x4500d6f4);  // adclt z20.s, z23.s, z0.s
    // vl128 state = 0x017ed1b0
    __ dci(0x4501d2e4);  // adclb z4.s, z23.s, z1.s
    // vl128 state = 0x327242bc
    __ dci(0x4501d1f4);  // adclb z20.s, z15.s, z1.s
    // vl128 state = 0x208174e8
    __ dci(0x4503d1b0);  // adclb z16.s, z13.s, z3.s
    // vl128 state = 0xa5a9f61d
    __ dci(0x4501d198);  // adclb z24.s, z12.s, z1.s
    // vl128 state = 0x97e22c2b
    __ dci(0x4501d3da);  // adclb z26.s, z30.s, z1.s
    // vl128 state = 0xd3ac35d5
    __ dci(0x4501d6de);  // adclt z30.s, z22.s, z1.s
    // vl128 state = 0xab835df9
    __ dci(0x4503d2dc);  // adclb z28.s, z22.s, z3.s
    // vl128 state = 0xa048599b
    __ dci(0x4502d6d8);  // adclt z24.s, z22.s, z2.s
    // vl128 state = 0x4c245fee
    __ dci(0x4502d6d0);  // adclt z16.s, z22.s, z2.s
    // vl128 state = 0x0222f3cc
    __ dci(0x4502d280);  // adclb z0.s, z20.s, z2.s
    // vl128 state = 0x16bd7f6a
    __ dci(0x458ad284);  // sbclb z4.s, z20.s, z10.s
    // vl128 state = 0x7ef7d0a2
    __ dci(0x458ad6d4);  // sbclt z20.s, z22.s, z10.s
    // vl128 state = 0x303d8262
    __ dci(0x458ad6dc);  // sbclt z28.s, z22.s, z10.s
    // vl128 state = 0x86b8b0e9
    __ dci(0x458bd7cc);  // sbclt z12.s, z30.s, z11.s
    // vl128 state = 0x068cc5cd
    __ dci(0x45dbd7ce);  // sbclt z14.d, z30.d, z27.d
    // vl128 state = 0x30acfa7f
    __ dci(0x45dfd75e);  // sbclt z30.d, z26.d, z31.d
    // vl128 state = 0xdbd8b32a
    __ dci(0x45ddd7ce);  // sbclt z14.d, z30.d, z29.d
    // vl128 state = 0x59c3c1a9
    __ dci(0x45ddd7cf);  // sbclt z15.d, z30.d, z29.d
    // vl128 state = 0x5c953a50
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x5c953a50,
        0x22fea196,
        0x084c11a8,
        0x6e7e24d1,
        0x70965ff7,
        0x8c7cb797,
        0xdb846b66,
        0x512f049d,
        0x5c45d25c,
        0xa349606f,
        0x68a853e5,
        0xd92fbeff,
        0x52e59a6b,
        0xf77ee8ce,
        0x6c79623b,
        0x7efed6cc,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_add_sub_high) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x45fd7464);  // subhnt z4.s, z3.d, z29.d
    // vl128 state = 0x0eea0f4a
    __ dci(0x45fc7c66);  // rsubhnt z6.s, z3.d, z28.d
    // vl128 state = 0x4dc0d938
    __ dci(0x45fc7c6e);  // rsubhnt z14.s, z3.d, z28.d
    // vl128 state = 0x33de615e
    __ dci(0x45f46c7e);  // raddhnt z30.s, z3.d, z20.d
    // vl128 state = 0xa24af7ae
    __ dci(0x45f06e7c);  // raddhnt z28.s, z19.d, z16.d
    // vl128 state = 0x13883aa2
    __ dci(0x45b06a6c);  // raddhnb z12.h, z19.s, z16.s
    // vl128 state = 0x5bf75f05
    __ dci(0x45b96a64);  // raddhnb z4.h, z19.s, z25.s
    // vl128 state = 0x0e489878
    __ dci(0x45b96820);  // raddhnb z0.h, z1.s, z25.s
    // vl128 state = 0x86df8f5f
    __ dci(0x45b96a01);  // raddhnb z1.h, z16.s, z25.s
    // vl128 state = 0x0d1563f2
    __ dci(0x45b96900);  // raddhnb z0.h, z8.s, z25.s
    // vl128 state = 0xd66de87e
    __ dci(0x45a97904);  // rsubhnb z4.h, z8.s, z9.s
    // vl128 state = 0x0c34bd33
    __ dci(0x45a9790c);  // rsubhnb z12.h, z8.s, z9.s
    // vl128 state = 0x7892f2c5
    __ dci(0x45e97988);  // rsubhnb z8.s, z12.d, z9.d
    // vl128 state = 0x9709efbd
    __ dci(0x45f97909);  // rsubhnb z9.s, z8.d, z25.d
    // vl128 state = 0x029a3116
    __ dci(0x45ff790d);  // rsubhnb z13.s, z8.d, z31.d
    // vl128 state = 0x48cf21c1
    __ dci(0x45ff6d05);  // raddhnt z5.s, z8.d, z31.d
    // vl128 state = 0x44c94a11
    __ dci(0x45ff6dc1);  // raddhnt z1.s, z14.d, z31.d
    // vl128 state = 0x12fab619
    __ dci(0x45ff79d1);  // rsubhnb z17.s, z14.d, z31.d
    // vl128 state = 0x6f749933
    __ dci(0x457f7dd0);  // rsubhnt z16.b, z14.h, z31.h
    // vl128 state = 0x404889de
    __ dci(0x457f75f1);  // subhnt z17.b, z15.h, z31.h
    // vl128 state = 0x1dae2a16
    __ dci(0x457f75f3);  // subhnt z19.b, z15.h, z31.h
    // vl128 state = 0xc441a9f0
    __ dci(0x456d75fb);  // subhnt z27.b, z15.h, z13.h
    // vl128 state = 0xdd79f567
    __ dci(0x45ed7dff);  // rsubhnt z31.s, z15.d, z13.d
    // vl128 state = 0x49b27a1f
    __ dci(0x45e17dfe);  // rsubhnt z30.s, z15.d, z1.d
    // vl128 state = 0x19cddb35
    __ dci(0x45e17df6);  // rsubhnt z22.s, z15.d, z1.d
    // vl128 state = 0xea722faa
    __ dci(0x45e37d72);  // rsubhnt z18.s, z11.d, z3.d
    // vl128 state = 0x907267b3
    __ dci(0x45737d62);  // rsubhnt z2.b, z11.h, z19.h
    // vl128 state = 0x1e5409d8
    __ dci(0x45726d6a);  // raddhnt z10.b, z11.h, z18.h
    // vl128 state = 0xce3b87ca
    __ dci(0x45726f5a);  // raddhnt z26.b, z26.h, z18.h
    // vl128 state = 0x2f330789
    __ dci(0x45706f18);  // raddhnt z24.b, z24.h, z16.h
    // vl128 state = 0xff09606a
    __ dci(0x45706f08);  // raddhnt z8.b, z24.h, z16.h
    // vl128 state = 0x062ac37b
    __ dci(0x45706f09);  // raddhnt z9.b, z24.h, z16.h
    // vl128 state = 0xb12c9142
    __ dci(0x45786b08);  // raddhnb z8.b, z24.h, z24.h
    // vl128 state = 0x77e41545
    __ dci(0x45786b0c);  // raddhnb z12.b, z24.h, z24.h
    // vl128 state = 0x1f3a202d
    __ dci(0x457a6308);  // addhnb z8.b, z24.h, z26.h
    // vl128 state = 0xea51f4b9
    __ dci(0x45fb6318);  // addhnb z24.s, z24.d, z27.d
    // vl128 state = 0x5b98747e
    __ dci(0x45b96319);  // addhnb z25.h, z24.s, z25.s
    // vl128 state = 0xdcebf700
    __ dci(0x45bb621d);  // addhnb z29.h, z16.s, z27.s
    // vl128 state = 0x55a216b1
    __ dci(0x45b3625f);  // addhnb z31.h, z18.s, z19.s
    // vl128 state = 0x3e86d641
    __ dci(0x45b3631b);  // addhnb z27.h, z24.s, z19.s
    // vl128 state = 0x36d052e3
    __ dci(0x45bb6213);  // addhnb z19.h, z16.s, z27.s
    // vl128 state = 0xba012cb8
    __ dci(0x45bf7217);  // subhnb z23.h, z16.s, z31.s
    // vl128 state = 0xdef826a7
    __ dci(0x45b67213);  // subhnb z19.h, z16.s, z22.s
    // vl128 state = 0x5cd11781
    __ dci(0x45b66223);  // addhnb z3.h, z17.s, z22.s
    // vl128 state = 0x2f04c440
    __ dci(0x45f66a27);  // raddhnb z7.s, z17.d, z22.d
    // vl128 state = 0x486d0d03
    __ dci(0x45f76825);  // raddhnb z5.s, z1.d, z23.d
    // vl128 state = 0x8a94d5c9
    __ dci(0x45f668a1);  // raddhnb z1.s, z5.d, z22.d
    // vl128 state = 0x14e8e0e7
    __ dci(0x45f469b1);  // raddhnb z17.s, z13.d, z20.d
    // vl128 state = 0x19b96fb3
    __ dci(0x45f469b3);  // raddhnb z19.s, z13.d, z20.d
    // vl128 state = 0xc98e7d4e
    __ dci(0x45f169b7);  // raddhnb z23.s, z13.d, z17.d
    // vl128 state = 0x7ff24d47
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x7ff24d47,
        0xc639a9b3,
        0x0a1df4a5,
        0x30db6e18,
        0xf3e2f795,
        0x36ff477d,
        0x162f1ca5,
        0x36da990b,
        0x110b2c35,
        0xaf1580f5,
        0x14e39873,
        0x7f5eb52c,
        0x2ececb6f,
        0x4e4d71f0,
        0x800769d1,
        0x1bcbe3a3,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_complex_addition) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x4500dc43);  // cadd z3.b, z3.b, z2.b, #270
    // vl128 state = 0x998365c2
    __ dci(0x4540dc13);  // cadd z19.h, z19.h, z0.h, #270
    // vl128 state = 0xcc866131
    __ dci(0x4541d81b);  // sqcadd z27.h, z27.h, z0.h, #90
    // vl128 state = 0x2ae23a6a
    __ dci(0x45c1d853);  // sqcadd z19.d, z19.d, z2.d, #90
    // vl128 state = 0x1f8de2d3
    __ dci(0x4541d8c3);  // sqcadd z3.h, z3.h, z6.h, #90
    // vl128 state = 0x3655c07c
    __ dci(0x4541d8d3);  // sqcadd z19.h, z19.h, z6.h, #90
    // vl128 state = 0x3a8fe2d9
    __ dci(0x4541d811);  // sqcadd z17.h, z17.h, z0.h, #90
    // vl128 state = 0x003c88ea
    __ dci(0x4540da10);  // cadd z16.h, z16.h, z16.h, #90
    // vl128 state = 0xe20c1375
    __ dci(0x4540da18);  // cadd z24.h, z24.h, z16.h, #90
    // vl128 state = 0x67bb0270
    __ dci(0x4540de5a);  // cadd z26.h, z26.h, z18.h, #270
    // vl128 state = 0x7abb4f8f
    __ dci(0x4540de4a);  // cadd z10.h, z10.h, z18.h, #270
    // vl128 state = 0x42850f11
    __ dci(0x4500decb);  // cadd z11.b, z11.b, z22.b, #270
    // vl128 state = 0xda605f59
    __ dci(0x4500da83);  // cadd z3.b, z3.b, z20.b, #90
    // vl128 state = 0x99e63476
    __ dci(0x4500dc8b);  // cadd z11.b, z11.b, z4.b, #270
    // vl128 state = 0xd444a939
    __ dci(0x4500dc8f);  // cadd z15.b, z15.b, z4.b, #270
    // vl128 state = 0xde3ad968
    __ dci(0x4500d99f);  // cadd z31.b, z31.b, z12.b, #90
    // vl128 state = 0xd7cdb177
    __ dci(0x4540d91e);  // cadd z30.h, z30.h, z8.h, #90
    // vl128 state = 0x74575b36
    __ dci(0x4541d81a);  // sqcadd z26.h, z26.h, z0.h, #90
    // vl128 state = 0x3d347b0b
    __ dci(0x4501d83b);  // sqcadd z27.b, z27.b, z1.b, #90
    // vl128 state = 0x03df7859
    __ dci(0x45c1d83f);  // sqcadd z31.d, z31.d, z1.d, #90
    // vl128 state = 0xf0cdbf68
    __ dci(0x45c1d83e);  // sqcadd z30.d, z30.d, z1.d, #90
    // vl128 state = 0x0931dda4
    __ dci(0x45c1d83c);  // sqcadd z28.d, z28.d, z1.d, #90
    // vl128 state = 0x460b5369
    __ dci(0x4581da3e);  // sqcadd z30.s, z30.s, z17.s, #90
    // vl128 state = 0x71af9203
    __ dci(0x45c1d83f);  // sqcadd z31.d, z31.d, z1.d, #90
    // vl128 state = 0xd6babc53
    __ dci(0x4581da3e);  // sqcadd z30.s, z30.s, z17.s, #90
    // vl128 state = 0xd3e4f42f
    __ dci(0x4501d83f);  // sqcadd z31.b, z31.b, z1.b, #90
    // vl128 state = 0x7a594239
    __ dci(0x4501dcbb);  // sqcadd z27.b, z27.b, z5.b, #270
    // vl128 state = 0x24a5a8c9
    __ dci(0x4501dfba);  // sqcadd z26.b, z26.b, z29.b, #270
    // vl128 state = 0x0c3df842
    __ dci(0x4581dfea);  // sqcadd z10.s, z10.s, z31.s, #270
    // vl128 state = 0x6173c97f
    __ dci(0x4581db7a);  // sqcadd z26.s, z26.s, z27.s, #90
    // vl128 state = 0x55090d5f
    __ dci(0x4581db1b);  // sqcadd z27.s, z27.s, z24.s, #90
    // vl128 state = 0x63477385
    __ dci(0x4581da93);  // sqcadd z19.s, z19.s, z20.s, #90
    // vl128 state = 0xc996545e
    __ dci(0x45c1db92);  // sqcadd z18.d, z18.d, z28.d, #90
    // vl128 state = 0xa48bf827
    __ dci(0x45c1db93);  // sqcadd z19.d, z19.d, z28.d, #90
    // vl128 state = 0xf5a3b641
    __ dci(0x45c1daa3);  // sqcadd z3.d, z3.d, z21.d, #90
    // vl128 state = 0x20ad4c28
    __ dci(0x4581dba7);  // sqcadd z7.s, z7.s, z29.s, #90
    // vl128 state = 0xc9e36e96
    __ dci(0x45c1daaf);  // sqcadd z15.d, z15.d, z21.d, #90
    // vl128 state = 0x6eb23fd2
    __ dci(0x45c1daae);  // sqcadd z14.d, z14.d, z21.d, #90
    // vl128 state = 0x585d4d63
    __ dci(0x4541dae6);  // sqcadd z6.h, z6.h, z23.h, #90
    // vl128 state = 0x827cc0a8
    __ dci(0x4541daee);  // sqcadd z14.h, z14.h, z23.h, #90
    // vl128 state = 0xe00543a0
    __ dci(0x4501dabe);  // sqcadd z30.b, z30.b, z21.b, #90
    // vl128 state = 0x2313db47
    __ dci(0x4501deff);  // sqcadd z31.b, z31.b, z23.b, #270
    // vl128 state = 0xe30d4e83
    __ dci(0x4501defd);  // sqcadd z29.b, z29.b, z23.b, #270
    // vl128 state = 0xb95d6d94
    __ dci(0x4501def5);  // sqcadd z21.b, z21.b, z23.b, #270
    // vl128 state = 0x4f18b02e
    __ dci(0x4501def4);  // sqcadd z20.b, z20.b, z23.b, #270
    // vl128 state = 0x20ae9a78
    __ dci(0x4501dee4);  // sqcadd z4.b, z4.b, z23.b, #270
    // vl128 state = 0x4eef87a9
    __ dci(0x4501dee6);  // sqcadd z6.b, z6.b, z23.b, #270
    // vl128 state = 0x1b041a7b
    __ dci(0x4501dfc2);  // sqcadd z2.b, z2.b, z30.b, #270
    // vl128 state = 0xeaf5e18f
    __ dci(0x4500df92);  // cadd z18.b, z18.b, z28.b, #270
    // vl128 state = 0xc47ee5e7
    __ dci(0x4500de13);  // cadd z19.b, z19.b, z16.b, #270
    // vl128 state = 0x6482d75c
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x6482d75c,
        0x48d9bd2f,
        0xd6bd52ae,
        0x56be94f0,
        0x620cfb69,
        0xb646e0fe,
        0x6034718f,
        0xd8187657,
        0x211218bb,
        0xc973a707,
        0x6020dcc9,
        0x8fadad0c,
        0x0132ecbc,
        0x3a07eb63,
        0x5c20eb82,
        0xc92d6cb2,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_bit_permute) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kSVEBitPerm,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x455fbb1a);  // bgrp z26.h, z24.h, z31.h
    // vl128 state = 0x39fb8e5b
    __ dci(0x451fbb58);  // bgrp z24.b, z26.b, z31.b
    // vl128 state = 0x7fbccdbd
    __ dci(0x4517bb19);  // bgrp z25.b, z24.b, z23.b
    // vl128 state = 0x67caf176
    __ dci(0x4517bb18);  // bgrp z24.b, z24.b, z23.b
    // vl128 state = 0x665fd977
    __ dci(0x4517ba5c);  // bgrp z28.b, z18.b, z23.b
    // vl128 state = 0x0f2c1473
    __ dci(0x4517ba38);  // bgrp z24.b, z17.b, z23.b
    // vl128 state = 0x253789a0
    __ dci(0x4517ba3c);  // bgrp z28.b, z17.b, z23.b
    // vl128 state = 0xd3b26fd2
    __ dci(0x4515ba6c);  // bgrp z12.b, z19.b, z21.b
    // vl128 state = 0x4bad6941
    __ dci(0x4515bac4);  // bgrp z4.b, z22.b, z21.b
    // vl128 state = 0x7c70d2d2
    __ dci(0x4517ba86);  // bgrp z6.b, z20.b, z23.b
    // vl128 state = 0x5794816b
    __ dci(0x4517ba87);  // bgrp z7.b, z20.b, z23.b
    // vl128 state = 0xe67993b1
    __ dci(0x4515b297);  // bext z23.b, z20.b, z21.b
    // vl128 state = 0x3041b7ee
    __ dci(0x4517b396);  // bext z22.b, z28.b, z23.b
    // vl128 state = 0xb571d524
    __ dci(0x451bb386);  // bext z6.b, z28.b, z27.b
    // vl128 state = 0x73ce1823
    __ dci(0x4513b784);  // bdep z4.b, z28.b, z19.b
    // vl128 state = 0x4264f0f2
    __ dci(0x4593b7ac);  // bdep z12.s, z29.s, z19.s
    // vl128 state = 0xf9cb9d26
    __ dci(0x4593b7a8);  // bdep z8.s, z29.s, z19.s
    // vl128 state = 0xa2b310a0
    __ dci(0x4597b780);  // bdep z0.s, z28.s, z23.s
    // vl128 state = 0xee25c82f
    __ dci(0x4597b781);  // bdep z1.s, z28.s, z23.s
    // vl128 state = 0xdca7577f
    __ dci(0x4597b7e3);  // bdep z3.s, z31.s, z23.s
    // vl128 state = 0x32294429
    __ dci(0x45dfb7e1);  // bdep z1.d, z31.d, z31.d
    // vl128 state = 0xc147e511
    __ dci(0x455db7e5);  // bdep z5.h, z31.h, z29.h
    // vl128 state = 0x7a51d422
    __ dci(0x45d5b7e4);  // bdep z4.d, z31.d, z21.d
    // vl128 state = 0x512ad92a
    __ dci(0x45c7b7ec);  // bdep z12.d, z31.d, z7.d
    // vl128 state = 0xe59fbf5c
    __ dci(0x4547b7a8);  // bdep z8.h, z29.h, z7.h
    // vl128 state = 0xb85fd3b1
    __ dci(0x454fb72c);  // bdep z12.h, z25.h, z15.h
    // vl128 state = 0xc820e9d0
    __ dci(0x4557b724);  // bdep z4.h, z25.h, z23.h
    // vl128 state = 0x814ff3f4
    __ dci(0x4557bb20);  // bgrp z0.h, z25.h, z23.h
    // vl128 state = 0xc58dee50
    __ dci(0x4556b321);  // bext z1.h, z25.h, z22.h
    // vl128 state = 0xf19c0956
    __ dci(0x4556b3e3);  // bext z3.h, z31.h, z22.h
    // vl128 state = 0x2a256808
    __ dci(0x4546b367);  // bext z7.h, z27.h, z6.h
    // vl128 state = 0x1c6696f4
    __ dci(0x4556bb66);  // bgrp z6.h, z27.h, z22.h
    // vl128 state = 0x32522ca2
    __ dci(0x4556bb76);  // bgrp z22.h, z27.h, z22.h
    // vl128 state = 0x33fe6590
    __ dci(0x45c6bb66);  // bgrp z6.d, z27.d, z6.d
    // vl128 state = 0x45d26723
    __ dci(0x45c2b976);  // bgrp z22.d, z11.d, z2.d
    // vl128 state = 0x364d9885
    __ dci(0x4540b974);  // bgrp z20.h, z11.h, z0.h
    // vl128 state = 0x36a0bd94
    __ dci(0x45c0b164);  // bext z4.d, z11.d, z0.d
    // vl128 state = 0x4ee9a90c
    __ dci(0x45ccb16c);  // bext z12.d, z11.d, z12.d
    // vl128 state = 0x30c32d69
    __ dci(0x458cb368);  // bext z8.s, z27.s, z12.s
    // vl128 state = 0xfc2c912f
    __ dci(0x450cb769);  // bdep z9.b, z27.b, z12.b
    // vl128 state = 0xef976b44
    __ dci(0x458cb7eb);  // bdep z11.s, z31.s, z12.s
    // vl128 state = 0x6f9e21b8
    __ dci(0x4588b5ef);  // bdep z15.s, z15.s, z8.s
    // vl128 state = 0xa1f212e2
    __ dci(0x4598b5ad);  // bdep z13.s, z13.s, z24.s
    // vl128 state = 0xe4286a40
    __ dci(0x4598b5af);  // bdep z15.s, z13.s, z24.s
    // vl128 state = 0x7d6622e5
    __ dci(0x4598b6ad);  // bdep z13.s, z21.s, z24.s
    // vl128 state = 0xcd00829c
    __ dci(0x4518b2af);  // bext z15.b, z21.b, z24.b
    // vl128 state = 0xa8d58b2d
    __ dci(0x4519b2e7);  // bext z7.b, z23.b, z25.b
    // vl128 state = 0x2b7b7c44
    __ dci(0x4518b2a6);  // bext z6.b, z21.b, z24.b
    // vl128 state = 0x09c81b7e
    __ dci(0x4518b2a7);  // bext z7.b, z21.b, z24.b
    // vl128 state = 0xab1b2b22
    __ dci(0x4519b6a5);  // bdep z5.b, z21.b, z25.b
    // vl128 state = 0x03476e4c
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x03476e4c,
        0xcc54e76f,
        0x08324d66,
        0xcc289ee1,
        0xacd3ba43,
        0xe961aeda,
        0x60a204b1,
        0xde020904,
        0x0652d1e5,
        0x7982dc25,
        0x02a2c1cb,
        0x4dd9e71b,
        0xb57f587f,
        0xb75e0d62,
        0x78330809,
        0xbc7046ae,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_smullb_smullt_umullb_umullt_vector) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x455a7bc2);  // umullb z2.h, z30.b, z26.b
    // vl128 state = 0xe2a2b611
    __ dci(0x454a7b92);  // umullb z18.h, z28.b, z10.b
    // vl128 state = 0x12b3b0c6
    __ dci(0x45427bda);  // umullb z26.h, z30.b, z2.b
    // vl128 state = 0x74f4a891
    __ dci(0x45c67bde);  // umullb z30.d, z30.s, z6.s
    // vl128 state = 0x20402d9f
    __ dci(0x45467b56);  // umullb z22.h, z26.b, z6.b
    // vl128 state = 0x75e15413
    __ dci(0x45427f54);  // umullt z20.h, z26.b, z2.b
    // vl128 state = 0x51478ee1
    __ dci(0x45427fe4);  // umullt z4.h, z31.b, z2.b
    // vl128 state = 0x63381b63
    __ dci(0x45567fe5);  // umullt z5.h, z31.b, z22.b
    // vl128 state = 0x0967f882
    __ dci(0x45467df5);  // umullt z21.h, z15.b, z6.b
    // vl128 state = 0x753e96b9
    __ dci(0x454279f1);  // umullb z17.h, z15.b, z2.b
    // vl128 state = 0xcff906e6
    __ dci(0x454078f5);  // umullb z21.h, z7.b, z0.b
    // vl128 state = 0x5609bd14
    __ dci(0x454070d4);  // smullb z20.h, z6.b, z0.b
    // vl128 state = 0xf284d300
    __ dci(0x45407016);  // smullb z22.h, z0.b, z0.b
    // vl128 state = 0xbb549bf7
    __ dci(0x45487086);  // smullb z6.h, z4.b, z8.b
    // vl128 state = 0x6ef99ff1
    __ dci(0x454070c7);  // smullb z7.h, z6.b, z0.b
    // vl128 state = 0x90177a84
    __ dci(0x45407846);  // umullb z6.h, z2.b, z0.b
    // vl128 state = 0xd3dbb2fe
    __ dci(0x45417a56);  // umullb z22.h, z18.b, z1.b
    // vl128 state = 0x7d30cf73
    __ dci(0x45417877);  // umullb z23.h, z3.b, z1.b
    // vl128 state = 0x0623e678
    __ dci(0x45417807);  // umullb z7.h, z0.b, z1.b
    // vl128 state = 0xe849cf35
    __ dci(0x454178a3);  // umullb z3.h, z5.b, z1.b
    // vl128 state = 0xcad236a9
    __ dci(0x45437cab);  // umullt z11.h, z5.b, z3.b
    // vl128 state = 0xc8dfcb1d
    __ dci(0x454b7c3b);  // umullt z27.h, z1.b, z11.b
    // vl128 state = 0x6136e2d6
    __ dci(0x454b7a3a);  // umullb z26.h, z17.b, z11.b
    // vl128 state = 0x091beb5a
    __ dci(0x454b72b2);  // smullb z18.h, z21.b, z11.b
    // vl128 state = 0x932b30ec
    __ dci(0x454b7622);  // smullt z2.h, z17.b, z11.b
    // vl128 state = 0xee51239c
    __ dci(0x454b76ea);  // smullt z10.h, z23.b, z11.b
    // vl128 state = 0xf4fcc577
    __ dci(0x454b74ab);  // smullt z11.h, z5.b, z11.b
    // vl128 state = 0xcf0c8028
    __ dci(0x454d74bb);  // smullt z27.h, z5.b, z13.b
    // vl128 state = 0x0f8523c8
    __ dci(0x454d740b);  // smullt z11.h, z0.b, z13.b
    // vl128 state = 0xc02b2f52
    __ dci(0x454d7403);  // smullt z3.h, z0.b, z13.b
    // vl128 state = 0x11b4180c
    __ dci(0x45557413);  // smullt z19.h, z0.b, z21.b
    // vl128 state = 0x26eef57a
    __ dci(0x45557531);  // smullt z17.h, z9.b, z21.b
    // vl128 state = 0x6f3fce98
    __ dci(0x455574b9);  // smullt z25.h, z5.b, z21.b
    // vl128 state = 0x0d4ac272
    __ dci(0x455571b1);  // smullb z17.h, z13.b, z21.b
    // vl128 state = 0x7c866a41
    __ dci(0x455573e1);  // smullb z1.h, z31.b, z21.b
    // vl128 state = 0x9c724758
    __ dci(0x455473c9);  // smullb z9.h, z30.b, z20.b
    // vl128 state = 0xa9a8d0aa
    __ dci(0x455473cb);  // smullb z11.h, z30.b, z20.b
    // vl128 state = 0xd7eec117
    __ dci(0x455473a9);  // smullb z9.h, z29.b, z20.b
    // vl128 state = 0x35caaa62
    __ dci(0x455473a8);  // smullb z8.h, z29.b, z20.b
    // vl128 state = 0x97a1d399
    __ dci(0x455473b8);  // smullb z24.h, z29.b, z20.b
    // vl128 state = 0x3adce4ee
    __ dci(0x455673fa);  // smullb z26.h, z31.b, z22.b
    // vl128 state = 0xd17120ea
    __ dci(0x455e77ea);  // smullt z10.h, z31.b, z30.b
    // vl128 state = 0x1e238a9e
    __ dci(0x455677da);  // smullt z26.h, z30.b, z22.b
    // vl128 state = 0xfbccf6c2
    __ dci(0x454673d8);  // smullb z24.h, z30.b, z6.b
    // vl128 state = 0xa47583be
    __ dci(0x45c67359);  // smullb z25.d, z26.s, z6.s
    // vl128 state = 0x4e8a9b37
    __ dci(0x45c47751);  // smullt z17.d, z26.s, z4.s
    // vl128 state = 0xe3c06571
    __ dci(0x45d67741);  // smullt z1.d, z26.s, z22.s
    // vl128 state = 0x6629e034
    __ dci(0x45d67b45);  // umullb z5.d, z26.s, z22.s
    // vl128 state = 0x66a99e85
    __ dci(0x45867b47);  // umullb z7.s, z26.h, z6.h
    // vl128 state = 0xf1cc3339
    __ dci(0x45867b45);  // umullb z5.s, z26.h, z6.h
    // vl128 state = 0x8bf658d7
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x8bf658d7,
        0x82fac555,
        0x07c3d434,
        0x25d2ee2b,
        0xe70f4394,
        0x79223404,
        0x368ed35f,
        0x6565d842,
        0xead08c30,
        0xae35e083,
        0xe1959b85,
        0x94ad31e7,
        0x9caeda4d,
        0x7611d6dc,
        0x22977911,
        0xcf3754ec,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_sqdmullb_sqdmullt_pmullb_pmullb_vector) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x45936164);  // sqdmullb z4.s, z11.h, z19.h
    // vl128 state = 0xacc89592
    __ dci(0x459161f4);  // sqdmullb z20.s, z15.h, z17.h
    // vl128 state = 0x142c66e5
    __ dci(0x459563f5);  // sqdmullb z21.s, z31.h, z21.h
    // vl128 state = 0x5cfcb839
    __ dci(0x45956265);  // sqdmullb z5.s, z19.h, z21.h
    // vl128 state = 0x33616223
    __ dci(0x45d56235);  // sqdmullb z21.d, z17.s, z21.s
    // vl128 state = 0x987a4a0d
    __ dci(0x45556031);  // sqdmullb z17.h, z1.b, z21.b
    // vl128 state = 0xf7dd9b01
    __ dci(0x45506035);  // sqdmullb z21.h, z1.b, z16.b
    // vl128 state = 0x6fa54cf3
    __ dci(0x45506334);  // sqdmullb z20.h, z25.b, z16.b
    // vl128 state = 0x04398c6e
    __ dci(0x45486336);  // sqdmullb z22.h, z25.b, z8.b
    // vl128 state = 0x4cda753c
    __ dci(0x45486334);  // sqdmullb z20.h, z25.b, z8.b
    // vl128 state = 0x53993d4a
    __ dci(0x45496b35);  // pmullb z21.h, z25.b, z9.b
    // vl128 state = 0xa591f97c
    __ dci(0x45496b37);  // pmullb z23.h, z25.b, z9.b
    // vl128 state = 0x5cb91e99
    __ dci(0x45496fb3);  // pmullt z19.h, z29.b, z9.b
    // vl128 state = 0x5031ac4d
    __ dci(0x45596f3b);  // pmullt z27.h, z25.b, z25.b
    // vl128 state = 0xb0a76e75
    __ dci(0x455d6f13);  // pmullt z19.h, z24.b, z29.b
    // vl128 state = 0xe84ca196
    __ dci(0x455d6fb2);  // pmullt z18.h, z29.b, z29.b
    // vl128 state = 0xd294ce54
    __ dci(0x455c6bb0);  // pmullb z16.h, z29.b, z28.b
    // vl128 state = 0x90f01471
    __ dci(0x45546bf8);  // pmullb z24.h, z31.b, z20.b
    // vl128 state = 0xd15f23fa
    __ dci(0x45546bf9);  // pmullb z25.h, z31.b, z20.b
    // vl128 state = 0x62ca83ea
    __ dci(0x45546bfb);  // pmullb z27.h, z31.b, z20.b
    // vl128 state = 0xf786c1e4
    __ dci(0x454469eb);  // pmullb z11.h, z15.b, z4.b
    // vl128 state = 0x3cc8c789
    __ dci(0x455069fb);  // pmullb z27.h, z15.b, z16.b
    // vl128 state = 0xb14709ca
    __ dci(0x45546dfa);  // pmullt z26.h, z15.b, z20.b
    // vl128 state = 0x38257820
    __ dci(0x45546df8);  // pmullt z24.h, z15.b, z20.b
    // vl128 state = 0x9cc5cd3a
    __ dci(0x45576dfc);  // pmullt z28.h, z15.b, z23.b
    // vl128 state = 0x704543ec
    __ dci(0x45d76d6c);  // pmullt z12.d, z11.s, z23.s
    // vl128 state = 0x15ec8e77
    __ dci(0x455f6d68);  // pmullt z8.h, z11.b, z31.b
    // vl128 state = 0xfa379a67
    __ dci(0x45596d6a);  // pmullt z10.h, z11.b, z25.b
    // vl128 state = 0x27fcfa49
    __ dci(0x45596d7a);  // pmullt z26.h, z11.b, z25.b
    // vl128 state = 0x13883ef0
    __ dci(0x45596532);  // sqdmullt z18.h, z9.b, z25.b
    // vl128 state = 0x667f8699
    __ dci(0x45596536);  // sqdmullt z22.h, z9.b, z25.b
    // vl128 state = 0x477ded37
    __ dci(0x45d16537);  // sqdmullt z23.d, z9.s, z17.s
    // vl128 state = 0x3323eb48
    __ dci(0x45c16515);  // sqdmullt z21.d, z8.s, z1.s
    // vl128 state = 0x3f581e83
    __ dci(0x45456517);  // sqdmullt z23.h, z8.b, z5.b
    // vl128 state = 0xd844e48b
    __ dci(0x45556555);  // sqdmullt z21.h, z10.b, z21.b
    // vl128 state = 0x95e6094e
    __ dci(0x45c56554);  // sqdmullt z20.d, z10.s, z5.s
    // vl128 state = 0x198a6f75
    __ dci(0x45cd6456);  // sqdmullt z22.d, z2.s, z13.s
    // vl128 state = 0x4d6b7178
    __ dci(0x45c96406);  // sqdmullt z6.d, z0.s, z9.s
    // vl128 state = 0xd989cd0f
    __ dci(0x45d96482);  // sqdmullt z2.d, z4.s, z25.s
    // vl128 state = 0xa80fdf92
    __ dci(0x45dd6406);  // sqdmullt z6.d, z0.s, z29.s
    // vl128 state = 0x9876a20d
    __ dci(0x45596404);  // sqdmullt z4.h, z0.b, z25.b
    // vl128 state = 0x5ad5787c
    __ dci(0x454b6414);  // sqdmullt z20.h, z0.b, z11.b
    // vl128 state = 0x86c077d7
    __ dci(0x454a601c);  // sqdmullb z28.h, z0.b, z10.b
    // vl128 state = 0xfe867841
    __ dci(0x4542641d);  // sqdmullt z29.h, z0.b, z2.b
    // vl128 state = 0x7bf363f1
    __ dci(0x4552643c);  // sqdmullt z28.h, z1.b, z18.b
    // vl128 state = 0x7cf26ed3
    __ dci(0x4552673d);  // sqdmullt z29.h, z25.b, z18.b
    // vl128 state = 0x748f1a99
    __ dci(0x45d6673f);  // sqdmullt z31.d, z25.s, z22.s
    // vl128 state = 0xbb15fd07
    __ dci(0x45d2633d);  // sqdmullb z29.d, z25.s, z18.s
    // vl128 state = 0x28e0985a
    __ dci(0x455a6339);  // sqdmullb z25.h, z25.b, z26.b
    // vl128 state = 0x9c0da0fd
    __ dci(0x45526738);  // sqdmullt z24.h, z25.b, z18.b
    // vl128 state = 0xa970ebb8
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xa970ebb8,
        0xc665eff5,
        0x8cc21595,
        0x0ea984f6,
        0x1dbce326,
        0x0845e911,
        0xa6fb6cf4,
        0x8544239a,
        0x2412d23d,
        0xbce6f5e0,
        0x780ff264,
        0xcf6cf172,
        0xef93a3b4,
        0x94080541,
        0xa0aedeba,
        0x8e8bddaa,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_sqdmullt_sqdmullb_z_zzi) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0x44eae5a9);  // sqdmullt z9.d, z13.s, z10.s[#0]
    // vl128 state = 0x311dfe35
    __ dci(0x44eae9a1);  // sqdmullb z1.d, z13.s, z10.s[#1]
    // vl128 state = 0x559243c3
    __ dci(0x44eae9a5);  // sqdmullb z5.d, z13.s, z10.s[#1]
    // vl128 state = 0x44d6824c
    __ dci(0x44e2edad);  // sqdmullt z13.d, z13.s, z2.s[#1]
    // vl128 state = 0xb5539592
    __ dci(0x44e6e9ac);  // sqdmullb z12.d, z13.s, z6.s[#1]
    // vl128 state = 0x5e66b9f8
    __ dci(0x44e4ebae);  // sqdmullb z14.d, z29.s, z4.s[#1]
    // vl128 state = 0x4347620a
    __ dci(0x44e4ebaf);  // sqdmullb z15.d, z29.s, z4.s[#1]
    // vl128 state = 0xe7cfe898
    __ dci(0x44a5ebad);  // sqdmullb z13.s, z29.h, z5.h[#1]
    // vl128 state = 0x0ca455c7
    __ dci(0x44a5e9fd);  // sqdmullb z29.s, z15.h, z5.h[#1]
    // vl128 state = 0xcac072a9
    __ dci(0x44e5e8fc);  // sqdmullb z28.d, z7.s, z5.s[#1]
    // vl128 state = 0xe18e8c66
    __ dci(0x44ede9ec);  // sqdmullb z12.d, z15.s, z13.s[#1]
    // vl128 state = 0x32f642cb
    __ dci(0x44ede9fc);  // sqdmullb z28.d, z15.s, z13.s[#1]
    // vl128 state = 0xa0467c8a
    __ dci(0x44fce9f4);  // sqdmullb z20.d, z15.s, z12.s[#3]
    // vl128 state = 0x7ada4130
    __ dci(0x44e4e9f6);  // sqdmullb z22.d, z15.s, z4.s[#1]
    // vl128 state = 0xc87deb44
    __ dci(0x44f4e9d2);  // sqdmullb z18.d, z14.s, z4.s[#3]
    // vl128 state = 0x6dc052ca
    __ dci(0x44f5e9e2);  // sqdmullb z2.d, z15.s, z5.s[#3]
    // vl128 state = 0xe05110d4
    __ dci(0x44f5ebb2);  // sqdmullb z18.d, z29.s, z5.s[#3]
    // vl128 state = 0x7ed21594
    __ dci(0x44b5efba);  // sqdmullt z26.s, z29.h, z5.h[#5]
    // vl128 state = 0x7d5dad40
    __ dci(0x44b5ef78);  // sqdmullt z24.s, z27.h, z5.h[#5]
    // vl128 state = 0x418f84bc
    __ dci(0x44f5eb70);  // sqdmullb z16.d, z27.s, z5.s[#3]
    // vl128 state = 0x72d78d32
    __ dci(0x44e5ebf4);  // sqdmullb z20.d, z31.s, z5.s[#1]
    // vl128 state = 0x391fad35
    __ dci(0x44e5efbc);  // sqdmullt z28.d, z29.s, z5.s[#1]
    // vl128 state = 0xb2143633
    __ dci(0x44e1ebbd);  // sqdmullb z29.d, z29.s, z1.s[#1]
    // vl128 state = 0x468dac6e
    __ dci(0x44f1ebed);  // sqdmullb z13.d, z31.s, z1.s[#3]
    // vl128 state = 0x9ab292bd
    __ dci(0x44f5efe5);  // sqdmullt z5.d, z31.s, z5.s[#3]
    // vl128 state = 0x4f2bd5d1
    __ dci(0x44fdeee7);  // sqdmullt z7.d, z23.s, z13.s[#3]
    // vl128 state = 0x7a810779
    __ dci(0x44fdee25);  // sqdmullt z5.d, z17.s, z13.s[#3]
    // vl128 state = 0x05d23734
    __ dci(0x44f5ea27);  // sqdmullb z7.d, z17.s, z5.s[#3]
    // vl128 state = 0x878580f5
    __ dci(0x44f1e225);  // sqdmullb z5.d, z17.s, z1.s[#2]
    // vl128 state = 0x5fa56f94
    __ dci(0x44e1ea21);  // sqdmullb z1.d, z17.s, z1.s[#1]
    // vl128 state = 0x05f1cdf0
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x05f1cdf0,
        0x6b88d4f2,
        0x83bf279d,
        0x12f21868,
        0x6c68a5ce,
        0x5710343f,
        0xa4d0d0ee,
        0x335b20c5,
        0x0dd491c5,
        0x98966292,
        0xb68cdacd,
        0xa26f9914,
        0x6dd60ced,
        0x5cd0d62c,
        0xebe3fb25,
        0xb264d998,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_xar) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 20 * kInstructionSize);
    __ dci(0x04293719);  // xar z25.b, z25.b, z24.b, #7
    // vl128 state = 0x596046c4
    __ dci(0x04293531);  // xar z17.b, z17.b, z9.b, #7
    // vl128 state = 0x38332d55
    __ dci(0x04e93533);  // xar z19.d, z19.d, z9.d, #23
    // vl128 state = 0x535c8af7
    __ dci(0x046b3523);  // xar z3.s, z3.s, z9.s, #21
    // vl128 state = 0x879a489f
    __ dci(0x04eb3427);  // xar z7.d, z7.d, z1.d, #21
    // vl128 state = 0xfbac317f
    __ dci(0x04ea3463);  // xar z3.d, z3.d, z3.d, #22
    // vl128 state = 0xfb44482e
    __ dci(0x04fa3447);  // xar z7.d, z7.d, z2.d, #6
    // vl128 state = 0xa59e324c
    __ dci(0x04f8346f);  // xar z15.d, z15.d, z3.d, #8
    // vl128 state = 0x7f064300
    __ dci(0x0479346b);  // xar z11.s, z11.s, z3.s, #7
    // vl128 state = 0x0c0d3573
    __ dci(0x0461346a);  // xar z10.s, z10.s, z3.s, #31
    // vl128 state = 0x3c61530d
    __ dci(0x0464346b);  // xar z11.s, z11.s, z3.s, #28
    // vl128 state = 0x137c1433
    __ dci(0x04643469);  // xar z9.s, z9.s, z3.s, #28
    // vl128 state = 0x81d55bb1
    __ dci(0x0464346b);  // xar z11.s, z11.s, z3.s, #28
    // vl128 state = 0xad2ac5c0
    __ dci(0x0434346a);  // xar z10.h, z10.h, z3.h, #12
    // vl128 state = 0x2997a1d9
    __ dci(0x04b434fa);  // xar z26.d, z26.d, z7.d, #44
    // vl128 state = 0x715f758d
    __ dci(0x04e434f2);  // xar z18.d, z18.d, z7.d, #28
    // vl128 state = 0x8bfa19ef
    __ dci(0x04ec34b3);  // xar z19.d, z19.d, z5.d, #20
    // vl128 state = 0xa8d646a5
    __ dci(0x04ae34b7);  // xar z23.d, z23.d, z5.d, #50
    // vl128 state = 0xf590c489
    __ dci(0x04ae34a7);  // xar z7.d, z7.d, z5.d, #50
    // vl128 state = 0xd6aafb5e
    __ dci(0x04ae3417);  // xar z23.d, z23.d, z0.d, #50
    // vl128 state = 0xd40a8d1a
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xd40a8d1a,
        0x834982b0,
        0x6fd8c07b,
        0x2654e6f3,
        0x79fa44fb,
        0xc8a60223,
        0xd12f35f0,
        0x1e0a3315,
        0x6970dcd2,
        0x62305aed,
        0xb9846a55,
        0x1147e436,
        0x97a8ceaa,
        0xe8f80c0e,
        0xea3ab3e7,
        0xb2abd654,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_histcnt) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 100 * kInstructionSize);
    __ dci(0x45e8c2f9);  // histcnt z25.d, p0/z, z23.d, z8.d
    // vl128 state = 0x892c6962
    __ dci(0x45e8c1f1);  // histcnt z17.d, p0/z, z15.d, z8.d
    // vl128 state = 0x6ef7d729
    __ dci(0x45e8c3a1);  // histcnt z1.d, p0/z, z29.d, z8.d
    // vl128 state = 0x17654f81
    __ dci(0x45e8c3a9);  // histcnt z9.d, p0/z, z29.d, z8.d
    // vl128 state = 0xe1a0067e
    __ dci(0x45e8c0a8);  // histcnt z8.d, p0/z, z5.d, z8.d
    // vl128 state = 0xd41f511b
    __ dci(0x45e8d0f8);  // histcnt z24.d, p4/z, z7.d, z8.d
    // vl128 state = 0x8b73945a
    __ dci(0x45e8d0fa);  // histcnt z26.d, p4/z, z7.d, z8.d
    // vl128 state = 0xc175acec
    __ dci(0x45aad0fb);  // histcnt z27.s, p4/z, z7.s, z10.s
    // vl128 state = 0x44f8385b
    __ dci(0x45aad2df);  // histcnt z31.s, p4/z, z22.s, z10.s
    // vl128 state = 0x52cd5d17
    __ dci(0x45aad2dd);  // histcnt z29.s, p4/z, z22.s, z10.s
    // vl128 state = 0x9f8d9611
    __ dci(0x45abd2f5);  // histcnt z21.s, p4/z, z23.s, z11.s
    // vl128 state = 0x5cc45fb0
    __ dci(0x45aad0f7);  // histcnt z23.s, p4/z, z7.s, z10.s
    // vl128 state = 0x5096a07f
    __ dci(0x45aad1b3);  // histcnt z19.s, p4/z, z13.s, z10.s
    // vl128 state = 0xf25781a6
    __ dci(0x45a8d1f2);  // histcnt z18.s, p4/z, z15.s, z8.s
    // vl128 state = 0xc7025934
    __ dci(0x45a0d0f6);  // histcnt z22.s, p4/z, z7.s, z0.s
    // vl128 state = 0xcda9c72a
    __ dci(0x45a0d87e);  // histcnt z30.s, p6/z, z3.s, z0.s
    // vl128 state = 0x75f6bbcc
    __ dci(0x45a0dc4e);  // histcnt z14.s, p7/z, z2.s, z0.s
    // vl128 state = 0x5e4e9fe0
    __ dci(0x45a0dc4a);  // histcnt z10.s, p7/z, z2.s, z0.s
    // vl128 state = 0x0ec8d2b8
    __ dci(0x45b0cc4b);  // histcnt z11.s, p3/z, z2.s, z16.s
    // vl128 state = 0x1228c442
    __ dci(0x45b0cc43);  // histcnt z3.s, p3/z, z2.s, z16.s
    // vl128 state = 0xc6067f7b
    __ dci(0x45b8cc73);  // histcnt z19.s, p3/z, z3.s, z24.s
    // vl128 state = 0xf04f9753
    __ dci(0x45b8d877);  // histcnt z23.s, p6/z, z3.s, z24.s
    // vl128 state = 0xdeb83b41
    __ dci(0x45b8d47f);  // histcnt z31.s, p5/z, z3.s, z24.s
    // vl128 state = 0x8ab3905f
    __ dci(0x45b8d46f);  // histcnt z15.s, p5/z, z3.s, z24.s
    // vl128 state = 0x762bf277
    __ dci(0x45b8d16d);  // histcnt z13.s, p4/z, z11.s, z24.s
    // vl128 state = 0x9a670783
    __ dci(0x45bcd125);  // histcnt z5.s, p4/z, z9.s, z28.s
    // vl128 state = 0x3e399489
    __ dci(0x45b8d021);  // histcnt z1.s, p4/z, z1.s, z24.s
    // vl128 state = 0x7fc8f1e7
    __ dci(0x45f8d220);  // histcnt z0.d, p4/z, z17.d, z24.d
    // vl128 state = 0x9cb004db
    __ dci(0x45f0d621);  // histcnt z1.d, p5/z, z17.d, z16.d
    // vl128 state = 0xdd4161b5
    __ dci(0x45a0d625);  // histcnt z5.s, p5/z, z17.s, z0.s
    // vl128 state = 0xb5cb70bb
    __ dci(0x45a0d4a1);  // histcnt z1.s, p5/z, z5.s, z0.s
    // vl128 state = 0x4452182b
    __ dci(0x45a0d4a3);  // histcnt z3.s, p5/z, z5.s, z0.s
    // vl128 state = 0x71298d3c
    __ dci(0x45a0d4a2);  // histcnt z2.s, p5/z, z5.s, z0.s
    // vl128 state = 0xa22914e1
    __ dci(0x45a2d6a3);  // histcnt z3.s, p5/z, z21.s, z2.s
    // vl128 state = 0x6183bfbc
    __ dci(0x45a2de21);  // histcnt z1.s, p7/z, z17.s, z2.s
    // vl128 state = 0xd1ebb242
    __ dci(0x45e2dc20);  // histcnt z0.d, p7/z, z1.d, z2.d
    // vl128 state = 0x297a432d
    __ dci(0x45e2d8b0);  // histcnt z16.d, p6/z, z5.d, z2.d
    // vl128 state = 0x1d2557c0
    __ dci(0x45eed8b8);  // histcnt z24.d, p6/z, z5.d, z14.d
    // vl128 state = 0xe6ef07fa
    __ dci(0x45eed8a8);  // histcnt z8.d, p6/z, z5.d, z14.d
    // vl128 state = 0xaf3665bb
    __ dci(0x45aed88c);  // histcnt z12.s, p6/z, z4.s, z14.s
    // vl128 state = 0x5c2b38bc
    __ dci(0x45efd88d);  // histcnt z13.d, p6/z, z4.d, z15.d
    // vl128 state = 0x8d5527d8
    __ dci(0x45ffc88f);  // histcnt z15.d, p2/z, z4.d, z31.d
    // vl128 state = 0x1d2e08d2
    __ dci(0x45fbc98d);  // histcnt z13.d, p2/z, z12.d, z27.d
    // vl128 state = 0x007388b0
    __ dci(0x45bbcd8f);  // histcnt z15.s, p3/z, z12.s, z27.s
    // vl128 state = 0x9008a7ba
    __ dci(0x45b3cc9f);  // histcnt z31.s, p3/z, z4.s, z19.s
    // vl128 state = 0xc4030ca4
    __ dci(0x45bbc497);  // histcnt z23.s, p1/z, z4.s, z27.s
    // vl128 state = 0xeaf4a0b6
    __ dci(0x45fbc415);  // histcnt z21.d, p1/z, z0.d, z27.d
    // vl128 state = 0x03d85428
    __ dci(0x45ffc517);  // histcnt z23.d, p1/z, z8.d, z31.d
    // vl128 state = 0xa836a751
    __ dci(0x45fbc596);  // histcnt z22.d, p1/z, z12.d, z27.d
    // vl128 state = 0x77e33f69
    __ dci(0x45fbc4c6);  // histcnt z6.d, p1/z, z6.d, z27.d
    // vl128 state = 0xf47bb379
    __ dci(0x45fbc4ce);  // histcnt z14.d, p1/z, z6.d, z27.d
    // vl128 state = 0x6dbfff33
    __ dci(0x45fad4ca);  // histcnt z10.d, p5/z, z6.d, z26.d
    // vl128 state = 0xbc04915a
    __ dci(0x45ead45a);  // histcnt z26.d, p5/z, z2.d, z10.d
    // vl128 state = 0x8969b1c5
    __ dci(0x45aad4ca);  // histcnt z10.s, p5/z, z6.s, z10.s
    // vl128 state = 0x58d2dfac
    __ dci(0x45aed0ce);  // histcnt z14.s, p4/z, z6.s, z14.s
    // vl128 state = 0xfa793cc7
    __ dci(0x45aec4c6);  // histcnt z6.s, p1/z, z6.s, z14.s
    // vl128 state = 0xff4c99d8
    __ dci(0x45abc4c7);  // histcnt z7.s, p1/z, z6.s, z11.s
    // vl128 state = 0x2b44a4ae
    __ dci(0x45abc4cf);  // histcnt z15.s, p1/z, z6.s, z11.s
    // vl128 state = 0xbb3f8ba4
    __ dci(0x45a9c44e);  // histcnt z14.s, p1/z, z2.s, z9.s
    // vl128 state = 0x5a3a40a6
    __ dci(0x45b9c46f);  // histcnt z15.s, p1/z, z3.s, z25.s
    // vl128 state = 0x72e31c5f
    __ dci(0x45b9c46e);  // histcnt z14.s, p1/z, z3.s, z25.s
    // vl128 state = 0xde56263e
    __ dci(0x45b1c67e);  // histcnt z30.s, p1/z, z19.s, z17.s
    // vl128 state = 0xc570f0b9
    __ dci(0x45b5c63a);  // histcnt z26.s, p1/z, z17.s, z21.s
    // vl128 state = 0x72ab1716
    __ dci(0x45a5c72a);  // histcnt z10.s, p1/z, z25.s, z5.s
    // vl128 state = 0xe8848b2d
    __ dci(0x45a1c77a);  // histcnt z26.s, p1/z, z27.s, z1.s
    // vl128 state = 0x2975ac38
    __ dci(0x45a1c77b);  // histcnt z27.s, p1/z, z27.s, z1.s
    // vl128 state = 0xb0638363
    __ dci(0x45a1c773);  // histcnt z19.s, p1/z, z27.s, z1.s
    // vl128 state = 0xc9620a45
    __ dci(0x45e9c777);  // histcnt z23.d, p1/z, z27.d, z9.d
    // vl128 state = 0x0414c679
    __ dci(0x45ebc67f);  // histcnt z31.d, p1/z, z19.d, z11.d
    // vl128 state = 0xc1d4410e
    __ dci(0x45ebc37b);  // histcnt z27.d, p0/z, z27.d, z11.d
    // vl128 state = 0x3ae32e36
    __ dci(0x45abd373);  // histcnt z19.s, p4/z, z27.s, z11.s
    // vl128 state = 0x75ffe12c
    __ dci(0x45fbd363);  // histcnt z3.d, p4/z, z27.d, z27.d
    // vl128 state = 0x4084743b
    __ dci(0x45ffc36b);  // histcnt z11.d, p0/z, z27.d, z31.d
    // vl128 state = 0xfade136b
    __ dci(0x45ffc3ca);  // histcnt z10.d, p0/z, z30.d, z31.d
    // vl128 state = 0x60f18f50
    __ dci(0x45efc2ce);  // histcnt z14.d, p0/z, z22.d, z15.d
    // vl128 state = 0x162ed112
    __ dci(0x45adc2c6);  // histcnt z6.s, p0/z, z22.s, z13.s
    // vl128 state = 0x4f84cb96
    __ dci(0x45adc2c4);  // histcnt z4.s, p0/z, z22.s, z13.s
    // vl128 state = 0x5d04ccb6
    __ dci(0x45a7c2d4);  // histcnt z20.s, p0/z, z22.s, z7.s
    // vl128 state = 0x38efdab7
    __ dci(0x45a6c0c4);  // histcnt z4.s, p0/z, z6.s, z6.s
    // vl128 state = 0xff7a0a24
    __ dci(0x45a7c2c0);  // histcnt z0.s, p0/z, z22.s, z7.s
    // vl128 state = 0x5f7b0a31
    __ dci(0x45a7d6c1);  // histcnt z1.s, p5/z, z22.s, z7.s
    // vl128 state = 0x1e8a6f5f
    __ dci(0x45afd7c5);  // histcnt z5.s, p5/z, z30.s, z15.s
    // vl128 state = 0x655ed237
    __ dci(0x45add3d5);  // histcnt z21.s, p4/z, z30.s, z13.s
    // vl128 state = 0x8c7226a9
    __ dci(0x45add3d4);  // histcnt z20.s, p4/z, z30.s, z13.s
    // vl128 state = 0x727304ad
    __ dci(0x45bcd3dc);  // histcnt z28.s, p4/z, z30.s, z28.s
    // vl128 state = 0xce4e49d0
    __ dci(0x45bcd3cc);  // histcnt z12.s, p4/z, z30.s, z28.s
    // vl128 state = 0x5c252d7d
    __ dci(0x45bcd15c);  // histcnt z28.s, p4/z, z10.s, z28.s
    // vl128 state = 0x5e1163f7
    __ dci(0x45b5d154);  // histcnt z20.s, p4/z, z10.s, z21.s
    // vl128 state = 0xf77c50ee
    __ dci(0x45b5d156);  // histcnt z22.s, p4/z, z10.s, z21.s
    // vl128 state = 0xe35c8438
    __ dci(0x45b3d157);  // histcnt z23.s, p4/z, z10.s, z19.s
    // vl128 state = 0xf6926673
    __ dci(0x45b3d156);  // histcnt z22.s, p4/z, z10.s, z19.s
    // vl128 state = 0xf9022ad2
    __ dci(0x45b3c554);  // histcnt z20.s, p1/z, z10.s, z19.s
    // vl128 state = 0xb90dfe28
    __ dci(0x45bbd55c);  // histcnt z28.s, p5/z, z10.s, z27.s
    // vl128 state = 0x9a939b84
    __ dci(0x45abd57e);  // histcnt z30.s, p5/z, z11.s, z11.s
    // vl128 state = 0xd9ad8be7
    __ dci(0x45abcd7a);  // histcnt z26.s, p3/z, z11.s, z11.s
    // vl128 state = 0x14869e4f
    __ dci(0x45bbc57b);  // histcnt z27.s, p1/z, z11.s, z27.s
    // vl128 state = 0x25130793
    __ dci(0x45bfcd73);  // histcnt z19.s, p3/z, z11.s, z31.s
    // vl128 state = 0x53adf455
    __ dci(0x45bfc863);  // histcnt z3.s, p2/z, z3.s, z31.s
    // vl128 state = 0x82fa6c44
    __ dci(0x45b7cc62);  // histcnt z2.s, p3/z, z3.s, z23.s
    // vl128 state = 0xfaefda71
    __ dci(0x45b6cce3);  // histcnt z3.s, p3/z, z7.s, z22.s
    // vl128 state = 0xdd697c2a
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xdd697c2a,
        0x1415ff61,
        0xb9e154c8,
        0x566a2af5,
        0xef7574b4,
        0x6da83471,
        0x356d5c4d,
        0x798a2403,
        0x2c16e862,
        0x6fa84021,
        0x6e09e8ff,
        0xc13a0eb6,
        0x88c92928,
        0xe51672fe,
        0x229b8ed5,
        0x9e662757,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_histseg) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 100 * kInstructionSize);
    __ dci(0x4524a228);  // histseg z8.b, z17.b, z4.b
    // vl128 state = 0x21ed28a1
    __ dci(0x452ca20c);  // histseg z12.b, z16.b, z12.b
    // vl128 state = 0xc135d593
    __ dci(0x453ca288);  // histseg z8.b, z20.b, z28.b
    // vl128 state = 0xb86cd6e7
    __ dci(0x4538a380);  // histseg z0.b, z28.b, z24.b
    // vl128 state = 0xd28ddd71
    __ dci(0x452aa388);  // histseg z8.b, z28.b, z10.b
    // vl128 state = 0x322d3aa8
    __ dci(0x452aa38c);  // histseg z12.b, z28.b, z10.b
    // vl128 state = 0x67d668fc
    __ dci(0x4532a384);  // histseg z4.b, z28.b, z18.b
    // vl128 state = 0xc57505d4
    __ dci(0x4537a380);  // histseg z0.b, z28.b, z23.b
    // vl128 state = 0xb47d0a11
    __ dci(0x4535a3a8);  // histseg z8.b, z29.b, z21.b
    // vl128 state = 0x347adf6f
    __ dci(0x4535a3ac);  // histseg z12.b, z29.b, z21.b
    // vl128 state = 0xb763510c
    __ dci(0x4535a3ae);  // histseg z14.b, z29.b, z21.b
    // vl128 state = 0xb28319d5
    __ dci(0x4525a39e);  // histseg z30.b, z28.b, z5.b
    // vl128 state = 0x0adc6533
    __ dci(0x4525a38e);  // histseg z14.b, z28.b, z5.b
    // vl128 state = 0x248409c6
    __ dci(0x452da3c6);  // histseg z6.b, z30.b, z13.b
    // vl128 state = 0xa71c85d6
    __ dci(0x452da187);  // histseg z7.b, z12.b, z13.b
    // vl128 state = 0x7314b8a0
    __ dci(0x4525a1a6);  // histseg z6.b, z13.b, z5.b
    // vl128 state = 0x129013d5
    __ dci(0x4527a18e);  // histseg z14.b, z12.b, z7.b
    // vl128 state = 0xc6b207b7
    __ dci(0x4521a18c);  // histseg z12.b, z12.b, z1.b
    // vl128 state = 0x03957bb5
    __ dci(0x4524a18d);  // histseg z13.b, z12.b, z4.b
    // vl128 state = 0x379af1c6
    __ dci(0x4524a125);  // histseg z5.b, z9.b, z4.b
    // vl128 state = 0x93c462cc
    __ dci(0x4522a127);  // histseg z7.b, z9.b, z2.b
    // vl128 state = 0xc95cb1a9
    __ dci(0x4532a117);  // histseg z23.b, z8.b, z18.b
    // vl128 state = 0xc50e4e66
    __ dci(0x4533a15f);  // histseg z31.b, z10.b, z19.b
    // vl128 state = 0x76663e3e
    __ dci(0x4533a14f);  // histseg z15.b, z10.b, z19.b
    // vl128 state = 0x84f5ca5f
    __ dci(0x4533a0ce);  // histseg z14.b, z6.b, z19.b
    // vl128 state = 0x50d7de3d
    __ dci(0x453ba1cc);  // histseg z12.b, z14.b, z27.b
    // vl128 state = 0x32e3b53f
    __ dci(0x453ba0fc);  // histseg z28.b, z7.b, z27.b
    // vl128 state = 0x0a5d4180
    __ dci(0x452ba2f4);  // histseg z20.b, z23.b, z11.b
    // vl128 state = 0x91b77585
    __ dci(0x453ba2c4);  // histseg z4.b, z22.b, z27.b
    // vl128 state = 0x5cd0c690
    __ dci(0x453ba2cc);  // histseg z12.b, z22.b, z27.b
    // vl128 state = 0xa6a5f749
    __ dci(0x453ba1c8);  // histseg z8.b, z14.b, z27.b
    // vl128 state = 0xe5036937
    __ dci(0x4529a1c9);  // histseg z9.b, z14.b, z9.b
    // vl128 state = 0x13c620c8
    __ dci(0x4529a1a8);  // histseg z8.b, z13.b, z9.b
    // vl128 state = 0xbf71d421
    __ dci(0x4521a198);  // histseg z24.b, z12.b, z1.b
    // vl128 state = 0xe01d1160
    __ dci(0x4529a1ba);  // histseg z26.b, z13.b, z9.b
    // vl128 state = 0xaa1b29d6
    __ dci(0x452fa1bb);  // histseg z27.b, z13.b, z15.b
    // vl128 state = 0x2f96bd61
    __ dci(0x452fa0ff);  // histseg z31.b, z7.b, z15.b
    // vl128 state = 0x5aeb6bec
    __ dci(0x4527a0de);  // histseg z30.b, z6.b, z7.b
    // vl128 state = 0xbcb1b299
    __ dci(0x4525a1d6);  // histseg z22.b, z14.b, z5.b
    // vl128 state = 0x0f89ea9b
    __ dci(0x4525a1d7);  // histseg z23.b, z14.b, z5.b
    // vl128 state = 0xe40f30a2
    __ dci(0x4521a3df);  // histseg z31.b, z30.b, z1.b
    // vl128 state = 0x342ff33b
    __ dci(0x4521a197);  // histseg z23.b, z12.b, z1.b
    // vl128 state = 0xdfa92902
    __ dci(0x4521a187);  // histseg z7.b, z12.b, z1.b
    // vl128 state = 0x8531fa67
    __ dci(0x4535a186);  // histseg z6.b, z12.b, z21.b
    // vl128 state = 0xe4b55112
    __ dci(0x4535a196);  // histseg z22.b, z12.b, z21.b
    // vl128 state = 0x5d26970e
    __ dci(0x4525a097);  // histseg z23.b, z4.b, z5.b
    // vl128 state = 0x7dcb1d13
    __ dci(0x4525a095);  // histseg z21.b, z4.b, z5.b
    // vl128 state = 0x5fb0789c
    __ dci(0x452da017);  // histseg z23.b, z0.b, z13.b
    // vl128 state = 0x7f5df281
    __ dci(0x452da295);  // histseg z21.b, z20.b, z13.b
    // vl128 state = 0x9e6f5eaf
    __ dci(0x453da39d);  // histseg z29.b, z28.b, z29.b
    // vl128 state = 0x532f95a9
    __ dci(0x453da39c);  // histseg z28.b, z28.b, z29.b
    // vl128 state = 0x64202514
    __ dci(0x4535a29e);  // histseg z30.b, z20.b, z21.b
    // vl128 state = 0x44bda972
    __ dci(0x4535a0bf);  // histseg z31.b, z5.b, z21.b
    // vl128 state = 0x258125d6
    __ dci(0x4535a0bb);  // histseg z27.b, z5.b, z21.b
    // vl128 state = 0xec63caaf
    __ dci(0x4537a2b3);  // histseg z19.b, z21.b, z23.b
    // vl128 state = 0xb937b6e8
    __ dci(0x4525a2b1);  // histseg z17.b, z21.b, z5.b
    // vl128 state = 0x1515ee94
    __ dci(0x4525a2b5);  // histseg z21.b, z21.b, z5.b
    // vl128 state = 0x4bb06873
    __ dci(0x4525a0fd);  // histseg z29.b, z7.b, z5.b
    // vl128 state = 0x23446114
    __ dci(0x4524a079);  // histseg z25.b, z3.b, z4.b
    // vl128 state = 0x48d52cf6
    __ dci(0x4524a0d8);  // histseg z24.b, z6.b, z4.b
    // vl128 state = 0x0deef019
    __ dci(0x452ca09c);  // histseg z28.b, z4.b, z12.b
    // vl128 state = 0xaba6e202
    __ dci(0x453ca018);  // histseg z24.b, z0.b, z28.b
    // vl128 state = 0xee9d3eed
    __ dci(0x4539a008);  // histseg z8.b, z0.b, z25.b
    // vl128 state = 0x254c57f3
    __ dci(0x4539a00c);  // histseg z12.b, z0.b, z25.b
    // vl128 state = 0x28fea24d
    __ dci(0x4531a048);  // histseg z8.b, z2.b, z17.b
    // vl128 state = 0xe32fcb53
    __ dci(0x4530a0ca);  // histseg z10.b, z6.b, z16.b
    // vl128 state = 0xb3a9860b
    __ dci(0x4520a0ee);  // histseg z14.b, z7.b, z0.b
    // vl128 state = 0xef9e57fa
    __ dci(0x4520a1de);  // histseg z30.b, z14.b, z0.b
    // vl128 state = 0x295902e9
    __ dci(0x4520a38e);  // histseg z14.b, z28.b, z0.b
    // vl128 state = 0x756ed318
    __ dci(0x4528a30f);  // histseg z15.b, z24.b, z8.b
    // vl128 state = 0x8591dff9
    __ dci(0x4538a39f);  // histseg z31.b, z28.b, z24.b
    // vl128 state = 0xe4ad535d
    __ dci(0x4538a39b);  // histseg z27.b, z28.b, z24.b
    // vl128 state = 0x2d4fbc24
    __ dci(0x4538a093);  // histseg z19.b, z4.b, z24.b
    // vl128 state = 0xd8ee932a
    __ dci(0x453aa0a3);  // histseg z3.b, z5.b, z26.b
    // vl128 state = 0x768b71a6
    __ dci(0x453aa0ab);  // histseg z11.b, z5.b, z26.b
    // vl128 state = 0xa78673d7
    __ dci(0x452ea0bb);  // histseg z27.b, z5.b, z14.b
    // vl128 state = 0x6e649cae
    __ dci(0x452fa1bf);  // histseg z31.b, z13.b, z15.b
    // vl128 state = 0x0f58100a
    __ dci(0x452fa1be);  // histseg z30.b, z13.b, z15.b
    // vl128 state = 0xc99f4519
    __ dci(0x452fa3f6);  // histseg z22.b, z31.b, z15.b
    // vl128 state = 0x700c8305
    __ dci(0x452fa3f4);  // histseg z20.b, z31.b, z15.b
    // vl128 state = 0xbdecfddc
    __ dci(0x453fa3b0);  // histseg z16.b, z29.b, z31.b
    // vl128 state = 0x3f5b7578
    __ dci(0x453fa3b8);  // histseg z24.b, z29.b, z31.b
    // vl128 state = 0xf0076715
    __ dci(0x453fa228);  // histseg z8.b, z17.b, z31.b
    // vl128 state = 0x3bd60e0b
    __ dci(0x4536a22a);  // histseg z10.b, z17.b, z22.b
    // vl128 state = 0x1171f63c
    __ dci(0x4530a23a);  // histseg z26.b, z17.b, z16.b
    // vl128 state = 0x3fef270c
    __ dci(0x4522a23e);  // histseg z30.b, z17.b, z2.b
    // vl128 state = 0xf928721f
    __ dci(0x4524a23c);  // histseg z28.b, z17.b, z4.b
    // vl128 state = 0xecec697b
    __ dci(0x4527a238);  // histseg z24.b, z17.b, z7.b
    // vl128 state = 0x23b07b16
    __ dci(0x4525a210);  // histseg z16.b, z16.b, z5.b
    // vl128 state = 0x9c1c2ac5
    __ dci(0x4525a200);  // histseg z0.b, z16.b, z5.b
    // vl128 state = 0xc446f89b
    __ dci(0x4520a202);  // histseg z2.b, z16.b, z0.b
    // vl128 state = 0x8afba046
    __ dci(0x4521a303);  // histseg z3.b, z24.b, z1.b
    // vl128 state = 0xf0b0f9f3
    __ dci(0x4520a201);  // histseg z1.b, z16.b, z0.b
    // vl128 state = 0x8922615b
    __ dci(0x4528a223);  // histseg z3.b, z17.b, z8.b
    // vl128 state = 0xf36938ee
    __ dci(0x4528a367);  // histseg z7.b, z27.b, z8.b
    // vl128 state = 0xc2d96c41
    __ dci(0x452ca3e6);  // histseg z6.b, z31.b, z12.b
    // vl128 state = 0xf15e835f
    __ dci(0x452ea3c4);  // histseg z4.b, z30.b, z14.b
    // vl128 state = 0xb3964bd8
    __ dci(0x452da3c6);  // histseg z6.b, z30.b, z13.b
    // vl128 state = 0x8011a4c6
    __ dci(0x452da0c4);  // histseg z4.b, z6.b, z13.b
    // vl128 state = 0x0fbedf54
    __ dci(0x4529a0ec);  // histseg z12.b, z7.b, z9.b
    // vl128 state = 0x9a4d7031
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x9a4d7031,
        0xebaa80ad,
        0x702155a3,
        0x181fff8d,
        0x7b071373,
        0x1bf0af96,
        0x9ca15297,
        0x615d2f4a,
        0x7658b554,
        0xd2bf7319,
        0xddf8d492,
        0xf5938d08,
        0xbe354cb1,
        0xfe2d5d63,
        0x29818684,
        0x2c862ef9,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_table) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x05212a38);  // tbl z24.b, {z17.b, z18.b}, z1.b
    // vl128 state = 0xbdd1e1c1
    __ dci(0x05212810);  // tbl z16.b, {z0.b, z1.b}, z1.b
    // vl128 state = 0x80ca38b6
    __ dci(0x05e12812);  // tbl z18.d, {z0.d, z1.d}, z1.d
    // vl128 state = 0xb59fe024
    __ dci(0x05632802);  // tbl z2.h, {z0.h, z1.h}, z3.h
    // vl128 state = 0xfb22b8f9
    __ dci(0x05e32906);  // tbl z6.d, {z8.d, z9.d}, z3.d
    // vl128 state = 0x78ba34e9
    __ dci(0x05e22942);  // tbl z2.d, {z10.d, z11.d}, z2.d
    // vl128 state = 0x000b006f
    __ dci(0x05f22d46);  // tbx z6.d, z10.d, z18.d
    // vl128 state = 0x28b746e5
    __ dci(0x05f32947);  // tbl z7.d, {z10.d, z11.d}, z19.d
    // vl128 state = 0xfcbf7b93
    __ dci(0x05e32963);  // tbl z3.d, {z11.d, z12.d}, z3.d
    // vl128 state = 0x2891c0aa
    __ dci(0x05e33161);  // tbl z1.d, {z11.d}, z3.d
    // vl128 state = 0x3468b9d4
    __ dci(0x05e13149);  // tbl z9.d, {z10.d}, z1.d
    // vl128 state = 0xc2adf02b
    __ dci(0x0560314d);  // tbl z13.h, {z10.h}, z0.h
    // vl128 state = 0xff9f1abb
    __ dci(0x0578314c);  // tbl z12.h, {z10.h}, z24.h
    // vl128 state = 0x2cffcd38
    __ dci(0x05e83144);  // tbl z4.d, {z10.d}, z8.d
    // vl128 state = 0x8e5ca010
    __ dci(0x05e83146);  // tbl z6.d, {z10.d}, z8.d
    // vl128 state = 0xa6e0e69a
    __ dci(0x05b83147);  // tbl z7.s, {z10.s}, z24.s
    // vl128 state = 0x513e6328
    __ dci(0x053831d7);  // tbl z23.b, {z14.b}, z24.b
    // vl128 state = 0xe2bd7bdf
    __ dci(0x056831df);  // tbl z31.h, {z14.h}, z8.h
    // vl128 state = 0xf4881e93
    __ dci(0x0560319e);  // tbl z30.h, {z12.h}, z0.h
    // vl128 state = 0x4cd76275
    __ dci(0x0522319a);  // tbl z26.b, {z12.b}, z2.b
    // vl128 state = 0x06d15ac3
    __ dci(0x0522318a);  // tbl z10.b, {z12.b}, z2.b
    // vl128 state = 0x5657179b
    __ dci(0x0522318e);  // tbl z14.b, {z12.b}, z2.b
    // vl128 state = 0x7def33b7
    __ dci(0x05a6318a);  // tbl z10.s, {z12.s}, z6.s
    // vl128 state = 0x38ee6756
    __ dci(0x05b2318b);  // tbl z11.s, {z12.s}, z18.s
    // vl128 state = 0x6ba1d599
    __ dci(0x05a231bb);  // tbl z27.s, {z13.s}, z2.s
    // vl128 state = 0xee2c412e
    __ dci(0x05a231ab);  // tbl z11.s, {z13.s}, z2.s
    // vl128 state = 0xa183e51b
    __ dci(0x05a831af);  // tbl z15.s, {z13.s}, z8.s
    // vl128 state = 0xcd60a839
    __ dci(0x05ea31a7);  // tbl z7.d, {z13.d}, z10.d
    // vl128 state = 0x3abe2d8b
    __ dci(0x05fa33af);  // tbl z15.d, {z29.d}, z26.d
    // vl128 state = 0xf596f00c
    __ dci(0x05fe32ae);  // tbl z14.d, {z21.d}, z30.d
    // vl128 state = 0x3e791a5a
    __ dci(0x057a32be);  // tbl z30.h, {z21.h}, z26.h
    // vl128 state = 0x27f4086e
    __ dci(0x05fe32ae);  // tbl z14.d, {z21.d}, z30.d
    // vl128 state = 0xec1be238
    __ dci(0x05fe32aa);  // tbl z10.d, {z21.d}, z30.d
    // vl128 state = 0xa91ab6d9
    __ dci(0x057e32e2);  // tbl z2.h, {z23.h}, z30.h
    // vl128 state = 0xd1ab825f
    __ dci(0x057e32e0);  // tbl z0.h, {z23.h}, z30.h
    // vl128 state = 0xca42860c
    __ dci(0x057f3270);  // tbl z16.h, {z19.h}, z31.h
    // vl128 state = 0xff27daa0
    __ dci(0x05673271);  // tbl z17.h, {z19.h}, z7.h
    // vl128 state = 0x9b358bbf
    __ dci(0x05e73379);  // tbl z25.d, {z27.d}, z7.d
    // vl128 state = 0xf0a4c65d
    __ dci(0x05e3333d);  // tbl z29.d, {z25.d}, z3.d
    // vl128 state = 0x3de40d5b
    __ dci(0x05e33335);  // tbl z21.d, {z25.d}, z3.d
    // vl128 state = 0xfeadc4fa
    __ dci(0x05f33137);  // tbl z23.d, {z9.d}, z19.d
    // vl128 state = 0x417c23c2
    __ dci(0x05b33336);  // tbl z22.s, {z25.s}, z19.s
    // vl128 state = 0x4bd7bddc
    __ dci(0x05b1323e);  // tbl z30.s, {z17.s}, z17.s
    // vl128 state = 0x525aafe8
    __ dci(0x05b0303c);  // tbl z28.s, {z1.s}, z16.s
    // vl128 state = 0xee67e295
    __ dci(0x05b0308c);  // tbl z12.s, {z4.s}, z16.s
    // vl128 state = 0xce1a6811
    __ dci(0x05b030e8);  // tbl z8.s, {z7.s}, z16.s
    // vl128 state = 0xfba53f74
    __ dci(0x05a030b8);  // tbl z24.s, {z5.s}, z0.s
    // vl128 state = 0x56a69350
    __ dci(0x05e830b0);  // tbl z16.d, {z5.d}, z8.d
    // vl128 state = 0xe0665941
    __ dci(0x05e830b2);  // tbl z18.d, {z5.d}, z8.d
    // vl128 state = 0xc6680470
    __ dci(0x05e931b3);  // tbl z19.d, {z13.d}, z9.d
    // vl128 state = 0x64a925a9
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x64a925a9,
        0x89750b9d,
        0xb803659e,
        0xa21efc63,
        0x67f967b8,
        0x4e52e209,
        0x42c1692f,
        0x4d8539c7,
        0x6828f0f4,
        0x3c75d27a,
        0x2e3341c9,
        0xfe4a8f4f,
        0xd27b47ae,
        0x665d8f8b,
        0x3230c584,
        0xcf1d6e82,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_cdot) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x4488104f);  // cdot z15.s, z2.b, z8.b, #0
    // vl128 state = 0x25fd51d1
    __ dci(0x448a106e);  // cdot z14.s, z3.b, z10.b, #0
    // vl128 state = 0x490576d5
    __ dci(0x448a1246);  // cdot z6.s, z18.b, z10.b, #0
    // vl128 state = 0x25a6fe4b
    __ dci(0x448e12ce);  // cdot z14.s, z22.b, z14.b, #0
    // vl128 state = 0xc378b2df
    __ dci(0x448412cf);  // cdot z15.s, z22.b, z4.b, #0
    // vl128 state = 0xe92a358d
    __ dci(0x448412c7);  // cdot z7.s, z22.b, z4.b, #0
    // vl128 state = 0x7408b292
    __ dci(0x44c41257);  // cdot z23.d, z18.h, z4.h, #0
    // vl128 state = 0xebc02289
    __ dci(0x448412d5);  // cdot z21.s, z22.b, z4.b, #0
    // vl128 state = 0x9a7c2f1a
    __ dci(0x448712d7);  // cdot z23.s, z22.b, z7.b, #0
    // vl128 state = 0xed91e0b4
    __ dci(0x44831295);  // cdot z21.s, z20.b, z3.b, #0
    // vl128 state = 0x3dae4184
    __ dci(0x44821385);  // cdot z5.s, z28.b, z2.b, #0
    // vl128 state = 0x213fb541
    __ dci(0x44c213c1);  // cdot z1.d, z30.h, z2.h, #0
    // vl128 state = 0xcba3207a
    __ dci(0x44c61340);  // cdot z0.d, z26.h, z6.h, #0
    // vl128 state = 0x9d6041f3
    __ dci(0x44c413d0);  // cdot z16.d, z30.h, z4.h, #0
    // vl128 state = 0x4b931738
    __ dci(0x44cc12d8);  // cdot z24.d, z22.h, z12.h, #0
    // vl128 state = 0x2503fbcc
    __ dci(0x448c1ac8);  // cdot z8.s, z22.b, z12.b, #180
    // vl128 state = 0x53bc5303
    __ dci(0x448c12ec);  // cdot z12.s, z23.b, z12.b, #0
    // vl128 state = 0xb3bf45c7
    __ dci(0x448812ad);  // cdot z13.s, z21.b, z8.b, #0
    // vl128 state = 0x938b4e4f
    __ dci(0x44881689);  // cdot z9.s, z20.b, z8.b, #90
    // vl128 state = 0x70106ddd
    __ dci(0x4498128b);  // cdot z11.s, z20.b, z24.b, #0
    // vl128 state = 0x92108bb2
    __ dci(0x4498129b);  // cdot z27.s, z20.b, z24.b, #0
    // vl128 state = 0x545230eb
    __ dci(0x449a12bf);  // cdot z31.s, z21.b, z26.b, #0
    // vl128 state = 0x5cd2fb12
    __ dci(0x44da10af);  // cdot z15.d, z5.h, z26.h, #0
    // vl128 state = 0xc03d9146
    __ dci(0x44da10ae);  // cdot z14.d, z5.h, z26.h, #0
    // vl128 state = 0xbc2712f7
    __ dci(0x44db12be);  // cdot z30.d, z21.h, z27.h, #0
    // vl128 state = 0xccf9d667
    __ dci(0x449b12ee);  // cdot z14.s, z23.b, z27.b, #0
    // vl128 state = 0x2c1e08f1
    __ dci(0x449b12ef);  // cdot z15.s, z23.b, z27.b, #0
    // vl128 state = 0x159d17d7
    __ dci(0x449b14ee);  // cdot z14.s, z7.b, z27.b, #90
    // vl128 state = 0x892c97d3
    __ dci(0x449b1cac);  // cdot z12.s, z5.b, z27.b, #270
    // vl128 state = 0x3841ce24
    __ dci(0x449b1aae);  // cdot z14.s, z21.b, z27.b, #180
    // vl128 state = 0x30a24868
    __ dci(0x449a1aec);  // cdot z12.s, z23.b, z26.b, #180
    // vl128 state = 0x2b836c8a
    __ dci(0x44981ace);  // cdot z14.s, z22.b, z24.b, #180
    // vl128 state = 0x16a81963
    __ dci(0x44901a86);  // cdot z6.s, z20.b, z16.b, #180
    // vl128 state = 0x924ac9ee
    __ dci(0x44981b8e);  // cdot z14.s, z28.b, z24.b, #180
    // vl128 state = 0x3953da61
    __ dci(0x44891b8a);  // cdot z10.s, z28.b, z9.b, #180
    // vl128 state = 0xad72b6d5
    __ dci(0x4499138b);  // cdot z11.s, z28.b, z25.b, #0
    // vl128 state = 0x569b1b2c
    __ dci(0x4498119b);  // cdot z27.s, z12.b, z24.b, #0
    // vl128 state = 0xdbb36925
    __ dci(0x449c199a);  // cdot z26.s, z12.b, z28.b, #180
    // vl128 state = 0x4be861d1
    __ dci(0x44901992);  // cdot z18.s, z12.b, z16.b, #180
    // vl128 state = 0x1e83ddb5
    __ dci(0x44901a90);  // cdot z16.s, z20.b, z16.b, #180
    // vl128 state = 0x180556e0
    __ dci(0x44911ac0);  // cdot z0.s, z22.b, z17.b, #180
    // vl128 state = 0x2cbf5db5
    __ dci(0x44951bc1);  // cdot z1.s, z30.b, z21.b, #180
    // vl128 state = 0x428f97bd
    __ dci(0x44851b40);  // cdot z0.s, z26.b, z5.b, #180
    // vl128 state = 0xe0f0659f
    __ dci(0x44851a70);  // cdot z16.s, z19.b, z5.b, #180
    // vl128 state = 0x4142d23c
    __ dci(0x44861a74);  // cdot z20.s, z19.b, z6.b, #180
    // vl128 state = 0x74f7d373
    __ dci(0x44921a76);  // cdot z22.s, z19.b, z18.b, #180
    // vl128 state = 0x5b4ef670
    __ dci(0x44921246);  // cdot z6.s, z18.b, z18.b, #0
    // vl128 state = 0x1fe5d31d
    __ dci(0x44981247);  // cdot z7.s, z18.b, z24.b, #0
    // vl128 state = 0x782a0559
    __ dci(0x44981746);  // cdot z6.s, z26.b, z24.b, #90
    // vl128 state = 0x84cbc61d
    __ dci(0x449816c4);  // cdot z4.s, z22.b, z24.b, #90
    // vl128 state = 0x078aa009
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x078aa009,
        0x3c4026df,
        0x3ae8e644,
        0x514dfdcd,
        0x2649444a,
        0x74a87bbe,
        0x14b8e9b3,
        0x92c65f4d,
        0xa3015fc1,
        0xab48b8fa,
        0x9e80ef05,
        0xb59b0dde,
        0xbcf04e6f,
        0xa7fa54a1,
        0xaed81dfc,
        0xdc7ffb07,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_bitwise_ternary) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x04793f99);  // bsl1n z25.d, z25.d, z25.d, z28.d
    // vl128 state = 0x70294e62
    __ dci(0x04b93f9b);  // bsl2n z27.d, z27.d, z25.d, z28.d
    // vl128 state = 0x0a3f0dc1
    __ dci(0x04b93f93);  // bsl2n z19.d, z19.d, z25.d, z28.d
    // vl128 state = 0x46500e35
    __ dci(0x04b93dbb);  // bsl2n z27.d, z27.d, z25.d, z13.d
    // vl128 state = 0x25bdcc83
    __ dci(0x04b53db9);  // bsl2n z25.d, z25.d, z21.d, z13.d
    // vl128 state = 0x6d33b943
    __ dci(0x04bd3d29);  // bsl2n z9.d, z9.d, z29.d, z9.d
    // vl128 state = 0xa218e11a
    __ dci(0x04ad3d0d);  // bsl2n z13.d, z13.d, z13.d, z8.d
    // vl128 state = 0xc5e2f5a2
    __ dci(0x04a53d4f);  // bsl2n z15.d, z15.d, z5.d, z10.d
    // vl128 state = 0x519e4735
    __ dci(0x04653d47);  // bsl1n z7.d, z7.d, z5.d, z10.d
    // vl128 state = 0x132f7ce6
    __ dci(0x04613dc6);  // bsl1n z6.d, z6.d, z1.d, z14.d
    // vl128 state = 0x91bcf19b
    __ dci(0x04673dc7);  // bsl1n z7.d, z7.d, z7.d, z14.d
    // vl128 state = 0x3bd0ba20
    __ dci(0x04673dc5);  // bsl1n z5.d, z5.d, z7.d, z14.d
    // vl128 state = 0xbf3b39fa
    __ dci(0x04e73cc1);  // nbsl z1.d, z1.d, z7.d, z6.d
    // vl128 state = 0xd304b643
    __ dci(0x04773cc5);  // bsl1n z5.d, z5.d, z23.d, z6.d
    // vl128 state = 0xdd6cd3ce
    __ dci(0x04773ac1);  // bcax z1.d, z1.d, z23.d, z22.d
    // vl128 state = 0x3f456acf
    __ dci(0x04773ac3);  // bcax z3.d, z3.d, z23.d, z22.d
    // vl128 state = 0xbe117f80
    __ dci(0x047739c7);  // bcax z7.d, z7.d, z23.d, z14.d
    // vl128 state = 0xd3cd3dcd
    __ dci(0x047439c5);  // bcax z5.d, z5.d, z20.d, z14.d
    // vl128 state = 0xee4f636d
    __ dci(0x04743841);  // bcax z1.d, z1.d, z20.d, z2.d
    // vl128 state = 0xf21b00a1
    __ dci(0x04753811);  // bcax z17.d, z17.d, z21.d, z0.d
    // vl128 state = 0x597ab14d
    __ dci(0x04753815);  // bcax z21.d, z21.d, z21.d, z0.d
    // vl128 state = 0xf5d56322
    __ dci(0x04713917);  // bcax z23.d, z23.d, z17.d, z8.d
    // vl128 state = 0x17f3cedf
    __ dci(0x04793987);  // bcax z7.d, z7.d, z25.d, z12.d
    // vl128 state = 0x7492c4e5
    __ dci(0x04693885);  // bcax z5.d, z5.d, z9.d, z4.d
    // vl128 state = 0xb796548c
    __ dci(0x046838d5);  // bcax z21.d, z21.d, z8.d, z6.d
    // vl128 state = 0xf4e12422
    __ dci(0x046838d4);  // bcax z20.d, z20.d, z8.d, z6.d
    // vl128 state = 0x16187a4c
    __ dci(0x043838d6);  // eor3 z22.d, z22.d, z24.d, z6.d
    // vl128 state = 0xd95e6713
    __ dci(0x043c39de);  // eor3 z30.d, z30.d, z28.d, z14.d
    // vl128 state = 0xb8322807
    __ dci(0x047c38ce);  // bcax z14.d, z14.d, z28.d, z6.d
    // vl128 state = 0x6871619d
    __ dci(0x047c38cf);  // bcax z15.d, z15.d, z28.d, z6.d
    // vl128 state = 0x57c5a4af
    __ dci(0x043c384e);  // eor3 z14.d, z14.d, z28.d, z2.d
    // vl128 state = 0x1a62efdf
    __ dci(0x0474385e);  // bcax z30.d, z30.d, z20.d, z2.d
    // vl128 state = 0xc9d1ea1e
    __ dci(0x047c3a4e);  // bcax z14.d, z14.d, z28.d, z18.d
    // vl128 state = 0xd5ced43e
    __ dci(0x047c3c4f);  // bsl1n z15.d, z15.d, z28.d, z2.d
    // vl128 state = 0x79f22e16
    __ dci(0x047d3d4b);  // bsl1n z11.d, z11.d, z29.d, z10.d
    // vl128 state = 0xc4ee5d6e
    __ dci(0x04793c49);  // bsl1n z9.d, z9.d, z25.d, z2.d
    // vl128 state = 0xea11e840
    __ dci(0x04793c99);  // bsl1n z25.d, z25.d, z25.d, z4.d
    // vl128 state = 0x95221bc2
    __ dci(0x04613c91);  // bsl1n z17.d, z17.d, z1.d, z4.d
    // vl128 state = 0xa40acfbe
    __ dci(0x04233c90);  // bsl z16.d, z16.d, z3.d, z4.d
    // vl128 state = 0x8d3ef22f
    __ dci(0x04233c80);  // bsl z0.d, z0.d, z3.d, z4.d
    // vl128 state = 0xd07d1bb2
    __ dci(0x04223ca4);  // bsl z4.d, z4.d, z2.d, z5.d
    // vl128 state = 0xa2c4169c
    __ dci(0x04223ca5);  // bsl z5.d, z5.d, z2.d, z5.d
    // vl128 state = 0x3c6415e5
    __ dci(0x04a03ca1);  // bsl2n z1.d, z1.d, z0.d, z5.d
    // vl128 state = 0x55b93add
    __ dci(0x04a03cb1);  // bsl2n z17.d, z17.d, z0.d, z5.d
    // vl128 state = 0x9b86e5b3
    __ dci(0x04a13cf9);  // bsl2n z25.d, z25.d, z1.d, z7.d
    // vl128 state = 0xdd310e8f
    __ dci(0x04a13cfd);  // bsl2n z29.d, z29.d, z1.d, z7.d
    // vl128 state = 0xae66fb44
    __ dci(0x04a13ced);  // bsl2n z13.d, z13.d, z1.d, z7.d
    // vl128 state = 0xc69dd926
    __ dci(0x04b93ce9);  // bsl2n z9.d, z9.d, z25.d, z7.d
    // vl128 state = 0x15592b37
    __ dci(0x04b93dcb);  // bsl2n z11.d, z11.d, z25.d, z14.d
    // vl128 state = 0xbfcda4d3
    __ dci(0x04b83d4f);  // bsl2n z15.d, z15.d, z24.d, z10.d
    // vl128 state = 0xaef1e0b6
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xaef1e0b6,
        0xc9b3303f,
        0xc547c948,
        0x0fc817f7,
        0x22d2eab3,
        0x225b3ecd,
        0xf7a34a06,
        0xa07e68ed,
        0xdba0f9fa,
        0x64199691,
        0xa650bfa3,
        0xc6bfeab9,
        0x7efe63c4,
        0x66e4139c,
        0xc580dcf5,
        0x95687693,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_while) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x257109e3);  // whilehs p3.h, w15, w17
    // vl128 state = 0x4568cc4c
    __ dci(0x257709f3);  // whilehi p3.h, w15, w23
    // vl128 state = 0xf148a8ac
    __ dci(0x25f509f7);  // whilehi p7.d, w15, w21
    // vl128 state = 0x2fe3dcb9
    __ dci(0x257508f5);  // whilehi p5.h, w7, w21
    // vl128 state = 0x88429dee
    __ dci(0x257100f1);  // whilegt p1.h, w7, w17
    // vl128 state = 0x5a3b89ec
    __ dci(0x253108f0);  // whilehi p0.b, w7, w17
    // vl128 state = 0x73276c52
    __ dci(0x253108f1);  // whilehi p1.b, w7, w17
    // vl128 state = 0xa278d7f0
    __ dci(0x257508f9);  // whilehi p9.h, w7, w21
    // vl128 state = 0xa438aefc
    __ dci(0x25750858);  // whilehi p8.h, w2, w21
    // vl128 state = 0x33e13c17
    __ dci(0x25770a50);  // whilehi p0.h, w18, w23
    // vl128 state = 0x01947abe
    __ dci(0x25751a52);  // whilehi p2.h, x18, x21
    // vl128 state = 0x2cf410f2
    __ dci(0x25711a7a);  // whilehi p10.h, x19, x17
    // vl128 state = 0x4bb6efc1
    __ dci(0x25391a78);  // whilehi p8.b, x19, x25
    // vl128 state = 0xec1afdd6
    __ dci(0x25290a70);  // whilehi p0.b, w19, w9
    // vl128 state = 0xde6fbb7f
    __ dci(0x25290a78);  // whilehi p8.b, w19, w9
    // vl128 state = 0x79c3a968
    __ dci(0x25a90b68);  // whilehs p8.s, w27, w9
    // vl128 state = 0x4b32e81a
    __ dci(0x25a903e9);  // whilege p9.s, wzr, w9
    // vl128 state = 0x994bfc18
    __ dci(0x25a909ed);  // whilehs p13.s, w15, w9
    // vl128 state = 0x6d6e231f
    __ dci(0x25a909ef);  // whilehs p15.s, w15, w9
    // vl128 state = 0x41945298
    __ dci(0x25a909eb);  // whilehs p11.s, w15, w9
    // vl128 state = 0x659ccb75
    __ dci(0x25b909c9);  // whilehs p9.s, w14, w25
    // vl128 state = 0xd078a7ed
    __ dci(0x25bd098d);  // whilehs p13.s, w12, w29
    // vl128 state = 0xf6f2d8ae
    __ dci(0x25b90909);  // whilehs p9.s, w8, w25
    // vl128 state = 0x248bccac
    __ dci(0x25fb090b);  // whilehs p11.d, w8, w27
    // vl128 state = 0x09b0b9cc
    __ dci(0x25fb090a);  // whilehs p10.d, w8, w27
    // vl128 state = 0xfa811fef
    __ dci(0x25eb0b02);  // whilehs p2.d, w24, w11
    // vl128 state = 0xdcb96f30
    __ dci(0x25eb0bc3);  // whilehs p3.d, w30, w11
    // vl128 state = 0xbae01fd2
    __ dci(0x25e30acb);  // whilehs p11.d, w22, w3
    // vl128 state = 0xbcfdc2b8
    __ dci(0x25eb08c9);  // whilehs p9.d, w6, w11
    // vl128 state = 0xdb60ba22
    __ dci(0x25a308c1);  // whilehs p1.s, w6, w3
    // vl128 state = 0xe895df80
    __ dci(0x25a108e5);  // whilehs p5.s, w7, w1
    // vl128 state = 0x3aeccb82
    __ dci(0x25a009e4);  // whilehs p4.s, w15, w0
    // vl128 state = 0xe6b1b3b3
    __ dci(0x25a009ec);  // whilehs p12.s, w15, w0
    // vl128 state = 0xd2e10d82
    __ dci(0x25a019ae);  // whilehs p14.s, x13, x0
    // vl128 state = 0x4bf596b8
    __ dci(0x25e018af);  // whilehs p15.d, x5, x0
    // vl128 state = 0xb8d27541
    __ dci(0x25e918ad);  // whilehs p13.d, x5, x9
    // vl128 state = 0x01b6f92f
    __ dci(0x25eb188c);  // whilehs p12.d, x4, x11
    // vl128 state = 0xd3cfed2d
    __ dci(0x25eb188e);  // whilehs p14.d, x4, x11
    // vl128 state = 0x9947e07e
    __ dci(0x25e21886);  // whilehs p6.d, x4, x2
    // vl128 state = 0xd9995e11
    __ dci(0x25a21084);  // whilege p4.s, x4, x2
    // vl128 state = 0xd45d81ed
    __ dci(0x25b31085);  // whilege p5.s, x4, x19
    // vl128 state = 0x4d67b543
    __ dci(0x25a3100d);  // whilege p13.s, x0, x3
    // vl128 state = 0x00f0526c
    __ dci(0x252b101d);  // whilegt p13.b, x0, x11
    // vl128 state = 0x9d176025
    __ dci(0x253b1095);  // whilegt p5.b, x4, x27
    // vl128 state = 0xd6544089
    __ dci(0x253b1091);  // whilegt p1.b, x4, x27
    // vl128 state = 0x37d83129
    __ dci(0x253f10d5);  // whilegt p5.b, x6, xzr
    // vl128 state = 0x8e121615
    __ dci(0x252f11d4);  // whilegt p4.b, x14, x15
    // vl128 state = 0x83d6c9e9
    __ dci(0x25af01d5);  // whilegt p5.s, w14, w15
    // vl128 state = 0xe865fad7
    __ dci(0x25eb01c5);  // whilege p5.d, w14, w11
    // vl128 state = 0x5eaf208e
    __ dci(0x25fb0144);  // whilege p4.d, w10, w27
    // vl128 state = 0x8cd6348c
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x8cd6348c,
        0x42a1f9b4,
        0x13fc2001,
        0x492cb2ac,
        0xa67cfb65,
        0x80d4639f,
        0xfa388a09,
        0x8c7ad8d9,
        0x299c5bfe,
        0x9183808a,
        0x3fc14d86,
        0x7cc08a05,
        0x9c85cd48,
        0xd06e8299,
        0x6a107152,
        0x81d99d7c,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_cdot_index) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x44bb4ef6);  // cdot z22.s, z23.b, z3.b[3], #270
    // vl128 state = 0x452d1d6e
    __ dci(0x44b94ff7);  // cdot z23.s, z31.b, z1.b[3], #270
    // vl128 state = 0x546c9569
    __ dci(0x44b94dd5);  // cdot z21.s, z14.b, z1.b[3], #270
    // vl128 state = 0xa2abf834
    __ dci(0x44bd45d7);  // cdot z23.s, z14.b, z5.b[3], #90
    // vl128 state = 0xba77ed64
    __ dci(0x44fc45df);  // cdot z31.d, z14.h, z12.h[1], #90
    // vl128 state = 0xe78163f2
    __ dci(0x44f441db);  // cdot z27.d, z14.h, z4.h[1], #0
    // vl128 state = 0xca3b116d
    __ dci(0x44f44dd3);  // cdot z19.d, z14.h, z4.h[1], #270
    // vl128 state = 0x57ba3771
    __ dci(0x44b44d83);  // cdot z3.s, z12.b, z4.b[2], #270
    // vl128 state = 0x4edccb88
    __ dci(0x44ac4d82);  // cdot z2.s, z12.b, z4.b[1], #270
    // vl128 state = 0xc9543499
    __ dci(0x44a84f8a);  // cdot z10.s, z28.b, z0.b[1], #270
    // vl128 state = 0x9d8fe439
    __ dci(0x44a84d08);  // cdot z8.s, z8.b, z0.b[1], #270
    // vl128 state = 0x3c1bf0cc
    __ dci(0x44ba4d09);  // cdot z9.s, z8.b, z2.b[3], #270
    // vl128 state = 0x983716f1
    __ dci(0x44ea4d0d);  // cdot z13.d, z8.h, z10.h[0], #270
    // vl128 state = 0x2df96300
    __ dci(0x44eb491d);  // cdot z29.d, z8.h, z11.h[0], #180
    // vl128 state = 0xc23edde3
    __ dci(0x44e9499f);  // cdot z31.d, z12.h, z9.h[0], #180
    // vl128 state = 0xef0ace9d
    __ dci(0x44e84b9d);  // cdot z29.d, z28.h, z8.h[0], #180
    // vl128 state = 0x2cce8002
    __ dci(0x44e84b99);  // cdot z25.d, z28.h, z8.h[0], #180
    // vl128 state = 0xd07f46a1
    __ dci(0x44f84a9d);  // cdot z29.d, z20.h, z8.h[1], #180
    // vl128 state = 0x239831e8
    __ dci(0x44f84a99);  // cdot z25.d, z20.h, z8.h[1], #180
    // vl128 state = 0xa110988d
    __ dci(0x44e84a09);  // cdot z9.d, z16.h, z8.h[0], #180
    // vl128 state = 0x2b9ef292
    __ dci(0x44e84a19);  // cdot z25.d, z16.h, z8.h[0], #180
    // vl128 state = 0x50eeb818
    __ dci(0x44e04b1b);  // cdot z27.d, z24.h, z0.h[0], #180
    // vl128 state = 0xc33ce03b
    __ dci(0x44e04a2b);  // cdot z11.d, z17.h, z0.h[0], #180
    // vl128 state = 0xe163b5c9
    __ dci(0x44e04b0f);  // cdot z15.d, z24.h, z0.h[0], #180
    // vl128 state = 0x052a34eb
    __ dci(0x44e04b1f);  // cdot z31.d, z24.h, z0.h[0], #180
    // vl128 state = 0x0660afb4
    __ dci(0x44e84b4f);  // cdot z15.d, z26.h, z8.h[0], #180
    // vl128 state = 0x0ae01233
    __ dci(0x44ee4b4e);  // cdot z14.d, z26.h, z14.h[0], #180
    // vl128 state = 0xde7bdd15
    __ dci(0x44ae4b7e);  // cdot z30.s, z27.b, z6.b[1], #180
    // vl128 state = 0x758973a1
    __ dci(0x44a6497f);  // cdot z31.s, z11.b, z6.b[0], #180
    // vl128 state = 0xb3c5df37
    __ dci(0x44a64df7);  // cdot z23.s, z15.b, z6.b[0], #270
    // vl128 state = 0xe652f054
    __ dci(0x44a64c73);  // cdot z19.s, z3.b, z6.b[0], #270
    // vl128 state = 0xc4b58041
    __ dci(0x44a64de3);  // cdot z3.s, z15.b, z6.b[0], #270
    // vl128 state = 0x1239ca90
    __ dci(0x44a749e2);  // cdot z2.s, z15.b, z7.b[0], #180
    // vl128 state = 0x4a01cdcb
    __ dci(0x44a740e0);  // cdot z0.s, z7.b, z7.b[0], #0
    // vl128 state = 0x604e45cf
    __ dci(0x44a344e2);  // cdot z2.s, z7.b, z3.b[0], #90
    // vl128 state = 0x12fe2972
    __ dci(0x44a34ca3);  // cdot z3.s, z5.b, z3.b[0], #270
    // vl128 state = 0x78e0bb2e
    __ dci(0x44e14cb3);  // cdot z19.d, z5.h, z1.h[0], #270
    // vl128 state = 0xe3a69b46
    __ dci(0x44e14d31);  // cdot z17.d, z9.h, z1.h[0], #270
    // vl128 state = 0xe6b58aa4
    __ dci(0x44f14d01);  // cdot z1.d, z8.h, z1.h[1], #270
    // vl128 state = 0xffcfb597
    __ dci(0x44f14551);  // cdot z17.d, z10.h, z1.h[1], #90
    // vl128 state = 0x2745934b
    __ dci(0x44f345d5);  // cdot z21.d, z14.h, z3.h[1], #90
    // vl128 state = 0xa38b5571
    __ dci(0x44f34574);  // cdot z20.d, z11.h, z3.h[1], #90
    // vl128 state = 0x978afd92
    __ dci(0x44f34576);  // cdot z22.d, z11.h, z3.h[1], #90
    // vl128 state = 0x9f1b19c9
    __ dci(0x44f34f77);  // cdot z23.d, z27.h, z3.h[1], #270
    // vl128 state = 0x61a31d64
    __ dci(0x44f24f5f);  // cdot z31.d, z26.h, z2.h[1], #270
    // vl128 state = 0x1e71023e
    __ dci(0x44fa4fcf);  // cdot z15.d, z30.h, z10.h[1], #270
    // vl128 state = 0xdbe5ffb3
    __ dci(0x44ba4f4e);  // cdot z14.s, z26.b, z2.b[3], #270
    // vl128 state = 0x51390e81
    __ dci(0x44ba470c);  // cdot z12.s, z24.b, z2.b[3], #90
    // vl128 state = 0x59ad5198
    __ dci(0x44b2479c);  // cdot z28.s, z28.b, z2.b[2], #90
    // vl128 state = 0xe997de49
    __ dci(0x44b24fbd);  // cdot z29.s, z29.b, z2.b[2], #270
    // vl128 state = 0x5533cefa
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x5533cefa,
        0x1462a298,
        0x1acb4ead,
        0xeb05ddf0,
        0x23fe8c86,
        0xbb1e9f8c,
        0x4a933f43,
        0x4cd64b55,
        0x84a4b8b7,
        0x52019619,
        0x4442432b,
        0x9b353ce8,
        0x333c9eef,
        0x291eac87,
        0x110f7371,
        0x009b25cb,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_splice) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x05ed89a7);  // splice z7.d, p2, {z13.d, z14.d}
    // vl128 state = 0x6acff994
    __ dci(0x05ed81e5);  // splice z5.d, p0, {z15.d, z16.d}
    // vl128 state = 0x2c8b3e5d
    __ dci(0x05ed8375);  // splice z21.d, p0, {z27.d, z28.d}
    // vl128 state = 0x2588e208
    __ dci(0x05ed9174);  // splice z20.d, p4, {z11.d, z12.d}
    // vl128 state = 0x4d6fa6b3
    __ dci(0x056d91f6);  // splice z22.h, p4, {z15.h, z16.h}
    // vl128 state = 0x9f00a308
    __ dci(0x056d92f2);  // splice z18.h, p4, {z23.h, z24.h}
    // vl128 state = 0x5479cc74
    __ dci(0x056d96a2);  // splice z2.h, p5, {z21.h, z22.h}
    // vl128 state = 0xca7a6a63
    __ dci(0x056d9fa6);  // splice z6.h, p7, {z29.h, z30.h}
    // vl128 state = 0x007fc934
    __ dci(0x056d9be4);  // splice z4.h, p6, {z31.h, z0.h}
    // vl128 state = 0x8186741b
    __ dci(0x056d97ec);  // splice z12.h, p5, {z31.h, z0.h}
    // vl128 state = 0x26ab76b9
    __ dci(0x056d979c);  // splice z28.h, p5, {z28.h, z29.h}
    // vl128 state = 0x933201f4
    __ dci(0x056d9794);  // splice z20.h, p5, {z28.h, z29.h}
    // vl128 state = 0x42cf6784
    __ dci(0x052d9f96);  // splice z22.b, p7, {z28.b, z29.b}
    // vl128 state = 0x0838e776
    __ dci(0x056d8f9e);  // splice z30.h, p3, {z28.h, z29.h}
    // vl128 state = 0x89637e78
    __ dci(0x056d9fd6);  // splice z22.h, p7, {z30.h, z31.h}
    // vl128 state = 0xb94dbb49
    __ dci(0x056d8dd7);  // splice z23.h, p3, {z14.h, z15.h}
    // vl128 state = 0x260f8127
    __ dci(0x05ad8ddf);  // splice z31.s, p3, {z14.s, z15.s}
    // vl128 state = 0x16257a12
    __ dci(0x05ad8ddd);  // splice z29.s, p3, {z14.s, z15.s}
    // vl128 state = 0x803d0766
    __ dci(0x05ad8d7c);  // splice z28.s, p3, {z11.s, z12.s}
    // vl128 state = 0xcc405331
    __ dci(0x05ad8d74);  // splice z20.s, p3, {z11.s, z12.s}
    // vl128 state = 0x0ed25e4c
    __ dci(0x05ad8d64);  // splice z4.s, p3, {z11.s, z12.s}
    // vl128 state = 0x167daf8b
    __ dci(0x05ed8c6c);  // splice z12.d, p3, {z3.d, z4.d}
    // vl128 state = 0x435f3bb9
    __ dci(0x05ed8cad);  // splice z13.d, p3, {z5.d, z6.d}
    // vl128 state = 0xe49df619
    __ dci(0x056d8dbd);  // splice z29.h, p3, {z13.h, z14.h}
    // vl128 state = 0x1f54e928
    __ dci(0x056d8f2d);  // splice z13.h, p3, {z25.h, z26.h}
    // vl128 state = 0x24adbe77
    __ dci(0x056d8f9d);  // splice z29.h, p3, {z28.h, z29.h}
    // vl128 state = 0xcc2ec3e6
    __ dci(0x056d8f95);  // splice z21.h, p3, {z28.h, z29.h}
    // vl128 state = 0xb71c64f7
    __ dci(0x056d8f34);  // splice z20.h, p3, {z25.h, z26.h}
    // vl128 state = 0xb32756f0
    __ dci(0x05ed8f64);  // splice z4.d, p3, {z27.d, z28.d}
    // vl128 state = 0x3f7d1f13
    __ dci(0x05ad8e60);  // splice z0.s, p3, {z19.s, z20.s}
    // vl128 state = 0x9a7ffbde
    __ dci(0x052d8e50);  // splice z16.b, p3, {z18.b, z19.b}
    // vl128 state = 0x5c82ed17
    __ dci(0x052d9652);  // splice z18.b, p5, {z18.b, z19.b}
    // vl128 state = 0x28b9cd60
    __ dci(0x052d9ed0);  // splice z16.b, p7, {z22.b, z23.b}
    // vl128 state = 0xab0238ba
    __ dci(0x052d9ed4);  // splice z20.b, p7, {z22.b, z23.b}
    // vl128 state = 0x9f0e0ef9
    __ dci(0x056d9cc4);  // splice z4.h, p7, {z6.h, z7.h}
    // vl128 state = 0xec31d5e7
    __ dci(0x056d98e6);  // splice z6.h, p6, {z7.h, z8.h}
    // vl128 state = 0xbc9c0048
    __ dci(0x056d9ee4);  // splice z4.h, p7, {z23.h, z24.h}
    // vl128 state = 0xe2e9c9a3
    __ dci(0x056d9ef4);  // splice z20.h, p7, {z23.h, z24.h}
    // vl128 state = 0x60ffa98a
    __ dci(0x056d9ab6);  // splice z22.h, p6, {z21.h, z22.h}
    // vl128 state = 0xae70ed0f
    __ dci(0x056d9294);  // splice z20.h, p4, {z20.h, z21.h}
    // vl128 state = 0x5736c563
    __ dci(0x056d9284);  // splice z4.h, p4, {z20.h, z21.h}
    // vl128 state = 0xf31dd2d9
    __ dci(0x052d920c);  // splice z12.b, p4, {z16.b, z17.b}
    // vl128 state = 0x04502fea
    __ dci(0x052d921c);  // splice z28.b, p4, {z16.b, z17.b}
    // vl128 state = 0x852f98b1
    __ dci(0x052d9094);  // splice z20.b, p4, {z4.b, z5.b}
    // vl128 state = 0xb40c5931
    __ dci(0x052d90f6);  // splice z22.b, p4, {z7.b, z8.b}
    // vl128 state = 0x64d6138d
    __ dci(0x052d88e6);  // splice z6.b, p2, {z7.b, z8.b}
    // vl128 state = 0x51bb6564
    __ dci(0x052d88e4);  // splice z4.b, p2, {z7.b, z8.b}
    // vl128 state = 0x7ed599b0
    __ dci(0x05ad8865);  // splice z5.s, p2, {z3.s, z4.s}
    // vl128 state = 0xa201547d
    __ dci(0x05ad9961);  // splice z1.s, p6, {z11.s, z12.s}
    // vl128 state = 0x9508f19c
    __ dci(0x05ed9945);  // splice z5.d, p6, {z10.d, z11.d}
    // vl128 state = 0x95399cfd
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x95399cfd,
        0xa960b01e,
        0x1fedaa18,
        0xe2fd3ec3,
        0x3edc353b,
        0xd809efd8,
        0x2a04f527,
        0xe4b9bb4a,
        0x72e5ed3e,
        0x63d6fe93,
        0xd2ad18fa,
        0x522fe057,
        0xc7ba2f7d,
        0x2dd44bd3,
        0x68b62ae6,
        0x06ea6854,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_whilerw_whilewr) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x25ac3026);  // whilewr p6.s, x1, x12
    // vl128 state = 0x91e301ae
    __ dci(0x25ac3024);  // whilewr p4.s, x1, x12
    // vl128 state = 0x9203b261
    __ dci(0x25af3020);  // whilewr p0.s, x1, x15
    // vl128 state = 0x87505080
    __ dci(0x25ef3222);  // whilewr p2.d, x17, x15
    // vl128 state = 0x4ba695cb
    __ dci(0x25eb320a);  // whilewr p10.d, x16, x11
    // vl128 state = 0x5909d726
    __ dci(0x25e33308);  // whilewr p8.d, x24, x3
    // vl128 state = 0x52766071
    __ dci(0x25ea3309);  // whilewr p9.d, x24, x10
    // vl128 state = 0xe906a65a
    __ dci(0x25aa3101);  // whilewr p1.s, x8, x10
    // vl128 state = 0xd9d56c58
    __ dci(0x252b3100);  // whilewr p0.b, x8, x11
    // vl128 state = 0xcc868eb9
    __ dci(0x252a3008);  // whilewr p8.b, x0, x10
    // vl128 state = 0xf78cb912
    __ dci(0x2528304c);  // whilewr p12.b, x2, x8
    // vl128 state = 0x5493a6c4
    __ dci(0x25203004);  // whilewr p4.b, x0, x0
    // vl128 state = 0xb3d754b6
    __ dci(0x25303105);  // whilewr p5.b, x8, x16
    // vl128 state = 0x7fc526df
    __ dci(0x25b4310d);  // whilewr p13.s, x8, x20
    // vl128 state = 0x5999edda
    __ dci(0x25ac310c);  // whilewr p12.s, x8, x12
    // vl128 state = 0x46a86248
    __ dci(0x25ac310e);  // whilewr p14.s, x8, x12
    // vl128 state = 0x0dc5ed70
    __ dci(0x252c330a);  // whilewr p10.b, x24, x12
    // vl128 state = 0x453a1aa9
    __ dci(0x252f330b);  // whilewr p11.b, x24, x15
    // vl128 state = 0x98fbdcdf
    __ dci(0x256e330f);  // whilewr p15.h, x24, x14
    // vl128 state = 0x84699750
    __ dci(0x252e334d);  // whilewr p13.b, x26, x14
    // vl128 state = 0x198ea519
    __ dci(0x252e3349);  // whilewr p9.b, x26, x14
    // vl128 state = 0xb4956673
    __ dci(0x253e33c1);  // whilewr p1.b, x30, x30
    // vl128 state = 0xfd88dd74
    __ dci(0x252e33e3);  // whilewr p3.b, xzr, x14
    // vl128 state = 0x68cda9df
    __ dci(0x25ae33cb);  // whilewr p11.s, x30, x14
    // vl128 state = 0x9104f644
    __ dci(0x25ae33ca);  // whilewr p10.s, x30, x14
    // vl128 state = 0xd9079300
    __ dci(0x25ea33da);  // whilerw p10.d, x30, x10
    // vl128 state = 0xd9fb019d
    __ dci(0x25ae33d8);  // whilerw p8.s, x30, x14
    // vl128 state = 0x9edf46fa
    __ dci(0x25ae32f9);  // whilerw p9.s, x23, x14
    // vl128 state = 0x3b10562f
    __ dci(0x25ee32d8);  // whilerw p8.d, x22, x14
    // vl128 state = 0x473e26e3
    __ dci(0x25ec3299);  // whilerw p9.d, x20, x12
    // vl128 state = 0x4feaf55c
    __ dci(0x25ec329d);  // whilerw p13.d, x20, x12
    // vl128 state = 0x9f9a203a
    __ dci(0x25e8321c);  // whilerw p12.d, x16, x8
    // vl128 state = 0xd8f32d11
    __ dci(0x2568301d);  // whilerw p13.h, x0, x8
    // vl128 state = 0xf04b6bb8
    __ dci(0x2528320d);  // whilewr p13.b, x16, x8
    // vl128 state = 0x0883f877
    __ dci(0x25a8323d);  // whilerw p13.s, x17, x8
    // vl128 state = 0x9564ca3e
    __ dci(0x25a8323f);  // whilerw p15.s, x17, x8
    // vl128 state = 0xa50cf036
    __ dci(0x25e8303d);  // whilerw p13.d, x1, x8
    // vl128 state = 0xe89b1719
    __ dci(0x25e83175);  // whilerw p5.d, x11, x8
    // vl128 state = 0xe79bea7c
    __ dci(0x256a3174);  // whilerw p4.h, x11, x10
    // vl128 state = 0xc8ca3b74
    __ dci(0x256a317c);  // whilerw p12.h, x11, x10
    // vl128 state = 0xc3c88548
    __ dci(0x256a33f8);  // whilerw p8.h, xzr, x10
    // vl128 state = 0x8b25acc6
    __ dci(0x256a33f0);  // whilerw p0.h, xzr, x10
    // vl128 state = 0x904c0fd1
    __ dci(0x25e833e0);  // whilewr p0.d, xzr, x8
    // vl128 state = 0xc893f4c8
    __ dci(0x25ec32e8);  // whilewr p8.d, x23, x12
    // vl128 state = 0x807edd46
    __ dci(0x25ed326c);  // whilewr p12.d, x19, x13
    // vl128 state = 0x8b7c637a
    __ dci(0x256d32ed);  // whilewr p13.h, x23, x13
    // vl128 state = 0xa3c425d3
    __ dci(0x252d30e9);  // whilewr p9.b, x7, x13
    // vl128 state = 0x0edfe6b9
    __ dci(0x252531eb);  // whilewr p11.b, x15, x5
    // vl128 state = 0xf716b922
    __ dci(0x252733ef);  // whilewr p15.b, xzr, x7
    // vl128 state = 0xbf9aea3e
    __ dci(0x25253367);  // whilewr p7.b, x27, x5
    // vl128 state = 0x357fc408
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x357fc408,
        0x8d6fc283,
        0x5f73c1df,
        0x2963d995,
        0x80713760,
        0x4638fc82,
        0x23955ead,
        0x52e4c002,
        0xd56ab65c,
        0x0e5bb2f2,
        0x8c78ec14,
        0xd9b634d2,
        0x83adc3a2,
        0x3b664eea,
        0x3d1f5422,
        0x7cdcd310,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_mul_index) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x4468fb6e);  // mul z14.h, z27.h, z0.h[5]
    // vl128 state = 0xcbe81b96
    __ dci(0x4468f93e);  // mul z30.h, z9.h, z0.h[5]
    // vl128 state = 0x8a75362d
    __ dci(0x4428f976);  // mul z22.h, z11.h, z0.h[1]
    // vl128 state = 0x1e3c5184
    __ dci(0x4428fa77);  // mul z23.h, z19.h, z0.h[1]
    // vl128 state = 0x173f58b5
    __ dci(0x4429fb67);  // mul z7.h, z27.h, z1.h[1]
    // vl128 state = 0x15686c87
    __ dci(0x4429fb63);  // mul z3.h, z27.h, z1.h[1]
    // vl128 state = 0x41068a87
    __ dci(0x4428fb53);  // mul z19.h, z26.h, z0.h[1]
    // vl128 state = 0xcfd6e02c
    __ dci(0x4429fbd1);  // mul z17.h, z30.h, z1.h[1]
    // vl128 state = 0xfd3e0e3c
    __ dci(0x442afbd9);  // mul z25.h, z30.h, z2.h[1]
    // vl128 state = 0x1e660bf7
    __ dci(0x442afa5b);  // mul z27.h, z18.h, z2.h[1]
    // vl128 state = 0xb5378f4e
    __ dci(0x44abfa4b);  // mul z11.s, z18.s, z3.s[1]
    // vl128 state = 0xf34416fe
    __ dci(0x44abfa4f);  // mul z15.s, z18.s, z3.s[1]
    // vl128 state = 0xc80d6ad9
    __ dci(0x44a9f84e);  // mul z14.s, z2.s, z1.s[1]
    // vl128 state = 0xa4fe2be7
    __ dci(0x44e9fa46);  // mul z6.d, z18.d, z9.d[0]
    // vl128 state = 0xaf461ebb
    __ dci(0x44e9fa8e);  // mul z14.d, z20.d, z9.d[0]
    // vl128 state = 0x9f7acd20
    __ dci(0x44f1fa8f);  // mul z15.d, z20.d, z1.d[1]
    // vl128 state = 0x1b710469
    __ dci(0x4471fa07);  // mul z7.h, z16.h, z1.h[6]
    // vl128 state = 0xa2120b4c
    __ dci(0x4470fa43);  // mul z3.h, z18.h, z0.h[6]
    // vl128 state = 0xb6d6ce4c
    __ dci(0x4474fb47);  // mul z7.h, z26.h, z4.h[6]
    // vl128 state = 0xeec634bf
    __ dci(0x4476fa57);  // mul z23.h, z18.h, z6.h[6]
    // vl128 state = 0x893bbe37
    __ dci(0x447cfa53);  // mul z19.h, z18.h, z4.h[7]
    // vl128 state = 0x8373940b
    __ dci(0x447dfb52);  // mul z18.h, z26.h, z5.h[7]
    // vl128 state = 0xd1c86434
    __ dci(0x4477fb56);  // mul z22.h, z26.h, z7.h[6]
    // vl128 state = 0xb247cf9e
    __ dci(0x4476fb77);  // mul z23.h, z27.h, z6.h[6]
    // vl128 state = 0x6106a868
    __ dci(0x4467fb7f);  // mul z31.h, z27.h, z7.h[4]
    // vl128 state = 0xc0a11edf
    __ dci(0x446ffa77);  // mul z23.h, z19.h, z7.h[5]
    // vl128 state = 0xe1879a44
    __ dci(0x442bfa76);  // mul z22.h, z19.h, z3.h[1]
    // vl128 state = 0xc773115b
    __ dci(0x442bfa7e);  // mul z30.h, z19.h, z3.h[1]
    // vl128 state = 0x5f5b4793
    __ dci(0x442afa2e);  // mul z14.h, z17.h, z2.h[1]
    // vl128 state = 0x144b30b2
    __ dci(0x442afa26);  // mul z6.h, z17.h, z2.h[1]
    // vl128 state = 0x905f8608
    __ dci(0x442afb6e);  // mul z14.h, z27.h, z2.h[1]
    // vl128 state = 0x0f826c19
    __ dci(0x44aefb66);  // mul z6.s, z27.s, z6.s[1]
    // vl128 state = 0x7043c090
    __ dci(0x44aefba4);  // mul z4.s, z29.s, z6.s[1]
    // vl128 state = 0xab3921a9
    __ dci(0x44aefbb4);  // mul z20.s, z29.s, z6.s[1]
    // vl128 state = 0x7d420495
    __ dci(0x44acfbf0);  // mul z16.s, z31.s, z4.s[1]
    // vl128 state = 0xceb17a45
    __ dci(0x44a4fb60);  // mul z0.s, z27.s, z4.s[0]
    // vl128 state = 0x97ed0929
    __ dci(0x44a5fb30);  // mul z16.s, z25.s, z5.s[0]
    // vl128 state = 0xb7fa54a5
    __ dci(0x4425f938);  // mul z24.h, z9.h, z5.h[0]
    // vl128 state = 0xfcc1c192
    __ dci(0x442df830);  // mul z16.h, z1.h, z5.h[1]
    // vl128 state = 0x933ed51d
    __ dci(0x4427f832);  // mul z18.h, z1.h, z7.h[0]
    // vl128 state = 0x2129d4f0
    __ dci(0x442ef822);  // mul z2.h, z1.h, z6.h[1]
    // vl128 state = 0x76f6854c
    __ dci(0x442af803);  // mul z3.h, z0.h, z2.h[1]
    // vl128 state = 0xe763df2d
    __ dci(0x442af801);  // mul z1.h, z0.h, z2.h[1]
    // vl128 state = 0x61db5a87
    __ dci(0x442bf900);  // mul z0.h, z8.h, z3.h[1]
    // vl128 state = 0x90883cfb
    __ dci(0x442bf881);  // mul z1.h, z4.h, z3.h[1]
    // vl128 state = 0xb4afb9b2
    __ dci(0x4427f885);  // mul z5.h, z4.h, z7.h[0]
    // vl128 state = 0xe512adca
    __ dci(0x4425f8ad);  // mul z13.h, z5.h, z5.h[0]
    // vl128 state = 0xd820475a
    __ dci(0x4420f8a5);  // mul z5.h, z5.h, z0.h[0]
    // vl128 state = 0xea9a6f50
    __ dci(0x4431f8a4);  // mul z4.h, z5.h, z1.h[2]
    // vl128 state = 0x9343e341
    __ dci(0x4425f8a0);  // mul z0.h, z5.h, z5.h[0]
    // vl128 state = 0x20a5f202
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x20a5f202,
        0xdb7b10ee,
        0x0607441b,
        0x4966f0ff,
        0x5f750338,
        0x9be09ff4,
        0x8805a320,
        0x52cf70b0,
        0x5f4c6d92,
        0xf8009f1f,
        0x56cd1ff6,
        0x345f063d,
        0x3807ccf3,
        0xf7eb85a8,
        0x1600c143,
        0x97be6c01,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_mla_mls_index) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x44200800);  // mla z0.h, z0.h, z0.h[0]
    // vl128 state = 0x06aac22e
    __ dci(0x44200a28);  // mla z8.h, z17.h, z0.h[0]
    // vl128 state = 0xde2255a4
    __ dci(0x44e00a2a);  // mla z10.d, z17.d, z0.d[0]
    // vl128 state = 0x9bf1bae6
    __ dci(0x44600e3a);  // mls z26.h, z17.h, z0.h[4]
    // vl128 state = 0x28b58feb
    __ dci(0x44e20e2a);  // mls z10.d, z17.d, z2.d[0]
    // vl128 state = 0x0ac8fcc8
    __ dci(0x44620f2e);  // mls z14.h, z25.h, z2.h[4]
    // vl128 state = 0x955da860
    __ dci(0x44630f6a);  // mls z10.h, z27.h, z3.h[4]
    // vl128 state = 0x654ee915
    __ dci(0x44730b6e);  // mla z14.h, z27.h, z3.h[6]
    // vl128 state = 0x3fd3e02c
    __ dci(0x44720f6f);  // mls z15.h, z27.h, z2.h[6]
    // vl128 state = 0x46031098
    __ dci(0x44620f4b);  // mls z11.h, z26.h, z2.h[4]
    // vl128 state = 0xd49183cf
    __ dci(0x446a0b5b);  // mla z27.h, z26.h, z2.h[5]
    // vl128 state = 0x4fe290c1
    __ dci(0x44680b73);  // mla z19.h, z27.h, z0.h[5]
    // vl128 state = 0xf6fccd86
    __ dci(0x44e90b77);  // mla z23.d, z27.d, z9.d[0]
    // vl128 state = 0x57b2090d
    __ dci(0x44f10b76);  // mla z22.d, z27.d, z1.d[1]
    // vl128 state = 0x5a6932eb
    __ dci(0x44f40b77);  // mla z23.d, z27.d, z4.d[1]
    // vl128 state = 0x8e33d7d5
    __ dci(0x44640b7f);  // mla z31.h, z27.h, z4.h[4]
    // vl128 state = 0xaa01885d
    __ dci(0x44640b7d);  // mla z29.h, z27.h, z4.h[4]
    // vl128 state = 0x2ef00e60
    __ dci(0x44640b7f);  // mla z31.h, z27.h, z4.h[4]
    // vl128 state = 0x94ac10d3
    __ dci(0x44340b7e);  // mla z30.h, z27.h, z4.h[2]
    // vl128 state = 0x48211118
    __ dci(0x44340e7a);  // mls z26.h, z19.h, z4.h[2]
    // vl128 state = 0x72cc2767
    __ dci(0x44b40eea);  // mls z10.s, z23.s, z4.s[2]
    // vl128 state = 0x3855f70f
    __ dci(0x44e40ee2);  // mls z2.d, z23.d, z4.d[0]
    // vl128 state = 0xf9225160
    __ dci(0x44ec0ea3);  // mls z3.d, z21.d, z12.d[0]
    // vl128 state = 0xf9b94fd0
    __ dci(0x44ae0ea7);  // mls z7.s, z21.s, z6.s[1]
    // vl128 state = 0x06070917
    __ dci(0x44ae0eb7);  // mls z23.s, z21.s, z6.s[1]
    // vl128 state = 0x26ecdd18
    __ dci(0x44ae0e07);  // mls z7.s, z16.s, z6.s[1]
    // vl128 state = 0xaa8e3a32
    __ dci(0x44ae0a85);  // mla z5.s, z20.s, z6.s[1]
    // vl128 state = 0x2379cba0
    __ dci(0x44ae0a81);  // mla z1.s, z20.s, z6.s[1]
    // vl128 state = 0x3cc8a61c
    __ dci(0x442a0a85);  // mla z5.h, z20.h, z2.h[1]
    // vl128 state = 0x96f118ef
    __ dci(0x443e0a84);  // mla z4.h, z20.h, z6.h[3]
    // vl128 state = 0xa3f8cb41
    __ dci(0x443f0b8c);  // mla z12.h, z28.h, z7.h[3]
    // vl128 state = 0x97fcb1da
    __ dci(0x442f0bbc);  // mla z28.h, z29.h, z7.h[1]
    // vl128 state = 0x761e9499
    __ dci(0x44270fac);  // mls z12.h, z29.h, z7.h[0]
    // vl128 state = 0xfb28f943
    __ dci(0x442f0ead);  // mls z13.h, z21.h, z7.h[1]
    // vl128 state = 0x387a2623
    __ dci(0x44270fa9);  // mls z9.h, z29.h, z7.h[0]
    // vl128 state = 0x22f03847
    __ dci(0x44270f68);  // mls z8.h, z27.h, z7.h[0]
    // vl128 state = 0xada4998b
    __ dci(0x44270f6c);  // mls z12.h, z27.h, z7.h[0]
    // vl128 state = 0xdf80a034
    __ dci(0x44270f7c);  // mls z28.h, z27.h, z7.h[0]
    // vl128 state = 0x3ccddaa6
    __ dci(0x44250f2c);  // mls z12.h, z25.h, z5.h[0]
    // vl128 state = 0x588502cb
    __ dci(0x442f0f28);  // mls z8.h, z25.h, z7.h[1]
    // vl128 state = 0x79c90307
    __ dci(0x446f0d2c);  // mls z12.h, z9.h, z7.h[5]
    // vl128 state = 0xaa0b21a9
    __ dci(0x44af0d2e);  // mls z14.s, z9.s, z7.s[1]
    // vl128 state = 0xd5ccc60c
    __ dci(0x44ed0d26);  // mls z6.d, z9.d, z13.d[0]
    // vl128 state = 0x15037cbe
    __ dci(0x44fd0f2e);  // mls z14.d, z25.d, z13.d[1]
    // vl128 state = 0x9f481fdf
    __ dci(0x44f90e2f);  // mls z15.d, z17.d, z9.d[1]
    // vl128 state = 0x93fe8537
    __ dci(0x447d0e3f);  // mls z31.h, z17.h, z5.h[7]
    // vl128 state = 0x14b9edf2
    __ dci(0x44f90e2f);  // mls z15.d, z17.d, z9.d[1]
    // vl128 state = 0xde1c0d1c
    __ dci(0x44790c27);  // mls z7.h, z1.h, z1.h[7]
    // vl128 state = 0x563d614a
    __ dci(0x44790c23);  // mls z3.h, z1.h, z1.h[7]
    // vl128 state = 0x8c6d9baf
    __ dci(0x44f90c6b);  // mls z11.d, z3.d, z9.d[1]
    // vl128 state = 0x1a25c073
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x1a25c073,
        0xfbb2c945,
        0x932b8ab7,
        0x99370bee,
        0x44a15f80,
        0xae898f1d,
        0x97382827,
        0xafec059e,
        0xf11bc007,
        0x34c49b30,
        0x73b95606,
        0x77324772,
        0x9ad7d21b,
        0x0d0958a7,
        0xee4accc3,
        0x31d34df8,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_mla_long) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x44935abe);  // umlslb z30.s, z21.h, z19.h
    // vl128 state = 0x4fac8e49
    __ dci(0x449358fa);  // umlslb z26.s, z7.h, z19.h
    // vl128 state = 0xca971f04
    __ dci(0x44935adb);  // umlslb z27.s, z22.h, z19.h
    // vl128 state = 0x5652564b
    __ dci(0x449359da);  // umlslb z26.s, z14.h, z19.h
    // vl128 state = 0xf2d81244
    __ dci(0x448349de);  // umlalb z30.s, z14.h, z3.h
    // vl128 state = 0x7cbaa548
    __ dci(0x448349d6);  // umlalb z22.s, z14.h, z3.h
    // vl128 state = 0x9e7b4915
    __ dci(0x44c34952);  // umlalb z18.d, z10.s, z3.s
    // vl128 state = 0x550af70e
    __ dci(0x44d349d3);  // umlalb z19.d, z14.s, z19.s
    // vl128 state = 0x676743b2
    __ dci(0x44d549d7);  // umlalb z23.d, z14.s, z21.s
    // vl128 state = 0x602e09e4
    __ dci(0x44d55ddf);  // umlslt z31.d, z14.s, z21.s
    // vl128 state = 0xd4c245de
    __ dci(0x44d55d1b);  // umlslt z27.d, z8.s, z21.s
    // vl128 state = 0x9c2c1cb4
    __ dci(0x44d5490b);  // umlalb z11.d, z8.s, z21.s
    // vl128 state = 0x8a702002
    __ dci(0x44554d0a);  // umlalt z10.h, z8.b, z21.b
    // vl128 state = 0x6758ce3c
    __ dci(0x4455452b);  // smlalt z11.h, z9.b, z21.b
    // vl128 state = 0x967e596e
    __ dci(0x44554529);  // smlalt z9.h, z9.b, z21.b
    // vl128 state = 0x1300909a
    __ dci(0x44474521);  // smlalt z1.h, z9.b, z7.b
    // vl128 state = 0x01ca26c1
    __ dci(0x44c74d25);  // umlalt z5.d, z9.s, z7.s
    // vl128 state = 0x8e6313b9
    __ dci(0x44cb4d24);  // umlalt z4.d, z9.s, z11.s
    // vl128 state = 0xdb41e004
    __ dci(0x44cb4d2c);  // umlalt z12.d, z9.s, z11.s
    // vl128 state = 0x941401ca
    __ dci(0x44c94da8);  // umlalt z8.d, z13.s, z9.s
    // vl128 state = 0x8a57334b
    __ dci(0x44594db8);  // umlalt z24.h, z13.b, z25.b
    // vl128 state = 0x94333fae
    __ dci(0x44585db0);  // umlslt z16.h, z13.b, z24.b
    // vl128 state = 0xf4fbe251
    __ dci(0x44585f80);  // umlslt z0.h, z28.b, z24.b
    // vl128 state = 0x1f5aeef3
    __ dci(0x445a5fc2);  // umlslt z2.h, z30.b, z26.b
    // vl128 state = 0x4b153d20
    __ dci(0x445a5fd2);  // umlslt z18.h, z30.b, z26.b
    // vl128 state = 0xbd82f0a2
    __ dci(0x445a5fd3);  // umlslt z19.h, z30.b, z26.b
    // vl128 state = 0x72d7083d
    __ dci(0x44525bd2);  // umlslb z18.h, z30.b, z18.b
    // vl128 state = 0x5018a138
    __ dci(0x44525bd6);  // umlslb z22.h, z30.b, z18.b
    // vl128 state = 0xcaf48a01
    __ dci(0x445053d2);  // smlslb z18.h, z30.b, z16.b
    // vl128 state = 0x76e2d850
    __ dci(0x44d153c2);  // smlslb z2.d, z30.s, z17.s
    // vl128 state = 0x8594d6c9
    __ dci(0x449353c3);  // smlslb z3.s, z30.h, z19.h
    // vl128 state = 0x8e0da89d
    __ dci(0x449152c7);  // smlslb z7.s, z22.h, z17.h
    // vl128 state = 0xe7d08864
    __ dci(0x44995285);  // smlslb z5.s, z20.h, z25.h
    // vl128 state = 0xd7c49fca
    __ dci(0x449953c1);  // smlslb z1.s, z30.h, z25.h
    // vl128 state = 0x3b648b39
    __ dci(0x449152c9);  // smlslb z9.s, z22.h, z17.h
    // vl128 state = 0x5b5bab94
    __ dci(0x449542cd);  // smlalb z13.s, z22.h, z21.h
    // vl128 state = 0x65282d76
    __ dci(0x449c42c9);  // smlalb z9.s, z22.h, z28.h
    // vl128 state = 0x94a92486
    __ dci(0x449c52f9);  // smlslb z25.s, z23.h, z28.h
    // vl128 state = 0xd4f62835
    __ dci(0x44dc5afd);  // umlslb z29.d, z23.s, z28.s
    // vl128 state = 0xf124c6a1
    __ dci(0x44dd58ff);  // umlslb z31.d, z7.s, z29.s
    // vl128 state = 0xbc694f1c
    __ dci(0x44dc587b);  // umlslb z27.d, z3.s, z28.s
    // vl128 state = 0xf1621eb2
    __ dci(0x44de596b);  // umlslb z11.d, z11.s, z30.s
    // vl128 state = 0x944b4b75
    __ dci(0x44de5969);  // umlslb z9.d, z11.s, z30.s
    // vl128 state = 0xa98a2c38
    __ dci(0x44db596d);  // umlslb z13.d, z11.s, z27.s
    // vl128 state = 0x6bd60807
    __ dci(0x44db5d5d);  // umlslt z29.d, z10.s, z27.s
    // vl128 state = 0x9c377b51
    __ dci(0x449b555f);  // smlslt z31.s, z10.h, z27.h
    // vl128 state = 0x7c81f1d5
    __ dci(0x449b555d);  // smlslt z29.s, z10.h, z27.h
    // vl128 state = 0xdaab1edb
    __ dci(0x44d35559);  // smlslt z25.d, z10.s, z19.s
    // vl128 state = 0xdc3f25f1
    __ dci(0x44d355f8);  // smlslt z24.d, z15.s, z19.s
    // vl128 state = 0x9c75a3cf
    __ dci(0x44d356f9);  // smlslt z25.d, z23.s, z19.s
    // vl128 state = 0x5b999178
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x5b999178,
        0xd6191e64,
        0x1f3bd2a1,
        0x1e0ac282,
        0x8d13f5d3,
        0x97157e8f,
        0x5d6e4134,
        0x8d2186b4,
        0x88078c65,
        0x6dd92db3,
        0xfcd02d21,
        0x81738dc2,
        0x644e3c06,
        0x9c9d2ac8,
        0xaaa43548,
        0x871e9b08,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_complex_integer_multiply_add_vector) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 40 * kInstructionSize);
    __ dci(0x44dd2f34);  // cmla z20.d, z25.d, z29.d, #270
    // vl128 state = 0x12e9bd68
    __ dci(0x44dd2f3c);  // cmla z28.d, z25.d, z29.d, #270
    // vl128 state = 0x4fd8ba3e
    __ dci(0x44dc2734);  // cmla z20.d, z25.d, z28.d, #90
    // vl128 state = 0x9b11d64f
    __ dci(0x44dc2e36);  // cmla z22.d, z17.d, z28.d, #270
    // vl128 state = 0x4658e6ae
    __ dci(0x44dd2f34);  // cmla z20.d, z25.d, z29.d, #270
    // vl128 state = 0x5151ea16
    __ dci(0x44dc2fb5);  // cmla z21.d, z29.d, z28.d, #270
    // vl128 state = 0x21c497cc
    __ dci(0x44dc2fbd);  // cmla z29.d, z29.d, z28.d, #270
    // vl128 state = 0xe823fd46
    __ dci(0x44dc2e3c);  // cmla z28.d, z17.d, z28.d, #270
    // vl128 state = 0xcc35cda6
    __ dci(0x44dc2e34);  // cmla z20.d, z17.d, z28.d, #270
    // vl128 state = 0x963047c0
    __ dci(0x44d42c30);  // cmla z16.d, z1.d, z20.d, #270
    // vl128 state = 0x5d2c5643
    __ dci(0x44c42c60);  // cmla z0.d, z3.d, z4.d, #270
    // vl128 state = 0xfd400169
    __ dci(0x44842464);  // cmla z4.s, z3.s, z4.s, #90
    // vl128 state = 0x00116098
    __ dci(0x44842d60);  // cmla z0.s, z11.s, z4.s, #270
    // vl128 state = 0x582d46e3
    __ dci(0x44042562);  // cmla z2.b, z11.b, z4.b, #90
    // vl128 state = 0x1bd70bf0
    __ dci(0x44042420);  // cmla z0.b, z1.b, z4.b, #90
    // vl128 state = 0x7682807d
    __ dci(0x44062401);  // cmla z1.b, z0.b, z6.b, #90
    // vl128 state = 0xaa3e2c64
    __ dci(0x44042449);  // cmla z9.b, z2.b, z4.b, #90
    // vl128 state = 0xd81638f9
    __ dci(0x44052059);  // cmla z25.b, z2.b, z5.b, #0
    // vl128 state = 0x38cb5d96
    __ dci(0x4415305d);  // sqrdcmlah z29.b, z2.b, z21.b, #0
    // vl128 state = 0x4c6b85e0
    __ dci(0x44153819);  // sqrdcmlah z25.b, z0.b, z21.b, #180
    // vl128 state = 0x229b5be9
    __ dci(0x4405391b);  // sqrdcmlah z27.b, z8.b, z5.b, #180
    // vl128 state = 0x82611aec
    __ dci(0x4405314b);  // sqrdcmlah z11.b, z10.b, z5.b, #0
    // vl128 state = 0xe58c48e0
    __ dci(0x4407316a);  // sqrdcmlah z10.b, z11.b, z7.b, #0
    // vl128 state = 0x5282838a
    __ dci(0x4407347a);  // sqrdcmlah z26.b, z3.b, z7.b, #90
    // vl128 state = 0x134a0891
    __ dci(0x4413347e);  // sqrdcmlah z30.b, z3.b, z19.b, #90
    // vl128 state = 0x455ab9e0
    __ dci(0x4443347f);  // sqrdcmlah z31.h, z3.h, z3.h, #90
    // vl128 state = 0x030d9d2c
    __ dci(0x444b307e);  // sqrdcmlah z30.h, z3.h, z11.h, #0
    // vl128 state = 0x91a95a2c
    __ dci(0x444b301f);  // sqrdcmlah z31.h, z0.h, z11.h, #0
    // vl128 state = 0x0f1c8468
    __ dci(0x4409300f);  // sqrdcmlah z15.b, z0.b, z9.b, #0
    // vl128 state = 0x95f802b7
    __ dci(0x440c300e);  // sqrdcmlah z14.b, z0.b, z12.b, #0
    // vl128 state = 0x5fa6d2c6
    __ dci(0x4404310c);  // sqrdcmlah z12.b, z8.b, z4.b, #0
    // vl128 state = 0x192b05a4
    __ dci(0x4415310d);  // sqrdcmlah z13.b, z8.b, z21.b, #0
    // vl128 state = 0xa8a8d37f
    __ dci(0x4414350f);  // sqrdcmlah z15.b, z8.b, z20.b, #90
    // vl128 state = 0xcd890d8c
    __ dci(0x4454354d);  // sqrdcmlah z13.h, z10.h, z20.h, #90
    // vl128 state = 0x91ab863e
    __ dci(0x444435c5);  // sqrdcmlah z5.h, z14.h, z4.h, #90
    // vl128 state = 0x41bbc90c
    __ dci(0x444c34c7);  // sqrdcmlah z7.h, z6.h, z12.h, #90
    // vl128 state = 0xb6329344
    __ dci(0x444836c6);  // sqrdcmlah z6.h, z22.h, z8.h, #90
    // vl128 state = 0xdf5f443c
    __ dci(0x444836d6);  // sqrdcmlah z22.h, z22.h, z8.h, #90
    // vl128 state = 0x719a2e70
    __ dci(0x44403694);  // sqrdcmlah z20.h, z20.h, z0.h, #90
    // vl128 state = 0x28a64934
    __ dci(0x4449369c);  // sqrdcmlah z28.h, z20.h, z9.h, #90
    // vl128 state = 0x5d41ba84
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x5d41ba84,
        0xd5e52f4d,
        0x9f627c0d,
        0x111f21a7,
        0x5d7b356e,
        0x1f345c0e,
        0xd881296e,
        0x819f9091,
        0x59823550,
        0xbe2162c7,
        0x5f5dca40,
        0xad7e429e,
        0x4f66661f,
        0x7c5fbca0,
        0x819ff997,
        0x68ebdb56,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_complex_integer_multiply_add_indexed) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 20 * kInstructionSize);
    __ dci(0x44fd7d52);  // sqrdcmlah z18.s, z10.s, z13.s[1], #270
    // vl128 state = 0x5c66baad
    __ dci(0x44fd7c13);  // sqrdcmlah z19.s, z0.s, z13.s[1], #270
    // vl128 state = 0xac8c451b
    __ dci(0x44f97e11);  // sqrdcmlah z17.s, z16.s, z9.s[1], #270
    // vl128 state = 0x02ebccdb
    __ dci(0x44e97615);  // sqrdcmlah z21.s, z16.s, z9.s[0], #90
    // vl128 state = 0xe43b1032
    __ dci(0x44e97614);  // sqrdcmlah z20.s, z16.s, z9.s[0], #90
    // vl128 state = 0xa28d9898
    __ dci(0x44e17635);  // sqrdcmlah z21.s, z17.s, z1.s[0], #90
    // vl128 state = 0x021764c6
    __ dci(0x44e17634);  // sqrdcmlah z20.s, z17.s, z1.s[0], #90
    // vl128 state = 0x812dbf22
    __ dci(0x44f07635);  // sqrdcmlah z21.s, z17.s, z0.s[1], #90
    // vl128 state = 0x5e87a59e
    __ dci(0x44f07465);  // sqrdcmlah z5.s, z3.s, z0.s[1], #90
    // vl128 state = 0xd1a78d9d
    __ dci(0x44f87675);  // sqrdcmlah z21.s, z19.s, z8.s[1], #90
    // vl128 state = 0xd4500975
    __ dci(0x44b87e7d);  // sqrdcmlah z29.h, z19.h, z0.h[3], #270
    // vl128 state = 0x765230ab
    __ dci(0x44b876f9);  // sqrdcmlah z25.h, z23.h, z0.h[3], #90
    // vl128 state = 0xca9c5bb4
    __ dci(0x44f874fb);  // sqrdcmlah z27.s, z7.s, z8.s[1], #90
    // vl128 state = 0xa4bc044a
    __ dci(0x44f070fa);  // sqrdcmlah z26.s, z7.s, z0.s[1], #0
    // vl128 state = 0xd0eaa1df
    __ dci(0x44f07038);  // sqrdcmlah z24.s, z1.s, z0.s[1], #0
    // vl128 state = 0x80836f9f
    __ dci(0x44b17030);  // sqrdcmlah z16.h, z1.h, z1.h[2], #0
    // vl128 state = 0x59ffa1ce
    __ dci(0x44b17032);  // sqrdcmlah z18.h, z1.h, z1.h[2], #0
    // vl128 state = 0xdb8beca5
    __ dci(0x44b07430);  // sqrdcmlah z16.h, z1.h, z0.h[2], #90
    // vl128 state = 0xe5b6a0e3
    __ dci(0x44b07438);  // sqrdcmlah z24.h, z1.h, z0.h[2], #90
    // vl128 state = 0x19cc8c20
    __ dci(0x44b0743a);  // sqrdcmlah z26.h, z1.h, z0.h[2], #90
    // vl128 state = 0x19c819af
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x19c819af,
        0xbb2225f2,
        0x7e54f513,
        0xdcbf6f0f,
        0x2bfdc97d,
        0x48890c54,
        0x65542c02,
        0xaef6b224,
        0x993b14fd,
        0x244d27c5,
        0xe8767ba8,
        0x4397a148,
        0xb3efcd2e,
        0xb5894aba,
        0x2a0f6f7a,
        0xbe45142c,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_saturating_multiply_add_long_vector) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 40 * kInstructionSize);
    __ dci(0x44db629b);  // sqdmlalb z27.d, z20.s, z27.s
    // vl128 state = 0x61e408e4
    __ dci(0x44db631f);  // sqdmlalb z31.d, z24.s, z27.s
    // vl128 state = 0xf146813f
    __ dci(0x44da6b1d);  // sqdmlslb z29.d, z24.s, z26.s
    // vl128 state = 0xb8d07371
    __ dci(0x44da6a35);  // sqdmlslb z21.d, z17.s, z26.s
    // vl128 state = 0xaf43cc88
    __ dci(0x444a6a3d);  // sqdmlslb z29.h, z17.b, z10.b
    // vl128 state = 0xba4c5067
    __ dci(0x444a6a39);  // sqdmlslb z25.h, z17.b, z10.b
    // vl128 state = 0x396202c3
    __ dci(0x445a6829);  // sqdmlslb z9.h, z1.b, z26.b
    // vl128 state = 0x22095f7f
    __ dci(0x445a6b28);  // sqdmlslb z8.h, z25.b, z26.b
    // vl128 state = 0xa9516b4b
    __ dci(0x44da6b69);  // sqdmlslb z9.d, z27.s, z26.s
    // vl128 state = 0x1f048226
    __ dci(0x44da616d);  // sqdmlalb z13.d, z11.s, z26.s
    // vl128 state = 0x0fdd982f
    __ dci(0x4458616f);  // sqdmlalb z15.h, z11.b, z24.b
    // vl128 state = 0x461ba137
    __ dci(0x4449617f);  // sqdmlalb z31.h, z11.b, z9.b
    // vl128 state = 0xd1071b0c
    __ dci(0x4459614f);  // sqdmlalb z15.h, z10.b, z25.b
    // vl128 state = 0x0fa6bae7
    __ dci(0x4458654d);  // sqdmlalt z13.h, z10.b, z24.b
    // vl128 state = 0xebd08a80
    __ dci(0x44586d05);  // sqdmlslt z5.h, z8.b, z24.b
    // vl128 state = 0xd4c41665
    __ dci(0x44506d84);  // sqdmlslt z4.h, z12.b, z16.b
    // vl128 state = 0x80f619f9
    __ dci(0x44506fc6);  // sqdmlslt z6.h, z30.b, z16.b
    // vl128 state = 0xb588af21
    __ dci(0x44566fc4);  // sqdmlslt z4.h, z30.b, z22.b
    // vl128 state = 0x4dd8437a
    __ dci(0x44566f0c);  // sqdmlslt z12.h, z24.b, z22.b
    // vl128 state = 0x48ca6e5c
    __ dci(0x44566f0e);  // sqdmlslt z14.h, z24.b, z22.b
    // vl128 state = 0x02d6f977
    __ dci(0x44566746);  // sqdmlalt z6.h, z26.b, z22.b
    // vl128 state = 0x179f59f4
    __ dci(0x445767c4);  // sqdmlalt z4.h, z30.b, z23.b
    // vl128 state = 0xf2d2823c
    __ dci(0x44d667c0);  // sqdmlalt z0.d, z30.s, z22.s
    // vl128 state = 0x404c277e
    __ dci(0x44566742);  // sqdmlalt z2.h, z26.b, z22.b
    // vl128 state = 0x986a72c1
    __ dci(0x44c6674a);  // sqdmlalt z10.d, z26.s, z6.s
    // vl128 state = 0xbb8044ab
    __ dci(0x44c66742);  // sqdmlalt z2.d, z26.s, z6.s
    // vl128 state = 0x9f5b244b
    __ dci(0x44ce6706);  // sqdmlalt z6.d, z24.s, z14.s
    // vl128 state = 0xc6ce6266
    __ dci(0x44ce670e);  // sqdmlalt z14.d, z24.s, z14.s
    // vl128 state = 0xc9e1a461
    __ dci(0x44de6746);  // sqdmlalt z6.d, z26.s, z30.s
    // vl128 state = 0x9f133504
    __ dci(0x44dc6342);  // sqdmlalb z2.d, z26.s, z28.s
    // vl128 state = 0x42deb468
    __ dci(0x44d46366);  // sqdmlalb z6.d, z27.s, z20.s
    // vl128 state = 0xb3436cd4
    __ dci(0x44d5626e);  // sqdmlalb z14.d, z19.s, z21.s
    // vl128 state = 0x0e0533ac
    __ dci(0x44d5646f);  // sqdmlalt z15.d, z3.s, z21.s
    // vl128 state = 0x92d04e7b
    __ dci(0x44d36467);  // sqdmlalt z7.d, z3.s, z19.s
    // vl128 state = 0xd9fa8b4d
    __ dci(0x44d360ef);  // sqdmlalb z15.d, z7.s, z19.s
    // vl128 state = 0x9c9a5778
    __ dci(0x44d3646b);  // sqdmlalt z11.d, z3.s, z19.s
    // vl128 state = 0x40d7c923
    __ dci(0x4492646f);  // sqdmlalt z15.s, z3.h, z18.h
    // vl128 state = 0x0b5b2334
    __ dci(0x4492647f);  // sqdmlalt z31.s, z3.h, z18.h
    // vl128 state = 0xfe6302c1
    __ dci(0x4494647d);  // sqdmlalt z29.s, z3.h, z20.h
    // vl128 state = 0xe3c05a37
    __ dci(0x4484666d);  // sqdmlalt z13.s, z19.h, z4.h
    // vl128 state = 0x15169e94
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x15169e94,
        0x6101102c,
        0xa5586d26,
        0x3fbf4f9f,
        0x8e62994d,
        0x4d77a9e5,
        0x4ceadc9e,
        0x8247db61,
        0x4aa10859,
        0x0b3280b3,
        0x015d75ea,
        0x1cf4825e,
        0xda7d3fea,
        0xc24bd624,
        0x60ee565a,
        0x7ac92c39,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_saturating_multiply_add_interleaved_long) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0x449e0ac6);  // sqdmlalbt z6.s, z22.h, z30.h
    // vl128 state = 0x1f0ef37c
    __ dci(0x449c0ae4);  // sqdmlalbt z4.s, z23.h, z28.h
    // vl128 state = 0xa80bf2c8
    __ dci(0x449c0ae6);  // sqdmlalbt z6.s, z23.h, z28.h
    // vl128 state = 0x4c5b0e8f
    __ dci(0x449e0aae);  // sqdmlalbt z14.s, z21.h, z30.h
    // vl128 state = 0xa6482041
    __ dci(0x449e0aaf);  // sqdmlalbt z15.s, z21.h, z30.h
    // vl128 state = 0x6ef82b7a
    __ dci(0x449c0a2b);  // sqdmlalbt z11.s, z17.h, z28.h
    // vl128 state = 0x0070a7fa
    __ dci(0x449e0829);  // sqdmlalbt z9.s, z1.h, z30.h
    // vl128 state = 0x08b9efc6
    __ dci(0x449e0c61);  // sqdmlslbt z1.s, z3.h, z30.h
    // vl128 state = 0xebd25c16
    __ dci(0x449e0c60);  // sqdmlslbt z0.s, z3.h, z30.h
    // vl128 state = 0x0926abbe
    __ dci(0x449e0c70);  // sqdmlslbt z16.s, z3.h, z30.h
    // vl128 state = 0xe9d3e5a7
    __ dci(0x449f0cf4);  // sqdmlslbt z20.s, z7.h, z31.h
    // vl128 state = 0xf062523d
    __ dci(0x449f08b5);  // sqdmlalbt z21.s, z5.h, z31.h
    // vl128 state = 0x6034c14e
    __ dci(0x449f08a5);  // sqdmlalbt z5.s, z5.h, z31.h
    // vl128 state = 0x0a73c74b
    __ dci(0x448e08b5);  // sqdmlalbt z21.s, z5.h, z14.h
    // vl128 state = 0xa4af2700
    __ dci(0x448c08e5);  // sqdmlalbt z5.s, z7.h, z12.h
    // vl128 state = 0x7499c587
    __ dci(0x448c08e1);  // sqdmlalbt z1.s, z7.h, z12.h
    // vl128 state = 0x968bca0e
    __ dci(0x448c0971);  // sqdmlalbt z17.s, z11.h, z12.h
    // vl128 state = 0xd7890449
    __ dci(0x448f0975);  // sqdmlalbt z21.s, z11.h, z15.h
    // vl128 state = 0xa2393863
    __ dci(0x448f0977);  // sqdmlalbt z23.s, z11.h, z15.h
    // vl128 state = 0x0f7d9688
    __ dci(0x449f093f);  // sqdmlalbt z31.s, z9.h, z31.h
    // vl128 state = 0xeb16ca99
    __ dci(0x449f09f7);  // sqdmlalbt z23.s, z15.h, z31.h
    // vl128 state = 0x5eca8b00
    __ dci(0x449f0987);  // sqdmlalbt z7.s, z12.h, z31.h
    // vl128 state = 0xf8f22744
    __ dci(0x449f0a83);  // sqdmlalbt z3.s, z20.h, z31.h
    // vl128 state = 0xc20d54f5
    __ dci(0x449b0ac1);  // sqdmlalbt z1.s, z22.h, z27.h
    // vl128 state = 0xf371a13b
    __ dci(0x449b0aa9);  // sqdmlalbt z9.s, z21.h, z27.h
    // vl128 state = 0xffae55ce
    __ dci(0x449b0ab9);  // sqdmlalbt z25.s, z21.h, z27.h
    // vl128 state = 0x0c5ab866
    __ dci(0x44d30aa9);  // sqdmlalbt z9.d, z21.s, z19.s
    // vl128 state = 0x388bfe27
    __ dci(0x44d30aab);  // sqdmlalbt z11.d, z21.s, z19.s
    // vl128 state = 0x6dc15ec8
    __ dci(0x44d70baf);  // sqdmlalbt z15.d, z29.s, z23.s
    // vl128 state = 0x6a858021
    __ dci(0x44d70ba7);  // sqdmlalbt z7.d, z29.s, z23.s
    // vl128 state = 0x52416517
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x52416517,
        0x1a625e10,
        0x3eaaa30f,
        0x0eefe820,
        0x9e2f7744,
        0x3dbc3206,
        0xca85b926,
        0x9428c809,
        0x7c35818c,
        0xb8bc3648,
        0x5b215c50,
        0xbdb56ba5,
        0xe4e4bc54,
        0x69ba132f,
        0xa498b17a,
        0xf482b2a6,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_saturating_multiply_add_long_indexed) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x44f52e3d);  // sqdmlalt z29.d, z17.s, z5.s[3]
    // vl128 state = 0x2a284ede
    __ dci(0x44f52e3c);  // sqdmlalt z28.d, z17.s, z5.s[3]
    // vl128 state = 0x48a615e9
    __ dci(0x44f72c3d);  // sqdmlalt z29.d, z1.s, z7.s[3]
    // vl128 state = 0x1bbe9cc5
    __ dci(0x44b62c35);  // sqdmlalt z21.s, z1.h, z6.h[5]
    // vl128 state = 0x99966225
    __ dci(0x44b624b7);  // sqdmlalt z23.s, z5.h, z6.h[4]
    // vl128 state = 0x36da4a3a
    __ dci(0x44f626b6);  // sqdmlalt z22.d, z21.s, z6.s[2]
    // vl128 state = 0xc009e514
    __ dci(0x44f62226);  // sqdmlalb z6.d, z17.s, z6.s[2]
    // vl128 state = 0x2140ee4b
    __ dci(0x44fa222e);  // sqdmlalb z14.d, z17.s, z10.s[2]
    // vl128 state = 0xf78c8bec
    __ dci(0x44fa2aac);  // sqdmlalb z12.d, z21.s, z10.s[3]
    // vl128 state = 0x329238c6
    __ dci(0x44fa2abc);  // sqdmlalb z28.d, z21.s, z10.s[3]
    // vl128 state = 0xadc9f9db
    __ dci(0x44fa2aac);  // sqdmlalb z12.d, z21.s, z10.s[3]
    // vl128 state = 0x877f64cf
    __ dci(0x44ba2a88);  // sqdmlalb z8.s, z20.h, z2.h[7]
    // vl128 state = 0x4e4a3117
    __ dci(0x44fb2a89);  // sqdmlalb z9.d, z20.s, z11.s[3]
    // vl128 state = 0xe26b041b
    __ dci(0x44f32ab9);  // sqdmlalb z25.d, z21.s, z3.s[3]
    // vl128 state = 0xbcf4e0b2
    __ dci(0x44e328bd);  // sqdmlalb z29.d, z5.s, z3.s[1]
    // vl128 state = 0x31391cc2
    __ dci(0x44f228ad);  // sqdmlalb z13.d, z5.s, z2.s[3]
    // vl128 state = 0xf4c6c098
    __ dci(0x44e238af);  // sqdmlslb z15.d, z5.s, z2.s[1]
    // vl128 state = 0x6e7cb20c
    __ dci(0x44e639ad);  // sqdmlslb z13.d, z13.s, z6.s[1]
    // vl128 state = 0xed16e292
    __ dci(0x44a63daf);  // sqdmlslt z15.s, z13.h, z6.h[1]
    // vl128 state = 0x7c0c3a9a
    __ dci(0x44ae3cbf);  // sqdmlslt z31.s, z5.h, z6.h[3]
    // vl128 state = 0x0e2dce8d
    __ dci(0x44a634b7);  // sqdmlslt z23.s, z5.h, z6.h[0]
    // vl128 state = 0xf3eeab27
    __ dci(0x44e234b5);  // sqdmlslt z21.d, z5.s, z2.s[0]
    // vl128 state = 0x55193209
    __ dci(0x44a23437);  // sqdmlslt z23.s, z1.h, z2.h[0]
    // vl128 state = 0x7652b538
    __ dci(0x44a63535);  // sqdmlslt z21.s, z9.h, z6.h[0]
    // vl128 state = 0x76046ab4
    __ dci(0x44a235b4);  // sqdmlslt z20.s, z13.h, z2.h[0]
    // vl128 state = 0x2f23fd0d
    __ dci(0x44a234e4);  // sqdmlslt z4.s, z7.h, z2.h[0]
    // vl128 state = 0x2a50774c
    __ dci(0x44a234ec);  // sqdmlslt z12.s, z7.h, z2.h[0]
    // vl128 state = 0x01ea8843
    __ dci(0x44a324e8);  // sqdmlalt z8.s, z7.h, z3.h[0]
    // vl128 state = 0xed54a157
    __ dci(0x44a334c9);  // sqdmlslt z9.s, z6.h, z3.h[0]
    // vl128 state = 0x39e0227b
    __ dci(0x44a324f9);  // sqdmlalt z25.s, z7.h, z3.h[0]
    // vl128 state = 0xf163fa0b
    __ dci(0x44a224d8);  // sqdmlalt z24.s, z6.h, z2.h[0]
    // vl128 state = 0xbb4e0d24
    __ dci(0x44b22448);  // sqdmlalt z8.s, z2.h, z2.h[4]
    // vl128 state = 0x26c102cc
    __ dci(0x44f224d8);  // sqdmlalt z24.d, z6.s, z2.s[2]
    // vl128 state = 0x40f79dde
    __ dci(0x44f220f9);  // sqdmlalb z25.d, z7.s, z2.s[2]
    // vl128 state = 0xf9d62034
    __ dci(0x44f020a9);  // sqdmlalb z9.d, z5.s, z0.s[2]
    // vl128 state = 0x2b78be2f
    __ dci(0x44f424ad);  // sqdmlalt z13.d, z5.s, z4.s[2]
    // vl128 state = 0xf0701e23
    __ dci(0x44f430a5);  // sqdmlslb z5.d, z5.s, z4.s[2]
    // vl128 state = 0x992b12d6
    __ dci(0x44f130a4);  // sqdmlslb z4.d, z5.s, z1.s[2]
    // vl128 state = 0x50292759
    __ dci(0x44f130ac);  // sqdmlslb z12.d, z5.s, z1.s[2]
    // vl128 state = 0x795462f2
    __ dci(0x44f3302d);  // sqdmlslb z13.d, z1.s, z3.s[2]
    // vl128 state = 0x8ac29815
    __ dci(0x44e3300c);  // sqdmlslb z12.d, z0.s, z3.s[0]
    // vl128 state = 0x842471eb
    __ dci(0x44e3300d);  // sqdmlslb z13.d, z0.s, z3.s[0]
    // vl128 state = 0x28762af1
    __ dci(0x44eb321d);  // sqdmlslb z29.d, z16.s, z11.s[0]
    // vl128 state = 0x352de071
    __ dci(0x44ef3259);  // sqdmlslb z25.d, z18.s, z15.s[0]
    // vl128 state = 0x90a4cf15
    __ dci(0x44ff3349);  // sqdmlslb z9.d, z26.s, z15.s[2]
    // vl128 state = 0x6be7e76a
    __ dci(0x44fb3319);  // sqdmlslb z25.d, z24.s, z11.s[2]
    // vl128 state = 0x7023e2de
    __ dci(0x44bb3b18);  // sqdmlslb z24.s, z24.h, z3.h[7]
    // vl128 state = 0xad48664c
    __ dci(0x44bb3b19);  // sqdmlslb z25.s, z24.h, z3.h[7]
    // vl128 state = 0xc7d8239b
    __ dci(0x44bb3b11);  // sqdmlslb z17.s, z24.h, z3.h[7]
    // vl128 state = 0x0d9b2b9b
    __ dci(0x44f33b15);  // sqdmlslb z21.d, z24.s, z3.s[3]
    // vl128 state = 0xbdb9c559
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xbdb9c559,
        0x0c2f83d5,
        0x3e1f2607,
        0x2db954ea,
        0xff33857d,
        0xd567c205,
        0x8b5ced4c,
        0x19ecc4d9,
        0x8581949e,
        0x30f1a921,
        0x8c94071b,
        0xb9ad4919,
        0x32dbb108,
        0x634f9cd4,
        0x2a122429,
        0xdae127f1,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_floating_multiply_add_long_vector) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm, kFpInputSet);
  // state = 0x1e5cbcac

  {
    ExactAssemblyScope scope(&masm, 30 * kInstructionSize);
    __ dci(0x64bfa635);  // fmlslt z21.s, z17.h, z31.h
    // vl128 state = 0x48383595
    __ dci(0x64bf867d);  // fmlalt z29.s, z19.h, z31.h
    // vl128 state = 0xf2812c0e
    __ dci(0x64af877c);  // fmlalt z28.s, z27.h, z15.h
    // vl128 state = 0x161daf06
    __ dci(0x64af8774);  // fmlalt z20.s, z27.h, z15.h
    // vl128 state = 0x8146f2bf
    __ dci(0x64be877c);  // fmlalt z28.s, z27.h, z30.h
    // vl128 state = 0x90bcd864
    __ dci(0x64bd876c);  // fmlalt z12.s, z27.h, z29.h
    // vl128 state = 0x22b60b78
    __ dci(0x64bf8728);  // fmlalt z8.s, z25.h, z31.h
    // vl128 state = 0x2c9ce51a
    __ dci(0x64bf836a);  // fmlalb z10.s, z27.h, z31.h
    // vl128 state = 0x40e6b398
    __ dci(0x64bf87eb);  // fmlalt z11.s, z31.h, z31.h
    // vl128 state = 0x479c4a98
    __ dci(0x64bf87e9);  // fmlalt z9.s, z31.h, z31.h
    // vl128 state = 0x25c987ad
    __ dci(0x64b78779);  // fmlalt z25.s, z27.h, z23.h
    // vl128 state = 0xb4fbc429
    __ dci(0x64b1877b);  // fmlalt z27.s, z27.h, z17.h
    // vl128 state = 0x390616d8
    __ dci(0x64b1871f);  // fmlalt z31.s, z24.h, z17.h
    // vl128 state = 0x7f24d2bf
    __ dci(0x64b5878f);  // fmlalt z15.s, z28.h, z21.h
    // vl128 state = 0x01a90318
    __ dci(0x64b4870d);  // fmlalt z13.s, z24.h, z20.h
    // vl128 state = 0x08789c2c
    __ dci(0x64b48709);  // fmlalt z9.s, z24.h, z20.h
    // vl128 state = 0x169f9b57
    __ dci(0x64b48779);  // fmlalt z25.s, z27.h, z20.h
    // vl128 state = 0xad4f23d7
    __ dci(0x64bc8671);  // fmlalt z17.s, z19.h, z28.h
    // vl128 state = 0xf86b0a64
    __ dci(0x64b98673);  // fmlalt z19.s, z19.h, z25.h
    // vl128 state = 0x78a848b2
    __ dci(0x64b18623);  // fmlalt z3.s, z17.h, z17.h
    // vl128 state = 0xcac211c9
    __ dci(0x64b18642);  // fmlalt z2.s, z18.h, z17.h
    // vl128 state = 0x9afcbe3f
    __ dci(0x64b1a6c0);  // fmlslt z0.s, z22.h, z17.h
    // vl128 state = 0x0047e4b2
    __ dci(0x64b086c4);  // fmlalt z4.s, z22.h, z16.h
    // vl128 state = 0x203324b5
    __ dci(0x64b28645);  // fmlalt z5.s, z18.h, z18.h
    // vl128 state = 0x7340c432
    __ dci(0x64b28264);  // fmlalb z4.s, z19.h, z18.h
    // vl128 state = 0x6dc657a9
    __ dci(0x64b28765);  // fmlalt z5.s, z27.h, z18.h
    // vl128 state = 0xa5d3889b
    __ dci(0x64ba8561);  // fmlalt z1.s, z11.h, z26.h
    // vl128 state = 0x5bbd2dd9
    __ dci(0x64aa8543);  // fmlalt z3.s, z10.h, z10.h
    // vl128 state = 0xa65ec305
    __ dci(0x64ae8141);  // fmlalb z1.s, z10.h, z14.h
    // vl128 state = 0xd23d588c
    __ dci(0x64ae80c3);  // fmlalb z3.s, z6.h, z14.h
    // vl128 state = 0x5a082bbc
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x5a082bbc,
        0x23c41852,
        0xf462f328,
        0x6fa4d12b,
        0x5e5f3e79,
        0x9939c7e6,
        0x0ed39313,
        0x2911107c,
        0x18f77b9a,
        0x7226d5b3,
        0x05df3c07,
        0x1653749c,
        0xcb4f6acf,
        0x4c5f0755,
        0xc4eed654,
        0x47893eeb,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_mla_long_index) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x44ea8d67);  // smlalt z7.d, z11.s, z10.s[1]
    // vl128 state = 0xd08dbe24
    __ dci(0x44ea9d2f);  // umlalt z15.d, z9.s, z10.s[1]
    // vl128 state = 0x56f6f237
    __ dci(0x44ea9d2d);  // umlalt z13.d, z9.s, z10.s[1]
    // vl128 state = 0x00f89e4d
    __ dci(0x44eb992f);  // umlalb z15.d, z9.s, z11.s[1]
    // vl128 state = 0xca4e469e
    __ dci(0x44ab99ae);  // umlalb z14.s, z13.h, z3.h[3]
    // vl128 state = 0xd4b18276
    __ dci(0x44ad99be);  // umlalb z30.s, z13.h, z5.h[3]
    // vl128 state = 0x8650a79e
    __ dci(0x44ad99ba);  // umlalb z26.s, z13.h, z5.h[3]
    // vl128 state = 0x6fa1a501
    __ dci(0x44adb9f2);  // umlslb z18.s, z15.h, z5.h[3]
    // vl128 state = 0x1a56a5d4
    __ dci(0x44bda9f3);  // smlslb z19.s, z15.h, z5.h[7]
    // vl128 state = 0xfdb18057
    __ dci(0x44b9a1fb);  // smlslb z27.s, z15.h, z1.h[6]
    // vl128 state = 0xb46b6c28
    __ dci(0x44b8a1b3);  // smlslb z19.s, z13.h, z0.h[6]
    // vl128 state = 0x623c62c3
    __ dci(0x44bc81b1);  // smlalb z17.s, z13.h, z4.h[6]
    // vl128 state = 0x2abab4d3
    __ dci(0x44bc82b0);  // smlalb z16.s, z21.h, z4.h[6]
    // vl128 state = 0x7a028731
    __ dci(0x44ac92b8);  // umlalb z24.s, z21.h, z4.h[2]
    // vl128 state = 0xf48f6936
    __ dci(0x44a4923a);  // umlalb z26.s, z17.h, z4.h[0]
    // vl128 state = 0xbcdf888d
    __ dci(0x44b49a3e);  // umlalb z30.s, z17.h, z4.h[5]
    // vl128 state = 0x5060778e
    __ dci(0x44b69a1c);  // umlalb z28.s, z16.h, z6.h[5]
    // vl128 state = 0x16da3835
    __ dci(0x44b6b218);  // umlslb z24.s, z16.h, z6.h[4]
    // vl128 state = 0xac7fb4d0
    __ dci(0x44b2b25a);  // umlslb z26.s, z18.h, z2.h[4]
    // vl128 state = 0x8d05433b
    __ dci(0x44b2ba0a);  // umlslb z10.s, z16.h, z2.h[5]
    // vl128 state = 0x62630101
    __ dci(0x44b29b08);  // umlalb z8.s, z24.h, z2.h[5]
    // vl128 state = 0x31ae445b
    __ dci(0x44b29b00);  // umlalb z0.s, z24.h, z2.h[5]
    // vl128 state = 0x539a5875
    __ dci(0x44b29e08);  // umlalt z8.s, z16.h, z2.h[5]
    // vl128 state = 0x07d4bf73
    __ dci(0x44b29eaa);  // umlalt z10.s, z21.h, z2.h[5]
    // vl128 state = 0x314f48a8
    __ dci(0x44b2be2e);  // umlslt z14.s, z17.h, z2.h[5]
    // vl128 state = 0x91bd2c17
    __ dci(0x44b2be3e);  // umlslt z30.s, z17.h, z2.h[5]
    // vl128 state = 0x4cbf4360
    __ dci(0x44f2be7a);  // umlslt z26.d, z19.s, z2.s[3]
    // vl128 state = 0xe94e76a9
    __ dci(0x44f2ae4a);  // smlslt z10.d, z18.s, z2.s[3]
    // vl128 state = 0xd0c2c4cc
    __ dci(0x44faae6e);  // smlslt z14.d, z19.s, z10.s[3]
    // vl128 state = 0xc64d6839
    __ dci(0x44faae6f);  // smlslt z15.d, z19.s, z10.s[3]
    // vl128 state = 0xa74358aa
    __ dci(0x44faae67);  // smlslt z7.d, z19.s, z10.s[3]
    // vl128 state = 0xb8d9664b
    __ dci(0x44fa8e57);  // smlalt z23.d, z18.s, z10.s[3]
    // vl128 state = 0xf1032ab4
    __ dci(0x44fa8c67);  // smlalt z7.d, z3.s, z10.s[3]
    // vl128 state = 0x763732f4
    __ dci(0x44eaac66);  // smlslt z6.d, z3.s, z10.s[1]
    // vl128 state = 0xdcf39367
    __ dci(0x44eaa456);  // smlslt z22.d, z2.s, z10.s[0]
    // vl128 state = 0x5ea67d82
    __ dci(0x44aea45e);  // smlslt z30.s, z2.h, z6.h[2]
    // vl128 state = 0x55da0908
    __ dci(0x44aaa64e);  // smlslt z14.s, z18.h, z2.h[2]
    // vl128 state = 0x69d105f5
    __ dci(0x44baa75e);  // smlslt z30.s, z26.h, z2.h[6]
    // vl128 state = 0x191bc065
    __ dci(0x44baa75a);  // smlslt z26.s, z26.h, z2.h[6]
    // vl128 state = 0xbf62d2a0
    __ dci(0x44eaa75b);  // smlslt z27.d, z26.s, z10.s[0]
    // vl128 state = 0x43803a21
    __ dci(0x44eabf5f);  // umlslt z31.d, z26.s, z10.s[1]
    // vl128 state = 0x0b33725c
    __ dci(0x44ebbd57);  // umlslt z23.d, z10.s, z11.s[1]
    // vl128 state = 0x0059a0f5
    __ dci(0x44abbf55);  // umlslt z21.s, z26.h, z3.h[3]
    // vl128 state = 0xb587057f
    __ dci(0x44abab5d);  // smlslb z29.s, z26.h, z3.h[3]
    // vl128 state = 0x0bfa30c6
    __ dci(0x44abab5c);  // smlslb z28.s, z26.h, z3.h[3]
    // vl128 state = 0x151045b4
    __ dci(0x44abaf78);  // smlslt z24.s, z27.h, z3.h[3]
    // vl128 state = 0xedb7fca9
    __ dci(0x44aaa77c);  // smlslt z28.s, z27.h, z2.h[2]
    // vl128 state = 0xb68216f9
    __ dci(0x44aaa178);  // smlslb z24.s, z11.h, z2.h[2]
    // vl128 state = 0x35447b11
    __ dci(0x44aa81fa);  // smlalb z26.s, z15.h, z2.h[2]
    // vl128 state = 0xf532285f
    __ dci(0x44aa8198);  // smlalb z24.s, z12.h, z2.h[2]
    // vl128 state = 0xd414889b
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xd414889b,
        0x79d8f659,
        0xe2c8f06b,
        0x91aadf3d,
        0xffb92c3e,
        0xc2d3138e,
        0xdd9f4396,
        0xce39a88e,
        0xfe68a5ca,
        0xdcb072b2,
        0x3756ede6,
        0x5c2eef22,
        0x01fd02a4,
        0xdd8d4890,
        0x87500dc9,
        0x8c895325,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_mul_long_index) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x44f1d492);  // umullt z18.d, z4.s, z1.s[2]
    // vl128 state = 0x4377a821
    __ dci(0x44fdd490);  // umullt z16.d, z4.s, z13.s[2]
    // vl128 state = 0x5879cb00
    __ dci(0x44fdc080);  // smullb z0.d, z4.s, z13.s[2]
    // vl128 state = 0xbe0f85f8
    __ dci(0x44fdc081);  // smullb z1.d, z4.s, z13.s[2]
    // vl128 state = 0xa0eb0d63
    __ dci(0x44fcc000);  // smullb z0.d, z0.s, z12.s[2]
    // vl128 state = 0xf023feb2
    __ dci(0x44ffc001);  // smullb z1.d, z0.s, z15.s[2]
    // vl128 state = 0xcc0dcc10
    __ dci(0x44ffc0c9);  // smullb z9.d, z6.s, z15.s[2]
    // vl128 state = 0x8e0d2525
    __ dci(0x44f7d0c8);  // umullb z8.d, z6.s, z7.s[2]
    // vl128 state = 0xaf711253
    __ dci(0x44b7d080);  // umullb z0.s, z4.h, z7.h[4]
    // vl128 state = 0x8cea3501
    __ dci(0x44f7d290);  // umullb z16.d, z20.s, z7.s[2]
    // vl128 state = 0x09be9a84
    __ dci(0x44f6da92);  // umullb z18.d, z20.s, z6.s[3]
    // vl128 state = 0x3906715f
    __ dci(0x44fed296);  // umullb z22.d, z20.s, z14.s[2]
    // vl128 state = 0xf399bb76
    __ dci(0x44f6c292);  // smullb z18.d, z20.s, z6.s[2]
    // vl128 state = 0x33ceff98
    __ dci(0x44e6c2a2);  // smullb z2.d, z21.s, z6.s[0]
    // vl128 state = 0x00765739
    __ dci(0x44e6c323);  // smullb z3.d, z25.s, z6.s[0]
    // vl128 state = 0x3dad5b1f
    __ dci(0x44e6c333);  // smullb z19.d, z25.s, z6.s[0]
    // vl128 state = 0xc5b39601
    __ dci(0x44e7c377);  // smullb z23.d, z27.s, z7.s[0]
    // vl128 state = 0x134b3d1f
    __ dci(0x44e7d3ff);  // umullb z31.d, z31.s, z7.s[0]
    // vl128 state = 0xc4be3961
    __ dci(0x44e7d3fe);  // umullb z30.d, z31.s, z7.s[0]
    // vl128 state = 0x195e406b
    __ dci(0x44e7c3da);  // smullb z26.d, z30.s, z7.s[0]
    // vl128 state = 0xae2522f9
    __ dci(0x44e7c2fe);  // smullb z30.d, z23.s, z7.s[0]
    // vl128 state = 0xed267bfb
    __ dci(0x44e3c3f6);  // smullb z22.d, z31.s, z3.s[0]
    // vl128 state = 0x6f6eeec4
    __ dci(0x44f3c2f2);  // smullb z18.d, z23.s, z3.s[2]
    // vl128 state = 0x1689afdf
    __ dci(0x44f3c2e2);  // smullb z2.d, z23.s, z3.s[2]
    // vl128 state = 0x24999374
    __ dci(0x44f3c06a);  // smullb z10.d, z3.s, z3.s[2]
    // vl128 state = 0x046126eb
    __ dci(0x44f3c06b);  // smullb z11.d, z3.s, z3.s[2]
    // vl128 state = 0x6b39941f
    __ dci(0x44f3c449);  // smullt z9.d, z2.s, z3.s[2]
    // vl128 state = 0xf161bcc6
    __ dci(0x44f3ccc8);  // smullt z8.d, z6.s, z3.s[3]
    // vl128 state = 0xbdc67c89
    __ dci(0x44f9ccd8);  // smullt z24.d, z6.s, z9.s[3]
    // vl128 state = 0xfed59871
    __ dci(0x44ffccdc);  // smullt z28.d, z6.s, z15.s[3]
    // vl128 state = 0x72746ff6
    __ dci(0x44fecc58);  // smullt z24.d, z2.s, z14.s[3]
    // vl128 state = 0xa15ee8f2
    __ dci(0x44bfcc48);  // smullt z8.s, z2.h, z7.h[7]
    // vl128 state = 0x3dccd2d6
    __ dci(0x44b7c84a);  // smullb z10.s, z2.h, z7.h[5]
    // vl128 state = 0x4537f0b2
    __ dci(0x44a5c84e);  // smullb z14.s, z2.h, z5.h[1]
    // vl128 state = 0x60e30690
    __ dci(0x44adca46);  // smullb z6.s, z18.h, z5.h[3]
    // vl128 state = 0xaef15cb5
    __ dci(0x44add847);  // umullb z7.s, z2.h, z5.h[3]
    // vl128 state = 0xe7df553d
    __ dci(0x44bdd04f);  // umullb z15.s, z2.h, z5.h[6]
    // vl128 state = 0xa713f809
    __ dci(0x44bdc007);  // smullb z7.s, z0.h, z5.h[6]
    // vl128 state = 0x4907c6b7
    __ dci(0x44bdc005);  // smullb z5.s, z0.h, z5.h[6]
    // vl128 state = 0x98a83fd0
    __ dci(0x44bdc0b5);  // smullb z21.s, z5.h, z5.h[6]
    // vl128 state = 0x3e6cb588
    __ dci(0x44bcc094);  // smullb z20.s, z4.h, z4.h[6]
    // vl128 state = 0x37e5a4ce
    __ dci(0x44bcc09c);  // smullb z28.s, z4.h, z4.h[6]
    // vl128 state = 0x719de631
    __ dci(0x44acc88c);  // smullb z12.s, z4.h, z4.h[3]
    // vl128 state = 0xf0f3dffe
    __ dci(0x44aac884);  // smullb z4.s, z4.h, z2.h[3]
    // vl128 state = 0x61a714ff
    __ dci(0x44a8c8ac);  // smullb z12.s, z5.h, z0.h[3]
    // vl128 state = 0xc47542ea
    __ dci(0x44a8cea4);  // smullt z4.s, z21.h, z0.h[3]
    // vl128 state = 0x37865031
    __ dci(0x44a8daa5);  // umullb z5.s, z21.h, z0.h[3]
    // vl128 state = 0x28cf4dc6
    __ dci(0x44b8dae4);  // umullb z4.s, z23.h, z0.h[7]
    // vl128 state = 0x6fe181d0
    __ dci(0x44b9da6c);  // umullb z12.s, z19.h, z1.h[7]
    // vl128 state = 0xde65c7e3
    __ dci(0x44b9da64);  // umullb z4.s, z19.h, z1.h[7]
    // vl128 state = 0x040a7e45
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x040a7e45,
        0x48fc4c2b,
        0x9a1c67d1,
        0xcb88ffdd,
        0xcda205bc,
        0x7a47b6fb,
        0x68ae16c8,
        0x483353c9,
        0x91d91835,
        0x17a9ca4a,
        0x4f3d394f,
        0x5182776c,
        0xc03c1d3b,
        0xe52799db,
        0x1ddd328e,
        0xe33903de,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_sat_double_mul_high) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x046c711a);  // sqdmulh z26.h, z8.h, z12.h
    // vl128 state = 0xe962209c
    __ dci(0x047c7138);  // sqdmulh z24.h, z9.h, z28.h
    // vl128 state = 0x06a43320
    __ dci(0x04fc7539);  // sqrdmulh z25.d, z9.d, z28.d
    // vl128 state = 0x8ce1cad6
    __ dci(0x04fc7029);  // sqdmulh z9.d, z1.d, z28.d
    // vl128 state = 0x6f3d1b22
    __ dci(0x04ac702d);  // sqdmulh z13.s, z1.s, z12.s
    // vl128 state = 0x14b0451c
    __ dci(0x04a4742c);  // sqrdmulh z12.s, z1.s, z4.s
    // vl128 state = 0x60206a6a
    __ dci(0x04a574ad);  // sqrdmulh z13.s, z5.s, z5.s
    // vl128 state = 0x388a9786
    __ dci(0x04a574a9);  // sqrdmulh z9.s, z5.s, z5.s
    // vl128 state = 0xee590c43
    __ dci(0x04e574e8);  // sqrdmulh z8.d, z7.d, z5.d
    // vl128 state = 0x8d16295c
    __ dci(0x04e570ca);  // sqdmulh z10.d, z6.d, z5.d
    // vl128 state = 0x2a5c234c
    __ dci(0x04e670cb);  // sqdmulh z11.d, z6.d, z6.d
    // vl128 state = 0xfacc9e06
    __ dci(0x04f6708f);  // sqdmulh z15.d, z4.d, z22.d
    // vl128 state = 0x2167ca56
    __ dci(0x04f67087);  // sqdmulh z7.d, z4.d, z22.d
    // vl128 state = 0xc7d7af1d
    __ dci(0x04f77185);  // sqdmulh z5.d, z12.d, z23.d
    // vl128 state = 0x15f82ac2
    __ dci(0x04f67104);  // sqdmulh z4.d, z8.d, z22.d
    // vl128 state = 0xb2484707
    __ dci(0x04f6710c);  // sqdmulh z12.d, z8.d, z22.d
    // vl128 state = 0x5a53b8e7
    __ dci(0x04f6708d);  // sqdmulh z13.d, z4.d, z22.d
    // vl128 state = 0xa9affac2
    __ dci(0x04f67085);  // sqdmulh z5.d, z4.d, z22.d
    // vl128 state = 0xa425052d
    __ dci(0x04fe7281);  // sqdmulh z1.d, z20.d, z30.d
    // vl128 state = 0x1c0f565c
    __ dci(0x04ee72d1);  // sqdmulh z17.d, z22.d, z14.d
    // vl128 state = 0xff12c401
    __ dci(0x04ee7393);  // sqdmulh z19.d, z28.d, z14.d
    // vl128 state = 0xcd1d9d3a
    __ dci(0x04ec73b2);  // sqdmulh z18.d, z29.d, z12.d
    // vl128 state = 0x2aa94767
    __ dci(0x04ee73fa);  // sqdmulh z26.d, z31.d, z14.d
    // vl128 state = 0x5ca68e9c
    __ dci(0x04ef77ea);  // sqrdmulh z10.d, z31.d, z15.d
    // vl128 state = 0xe5b65473
    __ dci(0x04ff76e8);  // sqrdmulh z8.d, z23.d, z31.d
    // vl128 state = 0xcc4e8803
    __ dci(0x04fd76c9);  // sqrdmulh z9.d, z22.d, z29.d
    // vl128 state = 0x19fff884
    __ dci(0x04fd73d9);  // sqdmulh z25.d, z30.d, z29.d
    // vl128 state = 0xb99d6147
    __ dci(0x04e973dd);  // sqdmulh z29.d, z30.d, z9.d
    // vl128 state = 0xe8f11301
    __ dci(0x04b973dc);  // sqdmulh z28.s, z30.s, z25.s
    // vl128 state = 0x24af5ffe
    __ dci(0x04b177dd);  // sqrdmulh z29.s, z30.s, z17.s
    // vl128 state = 0x5c32a08e
    __ dci(0x04b177bc);  // sqrdmulh z28.s, z29.s, z17.s
    // vl128 state = 0x12c8c1c4
    __ dci(0x04f377ac);  // sqrdmulh z12.d, z29.d, z19.d
    // vl128 state = 0x7bc1f2e6
    __ dci(0x04f677ad);  // sqrdmulh z13.d, z29.d, z22.d
    // vl128 state = 0x67d2640f
    __ dci(0x04fe76af);  // sqrdmulh z15.d, z21.d, z30.d
    // vl128 state = 0x98035fbd
    __ dci(0x04ef76ae);  // sqrdmulh z14.d, z21.d, z15.d
    // vl128 state = 0x5e561fd3
    __ dci(0x04ee72ac);  // sqdmulh z12.d, z21.d, z14.d
    // vl128 state = 0xb56c3914
    __ dci(0x04ae72ee);  // sqdmulh z14.s, z23.s, z14.s
    // vl128 state = 0x6bb1c4b1
    __ dci(0x04be7266);  // sqdmulh z6.s, z19.s, z30.s
    // vl128 state = 0x5a5bdda6
    __ dci(0x04b67364);  // sqdmulh z4.s, z27.s, z22.s
    // vl128 state = 0x09a447ea
    __ dci(0x04b27165);  // sqdmulh z5.s, z11.s, z18.s
    // vl128 state = 0xee84be35
    __ dci(0x04b27175);  // sqdmulh z21.s, z11.s, z18.s
    // vl128 state = 0x84146d85
    __ dci(0x04ba7137);  // sqdmulh z23.s, z9.s, z26.s
    // vl128 state = 0x92c2e5f6
    __ dci(0x04b3713f);  // sqdmulh z31.s, z9.s, z19.s
    // vl128 state = 0xe3836fb8
    __ dci(0x04b37017);  // sqdmulh z23.s, z0.s, z19.s
    // vl128 state = 0xb5225206
    __ dci(0x04b37615);  // sqrdmulh z21.s, z16.s, z19.s
    // vl128 state = 0x157484c7
    __ dci(0x04b37491);  // sqrdmulh z17.s, z4.s, z19.s
    // vl128 state = 0x586c4bbf
    __ dci(0x04b37481);  // sqrdmulh z1.s, z4.s, z19.s
    // vl128 state = 0xf5dc07cb
    __ dci(0x04b37489);  // sqrdmulh z9.s, z4.s, z19.s
    // vl128 state = 0x591875a8
    __ dci(0x04b5748d);  // sqrdmulh z13.s, z4.s, z21.s
    // vl128 state = 0xb01f8fd5
    __ dci(0x043d748f);  // sqrdmulh z15.b, z4.b, z29.b
    // vl128 state = 0xd466a58c
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xd466a58c,
        0xe2ec7fba,
        0x1644e93a,
        0x7c3ecb2e,
        0xed4ecd78,
        0xfd5b5783,
        0xa7094efe,
        0x92bd623f,
        0x6da5e423,
        0x1648b588,
        0x63ce5947,
        0xba9c7d90,
        0x756ae20d,
        0x6d4032ba,
        0x87ae8b8f,
        0x722b2f6f,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_cmla_index) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x44e867e6);  // cmla z6.s, z31.s, z8.s[0], #90
    // vl128 state = 0xee56e69b
    __ dci(0x44e86de4);  // cmla z4.s, z15.s, z8.s[0], #270
    // vl128 state = 0x0ed2e9f5
    __ dci(0x44e86be5);  // cmla z5.s, z31.s, z8.s[0], #180
    // vl128 state = 0x9074e2a6
    __ dci(0x44eb6bf5);  // cmla z21.s, z31.s, z11.s[0], #180
    // vl128 state = 0x8f43b8a8
    __ dci(0x44eb6b31);  // cmla z17.s, z25.s, z11.s[0], #180
    // vl128 state = 0xb6c51b97
    __ dci(0x44eb6135);  // cmla z21.s, z9.s, z11.s[0], #0
    // vl128 state = 0x4236beed
    __ dci(0x44e9633d);  // cmla z29.s, z25.s, z9.s[0], #0
    // vl128 state = 0x21879fe6
    __ dci(0x44f96379);  // cmla z25.s, z27.s, z9.s[1], #0
    // vl128 state = 0x78172805
    __ dci(0x44fd6349);  // cmla z9.s, z26.s, z13.s[1], #0
    // vl128 state = 0x242a3ae5
    __ dci(0x44f76341);  // cmla z1.s, z26.s, z7.s[1], #0
    // vl128 state = 0xa734ef3b
    __ dci(0x44f36305);  // cmla z5.s, z24.s, z3.s[1], #0
    // vl128 state = 0x00a035b1
    __ dci(0x44f76381);  // cmla z1.s, z28.s, z7.s[1], #0
    // vl128 state = 0xbdfda3d4
    __ dci(0x44f763e3);  // cmla z3.s, z31.s, z7.s[1], #0
    // vl128 state = 0xe1ed6ed9
    __ dci(0x44b763cb);  // cmla z11.h, z30.h, z7.h[2], #0
    // vl128 state = 0xae645ea8
    __ dci(0x44a763e9);  // cmla z9.h, z31.h, z7.h[0], #0
    // vl128 state = 0x392b3511
    __ dci(0x44a762ab);  // cmla z11.h, z21.h, z7.h[0], #0
    // vl128 state = 0x3a05f729
    __ dci(0x44a66aaf);  // cmla z15.h, z21.h, z6.h[0], #180
    // vl128 state = 0x7cfa0c08
    __ dci(0x44a66aa7);  // cmla z7.h, z21.h, z6.h[0], #180
    // vl128 state = 0x91749f43
    __ dci(0x44a663a5);  // cmla z5.h, z29.h, z6.h[0], #0
    // vl128 state = 0x438479ab
    __ dci(0x44a66bed);  // cmla z13.h, z31.h, z6.h[0], #180
    // vl128 state = 0xc25ce86d
    __ dci(0x44f66be9);  // cmla z9.s, z31.s, z6.s[1], #180
    // vl128 state = 0x6e8bdeca
    __ dci(0x44b66bd9);  // cmla z25.h, z30.h, z6.h[2], #180
    // vl128 state = 0x04745a63
    __ dci(0x44b66bd8);  // cmla z24.h, z30.h, z6.h[2], #180
    // vl128 state = 0xbfc59a82
    __ dci(0x44b66b7c);  // cmla z28.h, z27.h, z6.h[2], #180
    // vl128 state = 0x12d70fc2
    __ dci(0x44b6617e);  // cmla z30.h, z11.h, z6.h[2], #0
    // vl128 state = 0x53f4b9a1
    __ dci(0x44b7697c);  // cmla z28.h, z11.h, z7.h[2], #180
    // vl128 state = 0x74e99c24
    __ dci(0x44b3692c);  // cmla z12.h, z9.h, z3.h[2], #180
    // vl128 state = 0xdc80a875
    __ dci(0x44a1692e);  // cmla z14.h, z9.h, z1.h[0], #180
    // vl128 state = 0x307af313
    __ dci(0x44b169af);  // cmla z15.h, z13.h, z1.h[2], #180
    // vl128 state = 0xc92b23fe
    __ dci(0x44b165a7);  // cmla z7.h, z13.h, z1.h[2], #90
    // vl128 state = 0x33a52d1c
    __ dci(0x44b165a5);  // cmla z5.h, z13.h, z1.h[2], #90
    // vl128 state = 0xbc53ebfc
    __ dci(0x44f161a1);  // cmla z1.s, z13.s, z1.s[1], #0
    // vl128 state = 0x7ba34076
    __ dci(0x44f261a0);  // cmla z0.s, z13.s, z2.s[1], #0
    // vl128 state = 0x6fa2bab8
    __ dci(0x44b361b0);  // cmla z16.h, z13.h, z3.h[2], #0
    // vl128 state = 0xaae67807
    __ dci(0x44b36092);  // cmla z18.h, z4.h, z3.h[2], #0
    // vl128 state = 0xf1b05dff
    __ dci(0x44b36202);  // cmla z2.h, z16.h, z3.h[2], #0
    // vl128 state = 0xd226bf15
    __ dci(0x44b36a20);  // cmla z0.h, z17.h, z3.h[2], #180
    // vl128 state = 0x6a8ade58
    __ dci(0x44b26a10);  // cmla z16.h, z16.h, z2.h[2], #180
    // vl128 state = 0x075e00e4
    __ dci(0x44b26a18);  // cmla z24.h, z16.h, z2.h[2], #180
    // vl128 state = 0x9bcef7bd
    __ dci(0x44b06a28);  // cmla z8.h, z17.h, z0.h[2], #180
    // vl128 state = 0x8ac6d4b3
    __ dci(0x44b06a2a);  // cmla z10.h, z17.h, z0.h[2], #180
    // vl128 state = 0x51993d51
    __ dci(0x44b0620b);  // cmla z11.h, z16.h, z0.h[2], #0
    // vl128 state = 0x6d134734
    __ dci(0x44b06209);  // cmla z9.h, z16.h, z0.h[2], #0
    // vl128 state = 0x0ee4031f
    __ dci(0x44f06a0d);  // cmla z13.s, z16.s, z0.s[1], #180
    // vl128 state = 0x08ea247b
    __ dci(0x44f06b2c);  // cmla z12.s, z25.s, z0.s[1], #180
    // vl128 state = 0x6acbb19a
    __ dci(0x44f1692d);  // cmla z13.s, z9.s, z1.s[1], #180
    // vl128 state = 0x3ea2d161
    __ dci(0x44b36925);  // cmla z5.h, z9.h, z3.h[2], #180
    // vl128 state = 0x5b962e9b
    __ dci(0x44b36921);  // cmla z1.h, z9.h, z3.h[2], #180
    // vl128 state = 0x029f0eca
    __ dci(0x44b36d69);  // cmla z9.h, z11.h, z3.h[2], #270
    // vl128 state = 0x39a63c65
    __ dci(0x44bb6d28);  // cmla z8.h, z9.h, z3.h[3], #270
    // vl128 state = 0x6d58c136
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x6d58c136,
        0xfbdbae97,
        0x85c3cf1a,
        0xe4b53177,
        0x2f714586,
        0xde1afee8,
        0xd9613d2e,
        0x842c85a6,
        0xdc285523,
        0xccba7ba9,
        0x79e1e6f7,
        0xb19427f4,
        0x20d08a3a,
        0xfb7f4c43,
        0x0721ed60,
        0x4ee795ab,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_flogb) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x651cb31e);  // flogb z30.s, p4/m, z24.s
    // vl128 state = 0x161f1855
    __ dci(0x651cb3ae);  // flogb z14.s, p4/m, z29.s
    // vl128 state = 0xf9e5ce4d
    __ dci(0x651cb3be);  // flogb z30.s, p4/m, z29.s
    // vl128 state = 0xa06176bc
    __ dci(0x651ea3bc);  // flogb z28.d, p0/m, z29.d
    // vl128 state = 0xf793f7bb
    __ dci(0x651ea3cc);  // flogb z12.d, p0/m, z30.d
    // vl128 state = 0xe5d71081
    __ dci(0x651ea3dc);  // flogb z28.d, p0/m, z30.d
    // vl128 state = 0x33ffc09f
    __ dci(0x651ea3d4);  // flogb z20.d, p0/m, z30.d
    // vl128 state = 0xd908a72e
    __ dci(0x651ea3d5);  // flogb z21.d, p0/m, z30.d
    // vl128 state = 0x9528251a
    __ dci(0x651ca394);  // flogb z20.s, p0/m, z28.s
    // vl128 state = 0xb1ac4188
    __ dci(0x651ca396);  // flogb z22.s, p0/m, z28.s
    // vl128 state = 0xdc328726
    __ dci(0x651ca1d7);  // flogb z23.s, p0/m, z14.s
    // vl128 state = 0xfc232eb7
    __ dci(0x651ca947);  // flogb z7.s, p2/m, z10.s
    // vl128 state = 0xa9c53a1a
    __ dci(0x651ca805);  // flogb z5.s, p2/m, z0.s
    // vl128 state = 0x9e4a47e9
    __ dci(0x651ea841);  // flogb z1.d, p2/m, z2.d
    // vl128 state = 0x7a2aeaf6
    __ dci(0x651ea843);  // flogb z3.d, p2/m, z2.d
    // vl128 state = 0xedd4aa97
    __ dci(0x651caa4b);  // flogb z11.s, p2/m, z18.s
    // vl128 state = 0x7bfefefb
    __ dci(0x651cab6f);  // flogb z15.s, p2/m, z27.s
    // vl128 state = 0x91b5a183
    __ dci(0x651ca86b);  // flogb z11.s, p2/m, z3.s
    // vl128 state = 0x7b2776c2
    __ dci(0x651ca47b);  // flogb z27.s, p1/m, z3.s
    // vl128 state = 0x46ea46c7
    __ dci(0x651ca47f);  // flogb z31.s, p1/m, z3.s
    // vl128 state = 0x6e1d4e89
    __ dci(0x651ca477);  // flogb z23.s, p1/m, z3.s
    // vl128 state = 0x5ea1220c
    __ dci(0x651ca035);  // flogb z21.s, p0/m, z1.s
    // vl128 state = 0xb06e32be
    __ dci(0x651ca2a5);  // flogb z5.s, p0/m, z21.s
    // vl128 state = 0xb856d206
    __ dci(0x651caa2d);  // flogb z13.s, p2/m, z17.s
    // vl128 state = 0xebfd587f
    __ dci(0x651caa3d);  // flogb z29.s, p2/m, z17.s
    // vl128 state = 0xb029ba8d
    __ dci(0x651eaa7f);  // flogb z31.d, p2/m, z19.d
    // vl128 state = 0x07fd3f42
    __ dci(0x651ebb7e);  // flogb z30.d, p6/m, z27.d
    // vl128 state = 0x79761d7a
    __ dci(0x651ebb76);  // flogb z22.d, p6/m, z27.d
    // vl128 state = 0xdf56dd22
    __ dci(0x651ebb72);  // flogb z18.d, p6/m, z27.d
    // vl128 state = 0xce798ad7
    __ dci(0x651eb276);  // flogb z22.d, p4/m, z19.d
    // vl128 state = 0x84dd46d6
    __ dci(0x651eb652);  // flogb z18.d, p5/m, z18.d
    // vl128 state = 0x2ea4a0df
    __ dci(0x651cbe42);  // flogb z2.s, p7/m, z18.s
    // vl128 state = 0x8cdd1250
    __ dci(0x651cb852);  // flogb z18.s, p6/m, z2.s
    // vl128 state = 0x5f5b051d
    __ dci(0x651eb956);  // flogb z22.d, p6/m, z10.d
    // vl128 state = 0x7a17cdd1
    __ dci(0x651eb11e);  // flogb z30.d, p4/m, z8.d
    // vl128 state = 0x7367f8ec
    __ dci(0x651ab016);  // flogb z22.h, p4/m, z0.h
    // vl128 state = 0x8e1bfb06
    __ dci(0x651ab014);  // flogb z20.h, p4/m, z0.h
    // vl128 state = 0x2bcfa0f0
    __ dci(0x651aa81c);  // flogb z28.h, p2/m, z0.h
    // vl128 state = 0xeb9615e8
    __ dci(0x651aa80c);  // flogb z12.h, p2/m, z0.h
    // vl128 state = 0x5b55f5cd
    __ dci(0x651aa808);  // flogb z8.h, p2/m, z0.h
    // vl128 state = 0xdd1718f2
    __ dci(0x651aa20a);  // flogb z10.h, p0/m, z16.h
    // vl128 state = 0x205e88ed
    __ dci(0x651ab24e);  // flogb z14.h, p4/m, z18.h
    // vl128 state = 0x1c9f2035
    __ dci(0x651ab36f);  // flogb z15.h, p4/m, z27.h
    // vl128 state = 0xea22efaf
    __ dci(0x651ab36b);  // flogb z11.h, p4/m, z27.h
    // vl128 state = 0x0cd0b8cd
    __ dci(0x651abb29);  // flogb z9.h, p6/m, z25.h
    // vl128 state = 0xa1a017d1
    __ dci(0x651abb2d);  // flogb z13.h, p6/m, z25.h
    // vl128 state = 0x37d033d2
    __ dci(0x651aba0c);  // flogb z12.h, p6/m, z16.h
    // vl128 state = 0x971bde83
    __ dci(0x651cba1c);  // flogb z28.s, p6/m, z16.s
    // vl128 state = 0xb6b23bc2
    __ dci(0x651cba1d);  // flogb z29.s, p6/m, z16.s
    // vl128 state = 0x1af298e0
    __ dci(0x651cba15);  // flogb z21.s, p6/m, z16.s
    // vl128 state = 0x077a2869
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x077a2869,
        0xde5bc452,
        0xe80f0bc6,
        0x1c078cf2,
        0x66064034,
        0xa9f5264d,
        0xb19b24c1,
        0xb394864c,
        0x42991ea7,
        0xcf33094e,
        0xc4656d85,
        0x4cfa5b7e,
        0xbb7c121f,
        0xd2e8c839,
        0x028134cf,
        0x2f3e9779,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_fp_pair) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm, kFpInputSet);
  // state = 0x1e5cbcac

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x64d591aa);  // fminnmp z10.d, p4/m, z10.d, z13.d
    // vl128 state = 0x02a0f18c
    __ dci(0x64d59dab);  // fminnmp z11.d, p7/m, z11.d, z13.d
    // vl128 state = 0xd6d0a87f
    __ dci(0x64d59d7b);  // fminnmp z27.d, p7/m, z27.d, z11.d
    // vl128 state = 0x364f93b4
    __ dci(0x64d59c2b);  // fminnmp z11.d, p7/m, z11.d, z1.d
    // vl128 state = 0xc7ed7476
    __ dci(0x64d59f23);  // fminnmp z3.d, p7/m, z3.d, z25.d
    // vl128 state = 0x7a1ec868
    __ dci(0x64d59f22);  // fminnmp z2.d, p7/m, z2.d, z25.d
    // vl128 state = 0x862a3f3d
    __ dci(0x64d49fa0);  // fmaxnmp z0.d, p7/m, z0.d, z29.d
    // vl128 state = 0x11f71743
    __ dci(0x64d49fa8);  // fmaxnmp z8.d, p7/m, z8.d, z29.d
    // vl128 state = 0x302e45cd
    __ dci(0x64d49fa9);  // fmaxnmp z9.d, p7/m, z9.d, z29.d
    // vl128 state = 0x11cca180
    __ dci(0x64d68fb9);  // fmaxp z25.d, p3/m, z25.d, z29.d
    // vl128 state = 0xee6b2d42
    __ dci(0x64d68fb8);  // fmaxp z24.d, p3/m, z24.d, z29.d
    // vl128 state = 0x060efb2c
    __ dci(0x64d49fba);  // fmaxnmp z26.d, p7/m, z26.d, z29.d
    // vl128 state = 0x4f4232ac
    __ dci(0x649497b2);  // fmaxnmp z18.s, p5/m, z18.s, z29.s
    // vl128 state = 0xe3e04479
    __ dci(0x649096b6);  // faddp z22.s, p5/m, z22.s, z21.s
    // vl128 state = 0x2a407146
    __ dci(0x64909237);  // faddp z23.s, p4/m, z23.s, z17.s
    // vl128 state = 0x6d0b2bb8
    __ dci(0x64d09027);  // faddp z7.d, p4/m, z7.d, z1.d
    // vl128 state = 0x5e7d175f
    __ dci(0x64509006);  // faddp z6.h, p4/m, z6.h, z0.h
    // vl128 state = 0xa0a4cd20
    __ dci(0x64d0940e);  // faddp z14.d, p5/m, z14.d, z0.d
    // vl128 state = 0xf66b9cde
    __ dci(0x64d09c4f);  // faddp z15.d, p7/m, z15.d, z2.d
    // vl128 state = 0x5a2d08c9
    __ dci(0x64d09c5f);  // faddp z31.d, p7/m, z31.d, z2.d
    // vl128 state = 0x2e390409
    __ dci(0x64d09c57);  // faddp z23.d, p7/m, z23.d, z2.d
    // vl128 state = 0xfb4af476
    __ dci(0x64d09c56);  // faddp z22.d, p7/m, z22.d, z2.d
    // vl128 state = 0x8d8c621b
    __ dci(0x64d08e5e);  // faddp z30.d, p3/m, z30.d, z18.d
    // vl128 state = 0xba8962e6
    __ dci(0x64d0845c);  // faddp z28.d, p1/m, z28.d, z2.d
    // vl128 state = 0x224654c6
    __ dci(0x64d0845d);  // faddp z29.d, p1/m, z29.d, z2.d
    // vl128 state = 0xef608134
    __ dci(0x64d08e4d);  // faddp z13.d, p3/m, z13.d, z18.d
    // vl128 state = 0x5adedbf3
    __ dci(0x64908645);  // faddp z5.s, p1/m, z5.s, z18.s
    // vl128 state = 0x04b4f366
    __ dci(0x64908a4d);  // faddp z13.s, p2/m, z13.s, z18.s
    // vl128 state = 0xf0a7482a
    __ dci(0x64d08245);  // faddp z5.d, p0/m, z5.d, z18.d
    // vl128 state = 0x0f2ccd61
    __ dci(0x64909255);  // faddp z21.s, p4/m, z21.s, z18.s
    // vl128 state = 0x7665491f
    __ dci(0x649096c5);  // faddp z5.s, p5/m, z5.s, z22.s
    // vl128 state = 0xc3b53fd3
    __ dci(0x649492c1);  // fmaxnmp z1.s, p4/m, z1.s, z22.s
    // vl128 state = 0x589fd64a
    __ dci(0x649096d1);  // faddp z17.s, p5/m, z17.s, z22.s
    // vl128 state = 0x5a0d0d52
    __ dci(0x649096d5);  // faddp z21.s, p5/m, z21.s, z22.s
    // vl128 state = 0xba57cd51
    __ dci(0x649096d4);  // faddp z20.s, p5/m, z20.s, z22.s
    // vl128 state = 0xa5d7b29d
    __ dci(0x649093d0);  // faddp z16.s, p4/m, z16.s, z30.s
    // vl128 state = 0xa62cce9e
    __ dci(0x64909318);  // faddp z24.s, p4/m, z24.s, z24.s
    // vl128 state = 0x8cc209c7
    __ dci(0x64909008);  // faddp z8.s, p4/m, z8.s, z0.s
    // vl128 state = 0x56a9af04
    __ dci(0x64969000);  // fmaxp z0.s, p4/m, z0.s, z0.s
    // vl128 state = 0xc45f824a
    __ dci(0x64569004);  // fmaxp z4.h, p4/m, z4.h, z0.h
    // vl128 state = 0x82da5cb7
    __ dci(0x64569000);  // fmaxp z0.h, p4/m, z0.h, z0.h
    // vl128 state = 0xa9fff0bf
    __ dci(0x64569001);  // fmaxp z1.h, p4/m, z1.h, z0.h
    // vl128 state = 0x71c2e09a
    __ dci(0x64569605);  // fmaxp z5.h, p5/m, z5.h, z16.h
    // vl128 state = 0xe50c8b49
    __ dci(0x64579624);  // fminp z4.h, p5/m, z4.h, z17.h
    // vl128 state = 0x4f3817cb
    __ dci(0x6457962c);  // fminp z12.h, p5/m, z12.h, z17.h
    // vl128 state = 0x5a773e57
    __ dci(0x64d5963c);  // fminnmp z28.d, p5/m, z28.d, z17.d
    // vl128 state = 0xa5c5e37c
    __ dci(0x64d7943e);  // fminp z30.d, p5/m, z30.d, z1.d
    // vl128 state = 0xc778f8a3
    __ dci(0x6457953a);  // fminp z26.h, p5/m, z26.h, z9.h
    // vl128 state = 0x01abc4af
    __ dci(0x6457952a);  // fminp z10.h, p5/m, z10.h, z9.h
    // vl128 state = 0x45483a17
    __ dci(0x64579d7a);  // fminp z26.h, p7/m, z26.h, z11.h
    // vl128 state = 0x355b08b3
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x355b08b3,
        0x8f7890cd,
        0x5dddb069,
        0x030a5f52,
        0xc569c150,
        0x060423ba,
        0x5d729bd0,
        0x079b4f8b,
        0x06e75e58,
        0x6f631884,
        0xddc735f0,
        0x7213b8e2,
        0x8cbf507c,
        0x40654268,
        0x3cd7ad6c,
        0xfba0ee9e,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_fmlal_fmlsl_index) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x64a94f15);  // fmlalt z21.s, z24.h, z1.h[3]
    // vl128 state = 0x0895849b
    __ dci(0x64ab4f9d);  // fmlalt z29.s, z28.h, z3.h[3]
    // vl128 state = 0x6e0cf3fe
    __ dci(0x64a74f9c);  // fmlalt z28.s, z28.h, z7.h[1]
    // vl128 state = 0x482b4f57
    __ dci(0x64a74dde);  // fmlalt z30.s, z14.h, z7.h[1]
    // vl128 state = 0xf047791e
    __ dci(0x64a74cee);  // fmlalt z14.s, z7.h, z7.h[1]
    // vl128 state = 0xde33332c
    __ dci(0x64a648ef);  // fmlalb z15.s, z7.h, z6.h[1]
    // vl128 state = 0xf7148941
    __ dci(0x64a648ee);  // fmlalb z14.s, z7.h, z6.h[1]
    // vl128 state = 0x69f23fcb
    __ dci(0x64b649ea);  // fmlalb z10.s, z15.h, z6.h[5]
    // vl128 state = 0x979eea1a
    __ dci(0x64b649ee);  // fmlalb z14.s, z15.h, z6.h[5]
    // vl128 state = 0x522917a9
    __ dci(0x64b649e6);  // fmlalb z6.s, z15.h, z6.h[5]
    // vl128 state = 0x7d773525
    __ dci(0x64b64ba2);  // fmlalb z2.s, z29.h, z6.h[5]
    // vl128 state = 0x220960c6
    __ dci(0x64b46baa);  // fmlslb z10.s, z29.h, z4.h[5]
    // vl128 state = 0x2c8e384a
    __ dci(0x64b46dab);  // fmlslt z11.s, z13.h, z4.h[5]
    // vl128 state = 0xa592cde1
    __ dci(0x64b467bb);  // fmlslt z27.s, z29.h, z4.h[4]
    // vl128 state = 0xba31bd61
    __ dci(0x64b665b3);  // fmlslt z19.s, z13.h, z6.h[4]
    // vl128 state = 0x75dade04
    __ dci(0x64b663bb);  // fmlslb z27.s, z29.h, z6.h[4]
    // vl128 state = 0xa7358466
    __ dci(0x64a662bf);  // fmlslb z31.s, z21.h, z6.h[0]
    // vl128 state = 0x6125ca9d
    __ dci(0x64a7623e);  // fmlslb z30.s, z17.h, z7.h[0]
    // vl128 state = 0x4b1cda83
    __ dci(0x64a7462e);  // fmlalt z14.s, z17.h, z7.h[0]
    // vl128 state = 0x00d73a44
    __ dci(0x64a6662f);  // fmlslt z15.s, z17.h, z6.h[0]
    // vl128 state = 0xc5ea9f30
    __ dci(0x64a666ed);  // fmlslt z13.s, z23.h, z6.h[0]
    // vl128 state = 0xe17ba118
    __ dci(0x64a26eec);  // fmlslt z12.s, z23.h, z2.h[1]
    // vl128 state = 0xd1962c7a
    __ dci(0x64a26cbc);  // fmlslt z28.s, z5.h, z2.h[1]
    // vl128 state = 0xde6f1ace
    __ dci(0x64a26cb4);  // fmlslt z20.s, z5.h, z2.h[1]
    // vl128 state = 0x10d69920
    __ dci(0x64a26cbc);  // fmlslt z28.s, z5.h, z2.h[1]
    // vl128 state = 0x8d190aec
    __ dci(0x64a26cd8);  // fmlslt z24.s, z6.h, z2.h[1]
    // vl128 state = 0x432fdda3
    __ dci(0x64a26c1a);  // fmlslt z26.s, z0.h, z2.h[1]
    // vl128 state = 0x9ababf0a
    __ dci(0x64a24d1e);  // fmlalt z30.s, z8.h, z2.h[1]
    // vl128 state = 0x609040ae
    __ dci(0x64a24d1c);  // fmlalt z28.s, z8.h, z2.h[1]
    // vl128 state = 0x0a047710
    __ dci(0x64a24d1e);  // fmlalt z30.s, z8.h, z2.h[1]
    // vl128 state = 0xf273945a
    __ dci(0x64a0490e);  // fmlalb z14.s, z8.h, z0.h[1]
    // vl128 state = 0x3a5456f1
    __ dci(0x64a0490c);  // fmlalb z12.s, z8.h, z0.h[1]
    // vl128 state = 0xdb948daf
    __ dci(0x64b04b04);  // fmlalb z4.s, z24.h, z0.h[5]
    // vl128 state = 0xd2eae2af
    __ dci(0x64b04b06);  // fmlalb z6.s, z24.h, z0.h[5]
    // vl128 state = 0x26627a2c
    __ dci(0x64b04b07);  // fmlalb z7.s, z24.h, z0.h[5]
    // vl128 state = 0x2841173d
    __ dci(0x64b84b26);  // fmlalb z6.s, z25.h, z0.h[7]
    // vl128 state = 0x9b52bcc6
    __ dci(0x64ba4f27);  // fmlalt z7.s, z25.h, z2.h[7]
    // vl128 state = 0x813bbabe
    __ dci(0x64ba4923);  // fmlalb z3.s, z9.h, z2.h[7]
    // vl128 state = 0xbb608dad
    __ dci(0x64b84d22);  // fmlalt z2.s, z9.h, z0.h[7]
    // vl128 state = 0xf4d84ed6
    __ dci(0x64b84d23);  // fmlalt z3.s, z9.h, z0.h[7]
    // vl128 state = 0x1cc0784e
    __ dci(0x64bc4527);  // fmlalt z7.s, z9.h, z4.h[6]
    // vl128 state = 0x4eece4b7
    __ dci(0x64bc6737);  // fmlslt z23.s, z25.h, z4.h[6]
    // vl128 state = 0x00dacf34
    __ dci(0x64bc6fa7);  // fmlslt z7.s, z29.h, z4.h[7]
    // vl128 state = 0x597e23d4
    __ dci(0x64bc6e25);  // fmlslt z5.s, z17.h, z4.h[7]
    // vl128 state = 0xa66b843c
    __ dci(0x64be6f2d);  // fmlslt z13.s, z25.h, z6.h[7]
    // vl128 state = 0xb595ec08
    __ dci(0x64be6765);  // fmlslt z5.s, z27.h, z6.h[6]
    // vl128 state = 0xd6c3af0a
    __ dci(0x64be662d);  // fmlslt z13.s, z17.h, z6.h[6]
    // vl128 state = 0x864f26a8
    __ dci(0x64bf6225);  // fmlslb z5.s, z17.h, z7.h[6]
    // vl128 state = 0xb969be4d
    __ dci(0x64bb626d);  // fmlslb z13.s, z19.h, z3.h[6]
    // vl128 state = 0x73329b58
    __ dci(0x64b9622c);  // fmlslb z12.s, z17.h, z1.h[6]
    // vl128 state = 0xfb7e2da2
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xfb7e2da2,
        0x34ad546c,
        0xd914c0d4,
        0xc173287c,
        0x07db96b2,
        0xab5ece8c,
        0xcda13318,
        0x6e62dc3f,
        0x0268d9b4,
        0x15118567,
        0xf55fb24f,
        0xc4ab4b56,
        0x5911f225,
        0x6d9c320c,
        0xc69bdedf,
        0x1635a43f,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_fp_convert) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x640ab3ee);  // fcvtxnt z14.s, p4/m, z31.d
    // vl128 state = 0x3ea71f7a
    __ dci(0x64caa9e0);  // fcvtnt z0.s, p2/m, z15.d
    // vl128 state = 0xe9d750a1
    __ dci(0x64cab83d);  // fcvtnt z29.s, p6/m, z1.d
    // vl128 state = 0x9ce43257
    __ dci(0x650aad62);  // fcvtx z2.s, p3/m, z11.d
    // vl128 state = 0x60283e22
    __ dci(0x64cbb42b);  // fcvtlt z11.d, p5/m, z1.s
    // vl128 state = 0xfbecbe4a
    __ dci(0x6488ba54);  // fcvtnt z20.h, p6/m, z18.s
    // vl128 state = 0xbb81cc05
    __ dci(0x64cbb730);  // fcvtlt z16.d, p5/m, z25.s
    // vl128 state = 0xd9cebdf5
    __ dci(0x640aa5e4);  // fcvtxnt z4.s, p1/m, z15.d
    // vl128 state = 0x9dba64db
    __ dci(0x650aa715);  // fcvtx z21.s, p1/m, z24.d
    // vl128 state = 0x0e68fab9
    __ dci(0x64cabe86);  // fcvtnt z6.s, p7/m, z20.d
    // vl128 state = 0x5936ac32
    __ dci(0x64cba075);  // fcvtlt z21.d, p0/m, z3.s
    // vl128 state = 0x2eb8a37b
    __ dci(0x6488b3c5);  // fcvtnt z5.h, p4/m, z30.s
    // vl128 state = 0x9f471340
    __ dci(0x6489b24a);  // fcvtlt z10.s, p4/m, z18.h
    // vl128 state = 0xcf5e5808
    __ dci(0x64cbb514);  // fcvtlt z20.d, p5/m, z8.s
    // vl128 state = 0x870c5b85
    __ dci(0x650ab090);  // fcvtx z16.s, p4/m, z4.d
    // vl128 state = 0x305da0a0
    __ dci(0x64cbb2d3);  // fcvtlt z19.d, p4/m, z22.s
    // vl128 state = 0x8eb1b5fc
    __ dci(0x64cbb093);  // fcvtlt z19.d, p4/m, z4.s
    // vl128 state = 0x3c070332
    __ dci(0x6488b9b8);  // fcvtnt z24.h, p6/m, z13.s
    // vl128 state = 0xe0fc3455
    __ dci(0x650aa64d);  // fcvtx z13.s, p1/m, z18.d
    // vl128 state = 0x65556c34
    __ dci(0x6488b2d7);  // fcvtnt z23.h, p4/m, z22.s
    // vl128 state = 0xc9ccae47
    __ dci(0x650ab36d);  // fcvtx z13.s, p4/m, z27.d
    // vl128 state = 0x31d942a1
    __ dci(0x650aba2c);  // fcvtx z12.s, p6/m, z17.d
    // vl128 state = 0x27497e26
    __ dci(0x650aa377);  // fcvtx z23.s, p0/m, z27.d
    // vl128 state = 0xbe0a7446
    __ dci(0x6489a3a5);  // fcvtlt z5.s, p0/m, z29.h
    // vl128 state = 0x454c62cc
    __ dci(0x64cabeb9);  // fcvtnt z25.s, p7/m, z21.d
    // vl128 state = 0x808a014f
    __ dci(0x6489b4c2);  // fcvtlt z2.s, p5/m, z6.h
    // vl128 state = 0x55ae2250
    __ dci(0x64cba246);  // fcvtlt z6.d, p0/m, z18.s
    // vl128 state = 0x7ce05c24
    __ dci(0x650ab2a6);  // fcvtx z6.s, p4/m, z21.d
    // vl128 state = 0xa26121f5
    __ dci(0x64cbb239);  // fcvtlt z25.d, p4/m, z17.s
    // vl128 state = 0xb40c58e1
    __ dci(0x64cabdd9);  // fcvtnt z25.s, p7/m, z14.d
    // vl128 state = 0xf5077a54
    __ dci(0x650ab75a);  // fcvtx z26.s, p5/m, z26.d
    // vl128 state = 0x95b006de
    __ dci(0x650aa08b);  // fcvtx z11.s, p0/m, z4.d
    // vl128 state = 0x9ca5060c
    __ dci(0x640aafd3);  // fcvtxnt z19.s, p3/m, z30.d
    // vl128 state = 0x85c89705
    __ dci(0x64caaf3a);  // fcvtnt z26.s, p3/m, z25.d
    // vl128 state = 0x6b6aa4f9
    __ dci(0x640abda1);  // fcvtxnt z1.s, p7/m, z13.d
    // vl128 state = 0x769cf76e
    __ dci(0x6489a6f9);  // fcvtlt z25.s, p1/m, z23.h
    // vl128 state = 0x0a291b3b
    __ dci(0x6489b38d);  // fcvtlt z13.s, p4/m, z28.h
    // vl128 state = 0x6b72e558
    __ dci(0x650aaf63);  // fcvtx z3.s, p3/m, z27.d
    // vl128 state = 0xf4a004e0
    __ dci(0x6488bfa4);  // fcvtnt z4.h, p7/m, z29.s
    // vl128 state = 0xe01c349e
    __ dci(0x6489a6ee);  // fcvtlt z14.s, p1/m, z23.h
    // vl128 state = 0x3b06da53
    __ dci(0x64cabbf8);  // fcvtnt z24.s, p6/m, z31.d
    // vl128 state = 0xc60fbbf0
    __ dci(0x6489bc7f);  // fcvtlt z31.s, p7/m, z3.h
    // vl128 state = 0x8b281c78
    __ dci(0x64caaf1f);  // fcvtnt z31.s, p3/m, z24.d
    // vl128 state = 0x0f17afbb
    __ dci(0x650aac71);  // fcvtx z17.s, p3/m, z3.d
    // vl128 state = 0xce0ac3e1
    __ dci(0x650aa1df);  // fcvtx z31.s, p0/m, z14.d
    // vl128 state = 0x71ba2085
    __ dci(0x650aaf9f);  // fcvtx z31.s, p3/m, z28.d
    // vl128 state = 0xe42caea0
    __ dci(0x640abff9);  // fcvtxnt z25.s, p7/m, z31.d
    // vl128 state = 0xec3c032c
    __ dci(0x6489b8e5);  // fcvtlt z5.s, p6/m, z7.h
    // vl128 state = 0xe41850f7
    __ dci(0x640aa1a1);  // fcvtxnt z1.s, p0/m, z13.d
    // vl128 state = 0xaf3944b4
    __ dci(0x6488bf41);  // fcvtnt z1.h, p7/m, z26.s
    // vl128 state = 0xdffd02bd
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xdffd02bd,
        0x03d1f711,
        0x41cf3358,
        0xa351d0f6,
        0xffba25ff,
        0x14092947,
        0x26b194fe,
        0x42acd8a3,
        0xc0498960,
        0xcccf1171,
        0x8dca76ed,
        0xefbda194,
        0xcf04a23d,
        0x91e2629f,
        0xf05e8f52,
        0x4994ad4a,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_saturating_multiply_add_high_indexed) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 40 * kInstructionSize);
    __ dci(0x442b1100);  // sqrdmlah z0.h, z8.h, z3.h[1]
    // vl128 state = 0xb012d377
    __ dci(0x44211108);  // sqrdmlah z8.h, z8.h, z1.h[0]
    // vl128 state = 0xae399e50
    __ dci(0x4421110c);  // sqrdmlah z12.h, z8.h, z1.h[0]
    // vl128 state = 0x1a46b700
    __ dci(0x44291188);  // sqrdmlah z8.h, z12.h, z1.h[1]
    // vl128 state = 0x7525090a
    __ dci(0x442811a9);  // sqrdmlah z9.h, z13.h, z0.h[1]
    // vl128 state = 0xf2907eb8
    __ dci(0x442c11eb);  // sqrdmlah z11.h, z15.h, z4.h[1]
    // vl128 state = 0x65a71d51
    __ dci(0x442c11e3);  // sqrdmlah z3.h, z15.h, z4.h[1]
    // vl128 state = 0x8b30e19b
    __ dci(0x442413e1);  // sqrdmlah z1.h, z31.h, z4.h[0]
    // vl128 state = 0x448e4c0f
    __ dci(0x44a413a0);  // sqrdmlah z0.s, z29.s, z4.s[0]
    // vl128 state = 0x1745e0db
    __ dci(0x44241321);  // sqrdmlah z1.h, z25.h, z4.h[0]
    // vl128 state = 0xe07b491b
    __ dci(0x44a413a5);  // sqrdmlah z5.s, z29.s, z4.s[0]
    // vl128 state = 0xad39c91c
    __ dci(0x44e41327);  // sqrdmlah z7.d, z25.d, z4.d[0]
    // vl128 state = 0xd327dc1c
    __ dci(0x44e4132f);  // sqrdmlah z15.d, z25.d, z4.d[0]
    // vl128 state = 0x8da341ca
    __ dci(0x44e5130b);  // sqrdmlah z11.d, z24.d, z5.d[0]
    // vl128 state = 0x4dbd3ee1
    __ dci(0x44e3130a);  // sqrdmlah z10.d, z24.d, z3.d[0]
    // vl128 state = 0x71452896
    __ dci(0x44e3131a);  // sqrdmlah z26.d, z24.d, z3.d[0]
    // vl128 state = 0x4d6d8b90
    __ dci(0x4463135e);  // sqrdmlah z30.h, z26.h, z3.h[4]
    // vl128 state = 0x0b53f7b4
    __ dci(0x44e7135c);  // sqrdmlah z28.d, z26.d, z7.d[0]
    // vl128 state = 0x78ab2bb9
    __ dci(0x44e7134c);  // sqrdmlah z12.d, z26.d, z7.d[0]
    // vl128 state = 0x3773b9e2
    __ dci(0x44e51144);  // sqrdmlah z4.d, z10.d, z5.d[0]
    // vl128 state = 0x8f8883da
    __ dci(0x44e411c0);  // sqrdmlah z0.d, z14.d, z4.d[0]
    // vl128 state = 0xa27ef92f
    __ dci(0x44ec15c4);  // sqrdmlsh z4.d, z14.d, z12.d[0]
    // vl128 state = 0x6cea3cee
    __ dci(0x44ec14e0);  // sqrdmlsh z0.d, z7.d, z12.d[0]
    // vl128 state = 0xb5e40d5f
    __ dci(0x44ee16f0);  // sqrdmlsh z16.d, z23.d, z14.d[0]
    // vl128 state = 0xacf903eb
    __ dci(0x44ea16d4);  // sqrdmlsh z20.d, z22.d, z10.d[0]
    // vl128 state = 0x698246a6
    __ dci(0x44ea16d0);  // sqrdmlsh z16.d, z22.d, z10.d[0]
    // vl128 state = 0x58015eeb
    __ dci(0x44ea16d1);  // sqrdmlsh z17.d, z22.d, z10.d[0]
    // vl128 state = 0xdbf1d9a6
    __ dci(0x44ab16d3);  // sqrdmlsh z19.s, z22.s, z3.s[1]
    // vl128 state = 0xbde312bb
    __ dci(0x44aa17d1);  // sqrdmlsh z17.s, z30.s, z2.s[1]
    // vl128 state = 0xc033b9a1
    __ dci(0x44aa1650);  // sqrdmlsh z16.s, z18.s, z2.s[1]
    // vl128 state = 0x0e3b4c59
    __ dci(0x44aa1632);  // sqrdmlsh z18.s, z17.s, z2.s[1]
    // vl128 state = 0x6f849e01
    __ dci(0x44aa1710);  // sqrdmlsh z16.s, z24.s, z2.s[1]
    // vl128 state = 0x701e7316
    __ dci(0x44aa1711);  // sqrdmlsh z17.s, z24.s, z2.s[1]
    // vl128 state = 0xbfbc7895
    __ dci(0x44a91715);  // sqrdmlsh z21.s, z24.s, z1.s[1]
    // vl128 state = 0x2307c6f3
    __ dci(0x44a91697);  // sqrdmlsh z23.s, z20.s, z1.s[1]
    // vl128 state = 0x78db6627
    __ dci(0x44a91696);  // sqrdmlsh z22.s, z20.s, z1.s[1]
    // vl128 state = 0x37d25a35
    __ dci(0x44a816de);  // sqrdmlsh z30.s, z22.s, z0.s[1]
    // vl128 state = 0xf611db46
    __ dci(0x44ab16dc);  // sqrdmlsh z28.s, z22.s, z3.s[1]
    // vl128 state = 0x699a840f
    __ dci(0x44af165d);  // sqrdmlsh z29.s, z18.s, z7.s[1]
    // vl128 state = 0x0b5d451f
    __ dci(0x44af16f5);  // sqrdmlsh z21.s, z23.s, z7.s[1]
    // vl128 state = 0xe49e3b59
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xe49e3b59,
        0xce0062c7,
        0xf796ec27,
        0x1f952649,
        0x4e4354e6,
        0x90cb0c51,
        0xf0688aee,
        0xae9de352,
        0x652f0c0d,
        0x0000db74,
        0xdc23fff7,
        0x228c116c,
        0x8477dd7c,
        0x08377c46,
        0x6e05a40f,
        0x874126fb,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_sat_double_mul_high_index) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 50 * kInstructionSize);
    __ dci(0x447bf609);  // sqrdmulh z9.h, z16.h, z3.h[7]
    // vl128 state = 0xacad7d7c
    __ dci(0x447bf601);  // sqrdmulh z1.h, z16.h, z3.h[7]
    // vl128 state = 0xd6a976fe
    __ dci(0x447bf600);  // sqrdmulh z0.h, z16.h, z3.h[7]
    // vl128 state = 0x959d4287
    __ dci(0x446bf710);  // sqrdmulh z16.h, z24.h, z3.h[5]
    // vl128 state = 0x88b70b0e
    __ dci(0x446af612);  // sqrdmulh z18.h, z16.h, z2.h[5]
    // vl128 state = 0xea48068a
    __ dci(0x442af636);  // sqrdmulh z22.h, z17.h, z2.h[1]
    // vl128 state = 0x22135bae
    __ dci(0x442af626);  // sqrdmulh z6.h, z17.h, z2.h[1]
    // vl128 state = 0x1ed137a8
    __ dci(0x442af624);  // sqrdmulh z4.h, z17.h, z2.h[1]
    // vl128 state = 0x37aa44d4
    __ dci(0x4420f625);  // sqrdmulh z5.h, z17.h, z0.h[0]
    // vl128 state = 0x9747863a
    __ dci(0x4460f604);  // sqrdmulh z4.h, z16.h, z0.h[4]
    // vl128 state = 0xf6487f4b
    __ dci(0x4460f605);  // sqrdmulh z5.h, z16.h, z0.h[4]
    // vl128 state = 0xb85302a6
    __ dci(0x4420f641);  // sqrdmulh z1.h, z18.h, z0.h[0]
    // vl128 state = 0xfc85ce98
    __ dci(0x4424f669);  // sqrdmulh z9.h, z19.h, z4.h[0]
    // vl128 state = 0xf0b36dd3
    __ dci(0x4460f668);  // sqrdmulh z8.h, z19.h, z0.h[4]
    // vl128 state = 0x227fe9fe
    __ dci(0x4462f6f8);  // sqrdmulh z24.h, z23.h, z2.h[4]
    // vl128 state = 0x7f4d89ab
    __ dci(0x4462f6f0);  // sqrdmulh z16.h, z23.h, z2.h[4]
    // vl128 state = 0x61520386
    __ dci(0x4472f6d1);  // sqrdmulh z17.h, z22.h, z2.h[6]
    // vl128 state = 0x34d07c81
    __ dci(0x4472f250);  // sqdmulh z16.h, z18.h, z2.h[6]
    // vl128 state = 0x74313b89
    __ dci(0x44b2f254);  // sqdmulh z20.s, z18.s, z2.s[2]
    // vl128 state = 0x7acc9692
    __ dci(0x44e2f250);  // sqdmulh z16.d, z18.d, z2.d[0]
    // vl128 state = 0x3a1f908e
    __ dci(0x44e4f251);  // sqdmulh z17.d, z18.d, z4.d[0]
    // vl128 state = 0xd2ae3642
    __ dci(0x44e0f650);  // sqrdmulh z16.d, z18.d, z0.d[0]
    // vl128 state = 0x74da2dcc
    __ dci(0x44f8f640);  // sqrdmulh z0.d, z18.d, z8.d[1]
    // vl128 state = 0x0273639a
    __ dci(0x44f9f742);  // sqrdmulh z2.d, z26.d, z9.d[1]
    // vl128 state = 0x9c5062c9
    __ dci(0x44f9f7e6);  // sqrdmulh z6.d, z31.d, z9.d[1]
    // vl128 state = 0x095e8fd7
    __ dci(0x44fdf7ae);  // sqrdmulh z14.d, z29.d, z13.d[1]
    // vl128 state = 0x4ab7c261
    __ dci(0x44fdf7af);  // sqrdmulh z15.d, z29.d, z13.d[1]
    // vl128 state = 0x7913f02e
    __ dci(0x44f9f7ed);  // sqrdmulh z13.d, z31.d, z9.d[1]
    // vl128 state = 0xbbffd120
    __ dci(0x44f9f7e5);  // sqrdmulh z5.d, z31.d, z9.d[1]
    // vl128 state = 0xc9cc793f
    __ dci(0x44f5f7e4);  // sqrdmulh z4.d, z31.d, z5.d[1]
    // vl128 state = 0xc7cc2e4b
    __ dci(0x44e5f3e0);  // sqdmulh z0.d, z31.d, z5.d[0]
    // vl128 state = 0x8a4efda7
    __ dci(0x44e4f364);  // sqdmulh z4.d, z27.d, z4.d[0]
    // vl128 state = 0xfa30239a
    __ dci(0x44edf366);  // sqdmulh z6.d, z27.d, z13.d[0]
    // vl128 state = 0x9c538671
    __ dci(0x44adf322);  // sqdmulh z2.s, z25.s, z5.s[1]
    // vl128 state = 0xafb03157
    __ dci(0x44adf263);  // sqdmulh z3.s, z19.s, z5.s[1]
    // vl128 state = 0x6ea1e1ff
    __ dci(0x44bdf22b);  // sqdmulh z11.s, z17.s, z5.s[3]
    // vl128 state = 0x0040a3a0
    __ dci(0x44adf62a);  // sqrdmulh z10.s, z17.s, z5.s[1]
    // vl128 state = 0x8b3e6419
    __ dci(0x44adf622);  // sqrdmulh z2.s, z17.s, z5.s[1]
    // vl128 state = 0x579bf738
    __ dci(0x44abf632);  // sqrdmulh z18.s, z17.s, z3.s[1]
    // vl128 state = 0x2678c680
    __ dci(0x44a9f6ba);  // sqrdmulh z26.s, z21.s, z1.s[1]
    // vl128 state = 0xee25a322
    __ dci(0x44a9f6aa);  // sqrdmulh z10.s, z21.s, z1.s[1]
    // vl128 state = 0x99cfcf9f
    __ dci(0x44b1f6ab);  // sqrdmulh z11.s, z21.s, z1.s[2]
    // vl128 state = 0xa6785a38
    __ dci(0x44b1f0bb);  // sqdmulh z27.s, z5.s, z1.s[2]
    // vl128 state = 0xfc822233
    __ dci(0x4439f0bf);  // sqdmulh z31.h, z5.h, z1.h[3]
    // vl128 state = 0x322d49df
    __ dci(0x4433f0be);  // sqdmulh z30.h, z5.h, z3.h[2]
    // vl128 state = 0xbf6733d2
    __ dci(0x4433f0d6);  // sqdmulh z22.h, z6.h, z3.h[2]
    // vl128 state = 0x99f11483
    __ dci(0x4437f2d7);  // sqdmulh z23.h, z22.h, z7.h[2]
    // vl128 state = 0x9c146ede
    __ dci(0x4426f2d6);  // sqdmulh z22.h, z22.h, z6.h[0]
    // vl128 state = 0xc089284f
    __ dci(0x44a6f0de);  // sqdmulh z30.s, z6.s, z6.s[0]
    // vl128 state = 0xe962a269
    __ dci(0x44a4f04e);  // sqdmulh z14.s, z2.s, z4.s[0]
    // vl128 state = 0xaea2f35e
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0xaea2f35e,
        0xb4e17c50,
        0x97dfb966,
        0x070d3c78,
        0x5b2f880d,
        0x8e643be0,
        0x4d7f006b,
        0xfbd08185,
        0x4960a97d,
        0x1e85903f,
        0x443b62e4,
        0xf196453a,
        0x50dae6ef,
        0x0e4bb245,
        0x69d661ab,
        0x7d6fb839,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

TEST_SVE(sve2_extract) {
  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE,
                          CPUFeatures::kSVE2,
                          CPUFeatures::kNEON,
                          CPUFeatures::kCRC32);
  START();

  SetInitialMachineState(&masm);
  // state = 0xe2bd2480

  {
    ExactAssemblyScope scope(&masm, 60 * kInstructionSize);
    __ dci(0x056a1008);  // ext z8.b, {z0.b, z1.b}, #84
    // vl128 state = 0x06ae6d5d
    __ dci(0x05601418);  // ext z24.b, {z0.b, z1.b}, #5
    // vl128 state = 0x3b73c922
    __ dci(0x05601708);  // ext z8.b, {z24.b, z25.b}, #5
    // vl128 state = 0xc3526a3d
    __ dci(0x05601d0c);  // ext z12.b, {z8.b, z9.b}, #7
    // vl128 state = 0xbde17731
    __ dci(0x05600c1c);  // ext z28.b, {z0.b, z1.b}, #3
    // vl128 state = 0x9ac72141
    __ dci(0x05600c58);  // ext z24.b, {z2.b, z3.b}, #3
    // vl128 state = 0xccecefc0
    __ dci(0x05600410);  // ext z16.b, {z0.b, z1.b}, #1
    // vl128 state = 0xe49d5f89
    __ dci(0x05600438);  // ext z24.b, {z1.b, z2.b}, #1
    // vl128 state = 0x9967df9d
    __ dci(0x0560067a);  // ext z26.b, {z19.b, z20.b}, #1
    // vl128 state = 0x110a8b46
    __ dci(0x05601478);  // ext z24.b, {z3.b, z4.b}, #5
    // vl128 state = 0x558f95f2
    __ dci(0x0560117c);  // ext z28.b, {z11.b, z12.b}, #4
    // vl128 state = 0x18d0f048
    __ dci(0x0560157e);  // ext z30.b, {z11.b, z12.b}, #5
    // vl128 state = 0x1719547f
    __ dci(0x05601c7a);  // ext z26.b, {z3.b, z4.b}, #7
    // vl128 state = 0x600cfa8a
    __ dci(0x0560187e);  // ext z30.b, {z3.b, z4.b}, #6
    // vl128 state = 0xc93e431e
    __ dci(0x05601876);  // ext z22.b, {z3.b, z4.b}, #6
    // vl128 state = 0x5be7af00
    __ dci(0x05601c26);  // ext z6.b, {z1.b, z2.b}, #7
    // vl128 state = 0xd3d69d02
    __ dci(0x05601c2e);  // ext z14.b, {z1.b, z2.b}, #7
    // vl128 state = 0x1d88c27b
    __ dci(0x05601d3e);  // ext z30.b, {z9.b, z10.b}, #7
    // vl128 state = 0x56f91523
    __ dci(0x05601dae);  // ext z14.b, {z13.b, z14.b}, #7
    // vl128 state = 0xbc175582
    __ dci(0x056015ef);  // ext z15.b, {z15.b, z16.b}, #5
    // vl128 state = 0x9289a9ba
    __ dci(0x0560157f);  // ext z31.b, {z11.b, z12.b}, #5
    // vl128 state = 0x46be3725
    __ dci(0x0560157e);  // ext z30.b, {z11.b, z12.b}, #5
    // vl128 state = 0xa4fd59e9
    __ dci(0x0560156e);  // ext z14.b, {z11.b, z12.b}, #5
    // vl128 state = 0x88b9ba85
    __ dci(0x05601566);  // ext z6.b, {z11.b, z12.b}, #5
    // vl128 state = 0x7f3b2a36
    __ dci(0x056017e4);  // ext z4.b, {z31.b, z0.b}, #5
    // vl128 state = 0xa71b8fa9
    __ dci(0x05601f74);  // ext z20.b, {z27.b, z28.b}, #7
    // vl128 state = 0x89dcdeac
    __ dci(0x05601f44);  // ext z4.b, {z26.b, z27.b}, #7
    // vl128 state = 0xa877313f
    __ dci(0x05601e45);  // ext z5.b, {z18.b, z19.b}, #7
    // vl128 state = 0x6181834a
    __ dci(0x05601255);  // ext z21.b, {z18.b, z19.b}, #4
    // vl128 state = 0x7c3595cd
    __ dci(0x05701a51);  // ext z17.b, {z18.b, z19.b}, #134
    // vl128 state = 0x10fdfe4d
    __ dci(0x05701ad3);  // ext z19.b, {z22.b, z23.b}, #134
    // vl128 state = 0x08e923c5
    __ dci(0x05701ad1);  // ext z17.b, {z22.b, z23.b}, #134
    // vl128 state = 0xefb2c9e9
    __ dci(0x05701b41);  // ext z1.b, {z26.b, z27.b}, #134
    // vl128 state = 0xd5dccda9
    __ dci(0x05701b40);  // ext z0.b, {z26.b, z27.b}, #134
    // vl128 state = 0xd424c039
    __ dci(0x05701bd0);  // ext z16.b, {z30.b, z31.b}, #134
    // vl128 state = 0xd914c077
    __ dci(0x057013d8);  // ext z24.b, {z30.b, z31.b}, #132
    // vl128 state = 0x32459b3a
    __ dci(0x05701259);  // ext z25.b, {z18.b, z19.b}, #132
    // vl128 state = 0x422ed7bf
    __ dci(0x0570125d);  // ext z29.b, {z18.b, z19.b}, #132
    // vl128 state = 0x6bfc46ef
    __ dci(0x05700215);  // ext z21.b, {z16.b, z17.b}, #128
    // vl128 state = 0xc53b85ed
    __ dci(0x0560021d);  // ext z29.b, {z16.b, z17.b}, #0
    // vl128 state = 0xd391e5ec
    __ dci(0x0570121c);  // ext z28.b, {z16.b, z17.b}, #132
    // vl128 state = 0x7990c1d7
    __ dci(0x0570030c);  // ext z12.b, {z24.b, z25.b}, #128
    // vl128 state = 0xca0d3db8
    __ dci(0x05700b88);  // ext z8.b, {z28.b, z29.b}, #130
    // vl128 state = 0xe5c71442
    __ dci(0x05600b0c);  // ext z12.b, {z24.b, z25.b}, #2
    // vl128 state = 0x68510d62
    __ dci(0x05600f1c);  // ext z28.b, {z24.b, z25.b}, #3
    // vl128 state = 0x77f9f046
    __ dci(0x05600e14);  // ext z20.b, {z16.b, z17.b}, #3
    // vl128 state = 0x7068dedf
    __ dci(0x05600604);  // ext z4.b, {z16.b, z17.b}, #1
    // vl128 state = 0x8b70c406
    __ dci(0x05600406);  // ext z6.b, {z0.b, z1.b}, #1
    // vl128 state = 0x10e6b48c
    __ dci(0x05600056);  // ext z22.b, {z2.b, z3.b}, #0
    // vl128 state = 0xe1294d7a
    __ dci(0x05600052);  // ext z18.b, {z2.b, z3.b}, #0
    // vl128 state = 0x0762bbb0
    __ dci(0x056000d6);  // ext z22.b, {z6.b, z7.b}, #0
    // vl128 state = 0x58be0ba4
    __ dci(0x057008de);  // ext z30.b, {z6.b, z7.b}, #130
    // vl128 state = 0x8a2018e9
    __ dci(0x0570085a);  // ext z26.b, {z2.b, z3.b}, #130
    // vl128 state = 0xb019b7e0
    __ dci(0x057009d2);  // ext z18.b, {z14.b, z15.b}, #130
    // vl128 state = 0x9e6e14ed
    __ dci(0x057008fa);  // ext z26.b, {z7.b, z8.b}, #130
    // vl128 state = 0x4cf64d22
    __ dci(0x057008f2);  // ext z18.b, {z7.b, z8.b}, #130
    // vl128 state = 0x048c30f9
    __ dci(0x057002f3);  // ext z19.b, {z23.b, z24.b}, #128
    // vl128 state = 0x2d7eb43b
    __ dci(0x057006a3);  // ext z3.b, {z21.b, z22.b}, #129
    // vl128 state = 0xa37aeb5e
    __ dci(0x05700687);  // ext z7.b, {z20.b, z21.b}, #129
    // vl128 state = 0xd8d7cdc7
    __ dci(0x056006b7);  // ext z23.b, {z21.b, z22.b}, #1
    // vl128 state = 0x2480e1d4
  }

  uint32_t state;
  ComputeMachineStateHash(&masm, &state);
  __ Mov(x0, reinterpret_cast<uint64_t>(&state));
  __ Ldr(w0, MemOperand(x0));

  END();
  if (CAN_RUN()) {
    RUN();
    uint32_t expected_hashes[] = {
        0x2480e1d4,
        0x4dc42cc5,
        0x7ac24121,
        0x9eaf5c98,
        0x1b7b35dc,
        0x1b1035fc,
        0xe15f6899,
        0xaad14717,
        0x3327c3fc,
        0x7f349408,
        0x2d865b00,
        0x9819cd29,
        0x7f64cace,
        0x3751e2c1,
        0x7e60fc24,
        0xc6b308fc,
    };
    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - 1], x0);
  }
}

}  // namespace aarch64
}  // namespace vixl
