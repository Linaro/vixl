// Copyright 2016, VIXL authors
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

#ifndef VIXL_AARCH64_OPERANDS_AARCH64_H_
#define VIXL_AARCH64_OPERANDS_AARCH64_H_

#include <sstream>
#include <string>

#include "instructions-aarch64.h"
#include "registers-aarch64.h"

namespace vixl {
namespace aarch64 {

// Lists of registers.
class CPURegList {
 public:
  explicit CPURegList(CPURegister reg1,
                      CPURegister reg2 = NoCPUReg,
                      CPURegister reg3 = NoCPUReg,
                      CPURegister reg4 = NoCPUReg)
      : list_(reg1.GetBit() | reg2.GetBit() | reg3.GetBit() | reg4.GetBit()),
        size_(reg1.GetSizeInBits()),
        type_(reg1.GetType()) {
    VIXL_ASSERT(AreSameSizeAndType(reg1, reg2, reg3, reg4));
    VIXL_ASSERT(IsValid());
  }

  CPURegList(CPURegister::RegisterType type, unsigned size, RegList list)
      : list_(list), size_(size), type_(type) {
    VIXL_ASSERT(IsValid());
  }

  CPURegList(CPURegister::RegisterType type,
             unsigned size,
             unsigned first_reg,
             unsigned last_reg)
      : size_(size), type_(type) {
    VIXL_ASSERT(
        ((type == CPURegister::kRegister) && (last_reg < kNumberOfRegisters)) ||
        ((type == CPURegister::kVRegister) &&
         (last_reg < kNumberOfVRegisters)));
    VIXL_ASSERT(last_reg >= first_reg);
    list_ = (UINT64_C(1) << (last_reg + 1)) - 1;
    list_ &= ~((UINT64_C(1) << first_reg) - 1);
    VIXL_ASSERT(IsValid());
  }

  CPURegister::RegisterType GetType() const {
    VIXL_ASSERT(IsValid());
    return type_;
  }
  VIXL_DEPRECATED("GetType", CPURegister::RegisterType type() const) {
    return GetType();
  }

  CPURegister::RegisterBank GetBank() const {
    return CPURegister::GetBankFor(GetType());
  }

  // Combine another CPURegList into this one. Registers that already exist in
  // this list are left unchanged. The type and size of the registers in the
  // 'other' list must match those in this list.
  void Combine(const CPURegList& other) {
    VIXL_ASSERT(IsValid());
    VIXL_ASSERT(other.GetType() == type_);
    VIXL_ASSERT(other.GetRegisterSizeInBits() == size_);
    list_ |= other.GetList();
  }

  // Remove every register in the other CPURegList from this one. Registers that
  // do not exist in this list are ignored. The type and size of the registers
  // in the 'other' list must match those in this list.
  void Remove(const CPURegList& other) {
    VIXL_ASSERT(IsValid());
    VIXL_ASSERT(other.GetType() == type_);
    VIXL_ASSERT(other.GetRegisterSizeInBits() == size_);
    list_ &= ~other.GetList();
  }

  // Variants of Combine and Remove which take a single register.
  void Combine(const CPURegister& other) {
    VIXL_ASSERT(other.GetType() == type_);
    VIXL_ASSERT(other.GetSizeInBits() == size_);
    Combine(other.GetCode());
  }

  void Remove(const CPURegister& other) {
    VIXL_ASSERT(other.GetType() == type_);
    VIXL_ASSERT(other.GetSizeInBits() == size_);
    Remove(other.GetCode());
  }

  // Variants of Combine and Remove which take a single register by its code;
  // the type and size of the register is inferred from this list.
  void Combine(int code) {
    VIXL_ASSERT(IsValid());
    VIXL_ASSERT(CPURegister(code, size_, type_).IsValid());
    list_ |= (UINT64_C(1) << code);
  }

  void Remove(int code) {
    VIXL_ASSERT(IsValid());
    VIXL_ASSERT(CPURegister(code, size_, type_).IsValid());
    list_ &= ~(UINT64_C(1) << code);
  }

  static CPURegList Union(const CPURegList& list_1, const CPURegList& list_2) {
    VIXL_ASSERT(list_1.type_ == list_2.type_);
    VIXL_ASSERT(list_1.size_ == list_2.size_);
    return CPURegList(list_1.type_, list_1.size_, list_1.list_ | list_2.list_);
  }
  static CPURegList Union(const CPURegList& list_1,
                          const CPURegList& list_2,
                          const CPURegList& list_3);
  static CPURegList Union(const CPURegList& list_1,
                          const CPURegList& list_2,
                          const CPURegList& list_3,
                          const CPURegList& list_4);

  static CPURegList Intersection(const CPURegList& list_1,
                                 const CPURegList& list_2) {
    VIXL_ASSERT(list_1.type_ == list_2.type_);
    VIXL_ASSERT(list_1.size_ == list_2.size_);
    return CPURegList(list_1.type_, list_1.size_, list_1.list_ & list_2.list_);
  }
  static CPURegList Intersection(const CPURegList& list_1,
                                 const CPURegList& list_2,
                                 const CPURegList& list_3);
  static CPURegList Intersection(const CPURegList& list_1,
                                 const CPURegList& list_2,
                                 const CPURegList& list_3,
                                 const CPURegList& list_4);

  bool Overlaps(const CPURegList& other) const {
    return (type_ == other.type_) && ((list_ & other.list_) != 0);
  }

  RegList GetList() const {
    VIXL_ASSERT(IsValid());
    return list_;
  }
  VIXL_DEPRECATED("GetList", RegList list() const) { return GetList(); }

  void SetList(RegList new_list) {
    VIXL_ASSERT(IsValid());
    list_ = new_list;
  }
  VIXL_DEPRECATED("SetList", void set_list(RegList new_list)) {
    return SetList(new_list);
  }

  // Remove all callee-saved registers from the list. This can be useful when
  // preparing registers for an AAPCS64 function call, for example.
  void RemoveCalleeSaved();

  CPURegister PopLowestIndex();
  CPURegister PopHighestIndex();

  // AAPCS64 callee-saved registers.
  static CPURegList GetCalleeSaved(unsigned size = kXRegSize);
  static CPURegList GetCalleeSavedV(unsigned size = kDRegSize);

  // AAPCS64 caller-saved registers. Note that this includes lr.
  // TODO(all): Determine how we handle d8-d15 being callee-saved, but the top
  // 64-bits being caller-saved.
  static CPURegList GetCallerSaved(unsigned size = kXRegSize);
  static CPURegList GetCallerSavedV(unsigned size = kDRegSize);

  bool IsEmpty() const {
    VIXL_ASSERT(IsValid());
    return list_ == 0;
  }

  bool IncludesAliasOf(const CPURegister& other) const {
    VIXL_ASSERT(IsValid());
    return (GetBank() == other.GetBank()) && ((other.GetBit() & list_) != 0);
  }

  bool IncludesAliasOf(int code) const {
    VIXL_ASSERT(IsValid());
    return (((static_cast<RegList>(1) << code) & list_) != 0);
  }

  int GetCount() const {
    VIXL_ASSERT(IsValid());
    return CountSetBits(list_);
  }
  VIXL_DEPRECATED("GetCount", int Count()) const { return GetCount(); }

  int GetRegisterSizeInBits() const {
    VIXL_ASSERT(IsValid());
    return size_;
  }
  VIXL_DEPRECATED("GetRegisterSizeInBits", int RegisterSizeInBits() const) {
    return GetRegisterSizeInBits();
  }

  int GetRegisterSizeInBytes() const {
    int size_in_bits = GetRegisterSizeInBits();
    VIXL_ASSERT((size_in_bits % 8) == 0);
    return size_in_bits / 8;
  }
  VIXL_DEPRECATED("GetRegisterSizeInBytes", int RegisterSizeInBytes() const) {
    return GetRegisterSizeInBytes();
  }

  unsigned GetTotalSizeInBytes() const {
    VIXL_ASSERT(IsValid());
    return GetRegisterSizeInBytes() * GetCount();
  }
  VIXL_DEPRECATED("GetTotalSizeInBytes", unsigned TotalSizeInBytes() const) {
    return GetTotalSizeInBytes();
  }

 private:
  RegList list_;
  int size_;
  CPURegister::RegisterType type_;

  bool IsValid() const;
};


// AAPCS64 callee-saved registers.
extern const CPURegList kCalleeSaved;
extern const CPURegList kCalleeSavedV;


// AAPCS64 caller-saved registers. Note that this includes lr.
extern const CPURegList kCallerSaved;
extern const CPURegList kCallerSavedV;


// Operand.
class Operand {
 public:
  // #<immediate>
  // where <immediate> is int64_t.
  // This is allowed to be an implicit constructor because Operand is
  // a wrapper class that doesn't normally perform any type conversion.
  Operand(int64_t immediate = 0);  // NOLINT(runtime/explicit)

  // rm, {<shift> #<shift_amount>}
  // where <shift> is one of {LSL, LSR, ASR, ROR}.
  //       <shift_amount> is uint6_t.
  // This is allowed to be an implicit constructor because Operand is
  // a wrapper class that doesn't normally perform any type conversion.
  Operand(Register reg,
          Shift shift = LSL,
          unsigned shift_amount = 0);  // NOLINT(runtime/explicit)

  // rm, {<extend> {#<shift_amount>}}
  // where <extend> is one of {UXTB, UXTH, UXTW, UXTX, SXTB, SXTH, SXTW, SXTX}.
  //       <shift_amount> is uint2_t.
  explicit Operand(Register reg, Extend extend, unsigned shift_amount = 0);

  bool IsImmediate() const;
  bool IsPlainRegister() const;
  bool IsShiftedRegister() const;
  bool IsExtendedRegister() const;
  bool IsZero() const;

  // This returns an LSL shift (<= 4) operand as an equivalent extend operand,
  // which helps in the encoding of instructions that use the stack pointer.
  Operand ToExtendedRegister() const;

  int64_t GetImmediate() const {
    VIXL_ASSERT(IsImmediate());
    return immediate_;
  }
  VIXL_DEPRECATED("GetImmediate", int64_t immediate() const) {
    return GetImmediate();
  }

  int64_t GetEquivalentImmediate() const {
    return IsZero() ? 0 : GetImmediate();
  }

  Register GetRegister() const {
    VIXL_ASSERT(IsShiftedRegister() || IsExtendedRegister());
    return reg_;
  }
  VIXL_DEPRECATED("GetRegister", Register reg() const) { return GetRegister(); }
  Register GetBaseRegister() const { return GetRegister(); }

  Shift GetShift() const {
    VIXL_ASSERT(IsShiftedRegister());
    return shift_;
  }
  VIXL_DEPRECATED("GetShift", Shift shift() const) { return GetShift(); }

  Extend GetExtend() const {
    VIXL_ASSERT(IsExtendedRegister());
    return extend_;
  }
  VIXL_DEPRECATED("GetExtend", Extend extend() const) { return GetExtend(); }

  unsigned GetShiftAmount() const {
    VIXL_ASSERT(IsShiftedRegister() || IsExtendedRegister());
    return shift_amount_;
  }
  VIXL_DEPRECATED("GetShiftAmount", unsigned shift_amount() const) {
    return GetShiftAmount();
  }

 private:
  int64_t immediate_;
  Register reg_;
  Shift shift_;
  Extend extend_;
  unsigned shift_amount_;
};


// MemOperand represents the addressing mode of a load or store instruction.
class MemOperand {
 public:
  // Creates an invalid `MemOperand`.
  MemOperand();
  explicit MemOperand(Register base,
                      int64_t offset = 0,
                      AddrMode addrmode = Offset);
  MemOperand(Register base,
             Register regoffset,
             Shift shift = LSL,
             unsigned shift_amount = 0);
  MemOperand(Register base,
             Register regoffset,
             Extend extend,
             unsigned shift_amount = 0);
  MemOperand(Register base, const Operand& offset, AddrMode addrmode = Offset);

  const Register& GetBaseRegister() const { return base_; }
  VIXL_DEPRECATED("GetBaseRegister", const Register& base() const) {
    return GetBaseRegister();
  }

  const Register& GetRegisterOffset() const { return regoffset_; }
  VIXL_DEPRECATED("GetRegisterOffset", const Register& regoffset() const) {
    return GetRegisterOffset();
  }

  int64_t GetOffset() const { return offset_; }
  VIXL_DEPRECATED("GetOffset", int64_t offset() const) { return GetOffset(); }

  AddrMode GetAddrMode() const { return addrmode_; }
  VIXL_DEPRECATED("GetAddrMode", AddrMode addrmode() const) {
    return GetAddrMode();
  }

  Shift GetShift() const { return shift_; }
  VIXL_DEPRECATED("GetShift", Shift shift() const) { return GetShift(); }

  Extend GetExtend() const { return extend_; }
  VIXL_DEPRECATED("GetExtend", Extend extend() const) { return GetExtend(); }

  unsigned GetShiftAmount() const { return shift_amount_; }
  VIXL_DEPRECATED("GetShiftAmount", unsigned shift_amount() const) {
    return GetShiftAmount();
  }

  // True for MemOperands which represent something like [x0].
  // Currently, this will also return true for [x0, #0], because MemOperand has
  // no way to distinguish the two.
  bool IsPlainRegister() const;

  // True for MemOperands which represent something like [x0], or for compound
  // MemOperands which are functionally equivalent, such as [x0, #0], [x0, xzr]
  // or [x0, wzr, UXTW #3].
  bool IsEquivalentToPlainRegister() const;

  bool IsImmediateOffset() const;
  bool IsRegisterOffset() const;
  bool IsPreIndex() const;
  bool IsPostIndex() const;

  void AddOffset(int64_t offset);

  bool IsValid() const {
    return base_.IsValid() &&
           ((addrmode_ == Offset) || (addrmode_ == PreIndex) ||
            (addrmode_ == PostIndex)) &&
           ((shift_ == NO_SHIFT) || (extend_ == NO_EXTEND)) &&
           ((offset_ == 0) || !regoffset_.IsValid());
  }

  bool Equals(const MemOperand& other) const {
    return base_.Is(other.base_) && regoffset_.Is(other.regoffset_) &&
           (offset_ == other.offset_) && (addrmode_ == other.addrmode_) &&
           (shift_ == other.shift_) && (extend_ == other.extend_) &&
           (shift_amount_ == other.shift_amount_);
  }

 private:
  Register base_;
  Register regoffset_;
  int64_t offset_;
  AddrMode addrmode_;
  Shift shift_;
  Extend extend_;
  unsigned shift_amount_;
};

// This an abstraction that can represent a register or memory location. The
// `MacroAssembler` provides helpers to move data between generic operands.
class GenericOperand {
 public:
  GenericOperand() { VIXL_ASSERT(!IsValid()); }
  GenericOperand(const CPURegister& reg);  // NOLINT(runtime/explicit)
  GenericOperand(const MemOperand& mem_op,
                 size_t mem_op_size = 0);  // NOLINT(runtime/explicit)

  bool IsValid() const { return cpu_register_.IsValid() != mem_op_.IsValid(); }

  bool Equals(const GenericOperand& other) const;

  bool IsCPURegister() const {
    VIXL_ASSERT(IsValid());
    return cpu_register_.IsValid();
  }

  bool IsRegister() const {
    return IsCPURegister() && cpu_register_.IsRegister();
  }

  bool IsVRegister() const {
    return IsCPURegister() && cpu_register_.IsVRegister();
  }

  bool IsSameCPURegisterType(const GenericOperand& other) {
    return IsCPURegister() && other.IsCPURegister() &&
           GetCPURegister().IsSameType(other.GetCPURegister());
  }

  bool IsMemOperand() const {
    VIXL_ASSERT(IsValid());
    return mem_op_.IsValid();
  }

  CPURegister GetCPURegister() const {
    VIXL_ASSERT(IsCPURegister());
    return cpu_register_;
  }

  MemOperand GetMemOperand() const {
    VIXL_ASSERT(IsMemOperand());
    return mem_op_;
  }

  size_t GetMemOperandSizeInBytes() const {
    VIXL_ASSERT(IsMemOperand());
    return mem_op_size_;
  }

  size_t GetSizeInBytes() const {
    return IsCPURegister() ? cpu_register_.GetSizeInBytes()
                           : GetMemOperandSizeInBytes();
  }

  size_t GetSizeInBits() const { return GetSizeInBytes() * kBitsPerByte; }

 private:
  CPURegister cpu_register_;
  MemOperand mem_op_;
  // The size of the memory region pointed to, in bytes.
  // We only support sizes up to X/D register sizes.
  size_t mem_op_size_;
};
}
}  // namespace vixl::aarch64

#endif  // VIXL_AARCH64_OPERANDS_AARCH64_H_
