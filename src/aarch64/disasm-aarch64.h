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

#ifndef VIXL_AARCH64_DISASM_AARCH64_H
#define VIXL_AARCH64_DISASM_AARCH64_H

#include <utility>

#include "../globals-vixl.h"
#include "../utils-vixl.h"

#include "cpu-features-auditor-aarch64.h"
#include "decoder-aarch64.h"
#include "instructions-aarch64.h"
#include "isa-aarch64.h"
#include "operands-aarch64.h"

namespace vixl {
namespace aarch64 {

class Disassembler : public DecoderVisitor {
 public:
  Disassembler();
  Disassembler(char* text_buffer, int buffer_size);
  virtual ~Disassembler();
  char* GetOutput();

// Declare all Visitor functions.
#define DECLARE(A) \
  virtual void Visit##A(const Instruction* instr) VIXL_OVERRIDE;
  VISITOR_LIST(DECLARE)
#undef DECLARE

  virtual void VisitData(const Instruction* instr) VIXL_OVERRIDE;

 protected:
  virtual void ProcessOutput(const Instruction* instr);

  // Default output functions. The functions below implement a default way of
  // printing elements in the disassembly. A sub-class can override these to
  // customize the disassembly output.

  // Prints the name of a register.
  // TODO: This currently doesn't allow renaming of V registers.
  virtual void AppendRegisterNameToOutput(const Instruction* instr,
                                          const CPURegister& reg);

  // Prints a PC-relative offset. This is used for example when disassembling
  // branches to immediate offsets.
  virtual void AppendPCRelativeOffsetToOutput(const Instruction* instr,
                                              int64_t offset);

  // Prints an address, in the general case. It can be code or data. This is
  // used for example to print the target address of an ADR instruction.
  virtual void AppendCodeRelativeAddressToOutput(const Instruction* instr,
                                                 const void* addr);

  // Prints the address of some code.
  // This is used for example to print the target address of a branch to an
  // immediate offset.
  // A sub-class can for example override this method to lookup the address and
  // print an appropriate name.
  virtual void AppendCodeRelativeCodeAddressToOutput(const Instruction* instr,
                                                     const void* addr);

  // Prints the address of some data.
  // This is used for example to print the source address of a load literal
  // instruction.
  virtual void AppendCodeRelativeDataAddressToOutput(const Instruction* instr,
                                                     const void* addr);

  // Same as the above, but for addresses that are not relative to the code
  // buffer. They are currently not used by VIXL.
  virtual void AppendAddressToOutput(const Instruction* instr,
                                     const void* addr);
  virtual void AppendCodeAddressToOutput(const Instruction* instr,
                                         const void* addr);
  virtual void AppendDataAddressToOutput(const Instruction* instr,
                                         const void* addr);

 public:
  // Get/Set the offset that should be added to code addresses when printing
  // code-relative addresses in the AppendCodeRelative<Type>AddressToOutput()
  // helpers.
  // Below is an example of how a branch immediate instruction in memory at
  // address 0xb010200 would disassemble with different offsets.
  // Base address | Disassembly
  //          0x0 | 0xb010200:  b #+0xcc  (addr 0xb0102cc)
  //      0x10000 | 0xb000200:  b #+0xcc  (addr 0xb0002cc)
  //    0xb010200 |       0x0:  b #+0xcc  (addr 0xcc)
  void MapCodeAddress(int64_t base_address, const Instruction* instr_address);
  int64_t CodeRelativeAddress(const void* instr);

 private:
  void Format(const Instruction* instr,
              const char* mnemonic,
              const char* format0 = NULL,
              const char* format1 = NULL);
  void Substitute(const Instruction* instr, const char* string);
  int SubstituteField(const Instruction* instr, const char* format);
  int SubstituteRegisterField(const Instruction* instr, const char* format);
  int SubstitutePredicateRegisterField(const Instruction* instr,
                                       const char* format);
  int SubstituteImmediateField(const Instruction* instr, const char* format);
  int SubstituteLiteralField(const Instruction* instr, const char* format);
  int SubstituteBitfieldImmediateField(const Instruction* instr,
                                       const char* format);
  int SubstituteShiftField(const Instruction* instr, const char* format);
  int SubstituteExtendField(const Instruction* instr, const char* format);
  int SubstituteConditionField(const Instruction* instr, const char* format);
  int SubstituteRelAddressField(const Instruction* instr, const char* format);
  int SubstituteBranchTargetField(const Instruction* instr, const char* format);
  int SubstituteLSRegOffsetField(const Instruction* instr, const char* format);
  int SubstitutePrefetchField(const Instruction* instr, const char* format);
  int SubstituteBarrierField(const Instruction* instr, const char* format);
  int SubstituteSysOpField(const Instruction* instr, const char* format);
  int SubstituteCrField(const Instruction* instr, const char* format);
  int SubstituteIntField(const Instruction* instr, const char* format);
  int SubstituteSVESize(const Instruction* instr, const char* format);
  int SubstituteTernary(const Instruction* instr, const char* format);

  std::pair<unsigned, unsigned> GetRegNumForField(const Instruction* instr,
                                                  char reg_prefix,
                                                  const char* field);

  bool RdIsZROrSP(const Instruction* instr) const {
    return (instr->GetRd() == kZeroRegCode);
  }

  bool RnIsZROrSP(const Instruction* instr) const {
    return (instr->GetRn() == kZeroRegCode);
  }

  bool RmIsZROrSP(const Instruction* instr) const {
    return (instr->GetRm() == kZeroRegCode);
  }

  bool RaIsZROrSP(const Instruction* instr) const {
    return (instr->GetRa() == kZeroRegCode);
  }

  bool IsMovzMovnImm(unsigned reg_size, uint64_t value);

  int64_t code_address_offset() const { return code_address_offset_; }

 protected:
  void ResetOutput();
  void AppendToOutput(const char* string, ...) PRINTF_CHECK(2, 3);

  void set_code_address_offset(int64_t code_address_offset) {
    code_address_offset_ = code_address_offset;
  }

  char* buffer_;
  uint32_t buffer_pos_;
  uint32_t buffer_size_;
  bool own_buffer_;

  int64_t code_address_offset_;
};


class PrintDisassembler : public Disassembler {
 public:
  explicit PrintDisassembler(FILE* stream)
      : cpu_features_auditor_(NULL),
        cpu_features_prefix_("// Needs: "),
        cpu_features_suffix_(""),
        default_line_prefix_(""),
        next_disasm_prefix_(NULL),
        signed_addresses_(false),
        stream_(stream),
        last_printed_isa_(ISA::Data) {}

  // Convenience helpers for quick disassembly, without having to manually
  // create a decoder. If a map is provided, offset 0 must correspond with
  // `start`.
  void DisassembleBuffer(const Instruction* start,
                         uint64_t size,
                         const ISAMap* map = nullptr);
  void DisassembleBuffer(const Instruction* start,
                         const Instruction* end,
                         const ISAMap* map = nullptr);
  void Disassemble(const Instruction* instr, ISA isa = ISA::A64);

  // If a CPUFeaturesAuditor is specified, it will be used to annotate
  // disassembly. The CPUFeaturesAuditor is expected to visit the instructions
  // _before_ the disassembler, such that the CPUFeatures information is
  // available when the disassembler is called.
  void RegisterCPUFeaturesAuditor(CPUFeaturesAuditor* auditor) {
    cpu_features_auditor_ = auditor;
  }

  // Set the prefix to appear before the CPU features annotations.
  void SetCPUFeaturesPrefix(const char* prefix) {
    VIXL_ASSERT(prefix != NULL);
    cpu_features_prefix_ = prefix;
  }

  // Set the suffix to appear after the CPU features annotations.
  void SetCPUFeaturesSuffix(const char* suffix) {
    VIXL_ASSERT(suffix != NULL);
    cpu_features_suffix_ = suffix;
  }

  // Set a prefix to appear on each line, unless overridden by
  // SetNextDisassemblyPrefix.
  void SetDefaultLinePrefix(const char* prefix) {
    VIXL_ASSERT(prefix != NULL);
    default_line_prefix_ = prefix;
  }

  // Set a prefix to appear on the next disassembly line (but not ISA
  // annotations). Useful for highlighting specific instruction addresses.
  void SetNextDisassemblyPrefix(const char* prefix) {
    next_disasm_prefix_ = prefix;
  }

  // Print a banner when the ISA is changed.
  virtual void SetISA(ISA isa) VIXL_OVERRIDE;

  // By default, addresses are printed as simple, unsigned 64-bit hex values.
  //
  // With `PrintSignedAddresses(true)`:
  //  - negative addresses are printed as "-0x1234...",
  //  - positive addresses have a leading space, like " 0x1234...", to maintain
  //    alignment.
  //
  // This is most useful in combination with Disassembler::MapCodeAddress(...).
  void PrintSignedAddresses(bool s) { signed_addresses_ = s; }

 protected:
  virtual void ProcessOutput(const Instruction* instr) VIXL_OVERRIDE;

  CPUFeaturesAuditor* cpu_features_auditor_;
  const char* cpu_features_prefix_;
  const char* cpu_features_suffix_;
  const char* default_line_prefix_;
  const char* next_disasm_prefix_;
  bool signed_addresses_;

 private:
  FILE* stream_;

  // Sometimes we SetISA(...) with different ISAs before actually decoding
  // something. Rather than print the ISA on SetISA, we do it lazily, when
  // last_printed_isa_ doesn't match the current ISA.
  ISA last_printed_isa_;
};
}  // namespace aarch64
}  // namespace vixl

#endif  // VIXL_AARCH64_DISASM_AARCH64_H
