#!/usr/bin/env python3

# Copyright 2021, VIXL authors
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   * Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#   * Neither the name of ARM Limited nor the names of its contributors may be
#     used to endorse or promote products derived from this software without
#     specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from __future__ import print_function
from jinja2 import Template
from collections import defaultdict
import argparse
import re
import os
import string
import sys

# Purpose of this script is to parse file with instructions in reference manual
# syntax. For example:
# UZP1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
# UZP2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
# BFM  <Wd>, <Wn>, #<immr>, #<imms>
# ...
# Then check which instructions have matching functions implemented in VIXL:
# void uzp1(const ZRegister& zd, const ZRegister& zn, const ZRegister& zm);
# void uzp2(const ZRegister& zd, const ZRegister& zn, const ZRegister& zm);
# void bfm(const Register& rd, const Register& rn, unsigned immr, unsigned imms);

# C++ code of text assembler is generated with supported instructions using the
# jinja2 library. To test if instruction operands match, "prototypes" are used.
# They are a single-word representation of the operand list. For example:
#   "<Zd>.<T>, <Zn>.<T>, <Zm>.<T>"" -> prototype: "zzz"
#   "const ZRegister& zd, const ZRegister& zn, const ZRegister& zm" -> prototype: "zzz"
# Prototypes are also used internally in text assembler code.

REGEX_MAP = {
  # REGISTERS
  "<([WX][dnm]|[WX]?SP|[WX][tdnm]\|[W]?SP)>|<R><[mntd]>": "r",
  "[XW]ZR": "r",
  "<[XW][st]>": "r",
  "<[WX]dn>" : "r",
  "<[WX]a>": "r",
  "<R><n\|SP>": "r",
  "<R><dn>": "r",
  "<[WX]t[12]>": "r",
  "<[WX]\([ts]\+1\)>": "r",
  # ZREGISTERS
  "<Z[tdn]>": "z",
  "<Z[adnm]>\.<T>": "z",
  "<Zda>\.<T>": "z",
  "<Zd[an]>\.[BHSD]": "z",
  "<Z[dnm]>\.Q": "z",
  "<Z[dnm]>\.[BHSD]": "z",
  "<Z[nm]>\.<Tb>": "z",
  "<Zm>\.[BSDH]\[<imm>\]": "zi",
  "<Zdn>\.<T>": "z",
  "<Zn>\.<T>\[<imm>\]": "zi",
  "\{ <Z[tn](1)?>\.(?:[HSBD]|<T>)(, <Z[tn](?P<num>[234])?>\.(?:[HSBD]|<T>))* \}": "z_list",
  # PREGISTERS
  "<Pg>/M": "M",
  "<Pg>/Z": "Z",
  "<Pg>/<ZM>": "p",
  "<P[gt]>": "p",
  "<P[dnm]>\.<T>": "L",
  "<Pdn>\.<T>": "L",
  "<P[ndmg]>\.[BH]": "L",
  "<Pd[nm]>\.B": "L",
  # VREGISTERS
  "<D[ndma]>|<H[ndma]>|<S[ndma]>": "v",
  "<Bt>|<Ht>|<S[tn]>|<Dt>|<Q[tdn]>": "v",
  "<V><[dnm]>": "v",
  "<V><dn>": "v",
  "<V[ab]><[dnm]>": "v",
  "<V[dnm]>\.<T[ab]>": "v",
  "<V[dnm]>\.<T>": "v",
  "<V[adnm]>\.\d{1,2}[SBDH]": "v",
  "<V[nd]>\.D\[1\]": "v",
  "<V[nmd]>\.<Ts?>\[<index[12]?>\]": "vi",
  "<V[nm]>\.4?[HBSD]\[<index>\]": "vi",
  "<[SDQ]t[12]>": "v",
  "\{ <Vt>\.<T>(, <Vt(?P<num>[234])?>\.<T>)* \}(?P<imm>\[<index>\])?": "v_list",
  "\{ <V[tn]>\.[0-9]{0,2}[HSBD](, <V[tn]\+?(?P<num>[1234])>\.[0-9]{0,2}[HSBD])* \}(?P<imm>\[<index>\])?": "v_list",
  # IMMEDIATES
  "#<imm>, <shift>": "(o|ii)", # exception -> VIXL treats this combination as an Operand type
  "#<rotate>": "i",
  "#<index>": "i",
  "<pattern>": "i",
  "#<immr>|#<imms>": "i",
  "#<imm>": "i",
  "#<imm1>|#<imm2>|#<imm6>|#<imm8>": "i",
  "#<uimm6>|#<uimm4>": "i",
  "#<fbits>": "i",
  "<shift>": "i",
  "#<shift>": "i",
  "#<lsb>": "i",
  "#<width>": "i",
  "#<imm16>": "i",
  "#<const>": "i",
  "MUL #<imm>": "i",
  "LSL #<shift>": "i",
  "LSL #0": "Si",
  "MSL #<amount>": "Si",
  "LSL #<amount>": "S?i",
  "#<op1>|#<op2>": "i",
  "#0|#2|#4|#8|#16": "i",
  "<C[nm]>": "i",
  # FLOATING POINT
  "#0.0": "i",
  "<const>": "i",
  # MEM OPERANDS
  # POST INDEXING
  "\[<Xn\|SP>\], <Xm>": "__mem__",
  "\[<Xn\|SP>\], #[0-9]{1,2}": "__mem__",
  "\[<Xn\|SP>\], #<imm>": "__mem__",
  "\[<Xn\|SP>\], #<simm>": "__mem__",
  "\[<Xn\|SP>\], <imm>": "__mem__",
  # OTHERS
  "\[<Xn\|SP>\]": "__mem__",
  "\[<Xn\|SP>\]!": "__mem__",
  "\[<Xn\|SP>, #<imm>\]": "__mem__",
  "\[<Xn\|SP>, #<imm>\]!": "__mem__",
  "\[<Xn\|SP>,#0\]": "__mem__",
  "\[<Xn\|SP>, #<imm>, MUL VL\]" : "__mem__",
  "\[<Xn\|SP>, \(<Wm>\|<Xm>\)\]": "__mem__",
  "\[<Xn\|SP>, \(<Wm>\|<Xm>\), <extend> \]": "__mem__",
  "\[<Xn\|SP>, \(<Wm>\|<Xm>\), <extend> <amount>\]": "__mem__",
  "\[<Xn\|SP>, <Xm>, LSL <amount>\]": "__mem__",
  "\[<Xn\|SP>, <Xm>, LSL #2\]": "__mem__",
  "\[<Xn\|SP>, <Xm>\]": "__mem__",
  "\[<Xn\|SP>, #<simm>\]!": "__mem__",
  "\[<Xn\|SP>, #<pimm>\]": "__mem__",
  "\[<Xn\|SP>, #<simm>\]": "__mem__",
  "\[<Xn\|SP>, <Xm>, LSL #3\]": "__mem__",
  "\[<Xn\|SP> ,#0\]": "__mem__",
  "\[<Xn\|SP> \]": "__mem__",
  # SVE MEM OPERANDS
  "\[<Zn>\.S\]": "__mem__",
  "\[<Zn>\.<T>, <Zm>\.<T>\]": "__mem__",
  "\[<Zn>\.D, <Zm>\.D, SXTW\]": "__mem__",
  "\[<Zn>\.D, <Zm>\.D, UXTW\]": "__mem__",
  "\[<Zn>\.D, <Zm>\.D, SXTW <amount>\]": "__mem__",
  "\[<Zn>\.D, <Zm>\.D, UXTW <amount>\]": "__mem__",
  "\[<Zn>\.<T>, <Zm>\.<T>, <mod> <amount>\]": "__mem__",
  "\[<Xn\|SP>, <Zm>\.S, <mod> #1\]": "__mem__",
  "\[<Xn\|SP>, <Zm>\.D, LSL #1\]": "__mem__",
  "\[<Xn\|SP>, <Xm>, LSL #1\]": "__mem__",
  "\[<Zn>\.S, #<imm>\]": "__mem__",
  "\[<Zn>\.D\]": "__mem__",
  "\[<Zn>\.D, #<imm>\]": "__mem__",
  "\[<Xn\|SP>, <Zm>\.D, <mod> #1\]": "__mem__",
  "\[<Xn\|SP>, <Zm>\.D\]": "__mem__",
  "\[<Xn\|SP>, <Zm>\.S, <mod>\]": "__mem__",
  "\[<Xn\|SP>, <Zm>\.D, <mod>\]": "__mem__",
  "\[<Xn\|SP>, <Zm>\.S, <mod> #3\]": "__mem__",
  "\[<Xn\|SP>, <Zm>\.D, <mod> #3\]": "__mem__",
  "\[<Xn\|SP>, <Zm>\.D, LSL #3\]": "__mem__",
  "\[<Xn\|SP>, <Zm>\.S, <mod> #2\]": "__mem__",
  "\[<Xn\|SP>, <Zm>\.D, <mod> #2\]": "__mem__",
  "\[<Xn\|SP>, <Zm>\.D, LSL #2\]": "__mem__",
  # OTHERS
  "<prfop>": "P",
  "\(<prfop>\|#<imm5>\)": "(P|i)",
  "<label>": "l",
  "<extend>": "e",
  "<extend> #<amount>": "e",
  "LSL|ROR|<shift> #<amount>": "S",
  "<cond>|invert\(<cond>\)": "c",
  "<option>\|#<imm>": "Bb",
  "<option>nXS\|#<imm>": "Bb",
  "<targets>": "T",
  "#<mask>": "F",
  "#<nzcv>": "F",
  "<dc_op>": "D",
  "\(<systemreg>\|S<op0>_<op1>_<Cn>_<Cm>_<op2>\)": "R",
  "<pstatefield>": "R",
  "<ic_op>": "I",
}

TYPES = {
  "InstructionCacheOp": "I",
  "StatusFlags": "F",
  "SystemRegister": "R",
  "PrefetchOperation": "P",
  "CPURegister": "C",
  "LoadStoreScalingOption": "o",
  "BarrierDomain": "B",
  "BranchTargetIdentifier": "T",
  "BarrierType": "b",
  "Condition": "c",
  "DataCacheOp": "D",
  "MemOperand": "m",
  "SVEMemOperand": "s",
  "SystemHint": "H",
  "Register" : "r",
  "PRegisterM": "M",
  "PRegisterWithLaneSize": "L",
  "PRegister": "p",
  "PRegisterZ": "Z",
  # no support for functions taking RawLiteral in VIXL
  #"RawLiteral": "r",
  "VRegister": "v",
  "ZRegister": "z",
  "Operand" : "o",
  "int64_t" : "i64",
  "int": "i",
  "uint64_t" : "u64",
  "unsigned": "u",
  "double": "d",
  "float": "f",
  "Float16": "f16",
  "Extend" : "e",
  "Shift" : "S",
  "Label*": "l",
}


# Dictionary with matching instructions names that are different in VIXL and
# reference manual.
I_ALIAS = {
  "and" : "and_",
  "not" : "not_"
}

ROOT_DIR = os.path.dirname(os.path.realpath(__file__))
ASSEMBLER_HEADER = os.path.join(ROOT_DIR, '..', '..', 'src',
                                          'aarch64', 'assembler-aarch64.h')

# Class responsible for handling errors. Errors are printed in two files:
# - instruction-add-general-err.out : about mnemonics supported by VIXL
#   that couldn't be added
# - instruction-add-support-err.out : about mnemonics not supported by VIXL
class ErrorHandler(object):
  def __init__(self, signatures, verbosity):
    self.verbosity = verbosity
    self.signatures = signatures
    self.general_err_fn = "./instruction-add-general-err.out"
    self.support_err_fn = "./instruction-add-support-err.out"
    self.general_err_num = 0
    self.support_err_num = 0

  def __enter__(self):
    self.general_err_fp = open(self.general_err_fn, "w")
    self.support_err_fp = open(self.support_err_fn, "w")
    return self

  def __exit__(self, exc_type, exc_val, exc_tb):
    err_num = self.general_err_num + self.support_err_num
    self.general_err_fp.close()
    self.support_err_fp.close()

    if err_num == 0:
      return

    print("{} errors were raised.\n".format(err_num))
    print("{} about instructions supported by VIXL.".format(self.general_err_num),
         file=sys.stderr)
    print("Located in file {}\n".format(self.general_err_fn), file=sys.stderr)
    print("{} about instructions not supported by VIXL.".format(self.support_err_num),
          file=sys.stderr)
    print("Located in file {}\n".format(self.support_err_fn), file=sys.stderr)

  def PrintError(self, message, mnemonic, description=None):
    if mnemonic in self.signatures:
      err_file = self.general_err_fp
      self.general_err_num += 1
    else:
      err_file = self.support_err_fp
      self.support_err_num += 1

    error = "ERROR: {}\n".format(message)
    error += "mnemonic: {}\n".format(mnemonic)
    error += "\n" if description is None else description + '\n'
    print(error, file=err_file)

  def PrintMsg(self, message, verbosity):
    if verbosity <= self.verbosity:
      print(message)

class InstructionSet(object):
  def __init__(self, signature, types):
    self.signature = signature
    self.types = types
    self.mnemonics = set()


# Parse instructions operand list and find all possible placements of optional
# arguments. For example:
#   "<Xdn>{, <pattern>{, MUL #<imm>}}"
# Will be expanded to:
#   ["<Xdn>", "<Xdn>, <pattern>", "<Xdn>, <pattern>, MUL #<imm>"]
#
# Curly braces are used to mark optional operands and register lists. This
# function parses optional operands and leaves register lists unchanged.
def ParseOptionalArgs(arg_str):
  stack = [(arg_str, 0, [])]
  opt_set = set()

  while True:
    if len(stack) == 0:
      break
    arg_str, i, i_stack = stack.pop()

    if i >= len(arg_str):
      opt_set.add(arg_str)
      continue

    # There are two cases:
    # - register list (for example { <Zt>.B })
    # - optional arguments (for example {, <shift>bf #<amount>}).
    # If there is beginning of a register type after '{', expression is treated
    # as a register list. Otherwise as an optional argument.
    if (arg_str[i - 1:i + 3] == "{ <Z"
        or arg_str[i - 1:i + 3] == "{ <V") :
      i_stack.append((i - 1, False))
    elif arg_str[i - 1] == "{":
      i_stack.append((i - 1, True))

    if arg_str[i] == "}" and len(i_stack) > 0:
      beg, is_opt = i_stack.pop()
      if is_opt:
        # When optional operand is encountered, currently parsed operand list
        # is split into two possible strings. For example:
        # "<Xdn>{, <pattern>{, MUL #<imm>}}" -> encountered {, MUL #<imm>}
        # without operand: "<Xdn>{, <pattern>}"
        # with operand: "<Xdn>{, <pattern>, MUL #<imm>}"
        opt_wo = arg_str[:beg] + arg_str[i + 1:]
        opt_w = arg_str[:beg] + arg_str[beg + 1:i] + arg_str[i + 1:]
        stack.append((opt_w, i - 1, i_stack.copy()))
        stack.append((opt_wo, beg, i_stack))
      else:
        stack.append((arg_str, i + 1, i_stack))
    else:
      stack.append((arg_str, i + 1, i_stack))

  return opt_set


def GetSignaturesFromHeader():
  with open(ASSEMBLER_HEADER, "r") as fp:
    header_str = fp.read()

  inst_regex = "void (?P<name>[a-z0-9_]+?)\((?P<signature>[^)]*)\)"
  signatures = defaultdict(list)

  # Read all the instructions with signatures from assembler-aarch64.h file
  # into the map. Example of a single map element:
  # ('add', [
  # ['const Register& rd', 'const Register& rn', 'const Operand& operand'],
  # ['const VRegister& vd', 'const VRegister& vn', 'const VRegister& vm'],
  # ... ])
  for match in re.finditer(inst_regex, header_str, re.MULTILINE):
    matches = match.groupdict()
    s_match = matches['signature'].replace("\n", "")
    s_match = re.sub(' +', ' ', s_match)

    # Insert empty array - [], instead of [''] when function takes no
    # arguments.
    arg_arr = s_match.split(',') if s_match else []
    signatures[matches['name']].append(arg_arr)

  return signatures

# Get prototype of the instruction in the reference manual form. Prototype is
# created by matching argument list with regexes from global regex map.
# For example:
# instruction: "ADD  <Vd>.<T>, <Vn>.<T>, <Vm>.<T>" -> prototype: "vvv"
def GetPrototype(arguments):
  match = None
  prototype = ""

  while arguments:
    found = False
    for regex in REGEX_MAP:
      split_re = "\s*({})\s?(,(?=[^$])|$)".format(regex)
      match = re.match(split_re, arguments)

      if match:
        t_value = REGEX_MAP[regex]
        if t_value == "v_list":
          num = 1 if match.group('num') is None else int(match.group('num'))
          prototype = prototype.ljust(num + len(prototype), 'v')
          prototype += '' if match.group('imm') is None else 'i'
        elif t_value == "z_list":
          num = 1 if match.group('num') is None else int(match.group('num'))
          prototype = prototype.ljust(num + len(prototype), 'z')
        else:
          prototype += t_value
        arguments = arguments[match.end():].strip()
        break

    if not match:
      return (True, arguments)

  return (False, prototype)


# Parse file with instructions in reference manual form and create prototypes
# of given instructions. Returns list of instructions and its prototypes in
# given form:
# [('add', 'rro'), ('bfm', 'rrii'), ('cmpp', 'rr')...]
def ParseInputFile(file, eh):
  instructions = []
  input_line = file.readline()

  while input_line:
    instruction_arr = ParseOptionalArgs(input_line.strip())

    for instruction in instruction_arr:
      instruction = instruction.split(' ', maxsplit=1)
      arguments = '' if len(instruction) == 1 else instruction[1].strip()

      # Check if mnemonic contains condition at the end. For example b.eq.
      mnemonic = instruction[0].lower()
      match = re.match("(?P<mnemonic>.*)\.(?P<condition>.*)", mnemonic)

      if match:
        match_dict = match.groupdict()
        mnemonic = match_dict['mnemonic']
        arguments += ", {}".format(match_dict['condition'])

      if mnemonic in I_ALIAS:
        mnemonic = I_ALIAS[mnemonic]

      err, ret_str = GetPrototype(arguments)
      if err:
        eh.PrintError("Couldn't match any regex with argument", mnemonic,
                      description="not matched part: {}".format(ret_str))
      else:
        instructions.append((mnemonic, ret_str))

    input_line = file.readline()

  return instructions

# Check for a match between prototype from reference manual and possible
# prototypes of a function in VIXL.
def CheckPrototype(ref_prototype, prototype_arr):
  # Some types from reference manual are represented in different ways in VIXL.
  # It is needed to match specific type that is used in VIXL for given
  # instruction. For example:
  # - "Register, Shift" can be represented as "Operand"
  # - All types of registers can be represented as "CPURegister"
  operand_types = ["re", "rS", "iS", "r", "i"]
  register_types = ["r", "v", "z", "p", "L", "M", "Z"]
  other_types = [('i', "(d|f(16)?|[ui](?:32|64)?)"),
                 ('__mem__', "(m|s)"),
                 ('L', "(p|L)"),
                 ('M', "(p|M)"),
                 ('Z', "(p|Z)")]

  for prototype in prototype_arr:
    regex_p = ref_prototype

    if "o" in prototype and "o" not in ref_prototype:
      index = prototype.find('o')
      if len(ref_prototype) >= index + 2 and ref_prototype[index:index + 2] in operand_types:
        regex_p = ref_prototype[:index] + "o" + ref_prototype[index + 2:]
      elif len(ref_prototype) >= index + 1 and ref_prototype[index:index + 1] in operand_types:
        regex_p = ref_prototype[:index] + "o" + ref_prototype[index + 1:]

    for ot_type in other_types:
      regex_p = regex_p.replace(ot_type[0], ot_type[1])

    for reg_type in register_types:
      regex_p = regex_p.replace(reg_type, "({}|C)".format(reg_type))

    if bool(re.match("^" + regex_p + "$", prototype)):
      return prototype

  return None

# Iterate through list of instructions from reference manual and check if
# given mnemonic and prototype is supported by VIXL. Instructions that are
# supported are returned in a map.
def GetMatchingInstructions(instructions, signatures, eh):
  prototype_map = {}
  for mnemonic, prototype in instructions:
    t_regex = "((const )?(?P<type>\w+\*?)&? \w+(?P<default> = .*)?)?"
    found = False

    if mnemonic not in signatures:
      eh.PrintError("No matching mnemonic supported by VIXL", mnemonic,
                    description="prototype from reference: {}".format(prototype))
      continue

    # Iterate through array of signatures supported by VIXL for given mnemonic
    # and check if there is one matching.
    for signature in signatures[mnemonic]:
      curr_prototype = ""
      prototype_arr = []
      types_arr = []

      # Create prototype from signature by iterating through arguments list.
      # Prototype is created by extracting type name with regular expression
      # and lookup in the global types map.
      for argument in signature:
        m = re.match(t_regex, argument.strip())

        if m is None:
          eh.PrintError("Couldn't extract argument type: {}".format(argument),
                        mnemonic, description="signature: {}".format(signature))
          continue

        if m.group('type') not in TYPES:
          eh.PrintError("Type was not recognized: {}".format(argument), mnemonic,
                        description="signature: {}".format(signature))
          continue

        # When default arguments are encountered all possible prototypes are
        # written in the array. For example:
        # const Register& rd, int pattern = SVE_ALL, int multiplier = 1
        # prototype_arr = ['r', 'ri', 'rii']
        if m.group('default') is not None:
          prototype_arr.append(curr_prototype)
        types_arr.append(m.group('type'))
        curr_prototype += TYPES.get(m.group('type'))

      prototype_arr.append(curr_prototype)
      # Possible prototypes for given signature are checked for a match with
      # prototype of given instruction from reference manual.
      match_p = CheckPrototype(prototype, prototype_arr)
      if match_p is not None:
        found = True
        f_signature = ','.join(signature[:len(match_p)])
        f_signature = re.sub('\s*=\s*[^,)]*', '', f_signature)

        instructions = prototype_map.get(match_p, InstructionSet(f_signature, types_arr))
        instructions.mnemonics.add(mnemonic)
        prototype_map[match_p] = instructions
        eh.PrintMsg("Instruction added - mnemonic: {}, prototype: {}"
                    .format(mnemonic, prototype), 2)

    if not found:
      eh.PrintError("No matching signature for mnemonic supported by VIXL", mnemonic,
                    description="prototype from reference: {}".format(prototype))

  return prototype_map


if __name__ == '__main__':
  parser = argparse.ArgumentParser(
    description='Add instructions to text assembler.')
  parser.add_argument('filepath', type=argparse.FileType('r'), nargs=1,
                      help='path to a file with instructions to add')
  parser.add_argument('-v', '--verbose', action="count", default=0,
                      help=('print logs to standard output,'
                            ' two verbosity levels available'))
  args = parser.parse_args()
  header_insts = GetSignaturesFromHeader()

  with ErrorHandler(header_insts, args.verbose) as eh:
    eh.PrintMsg('Opening input file - {} and parsing content ...\n'
                 .format(args.filepath[0].name), 1)

    reference_insts = ParseInputFile(args.filepath[0], eh)
    prototype_map = GetMatchingInstructions(reference_insts, header_insts, eh)

    eh.PrintMsg("Opening and reading templates ...", 1)
    eh.PrintMsg("./tasm-template/tasm-assembler.jinja2", 1)
    eh.PrintMsg("./tasm-template/tasm-instructions.jinja2\n", 1)

    assembler_in = open("./tasm-template/tasm-assembler.jinja2", "r")
    instructions_in = open("./tasm-template/tasm-instructions.jinja2", "r")
    assembler_t = Template(assembler_in.read())
    instructions_t = Template(instructions_in.read())

    eh.PrintMsg("Generating C++ headers ...", 1)
    eh.PrintMsg("./tasm-template/tasm-assembler.h", 1)
    eh.PrintMsg("./tasm-template/tasm-instructions.h\n", 1)

    assembler_out = open("./tasm-template/tasm-assembler.h", "w")
    instructions_out = open("./tasm-template/tasm-instructions.h", "w")
    print(assembler_t.render(prototypes=prototype_map), file=assembler_out)
    print(instructions_t.render(prototypes=prototype_map), file=instructions_out)
