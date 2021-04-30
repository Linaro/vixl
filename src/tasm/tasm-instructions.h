// Copyright 2021, VIXL authors
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

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vv_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vv_insts_ = {
    {"fcvtxn2", VIXL_AF(&TextAssembler::fcvtxn2)},
    {"neg", VIXL_AF(&TextAssembler::neg)},
    {"uxtl", VIXL_AF(&TextAssembler::uxtl)},
    {"xtn", VIXL_AF(&TextAssembler::xtn)},
    {"fmaxnmv", VIXL_AF(&TextAssembler::fmaxnmv)},
    {"sqneg", VIXL_AF(&TextAssembler::sqneg)},
    {"frint32z", VIXL_AF(&TextAssembler::frint32z)},
    {"mvn", VIXL_AF(&TextAssembler::mvn)},
    {"frintn", VIXL_AF(&TextAssembler::frintn)},
    {"sminv", VIXL_AF(&TextAssembler::sminv)},
    {"frecpe", VIXL_AF(&TextAssembler::frecpe)},
    {"fcvtmu", VIXL_AF(&TextAssembler::fcvtmu)},
    {"rev64", VIXL_AF(&TextAssembler::rev64)},
    {"fminv", VIXL_AF(&TextAssembler::fminv)},
    {"uaddlv", VIXL_AF(&TextAssembler::uaddlv)},
    {"uadalp", VIXL_AF(&TextAssembler::uadalp)},
    {"sqxtun2", VIXL_AF(&TextAssembler::sqxtun2)},
    {"fcvtxn", VIXL_AF(&TextAssembler::fcvtxn)},
    {"fmaxnmp", VIXL_AF(&TextAssembler::fmaxnmp)},
    {"cls", VIXL_AF(&TextAssembler::cls)},
    {"rbit", VIXL_AF(&TextAssembler::rbit)},
    {"uqxtn", VIXL_AF(&TextAssembler::uqxtn)},
    {"fminp", VIXL_AF(&TextAssembler::fminp)},
    {"ursqrte", VIXL_AF(&TextAssembler::ursqrte)},
    {"fmaxv", VIXL_AF(&TextAssembler::fmaxv)},
    {"uaddlp", VIXL_AF(&TextAssembler::uaddlp)},
    {"fabs", VIXL_AF(&TextAssembler::fabs)},
    {"fcvtpu", VIXL_AF(&TextAssembler::fcvtpu)},
    {"sqabs", VIXL_AF(&TextAssembler::sqabs)},
    {"urecpe", VIXL_AF(&TextAssembler::urecpe)},
    {"abs", VIXL_AF(&TextAssembler::abs)},
    {"frinti", VIXL_AF(&TextAssembler::frinti)},
    {"fcvtn2", VIXL_AF(&TextAssembler::fcvtn2)},
    {"fcvtps", VIXL_AF(&TextAssembler::fcvtps)},
    {"rev32", VIXL_AF(&TextAssembler::rev32)},
    {"fcmpe", VIXL_AF(&TextAssembler::fcmpe)},
    {"frinta", VIXL_AF(&TextAssembler::frinta)},
    {"ucvtf", VIXL_AF(&TextAssembler::ucvtf)},
    {"fcvt", VIXL_AF(&TextAssembler::fcvt)},
    {"fcvtzs", VIXL_AF(&TextAssembler::fcvtzs)},
    {"fneg", VIXL_AF(&TextAssembler::fneg)},
    {"not", VIXL_AF(&TextAssembler::not_)},
    {"rev16", VIXL_AF(&TextAssembler::rev16)},
    {"usqadd", VIXL_AF(&TextAssembler::usqadd)},
    {"umaxv", VIXL_AF(&TextAssembler::umaxv)},
    {"uxtl2", VIXL_AF(&TextAssembler::uxtl2)},
    {"saddlv", VIXL_AF(&TextAssembler::saddlv)},
    {"fcvtn", VIXL_AF(&TextAssembler::fcvtn)},
    {"uminv", VIXL_AF(&TextAssembler::uminv)},
    {"frint64z", VIXL_AF(&TextAssembler::frint64z)},
    {"fminnmp", VIXL_AF(&TextAssembler::fminnmp)},
    {"frintz", VIXL_AF(&TextAssembler::frintz)},
    {"fminnmv", VIXL_AF(&TextAssembler::fminnmv)},
    {"smaxv", VIXL_AF(&TextAssembler::smaxv)},
    {"frint64x", VIXL_AF(&TextAssembler::frint64x)},
    {"sqxtn2", VIXL_AF(&TextAssembler::sqxtn2)},
    {"fcvtau", VIXL_AF(&TextAssembler::fcvtau)},
    {"fcvtnu", VIXL_AF(&TextAssembler::fcvtnu)},
    {"fcvtzu", VIXL_AF(&TextAssembler::fcvtzu)},
    {"fcvtl2", VIXL_AF(&TextAssembler::fcvtl2)},
    {"xtn2", VIXL_AF(&TextAssembler::xtn2)},
    {"addp", VIXL_AF(&TextAssembler::addp)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
    {"frecpx", VIXL_AF(&TextAssembler::frecpx)},
    {"fcvtl", VIXL_AF(&TextAssembler::fcvtl)},
    {"frsqrte", VIXL_AF(&TextAssembler::frsqrte)},
    {"faddp", VIXL_AF(&TextAssembler::faddp)},
    {"sqxtun", VIXL_AF(&TextAssembler::sqxtun)},
    {"fmaxp", VIXL_AF(&TextAssembler::fmaxp)},
    {"sqxtn", VIXL_AF(&TextAssembler::sqxtn)},
    {"frint32x", VIXL_AF(&TextAssembler::frint32x)},
    {"suqadd", VIXL_AF(&TextAssembler::suqadd)},
    {"clz", VIXL_AF(&TextAssembler::clz)},
    {"addv", VIXL_AF(&TextAssembler::addv)},
    {"fcmp", VIXL_AF(&TextAssembler::fcmp)},
    {"fcvtas", VIXL_AF(&TextAssembler::fcvtas)},
    {"frintp", VIXL_AF(&TextAssembler::frintp)},
    {"fcvtns", VIXL_AF(&TextAssembler::fcvtns)},
    {"scvtf", VIXL_AF(&TextAssembler::scvtf)},
    {"sxtl", VIXL_AF(&TextAssembler::sxtl)},
    {"saddlp", VIXL_AF(&TextAssembler::saddlp)},
    {"fmov", VIXL_AF(&TextAssembler::fmov)},
    {"uqxtn2", VIXL_AF(&TextAssembler::uqxtn2)},
    {"sxtl2", VIXL_AF(&TextAssembler::sxtl2)},
    {"cnt", VIXL_AF(&TextAssembler::cnt)},
    {"frintm", VIXL_AF(&TextAssembler::frintm)},
    {"frintx", VIXL_AF(&TextAssembler::frintx)},
    {"fsqrt", VIXL_AF(&TextAssembler::fsqrt)},
    {"fcvtms", VIXL_AF(&TextAssembler::fcvtms)},
    {"sadalp", VIXL_AF(&TextAssembler::sadalp)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zMz_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zMz_insts_ = {
    {"cnot", VIXL_AF(&TextAssembler::cnot)},
    {"fabs", VIXL_AF(&TextAssembler::fabs)},
    {"neg", VIXL_AF(&TextAssembler::neg)},
    {"revb", VIXL_AF(&TextAssembler::revb)},
    {"uxtb", VIXL_AF(&TextAssembler::uxtb)},
    {"revh", VIXL_AF(&TextAssembler::revh)},
    {"movprfx", VIXL_AF(&TextAssembler::movprfx)},
    {"sxtw", VIXL_AF(&TextAssembler::sxtw)},
    {"clz", VIXL_AF(&TextAssembler::clz)},
    {"abs", VIXL_AF(&TextAssembler::abs)},
    {"frinti", VIXL_AF(&TextAssembler::frinti)},
    {"frintp", VIXL_AF(&TextAssembler::frintp)},
    {"frintz", VIXL_AF(&TextAssembler::frintz)},
    {"cls", VIXL_AF(&TextAssembler::cls)},
    {"scvtf", VIXL_AF(&TextAssembler::scvtf)},
    {"frintn", VIXL_AF(&TextAssembler::frintn)},
    {"sxtb", VIXL_AF(&TextAssembler::sxtb)},
    {"uxtw", VIXL_AF(&TextAssembler::uxtw)},
    {"uxth", VIXL_AF(&TextAssembler::uxth)},
    {"frecpx", VIXL_AF(&TextAssembler::frecpx)},
    {"frinta", VIXL_AF(&TextAssembler::frinta)},
    {"revw", VIXL_AF(&TextAssembler::revw)},
    {"fsqrt", VIXL_AF(&TextAssembler::fsqrt)},
    {"cnt", VIXL_AF(&TextAssembler::cnt)},
    {"frintx", VIXL_AF(&TextAssembler::frintx)},
    {"frintm", VIXL_AF(&TextAssembler::frintm)},
    {"fcvtzu", VIXL_AF(&TextAssembler::fcvtzu)},
    {"sxth", VIXL_AF(&TextAssembler::sxth)},
    {"ucvtf", VIXL_AF(&TextAssembler::ucvtf)},
    {"fcvt", VIXL_AF(&TextAssembler::fcvt)},
    {"fcvtzs", VIXL_AF(&TextAssembler::fcvtzs)},
    {"fneg", VIXL_AF(&TextAssembler::fneg)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
    {"rbit", VIXL_AF(&TextAssembler::rbit)},
    {"not", VIXL_AF(&TextAssembler::not_)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rro_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rro_insts_ = {
    {"bic", VIXL_AF(&TextAssembler::bic)},
    {"eor", VIXL_AF(&TextAssembler::eor)},
    {"ands", VIXL_AF(&TextAssembler::ands)},
    {"and", VIXL_AF(&TextAssembler::and_)},
    {"orn", VIXL_AF(&TextAssembler::orn)},
    {"sbcs", VIXL_AF(&TextAssembler::sbcs)},
    {"adc", VIXL_AF(&TextAssembler::adc)},
    {"adcs", VIXL_AF(&TextAssembler::adcs)},
    {"bics", VIXL_AF(&TextAssembler::bics)},
    {"orr", VIXL_AF(&TextAssembler::orr)},
    {"sbc", VIXL_AF(&TextAssembler::sbc)},
    {"sub", VIXL_AF(&TextAssembler::sub)},
    {"adds", VIXL_AF(&TextAssembler::adds)},
    {"subs", VIXL_AF(&TextAssembler::subs)},
    {"add", VIXL_AF(&TextAssembler::add)},
    {"eon", VIXL_AF(&TextAssembler::eon)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvv_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvv_insts_ = {
    {"sabal", VIXL_AF(&TextAssembler::sabal)},
    {"addhn2", VIXL_AF(&TextAssembler::addhn2)},
    {"fmlal", VIXL_AF(&TextAssembler::fmlal)},
    {"orn", VIXL_AF(&TextAssembler::orn)},
    {"sqrdmlsh", VIXL_AF(&TextAssembler::sqrdmlsh)},
    {"fmaxnm", VIXL_AF(&TextAssembler::fmaxnm)},
    {"addhn", VIXL_AF(&TextAssembler::addhn)},
    {"pmul", VIXL_AF(&TextAssembler::pmul)},
    {"umax", VIXL_AF(&TextAssembler::umax)},
    {"fmlal2", VIXL_AF(&TextAssembler::fmlal2)},
    {"shadd", VIXL_AF(&TextAssembler::shadd)},
    {"sqdmull", VIXL_AF(&TextAssembler::sqdmull)},
    {"uqshl", VIXL_AF(&TextAssembler::uqshl)},
    {"zip1", VIXL_AF(&TextAssembler::zip1)},
    {"cmtst", VIXL_AF(&TextAssembler::cmtst)},
    {"sqdmull2", VIXL_AF(&TextAssembler::sqdmull2)},
    {"cmhi", VIXL_AF(&TextAssembler::cmhi)},
    {"eor", VIXL_AF(&TextAssembler::eor)},
    {"fminnm", VIXL_AF(&TextAssembler::fminnm)},
    {"fmax", VIXL_AF(&TextAssembler::fmax)},
    {"fmls", VIXL_AF(&TextAssembler::fmls)},
    {"fmlsl2", VIXL_AF(&TextAssembler::fmlsl2)},
    {"subhn2", VIXL_AF(&TextAssembler::subhn2)},
    {"sqadd", VIXL_AF(&TextAssembler::sqadd)},
    {"fmul", VIXL_AF(&TextAssembler::fmul)},
    {"fadd", VIXL_AF(&TextAssembler::fadd)},
    {"sqrdmulh", VIXL_AF(&TextAssembler::sqrdmulh)},
    {"ssubw", VIXL_AF(&TextAssembler::ssubw)},
    {"add", VIXL_AF(&TextAssembler::add)},
    {"fmaxnmp", VIXL_AF(&TextAssembler::fmaxnmp)},
    {"mla", VIXL_AF(&TextAssembler::mla)},
    {"smax", VIXL_AF(&TextAssembler::smax)},
    {"fmla", VIXL_AF(&TextAssembler::fmla)},
    {"fmlsl", VIXL_AF(&TextAssembler::fmlsl)},
    {"shsub", VIXL_AF(&TextAssembler::shsub)},
    {"fminp", VIXL_AF(&TextAssembler::fminp)},
    {"uabal", VIXL_AF(&TextAssembler::uabal)},
    {"umlal2", VIXL_AF(&TextAssembler::umlal2)},
    {"fmin", VIXL_AF(&TextAssembler::fmin)},
    {"cmge", VIXL_AF(&TextAssembler::cmge)},
    {"facgt", VIXL_AF(&TextAssembler::facgt)},
    {"sabd", VIXL_AF(&TextAssembler::sabd)},
    {"smaxp", VIXL_AF(&TextAssembler::smaxp)},
    {"ssubl", VIXL_AF(&TextAssembler::ssubl)},
    {"uaba", VIXL_AF(&TextAssembler::uaba)},
    {"uabdl", VIXL_AF(&TextAssembler::uabdl)},
    {"uqadd", VIXL_AF(&TextAssembler::uqadd)},
    {"orr", VIXL_AF(&TextAssembler::orr)},
    {"sshl", VIXL_AF(&TextAssembler::sshl)},
    {"tbl", VIXL_AF(&TextAssembler::tbl)},
    {"uhsub", VIXL_AF(&TextAssembler::uhsub)},
    {"saddl2", VIXL_AF(&TextAssembler::saddl2)},
    {"sminp", VIXL_AF(&TextAssembler::sminp)},
    {"umull", VIXL_AF(&TextAssembler::umull)},
    {"fsub", VIXL_AF(&TextAssembler::fsub)},
    {"fdiv", VIXL_AF(&TextAssembler::fdiv)},
    {"uzp2", VIXL_AF(&TextAssembler::uzp2)},
    {"srshl", VIXL_AF(&TextAssembler::srshl)},
    {"umlsl", VIXL_AF(&TextAssembler::umlsl)},
    {"and", VIXL_AF(&TextAssembler::and_)},
    {"fmulx", VIXL_AF(&TextAssembler::fmulx)},
    {"sqdmlsl", VIXL_AF(&TextAssembler::sqdmlsl)},
    {"sqrdmlah", VIXL_AF(&TextAssembler::sqrdmlah)},
    {"cmgt", VIXL_AF(&TextAssembler::cmgt)},
    {"raddhn", VIXL_AF(&TextAssembler::raddhn)},
    {"sub", VIXL_AF(&TextAssembler::sub)},
    {"subhn", VIXL_AF(&TextAssembler::subhn)},
    {"frecps", VIXL_AF(&TextAssembler::frecps)},
    {"fcmgt", VIXL_AF(&TextAssembler::fcmgt)},
    {"trn1", VIXL_AF(&TextAssembler::trn1)},
    {"sqrshl", VIXL_AF(&TextAssembler::sqrshl)},
    {"uqrshl", VIXL_AF(&TextAssembler::uqrshl)},
    {"bic", VIXL_AF(&TextAssembler::bic)},
    {"bsl", VIXL_AF(&TextAssembler::bsl)},
    {"sqdmulh", VIXL_AF(&TextAssembler::sqdmulh)},
    {"umlal", VIXL_AF(&TextAssembler::umlal)},
    {"umull2", VIXL_AF(&TextAssembler::umull2)},
    {"usubl2", VIXL_AF(&TextAssembler::usubl2)},
    {"bit", VIXL_AF(&TextAssembler::bit)},
    {"mls", VIXL_AF(&TextAssembler::mls)},
    {"smlal2", VIXL_AF(&TextAssembler::smlal2)},
    {"uabal2", VIXL_AF(&TextAssembler::uabal2)},
    {"fnmul", VIXL_AF(&TextAssembler::fnmul)},
    {"frsqrts", VIXL_AF(&TextAssembler::frsqrts)},
    {"uabd", VIXL_AF(&TextAssembler::uabd)},
    {"cmeq", VIXL_AF(&TextAssembler::cmeq)},
    {"sqdmlal", VIXL_AF(&TextAssembler::sqdmlal)},
    {"uminp", VIXL_AF(&TextAssembler::uminp)},
    {"sqdmlal2", VIXL_AF(&TextAssembler::sqdmlal2)},
    {"fminnmp", VIXL_AF(&TextAssembler::fminnmp)},
    {"raddhn2", VIXL_AF(&TextAssembler::raddhn2)},
    {"umaxp", VIXL_AF(&TextAssembler::umaxp)},
    {"uaddw2", VIXL_AF(&TextAssembler::uaddw2)},
    {"sabdl", VIXL_AF(&TextAssembler::sabdl)},
    {"sdot", VIXL_AF(&TextAssembler::sdot)},
    {"trn2", VIXL_AF(&TextAssembler::trn2)},
    {"uaddl2", VIXL_AF(&TextAssembler::uaddl2)},
    {"rsubhn", VIXL_AF(&TextAssembler::rsubhn)},
    {"sqdmlsl2", VIXL_AF(&TextAssembler::sqdmlsl2)},
    {"facge", VIXL_AF(&TextAssembler::facge)},
    {"cmhs", VIXL_AF(&TextAssembler::cmhs)},
    {"saddw", VIXL_AF(&TextAssembler::saddw)},
    {"smlal", VIXL_AF(&TextAssembler::smlal)},
    {"uaddw", VIXL_AF(&TextAssembler::uaddw)},
    {"udot", VIXL_AF(&TextAssembler::udot)},
    {"fcmge", VIXL_AF(&TextAssembler::fcmge)},
    {"saba", VIXL_AF(&TextAssembler::saba)},
    {"addp", VIXL_AF(&TextAssembler::addp)},
    {"smull2", VIXL_AF(&TextAssembler::smull2)},
    {"urshl", VIXL_AF(&TextAssembler::urshl)},
    {"uabdl2", VIXL_AF(&TextAssembler::uabdl2)},
    {"uhadd", VIXL_AF(&TextAssembler::uhadd)},
    {"fabd", VIXL_AF(&TextAssembler::fabd)},
    {"sqshl", VIXL_AF(&TextAssembler::sqshl)},
    {"faddp", VIXL_AF(&TextAssembler::faddp)},
    {"umlsl2", VIXL_AF(&TextAssembler::umlsl2)},
    {"fcmeq", VIXL_AF(&TextAssembler::fcmeq)},
    {"fmaxp", VIXL_AF(&TextAssembler::fmaxp)},
    {"sabdl2", VIXL_AF(&TextAssembler::sabdl2)},
    {"saddl", VIXL_AF(&TextAssembler::saddl)},
    {"ssubw2", VIXL_AF(&TextAssembler::ssubw2)},
    {"uaddl", VIXL_AF(&TextAssembler::uaddl)},
    {"ushl", VIXL_AF(&TextAssembler::ushl)},
    {"usubw2", VIXL_AF(&TextAssembler::usubw2)},
    {"zip2", VIXL_AF(&TextAssembler::zip2)},
    {"bif", VIXL_AF(&TextAssembler::bif)},
    {"pmull", VIXL_AF(&TextAssembler::pmull)},
    {"smin", VIXL_AF(&TextAssembler::smin)},
    {"urhadd", VIXL_AF(&TextAssembler::urhadd)},
    {"rsubhn2", VIXL_AF(&TextAssembler::rsubhn2)},
    {"srhadd", VIXL_AF(&TextAssembler::srhadd)},
    {"uqsub", VIXL_AF(&TextAssembler::uqsub)},
    {"pmull2", VIXL_AF(&TextAssembler::pmull2)},
    {"usubl", VIXL_AF(&TextAssembler::usubl)},
    {"uzp1", VIXL_AF(&TextAssembler::uzp1)},
    {"ssubl2", VIXL_AF(&TextAssembler::ssubl2)},
    {"saddw2", VIXL_AF(&TextAssembler::saddw2)},
    {"smlsl2", VIXL_AF(&TextAssembler::smlsl2)},
    {"mul", VIXL_AF(&TextAssembler::mul)},
    {"smull", VIXL_AF(&TextAssembler::smull)},
    {"smlsl", VIXL_AF(&TextAssembler::smlsl)},
    {"sqsub", VIXL_AF(&TextAssembler::sqsub)},
    {"sabal2", VIXL_AF(&TextAssembler::sabal2)},
    {"usubw", VIXL_AF(&TextAssembler::usubw)},
    {"tbx", VIXL_AF(&TextAssembler::tbx)},
    {"umin", VIXL_AF(&TextAssembler::umin)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zMzz_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zMzz_insts_ = {
    {"fdivr", VIXL_AF(&TextAssembler::fdivr)},
    {"fmin", VIXL_AF(&TextAssembler::fmin)},
    {"bic", VIXL_AF(&TextAssembler::bic)},
    {"fabd", VIXL_AF(&TextAssembler::fabd)},
    {"fmsb", VIXL_AF(&TextAssembler::fmsb)},
    {"fnmla", VIXL_AF(&TextAssembler::fnmla)},
    {"fnmls", VIXL_AF(&TextAssembler::fnmls)},
    {"sabd", VIXL_AF(&TextAssembler::sabd)},
    {"mls", VIXL_AF(&TextAssembler::mls)},
    {"sdiv", VIXL_AF(&TextAssembler::sdiv)},
    {"fnmad", VIXL_AF(&TextAssembler::fnmad)},
    {"orr", VIXL_AF(&TextAssembler::orr)},
    {"udiv", VIXL_AF(&TextAssembler::udiv)},
    {"umulh", VIXL_AF(&TextAssembler::umulh)},
    {"msb", VIXL_AF(&TextAssembler::msb)},
    {"fmaxnm", VIXL_AF(&TextAssembler::fmaxnm)},
    {"smin", VIXL_AF(&TextAssembler::smin)},
    {"mla", VIXL_AF(&TextAssembler::mla)},
    {"uabd", VIXL_AF(&TextAssembler::uabd)},
    {"lsl", VIXL_AF(&TextAssembler::lsl)},
    {"fmla", VIXL_AF(&TextAssembler::fmla)},
    {"fsub", VIXL_AF(&TextAssembler::fsub)},
    {"sdivr", VIXL_AF(&TextAssembler::sdivr)},
    {"umax", VIXL_AF(&TextAssembler::umax)},
    {"subr", VIXL_AF(&TextAssembler::subr)},
    {"fdiv", VIXL_AF(&TextAssembler::fdiv)},
    {"smax", VIXL_AF(&TextAssembler::smax)},
    {"and", VIXL_AF(&TextAssembler::and_)},
    {"mul", VIXL_AF(&TextAssembler::mul)},
    {"sub", VIXL_AF(&TextAssembler::sub)},
    {"fmulx", VIXL_AF(&TextAssembler::fmulx)},
    {"smulh", VIXL_AF(&TextAssembler::smulh)},
    {"umin", VIXL_AF(&TextAssembler::umin)},
    {"asr", VIXL_AF(&TextAssembler::asr)},
    {"eor", VIXL_AF(&TextAssembler::eor)},
    {"fmax", VIXL_AF(&TextAssembler::fmax)},
    {"fminnm", VIXL_AF(&TextAssembler::fminnm)},
    {"fmls", VIXL_AF(&TextAssembler::fmls)},
    {"lsr", VIXL_AF(&TextAssembler::lsr)},
    {"fmul", VIXL_AF(&TextAssembler::fmul)},
    {"mad", VIXL_AF(&TextAssembler::mad)},
    {"asrr", VIXL_AF(&TextAssembler::asrr)},
    {"fadd", VIXL_AF(&TextAssembler::fadd)},
    {"fnmsb", VIXL_AF(&TextAssembler::fnmsb)},
    {"fscale", VIXL_AF(&TextAssembler::fscale)},
    {"lsrr", VIXL_AF(&TextAssembler::lsrr)},
    {"udivr", VIXL_AF(&TextAssembler::udivr)},
    {"fsubr", VIXL_AF(&TextAssembler::fsubr)},
    {"lslr", VIXL_AF(&TextAssembler::lslr)},
    {"add", VIXL_AF(&TextAssembler::add)},
    {"fmad", VIXL_AF(&TextAssembler::fmad)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzi_insts_ = {
    {"mul", VIXL_AF(&TextAssembler::mul)},
    {"lsr", VIXL_AF(&TextAssembler::lsr)},
    {"asr", VIXL_AF(&TextAssembler::asr)},
    {"smax", VIXL_AF(&TextAssembler::smax)},
    {"smin", VIXL_AF(&TextAssembler::smin)},
    {"sqsub", VIXL_AF(&TextAssembler::sqsub)},
    {"lsl", VIXL_AF(&TextAssembler::lsl)},
    {"sqadd", VIXL_AF(&TextAssembler::sqadd)},
    {"sub", VIXL_AF(&TextAssembler::sub)},
    {"subr", VIXL_AF(&TextAssembler::subr)},
    {"uqsub", VIXL_AF(&TextAssembler::uqsub)},
    {"umax", VIXL_AF(&TextAssembler::umax)},
    {"add", VIXL_AF(&TextAssembler::add)},
    {"umin", VIXL_AF(&TextAssembler::umin)},
    {"uqadd", VIXL_AF(&TextAssembler::uqadd)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzii_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzii_insts_ = {
    {"uqadd", VIXL_AF(&TextAssembler::uqadd)},
    {"uqsub", VIXL_AF(&TextAssembler::uqsub)},
    {"sqsub", VIXL_AF(&TextAssembler::sqsub)},
    {"sub", VIXL_AF(&TextAssembler::sub)},
    {"subr", VIXL_AF(&TextAssembler::subr)},
    {"sqadd", VIXL_AF(&TextAssembler::sqadd)},
    {"add", VIXL_AF(&TextAssembler::add)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzz_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzz_insts_ = {
    {"bic", VIXL_AF(&TextAssembler::bic)},
    {"uqadd", VIXL_AF(&TextAssembler::uqadd)},
    {"uqsub", VIXL_AF(&TextAssembler::uqsub)},
    {"ftsmul", VIXL_AF(&TextAssembler::ftsmul)},
    {"orr", VIXL_AF(&TextAssembler::orr)},
    {"tbl", VIXL_AF(&TextAssembler::tbl)},
    {"zip2", VIXL_AF(&TextAssembler::zip2)},
    {"frsqrts", VIXL_AF(&TextAssembler::frsqrts)},
    {"lsl", VIXL_AF(&TextAssembler::lsl)},
    {"fsub", VIXL_AF(&TextAssembler::fsub)},
    {"uzp1", VIXL_AF(&TextAssembler::uzp1)},
    {"uzp2", VIXL_AF(&TextAssembler::uzp2)},
    {"zip1", VIXL_AF(&TextAssembler::zip1)},
    {"sdot", VIXL_AF(&TextAssembler::sdot)},
    {"trn2", VIXL_AF(&TextAssembler::trn2)},
    {"and", VIXL_AF(&TextAssembler::and_)},
    {"eor", VIXL_AF(&TextAssembler::eor)},
    {"asr", VIXL_AF(&TextAssembler::asr)},
    {"ftssel", VIXL_AF(&TextAssembler::ftssel)},
    {"lsr", VIXL_AF(&TextAssembler::lsr)},
    {"sub", VIXL_AF(&TextAssembler::sub)},
    {"sqsub", VIXL_AF(&TextAssembler::sqsub)},
    {"fmul", VIXL_AF(&TextAssembler::fmul)},
    {"sqadd", VIXL_AF(&TextAssembler::sqadd)},
    {"fadd", VIXL_AF(&TextAssembler::fadd)},
    {"frecps", VIXL_AF(&TextAssembler::frecps)},
    {"udot", VIXL_AF(&TextAssembler::udot)},
    {"trn1", VIXL_AF(&TextAssembler::trn1)},
    {"add", VIXL_AF(&TextAssembler::add)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rri_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rri_insts_ = {
    {"sqdech", VIXL_AF(&TextAssembler::sqdech)},
    {"sqdecd", VIXL_AF(&TextAssembler::sqdecd)},
    {"addpl", VIXL_AF(&TextAssembler::addpl)},
    {"sqinch", VIXL_AF(&TextAssembler::sqinch)},
    {"sqdecw", VIXL_AF(&TextAssembler::sqdecw)},
    {"sqincw", VIXL_AF(&TextAssembler::sqincw)},
    {"sqincb", VIXL_AF(&TextAssembler::sqincb)},
    {"addvl", VIXL_AF(&TextAssembler::addvl)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rl_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rl_insts_ = {
    {"cbz", VIXL_AF(&TextAssembler::cbz)},
    {"adr", VIXL_AF(&TextAssembler::adr)},
    {"cbnz", VIXL_AF(&TextAssembler::cbnz)},
    {"adrp", VIXL_AF(&TextAssembler::adrp)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_ri64_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::ri64_insts_ = {
    {"adr", VIXL_AF(&TextAssembler::adr)},
    {"cbnz", VIXL_AF(&TextAssembler::cbnz)},
    {"adrp", VIXL_AF(&TextAssembler::adrp)},
    {"cbz", VIXL_AF(&TextAssembler::cbz)},
    {"ldr", VIXL_AF(&TextAssembler::ldr)},
    {"ldrsw", VIXL_AF(&TextAssembler::ldrsw)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zs_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zs_insts_ = {
    {"ldr", VIXL_AF(&TextAssembler::ldr)},
    {"str", VIXL_AF(&TextAssembler::str)},
    {"adr", VIXL_AF(&TextAssembler::adr)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LZLL_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::LZLL_insts_ = {
    {"brkns", VIXL_AF(&TextAssembler::brkns)},
    {"brkpbs", VIXL_AF(&TextAssembler::brkpbs)},
    {"bic", VIXL_AF(&TextAssembler::bic)},
    {"brkpas", VIXL_AF(&TextAssembler::brkpas)},
    {"ands", VIXL_AF(&TextAssembler::ands)},
    {"and", VIXL_AF(&TextAssembler::and_)},
    {"eor", VIXL_AF(&TextAssembler::eor)},
    {"eors", VIXL_AF(&TextAssembler::eors)},
    {"brkn", VIXL_AF(&TextAssembler::brkn)},
    {"bics", VIXL_AF(&TextAssembler::bics)},
    {"nand", VIXL_AF(&TextAssembler::nand)},
    {"nands", VIXL_AF(&TextAssembler::nands)},
    {"orns", VIXL_AF(&TextAssembler::orns)},
    {"orr", VIXL_AF(&TextAssembler::orr)},
    {"orrs", VIXL_AF(&TextAssembler::orrs)},
    {"brkpa", VIXL_AF(&TextAssembler::brkpa)},
    {"brkpb", VIXL_AF(&TextAssembler::brkpb)},
    {"orn", VIXL_AF(&TextAssembler::orn)},
    {"nor", VIXL_AF(&TextAssembler::nor)},
    {"nors", VIXL_AF(&TextAssembler::nors)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzu64_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzu64_insts_ = {
    {"eor", VIXL_AF(&TextAssembler::eor)},
    {"bic", VIXL_AF(&TextAssembler::bic)},
    {"orn", VIXL_AF(&TextAssembler::orn)},
    {"and", VIXL_AF(&TextAssembler::and_)},
    {"orr", VIXL_AF(&TextAssembler::orr)},
    {"eon", VIXL_AF(&TextAssembler::eon)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vpz_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vpz_insts_ = {
    {"fmaxv", VIXL_AF(&TextAssembler::fmaxv)},
    {"faddv", VIXL_AF(&TextAssembler::faddv)},
    {"fminv", VIXL_AF(&TextAssembler::fminv)},
    {"lasta", VIXL_AF(&TextAssembler::lasta)},
    {"lastb", VIXL_AF(&TextAssembler::lastb)},
    {"orv", VIXL_AF(&TextAssembler::orv)},
    {"uaddv", VIXL_AF(&TextAssembler::uaddv)},
    {"fminnmv", VIXL_AF(&TextAssembler::fminnmv)},
    {"smaxv", VIXL_AF(&TextAssembler::smaxv)},
    {"umaxv", VIXL_AF(&TextAssembler::umaxv)},
    {"uminv", VIXL_AF(&TextAssembler::uminv)},
    {"andv", VIXL_AF(&TextAssembler::andv)},
    {"sminv", VIXL_AF(&TextAssembler::sminv)},
    {"saddv", VIXL_AF(&TextAssembler::saddv)},
    {"eorv", VIXL_AF(&TextAssembler::eorv)},
    {"fmaxnmv", VIXL_AF(&TextAssembler::fmaxnmv)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rru_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rru_insts_ = {
    {"ror", VIXL_AF(&TextAssembler::ror)},
    {"lsr", VIXL_AF(&TextAssembler::lsr)},
    {"lsl", VIXL_AF(&TextAssembler::lsl)},
    {"asr", VIXL_AF(&TextAssembler::asr)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zMzi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zMzi_insts_ = {
    {"lsr", VIXL_AF(&TextAssembler::lsr)},
    {"lsl", VIXL_AF(&TextAssembler::lsl)},
    {"asr", VIXL_AF(&TextAssembler::asr)},
    {"asrd", VIXL_AF(&TextAssembler::asrd)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rrr_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rrr_insts_ = {
    {"lsrv", VIXL_AF(&TextAssembler::lsrv)},
    {"crc32w", VIXL_AF(&TextAssembler::crc32w)},
    {"rorv", VIXL_AF(&TextAssembler::rorv)},
    {"sdiv", VIXL_AF(&TextAssembler::sdiv)},
    {"udiv", VIXL_AF(&TextAssembler::udiv)},
    {"umulh", VIXL_AF(&TextAssembler::umulh)},
    {"crc32cw", VIXL_AF(&TextAssembler::crc32cw)},
    {"umull", VIXL_AF(&TextAssembler::umull)},
    {"crc32b", VIXL_AF(&TextAssembler::crc32b)},
    {"crc32cx", VIXL_AF(&TextAssembler::crc32cx)},
    {"asrv", VIXL_AF(&TextAssembler::asrv)},
    {"crc32x", VIXL_AF(&TextAssembler::crc32x)},
    {"crc32cb", VIXL_AF(&TextAssembler::crc32cb)},
    {"mneg", VIXL_AF(&TextAssembler::mneg)},
    {"smulh", VIXL_AF(&TextAssembler::smulh)},
    {"mul", VIXL_AF(&TextAssembler::mul)},
    {"pacga", VIXL_AF(&TextAssembler::pacga)},
    {"smull", VIXL_AF(&TextAssembler::smull)},
    {"crc32h", VIXL_AF(&TextAssembler::crc32h)},
    {"lslv", VIXL_AF(&TextAssembler::lslv)},
    {"crc32ch", VIXL_AF(&TextAssembler::crc32ch)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rr_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rr_insts_ = {
    {"blrab", VIXL_AF(&TextAssembler::blrab)},
    {"brab", VIXL_AF(&TextAssembler::brab)},
    {"autia", VIXL_AF(&TextAssembler::autia)},
    {"rev16", VIXL_AF(&TextAssembler::rev16)},
    {"rev", VIXL_AF(&TextAssembler::rev)},
    {"uxtb", VIXL_AF(&TextAssembler::uxtb)},
    {"autib", VIXL_AF(&TextAssembler::autib)},
    {"pacdb", VIXL_AF(&TextAssembler::pacdb)},
    {"sxtw", VIXL_AF(&TextAssembler::sxtw)},
    {"blraa", VIXL_AF(&TextAssembler::blraa)},
    {"sqincb", VIXL_AF(&TextAssembler::sqincb)},
    {"braa", VIXL_AF(&TextAssembler::braa)},
    {"clz", VIXL_AF(&TextAssembler::clz)},
    {"ctermeq", VIXL_AF(&TextAssembler::ctermeq)},
    {"pacia", VIXL_AF(&TextAssembler::pacia)},
    {"sqdecd", VIXL_AF(&TextAssembler::sqdecd)},
    {"sqinch", VIXL_AF(&TextAssembler::sqinch)},
    {"cls", VIXL_AF(&TextAssembler::cls)},
    {"rev32", VIXL_AF(&TextAssembler::rev32)},
    {"sxtb", VIXL_AF(&TextAssembler::sxtb)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
    {"rev64", VIXL_AF(&TextAssembler::rev64)},
    {"uxth", VIXL_AF(&TextAssembler::uxth)},
    {"sqdech", VIXL_AF(&TextAssembler::sqdech)},
    {"pacib", VIXL_AF(&TextAssembler::pacib)},
    {"sqdecw", VIXL_AF(&TextAssembler::sqdecw)},
    {"sqincw", VIXL_AF(&TextAssembler::sqincw)},
    {"ctermne", VIXL_AF(&TextAssembler::ctermne)},
    {"sxth", VIXL_AF(&TextAssembler::sxth)},
    {"autda", VIXL_AF(&TextAssembler::autda)},
    {"autdb", VIXL_AF(&TextAssembler::autdb)},
    {"pacda", VIXL_AF(&TextAssembler::pacda)},
    {"rbit", VIXL_AF(&TextAssembler::rbit)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_r_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::r_insts_ = {
    {"cntd", VIXL_AF(&TextAssembler::cntd)},
    {"decw", VIXL_AF(&TextAssembler::decw)},
    {"autdzb", VIXL_AF(&TextAssembler::autdzb)},
    {"incb", VIXL_AF(&TextAssembler::incb)},
    {"pacdzb", VIXL_AF(&TextAssembler::pacdzb)},
    {"paciza", VIXL_AF(&TextAssembler::paciza)},
    {"incw", VIXL_AF(&TextAssembler::incw)},
    {"setf16", VIXL_AF(&TextAssembler::setf16)},
    {"sqincd", VIXL_AF(&TextAssembler::sqincd)},
    {"uqdecd", VIXL_AF(&TextAssembler::uqdecd)},
    {"uqinch", VIXL_AF(&TextAssembler::uqinch)},
    {"braaz", VIXL_AF(&TextAssembler::braaz)},
    {"sqincb", VIXL_AF(&TextAssembler::sqincb)},
    {"autizb", VIXL_AF(&TextAssembler::autizb)},
    {"autdza", VIXL_AF(&TextAssembler::autdza)},
    {"cnth", VIXL_AF(&TextAssembler::cnth)},
    {"decd", VIXL_AF(&TextAssembler::decd)},
    {"sqdecd", VIXL_AF(&TextAssembler::sqdecd)},
    {"sqinch", VIXL_AF(&TextAssembler::sqinch)},
    {"decb", VIXL_AF(&TextAssembler::decb)},
    {"uqdecb", VIXL_AF(&TextAssembler::uqdecb)},
    {"uqincd", VIXL_AF(&TextAssembler::uqincd)},
    {"blrabz", VIXL_AF(&TextAssembler::blrabz)},
    {"xpaci", VIXL_AF(&TextAssembler::xpaci)},
    {"dech", VIXL_AF(&TextAssembler::dech)},
    {"uqincw", VIXL_AF(&TextAssembler::uqincw)},
    {"autiza", VIXL_AF(&TextAssembler::autiza)},
    {"xpacd", VIXL_AF(&TextAssembler::xpacd)},
    {"pacdza", VIXL_AF(&TextAssembler::pacdza)},
    {"br", VIXL_AF(&TextAssembler::br)},
    {"uqdecw", VIXL_AF(&TextAssembler::uqdecw)},
    {"cntw", VIXL_AF(&TextAssembler::cntw)},
    {"pacizb", VIXL_AF(&TextAssembler::pacizb)},
    {"inch", VIXL_AF(&TextAssembler::inch)},
    {"setf8", VIXL_AF(&TextAssembler::setf8)},
    {"sqdecb", VIXL_AF(&TextAssembler::sqdecb)},
    {"sqdech", VIXL_AF(&TextAssembler::sqdech)},
    {"sqdecw", VIXL_AF(&TextAssembler::sqdecw)},
    {"sqincw", VIXL_AF(&TextAssembler::sqincw)},
    {"blraaz", VIXL_AF(&TextAssembler::blraaz)},
    {"cntb", VIXL_AF(&TextAssembler::cntb)},
    {"uqdech", VIXL_AF(&TextAssembler::uqdech)},
    {"uqincb", VIXL_AF(&TextAssembler::uqincb)},
    {"blr", VIXL_AF(&TextAssembler::blr)},
    {"incd", VIXL_AF(&TextAssembler::incd)},
    {"ret", VIXL_AF(&TextAssembler::ret)},
    {"brabz", VIXL_AF(&TextAssembler::brabz)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL__Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::_insts_ = {
    {"autibsp", VIXL_AF(&TextAssembler::autibsp)},
    {"autib1716", VIXL_AF(&TextAssembler::autib1716)},
    {"retab", VIXL_AF(&TextAssembler::retab)},
    {"setffr", VIXL_AF(&TextAssembler::setffr)},
    {"csdb", VIXL_AF(&TextAssembler::csdb)},
    {"cfinv", VIXL_AF(&TextAssembler::cfinv)},
    {"ret", VIXL_AF(&TextAssembler::ret)},
    {"retaa", VIXL_AF(&TextAssembler::retaa)},
    {"paciasp", VIXL_AF(&TextAssembler::paciasp)},
    {"xaflag", VIXL_AF(&TextAssembler::xaflag)},
    {"autiaz", VIXL_AF(&TextAssembler::autiaz)},
    {"pacibz", VIXL_AF(&TextAssembler::pacibz)},
    {"autiasp", VIXL_AF(&TextAssembler::autiasp)},
    {"isb", VIXL_AF(&TextAssembler::isb)},
    {"autia1716", VIXL_AF(&TextAssembler::autia1716)},
    {"nop", VIXL_AF(&TextAssembler::nop)},
    {"esb", VIXL_AF(&TextAssembler::esb)},
    {"pacia1716", VIXL_AF(&TextAssembler::pacia1716)},
    {"paciaz", VIXL_AF(&TextAssembler::paciaz)},
    {"pacibsp", VIXL_AF(&TextAssembler::pacibsp)},
    {"xpaclri", VIXL_AF(&TextAssembler::xpaclri)},
    {"autibz", VIXL_AF(&TextAssembler::autibz)},
    {"axflag", VIXL_AF(&TextAssembler::axflag)},
    {"pacib1716", VIXL_AF(&TextAssembler::pacib1716)},
    {"clrex", VIXL_AF(&TextAssembler::clrex)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_lc_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::lc_insts_ = {
    {"b", VIXL_AF(&TextAssembler::b)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_i64c_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::i64c_insts_ = {
    {"b", VIXL_AF(&TextAssembler::b)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_l_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::l_insts_ = {
    {"b", VIXL_AF(&TextAssembler::b)}, {"bl", VIXL_AF(&TextAssembler::bl)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_i64_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::i64_insts_ = {
    {"b", VIXL_AF(&TextAssembler::b)}, {"bl", VIXL_AF(&TextAssembler::bl)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_ruu_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::ruu_insts_ = {
    {"bfc", VIXL_AF(&TextAssembler::bfc)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rruu_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rruu_insts_ = {
    {"bfi", VIXL_AF(&TextAssembler::bfi)},
    {"bfm", VIXL_AF(&TextAssembler::bfm)},
    {"sbfm", VIXL_AF(&TextAssembler::sbfm)},
    {"bfxil", VIXL_AF(&TextAssembler::bfxil)},
    {"ubfiz", VIXL_AF(&TextAssembler::ubfiz)},
    {"ubfm", VIXL_AF(&TextAssembler::ubfm)},
    {"sbfx", VIXL_AF(&TextAssembler::sbfx)},
    {"sbfiz", VIXL_AF(&TextAssembler::sbfiz)},
    {"ubfx", VIXL_AF(&TextAssembler::ubfx)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vi_insts_ = {
    {"bic", VIXL_AF(&TextAssembler::bic)},
    {"orr", VIXL_AF(&TextAssembler::orr)},
    {"mvni", VIXL_AF(&TextAssembler::mvni)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vii_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vii_insts_ = {
    {"bic", VIXL_AF(&TextAssembler::bic)},
    {"orr", VIXL_AF(&TextAssembler::orr)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_i_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::i_insts_ = {
    {"svc", VIXL_AF(&TextAssembler::svc)},
    {"hlt", VIXL_AF(&TextAssembler::hlt)},
    {"brk", VIXL_AF(&TextAssembler::brk)},
    {"clrex", VIXL_AF(&TextAssembler::clrex)},
    {"udf", VIXL_AF(&TextAssembler::udf)},
    {"hint", VIXL_AF(&TextAssembler::hint)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LML_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::LML_insts_ = {
    {"mov", VIXL_AF(&TextAssembler::mov)},
    {"brkb", VIXL_AF(&TextAssembler::brkb)},
    {"brka", VIXL_AF(&TextAssembler::brka)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LZL_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::LZL_insts_ = {
    {"brkas", VIXL_AF(&TextAssembler::brkas)},
    {"movs", VIXL_AF(&TextAssembler::movs)},
    {"brka", VIXL_AF(&TextAssembler::brka)},
    {"nots", VIXL_AF(&TextAssembler::nots)},
    {"brkb", VIXL_AF(&TextAssembler::brkb)},
    {"brkbs", VIXL_AF(&TextAssembler::brkbs)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
    {"not", VIXL_AF(&TextAssembler::not_)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_T_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::T_insts_ = {
    {"bti", VIXL_AF(&TextAssembler::bti)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rrm_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rrm_insts_ = {
    {"ldsmin", VIXL_AF(&TextAssembler::ldsmin)},
    {"ldsminb", VIXL_AF(&TextAssembler::ldsminb)},
    {"ldset", VIXL_AF(&TextAssembler::ldset)},
    {"ldumaxa", VIXL_AF(&TextAssembler::ldumaxa)},
    {"ldumaxalb", VIXL_AF(&TextAssembler::ldumaxalb)},
    {"cas", VIXL_AF(&TextAssembler::cas)},
    {"ldseth", VIXL_AF(&TextAssembler::ldseth)},
    {"swpal", VIXL_AF(&TextAssembler::swpal)},
    {"swpab", VIXL_AF(&TextAssembler::swpab)},
    {"ldclrab", VIXL_AF(&TextAssembler::ldclrab)},
    {"ldclralh", VIXL_AF(&TextAssembler::ldclralh)},
    {"ldeorlh", VIXL_AF(&TextAssembler::ldeorlh)},
    {"stnp", VIXL_AF(&TextAssembler::stnp)},
    {"ldsminalh", VIXL_AF(&TextAssembler::ldsminalh)},
    {"ldsetlb", VIXL_AF(&TextAssembler::ldsetlb)},
    {"ldumin", VIXL_AF(&TextAssembler::ldumin)},
    {"ldsetlh", VIXL_AF(&TextAssembler::ldsetlh)},
    {"swpah", VIXL_AF(&TextAssembler::swpah)},
    {"ldumaxh", VIXL_AF(&TextAssembler::ldumaxh)},
    {"ldsmaxb", VIXL_AF(&TextAssembler::ldsmaxb)},
    {"ldsmaxlb", VIXL_AF(&TextAssembler::ldsmaxlb)},
    {"ldsminlb", VIXL_AF(&TextAssembler::ldsminlb)},
    {"ldsminah", VIXL_AF(&TextAssembler::ldsminah)},
    {"casah", VIXL_AF(&TextAssembler::casah)},
    {"ldaddab", VIXL_AF(&TextAssembler::ldaddab)},
    {"ldclral", VIXL_AF(&TextAssembler::ldclral)},
    {"ldclrlh", VIXL_AF(&TextAssembler::ldclrlh)},
    {"ldaddh", VIXL_AF(&TextAssembler::ldaddh)},
    {"ldumaxlh", VIXL_AF(&TextAssembler::ldumaxlh)},
    {"stxrh", VIXL_AF(&TextAssembler::stxrh)},
    {"ldsmaxa", VIXL_AF(&TextAssembler::ldsmaxa)},
    {"ldnp", VIXL_AF(&TextAssembler::ldnp)},
    {"ldsminlh", VIXL_AF(&TextAssembler::ldsminlh)},
    {"lduminalb", VIXL_AF(&TextAssembler::lduminalb)},
    {"stxrb", VIXL_AF(&TextAssembler::stxrb)},
    {"swpalb", VIXL_AF(&TextAssembler::swpalb)},
    {"swpalh", VIXL_AF(&TextAssembler::swpalh)},
    {"ldclra", VIXL_AF(&TextAssembler::ldclra)},
    {"ldaddalh", VIXL_AF(&TextAssembler::ldaddalh)},
    {"ldsetalb", VIXL_AF(&TextAssembler::ldsetalb)},
    {"ldsmaxalh", VIXL_AF(&TextAssembler::ldsmaxalh)},
    {"ldeorah", VIXL_AF(&TextAssembler::ldeorah)},
    {"lduminab", VIXL_AF(&TextAssembler::lduminab)},
    {"ldclrlb", VIXL_AF(&TextAssembler::ldclrlb)},
    {"ldeorb", VIXL_AF(&TextAssembler::ldeorb)},
    {"ldeorh", VIXL_AF(&TextAssembler::ldeorh)},
    {"ldsmax", VIXL_AF(&TextAssembler::ldsmax)},
    {"ldumaxlb", VIXL_AF(&TextAssembler::ldumaxlb)},
    {"swpa", VIXL_AF(&TextAssembler::swpa)},
    {"swplh", VIXL_AF(&TextAssembler::swplh)},
    {"ldaddalb", VIXL_AF(&TextAssembler::ldaddalb)},
    {"ldclrb", VIXL_AF(&TextAssembler::ldclrb)},
    {"ldsminab", VIXL_AF(&TextAssembler::ldsminab)},
    {"ldumaxab", VIXL_AF(&TextAssembler::ldumaxab)},
    {"lduminh", VIXL_AF(&TextAssembler::lduminh)},
    {"stlxrb", VIXL_AF(&TextAssembler::stlxrb)},
    {"cash", VIXL_AF(&TextAssembler::cash)},
    {"ldsetb", VIXL_AF(&TextAssembler::ldsetb)},
    {"ldumaxah", VIXL_AF(&TextAssembler::ldumaxah)},
    {"swplb", VIXL_AF(&TextAssembler::swplb)},
    {"ldeoralb", VIXL_AF(&TextAssembler::ldeoralb)},
    {"casalh", VIXL_AF(&TextAssembler::casalh)},
    {"ldsetah", VIXL_AF(&TextAssembler::ldsetah)},
    {"ldsmaxh", VIXL_AF(&TextAssembler::ldsmaxh)},
    {"casab", VIXL_AF(&TextAssembler::casab)},
    {"casb", VIXL_AF(&TextAssembler::casb)},
    {"ldeor", VIXL_AF(&TextAssembler::ldeor)},
    {"ldumaxb", VIXL_AF(&TextAssembler::ldumaxb)},
    {"casalb", VIXL_AF(&TextAssembler::casalb)},
    {"lduminah", VIXL_AF(&TextAssembler::lduminah)},
    {"ldsmaxl", VIXL_AF(&TextAssembler::ldsmaxl)},
    {"stp", VIXL_AF(&TextAssembler::stp)},
    {"casal", VIXL_AF(&TextAssembler::casal)},
    {"ldsetab", VIXL_AF(&TextAssembler::ldsetab)},
    {"ldclrh", VIXL_AF(&TextAssembler::ldclrh)},
    {"ldeoralh", VIXL_AF(&TextAssembler::ldeoralh)},
    {"ldsmina", VIXL_AF(&TextAssembler::ldsmina)},
    {"lduminlb", VIXL_AF(&TextAssembler::lduminlb)},
    {"ldadda", VIXL_AF(&TextAssembler::ldadda)},
    {"ldp", VIXL_AF(&TextAssembler::ldp)},
    {"swpb", VIXL_AF(&TextAssembler::swpb)},
    {"ldaddal", VIXL_AF(&TextAssembler::ldaddal)},
    {"ldaxp", VIXL_AF(&TextAssembler::ldaxp)},
    {"ldsminalb", VIXL_AF(&TextAssembler::ldsminalb)},
    {"stlxr", VIXL_AF(&TextAssembler::stlxr)},
    {"ldeorab", VIXL_AF(&TextAssembler::ldeorab)},
    {"ldseta", VIXL_AF(&TextAssembler::ldseta)},
    {"ldsminh", VIXL_AF(&TextAssembler::ldsminh)},
    {"swp", VIXL_AF(&TextAssembler::swp)},
    {"ldaddah", VIXL_AF(&TextAssembler::ldaddah)},
    {"ldclralb", VIXL_AF(&TextAssembler::ldclralb)},
    {"ldsmaxab", VIXL_AF(&TextAssembler::ldsmaxab)},
    {"ldumaxal", VIXL_AF(&TextAssembler::ldumaxal)},
    {"ldclrl", VIXL_AF(&TextAssembler::ldclrl)},
    {"ldeorl", VIXL_AF(&TextAssembler::ldeorl)},
    {"ldumaxalh", VIXL_AF(&TextAssembler::ldumaxalh)},
    {"swpl", VIXL_AF(&TextAssembler::swpl)},
    {"ldpsw", VIXL_AF(&TextAssembler::ldpsw)},
    {"lduminal", VIXL_AF(&TextAssembler::lduminal)},
    {"ldclrah", VIXL_AF(&TextAssembler::ldclrah)},
    {"ldeoral", VIXL_AF(&TextAssembler::ldeoral)},
    {"casa", VIXL_AF(&TextAssembler::casa)},
    {"ldaddl", VIXL_AF(&TextAssembler::ldaddl)},
    {"ldaddlb", VIXL_AF(&TextAssembler::ldaddlb)},
    {"ldaddlh", VIXL_AF(&TextAssembler::ldaddlh)},
    {"ldsminl", VIXL_AF(&TextAssembler::ldsminl)},
    {"ldsetl", VIXL_AF(&TextAssembler::ldsetl)},
    {"caslb", VIXL_AF(&TextAssembler::caslb)},
    {"ldclr", VIXL_AF(&TextAssembler::ldclr)},
    {"ldsetal", VIXL_AF(&TextAssembler::ldsetal)},
    {"ldsmaxal", VIXL_AF(&TextAssembler::ldsmaxal)},
    {"ldsmaxlh", VIXL_AF(&TextAssembler::ldsmaxlh)},
    {"ldumina", VIXL_AF(&TextAssembler::ldumina)},
    {"ldsmaxah", VIXL_AF(&TextAssembler::ldsmaxah)},
    {"ldumaxl", VIXL_AF(&TextAssembler::ldumaxl)},
    {"lduminl", VIXL_AF(&TextAssembler::lduminl)},
    {"ldxp", VIXL_AF(&TextAssembler::ldxp)},
    {"lduminalh", VIXL_AF(&TextAssembler::lduminalh)},
    {"casl", VIXL_AF(&TextAssembler::casl)},
    {"ldsetalh", VIXL_AF(&TextAssembler::ldsetalh)},
    {"ldsmaxalb", VIXL_AF(&TextAssembler::ldsmaxalb)},
    {"ldeora", VIXL_AF(&TextAssembler::ldeora)},
    {"stlxrh", VIXL_AF(&TextAssembler::stlxrh)},
    {"stxr", VIXL_AF(&TextAssembler::stxr)},
    {"ldsminal", VIXL_AF(&TextAssembler::ldsminal)},
    {"caslh", VIXL_AF(&TextAssembler::caslh)},
    {"ldumax", VIXL_AF(&TextAssembler::ldumax)},
    {"lduminlh", VIXL_AF(&TextAssembler::lduminlh)},
    {"swph", VIXL_AF(&TextAssembler::swph)},
    {"ldadd", VIXL_AF(&TextAssembler::ldadd)},
    {"ldaddb", VIXL_AF(&TextAssembler::ldaddb)},
    {"lduminb", VIXL_AF(&TextAssembler::lduminb)},
    {"ldeorlb", VIXL_AF(&TextAssembler::ldeorlb)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rrrrm_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rrrrm_insts_ = {
    {"casp", VIXL_AF(&TextAssembler::casp)},
    {"caspal", VIXL_AF(&TextAssembler::caspal)},
    {"caspl", VIXL_AF(&TextAssembler::caspl)},
    {"caspa", VIXL_AF(&TextAssembler::caspa)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_roFc_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::roFc_insts_ = {
    {"ccmn", VIXL_AF(&TextAssembler::ccmn)},
    {"ccmp", VIXL_AF(&TextAssembler::ccmp)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rrc_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rrc_insts_ = {
    {"cinc", VIXL_AF(&TextAssembler::cinc)},
    {"cinv", VIXL_AF(&TextAssembler::cinv)},
    {"cneg", VIXL_AF(&TextAssembler::cneg)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rprz_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rprz_insts_ = {
    {"clasta", VIXL_AF(&TextAssembler::clasta)},
    {"clastb", VIXL_AF(&TextAssembler::clastb)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vpvz_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vpvz_insts_ = {
    {"clasta", VIXL_AF(&TextAssembler::clasta)},
    {"clastb", VIXL_AF(&TextAssembler::clastb)},
    {"fadda", VIXL_AF(&TextAssembler::fadda)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zpzz_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zpzz_insts_ = {
    {"clasta", VIXL_AF(&TextAssembler::clasta)},
    {"splice", VIXL_AF(&TextAssembler::splice)},
    {"clastb", VIXL_AF(&TextAssembler::clastb)},
    {"sel", VIXL_AF(&TextAssembler::sel)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvi_insts_ = {
    {"sshll", VIXL_AF(&TextAssembler::sshll)},
    {"cmge", VIXL_AF(&TextAssembler::cmge)},
    {"shll2", VIXL_AF(&TextAssembler::shll2)},
    {"sqshl", VIXL_AF(&TextAssembler::sqshl)},
    {"sqshrn2", VIXL_AF(&TextAssembler::sqshrn2)},
    {"srsra", VIXL_AF(&TextAssembler::srsra)},
    {"ursra", VIXL_AF(&TextAssembler::ursra)},
    {"uqrshrn", VIXL_AF(&TextAssembler::uqrshrn)},
    {"rshrn2", VIXL_AF(&TextAssembler::rshrn2)},
    {"ushll2", VIXL_AF(&TextAssembler::ushll2)},
    {"sri", VIXL_AF(&TextAssembler::sri)},
    {"cmeq", VIXL_AF(&TextAssembler::cmeq)},
    {"scvtf", VIXL_AF(&TextAssembler::scvtf)},
    {"sshr", VIXL_AF(&TextAssembler::sshr)},
    {"shrn2", VIXL_AF(&TextAssembler::shrn2)},
    {"sqrshrn2", VIXL_AF(&TextAssembler::sqrshrn2)},
    {"sqrshrun", VIXL_AF(&TextAssembler::sqrshrun)},
    {"dup", VIXL_AF(&TextAssembler::dup)},
    {"sqshlu", VIXL_AF(&TextAssembler::sqshlu)},
    {"uqshl", VIXL_AF(&TextAssembler::uqshl)},
    {"rshrn", VIXL_AF(&TextAssembler::rshrn)},
    {"sqshrun", VIXL_AF(&TextAssembler::sqshrun)},
    {"sqshrn", VIXL_AF(&TextAssembler::sqshrn)},
    {"cmle", VIXL_AF(&TextAssembler::cmle)},
    {"sqrshrn", VIXL_AF(&TextAssembler::sqrshrn)},
    {"uqshrn", VIXL_AF(&TextAssembler::uqshrn)},
    {"uqshrn2", VIXL_AF(&TextAssembler::uqshrn2)},
    {"sqrshrun2", VIXL_AF(&TextAssembler::sqrshrun2)},
    {"ushll", VIXL_AF(&TextAssembler::ushll)},
    {"srshr", VIXL_AF(&TextAssembler::srshr)},
    {"ssra", VIXL_AF(&TextAssembler::ssra)},
    {"cmgt", VIXL_AF(&TextAssembler::cmgt)},
    {"uqrshrn2", VIXL_AF(&TextAssembler::uqrshrn2)},
    {"sqshrun2", VIXL_AF(&TextAssembler::sqshrun2)},
    {"fcvtzu", VIXL_AF(&TextAssembler::fcvtzu)},
    {"shl", VIXL_AF(&TextAssembler::shl)},
    {"shrn", VIXL_AF(&TextAssembler::shrn)},
    {"sshll2", VIXL_AF(&TextAssembler::sshll2)},
    {"ucvtf", VIXL_AF(&TextAssembler::ucvtf)},
    {"ushr", VIXL_AF(&TextAssembler::ushr)},
    {"cmlt", VIXL_AF(&TextAssembler::cmlt)},
    {"fcvtzs", VIXL_AF(&TextAssembler::fcvtzs)},
    {"shll", VIXL_AF(&TextAssembler::shll)},
    {"urshr", VIXL_AF(&TextAssembler::urshr)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
    {"usra", VIXL_AF(&TextAssembler::usra)},
    {"sli", VIXL_AF(&TextAssembler::sli)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_ro_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::ro_insts_ = {
    {"cmn", VIXL_AF(&TextAssembler::cmn)},
    {"ngc", VIXL_AF(&TextAssembler::ngc)},
    {"neg", VIXL_AF(&TextAssembler::neg)},
    {"tst", VIXL_AF(&TextAssembler::tst)},
    {"negs", VIXL_AF(&TextAssembler::negs)},
    {"cmp", VIXL_AF(&TextAssembler::cmp)},
    {"mvn", VIXL_AF(&TextAssembler::mvn)},
    {"ngcs", VIXL_AF(&TextAssembler::ngcs)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LZzi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::LZzi_insts_ = {
    {"cmple", VIXL_AF(&TextAssembler::cmple)},
    {"cmpeq", VIXL_AF(&TextAssembler::cmpeq)},
    {"cmplt", VIXL_AF(&TextAssembler::cmplt)},
    {"cmpge", VIXL_AF(&TextAssembler::cmpge)},
    {"cmpgt", VIXL_AF(&TextAssembler::cmpgt)},
    {"cmpne", VIXL_AF(&TextAssembler::cmpne)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LZzu_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::LZzu_insts_ = {
    {"cmpls", VIXL_AF(&TextAssembler::cmpls)},
    {"cmphs", VIXL_AF(&TextAssembler::cmphs)},
    {"cmplo", VIXL_AF(&TextAssembler::cmplo)},
    {"cmphi", VIXL_AF(&TextAssembler::cmphi)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LZzz_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::LZzz_insts_ = {
    {"cmpeq", VIXL_AF(&TextAssembler::cmpeq)},
    {"cmple", VIXL_AF(&TextAssembler::cmple)},
    {"cmphi", VIXL_AF(&TextAssembler::cmphi)},
    {"cmpge", VIXL_AF(&TextAssembler::cmpge)},
    {"cmplt", VIXL_AF(&TextAssembler::cmplt)},
    {"facgt", VIXL_AF(&TextAssembler::facgt)},
    {"cmpgt", VIXL_AF(&TextAssembler::cmpgt)},
    {"cmplo", VIXL_AF(&TextAssembler::cmplo)},
    {"cmpls", VIXL_AF(&TextAssembler::cmpls)},
    {"facge", VIXL_AF(&TextAssembler::facge)},
    {"fcmeq", VIXL_AF(&TextAssembler::fcmeq)},
    {"fcmge", VIXL_AF(&TextAssembler::fcmge)},
    {"fcmuo", VIXL_AF(&TextAssembler::fcmuo)},
    {"cmphs", VIXL_AF(&TextAssembler::cmphs)},
    {"fcmgt", VIXL_AF(&TextAssembler::fcmgt)},
    {"cmpne", VIXL_AF(&TextAssembler::cmpne)},
    {"fcmne", VIXL_AF(&TextAssembler::fcmne)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rii_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rii_insts_ = {
    {"cntd", VIXL_AF(&TextAssembler::cntd)},
    {"decw", VIXL_AF(&TextAssembler::decw)},
    {"incb", VIXL_AF(&TextAssembler::incb)},
    {"uqinch", VIXL_AF(&TextAssembler::uqinch)},
    {"incw", VIXL_AF(&TextAssembler::incw)},
    {"sqincd", VIXL_AF(&TextAssembler::sqincd)},
    {"uqdecd", VIXL_AF(&TextAssembler::uqdecd)},
    {"sqinch", VIXL_AF(&TextAssembler::sqinch)},
    {"cnth", VIXL_AF(&TextAssembler::cnth)},
    {"decd", VIXL_AF(&TextAssembler::decd)},
    {"sqdecd", VIXL_AF(&TextAssembler::sqdecd)},
    {"decb", VIXL_AF(&TextAssembler::decb)},
    {"uqdecb", VIXL_AF(&TextAssembler::uqdecb)},
    {"uqincd", VIXL_AF(&TextAssembler::uqincd)},
    {"dech", VIXL_AF(&TextAssembler::dech)},
    {"uqincw", VIXL_AF(&TextAssembler::uqincw)},
    {"uqdecw", VIXL_AF(&TextAssembler::uqdecw)},
    {"cntw", VIXL_AF(&TextAssembler::cntw)},
    {"sqdecb", VIXL_AF(&TextAssembler::sqdecb)},
    {"inch", VIXL_AF(&TextAssembler::inch)},
    {"sqdech", VIXL_AF(&TextAssembler::sqdech)},
    {"sqdecw", VIXL_AF(&TextAssembler::sqdecw)},
    {"sqincw", VIXL_AF(&TextAssembler::sqincw)},
    {"cntb", VIXL_AF(&TextAssembler::cntb)},
    {"uqincb", VIXL_AF(&TextAssembler::uqincb)},
    {"incd", VIXL_AF(&TextAssembler::incd)},
    {"sqincb", VIXL_AF(&TextAssembler::sqincb)},
    {"uqdech", VIXL_AF(&TextAssembler::uqdech)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_ri_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::ri_insts_ = {
    {"cntd", VIXL_AF(&TextAssembler::cntd)},
    {"decw", VIXL_AF(&TextAssembler::decw)},
    {"incb", VIXL_AF(&TextAssembler::incb)},
    {"uqinch", VIXL_AF(&TextAssembler::uqinch)},
    {"incw", VIXL_AF(&TextAssembler::incw)},
    {"sqincd", VIXL_AF(&TextAssembler::sqincd)},
    {"uqdecd", VIXL_AF(&TextAssembler::uqdecd)},
    {"sqinch", VIXL_AF(&TextAssembler::sqinch)},
    {"cnth", VIXL_AF(&TextAssembler::cnth)},
    {"decd", VIXL_AF(&TextAssembler::decd)},
    {"sqdecd", VIXL_AF(&TextAssembler::sqdecd)},
    {"decb", VIXL_AF(&TextAssembler::decb)},
    {"uqdecb", VIXL_AF(&TextAssembler::uqdecb)},
    {"uqincd", VIXL_AF(&TextAssembler::uqincd)},
    {"rdvl", VIXL_AF(&TextAssembler::rdvl)},
    {"dech", VIXL_AF(&TextAssembler::dech)},
    {"uqincw", VIXL_AF(&TextAssembler::uqincw)},
    {"uqdecw", VIXL_AF(&TextAssembler::uqdecw)},
    {"cntw", VIXL_AF(&TextAssembler::cntw)},
    {"sqdecb", VIXL_AF(&TextAssembler::sqdecb)},
    {"inch", VIXL_AF(&TextAssembler::inch)},
    {"sqdech", VIXL_AF(&TextAssembler::sqdech)},
    {"sqdecw", VIXL_AF(&TextAssembler::sqdecw)},
    {"sqincw", VIXL_AF(&TextAssembler::sqincw)},
    {"cntb", VIXL_AF(&TextAssembler::cntb)},
    {"uqincb", VIXL_AF(&TextAssembler::uqincb)},
    {"incd", VIXL_AF(&TextAssembler::incd)},
    {"sqincb", VIXL_AF(&TextAssembler::sqincb)},
    {"uqdech", VIXL_AF(&TextAssembler::uqdech)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rpL_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rpL_insts_ = {
    {"cntp", VIXL_AF(&TextAssembler::cntp)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zpz_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zpz_insts_ = {
    {"compact", VIXL_AF(&TextAssembler::compact)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zZii_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zZii_insts_ = {
    {"cpy", VIXL_AF(&TextAssembler::cpy)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zZi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zZi_insts_ = {
    {"cpy", VIXL_AF(&TextAssembler::cpy)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zMii_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zMii_insts_ = {
    {"cpy", VIXL_AF(&TextAssembler::cpy)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zMi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zMi_insts_ = {
    {"cpy", VIXL_AF(&TextAssembler::cpy)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zMr_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zMr_insts_ = {
    {"cpy", VIXL_AF(&TextAssembler::cpy)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zMv_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zMv_insts_ = {
    {"cpy", VIXL_AF(&TextAssembler::cpy)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rrrc_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rrrc_insts_ = {
    {"csinv", VIXL_AF(&TextAssembler::csinv)},
    {"csel", VIXL_AF(&TextAssembler::csel)},
    {"csneg", VIXL_AF(&TextAssembler::csneg)},
    {"csinc", VIXL_AF(&TextAssembler::csinc)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rc_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rc_insts_ = {
    {"csetm", VIXL_AF(&TextAssembler::csetm)},
    {"cset", VIXL_AF(&TextAssembler::cset)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_Dr_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::Dr_insts_ = {
    {"dc", VIXL_AF(&TextAssembler::dc)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zii_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zii_insts_ = {
    {"decw", VIXL_AF(&TextAssembler::decw)},
    {"uqinch", VIXL_AF(&TextAssembler::uqinch)},
    {"incw", VIXL_AF(&TextAssembler::incw)},
    {"sqincd", VIXL_AF(&TextAssembler::sqincd)},
    {"uqdecd", VIXL_AF(&TextAssembler::uqdecd)},
    {"decd", VIXL_AF(&TextAssembler::decd)},
    {"sqdecd", VIXL_AF(&TextAssembler::sqdecd)},
    {"sqinch", VIXL_AF(&TextAssembler::sqinch)},
    {"uqincd", VIXL_AF(&TextAssembler::uqincd)},
    {"uqincw", VIXL_AF(&TextAssembler::uqincw)},
    {"dup", VIXL_AF(&TextAssembler::dup)},
    {"dech", VIXL_AF(&TextAssembler::dech)},
    {"index", VIXL_AF(&TextAssembler::index)},
    {"uqdecw", VIXL_AF(&TextAssembler::uqdecw)},
    {"sqdech", VIXL_AF(&TextAssembler::sqdech)},
    {"inch", VIXL_AF(&TextAssembler::inch)},
    {"sqdecw", VIXL_AF(&TextAssembler::sqdecw)},
    {"sqincw", VIXL_AF(&TextAssembler::sqincw)},
    {"incd", VIXL_AF(&TextAssembler::incd)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
    {"uqdech", VIXL_AF(&TextAssembler::uqdech)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_z_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::z_insts_ = {
    {"decw", VIXL_AF(&TextAssembler::decw)},
    {"decd", VIXL_AF(&TextAssembler::decd)},
    {"inch", VIXL_AF(&TextAssembler::inch)},
    {"sqdecd", VIXL_AF(&TextAssembler::sqdecd)},
    {"sqdech", VIXL_AF(&TextAssembler::sqdech)},
    {"sqinch", VIXL_AF(&TextAssembler::sqinch)},
    {"incw", VIXL_AF(&TextAssembler::incw)},
    {"sqdecw", VIXL_AF(&TextAssembler::sqdecw)},
    {"sqincd", VIXL_AF(&TextAssembler::sqincd)},
    {"sqincw", VIXL_AF(&TextAssembler::sqincw)},
    {"uqdecd", VIXL_AF(&TextAssembler::uqdecd)},
    {"uqdech", VIXL_AF(&TextAssembler::uqdech)},
    {"uqdecw", VIXL_AF(&TextAssembler::uqdecw)},
    {"uqincd", VIXL_AF(&TextAssembler::uqincd)},
    {"incd", VIXL_AF(&TextAssembler::incd)},
    {"uqincw", VIXL_AF(&TextAssembler::uqincw)},
    {"uqinch", VIXL_AF(&TextAssembler::uqinch)},
    {"dech", VIXL_AF(&TextAssembler::dech)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zi_insts_ = {
    {"decw", VIXL_AF(&TextAssembler::decw)},
    {"decd", VIXL_AF(&TextAssembler::decd)},
    {"inch", VIXL_AF(&TextAssembler::inch)},
    {"sqdecd", VIXL_AF(&TextAssembler::sqdecd)},
    {"sqdech", VIXL_AF(&TextAssembler::sqdech)},
    {"sqinch", VIXL_AF(&TextAssembler::sqinch)},
    {"incw", VIXL_AF(&TextAssembler::incw)},
    {"sqdecw", VIXL_AF(&TextAssembler::sqdecw)},
    {"sqincd", VIXL_AF(&TextAssembler::sqincd)},
    {"sqincw", VIXL_AF(&TextAssembler::sqincw)},
    {"uqdecd", VIXL_AF(&TextAssembler::uqdecd)},
    {"uqdech", VIXL_AF(&TextAssembler::uqdech)},
    {"uqdecw", VIXL_AF(&TextAssembler::uqdecw)},
    {"uqincd", VIXL_AF(&TextAssembler::uqincd)},
    {"incd", VIXL_AF(&TextAssembler::incd)},
    {"uqincw", VIXL_AF(&TextAssembler::uqincw)},
    {"dup", VIXL_AF(&TextAssembler::dup)},
    {"uqinch", VIXL_AF(&TextAssembler::uqinch)},
    {"dech", VIXL_AF(&TextAssembler::dech)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rL_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rL_insts_ = {
    {"sqincp", VIXL_AF(&TextAssembler::sqincp)},
    {"uqdecp", VIXL_AF(&TextAssembler::uqdecp)},
    {"incp", VIXL_AF(&TextAssembler::incp)},
    {"uqincp", VIXL_AF(&TextAssembler::uqincp)},
    {"decp", VIXL_AF(&TextAssembler::decp)},
    {"sqdecp", VIXL_AF(&TextAssembler::sqdecp)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_Bb_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::Bb_insts_ = {
    {"dsb", VIXL_AF(&TextAssembler::dsb)},
    {"dmb", VIXL_AF(&TextAssembler::dmb)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vr_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vr_insts_ = {
    {"ucvtf", VIXL_AF(&TextAssembler::ucvtf)},
    {"fmov", VIXL_AF(&TextAssembler::fmov)},
    {"dup", VIXL_AF(&TextAssembler::dup)},
    {"scvtf", VIXL_AF(&TextAssembler::scvtf)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zr_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zr_insts_ = {
    {"insr", VIXL_AF(&TextAssembler::insr)},
    {"dup", VIXL_AF(&TextAssembler::dup)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzu_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzu_insts_ = {
    {"dup", VIXL_AF(&TextAssembler::dup)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zu64_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zu64_insts_ = {
    {"mov", VIXL_AF(&TextAssembler::mov)},
    {"dupm", VIXL_AF(&TextAssembler::dupm)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvvi_insts_ = {
    {"umlal2", VIXL_AF(&TextAssembler::umlal2)},
    {"sqdmulh", VIXL_AF(&TextAssembler::sqdmulh)},
    {"sqrdmlsh", VIXL_AF(&TextAssembler::sqrdmlsh)},
    {"fmlal", VIXL_AF(&TextAssembler::fmlal)},
    {"ext", VIXL_AF(&TextAssembler::ext)},
    {"umlal", VIXL_AF(&TextAssembler::umlal)},
    {"umlsl2", VIXL_AF(&TextAssembler::umlsl2)},
    {"mls", VIXL_AF(&TextAssembler::mls)},
    {"smlal2", VIXL_AF(&TextAssembler::smlal2)},
    {"umull2", VIXL_AF(&TextAssembler::umull2)},
    {"fcmla", VIXL_AF(&TextAssembler::fcmla)},
    {"umull", VIXL_AF(&TextAssembler::umull)},
    {"sqdmlal", VIXL_AF(&TextAssembler::sqdmlal)},
    {"fmla", VIXL_AF(&TextAssembler::fmla)},
    {"sqdmlal2", VIXL_AF(&TextAssembler::sqdmlal2)},
    {"fmlal2", VIXL_AF(&TextAssembler::fmlal2)},
    {"sqdmull", VIXL_AF(&TextAssembler::sqdmull)},
    {"sdot", VIXL_AF(&TextAssembler::sdot)},
    {"umlsl", VIXL_AF(&TextAssembler::umlsl)},
    {"fmulx", VIXL_AF(&TextAssembler::fmulx)},
    {"sqdmull2", VIXL_AF(&TextAssembler::sqdmull2)},
    {"sqrdmlah", VIXL_AF(&TextAssembler::sqrdmlah)},
    {"fmls", VIXL_AF(&TextAssembler::fmls)},
    {"fmlsl2", VIXL_AF(&TextAssembler::fmlsl2)},
    {"mul", VIXL_AF(&TextAssembler::mul)},
    {"smlsl2", VIXL_AF(&TextAssembler::smlsl2)},
    {"smull", VIXL_AF(&TextAssembler::smull)},
    {"smlsl", VIXL_AF(&TextAssembler::smlsl)},
    {"fmul", VIXL_AF(&TextAssembler::fmul)},
    {"sqdmlsl", VIXL_AF(&TextAssembler::sqdmlsl)},
    {"fcadd", VIXL_AF(&TextAssembler::fcadd)},
    {"smlal", VIXL_AF(&TextAssembler::smlal)},
    {"smull2", VIXL_AF(&TextAssembler::smull2)},
    {"sqdmlsl2", VIXL_AF(&TextAssembler::sqdmlsl2)},
    {"sqrdmulh", VIXL_AF(&TextAssembler::sqrdmulh)},
    {"udot", VIXL_AF(&TextAssembler::udot)},
    {"mla", VIXL_AF(&TextAssembler::mla)},
    {"fmlsl", VIXL_AF(&TextAssembler::fmlsl)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzzu_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzzu_insts_ = {
    {"fmul", VIXL_AF(&TextAssembler::fmul)},
    {"ext", VIXL_AF(&TextAssembler::ext)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rrru_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rrru_insts_ = {
    {"extr", VIXL_AF(&TextAssembler::extr)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zMzd_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zMzd_insts_ = {
    {"fmin", VIXL_AF(&TextAssembler::fmin)},
    {"fminnm", VIXL_AF(&TextAssembler::fminnm)},
    {"fmaxnm", VIXL_AF(&TextAssembler::fmaxnm)},
    {"fmax", VIXL_AF(&TextAssembler::fmax)},
    {"fmul", VIXL_AF(&TextAssembler::fmul)},
    {"fadd", VIXL_AF(&TextAssembler::fadd)},
    {"fsubr", VIXL_AF(&TextAssembler::fsubr)},
    {"fsub", VIXL_AF(&TextAssembler::fsub)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zMzzi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zMzzi_insts_ = {
    {"fcadd", VIXL_AF(&TextAssembler::fcadd)},
    {"fcmla", VIXL_AF(&TextAssembler::fcmla)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvFc_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvFc_insts_ = {
    {"fccmpe", VIXL_AF(&TextAssembler::fccmpe)},
    {"fccmp", VIXL_AF(&TextAssembler::fccmp)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvd_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvd_insts_ = {
    {"fcmle", VIXL_AF(&TextAssembler::fcmle)},
    {"fcmeq", VIXL_AF(&TextAssembler::fcmeq)},
    {"fcmgt", VIXL_AF(&TextAssembler::fcmgt)},
    {"fcmge", VIXL_AF(&TextAssembler::fcmge)},
    {"fcmlt", VIXL_AF(&TextAssembler::fcmlt)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LZzd_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::LZzd_insts_ = {
    {"fcmle", VIXL_AF(&TextAssembler::fcmle)},
    {"fcmne", VIXL_AF(&TextAssembler::fcmne)},
    {"fcmeq", VIXL_AF(&TextAssembler::fcmeq)},
    {"fcmgt", VIXL_AF(&TextAssembler::fcmgt)},
    {"fcmge", VIXL_AF(&TextAssembler::fcmge)},
    {"fcmlt", VIXL_AF(&TextAssembler::fcmlt)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvii_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvvii_insts_ = {
    {"fcmla", VIXL_AF(&TextAssembler::fcmla)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzzii_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzzii_insts_ = {
    {"fcmla", VIXL_AF(&TextAssembler::fcmla)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vd_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vd_insts_ = {
    {"fcmpe", VIXL_AF(&TextAssembler::fcmpe)},
    {"fmov", VIXL_AF(&TextAssembler::fmov)},
    {"fcmp", VIXL_AF(&TextAssembler::fcmp)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zMd_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zMd_insts_ = {
    {"fmov", VIXL_AF(&TextAssembler::fmov)},
    {"fcpy", VIXL_AF(&TextAssembler::fcpy)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zMf16_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zMf16_insts_ = {
    {"fcpy", VIXL_AF(&TextAssembler::fcpy)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvc_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvvc_insts_ = {
    {"fcsel", VIXL_AF(&TextAssembler::fcsel)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rv_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rv_insts_ = {
    {"fmov", VIXL_AF(&TextAssembler::fmov)},
    {"fcvtpu", VIXL_AF(&TextAssembler::fcvtpu)},
    {"fjcvtzs", VIXL_AF(&TextAssembler::fjcvtzs)},
    {"fcvtau", VIXL_AF(&TextAssembler::fcvtau)},
    {"fcvtas", VIXL_AF(&TextAssembler::fcvtas)},
    {"fcvtmu", VIXL_AF(&TextAssembler::fcvtmu)},
    {"fcvtms", VIXL_AF(&TextAssembler::fcvtms)},
    {"fcvtnu", VIXL_AF(&TextAssembler::fcvtnu)},
    {"fcvtzu", VIXL_AF(&TextAssembler::fcvtzu)},
    {"fcvtzs", VIXL_AF(&TextAssembler::fcvtzs)},
    {"fcvtns", VIXL_AF(&TextAssembler::fcvtns)},
    {"fcvtps", VIXL_AF(&TextAssembler::fcvtps)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rvi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rvi_insts_ = {
    {"umov", VIXL_AF(&TextAssembler::umov)},
    {"smov", VIXL_AF(&TextAssembler::smov)},
    {"fcvtzu", VIXL_AF(&TextAssembler::fcvtzu)},
    {"fcvtzs", VIXL_AF(&TextAssembler::fcvtzs)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zd_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zd_insts_ = {
    {"fmov", VIXL_AF(&TextAssembler::fmov)},
    {"fdup", VIXL_AF(&TextAssembler::fdup)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zf16_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zf16_insts_ = {
    {"fdup", VIXL_AF(&TextAssembler::fdup)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zz_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zz_insts_ = {
    {"frsqrte", VIXL_AF(&TextAssembler::frsqrte)},
    {"rev", VIXL_AF(&TextAssembler::rev)},
    {"sunpkhi", VIXL_AF(&TextAssembler::sunpkhi)},
    {"frecpe", VIXL_AF(&TextAssembler::frecpe)},
    {"sunpklo", VIXL_AF(&TextAssembler::sunpklo)},
    {"fexpa", VIXL_AF(&TextAssembler::fexpa)},
    {"movprfx", VIXL_AF(&TextAssembler::movprfx)},
    {"uunpkhi", VIXL_AF(&TextAssembler::uunpkhi)},
    {"uunpklo", VIXL_AF(&TextAssembler::uunpklo)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvv_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvvv_insts_ = {
    {"fmsub", VIXL_AF(&TextAssembler::fmsub)},
    {"fmadd", VIXL_AF(&TextAssembler::fmadd)},
    {"fnmadd", VIXL_AF(&TextAssembler::fnmadd)},
    {"tbl", VIXL_AF(&TextAssembler::tbl)},
    {"tbx", VIXL_AF(&TextAssembler::tbx)},
    {"fnmsub", VIXL_AF(&TextAssembler::fnmsub)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzzi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzzi_insts_ = {
    {"sdot", VIXL_AF(&TextAssembler::sdot)},
    {"fmls", VIXL_AF(&TextAssembler::fmls)},
    {"ftmad", VIXL_AF(&TextAssembler::ftmad)},
    {"udot", VIXL_AF(&TextAssembler::udot)},
    {"fmla", VIXL_AF(&TextAssembler::fmla)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vf_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vf_insts_ = {
    {"fmov", VIXL_AF(&TextAssembler::fmov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vf16_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vf16_insts_ = {
    {"fmov", VIXL_AF(&TextAssembler::fmov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_Ir_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::Ir_insts_ = {
    {"ic", VIXL_AF(&TextAssembler::ic)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zir_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zir_insts_ = {
    {"index", VIXL_AF(&TextAssembler::index)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zri_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zri_insts_ = {
    {"index", VIXL_AF(&TextAssembler::index)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zrr_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zrr_insts_ = {
    {"index", VIXL_AF(&TextAssembler::index)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vivi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vivi_insts_ = {
    {"ins", VIXL_AF(&TextAssembler::ins)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vir_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vir_insts_ = {
    {"ins", VIXL_AF(&TextAssembler::ins)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zv_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zv_insts_ = {
    {"insr", VIXL_AF(&TextAssembler::insr)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rpz_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rpz_insts_ = {
    {"lasta", VIXL_AF(&TextAssembler::lasta)},
    {"lastb", VIXL_AF(&TextAssembler::lastb)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vm_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vm_insts_ = {
    {"ld1r", VIXL_AF(&TextAssembler::ld1r)},
    {"ldr", VIXL_AF(&TextAssembler::ldr)},
    {"ldur", VIXL_AF(&TextAssembler::ldur)},
    {"st1", VIXL_AF(&TextAssembler::st1)},
    {"stur", VIXL_AF(&TextAssembler::stur)},
    {"str", VIXL_AF(&TextAssembler::str)},
    {"ld1", VIXL_AF(&TextAssembler::ld1)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvm_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvm_insts_ = {
    {"ld2r", VIXL_AF(&TextAssembler::ld2r)},
    {"st2", VIXL_AF(&TextAssembler::st2)},
    {"stp", VIXL_AF(&TextAssembler::stp)},
    {"ld2", VIXL_AF(&TextAssembler::ld2)},
    {"ld1", VIXL_AF(&TextAssembler::ld1)},
    {"st1", VIXL_AF(&TextAssembler::st1)},
    {"stnp", VIXL_AF(&TextAssembler::stnp)},
    {"ldnp", VIXL_AF(&TextAssembler::ldnp)},
    {"ldp", VIXL_AF(&TextAssembler::ldp)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvm_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvvm_insts_ = {
    {"ld3", VIXL_AF(&TextAssembler::ld3)},
    {"ld3r", VIXL_AF(&TextAssembler::ld3r)},
    {"st1", VIXL_AF(&TextAssembler::st1)},
    {"ld1", VIXL_AF(&TextAssembler::ld1)},
    {"st3", VIXL_AF(&TextAssembler::st3)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvvm_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvvvm_insts_ = {
    {"ld4r", VIXL_AF(&TextAssembler::ld4r)},
    {"st4", VIXL_AF(&TextAssembler::st4)},
    {"ld4", VIXL_AF(&TextAssembler::ld4)},
    {"st1", VIXL_AF(&TextAssembler::st1)},
    {"ld1", VIXL_AF(&TextAssembler::ld1)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vim_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vim_insts_ = {
    {"st1", VIXL_AF(&TextAssembler::st1)},
    {"ld1", VIXL_AF(&TextAssembler::ld1)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zZs_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zZs_insts_ = {
    {"ld1d", VIXL_AF(&TextAssembler::ld1d)},
    {"ld1rh", VIXL_AF(&TextAssembler::ld1rh)},
    {"ldff1b", VIXL_AF(&TextAssembler::ldff1b)},
    {"ld1rsw", VIXL_AF(&TextAssembler::ld1rsw)},
    {"ld1h", VIXL_AF(&TextAssembler::ld1h)},
    {"ld1sw", VIXL_AF(&TextAssembler::ld1sw)},
    {"ldff1d", VIXL_AF(&TextAssembler::ldff1d)},
    {"ldnf1d", VIXL_AF(&TextAssembler::ldnf1d)},
    {"ld1rd", VIXL_AF(&TextAssembler::ld1rd)},
    {"ld1b", VIXL_AF(&TextAssembler::ld1b)},
    {"ldff1w", VIXL_AF(&TextAssembler::ldff1w)},
    {"ldnt1h", VIXL_AF(&TextAssembler::ldnt1h)},
    {"ld1w", VIXL_AF(&TextAssembler::ld1w)},
    {"ld1rqw", VIXL_AF(&TextAssembler::ld1rqw)},
    {"ld1rsh", VIXL_AF(&TextAssembler::ld1rsh)},
    {"ldnf1b", VIXL_AF(&TextAssembler::ldnf1b)},
    {"ldnf1h", VIXL_AF(&TextAssembler::ldnf1h)},
    {"ldnf1w", VIXL_AF(&TextAssembler::ldnf1w)},
    {"ldnt1b", VIXL_AF(&TextAssembler::ldnt1b)},
    {"ld1sb", VIXL_AF(&TextAssembler::ld1sb)},
    {"ldnt1w", VIXL_AF(&TextAssembler::ldnt1w)},
    {"ldnf1sh", VIXL_AF(&TextAssembler::ldnf1sh)},
    {"ld1sh", VIXL_AF(&TextAssembler::ld1sh)},
    {"ld1rsb", VIXL_AF(&TextAssembler::ld1rsb)},
    {"ldnf1sb", VIXL_AF(&TextAssembler::ldnf1sb)},
    {"ldnf1sw", VIXL_AF(&TextAssembler::ldnf1sw)},
    {"ldff1sw", VIXL_AF(&TextAssembler::ldff1sw)},
    {"ld1rb", VIXL_AF(&TextAssembler::ld1rb)},
    {"ldff1h", VIXL_AF(&TextAssembler::ldff1h)},
    {"ld1rqb", VIXL_AF(&TextAssembler::ld1rqb)},
    {"ld1rqd", VIXL_AF(&TextAssembler::ld1rqd)},
    {"ldff1sh", VIXL_AF(&TextAssembler::ldff1sh)},
    {"ldnt1d", VIXL_AF(&TextAssembler::ldnt1d)},
    {"ldff1sb", VIXL_AF(&TextAssembler::ldff1sb)},
    {"ld1rqh", VIXL_AF(&TextAssembler::ld1rqh)},
    {"ld1rw", VIXL_AF(&TextAssembler::ld1rw)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvim_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvim_insts_ = {
    {"ld2", VIXL_AF(&TextAssembler::ld2)},
    {"st2", VIXL_AF(&TextAssembler::st2)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzZs_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzZs_insts_ = {
    {"ld2w", VIXL_AF(&TextAssembler::ld2w)},
    {"ld2b", VIXL_AF(&TextAssembler::ld2b)},
    {"ld2h", VIXL_AF(&TextAssembler::ld2h)},
    {"ld2d", VIXL_AF(&TextAssembler::ld2d)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvim_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvvim_insts_ = {
    {"ld3", VIXL_AF(&TextAssembler::ld3)},
    {"st3", VIXL_AF(&TextAssembler::st3)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzzZs_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzzZs_insts_ = {
    {"ld3d", VIXL_AF(&TextAssembler::ld3d)},
    {"ld3b", VIXL_AF(&TextAssembler::ld3b)},
    {"ld3h", VIXL_AF(&TextAssembler::ld3h)},
    {"ld3w", VIXL_AF(&TextAssembler::ld3w)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvvim_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvvvim_insts_ = {
    {"ld4", VIXL_AF(&TextAssembler::ld4)},
    {"st4", VIXL_AF(&TextAssembler::st4)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzzzZs_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzzzZs_insts_ = {
    {"ld4w", VIXL_AF(&TextAssembler::ld4w)},
    {"ld4d", VIXL_AF(&TextAssembler::ld4d)},
    {"ld4b", VIXL_AF(&TextAssembler::ld4b)},
    {"ld4h", VIXL_AF(&TextAssembler::ld4h)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rm_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rm_insts_ = {
    {"ldrab", VIXL_AF(&TextAssembler::ldrab)},
    {"ldlarb", VIXL_AF(&TextAssembler::ldlarb)},
    {"ldxrb", VIXL_AF(&TextAssembler::ldxrb)},
    {"ldlar", VIXL_AF(&TextAssembler::ldlar)},
    {"ldlarh", VIXL_AF(&TextAssembler::ldlarh)},
    {"steorl", VIXL_AF(&TextAssembler::steorl)},
    {"stsmaxlh", VIXL_AF(&TextAssembler::stsmaxlh)},
    {"stclrlh", VIXL_AF(&TextAssembler::stclrlh)},
    {"stsetlb", VIXL_AF(&TextAssembler::stsetlb)},
    {"ldapurb", VIXL_AF(&TextAssembler::ldapurb)},
    {"staddl", VIXL_AF(&TextAssembler::staddl)},
    {"steorb", VIXL_AF(&TextAssembler::steorb)},
    {"ldapur", VIXL_AF(&TextAssembler::ldapur)},
    {"ldrh", VIXL_AF(&TextAssembler::ldrh)},
    {"steorh", VIXL_AF(&TextAssembler::steorh)},
    {"stsminlh", VIXL_AF(&TextAssembler::stsminlh)},
    {"ldaxr", VIXL_AF(&TextAssembler::ldaxr)},
    {"stlurb", VIXL_AF(&TextAssembler::stlurb)},
    {"stlrb", VIXL_AF(&TextAssembler::stlrb)},
    {"stclrh", VIXL_AF(&TextAssembler::stclrh)},
    {"stclrl", VIXL_AF(&TextAssembler::stclrl)},
    {"strh", VIXL_AF(&TextAssembler::strh)},
    {"ldrb", VIXL_AF(&TextAssembler::ldrb)},
    {"steor", VIXL_AF(&TextAssembler::steor)},
    {"stlr", VIXL_AF(&TextAssembler::stlr)},
    {"ldrsb", VIXL_AF(&TextAssembler::ldrsb)},
    {"stlrh", VIXL_AF(&TextAssembler::stlrh)},
    {"stseth", VIXL_AF(&TextAssembler::stseth)},
    {"stsmaxlb", VIXL_AF(&TextAssembler::stsmaxlb)},
    {"stsetlh", VIXL_AF(&TextAssembler::stsetlh)},
    {"ldurb", VIXL_AF(&TextAssembler::ldurb)},
    {"stclrb", VIXL_AF(&TextAssembler::stclrb)},
    {"stllrb", VIXL_AF(&TextAssembler::stllrb)},
    {"stsminl", VIXL_AF(&TextAssembler::stsminl)},
    {"ldr", VIXL_AF(&TextAssembler::ldr)},
    {"ldar", VIXL_AF(&TextAssembler::ldar)},
    {"stsminb", VIXL_AF(&TextAssembler::stsminb)},
    {"ldraa", VIXL_AF(&TextAssembler::ldraa)},
    {"stumaxl", VIXL_AF(&TextAssembler::stumaxl)},
    {"stllrh", VIXL_AF(&TextAssembler::stllrh)},
    {"stumaxlh", VIXL_AF(&TextAssembler::stumaxlh)},
    {"ldursh", VIXL_AF(&TextAssembler::ldursh)},
    {"stumax", VIXL_AF(&TextAssembler::stumax)},
    {"stumaxh", VIXL_AF(&TextAssembler::stumaxh)},
    {"stur", VIXL_AF(&TextAssembler::stur)},
    {"ldapursb", VIXL_AF(&TextAssembler::ldapursb)},
    {"ldarb", VIXL_AF(&TextAssembler::ldarb)},
    {"ldaxrh", VIXL_AF(&TextAssembler::ldaxrh)},
    {"ldursb", VIXL_AF(&TextAssembler::ldursb)},
    {"stuminl", VIXL_AF(&TextAssembler::stuminl)},
    {"stuminh", VIXL_AF(&TextAssembler::stuminh)},
    {"ldxr", VIXL_AF(&TextAssembler::ldxr)},
    {"stadd", VIXL_AF(&TextAssembler::stadd)},
    {"stuminlb", VIXL_AF(&TextAssembler::stuminlb)},
    {"stclr", VIXL_AF(&TextAssembler::stclr)},
    {"stsmin", VIXL_AF(&TextAssembler::stsmin)},
    {"ldaprh", VIXL_AF(&TextAssembler::ldaprh)},
    {"ldaxrb", VIXL_AF(&TextAssembler::ldaxrb)},
    {"staddlb", VIXL_AF(&TextAssembler::staddlb)},
    {"str", VIXL_AF(&TextAssembler::str)},
    {"stsmaxb", VIXL_AF(&TextAssembler::stsmaxb)},
    {"stuminb", VIXL_AF(&TextAssembler::stuminb)},
    {"staddb", VIXL_AF(&TextAssembler::staddb)},
    {"ldrsw", VIXL_AF(&TextAssembler::ldrsw)},
    {"stsminlb", VIXL_AF(&TextAssembler::stsminlb)},
    {"stumin", VIXL_AF(&TextAssembler::stumin)},
    {"strb", VIXL_AF(&TextAssembler::strb)},
    {"ldapurh", VIXL_AF(&TextAssembler::ldapurh)},
    {"staddlh", VIXL_AF(&TextAssembler::staddlh)},
    {"stsmax", VIXL_AF(&TextAssembler::stsmax)},
    {"stumaxb", VIXL_AF(&TextAssembler::stumaxb)},
    {"ldxrh", VIXL_AF(&TextAssembler::ldxrh)},
    {"ldarh", VIXL_AF(&TextAssembler::ldarh)},
    {"sturb", VIXL_AF(&TextAssembler::sturb)},
    {"stsmaxh", VIXL_AF(&TextAssembler::stsmaxh)},
    {"ldrsh", VIXL_AF(&TextAssembler::ldrsh)},
    {"ldursw", VIXL_AF(&TextAssembler::ldursw)},
    {"stlur", VIXL_AF(&TextAssembler::stlur)},
    {"stsetl", VIXL_AF(&TextAssembler::stsetl)},
    {"sturh", VIXL_AF(&TextAssembler::sturh)},
    {"stsmaxl", VIXL_AF(&TextAssembler::stsmaxl)},
    {"stumaxlb", VIXL_AF(&TextAssembler::stumaxlb)},
    {"stlurh", VIXL_AF(&TextAssembler::stlurh)},
    {"stuminlh", VIXL_AF(&TextAssembler::stuminlh)},
    {"ldapr", VIXL_AF(&TextAssembler::ldapr)},
    {"ldurh", VIXL_AF(&TextAssembler::ldurh)},
    {"stllr", VIXL_AF(&TextAssembler::stllr)},
    {"stsetb", VIXL_AF(&TextAssembler::stsetb)},
    {"ldapursh", VIXL_AF(&TextAssembler::ldapursh)},
    {"staddh", VIXL_AF(&TextAssembler::staddh)},
    {"stset", VIXL_AF(&TextAssembler::stset)},
    {"stsminh", VIXL_AF(&TextAssembler::stsminh)},
    {"steorlh", VIXL_AF(&TextAssembler::steorlh)},
    {"ldapursw", VIXL_AF(&TextAssembler::ldapursw)},
    {"ldaprb", VIXL_AF(&TextAssembler::ldaprb)},
    {"stclrlb", VIXL_AF(&TextAssembler::stclrlb)},
    {"steorlb", VIXL_AF(&TextAssembler::steorlb)},
    {"ldur", VIXL_AF(&TextAssembler::ldur)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vi64_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vi64_insts_ = {
    {"ldr", VIXL_AF(&TextAssembler::ldr)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_pm_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::pm_insts_ = {
    {"str", VIXL_AF(&TextAssembler::str)},
    {"ldr", VIXL_AF(&TextAssembler::ldr)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_ps_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::ps_insts_ = {
    {"str", VIXL_AF(&TextAssembler::str)},
    {"ldr", VIXL_AF(&TextAssembler::ldr)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zm_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zm_insts_ = {
    {"str", VIXL_AF(&TextAssembler::str)},
    {"ldr", VIXL_AF(&TextAssembler::ldr)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rrrr_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rrrr_insts_ = {
    {"umaddl", VIXL_AF(&TextAssembler::umaddl)},
    {"smsubl", VIXL_AF(&TextAssembler::smsubl)},
    {"madd", VIXL_AF(&TextAssembler::madd)},
    {"msub", VIXL_AF(&TextAssembler::msub)},
    {"umsubl", VIXL_AF(&TextAssembler::umsubl)},
    {"smaddl", VIXL_AF(&TextAssembler::smaddl)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_ru64_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::ru64_insts_ = {
    {"movz", VIXL_AF(&TextAssembler::movz)},
    {"movk", VIXL_AF(&TextAssembler::movk)},
    {"movn", VIXL_AF(&TextAssembler::movn)},
    {"mov", VIXL_AF(&TextAssembler::mov)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vu64Si_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vu64Si_insts_ = {
    {"movi", VIXL_AF(&TextAssembler::movi)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vu64_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vu64_insts_ = {
    {"movi", VIXL_AF(&TextAssembler::movi)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_ru64i_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::ru64i_insts_ = {
    {"movz", VIXL_AF(&TextAssembler::movz)},
    {"movk", VIXL_AF(&TextAssembler::movk)},
    {"movn", VIXL_AF(&TextAssembler::movn)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zZz_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zZz_insts_ = {
    {"movprfx", VIXL_AF(&TextAssembler::movprfx)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rR_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rR_insts_ = {
    {"mrs", VIXL_AF(&TextAssembler::mrs)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_Rr_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::Rr_insts_ = {
    {"msr", VIXL_AF(&TextAssembler::msr)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_viSi_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::viSi_insts_ = {
    {"mvni", VIXL_AF(&TextAssembler::mvni)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_L_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::L_insts_ = {
    {"wrffr", VIXL_AF(&TextAssembler::wrffr)},
    {"pfalse", VIXL_AF(&TextAssembler::pfalse)},
    {"ptrue", VIXL_AF(&TextAssembler::ptrue)},
    {"ptrues", VIXL_AF(&TextAssembler::ptrues)},
    {"rdffr", VIXL_AF(&TextAssembler::rdffr)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LpL_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::LpL_insts_ = {
    {"pnext", VIXL_AF(&TextAssembler::pnext)},
    {"pfirst", VIXL_AF(&TextAssembler::pfirst)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_Pps_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::Pps_insts_ = {
    {"prfw", VIXL_AF(&TextAssembler::prfw)},
    {"prfd", VIXL_AF(&TextAssembler::prfd)},
    {"prfb", VIXL_AF(&TextAssembler::prfb)},
    {"prfh", VIXL_AF(&TextAssembler::prfh)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_Pm_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::Pm_insts_ = {
    {"prfum", VIXL_AF(&TextAssembler::prfum)},
    {"prfm", VIXL_AF(&TextAssembler::prfm)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_im_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::im_insts_ = {
    {"prfum", VIXL_AF(&TextAssembler::prfum)},
    {"prfm", VIXL_AF(&TextAssembler::prfm)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_Pi64_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::Pi64_insts_ = {
    {"prfm", VIXL_AF(&TextAssembler::prfm)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_ii64_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::ii64_insts_ = {
    {"prfm", VIXL_AF(&TextAssembler::prfm)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_pL_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::pL_insts_ = {
    {"ptest", VIXL_AF(&TextAssembler::ptest)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_Li_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::Li_insts_ = {
    {"ptrues", VIXL_AF(&TextAssembler::ptrues)},
    {"ptrue", VIXL_AF(&TextAssembler::ptrue)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LL_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::LL_insts_ = {
    {"punpkhi", VIXL_AF(&TextAssembler::punpkhi)},
    {"rev", VIXL_AF(&TextAssembler::rev)},
    {"punpklo", VIXL_AF(&TextAssembler::punpklo)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LZ_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::LZ_insts_ = {
    {"rdffrs", VIXL_AF(&TextAssembler::rdffrs)},
    {"rdffr", VIXL_AF(&TextAssembler::rdffr)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_ruF_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::ruF_insts_ = {
    {"rmif", VIXL_AF(&TextAssembler::rmif)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vri_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vri_insts_ = {
    {"ucvtf", VIXL_AF(&TextAssembler::ucvtf)},
    {"scvtf", VIXL_AF(&TextAssembler::scvtf)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LpLL_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::LpLL_insts_ = {
    {"sel", VIXL_AF(&TextAssembler::sel)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rrii_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rrii_insts_ = {
    {"sqdech", VIXL_AF(&TextAssembler::sqdech)},
    {"sqdecd", VIXL_AF(&TextAssembler::sqdecd)},
    {"sqdecb", VIXL_AF(&TextAssembler::sqdecb)},
    {"sqinch", VIXL_AF(&TextAssembler::sqinch)},
    {"sqdecw", VIXL_AF(&TextAssembler::sqdecw)},
    {"sqincd", VIXL_AF(&TextAssembler::sqincd)},
    {"sqincw", VIXL_AF(&TextAssembler::sqincw)},
    {"sqincb", VIXL_AF(&TextAssembler::sqincb)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rLr_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rLr_insts_ = {
    {"sqincp", VIXL_AF(&TextAssembler::sqincp)},
    {"sqdecp", VIXL_AF(&TextAssembler::sqdecp)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zps_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zps_insts_ = {
    {"stnt1d", VIXL_AF(&TextAssembler::stnt1d)},
    {"st1h", VIXL_AF(&TextAssembler::st1h)},
    {"stnt1w", VIXL_AF(&TextAssembler::stnt1w)},
    {"stnt1b", VIXL_AF(&TextAssembler::stnt1b)},
    {"st1d", VIXL_AF(&TextAssembler::st1d)},
    {"stnt1h", VIXL_AF(&TextAssembler::stnt1h)},
    {"st1w", VIXL_AF(&TextAssembler::st1w)},
    {"st1b", VIXL_AF(&TextAssembler::st1b)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzps_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzps_insts_ = {
    {"st2h", VIXL_AF(&TextAssembler::st2h)},
    {"st2w", VIXL_AF(&TextAssembler::st2w)},
    {"st2d", VIXL_AF(&TextAssembler::st2d)},
    {"st2b", VIXL_AF(&TextAssembler::st2b)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzzps_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzzps_insts_ = {
    {"st3b", VIXL_AF(&TextAssembler::st3b)},
    {"st3h", VIXL_AF(&TextAssembler::st3h)},
    {"st3w", VIXL_AF(&TextAssembler::st3w)},
    {"st3d", VIXL_AF(&TextAssembler::st3d)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_zzzzps_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::zzzzps_insts_ = {
    {"st4h", VIXL_AF(&TextAssembler::st4h)},
    {"st4b", VIXL_AF(&TextAssembler::st4b)},
    {"st4w", VIXL_AF(&TextAssembler::st4w)},
    {"st4d", VIXL_AF(&TextAssembler::st4d)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rrrm_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rrrm_insts_ = {
    {"stxp", VIXL_AF(&TextAssembler::stxp)},
    {"stlxp", VIXL_AF(&TextAssembler::stlxp)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_iiii_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::iiii_insts_ = {
    {"sys", VIXL_AF(&TextAssembler::sys)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_iiiir_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::iiiir_insts_ = {
    {"sys", VIXL_AF(&TextAssembler::sys)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvvv_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvvvv_insts_ = {
    {"tbx", VIXL_AF(&TextAssembler::tbx)},
    {"tbl", VIXL_AF(&TextAssembler::tbl)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_vvvvvv_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::vvvvvv_insts_ = {
    {"tbx", VIXL_AF(&TextAssembler::tbx)},
    {"tbl", VIXL_AF(&TextAssembler::tbl)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rul_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rul_insts_ = {
    {"tbnz", VIXL_AF(&TextAssembler::tbnz)},
    {"tbz", VIXL_AF(&TextAssembler::tbz)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_rui64_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::rui64_insts_ = {
    {"tbnz", VIXL_AF(&TextAssembler::tbnz)},
    {"tbz", VIXL_AF(&TextAssembler::tbz)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_LLL_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::LLL_insts_ = {
    {"trn2", VIXL_AF(&TextAssembler::trn2)},
    {"zip2", VIXL_AF(&TextAssembler::zip2)},
    {"trn1", VIXL_AF(&TextAssembler::trn1)},
    {"zip1", VIXL_AF(&TextAssembler::zip1)},
    {"uzp2", VIXL_AF(&TextAssembler::uzp2)},
    {"uzp1", VIXL_AF(&TextAssembler::uzp1)},
};
#undef VIXL_AF

#define VIXL_AF(F) AssemblerFn(static_cast<VIXL_Lrr_Fn>(F))
const std::map<std::string, AssemblerFn> TextAssembler::Lrr_insts_ = {
    {"whilelt", VIXL_AF(&TextAssembler::whilelt)},
    {"whilelo", VIXL_AF(&TextAssembler::whilelo)},
    {"whilels", VIXL_AF(&TextAssembler::whilels)},
    {"whilele", VIXL_AF(&TextAssembler::whilele)},
};
#undef VIXL_AF

// Instruction prototype is a signature of the function handling that
// instruction, for example
//  * rro -> Register, Register, Operand
//  * vvv-> VRegister, VRegister, VRegister
//  * ru -> Register, uint64_t
const std::map<std::string, std::map<std::string, AssemblerFn> >
    TextAssembler::prototypes_ = {
        {"vv", vv_insts_},
        {"zMz", zMz_insts_},
        {"rro", rro_insts_},
        {"vvv", vvv_insts_},
        {"zMzz", zMzz_insts_},
        {"zzi", zzi_insts_},
        {"zzii", zzii_insts_},
        {"zzz", zzz_insts_},
        {"rri", rri_insts_},
        {"rl", rl_insts_},
        {"ri64", ri64_insts_},
        {"zs", zs_insts_},
        {"LZLL", LZLL_insts_},
        {"zzu64", zzu64_insts_},
        {"vpz", vpz_insts_},
        {"rru", rru_insts_},
        {"zMzi", zMzi_insts_},
        {"rrr", rrr_insts_},
        {"rr", rr_insts_},
        {"r", r_insts_},
        {"", _insts_},
        {"lc", lc_insts_},
        {"i64c", i64c_insts_},
        {"l", l_insts_},
        {"i64", i64_insts_},
        {"ruu", ruu_insts_},
        {"rruu", rruu_insts_},
        {"vi", vi_insts_},
        {"vii", vii_insts_},
        {"i", i_insts_},
        {"LML", LML_insts_},
        {"LZL", LZL_insts_},
        {"T", T_insts_},
        {"rrm", rrm_insts_},
        {"rrrrm", rrrrm_insts_},
        {"roFc", roFc_insts_},
        {"rrc", rrc_insts_},
        {"rprz", rprz_insts_},
        {"vpvz", vpvz_insts_},
        {"zpzz", zpzz_insts_},
        {"vvi", vvi_insts_},
        {"ro", ro_insts_},
        {"LZzi", LZzi_insts_},
        {"LZzu", LZzu_insts_},
        {"LZzz", LZzz_insts_},
        {"rii", rii_insts_},
        {"ri", ri_insts_},
        {"rpL", rpL_insts_},
        {"zpz", zpz_insts_},
        {"zZii", zZii_insts_},
        {"zZi", zZi_insts_},
        {"zMii", zMii_insts_},
        {"zMi", zMi_insts_},
        {"zMr", zMr_insts_},
        {"zMv", zMv_insts_},
        {"rrrc", rrrc_insts_},
        {"rc", rc_insts_},
        {"Dr", Dr_insts_},
        {"zii", zii_insts_},
        {"z", z_insts_},
        {"zi", zi_insts_},
        {"rL", rL_insts_},
        {"Bb", Bb_insts_},
        {"vr", vr_insts_},
        {"zr", zr_insts_},
        {"zzu", zzu_insts_},
        {"zu64", zu64_insts_},
        {"vvvi", vvvi_insts_},
        {"zzzu", zzzu_insts_},
        {"rrru", rrru_insts_},
        {"zMzd", zMzd_insts_},
        {"zMzzi", zMzzi_insts_},
        {"vvFc", vvFc_insts_},
        {"vvd", vvd_insts_},
        {"LZzd", LZzd_insts_},
        {"vvvii", vvvii_insts_},
        {"zzzii", zzzii_insts_},
        {"vd", vd_insts_},
        {"zMd", zMd_insts_},
        {"zMf16", zMf16_insts_},
        {"vvvc", vvvc_insts_},
        {"rv", rv_insts_},
        {"rvi", rvi_insts_},
        {"zd", zd_insts_},
        {"zf16", zf16_insts_},
        {"zz", zz_insts_},
        {"vvvv", vvvv_insts_},
        {"zzzi", zzzi_insts_},
        {"vf", vf_insts_},
        {"vf16", vf16_insts_},
        {"Ir", Ir_insts_},
        {"zir", zir_insts_},
        {"zri", zri_insts_},
        {"zrr", zrr_insts_},
        {"vivi", vivi_insts_},
        {"vir", vir_insts_},
        {"zv", zv_insts_},
        {"rpz", rpz_insts_},
        {"vm", vm_insts_},
        {"vvm", vvm_insts_},
        {"vvvm", vvvm_insts_},
        {"vvvvm", vvvvm_insts_},
        {"vim", vim_insts_},
        {"zZs", zZs_insts_},
        {"vvim", vvim_insts_},
        {"zzZs", zzZs_insts_},
        {"vvvim", vvvim_insts_},
        {"zzzZs", zzzZs_insts_},
        {"vvvvim", vvvvim_insts_},
        {"zzzzZs", zzzzZs_insts_},
        {"rm", rm_insts_},
        {"vi64", vi64_insts_},
        {"pm", pm_insts_},
        {"ps", ps_insts_},
        {"zm", zm_insts_},
        {"rrrr", rrrr_insts_},
        {"ru64", ru64_insts_},
        {"vu64Si", vu64Si_insts_},
        {"vu64", vu64_insts_},
        {"ru64i", ru64i_insts_},
        {"zZz", zZz_insts_},
        {"rR", rR_insts_},
        {"Rr", Rr_insts_},
        {"viSi", viSi_insts_},
        {"L", L_insts_},
        {"LpL", LpL_insts_},
        {"Pps", Pps_insts_},
        {"Pm", Pm_insts_},
        {"im", im_insts_},
        {"Pi64", Pi64_insts_},
        {"ii64", ii64_insts_},
        {"pL", pL_insts_},
        {"Li", Li_insts_},
        {"LL", LL_insts_},
        {"LZ", LZ_insts_},
        {"ruF", ruF_insts_},
        {"vri", vri_insts_},
        {"LpLL", LpLL_insts_},
        {"rrii", rrii_insts_},
        {"rLr", rLr_insts_},
        {"zps", zps_insts_},
        {"zzps", zzps_insts_},
        {"zzzps", zzzps_insts_},
        {"zzzzps", zzzzps_insts_},
        {"rrrm", rrrm_insts_},
        {"iiii", iiii_insts_},
        {"iiiir", iiiir_insts_},
        {"vvvvv", vvvvv_insts_},
        {"vvvvvv", vvvvvv_insts_},
        {"rul", rul_insts_},
        {"rui64", rui64_insts_},
        {"LLL", LLL_insts_},
        {"Lrr", Lrr_insts_},
};
}
}
}  // namespace vixl::aarch64::tasm

#endif  // VIXL_TASM_INSTRUCTIONS_H