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

}  // namespace aarch64
}  // namespace vixl
