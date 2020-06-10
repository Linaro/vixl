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

TEST(morello_a64_aldar_c_r_c) {
  SETUP();

  // COMPARE_A64(ldar(c0, MemOperand(c1)), "TODO");
}

TEST(morello_a64_aldar_r_r_32) {
  SETUP();

  // COMPARE_A64(ldar(w0, MemOperand(c1)), "TODO");
}

TEST(morello_a64_aldarb_r_r_b) {
  SETUP();

  // COMPARE_A64(ldarb(w0, MemOperand(c1)), "TODO");
}

TEST(morello_a64_aldr_c_ri_c) {
  SETUP();

  // COMPARE_A64(ldr(c0, MemOperand(c1, 6928)), "TODO");
}

TEST(morello_a64_aldr_c_rrb_c) {
  SETUP();

  // COMPARE_A64(ldr(c0, MemOperand(c1, w2, UXTW, 4)), "TODO");
  // COMPARE_A64(ldr(c0, MemOperand(c1, x2, SXTX, 0)), "TODO");
}

TEST(morello_a64_aldr_r_ri_32) {
  SETUP();

  // COMPARE_A64(ldr(w0, MemOperand(c1, 1508)), "TODO");
}

TEST(morello_a64_aldr_r_ri_64) {
  SETUP();

  // COMPARE_A64(ldr(x0, MemOperand(c1, 640)), "TODO");
}

TEST(morello_a64_aldr_r_rrb_32) {
  SETUP();

  // COMPARE_A64(ldr(w0, MemOperand(c1, w2, SXTW, 2)), "TODO");
  // COMPARE_A64(ldr(w0, MemOperand(c1, x2, SXTX, 0)), "TODO");
}

TEST(morello_a64_aldr_r_rrb_64) {
  SETUP();

  // COMPARE_A64(ldr(x0, MemOperand(c1, w2, UXTW, 0)), "TODO");
  // COMPARE_A64(ldr(x0, MemOperand(c1, x2, SXTX, 0)), "TODO");
}

TEST(morello_a64_aldr_v_rrb_d) {
  SETUP();

  // COMPARE_A64(ldr(d0, MemOperand(c1, w2, SXTW, 3)), "TODO");
  // COMPARE_A64(ldr(d0, MemOperand(c1, x2, SXTX, 3)), "TODO");
}

TEST(morello_a64_aldr_v_rrb_s) {
  SETUP();

  // COMPARE_A64(ldr(s0, MemOperand(c1, w2, SXTW, 0)), "TODO");
  // COMPARE_A64(ldr(s0, MemOperand(c1, x2, SXTX, 2)), "TODO");
}

TEST(morello_a64_aldrb_r_ri_b) {
  SETUP();

  // COMPARE_A64(ldrb(w0, MemOperand(c1, 432)), "TODO");
}

TEST(morello_a64_aldrb_r_rrb_b) {
  SETUP();

  // COMPARE_A64(ldrb(w0, MemOperand(c1, w2, SXTW)), "TODO");
  // COMPARE_A64(ldrb(w0, MemOperand(c1, w2, UXTW)), "TODO");
  // COMPARE_A64(ldrb(w0, MemOperand(c1, x2, LSL)), "TODO");
  // COMPARE_A64(ldrb(w0, MemOperand(c1, x2, SXTX)), "TODO");
}

TEST(morello_a64_aldrh_r_rrb_32) {
  SETUP();

  // COMPARE_A64(ldrh(w0, MemOperand(c1, w2, UXTW, 1)), "TODO");
  // COMPARE_A64(ldrh(w0, MemOperand(c1, x2, LSL, 0)), "TODO");
}

TEST(morello_a64_aldrsb_r_rrb_32) {
  SETUP();

  // COMPARE_A64(ldrsb(w0, MemOperand(c1, w2, SXTW)), "TODO");
  // COMPARE_A64(ldrsb(w0, MemOperand(c1, w2, UXTW)), "TODO");
  // COMPARE_A64(ldrsb(w0, MemOperand(c1, x2, LSL)), "TODO");
  // COMPARE_A64(ldrsb(w0, MemOperand(c1, x2, SXTX)), "TODO");
}

TEST(morello_a64_aldrsb_r_rrb_64) {
  SETUP();

  // COMPARE_A64(ldrsb(x0, MemOperand(c1, w2, SXTW)), "TODO");
  // COMPARE_A64(ldrsb(x0, MemOperand(c1, w2, UXTW)), "TODO");
  // COMPARE_A64(ldrsb(x0, MemOperand(c1, x2, LSL)), "TODO");
  // COMPARE_A64(ldrsb(x0, MemOperand(c1, x2, SXTX)), "TODO");
}

TEST(morello_a64_aldrsh_r_rrb_32) {
  SETUP();

  // COMPARE_A64(ldrsh(w0, MemOperand(c1, w2, UXTW, 0)), "TODO");
  // COMPARE_A64(ldrsh(w0, MemOperand(c1, x2, SXTX, 1)), "TODO");
}

TEST(morello_a64_aldrsh_r_rrb_64) {
  SETUP();

  // COMPARE_A64(ldrsh(x0, MemOperand(c1, w2, UXTW, 0)), "TODO");
  // COMPARE_A64(ldrsh(x0, MemOperand(c1, x2, SXTX, 1)), "TODO");
}

TEST(morello_a64_aldur_c_ri_c) {
  SETUP();

  // COMPARE_A64(ldur(c0, MemOperand(c1, 182)), "TODO");
}

TEST(morello_a64_aldur_r_ri_32) {
  SETUP();

  // COMPARE_A64(ldur(w0, MemOperand(c1, 17)), "TODO");
}

TEST(morello_a64_aldur_r_ri_64) {
  SETUP();

  // COMPARE_A64(ldur(x0, MemOperand(c1, 197)), "TODO");
}

TEST(morello_a64_aldur_v_ri_b) {
  SETUP();

  // COMPARE_A64(ldur(b0, MemOperand(c1, -48)), "TODO");
}

TEST(morello_a64_aldur_v_ri_d) {
  SETUP();

  // COMPARE_A64(ldur(d0, MemOperand(c1, -167)), "TODO");
}

TEST(morello_a64_aldur_v_ri_h) {
  SETUP();

  // COMPARE_A64(ldur(h0, MemOperand(c1, 106)), "TODO");
}

TEST(morello_a64_aldur_v_ri_q) {
  SETUP();

  // COMPARE_A64(ldur(q0, MemOperand(c1, -145)), "TODO");
}

TEST(morello_a64_aldur_v_ri_s) {
  SETUP();

  // COMPARE_A64(ldur(s0, MemOperand(c1, 163)), "TODO");
}

TEST(morello_a64_aldurb_r_ri_32) {
  SETUP();

  // COMPARE_A64(ldurb(w0, MemOperand(c1, 16)), "TODO");
}

TEST(morello_a64_aldurh_r_ri_32) {
  SETUP();

  // COMPARE_A64(ldurh(w0, MemOperand(c1, 211)), "TODO");
}

TEST(morello_a64_aldursb_r_ri_32) {
  SETUP();

  // COMPARE_A64(ldursb(w0, MemOperand(c1, -123)), "TODO");
}

TEST(morello_a64_aldursb_r_ri_64) {
  SETUP();

  // COMPARE_A64(ldursb(x0, MemOperand(c1, 156)), "TODO");
}

TEST(morello_a64_aldursh_r_ri_32) {
  SETUP();

  // COMPARE_A64(ldursh(w0, MemOperand(c1, -228)), "TODO");
}

TEST(morello_a64_aldursh_r_ri_64) {
  SETUP();

  // COMPARE_A64(ldursh(x0, MemOperand(c1, 243)), "TODO");
}

TEST(morello_a64_aldursw_r_ri_64) {
  SETUP();

  // COMPARE_A64(ldursw(x0, MemOperand(c1, 108)), "TODO");
}

TEST(morello_a64_astlr_c_r_c) {
  SETUP();

  // COMPARE_A64(stlr(c0, MemOperand(c1)), "TODO");
}

TEST(morello_a64_astlr_r_r_32) {
  SETUP();

  // COMPARE_A64(stlr(w0, MemOperand(c1)), "TODO");
}

TEST(morello_a64_astlrb_r_r_b) {
  SETUP();

  // COMPARE_A64(stlrb(w0, MemOperand(c1)), "TODO");
}

TEST(morello_a64_astr_c_ri_c) {
  SETUP();

  // COMPARE_A64(str(c0, MemOperand(c1, 7152)), "TODO");
}

TEST(morello_a64_astr_c_rrb_c) {
  SETUP();

  // COMPARE_A64(str(c0, MemOperand(c1, w2, UXTW, 4)), "TODO");
  // COMPARE_A64(str(c0, MemOperand(c1, x2, LSL, 4)), "TODO");
}

TEST(morello_a64_astr_r_ri_32) {
  SETUP();

  // COMPARE_A64(str(w0, MemOperand(c1, 1520)), "TODO");
}

TEST(morello_a64_astr_r_ri_64) {
  SETUP();

  // COMPARE_A64(str(x0, MemOperand(c1, 2256)), "TODO");
}

TEST(morello_a64_astr_r_rrb_32) {
  SETUP();

  // COMPARE_A64(str(w0, MemOperand(c1, w2, SXTW, 2)), "TODO");
  // COMPARE_A64(str(w0, MemOperand(c1, x2, SXTX, 0)), "TODO");
}

TEST(morello_a64_astr_r_rrb_64) {
  SETUP();

  // COMPARE_A64(str(x0, MemOperand(c1, w2, UXTW, 3)), "TODO");
  // COMPARE_A64(str(x0, MemOperand(c1, x2, SXTX, 3)), "TODO");
}

TEST(morello_a64_astr_v_rrb_d) {
  SETUP();

  // COMPARE_A64(str(d0, MemOperand(c1, w2, SXTW, 3)), "TODO");
  // COMPARE_A64(str(d0, MemOperand(c1, x2, LSL, 0)), "TODO");
}

TEST(morello_a64_astr_v_rrb_s) {
  SETUP();

  // COMPARE_A64(str(s0, MemOperand(c1, w2, UXTW, 0)), "TODO");
  // COMPARE_A64(str(s0, MemOperand(c1, x2, SXTX, 0)), "TODO");
}

TEST(morello_a64_astrb_r_ri_b) {
  SETUP();

  // COMPARE_A64(strb(w0, MemOperand(c1, 2032)), "TODO");
}

TEST(morello_a64_astrb_r_rrb_b) {
  SETUP();

  // COMPARE_A64(strb(w0, MemOperand(c1, w2, SXTW)), "TODO");
  // COMPARE_A64(strb(w0, MemOperand(c1, w2, UXTW)), "TODO");
  // COMPARE_A64(strb(w0, MemOperand(c1, x2, LSL)), "TODO");
  // COMPARE_A64(strb(w0, MemOperand(c1, x2, SXTX)), "TODO");
}

TEST(morello_a64_astrh_r_rrb_32) {
  SETUP();

  // COMPARE_A64(strh(w0, MemOperand(c1, w2, SXTW, 1)), "TODO");
  // COMPARE_A64(strh(w0, MemOperand(c1, x2, SXTX, 0)), "TODO");
}

TEST(morello_a64_astur_c_ri_c) {
  SETUP();

  // COMPARE_A64(stur(c0, MemOperand(c1, -85)), "TODO");
}

TEST(morello_a64_astur_r_ri_32) {
  SETUP();

  // COMPARE_A64(stur(w0, MemOperand(c1, 179)), "TODO");
}

TEST(morello_a64_astur_r_ri_64) {
  SETUP();

  // COMPARE_A64(stur(x0, MemOperand(c1, -226)), "TODO");
}

TEST(morello_a64_astur_v_ri_b) {
  SETUP();

  // COMPARE_A64(stur(b0, MemOperand(c1, -101)), "TODO");
}

TEST(morello_a64_astur_v_ri_d) {
  SETUP();

  // COMPARE_A64(stur(d0, MemOperand(c1, 133)), "TODO");
}

TEST(morello_a64_astur_v_ri_h) {
  SETUP();

  // COMPARE_A64(stur(h0, MemOperand(c1, -27)), "TODO");
}

TEST(morello_a64_astur_v_ri_q) {
  SETUP();

  // COMPARE_A64(stur(q0, MemOperand(c1, 35)), "TODO");
}

TEST(morello_a64_astur_v_ri_s) {
  SETUP();

  // COMPARE_A64(stur(s0, MemOperand(c1, -148)), "TODO");
}

TEST(morello_a64_asturb_r_ri_32) {
  SETUP();

  // COMPARE_A64(sturb(w0, MemOperand(c1, 154)), "TODO");
}

TEST(morello_a64_asturh_r_ri_32) {
  SETUP();

  // COMPARE_A64(sturh(w0, MemOperand(c1, 247)), "TODO");
}

TEST(morello_a64_cas_c_r_c) {
  SETUP();

  // COMPARE_A64(cas(c0, c1, MemOperand(x2)), "TODO");
}

TEST(morello_a64_casa_c_r_c) {
  SETUP();

  // COMPARE_A64(casa(c0, c1, MemOperand(x2)), "TODO");
}

TEST(morello_a64_casal_c_r_c) {
  SETUP();

  // COMPARE_A64(casal(c0, c1, MemOperand(x2)), "TODO");
}

TEST(morello_a64_casl_c_r_c) {
  SETUP();

  // COMPARE_A64(casl(c0, c1, MemOperand(x2)), "TODO");
}

TEST(morello_a64_ldapr_c_r_c) {
  SETUP();

  // COMPARE_A64(ldapr(c0, MemOperand(x1)), "TODO");
}

TEST(morello_a64_ldar_c_r_c) {
  SETUP();

  // COMPARE_A64(ldar(c0, MemOperand(x1)), "TODO");
}

TEST(morello_a64_ldaxp_c_r_c) {
  SETUP();

  // COMPARE_A64(ldaxp(c0, c1, MemOperand(x2)), "TODO");
}

TEST(morello_a64_ldaxr_c_r_c) {
  SETUP();

  // COMPARE_A64(ldaxr(c0, MemOperand(x1)), "TODO");
}

TEST(morello_a64_ldct_r_r_) {
  SETUP();

  // COMPARE_A64(ldct(x0, MemOperand(x1)), "TODO");
}

TEST(morello_a64_ldnp_c_rib_c) {
  SETUP();

  // COMPARE_A64(ldnp(c0, c1, MemOperand(x2, -816)), "TODO");
}

TEST(morello_a64_ldp_c_rib_c) {
  SETUP();

  // COMPARE_A64(ldp(c0, c1, MemOperand(x2, 272)), "TODO");
}

TEST(morello_a64_ldp_c_ribw_c) {
  SETUP();

  // COMPARE_A64(ldp(c0, c1, MemOperand(x2, -352, PreIndex)), "TODO");
}

TEST(morello_a64_ldp_cc_riaw_c) {
  SETUP();

  // COMPARE_A64(ldp(c0, c1, MemOperand(x2, -256, PostIndex)), "TODO");
}

TEST(morello_a64_ldpblr_c_c_c) {
  SETUP();

  // COMPARE_A64(ldpblr(c0, MemOperand(c1)), "TODO");
}

TEST(morello_a64_ldpbr_c_c_c) {
  SETUP();

  // COMPARE_A64(ldpbr(c0, MemOperand(c1)), "TODO");
}

TEST(morello_a64_ldr_c_i_c) {
  SETUP();

  // COMPARE_A64(ldr(c0, -760672), "TODO");
}

TEST(morello_a64_ldr_c_riaw_c) {
  SETUP();

  // COMPARE_A64(ldr(c0, MemOperand(x1, 512, PostIndex)), "TODO");
}

TEST(morello_a64_ldr_c_rib_c) {
  SETUP();

  // COMPARE_A64(ldr(c0, MemOperand(x1, 6928)), "TODO");
}

TEST(morello_a64_ldr_c_ribw_c) {
  SETUP();

  // COMPARE_A64(ldr(c0, MemOperand(x1, 5760, PreIndex)), "TODO");
}

TEST(morello_a64_ldr_c_rrb_c) {
  SETUP();

  // COMPARE_A64(ldr(c0, MemOperand(x1, w2, UXTW, 4)), "TODO");
  // COMPARE_A64(ldr(c0, MemOperand(x1, x2, SXTX, 0)), "TODO");
}

TEST(morello_a64_ldur_c_ri_c) {
  SETUP();

  // COMPARE_A64(ldur(c0, MemOperand(x1, 182)), "TODO");
}

TEST(morello_a64_ldxp_c_r_c) {
  SETUP();

  // COMPARE_A64(ldxp(c0, c1, MemOperand(x2)), "TODO");
}

TEST(morello_a64_ldxr_c_r_c) {
  SETUP();

  // COMPARE_A64(ldxr(c0, MemOperand(x1)), "TODO");
}

TEST(morello_a64_ret_default) {
  SETUP();

  // `ret()` defaults to `ret(lr)` in A64, but we disassemble this as `ret`.
  COMPARE_A64(ret(), "ret");
  COMPARE_MACRO_A64(Ret(), "ret");

  // Check that we disassemble `ret(clr)` unambiguously even in A64 mode.
  COMPARE_A64(dci(RET_c | Assembler::Rn(clr)), "ret c30");
}

TEST(morello_a64_stct_r_r_) {
  SETUP();

  // COMPARE_A64(stct(x0, MemOperand(x1)), "TODO");
}

TEST(morello_a64_stlr_c_r_c) {
  SETUP();

  // COMPARE_A64(stlr(c0, MemOperand(x1)), "TODO");
}

TEST(morello_a64_stlxp_r_cr_c) {
  SETUP();

  // COMPARE_A64(stlxp(w0, c1, c2, MemOperand(x3)), "TODO");
}

TEST(morello_a64_stlxr_r_cr_c) {
  SETUP();

  // COMPARE_A64(stlxr(w0, c1, MemOperand(x2)), "TODO");
}

TEST(morello_a64_stnp_c_rib_c) {
  SETUP();

  // COMPARE_A64(stnp(c0, c1, MemOperand(x2, -464)), "TODO");
}

TEST(morello_a64_stp_c_rib_c) {
  SETUP();

  // COMPARE_A64(stp(c0, c1, MemOperand(x2, -128)), "TODO");
}

TEST(morello_a64_stp_c_ribw_c) {
  SETUP();

  // COMPARE_A64(stp(c0, c1, MemOperand(x2, -960, PreIndex)), "TODO");
}

TEST(morello_a64_stp_cc_riaw_c) {
  SETUP();

  // COMPARE_A64(stp(c0, c1, MemOperand(x2, 336, PostIndex)), "TODO");
}

TEST(morello_a64_str_c_riaw_c) {
  SETUP();

  // COMPARE_A64(str(c0, MemOperand(x1, 6368, PostIndex)), "TODO");
}

TEST(morello_a64_str_c_rib_c) {
  SETUP();

  // COMPARE_A64(str(c0, MemOperand(x1, 7152)), "TODO");
}

TEST(morello_a64_str_c_ribw_c) {
  SETUP();

  // COMPARE_A64(str(c0, MemOperand(x1, 6352, PreIndex)), "TODO");
}

TEST(morello_a64_str_c_rrb_c) {
  SETUP();

  // COMPARE_A64(str(c0, MemOperand(x1, w2, UXTW, 4)), "TODO");
  // COMPARE_A64(str(c0, MemOperand(x1, x2, LSL, 4)), "TODO");
}

TEST(morello_a64_stur_c_ri_c) {
  SETUP();

  // COMPARE_A64(stur(c0, MemOperand(x1, -85)), "TODO");
}

TEST(morello_a64_stxp_r_cr_c) {
  SETUP();

  // COMPARE_A64(stxp(w0, c1, c2, MemOperand(x3)), "TODO");
}

TEST(morello_a64_stxr_r_cr_c) {
  SETUP();

  // COMPARE_A64(stxr(w0, c1, MemOperand(x2)), "TODO");
}

TEST(morello_a64_swp_cc_r_c) {
  SETUP();

  // COMPARE_A64(swp(c0, c1, MemOperand(x2)), "TODO");
}

TEST(morello_a64_swpa_cc_r_c) {
  SETUP();

  // COMPARE_A64(swpa(c0, c1, MemOperand(x2)), "TODO");
}

TEST(morello_a64_swpal_cc_r_c) {
  SETUP();

  // COMPARE_A64(swpal(c0, c1, MemOperand(x2)), "TODO");
}

TEST(morello_a64_swpl_cc_r_c) {
  SETUP();

  // COMPARE_A64(swpl(c0, c1, MemOperand(x2)), "TODO");
}

}  // namespace aarch64
}  // namespace vixl
