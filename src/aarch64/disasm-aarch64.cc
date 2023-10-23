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

#include <bitset>
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


void Disassembler::VisitData(const Instruction *instr) {
  Format(instr, "(data)");
}


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
  const char *mnemonic = "unimplemented";
  const char *form = "(PCRelAddressing)";
  if (GetISA() == ISA::C64) {
    switch (instr->Mask(PCRelAddressingMask)) {
      case ADR:
        mnemonic = "adr";
        form = "'cd, 'AddrPCCRelByte";
        break;
      case ADRP:
        // C64 splits the A64 ADRP encoding into ADRDP and its own ADRP.
        mnemonic = instr->GetImmC64RelP() ? "adrp" : "adrdp";
        form = "'cd, 'AddrC64RelPage";
        break;
    }
  } else {
    switch (instr->Mask(PCRelAddressingMask)) {
      case ADR:
        mnemonic = "adr";
        form = "'Xd, 'AddrPCRelByte";
        break;
      case ADRP:
        mnemonic = "adrp";
        form = "'Xd, 'AddrPCRelPage";
        break;
    }
  }
  Format(instr, mnemonic, form);
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
      if ((instr->GetRn() == kLinkRegCode) && (GetISA() == ISA::A64)) {
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
  const char *form = "'Rt, 'It, 'TImmTest";

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
    form = C ", ['nns'ILSi]!"; \
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
    form = C ", ['nns]'ILSi"; \
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
    form = C ", ['nns'ILU]";       \
    break;
    LOAD_STORE_LIST(LS_UNSIGNEDOFFSET)
#undef LS_UNSIGNEDOFFSET
    case PRFM_unsigned:
      mnemonic = "prfm";
      form = "'prefOp, ['nns'ILU]";
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitLoadStoreRCpcUnscaledOffset(const Instruction *instr) {
  const char *mnemonic;
  const char *form = "'Wt, ['nns'ILS]";
  const char *form_x = "'Xt, ['nns'ILS]";

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
    form = C ", ['nns, 'Offsetreg]"; \
    break;
    LOAD_STORE_LIST(LS_REGISTEROFFSET)
#undef LS_REGISTEROFFSET
    case PRFM_reg:
      mnemonic = "prfm";
      form = "'prefOp, ['nns, 'Offsetreg]";
  }
  Format(instr, mnemonic, form);
}


void Disassembler::VisitLoadStoreUnscaledOffset(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Wt, ['nns'ILS]";
  const char *form_x = "'Xt, ['nns'ILS]";
  const char *form_b = "'Bt, ['nns'ILS]";
  const char *form_h = "'Ht, ['nns'ILS]";
  const char *form_s = "'St, ['nns'ILS]";
  const char *form_d = "'Dt, ['nns'ILS]";
  const char *form_q = "'Qt, ['nns'ILS]";
  const char *form_prefetch = "'prefOp, ['nns'ILS]";

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
    form = C ", ['nns]'ILP" D "i"; \
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
    form = C ", ['nns'ILP" D "i]!"; \
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
    form = C ", ['nns'ILP" D "]"; \
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
      form = "'Wt, 'Wt2, ['nns'ILP2]";
      break;
    case LDNP_w:
      mnemonic = "ldnp";
      form = "'Wt, 'Wt2, ['nns'ILP2]";
      break;
    case STNP_x:
      mnemonic = "stnp";
      form = "'Xt, 'Xt2, ['nns'ILP3]";
      break;
    case LDNP_x:
      mnemonic = "ldnp";
      form = "'Xt, 'Xt2, ['nns'ILP3]";
      break;
    case STNP_s:
      mnemonic = "stnp";
      form = "'St, 'St2, ['nns'ILP2]";
      break;
    case LDNP_s:
      mnemonic = "ldnp";
      form = "'St, 'St2, ['nns'ILP2]";
      break;
    case STNP_d:
      mnemonic = "stnp";
      form = "'Dt, 'Dt2, ['nns'ILP3]";
      break;
    case LDNP_d:
      mnemonic = "ldnp";
      form = "'Dt, 'Dt2, ['nns'ILP3]";
      break;
    case STNP_q:
      mnemonic = "stnp";
      form = "'Qt, 'Qt2, ['nns'ILP4]";
      break;
    case LDNP_q:
      mnemonic = "ldnp";
      form = "'Qt, 'Qt2, ['nns'ILP4]";
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
  V(CASP_w,   "casp",   "'Ws, 'Ws+, 'Wt, 'Wt+")       \
  V(CASP_x,   "casp",   "'Xs, 'Xs+, 'Xt, 'Xt+")       \
  V(CASPA_w,  "caspa",  "'Ws, 'Ws+, 'Wt, 'Wt+")       \
  V(CASPA_x,  "caspa",  "'Xs, 'Xs+, 'Xt, 'Xt+")       \
  V(CASPL_w,  "caspl",  "'Ws, 'Ws+, 'Wt, 'Wt+")       \
  V(CASPL_x,  "caspl",  "'Xs, 'Xs+, 'Xt, 'Xt+")       \
  V(CASPAL_w, "caspal", "'Ws, 'Ws+, 'Wt, 'Wt+")       \
  V(CASPAL_x, "caspal", "'Xs, 'Xs+, 'Xt, 'Xt+")
// clang-format on


void Disassembler::VisitLoadStoreExclusive(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form;

  switch (instr->Mask(LoadStoreExclusiveMask)) {
#define LSX(A, B, C)     \
  case A:                \
    mnemonic = B;        \
    form = C ", ['nns]"; \
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
  const char *form = "'Ws, 'Wt, ['nns]";

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
    form = "'Xs, 'Xt, ['nns]"; \
    break;                     \
  case A##A_x:                 \
    mnemonic = MN "a";         \
    form = "'Xs, 'Xt, ['nns]"; \
    break;                     \
  case A##L_x:                 \
    mnemonic = MN "l";         \
    form = "'Xs, 'Xt, ['nns]"; \
    break;                     \
  case A##AL_x:                \
    mnemonic = MN "al";        \
    form = "'Xs, 'Xt, ['nns]"; \
    break;
    ATOMIC_MEMORY_SIMPLE_LIST(AMS)

    // SWP has the same semantics as ldadd etc but without the store aliases.
    AMS(SWP, "swp")
#undef AMS

    case LDAPRB:
      mnemonic = "ldaprb";
      form = "'Wt, ['nns]";
      break;
    case LDAPRH:
      mnemonic = "ldaprh";
      form = "'Wt, ['nns]";
      break;
    case LDAPR_w:
      mnemonic = "ldapr";
      form = "'Wt, ['nns]";
      break;
    case LDAPR_x:
      mnemonic = "ldapr";
      form = "'Xt, ['nns]";
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
      form = "'Ws, ['nns]";          \
    }                                \
    break;                           \
  }                                  \
  case A##_x:                        \
  case A##L_x: {                     \
    prefix = "ld";                   \
    unsigned rt = instr->GetRt();    \
    if (Register(rt, 64).IsZero()) { \
      prefix = "st";                 \
      form = "'Xs, ['nns]";          \
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
#define FORMAT(A, B) \
  case A##_s:        \
  case A##_d:        \
    mnemonic = B;    \
    break;
    FORMAT(FRINT32X, "frint32x");
    FORMAT(FRINT32Z, "frint32z");
    FORMAT(FRINT64X, "frint64x");
    FORMAT(FRINT64Z, "frint64z");
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
      form = "'Hd, 'IFP";
      break;
    case FMOV_s_imm:
      mnemonic = "fmov";
      form = "'Sd, 'IFP";
      break;
    case FMOV_d_imm:
      mnemonic = "fmov";
      form = "'Dd, 'IFP";
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
      case NEON_FRINT32X:
        mnemonic = "frint32x";
        break;
      case NEON_FRINT32Z:
        mnemonic = "frint32z";
        break;
      case NEON_FRINT64X:
        mnemonic = "frint64x";
        break;
      case NEON_FRINT64Z:
        mnemonic = "frint64z";
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
    form = "'R30d, 'Vn.%s['IVInsIndex1]";
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
  const char *form_1v = "{'Vt.%1$s}, ['nns]";
  const char *form_2v = "{'Vt.%1$s, 'Vt2.%1$s}, ['nns]";
  const char *form_3v = "{'Vt.%1$s, 'Vt2.%1$s, 'Vt3.%1$s}, ['nns]";
  const char *form_4v = "{'Vt.%1$s, 'Vt2.%1$s, 'Vt3.%1$s, 'Vt4.%1$s}, ['nns]";
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
  const char *form_1v = "{'Vt.%1$s}, ['nns], 'Xmr1";
  const char *form_2v = "{'Vt.%1$s, 'Vt2.%1$s}, ['nns], 'Xmr2";
  const char *form_3v = "{'Vt.%1$s, 'Vt2.%1$s, 'Vt3.%1$s}, ['nns], 'Xmr3";
  const char *form_4v =
      "{'Vt.%1$s, 'Vt2.%1$s, 'Vt3.%1$s, 'Vt4.%1$s}, ['nns], 'Xmr4";
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

  const char *form_1b = "{'Vt.b}['IVLSLane0], ['nns]";
  const char *form_1h = "{'Vt.h}['IVLSLane1], ['nns]";
  const char *form_1s = "{'Vt.s}['IVLSLane2], ['nns]";
  const char *form_1d = "{'Vt.d}['IVLSLane3], ['nns]";
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
      form = "{'Vt.%s}, ['nns]";
      break;
    case NEON_LD2_b:
    case NEON_ST2_b:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld2" : "st2";
      form = "{'Vt.b, 'Vt2.b}['IVLSLane0], ['nns]";
      break;
    case NEON_LD2_h:
    case NEON_ST2_h:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld2" : "st2";
      form = "{'Vt.h, 'Vt2.h}['IVLSLane1], ['nns]";
      break;
    case NEON_LD2_s:
    case NEON_ST2_s:
      VIXL_STATIC_ASSERT((NEON_ST2_s | (1 << NEONLSSize_offset)) == NEON_ST2_d);
      VIXL_STATIC_ASSERT((NEON_LD2_s | (1 << NEONLSSize_offset)) == NEON_LD2_d);
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld2" : "st2";
      if ((instr->GetNEONLSSize() & 1) == 0) {
        form = "{'Vt.s, 'Vt2.s}['IVLSLane2], ['nns]";
      } else {
        form = "{'Vt.d, 'Vt2.d}['IVLSLane3], ['nns]";
      }
      break;
    case NEON_LD2R:
      mnemonic = "ld2r";
      form = "{'Vt.%s, 'Vt2.%s}, ['nns]";
      break;
    case NEON_LD3_b:
    case NEON_ST3_b:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld3" : "st3";
      form = "{'Vt.b, 'Vt2.b, 'Vt3.b}['IVLSLane0], ['nns]";
      break;
    case NEON_LD3_h:
    case NEON_ST3_h:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld3" : "st3";
      form = "{'Vt.h, 'Vt2.h, 'Vt3.h}['IVLSLane1], ['nns]";
      break;
    case NEON_LD3_s:
    case NEON_ST3_s:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld3" : "st3";
      if ((instr->GetNEONLSSize() & 1) == 0) {
        form = "{'Vt.s, 'Vt2.s, 'Vt3.s}['IVLSLane2], ['nns]";
      } else {
        form = "{'Vt.d, 'Vt2.d, 'Vt3.d}['IVLSLane3], ['nns]";
      }
      break;
    case NEON_LD3R:
      mnemonic = "ld3r";
      form = "{'Vt.%s, 'Vt2.%s, 'Vt3.%s}, ['nns]";
      break;
    case NEON_LD4_b:
    case NEON_ST4_b:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld4" : "st4";
      form = "{'Vt.b, 'Vt2.b, 'Vt3.b, 'Vt4.b}['IVLSLane0], ['nns]";
      break;
    case NEON_LD4_h:
    case NEON_ST4_h:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld4" : "st4";
      form = "{'Vt.h, 'Vt2.h, 'Vt3.h, 'Vt4.h}['IVLSLane1], ['nns]";
      break;
    case NEON_LD4_s:
    case NEON_ST4_s:
      VIXL_STATIC_ASSERT((NEON_LD4_s | (1 << NEONLSSize_offset)) == NEON_LD4_d);
      VIXL_STATIC_ASSERT((NEON_ST4_s | (1 << NEONLSSize_offset)) == NEON_ST4_d);
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld4" : "st4";
      if ((instr->GetNEONLSSize() & 1) == 0) {
        form = "{'Vt.s, 'Vt2.s, 'Vt3.s, 'Vt4.s}['IVLSLane2], ['nns]";
      } else {
        form = "{'Vt.d, 'Vt2.d, 'Vt3.d, 'Vt4.d}['IVLSLane3], ['nns]";
      }
      break;
    case NEON_LD4R:
      mnemonic = "ld4r";
      form = "{'Vt.%1$s, 'Vt2.%1$s, 'Vt3.%1$s, 'Vt4.%1$s}, ['nns]";
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

  const char *form_1b = "{'Vt.b}['IVLSLane0], ['nns], 'Xmb1";
  const char *form_1h = "{'Vt.h}['IVLSLane1], ['nns], 'Xmb2";
  const char *form_1s = "{'Vt.s}['IVLSLane2], ['nns], 'Xmb4";
  const char *form_1d = "{'Vt.d}['IVLSLane3], ['nns], 'Xmb8";
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
      form = "{'Vt.%s}, ['nns], 'Xmz1";
      break;
    case NEON_LD2_b_post:
    case NEON_ST2_b_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld2" : "st2";
      form = "{'Vt.b, 'Vt2.b}['IVLSLane0], ['nns], 'Xmb2";
      break;
    case NEON_ST2_h_post:
    case NEON_LD2_h_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld2" : "st2";
      form = "{'Vt.h, 'Vt2.h}['IVLSLane1], ['nns], 'Xmb4";
      break;
    case NEON_LD2_s_post:
    case NEON_ST2_s_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld2" : "st2";
      if ((instr->GetNEONLSSize() & 1) == 0)
        form = "{'Vt.s, 'Vt2.s}['IVLSLane2], ['nns], 'Xmb8";
      else
        form = "{'Vt.d, 'Vt2.d}['IVLSLane3], ['nns], 'Xmb16";
      break;
    case NEON_LD2R_post:
      mnemonic = "ld2r";
      form = "{'Vt.%s, 'Vt2.%s}, ['nns], 'Xmz2";
      break;
    case NEON_LD3_b_post:
    case NEON_ST3_b_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld3" : "st3";
      form = "{'Vt.b, 'Vt2.b, 'Vt3.b}['IVLSLane0], ['nns], 'Xmb3";
      break;
    case NEON_LD3_h_post:
    case NEON_ST3_h_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld3" : "st3";
      form = "{'Vt.h, 'Vt2.h, 'Vt3.h}['IVLSLane1], ['nns], 'Xmb6";
      break;
    case NEON_LD3_s_post:
    case NEON_ST3_s_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld3" : "st3";
      if ((instr->GetNEONLSSize() & 1) == 0)
        form = "{'Vt.s, 'Vt2.s, 'Vt3.s}['IVLSLane2], ['nns], 'Xmb12";
      else
        form = "{'Vt.d, 'Vt2.d, 'Vt3.d}['IVLSLane3], ['nns], 'Xmb24";
      break;
    case NEON_LD3R_post:
      mnemonic = "ld3r";
      form = "{'Vt.%s, 'Vt2.%s, 'Vt3.%s}, ['nns], 'Xmz3";
      break;
    case NEON_LD4_b_post:
    case NEON_ST4_b_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld4" : "st4";
      form = "{'Vt.b, 'Vt2.b, 'Vt3.b, 'Vt4.b}['IVLSLane0], ['nns], 'Xmb4";
      break;
    case NEON_LD4_h_post:
    case NEON_ST4_h_post:
      mnemonic = (instr->GetLdStXLoad()) == 1 ? "ld4" : "st4";
      form = "{'Vt.h, 'Vt2.h, 'Vt3.h, 'Vt4.h}['IVLSLane1], ['nns], 'Xmb8";
      break;
    case NEON_LD4_s_post:
    case NEON_ST4_s_post:
      mnemonic = (instr->GetLdStXLoad() == 1) ? "ld4" : "st4";
      if ((instr->GetNEONLSSize() & 1) == 0)
        form = "{'Vt.s, 'Vt2.s, 'Vt3.s, 'Vt4.s}['IVLSLane2], ['nns], 'Xmb16";
      else
        form = "{'Vt.d, 'Vt2.d, 'Vt3.d, 'Vt4.d}['IVLSLane3], ['nns], 'Xmb32";
      break;
    case NEON_LD4R_post:
      mnemonic = "ld4r";
      form = "{'Vt.%1$s, 'Vt2.%1$s, 'Vt3.%1$s, 'Vt4.%1$s}, ['nns], 'Xmz4";
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
          form = "'Vt.%s, 'IFPNeon";
          if (half_enc == 1) {
            nfd.SetFormatMap(0, &map_h);
          } else if (op == 0) {
            nfd.SetFormatMap(0, &map_s);
          } else if (q == 1) {
            form = "'Vt.2d, 'IFPNeon";
          } else {
            mnemonic = "unallocated";
            form = "(NEONModifiedImmediate)";
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

void Disassembler::
    VisitSVE32BitGatherLoadHalfwords_ScalarPlus32BitScaledOffsets(
        const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.s}, 'Pgl/z, ['Xns, 'Zm.s, '?22:suxtw #1]";

  switch (instr->Mask(
      SVE32BitGatherLoadHalfwords_ScalarPlus32BitScaledOffsetsMask)) {
    case LD1H_z_p_bz_s_x32_scaled:
      mnemonic = "ld1h";
      break;
    case LD1SH_z_p_bz_s_x32_scaled:
      mnemonic = "ld1sh";
      break;
    case LDFF1H_z_p_bz_s_x32_scaled:
      mnemonic = "ldff1h";
      break;
    case LDFF1SH_z_p_bz_s_x32_scaled:
      mnemonic = "ldff1sh";
      break;
    default:
      form = "(SVE32BitGatherLoadHalfwords_ScalarPlus32BitScaledOffsets)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE32BitGatherLoadWords_ScalarPlus32BitScaledOffsets(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.s}, 'Pgl/z, ['Xns, 'Zm.s, '?22:suxtw #2]";

  switch (
      instr->Mask(SVE32BitGatherLoadWords_ScalarPlus32BitScaledOffsetsMask)) {
    case LD1W_z_p_bz_s_x32_scaled:
      mnemonic = "ld1w";
      break;
    case LDFF1W_z_p_bz_s_x32_scaled:
      mnemonic = "ldff1w";
      break;
    default:
      form = "(SVE32BitGatherLoadWords_ScalarPlus32BitScaledOffsets)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets(
    const Instruction *instr) {
  const char *form = "{'Zt.s}, 'Pgl/z, ['Xns, 'Zm.s, '?22:suxtw]";

  const char *mnemonic = "unimplemented";
  switch (instr->Mask(SVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsetsMask)) {
    case LD1B_z_p_bz_s_x32_unscaled:
      mnemonic = "ld1b";
      break;
    case LD1H_z_p_bz_s_x32_unscaled:
      mnemonic = "ld1h";
      break;
    case LD1SB_z_p_bz_s_x32_unscaled:
      mnemonic = "ld1sb";
      break;
    case LD1SH_z_p_bz_s_x32_unscaled:
      mnemonic = "ld1sh";
      break;
    case LD1W_z_p_bz_s_x32_unscaled:
      mnemonic = "ld1w";
      break;
    case LDFF1B_z_p_bz_s_x32_unscaled:
      mnemonic = "ldff1b";
      break;
    case LDFF1H_z_p_bz_s_x32_unscaled:
      mnemonic = "ldff1h";
      break;
    case LDFF1SB_z_p_bz_s_x32_unscaled:
      mnemonic = "ldff1sb";
      break;
    case LDFF1SH_z_p_bz_s_x32_unscaled:
      mnemonic = "ldff1sh";
      break;
    case LDFF1W_z_p_bz_s_x32_unscaled:
      mnemonic = "ldff1w";
      break;
    default:
      form = "(SVE32BitGatherLoad_ScalarPlus32BitUnscaledOffsets)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE32BitGatherLoad_VectorPlusImm(
    const Instruction *instr) {
  const char *form = "{'Zt.s}, 'Pgl/z, ['Zn.s]";
  const char *form_imm_b = "{'Zt.s}, 'Pgl/z, ['Zn.s, #'u2016]";
  const char *form_imm_h = "{'Zt.s}, 'Pgl/z, ['Zn.s, #'u2016*2]";
  const char *form_imm_w = "{'Zt.s}, 'Pgl/z, ['Zn.s, #'u2016*4]";
  const char *form_imm;

  const char *mnemonic = "unimplemented";
  switch (instr->Mask(SVE32BitGatherLoad_VectorPlusImmMask)) {
    case LD1B_z_p_ai_s:
      mnemonic = "ld1b";
      form_imm = form_imm_b;
      break;
    case LD1H_z_p_ai_s:
      mnemonic = "ld1h";
      form_imm = form_imm_h;
      break;
    case LD1SB_z_p_ai_s:
      mnemonic = "ld1sb";
      form_imm = form_imm_b;
      break;
    case LD1SH_z_p_ai_s:
      mnemonic = "ld1sh";
      form_imm = form_imm_h;
      break;
    case LD1W_z_p_ai_s:
      mnemonic = "ld1w";
      form_imm = form_imm_w;
      break;
    case LDFF1B_z_p_ai_s:
      mnemonic = "ldff1b";
      form_imm = form_imm_b;
      break;
    case LDFF1H_z_p_ai_s:
      mnemonic = "ldff1h";
      form_imm = form_imm_h;
      break;
    case LDFF1SB_z_p_ai_s:
      mnemonic = "ldff1sb";
      form_imm = form_imm_b;
      break;
    case LDFF1SH_z_p_ai_s:
      mnemonic = "ldff1sh";
      form_imm = form_imm_h;
      break;
    case LDFF1W_z_p_ai_s:
      mnemonic = "ldff1w";
      form_imm = form_imm_w;
      break;
    default:
      form = "(SVE32BitGatherLoad_VectorPlusImm)";
      form_imm = form;
      break;
  }
  if (instr->ExtractBits(20, 16) != 0) form = form_imm;

  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE32BitGatherPrefetch_ScalarPlus32BitScaledOffsets(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'prefSVEOp, 'Pgl, ['Xns, 'Zm.s, '?22:suxtw";
  const char *suffix = NULL;

  switch (
      instr->Mask(SVE32BitGatherPrefetch_ScalarPlus32BitScaledOffsetsMask)) {
    case PRFB_i_p_bz_s_x32_scaled:
      mnemonic = "prfb";
      suffix = "]";
      break;
    case PRFD_i_p_bz_s_x32_scaled:
      mnemonic = "prfd";
      suffix = " #3]";
      break;
    case PRFH_i_p_bz_s_x32_scaled:
      mnemonic = "prfh";
      suffix = " #1]";
      break;
    case PRFW_i_p_bz_s_x32_scaled:
      mnemonic = "prfw";
      suffix = " #2]";
      break;
    default:
      form = "(SVE32BitGatherPrefetch_ScalarPlus32BitScaledOffsets)";
      break;
  }
  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVE32BitGatherPrefetch_VectorPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = (instr->ExtractBits(20, 16) != 0)
                         ? "'prefSVEOp, 'Pgl, ['Zn.s, #'u2016]"
                         : "'prefSVEOp, 'Pgl, ['Zn.s]";

  switch (instr->Mask(SVE32BitGatherPrefetch_VectorPlusImmMask)) {
    case PRFB_i_p_ai_s:
      mnemonic = "prfb";
      break;
    case PRFD_i_p_ai_s:
      mnemonic = "prfd";
      break;
    case PRFH_i_p_ai_s:
      mnemonic = "prfh";
      break;
    case PRFW_i_p_ai_s:
      mnemonic = "prfw";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE32BitScatterStore_ScalarPlus32BitScaledOffsets(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.s}, 'Pgl, ['Xns, 'Zm.s, '?14:suxtw #'u2423]";

  switch (instr->Mask(SVE32BitScatterStore_ScalarPlus32BitScaledOffsetsMask)) {
    case ST1H_z_p_bz_s_x32_scaled:
      mnemonic = "st1h";
      break;
    case ST1W_z_p_bz_s_x32_scaled:
      mnemonic = "st1w";
      break;
    default:
      form = "(SVE32BitScatterStore_ScalarPlus32BitScaledOffsets)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE32BitScatterStore_ScalarPlus32BitUnscaledOffsets(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.s}, 'Pgl, ['Xns, 'Zm.s, '?14:suxtw]";

  switch (
      instr->Mask(SVE32BitScatterStore_ScalarPlus32BitUnscaledOffsetsMask)) {
    case ST1B_z_p_bz_s_x32_unscaled:
      mnemonic = "st1b";
      break;
    case ST1H_z_p_bz_s_x32_unscaled:
      mnemonic = "st1h";
      break;
    case ST1W_z_p_bz_s_x32_unscaled:
      mnemonic = "st1w";
      break;
    default:
      form = "(SVE32BitScatterStore_ScalarPlus32BitUnscaledOffsets)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE32BitScatterStore_VectorPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.s}, 'Pgl, ['Zn.s";
  const char *suffix = NULL;

  bool is_zero = instr->ExtractBits(20, 16) == 0;

  switch (instr->Mask(SVE32BitScatterStore_VectorPlusImmMask)) {
    case ST1B_z_p_ai_s:
      mnemonic = "st1b";
      suffix = is_zero ? "]" : ", #'u2016]";
      break;
    case ST1H_z_p_ai_s:
      mnemonic = "st1h";
      suffix = is_zero ? "]" : ", #'u2016*2]";
      break;
    case ST1W_z_p_ai_s:
      mnemonic = "st1w";
      suffix = is_zero ? "]" : ", #'u2016*4]";
      break;
    default:
      form = "(SVE32BitScatterStore_VectorPlusImm)";
      break;
  }
  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsets(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.d}, 'Pgl/z, ['Xns, 'Zm.d, '?22:suxtw #'u2423]";

  switch (instr->Mask(
      SVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsetsMask)) {
    case LD1D_z_p_bz_d_x32_scaled:
      mnemonic = "ld1d";
      break;
    case LD1H_z_p_bz_d_x32_scaled:
      mnemonic = "ld1h";
      break;
    case LD1SH_z_p_bz_d_x32_scaled:
      mnemonic = "ld1sh";
      break;
    case LD1SW_z_p_bz_d_x32_scaled:
      mnemonic = "ld1sw";
      break;
    case LD1W_z_p_bz_d_x32_scaled:
      mnemonic = "ld1w";
      break;
    case LDFF1D_z_p_bz_d_x32_scaled:
      mnemonic = "ldff1d";
      break;
    case LDFF1H_z_p_bz_d_x32_scaled:
      mnemonic = "ldff1h";
      break;
    case LDFF1SH_z_p_bz_d_x32_scaled:
      mnemonic = "ldff1sh";
      break;
    case LDFF1SW_z_p_bz_d_x32_scaled:
      mnemonic = "ldff1sw";
      break;
    case LDFF1W_z_p_bz_d_x32_scaled:
      mnemonic = "ldff1w";
      break;
    default:
      form = "(SVE64BitGatherLoad_ScalarPlus32BitUnpackedScaledOffsetsMask)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE64BitGatherLoad_ScalarPlus64BitScaledOffsets(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.d}, 'Pgl/z, ['Xns, 'Zm.d, lsl #'u2423]";

  switch (instr->Mask(SVE64BitGatherLoad_ScalarPlus64BitScaledOffsetsMask)) {
    case LD1D_z_p_bz_d_64_scaled:
      mnemonic = "ld1d";
      break;
    case LD1H_z_p_bz_d_64_scaled:
      mnemonic = "ld1h";
      break;
    case LD1SH_z_p_bz_d_64_scaled:
      mnemonic = "ld1sh";
      break;
    case LD1SW_z_p_bz_d_64_scaled:
      mnemonic = "ld1sw";
      break;
    case LD1W_z_p_bz_d_64_scaled:
      mnemonic = "ld1w";
      break;
    case LDFF1D_z_p_bz_d_64_scaled:
      mnemonic = "ldff1d";
      break;
    case LDFF1H_z_p_bz_d_64_scaled:
      mnemonic = "ldff1h";
      break;
    case LDFF1SH_z_p_bz_d_64_scaled:
      mnemonic = "ldff1sh";
      break;
    case LDFF1SW_z_p_bz_d_64_scaled:
      mnemonic = "ldff1sw";
      break;
    case LDFF1W_z_p_bz_d_64_scaled:
      mnemonic = "ldff1w";
      break;
    default:
      form = "(SVE64BitGatherLoad_ScalarPlus64BitScaledOffsetsMask)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsets(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.d}, 'Pgl/z, ['Xns, 'Zm.d]";

  switch (instr->Mask(SVE64BitGatherLoad_ScalarPlus64BitUnscaledOffsetsMask)) {
    case LD1B_z_p_bz_d_64_unscaled:
      mnemonic = "ld1b";
      break;
    case LD1D_z_p_bz_d_64_unscaled:
      mnemonic = "ld1d";
      break;
    case LD1H_z_p_bz_d_64_unscaled:
      mnemonic = "ld1h";
      break;
    case LD1SB_z_p_bz_d_64_unscaled:
      mnemonic = "ld1sb";
      break;
    case LD1SH_z_p_bz_d_64_unscaled:
      mnemonic = "ld1sh";
      break;
    case LD1SW_z_p_bz_d_64_unscaled:
      mnemonic = "ld1sw";
      break;
    case LD1W_z_p_bz_d_64_unscaled:
      mnemonic = "ld1w";
      break;
    case LDFF1B_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1b";
      break;
    case LDFF1D_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1d";
      break;
    case LDFF1H_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1h";
      break;
    case LDFF1SB_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1sb";
      break;
    case LDFF1SH_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1sh";
      break;
    case LDFF1SW_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1sw";
      break;
    case LDFF1W_z_p_bz_d_64_unscaled:
      mnemonic = "ldff1w";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::
    VisitSVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsets(
        const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.d}, 'Pgl/z, ['Xns, 'Zm.d, '?22:suxtw]";

  switch (instr->Mask(
      SVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsetsMask)) {
    case LD1B_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1b";
      break;
    case LD1D_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1d";
      break;
    case LD1H_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1h";
      break;
    case LD1SB_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1sb";
      break;
    case LD1SH_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1sh";
      break;
    case LD1SW_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1sw";
      break;
    case LD1W_z_p_bz_d_x32_unscaled:
      mnemonic = "ld1w";
      break;
    case LDFF1B_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1b";
      break;
    case LDFF1D_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1d";
      break;
    case LDFF1H_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1h";
      break;
    case LDFF1SB_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1sb";
      break;
    case LDFF1SH_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1sh";
      break;
    case LDFF1SW_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1sw";
      break;
    case LDFF1W_z_p_bz_d_x32_unscaled:
      mnemonic = "ldff1w";
      break;
    default:
      form = "(SVE64BitGatherLoad_ScalarPlusUnpacked32BitUnscaledOffsetsMask)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE64BitGatherLoad_VectorPlusImm(
    const Instruction *instr) {
  const char *form = "{'Zt.d}, 'Pgl/z, ['Zn.d]";
  const char *form_imm[4] = {"{'Zt.d}, 'Pgl/z, ['Zn.d, #'u2016]",
                             "{'Zt.d}, 'Pgl/z, ['Zn.d, #'u2016*2]",
                             "{'Zt.d}, 'Pgl/z, ['Zn.d, #'u2016*4]",
                             "{'Zt.d}, 'Pgl/z, ['Zn.d, #'u2016*8]"};

  if (instr->ExtractBits(20, 16) != 0) {
    unsigned msz = instr->ExtractBits(24, 23);
    bool sign_extend = instr->ExtractBit(14) == 0;
    if ((msz == kDRegSizeInBytesLog2) && sign_extend) {
      form = "(SVE64BitGatherLoad_VectorPlusImm)";
    } else {
      VIXL_ASSERT(msz < ArrayLength(form_imm));
      form = form_imm[msz];
    }
  }

  const char *mnemonic = "unimplemented";
  switch (instr->Mask(SVE64BitGatherLoad_VectorPlusImmMask)) {
    case LD1B_z_p_ai_d:
      mnemonic = "ld1b";
      break;
    case LD1D_z_p_ai_d:
      mnemonic = "ld1d";
      break;
    case LD1H_z_p_ai_d:
      mnemonic = "ld1h";
      break;
    case LD1SB_z_p_ai_d:
      mnemonic = "ld1sb";
      break;
    case LD1SH_z_p_ai_d:
      mnemonic = "ld1sh";
      break;
    case LD1SW_z_p_ai_d:
      mnemonic = "ld1sw";
      break;
    case LD1W_z_p_ai_d:
      mnemonic = "ld1w";
      break;
    case LDFF1B_z_p_ai_d:
      mnemonic = "ldff1b";
      break;
    case LDFF1D_z_p_ai_d:
      mnemonic = "ldff1d";
      break;
    case LDFF1H_z_p_ai_d:
      mnemonic = "ldff1h";
      break;
    case LDFF1SB_z_p_ai_d:
      mnemonic = "ldff1sb";
      break;
    case LDFF1SH_z_p_ai_d:
      mnemonic = "ldff1sh";
      break;
    case LDFF1SW_z_p_ai_d:
      mnemonic = "ldff1sw";
      break;
    case LDFF1W_z_p_ai_d:
      mnemonic = "ldff1w";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE64BitGatherPrefetch_ScalarPlus64BitScaledOffsets(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVE64BitGatherPrefetch_ScalarPlus64BitScaledOffsets)";

  switch (
      instr->Mask(SVE64BitGatherPrefetch_ScalarPlus64BitScaledOffsetsMask)) {
    case PRFB_i_p_bz_d_64_scaled:
      mnemonic = "prfb";
      form = "'prefSVEOp, 'Pgl, ['Xns, 'Zm.d]";
      break;
    case PRFD_i_p_bz_d_64_scaled:
      mnemonic = "prfd";
      form = "'prefSVEOp, 'Pgl, ['Xns, 'Zm.d, lsl #3]";
      break;
    case PRFH_i_p_bz_d_64_scaled:
      mnemonic = "prfh";
      form = "'prefSVEOp, 'Pgl, ['Xns, 'Zm.d, lsl #1]";
      break;
    case PRFW_i_p_bz_d_64_scaled:
      mnemonic = "prfw";
      form = "'prefSVEOp, 'Pgl, ['Xns, 'Zm.d, lsl #2]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::
    VisitSVE64BitGatherPrefetch_ScalarPlusUnpacked32BitScaledOffsets(
        const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'prefSVEOp, 'Pgl, ['Xns, 'Zm.d, '?22:suxtw";
  const char *suffix = NULL;

  switch (instr->Mask(
      SVE64BitGatherPrefetch_ScalarPlusUnpacked32BitScaledOffsetsMask)) {
    case PRFB_i_p_bz_d_x32_scaled:
      mnemonic = "prfb";
      suffix = " ]";
      break;
    case PRFD_i_p_bz_d_x32_scaled:
      mnemonic = "prfd";
      suffix = " #3]";
      break;
    case PRFH_i_p_bz_d_x32_scaled:
      mnemonic = "prfh";
      suffix = " #1]";
      break;
    case PRFW_i_p_bz_d_x32_scaled:
      mnemonic = "prfw";
      suffix = " #2]";
      break;
    default:
      form = "(SVE64BitGatherPrefetch_ScalarPlusUnpacked32BitScaledOffsets)";
      break;
  }
  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVE64BitGatherPrefetch_VectorPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = (instr->ExtractBits(20, 16) != 0)
                         ? "'prefSVEOp, 'Pgl, ['Zn.d, #'u2016]"
                         : "'prefSVEOp, 'Pgl, ['Zn.d]";

  switch (instr->Mask(SVE64BitGatherPrefetch_VectorPlusImmMask)) {
    case PRFB_i_p_ai_d:
      mnemonic = "prfb";
      break;
    case PRFD_i_p_ai_d:
      mnemonic = "prfd";
      break;
    case PRFH_i_p_ai_d:
      mnemonic = "prfh";
      break;
    case PRFW_i_p_ai_d:
      mnemonic = "prfw";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE64BitScatterStore_ScalarPlus64BitScaledOffsets(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.d}, 'Pgl, ['Xns, 'Zm.d, lsl #'u2423]";

  switch (instr->Mask(SVE64BitScatterStore_ScalarPlus64BitScaledOffsetsMask)) {
    case ST1D_z_p_bz_d_64_scaled:
      mnemonic = "st1d";
      break;
    case ST1H_z_p_bz_d_64_scaled:
      mnemonic = "st1h";
      break;
    case ST1W_z_p_bz_d_64_scaled:
      mnemonic = "st1w";
      break;
    default:
      form = "(SVE64BitScatterStore_ScalarPlus64BitScaledOffsets)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE64BitScatterStore_ScalarPlus64BitUnscaledOffsets(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.d}, 'Pgl, ['Xns, 'Zm.d]";

  switch (
      instr->Mask(SVE64BitScatterStore_ScalarPlus64BitUnscaledOffsetsMask)) {
    case ST1B_z_p_bz_d_64_unscaled:
      mnemonic = "st1b";
      break;
    case ST1D_z_p_bz_d_64_unscaled:
      mnemonic = "st1d";
      break;
    case ST1H_z_p_bz_d_64_unscaled:
      mnemonic = "st1h";
      break;
    case ST1W_z_p_bz_d_64_unscaled:
      mnemonic = "st1w";
      break;
    default:
      form = "(SVE64BitScatterStore_ScalarPlus64BitUnscaledOffset)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::
    VisitSVE64BitScatterStore_ScalarPlusUnpacked32BitScaledOffsets(
        const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.d}, 'Pgl, ['Xns, 'Zm.d, '?14:suxtw #'u2423]";

  switch (instr->Mask(
      SVE64BitScatterStore_ScalarPlusUnpacked32BitScaledOffsetsMask)) {
    case ST1D_z_p_bz_d_x32_scaled:
      mnemonic = "st1d";
      break;
    case ST1H_z_p_bz_d_x32_scaled:
      mnemonic = "st1h";
      break;
    case ST1W_z_p_bz_d_x32_scaled:
      mnemonic = "st1w";
      break;
    default:
      form = "(SVE64BitScatterStore_ScalarPlusUnpacked32BitScaledOffsets)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::
    VisitSVE64BitScatterStore_ScalarPlusUnpacked32BitUnscaledOffsets(
        const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.d}, 'Pgl, ['Xns, 'Zm.d, '?14:suxtw]";

  switch (instr->Mask(
      SVE64BitScatterStore_ScalarPlusUnpacked32BitUnscaledOffsetsMask)) {
    case ST1B_z_p_bz_d_x32_unscaled:
      mnemonic = "st1b";
      break;
    case ST1D_z_p_bz_d_x32_unscaled:
      mnemonic = "st1d";
      break;
    case ST1H_z_p_bz_d_x32_unscaled:
      mnemonic = "st1h";
      break;
    case ST1W_z_p_bz_d_x32_unscaled:
      mnemonic = "st1w";
      break;
    default:
      form = "(SVE64BitScatterStore_ScalarPlusUnpacked32BitUnscaledOffsets)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVE64BitScatterStore_VectorPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.d}, 'Pgl, ['Zn.d";
  const char *suffix = NULL;

  bool is_zero = instr->ExtractBits(20, 16) == 0;

  switch (instr->Mask(SVE64BitScatterStore_VectorPlusImmMask)) {
    case ST1B_z_p_ai_d:
      mnemonic = "st1b";
      suffix = is_zero ? "]" : ", #'u2016]";
      break;
    case ST1D_z_p_ai_d:
      mnemonic = "st1d";
      suffix = is_zero ? "]" : ", #'u2016*8]";
      break;
    case ST1H_z_p_ai_d:
      mnemonic = "st1h";
      suffix = is_zero ? "]" : ", #'u2016*2]";
      break;
    case ST1W_z_p_ai_d:
      mnemonic = "st1w";
      suffix = is_zero ? "]" : ", #'u2016*4]";
      break;
    default:
      form = "(SVE64BitScatterStore_VectorPlusImm)";
      break;
  }
  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVEBitwiseLogicalWithImm_Unpredicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'tl, 'Zd.'tl, 'ITriSvel";

  if (instr->GetSVEImmLogical() == 0) {
    // The immediate encoded in the instruction is not in the expected format.
    Format(instr, "unallocated", "(SVEBitwiseImm)");
    return;
  }

  switch (instr->Mask(SVEBitwiseLogicalWithImm_UnpredicatedMask)) {
    case AND_z_zi:
      mnemonic = "and";
      break;
    case EOR_z_zi:
      mnemonic = "eor";
      break;
    case ORR_z_zi:
      mnemonic = "orr";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBitwiseLogical_Predicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEBitwiseLogical_PredicatedMask)) {
    case AND_z_p_zz:
      mnemonic = "and";
      break;
    case BIC_z_p_zz:
      mnemonic = "bic";
      break;
    case EOR_z_p_zz:
      mnemonic = "eor";
      break;
    case ORR_z_p_zz:
      mnemonic = "orr";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBitwiseShiftByImm_Predicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'tszp, 'Pgl/m, 'Zd.'tszp, 'ITriSveq";
  unsigned tsize = (instr->ExtractBits(23, 22) << 2) | instr->ExtractBits(9, 8);

  if (tsize == 0) {
    form = "(SVEBitwiseShiftByImm_Predicated)";
  } else {
    switch (instr->Mask(SVEBitwiseShiftByImm_PredicatedMask)) {
      case ASRD_z_p_zi:
        mnemonic = "asrd";
        break;
      case ASR_z_p_zi:
        mnemonic = "asr";
        break;
      case LSL_z_p_zi:
        mnemonic = "lsl";
        form = "'Zd.'tszp, p'u1210/m, 'Zd.'tszp, 'ITriSvep";
        break;
      case LSR_z_p_zi:
        mnemonic = "lsr";
        break;
      default:
        break;
    }
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBitwiseShiftByVector_Predicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEBitwiseShiftByVector_PredicatedMask)) {
    case ASRR_z_p_zz:
      mnemonic = "asrr";
      break;
    case ASR_z_p_zz:
      mnemonic = "asr";
      break;
    case LSLR_z_p_zz:
      mnemonic = "lslr";
      break;
    case LSL_z_p_zz:
      mnemonic = "lsl";
      break;
    case LSRR_z_p_zz:
      mnemonic = "lsrr";
      break;
    case LSR_z_p_zz:
      mnemonic = "lsr";
      break;
    default:
      form = "(SVEBitwiseShiftByVector_Predicated)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBitwiseShiftByWideElements_Predicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zd.'t, 'Zn.d";

  if (instr->GetSVESize() == kDRegSizeInBytesLog2) {
    form = "(SVEBitwiseShiftByWideElements_Predicated)";
  } else {
    switch (instr->Mask(SVEBitwiseShiftByWideElements_PredicatedMask)) {
      case ASR_z_p_zw:
        mnemonic = "asr";
        break;
      case LSL_z_p_zw:
        mnemonic = "lsl";
        break;
      case LSR_z_p_zw:
        mnemonic = "lsr";
        break;
      default:
        form = "(SVEBitwiseShiftByWideElements_Predicated)";
        break;
    }
  }
  Format(instr, mnemonic, form);
}

static bool SVEMoveMaskPreferred(uint64_t value, int lane_bytes_log2) {
  VIXL_ASSERT(IsUintN(8 << lane_bytes_log2, value));

  // Duplicate lane-sized value across double word.
  switch (lane_bytes_log2) {
    case 0:
      value *= 0x0101010101010101;
      break;
    case 1:
      value *= 0x0001000100010001;
      break;
    case 2:
      value *= 0x0000000100000001;
      break;
    case 3:  // Nothing to do
      break;
    default:
      VIXL_UNREACHABLE();
  }

  if ((value & 0xff) == 0) {
    // Check for 16-bit patterns. Set least-significant 16 bits, to make tests
    // easier; we already checked least-significant byte is zero above.
    uint64_t generic_value = value | 0xffff;

    // Check 0x00000000_0000pq00 or 0xffffffff_ffffpq00.
    if ((generic_value == 0xffff) || (generic_value == UINT64_MAX)) {
      return false;
    }

    // Check 0x0000pq00_0000pq00 or 0xffffpq00_ffffpq00.
    uint64_t rotvalue = RotateRight(value, 32, 64);
    if (value == rotvalue) {
      generic_value &= 0xffffffff;
      if ((generic_value == 0xffff) || (generic_value == UINT32_MAX)) {
        return false;
      }
    }

    // Check 0xpq00pq00_pq00pq00.
    rotvalue = RotateRight(value, 16, 64);
    if (value == rotvalue) {
      return false;
    }
  } else {
    // Check for 8-bit patterns. Set least-significant byte, to make tests
    // easier.
    uint64_t generic_value = value | 0xff;

    // Check 0x00000000_000000pq or 0xffffffff_ffffffpq.
    if ((generic_value == 0xff) || (generic_value == UINT64_MAX)) {
      return false;
    }

    // Check 0x000000pq_000000pq or 0xffffffpq_ffffffpq.
    uint64_t rotvalue = RotateRight(value, 32, 64);
    if (value == rotvalue) {
      generic_value &= 0xffffffff;
      if ((generic_value == 0xff) || (generic_value == UINT32_MAX)) {
        return false;
      }
    }

    // Check 0x00pq00pq_00pq00pq or 0xffpqffpq_ffpqffpq.
    rotvalue = RotateRight(value, 16, 64);
    if (value == rotvalue) {
      generic_value &= 0xffff;
      if ((generic_value == 0xff) || (generic_value == UINT16_MAX)) {
        return false;
      }
    }

    // Check 0xpqpqpqpq_pqpqpqpq.
    rotvalue = RotateRight(value, 8, 64);
    if (value == rotvalue) {
      return false;
    }
  }
  return true;
}

void Disassembler::VisitSVEBroadcastBitmaskImm(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEBroadcastBitmaskImm)";

  switch (instr->Mask(SVEBroadcastBitmaskImmMask)) {
    case DUPM_z_i: {
      uint64_t imm = instr->GetSVEImmLogical();
      if (imm != 0) {
        int lane_size = instr->GetSVEBitwiseImmLaneSizeInBytesLog2();
        mnemonic = SVEMoveMaskPreferred(imm, lane_size) ? "mov" : "dupm";
        form = "'Zd.'tl, 'ITriSvel";
      }
      break;
    }
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBroadcastFPImm_Unpredicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEBroadcastFPImm_Unpredicated)";

  switch (instr->Mask(SVEBroadcastFPImm_UnpredicatedMask)) {
    case FDUP_z_i:
      // The preferred disassembly for fdup is "fmov".
      mnemonic = "fmov";
      form = "'Zd.'t, 'IFPSve";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBroadcastGeneralRegister(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEBroadcastGeneralRegister)";

  switch (instr->Mask(SVEBroadcastGeneralRegisterMask)) {
    case DUP_z_r:
      // The preferred disassembly for dup is "mov".
      mnemonic = "mov";
      if (instr->GetSVESize() == kDRegSizeInBytesLog2) {
        form = "'Zd.'t, 'Xns";
      } else {
        form = "'Zd.'t, 'Wns";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBroadcastIndexElement(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEBroadcastIndexElement)";

  switch (instr->Mask(SVEBroadcastIndexElementMask)) {
    case DUP_z_zi: {
      // The tsz field must not be zero.
      int tsz = instr->ExtractBits(20, 16);
      if (tsz != 0) {
        // The preferred disassembly for dup is "mov".
        mnemonic = "mov";
        int imm2 = instr->ExtractBits(23, 22);
        if ((CountSetBits(imm2) + CountSetBits(tsz)) == 1) {
          // If imm2:tsz has one set bit, the index is zero. This is
          // disassembled as a mov from a b/h/s/d/q scalar register.
          form = "'Zd.'tszx, 'tszx'u0905";
        } else {
          form = "'Zd.'tszx, 'Zn.'tszx['IVInsSVEIndex]";
        }
      }
      break;
    }
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBroadcastIntImm_Unpredicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEBroadcastIntImm_Unpredicated)";

  switch (instr->Mask(SVEBroadcastIntImm_UnpredicatedMask)) {
    case DUP_z_i:
      // The encoding of byte-sized lanes with lsl #8 is undefined.
      if ((instr->GetSVEVectorFormat() == kFormatVnB) &&
          (instr->ExtractBit(13) == 1))
        break;

      // The preferred disassembly for dup is "mov".
      mnemonic = "mov";
      form = (instr->ExtractBit(13) == 0) ? "'Zd.'t, #'s1205"
                                          : "'Zd.'t, #'s1205, lsl #8";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVECompressActiveElements(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVECompressActiveElements)";

  switch (instr->Mask(SVECompressActiveElementsMask)) {
    case COMPACT_z_p_z:
      // The top bit of size is always set for compact, so 't can only be
      // substituted with types S and D.
      VIXL_ASSERT(instr->ExtractBit(23) == 1);
      mnemonic = "compact";
      form = "'Zd.'t, 'Pgl, 'Zn.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEConditionallyBroadcastElementToVector(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl, 'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEConditionallyBroadcastElementToVectorMask)) {
    case CLASTA_z_p_zz:
      mnemonic = "clasta";
      break;
    case CLASTB_z_p_zz:
      mnemonic = "clastb";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEConditionallyExtractElementToGeneralRegister(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Wd, 'Pgl, 'Wd, 'Zn.'t";

  if (instr->GetSVESize() == kDRegSizeInBytesLog2) {
    form = "'Xd, p'u1210, 'Xd, 'Zn.'t";
  }

  switch (instr->Mask(SVEConditionallyExtractElementToGeneralRegisterMask)) {
    case CLASTA_r_p_z:
      mnemonic = "clasta";
      break;
    case CLASTB_r_p_z:
      mnemonic = "clastb";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEConditionallyExtractElementToSIMDFPScalar(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'t'u0400, 'Pgl, 't'u0400, 'Zn.'t";

  switch (instr->Mask(SVEConditionallyExtractElementToSIMDFPScalarMask)) {
    case CLASTA_v_p_z:
      mnemonic = "clasta";
      break;
    case CLASTB_v_p_z:
      mnemonic = "clastb";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEConditionallyTerminateScalars(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = (instr->ExtractBit(22) == 0) ? "'Wn, 'Wm" : "'Xn, 'Xm";

  switch (instr->Mask(SVEConditionallyTerminateScalarsMask)) {
    case CTERMEQ_rr:
      mnemonic = "ctermeq";
      break;
    case CTERMNE_rr:
      mnemonic = "ctermne";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEConstructivePrefix_Unpredicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEConstructivePrefix_Unpredicated)";

  switch (instr->Mask(SVEConstructivePrefix_UnpredicatedMask)) {
    case MOVPRFX_z_z:
      mnemonic = "movprfx";
      form = "'Zd, 'Zn";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEContiguousFirstFaultLoad_ScalarPlusScalar(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";

  bool rm_is_zr = instr->GetRm() == kZeroRegCode;

  const char *form = "{'Zt.'tlss}, 'Pgl/z, ['Xns";
  const char *suffix = NULL;

  switch (instr->Mask(SVEContiguousFirstFaultLoad_ScalarPlusScalarMask)) {
    case LDFF1B_z_p_br_u16:
    case LDFF1B_z_p_br_u32:
    case LDFF1B_z_p_br_u64:
    case LDFF1B_z_p_br_u8:
      mnemonic = "ldff1b";
      suffix = rm_is_zr ? "]" : ", 'Xm]";
      break;
    case LDFF1D_z_p_br_u64:
      mnemonic = "ldff1d";
      suffix = rm_is_zr ? "]" : ", 'Xm, lsl #3]";
      break;
    case LDFF1H_z_p_br_u16:
    case LDFF1H_z_p_br_u32:
    case LDFF1H_z_p_br_u64:
      mnemonic = "ldff1h";
      suffix = rm_is_zr ? "]" : ", 'Xm, lsl #1]";
      break;
    case LDFF1SB_z_p_br_s16:
    case LDFF1SB_z_p_br_s32:
    case LDFF1SB_z_p_br_s64:
      mnemonic = "ldff1sb";
      suffix = rm_is_zr ? "]" : ", 'Xm]";
      break;
    case LDFF1SH_z_p_br_s32:
    case LDFF1SH_z_p_br_s64:
      mnemonic = "ldff1sh";
      suffix = rm_is_zr ? "]" : ", 'Xm, lsl #1]";
      break;
    case LDFF1SW_z_p_br_s64:
      mnemonic = "ldff1sw";
      suffix = rm_is_zr ? "]" : ", 'Xm, lsl #2]";
      break;
    case LDFF1W_z_p_br_u32:
    case LDFF1W_z_p_br_u64:
      mnemonic = "ldff1w";
      suffix = rm_is_zr ? "]" : ", 'Xm, lsl #2]";
      break;
    default:
      form = "(SVEContiguousFirstFaultLoad_ScalarPlusScalar)";
      break;
  }

  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVEContiguousNonFaultLoad_ScalarPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.'tlss}, 'Pgl/z, ['Xns";
  const char *suffix =
      (instr->ExtractBits(19, 16) == 0) ? "]" : ", #'s1916, mul vl]";

  switch (instr->Mask(SVEContiguousNonFaultLoad_ScalarPlusImmMask)) {
    case LDNF1B_z_p_bi_u16:
    case LDNF1B_z_p_bi_u32:
    case LDNF1B_z_p_bi_u64:
    case LDNF1B_z_p_bi_u8:
      mnemonic = "ldnf1b";
      break;
    case LDNF1D_z_p_bi_u64:
      mnemonic = "ldnf1d";
      break;
    case LDNF1H_z_p_bi_u16:
    case LDNF1H_z_p_bi_u32:
    case LDNF1H_z_p_bi_u64:
      mnemonic = "ldnf1h";
      break;
    case LDNF1SB_z_p_bi_s16:
    case LDNF1SB_z_p_bi_s32:
    case LDNF1SB_z_p_bi_s64:
      mnemonic = "ldnf1sb";
      break;
    case LDNF1SH_z_p_bi_s32:
    case LDNF1SH_z_p_bi_s64:
      mnemonic = "ldnf1sh";
      break;
    case LDNF1SW_z_p_bi_s64:
      mnemonic = "ldnf1sw";
      break;
    case LDNF1W_z_p_bi_u32:
    case LDNF1W_z_p_bi_u64:
      mnemonic = "ldnf1w";
      break;
    default:
      form = "(SVEContiguousNonFaultLoad_ScalarPlusImm)";
      suffix = NULL;
      break;
  }
  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVEContiguousNonTemporalLoad_ScalarPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEContiguousNonTemporalLoad_ScalarPlusImm)";

  const char *suffix =
      (instr->ExtractBits(19, 16) == 0) ? "]" : ", #'s1916, mul vl]";
  switch (instr->Mask(SVEContiguousNonTemporalLoad_ScalarPlusImmMask)) {
    case LDNT1B_z_p_bi_contiguous:
      mnemonic = "ldnt1b";
      form = "{'Zt.b}, 'Pgl/z, ['Xns";
      break;
    case LDNT1D_z_p_bi_contiguous:
      mnemonic = "ldnt1d";
      form = "{'Zt.d}, 'Pgl/z, ['Xns";
      break;
    case LDNT1H_z_p_bi_contiguous:
      mnemonic = "ldnt1h";
      form = "{'Zt.h}, 'Pgl/z, ['Xns";
      break;
    case LDNT1W_z_p_bi_contiguous:
      mnemonic = "ldnt1w";
      form = "{'Zt.s}, 'Pgl/z, ['Xns";
      break;
    default:
      suffix = NULL;
      break;
  }
  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVEContiguousNonTemporalLoad_ScalarPlusScalar(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEContiguousNonTemporalLoad_ScalarPlusScalar)";

  switch (instr->Mask(SVEContiguousNonTemporalLoad_ScalarPlusScalarMask)) {
    case LDNT1B_z_p_br_contiguous:
      mnemonic = "ldnt1b";
      form = "{'Zt.b}, 'Pgl/z, ['Xns, 'Rm]";
      break;
    case LDNT1D_z_p_br_contiguous:
      mnemonic = "ldnt1d";
      form = "{'Zt.d}, 'Pgl/z, ['Xns, 'Rm, lsl #3]";
      break;
    case LDNT1H_z_p_br_contiguous:
      mnemonic = "ldnt1h";
      form = "{'Zt.h}, 'Pgl/z, ['Xns, 'Rm, lsl #1]";
      break;
    case LDNT1W_z_p_br_contiguous:
      mnemonic = "ldnt1w";
      form = "{'Zt.s}, 'Pgl/z, ['Xns, 'Rm, lsl #2]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEContiguousNonTemporalStore_ScalarPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEContiguousNonTemporalStore_ScalarPlusImm)";

  const char *suffix =
      (instr->ExtractBits(19, 16) == 0) ? "]" : ", #'s1916, mul vl]";
  switch (instr->Mask(SVEContiguousNonTemporalStore_ScalarPlusImmMask)) {
    case STNT1B_z_p_bi_contiguous:
      mnemonic = "stnt1b";
      form = "{'Zt.b}, 'Pgl, ['Xns";
      break;
    case STNT1D_z_p_bi_contiguous:
      mnemonic = "stnt1d";
      form = "{'Zt.d}, 'Pgl, ['Xns";
      break;
    case STNT1H_z_p_bi_contiguous:
      mnemonic = "stnt1h";
      form = "{'Zt.h}, 'Pgl, ['Xns";
      break;
    case STNT1W_z_p_bi_contiguous:
      mnemonic = "stnt1w";
      form = "{'Zt.s}, 'Pgl, ['Xns";
      break;
    default:
      suffix = NULL;
      break;
  }
  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVEContiguousNonTemporalStore_ScalarPlusScalar(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEContiguousNonTemporalStore_ScalarPlusScalar)";

  switch (instr->Mask(SVEContiguousNonTemporalStore_ScalarPlusScalarMask)) {
    case STNT1B_z_p_br_contiguous:
      mnemonic = "stnt1b";
      form = "{'Zt.b}, 'Pgl, ['Xns, 'Rm]";
      break;
    case STNT1D_z_p_br_contiguous:
      mnemonic = "stnt1d";
      form = "{'Zt.d}, 'Pgl, ['Xns, 'Rm, lsl #3]";
      break;
    case STNT1H_z_p_br_contiguous:
      mnemonic = "stnt1h";
      form = "{'Zt.h}, 'Pgl, ['Xns, 'Rm, lsl #1]";
      break;
    case STNT1W_z_p_br_contiguous:
      mnemonic = "stnt1w";
      form = "{'Zt.s}, 'Pgl, ['Xns, 'Rm, lsl #2]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEContiguousPrefetch_ScalarPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = (instr->ExtractBits(21, 16) != 0)
                         ? "'prefSVEOp, 'Pgl, ['Xns, #'s2116, mul vl]"
                         : "'prefSVEOp, 'Pgl, ['Xns]";

  switch (instr->Mask(SVEContiguousPrefetch_ScalarPlusImmMask)) {
    case PRFB_i_p_bi_s:
      mnemonic = "prfb";
      break;
    case PRFD_i_p_bi_s:
      mnemonic = "prfd";
      break;
    case PRFH_i_p_bi_s:
      mnemonic = "prfh";
      break;
    case PRFW_i_p_bi_s:
      mnemonic = "prfw";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEContiguousPrefetch_ScalarPlusScalar(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEContiguousPrefetch_ScalarPlusScalar)";

  if (instr->GetRm() != kZeroRegCode) {
    switch (instr->Mask(SVEContiguousPrefetch_ScalarPlusScalarMask)) {
      case PRFB_i_p_br_s:
        mnemonic = "prfb";
        form = "'prefSVEOp, 'Pgl, ['Xns, 'Rm]";
        break;
      case PRFD_i_p_br_s:
        mnemonic = "prfd";
        form = "'prefSVEOp, 'Pgl, ['Xns, 'Rm, lsl #3]";
        break;
      case PRFH_i_p_br_s:
        mnemonic = "prfh";
        form = "'prefSVEOp, 'Pgl, ['Xns, 'Rm, lsl #1]";
        break;
      case PRFW_i_p_br_s:
        mnemonic = "prfw";
        form = "'prefSVEOp, 'Pgl, ['Xns, 'Rm, lsl #2]";
        break;
      default:
        break;
    }
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEContiguousStore_ScalarPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";

  // The 'size' field isn't in the usual place here.
  const char *form = "{'Zt.'tls}, 'Pgl, ['Xns, #'s1916, mul vl]";
  if (instr->ExtractBits(19, 16) == 0) {
    form = "{'Zt.'tls}, 'Pgl, ['Xns]";
  }

  switch (instr->Mask(SVEContiguousStore_ScalarPlusImmMask)) {
    case ST1B_z_p_bi:
      mnemonic = "st1b";
      break;
    case ST1D_z_p_bi:
      mnemonic = "st1d";
      break;
    case ST1H_z_p_bi:
      mnemonic = "st1h";
      break;
    case ST1W_z_p_bi:
      mnemonic = "st1w";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEContiguousStore_ScalarPlusScalar(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";

  // The 'size' field isn't in the usual place here.
  const char *form = "{'Zt.'tls}, 'Pgl, ['Xns, 'Xm'NSveS]";

  switch (instr->Mask(SVEContiguousStore_ScalarPlusScalarMask)) {
    case ST1B_z_p_br:
      mnemonic = "st1b";
      break;
    case ST1D_z_p_br:
      mnemonic = "st1d";
      break;
    case ST1H_z_p_br:
      mnemonic = "st1h";
      break;
    case ST1W_z_p_br:
      mnemonic = "st1w";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVECopyFPImm_Predicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVECopyFPImm_Predicated)";

  switch (instr->Mask(SVECopyFPImm_PredicatedMask)) {
    case FCPY_z_p_i:
      // The preferred disassembly for fcpy is "fmov".
      mnemonic = "fmov";
      form = "'Zd.'t, 'Pm/m, 'IFPSve";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVECopyGeneralRegisterToVector_Predicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVECopyGeneralRegisterToVector_Predicated)";

  switch (instr->Mask(SVECopyGeneralRegisterToVector_PredicatedMask)) {
    case CPY_z_p_r:
      // The preferred disassembly for cpy is "mov".
      mnemonic = "mov";
      form = "'Zd.'t, 'Pgl/m, 'Wns";
      if (instr->GetSVESize() == kXRegSizeInBytesLog2) {
        form = "'Zd.'t, 'Pgl/m, 'Xns";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVECopyIntImm_Predicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVECopyIntImm_Predicated)";
  const char *suffix = NULL;

  switch (instr->Mask(SVECopyIntImm_PredicatedMask)) {
    case CPY_z_p_i: {
      // The preferred disassembly for cpy is "mov".
      mnemonic = "mov";
      form = "'Zd.'t, 'Pm/'?14:mz, #'s1205";
      if (instr->ExtractBit(13) != 0) suffix = ", lsl #8";
      break;
    }
    default:
      break;
  }
  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVECopySIMDFPScalarRegisterToVector_Predicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVECopySIMDFPScalarRegisterToVector_Predicated)";

  switch (instr->Mask(SVECopySIMDFPScalarRegisterToVector_PredicatedMask)) {
    case CPY_z_p_v:
      // The preferred disassembly for cpy is "mov".
      mnemonic = "mov";
      form = "'Zd.'t, 'Pgl/m, 'Vnv";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEExtractElementToGeneralRegister(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Wd, 'Pgl, 'Zn.'t";

  if (instr->GetSVESize() == kDRegSizeInBytesLog2) {
    form = "'Xd, p'u1210, 'Zn.'t";
  }

  switch (instr->Mask(SVEExtractElementToGeneralRegisterMask)) {
    case LASTA_r_p_z:
      mnemonic = "lasta";
      break;
    case LASTB_r_p_z:
      mnemonic = "lastb";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEExtractElementToSIMDFPScalarRegister(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'t'u0400, 'Pgl, 'Zn.'t";

  switch (instr->Mask(SVEExtractElementToSIMDFPScalarRegisterMask)) {
    case LASTA_v_p_z:
      mnemonic = "lasta";
      break;
    case LASTB_v_p_z:
      mnemonic = "lastb";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFFRInitialise(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFFRInitialise)";

  switch (instr->Mask(SVEFFRInitialiseMask)) {
    case SETFFR_f:
      mnemonic = "setffr";
      form = " ";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFFRWriteFromPredicate(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFFRWriteFromPredicate)";

  switch (instr->Mask(SVEFFRWriteFromPredicateMask)) {
    case WRFFR_f_p:
      mnemonic = "wrffr";
      form = "'Pn.b";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPArithmeticWithImm_Predicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form00 = "'Zd.'t, 'Pgl/m, 'Zd.'t, #0.0";
  const char *form05 = "'Zd.'t, 'Pgl/m, 'Zd.'t, #0.5";
  const char *form10 = "'Zd.'t, 'Pgl/m, 'Zd.'t, #1.0";
  const char *form20 = "'Zd.'t, 'Pgl/m, 'Zd.'t, #2.0";
  int i1 = instr->ExtractBit(5);
  const char *form = i1 ? form10 : form00;

  switch (instr->Mask(SVEFPArithmeticWithImm_PredicatedMask)) {
    case FADD_z_p_zs:
      mnemonic = "fadd";
      form = i1 ? form10 : form05;
      break;
    case FMAXNM_z_p_zs:
      mnemonic = "fmaxnm";
      break;
    case FMAX_z_p_zs:
      mnemonic = "fmax";
      break;
    case FMINNM_z_p_zs:
      mnemonic = "fminnm";
      break;
    case FMIN_z_p_zs:
      mnemonic = "fmin";
      break;
    case FMUL_z_p_zs:
      mnemonic = "fmul";
      form = i1 ? form20 : form05;
      break;
    case FSUBR_z_p_zs:
      mnemonic = "fsubr";
      form = i1 ? form10 : form05;
      break;
    case FSUB_z_p_zs:
      mnemonic = "fsub";
      form = i1 ? form10 : form05;
      break;
    default:
      form = "(SVEFPArithmeticWithImm_Predicated)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPArithmetic_Predicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEFPArithmetic_PredicatedMask)) {
    case FABD_z_p_zz:
      mnemonic = "fabd";
      break;
    case FADD_z_p_zz:
      mnemonic = "fadd";
      break;
    case FDIVR_z_p_zz:
      mnemonic = "fdivr";
      break;
    case FDIV_z_p_zz:
      mnemonic = "fdiv";
      break;
    case FMAXNM_z_p_zz:
      mnemonic = "fmaxnm";
      break;
    case FMAX_z_p_zz:
      mnemonic = "fmax";
      break;
    case FMINNM_z_p_zz:
      mnemonic = "fminnm";
      break;
    case FMIN_z_p_zz:
      mnemonic = "fmin";
      break;
    case FMULX_z_p_zz:
      mnemonic = "fmulx";
      break;
    case FMUL_z_p_zz:
      mnemonic = "fmul";
      break;
    case FSCALE_z_p_zz:
      mnemonic = "fscale";
      break;
    case FSUBR_z_p_zz:
      mnemonic = "fsubr";
      break;
    case FSUB_z_p_zz:
      mnemonic = "fsub";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPConvertPrecision(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPConvertPrecision)";

  switch (instr->Mask(SVEFPConvertPrecisionMask)) {
    case FCVT_z_p_z_d2h:
      mnemonic = "fcvt";
      form = "'Zd.h, 'Pgl/m, 'Zn.d";
      break;
    case FCVT_z_p_z_d2s:
      mnemonic = "fcvt";
      form = "'Zd.s, 'Pgl/m, 'Zn.d";
      break;
    case FCVT_z_p_z_h2d:
      mnemonic = "fcvt";
      form = "'Zd.d, 'Pgl/m, 'Zn.h";
      break;
    case FCVT_z_p_z_h2s:
      mnemonic = "fcvt";
      form = "'Zd.s, 'Pgl/m, 'Zn.h";
      break;
    case FCVT_z_p_z_s2d:
      mnemonic = "fcvt";
      form = "'Zd.d, 'Pgl/m, 'Zn.s";
      break;
    case FCVT_z_p_z_s2h:
      mnemonic = "fcvt";
      form = "'Zd.h, 'Pgl/m, 'Zn.s";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPConvertToInt(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPConvertToInt)";

  switch (instr->Mask(SVEFPConvertToIntMask)) {
    case FCVTZS_z_p_z_d2w:
      mnemonic = "fcvtzs";
      form = "'Zd.s, 'Pgl/m, 'Zn.d";
      break;
    case FCVTZS_z_p_z_d2x:
      mnemonic = "fcvtzs";
      form = "'Zd.d, 'Pgl/m, 'Zn.d";
      break;
    case FCVTZS_z_p_z_fp162h:
      mnemonic = "fcvtzs";
      form = "'Zd.h, 'Pgl/m, 'Zn.h";
      break;
    case FCVTZS_z_p_z_fp162w:
      mnemonic = "fcvtzs";
      form = "'Zd.s, 'Pgl/m, 'Zn.h";
      break;
    case FCVTZS_z_p_z_fp162x:
      mnemonic = "fcvtzs";
      form = "'Zd.d, 'Pgl/m, 'Zn.h";
      break;
    case FCVTZS_z_p_z_s2w:
      mnemonic = "fcvtzs";
      form = "'Zd.s, 'Pgl/m, 'Zn.s";
      break;
    case FCVTZS_z_p_z_s2x:
      mnemonic = "fcvtzs";
      form = "'Zd.d, 'Pgl/m, 'Zn.s";
      break;
    case FCVTZU_z_p_z_d2w:
      mnemonic = "fcvtzu";
      form = "'Zd.s, 'Pgl/m, 'Zn.d";
      break;
    case FCVTZU_z_p_z_d2x:
      mnemonic = "fcvtzu";
      form = "'Zd.d, 'Pgl/m, 'Zn.d";
      break;
    case FCVTZU_z_p_z_fp162h:
      mnemonic = "fcvtzu";
      form = "'Zd.h, 'Pgl/m, 'Zn.h";
      break;
    case FCVTZU_z_p_z_fp162w:
      mnemonic = "fcvtzu";
      form = "'Zd.s, 'Pgl/m, 'Zn.h";
      break;
    case FCVTZU_z_p_z_fp162x:
      mnemonic = "fcvtzu";
      form = "'Zd.d, 'Pgl/m, 'Zn.h";
      break;
    case FCVTZU_z_p_z_s2w:
      mnemonic = "fcvtzu";
      form = "'Zd.s, 'Pgl/m, 'Zn.s";
      break;
    case FCVTZU_z_p_z_s2x:
      mnemonic = "fcvtzu";
      form = "'Zd.d, 'Pgl/m, 'Zn.s";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPExponentialAccelerator(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPExponentialAccelerator)";

  unsigned size = instr->GetSVESize();
  switch (instr->Mask(SVEFPExponentialAcceleratorMask)) {
    case FEXPA_z_z:
      if ((size == kHRegSizeInBytesLog2) || (size == kSRegSizeInBytesLog2) ||
          (size == kDRegSizeInBytesLog2)) {
        mnemonic = "fexpa";
        form = "'Zd.'t, 'Zn.'t";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPRoundToIntegralValue(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zn.'t";

  switch (instr->Mask(SVEFPRoundToIntegralValueMask)) {
    case FRINTA_z_p_z:
      mnemonic = "frinta";
      break;
    case FRINTI_z_p_z:
      mnemonic = "frinti";
      break;
    case FRINTM_z_p_z:
      mnemonic = "frintm";
      break;
    case FRINTN_z_p_z:
      mnemonic = "frintn";
      break;
    case FRINTP_z_p_z:
      mnemonic = "frintp";
      break;
    case FRINTX_z_p_z:
      mnemonic = "frintx";
      break;
    case FRINTZ_z_p_z:
      mnemonic = "frintz";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPTrigMulAddCoefficient(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPTrigMulAddCoefficient)";

  unsigned size = instr->GetSVESize();
  switch (instr->Mask(SVEFPTrigMulAddCoefficientMask)) {
    case FTMAD_z_zzi:
      if ((size == kHRegSizeInBytesLog2) || (size == kSRegSizeInBytesLog2) ||
          (size == kDRegSizeInBytesLog2)) {
        mnemonic = "ftmad";
        form = "'Zd.'t, 'Zd.'t, 'Zn.'t, #'u1816";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPTrigSelectCoefficient(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPTrigSelectCoefficient)";

  unsigned size = instr->GetSVESize();
  switch (instr->Mask(SVEFPTrigSelectCoefficientMask)) {
    case FTSSEL_z_zz:
      if ((size == kHRegSizeInBytesLog2) || (size == kSRegSizeInBytesLog2) ||
          (size == kDRegSizeInBytesLog2)) {
        mnemonic = "ftssel";
        form = "'Zd.'t, 'Zn.'t, 'Zm.'t";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPUnaryOp(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zn.'t";

  if (instr->GetSVESize() == kBRegSizeInBytesLog2) {
    form = "(SVEFPUnaryOp)";
  } else {
    switch (instr->Mask(SVEFPUnaryOpMask)) {
      case FRECPX_z_p_z:
        mnemonic = "frecpx";
        break;
      case FSQRT_z_p_z:
        mnemonic = "fsqrt";
        break;
      default:
        form = "(SVEFPUnaryOp)";
        break;
    }
  }
  Format(instr, mnemonic, form);
}

static const char *IncDecFormHelper(const Instruction *instr,
                                    const char *reg_pat_mul_form,
                                    const char *reg_pat_form,
                                    const char *reg_form) {
  if (instr->ExtractBits(19, 16) == 0) {
    if (instr->ExtractBits(9, 5) == SVE_ALL) {
      // Use the register only form if the multiplier is one (encoded as zero)
      // and the pattern is SVE_ALL.
      return reg_form;
    }
    // Use the register and pattern form if the multiplier is one.
    return reg_pat_form;
  }
  return reg_pat_mul_form;
}

void Disassembler::VisitSVEIncDecRegisterByElementCount(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form =
      IncDecFormHelper(instr, "'Xd, 'Ipc, mul #'u1916+1", "'Xd, 'Ipc", "'Xd");

  switch (instr->Mask(SVEIncDecRegisterByElementCountMask)) {
    case DECB_r_rs:
      mnemonic = "decb";
      break;
    case DECD_r_rs:
      mnemonic = "decd";
      break;
    case DECH_r_rs:
      mnemonic = "dech";
      break;
    case DECW_r_rs:
      mnemonic = "decw";
      break;
    case INCB_r_rs:
      mnemonic = "incb";
      break;
    case INCD_r_rs:
      mnemonic = "incd";
      break;
    case INCH_r_rs:
      mnemonic = "inch";
      break;
    case INCW_r_rs:
      mnemonic = "incw";
      break;
    default:
      form = "(SVEIncDecRegisterByElementCount)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIncDecVectorByElementCount(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = IncDecFormHelper(instr,
                                      "'Zd.'t, 'Ipc, mul #'u1916+1",
                                      "'Zd.'t, 'Ipc",
                                      "'Zd.'t");

  switch (instr->Mask(SVEIncDecVectorByElementCountMask)) {
    case DECD_z_zs:
      mnemonic = "decd";
      break;
    case DECH_z_zs:
      mnemonic = "dech";
      break;
    case DECW_z_zs:
      mnemonic = "decw";
      break;
    case INCD_z_zs:
      mnemonic = "incd";
      break;
    case INCH_z_zs:
      mnemonic = "inch";
      break;
    case INCW_z_zs:
      mnemonic = "incw";
      break;
    default:
      form = "(SVEIncDecVectorByElementCount)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEInsertGeneralRegister(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEInsertGeneralRegister)";

  switch (instr->Mask(SVEInsertGeneralRegisterMask)) {
    case INSR_z_r:
      mnemonic = "insr";
      if (instr->GetSVESize() == kDRegSizeInBytesLog2) {
        form = "'Zd.'t, 'Xn";
      } else {
        form = "'Zd.'t, 'Wn";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEInsertSIMDFPScalarRegister(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEInsertSIMDFPScalarRegister)";

  switch (instr->Mask(SVEInsertSIMDFPScalarRegisterMask)) {
    case INSR_z_v:
      mnemonic = "insr";
      form = "'Zd.'t, 'Vnv";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntAddSubtractImm_Unpredicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = (instr->ExtractBit(13) == 0)
                         ? "'Zd.'t, 'Zd.'t, #'u1205"
                         : "'Zd.'t, 'Zd.'t, #'u1205, lsl #8";

  switch (instr->Mask(SVEIntAddSubtractImm_UnpredicatedMask)) {
    case ADD_z_zi:
      mnemonic = "add";
      break;
    case SQADD_z_zi:
      mnemonic = "sqadd";
      break;
    case SQSUB_z_zi:
      mnemonic = "sqsub";
      break;
    case SUBR_z_zi:
      mnemonic = "subr";
      break;
    case SUB_z_zi:
      mnemonic = "sub";
      break;
    case UQADD_z_zi:
      mnemonic = "uqadd";
      break;
    case UQSUB_z_zi:
      mnemonic = "uqsub";
      break;
    default:
      form = "(SVEIntAddSubtractImm_Unpredicated)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntAddSubtractVectors_Predicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEIntAddSubtractVectors_PredicatedMask)) {
    case ADD_z_p_zz:
      mnemonic = "add";
      break;
    case SUBR_z_p_zz:
      mnemonic = "subr";
      break;
    case SUB_z_p_zz:
      mnemonic = "sub";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntCompareScalarCountAndLimit(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form =
      (instr->ExtractBit(12) == 0) ? "'Pd.'t, 'Wn, 'Wm" : "'Pd.'t, 'Xn, 'Xm";

  switch (instr->Mask(SVEIntCompareScalarCountAndLimitMask)) {
    case WHILELE_p_p_rr:
      mnemonic = "whilele";
      break;
    case WHILELO_p_p_rr:
      mnemonic = "whilelo";
      break;
    case WHILELS_p_p_rr:
      mnemonic = "whilels";
      break;
    case WHILELT_p_p_rr:
      mnemonic = "whilelt";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntConvertToFP(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEIntConvertToFP)";

  switch (instr->Mask(SVEIntConvertToFPMask)) {
    case SCVTF_z_p_z_h2fp16:
      mnemonic = "scvtf";
      form = "'Zd.h, 'Pgl/m, 'Zn.h";
      break;
    case SCVTF_z_p_z_w2d:
      mnemonic = "scvtf";
      form = "'Zd.d, 'Pgl/m, 'Zn.s";
      break;
    case SCVTF_z_p_z_w2fp16:
      mnemonic = "scvtf";
      form = "'Zd.h, 'Pgl/m, 'Zn.s";
      break;
    case SCVTF_z_p_z_w2s:
      mnemonic = "scvtf";
      form = "'Zd.s, 'Pgl/m, 'Zn.s";
      break;
    case SCVTF_z_p_z_x2d:
      mnemonic = "scvtf";
      form = "'Zd.d, 'Pgl/m, 'Zn.d";
      break;
    case SCVTF_z_p_z_x2fp16:
      mnemonic = "scvtf";
      form = "'Zd.h, 'Pgl/m, 'Zn.d";
      break;
    case SCVTF_z_p_z_x2s:
      mnemonic = "scvtf";
      form = "'Zd.s, 'Pgl/m, 'Zn.d";
      break;
    case UCVTF_z_p_z_h2fp16:
      mnemonic = "ucvtf";
      form = "'Zd.h, 'Pgl/m, 'Zn.h";
      break;
    case UCVTF_z_p_z_w2d:
      mnemonic = "ucvtf";
      form = "'Zd.d, 'Pgl/m, 'Zn.s";
      break;
    case UCVTF_z_p_z_w2fp16:
      mnemonic = "ucvtf";
      form = "'Zd.h, 'Pgl/m, 'Zn.s";
      break;
    case UCVTF_z_p_z_w2s:
      mnemonic = "ucvtf";
      form = "'Zd.s, 'Pgl/m, 'Zn.s";
      break;
    case UCVTF_z_p_z_x2d:
      mnemonic = "ucvtf";
      form = "'Zd.d, 'Pgl/m, 'Zn.d";
      break;
    case UCVTF_z_p_z_x2fp16:
      mnemonic = "ucvtf";
      form = "'Zd.h, 'Pgl/m, 'Zn.d";
      break;
    case UCVTF_z_p_z_x2s:
      mnemonic = "ucvtf";
      form = "'Zd.s, 'Pgl/m, 'Zn.d";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntDivideVectors_Predicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEIntDivideVectors_PredicatedMask)) {
    case SDIVR_z_p_zz:
      mnemonic = "sdivr";
      break;
    case SDIV_z_p_zz:
      mnemonic = "sdiv";
      break;
    case UDIVR_z_p_zz:
      mnemonic = "udivr";
      break;
    case UDIV_z_p_zz:
      mnemonic = "udiv";
      break;
    default:
      break;
  }

  switch (instr->Mask(SVEIntDivideVectors_PredicatedMask)) {
    case SDIVR_z_p_zz:
    case SDIV_z_p_zz:
    case UDIVR_z_p_zz:
    case UDIV_z_p_zz:
      switch (instr->GetSVESize()) {
        case kBRegSizeInBytesLog2:
        case kHRegSizeInBytesLog2:
          mnemonic = "unimplemented";
          form = "(SVEIntBinaryArithmeticPredicated)";
          break;
        case kSRegSizeInBytesLog2:
        case kDRegSizeInBytesLog2:
          // The default form works for these instructions.
          break;
        default:
          // GetSVESize() should never return other values.
          VIXL_UNREACHABLE();
          break;
      }
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntMinMaxDifference_Predicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEIntMinMaxDifference_PredicatedMask)) {
    case SABD_z_p_zz:
      mnemonic = "sabd";
      break;
    case SMAX_z_p_zz:
      mnemonic = "smax";
      break;
    case SMIN_z_p_zz:
      mnemonic = "smin";
      break;
    case UABD_z_p_zz:
      mnemonic = "uabd";
      break;
    case UMAX_z_p_zz:
      mnemonic = "umax";
      break;
    case UMIN_z_p_zz:
      mnemonic = "umin";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntMinMaxImm_Unpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Zd.'t, #'u1205";

  switch (instr->Mask(SVEIntMinMaxImm_UnpredicatedMask)) {
    case SMAX_z_zi:
      mnemonic = "smax";
      form = "'Zd.'t, 'Zd.'t, #'s1205";
      break;
    case SMIN_z_zi:
      mnemonic = "smin";
      form = "'Zd.'t, 'Zd.'t, #'s1205";
      break;
    case UMAX_z_zi:
      mnemonic = "umax";
      break;
    case UMIN_z_zi:
      mnemonic = "umin";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntMulImm_Unpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEIntMulImm_Unpredicated)";

  switch (instr->Mask(SVEIntMulImm_UnpredicatedMask)) {
    case MUL_z_zi:
      mnemonic = "mul";
      form = "'Zd.'t, 'Zd.'t, #'s1205";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntMulVectors_Predicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEIntMulVectors_PredicatedMask)) {
    case MUL_z_p_zz:
      mnemonic = "mul";
      break;
    case SMULH_z_p_zz:
      mnemonic = "smulh";
      break;
    case UMULH_z_p_zz:
      mnemonic = "umulh";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVELoadAndBroadcastElement(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVELoadAndBroadcastElement)";
  const char *suffix_b = ", #'u2116]";
  const char *suffix_h = ", #'u2116*2]";
  const char *suffix_w = ", #'u2116*4]";
  const char *suffix_d = ", #'u2116*8]";
  const char *suffix = NULL;

  switch (instr->Mask(SVELoadAndBroadcastElementMask)) {
    case LD1RB_z_p_bi_u16:
      mnemonic = "ld1rb";
      form = "{'Zt.h}, 'Pgl/z, ['Xns";
      suffix = suffix_b;
      break;
    case LD1RB_z_p_bi_u32:
      mnemonic = "ld1rb";
      form = "{'Zt.s}, 'Pgl/z, ['Xns";
      suffix = suffix_b;
      break;
    case LD1RB_z_p_bi_u64:
      mnemonic = "ld1rb";
      form = "{'Zt.d}, 'Pgl/z, ['Xns";
      suffix = suffix_b;
      break;
    case LD1RB_z_p_bi_u8:
      mnemonic = "ld1rb";
      form = "{'Zt.b}, 'Pgl/z, ['Xns";
      suffix = suffix_b;
      break;
    case LD1RD_z_p_bi_u64:
      mnemonic = "ld1rd";
      form = "{'Zt.d}, 'Pgl/z, ['Xns";
      suffix = suffix_d;
      break;
    case LD1RH_z_p_bi_u16:
      mnemonic = "ld1rh";
      form = "{'Zt.h}, 'Pgl/z, ['Xns";
      suffix = suffix_h;
      break;
    case LD1RH_z_p_bi_u32:
      mnemonic = "ld1rh";
      form = "{'Zt.s}, 'Pgl/z, ['Xns";
      suffix = suffix_h;
      break;
    case LD1RH_z_p_bi_u64:
      mnemonic = "ld1rh";
      form = "{'Zt.d}, 'Pgl/z, ['Xns";
      suffix = suffix_h;
      break;
    case LD1RSB_z_p_bi_s16:
      mnemonic = "ld1rsb";
      form = "{'Zt.h}, 'Pgl/z, ['Xns";
      suffix = suffix_b;
      break;
    case LD1RSB_z_p_bi_s32:
      mnemonic = "ld1rsb";
      form = "{'Zt.s}, 'Pgl/z, ['Xns";
      suffix = suffix_b;
      break;
    case LD1RSB_z_p_bi_s64:
      mnemonic = "ld1rsb";
      form = "{'Zt.d}, 'Pgl/z, ['Xns";
      suffix = suffix_b;
      break;
    case LD1RSH_z_p_bi_s32:
      mnemonic = "ld1rsh";
      form = "{'Zt.s}, 'Pgl/z, ['Xns";
      suffix = suffix_h;
      break;
    case LD1RSH_z_p_bi_s64:
      mnemonic = "ld1rsh";
      form = "{'Zt.d}, 'Pgl/z, ['Xns";
      suffix = suffix_h;
      break;
    case LD1RSW_z_p_bi_s64:
      mnemonic = "ld1rsw";
      form = "{'Zt.d}, 'Pgl/z, ['Xns";
      suffix = suffix_w;
      break;
    case LD1RW_z_p_bi_u32:
      mnemonic = "ld1rw";
      form = "{'Zt.s}, 'Pgl/z, ['Xns";
      suffix = suffix_w;
      break;
    case LD1RW_z_p_bi_u64:
      mnemonic = "ld1rw";
      form = "{'Zt.d}, 'Pgl/z, ['Xns";
      suffix = suffix_w;
      break;
    default:
      break;
  }

  // Hide curly brackets if immediate is zero.
  if (instr->ExtractBits(21, 16) == 0) {
    suffix = "]";
  }

  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVELoadAndBroadcastQuadword_ScalarPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVELoadAndBroadcastQuadword_ScalarPlusImm)";

  const char *suffix =
      (instr->ExtractBits(19, 16) == 0) ? "]" : ", #'s1916*16]";

  switch (instr->Mask(SVELoadAndBroadcastQuadword_ScalarPlusImmMask)) {
    case LD1RQB_z_p_bi_u8:
      mnemonic = "ld1rqb";
      form = "{'Zt.b}, 'Pgl/z, ['Xns";
      break;
    case LD1RQD_z_p_bi_u64:
      mnemonic = "ld1rqd";
      form = "{'Zt.d}, 'Pgl/z, ['Xns";
      break;
    case LD1RQH_z_p_bi_u16:
      mnemonic = "ld1rqh";
      form = "{'Zt.h}, 'Pgl/z, ['Xns";
      break;
    case LD1RQW_z_p_bi_u32:
      mnemonic = "ld1rqw";
      form = "{'Zt.s}, 'Pgl/z, ['Xns";
      break;
    default:
      suffix = NULL;
      break;
  }
  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVELoadAndBroadcastQuadword_ScalarPlusScalar(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVELoadAndBroadcastQuadword_ScalarPlusScalar)";

  switch (instr->Mask(SVELoadAndBroadcastQuadword_ScalarPlusScalarMask)) {
    case LD1RQB_z_p_br_contiguous:
      mnemonic = "ld1rqb";
      form = "{'Zt.b}, 'Pgl/z, ['Xns, 'Rm]";
      break;
    case LD1RQD_z_p_br_contiguous:
      mnemonic = "ld1rqd";
      form = "{'Zt.d}, 'Pgl/z, ['Xns, 'Rm, lsl #3]";
      break;
    case LD1RQH_z_p_br_contiguous:
      mnemonic = "ld1rqh";
      form = "{'Zt.h}, 'Pgl/z, ['Xns, 'Rm, lsl #1]";
      break;
    case LD1RQW_z_p_br_contiguous:
      mnemonic = "ld1rqw";
      form = "{'Zt.s}, 'Pgl/z, ['Xns, 'Rm, lsl #2]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVELoadMultipleStructures_ScalarPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVELoadMultipleStructures_ScalarPlusImm)";

  const char *form_2 = "{'Zt.'tmsz, 'Zt2.'tmsz}, 'Pgl/z, ['Xns'ISveSvl]";
  const char *form_3 =
      "{'Zt.'tmsz, 'Zt2.'tmsz, 'Zt3.'tmsz}, 'Pgl/z, ['Xns'ISveSvl]";
  const char *form_4 =
      "{'Zt.'tmsz, 'Zt2.'tmsz, 'Zt3.'tmsz, 'Zt4.'tmsz}, "
      "'Pgl/z, ['Xns'ISveSvl]";

  switch (instr->Mask(SVELoadMultipleStructures_ScalarPlusImmMask)) {
    case LD2B_z_p_bi_contiguous:
      mnemonic = "ld2b";
      form = form_2;
      break;
    case LD2D_z_p_bi_contiguous:
      mnemonic = "ld2d";
      form = form_2;
      break;
    case LD2H_z_p_bi_contiguous:
      mnemonic = "ld2h";
      form = form_2;
      break;
    case LD2W_z_p_bi_contiguous:
      mnemonic = "ld2w";
      form = form_2;
      break;
    case LD3B_z_p_bi_contiguous:
      mnemonic = "ld3b";
      form = form_3;
      break;
    case LD3D_z_p_bi_contiguous:
      mnemonic = "ld3d";
      form = form_3;
      break;
    case LD3H_z_p_bi_contiguous:
      mnemonic = "ld3h";
      form = form_3;
      break;
    case LD3W_z_p_bi_contiguous:
      mnemonic = "ld3w";
      form = form_3;
      break;
    case LD4B_z_p_bi_contiguous:
      mnemonic = "ld4b";
      form = form_4;
      break;
    case LD4D_z_p_bi_contiguous:
      mnemonic = "ld4d";
      form = form_4;
      break;
    case LD4H_z_p_bi_contiguous:
      mnemonic = "ld4h";
      form = form_4;
      break;
    case LD4W_z_p_bi_contiguous:
      mnemonic = "ld4w";
      form = form_4;
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVELoadMultipleStructures_ScalarPlusScalar(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVELoadMultipleStructures_ScalarPlusScalar)";

  const char *form_2 = "{'Zt.'tmsz, 'Zt2.'tmsz}, 'Pgl/z, ['Xns, 'Xm'NSveS]";
  const char *form_3 =
      "{'Zt.'tmsz, 'Zt2.'tmsz, 'Zt3.'tmsz}, 'Pgl/z, ['Xns, 'Xm'NSveS]";
  const char *form_4 =
      "{'Zt.'tmsz, 'Zt2.'tmsz, 'Zt3.'tmsz, 'Zt4.'tmsz}, "
      "'Pgl/z, ['Xns, 'Xm'NSveS]";

  switch (instr->Mask(SVELoadMultipleStructures_ScalarPlusScalarMask)) {
    case LD2B_z_p_br_contiguous:
      mnemonic = "ld2b";
      form = form_2;
      break;
    case LD2D_z_p_br_contiguous:
      mnemonic = "ld2d";
      form = form_2;
      break;
    case LD2H_z_p_br_contiguous:
      mnemonic = "ld2h";
      form = form_2;
      break;
    case LD2W_z_p_br_contiguous:
      mnemonic = "ld2w";
      form = form_2;
      break;
    case LD3B_z_p_br_contiguous:
      mnemonic = "ld3b";
      form = form_3;
      break;
    case LD3D_z_p_br_contiguous:
      mnemonic = "ld3d";
      form = form_3;
      break;
    case LD3H_z_p_br_contiguous:
      mnemonic = "ld3h";
      form = form_3;
      break;
    case LD3W_z_p_br_contiguous:
      mnemonic = "ld3w";
      form = form_3;
      break;
    case LD4B_z_p_br_contiguous:
      mnemonic = "ld4b";
      form = form_4;
      break;
    case LD4D_z_p_br_contiguous:
      mnemonic = "ld4d";
      form = form_4;
      break;
    case LD4H_z_p_br_contiguous:
      mnemonic = "ld4h";
      form = form_4;
      break;
    case LD4W_z_p_br_contiguous:
      mnemonic = "ld4w";
      form = form_4;
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVELoadPredicateRegister(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVELoadPredicateRegister)";

  switch (instr->Mask(SVELoadPredicateRegisterMask)) {
    case LDR_p_bi:
      mnemonic = "ldr";
      if (instr->Mask(0x003f1c00) == 0) {
        form = "'Pd, ['Xns]";
      } else {
        form = "'Pd, ['Xns, #'s2116:1210, mul vl]";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVELoadVectorRegister(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVELoadVectorRegister)";

  switch (instr->Mask(SVELoadVectorRegisterMask)) {
    case LDR_z_bi:
      mnemonic = "ldr";
      if (instr->Mask(0x003f1c00) == 0) {
        form = "'Zd, ['Xns]";
      } else {
        form = "'Zt, ['Xns, #'s2116:1210, mul vl]";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPartitionBreakCondition(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Pd.b, p'u1310/'?04:mz, 'Pn.b";

  switch (instr->Mask(SVEPartitionBreakConditionMask)) {
    case BRKAS_p_p_p_z:
      mnemonic = "brkas";
      break;
    case BRKA_p_p_p:
      mnemonic = "brka";
      break;
    case BRKBS_p_p_p_z:
      mnemonic = "brkbs";
      break;
    case BRKB_p_p_p:
      mnemonic = "brkb";
      break;
    default:
      form = "(SVEPartitionBreakCondition)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPermutePredicateElements(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Pd.'t, 'Pn.'t, 'Pm.'t";

  switch (instr->Mask(SVEPermutePredicateElementsMask)) {
    case TRN1_p_pp:
      mnemonic = "trn1";
      break;
    case TRN2_p_pp:
      mnemonic = "trn2";
      break;
    case UZP1_p_pp:
      mnemonic = "uzp1";
      break;
    case UZP2_p_pp:
      mnemonic = "uzp2";
      break;
    case ZIP1_p_pp:
      mnemonic = "zip1";
      break;
    case ZIP2_p_pp:
      mnemonic = "zip2";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPredicateFirstActive(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEPredicateFirstActive)";

  switch (instr->Mask(SVEPredicateFirstActiveMask)) {
    case PFIRST_p_p_p:
      mnemonic = "pfirst";
      form = "'Pd.b, 'Pn, 'Pd.b";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPredicateReadFromFFR_Unpredicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEPredicateReadFromFFR_Unpredicated)";

  switch (instr->Mask(SVEPredicateReadFromFFR_UnpredicatedMask)) {
    case RDFFR_p_f:
      mnemonic = "rdffr";
      form = "'Pd.b";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPredicateTest(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEPredicateTest)";

  switch (instr->Mask(SVEPredicateTestMask)) {
    case PTEST_p_p:
      mnemonic = "ptest";
      form = "p'u1310, 'Pn.b";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPredicateZero(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEPredicateZero)";

  switch (instr->Mask(SVEPredicateZeroMask)) {
    case PFALSE_p:
      mnemonic = "pfalse";
      form = "'Pd.b";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPropagateBreakToNextPartition(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Pd.b, p'u1310/z, 'Pn.b, 'Pd.b";

  switch (instr->Mask(SVEPropagateBreakToNextPartitionMask)) {
    case BRKNS_p_p_pp:
      mnemonic = "brkns";
      break;
    case BRKN_p_p_pp:
      mnemonic = "brkn";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEReversePredicateElements(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEReversePredicateElements)";

  switch (instr->Mask(SVEReversePredicateElementsMask)) {
    case REV_p_p:
      mnemonic = "rev";
      form = "'Pd.'t, 'Pn.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEReverseVectorElements(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEReverseVectorElements)";

  switch (instr->Mask(SVEReverseVectorElementsMask)) {
    case REV_z_z:
      mnemonic = "rev";
      form = "'Zd.'t, 'Zn.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEReverseWithinElements(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zn.'t";

  unsigned size = instr->GetSVESize();
  switch (instr->Mask(SVEReverseWithinElementsMask)) {
    case RBIT_z_p_z:
      mnemonic = "rbit";
      break;
    case REVB_z_z:
      if ((size == kHRegSizeInBytesLog2) || (size == kSRegSizeInBytesLog2) ||
          (size == kDRegSizeInBytesLog2)) {
        mnemonic = "revb";
      } else {
        form = "(SVEReverseWithinElements)";
      }
      break;
    case REVH_z_z:
      if ((size == kSRegSizeInBytesLog2) || (size == kDRegSizeInBytesLog2)) {
        mnemonic = "revh";
      } else {
        form = "(SVEReverseWithinElements)";
      }
      break;
    case REVW_z_z:
      if (size == kDRegSizeInBytesLog2) {
        mnemonic = "revw";
      } else {
        form = "(SVEReverseWithinElements)";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVESaturatingIncDecRegisterByElementCount(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = IncDecFormHelper(instr,
                                      "'R20d, 'Ipc, mul #'u1916+1",
                                      "'R20d, 'Ipc",
                                      "'R20d");
  const char *form_sx = IncDecFormHelper(instr,
                                         "'Xd, 'Wd, 'Ipc, mul #'u1916+1",
                                         "'Xd, 'Wd, 'Ipc",
                                         "'Xd, 'Wd");

  switch (instr->Mask(SVESaturatingIncDecRegisterByElementCountMask)) {
    case SQDECB_r_rs_sx:
      mnemonic = "sqdecb";
      form = form_sx;
      break;
    case SQDECD_r_rs_sx:
      mnemonic = "sqdecd";
      form = form_sx;
      break;
    case SQDECH_r_rs_sx:
      mnemonic = "sqdech";
      form = form_sx;
      break;
    case SQDECW_r_rs_sx:
      mnemonic = "sqdecw";
      form = form_sx;
      break;
    case SQINCB_r_rs_sx:
      mnemonic = "sqincb";
      form = form_sx;
      break;
    case SQINCD_r_rs_sx:
      mnemonic = "sqincd";
      form = form_sx;
      break;
    case SQINCH_r_rs_sx:
      mnemonic = "sqinch";
      form = form_sx;
      break;
    case SQINCW_r_rs_sx:
      mnemonic = "sqincw";
      form = form_sx;
      break;
    case SQDECB_r_rs_x:
      mnemonic = "sqdecb";
      break;
    case SQDECD_r_rs_x:
      mnemonic = "sqdecd";
      break;
    case SQDECH_r_rs_x:
      mnemonic = "sqdech";
      break;
    case SQDECW_r_rs_x:
      mnemonic = "sqdecw";
      break;
    case SQINCB_r_rs_x:
      mnemonic = "sqincb";
      break;
    case SQINCD_r_rs_x:
      mnemonic = "sqincd";
      break;
    case SQINCH_r_rs_x:
      mnemonic = "sqinch";
      break;
    case SQINCW_r_rs_x:
      mnemonic = "sqincw";
      break;
    case UQDECB_r_rs_uw:
    case UQDECB_r_rs_x:
      mnemonic = "uqdecb";
      break;
    case UQDECD_r_rs_uw:
    case UQDECD_r_rs_x:
      mnemonic = "uqdecd";
      break;
    case UQDECH_r_rs_uw:
    case UQDECH_r_rs_x:
      mnemonic = "uqdech";
      break;
    case UQDECW_r_rs_uw:
    case UQDECW_r_rs_x:
      mnemonic = "uqdecw";
      break;
    case UQINCB_r_rs_uw:
    case UQINCB_r_rs_x:
      mnemonic = "uqincb";
      break;
    case UQINCD_r_rs_uw:
    case UQINCD_r_rs_x:
      mnemonic = "uqincd";
      break;
    case UQINCH_r_rs_uw:
    case UQINCH_r_rs_x:
      mnemonic = "uqinch";
      break;
    case UQINCW_r_rs_uw:
    case UQINCW_r_rs_x:
      mnemonic = "uqincw";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVESaturatingIncDecVectorByElementCount(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = IncDecFormHelper(instr,
                                      "'Zd.'t, 'Ipc, mul #'u1916+1",
                                      "'Zd.'t, 'Ipc",
                                      "'Zd.'t");

  switch (instr->Mask(SVESaturatingIncDecVectorByElementCountMask)) {
    case SQDECD_z_zs:
      mnemonic = "sqdecd";
      break;
    case SQDECH_z_zs:
      mnemonic = "sqdech";
      break;
    case SQDECW_z_zs:
      mnemonic = "sqdecw";
      break;
    case SQINCD_z_zs:
      mnemonic = "sqincd";
      break;
    case SQINCH_z_zs:
      mnemonic = "sqinch";
      break;
    case SQINCW_z_zs:
      mnemonic = "sqincw";
      break;
    case UQDECD_z_zs:
      mnemonic = "uqdecd";
      break;
    case UQDECH_z_zs:
      mnemonic = "uqdech";
      break;
    case UQDECW_z_zs:
      mnemonic = "uqdecw";
      break;
    case UQINCD_z_zs:
      mnemonic = "uqincd";
      break;
    case UQINCH_z_zs:
      mnemonic = "uqinch";
      break;
    case UQINCW_z_zs:
      mnemonic = "uqincw";
      break;
    default:
      form = "(SVEElementCount)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEStoreMultipleStructures_ScalarPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEStoreMultipleStructures_ScalarPlusImm)";

  const char *form_2 = "{'Zt.'tmsz, 'Zt2.'tmsz}, 'Pgl, ['Xns'ISveSvl]";
  const char *form_3 =
      "{'Zt.'tmsz, 'Zt2.'tmsz, 'Zt3.'tmsz}, 'Pgl, ['Xns'ISveSvl]";
  const char *form_4 =
      "{'Zt.'tmsz, 'Zt2.'tmsz, 'Zt3.'tmsz, 'Zt4.'tmsz}, "
      "'Pgl, ['Xns'ISveSvl]";

  switch (instr->Mask(SVEStoreMultipleStructures_ScalarPlusImmMask)) {
    case ST2B_z_p_bi_contiguous:
      mnemonic = "st2b";
      form = form_2;
      break;
    case ST2H_z_p_bi_contiguous:
      mnemonic = "st2h";
      form = form_2;
      break;
    case ST2W_z_p_bi_contiguous:
      mnemonic = "st2w";
      form = form_2;
      break;
    case ST2D_z_p_bi_contiguous:
      mnemonic = "st2d";
      form = form_2;
      break;
    case ST3B_z_p_bi_contiguous:
      mnemonic = "st3b";
      form = form_3;
      break;
    case ST3H_z_p_bi_contiguous:
      mnemonic = "st3h";
      form = form_3;
      break;
    case ST3W_z_p_bi_contiguous:
      mnemonic = "st3w";
      form = form_3;
      break;
    case ST3D_z_p_bi_contiguous:
      mnemonic = "st3d";
      form = form_3;
      break;
    case ST4B_z_p_bi_contiguous:
      mnemonic = "st4b";
      form = form_4;
      break;
    case ST4H_z_p_bi_contiguous:
      mnemonic = "st4h";
      form = form_4;
      break;
    case ST4W_z_p_bi_contiguous:
      mnemonic = "st4w";
      form = form_4;
      break;
    case ST4D_z_p_bi_contiguous:
      mnemonic = "st4d";
      form = form_4;
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEStoreMultipleStructures_ScalarPlusScalar(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEStoreMultipleStructures_ScalarPlusScalar)";

  const char *form_2 = "{'Zt.'tmsz, 'Zt2.'tmsz}, 'Pgl, ['Xns, 'Xm'NSveS]";
  const char *form_3 =
      "{'Zt.'tmsz, 'Zt2.'tmsz, 'Zt3.'tmsz}, 'Pgl, ['Xns, 'Xm'NSveS]";
  const char *form_4 =
      "{'Zt.'tmsz, 'Zt2.'tmsz, 'Zt3.'tmsz, 'Zt4.'tmsz}, "
      "'Pgl, ['Xns, 'Xm'NSveS]";

  switch (instr->Mask(SVEStoreMultipleStructures_ScalarPlusScalarMask)) {
    case ST2B_z_p_br_contiguous:
      mnemonic = "st2b";
      form = form_2;
      break;
    case ST2D_z_p_br_contiguous:
      mnemonic = "st2d";
      form = form_2;
      break;
    case ST2H_z_p_br_contiguous:
      mnemonic = "st2h";
      form = form_2;
      break;
    case ST2W_z_p_br_contiguous:
      mnemonic = "st2w";
      form = form_2;
      break;
    case ST3B_z_p_br_contiguous:
      mnemonic = "st3b";
      form = form_3;
      break;
    case ST3D_z_p_br_contiguous:
      mnemonic = "st3d";
      form = form_3;
      break;
    case ST3H_z_p_br_contiguous:
      mnemonic = "st3h";
      form = form_3;
      break;
    case ST3W_z_p_br_contiguous:
      mnemonic = "st3w";
      form = form_3;
      break;
    case ST4B_z_p_br_contiguous:
      mnemonic = "st4b";
      form = form_4;
      break;
    case ST4D_z_p_br_contiguous:
      mnemonic = "st4d";
      form = form_4;
      break;
    case ST4H_z_p_br_contiguous:
      mnemonic = "st4h";
      form = form_4;
      break;
    case ST4W_z_p_br_contiguous:
      mnemonic = "st4w";
      form = form_4;
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEStorePredicateRegister(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEStorePredicateRegister)";

  switch (instr->Mask(SVEStorePredicateRegisterMask)) {
    case STR_p_bi:
      mnemonic = "str";
      if (instr->Mask(0x003f1c00) == 0) {
        form = "'Pd, ['Xns]";
      } else {
        form = "'Pd, ['Xns, #'s2116:1210, mul vl]";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEStoreVectorRegister(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEStoreVectorRegister)";

  switch (instr->Mask(SVEStoreVectorRegisterMask)) {
    case STR_z_bi:
      mnemonic = "str";
      if (instr->Mask(0x003f1c00) == 0) {
        form = "'Zd, ['Xns]";
      } else {
        form = "'Zt, ['Xns, #'s2116:1210, mul vl]";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVETableLookup(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVETableLookup)";

  switch (instr->Mask(SVETableLookupMask)) {
    case TBL_z_zz_1:
      mnemonic = "tbl";
      form = "'Zd.'t, {'Zn.'t}, 'Zm.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEUnpackPredicateElements(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Pd.h, 'Pn.b";

  switch (instr->Mask(SVEUnpackPredicateElementsMask)) {
    case PUNPKHI_p_p:
      mnemonic = "punpkhi";
      break;
    case PUNPKLO_p_p:
      mnemonic = "punpklo";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEUnpackVectorElements(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Zn.'th";

  if (instr->GetSVESize() == 0) {
    // The lowest lane size of the destination vector is H-sized lane.
    Format(instr, "unallocated", "(SVEUnpackVectorElements)");
    return;
  }

  switch (instr->Mask(SVEUnpackVectorElementsMask)) {
    case SUNPKHI_z_z:
      mnemonic = "sunpkhi";
      break;
    case SUNPKLO_z_z:
      mnemonic = "sunpklo";
      break;
    case UUNPKHI_z_z:
      mnemonic = "uunpkhi";
      break;
    case UUNPKLO_z_z:
      mnemonic = "uunpklo";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEVectorSplice_Destructive(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEVectorSplice_Destructive)";

  switch (instr->Mask(SVEVectorSplice_DestructiveMask)) {
    case SPLICE_z_p_zz_des:
      mnemonic = "splice";
      form = "'Zd.'t, 'Pgl, 'Zd.'t, 'Zn.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEAddressGeneration(const Instruction *instr) {
  const char *mnemonic = "adr";
  const char *form = "'Zd.d, ['Zn.d, 'Zm.d";
  const char *suffix = NULL;

  bool msz_is_zero = (instr->ExtractBits(11, 10) == 0);

  switch (instr->Mask(SVEAddressGenerationMask)) {
    case ADR_z_az_d_s32_scaled:
      suffix = msz_is_zero ? ", sxtw]" : ", sxtw #'u1110]";
      break;
    case ADR_z_az_d_u32_scaled:
      suffix = msz_is_zero ? ", uxtw]" : ", uxtw #'u1110]";
      break;
    case ADR_z_az_s_same_scaled:
    case ADR_z_az_d_same_scaled:
      form = "'Zd.'t, ['Zn.'t, 'Zm.'t";
      suffix = msz_is_zero ? "]" : ", lsl #'u1110]";
      break;
    default:
      mnemonic = "unimplemented";
      form = "(SVEAddressGeneration)";
      break;
  }
  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVEBitwiseLogicalUnpredicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.d, 'Zn.d, 'Zm.d";

  switch (instr->Mask(SVEBitwiseLogicalUnpredicatedMask)) {
    case AND_z_zz:
      mnemonic = "and";
      break;
    case BIC_z_zz:
      mnemonic = "bic";
      break;
    case EOR_z_zz:
      mnemonic = "eor";
      break;
    case ORR_z_zz:
      mnemonic = "orr";
      if (instr->GetRn() == instr->GetRm()) {
        mnemonic = "mov";
        form = "'Zd.d, 'Zn.d";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEBitwiseShiftUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEBitwiseShiftUnpredicated)";
  unsigned tsize =
      (instr->ExtractBits(23, 22) << 2) | instr->ExtractBits(20, 19);
  unsigned lane_size = instr->GetSVESize();

  switch (instr->Mask(SVEBitwiseShiftUnpredicatedMask)) {
    case ASR_z_zi:
      if (tsize != 0) {
        // The tsz field must not be zero.
        mnemonic = "asr";
        form = "'Zd.'tszs, 'Zn.'tszs, 'ITriSves";
      }
      break;
    case ASR_z_zw:
      if (lane_size <= kSRegSizeInBytesLog2) {
        mnemonic = "asr";
        form = "'Zd.'t, 'Zn.'t, 'Zm.d";
      }
      break;
    case LSL_z_zi:
      if (tsize != 0) {
        // The tsz field must not be zero.
        mnemonic = "lsl";
        form = "'Zd.'tszs, 'Zn.'tszs, 'ITriSver";
      }
      break;
    case LSL_z_zw:
      if (lane_size <= kSRegSizeInBytesLog2) {
        mnemonic = "lsl";
        form = "'Zd.'t, 'Zn.'t, 'Zm.d";
      }
      break;
    case LSR_z_zi:
      if (tsize != 0) {
        // The tsz field must not be zero.
        mnemonic = "lsr";
        form = "'Zd.'tszs, 'Zn.'tszs, 'ITriSves";
      }
      break;
    case LSR_z_zw:
      if (lane_size <= kSRegSizeInBytesLog2) {
        mnemonic = "lsr";
        form = "'Zd.'t, 'Zn.'t, 'Zm.d";
      }
      break;
    default:
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEElementCount(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form =
      IncDecFormHelper(instr, "'Xd, 'Ipc, mul #'u1916+1", "'Xd, 'Ipc", "'Xd");

  switch (instr->Mask(SVEElementCountMask)) {
    case CNTB_r_s:
      mnemonic = "cntb";
      break;
    case CNTD_r_s:
      mnemonic = "cntd";
      break;
    case CNTH_r_s:
      mnemonic = "cnth";
      break;
    case CNTW_r_s:
      mnemonic = "cntw";
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
    case FADDA_v_p_z:
      mnemonic = "fadda";
      form = "'t'u0400, 'Pgl, 't'u0400, 'Zn.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPArithmeticUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Zn.'t, 'Zm.'t";

  switch (instr->Mask(SVEFPArithmeticUnpredicatedMask)) {
    case FADD_z_zz:
      mnemonic = "fadd";
      break;
    case FMUL_z_zz:
      mnemonic = "fmul";
      break;
    case FRECPS_z_zz:
      mnemonic = "frecps";
      break;
    case FRSQRTS_z_zz:
      mnemonic = "frsqrts";
      break;
    case FSUB_z_zz:
      mnemonic = "fsub";
      break;
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
  const char *form = "'Pd.'t, 'Pgl/z, 'Zn.'t, 'Zm.'t";

  switch (instr->Mask(SVEFPCompareVectorsMask)) {
    case FACGE_p_p_zz:
      mnemonic = "facge";
      break;
    case FACGT_p_p_zz:
      mnemonic = "facgt";
      break;
    case FCMEQ_p_p_zz:
      mnemonic = "fcmeq";
      break;
    case FCMGE_p_p_zz:
      mnemonic = "fcmge";
      break;
    case FCMGT_p_p_zz:
      mnemonic = "fcmgt";
      break;
    case FCMNE_p_p_zz:
      mnemonic = "fcmne";
      break;
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
  const char *form = "'Pd.'t, 'Pgl/z, 'Zn.'t, #0.0";

  switch (instr->Mask(SVEFPCompareWithZeroMask)) {
    case FCMEQ_p_p_z0:
      mnemonic = "fcmeq";
      break;
    case FCMGE_p_p_z0:
      mnemonic = "fcmge";
      break;
    case FCMGT_p_p_z0:
      mnemonic = "fcmgt";
      break;
    case FCMLE_p_p_z0:
      mnemonic = "fcmle";
      break;
    case FCMLT_p_p_z0:
      mnemonic = "fcmlt";
      break;
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
    case FCADD_z_p_zz:
      mnemonic = "fcadd";
      if (instr->ExtractBit(16) == 0) {
        form = "'Zd.'t, 'Pgl/m, 'Zd.'t, 'Zn.'t, #90";
      } else {
        form = "'Zd.'t, 'Pgl/m, 'Zd.'t, 'Zn.'t, #270";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPComplexMulAdd(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPComplexMulAdd)";
  const char *suffix = NULL;

  const char *fcmla_constants[] = {"0", "90", "180", "270"};

  switch (instr->Mask(SVEFPComplexMulAddMask)) {
    case FCMLA_z_p_zzz:
      mnemonic = "fcmla";
      form = "'Zd.'t, 'Pgl/m, 'Zn.'t, 'Zm.'t, #";
      suffix = fcmla_constants[instr->ExtractBits(14, 13)];
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVEFPComplexMulAddIndex(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEFPComplexMulAddIndex)";

  const char *fcmla_constants[] = {"0", "90", "180", "270"};
  const char *suffix = fcmla_constants[instr->ExtractBits(11, 10)];

  switch (instr->Mask(SVEFPComplexMulAddIndexMask)) {
    case FCMLA_z_zzzi_h:
      mnemonic = "fcmla";
      form = "'Zd.h, 'Zn.h, z'u1816.h['u2019], #";
      break;
    case FCMLA_z_zzzi_s:
      mnemonic = "fcmla";
      form = "'Zd.s, 'Zn.s, z'u1916.s['u2020], #";
      break;
    default:
      suffix = NULL;
      break;
  }
  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVEFPFastReduction(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'t'u0400, 'Pgl, 'Zn.'t";

  switch (instr->Mask(SVEFPFastReductionMask)) {
    case FADDV_v_p_z:
      mnemonic = "faddv";
      break;
    case FMAXNMV_v_p_z:
      mnemonic = "fmaxnmv";
      break;
    case FMAXV_v_p_z:
      mnemonic = "fmaxv";
      break;
    case FMINNMV_v_p_z:
      mnemonic = "fminnmv";
      break;
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
    case FMUL_z_zzi_d:
      mnemonic = "fmul";
      form = "'Zd.d, 'Zn.d, z'u1916.d['u2020]";
      break;
    case FMUL_z_zzi_h:
    case FMUL_z_zzi_h_i3h:
      mnemonic = "fmul";
      form = "'Zd.h, 'Zn.h, z'u1816.h['u2222:2019]";
      break;
    case FMUL_z_zzi_s:
      mnemonic = "fmul";
      form = "'Zd.s, 'Zn.s, z'u1816.s['u2019]";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPMulAdd(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zn.'t, 'Zm.'t";

  switch (instr->Mask(SVEFPMulAddMask)) {
    case FMAD_z_p_zzz:
      mnemonic = "fmad";
      break;
    case FMLA_z_p_zzz:
      mnemonic = "fmla";
      break;
    case FMLS_z_p_zzz:
      mnemonic = "fmls";
      break;
    case FMSB_z_p_zzz:
      mnemonic = "fmsb";
      break;
    case FNMAD_z_p_zzz:
      mnemonic = "fnmad";
      break;
    case FNMLA_z_p_zzz:
      mnemonic = "fnmla";
      break;
    case FNMLS_z_p_zzz:
      mnemonic = "fnmls";
      break;
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
    case FMLA_z_zzzi_d:
      mnemonic = "fmla";
      form = "'Zd.d, 'Zn.d, z'u1916.d['u2020]";
      break;
    case FMLA_z_zzzi_s:
      mnemonic = "fmla";
      form = "'Zd.s, 'Zn.s, z'u1816.s['u2019]";
      break;
    case FMLS_z_zzzi_d:
      mnemonic = "fmls";
      form = "'Zd.d, 'Zn.d, z'u1916.d['u2020]";
      break;
    case FMLS_z_zzzi_s:
      mnemonic = "fmls";
      form = "'Zd.s, 'Zn.s, z'u1816.s['u2019]";
      break;
    case FMLA_z_zzzi_h:
    case FMLA_z_zzzi_h_i3h:
      mnemonic = "fmla";
      form = "'Zd.h, 'Zn.h, z'u1816.h['u2222:2019]";
      break;
    case FMLS_z_zzzi_h:
    case FMLS_z_zzzi_h_i3h:
      mnemonic = "fmls";
      form = "'Zd.h, 'Zn.h, z'u1816.h['u2222:2019]";
      break;
    default:
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEFPUnaryOpUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Zn.'t";

  switch (instr->Mask(SVEFPUnaryOpUnpredicatedMask)) {
    case FRECPE_z_z:
      mnemonic = "frecpe";
      break;
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
  const char *form = "(SVEIncDecByPredicateCount)";

  switch (instr->Mask(SVEIncDecByPredicateCountMask)) {
    case DECP_r_p_r:
    case DECP_z_p_z:
      mnemonic = "decp";
      break;
    case INCP_r_p_r:
    case INCP_z_p_z:
      mnemonic = "incp";
      break;
    case SQDECP_r_p_r_sx:
    case SQDECP_r_p_r_x:
    case SQDECP_z_p_z:
      mnemonic = "sqdecp";
      break;
    case SQINCP_r_p_r_sx:
    case SQINCP_r_p_r_x:
    case SQINCP_z_p_z:
      mnemonic = "sqincp";
      break;
    case UQDECP_r_p_r_uw:
    case UQDECP_r_p_r_x:
    case UQDECP_z_p_z:
      mnemonic = "uqdecp";
      break;
    case UQINCP_r_p_r_uw:
    case UQINCP_r_p_r_x:
    case UQINCP_z_p_z:
      mnemonic = "uqincp";
      break;
    default:
      break;
  }

  switch (instr->Mask(SVEIncDecByPredicateCountMask)) {
    // <Xdn>, <Pg>.<T>
    case DECP_r_p_r:
    case INCP_r_p_r:
      form = "'Xd, 'Pn.'t";
      break;
    // <Zdn>.<T>, <Pg>
    case DECP_z_p_z:
    case INCP_z_p_z:
    case SQDECP_z_p_z:
    case SQINCP_z_p_z:
    case UQDECP_z_p_z:
    case UQINCP_z_p_z:
      form = "'Zd.'t, 'Pn";
      break;
    // <Xdn>, <Pg>.<T>, <Wdn>
    case SQDECP_r_p_r_sx:
    case SQINCP_r_p_r_sx:
      form = "'Xd, 'Pn.'t, 'Wd";
      break;
    // <Xdn>, <Pg>.<T>
    case SQDECP_r_p_r_x:
    case SQINCP_r_p_r_x:
    case UQDECP_r_p_r_x:
    case UQINCP_r_p_r_x:
      form = "'Xd, 'Pn.'t";
      break;
    // <Wdn>, <Pg>.<T>
    case UQDECP_r_p_r_uw:
    case UQINCP_r_p_r_uw:
      form = "'Wd, 'Pn.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIndexGeneration(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEIndexGeneration)";

  bool w_inputs =
      static_cast<unsigned>(instr->GetSVESize()) <= kWRegSizeInBytesLog2;

  switch (instr->Mask(SVEIndexGenerationMask)) {
    case INDEX_z_ii:
      mnemonic = "index";
      form = "'Zd.'t, #'s0905, #'s2016";
      break;
    case INDEX_z_ir:
      mnemonic = "index";
      form = w_inputs ? "'Zd.'t, #'s0905, 'Wm" : "'Zd.'t, #'s0905, 'Xm";
      break;
    case INDEX_z_ri:
      mnemonic = "index";
      form = w_inputs ? "'Zd.'t, 'Wn, #'s2016" : "'Zd.'t, 'Xn, #'s2016";
      break;
    case INDEX_z_rr:
      mnemonic = "index";
      form = w_inputs ? "'Zd.'t, 'Wn, 'Wm" : "'Zd.'t, 'Xn, 'Xm";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntArithmeticUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Zn.'t, 'Zm.'t";

  switch (instr->Mask(SVEIntArithmeticUnpredicatedMask)) {
    case ADD_z_zz:
      mnemonic = "add";
      break;
    case SQADD_z_zz:
      mnemonic = "sqadd";
      break;
    case SQSUB_z_zz:
      mnemonic = "sqsub";
      break;
    case SUB_z_zz:
      mnemonic = "sub";
      break;
    case UQADD_z_zz:
      mnemonic = "uqadd";
      break;
    case UQSUB_z_zz:
      mnemonic = "uqsub";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntCompareSignedImm(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Pd.'t, 'Pgl/z, 'Zn.'t, #'s2016";

  switch (instr->Mask(SVEIntCompareSignedImmMask)) {
    case CMPEQ_p_p_zi:
      mnemonic = "cmpeq";
      break;
    case CMPGE_p_p_zi:
      mnemonic = "cmpge";
      break;
    case CMPGT_p_p_zi:
      mnemonic = "cmpgt";
      break;
    case CMPLE_p_p_zi:
      mnemonic = "cmple";
      break;
    case CMPLT_p_p_zi:
      mnemonic = "cmplt";
      break;
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
  const char *form = "'Pd.'t, 'Pgl/z, 'Zn.'t, #'u2014";

  switch (instr->Mask(SVEIntCompareUnsignedImmMask)) {
    case CMPHI_p_p_zi:
      mnemonic = "cmphi";
      break;
    case CMPHS_p_p_zi:
      mnemonic = "cmphs";
      break;
    case CMPLO_p_p_zi:
      mnemonic = "cmplo";
      break;
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
  const char *form = "'Pd.'t, 'Pgl/z, 'Zn.'t, 'Zm.d";

  switch (instr->Mask(SVEIntCompareVectorsMask)) {
    case CMPEQ_p_p_zw:
      mnemonic = "cmpeq";
      break;
    case CMPEQ_p_p_zz:
      mnemonic = "cmpeq";
      form = "'Pd.'t, 'Pgl/z, 'Zn.'t, 'Zm.'t";
      break;
    case CMPGE_p_p_zw:
      mnemonic = "cmpge";
      break;
    case CMPGE_p_p_zz:
      mnemonic = "cmpge";
      form = "'Pd.'t, 'Pgl/z, 'Zn.'t, 'Zm.'t";
      break;
    case CMPGT_p_p_zw:
      mnemonic = "cmpgt";
      break;
    case CMPGT_p_p_zz:
      mnemonic = "cmpgt";
      form = "'Pd.'t, 'Pgl/z, 'Zn.'t, 'Zm.'t";
      break;
    case CMPHI_p_p_zw:
      mnemonic = "cmphi";
      break;
    case CMPHI_p_p_zz:
      mnemonic = "cmphi";
      form = "'Pd.'t, 'Pgl/z, 'Zn.'t, 'Zm.'t";
      break;
    case CMPHS_p_p_zw:
      mnemonic = "cmphs";
      break;
    case CMPHS_p_p_zz:
      mnemonic = "cmphs";
      form = "'Pd.'t, 'Pgl/z, 'Zn.'t, 'Zm.'t";
      break;
    case CMPLE_p_p_zw:
      mnemonic = "cmple";
      break;
    case CMPLO_p_p_zw:
      mnemonic = "cmplo";
      break;
    case CMPLS_p_p_zw:
      mnemonic = "cmpls";
      break;
    case CMPLT_p_p_zw:
      mnemonic = "cmplt";
      break;
    case CMPNE_p_p_zw:
      mnemonic = "cmpne";
      break;
    case CMPNE_p_p_zz:
      mnemonic = "cmpne";
      form = "'Pd.'t, 'Pgl/z, 'Zn.'t, 'Zm.'t";
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
    case MAD_z_p_zzz:
      mnemonic = "mad";
      form = "'Zd.'t, 'Pgl/m, 'Zm.'t, 'Zn.'t";
      break;
    case MLA_z_p_zzz:
      mnemonic = "mla";
      form = "'Zd.'t, 'Pgl/m, 'Zn.'t, 'Zm.'t";
      break;
    case MLS_z_p_zzz:
      mnemonic = "mls";
      form = "'Zd.'t, 'Pgl/m, 'Zn.'t, 'Zm.'t";
      break;
    case MSB_z_p_zzz:
      mnemonic = "msb";
      form = "'Zd.'t, 'Pgl/m, 'Zm.'t, 'Zn.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntMulAddUnpredicated(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEIntMulAddUnpredicated)";

  if (static_cast<unsigned>(instr->GetSVESize()) >= kSRegSizeInBytesLog2) {
    form = "'Zd.'t, 'Zn.'tq, 'Zm.'tq";
    switch (instr->Mask(SVEIntMulAddUnpredicatedMask)) {
      case SDOT_z_zzz:
        mnemonic = "sdot";
        break;
      case UDOT_z_zzz:
        mnemonic = "udot";
        break;
      default:
        break;
    }
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEMovprfx(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEMovprfx)";

  if (instr->Mask(SVEMovprfxMask) == MOVPRFX_z_p_z) {
    mnemonic = "movprfx";
    form = "'Zd.'t, 'Pgl/'?16:mz, 'Zn.'t";
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntReduction(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Vdv, 'Pgl, 'Zn.'t";

  if (instr->Mask(SVEIntReductionLogicalFMask) == SVEIntReductionLogicalFixed) {
    switch (instr->Mask(SVEIntReductionLogicalMask)) {
      case ANDV_r_p_z:
        mnemonic = "andv";
        break;
      case EORV_r_p_z:
        mnemonic = "eorv";
        break;
      case ORV_r_p_z:
        mnemonic = "orv";
        break;
      default:
        break;
    }
  } else {
    switch (instr->Mask(SVEIntReductionMask)) {
      case SADDV_r_p_z:
        mnemonic = "saddv";
        form = "'Dd, 'Pgl, 'Zn.'t";
        break;
      case SMAXV_r_p_z:
        mnemonic = "smaxv";
        break;
      case SMINV_r_p_z:
        mnemonic = "sminv";
        break;
      case UADDV_r_p_z:
        mnemonic = "uaddv";
        form = "'Dd, 'Pgl, 'Zn.'t";
        break;
      case UMAXV_r_p_z:
        mnemonic = "umaxv";
        break;
      case UMINV_r_p_z:
        mnemonic = "uminv";
        break;
      default:
        break;
    }
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEIntUnaryArithmeticPredicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Pgl/m, 'Zn.'t";

  switch (instr->Mask(SVEIntUnaryArithmeticPredicatedMask)) {
    case ABS_z_p_z:
      mnemonic = "abs";
      break;
    case CLS_z_p_z:
      mnemonic = "cls";
      break;
    case CLZ_z_p_z:
      mnemonic = "clz";
      break;
    case CNOT_z_p_z:
      mnemonic = "cnot";
      break;
    case CNT_z_p_z:
      mnemonic = "cnt";
      break;
    case FABS_z_p_z:
      mnemonic = "fabs";
      break;
    case FNEG_z_p_z:
      mnemonic = "fneg";
      break;
    case NEG_z_p_z:
      mnemonic = "neg";
      break;
    case NOT_z_p_z:
      mnemonic = "not";
      break;
    case SXTB_z_p_z:
      mnemonic = "sxtb";
      break;
    case SXTH_z_p_z:
      mnemonic = "sxth";
      break;
    case SXTW_z_p_z:
      mnemonic = "sxtw";
      break;
    case UXTB_z_p_z:
      mnemonic = "uxtb";
      break;
    case UXTH_z_p_z:
      mnemonic = "uxth";
      break;
    case UXTW_z_p_z:
      mnemonic = "uxtw";
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
    case SDOT_z_zzzi_d:
      mnemonic = "sdot";
      form = "'Zd.d, 'Zn.h, z'u1916.h['u2020]";
      break;
    case SDOT_z_zzzi_s:
      mnemonic = "sdot";
      form = "'Zd.s, 'Zn.b, z'u1816.b['u2019]";
      break;
    case UDOT_z_zzzi_d:
      mnemonic = "udot";
      form = "'Zd.d, 'Zn.h, z'u1916.h['u2020]";
      break;
    case UDOT_z_zzzi_s:
      mnemonic = "udot";
      form = "'Zd.s, 'Zn.b, z'u1816.b['u2019]";
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
    case EXT_z_zi_des:
      mnemonic = "ext";
      form = "'Zd.b, 'Zd.b, 'Zn.b, #'u2016:1210";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPermuteVectorInterleaving(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Zd.'t, 'Zn.'t, 'Zm.'t";

  switch (instr->Mask(SVEPermuteVectorInterleavingMask)) {
    case TRN1_z_zz:
      mnemonic = "trn1";
      break;
    case TRN2_z_zz:
      mnemonic = "trn2";
      break;
    case UZP1_z_zz:
      mnemonic = "uzp1";
      break;
    case UZP2_z_zz:
      mnemonic = "uzp2";
      break;
    case ZIP1_z_zz:
      mnemonic = "zip1";
      break;
    case ZIP2_z_zz:
      mnemonic = "zip2";
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
    case CNTP_r_p_p:
      mnemonic = "cntp";
      form = "'Xd, p'u1310, 'Pn.'t";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPredicateLogical(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Pd.b, p'u1310/z, 'Pn.b, 'Pm.b";

  int pd = instr->GetPd();
  int pn = instr->GetPn();
  int pm = instr->GetPm();
  int pg = instr->ExtractBits(13, 10);

  switch (instr->Mask(SVEPredicateLogicalMask)) {
    case ANDS_p_p_pp_z:
      mnemonic = "ands";
      if (pn == pm) {
        mnemonic = "movs";
        form = "'Pd.b, p'u1310/z, 'Pn.b";
      }
      break;
    case AND_p_p_pp_z:
      mnemonic = "and";
      if (pn == pm) {
        mnemonic = "mov";
        form = "'Pd.b, p'u1310/z, 'Pn.b";
      }
      break;
    case BICS_p_p_pp_z:
      mnemonic = "bics";
      break;
    case BIC_p_p_pp_z:
      mnemonic = "bic";
      break;
    case EORS_p_p_pp_z:
      mnemonic = "eors";
      if (pm == pg) {
        mnemonic = "nots";
        form = "'Pd.b, 'Pm/z, 'Pn.b";
      }
      break;
    case EOR_p_p_pp_z:
      mnemonic = "eor";
      if (pm == pg) {
        mnemonic = "not";
        form = "'Pd.b, 'Pm/z, 'Pn.b";
      }
      break;
    case NANDS_p_p_pp_z:
      mnemonic = "nands";
      break;
    case NAND_p_p_pp_z:
      mnemonic = "nand";
      break;
    case NORS_p_p_pp_z:
      mnemonic = "nors";
      break;
    case NOR_p_p_pp_z:
      mnemonic = "nor";
      break;
    case ORNS_p_p_pp_z:
      mnemonic = "orns";
      break;
    case ORN_p_p_pp_z:
      mnemonic = "orn";
      break;
    case ORRS_p_p_pp_z:
      mnemonic = "orrs";
      if ((pn == pm) && (pn == pg)) {
        mnemonic = "movs";
        form = "'Pd.b, 'Pn.b";
      }
      break;
    case ORR_p_p_pp_z:
      mnemonic = "orr";
      if ((pn == pm) && (pn == pg)) {
        mnemonic = "mov";
        form = "'Pd.b, 'Pn.b";
      }
      break;
    case SEL_p_p_pp:
      if (pd == pm) {
        mnemonic = "mov";
        form = "'Pd.b, p'u1310/m, 'Pn.b";
      } else {
        mnemonic = "sel";
        form = "'Pd.b, p'u1310, 'Pn.b, 'Pm.b";
      }
      break;
    default:
      form = "(SVEPredicateLogical)";
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPredicateInitialize(const Instruction *instr) {
  // This group only contains PTRUE{S}, and there are no unallocated encodings.
  VIXL_STATIC_ASSERT(
      SVEPredicateInitializeMask ==
      (SVEPredicateInitializeFMask | SVEPredicateInitializeSetFlagsBit));
  VIXL_ASSERT((instr->Mask(SVEPredicateInitializeMask) == PTRUE_p_s) ||
              (instr->Mask(SVEPredicateInitializeMask) == PTRUES_p_s));

  const char *mnemonic = instr->ExtractBit(16) ? "ptrues" : "ptrue";
  const char *form = "'Pd.'t, 'Ipc";
  // Omit the pattern if it is the default ('ALL').
  if (instr->ExtractBits(9, 5) == SVE_ALL) form = "'Pd.'t";
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPredicateNextActive(const Instruction *instr) {
  // This group only contains PNEXT, and there are no unallocated encodings.
  VIXL_STATIC_ASSERT(SVEPredicateNextActiveFMask == SVEPredicateNextActiveMask);
  VIXL_ASSERT(instr->Mask(SVEPredicateNextActiveMask) == PNEXT_p_p_p);

  Format(instr, "pnext", "'Pd.'t, 'Pn, 'Pd.'t");
}

void Disassembler::VisitSVEPredicateReadFromFFR_Predicated(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEPredicateReadFromFFR_Predicated)";
  switch (instr->Mask(SVEPredicateReadFromFFR_PredicatedMask)) {
    case RDFFR_p_p_f:
    case RDFFRS_p_p_f:
      mnemonic = instr->ExtractBit(22) ? "rdffrs" : "rdffr";
      form = "'Pd.b, 'Pn/z";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEPropagateBreak(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Pd.b, p'u1310/z, 'Pn.b, 'Pm.b";

  switch (instr->Mask(SVEPropagateBreakMask)) {
    case BRKPAS_p_p_pp:
      mnemonic = "brkpas";
      break;
    case BRKPA_p_p_pp:
      mnemonic = "brkpa";
      break;
    case BRKPBS_p_p_pp:
      mnemonic = "brkpbs";
      break;
    case BRKPB_p_p_pp:
      mnemonic = "brkpb";
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEStackFrameAdjustment(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Xds, 'Xms, #'s1005";

  switch (instr->Mask(SVEStackFrameAdjustmentMask)) {
    case ADDPL_r_ri:
      mnemonic = "addpl";
      break;
    case ADDVL_r_ri:
      mnemonic = "addvl";
      break;
    default:
      form = "(SVEStackFrameAdjustment)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEStackFrameSize(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(SVEStackFrameSize)";

  switch (instr->Mask(SVEStackFrameSizeMask)) {
    case RDVL_r_i:
      mnemonic = "rdvl";
      form = "'Xd, #'s1005";
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
    case SEL_z_p_zz:
      if (instr->GetRd() == instr->GetRm()) {
        mnemonic = "mov";
        form = "'Zd.'t, p'u1310/m, 'Zn.'t";
      } else {
        mnemonic = "sel";
        form = "'Zd.'t, p'u1310, 'Zn.'t, 'Zm.'t";
      }
      break;
    default:
      break;
  }
  Format(instr, mnemonic, form);
}

void Disassembler::VisitSVEContiguousLoad_ScalarPlusImm(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.'tlss}, 'Pgl/z, ['Xns";
  const char *suffix =
      (instr->ExtractBits(19, 16) == 0) ? "]" : ", #'s1916, mul vl]";

  switch (instr->Mask(SVEContiguousLoad_ScalarPlusImmMask)) {
    case LD1B_z_p_bi_u16:
    case LD1B_z_p_bi_u32:
    case LD1B_z_p_bi_u64:
    case LD1B_z_p_bi_u8:
      mnemonic = "ld1b";
      break;
    case LD1D_z_p_bi_u64:
      mnemonic = "ld1d";
      break;
    case LD1H_z_p_bi_u16:
    case LD1H_z_p_bi_u32:
    case LD1H_z_p_bi_u64:
      mnemonic = "ld1h";
      break;
    case LD1SB_z_p_bi_s16:
    case LD1SB_z_p_bi_s32:
    case LD1SB_z_p_bi_s64:
      mnemonic = "ld1sb";
      break;
    case LD1SH_z_p_bi_s32:
    case LD1SH_z_p_bi_s64:
      mnemonic = "ld1sh";
      break;
    case LD1SW_z_p_bi_s64:
      mnemonic = "ld1sw";
      break;
    case LD1W_z_p_bi_u32:
    case LD1W_z_p_bi_u64:
      mnemonic = "ld1w";
      break;
    default:
      form = "(SVEContiguousLoad_ScalarPlusImm)";
      suffix = NULL;
      break;
  }

  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitSVEContiguousLoad_ScalarPlusScalar(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "{'Zt.'tlss}, 'Pgl/z, ['Xns, 'Xm";
  const char *suffix = NULL;

  switch (instr->Mask(SVEContiguousLoad_ScalarPlusScalarMask)) {
    case LD1B_z_p_br_u16:
    case LD1B_z_p_br_u32:
    case LD1B_z_p_br_u64:
    case LD1B_z_p_br_u8:
      mnemonic = "ld1b";
      suffix = "]";
      break;
    case LD1D_z_p_br_u64:
      mnemonic = "ld1d";
      suffix = ", lsl #'u2423]";
      break;
    case LD1H_z_p_br_u16:
    case LD1H_z_p_br_u32:
    case LD1H_z_p_br_u64:
      mnemonic = "ld1h";
      suffix = ", lsl #'u2423]";
      break;
    case LD1SB_z_p_br_s16:
    case LD1SB_z_p_br_s32:
    case LD1SB_z_p_br_s64:
      mnemonic = "ld1sb";
      suffix = "]";
      break;
    case LD1SH_z_p_br_s32:
    case LD1SH_z_p_br_s64:
      mnemonic = "ld1sh";
      suffix = ", lsl #1]";
      break;
    case LD1SW_z_p_br_s64:
      mnemonic = "ld1sw";
      suffix = ", lsl #2]";
      break;
    case LD1W_z_p_br_u32:
    case LD1W_z_p_br_u64:
      mnemonic = "ld1w";
      suffix = ", lsl #'u2423]";
      break;
    default:
      form = "(SVEContiguousLoad_ScalarPlusScalar)";
      suffix = NULL;
      break;
  }

  Format(instr, mnemonic, form, suffix);
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


void Disassembler::VisitMorelloBitwise(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cds, 'cns, 'Xm";

  switch (instr->Mask(MorelloBitwiseMask)) {
    case BICFLGS_c_cr:
      mnemonic = "bicflgs";
      break;
    case ORRFLGS_c_cr:
      mnemonic = "orrflgs";
      break;
    case EORFLGS_c_cr:
      mnemonic = "eorflgs";
      break;
    case CTHI_c_cr:
      mnemonic = "cthi";
      // <Cn> cannot be CSP for `cthi`.
      form = "'cds, 'cn, 'Xm";
      break;
    default:
      form = "(MorelloBitwise)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloBranchBx(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(MorelloBranchBx)";

  switch (instr->Mask(MorelloBranchBxMask)) {
    case BX:
      mnemonic = "bx";
      form = "#4";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorello2SrcCap(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(Morello2SrcCap)";

  switch (instr->Mask(Morello2SrcCapMask)) {
    case CHKEQ_cc:
      mnemonic = "chkeq";
      form = "'cns, 'cm";
      break;
    case CHKSS_cc:
      mnemonic = "chkss";
      form = "'cns, 'cms";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorello1Src1Dst(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Xd, 'Xn";

  switch (instr->Mask(Morello1Src1DstMask)) {
    case RRLEN_r_r:
      mnemonic = "rrlen";
      break;
    case RRMASK_r_r:
      mnemonic = "rrmask";
      break;
    default:
      form = "(Morello1Src1Dst)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloADD(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(MorelloADD)";

  switch (instr->Mask(MorelloADDMask)) {
    case ADD_c_cri:
      mnemonic = "add";
      // Note that '<extend>' is mandatory, and Rm is always an X register.
      form = "'cds, 'cns, 'Xm'ExtC";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloAddSubCap(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cds, 'cns, 'IAddSub";

  switch (instr->Mask(MorelloAddSubCapMask)) {
    case ADD_c_cis:
      mnemonic = "add";
      break;
    case SUB_c_cis:
      mnemonic = "sub";
      break;
    default:
      form = "(MorelloAddSubCap)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloAlignment(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cds, 'cns, #'u2015";

  switch (instr->Mask(MorelloAlignmentMask)) {
    case ALIGND_c_ci:
      mnemonic = "alignd";
      break;
    case ALIGNU_c_ci:
      mnemonic = "alignu";
      break;
    default:
      form = "(MorelloAlignment)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloBranch(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cn";

  switch (instr->Mask(MorelloBranchMask)) {
    case BR_c:
      mnemonic = "br";
      break;
    case BLR_c:
      mnemonic = "blr";
      break;
    case RET_c:
      mnemonic = "ret";
      break;
    default:
      // `bx #4` is handled by VisitMorelloBranchBx.
      form = "(MorelloBranch)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloBranchRestricted(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cn";

  switch (instr->Mask(MorelloBranchRestrictedMask)) {
    case BRR_c:
      mnemonic = "brr";
      break;
    case BLRR_c:
      mnemonic = "blrr";
      break;
    case RETR_c:
      mnemonic = "retr";
      break;
    default:
      form = "(MorelloBranchRestricted)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloBranchSealedDirect(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cn";

  switch (instr->Mask(MorelloBranchSealedDirectMask)) {
    case BRS_c:
      mnemonic = "brs";
      break;
    case BLRS_c:
      mnemonic = "blrs";
      break;
    case RETS_c:
      mnemonic = "rets";
      break;
    default:
      form = "(MorelloBranchSealedDirect)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloBranchSealedIndirect(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  // The immediate is mandatory.
  const char *form = "['cns, #'s1913*16]";

  switch (instr->Mask(MorelloBranchSealedIndirectMask)) {
    case BR_ci:
      mnemonic = "br";
      break;
    case BLR_ci:
      mnemonic = "blr";
      break;
    default:
      form = "(MorelloBranchSealedIndirect)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloBranchToSealed(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "c29, 'cn, 'cm";

  switch (instr->Mask(MorelloBranchToSealedMask)) {
    case BRS_c_c:
      mnemonic = "brs";
      break;
    case BLRS_c_c:
      mnemonic = "blrs";
      break;
    case RETS_c_c:
      mnemonic = "rets";
      break;
    default:
      form = "(MorelloBranchToSealed)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloChecks(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cns";

  switch (instr->Mask(MorelloChecksMask)) {
    case CHKSLD_c:
      mnemonic = "chksld";
      break;
    case CHKTGD_c:
      mnemonic = "chktgd";
      break;
    default:
      form = "(MorelloChecks)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloCLRPERMImm(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cds, 'cns, ";
  const char *suffix = "";

  const char *perm_names[] = {"#0", "X", "W", "WX", "R", "RX", "RW", "RWX"};
  uint32_t perm = instr->ExtractBits(15, 13);
  VIXL_ASSERT(perm <= ArrayLength(perm_names));

  switch (instr->Mask(MorelloCLRPERMImmMask)) {
    case CLRPERM_c_ci:
      mnemonic = "clrperm";
      suffix = perm_names[perm];
      break;
    default:
      form = "(MorelloCLRPERMImm)";
      break;
  }

  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitMorelloCompareAndSwap(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cs, 'ct, ['nns]";

  switch (instr->Mask(MorelloCompareAndSwapMask)) {
    case CAS_c_r:
      mnemonic = "cas";
      break;
    case CASA_c_r:
      mnemonic = "casa";
      break;
    case CASAL_c_r:
      mnemonic = "casal";
      break;
    case CASL_c_r:
      mnemonic = "casl";
      break;
    default:
      form = "(MorelloCompareAndSwap)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloConvertToCap(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cd, 'cns, 'Xm";

  switch (instr->Mask(MorelloConvertToCapMask)) {
    case CVT_c_cr:
      mnemonic = "cvt";
      break;
    case CVTZ_c_cr:
      mnemonic = "cvtz";
      break;
    default:
      form = "(MorelloConvertToCap)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloConvertToCapWithImplicitOperand(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cd, 'Xn";

  switch (instr->Mask(MorelloConvertToCapWithImplicitOperandMask)) {
    case CVTD_c_r:
      mnemonic = "cvtd";
      break;
    case CVTDZ_c_r:
      mnemonic = "cvtdz";
      break;
    case CVTP_c_r:
      mnemonic = "cvtp";
      break;
    case CVTPZ_c_r:
      mnemonic = "cvtpz";
      break;
    default:
      form = "(MorelloConvertToCapWithImplicitOperand)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloConvertToPointer(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Xd, 'cns";

  switch (instr->Mask(MorelloConvertToPointerMask)) {
    case CVTD_r_c:
      mnemonic = "cvtd";
      break;
    case CVTP_r_c:
      mnemonic = "cvtp";
      break;
    default:
      form = "(MorelloConvertToPointer)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloCVT(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Xd, 'cns, 'cm";

  switch (instr->Mask(MorelloCVTMask)) {
    case CVT_r_cc:
      mnemonic = "cvt";
      break;
    default:
      form = "(MorelloCVT)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloCSEL(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cd, 'cn, 'cm, 'Cond";

  switch (instr->Mask(MorelloCSELMask)) {
    case CSEL_c_ci:
      mnemonic = "csel";
      break;
    default:
      form = "(MorelloCSEL)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloGetField1(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Xd, 'cns";

  switch (instr->Mask(MorelloGetField1Mask)) {
    case GCBASE_r_c:
      mnemonic = "gcbase";
      break;
    case GCLEN_r_c:
      mnemonic = "gclen";
      break;
    case GCVALUE_r_c:
      mnemonic = "gcvalue";
      break;
    case GCOFF_r_c:
      mnemonic = "gcoff";
      break;
    case GCTAG:
      mnemonic = "gctag";
      break;
    case GCSEAL_r_c:
      mnemonic = "gcseal";
      break;
    case GCPERM_r_c:
      mnemonic = "gcperm";
      break;
    case GCTYPE_r_c:
      mnemonic = "gctype";
      break;
    default:
      form = "(MorelloGetField1)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloGetField2(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Xd, 'cns";

  switch (instr->Mask(MorelloGetField2Mask)) {
    case GCLIM_r_c:
      mnemonic = "gclim";
      break;
    case GCFLGS_r_c:
      mnemonic = "gcflgs";
      break;
    case CFHI_r_c:
      mnemonic = "cfhi";
      break;
    default:
      form = "(MorelloGetField2)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloGetSetSystemRegister(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(MorelloGetSetSystemRegister)";

  switch (instr->Mask(MorelloGetSetSystemRegisterMask)) {
    case MRS_c_i:
      mnemonic = "mrs";
      form = "'ct, 'IZ";
      break;
    case MSR_c_i:
      mnemonic = "msr";
      form = "'IZ, 'ct";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloImmBounds(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(MorelloImmBounds)";

  switch (instr->Mask(MorelloImmBoundsMask)) {
    case SCBNDS_c_ci_c:
      mnemonic = "scbnds";
      form = "'cds, 'cns, #'u2015";
      break;
    case SCBNDS_c_ci_s:
      mnemonic = "scbnds";
      form = "'cds, 'cns, #'u2015, lsl #4 ('u2015*16)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLDAPR(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, ['nns]";

  switch (instr->Mask(MorelloLDAPRMask)) {
    case LDAPR_c_r:
      mnemonic = "ldapr";
      break;
    default:
      form = "(MorelloLDAPR)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLDR(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, 'ILCLiteral 'LCValue";

  switch (instr->Mask(MorelloLDRMask)) {
    case LDR_c_i:
      mnemonic = "ldr";
      break;
    default:
      form = "(MorelloLDR)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadExclusive(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, ['nns]";

  switch (instr->Mask(MorelloLoadExclusiveMask)) {
    case LDAXR_c_r:
      mnemonic = "ldaxr";
      break;
    case LDXR_c_r:
      mnemonic = "ldxr";
      break;
    default:
      form = "(MorelloLoadExclusive)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStoreAcquireReleaseAltBase(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Wt, ['ans]";

  switch (instr->Mask(MorelloLoadStoreAcquireReleaseAltBaseMask)) {
    case ALDAR_r_r:
      mnemonic = "ldar";
      break;
    case ALDARB_r_r:
      mnemonic = "ldarb";
      break;
    case ASTLR_r_r:
      mnemonic = "stlr";
      break;
    case ASTLRB_r_r:
      mnemonic = "stlrb";
      break;
    default:
      form = "(MorelloLoadStoreAcquireReleaseAltBase)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStoreAcquireReleaseCapAltBase(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, ['ans]";

  switch (instr->Mask(MorelloLoadStoreAcquireReleaseCapAltBaseMask)) {
    case ALDAR_c_r:
      mnemonic = "ldar";
      break;
    case ASTLR_c_r:
      mnemonic = "stlr";
      break;
    default:
      form = "(MorelloLoadStoreAcquireReleaseCapAltBase)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStoreAcquireRelease(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, ['nns]";

  switch (instr->Mask(MorelloLoadStoreAcquireReleaseMask)) {
    case LDAR_c_r:
      mnemonic = "ldar";
      break;
    case STLR_c_r:
      mnemonic = "stlr";
      break;
    default:
      form = "(MorelloLoadStoreAcquireRelease)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStoreCapAltBase(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, ['ans, 'Offsetregc]";

  switch (instr->Mask(MorelloLoadStoreCapAltBaseMask)) {
    case ALDR_c_rrb:
      mnemonic = "ldr";
      break;
    case ASTR_c_rrb:
      mnemonic = "str";
      break;
    default:
      form = "(MorelloLoadStoreCapAltBase)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStoreImmediatePostIndex(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, ['nns], #'s2012*16";

  switch (instr->Mask(MorelloLoadStoreImmediatePostIndexMask)) {
    case LDR_c_riaw:
      mnemonic = "ldr";
      break;
    case STR_c_riaw:
      mnemonic = "str";
      break;
    default:
      form = "(MorelloLoadStoreImmediatePostIndex)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStoreImmediatePreIndex(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, ['nns, #'s2012*16]!";

  switch (instr->Mask(MorelloLoadStoreImmediatePreIndexMask)) {
    case LDR_c_ribw:
      mnemonic = "ldr";
      break;
    case STR_c_ribw:
      mnemonic = "str";
      break;
    default:
      form = "(MorelloLoadStoreImmediatePreIndex)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStoreTags(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Xt, ['nns]";

  switch (instr->Mask(MorelloLoadStoreTagsMask)) {
    case STCT_r_r:
      mnemonic = "stct";
      break;
    case LDCT_r_r:
      mnemonic = "ldct";
      break;
    default:
      form = "(MorelloLoadStoreTags)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadPairAndBranch(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, ['cns]";

  switch (instr->Mask(MorelloLoadPairAndBranchMask)) {
    case LDPBLR_c_c:
      mnemonic = "ldpblr";
      break;
    case LDPBR_c_c:
      mnemonic = "ldpbr";
      break;
    default:
      form = "(MorelloLoadPairAndBranch)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadPairExclusive(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, 'ct2, ['nns]";

  switch (instr->Mask(MorelloLoadPairExclusiveMask)) {
    case LDXP_c_r:
      mnemonic = "ldxp";
      break;
    case LDAXP_c_r:
      mnemonic = "ldaxp";
      break;
    default:
      form = "(MorelloLoadPairExclusive)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStorePair(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, 'ct2, ['nns'ILP4]";

  switch (instr->Mask(MorelloLoadStorePairMask)) {
    case LDP_c_rib:
      mnemonic = "ldp";
      break;
    case STP_c_rib:
      mnemonic = "stp";
      break;
    default:
      form = "(MorelloLoadStorePair)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStorePairNonTemporal(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, 'ct2, ['nns'ILP4]";

  switch (instr->Mask(MorelloLoadStorePairNonTemporalMask)) {
    case LDNP_c_rib:
      mnemonic = "ldnp";
      break;
    case STNP_c_rib:
      mnemonic = "stnp";
      break;
    default:
      form = "(MorelloLoadStorePairNonTemporal)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStorePairPostIndex(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, 'ct2, ['nns]'ILP4i";

  switch (instr->Mask(MorelloLoadStorePairPostIndexMask)) {
    case LDP_cc_riaw:
      mnemonic = "ldp";
      break;
    case STP_cc_riaw:
      mnemonic = "stp";
      break;
    default:
      form = "(MorelloLoadStorePairPostIndex)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStorePairPreIndex(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, 'ct2, ['nns'ILP4i]!";

  switch (instr->Mask(MorelloLoadStorePairPreIndexMask)) {
    case LDP_c_ribw:
      mnemonic = "ldp";
      break;
    case STP_c_ribw:
      mnemonic = "stp";
      break;
    default:
      form = "(MorelloLoadStorePairPreIndex)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStoreRegister(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, ['nns, 'Offsetregc]";

  switch (instr->Mask(MorelloLoadStoreRegisterMask)) {
    case LDR_c_rrb:
      mnemonic = "ldr";
      break;
    case STR_c_rrb:
      mnemonic = "str";
      break;
    default:
      form = "(MorelloLoadStoreRegister)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStoreRegisterAltBase(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(MorelloLoadStoreRegisterAltBase)";

  switch (instr->Mask(MorelloLoadStoreRegisterAltBaseMask)) {
    case ASTRB_r_rrb:
      mnemonic = "strb";
      form = "'Wt, ['ans, 'Offsetregb]";
      break;
    case ALDRSB_r_rrb_64:
      mnemonic = "ldrsb";
      form = "'Xt, ['ans, 'Offsetregb]";
      break;
    case ALDRSH_r_rrb_64:
      mnemonic = "ldrsh";
      form = "'Xt, ['ans, 'Offsetregh]";
      break;
    case ASTRH_r_rrb:
      mnemonic = "strh";
      form = "'Wt, ['ans, 'Offsetregh]";
      break;
    case ASTR_r_rrb_32:
      mnemonic = "str";
      form = "'Wt, ['ans, 'Offsetregw]";
      break;
    case ASTR_r_rrb_64:
      mnemonic = "str";
      form = "'Xt, ['ans, 'Offsetregx]";
      break;
    case ASTR_v_rrb_d:
      mnemonic = "str";
      form = "'Dt, ['ans, 'Offsetregd]";
      break;
    case ASTR_v_rrb_s:
      mnemonic = "str";
      form = "'St, ['ans, 'Offsetregs]";
      break;
    case ALDRB_r_rrb:
      mnemonic = "ldrb";
      form = "'Wt, ['ans, 'Offsetregb]";
      break;
    case ALDRSB_r_rrb_32:
      mnemonic = "ldrsb";
      form = "'Wt, ['ans, 'Offsetregb]";
      break;
    case ALDRSH_r_rrb_32:
      mnemonic = "ldrsh";
      form = "'Wt, ['ans, 'Offsetregh]";
      break;
    case ALDRH_r_rrb:
      mnemonic = "ldrh";
      form = "'Wt, ['ans, 'Offsetregh]";
      break;
    case ALDR_r_rrb_32:
      mnemonic = "ldr";
      form = "'Wt, ['ans, 'Offsetregw]";
      break;
    case ALDR_r_rrb_64:
      mnemonic = "ldr";
      form = "'Xt, ['ans, 'Offsetregx]";
      break;
    case ALDR_v_rrb_d:
      mnemonic = "ldr";
      form = "'Dt, ['ans, 'Offsetregd]";
      break;
    case ALDR_v_rrb_s:
      mnemonic = "ldr";
      form = "'St, ['ans, 'Offsetregs]";
      break;
    default:
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStoreUnscaledImmediate(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, ['nns'ILS]";

  switch (instr->Mask(MorelloLoadStoreUnscaledImmediateMask)) {
    case LDUR_c_ri:
      mnemonic = "ldur";
      break;
    case STUR_c_ri:
      mnemonic = "stur";
      break;
    default:
      form = "(MorelloLoadStoreUnscaledImmediate)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStoreUnscaledImmediateAltBase(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(MorelloLoadStoreUnscaledImmediateAltBase)";
  const char *suffix = ", ['ans'ILS]";

  switch (instr->Mask(MorelloLoadStoreUnscaledImmediateAltBaseMask)) {
    case ASTURB_r_ri:
      mnemonic = "sturb";
      form = "'Wt";
      break;
    case ALDURB_r_ri:
      mnemonic = "ldurb";
      form = "'Wt";
      break;
    case ALDURSB_r_ri_64:
      mnemonic = "ldursb";
      form = "'Xt";
      break;
    case ALDURSB_r_ri_32:
      mnemonic = "ldursb";
      form = "'Wt";
      break;
    case ASTUR_v_ri_b:
      mnemonic = "stur";
      form = "'Bt";
      break;
    case ALDUR_v_ri_b:
      mnemonic = "ldur";
      form = "'Bt";
      break;
    case ASTUR_v_ri_q:
      mnemonic = "stur";
      form = "'Qt";
      break;
    case ALDUR_v_ri_q:
      mnemonic = "ldur";
      form = "'Qt";
      break;
    case ASTURH_r_ri:
      mnemonic = "sturh";
      form = "'Wt";
      break;
    case ALDURH_r_ri:
      mnemonic = "ldurh";
      form = "'Wt";
      break;
    case ALDURSH_r_ri_64:
      mnemonic = "ldursh";
      form = "'Xt";
      break;
    case ALDURSH_r_ri_32:
      mnemonic = "ldursh";
      form = "'Wt";
      break;
    case ASTUR_v_ri_h:
      mnemonic = "stur";
      form = "'Ht";
      break;
    case ALDUR_v_ri_h:
      mnemonic = "ldur";
      form = "'Ht";
      break;
    case ASTUR_r_ri_32:
      mnemonic = "stur";
      form = "'Wt";
      break;
    case ALDUR_r_ri_32:
      mnemonic = "ldur";
      form = "'Wt";
      break;
    case ALDURSW_r_ri:
      mnemonic = "ldursw";
      form = "'Xt";
      break;
    case ASTUR_c_ri:
      mnemonic = "stur";
      form = "'ct";
      break;
    case ASTUR_v_ri_s:
      mnemonic = "stur";
      form = "'St";
      break;
    case ALDUR_v_ri_s:
      mnemonic = "ldur";
      form = "'St";
      break;
    case ASTUR_r_ri_64:
      mnemonic = "stur";
      form = "'Xt";
      break;
    case ALDUR_r_ri_64:
      mnemonic = "ldur";
      form = "'Xt";
      break;
    case ALDUR_c_ri:
      mnemonic = "ldur";
      form = "'ct";
      break;
    case ASTUR_v_ri_d:
      mnemonic = "stur";
      form = "'Dt";
      break;
    case ALDUR_v_ri_d:
      mnemonic = "ldur";
      form = "'Dt";
      break;
    default:
      suffix = nullptr;
      break;
  }

  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitMorelloLoadStoreUnsignedOffset(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'ct, ['nns'ILUc]";

  switch (instr->Mask(MorelloLoadStoreUnsignedOffsetMask)) {
    case LDR_c_rib:
      mnemonic = "ldr";
      break;
    case STR_c_rib:
      mnemonic = "str";
      break;
    default:
      form = "(MorelloLoadStoreUnsignedOffset)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLoadStoreUnsignedOffsetAltBase(
    const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(MorelloLoadStoreUnsignedOffsetAltBase)";

  bool zero = instr->GetImmLS() == 0;
  const char *form_c = zero ? "'ct, ['ans]" : "'ct, ['ans, #'u2012*16]";
  const char *form_x = zero ? "'Xt, ['ans]" : "'Xt, ['ans, #'u2012*8]";
  const char *form_w = zero ? "'Wt, ['ans]" : "'Wt, ['ans, #'u2012*4]";
  const char *form_b = zero ? "'Wt, ['ans]" : "'Wt, ['ans, #'u2012]";

  switch (instr->Mask(MorelloLoadStoreUnsignedOffsetAltBaseMask)) {
    case ALDR_c_ri:
      mnemonic = "ldr";
      form = form_c;
      break;
    case ASTR_c_ri:
      mnemonic = "str";
      form = form_c;
      break;
    case ASTR_r_ri_32:
      mnemonic = "str";
      form = form_w;
      break;
    case ASTR_r_ri_64:
      mnemonic = "str";
      form = form_x;
      break;
    case ALDR_r_ri_32:
      mnemonic = "ldr";
      form = form_w;
      break;
    case ALDR_r_ri_64:
      mnemonic = "ldr";
      form = form_x;
      break;
    case ASTRB_r_ri:
      mnemonic = "strb";
      form = form_b;
      break;
    case ALDRB_r_ri:
      mnemonic = "ldrb";
      form = form_b;
      break;
    default:
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloLogicalImm(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cds, 'cns, #'x2013";

  switch (instr->Mask(MorelloLogicalImmMask)) {
    case BICFLGS_c_ci:
      mnemonic = "bicflgs";
      break;
    case EORFLGS_c_ci:
      mnemonic = "eorflgs";
      break;
    case ORRFLGS_c_ci:
      mnemonic = "orrflgs";
      break;
    default:
      form = "(MorelloLogicalImm)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloMiscCap0(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cds, 'cns";

  switch (instr->Mask(MorelloMiscCap0Mask)) {
    case CPY_c_c:
      // `cpy` always disassembles to `mov`.
      mnemonic = "mov";
      break;
    case CLRTAG_c_c:
      mnemonic = "clrtag";
      break;
    default:
      form = "(MorelloMiscCap0)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloMiscCap1(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(MorelloMiscCap1)";

  const char *form_zr = "'cd, 'cn, 'cm";
  const char *form_sp = "'cds, 'cns, 'cms";

  switch (instr->Mask(MorelloMiscCap1Mask)) {
    case BUILD_c_c:
      mnemonic = "build";
      form = form_sp;
      break;
    case CPYTYPE_c_c:
      mnemonic = "cpytype";
      form = form_zr;
      break;
    case CSEAL_c_c:
      mnemonic = "cseal";
      form = form_sp;
      break;
    case CPYVALUE_c_c:
      mnemonic = "cpyvalue";
      form = form_zr;
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloMiscCap2(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cd, 'cn, 'cm";

  switch (instr->Mask(MorelloMiscCap2Mask)) {
    case SEAL_c_cc:
      mnemonic = "seal";
      break;
    case UNSEAL_c_cc:
      mnemonic = "unseal";
      break;
    case CHKSSU_c_cc:
      mnemonic = "chkssu";
      // `chkssu` can take SP as either input.
      form = "'cd, 'cns, 'cms";
      break;
    default:
      form = "(MorelloMiscCap2)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloSCFLGS(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cds, 'cns, 'Xm";

  switch (instr->Mask(MorelloSCFLGSMask)) {
    case SCFLGS_c_cr:
      mnemonic = "scflgs";
      break;
    default:
      form = "(MorelloSCFLGS)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloSEAL(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cd, 'cn, ";
  const char *suffix = "";

  const char *seal_form_names[] = {"RESERVED", "rb", "lpb", "lb"};
  uint32_t seal_form = instr->ExtractBits(14, 13);
  VIXL_ASSERT(seal_form <= ArrayLength(seal_form_names));

  switch (instr->Mask(MorelloSEALMask)) {
    case SEAL_c_ci:
      mnemonic = "seal";
      suffix = seal_form_names[seal_form];
      break;
    default:
      form = "(MorelloSEAL)";
      break;
  }

  Format(instr, mnemonic, form, suffix);
}

void Disassembler::VisitMorelloSetField1(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cds, 'cns, 'Xm";

  switch (instr->Mask(MorelloSetField1Mask)) {
    case SCBNDS_c_cr:
      mnemonic = "scbnds";
      break;
    case SCBNDSE_c_cr:
      mnemonic = "scbndse";
      break;
    case SCVALUE_c_cr:
      mnemonic = "scvalue";
      break;
    case SCOFF_c_cr:
      mnemonic = "scoff";
      break;
    default:
      form = "(MorelloSetField1)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloSetField2(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cds, 'cns, 'Xm";

  switch (instr->Mask(MorelloSetField2Mask)) {
    case SCTAG_c_cr:
      mnemonic = "sctag";
      break;
    case CLRPERM_c_cr:
      mnemonic = "clrperm";
      break;
    default:
      form = "(MorelloSetField2)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloStoreExclusive(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Ws, 'ct, ['nns]";

  switch (instr->Mask(MorelloStoreExclusiveMask)) {
    case STLXR_r_cr:
      mnemonic = "stlxr";
      break;
    case STXR_r_cr:
      mnemonic = "stxr";
      break;
    default:
      form = "(MorelloStoreExclusive)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloStorePairExclusive(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'Ws, 'ct, 'ct2, ['nns]";

  switch (instr->Mask(MorelloStorePairExclusiveMask)) {
    case STXP_r_cr:
      mnemonic = "stxp";
      break;
    case STLXP_r_cr:
      mnemonic = "stlxp";
      break;
    default:
      form = "(MorelloStorePairExclusive)";
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloSUBS(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "(MorelloSUBS)";

  switch (instr->Mask(MorelloSUBSMask)) {
    case SUBS_r_cc:
      if (instr->GetRd() == kZeroRegCode) {
        mnemonic = "cmp";
        form = "'cn, 'cm";
      } else {
        mnemonic = "subs";
        form = "'Xd, 'cn, 'cm";
      }
      break;
  }

  Format(instr, mnemonic, form);
}

void Disassembler::VisitMorelloSwap(const Instruction *instr) {
  const char *mnemonic = "unimplemented";
  const char *form = "'cs, 'ct, ['nns]";

  switch (instr->Mask(MorelloSwapMask)) {
    case SWP_cc_r:
      mnemonic = "swp";
      break;
    case SWPA_cc_r:
      mnemonic = "swpa";
      break;
    case SWPAL_cc_r:
      mnemonic = "swpal";
      break;
    case SWPL_cc_r:
      mnemonic = "swpl";
      break;
    default:
      form = "(MorelloSwap)";
      break;
  }

  Format(instr, mnemonic, form);
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
  } else if (reg.IsCRegister()) {
    reg_char = 'c';
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

  if (reg.IsVRegister() || ((reg.IsRegister() || reg.IsCRegister()) &&
                            !(reg.IsZero() || reg.IsSP()))) {
    // A core or scalar/vector register: [wxc]0 - 30, [bhsdq]0 - 31.
    AppendToOutput("%c%d", reg_char, reg.GetCode());
  } else if (reg.Aliases(sp)) {
    // Disassemble w31/x31/c31 as stack pointer wsp/sp/csp.
    if (reg.Is64Bits()) {
      AppendToOutput("sp");
    } else {
      AppendToOutput("%csp", reg_char);
    }
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


void Disassembler::AppendSignedAddressToOutput(const Instruction *instr,
                                               int64_t addr) {
  USE(instr);
  if (addr >= 0) {
    AppendToOutput("(addr 0x%" PRIx64 ")", addr);
  } else {
    AppendToOutput("(addr -0x%" PRIx64 ")", -static_cast<uint64_t>(addr));
  }
}


void Disassembler::AppendSignedCodeAddressToOutput(const Instruction *instr,
                                                   int64_t addr) {
  AppendSignedAddressToOutput(instr, addr);
}


void Disassembler::AppendSignedDataAddressToOutput(const Instruction *instr,
                                                   int64_t addr) {
  AppendSignedAddressToOutput(instr, addr);
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
  AppendSignedDataAddressToOutput(instr, CodeRelativeAddress(addr));
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
  set_code_address_offset(
      static_cast<ptraddr_t>(base_address) -
          WrapPointer(instr_address).GetAddress());
}
int64_t Disassembler::CodeRelativeAddress(const void *addr) {
  return RawbitsToInt64(WrapPointer(addr).GetAddress() + code_address_offset());
}


void Disassembler::Format(const Instruction *instr,
                          const char *mnemonic,
                          const char *format0,
                          const char *format1) {
  VIXL_ASSERT(mnemonic != NULL);
  ResetOutput();
  Substitute(instr, mnemonic);
  if (format0 != NULL) {
    VIXL_ASSERT(buffer_pos_ < buffer_size_);
    buffer_[buffer_pos_++] = ' ';
    Substitute(instr, format0);
    if (format1 != NULL) {
      Substitute(instr, format1);
    }
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
    // NB. The remaining substitution prefix upper-case characters are: JU.
    case 'R':  // Register. X or W, selected by sf (or alternative) bit.
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
    case 'c':  // Capability registers.
    case 'n':  // Native base: 'X in A64, 'c in C64.
    case 'a':  // Alt base: 'c in A64, 'X in C64.
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
      return SubstituteRelAddressField(instr, format);
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
    case 's':
    case 'x':
      return SubstituteIntField(instr, format);
    case 't':
      return SubstituteSVESize(instr, format);
    case '?':
      return SubstituteTernary(instr, format);
    default: {
      VIXL_UNREACHABLE();
      return 1;
    }
  }
}

std::pair<unsigned, unsigned> Disassembler::GetRegNumForField(
    const Instruction *instr, char reg_prefix, const char *field) {
  unsigned reg_num = UINT_MAX;
  unsigned field_len = 1;

  switch (field[0]) {
    case 'd':
      reg_num = instr->GetRd();
      break;
    case 'n':
      reg_num = instr->GetRn();
      break;
    case 'm':
      reg_num = instr->GetRm();
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
      break;
    default:
      VIXL_UNREACHABLE();
  }

  switch (field[1]) {
    case '2':
    case '3':
    case '4':
      if ((reg_prefix == 'V') || (reg_prefix == 'Z')) {  // Vt2/3/4, Zt2/3/4
        VIXL_ASSERT(field[0] == 't');
        reg_num = (reg_num + field[1] - '1') % 32;
        field_len++;
      } else {
        VIXL_ASSERT((field[0] == 't') && (field[1] == '2'));
        reg_num = instr->GetRt2();
        field_len++;
      }
      break;
    case '+':  // Rt+, Rs+ (ie. Rt + 1, Rs + 1)
      VIXL_ASSERT((reg_prefix == 'W') || (reg_prefix == 'X'));
      VIXL_ASSERT((field[0] == 's') || (field[0] == 't'));
      reg_num++;
      field_len++;
      break;
    case 's':  // Core registers that are (w)sp rather than zr.
      VIXL_ASSERT((reg_prefix == 'W') || (reg_prefix == 'X') ||
                  (reg_prefix == 'C'));
      reg_num = (reg_num == kZeroRegCode) ? kSPRegInternalCode : reg_num;
      field_len++;
      break;
  }

  VIXL_ASSERT(reg_num != UINT_MAX);
  return std::make_pair(reg_num, field_len);
}

int Disassembler::SubstituteRegisterField(const Instruction *instr,
                                          const char *format) {
  unsigned field_len = 1;  // Initially, count only the first character.

  // The first character of the register format field, eg R, X, S, etc.
  char reg_prefix = format[0];

  // Pointer to the character after the prefix. This may be one of the standard
  // symbols representing a register encoding, or a two digit bit position,
  // handled by the following code.
  const char *reg_field = &format[1];

  if (reg_prefix == 'R') {
    bool is_x = instr->GetSixtyFourBits();
    if (strspn(reg_field, "0123456789") == 2) {  // r20d, r31n, etc.
      // Core W or X registers where the type is determined by a specified bit
      // position, eg. 'R20d, 'R05n. This is like the 'Rd syntax, where bit 31
      // is implicitly used to select between W and X.
      int bitpos = ((reg_field[0] - '0') * 10) + (reg_field[1] - '0');
      VIXL_ASSERT(bitpos <= 31);
      is_x = (instr->ExtractBit(bitpos) == 1);
      reg_field = &format[3];
      field_len += 2;
    }
    reg_prefix = is_x ? 'X' : 'W';
  }

  if (reg_prefix == 'c') reg_prefix = 'C';
  if (reg_prefix == 'n') {
    reg_prefix = (GetISA() == ISA::C64) ? 'C' : 'X';
  }
  if (reg_prefix == 'a') {
    reg_prefix = (GetISA() == ISA::C64) ? 'X' : 'C';
  }

  std::pair<unsigned, unsigned> rn =
      GetRegNumForField(instr, reg_prefix, reg_field);
  unsigned reg_num = rn.first;
  field_len += rn.second;

  if (reg_field[0] == 'm') {
    switch (reg_field[1]) {
      // Handle registers tagged with b (bytes), z (instruction), or
      // r (registers), used for address updates in NEON load/store
      // instructions.
      case 'r':
      case 'b':
      case 'z': {
        VIXL_ASSERT(reg_prefix == 'X');
        field_len = 3;
        char *eimm;
        int imm = static_cast<int>(strtol(&reg_field[2], &eimm, 10));
        field_len += eimm - &reg_field[2];
        if (reg_num == 31) {
          switch (reg_field[1]) {
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
  }

  CPURegister::RegisterType reg_type = CPURegister::kRegister;
  unsigned reg_size = kXRegSize;

  if (reg_prefix == 'F') {
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
    case 'C':
      reg_type = CPURegister::kCRegister;
      reg_size = kCRegSize;
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
      if (reg_field[1] == 'v') {
        reg_type = CPURegister::kVRegister;
        reg_size = 1 << (instr->GetSVESize() + 3);
        field_len++;
        break;
      }
      AppendToOutput("v%d", reg_num);
      return field_len;
    case 'Z':
      AppendToOutput("z%d", reg_num);
      return field_len;
    default:
      VIXL_UNREACHABLE();
  }

  AppendRegisterNameToOutput(instr, CPURegister(reg_num, reg_size, reg_type));

  return field_len;
}

int Disassembler::SubstitutePredicateRegisterField(const Instruction *instr,
                                                   const char *format) {
  VIXL_ASSERT(format[0] == 'P');
  switch (format[1]) {
    // This field only supports P register that are always encoded in the same
    // position.
    case 'd':
    case 't':
      AppendToOutput("p%u", instr->GetPt());
      break;
    case 'n':
      AppendToOutput("p%u", instr->GetPn());
      break;
    case 'm':
      AppendToOutput("p%u", instr->GetPm());
      break;
    case 'g':
      VIXL_ASSERT(format[2] == 'l');
      AppendToOutput("p%u", instr->GetPgLow8());
      return 3;
    default:
      VIXL_UNREACHABLE();
  }
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
        case 'C': {  // ILCLiteral - Immediate Load Literal (capability).
          // The offset is applied to AlignDown(pc, 16), but we disassemble such
          // that `pcc + #imm` refers to the exact address loaded.
          uint64_t pc = CodeRelativeAddress(instr);
          uint64_t target = instr->GetLiteralAddress(pc);
          AppendToOutput("pcc%+" PRId64, RawbitsToInt64(target - pc));
          return 10;
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
                     // ILUc - As above, but a capability access.
          bool is_cap = format[3] == 'c';
          if (instr->GetImmLSUnsigned() != 0) {
            int shift = is_cap ? kCRegSizeInBytesLog2 : instr->GetSizeLS();
            AppendToOutput(", #%" PRId32, instr->GetImmLSUnsigned() << shift);
          }
          return is_cap ? 4 : 3;
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
      int64_t imm = instr->GetImmAddSub() << (12 * instr->GetImmAddSubShift());
      AppendToOutput("#0x%" PRIx64 " (%" PRId64 ")", imm, imm);
      return 7;
    }
    case 'F': {  // IFP, IFPNeon, IFPSve or IFPFBits.
      int imm8 = 0;
      int len = strlen("IFP");
      switch (format[3]) {
        case 'F':
          VIXL_ASSERT(strncmp(format, "IFPFBits", strlen("IFPFBits")) == 0);
          AppendToOutput("#%" PRId32, 64 - instr->GetFPScale());
          return strlen("IFPFBits");
        case 'N':
          VIXL_ASSERT(strncmp(format, "IFPNeon", strlen("IFPNeon")) == 0);
          imm8 = instr->GetImmNEONabcdefgh();
          len += strlen("Neon");
          break;
        case 'S':
          VIXL_ASSERT(strncmp(format, "IFPSve", strlen("IFPSve")) == 0);
          imm8 = instr->ExtractBits(12, 5);
          len += strlen("Sve");
          break;
        default:
          VIXL_ASSERT(strncmp(format, "IFP", strlen("IFP")) == 0);
          imm8 = instr->GetImmFP();
          break;
      }
      AppendToOutput("#0x%" PRIx32 " (%.4f)",
                     imm8,
                     Instruction::Imm8ToFP32(imm8));
      return len;
    }
    case 'H': {  // IH - ImmHint
      AppendToOutput("#%" PRId32, instr->GetImmHint());
      return 2;
    }
    case 'T': {  // ITri - Immediate Triangular Encoded.
      if (format[4] == 'S') {
        VIXL_ASSERT((format[5] == 'v') && (format[6] == 'e'));
        switch (format[7]) {
          case 'l':
            // SVE logical immediate encoding.
            AppendToOutput("#0x%" PRIx64, instr->GetSVEImmLogical());
            return 8;
          case 'p': {
            // SVE predicated shift immediate encoding, lsl.
            std::pair<int, int> shift_and_lane_size =
                instr->GetSVEImmShiftAndLaneSizeLog2(
                    /* is_predicated = */ true);
            int lane_bits = 8 << shift_and_lane_size.second;
            AppendToOutput("#%" PRId32, lane_bits - shift_and_lane_size.first);
            return 8;
          }
          case 'q': {
            // SVE predicated shift immediate encoding, asr and lsr.
            std::pair<int, int> shift_and_lane_size =
                instr->GetSVEImmShiftAndLaneSizeLog2(
                    /* is_predicated = */ true);
            AppendToOutput("#%" PRId32, shift_and_lane_size.first);
            return 8;
          }
          case 'r': {
            // SVE unpredicated shift immediate encoding, lsl.
            std::pair<int, int> shift_and_lane_size =
                instr->GetSVEImmShiftAndLaneSizeLog2(
                    /* is_predicated = */ false);
            int lane_bits = 8 << shift_and_lane_size.second;
            AppendToOutput("#%" PRId32, lane_bits - shift_and_lane_size.first);
            return 8;
          }
          case 's': {
            // SVE unpredicated shift immediate encoding, asr and lsr.
            std::pair<int, int> shift_and_lane_size =
                instr->GetSVEImmShiftAndLaneSizeLog2(
                    /* is_predicated = */ false);
            AppendToOutput("#%" PRId32, shift_and_lane_size.first);
            return 8;
          }
          default:
            VIXL_UNREACHABLE();
            return 0;
        }
      } else {
        AppendToOutput("#0x%" PRIx64, instr->GetImmLogical());
        return 4;
      }
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
    case 't': {  // It - Test and branch bit.
      AppendToOutput("#%" PRId32,
                     (instr->GetImmTestBranchBit5() << 5) |
                         instr->GetImmTestBranchBit40());
      return 2;
    }
    case 'S': {  // ISveSvl - SVE 'mul vl' immediate for structured ld/st.
      VIXL_ASSERT(strncmp(format, "ISveSvl", 7) == 0);
      int imm = instr->ExtractSignedBits(19, 16);
      if (imm != 0) {
        int reg_count = instr->ExtractBits(22, 21) + 1;
        AppendToOutput(", #%d, mul vl", imm * reg_count);
      }
      return 7;
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
          } else if (strncmp(format,
                             "IVInsSVEIndex",
                             strlen("IVInsSVEIndex")) == 0) {
            std::pair<int, int> index_and_lane_size =
                instr->GetSVEPermuteIndexAndLaneSizeLog2();
            AppendToOutput("%d", index_and_lane_size.first);
            return strlen("IVInsSVEIndex");
          }
          VIXL_FALLTHROUGH();
        }
        case 'L': {  // IVLSLane[0123] - suffix indicates access size shift.
          AppendToOutput("%d", instr->GetNEONLSIndex(format[8] - '0'));
          return 9;
        }
        case 'M': {  // Modified Immediate cases.
          if (strncmp(format, "IVMIImm8", strlen("IVMIImm8")) == 0) {
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
        case RSP_EL0:
          AppendToOutput("rsp_el0");
          break;
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
    case 'Z': {  // IZ - Capability system register immediate.
      switch (instr->GetImmSystemRegister()) {
        case CCTLR_EL0:
          AppendToOutput("cctlr_el0");
          break;
        case CID_EL0:
          AppendToOutput("cid_el0");
          break;
        case DDC:
          AppendToOutput("ddc");
          break;
        case RCSP_EL0:
          AppendToOutput("rcsp_el0");
          break;
        case RDDC_EL0:
          AppendToOutput("rddc_el0");
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
    case 'p': {  // Ipc - SVE predicate constraint specifier.
      VIXL_ASSERT(format[2] == 'c');
      unsigned pattern = instr->GetImmSVEPredicateConstraint();
      switch (pattern) {
        // VL1-VL8 are encoded directly.
        case SVE_VL1:
        case SVE_VL2:
        case SVE_VL3:
        case SVE_VL4:
        case SVE_VL5:
        case SVE_VL6:
        case SVE_VL7:
        case SVE_VL8:
          AppendToOutput("vl%u", pattern);
          break;
        // VL16-VL256 are encoded as log2(N) + c.
        case SVE_VL16:
        case SVE_VL32:
        case SVE_VL64:
        case SVE_VL128:
        case SVE_VL256:
          AppendToOutput("vl%u", 16 << (pattern - SVE_VL16));
          break;
        // Special cases.
        case SVE_POW2:
          AppendToOutput("pow2");
          break;
        case SVE_MUL4:
          AppendToOutput("mul4");
          break;
        case SVE_MUL3:
          AppendToOutput("mul3");
          break;
        case SVE_ALL:
          AppendToOutput("all");
          break;
        default:
          AppendToOutput("#0x%x", pattern);
          break;
      }
      return 3;
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
  VIXL_ASSERT((strncmp(format, "LValue", 6) == 0) ||
              (strncmp(format, "LCValue", 7) == 0));

  uint64_t pc = CodeRelativeAddress(instr);
  int64_t address = RawbitsToInt64(instr->GetLiteralAddress(pc));
  if (format[1] == 'C') {
    // Load capability literal.
    VIXL_ASSERT(instr->Mask(MorelloLDRMask) == LDR_c_i);
    AppendSignedDataAddressToOutput(instr, address);
    return 7;
  } else {
    switch (instr->Mask(LoadLiteralMask)) {
      case LDR_w_lit:
      case LDR_x_lit:
      case LDRSW_x_lit:
      case LDR_s_lit:
      case LDR_d_lit:
      case LDR_q_lit:
        AppendSignedDataAddressToOutput(instr, address);
        break;
      case PRFM_lit: {
        // Use the prefetch hint to decide how to print the address.
        switch (instr->GetPrefetchHint()) {
          case 0x0:  // PLD: prefetch for load.
          case 0x2:  // PST: prepare for store.
            AppendSignedDataAddressToOutput(instr, address);
            break;
          case 0x1:  // PLI: preload instructions.
            AppendSignedCodeAddressToOutput(instr, address);
            break;
          case 0x3:  // Unallocated hint.
            AppendSignedAddressToOutput(instr, address);
            break;
        }
        break;
      }
      default:
        VIXL_UNREACHABLE();
    }
    return 6;
  }
}


int Disassembler::SubstituteShiftField(const Instruction *instr,
                                       const char *format) {
  VIXL_ASSERT(format[0] == 'N');
  VIXL_ASSERT(instr->GetShiftDP() <= 0x3);

  switch (format[1]) {
    case 'D': {  // NDP.
      VIXL_ASSERT(instr->GetShiftDP() != ROR);
      VIXL_FALLTHROUGH();
    }
    case 'L': {  // NLo.
      if (instr->GetImmDPShift() != 0) {
        const char *shift_type[] = {"lsl", "lsr", "asr", "ror"};
        AppendToOutput(", %s #%" PRId32,
                       shift_type[instr->GetShiftDP()],
                       instr->GetImmDPShift());
      }
      return 3;
    }
    case 'S': {  // NSveS (SVE structured load/store indexing shift).
      VIXL_ASSERT(strncmp(format, "NSveS", 5) == 0);
      int msz = instr->ExtractBits(24, 23);
      if (msz > 0) {
        AppendToOutput(", lsl #%d", msz);
      }
      return 5;
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


int Disassembler::SubstituteRelAddressField(const Instruction *instr,
                                            const char *format) {
  bool is_page_scaled;
  bool is_adrdp;
  int64_t offset;
  int len;
  if (GetISA() == ISA::C64) {
    // Check strcmp, not strncmp, to ensure that these fields appear last in the
    // format (so we can test them with COMPARE_PREFIX).
    VIXL_ASSERT((strcmp(format, "AddrPCCRelByte") == 0) ||  // Used by `adr`.
                (strcmp(format, "AddrC64RelPage") == 0));  // Used by `adr{d}p`.
    is_page_scaled = format[10] == 'P';
    is_adrdp = is_page_scaled && (instr->GetImmC64RelP() == 0);
    offset = is_page_scaled ? instr->GetImmC64RelPage() : instr->GetImmPCRel();
    len = 14;
  } else {
    VIXL_ASSERT((strcmp(format, "AddrPCRelByte") == 0) ||  // Used by `adr`.
                (strcmp(format, "AddrPCRelPage") == 0));   // Used by `adrp`.
    is_page_scaled = format[9] == 'P';
    is_adrdp = false; // C64 only.
    offset = instr->GetImmPCRel();
    len = 13;
  }
  if (is_page_scaled) offset *= kPageSize;

  if (is_adrdp) {
    // Although described as DDC-relative, `adrdp` can also be configured to be
    // relative to c28, so we don't mention DDC in the output. We do not know
    // the base address, so we can only print the offset.
    VIXL_ASSERT(offset >= 0);
    AppendToOutput("#+0x%" PRIx64, offset);
  } else {
    // Compute the target address based on the effective address (after applying
    // code_address_offset). This is required for correct behaviour of adrp.
    const Instruction *base = instr + code_address_offset();
    if (is_page_scaled) base = AlignDown(base, kPageSize);
    // Strip code_address_offset before printing, so we can use the
    // semantically-correct AppendCodeRelativeAddressToOutput.
    const void *target =
        reinterpret_cast<const void *>(base + offset - code_address_offset());

    AppendPCRelativeOffsetToOutput(instr, offset);
    AppendToOutput(" ");
    AppendCodeRelativeAddressToOutput(instr, target);
  }
  return len;
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
  int len = 3;
  VIXL_ASSERT(instr->GetExtendMode() <= 7);
  USE(format);

  bool lsl = false;
  if (strncmp(format, "ExtC", 4) == 0) {
    len = 4;
  } else {
    // If rd or rn is SP, uxtw on 32-bit registers and uxtx on 64-bit
    // registers becomes lsl.
    lsl = ((instr->GetRd() == kSpRegCode) || (instr->GetRn() == kSpRegCode)) &&
          (((instr->GetExtendMode() == UXTW) &&
            (instr->GetSixtyFourBits() == 0)) ||
           (instr->GetExtendMode() == UXTX));
  }

  if (lsl) {
    if (instr->GetImmExtendShift() > 0) {
      AppendToOutput(", lsl #%" PRId32, instr->GetImmExtendShift());
    }
  } else {
    const char *extend_mode[] =
        {"uxtb", "uxth", "uxtw", "uxtx", "sxtb", "sxth", "sxtw", "sxtx"};
    AppendToOutput(", %s", extend_mode[instr->GetExtendMode()]);
    if (instr->GetImmExtendShift() > 0) {
      AppendToOutput(" #%" PRId32, instr->GetImmExtendShift());
    }
  }
  return len;
}


int Disassembler::SubstituteLSRegOffsetField(const Instruction *instr,
                                             const char *format) {
  int len = 9;
  VIXL_ASSERT(strncmp(format, "Offsetreg", len) == 0);
  unsigned size = instr->GetSizeLS();
  // For some encodings, GetSizeLS doesn't work, but we re-use the formatting
  // logic by specifying the size explicitly.
  switch (format[9]) {
    case 'c':  // 'Offsetregc
      size = kCRegSizeInBytesLog2;
      len++;
      break;
    case 'x':  // 'Offsetregx
    case 'd':  // 'Offsetregd
      size = kXRegSizeInBytesLog2;
      len++;
      break;
    case 'w':  // 'Offsetregw
    case 's':  // 'Offsetregs
      size = kWRegSizeInBytesLog2;
      len++;
      break;
    case 'h':  // 'Offsetregh
      size = kHRegSizeInBytesLog2;
      len++;
      break;
    case 'b':  // 'Offsetregb
      size = kBRegSizeInBytesLog2;
      len++;
      break;
  }
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
    if (shift != 0) AppendToOutput(" #%u", size);
  }
  return len;
}


int Disassembler::SubstitutePrefetchField(const Instruction *instr,
                                          const char *format) {
  VIXL_ASSERT(format[0] == 'p');
  USE(format);

  bool is_sve =
      (strncmp(format, "prefSVEOp", strlen("prefSVEOp")) == 0) ? true : false;
  int placeholder_length = is_sve ? 9 : 6;
  static const char *stream_options[] = {"keep", "strm"};

  auto get_hints = [](bool is_sve) -> std::vector<std::string> {
    static const std::vector<std::string> sve_hints = {"ld", "st"};
    static const std::vector<std::string> core_hints = {"ld", "li", "st"};
    return (is_sve) ? sve_hints : core_hints;
  };

  std::vector<std::string> hints = get_hints(is_sve);
  unsigned hint =
      is_sve ? instr->GetSVEPrefetchHint() : instr->GetPrefetchHint();
  unsigned target = instr->GetPrefetchTarget() + 1;
  unsigned stream = instr->GetPrefetchStream();

  if ((hint >= hints.size()) || (target > 3)) {
    // Unallocated prefetch operations.
    if (is_sve) {
      std::bitset<4> prefetch_mode(instr->GetSVEImmPrefetchOperation());
      AppendToOutput("#0b%s", prefetch_mode.to_string().c_str());
    } else {
      std::bitset<5> prefetch_mode(instr->GetImmPrefetchOperation());
      AppendToOutput("#0b%s", prefetch_mode.to_string().c_str());
    }
  } else {
    VIXL_ASSERT(stream < ArrayLength(stream_options));
    AppendToOutput("p%sl%d%s",
                   hints[hint].c_str(),
                   target,
                   stream_options[stream]);
  }
  return placeholder_length;
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

int Disassembler::SubstituteIntField(const Instruction *instr,
                                     const char *format) {
  VIXL_ASSERT((format[0] == 'u') || (format[0] == 's') || (format[0] == 'x'));

  // A generic int field uses a placeholder of the form 'sAABB, 'uAABB or
  // 'xAABB, where AA and BB are two digit bit positions between 00 and 31, and
  // AA >= BB. The placeholder is substituted with the integer represented by
  // the bits in the instruction between positions AA and BB inclusive.
  //
  //  's: Signed decimal integer.
  //  'u: Unsigned decimal integer.
  //  'x: Unsigned hexadecimal integer.
  //
  // In addition, split fields can be represented using 'sAABB:CCDD, where CCDD
  // become the least-significant bits of the result, and bit AA is the sign bit
  // (if 's is used).
  int32_t bits = 0;
  int width = 0;
  const char *c = format;
  do {
    c++;  // Skip the 'u', 's' or ':'.
    VIXL_ASSERT(strspn(c, "0123456789") == 4);
    int msb = ((c[0] - '0') * 10) + (c[1] - '0');
    int lsb = ((c[2] - '0') * 10) + (c[3] - '0');
    c += 4;  // Skip the characters we just read.
    int chunk_width = msb - lsb + 1;
    VIXL_ASSERT((chunk_width > 0) && (chunk_width < 32));
    bits = (bits << chunk_width) | (instr->ExtractBits(msb, lsb));
    width += chunk_width;
  } while (*c == ':');
  VIXL_ASSERT(IsUintN(width, bits));

  if (format[0] == 's') {
    bits = ExtractSignedBitfield32(width - 1, 0, bits);
  }

  if (*c == '+') {
    // A "+n" trailing the format specifier indicates the extracted value should
    // be incremented by n. This is for cases where the encoding is zero-based,
    // but range of values is not, eg. values [1, 16] encoded as [0, 15]
    char *new_c;
    uint64_t value = strtoul(c + 1, &new_c, 10);
    c = new_c;
    VIXL_ASSERT(IsInt32(value));
    bits += value;
  } else if (*c == '*') {
    // Similarly, a "*n" trailing the format specifier indicates the extracted
    // value should be multiplied by n. This is for cases where the encoded
    // immediate is scaled, for example by access size.
    char *new_c;
    uint64_t value = strtoul(c + 1, &new_c, 10);
    c = new_c;
    VIXL_ASSERT(IsInt32(value));
    bits *= value;
  }

  switch (format[0]) {
    case 's':
      AppendToOutput("%d", bits);
      break;
    case 'u':
      AppendToOutput("%u", bits);
      break;
    case 'x':
      // For hexadecimal outputs, always include the leading zeroes.
      // TODO: Perhaps make this behaviour optional, with some flag.
      AppendToOutput("0x%0*x", (width + 3) / 4, bits);
      break;
  }

  return static_cast<int>(c - format);
}

int Disassembler::SubstituteSVESize(const Instruction *instr,
                                    const char *format) {
  USE(format);
  VIXL_ASSERT(format[0] == 't');

  static const char sizes[] = {'b', 'h', 's', 'd', 'q'};
  // TODO: only the most common case for <size> is supported at the moment,
  // and even then, the RESERVED values are handled as if they're not
  // reserved.
  unsigned size_in_bytes_log2 = instr->GetSVESize();
  int placeholder_length = 1;
  switch (format[1]) {
    case 'l':
      placeholder_length++;
      if (format[2] == 's') {
        // 'tls: Loads and stores
        size_in_bytes_log2 = instr->ExtractBits(22, 21);
        placeholder_length++;
        if (format[3] == 's') {
          // Sign extension load.
          unsigned msize = instr->ExtractBits(24, 23);
          if (msize > size_in_bytes_log2) size_in_bytes_log2 ^= 0x3;
          placeholder_length++;
        }
      } else {
        // 'tl: Logical operations
        size_in_bytes_log2 = instr->GetSVEBitwiseImmLaneSizeInBytesLog2();
      }
      break;
    case 'm':  // 'tmsz
      VIXL_ASSERT(strncmp(format, "tmsz", 4) == 0);
      placeholder_length += 3;
      size_in_bytes_log2 = instr->ExtractBits(24, 23);
      break;
    case 's':
      if (format[2] == 'z') {
        VIXL_ASSERT((format[3] == 'x') || (format[3] == 's') ||
                    (format[3] == 'p'));
        if (format[3] == 'x') {
          // 'tszx: Indexes.
          std::pair<int, int> index_and_lane_size =
              instr->GetSVEPermuteIndexAndLaneSizeLog2();
          size_in_bytes_log2 = index_and_lane_size.second;
        } else if (format[3] == 'p') {
          // 'tszp: Predicated shifts.
          std::pair<int, int> shift_and_lane_size =
              instr->GetSVEImmShiftAndLaneSizeLog2(/* is_predicated = */ true);
          size_in_bytes_log2 = shift_and_lane_size.second;
        } else {
          // 'tszs: Unpredicated shifts.
          std::pair<int, int> shift_and_lane_size =
              instr->GetSVEImmShiftAndLaneSizeLog2(/* is_predicated = */ false);
          size_in_bytes_log2 = shift_and_lane_size.second;
        }
        placeholder_length += 3;  // skip `sz[x|s]`
      }
      break;
    case 'h':
      // Half size of the lane size field.
      size_in_bytes_log2 -= 1;
      placeholder_length++;
      break;
    case 'q':
      // Quarter size of the lane size field.
      size_in_bytes_log2 -= 2;
      placeholder_length++;
      break;
    default:
      break;
  }

  VIXL_ASSERT(size_in_bytes_log2 < ArrayLength(sizes));
  AppendToOutput("%c", sizes[size_in_bytes_log2]);

  return placeholder_length;
}

int Disassembler::SubstituteTernary(const Instruction *instr,
                                    const char *format) {
  VIXL_ASSERT((format[0] == '?') && (format[3] == ':'));

  // The ternary substitution of the format "'?bb:TF" is replaced by a single
  // character, either T or F, depending on the value of the bit at position
  // bb in the instruction. For example, "'?31:xw" is substituted with "x" if
  // bit 31 is true, and "w" otherwise.
  VIXL_ASSERT(strspn(&format[1], "0123456789") == 2);
  char *c;
  uint64_t value = strtoul(&format[1], &c, 10);
  VIXL_ASSERT(value < (kInstructionSize * kBitsPerByte));
  VIXL_ASSERT((*c == ':') && (strlen(c) >= 3));  // Minimum of ":TF"
  c++;
  AppendToOutput("%c", c[1 - instr->ExtractBit(static_cast<int>(value))]);
  return 6;
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


void PrintDisassembler::Disassemble(const Instruction *instr, ISA isa) {
  Decoder decoder;
  if (cpu_features_auditor_ != NULL) {
    decoder.AppendVisitor(cpu_features_auditor_);
  }
  decoder.AppendVisitor(this);
  decoder.SetISA(isa);
  decoder.Decode(instr);
}

void PrintDisassembler::DisassembleBuffer(const Instruction *start,
                                          const Instruction *end,
                                          const ISAMap *map) {
  Decoder decoder;
  if (cpu_features_auditor_ != NULL) {
    decoder.AppendVisitor(cpu_features_auditor_);
  }
  decoder.AppendVisitor(this);
  decoder.Decode(start, end, map);
}

void PrintDisassembler::DisassembleBuffer(const Instruction *start,
                                          uint64_t size,
                                          const ISAMap *map) {
  DisassembleBuffer(start, start + size, map);
}


void PrintDisassembler::SetISA(ISA isa) { Disassembler::SetISA(isa); }


void PrintDisassembler::ProcessOutput(const Instruction *instr) {
  ISA isa = GetISA();
  if (last_printed_isa_ != isa) {
    fprintf(stream_, "%s# ISA: %s\n", default_line_prefix_, GetISAName(isa));
    last_printed_isa_ = isa;
  }

  int64_t address = CodeRelativeAddress(instr);
  uint64_t abs_address;
  const char *sign;
  if (signed_addresses_) {
    if (address < 0) {
      sign = "-";
      abs_address = -static_cast<uint64_t>(address);
    } else {
      // Leave a leading space, to maintain alignment.
      sign = " ";
      abs_address = address;
    }
  } else {
    sign = "";
    abs_address = address;
  }

  const char *prefix =
      (next_disasm_prefix_ == NULL) ? default_line_prefix_ : next_disasm_prefix_;
  next_disasm_prefix_ = NULL;

  int bytes_printed = fprintf(stream_,
                              "%s%s0x%016" PRIx64 "  %08" PRIx32 "\t\t%s",
                              prefix,
                              sign,
                              abs_address,
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
