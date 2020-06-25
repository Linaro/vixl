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

// Tests of instructions new to Morello, in the C64 ISA.

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

  COMPARE_C64(ldr(c0, MemOperand(x1, 6928)), "ldr c0, [x1, #6928]");
  COMPARE_C64(ldr(c0, MemOperand(x1, 0)), "ldr c0, [x1]");
  COMPARE_C64(ldr(c0, MemOperand(x1, 8176)), "ldr c0, [x1, #8176]");
  COMPARE_C64(ldr(c0, MemOperand(sp, 6928)), "ldr c0, [sp, #6928]");
  COMPARE_C64(ldr(czr, MemOperand(x1, 6928)), "ldr czr, [x1, #6928]");

  // A generic CPURegister works the same.
  COMPARE_C64(ldr(CPURegister(c0), MemOperand(x1, 0)), "ldr c0, [x1]");
  COMPARE_C64(ldr(CPURegister(czr), MemOperand(x1, 8176)),
              "ldr czr, [x1, #8176]");
}

TEST(morello_c64_aldr_c_rrb_c) {
  SETUP();

  COMPARE_C64(ldr(c0, MemOperand(x1, w2, UXTW, 4)),
              "ldr c0, [x1, w2, uxtw #4]");
  COMPARE_C64(ldr(c0, MemOperand(x1, x2, SXTX, 0)), "ldr c0, [x1, x2, sxtx]");
  COMPARE_C64(ldr(c0, MemOperand(x1, x2, LSL, 4)), "ldr c0, [x1, x2, lsl #4]");
  COMPARE_C64(ldr(c0, MemOperand(x1, xzr, LSL, 4)),
              "ldr c0, [x1, xzr, lsl #4]");
  COMPARE_C64(ldr(c0, MemOperand(sp, x2, LSL, 4)), "ldr c0, [sp, x2, lsl #4]");
  COMPARE_C64(ldr(czr, MemOperand(x1, x2, LSL, 4)),
              "ldr czr, [x1, x2, lsl #4]");

  // A generic CPURegister works the same.
  COMPARE_C64(ldr(CPURegister(c0), MemOperand(x1, w2, UXTW, 4)),
              "ldr c0, [x1, w2, uxtw #4]");
  COMPARE_C64(ldr(CPURegister(czr), MemOperand(x1, x2, LSL, 4)),
              "ldr czr, [x1, x2, lsl #4]");
}

TEST(morello_c64_aldr_c_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(x1, 0)), "ldr c0, [x1]");
  COMPARE_MACRO_C64(Ldr(czr, MemOperand(sp, 0)), "ldr czr, [sp]");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(x1, 8176)), "ldr c0, [x1, #8176]");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(x1, w2, UXTW, 4)),
                    "ldr c0, [x1, w2, uxtw #4]");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(x1, x2, SXTX, 0)),
                    "ldr c0, [x1, x2, sxtx]");

  // Unencodable cases.
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(x1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add x16, x1, x16\n"
                    "ldr c0, [x16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(x1, 16, PreIndex)),
                    "add x1, x1, #0x10 (16)\n"
                    "ldr c0, [x1]");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(x1, -16, PostIndex)),
                    "ldr c0, [x1]\n"
                    "sub x1, x1, #0x10 (16)");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_C64(Ldr(CPURegister(c0), MemOperand(x1)), "ldr c0, [x1]");
}

TEST(morello_c64_aldr_r_ri_32) {
  SETUP();

  COMPARE_C64(ldr(w0, MemOperand(x1, 420)), "ldr w0, [x1, #420]");
  COMPARE_C64(ldr(w0, MemOperand(x1, 0)), "ldr w0, [x1]");
  COMPARE_C64(ldr(w0, MemOperand(x1, 2044)), "ldr w0, [x1, #2044]");
  COMPARE_C64(ldr(w0, MemOperand(sp, 420)), "ldr w0, [sp, #420]");
  COMPARE_C64(ldr(wzr, MemOperand(x1, 420)), "ldr wzr, [x1, #420]");

  // `ldur` can assemble to `ldr`, according to its LoadStoreScalingOption.
  COMPARE_C64(ldur(w0, MemOperand(c1, 256)), "ldr w0, [c1, #256]");
  COMPARE_C64(ldur(w0, MemOperand(c1, 2044)), "ldr w0, [c1, #2044]");

  // A generic CPURegister works the same.
  COMPARE_C64(ldr(CPURegister(w0), MemOperand(x1, 0)), "ldr w0, [x1]");
  COMPARE_C64(ldr(CPURegister(wzr), MemOperand(x1, 2044)),
              "ldr wzr, [x1, #2044]");
}

TEST(morello_c64_aldr_r_ri_64) {
  SETUP();

  COMPARE_C64(ldr(x0, MemOperand(x1, 840)), "ldr x0, [x1, #840]");
  COMPARE_C64(ldr(x0, MemOperand(x1, 0)), "ldr x0, [x1]");
  COMPARE_C64(ldr(x0, MemOperand(x1, 4088)), "ldr x0, [x1, #4088]");
  COMPARE_C64(ldr(x0, MemOperand(sp, 840)), "ldr x0, [sp, #840]");
  COMPARE_C64(ldr(xzr, MemOperand(x1, 840)), "ldr xzr, [x1, #840]");

  // `ldur` can assemble to `ldr`, according to its LoadStoreScalingOption.
  COMPARE_C64(ldur(x0, MemOperand(c1, 256)), "ldr x0, [c1, #256]");
  COMPARE_C64(ldur(x0, MemOperand(c1, 4088)), "ldr x0, [c1, #4088]");

  // A generic CPURegister works the same.
  COMPARE_C64(ldr(CPURegister(x0), MemOperand(x1, 0)), "ldr x0, [x1]");
  COMPARE_C64(ldr(CPURegister(xzr), MemOperand(x1, 4088)),
              "ldr xzr, [x1, #4088]");
}

TEST(morello_c64_aldr_r_rrb_32) {
  SETUP();

  COMPARE_C64(ldr(w0, MemOperand(x1, w2, UXTW, 2)),
              "ldr w0, [x1, w2, uxtw #2]");
  COMPARE_C64(ldr(w0, MemOperand(x1, x2, SXTX, 0)), "ldr w0, [x1, x2, sxtx]");
  COMPARE_C64(ldr(w0, MemOperand(x1, x2, LSL, 2)), "ldr w0, [x1, x2, lsl #2]");
  COMPARE_C64(ldr(w0, MemOperand(x1, xzr, LSL, 2)),
              "ldr w0, [x1, xzr, lsl #2]");
  COMPARE_C64(ldr(w0, MemOperand(sp, x2, LSL, 2)), "ldr w0, [sp, x2, lsl #2]");
  COMPARE_C64(ldr(wzr, MemOperand(x1, x2, LSL, 2)),
              "ldr wzr, [x1, x2, lsl #2]");

  // A generic CPURegister works the same.
  COMPARE_C64(ldr(CPURegister(w0), MemOperand(x1, w2, UXTW, 2)),
              "ldr w0, [x1, w2, uxtw #2]");
  COMPARE_C64(ldr(CPURegister(wzr), MemOperand(x1, x2, LSL, 2)),
              "ldr wzr, [x1, x2, lsl #2]");
}

TEST(morello_c64_aldr_r_rrb_64) {
  SETUP();

  COMPARE_C64(ldr(x0, MemOperand(x1, w2, UXTW, 3)),
              "ldr x0, [x1, w2, uxtw #3]");
  COMPARE_C64(ldr(x0, MemOperand(x1, x2, SXTX, 0)), "ldr x0, [x1, x2, sxtx]");
  COMPARE_C64(ldr(x0, MemOperand(x1, x2, LSL, 3)), "ldr x0, [x1, x2, lsl #3]");
  COMPARE_C64(ldr(x0, MemOperand(x1, xzr, LSL, 3)),
              "ldr x0, [x1, xzr, lsl #3]");
  COMPARE_C64(ldr(x0, MemOperand(sp, x2, LSL, 3)), "ldr x0, [sp, x2, lsl #3]");
  COMPARE_C64(ldr(xzr, MemOperand(x1, x2, LSL, 3)),
              "ldr xzr, [x1, x2, lsl #3]");

  // A generic CPURegister works the same.
  COMPARE_C64(ldr(CPURegister(x0), MemOperand(x1, w2, UXTW, 3)),
              "ldr x0, [x1, w2, uxtw #3]");
  COMPARE_C64(ldr(CPURegister(xzr), MemOperand(x1, x2, LSL, 3)),
              "ldr xzr, [x1, x2, lsl #3]");
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

  COMPARE_C64(ldur(c0, MemOperand(x1, 255)), "ldur c0, [x1, #255]");
  COMPARE_C64(ldur(c0, MemOperand(x1, -256)), "ldur c0, [x1, #-256]");
  COMPARE_C64(ldur(c0, MemOperand(x1, 0)), "ldur c0, [x1]");
  COMPARE_C64(ldur(c0, MemOperand(sp, 42)), "ldur c0, [sp, #42]");
  COMPARE_C64(ldur(czr, MemOperand(x1, 42)), "ldur czr, [x1, #42]");

  // `ldr` can assemble to `ldur`, according to its LoadStoreScalingOption.
  COMPARE_C64(ldr(c0, MemOperand(x1, -1)), "ldur c0, [x1, #-1]");
  COMPARE_C64(ldr(c0, MemOperand(x1, 42)), "ldur c0, [x1, #42]");

  // A generic CPURegister works the same.
  COMPARE_C64(ldur(CPURegister(c0), MemOperand(x1, 0)), "ldur c0, [x1]");
  COMPARE_C64(ldur(CPURegister(czr), MemOperand(x1, 42)),
              "ldur czr, [x1, #42]");
}

TEST(morello_c64_aldur_r_ri_32) {
  SETUP();

  COMPARE_C64(ldur(w0, MemOperand(x1, 255)), "ldur w0, [x1, #255]");
  COMPARE_C64(ldur(w0, MemOperand(x1, -256)), "ldur w0, [x1, #-256]");
  COMPARE_C64(ldur(w0, MemOperand(x1, 0)), "ldur w0, [x1]");
  COMPARE_C64(ldur(w0, MemOperand(sp, 42)), "ldur w0, [sp, #42]");
  COMPARE_C64(ldur(wzr, MemOperand(x1, 42)), "ldur wzr, [x1, #42]");

  // `ldr` can assemble to `ldur`, according to its LoadStoreScalingOption.
  COMPARE_C64(ldr(w0, MemOperand(x1, -1)), "ldur w0, [x1, #-1]");
  COMPARE_C64(ldr(w0, MemOperand(x1, 42)), "ldur w0, [x1, #42]");

  // A generic CPURegister works the same.
  COMPARE_C64(ldur(CPURegister(w0), MemOperand(x1, 0)), "ldur w0, [x1]");
  COMPARE_C64(ldur(CPURegister(wzr), MemOperand(x1, 42)),
              "ldur wzr, [x1, #42]");
}

TEST(morello_c64_aldur_r_ri_64) {
  SETUP();

  COMPARE_C64(ldur(x0, MemOperand(x1, 255)), "ldur x0, [x1, #255]");
  COMPARE_C64(ldur(x0, MemOperand(x1, -256)), "ldur x0, [x1, #-256]");
  COMPARE_C64(ldur(x0, MemOperand(x1, 0)), "ldur x0, [x1]");
  COMPARE_C64(ldur(x0, MemOperand(sp, 42)), "ldur x0, [sp, #42]");
  COMPARE_C64(ldur(xzr, MemOperand(x1, 42)), "ldur xzr, [x1, #42]");

  // `ldr` can assemble to `ldur`, according to its LoadStoreScalingOption.
  COMPARE_C64(ldr(x0, MemOperand(x1, -1)), "ldur x0, [x1, #-1]");
  COMPARE_C64(ldr(x0, MemOperand(x1, 42)), "ldur x0, [x1, #42]");

  // A generic CPURegister works the same.
  COMPARE_C64(ldur(CPURegister(x0), MemOperand(x1, 0)), "ldur x0, [x1]");
  COMPARE_C64(ldur(CPURegister(xzr), MemOperand(x1, 42)),
              "ldur xzr, [x1, #42]");
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

  COMPARE_C64(str(c0, MemOperand(x1, 6928)), "str c0, [x1, #6928]");
  COMPARE_C64(str(c0, MemOperand(x1, 0)), "str c0, [x1]");
  COMPARE_C64(str(c0, MemOperand(x1, 8176)), "str c0, [x1, #8176]");
  COMPARE_C64(str(c0, MemOperand(sp, 6928)), "str c0, [sp, #6928]");
  COMPARE_C64(str(czr, MemOperand(x1, 6928)), "str czr, [x1, #6928]");

  // A generic CPURegister works the same.
  COMPARE_C64(str(CPURegister(c0), MemOperand(x1, 0)), "str c0, [x1]");
  COMPARE_C64(str(CPURegister(czr), MemOperand(x1, 8176)),
              "str czr, [x1, #8176]");
}

TEST(morello_c64_astr_c_rrb_c) {
  SETUP();

  COMPARE_C64(str(c0, MemOperand(x1, w2, UXTW, 4)),
              "str c0, [x1, w2, uxtw #4]");
  COMPARE_C64(str(c0, MemOperand(x1, x2, SXTX, 0)), "str c0, [x1, x2, sxtx]");
  COMPARE_C64(str(c0, MemOperand(x1, x2, LSL, 4)), "str c0, [x1, x2, lsl #4]");
  COMPARE_C64(str(c0, MemOperand(x1, xzr, LSL, 4)),
              "str c0, [x1, xzr, lsl #4]");
  COMPARE_C64(str(c0, MemOperand(sp, x2, LSL, 4)), "str c0, [sp, x2, lsl #4]");
  COMPARE_C64(str(czr, MemOperand(x1, x2, LSL, 4)),
              "str czr, [x1, x2, lsl #4]");

  // A generic CPURegister works the same.
  COMPARE_C64(str(CPURegister(c0), MemOperand(x1, w2, UXTW, 4)),
              "str c0, [x1, w2, uxtw #4]");
  COMPARE_C64(str(CPURegister(czr), MemOperand(x1, x2, LSL, 4)),
              "str czr, [x1, x2, lsl #4]");
}

TEST(morello_c64_astr_c_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_C64(Str(c0, MemOperand(x1, 0)), "str c0, [x1]");
  COMPARE_MACRO_C64(Str(czr, MemOperand(sp, 0)), "str czr, [sp]");
  COMPARE_MACRO_C64(Str(c0, MemOperand(x1, 8176)), "str c0, [x1, #8176]");
  COMPARE_MACRO_C64(Str(c0, MemOperand(x1, w2, UXTW, 4)),
                    "str c0, [x1, w2, uxtw #4]");
  COMPARE_MACRO_C64(Str(c0, MemOperand(x1, x2, SXTX, 0)),
                    "str c0, [x1, x2, sxtx]");

  // Unencodable cases.
  COMPARE_MACRO_C64(Str(c0, MemOperand(x1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add x16, x1, x16\n"
                    "str c0, [x16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_C64(Str(c0, MemOperand(x1, 16, PreIndex)),
                    "add x1, x1, #0x10 (16)\n"
                    "str c0, [x1]");
  COMPARE_MACRO_C64(Str(c0, MemOperand(x1, -16, PostIndex)),
                    "str c0, [x1]\n"
                    "sub x1, x1, #0x10 (16)");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_C64(Str(CPURegister(c0), MemOperand(x1)), "str c0, [x1]");
}

TEST(morello_c64_astr_r_ri_32) {
  SETUP();

  COMPARE_C64(str(w0, MemOperand(x1, 420)), "str w0, [x1, #420]");
  COMPARE_C64(str(w0, MemOperand(x1, 0)), "str w0, [x1]");
  COMPARE_C64(str(w0, MemOperand(x1, 2044)), "str w0, [x1, #2044]");
  COMPARE_C64(str(w0, MemOperand(sp, 420)), "str w0, [sp, #420]");
  COMPARE_C64(str(wzr, MemOperand(x1, 420)), "str wzr, [x1, #420]");

  // `stur` can assemble to `str`, according to its LoadStoreScalingOption.
  COMPARE_C64(stur(w0, MemOperand(c1, 256)), "str w0, [c1, #256]");
  COMPARE_C64(stur(w0, MemOperand(c1, 2044)), "str w0, [c1, #2044]");

  // A generic CPURegister works the same.
  COMPARE_C64(str(CPURegister(w0), MemOperand(x1, 0)), "str w0, [x1]");
  COMPARE_C64(str(CPURegister(wzr), MemOperand(x1, 2044)),
              "str wzr, [x1, #2044]");
}

TEST(morello_c64_astr_r_ri_64) {
  SETUP();

  COMPARE_C64(str(x0, MemOperand(x1, 840)), "str x0, [x1, #840]");
  COMPARE_C64(str(x0, MemOperand(x1, 0)), "str x0, [x1]");
  COMPARE_C64(str(x0, MemOperand(x1, 4088)), "str x0, [x1, #4088]");
  COMPARE_C64(str(x0, MemOperand(sp, 840)), "str x0, [sp, #840]");
  COMPARE_C64(str(xzr, MemOperand(x1, 840)), "str xzr, [x1, #840]");

  // `stur` can assemble to `str`, according to its LoadStoreScalingOption.
  COMPARE_C64(stur(x0, MemOperand(c1, 256)), "str x0, [c1, #256]");
  COMPARE_C64(stur(x0, MemOperand(c1, 4088)), "str x0, [c1, #4088]");

  // A generic CPURegister works the same.
  COMPARE_C64(str(CPURegister(x0), MemOperand(x1, 0)), "str x0, [x1]");
  COMPARE_C64(str(CPURegister(xzr), MemOperand(x1, 4088)),
              "str xzr, [x1, #4088]");
}

TEST(morello_c64_astr_r_rrb_32) {
  SETUP();

  COMPARE_C64(str(w0, MemOperand(x1, w2, UXTW, 2)),
              "str w0, [x1, w2, uxtw #2]");
  COMPARE_C64(str(w0, MemOperand(x1, x2, SXTX, 0)), "str w0, [x1, x2, sxtx]");
  COMPARE_C64(str(w0, MemOperand(x1, x2, LSL, 2)), "str w0, [x1, x2, lsl #2]");
  COMPARE_C64(str(w0, MemOperand(x1, xzr, LSL, 2)),
              "str w0, [x1, xzr, lsl #2]");
  COMPARE_C64(str(w0, MemOperand(sp, x2, LSL, 2)), "str w0, [sp, x2, lsl #2]");
  COMPARE_C64(str(wzr, MemOperand(x1, x2, LSL, 2)),
              "str wzr, [x1, x2, lsl #2]");

  // A generic CPURegister works the same.
  COMPARE_C64(str(CPURegister(w0), MemOperand(x1, w2, UXTW, 2)),
              "str w0, [x1, w2, uxtw #2]");
  COMPARE_C64(str(CPURegister(wzr), MemOperand(x1, x2, LSL, 2)),
              "str wzr, [x1, x2, lsl #2]");
}

TEST(morello_c64_astr_r_rrb_64) {
  SETUP();

  COMPARE_C64(str(x0, MemOperand(x1, w2, UXTW, 3)),
              "str x0, [x1, w2, uxtw #3]");
  COMPARE_C64(str(x0, MemOperand(x1, x2, SXTX, 0)), "str x0, [x1, x2, sxtx]");
  COMPARE_C64(str(x0, MemOperand(x1, x2, LSL, 3)), "str x0, [x1, x2, lsl #3]");
  COMPARE_C64(str(x0, MemOperand(x1, xzr, LSL, 3)),
              "str x0, [x1, xzr, lsl #3]");
  COMPARE_C64(str(x0, MemOperand(sp, x2, LSL, 3)), "str x0, [sp, x2, lsl #3]");
  COMPARE_C64(str(xzr, MemOperand(x1, x2, LSL, 3)),
              "str xzr, [x1, x2, lsl #3]");

  // A generic CPURegister works the same.
  COMPARE_C64(str(CPURegister(x0), MemOperand(x1, w2, UXTW, 3)),
              "str x0, [x1, w2, uxtw #3]");
  COMPARE_C64(str(CPURegister(xzr), MemOperand(x1, x2, LSL, 3)),
              "str xzr, [x1, x2, lsl #3]");
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

  COMPARE_C64(stur(c0, MemOperand(x1, 255)), "stur c0, [x1, #255]");
  COMPARE_C64(stur(c0, MemOperand(x1, -256)), "stur c0, [x1, #-256]");
  COMPARE_C64(stur(c0, MemOperand(x1, 0)), "stur c0, [x1]");
  COMPARE_C64(stur(c0, MemOperand(sp, 42)), "stur c0, [sp, #42]");
  COMPARE_C64(stur(czr, MemOperand(x1, 42)), "stur czr, [x1, #42]");

  // `str` can assemble to `stur`, according to its LoadStoreScalingOption.
  COMPARE_C64(str(c0, MemOperand(x1, -1)), "stur c0, [x1, #-1]");
  COMPARE_C64(str(c0, MemOperand(x1, 42)), "stur c0, [x1, #42]");

  // A generic CPURegister works the same.
  COMPARE_C64(stur(CPURegister(c0), MemOperand(x1, 0)), "stur c0, [x1]");
  COMPARE_C64(stur(CPURegister(czr), MemOperand(x1, 42)),
              "stur czr, [x1, #42]");
}

TEST(morello_c64_astur_r_ri_32) {
  SETUP();

  COMPARE_C64(stur(w0, MemOperand(x1, 255)), "stur w0, [x1, #255]");
  COMPARE_C64(stur(w0, MemOperand(x1, -256)), "stur w0, [x1, #-256]");
  COMPARE_C64(stur(w0, MemOperand(x1, 0)), "stur w0, [x1]");
  COMPARE_C64(stur(w0, MemOperand(sp, 42)), "stur w0, [sp, #42]");
  COMPARE_C64(stur(wzr, MemOperand(x1, 42)), "stur wzr, [x1, #42]");

  // `str` can assemble to `stur`, according to its LoadStoreScalingOption.
  COMPARE_C64(str(w0, MemOperand(x1, -1)), "stur w0, [x1, #-1]");
  COMPARE_C64(str(w0, MemOperand(x1, 42)), "stur w0, [x1, #42]");

  // A generic CPURegister works the same.
  COMPARE_C64(stur(CPURegister(w0), MemOperand(x1, 0)), "stur w0, [x1]");
  COMPARE_C64(stur(CPURegister(wzr), MemOperand(x1, 42)),
              "stur wzr, [x1, #42]");
}

TEST(morello_c64_astur_r_ri_64) {
  SETUP();

  COMPARE_C64(stur(x0, MemOperand(x1, 255)), "stur x0, [x1, #255]");
  COMPARE_C64(stur(x0, MemOperand(x1, -256)), "stur x0, [x1, #-256]");
  COMPARE_C64(stur(x0, MemOperand(x1, 0)), "stur x0, [x1]");
  COMPARE_C64(stur(x0, MemOperand(sp, 42)), "stur x0, [sp, #42]");
  COMPARE_C64(stur(xzr, MemOperand(x1, 42)), "stur xzr, [x1, #42]");

  // `str` can assemble to `stur`, according to its LoadStoreScalingOption.
  COMPARE_C64(str(x0, MemOperand(x1, -1)), "stur x0, [x1, #-1]");
  COMPARE_C64(str(x0, MemOperand(x1, 42)), "stur x0, [x1, #42]");

  // A generic CPURegister works the same.
  COMPARE_C64(stur(CPURegister(x0), MemOperand(x1, 0)), "stur x0, [x1]");
  COMPARE_C64(stur(CPURegister(xzr), MemOperand(x1, 42)),
              "stur xzr, [x1, #42]");
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

  COMPARE_C64(ldr(c0, MemOperand(c1, 0, PostIndex)), "ldr c0, [c1], #0");
  COMPARE_C64(ldr(c0, MemOperand(c1, 672, PostIndex)), "ldr c0, [c1], #672");
  COMPARE_C64(ldr(c0, MemOperand(csp, 672, PostIndex)), "ldr c0, [csp], #672");
  COMPARE_C64(ldr(czr, MemOperand(c1, 672, PostIndex)), "ldr czr, [c1], #672");
  COMPARE_C64(ldr(c0, MemOperand(c1, -4096, PostIndex)),
              "ldr c0, [c1], #-4096");
  COMPARE_C64(ldr(c0, MemOperand(c1, 4080, PostIndex)), "ldr c0, [c1], #4080");

  // A generic CPURegister works the same.
  COMPARE_C64(ldr(CPURegister(c0), MemOperand(c1, 0, PostIndex)),
              "ldr c0, [c1], #0");
  COMPARE_C64(ldr(CPURegister(czr), MemOperand(c1, 672, PostIndex)),
              "ldr czr, [c1], #672");
}

TEST(morello_c64_ldr_c_rib_c) {
  SETUP();

  // Direct encodings.
  COMPARE_C64(ldr(c0, MemOperand(c1)), "ldr c0, [c1]");
  COMPARE_C64(ldr(c0, MemOperand(c1, 6928)), "ldr c0, [c1, #6928]");
  COMPARE_C64(ldr(c0, MemOperand(csp, 6928)), "ldr c0, [csp, #6928]");
  COMPARE_C64(ldr(czr, MemOperand(c1, 6928)), "ldr czr, [c1, #6928]");
  COMPARE_C64(ldr(c0, MemOperand(c1, 65520)), "ldr c0, [c1, #65520]");

  // `ldur` can assemble to `ldr`, according to its LoadStoreScalingOption.
  COMPARE_C64(ldur(c0, MemOperand(c1, 256)), "ldr c0, [c1, #256]");
  COMPARE_C64(ldur(c0, MemOperand(c1, 65520)), "ldr c0, [c1, #65520]");

  // A generic CPURegister works the same.
  COMPARE_C64(ldr(CPURegister(c0), MemOperand(c1)), "ldr c0, [c1]");
  COMPARE_C64(ldr(CPURegister(czr), MemOperand(c1, 6928)),
              "ldr czr, [c1, #6928]");
}

TEST(morello_c64_ldr_c_ribw_c) {
  SETUP();

  COMPARE_C64(ldr(c0, MemOperand(c1, 0, PreIndex)), "ldr c0, [c1, #0]!");
  COMPARE_C64(ldr(c0, MemOperand(c1, 672, PreIndex)), "ldr c0, [c1, #672]!");
  COMPARE_C64(ldr(c0, MemOperand(csp, 672, PreIndex)), "ldr c0, [csp, #672]!");
  COMPARE_C64(ldr(czr, MemOperand(c1, 672, PreIndex)), "ldr czr, [c1, #672]!");
  COMPARE_C64(ldr(c0, MemOperand(c1, -4096, PreIndex)),
              "ldr c0, [c1, #-4096]!");
  COMPARE_C64(ldr(c0, MemOperand(c1, 4080, PreIndex)), "ldr c0, [c1, #4080]!");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_C64(ldr(CPURegister(c0), MemOperand(c1, 0, PreIndex)),
              "ldr c0, [c1, #0]!");
  COMPARE_C64(ldr(CPURegister(czr), MemOperand(c1, 672, PreIndex)),
              "ldr czr, [c1, #672]!");
}

TEST(morello_c64_ldr_c_rrb_c) {
  SETUP();

  COMPARE_C64(ldr(c0, MemOperand(c1, w2, UXTW, 4)),
              "ldr c0, [c1, w2, uxtw #4]");
  COMPARE_C64(ldr(c0, MemOperand(c1, w2, SXTW)), "ldr c0, [c1, w2, sxtw]");
  COMPARE_C64(ldr(c0, MemOperand(c1, wzr, SXTW)), "ldr c0, [c1, wzr, sxtw]");
  COMPARE_C64(ldr(c0, MemOperand(csp, w2, SXTW)), "ldr c0, [csp, w2, sxtw]");
  COMPARE_C64(ldr(czr, MemOperand(c1, w2, SXTW)), "ldr czr, [c1, w2, sxtw]");
  COMPARE_C64(ldr(c0, MemOperand(c1, x2, SXTX)), "ldr c0, [c1, x2, sxtx]");
  COMPARE_C64(ldr(c0, MemOperand(c1, xzr, SXTX)), "ldr c0, [c1, xzr, sxtx]");
  COMPARE_C64(ldr(c0, MemOperand(c1, x2)), "ldr c0, [c1, x2]");
  COMPARE_C64(ldr(c0, MemOperand(c1, x2, LSL, 4)), "ldr c0, [c1, x2, lsl #4]");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_C64(ldr(CPURegister(c0), MemOperand(c1, w2, UXTW, 4)),
              "ldr c0, [c1, w2, uxtw #4]");
  COMPARE_C64(ldr(CPURegister(czr), MemOperand(c1, w2, SXTW)),
              "ldr czr, [c1, w2, sxtw]");
}

TEST(morello_c64_ldr_c_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, 0, PostIndex)), "ldr c0, [c1], #0");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, -4096, PostIndex)),
                    "ldr c0, [c1], #-4096");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, 4080, PostIndex)),
                    "ldr c0, [c1], #4080");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1)), "ldr c0, [c1]");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, 65520)), "ldr c0, [c1, #65520]");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, 0, PreIndex)), "ldr c0, [c1, #0]!");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, -4096, PreIndex)),
                    "ldr c0, [c1, #-4096]!");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, 4080, PreIndex)),
                    "ldr c0, [c1, #4080]!");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, w2, SXTW, 4)),
                    "ldr c0, [c1, w2, sxtw #4]");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, x2)), "ldr c0, [c1, x2]");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, -16)), "ldur c0, [c1, #-16]");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, -256)), "ldur c0, [c1, #-256]");
  COMPARE_MACRO_C64(Ldr(czr, MemOperand(c1, 255)), "ldur czr, [c1, #255]");

  // Unencodable cases.
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add c16, c1, x16, uxtx\n"
                    "ldr c0, [c16]");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, 0x4242, PreIndex)),
                    "mov x16, #0x4242\n"
                    "add c1, c1, x16, uxtx\n"
                    "ldr c0, [c1]");
  COMPARE_MACRO_C64(Ldr(c0, MemOperand(c1, 0x4242, PostIndex)),
                    "ldr c0, [c1]\n"
                    "mov x16, #0x4242\n"
                    "add c1, c1, x16, uxtx");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_C64(Ldr(CPURegister(c0), MemOperand(c1, 0, PostIndex)),
                    "ldr c0, [c1], #0");
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

  COMPARE_C64(str(c0, MemOperand(c1, 0, PostIndex)), "str c0, [c1], #0");
  COMPARE_C64(str(c0, MemOperand(c1, 672, PostIndex)), "str c0, [c1], #672");
  COMPARE_C64(str(c0, MemOperand(csp, 672, PostIndex)), "str c0, [csp], #672");
  COMPARE_C64(str(czr, MemOperand(c1, 672, PostIndex)), "str czr, [c1], #672");
  COMPARE_C64(str(c0, MemOperand(c1, -4096, PostIndex)),
              "str c0, [c1], #-4096");
  COMPARE_C64(str(c0, MemOperand(c1, 4080, PostIndex)), "str c0, [c1], #4080");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_C64(str(CPURegister(c0), MemOperand(c1, 0, PostIndex)),
              "str c0, [c1], #0");
  COMPARE_C64(str(CPURegister(czr), MemOperand(c1, 672, PostIndex)),
              "str czr, [c1], #672");
}

TEST(morello_c64_str_c_rib_c) {
  SETUP();

  // Direct encodings.
  COMPARE_C64(str(c0, MemOperand(c1)), "str c0, [c1]");
  COMPARE_C64(str(c0, MemOperand(c1, 6928)), "str c0, [c1, #6928]");
  COMPARE_C64(str(c0, MemOperand(csp, 6928)), "str c0, [csp, #6928]");
  COMPARE_C64(str(czr, MemOperand(c1, 6928)), "str czr, [c1, #6928]");
  COMPARE_C64(str(c0, MemOperand(c1, 65520)), "str c0, [c1, #65520]");

  // `stur` can assemble to `str`, according to its LoadStoreScalingOption.
  COMPARE_C64(stur(c0, MemOperand(c1, 256)), "str c0, [c1, #256]");
  COMPARE_C64(stur(c0, MemOperand(c1, 65520)), "str c0, [c1, #65520]");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_C64(str(CPURegister(c0), MemOperand(c1)), "str c0, [c1]");
  COMPARE_C64(str(CPURegister(czr), MemOperand(c1, 6928)),
              "str czr, [c1, #6928]");
}

TEST(morello_c64_str_c_ribw_c) {
  SETUP();

  COMPARE_C64(str(c0, MemOperand(c1, 0, PreIndex)), "str c0, [c1, #0]!");
  COMPARE_C64(str(c0, MemOperand(c1, 672, PreIndex)), "str c0, [c1, #672]!");
  COMPARE_C64(str(c0, MemOperand(csp, 672, PreIndex)), "str c0, [csp, #672]!");
  COMPARE_C64(str(czr, MemOperand(c1, 672, PreIndex)), "str czr, [c1, #672]!");
  COMPARE_C64(str(c0, MemOperand(c1, -4096, PreIndex)),
              "str c0, [c1, #-4096]!");
  COMPARE_C64(str(c0, MemOperand(c1, 4080, PreIndex)), "str c0, [c1, #4080]!");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_C64(str(CPURegister(c0), MemOperand(c1, 0, PreIndex)),
              "str c0, [c1, #0]!");
  COMPARE_C64(str(CPURegister(czr), MemOperand(c1, 672, PreIndex)),
              "str czr, [c1, #672]!");
}

TEST(morello_c64_str_c_rrb_c) {
  SETUP();

  COMPARE_C64(str(c0, MemOperand(c1, w2, UXTW, 4)),
              "str c0, [c1, w2, uxtw #4]");
  COMPARE_C64(str(c0, MemOperand(c1, w2, SXTW)), "str c0, [c1, w2, sxtw]");
  COMPARE_C64(str(c0, MemOperand(c1, wzr, SXTW)), "str c0, [c1, wzr, sxtw]");
  COMPARE_C64(str(c0, MemOperand(csp, w2, SXTW)), "str c0, [csp, w2, sxtw]");
  COMPARE_C64(str(czr, MemOperand(c1, w2, SXTW)), "str czr, [c1, w2, sxtw]");
  COMPARE_C64(str(c0, MemOperand(c1, x2, SXTX)), "str c0, [c1, x2, sxtx]");
  COMPARE_C64(str(c0, MemOperand(c1, xzr, SXTX)), "str c0, [c1, xzr, sxtx]");
  COMPARE_C64(str(c0, MemOperand(c1, x2)), "str c0, [c1, x2]");
  COMPARE_C64(str(c0, MemOperand(c1, x2, LSL, 4)), "str c0, [c1, x2, lsl #4]");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_C64(str(CPURegister(c0), MemOperand(c1, w2, UXTW, 4)),
              "str c0, [c1, w2, uxtw #4]");
  COMPARE_C64(str(CPURegister(czr), MemOperand(c1, w2, SXTW)),
              "str czr, [c1, w2, sxtw]");
}

TEST(morello_c64_str_c_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, 0, PostIndex)), "str c0, [c1], #0");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, -4096, PostIndex)),
                    "str c0, [c1], #-4096");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, 4080, PostIndex)),
                    "str c0, [c1], #4080");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1)), "str c0, [c1]");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, 65520)), "str c0, [c1, #65520]");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, 0, PreIndex)), "str c0, [c1, #0]!");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, -4096, PreIndex)),
                    "str c0, [c1, #-4096]!");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, 4080, PreIndex)),
                    "str c0, [c1, #4080]!");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, w2, SXTW, 4)),
                    "str c0, [c1, w2, sxtw #4]");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, x2)), "str c0, [c1, x2]");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, -16)), "stur c0, [c1, #-16]");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, -256)), "stur c0, [c1, #-256]");
  COMPARE_MACRO_C64(Str(czr, MemOperand(c1, 255)), "stur czr, [c1, #255]");

  // Unencodable cases.
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add c16, c1, x16, uxtx\n"
                    "str c0, [c16]");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, 0x4242, PreIndex)),
                    "mov x16, #0x4242\n"
                    "add c1, c1, x16, uxtx\n"
                    "str c0, [c1]");
  COMPARE_MACRO_C64(Str(c0, MemOperand(c1, 0x4242, PostIndex)),
                    "str c0, [c1]\n"
                    "mov x16, #0x4242\n"
                    "add c1, c1, x16, uxtx");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_C64(Str(CPURegister(c0), MemOperand(c1, 0, PostIndex)),
                    "str c0, [c1], #0");
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
