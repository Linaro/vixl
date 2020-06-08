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

  // COMPARE_MORELLO(mov(c0, c1), "TODO");
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

TEST(morello_adrdp_c_id_c) {
  SETUP();

  // COMPARE_MORELLO(adrdp(c0, &label), "TODO");
}

TEST(morello_adrp_c_ip_c) {
  SETUP();

  // COMPARE_MORELLO(adrp(c0, &label), "TODO");
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

  // COMPARE_MORELLO(bicflgs(c0, c1, 118), "TODO");
}

TEST(morello_bicflgs_c_cr_c) {
  SETUP();

  // COMPARE_MORELLO(bicflgs(c0, c1, x2), "TODO");
}

TEST(morello_blr_c_c) {
  SETUP();

  // COMPARE_MORELLO(blr(c0), "TODO");
}

TEST(morello_blr_ci_c) {
  SETUP();

  // COMPARE_MORELLO(blr(MemOperand(c0, -64)), "TODO");
}

TEST(morello_blrr_c_c) {
  SETUP();

  // COMPARE_MORELLO(blrr(c0), "TODO");
}

TEST(morello_blrs_c_c) {
  SETUP();

  // COMPARE_MORELLO(blrs(c0), "TODO");
}

TEST(morello_blrs_c_c_c) {
  SETUP();

  // COMPARE_MORELLO(blrs(C29, c0, c1), "TODO");
}

TEST(morello_br_c_c) {
  SETUP();

  // COMPARE_MORELLO(br(c0), "TODO");
}

TEST(morello_br_ci_c) {
  SETUP();

  // COMPARE_MORELLO(br(MemOperand(c0, 640)), "TODO");
}

TEST(morello_brr_c_c) {
  SETUP();

  // COMPARE_MORELLO(brr(c0), "TODO");
}

TEST(morello_brs_c_c) {
  SETUP();

  // COMPARE_MORELLO(brs(c0), "TODO");
}

TEST(morello_brs_c_c_c) {
  SETUP();

  // COMPARE_MORELLO(brs(C29, c0, c1), "TODO");
}

TEST(morello_build_c_c_c) {
  SETUP();

  // COMPARE_MORELLO(build(c0, c1, c2), "TODO");
}

TEST(morello_bx_c) {
  SETUP();

  COMPARE_MORELLO(bx(4), "bx #4");
  COMPARE_MACRO_MORELLO(Bx(), "bx #4");
}

TEST(morello_cfhi_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(cfhi(x0, c1), "TODO");
}

TEST(morello_chkeq_cc_c) {
  SETUP();

  // COMPARE_MORELLO(chkeq(c0, c1), "TODO");
}

TEST(morello_chksld_c_c) {
  SETUP();

  // COMPARE_MORELLO(chksld(c0), "TODO");
}

TEST(morello_chkss_cc_c) {
  SETUP();

  // COMPARE_MORELLO(chkss(c0, c1), "TODO");
}

TEST(morello_chkssu_c_cc_c) {
  SETUP();

  // COMPARE_MORELLO(chkssu(c0, c1, c2), "TODO");
}

TEST(morello_chktgd_c_c) {
  SETUP();

  // COMPARE_MORELLO(chktgd(c0), "TODO");
}

TEST(morello_clrperm_c_ci_c) {
  SETUP();

  // COMPARE_MORELLO(clrperm(c0, c1, Clrperm::X), "TODO");
}

TEST(morello_clrperm_c_cr_c) {
  SETUP();

  // COMPARE_MORELLO(clrperm(c0, c1, x2), "TODO");
}

TEST(morello_clrtag_c_c_c) {
  SETUP();

  // COMPARE_MORELLO(clrtag(c0, c1), "TODO");
}

TEST(morello_cpy_c_c_c) {
  SETUP();

  // COMPARE_MORELLO(cpy(c0, c1), "TODO");
}

TEST(morello_cpytype_c_c_c) {
  SETUP();

  // COMPARE_MORELLO(cpytype(c0, c1, c2), "TODO");
}

TEST(morello_cpyvalue_c_c_c) {
  SETUP();

  // COMPARE_MORELLO(cpyvalue(c0, c1, c2), "TODO");
}

TEST(morello_cseal_c_c_c) {
  SETUP();

  // COMPARE_MORELLO(cseal(c0, c1, c2), "TODO");
}

TEST(morello_csel_c_ci_c) {
  SETUP();

  // COMPARE_MORELLO(csel(c0, c1, c2, hi), "TODO");
}

TEST(morello_cthi_c_cr_c) {
  SETUP();

  // COMPARE_MORELLO(cthi(c0, c1, x2), "TODO");
}

TEST(morello_cvt_c_cr_c) {
  SETUP();

  // COMPARE_MORELLO(cvt(c0, c1, x2), "TODO");
}

TEST(morello_cvt_r_cc_c) {
  SETUP();

  // COMPARE_MORELLO(cvt(x0, c1, c2), "TODO");
}

TEST(morello_cvtd_c_r_c) {
  SETUP();

  // COMPARE_MORELLO(cvtd(c0, x1), "TODO");
}

TEST(morello_cvtd_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(cvtd(x0, c1), "TODO");
}

TEST(morello_cvtdz_c_r_c) {
  SETUP();

  // COMPARE_MORELLO(cvtdz(c0, x1), "TODO");
}

TEST(morello_cvtp_c_r_c) {
  SETUP();

  // COMPARE_MORELLO(cvtp(c0, x1), "TODO");
}

TEST(morello_cvtp_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(cvtp(x0, c1), "TODO");
}

TEST(morello_cvtpz_c_r_c) {
  SETUP();

  // COMPARE_MORELLO(cvtpz(c0, x1), "TODO");
}

TEST(morello_cvtz_c_cr_c) {
  SETUP();

  // COMPARE_MORELLO(cvtz(c0, c1, x2), "TODO");
}

TEST(morello_eorflgs_c_ci_c) {
  SETUP();

  // COMPARE_MORELLO(eorflgs(c0, c1, 173), "TODO");
}

TEST(morello_eorflgs_c_cr_c) {
  SETUP();

  // COMPARE_MORELLO(eorflgs(c0, c1, x2), "TODO");
}

TEST(morello_gcbase_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(gcbase(x0, c1), "TODO");
}

TEST(morello_gcflgs_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(gcflgs(x0, c1), "TODO");
}

TEST(morello_gclen_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(gclen(x0, c1), "TODO");
}

TEST(morello_gclim_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(gclim(x0, c1), "TODO");
}

TEST(morello_gcoff_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(gcoff(x0, c1), "TODO");
}

TEST(morello_gcperm_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(gcperm(x0, c1), "TODO");
}

TEST(morello_gcseal_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(gcseal(x0, c1), "TODO");
}

TEST(morello_gctag_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(gctag(x0, c1), "TODO");
}

TEST(morello_gctype_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(gctype(x0, c1), "TODO");
}

TEST(morello_gcvalue_r_c_c) {
  SETUP();

  // COMPARE_MORELLO(gcvalue(x0, c1), "TODO");
}

TEST(morello_mrs_c_i_c) {
  SETUP();

  // COMPARE_MORELLO(mrs(c0, FPCR), "TODO");
  // COMPARE_MORELLO(mrs(c0, RNDR), "TODO");
  // COMPARE_MORELLO(mrs(c0, RNDRRS), "TODO");
}

TEST(morello_msr_c_i_c) {
  SETUP();

  // COMPARE_MORELLO(msr(FPCR, c0), "TODO");
}

TEST(morello_orrflgs_c_ci_c) {
  SETUP();

  // COMPARE_MORELLO(orrflgs(c0, c1, 124), "TODO");
}

TEST(morello_orrflgs_c_cr_c) {
  SETUP();

  // COMPARE_MORELLO(orrflgs(c0, c1, x2), "TODO");
}

TEST(morello_ret_c_c) {
  SETUP();

  // COMPARE_MORELLO(ret(c0), "TODO");
}

TEST(morello_retr_c_c) {
  SETUP();

  // COMPARE_MORELLO(retr(c0), "TODO");
}

TEST(morello_rets_c_c) {
  SETUP();

  // COMPARE_MORELLO(rets(c0), "TODO");
}

TEST(morello_rets_c_c_c) {
  SETUP();

  // COMPARE_MORELLO(rets(C29, c0, c1), "TODO");
}

TEST(morello_rrlen_r_r_c) {
  SETUP();

  // COMPARE_MORELLO(rrlen(x0, x1), "TODO");
}

TEST(morello_rrmask_r_r_c) {
  SETUP();

  // COMPARE_MORELLO(rrmask(x0, x1), "TODO");
}

TEST(morello_scbnds_c_ci_c) {
  SETUP();

  // COMPARE_MORELLO(scbnds(c0, c1, 42), "TODO");
}

TEST(morello_scbnds_c_ci_s) {
  SETUP();

  // COMPARE_MORELLO(scbnds(c0, c1, 0x77 << 4), "TODO");
}

TEST(morello_scbnds_c_cr_c) {
  SETUP();

  // COMPARE_MORELLO(scbnds(c0, c1, x2), "TODO");
}

TEST(morello_scbndse_c_cr_c) {
  SETUP();

  // COMPARE_MORELLO(scbndse(c0, c1, x2), "TODO");
}

TEST(morello_scflgs_c_cr_c) {
  SETUP();

  // COMPARE_MORELLO(scflgs(c0, c1, x2), "TODO");
}

TEST(morello_scoff_c_cr_c) {
  SETUP();

  // COMPARE_MORELLO(scoff(c0, c1, x2), "TODO");
}

TEST(morello_scvalue_c_cr_c) {
  SETUP();

  // COMPARE_MORELLO(scvalue(c0, c1, x2), "TODO");
}

TEST(morello_seal_c_cc_c) {
  SETUP();

  // COMPARE_MORELLO(seal(c0, c1, c2), "TODO");
}

TEST(morello_seal_c_ci_c) {
  SETUP();

  // COMPARE_MORELLO(seal(c0, c1, Seal::LPB), "TODO");
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

  // COMPARE_MORELLO(unseal(c0, c1, c2), "TODO");
}

}  // namespace aarch64
}  // namespace vixl
