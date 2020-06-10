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

#include <cstring>
#include <iostream>
#include <string>

#include "test-runner.h"
#include "test-utils.h"
#include "aarch64/test-utils-aarch64.h"
#include "test-cpu-features-aarch64.h"

#include "aarch64/macro-assembler-aarch64.h"

namespace vixl {
namespace aarch64 {

#define TEST_MORELLO(NAME, ASM) \
  TEST_TEMPLATE(CPUFeatures(CPUFeatures::kMorello), Morello_##NAME, ASM)

TEST_MORELLO(add_0, add(c0, c1, 0x903))
TEST_MORELLO(add_1, add(c0, c1, 0x903, 0))
TEST_MORELLO(add_2, add(c0, c1, 0x6f8 << 12))
TEST_MORELLO(add_3, add(c0, c1, 0x6f8, 12))
TEST_MORELLO(add_4, add(c0, c1, Operand(0x6f8)))
TEST_MORELLO(add_5, add(c0, c1, Operand(x2, UXTX, 0)))
// TEST_MORELLO(adrdp_0, adrdp(c0, &label))
// TEST_MORELLO(adrp_0, adrp(c0, &label))
TEST_MORELLO(alignd_0, alignd(c0, c1, 45))
TEST_MORELLO(alignu_0, alignu(c0, c1, 63))
TEST_MORELLO(bicflgs_0, bicflgs(c0, c1, 118))
TEST_MORELLO(bicflgs_1, bicflgs(c0, c1, x2))
TEST_MORELLO(blr_0, blr(c0))
// TEST_MORELLO(blr_1, blr(MemOperand(c0, -64)))
TEST_MORELLO(blrr_0, blrr(c0))
TEST_MORELLO(blrs_0, blrs(c0))
// TEST_MORELLO(blrs_1, blrs(C29, c0, c1))
TEST_MORELLO(br_0, br(c0))
// TEST_MORELLO(br_1, br(MemOperand(c0, 640)))
TEST_MORELLO(brr_0, brr(c0))
TEST_MORELLO(brs_0, brs(c0))
// TEST_MORELLO(brs_1, brs(C29, c0, c1))
// TEST_MORELLO(build_0, build(c0, c1, c2))
TEST_MORELLO(bx_0, bx(4))
TEST_MORELLO(cfhi_0, cfhi(x0, c1))
TEST_MORELLO(chkeq_0, chkeq(c0, c1))
// TEST_MORELLO(chksld_0, chksld(c0))
TEST_MORELLO(chkss_0, chkss(c0, c1))
TEST_MORELLO(chkssu_0, chkssu(c0, c1, c2))
// TEST_MORELLO(chktgd_0, chktgd(c0))
// TEST_MORELLO(clrperm_0, clrperm(c0, c1, Clrperm::X))
TEST_MORELLO(clrperm_1, clrperm(c0, c1, x2))
// TEST_MORELLO(clrtag_0, clrtag(c0, c1))
// TEST_MORELLO(cpy_0, cpy(c0, c1))
// TEST_MORELLO(cpytype_0, cpytype(c0, c1, c2))
// TEST_MORELLO(cpyvalue_0, cpyvalue(c0, c1, c2))
// TEST_MORELLO(cseal_0, cseal(c0, c1, c2))
// TEST_MORELLO(csel_0, csel(c0, c1, c2, hi))
TEST_MORELLO(cthi_0, cthi(c0, c1, x2))
TEST_MORELLO(cvt_0, cvt(c0, c1, x2))
TEST_MORELLO(cvt_1, cvt(x0, c1, c2))
TEST_MORELLO(cvtd_0, cvtd(c0, x1))
TEST_MORELLO(cvtd_1, cvtd(x0, c1))
TEST_MORELLO(cvtdz_0, cvtdz(c0, x1))
TEST_MORELLO(cvtp_0, cvtp(c0, x1))
TEST_MORELLO(cvtp_1, cvtp(x0, c1))
TEST_MORELLO(cvtpz_0, cvtpz(c0, x1))
TEST_MORELLO(cvtz_0, cvtz(c0, c1, x2))
TEST_MORELLO(eorflgs_0, eorflgs(c0, c1, 173))
TEST_MORELLO(eorflgs_1, eorflgs(c0, c1, x2))
TEST_MORELLO(gcbase_0, gcbase(x0, c1))
TEST_MORELLO(gcflgs_0, gcflgs(x0, c1))
TEST_MORELLO(gclen_0, gclen(x0, c1))
TEST_MORELLO(gclim_0, gclim(x0, c1))
TEST_MORELLO(gcoff_0, gcoff(x0, c1))
TEST_MORELLO(gcperm_0, gcperm(x0, c1))
TEST_MORELLO(gcseal_0, gcseal(x0, c1))
TEST_MORELLO(gctag_0, gctag(x0, c1))
TEST_MORELLO(gctype_0, gctype(x0, c1))
TEST_MORELLO(gcvalue_0, gcvalue(x0, c1))
// TEST_MORELLO(mov_0, mov(c0, c1))
// TEST_MORELLO(mrs_0, mrs(c0, FPCR))
// TEST_MORELLO(msr_0, msr(FPCR, c0))
TEST_MORELLO(orrflgs_0, orrflgs(c0, c1, 124))
TEST_MORELLO(orrflgs_1, orrflgs(c0, c1, x2))
TEST_MORELLO(ret_0, ret(c0))
TEST_MORELLO(retr_0, retr(c0))
TEST_MORELLO(rets_0, rets(c0))
// TEST_MORELLO(rets_1, rets(C29, c0, c1))
// TEST_MORELLO(rrlen_0, rrlen(x0, x1))
// TEST_MORELLO(rrmask_0, rrmask(x0, x1))
// TEST_MORELLO(scbnds_0, scbnds(c0, c1, 42))
// TEST_MORELLO(scbnds_1, scbnds(c0, c1, 0x77 << 4))
TEST_MORELLO(scbnds_2, scbnds(c0, c1, x2))
TEST_MORELLO(scbndse_0, scbndse(c0, c1, x2))
TEST_MORELLO(scflgs_0, scflgs(c0, c1, x2))
TEST_MORELLO(scoff_0, scoff(c0, c1, x2))
TEST_MORELLO(scvalue_0, scvalue(c0, c1, x2))
TEST_MORELLO(seal_0, seal(c0, c1, c2))
// TEST_MORELLO(seal_1, seal(c0, c1, SealForm::LPB))
TEST_MORELLO(sub_0, sub(c0, c1, 0xb1f))
TEST_MORELLO(sub_1, sub(c0, c1, 0xb1f, 0))
TEST_MORELLO(sub_2, sub(c0, c1, 0x21c << 12))
TEST_MORELLO(sub_3, sub(c0, c1, 0x21c, 12))
TEST_MORELLO(subs_0, subs(x0, c1, c2))
TEST_MORELLO(unseal_0, unseal(c0, c1, c2))

#define TEST_RNG_MORELLO(NAME, ASM)                                    \
  TEST_TEMPLATE(CPUFeatures(CPUFeatures::kRNG, CPUFeatures::kMorello), \
                RNG_Morello_##NAME,                                    \
                ASM)

// TEST_RNG_MORELLO(mrs_0, mrs(c0, RNDR))
// TEST_RNG_MORELLO(mrs_1, mrs(c0, RNDRRS))

}  // namespace aarch64
}  // namespace vixl
