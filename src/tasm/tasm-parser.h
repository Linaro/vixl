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

#include <regex>
#include <string>
#include <variant>  // NOLINT(build/include_order)
#include <vector>

#include "aarch64/assembler-aarch64.h"

namespace vixl {
namespace aarch64 {
namespace tasm {

using Argument = std::variant<std::monostate,
                              Condition,
                              Label *,
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
                              IntegerOperand,
                              int64_t,
                              uint64_t,
                              unsigned,
                              int>;

// Structure holding information about encountered memory operand in the
// instruction line. beg_pos and end_pos are storing position of a first and
// last argument of the operand in the prototype string.
struct MemDescriptor {
  size_t beg_pos;
  size_t end_pos;
  AddrMode addr_mode;
};

class InstructionParser {
  using ParserFn = int (InstructionParser::*)(std::smatch, std::string *);
  using MnemonicFn = bool (*)(std::string, std::string);
  using PrototypesFn = std::vector<std::string> (*)();

 private:
  // Variable with defined mappings between regexes and parser functions
  static const std::vector<std::pair<ParserFn, std::string> > regex_parser;

  // Variables responsible for holding splited elements of currently parsed
  // instruction line.
  std::map<std::string, Label> labels;
  std::vector<Argument> args;
  std::vector<MemDescriptor> mem_args;
  std::string mnemonic;

  // Function pointers to handlers used to mnemonic/prototype lookup in
  // TextAssembler class.
  MnemonicFn mnemonic_exists;
  PrototypesFn get_prototypes;

 public:
  InstructionParser(MnemonicFn, PrototypesFn);
  ~InstructionParser();
  bool LoadInstruction(std::string inst_line, std::string *prototype);

  void ParseArgumentsLine(std::string *prototype, std::string arguments);
  std::string GetMnemonic();
  std::vector<Argument> GetArgs();

  // Functions responsible for parsing arguments of a given type.
  int ParseRegister(std::smatch arg_match, std::string *prototype);
  int ParseSVEConstraint(std::smatch arg_match, std::string *prototype);
  int ParseZRegister(std::smatch arg_match, std::string *prototype);
  int ParseVRegister(std::smatch arg_match, std::string *prototype);
  int ParsePRegister(std::smatch arg_match, std::string *prototype);
  int ParseExtend(std::smatch arg_match, std::string *prototype);
  int ParseShift(std::smatch arg_match, std::string *prototype);
  int ParseImmediate(std::smatch arg_match, std::string *prototype);
  int ParseVRegisterList(std::smatch arg_match, std::string *prototype);
  int ParseMemOperandList(std::smatch arg_match, std::string *prototype);

  // Helper functions for arguments parsing.
  bool MnemonicExists(std::string mnemonic, std::string prototype);
  std::smatch FindImmPrototype(std::string prototype);
  IntegerOperand ExtractInteger(std::string int_str);
  void ParseImmediatesTypes(std::string *prototype);
  void ParseMemOperandTypes(std::string *prototype, MemDescriptor mem_arg);
  void ParseMnemonic(std::string *mnemonic, std::string *prototype);
};
}
}
}  // namespace vixl::aarch64::tasm

#endif  // __cplusplus >= 201703L
#endif  // VIXL_TASM_PARSER_H