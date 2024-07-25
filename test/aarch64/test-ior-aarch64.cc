// Copyright 2022, VIXL authors
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


#include <cstdio>
#include <cstring>
#include <string>

#include "test-runner.h"

#include "aarch64/disasm-aarch64.h"
#include "aarch64/io-reporter-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"
#include "test-ior-aarch64.h"

namespace vixl {
namespace aarch64 {

#define R(...) \
  { __VA_ARGS__ }

// These tests are generated using limited parsing of the Disassembler output
// for randomly encoded instruction inputs, which have then been corrected by
// hand. They use "dci" as that's the simplest way to obtain an instruction
// from random data.
//
// Subsequent tests for new instructions should use normal Assembler methods
// instead, eg. adc(w6, w20, w28) rather than dci(0x1a1c0286).
//
// The grouping below is an arbitrary partitioning into 32-instruction blocks,
// to improve testing speed when executed in parallel.

TEST(group_0) {
  SETUP();

  CMP_IO(dci(0x0ea0ba38), R(v24), R(v17));         // abs v24.2s, v17.2s
  CMP_IO(dci(0x5ee0b9de), R(v30), R(v14));         // abs d30, d14
  CMP_IO(dci(0x0456aa45), R(v5), R(v5, p2, v18));  // abs z5.h, p2/m, z18.h
  CMP_IO(dci(0x1a1c0286), R(x6), R(x20, x28));     // adc w6, w20, w28
  CMP_IO(dci(0x9a1c00ef), R(x15), R(x7, x28));     // adc x15, x7, x28
  CMP_IO(dci(0x451ad14c),
         R(v12),
         R(v12, v10, v26));  // adclb z12.s, z10.s, z26.s
  CMP_IO(dci(0x4559d78e),
         R(v14),
         R(v14, v28, v25));                     // adclt z14.d, z28.d, z25.d
  CMP_IO(dci(0x3a1c037f), R(), R(x27, x28));    // adcs wzr, w27, w28
  CMP_IO(dci(0xba1c0116), R(x22), R(x8, x28));  // adcs x22, x8, x28
  CMP_IO(dci(0x0b3b27e9), R(x9), R(sp, x27));   // add w9, wsp, w27, uxth #1
  CMP_IO(dci(0x117b33e9), R(x9), R(sp));  // add w9, wsp, #0xecc000 (15515648)
  CMP_IO(dci(0x0b1a6b0a), R(x10), R(x24, x26));  // add w10, w24, w26, lsl #26
  CMP_IO(dci(0x8b3b5324), R(x4), R(x25, x27));   // add x4, x25, w27, uxtw #4
  CMP_IO(dci(0x911b5324), R(x4), R(x25));        // add x4, x25, #0x6d4 (1748)
  CMP_IO(dci(0x8b5af573), R(x19), R(x11, x26));  // add x19, x11, x26, lsr #61
  CMP_IO(dci(0x0e3d8557), R(v23), R(v10, v29));  // add v23.8b, v10.8b, v29.8b
  CMP_IO(dci(0x5efe84b9), R(v25), R(v5, v30));   // add d25, d5, d30
  CMP_IO(dci(0x048009e8),
         R(v8),
         R(p2, v8, v15));                       // add z8.s, p2/m, z8.s, z15.s
  CMP_IO(dci(0x2520c0ac), R(v12), R(v12));      // add z12.b, z12.b, #5
  CMP_IO(dci(0x04fd02e1), R(v1), R(v23, v29));  // add z1.d, z23.d, z29.d
  CMP_IO(dci(0x919cb516), R(x22), R(x8));       // addg x22, x8, #448, #13
  CMP_IO(dci(0x0e374146), R(v6), R(v10, v23));  // addhn v6.8b, v10.8h, v23.8h
  CMP_IO(dci(0x45796125), R(v5), R(v9, v25));   // addhnb z5.b, z9.h, z25.h
  CMP_IO(dci(0x45ba66e3), R(v3), R(v3, v23, v26));  // addhnt z3.h, z23.s, z26.s
  CMP_IO(dci(0x4e3abc87), R(v7), R(v4, v26));  // addp v7.16b, v4.16b, v26.16b
  CMP_IO(dci(0x5ef1b959), R(v25), R(v10));     // addp d25, v10.2d
  CMP_IO(dci(0x4491a358),
         R(v24),
         R(p0, v24, v26));                     // addp z24.s, p0/m, z24.s, z26.s
  CMP_IO(dci(0x047e5708), R(x8), R(x30));      // addpl x8, x30, #-8
  CMP_IO(dci(0x2b5c6923), R(x3), R(x9, x28));  // adds w3, w9, w28, lsr #26
  CMP_IO(dci(0x2b3b27e9), R(x9), R(sp, x27));  // adds w9, wsp, w27, uxth #1
  CMP_IO(dci(0x317b33e9), R(x9), R(sp));  // adds w9, wsp, #0xecc000 (15515648)
  CMP_IO(dci(0xab5cdeba), R(x26), R(x21, x28));  // adds x26, x21, x28, lsr #55

  CLEANUP();
}

TEST(group_1) {
  SETUP();

  CMP_IO(dci(0xab3b5324), R(x4), R(x25, x27));  // adds x4, x25, w27, uxtw #4
  CMP_IO(dci(0xb11b5324), R(x4), R(x25));       // adds x4, x25, #0x6d4 (1748)
  CMP_IO(dci(0x0e31b964), R(v4), R(v11));       // addv b4, v11.8b
  CMP_IO(dci(0x043d5092), R(x18), R(x29));      // addvl x18, x29, #4
  CMP_IO(dci(0x101d9a1c), R(x28), R());         // adr x28, #+0x3b340
  CMP_IO(dci(0x043baa79),
         R(v25),
         R(v19, v27));  // adr z25.d, [z19.d, z27.d, sxtw #2]
  CMP_IO(dci(0x047ca8ef),
         R(v15),
         R(v7, v28));  // adr z15.d, [z7.d, z28.d, uxtw #2]
  CMP_IO(dci(0x04baafdc),
         R(v28),
         R(v30, v26));                     // adr z28.s, [z30.s, z26.s, lsl #3]
  CMP_IO(dci(0xb01a594c), R(x12), R());    // adrp x12, #+0x34b29000
  CMP_IO(dci(0x121c4cfc), R(x28), R(x7));  // and w28, w7, #0xfffff0
  CMP_IO(dci(0x0a9c15db), R(x27), R(x14, x28));  // and w27, w14, w28, asr #5
  CMP_IO(dci(0x927c2dc1), R(x1), R(x14));        // and x1, x14, #0xfff0
  CMP_IO(dci(0x8a5c8b72), R(x18), R(x27, x28));  // and x18, x27, x28, lsr #34
  CMP_IO(dci(0x4e3d1fcd),
         R(v13),
         R(v30, v29));  // and v13.16b, v30.16b, v29.16b
  CMP_IO(dci(0x250d60eb),
         R(p11),
         R(p8, p7, p13));  // and p11.b, p8/z, p7.b, p13.b
  CMP_IO(dci(0x04da01ce),
         R(v14),
         R(p0, v14, v14));                // and z14.d, p0/m, z14.d, z14.d
  CMP_IO(dci(0x05827f22), R(v2), R(v2));  // and z2.d, z2.d, #0xfffe07ffffffffff
  CMP_IO(dci(0x043d32fb), R(v27), R(v23, v29));  // and z27.d, z23.d, z29.d
  CMP_IO(dci(0x6a5b64e2), R(x2), R(x7, x27));    // ands w2, w7, w27, lsr #25
  CMP_IO(dci(0x721c4cfc), R(x28), R(x7));        // ands w28, w7, #0xfffff0
  CMP_IO(dci(0xea9bef4b), R(x11), R(x26, x27));  // ands x11, x26, x27, asr #59
  CMP_IO(dci(0xf27c2dc1), R(x1), R(x14));        // ands x1, x14, #0xfff0
  CMP_IO(dci(0x254a55c2),
         R(p2),
         R(p5, p14, p10));                     // ands p2.b, p5/z, p14.b, p10.b
  CMP_IO(dci(0x045a382a), R(v10), R(p6, v1));  // andv h10, p6, z1.h
  CMP_IO(dci(0x04c09ba6), R(v6), R(p6, v6));   // asr z6.d, p6/m, z6.d, #3
  CMP_IO(dci(0x04188cfc),
         R(v28),
         R(p3, v28, v7));  // asr z28.b, p3/m, z28.b, z7.d
  CMP_IO(dci(0x04109f7f),
         R(v31),
         R(p7, v31, v27));                 // asr z31.b, p7/m, z31.b, z27.b
  CMP_IO(dci(0x04be9243), R(v3), R(v18));  // asr z3.d, z18.d, #34
  CMP_IO(dci(0x047d83cd), R(v13), R(v30, v29));  // asr z13.h, z30.h, z29.d
  CMP_IO(dci(0x044484e2), R(v2), R(p1, v2));     // asrd z2.s, p1/m, z2.s, #25
  CMP_IO(dci(0x0454841d),
         R(v29),
         R(p1, v29, v0));  // asrr z29.h, p1/m, z29.h, z0.h
  CMP_IO(dci(0x1ade2a50), R(x16), R(x18, x30));  // asr w16, w18, w30

  CLEANUP();
}

TEST(group_2) {
  SETUP();

  CMP_IO(dci(0x9ade2b15), R(x21), R(x24, x30));  // asr x21, x24, x30
  CMP_IO(dci(0xdac11b15), R(x21), R(x21, x24));  // autda x21, x24
  CMP_IO(dci(0xdac11e50), R(x16), R(x16, x18));  // autdb x16, x18
  CMP_IO(dci(0xdac13be5), R(x5), R(x5));         // autdza x5
  CMP_IO(dci(0xdac13fe0), R(x0), R(x0));         // autdzb x0
  CMP_IO(dci(0xd503219f), R(x17), R(x17, x16));  // autia1716
  CMP_IO(dci(0xdac11322), R(x2), R(x2, x25));    // autia x2, x25
  CMP_IO(dci(0xd50323bf), R(x30), R(x30, sp));   // autiasp
  CMP_IO(dci(0xd503239f), R(x30), R(x30));       // autiaz
  CMP_IO(dci(0xd50321df), R(x17), R(x17, x16));  // autib1716
  CMP_IO(dci(0xdac1158b), R(x11), R(x11, x12));  // autib x11, x12
  CMP_IO(dci(0xd50323ff), R(x30), R(x30, sp));   // autibsp
  CMP_IO(dci(0xd50323df), R(x30), R(x30));       // autibz
  CMP_IO(dci(0xdac133f2), R(x18), R(x18));       // autiza x18
  CMP_IO(dci(0xdac137fb), R(x27), R(x27));       // autizb x27
  CMP_IO(dci(0xd5004d5f), R(), R());             // axflag
  CMP_IO(dci(0x159b7cc8), R(), R());             // b #+0x66df320
  CMP_IO(dci(0x549ad628), R(), R());             // b.hi #-0xca53c
  CMP_IO(dci(0x047d3a1c),
         R(v28),
         R(v28, v29, v16));  // bcax z28.d, z28.d, z29.d, z16.d
  CMP_IO(dci(0x459ab6e3), R(v3), R(v23, v26));   // bdep z3.s, z23.s, z26.s
  CMP_IO(dci(0x4599b39b), R(v27), R(v28, v25));  // bext z27.s, z28.s, z25.s
  CMP_IO(dci(0x330075c2), R(x2), R(x2, x14));    // bfxil w2, w14, #0, #30
  CMP_IO(dci(0xb37d7ae1), R(x1), R(x1, x23));    // bfi x1, x23, #3, #31
  CMP_IO(dci(0x4519b9f7), R(v23), R(v15, v25));  // bgrp z23.b, z15.b, z25.b
  CMP_IO(dci(0x0a7c0b72), R(x18), R(x27, x28));  // bic w18, w27, w28, lsr #2
  CMP_IO(dci(0x8abc95db), R(x27), R(x14, x28));  // bic x27, x14, x28, asr #37
  CMP_IO(dci(0x2f04b758), R(v24), R(v24));       // bic v24.4h, #0x9a, lsl #8
  CMP_IO(dci(0x2f031645), R(v5), R(v5));         // bic v5.2s, #0x72, lsl #0
  CMP_IO(dci(0x4e7e1f08), R(v8), R(v24, v30));   // bic v8.16b, v24.16b, v30.16b
  CMP_IO(dci(0x250d5892),
         R(p2),
         R(p6, p4, p13));  // bic p2.b, p6/z, p4.b, p13.b
  CMP_IO(dci(0x041b0c37),
         R(v23),
         R(p3, v23, v1));  // bic z23.b, p3/m, z23.b, z1.b

  CLEANUP();
}

TEST(group_3) {
  SETUP();

  CMP_IO(dci(0x04fd3092), R(v18), R(v4, v29));   // bic z18.d, z4.d, z29.d
  CMP_IO(dci(0x6abb6f4b), R(x11), R(x26, x27));  // bics w11, w26, w27, asr #27
  CMP_IO(dci(0xea7be4e2), R(x2), R(x7, x27));    // bics x2, x7, x27, lsr #57
  CMP_IO(dci(0x254a603b),
         R(p11),
         R(p8, p1, p10));  // bics p11.b, p8/z, p1.b, p10.b
  CMP_IO(dci(0x6efd1efb),
         R(v27),
         R(v27, v23, v29));  // bif v27.16b, v23.16b, v29.16b
  CMP_IO(dci(0x6ebe1cb9),
         R(v25),
         R(v25, v5, v30));                      // bit v25.16b, v5.16b, v30.16b
  CMP_IO(dci(0x951c4cfc), R(x30), R());         // bl #+0x47133f0
  CMP_IO(dci(0xd63f00e0), R(x30), R(x7));       // blr x7
  CMP_IO(dci(0xd73f0b7f), R(x30), R(x27, sp));  // blraa x27, sp
  CMP_IO(dci(0xd63f0b7f), R(x30), R(x27));      // blraaz x27
  CMP_IO(dci(0xd73f0d16), R(x30), R(x8, x22));  // blrab x8, x22
  CMP_IO(dci(0xd63f0ebf), R(x30), R(x21));      // blrabz x21
  CMP_IO(dci(0xd61f0120), R(), R(x9));          // br x9
  CMP_IO(dci(0xd71f0973), R(), R(x11, x19));    // braa x11, x19
  CMP_IO(dci(0xd61f0bdf), R(), R(x30));         // braaz x30
  CMP_IO(dci(0xd71f0f0a), R(), R(x24, x10));    // brab x24, x10
  CMP_IO(dci(0xd61f0f3f), R(), R(x25));         // brabz x25
  CMP_IO(dci(0xd43de2e0), R(), R());            // brk #0xef17
  CMP_IO(dci(0x25107d7e),
         R(p14),
         R(p14, p15, p11));                        // brka p14.b, p15/m, p11.b
  CMP_IO(dci(0x25504d49), R(p9), R(p3, p10));      // brkas p9.b, p3/z, p10.b
  CMP_IO(dci(0x25907516), R(p6), R(p6, p13, p8));  // brkb p6.b, p13/m, p8.b
  CMP_IO(dci(0x25d06c84), R(p4), R(p11, p4));      // brkbs p4.b, p11/z, p4.b
  CMP_IO(dci(0x2518518a),
         R(p10),
         R(p4, p12, p10));  // brkn p10.b, p4/z, p12.b, p10.b
  CMP_IO(dci(0x25584083),
         R(p3),
         R(p0, p4, p3));  // brkns p3.b, p0/z, p4.b, p3.b
  CMP_IO(dci(0x250cfd6f),
         R(p15),
         R(p15, p11, p12));  // brkpa p15.b, p15/z, p11.b, p12.b
  CMP_IO(dci(0x254ccc86),
         R(p6),
         R(p3, p4, p12));  // brkpas p6.b, p3/z, p4.b, p12.b
  CMP_IO(dci(0x250cf516),
         R(p6),
         R(p13, p8, p12));  // brkpb p6.b, p13/z, p8.b, p12.b
  CMP_IO(dci(0x254cd8ff),
         R(p15),
         R(p6, p7, p12));  // brkpbs p15.b, p6/z, p7.b, p12.b
  CMP_IO(dci(0x047d3e02),
         R(v2),
         R(v2, v29, v16));  // bsl1n z2.d, z2.d, z29.d, z16.d
  CMP_IO(dci(0x04bd3c6b),
         R(v11),
         R(v11, v29, v3));  // bsl2n z11.d, z11.d, z29.d, z3.d
  CMP_IO(dci(0x2e7e1d8b),
         R(v11),
         R(v11, v12, v30));  // bsl v11.8b, v12.8b, v30.8b
  CMP_IO(dci(0x04393df7),
         R(v23),
         R(v23, v25, v15));  // bsl z23.d, z23.d, z25.d, z15.d

  CLEANUP();
}

TEST(group_4) {
  SETUP();

  CMP_IO(dci(0xd503245f), R(), R());  // bti c
  CMP_IO(dci(0x4500dabc),
         R(v28),
         R(v28, v21));  // cadd z28.b, z28.b, z21.b, #90
  CMP_IO(dci(0x88be7d7d), R(x30), R(x30, x29, x11));  // cas w30, w29, [x11]
  CMP_IO(dci(0xc8be7fe7), R(x30), R(x30, x7, sp));    // cas x30, x7, [sp]
  CMP_IO(dci(0x88fa7fb5), R(x26), R(x26, x21, x29));  // casa w26, w21, [x29]
  CMP_IO(dci(0xc8fa7ef0), R(x26), R(x26, x16, x23));  // casa x26, x16, [x23]
  CMP_IO(dci(0x08fe7fe7), R(x30), R(x30, x7, sp));    // casab w30, w7, [sp]
  CMP_IO(dci(0x48fe7e5d), R(x30), R(x30, x29, x18));  // casah w30, w29, [x18]
  CMP_IO(dci(0x88fefcb9), R(x30), R(x30, x25, x5));   // casal w30, w25, [x5]
  CMP_IO(dci(0xc8fefff4), R(x30), R(x30, x20, sp));   // casal x30, x20, [sp]
  CMP_IO(dci(0x08fafe2b), R(x26), R(x26, x11, x17));  // casalb w26, w11, [x17]
  CMP_IO(dci(0x48fafe11), R(x26), R(x26, x17, x16));  // casalh w26, w17, [x16]
  CMP_IO(dci(0x08ba7ef0), R(x26), R(x26, x16, x23));  // casb w26, w16, [x23]
  CMP_IO(dci(0x48ba7d66), R(x26), R(x26, x6, x11));   // cash w26, w6, [x11]
  CMP_IO(dci(0x88baffc2), R(x26), R(x26, x2, x30));   // casl w26, w2, [x30]
  CMP_IO(dci(0xc8bafe2b), R(x26), R(x26, x11, x17));  // casl x26, x11, [x17]
  CMP_IO(dci(0x08befff4), R(x30), R(x30, x20, sp));   // caslb w30, w20, [sp]
  CMP_IO(dci(0x48beffda), R(x30), R(x30, x26, x30));  // caslh w30, w26, [x30]
  CMP_IO(dci(0x083e7d7e),
         R(x30),
         R(x30, x11));  // casp w30, wzr, w30, wzr, [x11]
  CMP_IO(dci(0x482a7cf8),
         R(x10, x11),
         R(x10, x11, x24, x25, x7));  // casp x10, x11, x24, x25, [x7]
  CMP_IO(dci(0x086e7c76),
         R(x14, x15),
         R(x14, x15, x22, x23, x3));  // caspa w14, w15, w22, w23, [x3]
  CMP_IO(dci(0x487a7ef0),
         R(x26, x27),
         R(x26, x27, x16, x17, x23));  // caspa x26, x27, x16, x17, [x23]
  CMP_IO(dci(0x086afc70),
         R(x10, x11),
         R(x10, x11, x16, x17, x3));  // caspal w10, w11, w16, w17, [x3]
  CMP_IO(dci(0x487efff4),
         R(x30),
         R(x30, x20, x21, sp));  // caspal x30, xzr, x20, x21, [sp]
  CMP_IO(dci(0x083affc2),
         R(x26, x27),
         R(x26, x27, x2, x3, x30));  // caspl w26, w27, w2, w3, [x30]
  CMP_IO(dci(0x4838fedc),
         R(x24, x25),
         R(x24, x25, x28, x29, x22));         // caspl x24, x25, x28, x29, [x22]
  CMP_IO(dci(0x35bde2fb), R(), R(x27));       // cbnz w27, #-0x843a4
  CMP_IO(dci(0xb55dd892), R(), R(x18));       // cbnz x18, #+0xbbb10
  CMP_IO(dci(0x3479bd25), R(), R(x5));        // cbz w5, #+0xf37a4
  CMP_IO(dci(0xb499dc60), R(), R(x0));        // cbz x0, #-0xcc474
  CMP_IO(dci(0x3a5bd800), R(), R(x0));        // ccmn w0, #27, #nzcv, le
  CMP_IO(dci(0x3a5b724f), R(), R(x18, x27));  // ccmn w18, w27, #NZCV, vc

  CLEANUP();
}

TEST(group_5) {
  SETUP();

  CMP_IO(dci(0xba5bb8c5), R(), R(x6));        // ccmn x6, #27, #nZcV, lt
  CMP_IO(dci(0xba5b63e6), R(), R(x27));       // ccmn xzr, x27, #nZCv, vs
  CMP_IO(dci(0x7a5b3a42), R(), R(x18));       // ccmp w18, #27, #nzCv, lo
  CMP_IO(dci(0x7a5b8321), R(), R(x25, x27));  // ccmp w25, w27, #nzcV, hi
  CMP_IO(dci(0xfa5b48ab), R(), R(x5));        // ccmp x5, #27, #NzCV, mi
  CMP_IO(dci(0xfa5ba26c), R(), R(x19, x27));  // ccmp x19, x27, #NZcv, ge
  CMP_IO(dci(0x449d13c0),
         R(v0),
         R(v0, v30, v29));  // cdot z0.s, z30.b, z29.b, #0
  CMP_IO(dci(0x44fb4b3e),
         R(v30),
         R(v30, v25, v11));  // cdot z30.d, z25.h, z11.h[1], #180
  CMP_IO(dci(0x44ba4573),
         R(v19),
         R(v19, v11, v2));            // cdot z19.s, z11.b, z2.b[3], #90
  CMP_IO(dci(0xd5004e1f), R(), R());  // cfinv
  CMP_IO(dci(0x0570bdf5),
         R(x21),
         R(p7, x21, v15));  // clasta w21, p7, w21, z15.h
  CMP_IO(dci(0x05ea9451), R(v17), R(p5, v17, v2));  // clasta d17, p5, d17, z2.d
  CMP_IO(dci(0x05289f17),
         R(v23),
         R(p7, v23, v24));  // clasta z23.b, p7, z23.b, z24.b
  CMP_IO(dci(0x05b1bd30), R(x16), R(p7, x16, v9));  // clastb w16, p7, w16, z9.s
  CMP_IO(dci(0x052b938c),
         R(v12),
         R(p4, v12, v28));  // clastb b12, p4, b12, z28.b
  CMP_IO(dci(0x05e994ae),
         R(v14),
         R(p5, v14, v5));                   // clastb z14.d, p5, z14.d, z5.d
  CMP_IO(dci(0xd503375f), R(), R());        // clrex #0x7
  CMP_IO(dci(0x5ac017cf), R(x15), R(x30));  // cls w15, w30
  CMP_IO(dci(0xdac01566), R(x6), R(x11));   // cls x6, x11
  CMP_IO(dci(0x0e20494c), R(v12), R(v10));  // cls v12.8b, v10.8b
  CMP_IO(dci(0x04d8b3e9), R(v9), R(v9, p4, v31));    // cls z9.d, p4/m, z31.d
  CMP_IO(dci(0x5ac012f0), R(x16), R(x23));           // clz w16, w23
  CMP_IO(dci(0xdac013b5), R(x21), R(x29));           // clz x21, x29
  CMP_IO(dci(0x2ea04a2b), R(v11), R(v17));           // clz v11.2s, v17.2s
  CMP_IO(dci(0x0459ab0a), R(v10), R(v10, p2, v24));  // clz z10.h, p2/m, z24.h
  CMP_IO(dci(0x4ee098c6), R(v6), R(v6));             // cmeq v6.2d, v6.2d, #0
  CMP_IO(dci(0x6efd8c0f), R(v15), R(v0, v29));   // cmeq v15.2d, v0.2d, v29.2d
  CMP_IO(dci(0x5ee09a14), R(v20), R(v16));       // cmeq d20, d16, #0
  CMP_IO(dci(0x7eec8e34), R(v20), R(v17, v12));  // cmeq d20, d17, d12
  CMP_IO(dci(0x2e208964), R(v4), R(v11));        // cmge v4.8b, v11.8b, #0
  CMP_IO(dci(0x0e3a3e04), R(v4), R(v16, v26));   // cmge v4.8b, v16.8b, v26.8b
  CMP_IO(dci(0x7ee0897e), R(v30), R(v11));       // cmge d30, d11, #0

  CLEANUP();
}

TEST(group_6) {
  SETUP();

  CMP_IO(dci(0x5efa3e1e), R(v30), R(v16, v26));  // cmge d30, d16, d26
  CMP_IO(dci(0x0ea0898b), R(v11), R(v12));       // cmgt v11.2s, v12.2s, #0
  CMP_IO(dci(0x0eba362b), R(v11), R(v17, v26));  // cmgt v11.2s, v17.2s, v26.2s
  CMP_IO(dci(0x5ee08ac9), R(v9), R(v22));        // cmgt d9, d22, #0
  CMP_IO(dci(0x5ef636e9), R(v9), R(v23, v22));   // cmgt d9, d23, d22
  CMP_IO(dci(0x6ef936c9), R(v9), R(v22, v25));   // cmhi v9.2d, v22.2d, v25.2d
  CMP_IO(dci(0x7ef33662), R(v2), R(v19, v19));   // cmhi d2, d19, d19
  CMP_IO(dci(0x6eba3ee3), R(v3), R(v23, v26));   // cmhs v3.4s, v23.4s, v26.4s
  CMP_IO(dci(0x7eeb3fee), R(v14), R(v31, v11));  // cmhs d14, d31, d11
  CMP_IO(dci(0x44de24b9),
         R(v25),
         R(v25, v5, v30));  // cmla z25.d, z5.d, z30.d, #90
  CMP_IO(dci(0x44bb6f17),
         R(v23),
         R(v23, v24, v3));  // cmla z23.h, z24.h, z3.h[3], #270
  CMP_IO(dci(0x44fc6e86),
         R(v6),
         R(v6, v20, v12));                  // cmla z6.s, z20.s, z12.s[1], #270
  CMP_IO(dci(0x6ee09ae1), R(v1), R(v23));   // cmle v1.2d, v23.2d, #0
  CMP_IO(dci(0x7ee0992b), R(v11), R(v9));   // cmle d11, d9, #0
  CMP_IO(dci(0x4e60abda), R(v26), R(v30));  // cmlt v26.8h, v30.8h, #0
  CMP_IO(dci(0x5ee0a818), R(v24), R(v0));   // cmlt d24, d0, #0
  CMP_IO(dci(0x25dd9ba6), R(p6), R(p6, v29));  // cmpeq p6.d, p6/z, z29.d, #-3
  CMP_IO(dci(0x241c2cec),
         R(p12),
         R(p3, v7, v28));  // cmpeq p12.b, p3/z, z7.b, z28.d
  CMP_IO(dci(0x241cbf6f),
         R(p15),
         R(p7, v27, v28));                    // cmpeq p15.b, p7/z, z27.b, z28.b
  CMP_IO(dci(0x251c0044), R(p4), R(p0, v2));  // cmpge p4.b, p0/z, z2.b, #-4
  CMP_IO(dci(0x24dd484e),
         R(p14),
         R(p2, v2, v29));  // cmpge p14.d, p2/z, z2.d, z29.d
  CMP_IO(dci(0x24dd8789),
         R(p9),
         R(p1, v28, v29));                     // cmpge p9.d, p1/z, z28.d, z29.d
  CMP_IO(dci(0x259d1c7b), R(p11), R(p7, v3));  // cmpgt p11.s, p7/z, z3.s, #-3
  CMP_IO(dci(0x245c5775),
         R(p5),
         R(p5, v27, v28));  // cmpgt p5.h, p5/z, z27.h, z28.d
  CMP_IO(dci(0x245c8b72),
         R(p2),
         R(p2, v27, v28));                    // cmpgt p2.h, p2/z, z27.h, z28.h
  CMP_IO(dci(0x24fc8119), R(p9), R(p0, v8));  // cmphi p9.d, p0/z, z8.d, #114
  CMP_IO(dci(0x241dd2d7),
         R(p7),
         R(p4, v22, v29));  // cmphi p7.b, p4/z, z22.b, z29.d
  CMP_IO(dci(0x241d06d4),
         R(p4),
         R(p1, v22, v29));                    // cmphi p4.b, p1/z, z22.b, z29.b
  CMP_IO(dci(0x24bcc923), R(p3), R(p2, v9));  // cmphs p3.s, p2/z, z9.s, #115
  CMP_IO(dci(0x245ccaad),
         R(p13),
         R(p2, v21, v28));  // cmphs p13.h, p2/z, z21.h, z28.d
  CMP_IO(dci(0x245c182a),
         R(p10),
         R(p6, v1, v28));                      // cmphs p10.h, p6/z, z1.h, z28.h
  CMP_IO(dci(0x25dc2dd1), R(p1), R(p3, v14));  // cmple p1.d, p3/z, z14.d, #-4

  CLEANUP();
}

TEST(group_7) {
  SETUP();

  CMP_IO(dci(0x241d661f),
         R(p15),
         R(p1, v16, v29));  // cmple p15.b, p1/z, z16.b, z29.d
  CMP_IO(dci(0x24bcb5cb),
         R(p11),
         R(p5, v14));  // cmplo p11.s, p5/z, z14.s, #114
  CMP_IO(dci(0x245cfde5),
         R(p5),
         R(p7, v15, v28));                     // cmplo p5.h, p7/z, z15.h, z28.d
  CMP_IO(dci(0x243d32d7), R(p7), R(p4, v22));  // cmpls p7.b, p4/z, z22.b, #116
  CMP_IO(dci(0x24dce119),
         R(p9),
         R(p0, v8, v28));                       // cmpls p9.d, p0/z, z8.d, z28.d
  CMP_IO(dci(0x255c3eaa), R(p10), R(p7, v21));  // cmplt p10.h, p7/z, z21.h, #-4
  CMP_IO(dci(0x249c69e8),
         R(p8),
         R(p2, v15, v28));                     // cmplt p8.s, p2/z, z15.s, z28.d
  CMP_IO(dci(0x259c8293), R(p3), R(p0, v20));  // cmpne p3.s, p0/z, z20.s, #-4
  CMP_IO(dci(0x245d313d),
         R(p13),
         R(p4, v9, v29));  // cmpne p13.h, p4/z, z9.h, z29.d
  CMP_IO(dci(0x245cbeba),
         R(p10),
         R(p7, v21, v28));  // cmpne p10.h, p7/z, z21.h, z28.h
  CMP_IO(dci(0x0ebe8d8b), R(v11), R(v12, v30));  // cmtst v11.2s, v12.2s, v30.2s
  CMP_IO(dci(0x5ee08ec9), R(v9), R(v22, v0));    // cmtst d9, d22, d0
  CMP_IO(dci(0x049ba293), R(v19), R(v19, p0, v20));  // cnot z19.s, p0/m, z20.s
  CMP_IO(dci(0x4e205887), R(v7), R(v4));             // cnt v7.16b, v4.16b
  CMP_IO(dci(0x04daa7f6), R(v22), R(v22, p1, v31));  // cnt z22.d, p1/m, z31.d
  CMP_IO(dci(0x042de14a), R(x10), R());              // cntb x10, vl32, mul #14
  CMP_IO(dci(0x04ede3c0), R(x0), R());               // cntd x0, mul3, mul #14
  CMP_IO(dci(0x046de164), R(x4), R());               // cnth x4, vl64, mul #14
  CMP_IO(dci(0x25a0b078), R(x24), R(p12, p3));       // cntp x24, p12, p3.s
  CMP_IO(dci(0x04aee315), R(x21), R());              // cntw x21, #0x18, mul #15
  CMP_IO(dci(0x05e18024), R(v4), R(p0, v1));         // compact z4.d, p0, z1.d
  CMP_IO(dci(0x059b1c03), R(v3), R(p11));            // mov z3.s, p11/z, #-32
  CMP_IO(dci(0x05db67f6),
         R(v22),
         R(v22, p11));  // mov z22.d, p11/m, #63, lsl #8
  CMP_IO(dci(0x0528a5a7), R(v7), R(v7, p1, x13));  // mov z7.b, p1/m, w13
  CMP_IO(dci(0x05a09c03), R(v3), R(v3, p7, v0));   // mov z3.s, p7/m, s0
  CMP_IO(dci(0x1d8407bb),
         R(x27, x4, x29),
         R(x27, x4, x29));  // cpye [x27]!, [x4]!, x29!
  CMP_IO(dci(0x1d9dc629),
         R(x9, x29, x17),
         R(x9, x29, x17));  // cpyen [x9]!, [x29]!, x17!
  CMP_IO(dci(0x1d9a862b),
         R(x11, x26, x17),
         R(x11, x26, x17));  // cpyern [x11]!, [x26]!, x17!
  CMP_IO(dci(0x1d88446e),
         R(x14, x8, x3),
         R(x14, x8, x3));  // cpyewn [x14]!, [x8]!, x3!
  CMP_IO(dci(0x199e0655),
         R(x21, x30, x18),
         R(x21, x30, x18));  // cpyfe [x21]!, [x30]!, x18!
  CMP_IO(dci(0x1996c72b),
         R(x11, x22, x25),
         R(x11, x22, x25));  // cpyfen [x11]!, [x22]!, x25!
  CMP_IO(dci(0x199e865d),
         R(x29, x30, x18),
         R(x29, x30, x18));  // cpyfern [x29]!, [x30]!, x18!

  CLEANUP();
}

TEST(group_8) {
  SETUP();

  CMP_IO(dci(0x199e4722),
         R(x2, x30, x25),
         R(x2, x30, x25));  // cpyfewn [x2]!, [x30]!, x25!
  CMP_IO(dci(0x195d0629),
         R(x9, x29, x17),
         R(x9, x29, x17));  // cpyfm [x9]!, [x29]!, x17!
  CMP_IO(dci(0x1944c7bb),
         R(x27, x4, x29),
         R(x27, x4, x29));  // cpyfmn [x27]!, [x4]!, x29!
  CMP_IO(dci(0x19558545),
         R(x5, x21, x10),
         R(x5, x21, x10));  // cpyfmrn [x5]!, [x21]!, x10!
  CMP_IO(dci(0x1948448e),
         R(x14, x8, x4),
         R(x14, x8, x4));  // cpyfmwn [x14]!, [x8]!, x4!
  CMP_IO(dci(0x190f0582),
         R(x2, x15, x12),
         R(x2, x15, x12));  // cpyfp [x2]!, [x15]!, x12!
  CMP_IO(dci(0x190ec72c),
         R(x12, x14, x25),
         R(x12, x14, x25));  // cpyfpn [x12]!, [x14]!, x25!
  CMP_IO(dci(0x19028663),
         R(x3, x2, x19),
         R(x3, x2, x19));  // cpyfprn [x3]!, [x2]!, x19!
  CMP_IO(dci(0x191d44f1),
         R(x17, x29, x7),
         R(x17, x29, x7));  // cpyfpwn [x17]!, [x29]!, x7!
  CMP_IO(dci(0x1d56072b),
         R(x11, x22, x25),
         R(x11, x22, x25));  // cpym [x11]!, [x22]!, x25!
  CMP_IO(dci(0x1d5ec655),
         R(x21, x30, x18),
         R(x21, x30, x18));  // cpymn [x21]!, [x30]!, x18!
  CMP_IO(dci(0x1d558654),
         R(x20, x21, x18),
         R(x20, x21, x18));  // cpymrn [x20]!, [x21]!, x18!
  CMP_IO(dci(0x1d434458),
         R(x24, x3, x2),
         R(x24, x3, x2));  // cpymwn [x24]!, [x3]!, x2!
  CMP_IO(dci(0x1d1a054c),
         R(x12, x26, x10),
         R(x12, x26, x10));  // cpyp [x12]!, [x26]!, x10!
  CMP_IO(dci(0x1d01c66f),
         R(x15, x1, x19),
         R(x15, x1, x19));  // cpypn [x15]!, [x1]!, x19!
  CMP_IO(dci(0x1d0d86b0),
         R(x16, x13, x21),
         R(x16, x13, x21));  // cpyprn [x16]!, [x13]!, x21!
  CMP_IO(dci(0x1d1d45c9),
         R(x9, x29, x14),
         R(x9, x29, x14));                       // cpypwn [x9]!, [x29]!, x14!
  CMP_IO(dci(0x1ada414c), R(x12), R(x10, x26));  // crc32b w12, w10, w26
  CMP_IO(dci(0x1ade5171), R(x17), R(x11, x30));  // crc32cb w17, w11, w30
  CMP_IO(dci(0x1ade558b), R(x11), R(x12, x30));  // crc32ch w11, w12, w30
  CMP_IO(dci(0x1add5aee), R(x14), R(x23, x29));  // crc32cw w14, w23, w29
  CMP_IO(dci(0x9add5c92), R(x18), R(x4, x29));   // crc32cx w18, w4, x29
  CMP_IO(dci(0x1ada47c2), R(x2), R(x30, x26));   // crc32h w2, w30, w26
  CMP_IO(dci(0x1ad949f7), R(x23), R(x15, x25));  // crc32w w23, w15, w25
  CMP_IO(dci(0x9ad94f9b), R(x27), R(x28, x25));  // crc32x w27, w28, x25
  CMP_IO(dci(0xd503229f), R(), R());             // csdb
  CMP_IO(dci(0x1a9ad238), R(x24), R(x17, x26));  // csel w24, w17, w26, le
  CMP_IO(dci(0x9a9ab2fd), R(x29), R(x23, x26));  // csel x29, x23, x26, lt
  CMP_IO(dci(0x1a9e3643), R(x3), R(x18, x30));   // csinc w3, w18, w30, lo
  CMP_IO(dci(0x9a9e27da), R(x26), R(x30));       // cinc x26, x30, lo
  CMP_IO(dci(0x5a9dc3c0), R(x0), R(x30, x29));   // csinv w0, w30, w29, gt
  CMP_IO(dci(0xda9da085), R(x5), R(x4, x29));    // csinv x5, x4, x29, ge

  CLEANUP();
}

TEST(group_9) {
  SETUP();

  CMP_IO(dci(0x5a9e858b), R(x11), R(x12, x30));  // csneg w11, w12, w30, hi
  CMP_IO(dci(0xda9e7722), R(x2), R(x25, x30));   // csneg x2, x25, x30, vc
  CMP_IO(dci(0x25fd23a0), R(), R(x29, x29));     // ctermeq x29, x29
  CMP_IO(dci(0x25bc2290), R(), R(x20, x28));     // ctermne w20, w28
  CMP_IO(dci(0xd4bc18e1), R(), R());             // dcps1 {#0xe0c7}
  CMP_IO(dci(0xd4bc0e82), R(), R());             // dcps2 {#0xe074}
  CMP_IO(dci(0xd4bc0403), R(), R());             // dcps3 {#0xe020}
  CMP_IO(dci(0x0439e6c9), R(x9), R(x9));         // decb x9, #0x16, mul #10
  CMP_IO(dci(0x04fae53e), R(x30), R(x30));       // decd x30, vl16, mul #11
  CMP_IO(dci(0x04fac53f), R(v31), R(v31));       // decd z31.d, vl16, mul #11
  CMP_IO(dci(0x0479e453), R(x19), R(x19));       // dech x19, vl2, mul #10
  CMP_IO(dci(0x0479c453), R(v19), R(v19));       // dech z19.h, vl2, mul #10
  CMP_IO(dci(0x252d8837), R(x23), R(x23, p1));   // decp x23, p1.b
  CMP_IO(dci(0x252d8037), R(v23), R(v23, p1));   // decp z23.b, p1
  CMP_IO(dci(0x04bae566), R(x6), R(x6));         // decw x6, vl64, mul #11
  CMP_IO(dci(0x04bac566), R(v6), R(v6));         // decw z6.s, vl64, mul #11
  CMP_IO(dci(0xd50339bf), R(), R());             // dmb ishld
  CMP_IO(dci(0xd5033a9f), R(), R());             // dsb ishst
  CMP_IO(dci(0x0e1b0d80), R(v0), R(x12));        // dup v0.8b, w12
  CMP_IO(dci(0x4e0807a8), R(v8), R(v29));        // dup v8.2d, v29.d[0]
  CMP_IO(dci(0x5e1905f7), R(v23), R(v15));       // mov b23, v15.b[12]
  CMP_IO(dci(0x2578f132), R(v18), R());          // mov z18.h, #-119, lsl #8
  CMP_IO(dci(0x05a03ae3), R(v3), R(x23));        // mov z3.s, w23
  CMP_IO(dci(0x053e20ac), R(v12), R(v5));        // mov z12.h, z5.h[7]
  CMP_IO(dci(0x05c1a485), R(v5), R());           // mov z5.h, #0xf001
  CMP_IO(dci(0x4afc61ce), R(x14), R(x14, x28));  // eon w14, w14, w28, ror #24
  CMP_IO(dci(0xca3c6c37), R(x23), R(x1, x28));   // eon x23, x1, x28, lsl #27
  CMP_IO(dci(0x043b3b31),
         R(v17),
         R(v17, v27, v25));                // eor3 z17.d, z17.d, z27.d, z25.d
  CMP_IO(dci(0x523cd451), R(x17), R(x2));  // eor w17, w2, #0xf3f3f3f3
  CMP_IO(dci(0x4a5d3202), R(x2), R(x16, x29));  // eor w2, w16, w29, lsr #12
  CMP_IO(dci(0xd21cf38c), R(x12), R(x28));  // eor x12, x28, #0x5555555555555555
  CMP_IO(dci(0xca9d3c6b), R(x11), R(x3, x29));  // eor x11, x3, x29, asr #15

  CLEANUP();
}

TEST(group_10) {
  SETUP();

  CMP_IO(dci(0x2e3e1d98), R(v24), R(v12, v30));  // eor v24.8b, v12.8b, v30.8b
  CMP_IO(dci(0x250e7f22),
         R(p2),
         R(p15, p9, p14));  // eor p2.b, p15/z, p9.b, p14.b
  CMP_IO(dci(0x041912c7),
         R(v7),
         R(p4, v7, v22));                       // eor z7.b, p4/m, z7.b, z22.b
  CMP_IO(dci(0x0541e2fb), R(v27), R(v27));      // eor z27.s, z27.s, #0xffffff0
  CMP_IO(dci(0x04be3322), R(v2), R(v25, v30));  // eor z2.d, z25.d, z30.d
  CMP_IO(dci(0x459e9308), R(v8), R(v24, v30));  // eorbt z8.s, z24.s, z30.s
  CMP_IO(dci(0x25497b8b),
         R(p11),
         R(p14, p12, p9));  // eors p11.b, p14/z, p12.b, p9.b
  CMP_IO(dci(0x459e9708), R(v8), R(v24, v30));  // eortb z8.s, z24.s, z30.s
  CMP_IO(dci(0x04992923), R(v3), R(p2, v9));    // eorv s3, p2, z9.s
  CMP_IO(dci(0x2e041896), R(v22), R(v4, v4));   // ext v22.8b, v4.8b, v4.8b, #3
  CMP_IO(dci(0x057c0c37), R(v23), R(v1, v2));   // ext z23.b, {z1.b, z2.b}, #227
  CMP_IO(dci(0x053d12c7), R(v7), R(v7, v22));   // ext z7.b, z7.b, z22.b, #236
  CMP_IO(dci(0x139a106d), R(x13), R(x3, x26));  // extr w13, w3, w26, #4
  CMP_IO(dci(0x93da2fa8), R(x8), R(x29, x26));  // extr x8, x29, x26, #11
  CMP_IO(dci(0x6eb9d5f7), R(v23), R(v15, v25));   // fabd v23.4s, v15.4s, v25.4s
  CMP_IO(dci(0x6eda16f0), R(v16), R(v23, v26));   // fabd v16.8h, v23.8h, v26.8h
  CMP_IO(dci(0x7efad559), R(v25), R(v10, v26));   // fabd d25, d10, d26
  CMP_IO(dci(0x7ed91532), R(v18), R(v9, v25));    // fabd h18, h9, h25
  CMP_IO(dci(0x65889cc8), R(v8), R(p7, v8, v6));  // fabd z8.s, p7/m, z8.s, z6.s
  CMP_IO(dci(0x4ee0f9a2), R(v2), R(v13));         // fabs v2.2d, v13.2d
  CMP_IO(dci(0x0ef8fa29), R(v9), R(v17));         // fabs v9.4h, v17.4h
  CMP_IO(dci(0x1e60c13d), R(v29), R(v9));         // fabs d29, d9
  CMP_IO(dci(0x1ee0c051), R(v17), R(v2));         // fabs h17, h2
  CMP_IO(dci(0x1e20c044), R(v4), R(v2));          // fabs s4, s2
  CMP_IO(dci(0x049ca293), R(v19), R(v19, p0, v20));  // fabs z19.s, p0/m, z20.s
  CMP_IO(dci(0x2e26ec93), R(v19), R(v4, v6));    // facge v19.2s, v4.2s, v6.2s
  CMP_IO(dci(0x2e5e2d71), R(v17), R(v11, v30));  // facge v17.4h, v11.4h, v30.4h
  CMP_IO(dci(0x7e3eed98), R(v24), R(v12, v30));  // facge s24, s12, s30
  CMP_IO(dci(0x7e5e2c9f), R(v31), R(v4, v30));   // facge h31, h4, h30
  CMP_IO(dci(0x659cc293),
         R(p3),
         R(p0, v20, v28));                     // facge p3.s, p0/z, z20.s, z28.s
  CMP_IO(dci(0x6ee1ef45), R(v5), R(v26, v1));  // facgt v5.2d, v26.2d, v1.2d
  CMP_IO(dci(0x2edd2d4a), R(v10), R(v10, v29));  // facgt v10.4h, v10.4h, v29.4h

  CLEANUP();
}

TEST(group_11) {
  SETUP();

  CMP_IO(dci(0x7ebeec9f), R(v31), R(v4, v30));   // facgt s31, s4, s30
  CMP_IO(dci(0x7ede2d98), R(v24), R(v12, v30));  // facgt h24, h12, h30
  CMP_IO(dci(0x65c3fd78),
         R(p8),
         R(p7, v11, v3));                       // facgt p8.d, p7/z, z11.d, z3.d
  CMP_IO(dci(0x0e2bd415), R(v21), R(v0, v11));  // fadd v21.2s, v0.2s, v11.2s
  CMP_IO(dci(0x0e5a147a), R(v26), R(v3, v26));  // fadd v26.4h, v3.4h, v26.4h
  CMP_IO(dci(0x1e7c2b7f), R(v31), R(v27, v28));  // fadd d31, d27, d28
  CMP_IO(dci(0x1efc2a93), R(v19), R(v20, v28));  // fadd h19, h20, h28
  CMP_IO(dci(0x1e3d2a02), R(v2), R(v16, v29));   // fadd s2, s16, s29
  CMP_IO(dci(0x65d88021), R(v1), R(p0, v1));     // fadd z1.d, p0/m, z1.d, #1.0
  CMP_IO(dci(0x65409e52),
         R(v18),
         R(p7, v18, v18));  // fadd z18.h, p7/m, z18.h, z18.h
  CMP_IO(dci(0x65cc0053), R(v19), R(v2, v12));  // fadd z19.d, z2.d, z12.d
  CMP_IO(dci(0x655833eb),
         R(v11),
         R(p4, v11, v31));                       // fadda h11, p4, h11, z31.h
  CMP_IO(dci(0x6e61d745), R(v5), R(v26, v1));    // faddp v5.2d, v26.2d, v1.2d
  CMP_IO(dci(0x2e5d154a), R(v10), R(v10, v29));  // faddp v10.4h, v10.4h, v29.4h
  CMP_IO(dci(0x5e30dbb2), R(v18), R(v29));       // faddp h18, v29.2h
  CMP_IO(dci(0x7e30d9a7), R(v7), R(v13));        // faddp s7, v13.2s
  CMP_IO(dci(0x64509202),
         R(v2),
         R(p4, v2, v16));                       // faddp z2.h, p4/m, z2.h, z16.h
  CMP_IO(dci(0x65403e52), R(v18), R(p7, v18));  // faddv h18, p7, z18.h
  CMP_IO(dci(0x2e1bf717),
         R(v23),
         R(v24, v27));  // fcadd v23.8b, v24.8b, v27.8b, #270
  CMP_IO(dci(0x648095db),
         R(v27),
         R(p5, v27, v14));  // fcadd z27.s, p5/m, z27.s, z14.s, #90
  CMP_IO(dci(0x1e79f522), R(), R(v9, v25));      // fccmp d9, d25, #nzCv, nv
  CMP_IO(dci(0x1ef9c78e), R(), R(v28, v25));     // fccmp h28, h25, #NZCv, gt
  CMP_IO(dci(0x1e3ac7cf), R(), R(v30, v26));     // fccmp s30, s26, #NZCV, gt
  CMP_IO(dci(0x1e7df7dd), R(), R(v30, v29));     // fccmpe d30, d29, #NZcV, nv
  CMP_IO(dci(0x1efd76f1), R(), R(v23, v29));     // fccmpe h23, h29, #nzcV, vc
  CMP_IO(dci(0x1e3e97f4), R(), R(v31, v30));     // fccmpe s31, s30, #nZcv, ls
  CMP_IO(dci(0x4ee0d8c6), R(v6), R(v6));         // fcmeq v6.2d, v6.2d, #0.0
  CMP_IO(dci(0x4ef8dbcd), R(v13), R(v30));       // fcmeq v13.8h, v30.8h, #0.0
  CMP_IO(dci(0x4e3ee7f4), R(v20), R(v31, v30));  // fcmeq v20.4s, v31.4s, v30.4s
  CMP_IO(dci(0x4e5d26fb), R(v27), R(v23, v29));  // fcmeq v27.8h, v23.8h, v29.8h
  CMP_IO(dci(0x5ea0da1c), R(v28), R(v16));       // fcmeq s28, s16, #0.0
  CMP_IO(dci(0x5ef8da43), R(v3), R(v18));        // fcmeq h3, h18, #0.0

  CLEANUP();
}

TEST(group_12) {
  SETUP();

  CMP_IO(dci(0x5e7de6ee), R(v14), R(v23, v29));  // fcmeq d14, d23, d29
  CMP_IO(dci(0x5e5e2715), R(v21), R(v24, v30));  // fcmeq h21, h24, h30
  CMP_IO(dci(0x65922ee3), R(p3), R(p3, v23));  // fcmeq p3.s, p3/z, z23.s, #0.0
  CMP_IO(dci(0x65dd6789),
         R(p9),
         R(p1, v28, v29));                  // fcmeq p9.d, p1/z, z28.d, z29.d
  CMP_IO(dci(0x2ea0c964), R(v4), R(v11));   // fcmge v4.2s, v11.2s, #0.0
  CMP_IO(dci(0x2ef8c98b), R(v11), R(v12));  // fcmge v11.4h, v12.4h, #0.0
  CMP_IO(dci(0x2e3be55f), R(v31), R(v10, v27));  // fcmge v31.2s, v10.2s, v27.2s
  CMP_IO(dci(0x2e5e265d), R(v29), R(v18, v30));  // fcmge v29.4h, v18.4h, v30.4h
  CMP_IO(dci(0x7ee0c97e), R(v30), R(v11));       // fcmge d30, d11, #0.0
  CMP_IO(dci(0x7ef8c885), R(v5), R(v4));         // fcmge h5, h4, #0.0
  CMP_IO(dci(0x7e3ee4ac), R(v12), R(v5, v30));   // fcmge s12, s5, s30
  CMP_IO(dci(0x7e5d27b3), R(v19), R(v29, v29));  // fcmge h19, h29, h29
  CMP_IO(dci(0x65503d25), R(p5), R(p7, v9));     // fcmge p5.h, p7/z, z9.h, #0.0
  CMP_IO(dci(0x65d35f2b),
         R(p11),
         R(p7, v25, v19));                  // fcmge p11.d, p7/z, z25.d, z19.d
  CMP_IO(dci(0x0ea0c98b), R(v11), R(v12));  // fcmgt v11.2s, v12.2s, #0.0
  CMP_IO(dci(0x0ef8c964), R(v4), R(v11));   // fcmgt v4.4h, v11.4h, #0.0
  CMP_IO(dci(0x2ea8e53f), R(v31), R(v9, v8));    // fcmgt v31.2s, v9.2s, v8.2s
  CMP_IO(dci(0x2ede2636), R(v22), R(v17, v30));  // fcmgt v22.4h, v17.4h, v30.4h
  CMP_IO(dci(0x5ee0c885), R(v5), R(v4));         // fcmgt d5, d4, #0.0
  CMP_IO(dci(0x5ef8c97e), R(v30), R(v11));       // fcmgt h30, h11, #0.0
  CMP_IO(dci(0x7ebde7b3), R(v19), R(v29, v29));  // fcmgt s19, s29, s29
  CMP_IO(dci(0x7ede24ac), R(v12), R(v5, v30));   // fcmgt h12, h5, h30
  CMP_IO(dci(0x65d02e1e),
         R(p14),
         R(p3, v16));  // fcmgt p14.d, p3/z, z16.d, #0.0
  CMP_IO(dci(0x659d5078),
         R(p8),
         R(p4, v3, v29));  // fcmgt p8.s, p4/z, z3.s, z29.s
  CMP_IO(dci(0x6f5b70e2),
         R(v2),
         R(v2, v7, v11));  // fcmla v2.8h, v7.8h, v11.h[0], #270
  CMP_IO(dci(0x6fbb7b9a),
         R(v26),
         R(v26, v28, v11));  // fcmla v26.4s, v28.4s, v11.s[1], #270
  CMP_IO(dci(0x2e9bcd80),
         R(v0),
         R(v0, v12, v27));  // fcmla v0.2s, v12.2s, v27.2s, #90
  CMP_IO(dci(0x645b119a),
         R(v26),
         R(v26, p4, v12, v27));  // fcmla z26.h, p4/m, z12.h, z27.h, #0
  CMP_IO(dci(0x64bc1dc1),
         R(v1),
         R(v1, v14, v4));  // fcmla z1.h, z14.h, z4.h[3], #270
  CMP_IO(dci(0x64fd16d4),
         R(v20),
         R(v20, v22, v13));                 // fcmla z20.s, z22.s, z13.s[1], #90
  CMP_IO(dci(0x6ee0dae1), R(v1), R(v23));   // fcmle v1.2d, v23.2d, #0.0
  CMP_IO(dci(0x6ef8dbda), R(v26), R(v30));  // fcmle v26.8h, v30.8h, #0.0

  CLEANUP();
}

TEST(group_13) {
  SETUP();

  CMP_IO(dci(0x7ea0db2f), R(v15), R(v25));     // fcmle s15, s25, #0.0
  CMP_IO(dci(0x7ef8db08), R(v8), R(v24));      // fcmle h8, h24, #0.0
  CMP_IO(dci(0x65912fb8), R(p8), R(p3, v29));  // fcmle p8.s, p3/z, z29.s, #0.0
  CMP_IO(dci(0x4ee0ebda), R(v26), R(v30));     // fcmlt v26.2d, v30.2d, #0.0
  CMP_IO(dci(0x4ef8eae1), R(v1), R(v23));      // fcmlt v1.8h, v23.8h, #0.0
  CMP_IO(dci(0x5ea0eb08), R(v8), R(v24));      // fcmlt s8, s24, #0.0
  CMP_IO(dci(0x5ef8eb2f), R(v15), R(v25));     // fcmlt h15, h25, #0.0
  CMP_IO(dci(0x65d13d4b),
         R(p11),
         R(p7, v10));                          // fcmlt p11.d, p7/z, z10.d, #0.0
  CMP_IO(dci(0x65d33ac6), R(p6), R(p6, v22));  // fcmne p6.d, p6/z, z22.d, #0.0
  CMP_IO(dci(0x659c76b0),
         R(p0),
         R(p5, v21, v28));                    // fcmne p0.s, p5/z, z21.s, z28.s
  CMP_IO(dci(0x1e792120), R(), R(v9, v25));   // fcmp d9, d25
  CMP_IO(dci(0x1e7e20a8), R(), R(v5));        // fcmp d5, #0.0
  CMP_IO(dci(0x1ef92380), R(), R(v28, v25));  // fcmp h28, h25
  CMP_IO(dci(0x1efe2188), R(), R(v12));       // fcmp h12, #0.0
  CMP_IO(dci(0x1e3a23c0), R(), R(v30, v26));  // fcmp s30, s26
  CMP_IO(dci(0x1e3d2088), R(), R(v4));        // fcmp s4, #0.0
  CMP_IO(dci(0x1e7d23d0), R(), R(v30, v29));  // fcmpe d30, d29
  CMP_IO(dci(0x1e7a2098), R(), R(v4));        // fcmpe d4, #0.0
  CMP_IO(dci(0x1efd22f0), R(), R(v23, v29));  // fcmpe h23, h29
  CMP_IO(dci(0x1efa2238), R(), R(v17));       // fcmpe h17, #0.0
  CMP_IO(dci(0x1e3e23f0), R(), R(v31, v30));  // fcmpe s31, s30
  CMP_IO(dci(0x1e3921f8), R(), R(v15));       // fcmpe s15, #0.0
  CMP_IO(dci(0x6549c4e3),
         R(p3),
         R(p1, v7, v9));  // fcmuo p3.h, p1/z, z7.h, z9.h
  CMP_IO(dci(0x05d3d11c),
         R(v28),
         R(v28, p3));  // fmov z28.d, p3/m, #0x88 (-3.0000)
  CMP_IO(dci(0x1e7deefb), R(v27), R(v23, v29));  // fcsel d27, d23, d29, al
  CMP_IO(dci(0x1efdbd57), R(v23), R(v10, v29));  // fcsel h23, h10, h29, lt
  CMP_IO(dci(0x1e3eacc6), R(v6), R(v6, v30));    // fcsel s6, s6, s30, ge
  CMP_IO(dci(0x1ee2c0ae), R(v14), R(v5));        // fcvt d14, h5
  CMP_IO(dci(0x1e22c0ef), R(v15), R(v7));        // fcvt d15, s7
  CMP_IO(dci(0x1e63c0ae), R(v14), R(v5));        // fcvt h14, d5
  CMP_IO(dci(0x1e23c003), R(v3), R(v0));         // fcvt h3, s0
  CMP_IO(dci(0x1e6240ef), R(v15), R(v7));        // fcvt s15, d7

  CLEANUP();
}

TEST(group_14) {
  SETUP();

  CMP_IO(dci(0x1ee24003), R(v3), R(v0));             // fcvt s3, h0
  CMP_IO(dci(0x65c8bc03), R(v3), R(v3, p7, v0));     // fcvt z3.h, p7/m, z0.d
  CMP_IO(dci(0x65cabe52), R(v18), R(v18, p7, v18));  // fcvt z18.s, p7/m, z18.d
  CMP_IO(dci(0x65c9bc03), R(v3), R(v3, p7, v0));     // fcvt z3.d, p7/m, z0.h
  CMP_IO(dci(0x6589b4ae), R(v14), R(v14, p5, v5));   // fcvt z14.s, p5/m, z5.h
  CMP_IO(dci(0x65cbbe52), R(v18), R(v18, p7, v18));  // fcvt z18.d, p7/m, z18.s
  CMP_IO(dci(0x6588b4ae), R(v14), R(v14, p5, v5));   // fcvt z14.h, p5/m, z5.s
  CMP_IO(dci(0x1e64034b), R(x11), R(v26));           // fcvtas w11, d26
  CMP_IO(dci(0x1ee401a7), R(x7), R(v13));            // fcvtas w7, h13
  CMP_IO(dci(0x1e2403f6), R(x22), R(v31));           // fcvtas w22, s31
  CMP_IO(dci(0x9e6400e2), R(x2), R(v7));             // fcvtas x2, d7
  CMP_IO(dci(0x9ee4033e), R(x30), R(v25));           // fcvtas x30, h25
  CMP_IO(dci(0x9e24025f), R(), R(v18));              // fcvtas xzr, s18
  CMP_IO(dci(0x0e21c98b), R(v11), R(v12));           // fcvtas v11.2s, v12.2s
  CMP_IO(dci(0x0e79c964), R(v4), R(v11));            // fcvtas v4.4h, v11.4h
  CMP_IO(dci(0x5e61c885), R(v5), R(v4));             // fcvtas d5, d4
  CMP_IO(dci(0x5e79c97e), R(v30), R(v11));           // fcvtas h30, h11
  CMP_IO(dci(0x1e65001d), R(x29), R(v0));            // fcvtau w29, d0
  CMP_IO(dci(0x1ee50331), R(x17), R(v25));           // fcvtau w17, h25
  CMP_IO(dci(0x1e250324), R(x4), R(v25));            // fcvtau w4, s25
  CMP_IO(dci(0x9e650358), R(x24), R(v26));           // fcvtau x24, d26
  CMP_IO(dci(0x9ee5026c), R(x12), R(v19));           // fcvtau x12, h19
  CMP_IO(dci(0x9e2503e9), R(x9), R(v31));            // fcvtau x9, s31
  CMP_IO(dci(0x2e21c93f), R(v31), R(v9));            // fcvtau v31.2s, v9.2s
  CMP_IO(dci(0x2e79ca36), R(v22), R(v17));           // fcvtau v22.4h, v17.4h
  CMP_IO(dci(0x7e21cbb3), R(v19), R(v29));           // fcvtau s19, s29
  CMP_IO(dci(0x7e79c8ac), R(v12), R(v5));            // fcvtau h12, h5
  CMP_IO(dci(0x0e217a2b), R(v11), R(v17));           // fcvtl v11.4s, v17.4h
  CMP_IO(dci(0x6489ae86), R(v6), R(v6, p3, v20));    // fcvtlt z6.s, p3/m, z20.h
  CMP_IO(dci(0x64cbb19a),
         R(v26),
         R(v26, p4, v12));                  // fcvtlt z26.d, p4/m, z12.s
  CMP_IO(dci(0x1e7001a7), R(x7), R(v13));   // fcvtms w7, d13
  CMP_IO(dci(0x1ef0034b), R(x11), R(v26));  // fcvtms w11, h26

  CLEANUP();
}

TEST(group_15) {
  SETUP();

  CMP_IO(dci(0x1e30030a), R(x10), R(v24));  // fcvtms w10, s24
  CMP_IO(dci(0x9e70033e), R(x30), R(v25));  // fcvtms x30, d25
  CMP_IO(dci(0x9ef000e2), R(x2), R(v7));    // fcvtms x2, h7
  CMP_IO(dci(0x9e300173), R(x19), R(v11));  // fcvtms x19, s11
  CMP_IO(dci(0x4e21bbe7), R(v7), R(v31));   // fcvtms v7.4s, v31.4s
  CMP_IO(dci(0x4e79bbc0), R(v0), R(v30));   // fcvtms v0.8h, v30.8h
  CMP_IO(dci(0x5e61ba29), R(v9), R(v17));   // fcvtms d9, d17
  CMP_IO(dci(0x5e79bb22), R(v2), R(v25));   // fcvtms h2, h25
  CMP_IO(dci(0x1e710331), R(x17), R(v25));  // fcvtmu w17, d25
  CMP_IO(dci(0x1ef1001d), R(x29), R(v0));   // fcvtmu w29, h0
  CMP_IO(dci(0x1e310180), R(x0), R(v12));   // fcvtmu w0, s12
  CMP_IO(dci(0x9e71026c), R(x12), R(v19));  // fcvtmu x12, d19
  CMP_IO(dci(0x9ef10358), R(x24), R(v26));  // fcvtmu x24, h26
  CMP_IO(dci(0x9e310245), R(x5), R(v18));   // fcvtmu x5, s18
  CMP_IO(dci(0x6e61bb45), R(v5), R(v26));   // fcvtmu v5.2d, v26.2d
  CMP_IO(dci(0x2e79b94a), R(v10), R(v10));  // fcvtmu v10.4h, v10.4h
  CMP_IO(dci(0x7e21b89f), R(v31), R(v4));   // fcvtmu s31, s4
  CMP_IO(dci(0x7e79b998), R(v24), R(v12));  // fcvtmu h24, h12
  CMP_IO(dci(0x0e216a2b), R(v11), R(v17));  // fcvtn v11.4h, v17.4s
  CMP_IO(dci(0x1e60033e), R(x30), R(v25));  // fcvtns w30, d25
  CMP_IO(dci(0x1ee000e2), R(x2), R(v7));    // fcvtns w2, h7
  CMP_IO(dci(0x1e200173), R(x19), R(v11));  // fcvtns w19, s11
  CMP_IO(dci(0x9e6001a7), R(x7), R(v13));   // fcvtns x7, d13
  CMP_IO(dci(0x9ee0034b), R(x11), R(v26));  // fcvtns x11, h26
  CMP_IO(dci(0x9e20030a), R(x10), R(v24));  // fcvtns x10, s24
  CMP_IO(dci(0x4e21aace), R(v14), R(v22));  // fcvtns v14.4s, v22.4s
  CMP_IO(dci(0x0e79a885), R(v5), R(v4));    // fcvtns v5.4h, v4.4h
  CMP_IO(dci(0x5e21a964), R(v4), R(v11));   // fcvtns s4, s11
  CMP_IO(dci(0x5e79a98b), R(v11), R(v12));  // fcvtns h11, h12
  CMP_IO(dci(0x64caa66c),
         R(v12),
         R(v12, p1, v19));  // fcvtnt z12.s, p1/m, z19.d
  CMP_IO(dci(0x6488a358),
         R(v24),
         R(v24, p0, v26));                  // fcvtnt z24.h, p0/m, z26.s
  CMP_IO(dci(0x1e61026c), R(x12), R(v19));  // fcvtnu w12, d19

  CLEANUP();
}

TEST(group_16) {
  SETUP();

  CMP_IO(dci(0x1ee10358), R(x24), R(v26));           // fcvtnu w24, h26
  CMP_IO(dci(0x1e210245), R(x5), R(v18));            // fcvtnu w5, s18
  CMP_IO(dci(0x9e610331), R(x17), R(v25));           // fcvtnu x17, d25
  CMP_IO(dci(0x9ee1001d), R(x29), R(v0));            // fcvtnu x29, h0
  CMP_IO(dci(0x9e210180), R(x0), R(v12));            // fcvtnu x0, s12
  CMP_IO(dci(0x6e21a8ac), R(v12), R(v5));            // fcvtnu v12.4s, v5.4s
  CMP_IO(dci(0x6e79abb3), R(v19), R(v29));           // fcvtnu v19.8h, v29.8h
  CMP_IO(dci(0x7e61aa36), R(v22), R(v17));           // fcvtnu d22, d17
  CMP_IO(dci(0x7e79aa5d), R(v29), R(v18));           // fcvtnu h29, h18
  CMP_IO(dci(0x1e680324), R(x4), R(v25));            // fcvtps w4, d25
  CMP_IO(dci(0x1ee80180), R(x0), R(v12));            // fcvtps w0, h12
  CMP_IO(dci(0x1e28001d), R(x29), R(v0));            // fcvtps w29, s0
  CMP_IO(dci(0x9e6803e9), R(x9), R(v31));            // fcvtps x9, d31
  CMP_IO(dci(0x9ee80245), R(x5), R(v18));            // fcvtps x5, h18
  CMP_IO(dci(0x9e280358), R(x24), R(v26));           // fcvtps x24, s26
  CMP_IO(dci(0x0ea1a964), R(v4), R(v11));            // fcvtps v4.2s, v11.2s
  CMP_IO(dci(0x0ef9a98b), R(v11), R(v12));           // fcvtps v11.4h, v12.4h
  CMP_IO(dci(0x5ee1a97e), R(v30), R(v11));           // fcvtps d30, d11
  CMP_IO(dci(0x5ef9a885), R(v5), R(v4));             // fcvtps h5, h4
  CMP_IO(dci(0x1e6903f6), R(x22), R(v31));           // fcvtpu w22, d31
  CMP_IO(dci(0x1ee9030a), R(x10), R(v24));           // fcvtpu w10, h24
  CMP_IO(dci(0x1e29034b), R(x11), R(v26));           // fcvtpu w11, s26
  CMP_IO(dci(0x9e69025f), R(), R(v18));              // fcvtpu xzr, d18
  CMP_IO(dci(0x9ee90173), R(x19), R(v11));           // fcvtpu x19, h11
  CMP_IO(dci(0x9e2900e2), R(x2), R(v7));             // fcvtpu x2, s7
  CMP_IO(dci(0x2ea1a95f), R(v31), R(v10));           // fcvtpu v31.2s, v10.2s
  CMP_IO(dci(0x2ef9aa5d), R(v29), R(v18));           // fcvtpu v29.4h, v18.4h
  CMP_IO(dci(0x7ea1a8ac), R(v12), R(v5));            // fcvtpu s12, s5
  CMP_IO(dci(0x7ef9abb3), R(v19), R(v29));           // fcvtpu h19, h29
  CMP_IO(dci(0x650abeba), R(v26), R(v26, p7, v21));  // fcvtx z26.s, p7/m, z21.d
  CMP_IO(dci(0x6e61695f), R(v31), R(v10));           // fcvtxn2 v31.4s, v10.2d
  CMP_IO(dci(0x7e616a5d), R(v29), R(v18));           // fcvtxn s29, d18

  CLEANUP();
}

TEST(group_17) {
  SETUP();

  CMP_IO(dci(0x640aa044), R(v4), R(v4, p0, v2));   // fcvtxnt z4.s, p0/m, z2.d
  CMP_IO(dci(0x1e5894ae), R(x14), R(v5));          // fcvtzs w14, d5, #27
  CMP_IO(dci(0x1e78030a), R(x10), R(v24));         // fcvtzs w10, d24
  CMP_IO(dci(0x1ed8be52), R(x18), R(v18));         // fcvtzs w18, h18, #17
  CMP_IO(dci(0x1ef803f6), R(x22), R(v31));         // fcvtzs w22, h31
  CMP_IO(dci(0x1e189c03), R(x3), R(v0));           // fcvtzs w3, s0, #25
  CMP_IO(dci(0x1e3801a7), R(x7), R(v13));          // fcvtzs w7, s13
  CMP_IO(dci(0x9e581f17), R(x23), R(v24));         // fcvtzs x23, d24, #57
  CMP_IO(dci(0x9e780173), R(x19), R(v11));         // fcvtzs x19, d11
  CMP_IO(dci(0x9ed848bb), R(x27), R(v5));          // fcvtzs x27, h5, #46
  CMP_IO(dci(0x9ef8025f), R(), R(v18));            // fcvtzs xzr, h18
  CMP_IO(dci(0x9e18919a), R(x26), R(v12));         // fcvtzs x26, s12, #28
  CMP_IO(dci(0x9e38033e), R(x30), R(v25));         // fcvtzs x30, s25
  CMP_IO(dci(0x0ea1b893), R(v19), R(v4));          // fcvtzs v19.2s, v4.2s
  CMP_IO(dci(0x0ef9b971), R(v17), R(v11));         // fcvtzs v17.4h, v11.4h
  CMP_IO(dci(0x4f42ffc2), R(v2), R(v30));          // fcvtzs v2.2d, v30.2d, #62
  CMP_IO(dci(0x5ea1b998), R(v24), R(v12));         // fcvtzs s24, s12
  CMP_IO(dci(0x5ef9b89f), R(v31), R(v4));          // fcvtzs h31, h4
  CMP_IO(dci(0x5f11fc60), R(v0), R(v3));           // fcvtzs h0, h3, #15
  CMP_IO(dci(0x65d8aa45), R(v5), R(v5, p2, v18));  // fcvtzs z5.s, p2/m, z18.d
  CMP_IO(dci(0x65deb324), R(v4), R(v4, p4, v25));  // fcvtzs z4.d, p4/m, z25.d
  CMP_IO(dci(0x655abf7f),
         R(v31),
         R(v31, p7, v27));  // fcvtzs z31.h, p7/m, z27.h
  CMP_IO(dci(0x655cbeba),
         R(v26),
         R(v26, p7, v21));  // fcvtzs z26.s, p7/m, z21.h
  CMP_IO(dci(0x655ea60f),
         R(v15),
         R(v15, p1, v16));  // fcvtzs z15.d, p1/m, z16.h
  CMP_IO(dci(0x659ca66c),
         R(v12),
         R(v12, p1, v19));                          // fcvtzs z12.s, p1/m, z19.s
  CMP_IO(dci(0x65dca41d), R(v29), R(v29, p1, v0));  // fcvtzs z29.d, p1/m, z0.s
  CMP_IO(dci(0x1e59ffdc), R(x28), R(v30));          // fcvtzu w28, d30, #1
  CMP_IO(dci(0x1e790180), R(x0), R(v12));           // fcvtzu w0, d12
  CMP_IO(dci(0x1ed9fcc8), R(x8), R(v6));            // fcvtzu w8, h6, #1
  CMP_IO(dci(0x1ef90324), R(x4), R(v25));           // fcvtzu w4, h25
  CMP_IO(dci(0x1e19b0d5), R(x21), R(v6));           // fcvtzu w21, s6, #20
  CMP_IO(dci(0x1e390331), R(x17), R(v25));          // fcvtzu w17, s25

  CLEANUP();
}

TEST(group_18) {
  SETUP();

  CMP_IO(dci(0x9e59e0a1), R(x1), R(v5));    // fcvtzu x1, d5, #8
  CMP_IO(dci(0x9e790245), R(x5), R(v18));   // fcvtzu x5, d18
  CMP_IO(dci(0x9ed95d8d), R(x13), R(v12));  // fcvtzu x13, h12, #41
  CMP_IO(dci(0x9ef903e9), R(x9), R(v31));   // fcvtzu x9, h31
  CMP_IO(dci(0x9e19d010), R(x16), R(v0));   // fcvtzu x16, s0, #12
  CMP_IO(dci(0x9e39026c), R(x12), R(v19));  // fcvtzu x12, s19
  CMP_IO(dci(0x6ea1bbc0), R(v0), R(v30));   // fcvtzu v0.4s, v30.4s
  CMP_IO(dci(0x6ef9bbe7), R(v7), R(v31));   // fcvtzu v7.8h, v31.8h
  CMP_IO(dci(0x6f22fc27), R(v7), R(v1));    // fcvtzu v7.4s, v1.4s, #30
  CMP_IO(dci(0x7ee1bb22), R(v2), R(v25));   // fcvtzu d2, d25
  CMP_IO(dci(0x7ef9ba29), R(v9), R(v17));   // fcvtzu h9, h17
  CMP_IO(dci(0x7f21fdea), R(v10), R(v15));  // fcvtzu s10, s15, #31
  CMP_IO(dci(0x65d9b573),
         R(v19),
         R(v19, p5, v11));  // fcvtzu z19.s, p5/m, z11.d
  CMP_IO(dci(0x65dfa7f6),
         R(v22),
         R(v22, p1, v31));                          // fcvtzu z22.d, p1/m, z31.d
  CMP_IO(dci(0x655ba109), R(v9), R(v9, p0, v8));    // fcvtzu z9.h, p0/m, z8.h
  CMP_IO(dci(0x655dbd30), R(v16), R(v16, p7, v9));  // fcvtzu z16.s, p7/m, z9.h
  CMP_IO(dci(0x655fa799),
         R(v25),
         R(v25, p1, v28));  // fcvtzu z25.d, p1/m, z28.h
  CMP_IO(dci(0x659dbb3e),
         R(v30),
         R(v30, p6, v25));  // fcvtzu z30.s, p6/m, z25.s
  CMP_IO(dci(0x65ddaf4b),
         R(v11),
         R(v11, p3, v26));                        // fcvtzu z11.d, p3/m, z26.s
  CMP_IO(dci(0x2e3afe2b), R(v11), R(v17, v26));   // fdiv v11.2s, v17.2s, v26.2s
  CMP_IO(dci(0x2e5a3e04), R(v4), R(v16, v26));    // fdiv v4.4h, v16.4h, v26.4h
  CMP_IO(dci(0x1e7c19f5), R(v21), R(v15, v28));   // fdiv d21, d15, d28
  CMP_IO(dci(0x1efd1b99), R(v25), R(v28, v29));   // fdiv h25, h28, h29
  CMP_IO(dci(0x1e3c18fc), R(v28), R(v7, v28));    // fdiv s28, s7, s28
  CMP_IO(dci(0x65cd8024), R(v4), R(p0, v4, v1));  // fdiv z4.d, p0/m, z4.d, z1.d
  CMP_IO(dci(0x654c982a),
         R(v10),
         R(p6, v10, v1));                  // fdivr z10.h, p6/m, z10.h, z1.h
  CMP_IO(dci(0x25b9d708), R(v8), R());     // fmov z8.s, #0xb8 (-24.0000)
  CMP_IO(dci(0x0460b87a), R(v26), R(v3));  // fexpa z26.h, z3.h
  CMP_IO(dci(0x1e7e0044), R(x4), R(v2));   // fjcvtzs w4, d2
  CMP_IO(dci(0x651ab0d5), R(v21), R(v21, p4, v6));  // flogb z21.h, p4/m, z6.h
  CMP_IO(dci(0x65bd9c6b),
         R(v11),
         R(v11, p7, v3, v29));  // fmad z11.s, p7/m, z3.s, z29.s
  CMP_IO(dci(0x1f5a7573),
         R(v19),
         R(v11, v26, v29));  // fmadd d19, d11, d26, d29

  CLEANUP();
}

TEST(group_19) {
  SETUP();

  CMP_IO(dci(0x1fdb725f),
         R(v31),
         R(v18, v27, v28));  // fmadd h31, h18, h27, h28
  CMP_IO(dci(0x1f1b3b3e),
         R(v30),
         R(v25, v27, v14));                      // fmadd s30, s25, s27, s14
  CMP_IO(dci(0x0e2df6ee), R(v14), R(v23, v13));  // fmax v14.2s, v23.2s, v13.2s
  CMP_IO(dci(0x0e593525), R(v5), R(v9, v25));    // fmax v5.4h, v9.4h, v25.4h
  CMP_IO(dci(0x1e7d4ad4), R(v20), R(v22, v29));  // fmax d20, d22, d29
  CMP_IO(dci(0x1efd4930), R(v16), R(v9, v29));   // fmax h16, h9, h29
  CMP_IO(dci(0x1e3c4aad), R(v13), R(v21, v28));  // fmax s13, s21, s28
  CMP_IO(dci(0x65de983e), R(v30), R(p6, v30));  // fmax z30.d, p6/m, z30.d, #1.0
  CMP_IO(dci(0x6546841d),
         R(v29),
         R(p1, v29, v0));                      // fmax z29.h, p1/m, z29.h, z0.h
  CMP_IO(dci(0x4e3ac487), R(v7), R(v4, v26));  // fmaxnm v7.4s, v4.4s, v26.4s
  CMP_IO(dci(0x0e590460), R(v0), R(v3, v25));  // fmaxnm v0.4h, v3.4h, v25.4h
  CMP_IO(dci(0x1e7d6b99), R(v25), R(v28, v29));  // fmaxnm d25, d28, d29
  CMP_IO(dci(0x1efc69f5), R(v21), R(v15, v28));  // fmaxnm h21, h15, h28
  CMP_IO(dci(0x1e3c69e8), R(v8), R(v15, v28));   // fmaxnm s8, s15, s28
  CMP_IO(dci(0x65dc8c03), R(v3), R(p3, v3));  // fmaxnm z3.d, p3/m, z3.d, #0.0
  CMP_IO(dci(0x65848358),
         R(v24),
         R(p0, v24, v26));  // fmaxnm z24.s, p0/m, z24.s, z26.s
  CMP_IO(dci(0x2e3dc557),
         R(v23),
         R(v10, v29));  // fmaxnmp v23.2s, v10.2s, v29.2s
  CMP_IO(dci(0x2e5e0650),
         R(v16),
         R(v18, v30));                     // fmaxnmp v16.4h, v18.4h, v30.4h
  CMP_IO(dci(0x5e30c878), R(v24), R(v3));  // fmaxnmp h24, v3.2h
  CMP_IO(dci(0x7e70c8a1), R(v1), R(v5));   // fmaxnmp d1, v5.2d
  CMP_IO(dci(0x649489e8),
         R(v8),
         R(p2, v8, v15));                   // fmaxnmp z8.s, p2/m, z8.s, z15.s
  CMP_IO(dci(0x4e30c98d), R(v13), R(v12));  // fmaxnmv h13, v12.8h
  CMP_IO(dci(0x6e30cac4), R(v4), R(v22));   // fmaxnmv s4, v22.4s
  CMP_IO(dci(0x65842358), R(v24), R(p0, v26));   // fmaxnmv s24, p0, z26.s
  CMP_IO(dci(0x6e7df6ee), R(v14), R(v23, v29));  // fmaxp v14.2d, v23.2d, v29.2d
  CMP_IO(dci(0x6e5e3715), R(v21), R(v24, v30));  // fmaxp v21.8h, v24.8h, v30.8h
  CMP_IO(dci(0x5e30f921), R(v1), R(v9));         // fmaxp h1, v9.2h
  CMP_IO(dci(0x7e30fbdc), R(v28), R(v30));       // fmaxp s28, v30.2s
  CMP_IO(dci(0x64568aad),
         R(v13),
         R(p2, v13, v21));                 // fmaxp z13.h, p2/m, z13.h, z21.h
  CMP_IO(dci(0x4e30f8c8), R(v8), R(v6));   // fmaxv h8, v6.8h
  CMP_IO(dci(0x6e30f91c), R(v28), R(v8));  // fmaxv s28, v8.4s
  CMP_IO(dci(0x6546241d), R(v29), R(p1, v0));  // fmaxv h29, p1, z0.h

  CLEANUP();
}

TEST(group_20) {
  SETUP();

  CMP_IO(dci(0x0ebaf604), R(v4), R(v16, v26));   // fmin v4.2s, v16.2s, v26.2s
  CMP_IO(dci(0x0eda362b), R(v11), R(v17, v26));  // fmin v11.4h, v17.4h, v26.4h
  CMP_IO(dci(0x1e7c59ce), R(v14), R(v14, v28));  // fmin d14, d14, d28
  CMP_IO(dci(0x1efc5b72), R(v18), R(v27, v28));  // fmin h18, h27, h28
  CMP_IO(dci(0x1e3c5923), R(v3), R(v9, v28));    // fmin s3, s9, s28
  CMP_IO(dci(0x655f983c), R(v28), R(p6, v28));  // fmin z28.h, p6/m, z28.h, #1.0
  CMP_IO(dci(0x65879573),
         R(v19),
         R(p5, v19, v11));  // fmin z19.s, p5/m, z19.s, z11.s
  CMP_IO(dci(0x4ef9c6c9), R(v9), R(v22, v25));   // fminnm v9.2d, v22.2d, v25.2d
  CMP_IO(dci(0x4eda07c2), R(v2), R(v30, v26));   // fminnm v2.8h, v30.8h, v26.8h
  CMP_IO(dci(0x1e7c7837), R(v23), R(v1, v28));   // fminnm d23, d1, d28
  CMP_IO(dci(0x1efc79db), R(v27), R(v14, v28));  // fminnm h27, h14, h28
  CMP_IO(dci(0x1e3c7aba), R(v26), R(v21, v28));  // fminnm s26, s21, s28
  CMP_IO(dci(0x65dd9029), R(v9), R(p4, v9));  // fminnm z9.d, p4/m, z9.d, #1.0
  CMP_IO(dci(0x65458b0a),
         R(v10),
         R(p2, v10, v24));  // fminnm z10.h, p2/m, z10.h, z24.h
  CMP_IO(dci(0x6efec4b9), R(v25), R(v5, v30));  // fminnmp v25.2d, v5.2d, v30.2d
  CMP_IO(dci(0x2edd0492), R(v18), R(v4, v29));  // fminnmp v18.4h, v4.4h, v29.4h
  CMP_IO(dci(0x5eb0ca73), R(v19), R(v19));      // fminnmp h19, v19.2h
  CMP_IO(dci(0x7eb0c8ef), R(v15), R(v7));       // fminnmp s15, v7.2s
  CMP_IO(dci(0x64559eba),
         R(v26),
         R(p7, v26, v21));                 // fminnmp z26.h, p7/m, z26.h, z21.h
  CMP_IO(dci(0x4eb0c803), R(v3), R(v0));   // fminnmv h3, v0.8h
  CMP_IO(dci(0x6eb0c85e), R(v30), R(v2));  // fminnmv s30, v2.4s
  CMP_IO(dci(0x65452b0a), R(v10), R(p2, v24));   // fminnmv h10, p2, z24.h
  CMP_IO(dci(0x6ebef7f4), R(v20), R(v31, v30));  // fminp v20.4s, v31.4s, v30.4s
  CMP_IO(dci(0x6edd36fb), R(v27), R(v23, v29));  // fminp v27.8h, v23.8h, v29.8h
  CMP_IO(dci(0x5eb0fa93), R(v19), R(v20));       // fminp h19, v20.2h
  CMP_IO(dci(0x7ef0fa86), R(v6), R(v20));        // fminp d6, v20.2d
  CMP_IO(dci(0x64978923),
         R(v3),
         R(p2, v3, v9));                        // fminp z3.s, p2/m, z3.s, z9.s
  CMP_IO(dci(0x4eb0f99a), R(v26), R(v12));      // fminv h26, v12.8h
  CMP_IO(dci(0x6eb0fa37), R(v23), R(v17));      // fminv s23, v17.4s
  CMP_IO(dci(0x65873573), R(v19), R(p5, v11));  // fminv s19, p5, z11.s
  CMP_IO(dci(0x4ffb10d5),
         R(v21),
         R(v21, v6, v11));  // fmla v21.2d, v6.2d, v11.d[0]
  CMP_IO(dci(0x4f1a1bdc),
         R(v28),
         R(v28, v30, v10));  // fmla v28.8h, v30.8h, v10.h[5]

  CLEANUP();
}

TEST(group_21) {
  SETUP();

  CMP_IO(dci(0x4e79cdea),
         R(v10),
         R(v10, v15, v25));  // fmla v10.2d, v15.2d, v25.2d
  CMP_IO(dci(0x0e5a0e11),
         R(v17),
         R(v17, v16, v26));  // fmla v17.4h, v16.4h, v26.4h
  CMP_IO(dci(0x5fbb18bb), R(v27), R(v27, v5, v11));  // fmla s27, s5, v11.s[3]
  CMP_IO(dci(0x5f1c1a86), R(v6), R(v6, v20, v12));   // fmla h6, h20, v12.h[5]
  CMP_IO(dci(0x65bc0923),
         R(v3),
         R(v3, p2, v9, v28));  // fmla z3.s, p2/m, z9.s, z28.s
  CMP_IO(dci(0x64fb00c8), R(v8), R(v8, v6, v11));  // fmla z8.d, z6.d, z11.d[1]
  CMP_IO(dci(0x643a03dc),
         R(v28),
         R(v28, v30, v2));  // fmla z28.h, z30.h, z2.h[3]
  CMP_IO(dci(0x64bb0279),
         R(v25),
         R(v25, v19, v3));  // fmla z25.s, z19.s, z3.s[3]
  CMP_IO(dci(0x6fbc89f5),
         R(v21),
         R(v21, v15, v12));  // fmlal2 v21.4s, v15.4h, v12.h[7]
  CMP_IO(dci(0x6e3bcff6),
         R(v22),
         R(v22, v31, v27));  // fmlal2 v22.4s, v31.4h, v27.4h
  CMP_IO(dci(0x0fbd0957),
         R(v23),
         R(v23, v10, v13));  // fmlal v23.2s, v10.2h, v13.h[7]
  CMP_IO(dci(0x0e3aee38),
         R(v24),
         R(v24, v17, v26));  // fmlal v24.2s, v17.2h, v26.2h
  CMP_IO(dci(0x64be80ac),
         R(v12),
         R(v12, v5, v30));  // fmlalb z12.s, z5.h, z30.h
  CMP_IO(dci(0x64bb41a7),
         R(v7),
         R(v7, v13, v3));  // fmlalb z7.s, z13.h, z3.h[6]
  CMP_IO(dci(0x64bd86ee),
         R(v14),
         R(v14, v23, v29));                       // fmlalt z14.s, z23.h, z29.h
  CMP_IO(dci(0x64ba44a1), R(v1), R(v1, v5, v2));  // fmlalt z1.s, z5.h, z2.h[6]
  CMP_IO(dci(0x0fbb5b17),
         R(v23),
         R(v23, v24, v11));  // fmls v23.2s, v24.2s, v11.s[3]
  CMP_IO(dci(0x4f1b519a),
         R(v26),
         R(v26, v12, v11));  // fmls v26.8h, v12.8h, v11.h[1]
  CMP_IO(dci(0x0ebacd4c),
         R(v12),
         R(v12, v10, v26));  // fmls v12.2s, v10.2s, v26.2s
  CMP_IO(dci(0x0ed90c53),
         R(v19),
         R(v19, v2, v25));  // fmls v19.4h, v2.4h, v25.4h
  CMP_IO(dci(0x5ffb5a79), R(v25), R(v25, v19, v11));  // fmls d25, d19, v11.d[1]
  CMP_IO(dci(0x5f1b58c8), R(v8), R(v8, v6, v11));     // fmls h8, h6, v11.h[5]
  CMP_IO(dci(0x65fc2dc1),
         R(v1),
         R(v1, p3, v14, v28));  // fmls z1.d, p3/m, z14.d, z28.d
  CMP_IO(dci(0x64fc0686),
         R(v6),
         R(v6, v20, v12));  // fmls z6.d, z20.d, z12.d[1]
  CMP_IO(dci(0x647b059a),
         R(v26),
         R(v26, v12, v3));  // fmls z26.h, z12.h, z3.h[7]
  CMP_IO(dci(0x64bb04bb), R(v27), R(v27, v5, v3));  // fmls z27.s, z5.s, z3.s[3]
  CMP_IO(dci(0x2fbcc837),
         R(v23),
         R(v23, v1, v12));  // fmlsl2 v23.2s, v1.2h, v12.h[7]
  CMP_IO(dci(0x2ebccf58),
         R(v24),
         R(v24, v26, v28));  // fmlsl2 v24.2s, v26.2h, v28.2h
  CMP_IO(dci(0x4fbe4b15),
         R(v21),
         R(v21, v24, v14));  // fmlsl v21.4s, v24.4h, v14.h[7]
  CMP_IO(dci(0x4ebaeed6), R(v22), R(v22, v26));  // fmlsl v22.4s, v22.4h, v26.4h
  CMP_IO(dci(0x64bda14a), R(v10), R(v10, v29));  // fmlslb z10.s, z10.h, z29.h
  CMP_IO(dci(0x64bb6a45),
         R(v5),
         R(v5, v18, v3));  // fmlslb z5.s, z18.h, z3.h[7]

  CLEANUP();
}

TEST(group_22) {
  SETUP();

  CMP_IO(dci(0x64bea650),
         R(v16),
         R(v16, v18, v30));                   // fmlslt z16.s, z18.h, z30.h
  CMP_IO(dci(0x64bb6c03), R(v3), R(v0, v3));  // fmlslt z3.s, z0.h, z3.h[7]
  CMP_IO(dci(0x64fbe724), R(v4), R(v4, v25, v27));   // fmmla z4.d, z25.d, z27.d
  CMP_IO(dci(0x64bce41d), R(v29), R(v29, v0, v28));  // fmmla z29.s, z0.s, z28.s
  CMP_IO(dci(0x1ee6019a), R(x26), R(v12));           // fmov w26, h12
  CMP_IO(dci(0x1e2600bb), R(x27), R(v5));            // fmov w27, s5
  CMP_IO(dci(0x9e6600ef), R(x15), R(v7));            // fmov x15, d7
  CMP_IO(dci(0x9ee60003), R(x3), R(v0));             // fmov x3, h0
  CMP_IO(dci(0x9eae01f5), R(x21), R(v15));           // fmov x21, v15.D[1]
  CMP_IO(dci(0x6f02f638), R(v24), R());          // fmov v24.2d, #0x51 (0.2656)
  CMP_IO(dci(0x4f03fff6), R(v22), R());          // fmov v22.8h, #0x7f (1.9375)
  CMP_IO(dci(0x4f03f7f6), R(v22), R());          // fmov v22.4s, #0x7f (1.9375)
  CMP_IO(dci(0x9e6700ef), R(v15), R(x7));        // fmov d15, x7
  CMP_IO(dci(0x1e604399), R(v25), R(v28));       // fmov d25, d28
  CMP_IO(dci(0x1e7a7015), R(v21), R());          // fmov d21, #0xd3 (-0.2969)
  CMP_IO(dci(0x1ee7019a), R(v26), R(x12));       // fmov h26, w12
  CMP_IO(dci(0x9ee70003), R(v3), R(x0));         // fmov h3, x0
  CMP_IO(dci(0x1ee041f5), R(v21), R(v15));       // fmov h21, h15
  CMP_IO(dci(0x1efa9019), R(v25), R());          // fmov h25, #0xd4 (-0.3125)
  CMP_IO(dci(0x1e2700bb), R(v27), R(x5));        // fmov s27, w5
  CMP_IO(dci(0x1e2041e8), R(v8), R(v15));        // fmov s8, s15
  CMP_IO(dci(0x1e39b01c), R(v28), R());          // fmov s28, #0xcd (-0.2266)
  CMP_IO(dci(0x9eaf01ce), R(v14), R(v14, x14));  // fmov v14.D[1], x14
  CMP_IO(dci(0x65f3bf3b),
         R(v27),
         R(v27, p7, v25, v19));  // fmsb z27.d, p7/m, z25.d, z19.d
  CMP_IO(dci(0x1f5ba980), R(v0), R(v12, v27, v10));  // fmsub d0, d12, d27, d10
  CMP_IO(dci(0x1fdbd324), R(v4), R(v25, v27, v20));  // fmsub h4, h25, h27, h20
  CMP_IO(dci(0x1f1b8731), R(v17), R(v25, v27, v1));  // fmsub s17, s25, s27, s1
  CMP_IO(dci(0x4ffa90a1), R(v1), R(v5, v10));    // fmul v1.2d, v5.2d, v10.d[0]
  CMP_IO(dci(0x0f1b9010), R(v16), R(v0, v11));   // fmul v16.4h, v0.4h, v11.h[1]
  CMP_IO(dci(0x2e2ddeee), R(v14), R(v23, v13));  // fmul v14.2s, v23.2s, v13.2s
  CMP_IO(dci(0x2e591d25), R(v5), R(v9, v25));    // fmul v5.4h, v9.4h, v25.4h
  CMP_IO(dci(0x5fbc98ef), R(v15), R(v7, v12));   // fmul s15, s7, v12.s[3]

  CLEANUP();
}

TEST(group_23) {
  SETUP();

  CMP_IO(dci(0x5f1b9a52), R(v18), R(v18, v11));  // fmul h18, h18, v11.h[5]
  CMP_IO(dci(0x1e7d0ad4), R(v20), R(v22, v29));  // fmul d20, d22, d29
  CMP_IO(dci(0x1efd0930), R(v16), R(v9, v29));   // fmul h16, h9, h29
  CMP_IO(dci(0x1e3c0aad), R(v13), R(v21, v28));  // fmul s13, s21, s28
  CMP_IO(dci(0x65da983e), R(v30), R(p6, v30));  // fmul z30.d, p6/m, z30.d, #2.0
  CMP_IO(dci(0x6542841d),
         R(v29),
         R(p1, v29, v0));                       // fmul z29.h, p1/m, z29.h, z0.h
  CMP_IO(dci(0x65580984), R(v4), R(v12, v24));  // fmul z4.h, z12.h, z24.h
  CMP_IO(dci(0x64fc202a), R(v10), R(v1, v12));  // fmul z10.d, z1.d, z12.d[1]
  CMP_IO(dci(0x647c2116), R(v22), R(v8, v4));   // fmul z22.h, z8.h, z4.h[7]
  CMP_IO(dci(0x64bd22c7), R(v7), R(v22, v5));   // fmul z7.s, z22.s, z5.s[3]
  CMP_IO(dci(0x6ffd9399),
         R(v25),
         R(v28, v13));  // fmulx v25.2d, v28.2d, v13.d[0]
  CMP_IO(dci(0x6f1c99e8), R(v8), R(v15, v12));  // fmulx v8.8h, v15.8h, v12.h[5]
  CMP_IO(dci(0x0e3bdd5f), R(v31), R(v10, v27));  // fmulx v31.2s, v10.2s, v27.2s
  CMP_IO(dci(0x0e5e1e5d), R(v29), R(v18, v30));  // fmulx v29.4h, v18.4h, v30.4h
  CMP_IO(dci(0x7fbc9837), R(v23), R(v1, v12));   // fmulx s23, s1, v12.s[3]
  CMP_IO(dci(0x7f1c9aba), R(v26), R(v21, v12));  // fmulx h26, h21, v12.h[5]
  CMP_IO(dci(0x5e3edcac), R(v12), R(v5, v30));   // fmulx s12, s5, s30
  CMP_IO(dci(0x5e5d1fb3), R(v19), R(v29, v29));  // fmulx h19, h29, h29
  CMP_IO(dci(0x654a9765),
         R(v5),
         R(p5, v5, v27));                   // fmulx z5.h, p5/m, z5.h, z27.h
  CMP_IO(dci(0x6ee0faee), R(v14), R(v23));  // fneg v14.2d, v23.2d
  CMP_IO(dci(0x6ef8fb15), R(v21), R(v24));  // fneg v21.8h, v24.8h
  CMP_IO(dci(0x1e6141c1), R(v1), R(v14));   // fneg d1, d14
  CMP_IO(dci(0x1ee142ad), R(v13), R(v21));  // fneg h13, h21
  CMP_IO(dci(0x1e214130), R(v16), R(v9));   // fneg s16, s9
  CMP_IO(dci(0x049daa8b), R(v11), R(v11, p2, v20));  // fneg z11.s, p2/m, z20.s
  CMP_IO(dci(0x657bd0d5),
         R(v21),
         R(v21, p4, v6, v27));  // fnmad z21.h, p4/m, z6.h, z27.h
  CMP_IO(dci(0x1f7c2aad),
         R(v13),
         R(v21, v28, v10));  // fnmadd d13, d21, d28, d10
  CMP_IO(dci(0x1ffc2dc1), R(v1), R(v14, v28, v11));  // fnmadd h1, h14, h28, h11
  CMP_IO(dci(0x1f3d46d4),
         R(v20),
         R(v22, v29, v17));  // fnmadd s20, s22, s29, s17
  CMP_IO(dci(0x657c441d),
         R(v29),
         R(v29, p1, v0, v28));  // fnmla z29.h, p1/m, z0.h, z28.h
  CMP_IO(dci(0x65bb689b),
         R(v27),
         R(v27, p2, v4, v27));  // fnmls z27.s, p2/m, z4.s, z27.s
  CMP_IO(dci(0x65fae0a1),
         R(v1),
         R(v1, p0, v5, v26));  // fnmsb z1.d, p0/m, z5.d, z26.d

  CLEANUP();
}

TEST(group_24) {
  SETUP();

  CMP_IO(dci(0x1f7c8b72), R(v18), R(v27, v28, v2));  // fnmsub d18, d27, d28, d2
  CMP_IO(dci(0x1ffce1ce),
         R(v14),
         R(v14, v28, v24));  // fnmsub h14, h14, h28, h24
  CMP_IO(dci(0x1f3de60f),
         R(v15),
         R(v16, v29, v25));                      // fnmsub s15, s16, s29, s25
  CMP_IO(dci(0x1e7b8b3e), R(v30), R(v25, v27));  // fnmul d30, d25, d27
  CMP_IO(dci(0x1efb88e2), R(v2), R(v7, v27));    // fnmul h2, h7, h27
  CMP_IO(dci(0x1e3a8973), R(v19), R(v11, v26));  // fnmul s19, s11, s26
  CMP_IO(dci(0x4ea1d99d), R(v29), R(v12));       // frecpe v29.4s, v12.4s
  CMP_IO(dci(0x0ef9db22), R(v2), R(v25));        // frecpe v2.4h, v25.4h
  CMP_IO(dci(0x5ea1dbe7), R(v7), R(v31));        // frecpe s7, s31
  CMP_IO(dci(0x5ef9dbc0), R(v0), R(v30));        // frecpe h0, h30
  CMP_IO(dci(0x654e325d), R(v29), R(v18));       // frecpe z29.h, z18.h
  CMP_IO(dci(0x4e3afc87), R(v7), R(v4, v26));    // frecps v7.4s, v4.4s, v26.4s
  CMP_IO(dci(0x0e593c60), R(v0), R(v3, v25));    // frecps v0.4h, v3.4h, v25.4h
  CMP_IO(dci(0x5e79fec9), R(v9), R(v22, v25));   // frecps d9, d22, d25
  CMP_IO(dci(0x5e5a3fc2), R(v2), R(v30, v26));   // frecps h2, h30, h26
  CMP_IO(dci(0x65da1a11), R(v17), R(v16, v26));  // frecps z17.d, z16.d, z26.d
  CMP_IO(dci(0x5ea1f964), R(v4), R(v11));        // frecpx s4, s11
  CMP_IO(dci(0x5ef9f98b), R(v11), R(v12));       // frecpx h11, h12
  CMP_IO(dci(0x65ccbf3b),
         R(v27),
         R(v27, p7, v25));                  // frecpx z27.d, p7/m, z25.d
  CMP_IO(dci(0x6e61ebda), R(v26), R(v30));  // frint32x v26.2d, v30.2d
  CMP_IO(dci(0x1e68c1f5), R(v21), R(v15));  // frint32x d21, d15
  CMP_IO(dci(0x1e28c0fc), R(v28), R(v7));   // frint32x s28, s7
  CMP_IO(dci(0x4e21e8ac), R(v12), R(v5));   // frint32z v12.4s, v5.4s
  CMP_IO(dci(0x1e6842c7), R(v7), R(v22));   // frint32z d7, d22
  CMP_IO(dci(0x1e28402a), R(v10), R(v1));   // frint32z s10, s1
  CMP_IO(dci(0x2e21fa43), R(v3), R(v18));   // frint64x v3.2s, v18.2s
  CMP_IO(dci(0x1e69c130), R(v16), R(v9));   // frint64x d16, d9
  CMP_IO(dci(0x1e29c1c1), R(v1), R(v14));   // frint64x s1, s14
  CMP_IO(dci(0x4e61fb45), R(v5), R(v26));   // frint64z v5.2d, v26.2d
  CMP_IO(dci(0x1e69405e), R(v30), R(v2));   // frint64z d30, d2
  CMP_IO(dci(0x1e294293), R(v19), R(v20));  // frint64z s19, s20
  CMP_IO(dci(0x2e218a52), R(v18), R(v18));  // frinta v18.2s, v18.2s

  CLEANUP();
}

TEST(group_25) {
  SETUP();

  CMP_IO(dci(0x6e7988b9), R(v25), R(v5));           // frinta v25.8h, v5.8h
  CMP_IO(dci(0x1e6642ad), R(v13), R(v21));          // frinta d13, d21
  CMP_IO(dci(0x1ee641c1), R(v1), R(v14));           // frinta h1, h14
  CMP_IO(dci(0x1e2642d4), R(v20), R(v22));          // frinta s20, s22
  CMP_IO(dci(0x6584a923), R(v3), R(v3, p2, v9));    // frinta z3.s, p2/m, z9.s
  CMP_IO(dci(0x2ea19893), R(v19), R(v4));           // frinti v19.2s, v4.2s
  CMP_IO(dci(0x2ef99971), R(v17), R(v11));          // frinti v17.4h, v11.4h
  CMP_IO(dci(0x1e67c365), R(v5), R(v27));           // frinti d5, d27
  CMP_IO(dci(0x1ee7c109), R(v9), R(v8));            // frinti h9, h8
  CMP_IO(dci(0x1e27c38c), R(v12), R(v28));          // frinti s12, s28
  CMP_IO(dci(0x6587bc6b), R(v11), R(v11, p7, v3));  // frinti z11.s, p7/m, z3.s
  CMP_IO(dci(0x4e619aee), R(v14), R(v23));          // frintm v14.2d, v23.2d
  CMP_IO(dci(0x4e799b15), R(v21), R(v24));          // frintm v21.8h, v24.8h
  CMP_IO(dci(0x1e6541c1), R(v1), R(v14));           // frintm d1, d14
  CMP_IO(dci(0x1ee542ad), R(v13), R(v21));          // frintm h13, h21
  CMP_IO(dci(0x1e254130), R(v16), R(v9));           // frintm s16, s9
  CMP_IO(dci(0x6582aa8b),
         R(v11),
         R(v11, p2, v20));                         // frintm z11.s, p2/m, z20.s
  CMP_IO(dci(0x0e218957), R(v23), R(v10));         // frintn v23.2s, v10.2s
  CMP_IO(dci(0x0e798a50), R(v16), R(v18));         // frintn v16.4h, v18.4h
  CMP_IO(dci(0x1e6440fc), R(v28), R(v7));          // frintn d28, d7
  CMP_IO(dci(0x1ee441e8), R(v8), R(v15));          // frintn h8, h15
  CMP_IO(dci(0x1e2441f5), R(v21), R(v15));         // frintn s21, s15
  CMP_IO(dci(0x65c0bba6), R(v6), R(v6, p6, v29));  // frintn z6.d, p6/m, z29.d
  CMP_IO(dci(0x4ee188b9), R(v25), R(v5));          // frintp v25.2d, v5.2d
  CMP_IO(dci(0x0ef98892), R(v18), R(v4));          // frintp v18.4h, v4.4h
  CMP_IO(dci(0x1e64c3a6), R(v6), R(v29));          // frintp d6, d29
  CMP_IO(dci(0x1ee4c2ba), R(v26), R(v21));         // frintp h26, h21
  CMP_IO(dci(0x1e24c1db), R(v27), R(v14));         // frintp s27, s14
  CMP_IO(dci(0x6541ab74),
         R(v20),
         R(v20, p2, v27));                  // frintp z20.h, p2/m, z27.h
  CMP_IO(dci(0x6e619b45), R(v5), R(v26));   // frintx v5.2d, v26.2d
  CMP_IO(dci(0x2e79994a), R(v10), R(v10));  // frintx v10.4h, v10.4h
  CMP_IO(dci(0x1e67405e), R(v30), R(v2));   // frintx d30, d2

  CLEANUP();
}

TEST(group_26) {
  SETUP();

  CMP_IO(dci(0x1ee74202), R(v2), R(v16));           // frintx h2, h16
  CMP_IO(dci(0x1e274293), R(v19), R(v20));          // frintx s19, s20
  CMP_IO(dci(0x65c6b11c), R(v28), R(v28, p4, v8));  // frintx z28.d, p4/m, z8.d
  CMP_IO(dci(0x0ea19a43), R(v3), R(v18));           // frintz v3.2s, v18.2s
  CMP_IO(dci(0x0ef99a1c), R(v28), R(v16));          // frintz v28.4h, v16.4h
  CMP_IO(dci(0x1e65c130), R(v16), R(v9));           // frintz d16, d9
  CMP_IO(dci(0x1ee5c2d4), R(v20), R(v22));          // frintz h20, h22
  CMP_IO(dci(0x1e25c1c1), R(v1), R(v14));           // frintz s1, s14
  CMP_IO(dci(0x6543ab72),
         R(v18),
         R(v18, p2, v27));                     // frintz z18.h, p2/m, z27.h
  CMP_IO(dci(0x2ea1da2b), R(v11), R(v17));     // frsqrte v11.2s, v17.2s
  CMP_IO(dci(0x2ef9da04), R(v4), R(v16));      // frsqrte v4.4h, v16.4h
  CMP_IO(dci(0x7ee1d925), R(v5), R(v9));       // frsqrte d5, d9
  CMP_IO(dci(0x7ef9da1e), R(v30), R(v16));     // frsqrte h30, h16
  CMP_IO(dci(0x654f31fb), R(v27), R(v15));     // frsqrte z27.h, z15.h
  CMP_IO(dci(0x4ee0fec9), R(v9), R(v22, v0));  // frsqrts v9.2d, v22.2d, v0.2d
  CMP_IO(dci(0x0ede3d7e),
         R(v30),
         R(v11, v30));  // frsqrts v30.4h, v11.4h, v30.4h
  CMP_IO(dci(0x5ebefd8b), R(v11), R(v12, v30));  // frsqrts s11, s12, s30
  CMP_IO(dci(0x5edd3d64), R(v4), R(v11, v29));   // frsqrts h4, h11, h29
  CMP_IO(dci(0x659d1fb3), R(v19), R(v29, v29));  // frsqrts z19.s, z29.s, z29.s
  CMP_IO(dci(0x65c98731),
         R(v17),
         R(v17, p1, v17, v25));             // fscale z17.d, p1/m, z17.d, z25.d
  CMP_IO(dci(0x6ee1fad6), R(v22), R(v22));  // fsqrt v22.2d, v22.2d
  CMP_IO(dci(0x6ef9fafd), R(v29), R(v23));  // fsqrt v29.8h, v23.8h
  CMP_IO(dci(0x1e61c3e9), R(v9), R(v31));   // fsqrt d9, d31
  CMP_IO(dci(0x1ee1c245), R(v5), R(v18));   // fsqrt h5, h18
  CMP_IO(dci(0x1e21c358), R(v24), R(v26));  // fsqrt s24, s26
  CMP_IO(dci(0x65cdae43), R(v3), R(v3, p3, v18));  // fsqrt z3.d, p3/m, z18.d
  CMP_IO(dci(0x4ef9d78e), R(v14), R(v28, v25));  // fsub v14.2d, v28.2d, v25.2d
  CMP_IO(dci(0x4eda17b5), R(v21), R(v29, v26));  // fsub v21.8h, v29.8h, v26.8h
  CMP_IO(dci(0x1e7c3844), R(v4), R(v2, v28));    // fsub d4, d2, d28
  CMP_IO(dci(0x1efc3aa0), R(v0), R(v21, v28));   // fsub h0, h21, h28
  CMP_IO(dci(0x1e3d393d), R(v29), R(v9, v29));   // fsub s29, s9, s29
  CMP_IO(dci(0x65d9942e), R(v14), R(p5, v14));  // fsub z14.d, p5/m, z14.d, #1.0

  CLEANUP();
}

TEST(group_27) {
  SETUP();

  CMP_IO(dci(0x65419d8d),
         R(v13),
         R(p7, v13, v12));  // fsub z13.h, p7/m, z13.h, z12.h
  CMP_IO(dci(0x65cc0634), R(v20), R(v17, v12));  // fsub z20.d, z17.d, z12.d
  CMP_IO(dci(0x659b9c30),
         R(v16),
         R(p7, v16));  // fsubr z16.s, p7/m, z16.s, #1.0
  CMP_IO(dci(0x65838a8b),
         R(v11),
         R(p2, v11, v20));  // fsubr z11.s, p2/m, z11.s, z20.s
  CMP_IO(dci(0x659282e3), R(v3), R(v3, v23));    // ftmad z3.s, z3.s, z23.s, #2
  CMP_IO(dci(0x655a0fb5), R(v21), R(v29, v26));  // ftsmul z21.h, z29.h, z26.h
  CMP_IO(dci(0x04bab22b), R(v11), R(v17, v26));  // ftssel z11.s, z17.s, z26.s
  CMP_IO(dci(0x9add1557), R(x23), R(x10, x29));  // gmi x23, x10, x29
  CMP_IO(dci(0xd5032fdf), R(), R());             // hint #126
  CMP_IO(dci(0x45e0c383),
         R(v3),
         R(p0, v28, v0));  // histcnt z3.d, p0/z, z28.d, z0.d
  CMP_IO(dci(0x453ea3e7), R(v7), R(v31, v30));   // histseg z7.b, z31.b, z30.b
  CMP_IO(dci(0x043ae13f), R(), R());             // incb xzr, vl16, mul #11
  CMP_IO(dci(0x04f9e2c9), R(x9), R(x9));         // incd x9, #0x16, mul #10
  CMP_IO(dci(0x04f9c2c9), R(v9), R(v9));         // incd z9.d, #0x16, mul #10
  CMP_IO(dci(0x0479e125), R(x5), R(x5));         // inch x5, vl16, mul #10
  CMP_IO(dci(0x0479c125), R(v5), R(v5));         // inch z5.h, vl16, mul #10
  CMP_IO(dci(0x25ec89c1), R(x1), R(x1, p14));    // incp x1, p14.d
  CMP_IO(dci(0x25ec81c1), R(v1), R(v1, p14));    // incp z1.d, p14
  CMP_IO(dci(0x04bae094), R(x20), R(x20));       // incw x20, vl4, mul #11
  CMP_IO(dci(0x04bac094), R(v20), R(v20));       // incw z20.s, vl4, mul #11
  CMP_IO(dci(0x04be4198), R(v24), R());          // index z24.s, #12, #-2
  CMP_IO(dci(0x047d4885), R(v5), R(x29));        // index z5.h, #4, w29
  CMP_IO(dci(0x047d4485), R(v5), R(x4));         // index z5.h, w4, #-3
  CMP_IO(dci(0x04be4d98), R(v24), R(x12, x30));  // index z24.s, w12, w30
  CMP_IO(dci(0x4e1d1f58), R(v24), R(v24, x26));  // mov v24.b[14], w26
  CMP_IO(dci(0x6e1e2758), R(v24), R(v24, v26));  // mov v24.h[7], v26.h[2]
  CMP_IO(dci(0x05e4397e), R(v30), R(v30, x11));  // insr z30.d, x11
  CMP_IO(dci(0x05743bda), R(v26), R(v26, v30));  // insr z26.h, h30
  CMP_IO(dci(0x9ade1250), R(x16), R(x18, x30));  // irg x16, x18, x30
  CMP_IO(dci(0xd50331df), R(), R());             // isb
  CMP_IO(dci(0x05a0b010), R(x16), R(p4, v0));    // lasta w16, p4, z0.s
  CMP_IO(dci(0x0522866c), R(v12), R(p1, v19));   // lasta b12, p1, z19.b

  CLEANUP();
}

TEST(group_28) {
  SETUP();

  CMP_IO(dci(0x0561b0d5), R(x21), R(p4, v6));   // lastb w21, p4, z6.h
  CMP_IO(dci(0x05e38731), R(v17), R(p1, v25));  // lastb d17, p1, z25.d
  CMP_IO(dci(0x4c407fa8), R(v8), R(x29));       // ld1 {v8.2d}, [x29]
  CMP_IO(dci(0x4c40afa8), R(v8, v9), R(x29));   // ld1 {v8.2d, v9.2d}, [x29]
  CMP_IO(dci(0x4c406fa8),
         R(v8, v9, v10),
         R(x29));  // ld1 {v8.2d, v9.2d, v10.2d}, [x29]
  CMP_IO(dci(0x4c402fa8),
         R(v8, v9, v10, v11),
         R(x29));  // ld1 {v8.2d, v9.2d, v10.2d, v11.2d}, [x29]
  CMP_IO(dci(0x4cdf73f4), R(v20, sp), R(sp));  // ld1 {v20.16b}, [sp], #16
  CMP_IO(dci(0x4cdfa3f4),
         R(v20, v21, sp),
         R(sp));  // ld1 {v20.16b, v21.16b}, [sp], #32
  CMP_IO(dci(0x4cdf63f4),
         R(v20, v21, v22, sp),
         R(sp));  // ld1 {v20.16b, v21.16b, v22.16b}, [sp], #48
  CMP_IO(dci(0x4cdf23f4),
         R(v20, v21, v22, v23, sp),
         R(sp));  // ld1 {v20.16b, v21.16b, v22.16b, v23.16b}, [sp], #64
  CMP_IO(dci(0x4cc373f4), R(v20, sp), R(sp, x3));  // ld1 {v20.16b}, [sp], x3
  CMP_IO(dci(0x4cd3a3f4),
         R(v20, v21, sp),
         R(sp, x19));  // ld1 {v20.16b, v21.16b}, [sp], x19
  CMP_IO(dci(0x4cca63f4),
         R(v20, v21, v22, sp),
         R(sp, x10));  // ld1 {v20.16b, v21.16b, v22.16b}, [sp], x10
  CMP_IO(dci(0x4cc223f4),
         R(v20, v21, v22, v23, sp),
         R(sp, x2));  // ld1 {v20.16b, v21.16b, v22.16b, v23.16b}, [sp], x2
  CMP_IO(dci(0x4d401dea), R(v10), R(v10, x15));  // ld1 {v10.b}[15], [x15]
  CMP_IO(dci(0x4d4085ea), R(v10), R(v10, x15));  // ld1 {v10.d}[1], [x15]
  CMP_IO(dci(0x4d4059ea), R(v10), R(v10, x15));  // ld1 {v10.h}[7], [x15]
  CMP_IO(dci(0x4d4091ea), R(v10), R(v10, x15));  // ld1 {v10.s}[3], [x15]
  CMP_IO(dci(0x0ddf0d59),
         R(v25, x10),
         R(v25, x10));  // ld1 {v25.b}[3], [x10], #1
  CMP_IO(dci(0x4ddc13f4),
         R(v20, sp),
         R(v20, sp, x28));  // ld1 {v20.b}[12], [sp], x28
  CMP_IO(dci(0x0ddf8559),
         R(v25, x10),
         R(v25, x10));  // ld1 {v25.d}[0], [x10], #8
  CMP_IO(dci(0x4dd387f4),
         R(v20, sp),
         R(v20, sp, x19));  // ld1 {v20.d}[1], [sp], x19
  CMP_IO(dci(0x0ddf4959),
         R(v25, x10),
         R(v25, x10));  // ld1 {v25.h}[1], [x10], #2
  CMP_IO(dci(0x4dd053f4),
         R(v20, sp),
         R(v20, sp, x16));  // ld1 {v20.h}[6], [sp], x16
  CMP_IO(dci(0x0ddf8159),
         R(v25, x10),
         R(v25, x10));  // ld1 {v25.s}[0], [x10], #4
  CMP_IO(dci(0x4dc493f4),
         R(v20, sp),
         R(v20, sp, x4));  // ld1 {v20.s}[3], [sp], x4
  CMP_IO(dci(0xc43ccc37),
         R(v23),
         R(p3, v1));  // ld1b {z23.d}, p3/z, [z1.d, #28]
  CMP_IO(dci(0x843cdeba),
         R(v26),
         R(p7, v21));  // ld1b {z26.s}, p7/z, [z21.s, #28]
  CMP_IO(dci(0xa42cb38c),
         R(v12),
         R(p4, x28));  // ld1b {z12.h}, p4/z, [x28, #-4, mul vl]
  CMP_IO(dci(0xa44da85e),
         R(v30),
         R(p2, x2));  // ld1b {z30.s}, p2/z, [x2, #-3, mul vl]
  CMP_IO(dci(0xa46db2c7),
         R(v7),
         R(p4, x22));  // ld1b {z7.d}, p4/z, [x22, #-3, mul vl]
  CMP_IO(dci(0xa40db7cd),
         R(v13),
         R(p5, x30));  // ld1b {z13.b}, p5/z, [x30, #-3, mul vl]

  CLEANUP();
}

TEST(group_29) {
  SETUP();

  CMP_IO(dci(0xa43c4109),
         R(v9),
         R(p0, x8, x28));  // ld1b {z9.h}, p0/z, [x8, x28]
  CMP_IO(dci(0xa45c5f7f),
         R(v31),
         R(p7, x27, x28));  // ld1b {z31.s}, p7/z, [x27, x28]
  CMP_IO(dci(0xa47c5516),
         R(v22),
         R(p5, x8, x28));  // ld1b {z22.d}, p5/z, [x8, x28]
  CMP_IO(dci(0xa41e4a50),
         R(v16),
         R(p2, x18, x30));  // ld1b {z16.b}, p2/z, [x18, x30]
  CMP_IO(dci(0xc45cd38c),
         R(v12),
         R(p4, x28, v28));  // ld1b {z12.d}, p4/z, [x28, z28.d]
  CMP_IO(dci(0xc45b53e9),
         R(v9),
         R(p4, sp, v27));  // ld1b {z9.d}, p4/z, [sp, z27.d, sxtw]
  CMP_IO(dci(0x841c4358),
         R(v24),
         R(p0, x26, v28));  // ld1b {z24.s}, p0/z, [x26, z28.s, uxtw]
  CMP_IO(dci(0xc5bccdc1),
         R(v1),
         R(p3, v14));  // ld1d {z1.d}, p3/z, [z14.d, #224]
  CMP_IO(dci(0xa5ecb451),
         R(v17),
         R(p5, x2));  // ld1d {z17.d}, p5/z, [x2, #-4, mul vl]
  CMP_IO(dci(0xa5fc4044),
         R(v4),
         R(p0, x2, x28));  // ld1d {z4.d}, p0/z, [x2, x28, lsl #3]
  CMP_IO(dci(0xc5fcdf7f),
         R(v31),
         R(p7, x27, v28));  // ld1d {z31.d}, p7/z, [x27, z28.d, lsl #3]
  CMP_IO(dci(0xc5ddc85e),
         R(v30),
         R(p2, x2, v29));  // ld1d {z30.d}, p2/z, [x2, z29.d]
  CMP_IO(dci(0xc5fb5b3e),
         R(v30),
         R(p6, x25, v27));  // ld1d {z30.d}, p6/z, [x25, z27.d, sxtw #3]
  CMP_IO(dci(0xc59b525f),
         R(v31),
         R(p4, x18, v27));  // ld1d {z31.d}, p4/z, [x18, z27.d, uxtw]
  CMP_IO(dci(0xc4bccaad),
         R(v13),
         R(p2, v21));  // ld1h {z13.d}, p2/z, [z21.d, #56]
  CMP_IO(dci(0x84bdc6d4),
         R(v20),
         R(p1, v22));  // ld1h {z20.s}, p1/z, [z22.s, #58]
  CMP_IO(dci(0xa4adb202),
         R(v2),
         R(p4, x16));  // ld1h {z2.h}, p4/z, [x16, #-3, mul vl]
  CMP_IO(dci(0xa4cdb078),
         R(v24),
         R(p4, x3));  // ld1h {z24.s}, p4/z, [x3, #-3, mul vl]
  CMP_IO(dci(0xa4edb13d),
         R(v29),
         R(p4, x9));  // ld1h {z29.d}, p4/z, [x9, #-3, mul vl]
  CMP_IO(dci(0xa4bc4293),
         R(v19),
         R(p0, x20, x28));  // ld1h {z19.h}, p0/z, [x20, x28, lsl #1]
  CMP_IO(dci(0xa4dc5765),
         R(v5),
         R(p5, x27, x28));  // ld1h {z5.s}, p5/z, [x27, x28, lsl #1]
  CMP_IO(dci(0xa4fc56a0),
         R(v0),
         R(p5, x21, x28));  // ld1h {z0.d}, p5/z, [x21, x28, lsl #1]
  CMP_IO(dci(0xc4fcc293),
         R(v19),
         R(p0, x20, v28));  // ld1h {z19.d}, p0/z, [x20, z28.d, lsl #1]
  CMP_IO(dci(0xc4ddd202),
         R(v2),
         R(p4, x16, v29));  // ld1h {z2.d}, p4/z, [x16, z29.d]
  CMP_IO(dci(0xc4fb44e2),
         R(v2),
         R(p1, x7, v27));  // ld1h {z2.d}, p1/z, [x7, z27.d, sxtw #1]
  CMP_IO(dci(0xc49a5573),
         R(v19),
         R(p5, x11, v26));  // ld1h {z19.d}, p5/z, [x11, z26.d, uxtw]
  CMP_IO(dci(0x84bb525f),
         R(v31),
         R(p4, x18, v27));  // ld1h {z31.s}, p4/z, [x18, z27.s, uxtw #1]
  CMP_IO(dci(0x84db5b3e),
         R(v30),
         R(p6, x25, v27));  // ld1h {z30.s}, p6/z, [x25, z27.s, sxtw]
  CMP_IO(dci(0x4d40c708), R(v8), R(x24));        // ld1r {v8.8h}, [x24]
  CMP_IO(dci(0x0ddfce52), R(v18, x18), R(x18));  // ld1r {v18.1d}, [x18], #8
  CMP_IO(dci(0x4dd4c5db),
         R(v27, x14),
         R(x14, x20));  // ld1r {v27.8h}, [x14], x20
  CMP_IO(dci(0x845aab0a),
         R(v10),
         R(p2, x24));  // ld1rb {z10.h}, p2/z, [x24, #26]

  CLEANUP();
}

TEST(group_30) {
  SETUP();

  CMP_IO(dci(0x845bc980), R(v0), R(p2, x12));  // ld1rb {z0.s}, p2/z, [x12, #27]
  CMP_IO(dci(0x847bea45), R(v5), R(p2, x18));  // ld1rb {z5.d}, p2/z, [x18, #59]
  CMP_IO(dci(0x847a8bcf),
         R(v15),
         R(p2, x30));  // ld1rb {z15.b}, p2/z, [x30, #58]
  CMP_IO(dci(0x85faf573),
         R(v19),
         R(p5, x11));  // ld1rd {z19.d}, p5/z, [x11, #464]
  CMP_IO(dci(0x84dbb324), R(v4), R(p4, x25));  // ld1rh {z4.h}, p4/z, [x25, #54]
  CMP_IO(dci(0x84dbc7f6), R(v22), R(p1, sp));  // ld1rh {z22.s}, p1/z, [sp, #54]
  CMP_IO(dci(0x84fbf25f),
         R(v31),
         R(p4, x18));  // ld1rh {z31.d}, p4/z, [x18, #118]
  CMP_IO(dci(0xa42e2bda),
         R(v26),
         R(p2, x30));  // ld1rob {z26.b}, p2/z, [x30, #-64]
  CMP_IO(dci(0xa43a1fdc),
         R(v28),
         R(p7, x30, x26));  // ld1rob {z28.b}, p7/z, [x30, x26]
  CMP_IO(dci(0xa5ac3516),
         R(v22),
         R(p5, x8));  // ld1rod {z22.d}, p5/z, [x8, #-128]
  CMP_IO(dci(0xa5bb14ae),
         R(v14),
         R(p5, x5, x27));  // ld1rod {z14.d}, p5/z, [x5, x27, lsl #3]
  CMP_IO(dci(0xa4ac3765),
         R(v5),
         R(p5, x27));  // ld1roh {z5.h}, p5/z, [x27, #-128]
  CMP_IO(dci(0xa4bb1e52),
         R(v18),
         R(p7, x18, x27));  // ld1roh {z18.h}, p7/z, [x18, x27, lsl #1]
  CMP_IO(dci(0xa52d3ba6),
         R(v6),
         R(p6, x29));  // ld1row {z6.s}, p6/z, [x29, #-96]
  CMP_IO(dci(0xa53b05a7),
         R(v7),
         R(p1, x13, x27));  // ld1row {z7.s}, p1/z, [x13, x27, lsl #2]
  CMP_IO(dci(0xa40e3708),
         R(v8),
         R(p5, x24));  // ld1rqb {z8.b}, p5/z, [x24, #-32]
  CMP_IO(dci(0xa41c0e86),
         R(v6),
         R(p3, x20, x28));  // ld1rqb {z6.b}, p3/z, [x20, x28]
  CMP_IO(dci(0xa58c338c),
         R(v12),
         R(p4, x28));  // ld1rqd {z12.d}, p4/z, [x28, #-64]
  CMP_IO(dci(0xa59b19b4),
         R(v20),
         R(p6, x13, x27));  // ld1rqd {z20.d}, p6/z, [x13, x27, lsl #3]
  CMP_IO(dci(0xa48d3c6b),
         R(v11),
         R(p7, x3));  // ld1rqh {z11.h}, p7/z, [x3, #-48]
  CMP_IO(dci(0xa49b1010),
         R(v16),
         R(p4, x0, x27));  // ld1rqh {z16.h}, p4/z, [x0, x27, lsl #1]
  CMP_IO(dci(0xa50c2cfc),
         R(v28),
         R(p3, x7));  // ld1rqw {z28.s}, p3/z, [x7, #-64]
  CMP_IO(dci(0xa51b13e9),
         R(v9),
         R(p4, sp, x27));  // ld1rqw {z9.s}, p4/z, [sp, x27, lsl #2]
  CMP_IO(dci(0x85fcddf5),
         R(v21),
         R(p7, x15));  // ld1rsb {z21.h}, p7/z, [x15, #60]
  CMP_IO(dci(0x85fca923), R(v3), R(p2, x9));  // ld1rsb {z3.s}, p2/z, [x9, #60]
  CMP_IO(dci(0x85dc9eba),
         R(v26),
         R(p7, x21));  // ld1rsb {z26.d}, p7/z, [x21, #28]
  CMP_IO(dci(0x857da799),
         R(v25),
         R(p1, x28));  // ld1rsh {z25.s}, p1/z, [x28, #122]
  CMP_IO(dci(0x855d86d4),
         R(v20),
         R(p1, x22));  // ld1rsh {z20.d}, p1/z, [x22, #58]
  CMP_IO(dci(0x84fc9765),
         R(v5),
         R(p5, x27));  // ld1rsw {z5.d}, p5/z, [x27, #240]
  CMP_IO(dci(0x857bdc03), R(v3), R(p7, x0));  // ld1rw {z3.s}, p7/z, [x0, #236]
  CMP_IO(dci(0x855bf19a),
         R(v26),
         R(p4, x12));  // ld1rw {z26.d}, p4/z, [x12, #108]
  CMP_IO(dci(0xc43b9b3e),
         R(v30),
         R(p6, v25));  // ld1sb {z30.d}, p6/z, [z25.d, #27]

  CLEANUP();
}

TEST(group_31) {
  SETUP();

  CMP_IO(dci(0x843a9573),
         R(v19),
         R(p5, v11));  // ld1sb {z19.s}, p5/z, [z11.s, #26]
  CMP_IO(dci(0xa5cbbf17),
         R(v23),
         R(p7, x24));  // ld1sb {z23.h}, p7/z, [x24, #-5, mul vl]
  CMP_IO(dci(0xa5aaa0a1),
         R(v1),
         R(p0, x5));  // ld1sb {z1.s}, p0/z, [x5, #-6, mul vl]
  CMP_IO(dci(0xa58abfdc),
         R(v28),
         R(p7, x30));  // ld1sb {z28.d}, p7/z, [x30, #-6, mul vl]
  CMP_IO(dci(0xa5dc4e86),
         R(v6),
         R(p3, x20, x28));  // ld1sb {z6.h}, p3/z, [x20, x28]
  CMP_IO(dci(0xa5bb59b4),
         R(v20),
         R(p6, x13, x27));  // ld1sb {z20.s}, p6/z, [x13, x27]
  CMP_IO(dci(0xa59b5a79),
         R(v25),
         R(p6, x19, x27));  // ld1sb {z25.d}, p6/z, [x19, x27]
  CMP_IO(dci(0xc45a80a1),
         R(v1),
         R(p0, x5, v26));  // ld1sb {z1.d}, p0/z, [x5, z26.d]
  CMP_IO(dci(0xc41d06d4),
         R(v20),
         R(p1, x22, v29));  // ld1sb {z20.d}, p1/z, [x22, z29.d, uxtw]
  CMP_IO(dci(0x845c0aad),
         R(v13),
         R(p2, x21, v28));  // ld1sb {z13.s}, p2/z, [x21, z28.s, sxtw]
  CMP_IO(dci(0xc4bc8358),
         R(v24),
         R(p0, v26));  // ld1sh {z24.d}, p0/z, [z26.d, #56]
  CMP_IO(dci(0x84bb93e9),
         R(v9),
         R(p4, v31));  // ld1sh {z9.s}, p4/z, [z31.s, #54]
  CMP_IO(dci(0xa52ba8bb),
         R(v27),
         R(p2, x5));  // ld1sh {z27.s}, p2/z, [x5, #-5, mul vl]
  CMP_IO(dci(0xa50bbe52),
         R(v18),
         R(p7, x18));  // ld1sh {z18.d}, p7/z, [x18, #-5, mul vl]
  CMP_IO(dci(0xa53b519a),
         R(v26),
         R(p4, x12, x27));  // ld1sh {z26.s}, p4/z, [x12, x27, lsl #1]
  CMP_IO(dci(0xa51b5c03),
         R(v3),
         R(p7, x0, x27));  // ld1sh {z3.d}, p7/z, [x0, x27, lsl #1]
  CMP_IO(dci(0xc4fb919a),
         R(v26),
         R(p4, x12, v27));  // ld1sh {z26.d}, p4/z, [x12, z27.d, lsl #1]
  CMP_IO(dci(0xc4db88bb),
         R(v27),
         R(p2, x5, v27));  // ld1sh {z27.d}, p2/z, [x5, z27.d]
  CMP_IO(dci(0xc4bc15db),
         R(v27),
         R(p5, x14, v28));  // ld1sh {z27.d}, p5/z, [x14, z28.d, uxtw #1]
  CMP_IO(dci(0xc4dc1eba),
         R(v26),
         R(p7, x21, v28));  // ld1sh {z26.d}, p7/z, [x21, z28.d, sxtw]
  CMP_IO(dci(0x84fd1ba6),
         R(v6),
         R(p6, x29, v29));  // ld1sh {z6.s}, p6/z, [x29, z29.s, sxtw #1]
  CMP_IO(dci(0x849c0c37),
         R(v23),
         R(p3, x1, v28));  // ld1sh {z23.s}, p3/z, [x1, z28.s, uxtw]
  CMP_IO(dci(0xc53a80a1),
         R(v1),
         R(p0, v5));  // ld1sw {z1.d}, p0/z, [z5.d, #104]
  CMP_IO(dci(0xa48ba5a7),
         R(v7),
         R(p1, x13));  // ld1sw {z7.d}, p1/z, [x13, #-5, mul vl]
  CMP_IO(dci(0xa49b47f6),
         R(v22),
         R(p1, sp, x27));  // ld1sw {z22.d}, p1/z, [sp, x27, lsl #2]
  CMP_IO(dci(0xc57b925f),
         R(v31),
         R(p4, x18, v27));  // ld1sw {z31.d}, p4/z, [x18, z27.d, lsl #2]
  CMP_IO(dci(0xc55b9b3e),
         R(v30),
         R(p6, x25, v27));  // ld1sw {z30.d}, p6/z, [x25, z27.d]
  CMP_IO(dci(0xc57d085e),
         R(v30),
         R(p2, x2, v29));  // ld1sw {z30.d}, p2/z, [x2, z29.d, sxtw #2]
  CMP_IO(dci(0xc51c1f7f),
         R(v31),
         R(p7, x27, v28));  // ld1sw {z31.d}, p7/z, [x27, z28.d, uxtw]
  CMP_IO(dci(0xc53cd38c),
         R(v12),
         R(p4, v28));  // ld1w {z12.d}, p4/z, [z28.d, #112]
  CMP_IO(dci(0x853cd765),
         R(v5),
         R(p5, v27));  // ld1w {z5.s}, p5/z, [z27.s, #112]
  CMP_IO(dci(0xa54cadc1),
         R(v1),
         R(p3, x14));  // ld1w {z1.s}, p3/z, [x14, #-4, mul vl]

  CLEANUP();
}

TEST(group_32) {
  SETUP();

  CMP_IO(dci(0xa56cacfc),
         R(v28),
         R(p3, x7));  // ld1w {z28.d}, p3/z, [x7, #-4, mul vl]
  CMP_IO(dci(0xa55d46d4),
         R(v20),
         R(p1, x22, x29));  // ld1w {z20.s}, p1/z, [x22, x29, lsl #2]
  CMP_IO(dci(0xa57d4799),
         R(v25),
         R(p1, x28, x29));  // ld1w {z25.d}, p1/z, [x28, x29, lsl #2]
  CMP_IO(dci(0xc57ddba6),
         R(v6),
         R(p6, x29, v29));  // ld1w {z6.d}, p6/z, [x29, z29.d, lsl #2]
  CMP_IO(dci(0xc55ccc37),
         R(v23),
         R(p3, x1, v28));  // ld1w {z23.d}, p3/z, [x1, z28.d]
  CMP_IO(dci(0xc53b5f17),
         R(v23),
         R(p7, x24, v27));  // ld1w {z23.d}, p7/z, [x24, z27.d, uxtw #2]
  CMP_IO(dci(0xc55c4e86),
         R(v6),
         R(p3, x20, v28));  // ld1w {z6.d}, p3/z, [x20, z28.d, sxtw]
  CMP_IO(dci(0x857b519a),
         R(v26),
         R(p4, x12, v27));  // ld1w {z26.s}, p4/z, [x12, z27.s, sxtw #2]
  CMP_IO(dci(0x851b48bb),
         R(v27),
         R(p2, x5, v27));  // ld1w {z27.s}, p2/z, [x5, z27.s, uxtw]
  CMP_IO(dci(0x4c4082f0),
         R(v16, v17),
         R(x23));  // ld2 {v16.16b, v17.16b}, [x23]
  CMP_IO(dci(0x4cdf85db),
         R(v27, v28, x14),
         R(x14));  // ld2 {v27.8h, v28.8h}, [x14], #32
  CMP_IO(dci(0x4cc585db),
         R(v27, v28, x14),
         R(x14, x5));  // ld2 {v27.8h, v28.8h}, [x14], x5
  CMP_IO(dci(0x0d600853),
         R(v19, v20),
         R(v19, v20, x2));  // ld2 {v19.b, v20.b}[2], [x2]
  CMP_IO(dci(0x0d608453),
         R(v19, v20),
         R(v19, v20, x2));  // ld2 {v19.d, v20.d}[0], [x2]
  CMP_IO(dci(0x0d604853),
         R(v19, v20),
         R(v19, v20, x2));  // ld2 {v19.h, v20.h}[1], [x2]
  CMP_IO(dci(0x0d608053),
         R(v19, v20),
         R(v19, v20, x2));  // ld2 {v19.s, v20.s}[0], [x2]
  CMP_IO(dci(0x4dff0c94),
         R(v20, v21, x4),
         R(v20, v21, x4));  // ld2 {v20.b, v21.b}[11], [x4], #2
  CMP_IO(dci(0x4de714b9),
         R(v25, v26, x5),
         R(v25, v26, x5, x7));  // ld2 {v25.b, v26.b}[13], [x5], x7
  CMP_IO(dci(0x4dff8494),
         R(v20, v21, x4),
         R(v20, v21, x4));  // ld2 {v20.d, v21.d}[1], [x4], #16
  CMP_IO(dci(0x4dec84b9),
         R(v25, v26, x5),
         R(v25, v26, x5, x12));  // ld2 {v25.d, v26.d}[1], [x5], x12
  CMP_IO(dci(0x4dff4894),
         R(v20, v21, x4),
         R(v20, v21, x4));  // ld2 {v20.h, v21.h}[5], [x4], #4
  CMP_IO(dci(0x4dfe50b9),
         R(v25, v26, x5),
         R(v25, v26, x5, x30));  // ld2 {v25.h, v26.h}[6], [x5], x30
  CMP_IO(dci(0x4dff8094),
         R(v20, v21, x4),
         R(v20, v21, x4));  // ld2 {v20.s, v21.s}[2], [x4], #8
  CMP_IO(dci(0x4de290b9),
         R(v25, v26, x5),
         R(v25, v26, x5, x2));  // ld2 {v25.s, v26.s}[3], [x5], x2
  CMP_IO(dci(0xa42bff17),
         R(v23, v24),
         R(p7, x24));  // ld2b {z23.b, z24.b}, p7/z, [x24, #-10, mul vl]
  CMP_IO(dci(0xa43cce86),
         R(v6, v7),
         R(p3, x20, x28));  // ld2b {z6.b, z7.b}, p3/z, [x20, x28]
  CMP_IO(dci(0xa5aae0a1),
         R(v1, v2),
         R(p0, x5));  // ld2d {z1.d, z2.d}, p0/z, [x5, #-12, mul vl]
  CMP_IO(dci(0xa5bbd9b4),
         R(v20, v21),
         R(p6, x13, x27));  // ld2d {z20.d, z21.d}, p6/z, [x13, x27, lsl #3]
  CMP_IO(dci(0xa4abfd8d),
         R(v13, v14),
         R(p7, x12));  // ld2h {z13.h, z14.h}, p7/z, [x12, #-10, mul vl]
  CMP_IO(dci(0xa4bbd010),
         R(v16, v17),
         R(p4, x0, x27));  // ld2h {z16.h, z17.h}, p4/z, [x0, x27, lsl #1]
  CMP_IO(dci(0x4d60c708), R(v8, v9), R(x24));  // ld2r {v8.8h, v9.8h}, [x24]
  CMP_IO(dci(0x0dffce52),
         R(v18, v19, x18),
         R(x18));  // ld2r {v18.1d, v19.1d}, [x18], #16

  CLEANUP();
}

TEST(group_33) {
  SETUP();

  CMP_IO(dci(0x4deac5db),
         R(v27, v28, x14),
         R(x14, x10));  // ld2r {v27.8h, v28.8h}, [x14], x10
  CMP_IO(dci(0xa52be66c),
         R(v12, v13),
         R(p1, x19));  // ld2w {z12.s, z13.s}, p1/z, [x19, #-10, mul vl]
  CMP_IO(dci(0xa53bd3e9),
         R(v9, v10),
         R(p4, sp, x27));  // ld2w {z9.s, z10.s}, p4/z, [sp, x27, lsl #2]
  CMP_IO(dci(0x4c4042f0),
         R(v16, v17, v18),
         R(x23));  // ld3 {v16.16b, v17.16b, v18.16b}, [x23]
  CMP_IO(dci(0x4cdf45db),
         R(v27, v28, v29, x14),
         R(x14));  // ld3 {v27.8h, v28.8h, v29.8h}, [x14], #48
  CMP_IO(dci(0x4cc745db),
         R(v27, v28, v29, x14),
         R(x14, x7));  // ld3 {v27.8h, v28.8h, v29.8h}, [x14], x7
  CMP_IO(dci(0x4d4032bc),
         R(v28, v29, v30),
         R(v28, v29, v30, x21));  // ld3 {v28.b, v29.b, v30.b}[12], [x21]
  CMP_IO(dci(0x4d40a6bc),
         R(v28, v29, v30),
         R(v28, v29, v30, x21));  // ld3 {v28.d, v29.d, v30.d}[1], [x21]
  CMP_IO(dci(0x4d4072bc),
         R(v28, v29, v30),
         R(v28, v29, v30, x21));  // ld3 {v28.h, v29.h, v30.h}[6], [x21]
  CMP_IO(dci(0x4d40b2bc),
         R(v28, v29, v30),
         R(v28, v29, v30, x21));  // ld3 {v28.s, v29.s, v30.s}[3], [x21]
  CMP_IO(dci(0x0ddf222b),
         R(v11, v12, v13, x17),
         R(v11, v12, v13, x17));  // ld3 {v11.b, v12.b, v13.b}[0], [x17], #3
  CMP_IO(dci(0x0dda3f22),
         R(v2, v3, v4, x25),
         R(v2, v3, v4, x25, x26));  // ld3 {v2.b, v3.b, v4.b}[7], [x25], x26
  CMP_IO(dci(0x0ddfa62b),
         R(v11, v12, v13, x17),
         R(v11, v12, v13, x17));  // ld3 {v11.d, v12.d, v13.d}[0], [x17], #24
  CMP_IO(dci(0x0dcba722),
         R(v2, v3, v4, x25),
         R(v2, v3, v4, x25, x11));  // ld3 {v2.d, v3.d, v4.d}[0], [x25], x11
  CMP_IO(dci(0x0ddf622b),
         R(v11, v12, v13, x17),
         R(v11, v12, v13, x17));  // ld3 {v11.h, v12.h, v13.h}[0], [x17], #6
  CMP_IO(dci(0x0dc77b22),
         R(v2, v3, v4, x25),
         R(v2, v3, v4, x25, x7));  // ld3 {v2.h, v3.h, v4.h}[3], [x25], x7
  CMP_IO(dci(0x0ddfa22b),
         R(v11, v12, v13, x17),
         R(v11, v12, v13, x17));  // ld3 {v11.s, v12.s, v13.s}[0], [x17], #12
  CMP_IO(dci(0x0dd7b322),
         R(v2, v3, v4, x25),
         R(v2, v3, v4, x25, x23));  // ld3 {v2.s, v3.s, v4.s}[1], [x25], x23
  CMP_IO(dci(0xa44be980),
         R(v0, v1, v2),
         R(p2, x12));  // ld3b {z0.b, z1.b, z2.b}, p2/z, [x12, #-15, mul vl]
  CMP_IO(dci(0xa45cc41d),
         R(v29, v30, v31),
         R(p1, x0, x28));  // ld3b {z29.b, z30.b, z31.b}, p1/z, [x0, x28]
  CMP_IO(dci(0xa5caeb0a),
         R(v10, v11, v12),
         R(p2, x24));  // ld3d {z10.d, z11.d, z12.d}, p2/z, [x24, #-18, mul vl]
  CMP_IO(dci(0xa5dbcf4b),
         R(v11, v12, v13),
         R(p3,
           x26,
           x27));  // ld3d {z11.d, z12.d, z13.d}, p3/z, [x26, x27, lsl #3]
  CMP_IO(dci(0xa4cbe7f6),
         R(v22, v23, v24),
         R(p1, sp));  // ld3h {z22.h, z23.h, z24.h}, p1/z, [sp, #-15, mul vl]
  CMP_IO(dci(0xa4dbc5a7),
         R(v7, v8, v9),
         R(p1, x13, x27));  // ld3h {z7.h, z8.h, z9.h}, p1/z, [x13, x27, lsl #1]
  CMP_IO(dci(0x4d40e708),
         R(v8, v9, v10),
         R(x24));  // ld3r {v8.8h, v9.8h, v10.8h}, [x24]
  CMP_IO(dci(0x0ddfee52),
         R(v18, v19, v20, x18),
         R(x18));  // ld3r {v18.1d, v19.1d, v20.1d}, [x18], #24
  CMP_IO(dci(0x4dd5e5db),
         R(v27, v28, v29, x14),
         R(x14, x21));  // ld3r {v27.8h, v28.8h, v29.8h}, [x14], x21
  CMP_IO(dci(0xa54bfc03),
         R(v3, v4, v5),
         R(p7, x0));  // ld3w {z3.s, z4.s, z5.s}, p7/z, [x0, #-15, mul vl]
  CMP_IO(dci(0xa55bde52),
         R(v18, v19, v20),
         R(p7,
           x18,
           x27));  // ld3w {z18.s, z19.s, z20.s}, p7/z, [x18, x27, lsl #2]
  CMP_IO(dci(0x4c4002f0),
         R(v16, v17, v18, v19),
         R(x23));  // ld4 {v16.16b, v17.16b, v18.16b, v19.16b}, [x23]
  CMP_IO(dci(0x4cdf05db),
         R(v27, v28, v29, v30, x14),
         R(x14));  // ld4 {v27.8h, v28.8h, v29.8h, v30.8h}, [x14], #64
  CMP_IO(dci(0x4cce05db),
         R(v27, v28, v29, v30, x14),
         R(x14));  // ld4 {v27.8h, v28.8h, v29.8h, v30.8h}, [x14], x14

  CLEANUP();
}

TEST(group_34) {
  SETUP();

  CMP_IO(dci(0x0d603d25),
         R(v5, v6, v7, v8),
         R(v5, v6, v7, v8, x9));  // ld4 {v5.b, v6.b, v7.b, v8.b}[7], [x9]
  CMP_IO(dci(0x0d60a525),
         R(v5, v6, v7, v8),
         R(v5, v6, v7, v8, x9));  // ld4 {v5.d, v6.d, v7.d, v8.d}[0], [x9]
  CMP_IO(dci(0x0d607925),
         R(v5, v6, v7, v8),
         R(v5, v6, v7, v8, x9));  // ld4 {v5.h, v6.h, v7.h, v8.h}[3], [x9]
  CMP_IO(dci(0x0d60b125),
         R(v5, v6, v7, v8),
         R(v5, v6, v7, v8, x9));  // ld4 {v5.s, v6.s, v7.s, v8.s}[1], [x9]
  CMP_IO(dci(0x0dff2166),
         R(v6, v7, v8, v9, x11),
         R(v6, v7, v8, v9, x11));  // ld4 {v6.b, v7.b, v8.b, v9.b}[0], [x11], #4
  CMP_IO(dci(0x0de4332f),
         R(v15, v16, v17, v18, x25),
         R(v15, v16, v17, v18, x25, x4));  // ld4 {v15.b, v16.b, v17.b,
                                           // v18.b}[4], [x25], x4
  CMP_IO(dci(0x0dffa566),
         R(v6, v7, v8, v9, x11),
         R(v6,
           v7,
           v8,
           v9,
           x11));  // ld4 {v6.d, v7.d, v8.d, v9.d}[0], [x11], #32
  CMP_IO(dci(0x0defa72f),
         R(v15, v16, v17, v18, x25),
         R(v15, v16, v17, v18, x25, x15));  // ld4 {v15.d, v16.d, v17.d,
                                            // v18.d}[0], [x25], x15
  CMP_IO(dci(0x0dff6166),
         R(v6, v7, v8, v9, x11),
         R(v6, v7, v8, v9, x11));  // ld4 {v6.h, v7.h, v8.h, v9.h}[0], [x11], #8
  CMP_IO(dci(0x0de9732f),
         R(v15, v16, v17, v18, x25),
         R(v15, v16, v17, v18, x25, x9));  // ld4 {v15.h, v16.h, v17.h,
                                           // v18.h}[2], [x25], x9
  CMP_IO(dci(0x0dffa166),
         R(v6, v7, v8, v9, x11),
         R(v6,
           v7,
           v8,
           v9,
           x11));  // ld4 {v6.s, v7.s, v8.s, v9.s}[0], [x11], #16
  CMP_IO(dci(0x0dfdb32f),
         R(v15, v16, v17, v18, x25),
         R(v15, v16, v17, v18, x25, x29));  // ld4 {v15.s, v16.s, v17.s,
                                            // v18.s}[1], [x25], x29
  CMP_IO(dci(0xa46ae0a1),
         R(v1, v2, v3, v4),
         R(p0, x5));  // ld4b {z1.b, z2.b, z3.b, z4.b}, p0/z, [x5, #-24, mul vl]
  CMP_IO(dci(0xa47bd9b4),
         R(v20, v21, v22, v23),
         R(p6,
           x13,
           x27));  // ld4b {z20.b, z21.b, z22.b, z23.b}, p6/z, [x13, x27]
  CMP_IO(dci(0xa5ebff17), R(v23, v24, v25, v26), R(p7, x24));  // ld4d {z23.d,
                                                               // z24.d, z25.d,
                                                               // z26.d}, p7/z,
                                                               // [x24, #-20,
                                                               // mul vl]
  CMP_IO(dci(0xa5fcce86),
         R(v6, v7, v8, v9),
         R(p3,
           x20,
           x28));  // ld4d {z6.d, z7.d, z8.d, z9.d}, p3/z, [x20, x28, lsl #3]
  CMP_IO(dci(0xa4ebe8bb),
         R(v27, v28, v29, v30),
         R(p2,
           x5));  // ld4h {z27.h, z28.h, z29.h, z30.h}, p2/z, [x5, #-20, mul vl]
  CMP_IO(dci(0xa4fbd19a),
         R(v26, v27, v28, v29),
         R(p4, x12, x27));  // ld4h {z26.h, z27.h, z28.h, z29.h}, p4/z, [x12,
                            // x27, lsl #1]
  CMP_IO(dci(0x4d60e708),
         R(v8, v9, v10, v11),
         R(x24));  // ld4r {v8.8h, v9.8h, v10.8h, v11.8h}, [x24]
  CMP_IO(dci(0x0dffee52),
         R(v18, v19, v20, v21, x18),
         R(x18));  // ld4r {v18.1d, v19.1d, v20.1d, v21.1d}, [x18], #32
  CMP_IO(dci(0x4df8e5db),
         R(v27, v28, v29, v30, x14),
         R(x14, x24));  // ld4r {v27.8h, v28.8h, v29.8h, v30.8h}, [x14], x24
  CMP_IO(dci(0xa56bfb3e),
         R(v30, v31, v0, v1),
         R(p6,
           x25));  // ld4w {z30.s, z31.s, z0.s, z1.s}, p6/z, [x25, #-20, mul vl]
  CMP_IO(dci(0xa57bd25f),
         R(v31, v0, v1, v2),
         R(p4,
           x18,
           x27));  // ld4w {z31.s, z0.s, z1.s, z2.s}, p4/z, [x18, x27, lsl #2]
  CMP_IO(dci(0xb83d0092), R(x18), R(x29, x4));   // ldadd w29, w18, [x4]
  CMP_IO(dci(0xf83d02fb), R(x27), R(x29, x23));  // ldadd x29, x27, [x23]
  CMP_IO(dci(0xb8b902c9), R(x9), R(x25, x22));   // ldadda w25, w9, [x22]
  CMP_IO(dci(0xf8ba0204), R(x4), R(x26, x16));   // ldadda x26, x4, [x16]
  CMP_IO(dci(0x38bd02fb), R(x27), R(x29, x23));  // ldaddab w29, w27, [x23]
  CMP_IO(dci(0x78bd02e1), R(x1), R(x29, x23));   // ldaddah w29, w1, [x23]
  CMP_IO(dci(0xb8fd0085), R(x5), R(x29, x4));    // ldaddal w29, w5, [x4]
  CMP_IO(dci(0xf8fd03c0), R(x0), R(x29, x30));   // ldaddal x29, x0, [x30]
  CMP_IO(dci(0x38f901f7), R(x23), R(x25, x15));  // ldaddalb w25, w23, [x15]

  CLEANUP();
}

TEST(group_35) {
  SETUP();

  CMP_IO(dci(0x78fa006d), R(x13), R(x26, x3));   // ldaddalh w26, w13, [x3]
  CMP_IO(dci(0x383a0204), R(x4), R(x26, x16));   // ldaddb w26, w4, [x16]
  CMP_IO(dci(0x783901ea), R(x10), R(x25, x15));  // ldaddh w25, w10, [x15]
  CMP_IO(dci(0xb879038e), R(x14), R(x25, x28));  // ldaddl w25, w14, [x28]
  CMP_IO(dci(0xf87901f7), R(x23), R(x25, x15));  // ldaddl x25, x23, [x15]
  CMP_IO(dci(0x387d03c0), R(x0), R(x29, x30));   // ldaddlb w29, w0, [x30]
  CMP_IO(dci(0x787e0236), R(x22), R(x30, x17));  // ldaddlh w30, w22, [x17]
  CMP_IO(dci(0xb8b9c060), R(x0), R(x3));         // ldapr w0, [x3]
  CMP_IO(dci(0xf8b9c125), R(x5), R(x9));         // ldapr x5, [x9]
  CMP_IO(dci(0x38bdc2ee), R(x14), R(x23));       // ldaprb w14, [x23]
  CMP_IO(dci(0x78bec308), R(x8), R(x24));        // ldaprh w8, [x24]
  CMP_IO(dci(0x995cb37f), R(), R(x27));          // ldapur wzr, [x27, #-53]
  CMP_IO(dci(0xd95cb116), R(x22), R(x8));        // ldapur x22, [x8, #-53]
  CMP_IO(dci(0x195b518d), R(x13), R(x12));       // ldapurb w13, [x12, #-75]
  CMP_IO(dci(0x595b1317), R(x23), R(x24));       // ldapurh w23, [x24, #-79]
  CMP_IO(dci(0x19dd7078), R(x24), R(x3));        // ldapursb w24, [x3, #-41]
  CMP_IO(dci(0x199d513d), R(x29), R(x9));        // ldapursb x29, [x9, #-43]
  CMP_IO(dci(0x59dd005e), R(x30), R(x2));        // ldapursh w30, [x2, #-48]
  CMP_IO(dci(0x599d12c7), R(x7), R(x22));        // ldapursh x7, [x22, #-47]
  CMP_IO(dci(0x999c8372), R(x18), R(x27));       // ldapursw x18, [x27, #-56]
  CMP_IO(dci(0x88dd8fb3), R(x19), R(x29));       // ldar w19, [x29]
  CMP_IO(dci(0xc8dd854a), R(x10), R(x10));       // ldar x10, [x10]
  CMP_IO(dci(0x08d99381), R(x1), R(x28));        // ldarb w1, [x28]
  CMP_IO(dci(0x48d9fb9b), R(x27), R(x28));       // ldarh w27, [x28]
  CMP_IO(dci(0x887aa22b), R(x11, x8), R(x17));   // ldaxp w11, w8, [x17]
  CMP_IO(dci(0xc87a97c2), R(x2, x5), R(x30));    // ldaxp x2, x5, [x30]
  CMP_IO(dci(0x885aa22b), R(x11), R(x17));       // ldaxr w11, [x17]
  CMP_IO(dci(0xc85a97c2), R(x2), R(x30));        // ldaxr x2, [x30]
  CMP_IO(dci(0x085ef4b9), R(x25), R(x5));        // ldaxrb w25, [x5]
  CMP_IO(dci(0x485eb643), R(x3), R(x18));        // ldaxrh w3, [x18]
  CMP_IO(dci(0xb83e117e), R(x30), R(x30, x11));  // ldclr w30, w30, [x11]
  CMP_IO(dci(0xf83e13e7), R(x7), R(x30, sp));    // ldclr x30, x7, [sp]

  CLEANUP();
}

TEST(group_36) {
  SETUP();

  CMP_IO(dci(0xb8ba13b5), R(x21), R(x26, x29));  // ldclra w26, w21, [x29]
  CMP_IO(dci(0xf8ba12f0), R(x16), R(x26, x23));  // ldclra x26, x16, [x23]
  CMP_IO(dci(0x38be13e7), R(x7), R(x30, sp));    // ldclrab w30, w7, [sp]
  CMP_IO(dci(0x78be125d), R(x29), R(x30, x18));  // ldclrah w30, w29, [x18]
  CMP_IO(dci(0xb8fe10b9), R(x25), R(x30, x5));   // ldclral w30, w25, [x5]
  CMP_IO(dci(0xf8fe13f4), R(x20), R(x30, sp));   // ldclral x30, x20, [sp]
  CMP_IO(dci(0x38fa122b), R(x11), R(x26, x17));  // ldclralb w26, w11, [x17]
  CMP_IO(dci(0x78fa1211), R(x17), R(x26, x16));  // ldclralh w26, w17, [x16]
  CMP_IO(dci(0x383a12f0), R(x16), R(x26, x23));  // ldclrb w26, w16, [x23]
  CMP_IO(dci(0x783a1166), R(x6), R(x26, x11));   // ldclrh w26, w6, [x11]
  CMP_IO(dci(0xb87a13c2), R(x2), R(x26, x30));   // ldclrl w26, w2, [x30]
  CMP_IO(dci(0xf87a122b), R(x11), R(x26, x17));  // ldclrl x26, x11, [x17]
  CMP_IO(dci(0x387e13f4), R(x20), R(x30, sp));   // ldclrlb w30, w20, [sp]
  CMP_IO(dci(0x787e13da), R(x26), R(x30, x30));  // ldclrlh w30, w26, [x30]
  CMP_IO(dci(0xb83e2171), R(x17), R(x30, x11));  // ldeor w30, w17, [x11]
  CMP_IO(dci(0xf83e20ac), R(x12), R(x30, x5));   // ldeor x30, x12, [x5]
  CMP_IO(dci(0xb8ba207a), R(x26), R(x26, x3));   // ldeora w26, w26, [x3]
  CMP_IO(dci(0xf8ba22e3), R(x3), R(x26, x23));   // ldeora x26, x3, [x23]
  CMP_IO(dci(0x38be20ac), R(x12), R(x30, x5));   // ldeorab w30, w12, [x5]
  CMP_IO(dci(0x78be2322), R(x2), R(x30, x25));   // ldeorah w30, w2, [x25]
  CMP_IO(dci(0xb8fe20c6), R(x6), R(x30, x6));    // ldeoral w30, w6, [x6]
  CMP_IO(dci(0xf8fe232f), R(x15), R(x30, x25));  // ldeoral x30, x15, [x25]
  CMP_IO(dci(0x38fa2238), R(x24), R(x26, x17));  // ldeoralb w26, w24, [x17]
  CMP_IO(dci(0x78fa221e), R(x30), R(x26, x16));  // ldeoralh w26, w30, [x16]
  CMP_IO(dci(0x383a22e3), R(x3), R(x26, x23));   // ldeorb w26, w3, [x23]
  CMP_IO(dci(0x783a2159), R(x25), R(x26, x10));  // ldeorh w26, w25, [x10]
  CMP_IO(dci(0xb87a22fd), R(x29), R(x26, x23));  // ldeorl w26, w29, [x23]
  CMP_IO(dci(0xf87a2238), R(x24), R(x26, x17));  // ldeorl x26, x24, [x17]
  CMP_IO(dci(0x387e232f), R(x15), R(x30, x25));  // ldeorlb w30, w15, [x25]
  CMP_IO(dci(0x787e2315), R(x21), R(x30, x24));  // ldeorlh w30, w21, [x24]
  CMP_IO(dci(0xc43cec37),
         R(v23),
         R(p3, v1));  // ldff1b {z23.d}, p3/z, [z1.d, #28]
  CMP_IO(dci(0x843cfeba),
         R(v26),
         R(p7, v21));  // ldff1b {z26.s}, p7/z, [z21.s, #28]

  CLEANUP();
}

TEST(group_37) {
  SETUP();

  CMP_IO(dci(0xa43c6109),
         R(v9),
         R(p0, x8, x28));  // ldff1b {z9.h}, p0/z, [x8, x28]
  CMP_IO(dci(0xa45c7f7f),
         R(v31),
         R(p7, x27, x28));  // ldff1b {z31.s}, p7/z, [x27, x28]
  CMP_IO(dci(0xa47c7516),
         R(v22),
         R(p5, x8, x28));  // ldff1b {z22.d}, p5/z, [x8, x28]
  CMP_IO(dci(0xa41e6a50),
         R(v16),
         R(p2, x18, x30));  // ldff1b {z16.b}, p2/z, [x18, x30]
  CMP_IO(dci(0xc45cf38c),
         R(v12),
         R(p4, x28, v28));  // ldff1b {z12.d}, p4/z, [x28, z28.d]
  CMP_IO(dci(0xc45b73e9),
         R(v9),
         R(p4, sp, v27));  // ldff1b {z9.d}, p4/z, [sp, z27.d, sxtw]
  CMP_IO(dci(0x841c6358),
         R(v24),
         R(p0, x26, v28));  // ldff1b {z24.s}, p0/z, [x26, z28.s, uxtw]
  CMP_IO(dci(0xc5bcedc1),
         R(v1),
         R(p3, v14));  // ldff1d {z1.d}, p3/z, [z14.d, #224]
  CMP_IO(dci(0xa5fc6044),
         R(v4),
         R(p0, x2, x28));  // ldff1d {z4.d}, p0/z, [x2, x28, lsl #3]
  CMP_IO(dci(0xc5fcff7f),
         R(v31),
         R(p7, x27, v28));  // ldff1d {z31.d}, p7/z, [x27, z28.d, lsl #3]
  CMP_IO(dci(0xc5dde85e),
         R(v30),
         R(p2, x2, v29));  // ldff1d {z30.d}, p2/z, [x2, z29.d]
  CMP_IO(dci(0xc5fb7b3e),
         R(v30),
         R(p6, x25, v27));  // ldff1d {z30.d}, p6/z, [x25, z27.d, sxtw #3]
  CMP_IO(dci(0xc59b725f),
         R(v31),
         R(p4, x18, v27));  // ldff1d {z31.d}, p4/z, [x18, z27.d, uxtw]
  CMP_IO(dci(0xc4bceaad),
         R(v13),
         R(p2, v21));  // ldff1h {z13.d}, p2/z, [z21.d, #56]
  CMP_IO(dci(0x84bde6d4),
         R(v20),
         R(p1, v22));  // ldff1h {z20.s}, p1/z, [z22.s, #58]
  CMP_IO(dci(0xa4bc6293),
         R(v19),
         R(p0, x20, x28));  // ldff1h {z19.h}, p0/z, [x20, x28, lsl #1]
  CMP_IO(dci(0xa4dc7765),
         R(v5),
         R(p5, x27, x28));  // ldff1h {z5.s}, p5/z, [x27, x28, lsl #1]
  CMP_IO(dci(0xa4fc76a0),
         R(v0),
         R(p5, x21, x28));  // ldff1h {z0.d}, p5/z, [x21, x28, lsl #1]
  CMP_IO(dci(0xc4fce293),
         R(v19),
         R(p0, x20, v28));  // ldff1h {z19.d}, p0/z, [x20, z28.d, lsl #1]
  CMP_IO(dci(0xc4ddf202),
         R(v2),
         R(p4, x16, v29));  // ldff1h {z2.d}, p4/z, [x16, z29.d]
  CMP_IO(dci(0xc4fb64e2),
         R(v2),
         R(p1, x7, v27));  // ldff1h {z2.d}, p1/z, [x7, z27.d, sxtw #1]
  CMP_IO(dci(0xc49a7573),
         R(v19),
         R(p5, x11, v26));  // ldff1h {z19.d}, p5/z, [x11, z26.d, uxtw]
  CMP_IO(dci(0x84bb725f),
         R(v31),
         R(p4, x18, v27));  // ldff1h {z31.s}, p4/z, [x18, z27.s, uxtw #1]
  CMP_IO(dci(0x84db7b3e),
         R(v30),
         R(p6, x25, v27));  // ldff1h {z30.s}, p6/z, [x25, z27.s, sxtw]
  CMP_IO(dci(0xc43bbb3e),
         R(v30),
         R(p6, v25));  // ldff1sb {z30.d}, p6/z, [z25.d, #27]
  CMP_IO(dci(0x843ab573),
         R(v19),
         R(p5, v11));  // ldff1sb {z19.s}, p5/z, [z11.s, #26]
  CMP_IO(dci(0xa5dc6e86),
         R(v6),
         R(p3, x20, x28));  // ldff1sb {z6.h}, p3/z, [x20, x28]
  CMP_IO(dci(0xa5bb79b4),
         R(v20),
         R(p6, x13, x27));  // ldff1sb {z20.s}, p6/z, [x13, x27]
  CMP_IO(dci(0xa59b7a79),
         R(v25),
         R(p6, x19, x27));  // ldff1sb {z25.d}, p6/z, [x19, x27]
  CMP_IO(dci(0xc45aa0a1),
         R(v1),
         R(p0, x5, v26));  // ldff1sb {z1.d}, p0/z, [x5, z26.d]
  CMP_IO(dci(0xc41d26d4),
         R(v20),
         R(p1, x22, v29));  // ldff1sb {z20.d}, p1/z, [x22, z29.d, uxtw]
  CMP_IO(dci(0x845c2aad),
         R(v13),
         R(p2, x21, v28));  // ldff1sb {z13.s}, p2/z, [x21, z28.s, sxtw]

  CLEANUP();
}

TEST(group_38) {
  SETUP();

  CMP_IO(dci(0xc4bca358),
         R(v24),
         R(p0, v26));  // ldff1sh {z24.d}, p0/z, [z26.d, #56]
  CMP_IO(dci(0x84bbb3e9),
         R(v9),
         R(p4, v31));  // ldff1sh {z9.s}, p4/z, [z31.s, #54]
  CMP_IO(dci(0xa53b719a),
         R(v26),
         R(p4, x12, x27));  // ldff1sh {z26.s}, p4/z, [x12, x27, lsl #1]
  CMP_IO(dci(0xa51b7c03),
         R(v3),
         R(p7, x0, x27));  // ldff1sh {z3.d}, p7/z, [x0, x27, lsl #1]
  CMP_IO(dci(0xc4fbb19a),
         R(v26),
         R(p4, x12, v27));  // ldff1sh {z26.d}, p4/z, [x12, z27.d, lsl #1]
  CMP_IO(dci(0xc4dba8bb),
         R(v27),
         R(p2, x5, v27));  // ldff1sh {z27.d}, p2/z, [x5, z27.d]
  CMP_IO(dci(0xc4bc35db),
         R(v27),
         R(p5, x14, v28));  // ldff1sh {z27.d}, p5/z, [x14, z28.d, uxtw #1]
  CMP_IO(dci(0xc4dc3eba),
         R(v26),
         R(p7, x21, v28));  // ldff1sh {z26.d}, p7/z, [x21, z28.d, sxtw]
  CMP_IO(dci(0x84fd3ba6),
         R(v6),
         R(p6, x29, v29));  // ldff1sh {z6.s}, p6/z, [x29, z29.s, sxtw #1]
  CMP_IO(dci(0x849c2c37),
         R(v23),
         R(p3, x1, v28));  // ldff1sh {z23.s}, p3/z, [x1, z28.s, uxtw]
  CMP_IO(dci(0xc53aa0a1),
         R(v1),
         R(p0, v5));  // ldff1sw {z1.d}, p0/z, [z5.d, #104]
  CMP_IO(dci(0xa49b67f6),
         R(v22),
         R(p1, sp, x27));  // ldff1sw {z22.d}, p1/z, [sp, x27, lsl #2]
  CMP_IO(dci(0xc57bb25f),
         R(v31),
         R(p4, x18, v27));  // ldff1sw {z31.d}, p4/z, [x18, z27.d, lsl #2]
  CMP_IO(dci(0xc55bbb3e),
         R(v30),
         R(p6, x25, v27));  // ldff1sw {z30.d}, p6/z, [x25, z27.d]
  CMP_IO(dci(0xc57d285e),
         R(v30),
         R(p2, x2, v29));  // ldff1sw {z30.d}, p2/z, [x2, z29.d, sxtw #2]
  CMP_IO(dci(0xc51c3f7f),
         R(v31),
         R(p7, x27, v28));  // ldff1sw {z31.d}, p7/z, [x27, z28.d, uxtw]
  CMP_IO(dci(0xc53cf38c),
         R(v12),
         R(p4, v28));  // ldff1w {z12.d}, p4/z, [z28.d, #112]
  CMP_IO(dci(0x853cf765),
         R(v5),
         R(p5, v27));  // ldff1w {z5.s}, p5/z, [z27.s, #112]
  CMP_IO(dci(0xa55d66d4),
         R(v20),
         R(p1, x22, x29));  // ldff1w {z20.s}, p1/z, [x22, x29, lsl #2]
  CMP_IO(dci(0xa57d6799),
         R(v25),
         R(p1, x28, x29));  // ldff1w {z25.d}, p1/z, [x28, x29, lsl #2]
  CMP_IO(dci(0xc57dfba6),
         R(v6),
         R(p6, x29, v29));  // ldff1w {z6.d}, p6/z, [x29, z29.d, lsl #2]
  CMP_IO(dci(0xc55cec37),
         R(v23),
         R(p3, x1, v28));  // ldff1w {z23.d}, p3/z, [x1, z28.d]
  CMP_IO(dci(0xc53b7f17),
         R(v23),
         R(p7, x24, v27));  // ldff1w {z23.d}, p7/z, [x24, z27.d, uxtw #2]
  CMP_IO(dci(0xc55c6e86),
         R(v6),
         R(p3, x20, v28));  // ldff1w {z6.d}, p3/z, [x20, z28.d, sxtw]
  CMP_IO(dci(0x857b719a),
         R(v26),
         R(p4, x12, v27));  // ldff1w {z26.s}, p4/z, [x12, z27.s, sxtw #2]
  CMP_IO(dci(0x851b68bb),
         R(v27),
         R(p2, x5, v27));  // ldff1w {z27.s}, p2/z, [x5, z27.s, uxtw]
  CMP_IO(dci(0xd97e7322), R(x2), R(x25));   // ldg x2, [x25, #-400]
  CMP_IO(dci(0x88da253f), R(), R(x9));      // ldlar wzr, [x9]
  CMP_IO(dci(0xc8da1ad6), R(x22), R(x22));  // ldlar x22, [x22]
  CMP_IO(dci(0x08dd77cd), R(x13), R(x30));  // ldlarb w13, [x30]
  CMP_IO(dci(0x48dd3957), R(x23), R(x10));  // ldlarh w23, [x10]
  CMP_IO(dci(0xa43da6d4),
         R(v20),
         R(p1, x22));  // ldnf1b {z20.h}, p1/z, [x22, #-3, mul vl]

  CLEANUP();
}

TEST(group_39) {
  SETUP();

  CMP_IO(dci(0xa45dbba6),
         R(v6),
         R(p6, x29));  // ldnf1b {z6.s}, p6/z, [x29, #-3, mul vl]
  CMP_IO(dci(0xa47da60f),
         R(v15),
         R(p1, x16));  // ldnf1b {z15.d}, p1/z, [x16, #-3, mul vl]
  CMP_IO(dci(0xa41da485),
         R(v5),
         R(p1, x4));  // ldnf1b {z5.b}, p1/z, [x4, #-3, mul vl]
  CMP_IO(dci(0xa5fda799),
         R(v25),
         R(p1, x28));  // ldnf1d {z25.d}, p1/z, [x28, #-3, mul vl]
  CMP_IO(dci(0xa4bcbeba),
         R(v26),
         R(p7, x21));  // ldnf1h {z26.h}, p7/z, [x21, #-4, mul vl]
  CMP_IO(dci(0xa4ddbd30),
         R(v16),
         R(p7, x9));  // ldnf1h {z16.s}, p7/z, [x9, #-3, mul vl]
  CMP_IO(dci(0xa4fcbdf5),
         R(v21),
         R(p7, x15));  // ldnf1h {z21.d}, p7/z, [x15, #-4, mul vl]
  CMP_IO(dci(0xa5dbb25f),
         R(v31),
         R(p4, x18));  // ldnf1sb {z31.h}, p4/z, [x18, #-5, mul vl]
  CMP_IO(dci(0xa5bbb3e9),
         R(v9),
         R(p4, sp));  // ldnf1sb {z9.s}, p4/z, [sp, #-5, mul vl]
  CMP_IO(dci(0xa59bb324),
         R(v4),
         R(p4, x25));  // ldnf1sb {z4.d}, p4/z, [x25, #-5, mul vl]
  CMP_IO(dci(0xa53ab573),
         R(v19),
         R(p5, x11));  // ldnf1sh {z19.s}, p5/z, [x11, #-6, mul vl]
  CMP_IO(dci(0xa51aab0a),
         R(v10),
         R(p2, x24));  // ldnf1sh {z10.d}, p2/z, [x24, #-6, mul vl]
  CMP_IO(dci(0xa49cb8ef),
         R(v15),
         R(p6, x7));  // ldnf1sw {z15.d}, p6/z, [x7, #-4, mul vl]
  CMP_IO(dci(0xa55ca109),
         R(v9),
         R(p0, x8));  // ldnf1w {z9.s}, p0/z, [x8, #-4, mul vl]
  CMP_IO(dci(0xa57ca044),
         R(v4),
         R(p0, x2));  // ldnf1w {z4.d}, p0/z, [x2, #-4, mul vl]
  CMP_IO(dci(0x285b14ae), R(x14, x5), R(x5));   // ldnp w14, w5, [x5, #216]
  CMP_IO(dci(0xa87b1f17), R(x23, x7), R(x24));  // ldnp x23, x7, [x24, #-80]
  CMP_IO(dci(0x6c7c2dc1), R(v1, v11), R(x14));  // ldnp d1, d11, [x14, #-64]
  CMP_IO(dci(0xac5d085e), R(v30, v2), R(x2));   // ldnp q30, q2, [x2, #928]
  CMP_IO(dci(0x2c5d1d30), R(v16, v7), R(x9));   // ldnp s16, s7, [x9, #232]
  CMP_IO(dci(0xc41cc293),
         R(v19),
         R(p0, v20, x28));  // ldnt1b {z19.d}, p0/z, [z20.d, x28]
  CMP_IO(dci(0x841bb25f),
         R(v31),
         R(p4, v18, x27));  // ldnt1b {z31.s}, p4/z, [z18.s, x27]
  CMP_IO(dci(0xa40ce358),
         R(v24),
         R(p0, x26));  // ldnt1b {z24.b}, p0/z, [x26, #-4, mul vl]
  CMP_IO(dci(0xa41bca45),
         R(v5),
         R(p2, x18, x27));  // ldnt1b {z5.b}, p2/z, [x18, x27]
  CMP_IO(dci(0xc59cd765),
         R(v5),
         R(p5, v27, x28));  // ldnt1d {z5.d}, p5/z, [z27.d, x28]
  CMP_IO(dci(0xa58be4e2),
         R(v2),
         R(p1, x7));  // ldnt1d {z2.d}, p1/z, [x7, #-5, mul vl]
  CMP_IO(dci(0xa59ad573),
         R(v19),
         R(p5, x11, x26));  // ldnt1d {z19.d}, p5/z, [x11, x26, lsl #3]
  CMP_IO(dci(0xc49cc109),
         R(v9),
         R(p0, v8, x28));  // ldnt1h {z9.d}, p0/z, [z8.d, x28]
  CMP_IO(dci(0x849baa45),
         R(v5),
         R(p2, v18, x27));  // ldnt1h {z5.s}, p2/z, [z18.s, x27]
  CMP_IO(dci(0xa48bfb3e),
         R(v30),
         R(p6, x25));  // ldnt1h {z30.h}, p6/z, [x25, #-5, mul vl]
  CMP_IO(dci(0xa49bd25f),
         R(v31),
         R(p4, x18, x27));  // ldnt1h {z31.h}, p4/z, [x18, x27, lsl #1]
  CMP_IO(dci(0xc41b919a),
         R(v26),
         R(p4, v12, x27));  // ldnt1sb {z26.d}, p4/z, [z12.d, x27]

  CLEANUP();
}

TEST(group_40) {
  SETUP();

  CMP_IO(dci(0x841d9ba6),
         R(v6),
         R(p6, v29, x29));  // ldnt1sb {z6.s}, p6/z, [z29.s, x29]
  CMP_IO(dci(0xc49b99b4),
         R(v20),
         R(p6, v13, x27));  // ldnt1sh {z20.d}, p6/z, [z13.d, x27]
  CMP_IO(dci(0x849d9d30),
         R(v16),
         R(p7, v9, x29));  // ldnt1sh {z16.s}, p7/z, [z9.s, x29]
  CMP_IO(dci(0xc51b8a45),
         R(v5),
         R(p2, v18, x27));  // ldnt1sw {z5.d}, p2/z, [z18.d, x27]
  CMP_IO(dci(0xc51ddd30),
         R(v16),
         R(p7, v9, x29));  // ldnt1w {z16.d}, p7/z, [z9.d, x29]
  CMP_IO(dci(0x851bb9b4),
         R(v20),
         R(p6, v13, x27));  // ldnt1w {z20.s}, p6/z, [z13.s, x27]
  CMP_IO(dci(0xa50be8bb),
         R(v27),
         R(p2, x5));  // ldnt1w {z27.s}, p2/z, [x5, #-5, mul vl]
  CMP_IO(dci(0xa51bd19a),
         R(v26),
         R(p4, x12, x27));  // ldnt1w {z26.s}, p4/z, [x12, x27, lsl #2]
  CMP_IO(dci(0x295bd010), R(x16, x20), R(x0));       // ldp w16, w20, [x0, #220]
  CMP_IO(dci(0x28fc2dc1), R(x1, x11, x14), R(x14));  // ldp w1, w11, [x14], #-32
  CMP_IO(dci(0x29dc2358),
         R(x24, x8, x26),
         R(x26));                                   // ldp w24, w8, [x26, #224]!
  CMP_IO(dci(0xa97bb0d5), R(x21, x12), R(x6));      // ldp x21, x12, [x6, #-72]
  CMP_IO(dci(0xa8dc4cfc), R(x28, x19, x7), R(x7));  // ldp x28, x19, [x7], #448
  CMP_IO(dci(0xa9fc041d), R(x29, x1, x0), R(x0));   // ldp x29, x1, [x0, #-64]!
  CMP_IO(dci(0x6d7ce923), R(v3, v26), R(x9));       // ldp d3, d26, [x9, #-56]
  CMP_IO(dci(0x6cdb14ae), R(v14, v5, x5), R(x5));   // ldp d14, d5, [x5], #432
  CMP_IO(dci(0x6dfd3c6b), R(v11, v15, x3), R(x3));  // ldp d11, d15, [x3, #-48]!
  CMP_IO(dci(0xad5c76a0), R(v0, v29), R(x21));      // ldp q0, q29, [x21, #896]
  CMP_IO(dci(0xacfb8731),
         R(v17, v1, x25),
         R(x25));  // ldp q17, q1, [x25], #-144
  CMP_IO(dci(0xaddcc9e8),
         R(v8, v18, x15),
         R(x15));                                   // ldp q8, q18, [x15, #912]!
  CMP_IO(dci(0x2d5c61ce), R(v14, v24), R(x14));     // ldp s14, s24, [x14, #224]
  CMP_IO(dci(0x2cfb9c03), R(v3, v7, x0), R(x0));    // ldp s3, s7, [x0], #-36
  CMP_IO(dci(0x2ddcb516), R(v22, v13, x8), R(x8));  // ldp s22, s13, [x8, #228]!
  CMP_IO(dci(0x697b8731), R(x17, x1), R(x25));  // ldpsw x17, x1, [x25, #-36]
  CMP_IO(dci(0x68dc76a0),
         R(x0, x29, x21),
         R(x21));  // ldpsw x0, x29, [x21], #224
  CMP_IO(dci(0x69fbda79),
         R(x25, x22, x19),
         R(x19));                                // ldpsw x25, x22, [x19, #-36]!
  CMP_IO(dci(0xb85ae70a), R(x10, x24), R(x24));  // ldr w10, [x24], #-82
  CMP_IO(dci(0xb85d6e0f), R(x15, x16), R(x16));  // ldr w15, [x16, #-42]!
  CMP_IO(dci(0xb97c041d), R(x29), R(x0));        // ldr w29, [x0, #15364]
  CMP_IO(dci(0xb87cc9e8), R(x8), R(x15, x28));   // ldr w8, [x15, w28, sxtw]
  CMP_IO(dci(0xf85af573), R(x19, x11), R(x11));  // ldr x19, [x11], #-81
  CMP_IO(dci(0xf85d5fa6), R(x6, x29), R(x29));   // ldr x6, [x29, #-43]!

  CLEANUP();
}

TEST(group_41) {
  SETUP();

  CMP_IO(dci(0xf95c2358), R(x24), R(x26));  // ldr x24, [x26, #14400]
  CMP_IO(dci(0xf87caaad),
         R(x13),
         R(x21, x28));  // ldr x13, [x21, x28, undefined]
  CMP_IO(dci(0x3c5c4693), R(v19, x20), R(x20));  // ldr b19, [x20], #-60
  CMP_IO(dci(0x3c5c0e86), R(v6, x20), R(x20));   // ldr b6, [x20, #-64]!
  CMP_IO(dci(0x3d5d7078), R(v24), R(x3));        // ldr b24, [x3, #1884]
  CMP_IO(dci(0x3c7b998d),
         R(v13),
         R(x12, x27));  // ldr b13, [x12, x27, undefined #0]
  CMP_IO(dci(0x3c7c69c1), R(v1), R(x14, x28));   // ldr b1, [x14, x28]
  CMP_IO(dci(0xfc5c5765), R(v5, x27), R(x27));   // ldr d5, [x27], #-59
  CMP_IO(dci(0xfc5bfdb4), R(v20, x13), R(x13));  // ldr d20, [x13, #-65]!
  CMP_IO(dci(0xfd5d3202), R(v2), R(x16));        // ldr d2, [x16, #14944]
  CMP_IO(dci(0xfc7b48bb), R(v27), R(x5, x27));   // ldr d27, [x5, w27, uxtw]
  CMP_IO(dci(0x7c5c8509), R(v9, x8), R(x8));     // ldr h9, [x8], #-56
  CMP_IO(dci(0x7c5bdc10), R(v16, x0), R(x0));    // ldr h16, [x0, #-67]!
  CMP_IO(dci(0x7d5d085e), R(v30), R(x2));        // ldr h30, [x2, #3716]
  CMP_IO(dci(0x7c7b1b17),
         R(v23),
         R(x24, x27));                      // ldr h23, [x24, x27, undefined #1]
  CMP_IO(dci(0x85be098b), R(p11), R(x12));  // ldr p11, [x12, #-14, mul vl]
  CMP_IO(dci(0x3cdcd6ba), R(v26, x21), R(x21));  // ldr q26, [x21], #-51
  CMP_IO(dci(0x3cdb7e5f), R(v31, x18), R(x18));  // ldr q31, [x18, #-73]!
  CMP_IO(dci(0x3dfcaaad), R(v13), R(x21));       // ldr q13, [x21, #62112]
  CMP_IO(dci(0x3cfc2b58),
         R(v24),
         R(x26, x28));  // ldr q24, [x26, x28, undefined]
  CMP_IO(dci(0xbc5cf78c), R(v12, x28), R(x28));  // ldr s12, [x28], #-49
  CMP_IO(dci(0xbc5b5d8d), R(v13, x12), R(x12));  // ldr s13, [x12, #-75]!
  CMP_IO(dci(0xbd7cbf7f), R(v31), R(x27));       // ldr s31, [x27, #15548]
  CMP_IO(dci(0xbc7c0a86), R(v6), R(x20, x28));  // ldr s6, [x20, x28, undefined]
  CMP_IO(dci(0x85be4171), R(v17), R(x11));      // ldr z17, [x11, #-16, mul vl]
  CMP_IO(dci(0xf87ae70a), R(x10), R(x24));      // ldraa x10, [x24, #-656]
  CMP_IO(dci(0xf83d1ec7), R(x7, x22), R(x22));  // ldraa x7, [x22, #3720]!
  CMP_IO(dci(0xf8baf573), R(x19), R(x11));      // ldrab x19, [x11, #3448]
  CMP_IO(dci(0xf8fd0c5e), R(x30, x2), R(x2));   // ldrab x30, [x2, #-384]!
  CMP_IO(dci(0x385c44fc), R(x28, x7), R(x7));   // ldrb w28, [x7], #-60
  CMP_IO(dci(0x385c0c1d), R(x29, x0), R(x0));   // ldrb w29, [x0, #-64]!
  CMP_IO(dci(0x397d660f), R(x15), R(x16));      // ldrb w15, [x16, #3929]

  CLEANUP();
}

TEST(group_42) {
  SETUP();

  CMP_IO(dci(0x387ca9e8),
         R(x8),
         R(x15, x28));  // ldrb w8, [x15, x28, undefined]
  CMP_IO(dci(0x387a7bdc), R(x28), R(x30, x26));  // ldrb w28, [x30, x26, lsl #0]
  CMP_IO(dci(0x785c8772), R(x18, x27), R(x27));  // ldrh w18, [x27], #-56
  CMP_IO(dci(0x785bcda7), R(x7, x13), R(x13));   // ldrh w7, [x13, #-68]!
  CMP_IO(dci(0x797cfdf5), R(x21), R(x15));       // ldrh w21, [x15, #7806]
  CMP_IO(dci(0x787b2980),
         R(x0),
         R(x12, x27));  // ldrh w0, [x12, x27, undefined]
  CMP_IO(dci(0x38db8731), R(x17, x25), R(x25));  // ldrsb w17, [x25], #-72
  CMP_IO(dci(0x38dccde8), R(x8, x15), R(x15));   // ldrsb w8, [x15, #-52]!
  CMP_IO(dci(0x39db67f6), R(x22), R(sp));        // ldrsb w22, [sp, #1753]
  CMP_IO(dci(0x38fca81d),
         R(x29),
         R(x0, x28));  // ldrsb w29, [x0, x28, undefined]
  CMP_IO(dci(0x38fc7851), R(x17), R(x2, x28));   // ldrsb w17, [x2, x28, lsl #0]
  CMP_IO(dci(0x389ba66c), R(x12, x19), R(x19));  // ldrsb x12, [x19], #-70
  CMP_IO(dci(0x389caead), R(x13, x21), R(x21));  // ldrsb x13, [x21, #-54]!
  CMP_IO(dci(0x39bb725f), R(), R(x18));          // ldrsb xzr, [x18, #3804]
  CMP_IO(dci(0x38bc2b58),
         R(x24),
         R(x26, x28));  // ldrsb x24, [x26, x28, undefined]
  CMP_IO(dci(0x38bc7b8c),
         R(x12),
         R(x28, x28));  // ldrsb x12, [x28, x28, lsl #0]
  CMP_IO(dci(0x78dbe74b), R(x11, x26), R(x26));  // ldrsh w11, [x26], #-66
  CMP_IO(dci(0x78dc6dce), R(x14), R(x14));       // ldrsh w14, [x14, #-58]!
  CMP_IO(dci(0x79db2980), R(x0), R(x12));        // ldrsh w0, [x12, #3476]
  CMP_IO(dci(0x78fcf9f5),
         R(x21),
         R(x15, x28));                         // ldrsh w21, [x15, x28, sxtx #1]
  CMP_IO(dci(0x789be4e2), R(x2, x7), R(x7));   // ldrsh x2, [x7], #-66
  CMP_IO(dci(0x789c6c37), R(x23, x1), R(x1));  // ldrsh x23, [x1, #-58]!
  CMP_IO(dci(0x79bb0a45), R(x5), R(x18));      // ldrsh x5, [x18, #7556]
  CMP_IO(dci(0x78bd1930),
         R(x16),
         R(x9, x29));  // ldrsh x16, [x9, x29, undefined #1]
  CMP_IO(dci(0xb89b1717), R(x23, x24), R(x24));  // ldrsw x23, [x24], #-79
  CMP_IO(dci(0xb89d3e02), R(x2, x16), R(x16));   // ldrsw x2, [x16, #-45]!
  CMP_IO(dci(0xb99bf9b4), R(x20), R(x13));       // ldrsw x20, [x13, #7160]
  CMP_IO(dci(0xb8bc8909), R(x9), R(x8, x28));  // ldrsw x9, [x8, x28, undefined]
  CMP_IO(dci(0xb83d32fb), R(x27), R(x29, x23));  // ldset w29, w27, [x23]
  CMP_IO(dci(0xf83d3092), R(x18), R(x29, x4));   // ldset x29, x18, [x4]
  CMP_IO(dci(0xb8ba3204), R(x4), R(x26, x16));   // ldseta w26, w4, [x16]
  CMP_IO(dci(0xf8b932c9), R(x9), R(x25, x22));   // ldseta x25, x9, [x22]

  CLEANUP();
}

TEST(group_43) {
  SETUP();

  CMP_IO(dci(0x38bd3092), R(x18), R(x29, x4));   // ldsetab w29, w18, [x4]
  CMP_IO(dci(0x78bd321c), R(x28), R(x29, x16));  // ldsetah w29, w28, [x16]
  CMP_IO(dci(0xb8fd33c0), R(x0), R(x29, x30));   // ldsetal w29, w0, [x30]
  CMP_IO(dci(0xf8fd3085), R(x5), R(x29, x4));    // ldsetal x29, x5, [x4]
  CMP_IO(dci(0x38f9338e), R(x14), R(x25, x28));  // ldsetalb w25, w14, [x28]
  CMP_IO(dci(0x78fa33a8), R(x8), R(x26, x29));   // ldsetalh w26, w8, [x29]
  CMP_IO(dci(0x383932c9), R(x9), R(x25, x22));   // ldsetb w25, w9, [x22]
  CMP_IO(dci(0x78393053), R(x19), R(x25, x2));   // ldseth w25, w19, [x2]
  CMP_IO(dci(0xb87931f7), R(x23), R(x25, x15));  // ldsetl w25, w23, [x15]
  CMP_IO(dci(0xf879338e), R(x14), R(x25, x28));  // ldsetl x25, x14, [x28]
  CMP_IO(dci(0x387d3085), R(x5), R(x29, x4));    // ldsetlb w29, w5, [x4]
  CMP_IO(dci(0x787e309f), R(), R(x30, x4));      // stsetlh w30, [x4]
  CMP_IO(dci(0xb83a43a8), R(x8), R(x26, x29));   // ldsmax w26, w8, [x29]
  CMP_IO(dci(0xf83a406d), R(x13), R(x26, x3));   // ldsmax x26, x13, [x3]
  CMP_IO(dci(0xb8be409f), R(), R(x30, x4));      // stsmaxa w30, [x4]
  CMP_IO(dci(0xf8be4236), R(x22), R(x30, x17));  // ldsmaxa x30, x22, [x17]
  CMP_IO(dci(0x38ba406d), R(x13), R(x26, x3));   // ldsmaxab w26, w13, [x3]
  CMP_IO(dci(0x78b941f7), R(x23), R(x25, x15));  // ldsmaxah w25, w23, [x15]
  CMP_IO(dci(0xb8f94053), R(x19), R(x25, x2));   // ldsmaxal w25, w19, [x2]
  CMP_IO(dci(0xf8f941ea), R(x10), R(x25, x15));  // ldsmaxal x25, x10, [x15]
  CMP_IO(dci(0x38fd42e1), R(x1), R(x29, x23));   // ldsmaxalb w29, w1, [x23]
  CMP_IO(dci(0x78fd42fb), R(x27), R(x29, x23));  // ldsmaxalh w29, w27, [x23]
  CMP_IO(dci(0x383e4236), R(x22), R(x30, x17));  // ldsmaxb w30, w22, [x17]
  CMP_IO(dci(0x783d43c0), R(x0), R(x29, x30));   // ldsmaxh w29, w0, [x30]
  CMP_IO(dci(0xb87d421c), R(x28), R(x29, x16));  // ldsmaxl w29, w28, [x16]
  CMP_IO(dci(0xf87d42e1), R(x1), R(x29, x23));   // ldsmaxl x29, x1, [x23]
  CMP_IO(dci(0x387941ea), R(x10), R(x25, x15));  // ldsmaxlb w25, w10, [x15]
  CMP_IO(dci(0x787a4204), R(x4), R(x26, x16));   // ldsmaxlh w26, w4, [x16]
  CMP_IO(dci(0xb83a507a), R(x26), R(x26, x3));   // ldsmin w26, w26, [x3]
  CMP_IO(dci(0xf83a52e3), R(x3), R(x26, x23));   // ldsmin x26, x3, [x23]
  CMP_IO(dci(0xb8be5171), R(x17), R(x30, x11));  // ldsmina w30, w17, [x11]
  CMP_IO(dci(0xf8be50ac), R(x12), R(x30, x5));   // ldsmina x30, x12, [x5]

  CLEANUP();
}

TEST(group_44) {
  SETUP();

  CMP_IO(dci(0x38ba52e3), R(x3), R(x26, x23));   // ldsminab w26, w3, [x23]
  CMP_IO(dci(0x78ba5159), R(x25), R(x26, x10));  // ldsminah w26, w25, [x10]
  CMP_IO(dci(0xb8fa52fd), R(x29), R(x26, x23));  // ldsminal w26, w29, [x23]
  CMP_IO(dci(0xf8fa5238), R(x24), R(x26, x17));  // ldsminal x26, x24, [x17]
  CMP_IO(dci(0x38fe532f), R(x15), R(x30, x25));  // ldsminalb w30, w15, [x25]
  CMP_IO(dci(0x78fe5315), R(x21), R(x30, x24));  // ldsminalh w30, w21, [x24]
  CMP_IO(dci(0x383e50ac), R(x12), R(x30, x5));   // ldsminb w30, w12, [x5]
  CMP_IO(dci(0x783e5322), R(x2), R(x30, x25));   // ldsminh w30, w2, [x25]
  CMP_IO(dci(0xb87e50c6), R(x6), R(x30, x6));    // ldsminl w30, w6, [x6]
  CMP_IO(dci(0xf87e532f), R(x15), R(x30, x25));  // ldsminl x30, x15, [x25]
  CMP_IO(dci(0x387a5238), R(x24), R(x26, x17));  // ldsminlb w26, w24, [x17]
  CMP_IO(dci(0x787a521e), R(x30), R(x26, x16));  // ldsminlh w26, w30, [x16]
  CMP_IO(dci(0xb8396132), R(x18), R(x25, x9));   // ldumax w25, w18, [x9]
  CMP_IO(dci(0xf839639b), R(x27), R(x25, x28));  // ldumax x25, x27, [x28]
  CMP_IO(dci(0xb8bd6229), R(x9), R(x29, x17));   // ldumaxa w29, w9, [x17]
  CMP_IO(dci(0xf8bd6164), R(x4), R(x29, x11));   // ldumaxa x29, x4, [x11]
  CMP_IO(dci(0x38b9639b), R(x27), R(x25, x28));  // ldumaxab w25, w27, [x28]
  CMP_IO(dci(0x78b96381), R(x1), R(x25, x28));   // ldumaxah w25, w1, [x28]
  CMP_IO(dci(0xb8f96125), R(x5), R(x25, x9));    // ldumaxal w25, w5, [x9]
  CMP_IO(dci(0xf8f96060), R(x0), R(x25, x3));    // ldumaxal x25, x0, [x3]
  CMP_IO(dci(0x38fd6157), R(x23), R(x29, x10));  // ldumaxalb w29, w23, [x10]
  CMP_IO(dci(0x78fd63cd), R(x13), R(x29, x30));  // ldumaxalh w29, w13, [x30]
  CMP_IO(dci(0x383d6164), R(x4), R(x29, x11));   // ldumaxb w29, w4, [x11]
  CMP_IO(dci(0x783d614a), R(x10), R(x29, x10));  // ldumaxh w29, w10, [x10]
  CMP_IO(dci(0xb87d62ee), R(x14), R(x29, x23));  // ldumaxl w29, w14, [x23]
  CMP_IO(dci(0xf87d6157), R(x23), R(x29, x10));  // ldumaxl x29, x23, [x10]
  CMP_IO(dci(0x38796060), R(x0), R(x25, x3));    // ldumaxlb w25, w0, [x3]
  CMP_IO(dci(0x787a62d6), R(x22), R(x26, x22));  // ldumaxlh w26, w22, [x22]
  CMP_IO(dci(0xb83a72f0), R(x16), R(x26, x23));  // ldumin w26, w16, [x23]
  CMP_IO(dci(0xf83a73b5), R(x21), R(x26, x29));  // ldumin x26, x21, [x29]
  CMP_IO(dci(0xb8be73e7), R(x7), R(x30, sp));    // ldumina w30, w7, [sp]
  CMP_IO(dci(0xf8be717e), R(x30), R(x30, x11));  // ldumina x30, x30, [x11]

  CLEANUP();
}

TEST(group_45) {
  SETUP();

  CMP_IO(dci(0x38ba73b5), R(x21), R(x26, x29));  // lduminab w26, w21, [x29]
  CMP_IO(dci(0x78ba73cf), R(x15), R(x26, x30));  // lduminah w26, w15, [x30]
  CMP_IO(dci(0xb8fa722b), R(x11), R(x26, x17));  // lduminal w26, w11, [x17]
  CMP_IO(dci(0xf8fa73c2), R(x2), R(x26, x30));   // lduminal x26, x2, [x30]
  CMP_IO(dci(0x38fe70b9), R(x25), R(x30, x5));   // lduminalb w30, w25, [x5]
  CMP_IO(dci(0x78fe7243), R(x3), R(x30, x18));   // lduminalh w30, w3, [x18]
  CMP_IO(dci(0x383e717e), R(x30), R(x30, x11));  // lduminb w30, w30, [x11]
  CMP_IO(dci(0x783e7198), R(x24), R(x30, x12));  // lduminh w30, w24, [x12]
  CMP_IO(dci(0xb87e73f4), R(x20), R(x30, sp));   // lduminl w30, w20, [sp]
  CMP_IO(dci(0xf87e70b9), R(x25), R(x30, x5));   // lduminl x30, x25, [x5]
  CMP_IO(dci(0x387a73c2), R(x2), R(x26, x30));   // lduminlb w26, w2, [x30]
  CMP_IO(dci(0x787a714c), R(x12), R(x26, x10));  // lduminlh w26, w12, [x10]
  CMP_IO(dci(0xb85af173), R(x19), R(x11));       // ldur w19, [x11, #-81]
  CMP_IO(dci(0xf85ae30a), R(x10), R(x24));       // ldur x10, [x24, #-82]
  CMP_IO(dci(0x3c5c302a), R(v10), R(x1));        // ldur b10, [x1, #-61]
  CMP_IO(dci(0xfc5c72a0), R(v0), R(x21));        // ldur d0, [x21, #-57]
  CMP_IO(dci(0x7c5ca044), R(v4), R(x2));         // ldur h4, [x2, #-54]
  CMP_IO(dci(0x3cdce123), R(v3), R(x9));         // ldur q3, [x9, #-50]
  CMP_IO(dci(0xbc5cd051), R(v17), R(x2));        // ldur s17, [x2, #-51]
  CMP_IO(dci(0x385c21c1), R(x1), R(x14));        // ldurb w1, [x14, #-62]
  CMP_IO(dci(0x785c91db), R(x27), R(x14));       // ldurh w27, [x14, #-55]
  CMP_IO(dci(0x38dba26c), R(x12), R(x19));       // ldursb w12, [x19, #-70]
  CMP_IO(dci(0x389b8331), R(x17), R(x25));       // ldursb x17, [x25, #-72]
  CMP_IO(dci(0x78dbe0e2), R(x2), R(x7));         // ldursh w2, [x7, #-66]
  CMP_IO(dci(0x789be34b), R(x11), R(x26));       // ldursh x11, [x26, #-66]
  CMP_IO(dci(0xb89b10ae), R(x14), R(x5));        // ldursw x14, [x5, #-79]
  CMP_IO(dci(0x887e13f4), R(x20, x4), R(sp));    // ldxp w20, w4, [sp]
  CMP_IO(dci(0xc87e74b9), R(x25, x29), R(x5));   // ldxp x25, x29, [x5]
  CMP_IO(dci(0x885e13f4), R(x20), R(sp));        // ldxr w20, [sp]
  CMP_IO(dci(0xc85e74b9), R(x25), R(x5));        // ldxr x25, [x5]
  CMP_IO(dci(0x085a17c2), R(x2), R(x30));        // ldxrb w2, [x30]
  CMP_IO(dci(0x485a594c), R(x12), R(x10));       // ldxrh w12, [x10]

  CLEANUP();
}

TEST(group_46) {
  SETUP();

  CMP_IO(dci(0x04039f7f), R(v31), R(p7, v31));    // lsl z31.h, p7/m, z31.h, #11
  CMP_IO(dci(0x041b88a5), R(v5), R(p2, v5, v5));  // lsl z5.b, p2/m, z5.b, z5.d
  CMP_IO(dci(0x04d39ba6),
         R(v6),
         R(p6, v6, v29));                         // lsl z6.d, p6/m, z6.d, z29.d
  CMP_IO(dci(0x047d9d4a), R(v10), R(v10));        // lsl z10.s, z10.s, #29
  CMP_IO(dci(0x04be8d98), R(v24), R(v12, v30));   // lsl z24.s, z12.s, z30.d
  CMP_IO(dci(0x04979cc8), R(v8), R(p7, v8, v6));  // lslr z8.s, p7/m, z8.s, z6.s
  CMP_IO(dci(0x1add2085), R(x5), R(x4, x29));     // lsl w5, w4, w29
  CMP_IO(dci(0x9add23c0), R(x0), R(x30, x29));    // lsl x0, x30, x29
  CMP_IO(dci(0x04c19451), R(v17), R(p5, v17));    // lsr z17.d, p5/m, z17.d, #30
  CMP_IO(dci(0x04199f7f),
         R(v31),
         R(p7, v31, v27));  // lsr z31.b, p7/m, z31.b, z27.d
  CMP_IO(dci(0x04118cfc),
         R(v28),
         R(p3, v28, v7));                        // lsr z28.b, p3/m, z28.b, z7.b
  CMP_IO(dci(0x04be9598), R(v24), R(v12));       // lsr z24.d, z12.d, #34
  CMP_IO(dci(0x047d854a), R(v10), R(v10, v29));  // lsr z10.h, z10.h, z29.d
  CMP_IO(dci(0x0455919a),
         R(v26),
         R(p4, v26, v12));  // lsrr z26.h, p4/m, z26.h, z12.h
  CMP_IO(dci(0x1ade258b), R(x11), R(x12, x30));  // lsr w11, w12, w30
  CMP_IO(dci(0x9ade2722), R(x2), R(x25, x30));   // lsr x2, x25, x30
  CMP_IO(dci(0x045cc41d),
         R(v29),
         R(v29, p1, v28, v0));  // mad z29.h, p1/m, z28.h, z0.h
  CMP_IO(dci(0x1b1a1ad6), R(x22), R(x22, x26, x6));  // madd w22, w22, w26, w6
  CMP_IO(dci(0x9b1a253f), R(), R(x9, x26, x9));      // madd xzr, x9, x26, x9
  CMP_IO(dci(0x45748988),
         R(p8),
         R(p2, v12, v20));  // match p8.h, p2/z, z12.h, z20.h
  CMP_IO(dci(0x2fba022b),
         R(v11),
         R(v11, v17, v26));  // mla v11.2s, v17.2s, v26.s[1]
  CMP_IO(dci(0x4ebd97c0), R(v0), R(v0, v30, v29));  // mla v0.4s, v30.4s, v29.4s
  CMP_IO(dci(0x045b50d5),
         R(v21),
         R(v21, p4, v6, v27));  // mla z21.h, p4/m, z6.h, z27.h
  CMP_IO(dci(0x44fc0b72),
         R(v18),
         R(v18, v27, v12));                     // mla z18.d, z27.d, z12.d[1]
  CMP_IO(dci(0x447c09ce), R(v14), R(v14, v4));  // mla z14.h, z14.h, z4.h[7]
  CMP_IO(dci(0x44bd0a0f),
         R(v15),
         R(v15, v16, v5));  // mla z15.s, z16.s, z5.s[3]
  CMP_IO(dci(0x6f984b89),
         R(v9),
         R(v9, v28, v24));  // mls v9.4s, v28.4s, v24.s[2]
  CMP_IO(dci(0x6eb796ce),
         R(v14),
         R(v14, v22, v23));  // mls v14.4s, v22.4s, v23.4s
  CMP_IO(dci(0x041b7f17),
         R(v23),
         R(v23, p7, v24, v27));  // mls z23.b, p7/m, z24.b, z27.b
  CMP_IO(dci(0x44fd0ed4),
         R(v20),
         R(v20, v22, v13));  // mls z20.d, z22.d, z13.d[1]
  CMP_IO(dci(0x443d0d30), R(v16), R(v16, v9, v5));  // mls z16.h, z9.h, z5.h[3]
  CMP_IO(dci(0x44bc0ead),
         R(v13),
         R(v13, v21, v4));  // mls z13.s, z21.s, z4.s[3]

  CLEANUP();
}

TEST(group_47) {
  SETUP();

  CMP_IO(dci(0x6f02e611), R(v17), R());        // movi v17.2d, #0xff00ff00000000
  CMP_IO(dci(0x2f04e6ba), R(v26), R());        // movi d26, #0xff0000ff00ff00ff
  CMP_IO(dci(0x4f0485db), R(v27), R());        // movi v27.8h, #0x8e, lsl #0
  CMP_IO(dci(0x4f0446ba), R(v26), R());        // movi v26.4s, #0x95, lsl #16
  CMP_IO(dci(0x4f04d6ba), R(v26), R());        // movi v26.4s, #0x95, msl #16
  CMP_IO(dci(0x0f03e65f), R(v31), R());        // movi v31.8b, #0x72
  CMP_IO(dci(0x729ded64), R(x4), R(x4));       // movk w4, #0xef6b
  CMP_IO(dci(0xf2fdce29), R(x9), R(x9));       // movk x9, #0xee71, lsl #48
  CMP_IO(dci(0x12be0c9f), R(), R());           // mov wzr, #0xf9bffff
  CMP_IO(dci(0x92de0236), R(x22), R());        // mov x22, #0xffff0feeffffffff
  CMP_IO(dci(0x0450241d), R(v29), R(p1, v0));  // movprfx z29.h, p1/z, z0.h
  CMP_IO(dci(0x0420bd4c), R(v12), R(v10));     // movprfx z12, z10
  CMP_IO(dci(0x52be898b), R(x11), R());        // mov w11, #0xf44c0000
  CMP_IO(dci(0xd2de7f22), R(x2), R());         // mov x2, #0xf3f900000000
  CMP_IO(dci(0x04dbf3e9),
         R(v9),
         R(v9, p4, v27, v31));  // msb z9.d, p4/m, z27.d, z31.d
  CMP_IO(dci(0x1b19e6c9), R(x9), R(x22, x25, x25));  // msub w9, w22, w25, w25
  CMP_IO(dci(0x9b1a8604), R(x4), R(x16, x26, x1));   // msub x4, x16, x26, x1
  CMP_IO(dci(0x4f4281fb), R(v27), R(v15, v2));  // mul v27.8h, v15.8h, v2.h[0]
  CMP_IO(dci(0x4e3e9cac), R(v12), R(v5, v30));  // mul v12.16b, v5.16b, v30.16b
  CMP_IO(dci(0x04901c6b),
         R(v11),
         R(v11, p7, v11, v3));                   // mul z11.s, p7/m, z11.s, z3.s
  CMP_IO(dci(0x2530d957), R(v23), R(v23));       // mul z23.b, z23.b, #-54
  CMP_IO(dci(0x04fe617e), R(v30), R(v11, v30));  // mul z30.d, z11.d, z30.d
  CMP_IO(dci(0x44fbf980), R(v0), R(v12, v11));   // mul z0.d, z12.d, z11.d[1]
  CMP_IO(dci(0x443bfb24), R(v4), R(v25, v3));    // mul z4.h, z25.h, z3.h[3]
  CMP_IO(dci(0x44bbfb31), R(v17), R(v25, v3));   // mul z17.s, z25.s, z3.s[3]
  CMP_IO(dci(0x2f04a444), R(v4), R());           // mvni v4.4h, #0x82, lsl #8
  CMP_IO(dci(0x2f044451), R(v17), R());          // mvni v17.2s, #0x82, lsl #16
  CMP_IO(dci(0x2f04d451), R(v17), R());          // mvni v17.2s, #0x82, msl #16
  CMP_IO(dci(0x25897f35),
         R(p5),
         R(p15, p9, p9));  // nand p5.b, p15/z, p9.b, p9.b
  CMP_IO(dci(0x25ce6a50),
         R(p0),
         R(p10, p2, p14));  // nands p0.b, p10/z, p2.b, p14.b
  CMP_IO(dci(0x04fd3e29),
         R(v9),
         R(v9, v29, v17));                  // nbsl z9.d, z9.d, z29.d, z17.d
  CMP_IO(dci(0x6e20babc), R(v28), R(v21));  // neg v28.16b, v21.16b

  CLEANUP();
}

TEST(group_48) {
  SETUP();

  CMP_IO(dci(0x7ee0b966), R(v6), R(v11));            // neg d6, d11
  CMP_IO(dci(0x04d7ba79), R(v25), R(v25, p6, v19));  // neg z25.d, p6/m, z19.d
  CMP_IO(dci(0x457b84f2),
         R(p2),
         R(p1, v7, v27));             // nmatch p2.h, p1/z, z7.h, z27.h
  CMP_IO(dci(0xd503201f), R(), R());  // nop
  CMP_IO(dci(0x258e7643),
         R(p3),
         R(p13, p2, p14));  // nor p3.b, p13/z, p2.b, p14.b
  CMP_IO(dci(0x25c95fea),
         R(p10),
         R(p7, p15, p9));                   // nors p10.b, p7/z, p15.b, p9.b
  CMP_IO(dci(0x2e205a2b), R(v11), R(v17));  // mvn v11.8b, v17.8b
  CMP_IO(dci(0x045eab0a), R(v10), R(v10, p2, v24));  // not z10.h, p2/m, z24.h
  CMP_IO(dci(0x2abc6923), R(x3), R(x9, x28));        // orn w3, w9, w28, asr #26
  CMP_IO(dci(0xaa7cdeba), R(x26), R(x21, x28));  // orn x26, x21, x28, lsr #55
  CMP_IO(dci(0x4efe1f15),
         R(v21),
         R(v24, v30));  // orn v21.16b, v24.16b, v30.16b
  CMP_IO(dci(0x258e7453),
         R(p3),
         R(p13, p2, p14));  // orn p3.b, p13/z, p2.b, p14.b
  CMP_IO(dci(0x25c95dfa),
         R(p10),
         R(p7, p15, p9));                       // orns p10.b, p7/z, p15.b, p9.b
  CMP_IO(dci(0x321c76a0), R(x0), R(x21));       // orr w0, w21, #0xfffffff3
  CMP_IO(dci(0x2a1c3f7f), R(), R(x27, x28));    // orr wzr, w27, w28, lsl #15
  CMP_IO(dci(0xb27c5765), R(x5), R(x27));       // orr x5, x27, #0x3fffff0
  CMP_IO(dci(0xaadcb516), R(x22), R(x8, x28));  // orr x22, x8, x28, ror #45
  CMP_IO(dci(0x0f039679), R(v25), R(v25));      // orr v25.4h, #0x73, lsl #0
  CMP_IO(dci(0x4f0277dc), R(v28), R(v28));      // orr v28.4s, #0x5e, lsl #24
  CMP_IO(dci(0x0ebd1e29), R(v9), R(v17, v29));  // orr v9.8b, v17.8b, v29.8b
  CMP_IO(dci(0x258e4c8f),
         R(p15),
         R(p3, p4, p14));  // orr p15.b, p3/z, p4.b, p14.b
  CMP_IO(dci(0x0458182a),
         R(v10),
         R(p6, v10, v1));                 // orr z10.h, p6/m, z10.h, z1.h
  CMP_IO(dci(0x0502a8c6), R(v6), R(v6));  // orr z6.d, z6.d, #0x3f80000000000
  CMP_IO(dci(0x047e309f), R(v31), R(v4, v30));  // orr z31.d, z4.d, z30.d
  CMP_IO(dci(0x25ca4166),
         R(p6),
         R(p0, p11, p10));                     // orrs p6.b, p0/z, p11.b, p10.b
  CMP_IO(dci(0x04983010), R(v16), R(p4, v0));  // orv s16, p4, z0.s
  CMP_IO(dci(0xdac10957), R(x23), R(x23, x10));  // pacda x23, x10
  CMP_IO(dci(0xdac10eee), R(x14), R(x14, x23));  // pacdb x14, x23
  CMP_IO(dci(0xdac12be7), R(x7), R(x7));         // pacdza x7
  CMP_IO(dci(0xdac12ffe), R(x30), R(x30));       // pacdzb x30
  CMP_IO(dci(0x9add3157), R(x23), R(x10, x29));  // pacga x23, x10, x29
  CMP_IO(dci(0xd503211f), R(x17), R(x17, x16));  // pacia1716

  CLEANUP();
}

TEST(group_49) {
  SETUP();

  CMP_IO(dci(0xdac10164), R(x4), R(x4, x11));    // pacia x4, x11
  CMP_IO(dci(0xd503233f), R(x30), R(x30, sp));   // paciasp
  CMP_IO(dci(0xd503231f), R(x30), R(x30));       // paciaz
  CMP_IO(dci(0xd503215f), R(x17), R(x17, x16));  // pacib1716
  CMP_IO(dci(0xdac10629), R(x9), R(x9, x17));    // pacib x9, x17
  CMP_IO(dci(0xd503237f), R(x30), R(x30, sp));   // pacibsp
  CMP_IO(dci(0xd503235f), R(x30), R(x30));       // pacibz
  CMP_IO(dci(0xdac123f4), R(x20), R(x20));       // paciza x20
  CMP_IO(dci(0xdac127f9), R(x25), R(x25));       // pacizb x25
  CMP_IO(dci(0x2518e402), R(p2), R());           // pfalse p2.b
  CMP_IO(dci(0x2558c00f), R(p15), R(p0, p15));   // pfirst p15.b, p0, p15.b
  CMP_IO(dci(0x6e399ebc),
         R(v28),
         R(v21, v25));  // pmul v28.16b, v21.16b, v25.16b
  CMP_IO(dci(0x0439678e), R(v14), R(v28, v25));  // pmul z14.b, z28.b, z25.b
  CMP_IO(dci(0x4e2ae3e5),
         R(v5),
         R(v31, v10));  // pmull2 v5.8h, v31.16b, v10.16b
  CMP_IO(dci(0x459e68dd), R(v29), R(v6, v30));   // pmullb z29.s, z6.h, z30.h
  CMP_IO(dci(0x45da6d66), R(v6), R(v11, v26));   // pmullt z6.d, z11.s, z26.s
  CMP_IO(dci(0x2519c5cc), R(p12), R(p14, p12));  // pnext p12.b, p14, p12.b
  CMP_IO(dci(0xc419e6c9), R(), R(p1, v22));  // prfb pstl1strm, p1, [z22.d, #25]
  CMP_IO(dci(0x841af628), R(), R(p5, v17));  // prfb pstl1keep, p5, [z17.s, #26]
  CMP_IO(dci(0x85fa16ed),
         R(),
         R(p5, x23));  // prfb pstl3strm, p5, [x23, #-6, mul vl]
  CMP_IO(dci(0x8419dc60),
         R(),
         R(p7, x3, x25));  // prfb pldl1keep, p7, [x3, x25]
  CMP_IO(dci(0xc47991e7),
         R(),
         R(p4, x15, v25));  // prfb #0b0111, p4, [x15, z25.d]
  CMP_IO(dci(0xc47e08c6),
         R(),
         R(p2, x6, v30));  // prfb #0b0110, p2, [x6, z30.d, sxtw ]
  CMP_IO(dci(0x843d02eb),
         R(),
         R(p0, x23, v29));  // prfb pstl2strm, p0, [x23, z29.s, uxtw]
  CMP_IO(dci(0xc59ae52f), R(), R(p1, v9));   // prfd #0b1111, p1, [z9.d, #26]
  CMP_IO(dci(0x859af7c2), R(), R(p5, v30));  // prfd pldl2keep, p5, [z30.s, #26]
  CMP_IO(dci(0x85fa622b),
         R(),
         R(p0, x17));  // prfd pstl2strm, p0, [x17, #-6, mul vl]
  CMP_IO(dci(0x8599ddea),
         R(),
         R(p7, x15, x25));  // prfd pstl2keep, p7, [x15, x25, lsl #3]
  CMP_IO(dci(0xc479f381),
         R(),
         R(p4, x28, v25));  // prfd pldl1strm, p4, [x28, z25.d, lsl #3]
  CMP_IO(dci(0xc43e73e4),
         R(),
         R(p4, sp, v30));  // prfd pldl3keep, p4, [sp, z30.d, uxtw #3]
  CMP_IO(dci(0x847d77cd),
         R(),
         R(p5, x30, v29));  // prfd pstl3strm, p5, [x30, z29.s, sxtw #3]
  CMP_IO(dci(0xc499e843), R(), R(p2, v2));  // prfh pldl2strm, p2, [z2.d, #25]

  CLEANUP();
}

TEST(group_50) {
  SETUP();

  CMP_IO(dci(0x849aee0e), R(), R(p3, v16));  // prfh #0b1110, p3, [z16.s, #26]
  CMP_IO(dci(0x85fa3887),
         R(),
         R(p6, x4));  // prfh #0b0111, p6, [x4, #-6, mul vl]
  CMP_IO(dci(0x849adac6),
         R(),
         R(p6, x22, x26));  // prfh #0b0110, p6, [x22, x26, lsl #1]
  CMP_IO(dci(0xc47ab06d),
         R(),
         R(p4, x3, v26));  // prfh pstl3strm, p4, [x3, z26.d, lsl #1]
  CMP_IO(dci(0xc43e2a40),
         R(),
         R(p2, x18, v30));  // prfh pldl1keep, p2, [x18, z30.d, uxtw #1]
  CMP_IO(dci(0x847d3ae1),
         R(),
         R(p6, x23, v29));  // prfh pldl1strm, p6, [x23, z29.s, sxtw #1]
  CMP_IO(dci(0xf9bc18ef), R(), R(x7));  // prfm #0b01111, [x7, #30768]
  CMP_IO(dci(0xf8bcb916),
         R(),
         R(x8, x28));  // prfm #0b10110, [x8, x28, undefined #3]
  CMP_IO(dci(0xf89ae0a1), R(), R(x5));       // prfum pldl1strm, [x5, #-82]
  CMP_IO(dci(0xc51ae166), R(), R(p0, v11));  // prfw #0b0110, p0, [z11.d, #26]
  CMP_IO(dci(0x8519fb8b), R(), R(p6, v28));  // prfw pstl2strm, p6, [z28.s, #25]
  CMP_IO(dci(0x85d95122),
         R(),
         R(p4, x9));  // prfw pldl2keep, p4, [x9, #25, mul vl]
  CMP_IO(dci(0x851acee3),
         R(),
         R(p3, x23, x26));  // prfw pldl2strm, p3, [x23, x26, lsl #2]
  CMP_IO(dci(0xc47ad94c),
         R(),
         R(p6, x10, v26));  // prfw pstl3keep, p6, [x10, z26.d]
  CMP_IO(dci(0xc47d4e29),
         R(),
         R(p3, x17, v29));  // prfw pstl1strm, p3, [x17, z29.d, sxtw ]
  CMP_IO(dci(0x843e5d88),
         R(),
         R(p7, x12, v30));  // prfw pstl1keep, p7, [x12, z30.s, uxtw #2]
  CMP_IO(dci(0x2550c160), R(), R(p0, p11));  // ptest p0, p11.b
  CMP_IO(dci(0x2518e3cf), R(p15), R());      // ptrue p15.b, mul3
  CMP_IO(dci(0x25d9e226), R(p6), R());       // ptrues p6.d, #0x11
  CMP_IO(dci(0x0531418b), R(p11), R(p12));   // punpkhi p11.h, p12.b
  CMP_IO(dci(0x053040c9), R(p9), R(p6));     // punpklo p9.h, p6.b
  CMP_IO(dci(0x2ebe4198),
         R(v24),
         R(v12, v30));                         // raddhn v24.2s, v12.2d, v30.2d
  CMP_IO(dci(0x45a26b83), R(v3), R(v28, v2));  // raddhnb z3.h, z28.s, z2.s
  CMP_IO(dci(0x45fd6ee1),
         R(v1),
         R(v1, v23, v29));                         // raddhnt z1.s, z23.d, z29.d
  CMP_IO(dci(0x453df6d4), R(v20), R(v22, v29));    // rax1 z20.d, z22.d, z29.d
  CMP_IO(dci(0x5ac00308), R(x8), R(x24));          // rbit w8, w24
  CMP_IO(dci(0xdac003cd), R(x13), R(x30));         // rbit x13, x30
  CMP_IO(dci(0x6e605bb3), R(v19), R(v29));         // rbit v19.16b, v29.16b
  CMP_IO(dci(0x05679202), R(v2), R(v2, p4, v16));  // rbit z2.h, p4/m, z16.h
  CMP_IO(dci(0x2519f002), R(p2), R());             // rdffr p2.b
  CMP_IO(dci(0x2518f107), R(p7), R(p8));           // rdffr p7.b, p8/z
  CMP_IO(dci(0x2558f04e), R(p14), R(p2));          // rdffrs p14.b, p2/z

  CLEANUP();
}

TEST(group_51) {
  SETUP();

  CMP_IO(dci(0x04bf57cd), R(x13), R());              // rdvl x13, #-2
  CMP_IO(dci(0xd65f0380), R(), R(x28));              // ret x28
  CMP_IO(dci(0xd65f0bff), R(), R(sp, x30));          // retaa
  CMP_IO(dci(0xd65f0fff), R(), R(sp, x30));          // retab
  CMP_IO(dci(0x5ac00525), R(x5), R(x9));             // rev16 w5, w9
  CMP_IO(dci(0xdac00460), R(x0), R(x3));             // rev16 x0, x3
  CMP_IO(dci(0x0e201a72), R(v18), R(v19));           // rev16 v18.8b, v19.8b
  CMP_IO(dci(0xdac009ea), R(x10), R(x15));           // rev32 x10, x15
  CMP_IO(dci(0x2e600ab0), R(v16), R(v21));           // rev32 v16.4h, v21.4h
  CMP_IO(dci(0x0e60086d), R(v13), R(v3));            // rev64 v13.4h, v3.4h
  CMP_IO(dci(0x5ac00abc), R(x28), R(x21));           // rev w28, w21
  CMP_IO(dci(0xdac00f81), R(x1), R(x28));            // rev x1, x28
  CMP_IO(dci(0x05b4402b), R(p11), R(p1));            // rev p11.s, p1.s
  CMP_IO(dci(0x05b83a2b), R(v11), R(v17));           // rev z11.s, z17.s
  CMP_IO(dci(0x05e494b9), R(v25), R(v25, p5, v5));   // revb z25.d, p5/m, z5.d
  CMP_IO(dci(0x05a59643), R(v3), R(v3, p5, v18));    // revh z3.s, p5/m, z18.s
  CMP_IO(dci(0x05e68236), R(v22), R(v22, p0, v17));  // revw z22.d, p0/m, z17.d
  CMP_IO(dci(0xba1e0708), R(), R(x24));              // rmif x24, #60, #Nzcv
  CMP_IO(dci(0x1ade2cb9), R(x25), R(x5, x30));       // ror w25, w5, w30
  CMP_IO(dci(0x9ade2ff4), R(x20), R(x30));           // ror x20, xzr, x30
  CMP_IO(dci(0x0f158faa), R(v10), R(v29));       // rshrn v10.4h, v29.4s, #11
  CMP_IO(dci(0x45791b8e), R(v14), R(v28));       // rshrnb z14.s, z28.d, #7
  CMP_IO(dci(0x453a1d4c), R(v12), R(v12, v10));  // rshrnt z12.h, z10.s, #6
  CMP_IO(dci(0x2ebe618b),
         R(v11),
         R(v12, v30));                         // rsubhn v11.2s, v12.2d, v30.2d
  CMP_IO(dci(0x45a27964), R(v4), R(v11, v2));  // rsubhnb z4.h, z11.s, z2.s
  CMP_IO(dci(0x45fd7eee),
         R(v14),
         R(v14, v23, v29));  // rsubhnt z14.s, z23.d, z29.d
  CMP_IO(dci(0x4e3a7c87),
         R(v7),
         R(v7, v4, v26));  // saba v7.16b, v4.16b, v26.16b
  CMP_IO(dci(0x459dfafb),
         R(v27),
         R(v27, v23, v29));  // saba z27.s, z23.s, z29.s
  CMP_IO(dci(0x0ead52ee),
         R(v14),
         R(v14, v23, v13));  // sabal v14.2d, v23.2s, v13.2s
  CMP_IO(dci(0x455dc085), R(v5), R(v5, v4, v29));   // sabalb z5.h, z4.b, z29.b
  CMP_IO(dci(0x459ec643), R(v3), R(v3, v18, v30));  // sabalt z3.s, z18.h, z30.h
  CMP_IO(dci(0x0e3a754c), R(v12), R(v10, v26));  // sabd v12.8b, v10.8b, v26.8b

  CLEANUP();
}

TEST(group_52) {
  SETUP();

  CMP_IO(dci(0x048c0f4b),
         R(v11),
         R(p3, v11, v26));  // sabd z11.s, p3/m, z11.s, z26.s
  CMP_IO(dci(0x4e787365), R(v5), R(v27, v24));   // sabdl2 v5.4s, v27.8h, v24.8h
  CMP_IO(dci(0x45ce332c), R(v12), R(v25, v14));  // sabdlb z12.d, z25.s, z14.s
  CMP_IO(dci(0x45da36d6), R(v22), R(v22, v26));  // sabdlt z22.d, z22.s, z26.s
  CMP_IO(dci(0x0e206957), R(v23), R(v23, v10));  // sadalp v23.4h, v10.8b
  CMP_IO(dci(0x4484b078), R(v24), R(v24, p4, v3));  // sadalp z24.s, p4/m, z3.h
  CMP_IO(dci(0x4e3a0087), R(v7), R(v4, v26));   // saddl2 v7.8h, v4.16b, v26.16b
  CMP_IO(dci(0x45da0166), R(v6), R(v11, v26));  // saddlb z6.d, z11.s, z26.s
  CMP_IO(dci(0x455d8092), R(v18), R(v4, v29));  // saddlbt z18.h, z4.b, z29.b
  CMP_IO(dci(0x0e202a1c), R(v28), R(v16));      // saddlp v28.4h, v16.8b
  CMP_IO(dci(0x459e04dd), R(v29), R(v6, v30));  // saddlt z29.s, z6.h, z30.h
  CMP_IO(dci(0x4e703b15), R(v21), R(v24));      // saddlv s21, v24.8h
  CMP_IO(dci(0x04c03a79), R(v25), R(p6, v19));  // saddv d25, p6, z19.d
  CMP_IO(dci(0x4e3a1087), R(v7), R(v4, v26));   // saddw2 v7.8h, v4.8h, v26.16b
  CMP_IO(dci(0x45cc4053), R(v19), R(v2, v12));  // saddwb z19.d, z2.d, z12.s
  CMP_IO(dci(0x45da4611), R(v17), R(v16, v26));    // saddwt z17.d, z16.d, z26.s
  CMP_IO(dci(0x5a1b0252), R(x18), R(x18, x27));    // sbc w18, w18, w27
  CMP_IO(dci(0xda1b00bb), R(x27), R(x5, x27));     // sbc x27, x5, x27
  CMP_IO(dci(0x4599d060), R(v0), R(v0, v3, v25));  // sbclb z0.s, z3.s, z25.s
  CMP_IO(dci(0x45dad47a), R(v26), R(v26, v3, v26));  // sbclt z26.d, z3.d, z26.d
  CMP_IO(dci(0x7a1d006b), R(x11), R(x3, x29));       // sbcs w11, w3, w29
  CMP_IO(dci(0xfa1d0202), R(x2), R(x16, x29));       // sbcs x2, x16, x29
  CMP_IO(dci(0x13181b65), R(x5), R(x27));            // sbfiz w5, w27, #8, #7
  CMP_IO(dci(0x935a594c), R(x12), R(x10));           // sbfiz x12, x10, #38, #23
  CMP_IO(dci(0x4e21d894), R(v20), R(v4));            // scvtf v20.4s, v4.4s
  CMP_IO(dci(0x4e79db9b), R(v27), R(v28));           // scvtf v27.8h, v28.8h
  CMP_IO(dci(0x0f1de61c), R(v28), R(v16));           // scvtf v28.4h, v16.4h, #3
  CMP_IO(dci(0x5e61db8e), R(v14), R(v28));           // scvtf d14, d28
  CMP_IO(dci(0x5e79dbb5), R(v21), R(v29));           // scvtf h21, h29
  CMP_IO(dci(0x5f7ee4c6), R(v6), R(v6));             // scvtf d6, d6, #2
  CMP_IO(dci(0x1e42f078), R(v24), R(x3));            // scvtf d24, w3, #4
  CMP_IO(dci(0x1e6202d4), R(v20), R(x22));           // scvtf d20, w22

  CLEANUP();
}

TEST(group_53) {
  SETUP();

  CMP_IO(dci(0x9e42513d), R(v29), R(x9));            // scvtf d29, x9, #44
  CMP_IO(dci(0x9e620399), R(v25), R(x28));           // scvtf d25, x28
  CMP_IO(dci(0x1ec2f38c), R(v12), R(x28));           // scvtf h12, w28, #4
  CMP_IO(dci(0x1ee20130), R(v16), R(x9));            // scvtf h16, w9
  CMP_IO(dci(0x9ec2d451), R(v17), R(x2));            // scvtf h17, x2, #11
  CMP_IO(dci(0x9ee201f5), R(v21), R(x15));           // scvtf h21, x15
  CMP_IO(dci(0x1e028109), R(v9), R(x8));             // scvtf s9, w8, #32
  CMP_IO(dci(0x1e2202ad), R(v13), R(x21));           // scvtf s13, w21
  CMP_IO(dci(0x9e02a044), R(v4), R(x2));             // scvtf s4, x2, #24
  CMP_IO(dci(0x9e2201e8), R(v8), R(x15));            // scvtf s8, x15
  CMP_IO(dci(0x6552b0d5), R(v21), R(v21, p4, v6));   // scvtf z21.h, p4/m, z6.h
  CMP_IO(dci(0x65d0a60f), R(v15), R(v15, p1, v16));  // scvtf z15.d, p1/m, z16.s
  CMP_IO(dci(0x6554b324), R(v4), R(v4, p4, v25));    // scvtf z4.h, p4/m, z25.s
  CMP_IO(dci(0x6594ab72), R(v18), R(v18, p2, v27));  // scvtf z18.s, p2/m, z27.s
  CMP_IO(dci(0x65d6beba), R(v26), R(v26, p7, v21));  // scvtf z26.d, p7/m, z21.d
  CMP_IO(dci(0x6556aa45), R(v5), R(v5, p2, v18));    // scvtf z5.h, p2/m, z18.d
  CMP_IO(dci(0x65d4ac37), R(v23), R(v23, p3, v1));   // scvtf z23.s, p3/m, z1.d
  CMP_IO(dci(0x1add0c92), R(x18), R(x4, x29));       // sdiv w18, w4, w29
  CMP_IO(dci(0x9add0efb), R(x27), R(x23, x29));      // sdiv x27, x23, x29
  CMP_IO(dci(0x04d412af),
         R(v15),
         R(p4, v15, v21));  // sdiv z15.d, p4/m, z15.d, z21.d
  CMP_IO(dci(0x04961da5),
         R(v5),
         R(p7, v5, v13));  // sdivr z5.s, p7/m, z5.s, z13.s
  CMP_IO(dci(0x4ffdeae1),
         R(v1),
         R(v1, v23, v29));  // sdot v1.4s, v23.16b, v29.4b[3]
  CMP_IO(dci(0x0e5c9772),
         R(v18),
         R(v18, v27, v28));  // sdot v18.4h, v27.8b, v28.8b
  CMP_IO(dci(0x449e0250),
         R(v16),
         R(v16, v18, v30));  // sdot z16.s, z18.b, z30.b
  CMP_IO(dci(0x44fb00ae),
         R(v14),
         R(v14, v5, v11));                    // sdot z14.d, z5.h, z11.h[1]
  CMP_IO(dci(0x44bb0003), R(v3), R(v0, v3));  // sdot z3.s, z0.b, z3.b[3]
  CMP_IO(dci(0x250d661f),
         R(p15),
         R(p9, p0, p13));  // sel p15.b, p9, p0.b, p13.b
  CMP_IO(dci(0x057cfdf5),
         R(v21),
         R(p15, v15, v28));  // sel z21.h, p15, z15.h, z28.h
  CMP_IO(dci(0x19c487bb),
         R(x27, x29),
         R(x27, x29, x4));  // sete [x27]!, x29!, x4
  CMP_IO(dci(0x19dda629),
         R(x9, x17),
         R(x9, x17, x29));               // seten [x9]!, x17!, x29
  CMP_IO(dci(0x3a004a2d), R(), R(x17));  // setf16 w17
  CMP_IO(dci(0x3a00092d), R(), R(x9));   // setf8 w9

  CLEANUP();
}

TEST(group_54) {
  SETUP();

  CMP_IO(dci(0x252c9000), R(), R());                    // setffr
  CMP_IO(dci(0x1dc68493), R(x19, x4), R(x19, x4, x6));  // setge [x19]!, x4!, x6
  CMP_IO(dci(0x1dcda6b0),
         R(x16, x21),
         R(x16, x21, x13));  // setgen [x16]!, x21!, x13
  CMP_IO(dci(0x1dc24663), R(x3, x19), R(x3, x19, x2));  // setgm [x3]!, x19!, x2
  CMP_IO(dci(0x1dcd6499),
         R(x25, x4),
         R(x25, x4, x13));  // setgmn [x25]!, x4!, x13
  CMP_IO(dci(0x1dd50545),
         R(x5, x10),
         R(x5, x10, x21));  // setgp [x5]!, x10!, x21
  CMP_IO(dci(0x1dda262b),
         R(x11, x17),
         R(x11, x17, x26));  // setgpn [x11]!, x17!, x26
  CMP_IO(dci(0x19d6472b),
         R(x11, x25),
         R(x11, x25, x22));  // setm [x11]!, x25!, x22
  CMP_IO(dci(0x19de6655),
         R(x21, x18),
         R(x21, x18, x30));  // setmn [x21]!, x18!, x30
  CMP_IO(dci(0x19da054c),
         R(x12, x10),
         R(x12, x10, x26));  // setp [x12]!, x10!, x26
  CMP_IO(dci(0x19c1266f),
         R(x15, x19),
         R(x15, x19, x1));                      // setpn [x15]!, x19!, x1
  CMP_IO(dci(0x0ea1066f), R(v15), R(v19, v1));  // shadd v15.2s, v19.2s, v1.2s
  CMP_IO(dci(0x44509df5),
         R(v21),
         R(p7, v21, v15));                  // shadd z21.h, p7/m, z21.h, z15.h
  CMP_IO(dci(0x4f5e57da), R(v26), R(v30));  // shl v26.2d, v30.2d, #30
  CMP_IO(dci(0x5f775555), R(v21), R(v10));  // shl d21, d10, #55
  CMP_IO(dci(0x6ea13ace), R(v14), R(v22));  // shll2 v14.2d, v22.4s, #32
  CMP_IO(dci(0x0f158677), R(v23), R(v19));  // shrn v23.4h, v19.4s, #11
  CMP_IO(dci(0x453e1250), R(v16), R(v18));  // shrnb z16.h, z18.s, #2
  CMP_IO(dci(0x457d156a), R(v10), R(v10, v11));  // shrnt z10.s, z11.d, #3
  CMP_IO(dci(0x0eb824ed), R(v13), R(v7, v24));   // shsub v13.2s, v7.2s, v24.2s
  CMP_IO(dci(0x44529eba),
         R(v26),
         R(p7, v26, v21));  // shsub z26.h, p7/m, z26.h, z21.h
  CMP_IO(dci(0x44169fdc),
         R(v28),
         R(p7, v28, v30));  // shsubr z28.b, p7/m, z28.b, z30.b
  CMP_IO(dci(0x6f515745), R(v5), R(v5, v26));    // sli v5.2d, v26.2d, #17
  CMP_IO(dci(0x7f4f5612), R(v18), R(v18, v16));  // sli d18, d16, #15
  CMP_IO(dci(0x451af7cf), R(v15), R(v15, v30));  // sli z15.h, z30.h, #10
  CMP_IO(dci(0x9b3d3957),
         R(x23),
         R(x10, x29, x14));                     // smaddl x23, w10, w29, x14
  CMP_IO(dci(0x0eab6415), R(v21), R(v0, v11));  // smax v21.2s, v0.2s, v11.2s
  CMP_IO(dci(0x04481e52),
         R(v18),
         R(p7, v18, v18));                     // smax z18.h, p7/m, z18.h, z18.h
  CMP_IO(dci(0x25e8ce1e), R(v30), R(v30));     // smax z30.d, z30.d, #112
  CMP_IO(dci(0x4e61a745), R(v5), R(v26, v1));  // smaxp v5.8h, v26.8h, v1.8h
  CMP_IO(dci(0x4454b202),
         R(v2),
         R(p4, v2, v16));                  // smaxp z2.h, p4/m, z2.h, z16.h
  CMP_IO(dci(0x0e30a946), R(v6), R(v10));  // smaxv b6, v10.8b

  CLEANUP();
}

TEST(group_55) {
  SETUP();

  CMP_IO(dci(0x04c827f6), R(v22), R(p1, v31));  // smaxv d22, p1, z31.d
  CMP_IO(dci(0x4e3a6fcf),
         R(v15),
         R(v30, v26));  // smin v15.16b, v30.16b, v26.16b
  CMP_IO(dci(0x048a1010),
         R(v16),
         R(p4, v16, v0));                 // smin z16.s, p4/m, z16.s, z0.s
  CMP_IO(dci(0x252ac604), R(v4), R(v4));  // smin z4.b, z4.b, #48
  CMP_IO(dci(0x4e3eac9f),
         R(v31),
         R(v4, v30));  // sminp v31.16b, v4.16b, v30.16b
  CMP_IO(dci(0x4496b6a0),
         R(v0),
         R(p5, v0, v21));                       // sminp z0.s, p5/m, z0.s, z21.s
  CMP_IO(dci(0x0e31a94c), R(v12), R(v10));      // sminv b12, v10.8b
  CMP_IO(dci(0x040a266c), R(v12), R(p1, v19));  // sminv b12, p1, z19.b
  CMP_IO(dci(0x4eba82f0),
         R(v16),
         R(v16, v23, v26));  // smlal2 v16.2d, v23.4s, v26.4s
  CMP_IO(dci(0x4f8421ed),
         R(v13),
         R(v13, v15, v4));  // smlal2 v13.2d, v15.4s, v4.s[0]
  CMP_IO(dci(0x44d64033),
         R(v19),
         R(v19, v1, v22));  // smlalb z19.d, z1.s, z22.s
  CMP_IO(dci(0x44fb80d5),
         R(v21),
         R(v21, v6, v11));  // smlalb z21.d, z6.s, z11.s[2]
  CMP_IO(dci(0x44ba8bdc),
         R(v28),
         R(v28, v30, v2));  // smlalb z28.s, z30.h, z2.h[7]
  CMP_IO(dci(0x44de4571),
         R(v17),
         R(v17, v11, v30));  // smlalt z17.d, z11.s, z30.s
  CMP_IO(dci(0x44fa8573),
         R(v19),
         R(v19, v11, v10));  // smlalt z19.d, z11.s, z10.s[2]
  CMP_IO(dci(0x44bb8f3e),
         R(v30),
         R(v30, v25, v3));  // smlalt z30.s, z25.h, z3.h[7]
  CMP_IO(dci(0x0eb4a277),
         R(v23),
         R(v23, v19, v20));  // smlsl v23.2d, v19.2s, v20.2s
  CMP_IO(dci(0x0f596132),
         R(v18),
         R(v18, v9, v9));  // smlsl v18.4s, v9.4h, v9.h[1]
  CMP_IO(dci(0x445e5315),
         R(v21),
         R(v21, v24, v30));  // smlslb z21.h, z24.b, z30.b
  CMP_IO(dci(0x44fbab17),
         R(v23),
         R(v23, v24, v11));  // smlslb z23.d, z24.s, z11.s[3]
  CMP_IO(dci(0x44bba19a),
         R(v26),
         R(v26, v12, v3));  // smlslb z26.s, z12.h, z3.h[6]
  CMP_IO(dci(0x449d57b3),
         R(v19),
         R(v19, v29, v29));  // smlslt z19.s, z29.h, z29.h
  CMP_IO(dci(0x44fba731),
         R(v17),
         R(v17, v25, v11));  // smlslt z17.d, z25.s, z11.s[2]
  CMP_IO(dci(0x44bbad80),
         R(v0),
         R(v0, v12, v3));  // smlslt z0.s, z12.h, z3.h[7]
  CMP_IO(dci(0x4e9ba679),
         R(v25),
         R(v25, v19, v27));  // smmla v25.4s, v19.16b, v27.16b
  CMP_IO(dci(0x45199b8e),
         R(v14),
         R(v14, v28, v25));                // smmla z14.s, z28.b, z25.b
  CMP_IO(dci(0x0e1b2cae), R(x14), R(v5));  // smov w14, v5.b[13]
  CMP_IO(dci(0x4e142c5a), R(x26), R(v2));  // smov x26, v2.s[2]
  CMP_IO(dci(0x9b3d9a1c), R(x28), R(x16, x29, x6));  // smsubl x28, w16, w29, x6
  CMP_IO(dci(0x9b5a106d), R(x13), R(x3, x26));       // smulh x13, x3, x26
  CMP_IO(dci(0x0452182a),
         R(v10),
         R(p6, v10, v1));  // smulh z10.h, p6/m, z10.h, z1.h
  CMP_IO(dci(0x043e689f), R(v31), R(v4, v30));  // smulh z31.b, z4.b, z30.b

  CLEANUP();
}

TEST(group_56) {
  SETUP();

  CMP_IO(dci(0x4e39c2bc),
         R(v28),
         R(v21, v25));  // smull2 v28.8h, v21.16b, v25.16b
  CMP_IO(dci(0x4f9aaba8),
         R(v8),
         R(v29, v26));  // smull2 v8.2d, v29.4s, v26.s[2]
  CMP_IO(dci(0x45d972c9), R(v9), R(v22, v25));   // smullb z9.d, z22.s, z25.s
  CMP_IO(dci(0x44fdc86b), R(v11), R(v3, v13));   // smullb z11.d, z3.s, z13.s[3]
  CMP_IO(dci(0x44bcc116), R(v22), R(v8, v4));    // smullb z22.s, z8.h, z4.h[6]
  CMP_IO(dci(0x45ce754b), R(v11), R(v10, v14));  // smullt z11.d, z10.s, z14.s
  CMP_IO(dci(0x44fccead),
         R(v13),
         R(v21, v12));                          // smullt z13.d, z21.s, z12.s[3]
  CMP_IO(dci(0x44bdc6d4), R(v20), R(v22, v5));  // smullt z20.s, z22.h, z5.h[6]
  CMP_IO(dci(0x052d994c),
         R(v12),
         R(p6, v10, v11));  // splice z12.b, p6, {z10.b, z11.b}
  CMP_IO(dci(0x052c92bc),
         R(v28),
         R(p4, v28, v21));                  // splice z28.b, p4, z28.b, z21.b
  CMP_IO(dci(0x4ee0780f), R(v15), R(v0));   // sqabs v15.2d, v0.2d
  CMP_IO(dci(0x5e207abc), R(v28), R(v21));  // sqabs b28, b21
  CMP_IO(dci(0x4408bf17), R(v23), R(v23, p7, v24));  // sqabs z23.b, p7/m, z24.b
  CMP_IO(dci(0x0e7d0c85), R(v5), R(v4, v29));    // sqadd v5.4h, v4.4h, v29.4h
  CMP_IO(dci(0x5ebe0d8b), R(v11), R(v12, v30));  // sqadd s11, s12, s30
  CMP_IO(dci(0x44d89516),
         R(v22),
         R(v22, p5, v22, v8));              // sqadd z22.d, p5/m, z22.d, z8.d
  CMP_IO(dci(0x2564ebda), R(v26), R(v26));  // sqadd z26.h, z26.h, #94, lsl #8
  CMP_IO(dci(0x04bd13b3), R(v19), R(v29, v29));  // sqadd z19.s, z29.s, z29.s
  CMP_IO(dci(0x4541ddea),
         R(v10),
         R(v10, v15));                      // sqcadd z10.h, z10.h, z15.h, #270
  CMP_IO(dci(0x042dfa0f), R(x15), R(x15));  // sqdecb x15, w15, #0x10, mul #14
  CMP_IO(dci(0x043bfbf6), R(x22), R(x22));  // sqdecb x22, all, mul #12
  CMP_IO(dci(0x04edfb99), R(x25), R(x25));  // sqdecd x25, w25, #0x1c, mul #14
  CMP_IO(dci(0x04fbfb24), R(x4), R(x4));    // sqdecd x4, #0x19, mul #12
  CMP_IO(dci(0x04eac86d), R(v13), R(v13));  // sqdecd z13.d, vl3, mul #11
  CMP_IO(dci(0x046cf9f5), R(x21), R(x21));  // sqdech x21, w21, #0xf, mul #13
  CMP_IO(dci(0x047bf980), R(x0), R(x0));    // sqdech x0, vl128, mul #12
  CMP_IO(dci(0x0469cb81), R(v1), R(v1));    // sqdech z1.h, #0x1c, mul #10
  CMP_IO(dci(0x25ea8811), R(x17), R(p0, x17));  // sqdecp x17, p0.d, w17
  CMP_IO(dci(0x252a8c1c), R(x28), R(x28, p0));  // sqdecp x28, p0.b
  CMP_IO(dci(0x256a8165), R(v5), R(v5, p11));   // sqdecp z5.h, p11
  CMP_IO(dci(0x04acf844), R(x4), R(x4));        // sqdecw x4, w4, vl2, mul #13
  CMP_IO(dci(0x04bbfa79), R(x25), R(x25));      // sqdecw x25, #0x13, mul #12

  CLEANUP();
}

TEST(group_57) {
  SETUP();

  CMP_IO(dci(0x04aaca38), R(v24), R(v24));  // sqdecw z24.s, #0x11, mul #11
  CMP_IO(dci(0x0eb99053),
         R(v19),
         R(v19, v2, v25));  // sqdmlal v19.2d, v2.2s, v25.2s
  CMP_IO(dci(0x4f4231fb),
         R(v27),
         R(v27, v15, v2));  // sqdmlal2 v27.4s, v15.8h, v2.h[0]
  CMP_IO(dci(0x5e7d9085), R(v5), R(v5, v4, v29));  // sqdmlal s5, h4, h29
  CMP_IO(dci(0x5f583b65), R(v5), R(v5, v27, v8));  // sqdmlal s5, h27, v8.h[5]
  CMP_IO(dci(0x44826164), R(v4), R(v4, v11, v2));  // sqdmlalb z4.s, z11.h, z2.h
  CMP_IO(dci(0x44fa2b0a),
         R(v10),
         R(v24, v10));  // sqdmlalb z10.d, z24.s, z10.s[3]
  CMP_IO(dci(0x44bb21a7),
         R(v7),
         R(v7, v13, v3));  // sqdmlalb z7.s, z13.h, z3.h[6]
  CMP_IO(dci(0x44990860),
         R(v0),
         R(v0, v3, v25));  // sqdmlalbt z0.s, z3.h, z25.h
  CMP_IO(dci(0x44dd66ee),
         R(v14),
         R(v14, v23, v29));  // sqdmlalt z14.d, z23.s, z29.s
  CMP_IO(dci(0x44fb2410),
         R(v16),
         R(v16, v0, v11));  // sqdmlalt z16.d, z0.s, z11.s[2]
  CMP_IO(dci(0x44ba24a1),
         R(v1),
         R(v1, v5, v2));  // sqdmlalt z1.s, z5.h, z2.h[6]
  CMP_IO(dci(0x4e78b365),
         R(v5),
         R(v5, v27, v24));  // sqdmlsl2 v5.4s, v27.8h, v24.8h
  CMP_IO(dci(0x4f7a72fd),
         R(v29),
         R(v29, v23, v10));  // sqdmlsl2 v29.4s, v23.8h, v10.h[3]
  CMP_IO(dci(0x5ea2b383), R(v3), R(v3, v28, v2));  // sqdmlsl d3, s28, s2
  CMP_IO(dci(0x5fb97853),
         R(v19),
         R(v19, v2, v25));                       // sqdmlsl d19, s2, v25.s[3]
  CMP_IO(dci(0x445d694a), R(v10), R(v10, v29));  // sqdmlslb z10.h, z10.b, z29.b
  CMP_IO(dci(0x44fb326c),
         R(v12),
         R(v12, v19, v11));  // sqdmlslb z12.d, z19.s, z11.s[2]
  CMP_IO(dci(0x44bb3a45),
         R(v5),
         R(v5, v18, v3));  // sqdmlslb z5.s, z18.h, z3.h[7]
  CMP_IO(dci(0x44da0e1e),
         R(v30),
         R(v30, v16, v26));  // sqdmlslbt z30.d, z16.s, z26.s
  CMP_IO(dci(0x449e6e50),
         R(v16),
         R(v16, v18, v30));  // sqdmlslt z16.s, z18.h, z30.h
  CMP_IO(dci(0x44fb34ae),
         R(v14),
         R(v14, v5, v11));                    // sqdmlslt z14.d, z5.s, z11.s[2]
  CMP_IO(dci(0x44bb3c03), R(v3), R(v0, v3));  // sqdmlslt z3.s, z0.h, z3.h[7]
  CMP_IO(dci(0x0fabc015),
         R(v21),
         R(v0, v11));  // sqdmulh v21.2s, v0.2s, v11.s[1]
  CMP_IO(dci(0x0ea1b66f), R(v15), R(v19, v1));  // sqdmulh v15.2s, v19.2s, v1.2s
  CMP_IO(dci(0x5f65c176), R(v22), R(v11, v5));  // sqdmulh h22, h11, v5.h[2]
  CMP_IO(dci(0x5e7fb538), R(v24), R(v9, v31));  // sqdmulh h24, h9, h31
  CMP_IO(dci(0x043e73f4), R(v20), R(v31, v30));  // sqdmulh z20.b, z31.b, z30.b
  CMP_IO(dci(0x44fbf252),
         R(v18),
         R(v18, v11));                         // sqdmulh z18.d, z18.d, z11.d[1]
  CMP_IO(dci(0x447bf0ae), R(v14), R(v5, v3));  // sqdmulh z14.h, z5.h, z3.h[7]
  CMP_IO(dci(0x44bcf0ef), R(v15), R(v7, v4));  // sqdmulh z15.s, z7.s, z4.s[3]
  CMP_IO(dci(0x4eb8d3a3),
         R(v3),
         R(v29, v24));  // sqdmull2 v3.2d, v29.4s, v24.4s

  CLEANUP();
}

TEST(group_58) {
  SETUP();

  CMP_IO(dci(0x0fbab22b),
         R(v11),
         R(v17, v26));  // sqdmull v11.2d, v17.2s, v26.s[1]
  CMP_IO(dci(0x5e61d345), R(v5), R(v26, v1));   // sqdmull s5, h26, h1
  CMP_IO(dci(0x5f79b925), R(v5), R(v9, v9));    // sqdmull s5, h9, v9.h[7]
  CMP_IO(dci(0x45da6166), R(v6), R(v11, v26));  // sqdmullb z6.d, z11.s, z26.s
  CMP_IO(dci(0x44fce9e8),
         R(v8),
         R(v15, v12));  // sqdmullb z8.d, z15.s, z12.s[3]
  CMP_IO(dci(0x44bde399),
         R(v25),
         R(v28, v5));  // sqdmullb z25.s, z28.h, z5.h[6]
  CMP_IO(dci(0x459e64dd), R(v29), R(v6, v30));  // sqdmullt z29.s, z6.h, z30.h
  CMP_IO(dci(0x44fde602),
         R(v2),
         R(v16, v13));  // sqdmullt z2.d, z16.s, z13.s[2]
  CMP_IO(dci(0x44bcef7f),
         R(v31),
         R(v27, v4));                         // sqdmullt z31.s, z27.h, z4.h[7]
  CMP_IO(dci(0x042df399), R(x25), R(x25));    // sqincb x25, w25, #0x1c, mul #14
  CMP_IO(dci(0x043bf324), R(x4), R(x4));      // sqincb x4, #0x19, mul #12
  CMP_IO(dci(0x04edf20f), R(x15), R(x15));    // sqincd x15, w15, #0x10, mul #14
  CMP_IO(dci(0x04fbf3f6), R(x22), R(x22));    // sqincd x22, all, mul #12
  CMP_IO(dci(0x04e9c39b), R(v27), R(v27));    // sqincd z27.d, #0x1c, mul #10
  CMP_IO(dci(0x046cf123), R(x3), R(x3));      // sqinch x3, w3, vl16, mul #13
  CMP_IO(dci(0x047af30a), R(x10), R(x10));    // sqinch x10, #0x18, mul #11
  CMP_IO(dci(0x0469c1f7), R(v23), R(v23));    // sqinch z23.h, #0xf, mul #10
  CMP_IO(dci(0x25288887), R(x7), R(p4, x7));  // sqincp x7, p4.b, w7
  CMP_IO(dci(0x25e88cee), R(x14), R(x14, p7));  // sqincp x14, p7.d
  CMP_IO(dci(0x25a88093), R(v19), R(v19, p4));  // sqincp z19.s, p4
  CMP_IO(dci(0x04acf116), R(x22), R(x22));      // sqincw x22, w22, vl8, mul #13
  CMP_IO(dci(0x04bcf0ef), R(x15), R(x15));      // sqincw x15, vl7, mul #13
  CMP_IO(dci(0x04aac3c2), R(v2), R(v2));        // sqincw z2.s, mul3, mul #11
  CMP_IO(dci(0x6e6079ea), R(v10), R(v15));      // sqneg v10.8h, v15.8h
  CMP_IO(dci(0x7ea07a38), R(v24), R(v17));      // sqneg s24, s17
  CMP_IO(dci(0x4489af4b), R(v11), R(v11, p3, v26));  // sqneg z11.s, p3/m, z26.s
  CMP_IO(dci(0x44593d25),
         R(v5),
         R(v5, v9, v25));  // sqrdcmlah z5.h, z9.h, z25.h, #270
  CMP_IO(dci(0x44bd7c6b),
         R(v11),
         R(v11, v3, v5));  // sqrdcmlah z11.h, z3.h, z5.h[3], #270
  CMP_IO(dci(0x44fc782a),
         R(v10),
         R(v10, v1, v12));  // sqrdcmlah z10.s, z1.s, z12.s[1], #180
  CMP_IO(dci(0x6f84dbe8),
         R(v8),
         R(v8, v31, v4));  // sqrdmlah v8.4s, v31.4s, v4.s[2]
  CMP_IO(dci(0x2e5b8652),
         R(v18),
         R(v18, v18, v27));  // sqrdmlah v18.4h, v18.4h, v27.4h
  CMP_IO(dci(0x7fb8d0ed),
         R(v13),
         R(v13, v7, v24));  // sqrdmlah s13, s7, v24.s[1]

  CLEANUP();
}

TEST(group_59) {
  SETUP();

  CMP_IO(dci(0x7e9b8410), R(v16), R(v16, v0, v27));  // sqrdmlah s16, s0, s27
  CMP_IO(dci(0x449d73c0),
         R(v0),
         R(v0, v30, v29));  // sqrdmlah z0.s, z30.s, z29.s
  CMP_IO(dci(0x44fb133e),
         R(v30),
         R(v30, v25, v11));  // sqrdmlah z30.d, z25.d, z11.d[1]
  CMP_IO(dci(0x447b10e2),
         R(v2),
         R(v2, v7, v3));  // sqrdmlah z2.h, z7.h, z3.h[7]
  CMP_IO(dci(0x44ba1173),
         R(v19),
         R(v19, v11, v2));  // sqrdmlah z19.s, z11.s, z2.s[3]
  CMP_IO(dci(0x6f7efb15),
         R(v21),
         R(v21, v24, v14));  // sqrdmlsh v21.8h, v24.8h, v14.h[7]
  CMP_IO(dci(0x6e1b8db4),
         R(v20),
         R(v20, v13, v27));  // sqrdmlsh v20.16b, v13.16b, v27.16b
  CMP_IO(dci(0x7fbdf2fb),
         R(v27),
         R(v27, v23, v29));  // sqrdmlsh s27, s23, v29.s[1]
  CMP_IO(dci(0x7edb8cae), R(v14), R(v14, v5, v27));  // sqrdmlsh d14, d5, d27
  CMP_IO(dci(0x44de757e),
         R(v30),
         R(v30, v11, v30));  // sqrdmlsh z30.d, z11.d, z30.d
  CMP_IO(dci(0x44fb1580),
         R(v0),
         R(v0, v12, v11));  // sqrdmlsh z0.d, z12.d, z11.d[1]
  CMP_IO(dci(0x443b1724),
         R(v4),
         R(v4, v25, v3));  // sqrdmlsh z4.h, z25.h, z3.h[3]
  CMP_IO(dci(0x44bb1731),
         R(v17),
         R(v17, v25, v3));  // sqrdmlsh z17.s, z25.s, z3.s[3]
  CMP_IO(dci(0x0fbed243),
         R(v3),
         R(v18, v30));  // sqrdmulh v3.2s, v18.2s, v30.s[1]
  CMP_IO(dci(0x6ebab7a8),
         R(v8),
         R(v29, v26));  // sqrdmulh v8.4s, v29.4s, v26.4s
  CMP_IO(dci(0x5f7dd3cd), R(v13), R(v30, v13));  // sqrdmulh h13, h30, v13.h[3]
  CMP_IO(dci(0x7e7ab47a), R(v26), R(v3, v26));   // sqrdmulh h26, h3, h26
  CMP_IO(dci(0x04797532), R(v18), R(v9, v25));   // sqrdmulh z18.h, z9.h, z25.h
  CMP_IO(dci(0x44fdf6d4),
         R(v20),
         R(v22, v13));  // sqrdmulh z20.d, z22.d, z13.d[1]
  CMP_IO(dci(0x443df530), R(v16), R(v9, v5));  // sqrdmulh z16.h, z9.h, z5.h[3]
  CMP_IO(dci(0x44bcf6ad),
         R(v13),
         R(v21, v4));  // sqrdmulh z13.s, z21.s, z4.s[3]
  CMP_IO(dci(0x0eab5c15), R(v21), R(v0, v11));   // sqrshl v21.2s, v0.2s, v11.2s
  CMP_IO(dci(0x5e3a5fcf), R(v15), R(v30, v26));  // sqrshl b15, b30, b26
  CMP_IO(dci(0x444a9e52),
         R(v18),
         R(p7, v18, v18));  // sqrshl z18.h, p7/m, z18.h, z18.h
  CMP_IO(dci(0x440e86d4),
         R(v20),
         R(p1, v20, v22));                // sqrshlr z20.b, p1/m, z20.b, z22.b
  CMP_IO(dci(0x0f119c01), R(v1), R(v0));  // sqrshrn v1.4h, v0.4s, #15
  CMP_IO(dci(0x5f109c02), R(v2), R(v0));  // sqrshrn h0, s0, #16
  CMP_IO(dci(0x45392860), R(v0), R(v3));  // sqrshrnb z0.h, z3.s, #7
  CMP_IO(dci(0x457a2c7a), R(v26), R(v26, v3));  // sqrshrnt z26.s, z3.d, #6
  CMP_IO(dci(0x2f118c22), R(v2), R(v1));        // sqrshrun v2.4h, v1.4s, #15
  CMP_IO(dci(0x7f398df7), R(v23), R(v15));      // sqrshrun s23, d15, #7
  CMP_IO(dci(0x453e0a43), R(v3), R(v18));       // sqrshrunb z3.h, z18.s, #2

  CLEANUP();
}

TEST(group_60) {
  SETUP();

  CMP_IO(dci(0x457d0c85), R(v5), R(v5, v4));     // sqrshrunt z5.s, z4.d, #3
  CMP_IO(dci(0x0ebe4e43), R(v3), R(v18, v30));   // sqshl v3.2s, v18.2s, v30.2s
  CMP_IO(dci(0x4f7e74ac), R(v12), R(v5));        // sqshl v12.2d, v5.2d, #62
  CMP_IO(dci(0x5e7d4fcd), R(v13), R(v30, v29));  // sqshl h13, h30, h29
  CMP_IO(dci(0x5f667636), R(v22), R(v17));       // sqshl d22, d17, #38
  CMP_IO(dci(0x04c68109), R(v9), R(p0, v9));     // sqshl z9.d, p0/m, z9.d, #40
  CMP_IO(dci(0x440886d4),
         R(v20),
         R(p1, v20, v22));  // sqshl z20.b, p1/m, z20.b, z22.b
  CMP_IO(dci(0x444c9e52),
         R(v18),
         R(p7, v18, v18));                  // sqshlr z18.h, p7/m, z18.h, z18.h
  CMP_IO(dci(0x6f5965f7), R(v23), R(v15));  // sqshlu v23.2d, v15.2d, #25
  CMP_IO(dci(0x7f326559), R(v25), R(v10));  // sqshlu s25, s10, #18
  CMP_IO(dci(0x04cf8e86), R(v6), R(p3, v6));     // sqshlu z6.d, p3/m, z6.d, #52
  CMP_IO(dci(0x0f329638), R(v24), R(v17));       // sqshrn v24.2s, v17.2d, #14
  CMP_IO(dci(0x5f339445), R(v5), R(v2));         // sqshrn s5, d2, #13
  CMP_IO(dci(0x457e217e), R(v30), R(v11));       // sqshrnb z30.s, z11.d, #2
  CMP_IO(dci(0x453d261c), R(v28), R(v28, v16));  // sqshrnt z28.h, z16.s, #3
  CMP_IO(dci(0x6f3d86fb), R(v27), R(v23));       // sqshrun2 v27.4s, v23.2d, #3
  CMP_IO(dci(0x7f2e8715), R(v21), R(v24));       // sqshrun s21, d24, #18
  CMP_IO(dci(0x45790381), R(v1), R(v28));        // sqshrunb z1.s, z28.d, #7
  CMP_IO(dci(0x453a0487), R(v7), R(v7, v4));     // sqshrunt z7.h, z4.s, #6
  CMP_IO(dci(0x0e7d2d4a), R(v10), R(v10, v29));  // sqsub v10.4h, v10.4h, v29.4h
  CMP_IO(dci(0x5ebe2d98), R(v24), R(v12, v30));  // sqsub s24, s12, s30
  CMP_IO(dci(0x44da8109),
         R(v9),
         R(p0, v9, v8));                        // sqsub z9.d, p0/m, z9.d, z8.d
  CMP_IO(dci(0x2566cb15), R(v21), R(v21));      // sqsub z21.h, z21.h, #88
  CMP_IO(dci(0x04bd1bc0), R(v0), R(v30, v29));  // sqsub z0.s, z30.s, z29.s
  CMP_IO(dci(0x449e8f4b),
         R(v11),
         R(p3, v11, v26));                  // sqsubr z11.s, p3/m, z11.s, z26.s
  CMP_IO(dci(0x0e21494c), R(v12), R(v10));  // sqxtn v12.8b, v10.8h
  CMP_IO(dci(0x5e214946), R(v6), R(v10));   // sqxtn b6, h10
  CMP_IO(dci(0x45304258), R(v24), R(v18));  // sqxtnb z24.h, z18.s
  CMP_IO(dci(0x452846ee), R(v14), R(v14, v23));  // sqxtnt z14.b, z23.h
  CMP_IO(dci(0x2e212957), R(v23), R(v10));       // sqxtun v23.8b, v10.8h
  CMP_IO(dci(0x7ea128ed), R(v13), R(v7));        // sqxtun s13, d7
  CMP_IO(dci(0x456051f5), R(v21), R(v15));       // sqxtunb z21.s, z15.d

  CLEANUP();
}

TEST(group_61) {
  SETUP();

  CMP_IO(dci(0x453057f5), R(v21), R(v21, v31));  // sqxtunt z21.h, z31.s
  CMP_IO(dci(0x4eba17a8), R(v8), R(v29, v26));   // srhadd v8.4s, v29.4s, v26.4s
  CMP_IO(dci(0x44149f17),
         R(v23),
         R(p7, v23, v24));  // srhadd z23.b, p7/m, z23.b, z24.b
  CMP_IO(dci(0x6f66449f), R(v31), R(v31, v4));  // sri v31.2d, v4.2d, #26
  CMP_IO(dci(0x7f514489), R(v9), R(v9, v4));    // sri d9, d4, #47
  CMP_IO(dci(0x45d9f381), R(v1), R(v1, v28));   // sri z1.d, z28.d, #7
  CMP_IO(dci(0x4e7e57da), R(v26), R(v30));      // srshl v26.8h, v30.8h, v30.8h
  CMP_IO(dci(0x5efc5418), R(v24), R(v0, v28));  // srshl d24, d0, d28
  CMP_IO(dci(0x44c28799),
         R(v25),
         R(p1, v25, v28));  // srshl z25.d, p1/m, z25.d, z28.d
  CMP_IO(dci(0x448688bb),
         R(v27),
         R(p2, v27, v5));                   // srshlr z27.s, p2/m, z27.s, z5.s
  CMP_IO(dci(0x4f66249f), R(v31), R(v4));   // srshr v31.2d, v4.2d, #26
  CMP_IO(dci(0x5f762571), R(v17), R(v11));  // srshr d17, d11, #10
  CMP_IO(dci(0x04cc885e), R(v30), R(p2, v30));  // srshr z30.d, p2/m, z30.d, #30
  CMP_IO(dci(0x4f4537c0), R(v0), R(v0, v30));   // srsra v0.2d, v30.2d, #59
  CMP_IO(dci(0x5f4034eb), R(v11), R(v11, v7));  // srsra d11, d7, #64
  CMP_IO(dci(0x451ee8ac), R(v12), R(v12, v5));  // srsra z12.h, z5.h, #2
  CMP_IO(dci(0x4e3946bc),
         R(v28),
         R(v21, v25));  // sshl v28.16b, v21.16b, v25.16b
  CMP_IO(dci(0x5efa4566), R(v6), R(v11, v26));   // sshl d6, d11, d26
  CMP_IO(dci(0x0f28a4ee), R(v14), R(v7));        // sshll v14.2d, v7.2s, #8
  CMP_IO(dci(0x455aa159), R(v25), R(v10));       // sshllb z25.d, z10.s, #26
  CMP_IO(dci(0x451aa53f), R(v31), R(v9));        // sshllt z31.s, z9.h, #10
  CMP_IO(dci(0x4f6206fd), R(v29), R(v23));       // sshr v29.2d, v23.2d, #30
  CMP_IO(dci(0x5f410453), R(v19), R(v2));        // sshr d19, d2, #63
  CMP_IO(dci(0x4f5e16a2), R(v2), R(v2, v21));    // ssra v2.2d, v21.2d, #34
  CMP_IO(dci(0x5f421604), R(v4), R(v4, v16));    // ssra d4, d16, #62
  CMP_IO(dci(0x4559e1ea), R(v10), R(v10, v15));  // ssra z10.s, z15.s, #7
  CMP_IO(dci(0x0e3a214c), R(v12), R(v10, v26));  // ssubl v12.8h, v10.8b, v26.8b
  CMP_IO(dci(0x45da1159), R(v25), R(v10, v26));  // ssublb z25.d, z10.s, z26.s
  CMP_IO(dci(0x455d8bcd), R(v13), R(v30, v29));  // ssublbt z13.h, z30.b, z29.b
  CMP_IO(dci(0x454215fb), R(v27), R(v15, v2));   // ssublt z27.h, z15.b, z2.b
  CMP_IO(dci(0x455d8fcd), R(v13), R(v30, v29));  // ssubltb z13.h, z30.b, z29.b
  CMP_IO(dci(0x0e3a314c), R(v12), R(v10, v26));  // ssubw v12.8h, v10.8h, v26.8b

  CLEANUP();
}

TEST(group_62) {
  SETUP();

  CMP_IO(dci(0x45cc5234), R(v20), R(v17, v12));  // ssubwb z20.d, z17.d, z12.s
  CMP_IO(dci(0x45da561e), R(v30), R(v16, v26));  // ssubwt z30.d, z16.d, z26.s
  CMP_IO(dci(0x0c007381), R(), R(v1, x28));      // st1 {v1.8b}, [x28]
  CMP_IO(dci(0x0c00a381), R(), R(v1, v2, x28));  // st1 {v1.8b, v2.8b}, [x28]
  CMP_IO(dci(0x0c006381),
         R(),
         R(v1, v2, v3, x28));  // st1 {v1.8b, v2.8b, v3.8b}, [x28]
  CMP_IO(dci(0x0c002381),
         R(),
         R(v1, v2, v3, v4, x28));  // st1 {v1.8b, v2.8b, v3.8b, v4.8b}, [x28]
  CMP_IO(dci(0x4c9f7b15), R(x24), R(v21, x24));  // st1 {v21.4s}, [x24], #16
  CMP_IO(dci(0x4c9fab15),
         R(x24),
         R(v21, v22, x24));  // st1 {v21.4s, v22.4s}, [x24], #32
  CMP_IO(dci(0x4c9f6b15),
         R(x24),
         R(v21, v22, v23, x24));  // st1 {v21.4s, v22.4s, v23.4s}, [x24], #48
  CMP_IO(dci(0x4c9f2b15),
         R(x24),
         R(v21,
           v22,
           v23,
           v24,
           x24));  // st1 {v21.4s, v22.4s, v23.4s, v24.4s}, [x24], #64
  CMP_IO(dci(0x4c827b15), R(x24), R(v21, x24, x2));  // st1 {v21.4s}, [x24], x2
  CMP_IO(dci(0x4c9bab15),
         R(x24),
         R(v21, v22, x24, x27));  // st1 {v21.4s, v22.4s}, [x24], x27
  CMP_IO(dci(0x4c8f6b15),
         R(x24),
         R(v21,
           v22,
           v23,
           x24,
           x15));  // st1 {v21.4s, v22.4s, v23.4s}, [x24], x15
  CMP_IO(dci(0x4c882b15), R(x24), R(v21, v22, v23, v24, x24, x8));  // st1
                                                                    // {v21.4s,
                                                                    // v22.4s,
                                                                    // v23.4s,
                                                                    // v24.4s},
                                                                    // [x24], x8
  CMP_IO(dci(0x0d00053f), R(), R(v31, x9));      // st1 {v31.b}[1], [x9]
  CMP_IO(dci(0x0d00853f), R(), R(v31, x9));      // st1 {v31.d}[0], [x9]
  CMP_IO(dci(0x0d00413f), R(), R(v31, x9));      // st1 {v31.h}[0], [x9]
  CMP_IO(dci(0x0d00813f), R(), R(v31, x9));      // st1 {v31.s}[0], [x9]
  CMP_IO(dci(0x4d9f02f0), R(x23), R(v16, x23));  // st1 {v16.b}[8], [x23], #1
  CMP_IO(dci(0x4d860b15),
         R(x24),
         R(v21, x24, x6));                       // st1 {v21.b}[10], [x24], x6
  CMP_IO(dci(0x4d9f86f0), R(x23), R(v16, x23));  // st1 {v16.d}[1], [x23], #8
  CMP_IO(dci(0x4d8f8715),
         R(x24),
         R(v21, x24, x15));                      // st1 {v21.d}[1], [x24], x15
  CMP_IO(dci(0x4d9f42f0), R(x23), R(v16, x23));  // st1 {v16.h}[4], [x23], #2
  CMP_IO(dci(0x4d864b15),
         R(x24),
         R(v21, x24, x6));                       // st1 {v21.h}[5], [x24], x6
  CMP_IO(dci(0x4d9f82f0), R(x23), R(v16, x23));  // st1 {v16.s}[2], [x23], #4
  CMP_IO(dci(0x4d9b8315),
         R(x24),
         R(v21, x24, x27));  // st1 {v21.s}[2], [x24], x27
  CMP_IO(dci(0xe45cab72),
         R(),
         R(v18, p2, v27));  // st1b {z18.d}, p2, [z27.d, #28]
  CMP_IO(dci(0xe47da60f),
         R(),
         R(v15, p1, v16));  // st1b {z15.s}, p1, [z16.s, #29]
  CMP_IO(dci(0xe44eebda),
         R(),
         R(v26, p2, x30));  // st1b {z26.s}, p2, [x30, #-2, mul vl]
  CMP_IO(dci(0xe43d42fb),
         R(),
         R(v27, p0, x23, x29));  // st1b {z27.h}, p0, [x23, x29]
  CMP_IO(dci(0xe41db13d),
         R(),
         R(v29, p4, x9, v29));  // st1b {z29.d}, p4, [x9, z29.d]
  CMP_IO(dci(0xe41b8980),
         R(),
         R(v0, p2, x12, v27));  // st1b {z0.d}, p2, [x12, z27.d, uxtw]

  CLEANUP();
}

TEST(group_63) {
  SETUP();

  CMP_IO(dci(0xe45b8731),
         R(),
         R(v17, p1, x25, v27));  // st1b {z17.s}, p1, [x25, z27.s, uxtw]
  CMP_IO(dci(0xe5dca9e8),
         R(),
         R(v8, p2, v15));  // st1d {z8.d}, p2, [z15.d, #224]
  CMP_IO(dci(0xe58eea50),
         R(),
         R(v16, p2, x18));  // st1d {z16.b}, p2, [x18, #-2, mul vl]
  CMP_IO(dci(0xe5fd57cd),
         R(),
         R(v13, p5, x30, x29));  // st1d {z13.d}, p5, [x30, x29, lsl #3]
  CMP_IO(dci(0xe5bcb82a),
         R(),
         R(v10, p6, x1, v28));  // st1d {z10.d}, p6, [x1, z28.d, lsl #3]
  CMP_IO(dci(0xe59dbc6b),
         R(),
         R(v11, p7, x3, v29));  // st1d {z11.d}, p7, [x3, z29.d]
  CMP_IO(dci(0xe5bbcf4b),
         R(),
         R(v11, p3, x26, v27));  // st1d {z11.d}, p3, [x26, z27.d, sxtw #3]
  CMP_IO(dci(0xe59acb0a),
         R(),
         R(v10, p2, x24, v26));  // st1d {z10.d}, p2, [x24, z26.d, sxtw]
  CMP_IO(dci(0xe4dcacfc),
         R(),
         R(v28, p3, v7));  // st1h {z28.d}, p3, [z7.d, #56]
  CMP_IO(dci(0xe4fcbdf5),
         R(),
         R(v21, p7, v15));  // st1h {z21.s}, p7, [z15.s, #56]
  CMP_IO(dci(0xe48ef3f4),
         R(),
         R(v20, p4, sp));  // st1h {z20.b}, p4, [sp, #-2, mul vl]
  CMP_IO(dci(0xe4fd5ae1),
         R(),
         R(v1, p6, x23, x29));  // st1h {z1.d}, p6, [x23, x29, lsl #1]
  CMP_IO(dci(0xe4bcb516),
         R(),
         R(v22, p5, x8, v28));  // st1h {z22.d}, p5, [x8, z28.d, lsl #1]
  CMP_IO(dci(0xe49db2c7),
         R(),
         R(v7, p4, x22, v29));  // st1h {z7.d}, p4, [x22, z29.d]
  CMP_IO(dci(0xe4bbc5a7),
         R(),
         R(v7, p1, x13, v27));  // st1h {z7.d}, p1, [x13, z27.d, sxtw #1]
  CMP_IO(dci(0xe49bc7f6),
         R(),
         R(v22, p1, sp, v27));  // st1h {z22.d}, p1, [sp, z27.d, sxtw]
  CMP_IO(dci(0xe4facb0a),
         R(),
         R(v10, p2, x24, v26));  // st1h {z10.s}, p2, [x24, z26.s, sxtw #1]
  CMP_IO(dci(0xe4dbcf4b),
         R(),
         R(v11, p3, x26, v27));  // st1h {z11.s}, p3, [x26, z27.s, sxtw]
  CMP_IO(dci(0xe55db13d),
         R(),
         R(v29, p4, v9));  // st1w {z29.d}, p4, [z9.d, #116]
  CMP_IO(dci(0xe57ca044), R(), R(v4, p0, v2));  // st1w {z4.s}, p0, [z2.s, #112]
  CMP_IO(dci(0xe56de485),
         R(),
         R(v5, p1, x4));  // st1w {z5.d}, p1, [x4, #-3, mul vl]
  CMP_IO(dci(0xe51e5d98),
         R(),
         R(v24, p7, x12, x30));  // st1w {z24.b}, p7, [x12, x30, lsl #2]
  CMP_IO(dci(0xe53ca923),
         R(),
         R(v3, p2, x9, v28));  // st1w {z3.d}, p2, [x9, z28.d, lsl #2]
  CMP_IO(dci(0xe51cab72),
         R(),
         R(v18, p2, x27, v28));  // st1w {z18.d}, p2, [x27, z28.d]
  CMP_IO(dci(0xe53b9e52),
         R(),
         R(v18, p7, x18, v27));  // st1w {z18.d}, p7, [x18, z27.d, uxtw #2]
  CMP_IO(dci(0xe51b9c03),
         R(),
         R(v3, p7, x0, v27));  // st1w {z3.d}, p7, [x0, z27.d, uxtw]
  CMP_IO(dci(0xe57c98ef),
         R(),
         R(v15, p6, x7, v28));  // st1w {z15.s}, p6, [x7, z28.s, uxtw #2]
  CMP_IO(dci(0xe55b94ae),
         R(),
         R(v14, p5, x5, v27));  // st1w {z14.s}, p5, [x5, z27.s, uxtw]
  CMP_IO(dci(0x4c00810d),
         R(),
         R(v13, v14, x8));  // st2 {v13.16b, v14.16b}, [x8]
  CMP_IO(dci(0x4c9f81ce),
         R(x14),
         R(v14, v15, x14));  // st2 {v14.16b, v15.16b}, [x14], #32
  CMP_IO(dci(0x4c9b81ce),
         R(x14),
         R(v14, v15, x14, x27));  // st2 {v14.16b, v15.16b}, [x14], x27
  CMP_IO(dci(0x4d201ad6),
         R(),
         R(v22, v23, x22));  // st2 {v22.b, v23.b}[14], [x22]

  CLEANUP();
}

TEST(group_64) {
  SETUP();

  CMP_IO(dci(0x4d2086d6),
         R(),
         R(v22, v23, x22));  // st2 {v22.d, v23.d}[1], [x22]
  CMP_IO(dci(0x4d205ad6),
         R(),
         R(v22, v23, x22));  // st2 {v22.h, v23.h}[7], [x22]
  CMP_IO(dci(0x4d2092d6),
         R(),
         R(v22, v23, x22));  // st2 {v22.s, v23.s}[3], [x22]
  CMP_IO(dci(0x4dbf03b5),
         R(x29),
         R(v21, v22, x29));  // st2 {v21.b, v22.b}[8], [x29], #2
  CMP_IO(dci(0x0dba0a50),
         R(x18),
         R(v16, v17, x18, x26));  // st2 {v16.b, v17.b}[2], [x18], x26
  CMP_IO(dci(0x4dbf87b5),
         R(x29),
         R(v21, v22, x29));  // st2 {v21.d, v22.d}[1], [x29], #16
  CMP_IO(dci(0x0db98650),
         R(x18),
         R(v16, v17, x18, x25));  // st2 {v16.d, v17.d}[0], [x18], x25
  CMP_IO(dci(0x4dbf43b5),
         R(x29),
         R(v21, v22, x29));  // st2 {v21.h, v22.h}[4], [x29], #4
  CMP_IO(dci(0x0db34a50),
         R(x18),
         R(v16, v17, x18, x19));  // st2 {v16.h, v17.h}[1], [x18], x19
  CMP_IO(dci(0x4dbf83b5),
         R(x29),
         R(v21, v22, x29));  // st2 {v21.s, v22.s}[2], [x29], #8
  CMP_IO(dci(0x0dbb8250),
         R(x18),
         R(v16, v17, x18, x27));  // st2 {v16.s, v17.s}[0], [x18], x27
  CMP_IO(dci(0xe43bfe52),
         R(),
         R(v18, v19, p7, x18));  // st2b {z18.b, z19.b}, p7, [x18, #-10, mul vl]
  CMP_IO(dci(0xe43b7c03),
         R(),
         R(v3, v4, p7, x0, x27));  // st2b {z3.b, z4.b}, p7, [x0, x27]
  CMP_IO(dci(0xe5bbfcc8),
         R(),
         R(v8, v9, p7, x6));  // st2d {z8.d, z9.d}, p7, [x6, #-10, mul vl]
  CMP_IO(dci(0xe5bb70d5), R(), R(v21, v22, p4, x6, x27));  // st2d {z21.d,
                                                           // z22.d}, p4, [x6,
                                                           // x27, lsl #3]
  CMP_IO(dci(0xd9bd4ad4), R(), R(x20, x22));     // st2g x20, [x22, #-704]
  CMP_IO(dci(0xd9bcb77f), R(x27), R(sp, x27));   // st2g sp, [x27], #-848
  CMP_IO(dci(0xd9bb9d9a), R(x12), R(x26, x12));  // st2g x26, [x12, #-1136]!
  CMP_IO(dci(0xe4baffdc),
         R(),
         R(v28, v29, p7, x30));  // st2h {z28.h, z29.h}, p7, [x30, #-12, mul vl]
  CMP_IO(dci(0xe4bb7a79), R(), R(v25, v26, p6, x19, x27));  // st2h {z25.h,
                                                            // z26.h}, p6, [x19,
                                                            // x27, lsl #1]
  CMP_IO(dci(0xe53ce41d),
         R(),
         R(v29, v30, p1, x0));  // st2w {z29.s, z30.s}, p1, [x0, #-8, mul vl]
  CMP_IO(dci(0xe53b6980),
         R(),
         R(v0, v1, p2, x12, x27));  // st2w {z0.s, z1.s}, p2, [x12, x27, lsl #2]
  CMP_IO(dci(0x4c00410d),
         R(),
         R(v13, v14, v15, x8));  // st3 {v13.16b, v14.16b, v15.16b}, [x8]
  CMP_IO(dci(0x4c9f41ce),
         R(x14),
         R(v14, v15, v16, x14));  // st3 {v14.16b, v15.16b, v16.16b}, [x14], #48
  CMP_IO(dci(0x4c8a41ce),
         R(x14),
         R(v14,
           v15,
           v16,
           x14,
           x10));  // st3 {v14.16b, v15.16b, v16.16b}, [x14], x10
  CMP_IO(dci(0x0d00306d),
         R(),
         R(v13, v14, v15, x3));  // st3 {v13.b, v14.b, v15.b}[4], [x3]
  CMP_IO(dci(0x0d00a46d),
         R(),
         R(v13, v14, v15, x3));  // st3 {v13.d, v14.d, v15.d}[0], [x3]
  CMP_IO(dci(0x0d00706d),
         R(),
         R(v13, v14, v15, x3));  // st3 {v13.h, v14.h, v15.h}[2], [x3]
  CMP_IO(dci(0x0d00b06d),
         R(),
         R(v13, v14, v15, x3));  // st3 {v13.s, v14.s, v15.s}[1], [x3]
  CMP_IO(dci(0x0d9f2e1e),
         R(x16),
         R(v30, v31, v0, x16));  // st3 {v30.b, v31.b, v0.b}[3], [x16], #3
  CMP_IO(dci(0x4d9a3fe7),
         R(sp),
         R(v7, v8, v9, sp, x26));  // st3 {v7.b, v8.b, v9.b}[15], [sp], x26
  CMP_IO(dci(0x0d9fa61e),
         R(x16),
         R(v30, v31, v0, x16));  // st3 {v30.d, v31.d, v0.d}[0], [x16], #24

  CLEANUP();
}

TEST(group_65) {
  SETUP();

  CMP_IO(dci(0x4d94a7e7),
         R(sp),
         R(v7, v8, v9, sp, x20));  // st3 {v7.d, v8.d, v9.d}[1], [sp], x20
  CMP_IO(dci(0x0d9f6a1e),
         R(x16),
         R(v30, v31, v0, x16));  // st3 {v30.h, v31.h, v0.h}[1], [x16], #6
  CMP_IO(dci(0x4d897be7),
         R(sp),
         R(v7, v8, v9, sp, x9));  // st3 {v7.h, v8.h, v9.h}[7], [sp], x9
  CMP_IO(dci(0x0d9fa21e),
         R(x16),
         R(v30, v31, v0, x16));  // st3 {v30.s, v31.s, v0.s}[0], [x16], #12
  CMP_IO(dci(0x4d9eb3e7),
         R(sp),
         R(v7, v8, v9, sp, x30));  // st3 {v7.s, v8.s, v9.s}[3], [sp], x30
  CMP_IO(dci(0xe45bf3e9), R(), R(v9, v10, v11, p4, sp));  // st3b {z9.b, z10.b,
                                                          // z11.b}, p4, [sp,
                                                          // #-15, mul vl]
  CMP_IO(dci(0xe45b666c), R(), R(v12, v13, v14, p1, x19, x27));  // st3b {z12.b,
                                                                 // z13.b,
                                                                 // z14.b}, p1,
                                                                 // [x19, x27]
  CMP_IO(dci(0xe5dbf25f), R(), R(v31, v0, v1, p4, x18));  // st3d {z31.d, z0.d,
                                                          // z1.d}, p4, [x18,
                                                          // #-15, mul vl]
  CMP_IO(dci(0xe5db7b3e), R(), R(v30, v31, v0, p6, x25, x27));  // st3d {z30.d,
                                                                // z31.d, z0.d},
                                                                // p6, [x25,
                                                                // x27, lsl #3]
  CMP_IO(dci(0xe4daf573),
         R(),
         R(v19,
           v20,
           v21,
           p5,
           x11));  // st3h {z19.h, z20.h, z21.h}, p5, [x11, #-18, mul vl]
  CMP_IO(dci(0xe4db64e2), R(), R(v2, v3, v4, p1, x7, x27));  // st3h {z2.h,
                                                             // z3.h, z4.h}, p1,
                                                             // [x7, x27, lsl
                                                             // #1]
  CMP_IO(dci(0xe55cee86), R(), R(v6, v7, v8, p3, x20));  // st3w {z6.s, z7.s,
                                                         // z8.s}, p3, [x20,
                                                         // #-12, mul vl]
  CMP_IO(dci(0xe55b7f17), R(), R(v23, v24, v25, p7, x24, x27));  // st3w {z23.s,
                                                                 // z24.s,
                                                                 // z25.s}, p7,
                                                                 // [x24, x27,
                                                                 // lsl #2]
  CMP_IO(dci(0x4c00010d), R(), R(v13, v14, v15, v16, x8));  // st4 {v13.16b,
                                                            // v14.16b, v15.16b,
                                                            // v16.16b}, [x8]
  CMP_IO(dci(0x4c9f01ce),
         R(x14),
         R(v14,
           v15,
           v16,
           v17,
           x14));  // st4 {v14.16b, v15.16b, v16.16b, v17.16b}, [x14], #64
  CMP_IO(dci(0x4c9901ce),
         R(x14),
         R(v14, v15, v16, v17, x14, x25));  // st4 {v14.16b, v15.16b, v16.16b,
                                            // v17.16b}, [x14], x25
  CMP_IO(dci(0x0d202604),
         R(),
         R(v4, v5, v6, v7, x16));  // st4 {v4.b, v5.b, v6.b, v7.b}[1], [x16]
  CMP_IO(dci(0x0d20a604),
         R(),
         R(v4, v5, v6, v7, x16));  // st4 {v4.d, v5.d, v6.d, v7.d}[0], [x16]
  CMP_IO(dci(0x0d206204),
         R(),
         R(v4, v5, v6, v7, x16));  // st4 {v4.h, v5.h, v6.h, v7.h}[0], [x16]
  CMP_IO(dci(0x0d20a204),
         R(),
         R(v4, v5, v6, v7, x16));  // st4 {v4.s, v5.s, v6.s, v7.s}[0], [x16]
  CMP_IO(dci(0x4dbf2ee3), R(x23), R(v3, v4, v5, v6, x23));  // st4 {v3.b, v4.b,
                                                            // v5.b, v6.b}[11],
                                                            // [x23], #4
  CMP_IO(dci(0x4db12bda),
         R(x30),
         R(v26, v27, v28, v29, x30, x17));  // st4 {v26.b, v27.b, v28.b,
                                            // v29.b}[10], [x30], x17
  CMP_IO(dci(0x4dbfa6e3), R(x23), R(v3, v4, v5, v6, x23));  // st4 {v3.d, v4.d,
                                                            // v5.d, v6.d}[1],
                                                            // [x23], #32
  CMP_IO(dci(0x4db2a7da),
         R(x30),
         R(v26, v27, v28, v29, x30, x18));  // st4 {v26.d, v27.d, v28.d,
                                            // v29.d}[1], [x30], x18
  CMP_IO(dci(0x4dbf6ae3),
         R(x23),
         R(v3, v4, v5, v6, x23));  // st4 {v3.h, v4.h, v5.h, v6.h}[5], [x23], #8
  CMP_IO(dci(0x4daa6bda),
         R(x30),
         R(v26, v27, v28, v29, x30, x10));  // st4 {v26.h, v27.h, v28.h,
                                            // v29.h}[5], [x30], x10
  CMP_IO(dci(0x4dbfa2e3), R(x23), R(v3, v4, v5, v6, x23));  // st4 {v3.s, v4.s,
                                                            // v5.s, v6.s}[2],
                                                            // [x23], #16
  CMP_IO(dci(0x4da2a3da),
         R(x30),
         R(v26, v27, v28, v29, x30, x2));  // st4 {v26.s, v27.s, v28.s,
                                           // v29.s}[2], [x30], x2
  CMP_IO(dci(0xe47bfcc8), R(), R(v8, v9, v10, v11, p7, x6));  // st4b {z8.b,
                                                              // z9.b, z10.b,
                                                              // z11.b}, p7,
                                                              // [x6, #-20, mul
                                                              // vl]
  CMP_IO(dci(0xe47b70d5),
         R(),
         R(v21, v22, v23, v24, p4, x6, x27));  // st4b {z21.b, z22.b, z23.b,
                                               // z24.b}, p4, [x6, x27]
  CMP_IO(dci(0xe5fbfe52), R(), R(v18, v19, v20, v21, p7, x18));  // st4d {z18.d,
                                                                 // z19.d,
                                                                 // z20.d,
                                                                 // z21.d}, p7,
                                                                 // [x18, #-20,
                                                                 // mul vl]
  CMP_IO(dci(0xe5fb7c03), R(), R(v3, v4, v5, v6, p7, x0, x27));  // st4d {z3.d,
                                                                 // z4.d, z5.d,
                                                                 // z6.d}, p7,
                                                                 // [x0, x27,
                                                                 // lsl #3]

  CLEANUP();
}

TEST(group_66) {
  SETUP();

  CMP_IO(dci(0xe4fbf4ae), R(), R(v14, v15, v16, v17, p5, x5));  // st4h {z14.h,
                                                                // z15.h, z16.h,
                                                                // z17.h}, p5,
                                                                // [x5, #-20,
                                                                // mul vl]
  CMP_IO(dci(0xe4fc78ef),
         R(),
         R(v15, v16, v17, v18, p6, x7, x28));  // st4h {z15.h, z16.h, z17.h,
                                               // z18.h}, p6, [x7, x28, lsl #1]
  CMP_IO(dci(0xe57bef4b), R(), R(v11, v12, v13, v14, p3, x26));  // st4w {z11.s,
                                                                 // z12.s,
                                                                 // z13.s,
                                                                 // z14.s}, p3,
                                                                 // [x26, #-20,
                                                                 // mul vl]
  CMP_IO(dci(0xe57a6b0a),
         R(),
         R(v10, v11, v12, v13, p2, x24, x26));    // st4w {z10.s, z11.s, z12.s,
                                                  // z13.s}, p2, [x24, x26, lsl
                                                  // #2]
  CMP_IO(dci(0xd93dd892), R(), R(x18, x4));       // stg x18, [x4, #-560]
  CMP_IO(dci(0xd93e965d), R(x18), R(x29, x18));   // stg x29, [x18], #-368
  CMP_IO(dci(0xd939bebc), R(x21), R(x28, x21));   // stg x28, [x21, #-1616]!
  CMP_IO(dci(0x693d513d), R(), R(x29, x20, x9));  // stgp x29, x20, [x9, #-96]
  CMP_IO(dci(0x689b5324),
         R(x25),
         R(x4, x20, x25));                       // stgp x4, x20, [x25], #864
  CMP_IO(dci(0x69bcfdf5), R(x15), R(x21, x15));  // stgp x21, xzr, [x15, #-112]!
  CMP_IO(dci(0x889a1ad7), R(), R(x23, x22));     // stllr w23, [x22]
  CMP_IO(dci(0xc89a253f), R(), R(x9));           // stllr xzr, [x9]
  CMP_IO(dci(0x089e1708), R(), R(x8, x24));      // stllrb w8, [x24]
  CMP_IO(dci(0x489d2eee), R(), R(x14, x23));     // stllrh w14, [x23]
  CMP_IO(dci(0x889d854a), R(), R(x10));          // stlr w10, [x10]
  CMP_IO(dci(0xc89d8fb3), R(), R(x19, x29));     // stlr x19, [x29]
  CMP_IO(dci(0x0899b2bc), R(), R(x28, x21));     // stlrb w28, [x21]
  CMP_IO(dci(0x4899f132), R(), R(x18, x9));      // stlrh w18, [x9]
  CMP_IO(dci(0x991c001d), R(), R(x29, x0));      // stlur w29, [x0, #-64]
  CMP_IO(dci(0xd91c2358), R(), R(x24, x26));     // stlur x24, [x26, #-62]
  CMP_IO(dci(0x191d620f), R(), R(x15, x16));     // stlurb w15, [x16, #-42]
  CMP_IO(dci(0x591cf1f5), R(), R(x21, x15));     // stlurh w21, [x15, #-49]
  CMP_IO(dci(0x8839dc60), R(x25), R(x0, x23, x3));  // stlxp w25, w0, w23, [x3]
  CMP_IO(dci(0xc839bd25), R(x25), R(x5, x15, x9));  // stlxp w25, x5, x15, [x9]
  CMP_IO(dci(0x8819dc60), R(x25), R(x0, x3));       // stlxr w25, w0, [x3]
  CMP_IO(dci(0xc819bd25), R(x25), R(x5, x9));       // stlxr w25, x5, [x9]
  CMP_IO(dci(0x081daeee), R(x29), R(x14, x23));     // stlxrb w29, w14, [x23]
  CMP_IO(dci(0x481e9708), R(x30), R(x8, x24));      // stlxrh w30, w8, [x24]
  CMP_IO(dci(0x283b48bb), R(), R(x27, x18, x5));    // stnp w27, w18, [x5, #-40]
  CMP_IO(dci(0xa81b3e52), R(), R(x18, x15, x18));  // stnp x18, x15, [x18, #432]
  CMP_IO(dci(0x6c1cc9e8), R(), R(v8, v18, x15));   // stnp d8, d18, [x15, #456]
  CMP_IO(dci(0xac3d3c6b), R(), R(v11, v15, x3));   // stnp q11, q15, [x3, #-96]

  CLEANUP();
}

TEST(group_67) {
  SETUP();

  CMP_IO(dci(0x2c3d2799), R(), R(v25, v9, x28));  // stnp s25, s9, [x28, #-24]
  CMP_IO(dci(0xe41c3516),
         R(),
         R(v22, p5, v8, x28));  // stnt1b {z22.d}, p5, [z8.d, x28]
  CMP_IO(dci(0xe45a2b0a),
         R(),
         R(v10, p2, v24, x26));  // stnt1b {z10.s}, p2, [z24.s, x26]
  CMP_IO(dci(0xe41be731),
         R(),
         R(v17, p1, x25));  // stnt1b {z17.b}, p1, [x25, #-5, mul vl]
  CMP_IO(dci(0xe41b7324),
         R(),
         R(v4, p4, x25, x27));  // stnt1b {z4.b}, p4, [x25, x27]
  CMP_IO(dci(0xe59c2044),
         R(),
         R(v4, p0, v2, x28));  // stnt1d {z4.d}, p0, [z2.d, x28]
  CMP_IO(dci(0xe59be5a7),
         R(),
         R(v7, p1, x13));  // stnt1d {z7.d}, p1, [x13, #-5, mul vl]
  CMP_IO(dci(0xe59b67f6),
         R(),
         R(v22, p1, sp, x27));  // stnt1d {z22.d}, p1, [sp, x27, lsl #3]
  CMP_IO(dci(0xe49c36a0),
         R(),
         R(v0, p5, v21, x28));  // stnt1h {z0.d}, p5, [z21.d, x28]
  CMP_IO(dci(0xe4db3324),
         R(),
         R(v4, p4, v25, x27));  // stnt1h {z4.s}, p4, [z25.s, x27]
  CMP_IO(dci(0xe49bef4b),
         R(),
         R(v11, p3, x26));  // stnt1h {z11.h}, p3, [x26, #-5, mul vl]
  CMP_IO(dci(0xe49a6b0a),
         R(),
         R(v10, p2, x24, x26));  // stnt1h {z10.h}, p2, [x24, x26, lsl #1]
  CMP_IO(dci(0xe51d2799),
         R(),
         R(v25, p1, v28, x29));  // stnt1w {z25.d}, p1, [z28.d, x29]
  CMP_IO(dci(0xe55b30d5),
         R(),
         R(v21, p4, v6, x27));  // stnt1w {z21.s}, p4, [z6.s, x27]
  CMP_IO(dci(0xe51bf4ae),
         R(),
         R(v14, p5, x5));  // stnt1w {z14.s}, p5, [x5, #-5, mul vl]
  CMP_IO(dci(0xe51c78ef),
         R(),
         R(v15, p6, x7, x28));  // stnt1w {z15.s}, p6, [x7, x28, lsl #2]
  CMP_IO(dci(0x293bda79), R(), R(x25, x22, x19));  // stp w25, w22, [x19, #-36]
  CMP_IO(dci(0x289cc9e8), R(x15), R(x8, x18, x15));  // stp w8, w18, [x15], #228
  CMP_IO(dci(0x29bb8731),
         R(x25),
         R(x17, x1, x25));                         // stp w17, w1, [x25, #-36]!
  CMP_IO(dci(0xa91bf9b4), R(), R(x20, x30, x13));  // stp x20, x30, [x13, #440]
  CMP_IO(dci(0xa8bcaaad),
         R(x21),
         R(x13, x10, x21));  // stp x13, x10, [x21], #-56
  CMP_IO(dci(0xa99ba66c),
         R(x19),
         R(x12, x9, x19));                          // stp x12, x9, [x19, #440]!
  CMP_IO(dci(0x6d1d5ba6), R(), R(v6, v22, x29));    // stp d6, d22, [x29, #464]
  CMP_IO(dci(0x6cbb48bb), R(x5), R(v27, v18, x5));  // stp d27, d18, [x5], #-80
  CMP_IO(dci(0x6d9d085e), R(x2), R(v30, v2, x2));   // stp d30, d2, [x2, #464]!
  CMP_IO(dci(0xad3c8109), R(), R(v9, v0, x8));      // stp q9, q0, [x8, #-112]
  CMP_IO(dci(0xac9c2358), R(x26), R(v24, v8, x26));  // stp q24, q8, [x26], #896
  CMP_IO(dci(0xadbc2dc1),
         R(x14),
         R(v1, v11, x14));                        // stp q1, q11, [x14, #-128]!
  CMP_IO(dci(0x2d3c95db), R(), R(v27, v5, x14));  // stp s27, s5, [x14, #-28]
  CMP_IO(dci(0x2c9c0e86), R(x20), R(v6, v3, x20));  // stp s6, s3, [x20], #224
  CMP_IO(dci(0x2dbc4293),
         R(x20),
         R(v19, v16, x20));                 // stp s19, s16, [x20, #-32]!
  CMP_IO(dci(0xb81b765f), R(x18), R(x18));  // str wzr, [x18], #-73

  CLEANUP();
}

TEST(group_68) {
  SETUP();

  CMP_IO(dci(0xb81cdeba), R(x21), R(x26, x21));  // str w26, [x21, #-51]!
  CMP_IO(dci(0xb91ba66c), R(), R(x12, x19));     // str w12, [x19, #7076]
  CMP_IO(dci(0xb83c29c1),
         R(),
         R(x1, x14, x28));                     // str w1, [x14, x28, undefined]
  CMP_IO(dci(0xf81b67f6), R(sp), R(x22, sp));  // str x22, [sp], #-74
  CMP_IO(dci(0xf81ced23), R(x9), R(x3, x9));   // str x3, [x9, #-50]!
  CMP_IO(dci(0xf93b8731), R(), R(x17, x25));   // str x17, [x25, #30472]
  CMP_IO(dci(0xf83c48fc), R(), R(x7, x28));    // str x28, [x7, w28, uxtw]
  CMP_IO(dci(0x3c1cb516), R(x8), R(v22, x8));  // str b22, [x8], #-53
  CMP_IO(dci(0x3c1b9c03), R(x0), R(v3, x0));   // str b3, [x0, #-71]!
  CMP_IO(dci(0x3d3cd451), R(), R(v17, x2));    // str b17, [x2, #3893]
  CMP_IO(dci(0x3c3a3bdc),
         R(),
         R(v28, x30, x26));  // str b28, [x30, x26, undefined #0]
  CMP_IO(dci(0x3c3c69e8), R(), R(v8, x15, x28));  // str b8, [x15, x28]
  CMP_IO(dci(0xfc1ca444), R(x2), R(v4, x2));      // str d4, [x2], #-54
  CMP_IO(dci(0xfc1bbcd5), R(x6), R(v21, x6));     // str d21, [x6, #-69]!
  CMP_IO(dci(0xfd3d12c7), R(), R(v7, x22));       // str d7, [x22, #31264]
  CMP_IO(dci(0xfc3b18ae),
         R(),
         R(v14, x5, x27));  // str d14, [x5, x27, undefined #3]
  CMP_IO(dci(0x7c1c76a0), R(x21), R(v0, x21));   // str h0, [x21], #-57
  CMP_IO(dci(0x7c1bde79), R(x19), R(v25, x19));  // str h25, [x19, #-67]!
  CMP_IO(dci(0x7d3d3c6b), R(), R(v11, x3));      // str h11, [x3, #7838]
  CMP_IO(dci(0x7c3b3a52),
         R(),
         R(v18, x18, x27));  // str h18, [x18, x27, undefined #1]
  CMP_IO(dci(0xe59e156e), R(), R(p14, x11));     // str p14, [x11, #245, mul vl]
  CMP_IO(dci(0x3c9d660f), R(x16), R(v15, x16));  // str q15, [x16], #-42
  CMP_IO(dci(0x3c9aef0a), R(x24), R(v10, x24));  // str q10, [x24, #-82]!
  CMP_IO(dci(0x3d9c4cfc), R(), R(v28, x7));      // str q28, [x7, #28976]
  CMP_IO(dci(0x3cbb8b31),
         R(),
         R(v17, x25, x27));                    // str q17, [x25, x27, undefined]
  CMP_IO(dci(0xbc1d553d), R(x9), R(v29, x9));  // str s29, [x9], #-43
  CMP_IO(dci(0xbc1affdc), R(x30), R(v28, x30));  // str s28, [x30, #-81]!
  CMP_IO(dci(0xbd1c382a), R(), R(v10, x1));      // str s10, [x1, #7224]
  CMP_IO(dci(0xbc3b9803),
         R(),
         R(v3, x0, x27));                     // str s3, [x0, x27, undefined #2]
  CMP_IO(dci(0xe59e5d98), R(), R(v24, x12));  // str z24, [x12, #247, mul vl]
  CMP_IO(dci(0x381ca6ad), R(x21), R(x13, x21));  // strb w13, [x21], #-54
  CMP_IO(dci(0x381bae6c), R(x19), R(x12, x19));  // strb w12, [x19, #-70]!

  CLEANUP();
}

TEST(group_69) {
  SETUP();

  CMP_IO(dci(0x391cdeba), R(), R(x26, x21));  // strb w26, [x21, #1847]
  CMP_IO(dci(0x383ca9c1),
         R(),
         R(x1, x14, x28));  // strb w1, [x14, x28, undefined]
  CMP_IO(dci(0x383b798d),
         R(),
         R(x13, x12, x27));                    // strb w13, [x12, x27, lsl #0]
  CMP_IO(dci(0x781c6437), R(x1), R(x23, x1));  // strh w23, [x1], #-58
  CMP_IO(dci(0x781bece2), R(x7), R(x2, x7));   // strh w2, [x7, #-66]!
  CMP_IO(dci(0x791d46d4), R(), R(x20, x22));   // strh w20, [x22, #3746]
  CMP_IO(dci(0x783b3be9),
         R(),
         R(x9, sp, x27));                    // strh w9, [sp, x27, undefined #1]
  CMP_IO(dci(0xb81b63f6), R(), R(x22, sp));  // stur w22, [sp, #-74]
  CMP_IO(dci(0xf81b725f), R(), R(x18));      // stur xzr, [x18, #-73]
  CMP_IO(dci(0x3c1cb37f), R(), R(v31, x27));         // stur b31, [x27, #-53]
  CMP_IO(dci(0xfc1c8109), R(), R(v9, x8));           // stur d9, [x8, #-56]
  CMP_IO(dci(0x7c1c5365), R(), R(v5, x27));          // stur h5, [x27, #-59]
  CMP_IO(dci(0x3c9d53a6), R(), R(v6, x29));          // stur q6, [x29, #-43]
  CMP_IO(dci(0xbc1d7078), R(), R(v24, x3));          // stur s24, [x3, #-41]
  CMP_IO(dci(0x381cc1e8), R(), R(x8, x15));          // sturb w8, [x15, #-52]
  CMP_IO(dci(0x781c61ce), R(), R(x14));              // sturh w14, [x14, #-58]
  CMP_IO(dci(0x883d6d64), R(x29), R(x4, x27, x11));  // stxp w29, w4, w27, [x11]
  CMP_IO(dci(0xc83d4e29), R(x29), R(x9, x19, x17));  // stxp w29, x9, x19, [x17]
  CMP_IO(dci(0x881d6d64), R(x29), R(x4, x11));       // stxr w29, w4, [x11]
  CMP_IO(dci(0xc81d4e29), R(x29), R(x9, x17));       // stxr w29, x9, [x17]
  CMP_IO(dci(0x08197132), R(x25), R(x18, x9));       // stxrb w25, w18, [x9]
  CMP_IO(dci(0x481932bc), R(x25), R(x28, x21));      // stxrh w25, w28, [x21]
  CMP_IO(dci(0xd9faeb0a), R(), R(x10, x24));         // stz2g x10, [x24, #-1312]
  CMP_IO(dci(0xd9fbb4d5), R(x6), R(x21, x6));        // stz2g x21, [x6], #-1104
  CMP_IO(dci(0xd9fcac44), R(x2), R(x4, x2));         // stz2g x4, [x2, #-864]!
  CMP_IO(dci(0xd97a594c), R(), R(x12, x10));         // stzg x12, [x10, #-1456]
  CMP_IO(dci(0xd979d5f7), R(x15), R(x23, x15));      // stzg x23, [x15], #-1584
  CMP_IO(dci(0xd97e7f22), R(x25), R(x2, x25));       // stzg x2, [x25, #-400]!
  CMP_IO(dci(0x4b3b63f6), R(x22), R(sp, x27));       // sub w22, wsp, x27
  CMP_IO(dci(0x515b67f6), R(x22), R(sp));  // sub w22, wsp, #0x6d9000 (7180288)
  CMP_IO(dci(0x4b1b0a45), R(x5), R(x18, x27));  // sub w5, w18, w27, lsl #2
  CMP_IO(dci(0xcb3b6a5f), R(sp), R(x18, x27));  // sub sp, x18, x27, lsl #2

  CLEANUP();
}

TEST(group_70) {
  SETUP();

  CMP_IO(dci(0xd13b725f), R(sp), R(x18));        // sub sp, x18, #0xedc (3804)
  CMP_IO(dci(0xcb5b2980), R(x0), R(x12, x27));   // sub x0, x12, x27, lsr #10
  CMP_IO(dci(0x2e3d861c), R(v28), R(v16, v29));  // sub v28.8b, v16.8b, v29.8b
  CMP_IO(dci(0x7efe84c6), R(v6), R(v6, v30));    // sub d6, d6, d30
  CMP_IO(dci(0x048115db),
         R(v27),
         R(p5, v27, v14));                // sub z27.s, p5/m, z27.s, z14.s
  CMP_IO(dci(0x2521dfe7), R(v7), R(v7));  // sub z7.b, z7.b, #255
  CMP_IO(dci(0x04fd06ee), R(v14), R(v23, v29));  // sub z14.d, z23.d, z29.d
  CMP_IO(dci(0xd1bc8109), R(x9), R(x8));         // subg x9, x8, #960, #0
  CMP_IO(dci(0x0e2161bd), R(v29), R(v13, v1));   // subhn v29.8b, v13.8h, v1.8h
  CMP_IO(dci(0x457971ea), R(v10), R(v15, v25));  // subhnb z10.b, z15.h, z25.h
  CMP_IO(dci(0x45ba76f0),
         R(v16),
         R(v16, v23, v26));                      // subhnt z16.h, z23.s, z26.s
  CMP_IO(dci(0x9ad9038e), R(x14), R(x28, x25));  // subp x14, x28, x25
  CMP_IO(dci(0xbade03e7), R(x7), R(sp, x30));    // subps x7, sp, x30
  CMP_IO(dci(0x04c31a79),
         R(v25),
         R(p6, v25, v19));                // subr z25.d, p6/m, z25.d, z19.d
  CMP_IO(dci(0x2563fd25), R(v5), R(v5));  // subr z5.h, z5.h, #233, lsl #8
  CMP_IO(dci(0x6b5d1d30), R(x16), R(x9, x29));  // subs w16, w9, w29, lsr #7
  CMP_IO(dci(0x6b3b67f6), R(x22), R(sp, x27));  // subs w22, wsp, x27, lsl #1
  CMP_IO(dci(0x715b67f6), R(x22), R(sp));  // subs w22, wsp, #0x6d9000 (7180288)
  CMP_IO(dci(0xeb5cfdf5), R(x21), R(x15, x28));  // subs x21, x15, x28, lsr #63
  CMP_IO(dci(0xeb3b725f), R(), R(x18, x27));     // cmp x18, x27, lsl #4
  CMP_IO(dci(0xf13b725f), R(), R(x18));          // cmp x18, #0xedc (3804)
  CMP_IO(dci(0x0f1afa1e),
         R(v30),
         R(v30, v16, v26));  // sudot v30.2s, v16.8b, v26.4b[2]
  CMP_IO(dci(0x44bc1ea0), R(v0), R(v0, v21, v4));  // sudot z0.s, z21.b, z4.b[3]
  CMP_IO(dci(0x05713932), R(v18), R(v9));          // sunpkhi z18.h, z9.b
  CMP_IO(dci(0x05b038dd), R(v29), R(v6));          // sunpklo z29.s, z6.h
  CMP_IO(dci(0x0e603b22), R(v2), R(v25));          // suqadd v2.4h, v25.4h
  CMP_IO(dci(0x5ea03bc0), R(v0), R(v30));          // suqadd s0, s30
  CMP_IO(dci(0x44dc9a79),
         R(v25),
         R(p6, v25, v19));  // suqadd z25.d, p6/m, z25.d, z19.d
  CMP_IO(dci(0xb83e8315), R(x21), R(x30, x24));  // swp w30, w21, [x24]
  CMP_IO(dci(0xf83e8250), R(x16), R(x30, x18));  // swp x30, x16, [x18]
  CMP_IO(dci(0xb8ba821e), R(x30), R(x26, x16));  // swpa w26, w30, [x16]
  CMP_IO(dci(0xf8ba8087), R(x7), R(x26, x4));    // swpa x26, x7, [x4]

  CLEANUP();
}

TEST(group_71) {
  SETUP();

  CMP_IO(dci(0x38be8250), R(x16), R(x30, x18));      // swpab w30, w16, [x18]
  CMP_IO(dci(0x78be80c6), R(x6), R(x30, x6));        // swpah w30, w6, [x6]
  CMP_IO(dci(0xb8fe8322), R(x2), R(x30, x25));       // swpal w30, w2, [x25]
  CMP_IO(dci(0xf8fe818b), R(x11), R(x30, x12));      // swpal x30, x11, [x12]
  CMP_IO(dci(0x38fa8094), R(x20), R(x26, x4));       // swpalb w26, w20, [x4]
  CMP_IO(dci(0x78fa807a), R(x26), R(x26, x3));       // swpalh w26, w26, [x3]
  CMP_IO(dci(0x383a8087), R(x7), R(x26, x4));        // swpb w26, w7, [x4]
  CMP_IO(dci(0x783a82fd), R(x29), R(x26, x23));      // swph w26, w29, [x23]
  CMP_IO(dci(0xb87a8159), R(x25), R(x26, x10));      // swpl w26, w25, [x10]
  CMP_IO(dci(0xf87a8094), R(x20), R(x26, x4));       // swpl x26, x20, [x4]
  CMP_IO(dci(0x387e818b), R(x11), R(x30, x12));      // swplb w30, w11, [x12]
  CMP_IO(dci(0x787e8171), R(x17), R(x30, x11));      // swplh w30, w17, [x11]
  CMP_IO(dci(0x0450ab72), R(v18), R(v18, p2, v27));  // sxtb z18.h, p2/m, z27.h
  CMP_IO(dci(0x0492b8cf), R(v15), R(v15, p6, v6));   // sxth z15.s, p6/m, z6.s
  CMP_IO(dci(0x04d4b93e), R(v30), R(v30, p6, v9));   // sxtw z30.d, p6/m, z9.d
  CMP_IO(dci(0x0e1b034b),
         R(v11),
         R(v26, v27));  // tbl v11.8b, {v26.16b}, v27.8b
  CMP_IO(dci(0x0e1b234b),
         R(v11),
         R(v26, v27, v27));  // tbl v11.8b, {v26.16b, v27.16b}, v27.8b
  CMP_IO(dci(0x0e1b434b),
         R(v11),
         R(v26,
           v27,
           v28,
           v27));  // tbl v11.8b, {v26.16b, v27.16b, v28.16b}, v27.8b
  CMP_IO(dci(0x0e1b634b),
         R(v11),
         R(v26,
           v27,
           v28,
           v29,
           v27));  // tbl v11.8b, {v26.16b, v27.16b, v28.16b, v29.16b}, v27.8b
  CMP_IO(dci(0x05fa321e), R(v30), R(v16, v26));  // tbl z30.d, {z16.d}, z26.d
  CMP_IO(dci(0x053a2887),
         R(v7),
         R(v4, v5, v26));               // tbl z7.b, {z4.b, z5.b}, z26.b
  CMP_IO(dci(0xb79e1708), R(), R(x8));  // tbnz x8, #51, #-0x3d20
  CMP_IO(dci(0x0e1b125f),
         R(v31),
         R(v31, v18, v27));  // tbx v31.8b, {v18.16b}, v27.8b
  CMP_IO(dci(0x0e1b325f),
         R(v31),
         R(v31, v18, v19, v27));  // tbx v31.8b, {v18.16b, v19.16b}, v27.8b
  CMP_IO(dci(0x0e1b525f),
         R(v31),
         R(v31,
           v18,
           v19,
           v20,
           v27));  // tbx v31.8b, {v18.16b, v19.16b, v20.16b}, v27.8b
  CMP_IO(dci(0x0e1b725f),
         R(v31),
         R(v31, v18, v19, v20, v21, v27));  // tbx v31.8b, {v18.16b, v19.16b,
                                            // v20.16b, v21.16b}, v27.8b
  CMP_IO(dci(0x05be2f08), R(v8), R(v8, v24, v30));  // tbx z8.s, z24.s, z30.s
  CMP_IO(dci(0x36599dea), R(), R(x10));             // tbz w10, #11, #+0x33bc
  CMP_IO(dci(0x0e1c2b7f), R(v31), R(v27, v28));  // trn1 v31.8b, v27.8b, v28.8b
  CMP_IO(dci(0x05ac5123), R(p3), R(p9, p12));    // trn1 p3.s, p9.s, p12.s
  CMP_IO(dci(0x05fd7399), R(v25), R(v28, v29));  // trn1 z25.d, z28.d, z29.d
  CMP_IO(dci(0x4ec86a86), R(v6), R(v20, v8));    // trn2 v6.2d, v20.2d, v8.2d

  CLEANUP();
}

TEST(group_72) {
  SETUP();

  CMP_IO(dci(0x056c54aa), R(p10), R(p5, p12));   // trn2 p10.h, p5.h, p12.h
  CMP_IO(dci(0x053d76d4), R(v20), R(v22, v29));  // trn2 z20.b, z22.b, z29.b
  CMP_IO(dci(0x6e787f65),
         R(v5),
         R(v5, v27, v24));  // uaba v5.8h, v27.8h, v24.8h
  CMP_IO(dci(0x455dffcd),
         R(v13),
         R(v13, v30, v29));  // uaba z13.h, z30.h, z29.h
  CMP_IO(dci(0x2e3a514c),
         R(v12),
         R(v12, v10, v26));  // uabal v12.8h, v10.8b, v26.8b
  CMP_IO(dci(0x459dcbb3),
         R(v19),
         R(v19, v29, v29));  // uabalb z19.s, z29.h, z29.h
  CMP_IO(dci(0x455ecf15),
         R(v21),
         R(v21, v24, v30));                      // uabalt z21.h, z24.b, z30.b
  CMP_IO(dci(0x2ead76ee), R(v14), R(v23, v13));  // uabd v14.2s, v23.2s, v13.2s
  CMP_IO(dci(0x044d041d),
         R(v29),
         R(v29, p1, v29, v0));                  // uabd z29.h, p1/m, z29.h, z0.h
  CMP_IO(dci(0x6e3a7087), R(v7), R(v4, v26));   // uabdl2 v7.8h, v4.16b, v26.16b
  CMP_IO(dci(0x45da3966), R(v6), R(v11, v26));  // uabdlb z6.d, z11.s, z26.s
  CMP_IO(dci(0x459e3cdd), R(v29), R(v6, v30));  // uabdlt z29.s, z6.h, z30.h
  CMP_IO(dci(0x6ea06bf5), R(v21), R(v21, v31));    // uadalp v21.2d, v31.4s
  CMP_IO(dci(0x4445b202), R(v2), R(v2, p4, v16));  // uadalp z2.h, p4/m, z16.b
  CMP_IO(dci(0x6e780365), R(v5), R(v27, v24));   // uaddl2 v5.4s, v27.8h, v24.8h
  CMP_IO(dci(0x45ce0b2c), R(v12), R(v25, v14));  // uaddlb z12.d, z25.s, z14.s
  CMP_IO(dci(0x6e6028d7), R(v23), R(v6));        // uaddlp v23.4s, v6.8h
  CMP_IO(dci(0x45da0ed6), R(v22), R(v22, v26));  // uaddlt z22.d, z22.s, z26.s
  CMP_IO(dci(0x6e7038af), R(v15), R(v5));        // uaddlv s15, v5.8h
  CMP_IO(dci(0x040138ef), R(v15), R(p6, v7));    // uaddv d15, p6, z7.b
  CMP_IO(dci(0x6e781365), R(v5), R(v27, v24));   // uaddw2 v5.4s, v27.4s, v24.8h
  CMP_IO(dci(0x45d94b81), R(v1), R(v28, v25));   // uaddwb z1.d, z28.d, z25.s
  CMP_IO(dci(0x45984fa3), R(v3), R(v29, v24));   // uaddwt z3.s, z29.s, z24.h
  CMP_IO(dci(0x53111bde), R(x30), R(x30));       // ubfiz w30, w30, #15, #7
  CMP_IO(dci(0xd35a6e1e), R(x30), R(x16));       // ubfx x30, x16, #26, #2
  CMP_IO(dci(0x6e61d80f), R(v15), R(v0));        // ucvtf v15.2d, v0.2d
  CMP_IO(dci(0x2e79d86d), R(v13), R(v3));        // ucvtf v13.4h, v3.4h
  CMP_IO(dci(0x6f45e6ee), R(v14), R(v23));       // ucvtf v14.2d, v23.2d, #59
  CMP_IO(dci(0x7e21dabc), R(v28), R(v21));       // ucvtf s28, s21
  CMP_IO(dci(0x7e79dae3), R(v3), R(v23));        // ucvtf h3, h23
  CMP_IO(dci(0x7f5ee7f4), R(v20), R(v31));       // ucvtf d20, d31, #34
  CMP_IO(dci(0x1e43b202), R(v2), R(x16));        // ucvtf d2, w16, #20

  CLEANUP();
}

TEST(group_73) {
  SETUP();

  CMP_IO(dci(0x1e6303a6), R(v6), R(x29));            // ucvtf d6, w29
  CMP_IO(dci(0x9e433c6b), R(v11), R(x3));            // ucvtf d11, x3, #49
  CMP_IO(dci(0x9e63020f), R(v15), R(x16));           // ucvtf d15, x16
  CMP_IO(dci(0x1ec3885e), R(v30), R(x2));            // ucvtf h30, w2, #30
  CMP_IO(dci(0x1ee302ba), R(v26), R(x21));           // ucvtf h26, w21
  CMP_IO(dci(0x9ec312c7), R(v7), R(x22));            // ucvtf h7, x22, #60
  CMP_IO(dci(0x9ee30123), R(v3), R(x9));             // ucvtf h3, x9
  CMP_IO(dci(0x1e03bf7f), R(v31), R(x27));           // ucvtf s31, w27, #17
  CMP_IO(dci(0x1e2301db), R(v27), R(x14));           // ucvtf s27, w14
  CMP_IO(dci(0x9e03b516), R(v22), R(x8));            // ucvtf s22, x8, #19
  CMP_IO(dci(0x9e230372), R(v18), R(x27));           // ucvtf s18, x27
  CMP_IO(dci(0x6553bc03), R(v3), R(v3, p7, v0));     // ucvtf z3.h, p7/m, z0.h
  CMP_IO(dci(0x65d1a799), R(v25), R(v25, p1, v28));  // ucvtf z25.d, p1/m, z28.s
  CMP_IO(dci(0x6555a7f6), R(v22), R(v22, p1, v31));  // ucvtf z22.h, p1/m, z31.s
  CMP_IO(dci(0x6595a9e8), R(v8), R(v8, p2, v15));    // ucvtf z8.s, p2/m, z15.s
  CMP_IO(dci(0x65d7bd30), R(v16), R(v16, p7, v9));   // ucvtf z16.d, p7/m, z9.d
  CMP_IO(dci(0x6557b573), R(v19), R(v19, p5, v11));  // ucvtf z19.h, p5/m, z11.d
  CMP_IO(dci(0x65d5adc1), R(v1), R(v1, p3, v14));    // ucvtf z1.s, p3/m, z14.d
  CMP_IO(dci(0x0000919a), R(), R());                 // udf #0x919a
  CMP_IO(dci(0x1ade0b08), R(x8), R(x24, x30));       // udiv w8, w24, w30
  CMP_IO(dci(0x9add0bcd), R(x13), R(x30, x29));      // udiv x13, x30, x29
  CMP_IO(dci(0x04d500a1), R(v1), R(p0, v1, v5));  // udiv z1.d, p0/m, z1.d, z5.d
  CMP_IO(dci(0x04970923),
         R(v3),
         R(p2, v3, v9));  // udivr z3.s, p2/m, z3.s, z9.s
  CMP_IO(dci(0x2f3de957),
         R(v23),
         R(v23, v10, v29));  // udot v23.2s, v10.8b, v29.4b[3]
  CMP_IO(dci(0x6e9c95e8),
         R(v8),
         R(v8, v15, v28));  // udot v8.4s, v15.16b, v28.16b
  CMP_IO(dci(0x448d04ce), R(v14), R(v14, v6, v13));  // udot z14.s, z6.b, z13.b
  CMP_IO(dci(0x44fa07dc),
         R(v28),
         R(v28, v30, v10));  // udot z28.d, z30.h, z10.h[1]
  CMP_IO(dci(0x44bb04d5), R(v21), R(v21, v6, v3));  // udot z21.s, z6.b, z3.b[3]
  CMP_IO(dci(0x6e3e07f4),
         R(v20),
         R(v31, v30));  // uhadd v20.16b, v31.16b, v30.16b
  CMP_IO(dci(0x44918923),
         R(v3),
         R(p2, v3, v9));                         // uhadd z3.s, p2/m, z3.s, z9.s
  CMP_IO(dci(0x2e3e272f), R(v15), R(v25, v30));  // uhsub v15.8b, v25.8b, v30.8b
  CMP_IO(dci(0x44939d30),
         R(v16),
         R(p7, v16, v9));  // uhsub z16.s, p7/m, z16.s, z9.s

  CLEANUP();
}

TEST(group_74) {
  SETUP();

  CMP_IO(dci(0x44d794ae),
         R(v14),
         R(p5, v14, v5));  // uhsubr z14.d, p5/m, z14.d, z5.d
  CMP_IO(dci(0x9bbd7ae1), R(x1), R(x23, x29, x30));  // umaddl x1, w23, w29, x30
  CMP_IO(dci(0x6e3965f7),
         R(v23),
         R(v15, v25));  // umax v23.16b, v15.16b, v25.16b
  CMP_IO(dci(0x04891cc8), R(v8), R(p7, v8, v6));  // umax z8.s, p7/m, z8.s, z6.s
  CMP_IO(dci(0x2529d94c), R(v12), R(v12));        // umax z12.b, z12.b, #202
  CMP_IO(dci(0x6e3ea7e7),
         R(v7),
         R(v31, v30));  // umaxp v7.16b, v31.16b, v30.16b
  CMP_IO(dci(0x4495b078),
         R(v24),
         R(p4, v24, v3));                      // umaxp z24.s, p4/m, z24.s, z3.s
  CMP_IO(dci(0x2e30aa04), R(v4), R(v16));      // umaxv b4, v16.8b
  CMP_IO(dci(0x04093324), R(v4), R(p4, v25));  // umaxv b4, p4, z25.b
  CMP_IO(dci(0x6eae6d0d), R(v13), R(v8, v14));  // umin v13.4s, v8.4s, v14.4s
  CMP_IO(dci(0x044b119a),
         R(v26),
         R(p4, v26, v12));                  // umin z26.h, p4/m, z26.h, z12.h
  CMP_IO(dci(0x25ebdad6), R(v22), R(v22));  // umin z22.d, z22.d, #214
  CMP_IO(dci(0x6e2bad9d),
         R(v29),
         R(v12, v11));  // uminp v29.16b, v12.16b, v11.16b
  CMP_IO(dci(0x4457b82a),
         R(v10),
         R(p6, v10, v1));                  // uminp z10.h, p6/m, z10.h, z1.h
  CMP_IO(dci(0x2e71ab87), R(v7), R(v28));  // uminv h7, v28.4h
  CMP_IO(dci(0x04cb3b3e), R(v30), R(p6, v25));  // uminv d30, p6, z25.d
  CMP_IO(dci(0x2e7a807b),
         R(v27),
         R(v27, v3, v26));  // umlal v27.4s, v3.4h, v26.4h
  CMP_IO(dci(0x2f822b1a),
         R(v26),
         R(v26, v24, v2));  // umlal v26.2d, v24.2s, v2.s[2]
  CMP_IO(dci(0x44dd4ae1), R(v1), R(v1, v23, v29));  // umlalb z1.d, z23.s, z29.s
  CMP_IO(dci(0x44fb9803),
         R(v3),
         R(v3, v0, v11));  // umlalb z3.d, z0.s, z11.s[3]
  CMP_IO(dci(0x44bb90ae),
         R(v14),
         R(v14, v5, v3));  // umlalb z14.s, z5.h, z3.h[6]
  CMP_IO(dci(0x44824f83), R(v3), R(v3, v28, v2));  // umlalt z3.s, z28.h, z2.h
  CMP_IO(dci(0x44fb9e45),
         R(v5),
         R(v5, v18, v11));  // umlalt z5.d, z18.s, z11.s[3]
  CMP_IO(dci(0x44bb966c),
         R(v12),
         R(v12, v19, v3));  // umlalt z12.s, z19.h, z3.h[6]
  CMP_IO(dci(0x6e39a2bc),
         R(v28),
         R(v28, v21, v25));  // umlsl2 v28.8h, v21.16b, v25.16b
  CMP_IO(dci(0x6f9a6ba8),
         R(v8),
         R(v8, v29, v26));  // umlsl2 v8.2d, v29.4s, v26.s[2]
  CMP_IO(dci(0x449e5a43), R(v3), R(v3, v18, v30));  // umlslb z3.s, z18.h, z30.h
  CMP_IO(dci(0x44fab0a1),
         R(v1),
         R(v1, v5, v10));  // umlslb z1.d, z5.s, z10.s[2]
  CMP_IO(dci(0x44bbb010),
         R(v16),
         R(v16, v0, v3));  // umlslb z16.s, z0.h, z3.h[6]
  CMP_IO(dci(0x445d5c85), R(v5), R(v5, v4, v29));  // umlslt z5.h, z4.b, z29.b
  CMP_IO(dci(0x44fbb5a7),
         R(v7),
         R(v7, v13, v11));  // umlslt z7.d, z13.s, z11.s[2]
  CMP_IO(dci(0x44babf0a),
         R(v10),
         R(v10, v24, v2));  // umlslt z10.s, z24.h, z2.h[7]

  CLEANUP();
}

TEST(group_75) {
  SETUP();

  CMP_IO(dci(0x6e9ca4ef),
         R(v15),
         R(v15, v7, v28));  // ummla v15.4s, v7.16b, v28.16b
  CMP_IO(dci(0x45d99abc),
         R(v28),
         R(v28, v21, v25));                 // ummla z28.s, z21.b, z25.b
  CMP_IO(dci(0x0e1b3fdc), R(x28), R(v30));  // umov w28, v30.b[13]
  CMP_IO(dci(0x4e183fdc), R(x28), R(v30));  // mov x28, v30.d[1]
  CMP_IO(dci(0x9bbdaeee),
         R(x14),
         R(x23, x29, x11));                      // umsubl x14, w23, w29, x11
  CMP_IO(dci(0x9bd97b9b), R(x27), R(x28, x25));  // umulh x27, x28, x25
  CMP_IO(dci(0x049316a0),
         R(v0),
         R(p5, v0, v21));                       // umulh z0.s, p5/m, z0.s, z21.s
  CMP_IO(dci(0x04fd6e29), R(v9), R(v17, v29));  // umulh z9.d, z17.d, z29.d
  CMP_IO(dci(0x2eb4c277), R(v23), R(v19, v20));  // umull v23.2d, v19.2s, v20.2s
  CMP_IO(dci(0x2f59a132), R(v18), R(v9, v9));    // umull v18.4s, v9.4h, v9.h[1]
  CMP_IO(dci(0x454279fb), R(v27), R(v15, v2));   // umullb z27.h, z15.b, z2.b
  CMP_IO(dci(0x44fdd13d), R(v29), R(v9, v13));   // umullb z29.d, z9.s, z13.s[2]
  CMP_IO(dci(0x44bcd044), R(v4), R(v2, v4));     // umullb z4.s, z2.h, z4.h[6]
  CMP_IO(dci(0x45da7d59), R(v25), R(v10, v26));  // umullt z25.d, z10.s, z26.s
  CMP_IO(dci(0x44fcd5db),
         R(v27),
         R(v14, v12));                         // umullt z27.d, z14.s, z12.s[2]
  CMP_IO(dci(0x44bddfa6), R(v6), R(v29, v5));  // umullt z6.s, z29.h, z5.h[7]
  CMP_IO(dci(0x6ebd0fb3), R(v19), R(v29, v29));  // uqadd v19.4s, v29.4s, v29.4s
  CMP_IO(dci(0x7e7e0e5d), R(v29), R(v18, v30));  // uqadd h29, h18, h30
  CMP_IO(dci(0x44198044),
         R(v4),
         R(p0, v4, v2));                    // uqadd z4.b, p0/m, z4.b, z2.b
  CMP_IO(dci(0x25a5ea50), R(v16), R(v16));  // uqadd z16.s, z16.s, #82, lsl #8
  CMP_IO(dci(0x047d1485), R(v5), R(v4, v29));   // uqadd z5.h, z4.h, z29.h
  CMP_IO(dci(0x042cfeba), R(x26), R(x26));      // uqdecb w26, #0x15, mul #13
  CMP_IO(dci(0x043bff24), R(x4), R(x4));        // uqdecb x4, #0x19, mul #12
  CMP_IO(dci(0x04edfd30), R(x16), R(x16));      // uqdecd w16, vl16, mul #14
  CMP_IO(dci(0x04fbfff6), R(x22), R(x22));      // uqdecd x22, all, mul #12
  CMP_IO(dci(0x04e9cf9b), R(v27), R(v27));      // uqdecd z27.d, #0x1c, mul #10
  CMP_IO(dci(0x046dfed4), R(x20), R(x20));      // uqdech w20, #0x16, mul #14
  CMP_IO(dci(0x047aff0a), R(x10), R(x10));      // uqdech x10, #0x18, mul #11
  CMP_IO(dci(0x0469cdf7), R(v23), R(v23));      // uqdech z23.h, #0xf, mul #10
  CMP_IO(dci(0x25ab8838), R(x24), R(x24, p1));  // uqdecp w24, p1.s
  CMP_IO(dci(0x25eb8cee), R(x14), R(x14, p7));  // uqdecp x14, p7.d
  CMP_IO(dci(0x25ab8093), R(v19), R(v19, p4));  // uqdecp z19.s, p4

  CLEANUP();
}

TEST(group_76) {
  SETUP();

  CMP_IO(dci(0x04acff65), R(x5), R(x5));        // uqdecw w5, #0x1b, mul #13
  CMP_IO(dci(0x04bcfcef), R(x15), R(x15));      // uqdecw x15, vl7, mul #13
  CMP_IO(dci(0x04aacfc2), R(v2), R(v2));        // uqdecw z2.s, mul3, mul #11
  CMP_IO(dci(0x042df530), R(x16), R(x16));      // uqincb w16, vl16, mul #14
  CMP_IO(dci(0x043bf7f6), R(x22), R(x22));      // uqincb x22, all, mul #12
  CMP_IO(dci(0x04ecf6ba), R(x26), R(x26));      // uqincd w26, #0x15, mul #13
  CMP_IO(dci(0x04fbf724), R(x4), R(x4));        // uqincd x4, #0x19, mul #12
  CMP_IO(dci(0x04eac46d), R(v13), R(v13));      // uqincd z13.d, vl3, mul #11
  CMP_IO(dci(0x046df7a6), R(x6), R(x6));        // uqinch w6, mul4, mul #14
  CMP_IO(dci(0x047bf580), R(x0), R(x0));        // uqinch x0, vl128, mul #12
  CMP_IO(dci(0x0469c781), R(v1), R(v1));        // uqinch z1.h, #0x1c, mul #10
  CMP_IO(dci(0x256989c2), R(x2), R(x2, p14));   // uqincp w2, p14.h
  CMP_IO(dci(0x25298c1c), R(x28), R(x28, p0));  // uqincp x28, p0.b
  CMP_IO(dci(0x25698165), R(v5), R(v5, p11));   // uqincp z5.h, p11
  CMP_IO(dci(0x04acf693), R(x19), R(x19));      // uqincw w19, #0x14, mul #13
  CMP_IO(dci(0x04bbf679), R(x25), R(x25));      // uqincw x25, #0x13, mul #12
  CMP_IO(dci(0x04aac638), R(v24), R(v24));      // uqincw z24.s, #0x11, mul #11
  CMP_IO(dci(0x6e395df7),
         R(v23),
         R(v15, v25));  // uqrshl v23.16b, v15.16b, v25.16b
  CMP_IO(dci(0x7efa5d59), R(v25), R(v10, v26));  // uqrshl d25, d10, d26
  CMP_IO(dci(0x448b9cc8),
         R(v8),
         R(p7, v8, v6));  // uqrshl z8.s, p7/m, z8.s, z6.s
  CMP_IO(dci(0x44cf9ba6),
         R(v6),
         R(p6, v6, v29));                   // uqrshlr z6.d, p6/m, z6.d, z29.d
  CMP_IO(dci(0x2f259d64), R(v4), R(v11));   // uqrshrn v4.2s, v11.2d, #27
  CMP_IO(dci(0x7f3e9d7e), R(v30), R(v11));  // uqrshrn s30, d11, #2
  CMP_IO(dci(0x457939ea), R(v10), R(v15));  // uqrshrnb z10.s, z15.d, #7
  CMP_IO(dci(0x453a3ef0), R(v16), R(v16, v23));  // uqrshrnt z16.h, z23.s, #6
  CMP_IO(dci(0x6e7e4f15), R(v21), R(v24, v30));  // uqshl v21.8h, v24.8h, v30.8h
  CMP_IO(dci(0x6f4f76ce), R(v14), R(v22));       // uqshl v14.2d, v22.2d, #15
  CMP_IO(dci(0x7ebd4efb), R(v27), R(v23, v29));  // uqshl s27, s23, s29
  CMP_IO(dci(0x7f5d7564), R(v4), R(v11));        // uqshl d4, d11, #29
  CMP_IO(dci(0x04079f7f), R(v31), R(p7, v31));  // uqshl z31.h, p7/m, z31.h, #11
  CMP_IO(dci(0x44c99ba6),
         R(v6),
         R(p6, v6, v29));  // uqshl z6.d, p6/m, z6.d, z29.d
  CMP_IO(dci(0x448d9cc8),
         R(v8),
         R(p7, v8, v6));  // uqshlr z8.s, p7/m, z8.s, z6.s

  CLEANUP();
}

TEST(group_77) {
  SETUP();

  CMP_IO(dci(0x6f3a97c2), R(v2), R(v30));        // uqshrn2 v2.4s, v30.2d, #6
  CMP_IO(dci(0x7f199460), R(v0), R(v3));         // uqshrn h0, s3, #7
  CMP_IO(dci(0x453e30ac), R(v12), R(v5));        // uqshrnb z12.h, z5.s, #2
  CMP_IO(dci(0x457d36ee), R(v14), R(v14, v23));  // uqshrnt z14.s, z23.d, #3
  CMP_IO(dci(0x6ebd2fc0), R(v0), R(v30, v29));   // uqsub v0.4s, v30.4s, v29.4s
  CMP_IO(dci(0x7e7e2f22), R(v2), R(v25, v30));   // uqsub h2, h25, h30
  CMP_IO(dci(0x441b9f7f),
         R(v31),
         R(p7, v31, v27));                // uqsub z31.b, p7/m, z31.b, z27.b
  CMP_IO(dci(0x25a7f643), R(v3), R(v3));  // uqsub z3.s, z3.s, #178, lsl #8
  CMP_IO(dci(0x047d1d4a), R(v10), R(v10, v29));  // uqsub z10.h, z10.h, z29.h
  CMP_IO(dci(0x445f841d),
         R(v29),
         R(p1, v29, v0));                   // uqsubr z29.h, p1/m, z29.h, z0.h
  CMP_IO(dci(0x2ea14aee), R(v14), R(v23));  // uqxtn v14.2s, v23.2d
  CMP_IO(dci(0x7e214a04), R(v4), R(v16));   // uqxtn b4, h16
  CMP_IO(dci(0x45604b9e), R(v30), R(v28));  // uqxtnb z30.s, z28.d
  CMP_IO(dci(0x45304ed0), R(v16), R(v16, v22));    // uqxtnt z16.h, z22.s
  CMP_IO(dci(0x4ea1cbf4), R(v20), R(v31));         // urecpe v20.4s, v31.4s
  CMP_IO(dci(0x4480a225), R(v5), R(v5, p0, v17));  // urecpe z5.s, p0/m, z17.s
  CMP_IO(dci(0x2e791532), R(v18), R(v9, v25));  // urhadd v18.4h, v9.4h, v25.4h
  CMP_IO(dci(0x44d580a1),
         R(v1),
         R(p0, v1, v5));  // urhadd z1.d, p0/m, z1.d, z5.d
  CMP_IO(dci(0x2ebe5650), R(v16), R(v18, v30));  // urshl v16.2s, v18.2s, v30.2s
  CMP_IO(dci(0x7ef557e6), R(v6), R(v31, v21));   // urshl d6, d31, d21
  CMP_IO(dci(0x4403860f),
         R(v15),
         R(p1, v15, v16));  // urshl z15.b, p1/m, z15.b, z16.b
  CMP_IO(dci(0x44479d8d),
         R(v13),
         R(p7, v13, v12));                  // urshlr z13.h, p7/m, z13.h, z12.h
  CMP_IO(dci(0x6f4b259d), R(v29), R(v12));  // urshr v29.2d, v12.2d, #53
  CMP_IO(dci(0x7f4824bb), R(v27), R(v5));   // urshr d27, d5, #56
  CMP_IO(dci(0x040d938c), R(v12), R(p4, v12));  // urshr z12.h, p4/m, z12.h, #4
  CMP_IO(dci(0x2ea1ca72), R(v18), R(v19));      // ursqrte v18.2s, v19.2s
  CMP_IO(dci(0x4481bc03), R(v3), R(v3, p7, v0));  // ursqrte z3.s, p7/m, z0.s
  CMP_IO(dci(0x6f6434be), R(v30), R(v30, v5));    // ursra v30.2d, v5.2d, #28
  CMP_IO(dci(0x7f6e3598), R(v24), R(v24, v12));   // ursra d24, d12, #18
  CMP_IO(dci(0x45deed7e), R(v30), R(v30, v11));   // ursra z30.d, z11.d, #2
  CMP_IO(dci(0x0f9afa1e),
         R(v30),
         R(v30, v16, v26));  // usdot v30.2s, v16.8b, v26.4b[2]
  CMP_IO(dci(0x0e9c9c1d),
         R(v29),
         R(v29, v0, v28));  // usdot v29.2s, v0.8b, v28.8b

  CLEANUP();
}

TEST(group_78) {
  SETUP();

  CMP_IO(dci(0x449b7a5f),
         R(v31),
         R(v31, v18, v27));                        // usdot z31.s, z18.b, z27.b
  CMP_IO(dci(0x44bc1aa0), R(v0), R(v0, v21, v4));  // usdot z0.s, z21.b, z4.b[3]
  CMP_IO(dci(0x6ef9478e), R(v14), R(v28, v25));  // ushl v14.2d, v28.2d, v25.2d
  CMP_IO(dci(0x7eee472c), R(v12), R(v25, v14));  // ushl d12, d25, d14
  CMP_IO(dci(0x2f38a5cc), R(v12), R(v14));       // ushll v12.2d, v14.2s, #24
  CMP_IO(dci(0x451aabcf), R(v15), R(v30));       // ushllb z15.s, z30.h, #10
  CMP_IO(dci(0x4559aec9), R(v9), R(v22));        // ushllt z9.d, z22.s, #25
  CMP_IO(dci(0x6f430677), R(v23), R(v19));       // ushr v23.2d, v19.2d, #61
  CMP_IO(dci(0x7f610525), R(v5), R(v9));         // ushr d5, d9, #31
  CMP_IO(dci(0x4e9cad16),
         R(v22),
         R(v22, v8, v28));  // usmmla v22.4s, v8.16b, v28.16b
  CMP_IO(dci(0x459e9971),
         R(v17),
         R(v17, v11, v30));                // usmmla z17.s, z11.b, z30.b
  CMP_IO(dci(0x2e603b22), R(v2), R(v25));  // usqadd v2.4h, v25.4h
  CMP_IO(dci(0x7ea03bc0), R(v0), R(v30));  // usqadd s0, s30
  CMP_IO(dci(0x44dd9a79),
         R(v25),
         R(p6, v25, v19));  // usqadd z25.d, p6/m, z25.d, z19.d
  CMP_IO(dci(0x6f4b1628), R(v8), R(v8, v17));    // usra v8.2d, v17.2d, #53
  CMP_IO(dci(0x7f4a1439), R(v25), R(v25, v1));   // usra d25, d1, #54
  CMP_IO(dci(0x4599e460), R(v0), R(v0, v3));     // usra z0.d, z3.d, #39
  CMP_IO(dci(0x2ead22ee), R(v14), R(v23, v13));  // usubl v14.2d, v23.2s, v13.2s
  CMP_IO(dci(0x45ce194b), R(v11), R(v10, v14));  // usublb z11.d, z10.s, z14.s
  CMP_IO(dci(0x45d91ec9), R(v9), R(v22, v25));   // usublt z9.d, z22.s, z25.s
  CMP_IO(dci(0x2ead32ee), R(v14), R(v23, v13));  // usubw v14.2d, v23.2d, v13.2s
  CMP_IO(dci(0x45d95b8e), R(v14), R(v28, v25));  // usubwb z14.d, z28.d, z25.s
  CMP_IO(dci(0x45585d84), R(v4), R(v12, v24));   // usubwt z4.h, z12.h, z24.b
  CMP_IO(dci(0x05b33ba8), R(v8), R(v29));        // uunpkhi z8.s, z29.h
  CMP_IO(dci(0x05f23ad6), R(v22), R(v22));       // uunpklo z22.d, z22.s
  CMP_IO(dci(0x0491a9e8), R(v8), R(v8, p2, v15));    // uxtb z8.s, p2/m, z15.s
  CMP_IO(dci(0x04d3a1ce), R(v14), R(p0, v14));       // uxth z14.d, p0/m, z14.d
  CMP_IO(dci(0x04d5bdb3), R(v19), R(v19, p7, v13));  // uxtw z19.d, p7/m, z13.d
  CMP_IO(dci(0x4e5d1a02), R(v2), R(v16, v29));   // uzp1 v2.8h, v16.8h, v29.8h
  CMP_IO(dci(0x05ec49ce), R(p14), R(p14, p12));  // uzp1 p14.d, p14.d, p12.d
  CMP_IO(dci(0x05bc69e8), R(v8), R(v15, v28));   // uzp1 z8.s, z15.s, z28.s
  CMP_IO(dci(0x0e9d586b), R(v11), R(v3, v29));   // uzp2 v11.2s, v3.2s, v29.2s

  CLEANUP();
}

TEST(group_79) {
  SETUP();

  CMP_IO(dci(0x052c4c27), R(p7), R(p1, p12));    // uzp2 p7.b, p1.b, p12.b
  CMP_IO(dci(0x057c6ead), R(v13), R(v21, v28));  // uzp2 z13.h, z21.h, z28.h
  CMP_IO(dci(0x257d1202), R(p2), R(x16, x29));   // whilege p2.h, x16, x29
  CMP_IO(dci(0x25fc0119), R(p9), R(x8, x28));    // whilegt p9.d, w8, w28
  CMP_IO(dci(0x25bd187b), R(p11), R(x3, x29));   // whilehi p11.s, x3, x29
  CMP_IO(dci(0x25fc09c1), R(p1), R(x14, x28));   // whilehs p1.d, w14, w28
  CMP_IO(dci(0x25bc0533), R(p3), R(x9, x28));    // whilele p3.s, w9, w28
  CMP_IO(dci(0x25fd0f89), R(p9), R(x28, x29));   // whilelo p9.d, w28, w29
  CMP_IO(dci(0x257c1f75), R(p5), R(x27, x28));   // whilels p5.h, x27, x28
  CMP_IO(dci(0x253c04ec), R(p12), R(x7, x28));   // whilelt p12.b, w7, w28
  CMP_IO(dci(0x257e33da), R(p10), R(x30, x30));  // whilerw p10.h, x30, x30
  CMP_IO(dci(0x257e33ca), R(p10), R(x30, x30));  // whilewr p10.h, x30, x30
  CMP_IO(dci(0x25289020), R(), R(p1));           // wrffr p1.b
  CMP_IO(dci(0xd5004d3f), R(), R());             // xaflag
  CMP_IO(dci(0x043936bc), R(v28), R(v28, v21));  // xar z28.h, z28.h, z21.h, #7
  CMP_IO(dci(0xdac147fe), R(x30), R(x30));       // xpacd x30
  CMP_IO(dci(0xdac143f9), R(x25), R(x25));       // xpaci x25
  CMP_IO(dci(0xd50320ff), R(x30), R(x30));       // xpaclri
  CMP_IO(dci(0x0e61287a), R(v26), R(v3));        // xtn v26.4h, v3.4s
  CMP_IO(dci(0xd503203f), R(), R());             // yield
  CMP_IO(dci(0x4edc39ce), R(v14), R(v14, v28));  // zip1 v14.2d, v14.2d, v28.2d
  CMP_IO(dci(0x056d4002), R(p2), R(p0, p13));    // zip1 p2.h, p0.h, p13.h
  CMP_IO(dci(0x053c638c), R(v12), R(v28, v28));  // zip1 z12.b, z28.b, z28.b
  CMP_IO(dci(0x0e1c7837), R(v23), R(v1, v28));   // zip2 v23.8b, v1.8b, v28.8b
  CMP_IO(dci(0x05ad446b), R(p11), R(p3, p13));   // zip2 p11.s, p3.s, p13.s
  CMP_IO(dci(0x05fc6451), R(v17), R(v2, v28));   // zip2 z17.d, z2.d, z28.d

  CLEANUP();
}

TEST(group_80) {
  SETUP();

  CMP_IO(dci(0x98123456),
         R(x22),
         R());  // ldrsw x22, pc+149128 (addr 0x7fff098dd6e4)
  CMP_IO(dci(0x585555cc),
         R(x12),
         R());  // ldr x12, pc+699064 (addr 0x7fffeede4be4)
  CMP_IO(dci(0x18aaaaee),
         R(x14),
         R());  // ldr w14, pc-699044 (addr 0x7ffcd56d4478)
  CMP_IO(dci(0x1c564747),
         R(v7),
         R());  // ldr s7, pc+706792 (addr 0x7ffee374dd54)
  CMP_IO(dci(0x5c676fef),
         R(v15),
         R());  // ldr d15, pc+847356 (addr 0x7ffc51e91c68)
  CMP_IO(dci(0x9cedcaa2),
         R(v2),
         R());  // ldr q2, pc-149164 (addr 0x7ffd852c53f0)
  CMP_IO(dci(0xd8ed8700),
         R(),
         R());  // prfm pldl1keep, pc-151328 (addr 0x7ffd6bf2eb9c)

  CLEANUP();
}
#undef R

}  // namespace aarch64
}  // namespace vixl
