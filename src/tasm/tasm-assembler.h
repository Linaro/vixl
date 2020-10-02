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

#ifndef VIXL_TASM_ASSEMBLER_H
#define VIXL_TASM_ASSEMBLER_H

#if __cplusplus < 201703L
#error "Text Assembler requires C++17 standard or above."
#else

#include <iostream>
#include <map>
#include <string>
#include <variant>  // NOLINT(build/include_order)

#include "tasm-parser.h"
#include "aarch64/assembler-aarch64.h"

namespace vixl {
namespace aarch64 {
namespace tasm {

class TextAssembler;

using VIXL_LpL_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize &,
                                            const PRegister &,
                                            const PRegisterWithLaneSize &);
using VIXL_LZL_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize &,
                                            const PRegisterZ &,
                                            const PRegisterWithLaneSize &);
using VIXL_Lrr_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize &,
                                            const Register &,
                                            const Register &);
using VIXL_vvv_Fn = void (TextAssembler::*)(const VRegister &,
                                            const VRegister &,
                                            const VRegister &);
using VIXL_rro_Fn = void (TextAssembler::*)(const Register &,
                                            const Register &,
                                            const Operand &);
using VIXL_rrm_Fn = void (TextAssembler::*)(const Register &,
                                            const Register &,
                                            const MemOperand &);
using VIXL_rrr_Fn = void (TextAssembler::*)(const Register &,
                                            const Register &,
                                            const Register &);
using VIXL_zZs_Fn = void (TextAssembler::*)(const ZRegister &,
                                            const PRegisterZ &,
                                            const SVEMemOperand &);
using VIXL_zps_Fn = void (TextAssembler::*)(const ZRegister &,
                                            const PRegister &,
                                            const SVEMemOperand &);
using VIXL_Lc_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize &, int);
using VIXL_ru_Fn = void (TextAssembler::*)(const Register &, uint64_t);
using VIXL_ro_Fn = void (TextAssembler::*)(const Register &, const Operand &);
using VIXL_zr_Fn = void (TextAssembler::*)(const ZRegister &, const Register &);
using VIXL_L_Fn = void (TextAssembler::*)(const PRegisterWithLaneSize &);
using VIXL_r_Fn = void (TextAssembler::*)(const Register &);
using VIXL_Fn = void (TextAssembler::*)();

using AssemblerFn = std::variant<std::monostate,
                                 VIXL_LpL_Fn,
                                 VIXL_LZL_Fn,
                                 VIXL_Lrr_Fn,
                                 VIXL_rro_Fn,
                                 VIXL_rrr_Fn,
                                 VIXL_vvv_Fn,
                                 VIXL_rrm_Fn,
                                 VIXL_zZs_Fn,
                                 VIXL_zps_Fn,
                                 VIXL_ru_Fn,
                                 VIXL_ro_Fn,
                                 VIXL_Lc_Fn,
                                 VIXL_zr_Fn,
                                 VIXL_L_Fn,
                                 VIXL_r_Fn,
                                 VIXL_Fn>;

class TextAssembler : private Assembler {
 public:
  TextAssembler() : ip_(MnemonicExists, GetPrototypes) {}
  void Assemble(std::string instruction);

  // Functions allowing instruction prototype/mnemonic lookup.
  static bool MnemonicExists(std::string prototype, std::string mnemonic);
  static std::vector<std::string> GetPrototypes();

  // Necessary methods to expose some functionality of Assembler base class.
  using Assembler::GetBuffer;
  using Assembler::FinalizeCode;
  using Assembler::GetCPUFeatures;
  vixl::internal::AssemblerBase *AsAssemblerBase();

 private:
  InstructionParser ip_;
  static const std::vector<std::string> str_prototypes_;

  // Maps have to be members if we want to have private inheritance from
  // Assembler class.
  static const std::map<std::string, AssemblerFn> LpL_insts_;
  static const std::map<std::string, AssemblerFn> LZL_insts_;
  static const std::map<std::string, AssemblerFn> Lrr_insts_;
  static const std::map<std::string, AssemblerFn> rro_insts_;
  static const std::map<std::string, AssemblerFn> vvv_insts_;
  static const std::map<std::string, AssemblerFn> rrr_insts_;
  static const std::map<std::string, AssemblerFn> zZs_insts_;
  static const std::map<std::string, AssemblerFn> zps_insts_;
  static const std::map<std::string, AssemblerFn> Lc_insts_;
  static const std::map<std::string, AssemblerFn> ru_insts_;
  static const std::map<std::string, AssemblerFn> ro_insts_;
  static const std::map<std::string, AssemblerFn> zr_insts_;
  static const std::map<std::string, AssemblerFn> L_insts_;
  static const std::map<std::string, AssemblerFn> r_insts_;
  static const std::map<std::string, AssemblerFn> _insts_;
  static const std::map<std::string, std::map<std::string, AssemblerFn> >
      prototypes_;


  // TODO: here will be more methods invoking Assembler class methods with
  // different signatures.
  using Assembler::movz;
  void movz(const Register &rd, uint64_t imm);

  using Assembler::ptrue;
  void ptrue(const PRegisterWithLaneSize &pd);

  using Assembler::ptrues;
  void ptrues(const PRegisterWithLaneSize &pd);

  using Assembler::ret;
  void ret();
};

// Class used in std::visit to get current value held by std::variant.
class InstructionDispatcher {
 public:
  void operator()(std::monostate v) {
    std::cout << "No function with given signature !" << std::endl;
    (void)v;
  }
  void operator()(VIXL_LpL_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<PRegisterWithLaneSize>(args[2]));
  }
  void operator()(VIXL_LZL_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<PRegisterZ>(args[1]),
               std::get<PRegisterWithLaneSize>(args[2]));
  }
  void operator()(VIXL_Lrr_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<Register>(args[1]),
               std::get<Register>(args[2]));
  }
  void operator()(VIXL_vvv_Fn f) {
    (tasm->*f)(std::get<VRegister>(args[0]),
               std::get<VRegister>(args[1]),
               std::get<VRegister>(args[2]));
  }
  void operator()(VIXL_rrr_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<Register>(args[2]));
  }
  void operator()(VIXL_rro_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<Operand>(args[2]));
  }
  void operator()(VIXL_rrm_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]),
               std::get<Register>(args[1]),
               std::get<MemOperand>(args[2]));
  }
  void operator()(VIXL_zZs_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegisterZ>(args[1]),
               std::get<SVEMemOperand>(args[2]));
  }
  void operator()(VIXL_zps_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]),
               std::get<PRegister>(args[1]),
               std::get<SVEMemOperand>(args[2]));
  }
  void operator()(VIXL_ru_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]), std::get<uint64_t>(args[1]));
  }
  void operator()(VIXL_Lc_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]),
               std::get<int>(args[1]));
  }
  void operator()(VIXL_ro_Fn f) {
    (tasm->*f)(std::get<Register>(args[0]), std::get<Operand>(args[1]));
  }
  void operator()(VIXL_zr_Fn f) {
    (tasm->*f)(std::get<ZRegister>(args[0]), std::get<Register>(args[1]));
  }
  void operator()(VIXL_L_Fn f) {
    (tasm->*f)(std::get<PRegisterWithLaneSize>(args[0]));
  }
  void operator()(VIXL_r_Fn f) { (tasm->*f)(std::get<Register>(args[0])); }
  void operator()(VIXL_Fn f) { (tasm->*f)(); }

  InstructionDispatcher(TextAssembler *tasm, std::vector<Argument> args)
      : tasm(tasm), args(args) {}

 private:
  TextAssembler *tasm;
  std::vector<Argument> args;
};
}
}
}  // namespace vixl::aarch64::tasm

#endif  // __cplusplus >= 201703L
#endif  // VIXL_TASM_ASSEMBLER_H