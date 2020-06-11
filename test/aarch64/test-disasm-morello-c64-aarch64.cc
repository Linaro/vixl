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

TEST(morello_c64_adr_c_i_c) {
  SETUP();

  COMPARE_PREFIX_C64(adr(c0, 0), "adr c0, #+0x0 (addr 0x");
  COMPARE_PREFIX_C64(adr(c0, 1), "adr c0, #+0x1 (addr 0x");
  COMPARE_PREFIX_C64(adr(c0, -1), "adr c0, #-0x1 (addr 0x");
  COMPARE_PREFIX_C64(adr(c0, 42), "adr c0, #+0x2a (addr 0x");
  COMPARE_PREFIX_C64(adr(c0, 0xfffff), "adr c0, #+0xfffff (addr 0x");
  COMPARE_PREFIX_C64(adr(c0, -0x100000), "adr c0, #-0x100000 (addr 0x");
  COMPARE_PREFIX_C64(adr(czr, 42), "adr czr, #+0x2a (addr 0x");
  COMPARE_PREFIX_C64(adr(c30, 42), "adr c30, #+0x2a (addr 0x");
}

TEST(morello_c64_adr_label) {
  typedef DisasmTestUtilMacroAssembler MacroAssembler;

  SETUP();

  auto adr_c4 = [&masm](Label* l) { masm.adr(c4, l); };
  auto adr_czr = [&masm](Label* l) { masm.adr(czr, l); };

  // When targeting C64, the bottom bit should be set (for use in `br`, etc).
  COMPARE_PREFIX_C64(WithA64LabelBefore(adr_c4), "adr c4, #+0x0 (addr 0x");
  COMPARE_PREFIX_C64(WithC64LabelBefore(adr_czr), "adr czr, #+0x1 (addr 0x");
  COMPARE_PREFIX_C64(WithDataLabelBefore(adr_c4), "adr c4, #+0x0 (addr 0x");
  COMPARE_PREFIX_C64(WithA64LabelAfter(adr_c4), "adr c4, #+0x4 (addr 0x");
  COMPARE_PREFIX_C64(WithC64LabelAfter(adr_c4), "adr c4, #+0x5 (addr 0x");
  COMPARE_PREFIX_C64(WithDataLabelAfter(adr_c4), "adr c4, #+0x4 (addr 0x");

  auto macro_adr_c4 = [&masm](Label* l) { masm.Adr(c4, l); };
  COMPARE_MACRO_PREFIX_C64(WithA64LabelBefore(macro_adr_c4),
                           "adr c4, #+0x0 (addr 0x");
  COMPARE_MACRO_PREFIX_C64(WithC64LabelAfter(macro_adr_c4),
                           "adr c4, #+0x5 (addr 0x");
}

TEST(morello_c64_adrdp_c_id_c) {
  SETUP();

  COMPARE_C64(adrdp(c0, 0), "adrdp c0, #+0x0");
  COMPARE_C64(adrdp(c0, 1), "adrdp c0, #+0x1");
  COMPARE_C64(adrdp(c0, 42), "adrdp c0, #+0x2a");
  COMPARE_C64(adrdp(c0, 0xfffff), "adrdp c0, #+0xfffff");
  COMPARE_C64(adrdp(czr, 42), "adrdp czr, #+0x2a");
  COMPARE_C64(adrdp(c30, 42), "adrdp c30, #+0x2a");

  // We don't implement adrdp(CRegister, Label*) because VIXL labels can only be
  // PC or PCC-relative.

  COMPARE_MACRO_C64(Adrdp(c0, 0), "adrdp c0, #+0x0");
  COMPARE_MACRO_C64(Adrdp(c0, 42), "adrdp c0, #+0x2a");
}

TEST(morello_c64_adrp_c_ip_c) {
  SETUP();

  COMPARE_PREFIX_C64(adrp(c0, 0), "adrp c0, #+0x0 (addr 0x");
  COMPARE_PREFIX_C64(adrp(c0, 1), "adrp c0, #+0x1 (addr 0x");
  COMPARE_PREFIX_C64(adrp(c0, -1), "adrp c0, #-0x1 (addr 0x");
  COMPARE_PREFIX_C64(adrp(c0, 42), "adrp c0, #+0x2a (addr 0x");
  COMPARE_PREFIX_C64(adrp(c0, 0x7ffff), "adrp c0, #+0x7ffff (addr 0x");
  COMPARE_PREFIX_C64(adrp(c0, -0x80000), "adrp c0, #-0x80000 (addr 0x");
  COMPARE_PREFIX_C64(adrp(czr, 42), "adrp czr, #+0x2a (addr 0x");
  COMPARE_PREFIX_C64(adrp(c30, 42), "adrp c30, #+0x2a (addr 0x");
}

TEST(morello_c64_adrp_label) {
  typedef DisasmTestUtilMacroAssembler MacroAssembler;

  SETUP();

  auto adrp_c4 = [&masm](Label* l) { masm.adrp(c4, l); };
  auto adrp_czr = [&masm](Label* l) { masm.adrp(czr, l); };

  // Check that the C64 interworking bit is not applied when the offset is
  // page-scaled.
  COMPARE_PREFIX_C64(WithA64LabelBefore(adrp_c4), "adrp c4, #+0x0 (addr 0x");
  COMPARE_PREFIX_C64(WithC64LabelBefore(adrp_czr), "adrp czr, #+0x0 (addr 0x");
  COMPARE_PREFIX_C64(WithDataLabelBefore(adrp_c4), "adrp c4, #+0x0 (addr 0x");
  COMPARE_PREFIX_C64(WithA64LabelAfter(adrp_c4), "adrp c4, #+0x0 (addr 0x");
  COMPARE_PREFIX_C64(WithC64LabelAfter(adrp_c4), "adrp c4, #+0x0 (addr 0x");
  COMPARE_PREFIX_C64(WithDataLabelAfter(adrp_c4), "adrp c4, #+0x0 (addr 0x");

  auto macro_adrp_c4 = [&masm](Label* l) { masm.Adrp(c4, l); };
  COMPARE_MACRO_PREFIX_C64(WithA64LabelBefore(macro_adrp_c4),
                           "adrp c4, #+0x0 (addr 0x");
  COMPARE_MACRO_PREFIX_C64(WithC64LabelAfter(macro_adrp_c4),
                           "adrp c4, #+0x0 (addr 0x");
}

TEST(morello_c64_aldar_c_r_c) {
  SETUP();

  // COMPARE_C64(ldar(c0, MemOperand(x1)), "TODO");
}

TEST(morello_c64_aldar_r_r_32) {
  SETUP();

  // COMPARE_C64(ldar(w0, MemOperand(x1)), "TODO");
}

TEST(morello_c64_aldarb_r_r_b) {
  SETUP();

  // COMPARE_C64(ldarb(w0, MemOperand(x1)), "TODO");
}

TEST(morello_c64_aldr_c_ri_c) {
  SETUP();

  // COMPARE_C64(ldr(c0, MemOperand(x1, 6928)), "TODO");
}

TEST(morello_c64_aldr_c_rrb_c) {
  SETUP();

  // COMPARE_C64(ldr(c0, MemOperand(x1, w2, UXTW, 4)), "TODO");
  // COMPARE_C64(ldr(c0, MemOperand(x1, x2, SXTX, 0)), "TODO");
}

TEST(morello_c64_aldr_r_ri_32) {
  SETUP();

  // COMPARE_C64(ldr(w0, MemOperand(x1, 1508)), "TODO");
}

TEST(morello_c64_aldr_r_ri_64) {
  SETUP();

  // COMPARE_C64(ldr(x0, MemOperand(x1, 640)), "TODO");
}

TEST(morello_c64_aldr_r_rrb_32) {
  SETUP();

  // COMPARE_C64(ldr(w0, MemOperand(x1, w2, SXTW, 2)), "TODO");
  // COMPARE_C64(ldr(w0, MemOperand(x1, x2, SXTX, 0)), "TODO");
}

TEST(morello_c64_aldr_r_rrb_64) {
  SETUP();

  // COMPARE_C64(ldr(x0, MemOperand(x1, w2, UXTW, 0)), "TODO");
  // COMPARE_C64(ldr(x0, MemOperand(x1, x2, SXTX, 0)), "TODO");
}

TEST(morello_c64_aldr_v_rrb_d) {
  SETUP();

  // COMPARE_C64(ldr(d0, MemOperand(x1, w2, SXTW, 3)), "TODO");
  // COMPARE_C64(ldr(d0, MemOperand(x1, x2, SXTX, 3)), "TODO");
}

TEST(morello_c64_aldr_v_rrb_s) {
  SETUP();

  // COMPARE_C64(ldr(s0, MemOperand(x1, w2, SXTW, 0)), "TODO");
  // COMPARE_C64(ldr(s0, MemOperand(x1, x2, SXTX, 2)), "TODO");
}

TEST(morello_c64_aldrb_r_ri_b) {
  SETUP();

  // COMPARE_C64(ldrb(w0, MemOperand(x1, 432)), "TODO");
}

TEST(morello_c64_aldrb_r_rrb_b) {
  SETUP();

  // COMPARE_C64(ldrb(w0, MemOperand(x1, w2, SXTW)), "TODO");
  // COMPARE_C64(ldrb(w0, MemOperand(x1, w2, UXTW)), "TODO");
  // COMPARE_C64(ldrb(w0, MemOperand(x1, x2, LSL)), "TODO");
  // COMPARE_C64(ldrb(w0, MemOperand(x1, x2, SXTX)), "TODO");
}

TEST(morello_c64_aldrh_r_rrb_32) {
  SETUP();

  // COMPARE_C64(ldrh(w0, MemOperand(x1, w2, UXTW, 1)), "TODO");
  // COMPARE_C64(ldrh(w0, MemOperand(x1, x2, LSL, 0)), "TODO");
}

TEST(morello_c64_aldrsb_r_rrb_32) {
  SETUP();

  // COMPARE_C64(ldrsb(w0, MemOperand(x1, w2, SXTW)), "TODO");
  // COMPARE_C64(ldrsb(w0, MemOperand(x1, w2, UXTW)), "TODO");
  // COMPARE_C64(ldrsb(w0, MemOperand(x1, x2, LSL)), "TODO");
  // COMPARE_C64(ldrsb(w0, MemOperand(x1, x2, SXTX)), "TODO");
}

TEST(morello_c64_aldrsb_r_rrb_64) {
  SETUP();

  // COMPARE_C64(ldrsb(x0, MemOperand(x1, w2, SXTW)), "TODO");
  // COMPARE_C64(ldrsb(x0, MemOperand(x1, w2, UXTW)), "TODO");
  // COMPARE_C64(ldrsb(x0, MemOperand(x1, x2, LSL)), "TODO");
  // COMPARE_C64(ldrsb(x0, MemOperand(x1, x2, SXTX)), "TODO");
}

TEST(morello_c64_aldrsh_r_rrb_32) {
  SETUP();

  // COMPARE_C64(ldrsh(w0, MemOperand(x1, w2, UXTW, 0)), "TODO");
  // COMPARE_C64(ldrsh(w0, MemOperand(x1, x2, SXTX, 1)), "TODO");
}

TEST(morello_c64_aldrsh_r_rrb_64) {
  SETUP();

  // COMPARE_C64(ldrsh(x0, MemOperand(x1, w2, UXTW, 0)), "TODO");
  // COMPARE_C64(ldrsh(x0, MemOperand(x1, x2, SXTX, 1)), "TODO");
}

TEST(morello_c64_aldur_c_ri_c) {
  SETUP();

  // COMPARE_C64(ldur(c0, MemOperand(x1, 182)), "TODO");
}

TEST(morello_c64_aldur_r_ri_32) {
  SETUP();

  // COMPARE_C64(ldur(w0, MemOperand(x1, 17)), "TODO");
}

TEST(morello_c64_aldur_r_ri_64) {
  SETUP();

  // COMPARE_C64(ldur(x0, MemOperand(x1, 197)), "TODO");
}

TEST(morello_c64_aldur_v_ri_b) {
  SETUP();

  // COMPARE_C64(ldur(b0, MemOperand(x1, -48)), "TODO");
}

TEST(morello_c64_aldur_v_ri_d) {
  SETUP();

  // COMPARE_C64(ldur(d0, MemOperand(x1, -167)), "TODO");
}

TEST(morello_c64_aldur_v_ri_h) {
  SETUP();

  // COMPARE_C64(ldur(h0, MemOperand(x1, 106)), "TODO");
}

TEST(morello_c64_aldur_v_ri_q) {
  SETUP();

  // COMPARE_C64(ldur(q0, MemOperand(x1, -145)), "TODO");
}

TEST(morello_c64_aldur_v_ri_s) {
  SETUP();

  // COMPARE_C64(ldur(s0, MemOperand(x1, 163)), "TODO");
}

TEST(morello_c64_aldurb_r_ri_32) {
  SETUP();

  // COMPARE_C64(ldurb(w0, MemOperand(x1, 16)), "TODO");
}

TEST(morello_c64_aldurh_r_ri_32) {
  SETUP();

  // COMPARE_C64(ldurh(w0, MemOperand(x1, 211)), "TODO");
}

TEST(morello_c64_aldursb_r_ri_32) {
  SETUP();

  // COMPARE_C64(ldursb(w0, MemOperand(x1, -123)), "TODO");
}

TEST(morello_c64_aldursb_r_ri_64) {
  SETUP();

  // COMPARE_C64(ldursb(x0, MemOperand(x1, 156)), "TODO");
}

TEST(morello_c64_aldursh_r_ri_32) {
  SETUP();

  // COMPARE_C64(ldursh(w0, MemOperand(x1, -228)), "TODO");
}

TEST(morello_c64_aldursh_r_ri_64) {
  SETUP();

  // COMPARE_C64(ldursh(x0, MemOperand(x1, 243)), "TODO");
}

TEST(morello_c64_aldursw_r_ri_64) {
  SETUP();

  // COMPARE_C64(ldursw(x0, MemOperand(x1, 108)), "TODO");
}

TEST(morello_c64_astlr_c_r_c) {
  SETUP();

  // COMPARE_C64(stlr(c0, MemOperand(x1)), "TODO");
}

TEST(morello_c64_astlr_r_r_32) {
  SETUP();

  // COMPARE_C64(stlr(w0, MemOperand(x1)), "TODO");
}

TEST(morello_c64_astlrb_r_r_b) {
  SETUP();

  // COMPARE_C64(stlrb(w0, MemOperand(x1)), "TODO");
}

TEST(morello_c64_astr_c_ri_c) {
  SETUP();

  // COMPARE_C64(str(c0, MemOperand(x1, 7152)), "TODO");
}

TEST(morello_c64_astr_c_rrb_c) {
  SETUP();

  // COMPARE_C64(str(c0, MemOperand(x1, w2, UXTW, 4)), "TODO");
  // COMPARE_C64(str(c0, MemOperand(x1, x2, LSL, 4)), "TODO");
}

TEST(morello_c64_astr_r_ri_32) {
  SETUP();

  // COMPARE_C64(str(w0, MemOperand(x1, 1520)), "TODO");
}

TEST(morello_c64_astr_r_ri_64) {
  SETUP();

  // COMPARE_C64(str(x0, MemOperand(x1, 2256)), "TODO");
}

TEST(morello_c64_astr_r_rrb_32) {
  SETUP();

  // COMPARE_C64(str(w0, MemOperand(x1, w2, SXTW, 2)), "TODO");
  // COMPARE_C64(str(w0, MemOperand(x1, x2, SXTX, 0)), "TODO");
}

TEST(morello_c64_astr_r_rrb_64) {
  SETUP();

  // COMPARE_C64(str(x0, MemOperand(x1, w2, UXTW, 3)), "TODO");
  // COMPARE_C64(str(x0, MemOperand(x1, x2, SXTX, 3)), "TODO");
}

TEST(morello_c64_astr_v_rrb_d) {
  SETUP();

  // COMPARE_C64(str(d0, MemOperand(x1, w2, SXTW, 3)), "TODO");
  // COMPARE_C64(str(d0, MemOperand(x1, x2, LSL, 0)), "TODO");
}

TEST(morello_c64_astr_v_rrb_s) {
  SETUP();

  // COMPARE_C64(str(s0, MemOperand(x1, w2, UXTW, 0)), "TODO");
  // COMPARE_C64(str(s0, MemOperand(x1, x2, SXTX, 0)), "TODO");
}

TEST(morello_c64_astrb_r_ri_b) {
  SETUP();

  // COMPARE_C64(strb(w0, MemOperand(x1, 2032)), "TODO");
}

TEST(morello_c64_astrb_r_rrb_b) {
  SETUP();

  // COMPARE_C64(strb(w0, MemOperand(x1, w2, SXTW)), "TODO");
  // COMPARE_C64(strb(w0, MemOperand(x1, w2, UXTW)), "TODO");
  // COMPARE_C64(strb(w0, MemOperand(x1, x2, LSL)), "TODO");
  // COMPARE_C64(strb(w0, MemOperand(x1, x2, SXTX)), "TODO");
}

TEST(morello_c64_astrh_r_rrb_32) {
  SETUP();

  // COMPARE_C64(strh(w0, MemOperand(x1, w2, SXTW, 1)), "TODO");
  // COMPARE_C64(strh(w0, MemOperand(x1, x2, SXTX, 0)), "TODO");
}

TEST(morello_c64_astur_c_ri_c) {
  SETUP();

  // COMPARE_C64(stur(c0, MemOperand(x1, -85)), "TODO");
}

TEST(morello_c64_astur_r_ri_32) {
  SETUP();

  // COMPARE_C64(stur(w0, MemOperand(x1, 179)), "TODO");
}

TEST(morello_c64_astur_r_ri_64) {
  SETUP();

  // COMPARE_C64(stur(x0, MemOperand(x1, -226)), "TODO");
}

TEST(morello_c64_astur_v_ri_b) {
  SETUP();

  // COMPARE_C64(stur(b0, MemOperand(x1, -101)), "TODO");
}

TEST(morello_c64_astur_v_ri_d) {
  SETUP();

  // COMPARE_C64(stur(d0, MemOperand(x1, 133)), "TODO");
}

TEST(morello_c64_astur_v_ri_h) {
  SETUP();

  // COMPARE_C64(stur(h0, MemOperand(x1, -27)), "TODO");
}

TEST(morello_c64_astur_v_ri_q) {
  SETUP();

  // COMPARE_C64(stur(q0, MemOperand(x1, 35)), "TODO");
}

TEST(morello_c64_astur_v_ri_s) {
  SETUP();

  // COMPARE_C64(stur(s0, MemOperand(x1, -148)), "TODO");
}

TEST(morello_c64_asturb_r_ri_32) {
  SETUP();

  // COMPARE_C64(sturb(w0, MemOperand(x1, 154)), "TODO");
}

TEST(morello_c64_asturh_r_ri_32) {
  SETUP();

  // COMPARE_C64(sturh(w0, MemOperand(x1, 247)), "TODO");
}

TEST(morello_c64_cas_c_r_c) {
  SETUP();

  // COMPARE_C64(cas(c0, c1, MemOperand(c2)), "TODO");
}

TEST(morello_c64_casa_c_r_c) {
  SETUP();

  // COMPARE_C64(casa(c0, c1, MemOperand(c2)), "TODO");
}

TEST(morello_c64_casal_c_r_c) {
  SETUP();

  // COMPARE_C64(casal(c0, c1, MemOperand(c2)), "TODO");
}

TEST(morello_c64_casl_c_r_c) {
  SETUP();

  // COMPARE_C64(casl(c0, c1, MemOperand(c2)), "TODO");
}

TEST(morello_c64_ldapr_c_r_c) {
  SETUP();

  // COMPARE_C64(ldapr(c0, MemOperand(c1)), "TODO");
}

TEST(morello_c64_ldar_c_r_c) {
  SETUP();

  // COMPARE_C64(ldar(c0, MemOperand(c1)), "TODO");
}

TEST(morello_c64_ldaxp_c_r_c) {
  SETUP();

  // COMPARE_C64(ldaxp(c0, c1, MemOperand(c2)), "TODO");
}

TEST(morello_c64_ldaxr_c_r_c) {
  SETUP();

  // COMPARE_C64(ldaxr(c0, MemOperand(c1)), "TODO");
}

TEST(morello_c64_ldct_r_r_) {
  SETUP();

  // COMPARE_C64(ldct(x0, MemOperand(c1)), "TODO");
}

TEST(morello_c64_ldnp_c_rib_c) {
  SETUP();

  // COMPARE_C64(ldnp(c0, c1, MemOperand(c2, -816)), "TODO");
}

TEST(morello_c64_ldp_c_rib_c) {
  SETUP();

  // COMPARE_C64(ldp(c0, c1, MemOperand(c2, 272)), "TODO");
}

TEST(morello_c64_ldp_c_ribw_c) {
  SETUP();

  // COMPARE_C64(ldp(c0, c1, MemOperand(c2, -352, PreIndex)), "TODO");
}

TEST(morello_c64_ldp_cc_riaw_c) {
  SETUP();

  // COMPARE_C64(ldp(c0, c1, MemOperand(c2, -256, PostIndex)), "TODO");
}

TEST(morello_c64_ldpblr_c_c_c) {
  SETUP();

  // COMPARE_C64(ldpblr(c0, MemOperand(c1)), "TODO");
}

TEST(morello_c64_ldpbr_c_c_c) {
  SETUP();

  // COMPARE_C64(ldpbr(c0, MemOperand(c1)), "TODO");
}

TEST(morello_c64_ldr_c_i_c) {
  SETUP();

  // COMPARE_C64(ldr(c0, -760672), "TODO");
}

TEST(morello_c64_ldr_c_riaw_c) {
  SETUP();

  // COMPARE_C64(ldr(c0, MemOperand(c1, 512, PostIndex)), "TODO");
}

TEST(morello_c64_ldr_c_rib_c) {
  SETUP();

  // COMPARE_C64(ldr(c0, MemOperand(c1, 6928)), "TODO");
}

TEST(morello_c64_ldr_c_ribw_c) {
  SETUP();

  // COMPARE_C64(ldr(c0, MemOperand(c1, 5760, PreIndex)), "TODO");
}

TEST(morello_c64_ldr_c_rrb_c) {
  SETUP();

  // COMPARE_C64(ldr(c0, MemOperand(c1, w2, UXTW, 4)), "TODO");
  // COMPARE_C64(ldr(c0, MemOperand(c1, x2, SXTX, 0)), "TODO");
}

TEST(morello_c64_ldur_c_ri_c) {
  SETUP();

  // COMPARE_C64(ldur(c0, MemOperand(c1, 182)), "TODO");
}

TEST(morello_c64_ldxp_c_r_c) {
  SETUP();

  // COMPARE_C64(ldxp(c0, c1, MemOperand(c2)), "TODO");
}

TEST(morello_c64_ldxr_c_r_c) {
  SETUP();

  // COMPARE_C64(ldxr(c0, MemOperand(c1)), "TODO");
}


TEST(morello_c64_ret_default) {
  SETUP();

  // `ret()` defaults to `ret(lr)` in A64, but we disassemble this as `ret`.
  COMPARE_C64(ret(), "ret c30");
  COMPARE_MACRO_C64(Ret(), "ret c30");

  // Check that we disassemble `ret(lr)` and `ret(clr)` unambiguously regardless
  // of the ISA.
  COMPARE_C64(dci(RET | Assembler::Rn(lr)), "ret x30");
  COMPARE_C64(dci(RET_c | Assembler::Rn(clr)), "ret c30");
}

TEST(morello_c64_stct_r_r_) {
  SETUP();

  // COMPARE_C64(stct(x0, MemOperand(c1)), "TODO");
}

TEST(morello_c64_stlr_c_r_c) {
  SETUP();

  // COMPARE_C64(stlr(c0, MemOperand(c1)), "TODO");
}

TEST(morello_c64_stlxp_r_cr_c) {
  SETUP();

  // COMPARE_C64(stlxp(w0, c1, c2, MemOperand(c3)), "TODO");
}

TEST(morello_c64_stlxr_r_cr_c) {
  SETUP();

  // COMPARE_C64(stlxr(w0, c1, MemOperand(c2)), "TODO");
}

TEST(morello_c64_stnp_c_rib_c) {
  SETUP();

  // COMPARE_C64(stnp(c0, c1, MemOperand(c2, -464)), "TODO");
}

TEST(morello_c64_stp_c_rib_c) {
  SETUP();

  // COMPARE_C64(stp(c0, c1, MemOperand(c2, -128)), "TODO");
}

TEST(morello_c64_stp_c_ribw_c) {
  SETUP();

  // COMPARE_C64(stp(c0, c1, MemOperand(c2, -960, PreIndex)), "TODO");
}

TEST(morello_c64_stp_cc_riaw_c) {
  SETUP();

  // COMPARE_C64(stp(c0, c1, MemOperand(c2, 336, PostIndex)), "TODO");
}

TEST(morello_c64_str_c_riaw_c) {
  SETUP();

  // COMPARE_C64(str(c0, MemOperand(c1, 6368, PostIndex)), "TODO");
}

TEST(morello_c64_str_c_rib_c) {
  SETUP();

  // COMPARE_C64(str(c0, MemOperand(c1, 7152)), "TODO");
}

TEST(morello_c64_str_c_ribw_c) {
  SETUP();

  // COMPARE_C64(str(c0, MemOperand(c1, 6352, PreIndex)), "TODO");
}

TEST(morello_c64_str_c_rrb_c) {
  SETUP();

  // COMPARE_C64(str(c0, MemOperand(c1, w2, UXTW, 4)), "TODO");
  // COMPARE_C64(str(c0, MemOperand(c1, x2, LSL, 4)), "TODO");
}

TEST(morello_c64_stur_c_ri_c) {
  SETUP();

  // COMPARE_C64(stur(c0, MemOperand(c1, -85)), "TODO");
}

TEST(morello_c64_stxp_r_cr_c) {
  SETUP();

  // COMPARE_C64(stxp(w0, c1, c2, MemOperand(c3)), "TODO");
}

TEST(morello_c64_stxr_r_cr_c) {
  SETUP();

  // COMPARE_C64(stxr(w0, c1, MemOperand(c2)), "TODO");
}

TEST(morello_c64_swp_cc_r_c) {
  SETUP();

  // COMPARE_C64(swp(c0, c1, MemOperand(c2)), "TODO");
}

TEST(morello_c64_swpa_cc_r_c) {
  SETUP();

  // COMPARE_C64(swpa(c0, c1, MemOperand(c2)), "TODO");
}

TEST(morello_c64_swpal_cc_r_c) {
  SETUP();

  // COMPARE_C64(swpal(c0, c1, MemOperand(c2)), "TODO");
}

TEST(morello_c64_swpl_cc_r_c) {
  SETUP();

  // COMPARE_C64(swpl(c0, c1, MemOperand(c2)), "TODO");
}

}  // namespace aarch64
}  // namespace vixl
