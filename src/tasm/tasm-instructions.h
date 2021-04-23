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

#ifndef VIXL_TASM_INSTRUCTIONS_H
#define VIXL_TASM_INSTRUCTIONS_H

#include "tasm-assembler.h"

namespace vixl {
namespace aarch64 {
namespace tasm {

// TODO: Is this macro a good idea ? Maybe some other solution.
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LZzi_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::LZzi_insts_ =
    {{"cmple", VIXL_AF(&TextAssembler::cmple)},
     {"cmpeq", VIXL_AF(&TextAssembler::cmpeq)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_Lc_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::Lc_insts_ =
    {{"ptrue", VIXL_AF(&TextAssembler::ptrue)},
     {"ptrues", VIXL_AF(&TextAssembler::ptrues)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvim_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::vvvim_insts_ =
    {{"ld3", VIXL_AF(&TextAssembler::ld3)},
     {"st3", VIXL_AF(&TextAssembler::st3)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvm_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::vvvm_insts_ =
    {{"ld1", VIXL_AF(&TextAssembler::ld1)},
     {"ld3", VIXL_AF(&TextAssembler::ld3)},
     {"ld3r", VIXL_AF(&TextAssembler::ld3r)},
     {"st3", VIXL_AF(&TextAssembler::st3)},
     {"st1", VIXL_AF(&TextAssembler::st1)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvvm_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::vvvvm_insts_ =
    {{"ld4", VIXL_AF(&TextAssembler::ld4)},
     {"ld4r", VIXL_AF(&TextAssembler::ld4r)},
     {"st4", VIXL_AF(&TextAssembler::st4)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LpL_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::LpL_insts_ = {
    {"brka", VIXL_AF(&TextAssembler::brka)},
    {"brkb", VIXL_AF(&TextAssembler::brkb)},
};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LZL_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::LZL_insts_ = {
    {"not", VIXL_AF(&TextAssembler::not_)},
};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_Lrr_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::Lrr_insts_ =
    {{"whilele", VIXL_AF(&TextAssembler::whilele)},
     {"whilelo", VIXL_AF(&TextAssembler::whilelo)},
     {"whilels", VIXL_AF(&TextAssembler::whilels)},
     {"whilelt", VIXL_AF(&TextAssembler::whilelt)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rro_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rro_insts_ = {
    {"add", VIXL_AF(&TextAssembler::add)},
    {"sub", VIXL_AF(&TextAssembler::sub)},
    {"adds", VIXL_AF(&TextAssembler::adds)},
    {"orr", VIXL_AF(&TextAssembler::orr)},
};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvv_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::vvv_insts_ =
    {{"add", VIXL_AF(&TextAssembler::add)},
     {"addhn", VIXL_AF(&TextAssembler::addhn)},
     {"addhn2", VIXL_AF(&TextAssembler::addhn2)},
     {"addp", VIXL_AF(&TextAssembler::addp)},
     {"and", VIXL_AF(&TextAssembler::and_)},
     {"zip1", VIXL_AF(&TextAssembler::zip1)},
     {"zip2", VIXL_AF(&TextAssembler::zip2)},
     {"bic", VIXL_AF(&TextAssembler::bic)},
     {"bif", VIXL_AF(&TextAssembler::bif)},
     {"bit", VIXL_AF(&TextAssembler::bit)},
     {"bsl", VIXL_AF(&TextAssembler::bsl)},
     {"cmeq", VIXL_AF(&TextAssembler::cmeq)},
     {"cmge", VIXL_AF(&TextAssembler::cmge)},
     {"cmgt", VIXL_AF(&TextAssembler::cmgt)},
     {"cmhi", VIXL_AF(&TextAssembler::cmhi)},
     {"cmhs", VIXL_AF(&TextAssembler::cmhs)},
     {"cmtst", VIXL_AF(&TextAssembler::cmtst)},
     {"eor", VIXL_AF(&TextAssembler::eor)},
     {"fabd", VIXL_AF(&TextAssembler::fabd)},
     {"facge", VIXL_AF(&TextAssembler::facge)},
     {"facgt", VIXL_AF(&TextAssembler::facge)},
     {"fadd", VIXL_AF(&TextAssembler::fadd)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rrr_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::rrr_insts_ =
    {{"sdiv", VIXL_AF(&TextAssembler::sdiv)},
     {"asr", VIXL_AF(&TextAssembler::asrv)},
     {"crc32b", VIXL_AF(&TextAssembler::crc32b)},
     {"mul", VIXL_AF(&TextAssembler::mul)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zZs_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::zZs_insts_ =
    {{"ld1b", VIXL_AF(&TextAssembler::ld1b)},
     {"ld1h", VIXL_AF(&TextAssembler::ld1h)},
     {"ld1w", VIXL_AF(&TextAssembler::ld1w)},
     {"ld1d", VIXL_AF(&TextAssembler::ld1d)},
     {"ld1rb", VIXL_AF(&TextAssembler::ld1rb)},
     {"ld1rh", VIXL_AF(&TextAssembler::ld1rh)},
     {"ld1rw", VIXL_AF(&TextAssembler::ld1rw)},
     {"ld1rd", VIXL_AF(&TextAssembler::ld1rd)},
     {"ld1rqb", VIXL_AF(&TextAssembler::ld1rqb)},
     {"ld1rqh", VIXL_AF(&TextAssembler::ld1rqh)},
     {"ld1rqw", VIXL_AF(&TextAssembler::ld1rqw)},
     {"ld1rqd", VIXL_AF(&TextAssembler::ld1rqd)},
     {"ld1rsb", VIXL_AF(&TextAssembler::ld1rsb)},
     {"ld1rsh", VIXL_AF(&TextAssembler::ld1rsh)},
     {"ld1rsw", VIXL_AF(&TextAssembler::ld1rsw)},
     {"ld1sb", VIXL_AF(&TextAssembler::ld1sb)},
     {"ld1sh", VIXL_AF(&TextAssembler::ld1sh)},
     {"ld1sw", VIXL_AF(&TextAssembler::ld1sw)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zps_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::zps_insts_ =
    {{"st1b", VIXL_AF(&TextAssembler::st1b)},
     {"st1h", VIXL_AF(&TextAssembler::st1h)},
     {"st1w", VIXL_AF(&TextAssembler::st1w)},
     {"st1d", VIXL_AF(&TextAssembler::st1d)},
     {"stnt1b", VIXL_AF(&TextAssembler::stnt1b)},
     {"stnt1h", VIXL_AF(&TextAssembler::stnt1h)},
     {"stnt1w", VIXL_AF(&TextAssembler::stnt1w)},
     {"stnt1d", VIXL_AF(&TextAssembler::stnt1d)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_i64C_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::i64C_insts_ = {
    {"b", VIXL_AF(&TextAssembler::b)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_ru64_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::ru64_insts_ =
    {{"movz", VIXL_AF(&TextAssembler::movz)},
     {"mov", VIXL_AF(&TextAssembler::mov)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rl_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::rl_insts_ = {
    {"cbnz", VIXL_AF(&TextAssembler::cbnz)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_ro_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::ro_insts_ =
    {{"mvn", VIXL_AF(&TextAssembler::mvn)},
     {"cmp", VIXL_AF(&TextAssembler::cmp)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vu64_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::vu64_insts_ = {
    {"movi", VIXL_AF(&TextAssembler::movi)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zr_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::zr_insts_ = {
    {"mov", VIXL_AF(&TextAssembler::mov)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_i64_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::i64_insts_ = {
    {"b", VIXL_AF(&TextAssembler::b)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_L_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::L_insts_ =
    {{"ptrue", VIXL_AF(&TextAssembler::ptrue)},
     {"ptrues", VIXL_AF(&TextAssembler::ptrues)},
     {"rdffr", VIXL_AF(&TextAssembler::rdffr)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_l_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::l_insts_ = {
    {"b", VIXL_AF(&TextAssembler::b)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_r_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::r_insts_ = {
    {"ret", VIXL_AF(&TextAssembler::ret)}};

#undef VIXL_AF
#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_Fn>(F))

const std::map<std::string, AssemblerFn> TextAssembler::_insts_ =
    {{"ret", VIXL_AF(&TextAssembler::ret)},
     {"setffr", VIXL_AF(&TextAssembler::setffr)}};

#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn()

// Instruction prototype is a signature of the function handling that
// instruction, for example
//  * rro -> Register, Register, Operand
//  * vvv-> VRegister, VRegister, VRegister
//  * ru -> Register, uint64_t
const std::map<std::string, std::map<std::string, AssemblerFn> >
    TextAssembler::prototypes_ =
        {{"vvvim", vvvim_insts_}, {"vvvvm", vvvvm_insts_},
         {"vvvm", vvvm_insts_},   {"LZzi", LZzi_insts_},
         {"LZL", LZL_insts_},     {"LpL", LpL_insts_},
         {"Lrr", Lrr_insts_},     {"rro", rro_insts_},
         {"rrr", rrr_insts_},     {"vvv", vvv_insts_},
         {"zps", zps_insts_},     {"zZs", zZs_insts_},
         {"Lc", Lc_insts_},       {"ru64", ru64_insts_},
         {"ro", ro_insts_},       {"vu64", vu64_insts_},
         {"zr", zr_insts_},       {"L", L_insts_},
         {"l", l_insts_},         {"r", r_insts_},
         {"", _insts_},           {"rl", rl_insts_},
         {"i64C", i64C_insts_}};
}
}
}  // namespace vixl::aarch64::tasm

#endif  // VIXL_TASM_INSTRUCTIONS_H