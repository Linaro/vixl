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
  TEST_TEMPLATE_C64(CPUFeatures(CPUFeatures::kMorello), Morello_##NAME, ASM)

TEST_MORELLO(adr_0, adr(c0, 42))
TEST_MORELLO(adr_1, adr(c0, &label))
TEST_MORELLO(adrdp_0, adrdp(c0, 42))
// VIXL does not implement adrdp(..., Label*).
TEST_MORELLO(adrp_0, adrp(c0, 42))
TEST_MORELLO(adrp_1, adrp(c0, &label))
TEST_MORELLO(ldar_0, ldar(c0, MemOperand(x1)))
TEST_MORELLO(ldar_1, ldar(w0, MemOperand(x1)))
TEST_MORELLO(ldar_2, ldar(c0, MemOperand(c1)))
TEST_MORELLO(ldarb_0, ldarb(w0, MemOperand(x1)))
TEST_MORELLO(ldaxp_0, ldaxp(c0, c1, MemOperand(c2)))
TEST_MORELLO(ldaxr_0, ldaxr(c0, MemOperand(c1)))
TEST_MORELLO(ldct_0, ldct(x0, MemOperand(c1)))
TEST_MORELLO(ldnp_0, ldnp(c0, c1, MemOperand(c2, -816)))
TEST_MORELLO(ldp_0, ldp(c0, c1, MemOperand(c2, 272)))
TEST_MORELLO(ldp_1, ldp(c0, c1, MemOperand(c2, -352, PreIndex)))
TEST_MORELLO(ldp_2, ldp(c0, c1, MemOperand(c2, -256, PostIndex)))
TEST_MORELLO(ldpblr_0, ldpblr(c0, MemOperand(c1)))
TEST_MORELLO(ldpbr_0, ldpbr(c0, MemOperand(c1)))
TEST_MORELLO(ldr_0, ldr(c0, MemOperand(x1, 6928)))
TEST_MORELLO(ldr_1, ldr(c0, MemOperand(x1, w2, UXTW, 4)))
TEST_MORELLO(ldr_2, ldr(c0, MemOperand(x1, x2, SXTX, 0)))
TEST_MORELLO(ldr_3, ldr(w0, MemOperand(x1, 1508)))
TEST_MORELLO(ldr_4, ldr(x0, MemOperand(x1, 640)))
TEST_MORELLO(ldr_5, ldr(w0, MemOperand(x1, w2, SXTW, 2)))
TEST_MORELLO(ldr_6, ldr(w0, MemOperand(x1, x2, SXTX, 0)))
TEST_MORELLO(ldr_7, ldr(x0, MemOperand(x1, w2, UXTW, 0)))
TEST_MORELLO(ldr_8, ldr(x0, MemOperand(x1, x2, SXTX, 0)))
TEST_MORELLO(ldr_10, ldr(c0, MemOperand(c1, 512, PostIndex)))
TEST_MORELLO(ldr_11, ldr(c0, MemOperand(c1, 6928)))
TEST_MORELLO(ldr_12, ldr(c0, MemOperand(c1, -4096, PreIndex)))
TEST_MORELLO(ldr_13, ldr(c0, MemOperand(c1, w2, UXTW, 4)))
TEST_MORELLO(ldr_14, ldr(c0, MemOperand(c1, x2, SXTX, 0)))
TEST_MORELLO(ldrb_0, ldrb(w0, MemOperand(x1, 432)))
TEST_MORELLO(ldrb_1, ldrb(w0, MemOperand(x1, w2, UXTW)))
TEST_MORELLO(ldrb_2, ldrb(w0, MemOperand(x1, w2, SXTW)))
TEST_MORELLO(ldrb_3, ldrb(w0, MemOperand(x1, x2, LSL)))
TEST_MORELLO(ldrb_4, ldrb(w0, MemOperand(x1, x2, SXTX)))
TEST_MORELLO(ldrh_0, ldrh(w0, MemOperand(x1, w2, UXTW, 1)))
TEST_MORELLO(ldrh_1, ldrh(w0, MemOperand(x1, x2, LSL, 0)))
TEST_MORELLO(ldrsb_0, ldrsb(w0, MemOperand(x1, w2, UXTW)))
TEST_MORELLO(ldrsb_1, ldrsb(w0, MemOperand(x1, w2, SXTW)))
TEST_MORELLO(ldrsb_2, ldrsb(w0, MemOperand(x1, x2, LSL)))
TEST_MORELLO(ldrsb_3, ldrsb(w0, MemOperand(x1, x2, SXTX)))
TEST_MORELLO(ldrsb_4, ldrsb(x0, MemOperand(x1, w2, UXTW)))
TEST_MORELLO(ldrsb_5, ldrsb(x0, MemOperand(x1, w2, SXTW)))
TEST_MORELLO(ldrsb_6, ldrsb(x0, MemOperand(x1, x2, LSL)))
TEST_MORELLO(ldrsb_7, ldrsb(x0, MemOperand(x1, x2, SXTX)))
TEST_MORELLO(ldrsh_0, ldrsh(w0, MemOperand(x1, w2, UXTW, 0)))
TEST_MORELLO(ldrsh_1, ldrsh(w0, MemOperand(x1, x2, SXTX, 1)))
TEST_MORELLO(ldrsh_2, ldrsh(x0, MemOperand(x1, w2, UXTW, 0)))
TEST_MORELLO(ldrsh_3, ldrsh(x0, MemOperand(x1, x2, SXTX, 1)))
TEST_MORELLO(ldur_0, ldur(c0, MemOperand(x1, 182)))
TEST_MORELLO(ldur_1, ldur(w0, MemOperand(x1, 17)))
TEST_MORELLO(ldur_2, ldur(x0, MemOperand(x1, 197)))
TEST_MORELLO(ldur_3, ldur(c0, MemOperand(c1, 182)))
TEST_MORELLO(ldurb_0, ldurb(w0, MemOperand(x1, 16)))
TEST_MORELLO(ldurh_0, ldurh(w0, MemOperand(x1, 211)))
TEST_MORELLO(ldursb_0, ldursb(w0, MemOperand(x1, -123)))
TEST_MORELLO(ldursb_1, ldursb(x0, MemOperand(x1, 156)))
TEST_MORELLO(ldursh_0, ldursh(w0, MemOperand(x1, -228)))
TEST_MORELLO(ldursh_1, ldursh(x0, MemOperand(x1, 243)))
TEST_MORELLO(ldursw_0, ldursw(x0, MemOperand(x1, 108)))
TEST_MORELLO(ldxp_0, ldxp(c0, c1, MemOperand(c2)))
TEST_MORELLO(ldxr_0, ldxr(c0, MemOperand(c1)))
// VIXL does not support `stct`, because it isn't available at EL0.
TEST_MORELLO(stlr_0, stlr(c0, MemOperand(x1)))
TEST_MORELLO(stlr_1, stlr(w0, MemOperand(x1)))
TEST_MORELLO(stlr_2, stlr(c0, MemOperand(c1)))
TEST_MORELLO(stlrb_0, stlrb(w0, MemOperand(x1)))
TEST_MORELLO(stlxp_0, stlxp(w0, c1, c2, MemOperand(c3)))
TEST_MORELLO(stlxr_0, stlxr(w0, c1, MemOperand(c2)))
TEST_MORELLO(stnp_0, stnp(c0, c1, MemOperand(c2, -464)))
TEST_MORELLO(stp_0, stp(c0, c1, MemOperand(c2, -128)))
TEST_MORELLO(stp_1, stp(c0, c1, MemOperand(c2, -960, PreIndex)))
TEST_MORELLO(stp_2, stp(c0, c1, MemOperand(c2, 336, PostIndex)))
TEST_MORELLO(str_0, str(c0, MemOperand(x1, 7152)))
TEST_MORELLO(str_1, str(c0, MemOperand(x1, w2, UXTW, 4)))
TEST_MORELLO(str_2, str(c0, MemOperand(x1, x2, LSL, 4)))
TEST_MORELLO(str_3, str(w0, MemOperand(x1, 1520)))
TEST_MORELLO(str_4, str(x0, MemOperand(x1, 2256)))
TEST_MORELLO(str_5, str(w0, MemOperand(x1, w2, SXTW, 2)))
TEST_MORELLO(str_6, str(w0, MemOperand(x1, x2, SXTX, 0)))
TEST_MORELLO(str_7, str(x0, MemOperand(x1, w2, UXTW, 3)))
TEST_MORELLO(str_8, str(x0, MemOperand(x1, x2, SXTX, 3)))
TEST_MORELLO(str_9, str(c0, MemOperand(c1, -4096, PostIndex)))
TEST_MORELLO(str_10, str(c0, MemOperand(c1, 7152)))
TEST_MORELLO(str_11, str(c0, MemOperand(c1, 4080, PreIndex)))
TEST_MORELLO(str_12, str(c0, MemOperand(c1, w2, UXTW, 4)))
TEST_MORELLO(str_13, str(c0, MemOperand(c1, x2, LSL, 4)))
TEST_MORELLO(strb_0, strb(w0, MemOperand(x1, 0)))
TEST_MORELLO(strb_1, strb(w0, MemOperand(x1, w2, UXTW)))
TEST_MORELLO(strb_2, strb(w0, MemOperand(x1, w2, SXTW)))
TEST_MORELLO(strb_3, strb(w0, MemOperand(x1, x2, LSL)))
TEST_MORELLO(strb_4, strb(w0, MemOperand(x1, x2, SXTX)))
TEST_MORELLO(strh_0, strh(w0, MemOperand(x1, w2, SXTW, 1)))
TEST_MORELLO(strh_1, strh(w0, MemOperand(x1, x2, SXTX, 0)))
TEST_MORELLO(stur_0, stur(c0, MemOperand(x1, -85)))
TEST_MORELLO(stur_1, stur(w0, MemOperand(x1, 179)))
TEST_MORELLO(stur_2, stur(x0, MemOperand(x1, -226)))
TEST_MORELLO(stur_3, stur(c0, MemOperand(c1, -85)))
TEST_MORELLO(sturb_0, sturb(w0, MemOperand(x1, 154)))
TEST_MORELLO(sturh_0, sturh(w0, MemOperand(x1, 247)))
TEST_MORELLO(stxp_0, stxp(w0, c1, c2, MemOperand(c3)))
TEST_MORELLO(stxr_0, stxr(w0, c1, MemOperand(c2)))

// `ret` defaults to `clr` in C64 mode.
TEST_MORELLO(ret_default, ret())

#define TEST_FP_MORELLO(NAME, ASM)                                        \
  TEST_TEMPLATE_C64(CPUFeatures(CPUFeatures::kFP, CPUFeatures::kMorello), \
                    FP_Morello_##NAME,                                    \
                    ASM)

TEST_FP_MORELLO(ldr_0, ldr(d0, MemOperand(x1, w2, SXTW, 3)))
TEST_FP_MORELLO(ldr_1, ldr(d0, MemOperand(x1, x2, SXTX, 3)))
TEST_FP_MORELLO(ldr_2, ldr(s0, MemOperand(x1, w2, SXTW, 0)))
TEST_FP_MORELLO(ldr_3, ldr(s0, MemOperand(x1, x2, SXTX, 2)))
TEST_FP_MORELLO(ldur_0, ldur(d0, MemOperand(x1, -167)))
TEST_FP_MORELLO(ldur_1, ldur(h0, MemOperand(x1, 106)))
TEST_FP_MORELLO(ldur_2, ldur(s0, MemOperand(x1, 163)))
TEST_FP_MORELLO(str_0, str(d0, MemOperand(x1, w2, SXTW, 3)))
TEST_FP_MORELLO(str_1, str(d0, MemOperand(x1, x2, LSL, 0)))
TEST_FP_MORELLO(str_2, str(s0, MemOperand(x1, w2, UXTW, 0)))
TEST_FP_MORELLO(str_3, str(s0, MemOperand(x1, x2, SXTX, 0)))
TEST_FP_MORELLO(stur_0, stur(d0, MemOperand(x1, 133)))
TEST_FP_MORELLO(stur_1, stur(h0, MemOperand(x1, -27)))
TEST_FP_MORELLO(stur_2, stur(s0, MemOperand(x1, -148)))

#define TEST_NEON_MORELLO(NAME, ASM)                                        \
  TEST_TEMPLATE_C64(CPUFeatures(CPUFeatures::kNEON, CPUFeatures::kMorello), \
                    NEON_Morello_##NAME,                                    \
                    ASM)

TEST_NEON_MORELLO(ldr_0, ldr(d0, MemOperand(x1, w2, SXTW, 3)))
TEST_NEON_MORELLO(ldr_1, ldr(d0, MemOperand(x1, x2, SXTX, 3)))
TEST_NEON_MORELLO(ldr_2, ldr(s0, MemOperand(x1, w2, SXTW, 0)))
TEST_NEON_MORELLO(ldr_3, ldr(s0, MemOperand(x1, x2, SXTX, 2)))
TEST_NEON_MORELLO(ldur_0, ldur(b0, MemOperand(x1, -48)))
TEST_NEON_MORELLO(ldur_1, ldur(d0, MemOperand(x1, -167)))
TEST_NEON_MORELLO(ldur_2, ldur(h0, MemOperand(x1, 106)))
TEST_NEON_MORELLO(ldur_3, ldur(q0, MemOperand(x1, -145)))
TEST_NEON_MORELLO(ldur_4, ldur(s0, MemOperand(x1, 163)))
TEST_NEON_MORELLO(str_0, str(d0, MemOperand(x1, w2, SXTW, 3)))
TEST_NEON_MORELLO(str_1, str(d0, MemOperand(x1, x2, LSL, 0)))
TEST_NEON_MORELLO(str_2, str(s0, MemOperand(x1, w2, UXTW, 0)))
TEST_NEON_MORELLO(str_3, str(s0, MemOperand(x1, x2, SXTX, 0)))
TEST_NEON_MORELLO(stur_0, stur(b0, MemOperand(x1, -101)))
TEST_NEON_MORELLO(stur_1, stur(d0, MemOperand(x1, 133)))
TEST_NEON_MORELLO(stur_2, stur(h0, MemOperand(x1, -27)))
TEST_NEON_MORELLO(stur_3, stur(q0, MemOperand(x1, 35)))
TEST_NEON_MORELLO(stur_4, stur(s0, MemOperand(x1, -148)))

#define TEST_RCPC_MORELLO(NAME, ASM)                                        \
  TEST_TEMPLATE_C64(CPUFeatures(CPUFeatures::kRCpc, CPUFeatures::kMorello), \
                    RCpc_Morello_##NAME,                                    \
                    ASM)

TEST_RCPC_MORELLO(ldapr_0, ldapr(c0, MemOperand(c1)))

#define TEST_ATOMICS_MORELLO(NAME, ASM)                                        \
  TEST_TEMPLATE_C64(CPUFeatures(CPUFeatures::kAtomics, CPUFeatures::kMorello), \
                    Atomics_Morello_##NAME,                                    \
                    ASM)

TEST_ATOMICS_MORELLO(cas_0, cas(c0, c1, MemOperand(c2)))
TEST_ATOMICS_MORELLO(casa_0, casa(c0, c1, MemOperand(c2)))
TEST_ATOMICS_MORELLO(casal_0, casal(c0, c1, MemOperand(c2)))
TEST_ATOMICS_MORELLO(casl_0, casl(c0, c1, MemOperand(c2)))
TEST_ATOMICS_MORELLO(swp_0, swp(c0, c1, MemOperand(c2)))
TEST_ATOMICS_MORELLO(swpa_0, swpa(c0, c1, MemOperand(c2)))
TEST_ATOMICS_MORELLO(swpal_0, swpal(c0, c1, MemOperand(c2)))
TEST_ATOMICS_MORELLO(swpl_0, swpl(c0, c1, MemOperand(c2)))

}  // namespace aarch64
}  // namespace vixl
