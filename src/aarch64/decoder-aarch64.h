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
#define INSTRUCTION_VISITOR_LIST(V) \
  V(abs_asimdmisc_r)                \
  V(abs_asisdmisc_r)                \
  V(abs_z_p_z)                      \
  V(adc_32_addsub_carry)            \
  V(adc_64_addsub_carry)            \
  V(adclb_z_zzz)                    \
  V(adclt_z_zzz)                    \
  V(adcs_32_addsub_carry)           \
  V(adcs_64_addsub_carry)           \
  V(add_32_addsub_ext)              \
  V(add_32_addsub_imm)              \
  V(add_32_addsub_shift)            \
  V(add_64_addsub_ext)              \
  V(add_64_addsub_imm)              \
  V(add_64_addsub_shift)            \
  V(add_asimdsame_only)             \
  V(add_asisdsame_only)             \
  V(add_z_p_zz)                     \
  V(add_z_zi)                       \
  V(add_z_zz)                       \
  V(addg_64_addsub_immtags)         \
  V(addhn_asimddiff_n)              \
  V(addhnb_z_zz)                    \
  V(addhnt_z_zz)                    \
  V(addp_asimdsame_only)            \
  V(addp_asisdpair_only)            \
  V(addp_z_p_zz)                    \
  V(addpl_r_ri)                     \
  V(adds_32_addsub_shift)           \
  V(adds_32s_addsub_ext)            \
  V(adds_32s_addsub_imm)            \
  V(adds_64_addsub_shift)           \
  V(adds_64s_addsub_ext)            \
  V(adds_64s_addsub_imm)            \
  V(addv_asimdall_only)             \
  V(addvl_r_ri)                     \
  V(adr_only_pcreladdr)             \
  V(adr_z_az_d_s32_scaled)          \
  V(adr_z_az_d_u32_scaled)          \
  V(adr_z_az_sd_same_scaled)        \
  V(adrp_only_pcreladdr)            \
  V(aesd_b_cryptoaes)               \
  V(aesd_z_zz)                      \
  V(aese_b_cryptoaes)               \
  V(aese_z_zz)                      \
  V(aesimc_b_cryptoaes)             \
  V(aesimc_z_z)                     \
  V(aesmc_b_cryptoaes)              \
  V(aesmc_z_z)                      \
  V(and_32_log_imm)                 \
  V(and_32_log_shift)               \
  V(and_64_log_imm)                 \
  V(and_64_log_shift)               \
  V(and_asimdsame_only)             \
  V(and_p_p_pp_z)                   \
  V(and_z_p_zz)                     \
  V(and_z_zi)                       \
  V(and_z_zz)                       \
  V(ands_32_log_shift)              \
  V(ands_32s_log_imm)               \
  V(ands_64_log_shift)              \
  V(ands_64s_log_imm)               \
  V(ands_p_p_pp_z)                  \
  V(andv_r_p_z)                     \
  V(asr_z_p_zi)                     \
  V(asr_z_p_zw)                     \
  V(asr_z_p_zz)                     \
  V(asr_z_zi)                       \
  V(asr_z_zw)                       \
  V(asrd_z_p_zi)                    \
  V(asrr_z_p_zz)                    \
  V(asrv_32_dp_2src)                \
  V(asrv_64_dp_2src)                \
  V(autda_64p_dp_1src)              \
  V(autdb_64p_dp_1src)              \
  V(autdza_64z_dp_1src)             \
  V(autdzb_64z_dp_1src)             \
  V(autia1716_hi_hints)             \
  V(autia_64p_dp_1src)              \
  V(autiasp_hi_hints)               \
  V(autiaz_hi_hints)                \
  V(autib1716_hi_hints)             \
  V(autib_64p_dp_1src)              \
  V(autibsp_hi_hints)               \
  V(autibz_hi_hints)                \
  V(autiza_64z_dp_1src)             \
  V(autizb_64z_dp_1src)             \
  V(axflag_m_pstate)                \
  V(b_only_branch_imm)              \
  V(b_only_condbranch)              \
  V(bcax_vvv16_crypto4)             \
  V(bcax_z_zzz)                     \
  V(bdep_z_zz)                      \
  V(bext_z_zz)                      \
  V(bfcvt_bs_floatdp1)              \
  V(bfcvt_z_p_z_s2bf)               \
  V(bfcvtn_asimdmisc_4s)            \
  V(bfcvtnt_z_p_z_s2bf)             \
  V(bfdot_asimdelem_e)              \
  V(bfdot_asimdsame2_d)             \
  V(bfdot_z_zzz)                    \
  V(bfdot_z_zzzi)                   \
  V(bfm_32m_bitfield)               \
  V(bfm_64m_bitfield)               \
  V(bfmlal_asimdelem_f)             \
  V(bfmlal_asimdsame2_f)            \
  V(bfmlalb_z_zzz)                  \
  V(bfmlalb_z_zzzi)                 \
  V(bfmlalt_z_zzz)                  \
  V(bfmlalt_z_zzzi)                 \
  V(bfmmla_asimdsame2_e)            \
  V(bfmmla_z_zzz)                   \
  V(bgrp_z_zz)                      \
  V(bic_32_log_shift)               \
  V(bic_64_log_shift)               \
  V(bic_asimdimm_l_hl)              \
  V(bic_asimdimm_l_sl)              \
  V(bic_asimdsame_only)             \
  V(bic_p_p_pp_z)                   \
  V(bic_z_p_zz)                     \
  V(bic_z_zz)                       \
  V(bics_32_log_shift)              \
  V(bics_64_log_shift)              \
  V(bics_p_p_pp_z)                  \
  V(bif_asimdsame_only)             \
  V(bit_asimdsame_only)             \
  V(bl_only_branch_imm)             \
  V(blr_64_branch_reg)              \
  V(blraa_64p_branch_reg)           \
  V(blraaz_64_branch_reg)           \
  V(blrab_64p_branch_reg)           \
  V(blrabz_64_branch_reg)           \
  V(br_64_branch_reg)               \
  V(braa_64p_branch_reg)            \
  V(braaz_64_branch_reg)            \
  V(brab_64p_branch_reg)            \
  V(brabz_64_branch_reg)            \
  V(brk_ex_exception)               \
  V(brka_p_p_p)                     \
  V(brkas_p_p_p_z)                  \
  V(brkb_p_p_p)                     \
  V(brkbs_p_p_p_z)                  \
  V(brkn_p_p_pp)                    \
  V(brkns_p_p_pp)                   \
  V(brkpa_p_p_pp)                   \
  V(brkpas_p_p_pp)                  \
  V(brkpb_p_p_pp)                   \
  V(brkpbs_p_p_pp)                  \
  V(bsl1n_z_zzz)                    \
  V(bsl2n_z_zzz)                    \
  V(bsl_asimdsame_only)             \
  V(bsl_z_zzz)                      \
  V(bti_hb_hints)                   \
  V(cadd_z_zz)                      \
  V(cas_c32_ldstexcl)               \
  V(cas_c64_ldstexcl)               \
  V(casa_c32_ldstexcl)              \
  V(casa_c64_ldstexcl)              \
  V(casab_c32_ldstexcl)             \
  V(casah_c32_ldstexcl)             \
  V(casal_c32_ldstexcl)             \
  V(casal_c64_ldstexcl)             \
  V(casalb_c32_ldstexcl)            \
  V(casalh_c32_ldstexcl)            \
  V(casb_c32_ldstexcl)              \
  V(cash_c32_ldstexcl)              \
  V(casl_c32_ldstexcl)              \
  V(casl_c64_ldstexcl)              \
  V(caslb_c32_ldstexcl)             \
  V(caslh_c32_ldstexcl)             \
  V(casp_cp32_ldstexcl)             \
  V(casp_cp64_ldstexcl)             \
  V(caspa_cp32_ldstexcl)            \
  V(caspa_cp64_ldstexcl)            \
  V(caspal_cp32_ldstexcl)           \
  V(caspal_cp64_ldstexcl)           \
  V(caspl_cp32_ldstexcl)            \
  V(caspl_cp64_ldstexcl)            \
  V(cbnz_32_compbranch)             \
  V(cbnz_64_compbranch)             \
  V(cbz_32_compbranch)              \
  V(cbz_64_compbranch)              \
  V(ccmn_32_condcmp_imm)            \
  V(ccmn_32_condcmp_reg)            \
  V(ccmn_64_condcmp_imm)            \
  V(ccmn_64_condcmp_reg)            \
  V(ccmp_32_condcmp_imm)            \
  V(ccmp_32_condcmp_reg)            \
  V(ccmp_64_condcmp_imm)            \
  V(ccmp_64_condcmp_reg)            \
  V(cdot_z_zzz)                     \
  V(cdot_z_zzzi_d)                  \
  V(cdot_z_zzzi_s)                  \
  V(cfinv_m_pstate)                 \
  V(clasta_r_p_z)                   \
  V(clasta_v_p_z)                   \
  V(clasta_z_p_zz)                  \
  V(clastb_r_p_z)                   \
  V(clastb_v_p_z)                   \
  V(clastb_z_p_zz)                  \
  V(clrex_bn_barriers)              \
  V(cls_32_dp_1src)                 \
  V(cls_64_dp_1src)                 \
  V(cls_asimdmisc_r)                \
  V(cls_z_p_z)                      \
  V(clz_32_dp_1src)                 \
  V(clz_64_dp_1src)                 \
  V(clz_asimdmisc_r)                \
  V(clz_z_p_z)                      \
  V(cmeq_asimdmisc_z)               \
  V(cmeq_asimdsame_only)            \
  V(cmeq_asisdmisc_z)               \
  V(cmeq_asisdsame_only)            \
  V(cmge_asimdmisc_z)               \
  V(cmge_asimdsame_only)            \
  V(cmge_asisdmisc_z)               \
  V(cmge_asisdsame_only)            \
  V(cmgt_asimdmisc_z)               \
  V(cmgt_asimdsame_only)            \
  V(cmgt_asisdmisc_z)               \
  V(cmgt_asisdsame_only)            \
  V(cmhi_asimdsame_only)            \
  V(cmhi_asisdsame_only)            \
  V(cmhs_asimdsame_only)            \
  V(cmhs_asisdsame_only)            \
  V(cmla_z_zzz)                     \
  V(cmla_z_zzzi_h)                  \
  V(cmla_z_zzzi_s)                  \
  V(cmle_asimdmisc_z)               \
  V(cmle_asisdmisc_z)               \
  V(cmlt_asimdmisc_z)               \
  V(cmlt_asisdmisc_z)               \
  V(cmpeq_p_p_zi)                   \
  V(cmpeq_p_p_zw)                   \
  V(cmpeq_p_p_zz)                   \
  V(cmpge_p_p_zi)                   \
  V(cmpge_p_p_zw)                   \
  V(cmpge_p_p_zz)                   \
  V(cmpgt_p_p_zi)                   \
  V(cmpgt_p_p_zw)                   \
  V(cmpgt_p_p_zz)                   \
  V(cmphi_p_p_zi)                   \
  V(cmphi_p_p_zw)                   \
  V(cmphi_p_p_zz)                   \
  V(cmphs_p_p_zi)                   \
  V(cmphs_p_p_zw)                   \
  V(cmphs_p_p_zz)                   \
  V(cmple_p_p_zi)                   \
  V(cmple_p_p_zw)                   \
  V(cmplo_p_p_zi)                   \
  V(cmplo_p_p_zw)                   \
  V(cmpls_p_p_zi)                   \
  V(cmpls_p_p_zw)                   \
  V(cmplt_p_p_zi)                   \
  V(cmplt_p_p_zw)                   \
  V(cmpne_p_p_zi)                   \
  V(cmpne_p_p_zw)                   \
  V(cmpne_p_p_zz)                   \
  V(cmtst_asimdsame_only)           \
  V(cmtst_asisdsame_only)           \
  V(cnot_z_p_z)                     \
  V(cnt_asimdmisc_r)                \
  V(cnt_z_p_z)                      \
  V(cntb_r_s)                       \
  V(cntd_r_s)                       \
  V(cnth_r_s)                       \
  V(cntp_r_p_p)                     \
  V(cntw_r_s)                       \
  V(compact_z_p_z)                  \
  V(cpy_z_o_i)                      \
  V(cpy_z_p_i)                      \
  V(cpy_z_p_r)                      \
  V(cpy_z_p_v)                      \
  V(crc32b_32c_dp_2src)             \
  V(crc32cb_32c_dp_2src)            \
  V(crc32ch_32c_dp_2src)            \
  V(crc32cw_32c_dp_2src)            \
  V(crc32cx_64c_dp_2src)            \
  V(crc32h_32c_dp_2src)             \
  V(crc32w_32c_dp_2src)             \
  V(crc32x_64c_dp_2src)             \
  V(csdb_hi_hints)                  \
  V(csel_32_condsel)                \
  V(csel_64_condsel)                \
  V(csinc_32_condsel)               \
  V(csinc_64_condsel)               \
  V(csinv_32_condsel)               \
  V(csinv_64_condsel)               \
  V(csneg_32_condsel)               \
  V(csneg_64_condsel)               \
  V(ctermeq_rr)                     \
  V(ctermne_rr)                     \
  V(dcps1_dc_exception)             \
  V(dcps2_dc_exception)             \
  V(dcps3_dc_exception)             \
  V(decb_r_rs)                      \
  V(decd_r_rs)                      \
  V(decd_z_zs)                      \
  V(dech_r_rs)                      \
  V(dech_z_zs)                      \
  V(decp_r_p_r)                     \
  V(decp_z_p_z)                     \
  V(decw_r_rs)                      \
  V(decw_z_zs)                      \
  V(dgh_hi_hints)                   \
  V(dmb_bo_barriers)                \
  V(drps_64e_branch_reg)            \
  V(dsb_bo_barriers)                \
  V(dsb_bon_barriers)               \
  V(dup_asimdins_dr_r)              \
  V(dup_asimdins_dv_v)              \
  V(dup_asisdone_only)              \
  V(dup_z_i)                        \
  V(dup_z_r)                        \
  V(dup_z_zi)                       \
  V(dupm_z_i)                       \
  V(eon_32_log_shift)               \
  V(eon_64_log_shift)               \
  V(eor3_vvv16_crypto4)             \
  V(eor3_z_zzz)                     \
  V(eor_32_log_imm)                 \
  V(eor_32_log_shift)               \
  V(eor_64_log_imm)                 \
  V(eor_64_log_shift)               \
  V(eor_asimdsame_only)             \
  V(eor_p_p_pp_z)                   \
  V(eor_z_p_zz)                     \
  V(eor_z_zi)                       \
  V(eor_z_zz)                       \
  V(eorbt_z_zz)                     \
  V(eors_p_p_pp_z)                  \
  V(eortb_z_zz)                     \
  V(eorv_r_p_z)                     \
  V(eret_64e_branch_reg)            \
  V(eretaa_64e_branch_reg)          \
  V(eretab_64e_branch_reg)          \
  V(esb_hi_hints)                   \
  V(ext_asimdext_only)              \
  V(ext_z_zi_con)                   \
  V(ext_z_zi_des)                   \
  V(extr_32_extract)                \
  V(extr_64_extract)                \
  V(fabd_asimdsame_only)            \
  V(fabd_asimdsamefp16_only)        \
  V(fabd_asisdsame_only)            \
  V(fabd_asisdsamefp16_only)        \
  V(fabd_z_p_zz)                    \
  V(fabs_asimdmisc_r)               \
  V(fabs_asimdmiscfp16_r)           \
  V(fabs_d_floatdp1)                \
  V(fabs_h_floatdp1)                \
  V(fabs_s_floatdp1)                \
  V(fabs_z_p_z)                     \
  V(facge_asimdsame_only)           \
  V(facge_asimdsamefp16_only)       \
  V(facge_asisdsame_only)           \
  V(facge_asisdsamefp16_only)       \
  V(facge_p_p_zz)                   \
  V(facgt_asimdsame_only)           \
  V(facgt_asimdsamefp16_only)       \
  V(facgt_asisdsame_only)           \
  V(facgt_asisdsamefp16_only)       \
  V(facgt_p_p_zz)                   \
  V(fadd_asimdsame_only)            \
  V(fadd_asimdsamefp16_only)        \
  V(fadd_d_floatdp2)                \
  V(fadd_h_floatdp2)                \
  V(fadd_s_floatdp2)                \
  V(fadd_z_p_zs)                    \
  V(fadd_z_p_zz)                    \
  V(fadd_z_zz)                      \
  V(fadda_v_p_z)                    \
  V(faddp_asimdsame_only)           \
  V(faddp_asimdsamefp16_only)       \
  V(faddp_asisdpair_only_h)         \
  V(faddp_asisdpair_only_sd)        \
  V(faddp_z_p_zz)                   \
  V(faddv_v_p_z)                    \
  V(fcadd_asimdsame2_c)             \
  V(fcadd_z_p_zz)                   \
  V(fccmp_d_floatccmp)              \
  V(fccmp_h_floatccmp)              \
  V(fccmp_s_floatccmp)              \
  V(fccmpe_d_floatccmp)             \
  V(fccmpe_h_floatccmp)             \
  V(fccmpe_s_floatccmp)             \
  V(fcmeq_asimdmisc_fz)             \
  V(fcmeq_asimdmiscfp16_fz)         \
  V(fcmeq_asimdsame_only)           \
  V(fcmeq_asimdsamefp16_only)       \
  V(fcmeq_asisdmisc_fz)             \
  V(fcmeq_asisdmiscfp16_fz)         \
  V(fcmeq_asisdsame_only)           \
  V(fcmeq_asisdsamefp16_only)       \
  V(fcmeq_p_p_z0)                   \
  V(fcmeq_p_p_zz)                   \
  V(fcmge_asimdmisc_fz)             \
  V(fcmge_asimdmiscfp16_fz)         \
  V(fcmge_asimdsame_only)           \
  V(fcmge_asimdsamefp16_only)       \
  V(fcmge_asisdmisc_fz)             \
  V(fcmge_asisdmiscfp16_fz)         \
  V(fcmge_asisdsame_only)           \
  V(fcmge_asisdsamefp16_only)       \
  V(fcmge_p_p_z0)                   \
  V(fcmge_p_p_zz)                   \
  V(fcmgt_asimdmisc_fz)             \
  V(fcmgt_asimdmiscfp16_fz)         \
  V(fcmgt_asimdsame_only)           \
  V(fcmgt_asimdsamefp16_only)       \
  V(fcmgt_asisdmisc_fz)             \
  V(fcmgt_asisdmiscfp16_fz)         \
  V(fcmgt_asisdsame_only)           \
  V(fcmgt_asisdsamefp16_only)       \
  V(fcmgt_p_p_z0)                   \
  V(fcmgt_p_p_zz)                   \
  V(fcmla_asimdelem_c_h)            \
  V(fcmla_asimdelem_c_s)            \
  V(fcmla_asimdsame2_c)             \
  V(fcmla_z_p_zzz)                  \
  V(fcmla_z_zzzi_h)                 \
  V(fcmla_z_zzzi_s)                 \
  V(fcmle_asimdmisc_fz)             \
  V(fcmle_asimdmiscfp16_fz)         \
  V(fcmle_asisdmisc_fz)             \
  V(fcmle_asisdmiscfp16_fz)         \
  V(fcmle_p_p_z0)                   \
  V(fcmlt_asimdmisc_fz)             \
  V(fcmlt_asimdmiscfp16_fz)         \
  V(fcmlt_asisdmisc_fz)             \
  V(fcmlt_asisdmiscfp16_fz)         \
  V(fcmlt_p_p_z0)                   \
  V(fcmne_p_p_z0)                   \
  V(fcmne_p_p_zz)                   \
  V(fcmp_d_floatcmp)                \
  V(fcmp_dz_floatcmp)               \
  V(fcmp_h_floatcmp)                \
  V(fcmp_hz_floatcmp)               \
  V(fcmp_s_floatcmp)                \
  V(fcmp_sz_floatcmp)               \
  V(fcmpe_d_floatcmp)               \
  V(fcmpe_dz_floatcmp)              \
  V(fcmpe_h_floatcmp)               \
  V(fcmpe_hz_floatcmp)              \
  V(fcmpe_s_floatcmp)               \
  V(fcmpe_sz_floatcmp)              \
  V(fcmuo_p_p_zz)                   \
  V(fcpy_z_p_i)                     \
  V(fcsel_d_floatsel)               \
  V(fcsel_h_floatsel)               \
  V(fcsel_s_floatsel)               \
  V(fcvt_dh_floatdp1)               \
  V(fcvt_ds_floatdp1)               \
  V(fcvt_hd_floatdp1)               \
  V(fcvt_hs_floatdp1)               \
  V(fcvt_sd_floatdp1)               \
  V(fcvt_sh_floatdp1)               \
  V(fcvt_z_p_z_d2h)                 \
  V(fcvt_z_p_z_d2s)                 \
  V(fcvt_z_p_z_h2d)                 \
  V(fcvt_z_p_z_h2s)                 \
  V(fcvt_z_p_z_s2d)                 \
  V(fcvt_z_p_z_s2h)                 \
  V(fcvtas_32d_float2int)           \
  V(fcvtas_32h_float2int)           \
  V(fcvtas_32s_float2int)           \
  V(fcvtas_64d_float2int)           \
  V(fcvtas_64h_float2int)           \
  V(fcvtas_64s_float2int)           \
  V(fcvtas_asimdmisc_r)             \
  V(fcvtas_asimdmiscfp16_r)         \
  V(fcvtas_asisdmisc_r)             \
  V(fcvtas_asisdmiscfp16_r)         \
  V(fcvtau_32d_float2int)           \
  V(fcvtau_32h_float2int)           \
  V(fcvtau_32s_float2int)           \
  V(fcvtau_64d_float2int)           \
  V(fcvtau_64h_float2int)           \
  V(fcvtau_64s_float2int)           \
  V(fcvtau_asimdmisc_r)             \
  V(fcvtau_asimdmiscfp16_r)         \
  V(fcvtau_asisdmisc_r)             \
  V(fcvtau_asisdmiscfp16_r)         \
  V(fcvtl_asimdmisc_l)              \
  V(fcvtlt_z_p_z_h2s)               \
  V(fcvtlt_z_p_z_s2d)               \
  V(fcvtms_32d_float2int)           \
  V(fcvtms_32h_float2int)           \
  V(fcvtms_32s_float2int)           \
  V(fcvtms_64d_float2int)           \
  V(fcvtms_64h_float2int)           \
  V(fcvtms_64s_float2int)           \
  V(fcvtms_asimdmisc_r)             \
  V(fcvtms_asimdmiscfp16_r)         \
  V(fcvtms_asisdmisc_r)             \
  V(fcvtms_asisdmiscfp16_r)         \
  V(fcvtmu_32d_float2int)           \
  V(fcvtmu_32h_float2int)           \
  V(fcvtmu_32s_float2int)           \
  V(fcvtmu_64d_float2int)           \
  V(fcvtmu_64h_float2int)           \
  V(fcvtmu_64s_float2int)           \
  V(fcvtmu_asimdmisc_r)             \
  V(fcvtmu_asimdmiscfp16_r)         \
  V(fcvtmu_asisdmisc_r)             \
  V(fcvtmu_asisdmiscfp16_r)         \
  V(fcvtn_asimdmisc_n)              \
  V(fcvtns_32d_float2int)           \
  V(fcvtns_32h_float2int)           \
  V(fcvtns_32s_float2int)           \
  V(fcvtns_64d_float2int)           \
  V(fcvtns_64h_float2int)           \
  V(fcvtns_64s_float2int)           \
  V(fcvtns_asimdmisc_r)             \
  V(fcvtns_asimdmiscfp16_r)         \
  V(fcvtns_asisdmisc_r)             \
  V(fcvtns_asisdmiscfp16_r)         \
  V(fcvtnt_z_p_z_d2s)               \
  V(fcvtnt_z_p_z_s2h)               \
  V(fcvtnu_32d_float2int)           \
  V(fcvtnu_32h_float2int)           \
  V(fcvtnu_32s_float2int)           \
  V(fcvtnu_64d_float2int)           \
  V(fcvtnu_64h_float2int)           \
  V(fcvtnu_64s_float2int)           \
  V(fcvtnu_asimdmisc_r)             \
  V(fcvtnu_asimdmiscfp16_r)         \
  V(fcvtnu_asisdmisc_r)             \
  V(fcvtnu_asisdmiscfp16_r)         \
  V(fcvtps_32d_float2int)           \
  V(fcvtps_32h_float2int)           \
  V(fcvtps_32s_float2int)           \
  V(fcvtps_64d_float2int)           \
  V(fcvtps_64h_float2int)           \
  V(fcvtps_64s_float2int)           \
  V(fcvtps_asimdmisc_r)             \
  V(fcvtps_asimdmiscfp16_r)         \
  V(fcvtps_asisdmisc_r)             \
  V(fcvtps_asisdmiscfp16_r)         \
  V(fcvtpu_32d_float2int)           \
  V(fcvtpu_32h_float2int)           \
  V(fcvtpu_32s_float2int)           \
  V(fcvtpu_64d_float2int)           \
  V(fcvtpu_64h_float2int)           \
  V(fcvtpu_64s_float2int)           \
  V(fcvtpu_asimdmisc_r)             \
  V(fcvtpu_asimdmiscfp16_r)         \
  V(fcvtpu_asisdmisc_r)             \
  V(fcvtpu_asisdmiscfp16_r)         \
  V(fcvtx_z_p_z_d2s)                \
  V(fcvtxn_asimdmisc_n)             \
  V(fcvtxn_asisdmisc_n)             \
  V(fcvtxnt_z_p_z_d2s)              \
  V(fcvtzs_32d_float2fix)           \
  V(fcvtzs_32d_float2int)           \
  V(fcvtzs_32h_float2fix)           \
  V(fcvtzs_32h_float2int)           \
  V(fcvtzs_32s_float2fix)           \
  V(fcvtzs_32s_float2int)           \
  V(fcvtzs_64d_float2fix)           \
  V(fcvtzs_64d_float2int)           \
  V(fcvtzs_64h_float2fix)           \
  V(fcvtzs_64h_float2int)           \
  V(fcvtzs_64s_float2fix)           \
  V(fcvtzs_64s_float2int)           \
  V(fcvtzs_asimdmisc_r)             \
  V(fcvtzs_asimdmiscfp16_r)         \
  V(fcvtzs_asimdshf_c)              \
  V(fcvtzs_asisdmisc_r)             \
  V(fcvtzs_asisdmiscfp16_r)         \
  V(fcvtzs_asisdshf_c)              \
  V(fcvtzs_z_p_z_d2w)               \
  V(fcvtzs_z_p_z_d2x)               \
  V(fcvtzs_z_p_z_fp162h)            \
  V(fcvtzs_z_p_z_fp162w)            \
  V(fcvtzs_z_p_z_fp162x)            \
  V(fcvtzs_z_p_z_s2w)               \
  V(fcvtzs_z_p_z_s2x)               \
  V(fcvtzu_32d_float2fix)           \
  V(fcvtzu_32d_float2int)           \
  V(fcvtzu_32h_float2fix)           \
  V(fcvtzu_32h_float2int)           \
  V(fcvtzu_32s_float2fix)           \
  V(fcvtzu_32s_float2int)           \
  V(fcvtzu_64d_float2fix)           \
  V(fcvtzu_64d_float2int)           \
  V(fcvtzu_64h_float2fix)           \
  V(fcvtzu_64h_float2int)           \
  V(fcvtzu_64s_float2fix)           \
  V(fcvtzu_64s_float2int)           \
  V(fcvtzu_asimdmisc_r)             \
  V(fcvtzu_asimdmiscfp16_r)         \
  V(fcvtzu_asimdshf_c)              \
  V(fcvtzu_asisdmisc_r)             \
  V(fcvtzu_asisdmiscfp16_r)         \
  V(fcvtzu_asisdshf_c)              \
  V(fcvtzu_z_p_z_d2w)               \
  V(fcvtzu_z_p_z_d2x)               \
  V(fcvtzu_z_p_z_fp162h)            \
  V(fcvtzu_z_p_z_fp162w)            \
  V(fcvtzu_z_p_z_fp162x)            \
  V(fcvtzu_z_p_z_s2w)               \
  V(fcvtzu_z_p_z_s2x)               \
  V(fdiv_asimdsame_only)            \
  V(fdiv_asimdsamefp16_only)        \
  V(fdiv_d_floatdp2)                \
  V(fdiv_h_floatdp2)                \
  V(fdiv_s_floatdp2)                \
  V(fdiv_z_p_zz)                    \
  V(fdivr_z_p_zz)                   \
  V(fdup_z_i)                       \
  V(fexpa_z_z)                      \
  V(fjcvtzs_32d_float2int)          \
  V(flogb_z_p_z)                    \
  V(fmad_z_p_zzz)                   \
  V(fmadd_d_floatdp3)               \
  V(fmadd_h_floatdp3)               \
  V(fmadd_s_floatdp3)               \
  V(fmax_asimdsame_only)            \
  V(fmax_asimdsamefp16_only)        \
  V(fmax_d_floatdp2)                \
  V(fmax_h_floatdp2)                \
  V(fmax_s_floatdp2)                \
  V(fmax_z_p_zs)                    \
  V(fmax_z_p_zz)                    \
  V(fmaxnm_asimdsame_only)          \
  V(fmaxnm_asimdsamefp16_only)      \
  V(fmaxnm_d_floatdp2)              \
  V(fmaxnm_h_floatdp2)              \
  V(fmaxnm_s_floatdp2)              \
  V(fmaxnm_z_p_zs)                  \
  V(fmaxnm_z_p_zz)                  \
  V(fmaxnmp_asimdsame_only)         \
  V(fmaxnmp_asimdsamefp16_only)     \
  V(fmaxnmp_asisdpair_only_h)       \
  V(fmaxnmp_asisdpair_only_sd)      \
  V(fmaxnmp_z_p_zz)                 \
  V(fmaxnmv_asimdall_only_h)        \
  V(fmaxnmv_asimdall_only_sd)       \
  V(fmaxnmv_v_p_z)                  \
  V(fmaxp_asimdsame_only)           \
  V(fmaxp_asimdsamefp16_only)       \
  V(fmaxp_asisdpair_only_h)         \
  V(fmaxp_asisdpair_only_sd)        \
  V(fmaxp_z_p_zz)                   \
  V(fmaxv_asimdall_only_h)          \
  V(fmaxv_asimdall_only_sd)         \
  V(fmaxv_v_p_z)                    \
  V(fmin_asimdsame_only)            \
  V(fmin_asimdsamefp16_only)        \
  V(fmin_d_floatdp2)                \
  V(fmin_h_floatdp2)                \
  V(fmin_s_floatdp2)                \
  V(fmin_z_p_zs)                    \
  V(fmin_z_p_zz)                    \
  V(fminnm_asimdsame_only)          \
  V(fminnm_asimdsamefp16_only)      \
  V(fminnm_d_floatdp2)              \
  V(fminnm_h_floatdp2)              \
  V(fminnm_s_floatdp2)              \
  V(fminnm_z_p_zs)                  \
  V(fminnm_z_p_zz)                  \
  V(fminnmp_asimdsame_only)         \
  V(fminnmp_asimdsamefp16_only)     \
  V(fminnmp_asisdpair_only_h)       \
  V(fminnmp_asisdpair_only_sd)      \
  V(fminnmp_z_p_zz)                 \
  V(fminnmv_asimdall_only_h)        \
  V(fminnmv_asimdall_only_sd)       \
  V(fminnmv_v_p_z)                  \
  V(fminp_asimdsame_only)           \
  V(fminp_asimdsamefp16_only)       \
  V(fminp_asisdpair_only_h)         \
  V(fminp_asisdpair_only_sd)        \
  V(fminp_z_p_zz)                   \
  V(fminv_asimdall_only_h)          \
  V(fminv_asimdall_only_sd)         \
  V(fminv_v_p_z)                    \
  V(fmla_asimdelem_r_sd)            \
  V(fmla_asimdelem_rh_h)            \
  V(fmla_asimdsame_only)            \
  V(fmla_asimdsamefp16_only)        \
  V(fmla_asisdelem_r_sd)            \
  V(fmla_asisdelem_rh_h)            \
  V(fmla_z_p_zzz)                   \
  V(fmla_z_zzzi_d)                  \
  V(fmla_z_zzzi_h)                  \
  V(fmla_z_zzzi_s)                  \
  V(fmlal2_asimdelem_lh)            \
  V(fmlal2_asimdsame_f)             \
  V(fmlal_asimdelem_lh)             \
  V(fmlal_asimdsame_f)              \
  V(fmlalb_z_zzz)                   \
  V(fmlalb_z_zzzi_s)                \
  V(fmlalt_z_zzz)                   \
  V(fmlalt_z_zzzi_s)                \
  V(fmls_asimdelem_r_sd)            \
  V(fmls_asimdelem_rh_h)            \
  V(fmls_asimdsame_only)            \
  V(fmls_asimdsamefp16_only)        \
  V(fmls_asisdelem_r_sd)            \
  V(fmls_asisdelem_rh_h)            \
  V(fmls_z_p_zzz)                   \
  V(fmls_z_zzzi_d)                  \
  V(fmls_z_zzzi_h)                  \
  V(fmls_z_zzzi_s)                  \
  V(fmlsl2_asimdelem_lh)            \
  V(fmlsl2_asimdsame_f)             \
  V(fmlsl_asimdelem_lh)             \
  V(fmlsl_asimdsame_f)              \
  V(fmlslb_z_zzz)                   \
  V(fmlslb_z_zzzi_s)                \
  V(fmlslt_z_zzz)                   \
  V(fmlslt_z_zzzi_s)                \
  V(fmmla_z_zzz_d)                  \
  V(fmmla_z_zzz_s)                  \
  V(fmov_32h_float2int)             \
  V(fmov_32s_float2int)             \
  V(fmov_64d_float2int)             \
  V(fmov_64h_float2int)             \
  V(fmov_64vx_float2int)            \
  V(fmov_asimdimm_d2_d)             \
  V(fmov_asimdimm_h_h)              \
  V(fmov_asimdimm_s_s)              \
  V(fmov_d64_float2int)             \
  V(fmov_d_floatdp1)                \
  V(fmov_d_floatimm)                \
  V(fmov_h32_float2int)             \
  V(fmov_h64_float2int)             \
  V(fmov_h_floatdp1)                \
  V(fmov_h_floatimm)                \
  V(fmov_s32_float2int)             \
  V(fmov_s_floatdp1)                \
  V(fmov_s_floatimm)                \
  V(fmov_v64i_float2int)            \
  V(fmsb_z_p_zzz)                   \
  V(fmsub_d_floatdp3)               \
  V(fmsub_h_floatdp3)               \
  V(fmsub_s_floatdp3)               \
  V(fmul_asimdelem_r_sd)            \
  V(fmul_asimdelem_rh_h)            \
  V(fmul_asimdsame_only)            \
  V(fmul_asimdsamefp16_only)        \
  V(fmul_asisdelem_r_sd)            \
  V(fmul_asisdelem_rh_h)            \
  V(fmul_d_floatdp2)                \
  V(fmul_h_floatdp2)                \
  V(fmul_s_floatdp2)                \
  V(fmul_z_p_zs)                    \
  V(fmul_z_p_zz)                    \
  V(fmul_z_zz)                      \
  V(fmul_z_zzi_d)                   \
  V(fmul_z_zzi_h)                   \
  V(fmul_z_zzi_s)                   \
  V(fmulx_asimdelem_r_sd)           \
  V(fmulx_asimdelem_rh_h)           \
  V(fmulx_asimdsame_only)           \
  V(fmulx_asimdsamefp16_only)       \
  V(fmulx_asisdelem_r_sd)           \
  V(fmulx_asisdelem_rh_h)           \
  V(fmulx_asisdsame_only)           \
  V(fmulx_asisdsamefp16_only)       \
  V(fmulx_z_p_zz)                   \
  V(fneg_asimdmisc_r)               \
  V(fneg_asimdmiscfp16_r)           \
  V(fneg_d_floatdp1)                \
  V(fneg_h_floatdp1)                \
  V(fneg_s_floatdp1)                \
  V(fneg_z_p_z)                     \
  V(fnmad_z_p_zzz)                  \
  V(fnmadd_d_floatdp3)              \
  V(fnmadd_h_floatdp3)              \
  V(fnmadd_s_floatdp3)              \
  V(fnmla_z_p_zzz)                  \
  V(fnmls_z_p_zzz)                  \
  V(fnmsb_z_p_zzz)                  \
  V(fnmsub_d_floatdp3)              \
  V(fnmsub_h_floatdp3)              \
  V(fnmsub_s_floatdp3)              \
  V(fnmul_d_floatdp2)               \
  V(fnmul_h_floatdp2)               \
  V(fnmul_s_floatdp2)               \
  V(frecpe_asimdmisc_r)             \
  V(frecpe_asimdmiscfp16_r)         \
  V(frecpe_asisdmisc_r)             \
  V(frecpe_asisdmiscfp16_r)         \
  V(frecpe_z_z)                     \
  V(frecps_asimdsame_only)          \
  V(frecps_asimdsamefp16_only)      \
  V(frecps_asisdsame_only)          \
  V(frecps_asisdsamefp16_only)      \
  V(frecps_z_zz)                    \
  V(frecpx_asisdmisc_r)             \
  V(frecpx_asisdmiscfp16_r)         \
  V(frecpx_z_p_z)                   \
  V(frint32x_asimdmisc_r)           \
  V(frint32x_d_floatdp1)            \
  V(frint32x_s_floatdp1)            \
  V(frint32z_asimdmisc_r)           \
  V(frint32z_d_floatdp1)            \
  V(frint32z_s_floatdp1)            \
  V(frint64x_asimdmisc_r)           \
  V(frint64x_d_floatdp1)            \
  V(frint64x_s_floatdp1)            \
  V(frint64z_asimdmisc_r)           \
  V(frint64z_d_floatdp1)            \
  V(frint64z_s_floatdp1)            \
  V(frinta_asimdmisc_r)             \
  V(frinta_asimdmiscfp16_r)         \
  V(frinta_d_floatdp1)              \
  V(frinta_h_floatdp1)              \
  V(frinta_s_floatdp1)              \
  V(frinta_z_p_z)                   \
  V(frinti_asimdmisc_r)             \
  V(frinti_asimdmiscfp16_r)         \
  V(frinti_d_floatdp1)              \
  V(frinti_h_floatdp1)              \
  V(frinti_s_floatdp1)              \
  V(frinti_z_p_z)                   \
  V(frintm_asimdmisc_r)             \
  V(frintm_asimdmiscfp16_r)         \
  V(frintm_d_floatdp1)              \
  V(frintm_h_floatdp1)              \
  V(frintm_s_floatdp1)              \
  V(frintm_z_p_z)                   \
  V(frintn_asimdmisc_r)             \
  V(frintn_asimdmiscfp16_r)         \
  V(frintn_d_floatdp1)              \
  V(frintn_h_floatdp1)              \
  V(frintn_s_floatdp1)              \
  V(frintn_z_p_z)                   \
  V(frintp_asimdmisc_r)             \
  V(frintp_asimdmiscfp16_r)         \
  V(frintp_d_floatdp1)              \
  V(frintp_h_floatdp1)              \
  V(frintp_s_floatdp1)              \
  V(frintp_z_p_z)                   \
  V(frintx_asimdmisc_r)             \
  V(frintx_asimdmiscfp16_r)         \
  V(frintx_d_floatdp1)              \
  V(frintx_h_floatdp1)              \
  V(frintx_s_floatdp1)              \
  V(frintx_z_p_z)                   \
  V(frintz_asimdmisc_r)             \
  V(frintz_asimdmiscfp16_r)         \
  V(frintz_d_floatdp1)              \
  V(frintz_h_floatdp1)              \
  V(frintz_s_floatdp1)              \
  V(frintz_z_p_z)                   \
  V(frsqrte_asimdmisc_r)            \
  V(frsqrte_asimdmiscfp16_r)        \
  V(frsqrte_asisdmisc_r)            \
  V(frsqrte_asisdmiscfp16_r)        \
  V(frsqrte_z_z)                    \
  V(frsqrts_asimdsame_only)         \
  V(frsqrts_asimdsamefp16_only)     \
  V(frsqrts_asisdsame_only)         \
  V(frsqrts_asisdsamefp16_only)     \
  V(frsqrts_z_zz)                   \
  V(fscale_z_p_zz)                  \
  V(fsqrt_asimdmisc_r)              \
  V(fsqrt_asimdmiscfp16_r)          \
  V(fsqrt_d_floatdp1)               \
  V(fsqrt_h_floatdp1)               \
  V(fsqrt_s_floatdp1)               \
  V(fsqrt_z_p_z)                    \
  V(fsub_asimdsame_only)            \
  V(fsub_asimdsamefp16_only)        \
  V(fsub_d_floatdp2)                \
  V(fsub_h_floatdp2)                \
  V(fsub_s_floatdp2)                \
  V(fsub_z_p_zs)                    \
  V(fsub_z_p_zz)                    \
  V(fsub_z_zz)                      \
  V(fsubr_z_p_zs)                   \
  V(fsubr_z_p_zz)                   \
  V(ftmad_z_zzi)                    \
  V(ftsmul_z_zz)                    \
  V(ftssel_z_zz)                    \
  V(gmi_64g_dp_2src)                \
  V(hint_hm_hints)                  \
  V(histcnt_z_p_zz)                 \
  V(histseg_z_zz)                   \
  V(hlt_ex_exception)               \
  V(hvc_ex_exception)               \
  V(incb_r_rs)                      \
  V(incd_r_rs)                      \
  V(incd_z_zs)                      \
  V(inch_r_rs)                      \
  V(inch_z_zs)                      \
  V(incp_r_p_r)                     \
  V(incp_z_p_z)                     \
  V(incw_r_rs)                      \
  V(incw_z_zs)                      \
  V(index_z_ii)                     \
  V(index_z_ir)                     \
  V(index_z_ri)                     \
  V(index_z_rr)                     \
  V(ins_asimdins_ir_r)              \
  V(ins_asimdins_iv_v)              \
  V(insr_z_r)                       \
  V(insr_z_v)                       \
  V(irg_64i_dp_2src)                \
  V(isb_bi_barriers)                \
  V(lasta_r_p_z)                    \
  V(lasta_v_p_z)                    \
  V(lastb_r_p_z)                    \
  V(lastb_v_p_z)                    \
  V(ld1_asisdlse_r1_1v)             \
  V(ld1_asisdlse_r2_2v)             \
  V(ld1_asisdlse_r3_3v)             \
  V(ld1_asisdlse_r4_4v)             \
  V(ld1_asisdlsep_i1_i1)            \
  V(ld1_asisdlsep_i2_i2)            \
  V(ld1_asisdlsep_i3_i3)            \
  V(ld1_asisdlsep_i4_i4)            \
  V(ld1_asisdlsep_r1_r1)            \
  V(ld1_asisdlsep_r2_r2)            \
  V(ld1_asisdlsep_r3_r3)            \
  V(ld1_asisdlsep_r4_r4)            \
  V(ld1_asisdlso_b1_1b)             \
  V(ld1_asisdlso_d1_1d)             \
  V(ld1_asisdlso_h1_1h)             \
  V(ld1_asisdlso_s1_1s)             \
  V(ld1_asisdlsop_b1_i1b)           \
  V(ld1_asisdlsop_bx1_r1b)          \
  V(ld1_asisdlsop_d1_i1d)           \
  V(ld1_asisdlsop_dx1_r1d)          \
  V(ld1_asisdlsop_h1_i1h)           \
  V(ld1_asisdlsop_hx1_r1h)          \
  V(ld1_asisdlsop_s1_i1s)           \
  V(ld1_asisdlsop_sx1_r1s)          \
  V(ld1b_z_p_ai_d)                  \
  V(ld1b_z_p_ai_s)                  \
  V(ld1b_z_p_bi_u16)                \
  V(ld1b_z_p_bi_u32)                \
  V(ld1b_z_p_bi_u64)                \
  V(ld1b_z_p_bi_u8)                 \
  V(ld1b_z_p_br_u16)                \
  V(ld1b_z_p_br_u32)                \
  V(ld1b_z_p_br_u64)                \
  V(ld1b_z_p_br_u8)                 \
  V(ld1b_z_p_bz_d_64_unscaled)      \
  V(ld1b_z_p_bz_d_x32_unscaled)     \
  V(ld1b_z_p_bz_s_x32_unscaled)     \
  V(ld1d_z_p_ai_d)                  \
  V(ld1d_z_p_bi_u64)                \
  V(ld1d_z_p_br_u64)                \
  V(ld1d_z_p_bz_d_64_scaled)        \
  V(ld1d_z_p_bz_d_64_unscaled)      \
  V(ld1d_z_p_bz_d_x32_scaled)       \
  V(ld1d_z_p_bz_d_x32_unscaled)     \
  V(ld1h_z_p_ai_d)                  \
  V(ld1h_z_p_ai_s)                  \
  V(ld1h_z_p_bi_u16)                \
  V(ld1h_z_p_bi_u32)                \
  V(ld1h_z_p_bi_u64)                \
  V(ld1h_z_p_br_u16)                \
  V(ld1h_z_p_br_u32)                \
  V(ld1h_z_p_br_u64)                \
  V(ld1h_z_p_bz_d_64_scaled)        \
  V(ld1h_z_p_bz_d_64_unscaled)      \
  V(ld1h_z_p_bz_d_x32_scaled)       \
  V(ld1h_z_p_bz_d_x32_unscaled)     \
  V(ld1h_z_p_bz_s_x32_scaled)       \
  V(ld1h_z_p_bz_s_x32_unscaled)     \
  V(ld1r_asisdlso_r1)               \
  V(ld1r_asisdlsop_r1_i)            \
  V(ld1r_asisdlsop_rx1_r)           \
  V(ld1rb_z_p_bi_u16)               \
  V(ld1rb_z_p_bi_u32)               \
  V(ld1rb_z_p_bi_u64)               \
  V(ld1rb_z_p_bi_u8)                \
  V(ld1rd_z_p_bi_u64)               \
  V(ld1rh_z_p_bi_u16)               \
  V(ld1rh_z_p_bi_u32)               \
  V(ld1rh_z_p_bi_u64)               \
  V(ld1rob_z_p_bi_u8)               \
  V(ld1rob_z_p_br_contiguous)       \
  V(ld1rod_z_p_bi_u64)              \
  V(ld1rod_z_p_br_contiguous)       \
  V(ld1roh_z_p_bi_u16)              \
  V(ld1roh_z_p_br_contiguous)       \
  V(ld1row_z_p_bi_u32)              \
  V(ld1row_z_p_br_contiguous)       \
  V(ld1rqb_z_p_bi_u8)               \
  V(ld1rqb_z_p_br_contiguous)       \
  V(ld1rqd_z_p_bi_u64)              \
  V(ld1rqd_z_p_br_contiguous)       \
  V(ld1rqh_z_p_bi_u16)              \
  V(ld1rqh_z_p_br_contiguous)       \
  V(ld1rqw_z_p_bi_u32)              \
  V(ld1rqw_z_p_br_contiguous)       \
  V(ld1rsb_z_p_bi_s16)              \
  V(ld1rsb_z_p_bi_s32)              \
  V(ld1rsb_z_p_bi_s64)              \
  V(ld1rsh_z_p_bi_s32)              \
  V(ld1rsh_z_p_bi_s64)              \
  V(ld1rsw_z_p_bi_s64)              \
  V(ld1rw_z_p_bi_u32)               \
  V(ld1rw_z_p_bi_u64)               \
  V(ld1sb_z_p_ai_d)                 \
  V(ld1sb_z_p_ai_s)                 \
  V(ld1sb_z_p_bi_s16)               \
  V(ld1sb_z_p_bi_s32)               \
  V(ld1sb_z_p_bi_s64)               \
  V(ld1sb_z_p_br_s16)               \
  V(ld1sb_z_p_br_s32)               \
  V(ld1sb_z_p_br_s64)               \
  V(ld1sb_z_p_bz_d_64_unscaled)     \
  V(ld1sb_z_p_bz_d_x32_unscaled)    \
  V(ld1sb_z_p_bz_s_x32_unscaled)    \
  V(ld1sh_z_p_ai_d)                 \
  V(ld1sh_z_p_ai_s)                 \
  V(ld1sh_z_p_bi_s32)               \
  V(ld1sh_z_p_bi_s64)               \
  V(ld1sh_z_p_br_s32)               \
  V(ld1sh_z_p_br_s64)               \
  V(ld1sh_z_p_bz_d_64_scaled)       \
  V(ld1sh_z_p_bz_d_64_unscaled)     \
  V(ld1sh_z_p_bz_d_x32_scaled)      \
  V(ld1sh_z_p_bz_d_x32_unscaled)    \
  V(ld1sh_z_p_bz_s_x32_scaled)      \
  V(ld1sh_z_p_bz_s_x32_unscaled)    \
  V(ld1sw_z_p_ai_d)                 \
  V(ld1sw_z_p_bi_s64)               \
  V(ld1sw_z_p_br_s64)               \
  V(ld1sw_z_p_bz_d_64_scaled)       \
  V(ld1sw_z_p_bz_d_64_unscaled)     \
  V(ld1sw_z_p_bz_d_x32_scaled)      \
  V(ld1sw_z_p_bz_d_x32_unscaled)    \
  V(ld1w_z_p_ai_d)                  \
  V(ld1w_z_p_ai_s)                  \
  V(ld1w_z_p_bi_u32)                \
  V(ld1w_z_p_bi_u64)                \
  V(ld1w_z_p_br_u32)                \
  V(ld1w_z_p_br_u64)                \
  V(ld1w_z_p_bz_d_64_scaled)        \
  V(ld1w_z_p_bz_d_64_unscaled)      \
  V(ld1w_z_p_bz_d_x32_scaled)       \
  V(ld1w_z_p_bz_d_x32_unscaled)     \
  V(ld1w_z_p_bz_s_x32_scaled)       \
  V(ld1w_z_p_bz_s_x32_unscaled)     \
  V(ld2_asisdlse_r2)                \
  V(ld2_asisdlsep_i2_i)             \
  V(ld2_asisdlsep_r2_r)             \
  V(ld2_asisdlso_b2_2b)             \
  V(ld2_asisdlso_d2_2d)             \
  V(ld2_asisdlso_h2_2h)             \
  V(ld2_asisdlso_s2_2s)             \
  V(ld2_asisdlsop_b2_i2b)           \
  V(ld2_asisdlsop_bx2_r2b)          \
  V(ld2_asisdlsop_d2_i2d)           \
  V(ld2_asisdlsop_dx2_r2d)          \
  V(ld2_asisdlsop_h2_i2h)           \
  V(ld2_asisdlsop_hx2_r2h)          \
  V(ld2_asisdlsop_s2_i2s)           \
  V(ld2_asisdlsop_sx2_r2s)          \
  V(ld2b_z_p_bi_contiguous)         \
  V(ld2b_z_p_br_contiguous)         \
  V(ld2d_z_p_bi_contiguous)         \
  V(ld2d_z_p_br_contiguous)         \
  V(ld2h_z_p_bi_contiguous)         \
  V(ld2h_z_p_br_contiguous)         \
  V(ld2r_asisdlso_r2)               \
  V(ld2r_asisdlsop_r2_i)            \
  V(ld2r_asisdlsop_rx2_r)           \
  V(ld2w_z_p_bi_contiguous)         \
  V(ld2w_z_p_br_contiguous)         \
  V(ld3_asisdlse_r3)                \
  V(ld3_asisdlsep_i3_i)             \
  V(ld3_asisdlsep_r3_r)             \
  V(ld3_asisdlso_b3_3b)             \
  V(ld3_asisdlso_d3_3d)             \
  V(ld3_asisdlso_h3_3h)             \
  V(ld3_asisdlso_s3_3s)             \
  V(ld3_asisdlsop_b3_i3b)           \
  V(ld3_asisdlsop_bx3_r3b)          \
  V(ld3_asisdlsop_d3_i3d)           \
  V(ld3_asisdlsop_dx3_r3d)          \
  V(ld3_asisdlsop_h3_i3h)           \
  V(ld3_asisdlsop_hx3_r3h)          \
  V(ld3_asisdlsop_s3_i3s)           \
  V(ld3_asisdlsop_sx3_r3s)          \
  V(ld3b_z_p_bi_contiguous)         \
  V(ld3b_z_p_br_contiguous)         \
  V(ld3d_z_p_bi_contiguous)         \
  V(ld3d_z_p_br_contiguous)         \
  V(ld3h_z_p_bi_contiguous)         \
  V(ld3h_z_p_br_contiguous)         \
  V(ld3r_asisdlso_r3)               \
  V(ld3r_asisdlsop_r3_i)            \
  V(ld3r_asisdlsop_rx3_r)           \
  V(ld3w_z_p_bi_contiguous)         \
  V(ld3w_z_p_br_contiguous)         \
  V(ld4_asisdlse_r4)                \
  V(ld4_asisdlsep_i4_i)             \
  V(ld4_asisdlsep_r4_r)             \
  V(ld4_asisdlso_b4_4b)             \
  V(ld4_asisdlso_d4_4d)             \
  V(ld4_asisdlso_h4_4h)             \
  V(ld4_asisdlso_s4_4s)             \
  V(ld4_asisdlsop_b4_i4b)           \
  V(ld4_asisdlsop_bx4_r4b)          \
  V(ld4_asisdlsop_d4_i4d)           \
  V(ld4_asisdlsop_dx4_r4d)          \
  V(ld4_asisdlsop_h4_i4h)           \
  V(ld4_asisdlsop_hx4_r4h)          \
  V(ld4_asisdlsop_s4_i4s)           \
  V(ld4_asisdlsop_sx4_r4s)          \
  V(ld4b_z_p_bi_contiguous)         \
  V(ld4b_z_p_br_contiguous)         \
  V(ld4d_z_p_bi_contiguous)         \
  V(ld4d_z_p_br_contiguous)         \
  V(ld4h_z_p_bi_contiguous)         \
  V(ld4h_z_p_br_contiguous)         \
  V(ld4r_asisdlso_r4)               \
  V(ld4r_asisdlsop_r4_i)            \
  V(ld4r_asisdlsop_rx4_r)           \
  V(ld4w_z_p_bi_contiguous)         \
  V(ld4w_z_p_br_contiguous)         \
  V(ld64b_64l_memop)                \
  V(ldadd_32_memop)                 \
  V(ldadd_64_memop)                 \
  V(ldadda_32_memop)                \
  V(ldadda_64_memop)                \
  V(ldaddab_32_memop)               \
  V(ldaddah_32_memop)               \
  V(ldaddal_32_memop)               \
  V(ldaddal_64_memop)               \
  V(ldaddalb_32_memop)              \
  V(ldaddalh_32_memop)              \
  V(ldaddb_32_memop)                \
  V(ldaddh_32_memop)                \
  V(ldaddl_32_memop)                \
  V(ldaddl_64_memop)                \
  V(ldaddlb_32_memop)               \
  V(ldaddlh_32_memop)               \
  V(ldapr_32l_memop)                \
  V(ldapr_64l_memop)                \
  V(ldaprb_32l_memop)               \
  V(ldaprh_32l_memop)               \
  V(ldapur_32_ldapstl_unscaled)     \
  V(ldapur_64_ldapstl_unscaled)     \
  V(ldapurb_32_ldapstl_unscaled)    \
  V(ldapurh_32_ldapstl_unscaled)    \
  V(ldapursb_32_ldapstl_unscaled)   \
  V(ldapursb_64_ldapstl_unscaled)   \
  V(ldapursh_32_ldapstl_unscaled)   \
  V(ldapursh_64_ldapstl_unscaled)   \
  V(ldapursw_64_ldapstl_unscaled)   \
  V(ldar_lr32_ldstexcl)             \
  V(ldar_lr64_ldstexcl)             \
  V(ldarb_lr32_ldstexcl)            \
  V(ldarh_lr32_ldstexcl)            \
  V(ldaxp_lp32_ldstexcl)            \
  V(ldaxp_lp64_ldstexcl)            \
  V(ldaxr_lr32_ldstexcl)            \
  V(ldaxr_lr64_ldstexcl)            \
  V(ldaxrb_lr32_ldstexcl)           \
  V(ldaxrh_lr32_ldstexcl)           \
  V(ldclr_32_memop)                 \
  V(ldclr_64_memop)                 \
  V(ldclra_32_memop)                \
  V(ldclra_64_memop)                \
  V(ldclrab_32_memop)               \
  V(ldclrah_32_memop)               \
  V(ldclral_32_memop)               \
  V(ldclral_64_memop)               \
  V(ldclralb_32_memop)              \
  V(ldclralh_32_memop)              \
  V(ldclrb_32_memop)                \
  V(ldclrh_32_memop)                \
  V(ldclrl_32_memop)                \
  V(ldclrl_64_memop)                \
  V(ldclrlb_32_memop)               \
  V(ldclrlh_32_memop)               \
  V(ldeor_32_memop)                 \
  V(ldeor_64_memop)                 \
  V(ldeora_32_memop)                \
  V(ldeora_64_memop)                \
  V(ldeorab_32_memop)               \
  V(ldeorah_32_memop)               \
  V(ldeoral_32_memop)               \
  V(ldeoral_64_memop)               \
  V(ldeoralb_32_memop)              \
  V(ldeoralh_32_memop)              \
  V(ldeorb_32_memop)                \
  V(ldeorh_32_memop)                \
  V(ldeorl_32_memop)                \
  V(ldeorl_64_memop)                \
  V(ldeorlb_32_memop)               \
  V(ldeorlh_32_memop)               \
  V(ldff1b_z_p_ai_d)                \
  V(ldff1b_z_p_ai_s)                \
  V(ldff1b_z_p_br_u16)              \
  V(ldff1b_z_p_br_u32)              \
  V(ldff1b_z_p_br_u64)              \
  V(ldff1b_z_p_br_u8)               \
  V(ldff1b_z_p_bz_d_64_unscaled)    \
  V(ldff1b_z_p_bz_d_x32_unscaled)   \
  V(ldff1b_z_p_bz_s_x32_unscaled)   \
  V(ldff1d_z_p_ai_d)                \
  V(ldff1d_z_p_br_u64)              \
  V(ldff1d_z_p_bz_d_64_scaled)      \
  V(ldff1d_z_p_bz_d_64_unscaled)    \
  V(ldff1d_z_p_bz_d_x32_scaled)     \
  V(ldff1d_z_p_bz_d_x32_unscaled)   \
  V(ldff1h_z_p_ai_d)                \
  V(ldff1h_z_p_ai_s)                \
  V(ldff1h_z_p_br_u16)              \
  V(ldff1h_z_p_br_u32)              \
  V(ldff1h_z_p_br_u64)              \
  V(ldff1h_z_p_bz_d_64_scaled)      \
  V(ldff1h_z_p_bz_d_64_unscaled)    \
  V(ldff1h_z_p_bz_d_x32_scaled)     \
  V(ldff1h_z_p_bz_d_x32_unscaled)   \
  V(ldff1h_z_p_bz_s_x32_scaled)     \
  V(ldff1h_z_p_bz_s_x32_unscaled)   \
  V(ldff1sb_z_p_ai_d)               \
  V(ldff1sb_z_p_ai_s)               \
  V(ldff1sb_z_p_br_s16)             \
  V(ldff1sb_z_p_br_s32)             \
  V(ldff1sb_z_p_br_s64)             \
  V(ldff1sb_z_p_bz_d_64_unscaled)   \
  V(ldff1sb_z_p_bz_d_x32_unscaled)  \
  V(ldff1sb_z_p_bz_s_x32_unscaled)  \
  V(ldff1sh_z_p_ai_d)               \
  V(ldff1sh_z_p_ai_s)               \
  V(ldff1sh_z_p_br_s32)             \
  V(ldff1sh_z_p_br_s64)             \
  V(ldff1sh_z_p_bz_d_64_scaled)     \
  V(ldff1sh_z_p_bz_d_64_unscaled)   \
  V(ldff1sh_z_p_bz_d_x32_scaled)    \
  V(ldff1sh_z_p_bz_d_x32_unscaled)  \
  V(ldff1sh_z_p_bz_s_x32_scaled)    \
  V(ldff1sh_z_p_bz_s_x32_unscaled)  \
  V(ldff1sw_z_p_ai_d)               \
  V(ldff1sw_z_p_br_s64)             \
  V(ldff1sw_z_p_bz_d_64_scaled)     \
  V(ldff1sw_z_p_bz_d_64_unscaled)   \
  V(ldff1sw_z_p_bz_d_x32_scaled)    \
  V(ldff1sw_z_p_bz_d_x32_unscaled)  \
  V(ldff1w_z_p_ai_d)                \
  V(ldff1w_z_p_ai_s)                \
  V(ldff1w_z_p_br_u32)              \
  V(ldff1w_z_p_br_u64)              \
  V(ldff1w_z_p_bz_d_64_scaled)      \
  V(ldff1w_z_p_bz_d_64_unscaled)    \
  V(ldff1w_z_p_bz_d_x32_scaled)     \
  V(ldff1w_z_p_bz_d_x32_unscaled)   \
  V(ldff1w_z_p_bz_s_x32_scaled)     \
  V(ldff1w_z_p_bz_s_x32_unscaled)   \
  V(ldg_64loffset_ldsttags)         \
  V(ldgm_64bulk_ldsttags)           \
  V(ldlar_lr32_ldstexcl)            \
  V(ldlar_lr64_ldstexcl)            \
  V(ldlarb_lr32_ldstexcl)           \
  V(ldlarh_lr32_ldstexcl)           \
  V(ldnf1b_z_p_bi_u16)              \
  V(ldnf1b_z_p_bi_u32)              \
  V(ldnf1b_z_p_bi_u64)              \
  V(ldnf1b_z_p_bi_u8)               \
  V(ldnf1d_z_p_bi_u64)              \
  V(ldnf1h_z_p_bi_u16)              \
  V(ldnf1h_z_p_bi_u32)              \
  V(ldnf1h_z_p_bi_u64)              \
  V(ldnf1sb_z_p_bi_s16)             \
  V(ldnf1sb_z_p_bi_s32)             \
  V(ldnf1sb_z_p_bi_s64)             \
  V(ldnf1sh_z_p_bi_s32)             \
  V(ldnf1sh_z_p_bi_s64)             \
  V(ldnf1sw_z_p_bi_s64)             \
  V(ldnf1w_z_p_bi_u32)              \
  V(ldnf1w_z_p_bi_u64)              \
  V(ldnp_32_ldstnapair_offs)        \
  V(ldnp_64_ldstnapair_offs)        \
  V(ldnp_d_ldstnapair_offs)         \
  V(ldnp_q_ldstnapair_offs)         \
  V(ldnp_s_ldstnapair_offs)         \
  V(ldnt1b_z_p_ar_d_64_unscaled)    \
  V(ldnt1b_z_p_ar_s_x32_unscaled)   \
  V(ldnt1b_z_p_bi_contiguous)       \
  V(ldnt1b_z_p_br_contiguous)       \
  V(ldnt1d_z_p_ar_d_64_unscaled)    \
  V(ldnt1d_z_p_bi_contiguous)       \
  V(ldnt1d_z_p_br_contiguous)       \
  V(ldnt1h_z_p_ar_d_64_unscaled)    \
  V(ldnt1h_z_p_ar_s_x32_unscaled)   \
  V(ldnt1h_z_p_bi_contiguous)       \
  V(ldnt1h_z_p_br_contiguous)       \
  V(ldnt1sb_z_p_ar_d_64_unscaled)   \
  V(ldnt1sb_z_p_ar_s_x32_unscaled)  \
  V(ldnt1sh_z_p_ar_d_64_unscaled)   \
  V(ldnt1sh_z_p_ar_s_x32_unscaled)  \
  V(ldnt1sw_z_p_ar_d_64_unscaled)   \
  V(ldnt1w_z_p_ar_d_64_unscaled)    \
  V(ldnt1w_z_p_ar_s_x32_unscaled)   \
  V(ldnt1w_z_p_bi_contiguous)       \
  V(ldnt1w_z_p_br_contiguous)       \
  V(ldp_32_ldstpair_off)            \
  V(ldp_32_ldstpair_post)           \
  V(ldp_32_ldstpair_pre)            \
  V(ldp_64_ldstpair_off)            \
  V(ldp_64_ldstpair_post)           \
  V(ldp_64_ldstpair_pre)            \
  V(ldp_d_ldstpair_off)             \
  V(ldp_d_ldstpair_post)            \
  V(ldp_d_ldstpair_pre)             \
  V(ldp_q_ldstpair_off)             \
  V(ldp_q_ldstpair_post)            \
  V(ldp_q_ldstpair_pre)             \
  V(ldp_s_ldstpair_off)             \
  V(ldp_s_ldstpair_post)            \
  V(ldp_s_ldstpair_pre)             \
  V(ldpsw_64_ldstpair_off)          \
  V(ldpsw_64_ldstpair_post)         \
  V(ldpsw_64_ldstpair_pre)          \
  V(ldr_32_ldst_immpost)            \
  V(ldr_32_ldst_immpre)             \
  V(ldr_32_ldst_pos)                \
  V(ldr_32_ldst_regoff)             \
  V(ldr_32_loadlit)                 \
  V(ldr_64_ldst_immpost)            \
  V(ldr_64_ldst_immpre)             \
  V(ldr_64_ldst_pos)                \
  V(ldr_64_ldst_regoff)             \
  V(ldr_64_loadlit)                 \
  V(ldr_b_ldst_immpost)             \
  V(ldr_b_ldst_immpre)              \
  V(ldr_b_ldst_pos)                 \
  V(ldr_b_ldst_regoff)              \
  V(ldr_bl_ldst_regoff)             \
  V(ldr_d_ldst_immpost)             \
  V(ldr_d_ldst_immpre)              \
  V(ldr_d_ldst_pos)                 \
  V(ldr_d_ldst_regoff)              \
  V(ldr_d_loadlit)                  \
  V(ldr_h_ldst_immpost)             \
  V(ldr_h_ldst_immpre)              \
  V(ldr_h_ldst_pos)                 \
  V(ldr_h_ldst_regoff)              \
  V(ldr_p_bi)                       \
  V(ldr_q_ldst_immpost)             \
  V(ldr_q_ldst_immpre)              \
  V(ldr_q_ldst_pos)                 \
  V(ldr_q_ldst_regoff)              \
  V(ldr_q_loadlit)                  \
  V(ldr_s_ldst_immpost)             \
  V(ldr_s_ldst_immpre)              \
  V(ldr_s_ldst_pos)                 \
  V(ldr_s_ldst_regoff)              \
  V(ldr_s_loadlit)                  \
  V(ldr_z_bi)                       \
  V(ldraa_64_ldst_pac)              \
  V(ldraa_64w_ldst_pac)             \
  V(ldrab_64_ldst_pac)              \
  V(ldrab_64w_ldst_pac)             \
  V(ldrb_32_ldst_immpost)           \
  V(ldrb_32_ldst_immpre)            \
  V(ldrb_32_ldst_pos)               \
  V(ldrb_32b_ldst_regoff)           \
  V(ldrb_32bl_ldst_regoff)          \
  V(ldrh_32_ldst_immpost)           \
  V(ldrh_32_ldst_immpre)            \
  V(ldrh_32_ldst_pos)               \
  V(ldrh_32_ldst_regoff)            \
  V(ldrsb_32_ldst_immpost)          \
  V(ldrsb_32_ldst_immpre)           \
  V(ldrsb_32_ldst_pos)              \
  V(ldrsb_32b_ldst_regoff)          \
  V(ldrsb_32bl_ldst_regoff)         \
  V(ldrsb_64_ldst_immpost)          \
  V(ldrsb_64_ldst_immpre)           \
  V(ldrsb_64_ldst_pos)              \
  V(ldrsb_64b_ldst_regoff)          \
  V(ldrsb_64bl_ldst_regoff)         \
  V(ldrsh_32_ldst_immpost)          \
  V(ldrsh_32_ldst_immpre)           \
  V(ldrsh_32_ldst_pos)              \
  V(ldrsh_32_ldst_regoff)           \
  V(ldrsh_64_ldst_immpost)          \
  V(ldrsh_64_ldst_immpre)           \
  V(ldrsh_64_ldst_pos)              \
  V(ldrsh_64_ldst_regoff)           \
  V(ldrsw_64_ldst_immpost)          \
  V(ldrsw_64_ldst_immpre)           \
  V(ldrsw_64_ldst_pos)              \
  V(ldrsw_64_ldst_regoff)           \
  V(ldrsw_64_loadlit)               \
  V(ldset_32_memop)                 \
  V(ldset_64_memop)                 \
  V(ldseta_32_memop)                \
  V(ldseta_64_memop)                \
  V(ldsetab_32_memop)               \
  V(ldsetah_32_memop)               \
  V(ldsetal_32_memop)               \
  V(ldsetal_64_memop)               \
  V(ldsetalb_32_memop)              \
  V(ldsetalh_32_memop)              \
  V(ldsetb_32_memop)                \
  V(ldseth_32_memop)                \
  V(ldsetl_32_memop)                \
  V(ldsetl_64_memop)                \
  V(ldsetlb_32_memop)               \
  V(ldsetlh_32_memop)               \
  V(ldsmax_32_memop)                \
  V(ldsmax_64_memop)                \
  V(ldsmaxa_32_memop)               \
  V(ldsmaxa_64_memop)               \
  V(ldsmaxab_32_memop)              \
  V(ldsmaxah_32_memop)              \
  V(ldsmaxal_32_memop)              \
  V(ldsmaxal_64_memop)              \
  V(ldsmaxalb_32_memop)             \
  V(ldsmaxalh_32_memop)             \
  V(ldsmaxb_32_memop)               \
  V(ldsmaxh_32_memop)               \
  V(ldsmaxl_32_memop)               \
  V(ldsmaxl_64_memop)               \
  V(ldsmaxlb_32_memop)              \
  V(ldsmaxlh_32_memop)              \
  V(ldsmin_32_memop)                \
  V(ldsmin_64_memop)                \
  V(ldsmina_32_memop)               \
  V(ldsmina_64_memop)               \
  V(ldsminab_32_memop)              \
  V(ldsminah_32_memop)              \
  V(ldsminal_32_memop)              \
  V(ldsminal_64_memop)              \
  V(ldsminalb_32_memop)             \
  V(ldsminalh_32_memop)             \
  V(ldsminb_32_memop)               \
  V(ldsminh_32_memop)               \
  V(ldsminl_32_memop)               \
  V(ldsminl_64_memop)               \
  V(ldsminlb_32_memop)              \
  V(ldsminlh_32_memop)              \
  V(ldtr_32_ldst_unpriv)            \
  V(ldtr_64_ldst_unpriv)            \
  V(ldtrb_32_ldst_unpriv)           \
  V(ldtrh_32_ldst_unpriv)           \
  V(ldtrsb_32_ldst_unpriv)          \
  V(ldtrsb_64_ldst_unpriv)          \
  V(ldtrsh_32_ldst_unpriv)          \
  V(ldtrsh_64_ldst_unpriv)          \
  V(ldtrsw_64_ldst_unpriv)          \
  V(ldumax_32_memop)                \
  V(ldumax_64_memop)                \
  V(ldumaxa_32_memop)               \
  V(ldumaxa_64_memop)               \
  V(ldumaxab_32_memop)              \
  V(ldumaxah_32_memop)              \
  V(ldumaxal_32_memop)              \
  V(ldumaxal_64_memop)              \
  V(ldumaxalb_32_memop)             \
  V(ldumaxalh_32_memop)             \
  V(ldumaxb_32_memop)               \
  V(ldumaxh_32_memop)               \
  V(ldumaxl_32_memop)               \
  V(ldumaxl_64_memop)               \
  V(ldumaxlb_32_memop)              \
  V(ldumaxlh_32_memop)              \
  V(ldumin_32_memop)                \
  V(ldumin_64_memop)                \
  V(ldumina_32_memop)               \
  V(ldumina_64_memop)               \
  V(lduminab_32_memop)              \
  V(lduminah_32_memop)              \
  V(lduminal_32_memop)              \
  V(lduminal_64_memop)              \
  V(lduminalb_32_memop)             \
  V(lduminalh_32_memop)             \
  V(lduminb_32_memop)               \
  V(lduminh_32_memop)               \
  V(lduminl_32_memop)               \
  V(lduminl_64_memop)               \
  V(lduminlb_32_memop)              \
  V(lduminlh_32_memop)              \
  V(ldur_32_ldst_unscaled)          \
  V(ldur_64_ldst_unscaled)          \
  V(ldur_b_ldst_unscaled)           \
  V(ldur_d_ldst_unscaled)           \
  V(ldur_h_ldst_unscaled)           \
  V(ldur_q_ldst_unscaled)           \
  V(ldur_s_ldst_unscaled)           \
  V(ldurb_32_ldst_unscaled)         \
  V(ldurh_32_ldst_unscaled)         \
  V(ldursb_32_ldst_unscaled)        \
  V(ldursb_64_ldst_unscaled)        \
  V(ldursh_32_ldst_unscaled)        \
  V(ldursh_64_ldst_unscaled)        \
  V(ldursw_64_ldst_unscaled)        \
  V(ldxp_lp32_ldstexcl)             \
  V(ldxp_lp64_ldstexcl)             \
  V(ldxr_lr32_ldstexcl)             \
  V(ldxr_lr64_ldstexcl)             \
  V(ldxrb_lr32_ldstexcl)            \
  V(ldxrh_lr32_ldstexcl)            \
  V(lsl_z_p_zi)                     \
  V(lsl_z_p_zw)                     \
  V(lsl_z_p_zz)                     \
  V(lsl_z_zi)                       \
  V(lsl_z_zw)                       \
  V(lslr_z_p_zz)                    \
  V(lslv_32_dp_2src)                \
  V(lslv_64_dp_2src)                \
  V(lsr_z_p_zi)                     \
  V(lsr_z_p_zw)                     \
  V(lsr_z_p_zz)                     \
  V(lsr_z_zi)                       \
  V(lsr_z_zw)                       \
  V(lsrr_z_p_zz)                    \
  V(lsrv_32_dp_2src)                \
  V(lsrv_64_dp_2src)                \
  V(mad_z_p_zzz)                    \
  V(madd_32a_dp_3src)               \
  V(madd_64a_dp_3src)               \
  V(match_p_p_zz)                   \
  V(mla_asimdelem_r)                \
  V(mla_asimdsame_only)             \
  V(mla_z_p_zzz)                    \
  V(mla_z_zzzi_d)                   \
  V(mla_z_zzzi_h)                   \
  V(mla_z_zzzi_s)                   \
  V(mls_asimdelem_r)                \
  V(mls_asimdsame_only)             \
  V(mls_z_p_zzz)                    \
  V(mls_z_zzzi_d)                   \
  V(mls_z_zzzi_h)                   \
  V(mls_z_zzzi_s)                   \
  V(movi_asimdimm_d2_d)             \
  V(movi_asimdimm_d_ds)             \
  V(movi_asimdimm_l_hl)             \
  V(movi_asimdimm_l_sl)             \
  V(movi_asimdimm_m_sm)             \
  V(movi_asimdimm_n_b)              \
  V(movk_32_movewide)               \
  V(movk_64_movewide)               \
  V(movn_32_movewide)               \
  V(movn_64_movewide)               \
  V(movprfx_z_p_z)                  \
  V(movprfx_z_z)                    \
  V(movz_32_movewide)               \
  V(movz_64_movewide)               \
  V(mrs_rs_systemmove)              \
  V(msb_z_p_zzz)                    \
  V(msr_sr_systemmove)              \
  V(msub_32a_dp_3src)               \
  V(msub_64a_dp_3src)               \
  V(mul_asimdelem_r)                \
  V(mul_asimdsame_only)             \
  V(mul_z_p_zz)                     \
  V(mul_z_zi)                       \
  V(mul_z_zz)                       \
  V(mul_z_zzi_d)                    \
  V(mul_z_zzi_h)                    \
  V(mul_z_zzi_s)                    \
  V(mvni_asimdimm_l_hl)             \
  V(mvni_asimdimm_l_sl)             \
  V(mvni_asimdimm_m_sm)             \
  V(nand_p_p_pp_z)                  \
  V(nands_p_p_pp_z)                 \
  V(nbsl_z_zzz)                     \
  V(neg_asimdmisc_r)                \
  V(neg_asisdmisc_r)                \
  V(neg_z_p_z)                      \
  V(nmatch_p_p_zz)                  \
  V(nop_hi_hints)                   \
  V(nor_p_p_pp_z)                   \
  V(nors_p_p_pp_z)                  \
  V(not_asimdmisc_r)                \
  V(not_z_p_z)                      \
  V(orn_32_log_shift)               \
  V(orn_64_log_shift)               \
  V(orn_asimdsame_only)             \
  V(orn_p_p_pp_z)                   \
  V(orns_p_p_pp_z)                  \
  V(orr_32_log_imm)                 \
  V(orr_32_log_shift)               \
  V(orr_64_log_imm)                 \
  V(orr_64_log_shift)               \
  V(orr_asimdimm_l_hl)              \
  V(orr_asimdimm_l_sl)              \
  V(orr_asimdsame_only)             \
  V(orr_p_p_pp_z)                   \
  V(orr_z_p_zz)                     \
  V(orr_z_zi)                       \
  V(orr_z_zz)                       \
  V(orrs_p_p_pp_z)                  \
  V(orv_r_p_z)                      \
  V(pacda_64p_dp_1src)              \
  V(pacdb_64p_dp_1src)              \
  V(pacdza_64z_dp_1src)             \
  V(pacdzb_64z_dp_1src)             \
  V(pacga_64p_dp_2src)              \
  V(pacia1716_hi_hints)             \
  V(pacia_64p_dp_1src)              \
  V(paciasp_hi_hints)               \
  V(paciaz_hi_hints)                \
  V(pacib1716_hi_hints)             \
  V(pacib_64p_dp_1src)              \
  V(pacibsp_hi_hints)               \
  V(pacibz_hi_hints)                \
  V(paciza_64z_dp_1src)             \
  V(pacizb_64z_dp_1src)             \
  V(pfalse_p)                       \
  V(pfirst_p_p_p)                   \
  V(pmul_asimdsame_only)            \
  V(pmul_z_zz)                      \
  V(pmull_asimddiff_l)              \
  V(pmullb_z_zz)                    \
  V(pmullt_z_zz)                    \
  V(pnext_p_p_p)                    \
  V(prfb_i_p_ai_d)                  \
  V(prfb_i_p_ai_s)                  \
  V(prfb_i_p_bi_s)                  \
  V(prfb_i_p_br_s)                  \
  V(prfb_i_p_bz_d_64_scaled)        \
  V(prfb_i_p_bz_d_x32_scaled)       \
  V(prfb_i_p_bz_s_x32_scaled)       \
  V(prfd_i_p_ai_d)                  \
  V(prfd_i_p_ai_s)                  \
  V(prfd_i_p_bi_s)                  \
  V(prfd_i_p_br_s)                  \
  V(prfd_i_p_bz_d_64_scaled)        \
  V(prfd_i_p_bz_d_x32_scaled)       \
  V(prfd_i_p_bz_s_x32_scaled)       \
  V(prfh_i_p_ai_d)                  \
  V(prfh_i_p_ai_s)                  \
  V(prfh_i_p_bi_s)                  \
  V(prfh_i_p_br_s)                  \
  V(prfh_i_p_bz_d_64_scaled)        \
  V(prfh_i_p_bz_d_x32_scaled)       \
  V(prfh_i_p_bz_s_x32_scaled)       \
  V(prfm_p_ldst_pos)                \
  V(prfm_p_ldst_regoff)             \
  V(prfm_p_loadlit)                 \
  V(prfum_p_ldst_unscaled)          \
  V(prfw_i_p_ai_d)                  \
  V(prfw_i_p_ai_s)                  \
  V(prfw_i_p_bi_s)                  \
  V(prfw_i_p_br_s)                  \
  V(prfw_i_p_bz_d_64_scaled)        \
  V(prfw_i_p_bz_d_x32_scaled)       \
  V(prfw_i_p_bz_s_x32_scaled)       \
  V(psb_hc_hints)                   \
  V(pssbb_only_barriers)            \
  V(ptest_p_p)                      \
  V(ptrue_p_s)                      \
  V(ptrues_p_s)                     \
  V(punpkhi_p_p)                    \
  V(punpklo_p_p)                    \
  V(raddhn_asimddiff_n)             \
  V(raddhnb_z_zz)                   \
  V(raddhnt_z_zz)                   \
  V(rax1_vvv2_cryptosha512_3)       \
  V(rax1_z_zz)                      \
  V(rbit_32_dp_1src)                \
  V(rbit_64_dp_1src)                \
  V(rbit_asimdmisc_r)               \
  V(rbit_z_p_z)                     \
  V(rdffr_p_f)                      \
  V(rdffr_p_p_f)                    \
  V(rdffrs_p_p_f)                   \
  V(rdvl_r_i)                       \
  V(ret_64r_branch_reg)             \
  V(retaa_64e_branch_reg)           \
  V(retab_64e_branch_reg)           \
  V(rev16_32_dp_1src)               \
  V(rev16_64_dp_1src)               \
  V(rev16_asimdmisc_r)              \
  V(rev32_64_dp_1src)               \
  V(rev32_asimdmisc_r)              \
  V(rev64_asimdmisc_r)              \
  V(rev_32_dp_1src)                 \
  V(rev_64_dp_1src)                 \
  V(rev_p_p)                        \
  V(rev_z_z)                        \
  V(revb_z_z)                       \
  V(revh_z_z)                       \
  V(revw_z_z)                       \
  V(rmif_only_rmif)                 \
  V(rorv_32_dp_2src)                \
  V(rorv_64_dp_2src)                \
  V(rshrn_asimdshf_n)               \
  V(rshrnb_z_zi)                    \
  V(rshrnt_z_zi)                    \
  V(rsubhn_asimddiff_n)             \
  V(rsubhnb_z_zz)                   \
  V(rsubhnt_z_zz)                   \
  V(saba_asimdsame_only)            \
  V(saba_z_zzz)                     \
  V(sabal_asimddiff_l)              \
  V(sabalb_z_zzz)                   \
  V(sabalt_z_zzz)                   \
  V(sabd_asimdsame_only)            \
  V(sabd_z_p_zz)                    \
  V(sabdl_asimddiff_l)              \
  V(sabdlb_z_zz)                    \
  V(sabdlt_z_zz)                    \
  V(sadalp_asimdmisc_p)             \
  V(sadalp_z_p_z)                   \
  V(saddl_asimddiff_l)              \
  V(saddlb_z_zz)                    \
  V(saddlbt_z_zz)                   \
  V(saddlp_asimdmisc_p)             \
  V(saddlt_z_zz)                    \
  V(saddlv_asimdall_only)           \
  V(saddv_r_p_z)                    \
  V(saddw_asimddiff_w)              \
  V(saddwb_z_zz)                    \
  V(saddwt_z_zz)                    \
  V(sb_only_barriers)               \
  V(sbc_32_addsub_carry)            \
  V(sbc_64_addsub_carry)            \
  V(sbclb_z_zzz)                    \
  V(sbclt_z_zzz)                    \
  V(sbcs_32_addsub_carry)           \
  V(sbcs_64_addsub_carry)           \
  V(sbfm_32m_bitfield)              \
  V(sbfm_64m_bitfield)              \
  V(scvtf_asimdmisc_r)              \
  V(scvtf_asimdmiscfp16_r)          \
  V(scvtf_asimdshf_c)               \
  V(scvtf_asisdmisc_r)              \
  V(scvtf_asisdmiscfp16_r)          \
  V(scvtf_asisdshf_c)               \
  V(scvtf_d32_float2fix)            \
  V(scvtf_d32_float2int)            \
  V(scvtf_d64_float2fix)            \
  V(scvtf_d64_float2int)            \
  V(scvtf_h32_float2fix)            \
  V(scvtf_h32_float2int)            \
  V(scvtf_h64_float2fix)            \
  V(scvtf_h64_float2int)            \
  V(scvtf_s32_float2fix)            \
  V(scvtf_s32_float2int)            \
  V(scvtf_s64_float2fix)            \
  V(scvtf_s64_float2int)            \
  V(scvtf_z_p_z_h2fp16)             \
  V(scvtf_z_p_z_w2d)                \
  V(scvtf_z_p_z_w2fp16)             \
  V(scvtf_z_p_z_w2s)                \
  V(scvtf_z_p_z_x2d)                \
  V(scvtf_z_p_z_x2fp16)             \
  V(scvtf_z_p_z_x2s)                \
  V(sdiv_32_dp_2src)                \
  V(sdiv_64_dp_2src)                \
  V(sdiv_z_p_zz)                    \
  V(sdivr_z_p_zz)                   \
  V(sdot_asimdelem_d)               \
  V(sdot_asimdsame2_d)              \
  V(sdot_z_zzz)                     \
  V(sdot_z_zzzi_d)                  \
  V(sdot_z_zzzi_s)                  \
  V(sel_p_p_pp)                     \
  V(sel_z_p_zz)                     \
  V(setf16_only_setf)               \
  V(setf8_only_setf)                \
  V(setffr_f)                       \
  V(sev_hi_hints)                   \
  V(sevl_hi_hints)                  \
  V(sha1c_qsv_cryptosha3)           \
  V(sha1h_ss_cryptosha2)            \
  V(sha1m_qsv_cryptosha3)           \
  V(sha1p_qsv_cryptosha3)           \
  V(sha1su0_vvv_cryptosha3)         \
  V(sha1su1_vv_cryptosha2)          \
  V(sha256h2_qqv_cryptosha3)        \
  V(sha256h_qqv_cryptosha3)         \
  V(sha256su0_vv_cryptosha2)        \
  V(sha256su1_vvv_cryptosha3)       \
  V(sha512h2_qqv_cryptosha512_3)    \
  V(sha512h_qqv_cryptosha512_3)     \
  V(sha512su0_vv2_cryptosha512_2)   \
  V(sha512su1_vvv2_cryptosha512_3)  \
  V(shadd_asimdsame_only)           \
  V(shadd_z_p_zz)                   \
  V(shl_asimdshf_r)                 \
  V(shl_asisdshf_r)                 \
  V(shll_asimdmisc_s)               \
  V(shrn_asimdshf_n)                \
  V(shrnb_z_zi)                     \
  V(shrnt_z_zi)                     \
  V(shsub_asimdsame_only)           \
  V(shsub_z_p_zz)                   \
  V(shsubr_z_p_zz)                  \
  V(sli_asimdshf_r)                 \
  V(sli_asisdshf_r)                 \
  V(sli_z_zzi)                      \
  V(sm3partw1_vvv4_cryptosha512_3)  \
  V(sm3partw2_vvv4_cryptosha512_3)  \
  V(sm3ss1_vvv4_crypto4)            \
  V(sm3tt1a_vvv4_crypto3_imm2)      \
  V(sm3tt1b_vvv4_crypto3_imm2)      \
  V(sm3tt2a_vvv4_crypto3_imm2)      \
  V(sm3tt2b_vvv_crypto3_imm2)       \
  V(sm4e_vv4_cryptosha512_2)        \
  V(sm4e_z_zz)                      \
  V(sm4ekey_vvv4_cryptosha512_3)    \
  V(sm4ekey_z_zz)                   \
  V(smaddl_64wa_dp_3src)            \
  V(smax_asimdsame_only)            \
  V(smax_z_p_zz)                    \
  V(smax_z_zi)                      \
  V(smaxp_asimdsame_only)           \
  V(smaxp_z_p_zz)                   \
  V(smaxv_asimdall_only)            \
  V(smaxv_r_p_z)                    \
  V(smc_ex_exception)               \
  V(smin_asimdsame_only)            \
  V(smin_z_p_zz)                    \
  V(smin_z_zi)                      \
  V(sminp_asimdsame_only)           \
  V(sminp_z_p_zz)                   \
  V(sminv_asimdall_only)            \
  V(sminv_r_p_z)                    \
  V(smlal_asimddiff_l)              \
  V(smlal_asimdelem_l)              \
  V(smlalb_z_zzz)                   \
  V(smlalb_z_zzzi_d)                \
  V(smlalb_z_zzzi_s)                \
  V(smlalt_z_zzz)                   \
  V(smlalt_z_zzzi_d)                \
  V(smlalt_z_zzzi_s)                \
  V(smlsl_asimddiff_l)              \
  V(smlsl_asimdelem_l)              \
  V(smlslb_z_zzz)                   \
  V(smlslb_z_zzzi_d)                \
  V(smlslb_z_zzzi_s)                \
  V(smlslt_z_zzz)                   \
  V(smlslt_z_zzzi_d)                \
  V(smlslt_z_zzzi_s)                \
  V(smmla_asimdsame2_g)             \
  V(smmla_z_zzz)                    \
  V(smov_asimdins_w_w)              \
  V(smov_asimdins_x_x)              \
  V(smsubl_64wa_dp_3src)            \
  V(smulh_64_dp_3src)               \
  V(smulh_z_p_zz)                   \
  V(smulh_z_zz)                     \
  V(smull_asimddiff_l)              \
  V(smull_asimdelem_l)              \
  V(smullb_z_zz)                    \
  V(smullb_z_zzi_d)                 \
  V(smullb_z_zzi_s)                 \
  V(smullt_z_zz)                    \
  V(smullt_z_zzi_d)                 \
  V(smullt_z_zzi_s)                 \
  V(splice_z_p_zz_con)              \
  V(splice_z_p_zz_des)              \
  V(sqabs_asimdmisc_r)              \
  V(sqabs_asisdmisc_r)              \
  V(sqabs_z_p_z)                    \
  V(sqadd_asimdsame_only)           \
  V(sqadd_asisdsame_only)           \
  V(sqadd_z_p_zz)                   \
  V(sqadd_z_zi)                     \
  V(sqadd_z_zz)                     \
  V(sqcadd_z_zz)                    \
  V(sqdecb_r_rs_sx)                 \
  V(sqdecb_r_rs_x)                  \
  V(sqdecd_r_rs_sx)                 \
  V(sqdecd_r_rs_x)                  \
  V(sqdecd_z_zs)                    \
  V(sqdech_r_rs_sx)                 \
  V(sqdech_r_rs_x)                  \
  V(sqdech_z_zs)                    \
  V(sqdecp_r_p_r_sx)                \
  V(sqdecp_r_p_r_x)                 \
  V(sqdecp_z_p_z)                   \
  V(sqdecw_r_rs_sx)                 \
  V(sqdecw_r_rs_x)                  \
  V(sqdecw_z_zs)                    \
  V(sqdmlal_asimddiff_l)            \
  V(sqdmlal_asimdelem_l)            \
  V(sqdmlal_asisddiff_only)         \
  V(sqdmlal_asisdelem_l)            \
  V(sqdmlalb_z_zzz)                 \
  V(sqdmlalb_z_zzzi_d)              \
  V(sqdmlalb_z_zzzi_s)              \
  V(sqdmlalbt_z_zzz)                \
  V(sqdmlalt_z_zzz)                 \
  V(sqdmlalt_z_zzzi_d)              \
  V(sqdmlalt_z_zzzi_s)              \
  V(sqdmlsl_asimddiff_l)            \
  V(sqdmlsl_asimdelem_l)            \
  V(sqdmlsl_asisddiff_only)         \
  V(sqdmlsl_asisdelem_l)            \
  V(sqdmlslb_z_zzz)                 \
  V(sqdmlslb_z_zzzi_d)              \
  V(sqdmlslb_z_zzzi_s)              \
  V(sqdmlslbt_z_zzz)                \
  V(sqdmlslt_z_zzz)                 \
  V(sqdmlslt_z_zzzi_d)              \
  V(sqdmlslt_z_zzzi_s)              \
  V(sqdmulh_asimdelem_r)            \
  V(sqdmulh_asimdsame_only)         \
  V(sqdmulh_asisdelem_r)            \
  V(sqdmulh_asisdsame_only)         \
  V(sqdmulh_z_zz)                   \
  V(sqdmulh_z_zzi_d)                \
  V(sqdmulh_z_zzi_h)                \
  V(sqdmulh_z_zzi_s)                \
  V(sqdmull_asimddiff_l)            \
  V(sqdmull_asimdelem_l)            \
  V(sqdmull_asisddiff_only)         \
  V(sqdmull_asisdelem_l)            \
  V(sqdmullb_z_zz)                  \
  V(sqdmullb_z_zzi_d)               \
  V(sqdmullb_z_zzi_s)               \
  V(sqdmullt_z_zz)                  \
  V(sqdmullt_z_zzi_d)               \
  V(sqdmullt_z_zzi_s)               \
  V(sqincb_r_rs_sx)                 \
  V(sqincb_r_rs_x)                  \
  V(sqincd_r_rs_sx)                 \
  V(sqincd_r_rs_x)                  \
  V(sqincd_z_zs)                    \
  V(sqinch_r_rs_sx)                 \
  V(sqinch_r_rs_x)                  \
  V(sqinch_z_zs)                    \
  V(sqincp_r_p_r_sx)                \
  V(sqincp_r_p_r_x)                 \
  V(sqincp_z_p_z)                   \
  V(sqincw_r_rs_sx)                 \
  V(sqincw_r_rs_x)                  \
  V(sqincw_z_zs)                    \
  V(sqneg_asimdmisc_r)              \
  V(sqneg_asisdmisc_r)              \
  V(sqneg_z_p_z)                    \
  V(sqrdcmlah_z_zzz)                \
  V(sqrdcmlah_z_zzzi_h)             \
  V(sqrdcmlah_z_zzzi_s)             \
  V(sqrdmlah_asimdelem_r)           \
  V(sqrdmlah_asimdsame2_only)       \
  V(sqrdmlah_asisdelem_r)           \
  V(sqrdmlah_asisdsame2_only)       \
  V(sqrdmlah_z_zzz)                 \
  V(sqrdmlah_z_zzzi_d)              \
  V(sqrdmlah_z_zzzi_h)              \
  V(sqrdmlah_z_zzzi_s)              \
  V(sqrdmlsh_asimdelem_r)           \
  V(sqrdmlsh_asimdsame2_only)       \
  V(sqrdmlsh_asisdelem_r)           \
  V(sqrdmlsh_asisdsame2_only)       \
  V(sqrdmlsh_z_zzz)                 \
  V(sqrdmlsh_z_zzzi_d)              \
  V(sqrdmlsh_z_zzzi_h)              \
  V(sqrdmlsh_z_zzzi_s)              \
  V(sqrdmulh_asimdelem_r)           \
  V(sqrdmulh_asimdsame_only)        \
  V(sqrdmulh_asisdelem_r)           \
  V(sqrdmulh_asisdsame_only)        \
  V(sqrdmulh_z_zz)                  \
  V(sqrdmulh_z_zzi_d)               \
  V(sqrdmulh_z_zzi_h)               \
  V(sqrdmulh_z_zzi_s)               \
  V(sqrshl_asimdsame_only)          \
  V(sqrshl_asisdsame_only)          \
  V(sqrshl_z_p_zz)                  \
  V(sqrshlr_z_p_zz)                 \
  V(sqrshrn_asimdshf_n)             \
  V(sqrshrn_asisdshf_n)             \
  V(sqrshrnb_z_zi)                  \
  V(sqrshrnt_z_zi)                  \
  V(sqrshrun_asimdshf_n)            \
  V(sqrshrun_asisdshf_n)            \
  V(sqrshrunb_z_zi)                 \
  V(sqrshrunt_z_zi)                 \
  V(sqshl_asimdsame_only)           \
  V(sqshl_asimdshf_r)               \
  V(sqshl_asisdsame_only)           \
  V(sqshl_asisdshf_r)               \
  V(sqshl_z_p_zi)                   \
  V(sqshl_z_p_zz)                   \
  V(sqshlr_z_p_zz)                  \
  V(sqshlu_asimdshf_r)              \
  V(sqshlu_asisdshf_r)              \
  V(sqshlu_z_p_zi)                  \
  V(sqshrn_asimdshf_n)              \
  V(sqshrn_asisdshf_n)              \
  V(sqshrnb_z_zi)                   \
  V(sqshrnt_z_zi)                   \
  V(sqshrun_asimdshf_n)             \
  V(sqshrun_asisdshf_n)             \
  V(sqshrunb_z_zi)                  \
  V(sqshrunt_z_zi)                  \
  V(sqsub_asimdsame_only)           \
  V(sqsub_asisdsame_only)           \
  V(sqsub_z_p_zz)                   \
  V(sqsub_z_zi)                     \
  V(sqsub_z_zz)                     \
  V(sqsubr_z_p_zz)                  \
  V(sqxtn_asimdmisc_n)              \
  V(sqxtn_asisdmisc_n)              \
  V(sqxtnb_z_zz)                    \
  V(sqxtnt_z_zz)                    \
  V(sqxtun_asimdmisc_n)             \
  V(sqxtun_asisdmisc_n)             \
  V(sqxtunb_z_zz)                   \
  V(sqxtunt_z_zz)                   \
  V(srhadd_asimdsame_only)          \
  V(srhadd_z_p_zz)                  \
  V(sri_asimdshf_r)                 \
  V(sri_asisdshf_r)                 \
  V(sri_z_zzi)                      \
  V(srshl_asimdsame_only)           \
  V(srshl_asisdsame_only)           \
  V(srshl_z_p_zz)                   \
  V(srshlr_z_p_zz)                  \
  V(srshr_asimdshf_r)               \
  V(srshr_asisdshf_r)               \
  V(srshr_z_p_zi)                   \
  V(srsra_asimdshf_r)               \
  V(srsra_asisdshf_r)               \
  V(srsra_z_zi)                     \
  V(ssbb_only_barriers)             \
  V(sshl_asimdsame_only)            \
  V(sshl_asisdsame_only)            \
  V(sshll_asimdshf_l)               \
  V(sshllb_z_zi)                    \
  V(sshllt_z_zi)                    \
  V(sshr_asimdshf_r)                \
  V(sshr_asisdshf_r)                \
  V(ssra_asimdshf_r)                \
  V(ssra_asisdshf_r)                \
  V(ssra_z_zi)                      \
  V(ssubl_asimddiff_l)              \
  V(ssublb_z_zz)                    \
  V(ssublbt_z_zz)                   \
  V(ssublt_z_zz)                    \
  V(ssubltb_z_zz)                   \
  V(ssubw_asimddiff_w)              \
  V(ssubwb_z_zz)                    \
  V(ssubwt_z_zz)                    \
  V(st1_asisdlse_r1_1v)             \
  V(st1_asisdlse_r2_2v)             \
  V(st1_asisdlse_r3_3v)             \
  V(st1_asisdlse_r4_4v)             \
  V(st1_asisdlsep_i1_i1)            \
  V(st1_asisdlsep_i2_i2)            \
  V(st1_asisdlsep_i3_i3)            \
  V(st1_asisdlsep_i4_i4)            \
  V(st1_asisdlsep_r1_r1)            \
  V(st1_asisdlsep_r2_r2)            \
  V(st1_asisdlsep_r3_r3)            \
  V(st1_asisdlsep_r4_r4)            \
  V(st1_asisdlso_b1_1b)             \
  V(st1_asisdlso_d1_1d)             \
  V(st1_asisdlso_h1_1h)             \
  V(st1_asisdlso_s1_1s)             \
  V(st1_asisdlsop_b1_i1b)           \
  V(st1_asisdlsop_bx1_r1b)          \
  V(st1_asisdlsop_d1_i1d)           \
  V(st1_asisdlsop_dx1_r1d)          \
  V(st1_asisdlsop_h1_i1h)           \
  V(st1_asisdlsop_hx1_r1h)          \
  V(st1_asisdlsop_s1_i1s)           \
  V(st1_asisdlsop_sx1_r1s)          \
  V(st1b_z_p_ai_d)                  \
  V(st1b_z_p_ai_s)                  \
  V(st1b_z_p_bi)                    \
  V(st1b_z_p_br)                    \
  V(st1b_z_p_bz_d_64_unscaled)      \
  V(st1b_z_p_bz_d_x32_unscaled)     \
  V(st1b_z_p_bz_s_x32_unscaled)     \
  V(st1d_z_p_ai_d)                  \
  V(st1d_z_p_bi)                    \
  V(st1d_z_p_br)                    \
  V(st1d_z_p_bz_d_64_scaled)        \
  V(st1d_z_p_bz_d_64_unscaled)      \
  V(st1d_z_p_bz_d_x32_scaled)       \
  V(st1d_z_p_bz_d_x32_unscaled)     \
  V(st1h_z_p_ai_d)                  \
  V(st1h_z_p_ai_s)                  \
  V(st1h_z_p_bi)                    \
  V(st1h_z_p_br)                    \
  V(st1h_z_p_bz_d_64_scaled)        \
  V(st1h_z_p_bz_d_64_unscaled)      \
  V(st1h_z_p_bz_d_x32_scaled)       \
  V(st1h_z_p_bz_d_x32_unscaled)     \
  V(st1h_z_p_bz_s_x32_scaled)       \
  V(st1h_z_p_bz_s_x32_unscaled)     \
  V(st1w_z_p_ai_d)                  \
  V(st1w_z_p_ai_s)                  \
  V(st1w_z_p_bi)                    \
  V(st1w_z_p_br)                    \
  V(st1w_z_p_bz_d_64_scaled)        \
  V(st1w_z_p_bz_d_64_unscaled)      \
  V(st1w_z_p_bz_d_x32_scaled)       \
  V(st1w_z_p_bz_d_x32_unscaled)     \
  V(st1w_z_p_bz_s_x32_scaled)       \
  V(st1w_z_p_bz_s_x32_unscaled)     \
  V(st2_asisdlse_r2)                \
  V(st2_asisdlsep_i2_i)             \
  V(st2_asisdlsep_r2_r)             \
  V(st2_asisdlso_b2_2b)             \
  V(st2_asisdlso_d2_2d)             \
  V(st2_asisdlso_h2_2h)             \
  V(st2_asisdlso_s2_2s)             \
  V(st2_asisdlsop_b2_i2b)           \
  V(st2_asisdlsop_bx2_r2b)          \
  V(st2_asisdlsop_d2_i2d)           \
  V(st2_asisdlsop_dx2_r2d)          \
  V(st2_asisdlsop_h2_i2h)           \
  V(st2_asisdlsop_hx2_r2h)          \
  V(st2_asisdlsop_s2_i2s)           \
  V(st2_asisdlsop_sx2_r2s)          \
  V(st2b_z_p_bi_contiguous)         \
  V(st2b_z_p_br_contiguous)         \
  V(st2d_z_p_bi_contiguous)         \
  V(st2d_z_p_br_contiguous)         \
  V(st2g_64soffset_ldsttags)        \
  V(st2g_64spost_ldsttags)          \
  V(st2g_64spre_ldsttags)           \
  V(st2h_z_p_bi_contiguous)         \
  V(st2h_z_p_br_contiguous)         \
  V(st2w_z_p_bi_contiguous)         \
  V(st2w_z_p_br_contiguous)         \
  V(st3_asisdlse_r3)                \
  V(st3_asisdlsep_i3_i)             \
  V(st3_asisdlsep_r3_r)             \
  V(st3_asisdlso_b3_3b)             \
  V(st3_asisdlso_d3_3d)             \
  V(st3_asisdlso_h3_3h)             \
  V(st3_asisdlso_s3_3s)             \
  V(st3_asisdlsop_b3_i3b)           \
  V(st3_asisdlsop_bx3_r3b)          \
  V(st3_asisdlsop_d3_i3d)           \
  V(st3_asisdlsop_dx3_r3d)          \
  V(st3_asisdlsop_h3_i3h)           \
  V(st3_asisdlsop_hx3_r3h)          \
  V(st3_asisdlsop_s3_i3s)           \
  V(st3_asisdlsop_sx3_r3s)          \
  V(st3b_z_p_bi_contiguous)         \
  V(st3b_z_p_br_contiguous)         \
  V(st3d_z_p_bi_contiguous)         \
  V(st3d_z_p_br_contiguous)         \
  V(st3h_z_p_bi_contiguous)         \
  V(st3h_z_p_br_contiguous)         \
  V(st3w_z_p_bi_contiguous)         \
  V(st3w_z_p_br_contiguous)         \
  V(st4_asisdlse_r4)                \
  V(st4_asisdlsep_i4_i)             \
  V(st4_asisdlsep_r4_r)             \
  V(st4_asisdlso_b4_4b)             \
  V(st4_asisdlso_d4_4d)             \
  V(st4_asisdlso_h4_4h)             \
  V(st4_asisdlso_s4_4s)             \
  V(st4_asisdlsop_b4_i4b)           \
  V(st4_asisdlsop_bx4_r4b)          \
  V(st4_asisdlsop_d4_i4d)           \
  V(st4_asisdlsop_dx4_r4d)          \
  V(st4_asisdlsop_h4_i4h)           \
  V(st4_asisdlsop_hx4_r4h)          \
  V(st4_asisdlsop_s4_i4s)           \
  V(st4_asisdlsop_sx4_r4s)          \
  V(st4b_z_p_bi_contiguous)         \
  V(st4b_z_p_br_contiguous)         \
  V(st4d_z_p_bi_contiguous)         \
  V(st4d_z_p_br_contiguous)         \
  V(st4h_z_p_bi_contiguous)         \
  V(st4h_z_p_br_contiguous)         \
  V(st4w_z_p_bi_contiguous)         \
  V(st4w_z_p_br_contiguous)         \
  V(st64b_64l_memop)                \
  V(st64bv0_64_memop)               \
  V(st64bv_64_memop)                \
  V(stg_64soffset_ldsttags)         \
  V(stg_64spost_ldsttags)           \
  V(stg_64spre_ldsttags)            \
  V(stgm_64bulk_ldsttags)           \
  V(stgp_64_ldstpair_off)           \
  V(stgp_64_ldstpair_post)          \
  V(stgp_64_ldstpair_pre)           \
  V(stllr_sl32_ldstexcl)            \
  V(stllr_sl64_ldstexcl)            \
  V(stllrb_sl32_ldstexcl)           \
  V(stllrh_sl32_ldstexcl)           \
  V(stlr_sl32_ldstexcl)             \
  V(stlr_sl64_ldstexcl)             \
  V(stlrb_sl32_ldstexcl)            \
  V(stlrh_sl32_ldstexcl)            \
  V(stlur_32_ldapstl_unscaled)      \
  V(stlur_64_ldapstl_unscaled)      \
  V(stlurb_32_ldapstl_unscaled)     \
  V(stlurh_32_ldapstl_unscaled)     \
  V(stlxp_sp32_ldstexcl)            \
  V(stlxp_sp64_ldstexcl)            \
  V(stlxr_sr32_ldstexcl)            \
  V(stlxr_sr64_ldstexcl)            \
  V(stlxrb_sr32_ldstexcl)           \
  V(stlxrh_sr32_ldstexcl)           \
  V(stnp_32_ldstnapair_offs)        \
  V(stnp_64_ldstnapair_offs)        \
  V(stnp_d_ldstnapair_offs)         \
  V(stnp_q_ldstnapair_offs)         \
  V(stnp_s_ldstnapair_offs)         \
  V(stnt1b_z_p_ar_d_64_unscaled)    \
  V(stnt1b_z_p_ar_s_x32_unscaled)   \
  V(stnt1b_z_p_bi_contiguous)       \
  V(stnt1b_z_p_br_contiguous)       \
  V(stnt1d_z_p_ar_d_64_unscaled)    \
  V(stnt1d_z_p_bi_contiguous)       \
  V(stnt1d_z_p_br_contiguous)       \
  V(stnt1h_z_p_ar_d_64_unscaled)    \
  V(stnt1h_z_p_ar_s_x32_unscaled)   \
  V(stnt1h_z_p_bi_contiguous)       \
  V(stnt1h_z_p_br_contiguous)       \
  V(stnt1w_z_p_ar_d_64_unscaled)    \
  V(stnt1w_z_p_ar_s_x32_unscaled)   \
  V(stnt1w_z_p_bi_contiguous)       \
  V(stnt1w_z_p_br_contiguous)       \
  V(stp_32_ldstpair_off)            \
  V(stp_32_ldstpair_post)           \
  V(stp_32_ldstpair_pre)            \
  V(stp_64_ldstpair_off)            \
  V(stp_64_ldstpair_post)           \
  V(stp_64_ldstpair_pre)            \
  V(stp_d_ldstpair_off)             \
  V(stp_d_ldstpair_post)            \
  V(stp_d_ldstpair_pre)             \
  V(stp_q_ldstpair_off)             \
  V(stp_q_ldstpair_post)            \
  V(stp_q_ldstpair_pre)             \
  V(stp_s_ldstpair_off)             \
  V(stp_s_ldstpair_post)            \
  V(stp_s_ldstpair_pre)             \
  V(str_32_ldst_immpost)            \
  V(str_32_ldst_immpre)             \
  V(str_32_ldst_pos)                \
  V(str_32_ldst_regoff)             \
  V(str_64_ldst_immpost)            \
  V(str_64_ldst_immpre)             \
  V(str_64_ldst_pos)                \
  V(str_64_ldst_regoff)             \
  V(str_b_ldst_immpost)             \
  V(str_b_ldst_immpre)              \
  V(str_b_ldst_pos)                 \
  V(str_b_ldst_regoff)              \
  V(str_bl_ldst_regoff)             \
  V(str_d_ldst_immpost)             \
  V(str_d_ldst_immpre)              \
  V(str_d_ldst_pos)                 \
  V(str_d_ldst_regoff)              \
  V(str_h_ldst_immpost)             \
  V(str_h_ldst_immpre)              \
  V(str_h_ldst_pos)                 \
  V(str_h_ldst_regoff)              \
  V(str_p_bi)                       \
  V(str_q_ldst_immpost)             \
  V(str_q_ldst_immpre)              \
  V(str_q_ldst_pos)                 \
  V(str_q_ldst_regoff)              \
  V(str_s_ldst_immpost)             \
  V(str_s_ldst_immpre)              \
  V(str_s_ldst_pos)                 \
  V(str_s_ldst_regoff)              \
  V(str_z_bi)                       \
  V(strb_32_ldst_immpost)           \
  V(strb_32_ldst_immpre)            \
  V(strb_32_ldst_pos)               \
  V(strb_32b_ldst_regoff)           \
  V(strb_32bl_ldst_regoff)          \
  V(strh_32_ldst_immpost)           \
  V(strh_32_ldst_immpre)            \
  V(strh_32_ldst_pos)               \
  V(strh_32_ldst_regoff)            \
  V(sttr_32_ldst_unpriv)            \
  V(sttr_64_ldst_unpriv)            \
  V(sttrb_32_ldst_unpriv)           \
  V(sttrh_32_ldst_unpriv)           \
  V(stur_32_ldst_unscaled)          \
  V(stur_64_ldst_unscaled)          \
  V(stur_b_ldst_unscaled)           \
  V(stur_d_ldst_unscaled)           \
  V(stur_h_ldst_unscaled)           \
  V(stur_q_ldst_unscaled)           \
  V(stur_s_ldst_unscaled)           \
  V(sturb_32_ldst_unscaled)         \
  V(sturh_32_ldst_unscaled)         \
  V(stxp_sp32_ldstexcl)             \
  V(stxp_sp64_ldstexcl)             \
  V(stxr_sr32_ldstexcl)             \
  V(stxr_sr64_ldstexcl)             \
  V(stxrb_sr32_ldstexcl)            \
  V(stxrh_sr32_ldstexcl)            \
  V(stz2g_64soffset_ldsttags)       \
  V(stz2g_64spost_ldsttags)         \
  V(stz2g_64spre_ldsttags)          \
  V(stzg_64soffset_ldsttags)        \
  V(stzg_64spost_ldsttags)          \
  V(stzg_64spre_ldsttags)           \
  V(stzgm_64bulk_ldsttags)          \
  V(sub_32_addsub_ext)              \
  V(sub_32_addsub_imm)              \
  V(sub_32_addsub_shift)            \
  V(sub_64_addsub_ext)              \
  V(sub_64_addsub_imm)              \
  V(sub_64_addsub_shift)            \
  V(sub_asimdsame_only)             \
  V(sub_asisdsame_only)             \
  V(sub_z_p_zz)                     \
  V(sub_z_zi)                       \
  V(sub_z_zz)                       \
  V(subg_64_addsub_immtags)         \
  V(subhn_asimddiff_n)              \
  V(subhnb_z_zz)                    \
  V(subhnt_z_zz)                    \
  V(subp_64s_dp_2src)               \
  V(subps_64s_dp_2src)              \
  V(subr_z_p_zz)                    \
  V(subr_z_zi)                      \
  V(subs_32_addsub_shift)           \
  V(subs_32s_addsub_ext)            \
  V(subs_32s_addsub_imm)            \
  V(subs_64_addsub_shift)           \
  V(subs_64s_addsub_ext)            \
  V(subs_64s_addsub_imm)            \
  V(sudot_asimdelem_d)              \
  V(sudot_z_zzzi_s)                 \
  V(sunpkhi_z_z)                    \
  V(sunpklo_z_z)                    \
  V(suqadd_asimdmisc_r)             \
  V(suqadd_asisdmisc_r)             \
  V(suqadd_z_p_zz)                  \
  V(svc_ex_exception)               \
  V(swp_32_memop)                   \
  V(swp_64_memop)                   \
  V(swpa_32_memop)                  \
  V(swpa_64_memop)                  \
  V(swpab_32_memop)                 \
  V(swpah_32_memop)                 \
  V(swpal_32_memop)                 \
  V(swpal_64_memop)                 \
  V(swpalb_32_memop)                \
  V(swpalh_32_memop)                \
  V(swpb_32_memop)                  \
  V(swph_32_memop)                  \
  V(swpl_32_memop)                  \
  V(swpl_64_memop)                  \
  V(swplb_32_memop)                 \
  V(swplh_32_memop)                 \
  V(sxtb_z_p_z)                     \
  V(sxth_z_p_z)                     \
  V(sxtw_z_p_z)                     \
  V(sys_cr_systeminstrs)            \
  V(sysl_rc_systeminstrs)           \
  V(tbl_asimdtbl_l1_1)              \
  V(tbl_asimdtbl_l2_2)              \
  V(tbl_asimdtbl_l3_3)              \
  V(tbl_asimdtbl_l4_4)              \
  V(tbl_z_zz_1)                     \
  V(tbl_z_zz_2)                     \
  V(tbnz_only_testbranch)           \
  V(tbx_asimdtbl_l1_1)              \
  V(tbx_asimdtbl_l2_2)              \
  V(tbx_asimdtbl_l3_3)              \
  V(tbx_asimdtbl_l4_4)              \
  V(tbx_z_zz)                       \
  V(tbz_only_testbranch)            \
  V(tcancel_ex_exception)           \
  V(tcommit_only_barriers)          \
  V(trn1_asimdperm_only)            \
  V(trn1_p_pp)                      \
  V(trn1_z_zz)                      \
  V(trn1_z_zz_q)                    \
  V(trn2_asimdperm_only)            \
  V(trn2_p_pp)                      \
  V(trn2_z_zz)                      \
  V(trn2_z_zz_q)                    \
  V(tsb_hc_hints)                   \
  V(tstart_br_systemresult)         \
  V(ttest_br_systemresult)          \
  V(uaba_asimdsame_only)            \
  V(uaba_z_zzz)                     \
  V(uabal_asimddiff_l)              \
  V(uabalb_z_zzz)                   \
  V(uabalt_z_zzz)                   \
  V(uabd_asimdsame_only)            \
  V(uabd_z_p_zz)                    \
  V(uabdl_asimddiff_l)              \
  V(uabdlb_z_zz)                    \
  V(uabdlt_z_zz)                    \
  V(uadalp_asimdmisc_p)             \
  V(uadalp_z_p_z)                   \
  V(uaddl_asimddiff_l)              \
  V(uaddlb_z_zz)                    \
  V(uaddlp_asimdmisc_p)             \
  V(uaddlt_z_zz)                    \
  V(uaddlv_asimdall_only)           \
  V(uaddv_r_p_z)                    \
  V(uaddw_asimddiff_w)              \
  V(uaddwb_z_zz)                    \
  V(uaddwt_z_zz)                    \
  V(ubfm_32m_bitfield)              \
  V(ubfm_64m_bitfield)              \
  V(ucvtf_asimdmisc_r)              \
  V(ucvtf_asimdmiscfp16_r)          \
  V(ucvtf_asimdshf_c)               \
  V(ucvtf_asisdmisc_r)              \
  V(ucvtf_asisdmiscfp16_r)          \
  V(ucvtf_asisdshf_c)               \
  V(ucvtf_d32_float2fix)            \
  V(ucvtf_d32_float2int)            \
  V(ucvtf_d64_float2fix)            \
  V(ucvtf_d64_float2int)            \
  V(ucvtf_h32_float2fix)            \
  V(ucvtf_h32_float2int)            \
  V(ucvtf_h64_float2fix)            \
  V(ucvtf_h64_float2int)            \
  V(ucvtf_s32_float2fix)            \
  V(ucvtf_s32_float2int)            \
  V(ucvtf_s64_float2fix)            \
  V(ucvtf_s64_float2int)            \
  V(ucvtf_z_p_z_h2fp16)             \
  V(ucvtf_z_p_z_w2d)                \
  V(ucvtf_z_p_z_w2fp16)             \
  V(ucvtf_z_p_z_w2s)                \
  V(ucvtf_z_p_z_x2d)                \
  V(ucvtf_z_p_z_x2fp16)             \
  V(ucvtf_z_p_z_x2s)                \
  V(udf_only_perm_undef)            \
  V(udiv_32_dp_2src)                \
  V(udiv_64_dp_2src)                \
  V(udiv_z_p_zz)                    \
  V(udivr_z_p_zz)                   \
  V(udot_asimdelem_d)               \
  V(udot_asimdsame2_d)              \
  V(udot_z_zzz)                     \
  V(udot_z_zzzi_d)                  \
  V(udot_z_zzzi_s)                  \
  V(uhadd_asimdsame_only)           \
  V(uhadd_z_p_zz)                   \
  V(uhsub_asimdsame_only)           \
  V(uhsub_z_p_zz)                   \
  V(uhsubr_z_p_zz)                  \
  V(umaddl_64wa_dp_3src)            \
  V(umax_asimdsame_only)            \
  V(umax_z_p_zz)                    \
  V(umax_z_zi)                      \
  V(umaxp_asimdsame_only)           \
  V(umaxp_z_p_zz)                   \
  V(umaxv_asimdall_only)            \
  V(umaxv_r_p_z)                    \
  V(umin_asimdsame_only)            \
  V(umin_z_p_zz)                    \
  V(umin_z_zi)                      \
  V(uminp_asimdsame_only)           \
  V(uminp_z_p_zz)                   \
  V(uminv_asimdall_only)            \
  V(uminv_r_p_z)                    \
  V(umlal_asimddiff_l)              \
  V(umlal_asimdelem_l)              \
  V(umlalb_z_zzz)                   \
  V(umlalb_z_zzzi_d)                \
  V(umlalb_z_zzzi_s)                \
  V(umlalt_z_zzz)                   \
  V(umlalt_z_zzzi_d)                \
  V(umlalt_z_zzzi_s)                \
  V(umlsl_asimddiff_l)              \
  V(umlsl_asimdelem_l)              \
  V(umlslb_z_zzz)                   \
  V(umlslb_z_zzzi_d)                \
  V(umlslb_z_zzzi_s)                \
  V(umlslt_z_zzz)                   \
  V(umlslt_z_zzzi_d)                \
  V(umlslt_z_zzzi_s)                \
  V(ummla_asimdsame2_g)             \
  V(ummla_z_zzz)                    \
  V(umov_asimdins_w_w)              \
  V(umov_asimdins_x_x)              \
  V(umsubl_64wa_dp_3src)            \
  V(umulh_64_dp_3src)               \
  V(umulh_z_p_zz)                   \
  V(umulh_z_zz)                     \
  V(umull_asimddiff_l)              \
  V(umull_asimdelem_l)              \
  V(umullb_z_zz)                    \
  V(umullb_z_zzi_d)                 \
  V(umullb_z_zzi_s)                 \
  V(umullt_z_zz)                    \
  V(umullt_z_zzi_d)                 \
  V(umullt_z_zzi_s)                 \
  V(uqadd_asimdsame_only)           \
  V(uqadd_asisdsame_only)           \
  V(uqadd_z_p_zz)                   \
  V(uqadd_z_zi)                     \
  V(uqadd_z_zz)                     \
  V(uqdecb_r_rs_uw)                 \
  V(uqdecb_r_rs_x)                  \
  V(uqdecd_r_rs_uw)                 \
  V(uqdecd_r_rs_x)                  \
  V(uqdecd_z_zs)                    \
  V(uqdech_r_rs_uw)                 \
  V(uqdech_r_rs_x)                  \
  V(uqdech_z_zs)                    \
  V(uqdecp_r_p_r_uw)                \
  V(uqdecp_r_p_r_x)                 \
  V(uqdecp_z_p_z)                   \
  V(uqdecw_r_rs_uw)                 \
  V(uqdecw_r_rs_x)                  \
  V(uqdecw_z_zs)                    \
  V(uqincb_r_rs_uw)                 \
  V(uqincb_r_rs_x)                  \
  V(uqincd_r_rs_uw)                 \
  V(uqincd_r_rs_x)                  \
  V(uqincd_z_zs)                    \
  V(uqinch_r_rs_uw)                 \
  V(uqinch_r_rs_x)                  \
  V(uqinch_z_zs)                    \
  V(uqincp_r_p_r_uw)                \
  V(uqincp_r_p_r_x)                 \
  V(uqincp_z_p_z)                   \
  V(uqincw_r_rs_uw)                 \
  V(uqincw_r_rs_x)                  \
  V(uqincw_z_zs)                    \
  V(uqrshl_asimdsame_only)          \
  V(uqrshl_asisdsame_only)          \
  V(uqrshl_z_p_zz)                  \
  V(uqrshlr_z_p_zz)                 \
  V(uqrshrn_asimdshf_n)             \
  V(uqrshrn_asisdshf_n)             \
  V(uqrshrnb_z_zi)                  \
  V(uqrshrnt_z_zi)                  \
  V(uqshl_asimdsame_only)           \
  V(uqshl_asimdshf_r)               \
  V(uqshl_asisdsame_only)           \
  V(uqshl_asisdshf_r)               \
  V(uqshl_z_p_zi)                   \
  V(uqshl_z_p_zz)                   \
  V(uqshlr_z_p_zz)                  \
  V(uqshrn_asimdshf_n)              \
  V(uqshrn_asisdshf_n)              \
  V(uqshrnb_z_zi)                   \
  V(uqshrnt_z_zi)                   \
  V(uqsub_asimdsame_only)           \
  V(uqsub_asisdsame_only)           \
  V(uqsub_z_p_zz)                   \
  V(uqsub_z_zi)                     \
  V(uqsub_z_zz)                     \
  V(uqsubr_z_p_zz)                  \
  V(uqxtn_asimdmisc_n)              \
  V(uqxtn_asisdmisc_n)              \
  V(uqxtnb_z_zz)                    \
  V(uqxtnt_z_zz)                    \
  V(urecpe_asimdmisc_r)             \
  V(urecpe_z_p_z)                   \
  V(urhadd_asimdsame_only)          \
  V(urhadd_z_p_zz)                  \
  V(urshl_asimdsame_only)           \
  V(urshl_asisdsame_only)           \
  V(urshl_z_p_zz)                   \
  V(urshlr_z_p_zz)                  \
  V(urshr_asimdshf_r)               \
  V(urshr_asisdshf_r)               \
  V(urshr_z_p_zi)                   \
  V(ursqrte_asimdmisc_r)            \
  V(ursqrte_z_p_z)                  \
  V(ursra_asimdshf_r)               \
  V(ursra_asisdshf_r)               \
  V(ursra_z_zi)                     \
  V(usdot_asimdelem_d)              \
  V(usdot_asimdsame2_d)             \
  V(usdot_z_zzz_s)                  \
  V(usdot_z_zzzi_s)                 \
  V(ushl_asimdsame_only)            \
  V(ushl_asisdsame_only)            \
  V(ushll_asimdshf_l)               \
  V(ushllb_z_zi)                    \
  V(ushllt_z_zi)                    \
  V(ushr_asimdshf_r)                \
  V(ushr_asisdshf_r)                \
  V(usmmla_asimdsame2_g)            \
  V(usmmla_z_zzz)                   \
  V(usqadd_asimdmisc_r)             \
  V(usqadd_asisdmisc_r)             \
  V(usqadd_z_p_zz)                  \
  V(usra_asimdshf_r)                \
  V(usra_asisdshf_r)                \
  V(usra_z_zi)                      \
  V(usubl_asimddiff_l)              \
  V(usublb_z_zz)                    \
  V(usublt_z_zz)                    \
  V(usubw_asimddiff_w)              \
  V(usubwb_z_zz)                    \
  V(usubwt_z_zz)                    \
  V(uunpkhi_z_z)                    \
  V(uunpklo_z_z)                    \
  V(uxtb_z_p_z)                     \
  V(uxth_z_p_z)                     \
  V(uxtw_z_p_z)                     \
  V(uzp1_asimdperm_only)            \
  V(uzp1_p_pp)                      \
  V(uzp1_z_zz)                      \
  V(uzp1_z_zz_q)                    \
  V(uzp2_asimdperm_only)            \
  V(uzp2_p_pp)                      \
  V(uzp2_z_zz)                      \
  V(uzp2_z_zz_q)                    \
  V(wfe_hi_hints)                   \
  V(wfet_only_systeminstrswithreg)  \
  V(wfi_hi_hints)                   \
  V(wfit_only_systeminstrswithreg)  \
  V(whilege_p_p_rr)                 \
  V(whilegt_p_p_rr)                 \
  V(whilehi_p_p_rr)                 \
  V(whilehs_p_p_rr)                 \
  V(whilele_p_p_rr)                 \
  V(whilelo_p_p_rr)                 \
  V(whilels_p_p_rr)                 \
  V(whilelt_p_p_rr)                 \
  V(whilerw_p_rr)                   \
  V(whilewr_p_rr)                   \
  V(wrffr_f_p)                      \
  V(xaflag_m_pstate)                \
  V(xar_vvv2_crypto3_imm6)          \
  V(xar_z_zzi)                      \
  V(xpacd_64z_dp_1src)              \
  V(xpaci_64z_dp_1src)              \
  V(xpaclri_hi_hints)               \
  V(xtn_asimdmisc_n)                \
  V(yield_hi_hints)                 \
  V(zip1_asimdperm_only)            \
  V(zip1_p_pp)                      \
  V(zip1_z_zz)                      \
  V(zip1_z_zz_q)                    \
  V(zip2_asimdperm_only)            \
  V(zip2_p_pp)                      \
  V(zip2_z_zz)                      \
  V(zip2_z_zz_q)                    \
  V(Unallocated)

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
  V(SVELoadAndBroadcastQOWord_ScalarPlusImm)                     \
  V(SVELoadAndBroadcastQOWord_ScalarPlusScalar)                  \
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

// The Visitor interface consists only of the Visit() method. User classes
// that inherit from this one must provide an implementation of the method.
// Information about the instruction encountered by the Decoder is available
// via the metadata pointer.
class DecoderVisitor {
 public:
  enum VisitorConstness { kConstVisitor, kNonConstVisitor };
  explicit DecoderVisitor(VisitorConstness constness = kConstVisitor)
      : constness_(constness) {}

  virtual ~DecoderVisitor() {}

  virtual void Visit(Metadata* metadata, const Instruction* instr) = 0;

  bool IsConstVisitor() const { return constness_ == kConstVisitor; }
  Instruction* MutableInstruction(const Instruction* instr) {
    VIXL_ASSERT(!IsConstVisitor());
    return const_cast<Instruction*>(instr);
  }

 private:
  const VisitorConstness constness_;
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

#define DECLARE(A) void Visit_##A(const Instruction* instr);
  INSTRUCTION_VISITOR_LIST(DECLARE)
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

const int kMaxDecodeSampledBits = 24;
const int kMaxDecodeMappings = 280;
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
  BitExtractFn GetBitExtractFunction(uint32_t mask) {
    return GetBitExtractFunctionHelper(mask, 0);
  }

  // Get a pointer to an Instruction method that applies a mask to the
  // instruction bits, and tests if the result is equal to value. The returned
  // function gives a 1 result if (inst & mask == value), 0 otherwise.
  BitExtractFn GetBitExtractFunction(uint32_t mask, uint32_t value) {
    return GetBitExtractFunctionHelper(value, mask);
  }

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

  // Helper function that returns a bit extracting function. If y is zero,
  // x is a bit extraction mask. Otherwise, y is the mask, and x is the value
  // to match after masking.
  BitExtractFn GetBitExtractFunctionHelper(uint32_t x, uint32_t y);

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
