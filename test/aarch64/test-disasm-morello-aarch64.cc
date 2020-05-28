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

  // COMPARE(mov(c0, c1), "TODO");
}

TEST(morello_add_c_cis_c) {
  SETUP();

  // COMPARE(add(c0, c1, 0x6f8 << 12), "TODO");
  // COMPARE(add(c0, c1, 0x903), "TODO");
}

TEST(morello_add_c_cri_c) {
  SETUP();

  // COMPARE(add(c0, c1, Operand(x2, UXTX, 0)), "TODO");
}

TEST(morello_adrdp_c_id_c) {
  SETUP();

  // COMPARE(adrdp(c0, &label), "TODO");
}

TEST(morello_adrp_c_ip_c) {
  SETUP();

  // COMPARE(adrp(c0, &label), "TODO");
}

TEST(morello_alignd_c_ci_c) {
  SETUP();

  // COMPARE(alignd(c0, c1, 45), "TODO");
}

TEST(morello_alignu_c_ci_c) {
  SETUP();

  // COMPARE(alignu(c0, c1, 63), "TODO");
}

TEST(morello_bicflgs_c_ci_c) {
  SETUP();

  // COMPARE(bicflgs(c0, c1, 118), "TODO");
}

TEST(morello_bicflgs_c_cr_c) {
  SETUP();

  // COMPARE(bicflgs(c0, c1, x2), "TODO");
}

TEST(morello_blr_c_c) {
  SETUP();

  // COMPARE(blr(c0), "TODO");
}

TEST(morello_blr_ci_c) {
  SETUP();

  // COMPARE(blr(MemOperand(c0, -64)), "TODO");
}

TEST(morello_blrr_c_c) {
  SETUP();

  // COMPARE(blrr(c0), "TODO");
}

TEST(morello_blrs_c_c) {
  SETUP();

  // COMPARE(blrs(c0), "TODO");
}

TEST(morello_blrs_c_c_c) {
  SETUP();

  // COMPARE(blrs(C29, c0, c1), "TODO");
}

TEST(morello_br_c_c) {
  SETUP();

  // COMPARE(br(c0), "TODO");
}

TEST(morello_br_ci_c) {
  SETUP();

  // COMPARE(br(MemOperand(c0, 640)), "TODO");
}

TEST(morello_brr_c_c) {
  SETUP();

  // COMPARE(brr(c0), "TODO");
}

TEST(morello_brs_c_c) {
  SETUP();

  // COMPARE(brs(c0), "TODO");
}

TEST(morello_brs_c_c_c) {
  SETUP();

  // COMPARE(brs(C29, c0, c1), "TODO");
}

TEST(morello_build_c_c_c) {
  SETUP();

  // COMPARE(build(c0, c1, c2), "TODO");
}

TEST(morello_bx_c) {
  SETUP();

  // COMPARE(bx(4), "TODO");
}

TEST(morello_cfhi_r_c_c) {
  SETUP();

  // COMPARE(cfhi(x0, c1), "TODO");
}

TEST(morello_chkeq_cc_c) {
  SETUP();

  // COMPARE(chkeq(c0, c1), "TODO");
}

TEST(morello_chksld_c_c) {
  SETUP();

  // COMPARE(chksld(c0), "TODO");
}

TEST(morello_chkss_cc_c) {
  SETUP();

  // COMPARE(chkss(c0, c1), "TODO");
}

TEST(morello_chkssu_c_cc_c) {
  SETUP();

  // COMPARE(chkssu(c0, c1, c2), "TODO");
}

TEST(morello_chktgd_c_c) {
  SETUP();

  // COMPARE(chktgd(c0), "TODO");
}

TEST(morello_clrperm_c_ci_c) {
  SETUP();

  // COMPARE(clrperm(c0, c1, Clrperm::X), "TODO");
}

TEST(morello_clrperm_c_cr_c) {
  SETUP();

  // COMPARE(clrperm(c0, c1, x2), "TODO");
}

TEST(morello_clrtag_c_c_c) {
  SETUP();

  // COMPARE(clrtag(c0, c1), "TODO");
}

TEST(morello_cpy_c_c_c) {
  SETUP();

  // COMPARE(cpy(c0, c1), "TODO");
}

TEST(morello_cpytype_c_c_c) {
  SETUP();

  // COMPARE(cpytype(c0, c1, c2), "TODO");
}

TEST(morello_cpyvalue_c_c_c) {
  SETUP();

  // COMPARE(cpyvalue(c0, c1, c2), "TODO");
}

TEST(morello_cseal_c_c_c) {
  SETUP();

  // COMPARE(cseal(c0, c1, c2), "TODO");
}

TEST(morello_csel_c_ci_c) {
  SETUP();

  // COMPARE(csel(c0, c1, c2, hi), "TODO");
}

TEST(morello_cthi_c_cr_c) {
  SETUP();

  // COMPARE(cthi(c0, c1, x2), "TODO");
}

TEST(morello_cvt_c_cr_c) {
  SETUP();

  // COMPARE(cvt(c0, c1, x2), "TODO");
}

TEST(morello_cvt_r_cc_c) {
  SETUP();

  // COMPARE(cvt(x0, c1, c2), "TODO");
}

TEST(morello_cvtd_c_r_c) {
  SETUP();

  // COMPARE(cvtd(c0, x1), "TODO");
}

TEST(morello_cvtd_r_c_c) {
  SETUP();

  // COMPARE(cvtd(x0, c1), "TODO");
}

TEST(morello_cvtdz_c_r_c) {
  SETUP();

  // COMPARE(cvtdz(c0, x1), "TODO");
}

TEST(morello_cvtp_c_r_c) {
  SETUP();

  // COMPARE(cvtp(c0, x1), "TODO");
}

TEST(morello_cvtp_r_c_c) {
  SETUP();

  // COMPARE(cvtp(x0, c1), "TODO");
}

TEST(morello_cvtpz_c_r_c) {
  SETUP();

  // COMPARE(cvtpz(c0, x1), "TODO");
}

TEST(morello_cvtz_c_cr_c) {
  SETUP();

  // COMPARE(cvtz(c0, c1, x2), "TODO");
}

TEST(morello_eorflgs_c_ci_c) {
  SETUP();

  // COMPARE(eorflgs(c0, c1, 173), "TODO");
}

TEST(morello_eorflgs_c_cr_c) {
  SETUP();

  // COMPARE(eorflgs(c0, c1, x2), "TODO");
}

TEST(morello_gcbase_r_c_c) {
  SETUP();

  // COMPARE(gcbase(x0, c1), "TODO");
}

TEST(morello_gcflgs_r_c_c) {
  SETUP();

  // COMPARE(gcflgs(x0, c1), "TODO");
}

TEST(morello_gclen_r_c_c) {
  SETUP();

  // COMPARE(gclen(x0, c1), "TODO");
}

TEST(morello_gclim_r_c_c) {
  SETUP();

  // COMPARE(gclim(x0, c1), "TODO");
}

TEST(morello_gcoff_r_c_c) {
  SETUP();

  // COMPARE(gcoff(x0, c1), "TODO");
}

TEST(morello_gcperm_r_c_c) {
  SETUP();

  // COMPARE(gcperm(x0, c1), "TODO");
}

TEST(morello_gcseal_r_c_c) {
  SETUP();

  // COMPARE(gcseal(x0, c1), "TODO");
}

TEST(morello_gctag_r_c_c) {
  SETUP();

  // COMPARE(gctag(x0, c1), "TODO");
}

TEST(morello_gctype_r_c_c) {
  SETUP();

  // COMPARE(gctype(x0, c1), "TODO");
}

TEST(morello_gcvalue_r_c_c) {
  SETUP();

  // COMPARE(gcvalue(x0, c1), "TODO");
}

TEST(morello_mrs_c_i_c) {
  SETUP();

  // COMPARE(mrs(c0, FPCR), "TODO");
  // COMPARE(mrs(c0, RNDR), "TODO");
  // COMPARE(mrs(c0, RNDRRS), "TODO");
}

TEST(morello_msr_c_i_c) {
  SETUP();

  // COMPARE(msr(FPCR, c0), "TODO");
}

TEST(morello_orrflgs_c_ci_c) {
  SETUP();

  // COMPARE(orrflgs(c0, c1, 124), "TODO");
}

TEST(morello_orrflgs_c_cr_c) {
  SETUP();

  // COMPARE(orrflgs(c0, c1, x2), "TODO");
}

TEST(morello_ret_c_c) {
  SETUP();

  // COMPARE(ret(c0), "TODO");
}

TEST(morello_retr_c_c) {
  SETUP();

  // COMPARE(retr(c0), "TODO");
}

TEST(morello_rets_c_c) {
  SETUP();

  // COMPARE(rets(c0), "TODO");
}

TEST(morello_rets_c_c_c) {
  SETUP();

  // COMPARE(rets(C29, c0, c1), "TODO");
}

TEST(morello_rrlen_r_r_c) {
  SETUP();

  // COMPARE(rrlen(x0, x1), "TODO");
}

TEST(morello_rrmask_r_r_c) {
  SETUP();

  // COMPARE(rrmask(x0, x1), "TODO");
}

TEST(morello_scbnds_c_ci_c) {
  SETUP();

  // COMPARE(scbnds(c0, c1, 42), "TODO");
}

TEST(morello_scbnds_c_ci_s) {
  SETUP();

  // COMPARE(scbnds(c0, c1, 0x77 << 4), "TODO");
}

TEST(morello_scbnds_c_cr_c) {
  SETUP();

  // COMPARE(scbnds(c0, c1, x2), "TODO");
}

TEST(morello_scbndse_c_cr_c) {
  SETUP();

  // COMPARE(scbndse(c0, c1, x2), "TODO");
}

TEST(morello_scflgs_c_cr_c) {
  SETUP();

  // COMPARE(scflgs(c0, c1, x2), "TODO");
}

TEST(morello_scoff_c_cr_c) {
  SETUP();

  // COMPARE(scoff(c0, c1, x2), "TODO");
}

TEST(morello_scvalue_c_cr_c) {
  SETUP();

  // COMPARE(scvalue(c0, c1, x2), "TODO");
}

TEST(morello_seal_c_cc_c) {
  SETUP();

  // COMPARE(seal(c0, c1, c2), "TODO");
}

TEST(morello_seal_c_ci_c) {
  SETUP();

  // COMPARE(seal(c0, c1, Seal::LPB), "TODO");
}

TEST(morello_sub_c_cis_c) {
  SETUP();

  // COMPARE(sub(c0, c1, 0x21c << 12), "TODO");
  // COMPARE(sub(c0, c1, 0xb1f), "TODO");
}

TEST(morello_subs_r_cc_c) {
  SETUP();

  // COMPARE(subs(x0, c1, c2), "TODO");
}

TEST(morello_unseal_c_cc_c) {
  SETUP();

  // COMPARE(unseal(c0, c1, c2), "TODO");
}

}  // namespace aarch64
}  // namespace vixl
