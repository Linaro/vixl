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

#include "assembler-aarch64.h"

namespace vixl {
namespace aarch64 {

void Assembler::add(CRegister cd, CRegister cn, const Operand& operand) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  if (operand.IsExtendedRegister()) {
    Register reg = operand.GetRegister();
    Extend ext = operand.GetExtend();
    unsigned shift = operand.GetShiftAmount();
    // The ISA specifies an X register, but we accept a W register (with an
    // appropriate extend mode) for consistency with the core `add`.
    if ((ext == UXTX) || (ext == SXTX)) VIXL_ASSERT(reg.IsX());
    if ((shift <= 4) && !reg.IsSP()) {
      Emit(ADD_c_cri | Rm(reg) | ExtendMode(ext) | ImmExtendShift(shift) |
           RdSP(cd) | RnSP(cn));
      return;
    }
  } else if (operand.IsImmediate()) {
    add(cd, cn, operand.GetImmediate());
    return;
  }
  VIXL_ABORT();
}

void Assembler::add(CRegister cd, CRegister cn, uint64_t imm12, int shift) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  if (IsImmAddSub(imm12, shift)) {
    Emit(ADD_c_cis | ImmAddSub(imm12, shift) | RdSP(cd) | RnSP(cn));
    return;
  }
  VIXL_ABORT();
}

void Assembler::add(CRegister cd, CRegister cn, uint64_t imm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  if (IsImmAddSub(imm)) {
    Emit(ADD_c_cis | ImmAddSub(imm) | RdSP(cd) | RnSP(cn));
    return;
  }
  VIXL_ABORT();
}

void Assembler::alignd(CRegister cd, CRegister cn, int imm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT((imm >= 0) && (imm < 64));
  Emit(ALIGND_c_ci | RdSP(cd) | RnSP(cn) | ImmUnsignedField<20, 15>(imm));
}

void Assembler::alignu(CRegister cd, CRegister cn, int imm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT((imm >= 0) && (imm < 64));
  Emit(ALIGNU_c_ci | RdSP(cd) | RnSP(cn) | ImmUnsignedField<20, 15>(imm));
}

void Assembler::bicflgs(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(BICFLGS_c_cr | RdSP(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::bicflgs(CRegister cd, CRegister cn, int imm8) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(BICFLGS_c_ci | RdSP(cd) | RnSP(cn) | ImmUnsignedField<20, 13>(imm8));
}

void Assembler::bx(ptrdiff_t offset_in_bytes) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(offset_in_bytes == kInstructionSize);
  USE(offset_in_bytes);
  Emit(BX);
}

void Assembler::bx(Label* label) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // Link the label. Binding the label won't update the instruction because it
  // only supports an offset of 4, but it will check that the ISA and offset are
  // correct.
  ptrdiff_t offset = LinkAndGetByteOffsetTo(label);
  USE(offset);
  if (label->IsBound()) {
    // This occurs if BindToOffset binds the label ahead of the branch.
    VIXL_ASSERT(offset == kInstructionSize);
  } else {
    VIXL_ASSERT(offset == 0);
  }
  // `bx` unconditionally switches ISA.
  label->EnsureISA(vixl::aarch64::ExchangeISA(GetISA()));
  bx(kInstructionSize);
}

void Assembler::cfhi(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(CFHI_r_c | Rd(xd) | RnSP(cn));
}

void Assembler::chkeq(CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(CHKEQ_cc | RnSP(cn) | Rm(cm));
}

void Assembler::chkss(CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(CHKSS_cc | RnSP(cn) | RmSP(cm));
}

void Assembler::clrperm(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(CLRPERM_c_cr | RdSP(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::cmp(CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  subs(xzr, cn, cm);
}

void Assembler::cthi(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(CTHI_c_cr | RdSP(cd) | Rn(cn) | Rm(xm));
}

void Assembler::cvt(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(CVT_c_cr | Rd(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::cvt(Register xd, CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(CVT_r_cc | Rd(xd) | RnSP(cn) | Rm(cm));
}

void Assembler::cvtd(CRegister cd, Register xn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xn.IsX());
  Emit(CVTD_c_r | Rd(cd) | Rn(xn));
}

void Assembler::cvtd(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(CVTD_r_c | Rd(xd) | RnSP(cn));
}

void Assembler::cvtdz(CRegister cd, Register xn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xn.IsX());
  Emit(CVTDZ_c_r | Rd(cd) | Rn(xn));
}

void Assembler::cvtp(CRegister cd, Register xn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xn.IsX());
  Emit(CVTP_c_r | Rd(cd) | Rn(xn));
}

void Assembler::cvtp(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(CVTP_r_c | Rd(xd) | RnSP(cn));
}

void Assembler::cvtpz(CRegister cd, Register xn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xn.IsX());
  Emit(CVTPZ_c_r | Rd(cd) | Rn(xn));
}

void Assembler::cvtz(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(CVTZ_c_cr | Rd(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::eorflgs(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(EORFLGS_c_cr | RdSP(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::eorflgs(CRegister cd, CRegister cn, int imm8) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(EORFLGS_c_ci | RdSP(cd) | RnSP(cn) | ImmUnsignedField<20, 13>(imm8));
}

void Assembler::gcbase(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(GCBASE_r_c | Rd(xd) | RnSP(cn));
}

void Assembler::gclen(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(GCLEN_r_c | Rd(xd) | RnSP(cn));
}

void Assembler::gclim(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(GCLIM_r_c | Rd(xd) | RnSP(cn));
}

void Assembler::gcflgs(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(GCFLGS_r_c | Rd(xd) | RnSP(cn));
}

void Assembler::gcoff(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(GCOFF_r_c | Rd(xd) | RnSP(cn));
}

void Assembler::gcperm(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(GCPERM_r_c | Rd(xd) | RnSP(cn));
}

void Assembler::gcseal(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(GCSEAL_r_c | Rd(xd) | RnSP(cn));
}

void Assembler::gctag(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(GCTAG | Rd(xd) | RnSP(cn));
}

void Assembler::gctype(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(GCTYPE_r_c | Rd(xd) | RnSP(cn));
}

void Assembler::gcvalue(Register xd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(GCVALUE_r_c | Rd(xd) | RnSP(cn));
}

void Assembler::orrflgs(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(ORRFLGS_c_cr | RdSP(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::orrflgs(CRegister cd, CRegister cn, int imm8) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(ORRFLGS_c_ci | RdSP(cd) | RnSP(cn) | ImmUnsignedField<20, 13>(imm8));
}

void Assembler::scbnds(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(SCBNDS_c_cr | RdSP(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::scbndse(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(SCBNDSE_c_cr | RdSP(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::scflgs(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(SCFLGS_c_cr | RdSP(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::scoff(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(SCOFF_c_cr | RdSP(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::scvalue(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(SCVALUE_c_cr | RdSP(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::sub(CRegister cd, CRegister cn, uint64_t imm12, int shift) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // Morello's `sub` only has an immediate form.
  if (IsImmAddSub(imm12, shift)) {
    Emit(SUB_c_cis | ImmAddSub(imm12, shift) | RnSP(cn) | RdSP(cd));
    return;
  }
  VIXL_ABORT();
}

void Assembler::sub(CRegister cd, CRegister cn, uint64_t imm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // Morello's `sub` only has an immediate form.
  if (IsImmAddSub(imm)) {
    Emit(SUB_c_cis | ImmAddSub(imm) | RnSP(cn) | RdSP(cd));
    return;
  }
  VIXL_ABORT();
}

void Assembler::subs(Register xd, CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  Emit(SUBS_r_cc | Rd(xd) | Rn(cn) | Rm(cm));
}

}  // namespace aarch64
}  // namespace vixl
