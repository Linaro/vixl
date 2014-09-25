// Copyright 2013, ARM Limited
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

#ifndef VIXL_A64_MACRO_ASSEMBLER_A64_H_
#define VIXL_A64_MACRO_ASSEMBLER_A64_H_

#include <algorithm>
#include <limits>

#include "globals.h"
#include "a64/assembler-a64.h"
#include "a64/debugger-a64.h"


#define LS_MACRO_LIST(V)                                      \
  V(Ldrb, Register&, rt, LDRB_w)                              \
  V(Strb, Register&, rt, STRB_w)                              \
  V(Ldrsb, Register&, rt, rt.Is64Bits() ? LDRSB_x : LDRSB_w)  \
  V(Ldrh, Register&, rt, LDRH_w)                              \
  V(Strh, Register&, rt, STRH_w)                              \
  V(Ldrsh, Register&, rt, rt.Is64Bits() ? LDRSH_x : LDRSH_w)  \
  V(Ldr, CPURegister&, rt, LoadOpFor(rt))                     \
  V(Str, CPURegister&, rt, StoreOpFor(rt))                    \
  V(Ldrsw, Register&, rt, LDRSW_x)


#define LSPAIR_MACRO_LIST(V)                              \
  V(Ldp, CPURegister&, rt, rt2, LoadPairOpFor(rt, rt2))   \
  V(Stp, CPURegister&, rt, rt2, StorePairOpFor(rt, rt2))  \
  V(Ldpsw, CPURegister&, rt, rt2, LDPSW_x)

namespace vixl {

class LiteralPool {
 public:
  // Recommended not exact since the pool can be blocked for short periods.
  static const ptrdiff_t kRecommendedLiteralPoolRange = 128 * KBytes;
  enum EmitOption {
    kBranchRequired,
    kNoBranchRequired
  };

  explicit LiteralPool(Assembler *masm);
  ~LiteralPool();
  void Reset();

  template <typename T>
  RawLiteral* Add(T imm) {
    if (IsEmpty()) {
      first_use_ = assm_->CursorOffset();
    } else {
      VIXL_ASSERT(assm_->CursorOffset() > first_use_);
    }

    RawLiteral* literal = new Literal<T>(imm);
    entries_.push_back(literal);

    return literal;
  }
  bool IsEmpty() const { return entries_.empty(); }
  size_t Size() const ;

  void Block() { monitor_++; }
  void Release();
  bool IsBlocked() const { return monitor_ != 0; }

  ptrdiff_t MaxCursorOffset() const {
    if (IsEmpty()) return std::numeric_limits<ptrdiff_t>::max();
    return first_use_ + kMaxLoadLiteralRange;
  }

  void CheckEmitFor(size_t amount, EmitOption option = kBranchRequired);
  void Emit(EmitOption option = kNoBranchRequired);
  ptrdiff_t NextCheckOffset();

 private:
  Assembler* assm_;
  std::vector<RawLiteral*> entries_;
  ptrdiff_t first_use_;
  int monitor_;
};


// Forward declaration
class MacroAssembler;

// This scope has the following purposes:
//  * Acquire/Release the underlying assembler's code buffer.
//     * This is mandatory before emitting.
//  * Emit the literal pool if necessary before emitting the macro-instruction.
//  * Ensure there is enough space to emit the macro-instruction.
class EmissionCheckScope {
 public:
  EmissionCheckScope(MacroAssembler* masm, size_t size);
  ~EmissionCheckScope();

 protected:
#ifdef DEBUG
  MacroAssembler* masm_;
  Label start_;
  size_t size_;
#endif
};


// Helper for common Emission checks.
// The macro-instruction maps to a single instruction.
class SingleEmissionCheckScope : public EmissionCheckScope {
 public:
  explicit SingleEmissionCheckScope(MacroAssembler* masm)
      : EmissionCheckScope(masm, kInstructionSize) {}
};


// The macro instruction is a "typical" macro-instruction. Typical macro-
// instruction only emit a few instructions, a few being defined as 8 here.
class MacroEmissionCheckScope : public EmissionCheckScope {
 public:
  explicit MacroEmissionCheckScope(MacroAssembler* masm)
      : EmissionCheckScope(masm, kTypicalMacroInstructionMaxSize) {}

 private:
  static const size_t kTypicalMacroInstructionMaxSize = 8 * kInstructionSize;
};


enum BranchType {
  // Copies of architectural conditions.
  // The associated conditions can be used in place of those, the code will
  // take care of reinterpreting them with the correct type.
  integer_eq = eq,
  integer_ne = ne,
  integer_hs = hs,
  integer_lo = lo,
  integer_mi = mi,
  integer_pl = pl,
  integer_vs = vs,
  integer_vc = vc,
  integer_hi = hi,
  integer_ls = ls,
  integer_ge = ge,
  integer_lt = lt,
  integer_gt = gt,
  integer_le = le,
  integer_al = al,
  integer_nv = nv,

  // These two are *different* from the architectural codes al and nv.
  // 'always' is used to generate unconditional branches.
  // 'never' is used to not generate a branch (generally as the inverse
  // branch type of 'always).
  always, never,
  // cbz and cbnz
  reg_zero, reg_not_zero,
  // tbz and tbnz
  reg_bit_clear, reg_bit_set,

  // Aliases.
  kBranchTypeFirstCondition = eq,
  kBranchTypeLastCondition = nv,
  kBranchTypeFirstUsingReg = reg_zero,
  kBranchTypeFirstUsingBit = reg_bit_clear
};


enum DiscardMoveMode { kDontDiscardForSameWReg, kDiscardForSameWReg };


class MacroAssembler : public Assembler {
 public:
  MacroAssembler(size_t capacity,
                 PositionIndependentCodeOption pic = PositionIndependentCode);
  MacroAssembler(byte * buffer, size_t capacity,
                 PositionIndependentCodeOption pic = PositionIndependentCode);
  ~MacroAssembler();

  // Start generating code from the beginning of the buffer, discarding any code
  // and data that has already been emitted into the buffer.
  //
  // In order to avoid any accidental transfer of state, Reset ASSERTs that the
  // constant pool is not blocked.
  void Reset();

  // Finalize a code buffer of generated instructions. This function must be
  // called before executing or copying code from the buffer.
  void FinalizeCode();

  // Logical macros.
  void And(const Register& rd,
           const Register& rn,
           const Operand& operand);
  void Ands(const Register& rd,
            const Register& rn,
            const Operand& operand);
  void Bic(const Register& rd,
           const Register& rn,
           const Operand& operand);
  void Bics(const Register& rd,
            const Register& rn,
            const Operand& operand);
  void Orr(const Register& rd,
           const Register& rn,
           const Operand& operand);
  void Orn(const Register& rd,
           const Register& rn,
           const Operand& operand);
  void Eor(const Register& rd,
           const Register& rn,
           const Operand& operand);
  void Eon(const Register& rd,
           const Register& rn,
           const Operand& operand);
  void Tst(const Register& rn, const Operand& operand);
  void LogicalMacro(const Register& rd,
                    const Register& rn,
                    const Operand& operand,
                    LogicalOp op);

  // Add and sub macros.
  void Add(const Register& rd,
           const Register& rn,
           const Operand& operand);
  void Adds(const Register& rd,
            const Register& rn,
            const Operand& operand);
  void Sub(const Register& rd,
           const Register& rn,
           const Operand& operand);
  void Subs(const Register& rd,
            const Register& rn,
            const Operand& operand);
  void Cmn(const Register& rn, const Operand& operand);
  void Cmp(const Register& rn, const Operand& operand);
  void Neg(const Register& rd,
           const Operand& operand);
  void Negs(const Register& rd,
            const Operand& operand);

  void AddSubMacro(const Register& rd,
                   const Register& rn,
                   const Operand& operand,
                   FlagsUpdate S,
                   AddSubOp op);

  // Add/sub with carry macros.
  void Adc(const Register& rd,
           const Register& rn,
           const Operand& operand);
  void Adcs(const Register& rd,
            const Register& rn,
            const Operand& operand);
  void Sbc(const Register& rd,
           const Register& rn,
           const Operand& operand);
  void Sbcs(const Register& rd,
            const Register& rn,
            const Operand& operand);
  void Ngc(const Register& rd,
           const Operand& operand);
  void Ngcs(const Register& rd,
            const Operand& operand);
  void AddSubWithCarryMacro(const Register& rd,
                            const Register& rn,
                            const Operand& operand,
                            FlagsUpdate S,
                            AddSubWithCarryOp op);

  // Move macros.
  void Mov(const Register& rd, uint64_t imm);
  void Mov(const Register& rd,
           const Operand& operand,
           DiscardMoveMode discard_mode = kDontDiscardForSameWReg);
  void Mvn(const Register& rd, uint64_t imm) {
    Mov(rd, (rd.size() == kXRegSize) ? ~imm : (~imm & kWRegMask));
  }
  void Mvn(const Register& rd, const Operand& operand);
  bool IsImmMovz(uint64_t imm, unsigned reg_size);
  bool IsImmMovn(uint64_t imm, unsigned reg_size);
  unsigned CountClearHalfWords(uint64_t imm, unsigned reg_size);

  // Try to move an immediate into the destination register in a single
  // instruction. Returns true for success, and updates the contents of dst.
  // Returns false, otherwise.
  bool TryOneInstrMoveImmediate(const Register& dst, int64_t imm);

  // Move an immediate into register dst, and return an Operand object for
  // use with a subsequent instruction that accepts a shift. The value moved
  // into dst is not necessarily equal to imm; it may have had a shifting
  // operation applied to it that will be subsequently undone by the shift
  // applied in the Operand.
  Operand MoveImmediateForShiftedOp(const Register& dst, int64_t imm);

  // Conditional macros.
  void Ccmp(const Register& rn,
            const Operand& operand,
            StatusFlags nzcv,
            Condition cond);
  void Ccmn(const Register& rn,
            const Operand& operand,
            StatusFlags nzcv,
            Condition cond);
  void ConditionalCompareMacro(const Register& rn,
                               const Operand& operand,
                               StatusFlags nzcv,
                               Condition cond,
                               ConditionalCompareOp op);
  void Csel(const Register& rd,
            const Register& rn,
            const Operand& operand,
            Condition cond);

  // Load/store macros.
#define DECLARE_FUNCTION(FN, REGTYPE, REG, OP) \
  void FN(const REGTYPE REG, const MemOperand& addr);
  LS_MACRO_LIST(DECLARE_FUNCTION)
#undef DECLARE_FUNCTION

  void LoadStoreMacro(const CPURegister& rt,
                      const MemOperand& addr,
                      LoadStoreOp op);

#define DECLARE_FUNCTION(FN, REGTYPE, REG, REG2, OP) \
  void FN(const REGTYPE REG, const REGTYPE REG2, const MemOperand& addr);
  LSPAIR_MACRO_LIST(DECLARE_FUNCTION)
#undef DECLARE_FUNCTION

  void LoadStorePairMacro(const CPURegister& rt,
                          const CPURegister& rt2,
                          const MemOperand& addr,
                          LoadStorePairOp op);

  // Push or pop up to 4 registers of the same width to or from the stack,
  // using the current stack pointer as set by SetStackPointer.
  //
  // If an argument register is 'NoReg', all further arguments are also assumed
  // to be 'NoReg', and are thus not pushed or popped.
  //
  // Arguments are ordered such that "Push(a, b);" is functionally equivalent
  // to "Push(a); Push(b);".
  //
  // It is valid to push the same register more than once, and there is no
  // restriction on the order in which registers are specified.
  //
  // It is not valid to pop into the same register more than once in one
  // operation, not even into the zero register.
  //
  // If the current stack pointer (as set by SetStackPointer) is sp, then it
  // must be aligned to 16 bytes on entry and the total size of the specified
  // registers must also be a multiple of 16 bytes.
  //
  // Even if the current stack pointer is not the system stack pointer (sp),
  // Push (and derived methods) will still modify the system stack pointer in
  // order to comply with ABI rules about accessing memory below the system
  // stack pointer.
  //
  // Other than the registers passed into Pop, the stack pointer and (possibly)
  // the system stack pointer, these methods do not modify any other registers.
  void Push(const CPURegister& src0, const CPURegister& src1 = NoReg,
            const CPURegister& src2 = NoReg, const CPURegister& src3 = NoReg);
  void Pop(const CPURegister& dst0, const CPURegister& dst1 = NoReg,
           const CPURegister& dst2 = NoReg, const CPURegister& dst3 = NoReg);

  // Alternative forms of Push and Pop, taking a RegList or CPURegList that
  // specifies the registers that are to be pushed or popped. Higher-numbered
  // registers are associated with higher memory addresses (as in the A32 push
  // and pop instructions).
  //
  // (Push|Pop)SizeRegList allow you to specify the register size as a
  // parameter. Only kXRegSize, kWRegSize, kDRegSize and kSRegSize are
  // supported.
  //
  // Otherwise, (Push|Pop)(CPU|X|W|D|S)RegList is preferred.
  void PushCPURegList(CPURegList registers);
  void PopCPURegList(CPURegList registers);

  void PushSizeRegList(RegList registers, unsigned reg_size,
      CPURegister::RegisterType type = CPURegister::kRegister) {
    PushCPURegList(CPURegList(type, reg_size, registers));
  }
  void PopSizeRegList(RegList registers, unsigned reg_size,
      CPURegister::RegisterType type = CPURegister::kRegister) {
    PopCPURegList(CPURegList(type, reg_size, registers));
  }
  void PushXRegList(RegList regs) {
    PushSizeRegList(regs, kXRegSize);
  }
  void PopXRegList(RegList regs) {
    PopSizeRegList(regs, kXRegSize);
  }
  void PushWRegList(RegList regs) {
    PushSizeRegList(regs, kWRegSize);
  }
  void PopWRegList(RegList regs) {
    PopSizeRegList(regs, kWRegSize);
  }
  inline void PushDRegList(RegList regs) {
    PushSizeRegList(regs, kDRegSize, CPURegister::kFPRegister);
  }
  inline void PopDRegList(RegList regs) {
    PopSizeRegList(regs, kDRegSize, CPURegister::kFPRegister);
  }
  inline void PushSRegList(RegList regs) {
    PushSizeRegList(regs, kSRegSize, CPURegister::kFPRegister);
  }
  inline void PopSRegList(RegList regs) {
    PopSizeRegList(regs, kSRegSize, CPURegister::kFPRegister);
  }

  // Push the specified register 'count' times.
  void PushMultipleTimes(int count, Register src);

  // Poke 'src' onto the stack. The offset is in bytes.
  //
  // If the current stack pointer (as set by SetStackPointer) is sp, then sp
  // must be aligned to 16 bytes.
  void Poke(const Register& src, const Operand& offset);

  // Peek at a value on the stack, and put it in 'dst'. The offset is in bytes.
  //
  // If the current stack pointer (as set by SetStackPointer) is sp, then sp
  // must be aligned to 16 bytes.
  void Peek(const Register& dst, const Operand& offset);

  // Alternative forms of Peek and Poke, taking a RegList or CPURegList that
  // specifies the registers that are to be pushed or popped. Higher-numbered
  // registers are associated with higher memory addresses.
  //
  // (Peek|Poke)SizeRegList allow you to specify the register size as a
  // parameter. Only kXRegSize, kWRegSize, kDRegSize and kSRegSize are
  // supported.
  //
  // Otherwise, (Peek|Poke)(CPU|X|W|D|S)RegList is preferred.
  void PeekCPURegList(CPURegList registers, int offset);
  void PokeCPURegList(CPURegList registers, int offset);

  void PeekSizeRegList(RegList registers, int offset, unsigned reg_size,
      CPURegister::RegisterType type = CPURegister::kRegister) {
    PeekCPURegList(CPURegList(type, reg_size, registers), offset);
  }
  void PokeSizeRegList(RegList registers, int offset, unsigned reg_size,
      CPURegister::RegisterType type = CPURegister::kRegister) {
    PokeCPURegList(CPURegList(type, reg_size, registers), offset);
  }
  void PeekXRegList(RegList regs, int offset) {
    PeekSizeRegList(regs, offset, kXRegSize);
  }
  void PokeXRegList(RegList regs, int offset) {
    PokeSizeRegList(regs, offset, kXRegSize);
  }
  void PeekWRegList(RegList regs, int offset) {
    PeekSizeRegList(regs, offset, kWRegSize);
  }
  void PokeWRegList(RegList regs, int offset) {
    PokeSizeRegList(regs, offset, kWRegSize);
  }
  inline void PeekDRegList(RegList regs, int offset) {
    PeekSizeRegList(regs, offset, kDRegSize, CPURegister::kFPRegister);
  }
  inline void PokeDRegList(RegList regs, int offset) {
    PokeSizeRegList(regs, offset, kDRegSize, CPURegister::kFPRegister);
  }
  inline void PeekSRegList(RegList regs, int offset) {
    PeekSizeRegList(regs, offset, kSRegSize, CPURegister::kFPRegister);
  }
  inline void PokeSRegList(RegList regs, int offset) {
    PokeSizeRegList(regs, offset, kSRegSize, CPURegister::kFPRegister);
  }


  // Claim or drop stack space without actually accessing memory.
  //
  // If the current stack pointer (as set by SetStackPointer) is sp, then it
  // must be aligned to 16 bytes and the size claimed or dropped must be a
  // multiple of 16 bytes.
  void Claim(const Operand& size);
  void Drop(const Operand& size);

  // Preserve the callee-saved registers (as defined by AAPCS64).
  //
  // Higher-numbered registers are pushed before lower-numbered registers, and
  // thus get higher addresses.
  // Floating-point registers are pushed before general-purpose registers, and
  // thus get higher addresses.
  //
  // This method must not be called unless StackPointer() is sp, and it is
  // aligned to 16 bytes.
  void PushCalleeSavedRegisters();

  // Restore the callee-saved registers (as defined by AAPCS64).
  //
  // Higher-numbered registers are popped after lower-numbered registers, and
  // thus come from higher addresses.
  // Floating-point registers are popped after general-purpose registers, and
  // thus come from higher addresses.
  //
  // This method must not be called unless StackPointer() is sp, and it is
  // aligned to 16 bytes.
  void PopCalleeSavedRegisters();

  // Remaining instructions are simple pass-through calls to the assembler.
  void Adr(const Register& rd, Label* label) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    adr(rd, label);
  }
  void Adrp(const Register& rd, Label* label) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    adrp(rd, label);
  }
  void Asr(const Register& rd, const Register& rn, unsigned shift) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    asr(rd, rn, shift);
  }
  void Asr(const Register& rd, const Register& rn, const Register& rm) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    SingleEmissionCheckScope guard(this);
    asrv(rd, rn, rm);
  }

  // Branch type inversion relies on these relations.
  VIXL_STATIC_ASSERT((reg_zero      == (reg_not_zero ^ 1)) &&
                     (reg_bit_clear == (reg_bit_set ^ 1)) &&
                     (always        == (never ^ 1)));

  BranchType InvertBranchType(BranchType type) {
    if (kBranchTypeFirstCondition <= type && type <= kBranchTypeLastCondition) {
      return static_cast<BranchType>(
          InvertCondition(static_cast<Condition>(type)));
    } else {
      return static_cast<BranchType>(type ^ 1);
    }
  }

  void B(Label* label, BranchType type, Register reg = NoReg, int bit = -1);

  void B(Label* label) {
    SingleEmissionCheckScope guard(this);
    b(label);
  }
  void B(Label* label, Condition cond) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT((cond != al) && (cond != nv));
    SingleEmissionCheckScope guard(this);
    b(label, cond);
  }
  void B(Condition cond, Label* label) {
    B(label, cond);
  }
  void Bfi(const Register& rd,
           const Register& rn,
           unsigned lsb,
           unsigned width) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    bfi(rd, rn, lsb, width);
  }
  void Bfxil(const Register& rd,
             const Register& rn,
             unsigned lsb,
             unsigned width) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    bfxil(rd, rn, lsb, width);
  }
  void Bind(Label* label) {
    VIXL_ASSERT(allow_macro_instructions_);
    bind(label);
  }
  // Bind a label to a specified offset from the start of the buffer.
  void BindToOffset(Label* label, ptrdiff_t offset) {
    VIXL_ASSERT(allow_macro_instructions_);
    Assembler::BindToOffset(label, offset);
  }
  void Bl(Label* label) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    bl(label);
  }
  void Blr(const Register& xn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!xn.IsZero());
    SingleEmissionCheckScope guard(this);
    blr(xn);
  }
  void Br(const Register& xn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!xn.IsZero());
    SingleEmissionCheckScope guard(this);
    br(xn);
  }
  void Brk(int code = 0) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    brk(code);
  }
  void Cbnz(const Register& rt, Label* label) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rt.IsZero());
    SingleEmissionCheckScope guard(this);
    cbnz(rt, label);
  }
  void Cbz(const Register& rt, Label* label) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rt.IsZero());
    SingleEmissionCheckScope guard(this);
    cbz(rt, label);
  }
  void Cinc(const Register& rd, const Register& rn, Condition cond) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    cinc(rd, rn, cond);
  }
  void Cinv(const Register& rd, const Register& rn, Condition cond) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    cinv(rd, rn, cond);
  }
  void Clrex() {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    clrex();
  }
  void Cls(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    cls(rd, rn);
  }
  void Clz(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    clz(rd, rn);
  }
  void Cneg(const Register& rd, const Register& rn, Condition cond) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    cneg(rd, rn, cond);
  }
  void Cset(const Register& rd, Condition cond) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    cset(rd, cond);
  }
  void Csetm(const Register& rd, Condition cond) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    csetm(rd, cond);
  }
  void Csinc(const Register& rd,
             const Register& rn,
             const Register& rm,
             Condition cond) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    VIXL_ASSERT((cond != al) && (cond != nv));
    SingleEmissionCheckScope guard(this);
    csinc(rd, rn, rm, cond);
  }
  void Csinv(const Register& rd,
             const Register& rn,
             const Register& rm,
             Condition cond) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    VIXL_ASSERT((cond != al) && (cond != nv));
    SingleEmissionCheckScope guard(this);
    csinv(rd, rn, rm, cond);
  }
  void Csneg(const Register& rd,
             const Register& rn,
             const Register& rm,
             Condition cond) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    VIXL_ASSERT((cond != al) && (cond != nv));
    SingleEmissionCheckScope guard(this);
    csneg(rd, rn, rm, cond);
  }
  void Dmb(BarrierDomain domain, BarrierType type) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    dmb(domain, type);
  }
  void Dsb(BarrierDomain domain, BarrierType type) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    dsb(domain, type);
  }
  void Extr(const Register& rd,
            const Register& rn,
            const Register& rm,
            unsigned lsb) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    SingleEmissionCheckScope guard(this);
    extr(rd, rn, rm, lsb);
  }
  void Fabs(const FPRegister& fd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fabs(fd, fn);
  }
  void Fadd(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fadd(fd, fn, fm);
  }
  void Fccmp(const FPRegister& fn,
             const FPRegister& fm,
             StatusFlags nzcv,
             Condition cond) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT((cond != al) && (cond != nv));
    SingleEmissionCheckScope guard(this);
    fccmp(fn, fm, nzcv, cond);
  }
  void Fcmp(const FPRegister& fn, const FPRegister& fm) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fcmp(fn, fm);
  }
  void Fcmp(const FPRegister& fn, double value);
  void Fcsel(const FPRegister& fd,
             const FPRegister& fn,
             const FPRegister& fm,
             Condition cond) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT((cond != al) && (cond != nv));
    SingleEmissionCheckScope guard(this);
    fcsel(fd, fn, fm, cond);
  }
  void Fcvt(const FPRegister& fd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fcvt(fd, fn);
  }
  void Fcvtas(const Register& rd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    fcvtas(rd, fn);
  }
  void Fcvtau(const Register& rd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    fcvtau(rd, fn);
  }
  void Fcvtms(const Register& rd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    fcvtms(rd, fn);
  }
  void Fcvtmu(const Register& rd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    fcvtmu(rd, fn);
  }
  void Fcvtns(const Register& rd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    fcvtns(rd, fn);
  }
  void Fcvtnu(const Register& rd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    fcvtnu(rd, fn);
  }
  void Fcvtzs(const Register& rd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    fcvtzs(rd, fn);
  }
  void Fcvtzu(const Register& rd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    fcvtzu(rd, fn);
  }
  void Fdiv(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fdiv(fd, fn, fm);
  }
  void Fmax(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fmax(fd, fn, fm);
  }
  void Fmaxnm(const FPRegister& fd,
              const FPRegister& fn,
              const FPRegister& fm) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fmaxnm(fd, fn, fm);
  }
  void Fmin(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fmin(fd, fn, fm);
  }
  void Fminnm(const FPRegister& fd,
              const FPRegister& fn,
              const FPRegister& fm) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fminnm(fd, fn, fm);
  }
  void Fmov(FPRegister fd, FPRegister fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    // Only emit an instruction if fd and fn are different, and they are both D
    // registers. fmov(s0, s0) is not a no-op because it clears the top word of
    // d0. Technically, fmov(d0, d0) is not a no-op either because it clears
    // the top of q0, but FPRegister does not currently support Q registers.
    if (!fd.Is(fn) || !fd.Is64Bits()) {
      fmov(fd, fn);
    }
  }
  void Fmov(FPRegister fd, Register rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    fmov(fd, rn);
  }
  // Provide explicit double and float interfaces for FP immediate moves, rather
  // than relying on implicit C++ casts. This allows signalling NaNs to be
  // preserved when the immediate matches the format of fd. Most systems convert
  // signalling NaNs to quiet NaNs when converting between float and double.
  void Fmov(FPRegister fd, double imm);
  void Fmov(FPRegister fd, float imm);
  // Provide a template to allow other types to be converted automatically.
  template<typename T>
  void Fmov(FPRegister fd, T imm) {
    VIXL_ASSERT(allow_macro_instructions_);
    Fmov(fd, static_cast<double>(imm));
  }
  void Fmov(Register rd, FPRegister fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    fmov(rd, fn);
  }
  void Fmul(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fmul(fd, fn, fm);
  }
  void Fmadd(const FPRegister& fd,
             const FPRegister& fn,
             const FPRegister& fm,
             const FPRegister& fa) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fmadd(fd, fn, fm, fa);
  }
  void Fmsub(const FPRegister& fd,
             const FPRegister& fn,
             const FPRegister& fm,
             const FPRegister& fa) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fmsub(fd, fn, fm, fa);
  }
  void Fnmadd(const FPRegister& fd,
              const FPRegister& fn,
              const FPRegister& fm,
              const FPRegister& fa) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fnmadd(fd, fn, fm, fa);
  }
  void Fnmsub(const FPRegister& fd,
              const FPRegister& fn,
              const FPRegister& fm,
              const FPRegister& fa) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fnmsub(fd, fn, fm, fa);
  }
  void Fneg(const FPRegister& fd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fneg(fd, fn);
  }
  void Frinta(const FPRegister& fd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    frinta(fd, fn);
  }
  void Frintm(const FPRegister& fd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    frintm(fd, fn);
  }
  void Frintn(const FPRegister& fd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    frintn(fd, fn);
  }
  void Frintz(const FPRegister& fd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    frintz(fd, fn);
  }
  void Fsqrt(const FPRegister& fd, const FPRegister& fn) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fsqrt(fd, fn);
  }
  void Fsub(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    fsub(fd, fn, fm);
  }
  void Hint(SystemHint code) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    hint(code);
  }
  void Hlt(int code) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    hlt(code);
  }
  void Isb() {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    isb();
  }
  void Ldar(const Register& rt, const MemOperand& src) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    ldar(rt, src);
  }
  void Ldarb(const Register& rt, const MemOperand& src) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    ldarb(rt, src);
  }
  void Ldarh(const Register& rt, const MemOperand& src) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    ldarh(rt, src);
  }
  void Ldaxp(const Register& rt, const Register& rt2, const MemOperand& src) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rt.Aliases(rt2));
    SingleEmissionCheckScope guard(this);
    ldaxp(rt, rt2, src);
  }
  void Ldaxr(const Register& rt, const MemOperand& src) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    ldaxr(rt, src);
  }
  void Ldaxrb(const Register& rt, const MemOperand& src) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    ldaxrb(rt, src);
  }
  void Ldaxrh(const Register& rt, const MemOperand& src) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    ldaxrh(rt, src);
  }
  void Ldnp(const CPURegister& rt,
            const CPURegister& rt2,
            const MemOperand& src) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    ldnp(rt, rt2, src);
  }
  // Provide both double and float interfaces for FP immediate loads, rather
  // than relying on implicit C++ casts. This allows signalling NaNs to be
  // preserved when the immediate matches the format of fd. Most systems convert
  // signalling NaNs to quiet NaNs when converting between float and double.
  void Ldr(const FPRegister& ft, double imm) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    RawLiteral* literal;
    if (ft.Is64Bits()) {
      literal = literal_pool_.Add(imm);
    } else {
      literal = literal_pool_.Add(static_cast<float>(imm));
    }
    ldr(ft, literal);
  }
  void Ldr(const FPRegister& ft, float imm) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    RawLiteral* literal;
    if (ft.Is32Bits()) {
      literal = literal_pool_.Add(imm);
    } else {
      literal = literal_pool_.Add(static_cast<double>(imm));
    }
    ldr(ft, literal);
  }
  void Ldr(const Register& rt, uint64_t imm) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rt.IsZero());
    SingleEmissionCheckScope guard(this);
    RawLiteral* literal;
    if (rt.Is64Bits()) {
      literal = literal_pool_.Add(imm);
    } else {
      VIXL_ASSERT(rt.Is32Bits());
      VIXL_ASSERT(is_uint32(imm) || is_int32(imm));
      literal = literal_pool_.Add(static_cast<uint32_t>(imm));
    }
    ldr(rt, literal);
  }
  void Ldrsw(const Register& rt, uint32_t imm) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rt.IsZero());
    SingleEmissionCheckScope guard(this);
    RawLiteral* literal = literal_pool_.Add(imm);
    ldrsw(rt, literal);
  }
  void Ldxp(const Register& rt, const Register& rt2, const MemOperand& src) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rt.Aliases(rt2));
    SingleEmissionCheckScope guard(this);
    ldxp(rt, rt2, src);
  }
  void Ldxr(const Register& rt, const MemOperand& src) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    ldxr(rt, src);
  }
  void Ldxrb(const Register& rt, const MemOperand& src) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    ldxrb(rt, src);
  }
  void Ldxrh(const Register& rt, const MemOperand& src) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    ldxrh(rt, src);
  }
  void Lsl(const Register& rd, const Register& rn, unsigned shift) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    lsl(rd, rn, shift);
  }
  void Lsl(const Register& rd, const Register& rn, const Register& rm) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    SingleEmissionCheckScope guard(this);
    lslv(rd, rn, rm);
  }
  void Lsr(const Register& rd, const Register& rn, unsigned shift) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    lsr(rd, rn, shift);
  }
  void Lsr(const Register& rd, const Register& rn, const Register& rm) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    SingleEmissionCheckScope guard(this);
    lsrv(rd, rn, rm);
  }
  void Madd(const Register& rd,
            const Register& rn,
            const Register& rm,
            const Register& ra) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    VIXL_ASSERT(!ra.IsZero());
    SingleEmissionCheckScope guard(this);
    madd(rd, rn, rm, ra);
  }
  void Mneg(const Register& rd, const Register& rn, const Register& rm) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    SingleEmissionCheckScope guard(this);
    mneg(rd, rn, rm);
  }
  void Mov(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    mov(rd, rn);
  }
  void Movk(const Register& rd, uint64_t imm, int shift = -1) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    SingleEmissionCheckScope guard(this);
    movk(rd, imm, shift);
  }
  void Mrs(const Register& rt, SystemRegister sysreg) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rt.IsZero());
    SingleEmissionCheckScope guard(this);
    mrs(rt, sysreg);
  }
  void Msr(SystemRegister sysreg, const Register& rt) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rt.IsZero());
    SingleEmissionCheckScope guard(this);
    msr(sysreg, rt);
  }
  void Msub(const Register& rd,
            const Register& rn,
            const Register& rm,
            const Register& ra) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    VIXL_ASSERT(!ra.IsZero());
    SingleEmissionCheckScope guard(this);
    msub(rd, rn, rm, ra);
  }
  void Mul(const Register& rd, const Register& rn, const Register& rm) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    SingleEmissionCheckScope guard(this);
    mul(rd, rn, rm);
  }
  void Nop() {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    nop();
  }
  void Rbit(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    rbit(rd, rn);
  }
  void Ret(const Register& xn = lr) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!xn.IsZero());
    SingleEmissionCheckScope guard(this);
    ret(xn);
  }
  void Rev(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    rev(rd, rn);
  }
  void Rev16(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    rev16(rd, rn);
  }
  void Rev32(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    rev32(rd, rn);
  }
  void Ror(const Register& rd, const Register& rs, unsigned shift) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rs.IsZero());
    SingleEmissionCheckScope guard(this);
    ror(rd, rs, shift);
  }
  void Ror(const Register& rd, const Register& rn, const Register& rm) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    SingleEmissionCheckScope guard(this);
    rorv(rd, rn, rm);
  }
  void Sbfiz(const Register& rd,
             const Register& rn,
             unsigned lsb,
             unsigned width) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    sbfiz(rd, rn, lsb, width);
  }
  void Sbfx(const Register& rd,
            const Register& rn,
            unsigned lsb,
            unsigned width) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    sbfx(rd, rn, lsb, width);
  }
  void Scvtf(const FPRegister& fd, const Register& rn, unsigned fbits = 0) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    scvtf(fd, rn, fbits);
  }
  void Sdiv(const Register& rd, const Register& rn, const Register& rm) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    SingleEmissionCheckScope guard(this);
    sdiv(rd, rn, rm);
  }
  void Smaddl(const Register& rd,
              const Register& rn,
              const Register& rm,
              const Register& ra) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    VIXL_ASSERT(!ra.IsZero());
    SingleEmissionCheckScope guard(this);
    smaddl(rd, rn, rm, ra);
  }
  void Smsubl(const Register& rd,
              const Register& rn,
              const Register& rm,
              const Register& ra) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    VIXL_ASSERT(!ra.IsZero());
    SingleEmissionCheckScope guard(this);
    smsubl(rd, rn, rm, ra);
  }
  void Smull(const Register& rd, const Register& rn, const Register& rm) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    SingleEmissionCheckScope guard(this);
    smull(rd, rn, rm);
  }
  void Smulh(const Register& xd, const Register& xn, const Register& xm) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!xd.IsZero());
    VIXL_ASSERT(!xn.IsZero());
    VIXL_ASSERT(!xm.IsZero());
    SingleEmissionCheckScope guard(this);
    smulh(xd, xn, xm);
  }
  void Stlr(const Register& rt, const MemOperand& dst) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    stlr(rt, dst);
  }
  void Stlrb(const Register& rt, const MemOperand& dst) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    stlrb(rt, dst);
  }
  void Stlrh(const Register& rt, const MemOperand& dst) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    stlrh(rt, dst);
  }
  void Stlxp(const Register& rs,
             const Register& rt,
             const Register& rt2,
             const MemOperand& dst) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rs.Aliases(dst.base()));
    VIXL_ASSERT(!rs.Aliases(rt));
    VIXL_ASSERT(!rs.Aliases(rt2));
    SingleEmissionCheckScope guard(this);
    stlxp(rs, rt, rt2, dst);
  }
  void Stlxr(const Register& rs, const Register& rt, const MemOperand& dst) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rs.Aliases(dst.base()));
    VIXL_ASSERT(!rs.Aliases(rt));
    SingleEmissionCheckScope guard(this);
    stlxr(rs, rt, dst);
  }
  void Stlxrb(const Register& rs, const Register& rt, const MemOperand& dst) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rs.Aliases(dst.base()));
    VIXL_ASSERT(!rs.Aliases(rt));
    SingleEmissionCheckScope guard(this);
    stlxrb(rs, rt, dst);
  }
  void Stlxrh(const Register& rs, const Register& rt, const MemOperand& dst) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rs.Aliases(dst.base()));
    VIXL_ASSERT(!rs.Aliases(rt));
    SingleEmissionCheckScope guard(this);
    stlxrh(rs, rt, dst);
  }
  void Stnp(const CPURegister& rt,
            const CPURegister& rt2,
            const MemOperand& dst) {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
    stnp(rt, rt2, dst);
  }
  void Stxp(const Register& rs,
            const Register& rt,
            const Register& rt2,
            const MemOperand& dst) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rs.Aliases(dst.base()));
    VIXL_ASSERT(!rs.Aliases(rt));
    VIXL_ASSERT(!rs.Aliases(rt2));
    SingleEmissionCheckScope guard(this);
    stxp(rs, rt, rt2, dst);
  }
  void Stxr(const Register& rs, const Register& rt, const MemOperand& dst) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rs.Aliases(dst.base()));
    VIXL_ASSERT(!rs.Aliases(rt));
    SingleEmissionCheckScope guard(this);
    stxr(rs, rt, dst);
  }
  void Stxrb(const Register& rs, const Register& rt, const MemOperand& dst) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rs.Aliases(dst.base()));
    VIXL_ASSERT(!rs.Aliases(rt));
    SingleEmissionCheckScope guard(this);
    stxrb(rs, rt, dst);
  }
  void Stxrh(const Register& rs, const Register& rt, const MemOperand& dst) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rs.Aliases(dst.base()));
    VIXL_ASSERT(!rs.Aliases(rt));
    SingleEmissionCheckScope guard(this);
    stxrh(rs, rt, dst);
  }
  void Sxtb(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    sxtb(rd, rn);
  }
  void Sxth(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    sxth(rd, rn);
  }
  void Sxtw(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    sxtw(rd, rn);
  }
  void Tbnz(const Register& rt, unsigned bit_pos, Label* label) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rt.IsZero());
    SingleEmissionCheckScope guard(this);
    tbnz(rt, bit_pos, label);
  }
  void Tbz(const Register& rt, unsigned bit_pos, Label* label) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rt.IsZero());
    SingleEmissionCheckScope guard(this);
    tbz(rt, bit_pos, label);
  }
  void Ubfiz(const Register& rd,
             const Register& rn,
             unsigned lsb,
             unsigned width) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    ubfiz(rd, rn, lsb, width);
  }
  void Ubfx(const Register& rd,
            const Register& rn,
            unsigned lsb,
            unsigned width) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    ubfx(rd, rn, lsb, width);
  }
  void Ucvtf(const FPRegister& fd, const Register& rn, unsigned fbits = 0) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    ucvtf(fd, rn, fbits);
  }
  void Udiv(const Register& rd, const Register& rn, const Register& rm) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    SingleEmissionCheckScope guard(this);
    udiv(rd, rn, rm);
  }
  void Umaddl(const Register& rd,
              const Register& rn,
              const Register& rm,
              const Register& ra) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    VIXL_ASSERT(!ra.IsZero());
    SingleEmissionCheckScope guard(this);
    umaddl(rd, rn, rm, ra);
  }
  void Umsubl(const Register& rd,
              const Register& rn,
              const Register& rm,
              const Register& ra) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    VIXL_ASSERT(!rm.IsZero());
    VIXL_ASSERT(!ra.IsZero());
    SingleEmissionCheckScope guard(this);
    umsubl(rd, rn, rm, ra);
  }
  void Unreachable() {
    VIXL_ASSERT(allow_macro_instructions_);
    SingleEmissionCheckScope guard(this);
#ifdef USE_SIMULATOR
    hlt(kUnreachableOpcode);
#else
    // Branch to 0 to generate a segfault.
    // lr - kInstructionSize is the address of the offending instruction.
    blr(xzr);
#endif
  }
  void Uxtb(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    uxtb(rd, rn);
  }
  void Uxth(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    uxth(rd, rn);
  }
  void Uxtw(const Register& rd, const Register& rn) {
    VIXL_ASSERT(allow_macro_instructions_);
    VIXL_ASSERT(!rd.IsZero());
    VIXL_ASSERT(!rn.IsZero());
    SingleEmissionCheckScope guard(this);
    uxtw(rd, rn);
  }

  // Push the system stack pointer (sp) down to allow the same to be done to
  // the current stack pointer (according to StackPointer()). This must be
  // called _before_ accessing the memory.
  //
  // This is necessary when pushing or otherwise adding things to the stack, to
  // satisfy the AAPCS64 constraint that the memory below the system stack
  // pointer is not accessed.
  //
  // This method asserts that StackPointer() is not sp, since the call does
  // not make sense in that context.
  //
  // TODO: This method can only accept values of 'space' that can be encoded in
  // one instruction. Refer to the implementation for details.
  void BumpSystemStackPointer(const Operand& space);

#if DEBUG
  void SetAllowMacroInstructions(bool value) {
    allow_macro_instructions_ = value;
  }

  bool AllowMacroInstructions() const {
    return allow_macro_instructions_;
  }
#endif

  void BlockLiteralPool() { literal_pool_.Block(); }
  void ReleaseLiteralPool() { literal_pool_.Release(); }
  bool IsLiteralPoolBlocked() const { return literal_pool_.IsBlocked(); }

  size_t LiteralPoolSize() const {
    if (literal_pool_.IsEmpty()) return 0;
    return literal_pool_.Size() - kInstructionSize;
  }

  void EmitLiteralPool(LiteralPool::EmitOption option) {
    if (!literal_pool_.IsEmpty()) literal_pool_.Emit(option);

    checkpoint_ = NextCheckOffset();
  }

  ptrdiff_t NextCheckOffset() {
    return std::min(literal_pool_.NextCheckOffset(), BufferEndOffset());
  }

  void EnsureEmitFor(size_t amount) {
    ptrdiff_t offset = amount;
    if ((CursorOffset() + offset) > checkpoint_) {
      // Check if a pool need to be emitted.
      literal_pool_.CheckEmitFor(amount);
      // Ensure there's enough space for the emit, keep in mind the cursor will
      // have moved if a pool was emitted.
      if ((CursorOffset() + offset) > BufferEndOffset()) {
        EnsureSpaceFor(amount);
      }

      checkpoint_ = NextCheckOffset();
    }
  }

  // Set the current stack pointer, but don't generate any code.
  void SetStackPointer(const Register& stack_pointer) {
    VIXL_ASSERT(!TmpList()->IncludesAliasOf(stack_pointer));
    sp_ = stack_pointer;
  }

  // Return the current stack pointer, as set by SetStackPointer.
  const Register& StackPointer() const {
    return sp_;
  }

  CPURegList* TmpList() { return &tmp_list_; }
  CPURegList* FPTmpList() { return &fptmp_list_; }

  // Like printf, but print at run-time from generated code.
  //
  // The caller must ensure that arguments for floating-point placeholders
  // (such as %e, %f or %g) are FPRegisters, and that arguments for integer
  // placeholders are Registers.
  //
  // At the moment it is only possible to print the value of sp if it is the
  // current stack pointer. Otherwise, the MacroAssembler will automatically
  // update sp on every push (using BumpSystemStackPointer), so determining its
  // value is difficult.
  //
  // Format placeholders that refer to more than one argument, or to a specific
  // argument, are not supported. This includes formats like "%1$d" or "%.*d".
  //
  // This function automatically preserves caller-saved registers so that
  // calling code can use Printf at any point without having to worry about
  // corruption. The preservation mechanism generates a lot of code. If this is
  // a problem, preserve the important registers manually and then call
  // PrintfNoPreserve. Callee-saved registers are not used by Printf, and are
  // implicitly preserved.
  void Printf(const char * format,
              CPURegister arg0 = NoCPUReg,
              CPURegister arg1 = NoCPUReg,
              CPURegister arg2 = NoCPUReg,
              CPURegister arg3 = NoCPUReg);

  // Like Printf, but don't preserve any caller-saved registers, not even 'lr'.
  //
  // The return code from the system printf call will be returned in x0.
  void PrintfNoPreserve(const char * format,
                        const CPURegister& arg0 = NoCPUReg,
                        const CPURegister& arg1 = NoCPUReg,
                        const CPURegister& arg2 = NoCPUReg,
                        const CPURegister& arg3 = NoCPUReg);

  // Trace control when running the debug simulator.
  //
  // For example:
  //
  // __ Trace(LOG_REGS, TRACE_ENABLE);
  // Will add registers to the trace if it wasn't already the case.
  //
  // __ Trace(LOG_DISASM, TRACE_DISABLE);
  // Will stop logging disassembly. It has no effect if the disassembly wasn't
  // already being logged.
  void Trace(TraceParameters parameters, TraceCommand command);

  // Log the requested data independently of what is being traced.
  //
  // For example:
  //
  // __ Log(LOG_FLAGS)
  // Will output the flags.
  void Log(TraceParameters parameters);

  // Enable or disable instrumentation when an Instrument visitor is attached to
  // the simulator.
  void EnableInstrumentation();
  void DisableInstrumentation();

  // Add a marker to the instrumentation data produced by an Instrument visitor.
  // The name is a two character string that will be attached to the marker in
  // the output data.
  void AnnotateInstrumentation(const char* marker_name);

 private:
  // The actual Push and Pop implementations. These don't generate any code
  // other than that required for the push or pop. This allows
  // (Push|Pop)CPURegList to bundle together setup code for a large block of
  // registers.
  //
  // Note that size is per register, and is specified in bytes.
  void PushHelper(int count, int size,
                  const CPURegister& src0, const CPURegister& src1,
                  const CPURegister& src2, const CPURegister& src3);
  void PopHelper(int count, int size,
                 const CPURegister& dst0, const CPURegister& dst1,
                 const CPURegister& dst2, const CPURegister& dst3);

  // Perform necessary maintenance operations before a push or pop.
  //
  // Note that size is per register, and is specified in bytes.
  void PrepareForPush(int count, int size);
  void PrepareForPop(int count, int size);

#if DEBUG
  // Tell whether any of the macro instruction can be used. When false the
  // MacroAssembler will assert if a method which can emit a variable number
  // of instructions is called.
  bool allow_macro_instructions_;
#endif

  // The register to use as a stack pointer for stack operations.
  Register sp_;

  // Scratch registers available for use by the MacroAssembler.
  CPURegList tmp_list_;
  CPURegList fptmp_list_;

  LiteralPool literal_pool_;
  ptrdiff_t checkpoint_;
};


// Use this scope when you need a one-to-one mapping between methods and
// instructions. This scope prevents the MacroAssembler from being called and
// literal pools from being emitted. It also asserts the number of instructions
// emitted is what you specified when creating the scope.
class InstructionAccurateScope : public CodeBufferCheckScope {
 public:
  InstructionAccurateScope(MacroAssembler* masm,
                           int count,
                           AssertPolicy policy = kExactSize)
      : CodeBufferCheckScope(masm,
                             (count * kInstructionSize),
                             kCheck,
                             policy) {
    VIXL_ASSERT(policy != kNoAssert);
#ifdef DEBUG
    old_allow_macro_instructions_ = masm->AllowMacroInstructions();
    masm->SetAllowMacroInstructions(false);
#endif
  }

  ~InstructionAccurateScope() {
#ifdef DEBUG
    MacroAssembler* masm = reinterpret_cast<MacroAssembler*>(assm_);
    masm->SetAllowMacroInstructions(old_allow_macro_instructions_);
#endif
  }

 private:
#ifdef DEBUG
  bool old_allow_macro_instructions_;
#endif
};


class BlockLiteralPoolScope {
 public:
  explicit BlockLiteralPoolScope(MacroAssembler* masm) : masm_(masm) {
    masm_->BlockLiteralPool();
  }

  ~BlockLiteralPoolScope() {
    masm_->ReleaseLiteralPool();
  }

 private:
  MacroAssembler* masm_;
};


// This scope utility allows scratch registers to be managed safely. The
// MacroAssembler's TmpList() (and FPTmpList()) is used as a pool of scratch
// registers. These registers can be allocated on demand, and will be returned
// at the end of the scope.
//
// When the scope ends, the MacroAssembler's lists will be restored to their
// original state, even if the lists were modified by some other means.
class UseScratchRegisterScope {
 public:
  explicit UseScratchRegisterScope(MacroAssembler* masm)
      : available_(masm->TmpList()),
        availablefp_(masm->FPTmpList()),
        old_available_(available_->list()),
        old_availablefp_(availablefp_->list()) {
    VIXL_ASSERT(available_->type() == CPURegister::kRegister);
    VIXL_ASSERT(availablefp_->type() == CPURegister::kFPRegister);
  }


  ~UseScratchRegisterScope();


  bool IsAvailable(const CPURegister& reg) const;


  // Take a register from the appropriate temps list. It will be returned
  // automatically when the scope ends.
  Register AcquireW() { return AcquireNextAvailable(available_).W(); }
  Register AcquireX() { return AcquireNextAvailable(available_).X(); }
  FPRegister AcquireS() { return AcquireNextAvailable(availablefp_).S(); }
  FPRegister AcquireD() { return AcquireNextAvailable(availablefp_).D(); }


  Register AcquireSameSizeAs(const Register& reg);
  FPRegister AcquireSameSizeAs(const FPRegister& reg);


  // Explicitly release an acquired (or excluded) register, putting it back in
  // the appropriate temps list.
  void Release(const CPURegister& reg);


  // Make the specified registers available as scratch registers for the
  // duration of this scope.
  void Include(const CPURegList& list);
  void Include(const Register& reg1,
               const Register& reg2 = NoReg,
               const Register& reg3 = NoReg,
               const Register& reg4 = NoReg);
  void Include(const FPRegister& reg1,
               const FPRegister& reg2 = NoFPReg,
               const FPRegister& reg3 = NoFPReg,
               const FPRegister& reg4 = NoFPReg);


  // Make sure that the specified registers are not available in this scope.
  // This can be used to prevent helper functions from using sensitive
  // registers, for example.
  void Exclude(const CPURegList& list);
  void Exclude(const Register& reg1,
               const Register& reg2 = NoReg,
               const Register& reg3 = NoReg,
               const Register& reg4 = NoReg);
  void Exclude(const FPRegister& reg1,
               const FPRegister& reg2 = NoFPReg,
               const FPRegister& reg3 = NoFPReg,
               const FPRegister& reg4 = NoFPReg);
  void Exclude(const CPURegister& reg1,
               const CPURegister& reg2 = NoCPUReg,
               const CPURegister& reg3 = NoCPUReg,
               const CPURegister& reg4 = NoCPUReg);


  // Prevent any scratch registers from being used in this scope.
  void ExcludeAll();


 private:
  static CPURegister AcquireNextAvailable(CPURegList* available);

  static void ReleaseByCode(CPURegList* available, int code);

  static void ReleaseByRegList(CPURegList* available,
                               RegList regs);

  static void IncludeByRegList(CPURegList* available,
                               RegList exclude);

  static void ExcludeByRegList(CPURegList* available,
                               RegList exclude);

  // Available scratch registers.
  CPURegList* available_;     // kRegister
  CPURegList* availablefp_;   // kFPRegister

  // The state of the available lists at the start of this scope.
  RegList old_available_;     // kRegister
  RegList old_availablefp_;   // kFPRegister
};


}  // namespace vixl

#endif  // VIXL_A64_MACRO_ASSEMBLER_A64_H_
