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

#ifndef VIXL_A64_SIMULATOR_A64_H_
#define VIXL_A64_SIMULATOR_A64_H_

#include "globals.h"
#include "utils.h"
#include "a64/instructions-a64.h"
#include "a64/assembler-a64.h"
#include "a64/disasm-a64.h"
#include "a64/instrument-a64.h"

namespace vixl {

enum ReverseByteMode {
  Reverse16 = 0,
  Reverse32 = 1,
  Reverse64 = 2
};

// Printf. See debugger-a64.h for more informations on pseudo instructions.
//  - type: CPURegister::RegisterType stored as a uint32_t.
//
// Simulate a call to printf.
//
// Floating-point and integer arguments are passed in separate sets of
// registers in AAPCS64 (even for varargs functions), so it is not possible to
// determine the type of location of each argument without some information
// about the values that were passed in. This information could be retrieved
// from the printf format string, but the format string is not trivial to
// parse so we encode the relevant information with the HLT instruction under
// the type argument. Therefore the interface is:
//    x0: The format string
// x1-x7: Optional arguments, if type == CPURegister::kRegister
// d0-d7: Optional arguments, if type == CPURegister::kFPRegister
const Instr kPrintfOpcode = 0xdeb1;
const unsigned kPrintfTypeOffset = 1 * kInstructionSize;
const unsigned kPrintfLength = 2 * kInstructionSize;


// The proper way to initialize a simulated system register (such as NZCV) is as
// follows:
//  SimSystemRegister nzcv = SimSystemRegister::DefaultValueFor(NZCV);
class SimSystemRegister {
 public:
  // The default constructor represents a register which has no writable bits.
  // It is not possible to set its value to anything other than 0.
  SimSystemRegister() : value_(0), write_ignore_mask_(0xffffffff) { }

  inline uint32_t RawValue() const {
    return value_;
  }

  inline void SetRawValue(uint32_t new_value) {
    value_ = (value_ & write_ignore_mask_) | (new_value & ~write_ignore_mask_);
  }

  inline uint32_t Bits(int msb, int lsb) const {
    return unsigned_bitextract_32(msb, lsb, value_);
  }

  inline int32_t SignedBits(int msb, int lsb) const {
    return signed_bitextract_32(msb, lsb, value_);
  }

  void SetBits(int msb, int lsb, uint32_t bits);

  // Default system register values.
  static SimSystemRegister DefaultValueFor(SystemRegister id);

#define DEFINE_GETTER(Name, HighBit, LowBit, Func)                            \
  inline uint32_t Name() const { return Func(HighBit, LowBit); }              \
  inline void Set##Name(uint32_t bits) { SetBits(HighBit, LowBit, bits); }
#define DEFINE_WRITE_IGNORE_MASK(Name, Mask)                                  \
  static const uint32_t Name##WriteIgnoreMask = ~static_cast<uint32_t>(Mask);

  SYSTEM_REGISTER_FIELDS_LIST(DEFINE_GETTER, DEFINE_WRITE_IGNORE_MASK)

#undef DEFINE_ZERO_BITS
#undef DEFINE_GETTER

 protected:
  // Most system registers only implement a few of the bits in the word. Other
  // bits are "read-as-zero, write-ignored". The write_ignore_mask argument
  // describes the bits which are not modifiable.
  SimSystemRegister(uint32_t value, uint32_t write_ignore_mask)
      : value_(value), write_ignore_mask_(write_ignore_mask) { }

  uint32_t value_;
  uint32_t write_ignore_mask_;
};


// Represent a register (r0-r31, v0-v31).
template<int kSizeInBytes>
class SimRegisterBase {
 public:
  template<typename T>
  void Set(T new_value, unsigned size = sizeof(T)) {
    VIXL_ASSERT(size <= kSizeInBytes);
    VIXL_ASSERT(size <= sizeof(new_value));
    // All AArch64 registers are zero-extending; Writing a W register clears the
    // top bits of the corresponding X register.
    memset(value_, 0, kSizeInBytes);
    memcpy(value_, &new_value, size);
  }

  // Copy 'size' bytes of the register to the result, and zero-extend to fill
  // the result.
  template<typename T>
  T Get(unsigned size = sizeof(T)) const {
    VIXL_ASSERT(size <= kSizeInBytes);
    T result;
    memset(&result, 0, sizeof(result));
    memcpy(&result, value_, size);
    return result;
  }

 protected:
  uint8_t value_[kSizeInBytes];
};
typedef SimRegisterBase<kXRegSizeInBytes> SimRegister;      // r0-r31
typedef SimRegisterBase<kDRegSizeInBytes> SimFPRegister;    // v0-v31


class Simulator : public DecoderVisitor {
 public:
  explicit Simulator(Decoder* decoder, FILE* stream = stdout);
  ~Simulator();

  void ResetState();

  // Run the simulator.
  virtual void Run();
  void RunFrom(Instruction* first);

  // Simulation helpers.
  inline Instruction* pc() { return pc_; }
  inline void set_pc(Instruction* new_pc) {
    pc_ = new_pc;
    pc_modified_ = true;
  }

  inline void increment_pc() {
    if (!pc_modified_) {
      pc_ = pc_->NextInstruction();
    }

    pc_modified_ = false;
  }

  inline void ExecuteInstruction() {
    // The program counter should always be aligned.
    VIXL_ASSERT(IsWordAligned(pc_));
    decoder_->Decode(pc_);
    increment_pc();
  }

  // Declare all Visitor functions.
  #define DECLARE(A)  void Visit##A(Instruction* instr);
  VISITOR_LIST(DECLARE)
  #undef DECLARE

  // Register accessors.

  // Return 'size' bits of the value of an integer register, as the specified
  // type. The value is zero-extended to fill the result.
  //
  // The only supported values of 'size' are kXRegSize and kWRegSize.
  template<typename T>
  inline T reg(unsigned size, unsigned code,
               Reg31Mode r31mode = Reg31IsZeroRegister) const {
    unsigned size_in_bytes = size / 8;
    VIXL_ASSERT(size_in_bytes <= sizeof(T));
    VIXL_ASSERT((size == kXRegSize) || (size == kWRegSize));
    VIXL_ASSERT(code < kNumberOfRegisters);

    if ((code == 31) && (r31mode == Reg31IsZeroRegister)) {
      T result;
      memset(&result, 0, sizeof(result));
      return result;
    }
    return registers_[code].Get<T>(size_in_bytes);
  }

  // Like reg(), but infer the access size from the template type.
  template<typename T>
  inline T reg(unsigned code, Reg31Mode r31mode = Reg31IsZeroRegister) const {
    return reg<T>(sizeof(T) * 8, code, r31mode);
  }

  // Common specialized accessors for the reg() template.
  inline int32_t wreg(unsigned code,
                      Reg31Mode r31mode = Reg31IsZeroRegister) const {
    return reg<int32_t>(code, r31mode);
  }

  inline int64_t xreg(unsigned code,
                      Reg31Mode r31mode = Reg31IsZeroRegister) const {
    return reg<int64_t>(code, r31mode);
  }

  inline int64_t reg(unsigned size, unsigned code,
                     Reg31Mode r31mode = Reg31IsZeroRegister) const {
    return reg<int64_t>(size, code, r31mode);
  }

  // Write 'size' bits of 'value' into an integer register. The value is
  // zero-extended. This behaviour matches AArch64 register writes.
  //
  // The only supported values of 'size' are kXRegSize and kWRegSize.
  template<typename T>
  inline void set_reg(unsigned size, unsigned code, T value,
                      Reg31Mode r31mode = Reg31IsZeroRegister) {
    unsigned size_in_bytes = size / 8;
    VIXL_ASSERT(size_in_bytes <= sizeof(T));
    VIXL_ASSERT((size == kXRegSize) || (size == kWRegSize));
    VIXL_ASSERT(code < kNumberOfRegisters);

    if ((code == 31) && (r31mode == Reg31IsZeroRegister)) {
      return;
    }
    return registers_[code].Set(value, size_in_bytes);
  }

  // Like set_reg(), but infer the access size from the template type.
  template<typename T>
  inline void set_reg(unsigned code, T value,
                      Reg31Mode r31mode = Reg31IsZeroRegister) {
    set_reg(sizeof(value) * 8, code, value, r31mode);
  }

  // Common specialized accessors for the set_reg() template.
  inline void set_wreg(unsigned code, int32_t value,
                       Reg31Mode r31mode = Reg31IsZeroRegister) {
    set_reg(kWRegSize, code, value, r31mode);
  }

  inline void set_xreg(unsigned code, int64_t value,
                       Reg31Mode r31mode = Reg31IsZeroRegister) {
    set_reg(kXRegSize, code, value, r31mode);
  }

  // Commonly-used special cases.
  template<typename T>
  inline void set_lr(T value) {
    set_reg(kLinkRegCode, value);
  }

  template<typename T>
  inline void set_sp(T value) {
    set_reg(31, value, Reg31IsStackPointer);
  }

  // Return 'size' bits of the value of a floating-point register, as the
  // specified type. The value is zero-extended to fill the result.
  //
  // The only supported values of 'size' are kDRegSize and kSRegSize.
  template<typename T>
  inline T fpreg(unsigned size, unsigned code) const {
    unsigned size_in_bytes = size / 8;
    VIXL_ASSERT(size_in_bytes <= sizeof(T));
    VIXL_ASSERT((size == kDRegSize) || (size == kSRegSize));
    VIXL_ASSERT(code < kNumberOfFPRegisters);
    return fpregisters_[code].Get<T>(size_in_bytes);
  }

  // Like fpreg(), but infer the access size from the template type.
  template<typename T>
  inline T fpreg(unsigned code) const {
    return fpreg<T>(sizeof(T) * 8, code);
  }

  // Common specialized accessors for the fpreg() template.
  inline float sreg(unsigned code) const {
    return fpreg<float>(code);
  }

  inline uint32_t sreg_bits(unsigned code) const {
    return fpreg<uint32_t>(code);
  }

  inline double dreg(unsigned code) const {
    return fpreg<double>(code);
  }

  inline uint64_t dreg_bits(unsigned code) const {
    return fpreg<uint64_t>(code);
  }

  inline double fpreg(unsigned size, unsigned code) const {
    switch (size) {
      case kSRegSize: return sreg(code);
      case kDRegSize: return dreg(code);
      default:
        VIXL_UNREACHABLE();
        return 0.0;
    }
  }

  // Write 'value' into a floating-point register. The value is zero-extended.
  // This behaviour matches AArch64 register writes.
  template<typename T>
  inline void set_fpreg(unsigned code, T value) {
    VIXL_ASSERT((sizeof(value) == kDRegSizeInBytes) ||
           (sizeof(value) == kSRegSizeInBytes));
    VIXL_ASSERT(code < kNumberOfFPRegisters);
    fpregisters_[code].Set(value, sizeof(value));
  }

  // Common specialized accessors for the set_fpreg() template.
  inline void set_sreg(unsigned code, float value) {
    set_fpreg(code, value);
  }

  inline void set_sreg_bits(unsigned code, uint32_t value) {
    set_fpreg(code, value);
  }

  inline void set_dreg(unsigned code, double value) {
    set_fpreg(code, value);
  }

  inline void set_dreg_bits(unsigned code, uint64_t value) {
    set_fpreg(code, value);
  }

  bool N() { return nzcv_.N() != 0; }
  bool Z() { return nzcv_.Z() != 0; }
  bool C() { return nzcv_.C() != 0; }
  bool V() { return nzcv_.V() != 0; }
  SimSystemRegister& nzcv() { return nzcv_; }

  // TODO(jbramley): Find a way to make the fpcr_ members return the proper
  // types, so these accessors are not necessary.
  FPRounding RMode() { return static_cast<FPRounding>(fpcr_.RMode()); }
  bool DN() { return fpcr_.DN() != 0; }
  SimSystemRegister& fpcr() { return fpcr_; }

  // Debug helpers
  void PrintSystemRegisters(bool print_all = false);
  void PrintRegisters(bool print_all_regs = false);
  void PrintFPRegisters(bool print_all_regs = false);
  void PrintProcessorState();

  static const char* WRegNameForCode(unsigned code,
                                     Reg31Mode mode = Reg31IsZeroRegister);
  static const char* XRegNameForCode(unsigned code,
                                     Reg31Mode mode = Reg31IsZeroRegister);
  static const char* SRegNameForCode(unsigned code);
  static const char* DRegNameForCode(unsigned code);
  static const char* VRegNameForCode(unsigned code);

  inline bool coloured_trace() { return coloured_trace_; }
  void set_coloured_trace(bool value);

  inline bool disasm_trace() { return disasm_trace_; }
  inline void set_disasm_trace(bool value) {
    if (value != disasm_trace_) {
      if (value) {
        decoder_->InsertVisitorBefore(print_disasm_, this);
      } else {
        decoder_->RemoveVisitor(print_disasm_);
      }
      disasm_trace_ = value;
    }
  }
  inline void set_instruction_stats(bool value) {
    if (value != instruction_stats_) {
      if (value) {
        decoder_->AppendVisitor(instrumentation_);
      } else {
        decoder_->RemoveVisitor(instrumentation_);
      }
      instruction_stats_ = value;
    }
  }

 protected:
  const char* clr_normal;
  const char* clr_flag_name;
  const char* clr_flag_value;
  const char* clr_reg_name;
  const char* clr_reg_value;
  const char* clr_fpreg_name;
  const char* clr_fpreg_value;
  const char* clr_memory_value;
  const char* clr_memory_address;
  const char* clr_debug_number;
  const char* clr_debug_message;
  const char* clr_printf;

  // Simulation helpers ------------------------------------
  bool ConditionPassed(Condition cond) {
    switch (cond) {
      case eq:
        return Z();
      case ne:
        return !Z();
      case hs:
        return C();
      case lo:
        return !C();
      case mi:
        return N();
      case pl:
        return !N();
      case vs:
        return V();
      case vc:
        return !V();
      case hi:
        return C() && !Z();
      case ls:
        return !(C() && !Z());
      case ge:
        return N() == V();
      case lt:
        return N() != V();
      case gt:
        return !Z() && (N() == V());
      case le:
        return !(!Z() && (N() == V()));
      case nv:  // Fall through.
      case al:
        return true;
      default:
        VIXL_UNREACHABLE();
        return false;
    }
  }

  bool ConditionFailed(Condition cond) {
    return !ConditionPassed(cond);
  }

  void AddSubHelper(Instruction* instr, int64_t op2);
  int64_t AddWithCarry(unsigned reg_size,
                       bool set_flags,
                       int64_t src1,
                       int64_t src2,
                       int64_t carry_in = 0);
  void LogicalHelper(Instruction* instr, int64_t op2);
  void ConditionalCompareHelper(Instruction* instr, int64_t op2);
  void LoadStoreHelper(Instruction* instr,
                       int64_t offset,
                       AddrMode addrmode);
  void LoadStorePairHelper(Instruction* instr, AddrMode addrmode);
  uint8_t* AddressModeHelper(unsigned addr_reg,
                             int64_t offset,
                             AddrMode addrmode);

  uint64_t MemoryRead(const uint8_t* address, unsigned num_bytes);
  uint8_t MemoryRead8(uint8_t* address);
  uint16_t MemoryRead16(uint8_t* address);
  uint32_t MemoryRead32(uint8_t* address);
  float MemoryReadFP32(uint8_t* address);
  uint64_t MemoryRead64(uint8_t* address);
  double MemoryReadFP64(uint8_t* address);

  void MemoryWrite(uint8_t* address, uint64_t value, unsigned num_bytes);
  void MemoryWrite32(uint8_t* address, uint32_t value);
  void MemoryWriteFP32(uint8_t* address, float value);
  void MemoryWrite64(uint8_t* address, uint64_t value);
  void MemoryWriteFP64(uint8_t* address, double value);

  int64_t ShiftOperand(unsigned reg_size,
                       int64_t value,
                       Shift shift_type,
                       unsigned amount);
  int64_t Rotate(unsigned reg_width,
                 int64_t value,
                 Shift shift_type,
                 unsigned amount);
  int64_t ExtendValue(unsigned reg_width,
                      int64_t value,
                      Extend extend_type,
                      unsigned left_shift = 0);

  uint64_t ReverseBits(uint64_t value, unsigned num_bits);
  uint64_t ReverseBytes(uint64_t value, ReverseByteMode mode);

  template <typename T>
  T FPDefaultNaN() const;

  void FPCompare(double val0, double val1);
  double FPRoundInt(double value, FPRounding round_mode);
  double FPToDouble(float value);
  float FPToFloat(double value, FPRounding round_mode);
  double FixedToDouble(int64_t src, int fbits, FPRounding round_mode);
  double UFixedToDouble(uint64_t src, int fbits, FPRounding round_mode);
  float FixedToFloat(int64_t src, int fbits, FPRounding round_mode);
  float UFixedToFloat(uint64_t src, int fbits, FPRounding round_mode);
  int32_t FPToInt32(double value, FPRounding rmode);
  int64_t FPToInt64(double value, FPRounding rmode);
  uint32_t FPToUInt32(double value, FPRounding rmode);
  uint64_t FPToUInt64(double value, FPRounding rmode);

  template <typename T>
  T FPAdd(T op1, T op2);

  template <typename T>
  T FPDiv(T op1, T op2);

  template <typename T>
  T FPMax(T a, T b);

  template <typename T>
  T FPMaxNM(T a, T b);

  template <typename T>
  T FPMin(T a, T b);

  template <typename T>
  T FPMinNM(T a, T b);

  template <typename T>
  T FPMul(T op1, T op2);

  template <typename T>
  T FPMulAdd(T a, T op1, T op2);

  template <typename T>
  T FPSqrt(T op);

  template <typename T>
  T FPSub(T op1, T op2);

  // This doesn't do anything at the moment. We'll need it if we want support
  // for cumulative exception bits or floating-point exceptions.
  void FPProcessException() { }

  // Standard NaN processing.
  template <typename T>
  T FPProcessNaN(T op);

  bool FPProcessNaNs(Instruction* instr);

  template <typename T>
  T FPProcessNaNs(T op1, T op2);

  template <typename T>
  T FPProcessNaNs3(T op1, T op2, T op3);

  // Pseudo Printf instruction
  void DoPrintf(Instruction* instr);

  // Processor state ---------------------------------------

  // Output stream.
  FILE* stream_;
  PrintDisassembler* print_disasm_;

  // Instruction statistics instrumentation.
  Instrument* instrumentation_;

  // General purpose registers. Register 31 is the stack pointer.
  SimRegister registers_[kNumberOfRegisters];

  // Floating point registers
  SimFPRegister fpregisters_[kNumberOfFPRegisters];

  // Program Status Register.
  // bits[31, 27]: Condition flags N, Z, C, and V.
  //               (Negative, Zero, Carry, Overflow)
  SimSystemRegister nzcv_;

  // Floating-Point Control Register
  SimSystemRegister fpcr_;

  // Only a subset of FPCR features are supported by the simulator. This helper
  // checks that the FPCR settings are supported.
  //
  // This is checked when floating-point instructions are executed, not when
  // FPCR is set. This allows generated code to modify FPCR for external
  // functions, or to save and restore it when entering and leaving generated
  // code.
  void AssertSupportedFPCR() {
    VIXL_ASSERT(fpcr().FZ() == 0);             // No flush-to-zero support.
    VIXL_ASSERT(fpcr().RMode() == FPTieEven);  // Ties-to-even rounding only.

    // The simulator does not support half-precision operations so fpcr().AHP()
    // is irrelevant, and is not checked here.
  }

  static inline int CalcNFlag(uint64_t result, unsigned reg_size) {
    return (result >> (reg_size - 1)) & 1;
  }

  static inline int CalcZFlag(uint64_t result) {
    return result == 0;
  }

  static const uint32_t kConditionFlagsMask = 0xf0000000;

  // Stack
  byte* stack_;
  static const int stack_protection_size_ = 256;
  // 2 KB stack.
  static const int stack_size_ = 2 * 1024 + 2 * stack_protection_size_;
  byte* stack_limit_;

  Decoder* decoder_;
  // Indicates if the pc has been modified by the instruction and should not be
  // automatically incremented.
  bool pc_modified_;
  Instruction* pc_;

  static const char* xreg_names[];
  static const char* wreg_names[];
  static const char* sreg_names[];
  static const char* dreg_names[];
  static const char* vreg_names[];

  static const Instruction* kEndOfSimAddress;

 private:
  bool coloured_trace_;

  // Indicates whether the disassembly trace is active.
  bool disasm_trace_;

  // Indicates whether the instruction instrumentation is active.
  bool instruction_stats_;
};
}  // namespace vixl

#endif  // VIXL_A64_SIMULATOR_A64_H_
