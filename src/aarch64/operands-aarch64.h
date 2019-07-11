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

  // Construct an empty CPURegList with the specified size and type. If `size`
  // is CPURegister::kUnknownSize and the register type requires a size, a valid
  // but unspecified default will be picked.
  static CPURegList Empty(CPURegister::RegisterType type,
                          unsigned size = CPURegister::kUnknownSize) {
    return CPURegList(type, GetDefaultSizeFor(type, size), 0);
  }

  // Construct a CPURegList with all possible registers with the specified size
  // and type. If `size` is CPURegister::kUnknownSize and the register type
  // requires a size, a valid but unspecified default will be picked.
  static CPURegList All(CPURegister::RegisterType type,
                        unsigned size = CPURegister::kUnknownSize) {
    unsigned number_of_registers = (CPURegister::GetMaxCodeFor(type) + 1);
    RegList list = (static_cast<RegList>(1) << number_of_registers) - 1;
    if (type == CPURegister::kRegister) {
      // GetMaxCodeFor(kRegister) ignores SP, so explicitly include it.
      list |= (static_cast<RegList>(1) << kSPRegInternalCode);
    }
    return CPURegList(type, GetDefaultSizeFor(type, size), list);
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

  // Find the register in this list that appears in `mask` with the lowest or
  // highest code, remove it from the list and return it as a CPURegister. If
  // the list is empty, leave it unchanged and return NoCPUReg.
  CPURegister PopLowestIndex(RegList mask = ~static_cast<RegList>(0));
  CPURegister PopHighestIndex(RegList mask = ~static_cast<RegList>(0));

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
    return (GetBank() == other.GetBank()) && IncludesAliasOf(other.GetCode());
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
  // If `size` is CPURegister::kUnknownSize and the type requires a known size,
  // then return an arbitrary-but-valid size.
  //
  // Otherwise, the size is checked for validity and returned unchanged.
  static unsigned GetDefaultSizeFor(CPURegister::RegisterType type,
                                    unsigned size) {
    if (size == CPURegister::kUnknownSize) {
      if (type == CPURegister::kRegister) size = kXRegSize;
      if (type == CPURegister::kVRegister) size = kQRegSize;
      // All other types require kUnknownSize.
    }
    VIXL_ASSERT(CPURegister(0, size, type).IsValid());
    return size;
  }

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

class IntegerOperand;

// Operand.
class Operand {
 public:
  // #<immediate>
  // where <immediate> is int64_t.
  // This is allowed to be an implicit constructor because Operand is
  // a wrapper class that doesn't normally perform any type conversion.
  Operand(int64_t immediate);  // NOLINT(runtime/explicit)

  Operand(IntegerOperand immediate);  // NOLINT(runtime/explicit)

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

// Represent a signed or unsigned integer operand.
//
// This is designed to make instructions which naturally accept a _signed_
// immediate easier to implement and use, when we also want users to be able to
// specify raw-bits values (such as with hexadecimal constants). The advantage
// of this class over a simple uint64_t (with implicit C++ sign-extension) is
// that this class can strictly check the range of allowed values. With a simple
// uint64_t, it is impossible to distinguish -1 from UINT64_MAX.
//
// For example, these instructions are equivalent:
//
//     __ Insr(z0.VnB(), -1);
//     __ Insr(z0.VnB(), 0xff);
//
// ... as are these:
//
//     __ Insr(z0.VnD(), -1);
//     __ Insr(z0.VnD(), 0xffffffffffffffff);
//
// ... but this is invalid:
//
//     __ Insr(z0.VnB(), 0xffffffffffffffff);  // Too big for B-sized lanes.
class IntegerOperand {
 public:
#define VIXL_INT_TYPES(V) \
  V(char) V(short) V(int) V(long) V(long long)  // NOLINT(runtime/int)
#define VIXL_DECL_INT_OVERLOADS(T)                                        \
  /* These are allowed to be implicit constructors because this is a */   \
  /* wrapper class that doesn't normally perform any type conversion. */  \
  IntegerOperand(signed T immediate) /* NOLINT(runtime/explicit) */       \
      : raw_bits_(immediate),        /* Allow implicit sign-extension. */ \
        is_negative_(immediate < 0) {}                                    \
  IntegerOperand(unsigned T immediate) /* NOLINT(runtime/explicit) */     \
      : raw_bits_(immediate),                                             \
        is_negative_(false) {}
  VIXL_INT_TYPES(VIXL_DECL_INT_OVERLOADS)
#undef VIXL_DECL_INT_OVERLOADS
#undef VIXL_INT_TYPES

  // TODO: `Operand` can currently only hold an int64_t, so some large, unsigned
  // values will be misrepresented here.
  explicit IntegerOperand(const Operand& operand)
      : raw_bits_(operand.GetEquivalentImmediate()),
        is_negative_(operand.GetEquivalentImmediate() < 0) {}

  bool IsIntN(unsigned n) const {
    return is_negative_ ? vixl::IsIntN(n, RawbitsToInt64(raw_bits_))
                        : vixl::IsIntN(n, raw_bits_);
  }
  bool IsUintN(unsigned n) const {
    return !is_negative_ && vixl::IsUintN(n, raw_bits_);
  }

  bool FitsInBits(unsigned n) const {
    return is_negative_ ? IsIntN(n) : IsUintN(n);
  }
  bool FitsInLane(const CPURegister& zd) const {
    return FitsInBits(zd.GetLaneSizeInBits());
  }

  // Cast a value in the range [INT<n>_MIN, UINT<n>_MAX] to an unsigned integer
  // in the range [0, UINT<n>_MAX] (using two's complement mapping).
  uint64_t AsUintN(unsigned n) const {
    USE(n);
    VIXL_ASSERT(FitsInBits(n));
    return raw_bits_ & GetUintMask(n);
  }

  // Cast a value in the range [INT<n>_MIN, UINT<n>_MAX] to a signed integer in
  // the range [INT<n>_MIN, INT<n>_MAX] (using two's complement mapping).
  int64_t AsIntN(unsigned n) const {
    USE(n);
    VIXL_ASSERT(FitsInBits(n));
    return ExtractSignedBitfield64(n - 1, 0, raw_bits_);
  }

  // Several instructions encode a signed int<N>_t, which is then (optionally)
  // left-shifted and sign-extended to a Z register lane with a size which may
  // be larger than N. This helper tries to find an int<N>_t such that the
  // IntegerOperand's arithmetic value is reproduced in each lane.
  //
  // This is the mechanism that allows `Insr(z0.VnB(), 0xff)` to be treated as
  // `Insr(z0.VnB(), -1)`.
  template <unsigned N, unsigned kShift, typename T>
  bool TryEncodeAsShiftedIntNForLane(const CPURegister& zd, T* imm) const {
    VIXL_STATIC_ASSERT(std::numeric_limits<T>::digits > N);
    VIXL_ASSERT(FitsInLane(zd));
    if ((raw_bits_ & GetUintMask(kShift)) != 0) return false;

    // Reverse the specified left-shift.
    IntegerOperand unshifted(*this);
    unshifted.ArithmeticShiftRight(kShift);

    if (unshifted.IsIntN(N)) {
      // This is trivial, since sign-extension produces the same arithmetic
      // value irrespective of the destination size.
      *imm = static_cast<T>(unshifted.AsIntN(N));
      return true;
    }

    // Otherwise, we might be able to use the sign-extension to produce the
    // desired bit pattern. We can only do this for values in the range
    // [INT<N>_MAX + 1, UINT<N>_MAX], where the highest set bit is the sign bit.
    //
    // The lane size has to be adjusted to compensate for `kShift`, since the
    // high bits will be dropped when the encoded value is left-shifted.
    if (unshifted.IsUintN(zd.GetLaneSizeInBits() - kShift)) {
      int64_t encoded = unshifted.AsIntN(zd.GetLaneSizeInBits() - kShift);
      if (vixl::IsIntN(N, encoded)) {
        *imm = static_cast<T>(encoded);
        return true;
      }
    }
    return false;
  }

  // As above, but `kShift` is written to the `*shift` parameter on success, so
  // that it is easy to chain calls like this:
  //
  //     if (imm.TryEncodeAsShiftedIntNForLane<8, 0>(zd, &imm8, &shift) ||
  //         imm.TryEncodeAsShiftedIntNForLane<8, 8>(zd, &imm8, &shift)) {
  //       insn(zd, imm8, shift)
  //     }
  template <unsigned N, unsigned kShift, typename T, typename S>
  bool TryEncodeAsShiftedIntNForLane(const CPURegister& zd,
                                     T* imm,
                                     S* shift) const {
    if (TryEncodeAsShiftedIntNForLane<N, kShift>(zd, imm)) {
      *shift = kShift;
      return true;
    }
    return false;
  }

  // As above, but assume that `kShift` is 0.
  template <unsigned N, typename T>
  bool TryEncodeAsIntNForLane(const CPURegister& zd, T* imm) const {
    return TryEncodeAsShiftedIntNForLane<N, 0>(zd, imm);
  }

  bool IsZero() const { return raw_bits_ == 0; }

 private:
  // Shift the arithmetic value right, with sign extension if is_negative_.
  void ArithmeticShiftRight(int shift) {
    VIXL_ASSERT((shift >= 0) && (shift < 64));
    if (shift == 0) return;
    if (is_negative_) {
      raw_bits_ = ExtractSignedBitfield64(63, shift, raw_bits_);
    } else {
      raw_bits_ >>= shift;
    }
  }

  uint64_t raw_bits_;
  bool is_negative_;
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
