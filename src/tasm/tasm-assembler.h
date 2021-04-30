// Copyright 2021, VIXL authors
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

#ifndef VIXL_TASM_ASSEMBLER_H
#define VIXL_TASM_ASSEMBLER_H

#if __cplusplus < 201703L
#error "Text Assembler requires C++17 standard or above."
#else

#include <iostream>
#include <map>
#include <string>
#include <variant>  // NOLINT(build/include_order)

#include "tasm-errors.h"
#include "tasm-parser.h"
#include "aarch64/assembler-aarch64.h"

namespace vixl {
namespace aarch64 {
namespace tasm {

class TextAssembler;

using VIXL_vv_Fn = void (TextAssembler::*)(const VRegister& vd,
                                           const VRegister& vn);

using VIXL_zMz_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const PRegisterM& pg,
                                            const ZRegister& zn);

using VIXL_rro_Fn = void (TextAssembler::*)(const Register& rd,
                                            const Register& rn,
                                            const Operand& operand);

using VIXL_vvv_Fn = void (TextAssembler::*)(const VRegister& vd,
                                            const VRegister& vn,
                                            const VRegister& vm);

using VIXL_zMzz_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                             const PRegisterM& pg,
                                             const ZRegister& zn,
                                             const ZRegister& zm);

using VIXL_zzi_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const ZRegister& zn,
                                            int imm8);

using VIXL_zzii_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                             const ZRegister& zn,
                                             int imm8,
                                             int shift);

using VIXL_zzz_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const ZRegister& zn,
                                            const ZRegister& zm);

using VIXL_rri_Fn = void (TextAssembler::*)(const Register& xd,
                                            const Register& xn,
                                            int imm6);

using VIXL_rl_Fn = void (TextAssembler::*)(const Register& xd, Label* label);

using VIXL_ri64_Fn = void (TextAssembler::*)(const Register& xd, int64_t imm21);

using VIXL_zs_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                           const SVEMemOperand& addr);

using VIXL_LZLL_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                             const PRegisterZ& pg,
                                             const PRegisterWithLaneSize& pn,
                                             const PRegisterWithLaneSize& pm);

using VIXL_zzu64_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                              const ZRegister& zn,
                                              uint64_t imm);

using VIXL_vpz_Fn = void (TextAssembler::*)(const VRegister& vd,
                                            const PRegister& pg,
                                            const ZRegister& zn);

using VIXL_rru_Fn = void (TextAssembler::*)(const Register& rd,
                                            const Register& rn,
                                            unsigned shift);

using VIXL_zMzi_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                             const PRegisterM& pg,
                                             const ZRegister& zn,
                                             int shift);

using VIXL_rrr_Fn = void (TextAssembler::*)(const Register& rd,
                                            const Register& rn,
                                            const Register& rm);

using VIXL_rr_Fn = void (TextAssembler::*)(const Register& xd,
                                           const Register& xn);

using VIXL_r_Fn = void (TextAssembler::*)(const Register& xd);

using VIXL__Fn = void (TextAssembler::*)();

using VIXL_lc_Fn = void (TextAssembler::*)(Label* label, Condition cond);

using VIXL_i64c_Fn = void (TextAssembler::*)(int64_t imm19, Condition cond);

using VIXL_l_Fn = void (TextAssembler::*)(Label* label);

using VIXL_i64_Fn = void (TextAssembler::*)(int64_t imm26);

using VIXL_ruu_Fn = void (TextAssembler::*)(const Register& rd,
                                            unsigned lsb,
                                            unsigned width);

using VIXL_rruu_Fn = void (TextAssembler::*)(const Register& rd,
                                             const Register& rn,
                                             unsigned lsb,
                                             unsigned width);

using VIXL_vi_Fn = void (TextAssembler::*)(const VRegister& vd, const int imm8);

using VIXL_vii_Fn = void (TextAssembler::*)(const VRegister& vd,
                                            const int imm8,
                                            const int left_shift);

using VIXL_i_Fn = void (TextAssembler::*)(int code);

using VIXL_LML_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                            const PRegisterM& pg,
                                            const PRegisterWithLaneSize& pn);

using VIXL_LZL_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                            const PRegisterZ& pg,
                                            const PRegisterWithLaneSize& pn);

using VIXL_T_Fn = void (TextAssembler::*)(BranchTargetIdentifier id);

using VIXL_rrm_Fn = void (TextAssembler::*)(const Register& rs,
                                            const Register& rt,
                                            const MemOperand& src);

using VIXL_rrrrm_Fn = void (TextAssembler::*)(const Register& rs,
                                              const Register& rs2,
                                              const Register& rt,
                                              const Register& rt2,
                                              const MemOperand& src);

using VIXL_roFc_Fn = void (TextAssembler::*)(const Register& rn,
                                             const Operand& operand,
                                             StatusFlags nzcv,
                                             Condition cond);

using VIXL_rrc_Fn = void (TextAssembler::*)(const Register& rd,
                                            const Register& rn,
                                            Condition cond);

using VIXL_rprz_Fn = void (TextAssembler::*)(const Register& rd,
                                             const PRegister& pg,
                                             const Register& rn,
                                             const ZRegister& zm);

using VIXL_vpvz_Fn = void (TextAssembler::*)(const VRegister& vd,
                                             const PRegister& pg,
                                             const VRegister& vn,
                                             const ZRegister& zm);

using VIXL_zpzz_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                             const PRegister& pg,
                                             const ZRegister& zn,
                                             const ZRegister& zm);

using VIXL_vvi_Fn = void (TextAssembler::*)(const VRegister& vd,
                                            const VRegister& vn,
                                            int value);

using VIXL_ro_Fn = void (TextAssembler::*)(const Register& rn,
                                           const Operand& operand);

using VIXL_LZzi_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                             const PRegisterZ& pg,
                                             const ZRegister& zn,
                                             int imm5);

using VIXL_LZzu_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                             const PRegisterZ& pg,
                                             const ZRegister& zn,
                                             unsigned imm7);

using VIXL_LZzz_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                             const PRegisterZ& pg,
                                             const ZRegister& zn,
                                             const ZRegister& zm);

using VIXL_rii_Fn = void (TextAssembler::*)(const Register& rd,
                                            int pattern,
                                            int multiplier);

using VIXL_ri_Fn = void (TextAssembler::*)(const Register& rd, int pattern);

using VIXL_rpL_Fn = void (TextAssembler::*)(const Register& xd,
                                            const PRegister& pg,
                                            const PRegisterWithLaneSize& pn);

using VIXL_zpz_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const PRegister& pg,
                                            const ZRegister& zn);

using VIXL_zZii_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                             const PRegisterZ& pg,
                                             int imm8,
                                             int shift);

using VIXL_zZi_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const PRegisterZ& pg,
                                            int imm8);

using VIXL_zMii_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                             const PRegisterM& pg,
                                             int imm8,
                                             int shift);

using VIXL_zMi_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const PRegisterM& pg,
                                            int imm8);

using VIXL_zMr_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const PRegisterM& pg,
                                            const Register& rn);

using VIXL_zMv_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const PRegisterM& pg,
                                            const VRegister& vn);

using VIXL_rrrc_Fn = void (TextAssembler::*)(const Register& rd,
                                             const Register& rn,
                                             const Register& rm,
                                             Condition cond);

using VIXL_rc_Fn = void (TextAssembler::*)(const Register& rd, Condition cond);

using VIXL_Dr_Fn = void (TextAssembler::*)(DataCacheOp op, const Register& rt);

using VIXL_zii_Fn = void (TextAssembler::*)(const ZRegister& zdn,
                                            int pattern,
                                            int multiplier);

using VIXL_z_Fn = void (TextAssembler::*)(const ZRegister& zdn);

using VIXL_zi_Fn = void (TextAssembler::*)(const ZRegister& zdn, int pattern);

using VIXL_rL_Fn = void (TextAssembler::*)(const Register& rdn,
                                           const PRegisterWithLaneSize& pg);

using VIXL_Bb_Fn = void (TextAssembler::*)(BarrierDomain domain,
                                           BarrierType type);

using VIXL_vr_Fn = void (TextAssembler::*)(const VRegister& vd,
                                           const Register& rn);

using VIXL_zr_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                           const Register& xn);

using VIXL_zzu_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const ZRegister& zn,
                                            unsigned index);

using VIXL_zu64_Fn = void (TextAssembler::*)(const ZRegister& zd, uint64_t imm);

using VIXL_vvvi_Fn = void (TextAssembler::*)(const VRegister& vd,
                                             const VRegister& vn,
                                             const VRegister& vm,
                                             int index);

using VIXL_zzzu_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                             const ZRegister& zn,
                                             const ZRegister& zm,
                                             unsigned offset);

using VIXL_rrru_Fn = void (TextAssembler::*)(const Register& rd,
                                             const Register& rn,
                                             const Register& rm,
                                             unsigned lsb);

using VIXL_zMzd_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                             const PRegisterM& pg,
                                             const ZRegister& zn,
                                             double imm);

using VIXL_zMzzi_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                              const PRegisterM& pg,
                                              const ZRegister& zn,
                                              const ZRegister& zm,
                                              int rot);

using VIXL_vvFc_Fn = void (TextAssembler::*)(const VRegister& vn,
                                             const VRegister& vm,
                                             StatusFlags nzcv,
                                             Condition cond);

using VIXL_vvd_Fn = void (TextAssembler::*)(const VRegister& vd,
                                            const VRegister& vn,
                                            double imm);

using VIXL_LZzd_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                             const PRegisterZ& pg,
                                             const ZRegister& zn,
                                             double zero);

using VIXL_vvvii_Fn = void (TextAssembler::*)(const VRegister& vd,
                                              const VRegister& vn,
                                              const VRegister& vm,
                                              int vm_index,
                                              int rot);

using VIXL_zzzii_Fn = void (TextAssembler::*)(const ZRegister& zda,
                                              const ZRegister& zn,
                                              const ZRegister& zm,
                                              int index,
                                              int rot);

using VIXL_vd_Fn = void (TextAssembler::*)(const VRegister& vn, double value);

using VIXL_zMd_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const PRegisterM& pg,
                                            double imm);

using VIXL_zMf16_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                              const PRegisterM& pg,
                                              Float16 imm);

using VIXL_vvvc_Fn = void (TextAssembler::*)(const VRegister& vd,
                                             const VRegister& vn,
                                             const VRegister& vm,
                                             Condition cond);

using VIXL_rv_Fn = void (TextAssembler::*)(const Register& rd,
                                           const VRegister& vn);

using VIXL_rvi_Fn = void (TextAssembler::*)(const Register& rd,
                                            const VRegister& vn,
                                            int fbits);

using VIXL_zd_Fn = void (TextAssembler::*)(const ZRegister& zd, double imm);

using VIXL_zf16_Fn = void (TextAssembler::*)(const ZRegister& zd, Float16 imm);

using VIXL_zz_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                           const ZRegister& zn);

using VIXL_vvvv_Fn = void (TextAssembler::*)(const VRegister& vd,
                                             const VRegister& vn,
                                             const VRegister& vm,
                                             const VRegister& va);

using VIXL_zzzi_Fn = void (TextAssembler::*)(const ZRegister& zda,
                                             const ZRegister& zn,
                                             const ZRegister& zm,
                                             int index);

using VIXL_vf_Fn = void (TextAssembler::*)(const VRegister& vd, float imm);

using VIXL_vf16_Fn = void (TextAssembler::*)(const VRegister& vd, Float16 imm);

using VIXL_Ir_Fn = void (TextAssembler::*)(InstructionCacheOp op,
                                           const Register& rt);

using VIXL_zir_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            int imm5,
                                            const Register& rm);

using VIXL_zri_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const Register& rn,
                                            int imm5);

using VIXL_zrr_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const Register& rn,
                                            const Register& rm);

using VIXL_vivi_Fn = void (TextAssembler::*)(const VRegister& vd,
                                             int vd_index,
                                             const VRegister& vn,
                                             int vn_index);

using VIXL_vir_Fn = void (TextAssembler::*)(const VRegister& vd,
                                            int vd_index,
                                            const Register& rn);

using VIXL_zv_Fn = void (TextAssembler::*)(const ZRegister& zdn,
                                           const VRegister& vm);

using VIXL_rpz_Fn = void (TextAssembler::*)(const Register& rd,
                                            const PRegister& pg,
                                            const ZRegister& zn);

using VIXL_vm_Fn = void (TextAssembler::*)(const VRegister& vt,
                                           const MemOperand& src);

using VIXL_vvm_Fn = void (TextAssembler::*)(const VRegister& vt,
                                            const VRegister& vt2,
                                            const MemOperand& src);

using VIXL_vvvm_Fn = void (TextAssembler::*)(const VRegister& vt,
                                             const VRegister& vt2,
                                             const VRegister& vt3,
                                             const MemOperand& src);

using VIXL_vvvvm_Fn = void (TextAssembler::*)(const VRegister& vt,
                                              const VRegister& vt2,
                                              const VRegister& vt3,
                                              const VRegister& vt4,
                                              const MemOperand& src);

using VIXL_vim_Fn = void (TextAssembler::*)(const VRegister& vt,
                                            int lane,
                                            const MemOperand& src);

using VIXL_zZs_Fn = void (TextAssembler::*)(const ZRegister& zt,
                                            const PRegisterZ& pg,
                                            const SVEMemOperand& addr);

using VIXL_vvim_Fn = void (TextAssembler::*)(const VRegister& vt,
                                             const VRegister& vt2,
                                             int lane,
                                             const MemOperand& src);

using VIXL_zzZs_Fn = void (TextAssembler::*)(const ZRegister& zt1,
                                             const ZRegister& zt2,
                                             const PRegisterZ& pg,
                                             const SVEMemOperand& addr);

using VIXL_vvvim_Fn = void (TextAssembler::*)(const VRegister& vt,
                                              const VRegister& vt2,
                                              const VRegister& vt3,
                                              int lane,
                                              const MemOperand& src);

using VIXL_zzzZs_Fn = void (TextAssembler::*)(const ZRegister& zt1,
                                              const ZRegister& zt2,
                                              const ZRegister& zt3,
                                              const PRegisterZ& pg,
                                              const SVEMemOperand& addr);

using VIXL_vvvvim_Fn = void (TextAssembler::*)(const VRegister& vt,
                                               const VRegister& vt2,
                                               const VRegister& vt3,
                                               const VRegister& vt4,
                                               int lane,
                                               const MemOperand& src);

using VIXL_zzzzZs_Fn = void (TextAssembler::*)(const ZRegister& zt1,
                                               const ZRegister& zt2,
                                               const ZRegister& zt3,
                                               const ZRegister& zt4,
                                               const PRegisterZ& pg,
                                               const SVEMemOperand& addr);

using VIXL_rm_Fn = void (TextAssembler::*)(const Register& rt,
                                           const MemOperand& src);

using VIXL_vi64_Fn = void (TextAssembler::*)(const VRegister& rt,
                                             int64_t imm19);

using VIXL_pm_Fn = void (TextAssembler::*)(const PRegister& rt,
                                           const MemOperand& src);

using VIXL_ps_Fn = void (TextAssembler::*)(const PRegister& rt,
                                           const SVEMemOperand& addr);

using VIXL_zm_Fn = void (TextAssembler::*)(const ZRegister& rt,
                                           const MemOperand& src);

using VIXL_rrrr_Fn = void (TextAssembler::*)(const Register& rd,
                                             const Register& rn,
                                             const Register& rm,
                                             const Register& ra);

using VIXL_ru64_Fn = void (TextAssembler::*)(const Register& rd, uint64_t imm);

using VIXL_vu64Si_Fn = void (TextAssembler::*)(const VRegister& vd,
                                               const uint64_t imm,
                                               Shift shift,
                                               const int shift_amount);

using VIXL_vu64_Fn = void (TextAssembler::*)(const VRegister& vd,
                                             const uint64_t imm);

using VIXL_ru64i_Fn = void (TextAssembler::*)(const Register& rd,
                                              uint64_t imm,
                                              int shift);

using VIXL_zZz_Fn = void (TextAssembler::*)(const ZRegister& zd,
                                            const PRegisterZ& pg,
                                            const ZRegister& zn);

using VIXL_rR_Fn = void (TextAssembler::*)(const Register& xt,
                                           SystemRegister sysreg);

using VIXL_Rr_Fn = void (TextAssembler::*)(SystemRegister sysreg,
                                           const Register& xt);

using VIXL_viSi_Fn = void (TextAssembler::*)(const VRegister& vd,
                                             const int imm8,
                                             Shift shift,
                                             const int shift_amount);

using VIXL_L_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd);

using VIXL_LpL_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                            const PRegister& pg,
                                            const PRegisterWithLaneSize& pn);

using VIXL_Pps_Fn = void (TextAssembler::*)(PrefetchOperation prfop,
                                            const PRegister& pg,
                                            const SVEMemOperand& addr);

using VIXL_Pm_Fn = void (TextAssembler::*)(PrefetchOperation op,
                                           const MemOperand& addr);

using VIXL_im_Fn = void (TextAssembler::*)(int op, const MemOperand& addr);

using VIXL_Pi64_Fn = void (TextAssembler::*)(PrefetchOperation op,
                                             int64_t imm19);

using VIXL_ii64_Fn = void (TextAssembler::*)(int op, int64_t imm19);

using VIXL_pL_Fn = void (TextAssembler::*)(const PRegister& pg,
                                           const PRegisterWithLaneSize& pn);

using VIXL_Li_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                           int pattern);

using VIXL_LL_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                           const PRegisterWithLaneSize& pn);

using VIXL_LZ_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                           const PRegisterZ& pg);

using VIXL_ruF_Fn = void (TextAssembler::*)(const Register& xn,
                                            unsigned rotation,
                                            StatusFlags flags);

using VIXL_vri_Fn = void (TextAssembler::*)(const VRegister& fd,
                                            const Register& rn,
                                            int fbits);

using VIXL_LpLL_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                             const PRegister& pg,
                                             const PRegisterWithLaneSize& pn,
                                             const PRegisterWithLaneSize& pm);

using VIXL_rrii_Fn = void (TextAssembler::*)(const Register& xd,
                                             const Register& wn,
                                             int pattern,
                                             int multiplier);

using VIXL_rLr_Fn = void (TextAssembler::*)(const Register& xd,
                                            const PRegisterWithLaneSize& pg,
                                            const Register& wn);

using VIXL_zps_Fn = void (TextAssembler::*)(const ZRegister& zt,
                                            const PRegister& pg,
                                            const SVEMemOperand& addr);

using VIXL_zzps_Fn = void (TextAssembler::*)(const ZRegister& zt1,
                                             const ZRegister& zt2,
                                             const PRegister& pg,
                                             const SVEMemOperand& addr);

using VIXL_zzzps_Fn = void (TextAssembler::*)(const ZRegister& zt1,
                                              const ZRegister& zt2,
                                              const ZRegister& zt3,
                                              const PRegister& pg,
                                              const SVEMemOperand& addr);

using VIXL_zzzzps_Fn = void (TextAssembler::*)(const ZRegister& zt1,
                                               const ZRegister& zt2,
                                               const ZRegister& zt3,
                                               const ZRegister& zt4,
                                               const PRegister& pg,
                                               const SVEMemOperand& addr);

using VIXL_rrrm_Fn = void (TextAssembler::*)(const Register& rs,
                                             const Register& rt,
                                             const Register& rt2,
                                             const MemOperand& dst);

using VIXL_iiii_Fn = void (TextAssembler::*)(int op1,
                                             int crn,
                                             int crm,
                                             int op2);

using VIXL_iiiir_Fn = void (TextAssembler::*)(
    int op1, int crn, int crm, int op2, const Register& xt);

using VIXL_vvvvv_Fn = void (TextAssembler::*)(const VRegister& vd,
                                              const VRegister& vn,
                                              const VRegister& vn2,
                                              const VRegister& vn3,
                                              const VRegister& vm);

using VIXL_vvvvvv_Fn = void (TextAssembler::*)(const VRegister& vd,
                                               const VRegister& vn,
                                               const VRegister& vn2,
                                               const VRegister& vn3,
                                               const VRegister& vn4,
                                               const VRegister& vm);

using VIXL_rul_Fn = void (TextAssembler::*)(const Register& rt,
                                            unsigned bit_pos,
                                            Label* label);

using VIXL_rui64_Fn = void (TextAssembler::*)(const Register& rt,
                                              unsigned bit_pos,
                                              int64_t imm14);

using VIXL_LLL_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                            const PRegisterWithLaneSize& pn,
                                            const PRegisterWithLaneSize& pm);

using VIXL_Lrr_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize& pd,
                                            const Register& rn,
                                            const Register& rm);

using AssemblerFn = std::variant<std::monostate,
                                 VIXL_vv_Fn,
                                 VIXL_zMz_Fn,
                                 VIXL_rro_Fn,
                                 VIXL_vvv_Fn,
                                 VIXL_zMzz_Fn,
                                 VIXL_zzi_Fn,
                                 VIXL_zzii_Fn,
                                 VIXL_zzz_Fn,
                                 VIXL_rri_Fn,
                                 VIXL_rl_Fn,
                                 VIXL_ri64_Fn,
                                 VIXL_zs_Fn,
                                 VIXL_LZLL_Fn,
                                 VIXL_zzu64_Fn,
                                 VIXL_vpz_Fn,
                                 VIXL_rru_Fn,
                                 VIXL_zMzi_Fn,
                                 VIXL_rrr_Fn,
                                 VIXL_rr_Fn,
                                 VIXL_r_Fn,
                                 VIXL__Fn,
                                 VIXL_lc_Fn,
                                 VIXL_i64c_Fn,
                                 VIXL_l_Fn,
                                 VIXL_i64_Fn,
                                 VIXL_ruu_Fn,
                                 VIXL_rruu_Fn,
                                 VIXL_vi_Fn,
                                 VIXL_vii_Fn,
                                 VIXL_i_Fn,
                                 VIXL_LML_Fn,
                                 VIXL_LZL_Fn,
                                 VIXL_T_Fn,
                                 VIXL_rrm_Fn,
                                 VIXL_rrrrm_Fn,
                                 VIXL_roFc_Fn,
                                 VIXL_rrc_Fn,
                                 VIXL_rprz_Fn,
                                 VIXL_vpvz_Fn,
                                 VIXL_zpzz_Fn,
                                 VIXL_vvi_Fn,
                                 VIXL_ro_Fn,
                                 VIXL_LZzi_Fn,
                                 VIXL_LZzu_Fn,
                                 VIXL_LZzz_Fn,
                                 VIXL_rii_Fn,
                                 VIXL_ri_Fn,
                                 VIXL_rpL_Fn,
                                 VIXL_zpz_Fn,
                                 VIXL_zZii_Fn,
                                 VIXL_zZi_Fn,
                                 VIXL_zMii_Fn,
                                 VIXL_zMi_Fn,
                                 VIXL_zMr_Fn,
                                 VIXL_zMv_Fn,
                                 VIXL_rrrc_Fn,
                                 VIXL_rc_Fn,
                                 VIXL_Dr_Fn,
                                 VIXL_zii_Fn,
                                 VIXL_z_Fn,
                                 VIXL_zi_Fn,
                                 VIXL_rL_Fn,
                                 VIXL_Bb_Fn,
                                 VIXL_vr_Fn,
                                 VIXL_zr_Fn,
                                 VIXL_zzu_Fn,
                                 VIXL_zu64_Fn,
                                 VIXL_vvvi_Fn,
                                 VIXL_zzzu_Fn,
                                 VIXL_rrru_Fn,
                                 VIXL_zMzd_Fn,
                                 VIXL_zMzzi_Fn,
                                 VIXL_vvFc_Fn,
                                 VIXL_vvd_Fn,
                                 VIXL_LZzd_Fn,
                                 VIXL_vvvii_Fn,
                                 VIXL_zzzii_Fn,
                                 VIXL_vd_Fn,
                                 VIXL_zMd_Fn,
                                 VIXL_zMf16_Fn,
                                 VIXL_vvvc_Fn,
                                 VIXL_rv_Fn,
                                 VIXL_rvi_Fn,
                                 VIXL_zd_Fn,
                                 VIXL_zf16_Fn,
                                 VIXL_zz_Fn,
                                 VIXL_vvvv_Fn,
                                 VIXL_zzzi_Fn,
                                 VIXL_vf_Fn,
                                 VIXL_vf16_Fn,
                                 VIXL_Ir_Fn,
                                 VIXL_zir_Fn,
                                 VIXL_zri_Fn,
                                 VIXL_zrr_Fn,
                                 VIXL_vivi_Fn,
                                 VIXL_vir_Fn,
                                 VIXL_zv_Fn,
                                 VIXL_rpz_Fn,
                                 VIXL_vm_Fn,
                                 VIXL_vvm_Fn,
                                 VIXL_vvvm_Fn,
                                 VIXL_vvvvm_Fn,
                                 VIXL_vim_Fn,
                                 VIXL_zZs_Fn,
                                 VIXL_vvim_Fn,
                                 VIXL_zzZs_Fn,
                                 VIXL_vvvim_Fn,
                                 VIXL_zzzZs_Fn,
                                 VIXL_vvvvim_Fn,
                                 VIXL_zzzzZs_Fn,
                                 VIXL_rm_Fn,
                                 VIXL_vi64_Fn,
                                 VIXL_pm_Fn,
                                 VIXL_ps_Fn,
                                 VIXL_zm_Fn,
                                 VIXL_rrrr_Fn,
                                 VIXL_ru64_Fn,
                                 VIXL_vu64Si_Fn,
                                 VIXL_vu64_Fn,
                                 VIXL_ru64i_Fn,
                                 VIXL_zZz_Fn,
                                 VIXL_rR_Fn,
                                 VIXL_Rr_Fn,
                                 VIXL_viSi_Fn,
                                 VIXL_L_Fn,
                                 VIXL_LpL_Fn,
                                 VIXL_Pps_Fn,
                                 VIXL_Pm_Fn,
                                 VIXL_im_Fn,
                                 VIXL_Pi64_Fn,
                                 VIXL_ii64_Fn,
                                 VIXL_pL_Fn,
                                 VIXL_Li_Fn,
                                 VIXL_LL_Fn,
                                 VIXL_LZ_Fn,
                                 VIXL_ruF_Fn,
                                 VIXL_vri_Fn,
                                 VIXL_LpLL_Fn,
                                 VIXL_rrii_Fn,
                                 VIXL_rLr_Fn,
                                 VIXL_zps_Fn,
                                 VIXL_zzps_Fn,
                                 VIXL_zzzps_Fn,
                                 VIXL_zzzzps_Fn,
                                 VIXL_rrrm_Fn,
                                 VIXL_iiii_Fn,
                                 VIXL_iiiir_Fn,
                                 VIXL_vvvvv_Fn,
                                 VIXL_vvvvvv_Fn,
                                 VIXL_rul_Fn,
                                 VIXL_rui64_Fn,
                                 VIXL_LLL_Fn,
                                 VIXL_Lrr_Fn>;

class TextAssembler : private Assembler {
 public:
  void Assemble(std::string instruction);
  bool GetError();
  bool GetLineError();

  // Functions allowing instruction prototype/mnemonic lookup.
  static bool MnemonicExists(std::string prototype, std::string mnemonic);
  static std::vector<std::string> GetPrototypes();

  // Necessary methods to expose some functionality of Assembler base class.
  using Assembler::bind;
  using Assembler::GetBuffer;
  using Assembler::FinalizeCode;
  using Assembler::GetCPUFeatures;
  vixl::internal::AssemblerBase* AsAssemblerBase();

 private:
  ErrorHandler eh_;
  InstructionParser ip_{MnemonicExists, GetPrototypes, &eh_};
  static const std::vector<std::string> str_prototypes_;

  // Maps have to be members if we want to have private inheritance from
  // Assembler class.
  static const std::map<std::string, AssemblerFn> vv_insts_;
  static const std::map<std::string, AssemblerFn> zMz_insts_;
  static const std::map<std::string, AssemblerFn> rro_insts_;
  static const std::map<std::string, AssemblerFn> vvv_insts_;
  static const std::map<std::string, AssemblerFn> zMzz_insts_;
  static const std::map<std::string, AssemblerFn> zzi_insts_;
  static const std::map<std::string, AssemblerFn> zzii_insts_;
  static const std::map<std::string, AssemblerFn> zzz_insts_;
  static const std::map<std::string, AssemblerFn> rri_insts_;
  static const std::map<std::string, AssemblerFn> rl_insts_;
  static const std::map<std::string, AssemblerFn> ri64_insts_;
  static const std::map<std::string, AssemblerFn> zs_insts_;
  static const std::map<std::string, AssemblerFn> LZLL_insts_;
  static const std::map<std::string, AssemblerFn> zzu64_insts_;
  static const std::map<std::string, AssemblerFn> vpz_insts_;
  static const std::map<std::string, AssemblerFn> rru_insts_;
  static const std::map<std::string, AssemblerFn> zMzi_insts_;
  static const std::map<std::string, AssemblerFn> rrr_insts_;
  static const std::map<std::string, AssemblerFn> rr_insts_;
  static const std::map<std::string, AssemblerFn> r_insts_;
  static const std::map<std::string, AssemblerFn> _insts_;
  static const std::map<std::string, AssemblerFn> lc_insts_;
  static const std::map<std::string, AssemblerFn> i64c_insts_;
  static const std::map<std::string, AssemblerFn> l_insts_;
  static const std::map<std::string, AssemblerFn> i64_insts_;
  static const std::map<std::string, AssemblerFn> ruu_insts_;
  static const std::map<std::string, AssemblerFn> rruu_insts_;
  static const std::map<std::string, AssemblerFn> vi_insts_;
  static const std::map<std::string, AssemblerFn> vii_insts_;
  static const std::map<std::string, AssemblerFn> i_insts_;
  static const std::map<std::string, AssemblerFn> LML_insts_;
  static const std::map<std::string, AssemblerFn> LZL_insts_;
  static const std::map<std::string, AssemblerFn> T_insts_;
  static const std::map<std::string, AssemblerFn> rrm_insts_;
  static const std::map<std::string, AssemblerFn> rrrrm_insts_;
  static const std::map<std::string, AssemblerFn> roFc_insts_;
  static const std::map<std::string, AssemblerFn> rrc_insts_;
  static const std::map<std::string, AssemblerFn> rprz_insts_;
  static const std::map<std::string, AssemblerFn> vpvz_insts_;
  static const std::map<std::string, AssemblerFn> zpzz_insts_;
  static const std::map<std::string, AssemblerFn> vvi_insts_;
  static const std::map<std::string, AssemblerFn> ro_insts_;
  static const std::map<std::string, AssemblerFn> LZzi_insts_;
  static const std::map<std::string, AssemblerFn> LZzu_insts_;
  static const std::map<std::string, AssemblerFn> LZzz_insts_;
  static const std::map<std::string, AssemblerFn> rii_insts_;
  static const std::map<std::string, AssemblerFn> ri_insts_;
  static const std::map<std::string, AssemblerFn> rpL_insts_;
  static const std::map<std::string, AssemblerFn> zpz_insts_;
  static const std::map<std::string, AssemblerFn> zZii_insts_;
  static const std::map<std::string, AssemblerFn> zZi_insts_;
  static const std::map<std::string, AssemblerFn> zMii_insts_;
  static const std::map<std::string, AssemblerFn> zMi_insts_;
  static const std::map<std::string, AssemblerFn> zMr_insts_;
  static const std::map<std::string, AssemblerFn> zMv_insts_;
  static const std::map<std::string, AssemblerFn> rrrc_insts_;
  static const std::map<std::string, AssemblerFn> rc_insts_;
  static const std::map<std::string, AssemblerFn> Dr_insts_;
  static const std::map<std::string, AssemblerFn> zii_insts_;
  static const std::map<std::string, AssemblerFn> z_insts_;
  static const std::map<std::string, AssemblerFn> zi_insts_;
  static const std::map<std::string, AssemblerFn> rL_insts_;
  static const std::map<std::string, AssemblerFn> Bb_insts_;
  static const std::map<std::string, AssemblerFn> vr_insts_;
  static const std::map<std::string, AssemblerFn> zr_insts_;
  static const std::map<std::string, AssemblerFn> zzu_insts_;
  static const std::map<std::string, AssemblerFn> zu64_insts_;
  static const std::map<std::string, AssemblerFn> vvvi_insts_;
  static const std::map<std::string, AssemblerFn> zzzu_insts_;
  static const std::map<std::string, AssemblerFn> rrru_insts_;
  static const std::map<std::string, AssemblerFn> zMzd_insts_;
  static const std::map<std::string, AssemblerFn> zMzzi_insts_;
  static const std::map<std::string, AssemblerFn> vvFc_insts_;
  static const std::map<std::string, AssemblerFn> vvd_insts_;
  static const std::map<std::string, AssemblerFn> LZzd_insts_;
  static const std::map<std::string, AssemblerFn> vvvii_insts_;
  static const std::map<std::string, AssemblerFn> zzzii_insts_;
  static const std::map<std::string, AssemblerFn> vd_insts_;
  static const std::map<std::string, AssemblerFn> zMd_insts_;
  static const std::map<std::string, AssemblerFn> zMf16_insts_;
  static const std::map<std::string, AssemblerFn> vvvc_insts_;
  static const std::map<std::string, AssemblerFn> rv_insts_;
  static const std::map<std::string, AssemblerFn> rvi_insts_;
  static const std::map<std::string, AssemblerFn> zd_insts_;
  static const std::map<std::string, AssemblerFn> zf16_insts_;
  static const std::map<std::string, AssemblerFn> zz_insts_;
  static const std::map<std::string, AssemblerFn> vvvv_insts_;
  static const std::map<std::string, AssemblerFn> zzzi_insts_;
  static const std::map<std::string, AssemblerFn> vf_insts_;
  static const std::map<std::string, AssemblerFn> vf16_insts_;
  static const std::map<std::string, AssemblerFn> Ir_insts_;
  static const std::map<std::string, AssemblerFn> zir_insts_;
  static const std::map<std::string, AssemblerFn> zri_insts_;
  static const std::map<std::string, AssemblerFn> zrr_insts_;
  static const std::map<std::string, AssemblerFn> vivi_insts_;
  static const std::map<std::string, AssemblerFn> vir_insts_;
  static const std::map<std::string, AssemblerFn> zv_insts_;
  static const std::map<std::string, AssemblerFn> rpz_insts_;
  static const std::map<std::string, AssemblerFn> vm_insts_;
  static const std::map<std::string, AssemblerFn> vvm_insts_;
  static const std::map<std::string, AssemblerFn> vvvm_insts_;
  static const std::map<std::string, AssemblerFn> vvvvm_insts_;
  static const std::map<std::string, AssemblerFn> vim_insts_;
  static const std::map<std::string, AssemblerFn> zZs_insts_;
  static const std::map<std::string, AssemblerFn> vvim_insts_;
  static const std::map<std::string, AssemblerFn> zzZs_insts_;
  static const std::map<std::string, AssemblerFn> vvvim_insts_;
  static const std::map<std::string, AssemblerFn> zzzZs_insts_;
  static const std::map<std::string, AssemblerFn> vvvvim_insts_;
  static const std::map<std::string, AssemblerFn> zzzzZs_insts_;
  static const std::map<std::string, AssemblerFn> rm_insts_;
  static const std::map<std::string, AssemblerFn> vi64_insts_;
  static const std::map<std::string, AssemblerFn> pm_insts_;
  static const std::map<std::string, AssemblerFn> ps_insts_;
  static const std::map<std::string, AssemblerFn> zm_insts_;
  static const std::map<std::string, AssemblerFn> rrrr_insts_;
  static const std::map<std::string, AssemblerFn> ru64_insts_;
  static const std::map<std::string, AssemblerFn> vu64Si_insts_;
  static const std::map<std::string, AssemblerFn> vu64_insts_;
  static const std::map<std::string, AssemblerFn> ru64i_insts_;
  static const std::map<std::string, AssemblerFn> zZz_insts_;
  static const std::map<std::string, AssemblerFn> rR_insts_;
  static const std::map<std::string, AssemblerFn> Rr_insts_;
  static const std::map<std::string, AssemblerFn> viSi_insts_;
  static const std::map<std::string, AssemblerFn> L_insts_;
  static const std::map<std::string, AssemblerFn> LpL_insts_;
  static const std::map<std::string, AssemblerFn> Pps_insts_;
  static const std::map<std::string, AssemblerFn> Pm_insts_;
  static const std::map<std::string, AssemblerFn> im_insts_;
  static const std::map<std::string, AssemblerFn> Pi64_insts_;
  static const std::map<std::string, AssemblerFn> ii64_insts_;
  static const std::map<std::string, AssemblerFn> pL_insts_;
  static const std::map<std::string, AssemblerFn> Li_insts_;
  static const std::map<std::string, AssemblerFn> LL_insts_;
  static const std::map<std::string, AssemblerFn> LZ_insts_;
  static const std::map<std::string, AssemblerFn> ruF_insts_;
  static const std::map<std::string, AssemblerFn> vri_insts_;
  static const std::map<std::string, AssemblerFn> LpLL_insts_;
  static const std::map<std::string, AssemblerFn> rrii_insts_;
  static const std::map<std::string, AssemblerFn> rLr_insts_;
  static const std::map<std::string, AssemblerFn> zps_insts_;
  static const std::map<std::string, AssemblerFn> zzps_insts_;
  static const std::map<std::string, AssemblerFn> zzzps_insts_;
  static const std::map<std::string, AssemblerFn> zzzzps_insts_;
  static const std::map<std::string, AssemblerFn> rrrm_insts_;
  static const std::map<std::string, AssemblerFn> iiii_insts_;
  static const std::map<std::string, AssemblerFn> iiiir_insts_;
  static const std::map<std::string, AssemblerFn> vvvvv_insts_;
  static const std::map<std::string, AssemblerFn> vvvvvv_insts_;
  static const std::map<std::string, AssemblerFn> rul_insts_;
  static const std::map<std::string, AssemblerFn> rui64_insts_;
  static const std::map<std::string, AssemblerFn> LLL_insts_;
  static const std::map<std::string, AssemblerFn> Lrr_insts_;
  static const std::map<std::string, std::map<std::string, AssemblerFn> >
      prototypes_;

  // Functions below are wrappers around Assembler methods, they are generated
  // in two cases.
  //
  // 1) For Assembler methods with default arguments. For example:
  //     - cntd(const Register& rd, int pattern = SVE_ALL, int multiplier = 1)
  //
  //    For the above function two other ones will be generated:
  //     - cntd(const Register& rd)
  //     - cntd(const Register& rd, int pattern)
  //
  //    Text Assembler holds pointers to functions and needs explicit
  //    signatures to refer to this functions omitting default arguments.
  //
  // 2) For Assembler methods that take base type argument governing few
  //    subtypes. For example:
  //   -movprfx(const ZRegister& zd, const PRegister& pg, const ZRegister& zn)
  //
  //    For the above function two other ones will be generated:
  //   -movprfx(const ZRegister& zd, const PRegisterM& pg, const ZRegister& zn)
  //   -movprfx(const ZRegister& zd, const PRegisterZ& pg, const ZRegister& zn)
  //
  //    PRegisterM and PRegisterZ are subtypes of the PRegister type.
  //    Text Assembler parses arguments to the exact types and refers to the
  //    function signatures with these types.

  using Assembler::add;
  void add(const ZRegister& zd, const ZRegister& zn, int imm8) {
    Assembler::add(zd, zn, imm8);
  }

  using Assembler::bic;
  void bic(const VRegister& vd, const int imm8) { Assembler::bic(vd, imm8); }

  using Assembler::brka;
  void brka(const PRegisterWithLaneSize& pd,
            const PRegisterM& pg,
            const PRegisterWithLaneSize& pn) {
    Assembler::brka(pd, pg, pn);
  }
  void brka(const PRegisterWithLaneSize& pd,
            const PRegisterZ& pg,
            const PRegisterWithLaneSize& pn) {
    Assembler::brka(pd, pg, pn);
  }

  using Assembler::brkb;
  void brkb(const PRegisterWithLaneSize& pd,
            const PRegisterM& pg,
            const PRegisterWithLaneSize& pn) {
    Assembler::brkb(pd, pg, pn);
  }
  void brkb(const PRegisterWithLaneSize& pd,
            const PRegisterZ& pg,
            const PRegisterWithLaneSize& pn) {
    Assembler::brkb(pd, pg, pn);
  }

  using Assembler::clrex;
  void clrex() { Assembler::clrex(); }

  using Assembler::cntb;
  void cntb(const Register& rd) { Assembler::cntb(rd); }
  void cntb(const Register& rd, int pattern) { Assembler::cntb(rd, pattern); }

  using Assembler::cntd;
  void cntd(const Register& rd) { Assembler::cntd(rd); }
  void cntd(const Register& rd, int pattern) { Assembler::cntd(rd, pattern); }

  using Assembler::cnth;
  void cnth(const Register& rd, int pattern) { Assembler::cnth(rd, pattern); }
  void cnth(const Register& rd) { Assembler::cnth(rd); }

  using Assembler::cntw;
  void cntw(const Register& rd) { Assembler::cntw(rd); }
  void cntw(const Register& rd, int pattern) { Assembler::cntw(rd, pattern); }

  using Assembler::cpy;
  void cpy(const ZRegister& zd, const PRegisterM& pg, int imm8) {
    Assembler::cpy(zd, pg, imm8);
  }
  void cpy(const ZRegister& zd, const PRegisterM& pg, int imm8, int shift) {
    Assembler::cpy(zd, pg, imm8, shift);
  }
  void cpy(const ZRegister& zd, const PRegisterZ& pg, int imm8, int shift) {
    Assembler::cpy(zd, pg, imm8, shift);
  }
  void cpy(const ZRegister& zd, const PRegisterZ& pg, int imm8) {
    Assembler::cpy(zd, pg, imm8);
  }

  using Assembler::decb;
  void decb(const Register& xdn, int pattern) { Assembler::decb(xdn, pattern); }
  void decb(const Register& xdn) { Assembler::decb(xdn); }

  using Assembler::decd;
  void decd(const ZRegister& zdn, int pattern) {
    Assembler::decd(zdn, pattern);
  }
  void decd(const Register& xdn, int pattern) { Assembler::decd(xdn, pattern); }
  void decd(const Register& xdn) { Assembler::decd(xdn); }
  void decd(const ZRegister& zdn) { Assembler::decd(zdn); }

  using Assembler::dech;
  void dech(const Register& xdn, int pattern) { Assembler::dech(xdn, pattern); }
  void dech(const Register& xdn) { Assembler::dech(xdn); }
  void dech(const ZRegister& zdn) { Assembler::dech(zdn); }
  void dech(const ZRegister& zdn, int pattern) {
    Assembler::dech(zdn, pattern);
  }

  using Assembler::decw;
  void decw(const ZRegister& zdn, int pattern) {
    Assembler::decw(zdn, pattern);
  }
  void decw(const Register& xdn, int pattern) { Assembler::decw(xdn, pattern); }
  void decw(const Register& xdn) { Assembler::decw(xdn); }
  void decw(const ZRegister& zdn) { Assembler::decw(zdn); }

  using Assembler::dup;
  void dup(const ZRegister& zd, int imm8) { Assembler::dup(zd, imm8); }

  using Assembler::fcvtzs;
  void fcvtzs(const VRegister& vd, const VRegister& vn) {
    Assembler::fcvtzs(vd, vn);
  }
  void fcvtzs(const Register& rd, const VRegister& vn) {
    Assembler::fcvtzs(rd, vn);
  }

  using Assembler::fcvtzu;
  void fcvtzu(const VRegister& vd, const VRegister& vn) {
    Assembler::fcvtzu(vd, vn);
  }
  void fcvtzu(const Register& rd, const VRegister& vn) {
    Assembler::fcvtzu(rd, vn);
  }

  using Assembler::incb;
  void incb(const Register& xdn, int pattern) { Assembler::incb(xdn, pattern); }
  void incb(const Register& xdn) { Assembler::incb(xdn); }

  using Assembler::incd;
  void incd(const ZRegister& zdn, int pattern) {
    Assembler::incd(zdn, pattern);
  }
  void incd(const Register& xdn, int pattern) { Assembler::incd(xdn, pattern); }
  void incd(const Register& xdn) { Assembler::incd(xdn); }
  void incd(const ZRegister& zdn) { Assembler::incd(zdn); }

  using Assembler::inch;
  void inch(const ZRegister& zdn, int pattern) {
    Assembler::inch(zdn, pattern);
  }
  void inch(const Register& xdn, int pattern) { Assembler::inch(xdn, pattern); }
  void inch(const Register& xdn) { Assembler::inch(xdn); }
  void inch(const ZRegister& zdn) { Assembler::inch(zdn); }

  using Assembler::incw;
  void incw(const Register& xdn, int pattern) { Assembler::incw(xdn, pattern); }
  void incw(const Register& xdn) { Assembler::incw(xdn); }
  void incw(const ZRegister& zdn) { Assembler::incw(zdn); }
  void incw(const ZRegister& zdn, int pattern) {
    Assembler::incw(zdn, pattern);
  }

  using Assembler::ldnp;
  void ldnp(const VRegister& rt, const VRegister& rt2, const MemOperand& src) {
    Assembler::ldnp(rt, rt2, src);
  }
  void ldnp(const Register& rt, const Register& rt2, const MemOperand& src) {
    Assembler::ldnp(rt, rt2, src);
  }

  using Assembler::ldp;
  void ldp(const VRegister& rt, const VRegister& rt2, const MemOperand& src) {
    Assembler::ldp(rt, rt2, src);
  }
  void ldp(const Register& rt, const Register& rt2, const MemOperand& src) {
    Assembler::ldp(rt, rt2, src);
  }

  using Assembler::ldr;
  void ldr(const ZRegister& rt, const MemOperand& src) {
    Assembler::ldr(rt, src);
  }
  void ldr(const VRegister& rt, const MemOperand& src) {
    Assembler::ldr(rt, src);
  }
  void ldr(const Register& rt, int64_t imm19) { Assembler::ldr(rt, imm19); }
  void ldr(const ZRegister& rt, const SVEMemOperand& addr) {
    Assembler::ldr(rt, addr);
  }
  void ldr(const PRegister& rt, const MemOperand& src) {
    Assembler::ldr(rt, src);
  }
  void ldr(const VRegister& rt, int64_t imm19) { Assembler::ldr(rt, imm19); }
  void ldr(const PRegister& rt, const SVEMemOperand& addr) {
    Assembler::ldr(rt, addr);
  }
  void ldr(const Register& rt, const MemOperand& src) {
    Assembler::ldr(rt, src);
  }

  using Assembler::ldrb;
  void ldrb(const Register& rt, const MemOperand& src) {
    Assembler::ldrb(rt, src);
  }

  using Assembler::ldrh;
  void ldrh(const Register& rt, const MemOperand& src) {
    Assembler::ldrh(rt, src);
  }

  using Assembler::ldrsb;
  void ldrsb(const Register& rt, const MemOperand& src) {
    Assembler::ldrsb(rt, src);
  }

  using Assembler::ldrsh;
  void ldrsh(const Register& rt, const MemOperand& src) {
    Assembler::ldrsh(rt, src);
  }

  using Assembler::ldrsw;
  void ldrsw(const Register& xt, const MemOperand& src) {
    Assembler::ldrsw(xt, src);
  }

  using Assembler::ldur;
  void ldur(const Register& rt, const MemOperand& src) {
    Assembler::ldur(rt, src);
  }
  void ldur(const VRegister& rt, const MemOperand& src) {
    Assembler::ldur(rt, src);
  }

  using Assembler::ldurb;
  void ldurb(const Register& rt, const MemOperand& src) {
    Assembler::ldurb(rt, src);
  }

  using Assembler::ldurh;
  void ldurh(const Register& rt, const MemOperand& src) {
    Assembler::ldurh(rt, src);
  }

  using Assembler::ldursb;
  void ldursb(const Register& rt, const MemOperand& src) {
    Assembler::ldursb(rt, src);
  }

  using Assembler::ldursh;
  void ldursh(const Register& rt, const MemOperand& src) {
    Assembler::ldursh(rt, src);
  }

  using Assembler::ldursw;
  void ldursw(const Register& xt, const MemOperand& src) {
    Assembler::ldursw(xt, src);
  }

  using Assembler::mov;
  void mov(const ZRegister& zd, const PRegisterM& pg, int imm8) {
    Assembler::mov(zd, pg, imm8);
  }
  void mov(const ZRegister& zd, const PRegisterM& pg, int imm8, int shift) {
    Assembler::mov(zd, pg, imm8, shift);
  }
  void mov(const ZRegister& zd, const PRegisterZ& pg, int imm8, int shift) {
    Assembler::mov(zd, pg, imm8, shift);
  }
  void mov(const ZRegister& zd, const PRegisterZ& pg, int imm8) {
    Assembler::mov(zd, pg, imm8);
  }

  using Assembler::movi;
  void movi(const VRegister& vd, const uint64_t imm) {
    Assembler::movi(vd, imm);
  }

  using Assembler::movk;
  void movk(const Register& rd, uint64_t imm) { Assembler::movk(rd, imm); }

  using Assembler::movn;
  void movn(const Register& rd, uint64_t imm) { Assembler::movn(rd, imm); }

  using Assembler::movprfx;
  void movprfx(const ZRegister& zd, const PRegisterM& pg, const ZRegister& zn) {
    Assembler::movprfx(zd, pg, zn);
  }
  void movprfx(const ZRegister& zd, const PRegisterZ& pg, const ZRegister& zn) {
    Assembler::movprfx(zd, pg, zn);
  }

  using Assembler::movz;
  void movz(const Register& rd, uint64_t imm) { Assembler::movz(rd, imm); }

  using Assembler::mvni;
  void mvni(const VRegister& vd, const int imm8) { Assembler::mvni(vd, imm8); }

  using Assembler::orr;
  void orr(const VRegister& vd, const int imm8) { Assembler::orr(vd, imm8); }

  using Assembler::prfm;
  void prfm(PrefetchOperation op, const MemOperand& addr) {
    Assembler::prfm(op, addr);
  }
  void prfm(int op, const MemOperand& addr) { Assembler::prfm(op, addr); }

  using Assembler::prfum;
  void prfum(PrefetchOperation op, const MemOperand& addr) {
    Assembler::prfum(op, addr);
  }
  void prfum(int op, const MemOperand& addr) { Assembler::prfum(op, addr); }

  using Assembler::ptrue;
  void ptrue(const PRegisterWithLaneSize& pd) { Assembler::ptrue(pd); }

  using Assembler::ptrues;
  void ptrues(const PRegisterWithLaneSize& pd) { Assembler::ptrues(pd); }

  using Assembler::ret;
  void ret() { Assembler::ret(); }

  using Assembler::scvtf;
  void scvtf(const VRegister& fd, const Register& rn) {
    Assembler::scvtf(fd, rn);
  }
  void scvtf(const VRegister& fd, const VRegister& vn) {
    Assembler::scvtf(fd, vn);
  }

  using Assembler::sqadd;
  void sqadd(const ZRegister& zd, const ZRegister& zn, int imm8) {
    Assembler::sqadd(zd, zn, imm8);
  }

  using Assembler::sqdecb;
  void sqdecb(const Register& rdn) { Assembler::sqdecb(rdn); }
  void sqdecb(const Register& rdn, int pattern) {
    Assembler::sqdecb(rdn, pattern);
  }

  using Assembler::sqdecd;
  void sqdecd(const Register& rdn) { Assembler::sqdecd(rdn); }
  void sqdecd(const Register& xd, const Register& wn) {
    Assembler::sqdecd(xd, wn);
  }
  void sqdecd(const Register& xd, const Register& wn, int pattern) {
    Assembler::sqdecd(xd, wn, pattern);
  }
  void sqdecd(const Register& rdn, int pattern) {
    Assembler::sqdecd(rdn, pattern);
  }
  void sqdecd(const ZRegister& zdn, int pattern) {
    Assembler::sqdecd(zdn, pattern);
  }
  void sqdecd(const ZRegister& zdn) { Assembler::sqdecd(zdn); }

  using Assembler::sqdech;
  void sqdech(const Register& rdn) { Assembler::sqdech(rdn); }
  void sqdech(const Register& xd, const Register& wn) {
    Assembler::sqdech(xd, wn);
  }
  void sqdech(const Register& xd, const Register& wn, int pattern) {
    Assembler::sqdech(xd, wn, pattern);
  }
  void sqdech(const Register& rdn, int pattern) {
    Assembler::sqdech(rdn, pattern);
  }
  void sqdech(const ZRegister& zdn, int pattern) {
    Assembler::sqdech(zdn, pattern);
  }
  void sqdech(const ZRegister& zdn) { Assembler::sqdech(zdn); }

  using Assembler::sqdecw;
  void sqdecw(const Register& rdn) { Assembler::sqdecw(rdn); }
  void sqdecw(const Register& xd, const Register& wn) {
    Assembler::sqdecw(xd, wn);
  }
  void sqdecw(const Register& xd, const Register& wn, int pattern) {
    Assembler::sqdecw(xd, wn, pattern);
  }
  void sqdecw(const Register& rdn, int pattern) {
    Assembler::sqdecw(rdn, pattern);
  }
  void sqdecw(const ZRegister& zdn, int pattern) {
    Assembler::sqdecw(zdn, pattern);
  }
  void sqdecw(const ZRegister& zdn) { Assembler::sqdecw(zdn); }

  using Assembler::sqincb;
  void sqincb(const Register& rdn) { Assembler::sqincb(rdn); }
  void sqincb(const Register& xd, const Register& wn, int pattern) {
    Assembler::sqincb(xd, wn, pattern);
  }
  void sqincb(const Register& rdn, int pattern) {
    Assembler::sqincb(rdn, pattern);
  }
  void sqincb(const Register& xd, const Register& wn) {
    Assembler::sqincb(xd, wn);
  }

  using Assembler::sqincd;
  void sqincd(const Register& rdn) { Assembler::sqincd(rdn); }
  void sqincd(const ZRegister& zdn) { Assembler::sqincd(zdn); }
  void sqincd(const Register& rdn, int pattern) {
    Assembler::sqincd(rdn, pattern);
  }
  void sqincd(const ZRegister& zdn, int pattern) {
    Assembler::sqincd(zdn, pattern);
  }

  using Assembler::sqinch;
  void sqinch(const Register& rdn) { Assembler::sqinch(rdn); }
  void sqinch(const Register& xd, const Register& wn) {
    Assembler::sqinch(xd, wn);
  }
  void sqinch(const Register& xd, const Register& wn, int pattern) {
    Assembler::sqinch(xd, wn, pattern);
  }
  void sqinch(const Register& rdn, int pattern) {
    Assembler::sqinch(rdn, pattern);
  }
  void sqinch(const ZRegister& zdn, int pattern) {
    Assembler::sqinch(zdn, pattern);
  }
  void sqinch(const ZRegister& zdn) { Assembler::sqinch(zdn); }

  using Assembler::sqincw;
  void sqincw(const Register& rdn) { Assembler::sqincw(rdn); }
  void sqincw(const Register& xd, const Register& wn) {
    Assembler::sqincw(xd, wn);
  }
  void sqincw(const Register& xd, const Register& wn, int pattern) {
    Assembler::sqincw(xd, wn, pattern);
  }
  void sqincw(const Register& rdn, int pattern) {
    Assembler::sqincw(rdn, pattern);
  }
  void sqincw(const ZRegister& zdn, int pattern) {
    Assembler::sqincw(zdn, pattern);
  }
  void sqincw(const ZRegister& zdn) { Assembler::sqincw(zdn); }

  using Assembler::sqsub;
  void sqsub(const ZRegister& zd, const ZRegister& zn, int imm8) {
    Assembler::sqsub(zd, zn, imm8);
  }

  using Assembler::stnp;
  void stnp(const VRegister& rt, const VRegister& rt2, const MemOperand& dst) {
    Assembler::stnp(rt, rt2, dst);
  }
  void stnp(const Register& rt, const Register& rt2, const MemOperand& dst) {
    Assembler::stnp(rt, rt2, dst);
  }

  using Assembler::stp;
  void stp(const VRegister& rt, const VRegister& rt2, const MemOperand& dst) {
    Assembler::stp(rt, rt2, dst);
  }
  void stp(const Register& rt, const Register& rt2, const MemOperand& dst) {
    Assembler::stp(rt, rt2, dst);
  }

  using Assembler::str;
  void str(const VRegister& rt, const MemOperand& dst) {
    Assembler::str(rt, dst);
  }
  void str(const ZRegister& rt, const SVEMemOperand& addr) {
    Assembler::str(rt, addr);
  }
  void str(const ZRegister& rt, const MemOperand& dst) {
    Assembler::str(rt, dst);
  }
  void str(const PRegister& rt, const MemOperand& dst) {
    Assembler::str(rt, dst);
  }
  void str(const PRegister& rt, const SVEMemOperand& addr) {
    Assembler::str(rt, addr);
  }
  void str(const Register& rt, const MemOperand& dst) {
    Assembler::str(rt, dst);
  }

  using Assembler::strb;
  void strb(const Register& rt, const MemOperand& dst) {
    Assembler::strb(rt, dst);
  }

  using Assembler::strh;
  void strh(const Register& rt, const MemOperand& dst) {
    Assembler::strh(rt, dst);
  }

  using Assembler::stur;
  void stur(const Register& rt, const MemOperand& src) {
    Assembler::stur(rt, src);
  }
  void stur(const VRegister& rt, const MemOperand& src) {
    Assembler::stur(rt, src);
  }

  using Assembler::sturb;
  void sturb(const Register& rt, const MemOperand& dst) {
    Assembler::sturb(rt, dst);
  }

  using Assembler::sturh;
  void sturh(const Register& rt, const MemOperand& dst) {
    Assembler::sturh(rt, dst);
  }

  using Assembler::sub;
  void sub(const ZRegister& zd, const ZRegister& zn, int imm8) {
    Assembler::sub(zd, zn, imm8);
  }

  using Assembler::subr;
  void subr(const ZRegister& zd, const ZRegister& zn, int imm8) {
    Assembler::subr(zd, zn, imm8);
  }

  using Assembler::sys;
  void sys(int op1, int crn, int crm, int op2) {
    Assembler::sys(op1, crn, crm, op2);
  }

  using Assembler::ucvtf;
  void ucvtf(const VRegister& fd, const Register& rn) {
    Assembler::ucvtf(fd, rn);
  }
  void ucvtf(const VRegister& fd, const VRegister& vn) {
    Assembler::ucvtf(fd, vn);
  }

  using Assembler::uqadd;
  void uqadd(const ZRegister& zd, const ZRegister& zn, int imm8) {
    Assembler::uqadd(zd, zn, imm8);
  }

  using Assembler::uqdecb;
  void uqdecb(const Register& rdn) { Assembler::uqdecb(rdn); }
  void uqdecb(const Register& rdn, int pattern) {
    Assembler::uqdecb(rdn, pattern);
  }

  using Assembler::uqdecd;
  void uqdecd(const Register& rdn) { Assembler::uqdecd(rdn); }
  void uqdecd(const ZRegister& zdn) { Assembler::uqdecd(zdn); }
  void uqdecd(const Register& rdn, int pattern) {
    Assembler::uqdecd(rdn, pattern);
  }
  void uqdecd(const ZRegister& zdn, int pattern) {
    Assembler::uqdecd(zdn, pattern);
  }

  using Assembler::uqdech;
  void uqdech(const Register& rdn) { Assembler::uqdech(rdn); }
  void uqdech(const ZRegister& zdn) { Assembler::uqdech(zdn); }
  void uqdech(const Register& rdn, int pattern) {
    Assembler::uqdech(rdn, pattern);
  }
  void uqdech(const ZRegister& zdn, int pattern) {
    Assembler::uqdech(zdn, pattern);
  }

  using Assembler::uqdecw;
  void uqdecw(const Register& rdn) { Assembler::uqdecw(rdn); }
  void uqdecw(const ZRegister& zdn) { Assembler::uqdecw(zdn); }
  void uqdecw(const Register& rdn, int pattern) {
    Assembler::uqdecw(rdn, pattern);
  }
  void uqdecw(const ZRegister& zdn, int pattern) {
    Assembler::uqdecw(zdn, pattern);
  }

  using Assembler::uqincb;
  void uqincb(const Register& rdn) { Assembler::uqincb(rdn); }
  void uqincb(const Register& rdn, int pattern) {
    Assembler::uqincb(rdn, pattern);
  }

  using Assembler::uqincd;
  void uqincd(const Register& rdn) { Assembler::uqincd(rdn); }
  void uqincd(const ZRegister& zdn) { Assembler::uqincd(zdn); }
  void uqincd(const Register& rdn, int pattern) {
    Assembler::uqincd(rdn, pattern);
  }
  void uqincd(const ZRegister& zdn, int pattern) {
    Assembler::uqincd(zdn, pattern);
  }

  using Assembler::uqinch;
  void uqinch(const Register& rdn) { Assembler::uqinch(rdn); }
  void uqinch(const ZRegister& zdn) { Assembler::uqinch(zdn); }
  void uqinch(const Register& rdn, int pattern) {
    Assembler::uqinch(rdn, pattern);
  }
  void uqinch(const ZRegister& zdn, int pattern) {
    Assembler::uqinch(zdn, pattern);
  }

  using Assembler::uqincw;
  void uqincw(const ZRegister& zdn, int pattern) {
    Assembler::uqincw(zdn, pattern);
  }
  void uqincw(const Register& rdn) { Assembler::uqincw(rdn); }
  void uqincw(const Register& rdn, int pattern) {
    Assembler::uqincw(rdn, pattern);
  }
  void uqincw(const ZRegister& zdn) { Assembler::uqincw(zdn); }

  using Assembler::uqsub;
  void uqsub(const ZRegister& zd, const ZRegister& zn, int imm8) {
    Assembler::uqsub(zd, zn, imm8);
  }
};

// Class used in std::visit to get current value held by std::variant.
class InstructionDispatcher {
 public:
  void operator()(std::monostate v) {
    std::cout << "No function with given signature !" << std::endl;
    (void)v;
  }

  void operator()(VIXL_vv_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]), std::get<VRegister>(args[1]));
  }
  void operator()(VIXL_zMz_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterM>(args[1]),
               std::get<ZRegister>(args[2]));
  }
  void operator()(VIXL_rro_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<Operand>(args[2]));
  }
  void operator()(VIXL_vvv_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<VRegister>(args[2]));
  }
  void operator()(VIXL_zMzz_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterM>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<ZRegister>(args[3]));
  }
  void operator()(VIXL_zzi_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<int>(args[2]));
  }
  void operator()(VIXL_zzii_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<int>(args[2]),
               std::get<int>(args[3]));
  }
  void operator()(VIXL_zzz_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<ZRegister>(args[2]));
  }
  void operator()(VIXL_rri_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<int>(args[2]));
  }
  void operator()(VIXL_rl_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]), std::get<Label*>(args[1]));
  }
  void operator()(VIXL_ri64_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]), std::get<int64_t>(args[1]));
  }
  void operator()(VIXL_zs_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]), std::get<SVEMemOperand>(args[1]));
  }
  void operator()(VIXL_LZLL_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegisterZ>(args[1]),
               std::get<PRegisterWithLaneSize>(args[2]),
               std::get<PRegisterWithLaneSize>(args[3]));
  }
  void operator()(VIXL_zzu64_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<uint64_t>(args[2]));
  }
  void operator()(VIXL_vpz_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<ZRegister>(args[2]));
  }
  void operator()(VIXL_rru_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<unsigned>(args[2]));
  }
  void operator()(VIXL_zMzi_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterM>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<int>(args[3]));
  }
  void operator()(VIXL_rrr_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<Register>(args[2]));
  }
  void operator()(VIXL_rr_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]), std::get<Register>(args[1]));
  }
  void operator()(VIXL_r_Fn f) { (tasm->*f)(std::get<Register>(args[0])); }
  void operator()(VIXL__Fn f) { (tasm->*f)(); }
  void operator()(VIXL_lc_Fn f) {
    (tasm->*f)(std::get<Label*>(args[0]), std::get<Condition>(args[1]));
  }
  void operator()(VIXL_i64c_Fn f) {
    (tasm->*f)(std::get<int64_t>(args[0]), std::get<Condition>(args[1]));
  }
  void operator()(VIXL_l_Fn f) { (tasm->*f)(std::get<Label*>(args[0])); }
  void operator()(VIXL_i64_Fn f) { (tasm->*f)(std::get<int64_t>(args[0])); }
  void operator()(VIXL_ruu_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<unsigned>(args[1]),
               std::get<unsigned>(args[2]));
  }
  void operator()(VIXL_rruu_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<unsigned>(args[2]),
               std::get<unsigned>(args[3]));
  }
  void operator()(VIXL_vi_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]), std::get<int>(args[1]));
  }
  void operator()(VIXL_vii_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<int>(args[1]),
               std::get<int>(args[2]));
  }
  void operator()(VIXL_i_Fn f) { (tasm->*f)(std::get<int>(args[0])); }
  void operator()(VIXL_LML_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegisterM>(args[1]),
               std::get<PRegisterWithLaneSize>(args[2]));
  }
  void operator()(VIXL_LZL_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegisterZ>(args[1]),
               std::get<PRegisterWithLaneSize>(args[2]));
  }
  void operator()(VIXL_T_Fn f) {
    (tasm->*f)(std::get<BranchTargetIdentifier>(args[0]));
  }
  void operator()(VIXL_rrm_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<MemOperand>(args[2]));
  }
  void operator()(VIXL_rrrrm_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<Register>(args[2]),
               std::get<Register>(args[3]),
               std::get<MemOperand>(args[4]));
  }
  void operator()(VIXL_roFc_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Operand>(args[1]),
               std::get<StatusFlags>(args[2]),
               std::get<Condition>(args[3]));
  }
  void operator()(VIXL_rrc_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<Condition>(args[2]));
  }
  void operator()(VIXL_rprz_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<Register>(args[2]),
               std::get<ZRegister>(args[3]));
  }
  void operator()(VIXL_vpvz_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<VRegister>(args[2]),
               std::get<ZRegister>(args[3]));
  }
  void operator()(VIXL_zpzz_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<ZRegister>(args[3]));
  }
  void operator()(VIXL_vvi_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<int>(args[2]));
  }
  void operator()(VIXL_ro_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]), std::get<Operand>(args[1]));
  }
  void operator()(VIXL_LZzi_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegisterZ>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<int>(args[3]));
  }
  void operator()(VIXL_LZzu_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegisterZ>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<unsigned>(args[3]));
  }
  void operator()(VIXL_LZzz_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegisterZ>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<ZRegister>(args[3]));
  }
  void operator()(VIXL_rii_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<int>(args[1]),
               std::get<int>(args[2]));
  }
  void operator()(VIXL_ri_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]), std::get<int>(args[1]));
  }
  void operator()(VIXL_rpL_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<PRegisterWithLaneSize>(args[2]));
  }
  void operator()(VIXL_zpz_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<ZRegister>(args[2]));
  }
  void operator()(VIXL_zZii_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterZ>(args[1]),
               std::get<int>(args[2]),
               std::get<int>(args[3]));
  }
  void operator()(VIXL_zZi_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterZ>(args[1]),
               std::get<int>(args[2]));
  }
  void operator()(VIXL_zMii_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterM>(args[1]),
               std::get<int>(args[2]),
               std::get<int>(args[3]));
  }
  void operator()(VIXL_zMi_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterM>(args[1]),
               std::get<int>(args[2]));
  }
  void operator()(VIXL_zMr_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterM>(args[1]),
               std::get<Register>(args[2]));
  }
  void operator()(VIXL_zMv_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterM>(args[1]),
               std::get<VRegister>(args[2]));
  }
  void operator()(VIXL_rrrc_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<Register>(args[2]),
               std::get<Condition>(args[3]));
  }
  void operator()(VIXL_rc_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]), std::get<Condition>(args[1]));
  }
  void operator()(VIXL_Dr_Fn f) {
    (tasm->*f)(std::get<DataCacheOp>(args[0]), std::get<Register>(args[1]));
  }
  void operator()(VIXL_zii_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<int>(args[1]),
               std::get<int>(args[2]));
  }
  void operator()(VIXL_z_Fn f) { (tasm->*f)(std::get<ZRegister>(args[0])); }
  void operator()(VIXL_zi_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]), std::get<int>(args[1]));
  }
  void operator()(VIXL_rL_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<PRegisterWithLaneSize>(args[1]));
  }
  void operator()(VIXL_Bb_Fn f) {
    (tasm->*f)(std::get<BarrierDomain>(args[0]),
               std::get<BarrierType>(args[1]));
  }
  void operator()(VIXL_vr_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]), std::get<Register>(args[1]));
  }
  void operator()(VIXL_zr_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]), std::get<Register>(args[1]));
  }
  void operator()(VIXL_zzu_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<unsigned>(args[2]));
  }
  void operator()(VIXL_zu64_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]), std::get<uint64_t>(args[1]));
  }
  void operator()(VIXL_vvvi_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<VRegister>(args[2]),
               std::get<int>(args[3]));
  }
  void operator()(VIXL_zzzu_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<unsigned>(args[3]));
  }
  void operator()(VIXL_rrru_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<Register>(args[2]),
               std::get<unsigned>(args[3]));
  }
  void operator()(VIXL_zMzd_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterM>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<double>(args[3]));
  }
  void operator()(VIXL_zMzzi_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterM>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<ZRegister>(args[3]),
               std::get<int>(args[4]));
  }
  void operator()(VIXL_vvFc_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<StatusFlags>(args[2]),
               std::get<Condition>(args[3]));
  }
  void operator()(VIXL_vvd_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<double>(args[2]));
  }
  void operator()(VIXL_LZzd_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegisterZ>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<double>(args[3]));
  }
  void operator()(VIXL_vvvii_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<VRegister>(args[2]),
               std::get<int>(args[3]),
               std::get<int>(args[4]));
  }
  void operator()(VIXL_zzzii_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<int>(args[3]),
               std::get<int>(args[4]));
  }
  void operator()(VIXL_vd_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]), std::get<double>(args[1]));
  }
  void operator()(VIXL_zMd_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterM>(args[1]),
               std::get<double>(args[2]));
  }
  void operator()(VIXL_zMf16_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterM>(args[1]),
               std::get<Float16>(args[2]));
  }
  void operator()(VIXL_vvvc_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<VRegister>(args[2]),
               std::get<Condition>(args[3]));
  }
  void operator()(VIXL_rv_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]), std::get<VRegister>(args[1]));
  }
  void operator()(VIXL_rvi_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<int>(args[2]));
  }
  void operator()(VIXL_zd_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]), std::get<double>(args[1]));
  }
  void operator()(VIXL_zf16_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]), std::get<Float16>(args[1]));
  }
  void operator()(VIXL_zz_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]), std::get<ZRegister>(args[1]));
  }
  void operator()(VIXL_vvvv_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<VRegister>(args[2]),
               std::get<VRegister>(args[3]));
  }
  void operator()(VIXL_zzzi_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<int>(args[3]));
  }
  void operator()(VIXL_vf_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]), std::get<float>(args[1]));
  }
  void operator()(VIXL_vf16_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]), std::get<Float16>(args[1]));
  }
  void operator()(VIXL_Ir_Fn f) {
    (tasm->*f)(std::get<InstructionCacheOp>(args[0]),
               std::get<Register>(args[1]));
  }
  void operator()(VIXL_zir_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<int>(args[1]),
               std::get<Register>(args[2]));
  }
  void operator()(VIXL_zri_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<Register>(args[1]),
               std::get<int>(args[2]));
  }
  void operator()(VIXL_zrr_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<Register>(args[1]),
               std::get<Register>(args[2]));
  }
  void operator()(VIXL_vivi_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<int>(args[1]),
               std::get<VRegister>(args[2]),
               std::get<int>(args[3]));
  }
  void operator()(VIXL_vir_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<int>(args[1]),
               std::get<Register>(args[2]));
  }
  void operator()(VIXL_zv_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]), std::get<VRegister>(args[1]));
  }
  void operator()(VIXL_rpz_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<ZRegister>(args[2]));
  }
  void operator()(VIXL_vm_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]), std::get<MemOperand>(args[1]));
  }
  void operator()(VIXL_vvm_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<MemOperand>(args[2]));
  }
  void operator()(VIXL_vvvm_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<VRegister>(args[2]),
               std::get<MemOperand>(args[3]));
  }
  void operator()(VIXL_vvvvm_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<VRegister>(args[2]),
               std::get<VRegister>(args[3]),
               std::get<MemOperand>(args[4]));
  }
  void operator()(VIXL_vim_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<int>(args[1]),
               std::get<MemOperand>(args[2]));
  }
  void operator()(VIXL_zZs_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterZ>(args[1]),
               std::get<SVEMemOperand>(args[2]));
  }
  void operator()(VIXL_vvim_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<int>(args[2]),
               std::get<MemOperand>(args[3]));
  }
  void operator()(VIXL_zzZs_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<PRegisterZ>(args[2]),
               std::get<SVEMemOperand>(args[3]));
  }
  void operator()(VIXL_vvvim_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<VRegister>(args[2]),
               std::get<int>(args[3]),
               std::get<MemOperand>(args[4]));
  }
  void operator()(VIXL_zzzZs_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<PRegisterZ>(args[3]),
               std::get<SVEMemOperand>(args[4]));
  }
  void operator()(VIXL_vvvvim_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<VRegister>(args[2]),
               std::get<VRegister>(args[3]),
               std::get<int>(args[4]),
               std::get<MemOperand>(args[5]));
  }
  void operator()(VIXL_zzzzZs_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<ZRegister>(args[3]),
               std::get<PRegisterZ>(args[4]),
               std::get<SVEMemOperand>(args[5]));
  }
  void operator()(VIXL_rm_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]), std::get<MemOperand>(args[1]));
  }
  void operator()(VIXL_vi64_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]), std::get<int64_t>(args[1]));
  }
  void operator()(VIXL_pm_Fn f) {
    (tasm->*f)(std::get<PRegister>(args[0]), std::get<MemOperand>(args[1]));
  }
  void operator()(VIXL_ps_Fn f) {
    (tasm->*f)(std::get<PRegister>(args[0]), std::get<SVEMemOperand>(args[1]));
  }
  void operator()(VIXL_zm_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]), std::get<MemOperand>(args[1]));
  }
  void operator()(VIXL_rrrr_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<Register>(args[2]),
               std::get<Register>(args[3]));
  }
  void operator()(VIXL_ru64_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]), std::get<uint64_t>(args[1]));
  }
  void operator()(VIXL_vu64Si_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<uint64_t>(args[1]),
               std::get<Shift>(args[2]),
               std::get<int>(args[3]));
  }
  void operator()(VIXL_vu64_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]), std::get<uint64_t>(args[1]));
  }
  void operator()(VIXL_ru64i_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<uint64_t>(args[1]),
               std::get<int>(args[2]));
  }
  void operator()(VIXL_zZz_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterZ>(args[1]),
               std::get<ZRegister>(args[2]));
  }
  void operator()(VIXL_rR_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]), std::get<SystemRegister>(args[1]));
  }
  void operator()(VIXL_Rr_Fn f) {
    (tasm->*f)(std::get<SystemRegister>(args[0]), std::get<Register>(args[1]));
  }
  void operator()(VIXL_viSi_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<int>(args[1]),
               std::get<Shift>(args[2]),
               std::get<int>(args[3]));
  }
  void operator()(VIXL_L_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]));
  }
  void operator()(VIXL_LpL_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<PRegisterWithLaneSize>(args[2]));
  }
  void operator()(VIXL_Pps_Fn f) {
    (tasm->*f)(std::get<PrefetchOperation>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<SVEMemOperand>(args[2]));
  }
  void operator()(VIXL_Pm_Fn f) {
    (tasm->*f)(std::get<PrefetchOperation>(args[0]),
               std::get<MemOperand>(args[1]));
  }
  void operator()(VIXL_im_Fn f) {
    (tasm->*f)(std::get<int>(args[0]), std::get<MemOperand>(args[1]));
  }
  void operator()(VIXL_Pi64_Fn f) {
    (tasm->*f)(std::get<PrefetchOperation>(args[0]),
               std::get<int64_t>(args[1]));
  }
  void operator()(VIXL_ii64_Fn f) {
    (tasm->*f)(std::get<int>(args[0]), std::get<int64_t>(args[1]));
  }
  void operator()(VIXL_pL_Fn f) {
    (tasm->*f)(std::get<PRegister>(args[0]),
               std::get<PRegisterWithLaneSize>(args[1]));
  }
  void operator()(VIXL_Li_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<int>(args[1]));
  }
  void operator()(VIXL_LL_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegisterWithLaneSize>(args[1]));
  }
  void operator()(VIXL_LZ_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegisterZ>(args[1]));
  }
  void operator()(VIXL_ruF_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<unsigned>(args[1]),
               std::get<StatusFlags>(args[2]));
  }
  void operator()(VIXL_vri_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<Register>(args[1]),
               std::get<int>(args[2]));
  }
  void operator()(VIXL_LpLL_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<PRegisterWithLaneSize>(args[2]),
               std::get<PRegisterWithLaneSize>(args[3]));
  }
  void operator()(VIXL_rrii_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<int>(args[2]),
               std::get<int>(args[3]));
  }
  void operator()(VIXL_rLr_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<PRegisterWithLaneSize>(args[1]),
               std::get<Register>(args[2]));
  }
  void operator()(VIXL_zps_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<SVEMemOperand>(args[2]));
  }
  void operator()(VIXL_zzps_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<PRegister>(args[2]),
               std::get<SVEMemOperand>(args[3]));
  }
  void operator()(VIXL_zzzps_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<PRegister>(args[3]),
               std::get<SVEMemOperand>(args[4]));
  }
  void operator()(VIXL_zzzzps_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<ZRegister>(args[1]),
               std::get<ZRegister>(args[2]),
               std::get<ZRegister>(args[3]),
               std::get<PRegister>(args[4]),
               std::get<SVEMemOperand>(args[5]));
  }
  void operator()(VIXL_rrrm_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<Register>(args[2]),
               std::get<MemOperand>(args[3]));
  }
  void operator()(VIXL_iiii_Fn f) {
    (tasm->*f)(std::get<int>(args[0]),
               std::get<int>(args[1]),
               std::get<int>(args[2]),
               std::get<int>(args[3]));
  }
  void operator()(VIXL_iiiir_Fn f) {
    (tasm->*f)(std::get<int>(args[0]),
               std::get<int>(args[1]),
               std::get<int>(args[2]),
               std::get<int>(args[3]),
               std::get<Register>(args[4]));
  }
  void operator()(VIXL_vvvvv_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<VRegister>(args[2]),
               std::get<VRegister>(args[3]),
               std::get<VRegister>(args[4]));
  }
  void operator()(VIXL_vvvvvv_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<VRegister>(args[2]),
               std::get<VRegister>(args[3]),
               std::get<VRegister>(args[4]),
               std::get<VRegister>(args[5]));
  }
  void operator()(VIXL_rul_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<unsigned>(args[1]),
               std::get<Label*>(args[2]));
  }
  void operator()(VIXL_rui64_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<unsigned>(args[1]),
               std::get<int64_t>(args[2]));
  }
  void operator()(VIXL_LLL_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegisterWithLaneSize>(args[1]),
               std::get<PRegisterWithLaneSize>(args[2]));
  }
  void operator()(VIXL_Lrr_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<Register>(args[1]),
               std::get<Register>(args[2]));
  }

  InstructionDispatcher(TextAssembler* tasm,
                        std::vector<Argument> args,
                        std::string mnemonic)
      : tasm(tasm), args(args), mnemonic(mnemonic) {}

 private:
  TextAssembler* tasm;
  std::vector<Argument> args;
  std::string mnemonic;
};
}
}
}  // namespace vixl::aarch64::tasm

#endif  // __cplusplus >= 201703L
#endif  // VIXL_TASM_ASSEMBLER_H