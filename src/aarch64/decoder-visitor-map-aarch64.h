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

// Initialisation data for a std::map, from instruction form to the visitor
// function that handles it. This allows reuse of existing visitor functions
// that support groups of instructions, though they may do extra decoding
// no longer needed.
// In the long term, it's expected that each component that uses the decoder
// will want to group instruction handling in the way most appropriate to
// the component's function, so this map initialisation will no longer be
// shared.

#define DEFAULT_FORM_TO_VISITOR_MAP(VISITORCLASS)                              \
  {"abs_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},          \
      {"addpl_r_ri", &VISITORCLASS::VisitSVEStackFrameAdjustment},             \
      {"addvl_r_ri", &VISITORCLASS::VisitSVEStackFrameAdjustment},             \
      {"add_z_p_zz", &VISITORCLASS::VisitSVEIntAddSubtractVectors_Predicated}, \
      {"add_z_zi", &VISITORCLASS::VisitSVEIntAddSubtractImm_Unpredicated},     \
      {"add_z_zz", &VISITORCLASS::VisitSVEIntArithmeticUnpredicated},          \
      {"adr_z_az_d_s32_scaled", &VISITORCLASS::VisitSVEAddressGeneration},     \
      {"adr_z_az_d_u32_scaled", &VISITORCLASS::VisitSVEAddressGeneration},     \
      {"adr_z_az_sd_same_scaled", &VISITORCLASS::VisitSVEAddressGeneration},   \
      {"ands_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},              \
      {"andv_r_p_z", &VISITORCLASS::VisitSVEIntReduction},                     \
      {"and_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},               \
      {"and_z_p_zz", &VISITORCLASS::VisitSVEBitwiseLogical_Predicated},        \
      {"and_z_zi", &VISITORCLASS::VisitSVEBitwiseLogicalWithImm_Unpredicated}, \
      {"and_z_zz", &VISITORCLASS::VisitSVEBitwiseLogicalUnpredicated},         \
      {"asrd_z_p_zi", &VISITORCLASS::VisitSVEBitwiseShiftByImm_Predicated},    \
      {"asrr_z_p_zz", &VISITORCLASS::VisitSVEBitwiseShiftByVector_Predicated}, \
      {"asr_z_p_zi", &VISITORCLASS::VisitSVEBitwiseShiftByImm_Predicated},     \
      {"asr_z_p_zw",                                                           \
       &VISITORCLASS::VisitSVEBitwiseShiftByWideElements_Predicated},          \
      {"asr_z_p_zz", &VISITORCLASS::VisitSVEBitwiseShiftByVector_Predicated},  \
      {"asr_z_zi", &VISITORCLASS::VisitSVEBitwiseShiftUnpredicated},           \
      {"asr_z_zw", &VISITORCLASS::VisitSVEBitwiseShiftUnpredicated},           \
      {"bics_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},              \
      {"bic_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},               \
      {"bic_z_p_zz", &VISITORCLASS::VisitSVEBitwiseLogical_Predicated},        \
      {"bic_z_zz", &VISITORCLASS::VisitSVEBitwiseLogicalUnpredicated},         \
      {"brkas_p_p_p_z", &VISITORCLASS::VisitSVEPartitionBreakCondition},       \
      {"brka_p_p_p", &VISITORCLASS::VisitSVEPartitionBreakCondition},          \
      {"brkbs_p_p_p_z", &VISITORCLASS::VisitSVEPartitionBreakCondition},       \
      {"brkb_p_p_p", &VISITORCLASS::VisitSVEPartitionBreakCondition},          \
      {"brkns_p_p_pp", &VISITORCLASS::VisitSVEPropagateBreakToNextPartition},  \
      {"brkn_p_p_pp", &VISITORCLASS::VisitSVEPropagateBreakToNextPartition},   \
      {"brkpas_p_p_pp", &VISITORCLASS::VisitSVEPropagateBreak},                \
      {"brkpa_p_p_pp", &VISITORCLASS::VisitSVEPropagateBreak},                 \
      {"brkpbs_p_p_pp", &VISITORCLASS::VisitSVEPropagateBreak},                \
      {"brkpb_p_p_pp", &VISITORCLASS::VisitSVEPropagateBreak},                 \
      {"clasta_r_p_z",                                                         \
       &VISITORCLASS::VisitSVEConditionallyExtractElementToGeneralRegister},   \
      {"clasta_v_p_z",                                                         \
       &VISITORCLASS::VisitSVEConditionallyExtractElementToSIMDFPScalar},      \
      {"clasta_z_p_zz",                                                        \
       &VISITORCLASS::VisitSVEConditionallyBroadcastElementToVector},          \
      {"clastb_r_p_z",                                                         \
       &VISITORCLASS::VisitSVEConditionallyExtractElementToGeneralRegister},   \
      {"clastb_v_p_z",                                                         \
       &VISITORCLASS::VisitSVEConditionallyExtractElementToSIMDFPScalar},      \
      {"clastb_z_p_zz",                                                        \
       &VISITORCLASS::VisitSVEConditionallyBroadcastElementToVector},          \
      {"cls_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},      \
      {"clz_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},      \
      {"cmpeq_p_p_zi", &VISITORCLASS::VisitSVEIntCompareSignedImm},            \
      {"cmpeq_p_p_zw", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmpeq_p_p_zz", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmpge_p_p_zi", &VISITORCLASS::VisitSVEIntCompareSignedImm},            \
      {"cmpge_p_p_zw", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmpge_p_p_zz", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmpgt_p_p_zi", &VISITORCLASS::VisitSVEIntCompareSignedImm},            \
      {"cmpgt_p_p_zw", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmpgt_p_p_zz", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmphi_p_p_zi", &VISITORCLASS::VisitSVEIntCompareUnsignedImm},          \
      {"cmphi_p_p_zw", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmphi_p_p_zz", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmphs_p_p_zi", &VISITORCLASS::VisitSVEIntCompareUnsignedImm},          \
      {"cmphs_p_p_zw", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmphs_p_p_zz", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmple_p_p_zi", &VISITORCLASS::VisitSVEIntCompareSignedImm},            \
      {"cmple_p_p_zw", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmplo_p_p_zi", &VISITORCLASS::VisitSVEIntCompareUnsignedImm},          \
      {"cmplo_p_p_zw", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmpls_p_p_zi", &VISITORCLASS::VisitSVEIntCompareUnsignedImm},          \
      {"cmpls_p_p_zw", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmplt_p_p_zi", &VISITORCLASS::VisitSVEIntCompareSignedImm},            \
      {"cmplt_p_p_zw", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmpne_p_p_zi", &VISITORCLASS::VisitSVEIntCompareSignedImm},            \
      {"cmpne_p_p_zw", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cmpne_p_p_zz", &VISITORCLASS::VisitSVEIntCompareVectors},              \
      {"cnot_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},     \
      {"cntb_r_s", &VISITORCLASS::VisitSVEElementCount},                       \
      {"cntd_r_s", &VISITORCLASS::VisitSVEElementCount},                       \
      {"cnth_r_s", &VISITORCLASS::VisitSVEElementCount},                       \
      {"cntp_r_p_p", &VISITORCLASS::VisitSVEPredicateCount},                   \
      {"cntw_r_s", &VISITORCLASS::VisitSVEElementCount},                       \
      {"cnt_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},      \
      {"compact_z_p_z", &VISITORCLASS::VisitSVECompressActiveElements},        \
      {"cpy_z_o_i", &VISITORCLASS::VisitSVECopyIntImm_Predicated},             \
      {"cpy_z_p_i", &VISITORCLASS::VisitSVECopyIntImm_Predicated},             \
      {"cpy_z_p_r",                                                            \
       &VISITORCLASS::VisitSVECopyGeneralRegisterToVector_Predicated},         \
      {"cpy_z_p_v",                                                            \
       &VISITORCLASS::VisitSVECopySIMDFPScalarRegisterToVector_Predicated},    \
      {"ctermeq_rr", &VISITORCLASS::VisitSVEConditionallyTerminateScalars},    \
      {"ctermne_rr", &VISITORCLASS::VisitSVEConditionallyTerminateScalars},    \
      {"decb_r_rs", &VISITORCLASS::VisitSVEIncDecRegisterByElementCount},      \
      {"decd_r_rs", &VISITORCLASS::VisitSVEIncDecRegisterByElementCount},      \
      {"decd_z_zs", &VISITORCLASS::VisitSVEIncDecVectorByElementCount},        \
      {"dech_r_rs", &VISITORCLASS::VisitSVEIncDecRegisterByElementCount},      \
      {"dech_z_zs", &VISITORCLASS::VisitSVEIncDecVectorByElementCount},        \
      {"decp_r_p_r", &VISITORCLASS::VisitSVEIncDecByPredicateCount},           \
      {"decp_z_p_z", &VISITORCLASS::VisitSVEIncDecByPredicateCount},           \
      {"decw_r_rs", &VISITORCLASS::VisitSVEIncDecRegisterByElementCount},      \
      {"decw_z_zs", &VISITORCLASS::VisitSVEIncDecVectorByElementCount},        \
      {"dupm_z_i", &VISITORCLASS::VisitSVEBroadcastBitmaskImm},                \
      {"dup_z_i", &VISITORCLASS::VisitSVEBroadcastIntImm_Unpredicated},        \
      {"dup_z_r", &VISITORCLASS::VisitSVEBroadcastGeneralRegister},            \
      {"dup_z_zi", &VISITORCLASS::VisitSVEBroadcastIndexElement},              \
      {"eors_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},              \
      {"eorv_r_p_z", &VISITORCLASS::VisitSVEIntReduction},                     \
      {"eor_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},               \
      {"eor_z_p_zz", &VISITORCLASS::VisitSVEBitwiseLogical_Predicated},        \
      {"eor_z_zi", &VISITORCLASS::VisitSVEBitwiseLogicalWithImm_Unpredicated}, \
      {"eor_z_zz", &VISITORCLASS::VisitSVEBitwiseLogicalUnpredicated},         \
      {"ext_z_zi_des", &VISITORCLASS::VisitSVEPermuteVectorExtract},           \
      {"fabd_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},         \
      {"fabs_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},     \
      {"facge_p_p_zz", &VISITORCLASS::VisitSVEFPCompareVectors},               \
      {"facgt_p_p_zz", &VISITORCLASS::VisitSVEFPCompareVectors},               \
      {"fadda_v_p_z", &VISITORCLASS::VisitSVEFPAccumulatingReduction},         \
      {"faddv_v_p_z", &VISITORCLASS::VisitSVEFPFastReduction},                 \
      {"fadd_z_p_zs", &VISITORCLASS::VisitSVEFPArithmeticWithImm_Predicated},  \
      {"fadd_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},         \
      {"fadd_z_zz", &VISITORCLASS::VisitSVEFPArithmeticUnpredicated},          \
      {"fcadd_z_p_zz", &VISITORCLASS::VisitSVEFPComplexAddition},              \
      {"fcmeq_p_p_z0", &VISITORCLASS::VisitSVEFPCompareWithZero},              \
      {"fcmeq_p_p_zz", &VISITORCLASS::VisitSVEFPCompareVectors},               \
      {"fcmge_p_p_z0", &VISITORCLASS::VisitSVEFPCompareWithZero},              \
      {"fcmge_p_p_zz", &VISITORCLASS::VisitSVEFPCompareVectors},               \
      {"fcmgt_p_p_z0", &VISITORCLASS::VisitSVEFPCompareWithZero},              \
      {"fcmgt_p_p_zz", &VISITORCLASS::VisitSVEFPCompareVectors},               \
      {"fcmla_z_p_zzz", &VISITORCLASS::VisitSVEFPComplexMulAdd},               \
      {"fcmla_z_zzzi_h", &VISITORCLASS::VisitSVEFPComplexMulAddIndex},         \
      {"fcmla_z_zzzi_s", &VISITORCLASS::VisitSVEFPComplexMulAddIndex},         \
      {"fcmle_p_p_z0", &VISITORCLASS::VisitSVEFPCompareWithZero},              \
      {"fcmlt_p_p_z0", &VISITORCLASS::VisitSVEFPCompareWithZero},              \
      {"fcmne_p_p_z0", &VISITORCLASS::VisitSVEFPCompareWithZero},              \
      {"fcmne_p_p_zz", &VISITORCLASS::VisitSVEFPCompareVectors},               \
      {"fcmuo_p_p_zz", &VISITORCLASS::VisitSVEFPCompareVectors},               \
      {"fcpy_z_p_i", &VISITORCLASS::VisitSVECopyFPImm_Predicated},             \
      {"fcvtzs_z_p_z_d2w", &VISITORCLASS::VisitSVEFPConvertToInt},             \
      {"fcvtzs_z_p_z_d2x", &VISITORCLASS::VisitSVEFPConvertToInt},             \
      {"fcvtzs_z_p_z_fp162h", &VISITORCLASS::VisitSVEFPConvertToInt},          \
      {"fcvtzs_z_p_z_fp162w", &VISITORCLASS::VisitSVEFPConvertToInt},          \
      {"fcvtzs_z_p_z_fp162x", &VISITORCLASS::VisitSVEFPConvertToInt},          \
      {"fcvtzs_z_p_z_s2w", &VISITORCLASS::VisitSVEFPConvertToInt},             \
      {"fcvtzs_z_p_z_s2x", &VISITORCLASS::VisitSVEFPConvertToInt},             \
      {"fcvtzu_z_p_z_d2w", &VISITORCLASS::VisitSVEFPConvertToInt},             \
      {"fcvtzu_z_p_z_d2x", &VISITORCLASS::VisitSVEFPConvertToInt},             \
      {"fcvtzu_z_p_z_fp162h", &VISITORCLASS::VisitSVEFPConvertToInt},          \
      {"fcvtzu_z_p_z_fp162w", &VISITORCLASS::VisitSVEFPConvertToInt},          \
      {"fcvtzu_z_p_z_fp162x", &VISITORCLASS::VisitSVEFPConvertToInt},          \
      {"fcvtzu_z_p_z_s2w", &VISITORCLASS::VisitSVEFPConvertToInt},             \
      {"fcvtzu_z_p_z_s2x", &VISITORCLASS::VisitSVEFPConvertToInt},             \
      {"fcvt_z_p_z_d2h", &VISITORCLASS::VisitSVEFPConvertPrecision},           \
      {"fcvt_z_p_z_d2s", &VISITORCLASS::VisitSVEFPConvertPrecision},           \
      {"fcvt_z_p_z_h2d", &VISITORCLASS::VisitSVEFPConvertPrecision},           \
      {"fcvt_z_p_z_h2s", &VISITORCLASS::VisitSVEFPConvertPrecision},           \
      {"fcvt_z_p_z_s2d", &VISITORCLASS::VisitSVEFPConvertPrecision},           \
      {"fcvt_z_p_z_s2h", &VISITORCLASS::VisitSVEFPConvertPrecision},           \
      {"fdivr_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},        \
      {"fdiv_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},         \
      {"fdup_z_i", &VISITORCLASS::VisitSVEBroadcastFPImm_Unpredicated},        \
      {"fexpa_z_z", &VISITORCLASS::VisitSVEFPExponentialAccelerator},          \
      {"fmad_z_p_zzz", &VISITORCLASS::VisitSVEFPMulAdd},                       \
      {"fmaxnmv_v_p_z", &VISITORCLASS::VisitSVEFPFastReduction},               \
      {"fmaxnm_z_p_zs",                                                        \
       &VISITORCLASS::VisitSVEFPArithmeticWithImm_Predicated},                 \
      {"fmaxnm_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},       \
      {"fmaxv_v_p_z", &VISITORCLASS::VisitSVEFPFastReduction},                 \
      {"fmax_z_p_zs", &VISITORCLASS::VisitSVEFPArithmeticWithImm_Predicated},  \
      {"fmax_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},         \
      {"fminnmv_v_p_z", &VISITORCLASS::VisitSVEFPFastReduction},               \
      {"fminnm_z_p_zs",                                                        \
       &VISITORCLASS::VisitSVEFPArithmeticWithImm_Predicated},                 \
      {"fminnm_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},       \
      {"fminv_v_p_z", &VISITORCLASS::VisitSVEFPFastReduction},                 \
      {"fmin_z_p_zs", &VISITORCLASS::VisitSVEFPArithmeticWithImm_Predicated},  \
      {"fmin_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},         \
      {"fmla_z_p_zzz", &VISITORCLASS::VisitSVEFPMulAdd},                       \
      {"fmla_z_zzzi_d", &VISITORCLASS::VisitSVEFPMulAddIndex},                 \
      {"fmla_z_zzzi_h", &VISITORCLASS::VisitSVEFPMulAddIndex},                 \
      {"fmla_z_zzzi_s", &VISITORCLASS::VisitSVEFPMulAddIndex},                 \
      {"fmls_z_p_zzz", &VISITORCLASS::VisitSVEFPMulAdd},                       \
      {"fmls_z_zzzi_d", &VISITORCLASS::VisitSVEFPMulAddIndex},                 \
      {"fmls_z_zzzi_h", &VISITORCLASS::VisitSVEFPMulAddIndex},                 \
      {"fmls_z_zzzi_s", &VISITORCLASS::VisitSVEFPMulAddIndex},                 \
      {"fmsb_z_p_zzz", &VISITORCLASS::VisitSVEFPMulAdd},                       \
      {"fmulx_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},        \
      {"fmul_z_p_zs", &VISITORCLASS::VisitSVEFPArithmeticWithImm_Predicated},  \
      {"fmul_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},         \
      {"fmul_z_zz", &VISITORCLASS::VisitSVEFPArithmeticUnpredicated},          \
      {"fmul_z_zzi_d", &VISITORCLASS::VisitSVEFPMulIndex},                     \
      {"fmul_z_zzi_h", &VISITORCLASS::VisitSVEFPMulIndex},                     \
      {"fmul_z_zzi_s", &VISITORCLASS::VisitSVEFPMulIndex},                     \
      {"fneg_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},     \
      {"fnmad_z_p_zzz", &VISITORCLASS::VisitSVEFPMulAdd},                      \
      {"fnmla_z_p_zzz", &VISITORCLASS::VisitSVEFPMulAdd},                      \
      {"fnmls_z_p_zzz", &VISITORCLASS::VisitSVEFPMulAdd},                      \
      {"fnmsb_z_p_zzz", &VISITORCLASS::VisitSVEFPMulAdd},                      \
      {"frecpe_z_z", &VISITORCLASS::VisitSVEFPUnaryOpUnpredicated},            \
      {"frecps_z_zz", &VISITORCLASS::VisitSVEFPArithmeticUnpredicated},        \
      {"frecpx_z_p_z", &VISITORCLASS::VisitSVEFPUnaryOp},                      \
      {"frinta_z_p_z", &VISITORCLASS::VisitSVEFPRoundToIntegralValue},         \
      {"frinti_z_p_z", &VISITORCLASS::VisitSVEFPRoundToIntegralValue},         \
      {"frintm_z_p_z", &VISITORCLASS::VisitSVEFPRoundToIntegralValue},         \
      {"frintn_z_p_z", &VISITORCLASS::VisitSVEFPRoundToIntegralValue},         \
      {"frintp_z_p_z", &VISITORCLASS::VisitSVEFPRoundToIntegralValue},         \
      {"frintx_z_p_z", &VISITORCLASS::VisitSVEFPRoundToIntegralValue},         \
      {"frintz_z_p_z", &VISITORCLASS::VisitSVEFPRoundToIntegralValue},         \
      {"frsqrte_z_z", &VISITORCLASS::VisitSVEFPUnaryOpUnpredicated},           \
      {"frsqrts_z_zz", &VISITORCLASS::VisitSVEFPArithmeticUnpredicated},       \
      {"fscale_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},       \
      {"fsqrt_z_p_z", &VISITORCLASS::VisitSVEFPUnaryOp},                       \
      {"fsubr_z_p_zs", &VISITORCLASS::VisitSVEFPArithmeticWithImm_Predicated}, \
      {"fsubr_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},        \
      {"fsub_z_p_zs", &VISITORCLASS::VisitSVEFPArithmeticWithImm_Predicated},  \
      {"fsub_z_p_zz", &VISITORCLASS::VisitSVEFPArithmetic_Predicated},         \
      {"fsub_z_zz", &VISITORCLASS::VisitSVEFPArithmeticUnpredicated},          \
      {"ftmad_z_zzi", &VISITORCLASS::VisitSVEFPTrigMulAddCoefficient},         \
      {"ftsmul_z_zz", &VISITORCLASS::VisitSVEFPArithmeticUnpredicated},        \
      {"ftssel_z_zz", &VISITORCLASS::VisitSVEFPTrigSelectCoefficient},         \
      {"incb_r_rs", &VISITORCLASS::VisitSVEIncDecRegisterByElementCount},      \
      {"incd_r_rs", &VISITORCLASS::VisitSVEIncDecRegisterByElementCount},      \
      {"incd_z_zs", &VISITORCLASS::VisitSVEIncDecVectorByElementCount},        \
      {"inch_r_rs", &VISITORCLASS::VisitSVEIncDecRegisterByElementCount},      \
      {"inch_z_zs", &VISITORCLASS::VisitSVEIncDecVectorByElementCount},        \
      {"incp_r_p_r", &VISITORCLASS::VisitSVEIncDecByPredicateCount},           \
      {"incp_z_p_z", &VISITORCLASS::VisitSVEIncDecByPredicateCount},           \
      {"incw_r_rs", &VISITORCLASS::VisitSVEIncDecRegisterByElementCount},      \
      {"incw_z_zs", &VISITORCLASS::VisitSVEIncDecVectorByElementCount},        \
      {"index_z_ii", &VISITORCLASS::VisitSVEIndexGeneration},                  \
      {"index_z_ir", &VISITORCLASS::VisitSVEIndexGeneration},                  \
      {"index_z_ri", &VISITORCLASS::VisitSVEIndexGeneration},                  \
      {"index_z_rr", &VISITORCLASS::VisitSVEIndexGeneration},                  \
      {"insr_z_r", &VISITORCLASS::VisitSVEInsertGeneralRegister},              \
      {"insr_z_v", &VISITORCLASS::VisitSVEInsertSIMDFPScalarRegister},         \
      {"lasta_r_p_z", &VISITORCLASS::VisitSVEExtractElementToGeneralRegister}, \
      {"lasta_v_p_z",                                                          \
       &VISITORCLASS::VisitSVEExtractElementToSIMDFPScalarRegister},           \
      {"lastb_r_p_z", &VISITORCLASS::VisitSVEExtractElementToGeneralRegister}, \
      {"lastb_v_p_z",                                                          \
       &VISITORCLASS::VisitSVEExtractElementToSIMDFPScalarRegister},           \
      {"ld1b_z_p_ai_d", &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm}, \
      {"ld1b_z_p_ai_s", &VISITORCLASS::VisitSVE32BitGatherLoad_VectorPlusImm}, \
      {"ld1b_z_p_bi_u16",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1b_z_p_bi_u32",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1b_z_p_bi_u64",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1b_z_p_bi_u8", &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm}, \
      {"ld1b_z_p_br_u16",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1b_z_p_br_u32",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1b_z_p_br_u64",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1b_z_p_br_u8",                                                       \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1b_z_p_bz_d_64_unscaled",                                            \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ld1b_z_p_bz_d_x32_unscaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ld1b_z_p_bz_s_x32_unscaled",                                           \
       &VISITORCLASS::VisitSVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets}, \
      {"ld1d_z_p_ai_d", &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm}, \
      {"ld1d_z_p_bi_u64",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1d_z_p_br_u64",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1d_z_p_bz_d_64_scaled",                                              \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitScaledOffsets},   \
      {"ld1d_z_p_bz_d_64_unscaled",                                            \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ld1d_z_p_bz_d_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsets},      \
      {"ld1d_z_p_bz_d_x32_unscaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ld1h_z_p_ai_d", &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm}, \
      {"ld1h_z_p_ai_s", &VISITORCLASS::VisitSVE32BitGatherLoad_VectorPlusImm}, \
      {"ld1h_z_p_bi_u16",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1h_z_p_bi_u32",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1h_z_p_bi_u64",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1h_z_p_br_u16",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1h_z_p_br_u32",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1h_z_p_br_u64",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1h_z_p_bz_d_64_scaled",                                              \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitScaledOffsets},   \
      {"ld1h_z_p_bz_d_64_unscaled",                                            \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ld1h_z_p_bz_d_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsets},      \
      {"ld1h_z_p_bz_d_x32_unscaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ld1h_z_p_bz_s_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE32BitGatherLoadHalfwords_ScalarPlus32BitScaledOffsets},     \
      {"ld1h_z_p_bz_s_x32_unscaled",                                           \
       &VISITORCLASS::VisitSVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets}, \
      {"ld1rb_z_p_bi_u16", &VISITORCLASS::VisitSVELoadAndBroadcastElement},    \
      {"ld1rb_z_p_bi_u32", &VISITORCLASS::VisitSVELoadAndBroadcastElement},    \
      {"ld1rb_z_p_bi_u64", &VISITORCLASS::VisitSVELoadAndBroadcastElement},    \
      {"ld1rb_z_p_bi_u8", &VISITORCLASS::VisitSVELoadAndBroadcastElement},     \
      {"ld1rd_z_p_bi_u64", &VISITORCLASS::VisitSVELoadAndBroadcastElement},    \
      {"ld1rh_z_p_bi_u16", &VISITORCLASS::VisitSVELoadAndBroadcastElement},    \
      {"ld1rh_z_p_bi_u32", &VISITORCLASS::VisitSVELoadAndBroadcastElement},    \
      {"ld1rh_z_p_bi_u64", &VISITORCLASS::VisitSVELoadAndBroadcastElement},    \
      {"ld1rqb_z_p_bi_u8",                                                     \
       &VISITORCLASS::VisitSVELoadAndBroadcastQOWord_ScalarPlusImm},           \
      {"ld1rqb_z_p_br_contiguous",                                             \
       &VISITORCLASS::VisitSVELoadAndBroadcastQOWord_ScalarPlusScalar},        \
      {"ld1rqd_z_p_bi_u64",                                                    \
       &VISITORCLASS::VisitSVELoadAndBroadcastQOWord_ScalarPlusImm},           \
      {"ld1rqd_z_p_br_contiguous",                                             \
       &VISITORCLASS::VisitSVELoadAndBroadcastQOWord_ScalarPlusScalar},        \
      {"ld1rqh_z_p_bi_u16",                                                    \
       &VISITORCLASS::VisitSVELoadAndBroadcastQOWord_ScalarPlusImm},           \
      {"ld1rqh_z_p_br_contiguous",                                             \
       &VISITORCLASS::VisitSVELoadAndBroadcastQOWord_ScalarPlusScalar},        \
      {"ld1rqw_z_p_bi_u32",                                                    \
       &VISITORCLASS::VisitSVELoadAndBroadcastQOWord_ScalarPlusImm},           \
      {"ld1rqw_z_p_br_contiguous",                                             \
       &VISITORCLASS::VisitSVELoadAndBroadcastQOWord_ScalarPlusScalar},        \
      {"ld1rsb_z_p_bi_s16", &VISITORCLASS::VisitSVELoadAndBroadcastElement},   \
      {"ld1rsb_z_p_bi_s32", &VISITORCLASS::VisitSVELoadAndBroadcastElement},   \
      {"ld1rsb_z_p_bi_s64", &VISITORCLASS::VisitSVELoadAndBroadcastElement},   \
      {"ld1rsh_z_p_bi_s32", &VISITORCLASS::VisitSVELoadAndBroadcastElement},   \
      {"ld1rsh_z_p_bi_s64", &VISITORCLASS::VisitSVELoadAndBroadcastElement},   \
      {"ld1rsw_z_p_bi_s64", &VISITORCLASS::VisitSVELoadAndBroadcastElement},   \
      {"ld1rw_z_p_bi_u32", &VISITORCLASS::VisitSVELoadAndBroadcastElement},    \
      {"ld1rw_z_p_bi_u64", &VISITORCLASS::VisitSVELoadAndBroadcastElement},    \
      {"ld1sb_z_p_ai_d",                                                       \
       &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm},                  \
      {"ld1sb_z_p_ai_s",                                                       \
       &VISITORCLASS::VisitSVE32BitGatherLoad_VectorPlusImm},                  \
      {"ld1sb_z_p_bi_s16",                                                     \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1sb_z_p_bi_s32",                                                     \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1sb_z_p_bi_s64",                                                     \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1sb_z_p_br_s16",                                                     \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1sb_z_p_br_s32",                                                     \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1sb_z_p_br_s64",                                                     \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1sb_z_p_bz_d_64_unscaled",                                           \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ld1sb_z_p_bz_d_x32_unscaled",                                          \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ld1sb_z_p_bz_s_x32_unscaled",                                          \
       &VISITORCLASS::VisitSVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets}, \
      {"ld1sh_z_p_ai_d",                                                       \
       &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm},                  \
      {"ld1sh_z_p_ai_s",                                                       \
       &VISITORCLASS::VisitSVE32BitGatherLoad_VectorPlusImm},                  \
      {"ld1sh_z_p_bi_s32",                                                     \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1sh_z_p_bi_s64",                                                     \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1sh_z_p_br_s32",                                                     \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1sh_z_p_br_s64",                                                     \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1sh_z_p_bz_d_64_scaled",                                             \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitScaledOffsets},   \
      {"ld1sh_z_p_bz_d_64_unscaled",                                           \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ld1sh_z_p_bz_d_x32_scaled",                                            \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsets},      \
      {"ld1sh_z_p_bz_d_x32_unscaled",                                          \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ld1sh_z_p_bz_s_x32_scaled",                                            \
       &VISITORCLASS::                                                         \
           VisitSVE32BitGatherLoadHalfwords_ScalarPlus32BitScaledOffsets},     \
      {"ld1sh_z_p_bz_s_x32_unscaled",                                          \
       &VISITORCLASS::VisitSVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets}, \
      {"ld1sw_z_p_ai_d",                                                       \
       &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm},                  \
      {"ld1sw_z_p_bi_s64",                                                     \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1sw_z_p_br_s64",                                                     \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1sw_z_p_bz_d_64_scaled",                                             \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitScaledOffsets},   \
      {"ld1sw_z_p_bz_d_64_unscaled",                                           \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ld1sw_z_p_bz_d_x32_scaled",                                            \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsets},      \
      {"ld1sw_z_p_bz_d_x32_unscaled",                                          \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ld1w_z_p_ai_d", &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm}, \
      {"ld1w_z_p_ai_s", &VISITORCLASS::VisitSVE32BitGatherLoad_VectorPlusImm}, \
      {"ld1w_z_p_bi_u32",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1w_z_p_bi_u64",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusImm},                   \
      {"ld1w_z_p_br_u32",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1w_z_p_br_u64",                                                      \
       &VISITORCLASS::VisitSVEContiguousLoad_ScalarPlusScalar},                \
      {"ld1w_z_p_bz_d_64_scaled",                                              \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitScaledOffsets},   \
      {"ld1w_z_p_bz_d_64_unscaled",                                            \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ld1w_z_p_bz_d_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsets},      \
      {"ld1w_z_p_bz_d_x32_unscaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ld1w_z_p_bz_s_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE32BitGatherLoadWords_ScalarPlus32BitScaledOffsets},         \
      {"ld1w_z_p_bz_s_x32_unscaled",                                           \
       &VISITORCLASS::VisitSVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets}, \
      {"ld2b_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusImm},           \
      {"ld2b_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusScalar},        \
      {"ld2d_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusImm},           \
      {"ld2d_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusScalar},        \
      {"ld2h_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusImm},           \
      {"ld2h_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusScalar},        \
      {"ld2w_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusImm},           \
      {"ld2w_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusScalar},        \
      {"ld3b_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusImm},           \
      {"ld3b_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusScalar},        \
      {"ld3d_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusImm},           \
      {"ld3d_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusScalar},        \
      {"ld3h_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusImm},           \
      {"ld3h_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusScalar},        \
      {"ld3w_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusImm},           \
      {"ld3w_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusScalar},        \
      {"ld4b_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusImm},           \
      {"ld4b_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusScalar},        \
      {"ld4d_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusImm},           \
      {"ld4d_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusScalar},        \
      {"ld4h_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusImm},           \
      {"ld4h_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusScalar},        \
      {"ld4w_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusImm},           \
      {"ld4w_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVELoadMultipleStructures_ScalarPlusScalar},        \
      {"ldff1b_z_p_ai_d",                                                      \
       &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm},                  \
      {"ldff1b_z_p_ai_s",                                                      \
       &VISITORCLASS::VisitSVE32BitGatherLoad_VectorPlusImm},                  \
      {"ldff1b_z_p_br_u16",                                                    \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1b_z_p_br_u32",                                                    \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1b_z_p_br_u64",                                                    \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1b_z_p_br_u8",                                                     \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1b_z_p_bz_d_64_unscaled",                                          \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ldff1b_z_p_bz_d_x32_unscaled",                                         \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ldff1b_z_p_bz_s_x32_unscaled",                                         \
       &VISITORCLASS::VisitSVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets}, \
      {"ldff1d_z_p_ai_d",                                                      \
       &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm},                  \
      {"ldff1d_z_p_br_u64",                                                    \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1d_z_p_bz_d_64_scaled",                                            \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitScaledOffsets},   \
      {"ldff1d_z_p_bz_d_64_unscaled",                                          \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ldff1d_z_p_bz_d_x32_scaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsets},      \
      {"ldff1d_z_p_bz_d_x32_unscaled",                                         \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ldff1h_z_p_ai_d",                                                      \
       &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm},                  \
      {"ldff1h_z_p_ai_s",                                                      \
       &VISITORCLASS::VisitSVE32BitGatherLoad_VectorPlusImm},                  \
      {"ldff1h_z_p_br_u16",                                                    \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1h_z_p_br_u32",                                                    \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1h_z_p_br_u64",                                                    \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1h_z_p_bz_d_64_scaled",                                            \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitScaledOffsets},   \
      {"ldff1h_z_p_bz_d_64_unscaled",                                          \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ldff1h_z_p_bz_d_x32_scaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsets},      \
      {"ldff1h_z_p_bz_d_x32_unscaled",                                         \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ldff1h_z_p_bz_s_x32_scaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE32BitGatherLoadHalfwords_ScalarPlus32BitScaledOffsets},     \
      {"ldff1h_z_p_bz_s_x32_unscaled",                                         \
       &VISITORCLASS::VisitSVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets}, \
      {"ldff1sb_z_p_ai_d",                                                     \
       &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm},                  \
      {"ldff1sb_z_p_ai_s",                                                     \
       &VISITORCLASS::VisitSVE32BitGatherLoad_VectorPlusImm},                  \
      {"ldff1sb_z_p_br_s16",                                                   \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1sb_z_p_br_s32",                                                   \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1sb_z_p_br_s64",                                                   \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1sb_z_p_bz_d_64_unscaled",                                         \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ldff1sb_z_p_bz_d_x32_unscaled",                                        \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ldff1sb_z_p_bz_s_x32_unscaled",                                        \
       &VISITORCLASS::VisitSVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets}, \
      {"ldff1sh_z_p_ai_d",                                                     \
       &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm},                  \
      {"ldff1sh_z_p_ai_s",                                                     \
       &VISITORCLASS::VisitSVE32BitGatherLoad_VectorPlusImm},                  \
      {"ldff1sh_z_p_br_s32",                                                   \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1sh_z_p_br_s64",                                                   \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1sh_z_p_bz_d_64_scaled",                                           \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitScaledOffsets},   \
      {"ldff1sh_z_p_bz_d_64_unscaled",                                         \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ldff1sh_z_p_bz_d_x32_scaled",                                          \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsets},      \
      {"ldff1sh_z_p_bz_d_x32_unscaled",                                        \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ldff1sh_z_p_bz_s_x32_scaled",                                          \
       &VISITORCLASS::                                                         \
           VisitSVE32BitGatherLoadHalfwords_ScalarPlus32BitScaledOffsets},     \
      {"ldff1sh_z_p_bz_s_x32_unscaled",                                        \
       &VISITORCLASS::VisitSVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets}, \
      {"ldff1sw_z_p_ai_d",                                                     \
       &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm},                  \
      {"ldff1sw_z_p_br_s64",                                                   \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1sw_z_p_bz_d_64_scaled",                                           \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitScaledOffsets},   \
      {"ldff1sw_z_p_bz_d_64_unscaled",                                         \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ldff1sw_z_p_bz_d_x32_scaled",                                          \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsets},      \
      {"ldff1sw_z_p_bz_d_x32_unscaled",                                        \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ldff1w_z_p_ai_d",                                                      \
       &VISITORCLASS::VisitSVE64BitGatherLoad_VectorPlusImm},                  \
      {"ldff1w_z_p_ai_s",                                                      \
       &VISITORCLASS::VisitSVE32BitGatherLoad_VectorPlusImm},                  \
      {"ldff1w_z_p_br_u32",                                                    \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1w_z_p_br_u64",                                                    \
       &VISITORCLASS::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar},      \
      {"ldff1w_z_p_bz_d_64_scaled",                                            \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitScaledOffsets},   \
      {"ldff1w_z_p_bz_d_64_unscaled",                                          \
       &VISITORCLASS::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets}, \
      {"ldff1w_z_p_bz_d_x32_scaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsets},      \
      {"ldff1w_z_p_bz_d_x32_unscaled",                                         \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets},    \
      {"ldff1w_z_p_bz_s_x32_scaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE32BitGatherLoadWords_ScalarPlus32BitScaledOffsets},         \
      {"ldff1w_z_p_bz_s_x32_unscaled",                                         \
       &VISITORCLASS::VisitSVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets}, \
      {"ldnf1b_z_p_bi_u16",                                                    \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1b_z_p_bi_u32",                                                    \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1b_z_p_bi_u64",                                                    \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1b_z_p_bi_u8",                                                     \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1d_z_p_bi_u64",                                                    \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1h_z_p_bi_u16",                                                    \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1h_z_p_bi_u32",                                                    \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1h_z_p_bi_u64",                                                    \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1sb_z_p_bi_s16",                                                   \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1sb_z_p_bi_s32",                                                   \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1sb_z_p_bi_s64",                                                   \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1sh_z_p_bi_s32",                                                   \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1sh_z_p_bi_s64",                                                   \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1sw_z_p_bi_s64",                                                   \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1w_z_p_bi_u32",                                                    \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnf1w_z_p_bi_u64",                                                    \
       &VISITORCLASS::VisitSVEContiguousNonFaultLoad_ScalarPlusImm},           \
      {"ldnt1b_z_p_bi_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalLoad_ScalarPlusImm},        \
      {"ldnt1b_z_p_br_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalLoad_ScalarPlusScalar},     \
      {"ldnt1d_z_p_bi_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalLoad_ScalarPlusImm},        \
      {"ldnt1d_z_p_br_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalLoad_ScalarPlusScalar},     \
      {"ldnt1h_z_p_bi_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalLoad_ScalarPlusImm},        \
      {"ldnt1h_z_p_br_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalLoad_ScalarPlusScalar},     \
      {"ldnt1w_z_p_bi_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalLoad_ScalarPlusImm},        \
      {"ldnt1w_z_p_br_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalLoad_ScalarPlusScalar},     \
      {"ldr_p_bi", &VISITORCLASS::VisitSVELoadPredicateRegister},              \
      {"ldr_z_bi", &VISITORCLASS::VisitSVELoadVectorRegister},                 \
      {"lslr_z_p_zz", &VISITORCLASS::VisitSVEBitwiseShiftByVector_Predicated}, \
      {"lsl_z_p_zi", &VISITORCLASS::VisitSVEBitwiseShiftByImm_Predicated},     \
      {"lsl_z_p_zw",                                                           \
       &VISITORCLASS::VisitSVEBitwiseShiftByWideElements_Predicated},          \
      {"lsl_z_p_zz", &VISITORCLASS::VisitSVEBitwiseShiftByVector_Predicated},  \
      {"lsl_z_zi", &VISITORCLASS::VisitSVEBitwiseShiftUnpredicated},           \
      {"lsl_z_zw", &VISITORCLASS::VisitSVEBitwiseShiftUnpredicated},           \
      {"lsrr_z_p_zz", &VISITORCLASS::VisitSVEBitwiseShiftByVector_Predicated}, \
      {"lsr_z_p_zi", &VISITORCLASS::VisitSVEBitwiseShiftByImm_Predicated},     \
      {"lsr_z_p_zw",                                                           \
       &VISITORCLASS::VisitSVEBitwiseShiftByWideElements_Predicated},          \
      {"lsr_z_p_zz", &VISITORCLASS::VisitSVEBitwiseShiftByVector_Predicated},  \
      {"lsr_z_zi", &VISITORCLASS::VisitSVEBitwiseShiftUnpredicated},           \
      {"lsr_z_zw", &VISITORCLASS::VisitSVEBitwiseShiftUnpredicated},           \
      {"mad_z_p_zzz", &VISITORCLASS::VisitSVEIntMulAddPredicated},             \
      {"mla_z_p_zzz", &VISITORCLASS::VisitSVEIntMulAddPredicated},             \
      {"mls_z_p_zzz", &VISITORCLASS::VisitSVEIntMulAddPredicated},             \
      {"movprfx_z_p_z", &VISITORCLASS::VisitSVEMovprfx},                       \
      {"movprfx_z_z", &VISITORCLASS::VisitSVEConstructivePrefix_Unpredicated}, \
      {"msb_z_p_zzz", &VISITORCLASS::VisitSVEIntMulAddPredicated},             \
      {"mul_z_p_zz", &VISITORCLASS::VisitSVEIntMulVectors_Predicated},         \
      {"mul_z_zi", &VISITORCLASS::VisitSVEIntMulImm_Unpredicated},             \
      {"nands_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},             \
      {"nand_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},              \
      {"neg_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},      \
      {"nors_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},              \
      {"nor_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},               \
      {"not_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},      \
      {"orns_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},              \
      {"orn_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},               \
      {"orrs_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},              \
      {"orr_p_p_pp_z", &VISITORCLASS::VisitSVEPredicateLogical},               \
      {"orr_z_p_zz", &VISITORCLASS::VisitSVEBitwiseLogical_Predicated},        \
      {"orr_z_zi", &VISITORCLASS::VisitSVEBitwiseLogicalWithImm_Unpredicated}, \
      {"orr_z_zz", &VISITORCLASS::VisitSVEBitwiseLogicalUnpredicated},         \
      {"orv_r_p_z", &VISITORCLASS::VisitSVEIntReduction},                      \
      {"pfalse_p", &VISITORCLASS::VisitSVEPredicateZero},                      \
      {"pfirst_p_p_p", &VISITORCLASS::VisitSVEPredicateFirstActive},           \
      {"pnext_p_p_p", &VISITORCLASS::VisitSVEPredicateNextActive},             \
      {"prfb_i_p_ai_d",                                                        \
       &VISITORCLASS::VisitSVE64BitGatherPrefetch_VectorPlusImm},              \
      {"prfb_i_p_ai_s",                                                        \
       &VISITORCLASS::VisitSVE32BitGatherPrefetch_VectorPlusImm},              \
      {"prfb_i_p_bi_s",                                                        \
       &VISITORCLASS::VisitSVEContiguousPrefetch_ScalarPlusImm},               \
      {"prfb_i_p_br_s",                                                        \
       &VISITORCLASS::VisitSVEContiguousPrefetch_ScalarPlusScalar},            \
      {"prfb_i_p_bz_d_64_scaled",                                              \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherPrefetch_ScalarPlus64BitScaledOffsets},          \
      {"prfb_i_p_bz_d_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherPrefetch_ScalarPlusUnpacked32BitScaledOffsets},  \
      {"prfb_i_p_bz_s_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE32BitGatherPrefetch_ScalarPlus32BitScaledOffsets},          \
      {"prfd_i_p_ai_d",                                                        \
       &VISITORCLASS::VisitSVE64BitGatherPrefetch_VectorPlusImm},              \
      {"prfd_i_p_ai_s",                                                        \
       &VISITORCLASS::VisitSVE32BitGatherPrefetch_VectorPlusImm},              \
      {"prfd_i_p_bi_s",                                                        \
       &VISITORCLASS::VisitSVEContiguousPrefetch_ScalarPlusImm},               \
      {"prfd_i_p_br_s",                                                        \
       &VISITORCLASS::VisitSVEContiguousPrefetch_ScalarPlusScalar},            \
      {"prfd_i_p_bz_d_64_scaled",                                              \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherPrefetch_ScalarPlus64BitScaledOffsets},          \
      {"prfd_i_p_bz_d_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherPrefetch_ScalarPlusUnpacked32BitScaledOffsets},  \
      {"prfd_i_p_bz_s_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE32BitGatherPrefetch_ScalarPlus32BitScaledOffsets},          \
      {"prfh_i_p_ai_d",                                                        \
       &VISITORCLASS::VisitSVE64BitGatherPrefetch_VectorPlusImm},              \
      {"prfh_i_p_ai_s",                                                        \
       &VISITORCLASS::VisitSVE32BitGatherPrefetch_VectorPlusImm},              \
      {"prfh_i_p_bi_s",                                                        \
       &VISITORCLASS::VisitSVEContiguousPrefetch_ScalarPlusImm},               \
      {"prfh_i_p_br_s",                                                        \
       &VISITORCLASS::VisitSVEContiguousPrefetch_ScalarPlusScalar},            \
      {"prfh_i_p_bz_d_64_scaled",                                              \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherPrefetch_ScalarPlus64BitScaledOffsets},          \
      {"prfh_i_p_bz_d_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherPrefetch_ScalarPlusUnpacked32BitScaledOffsets},  \
      {"prfh_i_p_bz_s_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE32BitGatherPrefetch_ScalarPlus32BitScaledOffsets},          \
      {"prfw_i_p_ai_d",                                                        \
       &VISITORCLASS::VisitSVE64BitGatherPrefetch_VectorPlusImm},              \
      {"prfw_i_p_ai_s",                                                        \
       &VISITORCLASS::VisitSVE32BitGatherPrefetch_VectorPlusImm},              \
      {"prfw_i_p_bi_s",                                                        \
       &VISITORCLASS::VisitSVEContiguousPrefetch_ScalarPlusImm},               \
      {"prfw_i_p_br_s",                                                        \
       &VISITORCLASS::VisitSVEContiguousPrefetch_ScalarPlusScalar},            \
      {"prfw_i_p_bz_d_64_scaled",                                              \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherPrefetch_ScalarPlus64BitScaledOffsets},          \
      {"prfw_i_p_bz_d_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE64BitGatherPrefetch_ScalarPlusUnpacked32BitScaledOffsets},  \
      {"prfw_i_p_bz_s_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE32BitGatherPrefetch_ScalarPlus32BitScaledOffsets},          \
      {"ptest_p_p", &VISITORCLASS::VisitSVEPredicateTest},                     \
      {"ptrues_p_s", &VISITORCLASS::VisitSVEPredicateInitialize},              \
      {"ptrue_p_s", &VISITORCLASS::VisitSVEPredicateInitialize},               \
      {"punpkhi_p_p", &VISITORCLASS::VisitSVEUnpackPredicateElements},         \
      {"punpklo_p_p", &VISITORCLASS::VisitSVEUnpackPredicateElements},         \
      {"rbit_z_p_z", &VISITORCLASS::VisitSVEReverseWithinElements},            \
      {"rdffrs_p_p_f",                                                         \
       &VISITORCLASS::VisitSVEPredicateReadFromFFR_Predicated},                \
      {"rdffr_p_f", &VISITORCLASS::VisitSVEPredicateReadFromFFR_Unpredicated}, \
      {"rdffr_p_p_f", &VISITORCLASS::VisitSVEPredicateReadFromFFR_Predicated}, \
      {"rdvl_r_i", &VISITORCLASS::VisitSVEStackFrameSize},                     \
      {"revb_z_z", &VISITORCLASS::VisitSVEReverseWithinElements},              \
      {"revh_z_z", &VISITORCLASS::VisitSVEReverseWithinElements},              \
      {"revw_z_z", &VISITORCLASS::VisitSVEReverseWithinElements},              \
      {"rev_p_p", &VISITORCLASS::VisitSVEReversePredicateElements},            \
      {"rev_z_z", &VISITORCLASS::VisitSVEReverseVectorElements},               \
      {"sabd_z_p_zz", &VISITORCLASS::VisitSVEIntMinMaxDifference_Predicated},  \
      {"saddv_r_p_z", &VISITORCLASS::VisitSVEIntReduction},                    \
      {"scvtf_z_p_z_h2fp16", &VISITORCLASS::VisitSVEIntConvertToFP},           \
      {"scvtf_z_p_z_w2d", &VISITORCLASS::VisitSVEIntConvertToFP},              \
      {"scvtf_z_p_z_w2fp16", &VISITORCLASS::VisitSVEIntConvertToFP},           \
      {"scvtf_z_p_z_w2s", &VISITORCLASS::VisitSVEIntConvertToFP},              \
      {"scvtf_z_p_z_x2d", &VISITORCLASS::VisitSVEIntConvertToFP},              \
      {"scvtf_z_p_z_x2fp16", &VISITORCLASS::VisitSVEIntConvertToFP},           \
      {"scvtf_z_p_z_x2s", &VISITORCLASS::VisitSVEIntConvertToFP},              \
      {"sdivr_z_p_zz", &VISITORCLASS::VisitSVEIntDivideVectors_Predicated},    \
      {"sdiv_z_p_zz", &VISITORCLASS::VisitSVEIntDivideVectors_Predicated},     \
      {"sdot_z_zzz", &VISITORCLASS::VisitSVEIntMulAddUnpredicated},            \
      {"sdot_z_zzzi_d", &VISITORCLASS::VisitSVEMulIndex},                      \
      {"sdot_z_zzzi_s", &VISITORCLASS::VisitSVEMulIndex},                      \
      {"sel_p_p_pp", &VISITORCLASS::VisitSVEPredicateLogical},                 \
      {"sel_z_p_zz", &VISITORCLASS::VisitSVEVectorSelect},                     \
      {"setffr_f", &VISITORCLASS::VisitSVEFFRInitialise},                      \
      {"smaxv_r_p_z", &VISITORCLASS::VisitSVEIntReduction},                    \
      {"smax_z_p_zz", &VISITORCLASS::VisitSVEIntMinMaxDifference_Predicated},  \
      {"smax_z_zi", &VISITORCLASS::VisitSVEIntMinMaxImm_Unpredicated},         \
      {"sminv_r_p_z", &VISITORCLASS::VisitSVEIntReduction},                    \
      {"smin_z_p_zz", &VISITORCLASS::VisitSVEIntMinMaxDifference_Predicated},  \
      {"smin_z_zi", &VISITORCLASS::VisitSVEIntMinMaxImm_Unpredicated},         \
      {"smulh_z_p_zz", &VISITORCLASS::VisitSVEIntMulVectors_Predicated},       \
      {"splice_z_p_zz_des", &VISITORCLASS::VisitSVEVectorSplice},              \
      {"sqadd_z_zi", &VISITORCLASS::VisitSVEIntAddSubtractImm_Unpredicated},   \
      {"sqadd_z_zz", &VISITORCLASS::VisitSVEIntArithmeticUnpredicated},        \
      {"sqdecb_r_rs_sx",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqdecb_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqdecd_r_rs_sx",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqdecd_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqdecd_z_zs",                                                          \
       &VISITORCLASS::VisitSVESaturatingIncDecVectorByElementCount},           \
      {"sqdech_r_rs_sx",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqdech_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqdech_z_zs",                                                          \
       &VISITORCLASS::VisitSVESaturatingIncDecVectorByElementCount},           \
      {"sqdecp_r_p_r_sx", &VISITORCLASS::VisitSVEIncDecByPredicateCount},      \
      {"sqdecp_r_p_r_x", &VISITORCLASS::VisitSVEIncDecByPredicateCount},       \
      {"sqdecp_z_p_z", &VISITORCLASS::VisitSVEIncDecByPredicateCount},         \
      {"sqdecw_r_rs_sx",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqdecw_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqdecw_z_zs",                                                          \
       &VISITORCLASS::VisitSVESaturatingIncDecVectorByElementCount},           \
      {"sqincb_r_rs_sx",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqincb_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqincd_r_rs_sx",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqincd_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqincd_z_zs",                                                          \
       &VISITORCLASS::VisitSVESaturatingIncDecVectorByElementCount},           \
      {"sqinch_r_rs_sx",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqinch_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqinch_z_zs",                                                          \
       &VISITORCLASS::VisitSVESaturatingIncDecVectorByElementCount},           \
      {"sqincp_r_p_r_sx", &VISITORCLASS::VisitSVEIncDecByPredicateCount},      \
      {"sqincp_r_p_r_x", &VISITORCLASS::VisitSVEIncDecByPredicateCount},       \
      {"sqincp_z_p_z", &VISITORCLASS::VisitSVEIncDecByPredicateCount},         \
      {"sqincw_r_rs_sx",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqincw_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"sqincw_z_zs",                                                          \
       &VISITORCLASS::VisitSVESaturatingIncDecVectorByElementCount},           \
      {"sqsub_z_zi", &VISITORCLASS::VisitSVEIntAddSubtractImm_Unpredicated},   \
      {"sqsub_z_zz", &VISITORCLASS::VisitSVEIntArithmeticUnpredicated},        \
      {"st1b_z_p_ai_d",                                                        \
       &VISITORCLASS::VisitSVE64BitScatterStore_VectorPlusImm},                \
      {"st1b_z_p_ai_s",                                                        \
       &VISITORCLASS::VisitSVE32BitScatterStore_VectorPlusImm},                \
      {"st1b_z_p_bi", &VISITORCLASS::VisitSVEContiguousStore_ScalarPlusImm},   \
      {"st1b_z_p_br",                                                          \
       &VISITORCLASS::VisitSVEContiguousStore_ScalarPlusScalar},               \
      {"st1b_z_p_bz_d_64_unscaled",                                            \
       &VISITORCLASS::                                                         \
           VisitSVE64BitScatterStore_ScalarPlus64BitUnscaledOffsets},          \
      {"st1b_z_p_bz_d_x32_unscaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE64BitScatterStore_ScalarPlusUnpacked32BitUnscaledOffsets},  \
      {"st1b_z_p_bz_s_x32_unscaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE32BitScatterStore_ScalarPlus32BitUnscaledOffsets},          \
      {"st1d_z_p_ai_d",                                                        \
       &VISITORCLASS::VisitSVE64BitScatterStore_VectorPlusImm},                \
      {"st1d_z_p_bi", &VISITORCLASS::VisitSVEContiguousStore_ScalarPlusImm},   \
      {"st1d_z_p_br",                                                          \
       &VISITORCLASS::VisitSVEContiguousStore_ScalarPlusScalar},               \
      {"st1d_z_p_bz_d_64_scaled",                                              \
       &VISITORCLASS::VisitSVE64BitScatterStore_ScalarPlus64BitScaledOffsets}, \
      {"st1d_z_p_bz_d_64_unscaled",                                            \
       &VISITORCLASS::                                                         \
           VisitSVE64BitScatterStore_ScalarPlus64BitUnscaledOffsets},          \
      {"st1d_z_p_bz_d_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE64BitScatterStore_ScalarPlusUnpacked32BitScaledOffsets},    \
      {"st1d_z_p_bz_d_x32_unscaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE64BitScatterStore_ScalarPlusUnpacked32BitUnscaledOffsets},  \
      {"st1h_z_p_ai_d",                                                        \
       &VISITORCLASS::VisitSVE64BitScatterStore_VectorPlusImm},                \
      {"st1h_z_p_ai_s",                                                        \
       &VISITORCLASS::VisitSVE32BitScatterStore_VectorPlusImm},                \
      {"st1h_z_p_bi", &VISITORCLASS::VisitSVEContiguousStore_ScalarPlusImm},   \
      {"st1h_z_p_br",                                                          \
       &VISITORCLASS::VisitSVEContiguousStore_ScalarPlusScalar},               \
      {"st1h_z_p_bz_d_64_scaled",                                              \
       &VISITORCLASS::VisitSVE64BitScatterStore_ScalarPlus64BitScaledOffsets}, \
      {"st1h_z_p_bz_d_64_unscaled",                                            \
       &VISITORCLASS::                                                         \
           VisitSVE64BitScatterStore_ScalarPlus64BitUnscaledOffsets},          \
      {"st1h_z_p_bz_d_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE64BitScatterStore_ScalarPlusUnpacked32BitScaledOffsets},    \
      {"st1h_z_p_bz_d_x32_unscaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE64BitScatterStore_ScalarPlusUnpacked32BitUnscaledOffsets},  \
      {"st1h_z_p_bz_s_x32_scaled",                                             \
       &VISITORCLASS::VisitSVE32BitScatterStore_ScalarPlus32BitScaledOffsets}, \
      {"st1h_z_p_bz_s_x32_unscaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE32BitScatterStore_ScalarPlus32BitUnscaledOffsets},          \
      {"st1w_z_p_ai_d",                                                        \
       &VISITORCLASS::VisitSVE64BitScatterStore_VectorPlusImm},                \
      {"st1w_z_p_ai_s",                                                        \
       &VISITORCLASS::VisitSVE32BitScatterStore_VectorPlusImm},                \
      {"st1w_z_p_bi", &VISITORCLASS::VisitSVEContiguousStore_ScalarPlusImm},   \
      {"st1w_z_p_br",                                                          \
       &VISITORCLASS::VisitSVEContiguousStore_ScalarPlusScalar},               \
      {"st1w_z_p_bz_d_64_scaled",                                              \
       &VISITORCLASS::VisitSVE64BitScatterStore_ScalarPlus64BitScaledOffsets}, \
      {"st1w_z_p_bz_d_64_unscaled",                                            \
       &VISITORCLASS::                                                         \
           VisitSVE64BitScatterStore_ScalarPlus64BitUnscaledOffsets},          \
      {"st1w_z_p_bz_d_x32_scaled",                                             \
       &VISITORCLASS::                                                         \
           VisitSVE64BitScatterStore_ScalarPlusUnpacked32BitScaledOffsets},    \
      {"st1w_z_p_bz_d_x32_unscaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE64BitScatterStore_ScalarPlusUnpacked32BitUnscaledOffsets},  \
      {"st1w_z_p_bz_s_x32_scaled",                                             \
       &VISITORCLASS::VisitSVE32BitScatterStore_ScalarPlus32BitScaledOffsets}, \
      {"st1w_z_p_bz_s_x32_unscaled",                                           \
       &VISITORCLASS::                                                         \
           VisitSVE32BitScatterStore_ScalarPlus32BitUnscaledOffsets},          \
      {"st2b_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusImm},          \
      {"st2b_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusScalar},       \
      {"st2d_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusImm},          \
      {"st2d_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusScalar},       \
      {"st2h_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusImm},          \
      {"st2h_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusScalar},       \
      {"st2w_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusImm},          \
      {"st2w_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusScalar},       \
      {"st3b_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusImm},          \
      {"st3b_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusScalar},       \
      {"st3d_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusImm},          \
      {"st3d_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusScalar},       \
      {"st3h_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusImm},          \
      {"st3h_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusScalar},       \
      {"st3w_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusImm},          \
      {"st3w_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusScalar},       \
      {"st4b_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusImm},          \
      {"st4b_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusScalar},       \
      {"st4d_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusImm},          \
      {"st4d_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusScalar},       \
      {"st4h_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusImm},          \
      {"st4h_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusScalar},       \
      {"st4w_z_p_bi_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusImm},          \
      {"st4w_z_p_br_contiguous",                                               \
       &VISITORCLASS::VisitSVEStoreMultipleStructures_ScalarPlusScalar},       \
      {"stnt1b_z_p_bi_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalStore_ScalarPlusImm},       \
      {"stnt1b_z_p_br_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalStore_ScalarPlusScalar},    \
      {"stnt1d_z_p_bi_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalStore_ScalarPlusImm},       \
      {"stnt1d_z_p_br_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalStore_ScalarPlusScalar},    \
      {"stnt1h_z_p_bi_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalStore_ScalarPlusImm},       \
      {"stnt1h_z_p_br_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalStore_ScalarPlusScalar},    \
      {"stnt1w_z_p_bi_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalStore_ScalarPlusImm},       \
      {"stnt1w_z_p_br_contiguous",                                             \
       &VISITORCLASS::VisitSVEContiguousNonTemporalStore_ScalarPlusScalar},    \
      {"str_p_bi", &VISITORCLASS::VisitSVEStorePredicateRegister},             \
      {"str_z_bi", &VISITORCLASS::VisitSVEStoreVectorRegister},                \
      {"subr_z_p_zz",                                                          \
       &VISITORCLASS::VisitSVEIntAddSubtractVectors_Predicated},               \
      {"subr_z_zi", &VISITORCLASS::VisitSVEIntAddSubtractImm_Unpredicated},    \
      {"sub_z_p_zz", &VISITORCLASS::VisitSVEIntAddSubtractVectors_Predicated}, \
      {"sub_z_zi", &VISITORCLASS::VisitSVEIntAddSubtractImm_Unpredicated},     \
      {"sub_z_zz", &VISITORCLASS::VisitSVEIntArithmeticUnpredicated},          \
      {"sunpkhi_z_z", &VISITORCLASS::VisitSVEUnpackVectorElements},            \
      {"sunpklo_z_z", &VISITORCLASS::VisitSVEUnpackVectorElements},            \
      {"sxtb_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},     \
      {"sxth_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},     \
      {"sxtw_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},     \
      {"tbl_z_zz_1", &VISITORCLASS::VisitSVETableLookup},                      \
      {"trn1_p_pp", &VISITORCLASS::VisitSVEPermutePredicateElements},          \
      {"trn1_z_zz", &VISITORCLASS::VisitSVEPermuteVectorInterleaving},         \
      {"trn2_p_pp", &VISITORCLASS::VisitSVEPermutePredicateElements},          \
      {"trn2_z_zz", &VISITORCLASS::VisitSVEPermuteVectorInterleaving},         \
      {"uabd_z_p_zz", &VISITORCLASS::VisitSVEIntMinMaxDifference_Predicated},  \
      {"uaddv_r_p_z", &VISITORCLASS::VisitSVEIntReduction},                    \
      {"ucvtf_z_p_z_h2fp16", &VISITORCLASS::VisitSVEIntConvertToFP},           \
      {"ucvtf_z_p_z_w2d", &VISITORCLASS::VisitSVEIntConvertToFP},              \
      {"ucvtf_z_p_z_w2fp16", &VISITORCLASS::VisitSVEIntConvertToFP},           \
      {"ucvtf_z_p_z_w2s", &VISITORCLASS::VisitSVEIntConvertToFP},              \
      {"ucvtf_z_p_z_x2d", &VISITORCLASS::VisitSVEIntConvertToFP},              \
      {"ucvtf_z_p_z_x2fp16", &VISITORCLASS::VisitSVEIntConvertToFP},           \
      {"ucvtf_z_p_z_x2s", &VISITORCLASS::VisitSVEIntConvertToFP},              \
      {"udf_only_perm_undef", &VISITORCLASS::VisitReserved},                   \
      {"udivr_z_p_zz", &VISITORCLASS::VisitSVEIntDivideVectors_Predicated},    \
      {"udiv_z_p_zz", &VISITORCLASS::VisitSVEIntDivideVectors_Predicated},     \
      {"udot_z_zzz", &VISITORCLASS::VisitSVEIntMulAddUnpredicated},            \
      {"udot_z_zzzi_d", &VISITORCLASS::VisitSVEMulIndex},                      \
      {"udot_z_zzzi_s", &VISITORCLASS::VisitSVEMulIndex},                      \
      {"umaxv_r_p_z", &VISITORCLASS::VisitSVEIntReduction},                    \
      {"umax_z_p_zz", &VISITORCLASS::VisitSVEIntMinMaxDifference_Predicated},  \
      {"umax_z_zi", &VISITORCLASS::VisitSVEIntMinMaxImm_Unpredicated},         \
      {"uminv_r_p_z", &VISITORCLASS::VisitSVEIntReduction},                    \
      {"umin_z_p_zz", &VISITORCLASS::VisitSVEIntMinMaxDifference_Predicated},  \
      {"umin_z_zi", &VISITORCLASS::VisitSVEIntMinMaxImm_Unpredicated},         \
      {"umulh_z_p_zz", &VISITORCLASS::VisitSVEIntMulVectors_Predicated},       \
      {"uqadd_z_zi", &VISITORCLASS::VisitSVEIntAddSubtractImm_Unpredicated},   \
      {"uqadd_z_zz", &VISITORCLASS::VisitSVEIntArithmeticUnpredicated},        \
      {"uqdecb_r_rs_uw",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqdecb_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqdecd_r_rs_uw",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqdecd_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqdecd_z_zs",                                                          \
       &VISITORCLASS::VisitSVESaturatingIncDecVectorByElementCount},           \
      {"uqdech_r_rs_uw",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqdech_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqdech_z_zs",                                                          \
       &VISITORCLASS::VisitSVESaturatingIncDecVectorByElementCount},           \
      {"uqdecp_r_p_r_uw", &VISITORCLASS::VisitSVEIncDecByPredicateCount},      \
      {"uqdecp_r_p_r_x", &VISITORCLASS::VisitSVEIncDecByPredicateCount},       \
      {"uqdecp_z_p_z", &VISITORCLASS::VisitSVEIncDecByPredicateCount},         \
      {"uqdecw_r_rs_uw",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqdecw_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqdecw_z_zs",                                                          \
       &VISITORCLASS::VisitSVESaturatingIncDecVectorByElementCount},           \
      {"uqincb_r_rs_uw",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqincb_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqincd_r_rs_uw",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqincd_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqincd_z_zs",                                                          \
       &VISITORCLASS::VisitSVESaturatingIncDecVectorByElementCount},           \
      {"uqinch_r_rs_uw",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqinch_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqinch_z_zs",                                                          \
       &VISITORCLASS::VisitSVESaturatingIncDecVectorByElementCount},           \
      {"uqincp_r_p_r_uw", &VISITORCLASS::VisitSVEIncDecByPredicateCount},      \
      {"uqincp_r_p_r_x", &VISITORCLASS::VisitSVEIncDecByPredicateCount},       \
      {"uqincp_z_p_z", &VISITORCLASS::VisitSVEIncDecByPredicateCount},         \
      {"uqincw_r_rs_uw",                                                       \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqincw_r_rs_x",                                                        \
       &VISITORCLASS::VisitSVESaturatingIncDecRegisterByElementCount},         \
      {"uqincw_z_zs",                                                          \
       &VISITORCLASS::VisitSVESaturatingIncDecVectorByElementCount},           \
      {"uqsub_z_zi", &VISITORCLASS::VisitSVEIntAddSubtractImm_Unpredicated},   \
      {"uqsub_z_zz", &VISITORCLASS::VisitSVEIntArithmeticUnpredicated},        \
      {"uunpkhi_z_z", &VISITORCLASS::VisitSVEUnpackVectorElements},            \
      {"uunpklo_z_z", &VISITORCLASS::VisitSVEUnpackVectorElements},            \
      {"uxtb_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},     \
      {"uxth_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},     \
      {"uxtw_z_p_z", &VISITORCLASS::VisitSVEIntUnaryArithmeticPredicated},     \
      {"uzp1_p_pp", &VISITORCLASS::VisitSVEPermutePredicateElements},          \
      {"uzp1_z_zz", &VISITORCLASS::VisitSVEPermuteVectorInterleaving},         \
      {"uzp2_p_pp", &VISITORCLASS::VisitSVEPermutePredicateElements},          \
      {"uzp2_z_zz", &VISITORCLASS::VisitSVEPermuteVectorInterleaving},         \
      {"whilele_p_p_rr",                                                       \
       &VISITORCLASS::VisitSVEIntCompareScalarCountAndLimit},                  \
      {"whilelo_p_p_rr",                                                       \
       &VISITORCLASS::VisitSVEIntCompareScalarCountAndLimit},                  \
      {"whilels_p_p_rr",                                                       \
       &VISITORCLASS::VisitSVEIntCompareScalarCountAndLimit},                  \
      {"whilelt_p_p_rr",                                                       \
       &VISITORCLASS::VisitSVEIntCompareScalarCountAndLimit},                  \
      {"wrffr_f_p", &VISITORCLASS::VisitSVEFFRWriteFromPredicate},             \
      {"zip1_p_pp", &VISITORCLASS::VisitSVEPermutePredicateElements},          \
      {"zip1_z_zz", &VISITORCLASS::VisitSVEPermuteVectorInterleaving},         \
      {"zip2_p_pp", &VISITORCLASS::VisitSVEPermutePredicateElements},          \
      {"zip2_z_zz", &VISITORCLASS::VisitSVEPermuteVectorInterleaving},         \
      {"adds_32s_addsub_ext", &VISITORCLASS::VisitAddSubExtended},             \
      {"adds_64s_addsub_ext", &VISITORCLASS::VisitAddSubExtended},             \
      {"add_32_addsub_ext", &VISITORCLASS::VisitAddSubExtended},               \
      {"add_64_addsub_ext", &VISITORCLASS::VisitAddSubExtended},               \
      {"subs_32s_addsub_ext", &VISITORCLASS::VisitAddSubExtended},             \
      {"subs_64s_addsub_ext", &VISITORCLASS::VisitAddSubExtended},             \
      {"sub_32_addsub_ext", &VISITORCLASS::VisitAddSubExtended},               \
      {"sub_64_addsub_ext", &VISITORCLASS::VisitAddSubExtended},               \
      {"adds_32s_addsub_imm", &VISITORCLASS::VisitAddSubImmediate},            \
      {"adds_64s_addsub_imm", &VISITORCLASS::VisitAddSubImmediate},            \
      {"add_32_addsub_imm", &VISITORCLASS::VisitAddSubImmediate},              \
      {"add_64_addsub_imm", &VISITORCLASS::VisitAddSubImmediate},              \
      {"subs_32s_addsub_imm", &VISITORCLASS::VisitAddSubImmediate},            \
      {"subs_64s_addsub_imm", &VISITORCLASS::VisitAddSubImmediate},            \
      {"sub_32_addsub_imm", &VISITORCLASS::VisitAddSubImmediate},              \
      {"sub_64_addsub_imm", &VISITORCLASS::VisitAddSubImmediate},              \
      {"adds_32_addsub_shift", &VISITORCLASS::VisitAddSubShifted},             \
      {"adds_64_addsub_shift", &VISITORCLASS::VisitAddSubShifted},             \
      {"add_32_addsub_shift", &VISITORCLASS::VisitAddSubShifted},              \
      {"add_64_addsub_shift", &VISITORCLASS::VisitAddSubShifted},              \
      {"subs_32_addsub_shift", &VISITORCLASS::VisitAddSubShifted},             \
      {"subs_64_addsub_shift", &VISITORCLASS::VisitAddSubShifted},             \
      {"sub_32_addsub_shift", &VISITORCLASS::VisitAddSubShifted},              \
      {"sub_64_addsub_shift", &VISITORCLASS::VisitAddSubShifted},              \
      {"adcs_32_addsub_carry", &VISITORCLASS::VisitAddSubWithCarry},           \
      {"adcs_64_addsub_carry", &VISITORCLASS::VisitAddSubWithCarry},           \
      {"adc_32_addsub_carry", &VISITORCLASS::VisitAddSubWithCarry},            \
      {"adc_64_addsub_carry", &VISITORCLASS::VisitAddSubWithCarry},            \
      {"sbcs_32_addsub_carry", &VISITORCLASS::VisitAddSubWithCarry},           \
      {"sbcs_64_addsub_carry", &VISITORCLASS::VisitAddSubWithCarry},           \
      {"sbc_32_addsub_carry", &VISITORCLASS::VisitAddSubWithCarry},            \
      {"sbc_64_addsub_carry", &VISITORCLASS::VisitAddSubWithCarry},            \
      {"ldaddab_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldaddah_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldaddalb_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldaddalh_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldaddal_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldaddal_64_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldadda_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldadda_64_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldaddb_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldaddh_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldaddlb_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldaddlh_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldaddl_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldaddl_64_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldadd_32_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"ldadd_64_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"ldaprb_32l_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldaprh_32l_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldapr_32l_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldapr_64l_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldclrab_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldclrah_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldclralb_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldclralh_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldclral_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldclral_64_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldclra_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldclra_64_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldclrb_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldclrh_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldclrlb_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldclrlh_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldclrl_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldclrl_64_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldclr_32_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"ldclr_64_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"ldeorab_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldeorah_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldeoralb_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldeoralh_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldeoral_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldeoral_64_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldeora_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldeora_64_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldeorb_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldeorh_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldeorlb_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldeorlh_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldeorl_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldeorl_64_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldeor_32_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"ldeor_64_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"ldsetab_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsetah_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsetalb_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsetalh_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsetal_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsetal_64_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldseta_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldseta_64_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldsetb_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldseth_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldsetlb_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsetlh_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsetl_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldsetl_64_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldset_32_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"ldset_64_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"ldsmaxab_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsmaxah_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsmaxalb_32_memop", &VISITORCLASS::VisitAtomicMemory},                \
      {"ldsmaxalh_32_memop", &VISITORCLASS::VisitAtomicMemory},                \
      {"ldsmaxal_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsmaxal_64_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsmaxa_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsmaxa_64_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsmaxb_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsmaxh_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsmaxlb_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsmaxlh_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsmaxl_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsmaxl_64_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsmax_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldsmax_64_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldsminab_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsminah_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsminalb_32_memop", &VISITORCLASS::VisitAtomicMemory},                \
      {"ldsminalh_32_memop", &VISITORCLASS::VisitAtomicMemory},                \
      {"ldsminal_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsminal_64_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsmina_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsmina_64_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsminb_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsminh_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsminlb_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsminlh_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldsminl_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsminl_64_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldsmin_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldsmin_64_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldumaxab_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldumaxah_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldumaxalb_32_memop", &VISITORCLASS::VisitAtomicMemory},                \
      {"ldumaxalh_32_memop", &VISITORCLASS::VisitAtomicMemory},                \
      {"ldumaxal_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldumaxal_64_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldumaxa_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldumaxa_64_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldumaxb_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldumaxh_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldumaxlb_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldumaxlh_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldumaxl_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldumaxl_64_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldumax_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldumax_64_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"lduminab_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"lduminah_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"lduminalb_32_memop", &VISITORCLASS::VisitAtomicMemory},                \
      {"lduminalh_32_memop", &VISITORCLASS::VisitAtomicMemory},                \
      {"lduminal_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"lduminal_64_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"ldumina_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldumina_64_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"lduminb_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"lduminh_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"lduminlb_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"lduminlh_32_memop", &VISITORCLASS::VisitAtomicMemory},                 \
      {"lduminl_32_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"lduminl_64_memop", &VISITORCLASS::VisitAtomicMemory},                  \
      {"ldumin_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"ldumin_64_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"swpab_32_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"swpah_32_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"swpalb_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"swpalh_32_memop", &VISITORCLASS::VisitAtomicMemory},                   \
      {"swpal_32_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"swpal_64_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"swpa_32_memop", &VISITORCLASS::VisitAtomicMemory},                     \
      {"swpa_64_memop", &VISITORCLASS::VisitAtomicMemory},                     \
      {"swpb_32_memop", &VISITORCLASS::VisitAtomicMemory},                     \
      {"swph_32_memop", &VISITORCLASS::VisitAtomicMemory},                     \
      {"swplb_32_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"swplh_32_memop", &VISITORCLASS::VisitAtomicMemory},                    \
      {"swpl_32_memop", &VISITORCLASS::VisitAtomicMemory},                     \
      {"swpl_64_memop", &VISITORCLASS::VisitAtomicMemory},                     \
      {"swp_32_memop", &VISITORCLASS::VisitAtomicMemory},                      \
      {"swp_64_memop", &VISITORCLASS::VisitAtomicMemory},                      \
      {"bfm_32m_bitfield", &VISITORCLASS::VisitBitfield},                      \
      {"bfm_64m_bitfield", &VISITORCLASS::VisitBitfield},                      \
      {"sbfm_32m_bitfield", &VISITORCLASS::VisitBitfield},                     \
      {"sbfm_64m_bitfield", &VISITORCLASS::VisitBitfield},                     \
      {"ubfm_32m_bitfield", &VISITORCLASS::VisitBitfield},                     \
      {"ubfm_64m_bitfield", &VISITORCLASS::VisitBitfield},                     \
      {"cbnz_32_compbranch", &VISITORCLASS::VisitCompareBranch},               \
      {"cbnz_64_compbranch", &VISITORCLASS::VisitCompareBranch},               \
      {"cbz_32_compbranch", &VISITORCLASS::VisitCompareBranch},                \
      {"cbz_64_compbranch", &VISITORCLASS::VisitCompareBranch},                \
      {"b_only_condbranch", &VISITORCLASS::VisitConditionalBranch},            \
      {"ccmn_32_condcmp_imm",                                                  \
       &VISITORCLASS::VisitConditionalCompareImmediate},                       \
      {"ccmn_64_condcmp_imm",                                                  \
       &VISITORCLASS::VisitConditionalCompareImmediate},                       \
      {"ccmp_32_condcmp_imm",                                                  \
       &VISITORCLASS::VisitConditionalCompareImmediate},                       \
      {"ccmp_64_condcmp_imm",                                                  \
       &VISITORCLASS::VisitConditionalCompareImmediate},                       \
      {"ccmn_32_condcmp_reg", &VISITORCLASS::VisitConditionalCompareRegister}, \
      {"ccmn_64_condcmp_reg", &VISITORCLASS::VisitConditionalCompareRegister}, \
      {"ccmp_32_condcmp_reg", &VISITORCLASS::VisitConditionalCompareRegister}, \
      {"ccmp_64_condcmp_reg", &VISITORCLASS::VisitConditionalCompareRegister}, \
      {"csel_32_condsel", &VISITORCLASS::VisitConditionalSelect},              \
      {"csel_64_condsel", &VISITORCLASS::VisitConditionalSelect},              \
      {"csinc_32_condsel", &VISITORCLASS::VisitConditionalSelect},             \
      {"csinc_64_condsel", &VISITORCLASS::VisitConditionalSelect},             \
      {"csinv_32_condsel", &VISITORCLASS::VisitConditionalSelect},             \
      {"csinv_64_condsel", &VISITORCLASS::VisitConditionalSelect},             \
      {"csneg_32_condsel", &VISITORCLASS::VisitConditionalSelect},             \
      {"csneg_64_condsel", &VISITORCLASS::VisitConditionalSelect},             \
      {"sha1h_ss_cryptosha2", &VISITORCLASS::VisitCrypto2RegSHA},              \
      {"sha1su1_vv_cryptosha2", &VISITORCLASS::VisitCrypto2RegSHA},            \
      {"sha256su0_vv_cryptosha2", &VISITORCLASS::VisitCrypto2RegSHA},          \
      {"sha1c_qsv_cryptosha3", &VISITORCLASS::VisitCrypto3RegSHA},             \
      {"sha1m_qsv_cryptosha3", &VISITORCLASS::VisitCrypto3RegSHA},             \
      {"sha1p_qsv_cryptosha3", &VISITORCLASS::VisitCrypto3RegSHA},             \
      {"sha1su0_vvv_cryptosha3", &VISITORCLASS::VisitCrypto3RegSHA},           \
      {"sha256h2_qqv_cryptosha3", &VISITORCLASS::VisitCrypto3RegSHA},          \
      {"sha256h_qqv_cryptosha3", &VISITORCLASS::VisitCrypto3RegSHA},           \
      {"sha256su1_vvv_cryptosha3", &VISITORCLASS::VisitCrypto3RegSHA},         \
      {"aesd_b_cryptoaes", &VISITORCLASS::VisitCryptoAES},                     \
      {"aese_b_cryptoaes", &VISITORCLASS::VisitCryptoAES},                     \
      {"aesimc_b_cryptoaes", &VISITORCLASS::VisitCryptoAES},                   \
      {"aesmc_b_cryptoaes", &VISITORCLASS::VisitCryptoAES},                    \
      {"autda_64p_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},        \
      {"autdb_64p_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},        \
      {"autdza_64z_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},       \
      {"autdzb_64z_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},       \
      {"autia_64p_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},        \
      {"autib_64p_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},        \
      {"autiza_64z_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},       \
      {"autizb_64z_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},       \
      {"cls_32_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},           \
      {"cls_64_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},           \
      {"clz_32_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},           \
      {"clz_64_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},           \
      {"pacda_64p_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},        \
      {"pacdb_64p_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},        \
      {"pacdza_64z_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},       \
      {"pacdzb_64z_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},       \
      {"pacia_64p_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},        \
      {"pacib_64p_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},        \
      {"paciza_64z_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},       \
      {"pacizb_64z_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},       \
      {"rbit_32_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},          \
      {"rbit_64_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},          \
      {"rev16_32_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},         \
      {"rev16_64_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},         \
      {"rev32_64_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},         \
      {"rev_32_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},           \
      {"rev_64_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},           \
      {"xpacd_64z_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},        \
      {"xpaci_64z_dp_1src", &VISITORCLASS::VisitDataProcessing1Source},        \
      {"asrv_32_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"asrv_64_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"crc32b_32c_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},       \
      {"crc32cb_32c_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},      \
      {"crc32ch_32c_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},      \
      {"crc32cw_32c_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},      \
      {"crc32cx_64c_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},      \
      {"crc32h_32c_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},       \
      {"crc32w_32c_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},       \
      {"crc32x_64c_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},       \
      {"gmi_64g_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"irg_64i_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"lslv_32_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"lslv_64_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"lsrv_32_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"lsrv_64_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"pacga_64p_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},        \
      {"rorv_32_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"rorv_64_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"sdiv_32_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"sdiv_64_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"udiv_32_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"udiv_64_dp_2src", &VISITORCLASS::VisitDataProcessing2Source},          \
      {"madd_32a_dp_3src", &VISITORCLASS::VisitDataProcessing3Source},         \
      {"madd_64a_dp_3src", &VISITORCLASS::VisitDataProcessing3Source},         \
      {"msub_32a_dp_3src", &VISITORCLASS::VisitDataProcessing3Source},         \
      {"msub_64a_dp_3src", &VISITORCLASS::VisitDataProcessing3Source},         \
      {"smaddl_64wa_dp_3src", &VISITORCLASS::VisitDataProcessing3Source},      \
      {"smsubl_64wa_dp_3src", &VISITORCLASS::VisitDataProcessing3Source},      \
      {"smulh_64_dp_3src", &VISITORCLASS::VisitDataProcessing3Source},         \
      {"umaddl_64wa_dp_3src", &VISITORCLASS::VisitDataProcessing3Source},      \
      {"umsubl_64wa_dp_3src", &VISITORCLASS::VisitDataProcessing3Source},      \
      {"umulh_64_dp_3src", &VISITORCLASS::VisitDataProcessing3Source},         \
      {"setf16_only_setf", &VISITORCLASS::VisitEvaluateIntoFlags},             \
      {"setf8_only_setf", &VISITORCLASS::VisitEvaluateIntoFlags},              \
      {"brk_ex_exception", &VISITORCLASS::VisitException},                     \
      {"dcps1_dc_exception", &VISITORCLASS::VisitException},                   \
      {"dcps2_dc_exception", &VISITORCLASS::VisitException},                   \
      {"dcps3_dc_exception", &VISITORCLASS::VisitException},                   \
      {"hlt_ex_exception", &VISITORCLASS::VisitException},                     \
      {"hvc_ex_exception", &VISITORCLASS::VisitException},                     \
      {"smc_ex_exception", &VISITORCLASS::VisitException},                     \
      {"svc_ex_exception", &VISITORCLASS::VisitException},                     \
      {"extr_32_extract", &VISITORCLASS::VisitExtract},                        \
      {"extr_64_extract", &VISITORCLASS::VisitExtract},                        \
      {"fcmpe_dz_floatcmp", &VISITORCLASS::VisitFPCompare},                    \
      {"fcmpe_d_floatcmp", &VISITORCLASS::VisitFPCompare},                     \
      {"fcmpe_hz_floatcmp", &VISITORCLASS::VisitFPCompare},                    \
      {"fcmpe_h_floatcmp", &VISITORCLASS::VisitFPCompare},                     \
      {"fcmpe_sz_floatcmp", &VISITORCLASS::VisitFPCompare},                    \
      {"fcmpe_s_floatcmp", &VISITORCLASS::VisitFPCompare},                     \
      {"fcmp_dz_floatcmp", &VISITORCLASS::VisitFPCompare},                     \
      {"fcmp_d_floatcmp", &VISITORCLASS::VisitFPCompare},                      \
      {"fcmp_hz_floatcmp", &VISITORCLASS::VisitFPCompare},                     \
      {"fcmp_h_floatcmp", &VISITORCLASS::VisitFPCompare},                      \
      {"fcmp_sz_floatcmp", &VISITORCLASS::VisitFPCompare},                     \
      {"fcmp_s_floatcmp", &VISITORCLASS::VisitFPCompare},                      \
      {"fccmpe_d_floatccmp", &VISITORCLASS::VisitFPConditionalCompare},        \
      {"fccmpe_h_floatccmp", &VISITORCLASS::VisitFPConditionalCompare},        \
      {"fccmpe_s_floatccmp", &VISITORCLASS::VisitFPConditionalCompare},        \
      {"fccmp_d_floatccmp", &VISITORCLASS::VisitFPConditionalCompare},         \
      {"fccmp_h_floatccmp", &VISITORCLASS::VisitFPConditionalCompare},         \
      {"fccmp_s_floatccmp", &VISITORCLASS::VisitFPConditionalCompare},         \
      {"fcsel_d_floatsel", &VISITORCLASS::VisitFPConditionalSelect},           \
      {"fcsel_h_floatsel", &VISITORCLASS::VisitFPConditionalSelect},           \
      {"fcsel_s_floatsel", &VISITORCLASS::VisitFPConditionalSelect},           \
      {"bfcvt_bs_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"fabs_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},        \
      {"fabs_h_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},        \
      {"fabs_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},        \
      {"fcvt_dh_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},       \
      {"fcvt_ds_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},       \
      {"fcvt_hd_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},       \
      {"fcvt_hs_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},       \
      {"fcvt_sd_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},       \
      {"fcvt_sh_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},       \
      {"fmov_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},        \
      {"fmov_h_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},        \
      {"fmov_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},        \
      {"fneg_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},        \
      {"fneg_h_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},        \
      {"fneg_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},        \
      {"frint32x_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},    \
      {"frint32x_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},    \
      {"frint32z_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},    \
      {"frint32z_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},    \
      {"frint64x_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},    \
      {"frint64x_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},    \
      {"frint64z_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},    \
      {"frint64z_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},    \
      {"frinta_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frinta_h_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frinta_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frinti_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frinti_h_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frinti_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintm_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintm_h_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintm_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintn_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintn_h_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintn_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintp_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintp_h_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintp_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintx_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintx_h_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintx_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintz_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintz_h_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"frintz_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},      \
      {"fsqrt_d_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},       \
      {"fsqrt_h_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},       \
      {"fsqrt_s_floatdp1", &VISITORCLASS::VisitFPDataProcessing1Source},       \
      {"fadd_d_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fadd_h_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fadd_s_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fdiv_d_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fdiv_h_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fdiv_s_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fmaxnm_d_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},      \
      {"fmaxnm_h_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},      \
      {"fmaxnm_s_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},      \
      {"fmax_d_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fmax_h_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fmax_s_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fminnm_d_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},      \
      {"fminnm_h_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},      \
      {"fminnm_s_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},      \
      {"fmin_d_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fmin_h_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fmin_s_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fmul_d_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fmul_h_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fmul_s_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fnmul_d_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},       \
      {"fnmul_h_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},       \
      {"fnmul_s_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},       \
      {"fsub_d_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fsub_h_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fsub_s_floatdp2", &VISITORCLASS::VisitFPDataProcessing2Source},        \
      {"fmadd_d_floatdp3", &VISITORCLASS::VisitFPDataProcessing3Source},       \
      {"fmadd_h_floatdp3", &VISITORCLASS::VisitFPDataProcessing3Source},       \
      {"fmadd_s_floatdp3", &VISITORCLASS::VisitFPDataProcessing3Source},       \
      {"fmsub_d_floatdp3", &VISITORCLASS::VisitFPDataProcessing3Source},       \
      {"fmsub_h_floatdp3", &VISITORCLASS::VisitFPDataProcessing3Source},       \
      {"fmsub_s_floatdp3", &VISITORCLASS::VisitFPDataProcessing3Source},       \
      {"fnmadd_d_floatdp3", &VISITORCLASS::VisitFPDataProcessing3Source},      \
      {"fnmadd_h_floatdp3", &VISITORCLASS::VisitFPDataProcessing3Source},      \
      {"fnmadd_s_floatdp3", &VISITORCLASS::VisitFPDataProcessing3Source},      \
      {"fnmsub_d_floatdp3", &VISITORCLASS::VisitFPDataProcessing3Source},      \
      {"fnmsub_h_floatdp3", &VISITORCLASS::VisitFPDataProcessing3Source},      \
      {"fnmsub_s_floatdp3", &VISITORCLASS::VisitFPDataProcessing3Source},      \
      {"fcvtzs_32d_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},       \
      {"fcvtzs_32h_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},       \
      {"fcvtzs_32s_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},       \
      {"fcvtzs_64d_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},       \
      {"fcvtzs_64h_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},       \
      {"fcvtzs_64s_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},       \
      {"fcvtzu_32d_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},       \
      {"fcvtzu_32h_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},       \
      {"fcvtzu_32s_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},       \
      {"fcvtzu_64d_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},       \
      {"fcvtzu_64h_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},       \
      {"fcvtzu_64s_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},       \
      {"scvtf_d32_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},        \
      {"scvtf_d64_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},        \
      {"scvtf_h32_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},        \
      {"scvtf_h64_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},        \
      {"scvtf_s32_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},        \
      {"scvtf_s64_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},        \
      {"ucvtf_d32_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},        \
      {"ucvtf_d64_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},        \
      {"ucvtf_h32_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},        \
      {"ucvtf_h64_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},        \
      {"ucvtf_s32_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},        \
      {"ucvtf_s64_float2fix", &VISITORCLASS::VisitFPFixedPointConvert},        \
      {"fmov_d_floatimm", &VISITORCLASS::VisitFPImmediate},                    \
      {"fmov_h_floatimm", &VISITORCLASS::VisitFPImmediate},                    \
      {"fmov_s_floatimm", &VISITORCLASS::VisitFPImmediate},                    \
      {"fcvtas_32d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtas_32h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtas_32s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtas_64d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtas_64h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtas_64s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtau_32d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtau_32h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtau_32s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtau_64d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtau_64h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtau_64s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtms_32d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtms_32h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtms_32s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtms_64d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtms_64h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtms_64s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtmu_32d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtmu_32h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtmu_32s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtmu_64d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtmu_64h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtmu_64s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtns_32d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtns_32h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtns_32s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtns_64d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtns_64h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtns_64s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtnu_32d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtnu_32h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtnu_32s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtnu_64d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtnu_64h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtnu_64s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtps_32d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtps_32h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtps_32s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtps_64d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtps_64h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtps_64s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtpu_32d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtpu_32h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtpu_32s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtpu_64d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtpu_64h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtpu_64s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtzs_32d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtzs_32h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtzs_32s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtzs_64d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtzs_64h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtzs_64s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtzu_32d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtzu_32h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtzu_32s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtzu_64d_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtzu_64h_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fcvtzu_64s_float2int", &VISITORCLASS::VisitFPIntegerConvert},          \
      {"fjcvtzs_32d_float2int", &VISITORCLASS::VisitFPIntegerConvert},         \
      {"fmov_32h_float2int", &VISITORCLASS::VisitFPIntegerConvert},            \
      {"fmov_32s_float2int", &VISITORCLASS::VisitFPIntegerConvert},            \
      {"fmov_64d_float2int", &VISITORCLASS::VisitFPIntegerConvert},            \
      {"fmov_64h_float2int", &VISITORCLASS::VisitFPIntegerConvert},            \
      {"fmov_64vx_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"fmov_d64_float2int", &VISITORCLASS::VisitFPIntegerConvert},            \
      {"fmov_h32_float2int", &VISITORCLASS::VisitFPIntegerConvert},            \
      {"fmov_h64_float2int", &VISITORCLASS::VisitFPIntegerConvert},            \
      {"fmov_s32_float2int", &VISITORCLASS::VisitFPIntegerConvert},            \
      {"fmov_v64i_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"scvtf_d32_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"scvtf_d64_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"scvtf_h32_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"scvtf_h64_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"scvtf_s32_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"scvtf_s64_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"ucvtf_d32_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"ucvtf_d64_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"ucvtf_h32_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"ucvtf_h64_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"ucvtf_s32_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"ucvtf_s64_float2int", &VISITORCLASS::VisitFPIntegerConvert},           \
      {"ldrsw_64_loadlit", &VISITORCLASS::VisitLoadLiteral},                   \
      {"ldr_32_loadlit", &VISITORCLASS::VisitLoadLiteral},                     \
      {"ldr_64_loadlit", &VISITORCLASS::VisitLoadLiteral},                     \
      {"ldr_d_loadlit", &VISITORCLASS::VisitLoadLiteral},                      \
      {"ldr_q_loadlit", &VISITORCLASS::VisitLoadLiteral},                      \
      {"ldr_s_loadlit", &VISITORCLASS::VisitLoadLiteral},                      \
      {"prfm_p_loadlit", &VISITORCLASS::VisitLoadLiteral},                     \
      {"casab_c32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"casah_c32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"casalb_c32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"casalh_c32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"casal_c32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"casal_c64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"casa_c32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},           \
      {"casa_c64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},           \
      {"casb_c32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},           \
      {"cash_c32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},           \
      {"caslb_c32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"caslh_c32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"casl_c32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},           \
      {"casl_c64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},           \
      {"caspal_cp32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},        \
      {"caspal_cp64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},        \
      {"caspa_cp32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"caspa_cp64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"caspl_cp32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"caspl_cp64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"casp_cp32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"casp_cp64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"cas_c32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},            \
      {"cas_c64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},            \
      {"ldarb_lr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"ldarh_lr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"ldar_lr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"ldar_lr64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"ldaxp_lp32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"ldaxp_lp64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"ldaxrb_lr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},        \
      {"ldaxrh_lr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},        \
      {"ldaxr_lr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"ldaxr_lr64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"ldlarb_lr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},        \
      {"ldlarh_lr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},        \
      {"ldlar_lr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"ldlar_lr64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"ldxp_lp32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"ldxp_lp64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"ldxrb_lr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"ldxrh_lr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"ldxr_lr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"ldxr_lr64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"stllrb_sl32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},        \
      {"stllrh_sl32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},        \
      {"stllr_sl32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"stllr_sl64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"stlrb_sl32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"stlrh_sl32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"stlr_sl32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"stlr_sl64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"stlxp_sp32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"stlxp_sp64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"stlxrb_sr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},        \
      {"stlxrh_sr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},        \
      {"stlxr_sr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"stlxr_sr64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"stxp_sp32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"stxp_sp64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"stxrb_sr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"stxrh_sr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},         \
      {"stxr_sr32_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"stxr_sr64_ldstexcl", &VISITORCLASS::VisitLoadStoreExclusive},          \
      {"ldraa_64w_ldst_pac", &VISITORCLASS::VisitLoadStorePAC},                \
      {"ldraa_64_ldst_pac", &VISITORCLASS::VisitLoadStorePAC},                 \
      {"ldrab_64w_ldst_pac", &VISITORCLASS::VisitLoadStorePAC},                \
      {"ldrab_64_ldst_pac", &VISITORCLASS::VisitLoadStorePAC},                 \
      {"ldnp_32_ldstnapair_offs",                                              \
       &VISITORCLASS::VisitLoadStorePairNonTemporal},                          \
      {"ldnp_64_ldstnapair_offs",                                              \
       &VISITORCLASS::VisitLoadStorePairNonTemporal},                          \
      {"ldnp_d_ldstnapair_offs",                                               \
       &VISITORCLASS::VisitLoadStorePairNonTemporal},                          \
      {"ldnp_q_ldstnapair_offs",                                               \
       &VISITORCLASS::VisitLoadStorePairNonTemporal},                          \
      {"ldnp_s_ldstnapair_offs",                                               \
       &VISITORCLASS::VisitLoadStorePairNonTemporal},                          \
      {"stnp_32_ldstnapair_offs",                                              \
       &VISITORCLASS::VisitLoadStorePairNonTemporal},                          \
      {"stnp_64_ldstnapair_offs",                                              \
       &VISITORCLASS::VisitLoadStorePairNonTemporal},                          \
      {"stnp_d_ldstnapair_offs",                                               \
       &VISITORCLASS::VisitLoadStorePairNonTemporal},                          \
      {"stnp_q_ldstnapair_offs",                                               \
       &VISITORCLASS::VisitLoadStorePairNonTemporal},                          \
      {"stnp_s_ldstnapair_offs",                                               \
       &VISITORCLASS::VisitLoadStorePairNonTemporal},                          \
      {"ldpsw_64_ldstpair_off", &VISITORCLASS::VisitLoadStorePairOffset},      \
      {"ldp_32_ldstpair_off", &VISITORCLASS::VisitLoadStorePairOffset},        \
      {"ldp_64_ldstpair_off", &VISITORCLASS::VisitLoadStorePairOffset},        \
      {"ldp_d_ldstpair_off", &VISITORCLASS::VisitLoadStorePairOffset},         \
      {"ldp_q_ldstpair_off", &VISITORCLASS::VisitLoadStorePairOffset},         \
      {"ldp_s_ldstpair_off", &VISITORCLASS::VisitLoadStorePairOffset},         \
      {"stp_32_ldstpair_off", &VISITORCLASS::VisitLoadStorePairOffset},        \
      {"stp_64_ldstpair_off", &VISITORCLASS::VisitLoadStorePairOffset},        \
      {"stp_d_ldstpair_off", &VISITORCLASS::VisitLoadStorePairOffset},         \
      {"stp_q_ldstpair_off", &VISITORCLASS::VisitLoadStorePairOffset},         \
      {"stp_s_ldstpair_off", &VISITORCLASS::VisitLoadStorePairOffset},         \
      {"ldpsw_64_ldstpair_post", &VISITORCLASS::VisitLoadStorePairPostIndex},  \
      {"ldp_32_ldstpair_post", &VISITORCLASS::VisitLoadStorePairPostIndex},    \
      {"ldp_64_ldstpair_post", &VISITORCLASS::VisitLoadStorePairPostIndex},    \
      {"ldp_d_ldstpair_post", &VISITORCLASS::VisitLoadStorePairPostIndex},     \
      {"ldp_q_ldstpair_post", &VISITORCLASS::VisitLoadStorePairPostIndex},     \
      {"ldp_s_ldstpair_post", &VISITORCLASS::VisitLoadStorePairPostIndex},     \
      {"stp_32_ldstpair_post", &VISITORCLASS::VisitLoadStorePairPostIndex},    \
      {"stp_64_ldstpair_post", &VISITORCLASS::VisitLoadStorePairPostIndex},    \
      {"stp_d_ldstpair_post", &VISITORCLASS::VisitLoadStorePairPostIndex},     \
      {"stp_q_ldstpair_post", &VISITORCLASS::VisitLoadStorePairPostIndex},     \
      {"stp_s_ldstpair_post", &VISITORCLASS::VisitLoadStorePairPostIndex},     \
      {"ldpsw_64_ldstpair_pre", &VISITORCLASS::VisitLoadStorePairPreIndex},    \
      {"ldp_32_ldstpair_pre", &VISITORCLASS::VisitLoadStorePairPreIndex},      \
      {"ldp_64_ldstpair_pre", &VISITORCLASS::VisitLoadStorePairPreIndex},      \
      {"ldp_d_ldstpair_pre", &VISITORCLASS::VisitLoadStorePairPreIndex},       \
      {"ldp_q_ldstpair_pre", &VISITORCLASS::VisitLoadStorePairPreIndex},       \
      {"ldp_s_ldstpair_pre", &VISITORCLASS::VisitLoadStorePairPreIndex},       \
      {"stp_32_ldstpair_pre", &VISITORCLASS::VisitLoadStorePairPreIndex},      \
      {"stp_64_ldstpair_pre", &VISITORCLASS::VisitLoadStorePairPreIndex},      \
      {"stp_d_ldstpair_pre", &VISITORCLASS::VisitLoadStorePairPreIndex},       \
      {"stp_q_ldstpair_pre", &VISITORCLASS::VisitLoadStorePairPreIndex},       \
      {"stp_s_ldstpair_pre", &VISITORCLASS::VisitLoadStorePairPreIndex},       \
      {"ldrb_32_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},        \
      {"ldrh_32_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},        \
      {"ldrsb_32_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},       \
      {"ldrsb_64_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},       \
      {"ldrsh_32_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},       \
      {"ldrsh_64_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},       \
      {"ldrsw_64_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},       \
      {"ldr_32_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},         \
      {"ldr_64_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},         \
      {"ldr_b_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},          \
      {"ldr_d_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},          \
      {"ldr_h_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},          \
      {"ldr_q_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},          \
      {"ldr_s_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},          \
      {"strb_32_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},        \
      {"strh_32_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},        \
      {"str_32_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},         \
      {"str_64_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},         \
      {"str_b_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},          \
      {"str_d_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},          \
      {"str_h_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},          \
      {"str_q_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},          \
      {"str_s_ldst_immpost", &VISITORCLASS::VisitLoadStorePostIndex},          \
      {"ldrb_32_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},          \
      {"ldrh_32_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},          \
      {"ldrsb_32_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},         \
      {"ldrsb_64_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},         \
      {"ldrsh_32_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},         \
      {"ldrsh_64_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},         \
      {"ldrsw_64_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},         \
      {"ldr_32_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},           \
      {"ldr_64_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},           \
      {"ldr_b_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},            \
      {"ldr_d_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},            \
      {"ldr_h_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},            \
      {"ldr_q_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},            \
      {"ldr_s_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},            \
      {"strb_32_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},          \
      {"strh_32_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},          \
      {"str_32_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},           \
      {"str_64_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},           \
      {"str_b_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},            \
      {"str_d_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},            \
      {"str_h_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},            \
      {"str_q_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},            \
      {"str_s_ldst_immpre", &VISITORCLASS::VisitLoadStorePreIndex},            \
      {"ldapurb_32_ldapstl_unscaled",                                          \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"ldapurh_32_ldapstl_unscaled",                                          \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"ldapursb_32_ldapstl_unscaled",                                         \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"ldapursb_64_ldapstl_unscaled",                                         \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"ldapursh_32_ldapstl_unscaled",                                         \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"ldapursh_64_ldapstl_unscaled",                                         \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"ldapursw_64_ldapstl_unscaled",                                         \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"ldapur_32_ldapstl_unscaled",                                           \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"ldapur_64_ldapstl_unscaled",                                           \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"stlurb_32_ldapstl_unscaled",                                           \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"stlurh_32_ldapstl_unscaled",                                           \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"stlur_32_ldapstl_unscaled",                                            \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"stlur_64_ldapstl_unscaled",                                            \
       &VISITORCLASS::VisitLoadStoreRCpcUnscaledOffset},                       \
      {"ldrb_32bl_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},  \
      {"ldrb_32b_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},   \
      {"ldrh_32_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},    \
      {"ldrsb_32bl_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset}, \
      {"ldrsb_32b_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},  \
      {"ldrsb_64bl_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset}, \
      {"ldrsb_64b_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},  \
      {"ldrsh_32_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},   \
      {"ldrsh_64_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},   \
      {"ldrsw_64_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},   \
      {"ldr_32_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},     \
      {"ldr_64_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},     \
      {"ldr_bl_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},     \
      {"ldr_b_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},      \
      {"ldr_d_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},      \
      {"ldr_h_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},      \
      {"ldr_q_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},      \
      {"ldr_s_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},      \
      {"prfm_p_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},     \
      {"strb_32bl_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},  \
      {"strb_32b_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},   \
      {"strh_32_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},    \
      {"str_32_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},     \
      {"str_64_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},     \
      {"str_bl_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},     \
      {"str_b_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},      \
      {"str_d_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},      \
      {"str_h_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},      \
      {"str_q_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},      \
      {"str_s_ldst_regoff", &VISITORCLASS::VisitLoadStoreRegisterOffset},      \
      {"ldurb_32_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset}, \
      {"ldurh_32_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset}, \
      {"ldursb_32_ldst_unscaled",                                              \
       &VISITORCLASS::VisitLoadStoreUnscaledOffset},                           \
      {"ldursb_64_ldst_unscaled",                                              \
       &VISITORCLASS::VisitLoadStoreUnscaledOffset},                           \
      {"ldursh_32_ldst_unscaled",                                              \
       &VISITORCLASS::VisitLoadStoreUnscaledOffset},                           \
      {"ldursh_64_ldst_unscaled",                                              \
       &VISITORCLASS::VisitLoadStoreUnscaledOffset},                           \
      {"ldursw_64_ldst_unscaled",                                              \
       &VISITORCLASS::VisitLoadStoreUnscaledOffset},                           \
      {"ldur_32_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},  \
      {"ldur_64_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},  \
      {"ldur_b_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},   \
      {"ldur_d_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},   \
      {"ldur_h_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},   \
      {"ldur_q_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},   \
      {"ldur_s_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},   \
      {"prfum_p_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},  \
      {"sturb_32_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset}, \
      {"sturh_32_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset}, \
      {"stur_32_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},  \
      {"stur_64_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},  \
      {"stur_b_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},   \
      {"stur_d_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},   \
      {"stur_h_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},   \
      {"stur_q_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},   \
      {"stur_s_ldst_unscaled", &VISITORCLASS::VisitLoadStoreUnscaledOffset},   \
      {"ldrb_32_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},       \
      {"ldrh_32_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},       \
      {"ldrsb_32_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},      \
      {"ldrsb_64_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},      \
      {"ldrsh_32_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},      \
      {"ldrsh_64_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},      \
      {"ldrsw_64_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},      \
      {"ldr_32_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},        \
      {"ldr_64_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},        \
      {"ldr_b_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},         \
      {"ldr_d_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},         \
      {"ldr_h_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},         \
      {"ldr_q_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},         \
      {"ldr_s_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},         \
      {"prfm_p_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},        \
      {"strb_32_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},       \
      {"strh_32_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},       \
      {"str_32_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},        \
      {"str_64_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},        \
      {"str_b_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},         \
      {"str_d_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},         \
      {"str_h_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},         \
      {"str_q_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},         \
      {"str_s_ldst_pos", &VISITORCLASS::VisitLoadStoreUnsignedOffset},         \
      {"ands_32s_log_imm", &VISITORCLASS::VisitLogicalImmediate},              \
      {"ands_64s_log_imm", &VISITORCLASS::VisitLogicalImmediate},              \
      {"and_32_log_imm", &VISITORCLASS::VisitLogicalImmediate},                \
      {"and_64_log_imm", &VISITORCLASS::VisitLogicalImmediate},                \
      {"eor_32_log_imm", &VISITORCLASS::VisitLogicalImmediate},                \
      {"eor_64_log_imm", &VISITORCLASS::VisitLogicalImmediate},                \
      {"orr_32_log_imm", &VISITORCLASS::VisitLogicalImmediate},                \
      {"orr_64_log_imm", &VISITORCLASS::VisitLogicalImmediate},                \
      {"ands_32_log_shift", &VISITORCLASS::VisitLogicalShifted},               \
      {"ands_64_log_shift", &VISITORCLASS::VisitLogicalShifted},               \
      {"and_32_log_shift", &VISITORCLASS::VisitLogicalShifted},                \
      {"and_64_log_shift", &VISITORCLASS::VisitLogicalShifted},                \
      {"bics_32_log_shift", &VISITORCLASS::VisitLogicalShifted},               \
      {"bics_64_log_shift", &VISITORCLASS::VisitLogicalShifted},               \
      {"bic_32_log_shift", &VISITORCLASS::VisitLogicalShifted},                \
      {"bic_64_log_shift", &VISITORCLASS::VisitLogicalShifted},                \
      {"eon_32_log_shift", &VISITORCLASS::VisitLogicalShifted},                \
      {"eon_64_log_shift", &VISITORCLASS::VisitLogicalShifted},                \
      {"eor_32_log_shift", &VISITORCLASS::VisitLogicalShifted},                \
      {"eor_64_log_shift", &VISITORCLASS::VisitLogicalShifted},                \
      {"orn_32_log_shift", &VISITORCLASS::VisitLogicalShifted},                \
      {"orn_64_log_shift", &VISITORCLASS::VisitLogicalShifted},                \
      {"orr_32_log_shift", &VISITORCLASS::VisitLogicalShifted},                \
      {"orr_64_log_shift", &VISITORCLASS::VisitLogicalShifted},                \
      {"movk_32_movewide", &VISITORCLASS::VisitMoveWideImmediate},             \
      {"movk_64_movewide", &VISITORCLASS::VisitMoveWideImmediate},             \
      {"movn_32_movewide", &VISITORCLASS::VisitMoveWideImmediate},             \
      {"movn_64_movewide", &VISITORCLASS::VisitMoveWideImmediate},             \
      {"movz_32_movewide", &VISITORCLASS::VisitMoveWideImmediate},             \
      {"movz_64_movewide", &VISITORCLASS::VisitMoveWideImmediate},             \
      {"fabs_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},          \
      {"fcmeq_asimdmiscfp16_fz", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcmge_asimdmiscfp16_fz", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcmgt_asimdmiscfp16_fz", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcmle_asimdmiscfp16_fz", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcmlt_asimdmiscfp16_fz", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcvtas_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcvtau_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcvtms_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcvtmu_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcvtns_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcvtnu_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcvtps_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcvtpu_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcvtzs_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fcvtzu_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"fneg_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},          \
      {"frecpe_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"frinta_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"frinti_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"frintm_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"frintn_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"frintp_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"frintx_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"frintz_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},        \
      {"frsqrte_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},       \
      {"fsqrt_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},         \
      {"scvtf_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},         \
      {"ucvtf_asimdmiscfp16_r", &VISITORCLASS::VisitNEON2RegMiscFP16},         \
      {"addhn_asimddiff_n", &VISITORCLASS::VisitNEON3Different},               \
      {"pmull_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"raddhn_asimddiff_n", &VISITORCLASS::VisitNEON3Different},              \
      {"rsubhn_asimddiff_n", &VISITORCLASS::VisitNEON3Different},              \
      {"sabal_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"sabdl_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"saddl_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"saddw_asimddiff_w", &VISITORCLASS::VisitNEON3Different},               \
      {"smlal_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"smlsl_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"smull_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"sqdmlal_asimddiff_l", &VISITORCLASS::VisitNEON3Different},             \
      {"sqdmlsl_asimddiff_l", &VISITORCLASS::VisitNEON3Different},             \
      {"sqdmull_asimddiff_l", &VISITORCLASS::VisitNEON3Different},             \
      {"ssubl_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"ssubw_asimddiff_w", &VISITORCLASS::VisitNEON3Different},               \
      {"subhn_asimddiff_n", &VISITORCLASS::VisitNEON3Different},               \
      {"uabal_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"uabdl_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"uaddl_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"uaddw_asimddiff_w", &VISITORCLASS::VisitNEON3Different},               \
      {"umlal_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"umlsl_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"umull_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"usubl_asimddiff_l", &VISITORCLASS::VisitNEON3Different},               \
      {"usubw_asimddiff_w", &VISITORCLASS::VisitNEON3Different},               \
      {"addp_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"add_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"cmeq_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"cmge_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"cmgt_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"cmhi_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"cmhs_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"cmtst_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"fabd_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"facge_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"facgt_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"faddp_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"fadd_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"fcmeq_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"fcmge_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"fcmgt_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"fdiv_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"fmaxnmp_asimdsame_only", &VISITORCLASS::VisitNEON3Same},               \
      {"fmaxnm_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                \
      {"fmaxp_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"fmax_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"fminnmp_asimdsame_only", &VISITORCLASS::VisitNEON3Same},               \
      {"fminnm_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                \
      {"fminp_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"fmin_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"fmla_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"fmls_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"fmulx_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"fmul_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"frecps_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                \
      {"frsqrts_asimdsame_only", &VISITORCLASS::VisitNEON3Same},               \
      {"fsub_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"sqadd_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"sqdmulh_asimdsame_only", &VISITORCLASS::VisitNEON3Same},               \
      {"sqrdmulh_asimdsame_only", &VISITORCLASS::VisitNEON3Same},              \
      {"sqrshl_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                \
      {"sqshl_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"sqsub_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"srshl_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"sshl_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"sub_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"uqadd_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"uqrshl_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                \
      {"uqshl_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"uqsub_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"urshl_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"ushl_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"fcadd_asimdsame2_c", &VISITORCLASS::VisitNEON3SameExtra},              \
      {"fcmla_asimdsame2_c", &VISITORCLASS::VisitNEON3SameExtra},              \
      {"sdot_asimdsame2_d", &VISITORCLASS::VisitNEON3SameExtra},               \
      {"sqrdmlah_asimdsame2_only", &VISITORCLASS::VisitNEON3SameExtra},        \
      {"sqrdmlsh_asimdsame2_only", &VISITORCLASS::VisitNEON3SameExtra},        \
      {"udot_asimdsame2_d", &VISITORCLASS::VisitNEON3SameExtra},               \
      {"fabd_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},          \
      {"facge_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},         \
      {"facgt_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},         \
      {"faddp_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},         \
      {"fadd_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},          \
      {"fcmeq_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},         \
      {"fcmge_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},         \
      {"fcmgt_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},         \
      {"fdiv_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},          \
      {"fmaxnmp_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},       \
      {"fmaxnm_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},        \
      {"fmaxp_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},         \
      {"fmax_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},          \
      {"fminnmp_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},       \
      {"fminnm_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},        \
      {"fminp_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},         \
      {"fmin_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},          \
      {"fmla_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},          \
      {"fmls_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},          \
      {"fmulx_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},         \
      {"fmul_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},          \
      {"frecps_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},        \
      {"frsqrts_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},       \
      {"fsub_asimdsamefp16_only", &VISITORCLASS::VisitNEON3SameFP16},          \
      {"addv_asimdall_only", &VISITORCLASS::VisitNEONAcrossLanes},             \
      {"saddlv_asimdall_only", &VISITORCLASS::VisitNEONAcrossLanes},           \
      {"smaxv_asimdall_only", &VISITORCLASS::VisitNEONAcrossLanes},            \
      {"sminv_asimdall_only", &VISITORCLASS::VisitNEONAcrossLanes},            \
      {"uaddlv_asimdall_only", &VISITORCLASS::VisitNEONAcrossLanes},           \
      {"umaxv_asimdall_only", &VISITORCLASS::VisitNEONAcrossLanes},            \
      {"uminv_asimdall_only", &VISITORCLASS::VisitNEONAcrossLanes},            \
      {"mla_asimdelem_r", &VISITORCLASS::VisitNEONByIndexedElement},           \
      {"mls_asimdelem_r", &VISITORCLASS::VisitNEONByIndexedElement},           \
      {"mul_asimdelem_r", &VISITORCLASS::VisitNEONByIndexedElement},           \
      {"sqdmulh_asimdelem_r", &VISITORCLASS::VisitNEONByIndexedElement},       \
      {"sqrdmlah_asimdelem_r", &VISITORCLASS::VisitNEONByIndexedElement},      \
      {"sqrdmlsh_asimdelem_r", &VISITORCLASS::VisitNEONByIndexedElement},      \
      {"sqrdmulh_asimdelem_r", &VISITORCLASS::VisitNEONByIndexedElement},      \
      {"dup_asimdins_dr_r", &VISITORCLASS::VisitNEONCopy},                     \
      {"dup_asimdins_dv_v", &VISITORCLASS::VisitNEONCopy},                     \
      {"ins_asimdins_ir_r", &VISITORCLASS::VisitNEONCopy},                     \
      {"ins_asimdins_iv_v", &VISITORCLASS::VisitNEONCopy},                     \
      {"smov_asimdins_w_w", &VISITORCLASS::VisitNEONCopy},                     \
      {"smov_asimdins_x_x", &VISITORCLASS::VisitNEONCopy},                     \
      {"umov_asimdins_w_w", &VISITORCLASS::VisitNEONCopy},                     \
      {"umov_asimdins_x_x", &VISITORCLASS::VisitNEONCopy},                     \
      {"ext_asimdext_only", &VISITORCLASS::VisitNEONExtract},                  \
      {"ld1_asisdlse_r1_1v", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},    \
      {"ld1_asisdlse_r2_2v", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},    \
      {"ld1_asisdlse_r3_3v", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},    \
      {"ld1_asisdlse_r4_4v", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},    \
      {"ld2_asisdlse_r2", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},       \
      {"ld3_asisdlse_r3", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},       \
      {"ld4_asisdlse_r4", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},       \
      {"st1_asisdlse_r1_1v", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},    \
      {"st1_asisdlse_r2_2v", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},    \
      {"st1_asisdlse_r3_3v", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},    \
      {"st1_asisdlse_r4_4v", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},    \
      {"st2_asisdlse_r2", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},       \
      {"st3_asisdlse_r3", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},       \
      {"st4_asisdlse_r4", &VISITORCLASS::VisitNEONLoadStoreMultiStruct},       \
      {"ld1_asisdlsep_i1_i1",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld1_asisdlsep_i2_i2",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld1_asisdlsep_i3_i3",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld1_asisdlsep_i4_i4",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld1_asisdlsep_r1_r1",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld1_asisdlsep_r2_r2",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld1_asisdlsep_r3_r3",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld1_asisdlsep_r4_r4",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld2_asisdlsep_i2_i",                                                   \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld2_asisdlsep_r2_r",                                                   \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld3_asisdlsep_i3_i",                                                   \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld3_asisdlsep_r3_r",                                                   \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld4_asisdlsep_i4_i",                                                   \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld4_asisdlsep_r4_r",                                                   \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st1_asisdlsep_i1_i1",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st1_asisdlsep_i2_i2",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st1_asisdlsep_i3_i3",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st1_asisdlsep_i4_i4",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st1_asisdlsep_r1_r1",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st1_asisdlsep_r2_r2",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st1_asisdlsep_r3_r3",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st1_asisdlsep_r4_r4",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st2_asisdlsep_i2_i",                                                   \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st2_asisdlsep_r2_r",                                                   \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st3_asisdlsep_i3_i",                                                   \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st3_asisdlsep_r3_r",                                                   \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st4_asisdlsep_i4_i",                                                   \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"st4_asisdlsep_r4_r",                                                   \
       &VISITORCLASS::VisitNEONLoadStoreMultiStructPostIndex},                 \
      {"ld1r_asisdlso_r1", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},     \
      {"ld1_asisdlso_b1_1b", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld1_asisdlso_d1_1d", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld1_asisdlso_h1_1h", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld1_asisdlso_s1_1s", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld2r_asisdlso_r2", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},     \
      {"ld2_asisdlso_b2_2b", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld2_asisdlso_d2_2d", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld2_asisdlso_h2_2h", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld2_asisdlso_s2_2s", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld3r_asisdlso_r3", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},     \
      {"ld3_asisdlso_b3_3b", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld3_asisdlso_d3_3d", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld3_asisdlso_h3_3h", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld3_asisdlso_s3_3s", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld4r_asisdlso_r4", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},     \
      {"ld4_asisdlso_b4_4b", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld4_asisdlso_d4_4d", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld4_asisdlso_h4_4h", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld4_asisdlso_s4_4s", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st1_asisdlso_b1_1b", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st1_asisdlso_d1_1d", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st1_asisdlso_h1_1h", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st1_asisdlso_s1_1s", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st2_asisdlso_b2_2b", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st2_asisdlso_d2_2d", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st2_asisdlso_h2_2h", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st2_asisdlso_s2_2s", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st3_asisdlso_b3_3b", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st3_asisdlso_d3_3d", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st3_asisdlso_h3_3h", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st3_asisdlso_s3_3s", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st4_asisdlso_b4_4b", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st4_asisdlso_d4_4d", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st4_asisdlso_h4_4h", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"st4_asisdlso_s4_4s", &VISITORCLASS::VisitNEONLoadStoreSingleStruct},   \
      {"ld1r_asisdlsop_r1_i",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld1r_asisdlsop_rx1_r",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld1_asisdlsop_b1_i1b",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld1_asisdlsop_bx1_r1b",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld1_asisdlsop_d1_i1d",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld1_asisdlsop_dx1_r1d",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld1_asisdlsop_h1_i1h",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld1_asisdlsop_hx1_r1h",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld1_asisdlsop_s1_i1s",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld1_asisdlsop_sx1_r1s",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld2r_asisdlsop_r2_i",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld2r_asisdlsop_rx2_r",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld2_asisdlsop_b2_i2b",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld2_asisdlsop_bx2_r2b",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld2_asisdlsop_d2_i2d",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld2_asisdlsop_dx2_r2d",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld2_asisdlsop_h2_i2h",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld2_asisdlsop_hx2_r2h",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld2_asisdlsop_s2_i2s",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld2_asisdlsop_sx2_r2s",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld3r_asisdlsop_r3_i",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld3r_asisdlsop_rx3_r",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld3_asisdlsop_b3_i3b",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld3_asisdlsop_bx3_r3b",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld3_asisdlsop_d3_i3d",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld3_asisdlsop_dx3_r3d",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld3_asisdlsop_h3_i3h",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld3_asisdlsop_hx3_r3h",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld3_asisdlsop_s3_i3s",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld3_asisdlsop_sx3_r3s",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld4r_asisdlsop_r4_i",                                                  \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld4r_asisdlsop_rx4_r",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld4_asisdlsop_b4_i4b",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld4_asisdlsop_bx4_r4b",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld4_asisdlsop_d4_i4d",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld4_asisdlsop_dx4_r4d",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld4_asisdlsop_h4_i4h",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld4_asisdlsop_hx4_r4h",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld4_asisdlsop_s4_i4s",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"ld4_asisdlsop_sx4_r4s",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st1_asisdlsop_b1_i1b",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st1_asisdlsop_bx1_r1b",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st1_asisdlsop_d1_i1d",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st1_asisdlsop_dx1_r1d",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st1_asisdlsop_h1_i1h",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st1_asisdlsop_hx1_r1h",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st1_asisdlsop_s1_i1s",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st1_asisdlsop_sx1_r1s",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st2_asisdlsop_b2_i2b",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st2_asisdlsop_bx2_r2b",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st2_asisdlsop_d2_i2d",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st2_asisdlsop_dx2_r2d",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st2_asisdlsop_h2_i2h",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st2_asisdlsop_hx2_r2h",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st2_asisdlsop_s2_i2s",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st2_asisdlsop_sx2_r2s",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st3_asisdlsop_b3_i3b",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st3_asisdlsop_bx3_r3b",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st3_asisdlsop_d3_i3d",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st3_asisdlsop_dx3_r3d",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st3_asisdlsop_h3_i3h",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st3_asisdlsop_hx3_r3h",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st3_asisdlsop_s3_i3s",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st3_asisdlsop_sx3_r3s",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st4_asisdlsop_b4_i4b",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st4_asisdlsop_bx4_r4b",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st4_asisdlsop_d4_i4d",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st4_asisdlsop_dx4_r4d",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st4_asisdlsop_h4_i4h",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st4_asisdlsop_hx4_r4h",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st4_asisdlsop_s4_i4s",                                                 \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"st4_asisdlsop_sx4_r4s",                                                \
       &VISITORCLASS::VisitNEONLoadStoreSingleStructPostIndex},                \
      {"bic_asimdimm_l_hl", &VISITORCLASS::VisitNEONModifiedImmediate},        \
      {"bic_asimdimm_l_sl", &VISITORCLASS::VisitNEONModifiedImmediate},        \
      {"fmov_asimdimm_d2_d", &VISITORCLASS::VisitNEONModifiedImmediate},       \
      {"fmov_asimdimm_h_h", &VISITORCLASS::VisitNEONModifiedImmediate},        \
      {"fmov_asimdimm_s_s", &VISITORCLASS::VisitNEONModifiedImmediate},        \
      {"movi_asimdimm_d2_d", &VISITORCLASS::VisitNEONModifiedImmediate},       \
      {"movi_asimdimm_d_ds", &VISITORCLASS::VisitNEONModifiedImmediate},       \
      {"movi_asimdimm_l_hl", &VISITORCLASS::VisitNEONModifiedImmediate},       \
      {"movi_asimdimm_l_sl", &VISITORCLASS::VisitNEONModifiedImmediate},       \
      {"movi_asimdimm_m_sm", &VISITORCLASS::VisitNEONModifiedImmediate},       \
      {"movi_asimdimm_n_b", &VISITORCLASS::VisitNEONModifiedImmediate},        \
      {"mvni_asimdimm_l_hl", &VISITORCLASS::VisitNEONModifiedImmediate},       \
      {"mvni_asimdimm_l_sl", &VISITORCLASS::VisitNEONModifiedImmediate},       \
      {"mvni_asimdimm_m_sm", &VISITORCLASS::VisitNEONModifiedImmediate},       \
      {"orr_asimdimm_l_hl", &VISITORCLASS::VisitNEONModifiedImmediate},        \
      {"orr_asimdimm_l_sl", &VISITORCLASS::VisitNEONModifiedImmediate},        \
      {"trn1_asimdperm_only", &VISITORCLASS::VisitNEONPerm},                   \
      {"trn2_asimdperm_only", &VISITORCLASS::VisitNEONPerm},                   \
      {"uzp1_asimdperm_only", &VISITORCLASS::VisitNEONPerm},                   \
      {"uzp2_asimdperm_only", &VISITORCLASS::VisitNEONPerm},                   \
      {"zip1_asimdperm_only", &VISITORCLASS::VisitNEONPerm},                   \
      {"zip2_asimdperm_only", &VISITORCLASS::VisitNEONPerm},                   \
      {"sqabs_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},           \
      {"sqneg_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},           \
      {"sqxtn_asisdmisc_n", &VISITORCLASS::VisitNEONScalar2RegMisc},           \
      {"sqxtun_asisdmisc_n", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"suqadd_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"uqxtn_asisdmisc_n", &VISITORCLASS::VisitNEONScalar2RegMisc},           \
      {"usqadd_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcmeq_asisdmiscfp16_fz", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcmge_asisdmiscfp16_fz", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcmgt_asisdmiscfp16_fz", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcmle_asisdmiscfp16_fz", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcmlt_asisdmiscfp16_fz", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcvtas_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcvtau_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcvtms_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcvtmu_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcvtns_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcvtnu_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcvtps_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcvtpu_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcvtzs_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"fcvtzu_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"frecpe_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"frecpx_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},  \
      {"frsqrte_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16}, \
      {"scvtf_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},   \
      {"ucvtf_asisdmiscfp16_r", &VISITORCLASS::VisitNEONScalar2RegMiscFP16},   \
      {"sqdmlal_asisddiff_only", &VISITORCLASS::VisitNEONScalar3Diff},         \
      {"sqdmlsl_asisddiff_only", &VISITORCLASS::VisitNEONScalar3Diff},         \
      {"sqdmull_asisddiff_only", &VISITORCLASS::VisitNEONScalar3Diff},         \
      {"sqadd_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"sqdmulh_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},         \
      {"sqrdmulh_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},        \
      {"sqrshl_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},          \
      {"sqshl_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"sqsub_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"srshl_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"sshl_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},            \
      {"uqadd_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"uqrshl_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},          \
      {"uqshl_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"uqsub_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"urshl_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"ushl_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},            \
      {"fabd_asisdsamefp16_only", &VISITORCLASS::VisitNEONScalar3SameFP16},    \
      {"facge_asisdsamefp16_only", &VISITORCLASS::VisitNEONScalar3SameFP16},   \
      {"facgt_asisdsamefp16_only", &VISITORCLASS::VisitNEONScalar3SameFP16},   \
      {"fcmeq_asisdsamefp16_only", &VISITORCLASS::VisitNEONScalar3SameFP16},   \
      {"fcmge_asisdsamefp16_only", &VISITORCLASS::VisitNEONScalar3SameFP16},   \
      {"fcmgt_asisdsamefp16_only", &VISITORCLASS::VisitNEONScalar3SameFP16},   \
      {"fmulx_asisdsamefp16_only", &VISITORCLASS::VisitNEONScalar3SameFP16},   \
      {"frecps_asisdsamefp16_only", &VISITORCLASS::VisitNEONScalar3SameFP16},  \
      {"frsqrts_asisdsamefp16_only", &VISITORCLASS::VisitNEONScalar3SameFP16}, \
      {"sqdmulh_asisdelem_r", &VISITORCLASS::VisitNEONScalarByIndexedElement}, \
      {"sqrdmlah_asisdelem_r",                                                 \
       &VISITORCLASS::VisitNEONScalarByIndexedElement},                        \
      {"sqrdmlsh_asisdelem_r",                                                 \
       &VISITORCLASS::VisitNEONScalarByIndexedElement},                        \
      {"sqrdmulh_asisdelem_r",                                                 \
       &VISITORCLASS::VisitNEONScalarByIndexedElement},                        \
      {"dup_asisdone_only", &VISITORCLASS::VisitNEONScalarCopy},               \
      {"addp_asisdpair_only", &VISITORCLASS::VisitNEONScalarPairwise},         \
      {"faddp_asisdpair_only_h", &VISITORCLASS::VisitNEONScalarPairwise},      \
      {"faddp_asisdpair_only_sd", &VISITORCLASS::VisitNEONScalarPairwise},     \
      {"fmaxnmp_asisdpair_only_h", &VISITORCLASS::VisitNEONScalarPairwise},    \
      {"fmaxnmp_asisdpair_only_sd", &VISITORCLASS::VisitNEONScalarPairwise},   \
      {"fmaxp_asisdpair_only_h", &VISITORCLASS::VisitNEONScalarPairwise},      \
      {"fmaxp_asisdpair_only_sd", &VISITORCLASS::VisitNEONScalarPairwise},     \
      {"fminnmp_asisdpair_only_h", &VISITORCLASS::VisitNEONScalarPairwise},    \
      {"fminnmp_asisdpair_only_sd", &VISITORCLASS::VisitNEONScalarPairwise},   \
      {"fminp_asisdpair_only_h", &VISITORCLASS::VisitNEONScalarPairwise},      \
      {"fminp_asisdpair_only_sd", &VISITORCLASS::VisitNEONScalarPairwise},     \
      {"fcvtzs_asisdshf_c", &VISITORCLASS::VisitNEONScalarShiftImmediate},     \
      {"fcvtzu_asisdshf_c", &VISITORCLASS::VisitNEONScalarShiftImmediate},     \
      {"scvtf_asisdshf_c", &VISITORCLASS::VisitNEONScalarShiftImmediate},      \
      {"sqshlu_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},     \
      {"sqshl_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},      \
      {"ucvtf_asisdshf_c", &VISITORCLASS::VisitNEONScalarShiftImmediate},      \
      {"uqshl_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},      \
      {"sqshlu_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},           \
      {"sqshl_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},            \
      {"uqshl_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},            \
      {"shl_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},              \
      {"sli_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},              \
      {"tbl_asimdtbl_l1_1", &VISITORCLASS::VisitNEONTable},                    \
      {"tbl_asimdtbl_l2_2", &VISITORCLASS::VisitNEONTable},                    \
      {"tbl_asimdtbl_l3_3", &VISITORCLASS::VisitNEONTable},                    \
      {"tbl_asimdtbl_l4_4", &VISITORCLASS::VisitNEONTable},                    \
      {"tbx_asimdtbl_l1_1", &VISITORCLASS::VisitNEONTable},                    \
      {"tbx_asimdtbl_l2_2", &VISITORCLASS::VisitNEONTable},                    \
      {"tbx_asimdtbl_l3_3", &VISITORCLASS::VisitNEONTable},                    \
      {"tbx_asimdtbl_l4_4", &VISITORCLASS::VisitNEONTable},                    \
      {"adrp_only_pcreladdr", &VISITORCLASS::VisitPCRelAddressing},            \
      {"adr_only_pcreladdr", &VISITORCLASS::VisitPCRelAddressing},             \
      {"rmif_only_rmif", &VISITORCLASS::VisitRotateRightIntoFlags},            \
      {"bti_hb_hints", &VISITORCLASS::VisitSystem},                            \
      {"clrex_bn_barriers", &VISITORCLASS::VisitSystem},                       \
      {"dmb_bo_barriers", &VISITORCLASS::VisitSystem},                         \
      {"dsb_bo_barriers", &VISITORCLASS::VisitSystem},                         \
      {"hint_hm_hints", &VISITORCLASS::VisitSystem},                           \
      {"mrs_rs_systemmove", &VISITORCLASS::VisitSystem},                       \
      {"msr_si_pstate", &VISITORCLASS::VisitSystem},                           \
      {"msr_sr_systemmove", &VISITORCLASS::VisitSystem},                       \
      {"psb_hc_hints", &VISITORCLASS::VisitSystem},                            \
      {"sb_only_barriers", &VISITORCLASS::VisitSystem},                        \
      {"sysl_rc_systeminstrs", &VISITORCLASS::VisitSystem},                    \
      {"sys_cr_systeminstrs", &VISITORCLASS::VisitSystem},                     \
      {"tcommit_only_barriers", &VISITORCLASS::VisitSystem},                   \
      {"tsb_hc_hints", &VISITORCLASS::VisitSystem},                            \
      {"tbnz_only_testbranch", &VISITORCLASS::VisitTestBranch},                \
      {"tbz_only_testbranch", &VISITORCLASS::VisitTestBranch},                 \
      {"bl_only_branch_imm", &VISITORCLASS::VisitUnconditionalBranch},         \
      {"b_only_branch_imm", &VISITORCLASS::VisitUnconditionalBranch},          \
      {"blraaz_64_branch_reg",                                                 \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"blraa_64p_branch_reg",                                                 \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"blrabz_64_branch_reg",                                                 \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"blrab_64p_branch_reg",                                                 \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"blr_64_branch_reg",                                                    \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"braaz_64_branch_reg",                                                  \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"braa_64p_branch_reg",                                                  \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"brabz_64_branch_reg",                                                  \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"brab_64p_branch_reg",                                                  \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"br_64_branch_reg", &VISITORCLASS::VisitUnconditionalBranchToRegister}, \
      {"drps_64e_branch_reg",                                                  \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"eretaa_64e_branch_reg",                                                \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"eretab_64e_branch_reg",                                                \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"eret_64e_branch_reg",                                                  \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"retaa_64e_branch_reg",                                                 \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"retab_64e_branch_reg",                                                 \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"ret_64r_branch_reg",                                                   \
       &VISITORCLASS::VisitUnconditionalBranchToRegister},                     \
      {"addg_64_addsub_immtags", &VISITORCLASS::VisitUnimplemented},           \
      {"bcax_vvv16_crypto4", &VISITORCLASS::VisitUnimplemented},               \
      {"bfcvtn_asimdmisc_4s", &VISITORCLASS::VisitUnimplemented},              \
      {"bfdot_asimdelem_e", &VISITORCLASS::VisitUnimplemented},                \
      {"bfdot_asimdsame2_d", &VISITORCLASS::VisitUnimplemented},               \
      {"bfmlal_asimdelem_f", &VISITORCLASS::VisitUnimplemented},               \
      {"bfmlal_asimdsame2_f_", &VISITORCLASS::VisitUnimplemented},             \
      {"bfmmla_asimdsame2_e", &VISITORCLASS::VisitUnimplemented},              \
      {"dsb_bon_barriers", &VISITORCLASS::VisitUnimplemented},                 \
      {"eor3_vvv16_crypto4", &VISITORCLASS::VisitUnimplemented},               \
      {"ld64b_64l_memop", &VISITORCLASS::VisitUnimplemented},                  \
      {"ldgm_64bulk_ldsttags", &VISITORCLASS::VisitUnimplemented},             \
      {"ldg_64loffset_ldsttags", &VISITORCLASS::VisitUnimplemented},           \
      {"ldtrb_32_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},             \
      {"ldtrh_32_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},             \
      {"ldtrsb_32_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},            \
      {"ldtrsb_64_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},            \
      {"ldtrsh_32_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},            \
      {"ldtrsh_64_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},            \
      {"ldtrsw_64_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},            \
      {"ldtr_32_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},              \
      {"ldtr_64_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},              \
      {"rax1_vvv2_cryptosha512_3", &VISITORCLASS::VisitUnimplemented},         \
      {"sha512h2_qqv_cryptosha512_3", &VISITORCLASS::VisitUnimplemented},      \
      {"sha512h_qqv_cryptosha512_3", &VISITORCLASS::VisitUnimplemented},       \
      {"sha512su0_vv2_cryptosha512_2", &VISITORCLASS::VisitUnimplemented},     \
      {"sha512su1_vvv2_cryptosha512_3", &VISITORCLASS::VisitUnimplemented},    \
      {"sm3partw1_vvv4_cryptosha512_3", &VISITORCLASS::VisitUnimplemented},    \
      {"sm3partw2_vvv4_cryptosha512_3", &VISITORCLASS::VisitUnimplemented},    \
      {"sm3ss1_vvv4_crypto4", &VISITORCLASS::VisitUnimplemented},              \
      {"sm3tt1a_vvv4_crypto3_imm2", &VISITORCLASS::VisitUnimplemented},        \
      {"sm3tt1b_vvv4_crypto3_imm2", &VISITORCLASS::VisitUnimplemented},        \
      {"sm3tt2a_vvv4_crypto3_imm2", &VISITORCLASS::VisitUnimplemented},        \
      {"sm3tt2b_vvv_crypto3_imm2", &VISITORCLASS::VisitUnimplemented},         \
      {"sm4ekey_vvv4_cryptosha512_3", &VISITORCLASS::VisitUnimplemented},      \
      {"sm4e_vv4_cryptosha512_2", &VISITORCLASS::VisitUnimplemented},          \
      {"st2g_64soffset_ldsttags", &VISITORCLASS::VisitUnimplemented},          \
      {"st2g_64spost_ldsttags", &VISITORCLASS::VisitUnimplemented},            \
      {"st2g_64spre_ldsttags", &VISITORCLASS::VisitUnimplemented},             \
      {"st64b_64l_memop", &VISITORCLASS::VisitUnimplemented},                  \
      {"st64bv_64_memop", &VISITORCLASS::VisitUnimplemented},                  \
      {"st64bv0_64_memop", &VISITORCLASS::VisitUnimplemented},                 \
      {"stgm_64bulk_ldsttags", &VISITORCLASS::VisitUnimplemented},             \
      {"stgp_64_ldstpair_off", &VISITORCLASS::VisitUnimplemented},             \
      {"stgp_64_ldstpair_post", &VISITORCLASS::VisitUnimplemented},            \
      {"stgp_64_ldstpair_pre", &VISITORCLASS::VisitUnimplemented},             \
      {"stg_64soffset_ldsttags", &VISITORCLASS::VisitUnimplemented},           \
      {"stg_64spost_ldsttags", &VISITORCLASS::VisitUnimplemented},             \
      {"stg_64spre_ldsttags", &VISITORCLASS::VisitUnimplemented},              \
      {"sttrb_32_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},             \
      {"sttrh_32_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},             \
      {"sttr_32_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},              \
      {"sttr_64_ldst_unpriv", &VISITORCLASS::VisitUnimplemented},              \
      {"stz2g_64soffset_ldsttags", &VISITORCLASS::VisitUnimplemented},         \
      {"stz2g_64spost_ldsttags", &VISITORCLASS::VisitUnimplemented},           \
      {"stz2g_64spre_ldsttags", &VISITORCLASS::VisitUnimplemented},            \
      {"stzgm_64bulk_ldsttags", &VISITORCLASS::VisitUnimplemented},            \
      {"stzg_64soffset_ldsttags", &VISITORCLASS::VisitUnimplemented},          \
      {"stzg_64spost_ldsttags", &VISITORCLASS::VisitUnimplemented},            \
      {"stzg_64spre_ldsttags", &VISITORCLASS::VisitUnimplemented},             \
      {"subg_64_addsub_immtags", &VISITORCLASS::VisitUnimplemented},           \
      {"subps_64s_dp_2src", &VISITORCLASS::VisitUnimplemented},                \
      {"subp_64s_dp_2src", &VISITORCLASS::VisitUnimplemented},                 \
      {"tcancel_ex_exception", &VISITORCLASS::VisitUnimplemented},             \
      {"tstart_br_systemresult", &VISITORCLASS::VisitUnimplemented},           \
      {"ttest_br_systemresult", &VISITORCLASS::VisitUnimplemented},            \
      {"wfet_only_systeminstrswithreg", &VISITORCLASS::VisitUnimplemented},    \
      {"wfit_only_systeminstrswithreg", &VISITORCLASS::VisitUnimplemented},    \
      {"xar_vvv2_crypto3_imm6", &VISITORCLASS::VisitUnimplemented},            \
      {"bfcvt_z_p_z_s2bf", &VISITORCLASS::VisitUnimplemented},                 \
      {"bfcvtnt_z_p_z_s2bf", &VISITORCLASS::VisitUnimplemented},               \
      {"bfdot_z_zzz_", &VISITORCLASS::VisitUnimplemented},                     \
      {"bfdot_z_zzzi_", &VISITORCLASS::VisitUnimplemented},                    \
      {"bfmlalb_z_zzz_", &VISITORCLASS::VisitUnimplemented},                   \
      {"bfmlalb_z_zzzi_", &VISITORCLASS::VisitUnimplemented},                  \
      {"bfmlalt_z_zzz_", &VISITORCLASS::VisitUnimplemented},                   \
      {"bfmlalt_z_zzzi_", &VISITORCLASS::VisitUnimplemented},                  \
      {"bfmmla_z_zzz_", &VISITORCLASS::VisitUnimplemented}, {                  \
    "Unallocated", &VISITORCLASS::VisitUnallocated                             \
  }

#define SIM_AUD_VISITOR_MAP(VISITORCLASS)                                      \
  {"autia1716_hi_hints", &VISITORCLASS::VisitSystem},                          \
      {"autiasp_hi_hints", &VISITORCLASS::VisitSystem},                        \
      {"autiaz_hi_hints", &VISITORCLASS::VisitSystem},                         \
      {"autib1716_hi_hints", &VISITORCLASS::VisitSystem},                      \
      {"autibsp_hi_hints", &VISITORCLASS::VisitSystem},                        \
      {"autibz_hi_hints", &VISITORCLASS::VisitSystem},                         \
      {"axflag_m_pstate", &VISITORCLASS::VisitSystem},                         \
      {"cfinv_m_pstate", &VISITORCLASS::VisitSystem},                          \
      {"csdb_hi_hints", &VISITORCLASS::VisitSystem},                           \
      {"dgh_hi_hints", &VISITORCLASS::VisitSystem},                            \
      {"esb_hi_hints", &VISITORCLASS::VisitSystem},                            \
      {"isb_bi_barriers", &VISITORCLASS::VisitSystem},                         \
      {"nop_hi_hints", &VISITORCLASS::VisitSystem},                            \
      {"pacia1716_hi_hints", &VISITORCLASS::VisitSystem},                      \
      {"paciasp_hi_hints", &VISITORCLASS::VisitSystem},                        \
      {"paciaz_hi_hints", &VISITORCLASS::VisitSystem},                         \
      {"pacib1716_hi_hints", &VISITORCLASS::VisitSystem},                      \
      {"pacibsp_hi_hints", &VISITORCLASS::VisitSystem},                        \
      {"pacibz_hi_hints", &VISITORCLASS::VisitSystem},                         \
      {"pssbb_only_barriers", &VISITORCLASS::VisitSystem},                     \
      {"sev_hi_hints", &VISITORCLASS::VisitSystem},                            \
      {"sevl_hi_hints", &VISITORCLASS::VisitSystem},                           \
      {"ssbb_only_barriers", &VISITORCLASS::VisitSystem},                      \
      {"wfe_hi_hints", &VISITORCLASS::VisitSystem},                            \
      {"wfi_hi_hints", &VISITORCLASS::VisitSystem},                            \
      {"xaflag_m_pstate", &VISITORCLASS::VisitSystem},                         \
      {"xpaclri_hi_hints", &VISITORCLASS::VisitSystem},                        \
      {"yield_hi_hints", &VISITORCLASS::VisitSystem},                          \
      {"abs_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                   \
      {"cls_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                   \
      {"clz_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                   \
      {"cmeq_asimdmisc_z", &VISITORCLASS::VisitNEON2RegMisc},                  \
      {"cmge_asimdmisc_z", &VISITORCLASS::VisitNEON2RegMisc},                  \
      {"cmgt_asimdmisc_z", &VISITORCLASS::VisitNEON2RegMisc},                  \
      {"cmle_asimdmisc_z", &VISITORCLASS::VisitNEON2RegMisc},                  \
      {"cmlt_asimdmisc_z", &VISITORCLASS::VisitNEON2RegMisc},                  \
      {"cnt_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                   \
      {"fabs_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                  \
      {"fcmeq_asimdmisc_fz", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcmge_asimdmisc_fz", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcmgt_asimdmisc_fz", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcmle_asimdmisc_fz", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcmlt_asimdmisc_fz", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcvtas_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcvtau_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcvtl_asimdmisc_l", &VISITORCLASS::VisitNEON2RegMisc},                 \
      {"fcvtms_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcvtmu_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcvtns_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcvtnu_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcvtn_asimdmisc_n", &VISITORCLASS::VisitNEON2RegMisc},                 \
      {"fcvtps_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcvtpu_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcvtxn_asimdmisc_n", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcvtzs_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fcvtzu_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"fneg_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                  \
      {"frecpe_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"frint32x_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},              \
      {"frint32z_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},              \
      {"frint64x_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},              \
      {"frint64z_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},              \
      {"frinta_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"frinti_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"frintm_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"frintn_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"frintp_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"frintx_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"frintz_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"frsqrte_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},               \
      {"fsqrt_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                 \
      {"neg_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                   \
      {"not_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                   \
      {"rbit_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                  \
      {"rev16_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                 \
      {"rev32_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                 \
      {"rev64_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                 \
      {"sadalp_asimdmisc_p", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"saddlp_asimdmisc_p", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"scvtf_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                 \
      {"shll_asimdmisc_s", &VISITORCLASS::VisitNEON2RegMisc},                  \
      {"sqabs_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                 \
      {"sqneg_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                 \
      {"sqxtn_asimdmisc_n", &VISITORCLASS::VisitNEON2RegMisc},                 \
      {"sqxtun_asimdmisc_n", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"suqadd_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"uadalp_asimdmisc_p", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"uaddlp_asimdmisc_p", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"ucvtf_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                 \
      {"uqxtn_asimdmisc_n", &VISITORCLASS::VisitNEON2RegMisc},                 \
      {"urecpe_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"ursqrte_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},               \
      {"usqadd_asimdmisc_r", &VISITORCLASS::VisitNEON2RegMisc},                \
      {"xtn_asimdmisc_n", &VISITORCLASS::VisitNEON2RegMisc},                   \
      {"mla_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"mls_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"mul_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"saba_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"sabd_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"shadd_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"shsub_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"smaxp_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"smax_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"sminp_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"smin_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"srhadd_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                \
      {"uaba_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"uabd_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"uhadd_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"uhsub_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"umaxp_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"umax_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"uminp_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                 \
      {"umin_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"urhadd_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                \
      {"and_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"bic_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"bif_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"bit_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"bsl_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"eor_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"orr_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"orn_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                   \
      {"pmul_asimdsame_only", &VISITORCLASS::VisitNEON3Same},                  \
      {"fmlal2_asimdsame_f", &VISITORCLASS::VisitNEON3Same},                   \
      {"fmlal_asimdsame_f", &VISITORCLASS::VisitNEON3Same},                    \
      {"fmlsl2_asimdsame_f", &VISITORCLASS::VisitNEON3Same},                   \
      {"fmlsl_asimdsame_f", &VISITORCLASS::VisitNEON3Same},                    \
      {"ushll_asimdshf_l", &VISITORCLASS::VisitNEONShiftImmediate},            \
      {"sshll_asimdshf_l", &VISITORCLASS::VisitNEONShiftImmediate},            \
      {"shrn_asimdshf_n", &VISITORCLASS::VisitNEONShiftImmediate},             \
      {"rshrn_asimdshf_n", &VISITORCLASS::VisitNEONShiftImmediate},            \
      {"sqshrn_asimdshf_n", &VISITORCLASS::VisitNEONShiftImmediate},           \
      {"sqrshrn_asimdshf_n", &VISITORCLASS::VisitNEONShiftImmediate},          \
      {"sqshrun_asimdshf_n", &VISITORCLASS::VisitNEONShiftImmediate},          \
      {"sqrshrun_asimdshf_n", &VISITORCLASS::VisitNEONShiftImmediate},         \
      {"uqshrn_asimdshf_n", &VISITORCLASS::VisitNEONShiftImmediate},           \
      {"uqrshrn_asimdshf_n", &VISITORCLASS::VisitNEONShiftImmediate},          \
      {"sri_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},              \
      {"srshr_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},            \
      {"srsra_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},            \
      {"sshr_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},             \
      {"ssra_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},             \
      {"urshr_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},            \
      {"ursra_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},            \
      {"ushr_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},             \
      {"usra_asimdshf_r", &VISITORCLASS::VisitNEONShiftImmediate},             \
      {"scvtf_asimdshf_c", &VISITORCLASS::VisitNEONShiftImmediate},            \
      {"ucvtf_asimdshf_c", &VISITORCLASS::VisitNEONShiftImmediate},            \
      {"fcvtzs_asimdshf_c", &VISITORCLASS::VisitNEONShiftImmediate},           \
      {"fcvtzu_asimdshf_c", &VISITORCLASS::VisitNEONShiftImmediate},           \
      {"sqdmlal_asisdelem_l", &VISITORCLASS::VisitNEONScalarByIndexedElement}, \
      {"sqdmlsl_asisdelem_l", &VISITORCLASS::VisitNEONScalarByIndexedElement}, \
      {"sqdmull_asisdelem_l", &VISITORCLASS::VisitNEONScalarByIndexedElement}, \
      {"fmla_asisdelem_rh_h", &VISITORCLASS::VisitNEONScalarByIndexedElement}, \
      {"fmla_asisdelem_r_sd", &VISITORCLASS::VisitNEONScalarByIndexedElement}, \
      {"fmls_asisdelem_rh_h", &VISITORCLASS::VisitNEONScalarByIndexedElement}, \
      {"fmls_asisdelem_r_sd", &VISITORCLASS::VisitNEONScalarByIndexedElement}, \
      {"fmulx_asisdelem_rh_h",                                                 \
       &VISITORCLASS::VisitNEONScalarByIndexedElement},                        \
      {"fmulx_asisdelem_r_sd",                                                 \
       &VISITORCLASS::VisitNEONScalarByIndexedElement},                        \
      {"fmul_asisdelem_rh_h", &VISITORCLASS::VisitNEONScalarByIndexedElement}, \
      {"fmul_asisdelem_r_sd", &VISITORCLASS::VisitNEONScalarByIndexedElement}, \
      {"fabd_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},            \
      {"facge_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"facgt_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"fcmeq_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"fcmge_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"fcmgt_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"fmulx_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"frecps_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},          \
      {"frsqrts_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},         \
      {"cmeq_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},            \
      {"cmge_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},            \
      {"cmgt_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},            \
      {"cmhi_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},            \
      {"cmhs_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},            \
      {"cmtst_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},           \
      {"add_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},             \
      {"sub_asisdsame_only", &VISITORCLASS::VisitNEONScalar3Same},             \
      {"sqrdmlah_asisdsame2_only", &VISITORCLASS::VisitNEONScalar3SameExtra},  \
      {"sqrdmlsh_asisdsame2_only", &VISITORCLASS::VisitNEONScalar3SameExtra},  \
      {"fmaxnmv_asimdall_only_h", &VISITORCLASS::VisitNEONAcrossLanes},        \
      {"fmaxv_asimdall_only_h", &VISITORCLASS::VisitNEONAcrossLanes},          \
      {"fminnmv_asimdall_only_h", &VISITORCLASS::VisitNEONAcrossLanes},        \
      {"fminv_asimdall_only_h", &VISITORCLASS::VisitNEONAcrossLanes},          \
      {"fmaxnmv_asimdall_only_sd", &VISITORCLASS::VisitNEONAcrossLanes},       \
      {"fminnmv_asimdall_only_sd", &VISITORCLASS::VisitNEONAcrossLanes},       \
      {"fmaxv_asimdall_only_sd", &VISITORCLASS::VisitNEONAcrossLanes},         \
      {"fminv_asimdall_only_sd", &VISITORCLASS::VisitNEONAcrossLanes},         \
      {"shl_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},        \
      {"sli_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},        \
      {"sri_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},        \
      {"srshr_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},      \
      {"srsra_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},      \
      {"sshr_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},       \
      {"ssra_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},       \
      {"urshr_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},      \
      {"ursra_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},      \
      {"ushr_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},       \
      {"usra_asisdshf_r", &VISITORCLASS::VisitNEONScalarShiftImmediate},       \
      {"sqrshrn_asisdshf_n", &VISITORCLASS::VisitNEONScalarShiftImmediate},    \
      {"sqrshrun_asisdshf_n", &VISITORCLASS::VisitNEONScalarShiftImmediate},   \
      {"sqshrn_asisdshf_n", &VISITORCLASS::VisitNEONScalarShiftImmediate},     \
      {"sqshrun_asisdshf_n", &VISITORCLASS::VisitNEONScalarShiftImmediate},    \
      {"uqrshrn_asisdshf_n", &VISITORCLASS::VisitNEONScalarShiftImmediate},    \
      {"uqshrn_asisdshf_n", &VISITORCLASS::VisitNEONScalarShiftImmediate},     \
      {"cmeq_asisdmisc_z", &VISITORCLASS::VisitNEONScalar2RegMisc},            \
      {"cmge_asisdmisc_z", &VISITORCLASS::VisitNEONScalar2RegMisc},            \
      {"cmgt_asisdmisc_z", &VISITORCLASS::VisitNEONScalar2RegMisc},            \
      {"cmle_asisdmisc_z", &VISITORCLASS::VisitNEONScalar2RegMisc},            \
      {"cmlt_asisdmisc_z", &VISITORCLASS::VisitNEONScalar2RegMisc},            \
      {"abs_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},             \
      {"neg_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},             \
      {"fcmeq_asisdmisc_fz", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcmge_asisdmisc_fz", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcmgt_asisdmisc_fz", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcmle_asisdmisc_fz", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcmlt_asisdmisc_fz", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcvtas_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcvtau_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcvtms_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcvtmu_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcvtns_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcvtnu_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcvtps_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcvtpu_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcvtxn_asisdmisc_n", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcvtzs_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"fcvtzu_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"frecpe_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"frecpx_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},          \
      {"frsqrte_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc},         \
      {"scvtf_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc}, {         \
    "ucvtf_asisdmisc_r", &VISITORCLASS::VisitNEONScalar2RegMisc                \
  }
