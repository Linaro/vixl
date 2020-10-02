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

#if __cplusplus < 201703L
#error "Text Assembler requires C++17 standard or above."
#else

#include <cctype>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <variant>  // NOLINT(build/include_order)
#include <vector>

#include "tasm-parser.h"

namespace vixl {
namespace aarch64 {
namespace tasm {

// TODO: Make mnemonics and arguments case insensitive.
const std::vector<std::pair<InstructionParser::ParserFn, std::string> >
    InstructionParser::regex_parser =
        {{&InstructionParser::ParseVRegister, "(b|h|s|d)(\\d{1,2})"},
         {&InstructionParser::ParseShift, "(lsl|lsr|asr|ror)#(\\d+)"},
         {&InstructionParser::ParseExtend,
          "(sxtb|sxth|sxtw|sxtx|uxtb|uxth|uxtw|uxtx)(#\\d*)?"},
         {&InstructionParser::ParseImmediate, "#(\\d+|0x[\\da-fA-F]+)"},
         {&InstructionParser::ParseRegister, "(lr|sp|wsp|xzr|wzr)"},
         {&InstructionParser::ParseRegister, "(w|x)(\\d{1,2})"},
         {&InstructionParser::ParseZRegister, "z(\\d{1,2})\\.(b|h|s|d)"},
         {&InstructionParser::ParseVRegister,
          "v(\\d{1,2})\\.(16b|8b|8h|4h|4s|2s)"},
         {&InstructionParser::ParsePRegister, "p(\\d{1,2})"},
         {&InstructionParser::ParsePRegister, "p(\\d{1,2})\\.(b|h|s|d|q)"},
         {&InstructionParser::ParsePRegister, "p(\\d{1,2})/(m|z)"},
         {&InstructionParser::ParseSVEConstraint, "(pow2|vl\\d|mul\\d|all)"}};

// Following const variables could be also static/member variables.
const std::map<std::string, Shift> shift_map = {{"lsl", LSL},
                                                {"ror", ROR},
                                                {"lsr", LSR},
                                                {"asr", ASR},
                                                {"msl", MSL}};

const std::map<std::string, Extend> extend_map = {{"uxtw", UXTW},
                                                  {"sxtw", SXTW},
                                                  {"uxtb", UXTB},
                                                  {"sxtb", SXTB},
                                                  {"uxth", UXTH},
                                                  {"sxth", SXTH},
                                                  {"uxtx", UXTX},
                                                  {"sxtx", SXTX}};

const std::map<std::string, Register> register_map = {{"wzr", wzr},
                                                      {"xzr", xzr},
                                                      {"wsp", wsp},
                                                      {"sp", sp},
                                                      {"lr", lr}};

const std::map<std::string, SVEPredicateConstraint> constraint_map =
    {{"pow2", SVE_POW2},
     {"vl1", SVE_VL1},
     {"vl2", SVE_VL2},
     {"vl3", SVE_VL3},
     {"vl4", SVE_VL4},
     {"vl5", SVE_VL5},
     {"vl6", SVE_VL6},
     {"vl7", SVE_VL7},
     {"vl8", SVE_VL8},
     {"vl16", SVE_VL16},
     {"vl32", SVE_VL32},
     {"vl64", SVE_VL64},
     {"vl128", SVE_VL128},
     {"vl256", SVE_VL256},
     {"mul3", SVE_MUL3},
     {"mul4", SVE_MUL4},
     {"all", SVE_ALL}};

const std::map<char, unsigned int> sve_sizes_map = {
    {'b', kBRegSize},
    {'h', kHRegSize},
    {'s', kSRegSize},
    {'d', kDRegSize},
    {'q', kQRegSize},
};

// Initialization of handlers used for searching instructions
// and its prototypes defined in text assembler.
InstructionParser::InstructionParser(MnemonicFn mf_p, PrototypesFn sf_p) {
  mnemonic_exists = mf_p;
  get_prototypes = sf_p;
}

// Load instruction and split it into mnemonic and vector of consecutive string
// arguments.
void InstructionParser::LoadInstruction(std::string inst_line) {
  std::string inst_regex = "\\s*([a-zA-Z0-9.]+)(?:\\s+(.+))?";
  std::string args_regex = "([^,]+)";
  std::smatch mne_args;

  mnemonic.clear();
  str_args.clear();
  args.clear();

  std::transform(inst_line.begin(),
                 inst_line.end(),
                 inst_line.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (std::regex_match(inst_line, mne_args, std::regex(inst_regex))) {
    mnemonic = mne_args.str(1);
    std::string arguments =
        std::regex_replace(mne_args.str(2), std::regex(" "), "");

    while (std::regex_search(arguments, mne_args, std::regex(args_regex))) {
      str_args.push_back(mne_args.str(1));
      arguments = mne_args.suffix();
    }
  }
}

// Creates instruction prototype from vector of string arguments.
// Then fills vector of arguments with objects of coresponding type.
void InstructionParser::GetPrototype(std::string* prototype) {
  std::vector<std::pair<size_t, size_t> > mem_operands;

  // Iterating through vector of arguments and looking for a matching
  // regex(from regex_parser map) to recognize the argument type.
  for (auto& argument : str_args) {
    auto i = regex_parser.begin();
    Argument arg_object;

    if (argument[0] == '[') {
      argument.erase(0, 1);
      mem_operands.push_back({prototype->size(), -1});
    }

    if (argument.back() == ']') {
      argument.pop_back();
      mem_operands.back().second = prototype->size();
    }

    while (i != regex_parser.end()) {
      auto parse_func = i->first;
      // Parse string argument to the object of the given type in the parser
      // function if regex matches this type.
      if (std::regex_match(argument, std::regex(i->second))) {
        // TODO: return error code from the parser functions and
        // stop parsing arguments further.
        (this->*parse_func)(argument, prototype, &arg_object);
        break;
      }
      i++;
    }
    args.push_back(arg_object);
  }
  for (const auto& pair : mem_operands)
    ParseMemOperand(prototype, pair.first, pair.second);

  ParseImmediatesTypes(prototype);
}

int InstructionParser::ParseShift(std::string argument,
                                  std::string* prototype,
                                  Argument* arg_object) {
  if (prototype->back() == 'r') {
    int imm = 0;
    Register r = std::get<Register>(args.back());

    args.pop_back();
    SplitShiftExtend(&argument, &imm);

    Shift s = shift_map.at(argument);
    *arg_object = Operand(r, s, imm);
    prototype->back() = 'o';
  }
  return 0;
}

int InstructionParser::ParseExtend(std::string argument,
                                   std::string* prototype,
                                   Argument* arg_object) {
  if (prototype->back() == 'r') {
    int imm = 0;
    Register r = std::get<Register>(args.back());

    args.pop_back();
    SplitShiftExtend(&argument, &imm);

    Extend e = extend_map.at(argument);
    *arg_object = Operand(r, e, imm);
    prototype->back() = 'o';
  }
  return 0;
}

// TODO: Additional validity checks of registers.
int InstructionParser::ParseRegister(std::string argument,
                                     std::string* prototype,
                                     Argument* arg_object) {
  if (register_map.count(argument)) {
    *arg_object = register_map.at(argument);
  } else if (argument.length() >= 2) {
    int reg_num;
    char reg_type;

    reg_type = argument.at(0);
    argument = argument.erase(0, 1);
    reg_num = std::stoi(argument);

    VIXL_ASSERT((reg_type == 'w' || reg_type == 'x'));
    *arg_object = Register(reg_num, reg_type == 'w' ? kWRegSize : kXRegSize);
  } else {
    return -1;
  }

  // Register on a last place can be treated as an Operand object sometimes.
  // We need to handle such cases and convert argument respectively.
  std::string op_prototype = *prototype + 'o';
  prototype->push_back('r');

  if (str_args.size() == prototype->length() &&
      mnemonic_exists(op_prototype, mnemonic)) {
    *arg_object = Operand(std::get<Register>(*arg_object));
    prototype->back() = 'o';
  }

  return 0;
}

// TODO: Implement proper parser functions.
// (void)param just to avoid compiler error about unused variables temporarily.
int InstructionParser::ParseVRegister(std::string argument,
                                      std::string* prototype,
                                      Argument* arg_object) {
  (void)argument;
  (void)arg_object;
  (void)prototype;

  return 0;
}

int InstructionParser::ParseZRegister(std::string argument,
                                      std::string* prototype,
                                      Argument* arg_object) {
  int num_end = std::isdigit(argument.at(2)) ? 2 : 1;
  int reg_num = std::stoi(argument.substr(1, num_end));

  *arg_object = ZRegister(reg_num, sve_sizes_map.at(argument.at(num_end + 2)));
  prototype->push_back('z');

  return 0;
}

int InstructionParser::ParsePRegister(std::string argument,
                                      std::string* prototype,
                                      Argument* arg_object) {
  int num_end = (argument.size() > 2 && std::isdigit(argument.at(2))) ? 2 : 1;
  int reg_num = std::stoi(argument.substr(1, num_end));

  if (argument.length() <= 3) {
    *arg_object = PRegister(reg_num);
    prototype->push_back('p');
  } else if (argument.at(num_end + 1) == '.') {
    *arg_object =
        PRegisterWithLaneSize(reg_num,
                              sve_sizes_map.at(argument.at(num_end + 2)));
    prototype->push_back('L');
  } else if (argument.at(num_end + 1) == '/') {
    if (argument.at(num_end + 2) == 'm') {
      *arg_object = PRegisterM(reg_num);
      prototype->push_back('M');
    } else if (argument.at(num_end + 2) == 'z') {
      *arg_object = PRegisterZ(reg_num);
      prototype->push_back('Z');
    }
  } else {
    return -1;
  }
  return 0;
}

int InstructionParser::ParseSVEConstraint(std::string argument,
                                          std::string* prototype,
                                          Argument* arg_object) {
  auto it = constraint_map.find(argument);

  if (it != constraint_map.end()) {
    *arg_object = it->second;
    prototype->push_back('c');
  } else {
    return -1;
  }
  return 0;
}

// Immediates are parsed as unsigned integers and marked as "."
// in the prototype string. This dots are changed to consecutive types
// at the end of instruction parsing (in ParseImmediatesTypes function).
int InstructionParser::ParseImmediate(std::string argument,
                                      std::string* prototype,
                                      Argument* arg_object) {
  argument = argument.substr(1, argument.size() - 1);

  try {
    *arg_object = std::stoul(argument, NULL, 0);
  } catch (std::out_of_range) {
    std::cout << "immediate out of unsigned integer type range" << std::endl;
  }
  prototype->push_back('.');
  return 0;
}

// Combine parsed arguments from given range (specified by pos_start and
// pos_end) into SVEMemOperand or MemOperand.
void InstructionParser::ParseMemOperand(std::string* prototype,
                                        size_t pos_start,
                                        size_t pos_end) {
  Argument arg_object;
  std::smatch mem_op_match;
  std::string mem_op_regex = prototype->substr(0, pos_start) + "(s|m).*";

  std::vector<std::string> prototypes = get_prototypes();
  auto it = prototypes.begin();
  bool found = false;

  // Looking for prototype matching current prototype substring up to pos_start
  // with letter "s"(SVEMemOperand) or "m"(MemOperand) on the following place.
  while (it != prototypes.end() && !found) {
    if (std::regex_match(*it, mem_op_match, std::regex(mem_op_regex)) &&
        mnemonic_exists(*it, mnemonic))
      found = true;
    it++;
  }

  std::string mem_op_type = prototype->substr(pos_start, pos_end);

  // Is there some way to write switch/case with strings ?
  if (!mem_op_match.str(1).compare("s")) {
    // TODO: Handle all the cases of SVEMemOperand
    if (!mem_op_type.compare("r.")) {
      arg_object = SVEMemOperand(std::get<Register>(args[pos_start]),
                                 std::get<uint64_t>(args[pos_start + 1]));
    } else if (!mem_op_type.compare("z.")) {
      arg_object = SVEMemOperand(std::get<ZRegister>(args[pos_start]),
                                 std::get<uint64_t>(args[pos_start + 1]));
    } else if (!mem_op_type.compare("ro")) {
      Operand op = std::get<Operand>(args[pos_start + 1]);
      arg_object = SVEMemOperand(std::get<Register>(args[pos_start]),
                                 op.GetBaseRegister(),
                                 op.GetShift(),
                                 op.GetShiftAmount());
    } else if (!mem_op_type.compare("rr")) {
      arg_object = SVEMemOperand(std::get<Register>(args[pos_start]),
                                 std::get<Register>(args[pos_start + 1]));
    } else if (!mem_op_type.compare("rz")) {
      arg_object = SVEMemOperand(std::get<Register>(args[pos_start]),
                                 std::get<ZRegister>(args[pos_start + 1]));
    } else if (!mem_op_type.compare("r")) {
      arg_object = SVEMemOperand(std::get<Register>(args[pos_start]));
    } else if (!mem_op_type.compare("z")) {
      arg_object = SVEMemOperand(std::get<ZRegister>(args[pos_start]));
    }
  } else if (!mem_op_match.str(1).compare("m")) {
    // TODO: Implement logic handling MemOperand type
  }

  // If arg_object(of std::variant type) holds std::monostate(index 0),
  // then parsing was unsucessful
  if (arg_object.index() != 0) {
    args.erase(args.begin() + pos_start, args.begin() + pos_end + 1);
    args.insert(args.begin() + pos_start, arg_object);
    *prototype = prototype->substr(0, pos_start) + mem_op_match.str(1);
  }
}

// Parse immediates by matching currently built prototype string with candidate
// prototypes of a given mnemonic, then convert string arguments to given types
// consecutively.
void InstructionParser::ParseImmediatesTypes(std::string* prototype) {
  std::string matching_prototype;
  size_t imm_pos = prototype->find('.');

  std::vector<std::string> prototypes = get_prototypes();
  auto it = prototypes.begin();

  if (imm_pos == std::string::npos) return;

  // Looking for prototype matching parsed instruction and currently
  // built prototype without immediates.
  while (it != prototypes.end() && matching_prototype.empty()) {
    // Two prototype strings are compared as regexes. Dots in current
    // prototype string are matching any character on immediate place.
    // For example: "rr." and "rro".
    if (std::regex_match(*it, std::regex(*prototype)) &&
        mnemonic_exists(*it, mnemonic))
      matching_prototype = *it;
    it++;
  }

  if (matching_prototype.empty()) return;

  // Compare current prototype without immediates with full prototype found in a
  // map for given mnemonic and parse arguments to immediate objects
  // respectively.
  while (imm_pos != std::string::npos && imm_pos < args.size()) {
    uint64_t u_imm = std::get<uint64_t>(args[imm_pos]);
    str_args[imm_pos].erase(0, 1);

    switch (matching_prototype.at(imm_pos)) {
      case 'o':
        args[imm_pos] = Operand(u_imm);
        (*prototype)[imm_pos] = 'o';
        break;
      case 'i':
        args[imm_pos] = static_cast<int64_t>(u_imm);
        (*prototype)[imm_pos] = 'i';
        break;
      case 'u':
        args[imm_pos] = u_imm;
        (*prototype)[imm_pos] = 'u';
        break;
      default:
        std::cout << "unrecognized immediate type" << std::endl;
    }
    imm_pos = prototype->find('.', imm_pos + 1);
  }
}

// Split shift/extend into shift type and immediate, for example:
//  * lsl #3 -> lsl, 3
void InstructionParser::SplitShiftExtend(std::string* argument, int* imm) {
  std::string shift_regex = "(.*)(?:\\s*#)(\\d+)";
  std::smatch match;
  std::string imm_str;

  if (std::regex_match(*argument, match, std::regex(shift_regex))) {
    *argument = match.str(1);
    *imm = std::stoi(match.str(2));
  }
}

std::string InstructionParser::GetMnemonic() { return mnemonic; }

std::vector<Argument> InstructionParser::GetArgs() { return args; }
}
}
}  // namespace vixl::aarch64::tasm

#endif  // __cplusplus >= 201703L