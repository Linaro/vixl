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
import subprocess
import pathlib
import argparse
import re
import os
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
#   "const ZRegister& zd, const ZRegister& zn, const ZRegister& zm" ->
#   prototype: "zzz"
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
  "<Zm>\.[BSDH]\[<imm>\]": ["z", "i"],
  "<Zdn>\.<T>": "z",
  "<Zn>\.<T>\[<imm>\]": ["z", "i"],
  "\{ <Z[tn](1)?>\.(?:[HSBD]|<T>)(, <Z[tn](?P<num>[234])?>"
    "\.(?:[HSBD]|<T>))* \}": "z_list",
  # PREGISTERS
  "<Pg>/M": "M",
  "<Pg>/Z": "Z",
  "<Pg>/<ZM>": "pZM",
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
  "<V[nmd]>\.<Ts?>\[<index[12]?>\]": ["v", "i"],
  "<V[nm]>\.4?[HBSD]\[<index>\]": ["v", "i"],
  "<[SDQ]t[12]>": "v",
  "\{ <Vt>\.<T>(, <Vt(?P<num>[234])?>\.<T>)* \}(?P<imm>\[<index>\])?":
    "v_list",
  "\{ <V[tn]>\.[0-9]{0,2}[HSBD](, <V[tn]\+?(?P<num>[1234])>"
    "\.[0-9]{0,2}[HSBD])* \}(?P<imm>\[<index>\])?": "v_list",
  # IMMEDIATES
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
  "LSL #0": ["S", "i"],
  "MSL #<amount>": ["S", "i"],
  "LSL #<amount>": ["S?", "i"],
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
  # Instructions taking named labels accept also syntax with immediate offset
  # For example: b #+8
  "<label>": "(l|i)",
  "<extend>": "e",
  "<extend> #<amount>": "e",
  "LSL|ROR|<shift> #<amount>": "S",
  "<cond>|invert\(<cond>\)": "c",
  "<option>\|#<imm>": ["B", "b"],
  "<option>nXS\|#<imm>": ["B", "b"],
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

# Some of the subtypes are not accepted as a base type argument for some
# signatures in VIXL functions. This cases have to be checked to prevent
# from causing possible bugs in Text Assembler. For example:
#  - ldr(const CPURegister& rt, const SVEMemOperand& addr)
#  - ldr(const CPURegister& rt, const MemOperand& src)
# In such case with overloaded functions Text Assembler will decide how to
# parse memory operand (MemOperand or SVEMemOperand) based on the preceding
# register type.
BASE_TYPE_EXCEPTIONS = [
  ("const CPURegister& [\w]+, const SVEMemOperand& [\w]+",
    {'base': "CPURegister", 'derived': ["Register", "VRegister"]}),
  ("const CPURegister& [\w]+, const MemOperand& [\w]+, "
   "LoadStoreScalingOption [\w]+ = PreferScaledOffset",
    {'base': "CPURegister", 'derived': ["PRegister", "ZRegister"]})
]

# Dictionary with matching instructions names that are different in VIXL and
# reference manual.
I_ALIAS = {
  "and" : "and_",
  "not" : "not_"
}

VERBOSITY_LOW = 1
VERBOSITY_HIGH = 2
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
    print("{} about instructions supported by VIXL."
          .format(self.general_err_num),
          file=sys.stderr)
    print("Located in file {}\n".format(self.general_err_fn), file=sys.stderr)
    print("{} about instructions not supported by VIXL."
          .format(self.support_err_num),
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


# Class containing instructions and functions that will be added in generated
# Text Assembler headers. Map members in this class are passed directly to
# jinja template files.
class ParsedInstructionSet(object):
  def __init__(self):
    self.instructions_count = 0
    self.instruction_map = {}
    self.function_map = {}

  # Subclass containing single signature, array with type names and array with
  # mnemonics that are handled with this signature. This class is used for
  # more readability in jinja templates. Members can be accessed by names
  # instead of indexes.
  class MnemonicSet(object):
    def __init__(self, signature, types):
      self.signature = signature
      self.types = types
      self.mnemonics = set()

  def StripDefaultValues(self, signature):
    # Strip default values from default arguments in the function signature.
    # For example:
    #  - cntw(const Register& rd, int pattern = SVE_ALL, int multiplier = 1)
    #  - cntw(const Register& rd, int pattern, int multiplier)
    return re.sub('\s*=\s*[^,)]*', '', signature)

  def InsertInstruction(self, mnemonic, signature, types_arr, prototype):
    signature = self.StripDefaultValues(signature)
    self.instructions_count += 1
    str_prototype = ''.join(prototype)
    instructions = self.instruction_map.get(str_prototype,
                                            self.MnemonicSet(signature,
                                                                types_arr))
    instructions.mnemonics.add(mnemonic)
    self.instruction_map[str_prototype] = instructions

  def InsertFunction(self, mnemonic, signature, names):
    signature = self.StripDefaultValues(signature)
    arguments = self.function_map.get(mnemonic, set())
    arguments.add((signature, tuple(names)))
    self.function_map[mnemonic] = arguments

  def InstructionsCount(self):
    return self.instructions_count


class Mnemonic(object):
  def __init__(self, ref_name, hdr_name=None):
    self.hdr_name = ref_name if hdr_name is None else hdr_name
    self.ref_name = ref_name

  # Mnemonic class is used as a key in a set data type. Functions __hash__ and
  # __eq__ are implemented for proper comparision between instances of a class.
  def __hash__(self):
    return hash((self.ref_name, self.hdr_name))

  def __eq__(self, other):
    if not isinstance(other, Mnemonic):
      return False

    return (self.hdr_name == self.hdr_name and
            self.ref_name == self.ref_name)


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
# instruction: "ADD  <Vd>.<T>, <Vn>.<T>, <Vm>.<T>" -> prototype: ["v", "v", "v"]
def GetPrototype(arguments, mnemonic):
  match = None
  # Some instructions can take both PRegisterM and PRegisterZ arguments and
  # they have common function in VIXL, for example:
  #   - movprfx(const ZRegister& zd, const PRegister& pg, const ZRegister& zn)
  # Some of these arguments are marked as <Pg>/<ZM> in the reference.
  # In this cases two separate prototypes are returned instead of one.
  # Example for movprfx case:
  #   - ("zMz", "zZz") instead of "zPz"
  split_index = None
  prototype = []

  while arguments:
    found = False
    for regex in REGEX_MAP:
      split_re = "\s*({})\s?(,(?=[^$])|$)".format(regex)
      match = re.match(split_re, arguments)

      if match:
        t_value = REGEX_MAP[regex]
        if t_value == "v_list":
          num = 1 if match.group('num') is None else int(match.group('num'))
          prototype.extend(['v'] * num)
          prototype += '' if match.group('imm') is None else 'i'
        elif t_value == "z_list":
          num = 1 if match.group('num') is None else int(match.group('num'))
          prototype.extend(['z'] * num)
        elif t_value == "pZM" and split_index is None:
          split_index = len(prototype)
        else:
          prototype += t_value if isinstance(t_value, list) else [t_value]
        arguments = arguments[match.end():].strip()
        break

    if not match:
      return (True, arguments)

  if split_index:
    m_prototype = prototype[split_index:] + ["M"] + prototype[:split_index]
    z_prototype = prototype[split_index:] + ["Z"] + prototype[:split_index]
    return (False, [m_prototype, z_prototype])
  else:
    return (False, [prototype])


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

      err, ret_arr = GetPrototype(arguments, mnemonic)
      if err:
        eh.PrintError("Couldn't match any regex with argument", mnemonic,
                      description="not matched part: {}".format(ret_arr))
      else:
        for prototype in ret_arr:
          instructions.append((Mnemonic(mnemonic), prototype))

    input_line = file.readline()

  return instructions

# Check for a match between prototype from reference manual and possible
# prototypes of a function in VIXL.
def CheckPrototype(ref_prototype, hdr_prototype):
  # Some types from reference manual are represented in different ways in VIXL.
  # It is needed to match specific type that is used in VIXL for given
  # instruction. For example:
  # - "Register, Shift" can be represented as "Operand"
  regex_p = ''.join(ref_prototype)
  hdr_str = ''.join(hdr_prototype)
  operand_types = ["ii", "re", "rS", "iS", "r", "i"]
  other_types = [('i', "(d|f(16)?|[ui](?:32|64)?)"),
                 ('__mem__', "(m|s)")]

  if "o" in hdr_str and "o" not in regex_p:
    index = hdr_str.find('o')
    if len(regex_p) >= index + 2 and regex_p[index:index + 2] in operand_types:
      regex_p = regex_p[:index] + "o" + regex_p[index + 2:]
    elif (len(regex_p) >= index + 1 and
          regex_p[index:index + 1] in operand_types):
      regex_p = regex_p[:index] + "o" + regex_p[index + 1:]

  for ot_type in other_types:
    regex_p = regex_p.replace(ot_type[0], ot_type[1])

  if bool(re.match("^" + regex_p + "$", hdr_str)):
    return True

  return False


# Check if the type for a given argument from VIXL header is a base type for
# the one found in the xml reference. For example:
#   - PRegister is a base type for PRegisterM and PRegisterZ
# Return full signature for the subtype from xml.
def CheckBaseTypes(ref_prototype, index, name_str, types_arr, type_exception):
  derived_type = None
  base_types = {"PRegister": ["PRegisterM", "PRegisterZ"],
                "CPURegister": ["Register", "VRegister",
                                "ZRegister", "PRegister",
                                "PRegisterM", "PRegisterZ",
                                "PRegisterWithLaneSize"]}
  opt_types = base_types.get(types_arr[-1])

  if opt_types is None or index > len(ref_prototype):
    return None

  ref_type = ref_prototype[index]

  for opt_type in opt_types:
    if TYPES.get(opt_type) == ref_type:
      derived_type = opt_type
      break

  if (type_exception is not None and
      types_arr[-1] == type_exception['base'] and
      derived_type in type_exception['derived']):
    return None

  if derived_type is not None:
    type_def = "const {}& {}".format(derived_type, name_str)
    types_arr[-1] = derived_type
    return type_def

  return None


# Iterate through list of instructions from reference manual and check if
# given mnemonic and prototype is supported by VIXL. Instructions that are
# supported are returned in a map.
def GetMatchingInstructions(instructions, signatures, eh):
  parsed_insts = ParsedInstructionSet()

  for mnemonic, prototype in instructions:
    init_size = parsed_insts.InstructionsCount()
    t_regex = "((const )?(?P<type>\w+\*?)&? (?P<name>\w+)(?P<default> = .*)?)?"

    if mnemonic.ref_name in I_ALIAS:
      mnemonic.hdr_name = I_ALIAS[mnemonic.ref_name]

    if mnemonic.hdr_name not in signatures:
      eh.PrintError("No matching mnemonic supported by VIXL",
                    mnemonic.ref_name,
                    description="prototype from reference: {}"
                    .format(prototype))
      continue

    # Iterate through array of signatures supported by VIXL for given mnemonic
    # and check if there is one matching.
    for signature in signatures[mnemonic.hdr_name]:
      curr_signature = signature.copy()
      generate_func = False
      curr_prototype = []
      types_arr = []
      names_arr = []

      # Create prototype from signature by iterating through arguments list.
      # Prototype is created by extracting type name with regular expression
      # and lookup in the global types map.
      for index, argument in enumerate(signature):
        m = re.match(t_regex, argument.strip())

        if m is None:
          eh.PrintError("Couldn't extract argument type: {}".format(argument),
                        mnemonic.ref_name,
                        description="signature: {}".format(signature))
          continue

        if m.group('type') not in TYPES:
          eh.PrintError("Type was not recognized: {}".format(argument),
                        mnemonic.ref_name,
                        description="signature: {}".format(signature))
          continue

        # Text Assembler refers to functions by pointers and in C++ it is not
        # possible to omit default arguments in such case. When default
        # argument is encountered additional function is generated in the
        # Text Assembler header file. For example:
        #   - void clrex(int imm4 = 0xf);
        # For above function additional one will be generated:
        #   - void clrex()
        if (m.group('default') is not None
            and CheckPrototype(prototype, curr_prototype)):
          parsed_insts.InsertInstruction(mnemonic,
                                          ','.join(curr_signature[:index]),
                                          types_arr[:index],
                                          curr_prototype)
          parsed_insts.InsertFunction(mnemonic,
                                       ','.join(curr_signature[:index]),
                                       names_arr[:index])
          eh.PrintMsg("Instruction added - mnemonic: {}, prototype: {}\n"
                      "Truncated to default argument: {}"
                      .format(mnemonic.ref_name,
                              ''.join(curr_prototype),
                              curr_signature[index]), VERBOSITY_HIGH)

        names_arr.append(m.group('name'))
        types_arr.append(m.group('type'))
        type_exception = None

        for e_signature, e_types in BASE_TYPE_EXCEPTIONS:
          if bool(re.match(e_signature, ','.join(signature))):
            type_exception = e_types
            break

        # Check if currently parsed type from VIXL header is a base type for
        # the type found in the xml. Text Assembler refers to arguments by
        # exact type and additional functions are generated in its header file.
        # For example:
        #  - movprfx(const ZRegister& zd, const PRegister& pg,...
        # For the above function two others will be generated:
        #  - movprfx(const ZRegister& zd, const PRegisterM& pg,...
        #  - movprfx(const ZRegister& zd, const PRegisterZ& pg,...
        type_def = CheckBaseTypes(prototype, index, m.group('name'),
                                  types_arr, type_exception)

        if type_def is not None:
          generate_func = True
          curr_signature[index] = type_def

        curr_prototype.append(TYPES.get(types_arr[-1]))

      # Prototype constructed from the function signature is checked for a
      # match with the prototype of a given instruction from reference manual.
      if CheckPrototype(prototype, curr_prototype):
        parsed_insts.InsertInstruction(mnemonic,
                                        ','.join(curr_signature),
                                        types_arr,
                                        curr_prototype)
        if generate_func:
          parsed_insts.InsertFunction(mnemonic,
                                       ','.join(curr_signature),
                                       names_arr)

        eh.PrintMsg("Instruction added - mnemonic: {}, prototype: {}"
                    .format(mnemonic.ref_name, curr_prototype), VERBOSITY_HIGH)

    if init_size == parsed_insts.InstructionsCount():
      eh.PrintError("No matching signature for mnemonic supported by VIXL",
                    mnemonic.ref_name,
                    description="prototype from reference: {}"
                    .format(prototype))

  return parsed_insts


if __name__ == '__main__':
  parser = argparse.ArgumentParser(
    description='Add instructions to text assembler.')
  parser.add_argument('filepath', type=argparse.FileType('r'), nargs=1,
                      help='path to a file with instructions to add')
  parser.add_argument('-v', '--verbose', action="count", default=0,
                      help=('print logs to standard output,'
                            ' two verbosity levels available'))
  parser.add_argument('--clang-format', default="clang-format-4.0",
                      help='path to clang-format, by default clang-format-4.0'
                           ' is invoked.')

  args = parser.parse_args()
  header_insts = GetSignaturesFromHeader()

  with ErrorHandler(header_insts, args.verbose) as eh:
    eh.PrintMsg('Opening input file - {} and parsing content ...\n'
                 .format(args.filepath[0].name), VERBOSITY_LOW)

    reference_insts = ParseInputFile(args.filepath[0], eh)
    parsed_insts = GetMatchingInstructions(reference_insts, header_insts, eh)

    # Parse all files with .jinja2 extension from the ./tasm-template
    # subdirectory and produce Text Assembler header files in the same
    # directory.
    for template_file in pathlib.Path('tasm-template/').glob('*.jinja2'):
      eh.PrintMsg("Opening and reading template ...", VERBOSITY_LOW)
      eh.PrintMsg("{}\n".format(template_file), VERBOSITY_LOW)
      file_t = Template(open(template_file, "r").read())

      header_file = os.path.splitext(template_file)[0] + ".h"
      eh.PrintMsg("Generating C++ header ...", VERBOSITY_LOW)
      eh.PrintMsg("{}\n".format(header_file), VERBOSITY_LOW)
      file_out = open(header_file, "w")

      # Invoke clang-format on generated header files.
      eh.PrintMsg("Invoking clang-format on a generated file...\n",
                  VERBOSITY_LOW)
      proc = subprocess.Popen([args.clang_format], stdin=subprocess.PIPE,
                          stdout=subprocess.PIPE)
      out, _ = proc.communicate(
                file_t.render(prototypes=parsed_insts.instruction_map,
                              functions=parsed_insts.function_map).encode())
      file_out.write(out.decode())