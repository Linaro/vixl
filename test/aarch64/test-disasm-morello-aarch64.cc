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

TEST(morello_mov_cpy_c_c_c) {
  SETUP();

  COMPARE_MORELLO(mov(c0, c1), "mov c0, c1");
  COMPARE_MORELLO(mov(c0, csp), "mov c0, csp");
  COMPARE_MORELLO(mov(csp, c1), "mov csp, c1");
  COMPARE_MORELLO(mov(c30, c30), "mov c30, c30");
}

TEST(morello_mov_cpy_macro) {
  typedef DisasmTestUtilMacroAssembler MacroAssembler;

  SETUP();

  auto mov_x_00 = [&masm] { masm.Mov(x0, x0); };
  auto mov_x_00_keep = [&masm] { masm.Mov(x0, x0, kKeepExtendingMoves); };
  auto mov_x_00_discard = [&masm] { masm.Mov(x0, x0, kDiscardForSameReg); };
  auto mov_x_01 = [&masm] { masm.Mov(x0, x1); };
  auto mov_x_01_keep = [&masm] { masm.Mov(x0, x1, kKeepExtendingMoves); };
  auto mov_x_01_discard = [&masm] { masm.Mov(x0, x1, kDiscardForSameReg); };
  CPUFeatures cpu_none = CPUFeatures::None();
  COMPARE_MACRO_A64(WithCPUFeatures(mov_x_00, cpu_none), "");
  COMPARE_MACRO_A64(WithCPUFeatures(mov_x_00_keep, cpu_none), "");
  COMPARE_MACRO_A64(WithCPUFeatures(mov_x_00_discard, cpu_none), "");
  COMPARE_MACRO_A64(WithCPUFeatures(mov_x_01, cpu_none), "mov x0, x1");
  COMPARE_MACRO_A64(WithCPUFeatures(mov_x_01_keep, cpu_none), "mov x0, x1");
  COMPARE_MACRO_A64(WithCPUFeatures(mov_x_01_discard, cpu_none), "mov x0, x1");

  COMPARE_MACRO_MORELLO(Mov(x0, x0), "mov x0, x0");
  COMPARE_MACRO_MORELLO(Mov(x0, x0, kKeepExtendingMoves), "mov x0, x0");
  COMPARE_MACRO_MORELLO(Mov(x0, x0, kDiscardForSameReg), "");
  COMPARE_MACRO_MORELLO(Mov(x0, x1), "mov x0, x1");
  COMPARE_MACRO_MORELLO(Mov(x0, x1, kKeepExtendingMoves), "mov x0, x1");
  COMPARE_MACRO_MORELLO(Mov(x0, x1, kDiscardForSameReg), "mov x0, x1");

  COMPARE_MACRO_MORELLO(Mov(c0, c1), "mov c0, c1");
  COMPARE_MACRO_MORELLO(Mov(c0, csp), "mov c0, csp");
  COMPARE_MACRO_MORELLO(Mov(csp, c0), "mov csp, c0");
  // The MacroAssembler elides no-op moves.
  COMPARE_MACRO_MORELLO(Mov(csp, csp), "");
  COMPARE_MACRO_MORELLO(Mov(c30, c30), "");

  // CRegister `Cpy` behaves exactly like CRegister`Mov`
  COMPARE_MACRO_MORELLO(Cpy(c0, c1), "mov c0, c1");
  COMPARE_MACRO_MORELLO(Cpy(c0, csp), "mov c0, csp");
  COMPARE_MACRO_MORELLO(Cpy(csp, c0), "mov csp, c0");
  COMPARE_MACRO_MORELLO(Cpy(csp, csp), "");
  COMPARE_MACRO_MORELLO(Cpy(c30, c30), "");
}

TEST(morello_add_c_cis_c) {
  SETUP();

  // Explicit shift.
  COMPARE_MORELLO(add(c0, c1, 0x6f8, 12), "add c0, c1, #0x6f8000 (7307264)");
  COMPARE_MORELLO(add(c0, c1, 0x903, 0), "add c0, c1, #0x903 (2307)");
  COMPARE_MORELLO(add(csp, c1, 0x6f8, 12), "add csp, c1, #0x6f8000 (7307264)");
  COMPARE_MORELLO(add(c0, csp, 0x903, 0), "add c0, csp, #0x903 (2307)");
  COMPARE_MORELLO(add(c30, c30, 0x6f8, 12),
                  "add c30, c30, #0x6f8000 (7307264)");

  // Implicit shift.
  COMPARE_MORELLO(add(c0, c1, 0x6f8 << 12), "add c0, c1, #0x6f8000 (7307264)");
  COMPARE_MORELLO(add(c0, c1, 0x903), "add c0, c1, #0x903 (2307)");
  COMPARE_MORELLO(add(csp, c1, 0x6f8 << 12),
                  "add csp, c1, #0x6f8000 (7307264)");
  COMPARE_MORELLO(add(c0, csp, 0x903), "add c0, csp, #0x903 (2307)");
  COMPARE_MORELLO(add(c30, c30, 0x6f8 << 12),
                  "add c30, c30, #0x6f8000 (7307264)");

  // Implicit shift (via Operand).
  COMPARE_MORELLO(add(c0, c1, Operand(0x6f8 << 12)),
                  "add c0, c1, #0x6f8000 (7307264)");
  COMPARE_MORELLO(add(c0, c1, Operand(0x903)), "add c0, c1, #0x903 (2307)");
  COMPARE_MORELLO(add(csp, c1, Operand(0x6f8 << 12)),
                  "add csp, c1, #0x6f8000 (7307264)");
  COMPARE_MORELLO(add(c0, csp, Operand(0x903)), "add c0, csp, #0x903 (2307)");
  COMPARE_MORELLO(add(c30, c30, Operand(0x6f8 << 12)),
                  "add c30, c30, #0x6f8000 (7307264)");
}

TEST(morello_add_c_cri_c) {
  SETUP();

  COMPARE_MORELLO(add(c0, c1, Operand(x2, UXTX, 0)), "add c0, c1, x2, uxtx");
  COMPARE_MORELLO(add(c0, c1, Operand(w2, SXTW, 4)), "add c0, c1, x2, sxtw #4");
  COMPARE_MORELLO(add(c0, c1, Operand(x2, UXTB, 0)), "add c0, c1, x2, uxtb");
  COMPARE_MORELLO(add(c0, c1, Operand(w2, SXTH, 4)), "add c0, c1, x2, sxth #4");
  COMPARE_MORELLO(add(csp, c1, Operand(x2, UXTX, 0)), "add csp, c1, x2, uxtx");
  COMPARE_MORELLO(add(c0, csp, Operand(w2, SXTW, 3)),
                  "add c0, csp, x2, sxtw #3");
  COMPARE_MORELLO(add(c0, c1, Operand(xzr, UXTX, 0)), "add c0, c1, xzr, uxtx");
  COMPARE_MORELLO(add(c30, c30, Operand(x30, UXTX, 2)),
                  "add c30, c30, x30, uxtx #2");
}

TEST(morello_add_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_MORELLO(Add(c3, c23, 42), "add c3, c23, #0x2a (42)");
  COMPARE_MACRO_MORELLO(Add(c3, c23, 0x000fff), "add c3, c23, #0xfff (4095)");
  COMPARE_MACRO_MORELLO(Add(c3, c23, 0xfff000),
                        "add c3, c23, #0xfff000 (16773120)");
  COMPARE_MACRO_MORELLO(Add(csp, csp, 42), "add csp, csp, #0x2a (42)");
  COMPARE_MACRO_MORELLO(Add(c3, c23, Operand(x30, UXTX, 0)),
                        "add c3, c23, x30, uxtx");
  COMPARE_MACRO_MORELLO(Add(csp, csp, Operand(xzr, UXTX, 0)),
                        "add csp, csp, xzr, uxtx");

  // Negative immediates are handled by `sub`, if they are encodable.
  COMPARE_MACRO_MORELLO(Add(c3, c23, -1), "sub c3, c23, #0x1 (1)");
  COMPARE_MACRO_MORELLO(Add(c3, c23, -0xfff), "sub c3, c23, #0xfff (4095)");
  COMPARE_MACRO_MORELLO(Add(c3, c23, -0xfff000),
                        "sub c3, c23, #0xfff000 (16773120)");

  // LSL and no-op shifts get translated into UXTX.
  COMPARE_MACRO_MORELLO(Add(c3, c23, Operand(x8, LSL, 3)),
                        "add c3, c23, x8, uxtx #3");
  COMPARE_MACRO_MORELLO(Add(c3, c23, Operand(x8, LSR, 0)),
                        "add c3, c23, x8, uxtx");

  // Other operands are emulated with `Mov`.
  COMPARE_MACRO_MORELLO(Add(c3, c23, 0x001fff),
                        "mov x3, #0x1fff\n"
                        "add c3, c23, x3, uxtx");
  COMPARE_MACRO_MORELLO(Add(c3, c23, 0xfff001),
                        "mov x3, #0xf001\n"
                        "movk x3, #0xff, lsl #16\n"
                        "add c3, c23, x3, uxtx");
  COMPARE_MACRO_MORELLO(Add(c3, c3, Operand(x8, LSR, 1)),
                        "lsr x16, x8, #1\n"
                        "add c3, c3, x16, uxtx");
}

TEST(morello_alignd_c_ci_c) {
  SETUP();

  COMPARE_MORELLO(alignd(c0, c1, 45), "alignd c0, c1, #45");
  COMPARE_MORELLO(alignd(c0, c30, 0), "alignd c0, c30, #0");
  COMPARE_MORELLO(alignd(c30, c1, 0), "alignd c30, c1, #0");
  COMPARE_MORELLO(alignd(c0, csp, 63), "alignd c0, csp, #63");
  COMPARE_MORELLO(alignd(csp, c1, 63), "alignd csp, c1, #63");
  COMPARE_MORELLO(alignd(csp, csp, 45), "alignd csp, csp, #45");

  COMPARE_MACRO_MORELLO(Alignd(c6, c7, 42), "alignd c6, c7, #42");
  COMPARE_MACRO_MORELLO(Alignd(csp, csp, 42), "alignd csp, csp, #42");
}

TEST(morello_alignu_c_ci_c) {
  SETUP();

  COMPARE_MORELLO(alignu(c0, c1, 63), "alignu c0, c1, #63");
  COMPARE_MORELLO(alignu(c0, c30, 0), "alignu c0, c30, #0");
  COMPARE_MORELLO(alignu(c30, c1, 0), "alignu c30, c1, #0");
  COMPARE_MORELLO(alignu(c0, csp, 42), "alignu c0, csp, #42");
  COMPARE_MORELLO(alignu(csp, c1, 42), "alignu csp, c1, #42");
  COMPARE_MORELLO(alignu(csp, csp, 63), "alignu csp, csp, #63");

  COMPARE_MACRO_MORELLO(Alignu(c6, c7, 42), "alignu c6, c7, #42");
  COMPARE_MACRO_MORELLO(Alignu(csp, csp, 42), "alignu csp, csp, #42");
}

TEST(morello_bicflgs_c_ci_c) {
  SETUP();

  COMPARE_MORELLO(bicflgs(c0, c1, 0x00), "bicflgs c0, c1, #0x00");
  COMPARE_MORELLO(bicflgs(c0, c1, 0xad), "bicflgs c0, c1, #0xad");
  COMPARE_MORELLO(bicflgs(c0, c1, 0xff), "bicflgs c0, c1, #0xff");
  COMPARE_MORELLO(bicflgs(c0, csp, 0xad), "bicflgs c0, csp, #0xad");
  COMPARE_MORELLO(bicflgs(csp, c1, 0xad), "bicflgs csp, c1, #0xad");
  COMPARE_MORELLO(bicflgs(c30, c30, 0xad), "bicflgs c30, c30, #0xad");

  COMPARE_MACRO_MORELLO(Bicflgs(c0, c1, 0x42), "bicflgs c0, c1, #0x42");
}

TEST(morello_bicflgs_c_cr_c) {
  SETUP();

  COMPARE_MORELLO(bicflgs(c0, c1, x2), "bicflgs c0, c1, x2");
  COMPARE_MORELLO(bicflgs(c0, c1, xzr), "bicflgs c0, c1, xzr");
  COMPARE_MORELLO(bicflgs(c0, csp, x2), "bicflgs c0, csp, x2");
  COMPARE_MORELLO(bicflgs(csp, c1, x2), "bicflgs csp, c1, x2");
  COMPARE_MORELLO(bicflgs(c30, c29, x28), "bicflgs c30, c29, x28");
  COMPARE_MORELLO(bicflgs(c30, c30, x30), "bicflgs c30, c30, x30");

  COMPARE_MACRO_MORELLO(Bicflgs(c10, c11, x12), "bicflgs c10, c11, x12");
}

TEST(morello_blr_c_c) {
  SETUP();

  COMPARE_MORELLO(blr(c0), "blr c0");
  COMPARE_MORELLO(blr(czr), "blr czr");
  COMPARE_MORELLO(blr(c30), "blr c30");

  COMPARE_MACRO_MORELLO(Blr(c0), "blr c0");
  COMPARE_MACRO_MORELLO(Blr(czr), "blr czr");
}

TEST(morello_blr_ci_c) {
  SETUP();

  // COMPARE_MORELLO(blr(MemOperand(c0, -64)), "TODO");
}

TEST(morello_blrr_c_c) {
  SETUP();

  COMPARE_MORELLO(blrr(c0), "blrr c0");
  COMPARE_MORELLO(blrr(czr), "blrr czr");
  COMPARE_MORELLO(blrr(c30), "blrr c30");

  COMPARE_MACRO_MORELLO(Blrr(c0), "blrr c0");
  COMPARE_MACRO_MORELLO(Blrr(czr), "blrr czr");
}

TEST(morello_blrs_c_c) {
  SETUP();

  COMPARE_MORELLO(blrs(c0), "blrs c0");
  COMPARE_MORELLO(blrs(czr), "blrs czr");
  COMPARE_MORELLO(blrs(c30), "blrs c30");

  COMPARE_MACRO_MORELLO(Blrs(c0), "blrs c0");
  COMPARE_MACRO_MORELLO(Blrs(czr), "blrs czr");
}

TEST(morello_blrs_c_c_c) {
  SETUP();

  // The destination must be c29.
  COMPARE_MORELLO(blrs(c29, c0, c1), "blrs c29, c0, c1");
  COMPARE_MORELLO(blrs(c29, c0, czr), "blrs c29, c0, czr");
  COMPARE_MORELLO(blrs(c29, czr, c1), "blrs c29, czr, c1");
  COMPARE_MORELLO(blrs(c29, c29, c29), "blrs c29, c29, c29");

  COMPARE_MACRO_MORELLO(Blrs(c29, c0, c1), "blrs c29, c0, c1");
  COMPARE_MACRO_MORELLO(Blrs(c29, czr, czr), "blrs c29, czr, czr");
}

TEST(morello_br_c_c) {
  SETUP();

  COMPARE_MORELLO(br(c0), "br c0");
  COMPARE_MORELLO(br(czr), "br czr");
  COMPARE_MORELLO(br(c30), "br c30");

  COMPARE_MACRO_MORELLO(Br(c0), "br c0");
  COMPARE_MACRO_MORELLO(Br(czr), "br czr");
}

TEST(morello_br_ci_c) {
  SETUP();

  // COMPARE_MORELLO(br(MemOperand(c0, 640)), "TODO");
}

TEST(morello_brr_c_c) {
  SETUP();

  COMPARE_MORELLO(brr(c0), "brr c0");
  COMPARE_MORELLO(brr(czr), "brr czr");
  COMPARE_MORELLO(brr(c30), "brr c30");

  COMPARE_MACRO_MORELLO(Brr(c0), "brr c0");
  COMPARE_MACRO_MORELLO(Brr(czr), "brr czr");
}

TEST(morello_brs_c_c) {
  SETUP();

  COMPARE_MORELLO(brs(c0), "brs c0");
  COMPARE_MORELLO(brs(czr), "brs czr");
  COMPARE_MORELLO(brs(c30), "brs c30");

  COMPARE_MACRO_MORELLO(Brs(c0), "brs c0");
  COMPARE_MACRO_MORELLO(Brs(czr), "brs czr");
}

TEST(morello_brs_c_c_c) {
  SETUP();

  // The destination must be c29.
  COMPARE_MORELLO(brs(c29, c0, c1), "brs c29, c0, c1");
  COMPARE_MORELLO(brs(c29, c0, czr), "brs c29, c0, czr");
  COMPARE_MORELLO(brs(c29, czr, c1), "brs c29, czr, c1");
  COMPARE_MORELLO(brs(c29, c29, c29), "brs c29, c29, c29");

  COMPARE_MACRO_MORELLO(Brs(c29, c0, c1), "brs c29, c0, c1");
  COMPARE_MACRO_MORELLO(Brs(c29, czr, czr), "brs c29, czr, czr");
}

TEST(morello_build_c_c_c) {
  SETUP();

  COMPARE_MORELLO(build(c0, c1, c2), "build c0, c1, c2");
  COMPARE_MORELLO(build(c0, c1, csp), "build c0, c1, csp");
  COMPARE_MORELLO(build(c0, csp, c2), "build c0, csp, c2");
  COMPARE_MORELLO(build(csp, c1, c2), "build csp, c1, c2");
  COMPARE_MORELLO(build(c30, c30, c30), "build c30, c30, c30");

  COMPARE_MACRO_MORELLO(Build(c0, c1, c2), "build c0, c1, c2");
  COMPARE_MACRO_MORELLO(Build(csp, csp, csp), "build csp, csp, csp");
}

TEST(morello_bx_c) {
  SETUP();

  COMPARE_MORELLO(bx(4), "bx #4");
  COMPARE_MACRO_MORELLO(Bx(), "bx #4");
}

TEST(morello_cfhi_r_c_c) {
  SETUP();

  COMPARE_MORELLO(cfhi(x14, c15), "cfhi x14, c15");
  COMPARE_MORELLO(cfhi(x14, csp), "cfhi x14, csp");
  COMPARE_MORELLO(cfhi(xzr, c15), "cfhi xzr, c15");
  COMPARE_MORELLO(cfhi(x30, c30), "cfhi x30, c30");

  COMPARE_MACRO_MORELLO(Cfhi(x30, c30), "cfhi x30, c30");
}

TEST(morello_chkeq_cc_c) {
  SETUP();

  COMPARE_MORELLO(chkeq(c0, c1), "chkeq c0, c1");
  COMPARE_MORELLO(chkeq(c0, czr), "chkeq c0, czr");
  COMPARE_MORELLO(chkeq(csp, c1), "chkeq csp, c1");
  COMPARE_MORELLO(chkeq(c30, c30), "chkeq c30, c30");

  COMPARE_MACRO_MORELLO(Chkeq(c0, c1), "chkeq c0, c1");
  COMPARE_MACRO_MORELLO(Chkeq(csp, czr), "chkeq csp, czr");
  COMPARE_MACRO_MORELLO(Chkeq(c29, c28), "chkeq c29, c28");
}

TEST(morello_chksld_c_c) {
  SETUP();

  COMPARE_MORELLO(chksld(c0), "chksld c0");
  COMPARE_MORELLO(chksld(csp), "chksld csp");
  COMPARE_MORELLO(chksld(c30), "chksld c30");

  COMPARE_MACRO_MORELLO(Chksld(c0), "chksld c0");
  COMPARE_MACRO_MORELLO(Chksld(csp), "chksld csp");
}

TEST(morello_chkss_cc_c) {
  SETUP();

  COMPARE_MORELLO(chkss(c0, c1), "chkss c0, c1");
  COMPARE_MORELLO(chkss(c0, csp), "chkss c0, csp");
  COMPARE_MORELLO(chkss(csp, c1), "chkss csp, c1");
  COMPARE_MORELLO(chkss(c30, c30), "chkss c30, c30");

  COMPARE_MACRO_MORELLO(Chkss(c0, c1), "chkss c0, c1");
  COMPARE_MACRO_MORELLO(Chkss(csp, csp), "chkss csp, csp");
  COMPARE_MACRO_MORELLO(Chkss(c29, c28), "chkss c29, c28");
}

TEST(morello_chkssu_c_cc_c) {
  SETUP();

  COMPARE_MORELLO(chkssu(c0, c1, c2), "chkssu c0, c1, c2");
  COMPARE_MORELLO(chkssu(c0, c1, csp), "chkssu c0, c1, csp");
  COMPARE_MORELLO(chkssu(c0, csp, c2), "chkssu c0, csp, c2");
  COMPARE_MORELLO(chkssu(czr, c1, c2), "chkssu czr, c1, c2");
  COMPARE_MORELLO(chkssu(c30, c30, c30), "chkssu c30, c30, c30");

  COMPARE_MACRO_MORELLO(Chkssu(c0, c1, c2), "chkssu c0, c1, c2");
  COMPARE_MACRO_MORELLO(Chkssu(czr, csp, csp), "chkssu czr, csp, csp");
  COMPARE_MACRO_MORELLO(Chkssu(c28, c29, c30), "chkssu c28, c29, c30");
}

TEST(morello_chktgd_c_c) {
  SETUP();

  COMPARE_MORELLO(chktgd(c0), "chktgd c0");
  COMPARE_MORELLO(chktgd(csp), "chktgd csp");
  COMPARE_MORELLO(chktgd(c30), "chktgd c30");

  COMPARE_MACRO_MORELLO(Chktgd(c0), "chktgd c0");
  COMPARE_MACRO_MORELLO(Chktgd(csp), "chktgd csp");
}

TEST(morello_clrperm_c_ci_c) {
  SETUP();

  COMPARE_MORELLO(clrperm(c0, c1, ClrpermImm::None), "clrperm c0, c1, #0");
  COMPARE_MORELLO(clrperm(c0, c1, ClrpermImm::X), "clrperm c0, c1, X");
  COMPARE_MORELLO(clrperm(c0, c1, ClrpermImm::W), "clrperm c0, c1, W");
  COMPARE_MORELLO(clrperm(c0, c1, ClrpermImm::WX), "clrperm c0, c1, WX");
  COMPARE_MORELLO(clrperm(c0, c1, ClrpermImm::R), "clrperm c0, c1, R");
  COMPARE_MORELLO(clrperm(c0, c1, ClrpermImm::RX), "clrperm c0, c1, RX");
  COMPARE_MORELLO(clrperm(c0, c1, ClrpermImm::RW), "clrperm c0, c1, RW");
  COMPARE_MORELLO(clrperm(c0, c1, ClrpermImm::RWX), "clrperm c0, c1, RWX");
  COMPARE_MORELLO(clrperm(c0, csp, ClrpermImm::None), "clrperm c0, csp, #0");
  COMPARE_MORELLO(clrperm(csp, c1, ClrpermImm::None), "clrperm csp, c1, #0");
  COMPARE_MORELLO(clrperm(c30, c30, ClrpermImm::None), "clrperm c30, c30, #0");

  COMPARE_MACRO_MORELLO(Clrperm(c14, c5, ClrpermImm::RWX),
                        "clrperm c14, c5, RWX");
  COMPARE_MACRO_MORELLO(Clrperm(csp, csp, ClrpermImm::RWX),
                        "clrperm csp, csp, RWX");
}

TEST(morello_clrperm_c_cr_c) {
  SETUP();

  COMPARE_MORELLO(clrperm(c18, c19, x20), "clrperm c18, c19, x20");
  COMPARE_MORELLO(clrperm(c18, c19, xzr), "clrperm c18, c19, xzr");
  COMPARE_MORELLO(clrperm(c18, csp, x20), "clrperm c18, csp, x20");
  COMPARE_MORELLO(clrperm(csp, c19, x20), "clrperm csp, c19, x20");
  COMPARE_MORELLO(clrperm(c30, c30, x30), "clrperm c30, c30, x30");

  COMPARE_MACRO_MORELLO(Clrperm(c18, c19, x20), "clrperm c18, c19, x20");
  COMPARE_MACRO_MORELLO(Clrperm(c18, c19, w20), "clrperm c18, c19, x20");
  COMPARE_MACRO_MORELLO(Clrperm(csp, csp, xzr), "clrperm csp, csp, xzr");
  COMPARE_MACRO_MORELLO(Clrperm(csp, csp, wzr), "clrperm csp, csp, xzr");
  COMPARE_MACRO_MORELLO(Clrperm(c0, c0, x0), "clrperm c0, c0, x0");
}

TEST(morello_clrtag_c_c_c) {
  SETUP();

  COMPARE_MORELLO(clrtag(c0, c1), "clrtag c0, c1");
  COMPARE_MORELLO(clrtag(c0, csp), "clrtag c0, csp");
  COMPARE_MORELLO(clrtag(csp, c1), "clrtag csp, c1");
  COMPARE_MORELLO(clrtag(c30, c30), "clrtag c30, c30");

  COMPARE_MACRO_MORELLO(Clrtag(c0, c1), "clrtag c0, c1");
  COMPARE_MACRO_MORELLO(Clrtag(csp, csp), "clrtag csp, csp");
  COMPARE_MACRO_MORELLO(Clrtag(c30, c30), "clrtag c30, c30");
}

TEST(morello_cpy_c_c_c) {
  SETUP();

  // `cpy` always disassembles as `mov`.
  COMPARE_MORELLO(cpy(c0, c1), "mov c0, c1");
  COMPARE_MORELLO(cpy(c0, csp), "mov c0, csp");
  COMPARE_MORELLO(cpy(csp, c1), "mov csp, c1");
  COMPARE_MORELLO(cpy(c30, c30), "mov c30, c30");

  // The MacroAssembler is tested in `morello_mov_cpy_macro`.
}

TEST(morello_cpytype_c_c_c) {
  SETUP();

  COMPARE_MORELLO(cpytype(c0, c1, c2), "cpytype c0, c1, c2");
  COMPARE_MORELLO(cpytype(c0, c1, czr), "cpytype c0, c1, czr");
  COMPARE_MORELLO(cpytype(c0, czr, c2), "cpytype c0, czr, c2");
  COMPARE_MORELLO(cpytype(czr, c1, c2), "cpytype czr, c1, c2");
  COMPARE_MORELLO(cpytype(c30, c30, c30), "cpytype c30, c30, c30");

  COMPARE_MACRO_MORELLO(Cpytype(c0, c1, c2), "cpytype c0, c1, c2");
  COMPARE_MACRO_MORELLO(Cpytype(czr, czr, czr), "cpytype czr, czr, czr");
}

TEST(morello_cpyvalue_c_c_c) {
  SETUP();

  COMPARE_MORELLO(cpyvalue(c0, c1, c2), "cpyvalue c0, c1, c2");
  COMPARE_MORELLO(cpyvalue(c0, c1, czr), "cpyvalue c0, c1, czr");
  COMPARE_MORELLO(cpyvalue(c0, czr, c2), "cpyvalue c0, czr, c2");
  COMPARE_MORELLO(cpyvalue(czr, c1, c2), "cpyvalue czr, c1, c2");
  COMPARE_MORELLO(cpyvalue(c30, c30, c30), "cpyvalue c30, c30, c30");

  COMPARE_MACRO_MORELLO(Cpyvalue(c0, c1, c2), "cpyvalue c0, c1, c2");
  COMPARE_MACRO_MORELLO(Cpyvalue(czr, czr, czr), "cpyvalue czr, czr, czr");
}

TEST(morello_cseal_c_c_c) {
  SETUP();

  COMPARE_MORELLO(cseal(c0, c1, c2), "cseal c0, c1, c2");
  COMPARE_MORELLO(cseal(c0, c1, csp), "cseal c0, c1, csp");
  COMPARE_MORELLO(cseal(c0, csp, c2), "cseal c0, csp, c2");
  COMPARE_MORELLO(cseal(csp, c1, c2), "cseal csp, c1, c2");
  COMPARE_MORELLO(cseal(c30, c30, c30), "cseal c30, c30, c30");

  COMPARE_MACRO_MORELLO(Cseal(c0, c1, c2), "cseal c0, c1, c2");
  COMPARE_MACRO_MORELLO(Cseal(csp, csp, csp), "cseal csp, csp, csp");
}

TEST(morello_csel_c_ci_c) {
  SETUP();

  COMPARE_MORELLO(csel(c0, c1, c2, pl), "csel c0, c1, c2, pl");
  COMPARE_MORELLO(csel(c0, c1, c2, eq), "csel c0, c1, c2, eq");
  COMPARE_MORELLO(csel(c0, c1, c2, nv), "csel c0, c1, c2, nv");
  COMPARE_MORELLO(csel(c0, c1, czr, pl), "csel c0, c1, czr, pl");
  COMPARE_MORELLO(csel(c0, czr, c2, pl), "csel c0, czr, c2, pl");
  COMPARE_MORELLO(csel(czr, c1, c2, pl), "csel czr, c1, c2, pl");
  COMPARE_MORELLO(csel(c30, c30, c30, pl), "csel c30, c30, c30, pl");

  COMPARE_MACRO_MORELLO(Csel(c0, c1, c2, pl), "csel c0, c1, c2, pl");
  COMPARE_MACRO_MORELLO(Csel(czr, czr, czr, pl), "csel czr, czr, czr, pl");
  COMPARE_MACRO_MORELLO(Csel(c30, c30, c30, pl), "csel c30, c30, c30, pl");
}

TEST(morello_cthi_c_cr_c) {
  SETUP();

  COMPARE_MORELLO(cthi(c0, c1, x2), "cthi c0, c1, x2");
  COMPARE_MORELLO(cthi(c0, c1, xzr), "cthi c0, c1, xzr");
  COMPARE_MORELLO(cthi(c0, czr, x2), "cthi c0, czr, x2");  // <Cn> takes xzr.
  COMPARE_MORELLO(cthi(csp, c1, x2), "cthi csp, c1, x2");
  COMPARE_MORELLO(cthi(c30, c29, x28), "cthi c30, c29, x28");
  COMPARE_MORELLO(cthi(c30, c30, x30), "cthi c30, c30, x30");

  COMPARE_MACRO_MORELLO(Cthi(c10, c11, x12), "cthi c10, c11, x12");
}

TEST(morello_cvt_c_cr_c) {
  SETUP();

  COMPARE_MORELLO(cvt(c25, c13, x0), "cvt c25, c13, x0");
  COMPARE_MORELLO(cvt(c25, c13, xzr), "cvt c25, c13, xzr");
  COMPARE_MORELLO(cvt(c25, csp, x0), "cvt c25, csp, x0");
  COMPARE_MORELLO(cvt(czr, c13, x0), "cvt czr, c13, x0");
  COMPARE_MORELLO(cvt(c30, c30, x30), "cvt c30, c30, x30");

  COMPARE_MACRO_MORELLO(Cvt(c25, c13, x0), "cvt c25, c13, x0");
  COMPARE_MACRO_MORELLO(Cvt(c30, c30, x30), "cvt c30, c30, x30");
}

TEST(morello_cvt_r_cc_c) {
  SETUP();

  COMPARE_MORELLO(cvt(x10, c21, c11), "cvt x10, c21, c11");
  COMPARE_MORELLO(cvt(x10, c21, czr), "cvt x10, c21, czr");
  COMPARE_MORELLO(cvt(x10, csp, c11), "cvt x10, csp, c11");
  COMPARE_MORELLO(cvt(xzr, c21, c11), "cvt xzr, c21, c11");
  COMPARE_MORELLO(cvt(x30, c30, c30), "cvt x30, c30, c30");

  COMPARE_MACRO_MORELLO(Cvt(x10, c21, c11), "cvt x10, c21, c11");
  COMPARE_MACRO_MORELLO(Cvt(xzr, csp, czr), "cvt xzr, csp, czr");
  COMPARE_MACRO_MORELLO(Cvt(x29, c28, c27), "cvt x29, c28, c27");
}

TEST(morello_cvtd_c_r_c) {
  SETUP();

  // <Xn> cannot be `sp` for these instrutions.
  COMPARE_MORELLO(cvtd(c0, x1), "cvtd c0, x1");
  COMPARE_MORELLO(cvtd(c0, xzr), "cvtd c0, xzr");
  COMPARE_MORELLO(cvtd(czr, x1), "cvtd czr, x1");
  COMPARE_MORELLO(cvtd(c30, x30), "cvtd c30, x30");

  COMPARE_MACRO_MORELLO(Cvtd(c0, x1), "cvtd c0, x1");
  COMPARE_MACRO_MORELLO(Cvtd(czr, xzr), "cvtd czr, xzr");
  COMPARE_MACRO_MORELLO(Cvtd(c21, x21), "cvtd c21, x21");
}

TEST(morello_cvtd_r_c_c) {
  SETUP();

  COMPARE_MORELLO(cvtd(x21, c0), "cvtd x21, c0");
  COMPARE_MORELLO(cvtd(x21, csp), "cvtd x21, csp");
  COMPARE_MORELLO(cvtd(xzr, c0), "cvtd xzr, c0");
  COMPARE_MORELLO(cvtd(x30, c30), "cvtd x30, c30");

  COMPARE_MACRO_MORELLO(Cvtd(x21, c0), "cvtd x21, c0");
  COMPARE_MACRO_MORELLO(Cvtd(xzr, csp), "cvtd xzr, csp");
  COMPARE_MACRO_MORELLO(Cvtd(x29, c29), "cvtd x29, c29");
}

TEST(morello_cvtdz_c_r_c) {
  SETUP();

  // <Xn> cannot be `sp` for these instrutions.
  COMPARE_MORELLO(cvtdz(c0, x1), "cvtdz c0, x1");
  COMPARE_MORELLO(cvtdz(c0, xzr), "cvtdz c0, xzr");
  COMPARE_MORELLO(cvtdz(czr, x1), "cvtdz czr, x1");
  COMPARE_MORELLO(cvtdz(c30, x30), "cvtdz c30, x30");

  COMPARE_MACRO_MORELLO(Cvtdz(c0, x1), "cvtdz c0, x1");
  COMPARE_MACRO_MORELLO(Cvtdz(czr, xzr), "cvtdz czr, xzr");
  COMPARE_MACRO_MORELLO(Cvtdz(c21, x21), "cvtdz c21, x21");
}

TEST(morello_cvtp_c_r_c) {
  SETUP();

  // <Xn> cannot be `sp` for these instrutions.
  COMPARE_MORELLO(cvtp(c0, x1), "cvtp c0, x1");
  COMPARE_MORELLO(cvtp(c0, xzr), "cvtp c0, xzr");
  COMPARE_MORELLO(cvtp(czr, x1), "cvtp czr, x1");
  COMPARE_MORELLO(cvtp(c30, x30), "cvtp c30, x30");

  COMPARE_MACRO_MORELLO(Cvtp(c0, x1), "cvtp c0, x1");
  COMPARE_MACRO_MORELLO(Cvtp(czr, xzr), "cvtp czr, xzr");
  COMPARE_MACRO_MORELLO(Cvtp(c21, x21), "cvtp c21, x21");
}

TEST(morello_cvtp_r_c_c) {
  SETUP();

  COMPARE_MORELLO(cvtp(x21, c0), "cvtp x21, c0");
  COMPARE_MORELLO(cvtp(x21, csp), "cvtp x21, csp");
  COMPARE_MORELLO(cvtp(xzr, c0), "cvtp xzr, c0");
  COMPARE_MORELLO(cvtp(x30, c30), "cvtp x30, c30");

  COMPARE_MACRO_MORELLO(Cvtp(x21, c0), "cvtp x21, c0");
  COMPARE_MACRO_MORELLO(Cvtp(xzr, csp), "cvtp xzr, csp");
  COMPARE_MACRO_MORELLO(Cvtp(x29, c29), "cvtp x29, c29");
}

TEST(morello_cvtpz_c_r_c) {
  SETUP();

  // <Xn> cannot be `sp` for these instrutions.
  COMPARE_MORELLO(cvtpz(c0, x1), "cvtpz c0, x1");
  COMPARE_MORELLO(cvtpz(c0, xzr), "cvtpz c0, xzr");
  COMPARE_MORELLO(cvtpz(czr, x1), "cvtpz czr, x1");
  COMPARE_MORELLO(cvtpz(c30, x30), "cvtpz c30, x30");

  COMPARE_MACRO_MORELLO(Cvtpz(c0, x1), "cvtpz c0, x1");
  COMPARE_MACRO_MORELLO(Cvtpz(czr, xzr), "cvtpz czr, xzr");
  COMPARE_MACRO_MORELLO(Cvtpz(c21, x21), "cvtpz c21, x21");
}

TEST(morello_cvtz_c_cr_c) {
  SETUP();

  COMPARE_MORELLO(cvtz(c25, c13, x0), "cvtz c25, c13, x0");
  COMPARE_MORELLO(cvtz(c25, c13, xzr), "cvtz c25, c13, xzr");
  COMPARE_MORELLO(cvtz(c25, csp, x0), "cvtz c25, csp, x0");
  COMPARE_MORELLO(cvtz(czr, c13, x0), "cvtz czr, c13, x0");
  COMPARE_MORELLO(cvtz(c30, c30, x30), "cvtz c30, c30, x30");

  COMPARE_MACRO_MORELLO(Cvtz(c25, c13, x0), "cvtz c25, c13, x0");
  COMPARE_MACRO_MORELLO(Cvtz(c30, c30, x30), "cvtz c30, c30, x30");
}

TEST(morello_eorflgs_c_ci_c) {
  SETUP();

  COMPARE_MORELLO(eorflgs(c0, c1, 0x00), "eorflgs c0, c1, #0x00");
  COMPARE_MORELLO(eorflgs(c0, c1, 0xad), "eorflgs c0, c1, #0xad");
  COMPARE_MORELLO(eorflgs(c0, c1, 0xff), "eorflgs c0, c1, #0xff");
  COMPARE_MORELLO(eorflgs(c0, csp, 0xad), "eorflgs c0, csp, #0xad");
  COMPARE_MORELLO(eorflgs(csp, c1, 0xad), "eorflgs csp, c1, #0xad");
  COMPARE_MORELLO(eorflgs(c30, c30, 0xad), "eorflgs c30, c30, #0xad");

  COMPARE_MACRO_MORELLO(Eorflgs(c0, c1, 0x42), "eorflgs c0, c1, #0x42");
}

TEST(morello_eorflgs_c_cr_c) {
  SETUP();

  COMPARE_MORELLO(eorflgs(c0, c1, x2), "eorflgs c0, c1, x2");
  COMPARE_MORELLO(eorflgs(c0, c1, xzr), "eorflgs c0, c1, xzr");
  COMPARE_MORELLO(eorflgs(c0, csp, x2), "eorflgs c0, csp, x2");
  COMPARE_MORELLO(eorflgs(csp, c1, x2), "eorflgs csp, c1, x2");
  COMPARE_MORELLO(eorflgs(c30, c29, x28), "eorflgs c30, c29, x28");
  COMPARE_MORELLO(eorflgs(c30, c30, x30), "eorflgs c30, c30, x30");

  COMPARE_MACRO_MORELLO(Eorflgs(c10, c11, x12), "eorflgs c10, c11, x12");
}

TEST(morello_gcbase_r_c_c) {
  SETUP();

  COMPARE_MORELLO(gcbase(x15, c13), "gcbase x15, c13");
  COMPARE_MORELLO(gcbase(x15, csp), "gcbase x15, csp");
  COMPARE_MORELLO(gcbase(xzr, c13), "gcbase xzr, c13");
  COMPARE_MORELLO(gcbase(x30, c30), "gcbase x30, c30");

  COMPARE_MACRO_MORELLO(Gcbase(x30, c30), "gcbase x30, c30");
}

TEST(morello_gcflgs_r_c_c) {
  SETUP();

  COMPARE_MORELLO(gcflgs(x15, c13), "gcflgs x15, c13");
  COMPARE_MORELLO(gcflgs(x15, csp), "gcflgs x15, csp");
  COMPARE_MORELLO(gcflgs(xzr, c13), "gcflgs xzr, c13");
  COMPARE_MORELLO(gcflgs(x30, c30), "gcflgs x30, c30");

  COMPARE_MACRO_MORELLO(Gcflgs(x30, c30), "gcflgs x30, c30");
}

TEST(morello_gclen_r_c_c) {
  SETUP();

  COMPARE_MORELLO(gclen(x15, c13), "gclen x15, c13");
  COMPARE_MORELLO(gclen(x15, csp), "gclen x15, csp");
  COMPARE_MORELLO(gclen(xzr, c13), "gclen xzr, c13");
  COMPARE_MORELLO(gclen(x30, c30), "gclen x30, c30");

  COMPARE_MACRO_MORELLO(Gclen(x30, c30), "gclen x30, c30");
}

TEST(morello_gclim_r_c_c) {
  SETUP();

  COMPARE_MORELLO(gclim(x15, c13), "gclim x15, c13");
  COMPARE_MORELLO(gclim(x15, csp), "gclim x15, csp");
  COMPARE_MORELLO(gclim(xzr, c13), "gclim xzr, c13");
  COMPARE_MORELLO(gclim(x30, c30), "gclim x30, c30");

  COMPARE_MACRO_MORELLO(Gclim(x30, c30), "gclim x30, c30");
}

TEST(morello_gcoff_r_c_c) {
  SETUP();

  COMPARE_MORELLO(gcoff(x15, c13), "gcoff x15, c13");
  COMPARE_MORELLO(gcoff(x15, csp), "gcoff x15, csp");
  COMPARE_MORELLO(gcoff(xzr, c13), "gcoff xzr, c13");
  COMPARE_MORELLO(gcoff(x30, c30), "gcoff x30, c30");

  COMPARE_MACRO_MORELLO(Gcoff(x30, c30), "gcoff x30, c30");
}

TEST(morello_gcperm_r_c_c) {
  SETUP();

  COMPARE_MORELLO(gcperm(x15, c13), "gcperm x15, c13");
  COMPARE_MORELLO(gcperm(x15, csp), "gcperm x15, csp");
  COMPARE_MORELLO(gcperm(xzr, c13), "gcperm xzr, c13");
  COMPARE_MORELLO(gcperm(x30, c30), "gcperm x30, c30");

  COMPARE_MACRO_MORELLO(Gcperm(x15, c13), "gcperm x15, c13");
  COMPARE_MACRO_MORELLO(Gcperm(w15, c13), "gcperm x15, c13");
  COMPARE_MACRO_MORELLO(Gcperm(w30, c30), "gcperm x30, c30");
  COMPARE_MACRO_MORELLO(Gcperm(xzr, c13), "gcperm xzr, c13");
  COMPARE_MACRO_MORELLO(Gcperm(wzr, c13), "gcperm xzr, c13");
  COMPARE_MACRO_MORELLO(Gcperm(wzr, csp), "gcperm xzr, csp");
}

TEST(morello_gcseal_r_c_c) {
  SETUP();

  COMPARE_MORELLO(gcseal(x15, c13), "gcseal x15, c13");
  COMPARE_MORELLO(gcseal(x15, csp), "gcseal x15, csp");
  COMPARE_MORELLO(gcseal(xzr, c13), "gcseal xzr, c13");
  COMPARE_MORELLO(gcseal(x30, c30), "gcseal x30, c30");

  COMPARE_MACRO_MORELLO(Gcseal(x15, c13), "gcseal x15, c13");
  COMPARE_MACRO_MORELLO(Gcseal(w15, c13), "gcseal x15, c13");
  COMPARE_MACRO_MORELLO(Gcseal(w30, c30), "gcseal x30, c30");
  COMPARE_MACRO_MORELLO(Gcseal(xzr, c13), "gcseal xzr, c13");
  COMPARE_MACRO_MORELLO(Gcseal(wzr, c13), "gcseal xzr, c13");
  COMPARE_MACRO_MORELLO(Gcseal(wzr, csp), "gcseal xzr, csp");
}

TEST(morello_gctag_r_c_c) {
  SETUP();

  COMPARE_MORELLO(gctag(x15, c13), "gctag x15, c13");
  COMPARE_MORELLO(gctag(x15, csp), "gctag x15, csp");
  COMPARE_MORELLO(gctag(xzr, c13), "gctag xzr, c13");
  COMPARE_MORELLO(gctag(x30, c30), "gctag x30, c30");

  COMPARE_MACRO_MORELLO(Gctag(x15, c13), "gctag x15, c13");
  COMPARE_MACRO_MORELLO(Gctag(w15, c13), "gctag x15, c13");
  COMPARE_MACRO_MORELLO(Gctag(w30, c30), "gctag x30, c30");
  COMPARE_MACRO_MORELLO(Gctag(xzr, c13), "gctag xzr, c13");
  COMPARE_MACRO_MORELLO(Gctag(wzr, c13), "gctag xzr, c13");
  COMPARE_MACRO_MORELLO(Gctag(wzr, csp), "gctag xzr, csp");
}

TEST(morello_gctype_r_c_c) {
  SETUP();

  COMPARE_MORELLO(gctype(x15, c13), "gctype x15, c13");
  COMPARE_MORELLO(gctype(x15, csp), "gctype x15, csp");
  COMPARE_MORELLO(gctype(xzr, c13), "gctype xzr, c13");
  COMPARE_MORELLO(gctype(x30, c30), "gctype x30, c30");

  COMPARE_MACRO_MORELLO(Gctype(x15, c13), "gctype x15, c13");
  COMPARE_MACRO_MORELLO(Gctype(w15, c13), "gctype x15, c13");
  COMPARE_MACRO_MORELLO(Gctype(w30, c30), "gctype x30, c30");
  COMPARE_MACRO_MORELLO(Gctype(xzr, c13), "gctype xzr, c13");
  COMPARE_MACRO_MORELLO(Gctype(wzr, c13), "gctype xzr, c13");
  COMPARE_MACRO_MORELLO(Gctype(wzr, csp), "gctype xzr, csp");
}

TEST(morello_gcvalue_r_c_c) {
  SETUP();

  COMPARE_MORELLO(gcvalue(x15, c13), "gcvalue x15, c13");
  COMPARE_MORELLO(gcvalue(x15, csp), "gcvalue x15, csp");
  COMPARE_MORELLO(gcvalue(xzr, c13), "gcvalue xzr, c13");
  COMPARE_MORELLO(gcvalue(x30, c30), "gcvalue x30, c30");

  COMPARE_MACRO_MORELLO(Gcvalue(x30, c30), "gcvalue x30, c30");
}

TEST(morello_mrs_c_i_c) {
  SETUP();

  COMPARE_MORELLO(mrs(c0, CCTLR_EL0), "mrs c0, cctlr_el0");
  COMPARE_MORELLO(mrs(c0, CID_EL0), "mrs c0, cid_el0");
  COMPARE_MORELLO(mrs(c0, DDC), "mrs c0, ddc");
  COMPARE_MORELLO(mrs(c0, RCSP_EL0), "mrs c0, rcsp_el0");
  COMPARE_MORELLO(mrs(c0, RDDC_EL0), "mrs c0, rddc_el0");
  COMPARE_MORELLO(mrs(c30, DDC), "mrs c30, ddc");
  COMPARE_MORELLO(mrs(czr, DDC), "mrs czr, ddc");

  COMPARE_MACRO_MORELLO(Mrs(c0, CID_EL0), "mrs c0, cid_el0");
  COMPARE_MACRO_MORELLO(Mrs(c30, CID_EL0), "mrs c30, cid_el0");
  COMPARE_MACRO_MORELLO(Mrs(czr, CID_EL0), "mrs czr, cid_el0");

  // Test disassembly of registers that VIXL doesn't name.
  COMPARE(dci(MRS_c_i | (0b1'011'0001'0010'111 << 5)), "mrs c0, S3_3_c1_c2_7");
}

TEST(morello_msr_c_i_c) {
  SETUP();

  COMPARE_MORELLO(msr(CCTLR_EL0, c0), "msr cctlr_el0, c0");
  COMPARE_MORELLO(msr(CID_EL0, c0), "msr cid_el0, c0");
  COMPARE_MORELLO(msr(DDC, c0), "msr ddc, c0");
  COMPARE_MORELLO(msr(RCSP_EL0, c0), "msr rcsp_el0, c0");
  COMPARE_MORELLO(msr(RDDC_EL0, c0), "msr rddc_el0, c0");
  COMPARE_MORELLO(msr(DDC, c30), "msr ddc, c30");
  COMPARE_MORELLO(msr(DDC, czr), "msr ddc, czr");

  COMPARE_MACRO_MORELLO(Msr(CID_EL0, c0), "msr cid_el0, c0");
  COMPARE_MACRO_MORELLO(Msr(CID_EL0, c30), "msr cid_el0, c30");
  COMPARE_MACRO_MORELLO(Msr(CID_EL0, czr), "msr cid_el0, czr");

  // Test disassembly of registers that VIXL doesn't name.
  COMPARE(dci(MSR_c_i | (0b1'011'0001'0010'111 << 5)), "msr S3_3_c1_c2_7, c0");
}

TEST(morello_mrs_msr_x) {
  // X-sized accesses to Morello system registers.
  SETUP();

  COMPARE(mrs(x15, RSP_EL0), "mrs x15, rsp_el0");
  COMPARE(msr(RSP_EL0, x15), "msr rsp_el0, x15");

  COMPARE_MACRO(Mrs(x15, RSP_EL0), "mrs x15, rsp_el0");
  COMPARE_MACRO(Msr(RSP_EL0, x15), "msr rsp_el0, x15");
}

TEST(morello_orrflgs_c_ci_c) {
  SETUP();

  COMPARE_MORELLO(orrflgs(c0, c1, 0x00), "orrflgs c0, c1, #0x00");
  COMPARE_MORELLO(orrflgs(c0, c1, 0xad), "orrflgs c0, c1, #0xad");
  COMPARE_MORELLO(orrflgs(c0, c1, 0xff), "orrflgs c0, c1, #0xff");
  COMPARE_MORELLO(orrflgs(c0, csp, 0xad), "orrflgs c0, csp, #0xad");
  COMPARE_MORELLO(orrflgs(csp, c1, 0xad), "orrflgs csp, c1, #0xad");
  COMPARE_MORELLO(orrflgs(c30, c30, 0xad), "orrflgs c30, c30, #0xad");

  COMPARE_MACRO_MORELLO(Orrflgs(c0, c1, 0x42), "orrflgs c0, c1, #0x42");
}

TEST(morello_orrflgs_c_cr_c) {
  SETUP();

  COMPARE_MORELLO(orrflgs(c0, c1, x2), "orrflgs c0, c1, x2");
  COMPARE_MORELLO(orrflgs(c0, c1, xzr), "orrflgs c0, c1, xzr");
  COMPARE_MORELLO(orrflgs(c0, csp, x2), "orrflgs c0, csp, x2");
  COMPARE_MORELLO(orrflgs(csp, c1, x2), "orrflgs csp, c1, x2");
  COMPARE_MORELLO(orrflgs(c30, c29, x28), "orrflgs c30, c29, x28");
  COMPARE_MORELLO(orrflgs(c30, c30, x30), "orrflgs c30, c30, x30");

  COMPARE_MACRO_MORELLO(Orrflgs(c10, c11, x12), "orrflgs c10, c11, x12");
}

TEST(morello_ret_c_c) {
  SETUP();

  COMPARE_MORELLO(ret(c0), "ret c0");
  COMPARE_MORELLO(ret(czr), "ret czr");
  COMPARE_MORELLO(ret(c30), "ret c30");

  COMPARE_MACRO_MORELLO(Ret(c0), "ret c0");
  COMPARE_MACRO_MORELLO(Ret(czr), "ret czr");

  // If no register is specified, the behaviour depends on the target ISA. See
  // the ISA-specific `ret_default` tests for details.
}

TEST(morello_retr_c_c) {
  SETUP();

  COMPARE_MORELLO(retr(c0), "retr c0");
  COMPARE_MORELLO(retr(czr), "retr czr");
  COMPARE_MORELLO(retr(c30), "retr c30");
  COMPARE_MORELLO(retr(), "retr c30");

  COMPARE_MACRO_MORELLO(Retr(c0), "retr c0");
  COMPARE_MACRO_MORELLO(Retr(czr), "retr czr");
  COMPARE_MACRO_MORELLO(Retr(), "retr c30");
}

TEST(morello_rets_c_c) {
  SETUP();

  COMPARE_MORELLO(rets(c0), "rets c0");
  COMPARE_MORELLO(rets(czr), "rets czr");
  COMPARE_MORELLO(rets(c30), "rets c30");
  COMPARE_MORELLO(rets(), "rets c30");

  COMPARE_MACRO_MORELLO(Rets(c0), "rets c0");
  COMPARE_MACRO_MORELLO(Rets(czr), "rets czr");
  COMPARE_MACRO_MORELLO(Rets(), "rets c30");
}

TEST(morello_rets_c_c_c) {
  SETUP();

  // The destination must be c29.
  COMPARE_MORELLO(rets(c29, c0, c1), "rets c29, c0, c1");
  COMPARE_MORELLO(rets(c29, c0, czr), "rets c29, c0, czr");
  COMPARE_MORELLO(rets(c29, czr, c1), "rets c29, czr, c1");
  COMPARE_MORELLO(rets(c29, c29, c29), "rets c29, c29, c29");

  COMPARE_MACRO_MORELLO(Rets(c29, c0, c1), "rets c29, c0, c1");
  COMPARE_MACRO_MORELLO(Rets(c29, czr, czr), "rets c29, czr, czr");
}

TEST(morello_rrlen_r_r_c) {
  SETUP();

  COMPARE_MORELLO(rrlen(x21, x20), "rrlen x21, x20");
  COMPARE_MORELLO(rrlen(x21, xzr), "rrlen x21, xzr");
  COMPARE_MORELLO(rrlen(xzr, x20), "rrlen xzr, x20");
  COMPARE_MORELLO(rrlen(x0, x0), "rrlen x0, x0");

  COMPARE_MACRO_MORELLO(Rrlen(x21, x20), "rrlen x21, x20");
  COMPARE_MACRO_MORELLO(Rrlen(xzr, xzr), "rrlen xzr, xzr");
}

TEST(morello_rrmask_r_r_c) {
  SETUP();

  COMPARE_MORELLO(rrmask(x21, x20), "rrmask x21, x20");
  COMPARE_MORELLO(rrmask(x21, xzr), "rrmask x21, xzr");
  COMPARE_MORELLO(rrmask(xzr, x20), "rrmask xzr, x20");
  COMPARE_MORELLO(rrmask(x0, x0), "rrmask x0, x0");

  COMPARE_MACRO_MORELLO(Rrmask(x21, x20), "rrmask x21, x20");
  COMPARE_MACRO_MORELLO(Rrmask(xzr, xzr), "rrmask xzr, xzr");
}

TEST(morello_scbnds_c_ci_c) {
  SETUP();

  COMPARE_MORELLO(scbnds(c0, c1, 42), "scbnds c0, c1, #42");
  COMPARE_MORELLO(scbnds(c0, c1, 42, 0), "scbnds c0, c1, #42");
  COMPARE_MORELLO(scbnds(c0, csp, 42), "scbnds c0, csp, #42");
  COMPARE_MORELLO(scbnds(csp, c1, 42), "scbnds csp, c1, #42");
  COMPARE_MORELLO(scbnds(c30, c30, 42), "scbnds c30, c30, #42");
  COMPARE_MORELLO(scbnds(c0, c1, 0), "scbnds c0, c1, #0");
  COMPARE_MORELLO(scbnds(c0, c1, 0x3f), "scbnds c0, c1, #63");
}

TEST(morello_scbnds_c_ci_s) {
  SETUP();

  COMPARE_MORELLO(scbnds(c0, c1, 42, 4), "scbnds c0, c1, #42, lsl #4 (672)");
  COMPARE_MORELLO(scbnds(c0, c1, 42, 4), "scbnds c0, c1, #42, lsl #4 (672)");
  COMPARE_MORELLO(scbnds(c0, csp, 42, 4), "scbnds c0, csp, #42, lsl #4 (672)");
  COMPARE_MORELLO(scbnds(csp, c1, 42, 4), "scbnds csp, c1, #42, lsl #4 (672)");
  COMPARE_MORELLO(scbnds(c30, c30, 42, 4),
                  "scbnds c30, c30, #42, lsl #4 (672)");
  COMPARE_MORELLO(scbnds(c0, c1, 0, 4), "scbnds c0, c1, #0, lsl #4 (0)");
  COMPARE_MORELLO(scbnds(c0, c1, 0x3f, 4), "scbnds c0, c1, #63, lsl #4 (1008)");
}

TEST(morello_scbnds_c_cr_c) {
  SETUP();

  COMPARE_MORELLO(scbnds(c18, c19, x20), "scbnds c18, c19, x20");
  COMPARE_MORELLO(scbnds(c18, c19, xzr), "scbnds c18, c19, xzr");
  COMPARE_MORELLO(scbnds(c18, csp, x20), "scbnds c18, csp, x20");
  COMPARE_MORELLO(scbnds(csp, c19, x20), "scbnds csp, c19, x20");
  COMPARE_MORELLO(scbnds(c30, c30, x30), "scbnds c30, c30, x30");
}

TEST(morello_scbnds_macro) {
  SETUP();

  // Encodable immediates.
  COMPARE_MACRO_MORELLO(Scbnds(c0, c1, 0), "scbnds c0, c1, #0");
  COMPARE_MACRO_MORELLO(Scbnds(c0, c1, 42), "scbnds c0, c1, #42");
  COMPARE_MACRO_MORELLO(Scbnds(c0, c1, 48), "scbnds c0, c1, #48");
  COMPARE_MACRO_MORELLO(Scbnds(c0, c1, 63), "scbnds c0, c1, #63");
  COMPARE_MACRO_MORELLO(Scbnds(c0, c1, 4 << 4),
                        "scbnds c0, c1, #4, lsl #4 (64)");
  COMPARE_MACRO_MORELLO(Scbnds(c0, c1, 32 << 4),
                        "scbnds c0, c1, #32, lsl #4 (512)");
  COMPARE_MACRO_MORELLO(Scbnds(c0, c1, 63 << 4),
                        "scbnds c0, c1, #63, lsl #4 (1008)");
  COMPARE_MACRO_MORELLO(Scbnds(csp, csp, 63), "scbnds csp, csp, #63");
  COMPARE_MACRO_MORELLO(Scbnds(c30, c30, 63), "scbnds c30, c30, #63");
  COMPARE_MACRO_MORELLO(Scbnds(csp, csp, 63 << 4),
                        "scbnds csp, csp, #63, lsl #4 (1008)");
  COMPARE_MACRO_MORELLO(Scbnds(c30, c30, 63 << 4),
                        "scbnds c30, c30, #63, lsl #4 (1008)");

  // Encodable X registers.
  COMPARE_MACRO_MORELLO(Scbnds(c18, c19, x20), "scbnds c18, c19, x20");
  COMPARE_MACRO_MORELLO(Scbnds(csp, csp, xzr), "scbnds csp, csp, xzr");
  COMPARE_MACRO_MORELLO(Scbnds(c0, c0, x0), "scbnds c0, c0, x0");

  // Unencodable Operands.
  COMPARE_MACRO_MORELLO(Scbnds(c0, c1, 65),
                        "mov x16, #0x41\n"
                        "scbnds c0, c1, x16");
  COMPARE_MACRO_MORELLO(Scbnds(c0, c1, 1 << (6 + 4)),
                        "mov x16, #0x400\n"
                        "scbnds c0, c1, x16");
  COMPARE_MACRO_MORELLO(Scbnds(c0, c1, Operand(x2, ASR, 5)),
                        "asr x16, x2, #5\n"
                        "scbnds c0, c1, x16");
  COMPARE_MACRO_MORELLO(Scbnds(c0, c1, Operand(w2, UXTB, 3)),
                        "ubfiz x16, x2, #3, #8\n"
                        "scbnds c0, c1, x16");
}

TEST(morello_scbndse_c_cr_c) {
  SETUP();

  COMPARE_MORELLO(scbndse(c18, c19, x20), "scbndse c18, c19, x20");
  COMPARE_MORELLO(scbndse(c18, c19, xzr), "scbndse c18, c19, xzr");
  COMPARE_MORELLO(scbndse(c18, csp, x20), "scbndse c18, csp, x20");
  COMPARE_MORELLO(scbndse(csp, c19, x20), "scbndse csp, c19, x20");
  COMPARE_MORELLO(scbndse(c30, c30, x30), "scbndse c30, c30, x30");

  COMPARE_MACRO_MORELLO(Scbndse(c18, c19, x20), "scbndse c18, c19, x20");
  COMPARE_MACRO_MORELLO(Scbndse(csp, csp, xzr), "scbndse csp, csp, xzr");
  COMPARE_MACRO_MORELLO(Scbndse(c0, c0, x0), "scbndse c0, c0, x0");
}

TEST(morello_scflgs_c_cr_c) {
  SETUP();

  COMPARE_MORELLO(scflgs(c18, c19, x20), "scflgs c18, c19, x20");
  COMPARE_MORELLO(scflgs(c18, c19, xzr), "scflgs c18, c19, xzr");
  COMPARE_MORELLO(scflgs(c18, csp, x20), "scflgs c18, csp, x20");
  COMPARE_MORELLO(scflgs(csp, c19, x20), "scflgs csp, c19, x20");
  COMPARE_MORELLO(scflgs(c30, c30, x30), "scflgs c30, c30, x30");

  COMPARE_MACRO_MORELLO(Scflgs(c18, c19, x20), "scflgs c18, c19, x20");
  COMPARE_MACRO_MORELLO(Scflgs(csp, csp, xzr), "scflgs csp, csp, xzr");
  COMPARE_MACRO_MORELLO(Scflgs(c0, c0, x0), "scflgs c0, c0, x0");
}

TEST(morello_scoff_c_cr_c) {
  SETUP();

  COMPARE_MORELLO(scoff(c18, c19, x20), "scoff c18, c19, x20");
  COMPARE_MORELLO(scoff(c18, c19, xzr), "scoff c18, c19, xzr");
  COMPARE_MORELLO(scoff(c18, csp, x20), "scoff c18, csp, x20");
  COMPARE_MORELLO(scoff(csp, c19, x20), "scoff csp, c19, x20");
  COMPARE_MORELLO(scoff(c30, c30, x30), "scoff c30, c30, x30");

  COMPARE_MACRO_MORELLO(Scoff(c18, c19, x20), "scoff c18, c19, x20");
  COMPARE_MACRO_MORELLO(Scoff(csp, csp, xzr), "scoff csp, csp, xzr");
  COMPARE_MACRO_MORELLO(Scoff(c0, c0, x0), "scoff c0, c0, x0");
}

TEST(morello_scvalue_c_cr_c) {
  SETUP();

  COMPARE_MORELLO(scvalue(c18, c19, x20), "scvalue c18, c19, x20");
  COMPARE_MORELLO(scvalue(c18, c19, xzr), "scvalue c18, c19, xzr");
  COMPARE_MORELLO(scvalue(c18, csp, x20), "scvalue c18, csp, x20");
  COMPARE_MORELLO(scvalue(csp, c19, x20), "scvalue csp, c19, x20");
  COMPARE_MORELLO(scvalue(c30, c30, x30), "scvalue c30, c30, x30");

  COMPARE_MACRO_MORELLO(Scvalue(c18, c19, x20), "scvalue c18, c19, x20");
  COMPARE_MACRO_MORELLO(Scvalue(csp, csp, xzr), "scvalue csp, csp, xzr");
  COMPARE_MACRO_MORELLO(Scvalue(c0, c0, x0), "scvalue c0, c0, x0");
}

TEST(morello_seal_c_cc_c) {
  SETUP();

  COMPARE_MORELLO(seal(c0, c1, c2), "seal c0, c1, c2");
  COMPARE_MORELLO(seal(c0, c1, czr), "seal c0, c1, czr");
  COMPARE_MORELLO(seal(c0, czr, c2), "seal c0, czr, c2");
  COMPARE_MORELLO(seal(czr, c1, c2), "seal czr, c1, c2");
  COMPARE_MORELLO(seal(c30, c30, c30), "seal c30, c30, c30");

  COMPARE_MACRO_MORELLO(Seal(c0, c1, c2), "seal c0, c1, c2");
  COMPARE_MACRO_MORELLO(Seal(czr, czr, czr), "seal czr, czr, czr");
  COMPARE_MACRO_MORELLO(Seal(c28, c29, c30), "seal c28, c29, c30");
}

TEST(morello_seal_c_ci_c) {
  SETUP();

  COMPARE_MORELLO(seal(c0, c1, SealForm::RB), "seal c0, c1, rb");
  COMPARE_MORELLO(seal(c0, c1, SealForm::LPB), "seal c0, c1, lpb");
  COMPARE_MORELLO(seal(c0, c1, SealForm::LB), "seal c0, c1, lb");
  COMPARE_MORELLO(seal(c0, czr, SealForm::LB), "seal c0, czr, lb");
  COMPARE_MORELLO(seal(czr, c1, SealForm::LB), "seal czr, c1, lb");
  COMPARE_MORELLO(seal(c30, c30, SealForm::LB), "seal c30, c30, lb");

  COMPARE_MACRO_MORELLO(Seal(c0, c1, SealForm::RB), "seal c0, c1, rb");
  COMPARE_MACRO_MORELLO(Seal(c23, c12, SealForm::RB), "seal c23, c12, rb");
  COMPARE_MACRO_MORELLO(Seal(czr, czr, SealForm::RB), "seal czr, czr, rb");

  // Check that RESERVED encodings (form = 0b00) are disassembled as such.
  COMPARE_MORELLO(dci(SEAL_c_ci), "seal c0, c0, RESERVED");
}

TEST(morello_sub_c_cis_c) {
  SETUP();

  // Explicit shift.
  COMPARE_MORELLO(sub(c0, c1, 0x21c, 12), "sub c0, c1, #0x21c000 (2211840)");
  COMPARE_MORELLO(sub(c0, c1, 0xb1f, 0), "sub c0, c1, #0xb1f (2847)");
  COMPARE_MORELLO(sub(csp, c1, 0x21c, 12), "sub csp, c1, #0x21c000 (2211840)");
  COMPARE_MORELLO(sub(c0, csp, 0xb1f, 0), "sub c0, csp, #0xb1f (2847)");
  COMPARE_MORELLO(sub(c30, c30, 0x21c, 12),
                  "sub c30, c30, #0x21c000 (2211840)");

  // Implicit shift.
  COMPARE_MORELLO(sub(c0, c1, 0x21c << 12), "sub c0, c1, #0x21c000 (2211840)");
  COMPARE_MORELLO(sub(c0, c1, 0xb1f), "sub c0, c1, #0xb1f (2847)");
  COMPARE_MORELLO(sub(csp, c1, 0x21c << 12),
                  "sub csp, c1, #0x21c000 (2211840)");
  COMPARE_MORELLO(sub(c0, csp, 0xb1f), "sub c0, csp, #0xb1f (2847)");
  COMPARE_MORELLO(sub(c30, c30, 0x21c << 12),
                  "sub c30, c30, #0x21c000 (2211840)");
}

TEST(morello_subs_r_cc_c) {
  SETUP();

  COMPARE_MORELLO(subs(x0, c1, c2), "subs x0, c1, c2");
  COMPARE_MORELLO(subs(xzr, c1, c2), "cmp c1, c2");
  COMPARE_MORELLO(subs(x0, czr, c2), "subs x0, czr, c2");
  COMPARE_MORELLO(subs(x0, c1, czr), "subs x0, c1, czr");
  COMPARE_MORELLO(subs(x30, c30, c30), "subs x30, c30, c30");
  // 'cmp' is an alias of 'subs'.
  COMPARE_MORELLO(cmp(c4, c9), "cmp c4, c9");

  COMPARE_MACRO_MORELLO(Subs(x0, c1, c2), "subs x0, c1, c2");
  COMPARE_MACRO_MORELLO(Cmp(c4, c9), "cmp c4, c9");
}

TEST(morello_sub_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_MORELLO(Sub(c3, c23, 42), "sub c3, c23, #0x2a (42)");
  COMPARE_MACRO_MORELLO(Sub(c3, c23, 0x000fff), "sub c3, c23, #0xfff (4095)");
  COMPARE_MACRO_MORELLO(Sub(c3, c23, 0xfff000),
                        "sub c3, c23, #0xfff000 (16773120)");
  COMPARE_MACRO_MORELLO(Sub(csp, csp, 42), "sub csp, csp, #0x2a (42)");

  // Negative immediates are handled by `add`, if they are encodable.
  COMPARE_MACRO_MORELLO(Sub(c3, c23, -1), "add c3, c23, #0x1 (1)");
  COMPARE_MACRO_MORELLO(Sub(c3, c23, -0xfff), "add c3, c23, #0xfff (4095)");
  COMPARE_MACRO_MORELLO(Sub(c3, c23, -0xfff000),
                        "add c3, c23, #0xfff000 (16773120)");

  // Note that `sub` has no register-operand form, so other operands are
  // emulated with `Neg` and `add`.
  COMPARE_MACRO_MORELLO(Sub(c3, c23, 0x001fff),
                        "mov x3, #0xffffffffffffe001\n"
                        "add c3, c23, x3, uxtx");
  COMPARE_MACRO_MORELLO(Sub(c3, c23, 0xfff001),
                        "mov x3, #0xffffffffff000fff\n"
                        "add c3, c23, x3, uxtx");
  COMPARE_MACRO_MORELLO(Sub(c3, c23, Operand(x8, LSR, 1)),
                        "neg x3, x8, lsr #1\n"
                        "add c3, c23, x3, uxtx");
  // TODO: Improve code generation in this case.
  COMPARE_MACRO_MORELLO(Sub(c3, c3, Operand(x8, SXTX)),
                        "lsr x16, x8, #0\n"
                        "neg x16, x16\n"
                        "add c3, c3, x16, uxtx");
}

TEST(morello_unseal_c_cc_c) {
  SETUP();

  COMPARE_MORELLO(unseal(c0, c1, c2), "unseal c0, c1, c2");
  COMPARE_MORELLO(unseal(c0, c1, czr), "unseal c0, c1, czr");
  COMPARE_MORELLO(unseal(c0, czr, c2), "unseal c0, czr, c2");
  COMPARE_MORELLO(unseal(czr, c1, c2), "unseal czr, c1, c2");
  COMPARE_MORELLO(unseal(c30, c30, c30), "unseal c30, c30, c30");

  COMPARE_MACRO_MORELLO(Unseal(c0, c1, c2), "unseal c0, c1, c2");
  COMPARE_MACRO_MORELLO(Unseal(czr, czr, czr), "unseal czr, czr, czr");
  COMPARE_MACRO_MORELLO(Unseal(c28, c29, c30), "unseal c28, c29, c30");
}

}  // namespace aarch64
}  // namespace vixl
