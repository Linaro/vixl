// Copyright 2019, VIXL authors
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

#ifndef VIXL_AARCH64_DECODER_AARCH64_H_
#define VIXL_AARCH64_DECODER_AARCH64_H_

#include <list>
#include <map>
#include <string>

#include "../globals-vixl.h"

#include "instructions-aarch64.h"

// List macro containing all visitors needed by the decoder class.
#define SVE_INSTRUCTION_VISITOR_LIST(V) \
  V(abs_z_p_z)                          \
  V(adclb_z_zzz)                        \
  V(adclt_z_zzz)                        \
  V(addhnb_z_zz)                        \
  V(addhnt_z_zz)                        \
  V(addpl_r_ri)                         \
  V(addp_z_p_zz)                        \
  V(addvl_r_ri)                         \
  V(add_z_p_zz)                         \
  V(add_z_zi)                           \
  V(add_z_zz)                           \
  V(adr_z_az_d_s32_scaled)              \
  V(adr_z_az_d_u32_scaled)              \
  V(adr_z_az_sd_same_scaled)            \
  V(aesd_z_zz)                          \
  V(aese_z_zz)                          \
  V(aesimc_z_z)                         \
  V(aesmc_z_z)                          \
  V(and_p_p_pp_z)                       \
  V(ands_p_p_pp_z)                      \
  V(andv_r_p_z)                         \
  V(and_z_p_zz)                         \
  V(and_z_zi)                           \
  V(and_z_zz)                           \
  V(asrd_z_p_zi)                        \
  V(asrr_z_p_zz)                        \
  V(asr_z_p_zi)                         \
  V(asr_z_p_zw)                         \
  V(asr_z_p_zz)                         \
  V(asr_z_zi)                           \
  V(asr_z_zw)                           \
  V(bcax_z_zzz)                         \
  V(bdep_z_zz)                          \
  V(bext_z_zz)                          \
  V(bfcvtnt_z_p_z_s2bf)                 \
  V(bfcvt_z_p_z_s2bf)                   \
  V(bfdot_z_zzz)                        \
  V(bfdot_z_zzzi)                       \
  V(bfmlalb_z_zzz)                      \
  V(bfmlalb_z_zzzi)                     \
  V(bfmlalt_z_zzz)                      \
  V(bfmlalt_z_zzzi)                     \
  V(bfmmla_z_zzz)                       \
  V(bgrp_z_zz)                          \
  V(bic_p_p_pp_z)                       \
  V(bics_p_p_pp_z)                      \
  V(bic_z_p_zz)                         \
  V(bic_z_zz)                           \
  V(brka_p_p_p)                         \
  V(brkas_p_p_p_z)                      \
  V(brkb_p_p_p)                         \
  V(brkbs_p_p_p_z)                      \
  V(brkn_p_p_pp)                        \
  V(brkns_p_p_pp)                       \
  V(brkpa_p_p_pp)                       \
  V(brkpas_p_p_pp)                      \
  V(brkpb_p_p_pp)                       \
  V(brkpbs_p_p_pp)                      \
  V(bsl1n_z_zzz)                        \
  V(bsl2n_z_zzz)                        \
  V(bsl_z_zzz)                          \
  V(cadd_z_zz)                          \
  V(cdot_z_zzz)                         \
  V(cdot_z_zzzi_d)                      \
  V(cdot_z_zzzi_s)                      \
  V(clasta_r_p_z)                       \
  V(clasta_v_p_z)                       \
  V(clasta_z_p_zz)                      \
  V(clastb_r_p_z)                       \
  V(clastb_v_p_z)                       \
  V(clastb_z_p_zz)                      \
  V(cls_z_p_z)                          \
  V(clz_z_p_z)                          \
  V(cmla_z_zzz)                         \
  V(cmla_z_zzzi_h)                      \
  V(cmla_z_zzzi_s)                      \
  V(cmpeq_p_p_zi)                       \
  V(cmpeq_p_p_zw)                       \
  V(cmpeq_p_p_zz)                       \
  V(cmpge_p_p_zi)                       \
  V(cmpge_p_p_zw)                       \
  V(cmpge_p_p_zz)                       \
  V(cmpgt_p_p_zi)                       \
  V(cmpgt_p_p_zw)                       \
  V(cmpgt_p_p_zz)                       \
  V(cmphi_p_p_zi)                       \
  V(cmphi_p_p_zw)                       \
  V(cmphi_p_p_zz)                       \
  V(cmphs_p_p_zi)                       \
  V(cmphs_p_p_zw)                       \
  V(cmphs_p_p_zz)                       \
  V(cmple_p_p_zi)                       \
  V(cmple_p_p_zw)                       \
  V(cmplo_p_p_zi)                       \
  V(cmplo_p_p_zw)                       \
  V(cmpls_p_p_zi)                       \
  V(cmpls_p_p_zw)                       \
  V(cmplt_p_p_zi)                       \
  V(cmplt_p_p_zw)                       \
  V(cmpne_p_p_zi)                       \
  V(cmpne_p_p_zw)                       \
  V(cmpne_p_p_zz)                       \
  V(cnot_z_p_z)                         \
  V(cntb_r_s)                           \
  V(cntd_r_s)                           \
  V(cnth_r_s)                           \
  V(cntp_r_p_p)                         \
  V(cntw_r_s)                           \
  V(cnt_z_p_z)                          \
  V(compact_z_p_z)                      \
  V(cpy_z_o_i)                          \
  V(cpy_z_p_i)                          \
  V(cpy_z_p_r)                          \
  V(cpy_z_p_v)                          \
  V(ctermeq_rr)                         \
  V(ctermne_rr)                         \
  V(decb_r_rs)                          \
  V(decd_r_rs)                          \
  V(decd_z_zs)                          \
  V(dech_r_rs)                          \
  V(dech_z_zs)                          \
  V(decp_r_p_r)                         \
  V(decp_z_p_z)                         \
  V(decw_r_rs)                          \
  V(decw_z_zs)                          \
  V(dupm_z_i)                           \
  V(dup_z_i)                            \
  V(dup_z_r)                            \
  V(dup_z_zi)                           \
  V(eor3_z_zzz)                         \
  V(eorbt_z_zz)                         \
  V(eor_p_p_pp_z)                       \
  V(eors_p_p_pp_z)                      \
  V(eortb_z_zz)                         \
  V(eorv_r_p_z)                         \
  V(eor_z_p_zz)                         \
  V(eor_z_zi)                           \
  V(eor_z_zz)                           \
  V(ext_z_zi_con)                       \
  V(ext_z_zi_des)                       \
  V(fabd_z_p_zz)                        \
  V(fabs_z_p_z)                         \
  V(facge_p_p_zz)                       \
  V(facgt_p_p_zz)                       \
  V(fadda_v_p_z)                        \
  V(faddp_z_p_zz)                       \
  V(faddv_v_p_z)                        \
  V(fadd_z_p_zs)                        \
  V(fadd_z_p_zz)                        \
  V(fadd_z_zz)                          \
  V(fcadd_z_p_zz)                       \
  V(fcmeq_p_p_z0)                       \
  V(fcmeq_p_p_zz)                       \
  V(fcmge_p_p_z0)                       \
  V(fcmge_p_p_zz)                       \
  V(fcmgt_p_p_z0)                       \
  V(fcmgt_p_p_zz)                       \
  V(fcmla_z_p_zzz)                      \
  V(fcmla_z_zzzi_h)                     \
  V(fcmla_z_zzzi_s)                     \
  V(fcmle_p_p_z0)                       \
  V(fcmlt_p_p_z0)                       \
  V(fcmne_p_p_z0)                       \
  V(fcmne_p_p_zz)                       \
  V(fcmuo_p_p_zz)                       \
  V(fcpy_z_p_i)                         \
  V(fcvtlt_z_p_z_h2s)                   \
  V(fcvtlt_z_p_z_s2d)                   \
  V(fcvtnt_z_p_z_d2s)                   \
  V(fcvtnt_z_p_z_s2h)                   \
  V(fcvtxnt_z_p_z_d2s)                  \
  V(fcvtx_z_p_z_d2s)                    \
  V(fcvt_z_p_z_d2h)                     \
  V(fcvt_z_p_z_d2s)                     \
  V(fcvt_z_p_z_h2d)                     \
  V(fcvt_z_p_z_h2s)                     \
  V(fcvt_z_p_z_s2d)                     \
  V(fcvt_z_p_z_s2h)                     \
  V(fcvtzs_z_p_z_d2w)                   \
  V(fcvtzs_z_p_z_d2x)                   \
  V(fcvtzs_z_p_z_fp162h)                \
  V(fcvtzs_z_p_z_fp162w)                \
  V(fcvtzs_z_p_z_fp162x)                \
  V(fcvtzs_z_p_z_s2w)                   \
  V(fcvtzs_z_p_z_s2x)                   \
  V(fcvtzu_z_p_z_d2w)                   \
  V(fcvtzu_z_p_z_d2x)                   \
  V(fcvtzu_z_p_z_fp162h)                \
  V(fcvtzu_z_p_z_fp162w)                \
  V(fcvtzu_z_p_z_fp162x)                \
  V(fcvtzu_z_p_z_s2w)                   \
  V(fcvtzu_z_p_z_s2x)                   \
  V(fdivr_z_p_zz)                       \
  V(fdiv_z_p_zz)                        \
  V(fdup_z_i)                           \
  V(fexpa_z_z)                          \
  V(flogb_z_p_z)                        \
  V(fmad_z_p_zzz)                       \
  V(fmaxnmp_z_p_zz)                     \
  V(fmaxnmv_v_p_z)                      \
  V(fmaxnm_z_p_zs)                      \
  V(fmaxnm_z_p_zz)                      \
  V(fmaxp_z_p_zz)                       \
  V(fmaxv_v_p_z)                        \
  V(fmax_z_p_zs)                        \
  V(fmax_z_p_zz)                        \
  V(fminnmp_z_p_zz)                     \
  V(fminnmv_v_p_z)                      \
  V(fminnm_z_p_zs)                      \
  V(fminnm_z_p_zz)                      \
  V(fminp_z_p_zz)                       \
  V(fminv_v_p_z)                        \
  V(fmin_z_p_zs)                        \
  V(fmin_z_p_zz)                        \
  V(fmlalb_z_zzz)                       \
  V(fmlalb_z_zzzi_s)                    \
  V(fmlalt_z_zzz)                       \
  V(fmlalt_z_zzzi_s)                    \
  V(fmla_z_p_zzz)                       \
  V(fmla_z_zzzi_d)                      \
  V(fmla_z_zzzi_h)                      \
  V(fmla_z_zzzi_s)                      \
  V(fmlslb_z_zzz)                       \
  V(fmlslb_z_zzzi_s)                    \
  V(fmlslt_z_zzz)                       \
  V(fmlslt_z_zzzi_s)                    \
  V(fmls_z_p_zzz)                       \
  V(fmls_z_zzzi_d)                      \
  V(fmls_z_zzzi_h)                      \
  V(fmls_z_zzzi_s)                      \
  V(fmmla_z_zzz_d)                      \
  V(fmmla_z_zzz_s)                      \
  V(fmsb_z_p_zzz)                       \
  V(fmulx_z_p_zz)                       \
  V(fmul_z_p_zs)                        \
  V(fmul_z_p_zz)                        \
  V(fmul_z_zz)                          \
  V(fmul_z_zzi_d)                       \
  V(fmul_z_zzi_h)                       \
  V(fmul_z_zzi_s)                       \
  V(fneg_z_p_z)                         \
  V(fnmad_z_p_zzz)                      \
  V(fnmla_z_p_zzz)                      \
  V(fnmls_z_p_zzz)                      \
  V(fnmsb_z_p_zzz)                      \
  V(frecpe_z_z)                         \
  V(frecps_z_zz)                        \
  V(frecpx_z_p_z)                       \
  V(frinta_z_p_z)                       \
  V(frinti_z_p_z)                       \
  V(frintm_z_p_z)                       \
  V(frintn_z_p_z)                       \
  V(frintp_z_p_z)                       \
  V(frintx_z_p_z)                       \
  V(frintz_z_p_z)                       \
  V(frsqrte_z_z)                        \
  V(frsqrts_z_zz)                       \
  V(fscale_z_p_zz)                      \
  V(fsqrt_z_p_z)                        \
  V(fsubr_z_p_zs)                       \
  V(fsubr_z_p_zz)                       \
  V(fsub_z_p_zs)                        \
  V(fsub_z_p_zz)                        \
  V(fsub_z_zz)                          \
  V(ftmad_z_zzi)                        \
  V(ftsmul_z_zz)                        \
  V(ftssel_z_zz)                        \
  V(histcnt_z_p_zz)                     \
  V(histseg_z_zz)                       \
  V(incb_r_rs)                          \
  V(incd_r_rs)                          \
  V(incd_z_zs)                          \
  V(inch_r_rs)                          \
  V(inch_z_zs)                          \
  V(incp_r_p_r)                         \
  V(incp_z_p_z)                         \
  V(incw_r_rs)                          \
  V(incw_z_zs)                          \
  V(index_z_ii)                         \
  V(index_z_ir)                         \
  V(index_z_ri)                         \
  V(index_z_rr)                         \
  V(insr_z_r)                           \
  V(insr_z_v)                           \
  V(lasta_r_p_z)                        \
  V(lasta_v_p_z)                        \
  V(lastb_r_p_z)                        \
  V(lastb_v_p_z)                        \
  V(ld1b_z_p_ai_d)                      \
  V(ld1b_z_p_ai_s)                      \
  V(ld1b_z_p_bi_u16)                    \
  V(ld1b_z_p_bi_u32)                    \
  V(ld1b_z_p_bi_u64)                    \
  V(ld1b_z_p_bi_u8)                     \
  V(ld1b_z_p_br_u16)                    \
  V(ld1b_z_p_br_u32)                    \
  V(ld1b_z_p_br_u64)                    \
  V(ld1b_z_p_br_u8)                     \
  V(ld1b_z_p_bz_d_64_unscaled)          \
  V(ld1b_z_p_bz_d_x32_unscaled)         \
  V(ld1b_z_p_bz_s_x32_unscaled)         \
  V(ld1d_z_p_ai_d)                      \
  V(ld1d_z_p_bi_u64)                    \
  V(ld1d_z_p_br_u64)                    \
  V(ld1d_z_p_bz_d_64_scaled)            \
  V(ld1d_z_p_bz_d_64_unscaled)          \
  V(ld1d_z_p_bz_d_x32_scaled)           \
  V(ld1d_z_p_bz_d_x32_unscaled)         \
  V(ld1h_z_p_ai_d)                      \
  V(ld1h_z_p_ai_s)                      \
  V(ld1h_z_p_bi_u16)                    \
  V(ld1h_z_p_bi_u32)                    \
  V(ld1h_z_p_bi_u64)                    \
  V(ld1h_z_p_br_u16)                    \
  V(ld1h_z_p_br_u32)                    \
  V(ld1h_z_p_br_u64)                    \
  V(ld1h_z_p_bz_d_64_scaled)            \
  V(ld1h_z_p_bz_d_64_unscaled)          \
  V(ld1h_z_p_bz_d_x32_scaled)           \
  V(ld1h_z_p_bz_d_x32_unscaled)         \
  V(ld1h_z_p_bz_s_x32_scaled)           \
  V(ld1h_z_p_bz_s_x32_unscaled)         \
  V(ld1rb_z_p_bi_u16)                   \
  V(ld1rb_z_p_bi_u32)                   \
  V(ld1rb_z_p_bi_u64)                   \
  V(ld1rb_z_p_bi_u8)                    \
  V(ld1rd_z_p_bi_u64)                   \
  V(ld1rh_z_p_bi_u16)                   \
  V(ld1rh_z_p_bi_u32)                   \
  V(ld1rh_z_p_bi_u64)                   \
  V(ld1rob_z_p_bi_u8)                   \
  V(ld1rob_z_p_br_contiguous)           \
  V(ld1rod_z_p_bi_u64)                  \
  V(ld1rod_z_p_br_contiguous)           \
  V(ld1roh_z_p_bi_u16)                  \
  V(ld1roh_z_p_br_contiguous)           \
  V(ld1row_z_p_bi_u32)                  \
  V(ld1row_z_p_br_contiguous)           \
  V(ld1rqb_z_p_bi_u8)                   \
  V(ld1rqb_z_p_br_contiguous)           \
  V(ld1rqd_z_p_bi_u64)                  \
  V(ld1rqd_z_p_br_contiguous)           \
  V(ld1rqh_z_p_bi_u16)                  \
  V(ld1rqh_z_p_br_contiguous)           \
  V(ld1rqw_z_p_bi_u32)                  \
  V(ld1rqw_z_p_br_contiguous)           \
  V(ld1rsb_z_p_bi_s16)                  \
  V(ld1rsb_z_p_bi_s32)                  \
  V(ld1rsb_z_p_bi_s64)                  \
  V(ld1rsh_z_p_bi_s32)                  \
  V(ld1rsh_z_p_bi_s64)                  \
  V(ld1rsw_z_p_bi_s64)                  \
  V(ld1rw_z_p_bi_u32)                   \
  V(ld1rw_z_p_bi_u64)                   \
  V(ld1sb_z_p_ai_d)                     \
  V(ld1sb_z_p_ai_s)                     \
  V(ld1sb_z_p_bi_s16)                   \
  V(ld1sb_z_p_bi_s32)                   \
  V(ld1sb_z_p_bi_s64)                   \
  V(ld1sb_z_p_br_s16)                   \
  V(ld1sb_z_p_br_s32)                   \
  V(ld1sb_z_p_br_s64)                   \
  V(ld1sb_z_p_bz_d_64_unscaled)         \
  V(ld1sb_z_p_bz_d_x32_unscaled)        \
  V(ld1sb_z_p_bz_s_x32_unscaled)        \
  V(ld1sh_z_p_ai_d)                     \
  V(ld1sh_z_p_ai_s)                     \
  V(ld1sh_z_p_bi_s32)                   \
  V(ld1sh_z_p_bi_s64)                   \
  V(ld1sh_z_p_br_s32)                   \
  V(ld1sh_z_p_br_s64)                   \
  V(ld1sh_z_p_bz_d_64_scaled)           \
  V(ld1sh_z_p_bz_d_64_unscaled)         \
  V(ld1sh_z_p_bz_d_x32_scaled)          \
  V(ld1sh_z_p_bz_d_x32_unscaled)        \
  V(ld1sh_z_p_bz_s_x32_scaled)          \
  V(ld1sh_z_p_bz_s_x32_unscaled)        \
  V(ld1sw_z_p_ai_d)                     \
  V(ld1sw_z_p_bi_s64)                   \
  V(ld1sw_z_p_br_s64)                   \
  V(ld1sw_z_p_bz_d_64_scaled)           \
  V(ld1sw_z_p_bz_d_64_unscaled)         \
  V(ld1sw_z_p_bz_d_x32_scaled)          \
  V(ld1sw_z_p_bz_d_x32_unscaled)        \
  V(ld1w_z_p_ai_d)                      \
  V(ld1w_z_p_ai_s)                      \
  V(ld1w_z_p_bi_u32)                    \
  V(ld1w_z_p_bi_u64)                    \
  V(ld1w_z_p_br_u32)                    \
  V(ld1w_z_p_br_u64)                    \
  V(ld1w_z_p_bz_d_64_scaled)            \
  V(ld1w_z_p_bz_d_64_unscaled)          \
  V(ld1w_z_p_bz_d_x32_scaled)           \
  V(ld1w_z_p_bz_d_x32_unscaled)         \
  V(ld1w_z_p_bz_s_x32_scaled)           \
  V(ld1w_z_p_bz_s_x32_unscaled)         \
  V(ld2b_z_p_bi_contiguous)             \
  V(ld2b_z_p_br_contiguous)             \
  V(ld2d_z_p_bi_contiguous)             \
  V(ld2d_z_p_br_contiguous)             \
  V(ld2h_z_p_bi_contiguous)             \
  V(ld2h_z_p_br_contiguous)             \
  V(ld2w_z_p_bi_contiguous)             \
  V(ld2w_z_p_br_contiguous)             \
  V(ld3b_z_p_bi_contiguous)             \
  V(ld3b_z_p_br_contiguous)             \
  V(ld3d_z_p_bi_contiguous)             \
  V(ld3d_z_p_br_contiguous)             \
  V(ld3h_z_p_bi_contiguous)             \
  V(ld3h_z_p_br_contiguous)             \
  V(ld3w_z_p_bi_contiguous)             \
  V(ld3w_z_p_br_contiguous)             \
  V(ld4b_z_p_bi_contiguous)             \
  V(ld4b_z_p_br_contiguous)             \
  V(ld4d_z_p_bi_contiguous)             \
  V(ld4d_z_p_br_contiguous)             \
  V(ld4h_z_p_bi_contiguous)             \
  V(ld4h_z_p_br_contiguous)             \
  V(ld4w_z_p_bi_contiguous)             \
  V(ld4w_z_p_br_contiguous)             \
  V(ldff1b_z_p_ai_d)                    \
  V(ldff1b_z_p_ai_s)                    \
  V(ldff1b_z_p_br_u16)                  \
  V(ldff1b_z_p_br_u32)                  \
  V(ldff1b_z_p_br_u64)                  \
  V(ldff1b_z_p_br_u8)                   \
  V(ldff1b_z_p_bz_d_64_unscaled)        \
  V(ldff1b_z_p_bz_d_x32_unscaled)       \
  V(ldff1b_z_p_bz_s_x32_unscaled)       \
  V(ldff1d_z_p_ai_d)                    \
  V(ldff1d_z_p_br_u64)                  \
  V(ldff1d_z_p_bz_d_64_scaled)          \
  V(ldff1d_z_p_bz_d_64_unscaled)        \
  V(ldff1d_z_p_bz_d_x32_scaled)         \
  V(ldff1d_z_p_bz_d_x32_unscaled)       \
  V(ldff1h_z_p_ai_d)                    \
  V(ldff1h_z_p_ai_s)                    \
  V(ldff1h_z_p_br_u16)                  \
  V(ldff1h_z_p_br_u32)                  \
  V(ldff1h_z_p_br_u64)                  \
  V(ldff1h_z_p_bz_d_64_scaled)          \
  V(ldff1h_z_p_bz_d_64_unscaled)        \
  V(ldff1h_z_p_bz_d_x32_scaled)         \
  V(ldff1h_z_p_bz_d_x32_unscaled)       \
  V(ldff1h_z_p_bz_s_x32_scaled)         \
  V(ldff1h_z_p_bz_s_x32_unscaled)       \
  V(ldff1sb_z_p_ai_d)                   \
  V(ldff1sb_z_p_ai_s)                   \
  V(ldff1sb_z_p_br_s16)                 \
  V(ldff1sb_z_p_br_s32)                 \
  V(ldff1sb_z_p_br_s64)                 \
  V(ldff1sb_z_p_bz_d_64_unscaled)       \
  V(ldff1sb_z_p_bz_d_x32_unscaled)      \
  V(ldff1sb_z_p_bz_s_x32_unscaled)      \
  V(ldff1sh_z_p_ai_d)                   \
  V(ldff1sh_z_p_ai_s)                   \
  V(ldff1sh_z_p_br_s32)                 \
  V(ldff1sh_z_p_br_s64)                 \
  V(ldff1sh_z_p_bz_d_64_scaled)         \
  V(ldff1sh_z_p_bz_d_64_unscaled)       \
  V(ldff1sh_z_p_bz_d_x32_scaled)        \
  V(ldff1sh_z_p_bz_d_x32_unscaled)      \
  V(ldff1sh_z_p_bz_s_x32_scaled)        \
  V(ldff1sh_z_p_bz_s_x32_unscaled)      \
  V(ldff1sw_z_p_ai_d)                   \
  V(ldff1sw_z_p_br_s64)                 \
  V(ldff1sw_z_p_bz_d_64_scaled)         \
  V(ldff1sw_z_p_bz_d_64_unscaled)       \
  V(ldff1sw_z_p_bz_d_x32_scaled)        \
  V(ldff1sw_z_p_bz_d_x32_unscaled)      \
  V(ldff1w_z_p_ai_d)                    \
  V(ldff1w_z_p_ai_s)                    \
  V(ldff1w_z_p_br_u32)                  \
  V(ldff1w_z_p_br_u64)                  \
  V(ldff1w_z_p_bz_d_64_scaled)          \
  V(ldff1w_z_p_bz_d_64_unscaled)        \
  V(ldff1w_z_p_bz_d_x32_scaled)         \
  V(ldff1w_z_p_bz_d_x32_unscaled)       \
  V(ldff1w_z_p_bz_s_x32_scaled)         \
  V(ldff1w_z_p_bz_s_x32_unscaled)       \
  V(ldnf1b_z_p_bi_u16)                  \
  V(ldnf1b_z_p_bi_u32)                  \
  V(ldnf1b_z_p_bi_u64)                  \
  V(ldnf1b_z_p_bi_u8)                   \
  V(ldnf1d_z_p_bi_u64)                  \
  V(ldnf1h_z_p_bi_u16)                  \
  V(ldnf1h_z_p_bi_u32)                  \
  V(ldnf1h_z_p_bi_u64)                  \
  V(ldnf1sb_z_p_bi_s16)                 \
  V(ldnf1sb_z_p_bi_s32)                 \
  V(ldnf1sb_z_p_bi_s64)                 \
  V(ldnf1sh_z_p_bi_s32)                 \
  V(ldnf1sh_z_p_bi_s64)                 \
  V(ldnf1sw_z_p_bi_s64)                 \
  V(ldnf1w_z_p_bi_u32)                  \
  V(ldnf1w_z_p_bi_u64)                  \
  V(ldnt1b_z_p_ar_d_64_unscaled)        \
  V(ldnt1b_z_p_ar_s_x32_unscaled)       \
  V(ldnt1b_z_p_bi_contiguous)           \
  V(ldnt1b_z_p_br_contiguous)           \
  V(ldnt1d_z_p_ar_d_64_unscaled)        \
  V(ldnt1d_z_p_bi_contiguous)           \
  V(ldnt1d_z_p_br_contiguous)           \
  V(ldnt1h_z_p_ar_d_64_unscaled)        \
  V(ldnt1h_z_p_ar_s_x32_unscaled)       \
  V(ldnt1h_z_p_bi_contiguous)           \
  V(ldnt1h_z_p_br_contiguous)           \
  V(ldnt1sb_z_p_ar_d_64_unscaled)       \
  V(ldnt1sb_z_p_ar_s_x32_unscaled)      \
  V(ldnt1sh_z_p_ar_d_64_unscaled)       \
  V(ldnt1sh_z_p_ar_s_x32_unscaled)      \
  V(ldnt1sw_z_p_ar_d_64_unscaled)       \
  V(ldnt1w_z_p_ar_d_64_unscaled)        \
  V(ldnt1w_z_p_ar_s_x32_unscaled)       \
  V(ldnt1w_z_p_bi_contiguous)           \
  V(ldnt1w_z_p_br_contiguous)           \
  V(ldr_p_bi)                           \
  V(ldr_z_bi)                           \
  V(lslr_z_p_zz)                        \
  V(lsl_z_p_zi)                         \
  V(lsl_z_p_zw)                         \
  V(lsl_z_p_zz)                         \
  V(lsl_z_zi)                           \
  V(lsl_z_zw)                           \
  V(lsrr_z_p_zz)                        \
  V(lsr_z_p_zi)                         \
  V(lsr_z_p_zw)                         \
  V(lsr_z_p_zz)                         \
  V(lsr_z_zi)                           \
  V(lsr_z_zw)                           \
  V(mad_z_p_zzz)                        \
  V(match_p_p_zz)                       \
  V(mla_z_p_zzz)                        \
  V(mla_z_zzzi_d)                       \
  V(mla_z_zzzi_h)                       \
  V(mla_z_zzzi_s)                       \
  V(mls_z_p_zzz)                        \
  V(mls_z_zzzi_d)                       \
  V(mls_z_zzzi_h)                       \
  V(mls_z_zzzi_s)                       \
  V(movprfx_z_p_z)                      \
  V(movprfx_z_z)                        \
  V(msb_z_p_zzz)                        \
  V(mul_z_p_zz)                         \
  V(mul_z_zi)                           \
  V(mul_z_zz)                           \
  V(mul_z_zzi_d)                        \
  V(mul_z_zzi_h)                        \
  V(mul_z_zzi_s)                        \
  V(nand_p_p_pp_z)                      \
  V(nands_p_p_pp_z)                     \
  V(nbsl_z_zzz)                         \
  V(neg_z_p_z)                          \
  V(nmatch_p_p_zz)                      \
  V(nor_p_p_pp_z)                       \
  V(nors_p_p_pp_z)                      \
  V(not_z_p_z)                          \
  V(orn_p_p_pp_z)                       \
  V(orns_p_p_pp_z)                      \
  V(orr_p_p_pp_z)                       \
  V(orrs_p_p_pp_z)                      \
  V(orr_z_p_zz)                         \
  V(orr_z_zi)                           \
  V(orr_z_zz)                           \
  V(orv_r_p_z)                          \
  V(pfalse_p)                           \
  V(pfirst_p_p_p)                       \
  V(pmullb_z_zz)                        \
  V(pmullt_z_zz)                        \
  V(pmul_z_zz)                          \
  V(pnext_p_p_p)                        \
  V(prfb_i_p_ai_d)                      \
  V(prfb_i_p_ai_s)                      \
  V(prfb_i_p_bi_s)                      \
  V(prfb_i_p_br_s)                      \
  V(prfb_i_p_bz_d_64_scaled)            \
  V(prfb_i_p_bz_d_x32_scaled)           \
  V(prfb_i_p_bz_s_x32_scaled)           \
  V(prfd_i_p_ai_d)                      \
  V(prfd_i_p_ai_s)                      \
  V(prfd_i_p_bi_s)                      \
  V(prfd_i_p_br_s)                      \
  V(prfd_i_p_bz_d_64_scaled)            \
  V(prfd_i_p_bz_d_x32_scaled)           \
  V(prfd_i_p_bz_s_x32_scaled)           \
  V(prfh_i_p_ai_d)                      \
  V(prfh_i_p_ai_s)                      \
  V(prfh_i_p_bi_s)                      \
  V(prfh_i_p_br_s)                      \
  V(prfh_i_p_bz_d_64_scaled)            \
  V(prfh_i_p_bz_d_x32_scaled)           \
  V(prfh_i_p_bz_s_x32_scaled)           \
  V(prfw_i_p_ai_d)                      \
  V(prfw_i_p_ai_s)                      \
  V(prfw_i_p_bi_s)                      \
  V(prfw_i_p_br_s)                      \
  V(prfw_i_p_bz_d_64_scaled)            \
  V(prfw_i_p_bz_d_x32_scaled)           \
  V(prfw_i_p_bz_s_x32_scaled)           \
  V(ptest_p_p)                          \
  V(ptrue_p_s)                          \
  V(ptrues_p_s)                         \
  V(punpkhi_p_p)                        \
  V(punpklo_p_p)                        \
  V(raddhnb_z_zz)                       \
  V(raddhnt_z_zz)                       \
  V(rax1_z_zz)                          \
  V(rbit_z_p_z)                         \
  V(rdffr_p_f)                          \
  V(rdffr_p_p_f)                        \
  V(rdffrs_p_p_f)                       \
  V(rdvl_r_i)                           \
  V(revb_z_z)                           \
  V(revh_z_z)                           \
  V(rev_p_p)                            \
  V(revw_z_z)                           \
  V(rev_z_z)                            \
  V(rshrnb_z_zi)                        \
  V(rshrnt_z_zi)                        \
  V(rsubhnb_z_zz)                       \
  V(rsubhnt_z_zz)                       \
  V(sabalb_z_zzz)                       \
  V(sabalt_z_zzz)                       \
  V(saba_z_zzz)                         \
  V(sabdlb_z_zz)                        \
  V(sabdlt_z_zz)                        \
  V(sabd_z_p_zz)                        \
  V(sadalp_z_p_z)                       \
  V(saddlbt_z_zz)                       \
  V(saddlb_z_zz)                        \
  V(saddlt_z_zz)                        \
  V(saddv_r_p_z)                        \
  V(saddwb_z_zz)                        \
  V(saddwt_z_zz)                        \
  V(sbclb_z_zzz)                        \
  V(sbclt_z_zzz)                        \
  V(scvtf_z_p_z_h2fp16)                 \
  V(scvtf_z_p_z_w2d)                    \
  V(scvtf_z_p_z_w2fp16)                 \
  V(scvtf_z_p_z_w2s)                    \
  V(scvtf_z_p_z_x2d)                    \
  V(scvtf_z_p_z_x2fp16)                 \
  V(scvtf_z_p_z_x2s)                    \
  V(sdivr_z_p_zz)                       \
  V(sdiv_z_p_zz)                        \
  V(sdot_z_zzz)                         \
  V(sdot_z_zzzi_d)                      \
  V(sdot_z_zzzi_s)                      \
  V(sel_p_p_pp)                         \
  V(sel_z_p_zz)                         \
  V(setffr_f)                           \
  V(shadd_z_p_zz)                       \
  V(shrnb_z_zi)                         \
  V(shrnt_z_zi)                         \
  V(shsubr_z_p_zz)                      \
  V(shsub_z_p_zz)                       \
  V(sli_z_zzi)                          \
  V(sm4ekey_z_zz)                       \
  V(sm4e_z_zz)                          \
  V(smaxp_z_p_zz)                       \
  V(smaxv_r_p_z)                        \
  V(smax_z_p_zz)                        \
  V(smax_z_zi)                          \
  V(sminp_z_p_zz)                       \
  V(sminv_r_p_z)                        \
  V(smin_z_p_zz)                        \
  V(smin_z_zi)                          \
  V(smlalb_z_zzz)                       \
  V(smlalb_z_zzzi_d)                    \
  V(smlalb_z_zzzi_s)                    \
  V(smlalt_z_zzz)                       \
  V(smlalt_z_zzzi_d)                    \
  V(smlalt_z_zzzi_s)                    \
  V(smlslb_z_zzz)                       \
  V(smlslb_z_zzzi_d)                    \
  V(smlslb_z_zzzi_s)                    \
  V(smlslt_z_zzz)                       \
  V(smlslt_z_zzzi_d)                    \
  V(smlslt_z_zzzi_s)                    \
  V(smmla_z_zzz)                        \
  V(smulh_z_p_zz)                       \
  V(smulh_z_zz)                         \
  V(smullb_z_zz)                        \
  V(smullb_z_zzi_d)                     \
  V(smullb_z_zzi_s)                     \
  V(smullt_z_zz)                        \
  V(smullt_z_zzi_d)                     \
  V(smullt_z_zzi_s)                     \
  V(splice_z_p_zz_con)                  \
  V(splice_z_p_zz_des)                  \
  V(sqabs_z_p_z)                        \
  V(sqadd_z_p_zz)                       \
  V(sqadd_z_zi)                         \
  V(sqadd_z_zz)                         \
  V(sqcadd_z_zz)                        \
  V(sqdecb_r_rs_sx)                     \
  V(sqdecb_r_rs_x)                      \
  V(sqdecd_r_rs_sx)                     \
  V(sqdecd_r_rs_x)                      \
  V(sqdecd_z_zs)                        \
  V(sqdech_r_rs_sx)                     \
  V(sqdech_r_rs_x)                      \
  V(sqdech_z_zs)                        \
  V(sqdecp_r_p_r_sx)                    \
  V(sqdecp_r_p_r_x)                     \
  V(sqdecp_z_p_z)                       \
  V(sqdecw_r_rs_sx)                     \
  V(sqdecw_r_rs_x)                      \
  V(sqdecw_z_zs)                        \
  V(sqdmlalbt_z_zzz)                    \
  V(sqdmlalb_z_zzz)                     \
  V(sqdmlalb_z_zzzi_d)                  \
  V(sqdmlalb_z_zzzi_s)                  \
  V(sqdmlalt_z_zzz)                     \
  V(sqdmlalt_z_zzzi_d)                  \
  V(sqdmlalt_z_zzzi_s)                  \
  V(sqdmlslbt_z_zzz)                    \
  V(sqdmlslb_z_zzz)                     \
  V(sqdmlslb_z_zzzi_d)                  \
  V(sqdmlslb_z_zzzi_s)                  \
  V(sqdmlslt_z_zzz)                     \
  V(sqdmlslt_z_zzzi_d)                  \
  V(sqdmlslt_z_zzzi_s)                  \
  V(sqdmulh_z_zz)                       \
  V(sqdmulh_z_zzi_d)                    \
  V(sqdmulh_z_zzi_h)                    \
  V(sqdmulh_z_zzi_s)                    \
  V(sqdmullb_z_zz)                      \
  V(sqdmullb_z_zzi_d)                   \
  V(sqdmullb_z_zzi_s)                   \
  V(sqdmullt_z_zz)                      \
  V(sqdmullt_z_zzi_d)                   \
  V(sqdmullt_z_zzi_s)                   \
  V(sqincb_r_rs_sx)                     \
  V(sqincb_r_rs_x)                      \
  V(sqincd_r_rs_sx)                     \
  V(sqincd_r_rs_x)                      \
  V(sqincd_z_zs)                        \
  V(sqinch_r_rs_sx)                     \
  V(sqinch_r_rs_x)                      \
  V(sqinch_z_zs)                        \
  V(sqincp_r_p_r_sx)                    \
  V(sqincp_r_p_r_x)                     \
  V(sqincp_z_p_z)                       \
  V(sqincw_r_rs_sx)                     \
  V(sqincw_r_rs_x)                      \
  V(sqincw_z_zs)                        \
  V(sqneg_z_p_z)                        \
  V(sqrdcmlah_z_zzz)                    \
  V(sqrdcmlah_z_zzzi_h)                 \
  V(sqrdcmlah_z_zzzi_s)                 \
  V(sqrdmlah_z_zzz)                     \
  V(sqrdmlah_z_zzzi_d)                  \
  V(sqrdmlah_z_zzzi_h)                  \
  V(sqrdmlah_z_zzzi_s)                  \
  V(sqrdmlsh_z_zzz)                     \
  V(sqrdmlsh_z_zzzi_d)                  \
  V(sqrdmlsh_z_zzzi_h)                  \
  V(sqrdmlsh_z_zzzi_s)                  \
  V(sqrdmulh_z_zz)                      \
  V(sqrdmulh_z_zzi_d)                   \
  V(sqrdmulh_z_zzi_h)                   \
  V(sqrdmulh_z_zzi_s)                   \
  V(sqrshlr_z_p_zz)                     \
  V(sqrshl_z_p_zz)                      \
  V(sqrshrnb_z_zi)                      \
  V(sqrshrnt_z_zi)                      \
  V(sqrshrunb_z_zi)                     \
  V(sqrshrunt_z_zi)                     \
  V(sqshlr_z_p_zz)                      \
  V(sqshlu_z_p_zi)                      \
  V(sqshl_z_p_zi)                       \
  V(sqshl_z_p_zz)                       \
  V(sqshrnb_z_zi)                       \
  V(sqshrnt_z_zi)                       \
  V(sqshrunb_z_zi)                      \
  V(sqshrunt_z_zi)                      \
  V(sqsubr_z_p_zz)                      \
  V(sqsub_z_p_zz)                       \
  V(sqsub_z_zi)                         \
  V(sqsub_z_zz)                         \
  V(sqxtnb_z_zz)                        \
  V(sqxtnt_z_zz)                        \
  V(sqxtunb_z_zz)                       \
  V(sqxtunt_z_zz)                       \
  V(srhadd_z_p_zz)                      \
  V(sri_z_zzi)                          \
  V(srshlr_z_p_zz)                      \
  V(srshl_z_p_zz)                       \
  V(srshr_z_p_zi)                       \
  V(srsra_z_zi)                         \
  V(sshllb_z_zi)                        \
  V(sshllt_z_zi)                        \
  V(ssra_z_zi)                          \
  V(ssublbt_z_zz)                       \
  V(ssublb_z_zz)                        \
  V(ssubltb_z_zz)                       \
  V(ssublt_z_zz)                        \
  V(ssubwb_z_zz)                        \
  V(ssubwt_z_zz)                        \
  V(st1b_z_p_ai_d)                      \
  V(st1b_z_p_ai_s)                      \
  V(st1b_z_p_bi)                        \
  V(st1b_z_p_br)                        \
  V(st1b_z_p_bz_d_64_unscaled)          \
  V(st1b_z_p_bz_d_x32_unscaled)         \
  V(st1b_z_p_bz_s_x32_unscaled)         \
  V(st1d_z_p_ai_d)                      \
  V(st1d_z_p_bi)                        \
  V(st1d_z_p_br)                        \
  V(st1d_z_p_bz_d_64_scaled)            \
  V(st1d_z_p_bz_d_64_unscaled)          \
  V(st1d_z_p_bz_d_x32_scaled)           \
  V(st1d_z_p_bz_d_x32_unscaled)         \
  V(st1h_z_p_ai_d)                      \
  V(st1h_z_p_ai_s)                      \
  V(st1h_z_p_bi)                        \
  V(st1h_z_p_br)                        \
  V(st1h_z_p_bz_d_64_scaled)            \
  V(st1h_z_p_bz_d_64_unscaled)          \
  V(st1h_z_p_bz_d_x32_scaled)           \
  V(st1h_z_p_bz_d_x32_unscaled)         \
  V(st1h_z_p_bz_s_x32_scaled)           \
  V(st1h_z_p_bz_s_x32_unscaled)         \
  V(st1w_z_p_ai_d)                      \
  V(st1w_z_p_ai_s)                      \
  V(st1w_z_p_bi)                        \
  V(st1w_z_p_br)                        \
  V(st1w_z_p_bz_d_64_scaled)            \
  V(st1w_z_p_bz_d_64_unscaled)          \
  V(st1w_z_p_bz_d_x32_scaled)           \
  V(st1w_z_p_bz_d_x32_unscaled)         \
  V(st1w_z_p_bz_s_x32_scaled)           \
  V(st1w_z_p_bz_s_x32_unscaled)         \
  V(st2b_z_p_bi_contiguous)             \
  V(st2b_z_p_br_contiguous)             \
  V(st2d_z_p_bi_contiguous)             \
  V(st2d_z_p_br_contiguous)             \
  V(st2h_z_p_bi_contiguous)             \
  V(st2h_z_p_br_contiguous)             \
  V(st2w_z_p_bi_contiguous)             \
  V(st2w_z_p_br_contiguous)             \
  V(st3b_z_p_bi_contiguous)             \
  V(st3b_z_p_br_contiguous)             \
  V(st3d_z_p_bi_contiguous)             \
  V(st3d_z_p_br_contiguous)             \
  V(st3h_z_p_bi_contiguous)             \
  V(st3h_z_p_br_contiguous)             \
  V(st3w_z_p_bi_contiguous)             \
  V(st3w_z_p_br_contiguous)             \
  V(st4b_z_p_bi_contiguous)             \
  V(st4b_z_p_br_contiguous)             \
  V(st4d_z_p_bi_contiguous)             \
  V(st4d_z_p_br_contiguous)             \
  V(st4h_z_p_bi_contiguous)             \
  V(st4h_z_p_br_contiguous)             \
  V(st4w_z_p_bi_contiguous)             \
  V(st4w_z_p_br_contiguous)             \
  V(stnt1b_z_p_ar_d_64_unscaled)        \
  V(stnt1b_z_p_ar_s_x32_unscaled)       \
  V(stnt1b_z_p_bi_contiguous)           \
  V(stnt1b_z_p_br_contiguous)           \
  V(stnt1d_z_p_ar_d_64_unscaled)        \
  V(stnt1d_z_p_bi_contiguous)           \
  V(stnt1d_z_p_br_contiguous)           \
  V(stnt1h_z_p_ar_d_64_unscaled)        \
  V(stnt1h_z_p_ar_s_x32_unscaled)       \
  V(stnt1h_z_p_bi_contiguous)           \
  V(stnt1h_z_p_br_contiguous)           \
  V(stnt1w_z_p_ar_d_64_unscaled)        \
  V(stnt1w_z_p_ar_s_x32_unscaled)       \
  V(stnt1w_z_p_bi_contiguous)           \
  V(stnt1w_z_p_br_contiguous)           \
  V(str_p_bi)                           \
  V(str_z_bi)                           \
  V(subhnb_z_zz)                        \
  V(subhnt_z_zz)                        \
  V(subr_z_p_zz)                        \
  V(subr_z_zi)                          \
  V(sub_z_p_zz)                         \
  V(sub_z_zi)                           \
  V(sub_z_zz)                           \
  V(sudot_z_zzzi_s)                     \
  V(sunpkhi_z_z)                        \
  V(sunpklo_z_z)                        \
  V(suqadd_z_p_zz)                      \
  V(sxtb_z_p_z)                         \
  V(sxth_z_p_z)                         \
  V(sxtw_z_p_z)                         \
  V(tbl_z_zz_1)                         \
  V(tbl_z_zz_2)                         \
  V(tbx_z_zz)                           \
  V(trn1_p_pp)                          \
  V(trn1_z_zz)                          \
  V(trn1_z_zz_q)                        \
  V(trn2_p_pp)                          \
  V(trn2_z_zz)                          \
  V(trn2_z_zz_q)                        \
  V(uabalb_z_zzz)                       \
  V(uabalt_z_zzz)                       \
  V(uaba_z_zzz)                         \
  V(uabdlb_z_zz)                        \
  V(uabdlt_z_zz)                        \
  V(uabd_z_p_zz)                        \
  V(uadalp_z_p_z)                       \
  V(uaddlb_z_zz)                        \
  V(uaddlt_z_zz)                        \
  V(uaddv_r_p_z)                        \
  V(uaddwb_z_zz)                        \
  V(uaddwt_z_zz)                        \
  V(ucvtf_z_p_z_h2fp16)                 \
  V(ucvtf_z_p_z_w2d)                    \
  V(ucvtf_z_p_z_w2fp16)                 \
  V(ucvtf_z_p_z_w2s)                    \
  V(ucvtf_z_p_z_x2d)                    \
  V(ucvtf_z_p_z_x2fp16)                 \
  V(ucvtf_z_p_z_x2s)                    \
  V(udivr_z_p_zz)                       \
  V(udiv_z_p_zz)                        \
  V(udot_z_zzz)                         \
  V(udot_z_zzzi_d)                      \
  V(udot_z_zzzi_s)                      \
  V(uhadd_z_p_zz)                       \
  V(uhsubr_z_p_zz)                      \
  V(uhsub_z_p_zz)                       \
  V(umaxp_z_p_zz)                       \
  V(umaxv_r_p_z)                        \
  V(umax_z_p_zz)                        \
  V(umax_z_zi)                          \
  V(uminp_z_p_zz)                       \
  V(uminv_r_p_z)                        \
  V(umin_z_p_zz)                        \
  V(umin_z_zi)                          \
  V(umlalb_z_zzz)                       \
  V(umlalb_z_zzzi_d)                    \
  V(umlalb_z_zzzi_s)                    \
  V(umlalt_z_zzz)                       \
  V(umlalt_z_zzzi_d)                    \
  V(umlalt_z_zzzi_s)                    \
  V(umlslb_z_zzz)                       \
  V(umlslb_z_zzzi_d)                    \
  V(umlslb_z_zzzi_s)                    \
  V(umlslt_z_zzz)                       \
  V(umlslt_z_zzzi_d)                    \
  V(umlslt_z_zzzi_s)                    \
  V(ummla_z_zzz)                        \
  V(umulh_z_p_zz)                       \
  V(umulh_z_zz)                         \
  V(umullb_z_zz)                        \
  V(umullb_z_zzi_d)                     \
  V(umullb_z_zzi_s)                     \
  V(umullt_z_zz)                        \
  V(umullt_z_zzi_d)                     \
  V(umullt_z_zzi_s)                     \
  V(uqadd_z_p_zz)                       \
  V(uqadd_z_zi)                         \
  V(uqadd_z_zz)                         \
  V(uqdecb_r_rs_uw)                     \
  V(uqdecb_r_rs_x)                      \
  V(uqdecd_r_rs_uw)                     \
  V(uqdecd_r_rs_x)                      \
  V(uqdecd_z_zs)                        \
  V(uqdech_r_rs_uw)                     \
  V(uqdech_r_rs_x)                      \
  V(uqdech_z_zs)                        \
  V(uqdecp_r_p_r_uw)                    \
  V(uqdecp_r_p_r_x)                     \
  V(uqdecp_z_p_z)                       \
  V(uqdecw_r_rs_uw)                     \
  V(uqdecw_r_rs_x)                      \
  V(uqdecw_z_zs)                        \
  V(uqincb_r_rs_uw)                     \
  V(uqincb_r_rs_x)                      \
  V(uqincd_r_rs_uw)                     \
  V(uqincd_r_rs_x)                      \
  V(uqincd_z_zs)                        \
  V(uqinch_r_rs_uw)                     \
  V(uqinch_r_rs_x)                      \
  V(uqinch_z_zs)                        \
  V(uqincp_r_p_r_uw)                    \
  V(uqincp_r_p_r_x)                     \
  V(uqincp_z_p_z)                       \
  V(uqincw_r_rs_uw)                     \
  V(uqincw_r_rs_x)                      \
  V(uqincw_z_zs)                        \
  V(uqrshlr_z_p_zz)                     \
  V(uqrshl_z_p_zz)                      \
  V(uqrshrnb_z_zi)                      \
  V(uqrshrnt_z_zi)                      \
  V(uqshlr_z_p_zz)                      \
  V(uqshl_z_p_zi)                       \
  V(uqshl_z_p_zz)                       \
  V(uqshrnb_z_zi)                       \
  V(uqshrnt_z_zi)                       \
  V(uqsubr_z_p_zz)                      \
  V(uqsub_z_p_zz)                       \
  V(uqsub_z_zi)                         \
  V(uqsub_z_zz)                         \
  V(uqxtnb_z_zz)                        \
  V(uqxtnt_z_zz)                        \
  V(urecpe_z_p_z)                       \
  V(urhadd_z_p_zz)                      \
  V(urshlr_z_p_zz)                      \
  V(urshl_z_p_zz)                       \
  V(urshr_z_p_zi)                       \
  V(ursqrte_z_p_z)                      \
  V(ursra_z_zi)                         \
  V(usdot_z_zzzi_s)                     \
  V(usdot_z_zzz_s)                      \
  V(ushllb_z_zi)                        \
  V(ushllt_z_zi)                        \
  V(usmmla_z_zzz)                       \
  V(usqadd_z_p_zz)                      \
  V(usra_z_zi)                          \
  V(usublb_z_zz)                        \
  V(usublt_z_zz)                        \
  V(usubwb_z_zz)                        \
  V(usubwt_z_zz)                        \
  V(uunpkhi_z_z)                        \
  V(uunpklo_z_z)                        \
  V(uxtb_z_p_z)                         \
  V(uxth_z_p_z)                         \
  V(uxtw_z_p_z)                         \
  V(uzp1_p_pp)                          \
  V(uzp1_z_zz)                          \
  V(uzp1_z_zz_q)                        \
  V(uzp2_p_pp)                          \
  V(uzp2_z_zz)                          \
  V(uzp2_z_zz_q)                        \
  V(whilege_p_p_rr)                     \
  V(whilegt_p_p_rr)                     \
  V(whilehi_p_p_rr)                     \
  V(whilehs_p_p_rr)                     \
  V(whilele_p_p_rr)                     \
  V(whilelo_p_p_rr)                     \
  V(whilels_p_p_rr)                     \
  V(whilelt_p_p_rr)                     \
  V(whilerw_p_rr)                       \
  V(whilewr_p_rr)                       \
  V(wrffr_f_p)                          \
  V(xar_z_zzi)                          \
  V(zip1_p_pp)                          \
  V(zip1_z_zz)                          \
  V(zip1_z_zz_q)                        \
  V(zip2_p_pp)                          \
  V(zip2_z_zz)                          \
  V(zip2_z_zz_q)

#define VISITOR_LIST_THAT_RETURN(V)                              \
  V(AddSubExtended)                                              \
  V(AddSubImmediate)                                             \
  V(AddSubShifted)                                               \
  V(AddSubWithCarry)                                             \
  V(AtomicMemory)                                                \
  V(Bitfield)                                                    \
  V(CompareBranch)                                               \
  V(ConditionalBranch)                                           \
  V(ConditionalCompareImmediate)                                 \
  V(ConditionalCompareRegister)                                  \
  V(ConditionalSelect)                                           \
  V(Crypto2RegSHA)                                               \
  V(Crypto3RegSHA)                                               \
  V(CryptoAES)                                                   \
  V(DataProcessing1Source)                                       \
  V(DataProcessing2Source)                                       \
  V(DataProcessing3Source)                                       \
  V(EvaluateIntoFlags)                                           \
  V(Exception)                                                   \
  V(Extract)                                                     \
  V(FPCompare)                                                   \
  V(FPConditionalCompare)                                        \
  V(FPConditionalSelect)                                         \
  V(FPDataProcessing1Source)                                     \
  V(FPDataProcessing2Source)                                     \
  V(FPDataProcessing3Source)                                     \
  V(FPFixedPointConvert)                                         \
  V(FPImmediate)                                                 \
  V(FPIntegerConvert)                                            \
  V(LoadLiteral)                                                 \
  V(LoadStoreExclusive)                                          \
  V(LoadStorePAC)                                                \
  V(LoadStorePairNonTemporal)                                    \
  V(LoadStorePairOffset)                                         \
  V(LoadStorePairPostIndex)                                      \
  V(LoadStorePairPreIndex)                                       \
  V(LoadStorePostIndex)                                          \
  V(LoadStorePreIndex)                                           \
  V(LoadStoreRCpcUnscaledOffset)                                 \
  V(LoadStoreRegisterOffset)                                     \
  V(LoadStoreUnscaledOffset)                                     \
  V(LoadStoreUnsignedOffset)                                     \
  V(LogicalImmediate)                                            \
  V(LogicalShifted)                                              \
  V(MoveWideImmediate)                                           \
  V(NEON2RegMisc)                                                \
  V(NEON2RegMiscFP16)                                            \
  V(NEON3Different)                                              \
  V(NEON3Same)                                                   \
  V(NEON3SameExtra)                                              \
  V(NEON3SameFP16)                                               \
  V(NEONAcrossLanes)                                             \
  V(NEONByIndexedElement)                                        \
  V(NEONCopy)                                                    \
  V(NEONExtract)                                                 \
  V(NEONLoadStoreMultiStruct)                                    \
  V(NEONLoadStoreMultiStructPostIndex)                           \
  V(NEONLoadStoreSingleStruct)                                   \
  V(NEONLoadStoreSingleStructPostIndex)                          \
  V(NEONModifiedImmediate)                                       \
  V(NEONPerm)                                                    \
  V(NEONScalar2RegMisc)                                          \
  V(NEONScalar2RegMiscFP16)                                      \
  V(NEONScalar3Diff)                                             \
  V(NEONScalar3Same)                                             \
  V(NEONScalar3SameExtra)                                        \
  V(NEONScalar3SameFP16)                                         \
  V(NEONScalarByIndexedElement)                                  \
  V(NEONScalarCopy)                                              \
  V(NEONScalarPairwise)                                          \
  V(NEONScalarShiftImmediate)                                    \
  V(NEONShiftImmediate)                                          \
  V(NEONTable)                                                   \
  V(PCRelAddressing)                                             \
  V(RotateRightIntoFlags)                                        \
  V(SVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets)           \
  V(SVE32BitGatherLoad_VectorPlusImm)                            \
  V(SVE32BitGatherLoadHalfwords_ScalarPlus32BitScaledOffsets)    \
  V(SVE32BitGatherLoadWords_ScalarPlus32BitScaledOffsets)        \
  V(SVE32BitGatherPrefetch_ScalarPlus32BitScaledOffsets)         \
  V(SVE32BitGatherPrefetch_VectorPlusImm)                        \
  V(SVE32BitScatterStore_ScalarPlus32BitScaledOffsets)           \
  V(SVE32BitScatterStore_ScalarPlus32BitUnscaledOffsets)         \
  V(SVE32BitScatterStore_VectorPlusImm)                          \
  V(SVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsets)     \
  V(SVE64BitGatherLoad_ScalarPlus64BitScaledOffsets)             \
  V(SVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets)           \
  V(SVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets)   \
  V(SVE64BitGatherLoad_VectorPlusImm)                            \
  V(SVE64BitGatherPrefetch_ScalarPlus64BitScaledOffsets)         \
  V(SVE64BitGatherPrefetch_ScalarPlusUnpacked32BitScaledOffsets) \
  V(SVE64BitGatherPrefetch_VectorPlusImm)                        \
  V(SVE64BitScatterStore_ScalarPlus64BitScaledOffsets)           \
  V(SVE64BitScatterStore_ScalarPlus64BitUnscaledOffsets)         \
  V(SVE64BitScatterStore_ScalarPlusUnpacked32BitScaledOffsets)   \
  V(SVE64BitScatterStore_ScalarPlusUnpacked32BitUnscaledOffsets) \
  V(SVE64BitScatterStore_VectorPlusImm)                          \
  V(SVEAddressGeneration)                                        \
  V(SVEBitwiseLogicalUnpredicated)                               \
  V(SVEBitwiseShiftUnpredicated)                                 \
  V(SVEFFRInitialise)                                            \
  V(SVEFFRWriteFromPredicate)                                    \
  V(SVEFPAccumulatingReduction)                                  \
  V(SVEFPArithmeticUnpredicated)                                 \
  V(SVEFPCompareVectors)                                         \
  V(SVEFPCompareWithZero)                                        \
  V(SVEFPComplexAddition)                                        \
  V(SVEFPComplexMulAdd)                                          \
  V(SVEFPComplexMulAddIndex)                                     \
  V(SVEFPFastReduction)                                          \
  V(SVEFPMulIndex)                                               \
  V(SVEFPMulAdd)                                                 \
  V(SVEFPMulAddIndex)                                            \
  V(SVEFPUnaryOpUnpredicated)                                    \
  V(SVEIncDecByPredicateCount)                                   \
  V(SVEIndexGeneration)                                          \
  V(SVEIntArithmeticUnpredicated)                                \
  V(SVEIntCompareSignedImm)                                      \
  V(SVEIntCompareUnsignedImm)                                    \
  V(SVEIntCompareVectors)                                        \
  V(SVEIntMulAddPredicated)                                      \
  V(SVEIntMulAddUnpredicated)                                    \
  V(SVEIntReduction)                                             \
  V(SVEIntUnaryArithmeticPredicated)                             \
  V(SVEMovprfx)                                                  \
  V(SVEMulIndex)                                                 \
  V(SVEPermuteVectorExtract)                                     \
  V(SVEPermuteVectorInterleaving)                                \
  V(SVEPredicateCount)                                           \
  V(SVEPredicateLogical)                                         \
  V(SVEPropagateBreak)                                           \
  V(SVEStackFrameAdjustment)                                     \
  V(SVEStackFrameSize)                                           \
  V(SVEVectorSelect)                                             \
  V(SVEBitwiseLogical_Predicated)                                \
  V(SVEBitwiseLogicalWithImm_Unpredicated)                       \
  V(SVEBitwiseShiftByImm_Predicated)                             \
  V(SVEBitwiseShiftByVector_Predicated)                          \
  V(SVEBitwiseShiftByWideElements_Predicated)                    \
  V(SVEBroadcastBitmaskImm)                                      \
  V(SVEBroadcastFPImm_Unpredicated)                              \
  V(SVEBroadcastGeneralRegister)                                 \
  V(SVEBroadcastIndexElement)                                    \
  V(SVEBroadcastIntImm_Unpredicated)                             \
  V(SVECompressActiveElements)                                   \
  V(SVEConditionallyBroadcastElementToVector)                    \
  V(SVEConditionallyExtractElementToSIMDFPScalar)                \
  V(SVEConditionallyExtractElementToGeneralRegister)             \
  V(SVEConditionallyTerminateScalars)                            \
  V(SVEConstructivePrefix_Unpredicated)                          \
  V(SVEContiguousFirstFaultLoad_ScalarPlusScalar)                \
  V(SVEContiguousLoad_ScalarPlusImm)                             \
  V(SVEContiguousLoad_ScalarPlusScalar)                          \
  V(SVEContiguousNonFaultLoad_ScalarPlusImm)                     \
  V(SVEContiguousNonTemporalLoad_ScalarPlusImm)                  \
  V(SVEContiguousNonTemporalLoad_ScalarPlusScalar)               \
  V(SVEContiguousNonTemporalStore_ScalarPlusImm)                 \
  V(SVEContiguousNonTemporalStore_ScalarPlusScalar)              \
  V(SVEContiguousPrefetch_ScalarPlusImm)                         \
  V(SVEContiguousPrefetch_ScalarPlusScalar)                      \
  V(SVEContiguousStore_ScalarPlusImm)                            \
  V(SVEContiguousStore_ScalarPlusScalar)                         \
  V(SVECopySIMDFPScalarRegisterToVector_Predicated)              \
  V(SVECopyFPImm_Predicated)                                     \
  V(SVECopyGeneralRegisterToVector_Predicated)                   \
  V(SVECopyIntImm_Predicated)                                    \
  V(SVEElementCount)                                             \
  V(SVEExtractElementToSIMDFPScalarRegister)                     \
  V(SVEExtractElementToGeneralRegister)                          \
  V(SVEFPArithmetic_Predicated)                                  \
  V(SVEFPArithmeticWithImm_Predicated)                           \
  V(SVEFPConvertPrecision)                                       \
  V(SVEFPConvertToInt)                                           \
  V(SVEFPExponentialAccelerator)                                 \
  V(SVEFPRoundToIntegralValue)                                   \
  V(SVEFPTrigMulAddCoefficient)                                  \
  V(SVEFPTrigSelectCoefficient)                                  \
  V(SVEFPUnaryOp)                                                \
  V(SVEIncDecRegisterByElementCount)                             \
  V(SVEIncDecVectorByElementCount)                               \
  V(SVEInsertSIMDFPScalarRegister)                               \
  V(SVEInsertGeneralRegister)                                    \
  V(SVEIntAddSubtractImm_Unpredicated)                           \
  V(SVEIntAddSubtractVectors_Predicated)                         \
  V(SVEIntCompareScalarCountAndLimit)                            \
  V(SVEIntConvertToFP)                                           \
  V(SVEIntDivideVectors_Predicated)                              \
  V(SVEIntMinMaxImm_Unpredicated)                                \
  V(SVEIntMinMaxDifference_Predicated)                           \
  V(SVEIntMulImm_Unpredicated)                                   \
  V(SVEIntMulVectors_Predicated)                                 \
  V(SVELoadAndBroadcastElement)                                  \
  V(SVELoadAndBroadcastQuadword_ScalarPlusImm)                   \
  V(SVELoadAndBroadcastQuadword_ScalarPlusScalar)                \
  V(SVELoadMultipleStructures_ScalarPlusImm)                     \
  V(SVELoadMultipleStructures_ScalarPlusScalar)                  \
  V(SVELoadPredicateRegister)                                    \
  V(SVELoadVectorRegister)                                       \
  V(SVEPartitionBreakCondition)                                  \
  V(SVEPermutePredicateElements)                                 \
  V(SVEPredicateFirstActive)                                     \
  V(SVEPredicateInitialize)                                      \
  V(SVEPredicateNextActive)                                      \
  V(SVEPredicateReadFromFFR_Predicated)                          \
  V(SVEPredicateReadFromFFR_Unpredicated)                        \
  V(SVEPredicateTest)                                            \
  V(SVEPredicateZero)                                            \
  V(SVEPropagateBreakToNextPartition)                            \
  V(SVEReversePredicateElements)                                 \
  V(SVEReverseVectorElements)                                    \
  V(SVEReverseWithinElements)                                    \
  V(SVESaturatingIncDecRegisterByElementCount)                   \
  V(SVESaturatingIncDecVectorByElementCount)                     \
  V(SVEStoreMultipleStructures_ScalarPlusImm)                    \
  V(SVEStoreMultipleStructures_ScalarPlusScalar)                 \
  V(SVEStorePredicateRegister)                                   \
  V(SVEStoreVectorRegister)                                      \
  V(SVETableLookup)                                              \
  V(SVEUnpackPredicateElements)                                  \
  V(SVEUnpackVectorElements)                                     \
  V(SVEVectorSplice)                                             \
  V(System)                                                      \
  V(TestBranch)                                                  \
  V(Unallocated)                                                 \
  V(UnconditionalBranch)                                         \
  V(UnconditionalBranchToRegister)                               \
  V(Unimplemented)

#define VISITOR_LIST_THAT_DONT_RETURN(V) V(Reserved)

#define VISITOR_LIST(V)       \
  VISITOR_LIST_THAT_RETURN(V) \
  VISITOR_LIST_THAT_DONT_RETURN(V)

namespace vixl {
namespace aarch64 {

using Metadata = std::map<std::string, std::string>;

// The Visitor interface. Disassembler and simulator (and other tools)
// must provide implementations for all of these functions.
//
// Note that this class must change in breaking ways with even minor additions
// to VIXL, and so its API should be considered unstable. User classes that
// inherit from this one should be expected to break even on minor version
// updates. If this is a problem, consider using DecoderVisitorWithDefaults
// instead.
class DecoderVisitor {
 public:
  enum VisitorConstness { kConstVisitor, kNonConstVisitor };
  explicit DecoderVisitor(VisitorConstness constness = kConstVisitor)
      : constness_(constness) {}

  virtual ~DecoderVisitor() {}

#define DECLARE(A) virtual void Visit##A(const Instruction* instr) = 0;
  VISITOR_LIST(DECLARE)
#undef DECLARE

  virtual void Visit(Metadata* metadata, const Instruction* instr) = 0;

  bool IsConstVisitor() const { return constness_ == kConstVisitor; }
  Instruction* MutableInstruction(const Instruction* instr) {
    VIXL_ASSERT(!IsConstVisitor());
    return const_cast<Instruction*>(instr);
  }

 private:
  const VisitorConstness constness_;
};

// As above, but a default (no-op) implementation for each visitor is provided.
// This is useful for derived class that only care about specific visitors.
//
// A minor version update may add a visitor, but will never remove one, so it is
// safe (and recommended) to use `override` in derived classes.
class DecoderVisitorWithDefaults : public DecoderVisitor {
 public:
  explicit DecoderVisitorWithDefaults(
      VisitorConstness constness = kConstVisitor)
      : DecoderVisitor(constness) {}

  virtual ~DecoderVisitorWithDefaults() {}

#define DECLARE(A) \
  virtual void Visit##A(const Instruction* instr) VIXL_OVERRIDE { USE(instr); }
  VISITOR_LIST(DECLARE)
#undef DECLARE
  virtual void Visit(Metadata* metadata,
                     const Instruction* instr) VIXL_OVERRIDE {
    USE(metadata);
    USE(instr);
  }
};

class DecodeNode;
class CompiledDecodeNode;

// The instruction decoder is constructed from a graph of decode nodes. At each
// node, a number of bits are sampled from the instruction being decoded. The
// resulting value is used to look up the next node in the graph, which then
// samples other bits, and moves to other decode nodes. Eventually, a visitor
// node is reached, and the corresponding visitor function is called, which
// handles the instruction.
class Decoder {
 public:
  Decoder() { ConstructDecodeGraph(); }

  // Top-level wrappers around the actual decoding function.
  void Decode(const Instruction* instr);
  void Decode(Instruction* instr);

  // Decode all instructions from start (inclusive) to end (exclusive).
  template <typename T>
  void Decode(T start, T end) {
    for (T instr = start; instr < end; instr = instr->GetNextInstruction()) {
      Decode(instr);
    }
  }

  // Register a new visitor class with the decoder.
  // Decode() will call the corresponding visitor method from all registered
  // visitor classes when decoding reaches the leaf node of the instruction
  // decode tree.
  // Visitors are called in order.
  // A visitor can be registered multiple times.
  //
  //   d.AppendVisitor(V1);
  //   d.AppendVisitor(V2);
  //   d.PrependVisitor(V2);
  //   d.AppendVisitor(V3);
  //
  //   d.Decode(i);
  //
  // will call in order visitor methods in V2, V1, V2, V3.
  void AppendVisitor(DecoderVisitor* visitor);
  void PrependVisitor(DecoderVisitor* visitor);
  // These helpers register `new_visitor` before or after the first instance of
  // `registered_visiter` in the list.
  // So if
  //   V1, V2, V1, V2
  // are registered in this order in the decoder, calls to
  //   d.InsertVisitorAfter(V3, V1);
  //   d.InsertVisitorBefore(V4, V2);
  // will yield the order
  //   V1, V3, V4, V2, V1, V2
  //
  // For more complex modifications of the order of registered visitors, one can
  // directly access and modify the list of visitors via the `visitors()'
  // accessor.
  void InsertVisitorBefore(DecoderVisitor* new_visitor,
                           DecoderVisitor* registered_visitor);
  void InsertVisitorAfter(DecoderVisitor* new_visitor,
                          DecoderVisitor* registered_visitor);

  // Remove all instances of a previously registered visitor class from the list
  // of visitors stored by the decoder.
  void RemoveVisitor(DecoderVisitor* visitor);

#define DECLARE(A) void Visit##A(const Instruction* instr);
  VISITOR_LIST(DECLARE)
#undef DECLARE

#define DECLARE(A) void Visit_##A(const Instruction* instr);
  SVE_INSTRUCTION_VISITOR_LIST(DECLARE)
#undef DECLARE

  std::list<DecoderVisitor*>* visitors() { return &visitors_; }

  // Get a DecodeNode by name from the Decoder's map.
  DecodeNode* GetDecodeNode(std::string name);

 private:
  // Decodes an instruction and calls the visitor functions registered with the
  // Decoder class.
  void DecodeInstruction(const Instruction* instr);

  // Add an initialised DecodeNode to the decode_node_ map.
  void AddDecodeNode(const DecodeNode& node);

  // Visitors are registered in a list.
  std::list<DecoderVisitor*> visitors_;

  // Compile the dynamically generated decode graph based on the static
  // information in kDecodeMapping and kVisitorNodes.
  void ConstructDecodeGraph();

  // Root node for the compiled decoder graph, stored here to avoid a map lookup
  // for every instruction decoded.
  CompiledDecodeNode* compiled_decoder_root_;

  // Map of node names to DecodeNodes.
  std::map<std::string, DecodeNode> decode_nodes_;
};

const int kMaxDecodeSampledBits = 16;
const int kMaxDecodeMappings = 250;
typedef void (Decoder::*DecodeFnPtr)(const Instruction*);
typedef uint32_t (Instruction::*BitExtractFn)(void) const;

// A Visitor node maps the name of a visitor to the function that handles it.
struct VisitorNode {
  const char* name;
  const DecodeFnPtr visitor_fn;
};

// DecodePattern and DecodeMapping represent the input data to the decoder
// compilation stage. After compilation, the decoder is embodied in the graph
// of CompiledDecodeNodes pointer to by compiled_decoder_root_.

// A DecodePattern maps a pattern of set/unset/don't care (1, 0, x) bits as a
// string to the name of its handler.
struct DecodePattern {
  const char* pattern;
  const char* handler;
};

// A DecodeMapping consists of the name of a handler, the bits sampled in the
// instruction by that handler, and a mapping from the pattern that those
// sampled bits match to the corresponding name of a node.
struct DecodeMapping {
  const char* name;
  const uint8_t sampled_bits[kMaxDecodeSampledBits];
  const DecodePattern mapping[kMaxDecodeMappings];
};

// For speed, before nodes can be used for decoding instructions, they must
// be compiled. This converts the mapping "bit pattern strings to decoder name
// string" stored in DecodeNodes to an array look up for the pointer to the next
// node, stored in CompiledDecodeNodes. Compilation may also apply other
// optimisations for simple decode patterns.
class CompiledDecodeNode {
 public:
  // Constructor for decode node, containing a decode table and pointer to a
  // function that extracts the bits to be sampled.
  CompiledDecodeNode(BitExtractFn bit_extract_fn, size_t decode_table_size)
      : bit_extract_fn_(bit_extract_fn),
        visitor_fn_(NULL),
        decode_table_size_(decode_table_size),
        decoder_(NULL) {
    decode_table_ = new CompiledDecodeNode*[decode_table_size_];
    memset(decode_table_, 0, decode_table_size_ * sizeof(decode_table_[0]));
  }

  // Constructor for wrappers around visitor functions. These require no
  // decoding, so no bit extraction function or decode table is assigned.
  explicit CompiledDecodeNode(DecodeFnPtr visitor_fn, Decoder* decoder)
      : bit_extract_fn_(NULL),
        visitor_fn_(visitor_fn),
        decode_table_(NULL),
        decode_table_size_(0),
        decoder_(decoder) {}

  ~CompiledDecodeNode() VIXL_NEGATIVE_TESTING_ALLOW_EXCEPTION {
    // Free the decode table, if this is a compiled, non-leaf node.
    if (decode_table_ != NULL) {
      VIXL_ASSERT(!IsLeafNode());
      delete[] decode_table_;
    }
  }

  // Decode the instruction by either sampling the bits using the bit extract
  // function to find the next node, or, if we're at a leaf, calling the visitor
  // function.
  void Decode(const Instruction* instr) const;

  // A leaf node is a wrapper for a visitor function.
  bool IsLeafNode() const {
    VIXL_ASSERT(((visitor_fn_ == NULL) && (bit_extract_fn_ != NULL)) ||
                ((visitor_fn_ != NULL) && (bit_extract_fn_ == NULL)));
    return visitor_fn_ != NULL;
  }

  // Get a pointer to the next node required in the decode process, based on the
  // bits sampled by the current node.
  CompiledDecodeNode* GetNodeForBits(uint32_t bits) const {
    VIXL_ASSERT(bits < decode_table_size_);
    return decode_table_[bits];
  }

  // Set the next node in the decode process for the pattern of sampled bits in
  // the current node.
  void SetNodeForBits(uint32_t bits, CompiledDecodeNode* n) {
    VIXL_ASSERT(bits < decode_table_size_);
    VIXL_ASSERT(n != NULL);
    decode_table_[bits] = n;
  }

 private:
  // Pointer to an instantiated template function for extracting the bits
  // sampled by this node. Set to NULL for leaf nodes.
  const BitExtractFn bit_extract_fn_;

  // Visitor function that handles the instruction identified. Set only for
  // leaf nodes, where no extra decoding is required, otherwise NULL.
  const DecodeFnPtr visitor_fn_;

  // Mapping table from instruction bits to next decode stage.
  CompiledDecodeNode** decode_table_;
  const size_t decode_table_size_;

  // Pointer to the decoder containing this node, used to call its visitor
  // function for leaf nodes. Set to NULL for non-leaf nodes.
  Decoder* decoder_;
};

class DecodeNode {
 public:
  // Default constructor needed for map initialisation.
  DecodeNode() : compiled_node_(NULL) {}

  // Constructor for DecodeNode wrappers around visitor functions. These are
  // marked as "compiled", as there is no decoding left to do.
  explicit DecodeNode(const VisitorNode& visitor, Decoder* decoder)
      : name_(visitor.name),
        visitor_fn_(visitor.visitor_fn),
        decoder_(decoder),
        compiled_node_(NULL) {}

  // Constructor for DecodeNodes that map bit patterns to other DecodeNodes.
  explicit DecodeNode(const DecodeMapping& map, Decoder* decoder = NULL)
      : name_(map.name),
        visitor_fn_(NULL),
        decoder_(decoder),
        compiled_node_(NULL) {
    // The length of the bit string in the first mapping determines the number
    // of sampled bits. When adding patterns later, we assert that all mappings
    // sample the same number of bits.
    VIXL_CHECK(strcmp(map.mapping[0].pattern, "otherwise") != 0);
    int bit_count = static_cast<int>(strlen(map.mapping[0].pattern));
    VIXL_CHECK((bit_count > 0) && (bit_count <= 32));
    SetSampledBits(map.sampled_bits, bit_count);
    AddPatterns(map.mapping);
  }

  ~DecodeNode() {
    // Delete the compiled version of this node, if one was created.
    if (compiled_node_ != NULL) {
      delete compiled_node_;
    }
  }

  // Set the bits sampled from the instruction by this node.
  void SetSampledBits(const uint8_t* bits, int bit_count);

  // Get the bits sampled from the instruction by this node.
  std::vector<uint8_t> GetSampledBits() const;

  // Get the number of bits sampled from the instruction by this node.
  size_t GetSampledBitsCount() const;

  // Add patterns to this node's internal pattern table.
  void AddPatterns(const DecodePattern* patterns);

  // A leaf node is a DecodeNode that wraps the visitor function for the
  // identified instruction class.
  bool IsLeafNode() const { return visitor_fn_ != NULL; }

  std::string GetName() const { return name_; }

  // Create a CompiledDecodeNode of specified table size that uses
  // bit_extract_fn to sample bits from the instruction.
  void CreateCompiledNode(BitExtractFn bit_extract_fn, size_t table_size) {
    VIXL_ASSERT(bit_extract_fn != NULL);
    VIXL_ASSERT(table_size > 0);
    compiled_node_ = new CompiledDecodeNode(bit_extract_fn, table_size);
  }

  // Create a CompiledDecodeNode wrapping a visitor function. No decoding is
  // required for this node; the visitor function is called instead.
  void CreateVisitorNode() {
    compiled_node_ = new CompiledDecodeNode(visitor_fn_, decoder_);
  }

  // Find and compile the DecodeNode named "name", and set it as the node for
  // the pattern "bits".
  void CompileNodeForBits(Decoder* decoder, std::string name, uint32_t bits);

  // Get a pointer to an instruction method that extracts the instruction bits
  // specified by the mask argument, and returns those sampled bits as a
  // contiguous sequence, suitable for indexing an array.
  // For example, a mask of 0b1010 returns a function that, given an instruction
  // 0bXYZW, will return 0bXZ.
  BitExtractFn GetBitExtractFunction(uint32_t mask);

  // Get a pointer to an Instruction method that applies a mask to the
  // instruction bits, and tests if the result is equal to value. The returned
  // function gives a 1 result if (inst & mask == value), 0 otherwise.
  BitExtractFn GetBitExtractFunction(uint32_t mask, uint32_t value);

  // Compile this DecodeNode into a new CompiledDecodeNode and returns a pointer
  // to it. This pointer is also stored inside the DecodeNode itself. Destroying
  // a DecodeNode frees its associated CompiledDecodeNode.
  CompiledDecodeNode* Compile(Decoder* decoder);

  // Get a pointer to the CompiledDecodeNode associated with this DecodeNode.
  // Returns NULL if the node has not been compiled yet.
  CompiledDecodeNode* GetCompiledNode() const { return compiled_node_; }
  bool IsCompiled() const { return GetCompiledNode() != NULL; }

 private:
  // Generate a mask and value pair from a string constructed from 0, 1 and x
  // (don't care) characters.
  // For example "10x1" should return mask = 0b1101, value = 0b1001.
  typedef std::pair<Instr, Instr> MaskValuePair;
  MaskValuePair GenerateMaskValuePair(std::string pattern) const;

  // Generate a pattern string ordered by the bit positions sampled by this
  // node. The first character in the string corresponds to the lowest sampled
  // bit.
  // For example, a pattern of "1x0" expected when sampling bits 31, 1 and 30
  // returns the pattern "x01"; bit 1 should be 'x', bit 30 '0' and bit 31 '1'.
  // This output makes comparisons easier between the pattern and bits sampled
  // from an instruction using the fast "compress" algorithm. See
  // Instruction::Compress().
  std::string GenerateOrderedPattern(std::string pattern) const;

  // Generate a mask with a bit set at each sample position.
  uint32_t GenerateSampledBitsMask() const;

  // Try to compile a more optimised decode operation for this node, returning
  // true if successful.
  bool TryCompileOptimisedDecodeTable(Decoder* decoder);

  // Name of this decoder node, used to construct edges in the decode graph.
  std::string name_;

  // Vector of bits sampled from an instruction to determine which node to look
  // up next in the decode process.
  std::vector<uint8_t> sampled_bits_;

  // Visitor function that handles the instruction identified. Set only for leaf
  // nodes, where no extra decoding is required. For non-leaf decoding nodes,
  // this pointer is NULL.
  DecodeFnPtr visitor_fn_;

  // Source mapping from bit pattern to name of next decode stage.
  std::vector<DecodePattern> pattern_table_;

  // Pointer to the decoder containing this node, used to call its visitor
  // function for leaf nodes.
  Decoder* decoder_;

  // Pointer to the compiled version of this node. Is this node hasn't been
  // compiled yet, this pointer is NULL.
  CompiledDecodeNode* compiled_node_;
};

}  // namespace aarch64
}  // namespace vixl

#endif  // VIXL_AARCH64_DECODER_AARCH64_H_
