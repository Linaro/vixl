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
#include <map>
#include <regex>
#include <string>
#include <variant>  // NOLINT(build/include_order)
#include <vector>

#include "tasm-parser.h"

namespace vixl {
namespace aarch64 {
namespace tasm {

const std::vector<std::pair<InstructionParser::ParserFn, std::string> >
    InstructionParser::regex_parser =
        {{&InstructionParser::ParseFPRegister, "(b|h|s|d|q)(\\d{1,2})"},
         {&InstructionParser::ParseShift, "(lsl|lsr|asr|ror|msl)\\s*#(\\d+)"},
         {&InstructionParser::ParseExtend,
          "(sxtb|sxth|sxtw|sxtx|uxtb|uxth|uxtw|uxtx)\\s*(#\\d*)?"},
         {&InstructionParser::ParseImmediate, "#(-?\\d+|-?0x[\\da-fA-F]+)"},
         {&InstructionParser::ParseMemOffset,
          "(?:pc|#)\\+?([-]?\\d+|[-]?0x[\\da-f]+)"},
         {&InstructionParser::ParseFPImmediate, "#(-?\\d+\\.\\d+)"},
         {&InstructionParser::ParseRegister, "(lr|sp|wsp|xzr|wzr)"},
         {&InstructionParser::ParseRegister, "(w|x)(\\d{1,2})"},
         {&InstructionParser::ParseZRegister,
          "z(\\d{1,2})\\.(b|h|s|d)(?:\\[(\\d{1,2})\\])?"},
         {&InstructionParser::ParsePRegister, "p(\\d{1,2})"},
         {&InstructionParser::ParsePRegister, "p(\\d{1,2})\\.(b|h|s|d|q)"},
         {&InstructionParser::ParsePRegister, "p(\\d{1,2})/(m|z)"},
         {&InstructionParser::ParseMultiplier, "mul\\s+(vl|#\\d+)"},
         {&InstructionParser::ParseStatusFlags, "#nzcv"},
         {&InstructionParser::ParseSVEConstraint,
          "(pow2|vl\\d{1,3}|mul\\d|all)"},
         {&InstructionParser::ParseCondition,
          "(eq|ne|cs|cc|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|al|nv|hs|lo|uo)"},
         {&InstructionParser::ParseVRegister,
          "v(\\d{1,2})\\.(\\d{1,2})?(\\w)(?:\\[(\\d{1,2})\\])?"},
         {&InstructionParser::ParseRegisterList,
          "\\{(.*)\\}(?:\\[(\\d{1,2})\\])?"},
         {&InstructionParser::ParseMemOperandList, "\\[(.*)\\](!?)"},
         {&InstructionParser::ParseLabel, "([a-zA-Z0-9_]+)"}};

// Global maps that are used in a few parsing functions.
const std::map<char, unsigned int> sve_sizes_map = {
    {'b', kBRegSize},
    {'h', kHRegSize},
    {'s', kSRegSize},
    {'d', kDRegSize},
    {'q', kQRegSize},
};

const std::map<std::string, Condition> condition_map = {
    {"eq", eq}, {"ne", ne}, {"cs", cs}, {"cc", cc}, {"mi", mi},
    {"pl", pl}, {"vs", vs}, {"vc", vc}, {"hi", hi}, {"ls", ls},
    {"ge", ge}, {"lt", lt}, {"gt", gt}, {"le", le}, {"al", al},
    {"nv", nv}, {"hs", hs}, {"lo", lo}, {"uo", uo},
};

// Initialization of handlers used for searching instructions
// and its prototypes defined in text assembler.
InstructionParser::InstructionParser(MnemonicFn mf_p,
                                     PrototypesFn sf_p,
                                     ErrorHandler *err_h) {
  mnemonic_exists = mf_p;
  get_prototypes = sf_p;
  eh = err_h;
}

InstructionParser::~InstructionParser() {}

// Load instruction and split it into mnemonic and string of arguments
LineType InstructionParser::LoadInstruction(std::string inst_line,
                                            std::string *prototype) {
  std::string empty_regex = "\\s*";
  std::string comment_regex = "(.*)//.*";
  std::string inst_regex = "\\s*([a-zA-Z0-9.]+)(?:\\s+(.+))?\\s*";
  std::string label_regex = "\\s*([a-zA-Z0-9.]+):\\s*";
  std::smatch match_args;

  eh->IncrementLineNumber(inst_line);
  mem_args.clear();
  args.clear();

  // Strip comments from current line.
  if (std::regex_match(inst_line, match_args, std::regex(comment_regex))) {
    inst_line = match_args.str(1);
  }

  if (std::regex_match(inst_line, match_args, std::regex(inst_regex))) {
    // Parse current line as an instruction.
    mnemonic = match_args.str(1);

    // Make mnemonic case insensitive by converting all letters to
    // lower case.
    std::transform(mnemonic.begin(),
                   mnemonic.end(),
                   mnemonic.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    std::string str_args = match_args.str(2);
    ParseArgumentsLine(prototype, str_args);

    for (const auto &mem_arg : mem_args) {
      ParseMemOperandTypes(prototype, mem_arg);
    }
    ParseMnemonic(&mnemonic, prototype);
    ParseImmediatesTypes(prototype);

    return kInstruction;
  } else if (std::regex_match(inst_line, match_args, std::regex(label_regex))) {
    // Parse current line as a label.
    auto it = labels.try_emplace(match_args.str(1), Label());
    Label *label = &(it.first->second);

    if (label->IsBound()) {
      eh->Err("Label redeclaration.");
    } else {
      args.push_back(label);
    }
    return kLabel;
  } else if (std::regex_match(inst_line, match_args, std::regex(empty_regex))) {
    // Mark empty line properly.
    return kEmpty;
  }
  return kUnmatched;
}

// Creates instruction prototype from string of arguments and fills vector of
// arguments with objects of coresponding type.
void InstructionParser::ParseArgumentsLine(std::string *prototype,
                                           std::string arguments) {
  std::string raw_case_args = arguments;
  size_t arg_beg = 0;
  size_t arg_end = 0;

  std::transform(arguments.begin(),
                 arguments.end(),
                 arguments.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  while (arg_end < arguments.size()) {
    std::string args_left = arguments.substr(arg_beg);
    ArgumentSet arg_set;
    auto i = regex_parser.begin();
    bool found = false;

    while (i != regex_parser.end()) {
      auto parse_func = i->first;
      // After an argument there should be a comma or the end of line.
      // Positive lookahead `,(?=[^$])` excludes cases with trailing coma
      // after last instruction.
      std::regex arg_regex =
          std::regex("\\s*" + i->second + "\\s*(?:,(?=[^$])|$)",
                     std::regex_constants::icase);

      if (std::regex_search(args_left,
                            arg_set.match,
                            arg_regex,
                            std::regex_constants::match_continuous)) {
        arg_end += arg_set.match.str(0).size();
        arg_set.raw_case = raw_case_args.substr(arg_beg, arg_end);

        (this->*parse_func)(arg_set, prototype);
        found = true;
        break;
      }
      i++;
    }
    if (!found) {
      eh->Err("Couldn't match argument of any type: " + args_left);
      break;
    }
    arg_beg = arg_end;
  }
}
void InstructionParser::ParseMultiplier(ArgumentSet arg_set,
                                        std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  std::string mul_value = arg_match.str(1);

  if (mul_value.compare("vl") == 0) {
    prototype->push_back('M');
    args.push_back(SVE_MUL_VL);
  } else {
    IntegerOperand int_op = ExtractInteger(mul_value.substr(1));
    prototype->push_back('_');
    args.push_back(int_op);
  }
}

void InstructionParser::ParseShift(ArgumentSet arg_set,
                                   std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  const std::map<std::string, Shift> shift_map = {{"lsl", LSL},
                                                  {"ror", ROR},
                                                  {"lsr", LSR},
                                                  {"asr", ASR},
                                                  {"msl", MSL}};

  Shift s = shift_map.at(arg_match.str(1));
  IntegerOperand int_op = ExtractInteger(arg_match.str(2));

  if (prototype->back() == 'r') {
    Register r = std::get<Register>(args.back());
    args.back() = Operand(r, s, int_op.AsUint32());
    prototype->back() = 'o';
  } else {
    if (CurrArgumentInMemOperand(prototype->size()) ||
        mnemonic.compare("mvni") == 0 || mnemonic.compare("movi") == 0) {
      prototype->push_back('S');
      args.push_back(s);
    }
    args.push_back(int_op);
    prototype->push_back('_');
  }
}

void InstructionParser::ParseExtend(ArgumentSet arg_set,
                                    std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  const std::map<std::string, Extend> extend_map = {{"uxtw", UXTW},
                                                    {"sxtw", SXTW},
                                                    {"uxtb", UXTB},
                                                    {"sxtb", SXTB},
                                                    {"uxth", UXTH},
                                                    {"sxth", SXTH},
                                                    {"uxtx", UXTX},
                                                    {"sxtx", SXTX}};

  Extend e = extend_map.at(arg_match.str(1));
  IntegerOperand int_op{0};
  bool shift_imm = false;

  if (!arg_match.str(2).empty()) {
    int_op = ExtractInteger(arg_match.str(2).substr(1));
    shift_imm = true;
  }
  if (prototype->back() == 'r') {
    Register r = std::get<Register>(args.back());

    if (shift_imm) {
      args.back() = Operand(r, e, int_op.AsUint32());
    } else {
      args.back() = Operand(r, e);
    }

    prototype->back() = 'o';
  } else {
    args.push_back(e);
    prototype->push_back('e');

    if (shift_imm) {
      args.push_back(int_op);
      prototype->push_back('_');
    }
  }
}

void InstructionParser::ParseRegister(ArgumentSet arg_set,
                                      std::string *prototype) {
  Register reg;
  std::smatch arg_match = arg_set.match;
  const std::map<std::string, Register> register_map = {{"wzr", wzr},
                                                        {"xzr", xzr},
                                                        {"wsp", wsp},
                                                        {"sp", sp},
                                                        {"lr", lr}};

  // Currently parsed register is a special register (sp, xzr...).
  if (register_map.count(arg_match.str(1))) {
    reg = register_map.at(arg_match.str(1));
    // Currently parsed register is a general purpose register (x0 - x30).
  } else {
    unsigned reg_num = std::stoi(arg_match.str(2));
    char reg_type = arg_match.str(1).at(0);

    if ((reg_num >= kNumberOfRegisters) || (reg_num == kZeroRegCode)) {
      eh->Err("Register number should be in the range 0-30");
    }

    VIXL_ASSERT((reg_type == 'w' || reg_type == 'x'));
    reg = Register(reg_num, reg_type == 'w' ? kWRegSize : kXRegSize);
  }
  // Standalone register on the last place should be treated as an Operand for
  // some instructions. We need to handle such cases and convert arguments.
  if (MnemonicExists(*prototype + 'o', mnemonic) &&
      arg_match.suffix().str().empty()) {
    args.push_back(Operand(reg));
    prototype->push_back('o');
  } else {
    // If currently parsed register occurs just after memory operand we can
    // assume that it is the post indexing mode (for example [x0], x1).
    if (!mem_args.empty() &&
        (mem_args.back().end_pos == (prototype->size() - 1))) {
      mem_args.back().addr_mode = PostIndex;
      mem_args.back().end_pos += 1;
    }
    args.push_back(reg);
    prototype->push_back('r');
  }
}

void InstructionParser::ParseStatusFlags(ArgumentSet arg_set,
                                         std::string *prototype) {
  unsigned int nzcv_flags = 0;

  for (const auto &flag : arg_set.raw_case) {
    switch (flag) {
      case 'N':
        nzcv_flags |= N_mask;
        break;
      case 'Z':
        nzcv_flags |= Z_mask;
        break;
      case 'C':
        nzcv_flags |= C_mask;
        break;
      case 'V':
        nzcv_flags |= V_mask;
        break;
      default:
        break;
    }
  }
  args.push_back(static_cast<StatusFlags>(nzcv_flags));
  prototype->push_back('F');
}

void InstructionParser::ParseZRegister(ArgumentSet arg_set,
                                       std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  int reg_num = std::stoi(arg_match.str(1));
  unsigned int reg_size = sve_sizes_map.at(arg_match.str(2).at(0));

  args.push_back(ZRegister(reg_num, reg_size));
  prototype->push_back('z');

  if (!arg_match.str(3).empty()) {
    args.push_back(ExtractInteger(arg_match.str(3)));
    prototype->push_back('_');
  }
}

void InstructionParser::ParsePRegister(ArgumentSet arg_set,
                                       std::string *prototype) {
  Argument arg_object;
  std::smatch arg_match = arg_set.match;
  int reg_num = std::stoi(arg_match.str(1));

  if (arg_match.size() == 2) {
    arg_object = PRegister(reg_num);
    prototype->push_back('p');
  } else {
    char spe_arg = arg_match.str(2).at(0);
    if (spe_arg == 'm') {
      arg_object = PRegisterM(reg_num);
      prototype->push_back('M');
    } else if (spe_arg == 'z') {
      arg_object = PRegisterZ(reg_num);
      prototype->push_back('Z');
    } else {
      arg_object = PRegisterWithLaneSize(reg_num, sve_sizes_map.at(spe_arg));
      prototype->push_back('L');
    }
  }
  args.push_back(arg_object);
}

void InstructionParser::ParseSVEConstraint(ArgumentSet arg_set,
                                           std::string *prototype) {
  std::smatch arg_match = arg_set.match;
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

  auto it = constraint_map.find(arg_match.str(1));

  if (it != constraint_map.end()) {
    args.push_back(it->second);
    prototype->push_back('i');
  } else {
    eh->Err("SVE constraint in the wrong format.");
  }
}

void InstructionParser::ParseCondition(ArgumentSet arg_set,
                                       std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  auto it = condition_map.find(arg_match.str(1));

  if (it != condition_map.end()) {
    args.push_back(it->second);
    prototype->push_back('c');
  } else {
    eh->Err("Condition in a wrong format");
  }
}

void InstructionParser::ParseMemOffset(ArgumentSet arg_set,
                                       std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  IntegerOperand int_op = ExtractInteger(arg_match.str(1));

  prototype->push_back('_');
  args.push_back(int_op);
}

// Immediates are parsed corresponding to the sign and marked as "_" in the
// prototype string. They are replaced by the exact type in the vector of
// arguments and prototype string when all the arguments are parsed.
void InstructionParser::ParseImmediate(ArgumentSet arg_set,
                                       std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  IntegerOperand int_op = ExtractInteger(arg_match.str(1));

  // Standalone immediate on the last argument can be treated as an Operand in
  // some instructions. If we are parsing last argument and there is entry for
  // current mnemonic and prototype + 'o', we have to parse this argument as an
  // Operand.
  if (arg_match.suffix().str().empty() &&
      MnemonicExists(*prototype + 'o', mnemonic)) {
    if (int_op.IsInt64()) {
      args.push_back(Operand(int_op.AsInt64()));
      prototype->push_back('o');
    } else {
      eh->Err("Immediate format not suitable for the given instruction.");
    }
  } else {
    // If currently parsed immediate occurs just after memory operand we can
    // assume that it is the post indexing mode (for example [x0], #5).
    if (!mem_args.empty() &&
        (mem_args.back().end_pos == (prototype->size() - 1))) {
      mem_args.back().addr_mode = PostIndex;
      mem_args.back().end_pos += 1;
    }
    prototype->push_back('_');
    args.push_back(int_op);
  }
}

void InstructionParser::ParseFPImmediate(ArgumentSet arg_set,
                                         std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  const char *imm_str = arg_match.str(1).c_str();
  double imm = std::strtod(imm_str, NULL);

  if (errno == ERANGE) eh->Err("Immediate out of range.");

  args.push_back(imm);
  prototype->push_back('_');
}

void InstructionParser::ParseVRegister(ArgumentSet arg_set,
                                       std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  unsigned code = std::stoi(arg_match.str(1));
  char size = arg_match.str(3).at(0);
  VRegister vreg;

  if (code >= kNumberOfVRegisters) {
    eh->Err("Vector register number should be in the range 0-31");
  }

  if (!arg_match.str(2).empty()) {
    int lanes = std::stoi(arg_match.str(2));
    std::string val = arg_match.str(2) + arg_match.str(3);
    std::vector<std::string> reg_vals =
        {"16b", "8b", "8h", "4b", "4h", "4s", "2h", "2s", "2d", "1d"};

    if (std::find(reg_vals.begin(), reg_vals.end(), val) == reg_vals.end()) {
      eh->Err("Lanes number not suitable for given lane size.");
    }
    vreg = VRegister(code, lanes * sve_sizes_map.at(size), lanes);
  } else {
    vreg = VRegister(code, sve_sizes_map.at(size));
  }

  args.push_back(vreg);
  prototype->push_back('v');

  if (!arg_match.str(4).empty()) {
    args.push_back(ExtractInteger(arg_match.str(4)));
    prototype->push_back('_');
  }
}

// Parse and validate VRegister and ZRegister lists.
// For example: {v30.8b, v31.8b, v0.8b}
void InstructionParser::ParseRegisterList(ArgumentSet arg_set,
                                          std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  std::regex vreg_regex =
      std::regex("\\s*v(\\d{1,2})\\.(\\d{1,2})?(\\w)\\s*(?:,(?=[^$])|$)");
  std::regex zreg_regex =
      std::regex("\\s*z(\\d{1,2})\\.(b|h|s|d)\\s*(?:,(?=[^$])|$)");

  std::regex regex_type;
  ParserFn fn_type;

  std::string reg_list = arg_match.str(1);
  std::smatch match;
  std::vector<int> reg_codes;

  // Check type of the register list. Assign proper regular expression and
  // function for parsing its elements.
  if (arg_match.str(1).at(0) == 'v') {
    regex_type = vreg_regex;
    fn_type = &InstructionParser::ParseVRegister;
  } else if (arg_match.str(1).at(0) == 'z') {
    regex_type = zreg_regex;
    fn_type = &InstructionParser::ParseZRegister;
  } else {
    eh->Err("Wrong format of a register list.");
    return;
  }

  while (!reg_list.empty()) {
    if (std::regex_search(reg_list,
                          match,
                          regex_type,
                          std::regex_constants::match_continuous)) {
      (this->*fn_type)(ArgumentSet(match), prototype);
      reg_codes.push_back(std::stoi(match.str(1)));
      reg_list.erase(0, match.str(0).size());
    } else {
      eh->Err("Wrong format of a register list element.");
      return;
    }
  }
  // If there is an index caught by regex after vregister list in form
  // {...}[<index>], then parse it as an integer immediate.
  if (!arg_match.str(2).empty()) {
    IntegerOperand int_op = ExtractInteger(arg_match.str(2));
    prototype->push_back('_');
    args.push_back(int_op);
  }

  // Check if list elements are registers with consecutive numbers modulo 32.
  for (auto it = std::next(reg_codes.begin()); it != reg_codes.end();
       std::advance(it, 1)) {
    auto prev_code = std::prev(it);
    if (((*prev_code + 1) % 32) != *it) {
      eh->Err("Elements of the register list should have consecutive numbers");
    }
  }
}

void InstructionParser::ParseFPRegister(ArgumentSet arg_set,
                                        std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  char reg_size = arg_match.str(1).at(0);
  unsigned reg_num = std::stoi(arg_match.str(2));

  if (reg_num >= kNumberOfVRegisters) {
    eh->Err("Vector register number should be in the range 0-31");
  }

  switch (reg_size) {
    case 'b':
      args.push_back(BRegister(reg_num).AsVRegister());
      break;
    case 'h':
      args.push_back(HRegister(reg_num).AsVRegister());
      break;
    case 's':
      args.push_back(SRegister(reg_num).AsVRegister());
      break;
    case 'd':
      args.push_back(DRegister(reg_num).AsVRegister());
      break;
    case 'q':
      args.push_back(QRegister(reg_num).AsVRegister());
      break;
  }
  prototype->push_back('v');
}

void InstructionParser::ParseMemOperandList(ArgumentSet arg_set,
                                            std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  mem_args.push_back(MemDescriptor());
  MemDescriptor &mem_op = mem_args.back();

  mem_op.beg_pos = prototype->size();
  ParseArgumentsLine(prototype, arg_match.str(1));
  mem_op.end_pos = prototype->size() - 1;

  // Handling pre-index and offset addressing modes. Post-index mode is
  // handled in immediate parsing function - ParseImmediate.
  if (!arg_match.str(2).compare("!"))
    mem_op.addr_mode = PreIndex;
  else
    mem_op.addr_mode = Offset;
}

void InstructionParser::ParseLabel(ArgumentSet arg_set,
                                   std::string *prototype) {
  std::smatch arg_match = arg_set.match;
  auto it = labels.try_emplace(arg_match.str(1), Label());

  args.push_back(&(it.first->second));
  prototype->push_back('l');
}

// Combine parsed arguments from given range (specified by pos_start and
// pos_end) into SVEMemOperand or MemOperand.
void InstructionParser::ParseMemOperandTypes(std::string *prototype,
                                             MemDescriptor mem_arg) {
  size_t pos_start = mem_arg.beg_pos;
  size_t pos_end = mem_arg.end_pos;
  std::string mem_op_type = prototype->substr(pos_start, pos_end);
  std::string mem_prototype(*prototype);

  mem_prototype.replace(pos_start, pos_end - pos_start + 1, "s");

  if (!MnemonicExists(mem_prototype, mnemonic)) {
    mem_prototype.replace(pos_start, 1, "m");
  }

  IntegerOperand int_op(0);
  size_t imm_pos = mem_op_type.find('_');
  if (imm_pos != std::string::npos) {
    int_op = std::get<IntegerOperand>(args[pos_start + imm_pos]);
  }

  Argument arg_object;
  if (mem_prototype.at(pos_start) == 's') {
    if (imm_pos != std::string::npos && !int_op.IsUint64()) {
      eh->Err("Immediate in memory operand in a wrong format.");
    }

    if (!mem_op_type.compare("r")) {
      arg_object = SVEMemOperand(std::get<Register>(args[pos_start]));
    } else if (!mem_op_type.compare("z")) {
      arg_object = SVEMemOperand(std::get<ZRegister>(args[pos_start]));
    } else if (!mem_op_type.compare("r_")) {
      arg_object =
          SVEMemOperand(std::get<Register>(args[pos_start]), int_op.AsUint64());
    } else if (!mem_op_type.compare("z_")) {
      arg_object = SVEMemOperand(std::get<ZRegister>(args[pos_start]),
                                 int_op.AsUint64());
    } else if (!mem_op_type.compare("rr")) {
      arg_object = SVEMemOperand(std::get<Register>(args[pos_start]),
                                 std::get<Register>(args[pos_start + 1]));
    } else if (!mem_op_type.compare("rz")) {
      arg_object = SVEMemOperand(std::get<Register>(args[pos_start]),
                                 std::get<ZRegister>(args[pos_start + 1]));
    } else if (!mem_op_type.compare("zz")) {
      arg_object = SVEMemOperand(std::get<ZRegister>(args[pos_start]),
                                 std::get<ZRegister>(args[pos_start + 1]));
    } else if (!mem_op_type.compare("r_M")) {
      arg_object =
          SVEMemOperand(std::get<Register>(args[pos_start]),
                        int_op.AsUint64(),
                        std::get<SVEOffsetModifier>(args[pos_start + 2]));
    } else if (!mem_op_type.compare("ro")) {
      Operand o = std::get<Operand>(args[pos_start + 1]);
      if (o.IsExtendedRegister()) {
        arg_object = SVEMemOperand(std::get<Register>(args[pos_start]),
                                   o.GetBaseRegister(),
                                   o.GetExtend(),
                                   o.GetShiftAmount());
      } else if (o.IsShiftedRegister()) {
        arg_object = SVEMemOperand(std::get<Register>(args[pos_start]),
                                   o.GetBaseRegister(),
                                   o.GetShift(),
                                   o.GetShiftAmount());
      }
    } else if (!mem_op_type.compare("zze")) {
      arg_object = SVEMemOperand(std::get<ZRegister>(args[pos_start]),
                                 std::get<ZRegister>(args[pos_start + 1]),
                                 std::get<Extend>(args[pos_start + 2]));
    } else if (!mem_op_type.compare("zzS_")) {
      if (!int_op.IsUint32()) {
        eh->Err("Shift immediate in memory operand in a wrong format.");
      }
      arg_object = SVEMemOperand(std::get<ZRegister>(args[pos_start]),
                                 std::get<ZRegister>(args[pos_start + 1]),
                                 std::get<Shift>(args[pos_start + 2]),
                                 int_op.AsUint32());
    } else if (!mem_op_type.compare("zze_")) {
      if (!int_op.IsUint32()) {
        eh->Err("Extend immediate in memory operand in a wrong format.");
      }
      arg_object = SVEMemOperand(std::get<ZRegister>(args[pos_start]),
                                 std::get<ZRegister>(args[pos_start + 1]),
                                 std::get<Extend>(args[pos_start + 2]),
                                 int_op.AsUint32());
    } else if (!mem_op_type.compare("rzM")) {
      arg_object =
          SVEMemOperand(std::get<Register>(args[pos_start]),
                        std::get<ZRegister>(args[pos_start + 1]),
                        std::get<SVEOffsetModifier>(args[pos_start + 2]));
    } else if (!mem_op_type.compare("rze")) {
      arg_object = SVEMemOperand(std::get<Register>(args[pos_start]),
                                 std::get<ZRegister>(args[pos_start + 1]),
                                 std::get<Extend>(args[pos_start + 2]));
    }
  } else if (mem_prototype.at(pos_start) == 'm') {
    if (imm_pos != std::string::npos && !int_op.IsInt64()) {
      eh->Err("Immediate in memory operand in a wrong format.");
    }

    if (!mem_op_type.compare("r")) {
      arg_object = MemOperand(std::get<Register>(args[pos_start]));
    } else if (!mem_op_type.compare("r_")) {
      arg_object = MemOperand(std::get<Register>(args[pos_start]),
                              int_op.AsInt64(),
                              mem_arg.addr_mode);
    } else if (!mem_op_type.compare("rr")) {
      arg_object = MemOperand(std::get<Register>(args[pos_start]),
                              Operand(std::get<Register>(args[pos_start + 1])),
                              mem_arg.addr_mode);
    } else if (!mem_op_type.compare("ro")) {
      arg_object = MemOperand(std::get<Register>(args[pos_start]),
                              std::get<Operand>(args[pos_start + 1]),
                              mem_arg.addr_mode);
    }
  }

  // If arg_object(of std::variant type) holds std::monostate(index 0), then
  // parsing was unsucessful.
  if (arg_object.index() != 0) {
    args.erase(args.begin() + pos_start, args.begin() + pos_end + 1);
    args.insert(args.begin() + pos_start, arg_object);
    *prototype = mem_prototype;
  }
}

// Check if mnemonic has some additional condition (for example b.ne) and parse
// it as another argument.
void InstructionParser::ParseMnemonic(std::string *mnemonic,
                                      std::string *prototype) {
  std::string cond_regex = "(.*)\\.(.*)";
  std::smatch match_args;

  if (std::regex_match(*mnemonic, match_args, std::regex(cond_regex))) {
    *mnemonic = match_args.str(1);
    auto it = condition_map.find(match_args.str(2));
    if (it != condition_map.end()) {
      args.push_back(it->second);
      prototype->push_back('c');
    } else {
      eh->Err("Unrecognized condition in instruction mnemonic.");
    }
  }
}

// Parse immediates to given types. Types are resolved by matching currently
// built prototype (with '_' on immediate fields) with found full candidate
// prototype.
void InstructionParser::ParseImmediatesTypes(std::string *prototype) {
  std::smatch imm_match = FindImmPrototype(*prototype);
  size_t arg_index = prototype->find("_");
  size_t i = 1;

  while (i < imm_match.size() && arg_index != std::string::npos) {
    char imm_type = imm_match.str(i).at(0);

    if (imm_type == 'i' || imm_type == 'u') {
      IntegerOperand i_op(0);

      // From prototype string we know only that the current argument is an
      // immediate -> "_". It can be of a floating point or an integer type.
      // Check for the proper type is needed in this case.
      try {
        i_op = std::get<IntegerOperand>(args[arg_index]);
      } catch (std::bad_variant_access) {
        eh->Err(
            "Wrong argument for the instruction. Expected argument: integer "
            "immediate");
        return;
      }

      if (!imm_match.str(i).compare("i64") && i_op.IsInt64())
        args[arg_index] = i_op.AsInt64();
      else if (!imm_match.str(i).compare("i32") && i_op.IsInt32())
        args[arg_index] = i_op.AsInt32();
      else if (!imm_match.str(i).compare("i") && i_op.IsIntN(sizeof(int) * 8))
        args[arg_index] = static_cast<int>(i_op.AsIntN(sizeof(int) * 8));
      else if (!imm_match.str(i).compare("u64") &&
               (i_op.IsUint64() || i_op.IsInt64()))
        args[arg_index] = i_op.AsUint64();
      else if (!imm_match.str(i).compare("u32") &&
               (i_op.IsUint32() || i_op.IsInt32()))
        args[arg_index] = i_op.AsUint32();
      else if (!imm_match.str(i).compare("u") &&
               (i_op.IsUintN(sizeof(unsigned) * 8) ||
                i_op.IsIntN(sizeof(unsigned) * 8)))
        args[arg_index] =
            static_cast<unsigned>(i_op.AsUintN(sizeof(unsigned) * 8));
    } else if (imm_type == 'f' || imm_type == 'd') {
      double d_op = 0.0;

      // This case is analogous to the above one with the IntegerOperand.
      try {
        d_op = std::get<double>(args[arg_index]);
      } catch (std::bad_variant_access) {
        eh->Err(
            "Wrong argument for the instruction. Expected argument: floating "
            "point immediate");
        return;
      }

      if (!imm_match.str(i).compare("f16"))
        args[arg_index] = static_cast<Float16>(d_op);
      else if (!imm_match.str(i).compare("f"))
        args[arg_index] = static_cast<float>(d_op);
      else if (!imm_match.str(i).compare("d"))
        args[arg_index] = d_op;
    }

    arg_index = prototype->find("_", arg_index + 1);
    i++;
  }
  *prototype = imm_match.str(0);
}

// Find full prototype with proper immediate types by matching with currently
// built prototype.
std::smatch InstructionParser::FindImmPrototype(std::string prototype) {
  std::string imm_regex = "([ui](?:32|64)?|[fd](?:16)?)";
  std::smatch imm_match;
  size_t imm_index = prototype.find("_", 0);

  // Create regular expression from prototype by replacing '_' with expression
  // matching any immediate type.
  while (imm_index != std::string::npos) {
    prototype.replace(imm_index, 1, imm_regex);
    imm_index = prototype.find("_", imm_index);
  }

  for (const auto &p : get_prototypes()) {
    if (std::regex_match(p, imm_match, std::regex(prototype)) &&
        mnemonic_exists(imm_match.str(0), mnemonic)) {
      break;
    }
  }
  return imm_match;
}

// Convert string to integer based on the sign. Check for out of range error.
IntegerOperand InstructionParser::ExtractInteger(std::string int_str) {
  IntegerOperand int_op(0);

  if (int_str.at(0) == '-') {
    int64_t i_imm = std::strtoll(int_str.c_str(), NULL, 0);
    int_op = IntegerOperand(i_imm);
  } else {
    uint64_t u_imm = std::strtoull(int_str.c_str(), NULL, 0);
    int_op = IntegerOperand(u_imm);
  }
  if (errno == ERANGE) {
    eh->Err("Immediate out of range.");
  }
  return int_op;
}

bool InstructionParser::CurrArgumentInMemOperand(size_t curr_pos) {
  if (!mem_args.empty()) {
    MemDescriptor mem_op = mem_args.back();
    if ((mem_op.beg_pos < curr_pos) && (curr_pos <= mem_op.end_pos)) {
      return true;
    }
  }
  return false;
}

bool InstructionParser::MnemonicExists(std::string prototype,
                                       std::string mnemonic) {
  std::smatch imm_prototype = FindImmPrototype(prototype);

  return mnemonic_exists(imm_prototype.str(0), mnemonic);
}
std::string InstructionParser::GetMnemonic() { return mnemonic; }

std::vector<Argument> InstructionParser::GetArgs() { return args; }
}
}
}  // namespace vixl::aarch64::tasm

#endif  // __cplusplus >= 201703L