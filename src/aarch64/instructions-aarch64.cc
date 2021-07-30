// Copyright 2015, VIXL authors
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

#include "instructions-aarch64.h"
#include "assembler-aarch64.h"

namespace vixl {
namespace aarch64 {

static uint64_t RepeatBitsAcrossReg(unsigned reg_size,
                                    uint64_t value,
                                    unsigned width) {
  VIXL_ASSERT((width == 2) || (width == 4) || (width == 8) || (width == 16) ||
              (width == 32));
  VIXL_ASSERT((reg_size == kBRegSize) || (reg_size == kHRegSize) ||
              (reg_size == kSRegSize) || (reg_size == kDRegSize));
  uint64_t result = value & ((UINT64_C(1) << width) - 1);
  for (unsigned i = width; i < reg_size; i *= 2) {
    result |= (result << i);
  }
  return result;
}

bool Instruction::CanTakeSVEMovprfx(const char* form,
                                    const Instruction* movprfx) const {
  return CanTakeSVEMovprfx(Hash(form), movprfx);
}

bool Instruction::CanTakeSVEMovprfx(uint32_t form_hash,
                                    const Instruction* movprfx) const {
  bool movprfx_is_predicated = movprfx->Mask(SVEMovprfxMask) == MOVPRFX_z_p_z;
  bool movprfx_is_unpredicated =
      movprfx->Mask(SVEConstructivePrefix_UnpredicatedMask) == MOVPRFX_z_z;
  VIXL_ASSERT(movprfx_is_predicated != movprfx_is_unpredicated);

  int movprfx_zd = movprfx->GetRd();
  int movprfx_pg = movprfx_is_predicated ? movprfx->GetPgLow8() : -1;
  VectorFormat movprfx_vform =
      movprfx_is_predicated ? movprfx->GetSVEVectorFormat() : kFormatUndefined;

  bool pg_matches_low8 = movprfx_pg == GetPgLow8();
  bool vform_matches = movprfx_vform == GetSVEVectorFormat();
  bool zd_matches = movprfx_zd == GetRd();
  bool zd_isnt_zn = movprfx_zd != GetRn();
  bool zd_isnt_zm = movprfx_zd != GetRm();

  switch (form_hash) {
    case Hash("cdot_z_zzzi_s"):
    case Hash("sdot_z_zzzi_s"):
    case Hash("sudot_z_zzzi_s"):
    case Hash("udot_z_zzzi_s"):
    case Hash("usdot_z_zzzi_s"):
      return (GetRd() != static_cast<int>(ExtractBits(18, 16))) &&
             movprfx_is_unpredicated && zd_isnt_zn && zd_matches;

    case Hash("cdot_z_zzzi_d"):
    case Hash("sdot_z_zzzi_d"):
    case Hash("udot_z_zzzi_d"):
      return (GetRd() != static_cast<int>(ExtractBits(19, 16))) &&
             movprfx_is_unpredicated && zd_isnt_zn && zd_matches;

    case Hash("fmlalb_z_zzzi_s"):
    case Hash("fmlalt_z_zzzi_s"):
    case Hash("fmlslb_z_zzzi_s"):
    case Hash("fmlslt_z_zzzi_s"):
    case Hash("smlalb_z_zzzi_d"):
    case Hash("smlalb_z_zzzi_s"):
    case Hash("smlalt_z_zzzi_d"):
    case Hash("smlalt_z_zzzi_s"):
    case Hash("smlslb_z_zzzi_d"):
    case Hash("smlslb_z_zzzi_s"):
    case Hash("smlslt_z_zzzi_d"):
    case Hash("smlslt_z_zzzi_s"):
    case Hash("sqdmlalb_z_zzzi_d"):
    case Hash("sqdmlalb_z_zzzi_s"):
    case Hash("sqdmlalt_z_zzzi_d"):
    case Hash("sqdmlalt_z_zzzi_s"):
    case Hash("sqdmlslb_z_zzzi_d"):
    case Hash("sqdmlslb_z_zzzi_s"):
    case Hash("sqdmlslt_z_zzzi_d"):
    case Hash("sqdmlslt_z_zzzi_s"):
    case Hash("umlalb_z_zzzi_d"):
    case Hash("umlalb_z_zzzi_s"):
    case Hash("umlalt_z_zzzi_d"):
    case Hash("umlalt_z_zzzi_s"):
    case Hash("umlslb_z_zzzi_d"):
    case Hash("umlslb_z_zzzi_s"):
    case Hash("umlslt_z_zzzi_d"):
    case Hash("umlslt_z_zzzi_s"):
      return (GetRd() != GetSVEMulLongZmAndIndex().first) &&
             movprfx_is_unpredicated && zd_isnt_zn && zd_matches;

    case Hash("cmla_z_zzzi_h"):
    case Hash("cmla_z_zzzi_s"):
    case Hash("fcmla_z_zzzi_h"):
    case Hash("fcmla_z_zzzi_s"):
    case Hash("fmla_z_zzzi_d"):
    case Hash("fmla_z_zzzi_h"):
    case Hash("fmla_z_zzzi_s"):
    case Hash("fmls_z_zzzi_d"):
    case Hash("fmls_z_zzzi_h"):
    case Hash("fmls_z_zzzi_s"):
    case Hash("mla_z_zzzi_d"):
    case Hash("mla_z_zzzi_h"):
    case Hash("mla_z_zzzi_s"):
    case Hash("mls_z_zzzi_d"):
    case Hash("mls_z_zzzi_h"):
    case Hash("mls_z_zzzi_s"):
    case Hash("sqrdcmlah_z_zzzi_h"):
    case Hash("sqrdcmlah_z_zzzi_s"):
    case Hash("sqrdmlah_z_zzzi_d"):
    case Hash("sqrdmlah_z_zzzi_h"):
    case Hash("sqrdmlah_z_zzzi_s"):
    case Hash("sqrdmlsh_z_zzzi_d"):
    case Hash("sqrdmlsh_z_zzzi_h"):
    case Hash("sqrdmlsh_z_zzzi_s"):
      return (GetRd() != GetSVEMulZmAndIndex().first) &&
             movprfx_is_unpredicated && zd_isnt_zn && zd_matches;

    case Hash("adclb_z_zzz"):
    case Hash("adclt_z_zzz"):
    case Hash("bcax_z_zzz"):
    case Hash("bsl1n_z_zzz"):
    case Hash("bsl2n_z_zzz"):
    case Hash("bsl_z_zzz"):
    case Hash("cdot_z_zzz"):
    case Hash("cmla_z_zzz"):
    case Hash("eor3_z_zzz"):
    case Hash("eorbt_z_zz"):
    case Hash("eortb_z_zz"):
    case Hash("fmlalb_z_zzz"):
    case Hash("fmlalt_z_zzz"):
    case Hash("fmlslb_z_zzz"):
    case Hash("fmlslt_z_zzz"):
    case Hash("nbsl_z_zzz"):
    case Hash("saba_z_zzz"):
    case Hash("sabalb_z_zzz"):
    case Hash("sabalt_z_zzz"):
    case Hash("sbclb_z_zzz"):
    case Hash("sbclt_z_zzz"):
    case Hash("sdot_z_zzz"):
    case Hash("smlalb_z_zzz"):
    case Hash("smlalt_z_zzz"):
    case Hash("smlslb_z_zzz"):
    case Hash("smlslt_z_zzz"):
    case Hash("sqdmlalb_z_zzz"):
    case Hash("sqdmlalbt_z_zzz"):
    case Hash("sqdmlalt_z_zzz"):
    case Hash("sqdmlslb_z_zzz"):
    case Hash("sqdmlslbt_z_zzz"):
    case Hash("sqdmlslt_z_zzz"):
    case Hash("sqrdcmlah_z_zzz"):
    case Hash("sqrdmlah_z_zzz"):
    case Hash("sqrdmlsh_z_zzz"):
    case Hash("uaba_z_zzz"):
    case Hash("uabalb_z_zzz"):
    case Hash("uabalt_z_zzz"):
    case Hash("udot_z_zzz"):
    case Hash("umlalb_z_zzz"):
    case Hash("umlalt_z_zzz"):
    case Hash("umlslb_z_zzz"):
    case Hash("umlslt_z_zzz"):
    case Hash("usdot_z_zzz_s"):
    case Hash("fmmla_z_zzz_s"):
    case Hash("fmmla_z_zzz_d"):
    case Hash("smmla_z_zzz"):
    case Hash("ummla_z_zzz"):
    case Hash("usmmla_z_zzz"):
      return movprfx_is_unpredicated && zd_isnt_zm && zd_isnt_zn && zd_matches;

    case Hash("addp_z_p_zz"):
    case Hash("cadd_z_zz"):
    case Hash("clasta_z_p_zz"):
    case Hash("clastb_z_p_zz"):
    case Hash("decd_z_zs"):
    case Hash("dech_z_zs"):
    case Hash("decw_z_zs"):
    case Hash("faddp_z_p_zz"):
    case Hash("fmaxnmp_z_p_zz"):
    case Hash("fmaxp_z_p_zz"):
    case Hash("fminnmp_z_p_zz"):
    case Hash("fminp_z_p_zz"):
    case Hash("ftmad_z_zzi"):
    case Hash("incd_z_zs"):
    case Hash("inch_z_zs"):
    case Hash("incw_z_zs"):
    case Hash("insr_z_v"):
    case Hash("smaxp_z_p_zz"):
    case Hash("sminp_z_p_zz"):
    case Hash("splice_z_p_zz_con"):
    case Hash("splice_z_p_zz_des"):
    case Hash("sqcadd_z_zz"):
    case Hash("sqdecd_z_zs"):
    case Hash("sqdech_z_zs"):
    case Hash("sqdecw_z_zs"):
    case Hash("sqincd_z_zs"):
    case Hash("sqinch_z_zs"):
    case Hash("sqincw_z_zs"):
    case Hash("srsra_z_zi"):
    case Hash("ssra_z_zi"):
    case Hash("umaxp_z_p_zz"):
    case Hash("uminp_z_p_zz"):
    case Hash("uqdecd_z_zs"):
    case Hash("uqdech_z_zs"):
    case Hash("uqdecw_z_zs"):
    case Hash("uqincd_z_zs"):
    case Hash("uqinch_z_zs"):
    case Hash("uqincw_z_zs"):
    case Hash("ursra_z_zi"):
    case Hash("usra_z_zi"):
    case Hash("xar_z_zzi"):
      return movprfx_is_unpredicated && zd_isnt_zn && zd_matches;

    case Hash("add_z_zi"):
    case Hash("and_z_zi"):
    case Hash("decp_z_p_z"):
    case Hash("eor_z_zi"):
    case Hash("incp_z_p_z"):
    case Hash("insr_z_r"):
    case Hash("mul_z_zi"):
    case Hash("orr_z_zi"):
    case Hash("smax_z_zi"):
    case Hash("smin_z_zi"):
    case Hash("sqadd_z_zi"):
    case Hash("sqdecp_z_p_z"):
    case Hash("sqincp_z_p_z"):
    case Hash("sqsub_z_zi"):
    case Hash("sub_z_zi"):
    case Hash("subr_z_zi"):
    case Hash("umax_z_zi"):
    case Hash("umin_z_zi"):
    case Hash("uqadd_z_zi"):
    case Hash("uqdecp_z_p_z"):
    case Hash("uqincp_z_p_z"):
    case Hash("uqsub_z_zi"):
      return movprfx_is_unpredicated && zd_matches;

    case Hash("cpy_z_p_i"):
      if (movprfx_is_predicated) {
        if (!vform_matches) return false;
        if (movprfx_pg != GetRx<19, 16>()) return false;
      }
      // Only the merging form can take movprfx.
      if (ExtractBit(14) == 0) return false;
      return zd_matches;

    case Hash("fcpy_z_p_i"):
      return (movprfx_is_unpredicated ||
              ((movprfx_pg == GetRx<19, 16>()) && vform_matches)) &&
             zd_matches;

    case Hash("flogb_z_p_z"):
      return (movprfx_is_unpredicated ||
              ((movprfx_vform == GetSVEVectorFormat(17)) && pg_matches_low8)) &&
             zd_isnt_zn && zd_matches;

    case Hash("asr_z_p_zi"):
    case Hash("asrd_z_p_zi"):
    case Hash("lsl_z_p_zi"):
    case Hash("lsr_z_p_zi"):
    case Hash("sqshl_z_p_zi"):
    case Hash("sqshlu_z_p_zi"):
    case Hash("srshr_z_p_zi"):
    case Hash("uqshl_z_p_zi"):
    case Hash("urshr_z_p_zi"):
      return (movprfx_is_unpredicated ||
              ((movprfx_vform ==
                SVEFormatFromLaneSizeInBytesLog2(
                    GetSVEImmShiftAndLaneSizeLog2(true).second)) &&
               pg_matches_low8)) &&
             zd_matches;

    case Hash("fcvt_z_p_z_d2h"):
    case Hash("fcvt_z_p_z_d2s"):
    case Hash("fcvt_z_p_z_h2d"):
    case Hash("fcvt_z_p_z_s2d"):
    case Hash("fcvtx_z_p_z_d2s"):
    case Hash("fcvtzs_z_p_z_d2w"):
    case Hash("fcvtzs_z_p_z_d2x"):
    case Hash("fcvtzs_z_p_z_fp162x"):
    case Hash("fcvtzs_z_p_z_s2x"):
    case Hash("fcvtzu_z_p_z_d2w"):
    case Hash("fcvtzu_z_p_z_d2x"):
    case Hash("fcvtzu_z_p_z_fp162x"):
    case Hash("fcvtzu_z_p_z_s2x"):
    case Hash("scvtf_z_p_z_w2d"):
    case Hash("scvtf_z_p_z_x2d"):
    case Hash("scvtf_z_p_z_x2fp16"):
    case Hash("scvtf_z_p_z_x2s"):
    case Hash("ucvtf_z_p_z_w2d"):
    case Hash("ucvtf_z_p_z_x2d"):
    case Hash("ucvtf_z_p_z_x2fp16"):
    case Hash("ucvtf_z_p_z_x2s"):
      return (movprfx_is_unpredicated ||
              ((movprfx_vform == kFormatVnD) && pg_matches_low8)) &&
             zd_isnt_zn && zd_matches;

    case Hash("fcvtzs_z_p_z_fp162h"):
    case Hash("fcvtzu_z_p_z_fp162h"):
    case Hash("scvtf_z_p_z_h2fp16"):
    case Hash("ucvtf_z_p_z_h2fp16"):
      return (movprfx_is_unpredicated ||
              ((movprfx_vform == kFormatVnH) && pg_matches_low8)) &&
             zd_isnt_zn && zd_matches;

    case Hash("fcvt_z_p_z_h2s"):
    case Hash("fcvt_z_p_z_s2h"):
    case Hash("fcvtzs_z_p_z_fp162w"):
    case Hash("fcvtzs_z_p_z_s2w"):
    case Hash("fcvtzu_z_p_z_fp162w"):
    case Hash("fcvtzu_z_p_z_s2w"):
    case Hash("scvtf_z_p_z_w2fp16"):
    case Hash("scvtf_z_p_z_w2s"):
    case Hash("ucvtf_z_p_z_w2fp16"):
    case Hash("ucvtf_z_p_z_w2s"):
      return (movprfx_is_unpredicated ||
              ((movprfx_vform == kFormatVnS) && pg_matches_low8)) &&
             zd_isnt_zn && zd_matches;

    case Hash("fcmla_z_p_zzz"):
    case Hash("fmad_z_p_zzz"):
    case Hash("fmla_z_p_zzz"):
    case Hash("fmls_z_p_zzz"):
    case Hash("fmsb_z_p_zzz"):
    case Hash("fnmad_z_p_zzz"):
    case Hash("fnmla_z_p_zzz"):
    case Hash("fnmls_z_p_zzz"):
    case Hash("fnmsb_z_p_zzz"):
    case Hash("mad_z_p_zzz"):
    case Hash("mla_z_p_zzz"):
    case Hash("mls_z_p_zzz"):
    case Hash("msb_z_p_zzz"):
      return (movprfx_is_unpredicated || (pg_matches_low8 && vform_matches)) &&
             zd_isnt_zm && zd_isnt_zn && zd_matches;

    case Hash("abs_z_p_z"):
    case Hash("add_z_p_zz"):
    case Hash("and_z_p_zz"):
    case Hash("asr_z_p_zw"):
    case Hash("asr_z_p_zz"):
    case Hash("asrr_z_p_zz"):
    case Hash("bic_z_p_zz"):
    case Hash("cls_z_p_z"):
    case Hash("clz_z_p_z"):
    case Hash("cnot_z_p_z"):
    case Hash("cnt_z_p_z"):
    case Hash("cpy_z_p_v"):
    case Hash("eor_z_p_zz"):
    case Hash("fabd_z_p_zz"):
    case Hash("fabs_z_p_z"):
    case Hash("fadd_z_p_zz"):
    case Hash("fcadd_z_p_zz"):
    case Hash("fdiv_z_p_zz"):
    case Hash("fdivr_z_p_zz"):
    case Hash("fmax_z_p_zz"):
    case Hash("fmaxnm_z_p_zz"):
    case Hash("fmin_z_p_zz"):
    case Hash("fminnm_z_p_zz"):
    case Hash("fmul_z_p_zz"):
    case Hash("fmulx_z_p_zz"):
    case Hash("fneg_z_p_z"):
    case Hash("frecpx_z_p_z"):
    case Hash("frinta_z_p_z"):
    case Hash("frinti_z_p_z"):
    case Hash("frintm_z_p_z"):
    case Hash("frintn_z_p_z"):
    case Hash("frintp_z_p_z"):
    case Hash("frintx_z_p_z"):
    case Hash("frintz_z_p_z"):
    case Hash("fscale_z_p_zz"):
    case Hash("fsqrt_z_p_z"):
    case Hash("fsub_z_p_zz"):
    case Hash("fsubr_z_p_zz"):
    case Hash("lsl_z_p_zw"):
    case Hash("lsl_z_p_zz"):
    case Hash("lslr_z_p_zz"):
    case Hash("lsr_z_p_zw"):
    case Hash("lsr_z_p_zz"):
    case Hash("lsrr_z_p_zz"):
    case Hash("mul_z_p_zz"):
    case Hash("neg_z_p_z"):
    case Hash("not_z_p_z"):
    case Hash("orr_z_p_zz"):
    case Hash("rbit_z_p_z"):
    case Hash("revb_z_z"):
    case Hash("revh_z_z"):
    case Hash("revw_z_z"):
    case Hash("sabd_z_p_zz"):
    case Hash("sadalp_z_p_z"):
    case Hash("sdiv_z_p_zz"):
    case Hash("sdivr_z_p_zz"):
    case Hash("shadd_z_p_zz"):
    case Hash("shsub_z_p_zz"):
    case Hash("shsubr_z_p_zz"):
    case Hash("smax_z_p_zz"):
    case Hash("smin_z_p_zz"):
    case Hash("smulh_z_p_zz"):
    case Hash("sqabs_z_p_z"):
    case Hash("sqadd_z_p_zz"):
    case Hash("sqneg_z_p_z"):
    case Hash("sqrshl_z_p_zz"):
    case Hash("sqrshlr_z_p_zz"):
    case Hash("sqshl_z_p_zz"):
    case Hash("sqshlr_z_p_zz"):
    case Hash("sqsub_z_p_zz"):
    case Hash("sqsubr_z_p_zz"):
    case Hash("srhadd_z_p_zz"):
    case Hash("srshl_z_p_zz"):
    case Hash("srshlr_z_p_zz"):
    case Hash("sub_z_p_zz"):
    case Hash("subr_z_p_zz"):
    case Hash("suqadd_z_p_zz"):
    case Hash("sxtb_z_p_z"):
    case Hash("sxth_z_p_z"):
    case Hash("sxtw_z_p_z"):
    case Hash("uabd_z_p_zz"):
    case Hash("uadalp_z_p_z"):
    case Hash("udiv_z_p_zz"):
    case Hash("udivr_z_p_zz"):
    case Hash("uhadd_z_p_zz"):
    case Hash("uhsub_z_p_zz"):
    case Hash("uhsubr_z_p_zz"):
    case Hash("umax_z_p_zz"):
    case Hash("umin_z_p_zz"):
    case Hash("umulh_z_p_zz"):
    case Hash("uqadd_z_p_zz"):
    case Hash("uqrshl_z_p_zz"):
    case Hash("uqrshlr_z_p_zz"):
    case Hash("uqshl_z_p_zz"):
    case Hash("uqshlr_z_p_zz"):
    case Hash("uqsub_z_p_zz"):
    case Hash("uqsubr_z_p_zz"):
    case Hash("urecpe_z_p_z"):
    case Hash("urhadd_z_p_zz"):
    case Hash("urshl_z_p_zz"):
    case Hash("urshlr_z_p_zz"):
    case Hash("ursqrte_z_p_z"):
    case Hash("usqadd_z_p_zz"):
    case Hash("uxtb_z_p_z"):
    case Hash("uxth_z_p_z"):
    case Hash("uxtw_z_p_z"):
      return (movprfx_is_unpredicated || (pg_matches_low8 && vform_matches)) &&
             zd_isnt_zn && zd_matches;

    case Hash("cpy_z_p_r"):
    case Hash("fadd_z_p_zs"):
    case Hash("fmax_z_p_zs"):
    case Hash("fmaxnm_z_p_zs"):
    case Hash("fmin_z_p_zs"):
    case Hash("fminnm_z_p_zs"):
    case Hash("fmul_z_p_zs"):
    case Hash("fsub_z_p_zs"):
    case Hash("fsubr_z_p_zs"):
      return (movprfx_is_unpredicated || (pg_matches_low8 && vform_matches)) &&
             zd_matches;
    default:
      return false;
  }
}  // NOLINT(readability/fn_size)

bool Instruction::IsLoad() const {
  if (Mask(LoadStoreAnyFMask) != LoadStoreAnyFixed) {
    return false;
  }

  if (Mask(LoadStorePairAnyFMask) == LoadStorePairAnyFixed) {
    return Mask(LoadStorePairLBit) != 0;
  } else {
    LoadStoreOp op = static_cast<LoadStoreOp>(Mask(LoadStoreMask));
    switch (op) {
      case LDRB_w:
      case LDRH_w:
      case LDR_w:
      case LDR_x:
      case LDRSB_w:
      case LDRSB_x:
      case LDRSH_w:
      case LDRSH_x:
      case LDRSW_x:
      case LDR_b:
      case LDR_h:
      case LDR_s:
      case LDR_d:
      case LDR_q:
        return true;
      default:
        return false;
    }
  }
}


bool Instruction::IsStore() const {
  if (Mask(LoadStoreAnyFMask) != LoadStoreAnyFixed) {
    return false;
  }

  if (Mask(LoadStorePairAnyFMask) == LoadStorePairAnyFixed) {
    return Mask(LoadStorePairLBit) == 0;
  } else {
    LoadStoreOp op = static_cast<LoadStoreOp>(Mask(LoadStoreMask));
    switch (op) {
      case STRB_w:
      case STRH_w:
      case STR_w:
      case STR_x:
      case STR_b:
      case STR_h:
      case STR_s:
      case STR_d:
      case STR_q:
        return true;
      default:
        return false;
    }
  }
}


std::pair<int, int> Instruction::GetSVEPermuteIndexAndLaneSizeLog2() const {
  uint32_t imm_2 = ExtractBits<0x00C00000>();
  uint32_t tsz_5 = ExtractBits<0x001F0000>();
  uint32_t imm_7 = (imm_2 << 5) | tsz_5;
  int lane_size_in_byte_log_2 = std::min(CountTrailingZeros(tsz_5), 5);
  int index = ExtractUnsignedBitfield32(6, lane_size_in_byte_log_2 + 1, imm_7);
  return std::make_pair(index, lane_size_in_byte_log_2);
}

// Get the register and index for SVE indexed multiplies encoded in the forms:
//  .h : Zm = <18:16>, index = <22><20:19>
//  .s : Zm = <18:16>, index = <20:19>
//  .d : Zm = <19:16>, index = <20>
std::pair<int, int> Instruction::GetSVEMulZmAndIndex() const {
  int reg_code = GetRmLow16();
  int index = ExtractBits(20, 19);

  // For .h, index uses bit zero of the size field, so kFormatVnB below implies
  // half-word lane, with most-significant bit of the index zero.
  switch (GetSVEVectorFormat()) {
    case kFormatVnD:
      index >>= 1;  // Only bit 20 in the index for D lanes.
      break;
    case kFormatVnH:
      index += 4;  // Bit 22 is the top bit of index.
      VIXL_FALLTHROUGH();
    case kFormatVnB:
    case kFormatVnS:
      reg_code &= 7;  // Three bits used for the register.
      break;
    default:
      VIXL_UNIMPLEMENTED();
      break;
  }
  return std::make_pair(reg_code, index);
}

// Get the register and index for SVE indexed long multiplies encoded in the
// forms:
//  .h : Zm = <18:16>, index = <20:19><11>
//  .s : Zm = <19:16>, index = <20><11>
std::pair<int, int> Instruction::GetSVEMulLongZmAndIndex() const {
  int reg_code = GetRmLow16();
  int index = ExtractBit(11);

  // For long multiplies, the SVE size field <23:22> encodes the destination
  // element size. The source element size is half the width.
  switch (GetSVEVectorFormat()) {
    case kFormatVnS:
      reg_code &= 7;
      index |= ExtractBits(20, 19) << 1;
      break;
    case kFormatVnD:
      index |= ExtractBit(20) << 1;
      break;
    default:
      VIXL_UNIMPLEMENTED();
      break;
  }
  return std::make_pair(reg_code, index);
}

// Logical immediates can't encode zero, so a return value of zero is used to
// indicate a failure case. Specifically, where the constraints on imm_s are
// not met.
uint64_t Instruction::GetImmLogical() const {
  unsigned reg_size = GetSixtyFourBits() ? kXRegSize : kWRegSize;
  int32_t n = GetBitN();
  int32_t imm_s = GetImmSetBits();
  int32_t imm_r = GetImmRotate();
  return DecodeImmBitMask(n, imm_s, imm_r, reg_size);
}

// Logical immediates can't encode zero, so a return value of zero is used to
// indicate a failure case. Specifically, where the constraints on imm_s are
// not met.
uint64_t Instruction::GetSVEImmLogical() const {
  int n = GetSVEBitN();
  int imm_s = GetSVEImmSetBits();
  int imm_r = GetSVEImmRotate();
  int lane_size_in_bytes_log2 = GetSVEBitwiseImmLaneSizeInBytesLog2();
  switch (lane_size_in_bytes_log2) {
    case kDRegSizeInBytesLog2:
    case kSRegSizeInBytesLog2:
    case kHRegSizeInBytesLog2:
    case kBRegSizeInBytesLog2: {
      int lane_size_in_bits = 1 << (lane_size_in_bytes_log2 + 3);
      return DecodeImmBitMask(n, imm_s, imm_r, lane_size_in_bits);
    }
    default:
      return 0;
  }
}

std::pair<int, int> Instruction::GetSVEImmShiftAndLaneSizeLog2(
    bool is_predicated) const {
  Instr tsize =
      is_predicated ? ExtractBits<0x00C00300>() : ExtractBits<0x00D80000>();
  Instr imm_3 =
      is_predicated ? ExtractBits<0x000000E0>() : ExtractBits<0x00070000>();
  if (tsize == 0) {
    // The bit field `tsize` means undefined if it is zero, so return a
    // convenience value kWMinInt to indicate a failure case.
    return std::make_pair(kWMinInt, kWMinInt);
  }

  int lane_size_in_bytes_log_2 = 32 - CountLeadingZeros(tsize, 32) - 1;
  int esize = (1 << lane_size_in_bytes_log_2) * kBitsPerByte;
  int shift = (2 * esize) - ((tsize << 3) | imm_3);
  return std::make_pair(shift, lane_size_in_bytes_log_2);
}

int Instruction::GetSVEMsizeFromDtype(bool is_signed, int dtype_h_lsb) const {
  Instr dtype_h = ExtractBits(dtype_h_lsb + 1, dtype_h_lsb);
  if (is_signed) {
    dtype_h = dtype_h ^ 0x3;
  }
  return dtype_h;
}

int Instruction::GetSVEEsizeFromDtype(bool is_signed, int dtype_l_lsb) const {
  Instr dtype_l = ExtractBits(dtype_l_lsb + 1, dtype_l_lsb);
  if (is_signed) {
    dtype_l = dtype_l ^ 0x3;
  }
  return dtype_l;
}

int Instruction::GetSVEBitwiseImmLaneSizeInBytesLog2() const {
  int n = GetSVEBitN();
  int imm_s = GetSVEImmSetBits();
  unsigned type_bitset =
      (n << SVEImmSetBits_width) | (~imm_s & GetUintMask(SVEImmSetBits_width));

  // An lane size is constructed from the n and imm_s bits according to
  // the following table:
  //
  // N   imms   size
  // 0  0xxxxx   32
  // 0  10xxxx   16
  // 0  110xxx    8
  // 0  1110xx    8
  // 0  11110x    8
  // 1  xxxxxx   64

  if (type_bitset == 0) {
    // Bail out early since `HighestSetBitPosition` doesn't accept zero
    // value input.
    return -1;
  }

  switch (HighestSetBitPosition(type_bitset)) {
    case 6:
      return kDRegSizeInBytesLog2;
    case 5:
      return kSRegSizeInBytesLog2;
    case 4:
      return kHRegSizeInBytesLog2;
    case 3:
    case 2:
    case 1:
      return kBRegSizeInBytesLog2;
    default:
      // RESERVED encoding.
      return -1;
  }
}

int Instruction::GetSVEExtractImmediate() const {
  const int imm8h_mask = 0x001F0000;
  const int imm8l_mask = 0x00001C00;
  return ExtractBits<imm8h_mask | imm8l_mask>();
}

uint64_t Instruction::DecodeImmBitMask(int32_t n,
                                       int32_t imm_s,
                                       int32_t imm_r,
                                       int32_t size) const {
  // An integer is constructed from the n, imm_s and imm_r bits according to
  // the following table:
  //
  //  N   imms    immr    size        S             R
  //  1  ssssss  rrrrrr    64    UInt(ssssss)  UInt(rrrrrr)
  //  0  0sssss  xrrrrr    32    UInt(sssss)   UInt(rrrrr)
  //  0  10ssss  xxrrrr    16    UInt(ssss)    UInt(rrrr)
  //  0  110sss  xxxrrr     8    UInt(sss)     UInt(rrr)
  //  0  1110ss  xxxxrr     4    UInt(ss)      UInt(rr)
  //  0  11110s  xxxxxr     2    UInt(s)       UInt(r)
  // (s bits must not be all set)
  //
  // A pattern is constructed of size bits, where the least significant S+1
  // bits are set. The pattern is rotated right by R, and repeated across a
  // 32 or 64-bit value, depending on destination register width.
  //

  if (n == 1) {
    if (imm_s == 0x3f) {
      return 0;
    }
    uint64_t bits = (UINT64_C(1) << (imm_s + 1)) - 1;
    return RotateRight(bits, imm_r, 64);
  } else {
    if ((imm_s >> 1) == 0x1f) {
      return 0;
    }
    for (int width = 0x20; width >= 0x2; width >>= 1) {
      if ((imm_s & width) == 0) {
        int mask = width - 1;
        if ((imm_s & mask) == mask) {
          return 0;
        }
        uint64_t bits = (UINT64_C(1) << ((imm_s & mask) + 1)) - 1;
        return RepeatBitsAcrossReg(size,
                                   RotateRight(bits, imm_r & mask, width),
                                   width);
      }
    }
  }
  VIXL_UNREACHABLE();
  return 0;
}


uint32_t Instruction::GetImmNEONabcdefgh() const {
  return GetImmNEONabc() << 5 | GetImmNEONdefgh();
}


Float16 Instruction::Imm8ToFloat16(uint32_t imm8) {
  // Imm8: abcdefgh (8 bits)
  // Half: aBbb.cdef.gh00.0000 (16 bits)
  // where B is b ^ 1
  uint32_t bits = imm8;
  uint16_t bit7 = (bits >> 7) & 0x1;
  uint16_t bit6 = (bits >> 6) & 0x1;
  uint16_t bit5_to_0 = bits & 0x3f;
  uint16_t result = (bit7 << 15) | ((4 - bit6) << 12) | (bit5_to_0 << 6);
  return RawbitsToFloat16(result);
}


float Instruction::Imm8ToFP32(uint32_t imm8) {
  // Imm8: abcdefgh (8 bits)
  // Single: aBbb.bbbc.defg.h000.0000.0000.0000.0000 (32 bits)
  // where B is b ^ 1
  uint32_t bits = imm8;
  uint32_t bit7 = (bits >> 7) & 0x1;
  uint32_t bit6 = (bits >> 6) & 0x1;
  uint32_t bit5_to_0 = bits & 0x3f;
  uint32_t result = (bit7 << 31) | ((32 - bit6) << 25) | (bit5_to_0 << 19);

  return RawbitsToFloat(result);
}


Float16 Instruction::GetImmFP16() const { return Imm8ToFloat16(GetImmFP()); }


float Instruction::GetImmFP32() const { return Imm8ToFP32(GetImmFP()); }


double Instruction::Imm8ToFP64(uint32_t imm8) {
  // Imm8: abcdefgh (8 bits)
  // Double: aBbb.bbbb.bbcd.efgh.0000.0000.0000.0000
  //         0000.0000.0000.0000.0000.0000.0000.0000 (64 bits)
  // where B is b ^ 1
  uint32_t bits = imm8;
  uint64_t bit7 = (bits >> 7) & 0x1;
  uint64_t bit6 = (bits >> 6) & 0x1;
  uint64_t bit5_to_0 = bits & 0x3f;
  uint64_t result = (bit7 << 63) | ((256 - bit6) << 54) | (bit5_to_0 << 48);

  return RawbitsToDouble(result);
}


double Instruction::GetImmFP64() const { return Imm8ToFP64(GetImmFP()); }


Float16 Instruction::GetImmNEONFP16() const {
  return Imm8ToFloat16(GetImmNEONabcdefgh());
}


float Instruction::GetImmNEONFP32() const {
  return Imm8ToFP32(GetImmNEONabcdefgh());
}


double Instruction::GetImmNEONFP64() const {
  return Imm8ToFP64(GetImmNEONabcdefgh());
}


unsigned CalcLSDataSize(LoadStoreOp op) {
  VIXL_ASSERT((LSSize_offset + LSSize_width) == (kInstructionSize * 8));
  unsigned size = static_cast<Instr>(op) >> LSSize_offset;
  if ((op & LSVector_mask) != 0) {
    // Vector register memory operations encode the access size in the "size"
    // and "opc" fields.
    if ((size == 0) && ((op & LSOpc_mask) >> LSOpc_offset) >= 2) {
      size = kQRegSizeInBytesLog2;
    }
  }
  return size;
}


unsigned CalcLSPairDataSize(LoadStorePairOp op) {
  VIXL_STATIC_ASSERT(kXRegSizeInBytes == kDRegSizeInBytes);
  VIXL_STATIC_ASSERT(kWRegSizeInBytes == kSRegSizeInBytes);
  switch (op) {
    case STP_q:
    case LDP_q:
      return kQRegSizeInBytesLog2;
    case STP_x:
    case LDP_x:
    case STP_d:
    case LDP_d:
      return kXRegSizeInBytesLog2;
    default:
      return kWRegSizeInBytesLog2;
  }
}


int Instruction::GetImmBranchRangeBitwidth(ImmBranchType branch_type) {
  switch (branch_type) {
    case UncondBranchType:
      return ImmUncondBranch_width;
    case CondBranchType:
      return ImmCondBranch_width;
    case CompareBranchType:
      return ImmCmpBranch_width;
    case TestBranchType:
      return ImmTestBranch_width;
    default:
      VIXL_UNREACHABLE();
      return 0;
  }
}


int32_t Instruction::GetImmBranchForwardRange(ImmBranchType branch_type) {
  int32_t encoded_max = 1 << (GetImmBranchRangeBitwidth(branch_type) - 1);
  return encoded_max * kInstructionSize;
}


bool Instruction::IsValidImmPCOffset(ImmBranchType branch_type,
                                     int64_t offset) {
  return IsIntN(GetImmBranchRangeBitwidth(branch_type), offset);
}


const Instruction* Instruction::GetImmPCOffsetTarget() const {
  const Instruction* base = this;
  ptrdiff_t offset;
  if (IsPCRelAddressing()) {
    // ADR and ADRP.
    offset = GetImmPCRel();
    if (Mask(PCRelAddressingMask) == ADRP) {
      base = AlignDown(base, kPageSize);
      offset *= kPageSize;
    } else {
      VIXL_ASSERT(Mask(PCRelAddressingMask) == ADR);
    }
  } else {
    // All PC-relative branches.
    VIXL_ASSERT(GetBranchType() != UnknownBranchType);
    // Relative branch offsets are instruction-size-aligned.
    offset = GetImmBranch() * static_cast<int>(kInstructionSize);
  }
  return base + offset;
}


int Instruction::GetImmBranch() const {
  switch (GetBranchType()) {
    case CondBranchType:
      return GetImmCondBranch();
    case UncondBranchType:
      return GetImmUncondBranch();
    case CompareBranchType:
      return GetImmCmpBranch();
    case TestBranchType:
      return GetImmTestBranch();
    default:
      VIXL_UNREACHABLE();
  }
  return 0;
}


void Instruction::SetImmPCOffsetTarget(const Instruction* target) {
  if (IsPCRelAddressing()) {
    SetPCRelImmTarget(target);
  } else {
    SetBranchImmTarget(target);
  }
}


void Instruction::SetPCRelImmTarget(const Instruction* target) {
  ptrdiff_t imm21;
  if ((Mask(PCRelAddressingMask) == ADR)) {
    imm21 = target - this;
  } else {
    VIXL_ASSERT(Mask(PCRelAddressingMask) == ADRP);
    uintptr_t this_page = reinterpret_cast<uintptr_t>(this) / kPageSize;
    uintptr_t target_page = reinterpret_cast<uintptr_t>(target) / kPageSize;
    imm21 = target_page - this_page;
  }
  Instr imm = Assembler::ImmPCRelAddress(static_cast<int32_t>(imm21));

  SetInstructionBits(Mask(~ImmPCRel_mask) | imm);
}


void Instruction::SetBranchImmTarget(const Instruction* target) {
  VIXL_ASSERT(((target - this) & 3) == 0);
  Instr branch_imm = 0;
  uint32_t imm_mask = 0;
  int offset = static_cast<int>((target - this) >> kInstructionSizeLog2);
  switch (GetBranchType()) {
    case CondBranchType: {
      branch_imm = Assembler::ImmCondBranch(offset);
      imm_mask = ImmCondBranch_mask;
      break;
    }
    case UncondBranchType: {
      branch_imm = Assembler::ImmUncondBranch(offset);
      imm_mask = ImmUncondBranch_mask;
      break;
    }
    case CompareBranchType: {
      branch_imm = Assembler::ImmCmpBranch(offset);
      imm_mask = ImmCmpBranch_mask;
      break;
    }
    case TestBranchType: {
      branch_imm = Assembler::ImmTestBranch(offset);
      imm_mask = ImmTestBranch_mask;
      break;
    }
    default:
      VIXL_UNREACHABLE();
  }
  SetInstructionBits(Mask(~imm_mask) | branch_imm);
}


void Instruction::SetImmLLiteral(const Instruction* source) {
  VIXL_ASSERT(IsWordAligned(source));
  ptrdiff_t offset = (source - this) >> kLiteralEntrySizeLog2;
  Instr imm = Assembler::ImmLLiteral(static_cast<int>(offset));
  Instr mask = ImmLLiteral_mask;

  SetInstructionBits(Mask(~mask) | imm);
}


VectorFormat VectorFormatHalfWidth(VectorFormat vform) {
  switch (vform) {
    case kFormat8H:
      return kFormat8B;
    case kFormat4S:
      return kFormat4H;
    case kFormat2D:
      return kFormat2S;
    case kFormatH:
      return kFormatB;
    case kFormatS:
      return kFormatH;
    case kFormatD:
      return kFormatS;
    case kFormatVnH:
      return kFormatVnB;
    case kFormatVnS:
      return kFormatVnH;
    case kFormatVnD:
      return kFormatVnS;
    default:
      VIXL_UNREACHABLE();
      return kFormatUndefined;
  }
}


VectorFormat VectorFormatDoubleWidth(VectorFormat vform) {
  switch (vform) {
    case kFormat8B:
      return kFormat8H;
    case kFormat4H:
      return kFormat4S;
    case kFormat2S:
      return kFormat2D;
    case kFormatB:
      return kFormatH;
    case kFormatH:
      return kFormatS;
    case kFormatS:
      return kFormatD;
    case kFormatVnB:
      return kFormatVnH;
    case kFormatVnH:
      return kFormatVnS;
    case kFormatVnS:
      return kFormatVnD;
    default:
      VIXL_UNREACHABLE();
      return kFormatUndefined;
  }
}


VectorFormat VectorFormatFillQ(VectorFormat vform) {
  switch (vform) {
    case kFormatB:
    case kFormat8B:
    case kFormat16B:
      return kFormat16B;
    case kFormatH:
    case kFormat4H:
    case kFormat8H:
      return kFormat8H;
    case kFormatS:
    case kFormat2S:
    case kFormat4S:
      return kFormat4S;
    case kFormatD:
    case kFormat1D:
    case kFormat2D:
      return kFormat2D;
    default:
      VIXL_UNREACHABLE();
      return kFormatUndefined;
  }
}

VectorFormat VectorFormatHalfWidthDoubleLanes(VectorFormat vform) {
  switch (vform) {
    case kFormat4H:
      return kFormat8B;
    case kFormat8H:
      return kFormat16B;
    case kFormat2S:
      return kFormat4H;
    case kFormat4S:
      return kFormat8H;
    case kFormat1D:
      return kFormat2S;
    case kFormat2D:
      return kFormat4S;
    case kFormatVnH:
      return kFormatVnB;
    case kFormatVnS:
      return kFormatVnH;
    case kFormatVnD:
      return kFormatVnS;
    default:
      VIXL_UNREACHABLE();
      return kFormatUndefined;
  }
}

VectorFormat VectorFormatDoubleLanes(VectorFormat vform) {
  VIXL_ASSERT(vform == kFormat8B || vform == kFormat4H || vform == kFormat2S);
  switch (vform) {
    case kFormat8B:
      return kFormat16B;
    case kFormat4H:
      return kFormat8H;
    case kFormat2S:
      return kFormat4S;
    default:
      VIXL_UNREACHABLE();
      return kFormatUndefined;
  }
}


VectorFormat VectorFormatHalfLanes(VectorFormat vform) {
  VIXL_ASSERT(vform == kFormat16B || vform == kFormat8H || vform == kFormat4S);
  switch (vform) {
    case kFormat16B:
      return kFormat8B;
    case kFormat8H:
      return kFormat4H;
    case kFormat4S:
      return kFormat2S;
    default:
      VIXL_UNREACHABLE();
      return kFormatUndefined;
  }
}


VectorFormat ScalarFormatFromLaneSize(int lane_size_in_bits) {
  switch (lane_size_in_bits) {
    case 8:
      return kFormatB;
    case 16:
      return kFormatH;
    case 32:
      return kFormatS;
    case 64:
      return kFormatD;
    default:
      VIXL_UNREACHABLE();
      return kFormatUndefined;
  }
}


bool IsSVEFormat(VectorFormat vform) {
  switch (vform) {
    case kFormatVnB:
    case kFormatVnH:
    case kFormatVnS:
    case kFormatVnD:
    case kFormatVnQ:
    case kFormatVnO:
      return true;
    default:
      return false;
  }
}


VectorFormat SVEFormatFromLaneSizeInBytes(int lane_size_in_bytes) {
  switch (lane_size_in_bytes) {
    case 1:
      return kFormatVnB;
    case 2:
      return kFormatVnH;
    case 4:
      return kFormatVnS;
    case 8:
      return kFormatVnD;
    case 16:
      return kFormatVnQ;
    default:
      VIXL_UNREACHABLE();
      return kFormatUndefined;
  }
}


VectorFormat SVEFormatFromLaneSizeInBits(int lane_size_in_bits) {
  switch (lane_size_in_bits) {
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
      return SVEFormatFromLaneSizeInBytes(lane_size_in_bits / kBitsPerByte);
    default:
      VIXL_UNREACHABLE();
      return kFormatUndefined;
  }
}


VectorFormat SVEFormatFromLaneSizeInBytesLog2(int lane_size_in_bytes_log2) {
  switch (lane_size_in_bytes_log2) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      return SVEFormatFromLaneSizeInBytes(1 << lane_size_in_bytes_log2);
    default:
      VIXL_UNREACHABLE();
      return kFormatUndefined;
  }
}


VectorFormat ScalarFormatFromFormat(VectorFormat vform) {
  return ScalarFormatFromLaneSize(LaneSizeInBitsFromFormat(vform));
}


unsigned RegisterSizeInBitsFromFormat(VectorFormat vform) {
  VIXL_ASSERT(vform != kFormatUndefined);
  VIXL_ASSERT(!IsSVEFormat(vform));
  switch (vform) {
    case kFormatB:
      return kBRegSize;
    case kFormatH:
      return kHRegSize;
    case kFormatS:
    case kFormat2H:
      return kSRegSize;
    case kFormatD:
    case kFormat8B:
    case kFormat4H:
    case kFormat2S:
    case kFormat1D:
      return kDRegSize;
    case kFormat16B:
    case kFormat8H:
    case kFormat4S:
    case kFormat2D:
      return kQRegSize;
    default:
      VIXL_UNREACHABLE();
      return 0;
  }
}


unsigned RegisterSizeInBytesFromFormat(VectorFormat vform) {
  return RegisterSizeInBitsFromFormat(vform) / 8;
}


unsigned LaneSizeInBitsFromFormat(VectorFormat vform) {
  VIXL_ASSERT(vform != kFormatUndefined);
  switch (vform) {
    case kFormatB:
    case kFormat8B:
    case kFormat16B:
    case kFormatVnB:
      return 8;
    case kFormatH:
    case kFormat2H:
    case kFormat4H:
    case kFormat8H:
    case kFormatVnH:
      return 16;
    case kFormatS:
    case kFormat2S:
    case kFormat4S:
    case kFormatVnS:
      return 32;
    case kFormatD:
    case kFormat1D:
    case kFormat2D:
    case kFormatVnD:
      return 64;
    case kFormatVnQ:
      return 128;
    case kFormatVnO:
      return 256;
    default:
      VIXL_UNREACHABLE();
      return 0;
  }
}


int LaneSizeInBytesFromFormat(VectorFormat vform) {
  return LaneSizeInBitsFromFormat(vform) / 8;
}


int LaneSizeInBytesLog2FromFormat(VectorFormat vform) {
  VIXL_ASSERT(vform != kFormatUndefined);
  switch (vform) {
    case kFormatB:
    case kFormat8B:
    case kFormat16B:
    case kFormatVnB:
      return 0;
    case kFormatH:
    case kFormat2H:
    case kFormat4H:
    case kFormat8H:
    case kFormatVnH:
      return 1;
    case kFormatS:
    case kFormat2S:
    case kFormat4S:
    case kFormatVnS:
      return 2;
    case kFormatD:
    case kFormat1D:
    case kFormat2D:
    case kFormatVnD:
      return 3;
    case kFormatVnQ:
      return 4;
    default:
      VIXL_UNREACHABLE();
      return 0;
  }
}


int LaneCountFromFormat(VectorFormat vform) {
  VIXL_ASSERT(vform != kFormatUndefined);
  switch (vform) {
    case kFormat16B:
      return 16;
    case kFormat8B:
    case kFormat8H:
      return 8;
    case kFormat4H:
    case kFormat4S:
      return 4;
    case kFormat2H:
    case kFormat2S:
    case kFormat2D:
      return 2;
    case kFormat1D:
    case kFormatB:
    case kFormatH:
    case kFormatS:
    case kFormatD:
      return 1;
    default:
      VIXL_UNREACHABLE();
      return 0;
  }
}


int MaxLaneCountFromFormat(VectorFormat vform) {
  VIXL_ASSERT(vform != kFormatUndefined);
  switch (vform) {
    case kFormatB:
    case kFormat8B:
    case kFormat16B:
      return 16;
    case kFormatH:
    case kFormat4H:
    case kFormat8H:
      return 8;
    case kFormatS:
    case kFormat2S:
    case kFormat4S:
      return 4;
    case kFormatD:
    case kFormat1D:
    case kFormat2D:
      return 2;
    default:
      VIXL_UNREACHABLE();
      return 0;
  }
}


// Does 'vform' indicate a vector format or a scalar format?
bool IsVectorFormat(VectorFormat vform) {
  VIXL_ASSERT(vform != kFormatUndefined);
  switch (vform) {
    case kFormatB:
    case kFormatH:
    case kFormatS:
    case kFormatD:
      return false;
    default:
      return true;
  }
}


int64_t MaxIntFromFormat(VectorFormat vform) {
  int lane_size = LaneSizeInBitsFromFormat(vform);
  return static_cast<int64_t>(GetUintMask(lane_size) >> 1);
}


int64_t MinIntFromFormat(VectorFormat vform) {
  return -MaxIntFromFormat(vform) - 1;
}


uint64_t MaxUintFromFormat(VectorFormat vform) {
  return GetUintMask(LaneSizeInBitsFromFormat(vform));
}

}  // namespace aarch64
}  // namespace vixl
