// Copyright 2016, VIXL authors
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of ARM Limited nor the names of its contributors may
//     be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "aarch32/constants-aarch32.h"
#include "utils-vixl.h"

namespace vixl {
namespace aarch32 {

// Start of generated code.
const char* ToCString(InstructionType type) {
  switch (type) {
    case kAdc:
      return "adc";
    case kAdcs:
      return "adcs";
    case kAdd:
      return "add";
    case kAdds:
      return "adds";
    case kAddw:
      return "addw";
    case kAdr:
      return "adr";
    case kAesd:
      return "aesd";
    case kAese:
      return "aese";
    case kAesimc:
      return "aesimc";
    case kAesmc:
      return "aesmc";
    case kAnd:
      return "and";
    case kAnds:
      return "ands";
    case kAsr:
      return "asr";
    case kAsrs:
      return "asrs";
    case kB:
      return "b";
    case kBfc:
      return "bfc";
    case kBfi:
      return "bfi";
    case kBic:
      return "bic";
    case kBics:
      return "bics";
    case kBkpt:
      return "bkpt";
    case kBl:
      return "bl";
    case kBlx:
      return "blx";
    case kBx:
      return "bx";
    case kBxj:
      return "bxj";
    case kCbnz:
      return "cbnz";
    case kCbz:
      return "cbz";
    case kClrex:
      return "clrex";
    case kClz:
      return "clz";
    case kCmn:
      return "cmn";
    case kCmp:
      return "cmp";
    case kCrc32b:
      return "crc32b";
    case kCrc32cb:
      return "crc32cb";
    case kCrc32ch:
      return "crc32ch";
    case kCrc32cw:
      return "crc32cw";
    case kCrc32h:
      return "crc32h";
    case kCrc32w:
      return "crc32w";
    case kDmb:
      return "dmb";
    case kDsb:
      return "dsb";
    case kEor:
      return "eor";
    case kEors:
      return "eors";
    case kFldmdbx:
      return "fldmdbx";
    case kFldmiax:
      return "fldmiax";
    case kFstmdbx:
      return "fstmdbx";
    case kFstmiax:
      return "fstmiax";
    case kHlt:
      return "hlt";
    case kHvc:
      return "hvc";
    case kIsb:
      return "isb";
    case kIt:
      return "it";
    case kLda:
      return "lda";
    case kLdab:
      return "ldab";
    case kLdaex:
      return "ldaex";
    case kLdaexb:
      return "ldaexb";
    case kLdaexd:
      return "ldaexd";
    case kLdaexh:
      return "ldaexh";
    case kLdah:
      return "ldah";
    case kLdm:
      return "ldm";
    case kLdmda:
      return "ldmda";
    case kLdmdb:
      return "ldmdb";
    case kLdmea:
      return "ldmea";
    case kLdmed:
      return "ldmed";
    case kLdmfa:
      return "ldmfa";
    case kLdmfd:
      return "ldmfd";
    case kLdmib:
      return "ldmib";
    case kLdr:
      return "ldr";
    case kLdrb:
      return "ldrb";
    case kLdrd:
      return "ldrd";
    case kLdrex:
      return "ldrex";
    case kLdrexb:
      return "ldrexb";
    case kLdrexd:
      return "ldrexd";
    case kLdrexh:
      return "ldrexh";
    case kLdrh:
      return "ldrh";
    case kLdrsb:
      return "ldrsb";
    case kLdrsh:
      return "ldrsh";
    case kLsl:
      return "lsl";
    case kLsls:
      return "lsls";
    case kLsr:
      return "lsr";
    case kLsrs:
      return "lsrs";
    case kMla:
      return "mla";
    case kMlas:
      return "mlas";
    case kMls:
      return "mls";
    case kMov:
      return "mov";
    case kMovs:
      return "movs";
    case kMovt:
      return "movt";
    case kMovw:
      return "movw";
    case kMrs:
      return "mrs";
    case kMsr:
      return "msr";
    case kMul:
      return "mul";
    case kMuls:
      return "muls";
    case kMvn:
      return "mvn";
    case kMvns:
      return "mvns";
    case kNop:
      return "nop";
    case kOrn:
      return "orn";
    case kOrns:
      return "orns";
    case kOrr:
      return "orr";
    case kOrrs:
      return "orrs";
    case kPkhbt:
      return "pkhbt";
    case kPkhtb:
      return "pkhtb";
    case kPld:
      return "pld";
    case kPldw:
      return "pldw";
    case kPli:
      return "pli";
    case kPop:
      return "pop";
    case kPush:
      return "push";
    case kQadd:
      return "qadd";
    case kQadd16:
      return "qadd16";
    case kQadd8:
      return "qadd8";
    case kQasx:
      return "qasx";
    case kQdadd:
      return "qdadd";
    case kQdsub:
      return "qdsub";
    case kQsax:
      return "qsax";
    case kQsub:
      return "qsub";
    case kQsub16:
      return "qsub16";
    case kQsub8:
      return "qsub8";
    case kRbit:
      return "rbit";
    case kRev:
      return "rev";
    case kRev16:
      return "rev16";
    case kRevsh:
      return "revsh";
    case kRor:
      return "ror";
    case kRors:
      return "rors";
    case kRrx:
      return "rrx";
    case kRrxs:
      return "rrxs";
    case kRsb:
      return "rsb";
    case kRsbs:
      return "rsbs";
    case kRsc:
      return "rsc";
    case kRscs:
      return "rscs";
    case kSadd16:
      return "sadd16";
    case kSadd8:
      return "sadd8";
    case kSasx:
      return "sasx";
    case kSbc:
      return "sbc";
    case kSbcs:
      return "sbcs";
    case kSbfx:
      return "sbfx";
    case kSdiv:
      return "sdiv";
    case kSel:
      return "sel";
    case kShadd16:
      return "shadd16";
    case kShadd8:
      return "shadd8";
    case kShasx:
      return "shasx";
    case kShsax:
      return "shsax";
    case kShsub16:
      return "shsub16";
    case kShsub8:
      return "shsub8";
    case kSmlabb:
      return "smlabb";
    case kSmlabt:
      return "smlabt";
    case kSmlad:
      return "smlad";
    case kSmladx:
      return "smladx";
    case kSmlal:
      return "smlal";
    case kSmlalbb:
      return "smlalbb";
    case kSmlalbt:
      return "smlalbt";
    case kSmlald:
      return "smlald";
    case kSmlaldx:
      return "smlaldx";
    case kSmlals:
      return "smlals";
    case kSmlaltb:
      return "smlaltb";
    case kSmlaltt:
      return "smlaltt";
    case kSmlatb:
      return "smlatb";
    case kSmlatt:
      return "smlatt";
    case kSmlawb:
      return "smlawb";
    case kSmlawt:
      return "smlawt";
    case kSmlsd:
      return "smlsd";
    case kSmlsdx:
      return "smlsdx";
    case kSmlsld:
      return "smlsld";
    case kSmlsldx:
      return "smlsldx";
    case kSmmla:
      return "smmla";
    case kSmmlar:
      return "smmlar";
    case kSmmls:
      return "smmls";
    case kSmmlsr:
      return "smmlsr";
    case kSmmul:
      return "smmul";
    case kSmmulr:
      return "smmulr";
    case kSmuad:
      return "smuad";
    case kSmuadx:
      return "smuadx";
    case kSmulbb:
      return "smulbb";
    case kSmulbt:
      return "smulbt";
    case kSmull:
      return "smull";
    case kSmulls:
      return "smulls";
    case kSmultb:
      return "smultb";
    case kSmultt:
      return "smultt";
    case kSmulwb:
      return "smulwb";
    case kSmulwt:
      return "smulwt";
    case kSmusd:
      return "smusd";
    case kSmusdx:
      return "smusdx";
    case kSsat:
      return "ssat";
    case kSsat16:
      return "ssat16";
    case kSsax:
      return "ssax";
    case kSsub16:
      return "ssub16";
    case kSsub8:
      return "ssub8";
    case kStl:
      return "stl";
    case kStlb:
      return "stlb";
    case kStlex:
      return "stlex";
    case kStlexb:
      return "stlexb";
    case kStlexd:
      return "stlexd";
    case kStlexh:
      return "stlexh";
    case kStlh:
      return "stlh";
    case kStm:
      return "stm";
    case kStmda:
      return "stmda";
    case kStmdb:
      return "stmdb";
    case kStmea:
      return "stmea";
    case kStmed:
      return "stmed";
    case kStmfa:
      return "stmfa";
    case kStmfd:
      return "stmfd";
    case kStmib:
      return "stmib";
    case kStr:
      return "str";
    case kStrb:
      return "strb";
    case kStrd:
      return "strd";
    case kStrex:
      return "strex";
    case kStrexb:
      return "strexb";
    case kStrexd:
      return "strexd";
    case kStrexh:
      return "strexh";
    case kStrh:
      return "strh";
    case kSub:
      return "sub";
    case kSubs:
      return "subs";
    case kSubw:
      return "subw";
    case kSvc:
      return "svc";
    case kSxtab:
      return "sxtab";
    case kSxtab16:
      return "sxtab16";
    case kSxtah:
      return "sxtah";
    case kSxtb:
      return "sxtb";
    case kSxtb16:
      return "sxtb16";
    case kSxth:
      return "sxth";
    case kTbb:
      return "tbb";
    case kTbh:
      return "tbh";
    case kTeq:
      return "teq";
    case kTst:
      return "tst";
    case kUadd16:
      return "uadd16";
    case kUadd8:
      return "uadd8";
    case kUasx:
      return "uasx";
    case kUbfx:
      return "ubfx";
    case kUdf:
      return "udf";
    case kUdiv:
      return "udiv";
    case kUhadd16:
      return "uhadd16";
    case kUhadd8:
      return "uhadd8";
    case kUhasx:
      return "uhasx";
    case kUhsax:
      return "uhsax";
    case kUhsub16:
      return "uhsub16";
    case kUhsub8:
      return "uhsub8";
    case kUmaal:
      return "umaal";
    case kUmlal:
      return "umlal";
    case kUmlals:
      return "umlals";
    case kUmull:
      return "umull";
    case kUmulls:
      return "umulls";
    case kUqadd16:
      return "uqadd16";
    case kUqadd8:
      return "uqadd8";
    case kUqasx:
      return "uqasx";
    case kUqsax:
      return "uqsax";
    case kUqsub16:
      return "uqsub16";
    case kUqsub8:
      return "uqsub8";
    case kUsad8:
      return "usad8";
    case kUsada8:
      return "usada8";
    case kUsat:
      return "usat";
    case kUsat16:
      return "usat16";
    case kUsax:
      return "usax";
    case kUsub16:
      return "usub16";
    case kUsub8:
      return "usub8";
    case kUxtab:
      return "uxtab";
    case kUxtab16:
      return "uxtab16";
    case kUxtah:
      return "uxtah";
    case kUxtb:
      return "uxtb";
    case kUxtb16:
      return "uxtb16";
    case kUxth:
      return "uxth";
    case kVaba:
      return "vaba";
    case kVabal:
      return "vabal";
    case kVabd:
      return "vabd";
    case kVabdl:
      return "vabdl";
    case kVabs:
      return "vabs";
    case kVacge:
      return "vacge";
    case kVacgt:
      return "vacgt";
    case kVacle:
      return "vacle";
    case kVaclt:
      return "vaclt";
    case kVadd:
      return "vadd";
    case kVaddhn:
      return "vaddhn";
    case kVaddl:
      return "vaddl";
    case kVaddw:
      return "vaddw";
    case kVand:
      return "vand";
    case kVbic:
      return "vbic";
    case kVbif:
      return "vbif";
    case kVbit:
      return "vbit";
    case kVbsl:
      return "vbsl";
    case kVceq:
      return "vceq";
    case kVcge:
      return "vcge";
    case kVcgt:
      return "vcgt";
    case kVcle:
      return "vcle";
    case kVcls:
      return "vcls";
    case kVclt:
      return "vclt";
    case kVclz:
      return "vclz";
    case kVcmp:
      return "vcmp";
    case kVcmpe:
      return "vcmpe";
    case kVcnt:
      return "vcnt";
    case kVcvt:
      return "vcvt";
    case kVcvta:
      return "vcvta";
    case kVcvtb:
      return "vcvtb";
    case kVcvtm:
      return "vcvtm";
    case kVcvtn:
      return "vcvtn";
    case kVcvtp:
      return "vcvtp";
    case kVcvtr:
      return "vcvtr";
    case kVcvtt:
      return "vcvtt";
    case kVdiv:
      return "vdiv";
    case kVdup:
      return "vdup";
    case kVeor:
      return "veor";
    case kVext:
      return "vext";
    case kVfma:
      return "vfma";
    case kVfms:
      return "vfms";
    case kVfnma:
      return "vfnma";
    case kVfnms:
      return "vfnms";
    case kVhadd:
      return "vhadd";
    case kVhsub:
      return "vhsub";
    case kVld1:
      return "vld1";
    case kVld2:
      return "vld2";
    case kVld3:
      return "vld3";
    case kVld4:
      return "vld4";
    case kVldm:
      return "vldm";
    case kVldmdb:
      return "vldmdb";
    case kVldmia:
      return "vldmia";
    case kVldr:
      return "vldr";
    case kVmax:
      return "vmax";
    case kVmaxnm:
      return "vmaxnm";
    case kVmin:
      return "vmin";
    case kVminnm:
      return "vminnm";
    case kVmla:
      return "vmla";
    case kVmlal:
      return "vmlal";
    case kVmls:
      return "vmls";
    case kVmlsl:
      return "vmlsl";
    case kVmov:
      return "vmov";
    case kVmovl:
      return "vmovl";
    case kVmovn:
      return "vmovn";
    case kVmrs:
      return "vmrs";
    case kVmsr:
      return "vmsr";
    case kVmul:
      return "vmul";
    case kVmull:
      return "vmull";
    case kVmvn:
      return "vmvn";
    case kVneg:
      return "vneg";
    case kVnmla:
      return "vnmla";
    case kVnmls:
      return "vnmls";
    case kVnmul:
      return "vnmul";
    case kVorn:
      return "vorn";
    case kVorr:
      return "vorr";
    case kVpadal:
      return "vpadal";
    case kVpadd:
      return "vpadd";
    case kVpaddl:
      return "vpaddl";
    case kVpmax:
      return "vpmax";
    case kVpmin:
      return "vpmin";
    case kVpop:
      return "vpop";
    case kVpush:
      return "vpush";
    case kVqabs:
      return "vqabs";
    case kVqadd:
      return "vqadd";
    case kVqdmlal:
      return "vqdmlal";
    case kVqdmlsl:
      return "vqdmlsl";
    case kVqdmulh:
      return "vqdmulh";
    case kVqdmull:
      return "vqdmull";
    case kVqmovn:
      return "vqmovn";
    case kVqmovun:
      return "vqmovun";
    case kVqneg:
      return "vqneg";
    case kVqrdmulh:
      return "vqrdmulh";
    case kVqrshl:
      return "vqrshl";
    case kVqrshrn:
      return "vqrshrn";
    case kVqrshrun:
      return "vqrshrun";
    case kVqshl:
      return "vqshl";
    case kVqshlu:
      return "vqshlu";
    case kVqshrn:
      return "vqshrn";
    case kVqshrun:
      return "vqshrun";
    case kVqsub:
      return "vqsub";
    case kVraddhn:
      return "vraddhn";
    case kVrecpe:
      return "vrecpe";
    case kVrecps:
      return "vrecps";
    case kVrev16:
      return "vrev16";
    case kVrev32:
      return "vrev32";
    case kVrev64:
      return "vrev64";
    case kVrhadd:
      return "vrhadd";
    case kVrinta:
      return "vrinta";
    case kVrintm:
      return "vrintm";
    case kVrintn:
      return "vrintn";
    case kVrintp:
      return "vrintp";
    case kVrintr:
      return "vrintr";
    case kVrintx:
      return "vrintx";
    case kVrintz:
      return "vrintz";
    case kVrshl:
      return "vrshl";
    case kVrshr:
      return "vrshr";
    case kVrshrn:
      return "vrshrn";
    case kVrsqrte:
      return "vrsqrte";
    case kVrsqrts:
      return "vrsqrts";
    case kVrsra:
      return "vrsra";
    case kVrsubhn:
      return "vrsubhn";
    case kVseleq:
      return "vseleq";
    case kVselge:
      return "vselge";
    case kVselgt:
      return "vselgt";
    case kVselvs:
      return "vselvs";
    case kVshl:
      return "vshl";
    case kVshll:
      return "vshll";
    case kVshr:
      return "vshr";
    case kVshrn:
      return "vshrn";
    case kVsli:
      return "vsli";
    case kVsqrt:
      return "vsqrt";
    case kVsra:
      return "vsra";
    case kVsri:
      return "vsri";
    case kVst1:
      return "vst1";
    case kVst2:
      return "vst2";
    case kVst3:
      return "vst3";
    case kVst4:
      return "vst4";
    case kVstm:
      return "vstm";
    case kVstmdb:
      return "vstmdb";
    case kVstmia:
      return "vstmia";
    case kVstr:
      return "vstr";
    case kVsub:
      return "vsub";
    case kVsubhn:
      return "vsubhn";
    case kVsubl:
      return "vsubl";
    case kVsubw:
      return "vsubw";
    case kVswp:
      return "vswp";
    case kVtbl:
      return "vtbl";
    case kVtbx:
      return "vtbx";
    case kVtrn:
      return "vtrn";
    case kVtst:
      return "vtst";
    case kVuzp:
      return "vuzp";
    case kVzip:
      return "vzip";
    case kYield:
      return "yield";
    case kUndefInstructionType:
      VIXL_UNREACHABLE();
      return "";
  }
  VIXL_UNREACHABLE();
  return "";
}  // NOLINT(readability/fn_size)
// End of generated code.

}  // namespace aarch32
}  // namespace vixl
