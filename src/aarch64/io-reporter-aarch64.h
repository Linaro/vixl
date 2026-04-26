// Copyright 2022, VIXL authors
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
//   * Neither the name of Arm Limited nor the names of its contributors may be
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

#ifndef VIXL_AARCH64_IO_REPORTER_AARCH64_H_
#define VIXL_AARCH64_IO_REPORTER_AARCH64_H_

#include <unordered_map>
#include <unordered_set>

#include "decoder-aarch64.h"
#include "registers-aarch64.h"

namespace vixl {
namespace aarch64 {

class IOReporter : public DecoderVisitor {
 public:
  IOReporter() {}

  virtual void Visit(Metadata* metadata,
                     const Instruction* instr) VIXL_OVERRIDE;

  // Clear the contents of the seen register storage.
  void Reset() {
    source_.clear();
    destination_.clear();
  }

  // Return the set of source and destination registers.
  std::unordered_set<CPURegister> GetSourceRegisters() const { return source_; }
  std::unordered_set<CPURegister> GetDestinationRegisters() const {
    return destination_;
  }

 private:
  // Given a form, look up the source and desination registers it uses.
  using FormToRegisterUseMap = std::unordered_map<uint32_t, std::string>;
  static const FormToRegisterUseMap* GetFormToRegisterUseMap();

  // Return the register this token represents by examining the encoding of the
  // instruction.
  // There are four types of token that represent how to decode the registers
  // used by an instruction:
  //  'R - integer register bank, representing 64-bit registers x0-x30 and sp.
  //  'V - vector register bank, representing registers v0-v31, including
  //       overlapping z registers.
  //  'P - predicate register bank, representing registers p0-p15.
  //  'I - implicit registers, where an instruction encodes no explicit
  //       register, but uses one implicitly, eg. bl or autia1716. These are
  //       assumed to be in the integer register bank.
  // A token is between two (eg. Rd) and five (eg. V1816) characters long.
  CPURegister GetRegisterForToken(const Instruction* instr,
                                  const std::string& t);

  // Add entries to the source and destination register sets using the tokens
  // for this instruction.
  void AddRegistersFromTokens(const Instruction* instr, const std::string& s);

  // Convert the first two digit characters in s into a value between 0 and 31.
  // Commonly used to convert a bit position in a token to a number.
  uint32_t GetFiveBitNumberFromString(const std::string& s);

  // The sets of source and destination registers seen so far.
  std::unordered_set<CPURegister> source_;
  std::unordered_set<CPURegister> destination_;
};

}  // namespace aarch64
}  // namespace vixl

#endif  // VIXL_AARCH64_IO_REPORTER_AARCH64_H_
