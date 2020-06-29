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

  COMPARE_A64(ldar(c0, MemOperand(c1)), "ldar c0, [c1]");
  COMPARE_A64(ldar(c0, MemOperand(csp)), "ldar c0, [csp]");
  COMPARE_A64(ldar(czr, MemOperand(c1)), "ldar czr, [c1]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Ldar(c30, MemOperand(c29)), "ldar c30, [c29]");
  COMPARE_MACRO_A64(Ldar(czr, MemOperand(csp)), "ldar czr, [csp]");
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

  COMPARE_A64(ldr(w0, MemOperand(c1, 420)), "ldr w0, [c1, #420]");
  COMPARE_A64(ldr(w0, MemOperand(c1, 0)), "ldr w0, [c1]");
  COMPARE_A64(ldr(w0, MemOperand(c1, 2044)), "ldr w0, [c1, #2044]");
  COMPARE_A64(ldr(w0, MemOperand(csp, 420)), "ldr w0, [csp, #420]");
  COMPARE_A64(ldr(wzr, MemOperand(c1, 420)), "ldr wzr, [c1, #420]");

  // `ldur` can assemble to `ldr`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldur(w0, MemOperand(c1, 256)), "ldr w0, [c1, #256]");
  COMPARE_A64(ldur(w0, MemOperand(c1, 2044)), "ldr w0, [c1, #2044]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldr(CPURegister(w0), MemOperand(c1, 0)), "ldr w0, [c1]");
  COMPARE_A64(ldr(CPURegister(wzr), MemOperand(c1, 2044)),
              "ldr wzr, [c1, #2044]");
}

TEST(morello_a64_aldr_r_ri_64) {
  SETUP();

  COMPARE_A64(ldr(x0, MemOperand(c1, 840)), "ldr x0, [c1, #840]");
  COMPARE_A64(ldr(x0, MemOperand(c1, 0)), "ldr x0, [c1]");
  COMPARE_A64(ldr(x0, MemOperand(c1, 4088)), "ldr x0, [c1, #4088]");
  COMPARE_A64(ldr(x0, MemOperand(csp, 840)), "ldr x0, [csp, #840]");
  COMPARE_A64(ldr(xzr, MemOperand(c1, 840)), "ldr xzr, [c1, #840]");

  // `ldur` can assemble to `ldr`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldur(x0, MemOperand(c1, 256)), "ldr x0, [c1, #256]");
  COMPARE_A64(ldur(x0, MemOperand(c1, 4088)), "ldr x0, [c1, #4088]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldr(CPURegister(x0), MemOperand(c1, 0)), "ldr x0, [c1]");
  COMPARE_A64(ldr(CPURegister(xzr), MemOperand(c1, 4088)),
              "ldr xzr, [c1, #4088]");
}

TEST(morello_a64_aldr_r_rrb_32) {
  SETUP();

  COMPARE_A64(ldr(w0, MemOperand(c1, w2, UXTW, 2)),
              "ldr w0, [c1, w2, uxtw #2]");
  COMPARE_A64(ldr(w0, MemOperand(c1, x2, SXTX, 0)), "ldr w0, [c1, x2, sxtx]");
  COMPARE_A64(ldr(w0, MemOperand(c1, x2, LSL, 2)), "ldr w0, [c1, x2, lsl #2]");
  COMPARE_A64(ldr(w0, MemOperand(c1, xzr, LSL, 2)),
              "ldr w0, [c1, xzr, lsl #2]");
  COMPARE_A64(ldr(w0, MemOperand(csp, x2, LSL, 2)),
              "ldr w0, [csp, x2, lsl #2]");
  COMPARE_A64(ldr(wzr, MemOperand(c1, x2, LSL, 2)),
              "ldr wzr, [c1, x2, lsl #2]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldr(CPURegister(w0), MemOperand(c1, w2, UXTW, 2)),
              "ldr w0, [c1, w2, uxtw #2]");
  COMPARE_A64(ldr(CPURegister(wzr), MemOperand(c1, x2, LSL, 2)),
              "ldr wzr, [c1, x2, lsl #2]");
}

TEST(morello_a64_aldr_r_rrb_64) {
  SETUP();

  COMPARE_A64(ldr(x0, MemOperand(c1, w2, UXTW, 3)),
              "ldr x0, [c1, w2, uxtw #3]");
  COMPARE_A64(ldr(x0, MemOperand(c1, x2, SXTX, 0)), "ldr x0, [c1, x2, sxtx]");
  COMPARE_A64(ldr(x0, MemOperand(c1, x2, LSL, 3)), "ldr x0, [c1, x2, lsl #3]");
  COMPARE_A64(ldr(x0, MemOperand(c1, xzr, LSL, 3)),
              "ldr x0, [c1, xzr, lsl #3]");
  COMPARE_A64(ldr(x0, MemOperand(csp, x2, LSL, 3)),
              "ldr x0, [csp, x2, lsl #3]");
  COMPARE_A64(ldr(xzr, MemOperand(c1, x2, LSL, 3)),
              "ldr xzr, [c1, x2, lsl #3]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldr(CPURegister(x0), MemOperand(c1, w2, UXTW, 3)),
              "ldr x0, [c1, w2, uxtw #3]");
  COMPARE_A64(ldr(CPURegister(xzr), MemOperand(c1, x2, LSL, 3)),
              "ldr xzr, [c1, x2, lsl #3]");
}

TEST(morello_a64_aldr_v_rrb_d) {
  SETUP();

  COMPARE_A64(ldr(d0, MemOperand(c1, w2, UXTW, 3)),
              "ldr d0, [c1, w2, uxtw #3]");
  COMPARE_A64(ldr(d0, MemOperand(c1, x2, SXTX, 0)), "ldr d0, [c1, x2, sxtx]");
  COMPARE_A64(ldr(d0, MemOperand(c1, x2, LSL, 3)), "ldr d0, [c1, x2, lsl #3]");
  COMPARE_A64(ldr(d0, MemOperand(c1, xzr, LSL, 3)),
              "ldr d0, [c1, xzr, lsl #3]");
  COMPARE_A64(ldr(d0, MemOperand(csp, x2, LSL, 3)),
              "ldr d0, [csp, x2, lsl #3]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldr(CPURegister(d0), MemOperand(c1, w2, UXTW, 3)),
              "ldr d0, [c1, w2, uxtw #3]");
}

TEST(morello_a64_aldr_v_rrb_s) {
  SETUP();

  COMPARE_A64(ldr(s0, MemOperand(c1, w2, UXTW, 2)),
              "ldr s0, [c1, w2, uxtw #2]");
  COMPARE_A64(ldr(s0, MemOperand(c1, x2, SXTX, 0)), "ldr s0, [c1, x2, sxtx]");
  COMPARE_A64(ldr(s0, MemOperand(c1, x2, LSL, 2)), "ldr s0, [c1, x2, lsl #2]");
  COMPARE_A64(ldr(s0, MemOperand(c1, xzr, LSL, 2)),
              "ldr s0, [c1, xzr, lsl #2]");
  COMPARE_A64(ldr(s0, MemOperand(csp, x2, LSL, 2)),
              "ldr s0, [csp, x2, lsl #2]");
  COMPARE_A64(ldr(wzr, MemOperand(c1, x2, LSL, 2)),
              "ldr wzr, [c1, x2, lsl #2]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldr(CPURegister(w0), MemOperand(c1, w2, UXTW, 2)),
              "ldr w0, [c1, w2, uxtw #2]");
  COMPARE_A64(ldr(CPURegister(wzr), MemOperand(c1, x2, LSL, 2)),
              "ldr wzr, [c1, x2, lsl #2]");
}

TEST(morello_a64_aldr_v_macro_q) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Ldr(q0, MemOperand(c1, 0)), "ldur q0, [c1]");
  COMPARE_MACRO_A64(Ldr(q0, MemOperand(csp, 0)), "ldur q0, [csp]");
  COMPARE_MACRO_A64(Ldr(q31, MemOperand(c1, 0)), "ldur q31, [c1]");
  COMPARE_MACRO_A64(Ldr(q0, MemOperand(c1, 255)), "ldur q0, [c1, #255]");
  COMPARE_MACRO_A64(Ldr(q0, MemOperand(c1, -256)), "ldur q0, [c1, #-256]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldr(q0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "ldur q0, [c16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_A64(Ldr(q0, MemOperand(c1, 16, PreIndex)),
                    "add c1, c1, #0x10 (16)\n"
                    "ldur q0, [c1]");
  COMPARE_MACRO_A64(Ldr(q0, MemOperand(c1, -16, PostIndex)),
                    "ldur q0, [c1]\n"
                    "sub c1, c1, #0x10 (16)");
  // There are no register-offset modes for Q.
  COMPARE_MACRO_A64(Ldr(q0, MemOperand(c1, w2, UXTW, 4)),
                    "add c16, c1, x2, uxtw #4\n"
                    "ldur q0, [c16]");
  COMPARE_MACRO_A64(Ldr(q0, MemOperand(c1, x2, SXTX, 0)),
                    "add c16, c1, x2, sxtx\n"
                    "ldur q0, [c16]");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Ldr(CPURegister(q0), MemOperand(c1)), "ldur q0, [c1]");
}

TEST(morello_a64_aldr_v_macro_d) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Ldr(d0, MemOperand(c1, 0)), "ldur d0, [c1]");
  COMPARE_MACRO_A64(Ldr(d0, MemOperand(csp, 0)), "ldur d0, [csp]");
  COMPARE_MACRO_A64(Ldr(d31, MemOperand(c1, 0)), "ldur d31, [c1]");
  COMPARE_MACRO_A64(Ldr(d0, MemOperand(c1, 255)), "ldur d0, [c1, #255]");
  COMPARE_MACRO_A64(Ldr(d0, MemOperand(c1, -256)), "ldur d0, [c1, #-256]");
  COMPARE_MACRO_A64(Ldr(d0, MemOperand(c1, w2, UXTW, 3)),
                    "ldr d0, [c1, w2, uxtw #3]");
  COMPARE_MACRO_A64(Ldr(d0, MemOperand(c1, x2, SXTX, 0)),
                    "ldr d0, [c1, x2, sxtx]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldr(d0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "ldur d0, [c16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_A64(Ldr(d0, MemOperand(c1, 16, PreIndex)),
                    "add c1, c1, #0x10 (16)\n"
                    "ldur d0, [c1]");
  COMPARE_MACRO_A64(Ldr(d0, MemOperand(c1, -16, PostIndex)),
                    "ldur d0, [c1]\n"
                    "sub c1, c1, #0x10 (16)");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Ldr(CPURegister(d0), MemOperand(c1)), "ldur d0, [c1]");
}

TEST(morello_a64_aldr_v_macro_s) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Ldr(s0, MemOperand(c1, 0)), "ldur s0, [c1]");
  COMPARE_MACRO_A64(Ldr(s0, MemOperand(csp, 0)), "ldur s0, [csp]");
  COMPARE_MACRO_A64(Ldr(s31, MemOperand(c1, 0)), "ldur s31, [c1]");
  COMPARE_MACRO_A64(Ldr(s0, MemOperand(c1, 255)), "ldur s0, [c1, #255]");
  COMPARE_MACRO_A64(Ldr(s0, MemOperand(c1, -256)), "ldur s0, [c1, #-256]");
  COMPARE_MACRO_A64(Ldr(s0, MemOperand(c1, w2, UXTW, 2)),
                    "ldr s0, [c1, w2, uxtw #2]");
  COMPARE_MACRO_A64(Ldr(s0, MemOperand(c1, x2, SXTX, 0)),
                    "ldr s0, [c1, x2, sxtx]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldr(s0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "ldur s0, [c16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_A64(Ldr(s0, MemOperand(c1, 16, PreIndex)),
                    "add c1, c1, #0x10 (16)\n"
                    "ldur s0, [c1]");
  COMPARE_MACRO_A64(Ldr(s0, MemOperand(c1, -16, PostIndex)),
                    "ldur s0, [c1]\n"
                    "sub c1, c1, #0x10 (16)");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Ldr(CPURegister(s0), MemOperand(c1)), "ldur s0, [c1]");
}

TEST(morello_a64_aldr_v_macro_h) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Ldr(h0, MemOperand(c1, 0)), "ldur h0, [c1]");
  COMPARE_MACRO_A64(Ldr(h0, MemOperand(csp, 0)), "ldur h0, [csp]");
  COMPARE_MACRO_A64(Ldr(h31, MemOperand(c1, 0)), "ldur h31, [c1]");
  COMPARE_MACRO_A64(Ldr(h0, MemOperand(c1, 255)), "ldur h0, [c1, #255]");
  COMPARE_MACRO_A64(Ldr(h0, MemOperand(c1, -256)), "ldur h0, [c1, #-256]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldr(h0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "ldur h0, [c16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_A64(Ldr(h0, MemOperand(c1, 16, PreIndex)),
                    "add c1, c1, #0x10 (16)\n"
                    "ldur h0, [c1]");
  COMPARE_MACRO_A64(Ldr(h0, MemOperand(c1, -16, PostIndex)),
                    "ldur h0, [c1]\n"
                    "sub c1, c1, #0x10 (16)");
  // There are no register-offset modes for h.
  COMPARE_MACRO_A64(Ldr(h0, MemOperand(c1, w2, UXTW, 1)),
                    "add c16, c1, x2, uxtw #1\n"
                    "ldur h0, [c16]");
  COMPARE_MACRO_A64(Ldr(h0, MemOperand(c1, x2, SXTX, 0)),
                    "add c16, c1, x2, sxtx\n"
                    "ldur h0, [c16]");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Ldr(CPURegister(h0), MemOperand(c1)), "ldur h0, [c1]");
}

TEST(morello_a64_aldr_v_macro_b) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Ldr(b0, MemOperand(c1, 0)), "ldur b0, [c1]");
  COMPARE_MACRO_A64(Ldr(b0, MemOperand(csp, 0)), "ldur b0, [csp]");
  COMPARE_MACRO_A64(Ldr(b31, MemOperand(c1, 0)), "ldur b31, [c1]");
  COMPARE_MACRO_A64(Ldr(b0, MemOperand(c1, 255)), "ldur b0, [c1, #255]");
  COMPARE_MACRO_A64(Ldr(b0, MemOperand(c1, -256)), "ldur b0, [c1, #-256]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldr(b0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "ldur b0, [c16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_A64(Ldr(b0, MemOperand(c1, 16, PreIndex)),
                    "add c1, c1, #0x10 (16)\n"
                    "ldur b0, [c1]");
  COMPARE_MACRO_A64(Ldr(b0, MemOperand(c1, -16, PostIndex)),
                    "ldur b0, [c1]\n"
                    "sub c1, c1, #0x10 (16)");
  // There are no register-offset modes for b.
  COMPARE_MACRO_A64(Ldr(b0, MemOperand(c1, w2, UXTW)),
                    "add c16, c1, x2, uxtw\n"
                    "ldur b0, [c16]");
  COMPARE_MACRO_A64(Ldr(b0, MemOperand(c1, x2, SXTX)),
                    "add c16, c1, x2, sxtx\n"
                    "ldur b0, [c16]");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Ldr(CPURegister(b0), MemOperand(c1)), "ldur b0, [c1]");
}

TEST(morello_a64_aldrb_r_ri_b) {
  SETUP();

  COMPARE_A64(ldrb(w0, MemOperand(c1, 42)), "ldrb w0, [c1, #42]");
  COMPARE_A64(ldrb(w0, MemOperand(c1, 0)), "ldrb w0, [c1]");
  COMPARE_A64(ldrb(w0, MemOperand(c1, 511)), "ldrb w0, [c1, #511]");
  COMPARE_A64(ldrb(w0, MemOperand(csp, 42)), "ldrb w0, [csp, #42]");
  COMPARE_A64(ldrb(wzr, MemOperand(c1, 42)), "ldrb wzr, [c1, #42]");

  // `ldurb` can assemble to `ldrb`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldurb(w0, MemOperand(c1, 256)), "ldrb w0, [c1, #256]");
  COMPARE_A64(ldurb(w0, MemOperand(c1, 511)), "ldrb w0, [c1, #511]");
}

TEST(morello_a64_aldrb_r_rrb_b) {
  SETUP();

  COMPARE_A64(ldrb(w0, MemOperand(c1, w2, UXTW)), "ldrb w0, [c1, w2, uxtw]");
  COMPARE_A64(ldrb(w0, MemOperand(c1, x2, SXTX, 0)), "ldrb w0, [c1, x2, sxtx]");
  COMPARE_A64(ldrb(w0, MemOperand(c1, x2, LSL, 0)), "ldrb w0, [c1, x2]");
  COMPARE_A64(ldrb(w0, MemOperand(c1, xzr)), "ldrb w0, [c1, xzr]");
  COMPARE_A64(ldrb(w0, MemOperand(csp, x2)), "ldrb w0, [csp, x2]");
  COMPARE_A64(ldrb(wzr, MemOperand(c1, x2)), "ldrb wzr, [c1, x2]");
}

TEST(morello_a64_aldrh_r_rrb_32) {
  SETUP();

  COMPARE_A64(ldrh(w0, MemOperand(c1, x2)), "ldrh w0, [c1, x2]");
  COMPARE_A64(ldrh(w0, MemOperand(c1, w2, SXTW, 0)), "ldrh w0, [c1, w2, sxtw]");
  COMPARE_A64(ldrh(w0, MemOperand(c1, x2, LSL, 1)),
              "ldrh w0, [c1, x2, lsl #1]");
  COMPARE_A64(ldrh(w0, MemOperand(c1, xzr, LSL, 1)),
              "ldrh w0, [c1, xzr, lsl #1]");
  COMPARE_A64(ldrh(w0, MemOperand(csp, x2, LSL, 1)),
              "ldrh w0, [csp, x2, lsl #1]");
  COMPARE_A64(ldrh(wzr, MemOperand(c1, x2, LSL, 1)),
              "ldrh wzr, [c1, x2, lsl #1]");
}

TEST(morello_a64_aldrsb_r_rrb_32) {
  SETUP();

  COMPARE_A64(ldrsb(w0, MemOperand(c1, w2, UXTW)), "ldrsb w0, [c1, w2, uxtw]");
  COMPARE_A64(ldrsb(w0, MemOperand(c1, x2, SXTX, 0)),
              "ldrsb w0, [c1, x2, sxtx]");
  COMPARE_A64(ldrsb(w0, MemOperand(c1, x2, LSL, 0)), "ldrsb w0, [c1, x2]");
  COMPARE_A64(ldrsb(w0, MemOperand(c1, xzr)), "ldrsb w0, [c1, xzr]");
  COMPARE_A64(ldrsb(w0, MemOperand(csp, x2)), "ldrsb w0, [csp, x2]");
  COMPARE_A64(ldrsb(wzr, MemOperand(c1, x2)), "ldrsb wzr, [c1, x2]");
}

TEST(morello_a64_aldrsb_r_rrb_64) {
  SETUP();

  COMPARE_A64(ldrsb(x0, MemOperand(c1, w2, UXTW)), "ldrsb x0, [c1, w2, uxtw]");
  COMPARE_A64(ldrsb(x0, MemOperand(c1, x2, SXTX, 0)),
              "ldrsb x0, [c1, x2, sxtx]");
  COMPARE_A64(ldrsb(x0, MemOperand(c1, x2, LSL, 0)), "ldrsb x0, [c1, x2]");
  COMPARE_A64(ldrsb(x0, MemOperand(c1, xzr)), "ldrsb x0, [c1, xzr]");
  COMPARE_A64(ldrsb(x0, MemOperand(csp, x2)), "ldrsb x0, [csp, x2]");
  COMPARE_A64(ldrsb(wzr, MemOperand(c1, x2)), "ldrsb wzr, [c1, x2]");
}

TEST(morello_a64_aldrsh_r_rrb_32) {
  SETUP();

  COMPARE_A64(ldrsh(w0, MemOperand(c1, x2)), "ldrsh w0, [c1, x2]");
  COMPARE_A64(ldrsh(w0, MemOperand(c1, x2, SXTX, 0)),
              "ldrsh w0, [c1, x2, sxtx]");
  COMPARE_A64(ldrsh(w0, MemOperand(c1, x2, LSL, 1)),
              "ldrsh w0, [c1, x2, lsl #1]");
  COMPARE_A64(ldrsh(w0, MemOperand(c1, xzr, LSL, 1)),
              "ldrsh w0, [c1, xzr, lsl #1]");
  COMPARE_A64(ldrsh(w0, MemOperand(csp, w2, UXTW)),
              "ldrsh w0, [csp, w2, uxtw]");
  COMPARE_A64(ldrsh(wzr, MemOperand(c1, x2, LSL, 1)),
              "ldrsh wzr, [c1, x2, lsl #1]");
}

TEST(morello_a64_aldrsh_r_rrb_64) {
  SETUP();

  COMPARE_A64(ldrsh(x0, MemOperand(c1, w2, UXTW)), "ldrsh x0, [c1, w2, uxtw]");
  COMPARE_A64(ldrsh(x0, MemOperand(c1, x2, SXTX)), "ldrsh x0, [c1, x2, sxtx]");
  COMPARE_A64(ldrsh(x0, MemOperand(c1, x2, LSL, 1)),
              "ldrsh x0, [c1, x2, lsl #1]");
  COMPARE_A64(ldrsh(x0, MemOperand(c1, xzr, SXTX, 1)),
              "ldrsh x0, [c1, xzr, sxtx #1]");
  COMPARE_A64(ldrsh(x0, MemOperand(csp, x2, LSL, 1)),
              "ldrsh x0, [csp, x2, lsl #1]");
  COMPARE_A64(ldrsh(xzr, MemOperand(c1, x2, LSL, 1)),
              "ldrsh xzr, [c1, x2, lsl #1]");
}

TEST(morello_a64_aldrb_macro) {
  SETUP();

  // Encodable cases.
  // "unsigned offset"
  COMPARE_MACRO_A64(Ldrb(w0, MemOperand(c1, 42)), "ldrb w0, [c1, #42]");
  COMPARE_MACRO_A64(Ldrb(wzr, MemOperand(csp, 42)), "ldrb wzr, [csp, #42]");
  COMPARE_MACRO_A64(Ldrb(w0, MemOperand(c1, 511)), "ldrb w0, [c1, #511]");
  // "unscaled"
  COMPARE_MACRO_A64(Ldrb(w0, MemOperand(c1, -256)), "ldurb w0, [c1, #-256]");
  COMPARE_MACRO_A64(Ldrb(w0, MemOperand(c1, -1)), "ldurb w0, [c1, #-1]");
  COMPARE_MACRO_A64(Ldrb(wzr, MemOperand(csp, -1)), "ldurb wzr, [csp, #-1]");
  // "register"
  COMPARE_MACRO_A64(Ldrb(w0, MemOperand(c1, x2)), "ldrb w0, [c1, x2]");
  COMPARE_MACRO_A64(Ldrb(w0, MemOperand(c1, w2, SXTW)),
                    "ldrb w0, [c1, w2, sxtw]");

  // The MacroAssembler permits an X-sized result for zero-extending loads.
  COMPARE_MACRO_A64(Ldrb(x0, MemOperand(c1, 42)), "ldrb w0, [c1, #42]");
  COMPARE_MACRO_A64(Ldrb(x0, MemOperand(c1, -1)), "ldurb w0, [c1, #-1]");
  COMPARE_MACRO_A64(Ldrb(x0, MemOperand(c1, w2, UXTW)),
                    "ldrb w0, [c1, w2, uxtw]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldrb(w0, MemOperand(c1, 512)),
                    "add c16, c1, #0x200 (512)\n"
                    "ldrb w0, [c16]");
  // There are no index modes.
  COMPARE_MACRO_A64(Ldrb(w0, MemOperand(c1, -16, PreIndex)),
                    "sub c1, c1, #0x10 (16)\n"
                    "ldrb w0, [c1]");
  COMPARE_MACRO_A64(Ldrb(w0, MemOperand(c1, 16, PostIndex)),
                    "ldrb w0, [c1]\n"
                    "add c1, c1, #0x10 (16)");
}

TEST(morello_a64_aldrsb_macro_32) {
  SETUP();

  // Encodable cases.
  // "unscaled"
  COMPARE_MACRO_A64(Ldrsb(w0, MemOperand(c1, -256)), "ldursb w0, [c1, #-256]");
  COMPARE_MACRO_A64(Ldrsb(w0, MemOperand(c1, -1)), "ldursb w0, [c1, #-1]");
  COMPARE_MACRO_A64(Ldrsb(wzr, MemOperand(csp, -1)), "ldursb wzr, [csp, #-1]");
  // "register"
  COMPARE_MACRO_A64(Ldrsb(w0, MemOperand(c1, x2)), "ldrsb w0, [c1, x2]");
  COMPARE_MACRO_A64(Ldrsb(w0, MemOperand(c1, w2, SXTW)),
                    "ldrsb w0, [c1, w2, sxtw]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldrsb(w0, MemOperand(c1, 512)),
                    "add c16, c1, #0x200 (512)\n"
                    "ldursb w0, [c16]");
  // There are no index modes.
  COMPARE_MACRO_A64(Ldrsb(w0, MemOperand(c1, -16, PreIndex)),
                    "sub c1, c1, #0x10 (16)\n"
                    "ldursb w0, [c1]");
  COMPARE_MACRO_A64(Ldrsb(w0, MemOperand(c1, 16, PostIndex)),
                    "ldursb w0, [c1]\n"
                    "add c1, c1, #0x10 (16)");
  // There is no "unsigned offset" mode.
  COMPARE_MACRO_A64(Ldrsb(w0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "ldursb w0, [c16]");
}

TEST(morello_a64_aldrsb_macro_64) {
  SETUP();

  // Encodable cases.
  // "unscaled"
  COMPARE_MACRO_A64(Ldrsb(x0, MemOperand(c1, -256)), "ldursb x0, [c1, #-256]");
  COMPARE_MACRO_A64(Ldrsb(x0, MemOperand(c1, -1)), "ldursb x0, [c1, #-1]");
  COMPARE_MACRO_A64(Ldrsb(xzr, MemOperand(csp, -1)), "ldursb xzr, [csp, #-1]");
  // "register"
  COMPARE_MACRO_A64(Ldrsb(x0, MemOperand(c1, x2)), "ldrsb x0, [c1, x2]");
  COMPARE_MACRO_A64(Ldrsb(x0, MemOperand(c1, w2, SXTW)),
                    "ldrsb x0, [c1, w2, sxtw]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldrsb(x0, MemOperand(c1, 512)),
                    "add c16, c1, #0x200 (512)\n"
                    "ldursb x0, [c16]");
  // There are no index modes.
  COMPARE_MACRO_A64(Ldrsb(x0, MemOperand(c1, -16, PreIndex)),
                    "sub c1, c1, #0x10 (16)\n"
                    "ldursb x0, [c1]");
  COMPARE_MACRO_A64(Ldrsb(x0, MemOperand(c1, 16, PostIndex)),
                    "ldursb x0, [c1]\n"
                    "add c1, c1, #0x10 (16)");
  // There is no "unsigned offset" mode.
  COMPARE_MACRO_A64(Ldrsb(x0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "ldursb x0, [c16]");
}

TEST(morello_a64_aldrh_macro) {
  SETUP();

  // Encodable cases.
  // "unscaled"
  COMPARE_MACRO_A64(Ldrh(w0, MemOperand(c1, -256)), "ldurh w0, [c1, #-256]");
  COMPARE_MACRO_A64(Ldrh(w0, MemOperand(c1, 255)), "ldurh w0, [c1, #255]");
  COMPARE_MACRO_A64(Ldrh(wzr, MemOperand(csp, 42)), "ldurh wzr, [csp, #42]");
  // "register"
  COMPARE_MACRO_A64(Ldrh(w0, MemOperand(c1, x2)), "ldrh w0, [c1, x2]");
  COMPARE_MACRO_A64(Ldrh(w0, MemOperand(c1, w2, SXTW)),
                    "ldrh w0, [c1, w2, sxtw]");
  COMPARE_MACRO_A64(Ldrh(w0, MemOperand(c1, x2, LSL, 1)),
                    "ldrh w0, [c1, x2, lsl #1]");

  // The MacroAssembler permits an X-sized result for zero-extending loads.
  COMPARE_MACRO_A64(Ldrh(x0, MemOperand(c1, 42)), "ldurh w0, [c1, #42]");
  COMPARE_MACRO_A64(Ldrh(x0, MemOperand(c1, x2, SXTX, 1)),
                    "ldrh w0, [c1, x2, sxtx #1]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldrh(w0, MemOperand(c1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add c16, c1, x16, uxtx\n"
                    "ldurh w0, [c16]");
  // There are no index modes.
  COMPARE_MACRO_A64(Ldrh(w0, MemOperand(c1, -16, PreIndex)),
                    "sub c1, c1, #0x10 (16)\n"
                    "ldurh w0, [c1]");
  COMPARE_MACRO_A64(Ldrh(w0, MemOperand(c1, 16, PostIndex)),
                    "ldurh w0, [c1]\n"
                    "add c1, c1, #0x10 (16)");
  // There is no "unsigned offset" mode.
  COMPARE_MACRO_A64(Ldrh(w0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "ldurh w0, [c16]");
}

TEST(morello_a64_aldrsh_macro_32) {
  SETUP();

  // Encodable cases.
  // "unscaled"
  COMPARE_MACRO_A64(Ldrsh(w0, MemOperand(c1, -256)), "ldursh w0, [c1, #-256]");
  COMPARE_MACRO_A64(Ldrsh(w0, MemOperand(c1, 255)), "ldursh w0, [c1, #255]");
  COMPARE_MACRO_A64(Ldrsh(wzr, MemOperand(csp, 42)), "ldursh wzr, [csp, #42]");
  // "register"
  COMPARE_MACRO_A64(Ldrsh(w0, MemOperand(c1, x2)), "ldrsh w0, [c1, x2]");
  COMPARE_MACRO_A64(Ldrsh(w0, MemOperand(c1, w2, SXTW)),
                    "ldrsh w0, [c1, w2, sxtw]");
  COMPARE_MACRO_A64(Ldrsh(w0, MemOperand(c1, x2, LSL, 1)),
                    "ldrsh w0, [c1, x2, lsl #1]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldrsh(w0, MemOperand(c1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add c16, c1, x16, uxtx\n"
                    "ldursh w0, [c16]");
  // There are no index modes.
  COMPARE_MACRO_A64(Ldrsh(w0, MemOperand(c1, -16, PreIndex)),
                    "sub c1, c1, #0x10 (16)\n"
                    "ldursh w0, [c1]");
  COMPARE_MACRO_A64(Ldrsh(w0, MemOperand(c1, 16, PostIndex)),
                    "ldursh w0, [c1]\n"
                    "add c1, c1, #0x10 (16)");
  // There is no "unsigned offset" mode.
  COMPARE_MACRO_A64(Ldrsh(w0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "ldursh w0, [c16]");
}

TEST(morello_a64_aldrsh_macro_64) {
  SETUP();

  // Encodable cases.
  // "unscaled"
  COMPARE_MACRO_A64(Ldrsh(x0, MemOperand(c1, -256)), "ldursh x0, [c1, #-256]");
  COMPARE_MACRO_A64(Ldrsh(x0, MemOperand(c1, 255)), "ldursh x0, [c1, #255]");
  COMPARE_MACRO_A64(Ldrsh(xzr, MemOperand(csp, 42)), "ldursh xzr, [csp, #42]");
  // "register"
  COMPARE_MACRO_A64(Ldrsh(x0, MemOperand(c1, x2)), "ldrsh x0, [c1, x2]");
  COMPARE_MACRO_A64(Ldrsh(x0, MemOperand(c1, w2, SXTW)),
                    "ldrsh x0, [c1, w2, sxtw]");
  COMPARE_MACRO_A64(Ldrsh(x0, MemOperand(c1, x2, LSL, 1)),
                    "ldrsh x0, [c1, x2, lsl #1]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldrsh(x0, MemOperand(c1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add c16, c1, x16, uxtx\n"
                    "ldursh x0, [c16]");
  // There are no index modes.
  COMPARE_MACRO_A64(Ldrsh(x0, MemOperand(c1, -16, PreIndex)),
                    "sub c1, c1, #0x10 (16)\n"
                    "ldursh x0, [c1]");
  COMPARE_MACRO_A64(Ldrsh(x0, MemOperand(c1, 16, PostIndex)),
                    "ldursh x0, [c1]\n"
                    "add c1, c1, #0x10 (16)");
  // There is no "unsigned offset" mode.
  COMPARE_MACRO_A64(Ldrsh(x0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "ldursh x0, [c16]");
}

TEST(morello_a64_aldrsw_macro_64) {
  SETUP();

  // Encodable cases.
  // "unscaled"
  COMPARE_MACRO_A64(Ldrsw(x0, MemOperand(c1, -256)), "ldursw x0, [c1, #-256]");
  COMPARE_MACRO_A64(Ldrsw(x0, MemOperand(c1, 255)), "ldursw x0, [c1, #255]");
  COMPARE_MACRO_A64(Ldrsw(xzr, MemOperand(csp, 44)), "ldursw xzr, [csp, #44]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldrsw(x0, MemOperand(c1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add c16, c1, x16, uxtx\n"
                    "ldursw x0, [c16]");
  // There are no index modes.
  COMPARE_MACRO_A64(Ldrsw(x0, MemOperand(c1, -16, PreIndex)),
                    "sub c1, c1, #0x10 (16)\n"
                    "ldursw x0, [c1]");
  COMPARE_MACRO_A64(Ldrsw(x0, MemOperand(c1, 16, PostIndex)),
                    "ldursw x0, [c1]\n"
                    "add c1, c1, #0x10 (16)");
  // There is no "unsigned offset" mode.
  COMPARE_MACRO_A64(Ldrsw(x0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "ldursw x0, [c16]");
  // There is no "register" mode.
  COMPARE_MACRO_A64(Ldrsw(x0, MemOperand(c1, x2)),
                    "add c16, c1, x2, uxtx\n"
                    "ldursw x0, [c16]");
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

  COMPARE_A64(ldur(w0, MemOperand(c1, 255)), "ldur w0, [c1, #255]");
  COMPARE_A64(ldur(w0, MemOperand(c1, -256)), "ldur w0, [c1, #-256]");
  COMPARE_A64(ldur(w0, MemOperand(c1, 0)), "ldur w0, [c1]");
  COMPARE_A64(ldur(w0, MemOperand(csp, 42)), "ldur w0, [csp, #42]");
  COMPARE_A64(ldur(wzr, MemOperand(c1, 42)), "ldur wzr, [c1, #42]");

  // `ldr` can assemble to `ldur`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldr(w0, MemOperand(c1, -1)), "ldur w0, [c1, #-1]");
  COMPARE_A64(ldr(w0, MemOperand(c1, 42)), "ldur w0, [c1, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldur(CPURegister(w0), MemOperand(c1, 0)), "ldur w0, [c1]");
  COMPARE_A64(ldur(CPURegister(wzr), MemOperand(c1, 42)),
              "ldur wzr, [c1, #42]");
}

TEST(morello_a64_aldur_r_ri_64) {
  SETUP();

  COMPARE_A64(ldur(x0, MemOperand(c1, 255)), "ldur x0, [c1, #255]");
  COMPARE_A64(ldur(x0, MemOperand(c1, -256)), "ldur x0, [c1, #-256]");
  COMPARE_A64(ldur(x0, MemOperand(c1, 0)), "ldur x0, [c1]");
  COMPARE_A64(ldur(x0, MemOperand(csp, 42)), "ldur x0, [csp, #42]");
  COMPARE_A64(ldur(xzr, MemOperand(c1, 42)), "ldur xzr, [c1, #42]");

  // `ldr` can assemble to `ldur`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldr(x0, MemOperand(c1, -1)), "ldur x0, [c1, #-1]");
  COMPARE_A64(ldr(x0, MemOperand(c1, 42)), "ldur x0, [c1, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldur(CPURegister(x0), MemOperand(c1, 0)), "ldur x0, [c1]");
  COMPARE_A64(ldur(CPURegister(xzr), MemOperand(c1, 42)),
              "ldur xzr, [c1, #42]");
}

TEST(morello_a64_aldur_v_ri_b) {
  SETUP();

  COMPARE_A64(ldur(b0, MemOperand(c1, 255)), "ldur b0, [c1, #255]");
  COMPARE_A64(ldur(b0, MemOperand(c1, -256)), "ldur b0, [c1, #-256]");
  COMPARE_A64(ldur(b0, MemOperand(c1, 0)), "ldur b0, [c1]");
  COMPARE_A64(ldur(b0, MemOperand(csp, 42)), "ldur b0, [csp, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldur(CPURegister(b0), MemOperand(c1, 0)), "ldur b0, [c1]");
}

TEST(morello_a64_aldur_v_ri_d) {
  SETUP();

  COMPARE_A64(ldur(d0, MemOperand(c1, 255)), "ldur d0, [c1, #255]");
  COMPARE_A64(ldur(d0, MemOperand(c1, -256)), "ldur d0, [c1, #-256]");
  COMPARE_A64(ldur(d0, MemOperand(c1, 0)), "ldur d0, [c1]");
  COMPARE_A64(ldur(d0, MemOperand(csp, 42)), "ldur d0, [csp, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldur(CPURegister(d0), MemOperand(c1, 0)), "ldur d0, [c1]");
}

TEST(morello_a64_aldur_v_ri_h) {
  SETUP();

  COMPARE_A64(ldur(h0, MemOperand(c1, 255)), "ldur h0, [c1, #255]");
  COMPARE_A64(ldur(h0, MemOperand(c1, -256)), "ldur h0, [c1, #-256]");
  COMPARE_A64(ldur(h0, MemOperand(c1, 0)), "ldur h0, [c1]");
  COMPARE_A64(ldur(h0, MemOperand(csp, 42)), "ldur h0, [csp, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldur(CPURegister(h0), MemOperand(c1, 0)), "ldur h0, [c1]");
}

TEST(morello_a64_aldur_v_ri_q) {
  SETUP();

  COMPARE_A64(ldur(q0, MemOperand(c1, 255)), "ldur q0, [c1, #255]");
  COMPARE_A64(ldur(q0, MemOperand(c1, -256)), "ldur q0, [c1, #-256]");
  COMPARE_A64(ldur(q0, MemOperand(c1, 0)), "ldur q0, [c1]");
  COMPARE_A64(ldur(q0, MemOperand(csp, 42)), "ldur q0, [csp, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldur(CPURegister(q0), MemOperand(c1, 0)), "ldur q0, [c1]");
}

TEST(morello_a64_aldur_v_ri_s) {
  SETUP();

  COMPARE_A64(ldur(s0, MemOperand(c1, 255)), "ldur s0, [c1, #255]");
  COMPARE_A64(ldur(s0, MemOperand(c1, -256)), "ldur s0, [c1, #-256]");
  COMPARE_A64(ldur(s0, MemOperand(c1, 0)), "ldur s0, [c1]");
  COMPARE_A64(ldur(s0, MemOperand(csp, 42)), "ldur s0, [csp, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(ldur(CPURegister(s0), MemOperand(c1, 0)), "ldur s0, [c1]");
}

TEST(morello_a64_aldurb_r_ri_32) {
  SETUP();

  COMPARE_A64(ldurb(w0, MemOperand(c1, 42)), "ldurb w0, [c1, #42]");
  COMPARE_A64(ldurb(w0, MemOperand(c1, 0)), "ldurb w0, [c1]");
  COMPARE_A64(ldurb(w0, MemOperand(c1, -256)), "ldurb w0, [c1, #-256]");
  COMPARE_A64(ldurb(w0, MemOperand(c1, 255)), "ldurb w0, [c1, #255]");
  COMPARE_A64(ldurb(w0, MemOperand(csp, 42)), "ldurb w0, [csp, #42]");
  COMPARE_A64(ldurb(wzr, MemOperand(c1, 42)), "ldurb wzr, [c1, #42]");

  // `ldrb` can assemble to `ldurb`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldurb(w0, MemOperand(c1, -1)), "ldurb w0, [c1, #-1]");
}

TEST(morello_a64_aldurh_r_ri_32) {
  SETUP();

  COMPARE_A64(ldurh(w0, MemOperand(c1, 42)), "ldurh w0, [c1, #42]");
  COMPARE_A64(ldurh(w0, MemOperand(c1, 0)), "ldurh w0, [c1]");
  COMPARE_A64(ldurh(w0, MemOperand(c1, -256)), "ldurh w0, [c1, #-256]");
  COMPARE_A64(ldurh(w0, MemOperand(c1, 255)), "ldurh w0, [c1, #255]");
  COMPARE_A64(ldurh(w0, MemOperand(csp, 42)), "ldurh w0, [csp, #42]");
  COMPARE_A64(ldurh(wzr, MemOperand(c1, 42)), "ldurh wzr, [c1, #42]");

  // `ldrh` can assemble to `ldurh`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldurh(w0, MemOperand(c1, -1)), "ldurh w0, [c1, #-1]");
}

TEST(morello_a64_aldursb_r_ri_32) {
  SETUP();

  COMPARE_A64(ldursb(w0, MemOperand(c1, 42)), "ldursb w0, [c1, #42]");
  COMPARE_A64(ldursb(w0, MemOperand(c1, 0)), "ldursb w0, [c1]");
  COMPARE_A64(ldursb(w0, MemOperand(c1, -256)), "ldursb w0, [c1, #-256]");
  COMPARE_A64(ldursb(w0, MemOperand(c1, 255)), "ldursb w0, [c1, #255]");
  COMPARE_A64(ldursb(w0, MemOperand(csp, 42)), "ldursb w0, [csp, #42]");
  COMPARE_A64(ldursb(wzr, MemOperand(c1, 42)), "ldursb wzr, [c1, #42]");

  // `ldrsb` can assemble to `ldursb`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldursb(w0, MemOperand(c1, -1)), "ldursb w0, [c1, #-1]");
}

TEST(morello_a64_aldursb_r_ri_64) {
  SETUP();

  COMPARE_A64(ldursb(x0, MemOperand(c1, 42)), "ldursb x0, [c1, #42]");
  COMPARE_A64(ldursb(x0, MemOperand(c1, 0)), "ldursb x0, [c1]");
  COMPARE_A64(ldursb(x0, MemOperand(c1, -256)), "ldursb x0, [c1, #-256]");
  COMPARE_A64(ldursb(x0, MemOperand(c1, 255)), "ldursb x0, [c1, #255]");
  COMPARE_A64(ldursb(x0, MemOperand(csp, 42)), "ldursb x0, [csp, #42]");
  COMPARE_A64(ldursb(xzr, MemOperand(c1, 42)), "ldursb xzr, [c1, #42]");

  // `ldrsb` can assemble to `ldursb`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldursb(x0, MemOperand(c1, -1)), "ldursb x0, [c1, #-1]");
}

TEST(morello_a64_aldursh_r_ri_32) {
  SETUP();

  COMPARE_A64(ldursh(w0, MemOperand(c1, 42)), "ldursh w0, [c1, #42]");
  COMPARE_A64(ldursh(w0, MemOperand(c1, 0)), "ldursh w0, [c1]");
  COMPARE_A64(ldursh(w0, MemOperand(c1, -256)), "ldursh w0, [c1, #-256]");
  COMPARE_A64(ldursh(w0, MemOperand(c1, 255)), "ldursh w0, [c1, #255]");
  COMPARE_A64(ldursh(w0, MemOperand(csp, 42)), "ldursh w0, [csp, #42]");
  COMPARE_A64(ldursh(wzr, MemOperand(c1, 42)), "ldursh wzr, [c1, #42]");

  // `ldrsh` can assemble to `ldursh`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldursh(w0, MemOperand(c1, -1)), "ldursh w0, [c1, #-1]");
}

TEST(morello_a64_aldursh_r_ri_64) {
  SETUP();

  COMPARE_A64(ldursh(x0, MemOperand(c1, 42)), "ldursh x0, [c1, #42]");
  COMPARE_A64(ldursh(x0, MemOperand(c1, 0)), "ldursh x0, [c1]");
  COMPARE_A64(ldursh(x0, MemOperand(c1, -256)), "ldursh x0, [c1, #-256]");
  COMPARE_A64(ldursh(x0, MemOperand(c1, 255)), "ldursh x0, [c1, #255]");
  COMPARE_A64(ldursh(x0, MemOperand(csp, 42)), "ldursh x0, [csp, #42]");
  COMPARE_A64(ldursh(xzr, MemOperand(c1, 42)), "ldursh xzr, [c1, #42]");

  // `ldrsh` can assemble to `ldursh`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldursh(x0, MemOperand(c1, -1)), "ldursh x0, [c1, #-1]");
}

TEST(morello_a64_aldursw_r_ri_64) {
  SETUP();

  COMPARE_A64(ldursw(x0, MemOperand(c1, 42)), "ldursw x0, [c1, #42]");
  COMPARE_A64(ldursw(x0, MemOperand(c1, 0)), "ldursw x0, [c1]");
  COMPARE_A64(ldursw(x0, MemOperand(c1, -256)), "ldursw x0, [c1, #-256]");
  COMPARE_A64(ldursw(x0, MemOperand(c1, 255)), "ldursw x0, [c1, #255]");
  COMPARE_A64(ldursw(x0, MemOperand(csp, 42)), "ldursw x0, [csp, #42]");
  COMPARE_A64(ldursw(xzr, MemOperand(c1, 42)), "ldursw xzr, [c1, #42]");

  // `ldrsw` can assemble to `ldursw`, according to its LoadStoreScalingOption.
  COMPARE_A64(ldursw(x0, MemOperand(c1, -1)), "ldursw x0, [c1, #-1]");
}

TEST(morello_a64_astlr_c_r_c) {
  SETUP();

  COMPARE_A64(stlr(c0, MemOperand(c1)), "stlr c0, [c1]");
  COMPARE_A64(stlr(c0, MemOperand(csp)), "stlr c0, [csp]");
  COMPARE_A64(stlr(czr, MemOperand(c1)), "stlr czr, [c1]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Stlr(c30, MemOperand(c29)), "stlr c30, [c29]");
  COMPARE_MACRO_A64(Stlr(czr, MemOperand(csp)), "stlr czr, [csp]");
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

  COMPARE_A64(str(w0, MemOperand(c1, 420)), "str w0, [c1, #420]");
  COMPARE_A64(str(w0, MemOperand(c1, 0)), "str w0, [c1]");
  COMPARE_A64(str(w0, MemOperand(c1, 2044)), "str w0, [c1, #2044]");
  COMPARE_A64(str(w0, MemOperand(csp, 420)), "str w0, [csp, #420]");
  COMPARE_A64(str(wzr, MemOperand(c1, 420)), "str wzr, [c1, #420]");

  // `stur` can assemble to `str`, according to its LoadStoreScalingOption.
  COMPARE_A64(stur(w0, MemOperand(c1, 256)), "str w0, [c1, #256]");
  COMPARE_A64(stur(w0, MemOperand(c1, 2044)), "str w0, [c1, #2044]");

  // A generic CPURegister works the same.
  COMPARE_A64(str(CPURegister(w0), MemOperand(c1, 0)), "str w0, [c1]");
  COMPARE_A64(str(CPURegister(wzr), MemOperand(c1, 2044)),
              "str wzr, [c1, #2044]");
}

TEST(morello_a64_astr_r_ri_64) {
  SETUP();

  COMPARE_A64(str(x0, MemOperand(c1, 840)), "str x0, [c1, #840]");
  COMPARE_A64(str(x0, MemOperand(c1, 0)), "str x0, [c1]");
  COMPARE_A64(str(x0, MemOperand(c1, 4088)), "str x0, [c1, #4088]");
  COMPARE_A64(str(x0, MemOperand(csp, 840)), "str x0, [csp, #840]");
  COMPARE_A64(str(xzr, MemOperand(c1, 840)), "str xzr, [c1, #840]");

  // `stur` can assemble to `str`, according to its LoadStoreScalingOption.
  COMPARE_A64(stur(x0, MemOperand(c1, 256)), "str x0, [c1, #256]");
  COMPARE_A64(stur(x0, MemOperand(c1, 4088)), "str x0, [c1, #4088]");

  // A generic CPURegister works the same.
  COMPARE_A64(str(CPURegister(x0), MemOperand(c1, 0)), "str x0, [c1]");
  COMPARE_A64(str(CPURegister(xzr), MemOperand(c1, 4088)),
              "str xzr, [c1, #4088]");
}

TEST(morello_a64_astr_r_rrb_32) {
  SETUP();

  COMPARE_A64(str(w0, MemOperand(c1, w2, UXTW, 2)),
              "str w0, [c1, w2, uxtw #2]");
  COMPARE_A64(str(w0, MemOperand(c1, x2, SXTX, 0)), "str w0, [c1, x2, sxtx]");
  COMPARE_A64(str(w0, MemOperand(c1, x2, LSL, 2)), "str w0, [c1, x2, lsl #2]");
  COMPARE_A64(str(w0, MemOperand(c1, xzr, LSL, 2)),
              "str w0, [c1, xzr, lsl #2]");
  COMPARE_A64(str(w0, MemOperand(csp, x2, LSL, 2)),
              "str w0, [csp, x2, lsl #2]");
  COMPARE_A64(str(wzr, MemOperand(c1, x2, LSL, 2)),
              "str wzr, [c1, x2, lsl #2]");

  // A generic CPURegister works the same.
  COMPARE_A64(str(CPURegister(w0), MemOperand(c1, w2, UXTW, 2)),
              "str w0, [c1, w2, uxtw #2]");
  COMPARE_A64(str(CPURegister(wzr), MemOperand(c1, x2, LSL, 2)),
              "str wzr, [c1, x2, lsl #2]");
}

TEST(morello_a64_astr_r_rrb_64) {
  SETUP();

  COMPARE_A64(str(x0, MemOperand(c1, w2, UXTW, 3)),
              "str x0, [c1, w2, uxtw #3]");
  COMPARE_A64(str(x0, MemOperand(c1, x2, SXTX, 0)), "str x0, [c1, x2, sxtx]");
  COMPARE_A64(str(x0, MemOperand(c1, x2, LSL, 3)), "str x0, [c1, x2, lsl #3]");
  COMPARE_A64(str(x0, MemOperand(c1, xzr, LSL, 3)),
              "str x0, [c1, xzr, lsl #3]");
  COMPARE_A64(str(x0, MemOperand(csp, x2, LSL, 3)),
              "str x0, [csp, x2, lsl #3]");
  COMPARE_A64(str(xzr, MemOperand(c1, x2, LSL, 3)),
              "str xzr, [c1, x2, lsl #3]");

  // A generic CPURegister works the same.
  COMPARE_A64(str(CPURegister(x0), MemOperand(c1, w2, UXTW, 3)),
              "str x0, [c1, w2, uxtw #3]");
  COMPARE_A64(str(CPURegister(xzr), MemOperand(c1, x2, LSL, 3)),
              "str xzr, [c1, x2, lsl #3]");
}

TEST(morello_a64_astr_v_rrb_d) {
  SETUP();

  COMPARE_A64(str(d0, MemOperand(c1, w2, UXTW, 3)),
              "str d0, [c1, w2, uxtw #3]");
  COMPARE_A64(str(d0, MemOperand(c1, x2, SXTX, 0)), "str d0, [c1, x2, sxtx]");
  COMPARE_A64(str(d0, MemOperand(c1, x2, LSL, 3)), "str d0, [c1, x2, lsl #3]");
  COMPARE_A64(str(d0, MemOperand(c1, xzr, LSL, 3)),
              "str d0, [c1, xzr, lsl #3]");
  COMPARE_A64(str(d0, MemOperand(csp, x2, LSL, 3)),
              "str d0, [csp, x2, lsl #3]");

  // A generic CPURegister works the same.
  COMPARE_A64(str(CPURegister(d0), MemOperand(c1, w2, UXTW, 3)),
              "str d0, [c1, w2, uxtw #3]");
}

TEST(morello_a64_astr_v_rrb_s) {
  SETUP();

  COMPARE_A64(str(s0, MemOperand(c1, w2, UXTW, 2)),
              "str s0, [c1, w2, uxtw #2]");
  COMPARE_A64(str(s0, MemOperand(c1, x2, SXTX, 0)), "str s0, [c1, x2, sxtx]");
  COMPARE_A64(str(s0, MemOperand(c1, x2, LSL, 2)), "str s0, [c1, x2, lsl #2]");
  COMPARE_A64(str(s0, MemOperand(c1, xzr, LSL, 2)),
              "str s0, [c1, xzr, lsl #2]");
  COMPARE_A64(str(s0, MemOperand(csp, x2, LSL, 2)),
              "str s0, [csp, x2, lsl #2]");
  COMPARE_A64(str(wzr, MemOperand(c1, x2, LSL, 2)),
              "str wzr, [c1, x2, lsl #2]");

  // A generic CPURegister works the same.
  COMPARE_A64(str(CPURegister(w0), MemOperand(c1, w2, UXTW, 2)),
              "str w0, [c1, w2, uxtw #2]");
  COMPARE_A64(str(CPURegister(wzr), MemOperand(c1, x2, LSL, 2)),
              "str wzr, [c1, x2, lsl #2]");
}

TEST(morello_a64_astr_v_macro_q) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Str(q0, MemOperand(c1, 0)), "stur q0, [c1]");
  COMPARE_MACRO_A64(Str(q0, MemOperand(csp, 0)), "stur q0, [csp]");
  COMPARE_MACRO_A64(Str(q31, MemOperand(c1, 0)), "stur q31, [c1]");
  COMPARE_MACRO_A64(Str(q0, MemOperand(c1, 255)), "stur q0, [c1, #255]");
  COMPARE_MACRO_A64(Str(q0, MemOperand(c1, -256)), "stur q0, [c1, #-256]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Str(q0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "stur q0, [c16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_A64(Str(q0, MemOperand(c1, 16, PreIndex)),
                    "add c1, c1, #0x10 (16)\n"
                    "stur q0, [c1]");
  COMPARE_MACRO_A64(Str(q0, MemOperand(c1, -16, PostIndex)),
                    "stur q0, [c1]\n"
                    "sub c1, c1, #0x10 (16)");
  // There are no register-offset modes for Q.
  COMPARE_MACRO_A64(Str(q0, MemOperand(c1, w2, UXTW, 4)),
                    "add c16, c1, x2, uxtw #4\n"
                    "stur q0, [c16]");
  COMPARE_MACRO_A64(Str(q0, MemOperand(c1, x2, SXTX, 0)),
                    "add c16, c1, x2, sxtx\n"
                    "stur q0, [c16]");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Str(CPURegister(q0), MemOperand(c1)), "stur q0, [c1]");
}

TEST(morello_a64_astr_v_macro_d) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Str(d0, MemOperand(c1, 0)), "stur d0, [c1]");
  COMPARE_MACRO_A64(Str(d0, MemOperand(csp, 0)), "stur d0, [csp]");
  COMPARE_MACRO_A64(Str(d31, MemOperand(c1, 0)), "stur d31, [c1]");
  COMPARE_MACRO_A64(Str(d0, MemOperand(c1, 255)), "stur d0, [c1, #255]");
  COMPARE_MACRO_A64(Str(d0, MemOperand(c1, -256)), "stur d0, [c1, #-256]");
  COMPARE_MACRO_A64(Str(d0, MemOperand(c1, w2, UXTW, 3)),
                    "str d0, [c1, w2, uxtw #3]");
  COMPARE_MACRO_A64(Str(d0, MemOperand(c1, x2, SXTX, 0)),
                    "str d0, [c1, x2, sxtx]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Str(d0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "stur d0, [c16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_A64(Str(d0, MemOperand(c1, 16, PreIndex)),
                    "add c1, c1, #0x10 (16)\n"
                    "stur d0, [c1]");
  COMPARE_MACRO_A64(Str(d0, MemOperand(c1, -16, PostIndex)),
                    "stur d0, [c1]\n"
                    "sub c1, c1, #0x10 (16)");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Str(CPURegister(d0), MemOperand(c1)), "stur d0, [c1]");
}

TEST(morello_a64_astr_v_macro_s) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Str(s0, MemOperand(c1, 0)), "stur s0, [c1]");
  COMPARE_MACRO_A64(Str(s0, MemOperand(csp, 0)), "stur s0, [csp]");
  COMPARE_MACRO_A64(Str(s31, MemOperand(c1, 0)), "stur s31, [c1]");
  COMPARE_MACRO_A64(Str(s0, MemOperand(c1, 255)), "stur s0, [c1, #255]");
  COMPARE_MACRO_A64(Str(s0, MemOperand(c1, -256)), "stur s0, [c1, #-256]");
  COMPARE_MACRO_A64(Str(s0, MemOperand(c1, w2, UXTW, 2)),
                    "str s0, [c1, w2, uxtw #2]");
  COMPARE_MACRO_A64(Str(s0, MemOperand(c1, x2, SXTX, 0)),
                    "str s0, [c1, x2, sxtx]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Str(s0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "stur s0, [c16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_A64(Str(s0, MemOperand(c1, 16, PreIndex)),
                    "add c1, c1, #0x10 (16)\n"
                    "stur s0, [c1]");
  COMPARE_MACRO_A64(Str(s0, MemOperand(c1, -16, PostIndex)),
                    "stur s0, [c1]\n"
                    "sub c1, c1, #0x10 (16)");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Str(CPURegister(s0), MemOperand(c1)), "stur s0, [c1]");
}

TEST(morello_a64_astr_v_macro_h) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Str(h0, MemOperand(c1, 0)), "stur h0, [c1]");
  COMPARE_MACRO_A64(Str(h0, MemOperand(csp, 0)), "stur h0, [csp]");
  COMPARE_MACRO_A64(Str(h31, MemOperand(c1, 0)), "stur h31, [c1]");
  COMPARE_MACRO_A64(Str(h0, MemOperand(c1, 255)), "stur h0, [c1, #255]");
  COMPARE_MACRO_A64(Str(h0, MemOperand(c1, -256)), "stur h0, [c1, #-256]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Str(h0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "stur h0, [c16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_A64(Str(h0, MemOperand(c1, 16, PreIndex)),
                    "add c1, c1, #0x10 (16)\n"
                    "stur h0, [c1]");
  COMPARE_MACRO_A64(Str(h0, MemOperand(c1, -16, PostIndex)),
                    "stur h0, [c1]\n"
                    "sub c1, c1, #0x10 (16)");
  // There are no register-offset modes for h.
  COMPARE_MACRO_A64(Str(h0, MemOperand(c1, w2, UXTW, 1)),
                    "add c16, c1, x2, uxtw #1\n"
                    "stur h0, [c16]");
  COMPARE_MACRO_A64(Str(h0, MemOperand(c1, x2, SXTX, 0)),
                    "add c16, c1, x2, sxtx\n"
                    "stur h0, [c16]");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Str(CPURegister(h0), MemOperand(c1)), "stur h0, [c1]");
}

TEST(morello_a64_astr_v_macro_b) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Str(b0, MemOperand(c1, 0)), "stur b0, [c1]");
  COMPARE_MACRO_A64(Str(b0, MemOperand(csp, 0)), "stur b0, [csp]");
  COMPARE_MACRO_A64(Str(b31, MemOperand(c1, 0)), "stur b31, [c1]");
  COMPARE_MACRO_A64(Str(b0, MemOperand(c1, 255)), "stur b0, [c1, #255]");
  COMPARE_MACRO_A64(Str(b0, MemOperand(c1, -256)), "stur b0, [c1, #-256]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Str(b0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "stur b0, [c16]");
  // There are no index modes so these are always unencodable.
  COMPARE_MACRO_A64(Str(b0, MemOperand(c1, 16, PreIndex)),
                    "add c1, c1, #0x10 (16)\n"
                    "stur b0, [c1]");
  COMPARE_MACRO_A64(Str(b0, MemOperand(c1, -16, PostIndex)),
                    "stur b0, [c1]\n"
                    "sub c1, c1, #0x10 (16)");
  // There are no register-offset modes for b.
  COMPARE_MACRO_A64(Str(b0, MemOperand(c1, w2, UXTW)),
                    "add c16, c1, x2, uxtw\n"
                    "stur b0, [c16]");
  COMPARE_MACRO_A64(Str(b0, MemOperand(c1, x2, SXTX)),
                    "add c16, c1, x2, sxtx\n"
                    "stur b0, [c16]");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Str(CPURegister(b0), MemOperand(c1)), "stur b0, [c1]");
}

TEST(morello_a64_astrb_r_ri_b) {
  SETUP();

  COMPARE_A64(strb(w0, MemOperand(c1, 42)), "strb w0, [c1, #42]");
  COMPARE_A64(strb(w0, MemOperand(c1, 0)), "strb w0, [c1]");
  COMPARE_A64(strb(w0, MemOperand(c1, 511)), "strb w0, [c1, #511]");
  COMPARE_A64(strb(w0, MemOperand(csp, 42)), "strb w0, [csp, #42]");
  COMPARE_A64(strb(wzr, MemOperand(c1, 42)), "strb wzr, [c1, #42]");

  // `sturb` can assemble to `strb`, according to its LoadStoreScalingOption.
  COMPARE_A64(sturb(w0, MemOperand(c1, 256)), "strb w0, [c1, #256]");
  COMPARE_A64(sturb(w0, MemOperand(c1, 511)), "strb w0, [c1, #511]");
}

TEST(morello_a64_astrb_r_rrb_b) {
  SETUP();

  COMPARE_A64(strb(w0, MemOperand(c1, w2, UXTW)), "strb w0, [c1, w2, uxtw]");
  COMPARE_A64(strb(w0, MemOperand(c1, x2, SXTX, 0)), "strb w0, [c1, x2, sxtx]");
  COMPARE_A64(strb(w0, MemOperand(c1, x2, LSL, 0)), "strb w0, [c1, x2]");
  COMPARE_A64(strb(w0, MemOperand(c1, xzr)), "strb w0, [c1, xzr]");
  COMPARE_A64(strb(w0, MemOperand(csp, x2)), "strb w0, [csp, x2]");
  COMPARE_A64(strb(wzr, MemOperand(c1, x2)), "strb wzr, [c1, x2]");
}

TEST(morello_a64_astrh_r_rrb_32) {
  SETUP();

  COMPARE_A64(strh(w0, MemOperand(c1, x2)), "strh w0, [c1, x2]");
  COMPARE_A64(strh(w0, MemOperand(c1, w2, SXTW, 0)), "strh w0, [c1, w2, sxtw]");
  COMPARE_A64(strh(w0, MemOperand(c1, x2, LSL, 1)),
              "strh w0, [c1, x2, lsl #1]");
  COMPARE_A64(strh(w0, MemOperand(c1, xzr, LSL, 1)),
              "strh w0, [c1, xzr, lsl #1]");
  COMPARE_A64(strh(w0, MemOperand(csp, x2, LSL, 1)),
              "strh w0, [csp, x2, lsl #1]");
  COMPARE_A64(strh(wzr, MemOperand(c1, x2, LSL, 1)),
              "strh wzr, [c1, x2, lsl #1]");
}

TEST(morello_a64_astrb_macro) {
  SETUP();

  // Encodable cases.
  // "unsigned offset"
  COMPARE_MACRO_A64(Strb(w0, MemOperand(c1, 42)), "strb w0, [c1, #42]");
  COMPARE_MACRO_A64(Strb(wzr, MemOperand(csp, 42)), "strb wzr, [csp, #42]");
  COMPARE_MACRO_A64(Strb(w0, MemOperand(c1, 511)), "strb w0, [c1, #511]");
  // "unscaled"
  COMPARE_MACRO_A64(Strb(w0, MemOperand(c1, -256)), "sturb w0, [c1, #-256]");
  COMPARE_MACRO_A64(Strb(w0, MemOperand(c1, -1)), "sturb w0, [c1, #-1]");
  COMPARE_MACRO_A64(Strb(wzr, MemOperand(csp, -1)), "sturb wzr, [csp, #-1]");
  // "register"
  COMPARE_MACRO_A64(Strb(w0, MemOperand(c1, x2)), "strb w0, [c1, x2]");
  COMPARE_MACRO_A64(Strb(w0, MemOperand(c1, w2, SXTW)),
                    "strb w0, [c1, w2, sxtw]");

  // The MacroAssembler permits an X-sized source for truncating stores.
  COMPARE_MACRO_A64(Strb(x0, MemOperand(c1, 42)), "strb w0, [c1, #42]");
  COMPARE_MACRO_A64(Strb(x0, MemOperand(c1, -1)), "sturb w0, [c1, #-1]");
  COMPARE_MACRO_A64(Strb(x0, MemOperand(c1, w2, UXTW)),
                    "strb w0, [c1, w2, uxtw]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Strb(w0, MemOperand(c1, 512)),
                    "add c16, c1, #0x200 (512)\n"
                    "strb w0, [c16]");
  // There are no index modes.
  COMPARE_MACRO_A64(Strb(w0, MemOperand(c1, -16, PreIndex)),
                    "sub c1, c1, #0x10 (16)\n"
                    "strb w0, [c1]");
  COMPARE_MACRO_A64(Strb(w0, MemOperand(c1, 16, PostIndex)),
                    "strb w0, [c1]\n"
                    "add c1, c1, #0x10 (16)");
}

TEST(morello_a64_astrh_macro) {
  SETUP();

  // Encodable cases.
  // "unscaled"
  COMPARE_MACRO_A64(Strh(w0, MemOperand(c1, -256)), "sturh w0, [c1, #-256]");
  COMPARE_MACRO_A64(Strh(w0, MemOperand(c1, 255)), "sturh w0, [c1, #255]");
  COMPARE_MACRO_A64(Strh(wzr, MemOperand(csp, 42)), "sturh wzr, [csp, #42]");
  // "register"
  COMPARE_MACRO_A64(Strh(w0, MemOperand(c1, x2)), "strh w0, [c1, x2]");
  COMPARE_MACRO_A64(Strh(w0, MemOperand(c1, w2, SXTW)),
                    "strh w0, [c1, w2, sxtw]");
  COMPARE_MACRO_A64(Strh(w0, MemOperand(c1, x2, LSL, 1)),
                    "strh w0, [c1, x2, lsl #1]");

  // The MacroAssembler permits an X-sized source for truncating stores.
  COMPARE_MACRO_A64(Strh(x0, MemOperand(c1, 42)), "sturh w0, [c1, #42]");
  COMPARE_MACRO_A64(Strh(x0, MemOperand(c1, x2, SXTX, 1)),
                    "strh w0, [c1, x2, sxtx #1]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Strh(w0, MemOperand(c1, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add c16, c1, x16, uxtx\n"
                    "sturh w0, [c16]");
  // There are no index modes.
  COMPARE_MACRO_A64(Strh(w0, MemOperand(c1, -16, PreIndex)),
                    "sub c1, c1, #0x10 (16)\n"
                    "sturh w0, [c1]");
  COMPARE_MACRO_A64(Strh(w0, MemOperand(c1, 16, PostIndex)),
                    "sturh w0, [c1]\n"
                    "add c1, c1, #0x10 (16)");
  // There is no "unsigned offset" mode.
  COMPARE_MACRO_A64(Strh(w0, MemOperand(c1, 256)),
                    "add c16, c1, #0x100 (256)\n"
                    "sturh w0, [c16]");
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

  COMPARE_A64(stur(w0, MemOperand(c1, 255)), "stur w0, [c1, #255]");
  COMPARE_A64(stur(w0, MemOperand(c1, -256)), "stur w0, [c1, #-256]");
  COMPARE_A64(stur(w0, MemOperand(c1, 0)), "stur w0, [c1]");
  COMPARE_A64(stur(w0, MemOperand(csp, 42)), "stur w0, [csp, #42]");
  COMPARE_A64(stur(wzr, MemOperand(c1, 42)), "stur wzr, [c1, #42]");

  // `str` can assemble to `stur`, according to its LoadStoreScalingOption.
  COMPARE_A64(str(w0, MemOperand(c1, -1)), "stur w0, [c1, #-1]");
  COMPARE_A64(str(w0, MemOperand(c1, 42)), "stur w0, [c1, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(stur(CPURegister(w0), MemOperand(c1, 0)), "stur w0, [c1]");
  COMPARE_A64(stur(CPURegister(wzr), MemOperand(c1, 42)),
              "stur wzr, [c1, #42]");
}

TEST(morello_a64_astur_r_ri_64) {
  SETUP();

  COMPARE_A64(stur(x0, MemOperand(c1, 255)), "stur x0, [c1, #255]");
  COMPARE_A64(stur(x0, MemOperand(c1, -256)), "stur x0, [c1, #-256]");
  COMPARE_A64(stur(x0, MemOperand(c1, 0)), "stur x0, [c1]");
  COMPARE_A64(stur(x0, MemOperand(csp, 42)), "stur x0, [csp, #42]");
  COMPARE_A64(stur(xzr, MemOperand(c1, 42)), "stur xzr, [c1, #42]");

  // `str` can assemble to `stur`, according to its LoadStoreScalingOption.
  COMPARE_A64(str(x0, MemOperand(c1, -1)), "stur x0, [c1, #-1]");
  COMPARE_A64(str(x0, MemOperand(c1, 42)), "stur x0, [c1, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(stur(CPURegister(x0), MemOperand(c1, 0)), "stur x0, [c1]");
  COMPARE_A64(stur(CPURegister(xzr), MemOperand(c1, 42)),
              "stur xzr, [c1, #42]");
}

TEST(morello_a64_astur_v_ri_b) {
  SETUP();

  COMPARE_A64(stur(b0, MemOperand(c1, 255)), "stur b0, [c1, #255]");
  COMPARE_A64(stur(b0, MemOperand(c1, -256)), "stur b0, [c1, #-256]");
  COMPARE_A64(stur(b0, MemOperand(c1, 0)), "stur b0, [c1]");
  COMPARE_A64(stur(b0, MemOperand(csp, 42)), "stur b0, [csp, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(stur(CPURegister(b0), MemOperand(c1, 0)), "stur b0, [c1]");
}

TEST(morello_a64_astur_v_ri_d) {
  SETUP();

  COMPARE_A64(stur(d0, MemOperand(c1, 255)), "stur d0, [c1, #255]");
  COMPARE_A64(stur(d0, MemOperand(c1, -256)), "stur d0, [c1, #-256]");
  COMPARE_A64(stur(d0, MemOperand(c1, 0)), "stur d0, [c1]");
  COMPARE_A64(stur(d0, MemOperand(csp, 42)), "stur d0, [csp, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(stur(CPURegister(d0), MemOperand(c1, 0)), "stur d0, [c1]");
}

TEST(morello_a64_astur_v_ri_h) {
  SETUP();

  COMPARE_A64(stur(h0, MemOperand(c1, 255)), "stur h0, [c1, #255]");
  COMPARE_A64(stur(h0, MemOperand(c1, -256)), "stur h0, [c1, #-256]");
  COMPARE_A64(stur(h0, MemOperand(c1, 0)), "stur h0, [c1]");
  COMPARE_A64(stur(h0, MemOperand(csp, 42)), "stur h0, [csp, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(stur(CPURegister(h0), MemOperand(c1, 0)), "stur h0, [c1]");
}

TEST(morello_a64_astur_v_ri_q) {
  SETUP();

  COMPARE_A64(stur(q0, MemOperand(c1, 255)), "stur q0, [c1, #255]");
  COMPARE_A64(stur(q0, MemOperand(c1, -256)), "stur q0, [c1, #-256]");
  COMPARE_A64(stur(q0, MemOperand(c1, 0)), "stur q0, [c1]");
  COMPARE_A64(stur(q0, MemOperand(csp, 42)), "stur q0, [csp, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(stur(CPURegister(q0), MemOperand(c1, 0)), "stur q0, [c1]");
}

TEST(morello_a64_astur_v_ri_s) {
  SETUP();

  COMPARE_A64(stur(s0, MemOperand(c1, 255)), "stur s0, [c1, #255]");
  COMPARE_A64(stur(s0, MemOperand(c1, -256)), "stur s0, [c1, #-256]");
  COMPARE_A64(stur(s0, MemOperand(c1, 0)), "stur s0, [c1]");
  COMPARE_A64(stur(s0, MemOperand(csp, 42)), "stur s0, [csp, #42]");

  // A generic CPURegister works the same.
  COMPARE_A64(stur(CPURegister(s0), MemOperand(c1, 0)), "stur s0, [c1]");
}

TEST(morello_a64_asturb_r_ri_32) {
  SETUP();

  COMPARE_A64(sturb(w0, MemOperand(c1, 42)), "sturb w0, [c1, #42]");
  COMPARE_A64(sturb(w0, MemOperand(c1, 0)), "sturb w0, [c1]");
  COMPARE_A64(sturb(w0, MemOperand(c1, -256)), "sturb w0, [c1, #-256]");
  COMPARE_A64(sturb(w0, MemOperand(c1, 255)), "sturb w0, [c1, #255]");
  COMPARE_A64(sturb(w0, MemOperand(csp, 42)), "sturb w0, [csp, #42]");
  COMPARE_A64(sturb(wzr, MemOperand(c1, 42)), "sturb wzr, [c1, #42]");

  // `strb` can assemble to `sturb`, according to its LoadStoreScalingOption.
  COMPARE_A64(sturb(w0, MemOperand(c1, -1)), "sturb w0, [c1, #-1]");
}

TEST(morello_a64_asturh_r_ri_32) {
  SETUP();

  COMPARE_A64(sturh(w0, MemOperand(c1, 42)), "sturh w0, [c1, #42]");
  COMPARE_A64(sturh(w0, MemOperand(c1, 0)), "sturh w0, [c1]");
  COMPARE_A64(sturh(w0, MemOperand(c1, -256)), "sturh w0, [c1, #-256]");
  COMPARE_A64(sturh(w0, MemOperand(c1, 255)), "sturh w0, [c1, #255]");
  COMPARE_A64(sturh(w0, MemOperand(csp, 42)), "sturh w0, [csp, #42]");
  COMPARE_A64(sturh(wzr, MemOperand(c1, 42)), "sturh wzr, [c1, #42]");

  // `strh` can assemble to `sturh`, according to its LoadStoreScalingOption.
  COMPARE_A64(sturh(w0, MemOperand(c1, -1)), "sturh w0, [c1, #-1]");
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

  COMPARE_A64(ldapr(c0, MemOperand(x1)), "ldapr c0, [x1]");
  COMPARE_A64(ldapr(c0, MemOperand(sp)), "ldapr c0, [sp]");
  COMPARE_A64(ldapr(czr, MemOperand(x1)), "ldapr czr, [x1]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Ldapr(c30, MemOperand(x29)), "ldapr c30, [x29]");
  COMPARE_MACRO_A64(Ldapr(czr, MemOperand(sp)), "ldapr czr, [sp]");
}

TEST(morello_a64_ldar_c_r_c) {
  SETUP();

  COMPARE_A64(ldar(c0, MemOperand(x1)), "ldar c0, [x1]");
  COMPARE_A64(ldar(c0, MemOperand(sp)), "ldar c0, [sp]");
  COMPARE_A64(ldar(czr, MemOperand(x1)), "ldar czr, [x1]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Ldar(c30, MemOperand(x29)), "ldar c30, [x29]");
  COMPARE_MACRO_A64(Ldar(czr, MemOperand(sp)), "ldar czr, [sp]");
}

TEST(morello_a64_ldaxp_c_r_c) {
  SETUP();

  COMPARE_A64(ldaxp(c0, c1, MemOperand(x2)), "ldaxp c0, c1, [x2]");
  COMPARE_A64(ldaxp(c0, c1, MemOperand(sp)), "ldaxp c0, c1, [sp]");
  COMPARE_A64(ldaxp(c0, czr, MemOperand(x2)), "ldaxp c0, czr, [x2]");
  COMPARE_A64(ldaxp(czr, c1, MemOperand(x2)), "ldaxp czr, c1, [x2]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Ldaxp(c30, c29, MemOperand(x28)), "ldaxp c30, c29, [x28]");
  COMPARE_MACRO_A64(Ldaxp(c30, czr, MemOperand(sp)), "ldaxp c30, czr, [sp]");
}

TEST(morello_a64_ldaxr_c_r_c) {
  SETUP();

  COMPARE_A64(ldaxr(c0, MemOperand(x1)), "ldaxr c0, [x1]");
  COMPARE_A64(ldaxr(c0, MemOperand(sp)), "ldaxr c0, [sp]");
  COMPARE_A64(ldaxr(czr, MemOperand(x1)), "ldaxr czr, [x1]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Ldar(c30, MemOperand(x29)), "ldar c30, [x29]");
  COMPARE_MACRO_A64(Ldar(czr, MemOperand(sp)), "ldar czr, [sp]");
}

TEST(morello_a64_ldct_r_r_) {
  SETUP();

  // COMPARE_A64(ldct(x0, MemOperand(x1)), "TODO");
}

TEST(morello_a64_ldnp_c_rib_c) {
  SETUP();

  COMPARE_A64(ldnp(c0, c1, MemOperand(x2, 272)), "ldnp c0, c1, [x2, #272]");
  COMPARE_A64(ldnp(c0, c1, MemOperand(sp, 272)), "ldnp c0, c1, [sp, #272]");
  COMPARE_A64(ldnp(c0, czr, MemOperand(x2, 272)), "ldnp c0, czr, [x2, #272]");
  COMPARE_A64(ldnp(czr, c1, MemOperand(x2, 272)), "ldnp czr, c1, [x2, #272]");
  COMPARE_A64(ldnp(c0, c1, MemOperand(x2, 0)), "ldnp c0, c1, [x2]");
  COMPARE_A64(ldnp(c0, c1, MemOperand(x2, -1024)), "ldnp c0, c1, [x2, #-1024]");
  COMPARE_A64(ldnp(c0, c1, MemOperand(x2, 1008)), "ldnp c0, c1, [x2, #1008]");
}

TEST(morello_a64_ldnp_c_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Ldnp(c0, c1, MemOperand(x2)), "ldnp c0, c1, [x2]");
  COMPARE_MACRO_A64(Ldnp(c0, c1, MemOperand(x2, 1008)),
                    "ldnp c0, c1, [x2, #1008]");
  COMPARE_MACRO_A64(Ldnp(c0, c1, MemOperand(x2, -1024)),
                    "ldnp c0, c1, [x2, #-1024]");
  COMPARE_MACRO_A64(Ldnp(czr, c30, MemOperand(sp)), "ldnp czr, c30, [sp]");
  COMPARE_MACRO_A64(Ldnp(c30, czr, MemOperand(sp)), "ldnp c30, czr, [sp]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldnp(c0, c1, MemOperand(x2, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add x16, x2, x16\n"
                    "ldnp c0, c1, [x16]");
  // There are no addressing modes other than immediate offset.
  COMPARE_MACRO_A64(Ldnp(c0, c1, MemOperand(x2, x3)),
                    "add x16, x2, x3\n"
                    "ldnp c0, c1, [x16]");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Ldnp(CPURegister(c0), CPURegister(c1), MemOperand(x2)),
                    "ldnp c0, c1, [x2]");
}

TEST(morello_a64_ldp_c_rib_c) {
  SETUP();

  COMPARE_A64(ldp(c0, c1, MemOperand(x2, 272)), "ldp c0, c1, [x2, #272]");
  COMPARE_A64(ldp(c0, c1, MemOperand(sp, 272)), "ldp c0, c1, [sp, #272]");
  COMPARE_A64(ldp(c0, czr, MemOperand(x2, 272)), "ldp c0, czr, [x2, #272]");
  COMPARE_A64(ldp(czr, c1, MemOperand(x2, 272)), "ldp czr, c1, [x2, #272]");
  COMPARE_A64(ldp(c0, c1, MemOperand(x2, 0)), "ldp c0, c1, [x2]");
  COMPARE_A64(ldp(c0, c1, MemOperand(x2, -1024)), "ldp c0, c1, [x2, #-1024]");
  COMPARE_A64(ldp(c0, c1, MemOperand(x2, 1008)), "ldp c0, c1, [x2, #1008]");
}

TEST(morello_a64_ldp_c_ribw_c) {
  SETUP();

  COMPARE_A64(ldp(c0, c1, MemOperand(x2, 272, PreIndex)),
              "ldp c0, c1, [x2, #272]!");
  COMPARE_A64(ldp(c0, c1, MemOperand(sp, 272, PreIndex)),
              "ldp c0, c1, [sp, #272]!");
  COMPARE_A64(ldp(c0, czr, MemOperand(x2, 272, PreIndex)),
              "ldp c0, czr, [x2, #272]!");
  COMPARE_A64(ldp(czr, c1, MemOperand(x2, 272, PreIndex)),
              "ldp czr, c1, [x2, #272]!");
  COMPARE_A64(ldp(c0, c1, MemOperand(x2, 0, PreIndex)),
              "ldp c0, c1, [x2, #0]!");
  COMPARE_A64(ldp(c0, c1, MemOperand(x2, -1024, PreIndex)),
              "ldp c0, c1, [x2, #-1024]!");
  COMPARE_A64(ldp(c0, c1, MemOperand(x2, 1008, PreIndex)),
              "ldp c0, c1, [x2, #1008]!");
}

TEST(morello_a64_ldp_cc_riaw_c) {
  SETUP();

  COMPARE_A64(ldp(c0, c1, MemOperand(x2, 272, PostIndex)),
              "ldp c0, c1, [x2], #272");
  COMPARE_A64(ldp(c0, c1, MemOperand(sp, 272, PostIndex)),
              "ldp c0, c1, [sp], #272");
  COMPARE_A64(ldp(c0, czr, MemOperand(x2, 272, PostIndex)),
              "ldp c0, czr, [x2], #272");
  COMPARE_A64(ldp(czr, c1, MemOperand(x2, 272, PostIndex)),
              "ldp czr, c1, [x2], #272");
  COMPARE_A64(ldp(c0, c1, MemOperand(x2, 0, PostIndex)),
              "ldp c0, c1, [x2], #0");
  COMPARE_A64(ldp(c0, c1, MemOperand(x2, -1024, PostIndex)),
              "ldp c0, c1, [x2], #-1024");
  COMPARE_A64(ldp(c0, c1, MemOperand(x2, 1008, PostIndex)),
              "ldp c0, c1, [x2], #1008");
}

TEST(morello_a64_ldp_c_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2)), "ldp c0, c1, [x2]");
  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2, 1008)),
                    "ldp c0, c1, [x2, #1008]");
  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2, -1024)),
                    "ldp c0, c1, [x2, #-1024]");
  COMPARE_MACRO_A64(Ldp(czr, c30, MemOperand(sp)), "ldp czr, c30, [sp]");
  COMPARE_MACRO_A64(Ldp(c30, czr, MemOperand(sp)), "ldp c30, czr, [sp]");

  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2, 0, PostIndex)),
                    "ldp c0, c1, [x2], #0");
  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2, -1024, PostIndex)),
                    "ldp c0, c1, [x2], #-1024");
  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2, 1008, PostIndex)),
                    "ldp c0, c1, [x2], #1008");
  COMPARE_MACRO_A64(Ldp(czr, c30, MemOperand(sp, 0, PostIndex)),
                    "ldp czr, c30, [sp], #0");
  COMPARE_MACRO_A64(Ldp(c30, czr, MemOperand(sp, 0, PostIndex)),
                    "ldp c30, czr, [sp], #0");

  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2, 0, PreIndex)),
                    "ldp c0, c1, [x2, #0]!");
  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2, -1024, PreIndex)),
                    "ldp c0, c1, [x2, #-1024]!");
  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2, 1008, PreIndex)),
                    "ldp c0, c1, [x2, #1008]!");
  COMPARE_MACRO_A64(Ldp(czr, c30, MemOperand(sp, 0, PreIndex)),
                    "ldp czr, c30, [sp, #0]!");
  COMPARE_MACRO_A64(Ldp(c30, czr, MemOperand(sp, 0, PreIndex)),
                    "ldp c30, czr, [sp, #0]!");

  // Unencodable cases.
  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add x16, x2, x16\n"
                    "ldp c0, c1, [x16]");
  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2, 0x4242, PreIndex)),
                    "mov x16, #0x4242\n"
                    "add x2, x2, x16\n"
                    "ldp c0, c1, [x2]");
  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2, 0x4242, PostIndex)),
                    "ldp c0, c1, [x2]\n"
                    "mov x16, #0x4242\n"
                    "add x2, x2, x16");
  // There is no register-offset mode.
  COMPARE_MACRO_A64(Ldp(c0, c1, MemOperand(x2, x3)),
                    "add x16, x2, x3\n"
                    "ldp c0, c1, [x16]");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Ldp(CPURegister(c0),
                        CPURegister(c1),
                        MemOperand(x2, 0, PostIndex)),
                    "ldp c0, c1, [x2], #0");
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

  COMPARE_A64(ldxp(c0, c1, MemOperand(x2)), "ldxp c0, c1, [x2]");
  COMPARE_A64(ldxp(c0, c1, MemOperand(sp)), "ldxp c0, c1, [sp]");
  COMPARE_A64(ldxp(c0, czr, MemOperand(x2)), "ldxp c0, czr, [x2]");
  COMPARE_A64(ldxp(czr, c1, MemOperand(x2)), "ldxp czr, c1, [x2]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Ldxp(c30, c29, MemOperand(x28)), "ldxp c30, c29, [x28]");
  COMPARE_MACRO_A64(Ldxp(c30, czr, MemOperand(sp)), "ldxp c30, czr, [sp]");
}

TEST(morello_a64_ldxr_c_r_c) {
  SETUP();

  COMPARE_A64(ldxr(c0, MemOperand(x1)), "ldxr c0, [x1]");
  COMPARE_A64(ldxr(c0, MemOperand(sp)), "ldxr c0, [sp]");
  COMPARE_A64(ldxr(czr, MemOperand(x1)), "ldxr czr, [x1]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Ldxr(c30, MemOperand(x29)), "ldxr c30, [x29]");
  COMPARE_MACRO_A64(Ldxr(czr, MemOperand(sp)), "ldxr czr, [sp]");
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

  COMPARE_A64(stlr(c0, MemOperand(x1)), "stlr c0, [x1]");
  COMPARE_A64(stlr(c0, MemOperand(sp)), "stlr c0, [sp]");
  COMPARE_A64(stlr(czr, MemOperand(x1)), "stlr czr, [x1]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Stlr(c30, MemOperand(x29)), "stlr c30, [x29]");
  COMPARE_MACRO_A64(Stlr(czr, MemOperand(sp)), "stlr czr, [sp]");
}

TEST(morello_a64_stlxp_r_cr_c) {
  SETUP();

  COMPARE_A64(stlxp(w0, c1, c2, MemOperand(x3)), "stlxp w0, c1, c2, [x3]");
  COMPARE_A64(stlxp(w0, c1, c2, MemOperand(sp)), "stlxp w0, c1, c2, [sp]");
  COMPARE_A64(stlxp(w0, c1, czr, MemOperand(x3)), "stlxp w0, c1, czr, [x3]");
  COMPARE_A64(stlxp(w0, czr, c2, MemOperand(x3)), "stlxp w0, czr, c2, [x3]");
  COMPARE_A64(stlxp(wzr, c1, c2, MemOperand(x3)), "stlxp wzr, c1, c2, [x3]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Stlxp(w30, c29, c28, MemOperand(x27)),
                    "stlxp w30, c29, c28, [x27]");
  COMPARE_MACRO_A64(Stlxp(w30, c29, czr, MemOperand(sp)),
                    "stlxp w30, c29, czr, [sp]");
}

TEST(morello_a64_stlxr_r_cr_c) {
  SETUP();

  COMPARE_A64(stlxr(w0, c1, MemOperand(x2)), "stlxr w0, c1, [x2]");
  COMPARE_A64(stlxr(w0, c1, MemOperand(sp)), "stlxr w0, c1, [sp]");
  COMPARE_A64(stlxr(w0, czr, MemOperand(x2)), "stlxr w0, czr, [x2]");
  COMPARE_A64(stlxr(wzr, c1, MemOperand(x2)), "stlxr wzr, c1, [x2]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Stlxr(w30, c29, MemOperand(x28)), "stlxr w30, c29, [x28]");
  COMPARE_MACRO_A64(Stlxr(wzr, czr, MemOperand(sp)), "stlxr wzr, czr, [sp]");
}

TEST(morello_a64_stnp_c_rib_c) {
  SETUP();

  COMPARE_A64(stnp(c0, c1, MemOperand(x2, 272)), "stnp c0, c1, [x2, #272]");
  COMPARE_A64(stnp(c0, c1, MemOperand(sp, 272)), "stnp c0, c1, [sp, #272]");
  COMPARE_A64(stnp(c0, czr, MemOperand(x2, 272)), "stnp c0, czr, [x2, #272]");
  COMPARE_A64(stnp(czr, c1, MemOperand(x2, 272)), "stnp czr, c1, [x2, #272]");
  COMPARE_A64(stnp(c0, c1, MemOperand(x2, 0)), "stnp c0, c1, [x2]");
  COMPARE_A64(stnp(c0, c1, MemOperand(x2, -1024)), "stnp c0, c1, [x2, #-1024]");
  COMPARE_A64(stnp(c0, c1, MemOperand(x2, 1008)), "stnp c0, c1, [x2, #1008]");
}

TEST(morello_a64_stnp_c_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Stnp(c0, c1, MemOperand(x2)), "stnp c0, c1, [x2]");
  COMPARE_MACRO_A64(Stnp(c0, c1, MemOperand(x2, 1008)),
                    "stnp c0, c1, [x2, #1008]");
  COMPARE_MACRO_A64(Stnp(c0, c1, MemOperand(x2, -1024)),
                    "stnp c0, c1, [x2, #-1024]");
  COMPARE_MACRO_A64(Stnp(czr, c30, MemOperand(sp)), "stnp czr, c30, [sp]");
  COMPARE_MACRO_A64(Stnp(c30, czr, MemOperand(sp)), "stnp c30, czr, [sp]");

  // Unencodable cases.
  COMPARE_MACRO_A64(Stnp(c0, c1, MemOperand(x2, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add x16, x2, x16\n"
                    "stnp c0, c1, [x16]");
  // There are no addressing modes other than immediate offset.
  COMPARE_MACRO_A64(Stnp(c0, c1, MemOperand(x2, x3)),
                    "add x16, x2, x3\n"
                    "stnp c0, c1, [x16]");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Stnp(CPURegister(c0), CPURegister(c1), MemOperand(x2)),
                    "stnp c0, c1, [x2]");
}

TEST(morello_a64_stp_c_rib_c) {
  SETUP();

  COMPARE_A64(stp(c0, c1, MemOperand(x2, 272)), "stp c0, c1, [x2, #272]");
  COMPARE_A64(stp(c0, c1, MemOperand(sp, 272)), "stp c0, c1, [sp, #272]");
  COMPARE_A64(stp(c0, czr, MemOperand(x2, 272)), "stp c0, czr, [x2, #272]");
  COMPARE_A64(stp(czr, c1, MemOperand(x2, 272)), "stp czr, c1, [x2, #272]");
  COMPARE_A64(stp(c0, c1, MemOperand(x2, 0)), "stp c0, c1, [x2]");
  COMPARE_A64(stp(c0, c1, MemOperand(x2, -1024)), "stp c0, c1, [x2, #-1024]");
  COMPARE_A64(stp(c0, c1, MemOperand(x2, 1008)), "stp c0, c1, [x2, #1008]");
}

TEST(morello_a64_stp_c_ribw_c) {
  SETUP();

  COMPARE_A64(stp(c0, c1, MemOperand(x2, 272, PreIndex)),
              "stp c0, c1, [x2, #272]!");
  COMPARE_A64(stp(c0, c1, MemOperand(sp, 272, PreIndex)),
              "stp c0, c1, [sp, #272]!");
  COMPARE_A64(stp(c0, czr, MemOperand(x2, 272, PreIndex)),
              "stp c0, czr, [x2, #272]!");
  COMPARE_A64(stp(czr, c1, MemOperand(x2, 272, PreIndex)),
              "stp czr, c1, [x2, #272]!");
  COMPARE_A64(stp(c0, c1, MemOperand(x2, 0, PreIndex)),
              "stp c0, c1, [x2, #0]!");
  COMPARE_A64(stp(c0, c1, MemOperand(x2, -1024, PreIndex)),
              "stp c0, c1, [x2, #-1024]!");
  COMPARE_A64(stp(c0, c1, MemOperand(x2, 1008, PreIndex)),
              "stp c0, c1, [x2, #1008]!");
}

TEST(morello_a64_stp_cc_riaw_c) {
  SETUP();

  COMPARE_A64(stp(c0, c1, MemOperand(x2, 272, PostIndex)),
              "stp c0, c1, [x2], #272");
  COMPARE_A64(stp(c0, c1, MemOperand(sp, 272, PostIndex)),
              "stp c0, c1, [sp], #272");
  COMPARE_A64(stp(c0, czr, MemOperand(x2, 272, PostIndex)),
              "stp c0, czr, [x2], #272");
  COMPARE_A64(stp(czr, c1, MemOperand(x2, 272, PostIndex)),
              "stp czr, c1, [x2], #272");
  COMPARE_A64(stp(c0, c1, MemOperand(x2, 0, PostIndex)),
              "stp c0, c1, [x2], #0");
  COMPARE_A64(stp(c0, c1, MemOperand(x2, -1024, PostIndex)),
              "stp c0, c1, [x2], #-1024");
  COMPARE_A64(stp(c0, c1, MemOperand(x2, 1008, PostIndex)),
              "stp c0, c1, [x2], #1008");
}

TEST(morello_a64_stp_c_macro) {
  SETUP();

  // Encodable cases.
  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2)), "stp c0, c1, [x2]");
  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2, 1008)),
                    "stp c0, c1, [x2, #1008]");
  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2, -1024)),
                    "stp c0, c1, [x2, #-1024]");
  COMPARE_MACRO_A64(Stp(czr, c30, MemOperand(sp)), "stp czr, c30, [sp]");
  COMPARE_MACRO_A64(Stp(c30, czr, MemOperand(sp)), "stp c30, czr, [sp]");

  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2, 0, PostIndex)),
                    "stp c0, c1, [x2], #0");
  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2, -1024, PostIndex)),
                    "stp c0, c1, [x2], #-1024");
  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2, 1008, PostIndex)),
                    "stp c0, c1, [x2], #1008");
  COMPARE_MACRO_A64(Stp(czr, c30, MemOperand(sp, 0, PostIndex)),
                    "stp czr, c30, [sp], #0");
  COMPARE_MACRO_A64(Stp(c30, czr, MemOperand(sp, 0, PostIndex)),
                    "stp c30, czr, [sp], #0");

  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2, 0, PreIndex)),
                    "stp c0, c1, [x2, #0]!");
  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2, -1024, PreIndex)),
                    "stp c0, c1, [x2, #-1024]!");
  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2, 1008, PreIndex)),
                    "stp c0, c1, [x2, #1008]!");
  COMPARE_MACRO_A64(Stp(czr, c30, MemOperand(sp, 0, PreIndex)),
                    "stp czr, c30, [sp, #0]!");
  COMPARE_MACRO_A64(Stp(c30, czr, MemOperand(sp, 0, PreIndex)),
                    "stp c30, czr, [sp, #0]!");

  // Unencodable cases.
  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2, 0x4242)),
                    "mov x16, #0x4242\n"
                    "add x16, x2, x16\n"
                    "stp c0, c1, [x16]");
  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2, 0x4242, PreIndex)),
                    "mov x16, #0x4242\n"
                    "add x2, x2, x16\n"
                    "stp c0, c1, [x2]");
  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2, 0x4242, PostIndex)),
                    "stp c0, c1, [x2]\n"
                    "mov x16, #0x4242\n"
                    "add x2, x2, x16");
  // There is no register-offset mode.
  COMPARE_MACRO_A64(Stp(c0, c1, MemOperand(x2, x3)),
                    "add x16, x2, x3\n"
                    "stp c0, c1, [x16]");

  // A generic CPURegister produces the same result.
  COMPARE_MACRO_A64(Stp(CPURegister(c0),
                        CPURegister(c1),
                        MemOperand(x2, 0, PostIndex)),
                    "stp c0, c1, [x2], #0");
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

  COMPARE_A64(stxp(w0, c1, c2, MemOperand(x3)), "stxp w0, c1, c2, [x3]");
  COMPARE_A64(stxp(w0, c1, c2, MemOperand(sp)), "stxp w0, c1, c2, [sp]");
  COMPARE_A64(stxp(w0, c1, czr, MemOperand(x3)), "stxp w0, c1, czr, [x3]");
  COMPARE_A64(stxp(w0, czr, c2, MemOperand(x3)), "stxp w0, czr, c2, [x3]");
  COMPARE_A64(stxp(wzr, c1, c2, MemOperand(x3)), "stxp wzr, c1, c2, [x3]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Stxp(w30, c29, c28, MemOperand(x27)),
                    "stxp w30, c29, c28, [x27]");
  COMPARE_MACRO_A64(Stxp(w30, c29, czr, MemOperand(sp)),
                    "stxp w30, c29, czr, [sp]");
}

TEST(morello_a64_stxr_r_cr_c) {
  SETUP();

  COMPARE_A64(stxr(w0, c1, MemOperand(x2)), "stxr w0, c1, [x2]");
  COMPARE_A64(stxr(w0, c1, MemOperand(sp)), "stxr w0, c1, [sp]");
  COMPARE_A64(stxr(w0, czr, MemOperand(x2)), "stxr w0, czr, [x2]");
  COMPARE_A64(stxr(wzr, c1, MemOperand(x2)), "stxr wzr, c1, [x2]");

  // The MacroAssembler is a simple pass-through.
  COMPARE_MACRO_A64(Stxr(w30, c29, MemOperand(x28)), "stxr w30, c29, [x28]");
  COMPARE_MACRO_A64(Stxr(wzr, czr, MemOperand(sp)), "stxr wzr, czr, [sp]");
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
