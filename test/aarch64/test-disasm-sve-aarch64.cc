// Copyright 2019, VIXL authors
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
#include "aarch64/macro-assembler-aarch64.h"
#include "test-disasm-aarch64.h"

namespace vixl {
namespace aarch64 {

TEST(sve) {
  SETUP();

  // TODO: Replace these tests when the disassembler is more capable.
  COMPARE_PREFIX(asrr(z0.VnB(), p7.Merging(), z0.VnB(), z1.VnB()),
                 "asrr z0.b, p7/m, z0.b, z1.b");
  COMPARE_PREFIX(fcmeq(p6.VnD(), p7.Zeroing(), z0.VnD(), z1.VnD()),
                 "fcmeq p6.d, p7/z, z0.d, z1.d");
  COMPARE_PREFIX(mla(z0.VnB(), p7.Merging(), z0.VnB(), z1.VnB()),
                 "mla z0.b, p7/m, z0.b, z1.b");
  COMPARE_PREFIX(mla(z1.VnS(), p7.Merging(), z1.VnS(), z0.VnS()),
                 "mla z1.s, p7/m, z1.s, z0.s");
  COMPARE_PREFIX(asr(z1.VnB(), z0.VnB()), "asr z1.<T>, z0.<T>, #<const>");
  COMPARE_PREFIX(lsl(z0.VnH(), z1.VnH()), "lsl z0.<T>, z1.<T>, #<const>");
  COMPARE_PREFIX(lsr(z1.VnS(), z0.VnS()), "lsr z1.<T>, z0.<T>, #<const>");
  COMPARE_PREFIX(and_(p6.VnB(), p7.Zeroing(), p6.VnB(), p7.VnB()),
                 "and p6.b, p7/z, p6.b, p7.b");
  COMPARE_PREFIX(rev(p7.VnB(), p6.VnB()), "rev p7.b, p6.b");
  COMPARE_PREFIX(splice(z0.VnH(), p7, z0.VnH(), z1.VnH()),
                 "splice z0.h, p7, z0.h, z1.h");
  COMPARE_PREFIX(fnmad(z0.VnD(), p6.Merging(), z1.VnD(), z0.VnD()),
                 "fnmad z0.d, p6/m, z1.d, z0.d");

  CLEANUP();
}

TEST(sve_address_generation) {
  SETUP();

#if 0
  COMPARE_PREFIX(adr(z19.VnD(), z22.VnD(), z11.VnD()), "adr <Zd>.D, [<Zn>.D, <Zm>.D, SXTW{ <amount>}]");
  COMPARE_PREFIX(adr(z30.VnD(), z14.VnD(), z24.VnD()), "adr <Zd>.D, [<Zn>.D, <Zm>.D, UXTW{ <amount>}]");
  COMPARE_PREFIX(adr(z8.Vn?(), z16.Vn?(), z16.Vn?()), "adr <Zd>.<T>, [<Zn>.<T>, <Zm>.<T>{, <mod> <amount>}]");
#endif

  CLEANUP();
}

TEST(sve_bitwise_imm) {
  SETUP();

  // The assembler will necessarily encode an immediate in the simplest bitset.
  COMPARE_PREFIX(and_(z2.VnD(), z2.VnD(), 0x0000ffff0000ffff),
                 "and z2.s, z2.s, #0xffff");
  COMPARE_PREFIX(dupm(z15.VnS(), 0x7ffc7ffc), "dupm z15.h, #0x7ffc");
  COMPARE_PREFIX(eor(z26.VnH(), z26.VnH(), 0x7ff8),
                 "eor z26.h, z26.h, #0x7ff8");
  COMPARE_PREFIX(orr(z13.VnB(), z13.VnB(), 0x78), "orr z13.b, z13.b, #0x78");

  CLEANUP();
}

TEST(sve_bitwise_logical_unpredicated) {
  SETUP();

  COMPARE_PREFIX(and_(z12.VnD(), z5.VnD(), z29.VnD()),
                 "and z12.d, z5.d, z29.d");
  COMPARE_PREFIX(bic(z11.VnD(), z15.VnD(), z9.VnD()), "bic z11.d, z15.d, z9.d");
  COMPARE_PREFIX(eor(z9.VnD(), z31.VnD(), z29.VnD()), "eor z9.d, z31.d, z29.d");
  COMPARE_PREFIX(orr(z17.VnD(), z8.VnD(), z19.VnD()), "orr z17.d, z8.d, z19.d");

  CLEANUP();
}

TEST(sve_bitwise_shift_predicated) {
  SETUP();

#if 0
  COMPARE_PREFIX(asrd(z0.Vn?(), p4.Merging(), z0.Vn?()), "asrd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, #<const>");
  COMPARE_PREFIX(asrr(z20.VnB(), p3.Merging(), z20.VnB(), z11.VnB()), "asrr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(asrr(z20.VnH(), p3.Merging(), z20.VnH(), z11.VnH()), "asrr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(asrr(z20.VnS(), p3.Merging(), z20.VnS(), z11.VnS()), "asrr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(asrr(z20.VnD(), p3.Merging(), z20.VnD(), z11.VnD()), "asrr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(asr(z8.Vn?(), p7.Merging(), z8.Vn?()), "asr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, #<const>");
  COMPARE_PREFIX(asr(z4.VnB(), p0.Merging(), z4.VnB(), z30.VnD()), "asr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D");
  COMPARE_PREFIX(asr(z4.VnH(), p0.Merging(), z4.VnH(), z30.VnD()), "asr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D");
  COMPARE_PREFIX(asr(z4.VnS(), p0.Merging(), z4.VnS(), z30.VnD()), "asr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D");
  COMPARE_PREFIX(asr(z26.VnB(), p2.Merging(), z26.VnB(), z17.VnB()), "asr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(asr(z26.VnH(), p2.Merging(), z26.VnH(), z17.VnH()), "asr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(asr(z26.VnS(), p2.Merging(), z26.VnS(), z17.VnS()), "asr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(asr(z26.VnD(), p2.Merging(), z26.VnD(), z17.VnD()), "asr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lslr(z30.VnB(), p1.Merging(), z30.VnB(), z26.VnB()), "lslr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lslr(z30.VnH(), p1.Merging(), z30.VnH(), z26.VnH()), "lslr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lslr(z30.VnS(), p1.Merging(), z30.VnS(), z26.VnS()), "lslr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lslr(z30.VnD(), p1.Merging(), z30.VnD(), z26.VnD()), "lslr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lsl(z29.Vn?(), p6.Merging(), z29.Vn?()), "lsl <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, #<const>");
  COMPARE_PREFIX(lsl(z13.VnB(), p7.Merging(), z13.VnB(), z18.VnD()), "lsl <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D");
  COMPARE_PREFIX(lsl(z13.VnH(), p7.Merging(), z13.VnH(), z18.VnD()), "lsl <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D");
  COMPARE_PREFIX(lsl(z13.VnS(), p7.Merging(), z13.VnS(), z18.VnD()), "lsl <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D");
  COMPARE_PREFIX(lsl(z14.VnB(), p6.Merging(), z14.VnB(), z25.VnB()), "lsl <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lsl(z14.VnH(), p6.Merging(), z14.VnH(), z25.VnH()), "lsl <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lsl(z14.VnS(), p6.Merging(), z14.VnS(), z25.VnS()), "lsl <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lsl(z14.VnD(), p6.Merging(), z14.VnD(), z25.VnD()), "lsl <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lsrr(z3.VnB(), p1.Merging(), z3.VnB(), z16.VnB()), "lsrr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lsrr(z3.VnH(), p1.Merging(), z3.VnH(), z16.VnH()), "lsrr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lsrr(z3.VnS(), p1.Merging(), z3.VnS(), z16.VnS()), "lsrr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lsrr(z3.VnD(), p1.Merging(), z3.VnD(), z16.VnD()), "lsrr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lsr(z24.Vn?(), p2.Merging(), z24.Vn?()), "lsr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, #<const>");
  COMPARE_PREFIX(lsr(z1.VnB(), p4.Merging(), z1.VnB(), z14.VnD()), "lsr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D");
  COMPARE_PREFIX(lsr(z1.VnH(), p4.Merging(), z1.VnH(), z14.VnD()), "lsr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D");
  COMPARE_PREFIX(lsr(z1.VnS(), p4.Merging(), z1.VnS(), z14.VnD()), "lsr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D");
  COMPARE_PREFIX(lsr(z29.VnB(), p7.Merging(), z29.VnB(), z13.VnB()), "lsr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lsr(z29.VnH(), p7.Merging(), z29.VnH(), z13.VnH()), "lsr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lsr(z29.VnS(), p7.Merging(), z29.VnS(), z13.VnS()), "lsr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(lsr(z29.VnD(), p7.Merging(), z29.VnD(), z13.VnD()), "lsr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
#endif

  CLEANUP();
}

TEST(sve_bitwise_shift_unpredicated) {
  SETUP();

#if 0
  COMPARE_PREFIX(asr(z4.Vn?(), z27.Vn?()), "asr <Zd>.<T>, <Zn>.<T>, #<const>");
  COMPARE_PREFIX(asr(z12.VnB(), z8.VnB(), z14.VnD()), "asr <Zd>.<T>, <Zn>.<T>, <Zm>.D");
  COMPARE_PREFIX(asr(z12.VnH(), z8.VnH(), z14.VnD()), "asr <Zd>.<T>, <Zn>.<T>, <Zm>.D");
  COMPARE_PREFIX(asr(z12.VnS(), z8.VnS(), z14.VnD()), "asr <Zd>.<T>, <Zn>.<T>, <Zm>.D");
  COMPARE_PREFIX(lsl(z12.Vn?(), z18.Vn?()), "lsl <Zd>.<T>, <Zn>.<T>, #<const>");
  COMPARE_PREFIX(lsl(z21.VnB(), z16.VnB(), z15.VnD()), "lsl <Zd>.<T>, <Zn>.<T>, <Zm>.D");
  COMPARE_PREFIX(lsl(z21.VnH(), z16.VnH(), z15.VnD()), "lsl <Zd>.<T>, <Zn>.<T>, <Zm>.D");
  COMPARE_PREFIX(lsl(z21.VnS(), z16.VnS(), z15.VnD()), "lsl <Zd>.<T>, <Zn>.<T>, <Zm>.D");
  COMPARE_PREFIX(lsr(z14.Vn?(), z7.Vn?()), "lsr <Zd>.<T>, <Zn>.<T>, #<const>");
  COMPARE_PREFIX(lsr(z16.VnB(), z19.VnB(), z2.VnD()), "lsr <Zd>.<T>, <Zn>.<T>, <Zm>.D");
  COMPARE_PREFIX(lsr(z16.VnH(), z19.VnH(), z2.VnD()), "lsr <Zd>.<T>, <Zn>.<T>, <Zm>.D");
  COMPARE_PREFIX(lsr(z16.VnS(), z19.VnS(), z2.VnD()), "lsr <Zd>.<T>, <Zn>.<T>, <Zm>.D");
#endif

  CLEANUP();
}

TEST(sve_element_count) {
  SETUP();

#if 0
  COMPARE_PREFIX(cntb(x7, int pattern), "cntb <Xd>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(cntd(x1, int pattern), "cntd <Xd>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(cnth(x4, int pattern), "cnth <Xd>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(cntw(x26, int pattern), "cntw <Xd>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(decb(x1, int pattern), "decb <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(decd(x12, int pattern), "decd <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(decd(z4.VnD(), int pattern), "decd <Zdn>.D{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(dech(x9, int pattern), "dech <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(dech(z22.VnH(), int pattern), "dech <Zdn>.H{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(decw(x3, int pattern), "decw <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(decw(z23.VnS(), int pattern), "decw <Zdn>.S{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(incb(x19, int pattern), "incb <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(incd(x8, int pattern), "incd <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(incd(z8.VnD(), int pattern), "incd <Zdn>.D{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(inch(x10, int pattern), "inch <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(inch(z8.VnH(), int pattern), "inch <Zdn>.H{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(incw(x28, int pattern), "incw <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(incw(z22.VnS(), int pattern), "incw <Zdn>.S{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqdecb(x27, w27, int pattern), "sqdecb <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqdecb(x30, int pattern), "sqdecb <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqdecd(x0, w0, int pattern), "sqdecd <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqdecd(x8, int pattern), "sqdecd <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqdecd(z13.VnD(), int pattern), "sqdecd <Zdn>.D{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqdech(x8, w8, int pattern), "sqdech <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqdech(x17, int pattern), "sqdech <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqdech(z16.VnH(), int pattern), "sqdech <Zdn>.H{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqdecw(x1, w1, int pattern), "sqdecw <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqdecw(x1, int pattern), "sqdecw <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqdecw(z9.VnS(), int pattern), "sqdecw <Zdn>.S{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqincb(x24, w24, int pattern), "sqincb <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqincb(x22, int pattern), "sqincb <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqincd(x12, w12, int pattern), "sqincd <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqincd(x28, int pattern), "sqincd <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqincd(z18.VnD(), int pattern), "sqincd <Zdn>.D{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqinch(x19, w19, int pattern), "sqinch <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqinch(x14, int pattern), "sqinch <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqinch(z30.VnH(), int pattern), "sqinch <Zdn>.H{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqincw(x0, w0, int pattern), "sqincw <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqincw(x5, int pattern), "sqincw <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(sqincw(z17.VnS(), int pattern), "sqincw <Zdn>.S{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqdecb(w12, int pattern), "uqdecb <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqdecb(x27, int pattern), "uqdecb <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqdecd(w28, int pattern), "uqdecd <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqdecd(x24, int pattern), "uqdecd <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqdecd(z10.VnD(), int pattern), "uqdecd <Zdn>.D{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqdech(w25, int pattern), "uqdech <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqdech(x5, int pattern), "uqdech <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqdech(z25.VnH(), int pattern), "uqdech <Zdn>.H{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqdecw(w22, int pattern), "uqdecw <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqdecw(x11, int pattern), "uqdecw <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqdecw(z1.VnS(), int pattern), "uqdecw <Zdn>.S{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqincb(w28, int pattern), "uqincb <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqincb(x31, int pattern), "uqincb <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqincd(w9, int pattern), "uqincd <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqincd(x21, int pattern), "uqincd <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqincd(z12.VnD(), int pattern), "uqincd <Zdn>.D{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqinch(w14, int pattern), "uqinch <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqinch(x21, int pattern), "uqinch <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqinch(z28.VnH(), int pattern), "uqinch <Zdn>.H{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqincw(w22, int pattern), "uqincw <Wdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqincw(x17, int pattern), "uqincw <Xdn>{, <pattern>{, MUL #<imm>}}");
  COMPARE_PREFIX(uqincw(z0.VnS(), int pattern), "uqincw <Zdn>.S{, <pattern>{, MUL #<imm>}}");
#endif

  CLEANUP();
}

TEST(sve_fp_accumulating_reduction) {
  SETUP();

#if 0
  COMPARE_PREFIX(fadda(h10, p2, h10, z0.VnH()), "fadda <V><dn>, <Pg>, <V><dn>, <Zm>.<T>");
  COMPARE_PREFIX(fadda(s10, p2, s10, z0.VnS()), "fadda <V><dn>, <Pg>, <V><dn>, <Zm>.<T>");
  COMPARE_PREFIX(fadda(d10, p2, d10, z0.VnD()), "fadda <V><dn>, <Pg>, <V><dn>, <Zm>.<T>");
#endif

  CLEANUP();
}

TEST(sve_fp_arithmetic_predicated) {
  SETUP();

#if 0
  COMPARE_PREFIX(fabd(z31.VnH(), p7.Merging(), z31.VnH(), z17.VnH()), "fabd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fabd(z31.VnS(), p7.Merging(), z31.VnS(), z17.VnS()), "fabd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fabd(z31.VnD(), p7.Merging(), z31.VnD(), z17.VnD()), "fabd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fadd(z18.VnH(), p0.Merging(), z18.VnH()), "fadd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fadd(z18.VnS(), p0.Merging(), z18.VnS()), "fadd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fadd(z18.VnD(), p0.Merging(), z18.VnD()), "fadd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fadd(z24.VnH(), p2.Merging(), z24.VnH(), z15.VnH()), "fadd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fadd(z24.VnS(), p2.Merging(), z24.VnS(), z15.VnS()), "fadd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fadd(z24.VnD(), p2.Merging(), z24.VnD(), z15.VnD()), "fadd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fdivr(z25.VnH(), p7.Merging(), z25.VnH(), z19.VnH()), "fdivr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fdivr(z25.VnS(), p7.Merging(), z25.VnS(), z19.VnS()), "fdivr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fdivr(z25.VnD(), p7.Merging(), z25.VnD(), z19.VnD()), "fdivr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fdiv(z19.VnH(), p4.Merging(), z19.VnH(), z14.VnH()), "fdiv <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fdiv(z19.VnS(), p4.Merging(), z19.VnS(), z14.VnS()), "fdiv <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fdiv(z19.VnD(), p4.Merging(), z19.VnD(), z14.VnD()), "fdiv <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmaxnm(z6.VnH(), p1.Merging(), z6.VnH()), "fmaxnm <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fmaxnm(z6.VnS(), p1.Merging(), z6.VnS()), "fmaxnm <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fmaxnm(z6.VnD(), p1.Merging(), z6.VnD()), "fmaxnm <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fmaxnm(z15.VnH(), p4.Merging(), z15.VnH(), z3.VnH()), "fmaxnm <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmaxnm(z15.VnS(), p4.Merging(), z15.VnS(), z3.VnS()), "fmaxnm <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmaxnm(z15.VnD(), p4.Merging(), z15.VnD(), z3.VnD()), "fmaxnm <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmax(z8.VnH(), p6.Merging(), z8.VnH()), "fmax <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fmax(z8.VnS(), p6.Merging(), z8.VnS()), "fmax <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fmax(z8.VnD(), p6.Merging(), z8.VnD()), "fmax <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fmax(z14.VnH(), p1.Merging(), z14.VnH(), z29.VnH()), "fmax <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmax(z14.VnS(), p1.Merging(), z14.VnS(), z29.VnS()), "fmax <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmax(z14.VnD(), p1.Merging(), z14.VnD(), z29.VnD()), "fmax <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fminnm(z26.VnH(), p0.Merging(), z26.VnH()), "fminnm <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fminnm(z26.VnS(), p0.Merging(), z26.VnS()), "fminnm <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fminnm(z26.VnD(), p0.Merging(), z26.VnD()), "fminnm <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fminnm(z19.VnH(), p2.Merging(), z19.VnH(), z29.VnH()), "fminnm <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fminnm(z19.VnS(), p2.Merging(), z19.VnS(), z29.VnS()), "fminnm <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fminnm(z19.VnD(), p2.Merging(), z19.VnD(), z29.VnD()), "fminnm <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmin(z22.VnH(), p0.Merging(), z22.VnH()), "fmin <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fmin(z22.VnS(), p0.Merging(), z22.VnS()), "fmin <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fmin(z22.VnD(), p0.Merging(), z22.VnD()), "fmin <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fmin(z1.VnH(), p2.Merging(), z1.VnH(), z30.VnH()), "fmin <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmin(z1.VnS(), p2.Merging(), z1.VnS(), z30.VnS()), "fmin <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmin(z1.VnD(), p2.Merging(), z1.VnD(), z30.VnD()), "fmin <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmulx(z30.VnH(), p6.Merging(), z30.VnH(), z20.VnH()), "fmulx <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmulx(z30.VnS(), p6.Merging(), z30.VnS(), z20.VnS()), "fmulx <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmulx(z30.VnD(), p6.Merging(), z30.VnD(), z20.VnD()), "fmulx <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmul(z21.VnH(), p3.Merging(), z21.VnH()), "fmul <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fmul(z21.VnS(), p3.Merging(), z21.VnS()), "fmul <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fmul(z21.VnD(), p3.Merging(), z21.VnD()), "fmul <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fmul(z26.VnH(), p2.Merging(), z26.VnH(), z6.VnH()), "fmul <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmul(z26.VnS(), p2.Merging(), z26.VnS(), z6.VnS()), "fmul <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmul(z26.VnD(), p2.Merging(), z26.VnD(), z6.VnD()), "fmul <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fscale(z8.VnH(), p3.Merging(), z8.VnH(), z6.VnH()), "fscale <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fscale(z8.VnS(), p3.Merging(), z8.VnS(), z6.VnS()), "fscale <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fscale(z8.VnD(), p3.Merging(), z8.VnD(), z6.VnD()), "fscale <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fsubr(z21.VnH(), p3.Merging(), z21.VnH()), "fsubr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fsubr(z21.VnS(), p3.Merging(), z21.VnS()), "fsubr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fsubr(z21.VnD(), p3.Merging(), z21.VnD()), "fsubr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fsubr(z16.VnH(), p5.Merging(), z16.VnH(), z15.VnH()), "fsubr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fsubr(z16.VnS(), p5.Merging(), z16.VnS(), z15.VnS()), "fsubr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fsubr(z16.VnD(), p5.Merging(), z16.VnD(), z15.VnD()), "fsubr <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fsub(z26.VnH(), p4.Merging(), z26.VnH()), "fsub <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fsub(z26.VnS(), p4.Merging(), z26.VnS()), "fsub <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fsub(z26.VnD(), p4.Merging(), z26.VnD()), "fsub <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>");
  COMPARE_PREFIX(fsub(z16.VnH(), p5.Merging(), z16.VnH(), z26.VnH()), "fsub <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fsub(z16.VnS(), p5.Merging(), z16.VnS(), z26.VnS()), "fsub <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fsub(z16.VnD(), p5.Merging(), z16.VnD(), z26.VnD()), "fsub <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(ftmad(z21.VnH(), z21.VnH(), z22.VnH(), int imm3), "ftmad <Zdn>.<T>, <Zdn>.<T>, <Zm>.<T>, #<imm>");
  COMPARE_PREFIX(ftmad(z21.VnS(), z21.VnS(), z22.VnS(), int imm3), "ftmad <Zdn>.<T>, <Zdn>.<T>, <Zm>.<T>, #<imm>");
  COMPARE_PREFIX(ftmad(z21.VnD(), z21.VnD(), z22.VnD(), int imm3), "ftmad <Zdn>.<T>, <Zdn>.<T>, <Zm>.<T>, #<imm>");
#endif

  CLEANUP();
}

TEST(sve_fp_arithmetic_unpredicated) {
  SETUP();

#if 0
  COMPARE_PREFIX(fadd(z5.VnH(), z4.VnH(), z12.VnH()), "fadd <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fadd(z5.VnS(), z4.VnS(), z12.VnS()), "fadd <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fadd(z5.VnD(), z4.VnD(), z12.VnD()), "fadd <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmul(z19.VnH(), z4.VnH(), z0.VnH()), "fmul <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmul(z19.VnS(), z4.VnS(), z0.VnS()), "fmul <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmul(z19.VnD(), z4.VnD(), z0.VnD()), "fmul <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(frecps(z14.VnH(), z29.VnH(), z18.VnH()), "frecps <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(frecps(z14.VnS(), z29.VnS(), z18.VnS()), "frecps <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(frecps(z14.VnD(), z29.VnD(), z18.VnD()), "frecps <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(frsqrts(z5.VnH(), z6.VnH(), z28.VnH()), "frsqrts <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(frsqrts(z5.VnS(), z6.VnS(), z28.VnS()), "frsqrts <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(frsqrts(z5.VnD(), z6.VnD(), z28.VnD()), "frsqrts <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fsub(z4.VnH(), z4.VnH(), z9.VnH()), "fsub <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fsub(z4.VnS(), z4.VnS(), z9.VnS()), "fsub <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fsub(z4.VnD(), z4.VnD(), z9.VnD()), "fsub <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(ftsmul(z21.VnH(), z17.VnH(), z24.VnH()), "ftsmul <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(ftsmul(z21.VnS(), z17.VnS(), z24.VnS()), "ftsmul <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(ftsmul(z21.VnD(), z17.VnD(), z24.VnD()), "ftsmul <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
#endif

  CLEANUP();
}

TEST(sve_fp_compare_vectors) {
  SETUP();

#if 0
  COMPARE_PREFIX(facge(p1.VnH(), p3.Zeroing(), z22.VnH(), z25.VnH()), "facge <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(facge(p1.VnS(), p3.Zeroing(), z22.VnS(), z25.VnS()), "facge <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(facge(p1.VnD(), p3.Zeroing(), z22.VnD(), z25.VnD()), "facge <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(facgt(p8.VnH(), p7.Zeroing(), z25.VnH(), z17.VnH()), "facgt <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(facgt(p8.VnS(), p7.Zeroing(), z25.VnS(), z17.VnS()), "facgt <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(facgt(p8.VnD(), p7.Zeroing(), z25.VnD(), z17.VnD()), "facgt <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmeq(p10.VnH(), p2.Zeroing(), z1.VnH(), z17.VnH()), "fcmeq <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmeq(p10.VnS(), p2.Zeroing(), z1.VnS(), z17.VnS()), "fcmeq <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmeq(p10.VnD(), p2.Zeroing(), z1.VnD(), z17.VnD()), "fcmeq <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmge(p0.VnH(), p0.Zeroing(), z1.VnH(), z0.VnH()), "fcmge <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmge(p0.VnS(), p0.Zeroing(), z1.VnS(), z0.VnS()), "fcmge <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmge(p0.VnD(), p0.Zeroing(), z1.VnD(), z0.VnD()), "fcmge <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmgt(p15.VnH(), p5.Zeroing(), z26.VnH(), z5.VnH()), "fcmgt <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmgt(p15.VnS(), p5.Zeroing(), z26.VnS(), z5.VnS()), "fcmgt <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmgt(p15.VnD(), p5.Zeroing(), z26.VnD(), z5.VnD()), "fcmgt <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmne(p2.VnH(), p1.Zeroing(), z9.VnH(), z4.VnH()), "fcmne <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmne(p2.VnS(), p1.Zeroing(), z9.VnS(), z4.VnS()), "fcmne <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmne(p2.VnD(), p1.Zeroing(), z9.VnD(), z4.VnD()), "fcmne <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmuo(p6.VnH(), p4.Zeroing(), z10.VnH(), z21.VnH()), "fcmuo <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmuo(p6.VnS(), p4.Zeroing(), z10.VnS(), z21.VnS()), "fcmuo <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fcmuo(p6.VnD(), p4.Zeroing(), z10.VnD(), z21.VnD()), "fcmuo <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>");
#endif

  CLEANUP();
}

TEST(sve_fp_compare_with_zero) {
  SETUP();

#if 0
  COMPARE_PREFIX(fcmeq(p9.VnH(), p1.Zeroing(), z17.VnH()), "fcmeq <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmeq(p9.VnS(), p1.Zeroing(), z17.VnS()), "fcmeq <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmeq(p9.VnD(), p1.Zeroing(), z17.VnD()), "fcmeq <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmge(p13.VnH(), p3.Zeroing(), z13.VnH()), "fcmge <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmge(p13.VnS(), p3.Zeroing(), z13.VnS()), "fcmge <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmge(p13.VnD(), p3.Zeroing(), z13.VnD()), "fcmge <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmgt(p10.VnH(), p2.Zeroing(), z24.VnH()), "fcmgt <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmgt(p10.VnS(), p2.Zeroing(), z24.VnS()), "fcmgt <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmgt(p10.VnD(), p2.Zeroing(), z24.VnD()), "fcmgt <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmle(p4.VnH(), p7.Zeroing(), z1.VnH()), "fcmle <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmle(p4.VnS(), p7.Zeroing(), z1.VnS()), "fcmle <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmle(p4.VnD(), p7.Zeroing(), z1.VnD()), "fcmle <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmlt(p15.VnH(), p7.Zeroing(), z9.VnH()), "fcmlt <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmlt(p15.VnS(), p7.Zeroing(), z9.VnS()), "fcmlt <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmlt(p15.VnD(), p7.Zeroing(), z9.VnD()), "fcmlt <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmne(p14.VnH(), p7.Zeroing(), z28.VnH()), "fcmne <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmne(p14.VnS(), p7.Zeroing(), z28.VnS()), "fcmne <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
  COMPARE_PREFIX(fcmne(p14.VnD(), p7.Zeroing(), z28.VnD()), "fcmne <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0");
#endif

  CLEANUP();
}

TEST(sve_fp_complex_addition) {
  SETUP();

#if 0
  COMPARE_PREFIX(fcadd(z12.VnH(), p5.Merging(), z12.VnH(), z13.VnH()), "fcadd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>, <const>");
  COMPARE_PREFIX(fcadd(z12.VnS(), p5.Merging(), z12.VnS(), z13.VnS()), "fcadd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>, <const>");
  COMPARE_PREFIX(fcadd(z12.VnD(), p5.Merging(), z12.VnD(), z13.VnD()), "fcadd <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>, <const>");
#endif

  CLEANUP();
}

TEST(sve_fp_complex_mul_add) {
  SETUP();

#if 0
  COMPARE_PREFIX(fcmla(z19.VnH(), p7.Merging(), z16.VnH(), z0.VnH()), "fcmla <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>, <const>");
  COMPARE_PREFIX(fcmla(z19.VnS(), p7.Merging(), z16.VnS(), z0.VnS()), "fcmla <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>, <const>");
  COMPARE_PREFIX(fcmla(z19.VnD(), p7.Merging(), z16.VnD(), z0.VnD()), "fcmla <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>, <const>");
#endif

  CLEANUP();
}

TEST(sve_fp_complex_mul_add_index) {
  SETUP();

#if 0
  COMPARE_PREFIX(fcmla(z30.VnH(), z20.VnH()), "fcmla <Zda>.H, <Zn>.H, <Zm>.H[<imm>], <const>");
  COMPARE_PREFIX(fcmla(z19.VnS(), z20.VnS()), "fcmla <Zda>.S, <Zn>.S, <Zm>.S[<imm>], <const>");
#endif

  CLEANUP();
}

TEST(sve_fp_fast_reduction) {
  SETUP();

#if 0
  COMPARE_PREFIX(faddv(h26, p6, z16.VnH()), "faddv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(faddv(s26, p6, z16.VnS()), "faddv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(faddv(d26, p6, z16.VnD()), "faddv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(fmaxnmv(h28, p1, z0.VnH()), "fmaxnmv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(fmaxnmv(s28, p1, z0.VnS()), "fmaxnmv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(fmaxnmv(d28, p1, z0.VnD()), "fmaxnmv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(fmaxv(h3, p1, z23.VnH()), "fmaxv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(fmaxv(s3, p1, z23.VnS()), "fmaxv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(fmaxv(d3, p1, z23.VnD()), "fmaxv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(fminnmv(h20, p6, z21.VnH()), "fminnmv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(fminnmv(s20, p6, z21.VnS()), "fminnmv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(fminnmv(d20, p6, z21.VnD()), "fminnmv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(fminv(h10, p4, z27.VnH()), "fminv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(fminv(s10, p4, z27.VnS()), "fminv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(fminv(d10, p4, z27.VnD()), "fminv <V><d>, <Pg>, <Zn>.<T>");
#endif

  CLEANUP();
}

TEST(sve_fp_mul_add) {
  SETUP();

#if 0
  COMPARE_PREFIX(fmad(z31.VnH(), p2.Merging(), z8.VnH(), za.VnH()), "fmad <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>");
  COMPARE_PREFIX(fmad(z31.VnS(), p2.Merging(), z8.VnS(), za.VnS()), "fmad <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>");
  COMPARE_PREFIX(fmad(z31.VnD(), p2.Merging(), z8.VnD(), za.VnD()), "fmad <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>");
  COMPARE_PREFIX(fmla(z26.VnH(), p7.Merging(), z19.VnH(), z16.VnH()), "fmla <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmla(z26.VnS(), p7.Merging(), z19.VnS(), z16.VnS()), "fmla <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmla(z26.VnD(), p7.Merging(), z19.VnD(), z16.VnD()), "fmla <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmls(z20.VnH(), p6.Merging(), z28.VnH(), z0.VnH()), "fmls <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmls(z20.VnS(), p6.Merging(), z28.VnS(), z0.VnS()), "fmls <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmls(z20.VnD(), p6.Merging(), z28.VnD(), z0.VnD()), "fmls <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fmsb(z3.VnH(), p4.Merging(), z8.VnH(), za.VnH()), "fmsb <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>");
  COMPARE_PREFIX(fmsb(z3.VnS(), p4.Merging(), z8.VnS(), za.VnS()), "fmsb <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>");
  COMPARE_PREFIX(fmsb(z3.VnD(), p4.Merging(), z8.VnD(), za.VnD()), "fmsb <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>");
  COMPARE_PREFIX(fnmad(z0.VnH(), p5.Merging(), z20.VnH(), za.VnH()), "fnmad <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>");
  COMPARE_PREFIX(fnmad(z0.VnS(), p5.Merging(), z20.VnS(), za.VnS()), "fnmad <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>");
  COMPARE_PREFIX(fnmad(z0.VnD(), p5.Merging(), z20.VnD(), za.VnD()), "fnmad <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>");
  COMPARE_PREFIX(fnmla(z31.VnH(), p6.Merging(), z14.VnH(), z8.VnH()), "fnmla <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fnmla(z31.VnS(), p6.Merging(), z14.VnS(), z8.VnS()), "fnmla <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fnmla(z31.VnD(), p6.Merging(), z14.VnD(), z8.VnD()), "fnmla <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fnmls(z2.VnH(), p1.Merging(), z23.VnH(), z15.VnH()), "fnmls <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fnmls(z2.VnS(), p1.Merging(), z23.VnS(), z15.VnS()), "fnmls <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fnmls(z2.VnD(), p1.Merging(), z23.VnD(), z15.VnD()), "fnmls <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(fnmsb(z28.VnH(), p3.Merging(), z26.VnH(), za.VnH()), "fnmsb <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>");
  COMPARE_PREFIX(fnmsb(z28.VnS(), p3.Merging(), z26.VnS(), za.VnS()), "fnmsb <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>");
  COMPARE_PREFIX(fnmsb(z28.VnD(), p3.Merging(), z26.VnD(), za.VnD()), "fnmsb <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>");
#endif

  CLEANUP();
}

TEST(sve_fp_mul_add_index) {
  SETUP();

#if 0
  COMPARE_PREFIX(fmla(z25.VnD(), z9.VnD()), "fmla <Zda>.D, <Zn>.D, <Zm>.D[<imm>]");
  COMPARE_PREFIX(fmla(z13.VnH(), z7.VnH()), "fmla <Zda>.H, <Zn>.H, <Zm>.H[<imm>]");
  COMPARE_PREFIX(fmla(z17.VnS(), z27.VnS()), "fmla <Zda>.S, <Zn>.S, <Zm>.S[<imm>]");
  COMPARE_PREFIX(fmls(z28.VnD(), z2.VnD()), "fmls <Zda>.D, <Zn>.D, <Zm>.D[<imm>]");
  COMPARE_PREFIX(fmls(z30.VnH(), z29.VnH()), "fmls <Zda>.H, <Zn>.H, <Zm>.H[<imm>]");
  COMPARE_PREFIX(fmls(z30.VnS(), z1.VnS()), "fmls <Zda>.S, <Zn>.S, <Zm>.S[<imm>]");
#endif

  CLEANUP();
}

TEST(sve_fp_mul_index) {
  SETUP();

#if 0
  COMPARE_PREFIX(fmul(z12.VnD(), z3.VnD()), "fmul <Zd>.D, <Zn>.D, <Zm>.D[<imm>]");
  COMPARE_PREFIX(fmul(z22.VnH(), z2.VnH()), "fmul <Zd>.H, <Zn>.H, <Zm>.H[<imm>]");
  COMPARE_PREFIX(fmul(z2.VnS(), z8.VnS()), "fmul <Zd>.S, <Zn>.S, <Zm>.S[<imm>]");
#endif

  CLEANUP();
}

TEST(sve_fp_unary_op_predicated) {
  SETUP();

#if 0
  COMPARE_PREFIX(fcvtzs(z29.VnS(), p5.Merging(), z8.VnD()), "fcvtzs <Zd>.S, <Pg>/M, <Zn>.D");
  COMPARE_PREFIX(fcvtzs(z30.VnD(), p5.Merging(), z8.VnD()), "fcvtzs <Zd>.D, <Pg>/M, <Zn>.D");
  COMPARE_PREFIX(fcvtzs(z14.VnH(), p1.Merging(), z29.VnH()), "fcvtzs <Zd>.H, <Pg>/M, <Zn>.H");
  COMPARE_PREFIX(fcvtzs(z11.VnS(), p3.Merging(), z16.VnH()), "fcvtzs <Zd>.S, <Pg>/M, <Zn>.H");
  COMPARE_PREFIX(fcvtzs(z4.VnD(), p7.Merging(), z4.VnH()), "fcvtzs <Zd>.D, <Pg>/M, <Zn>.H");
  COMPARE_PREFIX(fcvtzs(z24.VnS(), p1.Merging(), z4.VnS()), "fcvtzs <Zd>.S, <Pg>/M, <Zn>.S");
  COMPARE_PREFIX(fcvtzs(z25.VnD(), p4.Merging(), z24.VnS()), "fcvtzs <Zd>.D, <Pg>/M, <Zn>.S");
  COMPARE_PREFIX(fcvtzu(z16.VnS(), p7.Merging(), z14.VnD()), "fcvtzu <Zd>.S, <Pg>/M, <Zn>.D");
  COMPARE_PREFIX(fcvtzu(z31.VnD(), p1.Merging(), z16.VnD()), "fcvtzu <Zd>.D, <Pg>/M, <Zn>.D");
  COMPARE_PREFIX(fcvtzu(z12.VnH(), p2.Merging(), z27.VnH()), "fcvtzu <Zd>.H, <Pg>/M, <Zn>.H");
  COMPARE_PREFIX(fcvtzu(z26.VnS(), p6.Merging(), z29.VnH()), "fcvtzu <Zd>.S, <Pg>/M, <Zn>.H");
  COMPARE_PREFIX(fcvtzu(z29.VnD(), p5.Merging(), z27.VnH()), "fcvtzu <Zd>.D, <Pg>/M, <Zn>.H");
  COMPARE_PREFIX(fcvtzu(z13.VnS(), p2.Merging(), z17.VnS()), "fcvtzu <Zd>.S, <Pg>/M, <Zn>.S");
  COMPARE_PREFIX(fcvtzu(z25.VnD(), p7.Merging(), z28.VnS()), "fcvtzu <Zd>.D, <Pg>/M, <Zn>.S");
  COMPARE_PREFIX(fcvt(z5.VnH(), p2.Merging(), z11.VnD()), "fcvt <Zd>.H, <Pg>/M, <Zn>.D");
  COMPARE_PREFIX(fcvt(z30.VnS(), p7.Merging(), z0.VnD()), "fcvt <Zd>.S, <Pg>/M, <Zn>.D");
  COMPARE_PREFIX(fcvt(z10.VnD(), p0.Merging(), z17.VnH()), "fcvt <Zd>.D, <Pg>/M, <Zn>.H");
  COMPARE_PREFIX(fcvt(z28.VnS(), p3.Merging(), z27.VnH()), "fcvt <Zd>.S, <Pg>/M, <Zn>.H");
  COMPARE_PREFIX(fcvt(z9.VnD(), p7.Merging(), z0.VnS()), "fcvt <Zd>.D, <Pg>/M, <Zn>.S");
  COMPARE_PREFIX(fcvt(z27.VnH(), p7.Merging(), z9.VnS()), "fcvt <Zd>.H, <Pg>/M, <Zn>.S");
  COMPARE_PREFIX(frecpx(z16.VnH(), p1.Merging(), z29.VnH()), "frecpx <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frecpx(z16.VnS(), p1.Merging(), z29.VnS()), "frecpx <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frecpx(z16.VnD(), p1.Merging(), z29.VnD()), "frecpx <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frinta(z11.VnH(), p0.Merging(), z3.VnH()), "frinta <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frinta(z11.VnS(), p0.Merging(), z3.VnS()), "frinta <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frinta(z11.VnD(), p0.Merging(), z3.VnD()), "frinta <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frinti(z17.VnH(), p0.Merging(), z16.VnH()), "frinti <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frinti(z17.VnS(), p0.Merging(), z16.VnS()), "frinti <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frinti(z17.VnD(), p0.Merging(), z16.VnD()), "frinti <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintm(z2.VnH(), p7.Merging(), z15.VnH()), "frintm <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintm(z2.VnS(), p7.Merging(), z15.VnS()), "frintm <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintm(z2.VnD(), p7.Merging(), z15.VnD()), "frintm <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintn(z14.VnH(), p5.Merging(), z18.VnH()), "frintn <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintn(z14.VnS(), p5.Merging(), z18.VnS()), "frintn <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintn(z14.VnD(), p5.Merging(), z18.VnD()), "frintn <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintp(z20.VnH(), p6.Merging(), z23.VnH()), "frintp <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintp(z20.VnS(), p6.Merging(), z23.VnS()), "frintp <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintp(z20.VnD(), p6.Merging(), z23.VnD()), "frintp <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintx(z2.VnH(), p6.Merging(), z18.VnH()), "frintx <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintx(z2.VnS(), p6.Merging(), z18.VnS()), "frintx <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintx(z2.VnD(), p6.Merging(), z18.VnD()), "frintx <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintz(z26.VnH(), p7.Merging(), z25.VnH()), "frintz <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintz(z26.VnS(), p7.Merging(), z25.VnS()), "frintz <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(frintz(z26.VnD(), p7.Merging(), z25.VnD()), "frintz <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(fsqrt(z30.VnH(), p3.Merging(), z13.VnH()), "fsqrt <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(fsqrt(z30.VnS(), p3.Merging(), z13.VnS()), "fsqrt <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(fsqrt(z30.VnD(), p3.Merging(), z13.VnD()), "fsqrt <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(scvtf(z16.VnH(), p6.Merging(), z5.VnH()), "scvtf <Zd>.H, <Pg>/M, <Zn>.H");
  COMPARE_PREFIX(scvtf(z31.VnD(), p5.Merging(), z26.VnS()), "scvtf <Zd>.D, <Pg>/M, <Zn>.S");
  COMPARE_PREFIX(scvtf(z0.VnH(), p7.Merging(), z0.VnS()), "scvtf <Zd>.H, <Pg>/M, <Zn>.S");
  COMPARE_PREFIX(scvtf(z12.VnS(), p7.Merging(), z0.VnS()), "scvtf <Zd>.S, <Pg>/M, <Zn>.S");
  COMPARE_PREFIX(scvtf(z17.VnD(), p1.Merging(), z17.VnD()), "scvtf <Zd>.D, <Pg>/M, <Zn>.D");
  COMPARE_PREFIX(scvtf(z2.VnH(), p0.Merging(), z9.VnD()), "scvtf <Zd>.H, <Pg>/M, <Zn>.D");
  COMPARE_PREFIX(scvtf(z26.VnS(), p5.Merging(), z4.VnD()), "scvtf <Zd>.S, <Pg>/M, <Zn>.D");
  COMPARE_PREFIX(ucvtf(z27.VnH(), p4.Merging(), z25.VnH()), "ucvtf <Zd>.H, <Pg>/M, <Zn>.H");
  COMPARE_PREFIX(ucvtf(z3.VnD(), p4.Merging(), z3.VnS()), "ucvtf <Zd>.D, <Pg>/M, <Zn>.S");
  COMPARE_PREFIX(ucvtf(z24.VnH(), p2.Merging(), z29.VnS()), "ucvtf <Zd>.H, <Pg>/M, <Zn>.S");
  COMPARE_PREFIX(ucvtf(z29.VnS(), p5.Merging(), z14.VnS()), "ucvtf <Zd>.S, <Pg>/M, <Zn>.S");
  COMPARE_PREFIX(ucvtf(z7.VnD(), p2.Merging(), z14.VnD()), "ucvtf <Zd>.D, <Pg>/M, <Zn>.D");
  COMPARE_PREFIX(ucvtf(z20.VnH(), p2.Merging(), z14.VnD()), "ucvtf <Zd>.H, <Pg>/M, <Zn>.D");
  COMPARE_PREFIX(ucvtf(z26.VnS(), p1.Merging(), z18.VnD()), "ucvtf <Zd>.S, <Pg>/M, <Zn>.D");
#endif

  CLEANUP();
}

TEST(sve_fp_unary_op_unpredicated) {
  SETUP();

#if 0
  COMPARE_PREFIX(frecpe(z0.VnH(), z2.VnH()), "frecpe <Zd>.<T>, <Zn>.<T>");
  COMPARE_PREFIX(frecpe(z0.VnS(), z2.VnS()), "frecpe <Zd>.<T>, <Zn>.<T>");
  COMPARE_PREFIX(frecpe(z0.VnD(), z2.VnD()), "frecpe <Zd>.<T>, <Zn>.<T>");
  COMPARE_PREFIX(frsqrte(z27.VnH(), z14.VnH()), "frsqrte <Zd>.<T>, <Zn>.<T>");
  COMPARE_PREFIX(frsqrte(z27.VnS(), z14.VnS()), "frsqrte <Zd>.<T>, <Zn>.<T>");
  COMPARE_PREFIX(frsqrte(z27.VnD(), z14.VnD()), "frsqrte <Zd>.<T>, <Zn>.<T>");
#endif

  CLEANUP();
}

TEST(sve_inc_dec_by_predicate_count) {
  SETUP();

  COMPARE_PREFIX(decp(x17, p0.VnB()), "decp x17, p0.b");
  COMPARE_PREFIX(decp(x17, p0.VnH()), "decp x17, p0.h");
  COMPARE_PREFIX(decp(x17, p0.VnS()), "decp x17, p0.s");
  COMPARE_PREFIX(decp(x17, p0.VnD()), "decp x17, p0.d");
  COMPARE_PREFIX(decp(z2.VnH(), p11), "decp z2.h, p11");
  COMPARE_PREFIX(decp(z2.VnS(), p11), "decp z2.s, p11");
  COMPARE_PREFIX(decp(z2.VnD(), p11), "decp z2.d, p11");
  COMPARE_PREFIX(incp(x26, p8.VnB()), "incp x26, p8.b");
  COMPARE_PREFIX(incp(x26, p8.VnH()), "incp x26, p8.h");
  COMPARE_PREFIX(incp(x26, p8.VnS()), "incp x26, p8.s");
  COMPARE_PREFIX(incp(x26, p8.VnD()), "incp x26, p8.d");
  COMPARE_PREFIX(incp(z27.VnH(), p9), "incp z27.h, p9");
  COMPARE_PREFIX(incp(z27.VnS(), p9), "incp z27.s, p9");
  COMPARE_PREFIX(incp(z27.VnD(), p9), "incp z27.d, p9");
  COMPARE_PREFIX(sqdecp(x12, p7.VnB(), w12), "sqdecp x12, p7.b, w12");
  COMPARE_PREFIX(sqdecp(x12, p7.VnH(), w12), "sqdecp x12, p7.h, w12");
  COMPARE_PREFIX(sqdecp(x12, p7.VnS(), w12), "sqdecp x12, p7.s, w12");
  COMPARE_PREFIX(sqdecp(x12, p7.VnD(), w12), "sqdecp x12, p7.d, w12");
  COMPARE_PREFIX(sqdecp(x30, p5.VnB()), "sqdecp x30, p5.b");
  COMPARE_PREFIX(sqdecp(x30, p5.VnH()), "sqdecp x30, p5.h");
  COMPARE_PREFIX(sqdecp(x30, p5.VnS()), "sqdecp x30, p5.s");
  COMPARE_PREFIX(sqdecp(x30, p5.VnD()), "sqdecp x30, p5.d");
  COMPARE_PREFIX(sqdecp(z13.VnH(), p1), "sqdecp z13.h, p1");
  COMPARE_PREFIX(sqdecp(z13.VnS(), p1), "sqdecp z13.s, p1");
  COMPARE_PREFIX(sqdecp(z13.VnD(), p1), "sqdecp z13.d, p1");
  COMPARE_PREFIX(sqincp(x26, p5.VnB(), w26), "sqincp x26, p5.b, w26");
  COMPARE_PREFIX(sqincp(x26, p5.VnH(), w26), "sqincp x26, p5.h, w26");
  COMPARE_PREFIX(sqincp(x26, p5.VnS(), w26), "sqincp x26, p5.s, w26");
  COMPARE_PREFIX(sqincp(x26, p5.VnD(), w26), "sqincp x26, p5.d, w26");
  COMPARE_PREFIX(sqincp(x5, p15.VnB()), "sqincp x5, p15.b");
  COMPARE_PREFIX(sqincp(x5, p15.VnH()), "sqincp x5, p15.h");
  COMPARE_PREFIX(sqincp(x5, p15.VnS()), "sqincp x5, p15.s");
  COMPARE_PREFIX(sqincp(x5, p15.VnD()), "sqincp x5, p15.d");
  COMPARE_PREFIX(sqincp(z14.VnH(), p4), "sqincp z14.h, p4");
  COMPARE_PREFIX(sqincp(z14.VnS(), p4), "sqincp z14.s, p4");
  COMPARE_PREFIX(sqincp(z14.VnD(), p4), "sqincp z14.d, p4");
  COMPARE_PREFIX(uqdecp(w3, p13.VnB()), "uqdecp w3, p13.b");
  COMPARE_PREFIX(uqdecp(w3, p13.VnH()), "uqdecp w3, p13.h");
  COMPARE_PREFIX(uqdecp(w3, p13.VnS()), "uqdecp w3, p13.s");
  COMPARE_PREFIX(uqdecp(w3, p13.VnD()), "uqdecp w3, p13.d");
  COMPARE_PREFIX(uqdecp(x19, p0.VnB()), "uqdecp x19, p0.b");
  COMPARE_PREFIX(uqdecp(x19, p0.VnH()), "uqdecp x19, p0.h");
  COMPARE_PREFIX(uqdecp(x19, p0.VnS()), "uqdecp x19, p0.s");
  COMPARE_PREFIX(uqdecp(x19, p0.VnD()), "uqdecp x19, p0.d");
  COMPARE_PREFIX(uqdecp(z15.VnH(), p9), "uqdecp z15.h, p9");
  COMPARE_PREFIX(uqdecp(z15.VnS(), p9), "uqdecp z15.s, p9");
  COMPARE_PREFIX(uqdecp(z15.VnD(), p9), "uqdecp z15.d, p9");
  COMPARE_PREFIX(uqincp(w18, p1.VnB()), "uqincp w18, p1.b");
  COMPARE_PREFIX(uqincp(w18, p1.VnH()), "uqincp w18, p1.h");
  COMPARE_PREFIX(uqincp(w18, p1.VnS()), "uqincp w18, p1.s");
  COMPARE_PREFIX(uqincp(w18, p1.VnD()), "uqincp w18, p1.d");
  COMPARE_PREFIX(uqincp(x17, p15.VnB()), "uqincp x17, p15.b");
  COMPARE_PREFIX(uqincp(x17, p15.VnH()), "uqincp x17, p15.h");
  COMPARE_PREFIX(uqincp(x17, p15.VnS()), "uqincp x17, p15.s");
  COMPARE_PREFIX(uqincp(x17, p15.VnD()), "uqincp x17, p15.d");
  COMPARE_PREFIX(uqincp(z4.VnH(), p3), "uqincp z4.h, p3");
  COMPARE_PREFIX(uqincp(z4.VnS(), p3), "uqincp z4.s, p3");
  COMPARE_PREFIX(uqincp(z4.VnD(), p3), "uqincp z4.d, p3");

  // The governing predicate can have a lane size as long as it matches zdn.
  COMPARE_PREFIX(decp(z0.VnH(), p1.VnH()), "decp z0.h, p1");
  COMPARE_PREFIX(incp(z2.VnS(), p3.VnS()), "incp z2.s, p3");
  COMPARE_PREFIX(sqdecp(z4.VnD(), p5.VnD()), "sqdecp z4.d, p5");
  COMPARE_PREFIX(sqincp(z6.VnH(), p7.VnH()), "sqincp z6.h, p7");
  COMPARE_PREFIX(uqdecp(z8.VnS(), p9.VnS()), "uqdecp z8.s, p9");
  COMPARE_PREFIX(uqincp(z10.VnD(), p11.VnD()), "uqincp z10.d, p11");

  CLEANUP();
}

TEST(sve_inc_dec_by_predicate_count_macro) {
  SETUP();

  // The MacroAssembler automatically generates movprfx where it can.
  COMPARE_MACRO(Decp(z0.VnD(), p1), "decp z0.d, p1");
  COMPARE_MACRO(Decp(z2.VnS(), p3, z2.VnS()), "decp z2.s, p3");
  COMPARE_MACRO(Decp(z3.VnS(), p3, z3.VnS()), "decp z3.s, p3");
  COMPARE_MACRO(Decp(z4.VnH(), p5, z6.VnH()),
                "movprfx z4, z6\n"
                "decp z4.h, p5");
  COMPARE_MACRO(Incp(z7.VnD(), p8), "incp z7.d, p8");
  COMPARE_MACRO(Incp(z9.VnS(), p10, z9.VnS()), "incp z9.s, p10");
  COMPARE_MACRO(Incp(z10.VnS(), p10, z10.VnS()), "incp z10.s, p10");
  COMPARE_MACRO(Incp(z10.VnH(), p11, z12.VnH()),
                "movprfx z10, z12\n"
                "incp z10.h, p11");
  COMPARE_MACRO(Sqdecp(z0.VnD(), p1), "sqdecp z0.d, p1");
  COMPARE_MACRO(Sqdecp(z2.VnS(), p3, z2.VnS()), "sqdecp z2.s, p3");
  COMPARE_MACRO(Sqdecp(z3.VnS(), p3, z3.VnS()), "sqdecp z3.s, p3");
  COMPARE_MACRO(Sqdecp(z4.VnH(), p5, z6.VnH()),
                "movprfx z4, z6\n"
                "sqdecp z4.h, p5");
  COMPARE_MACRO(Sqincp(z7.VnD(), p8), "sqincp z7.d, p8");
  COMPARE_MACRO(Sqincp(z9.VnS(), p10, z9.VnS()), "sqincp z9.s, p10");
  COMPARE_MACRO(Sqincp(z10.VnS(), p10, z10.VnS()), "sqincp z10.s, p10");
  COMPARE_MACRO(Sqincp(z10.VnH(), p11, z12.VnH()),
                "movprfx z10, z12\n"
                "sqincp z10.h, p11");
  COMPARE_MACRO(Uqdecp(z0.VnD(), p1), "uqdecp z0.d, p1");
  COMPARE_MACRO(Uqdecp(z2.VnS(), p3, z2.VnS()), "uqdecp z2.s, p3");
  COMPARE_MACRO(Uqdecp(z3.VnS(), p3, z3.VnS()), "uqdecp z3.s, p3");
  COMPARE_MACRO(Uqdecp(z4.VnH(), p5, z6.VnH()),
                "movprfx z4, z6\n"
                "uqdecp z4.h, p5");
  COMPARE_MACRO(Uqincp(z7.VnD(), p8), "uqincp z7.d, p8");
  COMPARE_MACRO(Uqincp(z9.VnS(), p10, z9.VnS()), "uqincp z9.s, p10");
  COMPARE_MACRO(Uqincp(z10.VnS(), p10, z10.VnS()), "uqincp z10.s, p10");
  COMPARE_MACRO(Uqincp(z10.VnH(), p11, z12.VnH()),
                "movprfx z10, z12\n"
                "uqincp z10.h, p11");

  // Scalar sign- or zero-extending variants are selected by the size of `rn`.
  COMPARE_MACRO(Sqdecp(x0, p1.VnB(), w0), "sqdecp x0, p1.b, w0");
  COMPARE_MACRO(Sqdecp(x2, p3.VnH(), x2), "sqdecp x2, p3.h");
  COMPARE_MACRO(Sqdecp(x4, p5.VnS()), "sqdecp x4, p5.s");
  // Sqdecp cannot write into a W register, but Uqdecp can.
  COMPARE_MACRO(Uqdecp(w6, p7.VnD(), w6), "uqdecp w6, p7.d");
  COMPARE_MACRO(Uqdecp(x8, p9.VnB(), w8), "uqdecp w8, p9.b");
  COMPARE_MACRO(Uqdecp(x10, p11.VnH(), x10), "uqdecp x10, p11.h");
  COMPARE_MACRO(Uqdecp(x12, p13.VnS()), "uqdecp x12, p13.s");
  COMPARE_MACRO(Uqdecp(w14, p15.VnD()), "uqdecp w14, p15.d");

  // The governing predicate can have a lane size as long as it matches zdn.
  COMPARE_MACRO(Decp(z10.VnH(), p6.VnH()), "decp z10.h, p6");
  COMPARE_MACRO(Incp(z12.VnS(), p8.VnS()), "incp z12.s, p8");
  COMPARE_MACRO(Sqdecp(z14.VnD(), p10.VnD()), "sqdecp z14.d, p10");
  COMPARE_MACRO(Sqincp(z16.VnH(), p12.VnH()), "sqincp z16.h, p12");
  COMPARE_MACRO(Uqdecp(z18.VnS(), p14.VnS()), "uqdecp z18.s, p14");
  COMPARE_MACRO(Uqincp(z20.VnD(), p15.VnD()), "uqincp z20.d, p15");

  CLEANUP();
}

TEST(sve_index_generation) {
  SETUP();

  COMPARE_PREFIX(index(z21.VnB(), -16, 15), "index z21.b, #-16, #15");
  COMPARE_PREFIX(index(z22.VnB(), -2, 1), "index z22.b, #-2, #1");
  COMPARE_PREFIX(index(z23.VnH(), -1, 0), "index z23.h, #-1, #0");
  COMPARE_PREFIX(index(z24.VnS(), 0, -1), "index z24.s, #0, #-1");
  COMPARE_PREFIX(index(z25.VnD(), 1, -2), "index z25.d, #1, #-2");
  COMPARE_PREFIX(index(z26.VnB(), 15, -16), "index z26.b, #15, #-16");
  COMPARE_PREFIX(index(z23.VnB(), -16, w8), "index z23.b, #-16, w8");
  COMPARE_PREFIX(index(z24.VnH(), -1, x9), "index z24.h, #-1, w9");
  COMPARE_PREFIX(index(z25.VnS(), 0, w10), "index z25.s, #0, w10");
  COMPARE_PREFIX(index(z26.VnD(), 15, x11), "index z26.d, #15, x11");
  COMPARE_PREFIX(index(z14.VnB(), w15, 15), "index z14.b, w15, #15");
  COMPARE_PREFIX(index(z15.VnH(), x16, 1), "index z15.h, w16, #1");
  COMPARE_PREFIX(index(z16.VnS(), w17, 0), "index z16.s, w17, #0");
  COMPARE_PREFIX(index(z17.VnD(), x18, -16), "index z17.d, x18, #-16");
  COMPARE_PREFIX(index(z20.VnB(), w23, w21), "index z20.b, w23, w21");
  COMPARE_PREFIX(index(z21.VnH(), x24, w22), "index z21.h, w24, w22");
  COMPARE_PREFIX(index(z22.VnS(), w25, x23), "index z22.s, w25, w23");
  COMPARE_PREFIX(index(z23.VnD(), x26, x24), "index z23.d, x26, x24");

  // Simple pass-through macros.
  COMPARE_MACRO(Index(z21.VnB(), -16, 15), "index z21.b, #-16, #15");
  COMPARE_MACRO(Index(z22.VnB(), -2, 1), "index z22.b, #-2, #1");
  COMPARE_MACRO(Index(z23.VnH(), -1, 0), "index z23.h, #-1, #0");
  COMPARE_MACRO(Index(z24.VnS(), 0, -1), "index z24.s, #0, #-1");
  COMPARE_MACRO(Index(z25.VnD(), 1, -2), "index z25.d, #1, #-2");
  COMPARE_MACRO(Index(z26.VnB(), 15, -16), "index z26.b, #15, #-16");
  COMPARE_MACRO(Index(z23.VnB(), -16, w8), "index z23.b, #-16, w8");
  COMPARE_MACRO(Index(z24.VnH(), -1, x9), "index z24.h, #-1, w9");
  COMPARE_MACRO(Index(z25.VnS(), 0, w10), "index z25.s, #0, w10");
  COMPARE_MACRO(Index(z26.VnD(), 15, x11), "index z26.d, #15, x11");
  COMPARE_MACRO(Index(z14.VnB(), w15, 15), "index z14.b, w15, #15");
  COMPARE_MACRO(Index(z15.VnH(), x16, 1), "index z15.h, w16, #1");
  COMPARE_MACRO(Index(z16.VnS(), w17, 0), "index z16.s, w17, #0");
  COMPARE_MACRO(Index(z17.VnD(), x18, -16), "index z17.d, x18, #-16");
  COMPARE_MACRO(Index(z20.VnB(), w23, w21), "index z20.b, w23, w21");
  COMPARE_MACRO(Index(z21.VnH(), x24, w22), "index z21.h, w24, w22");
  COMPARE_MACRO(Index(z22.VnS(), w25, x23), "index z22.s, w25, w23");
  COMPARE_MACRO(Index(z23.VnD(), x26, x24), "index z23.d, x26, x24");

  // Argument synthesis.
  COMPARE_MACRO(Index(z0.VnB(), 16, -17),
                "mov w16, #0x10\n"
                "mov w17, #0xffffffef\n"
                "index z0.b, w16, w17");
  COMPARE_MACRO(Index(z1.VnH(), x2, -17),
                "mov w16, #0xffffffef\n"
                "index z1.h, w2, w16");
  COMPARE_MACRO(Index(z3.VnS(), 16, w4),
                "mov w16, #0x10\n"
                "index z3.s, w16, w4");
  COMPARE_MACRO(Index(z4.VnD(), -17, 16),
                "mov x16, #0xffffffffffffffef\n"
                "mov x17, #0x10\n"
                "index z4.d, x16, x17");

  CLEANUP();
}

TEST(sve_int_arithmetic_unpredicated) {
  SETUP();

  COMPARE_PREFIX(add(z23.VnB(), z30.VnB(), z31.VnB()),
                 "add z23.b, z30.b, z31.b");
  COMPARE_PREFIX(add(z24.VnH(), z29.VnH(), z30.VnH()),
                 "add z24.h, z29.h, z30.h");
  COMPARE_PREFIX(add(z25.VnS(), z28.VnS(), z29.VnS()),
                 "add z25.s, z28.s, z29.s");
  COMPARE_PREFIX(add(z26.VnD(), z27.VnD(), z28.VnD()),
                 "add z26.d, z27.d, z28.d");
  COMPARE_PREFIX(sqadd(z26.VnB(), z21.VnB(), z1.VnB()),
                 "sqadd z26.b, z21.b, z1.b");
  COMPARE_PREFIX(sqadd(z25.VnH(), z20.VnH(), z2.VnH()),
                 "sqadd z25.h, z20.h, z2.h");
  COMPARE_PREFIX(sqadd(z24.VnS(), z19.VnS(), z3.VnS()),
                 "sqadd z24.s, z19.s, z3.s");
  COMPARE_PREFIX(sqadd(z23.VnD(), z18.VnD(), z4.VnD()),
                 "sqadd z23.d, z18.d, z4.d");
  COMPARE_PREFIX(sqsub(z1.VnB(), z10.VnB(), z0.VnB()),
                 "sqsub z1.b, z10.b, z0.b");
  COMPARE_PREFIX(sqsub(z2.VnH(), z11.VnH(), z1.VnH()),
                 "sqsub z2.h, z11.h, z1.h");
  COMPARE_PREFIX(sqsub(z3.VnS(), z12.VnS(), z2.VnS()),
                 "sqsub z3.s, z12.s, z2.s");
  COMPARE_PREFIX(sqsub(z4.VnD(), z13.VnD(), z3.VnD()),
                 "sqsub z4.d, z13.d, z3.d");
  COMPARE_PREFIX(sub(z9.VnB(), z7.VnB(), z25.VnB()), "sub z9.b, z7.b, z25.b");
  COMPARE_PREFIX(sub(z8.VnH(), z8.VnH(), z26.VnH()), "sub z8.h, z8.h, z26.h");
  COMPARE_PREFIX(sub(z7.VnS(), z9.VnS(), z27.VnS()), "sub z7.s, z9.s, z27.s");
  COMPARE_PREFIX(sub(z6.VnD(), z10.VnD(), z28.VnD()), "sub z6.d, z10.d, z28.d");
  COMPARE_PREFIX(uqadd(z13.VnB(), z15.VnB(), z3.VnB()),
                 "uqadd z13.b, z15.b, z3.b");
  COMPARE_PREFIX(uqadd(z12.VnH(), z16.VnH(), z2.VnH()),
                 "uqadd z12.h, z16.h, z2.h");
  COMPARE_PREFIX(uqadd(z11.VnS(), z17.VnS(), z1.VnS()),
                 "uqadd z11.s, z17.s, z1.s");
  COMPARE_PREFIX(uqadd(z10.VnD(), z18.VnD(), z0.VnD()),
                 "uqadd z10.d, z18.d, z0.d");
  COMPARE_PREFIX(uqsub(z9.VnB(), z13.VnB(), z13.VnB()),
                 "uqsub z9.b, z13.b, z13.b");
  COMPARE_PREFIX(uqsub(z11.VnH(), z15.VnH(), z11.VnH()),
                 "uqsub z11.h, z15.h, z11.h");
  COMPARE_PREFIX(uqsub(z13.VnS(), z17.VnS(), z13.VnS()),
                 "uqsub z13.s, z17.s, z13.s");
  COMPARE_PREFIX(uqsub(z15.VnD(), z19.VnD(), z15.VnD()),
                 "uqsub z15.d, z19.d, z15.d");

  CLEANUP();
}

TEST(sve_int_binary_arithmetic_predicated) {
  SETUP();

  COMPARE_PREFIX(add(z22.VnB(), p4.Merging(), z22.VnB(), z20.VnB()),
                 "add z22.b, p4/m, z22.b, z20.b");
  COMPARE_PREFIX(add(z22.VnH(), p4.Merging(), z22.VnH(), z20.VnH()),
                 "add z22.h, p4/m, z22.h, z20.h");
  COMPARE_PREFIX(add(z22.VnS(), p4.Merging(), z22.VnS(), z20.VnS()),
                 "add z22.s, p4/m, z22.s, z20.s");
  COMPARE_PREFIX(add(z22.VnD(), p4.Merging(), z22.VnD(), z20.VnD()),
                 "add z22.d, p4/m, z22.d, z20.d");
  COMPARE_PREFIX(and_(z22.VnB(), p3.Merging(), z22.VnB(), z3.VnB()),
                 "and z22.b, p3/m, z22.b, z3.b");
  COMPARE_PREFIX(and_(z22.VnH(), p3.Merging(), z22.VnH(), z3.VnH()),
                 "and z22.h, p3/m, z22.h, z3.h");
  COMPARE_PREFIX(and_(z22.VnS(), p3.Merging(), z22.VnS(), z3.VnS()),
                 "and z22.s, p3/m, z22.s, z3.s");
  COMPARE_PREFIX(and_(z22.VnD(), p3.Merging(), z22.VnD(), z3.VnD()),
                 "and z22.d, p3/m, z22.d, z3.d");
  COMPARE_PREFIX(bic(z17.VnB(), p7.Merging(), z17.VnB(), z10.VnB()),
                 "bic z17.b, p7/m, z17.b, z10.b");
  COMPARE_PREFIX(bic(z17.VnH(), p7.Merging(), z17.VnH(), z10.VnH()),
                 "bic z17.h, p7/m, z17.h, z10.h");
  COMPARE_PREFIX(bic(z17.VnS(), p7.Merging(), z17.VnS(), z10.VnS()),
                 "bic z17.s, p7/m, z17.s, z10.s");
  COMPARE_PREFIX(bic(z17.VnD(), p7.Merging(), z17.VnD(), z10.VnD()),
                 "bic z17.d, p7/m, z17.d, z10.d");
  COMPARE_PREFIX(eor(z23.VnB(), p4.Merging(), z23.VnB(), z15.VnB()),
                 "eor z23.b, p4/m, z23.b, z15.b");
  COMPARE_PREFIX(eor(z23.VnH(), p4.Merging(), z23.VnH(), z15.VnH()),
                 "eor z23.h, p4/m, z23.h, z15.h");
  COMPARE_PREFIX(eor(z23.VnS(), p4.Merging(), z23.VnS(), z15.VnS()),
                 "eor z23.s, p4/m, z23.s, z15.s");
  COMPARE_PREFIX(eor(z23.VnD(), p4.Merging(), z23.VnD(), z15.VnD()),
                 "eor z23.d, p4/m, z23.d, z15.d");
  COMPARE_PREFIX(mul(z15.VnB(), p5.Merging(), z15.VnB(), z15.VnB()),
                 "mul z15.b, p5/m, z15.b, z15.b");
  COMPARE_PREFIX(mul(z15.VnH(), p5.Merging(), z15.VnH(), z15.VnH()),
                 "mul z15.h, p5/m, z15.h, z15.h");
  COMPARE_PREFIX(mul(z15.VnS(), p5.Merging(), z15.VnS(), z15.VnS()),
                 "mul z15.s, p5/m, z15.s, z15.s");
  COMPARE_PREFIX(mul(z15.VnD(), p5.Merging(), z15.VnD(), z15.VnD()),
                 "mul z15.d, p5/m, z15.d, z15.d");
  COMPARE_PREFIX(orr(z9.VnB(), p1.Merging(), z9.VnB(), z28.VnB()),
                 "orr z9.b, p1/m, z9.b, z28.b");
  COMPARE_PREFIX(orr(z9.VnH(), p1.Merging(), z9.VnH(), z28.VnH()),
                 "orr z9.h, p1/m, z9.h, z28.h");
  COMPARE_PREFIX(orr(z9.VnS(), p1.Merging(), z9.VnS(), z28.VnS()),
                 "orr z9.s, p1/m, z9.s, z28.s");
  COMPARE_PREFIX(orr(z9.VnD(), p1.Merging(), z9.VnD(), z28.VnD()),
                 "orr z9.d, p1/m, z9.d, z28.d");
  COMPARE_PREFIX(sabd(z11.VnB(), p6.Merging(), z11.VnB(), z31.VnB()),
                 "sabd z11.b, p6/m, z11.b, z31.b");
  COMPARE_PREFIX(sabd(z11.VnH(), p6.Merging(), z11.VnH(), z31.VnH()),
                 "sabd z11.h, p6/m, z11.h, z31.h");
  COMPARE_PREFIX(sabd(z11.VnS(), p6.Merging(), z11.VnS(), z31.VnS()),
                 "sabd z11.s, p6/m, z11.s, z31.s");
  COMPARE_PREFIX(sabd(z11.VnD(), p6.Merging(), z11.VnD(), z31.VnD()),
                 "sabd z11.d, p6/m, z11.d, z31.d");
  COMPARE_PREFIX(sdivr(z20.VnS(), p5.Merging(), z20.VnS(), z23.VnS()),
                 "sdivr z20.s, p5/m, z20.s, z23.s");
  COMPARE_PREFIX(sdiv(z15.VnD(), p6.Merging(), z15.VnD(), z8.VnD()),
                 "sdiv z15.d, p6/m, z15.d, z8.d");
  COMPARE_PREFIX(smax(z30.VnB(), p4.Merging(), z30.VnB(), z30.VnB()),
                 "smax z30.b, p4/m, z30.b, z30.b");
  COMPARE_PREFIX(smax(z30.VnH(), p4.Merging(), z30.VnH(), z30.VnH()),
                 "smax z30.h, p4/m, z30.h, z30.h");
  COMPARE_PREFIX(smax(z30.VnS(), p4.Merging(), z30.VnS(), z30.VnS()),
                 "smax z30.s, p4/m, z30.s, z30.s");
  COMPARE_PREFIX(smax(z30.VnD(), p4.Merging(), z30.VnD(), z30.VnD()),
                 "smax z30.d, p4/m, z30.d, z30.d");
  COMPARE_PREFIX(smin(z20.VnB(), p7.Merging(), z20.VnB(), z19.VnB()),
                 "smin z20.b, p7/m, z20.b, z19.b");
  COMPARE_PREFIX(smin(z20.VnH(), p7.Merging(), z20.VnH(), z19.VnH()),
                 "smin z20.h, p7/m, z20.h, z19.h");
  COMPARE_PREFIX(smin(z20.VnS(), p7.Merging(), z20.VnS(), z19.VnS()),
                 "smin z20.s, p7/m, z20.s, z19.s");
  COMPARE_PREFIX(smin(z20.VnD(), p7.Merging(), z20.VnD(), z19.VnD()),
                 "smin z20.d, p7/m, z20.d, z19.d");
  COMPARE_PREFIX(smulh(z23.VnB(), p0.Merging(), z23.VnB(), z3.VnB()),
                 "smulh z23.b, p0/m, z23.b, z3.b");
  COMPARE_PREFIX(smulh(z23.VnH(), p0.Merging(), z23.VnH(), z3.VnH()),
                 "smulh z23.h, p0/m, z23.h, z3.h");
  COMPARE_PREFIX(smulh(z23.VnS(), p0.Merging(), z23.VnS(), z3.VnS()),
                 "smulh z23.s, p0/m, z23.s, z3.s");
  COMPARE_PREFIX(smulh(z23.VnD(), p0.Merging(), z23.VnD(), z3.VnD()),
                 "smulh z23.d, p0/m, z23.d, z3.d");
  COMPARE_PREFIX(subr(z1.VnB(), p6.Merging(), z1.VnB(), z1.VnB()),
                 "subr z1.b, p6/m, z1.b, z1.b");
  COMPARE_PREFIX(subr(z1.VnH(), p6.Merging(), z1.VnH(), z1.VnH()),
                 "subr z1.h, p6/m, z1.h, z1.h");
  COMPARE_PREFIX(subr(z1.VnS(), p6.Merging(), z1.VnS(), z1.VnS()),
                 "subr z1.s, p6/m, z1.s, z1.s");
  COMPARE_PREFIX(subr(z1.VnD(), p6.Merging(), z1.VnD(), z1.VnD()),
                 "subr z1.d, p6/m, z1.d, z1.d");
  COMPARE_PREFIX(sub(z28.VnB(), p2.Merging(), z28.VnB(), z0.VnB()),
                 "sub z28.b, p2/m, z28.b, z0.b");
  COMPARE_PREFIX(sub(z28.VnH(), p2.Merging(), z28.VnH(), z0.VnH()),
                 "sub z28.h, p2/m, z28.h, z0.h");
  COMPARE_PREFIX(sub(z28.VnS(), p2.Merging(), z28.VnS(), z0.VnS()),
                 "sub z28.s, p2/m, z28.s, z0.s");
  COMPARE_PREFIX(sub(z28.VnD(), p2.Merging(), z28.VnD(), z0.VnD()),
                 "sub z28.d, p2/m, z28.d, z0.d");
  COMPARE_PREFIX(uabd(z14.VnB(), p6.Merging(), z14.VnB(), z22.VnB()),
                 "uabd z14.b, p6/m, z14.b, z22.b");
  COMPARE_PREFIX(uabd(z14.VnH(), p6.Merging(), z14.VnH(), z22.VnH()),
                 "uabd z14.h, p6/m, z14.h, z22.h");
  COMPARE_PREFIX(uabd(z14.VnS(), p6.Merging(), z14.VnS(), z22.VnS()),
                 "uabd z14.s, p6/m, z14.s, z22.s");
  COMPARE_PREFIX(uabd(z14.VnD(), p6.Merging(), z14.VnD(), z22.VnD()),
                 "uabd z14.d, p6/m, z14.d, z22.d");
  COMPARE_PREFIX(udivr(z27.VnS(), p5.Merging(), z27.VnS(), z31.VnS()),
                 "udivr z27.s, p5/m, z27.s, z31.s");
  COMPARE_PREFIX(udiv(z13.VnD(), p4.Merging(), z13.VnD(), z11.VnD()),
                 "udiv z13.d, p4/m, z13.d, z11.d");
  COMPARE_PREFIX(umax(z0.VnB(), p5.Merging(), z0.VnB(), z14.VnB()),
                 "umax z0.b, p5/m, z0.b, z14.b");
  COMPARE_PREFIX(umax(z0.VnH(), p5.Merging(), z0.VnH(), z14.VnH()),
                 "umax z0.h, p5/m, z0.h, z14.h");
  COMPARE_PREFIX(umax(z0.VnS(), p5.Merging(), z0.VnS(), z14.VnS()),
                 "umax z0.s, p5/m, z0.s, z14.s");
  COMPARE_PREFIX(umax(z0.VnD(), p5.Merging(), z0.VnD(), z14.VnD()),
                 "umax z0.d, p5/m, z0.d, z14.d");
  COMPARE_PREFIX(umin(z26.VnB(), p5.Merging(), z26.VnB(), z12.VnB()),
                 "umin z26.b, p5/m, z26.b, z12.b");
  COMPARE_PREFIX(umin(z26.VnH(), p5.Merging(), z26.VnH(), z12.VnH()),
                 "umin z26.h, p5/m, z26.h, z12.h");
  COMPARE_PREFIX(umin(z26.VnS(), p5.Merging(), z26.VnS(), z12.VnS()),
                 "umin z26.s, p5/m, z26.s, z12.s");
  COMPARE_PREFIX(umin(z26.VnD(), p5.Merging(), z26.VnD(), z12.VnD()),
                 "umin z26.d, p5/m, z26.d, z12.d");
  COMPARE_PREFIX(umulh(z12.VnB(), p2.Merging(), z12.VnB(), z17.VnB()),
                 "umulh z12.b, p2/m, z12.b, z17.b");
  COMPARE_PREFIX(umulh(z12.VnH(), p2.Merging(), z12.VnH(), z17.VnH()),
                 "umulh z12.h, p2/m, z12.h, z17.h");
  COMPARE_PREFIX(umulh(z12.VnS(), p2.Merging(), z12.VnS(), z17.VnS()),
                 "umulh z12.s, p2/m, z12.s, z17.s");
  COMPARE_PREFIX(umulh(z12.VnD(), p2.Merging(), z12.VnD(), z17.VnD()),
                 "umulh z12.d, p2/m, z12.d, z17.d");
  CLEANUP();
}

TEST(sve_int_binary_arithmetic_predicated_macro) {
  SETUP();

  COMPARE_MACRO(Add(z22.VnB(), p4.Merging(), z22.VnB(), z20.VnB()),
                "add z22.b, p4/m, z22.b, z20.b");
  COMPARE_MACRO(Add(z22.VnH(), p4.Merging(), z20.VnH(), z22.VnH()),
                "add z22.h, p4/m, z22.h, z20.h");
  COMPARE_MACRO(Add(z22.VnS(), p4.Merging(), z21.VnS(), z20.VnS()),
                "movprfx z22.s, p4/m, z21.s\n"
                "add z22.s, p4/m, z22.s, z20.s");

  COMPARE_MACRO(And(z22.VnH(), p3.Merging(), z22.VnH(), z3.VnH()),
                "and z22.h, p3/m, z22.h, z3.h");
  COMPARE_MACRO(And(z22.VnS(), p3.Merging(), z3.VnS(), z22.VnS()),
                "and z22.s, p3/m, z22.s, z3.s");
  COMPARE_MACRO(And(z22.VnD(), p3.Merging(), z2.VnD(), z3.VnD()),
                "movprfx z22.d, p3/m, z2.d\n"
                "and z22.d, p3/m, z22.d, z3.d");

  COMPARE_MACRO(Bic(z17.VnB(), p7.Merging(), z17.VnB(), z10.VnB()),
                "bic z17.b, p7/m, z17.b, z10.b");
  COMPARE_MACRO(Bic(z17.VnS(), p7.Merging(), z10.VnS(), z17.VnS()),
                "bic z17.s, p7/m, z17.s, z10.s");
  COMPARE_MACRO(Bic(z17.VnD(), p7.Merging(), z7.VnD(), z27.VnD()),
                "movprfx z17.d, p7/m, z7.d\n"
                "bic z17.d, p7/m, z17.d, z27.d");

  COMPARE_MACRO(Eor(z23.VnB(), p4.Merging(), z23.VnB(), z15.VnB()),
                "eor z23.b, p4/m, z23.b, z15.b");
  COMPARE_MACRO(Eor(z23.VnH(), p4.Merging(), z23.VnH(), z15.VnH()),
                "eor z23.h, p4/m, z23.h, z15.h");
  COMPARE_MACRO(Eor(z23.VnD(), p4.Merging(), z18.VnD(), z15.VnD()),
                "movprfx z23.d, p4/m, z18.d\n"
                "eor z23.d, p4/m, z23.d, z15.d");

  COMPARE_MACRO(Mul(z15.VnB(), p5.Merging(), z15.VnB(), z15.VnB()),
                "mul z15.b, p5/m, z15.b, z15.b");
  COMPARE_MACRO(Mul(z15.VnH(), p5.Merging(), z7.VnH(), z15.VnH()),
                "mul z15.h, p5/m, z15.h, z7.h");
  COMPARE_MACRO(Mul(z15.VnS(), p5.Merging(), z0.VnS(), z1.VnS()),
                "movprfx z15.s, p5/m, z0.s\n"
                "mul z15.s, p5/m, z15.s, z1.s");

  COMPARE_MACRO(Orr(z9.VnH(), p1.Merging(), z9.VnH(), z28.VnH()),
                "orr z9.h, p1/m, z9.h, z28.h");
  COMPARE_MACRO(Orr(z9.VnS(), p1.Merging(), z9.VnS(), z28.VnS()),
                "orr z9.s, p1/m, z9.s, z28.s");
  COMPARE_MACRO(Orr(z9.VnD(), p1.Merging(), z6.VnD(), z7.VnD()),
                "movprfx z9.d, p1/m, z6.d\n"
                "orr z9.d, p1/m, z9.d, z7.d");

  COMPARE_MACRO(Sabd(z11.VnB(), p6.Merging(), z11.VnB(), z31.VnB()),
                "sabd z11.b, p6/m, z11.b, z31.b");
  COMPARE_MACRO(Sabd(z11.VnH(), p6.Merging(), z31.VnH(), z11.VnH()),
                "sabd z11.h, p6/m, z11.h, z31.h");
  COMPARE_MACRO(Sabd(z11.VnS(), p6.Merging(), z21.VnS(), z31.VnS()),
                "movprfx z11.s, p6/m, z21.s\n"
                "sabd z11.s, p6/m, z11.s, z31.s");

  COMPARE_MACRO(Sdiv(z20.VnS(), p5.Merging(), z23.VnS(), z20.VnS()),
                "sdivr z20.s, p5/m, z20.s, z23.s");
  COMPARE_MACRO(Sdiv(z15.VnD(), p6.Merging(), z30.VnD(), z8.VnD()),
                "movprfx z15.d, p6/m, z30.d\n"
                "sdiv z15.d, p6/m, z15.d, z8.d");

  COMPARE_MACRO(Smax(z30.VnB(), p4.Merging(), z30.VnB(), z31.VnB()),
                "smax z30.b, p4/m, z30.b, z31.b");
  COMPARE_MACRO(Smax(z30.VnS(), p4.Merging(), z3.VnS(), z30.VnS()),
                "smax z30.s, p4/m, z30.s, z3.s");
  COMPARE_MACRO(Smax(z30.VnD(), p4.Merging(), z1.VnD(), z5.VnD()),
                "movprfx z30.d, p4/m, z1.d\n"
                "smax z30.d, p4/m, z30.d, z5.d");

  COMPARE_MACRO(Smin(z20.VnH(), p7.Merging(), z20.VnH(), z19.VnH()),
                "smin z20.h, p7/m, z20.h, z19.h");
  COMPARE_MACRO(Smin(z20.VnS(), p7.Merging(), z19.VnS(), z20.VnS()),
                "smin z20.s, p7/m, z20.s, z19.s");
  COMPARE_MACRO(Smin(z20.VnD(), p7.Merging(), z14.VnD(), z15.VnD()),
                "movprfx z20.d, p7/m, z14.d\n"
                "smin z20.d, p7/m, z20.d, z15.d");

  COMPARE_MACRO(Smulh(z23.VnB(), p0.Merging(), z23.VnB(), z3.VnB()),
                "smulh z23.b, p0/m, z23.b, z3.b");
  COMPARE_MACRO(Smulh(z23.VnH(), p0.Merging(), z13.VnH(), z23.VnH()),
                "smulh z23.h, p0/m, z23.h, z13.h");
  COMPARE_MACRO(Smulh(z23.VnD(), p0.Merging(), z30.VnD(), z31.VnD()),
                "movprfx z23.d, p0/m, z30.d\n"
                "smulh z23.d, p0/m, z23.d, z31.d");

  COMPARE_MACRO(Sub(z28.VnB(), p2.Merging(), z28.VnB(), z0.VnB()),
                "sub z28.b, p2/m, z28.b, z0.b");
  COMPARE_MACRO(Sub(z28.VnH(), p2.Merging(), z14.VnH(), z28.VnH()),
                "subr z28.h, p2/m, z28.h, z14.h");
  COMPARE_MACRO(Sub(z28.VnS(), p2.Merging(), z7.VnS(), z13.VnS()),
                "movprfx z28.s, p2/m, z7.s\n"
                "sub z28.s, p2/m, z28.s, z13.s");

  COMPARE_MACRO(Uabd(z14.VnH(), p6.Merging(), z14.VnH(), z22.VnH()),
                "uabd z14.h, p6/m, z14.h, z22.h");
  COMPARE_MACRO(Uabd(z14.VnS(), p6.Merging(), z14.VnS(), z22.VnS()),
                "uabd z14.s, p6/m, z14.s, z22.s");
  COMPARE_MACRO(Uabd(z14.VnD(), p6.Merging(), z13.VnD(), z22.VnD()),
                "movprfx z14.d, p6/m, z13.d\n"
                "uabd z14.d, p6/m, z14.d, z22.d");

  COMPARE_MACRO(Udiv(z27.VnS(), p5.Merging(), z16.VnS(), z27.VnS()),
                "udivr z27.s, p5/m, z27.s, z16.s");
  COMPARE_MACRO(Udiv(z13.VnD(), p4.Merging(), z22.VnD(), z11.VnD()),
                "movprfx z13.d, p4/m, z22.d\n"
                "udiv z13.d, p4/m, z13.d, z11.d");

  COMPARE_MACRO(Umax(z0.VnB(), p5.Merging(), z0.VnB(), z14.VnB()),
                "umax z0.b, p5/m, z0.b, z14.b");
  COMPARE_MACRO(Umax(z0.VnS(), p5.Merging(), z14.VnS(), z0.VnS()),
                "umax z0.s, p5/m, z0.s, z14.s");
  COMPARE_MACRO(Umax(z0.VnD(), p5.Merging(), z29.VnD(), z14.VnD()),
                "movprfx z0.d, p5/m, z29.d\n"
                "umax z0.d, p5/m, z0.d, z14.d");

  COMPARE_MACRO(Umin(z26.VnB(), p5.Merging(), z26.VnB(), z12.VnB()),
                "umin z26.b, p5/m, z26.b, z12.b");
  COMPARE_MACRO(Umin(z26.VnH(), p5.Merging(), z27.VnH(), z26.VnH()),
                "umin z26.h, p5/m, z26.h, z27.h");
  COMPARE_MACRO(Umin(z26.VnD(), p5.Merging(), z13.VnD(), z12.VnD()),
                "movprfx z26.d, p5/m, z13.d\n"
                "umin z26.d, p5/m, z26.d, z12.d");

  COMPARE_MACRO(Umulh(z12.VnB(), p2.Merging(), z12.VnB(), z17.VnB()),
                "umulh z12.b, p2/m, z12.b, z17.b");
  COMPARE_MACRO(Umulh(z12.VnH(), p2.Merging(), z0.VnH(), z12.VnH()),
                "umulh z12.h, p2/m, z12.h, z0.h");
  COMPARE_MACRO(Umulh(z12.VnS(), p2.Merging(), z25.VnS(), z17.VnS()),
                "movprfx z12.s, p2/m, z25.s\n"
                "umulh z12.s, p2/m, z12.s, z17.s");

  CLEANUP();
}

TEST(sve_int_compare_scalars) {
  SETUP();

  COMPARE_PREFIX(ctermeq(w30, w26), "ctermeq w30, w26");
  COMPARE_PREFIX(ctermne(x21, x18), "ctermne x21, x18");
  COMPARE_PREFIX(whilele(p10.VnB(), x11, x6), "whilele p10.b, x11, x6");
  COMPARE_PREFIX(whilele(p10.VnH(), w11, w6), "whilele p10.h, w11, w6");
  COMPARE_PREFIX(whilele(p10.VnH(), x11, x6), "whilele p10.h, x11, x6");
  COMPARE_PREFIX(whilele(p10.VnS(), w11, w6), "whilele p10.s, w11, w6");
  COMPARE_PREFIX(whilele(p10.VnD(), x11, x6), "whilele p10.d, x11, x6");
  COMPARE_PREFIX(whilelo(p4.VnB(), w3, w25), "whilelo p4.b, w3, w25");
  COMPARE_PREFIX(whilelo(p4.VnH(), x3, x25), "whilelo p4.h, x3, x25");
  COMPARE_PREFIX(whilelo(p4.VnS(), w3, w25), "whilelo p4.s, w3, w25");
  COMPARE_PREFIX(whilelo(p4.VnD(), x3, x25), "whilelo p4.d, x3, x25");
  COMPARE_PREFIX(whilels(p7.VnB(), w15, w15), "whilels p7.b, w15, w15");
  COMPARE_PREFIX(whilels(p7.VnH(), x15, x15), "whilels p7.h, x15, x15");
  COMPARE_PREFIX(whilels(p7.VnS(), w15, w15), "whilels p7.s, w15, w15");
  COMPARE_PREFIX(whilels(p7.VnD(), x15, x15), "whilels p7.d, x15, x15");
  COMPARE_PREFIX(whilelt(p14.VnB(), w11, w14), "whilelt p14.b, w11, w14");
  COMPARE_PREFIX(whilelt(p14.VnH(), x11, x14), "whilelt p14.h, x11, x14");
  COMPARE_PREFIX(whilelt(p14.VnS(), w11, w14), "whilelt p14.s, w11, w14");
  COMPARE_PREFIX(whilelt(p14.VnD(), x11, x14), "whilelt p14.d, x11, x14");

  CLEANUP();
}

TEST(sve_int_compare_signed_imm) {
  SETUP();

  COMPARE_PREFIX(cmpeq(p0.VnB(), p3.Zeroing(), z1.VnB(), 15),
                 "cmpeq p0.b, p3/z, z1.b, #15");
  COMPARE_PREFIX(cmpeq(p0.VnH(), p3.Zeroing(), z1.VnH(), 7),
                 "cmpeq p0.h, p3/z, z1.h, #7");
  COMPARE_PREFIX(cmpeq(p0.VnS(), p3.Zeroing(), z1.VnS(), -3),
                 "cmpeq p0.s, p3/z, z1.s, #-3");
  COMPARE_PREFIX(cmpeq(p0.VnD(), p3.Zeroing(), z1.VnD(), -14),
                 "cmpeq p0.d, p3/z, z1.d, #-14");
  COMPARE_PREFIX(cmpge(p9.VnB(), p6.Zeroing(), z12.VnB(), 14),
                 "cmpge p9.b, p6/z, z12.b, #14");
  COMPARE_PREFIX(cmpge(p9.VnH(), p6.Zeroing(), z12.VnH(), 6),
                 "cmpge p9.h, p6/z, z12.h, #6");
  COMPARE_PREFIX(cmpge(p9.VnS(), p6.Zeroing(), z12.VnS(), -4),
                 "cmpge p9.s, p6/z, z12.s, #-4");
  COMPARE_PREFIX(cmpge(p9.VnD(), p6.Zeroing(), z12.VnD(), -13),
                 "cmpge p9.d, p6/z, z12.d, #-13");
  COMPARE_PREFIX(cmpgt(p15.VnB(), p4.Zeroing(), z23.VnB(), 13),
                 "cmpgt p15.b, p4/z, z23.b, #13");
  COMPARE_PREFIX(cmpgt(p15.VnH(), p4.Zeroing(), z23.VnH(), 5),
                 "cmpgt p15.h, p4/z, z23.h, #5");
  COMPARE_PREFIX(cmpgt(p15.VnS(), p4.Zeroing(), z23.VnS(), -12),
                 "cmpgt p15.s, p4/z, z23.s, #-12");
  COMPARE_PREFIX(cmpgt(p15.VnD(), p4.Zeroing(), z23.VnD(), -5),
                 "cmpgt p15.d, p4/z, z23.d, #-5");
  COMPARE_PREFIX(cmple(p4.VnB(), p3.Zeroing(), z5.VnB(), 12),
                 "cmple p4.b, p3/z, z5.b, #12");
  COMPARE_PREFIX(cmple(p4.VnH(), p3.Zeroing(), z5.VnH(), 4),
                 "cmple p4.h, p3/z, z5.h, #4");
  COMPARE_PREFIX(cmple(p4.VnS(), p3.Zeroing(), z5.VnS(), -11),
                 "cmple p4.s, p3/z, z5.s, #-11");
  COMPARE_PREFIX(cmple(p4.VnD(), p3.Zeroing(), z5.VnD(), -6),
                 "cmple p4.d, p3/z, z5.d, #-6");
  COMPARE_PREFIX(cmplt(p3.VnB(), p7.Zeroing(), z15.VnB(), 11),
                 "cmplt p3.b, p7/z, z15.b, #11");
  COMPARE_PREFIX(cmplt(p3.VnH(), p7.Zeroing(), z15.VnH(), 3),
                 "cmplt p3.h, p7/z, z15.h, #3");
  COMPARE_PREFIX(cmplt(p3.VnS(), p7.Zeroing(), z15.VnS(), -10),
                 "cmplt p3.s, p7/z, z15.s, #-10");
  COMPARE_PREFIX(cmplt(p3.VnD(), p7.Zeroing(), z15.VnD(), -7),
                 "cmplt p3.d, p7/z, z15.d, #-7");
  COMPARE_PREFIX(cmpne(p13.VnB(), p5.Zeroing(), z20.VnB(), 10),
                 "cmpne p13.b, p5/z, z20.b, #10");
  COMPARE_PREFIX(cmpne(p13.VnH(), p5.Zeroing(), z20.VnH(), 2),
                 "cmpne p13.h, p5/z, z20.h, #2");
  COMPARE_PREFIX(cmpne(p13.VnS(), p5.Zeroing(), z20.VnS(), -9),
                 "cmpne p13.s, p5/z, z20.s, #-9");
  COMPARE_PREFIX(cmpne(p13.VnD(), p5.Zeroing(), z20.VnD(), -8),
                 "cmpne p13.d, p5/z, z20.d, #-8");

  CLEANUP();
}

TEST(sve_int_compare_unsigned_imm) {
  SETUP();

  COMPARE_PREFIX(cmphi(p8.VnB(), p6.Zeroing(), z1.VnB(), 127),
                 "cmphi p8.b, p6/z, z1.b, #127");
  COMPARE_PREFIX(cmphi(p8.VnH(), p6.Zeroing(), z1.VnH(), 126),
                 "cmphi p8.h, p6/z, z1.h, #126");
  COMPARE_PREFIX(cmphi(p8.VnS(), p6.Zeroing(), z1.VnS(), 99),
                 "cmphi p8.s, p6/z, z1.s, #99");
  COMPARE_PREFIX(cmphi(p8.VnD(), p6.Zeroing(), z1.VnD(), 78),
                 "cmphi p8.d, p6/z, z1.d, #78");
  COMPARE_PREFIX(cmphs(p11.VnB(), p2.Zeroing(), z8.VnB(), 67),
                 "cmphs p11.b, p2/z, z8.b, #67");
  COMPARE_PREFIX(cmphs(p11.VnH(), p2.Zeroing(), z8.VnH(), 63),
                 "cmphs p11.h, p2/z, z8.h, #63");
  COMPARE_PREFIX(cmphs(p11.VnS(), p2.Zeroing(), z8.VnS(), 51),
                 "cmphs p11.s, p2/z, z8.s, #51");
  COMPARE_PREFIX(cmphs(p11.VnD(), p2.Zeroing(), z8.VnD(), 40),
                 "cmphs p11.d, p2/z, z8.d, #40");
  COMPARE_PREFIX(cmplo(p9.VnB(), p4.Zeroing(), z4.VnB(), 32),
                 "cmplo p9.b, p4/z, z4.b, #32");
  COMPARE_PREFIX(cmplo(p9.VnH(), p4.Zeroing(), z4.VnH(), 22),
                 "cmplo p9.h, p4/z, z4.h, #22");
  COMPARE_PREFIX(cmplo(p9.VnS(), p4.Zeroing(), z4.VnS(), 15),
                 "cmplo p9.s, p4/z, z4.s, #15");
  COMPARE_PREFIX(cmplo(p9.VnD(), p4.Zeroing(), z4.VnD(), 11),
                 "cmplo p9.d, p4/z, z4.d, #11");
  COMPARE_PREFIX(cmpls(p14.VnB(), p5.Zeroing(), z9.VnB(), 7),
                 "cmpls p14.b, p5/z, z9.b, #7");
  COMPARE_PREFIX(cmpls(p14.VnH(), p5.Zeroing(), z9.VnH(), 4),
                 "cmpls p14.h, p5/z, z9.h, #4");
  COMPARE_PREFIX(cmpls(p14.VnS(), p5.Zeroing(), z9.VnS(), 3),
                 "cmpls p14.s, p5/z, z9.s, #3");
  COMPARE_PREFIX(cmpls(p14.VnD(), p5.Zeroing(), z9.VnD(), 1),
                 "cmpls p14.d, p5/z, z9.d, #1");

  CLEANUP();
}

TEST(sve_int_compare_vectors) {
  SETUP();

  COMPARE_PREFIX(cmpeq(p13.VnB(), p0.Zeroing(), z26.VnB(), z10.VnD()),
                 "cmpeq p13.b, p0/z, z26.b, z10.d");
  COMPARE_PREFIX(cmpeq(p13.VnH(), p0.Zeroing(), z26.VnH(), z10.VnD()),
                 "cmpeq p13.h, p0/z, z26.h, z10.d");
  COMPARE_PREFIX(cmpeq(p13.VnS(), p0.Zeroing(), z26.VnS(), z10.VnD()),
                 "cmpeq p13.s, p0/z, z26.s, z10.d");
  COMPARE_PREFIX(cmpeq(p14.VnB(), p3.Zeroing(), z18.VnB(), z15.VnB()),
                 "cmpeq p14.b, p3/z, z18.b, z15.b");
  COMPARE_PREFIX(cmpeq(p14.VnH(), p3.Zeroing(), z18.VnH(), z15.VnH()),
                 "cmpeq p14.h, p3/z, z18.h, z15.h");
  COMPARE_PREFIX(cmpeq(p14.VnS(), p3.Zeroing(), z18.VnS(), z15.VnS()),
                 "cmpeq p14.s, p3/z, z18.s, z15.s");
  COMPARE_PREFIX(cmpeq(p14.VnD(), p3.Zeroing(), z18.VnD(), z15.VnD()),
                 "cmpeq p14.d, p3/z, z18.d, z15.d");
  COMPARE_PREFIX(cmpge(p8.VnB(), p3.Zeroing(), z13.VnB(), z0.VnD()),
                 "cmpge p8.b, p3/z, z13.b, z0.d");
  COMPARE_PREFIX(cmpge(p8.VnH(), p3.Zeroing(), z13.VnH(), z0.VnD()),
                 "cmpge p8.h, p3/z, z13.h, z0.d");
  COMPARE_PREFIX(cmpge(p8.VnS(), p3.Zeroing(), z13.VnS(), z0.VnD()),
                 "cmpge p8.s, p3/z, z13.s, z0.d");
  COMPARE_PREFIX(cmpge(p3.VnB(), p4.Zeroing(), z6.VnB(), z1.VnB()),
                 "cmpge p3.b, p4/z, z6.b, z1.b");
  COMPARE_PREFIX(cmpge(p3.VnH(), p4.Zeroing(), z6.VnH(), z1.VnH()),
                 "cmpge p3.h, p4/z, z6.h, z1.h");
  COMPARE_PREFIX(cmpge(p3.VnS(), p4.Zeroing(), z6.VnS(), z1.VnS()),
                 "cmpge p3.s, p4/z, z6.s, z1.s");
  COMPARE_PREFIX(cmpge(p3.VnD(), p4.Zeroing(), z6.VnD(), z1.VnD()),
                 "cmpge p3.d, p4/z, z6.d, z1.d");
  COMPARE_PREFIX(cmpgt(p4.VnB(), p2.Zeroing(), z24.VnB(), z1.VnD()),
                 "cmpgt p4.b, p2/z, z24.b, z1.d");
  COMPARE_PREFIX(cmpgt(p4.VnH(), p2.Zeroing(), z24.VnH(), z1.VnD()),
                 "cmpgt p4.h, p2/z, z24.h, z1.d");
  COMPARE_PREFIX(cmpgt(p4.VnS(), p2.Zeroing(), z24.VnS(), z1.VnD()),
                 "cmpgt p4.s, p2/z, z24.s, z1.d");
  COMPARE_PREFIX(cmpgt(p10.VnB(), p3.Zeroing(), z23.VnB(), z19.VnB()),
                 "cmpgt p10.b, p3/z, z23.b, z19.b");
  COMPARE_PREFIX(cmpgt(p10.VnH(), p3.Zeroing(), z23.VnH(), z19.VnH()),
                 "cmpgt p10.h, p3/z, z23.h, z19.h");
  COMPARE_PREFIX(cmpgt(p10.VnS(), p3.Zeroing(), z23.VnS(), z19.VnS()),
                 "cmpgt p10.s, p3/z, z23.s, z19.s");
  COMPARE_PREFIX(cmpgt(p10.VnD(), p3.Zeroing(), z23.VnD(), z19.VnD()),
                 "cmpgt p10.d, p3/z, z23.d, z19.d");
  COMPARE_PREFIX(cmphi(p10.VnB(), p6.Zeroing(), z6.VnB(), z11.VnD()),
                 "cmphi p10.b, p6/z, z6.b, z11.d");
  COMPARE_PREFIX(cmphi(p10.VnH(), p6.Zeroing(), z6.VnH(), z11.VnD()),
                 "cmphi p10.h, p6/z, z6.h, z11.d");
  COMPARE_PREFIX(cmphi(p10.VnS(), p6.Zeroing(), z6.VnS(), z11.VnD()),
                 "cmphi p10.s, p6/z, z6.s, z11.d");
  COMPARE_PREFIX(cmphi(p1.VnB(), p0.Zeroing(), z4.VnB(), z2.VnB()),
                 "cmphi p1.b, p0/z, z4.b, z2.b");
  COMPARE_PREFIX(cmphi(p1.VnH(), p0.Zeroing(), z4.VnH(), z2.VnH()),
                 "cmphi p1.h, p0/z, z4.h, z2.h");
  COMPARE_PREFIX(cmphi(p1.VnS(), p0.Zeroing(), z4.VnS(), z2.VnS()),
                 "cmphi p1.s, p0/z, z4.s, z2.s");
  COMPARE_PREFIX(cmphi(p1.VnD(), p0.Zeroing(), z4.VnD(), z2.VnD()),
                 "cmphi p1.d, p0/z, z4.d, z2.d");
  COMPARE_PREFIX(cmphs(p10.VnB(), p5.Zeroing(), z22.VnB(), z5.VnD()),
                 "cmphs p10.b, p5/z, z22.b, z5.d");
  COMPARE_PREFIX(cmphs(p10.VnH(), p5.Zeroing(), z22.VnH(), z5.VnD()),
                 "cmphs p10.h, p5/z, z22.h, z5.d");
  COMPARE_PREFIX(cmphs(p10.VnS(), p5.Zeroing(), z22.VnS(), z5.VnD()),
                 "cmphs p10.s, p5/z, z22.s, z5.d");
  COMPARE_PREFIX(cmphs(p12.VnB(), p6.Zeroing(), z20.VnB(), z24.VnB()),
                 "cmphs p12.b, p6/z, z20.b, z24.b");
  COMPARE_PREFIX(cmphs(p12.VnH(), p6.Zeroing(), z20.VnH(), z24.VnH()),
                 "cmphs p12.h, p6/z, z20.h, z24.h");
  COMPARE_PREFIX(cmphs(p12.VnS(), p6.Zeroing(), z20.VnS(), z24.VnS()),
                 "cmphs p12.s, p6/z, z20.s, z24.s");
  COMPARE_PREFIX(cmphs(p12.VnD(), p6.Zeroing(), z20.VnD(), z24.VnD()),
                 "cmphs p12.d, p6/z, z20.d, z24.d");
  COMPARE_PREFIX(cmple(p11.VnB(), p2.Zeroing(), z18.VnB(), z0.VnD()),
                 "cmple p11.b, p2/z, z18.b, z0.d");
  COMPARE_PREFIX(cmple(p11.VnH(), p2.Zeroing(), z18.VnH(), z0.VnD()),
                 "cmple p11.h, p2/z, z18.h, z0.d");
  COMPARE_PREFIX(cmple(p11.VnS(), p2.Zeroing(), z18.VnS(), z0.VnD()),
                 "cmple p11.s, p2/z, z18.s, z0.d");
  COMPARE_PREFIX(cmplo(p12.VnB(), p6.Zeroing(), z21.VnB(), z10.VnD()),
                 "cmplo p12.b, p6/z, z21.b, z10.d");
  COMPARE_PREFIX(cmplo(p12.VnH(), p6.Zeroing(), z21.VnH(), z10.VnD()),
                 "cmplo p12.h, p6/z, z21.h, z10.d");
  COMPARE_PREFIX(cmplo(p12.VnS(), p6.Zeroing(), z21.VnS(), z10.VnD()),
                 "cmplo p12.s, p6/z, z21.s, z10.d");
  COMPARE_PREFIX(cmpls(p8.VnB(), p4.Zeroing(), z9.VnB(), z15.VnD()),
                 "cmpls p8.b, p4/z, z9.b, z15.d");
  COMPARE_PREFIX(cmpls(p8.VnH(), p4.Zeroing(), z9.VnH(), z15.VnD()),
                 "cmpls p8.h, p4/z, z9.h, z15.d");
  COMPARE_PREFIX(cmpls(p8.VnS(), p4.Zeroing(), z9.VnS(), z15.VnD()),
                 "cmpls p8.s, p4/z, z9.s, z15.d");
  COMPARE_PREFIX(cmplt(p6.VnB(), p6.Zeroing(), z4.VnB(), z8.VnD()),
                 "cmplt p6.b, p6/z, z4.b, z8.d");
  COMPARE_PREFIX(cmplt(p6.VnH(), p6.Zeroing(), z4.VnH(), z8.VnD()),
                 "cmplt p6.h, p6/z, z4.h, z8.d");
  COMPARE_PREFIX(cmplt(p6.VnS(), p6.Zeroing(), z4.VnS(), z8.VnD()),
                 "cmplt p6.s, p6/z, z4.s, z8.d");
  COMPARE_PREFIX(cmpne(p1.VnB(), p6.Zeroing(), z31.VnB(), z16.VnD()),
                 "cmpne p1.b, p6/z, z31.b, z16.d");
  COMPARE_PREFIX(cmpne(p1.VnH(), p6.Zeroing(), z31.VnH(), z16.VnD()),
                 "cmpne p1.h, p6/z, z31.h, z16.d");
  COMPARE_PREFIX(cmpne(p1.VnS(), p6.Zeroing(), z31.VnS(), z16.VnD()),
                 "cmpne p1.s, p6/z, z31.s, z16.d");
  COMPARE_PREFIX(cmpne(p11.VnB(), p1.Zeroing(), z3.VnB(), z24.VnB()),
                 "cmpne p11.b, p1/z, z3.b, z24.b");
  COMPARE_PREFIX(cmpne(p11.VnH(), p1.Zeroing(), z3.VnH(), z24.VnH()),
                 "cmpne p11.h, p1/z, z3.h, z24.h");
  COMPARE_PREFIX(cmpne(p11.VnS(), p1.Zeroing(), z3.VnS(), z24.VnS()),
                 "cmpne p11.s, p1/z, z3.s, z24.s");
  COMPARE_PREFIX(cmpne(p11.VnD(), p1.Zeroing(), z3.VnD(), z24.VnD()),
                 "cmpne p11.d, p1/z, z3.d, z24.d");
  COMPARE_PREFIX(cmpls(p8.VnB(), p4.Zeroing(), z9.VnB(), z15.VnB()),
                 "cmphs p8.b, p4/z, z15.b, z9.b");
  COMPARE_PREFIX(cmpls(p8.VnH(), p4.Zeroing(), z9.VnH(), z15.VnH()),
                 "cmphs p8.h, p4/z, z15.h, z9.h");
  COMPARE_PREFIX(cmpls(p8.VnS(), p4.Zeroing(), z9.VnS(), z15.VnS()),
                 "cmphs p8.s, p4/z, z15.s, z9.s");
  COMPARE_PREFIX(cmpls(p8.VnD(), p4.Zeroing(), z9.VnD(), z15.VnD()),
                 "cmphs p8.d, p4/z, z15.d, z9.d");
  COMPARE_PREFIX(cmplo(p10.VnB(), p3.Zeroing(), z14.VnB(), z20.VnB()),
                 "cmphi p10.b, p3/z, z20.b, z14.b");
  COMPARE_PREFIX(cmplo(p10.VnH(), p3.Zeroing(), z14.VnH(), z20.VnH()),
                 "cmphi p10.h, p3/z, z20.h, z14.h");
  COMPARE_PREFIX(cmplo(p10.VnS(), p3.Zeroing(), z14.VnS(), z20.VnS()),
                 "cmphi p10.s, p3/z, z20.s, z14.s");
  COMPARE_PREFIX(cmplo(p10.VnD(), p3.Zeroing(), z14.VnD(), z20.VnD()),
                 "cmphi p10.d, p3/z, z20.d, z14.d");
  COMPARE_PREFIX(cmple(p12.VnB(), p2.Zeroing(), z19.VnB(), z25.VnB()),
                 "cmpge p12.b, p2/z, z25.b, z19.b");
  COMPARE_PREFIX(cmple(p12.VnH(), p2.Zeroing(), z19.VnH(), z25.VnH()),
                 "cmpge p12.h, p2/z, z25.h, z19.h");
  COMPARE_PREFIX(cmple(p12.VnS(), p2.Zeroing(), z19.VnS(), z25.VnS()),
                 "cmpge p12.s, p2/z, z25.s, z19.s");
  COMPARE_PREFIX(cmple(p12.VnD(), p2.Zeroing(), z19.VnD(), z25.VnD()),
                 "cmpge p12.d, p2/z, z25.d, z19.d");
  COMPARE_PREFIX(cmplt(p14.VnB(), p1.Zeroing(), z24.VnB(), z30.VnB()),
                 "cmpgt p14.b, p1/z, z30.b, z24.b");
  COMPARE_PREFIX(cmplt(p14.VnH(), p1.Zeroing(), z24.VnH(), z30.VnH()),
                 "cmpgt p14.h, p1/z, z30.h, z24.h");
  COMPARE_PREFIX(cmplt(p14.VnS(), p1.Zeroing(), z24.VnS(), z30.VnS()),
                 "cmpgt p14.s, p1/z, z30.s, z24.s");
  COMPARE_PREFIX(cmplt(p14.VnD(), p1.Zeroing(), z24.VnD(), z30.VnD()),
                 "cmpgt p14.d, p1/z, z30.d, z24.d");

  CLEANUP();
}

TEST(sve_int_misc_unpredicated) {
  SETUP();

#if 0
  COMPARE_PREFIX(fexpa(z5.VnH(), z9.VnH()), "fexpa <Zd>.<T>, <Zn>.<T>");
  COMPARE_PREFIX(fexpa(z5.VnS(), z9.VnS()), "fexpa <Zd>.<T>, <Zn>.<T>");
  COMPARE_PREFIX(fexpa(z5.VnD(), z9.VnD()), "fexpa <Zd>.<T>, <Zn>.<T>");
  COMPARE_PREFIX(ftssel(z17.VnH(), z24.VnH(), z14.VnH()), "ftssel <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(ftssel(z17.VnS(), z24.VnS(), z14.VnS()), "ftssel <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(ftssel(z17.VnD(), z24.VnD(), z14.VnD()), "ftssel <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
#endif
  COMPARE_PREFIX(movprfx(z24, z1), "movprfx z24, z1");

  CLEANUP();
}

TEST(sve_int_mul_add_predicated) {
  SETUP();

  COMPARE_PREFIX(mad(z29.VnB(), p6.Merging(), z22.VnB(), z21.VnB()),
                 "mad z29.b, p6/m, z22.b, z21.b");
  COMPARE_PREFIX(mad(z29.VnH(), p6.Merging(), z22.VnH(), z21.VnH()),
                 "mad z29.h, p6/m, z22.h, z21.h");
  COMPARE_PREFIX(mad(z29.VnS(), p6.Merging(), z22.VnS(), z21.VnS()),
                 "mad z29.s, p6/m, z22.s, z21.s");
  COMPARE_PREFIX(mad(z29.VnD(), p6.Merging(), z22.VnD(), z21.VnD()),
                 "mad z29.d, p6/m, z22.d, z21.d");
  COMPARE_PREFIX(mla(z23.VnB(), p1.Merging(), z21.VnB(), z23.VnB()),
                 "mla z23.b, p1/m, z21.b, z23.b");
  COMPARE_PREFIX(mla(z23.VnH(), p1.Merging(), z21.VnH(), z23.VnH()),
                 "mla z23.h, p1/m, z21.h, z23.h");
  COMPARE_PREFIX(mla(z23.VnS(), p1.Merging(), z21.VnS(), z23.VnS()),
                 "mla z23.s, p1/m, z21.s, z23.s");
  COMPARE_PREFIX(mla(z23.VnD(), p1.Merging(), z21.VnD(), z23.VnD()),
                 "mla z23.d, p1/m, z21.d, z23.d");
  COMPARE_PREFIX(mls(z4.VnB(), p6.Merging(), z17.VnB(), z28.VnB()),
                 "mls z4.b, p6/m, z17.b, z28.b");
  COMPARE_PREFIX(mls(z4.VnH(), p6.Merging(), z17.VnH(), z28.VnH()),
                 "mls z4.h, p6/m, z17.h, z28.h");
  COMPARE_PREFIX(mls(z4.VnS(), p6.Merging(), z17.VnS(), z28.VnS()),
                 "mls z4.s, p6/m, z17.s, z28.s");
  COMPARE_PREFIX(mls(z4.VnD(), p6.Merging(), z17.VnD(), z28.VnD()),
                 "mls z4.d, p6/m, z17.d, z28.d");
  COMPARE_PREFIX(msb(z27.VnB(), p7.Merging(), z29.VnB(), z1.VnB()),
                 "msb z27.b, p7/m, z29.b, z1.b");
  COMPARE_PREFIX(msb(z27.VnH(), p7.Merging(), z29.VnH(), z1.VnH()),
                 "msb z27.h, p7/m, z29.h, z1.h");
  COMPARE_PREFIX(msb(z27.VnS(), p7.Merging(), z29.VnS(), z1.VnS()),
                 "msb z27.s, p7/m, z29.s, z1.s");
  COMPARE_PREFIX(msb(z27.VnD(), p7.Merging(), z29.VnD(), z1.VnD()),
                 "msb z27.d, p7/m, z29.d, z1.d");

  CLEANUP();
}

TEST(sve_int_mul_add_predicated_macro) {
  SETUP();

  COMPARE_MACRO(Mla(z0.VnB(), p1.Merging(), z0.VnB(), z2.VnB(), z4.VnB()),
                "mla z0.b, p1/m, z2.b, z4.b");
  COMPARE_MACRO(Mla(z3.VnH(), p2.Merging(), z4.VnH(), z3.VnH(), z5.VnH()),
                "mad z3.h, p2/m, z5.h, z4.h");
  COMPARE_MACRO(Mla(z4.VnS(), p3.Merging(), z5.VnS(), z6.VnS(), z4.VnS()),
                "mad z4.s, p3/m, z6.s, z5.s");
  COMPARE_MACRO(Mla(z5.VnD(), p4.Merging(), z6.VnD(), z7.VnD(), z8.VnD()),
                "movprfx z5.d, p4/m, z6.d\n"
                "mla z5.d, p4/m, z7.d, z8.d");

  COMPARE_MACRO(Mls(z0.VnD(), p1.Merging(), z0.VnD(), z2.VnD(), z4.VnD()),
                "mls z0.d, p1/m, z2.d, z4.d");
  COMPARE_MACRO(Mls(z3.VnS(), p2.Merging(), z4.VnS(), z3.VnS(), z5.VnS()),
                "msb z3.s, p2/m, z5.s, z4.s");
  COMPARE_MACRO(Mls(z4.VnH(), p3.Merging(), z5.VnH(), z6.VnH(), z4.VnH()),
                "msb z4.h, p3/m, z6.h, z5.h");
  COMPARE_MACRO(Mls(z5.VnB(), p4.Merging(), z6.VnB(), z7.VnB(), z8.VnB()),
                "movprfx z5.b, p4/m, z6.b\n"
                "mls z5.b, p4/m, z7.b, z8.b");

  CLEANUP();
}

TEST(sve_int_mul_add_unpredicated) {
  SETUP();

#if 0
  COMPARE_PREFIX(sdot(z18.Vn?(), z27, z22), "sdot <Zda>.<T>, <Zn>.<Tb>, <Zm>.<Tb>");
  COMPARE_PREFIX(udot(z21.Vn?(), z9, z4), "udot <Zda>.<T>, <Zn>.<Tb>, <Zm>.<Tb>");
#endif

  CLEANUP();
}

TEST(sve_int_reduction) {
  SETUP();

  COMPARE_PREFIX(andv(b15, p1, z4.VnB()), "andv b15, p1, z4.b");
  COMPARE_PREFIX(andv(h14, p2, z3.VnH()), "andv h14, p2, z3.h");
  COMPARE_PREFIX(andv(s13, p3, z2.VnS()), "andv s13, p3, z2.s");
  COMPARE_PREFIX(andv(d12, p4, z1.VnD()), "andv d12, p4, z1.d");
  COMPARE_PREFIX(eorv(b12, p0, z30.VnB()), "eorv b12, p0, z30.b");
  COMPARE_PREFIX(eorv(h11, p1, z29.VnH()), "eorv h11, p1, z29.h");
  COMPARE_PREFIX(eorv(s10, p2, z28.VnS()), "eorv s10, p2, z28.s");
  COMPARE_PREFIX(eorv(d9, p3, z27.VnD()), "eorv d9, p3, z27.d");
  COMPARE_PREFIX(movprfx(z30.VnB(), p2.Zeroing(), z23.VnB()),
                 "movprfx z30.b, p2/z, z23.b");
  COMPARE_PREFIX(movprfx(z10.VnH(), p0.Merging(), z10.VnH()),
                 "movprfx z10.h, p0/m, z10.h");
  COMPARE_PREFIX(movprfx(z0.VnS(), p2.Zeroing(), z23.VnS()),
                 "movprfx z0.s, p2/z, z23.s");
  COMPARE_PREFIX(movprfx(z31.VnD(), p7.Merging(), z23.VnD()),
                 "movprfx z31.d, p7/m, z23.d");
  COMPARE_PREFIX(orv(b4, p0, z16.VnB()), "orv b4, p0, z16.b");
  COMPARE_PREFIX(orv(h6, p2, z18.VnH()), "orv h6, p2, z18.h");
  COMPARE_PREFIX(orv(s8, p4, z20.VnS()), "orv s8, p4, z20.s");
  COMPARE_PREFIX(orv(d10, p6, z22.VnD()), "orv d10, p6, z22.d");
#if 0
  COMPARE_PREFIX(saddv(d24, p1, z12.VnB()), "saddv <Dd>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(saddv(d24, p1, z12.VnH()), "saddv <Dd>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(saddv(d24, p1, z12.VnS()), "saddv <Dd>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(smaxv(b29, p0, z1.VnB()), "smaxv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(smaxv(h29, p0, z1.VnH()), "smaxv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(smaxv(s29, p0, z1.VnS()), "smaxv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(smaxv(d29, p0, z1.VnD()), "smaxv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(sminv(b8, p3, z4.VnB()), "sminv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(sminv(h8, p3, z4.VnH()), "sminv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(sminv(s8, p3, z4.VnS()), "sminv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(sminv(d8, p3, z4.VnD()), "sminv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(uaddv(d19, p2, z30.VnB()), "uaddv <Dd>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(uaddv(d19, p2, z30.VnH()), "uaddv <Dd>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(uaddv(d19, p2, z30.VnS()), "uaddv <Dd>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(uaddv(d19, p2, z30.VnD()), "uaddv <Dd>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(umaxv(b28, p6, z24.VnB()), "umaxv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(umaxv(h28, p6, z24.VnH()), "umaxv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(umaxv(s28, p6, z24.VnS()), "umaxv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(umaxv(d28, p6, z24.VnD()), "umaxv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(uminv(b16, p3, z30.VnB()), "uminv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(uminv(h16, p3, z30.VnH()), "uminv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(uminv(s16, p3, z30.VnS()), "uminv <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(uminv(d16, p3, z30.VnD()), "uminv <V><d>, <Pg>, <Zn>.<T>");
#endif

  CLEANUP();
}

TEST(sve_int_unary_arithmetic_predicated) {
  SETUP();

  COMPARE_PREFIX(abs(z5.VnB(), p5.Merging(), z31.VnB()),
                 "abs z5.b, p5/m, z31.b");
  COMPARE_PREFIX(abs(z29.VnH(), p5.Merging(), z17.VnH()),
                 "abs z29.h, p5/m, z17.h");
  COMPARE_PREFIX(abs(z6.VnS(), p4.Merging(), z24.VnS()),
                 "abs z6.s, p4/m, z24.s");
  COMPARE_PREFIX(abs(z19.VnD(), p3.Merging(), z25.VnD()),
                 "abs z19.d, p3/m, z25.d");
  COMPARE_PREFIX(cls(z4.VnB(), p0.Merging(), z20.VnB()),
                 "cls z4.b, p0/m, z20.b");
  COMPARE_PREFIX(cls(z11.VnH(), p0.Merging(), z26.VnH()),
                 "cls z11.h, p0/m, z26.h");
  COMPARE_PREFIX(cls(z10.VnS(), p1.Merging(), z10.VnS()),
                 "cls z10.s, p1/m, z10.s");
  COMPARE_PREFIX(cls(z5.VnD(), p1.Merging(), z4.VnD()), "cls z5.d, p1/m, z4.d");
  COMPARE_PREFIX(clz(z18.VnB(), p3.Merging(), z1.VnB()),
                 "clz z18.b, p3/m, z1.b");
  COMPARE_PREFIX(clz(z13.VnH(), p4.Merging(), z18.VnH()),
                 "clz z13.h, p4/m, z18.h");
  COMPARE_PREFIX(clz(z15.VnS(), p4.Merging(), z24.VnS()),
                 "clz z15.s, p4/m, z24.s");
  COMPARE_PREFIX(clz(z29.VnD(), p2.Merging(), z22.VnD()),
                 "clz z29.d, p2/m, z22.d");
  COMPARE_PREFIX(cnot(z16.VnB(), p6.Merging(), z20.VnB()),
                 "cnot z16.b, p6/m, z20.b");
  COMPARE_PREFIX(cnot(z10.VnH(), p5.Merging(), z12.VnH()),
                 "cnot z10.h, p5/m, z12.h");
  COMPARE_PREFIX(cnot(z8.VnS(), p5.Merging(), z21.VnS()),
                 "cnot z8.s, p5/m, z21.s");
  COMPARE_PREFIX(cnot(z3.VnD(), p3.Merging(), z18.VnD()),
                 "cnot z3.d, p3/m, z18.d");
  COMPARE_PREFIX(cnt(z29.VnB(), p3.Merging(), z7.VnB()),
                 "cnt z29.b, p3/m, z7.b");
  COMPARE_PREFIX(cnt(z3.VnH(), p6.Merging(), z31.VnH()),
                 "cnt z3.h, p6/m, z31.h");
  COMPARE_PREFIX(cnt(z2.VnS(), p4.Merging(), z16.VnS()),
                 "cnt z2.s, p4/m, z16.s");
  COMPARE_PREFIX(cnt(z0.VnD(), p0.Merging(), z24.VnD()),
                 "cnt z0.d, p0/m, z24.d");
  COMPARE_PREFIX(fabs(z17.VnH(), p7.Merging(), z15.VnH()),
                 "fabs z17.h, p7/m, z15.h");
  COMPARE_PREFIX(fabs(z18.VnS(), p0.Merging(), z29.VnS()),
                 "fabs z18.s, p0/m, z29.s");
  COMPARE_PREFIX(fabs(z17.VnD(), p1.Merging(), z9.VnD()),
                 "fabs z17.d, p1/m, z9.d");
  COMPARE_PREFIX(fneg(z25.VnH(), p1.Merging(), z28.VnH()),
                 "fneg z25.h, p1/m, z28.h");
  COMPARE_PREFIX(fneg(z5.VnS(), p1.Merging(), z25.VnS()),
                 "fneg z5.s, p1/m, z25.s");
  COMPARE_PREFIX(fneg(z6.VnD(), p1.Merging(), z17.VnD()),
                 "fneg z6.d, p1/m, z17.d");
  COMPARE_PREFIX(neg(z25.VnB(), p4.Merging(), z8.VnB()),
                 "neg z25.b, p4/m, z8.b");
  COMPARE_PREFIX(neg(z30.VnH(), p3.Merging(), z23.VnH()),
                 "neg z30.h, p3/m, z23.h");
  COMPARE_PREFIX(neg(z7.VnS(), p2.Merging(), z26.VnS()),
                 "neg z7.s, p2/m, z26.s");
  COMPARE_PREFIX(neg(z21.VnD(), p3.Merging(), z5.VnD()),
                 "neg z21.d, p3/m, z5.d");
  COMPARE_PREFIX(not_(z24.VnB(), p1.Merging(), z27.VnB()),
                 "not z24.b, p1/m, z27.b");
  COMPARE_PREFIX(not_(z31.VnH(), p6.Merging(), z19.VnH()),
                 "not z31.h, p6/m, z19.h");
  COMPARE_PREFIX(not_(z18.VnS(), p5.Merging(), z13.VnS()),
                 "not z18.s, p5/m, z13.s");
  COMPARE_PREFIX(not_(z12.VnD(), p2.Merging(), z28.VnD()),
                 "not z12.d, p2/m, z28.d");
  COMPARE_PREFIX(sxtb(z19.VnH(), p7.Merging(), z3.VnH()),
                 "sxtb z19.h, p7/m, z3.h");
  COMPARE_PREFIX(sxtb(z3.VnS(), p1.Merging(), z17.VnS()),
                 "sxtb z3.s, p1/m, z17.s");
  COMPARE_PREFIX(sxtb(z27.VnD(), p0.Merging(), z12.VnD()),
                 "sxtb z27.d, p0/m, z12.d");
  COMPARE_PREFIX(sxth(z6.VnS(), p1.Merging(), z17.VnS()),
                 "sxth z6.s, p1/m, z17.s");
  COMPARE_PREFIX(sxth(z8.VnD(), p6.Merging(), z2.VnD()),
                 "sxth z8.d, p6/m, z2.d");
  COMPARE_PREFIX(sxtw(z13.VnD(), p3.Merging(), z27.VnD()),
                 "sxtw z13.d, p3/m, z27.d");
  COMPARE_PREFIX(uxtb(z23.VnH(), p3.Merging(), z21.VnH()),
                 "uxtb z23.h, p3/m, z21.h");
  COMPARE_PREFIX(uxtb(z0.VnS(), p2.Merging(), z13.VnS()),
                 "uxtb z0.s, p2/m, z13.s");
  COMPARE_PREFIX(uxtb(z1.VnD(), p3.Merging(), z13.VnD()),
                 "uxtb z1.d, p3/m, z13.d");
  COMPARE_PREFIX(uxth(z27.VnS(), p0.Merging(), z29.VnS()),
                 "uxth z27.s, p0/m, z29.s");
  COMPARE_PREFIX(uxth(z22.VnD(), p4.Merging(), z20.VnD()),
                 "uxth z22.d, p4/m, z20.d");
  COMPARE_PREFIX(uxtw(z14.VnD(), p1.Merging(), z13.VnD()),
                 "uxtw z14.d, p1/m, z13.d");

  CLEANUP();
}

TEST(sve_neg_macro) {
  SETUP();

  COMPARE_MACRO(Neg(z0.VnB(), z0.VnB()), "subr z0.b, z0.b, #0");
  COMPARE_MACRO(Neg(z1.VnH(), z2.VnH()),
                "movprfx z1, z2\n"
                "subr z1.h, z1.h, #0");
  COMPARE_MACRO(Neg(z29.VnS(), z29.VnS()), "subr z29.s, z29.s, #0");
  COMPARE_MACRO(Neg(z30.VnD(), z31.VnD()),
                "movprfx z30, z31\n"
                "subr z30.d, z30.d, #0");

  CLEANUP();
}

TEST(sve_int_wide_imm_predicated) {
  SETUP();

#if 0
  COMPARE_PREFIX(cpy(z25.VnB(), p13, int imm8), "cpy <Zd>.<T>, <Pg>/<ZM>, #<imm>{, <shift>}");
  COMPARE_PREFIX(cpy(z25.VnH(), p13, int imm8), "cpy <Zd>.<T>, <Pg>/<ZM>, #<imm>{, <shift>}");
  COMPARE_PREFIX(cpy(z25.VnS(), p13, int imm8), "cpy <Zd>.<T>, <Pg>/<ZM>, #<imm>{, <shift>}");
  COMPARE_PREFIX(cpy(z25.VnD(), p13, int imm8), "cpy <Zd>.<T>, <Pg>/<ZM>, #<imm>{, <shift>}");
  COMPARE_PREFIX(fcpy(z20.VnH(), p11.Merging()), "fcpy <Zd>.<T>, <Pg>/M, #<const>");
  COMPARE_PREFIX(fcpy(z20.VnS(), p11.Merging()), "fcpy <Zd>.<T>, <Pg>/M, #<const>");
  COMPARE_PREFIX(fcpy(z20.VnD(), p11.Merging()), "fcpy <Zd>.<T>, <Pg>/M, #<const>");
#endif

  CLEANUP();
}

TEST(sve_int_wide_imm_unpredicated) {
  SETUP();

  COMPARE_PREFIX(add(z12.VnB(), z12.VnB(), 0), "add z12.b, z12.b, #0");
  COMPARE_PREFIX(add(z13.VnH(), z13.VnH(), 255), "add z13.h, z13.h, #255");
  COMPARE_PREFIX(add(z14.VnS(), z14.VnS(), 256),
                 "add z14.s, z14.s, #1, lsl #8");
  COMPARE_PREFIX(add(z15.VnD(), z15.VnD(), 255 * 256),
                 "add z15.d, z15.d, #255, lsl #8");

  COMPARE_PREFIX(dup(z6.VnB(), -128), "dup z6.b, #-128");
  COMPARE_PREFIX(dup(z7.VnH(), 127), "dup z7.h, #127");
  COMPARE_PREFIX(dup(z8.VnS(), -128 * 256), "dup z8.s, #-128, lsl #8");
  COMPARE_PREFIX(dup(z9.VnD(), 127 * 256), "dup z9.d, #127, lsl #8");

  COMPARE_PREFIX(sqadd(z7.VnB(), z7.VnB(), 124), "sqadd z7.b, z7.b, #124");
  COMPARE_PREFIX(sqadd(z8.VnH(), z8.VnH(), 131), "sqadd z8.h, z8.h, #131");
  COMPARE_PREFIX(sqadd(z9.VnS(), z9.VnS(), 252 * 256),
                 "sqadd z9.s, z9.s, #252, lsl #8");
  COMPARE_PREFIX(sqadd(z10.VnD(), z10.VnD(), 20 * 256),
                 "sqadd z10.d, z10.d, #20, lsl #8");

  COMPARE_PREFIX(sqsub(z31.VnB(), z31.VnB(), 132), "sqsub z31.b, z31.b, #132");
  COMPARE_PREFIX(sqsub(z30.VnH(), z30.VnH(), 251), "sqsub z30.h, z30.h, #251");
  COMPARE_PREFIX(sqsub(z29.VnS(), z29.VnS(), 21 * 256),
                 "sqsub z29.s, z29.s, #21, lsl #8");
  COMPARE_PREFIX(sqsub(z28.VnD(), z28.VnD(), 123 * 256),
                 "sqsub z28.d, z28.d, #123, lsl #8");

  COMPARE_PREFIX(subr(z20.VnB(), z20.VnB(), 250), "subr z20.b, z20.b, #250");
  COMPARE_PREFIX(subr(z21.VnH(), z21.VnH(), 22), "subr z21.h, z21.h, #22");
  COMPARE_PREFIX(subr(z22.VnS(), z22.VnS(), 122 * 256),
                 "subr z22.s, z22.s, #122, lsl #8");
  COMPARE_PREFIX(subr(z23.VnD(), z23.VnD(), 133 * 256),
                 "subr z23.d, z23.d, #133, lsl #8");

  COMPARE_PREFIX(sub(z18.VnB(), z18.VnB(), 23), "sub z18.b, z18.b, #23");
  COMPARE_PREFIX(sub(z19.VnH(), z19.VnH(), 121), "sub z19.h, z19.h, #121");
  COMPARE_PREFIX(sub(z20.VnS(), z20.VnS(), 134 * 256),
                 "sub z20.s, z20.s, #134, lsl #8");
  COMPARE_PREFIX(sub(z21.VnD(), z21.VnD(), 249 * 256),
                 "sub z21.d, z21.d, #249, lsl #8");

  COMPARE_PREFIX(uqadd(z21.VnB(), z21.VnB(), 246), "uqadd z21.b, z21.b, #246");
  COMPARE_PREFIX(uqadd(z22.VnH(), z22.VnH(), 26), "uqadd z22.h, z22.h, #26");
  COMPARE_PREFIX(uqadd(z23.VnS(), z23.VnS(), 118 * 256),
                 "uqadd z23.s, z23.s, #118, lsl #8");
  COMPARE_PREFIX(uqadd(z24.VnD(), z24.VnD(), 137 * 256),
                 "uqadd z24.d, z24.d, #137, lsl #8");

  COMPARE_PREFIX(uqsub(z10.VnB(), z10.VnB(), 27), "uqsub z10.b, z10.b, #27");
  COMPARE_PREFIX(uqsub(z11.VnH(), z11.VnH(), 117), "uqsub z11.h, z11.h, #117");
  COMPARE_PREFIX(uqsub(z12.VnS(), z12.VnS(), 138 * 256),
                 "uqsub z12.s, z12.s, #138, lsl #8");
  COMPARE_PREFIX(uqsub(z13.VnD(), z13.VnD(), 245 * 256),
                 "uqsub z13.d, z13.d, #245, lsl #8");

  COMPARE_PREFIX(fdup(z26.VnH(), Float16(-5.0f)),
                 "fdup z26.h, #0x94 (-5.0000)");
  COMPARE_PREFIX(fdup(z27.VnS(), -13.0f), "fdup z27.s, #0xaa (-13.0000)");
  COMPARE_PREFIX(fdup(z28.VnD(), 1.0f), "fdup z28.d, #0x70 (1.0000)");

  COMPARE_PREFIX(mul(z15.VnB(), z15.VnB(), -128), "mul z15.b, z15.b, #-128");
  COMPARE_PREFIX(mul(z16.VnH(), z16.VnH(), -1), "mul z16.h, z16.h, #-1");
  COMPARE_PREFIX(mul(z17.VnS(), z17.VnS(), 17), "mul z17.s, z17.s, #17");
  COMPARE_PREFIX(mul(z18.VnD(), z18.VnD(), 127), "mul z18.d, z18.d, #127");

  COMPARE_PREFIX(smax(z7.VnB(), z7.VnB(), -2), "smax z7.b, z7.b, #-2");
  COMPARE_PREFIX(smax(z8.VnH(), z8.VnH(), 18), "smax z8.h, z8.h, #18");
  COMPARE_PREFIX(smax(z9.VnS(), z9.VnS(), 126), "smax z9.s, z9.s, #126");
  COMPARE_PREFIX(smax(z10.VnD(), z10.VnD(), -127), "smax z10.d, z10.d, #-127");

  COMPARE_PREFIX(smin(z5.VnB(), z5.VnB(), 19), "smin z5.b, z5.b, #19");
  COMPARE_PREFIX(smin(z6.VnH(), z6.VnH(), 125), "smin z6.h, z6.h, #125");
  COMPARE_PREFIX(smin(z7.VnS(), z7.VnS(), -126), "smin z7.s, z7.s, #-126");
  COMPARE_PREFIX(smin(z8.VnD(), z8.VnD(), -3), "smin z8.d, z8.d, #-3");

  COMPARE_PREFIX(umax(z15.VnB(), z15.VnB(), 120), "umax z15.b, z15.b, #120");
  COMPARE_PREFIX(umax(z16.VnH(), z16.VnH(), 135), "umax z16.h, z16.h, #135");
  COMPARE_PREFIX(umax(z17.VnS(), z17.VnS(), 248), "umax z17.s, z17.s, #248");
  COMPARE_PREFIX(umax(z18.VnD(), z18.VnD(), 24), "umax z18.d, z18.d, #24");

  COMPARE_PREFIX(umin(z22.VnB(), z22.VnB(), 136), "umin z22.b, z22.b, #136");
  COMPARE_PREFIX(umin(z23.VnH(), z23.VnH(), 247), "umin z23.h, z23.h, #247");
  COMPARE_PREFIX(umin(z24.VnS(), z24.VnS(), 25), "umin z24.s, z24.s, #25");
  COMPARE_PREFIX(umin(z25.VnD(), z25.VnD(), 119), "umin z25.d, z25.d, #119");

  CLEANUP();
}

TEST(sve_int_wide_imm_unpredicated_macro) {
  SETUP();

  // The MacroAssembler automatically generates movprfx where it can.
  COMPARE_MACRO(Add(z12.VnB(), z13.VnB(), 0),
                "movprfx z12, z13\n"
                "add z12.b, z12.b, #0");
  COMPARE_MACRO(Sqadd(z7.VnB(), z1.VnB(), 124),
                "movprfx z7, z1\n"
                "sqadd z7.b, z7.b, #124");
  COMPARE_MACRO(Sqsub(z31.VnB(), z13.VnB(), 132),
                "movprfx z31, z13\n"
                "sqsub z31.b, z31.b, #132");
  COMPARE_MACRO(Sub(z20.VnB(), 250, z2.VnB()),
                "movprfx z20, z2\n"
                "subr z20.b, z20.b, #250");
  COMPARE_MACRO(Sub(z19.VnH(), z4.VnH(), 121),
                "movprfx z19, z4\n"
                "sub z19.h, z19.h, #121");
  COMPARE_MACRO(Uqadd(z21.VnB(), z14.VnB(), 246),
                "movprfx z21, z14\n"
                "uqadd z21.b, z21.b, #246");
  COMPARE_MACRO(Uqsub(z10.VnB(), z27.VnB(), 27),
                "movprfx z10, z27\n"
                "uqsub z10.b, z10.b, #27");
  COMPARE_MACRO(Mul(z1.VnD(), z18.VnD(), 127),
                "movprfx z1, z18\n"
                "mul z1.d, z1.d, #127");
  COMPARE_MACRO(Smax(z3.VnS(), z9.VnS(), 126),
                "movprfx z3, z9\n"
                "smax z3.s, z3.s, #126");
  COMPARE_MACRO(Smin(z26.VnH(), z6.VnH(), 125),
                "movprfx z26, z6\n"
                "smin z26.h, z26.h, #125");
  COMPARE_MACRO(Umax(z25.VnB(), z15.VnB(), 120),
                "movprfx z25, z15\n"
                "umax z25.b, z25.b, #120");
  COMPARE_MACRO(Umin(z13.VnD(), z25.VnD(), 119),
                "movprfx z13, z25\n"
                "umin z13.d, z13.d, #119");
  COMPARE_MACRO(Dup(z8.VnS(), -7654321),
                "mov w16, #0x344f\n"
                "movk w16, #0xff8b, lsl #16\n"
                "dup z8.s, w16.s");
  COMPARE_MACRO(Add(z13.VnH(), z13.VnH(), 0xffff),
                "dup z31.h, #-1\n"
                "add z13.h, z13.h, z31.h");

  // The MacroAssembler automatically generates dup if an immediate isn't
  // encodable, when it is out-of-range for example.
  COMPARE_MACRO(Add(z15.VnD(), z20.VnD(), 1234567890),
                "mov x16, #0x2d2\n"
                "movk x16, #0x4996, lsl #16\n"
                "dup z31.d, x16.d\n"
                "add z15.d, z20.d, z31.d");
  COMPARE_MACRO(Dup(z9.VnD(), 0x80000000), "dupm z9.d, #0x80000000");
  COMPARE_MACRO(Sqadd(z9.VnS(), z10.VnS(), 0xaaaaaaaa),
                "dupm z31.b, #0xaa\n"
                "sqadd z9.s, z10.s, z31.s");
  COMPARE_MACRO(Sqsub(z3.VnH(), z1.VnH(), 1188),
                "mov w16, #0x4a4\n"
                "dup z31.h, w16.h\n"
                "sqsub z3.h, z1.h, z31.h");
  COMPARE_MACRO(Sub(z22.VnS(), 256 * 256, z2.VnS()),
                "dupm z31.s, #0x10000\n"
                "sub z22.s, z31.s, z2.s");
  COMPARE_MACRO(Sub(z21.VnD(), z11.VnD(), 111111111111),
                "mov x16, #0x1c7\n"
                "movk x16, #0xdebd, lsl #16\n"
                "movk x16, #0x19, lsl #32\n"
                "dup z31.d, x16.d\n"
                "sub z21.d, z11.d, z31.d");
  COMPARE_MACRO(Uqadd(z23.VnH(), z16.VnH(), 5566),
                "mov w16, #0x15be\n"
                "dup z31.h, w16.h\n"
                "uqadd z23.h, z16.h, z31.h");
  COMPARE_MACRO(Uqsub(z11.VnH(), z2.VnH(), 7788),
                "mov w16, #0x1e6c\n"
                "dup z31.h, w16.h\n"
                "uqsub z11.h, z2.h, z31.h");
  COMPARE_MACRO(Fdup(z26.VnH(), Float16(0.0)), "dup z26.h, #0");
  COMPARE_MACRO(Fdup(z27.VnS(), 255.0f),
                "mov w16, #0x437f0000\n"
                "dup z27.s, w16.s");
  COMPARE_MACRO(Fdup(z28.VnD(), 12.3456),
                "mov x16, #0xfec5\n"
                "movk x16, #0x7bb2, lsl #16\n"
                "movk x16, #0xb0f2, lsl #32\n"
                "movk x16, #0x4028, lsl #48\n"
                "dup z28.d, x16.d");

  // Only predicated version of instruction is supported for unencodable
  // immediate.
  {
    UseScratchRegisterScope temps(&masm);
    temps.Include(p7, p15);
    COMPARE_MACRO(Mul(z18.VnD(), z18.VnD(), -1270000000),
                  "ptrue p7.d\n"
                  "mov x16, #0xffffffffffff5680\n"
                  "movk x16, #0xb44d, lsl #16\n"
                  "dup z31.d, x16.d\n"
                  "mul z18.d, p7/m, z18.d, z31.d");
    COMPARE_MACRO(Smax(z9.VnS(), z11.VnS(), -0x70000001),
                  "ptrue p7.s\n"
                  "dupm z9.s, #0x8fffffff\n"
                  "smax z9.s, p7/m, z9.s, z11.s");
    COMPARE_MACRO(Smin(z6.VnH(), z6.VnH(), -0x7eef),
                  "ptrue p7.h\n"
                  "mov w16, #0xffff8111\n"
                  "dup z31.h, w16.h\n"
                  "smin z6.h, p7/m, z6.h, z31.h");
    COMPARE_MACRO(Umax(z15.VnH(), z7.VnH(), 0xfeee),
                  "ptrue p7.h\n"
                  "mov w16, #0xfeee\n"
                  "dup z15.h, w16.h\n"
                  "umax z15.h, p7/m, z15.h, z7.h");
    COMPARE_MACRO(Umin(z25.VnD(), z25.VnD(), 123123123),
                  "ptrue p7.d\n"
                  "mov x16, #0xb5b3\n"
                  "movk x16, #0x756, lsl #16\n"
                  "dup z31.d, x16.d\n"
                  "umin z25.d, p7/m, z25.d, z31.d");
  }
}

TEST(sve_mem_32bit_gather_and_unsized_contiguous) {
  SETUP();

#if 0
  COMPARE_PREFIX(ld1b(z17.VnS(), p4.Zeroing(), z14.VnS(), int imm5), "ld1b { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(ld1b(z9.VnS(), p5.Zeroing(), x2, z1.VnS()), "ld1b { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(ld1h(z21.VnS(), p0.Zeroing(), z7.VnS(), int imm5), "ld1h { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(ld1h(z17.VnS(), p2.Zeroing(), x11, z24.VnS()), "ld1h { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod> #1]");
  COMPARE_PREFIX(ld1h(z9.VnS(), p3.Zeroing(), x18, z4.VnS()), "ld1h { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(ld1rb(z2.VnH(), p0.Zeroing(), x30, int imm6), "ld1rb { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rb(z14.VnS(), p2.Zeroing(), x11, int imm6), "ld1rb { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rb(z27.VnD(), p1.Zeroing(), x29, int imm6), "ld1rb { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rb(z0.VnB(), p3.Zeroing(), x20, int imm6), "ld1rb { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rd(z19.VnD(), p7.Zeroing(), x14, int imm6), "ld1rd { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rh(z19.VnH(), p5.Zeroing(), x1, int imm6), "ld1rh { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rh(z4.VnS(), p7.Zeroing(), x29, int imm6), "ld1rh { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rh(z24.VnD(), p0.Zeroing(), x29, int imm6), "ld1rh { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rsb(z16.VnH(), p1.Zeroing(), x29, int imm6), "ld1rsb { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rsb(z8.VnS(), p6.Zeroing(), x5, int imm6), "ld1rsb { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rsb(z25.VnD(), p2.Zeroing(), x18, int imm6), "ld1rsb { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rsh(z11.VnS(), p5.Zeroing(), x14, int imm6), "ld1rsh { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rsh(z28.VnD(), p1.Zeroing(), x19, int imm6), "ld1rsh { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rsw(z23.VnD(), p4.Zeroing(), x10, int imm6), "ld1rsw { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rw(z19.VnS(), p5.Zeroing(), x4, int imm6), "ld1rw { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rw(z13.VnD(), p3.Zeroing(), x2, int imm6), "ld1rw { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1sb(z28.VnS(), p7.Zeroing(), z6.VnS(), int imm5), "ld1sb { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(ld1sb(z12.VnS(), p7.Zeroing(), x17, z23.VnS()), "ld1sb { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(ld1sh(z15.VnS(), p1.Zeroing(), z22.VnS(), int imm5), "ld1sh { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(ld1sh(z11.VnS(), p2.Zeroing(), x18, z10.VnS()), "ld1sh { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod> #1]");
  COMPARE_PREFIX(ld1sh(z11.VnS(), p4.Zeroing(), x19, z0.VnS()), "ld1sh { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(ld1w(z25.VnS(), p7.Zeroing(), z4.VnS(), int imm5), "ld1w { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(ld1w(z22.VnS(), p6.Zeroing(), x17, z5.VnS()), "ld1w { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod> #2]");
  COMPARE_PREFIX(ld1w(z0.VnS(), p6.Zeroing(), x28, z21.VnS()), "ld1w { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(ldff1b(z16.VnS(), p3.Zeroing(), z3.VnS(), int imm5), "ldff1b { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(ldff1b(z18.VnS(), p6.Zeroing(), x27, z24.VnS()), "ldff1b { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(ldff1h(z16.VnS(), p4.Zeroing(), z9.VnS(), int imm5), "ldff1h { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(ldff1h(z25.VnS(), p3.Zeroing(), x17, z15.VnS()), "ldff1h { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod> #1]");
  COMPARE_PREFIX(ldff1h(z28.VnS(), p6.Zeroing(), x1, z30.VnS()), "ldff1h { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(ldff1sb(z2.VnS(), p0.Zeroing(), z19.VnS(), int imm5), "ldff1sb { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(ldff1sb(z15.VnS(), p5.Zeroing(), x5, z14.VnS()), "ldff1sb { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(ldff1sh(z31.VnS(), p2.Zeroing(), z29.VnS(), int imm5), "ldff1sh { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(ldff1sh(z10.VnS(), p0.Zeroing(), x19, z15.VnS()), "ldff1sh { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod> #1]");
  COMPARE_PREFIX(ldff1sh(z18.VnS(), p4.Zeroing(), x25, z25.VnS()), "ldff1sh { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(ldff1w(z0.VnS(), p2.Zeroing(), z25.VnS(), int imm5), "ldff1w { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(ldff1w(z5.VnS(), p4.Zeroing(), x23, z31.VnS()), "ldff1w { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod> #2]");
  COMPARE_PREFIX(ldff1w(z12.VnS(), p3.Zeroing(), x25, z27.VnS()), "ldff1w { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(prfb(int prfop, p5, z30.VnS(), int imm5), "prfb <prfop>, <Pg>, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(prfb(int prfop, p5, x28, int imm6), "prfb <prfop>, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(prfb(int prfop, p6, x31, x31), "prfb <prfop>, <Pg>, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(prfb(int prfop, p6, x7, z12.VnS()), "prfb <prfop>, <Pg>, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(prfd(int prfop, p5, z11.VnS(), int imm5), "prfd <prfop>, <Pg>, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(prfd(int prfop, p3, x0, int imm6), "prfd <prfop>, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(prfd(int prfop, p7, x5, x5), "prfd <prfop>, <Pg>, [<Xn|SP>, <Xm>, LSL #3]");
  COMPARE_PREFIX(prfd(int prfop, p1, x19, z18.VnS()), "prfd <prfop>, <Pg>, [<Xn|SP>, <Zm>.S, <mod> #3]");
  COMPARE_PREFIX(prfh(int prfop, p6, z0.VnS(), int imm5), "prfh <prfop>, <Pg>, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(prfh(int prfop, p4, x17, int imm6), "prfh <prfop>, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(prfh(int prfop, p3, x0, x0), "prfh <prfop>, <Pg>, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(prfh(int prfop, p4, x20, z0.VnS()), "prfh <prfop>, <Pg>, [<Xn|SP>, <Zm>.S, <mod> #1]");
  COMPARE_PREFIX(prfw(int prfop, p3, z23.VnS(), int imm5), "prfw <prfop>, <Pg>, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(prfw(int prfop, p1, x4, int imm6), "prfw <prfop>, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(prfw(int prfop, p2, x22, x22), "prfw <prfop>, <Pg>, [<Xn|SP>, <Xm>, LSL #2]");
  COMPARE_PREFIX(prfw(int prfop, p1, x2, z6.VnS()), "prfw <prfop>, <Pg>, [<Xn|SP>, <Zm>.S, <mod> #2]");
#endif

  CLEANUP();
}

TEST(sve_mem_64bit_gather) {
  SETUP();

#if 0
  COMPARE_PREFIX(ld1b(z2.VnD(), p2.Zeroing(), z12.VnD(), int imm5), "ld1b { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ld1b(z30.VnD(), p6.Zeroing(), x10, z24.VnD()), "ld1b { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ld1b(z19.VnD(), p5.Zeroing(), x21, z29.VnD()), "ld1b { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ld1d(z13.VnD(), p3.Zeroing(), z19.VnD(), int imm5), "ld1d { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ld1d(z20.VnD(), p3.Zeroing(), x3, z15.VnD()), "ld1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #3]");
  COMPARE_PREFIX(ld1d(z18.VnD(), p5.Zeroing(), x11, z11.VnD()), "ld1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ld1d(z25.VnD(), p3.Zeroing(), x14, z0.VnD()), "ld1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #3]");
  COMPARE_PREFIX(ld1d(z9.VnD(), p5.Zeroing(), x5, z21.VnD()), "ld1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ld1h(z30.VnD(), p7.Zeroing(), z28.VnD(), int imm5), "ld1h { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ld1h(z24.VnD(), p4.Zeroing(), x6, z11.VnD()), "ld1h { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #1]");
  COMPARE_PREFIX(ld1h(z2.VnD(), p3.Zeroing(), x16, z18.VnD()), "ld1h { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ld1h(z21.VnD(), p5.Zeroing(), x13, z8.VnD()), "ld1h { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #1]");
  COMPARE_PREFIX(ld1h(z26.VnD(), p3.Zeroing(), x1, z10.VnD()), "ld1h { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ld1sb(z16.VnD(), p7.Zeroing(), z31.VnD(), int imm5), "ld1sb { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ld1sb(z11.VnD(), p3.Zeroing(), x24, z21.VnD()), "ld1sb { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ld1sb(z4.VnD(), p1.Zeroing(), x24, z15.VnD()), "ld1sb { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ld1sh(z20.VnD(), p2.Zeroing(), z2.VnD(), int imm5), "ld1sh { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ld1sh(z22.VnD(), p6.Zeroing(), x7, z31.VnD()), "ld1sh { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #1]");
  COMPARE_PREFIX(ld1sh(z7.VnD(), p7.Zeroing(), x28, z23.VnD()), "ld1sh { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ld1sh(z29.VnD(), p0.Zeroing(), x9, z10.VnD()), "ld1sh { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #1]");
  COMPARE_PREFIX(ld1sh(z9.VnD(), p1.Zeroing(), x0, z12.VnD()), "ld1sh { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ld1sw(z2.VnD(), p7.Zeroing(), z25.VnD(), int imm5), "ld1sw { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ld1sw(z9.VnD(), p0.Zeroing(), x2, z27.VnD()), "ld1sw { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #2]");
  COMPARE_PREFIX(ld1sw(z29.VnD(), p7.Zeroing(), x27, z4.VnD()), "ld1sw { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ld1sw(z5.VnD(), p2.Zeroing(), x1, z23.VnD()), "ld1sw { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #2]");
  COMPARE_PREFIX(ld1sw(z19.VnD(), p2.Zeroing(), x19, z16.VnD()), "ld1sw { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ld1w(z10.VnD(), p5.Zeroing(), z4.VnD(), int imm5), "ld1w { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ld1w(z9.VnD(), p2.Zeroing(), x0, z0.VnD()), "ld1w { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #2]");
  COMPARE_PREFIX(ld1w(z19.VnD(), p1.Zeroing(), x27, z4.VnD()), "ld1w { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ld1w(z21.VnD(), p1.Zeroing(), x7, z8.VnD()), "ld1w { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #2]");
  COMPARE_PREFIX(ld1w(z13.VnD(), p3.Zeroing(), x8, z10.VnD()), "ld1w { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ldff1b(z24.VnD(), p5.Zeroing(), z8.VnD(), int imm5), "ldff1b { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ldff1b(z20.VnD(), p3.Zeroing(), x26, z11.VnD()), "ldff1b { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ldff1b(z8.VnD(), p6.Zeroing(), x18, z29.VnD()), "ldff1b { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ldff1d(z19.VnD(), p1.Zeroing(), z14.VnD(), int imm5), "ldff1d { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ldff1d(z7.VnD(), p7.Zeroing(), x24, z10.VnD()), "ldff1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #3]");
  COMPARE_PREFIX(ldff1d(z11.VnD(), p5.Zeroing(), x10, z21.VnD()), "ldff1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ldff1d(z25.VnD(), p0.Zeroing(), x21, z15.VnD()), "ldff1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #3]");
  COMPARE_PREFIX(ldff1d(z15.VnD(), p7.Zeroing(), x26, z9.VnD()), "ldff1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ldff1h(z9.VnD(), p3.Zeroing(), z19.VnD(), int imm5), "ldff1h { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ldff1h(z13.VnD(), p7.Zeroing(), x17, z17.VnD()), "ldff1h { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #1]");
  COMPARE_PREFIX(ldff1h(z26.VnD(), p7.Zeroing(), x26, z31.VnD()), "ldff1h { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ldff1h(z19.VnD(), p6.Zeroing(), x16, z12.VnD()), "ldff1h { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #1]");
  COMPARE_PREFIX(ldff1h(z16.VnD(), p1.Zeroing(), x20, z10.VnD()), "ldff1h { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ldff1sb(z26.VnD(), p5.Zeroing(), z14.VnD(), int imm5), "ldff1sb { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ldff1sb(z3.VnD(), p5.Zeroing(), x18, z3.VnD()), "ldff1sb { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ldff1sb(z31.VnD(), p3.Zeroing(), x13, z0.VnD()), "ldff1sb { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ldff1sh(z6.VnD(), p3.Zeroing(), z19.VnD(), int imm5), "ldff1sh { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ldff1sh(z2.VnD(), p1.Zeroing(), x8, z8.VnD()), "ldff1sh { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #1]");
  COMPARE_PREFIX(ldff1sh(z4.VnD(), p4.Zeroing(), x17, z18.VnD()), "ldff1sh { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ldff1sh(z3.VnD(), p0.Zeroing(), x13, z22.VnD()), "ldff1sh { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #1]");
  COMPARE_PREFIX(ldff1sh(z7.VnD(), p3.Zeroing(), x16, z20.VnD()), "ldff1sh { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ldff1sw(z19.VnD(), p7.Zeroing(), z14.VnD(), int imm5), "ldff1sw { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ldff1sw(z29.VnD(), p0.Zeroing(), x5, z1.VnD()), "ldff1sw { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #2]");
  COMPARE_PREFIX(ldff1sw(z26.VnD(), p3.Zeroing(), x0, z31.VnD()), "ldff1sw { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ldff1sw(z25.VnD(), p5.Zeroing(), x4, z17.VnD()), "ldff1sw { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #2]");
  COMPARE_PREFIX(ldff1sw(z7.VnD(), p6.Zeroing(), x12, z16.VnD()), "ldff1sw { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(ldff1w(z26.VnD(), p6.Zeroing(), z15.VnD(), int imm5), "ldff1w { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(ldff1w(z28.VnD(), p5.Zeroing(), x25, z28.VnD()), "ldff1w { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #2]");
  COMPARE_PREFIX(ldff1w(z20.VnD(), p4.Zeroing(), x30, z7.VnD()), "ldff1w { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(ldff1w(z10.VnD(), p2.Zeroing(), x23, z25.VnD()), "ldff1w { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #2]");
  COMPARE_PREFIX(ldff1w(z4.VnD(), p1.Zeroing(), x8, z1.VnD()), "ldff1w { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(prfb(int prfop, p7, z1.VnD(), int imm5), "prfb <prfop>, <Pg>, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(prfb(int prfop, p1, x15, z11.VnD()), "prfb <prfop>, <Pg>, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(prfb(int prfop, p5, x12, z18.VnD()), "prfb <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(prfd(int prfop, p1, z3.VnD(), int imm5), "prfd <prfop>, <Pg>, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(prfd(int prfop, p6, x4, z12.VnD()), "prfd <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, LSL #3]");
  COMPARE_PREFIX(prfd(int prfop, p6, x27, z29.VnD()), "prfd <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, <mod> #3]");
  COMPARE_PREFIX(prfh(int prfop, p6, z8.VnD(), int imm5), "prfh <prfop>, <Pg>, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(prfh(int prfop, p5, x11, z26.VnD()), "prfh <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, LSL #1]");
  COMPARE_PREFIX(prfh(int prfop, p2, x30, z7.VnD()), "prfh <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, <mod> #1]");
  COMPARE_PREFIX(prfw(int prfop, p7, z14.VnD(), int imm5), "prfw <prfop>, <Pg>, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(prfw(int prfop, p6, x24, z4.VnD()), "prfw <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, LSL #2]");
  COMPARE_PREFIX(prfw(int prfop, p1, x23, z0.VnD()), "prfw <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, <mod> #2]");
#endif

  CLEANUP();
}

TEST(sve_mem_contiguous_load) {
  SETUP();

#if 0
  COMPARE_PREFIX(ld1b(z0.VnH(), p4.Zeroing(), x13, int imm4), "ld1b { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1b(z6.VnS(), p2.Zeroing(), x6, int imm4), "ld1b { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1b(z24.VnD(), p1.Zeroing(), x8, int imm4), "ld1b { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1b(z14.VnB(), p0.Zeroing(), x0, int imm4), "ld1b { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1b(z18.VnH(), p1.Zeroing(), x20, x20), "ld1b { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(ld1b(z4.VnS(), p4.Zeroing(), x4, x4), "ld1b { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(ld1b(z18.VnD(), p5.Zeroing(), x12, x12), "ld1b { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(ld1b(z25.VnB(), p5.Zeroing(), x20, x20), "ld1b { <Zt>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(ld1d(z13.VnD(), p4.Zeroing(), x10, int imm4), "ld1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1d(z9.VnD(), p0.Zeroing(), x23, x23), "ld1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]");
  COMPARE_PREFIX(ld1h(z6.VnH(), p6.Zeroing(), x22, int imm4), "ld1h { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1h(z8.VnS(), p6.Zeroing(), x22, int imm4), "ld1h { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1h(z6.VnD(), p1.Zeroing(), x25, int imm4), "ld1h { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1h(z31.VnH(), p6.Zeroing(), x21, x21), "ld1h { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(ld1h(z31.VnS(), p5.Zeroing(), x12, x12), "ld1h { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(ld1h(z12.VnD(), p7.Zeroing(), x1, x1), "ld1h { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(ld1rqb(z18.VnB(), p2.Zeroing(), x18, int imm4), "ld1rqb { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rqb(z3.VnB(), p2.Zeroing(), x22, x22), "ld1rqb { <Zt>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(ld1rqd(z11.VnD(), p1.Zeroing(), x23, int imm4), "ld1rqd { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rqd(z6.VnD(), p0.Zeroing(), x18, x18), "ld1rqd { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]");
  COMPARE_PREFIX(ld1rqh(z11.VnH(), p1.Zeroing(), x0, int imm4), "ld1rqh { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rqh(z1.VnH(), p7.Zeroing(), x9, x9), "ld1rqh { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(ld1rqw(z22.VnS(), p3.Zeroing(), x31, int imm4), "ld1rqw { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]");
  COMPARE_PREFIX(ld1rqw(z12.VnS(), p4.Zeroing(), x6, x6), "ld1rqw { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]");
  COMPARE_PREFIX(ld1sb(z31.VnH(), p4.Zeroing(), x9, int imm4), "ld1sb { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1sb(z23.VnS(), p2.Zeroing(), x2, int imm4), "ld1sb { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1sb(z20.VnD(), p3.Zeroing(), x27, int imm4), "ld1sb { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1sb(z27.VnH(), p3.Zeroing(), x21, x21), "ld1sb { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(ld1sb(z8.VnS(), p3.Zeroing(), x29, x29), "ld1sb { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(ld1sb(z22.VnD(), p5.Zeroing(), x26, x26), "ld1sb { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(ld1sh(z20.VnS(), p2.Zeroing(), x11, int imm4), "ld1sh { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1sh(z31.VnD(), p1.Zeroing(), x12, int imm4), "ld1sh { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1sh(z20.VnS(), p7.Zeroing(), x19, x19), "ld1sh { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(ld1sh(z17.VnD(), p3.Zeroing(), x4, x4), "ld1sh { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(ld1sw(z19.VnD(), p3.Zeroing(), x27, int imm4), "ld1sw { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1sw(z22.VnD(), p5.Zeroing(), x12, x12), "ld1sw { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]");
  COMPARE_PREFIX(ld1w(z1.VnS(), p6.Zeroing(), x13, int imm4), "ld1w { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1w(z22.VnD(), p7.Zeroing(), x19, int imm4), "ld1w { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld1w(z10.VnS(), p4.Zeroing(), x18, x18), "ld1w { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]");
  COMPARE_PREFIX(ld1w(z18.VnD(), p6.Zeroing(), x6, x6), "ld1w { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]");
  COMPARE_PREFIX(ld2b(z7.VnB(), z8.VnB(), p4.Zeroing(), x7, int imm4), "ld2b { <Zt1>.B, <Zt2>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld2b(z17.VnB(), z18.VnB(), p4.Zeroing(), x17, x17), "ld2b { <Zt1>.B, <Zt2>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(ld2d(z14.VnD(), z15.VnD(), p2.Zeroing(), x14, int imm4), "ld2d { <Zt1>.D, <Zt2>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld2d(z18.VnD(), z19.VnD(), p6.Zeroing(), x18, x18), "ld2d { <Zt1>.D, <Zt2>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]");
  COMPARE_PREFIX(ld2h(z11.VnH(), z12.VnH(), p7.Zeroing(), x11, int imm4), "ld2h { <Zt1>.H, <Zt2>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld2h(z22.VnH(), z23.VnH(), p7.Zeroing(), x22, x22), "ld2h { <Zt1>.H, <Zt2>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(ld2w(z8.VnS(), z9.VnS(), p2.Zeroing(), x8, int imm4), "ld2w { <Zt1>.S, <Zt2>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld2w(z7.VnS(), z8.VnS(), p6.Zeroing(), x7, x7), "ld2w { <Zt1>.S, <Zt2>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]");
  COMPARE_PREFIX(ld3b(z28.VnB(), z29.VnB(), z30.VnB(), p1.Zeroing(), x28, int imm4), "ld3b { <Zt1>.B, <Zt2>.B, <Zt3>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld3b(z10.VnB(), z11.VnB(), z12.VnB(), p6.Zeroing(), x10, x10), "ld3b { <Zt1>.B, <Zt2>.B, <Zt3>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(ld3d(z27.VnD(), z28.VnD(), z29.VnD(), p4.Zeroing(), x27, int imm4), "ld3d { <Zt1>.D, <Zt2>.D, <Zt3>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld3d(z10.VnD(), z11.VnD(), z12.VnD(), p1.Zeroing(), x10, x10), "ld3d { <Zt1>.D, <Zt2>.D, <Zt3>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]");
  COMPARE_PREFIX(ld3h(z25.VnH(), z26.VnH(), z27.VnH(), p0.Zeroing(), x25, int imm4), "ld3h { <Zt1>.H, <Zt2>.H, <Zt3>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld3h(z20.VnH(), z21.VnH(), z22.VnH(), p2.Zeroing(), x20, x20), "ld3h { <Zt1>.H, <Zt2>.H, <Zt3>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(ld3w(z26.VnS(), z27.VnS(), z28.VnS(), p6.Zeroing(), x26, int imm4), "ld3w { <Zt1>.S, <Zt2>.S, <Zt3>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld3w(z5.VnS(), z6.VnS(), z7.VnS(), p0.Zeroing(), x5, x5), "ld3w { <Zt1>.S, <Zt2>.S, <Zt3>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]");
  COMPARE_PREFIX(ld4b(z0.VnB(), z1.VnB(), z2.VnB(), z3.VnB(), p2.Zeroing(), x0, int imm4), "ld4b { <Zt1>.B, <Zt2>.B, <Zt3>.B, <Zt4>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld4b(z20.VnB(), z21.VnB(), z22.VnB(), z23.VnB(), p0.Zeroing(), x20, x20), "ld4b { <Zt1>.B, <Zt2>.B, <Zt3>.B, <Zt4>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(ld4d(z7.VnD(), z8.VnD(), z9.VnD(), z10.VnD(), p4.Zeroing(), x7, int imm4), "ld4d { <Zt1>.D, <Zt2>.D, <Zt3>.D, <Zt4>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld4d(z10.VnD(), z11.VnD(), z12.VnD(), z13.VnD(), p2.Zeroing(), x10, x10), "ld4d { <Zt1>.D, <Zt2>.D, <Zt3>.D, <Zt4>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]");
  COMPARE_PREFIX(ld4h(z31.VnH(), z0.VnH(), z1.VnH(), z2.VnH(), p2.Zeroing(), x31, int imm4), "ld4h { <Zt1>.H, <Zt2>.H, <Zt3>.H, <Zt4>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld4h(z20.VnH(), z21.VnH(), z22.VnH(), z23.VnH(), p3.Zeroing(), x20, x20), "ld4h { <Zt1>.H, <Zt2>.H, <Zt3>.H, <Zt4>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(ld4w(z19.VnS(), z20.VnS(), z21.VnS(), z22.VnS(), p2.Zeroing(), x19, int imm4), "ld4w { <Zt1>.S, <Zt2>.S, <Zt3>.S, <Zt4>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ld4w(z27.VnS(), z28.VnS(), z29.VnS(), z30.VnS(), p3.Zeroing(), x27, x27), "ld4w { <Zt1>.S, <Zt2>.S, <Zt3>.S, <Zt4>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]");
  COMPARE_PREFIX(ldff1b(z2.VnH(), p1.Zeroing(), x23, x23), "ldff1b { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, <Xm>}]");
  COMPARE_PREFIX(ldff1b(z21.VnS(), p0.Zeroing(), x19, x19), "ldff1b { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>}]");
  COMPARE_PREFIX(ldff1b(z16.VnD(), p2.Zeroing(), x16, x16), "ldff1b { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>}]");
  COMPARE_PREFIX(ldff1b(z24.VnB(), p7.Zeroing(), x15, x15), "ldff1b { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, <Xm>}]");
  COMPARE_PREFIX(ldff1d(z19.VnD(), p4.Zeroing(), x8, x8), "ldff1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #3}]");
  COMPARE_PREFIX(ldff1h(z18.VnH(), p6.Zeroing(), x10, x10), "ldff1h { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]");
  COMPARE_PREFIX(ldff1h(z31.VnS(), p4.Zeroing(), x1, x1), "ldff1h { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]");
  COMPARE_PREFIX(ldff1h(z1.VnD(), p0.Zeroing(), x8, x8), "ldff1h { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]");
  COMPARE_PREFIX(ldff1sb(z12.VnH(), p2.Zeroing(), x10, x10), "ldff1sb { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, <Xm>}]");
  COMPARE_PREFIX(ldff1sb(z14.VnS(), p1.Zeroing(), x0, x0), "ldff1sb { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>}]");
  COMPARE_PREFIX(ldff1sb(z29.VnD(), p4.Zeroing(), x24, x24), "ldff1sb { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>}]");
  COMPARE_PREFIX(ldff1sh(z0.VnS(), p4.Zeroing(), x30, x30), "ldff1sh { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]");
  COMPARE_PREFIX(ldff1sh(z11.VnD(), p4.Zeroing(), x29, x29), "ldff1sh { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]");
  COMPARE_PREFIX(ldff1sw(z26.VnD(), p6.Zeroing(), x19, x19), "ldff1sw { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #2}]");
  COMPARE_PREFIX(ldff1w(z15.VnS(), p1.Zeroing(), x15, x15), "ldff1w { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #2}]");
  COMPARE_PREFIX(ldff1w(z12.VnD(), p7.Zeroing(), x22, x22), "ldff1w { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #2}]");
  COMPARE_PREFIX(ldnf1b(z1.VnH(), p0.Zeroing(), x25, int imm4), "ldnf1b { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1b(z0.VnS(), p0.Zeroing(), x2, int imm4), "ldnf1b { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1b(z31.VnD(), p6.Zeroing(), x0, int imm4), "ldnf1b { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1b(z25.VnB(), p1.Zeroing(), x5, int imm4), "ldnf1b { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1d(z25.VnD(), p0.Zeroing(), x11, int imm4), "ldnf1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1h(z22.VnH(), p4.Zeroing(), x7, int imm4), "ldnf1h { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1h(z7.VnS(), p2.Zeroing(), x1, int imm4), "ldnf1h { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1h(z5.VnD(), p3.Zeroing(), x29, int imm4), "ldnf1h { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1sb(z12.VnH(), p5.Zeroing(), x27, int imm4), "ldnf1sb { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1sb(z10.VnS(), p2.Zeroing(), x13, int imm4), "ldnf1sb { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1sb(z25.VnD(), p6.Zeroing(), x26, int imm4), "ldnf1sb { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1sh(z3.VnS(), p5.Zeroing(), x1, int imm4), "ldnf1sh { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1sh(z8.VnD(), p6.Zeroing(), x13, int imm4), "ldnf1sh { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1sw(z5.VnD(), p6.Zeroing(), x2, int imm4), "ldnf1sw { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1w(z11.VnS(), p3.Zeroing(), x26, int imm4), "ldnf1w { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnf1w(z10.VnD(), p6.Zeroing(), x12, int imm4), "ldnf1w { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnt1b(z2.VnB(), p2.Zeroing(), x13, int imm4), "ldnt1b { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnt1b(z21.VnB(), p5.Zeroing(), x1, x1), "ldnt1b { <Zt>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(ldnt1d(z2.VnD(), p7.Zeroing(), x13, int imm4), "ldnt1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnt1d(z10.VnD(), p0.Zeroing(), x23, x23), "ldnt1d { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]");
  COMPARE_PREFIX(ldnt1h(z26.VnH(), p4.Zeroing(), x16, int imm4), "ldnt1h { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnt1h(z30.VnH(), p4.Zeroing(), x6, x6), "ldnt1h { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(ldnt1w(z17.VnS(), p4.Zeroing(), x15, int imm4), "ldnt1w { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(ldnt1w(z0.VnS(), p4.Zeroing(), x11, x11), "ldnt1w { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]");
#endif

  CLEANUP();
}

TEST(sve_ldr_str_simple) {
  SETUP();

  COMPARE_PREFIX(str(p14, SVEMemOperand(x0)), "str p14, [x0]");
  COMPARE_PREFIX(str(z14, SVEMemOperand(sp)), "str z14, [sp]");
  COMPARE_PREFIX(ldr(p4, SVEMemOperand(x0)), "ldr p4, [x0]");
  COMPARE_PREFIX(ldr(z4, SVEMemOperand(sp)), "ldr z4, [sp]");
  COMPARE_PREFIX(str(p15, SVEMemOperand(sp, -256, SVE_MUL_VL)),
                 "str p15, [sp, #-256, MUL VL]");
  COMPARE_PREFIX(str(z16, SVEMemOperand(x13, 255, SVE_MUL_VL)),
                 "str z16, [x13, #255, MUL VL]");
  COMPARE_PREFIX(ldr(p5, SVEMemOperand(sp, -42, SVE_MUL_VL)),
                 "ldr p5, [sp, #-42, MUL VL]");
  COMPARE_PREFIX(ldr(z6, SVEMemOperand(x28, 42, SVE_MUL_VL)),
                 "ldr z6, [x28, #42, MUL VL]");

  COMPARE_MACRO(Str(p14, SVEMemOperand(x0)), "str p14, [x0]");
  COMPARE_MACRO(Str(z14, SVEMemOperand(sp)), "str z14, [sp]");
  COMPARE_MACRO(Ldr(p4, SVEMemOperand(x0)), "ldr p4, [x0]");
  COMPARE_MACRO(Ldr(z4, SVEMemOperand(sp)), "ldr z4, [sp]");
  COMPARE_MACRO(Str(p15, SVEMemOperand(sp, -256, SVE_MUL_VL)),
                "str p15, [sp, #-256, MUL VL]");
  COMPARE_MACRO(Str(z16, SVEMemOperand(x13, 255, SVE_MUL_VL)),
                "str z16, [x13, #255, MUL VL]");
  COMPARE_MACRO(Ldr(p5, SVEMemOperand(sp, -42, SVE_MUL_VL)),
                "ldr p5, [sp, #-42, MUL VL]");
  COMPARE_MACRO(Ldr(z6, SVEMemOperand(x28, 42, SVE_MUL_VL)),
                "ldr z6, [x28, #42, MUL VL]");

  COMPARE_MACRO(Ldr(z6, SVEMemOperand(x28, 42, SVE_MUL_VL)),
                "ldr z6, [x28, #42, MUL VL]");

  // IsEquivalentToScalar
  COMPARE_MACRO(Str(p0, SVEMemOperand(x0, xzr)), "str p0, [x0]");
  COMPARE_MACRO(Ldr(p1, SVEMemOperand(sp, xzr)), "ldr p1, [sp]");
  COMPARE_MACRO(Str(z2, SVEMemOperand(x12, xzr)), "str z2, [x12]");
  COMPARE_MACRO(Ldr(z3, SVEMemOperand(x7, xzr)), "ldr z3, [x7]");

  // Other cases fall back on Adr. We test Adr separately, so here we just test
  // sequences that stress scratch register allocation.
  COMPARE_MACRO(Str(p4, SVEMemOperand(x5, 4242, SVE_MUL_VL)),
                "mov x16, #0x1092\n"
                "rdvl x17, #1\n"
                "mul x16, x16, x17\n"
                "add x16, x5, x16, asr #3\n"
                "str p4, [x16]");
  COMPARE_MACRO(Ldr(p6, SVEMemOperand(sp, 4242, SVE_MUL_VL)),
                "mov x16, #0x1092\n"
                "rdvl x17, #1\n"
                "mul x16, x16, x17\n"
                "asr x16, x16, #3\n"
                "add x16, sp, x16\n"
                "ldr p6, [x16]");
  COMPARE_MACRO(Str(z7, SVEMemOperand(sp, 4242, SVE_MUL_VL)),
                "mov x16, #0x1092\n"
                "rdvl x17, #1\n"
                "mul x16, x16, x17\n"
                "add x16, sp, x16\n"
                "str z7, [x16]");
  COMPARE_MACRO(Ldr(z8, SVEMemOperand(x9, 4242, SVE_MUL_VL)),
                "mov x16, #0x1092\n"
                "rdvl x17, #1\n"
                "madd x16, x16, x17, x9\n"
                "ldr z8, [x16]");

  CLEANUP();
}

TEST(sve_ld1_st1) {
  SETUP();

  COMPARE_PREFIX(st1b(z11.VnB(), p0, SVEMemOperand(x22)),
                 "st1b { z11.b }, p0, [x22]");
  COMPARE_PREFIX(st1b(z15.VnH(), p1, SVEMemOperand(x15, 7, SVE_MUL_VL)),
                 "st1b { z15.h }, p1, [x15, #7, MUL VL]");
  COMPARE_PREFIX(st1b(z19.VnS(), p2, SVEMemOperand(sp, -8, SVE_MUL_VL)),
                 "st1b { z19.s }, p2, [sp, #-8, MUL VL]");
  COMPARE_PREFIX(st1b(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
                 "st1b { z23.d }, p3, [x1]");
  COMPARE_PREFIX(st1b(z2.VnB(), p4, SVEMemOperand(x1, x2)),
                 "st1b { z2.b }, p4, [x1, x2]");
  COMPARE_PREFIX(st1b(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 0)),
                 "st1b { z31.d }, p7, [x9, x9]");

  COMPARE_PREFIX(st1h(z15.VnH(), p1, SVEMemOperand(x15, 7, SVE_MUL_VL)),
                 "st1h { z15.h }, p1, [x15, #7, MUL VL]");
  COMPARE_PREFIX(st1h(z19.VnS(), p2, SVEMemOperand(sp, -8, SVE_MUL_VL)),
                 "st1h { z19.s }, p2, [sp, #-8, MUL VL]");
  COMPARE_PREFIX(st1h(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
                 "st1h { z23.d }, p3, [x1]");
  COMPARE_PREFIX(st1h(z2.VnH(), p4, SVEMemOperand(x1, x2, LSL, 1)),
                 "st1h { z2.h }, p4, [x1, x2, LSL #1]");
  COMPARE_PREFIX(st1h(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 1)),
                 "st1h { z31.d }, p7, [x9, x9, LSL #1]");

  COMPARE_PREFIX(st1w(z19.VnS(), p2, SVEMemOperand(sp, -8, SVE_MUL_VL)),
                 "st1w { z19.s }, p2, [sp, #-8, MUL VL]");
  COMPARE_PREFIX(st1w(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
                 "st1w { z23.d }, p3, [x1]");
  COMPARE_PREFIX(st1w(z2.VnS(), p4, SVEMemOperand(x1, x2, LSL, 2)),
                 "st1w { z2.s }, p4, [x1, x2, LSL #2]");
  COMPARE_PREFIX(st1w(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 2)),
                 "st1w { z31.d }, p7, [x9, x9, LSL #2]");

  COMPARE_PREFIX(st1d(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
                 "st1d { z23.d }, p3, [x1]");
  COMPARE_PREFIX(st1d(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 3)),
                 "st1d { z31.d }, p7, [x9, x9, LSL #3]");

  COMPARE_PREFIX(ld1b(z11.VnB(), p0.Zeroing(), SVEMemOperand(x22)),
                 "ld1b { z11.b }, p0/z, [x22]");
  COMPARE_PREFIX(ld1b(z15.VnH(),
                      p1.Zeroing(),
                      SVEMemOperand(x15, 7, SVE_MUL_VL)),
                 "ld1b { z15.h }, p1/z, [x15, #7, MUL VL]");
  COMPARE_PREFIX(ld1b(z19.VnS(),
                      p2.Zeroing(),
                      SVEMemOperand(sp, -8, SVE_MUL_VL)),
                 "ld1b { z19.s }, p2/z, [sp, #-8, MUL VL]");
  COMPARE_PREFIX(ld1b(z23.VnD(),
                      p3.Zeroing(),
                      SVEMemOperand(x1, 0, SVE_MUL_VL)),
                 "ld1b { z23.d }, p3/z, [x1]");
  COMPARE_PREFIX(ld1b(z2.VnB(), p4.Zeroing(), SVEMemOperand(x1, x2)),
                 "ld1b { z2.b }, p4/z, [x1, x2]");
  COMPARE_PREFIX(ld1b(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 0)),
                 "ld1b { z31.d }, p7/z, [x9, x9]");

  COMPARE_PREFIX(ld1h(z15.VnH(),
                      p1.Zeroing(),
                      SVEMemOperand(x15, 7, SVE_MUL_VL)),
                 "ld1h { z15.h }, p1/z, [x15, #7, MUL VL]");
  COMPARE_PREFIX(ld1h(z19.VnS(),
                      p2.Zeroing(),
                      SVEMemOperand(sp, -8, SVE_MUL_VL)),
                 "ld1h { z19.s }, p2/z, [sp, #-8, MUL VL]");
  COMPARE_PREFIX(ld1h(z23.VnD(),
                      p3.Zeroing(),
                      SVEMemOperand(x1, 0, SVE_MUL_VL)),
                 "ld1h { z23.d }, p3/z, [x1]");
  COMPARE_PREFIX(ld1h(z2.VnH(), p4.Zeroing(), SVEMemOperand(x1, x2, LSL, 1)),
                 "ld1h { z2.h }, p4/z, [x1, x2, LSL #1]");
  COMPARE_PREFIX(ld1h(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 1)),
                 "ld1h { z31.d }, p7/z, [x9, x9, LSL #1]");

  COMPARE_PREFIX(ld1w(z19.VnS(),
                      p2.Zeroing(),
                      SVEMemOperand(sp, -8, SVE_MUL_VL)),
                 "ld1w { z19.s }, p2/z, [sp, #-8, MUL VL]");
  COMPARE_PREFIX(ld1w(z23.VnD(),
                      p3.Zeroing(),
                      SVEMemOperand(x1, 0, SVE_MUL_VL)),
                 "ld1w { z23.d }, p3/z, [x1]");
  COMPARE_PREFIX(ld1w(z2.VnS(), p4.Zeroing(), SVEMemOperand(x1, x2, LSL, 2)),
                 "ld1w { z2.s }, p4/z, [x1, x2, LSL #2]");
  COMPARE_PREFIX(ld1w(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 2)),
                 "ld1w { z31.d }, p7/z, [x9, x9, LSL #2]");

  COMPARE_PREFIX(ld1d(z23.VnD(),
                      p3.Zeroing(),
                      SVEMemOperand(x1, 0, SVE_MUL_VL)),
                 "ld1d { z23.d }, p3/z, [x1]");
  COMPARE_PREFIX(ld1d(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 3)),
                 "ld1d { z31.d }, p7/z, [x9, x9, LSL #3]");

  COMPARE_PREFIX(ld1sb(z15.VnH(),
                       p1.Zeroing(),
                       SVEMemOperand(x15, 7, SVE_MUL_VL)),
                 "ld1sb { z15.h }, p1/z, [x15, #7, MUL VL]");
  COMPARE_PREFIX(ld1sb(z19.VnS(),
                       p2.Zeroing(),
                       SVEMemOperand(sp, -8, SVE_MUL_VL)),
                 "ld1sb { z19.s }, p2/z, [sp, #-8, MUL VL]");
  COMPARE_PREFIX(ld1d(z23.VnD(),
                      p3.Zeroing(),
                      SVEMemOperand(x1, 0, SVE_MUL_VL)),
                 "ld1d { z23.d }, p3/z, [x1]");
  COMPARE_PREFIX(ld1sb(z5.VnH(), p1.Zeroing(), SVEMemOperand(x15, x1, LSL, 0)),
                 "ld1sb { z5.h }, p1/z, [x15, x1]");
  COMPARE_PREFIX(ld1sb(z9.VnS(), p2.Zeroing(), SVEMemOperand(x29, x3, LSL, 0)),
                 "ld1sb { z9.s }, p2/z, [x29, x3]");
  COMPARE_PREFIX(ld1sb(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 0)),
                 "ld1sb { z31.d }, p7/z, [x9, x9]");

  COMPARE_PREFIX(ld1sh(z19.VnS(),
                       p2.Zeroing(),
                       SVEMemOperand(sp, -8, SVE_MUL_VL)),
                 "ld1sh { z19.s }, p2/z, [sp, #-8, MUL VL]");
  COMPARE_PREFIX(ld1sh(z23.VnD(),
                       p3.Zeroing(),
                       SVEMemOperand(x1, 0, SVE_MUL_VL)),
                 "ld1sh { z23.d }, p3/z, [x1]");
  COMPARE_PREFIX(ld1sh(z11.VnS(),
                       p4.Zeroing(),
                       SVEMemOperand(x22, x10, LSL, 1)),
                 "ld1sh { z11.s }, p4/z, [x22, x10, LSL #1]");
  COMPARE_PREFIX(ld1sh(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 1)),
                 "ld1sh { z31.d }, p7/z, [x9, x9, LSL #1]");

  COMPARE_PREFIX(ld1sw(z23.VnD(),
                       p3.Zeroing(),
                       SVEMemOperand(x1, 0, SVE_MUL_VL)),
                 "ld1sw { z23.d }, p3/z, [x1]");
  COMPARE_PREFIX(ld1sw(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 2)),
                 "ld1sw { z31.d }, p7/z, [x9, x9, LSL #2]");

  CLEANUP();
}

TEST(sve_ld1_st1_macro) {
  SETUP();

  // Pass-through cases.
  COMPARE_MACRO(St1b(z11.VnB(), p0, SVEMemOperand(x22)),
                "st1b { z11.b }, p0, [x22]");
  COMPARE_MACRO(St1b(z15.VnH(), p1, SVEMemOperand(x15, 7, SVE_MUL_VL)),
                "st1b { z15.h }, p1, [x15, #7, MUL VL]");
  COMPARE_MACRO(St1b(z19.VnS(), p2, SVEMemOperand(sp, -8, SVE_MUL_VL)),
                "st1b { z19.s }, p2, [sp, #-8, MUL VL]");
  COMPARE_MACRO(St1b(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "st1b { z23.d }, p3, [x1]");
  COMPARE_MACRO(St1b(z2.VnB(), p4, SVEMemOperand(x1, x2)),
                "st1b { z2.b }, p4, [x1, x2]");
  COMPARE_MACRO(St1b(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 0)),
                "st1b { z31.d }, p7, [x9, x9]");

  COMPARE_MACRO(St1h(z15.VnH(), p1, SVEMemOperand(x15, 7, SVE_MUL_VL)),
                "st1h { z15.h }, p1, [x15, #7, MUL VL]");
  COMPARE_MACRO(St1h(z19.VnS(), p2, SVEMemOperand(sp, -8, SVE_MUL_VL)),
                "st1h { z19.s }, p2, [sp, #-8, MUL VL]");
  COMPARE_MACRO(St1h(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "st1h { z23.d }, p3, [x1]");
  COMPARE_MACRO(St1h(z2.VnH(), p4, SVEMemOperand(x1, x2, LSL, 1)),
                "st1h { z2.h }, p4, [x1, x2, LSL #1]");
  COMPARE_MACRO(St1h(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 1)),
                "st1h { z31.d }, p7, [x9, x9, LSL #1]");

  COMPARE_MACRO(St1w(z19.VnS(), p2, SVEMemOperand(sp, -8, SVE_MUL_VL)),
                "st1w { z19.s }, p2, [sp, #-8, MUL VL]");
  COMPARE_MACRO(St1w(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "st1w { z23.d }, p3, [x1]");
  COMPARE_MACRO(St1w(z2.VnS(), p4, SVEMemOperand(x1, x2, LSL, 2)),
                "st1w { z2.s }, p4, [x1, x2, LSL #2]");
  COMPARE_MACRO(St1w(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 2)),
                "st1w { z31.d }, p7, [x9, x9, LSL #2]");

  COMPARE_MACRO(St1d(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "st1d { z23.d }, p3, [x1]");
  COMPARE_MACRO(St1d(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 3)),
                "st1d { z31.d }, p7, [x9, x9, LSL #3]");

  // SVEMemOperand synthesis.
  // Most cases are handled by `Adr`, which we test independently, but check
  // that the MacroAssembler falls back on `Adr` at the boundary conditions.
  COMPARE_MACRO(St1b(z10.VnB(), p7, SVEMemOperand(x0, 8, SVE_MUL_VL)),
                "addvl x16, x0, #8\n"
                "st1b { z10.b }, p7, [x16]");
  COMPARE_MACRO(St1h(z11.VnS(), p5, SVEMemOperand(sp, -9, SVE_MUL_VL)),
                "addvl x16, sp, #-9\n"
                "st1h { z11.s }, p5, [x16]");
  COMPARE_MACRO(St1w(z22.VnS(), p3, SVEMemOperand(sp, 42)),
                "add x16, sp, #0x2a (42)\n"
                "st1w { z22.s }, p3, [x16]");
  COMPARE_MACRO(St1d(z22.VnD(), p1, SVEMemOperand(x3, x4)),
                "add x16, x3, x4\n"
                "st1d { z22.d }, p1, [x16]");
  COMPARE_MACRO(St1b(z30.VnD(), p0, SVEMemOperand(x9, xzr)),
                "st1b { z30.d }, p0, [x9]");

  COMPARE_MACRO(Ld1b(z11.VnB(), p0.Zeroing(), SVEMemOperand(x22)),
                "ld1b { z11.b }, p0/z, [x22]");
  COMPARE_MACRO(Ld1b(z15.VnH(),
                     p1.Zeroing(),
                     SVEMemOperand(x15, 7, SVE_MUL_VL)),
                "ld1b { z15.h }, p1/z, [x15, #7, MUL VL]");
  COMPARE_MACRO(Ld1b(z19.VnS(),
                     p2.Zeroing(),
                     SVEMemOperand(sp, -8, SVE_MUL_VL)),
                "ld1b { z19.s }, p2/z, [sp, #-8, MUL VL]");
  COMPARE_MACRO(Ld1b(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "ld1b { z23.d }, p3/z, [x1]");
  COMPARE_MACRO(Ld1b(z2.VnB(), p4.Zeroing(), SVEMemOperand(x1, x2)),
                "ld1b { z2.b }, p4/z, [x1, x2]");
  COMPARE_MACRO(Ld1b(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 0)),
                "ld1b { z31.d }, p7/z, [x9, x9]");

  COMPARE_MACRO(Ld1h(z15.VnH(),
                     p1.Zeroing(),
                     SVEMemOperand(x15, 7, SVE_MUL_VL)),
                "ld1h { z15.h }, p1/z, [x15, #7, MUL VL]");
  COMPARE_MACRO(Ld1h(z19.VnS(),
                     p2.Zeroing(),
                     SVEMemOperand(sp, -8, SVE_MUL_VL)),
                "ld1h { z19.s }, p2/z, [sp, #-8, MUL VL]");
  COMPARE_MACRO(Ld1h(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "ld1h { z23.d }, p3/z, [x1]");
  COMPARE_MACRO(Ld1h(z2.VnH(), p4.Zeroing(), SVEMemOperand(x1, x2, LSL, 1)),
                "ld1h { z2.h }, p4/z, [x1, x2, LSL #1]");
  COMPARE_MACRO(Ld1h(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 1)),
                "ld1h { z31.d }, p7/z, [x9, x9, LSL #1]");

  COMPARE_MACRO(Ld1w(z19.VnS(),
                     p2.Zeroing(),
                     SVEMemOperand(sp, -8, SVE_MUL_VL)),
                "ld1w { z19.s }, p2/z, [sp, #-8, MUL VL]");
  COMPARE_MACRO(Ld1w(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "ld1w { z23.d }, p3/z, [x1]");
  COMPARE_MACRO(Ld1w(z2.VnS(), p4.Zeroing(), SVEMemOperand(x1, x2, LSL, 2)),
                "ld1w { z2.s }, p4/z, [x1, x2, LSL #2]");
  COMPARE_MACRO(Ld1w(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 2)),
                "ld1w { z31.d }, p7/z, [x9, x9, LSL #2]");

  COMPARE_MACRO(Ld1d(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "ld1d { z23.d }, p3/z, [x1]");
  COMPARE_MACRO(Ld1d(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 3)),
                "ld1d { z31.d }, p7/z, [x9, x9, LSL #3]");

  // SVEMemOperand synthesis.
  // Most cases are handled by `Adr`, which we test independently, but check
  // that the MacroAssembler falls back on `Adr` at the boundary conditions.
  COMPARE_MACRO(Ld1b(z10.VnB(), p7.Zeroing(), SVEMemOperand(x0, 8, SVE_MUL_VL)),
                "addvl x16, x0, #8\n"
                "ld1b { z10.b }, p7/z, [x16]");
  COMPARE_MACRO(Ld1h(z11.VnS(),
                     p5.Zeroing(),
                     SVEMemOperand(sp, -9, SVE_MUL_VL)),
                "addvl x16, sp, #-9\n"
                "ld1h { z11.s }, p5/z, [x16]");
  COMPARE_MACRO(Ld1w(z22.VnS(), p3.Zeroing(), SVEMemOperand(sp, 42)),
                "add x16, sp, #0x2a (42)\n"
                "ld1w { z22.s }, p3/z, [x16]");
  COMPARE_MACRO(Ld1d(z22.VnD(), p1.Zeroing(), SVEMemOperand(x3, x4)),
                "add x16, x3, x4\n"
                "ld1d { z22.d }, p1/z, [x16]");
  COMPARE_MACRO(Ld1b(z30.VnD(), p0.Zeroing(), SVEMemOperand(x9, xzr)),
                "ld1b { z30.d }, p0/z, [x9]");
  CLEANUP();
}

TEST(sve_mem_store) {
  SETUP();

#if 0
  COMPARE_PREFIX(st1b(z26.VnD(), p2, z6.VnD(), int imm5), "st1b { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(st1b(z25.VnS(), p1, z23.VnS(), int imm5), "st1b { <Zt>.S }, <Pg>, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(st1b(z31.VnD(), p1, x29, z15.VnD()), "st1b { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(st1b(z19.VnD(), p6, x7, z30.VnD()), "st1b { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(st1b(z3.VnS(), p7, x21, z11.VnS()), "st1b { <Zt>.S }, <Pg>, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(st1d(z19.VnD(), p7, z4.VnD(), int imm5), "st1d { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(st1d(z14.VnD(), p7, x21, int imm4), "st1d { <Zt>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st1d(z27.VnD(), p6, x13, x13), "st1d { <Zt>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]");
  COMPARE_PREFIX(st1d(z24.VnD(), p6, x1, z31.VnD()), "st1d { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, LSL #3]");
  COMPARE_PREFIX(st1d(z20.VnD(), p7, x18, z16.VnD()), "st1d { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(st1d(z8.VnD(), p5, x18, z9.VnD()), "st1d { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod> #3]");
  COMPARE_PREFIX(st1d(z16.VnD(), p6, x21, z13.VnD()), "st1d { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(st1h(z13.VnD(), p2, z11.VnD(), int imm5), "st1h { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(st1h(z4.VnS(), p2, z21.VnS(), int imm5), "st1h { <Zt>.S }, <Pg>, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(st1h(z15.Vn?(), p6, x16, int imm4), "st1h { <Zt>.<T> }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st1h(z15.Vn?(), p5, x15, x15), "st1h { <Zt>.<T> }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(st1h(z10.VnD(), p6, x17, z5.VnD()), "st1h { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, LSL #1]");
  COMPARE_PREFIX(st1h(z16.VnD(), p0, x19, z5.VnD()), "st1h { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(st1h(z11.VnD(), p3, x29, z10.VnD()), "st1h { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod> #1]");
  COMPARE_PREFIX(st1h(z1.VnD(), p5, x7, z6.VnD()), "st1h { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(st1h(z26.VnS(), p1, x3, z24.VnS()), "st1h { <Zt>.S }, <Pg>, [<Xn|SP>, <Zm>.S, <mod> #1]");
  COMPARE_PREFIX(st1h(z24.VnS(), p2, x11, z12.VnS()), "st1h { <Zt>.S }, <Pg>, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(st1w(z23.VnD(), p0, z13.VnD(), int imm5), "st1w { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]");
  COMPARE_PREFIX(st1w(z14.VnS(), p5, z19.VnS(), int imm5), "st1w { <Zt>.S }, <Pg>, [<Zn>.S{, #<imm>}]");
  COMPARE_PREFIX(st1w(z18.Vn?(), p4, x10, int imm4), "st1w { <Zt>.<T> }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st1w(z11.Vn?(), p4, x4, x4), "st1w { <Zt>.<T> }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]");
  COMPARE_PREFIX(st1w(z28.VnD(), p6, x25, z26.VnD()), "st1w { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, LSL #2]");
  COMPARE_PREFIX(st1w(z9.VnD(), p3, x2, z17.VnD()), "st1w { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D]");
  COMPARE_PREFIX(st1w(z7.VnD(), p6, x2, z26.VnD()), "st1w { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod> #2]");
  COMPARE_PREFIX(st1w(z8.VnD(), p5, x13, z3.VnD()), "st1w { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod>]");
  COMPARE_PREFIX(st1w(z5.VnS(), p5, x22, z26.VnS()), "st1w { <Zt>.S }, <Pg>, [<Xn|SP>, <Zm>.S, <mod> #2]");
  COMPARE_PREFIX(st1w(z14.VnS(), p2, x28, z27.VnS()), "st1w { <Zt>.S }, <Pg>, [<Xn|SP>, <Zm>.S, <mod>]");
  COMPARE_PREFIX(st2b(z15.VnB(), z16.VnB(), p6, x15, int imm4), "st2b { <Zt1>.B, <Zt2>.B }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st2b(z25.VnB(), z26.VnB(), p1, x25, x25), "st2b { <Zt1>.B, <Zt2>.B }, <Pg>, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(st2d(z22.VnD(), z23.VnD(), p1, x22, int imm4), "st2d { <Zt1>.D, <Zt2>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st2d(z24.VnD(), z25.VnD(), p1, x24, x24), "st2d { <Zt1>.D, <Zt2>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]");
  COMPARE_PREFIX(st2h(z3.VnH(), z4.VnH(), p1, x3, int imm4), "st2h { <Zt1>.H, <Zt2>.H }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st2h(z10.VnH(), z11.VnH(), p6, x10, x10), "st2h { <Zt1>.H, <Zt2>.H }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(st2w(z21.VnS(), z22.VnS(), p1, x21, int imm4), "st2w { <Zt1>.S, <Zt2>.S }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st2w(z29.VnS(), z30.VnS(), p5, x29, x29), "st2w { <Zt1>.S, <Zt2>.S }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]");
  COMPARE_PREFIX(st3b(z16.VnB(), z17.VnB(), z18.VnB(), p6, x16, int imm4), "st3b { <Zt1>.B, <Zt2>.B, <Zt3>.B }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st3b(z3.VnB(), z4.VnB(), z5.VnB(), p0, x3, x3), "st3b { <Zt1>.B, <Zt2>.B, <Zt3>.B }, <Pg>, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(st3d(z13.VnD(), z14.VnD(), z15.VnD(), p2, x13, int imm4), "st3d { <Zt1>.D, <Zt2>.D, <Zt3>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st3d(z6.VnD(), z7.VnD(), z8.VnD(), p7, x6, x6), "st3d { <Zt1>.D, <Zt2>.D, <Zt3>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]");
  COMPARE_PREFIX(st3h(z15.VnH(), z16.VnH(), z17.VnH(), p3, x15, int imm4), "st3h { <Zt1>.H, <Zt2>.H, <Zt3>.H }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st3h(z12.VnH(), z13.VnH(), z14.VnH(), p3, x12, x12), "st3h { <Zt1>.H, <Zt2>.H, <Zt3>.H }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(st3w(z16.VnS(), z17.VnS(), z18.VnS(), p5, x16, int imm4), "st3w { <Zt1>.S, <Zt2>.S, <Zt3>.S }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st3w(z30.VnS(), z31.VnS(), z0.VnS(), p0, x30, x30), "st3w { <Zt1>.S, <Zt2>.S, <Zt3>.S }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]");
  COMPARE_PREFIX(st4b(z1.VnB(), z2.VnB(), z3.VnB(), z4.VnB(), p5, x1, int imm4), "st4b { <Zt1>.B, <Zt2>.B, <Zt3>.B, <Zt4>.B }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st4b(z2.VnB(), z3.VnB(), z4.VnB(), z5.VnB(), p2, x2, x2), "st4b { <Zt1>.B, <Zt2>.B, <Zt3>.B, <Zt4>.B }, <Pg>, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(st4d(z1.VnD(), z2.VnD(), z3.VnD(), z4.VnD(), p6, x1, int imm4), "st4d { <Zt1>.D, <Zt2>.D, <Zt3>.D, <Zt4>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st4d(z30.VnD(), z31.VnD(), z0.VnD(), z1.VnD(), p1, x30, x30), "st4d { <Zt1>.D, <Zt2>.D, <Zt3>.D, <Zt4>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]");
  COMPARE_PREFIX(st4h(z8.VnH(), z9.VnH(), z10.VnH(), z11.VnH(), p1, x8, int imm4), "st4h { <Zt1>.H, <Zt2>.H, <Zt3>.H, <Zt4>.H }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st4h(z9.VnH(), z10.VnH(), z11.VnH(), z12.VnH(), p0, x9, x9), "st4h { <Zt1>.H, <Zt2>.H, <Zt3>.H, <Zt4>.H }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(st4w(z15.VnS(), z16.VnS(), z17.VnS(), z18.VnS(), p7, x15, int imm4), "st4w { <Zt1>.S, <Zt2>.S, <Zt3>.S, <Zt4>.S }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(st4w(z27.VnS(), z28.VnS(), z29.VnS(), z30.VnS(), p1, x27, x27), "st4w { <Zt1>.S, <Zt2>.S, <Zt3>.S, <Zt4>.S }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]");
  COMPARE_PREFIX(stnt1b(z21.VnB(), p3, x30, int imm4), "stnt1b { <Zt>.B }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(stnt1b(z23.VnB(), p5, x11, x11), "stnt1b { <Zt>.B }, <Pg>, [<Xn|SP>, <Xm>]");
  COMPARE_PREFIX(stnt1d(z5.VnD(), p3, x1, int imm4), "stnt1d { <Zt>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(stnt1d(z4.VnD(), p2, x21, x21), "stnt1d { <Zt>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]");
  COMPARE_PREFIX(stnt1h(z28.VnH(), p6, x27, int imm4), "stnt1h { <Zt>.H }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(stnt1h(z17.VnH(), p4, x17, x17), "stnt1h { <Zt>.H }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]");
  COMPARE_PREFIX(stnt1w(z23.VnS(), p2, x10, int imm4), "stnt1w { <Zt>.S }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]");
  COMPARE_PREFIX(stnt1w(z2.VnS(), p4, x1, x1), "stnt1w { <Zt>.S }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]");
#endif

  CLEANUP();
}

TEST(sve_mul_index) {
  SETUP();

#if 0
  COMPARE_PREFIX(sdot(z17.VnD(), z21.VnH()), "sdot <Zda>.D, <Zn>.H, <Zm>.H[<imm>]");
  COMPARE_PREFIX(sdot(z28.VnS(), z9.VnB()), "sdot <Zda>.S, <Zn>.B, <Zm>.B[<imm>]");
  COMPARE_PREFIX(udot(z26.VnD(), z15.VnH()), "udot <Zda>.D, <Zn>.H, <Zm>.H[<imm>]");
  COMPARE_PREFIX(udot(z23.VnS(), z24.VnB()), "udot <Zda>.S, <Zn>.B, <Zm>.B[<imm>]");
#endif

  CLEANUP();
}

TEST(sve_partition_break) {
  SETUP();

#if 0
  COMPARE_PREFIX(brkas(p8.VnB(), p5.Zeroing(), p4.VnB()), "brkas <Pd>.B, <Pg>/Z, <Pn>.B");
  COMPARE_PREFIX(brka(p11.VnB(), p7, p15.VnB()), "brka <Pd>.B, <Pg>/<ZM>, <Pn>.B");
  COMPARE_PREFIX(brkbs(p6.VnB(), p9.Zeroing(), p14.VnB()), "brkbs <Pd>.B, <Pg>/Z, <Pn>.B");
  COMPARE_PREFIX(brkb(p11.VnB(), p6, p4.VnB()), "brkb <Pd>.B, <Pg>/<ZM>, <Pn>.B");
  COMPARE_PREFIX(brkns(p2.VnB(), p11.Zeroing(), p0.VnB(), p2.VnB()), "brkns <Pdm>.B, <Pg>/Z, <Pn>.B, <Pdm>.B");
  COMPARE_PREFIX(brkn(p4.VnB(), p3.Zeroing(), p1.VnB(), p4.VnB()), "brkn <Pdm>.B, <Pg>/Z, <Pn>.B, <Pdm>.B");
#endif

  CLEANUP();
}

TEST(sve_permute_predicate) {
  SETUP();

#if 0
  COMPARE_PREFIX(punpkhi(p12.VnH(), p6.VnB()), "punpkhi <Pd>.H, <Pn>.B");
  COMPARE_PREFIX(punpklo(p4.VnH(), p14.VnB()), "punpklo <Pd>.H, <Pn>.B");
  COMPARE_PREFIX(rev(p15.VnB(), p6.VnB()), "rev <Pd>.<T>, <Pn>.<T>");
  COMPARE_PREFIX(rev(p15.VnH(), p6.VnH()), "rev <Pd>.<T>, <Pn>.<T>");
  COMPARE_PREFIX(rev(p15.VnS(), p6.VnS()), "rev <Pd>.<T>, <Pn>.<T>");
  COMPARE_PREFIX(rev(p15.VnD(), p6.VnD()), "rev <Pd>.<T>, <Pn>.<T>");
  COMPARE_PREFIX(trn1(p13.VnB(), p15.VnB(), p12.VnB()), "trn1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(trn1(p13.VnH(), p15.VnH(), p12.VnH()), "trn1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(trn1(p13.VnS(), p15.VnS(), p12.VnS()), "trn1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(trn1(p13.VnD(), p15.VnD(), p12.VnD()), "trn1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(trn2(p5.VnB(), p5.VnB(), p6.VnB()), "trn2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(trn2(p5.VnH(), p5.VnH(), p6.VnH()), "trn2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(trn2(p5.VnS(), p5.VnS(), p6.VnS()), "trn2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(trn2(p5.VnD(), p5.VnD(), p6.VnD()), "trn2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(uzp1(p14.VnB(), p4.VnB(), p14.VnB()), "uzp1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(uzp1(p14.VnH(), p4.VnH(), p14.VnH()), "uzp1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(uzp1(p14.VnS(), p4.VnS(), p14.VnS()), "uzp1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(uzp1(p14.VnD(), p4.VnD(), p14.VnD()), "uzp1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(uzp2(p6.VnB(), p11.VnB(), p2.VnB()), "uzp2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(uzp2(p6.VnH(), p11.VnH(), p2.VnH()), "uzp2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(uzp2(p6.VnS(), p11.VnS(), p2.VnS()), "uzp2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(uzp2(p6.VnD(), p11.VnD(), p2.VnD()), "uzp2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(zip1(p13.VnB(), p4.VnB(), p13.VnB()), "zip1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(zip1(p13.VnH(), p4.VnH(), p13.VnH()), "zip1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(zip1(p13.VnS(), p4.VnS(), p13.VnS()), "zip1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(zip1(p13.VnD(), p4.VnD(), p13.VnD()), "zip1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(zip2(p1.VnB(), p15.VnB(), p2.VnB()), "zip2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(zip2(p1.VnH(), p15.VnH(), p2.VnH()), "zip2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(zip2(p1.VnS(), p15.VnS(), p2.VnS()), "zip2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
  COMPARE_PREFIX(zip2(p1.VnD(), p15.VnD(), p2.VnD()), "zip2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>");
#endif

  CLEANUP();
}

TEST(sve_permute_vector_extract) {
  SETUP();

#if 0
  COMPARE_PREFIX(ext(z10.VnB(), z10.VnB(), z2.VnB()), "ext <Zdn>.B, <Zdn>.B, <Zm>.B, #<imm>");
#endif

  CLEANUP();
}

TEST(sve_permute_vector_interleaving) {
  SETUP();

#if 0
  COMPARE_PREFIX(trn1(z25.VnB(), z31.VnB(), z17.VnB()), "trn1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(trn1(z25.VnH(), z31.VnH(), z17.VnH()), "trn1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(trn1(z25.VnS(), z31.VnS(), z17.VnS()), "trn1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(trn1(z25.VnD(), z31.VnD(), z17.VnD()), "trn1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(trn2(z23.VnB(), z19.VnB(), z5.VnB()), "trn2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(trn2(z23.VnH(), z19.VnH(), z5.VnH()), "trn2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(trn2(z23.VnS(), z19.VnS(), z5.VnS()), "trn2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(trn2(z23.VnD(), z19.VnD(), z5.VnD()), "trn2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(uzp1(z3.VnB(), z27.VnB(), z10.VnB()), "uzp1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(uzp1(z3.VnH(), z27.VnH(), z10.VnH()), "uzp1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(uzp1(z3.VnS(), z27.VnS(), z10.VnS()), "uzp1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(uzp1(z3.VnD(), z27.VnD(), z10.VnD()), "uzp1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(uzp2(z22.VnB(), z26.VnB(), z15.VnB()), "uzp2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(uzp2(z22.VnH(), z26.VnH(), z15.VnH()), "uzp2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(uzp2(z22.VnS(), z26.VnS(), z15.VnS()), "uzp2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(uzp2(z22.VnD(), z26.VnD(), z15.VnD()), "uzp2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(zip1(z31.VnB(), z2.VnB(), z20.VnB()), "zip1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(zip1(z31.VnH(), z2.VnH(), z20.VnH()), "zip1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(zip1(z31.VnS(), z2.VnS(), z20.VnS()), "zip1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(zip1(z31.VnD(), z2.VnD(), z20.VnD()), "zip1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(zip2(z15.VnB(), z23.VnB(), z12.VnB()), "zip2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(zip2(z15.VnH(), z23.VnH(), z12.VnH()), "zip2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(zip2(z15.VnS(), z23.VnS(), z12.VnS()), "zip2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(zip2(z15.VnD(), z23.VnD(), z12.VnD()), "zip2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>");
#endif

  CLEANUP();
}

TEST(sve_permute_vector_predicated) {
  SETUP();

#if 0
  COMPARE_PREFIX(clasta(r6, p0, r6, z13.VnB()), "clasta <R><dn>, <Pg>, <R><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clasta(r6, p0, r6, z13.VnH()), "clasta <R><dn>, <Pg>, <R><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clasta(r6, p0, r6, z13.VnS()), "clasta <R><dn>, <Pg>, <R><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clasta(r6, p0, r6, z13.VnD()), "clasta <R><dn>, <Pg>, <R><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clasta(b8, p6, b8, z7.VnB()), "clasta <V><dn>, <Pg>, <V><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clasta(h8, p6, h8, z7.VnH()), "clasta <V><dn>, <Pg>, <V><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clasta(s8, p6, s8, z7.VnS()), "clasta <V><dn>, <Pg>, <V><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clasta(d8, p6, d8, z7.VnD()), "clasta <V><dn>, <Pg>, <V><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clasta(z4.VnB(), p2, z4.VnB(), z12.VnB()), "clasta <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(clasta(z4.VnH(), p2, z4.VnH(), z12.VnH()), "clasta <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(clasta(z4.VnS(), p2, z4.VnS(), z12.VnS()), "clasta <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(clasta(z4.VnD(), p2, z4.VnD(), z12.VnD()), "clasta <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(clastb(r21, p2, r21, z27.VnB()), "clastb <R><dn>, <Pg>, <R><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clastb(r21, p2, r21, z27.VnH()), "clastb <R><dn>, <Pg>, <R><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clastb(r21, p2, r21, z27.VnS()), "clastb <R><dn>, <Pg>, <R><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clastb(r21, p2, r21, z27.VnD()), "clastb <R><dn>, <Pg>, <R><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clastb(b17, p0, b17, z19.VnB()), "clastb <V><dn>, <Pg>, <V><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clastb(h17, p0, h17, z19.VnH()), "clastb <V><dn>, <Pg>, <V><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clastb(s17, p0, s17, z19.VnS()), "clastb <V><dn>, <Pg>, <V><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clastb(d17, p0, d17, z19.VnD()), "clastb <V><dn>, <Pg>, <V><dn>, <Zm>.<T>");
  COMPARE_PREFIX(clastb(z29.VnB(), p7, z29.VnB(), z26.VnB()), "clastb <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(clastb(z29.VnH(), p7, z29.VnH(), z26.VnH()), "clastb <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(clastb(z29.VnS(), p7, z29.VnS(), z26.VnS()), "clastb <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(clastb(z29.VnD(), p7, z29.VnD(), z26.VnD()), "clastb <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(compact(z13.Vn?(), p7, z1.Vn?()), "compact <Zd>.<T>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(cpy(z1.VnB(), p1.Merging(), xn), "cpy <Zd>.<T>, <Pg>/M, <R><n|SP>");
  COMPARE_PREFIX(cpy(z1.VnH(), p1.Merging(), xn), "cpy <Zd>.<T>, <Pg>/M, <R><n|SP>");
  COMPARE_PREFIX(cpy(z1.VnS(), p1.Merging(), xn), "cpy <Zd>.<T>, <Pg>/M, <R><n|SP>");
  COMPARE_PREFIX(cpy(z1.VnD(), p1.Merging(), xn), "cpy <Zd>.<T>, <Pg>/M, <R><n|SP>");
  COMPARE_PREFIX(cpy(z27.VnB(), p3.Merging(), b23), "cpy <Zd>.<T>, <Pg>/M, <V><n>");
  COMPARE_PREFIX(cpy(z27.VnH(), p3.Merging(), h23), "cpy <Zd>.<T>, <Pg>/M, <V><n>");
  COMPARE_PREFIX(cpy(z27.VnS(), p3.Merging(), s23), "cpy <Zd>.<T>, <Pg>/M, <V><n>");
  COMPARE_PREFIX(cpy(z27.VnD(), p3.Merging(), d23), "cpy <Zd>.<T>, <Pg>/M, <V><n>");
  COMPARE_PREFIX(lasta(r15, p3, z3.VnB()), "lasta <R><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lasta(r15, p3, z3.VnH()), "lasta <R><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lasta(r15, p3, z3.VnS()), "lasta <R><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lasta(r15, p3, z3.VnD()), "lasta <R><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lasta(b30, p4, z24.VnB()), "lasta <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lasta(h30, p4, z24.VnH()), "lasta <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lasta(s30, p4, z24.VnS()), "lasta <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lasta(d30, p4, z24.VnD()), "lasta <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lastb(r9, p2, z16.VnB()), "lastb <R><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lastb(r9, p2, z16.VnH()), "lastb <R><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lastb(r9, p2, z16.VnS()), "lastb <R><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lastb(r9, p2, z16.VnD()), "lastb <R><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lastb(b14, p5, z2.VnB()), "lastb <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lastb(h14, p5, z2.VnH()), "lastb <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lastb(s14, p5, z2.VnS()), "lastb <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(lastb(d14, p5, z2.VnD()), "lastb <V><d>, <Pg>, <Zn>.<T>");
  COMPARE_PREFIX(rbit(z22.VnB(), p2.Merging(), z24.VnB()), "rbit <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(rbit(z22.VnH(), p2.Merging(), z24.VnH()), "rbit <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(rbit(z22.VnS(), p2.Merging(), z24.VnS()), "rbit <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(rbit(z22.VnD(), p2.Merging(), z24.VnD()), "rbit <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(revb(z14.VnH(), p7.Merging(), z25.VnH()), "revb <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(revb(z14.VnS(), p7.Merging(), z25.VnS()), "revb <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(revb(z14.VnD(), p7.Merging(), z25.VnD()), "revb <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(revh(z16.Vn?(), p2.Merging(), z4.Vn?()), "revh <Zd>.<T>, <Pg>/M, <Zn>.<T>");
  COMPARE_PREFIX(revw(z26.VnD(), p5.Merging(), z10.VnD()), "revw <Zd>.D, <Pg>/M, <Zn>.D");
  COMPARE_PREFIX(splice(z7.VnB(), p6, z7.VnB(), z2.VnB()), "splice <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(splice(z7.VnH(), p6, z7.VnH(), z2.VnH()), "splice <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(splice(z7.VnS(), p6, z7.VnS(), z2.VnS()), "splice <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(splice(z7.VnD(), p6, z7.VnD(), z2.VnD()), "splice <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>");
#endif

  CLEANUP();
}

TEST(sve_permute_vector_unpredicated) {
  SETUP();

  COMPARE_PREFIX(dup(z4.VnB(), w7), "dup z4.b, w7");
  COMPARE_PREFIX(dup(z5.VnH(), w6), "dup z5.h, w6");
  COMPARE_PREFIX(dup(z6.VnS(), sp), "dup z6.s, wsp");
  COMPARE_PREFIX(dup(z7.VnD(), x4), "dup z7.d, x4");
  COMPARE_PREFIX(dup(z25.VnQ(), z28.VnQ(), 2), "dup z25.q, z28.q[2]");
  COMPARE_PREFIX(insr(z15.VnB(), w13), "insr z15.b, w13");
  COMPARE_PREFIX(insr(z16.VnH(), w14), "insr z16.h, w14");
  COMPARE_PREFIX(insr(z17.VnS(), w15), "insr z17.s, w15");
  COMPARE_PREFIX(insr(z18.VnD(), x16), "insr z18.d, x16");
  COMPARE_PREFIX(insr(z5.VnB(), b3), "insr z5.b, b3");
  COMPARE_PREFIX(insr(z6.VnH(), h15), "insr z6.h, h15");
  COMPARE_PREFIX(insr(z7.VnS(), s22), "insr z7.s, s22");
  COMPARE_PREFIX(insr(z8.VnD(), d30), "insr z8.d, d30");
  COMPARE_PREFIX(rev(z13.VnB(), z10.VnB()), "rev z13.b, z10.b");
  COMPARE_PREFIX(rev(z14.VnH(), z10.VnH()), "rev z14.h, z10.h");
  COMPARE_PREFIX(rev(z15.VnS(), z10.VnS()), "rev z15.s, z10.s");
  COMPARE_PREFIX(rev(z16.VnD(), z10.VnD()), "rev z16.d, z10.d");
  COMPARE_PREFIX(sunpkhi(z10.VnH(), z11.VnB()), "sunpkhi z10.h, z11.b");
  COMPARE_PREFIX(sunpkhi(z11.VnS(), z11.VnH()), "sunpkhi z11.s, z11.h");
  COMPARE_PREFIX(sunpkhi(z12.VnD(), z11.VnS()), "sunpkhi z12.d, z11.s");
  COMPARE_PREFIX(sunpklo(z20.VnH(), z12.VnB()), "sunpklo z20.h, z12.b");
  COMPARE_PREFIX(sunpklo(z21.VnS(), z12.VnH()), "sunpklo z21.s, z12.h");
  COMPARE_PREFIX(sunpklo(z22.VnD(), z12.VnS()), "sunpklo z22.d, z12.s");
  COMPARE_PREFIX(uunpkhi(z17.VnH(), z14.VnB()), "uunpkhi z17.h, z14.b");
  COMPARE_PREFIX(uunpkhi(z18.VnS(), z14.VnH()), "uunpkhi z18.s, z14.h");
  COMPARE_PREFIX(uunpkhi(z19.VnD(), z14.VnS()), "uunpkhi z19.d, z14.s");
  COMPARE_PREFIX(uunpklo(z27.VnH(), z6.VnB()), "uunpklo z27.h, z6.b");
  COMPARE_PREFIX(uunpklo(z28.VnS(), z6.VnH()), "uunpklo z28.s, z6.h");
  COMPARE_PREFIX(uunpklo(z29.VnD(), z6.VnS()), "uunpklo z29.d, z6.s");
  COMPARE_PREFIX(tbl(z24.VnB(), z29.VnB(), z0.VnB()),
                 "tbl z24.b, { z29.b }, z0.b");
  COMPARE_PREFIX(tbl(z25.VnH(), z29.VnH(), z1.VnH()),
                 "tbl z25.h, { z29.h }, z1.h");
  COMPARE_PREFIX(tbl(z26.VnS(), z29.VnS(), z2.VnS()),
                 "tbl z26.s, { z29.s }, z2.s");
  COMPARE_PREFIX(tbl(z27.VnD(), z29.VnD(), z3.VnD()),
                 "tbl z27.d, { z29.d }, z3.d");

  CLEANUP();
}

TEST(sve_predicate_count) {
  SETUP();

  COMPARE_PREFIX(cntp(x9, p1, p0.VnB()), "cntp x9, p1, p0.b");
  COMPARE_PREFIX(cntp(x10, p12, p1.VnH()), "cntp x10, p12, p1.h");
  COMPARE_PREFIX(cntp(x11, p13, p14.VnS()), "cntp x11, p13, p14.s");
  COMPARE_PREFIX(cntp(x12, p4, p15.VnD()), "cntp x12, p4, p15.d");

  COMPARE_MACRO(Cntp(x0, p1, p2.VnB()), "cntp x0, p1, p2.b");
  COMPARE_MACRO(Cntp(w10, p11, p12.VnH()), "cntp x10, p11, p12.h");

  CLEANUP();
}

TEST(sve_predicate_logical_op) {
  SETUP();

  COMPARE_PREFIX(ands(p13.VnB(), p9.Zeroing(), p5.VnB(), p15.VnB()),
                 "ands p13.b, p9/z, p5.b, p15.b");
  COMPARE_PREFIX(and_(p9.VnB(), p3.Zeroing(), p0.VnB(), p14.VnB()),
                 "and p9.b, p3/z, p0.b, p14.b");
  COMPARE_PREFIX(bics(p8.VnB(), p5.Zeroing(), p3.VnB(), p1.VnB()),
                 "bics p8.b, p5/z, p3.b, p1.b");
  COMPARE_PREFIX(bic(p5.VnB(), p5.Zeroing(), p9.VnB(), p9.VnB()),
                 "bic p5.b, p5/z, p9.b, p9.b");
  COMPARE_PREFIX(eors(p11.VnB(), p1.Zeroing(), p1.VnB(), p1.VnB()),
                 "eors p11.b, p1/z, p1.b, p1.b");
  COMPARE_PREFIX(eor(p8.VnB(), p6.Zeroing(), p1.VnB(), p11.VnB()),
                 "eor p8.b, p6/z, p1.b, p11.b");
  COMPARE_PREFIX(nands(p13.VnB(), p0.Zeroing(), p9.VnB(), p4.VnB()),
                 "nands p13.b, p0/z, p9.b, p4.b");
  COMPARE_PREFIX(nand(p7.VnB(), p7.Zeroing(), p15.VnB(), p2.VnB()),
                 "nand p7.b, p7/z, p15.b, p2.b");
  COMPARE_PREFIX(nors(p8.VnB(), p8.Zeroing(), p12.VnB(), p11.VnB()),
                 "nors p8.b, p8/z, p12.b, p11.b");
  COMPARE_PREFIX(nor(p3.VnB(), p6.Zeroing(), p15.VnB(), p12.VnB()),
                 "nor p3.b, p6/z, p15.b, p12.b");
  COMPARE_PREFIX(orns(p10.VnB(), p11.Zeroing(), p0.VnB(), p15.VnB()),
                 "orns p10.b, p11/z, p0.b, p15.b");
  COMPARE_PREFIX(orn(p0.VnB(), p1.Zeroing(), p7.VnB(), p4.VnB()),
                 "orn p0.b, p1/z, p7.b, p4.b");
  COMPARE_PREFIX(orrs(p14.VnB(), p6.Zeroing(), p1.VnB(), p5.VnB()),
                 "orrs p14.b, p6/z, p1.b, p5.b");
  COMPARE_PREFIX(orr(p13.VnB(), p7.Zeroing(), p10.VnB(), p4.VnB()),
                 "orr p13.b, p7/z, p10.b, p4.b");
  COMPARE_PREFIX(sel(p9.VnB(), p15, p15.VnB(), p7.VnB()),
                 "sel p9.b, p15, p15.b, p7.b");

  CLEANUP();
}

TEST(sve_predicate_first_active) {
  SETUP();

  COMPARE_PREFIX(pfirst(p0.VnB(), p7, p0.VnB()), "pfirst p0.b, p7, p0.b");
  COMPARE_PREFIX(pfirst(p7.VnB(), p0, p7.VnB()), "pfirst p7.b, p0, p7.b");

  COMPARE_MACRO(Pfirst(p1.VnB(), p2, p1.VnB()), "pfirst p1.b, p2, p1.b");
  COMPARE_MACRO(Pfirst(p3.VnB(), p4, p5.VnB()),
                "mov p3.b, p5.b\n"
                "pfirst p3.b, p4, p3.b");

  {
    UseScratchRegisterScope temps(&masm);
    temps.Include(p7, p15);
    COMPARE_MACRO(Pfirst(p6.VnB(), p6, p0.VnB()),
                  "mov p15.b, p6.b\n"
                  "mov p6.b, p0.b\n"
                  "pfirst p6.b, p15, p6.b");
  }

  CLEANUP();
}

TEST(sve_predicate_next_active) {
  SETUP();

  COMPARE_PREFIX(pnext(p0.VnB(), p8, p0.VnB()), "pnext p0.b, p8, p0.b");
  COMPARE_PREFIX(pnext(p1.VnH(), p9, p1.VnH()), "pnext p1.h, p9, p1.h");
  COMPARE_PREFIX(pnext(p2.VnS(), p10, p2.VnS()), "pnext p2.s, p10, p2.s");
  COMPARE_PREFIX(pnext(p3.VnD(), p11, p3.VnD()), "pnext p3.d, p11, p3.d");

  COMPARE_PREFIX(pnext(p12.VnB(), p4, p12.VnB()), "pnext p12.b, p4, p12.b");
  COMPARE_PREFIX(pnext(p13.VnH(), p5, p13.VnH()), "pnext p13.h, p5, p13.h");
  COMPARE_PREFIX(pnext(p14.VnS(), p6, p14.VnS()), "pnext p14.s, p6, p14.s");
  COMPARE_PREFIX(pnext(p15.VnD(), p7, p15.VnD()), "pnext p15.d, p7, p15.d");

  COMPARE_MACRO(Pnext(p5.VnB(), p9, p5.VnB()), "pnext p5.b, p9, p5.b");
  COMPARE_MACRO(Pnext(p6.VnH(), p8, p6.VnH()), "pnext p6.h, p8, p6.h");
  COMPARE_MACRO(Pnext(p7.VnS(), p5, p7.VnS()), "pnext p7.s, p5, p7.s");
  COMPARE_MACRO(Pnext(p8.VnD(), p6, p8.VnD()), "pnext p8.d, p6, p8.d");

  COMPARE_MACRO(Pnext(p6.VnB(), p4, p5.VnB()),
                "mov p6.b, p5.b\n"
                "pnext p6.b, p4, p6.b");
  COMPARE_MACRO(Pnext(p7.VnH(), p3, p8.VnH()),
                "mov p7.b, p8.b\n"
                "pnext p7.h, p3, p7.h");
  COMPARE_MACRO(Pnext(p8.VnS(), p2, p9.VnS()),
                "mov p8.b, p9.b\n"
                "pnext p8.s, p2, p8.s");
  COMPARE_MACRO(Pnext(p9.VnD(), p1, p10.VnD()),
                "mov p9.b, p10.b\n"
                "pnext p9.d, p1, p9.d");

  {
    UseScratchRegisterScope temps(&masm);
    temps.Include(p7, p8);
    COMPARE_MACRO(Pnext(p6.VnB(), p6, p0.VnB()),
                  "mov p8.b, p6.b\n"
                  "mov p6.b, p0.b\n"
                  "pnext p6.b, p8, p6.b");
    COMPARE_MACRO(Pnext(p7.VnH(), p7, p1.VnH()),
                  "mov p8.b, p7.b\n"
                  "mov p7.b, p1.b\n"
                  "pnext p7.h, p8, p7.h");
    COMPARE_MACRO(Pnext(p10.VnS(), p10, p2.VnS()),
                  "mov p8.b, p10.b\n"
                  "mov p10.b, p2.b\n"
                  "pnext p10.s, p8, p10.s");
    COMPARE_MACRO(Pnext(p11.VnD(), p11, p3.VnD()),
                  "mov p8.b, p11.b\n"
                  "mov p11.b, p3.b\n"
                  "pnext p11.d, p8, p11.d");
  }

  CLEANUP();
}

TEST(sve_predicate_initialize) {
  SETUP();

  // Basic forms.
  COMPARE_PREFIX(ptrue(p0.VnB(), SVE_POW2), "ptrue p0.b, pow2");
  COMPARE_PREFIX(ptrue(p1.VnH(), SVE_VL1), "ptrue p1.h, vl1");
  COMPARE_PREFIX(ptrue(p2.VnS(), SVE_VL8), "ptrue p2.s, vl8");
  COMPARE_PREFIX(ptrue(p3.VnD(), SVE_VL16), "ptrue p3.d, vl16");
  COMPARE_PREFIX(ptrue(p4.VnB(), SVE_VL256), "ptrue p4.b, vl256");
  COMPARE_PREFIX(ptrue(p5.VnH(), SVE_MUL3), "ptrue p5.h, mul3");
  COMPARE_PREFIX(ptrue(p6.VnS(), SVE_MUL4), "ptrue p6.s, mul4");
  COMPARE_PREFIX(ptrue(p7.VnD(), SVE_ALL), "ptrue p7.d");

  COMPARE_PREFIX(ptrues(p8.VnB(), SVE_ALL), "ptrues p8.b");
  COMPARE_PREFIX(ptrues(p9.VnH(), SVE_MUL4), "ptrues p9.h, mul4");
  COMPARE_PREFIX(ptrues(p10.VnS(), SVE_MUL3), "ptrues p10.s, mul3");
  COMPARE_PREFIX(ptrues(p11.VnD(), SVE_VL256), "ptrues p11.d, vl256");
  COMPARE_PREFIX(ptrues(p12.VnB(), SVE_VL16), "ptrues p12.b, vl16");
  COMPARE_PREFIX(ptrues(p13.VnH(), SVE_VL8), "ptrues p13.h, vl8");
  COMPARE_PREFIX(ptrues(p14.VnS(), SVE_VL1), "ptrues p14.s, vl1");
  COMPARE_PREFIX(ptrues(p15.VnD(), SVE_POW2), "ptrues p15.d, pow2");

  // The Assembler supports arbitrary immediates.
  COMPARE_PREFIX(ptrue(p7.VnS(), 0xd), "ptrue p7.s, vl256");
  COMPARE_PREFIX(ptrue(p8.VnD(), 0xe), "ptrue p8.d, #0xe");
  COMPARE_PREFIX(ptrue(p9.VnB(), 0x15), "ptrue p9.b, #0x15");
  COMPARE_PREFIX(ptrue(p10.VnH(), 0x19), "ptrue p10.h, #0x19");
  COMPARE_PREFIX(ptrue(p11.VnS(), 0x1a), "ptrue p11.s, #0x1a");
  COMPARE_PREFIX(ptrue(p12.VnD(), 0x1c), "ptrue p12.d, #0x1c");
  COMPARE_PREFIX(ptrue(p13.VnB(), 0x1d), "ptrue p13.b, mul4");

  COMPARE_PREFIX(ptrues(p14.VnS(), 0xd), "ptrues p14.s, vl256");
  COMPARE_PREFIX(ptrues(p15.VnD(), 0xe), "ptrues p15.d, #0xe");
  COMPARE_PREFIX(ptrues(p0.VnB(), 0x15), "ptrues p0.b, #0x15");
  COMPARE_PREFIX(ptrues(p1.VnH(), 0x19), "ptrues p1.h, #0x19");
  COMPARE_PREFIX(ptrues(p2.VnS(), 0x1a), "ptrues p2.s, #0x1a");
  COMPARE_PREFIX(ptrues(p3.VnD(), 0x1c), "ptrues p3.d, #0x1c");
  COMPARE_PREFIX(ptrues(p4.VnB(), 0x1d), "ptrues p4.b, mul4");

  // SVE_ALL is the default.
  COMPARE_PREFIX(ptrue(p15.VnS()), "ptrue p15.s");
  COMPARE_PREFIX(ptrues(p0.VnS()), "ptrues p0.s");

  // The MacroAssembler provides a `FlagsUpdate` argument.
  COMPARE_MACRO(Ptrue(p0.VnB(), SVE_MUL3), "ptrue p0.b, mul3");
  COMPARE_MACRO(Ptrues(p1.VnH(), SVE_MUL4), "ptrues p1.h, mul4");
  COMPARE_MACRO(Ptrue(p2.VnS(), SVE_VL32, LeaveFlags), "ptrue p2.s, vl32");
  COMPARE_MACRO(Ptrue(p3.VnD(), SVE_VL64, SetFlags), "ptrues p3.d, vl64");
}

TEST(sve_pfalse) {
  SETUP();

  COMPARE_PREFIX(pfalse(p0.VnB()), "pfalse p0.b");
  COMPARE_PREFIX(pfalse(p15.VnB()), "pfalse p15.b");

  COMPARE_MACRO(Pfalse(p1.VnB()), "pfalse p1.b");
  COMPARE_MACRO(Pfalse(p4.VnH()), "pfalse p4.b");
  COMPARE_MACRO(Pfalse(p9.VnS()), "pfalse p9.b");
  COMPARE_MACRO(Pfalse(p14.VnD()), "pfalse p14.b");
}

TEST(sve_ptest) {
  SETUP();

  COMPARE_PREFIX(ptest(p15, p0.VnB()), "ptest p15, p0.b");
  COMPARE_PREFIX(ptest(p0, p15.VnB()), "ptest p0, p15.b");
  COMPARE_PREFIX(ptest(p6, p6.VnB()), "ptest p6, p6.b");

  COMPARE_MACRO(Ptest(p0, p1.VnB()), "ptest p0, p1.b");
}

TEST(sve_predicate_misc) {
  // TODO: Replace this with separate tests.
  SETUP();

#if 0
  COMPARE_PREFIX(rdffrs(p14.VnB(), p9.Zeroing()), "rdffrs <Pd>.B, <Pg>/Z");
  COMPARE_PREFIX(rdffr(p13.VnB()), "rdffr <Pd>.B");
  COMPARE_PREFIX(rdffr(p5.VnB(), p14.Zeroing()), "rdffr <Pd>.B, <Pg>/Z");
#endif

  CLEANUP();
}

TEST(sve_propagate_break) {
  SETUP();

#if 0
  COMPARE_PREFIX(brkpas(p12.VnB(), p0.Zeroing(), p12.VnB(), p11.VnB()), "brkpas <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B");
  COMPARE_PREFIX(brkpa(p1.VnB(), p2.Zeroing(), p13.VnB(), p8.VnB()), "brkpa <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B");
  COMPARE_PREFIX(brkpbs(p14.VnB(), p1.Zeroing(), p8.VnB(), p3.VnB()), "brkpbs <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B");
  COMPARE_PREFIX(brkpb(p2.VnB(), p5.Zeroing(), p0.VnB(), p14.VnB()), "brkpb <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B");
#endif

  CLEANUP();
}

TEST(sve_stack_allocation) {
  SETUP();

  COMPARE_PREFIX(rdvl(x26, 0), "rdvl x26, #0");
  COMPARE_PREFIX(rdvl(x27, 31), "rdvl x27, #31");
  COMPARE_PREFIX(rdvl(x28, -32), "rdvl x28, #-32");
  COMPARE_PREFIX(rdvl(xzr, 9), "rdvl xzr, #9");

  COMPARE_PREFIX(addvl(x6, x20, 0), "addvl x6, x20, #0");
  COMPARE_PREFIX(addvl(x7, x21, 31), "addvl x7, x21, #31");
  COMPARE_PREFIX(addvl(x8, x22, -32), "addvl x8, x22, #-32");
  COMPARE_PREFIX(addvl(sp, x1, 5), "addvl sp, x1, #5");
  COMPARE_PREFIX(addvl(x9, sp, -16), "addvl x9, sp, #-16");

  COMPARE_PREFIX(addpl(x20, x6, 0), "addpl x20, x6, #0");
  COMPARE_PREFIX(addpl(x21, x7, 31), "addpl x21, x7, #31");
  COMPARE_PREFIX(addpl(x22, x8, -32), "addpl x22, x8, #-32");
  COMPARE_PREFIX(addpl(sp, x1, 5), "addpl sp, x1, #5");
  COMPARE_PREFIX(addpl(x9, sp, -16), "addpl x9, sp, #-16");

  CLEANUP();
}

TEST(sve_stack_allocation_macro) {
  SETUP();

  COMPARE_MACRO(Rdvl(x0, 3), "rdvl x0, #3");
  COMPARE_MACRO(Rdvl(x1, 42),
                "mov x1, #0x2a\n"
                "rdvl x16, #1\n"
                "mul x1, x1, x16");

  COMPARE_MACRO(Rdpl(x0, 8), "rdvl x0, #1");
  COMPARE_MACRO(Rdpl(x1, 7),
                "mov x1, #0x0\n"
                "addpl x1, x1, #7");
  COMPARE_MACRO(Rdpl(x2, 42),
                "mov x2, #0x2a\n"
                "rdvl x16, #1\n"
                "mul x2, x2, x16\n"
                "add x2, xzr, x2, asr #3");

  COMPARE_MACRO(Addvl(sp, sp, -3), "addvl sp, sp, #-3");
  COMPARE_MACRO(Addvl(x7, xzr, 8), "rdvl x7, #8");
  COMPARE_MACRO(Addvl(x7, x8, 42),
                "mov x7, #0x2a\n"
                "rdvl x16, #1\n"
                "madd x7, x7, x16, x8");
  COMPARE_MACRO(Addvl(x10, x10, 42),
                "mov x16, #0x2a\n"
                "rdvl x17, #1\n"
                "madd x10, x16, x17, x10");
  COMPARE_MACRO(Addvl(x10, sp, 42),
                "mov x10, #0x2a\n"
                "rdvl x16, #1\n"
                "mul x10, x10, x16\n"
                "add x10, sp, x10");
  COMPARE_MACRO(Addvl(sp, x10, 42),
                "mov x16, #0x2a\n"
                "rdvl x17, #1\n"
                "mul x16, x16, x17\n"
                "add sp, x10, x16");

  COMPARE_MACRO(Addpl(x22, x22, -3), "addpl x22, x22, #-3");
  COMPARE_MACRO(Addpl(x7, x8, 32), "addvl x7, x8, #4");
  COMPARE_MACRO(Addpl(x7, x8, 42),
                "mov x7, #0x2a\n"
                "rdvl x16, #1\n"
                "mul x7, x7, x16\n"
                "add x7, x8, x7, asr #3");
  COMPARE_MACRO(Addpl(x7, sp, 42),
                "mov x7, #0x2a\n"
                "rdvl x16, #1\n"
                "mul x7, x7, x16\n"
                "asr x7, x7, #3\n"
                "add x7, sp, x7");

  CLEANUP();
}

TEST(sve_vector_select) {
  SETUP();

#if 0
  COMPARE_PREFIX(sel(z13.VnB(), p3, z3.VnB(), z25.VnB()), "sel <Zd>.<T>, <Pg>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(sel(z13.VnH(), p3, z3.VnH(), z25.VnH()), "sel <Zd>.<T>, <Pg>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(sel(z13.VnS(), p3, z3.VnS(), z25.VnS()), "sel <Zd>.<T>, <Pg>, <Zn>.<T>, <Zm>.<T>");
  COMPARE_PREFIX(sel(z13.VnD(), p3, z3.VnD(), z25.VnD()), "sel <Zd>.<T>, <Pg>, <Zn>.<T>, <Zm>.<T>");
#endif

  CLEANUP();
}

TEST(sve_write_ffr) {
  SETUP();

#if 0
  COMPARE_PREFIX((), "setffr  ");
  COMPARE_PREFIX(wrffr(p9.VnB()), "wrffr <Pn>.B");
#endif

  CLEANUP();
}

}  // namespace aarch64
}  // namespace vixl
