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

#include "macro-assembler-aarch64.h"

namespace vixl {
namespace aarch64 {

void MacroAssembler::AddSubHelper(AddSubHelperOption option,
                                  const ZRegister& zd,
                                  const ZRegister& zn,
                                  IntegerOperand imm) {
  VIXL_ASSERT(imm.FitsInLane(zd));

  // Simple, encodable cases.
  if (TrySingleAddSub(option, zd, zn, imm)) return;

  VIXL_ASSERT((option == kAddImmediate) || (option == kSubImmediate));
  bool add_imm = (option == kAddImmediate);

  // Try to translate Add(..., -imm) to Sub(..., imm) if we can encode it in one
  // instruction. Also interpret the immediate as signed, so we can convert
  // Add(zd.VnH(), zn.VnH(), 0xffff...) to Sub(..., 1), etc.
  IntegerOperand signed_imm(imm.AsIntN(zd.GetLaneSizeInBits()));
  if (signed_imm.IsNegative()) {
    AddSubHelperOption n_option = add_imm ? kSubImmediate : kAddImmediate;
    IntegerOperand n_imm(signed_imm.GetMagnitude());
    // IntegerOperand can represent -INT_MIN, so this is always safe.
    VIXL_ASSERT(n_imm.IsPositiveOrZero());
    if (TrySingleAddSub(n_option, zd, zn, n_imm)) return;
  }

  // Otherwise, fall back to dup + ADD_z_z/SUB_z_z.
  UseScratchRegisterScope temps(this);
  ZRegister scratch = temps.AcquireZ().WithLaneSize(zn.GetLaneSizeInBits());
  Dup(scratch, imm);

  SingleEmissionCheckScope guard(this);
  if (add_imm) {
    add(zd, zn, scratch);
  } else {
    sub(zd, zn, scratch);
  }
}

bool MacroAssembler::TrySingleAddSub(AddSubHelperOption option,
                                     const ZRegister& zd,
                                     const ZRegister& zn,
                                     IntegerOperand imm) {
  VIXL_ASSERT(imm.FitsInLane(zd));

  int imm8;
  int shift = -1;
  if (imm.TryEncodeAsShiftedUintNForLane<8, 0>(zd, &imm8, &shift) ||
      imm.TryEncodeAsShiftedUintNForLane<8, 8>(zd, &imm8, &shift)) {
    MovprfxHelperScope guard(this, zd, zn);
    switch (option) {
      case kAddImmediate:
        add(zd, zd, imm8, shift);
        return true;
      case kSubImmediate:
        sub(zd, zd, imm8, shift);
        return true;
    }
  }
  return false;
}

void MacroAssembler::IntWideImmHelper(IntWideImmFn imm_fn,
                                      SVEArithPredicatedFn reg_macro,
                                      const ZRegister& zd,
                                      const ZRegister& zn,
                                      IntegerOperand imm,
                                      bool is_signed) {
  if (is_signed) {
    // E.g. MUL_z_zi, SMIN_z_zi, SMAX_z_zi
    if (imm.IsInt8()) {
      MovprfxHelperScope guard(this, zd, zn);
      (this->*imm_fn)(zd, zd, imm.AsInt8());
      return;
    }
  } else {
    // E.g. UMIN_z_zi, UMAX_z_zi
    if (imm.IsUint8()) {
      MovprfxHelperScope guard(this, zd, zn);
      (this->*imm_fn)(zd, zd, imm.AsUint8());
      return;
    }
  }

  UseScratchRegisterScope temps(this);
  PRegister pg = temps.AcquireGoverningP();
  Ptrue(pg.WithSameLaneSizeAs(zd));

  // Try to re-use zd if we can, so we can avoid a movprfx.
  ZRegister scratch =
      zd.Aliases(zn) ? temps.AcquireZ().WithLaneSize(zn.GetLaneSizeInBits())
                     : zd;
  Dup(scratch, imm);

  // The vector-form macro for commutative operations will swap the arguments to
  // avoid movprfx, if necessary.
  (this->*reg_macro)(zd, pg.Merging(), zn, scratch);
}

void MacroAssembler::Mul(const ZRegister& zd,
                         const ZRegister& zn,
                         IntegerOperand imm) {
  VIXL_ASSERT(allow_macro_instructions_);
  IntWideImmFn imm_fn = &Assembler::mul;
  SVEArithPredicatedFn reg_fn = &MacroAssembler::Mul;
  IntWideImmHelper(imm_fn, reg_fn, zd, zn, imm, true);
}

void MacroAssembler::Smin(const ZRegister& zd,
                          const ZRegister& zn,
                          IntegerOperand imm) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(imm.FitsInSignedLane(zd));
  IntWideImmFn imm_fn = &Assembler::smin;
  SVEArithPredicatedFn reg_fn = &MacroAssembler::Smin;
  IntWideImmHelper(imm_fn, reg_fn, zd, zn, imm, true);
}

void MacroAssembler::Smax(const ZRegister& zd,
                          const ZRegister& zn,
                          IntegerOperand imm) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(imm.FitsInSignedLane(zd));
  IntWideImmFn imm_fn = &Assembler::smax;
  SVEArithPredicatedFn reg_fn = &MacroAssembler::Smax;
  IntWideImmHelper(imm_fn, reg_fn, zd, zn, imm, true);
}

void MacroAssembler::Umax(const ZRegister& zd,
                          const ZRegister& zn,
                          IntegerOperand imm) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(imm.FitsInUnsignedLane(zd));
  IntWideImmFn imm_fn = &Assembler::umax;
  SVEArithPredicatedFn reg_fn = &MacroAssembler::Umax;
  IntWideImmHelper(imm_fn, reg_fn, zd, zn, imm, false);
}

void MacroAssembler::Umin(const ZRegister& zd,
                          const ZRegister& zn,
                          IntegerOperand imm) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(imm.FitsInUnsignedLane(zd));
  IntWideImmFn imm_fn = &Assembler::umin;
  SVEArithPredicatedFn reg_fn = &MacroAssembler::Umin;
  IntWideImmHelper(imm_fn, reg_fn, zd, zn, imm, false);
}

void MacroAssembler::Addpl(const Register& xd,
                           const Register& xn,
                           int64_t multiplier) {
  VIXL_ASSERT(allow_macro_instructions_);

  // This macro relies on `Rdvl` to handle some out-of-range cases. Check that
  // `VL * multiplier` cannot overflow, for any possible value of VL.
  VIXL_ASSERT(multiplier <= (INT64_MAX / kZRegMaxSizeInBytes));
  VIXL_ASSERT(multiplier >= (INT64_MIN / kZRegMaxSizeInBytes));

  if (xd.IsZero()) return;
  if (xn.IsZero() && xd.IsSP()) {
    // TODO: This operation doesn't make much sense, but we could support it
    // with a scratch register if necessary.
    VIXL_UNIMPLEMENTED();
  }

  // Handling xzr requires an extra move, so defer it until later so we can try
  // to use `rdvl` instead (via `Addvl`).
  if (IsInt6(multiplier) && !xn.IsZero()) {
    SingleEmissionCheckScope guard(this);
    addpl(xd, xn, static_cast<int>(multiplier));
    return;
  }

  // If `multiplier` is a multiple of 8, we can use `Addvl` instead.
  if ((multiplier % kZRegBitsPerPRegBit) == 0) {
    Addvl(xd, xn, multiplier / kZRegBitsPerPRegBit);
    return;
  }

  if (IsInt6(multiplier)) {
    VIXL_ASSERT(xn.IsZero());  // Other cases were handled with `addpl`.
    // There is no simple `rdpl` instruction, and `addpl` cannot accept xzr, so
    // materialise a zero.
    MacroEmissionCheckScope guard(this);
    movz(xd, 0);
    addpl(xd, xd, static_cast<int>(multiplier));
    return;
  }

  // TODO: Some probable cases result in rather long sequences. For example,
  // `Addpl(sp, sp, 33)` requires five instructions, even though it's only just
  // outside the encodable range. We should look for ways to cover such cases
  // without drastically increasing the complexity of this logic.

  // For other cases, calculate xn + (PL * multiplier) using discrete
  // instructions. This requires two scratch registers in the general case, so
  // try to re-use the destination as a scratch register.
  UseScratchRegisterScope temps(this);
  temps.Include(xd);
  temps.Exclude(xn);

  Register scratch = temps.AcquireX();
  // Because there is no `rdpl`, so we have to calculate PL from VL. We can't
  // scale the multiplier because (we already know) it isn't a multiple of 8.
  Rdvl(scratch, multiplier);

  MacroEmissionCheckScope guard(this);
  if (xn.IsZero()) {
    asr(xd, scratch, kZRegBitsPerPRegBitLog2);
  } else if (xd.IsSP() || xn.IsSP()) {
    // TODO: MacroAssembler::Add should be able to handle this.
    asr(scratch, scratch, kZRegBitsPerPRegBitLog2);
    add(xd, xn, scratch);
  } else {
    add(xd, xn, Operand(scratch, ASR, kZRegBitsPerPRegBitLog2));
  }
}

void MacroAssembler::Addvl(const Register& xd,
                           const Register& xn,
                           int64_t multiplier) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(xd.IsX());
  VIXL_ASSERT(xn.IsX());

  // Check that `VL * multiplier` cannot overflow, for any possible value of VL.
  VIXL_ASSERT(multiplier <= (INT64_MAX / kZRegMaxSizeInBytes));
  VIXL_ASSERT(multiplier >= (INT64_MIN / kZRegMaxSizeInBytes));

  if (xd.IsZero()) return;
  if (xn.IsZero() && xd.IsSP()) {
    // TODO: This operation doesn't make much sense, but we could support it
    // with a scratch register if necessary. `rdvl` cannot write into `sp`.
    VIXL_UNIMPLEMENTED();
  }

  if (IsInt6(multiplier)) {
    SingleEmissionCheckScope guard(this);
    if (xn.IsZero()) {
      rdvl(xd, static_cast<int>(multiplier));
    } else {
      addvl(xd, xn, static_cast<int>(multiplier));
    }
    return;
  }

  // TODO: Some probable cases result in rather long sequences. For example,
  // `Addvl(sp, sp, 42)` requires four instructions, even though it's only just
  // outside the encodable range. We should look for ways to cover such cases
  // without drastically increasing the complexity of this logic.

  // For other cases, calculate xn + (VL * multiplier) using discrete
  // instructions. This requires two scratch registers in the general case, so
  // we try to re-use the destination as a scratch register.
  UseScratchRegisterScope temps(this);
  temps.Include(xd);
  temps.Exclude(xn);

  Register a = temps.AcquireX();
  Mov(a, multiplier);

  MacroEmissionCheckScope guard(this);
  Register b = temps.AcquireX();
  rdvl(b, 1);
  if (xn.IsZero()) {
    mul(xd, a, b);
  } else if (xd.IsSP() || xn.IsSP()) {
    mul(a, a, b);
    add(xd, xn, a);
  } else {
    madd(xd, a, b, xn);
  }
}

void MacroAssembler::CalculateSVEAddress(const Register& xd,
                                         const SVEMemOperand& addr,
                                         int vl_divisor_log2) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(!addr.IsScatterGather());
  VIXL_ASSERT(xd.IsX());

  // The lower bound is where a whole Z register is accessed.
  VIXL_ASSERT(vl_divisor_log2 >= 0);
  // The upper bound is for P register accesses, and for instructions like
  // "st1b { z0.d } [...]", where one byte is accessed for every D-sized lane.
  VIXL_ASSERT(static_cast<unsigned>(vl_divisor_log2) <=
              kZRegBitsPerPRegBitLog2);

  SVEOffsetModifier mod = addr.GetOffsetModifier();
  Register base = addr.GetScalarBase();

  if (addr.IsEquivalentToScalar()) {
    // For example:
    //   [x0]
    //   [x0, #0]
    //   [x0, xzr, LSL 2]
    Mov(xd, base);
  } else if (addr.IsScalarPlusImmediate()) {
    // For example:
    //   [x0, #42]
    //   [x0, #42, MUL VL]
    int64_t offset = addr.GetImmediateOffset();
    VIXL_ASSERT(offset != 0);  // Handled by IsEquivalentToScalar.
    if (addr.IsMulVl()) {
      int vl_divisor = 1 << vl_divisor_log2;
      // For all possible values of vl_divisor, we can simply use `Addpl`. This
      // will select `addvl` if necessary.
      VIXL_ASSERT((kZRegBitsPerPRegBit % vl_divisor) == 0);
      Addpl(xd, base, offset * (kZRegBitsPerPRegBit / vl_divisor));
    } else {
      // IsScalarPlusImmediate() ensures that no other modifiers can occur.
      VIXL_ASSERT(mod == NO_SVE_OFFSET_MODIFIER);
      Add(xd, base, offset);
    }
  } else if (addr.IsScalarPlusScalar()) {
    // For example:
    //   [x0, x1]
    //   [x0, x1, LSL #4]
    Register offset = addr.GetScalarOffset();
    VIXL_ASSERT(!offset.IsZero());  // Handled by IsEquivalentToScalar.
    if (mod == SVE_LSL) {
      Add(xd, base, Operand(offset, LSL, addr.GetShiftAmount()));
    } else {
      // IsScalarPlusScalar() ensures that no other modifiers can occur.
      VIXL_ASSERT(mod == NO_SVE_OFFSET_MODIFIER);
      Add(xd, base, offset);
    }
  } else {
    // All other forms are scatter-gather addresses, which cannot be evaluated
    // into an X register.
    VIXL_UNREACHABLE();
  }
}

void MacroAssembler::Adr(const ZRegister& zd, const SVEMemOperand& addr) {
  USE(zd);
  USE(addr);
  // TODO: Handle all scatter-gather addressing modes, including the
  // vector-plus-vector mode used by `adr` itself.
  VIXL_UNIMPLEMENTED();
}

void MacroAssembler::Cpy(const ZRegister& zd,
                         const PRegister& pg,
                         IntegerOperand imm) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(imm.FitsInLane(zd));
  int imm8;
  int shift;
  if (imm.TryEncodeAsShiftedIntNForLane<8, 0>(zd, &imm8, &shift) ||
      imm.TryEncodeAsShiftedIntNForLane<8, 8>(zd, &imm8, &shift)) {
    SingleEmissionCheckScope guard(this);
    cpy(zd, pg, imm8, shift);
    return;
  }

  // The fallbacks rely on `cpy` variants that only support merging predication.
  // If zeroing predication was requested, zero the destination first.
  if (pg.IsZeroing()) {
    SingleEmissionCheckScope guard(this);
    dup(zd, 0);
  }
  PRegisterM pg_m = pg.Merging();

  // Try to encode the immediate using fcpy.
  VIXL_ASSERT(imm.FitsInLane(zd));
  if (zd.GetLaneSizeInBits() >= kHRegSize) {
    double fp_imm = 0.0;
    switch (zd.GetLaneSizeInBits()) {
      case kHRegSize:
        fp_imm =
            FPToDouble(RawbitsToFloat16(imm.AsUint16()), kIgnoreDefaultNaN);
        break;
      case kSRegSize:
        fp_imm = RawbitsToFloat(imm.AsUint32());
        break;
      case kDRegSize:
        fp_imm = RawbitsToDouble(imm.AsUint64());
        break;
      default:
        VIXL_UNREACHABLE();
        break;
    }
    // IsImmFP64 is equivalent to IsImmFP<n> for the same arithmetic value, so
    // we can use IsImmFP64 for all lane sizes.
    if (IsImmFP64(fp_imm)) {
      SingleEmissionCheckScope guard(this);
      fcpy(zd, pg_m, fp_imm);
      return;
    }
  }

  // Fall back to using a scratch register.
  UseScratchRegisterScope temps(this);
  Register scratch = temps.AcquireRegisterToHoldLane(zd);
  Mov(scratch, imm);

  SingleEmissionCheckScope guard(this);
  cpy(zd, pg_m, scratch);
}

// TODO: We implement Fcpy (amongst other things) for all FP types because it
// allows us to preserve user-specified NaNs. We should come up with some
// FPImmediate type to abstract this, and avoid all the duplication below (and
// elsewhere).

void MacroAssembler::Fcpy(const ZRegister& zd,
                          const PRegisterM& pg,
                          double imm) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(pg.IsMerging());

  if (IsImmFP64(imm)) {
    SingleEmissionCheckScope guard(this);
    fcpy(zd, pg, imm);
    return;
  }

  // As a fall-back, cast the immediate to the required lane size, and try to
  // encode the bit pattern using `Cpy`.
  Cpy(zd, pg, FPToRawbitsWithSize(zd.GetLaneSizeInBits(), imm));
}

void MacroAssembler::Fcpy(const ZRegister& zd,
                          const PRegisterM& pg,
                          float imm) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(pg.IsMerging());

  if (IsImmFP32(imm)) {
    SingleEmissionCheckScope guard(this);
    fcpy(zd, pg, imm);
    return;
  }

  // As a fall-back, cast the immediate to the required lane size, and try to
  // encode the bit pattern using `Cpy`.
  Cpy(zd, pg, FPToRawbitsWithSize(zd.GetLaneSizeInBits(), imm));
}

void MacroAssembler::Fcpy(const ZRegister& zd,
                          const PRegisterM& pg,
                          Float16 imm) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(pg.IsMerging());

  if (IsImmFP16(imm)) {
    SingleEmissionCheckScope guard(this);
    fcpy(zd, pg, imm);
    return;
  }

  // As a fall-back, cast the immediate to the required lane size, and try to
  // encode the bit pattern using `Cpy`.
  Cpy(zd, pg, FPToRawbitsWithSize(zd.GetLaneSizeInBits(), imm));
}

void MacroAssembler::Dup(const ZRegister& zd, IntegerOperand imm) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(imm.FitsInLane(zd));
  unsigned lane_size = zd.GetLaneSizeInBits();
  int imm8;
  int shift;
  if (imm.TryEncodeAsShiftedIntNForLane<8, 0>(zd, &imm8, &shift) ||
      imm.TryEncodeAsShiftedIntNForLane<8, 8>(zd, &imm8, &shift)) {
    SingleEmissionCheckScope guard(this);
    dup(zd, imm8, shift);
  } else if (IsImmLogical(imm.AsUintN(lane_size), lane_size)) {
    SingleEmissionCheckScope guard(this);
    dupm(zd, imm.AsUintN(lane_size));
  } else {
    UseScratchRegisterScope temps(this);
    Register scratch = temps.AcquireRegisterToHoldLane(zd);
    Mov(scratch, imm);

    SingleEmissionCheckScope guard(this);
    dup(zd, scratch);
  }
}

void MacroAssembler::NoncommutativeArithmeticHelper(
    const ZRegister& zd,
    const PRegisterM& pg,
    const ZRegister& zn,
    const ZRegister& zm,
    SVEArithPredicatedFn fn,
    SVEArithPredicatedFn rev_fn) {
  if (zd.Aliases(zn)) {
    // E.g. zd = zd / zm
    SingleEmissionCheckScope guard(this);
    (this->*fn)(zd, pg, zn, zm);
  } else if (zd.Aliases(zm)) {
    // E.g. zd = zn / zd
    SingleEmissionCheckScope guard(this);
    (this->*rev_fn)(zd, pg, zm, zn);
  } else {
    // E.g. zd = zn / zm
    MovprfxHelperScope guard(this, zd, pg, zn);
    (this->*fn)(zd, pg, zd, zm);
  }
}

void MacroAssembler::Fdiv(const ZRegister& zd,
                          const PRegisterM& pg,
                          const ZRegister& zn,
                          const ZRegister& zm) {
  VIXL_ASSERT(allow_macro_instructions_);
  NoncommutativeArithmeticHelper(zd,
                                 pg,
                                 zn,
                                 zm,
                                 static_cast<SVEArithPredicatedFn>(
                                     &Assembler::fdiv),
                                 static_cast<SVEArithPredicatedFn>(
                                     &Assembler::fdivr));
}

void MacroAssembler::Fdup(const ZRegister& zd, double imm) {
  VIXL_ASSERT(allow_macro_instructions_);

  switch (zd.GetLaneSizeInBits()) {
    case kHRegSize:
      Fdup(zd, Float16(imm));
      break;
    case kSRegSize:
      Fdup(zd, static_cast<float>(imm));
      break;
    case kDRegSize:
      if (IsImmFP64(imm)) {
        SingleEmissionCheckScope guard(this);
        fdup(zd, imm);
      } else {
        Dup(zd, DoubleToRawbits(imm));
      }
      break;
  }
}

void MacroAssembler::Fdup(const ZRegister& zd, float imm) {
  VIXL_ASSERT(allow_macro_instructions_);

  switch (zd.GetLaneSizeInBits()) {
    case kHRegSize:
      Fdup(zd, Float16(imm));
      break;
    case kSRegSize:
      if (IsImmFP32(imm)) {
        SingleEmissionCheckScope guard(this);
        fdup(zd, imm);
      } else {
        Dup(zd, FloatToRawbits(imm));
      }
      break;
    case kDRegSize:
      Fdup(zd, static_cast<double>(imm));
      break;
  }
}

void MacroAssembler::Fdup(const ZRegister& zd, Float16 imm) {
  VIXL_ASSERT(allow_macro_instructions_);

  switch (zd.GetLaneSizeInBits()) {
    case kHRegSize:
      if (IsImmFP16(imm)) {
        SingleEmissionCheckScope guard(this);
        fdup(zd, imm);
      } else {
        Dup(zd, Float16ToRawbits(imm));
      }
      break;
    case kSRegSize:
      Fdup(zd, FPToFloat(imm, kIgnoreDefaultNaN));
      break;
    case kDRegSize:
      Fdup(zd, FPToDouble(imm, kIgnoreDefaultNaN));
      break;
  }
}

void MacroAssembler::Index(const ZRegister& zd,
                           const Operand& start,
                           const Operand& step) {
  class IndexOperand : public Operand {
   public:
    static IndexOperand Prepare(MacroAssembler* masm,
                                UseScratchRegisterScope* temps,
                                const Operand& op,
                                const ZRegister& zd) {
      // Look for encodable immediates.
      int imm;
      if (op.IsImmediate()) {
        if (IntegerOperand(op).TryEncodeAsIntNForLane<5>(zd, &imm)) {
          return IndexOperand(imm);
        }
        Register scratch = temps->AcquireRegisterToHoldLane(zd);
        masm->Mov(scratch, op);
        return IndexOperand(scratch);
      } else {
        // Plain registers can be encoded directly.
        VIXL_ASSERT(op.IsPlainRegister());
        return IndexOperand(op.GetRegister());
      }
    }

    int GetImm5() const {
      int64_t imm = GetImmediate();
      VIXL_ASSERT(IsInt5(imm));
      return static_cast<int>(imm);
    }

   private:
    explicit IndexOperand(const Register& reg) : Operand(reg) {}
    explicit IndexOperand(int64_t imm) : Operand(imm) {}
  };

  UseScratchRegisterScope temps(this);
  IndexOperand start_enc = IndexOperand::Prepare(this, &temps, start, zd);
  IndexOperand step_enc = IndexOperand::Prepare(this, &temps, step, zd);

  SingleEmissionCheckScope guard(this);
  if (start_enc.IsImmediate()) {
    if (step_enc.IsImmediate()) {
      index(zd, start_enc.GetImm5(), step_enc.GetImm5());
    } else {
      index(zd, start_enc.GetImm5(), step_enc.GetRegister());
    }
  } else {
    if (step_enc.IsImmediate()) {
      index(zd, start_enc.GetRegister(), step_enc.GetImm5());
    } else {
      index(zd, start_enc.GetRegister(), step_enc.GetRegister());
    }
  }
}

void MacroAssembler::Insr(const ZRegister& zdn, IntegerOperand imm) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(imm.FitsInLane(zdn));

  if (imm.IsZero()) {
    SingleEmissionCheckScope guard(this);
    insr(zdn, xzr);
    return;
  }

  UseScratchRegisterScope temps(this);
  Register scratch = temps.AcquireRegisterToHoldLane(zdn);

  // TODO: There are many cases where we could optimise immediates, such as by
  // detecting repeating patterns or FP immediates. We should optimise and
  // abstract this for use in other SVE mov-immediate-like macros.
  Mov(scratch, imm);

  SingleEmissionCheckScope guard(this);
  insr(zdn, scratch);
}

void MacroAssembler::Mla(const ZRegister& zd,
                         const PRegisterM& pg,
                         const ZRegister& za,
                         const ZRegister& zn,
                         const ZRegister& zm) {
  VIXL_ASSERT(allow_macro_instructions_);
  if (zd.Aliases(za)) {
    // zda = zda + (zn * zm)
    SingleEmissionCheckScope guard(this);
    mla(zd, pg, zn, zm);
  } else if (zd.Aliases(zn)) {
    // zdn = za + (zdn * zm)
    SingleEmissionCheckScope guard(this);
    mad(zd, pg, zm, za);
  } else if (zd.Aliases(zm)) {
    // Multiplication is commutative, so we can swap zn and zm.
    // zdm = za + (zdm * zn)
    SingleEmissionCheckScope guard(this);
    mad(zd, pg, zn, za);
  } else {
    // zd = za + (zn * zm)
    ExactAssemblyScope guard(this, 2 * kInstructionSize);
    movprfx(zd, pg, za);
    mla(zd, pg, zn, zm);
  }
}

void MacroAssembler::Mls(const ZRegister& zd,
                         const PRegisterM& pg,
                         const ZRegister& za,
                         const ZRegister& zn,
                         const ZRegister& zm) {
  VIXL_ASSERT(allow_macro_instructions_);
  if (zd.Aliases(za)) {
    // zda = zda - (zn * zm)
    SingleEmissionCheckScope guard(this);
    mls(zd, pg, zn, zm);
  } else if (zd.Aliases(zn)) {
    // zdn = za - (zdn * zm)
    SingleEmissionCheckScope guard(this);
    msb(zd, pg, zm, za);
  } else if (zd.Aliases(zm)) {
    // Multiplication is commutative, so we can swap zn and zm.
    // zdm = za - (zdm * zn)
    SingleEmissionCheckScope guard(this);
    msb(zd, pg, zn, za);
  } else {
    // zd = za - (zn * zm)
    ExactAssemblyScope guard(this, 2 * kInstructionSize);
    movprfx(zd, pg, za);
    mls(zd, pg, zn, zm);
  }
}

void MacroAssembler::CompareHelper(Condition cond,
                                   const PRegisterWithLaneSize& pd,
                                   const PRegisterZ& pg,
                                   const ZRegister& zn,
                                   IntegerOperand imm) {
  UseScratchRegisterScope temps(this);
  ZRegister zm = temps.AcquireZ().WithLaneSize(zn.GetLaneSizeInBits());
  Dup(zm, imm);
  SingleEmissionCheckScope guard(this);
  cmp(cond, pd, pg, zn, zm);
}

void MacroAssembler::Pfirst(const PRegisterWithLaneSize& pd,
                            const PRegister& pg,
                            const PRegisterWithLaneSize& pn) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(pd.IsLaneSizeB());
  VIXL_ASSERT(pn.IsLaneSizeB());
  if (pd.Is(pn)) {
    SingleEmissionCheckScope guard(this);
    pfirst(pd, pg, pn);
  } else {
    UseScratchRegisterScope temps(this);
    PRegister temp_pg = pg;
    if (pd.Aliases(pg)) {
      temp_pg = temps.AcquireP();
      Mov(temp_pg.VnB(), pg.VnB());
    }
    Mov(pd, pn);
    SingleEmissionCheckScope guard(this);
    pfirst(pd, temp_pg, pd);
  }
}

void MacroAssembler::Pnext(const PRegisterWithLaneSize& pd,
                           const PRegister& pg,
                           const PRegisterWithLaneSize& pn) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(AreSameFormat(pd, pn));
  if (pd.Is(pn)) {
    SingleEmissionCheckScope guard(this);
    pnext(pd, pg, pn);
  } else {
    UseScratchRegisterScope temps(this);
    PRegister temp_pg = pg;
    if (pd.Aliases(pg)) {
      temp_pg = temps.AcquireP();
      Mov(temp_pg.VnB(), pg.VnB());
    }
    Mov(pd, pn);
    SingleEmissionCheckScope guard(this);
    pnext(pd, temp_pg, pd);
  }
}

void MacroAssembler::Ptrue(const PRegisterWithLaneSize& pd,
                           SVEPredicateConstraint pattern,
                           FlagsUpdate s) {
  VIXL_ASSERT(allow_macro_instructions_);
  switch (s) {
    case LeaveFlags:
      Ptrue(pd, pattern);
      return;
    case SetFlags:
      Ptrues(pd, pattern);
      return;
  }
  VIXL_UNREACHABLE();
}

void MacroAssembler::Sdiv(const ZRegister& zd,
                          const PRegisterM& pg,
                          const ZRegister& zn,
                          const ZRegister& zm) {
  VIXL_ASSERT(allow_macro_instructions_);
  NoncommutativeArithmeticHelper(zd,
                                 pg,
                                 zn,
                                 zm,
                                 static_cast<SVEArithPredicatedFn>(
                                     &Assembler::sdiv),
                                 static_cast<SVEArithPredicatedFn>(
                                     &Assembler::sdivr));
}

void MacroAssembler::Sub(const ZRegister& zd,
                         IntegerOperand imm,
                         const ZRegister& zm) {
  VIXL_ASSERT(allow_macro_instructions_);

  int imm8;
  int shift = -1;
  if (imm.TryEncodeAsShiftedUintNForLane<8, 0>(zd, &imm8, &shift) ||
      imm.TryEncodeAsShiftedUintNForLane<8, 8>(zd, &imm8, &shift)) {
    MovprfxHelperScope guard(this, zd, zm);
    subr(zd, zd, imm8, shift);
  } else {
    UseScratchRegisterScope temps(this);
    ZRegister scratch = temps.AcquireZ().WithLaneSize(zm.GetLaneSizeInBits());
    Dup(scratch, imm);

    SingleEmissionCheckScope guard(this);
    sub(zd, scratch, zm);
  }
}

void MacroAssembler::Sub(const ZRegister& zd,
                         const PRegisterM& pg,
                         const ZRegister& zn,
                         const ZRegister& zm) {
  VIXL_ASSERT(allow_macro_instructions_);
  NoncommutativeArithmeticHelper(zd,
                                 pg,
                                 zn,
                                 zm,
                                 static_cast<SVEArithPredicatedFn>(
                                     &Assembler::sub),
                                 static_cast<SVEArithPredicatedFn>(
                                     &Assembler::subr));
}

void MacroAssembler::Udiv(const ZRegister& zd,
                          const PRegisterM& pg,
                          const ZRegister& zn,
                          const ZRegister& zm) {
  VIXL_ASSERT(allow_macro_instructions_);
  NoncommutativeArithmeticHelper(zd,
                                 pg,
                                 zn,
                                 zm,
                                 static_cast<SVEArithPredicatedFn>(
                                     &Assembler::udiv),
                                 static_cast<SVEArithPredicatedFn>(
                                     &Assembler::udivr));
}

void MacroAssembler::SVELoadStoreScalarImmHelper(const CPURegister& rt,
                                                 const SVEMemOperand& addr,
                                                 SVELoadStoreFn fn) {
  VIXL_ASSERT(allow_macro_instructions_);
  VIXL_ASSERT(rt.IsZRegister() || rt.IsPRegister());

  if (addr.IsScalar() ||
      (addr.IsScalarPlusImmediate() && IsInt9(addr.GetImmediateOffset()) &&
       addr.IsMulVl())) {
    SingleEmissionCheckScope guard(this);
    (this->*fn)(rt, addr);
    return;
  }

  if (addr.IsEquivalentToScalar()) {
    SingleEmissionCheckScope guard(this);
    (this->*fn)(rt, SVEMemOperand(addr.GetScalarBase()));
    return;
  }

  UseScratchRegisterScope temps(this);
  Register scratch = temps.AcquireX();
  CalculateSVEAddress(scratch, addr, rt);
  SingleEmissionCheckScope guard(this);
  (this->*fn)(rt, SVEMemOperand(scratch));
}

// E.g. ld1b/st1b
typedef void (Assembler::*SVELoad1Fn)(const ZRegister& zt,
                                      const PRegisterZ& pg,
                                      const SVEMemOperand& addr);
typedef void (Assembler::*SVEStore1Fn)(const ZRegister& zt,
                                       const PRegister& pg,
                                       const SVEMemOperand& addr);

template <typename Tg, typename Tf>
void MacroAssembler::SVELoadStore1Helper(int msize_in_bytes_log2,
                                         const ZRegister& zt,
                                         const Tg& pg,
                                         const SVEMemOperand& addr,
                                         Tf fn) {
  if (addr.IsScalar() ||
      (addr.IsScalarPlusScalar() && !addr.GetScalarOffset().IsZero() &&
       addr.IsEquivalentToLSL(msize_in_bytes_log2)) ||
      (addr.IsScalarPlusImmediate() && IsInt4(addr.GetImmediateOffset()) &&
       addr.IsMulVl())) {
    SingleEmissionCheckScope guard(this);
    (this->*fn)(zt, pg, addr);
    return;
  }

  if (addr.IsEquivalentToScalar()) {
    SingleEmissionCheckScope guard(this);
    (this->*fn)(zt, pg, SVEMemOperand(addr.GetScalarBase()));
    return;
  }

  // TODO: Handle scatter-gather forms.

  UseScratchRegisterScope temps(this);
  if (addr.IsScatterGather()) {
    // TODO: Use Adr(ZRegister, ...) to synthesise the address in the same way
    // as the non-scatter-gather forms.
    VIXL_UNIMPLEMENTED();
  } else {
    Register scratch = temps.AcquireX();
    // TODO: If we have an immediate offset that is a multiple of
    // msize_in_bytes, we can use Rdvl/Rdpl and a scalar-plus-scalar form to
    // save an instruction.
    int vl_divisor_log2 = zt.GetLaneSizeInBytesLog2() - msize_in_bytes_log2;
    CalculateSVEAddress(scratch, addr, vl_divisor_log2);
    SingleEmissionCheckScope guard(this);
    (this->*fn)(zt, pg, SVEMemOperand(scratch));
  }
}

void MacroAssembler::Ld1b(const ZRegister& zt,
                          const PRegisterZ& pg,
                          const SVEMemOperand& addr) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVELoadStore1Helper(kBRegSizeInBytesLog2,
                      zt,
                      pg,
                      addr,
                      static_cast<SVELoad1Fn>(&Assembler::ld1b));
}

void MacroAssembler::Ld1h(const ZRegister& zt,
                          const PRegisterZ& pg,
                          const SVEMemOperand& addr) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVELoadStore1Helper(kHRegSizeInBytesLog2,
                      zt,
                      pg,
                      addr,
                      static_cast<SVELoad1Fn>(&Assembler::ld1h));
}

void MacroAssembler::Ld1w(const ZRegister& zt,
                          const PRegisterZ& pg,
                          const SVEMemOperand& addr) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVELoadStore1Helper(kWRegSizeInBytesLog2,
                      zt,
                      pg,
                      addr,
                      static_cast<SVELoad1Fn>(&Assembler::ld1w));
}

void MacroAssembler::Ld1d(const ZRegister& zt,
                          const PRegisterZ& pg,
                          const SVEMemOperand& addr) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVELoadStore1Helper(kDRegSizeInBytesLog2,
                      zt,
                      pg,
                      addr,
                      static_cast<SVELoad1Fn>(&Assembler::ld1d));
}

void MacroAssembler::Ld1sb(const ZRegister& zt,
                           const PRegisterZ& pg,
                           const SVEMemOperand& addr) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVELoadStore1Helper(kBRegSizeInBytesLog2,
                      zt,
                      pg,
                      addr,
                      static_cast<SVELoad1Fn>(&Assembler::ld1sb));
}

void MacroAssembler::Ld1sh(const ZRegister& zt,
                           const PRegisterZ& pg,
                           const SVEMemOperand& addr) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVELoadStore1Helper(kHRegSizeInBytesLog2,
                      zt,
                      pg,
                      addr,
                      static_cast<SVELoad1Fn>(&Assembler::ld1sh));
}

void MacroAssembler::Ld1sw(const ZRegister& zt,
                           const PRegisterZ& pg,
                           const SVEMemOperand& addr) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVELoadStore1Helper(kSRegSizeInBytesLog2,
                      zt,
                      pg,
                      addr,
                      static_cast<SVELoad1Fn>(&Assembler::ld1sw));
}

void MacroAssembler::St1b(const ZRegister& zt,
                          const PRegister& pg,
                          const SVEMemOperand& addr) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVELoadStore1Helper(kBRegSizeInBytesLog2,
                      zt,
                      pg,
                      addr,
                      static_cast<SVEStore1Fn>(&Assembler::st1b));
}

void MacroAssembler::St1h(const ZRegister& zt,
                          const PRegister& pg,
                          const SVEMemOperand& addr) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVELoadStore1Helper(kHRegSizeInBytesLog2,
                      zt,
                      pg,
                      addr,
                      static_cast<SVEStore1Fn>(&Assembler::st1h));
}

void MacroAssembler::St1w(const ZRegister& zt,
                          const PRegister& pg,
                          const SVEMemOperand& addr) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVELoadStore1Helper(kSRegSizeInBytesLog2,
                      zt,
                      pg,
                      addr,
                      static_cast<SVEStore1Fn>(&Assembler::st1w));
}

void MacroAssembler::St1d(const ZRegister& zt,
                          const PRegister& pg,
                          const SVEMemOperand& addr) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVELoadStore1Helper(kDRegSizeInBytesLog2,
                      zt,
                      pg,
                      addr,
                      static_cast<SVEStore1Fn>(&Assembler::st1d));
}

void MacroAssembler::SVESdotUdotHelper(IntArithFn fn,
                                       const ZRegister& zd,
                                       const ZRegister& za,
                                       const ZRegister& zn,
                                       const ZRegister& zm) {
  if (zd.Aliases(za)) {
    // zda = zda + (zn . zm)
    SingleEmissionCheckScope guard(this);
    (this->*fn)(zd, zn, zm);

  } else if (zd.Aliases(zn) || zd.Aliases(zm)) {
    // zdn = za + (zdn . zm)
    // zdm = za + (zn . zdm)
    // zdnm = za + (zdnm . zdnm)
    UseScratchRegisterScope temps(this);
    ZRegister scratch = temps.AcquireZ().WithSameLaneSizeAs(zd);
    {
      MovprfxHelperScope guard(this, scratch, za);
      (this->*fn)(scratch, zn, zm);
    }

    Mov(zd, scratch);
  } else {
    // zd = za + (zn . zm)
    MovprfxHelperScope guard(this, zd, za);
    (this->*fn)(zd, zn, zm);
  }
}

void MacroAssembler::Sdot(const ZRegister& zd,
                          const ZRegister& za,
                          const ZRegister& zn,
                          const ZRegister& zm) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVESdotUdotHelper(&Assembler::sdot, zd, za, zn, zm);
}

void MacroAssembler::Udot(const ZRegister& zd,
                          const ZRegister& za,
                          const ZRegister& zn,
                          const ZRegister& zm) {
  VIXL_ASSERT(allow_macro_instructions_);
  SVESdotUdotHelper(&Assembler::udot, zd, za, zn, zm);
}

}  // namespace aarch64
}  // namespace vixl
