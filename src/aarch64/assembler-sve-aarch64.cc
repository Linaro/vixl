// Copyright 2019, VIXL authors
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

#include "assembler-aarch64.h"

namespace vixl {
namespace aarch64 {

// SVEAddressGeneration.

// This prototype maps to 3 instruction encodings:
//  ADR_z_az_d_s32_scaled
//  ADR_z_az_d_u32_scaled
//  ADR_z_az_sd_same_scaled
void Assembler::adr(const ZRegister& zd,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // ADR <Zd>.D, [<Zn>.D, <Zm>.D, SXTW{ <amount>}]
  //  0000 0100 001. .... 1010 .... .... ....
  //  opc<23:22> = 00 | Zm<20:16> | msz<11:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ADR_z_az_d_s32_scaled | Rd(zd) | Rn(zn) | Rm(zm));
}

// SVEBitwiseImm.

void Assembler::and_(const ZRegister& zd, const ZRegister& zn) {
  // AND <Zdn>.<T>, <Zdn>.<T>, #<const>
  //  0000 0101 1000 00.. .... .... .... ....
  //  opc<23:22> = 10 | imm13<17:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));

  Emit(AND_z_zi | Rd(zd));
}

void Assembler::dupm(const ZRegister& zd) {
  // DUPM <Zd>.<T>, #<const>
  //  0000 0101 1100 00.. .... .... .... ....
  //  imm13<17:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(DUPM_z_i | Rd(zd));
}

void Assembler::eor(const ZRegister& zd, const ZRegister& zn) {
  // EOR <Zdn>.<T>, <Zdn>.<T>, #<const>
  //  0000 0101 0100 00.. .... .... .... ....
  //  opc<23:22> = 01 | imm13<17:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));

  Emit(EOR_z_zi | Rd(zd));
}

void Assembler::orr(const ZRegister& zd, const ZRegister& zn) {
  // ORR <Zdn>.<T>, <Zdn>.<T>, #<const>
  //  0000 0101 0000 00.. .... .... .... ....
  //  opc<23:22> = 00 | imm13<17:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));

  Emit(ORR_z_zi | Rd(zd));
}

// SVEBitwiseLogicalUnpredicated.

void Assembler::and_(const ZRegister& zd,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // AND <Zd>.D, <Zn>.D, <Zm>.D
  //  0000 0100 001. .... 0011 00.. .... ....
  //  opc<23:22> = 00 | Zm<20:16> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(AND_z_zz | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::bic(const ZRegister& zd,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // BIC <Zd>.D, <Zn>.D, <Zm>.D
  //  0000 0100 111. .... 0011 00.. .... ....
  //  opc<23:22> = 11 | Zm<20:16> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(BIC_z_zz | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::eor(const ZRegister& zd,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // EOR <Zd>.D, <Zn>.D, <Zm>.D
  //  0000 0100 101. .... 0011 00.. .... ....
  //  opc<23:22> = 10 | Zm<20:16> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(EOR_z_zz | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::orr(const ZRegister& zd,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // ORR <Zd>.D, <Zn>.D, <Zm>.D
  //  0000 0100 011. .... 0011 00.. .... ....
  //  opc<23:22> = 01 | Zm<20:16> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ORR_z_zz | Rd(zd) | Rn(zn) | Rm(zm));
}

// SVEBitwiseShiftPredicated.

void Assembler::asr(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn) {
  // ASR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, #<const>
  //  0000 0100 ..00 0000 100. .... .... ....
  //  tszh<23:22> | opc<19:18> = 00 | L<17> = 0 | U<16> = 0 | Pg<12:10> |
  //  tszl<9:8> | imm3<7:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));

  Emit(ASR_z_p_zi | Rd(zd) | Rx<12, 10>(pg));
}

// This prototype maps to 2 instruction encodings:
//  ASR_z_p_zw
//  ASR_z_p_zz
void Assembler::asr(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // ASR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D
  //  0000 0100 ..01 1000 100. .... .... ....
  //  size<23:22> | R<18> = 0 | L<17> = 0 | U<16> = 0 | Pg<12:10> | Zm<9:5> |
  //  Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(ASR_z_p_zw | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::asrd(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // ASRD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, #<const>
  //  0000 0100 ..00 0100 100. .... .... ....
  //  tszh<23:22> | opc<19:18> = 01 | L<17> = 0 | U<16> = 0 | Pg<12:10> |
  //  tszl<9:8> | imm3<7:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));

  Emit(ASRD_z_p_zi | Rd(zd) | Rx<12, 10>(pg));
}

void Assembler::asrr(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // ASRR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 0100 100. .... .... ....
  //  size<23:22> | R<18> = 1 | L<17> = 0 | U<16> = 0 | Pg<12:10> | Zm<9:5> |
  //  Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(ASRR_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::lsl(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn) {
  // LSL <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, #<const>
  //  0000 0100 ..00 0011 100. .... .... ....
  //  tszh<23:22> | opc<19:18> = 00 | L<17> = 1 | U<16> = 1 | Pg<12:10> |
  //  tszl<9:8> | imm3<7:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));

  Emit(LSL_z_p_zi | Rd(zd) | Rx<12, 10>(pg));
}

// This prototype maps to 2 instruction encodings:
//  LSL_z_p_zw
//  LSL_z_p_zz
void Assembler::lsl(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // LSL <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D
  //  0000 0100 ..01 1011 100. .... .... ....
  //  size<23:22> | R<18> = 0 | L<17> = 1 | U<16> = 1 | Pg<12:10> | Zm<9:5> |
  //  Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(LSL_z_p_zw | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::lslr(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // LSLR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 0111 100. .... .... ....
  //  size<23:22> | R<18> = 1 | L<17> = 1 | U<16> = 1 | Pg<12:10> | Zm<9:5> |
  //  Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(LSLR_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::lsr(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn) {
  // LSR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, #<const>
  //  0000 0100 ..00 0001 100. .... .... ....
  //  tszh<23:22> | opc<19:18> = 00 | L<17> = 0 | U<16> = 1 | Pg<12:10> |
  //  tszl<9:8> | imm3<7:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));

  Emit(LSR_z_p_zi | Rd(zd) | Rx<12, 10>(pg));
}

// This prototype maps to 2 instruction encodings:
//  LSR_z_p_zw
//  LSR_z_p_zz
void Assembler::lsr(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // LSR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.D
  //  0000 0100 ..01 1001 100. .... .... ....
  //  size<23:22> | R<18> = 0 | L<17> = 0 | U<16> = 1 | Pg<12:10> | Zm<9:5> |
  //  Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(LSR_z_p_zw | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::lsrr(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // LSRR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 0101 100. .... .... ....
  //  size<23:22> | R<18> = 1 | L<17> = 0 | U<16> = 1 | Pg<12:10> | Zm<9:5> |
  //  Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(LSRR_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

// SVEBitwiseShiftUnpredicated.

void Assembler::asr(const ZRegister& zd, const ZRegister& zn) {
  // ASR <Zd>.<T>, <Zn>.<T>, #<const>
  //  0000 0100 ..1. .... 1001 00.. .... ....
  //  tszh<23:22> | tszl<20:19> | imm3<18:16> | opc<11:10> = 00 | Zn<9:5> |
  //  Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ASR_z_zi | Rd(zd) | Rn(zn));
}

void Assembler::asr(const ZRegister& zd,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // ASR <Zd>.<T>, <Zn>.<T>, <Zm>.D
  //  0000 0100 ..1. .... 1000 00.. .... ....
  //  size<23:22> | Zm<20:16> | opc<11:10> = 00 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(ASR_z_zw | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::lsl(const ZRegister& zd, const ZRegister& zn) {
  // LSL <Zd>.<T>, <Zn>.<T>, #<const>
  //  0000 0100 ..1. .... 1001 11.. .... ....
  //  tszh<23:22> | tszl<20:19> | imm3<18:16> | opc<11:10> = 11 | Zn<9:5> |
  //  Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LSL_z_zi | Rd(zd) | Rn(zn));
}

void Assembler::lsl(const ZRegister& zd,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // LSL <Zd>.<T>, <Zn>.<T>, <Zm>.D
  //  0000 0100 ..1. .... 1000 11.. .... ....
  //  size<23:22> | Zm<20:16> | opc<11:10> = 11 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(LSL_z_zw | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::lsr(const ZRegister& zd, const ZRegister& zn) {
  // LSR <Zd>.<T>, <Zn>.<T>, #<const>
  //  0000 0100 ..1. .... 1001 01.. .... ....
  //  tszh<23:22> | tszl<20:19> | imm3<18:16> | opc<11:10> = 01 | Zn<9:5> |
  //  Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LSR_z_zi | Rd(zd) | Rn(zn));
}

void Assembler::lsr(const ZRegister& zd,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // LSR <Zd>.<T>, <Zn>.<T>, <Zm>.D
  //  0000 0100 ..1. .... 1000 01.. .... ....
  //  size<23:22> | Zm<20:16> | opc<11:10> = 01 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(LSR_z_zw | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

// SVEElementCount.

void Assembler::cntb(const Register& rd, int pattern) {
  // CNTB <Xd>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0010 .... 1110 00.. .... ....
  //  size<23:22> = 00 | imm4<19:16> | op<10> = 0 | pattern<9:5> | Rd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CNTB_r_s | Rd(rd) | ImmField<9, 5>(pattern));
}

void Assembler::cntd(const Register& rd, int pattern) {
  // CNTD <Xd>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1110 .... 1110 00.. .... ....
  //  size<23:22> = 11 | imm4<19:16> | op<10> = 0 | pattern<9:5> | Rd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CNTD_r_s | Rd(rd) | ImmField<9, 5>(pattern));
}

void Assembler::cnth(const Register& rd, int pattern) {
  // CNTH <Xd>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0110 .... 1110 00.. .... ....
  //  size<23:22> = 01 | imm4<19:16> | op<10> = 0 | pattern<9:5> | Rd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CNTH_r_s | Rd(rd) | ImmField<9, 5>(pattern));
}

void Assembler::cntw(const Register& rd, int pattern) {
  // CNTW <Xd>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1010 .... 1110 00.. .... ....
  //  size<23:22> = 10 | imm4<19:16> | op<10> = 0 | pattern<9:5> | Rd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CNTW_r_s | Rd(rd) | ImmField<9, 5>(pattern));
}

void Assembler::decb(const Register& rdn, int pattern) {
  // DECB <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0011 .... 1110 01.. .... ....
  //  size<23:22> = 00 | imm4<19:16> | D<10> = 1 | pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(DECB_r_rs | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::decd(const Register& rdn, int pattern) {
  // DECD <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1111 .... 1110 01.. .... ....
  //  size<23:22> = 11 | imm4<19:16> | D<10> = 1 | pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(DECD_r_rs | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::decd(const ZRegister& zdn, int pattern) {
  // DECD <Zdn>.D{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1111 .... 1100 01.. .... ....
  //  size<23:22> = 11 | imm4<19:16> | D<10> = 1 | pattern<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(DECD_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

void Assembler::dech(const Register& rdn, int pattern) {
  // DECH <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0111 .... 1110 01.. .... ....
  //  size<23:22> = 01 | imm4<19:16> | D<10> = 1 | pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(DECH_r_rs | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::dech(const ZRegister& zdn, int pattern) {
  // DECH <Zdn>.H{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0111 .... 1100 01.. .... ....
  //  size<23:22> = 01 | imm4<19:16> | D<10> = 1 | pattern<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(DECH_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

void Assembler::decw(const Register& rdn, int pattern) {
  // DECW <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1011 .... 1110 01.. .... ....
  //  size<23:22> = 10 | imm4<19:16> | D<10> = 1 | pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(DECW_r_rs | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::decw(const ZRegister& zdn, int pattern) {
  // DECW <Zdn>.S{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1011 .... 1100 01.. .... ....
  //  size<23:22> = 10 | imm4<19:16> | D<10> = 1 | pattern<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(DECW_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

void Assembler::incb(const Register& rdn, int pattern) {
  // INCB <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0011 .... 1110 00.. .... ....
  //  size<23:22> = 00 | imm4<19:16> | D<10> = 0 | pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INCB_r_rs | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::incd(const Register& rdn, int pattern) {
  // INCD <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1111 .... 1110 00.. .... ....
  //  size<23:22> = 11 | imm4<19:16> | D<10> = 0 | pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INCD_r_rs | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::incd(const ZRegister& zdn, int pattern) {
  // INCD <Zdn>.D{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1111 .... 1100 00.. .... ....
  //  size<23:22> = 11 | imm4<19:16> | D<10> = 0 | pattern<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INCD_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

void Assembler::inch(const Register& rdn, int pattern) {
  // INCH <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0111 .... 1110 00.. .... ....
  //  size<23:22> = 01 | imm4<19:16> | D<10> = 0 | pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INCH_r_rs | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::inch(const ZRegister& zdn, int pattern) {
  // INCH <Zdn>.H{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0111 .... 1100 00.. .... ....
  //  size<23:22> = 01 | imm4<19:16> | D<10> = 0 | pattern<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INCH_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

void Assembler::incw(const Register& rdn, int pattern) {
  // INCW <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1011 .... 1110 00.. .... ....
  //  size<23:22> = 10 | imm4<19:16> | D<10> = 0 | pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INCW_r_rs | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::incw(const ZRegister& zdn, int pattern) {
  // INCW <Zdn>.S{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1011 .... 1100 00.. .... ....
  //  size<23:22> = 10 | imm4<19:16> | D<10> = 0 | pattern<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INCW_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqdecb(const Register& rd, const Register& wn, int pattern) {
  // SQDECB <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0010 .... 1111 10.. .... ....
  //  size<23:22> = 00 | sf<20> = 0 | imm4<19:16> | D<11> = 1 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  USE(wn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(rd.Is(wn));

  Emit(SQDECB_r_rs_sx | Rd(rd) | ImmField<9, 5>(pattern));
}

void Assembler::sqdecb(const Register& rdn, int pattern) {
  // SQDECB <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0011 .... 1111 10.. .... ....
  //  size<23:22> = 00 | sf<20> = 1 | imm4<19:16> | D<11> = 1 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQDECB_r_rs_x | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqdecd(const Register& rd, const Register& wn, int pattern) {
  // SQDECD <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1110 .... 1111 10.. .... ....
  //  size<23:22> = 11 | sf<20> = 0 | imm4<19:16> | D<11> = 1 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  USE(wn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(rd.Is(wn));

  Emit(SQDECD_r_rs_sx | Rd(rd) | ImmField<9, 5>(pattern));
}

void Assembler::sqdecd(const Register& rdn, int pattern) {
  // SQDECD <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1111 .... 1111 10.. .... ....
  //  size<23:22> = 11 | sf<20> = 1 | imm4<19:16> | D<11> = 1 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQDECD_r_rs_x | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqdecd(const ZRegister& zdn, int pattern) {
  // SQDECD <Zdn>.D{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1110 .... 1100 10.. .... ....
  //  size<23:22> = 11 | imm4<19:16> | D<11> = 1 | U<10> = 0 | pattern<9:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQDECD_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqdech(const Register& rd, const Register& wn, int pattern) {
  // SQDECH <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0110 .... 1111 10.. .... ....
  //  size<23:22> = 01 | sf<20> = 0 | imm4<19:16> | D<11> = 1 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  USE(wn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(rd.Is(wn));

  Emit(SQDECH_r_rs_sx | Rd(rd) | ImmField<9, 5>(pattern));
}

void Assembler::sqdech(const Register& rdn, int pattern) {
  // SQDECH <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0111 .... 1111 10.. .... ....
  //  size<23:22> = 01 | sf<20> = 1 | imm4<19:16> | D<11> = 1 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQDECH_r_rs_x | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqdech(const ZRegister& zdn, int pattern) {
  // SQDECH <Zdn>.H{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0110 .... 1100 10.. .... ....
  //  size<23:22> = 01 | imm4<19:16> | D<11> = 1 | U<10> = 0 | pattern<9:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQDECH_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqdecw(const Register& rd, const Register& wn, int pattern) {
  // SQDECW <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1010 .... 1111 10.. .... ....
  //  size<23:22> = 10 | sf<20> = 0 | imm4<19:16> | D<11> = 1 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  USE(wn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(rd.Is(wn));

  Emit(SQDECW_r_rs_sx | Rd(rd) | ImmField<9, 5>(pattern));
}

void Assembler::sqdecw(const Register& rdn, int pattern) {
  // SQDECW <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1011 .... 1111 10.. .... ....
  //  size<23:22> = 10 | sf<20> = 1 | imm4<19:16> | D<11> = 1 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQDECW_r_rs_x | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqdecw(const ZRegister& zdn, int pattern) {
  // SQDECW <Zdn>.S{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1010 .... 1100 10.. .... ....
  //  size<23:22> = 10 | imm4<19:16> | D<11> = 1 | U<10> = 0 | pattern<9:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQDECW_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqincb(const Register& rd, const Register& wn, int pattern) {
  // SQINCB <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0010 .... 1111 00.. .... ....
  //  size<23:22> = 00 | sf<20> = 0 | imm4<19:16> | D<11> = 0 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  USE(wn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(rd.Is(wn));

  Emit(SQINCB_r_rs_sx | Rd(rd) | ImmField<9, 5>(pattern));
}

void Assembler::sqincb(const Register& rdn, int pattern) {
  // SQINCB <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0011 .... 1111 00.. .... ....
  //  size<23:22> = 00 | sf<20> = 1 | imm4<19:16> | D<11> = 0 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQINCB_r_rs_x | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqincd(const Register& rd, const Register& wn, int pattern) {
  // SQINCD <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1110 .... 1111 00.. .... ....
  //  size<23:22> = 11 | sf<20> = 0 | imm4<19:16> | D<11> = 0 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  USE(wn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(rd.Is(wn));

  Emit(SQINCD_r_rs_sx | Rd(rd) | ImmField<9, 5>(pattern));
}

void Assembler::sqincd(const Register& rdn, int pattern) {
  // SQINCD <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1111 .... 1111 00.. .... ....
  //  size<23:22> = 11 | sf<20> = 1 | imm4<19:16> | D<11> = 0 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQINCD_r_rs_x | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqincd(const ZRegister& zdn, int pattern) {
  // SQINCD <Zdn>.D{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1110 .... 1100 00.. .... ....
  //  size<23:22> = 11 | imm4<19:16> | D<11> = 0 | U<10> = 0 | pattern<9:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQINCD_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqinch(const Register& rd, const Register& wn, int pattern) {
  // SQINCH <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0110 .... 1111 00.. .... ....
  //  size<23:22> = 01 | sf<20> = 0 | imm4<19:16> | D<11> = 0 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  USE(wn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(rd.Is(wn));

  Emit(SQINCH_r_rs_sx | Rd(rd) | ImmField<9, 5>(pattern));
}

void Assembler::sqinch(const Register& rdn, int pattern) {
  // SQINCH <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0111 .... 1111 00.. .... ....
  //  size<23:22> = 01 | sf<20> = 1 | imm4<19:16> | D<11> = 0 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQINCH_r_rs_x | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqinch(const ZRegister& zdn, int pattern) {
  // SQINCH <Zdn>.H{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0110 .... 1100 00.. .... ....
  //  size<23:22> = 01 | imm4<19:16> | D<11> = 0 | U<10> = 0 | pattern<9:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQINCH_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqincw(const Register& rd, const Register& wn, int pattern) {
  // SQINCW <Xdn>, <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1010 .... 1111 00.. .... ....
  //  size<23:22> = 10 | sf<20> = 0 | imm4<19:16> | D<11> = 0 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  USE(wn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(rd.Is(wn));

  Emit(SQINCW_r_rs_sx | Rd(rd) | ImmField<9, 5>(pattern));
}

void Assembler::sqincw(const Register& rdn, int pattern) {
  // SQINCW <Xdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1011 .... 1111 00.. .... ....
  //  size<23:22> = 10 | sf<20> = 1 | imm4<19:16> | D<11> = 0 | U<10> = 0 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQINCW_r_rs_x | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::sqincw(const ZRegister& zdn, int pattern) {
  // SQINCW <Zdn>.S{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1010 .... 1100 00.. .... ....
  //  size<23:22> = 10 | imm4<19:16> | D<11> = 0 | U<10> = 0 | pattern<9:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQINCW_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

// This prototype maps to 2 instruction encodings:
//  UQDECB_r_rs_uw
//  UQDECB_r_rs_x
void Assembler::uqdecb(const Register& rdn, int pattern) {
  // UQDECB <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0010 .... 1111 11.. .... ....
  //  size<23:22> = 00 | sf<20> = 0 | imm4<19:16> | D<11> = 1 | U<10> = 1 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQDECB_r_rs_uw | Rd(rdn) | ImmField<9, 5>(pattern));
}

// This prototype maps to 2 instruction encodings:
//  UQDECD_r_rs_uw
//  UQDECD_r_rs_x
void Assembler::uqdecd(const Register& rdn, int pattern) {
  // UQDECD <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1110 .... 1111 11.. .... ....
  //  size<23:22> = 11 | sf<20> = 0 | imm4<19:16> | D<11> = 1 | U<10> = 1 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQDECD_r_rs_uw | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::uqdecd(const ZRegister& zdn, int pattern) {
  // UQDECD <Zdn>.D{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1110 .... 1100 11.. .... ....
  //  size<23:22> = 11 | imm4<19:16> | D<11> = 1 | U<10> = 1 | pattern<9:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQDECD_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

// This prototype maps to 2 instruction encodings:
//  UQDECH_r_rs_uw
//  UQDECH_r_rs_x
void Assembler::uqdech(const Register& rdn, int pattern) {
  // UQDECH <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0110 .... 1111 11.. .... ....
  //  size<23:22> = 01 | sf<20> = 0 | imm4<19:16> | D<11> = 1 | U<10> = 1 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQDECH_r_rs_uw | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::uqdech(const ZRegister& zdn, int pattern) {
  // UQDECH <Zdn>.H{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0110 .... 1100 11.. .... ....
  //  size<23:22> = 01 | imm4<19:16> | D<11> = 1 | U<10> = 1 | pattern<9:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQDECH_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

// This prototype maps to 2 instruction encodings:
//  UQDECW_r_rs_uw
//  UQDECW_r_rs_x
void Assembler::uqdecw(const Register& rdn, int pattern) {
  // UQDECW <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1010 .... 1111 11.. .... ....
  //  size<23:22> = 10 | sf<20> = 0 | imm4<19:16> | D<11> = 1 | U<10> = 1 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQDECW_r_rs_uw | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::uqdecw(const ZRegister& zdn, int pattern) {
  // UQDECW <Zdn>.S{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1010 .... 1100 11.. .... ....
  //  size<23:22> = 10 | imm4<19:16> | D<11> = 1 | U<10> = 1 | pattern<9:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQDECW_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

// This prototype maps to 2 instruction encodings:
//  UQINCB_r_rs_uw
//  UQINCB_r_rs_x
void Assembler::uqincb(const Register& rdn, int pattern) {
  // UQINCB <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0010 .... 1111 01.. .... ....
  //  size<23:22> = 00 | sf<20> = 0 | imm4<19:16> | D<11> = 0 | U<10> = 1 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQINCB_r_rs_uw | Rd(rdn) | ImmField<9, 5>(pattern));
}

// This prototype maps to 2 instruction encodings:
//  UQINCD_r_rs_uw
//  UQINCD_r_rs_x
void Assembler::uqincd(const Register& rdn, int pattern) {
  // UQINCD <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1110 .... 1111 01.. .... ....
  //  size<23:22> = 11 | sf<20> = 0 | imm4<19:16> | D<11> = 0 | U<10> = 1 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQINCD_r_rs_uw | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::uqincd(const ZRegister& zdn, int pattern) {
  // UQINCD <Zdn>.D{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1110 .... 1100 01.. .... ....
  //  size<23:22> = 11 | imm4<19:16> | D<11> = 0 | U<10> = 1 | pattern<9:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQINCD_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

// This prototype maps to 2 instruction encodings:
//  UQINCH_r_rs_uw
//  UQINCH_r_rs_x
void Assembler::uqinch(const Register& rdn, int pattern) {
  // UQINCH <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0110 .... 1111 01.. .... ....
  //  size<23:22> = 01 | sf<20> = 0 | imm4<19:16> | D<11> = 0 | U<10> = 1 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQINCH_r_rs_uw | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::uqinch(const ZRegister& zdn, int pattern) {
  // UQINCH <Zdn>.H{, <pattern>{, MUL #<imm>}}
  //  0000 0100 0110 .... 1100 01.. .... ....
  //  size<23:22> = 01 | imm4<19:16> | D<11> = 0 | U<10> = 1 | pattern<9:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQINCH_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

// This prototype maps to 2 instruction encodings:
//  UQINCW_r_rs_uw
//  UQINCW_r_rs_x
void Assembler::uqincw(const Register& rdn, int pattern) {
  // UQINCW <Wdn>{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1010 .... 1111 01.. .... ....
  //  size<23:22> = 10 | sf<20> = 0 | imm4<19:16> | D<11> = 0 | U<10> = 1 |
  //  pattern<9:5> | Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQINCW_r_rs_uw | Rd(rdn) | ImmField<9, 5>(pattern));
}

void Assembler::uqincw(const ZRegister& zdn, int pattern) {
  // UQINCW <Zdn>.S{, <pattern>{, MUL #<imm>}}
  //  0000 0100 1010 .... 1100 01.. .... ....
  //  size<23:22> = 10 | imm4<19:16> | D<11> = 0 | U<10> = 1 | pattern<9:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQINCW_z_zs | Rd(zdn) | ImmField<9, 5>(pattern));
}

// SVEFPAccumulatingReduction.

void Assembler::fadda(const VRegister& vd,
                      const PRegister& pg,
                      const VRegister& vn,
                      const ZRegister& zm) {
  // FADDA <V><dn>, <Pg>, <V><dn>, <Zm>.<T>
  //  0110 0101 ..01 1000 001. .... .... ....
  //  size<23:22> | opc<18:16> = 000 | Pg<12:10> | Zm<9:5> | Vdn<4:0>

  USE(vn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.Is(vn));
  VIXL_ASSERT(vd.IsScalar());
  VIXL_ASSERT(vn.IsScalar());
  VIXL_ASSERT(zm.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FADDA_v_p_z | SVESize(zm) | Rd(vd) | Rx<12, 10>(pg) | Rn(zm));
}

// SVEFPArithmeticPredicated.

void Assembler::fabd(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // FABD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 1000 100. .... .... ....
  //  size<23:22> | opc<19:16> = 1000 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FABD_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::fadd(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // FADD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
  //  0110 0101 ..01 1000 100. ..00 00.. ....
  //  size<23:22> | opc<18:16> = 000 | Pg<12:10> | i1<5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FADD_z_p_zs | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg));
}

void Assembler::fadd(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // FADD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 0000 100. .... .... ....
  //  size<23:22> | opc<19:16> = 0000 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FADD_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::fdiv(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // FDIV <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 1101 100. .... .... ....
  //  size<23:22> | opc<19:16> = 1101 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FDIV_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::fdivr(const ZRegister& zd,
                      const PRegisterM& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FDIVR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 1100 100. .... .... ....
  //  size<23:22> | opc<19:16> = 1100 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FDIVR_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::fmax(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // FMAX <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
  //  0110 0101 ..01 1110 100. ..00 00.. ....
  //  size<23:22> | opc<18:16> = 110 | Pg<12:10> | i1<5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMAX_z_p_zs | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg));
}

void Assembler::fmax(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // FMAX <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 0110 100. .... .... ....
  //  size<23:22> | opc<19:16> = 0110 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMAX_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::fmaxnm(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn) {
  // FMAXNM <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
  //  0110 0101 ..01 1100 100. ..00 00.. ....
  //  size<23:22> | opc<18:16> = 100 | Pg<12:10> | i1<5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMAXNM_z_p_zs | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg));
}

void Assembler::fmaxnm(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn,
                       const ZRegister& zm) {
  // FMAXNM <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 0100 100. .... .... ....
  //  size<23:22> | opc<19:16> = 0100 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMAXNM_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::fmin(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // FMIN <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
  //  0110 0101 ..01 1111 100. ..00 00.. ....
  //  size<23:22> | opc<18:16> = 111 | Pg<12:10> | i1<5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMIN_z_p_zs | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg));
}

void Assembler::fmin(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // FMIN <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 0111 100. .... .... ....
  //  size<23:22> | opc<19:16> = 0111 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMIN_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::fminnm(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn) {
  // FMINNM <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
  //  0110 0101 ..01 1101 100. ..00 00.. ....
  //  size<23:22> | opc<18:16> = 101 | Pg<12:10> | i1<5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMINNM_z_p_zs | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg));
}

void Assembler::fminnm(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn,
                       const ZRegister& zm) {
  // FMINNM <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 0101 100. .... .... ....
  //  size<23:22> | opc<19:16> = 0101 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMINNM_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::fmul(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // FMUL <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
  //  0110 0101 ..01 1010 100. ..00 00.. ....
  //  size<23:22> | opc<18:16> = 010 | Pg<12:10> | i1<5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMUL_z_p_zs | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg));
}

void Assembler::fmul(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // FMUL <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 0010 100. .... .... ....
  //  size<23:22> | opc<19:16> = 0010 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMUL_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::fmulx(const ZRegister& zd,
                      const PRegisterM& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FMULX <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 1010 100. .... .... ....
  //  size<23:22> | opc<19:16> = 1010 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMULX_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::fscale(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn,
                       const ZRegister& zm) {
  // FSCALE <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 1001 100. .... .... ....
  //  size<23:22> | opc<19:16> = 1001 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FSCALE_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::fsub(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // FSUB <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
  //  0110 0101 ..01 1001 100. ..00 00.. ....
  //  size<23:22> | opc<18:16> = 001 | Pg<12:10> | i1<5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FSUB_z_p_zs | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg));
}

void Assembler::fsub(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // FSUB <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 0001 100. .... .... ....
  //  size<23:22> | opc<19:16> = 0001 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FSUB_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::fsubr(const ZRegister& zd,
                      const PRegisterM& pg,
                      const ZRegister& zn) {
  // FSUBR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <const>
  //  0110 0101 ..01 1011 100. ..00 00.. ....
  //  size<23:22> | opc<18:16> = 011 | Pg<12:10> | i1<5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FSUBR_z_p_zs | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg));
}

void Assembler::fsubr(const ZRegister& zd,
                      const PRegisterM& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FSUBR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0110 0101 ..00 0011 100. .... .... ....
  //  size<23:22> | opc<19:16> = 0011 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FSUBR_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::ftmad(const ZRegister& zd,
                      const ZRegister& zn,
                      const ZRegister& zm,
                      int imm3) {
  // FTMAD <Zdn>.<T>, <Zdn>.<T>, <Zm>.<T>, #<imm>
  //  0110 0101 ..01 0... 1000 00.. .... ....
  //  size<23:22> | imm3<18:16> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FTMAD_z_zzi | SVESize(zd) | Rd(zd) | Rn(zm) | ImmField<18, 16>(imm3));
}

// SVEFPArithmeticUnpredicated.

void Assembler::fadd(const ZRegister& zd,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // FADD <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 0000 00.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 000 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FADD_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::fmul(const ZRegister& zd,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // FMUL <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 0000 10.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 010 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMUL_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::frecps(const ZRegister& zd,
                       const ZRegister& zn,
                       const ZRegister& zm) {
  // FRECPS <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 0001 10.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 110 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FRECPS_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::frsqrts(const ZRegister& zd,
                        const ZRegister& zn,
                        const ZRegister& zm) {
  // FRSQRTS <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 0001 11.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 111 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FRSQRTS_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::fsub(const ZRegister& zd,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // FSUB <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 0000 01.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 001 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FSUB_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::ftsmul(const ZRegister& zd,
                       const ZRegister& zn,
                       const ZRegister& zm) {
  // FTSMUL <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 0000 11.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 011 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FTSMUL_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

// SVEFPCompareVectors.

void Assembler::facge(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FACGE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 110. .... ...1 ....
  //  size<23:22> | Zm<20:16> | op<15> = 1 | o2<13> = 0 | Pg<12:10> | Zn<9:5> |
  //  o3<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FACGE_p_p_zz | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

void Assembler::facgt(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FACGT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 111. .... ...1 ....
  //  size<23:22> | Zm<20:16> | op<15> = 1 | o2<13> = 1 | Pg<12:10> | Zn<9:5> |
  //  o3<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FACGT_p_p_zz | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

void Assembler::fcmeq(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FCMEQ <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 011. .... ...0 ....
  //  size<23:22> | Zm<20:16> | op<15> = 0 | o2<13> = 1 | Pg<12:10> | Zn<9:5> |
  //  o3<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCMEQ_p_p_zz | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

void Assembler::fcmge(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FCMGE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 010. .... ...0 ....
  //  size<23:22> | Zm<20:16> | op<15> = 0 | o2<13> = 0 | Pg<12:10> | Zn<9:5> |
  //  o3<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCMGE_p_p_zz | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

void Assembler::fcmgt(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FCMGT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 010. .... ...1 ....
  //  size<23:22> | Zm<20:16> | op<15> = 0 | o2<13> = 0 | Pg<12:10> | Zn<9:5> |
  //  o3<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCMGT_p_p_zz | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

void Assembler::fcmne(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FCMNE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 011. .... ...1 ....
  //  size<23:22> | Zm<20:16> | op<15> = 0 | o2<13> = 1 | Pg<12:10> | Zn<9:5> |
  //  o3<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCMNE_p_p_zz | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

void Assembler::fcmuo(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FCMUO <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..0. .... 110. .... ...0 ....
  //  size<23:22> | Zm<20:16> | op<15> = 1 | o2<13> = 0 | Pg<12:10> | Zn<9:5> |
  //  o3<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCMUO_p_p_zz | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

// SVEFPCompareWithZero.

void Assembler::fcmeq(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn) {
  // FCMEQ <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
  //  0110 0101 ..01 0010 001. .... ...0 ....
  //  size<23:22> | eq<17> = 1 | lt<16> = 0 | Pg<12:10> | Zn<9:5> | ne<4> = 0 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCMEQ_p_p_z0 | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::fcmge(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn) {
  // FCMGE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
  //  0110 0101 ..01 0000 001. .... ...0 ....
  //  size<23:22> | eq<17> = 0 | lt<16> = 0 | Pg<12:10> | Zn<9:5> | ne<4> = 0 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCMGE_p_p_z0 | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::fcmgt(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn) {
  // FCMGT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
  //  0110 0101 ..01 0000 001. .... ...1 ....
  //  size<23:22> | eq<17> = 0 | lt<16> = 0 | Pg<12:10> | Zn<9:5> | ne<4> = 1 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCMGT_p_p_z0 | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::fcmle(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn) {
  // FCMLE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
  //  0110 0101 ..01 0001 001. .... ...1 ....
  //  size<23:22> | eq<17> = 0 | lt<16> = 1 | Pg<12:10> | Zn<9:5> | ne<4> = 1 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCMLE_p_p_z0 | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::fcmlt(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn) {
  // FCMLT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
  //  0110 0101 ..01 0001 001. .... ...0 ....
  //  size<23:22> | eq<17> = 0 | lt<16> = 1 | Pg<12:10> | Zn<9:5> | ne<4> = 0 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCMLT_p_p_z0 | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::fcmne(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn) {
  // FCMNE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #0.0
  //  0110 0101 ..01 0011 001. .... ...0 ....
  //  size<23:22> | eq<17> = 1 | lt<16> = 1 | Pg<12:10> | Zn<9:5> | ne<4> = 0 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCMNE_p_p_z0 | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn));
}

// SVEFPComplexAddition.

void Assembler::fcadd(const ZRegister& zd,
                      const PRegisterM& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FCADD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>, <const>
  //  0110 0100 ..00 000. 100. .... .... ....
  //  size<23:22> | rot<16> | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCADD_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

// SVEFPComplexMulAdd.

void Assembler::fcmla(const ZRegister& zda,
                      const PRegisterM& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FCMLA <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>, <const>
  //  0110 0100 ..0. .... 0... .... .... ....
  //  size<23:22> | Zm<20:16> | rot<14:13> | Pg<12:10> | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zda, zn, zm));
  VIXL_ASSERT(zda.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCMLA_z_p_zzz | SVESize(zda) | Rd(zda) | Rx<12, 10>(pg) | Rn(zn) |
       Rm(zm));
}

// SVEFPComplexMulAddIndex.

// This prototype maps to 2 instruction encodings:
//  FCMLA_z_zzzi_h
//  FCMLA_z_zzzi_s
void Assembler::fcmla(const ZRegister& zda, const ZRegister& zn) {
  // FCMLA <Zda>.H, <Zn>.H, <Zm>.H[<imm>], <const>
  //  0110 0100 101. .... 0001 .... .... ....
  //  size<23:22> = 10 | opc<20:16> | rot<11:10> | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(FCMLA_z_zzzi_h | Rd(zda) | Rn(zn));
}

// SVEFPFastReduction.

void Assembler::faddv(const VRegister& vd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // FADDV <V><d>, <Pg>, <Zn>.<T>
  //  0110 0101 ..00 0000 001. .... .... ....
  //  size<23:22> | opc<18:16> = 000 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FADDV_v_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::fmaxnmv(const VRegister& vd,
                        const PRegister& pg,
                        const ZRegister& zn) {
  // FMAXNMV <V><d>, <Pg>, <Zn>.<T>
  //  0110 0101 ..00 0100 001. .... .... ....
  //  size<23:22> | opc<18:16> = 100 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMAXNMV_v_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::fmaxv(const VRegister& vd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // FMAXV <V><d>, <Pg>, <Zn>.<T>
  //  0110 0101 ..00 0110 001. .... .... ....
  //  size<23:22> | opc<18:16> = 110 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMAXV_v_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::fminnmv(const VRegister& vd,
                        const PRegister& pg,
                        const ZRegister& zn) {
  // FMINNMV <V><d>, <Pg>, <Zn>.<T>
  //  0110 0101 ..00 0101 001. .... .... ....
  //  size<23:22> | opc<18:16> = 101 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMINNMV_v_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::fminv(const VRegister& vd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // FMINV <V><d>, <Pg>, <Zn>.<T>
  //  0110 0101 ..00 0111 001. .... .... ....
  //  size<23:22> | opc<18:16> = 111 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMINV_v_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

// SVEFPMulAdd.

void Assembler::fmad(const ZRegister& zdn,
                     const PRegisterM& pg,
                     const ZRegister& zm,
                     const ZRegister& za) {
  // FMAD <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
  //  0110 0101 ..1. .... 100. .... .... ....
  //  size<23:22> | Za<20:16> | opc<14:13> = 00 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zdn, zm, za));
  VIXL_ASSERT(zdn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMAD_z_p_zzz | SVESize(zdn) | Rd(zdn) | Rx<12, 10>(pg) | Rn(zm) |
       Rm(za));
}

void Assembler::fmla(const ZRegister& zda,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // FMLA <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..1. .... 000. .... .... ....
  //  size<23:22> | Zm<20:16> | opc<14:13> = 00 | Pg<12:10> | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zda, zn, zm));
  VIXL_ASSERT(zda.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMLA_z_p_zzz | SVESize(zda) | Rd(zda) | Rx<12, 10>(pg) | Rn(zn) |
       Rm(zm));
}

void Assembler::fmls(const ZRegister& zda,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // FMLS <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..1. .... 001. .... .... ....
  //  size<23:22> | Zm<20:16> | opc<14:13> = 01 | Pg<12:10> | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zda, zn, zm));
  VIXL_ASSERT(zda.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMLS_z_p_zzz | SVESize(zda) | Rd(zda) | Rx<12, 10>(pg) | Rn(zn) |
       Rm(zm));
}

void Assembler::fmsb(const ZRegister& zdn,
                     const PRegisterM& pg,
                     const ZRegister& zm,
                     const ZRegister& za) {
  // FMSB <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
  //  0110 0101 ..1. .... 101. .... .... ....
  //  size<23:22> | Za<20:16> | opc<14:13> = 01 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zdn, zm, za));
  VIXL_ASSERT(zdn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FMSB_z_p_zzz | SVESize(zdn) | Rd(zdn) | Rx<12, 10>(pg) | Rn(zm) |
       Rm(za));
}

void Assembler::fnmad(const ZRegister& zdn,
                      const PRegisterM& pg,
                      const ZRegister& zm,
                      const ZRegister& za) {
  // FNMAD <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
  //  0110 0101 ..1. .... 110. .... .... ....
  //  size<23:22> | Za<20:16> | opc<14:13> = 10 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zdn, zm, za));
  VIXL_ASSERT(zdn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FNMAD_z_p_zzz | SVESize(zdn) | Rd(zdn) | Rx<12, 10>(pg) | Rn(zm) |
       Rm(za));
}

void Assembler::fnmla(const ZRegister& zda,
                      const PRegisterM& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FNMLA <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..1. .... 010. .... .... ....
  //  size<23:22> | Zm<20:16> | opc<14:13> = 10 | Pg<12:10> | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zda, zn, zm));
  VIXL_ASSERT(zda.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FNMLA_z_p_zzz | SVESize(zda) | Rd(zda) | Rx<12, 10>(pg) | Rn(zn) |
       Rm(zm));
}

void Assembler::fnmls(const ZRegister& zda,
                      const PRegisterM& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // FNMLS <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>
  //  0110 0101 ..1. .... 011. .... .... ....
  //  size<23:22> | Zm<20:16> | opc<14:13> = 11 | Pg<12:10> | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zda, zn, zm));
  VIXL_ASSERT(zda.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FNMLS_z_p_zzz | SVESize(zda) | Rd(zda) | Rx<12, 10>(pg) | Rn(zn) |
       Rm(zm));
}

void Assembler::fnmsb(const ZRegister& zdn,
                      const PRegisterM& pg,
                      const ZRegister& zm,
                      const ZRegister& za) {
  // FNMSB <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
  //  0110 0101 ..1. .... 111. .... .... ....
  //  size<23:22> | Za<20:16> | opc<14:13> = 11 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zdn, zm, za));
  VIXL_ASSERT(zdn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FNMSB_z_p_zzz | SVESize(zdn) | Rd(zdn) | Rx<12, 10>(pg) | Rn(zm) |
       Rm(za));
}

// SVEFPMulAddIndex.

// This prototype maps to 3 instruction encodings:
//  FMLA_z_zzzi_d
//  FMLA_z_zzzi_h
//  FMLA_z_zzzi_s
void Assembler::fmla(const ZRegister& zda, const ZRegister& zn) {
  // FMLA <Zda>.D, <Zn>.D, <Zm>.D[<imm>]
  //  0110 0100 111. .... 0000 00.. .... ....
  //  size<23:22> = 11 | opc<20:16> | op<10> = 0 | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(FMLA_z_zzzi_d | Rd(zda) | Rn(zn));
}

// This prototype maps to 3 instruction encodings:
//  FMLS_z_zzzi_d
//  FMLS_z_zzzi_h
//  FMLS_z_zzzi_s
void Assembler::fmls(const ZRegister& zda, const ZRegister& zn) {
  // FMLS <Zda>.D, <Zn>.D, <Zm>.D[<imm>]
  //  0110 0100 111. .... 0000 01.. .... ....
  //  size<23:22> = 11 | opc<20:16> | op<10> = 1 | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(FMLS_z_zzzi_d | Rd(zda) | Rn(zn));
}

// SVEFPMulIndex.

// This prototype maps to 3 instruction encodings:
//  FMUL_z_zzi_d
//  FMUL_z_zzi_h
//  FMUL_z_zzi_s
void Assembler::fmul(const ZRegister& zd, const ZRegister& zn) {
  // FMUL <Zd>.D, <Zn>.D, <Zm>.D[<imm>]
  //  0110 0100 111. .... 0010 00.. .... ....
  //  size<23:22> = 11 | opc<20:16> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(FMUL_z_zzi_d | Rd(zd) | Rn(zn));
}

// SVEFPUnaryOpPredicated.

// This prototype maps to 6 instruction encodings:
//  FCVT_z_p_z_d2h
//  FCVT_z_p_z_d2s
//  FCVT_z_p_z_h2d
//  FCVT_z_p_z_h2s
//  FCVT_z_p_z_s2d
//  FCVT_z_p_z_s2h
void Assembler::fcvt(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // FCVT <Zd>.H, <Pg>/M, <Zn>.D
  //  0110 0101 1100 1000 101. .... .... ....
  //  opc<23:22> = 11 | opc2<17:16> = 00 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(FCVT_z_p_z_d2h | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

// This prototype maps to 7 instruction encodings:
//  FCVTZS_z_p_z_d2w
//  FCVTZS_z_p_z_d2x
//  FCVTZS_z_p_z_fp162h
//  FCVTZS_z_p_z_fp162w
//  FCVTZS_z_p_z_fp162x
//  FCVTZS_z_p_z_s2w
//  FCVTZS_z_p_z_s2x
void Assembler::fcvtzs(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn) {
  // FCVTZS <Zd>.S, <Pg>/M, <Zn>.D
  //  0110 0101 1101 1000 101. .... .... ....
  //  opc<23:22> = 11 | opc2<18:17> = 00 | U<16> = 0 | Pg<12:10> | Zn<9:5> |
  //  Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(FCVTZS_z_p_z_d2w | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

// This prototype maps to 7 instruction encodings:
//  FCVTZU_z_p_z_d2w
//  FCVTZU_z_p_z_d2x
//  FCVTZU_z_p_z_fp162h
//  FCVTZU_z_p_z_fp162w
//  FCVTZU_z_p_z_fp162x
//  FCVTZU_z_p_z_s2w
//  FCVTZU_z_p_z_s2x
void Assembler::fcvtzu(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn) {
  // FCVTZU <Zd>.S, <Pg>/M, <Zn>.D
  //  0110 0101 1101 1001 101. .... .... ....
  //  opc<23:22> = 11 | opc2<18:17> = 00 | U<16> = 1 | Pg<12:10> | Zn<9:5> |
  //  Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(FCVTZU_z_p_z_d2w | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::frecpx(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn) {
  // FRECPX <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0110 0101 ..00 1100 101. .... .... ....
  //  size<23:22> | opc<17:16> = 00 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FRECPX_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::frinta(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn) {
  // FRINTA <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0110 0101 ..00 0100 101. .... .... ....
  //  size<23:22> | opc<18:16> = 100 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FRINTA_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::frinti(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn) {
  // FRINTI <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0110 0101 ..00 0111 101. .... .... ....
  //  size<23:22> | opc<18:16> = 111 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FRINTI_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::frintm(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn) {
  // FRINTM <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0110 0101 ..00 0010 101. .... .... ....
  //  size<23:22> | opc<18:16> = 010 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FRINTM_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::frintn(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn) {
  // FRINTN <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0110 0101 ..00 0000 101. .... .... ....
  //  size<23:22> | opc<18:16> = 000 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FRINTN_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::frintp(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn) {
  // FRINTP <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0110 0101 ..00 0001 101. .... .... ....
  //  size<23:22> | opc<18:16> = 001 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FRINTP_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::frintx(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn) {
  // FRINTX <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0110 0101 ..00 0110 101. .... .... ....
  //  size<23:22> | opc<18:16> = 110 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FRINTX_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::frintz(const ZRegister& zd,
                       const PRegisterM& pg,
                       const ZRegister& zn) {
  // FRINTZ <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0110 0101 ..00 0011 101. .... .... ....
  //  size<23:22> | opc<18:16> = 011 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FRINTZ_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::fsqrt(const ZRegister& zd,
                      const PRegisterM& pg,
                      const ZRegister& zn) {
  // FSQRT <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0110 0101 ..00 1101 101. .... .... ....
  //  size<23:22> | opc<17:16> = 01 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FSQRT_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

// This prototype maps to 7 instruction encodings:
//  SCVTF_z_p_z_h2fp16
//  SCVTF_z_p_z_w2d
//  SCVTF_z_p_z_w2fp16
//  SCVTF_z_p_z_w2s
//  SCVTF_z_p_z_x2d
//  SCVTF_z_p_z_x2fp16
//  SCVTF_z_p_z_x2s
void Assembler::scvtf(const ZRegister& zd,
                      const PRegisterM& pg,
                      const ZRegister& zn) {
  // SCVTF <Zd>.H, <Pg>/M, <Zn>.H
  //  0110 0101 0101 0010 101. .... .... ....
  //  opc<23:22> = 01 | opc2<18:17> = 01 | U<16> = 0 | Pg<12:10> | Zn<9:5> |
  //  Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SCVTF_z_p_z_h2fp16 | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

// This prototype maps to 7 instruction encodings:
//  UCVTF_z_p_z_h2fp16
//  UCVTF_z_p_z_w2d
//  UCVTF_z_p_z_w2fp16
//  UCVTF_z_p_z_w2s
//  UCVTF_z_p_z_x2d
//  UCVTF_z_p_z_x2fp16
//  UCVTF_z_p_z_x2s
void Assembler::ucvtf(const ZRegister& zd,
                      const PRegisterM& pg,
                      const ZRegister& zn) {
  // UCVTF <Zd>.H, <Pg>/M, <Zn>.H
  //  0110 0101 0101 0011 101. .... .... ....
  //  opc<23:22> = 01 | opc2<18:17> = 01 | U<16> = 1 | Pg<12:10> | Zn<9:5> |
  //  Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UCVTF_z_p_z_h2fp16 | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

// SVEFPUnaryOpUnpredicated.

void Assembler::frecpe(const ZRegister& zd, const ZRegister& zn) {
  // FRECPE <Zd>.<T>, <Zn>.<T>
  //  0110 0101 ..00 1110 0011 00.. .... ....
  //  size<23:22> | opc<18:16> = 110 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FRECPE_z_z | SVESize(zd) | Rd(zd) | Rn(zn));
}

void Assembler::frsqrte(const ZRegister& zd, const ZRegister& zn) {
  // FRSQRTE <Zd>.<T>, <Zn>.<T>
  //  0110 0101 ..00 1111 0011 00.. .... ....
  //  size<23:22> | opc<18:16> = 111 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FRSQRTE_z_z | SVESize(zd) | Rd(zd) | Rn(zn));
}

// SVEIncDecByPredicateCount.

void Assembler::decp(const Register& rdn, const PRegister& pg) {
  // DECP <Xdn>, <Pg>.<T>
  //  0010 0101 ..10 1101 1000 100. .... ....
  //  size<23:22> | op<17> = 0 | D<16> = 1 | opc2<10:9> = 00 | Pg<8:5> |
  //  Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(DECP_r_p_r | Rd(rdn) | Rx<8, 5>(pg));
}

void Assembler::decp(const ZRegister& zdn, const PRegister& pg) {
  // DECP <Zdn>.<T>, <Pg>
  //  0010 0101 ..10 1101 1000 000. .... ....
  //  size<23:22> | op<17> = 0 | D<16> = 1 | opc2<10:9> = 00 | Pg<8:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zdn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(DECP_z_p_z | SVESize(zdn) | Rd(zdn) | Rx<8, 5>(pg));
}

void Assembler::incp(const Register& rdn, const PRegister& pg) {
  // INCP <Xdn>, <Pg>.<T>
  //  0010 0101 ..10 1100 1000 100. .... ....
  //  size<23:22> | op<17> = 0 | D<16> = 0 | opc2<10:9> = 00 | Pg<8:5> |
  //  Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INCP_r_p_r | Rd(rdn) | Rx<8, 5>(pg));
}

void Assembler::incp(const ZRegister& zdn, const PRegister& pg) {
  // INCP <Zdn>.<T>, <Pg>
  //  0010 0101 ..10 1100 1000 000. .... ....
  //  size<23:22> | op<17> = 0 | D<16> = 0 | opc2<10:9> = 00 | Pg<8:5> |
  //  Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zdn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(INCP_z_p_z | SVESize(zdn) | Rd(zdn) | Rx<8, 5>(pg));
}

void Assembler::sqdecp(const Register& rd,
                       const PRegister& pg,
                       const Register& wn) {
  // SQDECP <Xdn>, <Pg>.<T>, <Wdn>
  //  0010 0101 ..10 1010 1000 100. .... ....
  //  size<23:22> | D<17> = 1 | U<16> = 0 | sf<10> = 0 | op<9> = 0 | Pg<8:5> |
  //  Rdn<4:0>

  USE(wn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(rd.Is(wn));

  Emit(SQDECP_r_p_r_sx | Rd(rd) | Rx<8, 5>(pg));
}

void Assembler::sqdecp(const Register& rdn, const PRegister& pg) {
  // SQDECP <Xdn>, <Pg>.<T>
  //  0010 0101 ..10 1010 1000 110. .... ....
  //  size<23:22> | D<17> = 1 | U<16> = 0 | sf<10> = 1 | op<9> = 0 | Pg<8:5> |
  //  Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQDECP_r_p_r_x | Rd(rdn) | Rx<8, 5>(pg));
}

void Assembler::sqdecp(const ZRegister& zdn, const PRegister& pg) {
  // SQDECP <Zdn>.<T>, <Pg>
  //  0010 0101 ..10 1010 1000 000. .... ....
  //  size<23:22> | D<17> = 1 | U<16> = 0 | opc<10:9> = 00 | Pg<8:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zdn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(SQDECP_z_p_z | SVESize(zdn) | Rd(zdn) | Rx<8, 5>(pg));
}

void Assembler::sqincp(const Register& rd,
                       const PRegister& pg,
                       const Register& wn) {
  // SQINCP <Xdn>, <Pg>.<T>, <Wdn>
  //  0010 0101 ..10 1000 1000 100. .... ....
  //  size<23:22> | D<17> = 0 | U<16> = 0 | sf<10> = 0 | op<9> = 0 | Pg<8:5> |
  //  Rdn<4:0>

  USE(wn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(rd.Is(wn));

  Emit(SQINCP_r_p_r_sx | Rd(rd) | Rx<8, 5>(pg));
}

void Assembler::sqincp(const Register& rdn, const PRegister& pg) {
  // SQINCP <Xdn>, <Pg>.<T>
  //  0010 0101 ..10 1000 1000 110. .... ....
  //  size<23:22> | D<17> = 0 | U<16> = 0 | sf<10> = 1 | op<9> = 0 | Pg<8:5> |
  //  Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SQINCP_r_p_r_x | Rd(rdn) | Rx<8, 5>(pg));
}

void Assembler::sqincp(const ZRegister& zdn, const PRegister& pg) {
  // SQINCP <Zdn>.<T>, <Pg>
  //  0010 0101 ..10 1000 1000 000. .... ....
  //  size<23:22> | D<17> = 0 | U<16> = 0 | opc<10:9> = 00 | Pg<8:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zdn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(SQINCP_z_p_z | SVESize(zdn) | Rd(zdn) | Rx<8, 5>(pg));
}

// This prototype maps to 2 instruction encodings:
//  UQDECP_r_p_r_uw
//  UQDECP_r_p_r_x
void Assembler::uqdecp(const Register& rdn, const PRegister& pg) {
  // UQDECP <Wdn>, <Pg>.<T>
  //  0010 0101 ..10 1011 1000 100. .... ....
  //  size<23:22> | D<17> = 1 | U<16> = 1 | sf<10> = 0 | op<9> = 0 | Pg<8:5> |
  //  Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQDECP_r_p_r_uw | Rd(rdn) | Rx<8, 5>(pg));
}

void Assembler::uqdecp(const ZRegister& zdn, const PRegister& pg) {
  // UQDECP <Zdn>.<T>, <Pg>
  //  0010 0101 ..10 1011 1000 000. .... ....
  //  size<23:22> | D<17> = 1 | U<16> = 1 | opc<10:9> = 00 | Pg<8:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zdn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(UQDECP_z_p_z | SVESize(zdn) | Rd(zdn) | Rx<8, 5>(pg));
}

// This prototype maps to 2 instruction encodings:
//  UQINCP_r_p_r_uw
//  UQINCP_r_p_r_x
void Assembler::uqincp(const Register& rdn, const PRegister& pg) {
  // UQINCP <Wdn>, <Pg>.<T>
  //  0010 0101 ..10 1001 1000 100. .... ....
  //  size<23:22> | D<17> = 0 | U<16> = 1 | sf<10> = 0 | op<9> = 0 | Pg<8:5> |
  //  Rdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UQINCP_r_p_r_uw | Rd(rdn) | Rx<8, 5>(pg));
}

void Assembler::uqincp(const ZRegister& zdn, const PRegister& pg) {
  // UQINCP <Zdn>.<T>, <Pg>
  //  0010 0101 ..10 1001 1000 000. .... ....
  //  size<23:22> | D<17> = 0 | U<16> = 1 | opc<10:9> = 00 | Pg<8:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zdn.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(UQINCP_z_p_z | SVESize(zdn) | Rd(zdn) | Rx<8, 5>(pg));
}

// SVEIndexGeneration.

void Assembler::index(const ZRegister& zd) {
  // INDEX <Zd>.<T>, #<imm1>, #<imm2>
  //  0000 0100 ..1. .... 0100 00.. .... ....
  //  size<23:22> | imm5b<20:16> | imm5<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INDEX_z_ii | SVESize(zd) | Rd(zd));
}

void Assembler::index(const ZRegister& zd,
                      const Register& rn,
                      const Register& rm) {
  // INDEX <Zd>.<T>, <R><n>, <R><m>
  //  0000 0100 ..1. .... 0100 11.. .... ....
  //  size<23:22> | Rm<20:16> | Rn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INDEX_z_rr | SVESize(zd) | Rd(zd) | Rn(rn) | Rm(rm));
}

void Assembler::index(const ZRegister& zd, const Register& rn, int imm5) {
  // INDEX <Zd>.<T>, <R><n>, #<imm>
  //  0000 0100 ..1. .... 0100 01.. .... ....
  //  size<23:22> | imm5<20:16> | Rn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INDEX_z_ri | SVESize(zd) | Rd(zd) | Rn(rn) | ImmField<20, 16>(imm5));
}

void Assembler::index(const ZRegister& zd, int imm5, const Register& rm) {
  // INDEX <Zd>.<T>, #<imm>, <R><m>
  //  0000 0100 ..1. .... 0100 10.. .... ....
  //  size<23:22> | Rm<20:16> | imm5<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INDEX_z_ir | SVESize(zd) | Rd(zd) | ImmField<9, 5>(imm5) | Rm(rm));
}

// SVEIntArithmeticUnpredicated.

void Assembler::add(const ZRegister& zd,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // ADD <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0100 ..1. .... 0000 00.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 000 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(ADD_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::sqadd(const ZRegister& zd,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // SQADD <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0100 ..1. .... 0001 00.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 100 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(SQADD_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::sqsub(const ZRegister& zd,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // SQSUB <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0100 ..1. .... 0001 10.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 110 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(SQSUB_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::sub(const ZRegister& zd,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // SUB <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0100 ..1. .... 0000 01.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 001 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(SUB_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::uqadd(const ZRegister& zd,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // UQADD <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0100 ..1. .... 0001 01.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 101 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(UQADD_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::uqsub(const ZRegister& zd,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // UQSUB <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0100 ..1. .... 0001 11.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 111 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(UQSUB_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

// SVEIntBinaryArithmeticPredicated.

void Assembler::add(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // ADD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..00 0000 000. .... .... ....
  //  size<23:22> | opc<18:16> = 000 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(ADD_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::and_(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // AND <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 1010 000. .... .... ....
  //  size<23:22> | opc<18:16> = 010 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(AND_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::bic(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // BIC <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 1011 000. .... .... ....
  //  size<23:22> | opc<18:16> = 011 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(BIC_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::eor(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // EOR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 1001 000. .... .... ....
  //  size<23:22> | opc<18:16> = 001 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(EOR_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::mul(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // MUL <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 0000 000. .... .... ....
  //  size<23:22> | H<17> = 0 | U<16> = 0 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(MUL_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::orr(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // ORR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 1000 000. .... .... ....
  //  size<23:22> | opc<18:16> = 000 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(ORR_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::sabd(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // SABD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..00 1100 000. .... .... ....
  //  size<23:22> | opc<18:17> = 10 | U<16> = 0 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(SABD_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::sdiv(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // SDIV <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 0100 000. .... .... ....
  //  size<23:22> | R<17> = 0 | U<16> = 0 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));

  Emit(SDIV_z_p_zz | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::sdivr(const ZRegister& zd,
                      const PRegisterM& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // SDIVR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 0110 000. .... .... ....
  //  size<23:22> | R<17> = 1 | U<16> = 0 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));

  Emit(SDIVR_z_p_zz | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::smax(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // SMAX <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..00 1000 000. .... .... ....
  //  size<23:22> | opc<18:17> = 00 | U<16> = 0 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(SMAX_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::smin(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // SMIN <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..00 1010 000. .... .... ....
  //  size<23:22> | opc<18:17> = 01 | U<16> = 0 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(SMIN_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::smulh(const ZRegister& zd,
                      const PRegisterM& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // SMULH <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 0010 000. .... .... ....
  //  size<23:22> | H<17> = 1 | U<16> = 0 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(SMULH_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::sub(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // SUB <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..00 0001 000. .... .... ....
  //  size<23:22> | opc<18:16> = 001 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(SUB_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::subr(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // SUBR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..00 0011 000. .... .... ....
  //  size<23:22> | opc<18:16> = 011 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(SUBR_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::uabd(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // UABD <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..00 1101 000. .... .... ....
  //  size<23:22> | opc<18:17> = 10 | U<16> = 1 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(UABD_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::udiv(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // UDIV <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 0101 000. .... .... ....
  //  size<23:22> | R<17> = 0 | U<16> = 1 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));

  Emit(UDIV_z_p_zz | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::udivr(const ZRegister& zd,
                      const PRegisterM& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // UDIVR <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 0111 000. .... .... ....
  //  size<23:22> | R<17> = 1 | U<16> = 1 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));

  Emit(UDIVR_z_p_zz | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::umax(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // UMAX <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..00 1001 000. .... .... ....
  //  size<23:22> | opc<18:17> = 00 | U<16> = 1 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(UMAX_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::umin(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // UMIN <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..00 1011 000. .... .... ....
  //  size<23:22> | opc<18:17> = 01 | U<16> = 1 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(UMIN_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::umulh(const ZRegister& zd,
                      const PRegisterM& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // UMULH <Zdn>.<T>, <Pg>/M, <Zdn>.<T>, <Zm>.<T>
  //  0000 0100 ..01 0011 000. .... .... ....
  //  size<23:22> | H<17> = 1 | U<16> = 1 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(UMULH_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

// SVEIntCompareScalars.

void Assembler::ctermeq(const Register& rn, const Register& rm) {
  // CTERMEQ <R><n>, <R><m>
  //  0010 0101 1.1. .... 0010 00.. ...0 0000
  //  op<23> = 1 | sz<22> | Rm<20:16> | Rn<9:5> | ne<4> = 0

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CTERMEQ_rr | Rn(rn) | Rm(rm));
}

void Assembler::ctermne(const Register& rn, const Register& rm) {
  // CTERMNE <R><n>, <R><m>
  //  0010 0101 1.1. .... 0010 00.. ...1 0000
  //  op<23> = 1 | sz<22> | Rm<20:16> | Rn<9:5> | ne<4> = 1

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CTERMNE_rr | Rn(rn) | Rm(rm));
}

void Assembler::whilele(const PRegisterWithLaneSize& pd,
                        const Register& rn,
                        const Register& rm) {
  // WHILELE <Pd>.<T>, <R><n>, <R><m>
  //  0010 0101 ..1. .... 000. 01.. ...1 ....
  //  size<23:22> | Rm<20:16> | sf<12> | U<11> = 0 | lt<10> = 1 | Rn<9:5> |
  //  eq<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(WHILELE_p_p_rr | Pd(pd) | Rn(rn) | Rm(rm));
}

void Assembler::whilelo(const PRegisterWithLaneSize& pd,
                        const Register& rn,
                        const Register& rm) {
  // WHILELO <Pd>.<T>, <R><n>, <R><m>
  //  0010 0101 ..1. .... 000. 11.. ...0 ....
  //  size<23:22> | Rm<20:16> | sf<12> | U<11> = 1 | lt<10> = 1 | Rn<9:5> |
  //  eq<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(WHILELO_p_p_rr | Pd(pd) | Rn(rn) | Rm(rm));
}

void Assembler::whilels(const PRegisterWithLaneSize& pd,
                        const Register& rn,
                        const Register& rm) {
  // WHILELS <Pd>.<T>, <R><n>, <R><m>
  //  0010 0101 ..1. .... 000. 11.. ...1 ....
  //  size<23:22> | Rm<20:16> | sf<12> | U<11> = 1 | lt<10> = 1 | Rn<9:5> |
  //  eq<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(WHILELS_p_p_rr | Pd(pd) | Rn(rn) | Rm(rm));
}

void Assembler::whilelt(const PRegisterWithLaneSize& pd,
                        const Register& rn,
                        const Register& rm) {
  // WHILELT <Pd>.<T>, <R><n>, <R><m>
  //  0010 0101 ..1. .... 000. 01.. ...0 ....
  //  size<23:22> | Rm<20:16> | sf<12> | U<11> = 0 | lt<10> = 1 | Rn<9:5> |
  //  eq<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(WHILELT_p_p_rr | Pd(pd) | Rn(rn) | Rm(rm));
}

// SVEIntCompareSignedImm.

void Assembler::cmpeq(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm5) {
  // CMPEQ <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
  //  0010 0101 ..0. .... 100. .... ...0 ....
  //  size<23:22> | imm5<20:16> | op<15> = 1 | o2<13> = 0 | Pg<12:10> | Zn<9:5>
  //  | ne<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CMPEQ_p_p_zi | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

void Assembler::cmpge(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm5) {
  // CMPGE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
  //  0010 0101 ..0. .... 000. .... ...0 ....
  //  size<23:22> | imm5<20:16> | op<15> = 0 | o2<13> = 0 | Pg<12:10> | Zn<9:5>
  //  | ne<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CMPGE_p_p_zi | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

void Assembler::cmpgt(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm5) {
  // CMPGT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
  //  0010 0101 ..0. .... 000. .... ...1 ....
  //  size<23:22> | imm5<20:16> | op<15> = 0 | o2<13> = 0 | Pg<12:10> | Zn<9:5>
  //  | ne<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CMPGT_p_p_zi | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

void Assembler::cmple(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm5) {
  // CMPLE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
  //  0010 0101 ..0. .... 001. .... ...1 ....
  //  size<23:22> | imm5<20:16> | op<15> = 0 | o2<13> = 1 | Pg<12:10> | Zn<9:5>
  //  | ne<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CMPLE_p_p_zi | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

void Assembler::cmplt(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm5) {
  // CMPLT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
  //  0010 0101 ..0. .... 001. .... ...0 ....
  //  size<23:22> | imm5<20:16> | op<15> = 0 | o2<13> = 1 | Pg<12:10> | Zn<9:5>
  //  | ne<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CMPLT_p_p_zi | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

void Assembler::cmpne(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm5) {
  // CMPNE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
  //  0010 0101 ..0. .... 100. .... ...1 ....
  //  size<23:22> | imm5<20:16> | op<15> = 1 | o2<13> = 0 | Pg<12:10> | Zn<9:5>
  //  | ne<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CMPNE_p_p_zi | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// SVEIntCompareUnsignedImm.

void Assembler::cmphi(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm7) {
  // CMPHI <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
  //  0010 0100 ..1. .... ..0. .... ...1 ....
  //  size<23:22> | imm7<20:14> | lt<13> = 0 | Pg<12:10> | Zn<9:5> | ne<4> = 1 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CMPHI_p_p_zi | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 14>(imm7));
}

void Assembler::cmphs(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm7) {
  // CMPHS <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
  //  0010 0100 ..1. .... ..0. .... ...0 ....
  //  size<23:22> | imm7<20:14> | lt<13> = 0 | Pg<12:10> | Zn<9:5> | ne<4> = 0 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CMPHS_p_p_zi | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 14>(imm7));
}

void Assembler::cmplo(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm7) {
  // CMPLO <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
  //  0010 0100 ..1. .... ..1. .... ...0 ....
  //  size<23:22> | imm7<20:14> | lt<13> = 1 | Pg<12:10> | Zn<9:5> | ne<4> = 0 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CMPLO_p_p_zi | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 14>(imm7));
}

void Assembler::cmpls(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm7) {
  // CMPLS <Pd>.<T>, <Pg>/Z, <Zn>.<T>, #<imm>
  //  0010 0100 ..1. .... ..1. .... ...1 ....
  //  size<23:22> | imm7<20:14> | lt<13> = 1 | Pg<12:10> | Zn<9:5> | ne<4> = 1 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CMPLS_p_p_zi | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 14>(imm7));
}

// SVEIntCompareVectors.

// This prototype maps to 2 instruction encodings:
//  CMPEQ_p_p_zw
//  CMPEQ_p_p_zz
void Assembler::cmpeq(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // CMPEQ <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
  //  0010 0100 ..0. .... 001. .... ...0 ....
  //  size<23:22> | Zm<20:16> | op<15> = 0 | o2<13> = 1 | Pg<12:10> | Zn<9:5> |
  //  ne<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(CMPEQ_p_p_zw | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  CMPGE_p_p_zw
//  CMPGE_p_p_zz
void Assembler::cmpge(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // CMPGE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
  //  0010 0100 ..0. .... 010. .... ...0 ....
  //  size<23:22> | Zm<20:16> | U<15> = 0 | lt<13> = 0 | Pg<12:10> | Zn<9:5> |
  //  ne<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(CMPGE_p_p_zw | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  CMPGT_p_p_zw
//  CMPGT_p_p_zz
void Assembler::cmpgt(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // CMPGT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
  //  0010 0100 ..0. .... 010. .... ...1 ....
  //  size<23:22> | Zm<20:16> | U<15> = 0 | lt<13> = 0 | Pg<12:10> | Zn<9:5> |
  //  ne<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(CMPGT_p_p_zw | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  CMPHI_p_p_zw
//  CMPHI_p_p_zz
void Assembler::cmphi(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // CMPHI <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
  //  0010 0100 ..0. .... 110. .... ...1 ....
  //  size<23:22> | Zm<20:16> | U<15> = 1 | lt<13> = 0 | Pg<12:10> | Zn<9:5> |
  //  ne<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(CMPHI_p_p_zw | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  CMPHS_p_p_zw
//  CMPHS_p_p_zz
void Assembler::cmphs(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // CMPHS <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
  //  0010 0100 ..0. .... 110. .... ...0 ....
  //  size<23:22> | Zm<20:16> | U<15> = 1 | lt<13> = 0 | Pg<12:10> | Zn<9:5> |
  //  ne<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(CMPHS_p_p_zw | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

void Assembler::cmple(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // CMPLE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
  //  0010 0100 ..0. .... 011. .... ...1 ....
  //  size<23:22> | Zm<20:16> | U<15> = 0 | lt<13> = 1 | Pg<12:10> | Zn<9:5> |
  //  ne<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(CMPLE_p_p_zw | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

void Assembler::cmplo(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // CMPLO <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
  //  0010 0100 ..0. .... 111. .... ...0 ....
  //  size<23:22> | Zm<20:16> | U<15> = 1 | lt<13> = 1 | Pg<12:10> | Zn<9:5> |
  //  ne<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(CMPLO_p_p_zw | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

void Assembler::cmpls(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // CMPLS <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
  //  0010 0100 ..0. .... 111. .... ...1 ....
  //  size<23:22> | Zm<20:16> | U<15> = 1 | lt<13> = 1 | Pg<12:10> | Zn<9:5> |
  //  ne<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(CMPLS_p_p_zw | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

void Assembler::cmplt(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // CMPLT <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
  //  0010 0100 ..0. .... 011. .... ...0 ....
  //  size<23:22> | Zm<20:16> | U<15> = 0 | lt<13> = 1 | Pg<12:10> | Zn<9:5> |
  //  ne<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(CMPLT_p_p_zw | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  CMPNE_p_p_zw
//  CMPNE_p_p_zz
void Assembler::cmpne(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      const ZRegister& zm) {
  // CMPNE <Pd>.<T>, <Pg>/Z, <Zn>.<T>, <Zm>.D
  //  0010 0100 ..0. .... 001. .... ...1 ....
  //  size<23:22> | Zm<20:16> | op<15> = 0 | o2<13> = 1 | Pg<12:10> | Zn<9:5> |
  //  ne<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zn, zm));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(CMPNE_p_p_zw | SVESize(zn) | Pd(pd) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

// SVEIntMiscUnpredicated.

void Assembler::fexpa(const ZRegister& zd, const ZRegister& zn) {
  // FEXPA <Zd>.<T>, <Zn>.<T>
  //  0000 0100 ..10 0000 1011 10.. .... ....
  //  size<23:22> | opc<20:16> = 00000 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FEXPA_z_z | SVESize(zd) | Rd(zd) | Rn(zn));
}

void Assembler::ftssel(const ZRegister& zd,
                       const ZRegister& zn,
                       const ZRegister& zm) {
  // FTSSEL <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0100 ..1. .... 1011 00.. .... ....
  //  size<23:22> | Zm<20:16> | op<10> = 0 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FTSSEL_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::movprfx(const ZRegister& zd, const ZRegister& zn) {
  // MOVPRFX <Zd>, <Zn>
  //  0000 0100 0010 0000 1011 11.. .... ....
  //  opc<23:22> = 00 | opc2<20:16> = 00000 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(MOVPRFX_z_z | Rd(zd) | Rn(zn));
}

// SVEIntMulAddPredicated.

void Assembler::mad(const ZRegister& zdn,
                    const PRegisterM& pg,
                    const ZRegister& zm,
                    const ZRegister& za) {
  // MAD <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
  //  0000 0100 ..0. .... 110. .... .... ....
  //  size<23:22> | Zm<20:16> | op<13> = 0 | Pg<12:10> | Za<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zdn, zm, za));

  Emit(MAD_z_p_zzz | SVESize(zdn) | Rd(zdn) | Rx<12, 10>(pg) | Rm(zm) | Rn(za));
}

void Assembler::mla(const ZRegister& zda,
                    const PRegisterM& pg,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // MLA <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>
  //  0000 0100 ..0. .... 010. .... .... ....
  //  size<23:22> | Zm<20:16> | op<13> = 0 | Pg<12:10> | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zda, zn, zm));

  Emit(MLA_z_p_zzz | SVESize(zda) | Rd(zda) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

void Assembler::mls(const ZRegister& zda,
                    const PRegisterM& pg,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // MLS <Zda>.<T>, <Pg>/M, <Zn>.<T>, <Zm>.<T>
  //  0000 0100 ..0. .... 011. .... .... ....
  //  size<23:22> | Zm<20:16> | op<13> = 1 | Pg<12:10> | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zda, zn, zm));

  Emit(MLS_z_p_zzz | SVESize(zda) | Rd(zda) | Rx<12, 10>(pg) | Rn(zn) | Rm(zm));
}

void Assembler::msb(const ZRegister& zdn,
                    const PRegisterM& pg,
                    const ZRegister& zm,
                    const ZRegister& za) {
  // MSB <Zdn>.<T>, <Pg>/M, <Zm>.<T>, <Za>.<T>
  //  0000 0100 ..0. .... 111. .... .... ....
  //  size<23:22> | Zm<20:16> | op<13> = 1 | Pg<12:10> | Za<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zdn, zm, za));

  Emit(MSB_z_p_zzz | SVESize(zdn) | Rd(zdn) | Rx<12, 10>(pg) | Rm(zm) | Rn(za));
}

// SVEIntMulAddUnpredicated.

void Assembler::sdot(const ZRegister& zda,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // SDOT <Zda>.<T>, <Zn>.<Tb>, <Zm>.<Tb>
  //  0100 0100 ..0. .... 0000 00.. .... ....
  //  size<23:22> | Zm<20:16> | U<10> = 0 | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SDOT_z_zzz | Rd(zda) | Rn(zn) | Rm(zm));
}

void Assembler::udot(const ZRegister& zda,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // UDOT <Zda>.<T>, <Zn>.<Tb>, <Zm>.<Tb>
  //  0100 0100 ..0. .... 0000 01.. .... ....
  //  size<23:22> | Zm<20:16> | U<10> = 1 | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UDOT_z_zzz | Rd(zda) | Rn(zn) | Rm(zm));
}

// SVEIntReduction.

void Assembler::andv(const VRegister& vd,
                     const PRegister& pg,
                     const ZRegister& zn) {
  // ANDV <V><d>, <Pg>, <Zn>.<T>
  //  0000 0100 ..01 1010 001. .... .... ....
  //  size<23:22> | opc<18:16> = 010 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());

  Emit(ANDV_r_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::eorv(const VRegister& vd,
                     const PRegister& pg,
                     const ZRegister& zn) {
  // EORV <V><d>, <Pg>, <Zn>.<T>
  //  0000 0100 ..01 1001 001. .... .... ....
  //  size<23:22> | opc<18:16> = 001 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());

  Emit(EORV_r_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::movprfx(const ZRegister& zd,
                        const PRegister& pg,
                        const ZRegister& zn) {
  // MOVPRFX <Zd>.<T>, <Pg>/<ZM>, <Zn>.<T>
  //  0000 0100 ..01 000. 001. .... .... ....
  //  size<23:22> | opc<18:17> = 00 | M<16> | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(MOVPRFX_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::orv(const VRegister& vd,
                    const PRegister& pg,
                    const ZRegister& zn) {
  // ORV <V><d>, <Pg>, <Zn>.<T>
  //  0000 0100 ..01 1000 001. .... .... ....
  //  size<23:22> | opc<18:16> = 000 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());

  Emit(ORV_r_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::saddv(const VRegister& dd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // SADDV <Dd>, <Pg>, <Zn>.<T>
  //  0000 0100 ..00 0000 001. .... .... ....
  //  size<23:22> | opc<18:17> = 00 | U<16> = 0 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zn.GetLaneSizeInBytes() != kDRegSizeInBytes);

  Emit(SADDV_r_p_z | SVESize(zn) | Rd(dd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::smaxv(const VRegister& vd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // SMAXV <V><d>, <Pg>, <Zn>.<T>
  //  0000 0100 ..00 1000 001. .... .... ....
  //  size<23:22> | opc<18:17> = 00 | U<16> = 0 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());

  Emit(SMAXV_r_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::sminv(const VRegister& vd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // SMINV <V><d>, <Pg>, <Zn>.<T>
  //  0000 0100 ..00 1010 001. .... .... ....
  //  size<23:22> | opc<18:17> = 01 | U<16> = 0 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());

  Emit(SMINV_r_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::uaddv(const VRegister& dd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // UADDV <Dd>, <Pg>, <Zn>.<T>
  //  0000 0100 ..00 0001 001. .... .... ....
  //  size<23:22> | opc<18:17> = 00 | U<16> = 1 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UADDV_r_p_z | SVESize(zn) | Rd(dd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::umaxv(const VRegister& vd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // UMAXV <V><d>, <Pg>, <Zn>.<T>
  //  0000 0100 ..00 1001 001. .... .... ....
  //  size<23:22> | opc<18:17> = 00 | U<16> = 1 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());

  Emit(UMAXV_r_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::uminv(const VRegister& vd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // UMINV <V><d>, <Pg>, <Zn>.<T>
  //  0000 0100 ..00 1011 001. .... .... ....
  //  size<23:22> | opc<18:17> = 01 | U<16> = 1 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());

  Emit(UMINV_r_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

// SVEIntUnaryArithmeticPredicated.

void Assembler::abs(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn) {
  // ABS <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 0110 101. .... .... ....
  //  size<23:22> | opc<18:16> = 110 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(ABS_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::cls(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn) {
  // CLS <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 1000 101. .... .... ....
  //  size<23:22> | opc<18:16> = 000 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(CLS_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::clz(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn) {
  // CLZ <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 1001 101. .... .... ....
  //  size<23:22> | opc<18:16> = 001 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(CLZ_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::cnot(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // CNOT <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 1011 101. .... .... ....
  //  size<23:22> | opc<18:16> = 011 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(CNOT_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::cnt(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn) {
  // CNT <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 1010 101. .... .... ....
  //  size<23:22> | opc<18:16> = 010 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(CNT_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::fabs(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // FABS <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 1100 101. .... .... ....
  //  size<23:22> | opc<18:16> = 100 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FABS_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::fneg(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // FNEG <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 1101 101. .... .... ....
  //  size<23:22> | opc<18:16> = 101 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FNEG_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::neg(const ZRegister& zd,
                    const PRegisterM& pg,
                    const ZRegister& zn) {
  // NEG <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 0111 101. .... .... ....
  //  size<23:22> | opc<18:16> = 111 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(NEG_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::not_(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // NOT <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 1110 101. .... .... ....
  //  size<23:22> | opc<18:16> = 110 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(NOT_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::sxtb(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // SXTB <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 0000 101. .... .... ....
  //  size<23:22> | opc<18:16> = 000 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(SXTB_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::sxth(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // SXTH <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 0010 101. .... .... ....
  //  size<23:22> | opc<18:16> = 010 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SXTH_z_p_z | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::sxtw(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // SXTW <Zd>.D, <Pg>/M, <Zn>.D
  //  0000 0100 ..01 0100 101. .... .... ....
  //  size<23:22> | opc<18:16> = 100 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SXTW_z_p_z | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::uxtb(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // UXTB <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 0001 101. .... .... ....
  //  size<23:22> | opc<18:16> = 001 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(UXTB_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::uxth(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // UXTH <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0100 ..01 0011 101. .... .... ....
  //  size<23:22> | opc<18:16> = 011 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UXTH_z_p_z | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::uxtw(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // UXTW <Zd>.D, <Pg>/M, <Zn>.D
  //  0000 0100 ..01 0101 101. .... .... ....
  //  size<23:22> | opc<18:16> = 101 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UXTW_z_p_z | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

// SVEIntWideImmPredicated.

void Assembler::cpy(const ZRegister& zd, const PRegister& pg, int imm8) {
  // CPY <Zd>.<T>, <Pg>/<ZM>, #<imm>{, <shift>}
  //  0000 0101 ..01 .... 0... .... .... ....
  //  size<23:22> | Pg<19:16> | M<14> | sh<13> | imm8<12:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CPY_z_p_i | SVESize(zd) | Rd(zd) | Rx<19, 16>(pg) |
       ImmField<12, 5>(imm8));
}

void Assembler::fcpy(const ZRegister& zd, const PRegisterM& pg) {
  // FCPY <Zd>.<T>, <Pg>/M, #<const>
  //  0000 0101 ..01 .... 110. .... .... ....
  //  size<23:22> | Pg<19:16> | imm8<12:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FCPY_z_p_i | SVESize(zd) | Rd(zd) | Rx<19, 16>(pg));
}

// SVEIntWideImmUnpredicated.

void Assembler::add(const ZRegister& zd, const ZRegister& zn, int imm8) {
  // ADD <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
  //  0010 0101 ..10 0000 11.. .... .... ....
  //  size<23:22> | opc<18:16> = 000 | sh<13> | imm8<12:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(ADD_z_zi | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

void Assembler::dup(const ZRegister& zd, int imm8) {
  // DUP <Zd>.<T>, #<imm>{, <shift>}
  //  0010 0101 ..11 1000 11.. .... .... ....
  //  size<23:22> | opc<18:17> = 00 | sh<13> | imm8<12:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(DUP_z_i | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

void Assembler::fdup(const ZRegister& zd) {
  // FDUP <Zd>.<T>, #<const>
  //  0010 0101 ..11 1001 110. .... .... ....
  //  size<23:22> | opc<18:17> = 00 | o2<13> = 0 | imm8<12:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(FDUP_z_i | SVESize(zd) | Rd(zd));
}

void Assembler::mul(const ZRegister& zd, const ZRegister& zn, int imm8) {
  // MUL <Zdn>.<T>, <Zdn>.<T>, #<imm>
  //  0010 0101 ..11 0000 110. .... .... ....
  //  size<23:22> | opc<18:16> = 000 | o2<13> = 0 | imm8<12:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(MUL_z_zi | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

void Assembler::smax(const ZRegister& zd, const ZRegister& zn, int imm8) {
  // SMAX <Zdn>.<T>, <Zdn>.<T>, #<imm>
  //  0010 0101 ..10 1000 110. .... .... ....
  //  size<23:22> | opc<18:16> = 000 | o2<13> = 0 | imm8<12:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(SMAX_z_zi | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

void Assembler::smin(const ZRegister& zd, const ZRegister& zn, int imm8) {
  // SMIN <Zdn>.<T>, <Zdn>.<T>, #<imm>
  //  0010 0101 ..10 1010 110. .... .... ....
  //  size<23:22> | opc<18:16> = 010 | o2<13> = 0 | imm8<12:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(SMIN_z_zi | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

void Assembler::sqadd(const ZRegister& zd, const ZRegister& zn, int imm8) {
  // SQADD <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
  //  0010 0101 ..10 0100 11.. .... .... ....
  //  size<23:22> | opc<18:16> = 100 | sh<13> | imm8<12:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(SQADD_z_zi | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

void Assembler::sqsub(const ZRegister& zd, const ZRegister& zn, int imm8) {
  // SQSUB <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
  //  0010 0101 ..10 0110 11.. .... .... ....
  //  size<23:22> | opc<18:16> = 110 | sh<13> | imm8<12:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(SQSUB_z_zi | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

void Assembler::sub(const ZRegister& zd, const ZRegister& zn, int imm8) {
  // SUB <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
  //  0010 0101 ..10 0001 11.. .... .... ....
  //  size<23:22> | opc<18:16> = 001 | sh<13> | imm8<12:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(SUB_z_zi | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

void Assembler::subr(const ZRegister& zd, const ZRegister& zn, int imm8) {
  // SUBR <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
  //  0010 0101 ..10 0011 11.. .... .... ....
  //  size<23:22> | opc<18:16> = 011 | sh<13> | imm8<12:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(SUBR_z_zi | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

void Assembler::umax(const ZRegister& zd, const ZRegister& zn, int imm8) {
  // UMAX <Zdn>.<T>, <Zdn>.<T>, #<imm>
  //  0010 0101 ..10 1001 110. .... .... ....
  //  size<23:22> | opc<18:16> = 001 | o2<13> = 0 | imm8<12:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(UMAX_z_zi | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

void Assembler::umin(const ZRegister& zd, const ZRegister& zn, int imm8) {
  // UMIN <Zdn>.<T>, <Zdn>.<T>, #<imm>
  //  0010 0101 ..10 1011 110. .... .... ....
  //  size<23:22> | opc<18:16> = 011 | o2<13> = 0 | imm8<12:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(UMIN_z_zi | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

void Assembler::uqadd(const ZRegister& zd, const ZRegister& zn, int imm8) {
  // UQADD <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
  //  0010 0101 ..10 0101 11.. .... .... ....
  //  size<23:22> | opc<18:16> = 101 | sh<13> | imm8<12:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(UQADD_z_zi | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

void Assembler::uqsub(const ZRegister& zd, const ZRegister& zn, int imm8) {
  // UQSUB <Zdn>.<T>, <Zdn>.<T>, #<imm>{, <shift>}
  //  0010 0101 ..10 0111 11.. .... .... ....
  //  size<23:22> | opc<18:16> = 111 | sh<13> | imm8<12:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(UQSUB_z_zi | SVESize(zd) | Rd(zd) | ImmField<12, 5>(imm8));
}

// SVEMem32BitGatherAndUnsizedContiguous.

// This prototype maps to 4 instruction encodings:
//  LD1RB_z_p_bi_u16
//  LD1RB_z_p_bi_u32
//  LD1RB_z_p_bi_u64
//  LD1RB_z_p_bi_u8
void Assembler::ld1rb(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      int imm6) {
  // LD1RB { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
  //  1000 0100 01.. .... 101. .... .... ....
  //  dtypeh<24:23> = 00 | imm6<21:16> | dtypel<14:13> = 01 | Pg<12:10> |
  //  Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RB_z_p_bi_u16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<21, 16>(imm6));
}

void Assembler::ld1rd(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      int imm6) {
  // LD1RD { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
  //  1000 0101 11.. .... 111. .... .... ....
  //  dtypeh<24:23> = 11 | imm6<21:16> | dtypel<14:13> = 11 | Pg<12:10> |
  //  Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RD_z_p_bi_u64 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<21, 16>(imm6));
}

// This prototype maps to 3 instruction encodings:
//  LD1RH_z_p_bi_u16
//  LD1RH_z_p_bi_u32
//  LD1RH_z_p_bi_u64
void Assembler::ld1rh(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      int imm6) {
  // LD1RH { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
  //  1000 0100 11.. .... 101. .... .... ....
  //  dtypeh<24:23> = 01 | imm6<21:16> | dtypel<14:13> = 01 | Pg<12:10> |
  //  Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RH_z_p_bi_u16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<21, 16>(imm6));
}

// This prototype maps to 3 instruction encodings:
//  LD1RSB_z_p_bi_s16
//  LD1RSB_z_p_bi_s32
//  LD1RSB_z_p_bi_s64
void Assembler::ld1rsb(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm6) {
  // LD1RSB { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
  //  1000 0101 11.. .... 110. .... .... ....
  //  dtypeh<24:23> = 11 | imm6<21:16> | dtypel<14:13> = 10 | Pg<12:10> |
  //  Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RSB_z_p_bi_s16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<21, 16>(imm6));
}

// This prototype maps to 2 instruction encodings:
//  LD1RSH_z_p_bi_s32
//  LD1RSH_z_p_bi_s64
void Assembler::ld1rsh(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm6) {
  // LD1RSH { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
  //  1000 0101 01.. .... 101. .... .... ....
  //  dtypeh<24:23> = 10 | imm6<21:16> | dtypel<14:13> = 01 | Pg<12:10> |
  //  Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RSH_z_p_bi_s32 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<21, 16>(imm6));
}

void Assembler::ld1rsw(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm6) {
  // LD1RSW { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
  //  1000 0100 11.. .... 100. .... .... ....
  //  dtypeh<24:23> = 01 | imm6<21:16> | dtypel<14:13> = 00 | Pg<12:10> |
  //  Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RSW_z_p_bi_s64 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<21, 16>(imm6));
}

// This prototype maps to 2 instruction encodings:
//  LD1RW_z_p_bi_u32
//  LD1RW_z_p_bi_u64
void Assembler::ld1rw(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      int imm6) {
  // LD1RW { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
  //  1000 0101 01.. .... 110. .... .... ....
  //  dtypeh<24:23> = 10 | imm6<21:16> | dtypel<14:13> = 10 | Pg<12:10> |
  //  Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RW_z_p_bi_u32 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<21, 16>(imm6));
}

void Assembler::ldr(const PRegister& pt, const Register& xn) {
  // LDR <Pt>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1000 0101 10.. .... 000. .... ...0 ....
  //  imm9h<21:16> | imm9l<12:10> | Rn<9:5> | Pt<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDR_p_bi | Pd(pt) | RnSP(xn));
}

void Assembler::ldr(const ZRegisterNoLaneSize& zt, const Register& xn) {
  // LDR <Zt>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1000 0101 10.. .... 010. .... .... ....
  //  imm9h<21:16> | imm9l<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDR_z_bi | Rt(zt) | RnSP(xn));
}

void Assembler::prfb(int prfop,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // PRFB <prfop>, <Pg>, [<Xn|SP>, <Xm>]
  //  1000 0100 000. .... 110. .... ...0 ....
  //  msz<24:23> = 00 | Rm<20:16> | Pg<12:10> | Rn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFB_i_p_br_s | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) | RnSP(xn) |
       Rm(rm));
}

void Assembler::prfb(int prfop,
                     const PRegister& pg,
                     const Register& xn,
                     int imm6) {
  // PRFB <prfop>, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1000 0101 11.. .... 000. .... ...0 ....
  //  imm6<21:16> | msz<14:13> = 00 | Pg<12:10> | Rn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFB_i_p_bi_s | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<21, 16>(imm6));
}

void Assembler::prfd(int prfop,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // PRFD <prfop>, <Pg>, [<Xn|SP>, <Xm>, LSL #3]
  //  1000 0101 100. .... 110. .... ...0 ....
  //  msz<24:23> = 11 | Rm<20:16> | Pg<12:10> | Rn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFD_i_p_br_s | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) | RnSP(xn) |
       Rm(rm));
}

void Assembler::prfd(int prfop,
                     const PRegister& pg,
                     const Register& xn,
                     int imm6) {
  // PRFD <prfop>, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1000 0101 11.. .... 011. .... ...0 ....
  //  imm6<21:16> | msz<14:13> = 11 | Pg<12:10> | Rn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFD_i_p_bi_s | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<21, 16>(imm6));
}

void Assembler::prfh(int prfop,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // PRFH <prfop>, <Pg>, [<Xn|SP>, <Xm>, LSL #1]
  //  1000 0100 100. .... 110. .... ...0 ....
  //  msz<24:23> = 01 | Rm<20:16> | Pg<12:10> | Rn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFH_i_p_br_s | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) | RnSP(xn) |
       Rm(rm));
}

void Assembler::prfh(int prfop,
                     const PRegister& pg,
                     const Register& xn,
                     int imm6) {
  // PRFH <prfop>, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1000 0101 11.. .... 001. .... ...0 ....
  //  imm6<21:16> | msz<14:13> = 01 | Pg<12:10> | Rn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFH_i_p_bi_s | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<21, 16>(imm6));
}

void Assembler::prfw(int prfop,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // PRFW <prfop>, <Pg>, [<Xn|SP>, <Xm>, LSL #2]
  //  1000 0101 000. .... 110. .... ...0 ....
  //  msz<24:23> = 10 | Rm<20:16> | Pg<12:10> | Rn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFW_i_p_br_s | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) | RnSP(xn) |
       Rm(rm));
}

void Assembler::prfw(int prfop,
                     const PRegister& pg,
                     const Register& xn,
                     int imm6) {
  // PRFW <prfop>, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1000 0101 11.. .... 010. .... ...0 ....
  //  imm6<21:16> | msz<14:13> = 10 | Pg<12:10> | Rn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFW_i_p_bi_s | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<21, 16>(imm6));
}

// SVEMem64BitGather.

// This prototype maps to 3 instruction encodings:
//  LD1B_z_p_bz_d_64_unscaled
//  LD1B_z_p_bz_d_x32_unscaled
//  LD1B_z_p_bz_s_x32_unscaled
void Assembler::ld1b(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const ZRegister& zm) {
  // LD1B { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
  //  1100 0100 010. .... 110. .... .... ....
  //  msz<24:23> = 00 | Zm<20:16> | U<14> = 1 | ff<13> = 0 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1B_z_p_bz_d_64_unscaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  LD1B_z_p_ai_d
//  LD1B_z_p_ai_s
void Assembler::ld1b(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const ZRegister& zn,
                     int imm5) {
  // LD1B { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0100 001. .... 110. .... .... ....
  //  msz<24:23> = 00 | imm5<20:16> | U<14> = 1 | ff<13> = 0 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1B_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 4 instruction encodings:
//  LD1D_z_p_bz_d_64_scaled
//  LD1D_z_p_bz_d_64_unscaled
//  LD1D_z_p_bz_d_x32_scaled
//  LD1D_z_p_bz_d_x32_unscaled
void Assembler::ld1d(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const ZRegister& zm) {
  // LD1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #3]
  //  1100 0101 111. .... 110. .... .... ....
  //  msz<24:23> = 11 | Zm<20:16> | U<14> = 1 | ff<13> = 0 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1D_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

void Assembler::ld1d(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const ZRegister& zn,
                     int imm5) {
  // LD1D { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0101 101. .... 110. .... .... ....
  //  msz<24:23> = 11 | imm5<20:16> | U<14> = 1 | ff<13> = 0 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1D_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 6 instruction encodings:
//  LD1H_z_p_bz_d_64_scaled
//  LD1H_z_p_bz_d_64_unscaled
//  LD1H_z_p_bz_d_x32_scaled
//  LD1H_z_p_bz_d_x32_unscaled
//  LD1H_z_p_bz_s_x32_scaled
//  LD1H_z_p_bz_s_x32_unscaled
void Assembler::ld1h(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const ZRegister& zm) {
  // LD1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #1]
  //  1100 0100 111. .... 110. .... .... ....
  //  msz<24:23> = 01 | Zm<20:16> | U<14> = 1 | ff<13> = 0 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1H_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  LD1H_z_p_ai_d
//  LD1H_z_p_ai_s
void Assembler::ld1h(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const ZRegister& zn,
                     int imm5) {
  // LD1H { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0100 101. .... 110. .... .... ....
  //  msz<24:23> = 01 | imm5<20:16> | U<14> = 1 | ff<13> = 0 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1H_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 3 instruction encodings:
//  LD1SB_z_p_bz_d_64_unscaled
//  LD1SB_z_p_bz_d_x32_unscaled
//  LD1SB_z_p_bz_s_x32_unscaled
void Assembler::ld1sb(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      const ZRegister& zm) {
  // LD1SB { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
  //  1100 0100 010. .... 100. .... .... ....
  //  msz<24:23> = 00 | Zm<20:16> | U<14> = 0 | ff<13> = 0 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1SB_z_p_bz_d_64_unscaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  LD1SB_z_p_ai_d
//  LD1SB_z_p_ai_s
void Assembler::ld1sb(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm5) {
  // LD1SB { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0100 001. .... 100. .... .... ....
  //  msz<24:23> = 00 | imm5<20:16> | U<14> = 0 | ff<13> = 0 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1SB_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 6 instruction encodings:
//  LD1SH_z_p_bz_d_64_scaled
//  LD1SH_z_p_bz_d_64_unscaled
//  LD1SH_z_p_bz_d_x32_scaled
//  LD1SH_z_p_bz_d_x32_unscaled
//  LD1SH_z_p_bz_s_x32_scaled
//  LD1SH_z_p_bz_s_x32_unscaled
void Assembler::ld1sh(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      const ZRegister& zm) {
  // LD1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #1]
  //  1100 0100 111. .... 100. .... .... ....
  //  msz<24:23> = 01 | Zm<20:16> | U<14> = 0 | ff<13> = 0 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1SH_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  LD1SH_z_p_ai_d
//  LD1SH_z_p_ai_s
void Assembler::ld1sh(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm5) {
  // LD1SH { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0100 101. .... 100. .... .... ....
  //  msz<24:23> = 01 | imm5<20:16> | U<14> = 0 | ff<13> = 0 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1SH_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 4 instruction encodings:
//  LD1SW_z_p_bz_d_64_scaled
//  LD1SW_z_p_bz_d_64_unscaled
//  LD1SW_z_p_bz_d_x32_scaled
//  LD1SW_z_p_bz_d_x32_unscaled
void Assembler::ld1sw(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      const ZRegister& zm) {
  // LD1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #2]
  //  1100 0101 011. .... 100. .... .... ....
  //  msz<24:23> = 10 | Zm<20:16> | U<14> = 0 | ff<13> = 0 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1SW_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

void Assembler::ld1sw(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const ZRegister& zn,
                      int imm5) {
  // LD1SW { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0101 001. .... 100. .... .... ....
  //  msz<24:23> = 10 | imm5<20:16> | U<14> = 0 | ff<13> = 0 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1SW_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 6 instruction encodings:
//  LD1W_z_p_bz_d_64_scaled
//  LD1W_z_p_bz_d_64_unscaled
//  LD1W_z_p_bz_d_x32_scaled
//  LD1W_z_p_bz_d_x32_unscaled
//  LD1W_z_p_bz_s_x32_scaled
//  LD1W_z_p_bz_s_x32_unscaled
void Assembler::ld1w(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const ZRegister& zm) {
  // LD1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #2]
  //  1100 0101 011. .... 110. .... .... ....
  //  msz<24:23> = 10 | Zm<20:16> | U<14> = 1 | ff<13> = 0 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1W_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  LD1W_z_p_ai_d
//  LD1W_z_p_ai_s
void Assembler::ld1w(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const ZRegister& zn,
                     int imm5) {
  // LD1W { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0101 001. .... 110. .... .... ....
  //  msz<24:23> = 10 | imm5<20:16> | U<14> = 1 | ff<13> = 0 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1W_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 3 instruction encodings:
//  LDFF1B_z_p_bz_d_64_unscaled
//  LDFF1B_z_p_bz_d_x32_unscaled
//  LDFF1B_z_p_bz_s_x32_unscaled
void Assembler::ldff1b(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const ZRegister& zm) {
  // LDFF1B { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
  //  1100 0100 010. .... 111. .... .... ....
  //  msz<24:23> = 00 | Zm<20:16> | U<14> = 1 | ff<13> = 1 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1B_z_p_bz_d_64_unscaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  LDFF1B_z_p_ai_d
//  LDFF1B_z_p_ai_s
void Assembler::ldff1b(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const ZRegister& zn,
                       int imm5) {
  // LDFF1B { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0100 001. .... 111. .... .... ....
  //  msz<24:23> = 00 | imm5<20:16> | U<14> = 1 | ff<13> = 1 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1B_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 4 instruction encodings:
//  LDFF1D_z_p_bz_d_64_scaled
//  LDFF1D_z_p_bz_d_64_unscaled
//  LDFF1D_z_p_bz_d_x32_scaled
//  LDFF1D_z_p_bz_d_x32_unscaled
void Assembler::ldff1d(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const ZRegister& zm) {
  // LDFF1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #3]
  //  1100 0101 111. .... 111. .... .... ....
  //  msz<24:23> = 11 | Zm<20:16> | U<14> = 1 | ff<13> = 1 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1D_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

void Assembler::ldff1d(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const ZRegister& zn,
                       int imm5) {
  // LDFF1D { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0101 101. .... 111. .... .... ....
  //  msz<24:23> = 11 | imm5<20:16> | U<14> = 1 | ff<13> = 1 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1D_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 6 instruction encodings:
//  LDFF1H_z_p_bz_d_64_scaled
//  LDFF1H_z_p_bz_d_64_unscaled
//  LDFF1H_z_p_bz_d_x32_scaled
//  LDFF1H_z_p_bz_d_x32_unscaled
//  LDFF1H_z_p_bz_s_x32_scaled
//  LDFF1H_z_p_bz_s_x32_unscaled
void Assembler::ldff1h(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const ZRegister& zm) {
  // LDFF1H { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #1]
  //  1100 0100 111. .... 111. .... .... ....
  //  msz<24:23> = 01 | Zm<20:16> | U<14> = 1 | ff<13> = 1 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1H_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  LDFF1H_z_p_ai_d
//  LDFF1H_z_p_ai_s
void Assembler::ldff1h(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const ZRegister& zn,
                       int imm5) {
  // LDFF1H { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0100 101. .... 111. .... .... ....
  //  msz<24:23> = 01 | imm5<20:16> | U<14> = 1 | ff<13> = 1 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1H_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 3 instruction encodings:
//  LDFF1SB_z_p_bz_d_64_unscaled
//  LDFF1SB_z_p_bz_d_x32_unscaled
//  LDFF1SB_z_p_bz_s_x32_unscaled
void Assembler::ldff1sb(const ZRegister& zt,
                        const PRegisterZ& pg,
                        const Register& xn,
                        const ZRegister& zm) {
  // LDFF1SB { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D]
  //  1100 0100 010. .... 101. .... .... ....
  //  msz<24:23> = 00 | Zm<20:16> | U<14> = 0 | ff<13> = 1 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1SB_z_p_bz_d_64_unscaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  LDFF1SB_z_p_ai_d
//  LDFF1SB_z_p_ai_s
void Assembler::ldff1sb(const ZRegister& zt,
                        const PRegisterZ& pg,
                        const ZRegister& zn,
                        int imm5) {
  // LDFF1SB { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0100 001. .... 101. .... .... ....
  //  msz<24:23> = 00 | imm5<20:16> | U<14> = 0 | ff<13> = 1 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1SB_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 6 instruction encodings:
//  LDFF1SH_z_p_bz_d_64_scaled
//  LDFF1SH_z_p_bz_d_64_unscaled
//  LDFF1SH_z_p_bz_d_x32_scaled
//  LDFF1SH_z_p_bz_d_x32_unscaled
//  LDFF1SH_z_p_bz_s_x32_scaled
//  LDFF1SH_z_p_bz_s_x32_unscaled
void Assembler::ldff1sh(const ZRegister& zt,
                        const PRegisterZ& pg,
                        const Register& xn,
                        const ZRegister& zm) {
  // LDFF1SH { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #1]
  //  1100 0100 111. .... 101. .... .... ....
  //  msz<24:23> = 01 | Zm<20:16> | U<14> = 0 | ff<13> = 1 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1SH_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  LDFF1SH_z_p_ai_d
//  LDFF1SH_z_p_ai_s
void Assembler::ldff1sh(const ZRegister& zt,
                        const PRegisterZ& pg,
                        const ZRegister& zn,
                        int imm5) {
  // LDFF1SH { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0100 101. .... 101. .... .... ....
  //  msz<24:23> = 01 | imm5<20:16> | U<14> = 0 | ff<13> = 1 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1SH_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 4 instruction encodings:
//  LDFF1SW_z_p_bz_d_64_scaled
//  LDFF1SW_z_p_bz_d_64_unscaled
//  LDFF1SW_z_p_bz_d_x32_scaled
//  LDFF1SW_z_p_bz_d_x32_unscaled
void Assembler::ldff1sw(const ZRegister& zt,
                        const PRegisterZ& pg,
                        const Register& xn,
                        const ZRegister& zm) {
  // LDFF1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #2]
  //  1100 0101 011. .... 101. .... .... ....
  //  msz<24:23> = 10 | Zm<20:16> | U<14> = 0 | ff<13> = 1 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1SW_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       Rm(zm));
}

void Assembler::ldff1sw(const ZRegister& zt,
                        const PRegisterZ& pg,
                        const ZRegister& zn,
                        int imm5) {
  // LDFF1SW { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0101 001. .... 101. .... .... ....
  //  msz<24:23> = 10 | imm5<20:16> | U<14> = 0 | ff<13> = 1 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1SW_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 6 instruction encodings:
//  LDFF1W_z_p_bz_d_64_scaled
//  LDFF1W_z_p_bz_d_64_unscaled
//  LDFF1W_z_p_bz_d_x32_scaled
//  LDFF1W_z_p_bz_d_x32_unscaled
//  LDFF1W_z_p_bz_s_x32_scaled
//  LDFF1W_z_p_bz_s_x32_unscaled
void Assembler::ldff1w(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const ZRegister& zm) {
  // LDFF1W { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Zm>.D, LSL #2]
  //  1100 0101 011. .... 111. .... .... ....
  //  msz<24:23> = 10 | Zm<20:16> | U<14> = 1 | ff<13> = 1 | Pg<12:10> | Rn<9:5>
  //  | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1W_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  LDFF1W_z_p_ai_d
//  LDFF1W_z_p_ai_s
void Assembler::ldff1w(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const ZRegister& zn,
                       int imm5) {
  // LDFF1W { <Zt>.D }, <Pg>/Z, [<Zn>.D{, #<imm>}]
  //  1100 0101 001. .... 111. .... .... ....
  //  msz<24:23> = 10 | imm5<20:16> | U<14> = 1 | ff<13> = 1 | Pg<12:10> |
  //  Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1W_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 3 instruction encodings:
//  PRFB_i_p_bz_d_64_scaled
//  PRFB_i_p_bz_d_x32_scaled
//  PRFB_i_p_bz_s_x32_scaled
void Assembler::prfb(int prfop,
                     const PRegister& pg,
                     const Register& xn,
                     const ZRegister& zm) {
  // PRFB <prfop>, <Pg>, [<Xn|SP>, <Zm>.D]
  //  1100 0100 011. .... 100. .... ...0 ....
  //  Zm<20:16> | msz<14:13> = 00 | Pg<12:10> | Rn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFB_i_p_bz_d_64_scaled | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) |
       RnSP(xn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  PRFB_i_p_ai_d
//  PRFB_i_p_ai_s
void Assembler::prfb(int prfop,
                     const PRegister& pg,
                     const ZRegister& zn,
                     int imm5) {
  // PRFB <prfop>, <Pg>, [<Zn>.D{, #<imm>}]
  //  1100 0100 000. .... 111. .... ...0 ....
  //  msz<24:23> = 00 | imm5<20:16> | Pg<12:10> | Zn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFB_i_p_ai_d | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 3 instruction encodings:
//  PRFD_i_p_bz_d_64_scaled
//  PRFD_i_p_bz_d_x32_scaled
//  PRFD_i_p_bz_s_x32_scaled
void Assembler::prfd(int prfop,
                     const PRegister& pg,
                     const Register& xn,
                     const ZRegister& zm) {
  // PRFD <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, LSL #3]
  //  1100 0100 011. .... 111. .... ...0 ....
  //  Zm<20:16> | msz<14:13> = 11 | Pg<12:10> | Rn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFD_i_p_bz_d_64_scaled | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) |
       RnSP(xn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  PRFD_i_p_ai_d
//  PRFD_i_p_ai_s
void Assembler::prfd(int prfop,
                     const PRegister& pg,
                     const ZRegister& zn,
                     int imm5) {
  // PRFD <prfop>, <Pg>, [<Zn>.D{, #<imm>}]
  //  1100 0101 100. .... 111. .... ...0 ....
  //  msz<24:23> = 11 | imm5<20:16> | Pg<12:10> | Zn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFD_i_p_ai_d | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 3 instruction encodings:
//  PRFH_i_p_bz_d_64_scaled
//  PRFH_i_p_bz_d_x32_scaled
//  PRFH_i_p_bz_s_x32_scaled
void Assembler::prfh(int prfop,
                     const PRegister& pg,
                     const Register& xn,
                     const ZRegister& zm) {
  // PRFH <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, LSL #1]
  //  1100 0100 011. .... 101. .... ...0 ....
  //  Zm<20:16> | msz<14:13> = 01 | Pg<12:10> | Rn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFH_i_p_bz_d_64_scaled | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) |
       RnSP(xn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  PRFH_i_p_ai_d
//  PRFH_i_p_ai_s
void Assembler::prfh(int prfop,
                     const PRegister& pg,
                     const ZRegister& zn,
                     int imm5) {
  // PRFH <prfop>, <Pg>, [<Zn>.D{, #<imm>}]
  //  1100 0100 100. .... 111. .... ...0 ....
  //  msz<24:23> = 01 | imm5<20:16> | Pg<12:10> | Zn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFH_i_p_ai_d | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// This prototype maps to 3 instruction encodings:
//  PRFW_i_p_bz_d_64_scaled
//  PRFW_i_p_bz_d_x32_scaled
//  PRFW_i_p_bz_s_x32_scaled
void Assembler::prfw(int prfop,
                     const PRegister& pg,
                     const Register& xn,
                     const ZRegister& zm) {
  // PRFW <prfop>, <Pg>, [<Xn|SP>, <Zm>.D, LSL #2]
  //  1100 0100 011. .... 110. .... ...0 ....
  //  Zm<20:16> | msz<14:13> = 10 | Pg<12:10> | Rn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFW_i_p_bz_d_64_scaled | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) |
       RnSP(xn) | Rm(zm));
}

// This prototype maps to 2 instruction encodings:
//  PRFW_i_p_ai_d
//  PRFW_i_p_ai_s
void Assembler::prfw(int prfop,
                     const PRegister& pg,
                     const ZRegister& zn,
                     int imm5) {
  // PRFW <prfop>, <Pg>, [<Zn>.D{, #<imm>}]
  //  1100 0101 000. .... 111. .... ...0 ....
  //  msz<24:23> = 10 | imm5<20:16> | Pg<12:10> | Zn<9:5> | prfop<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PRFW_i_p_ai_d | ImmField<3, 0>(prfop) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

// SVEMemContiguousLoad.

// This prototype maps to 4 instruction encodings:
//  LD1B_z_p_br_u16
//  LD1B_z_p_br_u32
//  LD1B_z_p_br_u64
//  LD1B_z_p_br_u8
void Assembler::ld1b(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD1B { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>]
  //  1010 0100 001. .... 010. .... .... ....
  //  dtype<24:21> = 0001 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1B_z_p_br_u16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 4 instruction encodings:
//  LD1B_z_p_bi_u16
//  LD1B_z_p_bi_u32
//  LD1B_z_p_bi_u64
//  LD1B_z_p_bi_u8
void Assembler::ld1b(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD1B { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0100 0010 .... 101. .... .... ....
  //  dtype<24:21> = 0001 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1B_z_p_bi_u16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld1d(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]
  //  1010 0101 111. .... 010. .... .... ....
  //  dtype<24:21> = 1111 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1D_z_p_br_u64 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld1d(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 1110 .... 101. .... .... ....
  //  dtype<24:21> = 1111 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1D_z_p_bi_u64 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// This prototype maps to 3 instruction encodings:
//  LD1H_z_p_br_u16
//  LD1H_z_p_br_u32
//  LD1H_z_p_br_u64
void Assembler::ld1h(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD1H { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
  //  1010 0100 101. .... 010. .... .... ....
  //  dtype<24:21> = 0101 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1H_z_p_br_u16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 3 instruction encodings:
//  LD1H_z_p_bi_u16
//  LD1H_z_p_bi_u32
//  LD1H_z_p_bi_u64
void Assembler::ld1h(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD1H { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0100 1010 .... 101. .... .... ....
  //  dtype<24:21> = 0101 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1H_z_p_bi_u16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld1rqb(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const Register& rm) {
  // LD1RQB { <Zt>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
  //  1010 0100 000. .... 000. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 00 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RQB_z_p_br_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld1rqb(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm4) {
  // LD1RQB { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
  //  1010 0100 0000 .... 001. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 00 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RQB_z_p_bi_u8 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld1rqd(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const Register& rm) {
  // LD1RQD { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]
  //  1010 0101 100. .... 000. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 00 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RQD_z_p_br_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld1rqd(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm4) {
  // LD1RQD { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
  //  1010 0101 1000 .... 001. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 00 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RQD_z_p_bi_u64 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld1rqh(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const Register& rm) {
  // LD1RQH { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
  //  1010 0100 100. .... 000. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 00 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RQH_z_p_br_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld1rqh(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm4) {
  // LD1RQH { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
  //  1010 0100 1000 .... 001. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 00 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RQH_z_p_bi_u16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld1rqw(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const Register& rm) {
  // LD1RQW { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
  //  1010 0101 000. .... 000. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 00 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RQW_z_p_br_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld1rqw(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm4) {
  // LD1RQW { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>}]
  //  1010 0101 0000 .... 001. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 00 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1RQW_z_p_bi_u32 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// This prototype maps to 3 instruction encodings:
//  LD1SB_z_p_br_s16
//  LD1SB_z_p_br_s32
//  LD1SB_z_p_br_s64
void Assembler::ld1sb(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      const Register& rm) {
  // LD1SB { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>]
  //  1010 0101 110. .... 010. .... .... ....
  //  dtype<24:21> = 1110 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1SB_z_p_br_s16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 3 instruction encodings:
//  LD1SB_z_p_bi_s16
//  LD1SB_z_p_bi_s32
//  LD1SB_z_p_bi_s64
void Assembler::ld1sb(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      int imm4) {
  // LD1SB { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 1100 .... 101. .... .... ....
  //  dtype<24:21> = 1110 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1SB_z_p_bi_s16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// This prototype maps to 2 instruction encodings:
//  LD1SH_z_p_br_s32
//  LD1SH_z_p_br_s64
void Assembler::ld1sh(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      const Register& rm) {
  // LD1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
  //  1010 0101 001. .... 010. .... .... ....
  //  dtype<24:21> = 1001 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1SH_z_p_br_s32 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 2 instruction encodings:
//  LD1SH_z_p_bi_s32
//  LD1SH_z_p_bi_s64
void Assembler::ld1sh(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      int imm4) {
  // LD1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 0010 .... 101. .... .... ....
  //  dtype<24:21> = 1001 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1SH_z_p_bi_s32 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld1sw(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      const Register& rm) {
  // LD1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
  //  1010 0100 100. .... 010. .... .... ....
  //  dtype<24:21> = 0100 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1SW_z_p_br_s64 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld1sw(const ZRegister& zt,
                      const PRegisterZ& pg,
                      const Register& xn,
                      int imm4) {
  // LD1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0100 1000 .... 101. .... .... ....
  //  dtype<24:21> = 0100 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1SW_z_p_bi_s64 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// This prototype maps to 2 instruction encodings:
//  LD1W_z_p_br_u32
//  LD1W_z_p_br_u64
void Assembler::ld1w(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
  //  1010 0101 010. .... 010. .... .... ....
  //  dtype<24:21> = 1010 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1W_z_p_br_u32 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 2 instruction encodings:
//  LD1W_z_p_bi_u32
//  LD1W_z_p_bi_u64
void Assembler::ld1w(const ZRegister& zt,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 0100 .... 101. .... .... ....
  //  dtype<24:21> = 1010 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LD1W_z_p_bi_u32 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld2b(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD2B { <Zt1>.B, <Zt2>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
  //  1010 0100 001. .... 110. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 01 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(LD2B_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld2b(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD2B { <Zt1>.B, <Zt2>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0100 0010 .... 111. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 01 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(LD2B_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld2d(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD2D { <Zt1>.D, <Zt2>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]
  //  1010 0101 101. .... 110. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 01 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(LD2D_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld2d(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD2D { <Zt1>.D, <Zt2>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 1010 .... 111. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 01 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(LD2D_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld2h(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD2H { <Zt1>.H, <Zt2>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
  //  1010 0100 101. .... 110. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 01 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(LD2H_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld2h(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD2H { <Zt1>.H, <Zt2>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0100 1010 .... 111. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 01 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(LD2H_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld2w(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD2W { <Zt1>.S, <Zt2>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
  //  1010 0101 001. .... 110. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 01 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(LD2W_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld2w(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD2W { <Zt1>.S, <Zt2>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 0010 .... 111. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 01 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(LD2W_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld3b(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD3B { <Zt1>.B, <Zt2>.B, <Zt3>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
  //  1010 0100 010. .... 110. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 10 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(LD3B_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld3b(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD3B { <Zt1>.B, <Zt2>.B, <Zt3>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0100 0100 .... 111. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 10 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(LD3B_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld3d(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD3D { <Zt1>.D, <Zt2>.D, <Zt3>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]
  //  1010 0101 110. .... 110. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 10 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(LD3D_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld3d(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD3D { <Zt1>.D, <Zt2>.D, <Zt3>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 1100 .... 111. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 10 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(LD3D_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld3h(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD3H { <Zt1>.H, <Zt2>.H, <Zt3>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
  //  1010 0100 110. .... 110. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 10 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(LD3H_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld3h(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD3H { <Zt1>.H, <Zt2>.H, <Zt3>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0100 1100 .... 111. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 10 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(LD3H_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld3w(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD3W { <Zt1>.S, <Zt2>.S, <Zt3>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
  //  1010 0101 010. .... 110. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 10 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(LD3W_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld3w(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD3W { <Zt1>.S, <Zt2>.S, <Zt3>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 0100 .... 111. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 10 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(LD3W_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld4b(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD4B { <Zt1>.B, <Zt2>.B, <Zt3>.B, <Zt4>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
  //  1010 0100 011. .... 110. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 11 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(LD4B_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld4b(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD4B { <Zt1>.B, <Zt2>.B, <Zt3>.B, <Zt4>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL
  // VL}]
  //  1010 0100 0110 .... 111. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 11 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(LD4B_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld4d(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD4D { <Zt1>.D, <Zt2>.D, <Zt3>.D, <Zt4>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL
  // #3]
  //  1010 0101 111. .... 110. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 11 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(LD4D_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld4d(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD4D { <Zt1>.D, <Zt2>.D, <Zt3>.D, <Zt4>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL
  // VL}]
  //  1010 0101 1110 .... 111. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 11 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(LD4D_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld4h(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD4H { <Zt1>.H, <Zt2>.H, <Zt3>.H, <Zt4>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL
  // #1]
  //  1010 0100 111. .... 110. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 11 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(LD4H_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld4h(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD4H { <Zt1>.H, <Zt2>.H, <Zt3>.H, <Zt4>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL
  // VL}]
  //  1010 0100 1110 .... 111. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 11 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(LD4H_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ld4w(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegisterZ& pg,
                     const Register& xn,
                     const Register& rm) {
  // LD4W { <Zt1>.S, <Zt2>.S, <Zt3>.S, <Zt4>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL
  // #2]
  //  1010 0101 011. .... 110. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 11 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(LD4W_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ld4w(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegisterZ& pg,
                     const Register& xn,
                     int imm4) {
  // LD4W { <Zt1>.S, <Zt2>.S, <Zt3>.S, <Zt4>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL
  // VL}]
  //  1010 0101 0110 .... 111. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 11 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(LD4W_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// This prototype maps to 4 instruction encodings:
//  LDFF1B_z_p_br_u16
//  LDFF1B_z_p_br_u32
//  LDFF1B_z_p_br_u64
//  LDFF1B_z_p_br_u8
void Assembler::ldff1b(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const Register& rm) {
  // LDFF1B { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
  //  1010 0100 001. .... 011. .... .... ....
  //  dtype<24:21> = 0001 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1B_z_p_br_u16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ldff1d(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const Register& rm) {
  // LDFF1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #3}]
  //  1010 0101 111. .... 011. .... .... ....
  //  dtype<24:21> = 1111 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1D_z_p_br_u64 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 3 instruction encodings:
//  LDFF1H_z_p_br_u16
//  LDFF1H_z_p_br_u32
//  LDFF1H_z_p_br_u64
void Assembler::ldff1h(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const Register& rm) {
  // LDFF1H { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]
  //  1010 0100 101. .... 011. .... .... ....
  //  dtype<24:21> = 0101 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1H_z_p_br_u16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 3 instruction encodings:
//  LDFF1SB_z_p_br_s16
//  LDFF1SB_z_p_br_s32
//  LDFF1SB_z_p_br_s64
void Assembler::ldff1sb(const ZRegister& zt,
                        const PRegisterZ& pg,
                        const Register& xn,
                        const Register& rm) {
  // LDFF1SB { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, <Xm>}]
  //  1010 0101 110. .... 011. .... .... ....
  //  dtype<24:21> = 1110 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1SB_z_p_br_s16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 2 instruction encodings:
//  LDFF1SH_z_p_br_s32
//  LDFF1SH_z_p_br_s64
void Assembler::ldff1sh(const ZRegister& zt,
                        const PRegisterZ& pg,
                        const Register& xn,
                        const Register& rm) {
  // LDFF1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #1}]
  //  1010 0101 001. .... 011. .... .... ....
  //  dtype<24:21> = 1001 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1SH_z_p_br_s32 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ldff1sw(const ZRegister& zt,
                        const PRegisterZ& pg,
                        const Register& xn,
                        const Register& rm) {
  // LDFF1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #2}]
  //  1010 0100 100. .... 011. .... .... ....
  //  dtype<24:21> = 0100 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1SW_z_p_br_s64 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 2 instruction encodings:
//  LDFF1W_z_p_br_u32
//  LDFF1W_z_p_br_u64
void Assembler::ldff1w(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const Register& rm) {
  // LDFF1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, <Xm>, LSL #2}]
  //  1010 0101 010. .... 011. .... .... ....
  //  dtype<24:21> = 1010 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDFF1W_z_p_br_u32 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 4 instruction encodings:
//  LDNF1B_z_p_bi_u16
//  LDNF1B_z_p_bi_u32
//  LDNF1B_z_p_bi_u64
//  LDNF1B_z_p_bi_u8
void Assembler::ldnf1b(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm4) {
  // LDNF1B { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0100 0011 .... 101. .... .... ....
  //  dtype<24:21> = 0001 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNF1B_z_p_bi_u16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ldnf1d(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm4) {
  // LDNF1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 1111 .... 101. .... .... ....
  //  dtype<24:21> = 1111 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNF1D_z_p_bi_u64 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// This prototype maps to 3 instruction encodings:
//  LDNF1H_z_p_bi_u16
//  LDNF1H_z_p_bi_u32
//  LDNF1H_z_p_bi_u64
void Assembler::ldnf1h(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm4) {
  // LDNF1H { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0100 1011 .... 101. .... .... ....
  //  dtype<24:21> = 0101 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNF1H_z_p_bi_u16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// This prototype maps to 3 instruction encodings:
//  LDNF1SB_z_p_bi_s16
//  LDNF1SB_z_p_bi_s32
//  LDNF1SB_z_p_bi_s64
void Assembler::ldnf1sb(const ZRegister& zt,
                        const PRegisterZ& pg,
                        const Register& xn,
                        int imm4) {
  // LDNF1SB { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 1101 .... 101. .... .... ....
  //  dtype<24:21> = 1110 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNF1SB_z_p_bi_s16 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// This prototype maps to 2 instruction encodings:
//  LDNF1SH_z_p_bi_s32
//  LDNF1SH_z_p_bi_s64
void Assembler::ldnf1sh(const ZRegister& zt,
                        const PRegisterZ& pg,
                        const Register& xn,
                        int imm4) {
  // LDNF1SH { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 0011 .... 101. .... .... ....
  //  dtype<24:21> = 1001 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNF1SH_z_p_bi_s32 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ldnf1sw(const ZRegister& zt,
                        const PRegisterZ& pg,
                        const Register& xn,
                        int imm4) {
  // LDNF1SW { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0100 1001 .... 101. .... .... ....
  //  dtype<24:21> = 0100 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNF1SW_z_p_bi_s64 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// This prototype maps to 2 instruction encodings:
//  LDNF1W_z_p_bi_u32
//  LDNF1W_z_p_bi_u64
void Assembler::ldnf1w(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm4) {
  // LDNF1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 0101 .... 101. .... .... ....
  //  dtype<24:21> = 1010 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNF1W_z_p_bi_u32 | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ldnt1b(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const Register& rm) {
  // LDNT1B { <Zt>.B }, <Pg>/Z, [<Xn|SP>, <Xm>]
  //  1010 0100 000. .... 110. .... .... ....
  //  msz<24:23> = 00 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNT1B_z_p_br_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ldnt1b(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm4) {
  // LDNT1B { <Zt>.B }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0100 0000 .... 111. .... .... ....
  //  msz<24:23> = 00 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNT1B_z_p_bi_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ldnt1d(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const Register& rm) {
  // LDNT1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #3]
  //  1010 0101 100. .... 110. .... .... ....
  //  msz<24:23> = 11 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNT1D_z_p_br_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ldnt1d(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm4) {
  // LDNT1D { <Zt>.D }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 1000 .... 111. .... .... ....
  //  msz<24:23> = 11 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNT1D_z_p_bi_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ldnt1h(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const Register& rm) {
  // LDNT1H { <Zt>.H }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #1]
  //  1010 0100 100. .... 110. .... .... ....
  //  msz<24:23> = 01 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNT1H_z_p_br_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ldnt1h(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm4) {
  // LDNT1H { <Zt>.H }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0100 1000 .... 111. .... .... ....
  //  msz<24:23> = 01 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNT1H_z_p_bi_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::ldnt1w(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       const Register& rm) {
  // LDNT1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>, <Xm>, LSL #2]
  //  1010 0101 000. .... 110. .... .... ....
  //  msz<24:23> = 10 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNT1W_z_p_br_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::ldnt1w(const ZRegister& zt,
                       const PRegisterZ& pg,
                       const Register& xn,
                       int imm4) {
  // LDNT1W { <Zt>.S }, <Pg>/Z, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1010 0101 0000 .... 111. .... .... ....
  //  msz<24:23> = 10 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LDNT1W_z_p_bi_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// SVEMemStore.

void Assembler::st1b(const ZRegister& zt,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST1B { <Zt>.<T> }, <Pg>, [<Xn|SP>, <Xm>]
  //  1110 0100 0... .... 010. .... .... ....
  //  msz<24:23> = 00 | size<22:21> | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1B_z_p_br | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 3 instruction encodings:
//  ST1B_z_p_bz_d_64_unscaled
//  ST1B_z_p_bz_d_x32_unscaled
//  ST1B_z_p_bz_s_x32_unscaled
void Assembler::st1b(const ZRegister& zt,
                     const PRegister& pg,
                     const Register& xn,
                     const ZRegister& zm) {
  // ST1B { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D]
  //  1110 0100 000. .... 101. .... .... ....
  //  msz<24:23> = 00 | Zm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1B_z_p_bz_d_64_unscaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

void Assembler::st1b(const ZRegister& zt,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST1B { <Zt>.<T> }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0100 0..0 .... 111. .... .... ....
  //  msz<24:23> = 00 | size<22:21> | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1B_z_p_bi | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// This prototype maps to 2 instruction encodings:
//  ST1B_z_p_ai_d
//  ST1B_z_p_ai_s
void Assembler::st1b(const ZRegister& zt,
                     const PRegister& pg,
                     const ZRegister& zn,
                     int imm5) {
  // ST1B { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]
  //  1110 0100 010. .... 101. .... .... ....
  //  msz<24:23> = 00 | imm5<20:16> | Pg<12:10> | Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1B_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

void Assembler::st1d(const ZRegister& zt,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST1D { <Zt>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]
  //  1110 0101 1... .... 010. .... .... ....
  //  msz<24:23> = 11 | size<22:21> | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1D_z_p_bi | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 4 instruction encodings:
//  ST1D_z_p_bz_d_64_scaled
//  ST1D_z_p_bz_d_64_unscaled
//  ST1D_z_p_bz_d_x32_scaled
//  ST1D_z_p_bz_d_x32_unscaled
void Assembler::st1d(const ZRegister& zt,
                     const PRegister& pg,
                     const Register& xn,
                     const ZRegister& zm) {
  // ST1D { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, LSL #3]
  //  1110 0101 101. .... 101. .... .... ....
  //  msz<24:23> = 11 | Zm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1D_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

void Assembler::st1d(const ZRegister& zt,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST1D { <Zt>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0101 1..0 .... 111. .... .... ....
  //  msz<24:23> = 11 | size<22:21> | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1D_z_p_bi | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::st1d(const ZRegister& zt,
                     const PRegister& pg,
                     const ZRegister& zn,
                     int imm5) {
  // ST1D { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]
  //  1110 0101 110. .... 101. .... .... ....
  //  msz<24:23> = 11 | imm5<20:16> | Pg<12:10> | Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1D_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

void Assembler::st1h(const ZRegister& zt,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST1H { <Zt>.<T> }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]
  //  1110 0100 1... .... 010. .... .... ....
  //  msz<24:23> = 01 | size<22:21> | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1H_z_p_br | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 6 instruction encodings:
//  ST1H_z_p_bz_d_64_scaled
//  ST1H_z_p_bz_d_64_unscaled
//  ST1H_z_p_bz_d_x32_scaled
//  ST1H_z_p_bz_d_x32_unscaled
//  ST1H_z_p_bz_s_x32_scaled
//  ST1H_z_p_bz_s_x32_unscaled
void Assembler::st1h(const ZRegister& zt,
                     const PRegister& pg,
                     const Register& xn,
                     const ZRegister& zm) {
  // ST1H { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, LSL #1]
  //  1110 0100 101. .... 101. .... .... ....
  //  msz<24:23> = 01 | Zm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1H_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

void Assembler::st1h(const ZRegister& zt,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST1H { <Zt>.<T> }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0100 1..0 .... 111. .... .... ....
  //  msz<24:23> = 01 | size<22:21> | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1H_z_p_bi | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// This prototype maps to 2 instruction encodings:
//  ST1H_z_p_ai_d
//  ST1H_z_p_ai_s
void Assembler::st1h(const ZRegister& zt,
                     const PRegister& pg,
                     const ZRegister& zn,
                     int imm5) {
  // ST1H { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]
  //  1110 0100 110. .... 101. .... .... ....
  //  msz<24:23> = 01 | imm5<20:16> | Pg<12:10> | Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1H_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

void Assembler::st1w(const ZRegister& zt,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST1W { <Zt>.<T> }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]
  //  1110 0101 0... .... 010. .... .... ....
  //  msz<24:23> = 10 | size<22:21> | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1W_z_p_br | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

// This prototype maps to 6 instruction encodings:
//  ST1W_z_p_bz_d_64_scaled
//  ST1W_z_p_bz_d_64_unscaled
//  ST1W_z_p_bz_d_x32_scaled
//  ST1W_z_p_bz_d_x32_unscaled
//  ST1W_z_p_bz_s_x32_scaled
//  ST1W_z_p_bz_s_x32_unscaled
void Assembler::st1w(const ZRegister& zt,
                     const PRegister& pg,
                     const Register& xn,
                     const ZRegister& zm) {
  // ST1W { <Zt>.D }, <Pg>, [<Xn|SP>, <Zm>.D, LSL #2]
  //  1110 0101 001. .... 101. .... .... ....
  //  msz<24:23> = 10 | Zm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1W_z_p_bz_d_64_scaled | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(zm));
}

void Assembler::st1w(const ZRegister& zt,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST1W { <Zt>.<T> }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0101 0..0 .... 111. .... .... ....
  //  msz<24:23> = 10 | size<22:21> | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1W_z_p_bi | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

// This prototype maps to 2 instruction encodings:
//  ST1W_z_p_ai_d
//  ST1W_z_p_ai_s
void Assembler::st1w(const ZRegister& zt,
                     const PRegister& pg,
                     const ZRegister& zn,
                     int imm5) {
  // ST1W { <Zt>.D }, <Pg>, [<Zn>.D{, #<imm>}]
  //  1110 0101 010. .... 101. .... .... ....
  //  msz<24:23> = 10 | imm5<20:16> | Pg<12:10> | Zn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ST1W_z_p_ai_d | Rt(zt) | Rx<12, 10>(pg) | Rn(zn) |
       ImmField<20, 16>(imm5));
}

void Assembler::st2b(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST2B { <Zt1>.B, <Zt2>.B }, <Pg>, [<Xn|SP>, <Xm>]
  //  1110 0100 001. .... 011. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 01 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(ST2B_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::st2b(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST2B { <Zt1>.B, <Zt2>.B }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0100 0011 .... 111. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 01 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(ST2B_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::st2d(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST2D { <Zt1>.D, <Zt2>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]
  //  1110 0101 101. .... 011. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 01 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(ST2D_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::st2d(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST2D { <Zt1>.D, <Zt2>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0101 1011 .... 111. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 01 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(ST2D_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::st2h(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST2H { <Zt1>.H, <Zt2>.H }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]
  //  1110 0100 101. .... 011. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 01 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(ST2H_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::st2h(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST2H { <Zt1>.H, <Zt2>.H }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0100 1011 .... 111. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 01 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(ST2H_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::st2w(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST2W { <Zt1>.S, <Zt2>.S }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]
  //  1110 0101 001. .... 011. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 01 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(ST2W_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::st2w(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST2W { <Zt1>.S, <Zt2>.S }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0101 0011 .... 111. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 01 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2));

  Emit(ST2W_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::st3b(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST3B { <Zt1>.B, <Zt2>.B, <Zt3>.B }, <Pg>, [<Xn|SP>, <Xm>]
  //  1110 0100 010. .... 011. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 10 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(ST3B_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::st3b(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST3B { <Zt1>.B, <Zt2>.B, <Zt3>.B }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0100 0101 .... 111. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 10 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(ST3B_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::st3d(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST3D { <Zt1>.D, <Zt2>.D, <Zt3>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]
  //  1110 0101 110. .... 011. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 10 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(ST3D_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::st3d(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST3D { <Zt1>.D, <Zt2>.D, <Zt3>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0101 1101 .... 111. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 10 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(ST3D_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::st3h(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST3H { <Zt1>.H, <Zt2>.H, <Zt3>.H }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]
  //  1110 0100 110. .... 011. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 10 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(ST3H_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::st3h(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST3H { <Zt1>.H, <Zt2>.H, <Zt3>.H }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0100 1101 .... 111. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 10 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(ST3H_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::st3w(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST3W { <Zt1>.S, <Zt2>.S, <Zt3>.S }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]
  //  1110 0101 010. .... 011. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 10 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(ST3W_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::st3w(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST3W { <Zt1>.S, <Zt2>.S, <Zt3>.S }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0101 0101 .... 111. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 10 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3));

  Emit(ST3W_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::st4b(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST4B { <Zt1>.B, <Zt2>.B, <Zt3>.B, <Zt4>.B }, <Pg>, [<Xn|SP>, <Xm>]
  //  1110 0100 011. .... 011. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 11 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(ST4B_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::st4b(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST4B { <Zt1>.B, <Zt2>.B, <Zt3>.B, <Zt4>.B }, <Pg>, [<Xn|SP>{, #<imm>, MUL
  // VL}]
  //  1110 0100 0111 .... 111. .... .... ....
  //  msz<24:23> = 00 | num<22:21> = 11 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(ST4B_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::st4d(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST4D { <Zt1>.D, <Zt2>.D, <Zt3>.D, <Zt4>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]
  //  1110 0101 111. .... 011. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 11 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(ST4D_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::st4d(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST4D { <Zt1>.D, <Zt2>.D, <Zt3>.D, <Zt4>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL
  // VL}]
  //  1110 0101 1111 .... 111. .... .... ....
  //  msz<24:23> = 11 | num<22:21> = 11 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(ST4D_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::st4h(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST4H { <Zt1>.H, <Zt2>.H, <Zt3>.H, <Zt4>.H }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]
  //  1110 0100 111. .... 011. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 11 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(ST4H_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::st4h(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST4H { <Zt1>.H, <Zt2>.H, <Zt3>.H, <Zt4>.H }, <Pg>, [<Xn|SP>{, #<imm>, MUL
  // VL}]
  //  1110 0100 1111 .... 111. .... .... ....
  //  msz<24:23> = 01 | num<22:21> = 11 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(ST4H_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::st4w(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegister& pg,
                     const Register& xn,
                     const Register& rm) {
  // ST4W { <Zt1>.S, <Zt2>.S, <Zt3>.S, <Zt4>.S }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]
  //  1110 0101 011. .... 011. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 11 | Rm<20:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(ST4W_z_p_br_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::st4w(const ZRegister& zt1,
                     const ZRegister& zt2,
                     const ZRegister& zt3,
                     const ZRegister& zt4,
                     const PRegister& pg,
                     const Register& xn,
                     int imm4) {
  // ST4W { <Zt1>.S, <Zt2>.S, <Zt3>.S, <Zt4>.S }, <Pg>, [<Xn|SP>{, #<imm>, MUL
  // VL}]
  //  1110 0101 0111 .... 111. .... .... ....
  //  msz<24:23> = 10 | num<22:21> = 11 | imm4<19:16> | Pg<12:10> | Rn<9:5> |
  //  Zt<4:0>

  USE(zt2, zt3, zt4);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreConsecutive(zt1, zt2, zt3, zt4));

  Emit(ST4W_z_p_bi_contiguous | Rt(zt1) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::stnt1b(const ZRegister& zt,
                       const PRegister& pg,
                       const Register& xn,
                       const Register& rm) {
  // STNT1B { <Zt>.B }, <Pg>, [<Xn|SP>, <Xm>]
  //  1110 0100 000. .... 011. .... .... ....
  //  msz<24:23> = 00 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(STNT1B_z_p_br_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::stnt1b(const ZRegister& zt,
                       const PRegister& pg,
                       const Register& xn,
                       int imm4) {
  // STNT1B { <Zt>.B }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0100 0001 .... 111. .... .... ....
  //  msz<24:23> = 00 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(STNT1B_z_p_bi_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::stnt1d(const ZRegister& zt,
                       const PRegister& pg,
                       const Register& xn,
                       const Register& rm) {
  // STNT1D { <Zt>.D }, <Pg>, [<Xn|SP>, <Xm>, LSL #3]
  //  1110 0101 100. .... 011. .... .... ....
  //  msz<24:23> = 11 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(STNT1D_z_p_br_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::stnt1d(const ZRegister& zt,
                       const PRegister& pg,
                       const Register& xn,
                       int imm4) {
  // STNT1D { <Zt>.D }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0101 1001 .... 111. .... .... ....
  //  msz<24:23> = 11 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(STNT1D_z_p_bi_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::stnt1h(const ZRegister& zt,
                       const PRegister& pg,
                       const Register& xn,
                       const Register& rm) {
  // STNT1H { <Zt>.H }, <Pg>, [<Xn|SP>, <Xm>, LSL #1]
  //  1110 0100 100. .... 011. .... .... ....
  //  msz<24:23> = 01 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(STNT1H_z_p_br_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::stnt1h(const ZRegister& zt,
                       const PRegister& pg,
                       const Register& xn,
                       int imm4) {
  // STNT1H { <Zt>.H }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0100 1001 .... 111. .... .... ....
  //  msz<24:23> = 01 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(STNT1H_z_p_bi_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::stnt1w(const ZRegister& zt,
                       const PRegister& pg,
                       const Register& xn,
                       const Register& rm) {
  // STNT1W { <Zt>.S }, <Pg>, [<Xn|SP>, <Xm>, LSL #2]
  //  1110 0101 000. .... 011. .... .... ....
  //  msz<24:23> = 10 | Rm<20:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(STNT1W_z_p_br_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) | Rm(rm));
}

void Assembler::stnt1w(const ZRegister& zt,
                       const PRegister& pg,
                       const Register& xn,
                       int imm4) {
  // STNT1W { <Zt>.S }, <Pg>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0101 0001 .... 111. .... .... ....
  //  msz<24:23> = 10 | imm4<19:16> | Pg<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(STNT1W_z_p_bi_contiguous | Rt(zt) | Rx<12, 10>(pg) | RnSP(xn) |
       ImmField<19, 16>(imm4));
}

void Assembler::str(const PRegister& pt, const Register& xn) {
  // STR <Pt>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0101 10.. .... 000. .... ...0 ....
  //  imm9h<21:16> | imm9l<12:10> | Rn<9:5> | Pt<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(STR_p_bi | Pd(pt) | RnSP(xn));
}

void Assembler::str(const ZRegisterNoLaneSize& zt, const Register& xn) {
  // STR <Zt>, [<Xn|SP>{, #<imm>, MUL VL}]
  //  1110 0101 10.. .... 010. .... .... ....
  //  imm9h<21:16> | imm9l<12:10> | Rn<9:5> | Zt<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(STR_z_bi | Rt(zt) | RnSP(xn));
}

// SVEMulIndex.

// This prototype maps to 2 instruction encodings:
//  SDOT_z_zzzi_d
//  SDOT_z_zzzi_s
void Assembler::sdot(const ZRegister& zda, const ZRegister& zn) {
  // SDOT <Zda>.D, <Zn>.H, <Zm>.H[<imm>]
  //  0100 0100 111. .... 0000 00.. .... ....
  //  size<23:22> = 11 | opc<20:16> | U<10> = 0 | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SDOT_z_zzzi_d | Rd(zda) | Rn(zn));
}

// This prototype maps to 2 instruction encodings:
//  UDOT_z_zzzi_d
//  UDOT_z_zzzi_s
void Assembler::udot(const ZRegister& zda, const ZRegister& zn) {
  // UDOT <Zda>.D, <Zn>.H, <Zm>.H[<imm>]
  //  0100 0100 111. .... 0000 01.. .... ....
  //  size<23:22> = 11 | opc<20:16> | U<10> = 1 | Zn<9:5> | Zda<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UDOT_z_zzzi_d | Rd(zda) | Rn(zn));
}

// SVEPartitionBreak.

void Assembler::brka(const PRegisterWithLaneSize& pd,
                     const PRegister& pg,
                     const PRegisterWithLaneSize& pn) {
  // BRKA <Pd>.B, <Pg>/<ZM>, <Pn>.B
  //  0010 0101 0001 0000 01.. ..0. .... ....
  //  B<23> = 0 | S<22> = 0 | Pg<13:10> | Pn<8:5> | M<4> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(BRKA_p_p_p | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn));
}

void Assembler::brkas(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const PRegisterWithLaneSize& pn) {
  // BRKAS <Pd>.B, <Pg>/Z, <Pn>.B
  //  0010 0101 0101 0000 01.. ..0. ...0 ....
  //  B<23> = 0 | S<22> = 1 | Pg<13:10> | Pn<8:5> | M<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(BRKAS_p_p_p_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn));
}

void Assembler::brkb(const PRegisterWithLaneSize& pd,
                     const PRegister& pg,
                     const PRegisterWithLaneSize& pn) {
  // BRKB <Pd>.B, <Pg>/<ZM>, <Pn>.B
  //  0010 0101 1001 0000 01.. ..0. .... ....
  //  B<23> = 1 | S<22> = 0 | Pg<13:10> | Pn<8:5> | M<4> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(BRKB_p_p_p | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn));
}

void Assembler::brkbs(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const PRegisterWithLaneSize& pn) {
  // BRKBS <Pd>.B, <Pg>/Z, <Pn>.B
  //  0010 0101 1101 0000 01.. ..0. ...0 ....
  //  B<23> = 1 | S<22> = 1 | Pg<13:10> | Pn<8:5> | M<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(BRKBS_p_p_p_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn));
}

void Assembler::brkn(const PRegisterWithLaneSize& pd,
                     const PRegisterZ& pg,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // BRKN <Pdm>.B, <Pg>/Z, <Pn>.B, <Pdm>.B
  //  0010 0101 0001 1000 01.. ..0. ...0 ....
  //  S<22> = 0 | Pg<13:10> | Pn<8:5> | Pdm<3:0>

  USE(pm);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(pd.Is(pm));

  Emit(BRKN_p_p_pp | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn));
}

void Assembler::brkns(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const PRegisterWithLaneSize& pn,
                      const PRegisterWithLaneSize& pm) {
  // BRKNS <Pdm>.B, <Pg>/Z, <Pn>.B, <Pdm>.B
  //  0010 0101 0101 1000 01.. ..0. ...0 ....
  //  S<22> = 1 | Pg<13:10> | Pn<8:5> | Pdm<3:0>

  USE(pm);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(pd.Is(pm));

  Emit(BRKNS_p_p_pp | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn));
}

// SVEPermutePredicate.

void Assembler::punpkhi(const PRegisterWithLaneSize& pd,
                        const PRegisterWithLaneSize& pn) {
  // PUNPKHI <Pd>.H, <Pn>.B
  //  0000 0101 0011 0001 0100 000. ...0 ....
  //  H<16> = 1 | Pn<8:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PUNPKHI_p_p | Pd(pd) | Rx<8, 5>(pn));
}

void Assembler::punpklo(const PRegisterWithLaneSize& pd,
                        const PRegisterWithLaneSize& pn) {
  // PUNPKLO <Pd>.H, <Pn>.B
  //  0000 0101 0011 0000 0100 000. ...0 ....
  //  H<16> = 0 | Pn<8:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PUNPKLO_p_p | Pd(pd) | Rx<8, 5>(pn));
}

void Assembler::rev(const PRegisterWithLaneSize& pd,
                    const PRegisterWithLaneSize& pn) {
  // REV <Pd>.<T>, <Pn>.<T>
  //  0000 0101 ..11 0100 0100 000. ...0 ....
  //  size<23:22> | Pn<8:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(REV_p_p | Pd(pd) | Rx<8, 5>(pn));
}

void Assembler::trn1(const PRegisterWithLaneSize& pd,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // TRN1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
  //  0000 0101 ..10 .... 0101 000. ...0 ....
  //  size<23:22> | Pm<19:16> | opc<12:11> = 10 | H<10> = 0 | Pn<8:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(TRN1_p_pp | Pd(pd) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::trn2(const PRegisterWithLaneSize& pd,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // TRN2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
  //  0000 0101 ..10 .... 0101 010. ...0 ....
  //  size<23:22> | Pm<19:16> | opc<12:11> = 10 | H<10> = 1 | Pn<8:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(TRN2_p_pp | Pd(pd) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::uzp1(const PRegisterWithLaneSize& pd,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // UZP1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
  //  0000 0101 ..10 .... 0100 100. ...0 ....
  //  size<23:22> | Pm<19:16> | opc<12:11> = 01 | H<10> = 0 | Pn<8:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UZP1_p_pp | Pd(pd) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::uzp2(const PRegisterWithLaneSize& pd,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // UZP2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
  //  0000 0101 ..10 .... 0100 110. ...0 ....
  //  size<23:22> | Pm<19:16> | opc<12:11> = 01 | H<10> = 1 | Pn<8:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(UZP2_p_pp | Pd(pd) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::zip1(const PRegisterWithLaneSize& pd,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // ZIP1 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
  //  0000 0101 ..10 .... 0100 000. ...0 ....
  //  size<23:22> | Pm<19:16> | opc<12:11> = 00 | H<10> = 0 | Pn<8:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ZIP1_p_pp | Pd(pd) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::zip2(const PRegisterWithLaneSize& pd,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // ZIP2 <Pd>.<T>, <Pn>.<T>, <Pm>.<T>
  //  0000 0101 ..10 .... 0100 010. ...0 ....
  //  size<23:22> | Pm<19:16> | opc<12:11> = 00 | H<10> = 1 | Pn<8:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ZIP2_p_pp | Pd(pd) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

// SVEPermuteVectorExtract.

void Assembler::ext(const ZRegister& zd,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // EXT <Zdn>.B, <Zdn>.B, <Zm>.B, #<imm>
  //  0000 0101 001. .... 000. .... .... ....
  //  imm8h<20:16> | imm8l<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));

  Emit(EXT_z_zi_des | Rd(zd) | Rn(zm));
}

// SVEPermuteVectorInterleaving.

void Assembler::trn1(const ZRegister& zd,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // TRN1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0101 ..1. .... 0111 00.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 100 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(TRN1_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::trn2(const ZRegister& zd,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // TRN2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0101 ..1. .... 0111 01.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 101 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(TRN2_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::uzp1(const ZRegister& zd,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // UZP1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0101 ..1. .... 0110 10.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 010 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(UZP1_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::uzp2(const ZRegister& zd,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // UZP2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0101 ..1. .... 0110 11.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 011 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(UZP2_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::zip1(const ZRegister& zd,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // ZIP1 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0101 ..1. .... 0110 00.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 000 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(ZIP1_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::zip2(const ZRegister& zd,
                     const ZRegister& zn,
                     const ZRegister& zm) {
  // ZIP2 <Zd>.<T>, <Zn>.<T>, <Zm>.<T>
  //  0000 0101 ..1. .... 0110 01.. .... ....
  //  size<23:22> | Zm<20:16> | opc<12:10> = 001 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(ZIP2_z_zz | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

// SVEPermuteVectorPredicated.

void Assembler::clasta(const Register& rd,
                       const PRegister& pg,
                       const Register& rn,
                       const ZRegister& zm) {
  // CLASTA <R><dn>, <Pg>, <R><dn>, <Zm>.<T>
  //  0000 0101 ..11 0000 101. .... .... ....
  //  size<23:22> | B<16> = 0 | Pg<12:10> | Zm<9:5> | Rdn<4:0>

  USE(rn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(rd.Is(rn));

  Emit(CLASTA_r_p_z | SVESize(zm) | Rd(rd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::clasta(const VRegister& vd,
                       const PRegister& pg,
                       const VRegister& vn,
                       const ZRegister& zm) {
  // CLASTA <V><dn>, <Pg>, <V><dn>, <Zm>.<T>
  //  0000 0101 ..10 1010 100. .... .... ....
  //  size<23:22> | B<16> = 0 | Pg<12:10> | Zm<9:5> | Vdn<4:0>

  USE(vn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.Is(vn));
  VIXL_ASSERT(vd.IsScalar());
  VIXL_ASSERT(vn.IsScalar());

  Emit(CLASTA_v_p_z | SVESize(zm) | Rd(vd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::clasta(const ZRegister& zd,
                       const PRegister& pg,
                       const ZRegister& zn,
                       const ZRegister& zm) {
  // CLASTA <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>
  //  0000 0101 ..10 1000 100. .... .... ....
  //  size<23:22> | B<16> = 0 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(CLASTA_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::clastb(const Register& rd,
                       const PRegister& pg,
                       const Register& rn,
                       const ZRegister& zm) {
  // CLASTB <R><dn>, <Pg>, <R><dn>, <Zm>.<T>
  //  0000 0101 ..11 0001 101. .... .... ....
  //  size<23:22> | B<16> = 1 | Pg<12:10> | Zm<9:5> | Rdn<4:0>

  USE(rn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(rd.Is(rn));

  Emit(CLASTB_r_p_z | SVESize(zm) | Rd(rd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::clastb(const VRegister& vd,
                       const PRegister& pg,
                       const VRegister& vn,
                       const ZRegister& zm) {
  // CLASTB <V><dn>, <Pg>, <V><dn>, <Zm>.<T>
  //  0000 0101 ..10 1011 100. .... .... ....
  //  size<23:22> | B<16> = 1 | Pg<12:10> | Zm<9:5> | Vdn<4:0>

  USE(vn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.Is(vn));
  VIXL_ASSERT(vd.IsScalar());
  VIXL_ASSERT(vn.IsScalar());

  Emit(CLASTB_v_p_z | SVESize(zm) | Rd(vd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::clastb(const ZRegister& zd,
                       const PRegister& pg,
                       const ZRegister& zn,
                       const ZRegister& zm) {
  // CLASTB <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>
  //  0000 0101 ..10 1001 100. .... .... ....
  //  size<23:22> | B<16> = 1 | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(CLASTB_z_p_zz | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

void Assembler::compact(const ZRegister& zd,
                        const PRegister& pg,
                        const ZRegister& zn) {
  // COMPACT <Zd>.<T>, <Pg>, <Zn>.<T>
  //  0000 0101 1.10 0001 100. .... .... ....
  //  sz<22> | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(COMPACT_z_p_z | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::cpy(const ZRegister& zd,
                    const PRegisterM& pg,
                    const Register& xn) {
  // CPY <Zd>.<T>, <Pg>/M, <R><n|SP>
  //  0000 0101 ..10 1000 101. .... .... ....
  //  size<23:22> | Pg<12:10> | Rn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CPY_z_p_r | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | RnSP(xn));
}

void Assembler::cpy(const ZRegister& zd,
                    const PRegisterM& pg,
                    const VRegister& vn) {
  // CPY <Zd>.<T>, <Pg>/M, <V><n>
  //  0000 0101 ..10 0000 100. .... .... ....
  //  size<23:22> | Pg<12:10> | Vn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vn.IsScalar());

  Emit(CPY_z_p_v | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(vn));
}

void Assembler::lasta(const Register& rd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // LASTA <R><d>, <Pg>, <Zn>.<T>
  //  0000 0101 ..10 0000 101. .... .... ....
  //  size<23:22> | B<16> = 0 | Pg<12:10> | Zn<9:5> | Rd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LASTA_r_p_z | SVESize(zn) | Rd(rd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::lasta(const VRegister& vd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // LASTA <V><d>, <Pg>, <Zn>.<T>
  //  0000 0101 ..10 0010 100. .... .... ....
  //  size<23:22> | B<16> = 0 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());

  Emit(LASTA_v_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::lastb(const Register& rd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // LASTB <R><d>, <Pg>, <Zn>.<T>
  //  0000 0101 ..10 0001 101. .... .... ....
  //  size<23:22> | B<16> = 1 | Pg<12:10> | Zn<9:5> | Rd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(LASTB_r_p_z | SVESize(zn) | Rd(rd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::lastb(const VRegister& vd,
                      const PRegister& pg,
                      const ZRegister& zn) {
  // LASTB <V><d>, <Pg>, <Zn>.<T>
  //  0000 0101 ..10 0011 100. .... .... ....
  //  size<23:22> | B<16> = 1 | Pg<12:10> | Zn<9:5> | Vd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vd.IsScalar());

  Emit(LASTB_v_p_z | SVESize(zn) | Rd(vd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::rbit(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // RBIT <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0101 ..10 0111 100. .... .... ....
  //  size<23:22> | opc<17:16> = 11 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(RBIT_z_p_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::revb(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // REVB <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0101 ..10 0100 100. .... .... ....
  //  size<23:22> | opc<17:16> = 00 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(REVB_z_z | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::revh(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // REVH <Zd>.<T>, <Pg>/M, <Zn>.<T>
  //  0000 0101 ..10 0101 100. .... .... ....
  //  size<23:22> | opc<17:16> = 01 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(REVH_z_z | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::revw(const ZRegister& zd,
                     const PRegisterM& pg,
                     const ZRegister& zn) {
  // REVW <Zd>.D, <Pg>/M, <Zn>.D
  //  0000 0101 ..10 0110 100. .... .... ....
  //  size<23:22> | opc<17:16> = 10 | Pg<12:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(REVW_z_z | Rd(zd) | Rx<12, 10>(pg) | Rn(zn));
}

void Assembler::splice(const ZRegister& zd,
                       const PRegister& pg,
                       const ZRegister& zn,
                       const ZRegister& zm) {
  // SPLICE <Zdn>.<T>, <Pg>, <Zdn>.<T>, <Zm>.<T>
  //  0000 0101 ..10 1100 100. .... .... ....
  //  size<23:22> | Pg<12:10> | Zm<9:5> | Zdn<4:0>

  USE(zn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(zd.Is(zn));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(SPLICE_z_p_zz_des | SVESize(zd) | Rd(zd) | Rx<12, 10>(pg) | Rn(zm));
}

// SVEPermuteVectorUnpredicated.

void Assembler::dup(const ZRegister& zd, const Register& xn) {
  // DUP <Zd>.<T>, <R><n|SP>
  //  0000 0101 ..10 0000 0011 10.. .... ....
  //  size<23:22> | Rn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(DUP_z_r | SVESize(zd) | Rd(zd) | RnSP(xn));
}

void Assembler::dup(const ZRegister& zd, const ZRegister& zn) {
  // DUP <Zd>.<T>, <Zn>.<T>[<imm>]
  //  0000 0101 ..1. .... 0010 00.. .... ....
  //  imm2<23:22> | tsz<20:16> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(DUP_z_zi | Rd(zd) | Rn(zn));
}

void Assembler::insr(const ZRegister& zdn, const Register& rm) {
  // INSR <Zdn>.<T>, <R><m>
  //  0000 0101 ..10 0100 0011 10.. .... ....
  //  size<23:22> | Rm<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(INSR_z_r | SVESize(zdn) | Rd(zdn) | Rn(rm));
}

void Assembler::insr(const ZRegister& zdn, const VRegister& vm) {
  // INSR <Zdn>.<T>, <V><m>
  //  0000 0101 ..11 0100 0011 10.. .... ....
  //  size<23:22> | Vm<9:5> | Zdn<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(vm.IsScalar());

  Emit(INSR_z_v | SVESize(zdn) | Rd(zdn) | Rn(vm));
}

void Assembler::rev(const ZRegister& zd, const ZRegister& zn) {
  // REV <Zd>.<T>, <Zn>.<T>
  //  0000 0101 ..11 1000 0011 10.. .... ....
  //  size<23:22> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));

  Emit(REV_z_z | SVESize(zd) | Rd(zd) | Rn(zn));
}

void Assembler::sunpkhi(const ZRegister& zd, const ZRegister& zn) {
  // SUNPKHI <Zd>.<T>, <Zn>.<Tb>
  //  0000 0101 ..11 0001 0011 10.. .... ....
  //  size<23:22> | U<17> = 0 | H<16> = 1 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(SUNPKHI_z_z | SVESize(zd) | Rd(zd) | Rn(zn));
}

void Assembler::sunpklo(const ZRegister& zd, const ZRegister& zn) {
  // SUNPKLO <Zd>.<T>, <Zn>.<Tb>
  //  0000 0101 ..11 0000 0011 10.. .... ....
  //  size<23:22> | U<17> = 0 | H<16> = 0 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(SUNPKLO_z_z | SVESize(zd) | Rd(zd) | Rn(zn));
}

void Assembler::tbl(const ZRegister& zd,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // TBL <Zd>.<T>, { <Zn>.<T> }, <Zm>.<T>
  //  0000 0101 ..1. .... 0011 00.. .... ....
  //  size<23:22> | Zm<20:16> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(TBL_z_zz_1 | SVESize(zd) | Rd(zd) | Rn(zn) | Rm(zm));
}

void Assembler::uunpkhi(const ZRegister& zd, const ZRegister& zn) {
  // UUNPKHI <Zd>.<T>, <Zn>.<Tb>
  //  0000 0101 ..11 0011 0011 10.. .... ....
  //  size<23:22> | U<17> = 1 | H<16> = 1 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(UUNPKHI_z_z | SVESize(zd) | Rd(zd) | Rn(zn));
}

void Assembler::uunpklo(const ZRegister& zd, const ZRegister& zn) {
  // UUNPKLO <Zd>.<T>, <Zn>.<Tb>
  //  0000 0101 ..11 0010 0011 10.. .... ....
  //  size<23:22> | U<17> = 1 | H<16> = 0 | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn));
  VIXL_ASSERT(zd.GetLaneSizeInBytes() != kBRegSizeInBytes);

  Emit(UUNPKLO_z_z | SVESize(zd) | Rd(zd) | Rn(zn));
}

// SVEPredicateCount.

void Assembler::cntp(const Register& rd,
                     const PRegister& pg,
                     const PRegisterWithLaneSize& pn) {
  // CNTP <Xd>, <Pg>, <Pn>.<T>
  //  0010 0101 ..10 0000 10.. ..0. .... ....
  //  size<23:22> | opc<18:16> = 000 | Pg<13:10> | o2<9> = 0 | Pn<8:5> | Rd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(CNTP_r_p_p | Rd(rd) | Rx<13, 10>(pg) | Rx<8, 5>(pn));
}

// SVEPredicateLogicalOp.

void Assembler::and_(const PRegisterWithLaneSize& pd,
                     const PRegisterZ& pg,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // AND <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 0000 .... 01.. ..0. ...0 ....
  //  op<23> = 0 | S<22> = 0 | Pm<19:16> | Pg<13:10> | o2<9> = 0 | Pn<8:5> |
  //  o3<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(AND_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::ands(const PRegisterWithLaneSize& pd,
                     const PRegisterZ& pg,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // ANDS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 0100 .... 01.. ..0. ...0 ....
  //  op<23> = 0 | S<22> = 1 | Pm<19:16> | Pg<13:10> | o2<9> = 0 | Pn<8:5> |
  //  o3<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ANDS_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::bic(const PRegisterWithLaneSize& pd,
                    const PRegisterZ& pg,
                    const PRegisterWithLaneSize& pn,
                    const PRegisterWithLaneSize& pm) {
  // BIC <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 0000 .... 01.. ..0. ...1 ....
  //  op<23> = 0 | S<22> = 0 | Pm<19:16> | Pg<13:10> | o2<9> = 0 | Pn<8:5> |
  //  o3<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(BIC_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::bics(const PRegisterWithLaneSize& pd,
                     const PRegisterZ& pg,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // BICS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 0100 .... 01.. ..0. ...1 ....
  //  op<23> = 0 | S<22> = 1 | Pm<19:16> | Pg<13:10> | o2<9> = 0 | Pn<8:5> |
  //  o3<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(BICS_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::eor(const PRegisterWithLaneSize& pd,
                    const PRegisterZ& pg,
                    const PRegisterWithLaneSize& pn,
                    const PRegisterWithLaneSize& pm) {
  // EOR <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 0000 .... 01.. ..1. ...0 ....
  //  op<23> = 0 | S<22> = 0 | Pm<19:16> | Pg<13:10> | o2<9> = 1 | Pn<8:5> |
  //  o3<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(EOR_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::eors(const PRegisterWithLaneSize& pd,
                     const PRegisterZ& pg,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // EORS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 0100 .... 01.. ..1. ...0 ....
  //  op<23> = 0 | S<22> = 1 | Pm<19:16> | Pg<13:10> | o2<9> = 1 | Pn<8:5> |
  //  o3<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(EORS_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::nand(const PRegisterWithLaneSize& pd,
                     const PRegisterZ& pg,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // NAND <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 1000 .... 01.. ..1. ...1 ....
  //  op<23> = 1 | S<22> = 0 | Pm<19:16> | Pg<13:10> | o2<9> = 1 | Pn<8:5> |
  //  o3<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(NAND_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::nands(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const PRegisterWithLaneSize& pn,
                      const PRegisterWithLaneSize& pm) {
  // NANDS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 1100 .... 01.. ..1. ...1 ....
  //  op<23> = 1 | S<22> = 1 | Pm<19:16> | Pg<13:10> | o2<9> = 1 | Pn<8:5> |
  //  o3<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(NANDS_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) |
       Rx<19, 16>(pm));
}

void Assembler::nor(const PRegisterWithLaneSize& pd,
                    const PRegisterZ& pg,
                    const PRegisterWithLaneSize& pn,
                    const PRegisterWithLaneSize& pm) {
  // NOR <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 1000 .... 01.. ..1. ...0 ....
  //  op<23> = 1 | S<22> = 0 | Pm<19:16> | Pg<13:10> | o2<9> = 1 | Pn<8:5> |
  //  o3<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(NOR_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::nors(const PRegisterWithLaneSize& pd,
                     const PRegisterZ& pg,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // NORS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 1100 .... 01.. ..1. ...0 ....
  //  op<23> = 1 | S<22> = 1 | Pm<19:16> | Pg<13:10> | o2<9> = 1 | Pn<8:5> |
  //  o3<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(NORS_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::orn(const PRegisterWithLaneSize& pd,
                    const PRegisterZ& pg,
                    const PRegisterWithLaneSize& pn,
                    const PRegisterWithLaneSize& pm) {
  // ORN <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 1000 .... 01.. ..0. ...1 ....
  //  op<23> = 1 | S<22> = 0 | Pm<19:16> | Pg<13:10> | o2<9> = 0 | Pn<8:5> |
  //  o3<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ORN_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::orns(const PRegisterWithLaneSize& pd,
                     const PRegisterZ& pg,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // ORNS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 1100 .... 01.. ..0. ...1 ....
  //  op<23> = 1 | S<22> = 1 | Pm<19:16> | Pg<13:10> | o2<9> = 0 | Pn<8:5> |
  //  o3<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ORNS_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::orr(const PRegisterWithLaneSize& pd,
                    const PRegisterZ& pg,
                    const PRegisterWithLaneSize& pn,
                    const PRegisterWithLaneSize& pm) {
  // ORR <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 1000 .... 01.. ..0. ...0 ....
  //  op<23> = 1 | S<22> = 0 | Pm<19:16> | Pg<13:10> | o2<9> = 0 | Pn<8:5> |
  //  o3<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ORR_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::orrs(const PRegisterWithLaneSize& pd,
                     const PRegisterZ& pg,
                     const PRegisterWithLaneSize& pn,
                     const PRegisterWithLaneSize& pm) {
  // ORRS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 1100 .... 01.. ..0. ...0 ....
  //  op<23> = 1 | S<22> = 1 | Pm<19:16> | Pg<13:10> | o2<9> = 0 | Pn<8:5> |
  //  o3<4> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ORRS_p_p_pp_z | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::sel(const PRegisterWithLaneSize& pd,
                    const PRegister& pg,
                    const PRegisterWithLaneSize& pn,
                    const PRegisterWithLaneSize& pm) {
  // SEL <Pd>.B, <Pg>, <Pn>.B, <Pm>.B
  //  0010 0101 0000 .... 01.. ..1. ...1 ....
  //  op<23> = 0 | S<22> = 0 | Pm<19:16> | Pg<13:10> | o2<9> = 1 | Pn<8:5> |
  //  o3<4> = 1 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SEL_p_p_pp | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

// SVEPredicateMisc.

void Assembler::pfalse(const PRegisterWithLaneSize& pd) {
  // PFALSE <Pd>.B
  //  0010 0101 0001 1000 1110 0100 0000 ....
  //  op<23> = 0 | S<22> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PFALSE_p | Pd(pd));
}

void Assembler::pfirst(const PRegisterWithLaneSize& pd,
                       const PRegister& pg,
                       const PRegisterWithLaneSize& pn) {
  // PFIRST <Pdn>.B, <Pg>, <Pdn>.B
  //  0010 0101 0101 1000 1100 000. ...0 ....
  //  op<23> = 0 | S<22> = 1 | Pg<8:5> | Pdn<3:0>

  USE(pn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(pd.Is(pn));

  Emit(PFIRST_p_p_p | Pd(pd) | Rx<8, 5>(pg));
}

void Assembler::pnext(const PRegisterWithLaneSize& pd,
                      const PRegister& pg,
                      const PRegisterWithLaneSize& pn) {
  // PNEXT <Pdn>.<T>, <Pg>, <Pdn>.<T>
  //  0010 0101 ..01 1001 1100 010. ...0 ....
  //  size<23:22> | Pg<8:5> | Pdn<3:0>

  USE(pn);
  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(pd.Is(pn));

  Emit(PNEXT_p_p_p | Pd(pd) | Rx<8, 5>(pg));
}

void Assembler::ptest(const PRegister& pg, const PRegisterWithLaneSize& pn) {
  // PTEST <Pg>, <Pn>.B
  //  0010 0101 0101 0000 11.. ..0. ...0 0000
  //  op<23> = 0 | S<22> = 1 | Pg<13:10> | Pn<8:5> | opc2<3:0> = 0000

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PTEST_p_p | Rx<13, 10>(pg) | Rx<8, 5>(pn));
}

void Assembler::ptrue(const PRegisterWithLaneSize& pd, int pattern) {
  // PTRUE <Pd>.<T>{, <pattern>}
  //  0010 0101 ..01 1000 1110 00.. ...0 ....
  //  size<23:22> | S<16> = 0 | pattern<9:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PTRUE_p_s | Pd(pd) | ImmField<9, 5>(pattern));
}

void Assembler::ptrues(const PRegisterWithLaneSize& pd, int pattern) {
  // PTRUES <Pd>.<T>{, <pattern>}
  //  0010 0101 ..01 1001 1110 00.. ...0 ....
  //  size<23:22> | S<16> = 1 | pattern<9:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(PTRUES_p_s | Pd(pd) | ImmField<9, 5>(pattern));
}

void Assembler::rdffr(const PRegisterWithLaneSize& pd) {
  // RDFFR <Pd>.B
  //  0010 0101 0001 1001 1111 0000 0000 ....
  //  op<23> = 0 | S<22> = 0 | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(RDFFR_p_f | Pd(pd));
}

void Assembler::rdffr(const PRegisterWithLaneSize& pd, const PRegisterZ& pg) {
  // RDFFR <Pd>.B, <Pg>/Z
  //  0010 0101 0001 1000 1111 000. ...0 ....
  //  op<23> = 0 | S<22> = 0 | Pg<8:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(RDFFR_p_p_f | Pd(pd) | Rx<8, 5>(pg));
}

void Assembler::rdffrs(const PRegisterWithLaneSize& pd, const PRegisterZ& pg) {
  // RDFFRS <Pd>.B, <Pg>/Z
  //  0010 0101 0101 1000 1111 000. ...0 ....
  //  op<23> = 0 | S<22> = 1 | Pg<8:5> | Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(RDFFRS_p_p_f | Pd(pd) | Rx<8, 5>(pg));
}

// SVEPropagateBreak.

void Assembler::brkpa(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const PRegisterWithLaneSize& pn,
                      const PRegisterWithLaneSize& pm) {
  // BRKPA <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 0000 .... 11.. ..0. ...0 ....
  //  op<23> = 0 | S<22> = 0 | Pm<19:16> | Pg<13:10> | Pn<8:5> | B<4> = 0 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(BRKPA_p_p_pp | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::brkpas(const PRegisterWithLaneSize& pd,
                       const PRegisterZ& pg,
                       const PRegisterWithLaneSize& pn,
                       const PRegisterWithLaneSize& pm) {
  // BRKPAS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 0100 .... 11.. ..0. ...0 ....
  //  op<23> = 0 | S<22> = 1 | Pm<19:16> | Pg<13:10> | Pn<8:5> | B<4> = 0 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(BRKPAS_p_p_pp | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::brkpb(const PRegisterWithLaneSize& pd,
                      const PRegisterZ& pg,
                      const PRegisterWithLaneSize& pn,
                      const PRegisterWithLaneSize& pm) {
  // BRKPB <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 0000 .... 11.. ..0. ...1 ....
  //  op<23> = 0 | S<22> = 0 | Pm<19:16> | Pg<13:10> | Pn<8:5> | B<4> = 1 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(BRKPB_p_p_pp | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

void Assembler::brkpbs(const PRegisterWithLaneSize& pd,
                       const PRegisterZ& pg,
                       const PRegisterWithLaneSize& pn,
                       const PRegisterWithLaneSize& pm) {
  // BRKPBS <Pd>.B, <Pg>/Z, <Pn>.B, <Pm>.B
  //  0010 0101 0100 .... 11.. ..0. ...1 ....
  //  op<23> = 0 | S<22> = 1 | Pm<19:16> | Pg<13:10> | Pn<8:5> | B<4> = 1 |
  //  Pd<3:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(BRKPBS_p_p_pp | Pd(pd) | Rx<13, 10>(pg) | Rx<8, 5>(pn) | Rx<19, 16>(pm));
}

// SVEStackAllocation.

void Assembler::addpl(const Register& xd, const Register& xn, int imm6) {
  // ADDPL <Xd|SP>, <Xn|SP>, #<imm>
  //  0000 0100 011. .... 0101 0... .... ....
  //  op<22> = 1 | Rn<20:16> | imm6<10:5> | Rd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ADDPL_r_ri | RdSP(xd) | RnSP(xn) | ImmField<10, 5>(imm6));
}

void Assembler::addvl(const Register& xd, const Register& xn, int imm6) {
  // ADDVL <Xd|SP>, <Xn|SP>, #<imm>
  //  0000 0100 001. .... 0101 0... .... ....
  //  op<22> = 0 | Rn<20:16> | imm6<10:5> | Rd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(ADDVL_r_ri | RdSP(xd) | RnSP(xn) | ImmField<10, 5>(imm6));
}

void Assembler::rdvl(const Register& rd, int imm6) {
  // RDVL <Xd>, #<imm>
  //  0000 0100 1011 1111 0101 0... .... ....
  //  op<22> = 0 | opc2<20:16> = 11111 | imm6<10:5> | Rd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(RDVL_r_i | Rd(rd) | ImmField<10, 5>(imm6));
}

// SVEVectorSelect.

void Assembler::sel(const ZRegister& zd,
                    const PRegister& pg,
                    const ZRegister& zn,
                    const ZRegister& zm) {
  // SEL <Zd>.<T>, <Pg>, <Zn>.<T>, <Zm>.<T>
  //  0000 0101 ..1. .... 11.. .... .... ....
  //  size<23:22> | Zm<20:16> | Pg<13:10> | Zn<9:5> | Zd<4:0>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));
  VIXL_ASSERT(AreSameLaneSize(zd, zn, zm));

  Emit(SEL_z_p_zz | SVESize(zd) | Rd(zd) | Rx<13, 10>(pg) | Rn(zn) | Rm(zm));
}

// SVEWriteFFR.

void Assembler::setffr() {
  // SETFFR
  //  0010 0101 0010 1100 1001 0000 0000 0000
  //  opc<23:22> = 00

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(SETFFR_f);
}

void Assembler::wrffr(const PRegisterWithLaneSize& pn) {
  // WRFFR <Pn>.B
  //  0010 0101 0010 1000 1001 000. ...0 0000
  //  opc<23:22> = 00 | Pn<8:5>

  VIXL_ASSERT(CPUHas(CPUFeatures::kSVE));

  Emit(WRFFR_f_p | Rx<8, 5>(pn));
}

}  // namespace aarch64
}  // namespace vixl
