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

void Assembler::adr(CRegister cd, int64_t imm21) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // In A64, this encoding has different behaviour. See `adr(Register, ...)`.
  VIXL_ASSERT(GetISA() == ISA::C64);
  Emit(ADR | ImmPCRelAddress(imm21) | Rd(cd));
}

void Assembler::adr(CRegister cd, Label* label) {
  adr(cd, LinkAndGetByteOffsetTo(label));
}

void Assembler::adrdp(CRegister cd, int64_t imm20) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // In A64, this encoding has different behaviour. See `adrp(Register, ...)`.
  VIXL_ASSERT(GetISA() == ISA::C64);
  // We don't need to check `AllowPageOffsetDependentCode()` here because this
  // is based on DDC/c28, not PCC.
  Emit(ADRP | ImmC64RelAddressADRDP(imm20) | Rd(cd));
}

void Assembler::adrp(CRegister cd, int64_t imm20) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // In A64, this encoding has different behaviour. See `adrp(Register, ...)`.
  VIXL_ASSERT(GetISA() == ISA::C64);
  VIXL_ASSERT(AllowPageOffsetDependentCode());
  Emit(ADRP | ImmC64RelAddressADRP(imm20) | Rd(cd));
}

void Assembler::adrp(CRegister cd, Label* label) {
  VIXL_ASSERT(AllowPageOffsetDependentCode());
  adrp(cd, LinkAndGetPageOffsetTo(label));
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

void Assembler::blr(CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(BLR_c | Rn(cn));
}

void Assembler::blr(const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // The base register must be a capability.
  CRegister cn = addr.GetBaseCRegister();
  // The offset is signed, and scaled by 16 bytes.
  VIXL_ASSERT(addr.IsImmediateOffset());
  int64_t offset = addr.GetOffset();
  VIXL_ASSERT(IsMultiple<kCRegSizeInBytes>(offset));
  Emit(BLR_ci | RnSP(cn) | ImmField<19, 13>(offset / kCRegSizeInBytes));
}

void Assembler::blrr(CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(BLRR_c | Rn(cn));
}

void Assembler::blrs(CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(BLRS_c | Rn(cn));
}

void Assembler::blrs(CRegister cd, CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(cd.Is(c29));
  USE(cd);
  Emit(BLRS_c_c | Rn(cn) | Rm(cm));
}

void Assembler::br(CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(BR_c | Rn(cn));
}

void Assembler::br(const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // The base register must be a capability.
  CRegister cn = addr.GetBaseCRegister();
  // The offset is signed, and scaled by 16 bytes.
  VIXL_ASSERT(addr.IsImmediateOffset());
  int64_t offset = addr.GetOffset();
  VIXL_ASSERT(IsMultiple<kCRegSizeInBytes>(offset));
  Emit(BR_ci | RnSP(cn) | ImmField<19, 13>(offset / kCRegSizeInBytes));
}

void Assembler::brr(CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(BRR_c | Rn(cn));
}

void Assembler::brs(CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(BRS_c | Rn(cn));
}

void Assembler::brs(CRegister cd, CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(cd.Is(c29));
  USE(cd);
  Emit(BRS_c_c | Rn(cn) | Rm(cm));
}

void Assembler::build(CRegister cd, CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(BUILD_c_c | RdSP(cd) | RnSP(cn) | RmSP(cm));
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

void Assembler::chksld(CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(CHKSLD_c | RnSP(cn));
}

void Assembler::chkss(CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(CHKSS_cc | RnSP(cn) | RmSP(cm));
}

void Assembler::chkssu(CRegister cd, CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(CHKSSU_c_cc | Rd(cd) | RnSP(cn) | RmSP(cm));
}

void Assembler::chktgd(CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(CHKTGD_c | RnSP(cn));
}

void Assembler::clrperm(CRegister cd, CRegister cn, ClrpermImm perm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Instr perm_enc = ImmUnsignedField<15, 13>(static_cast<int>(perm));
  Emit(CLRPERM_c_ci | RdSP(cd) | RnSP(cn) | perm_enc);
}

void Assembler::clrperm(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(CLRPERM_c_cr | RdSP(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::clrtag(CRegister cd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(CLRTAG_c_c | RdSP(cd) | RnSP(cn));
}

void Assembler::cmp(CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  subs(xzr, cn, cm);
}

void Assembler::cpy(CRegister cd, CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(CPY_c_c | RdSP(cd) | RnSP(cn));
}

void Assembler::cpytype(CRegister cd, CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(CPYTYPE_c_c | Rd(cd) | Rn(cn) | Rm(cm));
}

void Assembler::cpyvalue(CRegister cd, CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(CPYVALUE_c_c | Rd(cd) | Rn(cn) | Rm(cm));
}

void Assembler::cseal(CRegister cd, CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(CSEAL_c_c | RdSP(cd) | RnSP(cn) | RmSP(cm));
}

void Assembler::csel(CRegister cd, CRegister cn, CRegister cm, Condition cond) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(CSEL_c_ci | Rd(cd) | Rn(cn) | Rm(cm) | Cond(cond));
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

void Assembler::ldapr(CRegister ct, const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kRCpc, CPUFeatures::kMorello));
  // There is no alt-base form of this instruction.
  VIXL_ASSERT(!addr.IsAltBase(GetISA()));
  VIXL_ASSERT(addr.IsPlainRegister());
  Emit(LDAPR_c_r | Rt(ct) | RnSP(addr.GetBase()));
}

void Assembler::ldar(CRegister ct, const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(addr.IsPlainRegister());
  if (addr.IsAltBase(GetISA())) {
    Emit(ALDAR_c_r | Rt(ct) | RnSP(addr.GetBase()));
  } else {
    Emit(LDAR_c_r | Rt(ct) | RnSP(addr.GetBase()));
  }
}

void Assembler::ldaxp(CRegister ct, CRegister ct2, const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // There is no alt-base form of this instruction.
  VIXL_ASSERT(!addr.IsAltBase(GetISA()));
  VIXL_ASSERT(addr.IsPlainRegister());
  Emit(LDAXP_c_r | Rt(ct) | Rt2(ct2) | RnSP(addr.GetBase()));
}

void Assembler::ldaxr(CRegister ct, const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // There is no alt-base form of this instruction.
  VIXL_ASSERT(!addr.IsAltBase(GetISA()));
  VIXL_ASSERT(addr.IsPlainRegister());
  Emit(LDAXR_c_r | Rt(ct) | RnSP(addr.GetBase()));
}

void Assembler::ldxr(CRegister ct, const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // There is no alt-base form of this instruction.
  VIXL_ASSERT(!addr.IsAltBase(GetISA()));
  VIXL_ASSERT(addr.IsPlainRegister());
  Emit(LDXR_c_r | Rt(ct) | RnSP(addr.GetBase()));
}

void Assembler::ldxp(CRegister ct, CRegister ct2, const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // There is no alt-base form of this instruction.
  VIXL_ASSERT(!addr.IsAltBase(GetISA()));
  VIXL_ASSERT(addr.IsPlainRegister());
  Emit(LDXP_c_r | Rt(ct) | Rt2(ct2) | RnSP(addr.GetBase()));
}

void Assembler::mrs(CRegister ct, CapabilitySystemRegister sysreg) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(CPUHas(sysreg));
  Emit(MRS_c_i | Rt(ct) | ImmSystemRegister(sysreg));
}

void Assembler::msr(CapabilitySystemRegister sysreg, CRegister ct) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(CPUHas(sysreg));
  Emit(MSR_c_i | Rt(ct) | ImmSystemRegister(sysreg));
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

void Assembler::ret(CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(RET_c | Rn(cn));
}

void Assembler::retr(CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(RETR_c | Rn(cn));
}

void Assembler::rets(CRegister cn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(RETS_c | Rn(cn));
}

void Assembler::rets(CRegister cd, CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(cd.Is(c29));
  USE(cd);
  Emit(RETS_c_c | Rn(cn) | Rm(cm));
}

void Assembler::rrlen(Register xd, Register xn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  VIXL_ASSERT(xn.IsX());
  Emit(RRLEN_r_r | Rd(xd) | Rn(xn));
}

void Assembler::rrmask(Register xd, Register xn) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xd.IsX());
  VIXL_ASSERT(xn.IsX());
  Emit(RRMASK_r_r | Rd(xd) | Rn(xn));
}

void Assembler::scbnds(CRegister cd, CRegister cn, Register xm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(xm.IsX());
  Emit(SCBNDS_c_cr | RdSP(cd) | RnSP(cn) | Rm(xm));
}

void Assembler::scbnds(CRegister cd, CRegister cn, uint64_t imm6) {
  // TODO: Should this be able to infer a shift, like `add`? Currently we only
  // do that in the MacroAssembler.
  scbnds(cd, cn, imm6, 0);
}

void Assembler::scbnds(CRegister cd, CRegister cn, uint64_t imm6, int shift) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT((shift == 0) || (shift == 4));
  Instr op = (shift == 0) ? SCBNDS_c_ci_c : SCBNDS_c_ci_s;
  Emit(op | RdSP(cd) | RnSP(cn) | ImmUnsignedField<20, 15>(imm6));
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

void Assembler::seal(CRegister cd, CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(SEAL_c_cc | Rd(cd) | Rn(cn) | Rm(cm));
}

void Assembler::seal(CRegister cd, CRegister cn, SealForm form) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Instr form_enc = ImmUnsignedField<14, 13>(static_cast<int>(form));
  Emit(SEAL_c_ci | Rd(cd) | Rn(cn) | form_enc);
}

void Assembler::stlr(CRegister ct, const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(addr.IsPlainRegister());
  if (addr.IsAltBase(GetISA())) {
    Emit(ASTLR_c_r | Rt(ct) | RnSP(addr.GetBase()));
  } else {
    Emit(STLR_c_r | Rt(ct) | RnSP(addr.GetBase()));
  }
}

void Assembler::stlxp(Register ws,
                      CRegister ct,
                      CRegister ct2,
                      const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // There is no alt-base form of this instruction.
  VIXL_ASSERT(!addr.IsAltBase(GetISA()));
  VIXL_ASSERT(addr.IsPlainRegister());
  Emit(STLXP_r_cr | Rs(ws) | Rt(ct) | Rt2(ct2) | RnSP(addr.GetBase()));
}

void Assembler::stlxr(Register ws, CRegister ct, const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // There is no alt-base form of this instruction.
  VIXL_ASSERT(!addr.IsAltBase(GetISA()));
  VIXL_ASSERT(addr.IsPlainRegister());
  Emit(STLXR_r_cr | Rs(ws) | Rt(ct) | RnSP(addr.GetBase()));
}

void Assembler::stxp(Register ws,
                     CRegister ct,
                     CRegister ct2,
                     const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // There is no alt-base form of this instruction.
  VIXL_ASSERT(!addr.IsAltBase(GetISA()));
  VIXL_ASSERT(addr.IsPlainRegister());
  Emit(STXP_r_cr | Rs(ws) | Rt(ct) | Rt2(ct2) | RnSP(addr.GetBase()));
}

void Assembler::stxr(Register ws, CRegister ct, const MemOperand& addr) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  // There is no alt-base form of this instruction.
  VIXL_ASSERT(!addr.IsAltBase(GetISA()));
  VIXL_ASSERT(addr.IsPlainRegister());
  Emit(STXR_r_cr | Rs(ws) | Rt(ct) | RnSP(addr.GetBase()));
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

void Assembler::unseal(CRegister cd, CRegister cn, CRegister cm) {
  VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  Emit(UNSEAL_c_cc | Rd(cd) | Rn(cn) | Rm(cm));
}

// Helpers and utilities.

Assembler::LoadStoreOpSet Assembler::LoadStoreOpSet::Ldr(CRegister rt) {
  LoadStoreOpSet set(rt.GetLaneSizeInBytesLog2());
  set.scaled_uint12_offset_op_ = LDR_c_rib;
  set.extended_register_op_ = LDR_c_rrb;
  set.unscaled_int9_offset_op_ = LDUR_c_ri;
  set.scaled_int9_pre_index_op_ = LDR_c_ribw;
  set.scaled_int9_post_index_op_ = LDR_c_riaw;
  return set;
}

Assembler::LoadStoreOpSet Assembler::LoadStoreOpSet::Str(CRegister rt) {
  LoadStoreOpSet set(rt.GetLaneSizeInBytesLog2());
  set.scaled_uint12_offset_op_ = STR_c_rib;
  set.extended_register_op_ = STR_c_rrb;
  set.unscaled_int9_offset_op_ = STUR_c_ri;
  set.scaled_int9_pre_index_op_ = STR_c_ribw;
  set.scaled_int9_post_index_op_ = STR_c_riaw;
  return set;
}

Assembler::LoadStoreOpSet Assembler::LoadStoreOpSet::Aldr(CPURegister rt) {
  VIXL_ASSERT(rt.IsScalar());
  LoadStoreOpSet set(rt.GetLaneSizeInBytesLog2());
  if (rt.IsCRegister()) {
    set.scaled_uint9_offset_op_ = ALDR_c_ri;
    set.extended_register_op_ = ALDR_c_rrb;
    set.unscaled_int9_offset_op_ = ALDUR_c_ri;
  } else if (rt.IsRegister()) {
    switch (rt.GetSizeInBits()) {
      case kXRegSize:
        set.scaled_uint9_offset_op_ = ALDR_r_ri_64;
        set.extended_register_op_ = ALDR_r_rrb_64;
        set.unscaled_int9_offset_op_ = ALDUR_r_ri_64;
        break;
      case kWRegSize:
        set.scaled_uint9_offset_op_ = ALDR_r_ri_32;
        set.extended_register_op_ = ALDR_r_rrb_32;
        set.unscaled_int9_offset_op_ = ALDUR_r_ri_32;
        break;
      default:
        VIXL_UNREACHABLE();
        break;
    }
  } else if (rt.IsVRegister()) {
    switch (rt.GetSizeInBits()) {
      case kQRegSize:
        set.unscaled_int9_offset_op_ = ALDUR_v_ri_q;
        break;
      case kDRegSize:
        set.extended_register_op_ = ALDR_v_rrb_d;
        set.unscaled_int9_offset_op_ = ALDUR_v_ri_d;
        break;
      case kSRegSize:
        set.extended_register_op_ = ALDR_v_rrb_s;
        set.unscaled_int9_offset_op_ = ALDUR_v_ri_s;
        break;
      case kHRegSize:
        set.unscaled_int9_offset_op_ = ALDUR_v_ri_h;
        break;
      case kBRegSize:
        set.unscaled_int9_offset_op_ = ALDUR_v_ri_b;
        break;
    }
  }
  return set;
}

Assembler::LoadStoreOpSet Assembler::LoadStoreOpSet::Astr(CPURegister rt) {
  VIXL_ASSERT(rt.IsScalar());
  LoadStoreOpSet set(rt.GetLaneSizeInBytesLog2());
  if (rt.IsCRegister()) {
    set.scaled_uint9_offset_op_ = ASTR_c_ri;
    set.extended_register_op_ = ASTR_c_rrb;
    set.unscaled_int9_offset_op_ = ASTUR_c_ri;
  } else if (rt.IsRegister()) {
    switch (rt.GetSizeInBits()) {
      case kXRegSize:
        set.scaled_uint9_offset_op_ = ASTR_r_ri_64;
        set.extended_register_op_ = ASTR_r_rrb_64;
        set.unscaled_int9_offset_op_ = ASTUR_r_ri_64;
        break;
      case kWRegSize:
        set.scaled_uint9_offset_op_ = ASTR_r_ri_32;
        set.extended_register_op_ = ASTR_r_rrb_32;
        set.unscaled_int9_offset_op_ = ASTUR_r_ri_32;
        break;
      default:
        VIXL_UNREACHABLE();
        break;
    }
  } else if (rt.IsVRegister()) {
    switch (rt.GetSizeInBits()) {
      case kQRegSize:
        set.unscaled_int9_offset_op_ = ASTUR_v_ri_q;
        break;
      case kDRegSize:
        set.extended_register_op_ = ASTR_v_rrb_d;
        set.unscaled_int9_offset_op_ = ASTUR_v_ri_d;
        break;
      case kSRegSize:
        set.extended_register_op_ = ASTR_v_rrb_s;
        set.unscaled_int9_offset_op_ = ASTUR_v_ri_s;
        break;
      case kHRegSize:
        set.unscaled_int9_offset_op_ = ASTUR_v_ri_h;
        break;
      case kBRegSize:
        set.unscaled_int9_offset_op_ = ASTUR_v_ri_b;
        break;
    }
  }
  return set;
}

Assembler::LoadStoreOpSet Assembler::LoadStoreOpSet::Aldrb(Register rt) {
  USE(rt);
  LoadStoreOpSet set(kBRegSizeInBytesLog2);
  set.scaled_uint9_offset_op_ = ALDRB_r_ri;
  set.extended_register_op_ = ALDRB_r_rrb;
  set.unscaled_int9_offset_op_ = ALDURB_r_ri;
  return set;
}

Assembler::LoadStoreOpSet Assembler::LoadStoreOpSet::Aldrsb(Register rt) {
  LoadStoreOpSet set(kBRegSizeInBytesLog2);
  switch (rt.GetSizeInBits()) {
    case kXRegSize:
      set.extended_register_op_ = ALDRSB_r_rrb_64;
      set.unscaled_int9_offset_op_ = ALDURSB_r_ri_64;
      break;
    case kWRegSize:
      set.extended_register_op_ = ALDRSB_r_rrb_32;
      set.unscaled_int9_offset_op_ = ALDURSB_r_ri_32;
      break;
  }
  return set;
}

Assembler::LoadStoreOpSet Assembler::LoadStoreOpSet::Astrb(Register rt) {
  USE(rt);
  LoadStoreOpSet set(kBRegSizeInBytesLog2);
  set.scaled_uint9_offset_op_ = ASTRB_r_ri;
  set.extended_register_op_ = ASTRB_r_rrb;
  set.unscaled_int9_offset_op_ = ASTURB_r_ri;
  return set;
}

Assembler::LoadStoreOpSet Assembler::LoadStoreOpSet::Aldrh(Register rt) {
  USE(rt);
  LoadStoreOpSet set(kHRegSizeInBytesLog2);
  set.extended_register_op_ = ALDRH_r_rrb;
  set.unscaled_int9_offset_op_ = ALDURH_r_ri;
  return set;
}

Assembler::LoadStoreOpSet Assembler::LoadStoreOpSet::Aldrsh(Register rt) {
  LoadStoreOpSet set(kHRegSizeInBytesLog2);
  switch (rt.GetSizeInBits()) {
    case kXRegSize:
      set.extended_register_op_ = ALDRSH_r_rrb_64;
      set.unscaled_int9_offset_op_ = ALDURSH_r_ri_64;
      break;
    case kWRegSize:
      set.extended_register_op_ = ALDRSH_r_rrb_32;
      set.unscaled_int9_offset_op_ = ALDURSH_r_ri_32;
      break;
  }
  return set;
}

Assembler::LoadStoreOpSet Assembler::LoadStoreOpSet::Astrh(Register rt) {
  USE(rt);
  LoadStoreOpSet set(kHRegSizeInBytesLog2);
  set.extended_register_op_ = ASTRH_r_rrb;
  set.unscaled_int9_offset_op_ = ASTURH_r_ri;
  return set;
}

Assembler::LoadStoreOpSet Assembler::LoadStoreOpSet::Aldrsw(Register rt) {
  USE(rt);
  LoadStoreOpSet set(kWRegSizeInBytesLog2);
  set.unscaled_int9_offset_op_ = ALDURSW_r_ri;
  return set;
}

Instr Assembler::LoadStoreOpSet::TryEncode(
    CPURegister rt,
    const MemOperand& addr,
    LoadStoreScalingOption option) const {
  VIXL_ASSERT(rt.IsScalar());
  Instr instr = TryEncodeMemOperand(addr, option);
  VIXL_ASSERT((instr == kUnsupported) || ((instr & Rt_mask) == 0));
  return instr | Rt(rt);
}

Instr Assembler::LoadStoreOpSet::TryEncodeMemOperand(
    const MemOperand& addr, LoadStoreScalingOption option) const {
  unsigned access_size_in_bytes_log_2 = GetAccessSizeInBytesLog2();
  unsigned access_size_in_bytes = GetAccessSizeInBytes();
  CPURegister rn = addr.GetBase();
  int64_t offset = addr.GetOffset();
  int64_t scaled_offset = offset / access_size_in_bytes;

  if (addr.IsImmediateOffset()) {
    // Select between "unsigned offset" (e.g. ldr) and "unscaled" (e.g. ldur).
    bool can_use_scaled_uint9 = HasScaledUint9OffsetOp() &&
                                (option != RequireUnscaledOffset) &&
                                IsScaledUint<9>(offset, access_size_in_bytes);
    bool can_use_scaled_uint12 = HasScaledUint12OffsetOp() &&
                                 (option != RequireUnscaledOffset) &&
                                 IsScaledUint<12>(offset, access_size_in_bytes);
    bool can_use_unscaled_int9 = HasUnscaledInt9OffsetOp() &&
                                 (option != RequireScaledOffset) &&
                                 IsInt9(offset);

    bool prefer_unscaled_offset =
        can_use_unscaled_int9 && (option == PreferUnscaledOffset);
    if (can_use_scaled_uint9 && !prefer_unscaled_offset) {
      return GetScaledUint9OffsetOp() | RnSP(rn) |
             ImmUnsignedField<20, 12>(scaled_offset);
    }
    if (can_use_scaled_uint12 && !prefer_unscaled_offset) {
      return GetScaledUint12OffsetOp() | RnSP(rn) |
             ImmUnsignedField<21, 10>(scaled_offset);
    }
    if (can_use_unscaled_int9) {
      return GetUnscaledInt9OffsetOp() | RnSP(rn) | ImmField<20, 12>(offset);
    }
  }

  // All remaining addressing modes are register-offset, pre-indexed or
  // post-indexed modes, so we cannot honour Require* options.
  VIXL_ASSERT((option != RequireUnscaledOffset) &&
              (option != RequireScaledOffset));

  if (HasExtendedRegisterOp() && addr.IsRegisterOffset()) {
    Register rm = addr.GetRegisterOffset();
    unsigned shift_amount = addr.GetShiftAmount();
    // We must have either a shift (LSL) or an extend (SXTW, UXTW, SXTX).
    Extend extend = addr.GetExtend();
    Shift shift = addr.GetShift();
    if ((shift == LSL) && (extend = NO_EXTEND)) {
      // LSL is encoded as UXTX. Note that UXTX cannot be specified explicitly.
      extend = UXTX;
      shift = NO_SHIFT;
    }
    if (!rm.IsSP() &&
        ((shift_amount == 0) || (shift_amount == access_size_in_bytes_log_2)) &&
        (shift == NO_SHIFT) && ((extend == UXTW) || (extend == UXTX) ||
                                (extend == SXTW) || (extend == SXTX))) {
      return GetExtendedRegisterOp() | RnSP(rn) | Rm(rm) | ExtendMode(extend) |
             ImmShiftLS((shift_amount > 0) ? 1 : 0);
    }
  }

  if (HasScaledInt9PreIndexOp() && addr.IsPreIndex() &&
      IsScaledInt<9>(offset, access_size_in_bytes)) {
    return GetScaledInt9PreIndexOp() | Assembler::RnSP(rn) |
           ImmField<20, 12>(scaled_offset);
  }

  if (HasScaledInt9PostIndexOp() && addr.IsPostIndex() &&
      IsScaledInt<9>(offset, access_size_in_bytes)) {
    return GetScaledInt9PostIndexOp() | Assembler::RnSP(rn) |
           ImmField<20, 12>(scaled_offset);
  }
  return kUnsupported;
}

Assembler::LoadStorePairOpSet Assembler::LoadStorePairOpSet::Ldp(
    CRegister rt, CRegister rt2) {
  USE(rt, rt2);
  LoadStorePairOpSet set;
  set.scaled_int7_offset_op_ = LDP_c_rib;
  set.scaled_int7_pre_index_op_ = LDP_c_ribw;
  set.scaled_int7_post_index_op_ = LDP_cc_riaw;
  return set;
}

Assembler::LoadStorePairOpSet Assembler::LoadStorePairOpSet::Stp(
    CRegister rt, CRegister rt2) {
  USE(rt, rt2);
  LoadStorePairOpSet set;
  set.scaled_int7_offset_op_ = STP_c_rib;
  set.scaled_int7_pre_index_op_ = STP_c_ribw;
  set.scaled_int7_post_index_op_ = STP_cc_riaw;
  return set;
}

Assembler::LoadStorePairOpSet Assembler::LoadStorePairOpSet::Ldnp(
    CRegister rt, CRegister rt2) {
  USE(rt, rt2);
  LoadStorePairOpSet set;
  set.scaled_int7_offset_op_ = LDNP_c_rib;
  return set;
}

Assembler::LoadStorePairOpSet Assembler::LoadStorePairOpSet::Stnp(
    CRegister rt, CRegister rt2) {
  USE(rt, rt2);
  LoadStorePairOpSet set;
  set.scaled_int7_offset_op_ = STNP_c_rib;
  return set;
}

Instr Assembler::LoadStorePairOpSet::TryEncode(CRegister ct,
                                               CRegister ct2,
                                               const MemOperand& addr) const {
  VIXL_ASSERT(AreSameFormat(ct, ct2));
  CPURegister rn = addr.GetBase();
  int64_t offset = addr.GetOffset();
  int64_t scaled_offset = offset / kCRegSizeInBytes;

  // All addressing modes are encoded in a similar way.
  if (IsScaledInt<7, kCRegSizeInBytes>(offset)) {
    Instr fields =
        RnSP(rn) | Rt(ct) | Rt2(ct2) | ImmField<21, 15>(scaled_offset);

    if (HasScaledInt7OffsetOp() && addr.IsImmediateOffset()) {
      return GetScaledInt7OffsetOp() | fields;
    }

    if (HasScaledInt7PreIndexOp() && addr.IsPreIndex()) {
      return GetScaledInt7PreIndexOp() | fields;
    }

    if (HasScaledInt7PostIndexOp() && addr.IsPostIndex()) {
      return GetScaledInt7PostIndexOp() | fields;
    }
  }
  return kUnsupported;
}

void Assembler::LoadStore(const CPURegister& rt,
                          const MemOperand& addr,
                          LoadStoreOpSet op_set,
                          LoadStoreScalingOption option) {
  VIXL_ASSERT(CPUHas(rt));
  if (addr.IsAltBase(GetISA())) VIXL_ASSERT(CPUHas(CPUFeatures::kMorello));
  VIXL_ASSERT(rt.IsScalar());
  // TODO: This helper is currently only used for Morello (capability or
  // alt-base) accesses, but it could work for all accesses.
  VIXL_ASSERT(rt.IsCRegister() || addr.IsAltBase(GetISA()));
  // Morello accesses are encoded similarly to base (X or W) transfers, so we
  // can reuse most encoding helpers, but there are two notable differences:
  //    1. The fixed encodings are different for each class. We cannot use
  //       the existing LoadStoreOp, etc.
  //    2. Pre- and post-indexing offsets are scaled by access size (but still
  //       signed). We treat this as a new addressing mode.

  if (op_set.CanEncode(rt, addr, option)) {
    Emit(op_set.Encode(rt, addr, option));
    return;
  }
  VIXL_ABORT();
}

void Assembler::LoadStorePair(const CRegister& rt,
                              const CRegister& rt2,
                              const MemOperand& addr,
                              LoadStorePairOpSet op_set) {
  // TODO: This helper is currently only used for capability accesses, but it
  // could work for all pair accesses.
  VIXL_ASSERT(CPUHas(rt, rt2));
  if (op_set.CanEncode(rt, rt2, addr)) {
    Emit(op_set.Encode(rt, rt2, addr));
    return;
  }
  VIXL_ABORT();
}

}  // namespace aarch64
}  // namespace vixl
