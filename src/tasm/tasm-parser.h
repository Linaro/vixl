// Copyright 2020, VIXL authors
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

#ifndef VIXL_TASM_PARSER_H
#define VIXL_TASM_PARSER_H

#if __cplusplus < 201703L
#error "Text Assembler requires C++17 standard or above."
#else

#include <string>
#include <variant>  // NOLINT(build/include_order)
#include <vector>

#include "aarch64/assembler-aarch64.h"

namespace vixl {
namespace aarch64 {
namespace tasm {

using Argument = std::variant<std::monostate,
                              Register,
                              PRegister,
                              ZRegister,
                              VRegister,
                              PRegisterWithLaneSize,
                              PRegisterM,
                              PRegisterZ,
                              Operand,
                              MemOperand,
                              SVEMemOperand,
                              int64_t,
                              uint64_t,
                              int>;

class InstructionParser {
  using ParserFn = int (InstructionParser::*)(std::string,
                                              std::string*,
                                              Argument*);
  using MnemonicFn = bool (*)(std::string, std::string);
  using PrototypesFn = std::vector<std::string> (*)();

 private:
  // Variable with defined mappings between regexes and parser functions
  static const std::vector<std::pair<ParserFn, std::string> > regex_parser;

  // Variables responsible for holding splited elements of currently parsed
  // instruction line.
  std::vector<std::string> str_args;
  std::vector<Argument> args;
  std::string mnemonic;

  // Function pointers to handlers used to mnemonic/prototype lookup in
  // TextAssembler class.
  MnemonicFn mnemonic_exists;
  PrototypesFn get_prototypes;

 public:
  InstructionParser(MnemonicFn mf, PrototypesFn sf);
  void LoadInstruction(std::string inst_line);

  void GetPrototype(std::string* prototype);
  std::string GetMnemonic();
  std::vector<Argument> GetArgs();

  // Functions responsible for parsing arguments of a given type.
  int ParseRegister(std::string argument,
                    std::string* prototype,
                    Argument* arg_object);
  int ParseSVEConstraint(std::string argument,
                         std::string* prototype,
                         Argument* arg_object);
  int ParseZRegister(std::string argument,
                     std::string* prototype,
                     Argument* arg_object);
  int ParseVRegister(std::string argument,
                     std::string* prototype,
                     Argument* arg_object);
  int ParsePRegister(std::string argument,
                     std::string* prototype,
                     Argument* arg_object);
  int ParseExtend(std::string argument,
                  std::string* prototype,
                  Argument* arg_object);
  int ParseShift(std::string argument,
                 std::string* prototype,
                 Argument* arg_object);
  int ParseImmediate(std::string argument,
                     std::string* prototype,
                     Argument* arg_object);

  // Helper functions for arguments parsing.
  void ParseImmediatesTypes(std::string* prototype);
  void ParseMemOperand(std::string* prototype,
                       size_t pos_start,
                       size_t pos_end);
  void SplitShiftExtend(std::string* argument, int* imm);
};
}
}
}  // namespace vixl::aarch64::tasm

#endif  // __cplusplus >= 201703L
#endif  // VIXL_TASM_PARSER_H