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


#include <cstdio>
#include <cstring>
#include <string>

#include "test-runner.h"
#include "aarch64/test-utils-aarch64.h"

#include "aarch64/disasm-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"
#include "test-disasm-aarch64.h"
#include "test-utils-aarch64.h"

namespace vixl {
namespace aarch64 {

// Tests of pre-Morello instructions, but in C64, where the base register
// becomes a C register. These tests should cover one example of each addressing
// mode of each instruction, to check that the base register is correctly
// handled. More detailed tests of the addressing modes themselves should exist
// elsewhere.
//
// TODO: Another approach is to add some placeholder to the expected string of
// existing tests, then run them in both ISAs. That has greater coverage, but at
// the cost of some test complexity.

TEST(c64_cas_w) {
  SETUP();

  // cas*
  COMPARE_MACRO_C64(Casa(w22, w23, MemOperand(c1)), "casa w22, w23, [c1]");
  COMPARE_MACRO_C64(Casal(w12, w13, MemOperand(c28)), "casal w12, w13, [c28]");
  COMPARE_MACRO_C64(Cas(w14, w15, MemOperand(c26)), "cas w14, w15, [c26]");
  COMPARE_MACRO_C64(Casl(w26, w27, MemOperand(csp)), "casl w26, w27, [csp]");

  // cas*b
  COMPARE_MACRO_C64(Casab(w0, w1, MemOperand(c27)), "casab w0, w1, [c27]");
  COMPARE_MACRO_C64(Casalb(w18, w19, MemOperand(c7)), "casalb w18, w19, [c7]");
  COMPARE_MACRO_C64(Casb(w4, w5, MemOperand(c6)), "casb w4, w5, [c6]");
  COMPARE_MACRO_C64(Caslb(w10, w11, MemOperand(c0)), "caslb w10, w11, [c0]");

  // cas*h
  COMPARE_MACRO_C64(Casah(w26, w27, MemOperand(c28)), "casah w26, w27, [c28]");
  COMPARE_MACRO_C64(Casalh(w4, w5, MemOperand(c1)), "casalh w4, w5, [c1]");
  COMPARE_MACRO_C64(Cash(w8, w9, MemOperand(csp)), "cash w8, w9, [csp]");
  COMPARE_MACRO_C64(Caslh(w10, w11, MemOperand(c24)), "caslh w10, w11, [c24]");

  // casp*
  COMPARE_MACRO_C64(Caspa(w26, w27, w24, w25, MemOperand(c3)),
                    "caspa w26, w27, w24, w25, [c3]");
  COMPARE_MACRO_C64(Caspal(w2, w3, w14, w15, MemOperand(c29)),
                    "caspal w2, w3, w14, w15, [c29]");
  COMPARE_MACRO_C64(Casp(w4, w5, w8, w9, MemOperand(c4)),
                    "casp w4, w5, w8, w9, [c4]");
  COMPARE_MACRO_C64(Caspl(w4, w5, w6, w7, MemOperand(c24)),
                    "caspl w4, w5, w6, w7, [c24]");
}

TEST(c64_cas_x) {
  SETUP();

  // cas*
  COMPARE_MACRO_C64(Casa(x2, x3, MemOperand(c28)), "casa x2, x3, [c28]");
  COMPARE_MACRO_C64(Casal(x30, xzr, MemOperand(c4)), "casal x30, xzr, [c4]");
  COMPARE_MACRO_C64(Cas(x0, x1, MemOperand(c25)), "cas x0, x1, [c25]");
  COMPARE_MACRO_C64(Casl(x12, x13, MemOperand(c24)), "casl x12, x13, [c24]");

  // casp*
  COMPARE_MACRO_C64(Caspa(x6, x7, x10, x11, MemOperand(c2)),
                    "caspa x6, x7, x10, x11, [c2]");
  COMPARE_MACRO_C64(Caspal(x30, xzr, x8, x9, MemOperand(c3)),
                    "caspal x30, xzr, x8, x9, [c3]");
  COMPARE_MACRO_C64(Casp(x4, x5, x30, xzr, MemOperand(c25)),
                    "casp x4, x5, x30, xzr, [c25]");
  COMPARE_MACRO_C64(Caspl(x0, x1, x2, x3, MemOperand(c1)),
                    "caspl x0, x1, x2, x3, [c1]");
}

TEST(c64_ldadd_w) {
  SETUP();

  // ldadd*
  COMPARE_MACRO_C64(Ldadd(w15, w19, MemOperand(c30)), "ldadd w15, w19, [c30]");
  COMPARE_MACRO_C64(Ldadda(w1, w23, MemOperand(c24)), "ldadda w1, w23, [c24]");
  COMPARE_MACRO_C64(Ldaddal(w8, w26, MemOperand(c25)),
                    "ldaddal w8, w26, [c25]");
  COMPARE_MACRO_C64(Ldaddl(w2, w7, MemOperand(c24)), "ldaddl w2, w7, [c24]");

  // ldadd*b
  COMPARE_MACRO_C64(Ldaddb(w11, w28, MemOperand(c29)),
                    "ldaddb w11, w28, [c29]");
  COMPARE_MACRO_C64(Ldaddab(w7, w20, MemOperand(c3)), "ldaddab w7, w20, [c3]");
  COMPARE_MACRO_C64(Ldaddalb(w26, w29, MemOperand(c30)),
                    "ldaddalb w26, w29, [c30]");
  COMPARE_MACRO_C64(Ldaddlb(w19, w14, MemOperand(c27)),
                    "ldaddlb w19, w14, [c27]");

  // ldadd*h
  COMPARE_MACRO_C64(Ldaddh(w19, w3, MemOperand(c30)), "ldaddh w19, w3, [c30]");
  COMPARE_MACRO_C64(Ldaddah(w6, w5, MemOperand(c6)), "ldaddah w6, w5, [c6]");
  COMPARE_MACRO_C64(Ldaddalh(w22, w1, MemOperand(c24)),
                    "ldaddalh w22, w1, [c24]");
  COMPARE_MACRO_C64(Ldaddlh(w10, w13, MemOperand(c4)),
                    "ldaddlh w10, w13, [c4]");
}

TEST(c64_ldadd_x) {
  SETUP();

  // ldadd*
  COMPARE_MACRO_C64(Ldadd(x13, x0, MemOperand(c1)), "ldadd x13, x0, [c1]");
  COMPARE_MACRO_C64(Ldadda(x10, x2, MemOperand(c27)), "ldadda x10, x2, [c27]");
  COMPARE_MACRO_C64(Ldaddal(x11, x11, MemOperand(c4)),
                    "ldaddal x11, x11, [c4]");
  COMPARE_MACRO_C64(Ldaddl(x0, x29, MemOperand(c6)), "ldaddl x0, x29, [c6]");
}

TEST(c64_ldapr) {
  SETUP();

  COMPARE_MACRO_C64(Ldapr(x23, MemOperand(c27)), "ldapr x23, [c27]");
  COMPARE_MACRO_C64(Ldapr(w28, MemOperand(c5)), "ldapr w28, [c5]");
  COMPARE_MACRO_C64(Ldaprb(w0, MemOperand(c24)), "ldaprb w0, [c24]");
  COMPARE_MACRO_C64(Ldaprh(w28, MemOperand(c4)), "ldaprh w28, [c4]");
}

TEST(c64_ldar) {
  SETUP();

  COMPARE_MACRO_C64(Ldar(x25, MemOperand(c2)), "ldar x25, [c2]");
  COMPARE_MACRO_C64(Ldar(w25, MemOperand(c30)), "ldar w25, [c30]");
  COMPARE_MACRO_C64(Ldarb(w28, MemOperand(c0)), "ldarb w28, [c0]");
  COMPARE_MACRO_C64(Ldarh(w23, MemOperand(csp)), "ldarh w23, [csp]");
}

TEST(c64_ldax) {
  SETUP();

  COMPARE_MACRO_C64(Ldaxp(x25, x4, MemOperand(c24)), "ldaxp x25, x4, [c24]");
  COMPARE_MACRO_C64(Ldaxp(w22, w19, MemOperand(c3)), "ldaxp w22, w19, [c3]");
  COMPARE_MACRO_C64(Ldaxr(x1, MemOperand(c0)), "ldaxr x1, [c0]");
  COMPARE_MACRO_C64(Ldaxr(w27, MemOperand(c30)), "ldaxr w27, [c30]");
  COMPARE_MACRO_C64(Ldaxrb(w11, MemOperand(c1)), "ldaxrb w11, [c1]");
  COMPARE_MACRO_C64(Ldaxrh(w23, MemOperand(c24)), "ldaxrh w23, [c24]");
}

TEST(c64_ldclr_w) {
  SETUP();

  // ldclr*
  COMPARE_MACRO_C64(Ldclr(w28, w30, MemOperand(c30)), "ldclr w28, w30, [c30]");
  COMPARE_MACRO_C64(Ldclra(w28, w19, MemOperand(c4)), "ldclra w28, w19, [c4]");
  COMPARE_MACRO_C64(Ldclral(w25, w2, MemOperand(c3)), "ldclral w25, w2, [c3]");
  COMPARE_MACRO_C64(Ldclrl(w8, w30, MemOperand(c6)), "ldclrl w8, w30, [c6]");

  // ldclr*b
  COMPARE_MACRO_C64(Ldclrb(w13, w5, MemOperand(c7)), "ldclrb w13, w5, [c7]");
  COMPARE_MACRO_C64(Ldclrab(w28, w3, MemOperand(c1)), "ldclrab w28, w3, [c1]");
  COMPARE_MACRO_C64(Ldclralb(w30, w25, MemOperand(c28)),
                    "ldclralb w30, w25, [c28]");
  COMPARE_MACRO_C64(Ldclrlb(w13, w14, MemOperand(c30)),
                    "ldclrlb w13, w14, [c30]");

  // ldclr*h
  COMPARE_MACRO_C64(Ldclrh(w18, w14, MemOperand(c6)), "ldclrh w18, w14, [c6]");
  COMPARE_MACRO_C64(Ldclrah(w19, w0, MemOperand(c26)),
                    "ldclrah w19, w0, [c26]");
  COMPARE_MACRO_C64(Ldclralh(w20, w29, MemOperand(c25)),
                    "ldclralh w20, w29, [c25]");
  COMPARE_MACRO_C64(Ldclrlh(w9, w4, MemOperand(c26)), "ldclrlh w9, w4, [c26]");
}

TEST(c64_ldclr_x) {
  SETUP();

  // ldclr*
  COMPARE_MACRO_C64(Ldclr(x28, x15, MemOperand(c2)), "ldclr x28, x15, [c2]");
  COMPARE_MACRO_C64(Ldclra(x26, x28, MemOperand(c26)),
                    "ldclra x26, x28, [c26]");
  COMPARE_MACRO_C64(Ldclral(x22, x13, MemOperand(c24)),
                    "ldclral x22, x13, [c24]");
  COMPARE_MACRO_C64(Ldclrl(x14, x4, MemOperand(c6)), "ldclrl x14, x4, [c6]");
}

TEST(c64_ldeor_w) {
  SETUP();

  // ldeor*
  COMPARE_MACRO_C64(Ldeor(w11, w14, MemOperand(c25)), "ldeor w11, w14, [c25]");
  COMPARE_MACRO_C64(Ldeora(w2, w29, MemOperand(c0)), "ldeora w2, w29, [c0]");
  COMPARE_MACRO_C64(Ldeoral(w20, w21, MemOperand(c27)),
                    "ldeoral w20, w21, [c27]");
  COMPARE_MACRO_C64(Ldeorl(w21, w13, MemOperand(c2)), "ldeorl w21, w13, [c2]");

  // ldeor*b
  COMPARE_MACRO_C64(Ldeorb(w24, w27, MemOperand(c26)),
                    "ldeorb w24, w27, [c26]");
  COMPARE_MACRO_C64(Ldeorab(w11, w13, MemOperand(c27)),
                    "ldeorab w11, w13, [c27]");
  COMPARE_MACRO_C64(Ldeoralb(w9, w1, MemOperand(c2)), "ldeoralb w9, w1, [c2]");
  COMPARE_MACRO_C64(Ldeorlb(w22, w22, MemOperand(c7)),
                    "ldeorlb w22, w22, [c7]");

  // ldeor*h
  COMPARE_MACRO_C64(Ldeorh(w27, w21, MemOperand(c26)),
                    "ldeorh w27, w21, [c26]");
  COMPARE_MACRO_C64(Ldeorah(w3, w18, MemOperand(c26)),
                    "ldeorah w3, w18, [c26]");
  COMPARE_MACRO_C64(Ldeoralh(w4, w13, MemOperand(c2)),
                    "ldeoralh w4, w13, [c2]");
  COMPARE_MACRO_C64(Ldeorlh(w25, w22, MemOperand(c25)),
                    "ldeorlh w25, w22, [c25]");
}

TEST(c64_ldeor_x) {
  SETUP();

  // ldeor*
  COMPARE_MACRO_C64(Ldeor(x20, x29, MemOperand(c1)), "ldeor x20, x29, [c1]");
  COMPARE_MACRO_C64(Ldeora(x29, x13, MemOperand(c7)), "ldeora x29, x13, [c7]");
  COMPARE_MACRO_C64(Ldeoral(x4, x0, MemOperand(c24)), "ldeoral x4, x0, [c24]");
  COMPARE_MACRO_C64(Ldeorl(x23, x9, MemOperand(c5)), "ldeorl x23, x9, [c5]");
}

TEST(c64_ldlar_stllr) {
  SETUP();

  COMPARE_MACRO_C64(Ldlar(x7, MemOperand(c29)), "ldlar x7, [c29]");
  COMPARE_MACRO_C64(Ldlar(w26, MemOperand(c5)), "ldlar w26, [c5]");
  COMPARE_MACRO_C64(Ldlarb(w3, MemOperand(c26)), "ldlarb w3, [c26]");
  COMPARE_MACRO_C64(Ldlarh(w4, MemOperand(c6)), "ldlarh w4, [c6]");

  COMPARE_MACRO_C64(Stllr(x13, MemOperand(csp)), "stllr x13, [csp]");
  COMPARE_MACRO_C64(Stllr(w14, MemOperand(c2)), "stllr w14, [c2]");
  COMPARE_MACRO_C64(Stllrb(w25, MemOperand(c26)), "stllrb w25, [c26]");
  COMPARE_MACRO_C64(Stllrh(w13, MemOperand(c28)), "stllrh w13, [c28]");
}

TEST(c64_ldnp_stnp) {
  SETUP();

  COMPARE_MACRO_C64(Ldnp(x29, x7, MemOperand(c28)), "ldnp x29, x7, [c28]");
  COMPARE_MACRO_C64(Ldnp(w14, w25, MemOperand(c26)), "ldnp w14, w25, [c26]");

  COMPARE_MACRO_C64(Stnp(x18, x13, MemOperand(c0)), "stnp x18, x13, [c0]");
  COMPARE_MACRO_C64(Stnp(w1, w22, MemOperand(c2)), "stnp w1, w22, [c2]");
}

TEST(c64_ldp_stp) {
  SETUP();

  COMPARE_MACRO_C64(Ldp(x11, x19, MemOperand(c1)), "ldp x11, x19, [c1]");
  COMPARE_MACRO_C64(Ldp(x0, x5, MemOperand(c2, 504, PreIndex)),
                    "ldp x0, x5, [c2, #504]!");
  COMPARE_MACRO_C64(Ldp(x12, x3, MemOperand(c30, -512, PostIndex)),
                    "ldp x12, x3, [c30], #-512");

  COMPARE_MACRO_C64(Ldp(w19, w4, MemOperand(c27)), "ldp w19, w4, [c27]");
  COMPARE_MACRO_C64(Ldp(w22, w24, MemOperand(c6, 252, PreIndex)),
                    "ldp w22, w24, [c6, #252]!");
  COMPARE_MACRO_C64(Ldp(w5, w19, MemOperand(c29, -256, PostIndex)),
                    "ldp w5, w19, [c29], #-256");

  COMPARE_MACRO_C64(Ldpsw(x18, x2, MemOperand(c28)), "ldpsw x18, x2, [c28]");
  COMPARE_MACRO_C64(Ldpsw(x10, x30, MemOperand(c5, 252, PreIndex)),
                    "ldpsw x10, x30, [c5, #252]!");
  COMPARE_MACRO_C64(Ldpsw(x26, x11, MemOperand(c1, -256, PostIndex)),
                    "ldpsw x26, x11, [c1], #-256");

  COMPARE_MACRO_C64(Stp(x14, x29, MemOperand(c3)), "stp x14, x29, [c3]");
  COMPARE_MACRO_C64(Stp(x13, x25, MemOperand(c29, 504, PreIndex)),
                    "stp x13, x25, [c29, #504]!");
  COMPARE_MACRO_C64(Stp(x24, x5, MemOperand(c28, -512, PostIndex)),
                    "stp x24, x5, [c28], #-512");

  COMPARE_MACRO_C64(Stp(w18, w20, MemOperand(c28)), "stp w18, w20, [c28]");
  COMPARE_MACRO_C64(Stp(w20, w20, MemOperand(c7, 252, PreIndex)),
                    "stp w20, w20, [c7, #252]!");
  COMPARE_MACRO_C64(Stp(w9, w22, MemOperand(c30, -256, PostIndex)),
                    "stp w9, w22, [c30], #-256");
}

TEST(c64_ldr_str_x) {
  SETUP();

  COMPARE_MACRO_C64(Ldr(w23, MemOperand(csp)), "ldr w23, [csp]");
  COMPARE_MACRO_C64(Ldr(w25, MemOperand(c0, 255, PreIndex)),
                    "ldr w25, [c0, #255]!");
  COMPARE_MACRO_C64(Ldr(w23, MemOperand(c7, -256, PostIndex)),
                    "ldr w23, [c7], #-256");
  COMPARE_MACRO_C64(Ldr(w30, MemOperand(c24, w18, SXTW)),
                    "ldr w30, [c24, w18, sxtw]");
  COMPARE_MACRO_C64(Ldr(w11, MemOperand(c27, x25)), "ldr w11, [c27, x25]");

  COMPARE_MACRO_C64(Str(w8, MemOperand(c24)), "str w8, [c24]");
  COMPARE_MACRO_C64(Str(w30, MemOperand(c4, 255, PreIndex)),
                    "str w30, [c4, #255]!");
  COMPARE_MACRO_C64(Str(w27, MemOperand(c27, -256, PostIndex)),
                    "str w27, [c27], #-256");
  COMPARE_MACRO_C64(Str(w1, MemOperand(c4, w12, SXTW)),
                    "str w1, [c4, w12, sxtw]");
  COMPARE_MACRO_C64(Str(w18, MemOperand(c4, x11)), "str w18, [c4, x11]");
}

TEST(c64_ldr_str_w) {
  SETUP();

  COMPARE_MACRO_C64(Ldr(x10, MemOperand(c1)), "ldr x10, [c1]");
  COMPARE_MACRO_C64(Ldr(x5, MemOperand(c6, 255, PreIndex)),
                    "ldr x5, [c6, #255]!");
  COMPARE_MACRO_C64(Ldr(x25, MemOperand(c30, -256, PostIndex)),
                    "ldr x25, [c30], #-256");
  COMPARE_MACRO_C64(Ldr(x10, MemOperand(c5, w0, SXTW)),
                    "ldr x10, [c5, w0, sxtw]");
  COMPARE_MACRO_C64(Ldr(x0, MemOperand(c5, x13)), "ldr x0, [c5, x13]");

  COMPARE_MACRO_C64(Str(x26, MemOperand(c6)), "str x26, [c6]");
  COMPARE_MACRO_C64(Str(x0, MemOperand(c28, 255, PreIndex)),
                    "str x0, [c28, #255]!");
  COMPARE_MACRO_C64(Str(x11, MemOperand(c30, -256, PostIndex)),
                    "str x11, [c30], #-256");
  COMPARE_MACRO_C64(Str(x12, MemOperand(c25, w11, SXTW)),
                    "str x12, [c25, w11, sxtw]");
  COMPARE_MACRO_C64(Str(x30, MemOperand(c1, x22)), "str x30, [c1, x22]");
}

TEST(c64_ldrb_strb) {
  SETUP();

  COMPARE_MACRO_C64(Ldrb(w4, MemOperand(c28)), "ldrb w4, [c28]");
  COMPARE_MACRO_C64(Ldrb(w29, MemOperand(c1, 255, PreIndex)),
                    "ldrb w29, [c1, #255]!");
  COMPARE_MACRO_C64(Ldrb(w15, MemOperand(c2, -256, PostIndex)),
                    "ldrb w15, [c2], #-256");
  COMPARE_MACRO_C64(Ldrb(w13, MemOperand(c28, w23, SXTW)),
                    "ldrb w13, [c28, w23, sxtw]");
  COMPARE_MACRO_C64(Ldrb(w10, MemOperand(c6, x12)), "ldrb w10, [c6, x12]");

  COMPARE_MACRO_C64(Strb(w11, MemOperand(c1)), "strb w11, [c1]");
  COMPARE_MACRO_C64(Strb(w11, MemOperand(c6, 255, PreIndex)),
                    "strb w11, [c6, #255]!");
  COMPARE_MACRO_C64(Strb(w15, MemOperand(c27, -256, PostIndex)),
                    "strb w15, [c27], #-256");
  COMPARE_MACRO_C64(Strb(w0, MemOperand(c1, w30, SXTW)),
                    "strb w0, [c1, w30, sxtw]");
  COMPARE_MACRO_C64(Strb(w5, MemOperand(c28, x0)), "strb w5, [c28, x0]");
}

TEST(c64_ldrh_strh) {
  SETUP();

  COMPARE_MACRO_C64(Ldrh(w29, MemOperand(c1)), "ldrh w29, [c1]");
  COMPARE_MACRO_C64(Ldrh(w14, MemOperand(c30, 255, PreIndex)),
                    "ldrh w14, [c30, #255]!");
  COMPARE_MACRO_C64(Ldrh(w26, MemOperand(c2, -256, PostIndex)),
                    "ldrh w26, [c2], #-256");
  COMPARE_MACRO_C64(Ldrh(w14, MemOperand(c1, w27, SXTW)),
                    "ldrh w14, [c1, w27, sxtw]");
  COMPARE_MACRO_C64(Ldrh(w28, MemOperand(c24, x1)), "ldrh w28, [c24, x1]");

  COMPARE_MACRO_C64(Strh(w22, MemOperand(c7)), "strh w22, [c7]");
  COMPARE_MACRO_C64(Strh(w13, MemOperand(c24, 255, PreIndex)),
                    "strh w13, [c24, #255]!");
  COMPARE_MACRO_C64(Strh(w28, MemOperand(c6, -256, PostIndex)),
                    "strh w28, [c6], #-256");
  COMPARE_MACRO_C64(Strh(w24, MemOperand(c24, w4, SXTW)),
                    "strh w24, [c24, w4, sxtw]");
  COMPARE_MACRO_C64(Strh(w1, MemOperand(csp, x12)), "strh w1, [csp, x12]");
}

TEST(c64_ldrsb) {
  SETUP();

  COMPARE_MACRO_C64(Ldrsb(w15, MemOperand(c26)), "ldrsb w15, [c26]");
  COMPARE_MACRO_C64(Ldrsb(w6, MemOperand(c0, 255, PreIndex)),
                    "ldrsb w6, [c0, #255]!");
  COMPARE_MACRO_C64(Ldrsb(w25, MemOperand(c29, -256, PostIndex)),
                    "ldrsb w25, [c29], #-256");
  COMPARE_MACRO_C64(Ldrsb(w20, MemOperand(c0, w15, SXTW)),
                    "ldrsb w20, [c0, w15, sxtw]");
  COMPARE_MACRO_C64(Ldrsb(w22, MemOperand(c28, x12)), "ldrsb w22, [c28, x12]");

  COMPARE_MACRO_C64(Ldrsb(x10, MemOperand(c6)), "ldrsb x10, [c6]");
  COMPARE_MACRO_C64(Ldrsb(x2, MemOperand(c5, 255, PreIndex)),
                    "ldrsb x2, [c5, #255]!");
  COMPARE_MACRO_C64(Ldrsb(x27, MemOperand(c29, -256, PostIndex)),
                    "ldrsb x27, [c29], #-256");
  COMPARE_MACRO_C64(Ldrsb(x1, MemOperand(c29, w4, SXTW)),
                    "ldrsb x1, [c29, w4, sxtw]");
  COMPARE_MACRO_C64(Ldrsb(x26, MemOperand(c24, x26)), "ldrsb x26, [c24, x26]");
}

TEST(c64_ldrsh) {
  SETUP();

  COMPARE_MACRO_C64(Ldrsh(w8, MemOperand(c30)), "ldrsh w8, [c30]");
  COMPARE_MACRO_C64(Ldrsh(w19, MemOperand(c7, 255, PreIndex)),
                    "ldrsh w19, [c7, #255]!");
  COMPARE_MACRO_C64(Ldrsh(w14, MemOperand(c24, -256, PostIndex)),
                    "ldrsh w14, [c24], #-256");
  COMPARE_MACRO_C64(Ldrsh(w14, MemOperand(c29, w28, SXTW)),
                    "ldrsh w14, [c29, w28, sxtw]");
  COMPARE_MACRO_C64(Ldrsh(w2, MemOperand(c5, x8)), "ldrsh w2, [c5, x8]");

  COMPARE_MACRO_C64(Ldrsh(x12, MemOperand(c24)), "ldrsh x12, [c24]");
  COMPARE_MACRO_C64(Ldrsh(x19, MemOperand(c4, 255, PreIndex)),
                    "ldrsh x19, [c4, #255]!");
  COMPARE_MACRO_C64(Ldrsh(x0, MemOperand(c2, -256, PostIndex)),
                    "ldrsh x0, [c2], #-256");
  COMPARE_MACRO_C64(Ldrsh(x14, MemOperand(c4, w9, SXTW)),
                    "ldrsh x14, [c4, w9, sxtw]");
  COMPARE_MACRO_C64(Ldrsh(x30, MemOperand(c4, x19)), "ldrsh x30, [c4, x19]");
}

TEST(c64_ldrsw) {
  SETUP();

  COMPARE_MACRO_C64(Ldrsw(x29, MemOperand(c29)), "ldrsw x29, [c29]");
  COMPARE_MACRO_C64(Ldrsw(x2, MemOperand(c7, 255, PreIndex)),
                    "ldrsw x2, [c7, #255]!");
  COMPARE_MACRO_C64(Ldrsw(x25, MemOperand(c3, -256, PostIndex)),
                    "ldrsw x25, [c3], #-256");
  COMPARE_MACRO_C64(Ldrsw(x7, MemOperand(c26, w4, SXTW)),
                    "ldrsw x7, [c26, w4, sxtw]");
  COMPARE_MACRO_C64(Ldrsw(x15, MemOperand(c0, x20)), "ldrsw x15, [c0, x20]");
}

TEST(c64_ldset_w) {
  SETUP();

  // ldset*
  COMPARE_MACRO_C64(Ldset(w7, w3, MemOperand(c30)), "ldset w7, w3, [c30]");
  COMPARE_MACRO_C64(Ldseta(w6, w23, MemOperand(c2)), "ldseta w6, w23, [c2]");
  COMPARE_MACRO_C64(Ldsetal(w4, w19, MemOperand(c27)),
                    "ldsetal w4, w19, [c27]");
  COMPARE_MACRO_C64(Ldsetl(w5, w0, MemOperand(c26)), "ldsetl w5, w0, [c26]");

  // ldset*b
  COMPARE_MACRO_C64(Ldsetb(w4, w27, MemOperand(c26)), "ldsetb w4, w27, [c26]");
  COMPARE_MACRO_C64(Ldsetab(w13, w2, MemOperand(c28)),
                    "ldsetab w13, w2, [c28]");
  COMPARE_MACRO_C64(Ldsetalb(w2, w7, MemOperand(c6)), "ldsetalb w2, w7, [c6]");
  COMPARE_MACRO_C64(Ldsetlb(w15, w5, MemOperand(c25)),
                    "ldsetlb w15, w5, [c25]");

  // ldset*h
  COMPARE_MACRO_C64(Ldseth(w6, w9, MemOperand(c25)), "ldseth w6, w9, [c25]");
  COMPARE_MACRO_C64(Ldsetah(w26, w13, MemOperand(c26)),
                    "ldsetah w26, w13, [c26]");
  COMPARE_MACRO_C64(Ldsetalh(w12, w24, MemOperand(c5)),
                    "ldsetalh w12, w24, [c5]");
  COMPARE_MACRO_C64(Ldsetlh(w26, w28, MemOperand(c30)),
                    "ldsetlh w26, w28, [c30]");
}

TEST(c64_ldset_x) {
  SETUP();

  // ldset*
  COMPARE_MACRO_C64(Ldset(x3, x14, MemOperand(c1)), "ldset x3, x14, [c1]");
  COMPARE_MACRO_C64(Ldseta(x1, x15, MemOperand(csp)), "ldseta x1, x15, [csp]");
  COMPARE_MACRO_C64(Ldsetal(x18, x25, MemOperand(c6)),
                    "ldsetal x18, x25, [c6]");
  COMPARE_MACRO_C64(Ldsetl(x25, x11, MemOperand(c1)), "ldsetl x25, x11, [c1]");
}

TEST(c64_ldsmax_w) {
  SETUP();

  // ldsmax*
  COMPARE_MACRO_C64(Ldsmax(w29, w13, MemOperand(c4)), "ldsmax w29, w13, [c4]");
  COMPARE_MACRO_C64(Ldsmaxa(w19, w3, MemOperand(c27)),
                    "ldsmaxa w19, w3, [c27]");
  COMPARE_MACRO_C64(Ldsmaxal(w13, w14, MemOperand(c25)),
                    "ldsmaxal w13, w14, [c25]");
  COMPARE_MACRO_C64(Ldsmaxl(w9, w23, MemOperand(c4)), "ldsmaxl w9, w23, [c4]");

  // ldsmax*b
  COMPARE_MACRO_C64(Ldsmaxb(w20, w12, MemOperand(c1)),
                    "ldsmaxb w20, w12, [c1]");
  COMPARE_MACRO_C64(Ldsmaxab(w15, w10, MemOperand(c0)),
                    "ldsmaxab w15, w10, [c0]");
  COMPARE_MACRO_C64(Ldsmaxalb(w13, w8, MemOperand(c1)),
                    "ldsmaxalb w13, w8, [c1]");
  COMPARE_MACRO_C64(Ldsmaxlb(w14, w9, MemOperand(c7)),
                    "ldsmaxlb w14, w9, [c7]");

  // ldsmax*h
  COMPARE_MACRO_C64(Ldsmaxh(w7, w9, MemOperand(c7)), "ldsmaxh w7, w9, [c7]");
  COMPARE_MACRO_C64(Ldsmaxah(w1, w12, MemOperand(c3)),
                    "ldsmaxah w1, w12, [c3]");
  COMPARE_MACRO_C64(Ldsmaxalh(w13, w12, MemOperand(c2)),
                    "ldsmaxalh w13, w12, [c2]");
  COMPARE_MACRO_C64(Ldsmaxlh(w10, w26, MemOperand(c24)),
                    "ldsmaxlh w10, w26, [c24]");
}

TEST(c64_ldsmax_x) {
  SETUP();

  // ldsmax*
  COMPARE_MACRO_C64(Ldsmax(x19, x6, MemOperand(c0)), "ldsmax x19, x6, [c0]");
  COMPARE_MACRO_C64(Ldsmaxa(x6, x3, MemOperand(c3)), "ldsmaxa x6, x3, [c3]");
  COMPARE_MACRO_C64(Ldsmaxal(x10, x5, MemOperand(c5)),
                    "ldsmaxal x10, x5, [c5]");
  COMPARE_MACRO_C64(Ldsmaxl(x24, x1, MemOperand(c30)),
                    "ldsmaxl x24, x1, [c30]");
}

TEST(c64_ldsmin_w) {
  SETUP();

  // ldsmin*
  COMPARE_MACRO_C64(Ldsmin(w2, w26, MemOperand(c1)), "ldsmin w2, w26, [c1]");
  COMPARE_MACRO_C64(Ldsmina(w15, w25, MemOperand(c6)),
                    "ldsmina w15, w25, [c6]");
  COMPARE_MACRO_C64(Ldsminal(w6, w21, MemOperand(c29)),
                    "ldsminal w6, w21, [c29]");
  COMPARE_MACRO_C64(Ldsminl(w18, w18, MemOperand(c3)),
                    "ldsminl w18, w18, [c3]");

  // ldsmin*b
  COMPARE_MACRO_C64(Ldsminb(w1, w30, MemOperand(c1)), "ldsminb w1, w30, [c1]");
  COMPARE_MACRO_C64(Ldsminab(w4, w26, MemOperand(c1)),
                    "ldsminab w4, w26, [c1]");
  COMPARE_MACRO_C64(Ldsminalb(w30, w20, MemOperand(c3)),
                    "ldsminalb w30, w20, [c3]");
  COMPARE_MACRO_C64(Ldsminlb(w27, w5, MemOperand(c6)),
                    "ldsminlb w27, w5, [c6]");

  // ldsmin*h
  COMPARE_MACRO_C64(Ldsminh(w25, w19, MemOperand(c29)),
                    "ldsminh w25, w19, [c29]");
  COMPARE_MACRO_C64(Ldsminah(w4, w25, MemOperand(c4)),
                    "ldsminah w4, w25, [c4]");
  COMPARE_MACRO_C64(Ldsminalh(w2, w11, MemOperand(c27)),
                    "ldsminalh w2, w11, [c27]");
  COMPARE_MACRO_C64(Ldsminlh(w23, w23, MemOperand(c7)),
                    "ldsminlh w23, w23, [c7]");
}

TEST(c64_ldsmin_x) {
  SETUP();

  // ldsmin*
  COMPARE_MACRO_C64(Ldsmin(x1, x15, MemOperand(c29)), "ldsmin x1, x15, [c29]");
  COMPARE_MACRO_C64(Ldsmina(x8, x9, MemOperand(c3)), "ldsmina x8, x9, [c3]");
  COMPARE_MACRO_C64(Ldsminal(x13, x30, MemOperand(c26)),
                    "ldsminal x13, x30, [c26]");
  COMPARE_MACRO_C64(Ldsminl(x25, x1, MemOperand(c3)), "ldsminl x25, x1, [c3]");
}

TEST(c64_ldumax_w) {
  SETUP();

  // ldumax*
  COMPARE_MACRO_C64(Ldumax(w25, w2, MemOperand(c30)), "ldumax w25, w2, [c30]");
  COMPARE_MACRO_C64(Ldumaxa(w7, w10, MemOperand(c27)),
                    "ldumaxa w7, w10, [c27]");
  COMPARE_MACRO_C64(Ldumaxal(w0, w13, MemOperand(c26)),
                    "ldumaxal w0, w13, [c26]");
  COMPARE_MACRO_C64(Ldumaxl(w6, w12, MemOperand(c27)),
                    "ldumaxl w6, w12, [c27]");

  // ldumax*b
  COMPARE_MACRO_C64(Ldumaxb(w20, w3, MemOperand(c5)), "ldumaxb w20, w3, [c5]");
  COMPARE_MACRO_C64(Ldumaxab(w26, w24, MemOperand(c1)),
                    "ldumaxab w26, w24, [c1]");
  COMPARE_MACRO_C64(Ldumaxalb(w5, w0, MemOperand(c29)),
                    "ldumaxalb w5, w0, [c29]");
  COMPARE_MACRO_C64(Ldumaxlb(w8, w4, MemOperand(csp)),
                    "ldumaxlb w8, w4, [csp]");

  // ldumax*h
  COMPARE_MACRO_C64(Ldumaxh(w18, w12, MemOperand(c3)),
                    "ldumaxh w18, w12, [c3]");
  COMPARE_MACRO_C64(Ldumaxah(w9, w6, MemOperand(c7)), "ldumaxah w9, w6, [c7]");
  COMPARE_MACRO_C64(Ldumaxalh(w28, w3, MemOperand(c1)),
                    "ldumaxalh w28, w3, [c1]");
  COMPARE_MACRO_C64(Ldumaxlh(w7, w6, MemOperand(c1)), "ldumaxlh w7, w6, [c1]");
}

TEST(c64_ldumax_x) {
  SETUP();

  // ldumax*
  COMPARE_MACRO_C64(Ldumax(x11, x5, MemOperand(c28)), "ldumax x11, x5, [c28]");
  COMPARE_MACRO_C64(Ldumaxa(x11, x13, MemOperand(c27)),
                    "ldumaxa x11, x13, [c27]");
  COMPARE_MACRO_C64(Ldumaxal(x29, x24, MemOperand(c2)),
                    "ldumaxal x29, x24, [c2]");
  COMPARE_MACRO_C64(Ldumaxl(x19, x25, MemOperand(c26)),
                    "ldumaxl x19, x25, [c26]");
}

TEST(c64_ldumin_w) {
  SETUP();

  // ldumin*
  COMPARE_MACRO_C64(Ldumin(w5, w12, MemOperand(c1)), "ldumin w5, w12, [c1]");
  COMPARE_MACRO_C64(Ldumina(w0, w12, MemOperand(c5)), "ldumina w0, w12, [c5]");
  COMPARE_MACRO_C64(Lduminal(w10, w28, MemOperand(c29)),
                    "lduminal w10, w28, [c29]");
  COMPARE_MACRO_C64(Lduminl(w24, w22, MemOperand(c5)),
                    "lduminl w24, w22, [c5]");

  // ldumin*b
  COMPARE_MACRO_C64(Lduminb(w26, w0, MemOperand(c24)),
                    "lduminb w26, w0, [c24]");
  COMPARE_MACRO_C64(Lduminab(w9, w29, MemOperand(c4)),
                    "lduminab w9, w29, [c4]");
  COMPARE_MACRO_C64(Lduminalb(w5, w28, MemOperand(c29)),
                    "lduminalb w5, w28, [c29]");
  COMPARE_MACRO_C64(Lduminlb(w0, w1, MemOperand(c24)),
                    "lduminlb w0, w1, [c24]");

  // ldumin*h
  COMPARE_MACRO_C64(Lduminh(w30, w6, MemOperand(c27)),
                    "lduminh w30, w6, [c27]");
  COMPARE_MACRO_C64(Lduminah(w1, w25, MemOperand(c27)),
                    "lduminah w1, w25, [c27]");
  COMPARE_MACRO_C64(Lduminalh(w6, w21, MemOperand(c30)),
                    "lduminalh w6, w21, [c30]");
  COMPARE_MACRO_C64(Lduminlh(w6, w0, MemOperand(c5)), "lduminlh w6, w0, [c5]");
}

TEST(c64_ldumin_x) {
  SETUP();

  // ldumin*
  COMPARE_MACRO_C64(Ldumin(x3, x12, MemOperand(c29)), "ldumin x3, x12, [c29]");
  COMPARE_MACRO_C64(Ldumina(x7, x12, MemOperand(c1)), "ldumina x7, x12, [c1]");
  COMPARE_MACRO_C64(Lduminal(x30, x10, MemOperand(c26)),
                    "lduminal x30, x10, [c26]");
  COMPARE_MACRO_C64(Lduminl(x14, x12, MemOperand(c24)),
                    "lduminl x14, x12, [c24]");
}

TEST(c64_ldur_stur) {
  SETUP();

  COMPARE_MACRO_C64(Ldr(x27, MemOperand(c5, 1)), "ldur x27, [c5, #1]");
  COMPARE_MACRO_C64(Ldr(w21, MemOperand(c5, 1)), "ldur w21, [c5, #1]");
  COMPARE_MACRO_C64(Ldrh(w7, MemOperand(c25, 1)), "ldurh w7, [c25, #1]");
  COMPARE_MACRO_C64(Ldrb(w24, MemOperand(c29, -1)), "ldurb w24, [c29, #-1]");

  COMPARE_MACRO_C64(Ldrsb(x9, MemOperand(c29, -1)), "ldursb x9, [c29, #-1]");
  COMPARE_MACRO_C64(Ldrsb(w11, MemOperand(c28, -1)), "ldursb w11, [c28, #-1]");
  COMPARE_MACRO_C64(Ldrsh(x10, MemOperand(c3, 1)), "ldursh x10, [c3, #1]");
  COMPARE_MACRO_C64(Ldrsh(w4, MemOperand(c29, 1)), "ldursh w4, [c29, #1]");
  COMPARE_MACRO_C64(Ldrsw(x21, MemOperand(c28, 1)), "ldursw x21, [c28, #1]");

  COMPARE_MACRO_C64(Str(x26, MemOperand(c30, 1)), "stur x26, [c30, #1]");
  COMPARE_MACRO_C64(Str(w27, MemOperand(c4, 1)), "stur w27, [c4, #1]");
  COMPARE_MACRO_C64(Strh(w21, MemOperand(c4, 1)), "sturh w21, [c4, #1]");
  COMPARE_MACRO_C64(Strb(w3, MemOperand(c24, -1)), "sturb w3, [c24, #-1]");
}

TEST(c64_ldx_stx) {
  SETUP();

  COMPARE_MACRO_C64(Ldxp(x19, x7, MemOperand(c5)), "ldxp x19, x7, [c5]");
  COMPARE_MACRO_C64(Ldxp(w1, w24, MemOperand(c1)), "ldxp w1, w24, [c1]");
  COMPARE_MACRO_C64(Ldxr(x18, MemOperand(c3)), "ldxr x18, [c3]");
  COMPARE_MACRO_C64(Ldxr(w23, MemOperand(c3)), "ldxr w23, [c3]");
  COMPARE_MACRO_C64(Ldxrb(w11, MemOperand(c2)), "ldxrb w11, [c2]");
  COMPARE_MACRO_C64(Ldxrh(w4, MemOperand(c3)), "ldxrh w4, [c3]");

  COMPARE_MACRO_C64(Stxp(w11, x19, x0, MemOperand(c2)),
                    "stxp w11, x19, x0, [c2]");
  COMPARE_MACRO_C64(Stxp(w14, w22, w19, MemOperand(c2)),
                    "stxp w14, w22, w19, [c2]");
  COMPARE_MACRO_C64(Stxr(w27, x6, MemOperand(c26)), "stxr w27, x6, [c26]");
  COMPARE_MACRO_C64(Stxr(w0, w23, MemOperand(c26)), "stxr w0, w23, [c26]");
  COMPARE_MACRO_C64(Stxrb(w10, w12, MemOperand(c27)), "stxrb w10, w12, [c27]");
  COMPARE_MACRO_C64(Stxrh(w12, w25, MemOperand(c3)), "stxrh w12, w25, [c3]");
}

TEST(c64_stadd_w) {
  SETUP();

  // stadd*
  COMPARE_MACRO_C64(Stadd(w11, MemOperand(c26)), "stadd w11, [c26]");
  COMPARE_MACRO_C64(Staddl(w3, MemOperand(c27)), "staddl w3, [c27]");

  // stadd*b
  COMPARE_MACRO_C64(Staddb(w25, MemOperand(c1)), "staddb w25, [c1]");
  COMPARE_MACRO_C64(Staddlb(w8, MemOperand(c30)), "staddlb w8, [c30]");

  // stadd*h
  COMPARE_MACRO_C64(Staddh(w29, MemOperand(c28)), "staddh w29, [c28]");
  COMPARE_MACRO_C64(Staddlh(w1, MemOperand(c30)), "staddlh w1, [c30]");
}

TEST(c64_stadd_x) {
  SETUP();

  // stadd*
  COMPARE_MACRO_C64(Stadd(x26, MemOperand(c24)), "stadd x26, [c24]");
  COMPARE_MACRO_C64(Staddl(x25, MemOperand(c30)), "staddl x25, [c30]");
}

TEST(c64_stclr_w) {
  SETUP();

  // stclr*
  COMPARE_MACRO_C64(Stclr(w18, MemOperand(c25)), "stclr w18, [c25]");
  COMPARE_MACRO_C64(Stclrl(w10, MemOperand(c5)), "stclrl w10, [c5]");

  // stclr*b
  COMPARE_MACRO_C64(Stclrb(w11, MemOperand(c29)), "stclrb w11, [c29]");
  COMPARE_MACRO_C64(Stclrlb(w28, MemOperand(c26)), "stclrlb w28, [c26]");

  // stclr*h
  COMPARE_MACRO_C64(Stclrh(w19, MemOperand(c26)), "stclrh w19, [c26]");
  COMPARE_MACRO_C64(Stclrlh(w25, MemOperand(c29)), "stclrlh w25, [c29]");
}

TEST(c64_stclr_x) {
  SETUP();

  // stclr*
  COMPARE_MACRO_C64(Stclr(x13, MemOperand(c6)), "stclr x13, [c6]");
  COMPARE_MACRO_C64(Stclrl(x26, MemOperand(c7)), "stclrl x26, [c7]");
}

TEST(c64_steor_w) {
  SETUP();

  // steor*
  COMPARE_MACRO_C64(Steor(w6, MemOperand(c27)), "steor w6, [c27]");
  COMPARE_MACRO_C64(Steorl(w14, MemOperand(c5)), "steorl w14, [c5]");

  // steor*b
  COMPARE_MACRO_C64(Steorb(w4, MemOperand(c2)), "steorb w4, [c2]");
  COMPARE_MACRO_C64(Steorlb(w0, MemOperand(c1)), "steorlb w0, [c1]");

  // steor*h
  COMPARE_MACRO_C64(Steorh(w25, MemOperand(c30)), "steorh w25, [c30]");
  COMPARE_MACRO_C64(Steorlh(w22, MemOperand(c5)), "steorlh w22, [c5]");
}

TEST(c64_steor_x) {
  SETUP();

  // steor*
  COMPARE_MACRO_C64(Steor(x0, MemOperand(c0)), "steor x0, [c0]");
  COMPARE_MACRO_C64(Steorl(x1, MemOperand(c4)), "steorl x1, [c4]");
}

TEST(c64_stlr) {
  SETUP();

  COMPARE_MACRO_C64(Stlr(x0, MemOperand(c28)), "stlr x0, [c28]");
  COMPARE_MACRO_C64(Stlr(w7, MemOperand(c30)), "stlr w7, [c30]");
  COMPARE_MACRO_C64(Stlrb(w3, MemOperand(c6)), "stlrb w3, [c6]");
  COMPARE_MACRO_C64(Stlrh(w11, MemOperand(c4)), "stlrh w11, [c4]");
}

TEST(c64_stlx) {
  SETUP();

  COMPARE_MACRO_C64(Stlxp(w6, x2, x1, MemOperand(c4)),
                    "stlxp w6, x2, x1, [c4]");
  COMPARE_MACRO_C64(Stlxp(w9, w26, w27, MemOperand(c26)),
                    "stlxp w9, w26, w27, [c26]");
  COMPARE_MACRO_C64(Stlxr(w18, x24, MemOperand(csp)), "stlxr w18, x24, [csp]");
  COMPARE_MACRO_C64(Stlxr(w5, w24, MemOperand(c30)), "stlxr w5, w24, [c30]");
  COMPARE_MACRO_C64(Stlxrb(w5, w12, MemOperand(c25)), "stlxrb w5, w12, [c25]");
  COMPARE_MACRO_C64(Stlxrh(w12, w19, MemOperand(c3)), "stlxrh w12, w19, [c3]");
}

TEST(c64_stset_w) {
  SETUP();

  // stset*
  COMPARE_MACRO_C64(Stset(w21, MemOperand(c28)), "stset w21, [c28]");
  COMPARE_MACRO_C64(Stsetl(w18, MemOperand(c28)), "stsetl w18, [c28]");

  // stset*b
  COMPARE_MACRO_C64(Stsetb(w14, MemOperand(c0)), "stsetb w14, [c0]");
  COMPARE_MACRO_C64(Stsetlb(w28, MemOperand(c2)), "stsetlb w28, [c2]");

  // stset*h
  COMPARE_MACRO_C64(Stseth(w3, MemOperand(c6)), "stseth w3, [c6]");
  COMPARE_MACRO_C64(Stsetlh(w20, MemOperand(c6)), "stsetlh w20, [c6]");
}

TEST(c64_stset_x) {
  SETUP();

  // stset*
  COMPARE_MACRO_C64(Stset(x13, MemOperand(c6)), "stset x13, [c6]");
  COMPARE_MACRO_C64(Stsetl(x9, MemOperand(c30)), "stsetl x9, [c30]");
}

TEST(c64_stsmax_w) {
  SETUP();

  // stsmax*
  COMPARE_MACRO_C64(Stsmax(w13, MemOperand(c26)), "stsmax w13, [c26]");
  COMPARE_MACRO_C64(Stsmaxl(w9, MemOperand(c6)), "stsmaxl w9, [c6]");

  // stsmax*b
  COMPARE_MACRO_C64(Stsmaxb(w2, MemOperand(c24)), "stsmaxb w2, [c24]");
  COMPARE_MACRO_C64(Stsmaxlb(w14, MemOperand(c7)), "stsmaxlb w14, [c7]");

  // stsmax*h
  COMPARE_MACRO_C64(Stsmaxh(w23, MemOperand(c3)), "stsmaxh w23, [c3]");
  COMPARE_MACRO_C64(Stsmaxlh(w18, MemOperand(c28)), "stsmaxlh w18, [c28]");
}

TEST(c64_stsmax_x) {
  SETUP();

  // stsmax*
  COMPARE_MACRO_C64(Stsmax(x0, MemOperand(c24)), "stsmax x0, [c24]");
  COMPARE_MACRO_C64(Stsmaxl(x15, MemOperand(c25)), "stsmaxl x15, [c25]");
}

TEST(c64_stsmin_w) {
  SETUP();

  // stsmin*
  COMPARE_MACRO_C64(Stsmin(w5, MemOperand(c29)), "stsmin w5, [c29]");
  COMPARE_MACRO_C64(Stsminl(w5, MemOperand(c4)), "stsminl w5, [c4]");

  // stsmin*b
  COMPARE_MACRO_C64(Stsminb(w11, MemOperand(c3)), "stsminb w11, [c3]");
  COMPARE_MACRO_C64(Stsminlb(w19, MemOperand(c6)), "stsminlb w19, [c6]");

  // stsmin*h
  COMPARE_MACRO_C64(Stsminh(w7, MemOperand(c0)), "stsminh w7, [c0]");
  COMPARE_MACRO_C64(Stsminlh(w8, MemOperand(c25)), "stsminlh w8, [c25]");
}

TEST(c64_stsmin_x) {
  SETUP();

  // stsmin*
  COMPARE_MACRO_C64(Stsmin(x19, MemOperand(c27)), "stsmin x19, [c27]");
  COMPARE_MACRO_C64(Stsminl(x26, MemOperand(c4)), "stsminl x26, [c4]");
}

TEST(c64_stumax_w) {
  SETUP();

  // stumax*
  COMPARE_MACRO_C64(Stumax(w9, MemOperand(c3)), "stumax w9, [c3]");
  COMPARE_MACRO_C64(Stumaxl(w18, MemOperand(c26)), "stumaxl w18, [c26]");

  // stumax*b
  COMPARE_MACRO_C64(Stumaxb(w8, MemOperand(c28)), "stumaxb w8, [c28]");
  COMPARE_MACRO_C64(Stumaxlb(w2, MemOperand(c29)), "stumaxlb w2, [c29]");

  // stumax*h
  COMPARE_MACRO_C64(Stumaxh(w26, MemOperand(c30)), "stumaxh w26, [c30]");
  COMPARE_MACRO_C64(Stumaxlh(w15, MemOperand(c30)), "stumaxlh w15, [c30]");
}

TEST(c64_stumax_x) {
  SETUP();

  // stumax*
  COMPARE_MACRO_C64(Stumax(x3, MemOperand(c28)), "stumax x3, [c28]");
  COMPARE_MACRO_C64(Stumaxl(x20, MemOperand(c5)), "stumaxl x20, [c5]");
}

TEST(c64_stumin_w) {
  SETUP();

  // stumin*
  COMPARE_MACRO_C64(Stumin(w9, MemOperand(c2)), "stumin w9, [c2]");
  COMPARE_MACRO_C64(Stuminl(w26, MemOperand(c4)), "stuminl w26, [c4]");

  // stumin*b
  COMPARE_MACRO_C64(Stuminb(w5, MemOperand(c25)), "stuminb w5, [c25]");
  COMPARE_MACRO_C64(Stuminlb(w11, MemOperand(c29)), "stuminlb w11, [c29]");

  // stumin*h
  COMPARE_MACRO_C64(Stuminh(w14, MemOperand(c24)), "stuminh w14, [c24]");
  COMPARE_MACRO_C64(Stuminlh(w30, MemOperand(c7)), "stuminlh w30, [c7]");
}

TEST(c64_stumin_x) {
  SETUP();

  // stumin*
  COMPARE_MACRO_C64(Stumin(x4, MemOperand(c5)), "stumin x4, [c5]");
  COMPARE_MACRO_C64(Stuminl(x20, MemOperand(c26)), "stuminl x20, [c26]");
}

TEST(c64_swp_w) {
  SETUP();

  // swp*
  COMPARE_MACRO_C64(Swp(w3, w6, MemOperand(c3)), "swp w3, w6, [c3]");
  COMPARE_MACRO_C64(Swpa(w9, w2, MemOperand(c6)), "swpa w9, w2, [c6]");
  COMPARE_MACRO_C64(Swpal(w11, w15, MemOperand(c25)), "swpal w11, w15, [c25]");
  COMPARE_MACRO_C64(Swpl(w7, w2, MemOperand(c25)), "swpl w7, w2, [c25]");

  // swp*b
  COMPARE_MACRO_C64(Swpb(w26, w29, MemOperand(c25)), "swpb w26, w29, [c25]");
  COMPARE_MACRO_C64(Swpab(w27, w12, MemOperand(csp)), "swpab w27, w12, [csp]");
  COMPARE_MACRO_C64(Swpalb(w6, w20, MemOperand(c4)), "swpalb w6, w20, [c4]");
  COMPARE_MACRO_C64(Swplb(w27, w21, MemOperand(c24)), "swplb w27, w21, [c24]");

  // swp*h
  COMPARE_MACRO_C64(Swph(w18, w1, MemOperand(c25)), "swph w18, w1, [c25]");
  COMPARE_MACRO_C64(Swpah(w0, w21, MemOperand(c1)), "swpah w0, w21, [c1]");
  COMPARE_MACRO_C64(Swpalh(w7, w6, MemOperand(c29)), "swpalh w7, w6, [c29]");
  COMPARE_MACRO_C64(Swplh(w20, w23, MemOperand(c1)), "swplh w20, w23, [c1]");
}

TEST(c64_swp_x) {
  SETUP();

  // swp*
  COMPARE_MACRO_C64(Swp(x4, x10, MemOperand(c6)), "swp x4, x10, [c6]");
  COMPARE_MACRO_C64(Swpa(x12, x8, MemOperand(c30)), "swpa x12, x8, [c30]");
  COMPARE_MACRO_C64(Swpal(x0, x22, MemOperand(c1)), "swpal x0, x22, [c1]");
  COMPARE_MACRO_C64(Swpl(x5, x11, MemOperand(c28)), "swpl x5, x11, [c28]");
}

}  // namespace aarch64
}  // namespace vixl
