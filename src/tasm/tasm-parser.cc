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

const std::vector<std::pair<InstructionParser::ParserFn, std::string> >
    InstructionParser::regex_parser =
        {{&InstructionParser::ParseVRegister, "(b|h|s|d)(\\d{1,2})"},
         {&InstructionParser::ParseShift, "(lsl|lsr|asr|ror)#(\\d+)"},
         {&InstructionParser::ParseExtend,
          "(sxtb|sxth|sxtw|sxtx|uxtb|uxth|uxtw|uxtx)(#\\d*)?"},
         {&InstructionParser::ParseImmediate, "#(-?\\d+|-?0x[\\da-fA-F]+)"},
         {&InstructionParser::ParseRegister, "(lr|sp|wsp|xzr|wzr)"},
         {&InstructionParser::ParseRegister, "(w|x)(\\d{1,2})"},
         {&InstructionParser::ParseZRegister, "z(\\d{1,2})\\.(b|h|s|d)"},
         {&InstructionParser::ParsePRegister, "p(\\d{1,2})"},
         {&InstructionParser::ParsePRegister, "p(\\d{1,2})\\.(b|h|s|d|q)"},
         {&InstructionParser::ParsePRegister, "p(\\d{1,2})/(m|z)"},
         {&InstructionParser::ParseSVEConstraint, "(pow2|vl\\d|mul\\d|all)"},
         {&InstructionParser::ParseVRegister, "v(\\d{1,2})\\.(\\d{1,2})(\\w)"},
         {&InstructionParser::ParseVRegisterList,
          "\\{(.*)\\}(?:\\[(\\d{1,2})\\])?"},
         {&InstructionParser::ParseMemOperandList, "\\[(.*)\\](!?)"}};

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

InstructionParser::~InstructionParser() {}

// Load instruction and split it into mnemonic and string of arguments
bool InstructionParser::LoadInstruction(std::string inst_line,
                                        std::string *prototype) {
  std::string empty_regex = "\\s*";
  std::string comment_regex = "(.*)//.*";
  std::string inst_regex = "\\s*([a-zA-Z0-9.]+)(?:\\s+(.+))?";

  std::smatch match_args;

  mnemonic.clear();
  args.clear();
  mem_args.clear();

  // Strip comments from current line.
  if (std::regex_match(inst_line, match_args, std::regex(comment_regex))) {
    inst_line = match_args.str(1);
  }

  // Parse current line as an instruction.
  if (std::regex_match(inst_line, match_args, std::regex(inst_regex))) {
    // Make instruction line case insensitive by converting all letters to
    // lower case.
    std::transform(inst_line.begin(),
                   inst_line.end(),
                   inst_line.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    mnemonic = match_args.str(1);
    std::string arguments =
        std::regex_replace(match_args.str(2), std::regex(" "), "");
    ParseArgumentsLine(prototype, arguments);

    for (const auto &mem_arg : mem_args) {
      ParseMemOperandTypes(prototype, mem_arg);
    }

    ParseMnemonic(&mnemonic, prototype);
    ParseImmediatesTypes(prototype);
  } else if (std::regex_match(inst_line, match_args, std::regex(empty_regex))) {
    return false;
  }
  return true;
}

// Creates instruction prototype from string of arguments and fills vector of
// arguments with objects of coresponding type.
void InstructionParser::ParseArgumentsLine(std::string *prototype,
                                           std::string arguments) {
  std::smatch args_match;

  while (!arguments.empty()) {
    auto i = regex_parser.begin();
    bool found = false;

    while (i != regex_parser.end()) {
      auto parse_func = i->first;
      // After an argument there should be a comma or the end of line.
      // Positive lookahead `,(?=[^$])` excludes cases with trailing coma
      // after last instruction.
      std::regex arg_regex = std::regex(i->second + "(?:,(?=[^$])|$)");

      if (std::regex_search(arguments,
                            args_match,
                            arg_regex,
                            std::regex_constants::match_continuous)) {
        found = true;
        // TODO: return error code from the parser functions and
        // stop parsing arguments further.
        (this->*parse_func)(args_match, prototype);
        break;
      }
      i++;
    }
    if (!found) {
      std::cerr << "Error: Argument not matching any type" << std::endl;
      break;
    }
    arguments.erase(0, args_match.str(0).size());
  }
}

int InstructionParser::ParseShift(std::smatch arg_match,
                                  std::string *prototype) {
  if (prototype->back() == 'r') {
    Register r = std::get<Register>(args.back());
    Shift s = shift_map.at(arg_match.str(1));
    IntegerOperand int_op = ExtractInteger(arg_match.str(2));

    if (!int_op.IsUint32()) {
      std::cerr << "shift immediate in the wrong format" << std::endl;
      return -1;
    }
    args.back() = Operand(r, s, int_op.AsUint32());
    prototype->back() = 'o';
  }
  return 0;
}

int InstructionParser::ParseExtend(std::smatch arg_match,
                                   std::string *prototype) {
  if (prototype->back() == 'r') {
    Register r = std::get<Register>(args.back());
    Extend e = extend_map.at(arg_match.str(1));

    if (arg_match.size() > 1) {
      IntegerOperand int_op = ExtractInteger(arg_match.str(2));

      if (!int_op.IsUint32()) {
        std::cerr << "extend immediate in the wrong format" << std::endl;
        return -1;
      }
      args.back() = Operand(r, e, int_op.AsUint32());
    } else {
      args.back() = Operand(r, e);
    }
    prototype->back() = 'o';
  }
  return 0;
}

int InstructionParser::ParseRegister(std::smatch arg_match,
                                     std::string *prototype) {
  Register reg;
  // Currently parsed register is a special register (sp, xzr...).
  if (register_map.count(arg_match.str(1))) {
    reg = register_map.at(arg_match.str(1));
    // Currently parsed register is a general purpose register (x0 - x31).
  } else {
    int reg_num = std::stoi(arg_match.str(2));
    char reg_type = arg_match.str(1).at(0);

    if (reg_num < 0 || reg_num > 31) return -1;

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
    if (!mem_args.empty() && mem_args.back().end_pos == prototype->size() - 1) {
      mem_args.back().addr_mode = PostIndex;
      mem_args.back().end_pos += 1;
    }
    args.push_back(reg);
    prototype->push_back('r');
  }
  return 0;
}

int InstructionParser::ParseZRegister(std::smatch arg_match,
                                      std::string *prototype) {
  int reg_num = std::stoi(arg_match.str(1));
  unsigned int reg_size = sve_sizes_map.at(arg_match.str(2).at(0));

  args.push_back(ZRegister(reg_num, reg_size));
  prototype->push_back('z');
  return 0;
}

int InstructionParser::ParsePRegister(std::smatch arg_match,
                                      std::string *prototype) {
  Argument arg_object;
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
  return 0;
}

int InstructionParser::ParseSVEConstraint(std::smatch arg_match,
                                          std::string *prototype) {
  auto it = constraint_map.find(arg_match.str(1));
  if (it != constraint_map.end()) {
    args.push_back(it->second);
    prototype->push_back('c');
  } else {
    std::cerr << "SVE constraint in the wrong format" << std::endl;
    return -1;
  }
  return 0;
}

// Immediates are parsed corresponding to the sign and marked as "_" in the
// prototype string. They are replaced by the exact type in the vector of
// arguments and prototype string when all the arguments are parsed.
int InstructionParser::ParseImmediate(std::smatch arg_match,
                                      std::string *prototype) {
  IntegerOperand int_op = ExtractInteger(arg_match.str(1));

  // Standalone immediate on the last argument can be treated as an Operand in
  // some instructions. If we are parsing last argument and there is entry for
  // current mnemonic and prototype + 'o', we have to parse this argument as an
  // Operand.
  if (arg_match.suffix().length() == 0 &&
      MnemonicExists(*prototype + 'o', mnemonic)) {
    if (int_op.IsInt64()) {
      args.push_back(Operand(int_op.AsInt64()));
      prototype->push_back('o');
    } else {
      std::cerr << "Error: immediate not suitable for given instruction: "
                << mnemonic << std::endl;
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
  return 0;
}

int InstructionParser::ParseVRegister(std::smatch arg_match,
                                      std::string *prototype) {
  int code = std::stoi(arg_match.str(1));
  int lanes = std::stoi(arg_match.str(2));
  char size = arg_match.str(3).at(0);
  std::map<int, char> vreg_vals =
      {{16, 'b'}, {8, 'b'}, {8, 'h'}, {4, 'h'}, {4, 's'}, {2, 's'}};

  if (code > 32 || code < 0) return -1;

  if (!vreg_vals.count(lanes) || vreg_vals.at(lanes) != size) return -1;

  args.push_back(VRegister(code, lanes * sve_sizes_map.at(size), lanes));
  prototype->push_back('v');
  return 0;
}

// Parse and validate VRegister list (for example {v30.8b, v31.8b, v0.8b})
int InstructionParser::ParseVRegisterList(std::smatch arg_match,
                                          std::string *prototype) {
  std::string vreg_list = arg_match.str(1);
  std::regex vreg_regex =
      std::regex("v(\\d{1,2})\\.(\\d{1,2})(\\w)(?:,(?=[^$])|$)");
  std::smatch vreg_match;
  std::vector<int> vreg_codes;

  while (!vreg_list.empty()) {
    if (std::regex_search(vreg_list,
                          vreg_match,
                          vreg_regex,
                          std::regex_constants::match_continuous)) {
      ParseVRegister(vreg_match, prototype);
      vreg_codes.push_back(std::stoi(vreg_match.str(1)));
      vreg_list.erase(0, vreg_match.str(0).size());
    } else {
      std::cerr << "Error: wrong format of a VRegister list element"
                << std::endl;
      return -1;
    }
  }
  // If there is an index caught by regex after vregister list in form
  // {...}[<index>], then parse it as an integer immediate.
  if (!arg_match.str(2).empty()) {
    IntegerOperand int_op = ExtractInteger(arg_match.str(2));
    prototype->push_back('_');
    args.push_back(int_op);
  }

  size_t inst_args_num = mnemonic.back() - '0';
  // Check if mnemonic number matches list size. For example:
  // st3 {v30.8b, v31.8b, v0.8b}
  if (vreg_codes.size() != inst_args_num) {
    std::cerr << "Error: number of elements in vregister list doesn't"
              << " match elements required by instruction: " << mnemonic
              << std::endl;
  }
  // Check if list elements are registers with consecutive numbers modulo 32.
  for (auto it = std::next(vreg_codes.begin()); it != vreg_codes.end();
       std::advance(it, 1)) {
    auto prev_code = std::prev(it);
    if ((*prev_code + 1) % 32 != *it)
      std::cerr << "Error: elements of the list should have consecutive numbers"
                << std::endl;
  }
  return 0;
}

int InstructionParser::ParseMemOperandList(std::smatch arg_match,
                                           std::string *prototype) {
  MemDescriptor mem_op;

  mem_op.beg_pos = prototype->size();
  ParseArgumentsLine(prototype, arg_match.str(1));
  mem_op.end_pos = prototype->size() - 1;

  // Handling pre-index and offset addressing modes. Post-index mode is
  // handled in immediate parsing function - ParseImmediate.
  if (!arg_match.str(2).compare("!"))
    mem_op.addr_mode = PreIndex;
  else
    mem_op.addr_mode = Offset;

  mem_args.push_back(mem_op);
  return 0;
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

  uint64_t imm = 0;
  if (size_t imm_pos = mem_op_type.find('_') != std::string::npos) {
    IntegerOperand int_op = std::get<IntegerOperand>(args[pos_start + imm_pos]);
    if (int_op.IsUint64())
      imm = int_op.AsUint64();
    else
      std::cerr << "Error: immediate in memory operand in a wrong format"
                << std::endl;
  }

  Argument arg_object;
  // Is there some way to write switch/case with strings ?
  if (mem_prototype.at(pos_start) == 's') {
    // TODO: Handle all the cases of SVEMemOperand
    if (!mem_op_type.compare("r_")) {
      arg_object = SVEMemOperand(std::get<Register>(args[pos_start]), imm);
    } else if (!mem_op_type.compare("z_")) {
      arg_object = SVEMemOperand(std::get<ZRegister>(args[pos_start]), imm);
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
  } else if (mem_prototype.at(pos_start) == 'm') {
    // TODO: Handle all cases of MemOperand
    if (!mem_op_type.compare("r_")) {
      arg_object = MemOperand(std::get<Register>(args[pos_start]),
                              imm,
                              mem_arg.addr_mode);
    } else if (!mem_op_type.compare("rr")) {
      arg_object = MemOperand(std::get<Register>(args[pos_start]),
                              Operand(std::get<Register>(args[pos_start + 1])),
                              mem_arg.addr_mode);
    } else if (!mem_op_type.compare("r")) {
      arg_object = MemOperand(std::get<Register>(args[pos_start]));
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
  std::map<std::string, Condition> cond_vals = {{"eq", eq},
                                                {"ne", ne},
                                                {"cs", cs},
                                                {"cc", cc},
                                                {"mi", mi},
                                                {"pl", pl},
                                                {"vs", vs},
                                                {"vc", vc},
                                                {"hi", hi},
                                                {"ls", ls},
                                                {"ge", ge},
                                                {"lt", lt},
                                                {"gt", gt},
                                                {"le", le},
                                                {"al", al},
                                                {"nv", nv}};

  if (std::regex_match(*mnemonic, match_args, std::regex(cond_regex))) {
    *mnemonic = match_args.str(1);
    auto it = cond_vals.find(match_args.str(2));
    if (it != cond_vals.end()) {
      args.push_back(it->second);
      prototype->push_back('C');
    } else {
      std::cerr << "Unrecognized condition in instruction mnemonic: "
                << match_args.str(2) << std::endl;
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
    IntegerOperand i_op = std::get<IntegerOperand>(args[arg_index]);

    if (!imm_match.str(i).compare("i64") && i_op.IsInt64())
      args[arg_index] = i_op.AsInt64();
    else if (!imm_match.str(i).compare("i32") && i_op.IsInt32())
      args[arg_index] = i_op.AsInt32();
    else if (!imm_match.str(i).compare("i") && i_op.IsIntN(sizeof(int)))
      args[arg_index] = static_cast<int>(i_op.AsIntN(sizeof(int)));
    else if (!imm_match.str(i).compare("u64") && i_op.IsUint64())
      args[arg_index] = i_op.AsUint64();
    else if (!imm_match.str(i).compare("u32") && i_op.IsUint32())
      args[arg_index] = i_op.AsUint32();
    else if (!imm_match.str(i).compare("u") && i_op.IsUintN(sizeof(unsigned)))
      args[arg_index] = static_cast<unsigned>(i_op.AsUintN(sizeof(unsigned)));
    else
      std::cerr << "Error: immediate in a wrong format for the given mnemonic"
                << std::endl;
    arg_index = prototype->find("_");
    i++;
  }
  *prototype = imm_match.str(0);
}

// Find full prototype with proper immediate types by matching with currently
// built prototype.
std::smatch InstructionParser::FindImmPrototype(std::string prototype) {
  std::string imm_regex = "([ui](?:32|64)?)";
  std::smatch imm_match;
  size_t imm_index = prototype.find("_", 0);

  // Create regular expression from prototype by replacing '_' with expression
  // matching any immediate type.
  while (imm_index != std::string::npos) {
    prototype.replace(imm_index, 1, imm_regex);
    imm_index = prototype.find("_", imm_index);
  }

  for (const auto &p : get_prototypes()) {
    if (std::regex_match(p, imm_match, std::regex(prototype))) break;
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
    std::cerr << "Error: immediate out of range: " << int_str << std::endl;
  }
  return int_op;
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