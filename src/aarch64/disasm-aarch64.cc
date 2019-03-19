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

#include <cstdlib>
#include <sstream>

#include "disasm-aarch64.h"

namespace vixl {
namespace aarch64 {


Disassembler::Disassembler() {
  buffer_size_ = 256;
  buffer_ = reinterpret_cast<char *>(malloc(buffer_size_));
  buffer_pos_ = 0;
  own_buffer_ = true;
  code_address_offset_ = 0;
}


Disassembler::Disassembler(char *text_buffer, int buffer_size) {
  buffer_size_ = buffer_size;
  buffer_ = text_buffer;
  buffer_pos_ = 0;
  own_buffer_ = false;
  code_address_offset_ = 0;
}


Disassembler::~Disassembler() {
  if (own_buffer_) {
    free(buffer_);
  }
}


char *Disassembler::GetOutput() { return buffer_; }


void Disassembler::VisitAddSubImmediate(const Instruction *instr) {
  bool rd_is_zr = RdIsZROrSP(instr);
  bool stack_op =
      (rd_is_zr || RnIsZROrSP(instr)) && (instr->GetImmAddSub() == 0) ? true
                                                                      : false;
  const char *mnemonic = "";
  const char *form = "'Rds, 'Rns, 'IAddSub";
  const char *form_cmp = "'Rns, 'IAddSub";
  const char *form_mov = "'Rds, 'Rns";

  switch (instr->Mask(AddSubImmediateMask)) {
    case ADD_w_imm:
    case ADD_x_imm: {
      mnemonic = "add";
      if (stack_op) {
        mnemonic = "mov";
        form = form_mov;
      }
      break;
    }
    case ADDS_w_imm:
    case ADDS_x_imm: {
      mnemonic = "adds";
      if (rd_is_zr) {
        mnemonic = "cmn";
        form = form_cmp;
      }
      break;
    }
    case SUB_w_imm:
    case SUB_x_imm:
      mnemonic = "sub";
      break;
    case SUBS_w_imm:
    case SUBS_x_imm: {
      mnemonic = "subs";
      if (rd_is_zr) {
        mnemonic = "cmp";
        form = form_cmp;
      }
      break;
    }
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitAddSubShifted(const Instruction *instr) {
  bool rd_is_zr = RdIsZROrSP(instr);
  bool rn_is_zr = RnIsZROrSP(instr);
  const char *mnemonic = "";
  const char *form = "'Rd, 'Rn, 'Rm'NDP";
  const char *form_cmp = "'Rn, 'Rm'NDP";
  const char *form_neg = "'Rd, 'Rm'NDP";

  switch (instr->Mask(AddSubShiftedMask)) {
    case ADD_w_shift:
    case ADD_x_shift:
      mnemonic = "add";
      break;
    case ADDS_w_shift:
    case ADDS_x_shift: {
      mnemonic = "adds";
      if (rd_is_zr) {
        mnemonic = "cmn";
        form = form_cmp;
      }
      break;
    }
    case SUB_w_shift:
    case SUB_x_shift: {
      mnemonic = "sub";
      if (rn_is_zr) {
        mnemonic = "neg";
        form = form_neg;
      }
      break;
    }
    case SUBS_w_shift:
    case SUBS_x_shift: {
      mnemonic = "subs";
      if (rd_is_zr) {
        mnemonic = "cmp";
        form = form_cmp;
      } else if (rn_is_zr) {
        mnemonic = "negs";
        form = form_neg;
      }
      break;
    }
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitAddSubExtended(const Instruction *instr) {
  bool rd_is_zr = RdIsZROrSP(instr);
  const char *mnemonic = "";
  Extend mode = static_cast<Extend>(instr->GetExtendMode());
  const char *form = ((mode == UXTX) || (mode == SXTX)) ? "'Rds, 'Rns, 'Xm'Ext"
                                                        : "'Rds, 'Rns, 'Wm'Ext";
  const char *form_cmp =
      ((mode == UXTX) || (mode == SXTX)) ? "'Rns, 'Xm'Ext" : "'Rns, 'Wm'Ext";

  switch (instr->Mask(AddSubExtendedMask)) {
    case ADD_w_ext:
    case ADD_x_ext:
      mnemonic = "add";
      break;
    case ADDS_w_ext:
    case ADDS_x_ext: {
      mnemonic = "adds";
      if (rd_is_zr) {
        mnemonic = "cmn";
        form = form_cmp;
      }
      break;
    }
    case SUB_w_ext:
    case SUB_x_ext:
      mnemonic = "sub";
      break;
    case SUBS_w_ext:
    case SUBS_x_ext: {
      mnemonic = "subs";
      if (rd_is_zr) {
        mnemonic = "cmp";
        form = form_cmp;
      }
      break;
    }
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitAddSubWithCarry(const Instruction *instr) {
  bool rn_is_zr = RnIsZROrSP(instr);
  const char *mnemonic = "";
  const char *form = "'Rd, 'Rn, 'Rm";
  const char *form_neg = "'Rd, 'Rm";

  switch (instr->Mask(AddSubWithCarryMask)) {
    case ADC_w:
    case ADC_x:
      mnemonic = "adc";
      break;
    case ADCS_w:
    case ADCS_x:
      mnemonic = "adcs";
      break;
    case SBC_w:
    case SBC_x: {
      mnemonic = "sbc";
      if (rn_is_zr) {
        mnemonic = "ngc";
        form = form_neg;
      }
      break;
    }
    case SBCS_w:
    case SBCS_x: {
      mnemonic = "sbcs";
      if (rn_is_zr) {
        mnemonic = "ngcs";
        form = form_neg;
      }
      break;
    }
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitRotateRightIntoFlags(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(RotateRightIntoFlags)";

  switch (instr->Mask(RotateRightIntoFlagsMask)) {
    case RMIF:
      mnemonic = "rmif";
      form = "'Xn, 'IRr, 'INzcv";
      break;
    default:
      VIXL_UNREACHABLE();
  }

  Format(instr, mnemonic, form);
}


void Disassembler::VisitEvaluateIntoFlags(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(EvaluateIntoFlags)";

  switch (instr->Mask(EvaluateIntoFlagsMask)) {
    case SETF8:
      mnemonic = "setf8";
      form = "'Wn";
      break;
    case SETF16:
      mnemonic = "setf16";
      form = "'Wn";
      break;
    default:
      VIXL_UNREACHABLE();
  }

  Format(instr, mnemonic, form);
}


void Disassembler::VisitLogicalImmediate(const Instruction *instr) {
  bool rd_is_zr = RdIsZROrSP(instr);
  bool rn_is_zr = RnIsZROrSP(instr);
  const char *mnemonic = "";
  const char *form = "'Rds, 'Rn, 'ITri";

  if (instr->GetImmLogical() == 0) {
    // The immediate encoded in the instruction is not in the expected format.
    Format(instr, "unallocated", "(LogicalImmediate)");
    return;
  }

  switch (instr->Mask(LogicalImmediateMask)) {
    case AND_w_imm:
    case AND_x_imm:
      mnemonic = "and";
      break;
    case ORR_w_imm:
    case ORR_x_imm: {
      mnemonic = "orr";
      unsigned reg_size =
          (instr->GetSixtyFourBits() == 1) ? kXRegSize : kWRegSize;
      if (rn_is_zr && !IsMovzMovnImm(reg_size, instr->GetImmLogical())) {
        mnemonic = "mov";
        form = "'Rds, 'ITri";
      }
      break;
    }
    case EOR_w_imm:
    case EOR_x_imm:
      mnemonic = "eor";
      break;
    case ANDS_w_imm:
    case ANDS_x_imm: {
      mnemonic = "ands";
      if (rd_is_zr) {
        mnemonic = "tst";
        form = "'Rn, 'ITri";
      }
      break;
    }
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


bool Disassembler::IsMovzMovnImm(unsigned reg_size, uint64_t value) {
  VIXL_ASSERT((reg_size == kXRegSize) ||
              ((reg_size == kWRegSize) && (value <= 0xffffffff)));

  // Test for movz: 16 bits set at positions 0, 16, 32 or 48.
  if (((value & UINT64_C(0xffffffffffff0000)) == 0) ||
      ((value & UINT64_C(0xffffffff0000ffff)) == 0) ||
      ((value & UINT64_C(0xffff0000ffffffff)) == 0) ||
      ((value & UINT64_C(0x0000ffffffffffff)) == 0)) {
    return true;
  }

  // Test for movn: NOT(16 bits set at positions 0, 16, 32 or 48).
  if ((reg_size == kXRegSize) &&
      (((~value & UINT64_C(0xffffffffffff0000)) == 0) ||
       ((~value & UINT64_C(0xffffffff0000ffff)) == 0) ||
       ((~value & UINT64_C(0xffff0000ffffffff)) == 0) ||
       ((~value & UINT64_C(0x0000ffffffffffff)) == 0))) {
    return true;
  }
  if ((reg_size == kWRegSize) && (((value & 0xffff0000) == 0xffff0000) ||
                                  ((value & 0x0000ffff) == 0x0000ffff))) {
    return true;
  }
  return false;
}


void Disassembler::VisitLogicalShifted(const Instruction *instr) {
  bool rd_is_zr = RdIsZROrSP(instr);
  bool rn_is_zr = RnIsZROrSP(instr);
  const char *mnemonic = "";
  const char *form = "'Rd, 'Rn, 'Rm'NLo";

  switch (instr->Mask(LogicalShiftedMask)) {
    case AND_w:
    case AND_x:
      mnemonic = "and";
      break;
    case BIC_w:
    case BIC_x:
      mnemonic = "bic";
      break;
    case EOR_w:
    case EOR_x:
      mnemonic = "eor";
      break;
    case EON_w:
    case EON_x:
      mnemonic = "eon";
      break;
    case BICS_w:
    case BICS_x:
      mnemonic = "bics";
      break;
    case ANDS_w:
    case ANDS_x: {
      mnemonic = "ands";
      if (rd_is_zr) {
        mnemonic = "tst";
        form = "'Rn, 'Rm'NLo";
      }
      break;
    }
    case ORR_w:
    case ORR_x: {
      mnemonic = "orr";
      if (rn_is_zr && (instr->GetImmDPShift() == 0) &&
          (instr->GetShiftDP() == LSL)) {
        mnemonic = "mov";
        form = "'Rd, 'Rm";
      }
      break;
    }
    case ORN_w:
    case ORN_x: {
      mnemonic = "orn";
      if (rn_is_zr) {
        mnemonic = "mvn";
        form = "'Rd, 'Rm'NLo";
      }
      break;
    }
    default:
      VIXL_UNREACHABLE();
  }

  Format(instr, mnemonic, form);
}


void Disassembler::VisitConditionalCompareRegister(const Instruction *instr) {
  const char *mnemonic = "";
  const char *form = "'Rn, 'Rm, 'INzcv, 'Cond";

  switch (instr->Mask(ConditionalCompareRegisterMask)) {
    case CCMN_w:
    case CCMN_x:
      mnemonic = "ccmn";
      break;
    case CCMP_w:
    case CCMP_x:
      mnemonic = "ccmp";
      break;
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitConditionalCompareImmediate(const Instruction *instr) {
  const char *mnemonic = "";
  const char *form = "'Rn, 'IP, 'INzcv, 'Cond";

  switch (instr->Mask(ConditionalCompareImmediateMask)) {
    case CCMN_w_imm:
    case CCMN_x_imm:
      mnemonic = "ccmn";
      break;
    case CCMP_w_imm:
    case CCMP_x_imm:
      mnemonic = "ccmp";
      break;
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitConditionalSelect(const Instruction *instr) {
  bool rnm_is_zr = (RnIsZROrSP(instr) && RmIsZROrSP(instr));
  bool rn_is_rm = (instr->GetRn() == instr->GetRm());
  const char *mnemonic = "";
  const char *form = "'Rd, 'Rn, 'Rm, 'Cond";
  const char *form_test = "'Rd, 'CInv";
  const char *form_update = "'Rd, 'Rn, 'CInv";

  Condition cond = static_cast<Condition>(instr->GetCondition());
  bool invertible_cond = (cond != al) && (cond != nv);

  switch (instr->Mask(ConditionalSelectMask)) {
    case CSEL_w:
    case CSEL_x:
      mnemonic = "csel";
      break;
    case CSINC_w:
    case CSINC_x: {
      mnemonic = "csinc";
      if (rnm_is_zr && invertible_cond) {
        mnemonic = "cset";
        form = form_test;
      } else if (rn_is_rm && invertible_cond) {
        mnemonic = "cinc";
        form = form_update;
      }
      break;
    }
    case CSINV_w:
    case CSINV_x: {
      mnemonic = "csinv";
      if (rnm_is_zr && invertible_cond) {
        mnemonic = "csetm";
        form = form_test;
      } else if (rn_is_rm && invertible_cond) {
        mnemonic = "cinv";
        form = form_update;
      }
      break;
    }
    case CSNEG_w:
    case CSNEG_x: {
      mnemonic = "csneg";
      if (rn_is_rm && invertible_cond) {
        mnemonic = "cneg";
        form = form_update;
      }
      break;
    }
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitBitfield(const Instruction *instr) {
  unsigned s = instr->GetImmS();
  unsigned r = instr->GetImmR();
  unsigned rd_size_minus_1 =
      ((instr->GetSixtyFourBits() == 1) ? kXRegSize : kWRegSize) - 1;
  const char *mnemonic = "";
  const char *form = "";
  const char *form_shift_right = "'Rd, 'Rn, 'IBr";
  const char *form_extend = "'Rd, 'Wn";
  const char *form_bfiz = "'Rd, 'Rn, 'IBZ-r, 'IBs+1";
  const char *form_bfc = "'Rd, 'IBZ-r, 'IBs+1";
  const char *form_bfx = "'Rd, 'Rn, 'IBr, 'IBs-r+1";
  const char *form_lsl = "'Rd, 'Rn, 'IBZ-r";

  switch (instr->Mask(BitfieldMask)) {
    case SBFM_w:
    case SBFM_x: {
      mnemonic = "sbfx";
      form = form_bfx;
      if (r == 0) {
        form = form_extend;
        if (s == 7) {
          mnemonic = "sxtb";
        } else if (s == 15) {
          mnemonic = "sxth";
        } else if ((s == 31) && (instr->GetSixtyFourBits() == 1)) {
          mnemonic = "sxtw";
        } else {
          form = form_bfx;
        }
      } else if (s == rd_size_minus_1) {
        mnemonic = "asr";
        form = form_shift_right;
      } else if (s < r) {
        mnemonic = "sbfiz";
        form = form_bfiz;
      }
      break;
    }
    case UBFM_w:
    case UBFM_x: {
      mnemonic = "ubfx";
      form = form_bfx;
      if (r == 0) {
        form = form_extend;
        if (s == 7) {
          mnemonic = "uxtb";
        } else if (s == 15) {
          mnemonic = "uxth";
        } else {
          form = form_bfx;
        }
      }
      if (s == rd_size_minus_1) {
        mnemonic = "lsr";
        form = form_shift_right;
      } else if (r == s + 1) {
        mnemonic = "lsl";
        form = form_lsl;
      } else if (s < r) {
        mnemonic = "ubfiz";
        form = form_bfiz;
      }
      break;
    }
    case BFM_w:
    case BFM_x: {
      mnemonic = "bfxil";
      form = form_bfx;
      if (s < r) {
        if (instr->GetRn() == kZeroRegCode) {
          mnemonic = "bfc";
          form = form_bfc;
        } else {
          mnemonic = "bfi";
          form = form_bfiz;
        }
      }
    }
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitExtract(const Instruction *instr) {
  const char *mnemonic = "";
  const char *form = "'Rd, 'Rn, 'Rm, 'IExtract";

  switch (instr->Mask(ExtractMask)) {
    case EXTR_w:
    case EXTR_x: {
      if (instr->GetRn() == instr->GetRm()) {
        mnemonic = "ror";
        form = "'Rd, 'Rn, 'IExtract";
      } else {
        mnemonic = "extr";
      }
      break;
    }
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitPCRelAddressing(const Instruction *instr) {
  switch (instr->Mask(PCRelAddressingMask)) {
    case ADR:
      Format(instr, "adr", "'Xd, 'AddrPCRelByte");
      break;
    case ADRP:
      Format(instr, "adrp", "'Xd, 'AddrPCRelPage");
      break;
    default:
      Format(instr, "unimplemented", "(PCRelAddressing)");
  }
}


void Disassembler::VisitConditionalBranch(const Instruction *instr) {
  switch (instr->Mask(ConditionalBranchMask)) {
    case B_cond:
      Format(instr, "b.'CBrn", "'TImmCond");
      break;
    default:
      VIXL_UNREACHABLE();
  }
}


void Disassembler::VisitUnconditionalBranchToRegister(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form;

  switch (instr->Mask(UnconditionalBranchToRegisterMask)) {
    case BR:
      mnemonic = "br";
      form = "'Xn";
      break;
    case BLR:
      mnemonic = "blr";
      form = "'Xn";
      break;
    case RET: {
      mnemonic = "ret";
      if (instr->GetRn() == kLinkRegCode) {
        form = NULL;
      } else {
        form = "'Xn";
      }
      break;
    }
    case BRAAZ:
      mnemonic = "braaz";
      form = "'Xn";
      break;
    case BRABZ:
      mnemonic = "brabz";
      form = "'Xn";
      break;
    case BLRAAZ:
      mnemonic = "blraaz";
      form = "'Xn";
      break;
    case BLRABZ:
      mnemonic = "blrabz";
      form = "'Xn";
      break;
    case RETAA:
      mnemonic = "retaa";
      form = NULL;
      break;
    case RETAB:
      mnemonic = "retab";
      form = NULL;
      break;
    case BRAA:
      mnemonic = "braa";
      form = "'Xn, 'Xds";
      break;
    case BRAB:
      mnemonic = "brab";
      form = "'Xn, 'Xds";
      break;
    case BLRAA:
      mnemonic = "blraa";
      form = "'Xn, 'Xds";
      break;
    case BLRAB:
      mnemonic = "blrab";
      form = "'Xn, 'Xds";
      break;
    default:
      form = "(UnconditionalBranchToRegister)";
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitUnconditionalBranch(const Instruction *instr) {
  const char *mnemonic = "";
  const char *form = "'TImmUncn";

  switch (instr->Mask(UnconditionalBranchMask)) {
    case B:
      mnemonic = "b";
      break;
    case BL:
      mnemonic = "bl";
      break;
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitDataProcessing1Source(const Instruction *instr) {
  const char *mnemonic = "";
  const char *form = "'Rd, 'Rn";

  switch (instr->Mask(DataProcessing1SourceMask)) {
#define FORMAT(A, B) \
  case A##_w:        \
  case A##_x:        \
    mnemonic = B;    \
    break;
    FORMAT(RBIT, "rbit");
    FORMAT(REV16, "rev16");
    FORMAT(REV, "rev");
    FORMAT(CLZ, "clz");
    FORMAT(CLS, "cls");
#undef FORMAT

#define PAUTH_VARIATIONS(V) \
  V(PACI, "paci")           \
  V(PACD, "pacd")           \
  V(AUTI, "auti")           \
  V(AUTD, "autd")
#define PAUTH_CASE(NAME, MN) \
  case NAME##A:              \
    mnemonic = MN "a";       \
    form = "'Xd, 'Xns";      \
    break;                   \
  case NAME##ZA:             \
    mnemonic = MN "za";      \
    form = "'Xd";            \
    break;                   \
  case NAME##B:              \
    mnemonic = MN "b";       \
    form = "'Xd, 'Xns";      \
    break;                   \
  case NAME##ZB:             \
    mnemonic = MN "zb";      \
    form = "'Xd";            \
    break;

    PAUTH_VARIATIONS(PAUTH_CASE)
#undef PAUTH_CASE

    case XPACI:
      mnemonic = "xpaci";
      form = "'Xd";
      break;
    case XPACD:
      mnemonic = "xpacd";
      form = "'Xd";
      break;
    case REV32_x:
      mnemonic = "rev32";
      break;
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitDataProcessing2Source(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Rd, 'Rn, 'Rm";
  const char *form_wwx = "'Wd, 'Wn, 'Xm";

  switch (instr->Mask(DataProcessing2SourceMask)) {
#define FORMAT(A, B) \
  case A##_w:        \
  case A##_x:        \
    mnemonic = B;    \
    break;
    FORMAT(UDIV, "udiv");
    FORMAT(SDIV, "sdiv");
    FORMAT(LSLV, "lsl");
    FORMAT(LSRV, "lsr");
    FORMAT(ASRV, "asr");
    FORMAT(RORV, "ror");
#undef FORMAT
    case PACGA:
      mnemonic = "pacga";
      form = "'Xd, 'Xn, 'Xms";
      break;
    case CRC32B:
      mnemonic = "crc32b";
      break;
    case CRC32H:
      mnemonic = "crc32h";
      break;
    case CRC32W:
      mnemonic = "crc32w";
      break;
    case CRC32X:
      mnemonic = "crc32x";
      form = form_wwx;
      break;
    case CRC32CB:
      mnemonic = "crc32cb";
      break;
    case CRC32CH:
      mnemonic = "crc32ch";
      break;
    case CRC32CW:
      mnemonic = "crc32cw";
      break;
    case CRC32CX:
      mnemonic = "crc32cx";
      form = form_wwx;
      break;
    default:
      form = "(DataProcessing2Source)";
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitDataProcessing3Source(const Instruction *instr) {
  bool ra_is_zr = RaIsZROrSP(instr);
  const char *mnemonic = "";
  const char *form = "'Xd, 'Wn, 'Wm, 'Xa";
  const char *form_rrr = "'Rd, 'Rn, 'Rm";
  const char *form_rrrr = "'Rd, 'Rn, 'Rm, 'Ra";
  const char *form_xww = "'Xd, 'Wn, 'Wm";
  const char *form_xxx = "'Xd, 'Xn, 'Xm";

  switch (instr->Mask(DataProcessing3SourceMask)) {
    case MADD_w:
    case MADD_x: {
      mnemonic = "madd";
      form = form_rrrr;
      if (ra_is_zr) {
        mnemonic = "mul";
        form = form_rrr;
      }
      break;
    }
    case MSUB_w:
    case MSUB_x: {
      mnemonic = "msub";
      form = form_rrrr;
      if (ra_is_zr) {
        mnemonic = "mneg";
        form = form_rrr;
      }
      break;
    }
    case SMADDL_x: {
      mnemonic = "smaddl";
      if (ra_is_zr) {
        mnemonic = "smull";
        form = form_xww;
      }
      break;
    }
    case SMSUBL_x: {
      mnemonic = "smsubl";
      if (ra_is_zr) {
        mnemonic = "smnegl";
        form = form_xww;
      }
      break;
    }
    case UMADDL_x: {
      mnemonic = "umaddl";
      if (ra_is_zr) {
        mnemonic = "umull";
        form = form_xww;
      }
      break;
    }
    case UMSUBL_x: {
      mnemonic = "umsubl";
      if (ra_is_zr) {
        mnemonic = "umnegl";
        form = form_xww;
      }
      break;
    }
    case SMULH_x: {
      mnemonic = "smulh";
      form = form_xxx;
      break;
    }
    case UMULH_x: {
      mnemonic = "umulh";
      form = form_xxx;
      break;
    }
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitCompareBranch(const Instruction *instr) {
  const char *mnemonic = "";
  const char *form = "'Rt, 'TImmCmpa";

  switch (instr->Mask(CompareBranchMask)) {
    case CBZ_w:
    case CBZ_x:
      mnemonic = "cbz";
      break;
    case CBNZ_w:
    case CBNZ_x:
      mnemonic = "cbnz";
      break;
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitTestBranch(const Instruction *instr) {
  const char *mnemonic = "";
  // If the top bit of the immediate is clear, the tested register is
  // disassembled as Wt, otherwise Xt. As the top bit of the immediate is
  // encoded in bit 31 of the instruction, we can reuse the Rt form, which
  // uses bit 31 (normally "sf") to choose the register size.
  const char *form = "'Rt, 'IS, 'TImmTest";

  switch (instr->Mask(TestBranchMask)) {
    case TBZ:
      mnemonic = "tbz";
      break;
    case TBNZ:
      mnemonic = "tbnz";
      break;
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitMoveWideImmediate(const Instruction *instr) {
  const char *mnemonic = "";
  const char *form = "'Rd, 'IMoveImm";

  // Print the shift separately for movk, to make it clear which half word will
  // be overwritten. Movn and movz print the computed immediate, which includes
  // shift calculation.
  switch (instr->Mask(MoveWideImmediateMask)) {
    case MOVN_w:
    case MOVN_x:
      if ((instr->GetImmMoveWide()) || (instr->GetShiftMoveWide() == 0)) {
        if ((instr->GetSixtyFourBits() == 0) &&
            (instr->GetImmMoveWide() == 0xffff)) {
          mnemonic = "movn";
        } else {
          mnemonic = "mov";
          form = "'Rd, 'IMoveNeg";
        }
      } else {
        mnemonic = "movn";
      }
      break;
    case MOVZ_w:
    case MOVZ_x:
      if ((instr->GetImmMoveWide()) || (instr->GetShiftMoveWide() == 0))
        mnemonic = "mov";
      else
        mnemonic = "movz";
      break;
    case MOVK_w:
    case MOVK_x:
      mnemonic = "movk";
      form = "'Rd, 'IMoveLSL";
      break;
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


#define LOAD_STORE_LIST(V)   \
  V(STRB_w, "strb", "'Wt")   \
  V(STRH_w, "strh", "'Wt")   \
  V(STR_w, "str", "'Wt")     \
  V(STR_x, "str", "'Xt")     \
  V(LDRB_w, "ldrb", "'Wt")   \
  V(LDRH_w, "ldrh", "'Wt")   \
  V(LDR_w, "ldr", "'Wt")     \
  V(LDR_x, "ldr", "'Xt")     \
  V(LDRSB_x, "ldrsb", "'Xt") \
  V(LDRSH_x, "ldrsh", "'Xt") \
  V(LDRSW_x, "ldrsw", "'Xt") \
  V(LDRSB_w, "ldrsb", "'Wt") \
  V(LDRSH_w, "ldrsh", "'Wt") \
  V(STR_b, "str", "'Bt")     \
  V(STR_h, "str", "'Ht")     \
  V(STR_s, "str", "'St")     \
  V(STR_d, "str", "'Dt")     \
  V(LDR_b, "ldr", "'Bt")     \
  V(LDR_h, "ldr", "'Ht")     \
  V(LDR_s, "ldr", "'St")     \
  V(LDR_d, "ldr", "'Dt")     \
  V(STR_q, "str", "'Qt")     \
  V(LDR_q, "ldr", "'Qt")

void Disassembler::VisitLoadStorePreIndex(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(LoadStorePreIndex)";

  switch (instr->Mask(LoadStorePreIndexMask)) {
#define LS_PREINDEX(A, B, C)   \
  case A##_pre:                \
    mnemonic = B;              \
    form = C ", ['Xns'ILSi]!"; \
    break;
    LOAD_STORE_LIST(LS_PREINDEX)
#undef LS_PREINDEX
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitLoadStorePostIndex(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(LoadStorePostIndex)";

  switch (instr->Mask(LoadStorePostIndexMask)) {
#define LS_POSTINDEX(A, B, C) \
  case A##_post:              \
    mnemonic = B;             \
    form = C ", ['Xns]'ILSi"; \
    break;
    LOAD_STORE_LIST(LS_POSTINDEX)
#undef LS_POSTINDEX
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitLoadStoreUnsignedOffset(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(LoadStoreUnsignedOffset)";

  switch (instr->Mask(LoadStoreUnsignedOffsetMask)) {
#define LS_UNSIGNEDOFFSET(A, B, C) \
  case A##_unsigned:               \
    mnemonic = B;                  \
    form = C ", ['Xns'ILU]";       \
    break;
    LOAD_STORE_LIST(LS_UNSIGNEDOFFSET)
#undef LS_UNSIGNEDOFFSET
    case PRFM_unsigned:
      mnemonic = "prfm";
      form = "'prefOp, ['Xns'ILU]";
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitLoadStoreRCpcUnscaledOffset(const Instruction *instr) {
  const char *mnemonic;
  const char *form = "'Wt, ['Xns'ILS]";
  const char *form_x = "'Xt, ['Xns'ILS]";

  switch (instr->Mask(LoadStoreRCpcUnscaledOffsetMask)) {
    case STLURB:
      mnemonic = "stlurb";
      break;
    case LDAPURB:
      mnemonic = "ldapurb";
      break;
    case LDAPURSB_w:
      mnemonic = "ldapursb";
      break;
    case LDAPURSB_x:
      mnemonic = "ldapursb";
      form = form_x;
      break;
    case STLURH:
      mnemonic = "stlurh";
      break;
    case LDAPURH:
      mnemonic = "ldapurh";
      break;
    case LDAPURSH_w:
      mnemonic = "ldapursh";
      break;
    case LDAPURSH_x:
      mnemonic = "ldapursh";
      form = form_x;
      break;
    case STLUR_w:
      mnemonic = "stlur";
      break;
    case LDAPUR_w:
      mnemonic = "ldapur";
      break;
    case LDAPURSW:
      mnemonic = "ldapursw";
      form = form_x;
      break;
    case STLUR_x:
      mnemonic = "stlur";
      form = form_x;
      break;
    case LDAPUR_x:
      mnemonic = "ldapur";
      form = form_x;
      break;
    default:
      mnemonic = "unimplemented";
      form = "(LoadStoreRCpcUnscaledOffset)";
  }

  Format(instr, mnemonic, form);
}


void Disassembler::VisitLoadStoreRegisterOffset(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(LoadStoreRegisterOffset)";

  switch (instr->Mask(LoadStoreRegisterOffsetMask)) {
#define LS_REGISTEROFFSET(A, B, C)   \
  case A##_reg:                      \
    mnemonic = B;                    \
    form = C ", ['Xns, 'Offsetreg]"; \
    break;
    LOAD_STORE_LIST(LS_REGISTEROFFSET)
#undef LS_REGISTEROFFSET
    case PRFM_reg:
      mnemonic = "prfm";
      form = "'prefOp, ['Xns, 'Offsetreg]";
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitLoadStoreUnscaledOffset(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Wt, ['Xns'ILS]";
  const char *form_x = "'Xt, ['Xns'ILS]";
  const char *form_b = "'Bt, ['Xns'ILS]";
  const char *form_h = "'Ht, ['Xns'ILS]";
  const char *form_s = "'St, ['Xns'ILS]";
  const char *form_d = "'Dt, ['Xns'ILS]";
  const char *form_q = "'Qt, ['Xns'ILS]";
  const char *form_prefetch = "'prefOp, ['Xns'ILS]";

  switch (instr->Mask(LoadStoreUnscaledOffsetMask)) {
    case STURB_w:
      mnemonic = "sturb";
      break;
    case STURH_w:
      mnemonic = "sturh";
      break;
    case STUR_w:
      mnemonic = "stur";
      break;
    case STUR_x:
      mnemonic = "stur";
      form = form_x;
      break;
    case STUR_b:
      mnemonic = "stur";
      form = form_b;
      break;
    case STUR_h:
      mnemonic = "stur";
      form = form_h;
      break;
    case STUR_s:
      mnemonic = "stur";
      form = form_s;
      break;
    case STUR_d:
      mnemonic = "stur";
      form = form_d;
      break;
    case STUR_q:
      mnemonic = "stur";
      form = form_q;
      break;
    case LDURB_w:
      mnemonic = "ldurb";
      break;
    case LDURH_w:
      mnemonic = "ldurh";
      break;
    case LDUR_w:
      mnemonic = "ldur";
      break;
    case LDUR_x:
      mnemonic = "ldur";
      form = form_x;
      break;
    case LDUR_b:
      mnemonic = "ldur";
      form = form_b;
      break;
    case LDUR_h:
      mnemonic = "ldur";
      form = form_h;
      break;
    case LDUR_s:
      mnemonic = "ldur";
      form = form_s;
      break;
    case LDUR_d:
      mnemonic = "ldur";
      form = form_d;
      break;
    case LDUR_q:
      mnemonic = "ldur";
      form = form_q;
      break;
    case LDURSB_x:
      form = form_x;
      VIXL_FALLTHROUGH();
    case LDURSB_w:
      mnemonic = "ldursb";
      break;
    case LDURSH_x:
      form = form_x;
      VIXL_FALLTHROUGH();
    case LDURSH_w:
      mnemonic = "ldursh";
      break;
    case LDURSW_x:
      mnemonic = "ldursw";
      form = form_x;
      break;
    case PRFUM:
      mnemonic = "prfum";
      form = form_prefetch;
      break;
    default:
      form = "(LoadStoreUnscaledOffset)";
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitLoadLiteral(const Instruction *instr) {
  const char *mnemonic = "ldr";
  const char *form = "(LoadLiteral)";

  switch (instr->Mask(LoadLiteralMask)) {
    case LDR_w_lit:
      form = "'Wt, 'ILLiteral 'LValue";
      break;
    case LDR_x_lit:
      form = "'Xt, 'ILLiteral 'LValue";
      break;
    case LDR_s_lit:
      form = "'St, 'ILLiteral 'LValue";
      break;
    case LDR_d_lit:
      form = "'Dt, 'ILLiteral 'LValue";
      break;
    case LDR_q_lit:
      form = "'Qt, 'ILLiteral 'LValue";
      break;
    case LDRSW_x_lit: {
      mnemonic = "ldrsw";
      form = "'Xt, 'ILLiteral 'LValue";
      break;
    }
    case PRFM_lit: {
      mnemonic = "prfm";
      form = "'prefOp, 'ILLiteral 'LValue";
      break;
    }
    default:
      mnemonic = "unimplemented";
  }
  Format(instr, mnemonic, form);
}


#define LOAD_STORE_PAIR_LIST(V)         \
  V(STP_w, "stp", "'Wt, 'Wt2", "2")     \
  V(LDP_w, "ldp", "'Wt, 'Wt2", "2")     \
  V(LDPSW_x, "ldpsw", "'Xt, 'Xt2", "2") \
  V(STP_x, "stp", "'Xt, 'Xt2", "3")     \
  V(LDP_x, "ldp", "'Xt, 'Xt2", "3")     \
  V(STP_s, "stp", "'St, 'St2", "2")     \
  V(LDP_s, "ldp", "'St, 'St2", "2")     \
  V(STP_d, "stp", "'Dt, 'Dt2", "3")     \
  V(LDP_d, "ldp", "'Dt, 'Dt2", "3")     \
  V(LDP_q, "ldp", "'Qt, 'Qt2", "4")     \
  V(STP_q, "stp", "'Qt, 'Qt2", "4")

void Disassembler::VisitLoadStorePairPostIndex(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(LoadStorePairPostIndex)";

  switch (instr->Mask(LoadStorePairPostIndexMask)) {
#define LSP_POSTINDEX(A, B, C, D)  \
  case A##_post:                   \
    mnemonic = B;                  \
    form = C ", ['Xns]'ILP" D "i"; \
    break;
    LOAD_STORE_PAIR_LIST(LSP_POSTINDEX)
#undef LSP_POSTINDEX
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitLoadStorePairPreIndex(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(LoadStorePairPreIndex)";

  switch (instr->Mask(LoadStorePairPreIndexMask)) {
#define LSP_PREINDEX(A, B, C, D)    \
  case A##_pre:                     \
    mnemonic = B;                   \
    form = C ", ['Xns'ILP" D "i]!"; \
    break;
    LOAD_STORE_PAIR_LIST(LSP_PREINDEX)
#undef LSP_PREINDEX
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitLoadStorePairOffset(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(LoadStorePairOffset)";

  switch (instr->Mask(LoadStorePairOffsetMask)) {
#define LSP_OFFSET(A, B, C, D)    \
  case A##_off:                   \
    mnemonic = B;                 \
    form = C ", ['Xns'ILP" D "]"; \
    break;
    LOAD_STORE_PAIR_LIST(LSP_OFFSET)
#undef LSP_OFFSET
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitLoadStorePairNonTemporal(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form;

  switch (instr->Mask(LoadStorePairNonTemporalMask)) {
    case STNP_w:
      mnemonic = "stnp";
      form = "'Wt, 'Wt2, ['Xns'ILP2]";
      break;
    case LDNP_w:
      mnemonic = "ldnp";
      form = "'Wt, 'Wt2, ['Xns'ILP2]";
      break;
    case STNP_x:
      mnemonic = "stnp";
      form = "'Xt, 'Xt2, ['Xns'ILP3]";
      break;
    case LDNP_x:
      mnemonic = "ldnp";
      form = "'Xt, 'Xt2, ['Xns'ILP3]";
      break;
    case STNP_s:
      mnemonic = "stnp";
      form = "'St, 'St2, ['Xns'ILP2]";
      break;
    case LDNP_s:
      mnemonic = "ldnp";
      form = "'St, 'St2, ['Xns'ILP2]";
      break;
    case STNP_d:
      mnemonic = "stnp";
      form = "'Dt, 'Dt2, ['Xns'ILP3]";
      break;
    case LDNP_d:
      mnemonic = "ldnp";
      form = "'Dt, 'Dt2, ['Xns'ILP3]";
      break;
    case STNP_q:
      mnemonic = "stnp";
      form = "'Qt, 'Qt2, ['Xns'ILP4]";
      break;
    case LDNP_q:
      mnemonic = "ldnp";
      form = "'Qt, 'Qt2, ['Xns'ILP4]";
      break;
    default:
      form = "(LoadStorePairNonTemporal)";
  }
  Format(instr, mnemonic, form);
}

// clang-format off
#define LOAD_STORE_EXCLUSIVE_LIST(V)                  \
  V(STXRB_w,  "stxrb",  "'Ws, 'Wt")                   \
  V(STXRH_w,  "stxrh",  "'Ws, 'Wt")                   \
  V(STXR_w,   "stxr",   "'Ws, 'Wt")                   \
  V(STXR_x,   "stxr",   "'Ws, 'Xt")                   \
  V(LDXRB_w,  "ldxrb",  "'Wt")                        \
  V(LDXRH_w,  "ldxrh",  "'Wt")                        \
  V(LDXR_w,   "ldxr",   "'Wt")                        \
  V(LDXR_x,   "ldxr",   "'Xt")                        \
  V(STXP_w,   "stxp",   "'Ws, 'Wt, 'Wt2")             \
  V(STXP_x,   "stxp",   "'Ws, 'Xt, 'Xt2")             \
  V(LDXP_w,   "ldxp",   "'Wt, 'Wt2")                  \
  V(LDXP_x,   "ldxp",   "'Xt, 'Xt2")                  \
  V(STLXRB_w, "stlxrb", "'Ws, 'Wt")                   \
  V(STLXRH_w, "stlxrh", "'Ws, 'Wt")                   \
  V(STLXR_w,  "stlxr",  "'Ws, 'Wt")                   \
  V(STLXR_x,  "stlxr",  "'Ws, 'Xt")                   \
  V(LDAXRB_w, "ldaxrb", "'Wt")                        \
  V(LDAXRH_w, "ldaxrh", "'Wt")                        \
  V(LDAXR_w,  "ldaxr",  "'Wt")                        \
  V(LDAXR_x,  "ldaxr",  "'Xt")                        \
  V(STLXP_w,  "stlxp",  "'Ws, 'Wt, 'Wt2")             \
  V(STLXP_x,  "stlxp",  "'Ws, 'Xt, 'Xt2")             \
  V(LDAXP_w,  "ldaxp",  "'Wt, 'Wt2")                  \
  V(LDAXP_x,  "ldaxp",  "'Xt, 'Xt2")                  \
  V(STLRB_w,  "stlrb",  "'Wt")                        \
  V(STLRH_w,  "stlrh",  "'Wt")                        \
  V(STLR_w,   "stlr",   "'Wt")                        \
  V(STLR_x,   "stlr",   "'Xt")                        \
  V(LDARB_w,  "ldarb",  "'Wt")                        \
  V(LDARH_w,  "ldarh",  "'Wt")                        \
  V(LDAR_w,   "ldar",   "'Wt")                        \
  V(LDAR_x,   "ldar",   "'Xt")                        \
  V(STLLRB,   "stllrb", "'Wt")                        \
  V(STLLRH,   "stllrh", "'Wt")                        \
  V(STLLR_w,  "stllr",  "'Wt")                        \
  V(STLLR_x,  "stllr",  "'Xt")                        \
  V(LDLARB,   "ldlarb", "'Wt")                        \
  V(LDLARH,   "ldlarh", "'Wt")                        \
  V(LDLAR_w,  "ldlar",  "'Wt")                        \
  V(LDLAR_x,  "ldlar",  "'Xt")                        \
  V(CAS_w,    "cas",    "'Ws, 'Wt")                   \
  V(CAS_x,    "cas",    "'Xs, 'Xt")                   \
  V(CASA_w,   "casa",   "'Ws, 'Wt")                   \
  V(CASA_x,   "casa",   "'Xs, 'Xt")                   \
  V(CASL_w,   "casl",   "'Ws, 'Wt")                   \
  V(CASL_x,   "casl",   "'Xs, 'Xt")                   \
  V(CASAL_w,  "casal",  "'Ws, 'Wt")                   \
  V(CASAL_x,  "casal",  "'Xs, 'Xt")                   \
  V(CASB,     "casb",   "'Ws, 'Wt")                   \
  V(CASAB,    "casab",  "'Ws, 'Wt")                   \
  V(CASLB,    "caslb",  "'Ws, 'Wt")                   \
  V(CASALB,   "casalb", "'Ws, 'Wt")                   \
  V(CASH,     "cash",   "'Ws, 'Wt")                   \
  V(CASAH,    "casah",  "'Ws, 'Wt")                   \
  V(CASLH,    "caslh",  "'Ws, 'Wt")                   \
  V(CASALH,   "casalh", "'Ws, 'Wt")                   \
  V(CASP_w,   "casp",   "'Ws, 'W(s+1), 'Wt, 'W(t+1)") \
  V(CASP_x,   "casp",   "'Xs, 'X(s+1), 'Xt, 'X(t+1)") \
  V(CASPA_w,  "caspa",  "'Ws, 'W(s+1), 'Wt, 'W(t+1)") \
  V(CASPA_x,  "caspa",  "'Xs, 'X(s+1), 'Xt, 'X(t+1)") \
  V(CASPL_w,  "caspl",  "'Ws, 'W(s+1), 'Wt, 'W(t+1)") \
  V(CASPL_x,  "caspl",  "'Xs, 'X(s+1), 'Xt, 'X(t+1)") \
  V(CASPAL_w, "caspal", "'Ws, 'W(s+1), 'Wt, 'W(t+1)") \
  V(CASPAL_x, "caspal", "'Xs, 'X(s+1), 'Xt, 'X(t+1)")
// clang-format on


void Disassembler::VisitLoadStoreExclusive(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form;

  switch (instr->Mask(LoadStoreExclusiveMask)) {
#define LSX(A, B, C)     \
  case A:                \
    mnemonic = B;        \
    form = C ", ['Xns]"; \
    break;
    LOAD_STORE_EXCLUSIVE_LIST(LSX)
#undef LSX
    default:
      form = "(LoadStoreExclusive)";
  }

  switch (instr->Mask(LoadStoreExclusiveMask)) {
    case CASP_w:
    case CASP_x:
    case CASPA_w:
    case CASPA_x:
    case CASPL_w:
    case CASPL_x:
    case CASPAL_w:
    case CASPAL_x:
      if ((instr->GetRs() % 2 == 1) || (instr->GetRt() % 2 == 1)) {
        mnemonic = "unallocated";
        form = "(LoadStoreExclusive)";
      }
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitLoadStorePAC(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(LoadStorePAC)";

  switch (instr->Mask(LoadStorePACMask)) {
    case LDRAA:
      mnemonic = "ldraa";
      form = "'Xt, ['Xns'ILA]";
      break;
    case LDRAB:
      mnemonic = "ldrab";
      form = "'Xt, ['Xns'ILA]";
      break;
    case LDRAA_pre:
      mnemonic = "ldraa";
      form = "'Xt, ['Xns'ILA]!";
      break;
    case LDRAB_pre:
      mnemonic = "ldrab";
      form = "'Xt, ['Xns'ILA]!";
      break;
  }

  Format(instr, mnemonic, form);
}

#define ATOMIC_MEMORY_SIMPLE_LIST(V) \
  V(LDADD, "add")                    \
  V(LDCLR, "clr")                    \
  V(LDEOR, "eor")                    \
  V(LDSET, "set")                    \
  V(LDSMAX, "smax")                  \
  V(LDSMIN, "smin")                  \
  V(LDUMAX, "umax")                  \
  V(LDUMIN, "umin")

void Disassembler::VisitAtomicMemory(const Instruction *instr) {
  const int kMaxAtomicOpMnemonicLength = 16;
  const char *mnemonic;
  const char *form = "'Ws, 'Wt, ['Xns]";

  switch (instr->Mask(AtomicMemoryMask)) {
#define AMS(A, MN)             \
  case A##B:                   \
    mnemonic = MN "b";         \
    break;                     \
  case A##AB:                  \
    mnemonic = MN "ab";        \
    break;                     \
  case A##LB:                  \
    mnemonic = MN "lb";        \
    break;                     \
  case A##ALB:                 \
    mnemonic = MN "alb";       \
    break;                     \
  case A##H:                   \
    mnemonic = MN "h";         \
    break;                     \
  case A##AH:                  \
    mnemonic = MN "ah";        \
    break;                     \
  case A##LH:                  \
    mnemonic = MN "lh";        \
    break;                     \
  case A##ALH:                 \
    mnemonic = MN "alh";       \
    break;                     \
  case A##_w:                  \
    mnemonic = MN;             \
    break;                     \
  case A##A_w:                 \
    mnemonic = MN "a";         \
    break;                     \
  case A##L_w:                 \
    mnemonic = MN "l";         \
    break;                     \
  case A##AL_w:                \
    mnemonic = MN "al";        \
    break;                     \
  case A##_x:                  \
    mnemonic = MN;             \
    form = "'Xs, 'Xt, ['Xns]"; \
    break;                     \
  case A##A_x:                 \
    mnemonic = MN "a";         \
    form = "'Xs, 'Xt, ['Xns]"; \
    break;                     \
  case A##L_x:                 \
    mnemonic = MN "l";         \
    form = "'Xs, 'Xt, ['Xns]"; \
    break;                     \
  case A##AL_x:                \
    mnemonic = MN "al";        \
    form = "'Xs, 'Xt, ['Xns]"; \
    break;
    ATOMIC_MEMORY_SIMPLE_LIST(AMS)

    // SWP has the same semantics as ldadd etc but without the store aliases.
    AMS(SWP, "swp")
#undef AMS

    case LDAPRB:
      mnemonic = "ldaprb";
      form = "'Wt, ['Xns]";
      break;
    case LDAPRH:
      mnemonic = "ldaprh";
      form = "'Wt, ['Xns]";
      break;
    case LDAPR_w:
      mnemonic = "ldapr";
      form = "'Wt, ['Xns]";
      break;
    case LDAPR_x:
      mnemonic = "ldapr";
      form = "'Xt, ['Xns]";
      break;
    default:
      mnemonic = "unimplemented";
      form = "(AtomicMemory)";
  }

  const char *prefix = "";
  switch (instr->Mask(AtomicMemoryMask)) {
#define AMS(A, MN)                   \
  case A##AB:                        \
  case A##ALB:                       \
  case A##AH:                        \
  case A##ALH:                       \
  case A##A_w:                       \
  case A##AL_w:                      \
  case A##A_x:                       \
  case A##AL_x:                      \
    prefix = "ld";                   \
    break;                           \
  case A##B:                         \
  case A##LB:                        \
  case A##H:                         \
  case A##LH:                        \
  case A##_w:                        \
  case A##L_w: {                     \
    prefix = "ld";                   \
    unsigned rt = instr->GetRt();    \
    if (Register(rt, 32).IsZero()) { \
      prefix = "st";                 \
      form = "'Ws, ['Xns]";          \
    }                                \
    break;                           \
  }                                  \
  case A##_x:                        \
  case A##L_x: {                     \
    prefix = "ld";                   \
    unsigned rt = instr->GetRt();    \
    if (Register(rt, 64).IsZero()) { \
      prefix = "st";                 \
      form = "'Xs, ['Xns]";          \
    }                                \
    break;                           \
  }
    ATOMIC_MEMORY_SIMPLE_LIST(AMS)
#undef AMS
  }

  char buffer[kMaxAtomicOpMnemonicLength];
  if (strlen(prefix) > 0) {
    snprintf(buffer, kMaxAtomicOpMnemonicLength, "%s%s", prefix, mnemonic);
    mnemonic = buffer;
  }

  Format(instr, mnemonic, form);
}


void Disassembler::VisitFPCompare(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Fn, 'Fm";
  const char *form_zero = "'Fn, #0.0";

  switch (instr->Mask(FPCompareMask)) {
    case FCMP_h_zero:
    case FCMP_s_zero:
    case FCMP_d_zero:
      form = form_zero;
      VIXL_FALLTHROUGH();
    case FCMP_h:
    case FCMP_s:
    case FCMP_d:
      mnemonic = "fcmp";
      break;
    case FCMPE_h_zero:
    case FCMPE_s_zero:
    case FCMPE_d_zero:
      form = form_zero;
      VIXL_FALLTHROUGH();
    case FCMPE_h:
    case FCMPE_s:
    case FCMPE_d:
      mnemonic = "fcmpe";
      break;
    default:
      form = "(FPCompare)";
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitFPConditionalCompare(const Instruction *instr) {
  const char *mnemonic = "unmplemented";
  const char *form = "'Fn, 'Fm, 'INzcv, 'Cond";

  switch (instr->Mask(FPConditionalCompareMask)) {
    case FCCMP_h:
    case FCCMP_s:
    case FCCMP_d:
      mnemonic = "fccmp";
      break;
    case FCCMPE_h:
    case FCCMPE_s:
    case FCCMPE_d:
      mnemonic = "fccmpe";
      break;
    default:
      form = "(FPConditionalCompare)";
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitFPConditionalSelect(const Instruction *instr) {
  const char *mnemonic = "";
  const char *form = "'Fd, 'Fn, 'Fm, 'Cond";

  switch (instr->Mask(FPConditionalSelectMask)) {
    case FCSEL_h:
    case FCSEL_s:
    case FCSEL_d:
      mnemonic = "fcsel";
      break;
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitFPDataProcessing1Source(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Fd, 'Fn";

  switch (instr->Mask(FPDataProcessing1SourceMask)) {
#define FORMAT(A, B) \
  case A##_h:        \
  case A##_s:        \
  case A##_d:        \
    mnemonic = B;    \
    break;
    FORMAT(FMOV, "fmov");
    FORMAT(FABS, "fabs");
    FORMAT(FNEG, "fneg");
    FORMAT(FSQRT, "fsqrt");
    FORMAT(FRINTN, "frintn");
    FORMAT(FRINTP, "frintp");
    FORMAT(FRINTM, "frintm");
    FORMAT(FRINTZ, "frintz");
    FORMAT(FRINTA, "frinta");
    FORMAT(FRINTX, "frintx");
    FORMAT(FRINTI, "frinti");
#undef FORMAT
    case FCVT_ds:
      mnemonic = "fcvt";
      form = "'Dd, 'Sn";
      break;
    case FCVT_sd:
      mnemonic = "fcvt";
      form = "'Sd, 'Dn";
      break;
    case FCVT_hs:
      mnemonic = "fcvt";
      form = "'Hd, 'Sn";
      break;
    case FCVT_sh:
      mnemonic = "fcvt";
      form = "'Sd, 'Hn";
      break;
    case FCVT_dh:
      mnemonic = "fcvt";
      form = "'Dd, 'Hn";
      break;
    case FCVT_hd:
      mnemonic = "fcvt";
      form = "'Hd, 'Dn";
      break;
    default:
      form = "(FPDataProcessing1Source)";
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitFPDataProcessing2Source(const Instruction *instr) {
  const char *mnemonic = "";
  const char *form = "'Fd, 'Fn, 'Fm";

  switch (instr->Mask(FPDataProcessing2SourceMask)) {
#define FORMAT(A, B) \
  case A##_h:        \
  case A##_s:        \
  case A##_d:        \
    mnemonic = B;    \
    break;
    FORMAT(FADD, "fadd");
    FORMAT(FSUB, "fsub");
    FORMAT(FMUL, "fmul");
    FORMAT(FDIV, "fdiv");
    FORMAT(FMAX, "fmax");
    FORMAT(FMIN, "fmin");
    FORMAT(FMAXNM, "fmaxnm");
    FORMAT(FMINNM, "fminnm");
    FORMAT(FNMUL, "fnmul");
#undef FORMAT
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitFPDataProcessing3Source(const Instruction *instr) {
  const char *mnemonic = "";
  const char *form = "'Fd, 'Fn, 'Fm, 'Fa";

  switch (instr->Mask(FPDataProcessing3SourceMask)) {
#define FORMAT(A, B) \
  case A##_h:        \
  case A##_s:        \
  case A##_d:        \
    mnemonic = B;    \
    break;
    FORMAT(FMADD, "fmadd");
    FORMAT(FMSUB, "fmsub");
    FORMAT(FNMADD, "fnmadd");
    FORMAT(FNMSUB, "fnmsub");
#undef FORMAT
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitFPImmediate(const Instruction *instr) {
  const char *mnemonic = "";
  const char *form = "(FPImmediate)";
  switch (instr->Mask(FPImmediateMask)) {
    case FMOV_h_imm:
      mnemonic = "fmov";
      form = "'Hd, 'IFPHalf";
      break;
    case FMOV_s_imm:
      mnemonic = "fmov";
      form = "'Sd, 'IFPSingle";
      break;
    case FMOV_d_imm:
      mnemonic = "fmov";
      form = "'Dd, 'IFPDouble";
      break;
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitFPIntegerConvert(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(FPIntegerConvert)";
  const char *form_rf = "'Rd, 'Fn";
  const char *form_fr = "'Fd, 'Rn";

  switch (instr->Mask(FPIntegerConvertMask)) {
    case FMOV_wh:
    case FMOV_xh:
    case FMOV_ws:
    case FMOV_xd:
      mnemonic = "fmov";
      form = form_rf;
      break;
    case FMOV_hw:
    case FMOV_hx:
    case FMOV_sw:
    case FMOV_dx:
      mnemonic = "fmov";
      form = form_fr;
      break;
    case FMOV_d1_x:
      mnemonic = "fmov";
      form = "'Vd.D[1], 'Rn";
      break;
    case FMOV_x_d1:
      mnemonic = "fmov";
      form = "'Rd, 'Vn.D[1]";
      break;
    case FCVTAS_wh:
    case FCVTAS_xh:
    case FCVTAS_ws:
    case FCVTAS_xs:
    case FCVTAS_wd:
    case FCVTAS_xd:
      mnemonic = "fcvtas";
      form = form_rf;
      break;
    case FCVTAU_wh:
    case FCVTAU_xh:
    case FCVTAU_ws:
    case FCVTAU_xs:
    case FCVTAU_wd:
    case FCVTAU_xd:
      mnemonic = "fcvtau";
      form = form_rf;
      break;
    case FCVTMS_wh:
    case FCVTMS_xh:
    case FCVTMS_ws:
    case FCVTMS_xs:
    case FCVTMS_wd:
    case FCVTMS_xd:
      mnemonic = "fcvtms";
      form = form_rf;
      break;
    case FCVTMU_wh:
    case FCVTMU_xh:
    case FCVTMU_ws:
    case FCVTMU_xs:
    case FCVTMU_wd:
    case FCVTMU_xd:
      mnemonic = "fcvtmu";
      form = form_rf;
      break;
    case FCVTNS_wh:
    case FCVTNS_xh:
    case FCVTNS_ws:
    case FCVTNS_xs:
    case FCVTNS_wd:
    case FCVTNS_xd:
      mnemonic = "fcvtns";
      form = form_rf;
      break;
    case FCVTNU_wh:
    case FCVTNU_xh:
    case FCVTNU_ws:
    case FCVTNU_xs:
    case FCVTNU_wd:
    case FCVTNU_xd:
      mnemonic = "fcvtnu";
      form = form_rf;
      break;
    case FCVTZU_wh:
    case FCVTZU_xh:
    case FCVTZU_ws:
    case FCVTZU_xs:
    case FCVTZU_wd:
    case FCVTZU_xd:
      mnemonic = "fcvtzu";
      form = form_rf;
      break;
    case FCVTZS_wh:
    case FCVTZS_xh:
    case FCVTZS_ws:
    case FCVTZS_xs:
    case FCVTZS_wd:
    case FCVTZS_xd:
      mnemonic = "fcvtzs";
      form = form_rf;
      break;
    case FCVTPU_wh:
    case FCVTPU_xh:
    case FCVTPU_xs:
    case FCVTPU_wd:
    case FCVTPU_ws:
    case FCVTPU_xd:
      mnemonic = "fcvtpu";
      form = form_rf;
      break;
    case FCVTPS_wh:
    case FCVTPS_xh:
    case FCVTPS_ws:
    case FCVTPS_xs:
    case FCVTPS_wd:
    case FCVTPS_xd:
      mnemonic = "fcvtps";
      form = form_rf;
      break;
    case SCVTF_hw:
    case SCVTF_hx:
    case SCVTF_sw:
    case SCVTF_sx:
    case SCVTF_dw:
    case SCVTF_dx:
      mnemonic = "scvtf";
      form = form_fr;
      break;
    case UCVTF_hw:
    case UCVTF_hx:
    case UCVTF_sw:
    case UCVTF_sx:
    case UCVTF_dw:
    case UCVTF_dx:
      mnemonic = "ucvtf";
      form = form_fr;
      break;
    case FJCVTZS:
      mnemonic = "fjcvtzs";
      form = form_rf;
      break;
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitFPFixedPointConvert(const Instruction *instr) {
  const char *mnemonic = "";
  const char *form = "'Rd, 'Fn, 'IFPFBits";
  const char *form_fr = "'Fd, 'Rn, 'IFPFBits";

  switch (instr->Mask(FPFixedPointConvertMask)) {
    case FCVTZS_wh_fixed:
    case FCVTZS_xh_fixed:
    case FCVTZS_ws_fixed:
    case FCVTZS_xs_fixed:
    case FCVTZS_wd_fixed:
    case FCVTZS_xd_fixed:
      mnemonic = "fcvtzs";
      break;
    case FCVTZU_wh_fixed:
    case FCVTZU_xh_fixed:
    case FCVTZU_ws_fixed:
    case FCVTZU_xs_fixed:
    case FCVTZU_wd_fixed:
    case FCVTZU_xd_fixed:
      mnemonic = "fcvtzu";
      break;
    case SCVTF_hw_fixed:
    case SCVTF_hx_fixed:
    case SCVTF_sw_fixed:
    case SCVTF_sx_fixed:
    case SCVTF_dw_fixed:
    case SCVTF_dx_fixed:
      mnemonic = "scvtf";
      form = form_fr;
      break;
    case UCVTF_hw_fixed:
    case UCVTF_hx_fixed:
    case UCVTF_sw_fixed:
    case UCVTF_sx_fixed:
    case UCVTF_dw_fixed:
    case UCVTF_dx_fixed:
      mnemonic = "ucvtf";
      form = form_fr;
      break;
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}

// clang-format off
#define PAUTH_SYSTEM_MNEMONICS(V) \
  V(PACIA1716, "pacia1716")       \
  V(PACIB1716, "pacib1716")       \
  V(AUTIA1716, "autia1716")       \
  V(AUTIB1716, "autib1716")       \
  V(PACIAZ,    "paciaz")          \
  V(PACIASP,   "paciasp")         \
  V(PACIBZ,    "pacibz")          \
  V(PACIBSP,   "pacibsp")         \
  V(AUTIAZ,    "autiaz")          \
  V(AUTIASP,   "autiasp")         \
  V(AUTIBZ,    "autibz")          \
  V(AUTIBSP,   "autibsp")
// clang-format on

void Disassembler::VisitSystem(const Instruction *instr) {
  // Some system instructions hijack their Op and Cp fields to represent a
  // range of immediates instead of indicating a different instruction. This
  // makes the decoding tricky.
  const char *mnemonic = "unimplemented";
  const char *form = "(System)";
  if (instr->GetInstructionBits() == XPACLRI) {
    mnemonic = "xpaclri";
    form = NULL;
  } else if (instr->Mask(SystemPStateFMask) == SystemPStateFixed) {
    switch (instr->Mask(SystemPStateMask)) {
      case CFINV:
        mnemonic = "cfinv";
        form = NULL;
        break;
      case AXFLAG:
        mnemonic = "axflag";
        form = NULL;
        break;
      case XAFLAG:
        mnemonic = "xaflag";
        form = NULL;
        break;
    }
  } else if (instr->Mask(SystemPAuthFMask) == SystemPAuthFixed) {
    switch (instr->Mask(SystemPAuthMask)) {
#define PAUTH_CASE(NAME, MN) \
  case NAME:                 \
    mnemonic = MN;           \
    form = NULL;             \
    break;

      PAUTH_SYSTEM_MNEMONICS(PAUTH_CASE)
#undef PAUTH_CASE
    }
  } else if (instr->Mask(SystemExclusiveMonitorFMask) ==
             SystemExclusiveMonitorFixed) {
    switch (instr->Mask(SystemExclusiveMonitorMask)) {
      case CLREX: {
        mnemonic = "clrex";
        form = (instr->GetCRm() == 0xf) ? NULL : "'IX";
        break;
      }
    }
  } else if (instr->Mask(SystemSysRegFMask) == SystemSysRegFixed) {
    switch (instr->Mask(SystemSysRegMask)) {
      case MRS: {
        mnemonic = "mrs";
        form = "'Xt, 'IY";
        break;
      }
      case MSR: {
        mnemonic = "msr";
        form = "'IY, 'Xt";
        break;
      }
    }
  } else if (instr->Mask(SystemHintFMask) == SystemHintFixed) {
    form = NULL;
    switch (instr->GetImmHint()) {
      case NOP:
        mnemonic = "nop";
        break;
      case YIELD:
        mnemonic = "yield";
        break;
      case WFE:
        mnemonic = "wfe";
        break;
      case WFI:
        mnemonic = "wfi";
        break;
      case SEV:
        mnemonic = "sev";
        break;
      case SEVL:
        mnemonic = "sevl";
        break;
      case ESB:
        mnemonic = "esb";
        break;
      case CSDB:
        mnemonic = "csdb";
        break;
      case BTI:
        mnemonic = "bti";
        break;
      case BTI_c:
        mnemonic = "bti c";
        break;
      case BTI_j:
        mnemonic = "bti j";
        break;
      case BTI_jc:
        mnemonic = "bti jc";
        break;
      default:
        // Fall back to 'hint #<imm7>'.
        form = "'IH";
        mnemonic = "hint";
        break;
    }
  } else if (instr->Mask(MemBarrierFMask) == MemBarrierFixed) {
    switch (instr->Mask(MemBarrierMask)) {
      case DMB: {
        mnemonic = "dmb";
        form = "'M";
        break;
      }
      case DSB: {
        mnemonic = "dsb";
        form = "'M";
        break;
      }
      case ISB: {
        mnemonic = "isb";
        form = NULL;
        break;
      }
    }
  } else if (instr->Mask(SystemSysFMask) == SystemSysFixed) {
    switch (instr->GetSysOp()) {
      case IVAU:
        mnemonic = "ic";
        form = "ivau, 'Xt";
        break;
      case CVAC:
        mnemonic = "dc";
        form = "cvac, 'Xt";
        break;
      case CVAU:
        mnemonic = "dc";
        form = "cvau, 'Xt";
        break;
      case CVAP:
        mnemonic = "dc";
        form = "cvap, 'Xt";
        break;
      case CVADP:
        mnemonic = "dc";
        form = "cvadp, 'Xt";
        break;
      case CIVAC:
        mnemonic = "dc";
        form = "civac, 'Xt";
        break;
      case ZVA:
        mnemonic = "dc";
        form = "zva, 'Xt";
        break;
      default:
        mnemonic = "sys";
        if (instr->GetRt() == 31) {
          form = "'G1, 'Kn, 'Km, 'G2";
        } else {
          form = "'G1, 'Kn, 'Km, 'G2, 'Xt";
        }
        break;
    }
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitException(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'IDebug";

  switch (instr->Mask(ExceptionMask)) {
    case HLT:
      mnemonic = "hlt";
      break;
    case BRK:
      mnemonic = "brk";
      break;
    case SVC:
      mnemonic = "svc";
      break;
    case HVC:
      mnemonic = "hvc";
      break;
    case SMC:
      mnemonic = "smc";
      break;
    case DCPS1:
      mnemonic = "dcps1";
      form = "{'IDebug}";
      break;
    case DCPS2:
      mnemonic = "dcps2";
      form = "{'IDebug}";
      break;
    case DCPS3:
      mnemonic = "dcps3";
      form = "{'IDebug}";
      break;
    default:
      form = "(Exception)";
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitCrypto2RegSHA(const Instruction *instr) {
  VisitUnimplemented(instr);
}


void Disassembler::VisitCrypto3RegSHA(const Instruction *instr) {
  VisitUnimplemented(instr);
}


void Disassembler::VisitCryptoAES(const Instruction *instr) {
  VisitUnimplemented(instr);
}


void Disassembler::VisitNEON2RegMisc(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Vd.%s, 'Vn.%s";
  const char *form_cmp_zero = "'Vd.%s, 'Vn.%s, #0";
  const char *form_fcmp_zero = "'Vd.%s, 'Vn.%s, #0.0";
  NEONFormatDecoder nfd(instr);

  static const NEONFormatMap map_lp_ta =
      {{23, 22, 30}, {NF_4H, NF_8H, NF_2S, NF_4S, NF_1D, NF_2D}};

  static const NEONFormatMap map_cvt_ta = {{22}, {NF_4S, NF_2D}};

  static const NEONFormatMap map_cvt_tb = {{22, 30},
                                           {NF_4H, NF_8H, NF_2S, NF_4S}};

  if (instr->Mask(NEON2RegMiscOpcode) <= NEON_NEG_opcode) {
    // These instructions all use a two bit size field, except NOT and RBIT,
    // which use the field to encode the operation.
    switch (instr->Mask(NEON2RegMiscMask)) {
      case NEON_REV64:
        mnemonic = "rev64";
        break;
      case NEON_REV32:
        mnemonic = "rev32";
        break;
      case NEON_REV16:
        mnemonic = "rev16";
        break;
      case NEON_SADDLP:
        mnemonic = "saddlp";
        nfd.SetFormatMap(0, &map_lp_ta);
        break;
      case NEON_UADDLP:
        mnemonic = "uaddlp";
        nfd.SetFormatMap(0, &map_lp_ta);
        break;
      case NEON_SUQADD:
        mnemonic = "suqadd";
        break;
      case NEON_USQADD:
        mnemonic = "usqadd";
        break;
      case NEON_CLS:
        mnemonic = "cls";
        break;
      case NEON_CLZ:
        mnemonic = "clz";
        break;
      case NEON_CNT:
        mnemonic = "cnt";
        break;
      case NEON_SADALP:
        mnemonic = "sadalp";
        nfd.SetFormatMap(0, &map_lp_ta);
        break;
      case NEON_UADALP:
        mnemonic = "uadalp";
        nfd.SetFormatMap(0, &map_lp_ta);
        break;
      case NEON_SQABS:
        mnemonic = "sqabs";
        break;
      case NEON_SQNEG:
        mnemonic = "sqneg";
        break;
      case NEON_CMGT_zero:
        mnemonic = "cmgt";
        form = form_cmp_zero;
        break;
      case NEON_CMGE_zero:
        mnemonic = "cmge";
        form = form_cmp_zero;
        break;
      case NEON_CMEQ_zero:
        mnemonic = "cmeq";
        form = form_cmp_zero;
        break;
      case NEON_CMLE_zero:
        mnemonic = "cmle";
        form = form_cmp_zero;
        break;
      case NEON_CMLT_zero:
        mnemonic = "cmlt";
        form = form_cmp_zero;
        break;
      case NEON_ABS:
        mnemonic = "abs";
        break;
      case NEON_NEG:
        mnemonic = "neg";
        break;
      case NEON_RBIT_NOT:
        switch (instr->GetFPType()) {
          case 0:
            mnemonic = "mvn";
            break;
          case 1:
            mnemonic = "rbit";
            break;
          default:
            form = "(NEON2RegMisc)";
        }
        nfd.SetFormatMaps(nfd.LogicalFormatMap());
        break;
    }
  } else {
    // These instructions all use a one bit size field, except XTN, SQXTUN,
    // SHLL, SQXTN and UQXTN, which use a two bit size field.
    nfd.SetFormatMaps(nfd.FPFormatMap());
    switch (instr->Mask(NEON2RegMiscFPMask)) {
      case NEON_FABS:
        mnemonic = "fabs";
        break;
      case NEON_FNEG:
        mnemonic = "fneg";
        break;
      case NEON_FCVTN:
        mnemonic = instr->Mask(NEON_Q) ? "fcvtn2" : "fcvtn";
        nfd.SetFormatMap(0, &map_cvt_tb);
        nfd.SetFormatMap(1, &map_cvt_ta);
        break;
      case NEON_FCVTXN:
        mnemonic = instr->Mask(NEON_Q) ? "fcvtxn2" : "fcvtxn";
        nfd.SetFormatMap(0, &map_cvt_tb);
        nfd.SetFormatMap(1, &map_cvt_ta);
        break;
      case NEON_FCVTL:
        mnemonic = instr->Mask(NEON_Q) ? "fcvtl2" : "fcvtl";
        nfd.SetFormatMap(0, &map_cvt_ta);
        nfd.SetFormatMap(1, &map_cvt_tb);
        break;
      case NEON_FRINTN:
        mnemonic = "frintn";
        break;
      case NEON_FRINTA:
        mnemonic = "frinta";
        break;
      case NEON_FRINTP:
        mnemonic = "frintp";
        break;
      case NEON_FRINTM:
        mnemonic = "frintm";
        break;
      case NEON_FRINTX:
        mnemonic = "frintx";
        break;
      case NEON_FRINTZ:
        mnemonic = "frintz";
        break;
      case NEON_FRINTI:
        mnemonic = "frinti";
        break;
      case NEON_FCVTNS:
        mnemonic = "fcvtns";
        break;
      case NEON_FCVTNU:
        mnemonic = "fcvtnu";
        break;
      case NEON_FCVTPS:
        mnemonic = "fcvtps";
        break;
      case NEON_FCVTPU:
        mnemonic = "fcvtpu";
        break;
      case NEON_FCVTMS:
        mnemonic = "fcvtms";
        break;
      case NEON_FCVTMU:
        mnemonic = "fcvtmu";
        break;
      case NEON_FCVTZS:
        mnemonic = "fcvtzs";
        break;
      case NEON_FCVTZU:
        mnemonic = "fcvtzu";
        break;
      case NEON_FCVTAS:
        mnemonic = "fcvtas";
        break;
      case NEON_FCVTAU:
        mnemonic = "fcvtau";
        break;
      case NEON_FSQRT:
        mnemonic = "fsqrt";
        break;
      case NEON_SCVTF:
        mnemonic = "scvtf";
        break;
      case NEON_UCVTF:
        mnemonic = "ucvtf";
        break;
      case NEON_URSQRTE:
        mnemonic = "ursqrte";
        break;
      case NEON_URECPE:
        mnemonic = "urecpe";
        break;
      case NEON_FRSQRTE:
        mnemonic = "frsqrte";
        break;
      case NEON_FRECPE:
        mnemonic = "frecpe";
        break;
      case NEON_FCMGT_zero:
        mnemonic = "fcmgt";
        form = form_fcmp_zero;
        break;
      case NEON_FCMGE_zero:
        mnemonic = "fcmge";
        form = form_fcmp_zero;
        break;
      case NEON_FCMEQ_zero:
        mnemonic = "fcmeq";
        form = form_fcmp_zero;
        break;
      case NEON_FCMLE_zero:
        mnemonic = "fcmle";
        form = form_fcmp_zero;
        break;
      case NEON_FCMLT_zero:
        mnemonic = "fcmlt";
        form = form_fcmp_zero;
        break;
      default:
        if ((NEON_XTN_opcode <= instr->Mask(NEON2RegMiscOpcode)) &&
            (instr->Mask(NEON2RegMiscOpcode) <= NEON_UQXTN_opcode)) {
          nfd.SetFormatMap(0, nfd.IntegerFormatMap());
          nfd.SetFormatMap(1, nfd.LongIntegerFormatMap());

          switch (instr->Mask(NEON2RegMiscMask)) {
            case NEON_XTN:
              mnemonic = "xtn";
              break;
            case NEON_SQXTN:
              mnemonic = "sqxtn";
              break;
            case NEON_UQXTN:
              mnemonic = "uqxtn";
              break;
            case NEON_SQXTUN:
              mnemonic = "sqxtun";
              break;
            case NEON_SHLL:
              mnemonic = "shll";
              nfd.SetFormatMap(0, nfd.LongIntegerFormatMap());
              nfd.SetFormatMap(1, nfd.IntegerFormatMap());
              switch (instr->GetNEONSize()) {
                case 0:
                  form = "'Vd.%s, 'Vn.%s, #8";
                  break;
                case 1:
                  form = "'Vd.%s, 'Vn.%s, #16";
                  break;
                case 2:
                  form = "'Vd.%s, 'Vn.%s, #32";
                  break;
                default:
                  Format(instr, "unallocated", "(NEON2RegMisc)");
                  return;
              }
          }
          Format(instr, nfd.Mnemonic(mnemonic), nfd.Substitute(form));
          return;
        } else {
          form = "(NEON2RegMisc)";
        }
    }
  }
  Format(instr, mnemonic, nfd.Substitute(form));
}

void Disassembler::VisitNEON2RegMiscFP16(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Vd.%s, 'Vn.%s";
  const char *form_cmp = "'Vd.%s, 'Vn.%s, #0.0";

  static const NEONFormatMap map_half = {{30}, {NF_4H, NF_8H}};
  NEONFormatDecoder nfd(instr, &map_half);

  switch (instr->Mask(NEON2RegMiscFP16Mask)) {
// clang-format off
#define FORMAT(A, B) \
  case NEON_##A##_H: \
    mnemonic = B;    \
    break;
    FORMAT(FABS,    "fabs")
    FORMAT(FCVTAS,  "fcvtas")
    FORMAT(FCVTAU,  "fcvtau")
    FORMAT(FCVTMS,  "fcvtms")
    FORMAT(FCVTMU,  "fcvtmu")
    FORMAT(FCVTNS,  "fcvtns")
    FORMAT(FCVTNU,  "fcvtnu")
    FORMAT(FCVTPS,  "fcvtps")
    FORMAT(FCVTPU,  "fcvtpu")
    FORMAT(FCVTZS,  "fcvtzs")
    FORMAT(FCVTZU,  "fcvtzu")
    FORMAT(FNEG,    "fneg")
    FORMAT(FRECPE,  "frecpe")
    FORMAT(FRINTA,  "frinta")
    FORMAT(FRINTI,  "frinti")
    FORMAT(FRINTM,  "frintm")
    FORMAT(FRINTN,  "frintn")
    FORMAT(FRINTP,  "frintp")
    FORMAT(FRINTX,  "frintx")
    FORMAT(FRINTZ,  "frintz")
    FORMAT(FRSQRTE, "frsqrte")
    FORMAT(FSQRT,   "fsqrt")
    FORMAT(SCVTF,   "scvtf")
    FORMAT(UCVTF,   "ucvtf")
// clang-format on
#undef FORMAT

    case NEON_FCMEQ_H_zero:
      mnemonic = "fcmeq";
      form = form_cmp;
      break;
    case NEON_FCMGT_H_zero:
      mnemonic = "fcmgt";
      form = form_cmp;
      break;
    case NEON_FCMGE_H_zero:
      mnemonic = "fcmge";
      form = form_cmp;
      break;
    case NEON_FCMLT_H_zero:
      mnemonic = "fcmlt";
      form = form_cmp;
      break;
    case NEON_FCMLE_H_zero:
      mnemonic = "fcmle";
      form = form_cmp;
      break;
    default:
      form = "(NEON2RegMiscFP16)";
  }
  Format(instr, mnemonic, nfd.Substitute(form));
}


void Disassembler::VisitNEON3Same(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Vd.%s, 'Vn.%s, 'Vm.%s";
  NEONFormatDecoder nfd(instr);

  if (instr->Mask(NEON3SameLogicalFMask) == NEON3SameLogicalFixed) {
    switch (instr->Mask(NEON3SameLogicalMask)) {
      case NEON_AND:
        mnemonic = "and";
        break;
      case NEON_ORR:
        mnemonic = "orr";
        if (instr->GetRm() == instr->GetRn()) {
          mnemonic = "mov";
          form = "'Vd.%s, 'Vn.%s";
        }
        break;
      case NEON_ORN:
        mnemonic = "orn";
        break;
      case NEON_EOR:
        mnemonic = "eor";
        break;
      case NEON_BIC:
        mnemonic = "bic";
        break;
      case NEON_BIF:
        mnemonic = "bif";
        break;
      case NEON_BIT:
        mnemonic = "bit";
        break;
      case NEON_BSL:
        mnemonic = "bsl";
        break;
      default:
        form = "(NEON3Same)";
    }
    nfd.SetFormatMaps(nfd.LogicalFormatMap());
  } else {
    static const char kUnknown[] = "unallocated";
    static const char *mnemonics[] = {"shadd",
                                      "uhadd",
                                      "shadd",
                                      "uhadd",
                                      "sqadd",
                                      "uqadd",
                                      "sqadd",
                                      "uqadd",
                                      "srhadd",
                                      "urhadd",
                                      "srhadd",
                                      "urhadd",
                                      // Handled by logical cases above.
                                      NULL,
                                      NULL,
                                      NULL,
                                      NULL,
                                      "shsub",
                                      "uhsub",
                                      "shsub",
                                      "uhsub",
                                      "sqsub",
                                      "uqsub",
                                      "sqsub",
                                      "uqsub",
                                      "cmgt",
                                      "cmhi",
                                      "cmgt",
                                      "cmhi",
                                      "cmge",
                                      "cmhs",
                                      "cmge",
                                      "cmhs",
                                      "sshl",
                                      "ushl",
                                      "sshl",
                                      "ushl",
                                      "sqshl",
                                      "uqshl",
                                      "sqshl",
                                      "uqshl",
                                      "srshl",
                                      "urshl",
                                      "srshl",
                                      "urshl",
                                      "sqrshl",
                                      "uqrshl",
                                      "sqrshl",
                                      "uqrshl",
                                      "smax",
                                      "umax",
                                      "smax",
                                      "umax",
                                      "smin",
                                      "umin",
                                      "smin",
                                      "umin",
                                      "sabd",
                                      "uabd",
                                      "sabd",
                                      "uabd",
                                      "saba",
                                      "uaba",
                                      "saba",
                                      "uaba",
                                      "add",
                                      "sub",
                                      "add",
                                      "sub",
                                      "cmtst",
                                      "cmeq",
                                      "cmtst",
                                      "cmeq",
                                      "mla",
                                      "mls",
                                      "mla",
                                      "mls",
                                      "mul",
                                      "pmul",
                                      "mul",
                                      "pmul",
                                      "smaxp",
                                      "umaxp",
                                      "smaxp",
                                      "umaxp",
                                      "sminp",
                                      "uminp",
                                      "sminp",
                                      "uminp",
                                      "sqdmulh",
                                      "sqrdmulh",
                                      "sqdmulh",
                                      "sqrdmulh",
                                      "addp",
                                      kUnknown,
                                      "addp",
                                      kUnknown,
                                      "fmaxnm",
                                      "fmaxnmp",
                                      "fminnm",
                                      "fminnmp",
                                      "fmla",
                                      kUnknown,  // FMLAL2 or unallocated
                                      "fmls",
                                      kUnknown,  // FMLSL2 or unallocated
                                      "fadd",
                                      "faddp",
                                      "fsub",
                                      "fabd",
                                      "fmulx",
                                      "fmul",
                                      kUnknown,
                                      kUnknown,
                                      "fcmeq",
                                      "fcmge",
                                      kUnknown,
                                      "fcmgt",
                                      kUnknown,  // FMLAL or unallocated
                                      "facge",
                                      kUnknown,  // FMLSL or unallocated
                                      "facgt",
                                      "fmax",
                                      "fmaxp",
                                      "fmin",
                                      "fminp",
                                      "frecps",
                                      "fdiv",
                                      "frsqrts",
                                      kUnknown};

    // Operation is determined by the opcode bits (15-11), the top bit of
    // size (23) and the U bit (29).
    unsigned index = (instr->ExtractBits(15, 11) << 2) |
                     (instr->ExtractBit(23) << 1) | instr->ExtractBit(29);
    VIXL_ASSERT(index < ArrayLength(mnemonics));
    mnemonic = mnemonics[index];
    // Assert that index is not one of the previously handled logical
    // instructions.
    VIXL_ASSERT(mnemonic != NULL);

    if (mnemonic == kUnknown) {
      // Catch special cases where we need to check more bits than we have in
      // the table index. Anything not matched here is unallocated.

      const char *fhm_form = (instr->Mask(NEON_Q) == 0)
                                 ? "'Vd.2s, 'Vn.2h, 'Vm.2h"
                                 : "'Vd.4s, 'Vn.4h, 'Vm.4h";
      switch (instr->Mask(NEON3SameFHMMask)) {
        case NEON_FMLAL:
          mnemonic = "fmlal";
          form = fhm_form;
          break;
        case NEON_FMLAL2:
          mnemonic = "fmlal2";
          form = fhm_form;
          break;
        case NEON_FMLSL:
          mnemonic = "fmlsl";
          form = fhm_form;
          break;
        case NEON_FMLSL2:
          mnemonic = "fmlsl2";
          form = fhm_form;
          break;
        default:
          VIXL_ASSERT(strcmp(mnemonic, "unallocated") == 0);
          form = "(NEON3Same)";
          break;
      }
    }

    if (instr->Mask(NEON3SameFPFMask) == NEON3SameFPFixed) {
      nfd.SetFormatMaps(nfd.FPFormatMap());
    }
  }
  Format(instr, mnemonic, nfd.Substitute(form));
}

void Disassembler::VisitNEON3SameFP16(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Vd.%s, 'Vn.%s, 'Vm.%s";

  NEONFormatDecoder nfd(instr);
  nfd.SetFormatMaps(nfd.FP16FormatMap());

  switch (instr->Mask(NEON3SameFP16Mask)) {
#define FORMAT(A, B) \
  case NEON_##A##_H: \
    mnemonic = B;    \
    break;
    FORMAT(FMAXNM, "fmaxnm");
    FORMAT(FMLA, "fmla");
    FORMAT(FADD, "fadd");
    FORMAT(FMULX, "fmulx");
    FORMAT(FCMEQ, "fcmeq");
    FORMAT(FMAX, "fmax");
    FORMAT(FRECPS, "frecps");
    FORMAT(FMINNM, "fminnm");
    FORMAT(FMLS, "fmls");
    FORMAT(FSUB, "fsub");
    FORMAT(FMIN, "fmin");
    FORMAT(FRSQRTS, "frsqrts");
    FORMAT(FMAXNMP, "fmaxnmp");
    FORMAT(FADDP, "faddp");
    FORMAT(FMUL, "fmul");
    FORMAT(FCMGE, "fcmge");
    FORMAT(FACGE, "facge");
    FORMAT(FMAXP, "fmaxp");
    FORMAT(FDIV, "fdiv");
    FORMAT(FMINNMP, "fminnmp");
    FORMAT(FABD, "fabd");
    FORMAT(FCMGT, "fcmgt");
    FORMAT(FACGT, "facgt");
    FORMAT(FMINP, "fminp");
#undef FORMAT
    default:
      form = "(NEON3SameFP16)";
  }

  Format(instr, mnemonic, nfd.Substitute(form));
}

void Disassembler::VisitNEON3SameExtra(const Instruction *instr) {
  static const NEONFormatMap map_usdot = {{30}, {NF_8B, NF_16B}};

  const char *mnemonic = "unallocated";
  const char *form = "(NEON3SameExtra)";

  NEONFormatDecoder nfd(instr);

  if (instr->Mask(NEON3SameExtraFCMLAMask) == NEON_FCMLA) {
    mnemonic = "fcmla";
    form = "'Vd.%s, 'Vn.%s, 'Vm.%s, 'IVFCNM";
  } else if (instr->Mask(NEON3SameExtraFCADDMask) == NEON_FCADD) {
    mnemonic = "fcadd";
    form = "'Vd.%s, 'Vn.%s, 'Vm.%s, 'IVFCNA";
  } else {
    form = "'Vd.%s, 'Vn.%s, 'Vm.%s";
    switch (instr->Mask(NEON3SameExtraMask)) {
      case NEON_SDOT:
        mnemonic = "sdot";
        nfd.SetFormatMap(1, &map_usdot);
        nfd.SetFormatMap(2, &map_usdot);
        break;
      case NEON_SQRDMLAH:
        mnemonic = "sqrdmlah";
        break;
      case NEON_UDOT:
        mnemonic = "udot";
        nfd.SetFormatMap(1, &map_usdot);
        nfd.SetFormatMap(2, &map_usdot);
        break;
      case NEON_SQRDMLSH:
        mnemonic = "sqrdmlsh";
        break;
    }
  }

  Format(instr, mnemonic, nfd.Substitute(form));
}


void Disassembler::VisitNEON3Different(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Vd.%s, 'Vn.%s, 'Vm.%s";

  NEONFormatDecoder nfd(instr);
  nfd.SetFormatMap(0, nfd.LongIntegerFormatMap());

  // Ignore the Q bit. Appending a "2" suffix is handled later.
  switch (instr->Mask(NEON3DifferentMask) & ~NEON_Q) {
    case NEON_PMULL:
      mnemonic = "pmull";
      break;
    case NEON_SABAL:
      mnemonic = "sabal";
      break;
    case NEON_SABDL:
      mnemonic = "sabdl";
      break;
    case NEON_SADDL:
      mnemonic = "saddl";
      break;
    case NEON_SMLAL:
      mnemonic = "smlal";
      break;
    case NEON_SMLSL:
      mnemonic = "smlsl";
      break;
    case NEON_SMULL:
      mnemonic = "smull";
      break;
    case NEON_SSUBL:
      mnemonic = "ssubl";
      break;
    case NEON_SQDMLAL:
      mnemonic = "sqdmlal";
      break;
    case NEON_SQDMLSL:
      mnemonic = "sqdmlsl";
      break;
    case NEON_SQDMULL:
      mnemonic = "sqdmull";
      break;
    case NEON_UABAL:
      mnemonic = "uabal";
      break;
    case NEON_UABDL:
      mnemonic = "uabdl";
      break;
    case NEON_UADDL:
      mnemonic = "uaddl";
      break;
    case NEON_UMLAL:
      mnemonic = "umlal";
      break;
    case NEON_UMLSL:
      mnemonic = "umlsl";
      break;
    case NEON_UMULL:
      mnemonic = "umull";
      break;
    case NEON_USUBL:
      mnemonic = "usubl";
      break;
    case NEON_SADDW:
      mnemonic = "saddw";
      nfd.SetFormatMap(1, nfd.LongIntegerFormatMap());
      break;
    case NEON_SSUBW:
      mnemonic = "ssubw";
      nfd.SetFormatMap(1, nfd.LongIntegerFormatMap());
      break;
    case NEON_UADDW:
      mnemonic = "uaddw";
      nfd.SetFormatMap(1, nfd.LongIntegerFormatMap());
      break;
    case NEON_USUBW:
      mnemonic = "usubw";
      nfd.SetFormatMap(1, nfd.LongIntegerFormatMap());
      break;
    case NEON_ADDHN:
      mnemonic = "addhn";
      nfd.SetFormatMaps(nfd.LongIntegerFormatMap());
      nfd.SetFormatMap(0, nfd.IntegerFormatMap());
      break;
    case NEON_RADDHN:
      mnemonic = "raddhn";
      nfd.SetFormatMaps(nfd.LongIntegerFormatMap());
      nfd.SetFormatMap(0, nfd.IntegerFormatMap());
      break;
    case NEON_RSUBHN:
      mnemonic = "rsubhn";
      nfd.SetFormatMaps(nfd.LongIntegerFormatMap());
      nfd.SetFormatMap(0, nfd.IntegerFormatMap());
      break;
    case NEON_SUBHN:
      mnemonic = "subhn";
      nfd.SetFormatMaps(nfd.LongIntegerFormatMap());
      nfd.SetFormatMap(0, nfd.IntegerFormatMap());
      break;
    default:
      form = "(NEON3Different)";
  }
  Format(instr, nfd.Mnemonic(mnemonic), nfd.Substitute(form));
}


void Disassembler::VisitNEONAcrossLanes(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "%sd, 'Vn.%s";
  const char *form_half = "'Hd, 'Vn.%s";
  bool half_op = false;
  static const NEONFormatMap map_half = {{30}, {NF_4H, NF_8H}};

  NEONFormatDecoder nfd(instr,
                        NEONFormatDecoder::ScalarFormatMap(),
                        NEONFormatDecoder::IntegerFormatMap());

  if (instr->Mask(NEONAcrossLanesFP16FMask) == NEONAcrossLanesFP16Fixed) {
    half_op = true;
    form = form_half;
    nfd.SetFormatMaps(&map_half);
    switch (instr->Mask(NEONAcrossLanesFP16Mask)) {
      case NEON_FMAXV_H:
        mnemonic = "fmaxv";
        break;
      case NEON_FMINV_H:
        mnemonic = "fminv";
        break;
      case NEON_FMAXNMV_H:
        mnemonic = "fmaxnmv";
        break;
      case NEON_FMINNMV_H:
        mnemonic = "fminnmv";
        break;
    }
  } else if (instr->Mask(NEONAcrossLanesFPFMask) == NEONAcrossLanesFPFixed) {
    nfd.SetFormatMap(0, nfd.FPScalarFormatMap());
    nfd.SetFormatMap(1, nfd.FPFormatMap());
    switch (instr->Mask(NEONAcrossLanesFPMask)) {
      case NEON_FMAXV:
        mnemonic = "fmaxv";
        break;
      case NEON_FMINV:
        mnemonic = "fminv";
        break;
      case NEON_FMAXNMV:
        mnemonic = "fmaxnmv";
        break;
      case NEON_FMINNMV:
        mnemonic = "fminnmv";
        break;
      default:
        form = "(NEONAcrossLanes)";
        break;
    }
  } else if (instr->Mask(NEONAcrossLanesFMask) == NEONAcrossLanesFixed) {
    switch (instr->Mask(NEONAcrossLanesMask)) {
      case NEON_ADDV:
        mnemonic = "addv";
        break;
      case NEON_SMAXV:
        mnemonic = "smaxv";
        break;
      case NEON_SMINV:
        mnemonic = "sminv";
        break;
      case NEON_UMAXV:
        mnemonic = "umaxv";
        break;
      case NEON_UMINV:
        mnemonic = "uminv";
        break;
      case NEON_SADDLV:
        mnemonic = "saddlv";
        nfd.SetFormatMap(0, nfd.LongScalarFormatMap());
        break;
      case NEON_UADDLV:
        mnemonic = "uaddlv";
        nfd.SetFormatMap(0, nfd.LongScalarFormatMap());
        break;
      default:
        form = "(NEONAcrossLanes)";
        break;
    }
  }

  if (half_op) {
    Format(instr, mnemonic, nfd.Substitute(form));
  } else {
    Format(instr,
           mnemonic,
           nfd.Substitute(form,
                          NEONFormatDecoder::kPlaceholder,
                          NEONFormatDecoder::kFormat));
  }
}


void Disassembler::VisitNEONByIndexedElement(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  bool l_instr = false;
  bool fp_instr = false;
  bool cn_instr = false;
  bool half_instr = false;
  bool fhm_instr = false;  // FMLAL{2}, FMLSL{2}

  const char *form = "'Vd.%s, 'Vn.%s, 'Ve.%s['IVByElemIndex]";

  static const NEONFormatMap map_ta = {{23, 22}, {NF_UNDEF, NF_4S, NF_2D}};
  static const NEONFormatMap map_cn =
      {{23, 22, 30},
       {NF_UNDEF, NF_UNDEF, NF_4H, NF_8H, NF_UNDEF, NF_4S, NF_UNDEF, NF_UNDEF}};
  static const NEONFormatMap map_usdot = {{30}, {NF_8B, NF_16B}};
  static const NEONFormatMap map_half = {{30}, {NF_4H, NF_8H}};

  NEONFormatDecoder nfd(instr,
                        &map_ta,
                        NEONFormatDecoder::IntegerFormatMap(),
                        NEONFormatDecoder::ScalarFormatMap());

  switch (instr->Mask(NEONByIndexedElementMask)) {
    case NEON_SMULL_byelement:
      mnemonic = "smull";
      l_instr = true;
      break;
    case NEON_UMULL_byelement:
      mnemonic = "umull";
      l_instr = true;
      break;
    case NEON_SMLAL_byelement:
      mnemonic = "smlal";
      l_instr = true;
      break;
    case NEON_UMLAL_byelement:
      mnemonic = "umlal";
      l_instr = true;
      break;
    case NEON_SMLSL_byelement:
      mnemonic = "smlsl";
      l_instr = true;
      break;
    case NEON_UMLSL_byelement:
      mnemonic = "umlsl";
      l_instr = true;
      break;
    case NEON_SQDMULL_byelement:
      mnemonic = "sqdmull";
      l_instr = true;
      break;
    case NEON_SQDMLAL_byelement:
      mnemonic = "sqdmlal";
      l_instr = true;
      break;
    case NEON_SQDMLSL_byelement:
      mnemonic = "sqdmlsl";
      l_instr = true;
      break;
    case NEON_MUL_byelement:
      mnemonic = "mul";
      break;
    case NEON_MLA_byelement:
      mnemonic = "mla";
      break;
    case NEON_MLS_byelement:
      mnemonic = "mls";
      break;
    case NEON_SQDMULH_byelement:
      mnemonic = "sqdmulh";
      break;
    case NEON_SQRDMULH_byelement:
      mnemonic = "sqrdmulh";
      break;
    case NEON_SDOT_byelement:
      mnemonic = "sdot";
      form = "'Vd.%s, 'Vn.%s, 'Ve.4b['IVByElemIndex]";
      nfd.SetFormatMap(1, &map_usdot);
      break;
    case NEON_SQRDMLAH_byelement:
      mnemonic = "sqrdmlah";
      break;
    case NEON_UDOT_byelement:
      mnemonic = "udot";
      form = "'Vd.%s, 'Vn.%s, 'Ve.4b['IVByElemIndex]";
      nfd.SetFormatMap(1, &map_usdot);
      break;
    case NEON_SQRDMLSH_byelement:
      mnemonic = "sqrdmlsh";
      break;
    default: {
      switch (instr->Mask(NEONByIndexedElementFPLongMask)) {
        case NEON_FMLAL_H_byelement:
          mnemonic = "fmlal";
          fhm_instr = true;
          break;
        case NEON_FMLAL2_H_byelement:
          mnemonic = "fmlal2";
          fhm_instr = true;
          break;
        case NEON_FMLSL_H_byelement:
          mnemonic = "fmlsl";
          fhm_instr = true;
          break;
        case NEON_FMLSL2_H_byelement:
          mnemonic = "fmlsl2";
          fhm_instr = true;
          break;
        default:
          switch (instr->Mask(NEONByIndexedElementFPMask)) {
            case NEON_FMUL_byelement:
              mnemonic = "fmul";
              fp_instr = true;
              break;
            case NEON_FMLA_byelement:
              mnemonic = "fmla";
              fp_instr = true;
              break;
            case NEON_FMLS_byelement:
              mnemonic = "fmls";
              fp_instr = true;
              break;
            case NEON_FMULX_byelement:
              mnemonic = "fmulx";
              fp_instr = true;
              break;
            case NEON_FMLA_H_byelement:
              mnemonic = "fmla";
              half_instr = true;
              break;
            case NEON_FMLS_H_byelement:
              mnemonic = "fmls";
              half_instr = true;
              break;
            case NEON_FMUL_H_byelement:
              mnemonic = "fmul";
              half_instr = true;
              break;
            case NEON_FMULX_H_byelement:
              mnemonic = "fmulx";
              half_instr = true;
              break;
            default:
              switch (instr->Mask(NEONByIndexedElementFPComplexMask)) {
                case NEON_FCMLA_byelement:
                  mnemonic = "fcmla";
                  cn_instr = true;
                  form = "'Vd.%s, 'Vn.%s, 'Ve.%s['IVByElemIndexRot], 'ILFCNR";
                  break;
              }
          }
      }
    }
  }

  if (fhm_instr) {
    // These are oddballs. Set the format manually.
    form = (instr->Mask(NEON_Q) == 0)
               ? "'Vd.2s, 'Vn.2h, 'Ve.h['IVByElemIndexFHM]"
               : "'Vd.4s, 'Vn.4h, 'Ve.h['IVByElemIndexFHM]";
    Format(instr, mnemonic, nfd.Substitute(form));
  } else if (half_instr) {
    form = "'Vd.%s, 'Vn.%s, 'Ve.h['IVByElemIndex]";
    nfd.SetFormatMaps(&map_half, &map_half);
    Format(instr, mnemonic, nfd.Substitute(form));
  } else if (l_instr) {
    Format(instr, nfd.Mnemonic(mnemonic), nfd.Substitute(form));
  } else if (fp_instr) {
    nfd.SetFormatMap(0, nfd.FPFormatMap());
    Format(instr, mnemonic, nfd.Substitute(form));
  } else if (cn_instr) {
    nfd.SetFormatMap(0, &map_cn);
    nfd.SetFormatMap(1, &map_cn);
    Format(instr, mnemonic, nfd.Substitute(form));
  } else {
    nfd.SetFormatMap(0, nfd.IntegerFormatMap());
    Format(instr, mnemonic, nfd.Substitute(form));
  }
}


void Disassembler::VisitNEONCopy(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(NEONCopy)";

  NEONFormatDecoder nfd(instr,
                        NEONFormatDecoder::TriangularFormatMap(),
                        NEONFormatDecoder::TriangularScalarFormatMap());

  if (instr->Mask(NEONCopyInsElementMask) == NEON_INS_ELEMENT) {
    mnemonic = "mov";
    nfd.SetFormatMap(0, nfd.TriangularScalarFormatMap());
    form = "'Vd.%s['IVInsIndex1], 'Vn.%s['IVInsIndex2]";
  } else if (instr->Mask(NEONCopyInsGeneralMask) == NEON_INS_GENERAL) {
    mnemonic = "mov";
    nfd.SetFormatMap(0, nfd.TriangularScalarFormatMap());
    if (nfd.GetVectorFormat() == kFormatD) {
      form = "'Vd.%s['IVInsIndex1], 'Xn";
    } else {
      form = "'Vd.%s['IVInsIndex1], 'Wn";
    }
  } else if (instr->Mask(NEONCopyUmovMask) == NEON_UMOV) {
    if (instr->Mask(NEON_Q) || ((instr->GetImmNEON5() & 7) == 4)) {
      mnemonic = "mov";
    } else {
      mnemonic = "umov";
    }
    nfd.SetFormatMap(0, nfd.TriangularScalarFormatMap());
    if (nfd.GetVectorFormat() == kFormatD) {
      form = "'Xd, 'Vn.%s['IVInsIndex1]";
    } else {
      form = "'Wd, 'Vn.%s['IVInsIndex1]";
    }
  } else if (instr->Mask(NEONCopySmovMask) == NEON_SMOV) {
    mnemonic = "smov";
    nfd.SetFormatMap(0, nfd.TriangularScalarFormatMap());
    form = "'Rdq, 'Vn.%s['IVInsIndex1]";
  } else if (instr->Mask(NEONCopyDupElementMask) == NEON_DUP_ELEMENT) {
    mnemonic = "dup";
    form = "'Vd.%s, 'Vn.%s['IVInsIndex1]";
  } else if (instr->Mask(NEONCopyDupGeneralMask) == NEON_DUP_GENERAL) {
    mnemonic = "dup";
    if (nfd.GetVectorFormat() == kFormat2D) {
      form = "'Vd.%s, 'Xn";
    } else {
      form = "'Vd.%s, 'Wn";
    }
  }
  Format(instr, mnemonic, nfd.Substitute(form));
}


void Disassembler::VisitNEONExtract(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(NEONExtract)";
  NEONFormatDecoder nfd(instr, NEONFormatDecoder::LogicalFormatMap());
  if (instr->Mask(NEONExtractMask) == NEON_EXT) {
    mnemonic = "ext";
    form = "'Vd.%s, 'Vn.%s, 'Vm.%s, 'IVExtract";
  }
  Format(instr, mnemonic, nfd.Substitute(form));
}


void Disassembler::VisitNEONLoadStoreMultiStruct(const Instruction *instr) {
  const char *mnemonic = NULL;
  const char *form = NULL;
  const char *form_1v = "{'Vt.%1$s}, ['Xns]";
  const char *form_2v = "{'Vt.%1$s, 'Vt2.%1$s}, ['Xns]";
  const char *form_3v = "{'Vt.%1$s, 'Vt2.%1$s, 'Vt3.%1$s}, ['Xns]";
  const char *form_4v = "{'Vt.%1$s, 'Vt2.%1$s, 'Vt3.%1$s, 'Vt4.%1$s}, ['Xns]";
  NEONFormatDecoder nfd(instr, NEONFormatDecoder::LoadStoreFormatMap());

  switch (instr->Mask(NEONLoadStoreMultiStructMask)) {
    case NEON_LD1_1v:
      mnemonic = "ld1";
      form = form_1v;
      break;
    case NEON_LD1_2v:
      mnemonic = "ld1";
      form = form_2v;
      break;
    case NEON_LD1_3v:
      mnemonic = "ld1";
      form = form_3v;
      break;
    case NEON_LD1_4v:
      mnemonic = "ld1";
      form = form_4v;
      break;
    case NEON_LD2:
      mnemonic = "ld2";
      form = form_2v;
      break;
    case NEON_LD3:
      mnemonic = "ld3";
      form = form_3v;
      break;
    case NEON_LD4:
      mnemonic = "ld4";
      form = form_4v;
      break;
    case NEON_ST1_1v:
      mnemonic = "st1";
      form = form_1v;
      break;
    case NEON_ST1_2v:
      mnemonic = "st1";
      form = form_2v;
      break;
    case NEON_ST1_3v:
      mnemonic = "st1";
      form = form_3v;
      break;
    case NEON_ST1_4v:
      mnemonic = "st1";
      form = form_4v;
      break;
    case NEON_ST2:
      mnemonic = "st2";
      form = form_2v;
      break;
    case NEON_ST3:
      mnemonic = "st3";
      form = form_3v;
      break;
    case NEON_ST4:
      mnemonic = "st4";
      form = form_4v;
      break;
    default:
      break;
  }

  // Work out unallocated encodings.
  bool allocated = (mnemonic != NULL);
  switch (instr->Mask(NEONLoadStoreMultiStructMask)) {
    case NEON_LD2:
    case NEON_LD3:
    case NEON_LD4:
    case NEON_ST2:
    case NEON_ST3:
    case NEON_ST4:
      // LD[2-4] and ST[2-4] cannot use .1d format.
      allocated = (instr->GetNEONQ() != 0) || (instr->GetNEONLSSize() != 3);
      break;
    default:
      break;
  }
  if (allocated) {
    VIXL_ASSERT(mnemonic != NULL);
    VIXL_ASSERT(form != NULL);
  } else {
    mnemonic = "unallocated";
    form = "(NEONLoadStoreMultiStruct)";
  }

  Format(instr, mnemonic, nfd.Substitute(form));
}


void Disassembler::VisitNEONLoadStoreMultiStructPostIndex(
    const Instruction *instr) {
  const char *mnemonic = NULL;
  const char *form = NULL;
  const char *form_1v = "{'Vt.%1$s}, ['Xns], 'Xmr1";
  const char *form_2v = "{'Vt.%1$s, 'Vt2.%1$s}, ['Xns], 'Xmr2";
  const char *form_3v = "{'Vt.%1$s, 'Vt2.%1$s, 'Vt3.%1$s}, ['Xns], 'Xmr3";
  const char *form_4v =
      "{'Vt.%1$s, 'Vt2.%1$s, 'Vt3.%1$s, 'Vt4.%1$s}, ['Xns], 'Xmr4";
  NEONFormatDecoder nfd(instr, NEONFormatDecoder::LoadStoreFormatMap());

  switch (instr->Mask(NEONLoadStoreMultiStructPostIndexMask)) {
    case NEON_LD1_1v_post:
      mnemonic = "ld1";
      form = form_1v;
      break;
    case NEON_LD1_2v_post:
      mnemonic = "ld1";
      form = form_2v;
      break;
    case NEON_LD1_3v_post:
      mnemonic = "ld1";
      form = form_3v;
      break;
    case NEON_LD1_4v_post:
      mnemonic = "ld1";
      form = form_4v;
      break;
    case NEON_LD2_post:
      mnemonic = "ld2";
      form = form_2v;
      break;
    case NEON_LD3_post:
      mnemonic = "ld3";
      form = form_3v;
      break;
    case NEON_LD4_post:
      mnemonic = "ld4";
      form = form_4v;
      break;
    case NEON_ST1_1v_post:
      mnemonic = "st1";
      form = form_1v;
      break;
    case NEON_ST1_2v_post:
      mnemonic = "st1";
      form = form_2v;
      break;
    case NEON_ST1_3v_post:
      mnemonic = "st1";
      form = form_3v;
      break;
    case NEON_ST1_4v_post:
      mnemonic = "st1";
      form = form_4v;
      break;
    case NEON_ST2_post:
      mnemonic = "st2";
      form = form_2v;
      break;
    case NEON_ST3_post:
      mnemonic = "st3";
      form = form_3v;
      break;
    case NEON_ST4_post:
      mnemonic = "st4";
      form = form_4v;
      break;
    default:
      break;
  }

  // Work out unallocated encodings.
  bool allocated = (mnemonic != NULL);
  switch (instr->Mask(NEONLoadStoreMultiStructPostIndexMask)) {
    case NEON_LD2_post:
    case NEON_LD3_post:
    case NEON_LD4_post:
    case NEON_ST2_post:
    case NEON_ST3_post:
    case NEON_ST4_post:
      // LD[2-4] and ST[2-4] cannot use .1d format.
      allocated = (instr->GetNEONQ() != 0) || (instr->GetNEONLSSize() != 3);
      break;
    default:
      break;
  }
  if (allocated) {
    VIXL_ASSERT(mnemonic != NULL);
    VIXL_ASSERT(form != NULL);
  } else {
    mnemonic = "unallocated";
    form = "(NEONLoadStoreMultiStructPostIndex)";
  }

  Format(instr, mnemonic, nfd.Substitute(form));
}


void Disassembler::VisitNEONLoadStoreSingleStruct(const Instruction *instr) {
  const char *mnemonic = NULL;
  const char *form = NULL;

  const char *form_1b = "{'Vt.b}['IVLSLane0], ['Xns]";
  const char *form_1h = "{'Vt.h}['IVLSLane1], ['Xns]";
  const char *form_1s = "{'Vt.s}['IVLSLane2], ['Xns]";
  const char *form_1d = "{'Vt.d}['IVLSLane3], ['Xns]";
  NEONFormatDecoder nfd(instr, NEONFormatDecoder::LoadStoreFormatMap());

  switch (instr->Mask(NEONLoadStoreSingleStructMask)) {
    case NEON_LD1_b:
      mnemonic = "ld1";
      form = form_1b;
      break;
    case NEON_LD1_h:
      mnemonic = "ld1";
      form = form_1h;
      break;
    case NEON_LD1_s:
      mnemonic = "ld1";
      VIXL_STATIC_ASSERT((NEON_LD1_s | (1 << NEONLSSize_offset)) == NEON_LD1_d);
      form = ((instr->GetNEONLSSize() & 1) == 0) ? form_1s : form_1d;
      break;
    case NEON_ST1_b:
      mnemonic = "st1";
      form = form_1b;
      break;
    case NEON_ST1_h:
      mnemonic = "st1";
      form = form_1h;
      break;
    case NEON_ST1_s:
      mnemonic = "st1";
      VIXL_STATIC_ASSERT((NEON_ST1_s | (1 << NEONLSSize_offset)) == NEON_ST1_d);
      form = ((instr->GetNEONLSSize() & 1) == 0) ? form_1s : form_1d;
      break;
    case NEON_LD1R:
      mnemonic = "ld1r";
      form = "{'Vt.%s}, ['Xns]";
      break;
    case NEON_LD2_b:
    case NEON_ST2_b:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld2" : "st2";
      form = "{'Vt.b, 'Vt2.b}['IVLSLane0], ['Xns]";
      break;
    case NEON_LD2_h:
    case NEON_ST2_h:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld2" : "st2";
      form = "{'Vt.h, 'Vt2.h}['IVLSLane1], ['Xns]";
      break;
    case NEON_LD2_s:
    case NEON_ST2_s:
      VIXL_STATIC_ASSERT((NEON_ST2_s | (1 << NEONLSSize_offset)) == NEON_ST2_d);
      VIXL_STATIC_ASSERT((NEON_LD2_s | (1 << NEONLSSize_offset)) == NEON_LD2_d);
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld2" : "st2";
      if ((instr->GetNEONLSSize() & 1) == 0) {
        form = "{'Vt.s, 'Vt2.s}['IVLSLane2], ['Xns]";
      } else {
        form = "{'Vt.d, 'Vt2.d}['IVLSLane3], ['Xns]";
      }
      break;
    case NEON_LD2R:
      mnemonic = "ld2r";
      form = "{'Vt.%s, 'Vt2.%s}, ['Xns]";
      break;
    case NEON_LD3_b:
    case NEON_ST3_b:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld3" : "st3";
      form = "{'Vt.b, 'Vt2.b, 'Vt3.b}['IVLSLane0], ['Xns]";
      break;
    case NEON_LD3_h:
    case NEON_ST3_h:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld3" : "st3";
      form = "{'Vt.h, 'Vt2.h, 'Vt3.h}['IVLSLane1], ['Xns]";
      break;
    case NEON_LD3_s:
    case NEON_ST3_s:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld3" : "st3";
      if ((instr->GetNEONLSSize() & 1) == 0) {
        form = "{'Vt.s, 'Vt2.s, 'Vt3.s}['IVLSLane2], ['Xns]";
      } else {
        form = "{'Vt.d, 'Vt2.d, 'Vt3.d}['IVLSLane3], ['Xns]";
      }
      break;
    case NEON_LD3R:
      mnemonic = "ld3r";
      form = "{'Vt.%s, 'Vt2.%s, 'Vt3.%s}, ['Xns]";
      break;
    case NEON_LD4_b:
    case NEON_ST4_b:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld4" : "st4";
      form = "{'Vt.b, 'Vt2.b, 'Vt3.b, 'Vt4.b}['IVLSLane0], ['Xns]";
      break;
    case NEON_LD4_h:
    case NEON_ST4_h:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld4" : "st4";
      form = "{'Vt.h, 'Vt2.h, 'Vt3.h, 'Vt4.h}['IVLSLane1], ['Xns]";
      break;
    case NEON_LD4_s:
    case NEON_ST4_s:
      VIXL_STATIC_ASSERT((NEON_LD4_s | (1 << NEONLSSize_offset)) == NEON_LD4_d);
      VIXL_STATIC_ASSERT((NEON_ST4_s | (1 << NEONLSSize_offset)) == NEON_ST4_d);
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld4" : "st4";
      if ((instr->GetNEONLSSize() & 1) == 0) {
        form = "{'Vt.s, 'Vt2.s, 'Vt3.s, 'Vt4.s}['IVLSLane2], ['Xns]";
      } else {
        form = "{'Vt.d, 'Vt2.d, 'Vt3.d, 'Vt4.d}['IVLSLane3], ['Xns]";
      }
      break;
    case NEON_LD4R:
      mnemonic = "ld4r";
      form = "{'Vt.%1$s, 'Vt2.%1$s, 'Vt3.%1$s, 'Vt4.%1$s}, ['Xns]";
      break;
    default:
      break;
  }

  // Work out unallocated encodings.
  bool allocated = (mnemonic != NULL);
  switch (instr->Mask(NEONLoadStoreSingleStructMask)) {
    case NEON_LD1_h:
    case NEON_LD2_h:
    case NEON_LD3_h:
    case NEON_LD4_h:
    case NEON_ST1_h:
    case NEON_ST2_h:
    case NEON_ST3_h:
    case NEON_ST4_h:
      VIXL_ASSERT(allocated);
      allocated = ((instr->GetNEONLSSize() & 1) == 0);
      break;
    case NEON_LD1_s:
    case NEON_LD2_s:
    case NEON_LD3_s:
    case NEON_LD4_s:
    case NEON_ST1_s:
    case NEON_ST2_s:
    case NEON_ST3_s:
    case NEON_ST4_s:
      VIXL_ASSERT(allocated);
      allocated = (instr->GetNEONLSSize() <= 1) &&
                  ((instr->GetNEONLSSize() == 0) || (instr->GetNEONS() == 0));
      break;
    case NEON_LD1R:
    case NEON_LD2R:
    case NEON_LD3R:
    case NEON_LD4R:
      VIXL_ASSERT(allocated);
      allocated = (instr->GetNEONS() == 0);
      break;
    default:
      break;
  }
  if (allocated) {
    VIXL_ASSERT(mnemonic != NULL);
    VIXL_ASSERT(form != NULL);
  } else {
    mnemonic = "unallocated";
    form = "(NEONLoadStoreSingleStruct)";
  }

  Format(instr, mnemonic, nfd.Substitute(form));
}


void Disassembler::VisitNEONLoadStoreSingleStructPostIndex(
    const Instruction *instr) {
  const char *mnemonic = NULL;
  const char *form = NULL;

  const char *form_1b = "{'Vt.b}['IVLSLane0], ['Xns], 'Xmb1";
  const char *form_1h = "{'Vt.h}['IVLSLane1], ['Xns], 'Xmb2";
  const char *form_1s = "{'Vt.s}['IVLSLane2], ['Xns], 'Xmb4";
  const char *form_1d = "{'Vt.d}['IVLSLane3], ['Xns], 'Xmb8";
  NEONFormatDecoder nfd(instr, NEONFormatDecoder::LoadStoreFormatMap());

  switch (instr->Mask(NEONLoadStoreSingleStructPostIndexMask)) {
    case NEON_LD1_b_post:
      mnemonic = "ld1";
      form = form_1b;
      break;
    case NEON_LD1_h_post:
      mnemonic = "ld1";
      form = form_1h;
      break;
    case NEON_LD1_s_post:
      mnemonic = "ld1";
      VIXL_STATIC_ASSERT((NEON_LD1_s | (1 << NEONLSSize_offset)) == NEON_LD1_d);
      form = ((instr->GetNEONLSSize() & 1) == 0) ? form_1s : form_1d;
      break;
    case NEON_ST1_b_post:
      mnemonic = "st1";
      form = form_1b;
      break;
    case NEON_ST1_h_post:
      mnemonic = "st1";
      form = form_1h;
      break;
    case NEON_ST1_s_post:
      mnemonic = "st1";
      VIXL_STATIC_ASSERT((NEON_ST1_s | (1 << NEONLSSize_offset)) == NEON_ST1_d);
      form = ((instr->GetNEONLSSize() & 1) == 0) ? form_1s : form_1d;
      break;
    case NEON_LD1R_post:
      mnemonic = "ld1r";
      form = "{'Vt.%s}, ['Xns], 'Xmz1";
      break;
    case NEON_LD2_b_post:
    case NEON_ST2_b_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld2" : "st2";
      form = "{'Vt.b, 'Vt2.b}['IVLSLane0], ['Xns], 'Xmb2";
      break;
    case NEON_ST2_h_post:
    case NEON_LD2_h_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld2" : "st2";
      form = "{'Vt.h, 'Vt2.h}['IVLSLane1], ['Xns], 'Xmb4";
      break;
    case NEON_LD2_s_post:
    case NEON_ST2_s_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld2" : "st2";
      if ((instr->GetNEONLSSize() & 1) == 0)
        form = "{'Vt.s, 'Vt2.s}['IVLSLane2], ['Xns], 'Xmb8";
      else
        form = "{'Vt.d, 'Vt2.d}['IVLSLane3], ['Xns], 'Xmb16";
      break;
    case NEON_LD2R_post:
      mnemonic = "ld2r";
      form = "{'Vt.%s, 'Vt2.%s}, ['Xns], 'Xmz2";
      break;
    case NEON_LD3_b_post:
    case NEON_ST3_b_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld3" : "st3";
      form = "{'Vt.b, 'Vt2.b, 'Vt3.b}['IVLSLane0], ['Xns], 'Xmb3";
      break;
    case NEON_LD3_h_post:
    case NEON_ST3_h_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld3" : "st3";
      form = "{'Vt.h, 'Vt2.h, 'Vt3.h}['IVLSLane1], ['Xns], 'Xmb6";
      break;
    case NEON_LD3_s_post:
    case NEON_ST3_s_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld3" : "st3";
      if ((instr->GetNEONLSSize() & 1) == 0)
        form = "{'Vt.s, 'Vt2.s, 'Vt3.s}['IVLSLane2], ['Xns], 'Xmb12";
      else
        form = "{'Vt.d, 'Vt2.d, 'Vt3.d}['IVLSLane3], ['Xns], 'Xmb24";
      break;
    case NEON_LD3R_post:
      mnemonic = "ld3r";
      form = "{'Vt.%s, 'Vt2.%s, 'Vt3.%s}, ['Xns], 'Xmz3";
      break;
    case NEON_LD4_b_post:
    case NEON_ST4_b_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld4" : "st4";
      form = "{'Vt.b, 'Vt2.b, 'Vt3.b, 'Vt4.b}['IVLSLane0], ['Xns], 'Xmb4";
      break;
    case NEON_LD4_h_post:
    case NEON_ST4_h_post:
      mnemonic = (instr->GetLdStXLoad()) == 1 ? "ld4" : "st4";
      form = "{'Vt.h, 'Vt2.h, 'Vt3.h, 'Vt4.h}['IVLSLane1], ['Xns], 'Xmb8";
      break;
    case NEON_LD4_s_post:
    case NEON_ST4_s_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld4" : "st4";
      if ((instr->GetNEONLSSize() & 1) == 0)
        form = "{'Vt.s, 'Vt2.s, 'Vt3.s, 'Vt4.s}['IVLSLane2], ['Xns], 'Xmb16";
      else
        form = "{'Vt.d, 'Vt2.d, 'Vt3.d, 'Vt4.d}['IVLSLane3], ['Xns], 'Xmb32";
      break;
    case NEON_LD4R_post:
      mnemonic = "ld4r";
      form = "{'Vt.%1$s, 'Vt2.%1$s, 'Vt3.%1$s, 'Vt4.%1$s}, ['Xns], 'Xmz4";
      break;
    default:
      break;
  }

  // Work out unallocated encodings.
  bool allocated = (mnemonic != NULL);
  switch (instr->Mask(NEONLoadStoreSingleStructPostIndexMask)) {
    case NEON_LD1_h_post:
    case NEON_LD2_h_post:
    case NEON_LD3_h_post:
    case NEON_LD4_h_post:
    case NEON_ST1_h_post:
    case NEON_ST2_h_post:
    case NEON_ST3_h_post:
    case NEON_ST4_h_post:
      VIXL_ASSERT(allocated);
      allocated = ((instr->GetNEONLSSize() & 1) == 0);
      break;
    case NEON_LD1_s_post:
    case NEON_LD2_s_post:
    case NEON_LD3_s_post:
    case NEON_LD4_s_post:
    case NEON_ST1_s_post:
    case NEON_ST2_s_post:
    case NEON_ST3_s_post:
    case NEON_ST4_s_post:
      VIXL_ASSERT(allocated);
      allocated = (instr->GetNEONLSSize() <= 1) &&
                  ((instr->GetNEONLSSize() == 0) || (instr->GetNEONS() == 0));
      break;
    case NEON_LD1R_post:
    case NEON_LD2R_post:
    case NEON_LD3R_post:
    case NEON_LD4R_post:
      VIXL_ASSERT(allocated);
      allocated = (instr->GetNEONS() == 0);
      break;
    default:
      break;
  }
  if (allocated) {
    VIXL_ASSERT(mnemonic != NULL);
    VIXL_ASSERT(form != NULL);
  } else {
    mnemonic = "unallocated";
    form = "(NEONLoadStoreSingleStructPostIndex)";
  }

  Format(instr, mnemonic, nfd.Substitute(form));
}


void Disassembler::VisitNEONModifiedImmediate(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Vt.%s, 'IVMIImm8, lsl 'IVMIShiftAmt1";

  int half_enc = instr->ExtractBit(11);
  int cmode = instr->GetNEONCmode();
  int cmode_3 = (cmode >> 3) & 1;
  int cmode_2 = (cmode >> 2) & 1;
  int cmode_1 = (cmode >> 1) & 1;
  int cmode_0 = cmode & 1;
  int q = instr->GetNEONQ();
  int op = instr->GetNEONModImmOp();

  static const NEONFormatMap map_b = {{30}, {NF_8B, NF_16B}};
  static const NEONFormatMap map_h = {{30}, {NF_4H, NF_8H}};
  static const NEONFormatMap map_s = {{30}, {NF_2S, NF_4S}};
  NEONFormatDecoder nfd(instr, &map_b);
  if (cmode_3 == 0) {
    if (cmode_0 == 0) {
      mnemonic = (op == 1) ? "mvni" : "movi";
    } else {  // cmode<0> == '1'.
      mnemonic = (op == 1) ? "bic" : "orr";
    }
    nfd.SetFormatMap(0, &map_s);
  } else {  // cmode<3> == '1'.
    if (cmode_2 == 0) {
      if (cmode_0 == 0) {
        mnemonic = (op == 1) ? "mvni" : "movi";
      } else {  // cmode<0> == '1'.
        mnemonic = (op == 1) ? "bic" : "orr";
      }
      nfd.SetFormatMap(0, &map_h);
    } else {  // cmode<2> == '1'.
      if (cmode_1 == 0) {
        mnemonic = (op == 1) ? "mvni" : "movi";
        form = "'Vt.%s, 'IVMIImm8, msl 'IVMIShiftAmt2";
        nfd.SetFormatMap(0, &map_s);
      } else {  // cmode<1> == '1'.
        if (cmode_0 == 0) {
          mnemonic = "movi";
          if (op == 0) {
            form = "'Vt.%s, 'IVMIImm8";
          } else {
            form = (q == 0) ? "'Dd, 'IVMIImm" : "'Vt.2d, 'IVMIImm";
          }
        } else {  // cmode<0> == '1'
          mnemonic = "fmov";
          if (half_enc == 1) {
            form = "'Vt.%s, 'IVMIImmFPHalf";
            nfd.SetFormatMap(0, &map_h);
          } else if (op == 0) {
            form = "'Vt.%s, 'IVMIImmFPSingle";
            nfd.SetFormatMap(0, &map_s);
          } else {
            if (q == 1) {
              form = "'Vt.2d, 'IVMIImmFPDouble";
            } else {
              mnemonic = "unallocated";
              form = "(NEONModifiedImmediate)";
            }
          }
        }
      }
    }
  }
  Format(instr, mnemonic, nfd.Substitute(form));
}


void Disassembler::VisitNEONScalar2RegMisc(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "%sd, %sn";
  const char *form_0 = "%sd, %sn, #0";
  const char *form_fp0 = "%sd, %sn, #0.0";

  NEONFormatDecoder nfd(instr, NEONFormatDecoder::ScalarFormatMap());

  if (instr->Mask(NEON2RegMiscOpcode) <= NEON_NEG_scalar_opcode) {
    // These instructions all use a two bit size field, except NOT and RBIT,
    // which use the field to encode the operation.
    switch (instr->Mask(NEONScalar2RegMiscMask)) {
      case NEON_CMGT_zero_scalar:
        mnemonic = "cmgt";
        form = form_0;
        break;
      case NEON_CMGE_zero_scalar:
        mnemonic = "cmge";
        form = form_0;
        break;
      case NEON_CMLE_zero_scalar:
        mnemonic = "cmle";
        form = form_0;
        break;
      case NEON_CMLT_zero_scalar:
        mnemonic = "cmlt";
        form = form_0;
        break;
      case NEON_CMEQ_zero_scalar:
        mnemonic = "cmeq";
        form = form_0;
        break;
      case NEON_NEG_scalar:
        mnemonic = "neg";
        break;
      case NEON_SQNEG_scalar:
        mnemonic = "sqneg";
        break;
      case NEON_ABS_scalar:
        mnemonic = "abs";
        break;
      case NEON_SQABS_scalar:
        mnemonic = "sqabs";
        break;
      case NEON_SUQADD_scalar:
        mnemonic = "suqadd";
        break;
      case NEON_USQADD_scalar:
        mnemonic = "usqadd";
        break;
      default:
        form = "(NEONScalar2RegMisc)";
    }
  } else {
    // These instructions all use a one bit size field, except SQXTUN, SQXTN
    // and UQXTN, which use a two bit size field.
    nfd.SetFormatMaps(nfd.FPScalarFormatMap());
    switch (instr->Mask(NEONScalar2RegMiscFPMask)) {
      case NEON_FRSQRTE_scalar:
        mnemonic = "frsqrte";
        break;
      case NEON_FRECPE_scalar:
        mnemonic = "frecpe";
        break;
      case NEON_SCVTF_scalar:
        mnemonic = "scvtf";
        break;
      case NEON_UCVTF_scalar:
        mnemonic = "ucvtf";
        break;
      case NEON_FCMGT_zero_scalar:
        mnemonic = "fcmgt";
        form = form_fp0;
        break;
      case NEON_FCMGE_zero_scalar:
        mnemonic = "fcmge";
        form = form_fp0;
        break;
      case NEON_FCMLE_zero_scalar:
        mnemonic = "fcmle";
        form = form_fp0;
        break;
      case NEON_FCMLT_zero_scalar:
        mnemonic = "fcmlt";
        form = form_fp0;
        break;
      case NEON_FCMEQ_zero_scalar:
        mnemonic = "fcmeq";
        form = form_fp0;
        break;
      case NEON_FRECPX_scalar:
        mnemonic = "frecpx";
        break;
      case NEON_FCVTNS_scalar:
        mnemonic = "fcvtns";
        break;
      case NEON_FCVTNU_scalar:
        mnemonic = "fcvtnu";
        break;
      case NEON_FCVTPS_scalar:
        mnemonic = "fcvtps";
        break;
      case NEON_FCVTPU_scalar:
        mnemonic = "fcvtpu";
        break;
      case NEON_FCVTMS_scalar:
        mnemonic = "fcvtms";
        break;
      case NEON_FCVTMU_scalar:
        mnemonic = "fcvtmu";
        break;
      case NEON_FCVTZS_scalar:
        mnemonic = "fcvtzs";
        break;
      case NEON_FCVTZU_scalar:
        mnemonic = "fcvtzu";
        break;
      case NEON_FCVTAS_scalar:
        mnemonic = "fcvtas";
        break;
      case NEON_FCVTAU_scalar:
        mnemonic = "fcvtau";
        break;
      case NEON_FCVTXN_scalar:
        nfd.SetFormatMap(0, nfd.LongScalarFormatMap());
        mnemonic = "fcvtxn";
        break;
      default:
        nfd.SetFormatMap(0, nfd.ScalarFormatMap());
        nfd.SetFormatMap(1, nfd.LongScalarFormatMap());
        switch (instr->Mask(NEONScalar2RegMiscMask)) {
          case NEON_SQXTN_scalar:
            mnemonic = "sqxtn";
            break;
          case NEON_UQXTN_scalar:
            mnemonic = "uqxtn";
            break;
          case NEON_SQXTUN_scalar:
            mnemonic = "sqxtun";
            break;
          default:
            form = "(NEONScalar2RegMisc)";
        }
    }
  }
  Format(instr, mnemonic, nfd.SubstitutePlaceholders(form));
}

void Disassembler::VisitNEONScalar2RegMiscFP16(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Hd, 'Hn";
  const char *form_fp0 = "'Hd, 'Hn, #0.0";

  switch (instr->Mask(NEONScalar2RegMiscFP16Mask)) {
#define FORMAT(A, B)        \
  case NEON_##A##_H_scalar: \
    mnemonic = B;           \
    break;
    // clang-format off
    FORMAT(FCVTNS,  "fcvtns")
    FORMAT(FCVTMS,  "fcvtms")
    FORMAT(FCVTAS,  "fcvtas")
    FORMAT(SCVTF,   "scvtf")
    FORMAT(FCVTPS,  "fcvtps")
    FORMAT(FCVTZS,  "fcvtzs")
    FORMAT(FRECPE,  "frecpe")
    FORMAT(FRECPX,  "frecpx")
    FORMAT(FCVTNU,  "fcvtnu")
    FORMAT(FCVTMU,  "fcvtmu")
    FORMAT(FCVTAU,  "fcvtau")
    FORMAT(UCVTF,   "ucvtf")
    FORMAT(FCVTPU,  "fcvtpu")
    FORMAT(FCVTZU,  "fcvtzu")
    FORMAT(FRSQRTE, "frsqrte")
// clang-format on
#undef FORMAT
#define FORMAT(A, B)             \
  case NEON_##A##_H_zero_scalar: \
    mnemonic = B;                \
    form = form_fp0;             \
    break;
    FORMAT(FCMGT, "fcmgt")
    FORMAT(FCMEQ, "fcmeq")
    FORMAT(FCMLT, "fcmlt")
    FORMAT(FCMGE, "fcmge")
    FORMAT(FCMLE, "fcmle")
#undef FORMAT

    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitNEONScalar3Diff(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "%sd, %sn, %sm";
  NEONFormatDecoder nfd(instr,
                        NEONFormatDecoder::LongScalarFormatMap(),
                        NEONFormatDecoder::ScalarFormatMap());

  switch (instr->Mask(NEONScalar3DiffMask)) {
    case NEON_SQDMLAL_scalar:
      mnemonic = "sqdmlal";
      break;
    case NEON_SQDMLSL_scalar:
      mnemonic = "sqdmlsl";
      break;
    case NEON_SQDMULL_scalar:
      mnemonic = "sqdmull";
      break;
    default:
      form = "(NEONScalar3Diff)";
  }
  Format(instr, mnemonic, nfd.SubstitutePlaceholders(form));
}


void Disassembler::VisitNEONScalar3Same(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "%sd, %sn, %sm";
  NEONFormatDecoder nfd(instr, NEONFormatDecoder::ScalarFormatMap());

  if (instr->Mask(NEONScalar3SameFPFMask) == NEONScalar3SameFPFixed) {
    nfd.SetFormatMaps(nfd.FPScalarFormatMap());
    switch (instr->Mask(NEONScalar3SameFPMask)) {
      case NEON_FACGE_scalar:
        mnemonic = "facge";
        break;
      case NEON_FACGT_scalar:
        mnemonic = "facgt";
        break;
      case NEON_FCMEQ_scalar:
        mnemonic = "fcmeq";
        break;
      case NEON_FCMGE_scalar:
        mnemonic = "fcmge";
        break;
      case NEON_FCMGT_scalar:
        mnemonic = "fcmgt";
        break;
      case NEON_FMULX_scalar:
        mnemonic = "fmulx";
        break;
      case NEON_FRECPS_scalar:
        mnemonic = "frecps";
        break;
      case NEON_FRSQRTS_scalar:
        mnemonic = "frsqrts";
        break;
      case NEON_FABD_scalar:
        mnemonic = "fabd";
        break;
      default:
        form = "(NEONScalar3Same)";
    }
  } else {
    switch (instr->Mask(NEONScalar3SameMask)) {
      case NEON_ADD_scalar:
        mnemonic = "add";
        break;
      case NEON_SUB_scalar:
        mnemonic = "sub";
        break;
      case NEON_CMEQ_scalar:
        mnemonic = "cmeq";
        break;
      case NEON_CMGE_scalar:
        mnemonic = "cmge";
        break;
      case NEON_CMGT_scalar:
        mnemonic = "cmgt";
        break;
      case NEON_CMHI_scalar:
        mnemonic = "cmhi";
        break;
      case NEON_CMHS_scalar:
        mnemonic = "cmhs";
        break;
      case NEON_CMTST_scalar:
        mnemonic = "cmtst";
        break;
      case NEON_UQADD_scalar:
        mnemonic = "uqadd";
        break;
      case NEON_SQADD_scalar:
        mnemonic = "sqadd";
        break;
      case NEON_UQSUB_scalar:
        mnemonic = "uqsub";
        break;
      case NEON_SQSUB_scalar:
        mnemonic = "sqsub";
        break;
      case NEON_USHL_scalar:
        mnemonic = "ushl";
        break;
      case NEON_SSHL_scalar:
        mnemonic = "sshl";
        break;
      case NEON_UQSHL_scalar:
        mnemonic = "uqshl";
        break;
      case NEON_SQSHL_scalar:
        mnemonic = "sqshl";
        break;
      case NEON_URSHL_scalar:
        mnemonic = "urshl";
        break;
      case NEON_SRSHL_scalar:
        mnemonic = "srshl";
        break;
      case NEON_UQRSHL_scalar:
        mnemonic = "uqrshl";
        break;
      case NEON_SQRSHL_scalar:
        mnemonic = "sqrshl";
        break;
      case NEON_SQDMULH_scalar:
        mnemonic = "sqdmulh";
        break;
      case NEON_SQRDMULH_scalar:
        mnemonic = "sqrdmulh";
        break;
      default:
        form = "(NEONScalar3Same)";
    }
  }
  Format(instr, mnemonic, nfd.SubstitutePlaceholders(form));
}

void Disassembler::VisitNEONScalar3SameFP16(const Instruction *instr) {
  const char *mnemonic = NULL;
  const char *form = "'Hd, 'Hn, 'Hm";

  switch (instr->Mask(NEONScalar3SameFP16Mask)) {
    case NEON_FABD_H_scalar:
      mnemonic = "fabd";
      break;
    case NEON_FMULX_H_scalar:
      mnemonic = "fmulx";
      break;
    case NEON_FCMEQ_H_scalar:
      mnemonic = "fcmeq";
      break;
    case NEON_FCMGE_H_scalar:
      mnemonic = "fcmge";
      break;
    case NEON_FCMGT_H_scalar:
      mnemonic = "fcmgt";
      break;
    case NEON_FACGE_H_scalar:
      mnemonic = "facge";
      break;
    case NEON_FACGT_H_scalar:
      mnemonic = "facgt";
      break;
    case NEON_FRECPS_H_scalar:
      mnemonic = "frecps";
      break;
    case NEON_FRSQRTS_H_scalar:
      mnemonic = "frsqrts";
      break;
    default:
      VIXL_UNREACHABLE();
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitNEONScalar3SameExtra(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "%sd, %sn, %sm";
  NEONFormatDecoder nfd(instr, NEONFormatDecoder::ScalarFormatMap());

  switch (instr->Mask(NEONScalar3SameExtraMask)) {
    case NEON_SQRDMLAH_scalar:
      mnemonic = "sqrdmlah";
      break;
    case NEON_SQRDMLSH_scalar:
      mnemonic = "sqrdmlsh";
      break;
    default:
      form = "(NEONScalar3SameExtra)";
  }
  Format(instr, mnemonic, nfd.SubstitutePlaceholders(form));
}


void Disassembler::VisitNEONScalarByIndexedElement(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "%sd, %sn, 'Ve.%s['IVByElemIndex]";
  const char *form_half = "'Hd, 'Hn, 'Ve.h['IVByElemIndex]";
  NEONFormatDecoder nfd(instr, NEONFormatDecoder::ScalarFormatMap());
  bool long_instr = false;

  switch (instr->Mask(NEONScalarByIndexedElementMask)) {
    case NEON_SQDMULL_byelement_scalar:
      mnemonic = "sqdmull";
      long_instr = true;
      break;
    case NEON_SQDMLAL_byelement_scalar:
      mnemonic = "sqdmlal";
      long_instr = true;
      break;
    case NEON_SQDMLSL_byelement_scalar:
      mnemonic = "sqdmlsl";
      long_instr = true;
      break;
    case NEON_SQDMULH_byelement_scalar:
      mnemonic = "sqdmulh";
      break;
    case NEON_SQRDMULH_byelement_scalar:
      mnemonic = "sqrdmulh";
      break;
    case NEON_SQRDMLAH_byelement_scalar:
      mnemonic = "sqrdmlah";
      break;
    case NEON_SQRDMLSH_byelement_scalar:
      mnemonic = "sqrdmlsh";
      break;
    default:
      nfd.SetFormatMap(0, nfd.FPScalarFormatMap());
      switch (instr->Mask(NEONScalarByIndexedElementFPMask)) {
        case NEON_FMUL_byelement_scalar:
          mnemonic = "fmul";
          break;
        case NEON_FMLA_byelement_scalar:
          mnemonic = "fmla";
          break;
        case NEON_FMLS_byelement_scalar:
          mnemonic = "fmls";
          break;
        case NEON_FMULX_byelement_scalar:
          mnemonic = "fmulx";
          break;
        case NEON_FMLA_H_byelement_scalar:
          mnemonic = "fmla";
          form = form_half;
          break;
        case NEON_FMLS_H_byelement_scalar:
          mnemonic = "fmls";
          form = form_half;
          break;
        case NEON_FMUL_H_byelement_scalar:
          mnemonic = "fmul";
          form = form_half;
          break;
        case NEON_FMULX_H_byelement_scalar:
          mnemonic = "fmulx";
          form = form_half;
          break;
        default:
          form = "(NEONScalarByIndexedElement)";
      }
  }

  if (long_instr) {
    nfd.SetFormatMap(0, nfd.LongScalarFormatMap());
  }

  Format(instr,
         mnemonic,
         nfd.Substitute(form, nfd.kPlaceholder, nfd.kPlaceholder, nfd.kFormat));
}


void Disassembler::VisitNEONScalarCopy(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(NEONScalarCopy)";

  NEONFormatDecoder nfd(instr, NEONFormatDecoder::TriangularScalarFormatMap());

  if (instr->Mask(NEONScalarCopyMask) == NEON_DUP_ELEMENT_scalar) {
    mnemonic = "mov";
    form = "%sd, 'Vn.%s['IVInsIndex1]";
  }

  Format(instr, mnemonic, nfd.Substitute(form, nfd.kPlaceholder, nfd.kFormat));
}


void Disassembler::VisitNEONScalarPairwise(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "%sd, 'Vn.%s";
  NEONFormatMap map = {{22}, {NF_2S, NF_2D}};
  NEONFormatDecoder nfd(instr,
                        NEONFormatDecoder::FPScalarPairwiseFormatMap(),
                        &map);

  switch (instr->Mask(NEONScalarPairwiseMask)) {
    case NEON_ADDP_scalar:
      // All pairwise operations except ADDP use bit U to differentiate FP16
      // from FP32/FP64 variations.
      nfd.SetFormatMap(0, NEONFormatDecoder::FPScalarFormatMap());
      mnemonic = "addp";
      break;
    case NEON_FADDP_h_scalar:
      form = "%sd, 'Vn.2h";
      VIXL_FALLTHROUGH();
    case NEON_FADDP_scalar:
      mnemonic = "faddp";
      break;
    case NEON_FMAXP_h_scalar:
      form = "%sd, 'Vn.2h";
      VIXL_FALLTHROUGH();
    case NEON_FMAXP_scalar:
      mnemonic = "fmaxp";
      break;
    case NEON_FMAXNMP_h_scalar:
      form = "%sd, 'Vn.2h";
      VIXL_FALLTHROUGH();
    case NEON_FMAXNMP_scalar:
      mnemonic = "fmaxnmp";
      break;
    case NEON_FMINP_h_scalar:
      form = "%sd, 'Vn.2h";
      VIXL_FALLTHROUGH();
    case NEON_FMINP_scalar:
      mnemonic = "fminp";
      break;
    case NEON_FMINNMP_h_scalar:
      form = "%sd, 'Vn.2h";
      VIXL_FALLTHROUGH();
    case NEON_FMINNMP_scalar:
      mnemonic = "fminnmp";
      break;
    default:
      form = "(NEONScalarPairwise)";
  }
  Format(instr,
         mnemonic,
         nfd.Substitute(form,
                        NEONFormatDecoder::kPlaceholder,
                        NEONFormatDecoder::kFormat));
}


void Disassembler::VisitNEONScalarShiftImmediate(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "%sd, %sn, 'Is1";
  const char *form_2 = "%sd, %sn, 'Is2";

  static const NEONFormatMap map_shift = {{22, 21, 20, 19},
                                          {NF_UNDEF,
                                           NF_B,
                                           NF_H,
                                           NF_H,
                                           NF_S,
                                           NF_S,
                                           NF_S,
                                           NF_S,
                                           NF_D,
                                           NF_D,
                                           NF_D,
                                           NF_D,
                                           NF_D,
                                           NF_D,
                                           NF_D,
                                           NF_D}};
  static const NEONFormatMap map_shift_narrow =
      {{21, 20, 19}, {NF_UNDEF, NF_H, NF_S, NF_S, NF_D, NF_D, NF_D, NF_D}};
  NEONFormatDecoder nfd(instr, &map_shift);

  if (instr->GetImmNEONImmh()) {  // immh has to be non-zero.
    switch (instr->Mask(NEONScalarShiftImmediateMask)) {
      case NEON_FCVTZU_imm_scalar:
        mnemonic = "fcvtzu";
        break;
      case NEON_FCVTZS_imm_scalar:
        mnemonic = "fcvtzs";
        break;
      case NEON_SCVTF_imm_scalar:
        mnemonic = "scvtf";
        break;
      case NEON_UCVTF_imm_scalar:
        mnemonic = "ucvtf";
        break;
      case NEON_SRI_scalar:
        mnemonic = "sri";
        break;
      case NEON_SSHR_scalar:
        mnemonic = "sshr";
        break;
      case NEON_USHR_scalar:
        mnemonic = "ushr";
        break;
      case NEON_SRSHR_scalar:
        mnemonic = "srshr";
        break;
      case NEON_URSHR_scalar:
        mnemonic = "urshr";
        break;
      case NEON_SSRA_scalar:
        mnemonic = "ssra";
        break;
      case NEON_USRA_scalar:
        mnemonic = "usra";
        break;
      case NEON_SRSRA_scalar:
        mnemonic = "srsra";
        break;
      case NEON_URSRA_scalar:
        mnemonic = "ursra";
        break;
      case NEON_SHL_scalar:
        mnemonic = "shl";
        form = form_2;
        break;
      case NEON_SLI_scalar:
        mnemonic = "sli";
        form = form_2;
        break;
      case NEON_SQSHLU_scalar:
        mnemonic = "sqshlu";
        form = form_2;
        break;
      case NEON_SQSHL_imm_scalar:
        mnemonic = "sqshl";
        form = form_2;
        break;
      case NEON_UQSHL_imm_scalar:
        mnemonic = "uqshl";
        form = form_2;
        break;
      case NEON_UQSHRN_scalar:
        mnemonic = "uqshrn";
        nfd.SetFormatMap(1, &map_shift_narrow);
        break;
      case NEON_UQRSHRN_scalar:
        mnemonic = "uqrshrn";
        nfd.SetFormatMap(1, &map_shift_narrow);
        break;
      case NEON_SQSHRN_scalar:
        mnemonic = "sqshrn";
        nfd.SetFormatMap(1, &map_shift_narrow);
        break;
      case NEON_SQRSHRN_scalar:
        mnemonic = "sqrshrn";
        nfd.SetFormatMap(1, &map_shift_narrow);
        break;
      case NEON_SQSHRUN_scalar:
        mnemonic = "sqshrun";
        nfd.SetFormatMap(1, &map_shift_narrow);
        break;
      case NEON_SQRSHRUN_scalar:
        mnemonic = "sqrshrun";
        nfd.SetFormatMap(1, &map_shift_narrow);
        break;
      default:
        form = "(NEONScalarShiftImmediate)";
    }
  } else {
    form = "(NEONScalarShiftImmediate)";
  }
  Format(instr, mnemonic, nfd.SubstitutePlaceholders(form));
}


void Disassembler::VisitNEONShiftImmediate(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Vd.%s, 'Vn.%s, 'Is1";
  const char *form_shift_2 = "'Vd.%s, 'Vn.%s, 'Is2";
  const char *form_xtl = "'Vd.%s, 'Vn.%s";

  // 0001->8H, 001x->4S, 01xx->2D, all others undefined.
  static const NEONFormatMap map_shift_ta =
      {{22, 21, 20, 19},
       {NF_UNDEF, NF_8H, NF_4S, NF_4S, NF_2D, NF_2D, NF_2D, NF_2D}};

  // 00010->8B, 00011->16B, 001x0->4H, 001x1->8H,
  // 01xx0->2S, 01xx1->4S, 1xxx1->2D, all others undefined.
  static const NEONFormatMap map_shift_tb =
      {{22, 21, 20, 19, 30},
       {NF_UNDEF, NF_UNDEF, NF_8B,    NF_16B,   NF_4H,    NF_8H,    NF_4H,
        NF_8H,    NF_2S,    NF_4S,    NF_2S,    NF_4S,    NF_2S,    NF_4S,
        NF_2S,    NF_4S,    NF_UNDEF, NF_2D,    NF_UNDEF, NF_2D,    NF_UNDEF,
        NF_2D,    NF_UNDEF, NF_2D,    NF_UNDEF, NF_2D,    NF_UNDEF, NF_2D,
        NF_UNDEF, NF_2D,    NF_UNDEF, NF_2D}};

  NEONFormatDecoder nfd(instr, &map_shift_tb);

  if (instr->GetImmNEONImmh()) {  // immh has to be non-zero.
    switch (instr->Mask(NEONShiftImmediateMask)) {
      case NEON_SQSHLU:
        mnemonic = "sqshlu";
        form = form_shift_2;
        break;
      case NEON_SQSHL_imm:
        mnemonic = "sqshl";
        form = form_shift_2;
        break;
      case NEON_UQSHL_imm:
        mnemonic = "uqshl";
        form = form_shift_2;
        break;
      case NEON_SHL:
        mnemonic = "shl";
        form = form_shift_2;
        break;
      case NEON_SLI:
        mnemonic = "sli";
        form = form_shift_2;
        break;
      case NEON_SCVTF_imm:
        mnemonic = "scvtf";
        break;
      case NEON_UCVTF_imm:
        mnemonic = "ucvtf";
        break;
      case NEON_FCVTZU_imm:
        mnemonic = "fcvtzu";
        break;
      case NEON_FCVTZS_imm:
        mnemonic = "fcvtzs";
        break;
      case NEON_SRI:
        mnemonic = "sri";
        break;
      case NEON_SSHR:
        mnemonic = "sshr";
        break;
      case NEON_USHR:
        mnemonic = "ushr";
        break;
      case NEON_SRSHR:
        mnemonic = "srshr";
        break;
      case NEON_URSHR:
        mnemonic = "urshr";
        break;
      case NEON_SSRA:
        mnemonic = "ssra";
        break;
      case NEON_USRA:
        mnemonic = "usra";
        break;
      case NEON_SRSRA:
        mnemonic = "srsra";
        break;
      case NEON_URSRA:
        mnemonic = "ursra";
        break;
      case NEON_SHRN:
        mnemonic = instr->Mask(NEON_Q) ? "shrn2" : "shrn";
        nfd.SetFormatMap(1, &map_shift_ta);
        break;
      case NEON_RSHRN:
        mnemonic = instr->Mask(NEON_Q) ? "rshrn2" : "rshrn";
        nfd.SetFormatMap(1, &map_shift_ta);
        break;
      case NEON_UQSHRN:
        mnemonic = instr->Mask(NEON_Q) ? "uqshrn2" : "uqshrn";
        nfd.SetFormatMap(1, &map_shift_ta);
        break;
      case NEON_UQRSHRN:
        mnemonic = instr->Mask(NEON_Q) ? "uqrshrn2" : "uqrshrn";
        nfd.SetFormatMap(1, &map_shift_ta);
        break;
      case NEON_SQSHRN:
        mnemonic = instr->Mask(NEON_Q) ? "sqshrn2" : "sqshrn";
        nfd.SetFormatMap(1, &map_shift_ta);
        break;
      case NEON_SQRSHRN:
        mnemonic = instr->Mask(NEON_Q) ? "sqrshrn2" : "sqrshrn";
        nfd.SetFormatMap(1, &map_shift_ta);
        break;
      case NEON_SQSHRUN:
        mnemonic = instr->Mask(NEON_Q) ? "sqshrun2" : "sqshrun";
        nfd.SetFormatMap(1, &map_shift_ta);
        break;
      case NEON_SQRSHRUN:
        mnemonic = instr->Mask(NEON_Q) ? "sqrshrun2" : "sqrshrun";
        nfd.SetFormatMap(1, &map_shift_ta);
        break;
      case NEON_SSHLL:
        nfd.SetFormatMap(0, &map_shift_ta);
        if (instr->GetImmNEONImmb() == 0 &&
            CountSetBits(instr->GetImmNEONImmh(), 32) == 1) {  // sxtl variant.
          form = form_xtl;
          mnemonic = instr->Mask(NEON_Q) ? "sxtl2" : "sxtl";
        } else {  // sshll variant.
          form = form_shift_2;
          mnemonic = instr->Mask(NEON_Q) ? "sshll2" : "sshll";
        }
        break;
      case NEON_USHLL:
        nfd.SetFormatMap(0, &map_shift_ta);
        if (instr->GetImmNEONImmb() == 0 &&
            CountSetBits(instr->GetImmNEONImmh(), 32) == 1) {  // uxtl variant.
          form = form_xtl;
          mnemonic = instr->Mask(NEON_Q) ? "uxtl2" : "uxtl";
        } else {  // ushll variant.
          form = form_shift_2;
          mnemonic = instr->Mask(NEON_Q) ? "ushll2" : "ushll";
        }
        break;
      default:
        form = "(NEONShiftImmediate)";
    }
  } else {
    form = "(NEONShiftImmediate)";
  }
  Format(instr, mnemonic, nfd.Substitute(form));
}


void Disassembler::VisitNEONTable(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(NEONTable)";
  const char form_1v[] = "'Vd.%%s, {'Vn.16b}, 'Vm.%%s";
  const char form_2v[] = "'Vd.%%s, {'Vn.16b, v%d.16b}, 'Vm.%%s";
  const char form_3v[] = "'Vd.%%s, {'Vn.16b, v%d.16b, v%d.16b}, 'Vm.%%s";
  const char form_4v[] =
      "'Vd.%%s, {'Vn.16b, v%d.16b, v%d.16b, v%d.16b}, 'Vm.%%s";
  static const NEONFormatMap map_b = {{30}, {NF_8B, NF_16B}};
  NEONFormatDecoder nfd(instr, &map_b);

  switch (instr->Mask(NEONTableMask)) {
    case NEON_TBL_1v:
      mnemonic = "tbl";
      form = form_1v;
      break;
    case NEON_TBL_2v:
      mnemonic = "tbl";
      form = form_2v;
      break;
    case NEON_TBL_3v:
      mnemonic = "tbl";
      form = form_3v;
      break;
    case NEON_TBL_4v:
      mnemonic = "tbl";
      form = form_4v;
      break;
    case NEON_TBX_1v:
      mnemonic = "tbx";
      form = form_1v;
      break;
    case NEON_TBX_2v:
      mnemonic = "tbx";
      form = form_2v;
      break;
    case NEON_TBX_3v:
      mnemonic = "tbx";
      form = form_3v;
      break;
    case NEON_TBX_4v:
      mnemonic = "tbx";
      form = form_4v;
      break;
    default:
      break;
  }

  char re_form[sizeof(form_4v) + 6];
  int reg_num = instr->GetRn();
  snprintf(re_form,
           sizeof(re_form),
           form,
           (reg_num + 1) % kNumberOfVRegisters,
           (reg_num + 2) % kNumberOfVRegisters,
           (reg_num + 3) % kNumberOfVRegisters);

  Format(instr, mnemonic, nfd.Substitute(re_form));
}


void Disassembler::VisitNEONPerm(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Vd.%s, 'Vn.%s, 'Vm.%s";
  NEONFormatDecoder nfd(instr);

  switch (instr->Mask(NEONPermMask)) {
    case NEON_TRN1:
      mnemonic = "trn1";
      break;
    case NEON_TRN2:
      mnemonic = "trn2";
      break;
    case NEON_UZP1:
      mnemonic = "uzp1";
      break;
    case NEON_UZP2:
      mnemonic = "uzp2";
      break;
    case NEON_ZIP1:
      mnemonic = "zip1";
      break;
    case NEON_ZIP2:
      mnemonic = "zip2";
      break;
    default:
      form = "(NEONPerm)";
  }
  Format(instr, mnemonic, nfd.Substitute(form));
}

void Disassembler::VisitSVEAddressGeneration(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEAddressGeneration)";

  switch (instr->Mask(SVEAddressGenerationMask)) {
    // ADR <Zd>.D, [<Zn>.D, <Zm>.D, SXTW{ <amount>}]
    case ADR_z_az_d_s32_scaled:
      mnemonic = "adr";
      form = "'Zd.d, ['Zn.d, 'Zm.d, SXTW{ <amount>}]";
      break;
    // ADR <Zd>.D, [<Zn>.D, <Zm>.D, UXTW{ <amount>}]
    case ADR_z_az_d_u32_scaled:
      mnemonic = "adr";
      form = "'Zd.d, ['Zn.d, 'Zm.d, UXTW{ <amount>}]";
      break;
    // ADR <Zd>.<T>, [<Zn>.<T>, <Zm>.<T>{, <mod> <amount>}]
    case ADR_z_az_sd_same_scaled:
      mnemonic = "adr";
      form = "'Zd.<T>, ['Zn.<T>, 'Zm.<T>{, <mod> <amount>}]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBitwiseImm(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zdn>.<T>, <Zdn>.<T>, #<const>
  const char *form = "'Zd.<T>, 'Zd.<T>, #<const>";

  switch (instr->Mask(SVEBitwiseImmMask)) {
    // AND <Zdn>.<T>, <Zdn>.<T>, #<const>
    case AND_z_zi:
      mnemonic = "and";
      break;
    // DUPM <Zd>.<T>, #<const>
    case DUPM_z_i:
      mnemonic = "dupm";
      form = "'Zd.<T>, #<const>";
      break;
    // EOR <Zdn>.<T>, <Zdn>.<T>, #<const>
    case EOR_z_zi:
      mnemonic = "eor";
      break;
    // ORR <Zdn>.<T>, <Zdn>.<T>, #<const>
    case ORR_z_zi:
      mnemonic = "orr";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBitwiseLogicalUnpredicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zd>.D, <Zn>.D, <Zm>.D
  const char *form = "'Zd.d, 'Zn.d, 'Zm.d";

  switch (instr->Mask(SVEBitwiseLogicalUnpredicatedMask)) {
    // AND <Zd>.D, <Zn>.D, <Zm>.D
    case AND_z_zz:
      mnemonic = "and";
      break;
    // BIC <Zd>.D, <Zn>.D, <Zm>.D
    case BIC_z_zz:
      mnemonic = "bic";
      break;
    // EOR <Zd>.D, <Zn>.D, <Zm>.D
    case EOR_z_zz:
      mnemonic = "eor";
      break;
    // ORR <Zd>.D, <Zn>.D, <Zm>.D
    case ORR_z_zz:
      mnemonic = "orr";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBitwiseShiftPredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  const char *form = "'Zd.'t, p'u1210/m, 'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEBitwiseShiftPredicatedMask)) {
    // ASRD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, #<const>
    case ASRD_z_p_zi:
      mnemonic = "asrd";
      form = "'Zd.<T>, p'u1210/m, 'Zd.<T>, #<const>";
      break;
    // ASRR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case ASRR_z_p_zz:
      mnemonic = "asrr";
      break;
    // ASR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, #<const>
    case ASR_z_p_zi:
      mnemonic = "asr";
      form = "'Zd.<T>, p'u1210/m, 'Zd.<T>, #<const>";
      break;
    // ASR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D
    case ASR_z_p_zw:
      mnemonic = "asr";
      form = "'Zd.'t, p'u1210/m, 'Zd.'t, 'Zn.d";
      break;
    // ASR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case ASR_z_p_zz:
      mnemonic = "asr";
      break;
    // LSLR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case LSLR_z_p_zz:
      mnemonic = "lslr";
      break;
    // LSL <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, #<const>
    case LSL_z_p_zi:
      mnemonic = "lsl";
      form = "'Zd.<T>, p'u1210/m, 'Zd.<T>, #<const>";
      break;
    // LSL <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D
    case LSL_z_p_zw:
      mnemonic = "lsl";
      form = "'Zd.'t, p'u1210/m, 'Zd.'t, 'Zn.d";
      break;
    // LSL <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case LSL_z_p_zz:
      mnemonic = "lsl";
      break;
    // LSRR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case LSRR_z_p_zz:
      mnemonic = "lsrr";
      break;
    // LSR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, #<const>
    case LSR_z_p_zi:
      mnemonic = "lsr";
      form = "'Zd.<T>, p'u1210/m, 'Zd.<T>, #<const>";
      break;
    // LSR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D
    case LSR_z_p_zw:
      mnemonic = "lsr";
      form = "'Zd.'t, p'u1210/m, 'Zd.'t, 'Zn.d";
      break;
    // LSR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case LSR_z_p_zz:
      mnemonic = "lsr";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBitwiseShiftUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zd>.<T>, <Zn>.<T>, <Zm>.D
  const char *form = "'Zd.'t, 'Zn.'t, 'Zm.d";

  switch (instr->Mask(SVEBitwiseShiftUnpredicatedMask)) {
    // ASR <Zd>.<T>, <Zn>.<T>, #<const>
    case ASR_z_zi:
      mnemonic = "asr";
      form = "'Zd.<T>, 'Zn.<T>, #<const>";
      break;
    // ASR <Zd>.<T>, <Zn>.<T>, <Zm>.D
    case ASR_z_zw:
      mnemonic = "asr";
      break;
    // LSL <Zd>.<T>, <Zn>.<T>, #<const>
    case LSL_z_zi:
      mnemonic = "lsl";
      form = "'Zd.<T>, 'Zn.<T>, #<const>";
      break;
    // LSL <Zd>.<T>, <Zn>.<T>, <Zm>.D
    case LSL_z_zw:
      mnemonic = "lsl";
      break;
    // LSR <Zd>.<T>, <Zn>.<T>, #<const>
    case LSR_z_zi:
      mnemonic = "lsr";
      form = "'Zd.<T>, 'Zn.<T>, #<const>";
      break;
    // LSR <Zd>.<T>, <Zn>.<T>, <Zm>.D
    case LSR_z_zw:
      mnemonic = "lsr";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEElementCount(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Xdn>{, <pattern>{, MUL #<imm>}}
  const char *form = "'Rd{, #'u0905{, MUL #<imm>}}";

  switch (instr->Mask(SVEElementCountMask)) {
    // CNTB <Xd>{, <pattern>{, MUL #<imm>}}
    case CNTB_r_s:
      mnemonic = "cntb";
      break;
    // CNTD <Xd>{, <pattern>{, MUL #<imm>}}
    case CNTD_r_s:
      mnemonic = "cntd";
      break;
    // CNTH <Xd>{, <pattern>{, MUL #<imm>}}
    case CNTH_r_s:
      mnemonic = "cnth";
      break;
    // CNTW <Xd>{, <pattern>{, MUL #<imm>}}
    case CNTW_r_s:
      mnemonic = "cntw";
      break;
    // DECB <Xdn>{, <pattern>{, MUL #<imm>}}
    case DECB_r_rs:
      mnemonic = "decb";
      break;
    // DECD <Xdn>{, <pattern>{, MUL #<imm>}}
    case DECD_r_rs:
      mnemonic = "decd";
      break;
    // DECD <Zdn>.D{, <pattern>{, MUL #<imm>}}
    case DECD_z_zs:
      mnemonic = "decd";
      form = "'Zd.d{, #'u0905{, MUL #<imm>}}";
      break;
    // DECH <Xdn>{, <pattern>{, MUL #<imm>}}
    case DECH_r_rs:
      mnemonic = "dech";
      break;
    // DECH <Zdn>.H{, <pattern>{, MUL #<imm>}}
    case DECH_z_zs:
      mnemonic = "dech";
      form = "'Zd.h{, #'u0905{, MUL #<imm>}}";
      break;
    // DECW <Xdn>{, <pattern>{, MUL #<imm>}}
    case DECW_r_rs:
      mnemonic = "decw";
      break;
    // DECW <Zdn>.S{, <pattern>{, MUL #<imm>}}
    case DECW_z_zs:
      mnemonic = "decw";
      form = "'Zd.s{, #'u0905{, MUL #<imm>}}";
      break;
    // INCB <Xdn>{, <pattern>{, MUL #<imm>}}
    case INCB_r_rs:
      mnemonic = "incb";
      break;
    // INCD <Xdn>{, <pattern>{, MUL #<imm>}}
    case INCD_r_rs:
      mnemonic = "incd";
      break;
    // INCD <Zdn>.D{, <pattern>{, MUL #<imm>}}
    case INCD_z_zs:
      mnemonic = "incd";
      form = "'Zd.d{, #'u0905{, MUL #<imm>}}";
      break;
    // INCH <Xdn>{, <pattern>{, MUL #<imm>}}
    case INCH_r_rs:
      mnemonic = "inch";
      break;
    // INCH <Zdn>.H{, <pattern>{, MUL #<imm>}}
    case INCH_z_zs:
      mnemonic = "inch";
      form = "'Zd.h{, #'u0905{, MUL #<imm>}}";
      break;
    // INCW <Xdn>{, <pattern>{, MUL #<imm>}}
    case INCW_r_rs:
      mnemonic = "incw";
      break;
    // INCW <Zdn>.S{, <pattern>{, MUL #<imm>}}
    case INCW_z_zs:
      mnemonic = "incw";
      form = "'Zd.s{, #'u0905{, MUL #<imm>}}";
      break;
    // SQDECB <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
    case SQDECB_r_rs_sx:
      mnemonic = "sqdecb";
      form = "'Rd, 'Rd{, #'u0905{, MUL #<imm>}}";
      break;
    // SQDECB <Xdn>{, <pattern>{, MUL #<imm>}}
    case SQDECB_r_rs_x:
      mnemonic = "sqdecb";
      break;
    // SQDECD <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
    case SQDECD_r_rs_sx:
      mnemonic = "sqdecd";
      form = "'Rd, 'Rd{, #'u0905{, MUL #<imm>}}";
      break;
    // SQDECD <Xdn>{, <pattern>{, MUL #<imm>}}
    case SQDECD_r_rs_x:
      mnemonic = "sqdecd";
      break;
    // SQDECD <Zdn>.D{, <pattern>{, MUL #<imm>}}
    case SQDECD_z_zs:
      mnemonic = "sqdecd";
      form = "'Zd.d{, #'u0905{, MUL #<imm>}}";
      break;
    // SQDECH <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
    case SQDECH_r_rs_sx:
      mnemonic = "sqdech";
      form = "'Rd, 'Rd{, #'u0905{, MUL #<imm>}}";
      break;
    // SQDECH <Xdn>{, <pattern>{, MUL #<imm>}}
    case SQDECH_r_rs_x:
      mnemonic = "sqdech";
      break;
    // SQDECH <Zdn>.H{, <pattern>{, MUL #<imm>}}
    case SQDECH_z_zs:
      mnemonic = "sqdech";
      form = "'Zd.h{, #'u0905{, MUL #<imm>}}";
      break;
    // SQDECW <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
    case SQDECW_r_rs_sx:
      mnemonic = "sqdecw";
      form = "'Rd, 'Rd{, #'u0905{, MUL #<imm>}}";
      break;
    // SQDECW <Xdn>{, <pattern>{, MUL #<imm>}}
    case SQDECW_r_rs_x:
      mnemonic = "sqdecw";
      break;
    // SQDECW <Zdn>.S{, <pattern>{, MUL #<imm>}}
    case SQDECW_z_zs:
      mnemonic = "sqdecw";
      form = "'Zd.s{, #'u0905{, MUL #<imm>}}";
      break;
    // SQINCB <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
    case SQINCB_r_rs_sx:
      mnemonic = "sqincb";
      form = "'Rd, 'Rd{, #'u0905{, MUL #<imm>}}";
      break;
    // SQINCB <Xdn>{, <pattern>{, MUL #<imm>}}
    case SQINCB_r_rs_x:
      mnemonic = "sqincb";
      break;
    // SQINCD <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
    case SQINCD_r_rs_sx:
      mnemonic = "sqincd";
      form = "'Rd, 'Rd{, #'u0905{, MUL #<imm>}}";
      break;
    // SQINCD <Xdn>{, <pattern>{, MUL #<imm>}}
    case SQINCD_r_rs_x:
      mnemonic = "sqincd";
      break;
    // SQINCD <Zdn>.D{, <pattern>{, MUL #<imm>}}
    case SQINCD_z_zs:
      mnemonic = "sqincd";
      form = "'Zd.d{, #'u0905{, MUL #<imm>}}";
      break;
    // SQINCH <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
    case SQINCH_r_rs_sx:
      mnemonic = "sqinch";
      form = "'Rd, 'Rd{, #'u0905{, MUL #<imm>}}";
      break;
    // SQINCH <Xdn>{, <pattern>{, MUL #<imm>}}
    case SQINCH_r_rs_x:
      mnemonic = "sqinch";
      break;
    // SQINCH <Zdn>.H{, <pattern>{, MUL #<imm>}}
    case SQINCH_z_zs:
      mnemonic = "sqinch";
      form = "'Zd.h{, #'u0905{, MUL #<imm>}}";
      break;
    // SQINCW <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
    case SQINCW_r_rs_sx:
      mnemonic = "sqincw";
      form = "'Rd, 'Rd{, #'u0905{, MUL #<imm>}}";
      break;
    // SQINCW <Xdn>{, <pattern>{, MUL #<imm>}}
    case SQINCW_r_rs_x:
      mnemonic = "sqincw";
      break;
    // SQINCW <Zdn>.S{, <pattern>{, MUL #<imm>}}
    case SQINCW_z_zs:
      mnemonic = "sqincw";
      form = "'Zd.s{, #'u0905{, MUL #<imm>}}";
      break;
    // UQDECB <Wdn>{, <pattern>{, MUL #<imm>}}
    case UQDECB_r_rs_uw:
      mnemonic = "uqdecb";
      break;
    // UQDECB <Xdn>{, <pattern>{, MUL #<imm>}}
    case UQDECB_r_rs_x:
      mnemonic = "uqdecb";
      break;
    // UQDECD <Wdn>{, <pattern>{, MUL #<imm>}}
    case UQDECD_r_rs_uw:
      mnemonic = "uqdecd";
      break;
    // UQDECD <Xdn>{, <pattern>{, MUL #<imm>}}
    case UQDECD_r_rs_x:
      mnemonic = "uqdecd";
      break;
    // UQDECD <Zdn>.D{, <pattern>{, MUL #<imm>}}
    case UQDECD_z_zs:
      mnemonic = "uqdecd";
      form = "'Zd.d{, #'u0905{, MUL #<imm>}}";
      break;
    // UQDECH <Wdn>{, <pattern>{, MUL #<imm>}}
    case UQDECH_r_rs_uw:
      mnemonic = "uqdech";
      break;
    // UQDECH <Xdn>{, <pattern>{, MUL #<imm>}}
    case UQDECH_r_rs_x:
      mnemonic = "uqdech";
      break;
    // UQDECH <Zdn>.H{, <pattern>{, MUL #<imm>}}
    case UQDECH_z_zs:
      mnemonic = "uqdech";
      form = "'Zd.h{, #'u0905{, MUL #<imm>}}";
      break;
    // UQDECW <Wdn>{, <pattern>{, MUL #<imm>}}
    case UQDECW_r_rs_uw:
      mnemonic = "uqdecw";
      break;
    // UQDECW <Xdn>{, <pattern>{, MUL #<imm>}}
    case UQDECW_r_rs_x:
      mnemonic = "uqdecw";
      break;
    // UQDECW <Zdn>.S{, <pattern>{, MUL #<imm>}}
    case UQDECW_z_zs:
      mnemonic = "uqdecw";
      form = "'Zd.s{, #'u0905{, MUL #<imm>}}";
      break;
    // UQINCB <Wdn>{, <pattern>{, MUL #<imm>}}
    case UQINCB_r_rs_uw:
      mnemonic = "uqincb";
      break;
    // UQINCB <Xdn>{, <pattern>{, MUL #<imm>}}
    case UQINCB_r_rs_x:
      mnemonic = "uqincb";
      break;
    // UQINCD <Wdn>{, <pattern>{, MUL #<imm>}}
    case UQINCD_r_rs_uw:
      mnemonic = "uqincd";
      break;
    // UQINCD <Xdn>{, <pattern>{, MUL #<imm>}}
    case UQINCD_r_rs_x:
      mnemonic = "uqincd";
      break;
    // UQINCD <Zdn>.D{, <pattern>{, MUL #<imm>}}
    case UQINCD_z_zs:
      mnemonic = "uqincd";
      form = "'Zd.d{, #'u0905{, MUL #<imm>}}";
      break;
    // UQINCH <Wdn>{, <pattern>{, MUL #<imm>}}
    case UQINCH_r_rs_uw:
      mnemonic = "uqinch";
      break;
    // UQINCH <Xdn>{, <pattern>{, MUL #<imm>}}
    case UQINCH_r_rs_x:
      mnemonic = "uqinch";
      break;
    // UQINCH <Zdn>.H{, <pattern>{, MUL #<imm>}}
    case UQINCH_z_zs:
      mnemonic = "uqinch";
      form = "'Zd.h{, #'u0905{, MUL #<imm>}}";
      break;
    // UQINCW <Wdn>{, <pattern>{, MUL #<imm>}}
    case UQINCW_r_rs_uw:
      mnemonic = "uqincw";
      break;
    // UQINCW <Xdn>{, <pattern>{, MUL #<imm>}}
    case UQINCW_r_rs_x:
      mnemonic = "uqincw";
      break;
    // UQINCW <Zdn>.S{, <pattern>{, MUL #<imm>}}
    case UQINCW_z_zs:
      mnemonic = "uqincw";
      form = "'Zd.s{, #'u0905{, MUL #<imm>}}";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPAccumulatingReduction(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPAccumulatingReduction)";

  switch (instr->Mask(SVEFPAccumulatingReductionMask)) {
    // FADDA <V><dn>, <Pg>, <V><dn>, <Zm>.<T>
    case FADDA_v_p_z:
      mnemonic = "fadda";
      form = "'Vd, p'u1210, 'Vd, 'Zn.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPArithmeticPredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  const char *form = "'Zd.'t, p'u1210/m, 'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEFPArithmeticPredicatedMask)) {
    // FABD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FABD_z_p_zz:
      mnemonic = "fabd";
      break;
    // FADD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
    case FADD_z_p_zs:
      mnemonic = "fadd";
      form = "'Zd.'t, p'u1210/m, 'Zd.'t, <const>";
      break;
    // FADD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FADD_z_p_zz:
      mnemonic = "fadd";
      break;
    // FDIVR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FDIVR_z_p_zz:
      mnemonic = "fdivr";
      break;
    // FDIV <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FDIV_z_p_zz:
      mnemonic = "fdiv";
      break;
    // FMAXNM <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
    case FMAXNM_z_p_zs:
      mnemonic = "fmaxnm";
      form = "'Zd.'t, p'u1210/m, 'Zd.'t, <const>";
      break;
    // FMAXNM <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FMAXNM_z_p_zz:
      mnemonic = "fmaxnm";
      break;
    // FMAX <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
    case FMAX_z_p_zs:
      mnemonic = "fmax";
      form = "'Zd.'t, p'u1210/m, 'Zd.'t, <const>";
      break;
    // FMAX <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FMAX_z_p_zz:
      mnemonic = "fmax";
      break;
    // FMINNM <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
    case FMINNM_z_p_zs:
      mnemonic = "fminnm";
      form = "'Zd.'t, p'u1210/m, 'Zd.'t, <const>";
      break;
    // FMINNM <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FMINNM_z_p_zz:
      mnemonic = "fminnm";
      break;
    // FMIN <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
    case FMIN_z_p_zs:
      mnemonic = "fmin";
      form = "'Zd.'t, p'u1210/m, 'Zd.'t, <const>";
      break;
    // FMIN <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FMIN_z_p_zz:
      mnemonic = "fmin";
      break;
    // FMULX <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FMULX_z_p_zz:
      mnemonic = "fmulx";
      break;
    // FMUL <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
    case FMUL_z_p_zs:
      mnemonic = "fmul";
      form = "'Zd.'t, p'u1210/m, 'Zd.'t, <const>";
      break;
    // FMUL <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FMUL_z_p_zz:
      mnemonic = "fmul";
      break;
    // FSCALE <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FSCALE_z_p_zz:
      mnemonic = "fscale";
      break;
    // FSUBR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
    case FSUBR_z_p_zs:
      mnemonic = "fsubr";
      form = "'Zd.'t, p'u1210/m, 'Zd.'t, <const>";
      break;
    // FSUBR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FSUBR_z_p_zz:
      mnemonic = "fsubr";
      break;
    // FSUB <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
    case FSUB_z_p_zs:
      mnemonic = "fsub";
      form = "'Zd.'t, p'u1210/m, 'Zd.'t, <const>";
      break;
    // FSUB <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case FSUB_z_p_zz:
      mnemonic = "fsub";
      break;
    // FTMAD <Zdn>.<T>, <Zdn>.<T>, <Zm>.<T>, #<imm>
    case FTMAD_z_zzi:
      mnemonic = "ftmad";
      form = "'Zd.'t, 'Zd.'t, 'Zn.'t, #'u1816";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPArithmeticUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  const char *form = "'Zd.'t, 'Zn.'t, 'Zm.'t";

  switch (instr->Mask(SVEFPArithmeticUnpredicatedMask)) {
    // FADD <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case FADD_z_zz:
      mnemonic = "fadd";
      break;
    // FMUL <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case FMUL_z_zz:
      mnemonic = "fmul";
      break;
    // FRECPS <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case FRECPS_z_zz:
      mnemonic = "frecps";
      break;
    // FRSQRTS <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case FRSQRTS_z_zz:
      mnemonic = "frsqrts";
      break;
    // FSUB <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case FSUB_z_zz:
      mnemonic = "fsub";
      break;
    // FTSMUL <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case FTSMUL_z_zz:
      mnemonic = "ftsmul";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPCompareVectors(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
  const char *form = "'Pd.'t, p'u1210/z, 'Zn.'t, 'Zm.'t";

  switch (instr->Mask(SVEFPCompareVectorsMask)) {
    // FACGE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case FACGE_p_p_zz:
      mnemonic = "facge";
      break;
    // FACGT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case FACGT_p_p_zz:
      mnemonic = "facgt";
      break;
    // FCMEQ <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case FCMEQ_p_p_zz:
      mnemonic = "fcmeq";
      break;
    // FCMGE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case FCMGE_p_p_zz:
      mnemonic = "fcmge";
      break;
    // FCMGT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case FCMGT_p_p_zz:
      mnemonic = "fcmgt";
      break;
    // FCMNE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case FCMNE_p_p_zz:
      mnemonic = "fcmne";
      break;
    // FCMUO <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case FCMUO_p_p_zz:
      mnemonic = "fcmuo";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPCompareWithZero(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
  const char *form = "'Pd.'t, p'u1210/z, 'Zn.'t, #0.0";

  switch (instr->Mask(SVEFPCompareWithZeroMask)) {
    // FCMEQ <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
    case FCMEQ_p_p_z0:
      mnemonic = "fcmeq";
      break;
    // FCMGE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
    case FCMGE_p_p_z0:
      mnemonic = "fcmge";
      break;
    // FCMGT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
    case FCMGT_p_p_z0:
      mnemonic = "fcmgt";
      break;
    // FCMLE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
    case FCMLE_p_p_z0:
      mnemonic = "fcmle";
      break;
    // FCMLT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
    case FCMLT_p_p_z0:
      mnemonic = "fcmlt";
      break;
    // FCMNE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
    case FCMNE_p_p_z0:
      mnemonic = "fcmne";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPComplexAddition(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPComplexAddition)";

  switch (instr->Mask(SVEFPComplexAdditionMask)) {
    // FCADD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>, <const>
    case FCADD_z_p_zz:
      mnemonic = "fcadd";
      form = "'Zd.'t, p'u1210/m, 'Zd.'t, 'Zn.'t, <const>";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPComplexMulAdd(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPComplexMulAdd)";

  switch (instr->Mask(SVEFPComplexMulAddMask)) {
    // FCMLA <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>, <const>
    case FCMLA_z_p_zzz:
      mnemonic = "fcmla";
      form = "'Zd.'t, p'u1210/m, 'Zn.'t, 'Zm.'t, <const>";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPComplexMulAddIndex(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPComplexMulAddIndex)";

  switch (instr->Mask(SVEFPComplexMulAddIndexMask)) {
    // FCMLA <Zda>.H, <Zn>.H, <Zm>.H[<imm>], <const>
    case FCMLA_z_zzzi_h:
      mnemonic = "fcmla";
      form = "'Zd.h, 'Zn.h, <Zm>.h[<imm>], <const>";
      break;
    // FCMLA <Zda>.S, <Zn>.S, <Zm>.S[<imm>], <const>
    case FCMLA_z_zzzi_s:
      mnemonic = "fcmla";
      form = "'Zd.s, 'Zn.s, <Zm>.s[<imm>], <const>";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPFastReduction(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <V><d>, <Pg>, <Zn>.<T>
  const char *form = "'Vd, p'u1210, 'Zn.'t";

  switch (instr->Mask(SVEFPFastReductionMask)) {
    // FADDV <V><d>, <Pg>, <Zn>.<T>
    case FADDV_v_p_z:
      mnemonic = "faddv";
      break;
    // FMAXNMV <V><d>, <Pg>, <Zn>.<T>
    case FMAXNMV_v_p_z:
      mnemonic = "fmaxnmv";
      break;
    // FMAXV <V><d>, <Pg>, <Zn>.<T>
    case FMAXV_v_p_z:
      mnemonic = "fmaxv";
      break;
    // FMINNMV <V><d>, <Pg>, <Zn>.<T>
    case FMINNMV_v_p_z:
      mnemonic = "fminnmv";
      break;
    // FMINV <V><d>, <Pg>, <Zn>.<T>
    case FMINV_v_p_z:
      mnemonic = "fminv";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPMulIndex(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPMulIndex)";

  switch (instr->Mask(SVEFPMulIndexMask)) {
    // FMUL <Zd>.D, <Zn>.D, <Zm>.D[<imm>]
    case FMUL_z_zzi_d:
      mnemonic = "fmul";
      form = "'Zd.d, 'Zn.d, <Zm>.d[<imm>]";
      break;
    // FMUL <Zd>.H, <Zn>.H, <Zm>.H[<imm>]
    case FMUL_z_zzi_h:
      mnemonic = "fmul";
      form = "'Zd.h, 'Zn.h, <Zm>.h[<imm>]";
      break;
    // FMUL <Zd>.S, <Zn>.S, <Zm>.S[<imm>]
    case FMUL_z_zzi_s:
      mnemonic = "fmul";
      form = "'Zd.s, 'Zn.s, <Zm>.s[<imm>]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPMulAdd(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
  const char *form = "'Zd.'t, p'u1210/m, 'Zn.'t, 'Zm.'t";

  switch (instr->Mask(SVEFPMulAddMask)) {
    // FMAD <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
    case FMAD_z_p_zzz:
      mnemonic = "fmad";
      break;
    // FMLA <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>
    case FMLA_z_p_zzz:
      mnemonic = "fmla";
      break;
    // FMLS <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>
    case FMLS_z_p_zzz:
      mnemonic = "fmls";
      break;
    // FMSB <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
    case FMSB_z_p_zzz:
      mnemonic = "fmsb";
      break;
    // FNMAD <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
    case FNMAD_z_p_zzz:
      mnemonic = "fnmad";
      break;
    // FNMLA <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>
    case FNMLA_z_p_zzz:
      mnemonic = "fnmla";
      break;
    // FNMLS <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>
    case FNMLS_z_p_zzz:
      mnemonic = "fnmls";
      break;
    // FNMSB <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
    case FNMSB_z_p_zzz:
      mnemonic = "fnmsb";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPMulAddIndex(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPMulAddIndex)";

  switch (instr->Mask(SVEFPMulAddIndexMask)) {
    // FMLA <Zda>.D, <Zn>.D, <Zm>.D[<imm>]
    case FMLA_z_zzzi_d:
      mnemonic = "fmla";
      form = "'Zd.d, 'Zn.d, <Zm>.d[<imm>]";
      break;
    // FMLA <Zda>.H, <Zn>.H, <Zm>.H[<imm>]
    case FMLA_z_zzzi_h:
      mnemonic = "fmla";
      form = "'Zd.h, 'Zn.h, <Zm>.h[<imm>]";
      break;
    // FMLA <Zda>.S, <Zn>.S, <Zm>.S[<imm>]
    case FMLA_z_zzzi_s:
      mnemonic = "fmla";
      form = "'Zd.s, 'Zn.s, <Zm>.s[<imm>]";
      break;
    // FMLS <Zda>.D, <Zn>.D, <Zm>.D[<imm>]
    case FMLS_z_zzzi_d:
      mnemonic = "fmls";
      form = "'Zd.d, 'Zn.d, <Zm>.d[<imm>]";
      break;
    // FMLS <Zda>.H, <Zn>.H, <Zm>.H[<imm>]
    case FMLS_z_zzzi_h:
      mnemonic = "fmls";
      form = "'Zd.h, 'Zn.h, <Zm>.h[<imm>]";
      break;
    // FMLS <Zda>.S, <Zn>.S, <Zm>.S[<imm>]
    case FMLS_z_zzzi_s:
      mnemonic = "fmls";
      form = "'Zd.s, 'Zn.s, <Zm>.s[<imm>]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPUnaryOpPredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zd>.<T>, <Pg>/M, <Zn>.<T>
  const char *form = "'Zd.'t, p'u1210/m, 'Zn.'t";

  switch (instr->Mask(SVEFPUnaryOpPredicatedMask)) {
    // FCVTZS <Zd>.S, <Pg>/M, <Zn>.D
    case FCVTZS_z_p_z_d2w:
      mnemonic = "fcvtzs";
      form = "'Zd.s, p'u1210/m, 'Zn.d";
      break;
    // FCVTZS <Zd>.D, <Pg>/M, <Zn>.D
    case FCVTZS_z_p_z_d2x:
      mnemonic = "fcvtzs";
      form = "'Zd.d, p'u1210/m, 'Zn.d";
      break;
    // FCVTZS <Zd>.H, <Pg>/M, <Zn>.H
    case FCVTZS_z_p_z_fp162h:
      mnemonic = "fcvtzs";
      form = "'Zd.h, p'u1210/m, 'Zn.h";
      break;
    // FCVTZS <Zd>.S, <Pg>/M, <Zn>.H
    case FCVTZS_z_p_z_fp162w:
      mnemonic = "fcvtzs";
      form = "'Zd.s, p'u1210/m, 'Zn.h";
      break;
    // FCVTZS <Zd>.D, <Pg>/M, <Zn>.H
    case FCVTZS_z_p_z_fp162x:
      mnemonic = "fcvtzs";
      form = "'Zd.d, p'u1210/m, 'Zn.h";
      break;
    // FCVTZS <Zd>.S, <Pg>/M, <Zn>.S
    case FCVTZS_z_p_z_s2w:
      mnemonic = "fcvtzs";
      form = "'Zd.s, p'u1210/m, 'Zn.s";
      break;
    // FCVTZS <Zd>.D, <Pg>/M, <Zn>.S
    case FCVTZS_z_p_z_s2x:
      mnemonic = "fcvtzs";
      form = "'Zd.d, p'u1210/m, 'Zn.s";
      break;
    // FCVTZU <Zd>.S, <Pg>/M, <Zn>.D
    case FCVTZU_z_p_z_d2w:
      mnemonic = "fcvtzu";
      form = "'Zd.s, p'u1210/m, 'Zn.d";
      break;
    // FCVTZU <Zd>.D, <Pg>/M, <Zn>.D
    case FCVTZU_z_p_z_d2x:
      mnemonic = "fcvtzu";
      form = "'Zd.d, p'u1210/m, 'Zn.d";
      break;
    // FCVTZU <Zd>.H, <Pg>/M, <Zn>.H
    case FCVTZU_z_p_z_fp162h:
      mnemonic = "fcvtzu";
      form = "'Zd.h, p'u1210/m, 'Zn.h";
      break;
    // FCVTZU <Zd>.S, <Pg>/M, <Zn>.H
    case FCVTZU_z_p_z_fp162w:
      mnemonic = "fcvtzu";
      form = "'Zd.s, p'u1210/m, 'Zn.h";
      break;
    // FCVTZU <Zd>.D, <Pg>/M, <Zn>.H
    case FCVTZU_z_p_z_fp162x:
      mnemonic = "fcvtzu";
      form = "'Zd.d, p'u1210/m, 'Zn.h";
      break;
    // FCVTZU <Zd>.S, <Pg>/M, <Zn>.S
    case FCVTZU_z_p_z_s2w:
      mnemonic = "fcvtzu";
      form = "'Zd.s, p'u1210/m, 'Zn.s";
      break;
    // FCVTZU <Zd>.D, <Pg>/M, <Zn>.S
    case FCVTZU_z_p_z_s2x:
      mnemonic = "fcvtzu";
      form = "'Zd.d, p'u1210/m, 'Zn.s";
      break;
    // FCVT <Zd>.H, <Pg>/M, <Zn>.D
    case FCVT_z_p_z_d2h:
      mnemonic = "fcvt";
      form = "'Zd.h, p'u1210/m, 'Zn.d";
      break;
    // FCVT <Zd>.S, <Pg>/M, <Zn>.D
    case FCVT_z_p_z_d2s:
      mnemonic = "fcvt";
      form = "'Zd.s, p'u1210/m, 'Zn.d";
      break;
    // FCVT <Zd>.D, <Pg>/M, <Zn>.H
    case FCVT_z_p_z_h2d:
      mnemonic = "fcvt";
      form = "'Zd.d, p'u1210/m, 'Zn.h";
      break;
    // FCVT <Zd>.S, <Pg>/M, <Zn>.H
    case FCVT_z_p_z_h2s:
      mnemonic = "fcvt";
      form = "'Zd.s, p'u1210/m, 'Zn.h";
      break;
    // FCVT <Zd>.D, <Pg>/M, <Zn>.S
    case FCVT_z_p_z_s2d:
      mnemonic = "fcvt";
      form = "'Zd.d, p'u1210/m, 'Zn.s";
      break;
    // FCVT <Zd>.H, <Pg>/M, <Zn>.S
    case FCVT_z_p_z_s2h:
      mnemonic = "fcvt";
      form = "'Zd.h, p'u1210/m, 'Zn.s";
      break;
    // FRECPX <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case FRECPX_z_p_z:
      mnemonic = "frecpx";
      break;
    // FRINTA <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case FRINTA_z_p_z:
      mnemonic = "frinta";
      break;
    // FRINTI <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case FRINTI_z_p_z:
      mnemonic = "frinti";
      break;
    // FRINTM <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case FRINTM_z_p_z:
      mnemonic = "frintm";
      break;
    // FRINTN <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case FRINTN_z_p_z:
      mnemonic = "frintn";
      break;
    // FRINTP <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case FRINTP_z_p_z:
      mnemonic = "frintp";
      break;
    // FRINTX <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case FRINTX_z_p_z:
      mnemonic = "frintx";
      break;
    // FRINTZ <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case FRINTZ_z_p_z:
      mnemonic = "frintz";
      break;
    // FSQRT <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case FSQRT_z_p_z:
      mnemonic = "fsqrt";
      break;
    // SCVTF <Zd>.H, <Pg>/M, <Zn>.H
    case SCVTF_z_p_z_h2fp16:
      mnemonic = "scvtf";
      form = "'Zd.h, p'u1210/m, 'Zn.h";
      break;
    // SCVTF <Zd>.D, <Pg>/M, <Zn>.S
    case SCVTF_z_p_z_w2d:
      mnemonic = "scvtf";
      form = "'Zd.d, p'u1210/m, 'Zn.s";
      break;
    // SCVTF <Zd>.H, <Pg>/M, <Zn>.S
    case SCVTF_z_p_z_w2fp16:
      mnemonic = "scvtf";
      form = "'Zd.h, p'u1210/m, 'Zn.s";
      break;
    // SCVTF <Zd>.S, <Pg>/M, <Zn>.S
    case SCVTF_z_p_z_w2s:
      mnemonic = "scvtf";
      form = "'Zd.s, p'u1210/m, 'Zn.s";
      break;
    // SCVTF <Zd>.D, <Pg>/M, <Zn>.D
    case SCVTF_z_p_z_x2d:
      mnemonic = "scvtf";
      form = "'Zd.d, p'u1210/m, 'Zn.d";
      break;
    // SCVTF <Zd>.H, <Pg>/M, <Zn>.D
    case SCVTF_z_p_z_x2fp16:
      mnemonic = "scvtf";
      form = "'Zd.h, p'u1210/m, 'Zn.d";
      break;
    // SCVTF <Zd>.S, <Pg>/M, <Zn>.D
    case SCVTF_z_p_z_x2s:
      mnemonic = "scvtf";
      form = "'Zd.s, p'u1210/m, 'Zn.d";
      break;
    // UCVTF <Zd>.H, <Pg>/M, <Zn>.H
    case UCVTF_z_p_z_h2fp16:
      mnemonic = "ucvtf";
      form = "'Zd.h, p'u1210/m, 'Zn.h";
      break;
    // UCVTF <Zd>.D, <Pg>/M, <Zn>.S
    case UCVTF_z_p_z_w2d:
      mnemonic = "ucvtf";
      form = "'Zd.d, p'u1210/m, 'Zn.s";
      break;
    // UCVTF <Zd>.H, <Pg>/M, <Zn>.S
    case UCVTF_z_p_z_w2fp16:
      mnemonic = "ucvtf";
      form = "'Zd.h, p'u1210/m, 'Zn.s";
      break;
    // UCVTF <Zd>.S, <Pg>/M, <Zn>.S
    case UCVTF_z_p_z_w2s:
      mnemonic = "ucvtf";
      form = "'Zd.s, p'u1210/m, 'Zn.s";
      break;
    // UCVTF <Zd>.D, <Pg>/M, <Zn>.D
    case UCVTF_z_p_z_x2d:
      mnemonic = "ucvtf";
      form = "'Zd.d, p'u1210/m, 'Zn.d";
      break;
    // UCVTF <Zd>.H, <Pg>/M, <Zn>.D
    case UCVTF_z_p_z_x2fp16:
      mnemonic = "ucvtf";
      form = "'Zd.h, p'u1210/m, 'Zn.d";
      break;
    // UCVTF <Zd>.S, <Pg>/M, <Zn>.D
    case UCVTF_z_p_z_x2s:
      mnemonic = "ucvtf";
      form = "'Zd.s, p'u1210/m, 'Zn.d";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPUnaryOpUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zd>.<T>, <Zn>.<T>
  const char *form = "'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEFPUnaryOpUnpredicatedMask)) {
    // FRECPE <Zd>.<T>, <Zn>.<T>
    case FRECPE_z_z:
      mnemonic = "frecpe";
      break;
    // FRSQRTE <Zd>.<T>, <Zn>.<T>
    case FRSQRTE_z_z:
      mnemonic = "frsqrte";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIncDecByPredicateCount(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zdn>.<T>, <Pg>
  const char *form = "'Zd.'t, p'u0805";

  switch (instr->Mask(SVEIncDecByPredicateCountMask)) {
    // DECP <Xdn>, <Pg>.<T>
    case DECP_r_p_r:
      mnemonic = "decp";
      form = "'Rd, p'u0805.'t";
      break;
    // DECP <Zdn>.<T>, <Pg>
    case DECP_z_p_z:
      mnemonic = "decp";
      break;
    // INCP <Xdn>, <Pg>.<T>
    case INCP_r_p_r:
      mnemonic = "incp";
      form = "'Rd, p'u0805.'t";
      break;
    // INCP <Zdn>.<T>, <Pg>
    case INCP_z_p_z:
      mnemonic = "incp";
      break;
    // SQDECP <Xdn>, <Pg>.<T>, <Wdn>
    case SQDECP_r_p_r_sx:
      mnemonic = "sqdecp";
      form = "'Rd, p'u0805.'t, 'Rd";
      break;
    // SQDECP <Xdn>, <Pg>.<T>
    case SQDECP_r_p_r_x:
      mnemonic = "sqdecp";
      form = "'Rd, p'u0805.'t";
      break;
    // SQDECP <Zdn>.<T>, <Pg>
    case SQDECP_z_p_z:
      mnemonic = "sqdecp";
      break;
    // SQINCP <Xdn>, <Pg>.<T>, <Wdn>
    case SQINCP_r_p_r_sx:
      mnemonic = "sqincp";
      form = "'Rd, p'u0805.'t, 'Rd";
      break;
    // SQINCP <Xdn>, <Pg>.<T>
    case SQINCP_r_p_r_x:
      mnemonic = "sqincp";
      form = "'Rd, p'u0805.'t";
      break;
    // SQINCP <Zdn>.<T>, <Pg>
    case SQINCP_z_p_z:
      mnemonic = "sqincp";
      break;
    // UQDECP <Wdn>, <Pg>.<T>
    case UQDECP_r_p_r_uw:
      mnemonic = "uqdecp";
      form = "'Rd, p'u0805.'t";
      break;
    // UQDECP <Xdn>, <Pg>.<T>
    case UQDECP_r_p_r_x:
      mnemonic = "uqdecp";
      form = "'Rd, p'u0805.'t";
      break;
    // UQDECP <Zdn>.<T>, <Pg>
    case UQDECP_z_p_z:
      mnemonic = "uqdecp";
      break;
    // UQINCP <Wdn>, <Pg>.<T>
    case UQINCP_r_p_r_uw:
      mnemonic = "uqincp";
      form = "'Rd, p'u0805.'t";
      break;
    // UQINCP <Xdn>, <Pg>.<T>
    case UQINCP_r_p_r_x:
      mnemonic = "uqincp";
      form = "'Rd, p'u0805.'t";
      break;
    // UQINCP <Zdn>.<T>, <Pg>
    case UQINCP_z_p_z:
      mnemonic = "uqincp";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIndexGeneration(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEIndexGeneration)";

  switch (instr->Mask(SVEIndexGenerationMask)) {
    // INDEX <Zd>.<T>, #<imm1>, #<imm2>
    case INDEX_z_ii:
      mnemonic = "index";
      form = "'Zd.'t, #<imm1>, #<imm2>";
      break;
    // INDEX <Zd>.<T>, #<imm>, <R><m>
    case INDEX_z_ir:
      mnemonic = "index";
      form = "'Zd.'t, #'u0905, 'Rm";
      break;
    // INDEX <Zd>.<T>, <R><n>, #<imm>
    case INDEX_z_ri:
      mnemonic = "index";
      form = "'Zd.'t, 'Rn, #'u2016";
      break;
    // INDEX <Zd>.<T>, <R><n>, <R><m>
    case INDEX_z_rr:
      mnemonic = "index";
      form = "'Zd.'t, 'Rn, 'Rm";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntArithmeticUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  const char *form = "'Zd.'t, 'Zn.'t, 'Zm.'t";

  switch (instr->Mask(SVEIntArithmeticUnpredicatedMask)) {
    // ADD <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case ADD_z_zz:
      mnemonic = "add";
      break;
    // SQADD <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case SQADD_z_zz:
      mnemonic = "sqadd";
      break;
    // SQSUB <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case SQSUB_z_zz:
      mnemonic = "sqsub";
      break;
    // SUB <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case SUB_z_zz:
      mnemonic = "sub";
      break;
    // UQADD <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case UQADD_z_zz:
      mnemonic = "uqadd";
      break;
    // UQSUB <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case UQSUB_z_zz:
      mnemonic = "uqsub";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntBinaryArithmeticPredicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  const char *form = "'Zd.'t, p'u1210/m, 'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEIntBinaryArithmeticPredicatedMask)) {
    // ADD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case ADD_z_p_zz:
      mnemonic = "add";
      break;
    // AND <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case AND_z_p_zz:
      mnemonic = "and";
      break;
    // BIC <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case BIC_z_p_zz:
      mnemonic = "bic";
      break;
    // EOR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case EOR_z_p_zz:
      mnemonic = "eor";
      break;
    // MUL <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case MUL_z_p_zz:
      mnemonic = "mul";
      break;
    // ORR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case ORR_z_p_zz:
      mnemonic = "orr";
      break;
    // SABD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case SABD_z_p_zz:
      mnemonic = "sabd";
      break;
    // SDIVR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case SDIVR_z_p_zz:
      mnemonic = "sdivr";
      form = "'Zd.<T>, p'u1210/m, 'Zd.<T>, 'Zn.<T>";
      break;
    // SDIV <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case SDIV_z_p_zz:
      mnemonic = "sdiv";
      form = "'Zd.<T>, p'u1210/m, 'Zd.<T>, 'Zn.<T>";
      break;
    // SMAX <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case SMAX_z_p_zz:
      mnemonic = "smax";
      break;
    // SMIN <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case SMIN_z_p_zz:
      mnemonic = "smin";
      break;
    // SMULH <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case SMULH_z_p_zz:
      mnemonic = "smulh";
      break;
    // SUBR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case SUBR_z_p_zz:
      mnemonic = "subr";
      break;
    // SUB <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case SUB_z_p_zz:
      mnemonic = "sub";
      break;
    // UABD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case UABD_z_p_zz:
      mnemonic = "uabd";
      break;
    // UDIVR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case UDIVR_z_p_zz:
      mnemonic = "udivr";
      form = "'Zd.<T>, p'u1210/m, 'Zd.<T>, 'Zn.<T>";
      break;
    // UDIV <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case UDIV_z_p_zz:
      mnemonic = "udiv";
      form = "'Zd.<T>, p'u1210/m, 'Zd.<T>, 'Zn.<T>";
      break;
    // UMAX <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case UMAX_z_p_zz:
      mnemonic = "umax";
      break;
    // UMIN <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case UMIN_z_p_zz:
      mnemonic = "umin";
      break;
    // UMULH <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
    case UMULH_z_p_zz:
      mnemonic = "umulh";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntCompareScalars(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Pd>.<T>, <R><n>, <R><m>
  const char *form = "'Pd.'t, 'Rn, 'Rm";

  switch (instr->Mask(SVEIntCompareScalarsMask)) {
    // CTERMEQ <R><n>, <R><m>
    case CTERMEQ_rr:
      mnemonic = "ctermeq";
      form = "'Rn, 'Rm";
      break;
    // CTERMNE <R><n>, <R><m>
    case CTERMNE_rr:
      mnemonic = "ctermne";
      form = "'Rn, 'Rm";
      break;
    // WHILELE <Pd>.<T>, <R><n>, <R><m>
    case WHILELE_p_p_rr:
      mnemonic = "whilele";
      break;
    // WHILELO <Pd>.<T>, <R><n>, <R><m>
    case WHILELO_p_p_rr:
      mnemonic = "whilelo";
      break;
    // WHILELS <Pd>.<T>, <R><n>, <R><m>
    case WHILELS_p_p_rr:
      mnemonic = "whilels";
      break;
    // WHILELT <Pd>.<T>, <R><n>, <R><m>
    case WHILELT_p_p_rr:
      mnemonic = "whilelt";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntCompareSignedImm(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
  const char *form = "'Pd.'t, p'u1210/z, 'Zn.'t, #'u2016";

  switch (instr->Mask(SVEIntCompareSignedImmMask)) {
    // CMPEQ <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
    case CMPEQ_p_p_zi:
      mnemonic = "cmpeq";
      break;
    // CMPGE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
    case CMPGE_p_p_zi:
      mnemonic = "cmpge";
      break;
    // CMPGT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
    case CMPGT_p_p_zi:
      mnemonic = "cmpgt";
      break;
    // CMPLE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
    case CMPLE_p_p_zi:
      mnemonic = "cmple";
      break;
    // CMPLT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
    case CMPLT_p_p_zi:
      mnemonic = "cmplt";
      break;
    // CMPNE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
    case CMPNE_p_p_zi:
      mnemonic = "cmpne";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntCompareUnsignedImm(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
  const char *form = "'Pd.'t, p'u1210/z, 'Zn.'t, #'u2014";

  switch (instr->Mask(SVEIntCompareUnsignedImmMask)) {
    // CMPHI <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
    case CMPHI_p_p_zi:
      mnemonic = "cmphi";
      break;
    // CMPHS <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
    case CMPHS_p_p_zi:
      mnemonic = "cmphs";
      break;
    // CMPLO <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
    case CMPLO_p_p_zi:
      mnemonic = "cmplo";
      break;
    // CMPLS <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
    case CMPLS_p_p_zi:
      mnemonic = "cmpls";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntCompareVectors(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
  const char *form = "'Pd.'t, p'u1210/z, 'Zn.'t, 'Zm.d";

  switch (instr->Mask(SVEIntCompareVectorsMask)) {
    // CMPEQ <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
    case CMPEQ_p_p_zw:
      mnemonic = "cmpeq";
      break;
    // CMPEQ <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case CMPEQ_p_p_zz:
      mnemonic = "cmpeq";
      form = "'Pd.'t, p'u1210/z, 'Zn.'t, 'Zm.'t";
      break;
    // CMPGE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
    case CMPGE_p_p_zw:
      mnemonic = "cmpge";
      break;
    // CMPGE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case CMPGE_p_p_zz:
      mnemonic = "cmpge";
      form = "'Pd.'t, p'u1210/z, 'Zn.'t, 'Zm.'t";
      break;
    // CMPGT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
    case CMPGT_p_p_zw:
      mnemonic = "cmpgt";
      break;
    // CMPGT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case CMPGT_p_p_zz:
      mnemonic = "cmpgt";
      form = "'Pd.'t, p'u1210/z, 'Zn.'t, 'Zm.'t";
      break;
    // CMPHI <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
    case CMPHI_p_p_zw:
      mnemonic = "cmphi";
      break;
    // CMPHI <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case CMPHI_p_p_zz:
      mnemonic = "cmphi";
      form = "'Pd.'t, p'u1210/z, 'Zn.'t, 'Zm.'t";
      break;
    // CMPHS <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
    case CMPHS_p_p_zw:
      mnemonic = "cmphs";
      break;
    // CMPHS <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case CMPHS_p_p_zz:
      mnemonic = "cmphs";
      form = "'Pd.'t, p'u1210/z, 'Zn.'t, 'Zm.'t";
      break;
    // CMPLE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
    case CMPLE_p_p_zw:
      mnemonic = "cmple";
      break;
    // CMPLO <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
    case CMPLO_p_p_zw:
      mnemonic = "cmplo";
      break;
    // CMPLS <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
    case CMPLS_p_p_zw:
      mnemonic = "cmpls";
      break;
    // CMPLT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
    case CMPLT_p_p_zw:
      mnemonic = "cmplt";
      break;
    // CMPNE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
    case CMPNE_p_p_zw:
      mnemonic = "cmpne";
      break;
    // CMPNE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
    case CMPNE_p_p_zz:
      mnemonic = "cmpne";
      form = "'Pd.'t, p'u1210/z, 'Zn.'t, 'Zm.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntMiscUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEIntMiscUnpredicated)";

  switch (instr->Mask(SVEIntMiscUnpredicatedMask)) {
    // FEXPA <Zd>.<T>, <Zn>.<T>
    case FEXPA_z_z:
      mnemonic = "fexpa";
      form = "'Zd.'t, 'Zn.'t";
      break;
    // FTSSEL <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case FTSSEL_z_zz:
      mnemonic = "ftssel";
      form = "'Zd.'t, 'Zn.'t, 'Zm.'t";
      break;
    // MOVPRFX <Zd>, <Zn>
    case MOVPRFX_z_z:
      mnemonic = "movprfx";
      form = "'Zd, 'Zn";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntMulAddPredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEIntMulAddPredicated)";

  switch (instr->Mask(SVEIntMulAddPredicatedMask)) {
    // MAD <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
    case MAD_z_p_zzz:
      mnemonic = "mad";
      form = "'Zd.'t, p'u1210/m, 'Zm.'t, 'Zn.'t";
      break;
    // MLA <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>
    case MLA_z_p_zzz:
      mnemonic = "mla";
      form = "'Zd.'t, p'u1210/m, 'Zn.'t, 'Zm.'t";
      break;
    // MLS <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>
    case MLS_z_p_zzz:
      mnemonic = "mls";
      form = "'Zd.'t, p'u1210/m, 'Zn.'t, 'Zm.'t";
      break;
    // MSB <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
    case MSB_z_p_zzz:
      mnemonic = "msb";
      form = "'Zd.'t, p'u1210/m, 'Zm.'t, 'Zn.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntMulAddUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zda>.<T>, <Zn>.<Tb>, <Zm>.<Tb>
  const char *form = "'Zd.<T>, 'Zn, 'Zm";

  switch (instr->Mask(SVEIntMulAddUnpredicatedMask)) {
    // SDOT <Zda>.<T>, <Zn>.<Tb>, <Zm>.<Tb>
    case SDOT_z_zzz:
      mnemonic = "sdot";
      break;
    // UDOT <Zda>.<T>, <Zn>.<Tb>, <Zm>.<Tb>
    case UDOT_z_zzz:
      mnemonic = "udot";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntReduction(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <V><d>, <Pg>, <Zn>.<T>
  const char *form = "'Vd, p'u1210, 'Zn.'t";

  switch (instr->Mask(SVEIntReductionMask)) {
    // ANDV <V><d>, <Pg>, <Zn>.<T>
    case ANDV_r_p_z:
      mnemonic = "andv";
      break;
    // EORV <V><d>, <Pg>, <Zn>.<T>
    case EORV_r_p_z:
      mnemonic = "eorv";
      break;
    // MOVPRFX <Zd>.<T>, <Pg>/<ZM>, <Zn>.<T>
    case MOVPRFX_z_p_z:
      mnemonic = "movprfx";
      form = "'Zd.'t, p'u1210/<ZM>, 'Zn.'t";
      break;
    // ORV <V><d>, <Pg>, <Zn>.<T>
    case ORV_r_p_z:
      mnemonic = "orv";
      break;
    // SADDV <Dd>, <Pg>, <Zn>.<T>
    case SADDV_r_p_z:
      mnemonic = "saddv";
      form = "'Dd, p'u1210, 'Zn.'t";
      break;
    // SMAXV <V><d>, <Pg>, <Zn>.<T>
    case SMAXV_r_p_z:
      mnemonic = "smaxv";
      break;
    // SMINV <V><d>, <Pg>, <Zn>.<T>
    case SMINV_r_p_z:
      mnemonic = "sminv";
      break;
    // UADDV <Dd>, <Pg>, <Zn>.<T>
    case UADDV_r_p_z:
      mnemonic = "uaddv";
      form = "'Dd, p'u1210, 'Zn.'t";
      break;
    // UMAXV <V><d>, <Pg>, <Zn>.<T>
    case UMAXV_r_p_z:
      mnemonic = "umaxv";
      break;
    // UMINV <V><d>, <Pg>, <Zn>.<T>
    case UMINV_r_p_z:
      mnemonic = "uminv";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntUnaryArithmeticPredicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zd>.<T>, <Pg>/M, <Zn>.<T>
  const char *form = "'Zd.'t, p'u1210/m, 'Zn.'t";

  switch (instr->Mask(SVEIntUnaryArithmeticPredicatedMask)) {
    // ABS <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case ABS_z_p_z:
      mnemonic = "abs";
      break;
    // CLS <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case CLS_z_p_z:
      mnemonic = "cls";
      break;
    // CLZ <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case CLZ_z_p_z:
      mnemonic = "clz";
      break;
    // CNOT <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case CNOT_z_p_z:
      mnemonic = "cnot";
      break;
    // CNT <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case CNT_z_p_z:
      mnemonic = "cnt";
      break;
    // FABS <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case FABS_z_p_z:
      mnemonic = "fabs";
      break;
    // FNEG <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case FNEG_z_p_z:
      mnemonic = "fneg";
      break;
    // NEG <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case NEG_z_p_z:
      mnemonic = "neg";
      break;
    // NOT <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case NOT_z_p_z:
      mnemonic = "not";
      break;
    // SXTB <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case SXTB_z_p_z:
      mnemonic = "sxtb";
      break;
    // SXTH <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case SXTH_z_p_z:
      mnemonic = "sxth";
      form = "'Zd.<T>, p'u1210/m, 'Zn.<T>";
      break;
    // SXTW <Zd>.D, <Pg>/M, <Zn>.D
    case SXTW_z_p_z:
      mnemonic = "sxtw";
      form = "'Zd.d, p'u1210/m, 'Zn.d";
      break;
    // UXTB <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case UXTB_z_p_z:
      mnemonic = "uxtb";
      break;
    // UXTH <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case UXTH_z_p_z:
      mnemonic = "uxth";
      form = "'Zd.<T>, p'u1210/m, 'Zn.<T>";
      break;
    // UXTW <Zd>.D, <Pg>/M, <Zn>.D
    case UXTW_z_p_z:
      mnemonic = "uxtw";
      form = "'Zd.d, p'u1210/m, 'Zn.d";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntWideImmPredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEIntWideImmPredicated)";

  switch (instr->Mask(SVEIntWideImmPredicatedMask)) {
    // CPY <Zd>.<T>, <Pg>/<ZM>, #<imm>{, <shift>}
    case CPY_z_p_i:
      mnemonic = "cpy";
      form = "'Zd.'t, p'u1916/<ZM>, #'u1205{, <shift>}";
      break;
    // FCPY <Zd>.<T>, <Pg>/M, #<const>
    case FCPY_z_p_i:
      mnemonic = "fcpy";
      form = "'Zd.'t, p'u1916/m, #<const>";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntWideImmUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
  const char *form = "'Zd.'t, 'Zd.'t, #'u1205{, <shift>}";

  switch (instr->Mask(SVEIntWideImmUnpredicatedMask)) {
    // ADD <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
    case ADD_z_zi:
      mnemonic = "add";
      break;
    // DUP <Zd>.<T>, #<imm>{, <shift>}
    case DUP_z_i:
      mnemonic = "dup";
      form = "'Zd.'t, #'u1205{, <shift>}";
      break;
    // FDUP <Zd>.<T>, #<const>
    case FDUP_z_i:
      mnemonic = "fdup";
      form = "'Zd.'t, #<const>";
      break;
    // MUL <Zdn>.<T>, <Zdn>.<T>, #<imm>
    case MUL_z_zi:
      mnemonic = "mul";
      form = "'Zd.'t, 'Zd.'t, #'u1205";
      break;
    // SMAX <Zdn>.<T>, <Zdn>.<T>, #<imm>
    case SMAX_z_zi:
      mnemonic = "smax";
      form = "'Zd.'t, 'Zd.'t, #'u1205";
      break;
    // SMIN <Zdn>.<T>, <Zdn>.<T>, #<imm>
    case SMIN_z_zi:
      mnemonic = "smin";
      form = "'Zd.'t, 'Zd.'t, #'u1205";
      break;
    // SQADD <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
    case SQADD_z_zi:
      mnemonic = "sqadd";
      break;
    // SQSUB <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
    case SQSUB_z_zi:
      mnemonic = "sqsub";
      break;
    // SUBR <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
    case SUBR_z_zi:
      mnemonic = "subr";
      break;
    // SUB <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
    case SUB_z_zi:
      mnemonic = "sub";
      break;
    // UMAX <Zdn>.<T>, <Zdn>.<T>, #<imm>
    case UMAX_z_zi:
      mnemonic = "umax";
      form = "'Zd.'t, 'Zd.'t, #'u1205";
      break;
    // UMIN <Zdn>.<T>, <Zdn>.<T>, #<imm>
    case UMIN_z_zi:
      mnemonic = "umin";
      form = "'Zd.'t, 'Zd.'t, #'u1205";
      break;
    // UQADD <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
    case UQADD_z_zi:
      mnemonic = "uqadd";
      break;
    // UQSUB <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
    case UQSUB_z_zi:
      mnemonic = "uqsub";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEMem32BitGatherAndUnsizedContiguous(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]
  const char *form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Zm.s, <mod>]";

  switch (instr->Mask(SVEMem32BitGatherAndUnsizedContiguousMask)) {
    // LD1B { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]
    case LD1B_z_p_ai_s:
      mnemonic = "ld1b";
      form = "{ 'Zt.s }, p'u1210/z, ['Zn.s{, #'u2016}]";
      break;
    // LD1B { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]
    case LD1B_z_p_bz_s_x32_unscaled:
      mnemonic = "ld1b";
      break;
    // LD1H { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]
    case LD1H_z_p_ai_s:
      mnemonic = "ld1h";
      form = "{ 'Zt.s }, p'u1210/z, ['Zn.s{, #'u2016}]";
      break;
    // LD1H { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod> #1]
    case LD1H_z_p_bz_s_x32_scaled:
      mnemonic = "ld1h";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Zm.s, <mod> #1]";
      break;
    // LD1H { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]
    case LD1H_z_p_bz_s_x32_unscaled:
      mnemonic = "ld1h";
      break;
    // LD1RB { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RB_z_p_bi_u16:
      mnemonic = "ld1rb";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RB { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RB_z_p_bi_u32:
      mnemonic = "ld1rb";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RB { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RB_z_p_bi_u64:
      mnemonic = "ld1rb";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RB { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RB_z_p_bi_u8:
      mnemonic = "ld1rb";
      form = "{ 'Zt.b }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RD { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RD_z_p_bi_u64:
      mnemonic = "ld1rd";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RH { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RH_z_p_bi_u16:
      mnemonic = "ld1rh";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RH { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RH_z_p_bi_u32:
      mnemonic = "ld1rh";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RH { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RH_z_p_bi_u64:
      mnemonic = "ld1rh";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RSB { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RSB_z_p_bi_s16:
      mnemonic = "ld1rsb";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RSB { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RSB_z_p_bi_s32:
      mnemonic = "ld1rsb";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RSB { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RSB_z_p_bi_s64:
      mnemonic = "ld1rsb";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RSH { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RSH_z_p_bi_s32:
      mnemonic = "ld1rsh";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RSH { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RSH_z_p_bi_s64:
      mnemonic = "ld1rsh";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RSW { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RSW_z_p_bi_s64:
      mnemonic = "ld1rsw";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RW { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RW_z_p_bi_u32:
      mnemonic = "ld1rw";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1RW { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RW_z_p_bi_u64:
      mnemonic = "ld1rw";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, #'u2116}]";
      break;
    // LD1SB { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]
    case LD1SB_z_p_ai_s:
      mnemonic = "ld1sb";
      form = "{ 'Zt.s }, p'u1210/z, ['Zn.s{, #'u2016}]";
      break;
    // LD1SB { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]
    case LD1SB_z_p_bz_s_x32_unscaled:
      mnemonic = "ld1sb";
      break;
    // LD1SH { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]
    case LD1SH_z_p_ai_s:
      mnemonic = "ld1sh";
      form = "{ 'Zt.s }, p'u1210/z, ['Zn.s{, #'u2016}]";
      break;
    // LD1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod> #1]
    case LD1SH_z_p_bz_s_x32_scaled:
      mnemonic = "ld1sh";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Zm.s, <mod> #1]";
      break;
    // LD1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]
    case LD1SH_z_p_bz_s_x32_unscaled:
      mnemonic = "ld1sh";
      break;
    // LD1W { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]
    case LD1W_z_p_ai_s:
      mnemonic = "ld1w";
      form = "{ 'Zt.s }, p'u1210/z, ['Zn.s{, #'u2016}]";
      break;
    // LD1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod> #2]
    case LD1W_z_p_bz_s_x32_scaled:
      mnemonic = "ld1w";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Zm.s, <mod> #2]";
      break;
    // LD1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]
    case LD1W_z_p_bz_s_x32_unscaled:
      mnemonic = "ld1w";
      break;
    // LDFF1B { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]
    case LDFF1B_z_p_ai_s:
      mnemonic = "ldff1b";
      form = "{ 'Zt.s }, p'u1210/z, ['Zn.s{, #'u2016}]";
      break;
    // LDFF1B { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]
    case LDFF1B_z_p_bz_s_x32_unscaled:
      mnemonic = "ldff1b";
      break;
    // LDFF1H { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]
    case LDFF1H_z_p_ai_s:
      mnemonic = "ldff1h";
      form = "{ 'Zt.s }, p'u1210/z, ['Zn.s{, #'u2016}]";
      break;
    // LDFF1H { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod> #1]
    case LDFF1H_z_p_bz_s_x32_scaled:
      mnemonic = "ldff1h";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Zm.s, <mod> #1]";
      break;
    // LDFF1H { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]
    case LDFF1H_z_p_bz_s_x32_unscaled:
      mnemonic = "ldff1h";
      break;
    // LDFF1SB { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]
    case LDFF1SB_z_p_ai_s:
      mnemonic = "ldff1sb";
      form = "{ 'Zt.s }, p'u1210/z, ['Zn.s{, #'u2016}]";
      break;
    // LDFF1SB { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]
    case LDFF1SB_z_p_bz_s_x32_unscaled:
      mnemonic = "ldff1sb";
      break;
    // LDFF1SH { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]
    case LDFF1SH_z_p_ai_s:
      mnemonic = "ldff1sh";
      form = "{ 'Zt.s }, p'u1210/z, ['Zn.s{, #'u2016}]";
      break;
    // LDFF1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod> #1]
    case LDFF1SH_z_p_bz_s_x32_scaled:
      mnemonic = "ldff1sh";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Zm.s, <mod> #1]";
      break;
    // LDFF1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]
    case LDFF1SH_z_p_bz_s_x32_unscaled:
      mnemonic = "ldff1sh";
      break;
    // LDFF1W { <Zt>.S }, <Pg>/Z, [<Zn>.S{, #<imm>}]
    case LDFF1W_z_p_ai_s:
      mnemonic = "ldff1w";
      form = "{ 'Zt.s }, p'u1210/z, ['Zn.s{, #'u2016}]";
      break;
    // LDFF1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod> #2]
    case LDFF1W_z_p_bz_s_x32_scaled:
      mnemonic = "ldff1w";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Zm.s, <mod> #2]";
      break;
    // LDFF1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Zm>.S, <mod>]
    case LDFF1W_z_p_bz_s_x32_unscaled:
      mnemonic = "ldff1w";
      break;
    // LDR <Pt>, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDR_p_bi:
      mnemonic = "ldr";
      form = "'Pd, ['Xns{, #<imm>, MUL VL}]";
      break;
    // LDR <Zt>, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDR_z_bi:
      mnemonic = "ldr";
      form = "'Zt, ['Xns{, #<imm>, MUL VL}]";
      break;
    // PRFB <prfop>, <Pg>, [<Zn>.S{, #<imm>}]
    case PRFB_i_p_ai_s:
      mnemonic = "prfb";
      form = "#'u0300, p'u1210, ['Zn.s{, #'u2016}]";
      break;
    // PRFB <prfop>, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case PRFB_i_p_bi_s:
      mnemonic = "prfb";
      form = "#'u0300, p'u1210, ['Xns{, #'u2116, MUL VL}]";
      break;
    // PRFB <prfop>, <Pg>, [<Xn|SP>, <Xm>]
    case PRFB_i_p_br_s:
      mnemonic = "prfb";
      form = "#'u0300, p'u1210, ['Xns, 'Rm]";
      break;
    // PRFB <prfop>, <Pg>, [<Xn|SP>, <Zm>.S, <mod>]
    case PRFB_i_p_bz_s_x32_scaled:
      mnemonic = "prfb";
      form = "#'u0300, p'u1210, ['Xns, 'Zm.s, <mod>]";
      break;
    // PRFD <prfop>, <Pg>, [<Zn>.S{, #<imm>}]
    case PRFD_i_p_ai_s:
      mnemonic = "prfd";
      form = "#'u0300, p'u1210, ['Zn.s{, #'u2016}]";
      break;
    // PRFD <prfop>, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case PRFD_i_p_bi_s:
      mnemonic = "prfd";
      form = "#'u0300, p'u1210, ['Xns{, #'u2116, MUL VL}]";
      break;
    // PRFD <prfop>, <Pg>, [<Xn|SP>, <Xm>, LSL #3]
    case PRFD_i_p_br_s:
      mnemonic = "prfd";
      form = "#'u0300, p'u1210, ['Xns, 'Rm, LSL #3]";
      break;
    // PRFD <prfop>, <Pg>, [<Xn|SP>, <Zm>.S, <mod> #3]
    case PRFD_i_p_bz_s_x32_scaled:
      mnemonic = "prfd";
      form = "#'u0300, p'u1210, ['Xns, 'Zm.s, <mod> #3]";
      break;
    // PRFH <prfop>, <Pg>, [<Zn>.S{, #<imm>}]
    case PRFH_i_p_ai_s:
      mnemonic = "prfh";
      form = "#'u0300, p'u1210, ['Zn.s{, #'u2016}]";
      break;
    // PRFH <prfop>, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case PRFH_i_p_bi_s:
      mnemonic = "prfh";
      form = "#'u0300, p'u1210, ['Xns{, #'u2116, MUL VL}]";
      break;
    // PRFH <prfop>, <Pg>, [<Xn|SP>, <Xm>, LSL #1]
    case PRFH_i_p_br_s:
      mnemonic = "prfh";
      form = "#'u0300, p'u1210, ['Xns, 'Rm, LSL #1]";
      break;
    // PRFH <prfop>, <Pg>, [<Xn|SP>, <Zm>.S, <mod> #1]
    case PRFH_i_p_bz_s_x32_scaled:
      mnemonic = "prfh";
      form = "#'u0300, p'u1210, ['Xns, 'Zm.s, <mod> #1]";
      break;
    // PRFW <prfop>, <Pg>, [<Zn>.S{, #<imm>}]
    case PRFW_i_p_ai_s:
      mnemonic = "prfw";
      form = "#'u0300, p'u1210, ['Zn.s{, #'u2016}]";
      break;
    // PRFW <prfop>, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case PRFW_i_p_bi_s:
      mnemonic = "prfw";
      form = "#'u0300, p'u1210, ['Xns{, #'u2116, MUL VL}]";
      break;
    // PRFW <prfop>, <Pg>, [<Xn|SP>, <Xm>, LSL #2]
    case PRFW_i_p_br_s:
      mnemonic = "prfw";
      form = "#'u0300, p'u1210, ['Xns, 'Rm, LSL #2]";
      break;
    // PRFW <prfop>, <Pg>, [<Xn|SP>, <Zm>.S, <mod> #2]
    case PRFW_i_p_bz_s_x32_scaled:
      mnemonic = "prfw";
      form = "#'u0300, p'u1210, ['Xns, 'Zm.s, <mod> #2]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEMem64BitGather(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  const char *form = "{ 'Zt.d }, p'u1210/z, ['Zn.d{, #'u2016}]";

  switch (instr->Mask(SVEMem64BitGatherMask)) {
    // LD1B { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LD1B_z_p_ai_d:
      mnemonic = "ld1b";
      break;
    // LD1B { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LD1B_z_p_bz_d_64_unscaled:
      mnemonic = "ld1b";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LD1B { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LD1B_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1b";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LD1D { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LD1D_z_p_ai_d:
      mnemonic = "ld1d";
      break;
    // LD1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #3]
    case LD1D_z_p_bz_d_64_scaled:
      mnemonic = "ld1d";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, LSL #3]";
      break;
    // LD1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LD1D_z_p_bz_d_64_unscaled:
      mnemonic = "ld1d";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LD1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #3]
    case LD1D_z_p_bz_d_x32_scaled:
      mnemonic = "ld1d";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod> #3]";
      break;
    // LD1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LD1D_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1d";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LD1H { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LD1H_z_p_ai_d:
      mnemonic = "ld1h";
      break;
    // LD1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #1]
    case LD1H_z_p_bz_d_64_scaled:
      mnemonic = "ld1h";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, LSL #1]";
      break;
    // LD1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LD1H_z_p_bz_d_64_unscaled:
      mnemonic = "ld1h";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LD1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #1]
    case LD1H_z_p_bz_d_x32_scaled:
      mnemonic = "ld1h";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod> #1]";
      break;
    // LD1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LD1H_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1h";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LD1SB { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LD1SB_z_p_ai_d:
      mnemonic = "ld1sb";
      break;
    // LD1SB { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LD1SB_z_p_bz_d_64_unscaled:
      mnemonic = "ld1sb";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LD1SB { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LD1SB_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1sb";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LD1SH { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LD1SH_z_p_ai_d:
      mnemonic = "ld1sh";
      break;
    // LD1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #1]
    case LD1SH_z_p_bz_d_64_scaled:
      mnemonic = "ld1sh";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, LSL #1]";
      break;
    // LD1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LD1SH_z_p_bz_d_64_unscaled:
      mnemonic = "ld1sh";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LD1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #1]
    case LD1SH_z_p_bz_d_x32_scaled:
      mnemonic = "ld1sh";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod> #1]";
      break;
    // LD1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LD1SH_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1sh";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LD1SW { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LD1SW_z_p_ai_d:
      mnemonic = "ld1sw";
      break;
    // LD1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #2]
    case LD1SW_z_p_bz_d_64_scaled:
      mnemonic = "ld1sw";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, LSL #2]";
      break;
    // LD1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LD1SW_z_p_bz_d_64_unscaled:
      mnemonic = "ld1sw";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LD1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #2]
    case LD1SW_z_p_bz_d_x32_scaled:
      mnemonic = "ld1sw";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod> #2]";
      break;
    // LD1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LD1SW_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1sw";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LD1W { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LD1W_z_p_ai_d:
      mnemonic = "ld1w";
      break;
    // LD1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #2]
    case LD1W_z_p_bz_d_64_scaled:
      mnemonic = "ld1w";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, LSL #2]";
      break;
    // LD1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LD1W_z_p_bz_d_64_unscaled:
      mnemonic = "ld1w";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LD1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #2]
    case LD1W_z_p_bz_d_x32_scaled:
      mnemonic = "ld1w";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod> #2]";
      break;
    // LD1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LD1W_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1w";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LDFF1B { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LDFF1B_z_p_ai_d:
      mnemonic = "ldff1b";
      break;
    // LDFF1B { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LDFF1B_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1b";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LDFF1B { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LDFF1B_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1b";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LDFF1D { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LDFF1D_z_p_ai_d:
      mnemonic = "ldff1d";
      break;
    // LDFF1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #3]
    case LDFF1D_z_p_bz_d_64_scaled:
      mnemonic = "ldff1d";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, LSL #3]";
      break;
    // LDFF1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LDFF1D_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1d";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LDFF1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #3]
    case LDFF1D_z_p_bz_d_x32_scaled:
      mnemonic = "ldff1d";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod> #3]";
      break;
    // LDFF1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LDFF1D_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1d";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LDFF1H { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LDFF1H_z_p_ai_d:
      mnemonic = "ldff1h";
      break;
    // LDFF1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #1]
    case LDFF1H_z_p_bz_d_64_scaled:
      mnemonic = "ldff1h";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, LSL #1]";
      break;
    // LDFF1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LDFF1H_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1h";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LDFF1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #1]
    case LDFF1H_z_p_bz_d_x32_scaled:
      mnemonic = "ldff1h";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod> #1]";
      break;
    // LDFF1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LDFF1H_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1h";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LDFF1SB { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LDFF1SB_z_p_ai_d:
      mnemonic = "ldff1sb";
      break;
    // LDFF1SB { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LDFF1SB_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1sb";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LDFF1SB { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LDFF1SB_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1sb";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LDFF1SH { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LDFF1SH_z_p_ai_d:
      mnemonic = "ldff1sh";
      break;
    // LDFF1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #1]
    case LDFF1SH_z_p_bz_d_64_scaled:
      mnemonic = "ldff1sh";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, LSL #1]";
      break;
    // LDFF1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LDFF1SH_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1sh";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LDFF1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #1]
    case LDFF1SH_z_p_bz_d_x32_scaled:
      mnemonic = "ldff1sh";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod> #1]";
      break;
    // LDFF1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LDFF1SH_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1sh";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LDFF1SW { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LDFF1SW_z_p_ai_d:
      mnemonic = "ldff1sw";
      break;
    // LDFF1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #2]
    case LDFF1SW_z_p_bz_d_64_scaled:
      mnemonic = "ldff1sw";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, LSL #2]";
      break;
    // LDFF1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LDFF1SW_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1sw";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LDFF1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #2]
    case LDFF1SW_z_p_bz_d_x32_scaled:
      mnemonic = "ldff1sw";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod> #2]";
      break;
    // LDFF1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LDFF1SW_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1sw";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // LDFF1W { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
    case LDFF1W_z_p_ai_d:
      mnemonic = "ldff1w";
      break;
    // LDFF1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #2]
    case LDFF1W_z_p_bz_d_64_scaled:
      mnemonic = "ldff1w";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, LSL #2]";
      break;
    // LDFF1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
    case LDFF1W_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1w";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d]";
      break;
    // LDFF1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod> #2]
    case LDFF1W_z_p_bz_d_x32_scaled:
      mnemonic = "ldff1w";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod> #2]";
      break;
    // LDFF1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, <mod>]
    case LDFF1W_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1w";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Zm.d, <mod>]";
      break;
    // PRFB <prfop>, <Pg>, [<Zn>.D{, #<imm>}]
    case PRFB_i_p_ai_d:
      mnemonic = "prfb";
      form = "#'u0300, p'u1210, ['Zn.d{, #'u2016}]";
      break;
    // PRFB <prfop>, <Pg>, [<Xn|SP>, <Zm>.D]
    case PRFB_i_p_bz_d_64_scaled:
      mnemonic = "prfb";
      form = "#'u0300, p'u1210, ['Xns, 'Zm.d]";
      break;
    // PRFB <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, <mod>]
    case PRFB_i_p_bz_d_x32_scaled:
      mnemonic = "prfb";
      form = "#'u0300, p'u1210, ['Xns, 'Zm.d, <mod>]";
      break;
    // PRFD <prfop>, <Pg>, [<Zn>.D{, #<imm>}]
    case PRFD_i_p_ai_d:
      mnemonic = "prfd";
      form = "#'u0300, p'u1210, ['Zn.d{, #'u2016}]";
      break;
    // PRFD <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, LSL #3]
    case PRFD_i_p_bz_d_64_scaled:
      mnemonic = "prfd";
      form = "#'u0300, p'u1210, ['Xns, 'Zm.d, LSL #3]";
      break;
    // PRFD <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, <mod> #3]
    case PRFD_i_p_bz_d_x32_scaled:
      mnemonic = "prfd";
      form = "#'u0300, p'u1210, ['Xns, 'Zm.d, <mod> #3]";
      break;
    // PRFH <prfop>, <Pg>, [<Zn>.D{, #<imm>}]
    case PRFH_i_p_ai_d:
      mnemonic = "prfh";
      form = "#'u0300, p'u1210, ['Zn.d{, #'u2016}]";
      break;
    // PRFH <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, LSL #1]
    case PRFH_i_p_bz_d_64_scaled:
      mnemonic = "prfh";
      form = "#'u0300, p'u1210, ['Xns, 'Zm.d, LSL #1]";
      break;
    // PRFH <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, <mod> #1]
    case PRFH_i_p_bz_d_x32_scaled:
      mnemonic = "prfh";
      form = "#'u0300, p'u1210, ['Xns, 'Zm.d, <mod> #1]";
      break;
    // PRFW <prfop>, <Pg>, [<Zn>.D{, #<imm>}]
    case PRFW_i_p_ai_d:
      mnemonic = "prfw";
      form = "#'u0300, p'u1210, ['Zn.d{, #'u2016}]";
      break;
    // PRFW <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, LSL #2]
    case PRFW_i_p_bz_d_64_scaled:
      mnemonic = "prfw";
      form = "#'u0300, p'u1210, ['Xns, 'Zm.d, LSL #2]";
      break;
    // PRFW <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, <mod> #2]
    case PRFW_i_p_bz_d_x32_scaled:
      mnemonic = "prfw";
      form = "#'u0300, p'u1210, ['Xns, 'Zm.d, <mod> #2]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEMemContiguousLoad(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  const char *form = "{ 'Zt.d }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";

  switch (instr->Mask(SVEMemContiguousLoadMask)) {
    // LD1B { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1B_z_p_bi_u16:
      mnemonic = "ld1b";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD1B { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1B_z_p_bi_u32:
      mnemonic = "ld1b";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD1B { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1B_z_p_bi_u64:
      mnemonic = "ld1b";
      break;
    // LD1B { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1B_z_p_bi_u8:
      mnemonic = "ld1b";
      form = "{ 'Zt.b }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD1B { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>]
    case LD1B_z_p_br_u16:
      mnemonic = "ld1b";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns, 'Rm]";
      break;
    // LD1B { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>]
    case LD1B_z_p_br_u32:
      mnemonic = "ld1b";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Rm]";
      break;
    // LD1B { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>]
    case LD1B_z_p_br_u64:
      mnemonic = "ld1b";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Rm]";
      break;
    // LD1B { <Zt>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
    case LD1B_z_p_br_u8:
      mnemonic = "ld1b";
      form = "{ 'Zt.b }, p'u1210/z, ['Xns, 'Rm]";
      break;
    // LD1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1D_z_p_bi_u64:
      mnemonic = "ld1d";
      break;
    // LD1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]
    case LD1D_z_p_br_u64:
      mnemonic = "ld1d";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Rm, LSL #3]";
      break;
    // LD1H { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1H_z_p_bi_u16:
      mnemonic = "ld1h";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD1H { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1H_z_p_bi_u32:
      mnemonic = "ld1h";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1H_z_p_bi_u64:
      mnemonic = "ld1h";
      break;
    // LD1H { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
    case LD1H_z_p_br_u16:
      mnemonic = "ld1h";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns, 'Rm, LSL #1]";
      break;
    // LD1H { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
    case LD1H_z_p_br_u32:
      mnemonic = "ld1h";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Rm, LSL #1]";
      break;
    // LD1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
    case LD1H_z_p_br_u64:
      mnemonic = "ld1h";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Rm, LSL #1]";
      break;
    // LD1RQB { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RQB_z_p_bi_u8:
      mnemonic = "ld1rqb";
      form = "{ 'Zt.b }, p'u1210/z, ['Xns{, #'u1916}]";
      break;
    // LD1RQB { <Zt>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
    case LD1RQB_z_p_br_contiguous:
      mnemonic = "ld1rqb";
      form = "{ 'Zt.b }, p'u1210/z, ['Xns, 'Rm]";
      break;
    // LD1RQD { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RQD_z_p_bi_u64:
      mnemonic = "ld1rqd";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, #'u1916}]";
      break;
    // LD1RQD { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]
    case LD1RQD_z_p_br_contiguous:
      mnemonic = "ld1rqd";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Rm, LSL #3]";
      break;
    // LD1RQH { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RQH_z_p_bi_u16:
      mnemonic = "ld1rqh";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, #'u1916}]";
      break;
    // LD1RQH { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
    case LD1RQH_z_p_br_contiguous:
      mnemonic = "ld1rqh";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns, 'Rm, LSL #1]";
      break;
    // LD1RQW { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
    case LD1RQW_z_p_bi_u32:
      mnemonic = "ld1rqw";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u1916}]";
      break;
    // LD1RQW { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
    case LD1RQW_z_p_br_contiguous:
      mnemonic = "ld1rqw";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Rm, LSL #2]";
      break;
    // LD1SB { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1SB_z_p_bi_s16:
      mnemonic = "ld1sb";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD1SB { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1SB_z_p_bi_s32:
      mnemonic = "ld1sb";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD1SB { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1SB_z_p_bi_s64:
      mnemonic = "ld1sb";
      break;
    // LD1SB { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>]
    case LD1SB_z_p_br_s16:
      mnemonic = "ld1sb";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns, 'Rm]";
      break;
    // LD1SB { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>]
    case LD1SB_z_p_br_s32:
      mnemonic = "ld1sb";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Rm]";
      break;
    // LD1SB { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>]
    case LD1SB_z_p_br_s64:
      mnemonic = "ld1sb";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Rm]";
      break;
    // LD1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1SH_z_p_bi_s32:
      mnemonic = "ld1sh";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1SH_z_p_bi_s64:
      mnemonic = "ld1sh";
      break;
    // LD1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
    case LD1SH_z_p_br_s32:
      mnemonic = "ld1sh";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Rm, LSL #1]";
      break;
    // LD1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
    case LD1SH_z_p_br_s64:
      mnemonic = "ld1sh";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Rm, LSL #1]";
      break;
    // LD1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1SW_z_p_bi_s64:
      mnemonic = "ld1sw";
      break;
    // LD1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
    case LD1SW_z_p_br_s64:
      mnemonic = "ld1sw";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Rm, LSL #2]";
      break;
    // LD1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1W_z_p_bi_u32:
      mnemonic = "ld1w";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD1W_z_p_bi_u64:
      mnemonic = "ld1w";
      break;
    // LD1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
    case LD1W_z_p_br_u32:
      mnemonic = "ld1w";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Rm, LSL #2]";
      break;
    // LD1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
    case LD1W_z_p_br_u64:
      mnemonic = "ld1w";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Rm, LSL #2]";
      break;
    // LD2B { <Zt1>.B, <Zt2>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD2B_z_p_bi_contiguous:
      mnemonic = "ld2b";
      form = "{ 'Zt.b, 'Zt2.b }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD2B { <Zt1>.B, <Zt2>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
    case LD2B_z_p_br_contiguous:
      mnemonic = "ld2b";
      form = "{ 'Zt.b, 'Zt2.b }, p'u1210/z, ['Xns, 'Rm]";
      break;
    // LD2D { <Zt1>.D, <Zt2>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD2D_z_p_bi_contiguous:
      mnemonic = "ld2d";
      form = "{ 'Zt.d, 'Zt2.d }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD2D { <Zt1>.D, <Zt2>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]
    case LD2D_z_p_br_contiguous:
      mnemonic = "ld2d";
      form = "{ 'Zt.d, 'Zt2.d }, p'u1210/z, ['Xns, 'Rm, LSL #3]";
      break;
    // LD2H { <Zt1>.H, <Zt2>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD2H_z_p_bi_contiguous:
      mnemonic = "ld2h";
      form = "{ 'Zt.h, 'Zt2.h }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD2H { <Zt1>.H, <Zt2>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
    case LD2H_z_p_br_contiguous:
      mnemonic = "ld2h";
      form = "{ 'Zt.h, 'Zt2.h }, p'u1210/z, ['Xns, 'Rm, LSL #1]";
      break;
    // LD2W { <Zt1>.S, <Zt2>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD2W_z_p_bi_contiguous:
      mnemonic = "ld2w";
      form = "{ 'Zt.s, 'Zt2.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD2W { <Zt1>.S, <Zt2>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
    case LD2W_z_p_br_contiguous:
      mnemonic = "ld2w";
      form = "{ 'Zt.s, 'Zt2.s }, p'u1210/z, ['Xns, 'Rm, LSL #2]";
      break;
    // LD3B { <Zt1>.B, <Zt2>.B, <Zt3>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD3B_z_p_bi_contiguous:
      mnemonic = "ld3b";
      form = "{ 'Zt.b, 'Zt2.b, 'Zt3.b }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD3B { <Zt1>.B, <Zt2>.B, <Zt3>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
    case LD3B_z_p_br_contiguous:
      mnemonic = "ld3b";
      form = "{ 'Zt.b, 'Zt2.b, 'Zt3.b }, p'u1210/z, ['Xns, 'Rm]";
      break;
    // LD3D { <Zt1>.D, <Zt2>.D, <Zt3>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD3D_z_p_bi_contiguous:
      mnemonic = "ld3d";
      form = "{ 'Zt.d, 'Zt2.d, 'Zt3.d }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD3D { <Zt1>.D, <Zt2>.D, <Zt3>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]
    case LD3D_z_p_br_contiguous:
      mnemonic = "ld3d";
      form = "{ 'Zt.d, 'Zt2.d, 'Zt3.d }, p'u1210/z, ['Xns, 'Rm, LSL #3]";
      break;
    // LD3H { <Zt1>.H, <Zt2>.H, <Zt3>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD3H_z_p_bi_contiguous:
      mnemonic = "ld3h";
      form = "{ 'Zt.h, 'Zt2.h, 'Zt3.h }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD3H { <Zt1>.H, <Zt2>.H, <Zt3>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
    case LD3H_z_p_br_contiguous:
      mnemonic = "ld3h";
      form = "{ 'Zt.h, 'Zt2.h, 'Zt3.h }, p'u1210/z, ['Xns, 'Rm, LSL #1]";
      break;
    // LD3W { <Zt1>.S, <Zt2>.S, <Zt3>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LD3W_z_p_bi_contiguous:
      mnemonic = "ld3w";
      form = "{ 'Zt.s, 'Zt2.s, 'Zt3.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LD3W { <Zt1>.S, <Zt2>.S, <Zt3>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
    case LD3W_z_p_br_contiguous:
      mnemonic = "ld3w";
      form = "{ 'Zt.s, 'Zt2.s, 'Zt3.s }, p'u1210/z, ['Xns, 'Rm, LSL #2]";
      break;
    // LD4B { <Zt1>.B, <Zt2>.B, <Zt3>.B, <Zt4>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>,
    // MUL VL}]
    case LD4B_z_p_bi_contiguous:
      mnemonic = "ld4b";
      form =
          "{ 'Zt.b, 'Zt2.b, 'Zt3.b, 'Zt4.b }, p'u1210/z, ['Xns{, #'u1916, MUL "
          "VL}]";
      break;
    // LD4B { <Zt1>.B, <Zt2>.B, <Zt3>.B, <Zt4>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
    case LD4B_z_p_br_contiguous:
      mnemonic = "ld4b";
      form = "{ 'Zt.b, 'Zt2.b, 'Zt3.b, 'Zt4.b }, p'u1210/z, ['Xns, 'Rm]";
      break;
    // LD4D { <Zt1>.D, <Zt2>.D, <Zt3>.D, <Zt4>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>,
    // MUL VL}]
    case LD4D_z_p_bi_contiguous:
      mnemonic = "ld4d";
      form =
          "{ 'Zt.d, 'Zt2.d, 'Zt3.d, 'Zt4.d }, p'u1210/z, ['Xns{, #'u1916, MUL "
          "VL}]";
      break;
    // LD4D { <Zt1>.D, <Zt2>.D, <Zt3>.D, <Zt4>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL
    // #3]
    case LD4D_z_p_br_contiguous:
      mnemonic = "ld4d";
      form =
          "{ 'Zt.d, 'Zt2.d, 'Zt3.d, 'Zt4.d }, p'u1210/z, ['Xns, 'Rm, LSL #3]";
      break;
    // LD4H { <Zt1>.H, <Zt2>.H, <Zt3>.H, <Zt4>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>,
    // MUL VL}]
    case LD4H_z_p_bi_contiguous:
      mnemonic = "ld4h";
      form =
          "{ 'Zt.h, 'Zt2.h, 'Zt3.h, 'Zt4.h }, p'u1210/z, ['Xns{, #'u1916, MUL "
          "VL}]";
      break;
    // LD4H { <Zt1>.H, <Zt2>.H, <Zt3>.H, <Zt4>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL
    // #1]
    case LD4H_z_p_br_contiguous:
      mnemonic = "ld4h";
      form =
          "{ 'Zt.h, 'Zt2.h, 'Zt3.h, 'Zt4.h }, p'u1210/z, ['Xns, 'Rm, LSL #1]";
      break;
    // LD4W { <Zt1>.S, <Zt2>.S, <Zt3>.S, <Zt4>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>,
    // MUL VL}]
    case LD4W_z_p_bi_contiguous:
      mnemonic = "ld4w";
      form =
          "{ 'Zt.s, 'Zt2.s, 'Zt3.s, 'Zt4.s }, p'u1210/z, ['Xns{, #'u1916, MUL "
          "VL}]";
      break;
    // LD4W { <Zt1>.S, <Zt2>.S, <Zt3>.S, <Zt4>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL
    // #2]
    case LD4W_z_p_br_contiguous:
      mnemonic = "ld4w";
      form =
          "{ 'Zt.s, 'Zt2.s, 'Zt3.s, 'Zt4.s }, p'u1210/z, ['Xns, 'Rm, LSL #2]";
      break;
    // LDFF1B { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
    case LDFF1B_z_p_br_u16:
      mnemonic = "ldff1b";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, 'Rm}]";
      break;
    // LDFF1B { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
    case LDFF1B_z_p_br_u32:
      mnemonic = "ldff1b";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, 'Rm}]";
      break;
    // LDFF1B { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
    case LDFF1B_z_p_br_u64:
      mnemonic = "ldff1b";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, 'Rm}]";
      break;
    // LDFF1B { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
    case LDFF1B_z_p_br_u8:
      mnemonic = "ldff1b";
      form = "{ 'Zt.b }, p'u1210/z, ['Xns{, 'Rm}]";
      break;
    // LDFF1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #3}]
    case LDFF1D_z_p_br_u64:
      mnemonic = "ldff1d";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, 'Rm, LSL #3}]";
      break;
    // LDFF1H { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]
    case LDFF1H_z_p_br_u16:
      mnemonic = "ldff1h";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, 'Rm, LSL #1}]";
      break;
    // LDFF1H { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]
    case LDFF1H_z_p_br_u32:
      mnemonic = "ldff1h";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, 'Rm, LSL #1}]";
      break;
    // LDFF1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]
    case LDFF1H_z_p_br_u64:
      mnemonic = "ldff1h";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, 'Rm, LSL #1}]";
      break;
    // LDFF1SB { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
    case LDFF1SB_z_p_br_s16:
      mnemonic = "ldff1sb";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, 'Rm}]";
      break;
    // LDFF1SB { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
    case LDFF1SB_z_p_br_s32:
      mnemonic = "ldff1sb";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, 'Rm}]";
      break;
    // LDFF1SB { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
    case LDFF1SB_z_p_br_s64:
      mnemonic = "ldff1sb";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, 'Rm}]";
      break;
    // LDFF1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]
    case LDFF1SH_z_p_br_s32:
      mnemonic = "ldff1sh";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, 'Rm, LSL #1}]";
      break;
    // LDFF1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]
    case LDFF1SH_z_p_br_s64:
      mnemonic = "ldff1sh";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, 'Rm, LSL #1}]";
      break;
    // LDFF1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #2}]
    case LDFF1SW_z_p_br_s64:
      mnemonic = "ldff1sw";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, 'Rm, LSL #2}]";
      break;
    // LDFF1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #2}]
    case LDFF1W_z_p_br_u32:
      mnemonic = "ldff1w";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, 'Rm, LSL #2}]";
      break;
    // LDFF1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #2}]
    case LDFF1W_z_p_br_u64:
      mnemonic = "ldff1w";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns{, 'Rm, LSL #2}]";
      break;
    // LDNF1B { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1B_z_p_bi_u16:
      mnemonic = "ldnf1b";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LDNF1B { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1B_z_p_bi_u32:
      mnemonic = "ldnf1b";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LDNF1B { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1B_z_p_bi_u64:
      mnemonic = "ldnf1b";
      break;
    // LDNF1B { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1B_z_p_bi_u8:
      mnemonic = "ldnf1b";
      form = "{ 'Zt.b }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LDNF1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1D_z_p_bi_u64:
      mnemonic = "ldnf1d";
      break;
    // LDNF1H { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1H_z_p_bi_u16:
      mnemonic = "ldnf1h";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LDNF1H { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1H_z_p_bi_u32:
      mnemonic = "ldnf1h";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LDNF1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1H_z_p_bi_u64:
      mnemonic = "ldnf1h";
      break;
    // LDNF1SB { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1SB_z_p_bi_s16:
      mnemonic = "ldnf1sb";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LDNF1SB { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1SB_z_p_bi_s32:
      mnemonic = "ldnf1sb";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LDNF1SB { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1SB_z_p_bi_s64:
      mnemonic = "ldnf1sb";
      break;
    // LDNF1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1SH_z_p_bi_s32:
      mnemonic = "ldnf1sh";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LDNF1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1SH_z_p_bi_s64:
      mnemonic = "ldnf1sh";
      break;
    // LDNF1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1SW_z_p_bi_s64:
      mnemonic = "ldnf1sw";
      break;
    // LDNF1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1W_z_p_bi_u32:
      mnemonic = "ldnf1w";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LDNF1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNF1W_z_p_bi_u64:
      mnemonic = "ldnf1w";
      break;
    // LDNT1B { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNT1B_z_p_bi_contiguous:
      mnemonic = "ldnt1b";
      form = "{ 'Zt.b }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LDNT1B { <Zt>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
    case LDNT1B_z_p_br_contiguous:
      mnemonic = "ldnt1b";
      form = "{ 'Zt.b }, p'u1210/z, ['Xns, 'Rm]";
      break;
    // LDNT1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNT1D_z_p_bi_contiguous:
      mnemonic = "ldnt1d";
      break;
    // LDNT1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]
    case LDNT1D_z_p_br_contiguous:
      mnemonic = "ldnt1d";
      form = "{ 'Zt.d }, p'u1210/z, ['Xns, 'Rm, LSL #3]";
      break;
    // LDNT1H { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNT1H_z_p_bi_contiguous:
      mnemonic = "ldnt1h";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LDNT1H { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
    case LDNT1H_z_p_br_contiguous:
      mnemonic = "ldnt1h";
      form = "{ 'Zt.h }, p'u1210/z, ['Xns, 'Rm, LSL #1]";
      break;
    // LDNT1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
    case LDNT1W_z_p_bi_contiguous:
      mnemonic = "ldnt1w";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns{, #'u1916, MUL VL}]";
      break;
    // LDNT1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
    case LDNT1W_z_p_br_contiguous:
      mnemonic = "ldnt1w";
      form = "{ 'Zt.s }, p'u1210/z, ['Xns, 'Rm, LSL #2]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEMemStore(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]
  const char *form = "{ 'Zt.d }, p'u1210, ['Zn.d{, #'u2016}]";

  switch (instr->Mask(SVEMemStoreMask)) {
    // ST1B { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]
    case ST1B_z_p_ai_d:
      mnemonic = "st1b";
      break;
    // ST1B { <Zt>.S }, <Pg>, [<Zn>.S{, #<imm>}]
    case ST1B_z_p_ai_s:
      mnemonic = "st1b";
      form = "{ 'Zt.s }, p'u1210, ['Zn.s{, #'u2016}]";
      break;
    // ST1B { <Zt>.<T> }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case ST1B_z_p_bi:
      mnemonic = "st1b";
      form = "{ 'Zt.<T> }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // ST1B { <Zt>.<T> }, <Pg>, [<Xn|SP>, <Xm>]
    case ST1B_z_p_br:
      mnemonic = "st1b";
      form = "{ 'Zt.<T> }, p'u1210, ['Xns, 'Rm]";
      break;
    // ST1B { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D]
    case ST1B_z_p_bz_d_64_unscaled:
      mnemonic = "st1b";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d]";
      break;
    // ST1B { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod>]
    case ST1B_z_p_bz_d_x32_unscaled:
      mnemonic = "st1b";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d, <mod>]";
      break;
    // ST1B { <Zt>.S }, <Pg>, [<Xn|SP>, <Zm>.S, <mod>]
    case ST1B_z_p_bz_s_x32_unscaled:
      mnemonic = "st1b";
      form = "{ 'Zt.s }, p'u1210, ['Xns, 'Zm.s, <mod>]";
      break;
    // ST1D { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]
    case ST1D_z_p_ai_d:
      mnemonic = "st1d";
      break;
    // ST1D { <Zt>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case ST1D_z_p_bi:
      mnemonic = "st1d";
      form = "{ 'Zt.d }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // ST1D { <Zt>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]
    // TODO: fix encoding alias issue with enum above.
    //    case ST1D_z_p_br:
    //      mnemonic = "st1d";
    //      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Rm, LSL #3]";
    //      break;
    // ST1D { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, LSL #3]
    case ST1D_z_p_bz_d_64_scaled:
      mnemonic = "st1d";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d, LSL #3]";
      break;
    // ST1D { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D]
    case ST1D_z_p_bz_d_64_unscaled:
      mnemonic = "st1d";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d]";
      break;
    // ST1D { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod> #3]
    case ST1D_z_p_bz_d_x32_scaled:
      mnemonic = "st1d";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d, <mod> #3]";
      break;
    // ST1D { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod>]
    case ST1D_z_p_bz_d_x32_unscaled:
      mnemonic = "st1d";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d, <mod>]";
      break;
    // ST1H { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]
    case ST1H_z_p_ai_d:
      mnemonic = "st1h";
      break;
    // ST1H { <Zt>.S }, <Pg>, [<Zn>.S{, #<imm>}]
    case ST1H_z_p_ai_s:
      mnemonic = "st1h";
      form = "{ 'Zt.s }, p'u1210, ['Zn.s{, #'u2016}]";
      break;
    // ST1H { <Zt>.<T> }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case ST1H_z_p_bi:
      mnemonic = "st1h";
      form = "{ 'Zt.<T> }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // ST1H { <Zt>.<T> }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]
    case ST1H_z_p_br:
      mnemonic = "st1h";
      form = "{ 'Zt.<T> }, p'u1210, ['Xns, 'Rm, LSL #1]";
      break;
    // ST1H { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, LSL #1]
    case ST1H_z_p_bz_d_64_scaled:
      mnemonic = "st1h";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d, LSL #1]";
      break;
    // ST1H { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D]
    case ST1H_z_p_bz_d_64_unscaled:
      mnemonic = "st1h";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d]";
      break;
    // ST1H { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod> #1]
    case ST1H_z_p_bz_d_x32_scaled:
      mnemonic = "st1h";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d, <mod> #1]";
      break;
    // ST1H { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod>]
    case ST1H_z_p_bz_d_x32_unscaled:
      mnemonic = "st1h";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d, <mod>]";
      break;
    // ST1H { <Zt>.S }, <Pg>, [<Xn|SP>, <Zm>.S, <mod> #1]
    case ST1H_z_p_bz_s_x32_scaled:
      mnemonic = "st1h";
      form = "{ 'Zt.s }, p'u1210, ['Xns, 'Zm.s, <mod> #1]";
      break;
    // ST1H { <Zt>.S }, <Pg>, [<Xn|SP>, <Zm>.S, <mod>]
    case ST1H_z_p_bz_s_x32_unscaled:
      mnemonic = "st1h";
      form = "{ 'Zt.s }, p'u1210, ['Xns, 'Zm.s, <mod>]";
      break;
    // ST1W { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]
    case ST1W_z_p_ai_d:
      mnemonic = "st1w";
      break;
    // ST1W { <Zt>.S }, <Pg>, [<Zn>.S{, #<imm>}]
    case ST1W_z_p_ai_s:
      mnemonic = "st1w";
      form = "{ 'Zt.s }, p'u1210, ['Zn.s{, #'u2016}]";
      break;
    // ST1W { <Zt>.<T> }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case ST1W_z_p_bi:
      mnemonic = "st1w";
      form = "{ 'Zt.<T> }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // ST1W { <Zt>.<T> }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]
    case ST1W_z_p_br:
      mnemonic = "st1w";
      form = "{ 'Zt.<T> }, p'u1210, ['Xns, 'Rm, LSL #2]";
      break;
    // ST1W { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, LSL #2]
    case ST1W_z_p_bz_d_64_scaled:
      mnemonic = "st1w";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d, LSL #2]";
      break;
    // ST1W { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D]
    case ST1W_z_p_bz_d_64_unscaled:
      mnemonic = "st1w";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d]";
      break;
    // ST1W { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod> #2]
    case ST1W_z_p_bz_d_x32_scaled:
      mnemonic = "st1w";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d, <mod> #2]";
      break;
    // ST1W { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, <mod>]
    case ST1W_z_p_bz_d_x32_unscaled:
      mnemonic = "st1w";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Zm.d, <mod>]";
      break;
    // ST1W { <Zt>.S }, <Pg>, [<Xn|SP>, <Zm>.S, <mod> #2]
    case ST1W_z_p_bz_s_x32_scaled:
      mnemonic = "st1w";
      form = "{ 'Zt.s }, p'u1210, ['Xns, 'Zm.s, <mod> #2]";
      break;
    // ST1W { <Zt>.S }, <Pg>, [<Xn|SP>, <Zm>.S, <mod>]
    case ST1W_z_p_bz_s_x32_unscaled:
      mnemonic = "st1w";
      form = "{ 'Zt.s }, p'u1210, ['Xns, 'Zm.s, <mod>]";
      break;
    // ST2B { <Zt1>.B, <Zt2>.B }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case ST2B_z_p_bi_contiguous:
      mnemonic = "st2b";
      form = "{ 'Zt.b, 'Zt2.b }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // ST2B { <Zt1>.B, <Zt2>.B }, <Pg>, [<Xn|SP>, <Xm>]
    case ST2B_z_p_br_contiguous:
      mnemonic = "st2b";
      form = "{ 'Zt.b, 'Zt2.b }, p'u1210, ['Xns, 'Rm]";
      break;
    // ST2D { <Zt1>.D, <Zt2>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case ST2D_z_p_bi_contiguous:
      mnemonic = "st2d";
      form = "{ 'Zt.d, 'Zt2.d }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // ST2D { <Zt1>.D, <Zt2>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]
    case ST2D_z_p_br_contiguous:
      mnemonic = "st2d";
      form = "{ 'Zt.d, 'Zt2.d }, p'u1210, ['Xns, 'Rm, LSL #3]";
      break;
    // ST2H { <Zt1>.H, <Zt2>.H }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case ST2H_z_p_bi_contiguous:
      mnemonic = "st2h";
      form = "{ 'Zt.h, 'Zt2.h }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // ST2H { <Zt1>.H, <Zt2>.H }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]
    case ST2H_z_p_br_contiguous:
      mnemonic = "st2h";
      form = "{ 'Zt.h, 'Zt2.h }, p'u1210, ['Xns, 'Rm, LSL #1]";
      break;
    // ST2W { <Zt1>.S, <Zt2>.S }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case ST2W_z_p_bi_contiguous:
      mnemonic = "st2w";
      form = "{ 'Zt.s, 'Zt2.s }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // ST2W { <Zt1>.S, <Zt2>.S }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]
    case ST2W_z_p_br_contiguous:
      mnemonic = "st2w";
      form = "{ 'Zt.s, 'Zt2.s }, p'u1210, ['Xns, 'Rm, LSL #2]";
      break;
    // ST3B { <Zt1>.B, <Zt2>.B, <Zt3>.B }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case ST3B_z_p_bi_contiguous:
      mnemonic = "st3b";
      form = "{ 'Zt.b, 'Zt2.b, 'Zt3.b }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // ST3B { <Zt1>.B, <Zt2>.B, <Zt3>.B }, <Pg>, [<Xn|SP>, <Xm>]
    case ST3B_z_p_br_contiguous:
      mnemonic = "st3b";
      form = "{ 'Zt.b, 'Zt2.b, 'Zt3.b }, p'u1210, ['Xns, 'Rm]";
      break;
    // ST3D { <Zt1>.D, <Zt2>.D, <Zt3>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case ST3D_z_p_bi_contiguous:
      mnemonic = "st3d";
      form = "{ 'Zt.d, 'Zt2.d, 'Zt3.d }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // ST3D { <Zt1>.D, <Zt2>.D, <Zt3>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]
    case ST3D_z_p_br_contiguous:
      mnemonic = "st3d";
      form = "{ 'Zt.d, 'Zt2.d, 'Zt3.d }, p'u1210, ['Xns, 'Rm, LSL #3]";
      break;
    // ST3H { <Zt1>.H, <Zt2>.H, <Zt3>.H }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case ST3H_z_p_bi_contiguous:
      mnemonic = "st3h";
      form = "{ 'Zt.h, 'Zt2.h, 'Zt3.h }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // ST3H { <Zt1>.H, <Zt2>.H, <Zt3>.H }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]
    case ST3H_z_p_br_contiguous:
      mnemonic = "st3h";
      form = "{ 'Zt.h, 'Zt2.h, 'Zt3.h }, p'u1210, ['Xns, 'Rm, LSL #1]";
      break;
    // ST3W { <Zt1>.S, <Zt2>.S, <Zt3>.S }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case ST3W_z_p_bi_contiguous:
      mnemonic = "st3w";
      form = "{ 'Zt.s, 'Zt2.s, 'Zt3.s }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // ST3W { <Zt1>.S, <Zt2>.S, <Zt3>.S }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]
    case ST3W_z_p_br_contiguous:
      mnemonic = "st3w";
      form = "{ 'Zt.s, 'Zt2.s, 'Zt3.s }, p'u1210, ['Xns, 'Rm, LSL #2]";
      break;
    // ST4B { <Zt1>.B, <Zt2>.B, <Zt3>.B, <Zt4>.B }, <Pg>, [<Xn|SP>{, #<imm>, MUL
    // VL}]
    case ST4B_z_p_bi_contiguous:
      mnemonic = "st4b";
      form =
          "{ 'Zt.b, 'Zt2.b, 'Zt3.b, 'Zt4.b }, p'u1210, ['Xns{, #'u1916, MUL "
          "VL}]";
      break;
    // ST4B { <Zt1>.B, <Zt2>.B, <Zt3>.B, <Zt4>.B }, <Pg>, [<Xn|SP>, <Xm>]
    case ST4B_z_p_br_contiguous:
      mnemonic = "st4b";
      form = "{ 'Zt.b, 'Zt2.b, 'Zt3.b, 'Zt4.b }, p'u1210, ['Xns, 'Rm]";
      break;
    // ST4D { <Zt1>.D, <Zt2>.D, <Zt3>.D, <Zt4>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL
    // VL}]
    case ST4D_z_p_bi_contiguous:
      mnemonic = "st4d";
      form =
          "{ 'Zt.d, 'Zt2.d, 'Zt3.d, 'Zt4.d }, p'u1210, ['Xns{, #'u1916, MUL "
          "VL}]";
      break;
    // ST4D { <Zt1>.D, <Zt2>.D, <Zt3>.D, <Zt4>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL
    // #3]
    case ST4D_z_p_br_contiguous:
      mnemonic = "st4d";
      form = "{ 'Zt.d, 'Zt2.d, 'Zt3.d, 'Zt4.d }, p'u1210, ['Xns, 'Rm, LSL #3]";
      break;
    // ST4H { <Zt1>.H, <Zt2>.H, <Zt3>.H, <Zt4>.H }, <Pg>, [<Xn|SP>{, #<imm>, MUL
    // VL}]
    case ST4H_z_p_bi_contiguous:
      mnemonic = "st4h";
      form =
          "{ 'Zt.h, 'Zt2.h, 'Zt3.h, 'Zt4.h }, p'u1210, ['Xns{, #'u1916, MUL "
          "VL}]";
      break;
    // ST4H { <Zt1>.H, <Zt2>.H, <Zt3>.H, <Zt4>.H }, <Pg>, [<Xn|SP>, <Xm>, LSL
    // #1]
    case ST4H_z_p_br_contiguous:
      mnemonic = "st4h";
      form = "{ 'Zt.h, 'Zt2.h, 'Zt3.h, 'Zt4.h }, p'u1210, ['Xns, 'Rm, LSL #1]";
      break;
    // ST4W { <Zt1>.S, <Zt2>.S, <Zt3>.S, <Zt4>.S }, <Pg>, [<Xn|SP>{, #<imm>, MUL
    // VL}]
    case ST4W_z_p_bi_contiguous:
      mnemonic = "st4w";
      form =
          "{ 'Zt.s, 'Zt2.s, 'Zt3.s, 'Zt4.s }, p'u1210, ['Xns{, #'u1916, MUL "
          "VL}]";
      break;
    // ST4W { <Zt1>.S, <Zt2>.S, <Zt3>.S, <Zt4>.S }, <Pg>, [<Xn|SP>, <Xm>, LSL
    // #2]
    case ST4W_z_p_br_contiguous:
      mnemonic = "st4w";
      form = "{ 'Zt.s, 'Zt2.s, 'Zt3.s, 'Zt4.s }, p'u1210, ['Xns, 'Rm, LSL #2]";
      break;
    // STNT1B { <Zt>.B }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case STNT1B_z_p_bi_contiguous:
      mnemonic = "stnt1b";
      form = "{ 'Zt.b }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // STNT1B { <Zt>.B }, <Pg>, [<Xn|SP>, <Xm>]
    case STNT1B_z_p_br_contiguous:
      mnemonic = "stnt1b";
      form = "{ 'Zt.b }, p'u1210, ['Xns, 'Rm]";
      break;
    // STNT1D { <Zt>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case STNT1D_z_p_bi_contiguous:
      mnemonic = "stnt1d";
      form = "{ 'Zt.d }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // STNT1D { <Zt>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]
    case STNT1D_z_p_br_contiguous:
      mnemonic = "stnt1d";
      form = "{ 'Zt.d }, p'u1210, ['Xns, 'Rm, LSL #3]";
      break;
    // STNT1H { <Zt>.H }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case STNT1H_z_p_bi_contiguous:
      mnemonic = "stnt1h";
      form = "{ 'Zt.h }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // STNT1H { <Zt>.H }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]
    case STNT1H_z_p_br_contiguous:
      mnemonic = "stnt1h";
      form = "{ 'Zt.h }, p'u1210, ['Xns, 'Rm, LSL #1]";
      break;
    // STNT1W { <Zt>.S }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
    case STNT1W_z_p_bi_contiguous:
      mnemonic = "stnt1w";
      form = "{ 'Zt.s }, p'u1210, ['Xns{, #'u1916, MUL VL}]";
      break;
    // STNT1W { <Zt>.S }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]
    case STNT1W_z_p_br_contiguous:
      mnemonic = "stnt1w";
      form = "{ 'Zt.s }, p'u1210, ['Xns, 'Rm, LSL #2]";
      break;
    // STR <Pt>, [<Xn|SP>{, #<imm>, MUL VL}]
    case STR_p_bi:
      mnemonic = "str";
      form = "'Pd, ['Xns{, #<imm>, MUL VL}]";
      break;
    // STR <Zt>, [<Xn|SP>{, #<imm>, MUL VL}]
    case STR_z_bi:
      mnemonic = "str";
      form = "'Zt, ['Xns{, #<imm>, MUL VL}]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEMulIndex(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEMulIndex)";

  switch (instr->Mask(SVEMulIndexMask)) {
    // SDOT <Zda>.D, <Zn>.H, <Zm>.H[<imm>]
    case SDOT_z_zzzi_d:
      mnemonic = "sdot";
      form = "'Zd.d, 'Zn.h, <Zm>.h[<imm>]";
      break;
    // SDOT <Zda>.S, <Zn>.B, <Zm>.B[<imm>]
    case SDOT_z_zzzi_s:
      mnemonic = "sdot";
      form = "'Zd.s, 'Zn.b, <Zm>.b[<imm>]";
      break;
    // UDOT <Zda>.D, <Zn>.H, <Zm>.H[<imm>]
    case UDOT_z_zzzi_d:
      mnemonic = "udot";
      form = "'Zd.d, 'Zn.h, <Zm>.h[<imm>]";
      break;
    // UDOT <Zda>.S, <Zn>.B, <Zm>.B[<imm>]
    case UDOT_z_zzzi_s:
      mnemonic = "udot";
      form = "'Zd.s, 'Zn.b, <Zm>.b[<imm>]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPartitionBreak(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEPartitionBreak)";

  switch (instr->Mask(SVEPartitionBreakMask)) {
    // BRKAS <Pd>.B, <Pg>/Z, <Pn>.B
    case BRKAS_p_p_p_z:
      mnemonic = "brkas";
      form = "'Pd.b, p'u1310/z, p'u0805.b";
      break;
    // BRKA <Pd>.B, <Pg>/<ZM>, <Pn>.B
    case BRKA_p_p_p:
      mnemonic = "brka";
      form = "'Pd.b, p'u1310/<ZM>, p'u0805.b";
      break;
    // BRKBS <Pd>.B, <Pg>/Z, <Pn>.B
    case BRKBS_p_p_p_z:
      mnemonic = "brkbs";
      form = "'Pd.b, p'u1310/z, p'u0805.b";
      break;
    // BRKB <Pd>.B, <Pg>/<ZM>, <Pn>.B
    case BRKB_p_p_p:
      mnemonic = "brkb";
      form = "'Pd.b, p'u1310/<ZM>, p'u0805.b";
      break;
    // BRKNS <Pdm>.B, <Pg>/Z, <Pn>.B, <Pdm>.B
    case BRKNS_p_p_pp:
      mnemonic = "brkns";
      form = "'Pd.b, p'u1310/z, p'u0805.b, 'Pd.b";
      break;
    // BRKN <Pdm>.B, <Pg>/Z, <Pn>.B, <Pdm>.B
    case BRKN_p_p_pp:
      mnemonic = "brkn";
      form = "'Pd.b, p'u1310/z, p'u0805.b, 'Pd.b";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPermutePredicate(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
  const char *form = "'Pd.'t, p'u0805.'t, p'u1916.'t";

  switch (instr->Mask(SVEPermutePredicateMask)) {
    // PUNPKHI <Pd>.H, <Pn>.B
    case PUNPKHI_p_p:
      mnemonic = "punpkhi";
      form = "'Pd.h, p'u0805.b";
      break;
    // PUNPKLO <Pd>.H, <Pn>.B
    case PUNPKLO_p_p:
      mnemonic = "punpklo";
      form = "'Pd.h, p'u0805.b";
      break;
    // REV <Pd>.<T>, <Pn>.<T>
    case REV_p_p:
      mnemonic = "rev";
      form = "'Pd.'t, p'u0805.'t";
      break;
    // TRN1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
    case TRN1_p_pp:
      mnemonic = "trn1";
      break;
    // TRN2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
    case TRN2_p_pp:
      mnemonic = "trn2";
      break;
    // UZP1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
    case UZP1_p_pp:
      mnemonic = "uzp1";
      break;
    // UZP2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
    case UZP2_p_pp:
      mnemonic = "uzp2";
      break;
    // ZIP1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
    case ZIP1_p_pp:
      mnemonic = "zip1";
      break;
    // ZIP2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
    case ZIP2_p_pp:
      mnemonic = "zip2";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPermuteVectorExtract(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEPermuteVectorExtract)";

  switch (instr->Mask(SVEPermuteVectorExtractMask)) {
    // EXT <Zdn>.B, <Zdn>.B, <Zm>.B, #<imm>
    case EXT_z_zi_des:
      mnemonic = "ext";
      form = "'Zd.b, 'Zd.b, 'Zn.b, #<imm>";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPermuteVectorInterleaving(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  const char *form = "'Zd.'t, 'Zn.'t, 'Zm.'t";

  switch (instr->Mask(SVEPermuteVectorInterleavingMask)) {
    // TRN1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case TRN1_z_zz:
      mnemonic = "trn1";
      break;
    // TRN2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case TRN2_z_zz:
      mnemonic = "trn2";
      break;
    // UZP1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case UZP1_z_zz:
      mnemonic = "uzp1";
      break;
    // UZP2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case UZP2_z_zz:
      mnemonic = "uzp2";
      break;
    // ZIP1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case ZIP1_z_zz:
      mnemonic = "zip1";
      break;
    // ZIP2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
    case ZIP2_z_zz:
      mnemonic = "zip2";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPermuteVectorPredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>
  const char *form = "'Zd.'t, p'u1210, 'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEPermuteVectorPredicatedMask)) {
    // CLASTA <R><dn>, <Pg>, <R><dn>, <Zm>.<T>
    case CLASTA_r_p_z:
      mnemonic = "clasta";
      form = "'Rd, p'u1210, 'Rd, 'Zn.'t";
      break;
    // CLASTA <V><dn>, <Pg>, <V><dn>, <Zm>.<T>
    case CLASTA_v_p_z:
      mnemonic = "clasta";
      form = "'Vd, p'u1210, 'Vd, 'Zn.'t";
      break;
    // CLASTA <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>
    case CLASTA_z_p_zz:
      mnemonic = "clasta";
      break;
    // CLASTB <R><dn>, <Pg>, <R><dn>, <Zm>.<T>
    case CLASTB_r_p_z:
      mnemonic = "clastb";
      form = "'Rd, p'u1210, 'Rd, 'Zn.'t";
      break;
    // CLASTB <V><dn>, <Pg>, <V><dn>, <Zm>.<T>
    case CLASTB_v_p_z:
      mnemonic = "clastb";
      form = "'Vd, p'u1210, 'Vd, 'Zn.'t";
      break;
    // CLASTB <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>
    case CLASTB_z_p_zz:
      mnemonic = "clastb";
      break;
    // COMPACT <Zd>.<T>, <Pg>, <Zn>.<T>
    case COMPACT_z_p_z:
      mnemonic = "compact";
      form = "'Zd.<T>, p'u1210, 'Zn.<T>";
      break;
    // CPY <Zd>.<T>, <Pg>/M, <R><n|SP>
    case CPY_z_p_r:
      mnemonic = "cpy";
      form = "'Zd.'t, p'u1210/m, 'Xns";
      break;
    // CPY <Zd>.<T>, <Pg>/M, <V><n>
    case CPY_z_p_v:
      mnemonic = "cpy";
      form = "'Zd.'t, p'u1210/m, 'Vn";
      break;
    // LASTA <R><d>, <Pg>, <Zn>.<T>
    case LASTA_r_p_z:
      mnemonic = "lasta";
      form = "'Rd, p'u1210, 'Zn.'t";
      break;
    // LASTA <V><d>, <Pg>, <Zn>.<T>
    case LASTA_v_p_z:
      mnemonic = "lasta";
      form = "'Vd, p'u1210, 'Zn.'t";
      break;
    // LASTB <R><d>, <Pg>, <Zn>.<T>
    case LASTB_r_p_z:
      mnemonic = "lastb";
      form = "'Rd, p'u1210, 'Zn.'t";
      break;
    // LASTB <V><d>, <Pg>, <Zn>.<T>
    case LASTB_v_p_z:
      mnemonic = "lastb";
      form = "'Vd, p'u1210, 'Zn.'t";
      break;
    // RBIT <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case RBIT_z_p_z:
      mnemonic = "rbit";
      form = "'Zd.'t, p'u1210/m, 'Zn.'t";
      break;
    // REVB <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case REVB_z_z:
      mnemonic = "revb";
      form = "'Zd.'t, p'u1210/m, 'Zn.'t";
      break;
    // REVH <Zd>.<T>, <Pg>/M, <Zn>.<T>
    case REVH_z_z:
      mnemonic = "revh";
      form = "'Zd.<T>, p'u1210/m, 'Zn.<T>";
      break;
    // REVW <Zd>.D, <Pg>/M, <Zn>.D
    case REVW_z_z:
      mnemonic = "revw";
      form = "'Zd.d, p'u1210/m, 'Zn.d";
      break;
    // SPLICE <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>
    case SPLICE_z_p_zz_des:
      mnemonic = "splice";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPermuteVectorUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Zd>.<T>, <Zn>.<Tb>
  const char *form = "'Zd.'t, 'Zn";

  switch (instr->Mask(SVEPermuteVectorUnpredicatedMask)) {
    // DUP <Zd>.<T>, <R><n|SP>
    case DUP_z_r:
      mnemonic = "dup";
      form = "'Zd.'t, 'Xns";
      break;
    // DUP <Zd>.<T>, <Zn>.<T>[<imm>]
    case DUP_z_zi:
      mnemonic = "dup";
      form = "'Zd.<T>, <Zn>.<T>[<imm>]";
      break;
    // INSR <Zdn>.<T>, <R><m>
    case INSR_z_r:
      mnemonic = "insr";
      form = "'Zd.'t, 'Rn";
      break;
    // INSR <Zdn>.<T>, <V><m>
    case INSR_z_v:
      mnemonic = "insr";
      form = "'Zd.'t, 'Vn";
      break;
    // REV <Zd>.<T>, <Zn>.<T>
    case REV_z_z:
      mnemonic = "rev";
      form = "'Zd.'t, 'Zn.'t";
      break;
    // SUNPKHI <Zd>.<T>, <Zn>.<Tb>
    case SUNPKHI_z_z:
      mnemonic = "sunpkhi";
      break;
    // SUNPKLO <Zd>.<T>, <Zn>.<Tb>
    case SUNPKLO_z_z:
      mnemonic = "sunpklo";
      break;
    // TBL <Zd>.<T>, { <Zn>.<T> }, <Zm>.<T>
    case TBL_z_zz_1:
      mnemonic = "tbl";
      form = "'Zd.'t, { 'Zn.'t }, 'Zm.'t";
      break;
    // UUNPKHI <Zd>.<T>, <Zn>.<Tb>
    case UUNPKHI_z_z:
      mnemonic = "uunpkhi";
      break;
    // UUNPKLO <Zd>.<T>, <Zn>.<Tb>
    case UUNPKLO_z_z:
      mnemonic = "uunpklo";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPredicateCount(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEPredicateCount)";

  switch (instr->Mask(SVEPredicateCountMask)) {
    // CNTP <Xd>, <Pg>, <Pn>.<T>
    case CNTP_r_p_p:
      mnemonic = "cntp";
      form = "'Rd, p'u1310, p'u0805.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPredicateLogicalOp(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  const char *form = "'Pd.b, p'u1310/z, p'u0805.b, p'u1916.b";

  switch (instr->Mask(SVEPredicateLogicalOpMask)) {
    // ANDS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case ANDS_p_p_pp_z:
      mnemonic = "ands";
      break;
    // AND <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case AND_p_p_pp_z:
      mnemonic = "and";
      break;
    // BICS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case BICS_p_p_pp_z:
      mnemonic = "bics";
      break;
    // BIC <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case BIC_p_p_pp_z:
      mnemonic = "bic";
      break;
    // EORS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case EORS_p_p_pp_z:
      mnemonic = "eors";
      break;
    // EOR <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case EOR_p_p_pp_z:
      mnemonic = "eor";
      break;
    // NANDS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case NANDS_p_p_pp_z:
      mnemonic = "nands";
      break;
    // NAND <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case NAND_p_p_pp_z:
      mnemonic = "nand";
      break;
    // NORS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case NORS_p_p_pp_z:
      mnemonic = "nors";
      break;
    // NOR <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case NOR_p_p_pp_z:
      mnemonic = "nor";
      break;
    // ORNS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case ORNS_p_p_pp_z:
      mnemonic = "orns";
      break;
    // ORN <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case ORN_p_p_pp_z:
      mnemonic = "orn";
      break;
    // ORRS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case ORRS_p_p_pp_z:
      mnemonic = "orrs";
      break;
    // ORR <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case ORR_p_p_pp_z:
      mnemonic = "orr";
      break;
    // SEL <Pd>.B, <Pg>, <Pn>.B, <Pm>.B
    case SEL_p_p_pp:
      mnemonic = "sel";
      form = "'Pd.b, p'u1310, p'u0805.b, p'u1916.b";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPredicateMisc(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEPredicateMisc)";

  switch (instr->Mask(SVEPredicateMiscMask)) {
    // PFALSE <Pd>.B
    case PFALSE_p:
      mnemonic = "pfalse";
      form = "'Pd.b";
      break;
    // PFIRST <Pdn>.B, <Pg>, <Pdn>.B
    case PFIRST_p_p_p:
      mnemonic = "pfirst";
      form = "'Pd.b, p'u0805, 'Pd.b";
      break;
    // PNEXT <Pdn>.<T>, <Pg>, <Pdn>.<T>
    case PNEXT_p_p_p:
      mnemonic = "pnext";
      form = "'Pd.'t, p'u0805, 'Pd.'t";
      break;
    // PTEST <Pg>, <Pn>.B
    case PTEST_p_p:
      mnemonic = "ptest";
      form = "p'u1310, p'u0805.b";
      break;
    // PTRUES <Pd>.<T>{, <pattern>}
    case PTRUES_p_s:
      mnemonic = "ptrues";
      form = "'Pd.'t{, #'u0905}";
      break;
    // PTRUE <Pd>.<T>{, <pattern>}
    case PTRUE_p_s:
      mnemonic = "ptrue";
      form = "'Pd.'t{, #'u0905}";
      break;
    // RDFFRS <Pd>.B, <Pg>/Z
    case RDFFRS_p_p_f:
      mnemonic = "rdffrs";
      form = "'Pd.b, p'u0805/z";
      break;
    // RDFFR <Pd>.B
    case RDFFR_p_f:
      mnemonic = "rdffr";
      form = "'Pd.b";
      break;
    // RDFFR <Pd>.B, <Pg>/Z
    case RDFFR_p_p_f:
      mnemonic = "rdffr";
      form = "'Pd.b, p'u0805/z";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPropagateBreak(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  const char *form = "'Pd.b, p'u1310/z, p'u0805.b, p'u1916.b";

  switch (instr->Mask(SVEPropagateBreakMask)) {
    // BRKPAS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case BRKPAS_p_p_pp:
      mnemonic = "brkpas";
      break;
    // BRKPA <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case BRKPA_p_p_pp:
      mnemonic = "brkpa";
      break;
    // BRKPBS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case BRKPBS_p_p_pp:
      mnemonic = "brkpbs";
      break;
    // BRKPB <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
    case BRKPB_p_p_pp:
      mnemonic = "brkpb";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEStackAllocation(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEStackAllocation)";

  switch (instr->Mask(SVEStackAllocationMask)) {
    // ADDPL <Xd|SP>, <Xn|SP>, #<imm>
    case ADDPL_r_ri:
      mnemonic = "addpl";
      form = "'Xds, 'Xns, #'u1005";
      break;
    // ADDVL <Xd|SP>, <Xn|SP>, #<imm>
    case ADDVL_r_ri:
      mnemonic = "addvl";
      form = "'Xds, 'Xns, #'u1005";
      break;
    // RDVL <Xd>, #<imm>
    case RDVL_r_i:
      mnemonic = "rdvl";
      form = "'Rd, #'u1005";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEVectorSelect(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEVectorSelect)";

  switch (instr->Mask(SVEVectorSelectMask)) {
    // SEL <Zd>.<T>, <Pg>, <Zn>.<T>, <Zm>.<T>
    case SEL_z_p_zz:
      mnemonic = "sel";
      form = "'Zd.'t, p'u1310, 'Zn.'t, 'Zm.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEWriteFFR(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEWriteFFR)";

  switch (instr->Mask(SVEWriteFFRMask)) {
    // SETFFR
    case SETFFR_f:
      mnemonic = "setffr";
      form = " ";
      break;
    // WRFFR <Pn>.B
    case WRFFR_f_p:
      mnemonic = "wrffr";
      form = "p'u0805.b";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitReserved(const Instruction *instr) {
  // UDF is the only instruction in this group, and the Decoder is precise.
  VIXL_ASSERT(instr->Mask(ReservedMask) == UDF);
  Format(instr, "udf", "'IUdf");
}


void Disassembler::VisitUnimplemented(const Instruction *instr) {
  Format(instr, "unimplemented", "(Unimplemented)");
}


void Disassembler::VisitUnallocated(const Instruction *instr) {
  Format(instr, "unallocated", "(Unallocated)");
}


void Disassembler::ProcessOutput(const Instruction * /*instr*/) {
  // The base disasm does nothing more than disassembling into a buffer.
}


void Disassembler::AppendRegisterNameToOutput(const Instruction *instr,
                                              const CPURegister &reg) {
  USE(instr);
  VIXL_ASSERT(reg.IsValid());
  char reg_char;

  if (reg.IsRegister()) {
    reg_char = reg.Is64Bits() ? 'x' : 'w';
  } else {
    VIXL_ASSERT(reg.IsVRegister());
    switch (reg.GetSizeInBits()) {
      case kBRegSize:
        reg_char = 'b';
        break;
      case kHRegSize:
        reg_char = 'h';
        break;
      case kSRegSize:
        reg_char = 's';
        break;
      case kDRegSize:
        reg_char = 'd';
        break;
      default:
        VIXL_ASSERT(reg.Is128Bits());
        reg_char = 'q';
    }
  }

  if (reg.IsVRegister() || !(reg.Aliases(sp) || reg.Aliases(xzr))) {
    // A core or scalar/vector register: [wx]0 - 30, [bhsdq]0 - 31.
    AppendToOutput("%c%d", reg_char, reg.GetCode());
  } else if (reg.Aliases(sp)) {
    // Disassemble w31/x31 as stack pointer wsp/sp.
    AppendToOutput("%s", reg.Is64Bits() ? "sp" : "wsp");
  } else {
    // Disassemble w31/x31 as zero register wzr/xzr.
    AppendToOutput("%czr", reg_char);
  }
}


void Disassembler::AppendPCRelativeOffsetToOutput(const Instruction *instr,
                                                  int64_t offset) {
  USE(instr);
  if (offset < 0) {
    // Cast to uint64_t so that INT64_MIN is handled in a well-defined way.
    uint64_t abs_offset = -static_cast<uint64_t>(offset);
    AppendToOutput("#-0x%" PRIx64, abs_offset);
  } else {
    AppendToOutput("#+0x%" PRIx64, offset);
  }
}


void Disassembler::AppendAddressToOutput(const Instruction *instr,
                                         const void *addr) {
  USE(instr);
  AppendToOutput("(addr 0x%" PRIxPTR ")", reinterpret_cast<uintptr_t>(addr));
}


void Disassembler::AppendCodeAddressToOutput(const Instruction *instr,
                                             const void *addr) {
  AppendAddressToOutput(instr, addr);
}


void Disassembler::AppendDataAddressToOutput(const Instruction *instr,
                                             const void *addr) {
  AppendAddressToOutput(instr, addr);
}


void Disassembler::AppendCodeRelativeAddressToOutput(const Instruction *instr,
                                                     const void *addr) {
  USE(instr);
  int64_t rel_addr = CodeRelativeAddress(addr);
  if (rel_addr >= 0) {
    AppendToOutput("(addr 0x%" PRIx64 ")", rel_addr);
  } else {
    AppendToOutput("(addr -0x%" PRIx64 ")", -rel_addr);
  }
}


void Disassembler::AppendCodeRelativeCodeAddressToOutput(
    const Instruction *instr, const void *addr) {
  AppendCodeRelativeAddressToOutput(instr, addr);
}


void Disassembler::AppendCodeRelativeDataAddressToOutput(
    const Instruction *instr, const void *addr) {
  AppendCodeRelativeAddressToOutput(instr, addr);
}


void Disassembler::MapCodeAddress(int64_t base_address,
                                  const Instruction *instr_address) {
  set_code_address_offset(base_address -
                          reinterpret_cast<intptr_t>(instr_address));
}
int64_t Disassembler::CodeRelativeAddress(const void *addr) {
  return reinterpret_cast<intptr_t>(addr) + code_address_offset();
}


void Disassembler::Format(const Instruction *instr,
                          const char *mnemonic,
                          const char *format) {
  VIXL_ASSERT(mnemonic != NULL);
  ResetOutput();
  Substitute(instr, mnemonic);
  if (format != NULL) {
    VIXL_ASSERT(buffer_pos_ < buffer_size_);
    buffer_[buffer_pos_++] = ' ';
    Substitute(instr, format);
  }
  VIXL_ASSERT(buffer_pos_ < buffer_size_);
  buffer_[buffer_pos_] = 0;
  ProcessOutput(instr);
}


void Disassembler::Substitute(const Instruction *instr, const char *string) {
  char chr = *string++;
  while (chr != '\0') {
    if (chr == '\'') {
      string += SubstituteField(instr, string);
    } else {
      VIXL_ASSERT(buffer_pos_ < buffer_size_);
      buffer_[buffer_pos_++] = chr;
    }
    chr = *string++;
  }
}


int Disassembler::SubstituteField(const Instruction *instr,
                                  const char *format) {
  switch (format[0]) {
    // NB. The remaining substitution prefix upper-case characters are: JKU.
    case 'R':  // Register. X or W, selected by sf bit.
    case 'F':  // FP register. S or D, selected by type field.
    case 'V':  // Vector register, V, vector format.
    case 'Z':  // Scalable vector register.
    case 'W':
    case 'X':
    case 'B':
    case 'H':
    case 'S':
    case 'D':
    case 'Q':
      return SubstituteRegisterField(instr, format);
    case 'P':
      return SubstitutePredicateRegisterField(instr, format);
    case 'I':
      return SubstituteImmediateField(instr, format);
    case 'L':
      return SubstituteLiteralField(instr, format);
    case 'N':
      return SubstituteShiftField(instr, format);
    case 'C':
      return SubstituteConditionField(instr, format);
    case 'E':
      return SubstituteExtendField(instr, format);
    case 'A':
      return SubstitutePCRelAddressField(instr, format);
    case 'T':
      return SubstituteBranchTargetField(instr, format);
    case 'O':
      return SubstituteLSRegOffsetField(instr, format);
    case 'M':
      return SubstituteBarrierField(instr, format);
    case 'K':
      return SubstituteCrField(instr, format);
    case 'G':
      return SubstituteSysOpField(instr, format);
    case 'p':
      return SubstitutePrefetchField(instr, format);
    case 'u':
      return SubstituteUIntField(instr, format);
    case 't':
      return SubstituteSVESize(instr, format);
    default: {
      VIXL_UNREACHABLE();
      return 1;
    }
  }
}


int Disassembler::SubstituteRegisterField(const Instruction *instr,
                                          const char *format) {
  char reg_prefix = format[0];
  unsigned reg_num = 0;
  unsigned field_len = 2;

  switch (format[1]) {
    case 'd':
      reg_num = instr->GetRd();
      if (format[2] == 'q') {
        reg_prefix = instr->GetNEONQ() ? 'X' : 'W';
        field_len = 3;
      }
      break;
    case 'n':
      reg_num = instr->GetRn();
      break;
    case 'm':
      reg_num = instr->GetRm();
      switch (format[2]) {
        // Handle registers tagged with b (bytes), z (instruction), or
        // r (registers), used for address updates in
        // NEON load/store instructions.
        case 'r':
        case 'b':
        case 'z': {
          field_len = 3;
          char *eimm;
          int imm = static_cast<int>(strtol(&format[3], &eimm, 10));
          field_len += eimm - &format[3];
          if (reg_num == 31) {
            switch (format[2]) {
              case 'z':
                imm *= (1 << instr->GetNEONLSSize());
                break;
              case 'r':
                imm *= (instr->GetNEONQ() == 0) ? kDRegSizeInBytes
                                                : kQRegSizeInBytes;
                break;
              case 'b':
                break;
            }
            AppendToOutput("#%d", imm);
            return field_len;
          }
          break;
        }
      }
      break;
    case 'e':
      // This is register Rm, but using a 4-bit specifier. Used in NEON
      // by-element instructions.
      reg_num = instr->GetRmLow16();
      break;
    case 'a':
      reg_num = instr->GetRa();
      break;
    case 's':
      reg_num = instr->GetRs();
      break;
    case 't':
      reg_num = instr->GetRt();
      if (format[0] == 'V') {
        if ((format[2] >= '2') && (format[2] <= '4')) {
          // Handle consecutive vector register specifiers Vt2, Vt3 and Vt4.
          reg_num = (reg_num + format[2] - '1') % 32;
          field_len = 3;
        }
      } else {
        if (format[2] == '2') {
          // Handle register specifier Rt2.
          reg_num = instr->GetRt2();
          field_len = 3;
        }
      }
      break;
    case '(': {
      switch (format[2]) {
        case 's':
          reg_num = instr->GetRs();
          break;
        case 't':
          reg_num = instr->GetRt();
          break;
        default:
          VIXL_UNREACHABLE();
      }

      VIXL_ASSERT(format[3] == '+');
      int i = 4;
      int addition = 0;
      while (format[i] != ')') {
        VIXL_ASSERT((format[i] >= '0') && (format[i] <= '9'));
        addition *= 10;
        addition += format[i] - '0';
        ++i;
      }
      reg_num += addition;
      field_len = i + 1;
      break;
    }
    default:
      VIXL_UNREACHABLE();
  }

  // Increase field length for registers tagged as stack.
  if (format[1] != '(' && format[2] == 's') {
    field_len = 3;
  }

  CPURegister::RegisterType reg_type = CPURegister::kRegister;
  unsigned reg_size = kXRegSize;

  switch (reg_prefix) {
    case 'R':
      reg_prefix = instr->GetSixtyFourBits() ? 'X' : 'W';
      break;
    case 'F':
      switch (instr->GetFPType()) {
        case 3:
          reg_prefix = 'H';
          break;
        case 0:
          reg_prefix = 'S';
          break;
        default:
          reg_prefix = 'D';
      }
  }

  switch (reg_prefix) {
    case 'W':
      reg_type = CPURegister::kRegister;
      reg_size = kWRegSize;
      break;
    case 'X':
      reg_type = CPURegister::kRegister;
      reg_size = kXRegSize;
      break;
    case 'B':
      reg_type = CPURegister::kVRegister;
      reg_size = kBRegSize;
      break;
    case 'H':
      reg_type = CPURegister::kVRegister;
      reg_size = kHRegSize;
      break;
    case 'S':
      reg_type = CPURegister::kVRegister;
      reg_size = kSRegSize;
      break;
    case 'D':
      reg_type = CPURegister::kVRegister;
      reg_size = kDRegSize;
      break;
    case 'Q':
      reg_type = CPURegister::kVRegister;
      reg_size = kQRegSize;
      break;
    case 'V':
      AppendToOutput("v%d", reg_num);
      return field_len;
    case 'Z':
      AppendToOutput("z%d", reg_num);
      return field_len;
    default:
      VIXL_UNREACHABLE();
  }

  if ((reg_type == CPURegister::kRegister) && (reg_num == kZeroRegCode) &&
      (format[2] == 's')) {
    reg_num = kSPRegInternalCode;
  }

  AppendRegisterNameToOutput(instr, CPURegister(reg_num, reg_size, reg_type));

  return field_len;
}

int Disassembler::SubstitutePredicateRegisterField(const Instruction *instr,
                                                   const char *format) {
  USE(format);
  // Only 'Pd is supported, as it's the only predicate register always encoded
  // in the same position in an instruction.
  VIXL_ASSERT((format[0] == 'P') && (format[1] == 'd'));
  AppendToOutput("p%u", instr->ExtractBits(3, 0));
  return 2;
}

int Disassembler::SubstituteImmediateField(const Instruction *instr,
                                           const char *format) {
  VIXL_ASSERT(format[0] == 'I');

  switch (format[1]) {
    case 'M': {  // IMoveImm, IMoveNeg or IMoveLSL.
      if (format[5] == 'L') {
        AppendToOutput("#0x%" PRIx32, instr->GetImmMoveWide());
        if (instr->GetShiftMoveWide() > 0) {
          AppendToOutput(", lsl #%" PRId32, 16 * instr->GetShiftMoveWide());
        }
      } else {
        VIXL_ASSERT((format[5] == 'I') || (format[5] == 'N'));
        uint64_t imm = static_cast<uint64_t>(instr->GetImmMoveWide())
                       << (16 * instr->GetShiftMoveWide());
        if (format[5] == 'N') imm = ~imm;
        if (!instr->GetSixtyFourBits()) imm &= UINT64_C(0xffffffff);
        AppendToOutput("#0x%" PRIx64, imm);
      }
      return 8;
    }
    case 'L': {
      switch (format[2]) {
        case 'L': {  // ILLiteral - Immediate Load Literal.
          AppendToOutput("pc%+" PRId32,
                         instr->GetImmLLiteral() *
                             static_cast<int>(kLiteralEntrySize));
          return 9;
        }
        case 'S': {  // ILS - Immediate Load/Store.
                     // ILSi - As above, but an index field which must not be
                     // omitted even if it is zero.
          bool is_index = format[3] == 'i';
          if (is_index || (instr->GetImmLS() != 0)) {
            AppendToOutput(", #%" PRId32, instr->GetImmLS());
          }
          return is_index ? 4 : 3;
        }
        case 'P': {  // ILPx - Immediate Load/Store Pair, x = access size.
                     // ILPxi - As above, but an index field which must not be
                     // omitted even if it is zero.
          VIXL_ASSERT((format[3] >= '0') && (format[3] <= '9'));
          bool is_index = format[4] == 'i';
          if (is_index || (instr->GetImmLSPair() != 0)) {
            // format[3] is the scale value. Convert to a number.
            int scale = 1 << (format[3] - '0');
            AppendToOutput(", #%" PRId32, instr->GetImmLSPair() * scale);
          }
          return is_index ? 5 : 4;
        }
        case 'U': {  // ILU - Immediate Load/Store Unsigned.
          if (instr->GetImmLSUnsigned() != 0) {
            int shift = instr->GetSizeLS();
            AppendToOutput(", #%" PRId32, instr->GetImmLSUnsigned() << shift);
          }
          return 3;
        }
        case 'F': {  // ILF(CNR) - Immediate Rotation Value for Complex Numbers
          AppendToOutput("#%" PRId32, instr->GetImmRotFcmlaSca() * 90);
          return strlen("ILFCNR");
        }
        case 'A': {  // ILA - Immediate Load with pointer authentication.
          if (instr->GetImmLSPAC() != 0) {
            AppendToOutput(", #%" PRId32, instr->GetImmLSPAC());
          }
          return 3;
        }
        default: {
          VIXL_UNIMPLEMENTED();
          return 0;
        }
      }
    }
    case 'C': {  // ICondB - Immediate Conditional Branch.
      int64_t offset = instr->GetImmCondBranch() << 2;
      AppendPCRelativeOffsetToOutput(instr, offset);
      return 6;
    }
    case 'A': {  // IAddSub.
      VIXL_ASSERT(instr->GetShiftAddSub() <= 1);
      int64_t imm = instr->GetImmAddSub() << (12 * instr->GetShiftAddSub());
      AppendToOutput("#0x%" PRIx64 " (%" PRId64 ")", imm, imm);
      return 7;
    }
    case 'F': {                // IFPHalf, IFPSingle, IFPDouble, or IFPFBits.
      if (format[3] == 'F') {  // IFPFbits.
        AppendToOutput("#%" PRId32, 64 - instr->GetFPScale());
        return 8;
      } else {
        AppendToOutput("#0x%" PRIx32 " (%.4f)",
                       instr->GetImmFP(),
                       format[3] == 'H'
                           ? FPToFloat(instr->GetImmFP16(), kIgnoreDefaultNaN)
                           : (format[3] == 'S') ? instr->GetImmFP32()
                                                : instr->GetImmFP64());
        if (format[3] == 'H') {
          return 7;
        } else {
          return 9;
        }
      }
    }
    case 'H': {  // IH - ImmHint
      AppendToOutput("#%" PRId32, instr->GetImmHint());
      return 2;
    }
    case 'T': {  // ITri - Immediate Triangular Encoded.
      AppendToOutput("#0x%" PRIx64, instr->GetImmLogical());
      return 4;
    }
    case 'N': {  // INzcv.
      int nzcv = (instr->GetNzcv() << Flags_offset);
      AppendToOutput("#%c%c%c%c",
                     ((nzcv & NFlag) == 0) ? 'n' : 'N',
                     ((nzcv & ZFlag) == 0) ? 'z' : 'Z',
                     ((nzcv & CFlag) == 0) ? 'c' : 'C',
                     ((nzcv & VFlag) == 0) ? 'v' : 'V');
      return 5;
    }
    case 'P': {  // IP - Conditional compare.
      AppendToOutput("#%" PRId32, instr->GetImmCondCmp());
      return 2;
    }
    case 'B': {  // Bitfields.
      return SubstituteBitfieldImmediateField(instr, format);
    }
    case 'E': {  // IExtract.
      AppendToOutput("#%" PRId32, instr->GetImmS());
      return 8;
    }
    case 'S': {  // IS - Test and branch bit.
      AppendToOutput("#%" PRId32,
                     (instr->GetImmTestBranchBit5() << 5) |
                         instr->GetImmTestBranchBit40());
      return 2;
    }
    case 's': {  // Is - Shift (immediate).
      switch (format[2]) {
        case '1': {  // Is1 - SSHR.
          int shift = 16 << HighestSetBitPosition(instr->GetImmNEONImmh());
          shift -= instr->GetImmNEONImmhImmb();
          AppendToOutput("#%d", shift);
          return 3;
        }
        case '2': {  // Is2 - SLI.
          int shift = instr->GetImmNEONImmhImmb();
          shift -= 8 << HighestSetBitPosition(instr->GetImmNEONImmh());
          AppendToOutput("#%d", shift);
          return 3;
        }
        default: {
          VIXL_UNIMPLEMENTED();
          return 0;
        }
      }
    }
    case 'D': {  // IDebug - HLT and BRK instructions.
      AppendToOutput("#0x%" PRIx32, instr->GetImmException());
      return 6;
    }
    case 'U': {  // IUdf - UDF immediate.
      AppendToOutput("#0x%" PRIx32, instr->GetImmUdf());
      return 4;
    }
    case 'V': {  // Immediate Vector.
      switch (format[2]) {
        case 'F': {
          switch (format[5]) {
            // Convert 'rot' bit encodings into equivalent angle rotation
            case 'A':
              AppendToOutput("#%" PRId32,
                             instr->GetImmRotFcadd() == 1 ? 270 : 90);
              break;
            case 'M':
              AppendToOutput("#%" PRId32, instr->GetImmRotFcmlaVec() * 90);
              break;
          }
          return strlen("IVFCN") + 1;
        }
        case 'E': {  // IVExtract.
          AppendToOutput("#%" PRId32, instr->GetImmNEONExt());
          return 9;
        }
        case 'B': {  // IVByElemIndex.
          int ret = strlen("IVByElemIndex");
          int vm_index = (instr->GetNEONH() << 1) | instr->GetNEONL();
          static const char *format_rot = "IVByElemIndexRot";
          static const char *format_fhm = "IVByElemIndexFHM";
          bool is_fhm = strncmp(format, format_fhm, strlen(format_fhm)) == 0;
          if (strncmp(format, format_rot, strlen(format_rot)) == 0) {
            // FCMLA uses 'H' bit index when SIZE is 2, else H:L
            if (instr->GetNEONSize() == 2) {
              vm_index = instr->GetNEONH();
            }
            ret = static_cast<int>(strlen(format_rot));
          } else if (is_fhm || (instr->GetNEONSize() == 0)) {
            // Half-precision FP ops use H:L:M bit index
            // Widening operations with H-sized operands also use H:L:M.
            vm_index = (instr->GetNEONH() << 2) | (instr->GetNEONL() << 1) |
                       instr->GetNEONM();
            if (is_fhm) ret = static_cast<int>(strlen(format_fhm));
          } else if (instr->GetNEONSize() == 1) {
            vm_index = (vm_index << 1) | instr->GetNEONM();
          }
          AppendToOutput("%d", vm_index);
          return ret;
        }
        case 'I': {  // INS element.
          if (strncmp(format, "IVInsIndex", strlen("IVInsIndex")) == 0) {
            unsigned rd_index, rn_index;
            unsigned imm5 = instr->GetImmNEON5();
            unsigned imm4 = instr->GetImmNEON4();
            int tz = CountTrailingZeros(imm5, 32);
            if (tz <= 3) {  // Defined for tz = 0 to 3 only.
              rd_index = imm5 >> (tz + 1);
              rn_index = imm4 >> tz;
              if (strncmp(format, "IVInsIndex1", strlen("IVInsIndex1")) == 0) {
                AppendToOutput("%d", rd_index);
                return strlen("IVInsIndex1");
              } else if (strncmp(format,
                                 "IVInsIndex2",
                                 strlen("IVInsIndex2")) == 0) {
                AppendToOutput("%d", rn_index);
                return strlen("IVInsIndex2");
              }
            }
            return 0;
          }
          VIXL_FALLTHROUGH();
        }
        case 'L': {  // IVLSLane[0123] - suffix indicates access size shift.
          AppendToOutput("%d", instr->GetNEONLSIndex(format[8] - '0'));
          return 9;
        }
        case 'M': {  // Modified Immediate cases.
          if (strncmp(format, "IVMIImmFPHalf", strlen("IVMIImmFPHalf")) == 0) {
            AppendToOutput("#0x%" PRIx32 " (%.4f)",
                           instr->GetImmNEONabcdefgh(),
                           FPToFloat(instr->GetImmNEONFP16(),
                                     kIgnoreDefaultNaN));
            return strlen("IVMIImmFPHalf");
          } else if (strncmp(format,
                             "IVMIImmFPSingle",
                             strlen("IVMIImmFPSingle")) == 0) {
            AppendToOutput("#0x%" PRIx32 " (%.4f)",
                           instr->GetImmNEONabcdefgh(),
                           instr->GetImmNEONFP32());
            return strlen("IVMIImmFPSingle");
          } else if (strncmp(format,
                             "IVMIImmFPDouble",
                             strlen("IVMIImmFPDouble")) == 0) {
            AppendToOutput("#0x%" PRIx32 " (%.4f)",
                           instr->GetImmNEONabcdefgh(),
                           instr->GetImmNEONFP64());
            return strlen("IVMIImmFPDouble");
          } else if (strncmp(format, "IVMIImm8", strlen("IVMIImm8")) == 0) {
            uint64_t imm8 = instr->GetImmNEONabcdefgh();
            AppendToOutput("#0x%" PRIx64, imm8);
            return strlen("IVMIImm8");
          } else if (strncmp(format, "IVMIImm", strlen("IVMIImm")) == 0) {
            uint64_t imm8 = instr->GetImmNEONabcdefgh();
            uint64_t imm = 0;
            for (int i = 0; i < 8; ++i) {
              if (imm8 & (1 << i)) {
                imm |= (UINT64_C(0xff) << (8 * i));
              }
            }
            AppendToOutput("#0x%" PRIx64, imm);
            return strlen("IVMIImm");
          } else if (strncmp(format,
                             "IVMIShiftAmt1",
                             strlen("IVMIShiftAmt1")) == 0) {
            int cmode = instr->GetNEONCmode();
            int shift_amount = 8 * ((cmode >> 1) & 3);
            AppendToOutput("#%d", shift_amount);
            return strlen("IVMIShiftAmt1");
          } else if (strncmp(format,
                             "IVMIShiftAmt2",
                             strlen("IVMIShiftAmt2")) == 0) {
            int cmode = instr->GetNEONCmode();
            int shift_amount = 8 << (cmode & 1);
            AppendToOutput("#%d", shift_amount);
            return strlen("IVMIShiftAmt2");
          } else {
            VIXL_UNIMPLEMENTED();
            return 0;
          }
        }
        default: {
          VIXL_UNIMPLEMENTED();
          return 0;
        }
      }
    }
    case 'X': {  // IX - CLREX instruction.
      AppendToOutput("#0x%" PRIx32, instr->GetCRm());
      return 2;
    }
    case 'Y': {  // IY - system register immediate.
      switch (instr->GetImmSystemRegister()) {
        case NZCV:
          AppendToOutput("nzcv");
          break;
        case FPCR:
          AppendToOutput("fpcr");
          break;
        case RNDR:
          AppendToOutput("rndr");
          break;
        case RNDRRS:
          AppendToOutput("rndrrs");
          break;
        default:
          AppendToOutput("S%d_%d_c%d_c%d_%d",
                         instr->GetSysOp0(),
                         instr->GetSysOp1(),
                         instr->GetCRn(),
                         instr->GetCRm(),
                         instr->GetSysOp2());
          break;
      }
      return 2;
    }
    case 'R': {  // IR - Rotate right into flags.
      switch (format[2]) {
        case 'r': {  // IRr - Rotate amount.
          AppendToOutput("#%d", instr->GetImmRMIFRotation());
          return 3;
        }
        default: {
          VIXL_UNIMPLEMENTED();
          return 0;
        }
      }
    }
    default: {
      VIXL_UNIMPLEMENTED();
      return 0;
    }
  }
}


int Disassembler::SubstituteBitfieldImmediateField(const Instruction *instr,
                                                   const char *format) {
  VIXL_ASSERT((format[0] == 'I') && (format[1] == 'B'));
  unsigned r = instr->GetImmR();
  unsigned s = instr->GetImmS();

  switch (format[2]) {
    case 'r': {  // IBr.
      AppendToOutput("#%d", r);
      return 3;
    }
    case 's': {  // IBs+1 or IBs-r+1.
      if (format[3] == '+') {
        AppendToOutput("#%d", s + 1);
        return 5;
      } else {
        VIXL_ASSERT(format[3] == '-');
        AppendToOutput("#%d", s - r + 1);
        return 7;
      }
    }
    case 'Z': {  // IBZ-r.
      VIXL_ASSERT((format[3] == '-') && (format[4] == 'r'));
      unsigned reg_size =
          (instr->GetSixtyFourBits() == 1) ? kXRegSize : kWRegSize;
      AppendToOutput("#%d", reg_size - r);
      return 5;
    }
    default: {
      VIXL_UNREACHABLE();
      return 0;
    }
  }
}


int Disassembler::SubstituteLiteralField(const Instruction *instr,
                                         const char *format) {
  VIXL_ASSERT(strncmp(format, "LValue", 6) == 0);
  USE(format);

  const void *address = instr->GetLiteralAddress<const void *>();
  switch (instr->Mask(LoadLiteralMask)) {
    case LDR_w_lit:
    case LDR_x_lit:
    case LDRSW_x_lit:
    case LDR_s_lit:
    case LDR_d_lit:
    case LDR_q_lit:
      AppendCodeRelativeDataAddressToOutput(instr, address);
      break;
    case PRFM_lit: {
      // Use the prefetch hint to decide how to print the address.
      switch (instr->GetPrefetchHint()) {
        case 0x0:  // PLD: prefetch for load.
        case 0x2:  // PST: prepare for store.
          AppendCodeRelativeDataAddressToOutput(instr, address);
          break;
        case 0x1:  // PLI: preload instructions.
          AppendCodeRelativeCodeAddressToOutput(instr, address);
          break;
        case 0x3:  // Unallocated hint.
          AppendCodeRelativeAddressToOutput(instr, address);
          break;
      }
      break;
    }
    default:
      VIXL_UNREACHABLE();
  }

  return 6;
}


int Disassembler::SubstituteShiftField(const Instruction *instr,
                                       const char *format) {
  VIXL_ASSERT(format[0] == 'N');
  VIXL_ASSERT(instr->GetShiftDP() <= 0x3);

  switch (format[1]) {
    case 'D': {  // HDP.
      VIXL_ASSERT(instr->GetShiftDP() != ROR);
      VIXL_FALLTHROUGH();
    }
    case 'L': {  // HLo.
      if (instr->GetImmDPShift() != 0) {
        const char *shift_type[] = {"lsl", "lsr", "asr", "ror"};
        AppendToOutput(", %s #%" PRId32,
                       shift_type[instr->GetShiftDP()],
                       instr->GetImmDPShift());
      }
      return 3;
    }
    default:
      VIXL_UNIMPLEMENTED();
      return 0;
  }
}


int Disassembler::SubstituteConditionField(const Instruction *instr,
                                           const char *format) {
  VIXL_ASSERT(format[0] == 'C');
  const char *condition_code[] = {"eq",
                                  "ne",
                                  "hs",
                                  "lo",
                                  "mi",
                                  "pl",
                                  "vs",
                                  "vc",
                                  "hi",
                                  "ls",
                                  "ge",
                                  "lt",
                                  "gt",
                                  "le",
                                  "al",
                                  "nv"};
  int cond;
  switch (format[1]) {
    case 'B':
      cond = instr->GetConditionBranch();
      break;
    case 'I': {
      cond = InvertCondition(static_cast<Condition>(instr->GetCondition()));
      break;
    }
    default:
      cond = instr->GetCondition();
  }
  AppendToOutput("%s", condition_code[cond]);
  return 4;
}


int Disassembler::SubstitutePCRelAddressField(const Instruction *instr,
                                              const char *format) {
  VIXL_ASSERT((strcmp(format, "AddrPCRelByte") == 0) ||  // Used by `adr`.
              (strcmp(format, "AddrPCRelPage") == 0));   // Used by `adrp`.

  int64_t offset = instr->GetImmPCRel();

  // Compute the target address based on the effective address (after applying
  // code_address_offset). This is required for correct behaviour of adrp.
  const Instruction *base = instr + code_address_offset();
  if (format[9] == 'P') {
    offset *= kPageSize;
    base = AlignDown(base, kPageSize);
  }
  // Strip code_address_offset before printing, so we can use the
  // semantically-correct AppendCodeRelativeAddressToOutput.
  const void *target =
      reinterpret_cast<const void *>(base + offset - code_address_offset());

  AppendPCRelativeOffsetToOutput(instr, offset);
  AppendToOutput(" ");
  AppendCodeRelativeAddressToOutput(instr, target);
  return 13;
}


int Disassembler::SubstituteBranchTargetField(const Instruction *instr,
                                              const char *format) {
  VIXL_ASSERT(strncmp(format, "TImm", 4) == 0);

  int64_t offset = 0;
  switch (format[5]) {
    // BImmUncn - unconditional branch immediate.
    case 'n':
      offset = instr->GetImmUncondBranch();
      break;
    // BImmCond - conditional branch immediate.
    case 'o':
      offset = instr->GetImmCondBranch();
      break;
    // BImmCmpa - compare and branch immediate.
    case 'm':
      offset = instr->GetImmCmpBranch();
      break;
    // BImmTest - test and branch immediate.
    case 'e':
      offset = instr->GetImmTestBranch();
      break;
    default:
      VIXL_UNIMPLEMENTED();
  }
  offset *= static_cast<int>(kInstructionSize);
  const void *target_address = reinterpret_cast<const void *>(instr + offset);
  VIXL_STATIC_ASSERT(sizeof(*instr) == 1);

  AppendPCRelativeOffsetToOutput(instr, offset);
  AppendToOutput(" ");
  AppendCodeRelativeCodeAddressToOutput(instr, target_address);

  return 8;
}


int Disassembler::SubstituteExtendField(const Instruction *instr,
                                        const char *format) {
  VIXL_ASSERT(strncmp(format, "Ext", 3) == 0);
  VIXL_ASSERT(instr->GetExtendMode() <= 7);
  USE(format);

  const char *extend_mode[] =
      {"uxtb", "uxth", "uxtw", "uxtx", "sxtb", "sxth", "sxtw", "sxtx"};

  // If rd or rn is SP, uxtw on 32-bit registers and uxtx on 64-bit
  // registers becomes lsl.
  if (((instr->GetRd() == kZeroRegCode) || (instr->GetRn() == kZeroRegCode)) &&
      (((instr->GetExtendMode() == UXTW) && (instr->GetSixtyFourBits() == 0)) ||
       (instr->GetExtendMode() == UXTX))) {
    if (instr->GetImmExtendShift() > 0) {
      AppendToOutput(", lsl #%" PRId32, instr->GetImmExtendShift());
    }
  } else {
    AppendToOutput(", %s", extend_mode[instr->GetExtendMode()]);
    if (instr->GetImmExtendShift() > 0) {
      AppendToOutput(" #%" PRId32, instr->GetImmExtendShift());
    }
  }
  return 3;
}


int Disassembler::SubstituteLSRegOffsetField(const Instruction *instr,
                                             const char *format) {
  VIXL_ASSERT(strncmp(format, "Offsetreg", 9) == 0);
  const char *extend_mode[] = {"undefined",
                               "undefined",
                               "uxtw",
                               "lsl",
                               "undefined",
                               "undefined",
                               "sxtw",
                               "sxtx"};
  USE(format);

  unsigned shift = instr->GetImmShiftLS();
  Extend ext = static_cast<Extend>(instr->GetExtendMode());
  char reg_type = ((ext == UXTW) || (ext == SXTW)) ? 'w' : 'x';

  unsigned rm = instr->GetRm();
  if (rm == kZeroRegCode) {
    AppendToOutput("%czr", reg_type);
  } else {
    AppendToOutput("%c%d", reg_type, rm);
  }

  // Extend mode UXTX is an alias for shift mode LSL here.
  if (!((ext == UXTX) && (shift == 0))) {
    AppendToOutput(", %s", extend_mode[ext]);
    if (shift != 0) {
      AppendToOutput(" #%d", instr->GetSizeLS());
    }
  }
  return 9;
}


int Disassembler::SubstitutePrefetchField(const Instruction *instr,
                                          const char *format) {
  VIXL_ASSERT(format[0] == 'p');
  USE(format);

  static const char *hints[] = {"ld", "li", "st"};
  static const char *stream_options[] = {"keep", "strm"};

  unsigned hint = instr->GetPrefetchHint();
  unsigned target = instr->GetPrefetchTarget() + 1;
  unsigned stream = instr->GetPrefetchStream();

  if ((hint >= ArrayLength(hints)) || (target > 3)) {
    // Unallocated prefetch operations.
    int prefetch_mode = instr->GetImmPrefetchOperation();
    AppendToOutput("#0b%c%c%c%c%c",
                   (prefetch_mode & (1 << 4)) ? '1' : '0',
                   (prefetch_mode & (1 << 3)) ? '1' : '0',
                   (prefetch_mode & (1 << 2)) ? '1' : '0',
                   (prefetch_mode & (1 << 1)) ? '1' : '0',
                   (prefetch_mode & (1 << 0)) ? '1' : '0');
  } else {
    VIXL_ASSERT(stream < ArrayLength(stream_options));
    AppendToOutput("p%sl%d%s", hints[hint], target, stream_options[stream]);
  }
  return 6;
}

int Disassembler::SubstituteBarrierField(const Instruction *instr,
                                         const char *format) {
  VIXL_ASSERT(format[0] == 'M');
  USE(format);

  static const char *options[4][4] = {{"sy (0b0000)", "oshld", "oshst", "osh"},
                                      {"sy (0b0100)", "nshld", "nshst", "nsh"},
                                      {"sy (0b1000)", "ishld", "ishst", "ish"},
                                      {"sy (0b1100)", "ld", "st", "sy"}};
  int domain = instr->GetImmBarrierDomain();
  int type = instr->GetImmBarrierType();

  AppendToOutput("%s", options[domain][type]);
  return 1;
}

int Disassembler::SubstituteSysOpField(const Instruction *instr,
                                       const char *format) {
  VIXL_ASSERT(format[0] == 'G');
  int op = -1;
  switch (format[1]) {
    case '1':
      op = instr->GetSysOp1();
      break;
    case '2':
      op = instr->GetSysOp2();
      break;
    default:
      VIXL_UNREACHABLE();
  }
  AppendToOutput("#%d", op);
  return 2;
}

int Disassembler::SubstituteCrField(const Instruction *instr,
                                    const char *format) {
  VIXL_ASSERT(format[0] == 'K');
  int cr = -1;
  switch (format[1]) {
    case 'n':
      cr = instr->GetCRn();
      break;
    case 'm':
      cr = instr->GetCRm();
      break;
    default:
      VIXL_UNREACHABLE();
  }
  AppendToOutput("C%d", cr);
  return 2;
}

int Disassembler::SubstituteUIntField(const Instruction *instr,
                                      const char *format) {
  VIXL_ASSERT(format[0] == 'u');
  VIXL_ASSERT(strspn(&format[1], "0123456789") == 4);

  // A generic unsigned int field uses a placeholder of the form 'uXXYY where
  // XX and YY are two digit bit positions between 00 and 31, and XX >= YY. The
  // placeholder is substituted with the decimal unsigned integer represented
  // by the bits in the instruction between positions XX and YY inclusive.
  int bitpos = atoi(&format[1]);  // (hibit * 100) + lobit.
  int hibit = bitpos / 100;
  int lobit = bitpos % 100;
  VIXL_ASSERT(hibit < static_cast<int>(kInstructionSize * kBitsPerByte));
  VIXL_ASSERT(lobit <= hibit);
  AppendToOutput("%u", instr->ExtractBits(hibit, lobit));

  return 5;
}

int Disassembler::SubstituteSVESize(const Instruction *instr,
                                    const char *format) {
  USE(format);
  VIXL_ASSERT(format[0] == 't');

  // TODO: only the most common case for <size> is supported at the moment,
  // and even then, the RESERVED values are handled as if they're not reserved.
  static const char sizes[] = {'b', 'h', 's', 'd'};
  unsigned s = instr->GetNEONSize();
  VIXL_ASSERT(s < ArrayLength(sizes));
  AppendToOutput("%c", sizes[s]);

  return 1;
}

void Disassembler::ResetOutput() {
  buffer_pos_ = 0;
  buffer_[buffer_pos_] = 0;
}


void Disassembler::AppendToOutput(const char *format, ...) {
  va_list args;
  va_start(args, format);
  buffer_pos_ += vsnprintf(&buffer_[buffer_pos_],
                           buffer_size_ - buffer_pos_,
                           format,
                           args);
  va_end(args);
}


void PrintDisassembler::Disassemble(const Instruction *instr) {
  Decoder decoder;
  if (cpu_features_auditor_ != NULL) {
    decoder.AppendVisitor(cpu_features_auditor_);
  }
  decoder.AppendVisitor(this);
  decoder.Decode(instr);
}

void PrintDisassembler::DisassembleBuffer(const Instruction *start,
                                          const Instruction *end) {
  Decoder decoder;
  if (cpu_features_auditor_ != NULL) {
    decoder.AppendVisitor(cpu_features_auditor_);
  }
  decoder.AppendVisitor(this);
  decoder.Decode(start, end);
}

void PrintDisassembler::DisassembleBuffer(const Instruction *start,
                                          uint64_t size) {
  DisassembleBuffer(start, start + size);
}


void PrintDisassembler::ProcessOutput(const Instruction *instr) {
  int bytes_printed = fprintf(stream_,
                              "0x%016" PRIx64 "  %08" PRIx32 "\t\t%s",
                              reinterpret_cast<uint64_t>(instr),
                              instr->GetInstructionBits(),
                              GetOutput());
  if (cpu_features_auditor_ != NULL) {
    CPUFeatures needs = cpu_features_auditor_->GetInstructionFeatures();
    needs.Remove(cpu_features_auditor_->GetAvailableFeatures());
    if (needs != CPUFeatures::None()) {
      // Try to align annotations. This value is arbitrary, but based on looking
      // good with most instructions. Note that, for historical reasons, the
      // disassembly itself is printed with tab characters, so bytes_printed is
      // _not_ equivalent to the number of occupied screen columns. However, the
      // prefix before the tabs is always the same length, so the annotation
      // indentation does not change from one line to the next.
      const int indent_to = 70;
      // Always allow some space between the instruction and the annotation.
      const int min_pad = 2;

      int pad = std::max(min_pad, (indent_to - bytes_printed));
      fprintf(stream_, "%*s", pad, "");

      std::stringstream features;
      features << needs;
      fprintf(stream_,
              "%s%s%s",
              cpu_features_prefix_,
              features.str().c_str(),
              cpu_features_suffix_);
    }
  }
  fprintf(stream_, "\n");
}

}  // namespace aarch64
}  // namespace vixl
