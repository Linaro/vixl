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

TEST(morello_a64_adr_label) {
  typedef DisasmTestUtilMacroAssembler MacroAssembler;

  SETUP();

  // Morello does not change this instruction, but if `adr` refers to a C64
  // label, we set bit 0 (for correct interworking with `br` etc).

  auto adr_x4 = [&masm](Label* l) { masm.adr(x4, l); };
  auto adr_xzr = [&masm](Label* l) { masm.adr(xzr, l); };

  // When targeting C64, the bottom bit should be set (for use in `br`, etc).
  COMPARE_PREFIX_A64(WithA64LabelBefore(adr_x4), "adr x4, #+0x0 (addr 0x");
  COMPARE_PREFIX_A64(WithC64LabelBefore(adr_xzr), "adr xzr, #+0x1 (addr 0x");
  COMPARE_PREFIX_A64(WithDataLabelBefore(adr_x4), "adr x4, #+0x0 (addr 0x");
  COMPARE_PREFIX_A64(WithA64LabelAfter(adr_x4), "adr x4, #+0x4 (addr 0x");
  COMPARE_PREFIX_A64(WithC64LabelAfter(adr_x4), "adr x4, #+0x5 (addr 0x");
  COMPARE_PREFIX_A64(WithDataLabelAfter(adr_x4), "adr x4, #+0x4 (addr 0x");

  auto macro_adr_x4 = [&masm](Label* l) { masm.Adr(x4, l); };
  COMPARE_MACRO_PREFIX_A64(WithA64LabelBefore(macro_adr_x4),
                           "adr x4, #+0x0 (addr 0x");
  COMPARE_MACRO_PREFIX_A64(WithC64LabelAfter(macro_adr_x4),
                           "adr x4, #+0x5 (addr 0x");
}

TEST(morello_a64_adrp_c_i_c) {
  typedef DisasmTestUtilMacroAssembler MacroAssembler;

  SETUP();

  // Morello does not change this instruction in A64 mode, but check that the
  // C64 interworking bit is not applied when the offset is page-scaled.

  auto adrp_x4 = [&masm](Label* l) { masm.adrp(x4, l); };
  auto adrp_xzr = [&masm](Label* l) { masm.adrp(xzr, l); };

  // Check that the C64 interworking bit is not applied when the offset is
  // page-scaled.
  COMPARE_PREFIX_A64(WithA64LabelBefore(adrp_x4), "adrp x4, #+0x0 (addr 0x");
  COMPARE_PREFIX_A64(WithC64LabelBefore(adrp_xzr), "adrp xzr, #+0x0 (addr 0x");
  COMPARE_PREFIX_A64(WithDataLabelBefore(adrp_x4), "adrp x4, #+0x0 (addr 0x");
  COMPARE_PREFIX_A64(WithA64LabelAfter(adrp_x4), "adrp x4, #+0x0 (addr 0x");
  COMPARE_PREFIX_A64(WithC64LabelAfter(adrp_x4), "adrp x4, #+0x0 (addr 0x");
  COMPARE_PREFIX_A64(WithDataLabelAfter(adrp_x4), "adrp x4, #+0x0 (addr 0x");

  auto macro_adrp_x4 = [&masm](Label* l) { masm.Adrp(x4, l); };
  COMPARE_MACRO_PREFIX_A64(WithA64LabelBefore(macro_adrp_x4),
                           "adrp x4, #+0x0 (addr 0x");
  COMPARE_MACRO_PREFIX_A64(WithC64LabelAfter(macro_adrp_x4),
                           "adrp x4, #+0x0 (addr 0x");
}

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

  COMPARE_A64(ldr(c0, MemOperand(c1, 6928)), "ldr c0, [c1, #6928]");
  COMPARE_A64(ldr(c0, MemOperand(c1, 0)), "ldr c0, [c1]");
  COMPARE_A64(ldr(c0, MemOperand(c1, 8176)), "ldr c0, [c1, #8176]");
  COMPARE_A64(ldr(c0, MemOperand(csp, 6928)), "ldr c0, [csp, #6928]");
  COMPARE_A64(ldr(czr, MemOperand(c1, 6928)), "ldr czr, [c1, #6928]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldr(CPURegister(c0), MemOperand(c1, 0)), "ldr c0, [c1]");
  COMPARE_A64(ldr(CPURegister(czr), MemOperand(c1, 8176)),
              "ldr czr, [c1, #8176]");
}

TEST(morello_a64_aldr_c_rrb_c) {
  SETUP();

  COMPARE_A64(ldr(c0, MemOperand(c1, w2, UXTW, 4)),
              "ldr c0, [c1, w2, uxtw #4]");
  COMPARE_A64(ldr(c0, MemOperand(c1, x2, SXTX, 0)), "ldr c0, [c1, x2, sxtx]");
  COMPARE_A64(ldr(c0, MemOperand(c1, x2, LSL, 4)), "ldr c0, [c1, x2, lsl #4]");
  COMPARE_A64(ldr(c0, MemOperand(c1, xzr, LSL, 4)),
              "ldr c0, [c1, xzr, lsl #4]");
  COMPARE_A64(ldr(c0, MemOperand(csp, x2, LSL, 4)),
              "ldr c0, [csp, x2, lsl #4]");
  COMPARE_A64(ldr(czr, MemOperand(c1, x2, LSL, 4)),
              "ldr czr, [c1, x2, lsl #4]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldr(CPURegister(c0), MemOperand(c1, w2, UXTW, 4)),
              "ldr c0, [c1, w2, uxtw #4]");
  COMPARE_A64(ldr(CPURegister(czr), MemOperand(c1, x2, LSL, 4)),
              "ldr czr, [c1, x2, lsl #4]");

  // Unallocated encodings (where option<1> = 0).
  COMPARE_PREFIX_A64(dci(0b11000010111'00000'000'0'1'1'00000'00000),
                     "unallocated");
  COMPARE_PREFIX_A64(dci(0b11000010111'11111'101'1'1'1'11111'11111),
                     "unallocated");
}

TEST(morello_a64_aldr_c_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(c1, 0)), "ldr c0, [c1]");
  COMPARE_MACRO_A64(Ldr(czr, MemOperand(csp, 0)), "ldr czr, [csp]");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(c1, 8176)), "ldr c0, [c1, #8176]");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(c1, w2, UXTW, 4)),
                    "ldr c0, [c1, w2, uxtw #4]");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(c1, x2, SXTX, 0)),
                    "ldr c0, [c1, x2, sxtx]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(c1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add c16, c1, x16, uxtx\n"
                    "ldr c0, [c16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(c1, 16, PreIndex)),
                    "add c1, c1, #0x10 (16)\n"
                    "ldr c0, [c1]");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(c1, -16, PostIndex)),
                    "ldr c0, [c1]\n"
                    "sub c1, c1, #0x10 (16)");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Ldr(CPURegister(c0), MemOperand(c1)), "ldr c0, [c1]");
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

  COMPARE_A64(ldur(c0, MemOperand(c1, 255)), "ldur c0, [c1, #255]");
  COMPARE_A64(ldur(c0, MemOperand(c1, -256)), "ldur c0, [c1, #-256]");
  COMPARE_A64(ldur(c0, MemOperand(c1, 0)), "ldur c0, [c1]");
  COMPARE_A64(ldur(c0, MemOperand(csp, 42)), "ldur c0, [csp, #42]");
  COMPARE_A64(ldur(czr, MemOperand(c1, 42)), "ldur czr, [c1, #42]");

  // `ldr` can assemble to `ldur`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldr(c0, MemOperand(c1, -1)), "ldur c0, [c1, #-1]");
  COMPARE_A64(ldr(c0, MemOperand(c1, 42)), "ldur c0, [c1, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldur(CPURegister(c0), MemOperand(c1, 0)), "ldur c0, [c1]");
  COMPARE_A64(ldur(CPURegister(czr), MemOperand(c1, 42)),
              "ldur czr, [c1, #42]");
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

  COMPARE_A64(str(c0, MemOperand(c1, 6928)), "str c0, [c1, #6928]");
  COMPARE_A64(str(c0, MemOperand(c1, 0)), "str c0, [c1]");
  COMPARE_A64(str(c0, MemOperand(c1, 8176)), "str c0, [c1, #8176]");
  COMPARE_A64(str(c0, MemOperand(csp, 6928)), "str c0, [csp, #6928]");
  COMPARE_A64(str(czr, MemOperand(c1, 6928)), "str czr, [c1, #6928]");

  // A generic CPURegister works the same.
  COMPARE_A64(str(CPURegister(c0), MemOperand(c1, 0)), "str c0, [c1]");
  COMPARE_A64(str(CPURegister(czr), MemOperand(c1, 8176)),
              "str czr, [c1, #8176]");
}

TEST(morello_a64_astr_c_rrb_c) {
  SETUP();

  COMPARE_A64(str(c0, MemOperand(c1, w2, UXTW, 4)),
              "str c0, [c1, w2, uxtw #4]");
  COMPARE_A64(str(c0, MemOperand(c1, x2, SXTX, 0)), "str c0, [c1, x2, sxtx]");
  COMPARE_A64(str(c0, MemOperand(c1, x2, LSL, 4)), "str c0, [c1, x2, lsl #4]");
  COMPARE_A64(str(c0, MemOperand(c1, xzr, LSL, 4)),
              "str c0, [c1, xzr, lsl #4]");
  COMPARE_A64(str(c0, MemOperand(csp, x2, LSL, 4)),
              "str c0, [csp, x2, lsl #4]");
  COMPARE_A64(str(czr, MemOperand(c1, x2, LSL, 4)),
              "str czr, [c1, x2, lsl #4]");

  // A generic CPURegister works the same.
  COMPARE_A64(str(CPURegister(c0), MemOperand(c1, w2, UXTW, 4)),
              "str c0, [c1, w2, uxtw #4]");
  COMPARE_A64(str(CPURegister(czr), MemOperand(c1, x2, LSL, 4)),
              "str czr, [c1, x2, lsl #4]");

  // Unallocated encodings (where option<1> = 0).
  COMPARE_PREFIX_A64(dci(0b11000010111'00000'000'0'1'1'00000'00000),
                     "unallocated");
  COMPARE_PREFIX_A64(dci(0b11000010111'11111'101'1'1'1'11111'11111),
                     "unallocated");
}

TEST(morello_a64_astr_c_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Str(c0, MemOperand(c1, 0)), "str c0, [c1]");
  COMPARE_MACRO_A64(Str(czr, MemOperand(csp, 0)), "str czr, [csp]");
  COMPARE_MACRO_A64(Str(c0, MemOperand(c1, 8176)), "str c0, [c1, #8176]");
  COMPARE_MACRO_A64(Str(c0, MemOperand(c1, w2, UXTW, 4)),
                    "str c0, [c1, w2, uxtw #4]");
  COMPARE_MACRO_A64(Str(c0, MemOperand(c1, x2, SXTX, 0)),
                    "str c0, [c1, x2, sxtx]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Str(c0, MemOperand(c1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add c16, c1, x16, uxtx\n"
                    "str c0, [c16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_A64(Str(c0, MemOperand(c1, 16, PreIndex)),
                    "add c1, c1, #0x10 (16)\n"
                    "str c0, [c1]");
  COMPARE_MACRO_A64(Str(c0, MemOperand(c1, -16, PostIndex)),
                    "str c0, [c1]\n"
                    "sub c1, c1, #0x10 (16)");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Str(CPURegister(c0), MemOperand(c1)), "str c0, [c1]");
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

  COMPARE_A64(stur(c0, MemOperand(c1, 255)), "stur c0, [c1, #255]");
  COMPARE_A64(stur(c0, MemOperand(c1, -256)), "stur c0, [c1, #-256]");
  COMPARE_A64(stur(c0, MemOperand(c1, 0)), "stur c0, [c1]");
  COMPARE_A64(stur(c0, MemOperand(csp, 42)), "stur c0, [csp, #42]");
  COMPARE_A64(stur(czr, MemOperand(c1, 42)), "stur czr, [c1, #42]");

  // `str` can assemble to `stur`, according to its LoadStoreScalingOption.
  COMPARE_A64(str(c0, MemOperand(c1, -1)), "stur c0, [c1, #-1]");
  COMPARE_A64(str(c0, MemOperand(c1, 42)), "stur c0, [c1, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(stur(CPURegister(c0), MemOperand(c1, 0)), "stur c0, [c1]");
  COMPARE_A64(stur(CPURegister(czr), MemOperand(c1, 42)),
              "stur czr, [c1, #42]");
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

TEST(morello_a64_ldr_c_riaw_c) {
  SETUP();

  COMPARE_A64(ldr(c0, MemOperand(x1, 0, PostIndex)), "ldr c0, [x1], #0");
  COMPARE_A64(ldr(c0, MemOperand(x1, 672, PostIndex)), "ldr c0, [x1], #672");
  COMPARE_A64(ldr(c0, MemOperand(sp, 672, PostIndex)), "ldr c0, [sp], #672");
  COMPARE_A64(ldr(czr, MemOperand(x1, 672, PostIndex)), "ldr czr, [x1], #672");
  COMPARE_A64(ldr(c0, MemOperand(x1, -4096, PostIndex)),
              "ldr c0, [x1], #-4096");
  COMPARE_A64(ldr(c0, MemOperand(x1, 4080, PostIndex)), "ldr c0, [x1], #4080");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_A64(ldr(CPURegister(c0), MemOperand(x1, 0, PostIndex)),
              "ldr c0, [x1], #0");
  COMPARE_A64(ldr(CPURegister(czr), MemOperand(x1, 672, PostIndex)),
              "ldr czr, [x1], #672");
}

TEST(morello_a64_ldr_c_rib_c) {
  SETUP();

  // Direct encodings.
  COMPARE_A64(ldr(c0, MemOperand(x1)), "ldr c0, [x1]");
  COMPARE_A64(ldr(c0, MemOperand(x1, 6928)), "ldr c0, [x1, #6928]");
  COMPARE_A64(ldr(c0, MemOperand(sp, 6928)), "ldr c0, [sp, #6928]");
  COMPARE_A64(ldr(czr, MemOperand(x1, 6928)), "ldr czr, [x1, #6928]");
  COMPARE_A64(ldr(c0, MemOperand(x1, 65520)), "ldr c0, [x1, #65520]");

  // `ldur` can assemble to `ldr`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldur(c0, MemOperand(x1, 256)), "ldr c0, [x1, #256]");
  COMPARE_A64(ldur(c0, MemOperand(x1, 65520)), "ldr c0, [x1, #65520]");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_A64(ldr(CPURegister(c0), MemOperand(x1)), "ldr c0, [x1]");
  COMPARE_A64(ldr(CPURegister(czr), MemOperand(x1, 6928)),
              "ldr czr, [x1, #6928]");
}

TEST(morello_a64_ldr_c_ribw_c) {
  SETUP();

  COMPARE_A64(ldr(c0, MemOperand(x1, 0, PreIndex)), "ldr c0, [x1, #0]!");
  COMPARE_A64(ldr(c0, MemOperand(x1, 672, PreIndex)), "ldr c0, [x1, #672]!");
  COMPARE_A64(ldr(c0, MemOperand(sp, 672, PreIndex)), "ldr c0, [sp, #672]!");
  COMPARE_A64(ldr(czr, MemOperand(x1, 672, PreIndex)), "ldr czr, [x1, #672]!");
  COMPARE_A64(ldr(c0, MemOperand(x1, -4096, PreIndex)),
              "ldr c0, [x1, #-4096]!");
  COMPARE_A64(ldr(c0, MemOperand(x1, 4080, PreIndex)), "ldr c0, [x1, #4080]!");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_A64(ldr(CPURegister(c0), MemOperand(x1, 0, PreIndex)),
              "ldr c0, [x1, #0]!");
  COMPARE_A64(ldr(CPURegister(czr), MemOperand(x1, 672, PreIndex)),
              "ldr czr, [x1, #672]!");
}

TEST(morello_a64_ldr_c_rrb_c) {
  SETUP();

  COMPARE_A64(ldr(c0, MemOperand(x1, w2, UXTW, 4)),
              "ldr c0, [x1, w2, uxtw #4]");
  COMPARE_A64(ldr(c0, MemOperand(x1, w2, SXTW)), "ldr c0, [x1, w2, sxtw]");
  COMPARE_A64(ldr(c0, MemOperand(x1, wzr, SXTW)), "ldr c0, [x1, wzr, sxtw]");
  COMPARE_A64(ldr(c0, MemOperand(sp, w2, SXTW)), "ldr c0, [sp, w2, sxtw]");
  COMPARE_A64(ldr(czr, MemOperand(x1, w2, SXTW)), "ldr czr, [x1, w2, sxtw]");
  COMPARE_A64(ldr(c0, MemOperand(x1, x2, SXTX)), "ldr c0, [x1, x2, sxtx]");
  COMPARE_A64(ldr(c0, MemOperand(x1, xzr, SXTX)), "ldr c0, [x1, xzr, sxtx]");
  COMPARE_A64(ldr(c0, MemOperand(x1, x2)), "ldr c0, [x1, x2]");
  COMPARE_A64(ldr(c0, MemOperand(x1, x2, LSL, 4)), "ldr c0, [x1, x2, lsl #4]");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_A64(ldr(CPURegister(c0), MemOperand(x1, w2, UXTW, 4)),
              "ldr c0, [x1, w2, uxtw #4]");
  COMPARE_A64(ldr(CPURegister(czr), MemOperand(x1, w2, SXTW)),
              "ldr czr, [x1, w2, sxtw]");
}

TEST(morello_a64_ldr_c_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, 0, PostIndex)), "ldr c0, [x1], #0");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, -4096, PostIndex)),
                    "ldr c0, [x1], #-4096");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, 4080, PostIndex)),
                    "ldr c0, [x1], #4080");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1)), "ldr c0, [x1]");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, 65520)), "ldr c0, [x1, #65520]");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, 0, PreIndex)), "ldr c0, [x1, #0]!");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, -4096, PreIndex)),
                    "ldr c0, [x1, #-4096]!");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, 4080, PreIndex)),
                    "ldr c0, [x1, #4080]!");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, w2, SXTW, 4)),
                    "ldr c0, [x1, w2, sxtw #4]");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, x2)), "ldr c0, [x1, x2]");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, -16)), "ldur c0, [x1, #-16]");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, -256)), "ldur c0, [x1, #-256]");
  COMPARE_MACRO_A64(Ldr(czr, MemOperand(x1, 255)), "ldur czr, [x1, #255]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add x16, x1, x16\n"
                    "ldr c0, [x16]");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, 0x4242, PreIndex)),
                    "mov x16, #0x4242\n"
                    "add x1, x1, x16\n"
                    "ldr c0, [x1]");
  COMPARE_MACRO_A64(Ldr(c0, MemOperand(x1, 0x4242, PostIndex)),
                    "ldr c0, [x1]\n"
                    "mov x16, #0x4242\n"
                    "add x1, x1, x16");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Ldr(CPURegister(c0), MemOperand(x1, 0, PostIndex)),
                    "ldr c0, [x1], #0");
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

  COMPARE_A64(str(c0, MemOperand(x1, 0, PostIndex)), "str c0, [x1], #0");
  COMPARE_A64(str(c0, MemOperand(x1, 672, PostIndex)), "str c0, [x1], #672");
  COMPARE_A64(str(c0, MemOperand(sp, 672, PostIndex)), "str c0, [sp], #672");
  COMPARE_A64(str(czr, MemOperand(x1, 672, PostIndex)), "str czr, [x1], #672");
  COMPARE_A64(str(c0, MemOperand(x1, -4096, PostIndex)),
              "str c0, [x1], #-4096");
  COMPARE_A64(str(c0, MemOperand(x1, 4080, PostIndex)), "str c0, [x1], #4080");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_A64(str(CPURegister(c0), MemOperand(x1, 0, PostIndex)),
              "str c0, [x1], #0");
  COMPARE_A64(str(CPURegister(czr), MemOperand(x1, 672, PostIndex)),
              "str czr, [x1], #672");
}

TEST(morello_a64_str_c_rib_c) {
  SETUP();

  // Direct encodings.
  COMPARE_A64(str(c0, MemOperand(x1)), "str c0, [x1]");
  COMPARE_A64(str(c0, MemOperand(x1, 6928)), "str c0, [x1, #6928]");
  COMPARE_A64(str(c0, MemOperand(sp, 6928)), "str c0, [sp, #6928]");
  COMPARE_A64(str(czr, MemOperand(x1, 6928)), "str czr, [x1, #6928]");
  COMPARE_A64(str(c0, MemOperand(x1, 65520)), "str c0, [x1, #65520]");

  // `stur` can assemble to `str`, according to its LoadStoreScalingOption.
  COMPARE_A64(stur(c0, MemOperand(x1, 256)), "str c0, [x1, #256]");
  COMPARE_A64(stur(c0, MemOperand(x1, 65520)), "str c0, [x1, #65520]");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_A64(str(CPURegister(c0), MemOperand(x1)), "str c0, [x1]");
  COMPARE_A64(str(CPURegister(czr), MemOperand(x1, 6928)),
              "str czr, [x1, #6928]");
}

TEST(morello_a64_str_c_ribw_c) {
  SETUP();

  COMPARE_A64(str(c0, MemOperand(x1, 0, PreIndex)), "str c0, [x1, #0]!");
  COMPARE_A64(str(c0, MemOperand(x1, 672, PreIndex)), "str c0, [x1, #672]!");
  COMPARE_A64(str(c0, MemOperand(sp, 672, PreIndex)), "str c0, [sp, #672]!");
  COMPARE_A64(str(czr, MemOperand(x1, 672, PreIndex)), "str czr, [x1, #672]!");
  COMPARE_A64(str(c0, MemOperand(x1, -4096, PreIndex)),
              "str c0, [x1, #-4096]!");
  COMPARE_A64(str(c0, MemOperand(x1, 4080, PreIndex)), "str c0, [x1, #4080]!");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_A64(str(CPURegister(c0), MemOperand(x1, 0, PreIndex)),
              "str c0, [x1, #0]!");
  COMPARE_A64(str(CPURegister(czr), MemOperand(x1, 672, PreIndex)),
              "str czr, [x1, #672]!");
}

TEST(morello_a64_str_c_rrb_c) {
  SETUP();

  COMPARE_A64(str(c0, MemOperand(x1, w2, UXTW, 4)),
              "str c0, [x1, w2, uxtw #4]");
  COMPARE_A64(str(c0, MemOperand(x1, w2, SXTW)), "str c0, [x1, w2, sxtw]");
  COMPARE_A64(str(c0, MemOperand(x1, wzr, SXTW)), "str c0, [x1, wzr, sxtw]");
  COMPARE_A64(str(c0, MemOperand(sp, w2, SXTW)), "str c0, [sp, w2, sxtw]");
  COMPARE_A64(str(czr, MemOperand(x1, w2, SXTW)), "str czr, [x1, w2, sxtw]");
  COMPARE_A64(str(c0, MemOperand(x1, x2, SXTX)), "str c0, [x1, x2, sxtx]");
  COMPARE_A64(str(c0, MemOperand(x1, xzr, SXTX)), "str c0, [x1, xzr, sxtx]");
  COMPARE_A64(str(c0, MemOperand(x1, x2)), "str c0, [x1, x2]");
  COMPARE_A64(str(c0, MemOperand(x1, x2, LSL, 4)), "str c0, [x1, x2, lsl #4]");

  // The generic (CPURegister) API calls the same helper.
  COMPARE_A64(str(CPURegister(c0), MemOperand(x1, w2, UXTW, 4)),
              "str c0, [x1, w2, uxtw #4]");
  COMPARE_A64(str(CPURegister(czr), MemOperand(x1, w2, SXTW)),
              "str czr, [x1, w2, sxtw]");
}

TEST(morello_a64_str_c_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, 0, PostIndex)), "str c0, [x1], #0");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, -4096, PostIndex)),
                    "str c0, [x1], #-4096");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, 4080, PostIndex)),
                    "str c0, [x1], #4080");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1)), "str c0, [x1]");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, 65520)), "str c0, [x1, #65520]");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, 0, PreIndex)), "str c0, [x1, #0]!");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, -4096, PreIndex)),
                    "str c0, [x1, #-4096]!");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, 4080, PreIndex)),
                    "str c0, [x1, #4080]!");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, w2, SXTW, 4)),
                    "str c0, [x1, w2, sxtw #4]");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, x2)), "str c0, [x1, x2]");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, -16)), "stur c0, [x1, #-16]");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, -256)), "stur c0, [x1, #-256]");
  COMPARE_MACRO_A64(Str(czr, MemOperand(x1, 255)), "stur czr, [x1, #255]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add x16, x1, x16\n"
                    "str c0, [x16]");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, 0x4242, PreIndex)),
                    "mov x16, #0x4242\n"
                    "add x1, x1, x16\n"
                    "str c0, [x1]");
  COMPARE_MACRO_A64(Str(c0, MemOperand(x1, 0x4242, PostIndex)),
                    "str c0, [x1]\n"
                    "mov x16, #0x4242\n"
                    "add x1, x1, x16");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Str(CPURegister(c0), MemOperand(x1, 0, PostIndex)),
                    "str c0, [x1], #0");
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
