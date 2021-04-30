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

#include "tasm-errors.h"
#include "aarch64/assembler-aarch64.h"

namespace vixl {
namespace aarch64 {
namespace tasm {

using Argument = std::variant<std::monostate,
                              Extend,
                              Shift,
                              StatusFlags,
                              SystemRegister,
                              SVEOffsetModifier,
                              PrefetchOperation,
                              LoadStoreScalingOption,
                              BarrierDomain,
                              BranchTargetIdentifier,
                              InstructionCacheOp,
                              BarrierType,
                              DataCacheOp,
                              SystemHint,
                              CPURegister,
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
                              int,
                              Float16,
                              float,
                              double>;

// Structure holding information about encountered memory operand in the
// instruction line. beg_pos and end_pos are storing position of a first and
// last argument of the operand in the prototype string.
struct MemDescriptor {
  size_t beg_pos;
  size_t end_pos{SIZE_MAX};
  AddrMode addr_mode{Offset};
};

// Structure holding information about single parsed argument from the
// instruction line. match and raw_case are storing the result of the regular
// expression match and raw argument string.
struct ArgumentSet {
  std::smatch match;
  std::string raw_case;

  ArgumentSet() = default;
  explicit ArgumentSet(std::smatch m) : match{m}, raw_case{""} {};
};

// Enum with possible types of the single input line.
enum LineType { kInstruction, kLabel, kEmpty, kUnmatched };

class InstructionParser {
  using ParserFn = void (InstructionParser::*)(ArgumentSet, std::string *);
  using MnemonicFn = bool (*)(std::string, std::string);
  using PrototypesFn = std::vector<std::string> (*)();

 private:
  ErrorHandler *eh;
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
  InstructionParser(MnemonicFn mf_p, PrototypesFn sf_p, ErrorHandler *err_h);
  ~InstructionParser();
  LineType LoadInstruction(std::string inst_line, std::string *prototype);

  void ParseArgumentsLine(std::string *prototype, std::string arguments);
  std::string GetMnemonic();
  std::vector<Argument> GetArgs();

  // Functions responsible for parsing arguments of a given type.
  void ParseRegister(ArgumentSet arg_set, std::string *prototype);
  void ParseSVEConstraint(ArgumentSet arg_set, std::string *prototype);
  void ParseCondition(ArgumentSet arg_set, std::string *prototype);
  void ParseZRegister(ArgumentSet arg_set, std::string *prototype);
  void ParseVRegister(ArgumentSet arg_set, std::string *prototype);
  void ParseFPRegister(ArgumentSet arg_set, std::string *prototype);
  void ParsePRegister(ArgumentSet arg_set, std::string *prototype);
  void ParseMultiplier(ArgumentSet arg_set, std::string *prototype);
  void ParseExtend(ArgumentSet arg_set, std::string *prototype);
  void ParseShift(ArgumentSet arg_set, std::string *prototype);
  void ParseStatusFlags(ArgumentSet arg_set, std::string *prototype);
  void ParseImmediate(ArgumentSet arg_set, std::string *prototype);
  void ParseMemOffset(ArgumentSet arg_set, std::string *prototype);
  void ParseFPImmediate(ArgumentSet arg_set, std::string *prototype);
  void ParseRegisterList(ArgumentSet arg_set, std::string *prototype);
  void ParseMemOperandList(ArgumentSet arg_set, std::string *prototype);
  void ParseLabel(ArgumentSet arg_set, std::string *prototype);

  // Helper functions for arguments parsing.
  bool MnemonicExists(std::string mnemonic, std::string prototype);
  std::smatch FindImmPrototype(std::string prototype);
  IntegerOperand ExtractInteger(std::string int_str);
  void ParseImmediatesTypes(std::string *prototype);
  void ParseMemOperandTypes(std::string *prototype, MemDescriptor mem_arg);
  void ParseMnemonic(std::string *mnemonic, std::string *prototype);
  bool CurrArgumentInMemOperand(size_t curr_pos);
};
}
}
}  // namespace vixl::aarch64::tasm

#endif  // __cplusplus >= 201703L
#endif  // VIXL_TASM_PARSER_H