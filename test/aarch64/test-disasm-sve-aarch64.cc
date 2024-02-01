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


#include <cstdio>
#include <cstring>
#include <string>

#include "test-runner.h"

#include "aarch64/disasm-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"
#include "aarch64/test-utils-aarch64.h"
#include "test-disasm-aarch64.h"
#include "test-utils-aarch64.h"

namespace vixl {
namespace aarch64 {

TEST(sve) {
  SETUP();

  // TODO: Replace these tests when the disassembler is more capable.
  COMPARE(asrr(z0.VnB(), p7.Merging(), z0.VnB(), z1.VnB()),
          "asrr z0.b, p7/m, z0.b, z1.b");
  COMPARE(fcmeq(p6.VnD(), p7.Zeroing(), z0.VnD(), z1.VnD()),
          "fcmeq p6.d, p7/z, z0.d, z1.d");
  COMPARE(mla(z0.VnB(), p7.Merging(), z0.VnB(), z1.VnB()),
          "mla z0.b, p7/m, z0.b, z1.b");
  COMPARE(mla(z1.VnS(), p7.Merging(), z1.VnS(), z0.VnS()),
          "mla z1.s, p7/m, z1.s, z0.s");
  COMPARE(asr(z1.VnB(), z0.VnB(), 8), "asr z1.b, z0.b, #8");
  COMPARE(lsl(z0.VnH(), z1.VnH(), 15), "lsl z0.h, z1.h, #15");
  COMPARE(lsr(z1.VnS(), z0.VnS(), 32), "lsr z1.s, z0.s, #32");
  COMPARE(and_(p6.VnB(), p7.Zeroing(), p6.VnB(), p7.VnB()),
          "and p6.b, p7/z, p6.b, p7.b");
  COMPARE(rev(p7.VnB(), p6.VnB()), "rev p7.b, p6.b");
  COMPARE(splice(z0.VnH(), p7, z0.VnH(), z1.VnH()),
          "splice z0.h, p7, z0.h, z1.h");
  COMPARE(fnmad(z0.VnD(), p6.Merging(), z1.VnD(), z0.VnD()),
          "fnmad z0.d, p6/m, z1.d, z0.d");

  CLEANUP();
}

TEST(sve_unallocated_fp_byte_type) {
  // Ensure disassembly of FP instructions does not report byte-sized lanes.

  SETUP();

  COMPARE_PREFIX(dci(0x650003ca), "unallocated");
  COMPARE_PREFIX(dci(0x6500230b), "unallocated");
  COMPARE_PREFIX(dci(0x6500424c), "unallocated");
  COMPARE_PREFIX(dci(0x6500618d), "unallocated");
  COMPARE_PREFIX(dci(0x6500a00f), "unallocated");
  COMPARE_PREFIX(dci(0x6500de91), "unallocated");
  COMPARE_PREFIX(dci(0x6500fdd2), "unallocated");
  COMPARE_PREFIX(dci(0x65011d13), "unallocated");
  COMPARE_PREFIX(dci(0x65015b95), "unallocated");
  COMPARE_PREFIX(dci(0x65017ad6), "unallocated");
  COMPARE_PREFIX(dci(0x65019a17), "unallocated");
  COMPARE_PREFIX(dci(0x6501b958), "unallocated");
  COMPARE_PREFIX(dci(0x6502941f), "unallocated");
  COMPARE_PREFIX(dci(0x6502b360), "unallocated");
  COMPARE_PREFIX(dci(0x6502d2a1), "unallocated");
  COMPARE_PREFIX(dci(0x65038e27), "unallocated");
  COMPARE_PREFIX(dci(0x6503ad68), "unallocated");
  COMPARE_PREFIX(dci(0x65042a6c), "unallocated");
  COMPARE_PREFIX(dci(0x6504882f), "unallocated");
  COMPARE_PREFIX(dci(0x6504a770), "unallocated");
  COMPARE_PREFIX(dci(0x65052474), "unallocated");
  COMPARE_PREFIX(dci(0x65058237), "unallocated");
  COMPARE_PREFIX(dci(0x65063dbd), "unallocated");
  COMPARE_PREFIX(dci(0x65069b80), "unallocated");
  COMPARE_PREFIX(dci(0x6506bac1), "unallocated");
  COMPARE_PREFIX(dci(0x65071884), "unallocated");
  COMPARE_PREFIX(dci(0x650737c5), "unallocated");
  COMPARE_PREFIX(dci(0x65079588), "unallocated");
  COMPARE_PREFIX(dci(0x6507b4c9), "unallocated");
  COMPARE_PREFIX(dci(0x65088f90), "unallocated");
  COMPARE_PREFIX(dci(0x65090c94), "unallocated");
  COMPARE_PREFIX(dci(0x65098998), "unallocated");
  COMPARE_PREFIX(dci(0x650a83a0), "unallocated");
  COMPARE_PREFIX(dci(0x650c96f1), "unallocated");
  COMPARE_PREFIX(dci(0x650d90f9), "unallocated");
  COMPARE_PREFIX(dci(0x65113a97), "unallocated");
  COMPARE_PREFIX(dci(0x65183010), "unallocated");
  COMPARE_PREFIX(dci(0x65200050), "unallocated");
  COMPARE_PREFIX(dci(0x65203ed2), "unallocated");
  COMPARE_PREFIX(dci(0x65205e13), "unallocated");
  COMPARE_PREFIX(dci(0x65207d54), "unallocated");
  COMPARE_PREFIX(dci(0x65209c95), "unallocated");
  COMPARE_PREFIX(dci(0x6520bbd6), "unallocated");
  COMPARE_PREFIX(dci(0x6520db17), "unallocated");
  COMPARE_PREFIX(dci(0x6520fa58), "unallocated");
  COMPARE_PREFIX(dci(0x650f31e1), "unallocated");
  COMPARE_PREFIX(dci(0x650e30f7), "unallocated");
  COMPARE_PREFIX(dci(0x6511376e), "unallocated");

  CLEANUP();
}

TEST(sve_address_generation) {
  SETUP();

  COMPARE(adr(z19.VnD(), SVEMemOperand(z22.VnD(), z11.VnD(), SXTW)),
          "adr z19.d, [z22.d, z11.d, sxtw]");
  COMPARE(adr(z19.VnD(), SVEMemOperand(z22.VnD(), z11.VnD(), SXTW, 1)),
          "adr z19.d, [z22.d, z11.d, sxtw #1]");
  COMPARE(adr(z19.VnD(), SVEMemOperand(z22.VnD(), z11.VnD(), SXTW, 2)),
          "adr z19.d, [z22.d, z11.d, sxtw #2]");
  COMPARE(adr(z19.VnD(), SVEMemOperand(z22.VnD(), z11.VnD(), SXTW, 3)),
          "adr z19.d, [z22.d, z11.d, sxtw #3]");
  COMPARE(adr(z30.VnD(), SVEMemOperand(z14.VnD(), z16.VnD(), UXTW)),
          "adr z30.d, [z14.d, z16.d, uxtw]");
  COMPARE(adr(z30.VnD(), SVEMemOperand(z14.VnD(), z16.VnD(), UXTW, 1)),
          "adr z30.d, [z14.d, z16.d, uxtw #1]");
  COMPARE(adr(z30.VnD(), SVEMemOperand(z14.VnD(), z16.VnD(), UXTW, 2)),
          "adr z30.d, [z14.d, z16.d, uxtw #2]");
  COMPARE(adr(z30.VnD(), SVEMemOperand(z14.VnD(), z16.VnD(), UXTW, 3)),
          "adr z30.d, [z14.d, z16.d, uxtw #3]");
  COMPARE(adr(z8.VnS(), SVEMemOperand(z16.VnS(), z16.VnS())),
          "adr z8.s, [z16.s, z16.s]");
  COMPARE(adr(z8.VnS(), SVEMemOperand(z16.VnS(), z16.VnS(), LSL, 1)),
          "adr z8.s, [z16.s, z16.s, lsl #1]");
  COMPARE(adr(z8.VnS(), SVEMemOperand(z16.VnS(), z16.VnS(), LSL, 2)),
          "adr z8.s, [z16.s, z16.s, lsl #2]");
  COMPARE(adr(z8.VnS(), SVEMemOperand(z16.VnS(), z16.VnS(), LSL, 3)),
          "adr z8.s, [z16.s, z16.s, lsl #3]");
  COMPARE(adr(z9.VnD(), SVEMemOperand(z1.VnD(), z16.VnD())),
          "adr z9.d, [z1.d, z16.d]");
  COMPARE(adr(z9.VnD(), SVEMemOperand(z1.VnD(), z16.VnD(), LSL, 1)),
          "adr z9.d, [z1.d, z16.d, lsl #1]");
  COMPARE(adr(z9.VnD(), SVEMemOperand(z1.VnD(), z16.VnD(), LSL, 2)),
          "adr z9.d, [z1.d, z16.d, lsl #2]");
  COMPARE(adr(z9.VnD(), SVEMemOperand(z1.VnD(), z16.VnD(), LSL, 3)),
          "adr z9.d, [z1.d, z16.d, lsl #3]");


  CLEANUP();
}

TEST(sve_calculate_sve_address) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

  // Shadow the `MacroAssembler` type so that the test macros work without
  // modification.
  typedef CalculateSVEAddressMacroAssembler MacroAssembler;

  SETUP();

  // It is important that we cover every branch in this test because most other
  // tests tend not to check every code path.

  // IsEquivalentToScalar()
  COMPARE_MACRO(CalculateSVEAddress(x0, SVEMemOperand(x1)), "mov x0, x1");
  COMPARE_MACRO(CalculateSVEAddress(x4, SVEMemOperand(x2, 0)), "mov x4, x2");
  COMPARE_MACRO(CalculateSVEAddress(x4, SVEMemOperand(x2, xzr, LSL, 2)),
                "mov x4, x2");

  // IsScalarPlusImmediate()
  // Simple immediates just pass through to 'Add'.
  COMPARE_MACRO(CalculateSVEAddress(x10, SVEMemOperand(x0, 42)),
                "add x10, x0, #0x2a (42)");
  COMPARE_MACRO(CalculateSVEAddress(x10, SVEMemOperand(sp, 42)),
                "add x10, sp, #0x2a (42)");
  // SVE_MUL_VL variants use `Addpl`, which has its own tests, but
  // `CalculateSVEAddress` needs to check and handle the access size.
  COMPARE_MACRO(CalculateSVEAddress(x10, SVEMemOperand(x0, 3, SVE_MUL_VL), 0),
                "addpl x10, x0, #24");
  COMPARE_MACRO(CalculateSVEAddress(x10, SVEMemOperand(x0, 3, SVE_MUL_VL), 1),
                "addpl x10, x0, #12");
  COMPARE_MACRO(CalculateSVEAddress(x10, SVEMemOperand(x0, 3, SVE_MUL_VL), 2),
                "addpl x10, x0, #6");
  COMPARE_MACRO(CalculateSVEAddress(x10, SVEMemOperand(x0, 3, SVE_MUL_VL), 3),
                "addpl x10, x0, #3");

  // IsScalarPlusScalar()
  // All forms pass through to `Add`, but SVE_LSL must be handled correctly.
  COMPARE_MACRO(CalculateSVEAddress(x22, SVEMemOperand(x2, x3)),
                "add x22, x2, x3");
  COMPARE_MACRO(CalculateSVEAddress(x22, SVEMemOperand(sp, x3)),
                "add x22, sp, x3");
  COMPARE_MACRO(CalculateSVEAddress(x22, SVEMemOperand(x2, x3, LSL, 2)),
                "add x22, x2, x3, lsl #2");
  COMPARE_MACRO(CalculateSVEAddress(x22, SVEMemOperand(sp, x3, LSL, 2)),
                "add x22, sp, x3, lsl #2");

  CLEANUP();

#pragma GCC diagnostic pop
}

TEST(sve_bitwise_imm) {
  SETUP();

  // The assembler will necessarily encode an immediate in the simplest bitset.
  COMPARE(and_(z2.VnD(), z2.VnD(), 0x0000ffff0000ffff),
          "and z2.s, z2.s, #0xffff");
  COMPARE(dupm(z15.VnS(), 0x7f007f00), "dupm z15.h, #0x7f00");
  COMPARE(eor(z26.VnH(), z26.VnH(), 0x7ff8), "eor z26.h, z26.h, #0x7ff8");
  COMPARE(orr(z13.VnB(), z13.VnB(), 0x78), "orr z13.b, z13.b, #0x78");

  // Logical aliases.
  COMPARE(bic(z21.VnD(), z21.VnD(), 0xffff00000000ffff),
          "and z21.d, z21.d, #0xffffffff0000");
  COMPARE(eon(z31.VnS(), z31.VnS(), 0x1ffe), "eor z31.s, z31.s, #0xffffe001");
  COMPARE(orn(z11.VnH(), z11.VnH(), 0x2), "orr z11.h, z11.h, #0xfffd");

  // Mov alias for dupm.
  COMPARE(mov(z0.VnH(), 0xf00f), "mov z0.h, #0xf00f");
  COMPARE_MACRO(Mov(z11.VnS(), 0xe0000003), "mov z11.s, #0xe0000003");
  COMPARE_MACRO(Mov(z22.VnD(), 0x8000), "dupm z22.d, #0x8000");

  // Test dupm versus mov disassembly.
  COMPARE(dupm(z0.VnH(), 0xfe), "dupm z0.h, #0xfe");
  COMPARE(dupm(z0.VnH(), 0xff), "dupm z0.h, #0xff");
  COMPARE(dupm(z0.VnH(), 0x1fe), "mov z0.h, #0x1fe");
  COMPARE(dupm(z0.VnH(), 0xfe00), "dupm z0.h, #0xfe00");
  COMPARE(dupm(z0.VnH(), 0xfe01), "mov z0.h, #0xfe01");
  COMPARE(dupm(z0.VnS(), 0xfe00), "dupm z0.s, #0xfe00");
  COMPARE(dupm(z0.VnS(), 0xfe000001), "mov z0.s, #0xfe000001");
  COMPARE(dupm(z0.VnS(), 0xffffff00), "dupm z0.s, #0xffffff00");
  COMPARE(dupm(z0.VnS(), 0xffffff01), "dupm z0.s, #0xffffff01");
  COMPARE(dupm(z0.VnS(), 0xfffffe01), "mov z0.s, #0xfffffe01");
  COMPARE(dupm(z0.VnS(), 0xfff), "mov z0.s, #0xfff");
  COMPARE(dupm(z0.VnD(), 0xffffffffffffff00), "dupm z0.d, #0xffffffffffffff00");
  COMPARE(dupm(z0.VnD(), 0x7fffffffffffff80), "mov z0.d, #0x7fffffffffffff80");
  COMPARE(dupm(z0.VnD(), 0x8000), "dupm z0.d, #0x8000");
  COMPARE(dupm(z0.VnD(), 0x10000), "mov z0.d, #0x10000");

  CLEANUP();
}

TEST(sve_bitwise_logical_unpredicated) {
  SETUP();

  COMPARE(and_(z12.VnD(), z5.VnD(), z29.VnD()), "and z12.d, z5.d, z29.d");
  COMPARE(bic(z11.VnD(), z15.VnD(), z9.VnD()), "bic z11.d, z15.d, z9.d");
  COMPARE(eor(z9.VnD(), z31.VnD(), z29.VnD()), "eor z9.d, z31.d, z29.d");
  COMPARE(orr(z17.VnD(), z8.VnD(), z19.VnD()), "orr z17.d, z8.d, z19.d");

  // Check mov aliases.
  COMPARE(orr(z17.VnD(), z8.VnD(), z8.VnD()), "mov z17.d, z8.d");
  COMPARE(mov(z18, z9), "mov z18.d, z9.d");
  COMPARE_MACRO(Mov(z19, z10), "mov z19.d, z10.d");

  CLEANUP();
}

TEST(sve_bitwise_shift_predicated) {
  SETUP();

  COMPARE(asrr(z20.VnB(), p3.Merging(), z20.VnB(), z11.VnB()),
          "asrr z20.b, p3/m, z20.b, z11.b");
  COMPARE(asrr(z20.VnH(), p3.Merging(), z20.VnH(), z11.VnH()),
          "asrr z20.h, p3/m, z20.h, z11.h");
  COMPARE(asrr(z20.VnS(), p3.Merging(), z20.VnS(), z11.VnS()),
          "asrr z20.s, p3/m, z20.s, z11.s");
  COMPARE(asrr(z20.VnD(), p3.Merging(), z20.VnD(), z11.VnD()),
          "asrr z20.d, p3/m, z20.d, z11.d");
  COMPARE(asr(z26.VnB(), p2.Merging(), z26.VnB(), z17.VnB()),
          "asr z26.b, p2/m, z26.b, z17.b");
  COMPARE(asr(z26.VnH(), p2.Merging(), z26.VnH(), z17.VnH()),
          "asr z26.h, p2/m, z26.h, z17.h");
  COMPARE(asr(z26.VnS(), p2.Merging(), z26.VnS(), z17.VnS()),
          "asr z26.s, p2/m, z26.s, z17.s");
  COMPARE(asr(z26.VnD(), p2.Merging(), z26.VnD(), z17.VnD()),
          "asr z26.d, p2/m, z26.d, z17.d");
  COMPARE(lslr(z30.VnB(), p1.Merging(), z30.VnB(), z26.VnB()),
          "lslr z30.b, p1/m, z30.b, z26.b");
  COMPARE(lslr(z30.VnH(), p1.Merging(), z30.VnH(), z26.VnH()),
          "lslr z30.h, p1/m, z30.h, z26.h");
  COMPARE(lslr(z30.VnS(), p1.Merging(), z30.VnS(), z26.VnS()),
          "lslr z30.s, p1/m, z30.s, z26.s");
  COMPARE(lslr(z30.VnD(), p1.Merging(), z30.VnD(), z26.VnD()),
          "lslr z30.d, p1/m, z30.d, z26.d");
  COMPARE(lsl(z14.VnB(), p6.Merging(), z14.VnB(), z25.VnB()),
          "lsl z14.b, p6/m, z14.b, z25.b");
  COMPARE(lsl(z14.VnH(), p6.Merging(), z14.VnH(), z25.VnH()),
          "lsl z14.h, p6/m, z14.h, z25.h");
  COMPARE(lsl(z14.VnS(), p6.Merging(), z14.VnS(), z25.VnS()),
          "lsl z14.s, p6/m, z14.s, z25.s");
  COMPARE(lsl(z14.VnD(), p6.Merging(), z14.VnD(), z25.VnD()),
          "lsl z14.d, p6/m, z14.d, z25.d");
  COMPARE(lsrr(z3.VnB(), p1.Merging(), z3.VnB(), z16.VnB()),
          "lsrr z3.b, p1/m, z3.b, z16.b");
  COMPARE(lsrr(z3.VnH(), p1.Merging(), z3.VnH(), z16.VnH()),
          "lsrr z3.h, p1/m, z3.h, z16.h");
  COMPARE(lsrr(z3.VnS(), p1.Merging(), z3.VnS(), z16.VnS()),
          "lsrr z3.s, p1/m, z3.s, z16.s");
  COMPARE(lsrr(z3.VnD(), p1.Merging(), z3.VnD(), z16.VnD()),
          "lsrr z3.d, p1/m, z3.d, z16.d");
  COMPARE(lsr(z29.VnB(), p7.Merging(), z29.VnB(), z13.VnB()),
          "lsr z29.b, p7/m, z29.b, z13.b");
  COMPARE(lsr(z29.VnH(), p7.Merging(), z29.VnH(), z13.VnH()),
          "lsr z29.h, p7/m, z29.h, z13.h");
  COMPARE(lsr(z29.VnS(), p7.Merging(), z29.VnS(), z13.VnS()),
          "lsr z29.s, p7/m, z29.s, z13.s");
  COMPARE(lsr(z29.VnD(), p7.Merging(), z29.VnD(), z13.VnD()),
          "lsr z29.d, p7/m, z29.d, z13.d");

  COMPARE(asr(z4.VnB(), p0.Merging(), z4.VnB(), z30.VnD()),
          "asr z4.b, p0/m, z4.b, z30.d");
  COMPARE(asr(z4.VnH(), p0.Merging(), z4.VnH(), z30.VnD()),
          "asr z4.h, p0/m, z4.h, z30.d");
  COMPARE(asr(z4.VnS(), p0.Merging(), z4.VnS(), z30.VnD()),
          "asr z4.s, p0/m, z4.s, z30.d");
  COMPARE(lsl(z13.VnB(), p7.Merging(), z13.VnB(), z18.VnD()),
          "lsl z13.b, p7/m, z13.b, z18.d");
  COMPARE(lsl(z13.VnH(), p7.Merging(), z13.VnH(), z18.VnD()),
          "lsl z13.h, p7/m, z13.h, z18.d");
  COMPARE(lsl(z13.VnS(), p7.Merging(), z13.VnS(), z18.VnD()),
          "lsl z13.s, p7/m, z13.s, z18.d");
  COMPARE(lsr(z1.VnB(), p4.Merging(), z1.VnB(), z14.VnD()),
          "lsr z1.b, p4/m, z1.b, z14.d");
  COMPARE(lsr(z1.VnH(), p4.Merging(), z1.VnH(), z14.VnD()),
          "lsr z1.h, p4/m, z1.h, z14.d");
  COMPARE(lsr(z1.VnS(), p4.Merging(), z1.VnS(), z14.VnD()),
          "lsr z1.s, p4/m, z1.s, z14.d");

  COMPARE_MACRO(Asr(z4.VnB(), p0.Merging(), z4.VnB(), z30.VnB()),
                "asr z4.b, p0/m, z4.b, z30.b");
  COMPARE_MACRO(Asr(z4.VnB(), p0.Merging(), z30.VnB(), z4.VnB()),
                "asrr z4.b, p0/m, z4.b, z30.b");
  COMPARE_MACRO(Asr(z4.VnB(), p0.Merging(), z10.VnB(), z14.VnB()),
                "movprfx z4.b, p0/m, z10.b\n"
                "asr z4.b, p0/m, z4.b, z14.b");
  COMPARE_MACRO(Lsl(z4.VnB(), p0.Merging(), z4.VnB(), z30.VnB()),
                "lsl z4.b, p0/m, z4.b, z30.b");
  COMPARE_MACRO(Lsl(z4.VnB(), p0.Merging(), z30.VnB(), z4.VnB()),
                "lslr z4.b, p0/m, z4.b, z30.b");
  COMPARE_MACRO(Lsl(z4.VnB(), p0.Merging(), z10.VnB(), z14.VnB()),
                "movprfx z4.b, p0/m, z10.b\n"
                "lsl z4.b, p0/m, z4.b, z14.b");
  COMPARE_MACRO(Lsr(z4.VnB(), p0.Merging(), z4.VnB(), z30.VnB()),
                "lsr z4.b, p0/m, z4.b, z30.b");
  COMPARE_MACRO(Lsr(z4.VnB(), p0.Merging(), z30.VnB(), z4.VnB()),
                "lsrr z4.b, p0/m, z4.b, z30.b");
  COMPARE_MACRO(Lsr(z4.VnB(), p0.Merging(), z10.VnB(), z14.VnB()),
                "movprfx z4.b, p0/m, z10.b\n"
                "lsr z4.b, p0/m, z4.b, z14.b");

  COMPARE(asrd(z0.VnB(), p4.Merging(), z0.VnB(), 1),
          "asrd z0.b, p4/m, z0.b, #1");
  COMPARE(asrd(z0.VnH(), p4.Merging(), z0.VnH(), 1),
          "asrd z0.h, p4/m, z0.h, #1");
  COMPARE(asrd(z0.VnS(), p4.Merging(), z0.VnS(), 1),
          "asrd z0.s, p4/m, z0.s, #1");
  COMPARE(asrd(z0.VnD(), p4.Merging(), z0.VnD(), 1),
          "asrd z0.d, p4/m, z0.d, #1");
  COMPARE(asr(z8.VnB(), p7.Merging(), z8.VnB(), 3), "asr z8.b, p7/m, z8.b, #3");
  COMPARE(asr(z8.VnH(), p7.Merging(), z8.VnH(), 3), "asr z8.h, p7/m, z8.h, #3");
  COMPARE(asr(z8.VnS(), p7.Merging(), z8.VnS(), 3), "asr z8.s, p7/m, z8.s, #3");
  COMPARE(asr(z8.VnD(), p7.Merging(), z8.VnD(), 3), "asr z8.d, p7/m, z8.d, #3");
  COMPARE(lsl(z29.VnB(), p6.Merging(), z29.VnB(), 0),
          "lsl z29.b, p6/m, z29.b, #0");
  COMPARE(lsl(z29.VnH(), p6.Merging(), z29.VnH(), 5),
          "lsl z29.h, p6/m, z29.h, #5");
  COMPARE(lsl(z29.VnS(), p6.Merging(), z29.VnS(), 0),
          "lsl z29.s, p6/m, z29.s, #0");
  COMPARE(lsl(z29.VnD(), p6.Merging(), z29.VnD(), 63),
          "lsl z29.d, p6/m, z29.d, #63");
  COMPARE(lsr(z24.VnB(), p2.Merging(), z24.VnB(), 8),
          "lsr z24.b, p2/m, z24.b, #8");
  COMPARE(lsr(z24.VnH(), p2.Merging(), z24.VnH(), 16),
          "lsr z24.h, p2/m, z24.h, #16");
  COMPARE(lsr(z24.VnS(), p2.Merging(), z24.VnS(), 32),
          "lsr z24.s, p2/m, z24.s, #32");
  COMPARE(lsr(z24.VnD(), p2.Merging(), z24.VnD(), 64),
          "lsr z24.d, p2/m, z24.d, #64");

  COMPARE_MACRO(Asrd(z0.VnB(), p4.Merging(), z8.VnB(), 1),
                "movprfx z0.b, p4/m, z8.b\n"
                "asrd z0.b, p4/m, z0.b, #1");
  COMPARE_MACRO(Asr(z8.VnH(), p7.Merging(), z29.VnH(), 3),
                "movprfx z8.h, p7/m, z29.h\n"
                "asr z8.h, p7/m, z8.h, #3");
  COMPARE_MACRO(Lsl(z29.VnS(), p6.Merging(), z24.VnS(), 0),
                "movprfx z29.s, p6/m, z24.s\n"
                "lsl z29.s, p6/m, z29.s, #0");
  COMPARE_MACRO(Lsr(z24.VnD(), p2.Merging(), z0.VnD(), 64),
                "movprfx z24.d, p2/m, z0.d\n"
                "lsr z24.d, p2/m, z24.d, #64");

  CLEANUP();
}

TEST(sve_bitwise_shift_unpredicated) {
  SETUP();

  // Test lsl with reserved D-sized lane field.
  COMPARE_PREFIX(dci(0x04ef8e15), "unimplemented");
  // Test asr with reserved tsz field.
  COMPARE_PREFIX(dci(0x04209345), "unimplemented");

  COMPARE(asr(z4.VnB(), z27.VnB(), 1), "asr z4.b, z27.b, #1");
  COMPARE(asr(z5.VnB(), z26.VnB(), 8), "asr z5.b, z26.b, #8");
  COMPARE(asr(z6.VnH(), z25.VnH(), 1), "asr z6.h, z25.h, #1");
  COMPARE(asr(z7.VnH(), z24.VnH(), 16), "asr z7.h, z24.h, #16");
  COMPARE(asr(z8.VnS(), z23.VnS(), 1), "asr z8.s, z23.s, #1");
  COMPARE(asr(z9.VnS(), z22.VnS(), 32), "asr z9.s, z22.s, #32");
  COMPARE(asr(z10.VnD(), z21.VnD(), 1), "asr z10.d, z21.d, #1");
  COMPARE(asr(z11.VnD(), z20.VnD(), 64), "asr z11.d, z20.d, #64");
  COMPARE(lsr(z4.VnB(), z27.VnB(), 3), "lsr z4.b, z27.b, #3");
  COMPARE(lsr(z5.VnB(), z26.VnB(), 7), "lsr z5.b, z26.b, #7");
  COMPARE(lsr(z6.VnH(), z25.VnH(), 8), "lsr z6.h, z25.h, #8");
  COMPARE(lsr(z7.VnH(), z24.VnH(), 15), "lsr z7.h, z24.h, #15");
  COMPARE(lsr(z8.VnS(), z23.VnS(), 14), "lsr z8.s, z23.s, #14");
  COMPARE(lsr(z9.VnS(), z22.VnS(), 31), "lsr z9.s, z22.s, #31");
  COMPARE(lsr(z10.VnD(), z21.VnD(), 30), "lsr z10.d, z21.d, #30");
  COMPARE(lsr(z11.VnD(), z20.VnD(), 63), "lsr z11.d, z20.d, #63");
  COMPARE(lsl(z4.VnB(), z27.VnB(), 4), "lsl z4.b, z27.b, #4");
  COMPARE(lsl(z5.VnB(), z26.VnB(), 6), "lsl z5.b, z26.b, #6");
  COMPARE(lsl(z6.VnH(), z25.VnH(), 10), "lsl z6.h, z25.h, #10");
  COMPARE(lsl(z7.VnH(), z24.VnH(), 14), "lsl z7.h, z24.h, #14");
  COMPARE(lsl(z8.VnS(), z23.VnS(), 21), "lsl z8.s, z23.s, #21");
  COMPARE(lsl(z9.VnS(), z22.VnS(), 30), "lsl z9.s, z22.s, #30");
  COMPARE(lsl(z10.VnD(), z21.VnD(), 44), "lsl z10.d, z21.d, #44");
  COMPARE(lsl(z11.VnD(), z20.VnD(), 62), "lsl z11.d, z20.d, #62");
  COMPARE(asr(z12.VnB(), z8.VnB(), z14.VnD()), "asr z12.b, z8.b, z14.d");
  COMPARE(asr(z14.VnH(), z8.VnH(), z12.VnD()), "asr z14.h, z8.h, z12.d");
  COMPARE(asr(z16.VnS(), z8.VnS(), z10.VnD()), "asr z16.s, z8.s, z10.d");
  COMPARE(lsl(z21.VnB(), z16.VnB(), z15.VnD()), "lsl z21.b, z16.b, z15.d");
  COMPARE(lsl(z23.VnH(), z16.VnH(), z13.VnD()), "lsl z23.h, z16.h, z13.d");
  COMPARE(lsl(z25.VnS(), z16.VnS(), z11.VnD()), "lsl z25.s, z16.s, z11.d");
  COMPARE(lsr(z16.VnB(), z19.VnB(), z2.VnD()), "lsr z16.b, z19.b, z2.d");
  COMPARE(lsr(z18.VnH(), z19.VnH(), z4.VnD()), "lsr z18.h, z19.h, z4.d");
  COMPARE(lsr(z20.VnS(), z19.VnS(), z6.VnD()), "lsr z20.s, z19.s, z6.d");

  CLEANUP();
}


TEST(sve_element_count) {
  SETUP();

  COMPARE_MACRO(Cntb(x7), "cntb x7");
  COMPARE_MACRO(Cntb(x7, SVE_POW2), "cntb x7, pow2");
  COMPARE_MACRO(Cntb(x7, SVE_VL1), "cntb x7, vl1");
  COMPARE_MACRO(Cntb(x7, SVE_VL2), "cntb x7, vl2");
  COMPARE_MACRO(Cntb(x7, SVE_VL16), "cntb x7, vl16");
  COMPARE_MACRO(Cntb(x7, SVE_VL256), "cntb x7, vl256");
  COMPARE_MACRO(Cntb(x7, SVE_MUL4), "cntb x7, mul4");
  COMPARE_MACRO(Cntb(x7, SVE_MUL3), "cntb x7, mul3");
  COMPARE_MACRO(Cntb(x7, SVE_ALL), "cntb x7");

  COMPARE_MACRO(Cntb(x7, SVE_POW2, 1), "cntb x7, pow2");
  COMPARE_MACRO(Cntb(x7, SVE_VL1, 16), "cntb x7, vl1, mul #16");
  COMPARE_MACRO(Cntb(x7, SVE_VL2, 15), "cntb x7, vl2, mul #15");
  COMPARE_MACRO(Cntb(x7, SVE_VL16, 14), "cntb x7, vl16, mul #14");
  COMPARE_MACRO(Cntb(x7, SVE_VL256, 8), "cntb x7, vl256, mul #8");
  COMPARE_MACRO(Cntb(x7, SVE_MUL4, 4), "cntb x7, mul4, mul #4");
  COMPARE_MACRO(Cntb(x7, SVE_MUL3, 3), "cntb x7, mul3, mul #3");
  COMPARE_MACRO(Cntb(x7, SVE_ALL, 2), "cntb x7, all, mul #2");

  COMPARE_MACRO(Cntb(x30), "cntb x30");
  COMPARE_MACRO(Cntd(xzr, SVE_POW2), "cntd xzr, pow2");
  COMPARE_MACRO(Cntd(xzr, SVE_MUL4, 1), "cntd xzr, mul4");
  COMPARE_MACRO(Cnth(x29, SVE_MUL3, 4), "cnth x29, mul3, mul #4");
  COMPARE_MACRO(Cntw(x28, SVE_VL256, 16), "cntw x28, vl256, mul #16");

  CLEANUP();
}

TEST(sve_incdec_reg_element_count) {
  SETUP();

  COMPARE_MACRO(Decb(x4), "decb x4");
  COMPARE_MACRO(Decb(x4, SVE_POW2), "decb x4, pow2");
  COMPARE_MACRO(Decb(x4, SVE_VL1), "decb x4, vl1");
  COMPARE_MACRO(Decb(x4, SVE_VL2), "decb x4, vl2");
  COMPARE_MACRO(Decb(x4, SVE_VL16), "decb x4, vl16");
  COMPARE_MACRO(Decb(x4, SVE_VL256), "decb x4, vl256");
  COMPARE_MACRO(Decb(x4, SVE_MUL4), "decb x4, mul4");
  COMPARE_MACRO(Decb(x4, SVE_MUL3), "decb x4, mul3");
  COMPARE_MACRO(Decb(x4, SVE_ALL), "decb x4");

  COMPARE_MACRO(Decb(x4, SVE_POW2, 1), "decb x4, pow2");
  COMPARE_MACRO(Decb(x4, SVE_VL1, 16), "decb x4, vl1, mul #16");
  COMPARE_MACRO(Decb(x4, SVE_VL2, 15), "decb x4, vl2, mul #15");
  COMPARE_MACRO(Decb(x4, SVE_VL16, 14), "decb x4, vl16, mul #14");
  COMPARE_MACRO(Decb(x4, SVE_VL256, 8), "decb x4, vl256, mul #8");
  COMPARE_MACRO(Decb(x4, SVE_MUL4, 4), "decb x4, mul4, mul #4");
  COMPARE_MACRO(Decb(x4, SVE_MUL3, 3), "decb x4, mul3, mul #3");
  COMPARE_MACRO(Decb(x4, SVE_ALL, 2), "decb x4, all, mul #2");

  COMPARE_MACRO(Decb(x30), "decb x30");
  COMPARE_MACRO(Decd(xzr, SVE_POW2), "decd xzr, pow2");
  COMPARE_MACRO(Decd(xzr, SVE_MUL4, 1), "decd xzr, mul4");
  COMPARE_MACRO(Dech(x29, SVE_MUL3, 4), "dech x29, mul3, mul #4");
  COMPARE_MACRO(Decw(x28, SVE_VL256, 16), "decw x28, vl256, mul #16");

  COMPARE_MACRO(Incb(x17), "incb x17");
  COMPARE_MACRO(Incb(x17, SVE_POW2), "incb x17, pow2");
  COMPARE_MACRO(Incb(x17, SVE_VL1), "incb x17, vl1");
  COMPARE_MACRO(Incb(x17, SVE_VL2), "incb x17, vl2");
  COMPARE_MACRO(Incb(x17, SVE_VL16), "incb x17, vl16");
  COMPARE_MACRO(Incb(x17, SVE_VL256), "incb x17, vl256");
  COMPARE_MACRO(Incb(x17, SVE_MUL4), "incb x17, mul4");
  COMPARE_MACRO(Incb(x17, SVE_MUL3), "incb x17, mul3");
  COMPARE_MACRO(Incb(x17, SVE_ALL), "incb x17");

  COMPARE_MACRO(Incb(x17, SVE_POW2, 1), "incb x17, pow2");
  COMPARE_MACRO(Incb(x17, SVE_VL1, 16), "incb x17, vl1, mul #16");
  COMPARE_MACRO(Incb(x17, SVE_VL2, 15), "incb x17, vl2, mul #15");
  COMPARE_MACRO(Incb(x17, SVE_VL16, 14), "incb x17, vl16, mul #14");
  COMPARE_MACRO(Incb(x17, SVE_VL256, 8), "incb x17, vl256, mul #8");
  COMPARE_MACRO(Incb(x17, SVE_MUL4, 4), "incb x17, mul4, mul #4");
  COMPARE_MACRO(Incb(x17, SVE_MUL3, 3), "incb x17, mul3, mul #3");
  COMPARE_MACRO(Incb(x17, SVE_ALL, 2), "incb x17, all, mul #2");

  COMPARE_MACRO(Incb(x30), "incb x30");
  COMPARE_MACRO(Incd(xzr, SVE_POW2), "incd xzr, pow2");
  COMPARE_MACRO(Incd(xzr, SVE_MUL4, 1), "incd xzr, mul4");
  COMPARE_MACRO(Inch(x29, SVE_MUL3, 4), "inch x29, mul3, mul #4");
  COMPARE_MACRO(Incw(x28, SVE_VL256, 16), "incw x28, vl256, mul #16");

  CLEANUP();
}

TEST(sve_signed_sat_incdec_reg_element_count) {
  SETUP();

  COMPARE_MACRO(Sqdecb(x12, w12), "sqdecb x12, w12");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_POW2), "sqdecb x12, w12, pow2");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_VL1), "sqdecb x12, w12, vl1");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_VL2), "sqdecb x12, w12, vl2");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_VL16), "sqdecb x12, w12, vl16");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_VL256), "sqdecb x12, w12, vl256");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_MUL4), "sqdecb x12, w12, mul4");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_MUL3), "sqdecb x12, w12, mul3");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_ALL), "sqdecb x12, w12");

  COMPARE_MACRO(Sqdecb(x12, w12, SVE_POW2, 1), "sqdecb x12, w12, pow2");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_VL1, 16), "sqdecb x12, w12, vl1, mul #16");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_VL2, 15), "sqdecb x12, w12, vl2, mul #15");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_VL16, 14),
                "sqdecb x12, w12, vl16, mul #14");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_VL256, 8),
                "sqdecb x12, w12, vl256, mul #8");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_MUL4, 4), "sqdecb x12, w12, mul4, mul #4");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_MUL3, 3), "sqdecb x12, w12, mul3, mul #3");
  COMPARE_MACRO(Sqdecb(x12, w12, SVE_ALL, 2), "sqdecb x12, w12, all, mul #2");

  COMPARE_MACRO(Sqdecb(xzr, wzr, SVE_POW2), "sqdecb xzr, wzr, pow2");
  COMPARE_MACRO(Sqdecb(xzr, wzr, SVE_MUL4, 1), "sqdecb xzr, wzr, mul4");

  COMPARE_MACRO(Sqincw(x20, w20, SVE_POW2, 1), "sqincw x20, w20, pow2");
  COMPARE_MACRO(Sqincd(x20, w20, SVE_VL1, 16), "sqincd x20, w20, vl1, mul #16");
  COMPARE_MACRO(Sqinch(x20, w20, SVE_VL2, 15), "sqinch x20, w20, vl2, mul #15");
  COMPARE_MACRO(Sqincw(x20, w20, SVE_VL16, 14),
                "sqincw x20, w20, vl16, mul #14");
  COMPARE_MACRO(Sqincd(x20, w20, SVE_VL256, 8),
                "sqincd x20, w20, vl256, mul #8");
  COMPARE_MACRO(Sqinch(x20, w20, SVE_MUL4, 4), "sqinch x20, w20, mul4, mul #4");
  COMPARE_MACRO(Sqincw(x20, w20, SVE_MUL3, 3), "sqincw x20, w20, mul3, mul #3");
  COMPARE_MACRO(Sqincd(x20, w20, SVE_ALL, 2), "sqincd x20, w20, all, mul #2");

  COMPARE_MACRO(Sqdecb(x5), "sqdecb x5");
  COMPARE_MACRO(Sqdecb(x5, SVE_POW2), "sqdecb x5, pow2");
  COMPARE_MACRO(Sqdecb(x5, SVE_VL1), "sqdecb x5, vl1");
  COMPARE_MACRO(Sqdecb(x5, SVE_VL2), "sqdecb x5, vl2");
  COMPARE_MACRO(Sqdecb(x5, SVE_VL16), "sqdecb x5, vl16");
  COMPARE_MACRO(Sqdecb(x5, SVE_VL256), "sqdecb x5, vl256");
  COMPARE_MACRO(Sqdecb(x5, SVE_MUL4), "sqdecb x5, mul4");
  COMPARE_MACRO(Sqdecb(x5, SVE_MUL3), "sqdecb x5, mul3");
  COMPARE_MACRO(Sqdecb(x5, SVE_ALL), "sqdecb x5");

  COMPARE_MACRO(Sqdecb(x5, SVE_POW2, 1), "sqdecb x5, pow2");
  COMPARE_MACRO(Sqdecb(x5, SVE_VL1, 16), "sqdecb x5, vl1, mul #16");
  COMPARE_MACRO(Sqdecb(x5, SVE_VL2, 15), "sqdecb x5, vl2, mul #15");
  COMPARE_MACRO(Sqdecb(x5, SVE_VL16, 14), "sqdecb x5, vl16, mul #14");
  COMPARE_MACRO(Sqdecb(x5, SVE_VL256, 8), "sqdecb x5, vl256, mul #8");
  COMPARE_MACRO(Sqdecb(x5, SVE_MUL4, 4), "sqdecb x5, mul4, mul #4");
  COMPARE_MACRO(Sqdecb(x5, SVE_MUL3, 3), "sqdecb x5, mul3, mul #3");
  COMPARE_MACRO(Sqdecb(x5, SVE_ALL, 2), "sqdecb x5, all, mul #2");

  COMPARE_MACRO(Sqdecb(xzr, SVE_POW2), "sqdecb xzr, pow2");
  COMPARE_MACRO(Sqdecb(xzr, SVE_MUL4, 1), "sqdecb xzr, mul4");

  COMPARE_MACRO(Sqincw(x7, SVE_POW2, 1), "sqincw x7, pow2");
  COMPARE_MACRO(Sqincd(x7, SVE_VL1, 16), "sqincd x7, vl1, mul #16");
  COMPARE_MACRO(Sqinch(x7, SVE_VL2, 15), "sqinch x7, vl2, mul #15");
  COMPARE_MACRO(Sqincw(x7, SVE_VL16, 14), "sqincw x7, vl16, mul #14");
  COMPARE_MACRO(Sqincd(x7, SVE_VL256, 8), "sqincd x7, vl256, mul #8");
  COMPARE_MACRO(Sqinch(x7, SVE_MUL4, 4), "sqinch x7, mul4, mul #4");
  COMPARE_MACRO(Sqincw(x7, SVE_MUL3, 3), "sqincw x7, mul3, mul #3");
  COMPARE_MACRO(Sqincd(x7, SVE_ALL, 2), "sqincd x7, all, mul #2");

  CLEANUP();
}

TEST(sve_unsigned_sat_incdec_reg_element_count) {
  SETUP();

  COMPARE_MACRO(Uqdecb(w12), "uqdecb w12");
  COMPARE_MACRO(Uqdecb(w12, SVE_POW2), "uqdecb w12, pow2");
  COMPARE_MACRO(Uqdecb(w12, SVE_VL1), "uqdecb w12, vl1");
  COMPARE_MACRO(Uqdecb(w12, SVE_VL2), "uqdecb w12, vl2");
  COMPARE_MACRO(Uqdecb(w12, SVE_VL16), "uqdecb w12, vl16");
  COMPARE_MACRO(Uqdecb(w12, SVE_VL256), "uqdecb w12, vl256");
  COMPARE_MACRO(Uqdecb(w12, SVE_MUL4), "uqdecb w12, mul4");
  COMPARE_MACRO(Uqdecb(w12, SVE_MUL3), "uqdecb w12, mul3");
  COMPARE_MACRO(Uqdecb(w12, SVE_ALL), "uqdecb w12");

  COMPARE_MACRO(Uqdecb(w12, SVE_POW2, 1), "uqdecb w12, pow2");
  COMPARE_MACRO(Uqdecb(w12, SVE_VL1, 16), "uqdecb w12, vl1, mul #16");
  COMPARE_MACRO(Uqdecb(w12, SVE_VL2, 15), "uqdecb w12, vl2, mul #15");
  COMPARE_MACRO(Uqdecb(w12, SVE_VL16, 14), "uqdecb w12, vl16, mul #14");
  COMPARE_MACRO(Uqdecb(w12, SVE_VL256, 8), "uqdecb w12, vl256, mul #8");
  COMPARE_MACRO(Uqdecb(w12, SVE_MUL4, 4), "uqdecb w12, mul4, mul #4");
  COMPARE_MACRO(Uqdecb(w12, SVE_MUL3, 3), "uqdecb w12, mul3, mul #3");
  COMPARE_MACRO(Uqdecb(w12, SVE_ALL, 2), "uqdecb w12, all, mul #2");

  COMPARE_MACRO(Uqdecb(x30), "uqdecb x30");
  COMPARE_MACRO(Uqdecb(xzr, SVE_POW2), "uqdecb xzr, pow2");
  COMPARE_MACRO(Uqdecb(xzr, SVE_MUL4, 1), "uqdecb xzr, mul4");

  COMPARE_MACRO(Uqdecw(w22), "uqdecw w22");
  COMPARE_MACRO(Uqdecd(w22, SVE_POW2, 1), "uqdecd w22, pow2");
  COMPARE_MACRO(Uqdech(w22, SVE_VL1, 16), "uqdech w22, vl1, mul #16");
  COMPARE_MACRO(Uqdecw(x22, SVE_VL2, 15), "uqdecw x22, vl2, mul #15");
  COMPARE_MACRO(Uqdecd(x22, SVE_VL16, 14), "uqdecd x22, vl16, mul #14");
  COMPARE_MACRO(Uqdech(x22, SVE_VL256, 8), "uqdech x22, vl256, mul #8");
  COMPARE_MACRO(Uqdecw(w22, SVE_MUL4, 4), "uqdecw w22, mul4, mul #4");
  COMPARE_MACRO(Uqdecd(w22, SVE_MUL3, 3), "uqdecd w22, mul3, mul #3");
  COMPARE_MACRO(Uqdech(w22, SVE_ALL, 2), "uqdech w22, all, mul #2");

  COMPARE_MACRO(Uqincb(w29), "uqincb w29");
  COMPARE_MACRO(Uqincb(w29, SVE_POW2), "uqincb w29, pow2");
  COMPARE_MACRO(Uqincb(w29, SVE_VL1), "uqincb w29, vl1");
  COMPARE_MACRO(Uqincb(w29, SVE_VL2), "uqincb w29, vl2");
  COMPARE_MACRO(Uqincb(w29, SVE_VL16), "uqincb w29, vl16");
  COMPARE_MACRO(Uqincb(w29, SVE_VL256), "uqincb w29, vl256");
  COMPARE_MACRO(Uqincb(w29, SVE_MUL4), "uqincb w29, mul4");
  COMPARE_MACRO(Uqincb(w29, SVE_MUL3), "uqincb w29, mul3");
  COMPARE_MACRO(Uqincb(w29, SVE_ALL), "uqincb w29");

  COMPARE_MACRO(Uqincb(w29, SVE_POW2, 1), "uqincb w29, pow2");
  COMPARE_MACRO(Uqincb(w29, SVE_VL1, 16), "uqincb w29, vl1, mul #16");
  COMPARE_MACRO(Uqincb(w29, SVE_VL2, 15), "uqincb w29, vl2, mul #15");
  COMPARE_MACRO(Uqincb(w29, SVE_VL16, 14), "uqincb w29, vl16, mul #14");
  COMPARE_MACRO(Uqincb(w29, SVE_VL256, 8), "uqincb w29, vl256, mul #8");
  COMPARE_MACRO(Uqincb(w29, SVE_MUL4, 4), "uqincb w29, mul4, mul #4");
  COMPARE_MACRO(Uqincb(w29, SVE_MUL3, 3), "uqincb w29, mul3, mul #3");
  COMPARE_MACRO(Uqincb(w29, SVE_ALL, 2), "uqincb w29, all, mul #2");

  COMPARE_MACRO(Uqincb(x30), "uqincb x30");
  COMPARE_MACRO(Uqincb(xzr, SVE_POW2), "uqincb xzr, pow2");
  COMPARE_MACRO(Uqincb(xzr, SVE_MUL4, 1), "uqincb xzr, mul4");

  COMPARE_MACRO(Uqinch(w11), "uqinch w11");
  COMPARE_MACRO(Uqincw(w11, SVE_POW2), "uqincw w11, pow2");
  COMPARE_MACRO(Uqincd(w11, SVE_VL1), "uqincd w11, vl1");
  COMPARE_MACRO(Uqinch(x11, SVE_VL2), "uqinch x11, vl2");
  COMPARE_MACRO(Uqincw(x11, SVE_VL16), "uqincw x11, vl16");
  COMPARE_MACRO(Uqincd(x11, SVE_VL256), "uqincd x11, vl256");
  COMPARE_MACRO(Uqinch(w11, SVE_MUL4), "uqinch w11, mul4");
  COMPARE_MACRO(Uqincw(w11, SVE_MUL3), "uqincw w11, mul3");
  COMPARE_MACRO(Uqincd(w11, SVE_ALL), "uqincd w11");

  CLEANUP();
}

TEST(sve_vector_dec_element_count) {
  SETUP();

  COMPARE_MACRO(Decd(z14.VnD()), "decd z14.d");
  COMPARE_MACRO(Decd(z14.VnD(), SVE_POW2), "decd z14.d, pow2");
  COMPARE_MACRO(Decd(z14.VnD(), SVE_VL1), "decd z14.d, vl1");
  COMPARE_MACRO(Decd(z14.VnD(), SVE_VL2), "decd z14.d, vl2");
  COMPARE_MACRO(Decd(z14.VnD(), SVE_VL16), "decd z14.d, vl16");
  COMPARE_MACRO(Decd(z14.VnD(), SVE_VL256), "decd z14.d, vl256");
  COMPARE_MACRO(Decd(z14.VnD(), SVE_MUL4), "decd z14.d, mul4");
  COMPARE_MACRO(Decd(z14.VnD(), SVE_MUL3), "decd z14.d, mul3");
  COMPARE_MACRO(Decd(z14.VnD(), SVE_ALL), "decd z14.d");

  COMPARE_MACRO(Decd(z19.VnD(), SVE_POW2, 1), "decd z19.d, pow2");
  COMPARE_MACRO(Decd(z19.VnD(), SVE_VL1, 16), "decd z19.d, vl1, mul #16");
  COMPARE_MACRO(Decd(z19.VnD(), SVE_VL2, 15), "decd z19.d, vl2, mul #15");
  COMPARE_MACRO(Decd(z19.VnD(), SVE_VL16, 14), "decd z19.d, vl16, mul #14");
  COMPARE_MACRO(Decd(z19.VnD(), SVE_VL256, 8), "decd z19.d, vl256, mul #8");
  COMPARE_MACRO(Decd(z19.VnD(), SVE_MUL4, 4), "decd z19.d, mul4, mul #4");
  COMPARE_MACRO(Decd(z19.VnD(), SVE_MUL3, 3), "decd z19.d, mul3, mul #3");
  COMPARE_MACRO(Decd(z19.VnD(), SVE_ALL, 2), "decd z19.d, all, mul #2");

  COMPARE_MACRO(Dech(z27.VnH(), SVE_POW2, 1), "dech z27.h, pow2");
  COMPARE_MACRO(Decw(z27.VnS(), SVE_VL1, 16), "decw z27.s, vl1, mul #16");
  COMPARE_MACRO(Dech(z27.VnH(), SVE_VL2, 15), "dech z27.h, vl2, mul #15");
  COMPARE_MACRO(Decw(z27.VnS(), SVE_VL16, 14), "decw z27.s, vl16, mul #14");
  COMPARE_MACRO(Dech(z27.VnH(), SVE_VL256, 8), "dech z27.h, vl256, mul #8");
  COMPARE_MACRO(Decw(z27.VnS(), SVE_MUL4, 4), "decw z27.s, mul4, mul #4");
  COMPARE_MACRO(Dech(z27.VnH(), SVE_MUL3, 3), "dech z27.h, mul3, mul #3");
  COMPARE_MACRO(Decw(z27.VnS(), SVE_ALL, 2), "decw z27.s, all, mul #2");

  COMPARE_MACRO(Sqdecd(z13.VnD()), "sqdecd z13.d");
  COMPARE_MACRO(Sqdecd(z13.VnD(), SVE_POW2), "sqdecd z13.d, pow2");
  COMPARE_MACRO(Sqdecd(z13.VnD(), SVE_VL1), "sqdecd z13.d, vl1");
  COMPARE_MACRO(Sqdecd(z13.VnD(), SVE_VL2), "sqdecd z13.d, vl2");
  COMPARE_MACRO(Sqdecd(z13.VnD(), SVE_VL16), "sqdecd z13.d, vl16");
  COMPARE_MACRO(Sqdecd(z13.VnD(), SVE_VL256), "sqdecd z13.d, vl256");
  COMPARE_MACRO(Sqdecd(z13.VnD(), SVE_MUL4), "sqdecd z13.d, mul4");
  COMPARE_MACRO(Sqdecd(z13.VnD(), SVE_MUL3), "sqdecd z13.d, mul3");
  COMPARE_MACRO(Sqdecd(z13.VnD(), SVE_ALL), "sqdecd z13.d");

  COMPARE_MACRO(Sqdecd(z9.VnD(), SVE_POW2, 1), "sqdecd z9.d, pow2");
  COMPARE_MACRO(Sqdecd(z9.VnD(), SVE_VL1, 16), "sqdecd z9.d, vl1, mul #16");
  COMPARE_MACRO(Sqdecd(z9.VnD(), SVE_VL2, 15), "sqdecd z9.d, vl2, mul #15");
  COMPARE_MACRO(Sqdecd(z9.VnD(), SVE_VL16, 14), "sqdecd z9.d, vl16, mul #14");
  COMPARE_MACRO(Sqdecd(z9.VnD(), SVE_VL256, 8), "sqdecd z9.d, vl256, mul #8");
  COMPARE_MACRO(Sqdecd(z9.VnD(), SVE_MUL4, 4), "sqdecd z9.d, mul4, mul #4");
  COMPARE_MACRO(Sqdecd(z9.VnD(), SVE_MUL3, 3), "sqdecd z9.d, mul3, mul #3");
  COMPARE_MACRO(Sqdecd(z9.VnD(), SVE_ALL, 2), "sqdecd z9.d, all, mul #2");

  COMPARE_MACRO(Sqdech(z31.VnH(), SVE_POW2, 1), "sqdech z31.h, pow2");
  COMPARE_MACRO(Sqdecw(z31.VnS(), SVE_VL1, 16), "sqdecw z31.s, vl1, mul #16");
  COMPARE_MACRO(Sqdech(z31.VnH(), SVE_VL2, 15), "sqdech z31.h, vl2, mul #15");
  COMPARE_MACRO(Sqdecw(z31.VnS(), SVE_VL16, 14), "sqdecw z31.s, vl16, mul #14");
  COMPARE_MACRO(Sqdech(z31.VnH(), SVE_VL256, 8), "sqdech z31.h, vl256, mul #8");
  COMPARE_MACRO(Sqdecw(z31.VnS(), SVE_MUL4, 4), "sqdecw z31.s, mul4, mul #4");
  COMPARE_MACRO(Sqdech(z31.VnH(), SVE_MUL3, 3), "sqdech z31.h, mul3, mul #3");
  COMPARE_MACRO(Sqdecw(z31.VnS(), SVE_ALL, 2), "sqdecw z31.s, all, mul #2");

  COMPARE_MACRO(Uqdecd(z1.VnD()), "uqdecd z1.d");
  COMPARE_MACRO(Uqdecd(z1.VnD(), SVE_POW2), "uqdecd z1.d, pow2");
  COMPARE_MACRO(Uqdecd(z1.VnD(), SVE_VL1), "uqdecd z1.d, vl1");
  COMPARE_MACRO(Uqdecd(z1.VnD(), SVE_VL2), "uqdecd z1.d, vl2");
  COMPARE_MACRO(Uqdecd(z1.VnD(), SVE_VL16), "uqdecd z1.d, vl16");
  COMPARE_MACRO(Uqdecd(z1.VnD(), SVE_VL256), "uqdecd z1.d, vl256");
  COMPARE_MACRO(Uqdecd(z1.VnD(), SVE_MUL4), "uqdecd z1.d, mul4");
  COMPARE_MACRO(Uqdecd(z1.VnD(), SVE_MUL3), "uqdecd z1.d, mul3");
  COMPARE_MACRO(Uqdecd(z1.VnD(), SVE_ALL), "uqdecd z1.d");

  COMPARE_MACRO(Uqdecd(z7.VnD(), SVE_POW2, 1), "uqdecd z7.d, pow2");
  COMPARE_MACRO(Uqdecd(z7.VnD(), SVE_VL1, 16), "uqdecd z7.d, vl1, mul #16");
  COMPARE_MACRO(Uqdecd(z7.VnD(), SVE_VL2, 15), "uqdecd z7.d, vl2, mul #15");
  COMPARE_MACRO(Uqdecd(z7.VnD(), SVE_VL16, 14), "uqdecd z7.d, vl16, mul #14");
  COMPARE_MACRO(Uqdecd(z7.VnD(), SVE_VL256, 8), "uqdecd z7.d, vl256, mul #8");
  COMPARE_MACRO(Uqdecd(z7.VnD(), SVE_MUL4, 4), "uqdecd z7.d, mul4, mul #4");
  COMPARE_MACRO(Uqdecd(z7.VnD(), SVE_MUL3, 3), "uqdecd z7.d, mul3, mul #3");
  COMPARE_MACRO(Uqdecd(z7.VnD(), SVE_ALL, 2), "uqdecd z7.d, all, mul #2");

  COMPARE_MACRO(Uqdech(z26.VnH(), SVE_POW2, 1), "uqdech z26.h, pow2");
  COMPARE_MACRO(Uqdecw(z26.VnS(), SVE_VL1, 16), "uqdecw z26.s, vl1, mul #16");
  COMPARE_MACRO(Uqdech(z26.VnH(), SVE_VL2, 15), "uqdech z26.h, vl2, mul #15");
  COMPARE_MACRO(Uqdecw(z26.VnS(), SVE_VL16, 14), "uqdecw z26.s, vl16, mul #14");
  COMPARE_MACRO(Uqdech(z26.VnH(), SVE_VL256, 8), "uqdech z26.h, vl256, mul #8");
  COMPARE_MACRO(Uqdecw(z26.VnS(), SVE_MUL4, 4), "uqdecw z26.s, mul4, mul #4");
  COMPARE_MACRO(Uqdech(z26.VnH(), SVE_MUL3, 3), "uqdech z26.h, mul3, mul #3");
  COMPARE_MACRO(Uqdecw(z26.VnS(), SVE_ALL, 2), "uqdecw z26.s, all, mul #2");

  CLEANUP();
}

TEST(sve_vector_inc_element_count) {
  SETUP();

  COMPARE_MACRO(Incd(z16.VnD()), "incd z16.d");
  COMPARE_MACRO(Incd(z16.VnD(), SVE_POW2), "incd z16.d, pow2");
  COMPARE_MACRO(Incd(z16.VnD(), SVE_VL1), "incd z16.d, vl1");
  COMPARE_MACRO(Incd(z16.VnD(), SVE_VL2), "incd z16.d, vl2");
  COMPARE_MACRO(Incd(z16.VnD(), SVE_VL16), "incd z16.d, vl16");
  COMPARE_MACRO(Incd(z16.VnD(), SVE_VL256), "incd z16.d, vl256");
  COMPARE_MACRO(Incd(z16.VnD(), SVE_MUL4), "incd z16.d, mul4");
  COMPARE_MACRO(Incd(z16.VnD(), SVE_MUL3), "incd z16.d, mul3");
  COMPARE_MACRO(Incd(z16.VnD(), SVE_ALL), "incd z16.d");

  COMPARE_MACRO(Incd(z18.VnD(), SVE_POW2, 1), "incd z18.d, pow2");
  COMPARE_MACRO(Incd(z18.VnD(), SVE_VL1, 16), "incd z18.d, vl1, mul #16");
  COMPARE_MACRO(Incd(z18.VnD(), SVE_VL2, 15), "incd z18.d, vl2, mul #15");
  COMPARE_MACRO(Incd(z18.VnD(), SVE_VL16, 14), "incd z18.d, vl16, mul #14");
  COMPARE_MACRO(Incd(z18.VnD(), SVE_VL256, 8), "incd z18.d, vl256, mul #8");
  COMPARE_MACRO(Incd(z18.VnD(), SVE_MUL4, 4), "incd z18.d, mul4, mul #4");
  COMPARE_MACRO(Incd(z18.VnD(), SVE_MUL3, 3), "incd z18.d, mul3, mul #3");
  COMPARE_MACRO(Incd(z18.VnD(), SVE_ALL, 2), "incd z18.d, all, mul #2");

  COMPARE_MACRO(Inch(z24.VnH(), SVE_POW2, 1), "inch z24.h, pow2");
  COMPARE_MACRO(Incw(z24.VnS(), SVE_VL1, 16), "incw z24.s, vl1, mul #16");
  COMPARE_MACRO(Inch(z24.VnH(), SVE_VL2, 15), "inch z24.h, vl2, mul #15");
  COMPARE_MACRO(Incw(z24.VnS(), SVE_VL16, 14), "incw z24.s, vl16, mul #14");
  COMPARE_MACRO(Inch(z24.VnH(), SVE_VL256, 8), "inch z24.h, vl256, mul #8");
  COMPARE_MACRO(Incw(z24.VnS(), SVE_MUL4, 4), "incw z24.s, mul4, mul #4");
  COMPARE_MACRO(Inch(z24.VnH(), SVE_MUL3, 3), "inch z24.h, mul3, mul #3");
  COMPARE_MACRO(Incw(z24.VnS(), SVE_ALL, 2), "incw z24.s, all, mul #2");

  COMPARE_MACRO(Sqincd(z10.VnD()), "sqincd z10.d");
  COMPARE_MACRO(Sqincd(z10.VnD(), SVE_POW2), "sqincd z10.d, pow2");
  COMPARE_MACRO(Sqincd(z10.VnD(), SVE_VL1), "sqincd z10.d, vl1");
  COMPARE_MACRO(Sqincd(z10.VnD(), SVE_VL2), "sqincd z10.d, vl2");
  COMPARE_MACRO(Sqincd(z10.VnD(), SVE_VL16), "sqincd z10.d, vl16");
  COMPARE_MACRO(Sqincd(z10.VnD(), SVE_VL256), "sqincd z10.d, vl256");
  COMPARE_MACRO(Sqincd(z10.VnD(), SVE_MUL4), "sqincd z10.d, mul4");
  COMPARE_MACRO(Sqincd(z10.VnD(), SVE_MUL3), "sqincd z10.d, mul3");
  COMPARE_MACRO(Sqincd(z10.VnD(), SVE_ALL), "sqincd z10.d");

  COMPARE_MACRO(Sqincd(z3.VnD(), SVE_POW2, 1), "sqincd z3.d, pow2");
  COMPARE_MACRO(Sqincd(z3.VnD(), SVE_VL1, 16), "sqincd z3.d, vl1, mul #16");
  COMPARE_MACRO(Sqincd(z3.VnD(), SVE_VL2, 15), "sqincd z3.d, vl2, mul #15");
  COMPARE_MACRO(Sqincd(z3.VnD(), SVE_VL16, 14), "sqincd z3.d, vl16, mul #14");
  COMPARE_MACRO(Sqincd(z3.VnD(), SVE_VL256, 8), "sqincd z3.d, vl256, mul #8");
  COMPARE_MACRO(Sqincd(z3.VnD(), SVE_MUL4, 4), "sqincd z3.d, mul4, mul #4");
  COMPARE_MACRO(Sqincd(z3.VnD(), SVE_MUL3, 3), "sqincd z3.d, mul3, mul #3");
  COMPARE_MACRO(Sqincd(z3.VnD(), SVE_ALL, 2), "sqincd z3.d, all, mul #2");

  COMPARE_MACRO(Sqinch(z24.VnH(), SVE_POW2, 1), "sqinch z24.h, pow2");
  COMPARE_MACRO(Sqincw(z24.VnS(), SVE_VL1, 16), "sqincw z24.s, vl1, mul #16");
  COMPARE_MACRO(Sqinch(z24.VnH(), SVE_VL2, 15), "sqinch z24.h, vl2, mul #15");
  COMPARE_MACRO(Sqincw(z24.VnS(), SVE_VL16, 14), "sqincw z24.s, vl16, mul #14");
  COMPARE_MACRO(Sqinch(z24.VnH(), SVE_VL256, 8), "sqinch z24.h, vl256, mul #8");
  COMPARE_MACRO(Sqincw(z24.VnS(), SVE_MUL4, 4), "sqincw z24.s, mul4, mul #4");
  COMPARE_MACRO(Sqinch(z24.VnH(), SVE_MUL3, 3), "sqinch z24.h, mul3, mul #3");
  COMPARE_MACRO(Sqincw(z24.VnS(), SVE_ALL, 2), "sqincw z24.s, all, mul #2");

  COMPARE_MACRO(Uqincd(z10.VnD()), "uqincd z10.d");
  COMPARE_MACRO(Uqincd(z10.VnD(), SVE_POW2), "uqincd z10.d, pow2");
  COMPARE_MACRO(Uqincd(z10.VnD(), SVE_VL1), "uqincd z10.d, vl1");
  COMPARE_MACRO(Uqincd(z10.VnD(), SVE_VL2), "uqincd z10.d, vl2");
  COMPARE_MACRO(Uqincd(z10.VnD(), SVE_VL16), "uqincd z10.d, vl16");
  COMPARE_MACRO(Uqincd(z10.VnD(), SVE_VL256), "uqincd z10.d, vl256");
  COMPARE_MACRO(Uqincd(z10.VnD(), SVE_MUL4), "uqincd z10.d, mul4");
  COMPARE_MACRO(Uqincd(z10.VnD(), SVE_MUL3), "uqincd z10.d, mul3");
  COMPARE_MACRO(Uqincd(z10.VnD(), SVE_ALL), "uqincd z10.d");

  COMPARE_MACRO(Uqincd(z6.VnD(), SVE_POW2, 1), "uqincd z6.d, pow2");
  COMPARE_MACRO(Uqincd(z6.VnD(), SVE_VL1, 16), "uqincd z6.d, vl1, mul #16");
  COMPARE_MACRO(Uqincd(z6.VnD(), SVE_VL2, 15), "uqincd z6.d, vl2, mul #15");
  COMPARE_MACRO(Uqincd(z6.VnD(), SVE_VL16, 14), "uqincd z6.d, vl16, mul #14");
  COMPARE_MACRO(Uqincd(z6.VnD(), SVE_VL256, 8), "uqincd z6.d, vl256, mul #8");
  COMPARE_MACRO(Uqincd(z6.VnD(), SVE_MUL4, 4), "uqincd z6.d, mul4, mul #4");
  COMPARE_MACRO(Uqincd(z6.VnD(), SVE_MUL3, 3), "uqincd z6.d, mul3, mul #3");
  COMPARE_MACRO(Uqincd(z6.VnD(), SVE_ALL, 2), "uqincd z6.d, all, mul #2");

  COMPARE_MACRO(Uqinch(z20.VnH(), SVE_POW2, 1), "uqinch z20.h, pow2");
  COMPARE_MACRO(Uqincw(z20.VnS(), SVE_VL1, 16), "uqincw z20.s, vl1, mul #16");
  COMPARE_MACRO(Uqinch(z20.VnH(), SVE_VL2, 15), "uqinch z20.h, vl2, mul #15");
  COMPARE_MACRO(Uqincw(z20.VnS(), SVE_VL16, 14), "uqincw z20.s, vl16, mul #14");
  COMPARE_MACRO(Uqinch(z20.VnH(), SVE_VL256, 8), "uqinch z20.h, vl256, mul #8");
  COMPARE_MACRO(Uqincw(z20.VnS(), SVE_MUL4, 4), "uqincw z20.s, mul4, mul #4");
  COMPARE_MACRO(Uqinch(z20.VnH(), SVE_MUL3, 3), "uqinch z20.h, mul3, mul #3");
  COMPARE_MACRO(Uqincw(z20.VnS(), SVE_ALL, 2), "uqincw z20.s, all, mul #2");

  CLEANUP();
}

TEST(sve_fp_accumulating_reduction) {
  SETUP();

  COMPARE(fadda(h10, p2, h10, z0.VnH()), "fadda h10, p2, h10, z0.h");
  COMPARE(fadda(s10, p2, s10, z0.VnS()), "fadda s10, p2, s10, z0.s");
  COMPARE(fadda(d10, p2, d10, z0.VnD()), "fadda d10, p2, d10, z0.d");

  CLEANUP();
}

TEST(sve_fp_arithmetic_predicated) {
  SETUP();

  COMPARE(fdiv(z9.VnH(), p4.Merging(), z9.VnH(), z4.VnH()),
          "fdiv z9.h, p4/m, z9.h, z4.h");
  COMPARE(fdiv(z19.VnS(), p5.Merging(), z19.VnS(), z14.VnS()),
          "fdiv z19.s, p5/m, z19.s, z14.s");
  COMPARE(fdiv(z29.VnD(), p6.Merging(), z29.VnD(), z24.VnD()),
          "fdiv z29.d, p6/m, z29.d, z24.d");
  COMPARE(fdivr(z21.VnH(), p3.Merging(), z21.VnH(), z11.VnH()),
          "fdivr z21.h, p3/m, z21.h, z11.h");
  COMPARE(fdivr(z23.VnS(), p5.Merging(), z23.VnS(), z15.VnS()),
          "fdivr z23.s, p5/m, z23.s, z15.s");
  COMPARE(fdivr(z25.VnD(), p7.Merging(), z25.VnD(), z19.VnD()),
          "fdivr z25.d, p7/m, z25.d, z19.d");
  COMPARE(fmax(z4.VnH(), p1.Merging(), z4.VnH(), z29.VnH()),
          "fmax z4.h, p1/m, z4.h, z29.h");
  COMPARE(fmax(z14.VnS(), p3.Merging(), z14.VnS(), z29.VnS()),
          "fmax z14.s, p3/m, z14.s, z29.s");
  COMPARE(fmax(z24.VnD(), p5.Merging(), z24.VnD(), z29.VnD()),
          "fmax z24.d, p5/m, z24.d, z29.d");
  COMPARE(fmin(z1.VnH(), p2.Merging(), z1.VnH(), z30.VnH()),
          "fmin z1.h, p2/m, z1.h, z30.h");
  COMPARE(fmin(z11.VnS(), p4.Merging(), z11.VnS(), z30.VnS()),
          "fmin z11.s, p4/m, z11.s, z30.s");
  COMPARE(fmin(z21.VnD(), p6.Merging(), z21.VnD(), z30.VnD()),
          "fmin z21.d, p6/m, z21.d, z30.d");

  COMPARE(ftmad(z21.VnH(), z21.VnH(), z22.VnH(), 0),
          "ftmad z21.h, z21.h, z22.h, #0");
  COMPARE(ftmad(z21.VnH(), z21.VnH(), z22.VnH(), 2),
          "ftmad z21.h, z21.h, z22.h, #2");
  COMPARE(ftmad(z2.VnH(), z2.VnH(), z21.VnH(), 7),
          "ftmad z2.h, z2.h, z21.h, #7");
  COMPARE(ftmad(z21.VnS(), z21.VnS(), z22.VnS(), 0),
          "ftmad z21.s, z21.s, z22.s, #0");
  COMPARE(ftmad(z21.VnS(), z21.VnS(), z22.VnS(), 2),
          "ftmad z21.s, z21.s, z22.s, #2");
  COMPARE(ftmad(z2.VnS(), z2.VnS(), z21.VnS(), 7),
          "ftmad z2.s, z2.s, z21.s, #7");
  COMPARE(ftmad(z21.VnD(), z21.VnD(), z22.VnD(), 0),
          "ftmad z21.d, z21.d, z22.d, #0");
  COMPARE(ftmad(z21.VnD(), z21.VnD(), z22.VnD(), 2),
          "ftmad z21.d, z21.d, z22.d, #2");
  COMPARE(ftmad(z2.VnD(), z2.VnD(), z21.VnD(), 7),
          "ftmad z2.d, z2.d, z21.d, #7");

  COMPARE_MACRO(Ftmad(z3.VnH(), z2.VnH(), z1.VnH(), 1),
                "movprfx z3, z2\n"
                "ftmad z3.h, z3.h, z1.h, #1");
  COMPARE_MACRO(Ftmad(z6.VnS(), z4.VnS(), z6.VnS(), 1),
                "mov z31.d, z6.d\n"
                "movprfx z6, z4\n"
                "ftmad z6.s, z6.s, z31.s, #1");

  COMPARE(fabd(z31.VnH(), p7.Merging(), z31.VnH(), z17.VnH()),
          "fabd z31.h, p7/m, z31.h, z17.h");
  COMPARE(fabd(z31.VnS(), p7.Merging(), z31.VnS(), z17.VnS()),
          "fabd z31.s, p7/m, z31.s, z17.s");
  COMPARE(fabd(z31.VnD(), p7.Merging(), z31.VnD(), z17.VnD()),
          "fabd z31.d, p7/m, z31.d, z17.d");
  COMPARE(fadd(z24.VnH(), p2.Merging(), z24.VnH(), z15.VnH()),
          "fadd z24.h, p2/m, z24.h, z15.h");
  COMPARE(fadd(z24.VnS(), p2.Merging(), z24.VnS(), z15.VnS()),
          "fadd z24.s, p2/m, z24.s, z15.s");
  COMPARE(fadd(z24.VnD(), p2.Merging(), z24.VnD(), z15.VnD()),
          "fadd z24.d, p2/m, z24.d, z15.d");
  COMPARE(fmaxnm(z15.VnH(), p4.Merging(), z15.VnH(), z3.VnH()),
          "fmaxnm z15.h, p4/m, z15.h, z3.h");
  COMPARE(fmaxnm(z15.VnS(), p4.Merging(), z15.VnS(), z3.VnS()),
          "fmaxnm z15.s, p4/m, z15.s, z3.s");
  COMPARE(fmaxnm(z15.VnD(), p4.Merging(), z15.VnD(), z3.VnD()),
          "fmaxnm z15.d, p4/m, z15.d, z3.d");
  COMPARE(fminnm(z19.VnH(), p2.Merging(), z19.VnH(), z29.VnH()),
          "fminnm z19.h, p2/m, z19.h, z29.h");
  COMPARE(fminnm(z19.VnS(), p2.Merging(), z19.VnS(), z29.VnS()),
          "fminnm z19.s, p2/m, z19.s, z29.s");
  COMPARE(fminnm(z19.VnD(), p2.Merging(), z19.VnD(), z29.VnD()),
          "fminnm z19.d, p2/m, z19.d, z29.d");
  COMPARE(fmulx(z30.VnH(), p6.Merging(), z30.VnH(), z20.VnH()),
          "fmulx z30.h, p6/m, z30.h, z20.h");
  COMPARE(fmulx(z30.VnS(), p6.Merging(), z30.VnS(), z20.VnS()),
          "fmulx z30.s, p6/m, z30.s, z20.s");
  COMPARE(fmulx(z30.VnD(), p6.Merging(), z30.VnD(), z20.VnD()),
          "fmulx z30.d, p6/m, z30.d, z20.d");
  COMPARE(fmul(z26.VnH(), p2.Merging(), z26.VnH(), z6.VnH()),
          "fmul z26.h, p2/m, z26.h, z6.h");
  COMPARE(fmul(z26.VnS(), p2.Merging(), z26.VnS(), z6.VnS()),
          "fmul z26.s, p2/m, z26.s, z6.s");
  COMPARE(fmul(z26.VnD(), p2.Merging(), z26.VnD(), z6.VnD()),
          "fmul z26.d, p2/m, z26.d, z6.d");
  COMPARE(fscale(z8.VnH(), p3.Merging(), z8.VnH(), z6.VnH()),
          "fscale z8.h, p3/m, z8.h, z6.h");
  COMPARE(fscale(z8.VnS(), p3.Merging(), z8.VnS(), z6.VnS()),
          "fscale z8.s, p3/m, z8.s, z6.s");
  COMPARE(fscale(z8.VnD(), p3.Merging(), z8.VnD(), z6.VnD()),
          "fscale z8.d, p3/m, z8.d, z6.d");
  COMPARE(fsubr(z16.VnH(), p5.Merging(), z16.VnH(), z15.VnH()),
          "fsubr z16.h, p5/m, z16.h, z15.h");
  COMPARE(fsubr(z16.VnS(), p5.Merging(), z16.VnS(), z15.VnS()),
          "fsubr z16.s, p5/m, z16.s, z15.s");
  COMPARE(fsubr(z16.VnD(), p5.Merging(), z16.VnD(), z15.VnD()),
          "fsubr z16.d, p5/m, z16.d, z15.d");
  COMPARE(fsub(z16.VnH(), p5.Merging(), z16.VnH(), z26.VnH()),
          "fsub z16.h, p5/m, z16.h, z26.h");
  COMPARE(fsub(z16.VnS(), p5.Merging(), z16.VnS(), z26.VnS()),
          "fsub z16.s, p5/m, z16.s, z26.s");
  COMPARE(fsub(z16.VnD(), p5.Merging(), z16.VnD(), z26.VnD()),
          "fsub z16.d, p5/m, z16.d, z26.d");

  COMPARE_MACRO(Fsub(z0.VnH(), p0.Merging(), z1.VnH(), z0.VnH()),
                "fsubr z0.h, p0/m, z0.h, z1.h");

  COMPARE_MACRO(Fadd(z0.VnH(),
                     p0.Merging(),
                     z1.VnH(),
                     z2.VnH(),
                     FastNaNPropagation),
                "movprfx z0.h, p0/m, z1.h\n"
                "fadd z0.h, p0/m, z0.h, z2.h");
  COMPARE_MACRO(Fadd(z0.VnH(),
                     p0.Merging(),
                     z1.VnH(),
                     z0.VnH(),
                     FastNaNPropagation),
                "fadd z0.h, p0/m, z0.h, z1.h");
  COMPARE_MACRO(Fadd(z0.VnH(),
                     p0.Merging(),
                     z1.VnH(),
                     z0.VnH(),
                     StrictNaNPropagation),
                "movprfx z31.h, p0/m, z1.h\n"
                "fadd z31.h, p0/m, z31.h, z0.h\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Fmul(z1.VnS(),
                     p1.Merging(),
                     z2.VnS(),
                     z3.VnS(),
                     FastNaNPropagation),
                "movprfx z1.s, p1/m, z2.s\n"
                "fmul z1.s, p1/m, z1.s, z3.s");
  COMPARE_MACRO(Fmulx(z2.VnD(),
                      p2.Merging(),
                      z3.VnD(),
                      z4.VnD(),
                      FastNaNPropagation),
                "movprfx z2.d, p2/m, z3.d\n"
                "fmulx z2.d, p2/m, z2.d, z4.d");
  COMPARE_MACRO(Fminnm(z3.VnH(),
                       p3.Merging(),
                       z4.VnH(),
                       z5.VnH(),
                       FastNaNPropagation),
                "movprfx z3.h, p3/m, z4.h\n"
                "fminnm z3.h, p3/m, z3.h, z5.h");
  COMPARE_MACRO(Fmaxnm(z4.VnS(),
                       p4.Merging(),
                       z5.VnS(),
                       z6.VnS(),
                       FastNaNPropagation),
                "movprfx z4.s, p4/m, z5.s\n"
                "fmaxnm z4.s, p4/m, z4.s, z6.s");
  COMPARE_MACRO(Fsub(z5.VnD(), p5.Merging(), z6.VnD(), z7.VnD()),
                "movprfx z5.d, p5/m, z6.d\n"
                "fsub z5.d, p5/m, z5.d, z7.d");
  COMPARE_MACRO(Fscale(z6.VnH(), p6.Merging(), z7.VnH(), z8.VnH()),
                "movprfx z6.h, p6/m, z7.h\n"
                "fscale z6.h, p6/m, z6.h, z8.h");
  COMPARE_MACRO(Fscale(z7.VnS(), p7.Merging(), z8.VnS(), z7.VnS()),
                "mov z31.d, z7.d\n"
                "movprfx z7.s, p7/m, z8.s\n"
                "fscale z7.s, p7/m, z7.s, z31.s");

  COMPARE(fadd(z18.VnH(), p0.Merging(), z18.VnH(), 0.5),
          "fadd z18.h, p0/m, z18.h, #0.5");
  COMPARE(fadd(z18.VnS(), p0.Merging(), z18.VnS(), 1.0),
          "fadd z18.s, p0/m, z18.s, #1.0");
  COMPARE(fadd(z18.VnD(), p0.Merging(), z18.VnD(), 1.0),
          "fadd z18.d, p0/m, z18.d, #1.0");
  COMPARE(fmaxnm(z6.VnH(), p1.Merging(), z6.VnH(), 0.0),
          "fmaxnm z6.h, p1/m, z6.h, #0.0");
  COMPARE(fmaxnm(z6.VnS(), p1.Merging(), z6.VnS(), 1.0),
          "fmaxnm z6.s, p1/m, z6.s, #1.0");
  COMPARE(fmaxnm(z6.VnD(), p1.Merging(), z6.VnD(), 1.0),
          "fmaxnm z6.d, p1/m, z6.d, #1.0");
  COMPARE(fmax(z8.VnH(), p6.Merging(), z8.VnH(), 0.0),
          "fmax z8.h, p6/m, z8.h, #0.0");
  COMPARE(fmax(z8.VnS(), p6.Merging(), z8.VnS(), 0.0),
          "fmax z8.s, p6/m, z8.s, #0.0");
  COMPARE(fmax(z8.VnD(), p6.Merging(), z8.VnD(), 1.0),
          "fmax z8.d, p6/m, z8.d, #1.0");
  COMPARE(fminnm(z26.VnH(), p0.Merging(), z26.VnH(), 1.0),
          "fminnm z26.h, p0/m, z26.h, #1.0");
  COMPARE(fminnm(z26.VnS(), p0.Merging(), z26.VnS(), 0.0),
          "fminnm z26.s, p0/m, z26.s, #0.0");
  COMPARE(fminnm(z26.VnD(), p0.Merging(), z26.VnD(), 1.0),
          "fminnm z26.d, p0/m, z26.d, #1.0");
  COMPARE(fmin(z22.VnH(), p0.Merging(), z22.VnH(), 1.0),
          "fmin z22.h, p0/m, z22.h, #1.0");
  COMPARE(fmin(z22.VnS(), p0.Merging(), z22.VnS(), 1.0),
          "fmin z22.s, p0/m, z22.s, #1.0");
  COMPARE(fmin(z22.VnD(), p0.Merging(), z22.VnD(), 0.0),
          "fmin z22.d, p0/m, z22.d, #0.0");
  COMPARE(fmul(z21.VnH(), p3.Merging(), z21.VnH(), 0.5),
          "fmul z21.h, p3/m, z21.h, #0.5");
  COMPARE(fmul(z21.VnS(), p3.Merging(), z21.VnS(), 2.0),
          "fmul z21.s, p3/m, z21.s, #2.0");
  COMPARE(fmul(z21.VnD(), p3.Merging(), z21.VnD(), 2.0),
          "fmul z21.d, p3/m, z21.d, #2.0");
  COMPARE(fsubr(z21.VnH(), p3.Merging(), z21.VnH(), 1.0),
          "fsubr z21.h, p3/m, z21.h, #1.0");
  COMPARE(fsubr(z21.VnS(), p3.Merging(), z21.VnS(), 0.5),
          "fsubr z21.s, p3/m, z21.s, #0.5");
  COMPARE(fsubr(z21.VnD(), p3.Merging(), z21.VnD(), 1.0),
          "fsubr z21.d, p3/m, z21.d, #1.0");
  COMPARE(fsub(z26.VnH(), p4.Merging(), z26.VnH(), 0.5),
          "fsub z26.h, p4/m, z26.h, #0.5");
  COMPARE(fsub(z26.VnS(), p4.Merging(), z26.VnS(), 1.0),
          "fsub z26.s, p4/m, z26.s, #1.0");
  COMPARE(fsub(z26.VnD(), p4.Merging(), z26.VnD(), 0.5),
          "fsub z26.d, p4/m, z26.d, #0.5");

  COMPARE_MACRO(Fadd(z18.VnH(), p0.Merging(), z8.VnH(), 1.0),
                "movprfx z18.h, p0/m, z8.h\n"
                "fadd z18.h, p0/m, z18.h, #1.0");
  COMPARE_MACRO(Fsub(z19.VnH(), p1.Merging(), z9.VnH(), 0.5),
                "movprfx z19.h, p1/m, z9.h\n"
                "fsub z19.h, p1/m, z19.h, #0.5");
  COMPARE_MACRO(Fsub(z20.VnH(), p2.Merging(), 1.0, z10.VnH()),
                "movprfx z20.h, p2/m, z10.h\n"
                "fsubr z20.h, p2/m, z20.h, #1.0");
  COMPARE_MACRO(Fmul(z21.VnH(), p3.Merging(), z11.VnH(), 2.0),
                "movprfx z21.h, p3/m, z11.h\n"
                "fmul z21.h, p3/m, z21.h, #2.0");
  COMPARE_MACRO(Fmin(z22.VnH(), p4.Merging(), z12.VnH(), 0.0),
                "movprfx z22.h, p4/m, z12.h\n"
                "fmin z22.h, p4/m, z22.h, #0.0");
  COMPARE_MACRO(Fminnm(z22.VnH(), p4.Merging(), z12.VnH(), 0.0),
                "movprfx z22.h, p4/m, z12.h\n"
                "fminnm z22.h, p4/m, z22.h, #0.0");
  COMPARE_MACRO(Fmax(z23.VnH(), p5.Merging(), z13.VnH(), 1.0),
                "movprfx z23.h, p5/m, z13.h\n"
                "fmax z23.h, p5/m, z23.h, #1.0");
  COMPARE_MACRO(Fmaxnm(z23.VnH(), p5.Merging(), z13.VnH(), 1.0),
                "movprfx z23.h, p5/m, z13.h\n"
                "fmaxnm z23.h, p5/m, z23.h, #1.0");
  CLEANUP();
}

TEST(sve_fp_arithmetic_predicated_macro_fast_nan_propagation) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

  // Shadow the `MacroAssembler` type so that the test macros work without
  // modification.
  typedef FastNaNPropagationMacroAssembler MacroAssembler;

  SETUP();

  COMPARE_MACRO(Fdiv(z8.VnH(), p4.Merging(), z8.VnH(), z4.VnH()),
                "fdiv z8.h, p4/m, z8.h, z4.h");
  COMPARE_MACRO(Fdiv(z18.VnS(), p5.Merging(), z18.VnS(), z14.VnS()),
                "fdiv z18.s, p5/m, z18.s, z14.s");
  COMPARE_MACRO(Fdiv(z28.VnD(), p6.Merging(), z28.VnD(), z24.VnD()),
                "fdiv z28.d, p6/m, z28.d, z24.d");
  COMPARE_MACRO(Fdiv(z17.VnH(), p2.Merging(), z17.VnH(), z17.VnH()),
                "fdiv z17.h, p2/m, z17.h, z17.h");
  COMPARE_MACRO(Fdiv(z20.VnS(), p5.Merging(), z23.VnS(), z20.VnS()),
                "fdivr z20.s, p5/m, z20.s, z23.s");
  COMPARE_MACRO(Fdiv(z15.VnD(), p6.Merging(), z30.VnD(), z8.VnD()),
                "movprfx z15.d, p6/m, z30.d\n"
                "fdiv z15.d, p6/m, z15.d, z8.d");

  COMPARE_MACRO(Fmax(z8.VnH(), p4.Merging(), z8.VnH(), z4.VnH()),
                "fmax z8.h, p4/m, z8.h, z4.h");
  COMPARE_MACRO(Fmax(z18.VnS(), p5.Merging(), z18.VnS(), z14.VnS()),
                "fmax z18.s, p5/m, z18.s, z14.s");
  COMPARE_MACRO(Fmax(z28.VnD(), p6.Merging(), z28.VnD(), z24.VnD()),
                "fmax z28.d, p6/m, z28.d, z24.d");
  COMPARE_MACRO(Fmax(z17.VnH(), p2.Merging(), z17.VnH(), z17.VnH()),
                "fmax z17.h, p2/m, z17.h, z17.h");
  COMPARE_MACRO(Fmax(z20.VnS(), p5.Merging(), z23.VnS(), z20.VnS()),
                "fmax z20.s, p5/m, z20.s, z23.s");
  COMPARE_MACRO(Fmax(z15.VnD(), p6.Merging(), z30.VnD(), z8.VnD()),
                "movprfx z15.d, p6/m, z30.d\n"
                "fmax z15.d, p6/m, z15.d, z8.d");

  COMPARE_MACRO(Fmin(z8.VnH(), p4.Merging(), z8.VnH(), z4.VnH()),
                "fmin z8.h, p4/m, z8.h, z4.h");
  COMPARE_MACRO(Fmin(z18.VnS(), p5.Merging(), z18.VnS(), z14.VnS()),
                "fmin z18.s, p5/m, z18.s, z14.s");
  COMPARE_MACRO(Fmin(z28.VnD(), p6.Merging(), z28.VnD(), z24.VnD()),
                "fmin z28.d, p6/m, z28.d, z24.d");
  COMPARE_MACRO(Fmin(z17.VnH(), p2.Merging(), z17.VnH(), z17.VnH()),
                "fmin z17.h, p2/m, z17.h, z17.h");
  COMPARE_MACRO(Fmin(z20.VnS(), p5.Merging(), z23.VnS(), z20.VnS()),
                "fmin z20.s, p5/m, z20.s, z23.s");
  COMPARE_MACRO(Fmin(z15.VnD(), p6.Merging(), z30.VnD(), z8.VnD()),
                "movprfx z15.d, p6/m, z30.d\n"
                "fmin z15.d, p6/m, z15.d, z8.d");

  CLEANUP();

#pragma GCC diagnostic pop
}

TEST(sve_fp_arithmetic_predicated_macro_strict_nan_propagation) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

  // Shadow the `MacroAssembler` type so that the test macros work without
  // modification.
  typedef StrictNaNPropagationMacroAssembler MacroAssembler;

  SETUP();

  COMPARE_MACRO(Fdiv(z8.VnH(), p4.Merging(), z8.VnH(), z4.VnH()),
                "fdiv z8.h, p4/m, z8.h, z4.h");
  COMPARE_MACRO(Fdiv(z18.VnS(), p5.Merging(), z18.VnS(), z14.VnS()),
                "fdiv z18.s, p5/m, z18.s, z14.s");
  COMPARE_MACRO(Fdiv(z28.VnD(), p6.Merging(), z28.VnD(), z24.VnD()),
                "fdiv z28.d, p6/m, z28.d, z24.d");
  COMPARE_MACRO(Fdiv(z17.VnH(), p2.Merging(), z17.VnH(), z17.VnH()),
                "fdiv z17.h, p2/m, z17.h, z17.h");
  COMPARE_MACRO(Fdiv(z20.VnS(), p5.Merging(), z23.VnS(), z20.VnS()),
                "fdivr z20.s, p5/m, z20.s, z23.s");
  COMPARE_MACRO(Fdiv(z15.VnD(), p6.Merging(), z30.VnD(), z8.VnD()),
                "movprfx z15.d, p6/m, z30.d\n"
                "fdiv z15.d, p6/m, z15.d, z8.d");

  COMPARE_MACRO(Fmax(z8.VnH(), p4.Merging(), z8.VnH(), z4.VnH()),
                "fmax z8.h, p4/m, z8.h, z4.h");
  COMPARE_MACRO(Fmax(z18.VnS(), p5.Merging(), z18.VnS(), z14.VnS()),
                "fmax z18.s, p5/m, z18.s, z14.s");
  COMPARE_MACRO(Fmax(z28.VnD(), p6.Merging(), z28.VnD(), z24.VnD()),
                "fmax z28.d, p6/m, z28.d, z24.d");
  COMPARE_MACRO(Fmax(z17.VnH(), p2.Merging(), z17.VnH(), z17.VnH()),
                "fmax z17.h, p2/m, z17.h, z17.h");
  COMPARE_MACRO(Fmax(z20.VnS(), p5.Merging(), z23.VnS(), z20.VnS()),
                "movprfx z31.s, p5/m, z23.s\n"
                "fmax z31.s, p5/m, z31.s, z20.s\n"
                "mov z20.d, z31.d");
  COMPARE_MACRO(Fmax(z15.VnD(), p6.Merging(), z30.VnD(), z8.VnD()),
                "movprfx z15.d, p6/m, z30.d\n"
                "fmax z15.d, p6/m, z15.d, z8.d");

  COMPARE_MACRO(Fmin(z8.VnH(), p4.Merging(), z8.VnH(), z4.VnH()),
                "fmin z8.h, p4/m, z8.h, z4.h");
  COMPARE_MACRO(Fmin(z18.VnS(), p5.Merging(), z18.VnS(), z14.VnS()),
                "fmin z18.s, p5/m, z18.s, z14.s");
  COMPARE_MACRO(Fmin(z28.VnD(), p6.Merging(), z28.VnD(), z24.VnD()),
                "fmin z28.d, p6/m, z28.d, z24.d");
  COMPARE_MACRO(Fmin(z17.VnH(), p2.Merging(), z17.VnH(), z17.VnH()),
                "fmin z17.h, p2/m, z17.h, z17.h");
  COMPARE_MACRO(Fmin(z20.VnS(), p5.Merging(), z23.VnS(), z20.VnS()),
                "movprfx z31.s, p5/m, z23.s\n"
                "fmin z31.s, p5/m, z31.s, z20.s\n"
                "mov z20.d, z31.d");
  COMPARE_MACRO(Fmin(z15.VnD(), p6.Merging(), z30.VnD(), z8.VnD()),
                "movprfx z15.d, p6/m, z30.d\n"
                "fmin z15.d, p6/m, z15.d, z8.d");

  CLEANUP();

#pragma GCC diagnostic pop
}

TEST(sve_fp_arithmetic_unpredicated) {
  SETUP();

  COMPARE(fadd(z5.VnH(), z24.VnH(), z11.VnH()), "fadd z5.h, z24.h, z11.h");
  COMPARE(fadd(z15.VnS(), z14.VnS(), z12.VnS()), "fadd z15.s, z14.s, z12.s");
  COMPARE(fadd(z25.VnD(), z4.VnD(), z13.VnD()), "fadd z25.d, z4.d, z13.d");
  COMPARE(fmul(z9.VnH(), z24.VnH(), z10.VnH()), "fmul z9.h, z24.h, z10.h");
  COMPARE(fmul(z19.VnS(), z14.VnS(), z0.VnS()), "fmul z19.s, z14.s, z0.s");
  COMPARE(fmul(z29.VnD(), z4.VnD(), z20.VnD()), "fmul z29.d, z4.d, z20.d");
  COMPARE(fsub(z4.VnH(), z14.VnH(), z29.VnH()), "fsub z4.h, z14.h, z29.h");
  COMPARE(fsub(z14.VnS(), z24.VnS(), z9.VnS()), "fsub z14.s, z24.s, z9.s");
  COMPARE(fsub(z14.VnD(), z4.VnD(), z19.VnD()), "fsub z14.d, z4.d, z19.d");
  COMPARE(frecps(z14.VnH(), z29.VnH(), z18.VnH()),
          "frecps z14.h, z29.h, z18.h");
  COMPARE(frecps(z14.VnS(), z29.VnS(), z18.VnS()),
          "frecps z14.s, z29.s, z18.s");
  COMPARE(frecps(z14.VnD(), z29.VnD(), z18.VnD()),
          "frecps z14.d, z29.d, z18.d");
  COMPARE(frsqrts(z5.VnH(), z6.VnH(), z28.VnH()), "frsqrts z5.h, z6.h, z28.h");
  COMPARE(frsqrts(z5.VnS(), z6.VnS(), z28.VnS()), "frsqrts z5.s, z6.s, z28.s");
  COMPARE(frsqrts(z5.VnD(), z6.VnD(), z28.VnD()), "frsqrts z5.d, z6.d, z28.d");
  COMPARE(ftsmul(z21.VnH(), z17.VnH(), z24.VnH()),
          "ftsmul z21.h, z17.h, z24.h");
  COMPARE(ftsmul(z21.VnS(), z17.VnS(), z24.VnS()),
          "ftsmul z21.s, z17.s, z24.s");
  COMPARE(ftsmul(z21.VnD(), z17.VnD(), z24.VnD()),
          "ftsmul z21.d, z17.d, z24.d");

  CLEANUP();
}

TEST(sve_fp_compare_vectors) {
  SETUP();


  COMPARE(facge(p1.VnH(), p3.Zeroing(), z22.VnH(), z25.VnH()),
          "facge p1.h, p3/z, z22.h, z25.h");
  COMPARE(facge(p1.VnS(), p3.Zeroing(), z22.VnS(), z25.VnS()),
          "facge p1.s, p3/z, z22.s, z25.s");
  COMPARE(facge(p1.VnD(), p3.Zeroing(), z22.VnD(), z25.VnD()),
          "facge p1.d, p3/z, z22.d, z25.d");
  COMPARE(facgt(p8.VnH(), p7.Zeroing(), z25.VnH(), z17.VnH()),
          "facgt p8.h, p7/z, z25.h, z17.h");
  COMPARE(facgt(p8.VnS(), p7.Zeroing(), z25.VnS(), z17.VnS()),
          "facgt p8.s, p7/z, z25.s, z17.s");
  COMPARE(facgt(p8.VnD(), p7.Zeroing(), z25.VnD(), z17.VnD()),
          "facgt p8.d, p7/z, z25.d, z17.d");
  COMPARE(fcmeq(p10.VnH(), p2.Zeroing(), z1.VnH(), z17.VnH()),
          "fcmeq p10.h, p2/z, z1.h, z17.h");
  COMPARE(fcmeq(p10.VnS(), p2.Zeroing(), z1.VnS(), z17.VnS()),
          "fcmeq p10.s, p2/z, z1.s, z17.s");
  COMPARE(fcmeq(p10.VnD(), p2.Zeroing(), z1.VnD(), z17.VnD()),
          "fcmeq p10.d, p2/z, z1.d, z17.d");
  COMPARE(fcmge(p0.VnH(), p0.Zeroing(), z1.VnH(), z0.VnH()),
          "fcmge p0.h, p0/z, z1.h, z0.h");
  COMPARE(fcmge(p0.VnS(), p0.Zeroing(), z1.VnS(), z0.VnS()),
          "fcmge p0.s, p0/z, z1.s, z0.s");
  COMPARE(fcmge(p0.VnD(), p0.Zeroing(), z1.VnD(), z0.VnD()),
          "fcmge p0.d, p0/z, z1.d, z0.d");
  COMPARE(fcmgt(p15.VnH(), p5.Zeroing(), z26.VnH(), z5.VnH()),
          "fcmgt p15.h, p5/z, z26.h, z5.h");
  COMPARE(fcmgt(p15.VnS(), p5.Zeroing(), z26.VnS(), z5.VnS()),
          "fcmgt p15.s, p5/z, z26.s, z5.s");
  COMPARE(fcmgt(p15.VnD(), p5.Zeroing(), z26.VnD(), z5.VnD()),
          "fcmgt p15.d, p5/z, z26.d, z5.d");
  COMPARE(fcmne(p2.VnH(), p1.Zeroing(), z9.VnH(), z4.VnH()),
          "fcmne p2.h, p1/z, z9.h, z4.h");
  COMPARE(fcmne(p2.VnS(), p1.Zeroing(), z9.VnS(), z4.VnS()),
          "fcmne p2.s, p1/z, z9.s, z4.s");
  COMPARE(fcmne(p2.VnD(), p1.Zeroing(), z9.VnD(), z4.VnD()),
          "fcmne p2.d, p1/z, z9.d, z4.d");
  COMPARE(fcmuo(p6.VnH(), p4.Zeroing(), z10.VnH(), z21.VnH()),
          "fcmuo p6.h, p4/z, z10.h, z21.h");
  COMPARE(fcmuo(p6.VnS(), p4.Zeroing(), z10.VnS(), z21.VnS()),
          "fcmuo p6.s, p4/z, z10.s, z21.s");
  COMPARE(fcmuo(p6.VnD(), p4.Zeroing(), z10.VnD(), z21.VnD()),
          "fcmuo p6.d, p4/z, z10.d, z21.d");

  COMPARE_MACRO(Facle(p2.VnH(), p0.Zeroing(), z11.VnH(), z15.VnH()),
                "facge p2.h, p0/z, z15.h, z11.h");
  COMPARE_MACRO(Facle(p2.VnS(), p0.Zeroing(), z11.VnS(), z15.VnS()),
                "facge p2.s, p0/z, z15.s, z11.s");
  COMPARE_MACRO(Facle(p2.VnD(), p0.Zeroing(), z11.VnD(), z15.VnD()),
                "facge p2.d, p0/z, z15.d, z11.d");
  COMPARE_MACRO(Faclt(p9.VnH(), p4.Zeroing(), z27.VnH(), z5.VnH()),
                "facgt p9.h, p4/z, z5.h, z27.h");
  COMPARE_MACRO(Faclt(p9.VnS(), p4.Zeroing(), z27.VnS(), z5.VnS()),
                "facgt p9.s, p4/z, z5.s, z27.s");
  COMPARE_MACRO(Faclt(p9.VnD(), p4.Zeroing(), z27.VnD(), z5.VnD()),
                "facgt p9.d, p4/z, z5.d, z27.d");

  COMPARE_MACRO(Fcmle(p12.VnH(), p2.Zeroing(), z21.VnH(), z29.VnH()),
                "fcmge p12.h, p2/z, z29.h, z21.h");
  COMPARE_MACRO(Fcmle(p12.VnS(), p2.Zeroing(), z21.VnS(), z29.VnS()),
                "fcmge p12.s, p2/z, z29.s, z21.s");
  COMPARE_MACRO(Fcmle(p12.VnD(), p2.Zeroing(), z21.VnD(), z29.VnD()),
                "fcmge p12.d, p2/z, z29.d, z21.d");
  COMPARE_MACRO(Fcmlt(p7.VnH(), p3.Zeroing(), z7.VnH(), z14.VnH()),
                "fcmgt p7.h, p3/z, z14.h, z7.h");
  COMPARE_MACRO(Fcmlt(p7.VnS(), p3.Zeroing(), z7.VnS(), z14.VnS()),
                "fcmgt p7.s, p3/z, z14.s, z7.s");
  COMPARE_MACRO(Fcmlt(p7.VnD(), p3.Zeroing(), z7.VnD(), z14.VnD()),
                "fcmgt p7.d, p3/z, z14.d, z7.d");

  CLEANUP();
}

TEST(sve_fp_compare_with_zero) {
  SETUP();

  COMPARE(fcmeq(p9.VnH(), p1.Zeroing(), z17.VnH(), 0),
          "fcmeq p9.h, p1/z, z17.h, #0.0");
  COMPARE(fcmeq(p9.VnS(), p1.Zeroing(), z17.VnS(), 0),
          "fcmeq p9.s, p1/z, z17.s, #0.0");
  COMPARE(fcmeq(p9.VnD(), p1.Zeroing(), z17.VnD(), 0),
          "fcmeq p9.d, p1/z, z17.d, #0.0");
  COMPARE(fcmge(p13.VnH(), p3.Zeroing(), z13.VnH(), 0),
          "fcmge p13.h, p3/z, z13.h, #0.0");
  COMPARE(fcmge(p13.VnS(), p3.Zeroing(), z13.VnS(), 0),
          "fcmge p13.s, p3/z, z13.s, #0.0");
  COMPARE(fcmge(p13.VnD(), p3.Zeroing(), z13.VnD(), 0),
          "fcmge p13.d, p3/z, z13.d, #0.0");
  COMPARE(fcmgt(p10.VnH(), p2.Zeroing(), z24.VnH(), 0),
          "fcmgt p10.h, p2/z, z24.h, #0.0");
  COMPARE(fcmgt(p10.VnS(), p2.Zeroing(), z24.VnS(), 0),
          "fcmgt p10.s, p2/z, z24.s, #0.0");
  COMPARE(fcmgt(p10.VnD(), p2.Zeroing(), z24.VnD(), 0),
          "fcmgt p10.d, p2/z, z24.d, #0.0");
  COMPARE(fcmle(p4.VnH(), p7.Zeroing(), z1.VnH(), 0),
          "fcmle p4.h, p7/z, z1.h, #0.0");
  COMPARE(fcmle(p4.VnS(), p7.Zeroing(), z1.VnS(), 0),
          "fcmle p4.s, p7/z, z1.s, #0.0");
  COMPARE(fcmle(p4.VnD(), p7.Zeroing(), z1.VnD(), 0),
          "fcmle p4.d, p7/z, z1.d, #0.0");
  COMPARE(fcmlt(p15.VnH(), p7.Zeroing(), z9.VnH(), 0),
          "fcmlt p15.h, p7/z, z9.h, #0.0");
  COMPARE(fcmlt(p15.VnS(), p7.Zeroing(), z9.VnS(), 0),
          "fcmlt p15.s, p7/z, z9.s, #0.0");
  COMPARE(fcmlt(p15.VnD(), p7.Zeroing(), z9.VnD(), 0),
          "fcmlt p15.d, p7/z, z9.d, #0.0");
  COMPARE(fcmne(p14.VnH(), p7.Zeroing(), z28.VnH(), 0),
          "fcmne p14.h, p7/z, z28.h, #0.0");
  COMPARE(fcmne(p14.VnS(), p7.Zeroing(), z28.VnS(), 0),
          "fcmne p14.s, p7/z, z28.s, #0.0");
  COMPARE(fcmne(p14.VnD(), p7.Zeroing(), z28.VnD(), 0),
          "fcmne p14.d, p7/z, z28.d, #0.0");

  CLEANUP();
}

TEST(sve_fp_complex_addition) {
  SETUP();

  COMPARE(fcadd(z12.VnH(), p5.Merging(), z12.VnH(), z13.VnH(), 90),
          "fcadd z12.h, p5/m, z12.h, z13.h, #90");
  COMPARE(fcadd(z12.VnS(), p5.Merging(), z12.VnS(), z13.VnS(), 90),
          "fcadd z12.s, p5/m, z12.s, z13.s, #90");
  COMPARE(fcadd(z12.VnD(), p5.Merging(), z12.VnD(), z13.VnD(), 90),
          "fcadd z12.d, p5/m, z12.d, z13.d, #90");
  COMPARE(fcadd(z22.VnH(), p0.Merging(), z22.VnH(), z23.VnH(), 270),
          "fcadd z22.h, p0/m, z22.h, z23.h, #270");
  COMPARE(fcadd(z22.VnS(), p0.Merging(), z22.VnS(), z23.VnS(), 270),
          "fcadd z22.s, p0/m, z22.s, z23.s, #270");
  COMPARE(fcadd(z22.VnD(), p0.Merging(), z22.VnD(), z23.VnD(), 270),
          "fcadd z22.d, p0/m, z22.d, z23.d, #270");

  COMPARE_MACRO(Fcadd(z12.VnH(), p5.Merging(), z1.VnH(), z13.VnH(), 90),
                "movprfx z12.h, p5/m, z1.h\n"
                "fcadd z12.h, p5/m, z12.h, z13.h, #90");
  COMPARE_MACRO(Fcadd(z12.VnH(), p5.Merging(), z1.VnH(), z12.VnH(), 90),
                "movprfx z31.h, p5/m, z1.h\n"
                "fcadd z31.h, p5/m, z31.h, z12.h, #90\n"
                "mov z12.d, z31.d");
  CLEANUP();
}

TEST(sve_fp_complex_mul_add) {
  SETUP();

  COMPARE_MACRO(Fcmla(z19.VnH(),
                      p7.Merging(),
                      z19.VnH(),
                      z16.VnH(),
                      z0.VnH(),
                      90),
                "fcmla z19.h, p7/m, z16.h, z0.h, #90");
  COMPARE_MACRO(Fcmla(z19.VnS(),
                      p7.Merging(),
                      z19.VnS(),
                      z16.VnS(),
                      z0.VnS(),
                      90),
                "fcmla z19.s, p7/m, z16.s, z0.s, #90");
  COMPARE_MACRO(Fcmla(z19.VnD(),
                      p7.Merging(),
                      z19.VnD(),
                      z16.VnD(),
                      z0.VnD(),
                      90),
                "fcmla z19.d, p7/m, z16.d, z0.d, #90");

  COMPARE_MACRO(Fcmla(z20.VnD(),
                      p6.Merging(),
                      z20.VnD(),
                      z15.VnD(),
                      z1.VnD(),
                      0),
                "fcmla z20.d, p6/m, z15.d, z1.d, #0");
  COMPARE_MACRO(Fcmla(z20.VnD(),
                      p6.Merging(),
                      z20.VnD(),
                      z15.VnD(),
                      z1.VnD(),
                      180),
                "fcmla z20.d, p6/m, z15.d, z1.d, #180");
  COMPARE_MACRO(Fcmla(z20.VnD(),
                      p6.Merging(),
                      z20.VnD(),
                      z15.VnD(),
                      z1.VnD(),
                      270),
                "fcmla z20.d, p6/m, z15.d, z1.d, #270");

  COMPARE_MACRO(Fcmla(z20.VnD(),
                      p6.Merging(),
                      z20.VnD(),
                      z15.VnD(),
                      z20.VnD(),
                      270),
                "fcmla z20.d, p6/m, z15.d, z20.d, #270");
  COMPARE_MACRO(Fcmla(z20.VnD(),
                      p6.Merging(),
                      z21.VnD(),
                      z15.VnD(),
                      z1.VnD(),
                      270),
                "movprfx z20.d, p6/m, z21.d\n"
                "fcmla z20.d, p6/m, z15.d, z1.d, #270");
  COMPARE_MACRO(Fcmla(z20.VnD(),
                      p6.Merging(),
                      z21.VnD(),
                      z20.VnD(),
                      z1.VnD(),
                      270),
                "movprfx z31, z21\n"
                "fcmla z31.d, p6/m, z20.d, z1.d, #270\n"
                "mov z20.d, p6/m, z31.d");
  COMPARE_MACRO(Fcmla(z20.VnD(),
                      p6.Merging(),
                      z21.VnD(),
                      z15.VnD(),
                      z20.VnD(),
                      270),
                "movprfx z31, z21\n"
                "fcmla z31.d, p6/m, z15.d, z20.d, #270\n"
                "mov z20.d, p6/m, z31.d");
  COMPARE_MACRO(Fcmla(z20.VnD(),
                      p6.Merging(),
                      z21.VnD(),
                      z20.VnD(),
                      z20.VnD(),
                      270),
                "movprfx z31, z21\n"
                "fcmla z31.d, p6/m, z20.d, z20.d, #270\n"
                "mov z20.d, p6/m, z31.d");

  CLEANUP();
}

TEST(sve_fp_complex_mul_add_index) {
  SETUP();

  COMPARE(fcmla(z30.VnH(), z20.VnH(), z3.VnH(), 0, 0),
          "fcmla z30.h, z20.h, z3.h[0], #0");
  COMPARE(fcmla(z30.VnH(), z20.VnH(), z3.VnH(), 1, 0),
          "fcmla z30.h, z20.h, z3.h[1], #0");
  COMPARE(fcmla(z30.VnH(), z20.VnH(), z3.VnH(), 2, 90),
          "fcmla z30.h, z20.h, z3.h[2], #90");
  COMPARE(fcmla(z30.VnH(), z20.VnH(), z3.VnH(), 0, 270),
          "fcmla z30.h, z20.h, z3.h[0], #270");
  COMPARE(fcmla(z10.VnS(), z20.VnS(), z1.VnS(), 0, 0),
          "fcmla z10.s, z20.s, z1.s[0], #0");
  COMPARE(fcmla(z10.VnS(), z20.VnS(), z1.VnS(), 1, 0),
          "fcmla z10.s, z20.s, z1.s[1], #0");
  COMPARE(fcmla(z10.VnS(), z20.VnS(), z1.VnS(), 1, 90),
          "fcmla z10.s, z20.s, z1.s[1], #90");
  COMPARE(fcmla(z10.VnS(), z20.VnS(), z1.VnS(), 0, 270),
          "fcmla z10.s, z20.s, z1.s[0], #270");

  CLEANUP();
}

TEST(sve_fp_fast_reduction) {
  SETUP();

  COMPARE(faddv(h26, p6, z16.VnH()), "faddv h26, p6, z16.h");
  COMPARE(faddv(s26, p6, z16.VnS()), "faddv s26, p6, z16.s");
  COMPARE(faddv(d26, p6, z16.VnD()), "faddv d26, p6, z16.d");
  COMPARE(fmaxnmv(h28, p1, z0.VnH()), "fmaxnmv h28, p1, z0.h");
  COMPARE(fmaxnmv(s28, p1, z0.VnS()), "fmaxnmv s28, p1, z0.s");
  COMPARE(fmaxnmv(d28, p1, z0.VnD()), "fmaxnmv d28, p1, z0.d");
  COMPARE(fmaxv(h3, p1, z23.VnH()), "fmaxv h3, p1, z23.h");
  COMPARE(fmaxv(s3, p1, z23.VnS()), "fmaxv s3, p1, z23.s");
  COMPARE(fmaxv(d3, p1, z23.VnD()), "fmaxv d3, p1, z23.d");
  COMPARE(fminnmv(h20, p6, z21.VnH()), "fminnmv h20, p6, z21.h");
  COMPARE(fminnmv(s20, p6, z21.VnS()), "fminnmv s20, p6, z21.s");
  COMPARE(fminnmv(d20, p6, z21.VnD()), "fminnmv d20, p6, z21.d");
  COMPARE(fminv(h10, p4, z27.VnH()), "fminv h10, p4, z27.h");
  COMPARE(fminv(s10, p4, z27.VnS()), "fminv s10, p4, z27.s");
  COMPARE(fminv(d10, p4, z27.VnD()), "fminv d10, p4, z27.d");

  CLEANUP();
}

TEST(sve_fp_mul_add) {
  SETUP();

  COMPARE(fmad(z31.VnH(), p2.Merging(), z8.VnH(), z1.VnH()),
          "fmad z31.h, p2/m, z8.h, z1.h");
  COMPARE(fmad(z31.VnS(), p2.Merging(), z8.VnS(), z1.VnS()),
          "fmad z31.s, p2/m, z8.s, z1.s");
  COMPARE(fmad(z31.VnD(), p2.Merging(), z8.VnD(), z1.VnD()),
          "fmad z31.d, p2/m, z8.d, z1.d");
  COMPARE(fmla(z26.VnH(), p7.Merging(), z19.VnH(), z16.VnH()),
          "fmla z26.h, p7/m, z19.h, z16.h");
  COMPARE(fmla(z26.VnS(), p7.Merging(), z19.VnS(), z16.VnS()),
          "fmla z26.s, p7/m, z19.s, z16.s");
  COMPARE(fmla(z26.VnD(), p7.Merging(), z19.VnD(), z16.VnD()),
          "fmla z26.d, p7/m, z19.d, z16.d");
  COMPARE(fmls(z20.VnH(), p6.Merging(), z28.VnH(), z0.VnH()),
          "fmls z20.h, p6/m, z28.h, z0.h");
  COMPARE(fmls(z20.VnS(), p6.Merging(), z28.VnS(), z0.VnS()),
          "fmls z20.s, p6/m, z28.s, z0.s");
  COMPARE(fmls(z20.VnD(), p6.Merging(), z28.VnD(), z0.VnD()),
          "fmls z20.d, p6/m, z28.d, z0.d");
  COMPARE(fmsb(z3.VnH(), p4.Merging(), z8.VnH(), z22.VnH()),
          "fmsb z3.h, p4/m, z8.h, z22.h");
  COMPARE(fmsb(z3.VnS(), p4.Merging(), z8.VnS(), z22.VnS()),
          "fmsb z3.s, p4/m, z8.s, z22.s");
  COMPARE(fmsb(z3.VnD(), p4.Merging(), z8.VnD(), z22.VnD()),
          "fmsb z3.d, p4/m, z8.d, z22.d");
  COMPARE(fnmad(z0.VnH(), p5.Merging(), z20.VnH(), z17.VnH()),
          "fnmad z0.h, p5/m, z20.h, z17.h");
  COMPARE(fnmad(z0.VnS(), p5.Merging(), z20.VnS(), z17.VnS()),
          "fnmad z0.s, p5/m, z20.s, z17.s");
  COMPARE(fnmad(z0.VnD(), p5.Merging(), z20.VnD(), z17.VnD()),
          "fnmad z0.d, p5/m, z20.d, z17.d");
  COMPARE(fnmla(z31.VnH(), p6.Merging(), z14.VnH(), z8.VnH()),
          "fnmla z31.h, p6/m, z14.h, z8.h");
  COMPARE(fnmla(z31.VnS(), p6.Merging(), z14.VnS(), z8.VnS()),
          "fnmla z31.s, p6/m, z14.s, z8.s");
  COMPARE(fnmla(z31.VnD(), p6.Merging(), z14.VnD(), z8.VnD()),
          "fnmla z31.d, p6/m, z14.d, z8.d");
  COMPARE(fnmls(z2.VnH(), p1.Merging(), z23.VnH(), z15.VnH()),
          "fnmls z2.h, p1/m, z23.h, z15.h");
  COMPARE(fnmls(z2.VnS(), p1.Merging(), z23.VnS(), z15.VnS()),
          "fnmls z2.s, p1/m, z23.s, z15.s");
  COMPARE(fnmls(z2.VnD(), p1.Merging(), z23.VnD(), z15.VnD()),
          "fnmls z2.d, p1/m, z23.d, z15.d");
  COMPARE(fnmsb(z28.VnH(), p3.Merging(), z26.VnH(), z11.VnH()),
          "fnmsb z28.h, p3/m, z26.h, z11.h");
  COMPARE(fnmsb(z28.VnS(), p3.Merging(), z26.VnS(), z11.VnS()),
          "fnmsb z28.s, p3/m, z26.s, z11.s");
  COMPARE(fnmsb(z28.VnD(), p3.Merging(), z26.VnD(), z11.VnD()),
          "fnmsb z28.d, p3/m, z26.d, z11.d");

  CLEANUP();
}

TEST(sve_fp_mul_add_macro_strict_nan_propagation) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

  // Shadow the `MacroAssembler` type so that the test macros work without
  // modification.
  typedef StrictNaNPropagationMacroAssembler MacroAssembler;

  SETUP();

  COMPARE_MACRO(Fmla(z0.VnH(), p1.Merging(), z0.VnH(), z2.VnH(), z4.VnH()),
                "fmla z0.h, p1/m, z2.h, z4.h");
  COMPARE_MACRO(Fmla(z3.VnH(), p2.Merging(), z4.VnH(), z3.VnH(), z5.VnH()),
                "fmad z3.h, p2/m, z5.h, z4.h");
  COMPARE_MACRO(Fmla(z4.VnS(), p3.Merging(), z5.VnS(), z6.VnS(), z4.VnS()),
                "movprfx z31.s, p3/m, z5.s\n"
                "fmla z31.s, p3/m, z6.s, z4.s\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Fmla(z5.VnD(), p4.Merging(), z6.VnD(), z7.VnD(), z8.VnD()),
                "movprfx z5.d, p4/m, z6.d\n"
                "fmla z5.d, p4/m, z7.d, z8.d");

  COMPARE_MACRO(Fmls(z0.VnD(), p1.Merging(), z0.VnD(), z2.VnD(), z4.VnD()),
                "fmls z0.d, p1/m, z2.d, z4.d");
  COMPARE_MACRO(Fmls(z3.VnS(), p2.Merging(), z4.VnS(), z3.VnS(), z5.VnS()),
                "fmsb z3.s, p2/m, z5.s, z4.s");
  COMPARE_MACRO(Fmls(z4.VnH(), p3.Merging(), z5.VnH(), z6.VnH(), z4.VnH()),
                "movprfx z31.h, p3/m, z5.h\n"
                "fmls z31.h, p3/m, z6.h, z4.h\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Fmls(z5.VnD(), p4.Merging(), z6.VnD(), z7.VnD(), z8.VnD()),
                "movprfx z5.d, p4/m, z6.d\n"
                "fmls z5.d, p4/m, z7.d, z8.d");

  COMPARE_MACRO(Fnmla(z10.VnH(), p5.Merging(), z10.VnH(), z12.VnH(), z14.VnH()),
                "fnmla z10.h, p5/m, z12.h, z14.h");
  COMPARE_MACRO(Fnmla(z13.VnH(), p6.Merging(), z14.VnH(), z13.VnH(), z15.VnH()),
                "fnmad z13.h, p6/m, z15.h, z14.h");
  COMPARE_MACRO(Fnmla(z14.VnS(), p7.Merging(), z15.VnS(), z16.VnS(), z14.VnS()),
                "movprfx z31.s, p7/m, z15.s\n"
                "fnmla z31.s, p7/m, z16.s, z14.s\n"
                "mov z14.d, z31.d");
  COMPARE_MACRO(Fnmla(z15.VnD(), p0.Merging(), z16.VnD(), z17.VnD(), z18.VnD()),
                "movprfx z15.d, p0/m, z16.d\n"
                "fnmla z15.d, p0/m, z17.d, z18.d");

  COMPARE_MACRO(Fnmls(z10.VnD(), p5.Merging(), z10.VnD(), z12.VnD(), z14.VnD()),
                "fnmls z10.d, p5/m, z12.d, z14.d");
  COMPARE_MACRO(Fnmls(z13.VnS(), p6.Merging(), z14.VnS(), z13.VnS(), z15.VnS()),
                "fnmsb z13.s, p6/m, z15.s, z14.s");
  COMPARE_MACRO(Fnmls(z14.VnH(), p7.Merging(), z15.VnH(), z16.VnH(), z14.VnH()),
                "movprfx z31.h, p7/m, z15.h\n"
                "fnmls z31.h, p7/m, z16.h, z14.h\n"
                "mov z14.d, z31.d");
  COMPARE_MACRO(Fnmls(z15.VnD(), p0.Merging(), z16.VnD(), z17.VnD(), z18.VnD()),
                "movprfx z15.d, p0/m, z16.d\n"
                "fnmls z15.d, p0/m, z17.d, z18.d");

  CLEANUP();

#pragma GCC diagnostic pop
}

TEST(sve_fp_mul_add_macro_fast_nan_propagation) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

  // Shadow the `MacroAssembler` type so that the test macros work without
  // modification.
  typedef FastNaNPropagationMacroAssembler MacroAssembler;

  SETUP();

  COMPARE_MACRO(Fmla(z0.VnH(), p1.Merging(), z0.VnH(), z2.VnH(), z4.VnH()),
                "fmla z0.h, p1/m, z2.h, z4.h");
  COMPARE_MACRO(Fmla(z3.VnH(), p2.Merging(), z4.VnH(), z3.VnH(), z5.VnH()),
                "fmad z3.h, p2/m, z5.h, z4.h");
  COMPARE_MACRO(Fmla(z4.VnS(), p3.Merging(), z5.VnS(), z6.VnS(), z4.VnS()),
                "fmad z4.s, p3/m, z6.s, z5.s");
  COMPARE_MACRO(Fmla(z5.VnD(), p4.Merging(), z6.VnD(), z7.VnD(), z8.VnD()),
                "movprfx z5.d, p4/m, z6.d\n"
                "fmla z5.d, p4/m, z7.d, z8.d");

  COMPARE_MACRO(Fmls(z0.VnD(), p1.Merging(), z0.VnD(), z2.VnD(), z4.VnD()),
                "fmls z0.d, p1/m, z2.d, z4.d");
  COMPARE_MACRO(Fmls(z3.VnS(), p2.Merging(), z4.VnS(), z3.VnS(), z5.VnS()),
                "fmsb z3.s, p2/m, z5.s, z4.s");
  COMPARE_MACRO(Fmls(z4.VnH(), p3.Merging(), z5.VnH(), z6.VnH(), z4.VnH()),
                "fmsb z4.h, p3/m, z6.h, z5.h");
  COMPARE_MACRO(Fmls(z5.VnD(), p4.Merging(), z6.VnD(), z7.VnD(), z8.VnD()),
                "movprfx z5.d, p4/m, z6.d\n"
                "fmls z5.d, p4/m, z7.d, z8.d");

  COMPARE_MACRO(Fnmla(z10.VnH(), p5.Merging(), z10.VnH(), z12.VnH(), z14.VnH()),
                "fnmla z10.h, p5/m, z12.h, z14.h");
  COMPARE_MACRO(Fnmla(z13.VnH(), p6.Merging(), z14.VnH(), z13.VnH(), z15.VnH()),
                "fnmad z13.h, p6/m, z15.h, z14.h");
  COMPARE_MACRO(Fnmla(z14.VnS(), p7.Merging(), z15.VnS(), z16.VnS(), z14.VnS()),
                "fnmad z14.s, p7/m, z16.s, z15.s");
  COMPARE_MACRO(Fnmla(z15.VnD(), p0.Merging(), z16.VnD(), z17.VnD(), z18.VnD()),
                "movprfx z15.d, p0/m, z16.d\n"
                "fnmla z15.d, p0/m, z17.d, z18.d");

  COMPARE_MACRO(Fnmls(z10.VnD(), p5.Merging(), z10.VnD(), z12.VnD(), z14.VnD()),
                "fnmls z10.d, p5/m, z12.d, z14.d");
  COMPARE_MACRO(Fnmls(z13.VnS(), p6.Merging(), z14.VnS(), z13.VnS(), z15.VnS()),
                "fnmsb z13.s, p6/m, z15.s, z14.s");
  COMPARE_MACRO(Fnmls(z14.VnH(), p7.Merging(), z15.VnH(), z16.VnH(), z14.VnH()),
                "fnmsb z14.h, p7/m, z16.h, z15.h");
  COMPARE_MACRO(Fnmls(z15.VnD(), p0.Merging(), z16.VnD(), z17.VnD(), z18.VnD()),
                "movprfx z15.d, p0/m, z16.d\n"
                "fnmls z15.d, p0/m, z17.d, z18.d");

  CLEANUP();

#pragma GCC diagnostic pop
}

TEST(sve_fp_mul_add_index) {
  SETUP();

  COMPARE(fmla(z25.VnD(), z9.VnD(), z1.VnD(), 0), "fmla z25.d, z9.d, z1.d[0]");
  COMPARE(fmla(z25.VnD(), z9.VnD(), z1.VnD(), 1), "fmla z25.d, z9.d, z1.d[1]");

  COMPARE(fmla(z13.VnH(), z7.VnH(), z7.VnH(), 0), "fmla z13.h, z7.h, z7.h[0]");
  COMPARE(fmla(z13.VnH(), z7.VnH(), z7.VnH(), 2), "fmla z13.h, z7.h, z7.h[2]");
  COMPARE(fmla(z13.VnH(), z7.VnH(), z7.VnH(), 5), "fmla z13.h, z7.h, z7.h[5]");
  COMPARE(fmla(z13.VnH(), z7.VnH(), z7.VnH(), 7), "fmla z13.h, z7.h, z7.h[7]");

  COMPARE(fmla(z17.VnS(), z27.VnS(), z2.VnS(), 0),
          "fmla z17.s, z27.s, z2.s[0]");
  COMPARE(fmla(z17.VnS(), z27.VnS(), z2.VnS(), 1),
          "fmla z17.s, z27.s, z2.s[1]");
  COMPARE(fmla(z17.VnS(), z27.VnS(), z2.VnS(), 2),
          "fmla z17.s, z27.s, z2.s[2]");
  COMPARE(fmla(z17.VnS(), z27.VnS(), z2.VnS(), 3),
          "fmla z17.s, z27.s, z2.s[3]");

  COMPARE(fmls(z28.VnD(), z2.VnD(), z0.VnD(), 0), "fmls z28.d, z2.d, z0.d[0]");
  COMPARE(fmls(z28.VnD(), z2.VnD(), z0.VnD(), 1), "fmls z28.d, z2.d, z0.d[1]");

  COMPARE(fmls(z30.VnH(), z29.VnH(), z7.VnH(), 1),
          "fmls z30.h, z29.h, z7.h[1]");
  COMPARE(fmls(z30.VnH(), z29.VnH(), z7.VnH(), 4),
          "fmls z30.h, z29.h, z7.h[4]");
  COMPARE(fmls(z30.VnH(), z29.VnH(), z7.VnH(), 3),
          "fmls z30.h, z29.h, z7.h[3]");
  COMPARE(fmls(z30.VnH(), z29.VnH(), z7.VnH(), 6),
          "fmls z30.h, z29.h, z7.h[6]");

  COMPARE(fmls(z30.VnS(), z1.VnS(), z6.VnS(), 0), "fmls z30.s, z1.s, z6.s[0]");
  COMPARE(fmls(z30.VnS(), z1.VnS(), z6.VnS(), 1), "fmls z30.s, z1.s, z6.s[1]");
  COMPARE(fmls(z30.VnS(), z1.VnS(), z6.VnS(), 2), "fmls z30.s, z1.s, z6.s[2]");
  COMPARE(fmls(z30.VnS(), z1.VnS(), z6.VnS(), 3), "fmls z30.s, z1.s, z6.s[3]");

  COMPARE_MACRO(Fmla(z10.VnH(), z11.VnH(), z12.VnH(), z4.VnH(), 7),
                "movprfx z10, z11\n"
                "fmla z10.h, z12.h, z4.h[7]");
  COMPARE_MACRO(Fmla(z10.VnH(), z10.VnH(), z12.VnH(), z4.VnH(), 6),
                "fmla z10.h, z12.h, z4.h[6]");
  COMPARE_MACRO(Fmla(z11.VnS(), z12.VnS(), z11.VnS(), z5.VnS(), 3),
                "movprfx z31, z12\n"
                "fmla z31.s, z11.s, z5.s[3]\n"
                "mov z11.d, z31.d");
  COMPARE_MACRO(Fmla(z12.VnD(), z13.VnD(), z14.VnD(), z12.VnD(), 1),
                "movprfx z31, z13\n"
                "fmla z31.d, z14.d, z12.d[1]\n"
                "mov z12.d, z31.d");

  COMPARE_MACRO(Fmls(z10.VnH(), z11.VnH(), z12.VnH(), z4.VnH(), 7),
                "movprfx z10, z11\n"
                "fmls z10.h, z12.h, z4.h[7]");
  COMPARE_MACRO(Fmls(z10.VnH(), z10.VnH(), z12.VnH(), z4.VnH(), 6),
                "fmls z10.h, z12.h, z4.h[6]");
  COMPARE_MACRO(Fmls(z11.VnS(), z12.VnS(), z11.VnS(), z5.VnS(), 3),
                "movprfx z31, z12\n"
                "fmls z31.s, z11.s, z5.s[3]\n"
                "mov z11.d, z31.d");
  COMPARE_MACRO(Fmls(z12.VnD(), z13.VnD(), z14.VnD(), z12.VnD(), 1),
                "movprfx z31, z13\n"
                "fmls z31.d, z14.d, z12.d[1]\n"
                "mov z12.d, z31.d");

  CLEANUP();
}

TEST(sve_fp_mul_index) {
  SETUP();

  COMPARE(fmul(z12.VnD(), z3.VnD(), z4.VnD(), 0), "fmul z12.d, z3.d, z4.d[0]");
  COMPARE(fmul(z12.VnD(), z3.VnD(), z4.VnD(), 1), "fmul z12.d, z3.d, z4.d[1]");

  COMPARE(fmul(z22.VnH(), z2.VnH(), z3.VnH(), 0), "fmul z22.h, z2.h, z3.h[0]");
  COMPARE(fmul(z22.VnH(), z2.VnH(), z3.VnH(), 3), "fmul z22.h, z2.h, z3.h[3]");
  COMPARE(fmul(z22.VnH(), z2.VnH(), z3.VnH(), 4), "fmul z22.h, z2.h, z3.h[4]");
  COMPARE(fmul(z22.VnH(), z2.VnH(), z3.VnH(), 7), "fmul z22.h, z2.h, z3.h[7]");

  COMPARE(fmul(z2.VnS(), z8.VnS(), z7.VnS(), 0), "fmul z2.s, z8.s, z7.s[0]");
  COMPARE(fmul(z2.VnS(), z8.VnS(), z7.VnS(), 1), "fmul z2.s, z8.s, z7.s[1]");
  COMPARE(fmul(z2.VnS(), z8.VnS(), z7.VnS(), 2), "fmul z2.s, z8.s, z7.s[2]");
  COMPARE(fmul(z2.VnS(), z8.VnS(), z7.VnS(), 3), "fmul z2.s, z8.s, z7.s[3]");

  CLEANUP();
}

TEST(sve_fp_unary_op_predicated) {
  SETUP();

  COMPARE(fcvtzs(z29.VnS(), p5.Merging(), z8.VnD()),
          "fcvtzs z29.s, p5/m, z8.d");
  COMPARE(fcvtzs(z30.VnD(), p5.Merging(), z8.VnD()),
          "fcvtzs z30.d, p5/m, z8.d");
  COMPARE(fcvtzs(z14.VnH(), p1.Merging(), z29.VnH()),
          "fcvtzs z14.h, p1/m, z29.h");
  COMPARE(fcvtzs(z11.VnS(), p3.Merging(), z16.VnH()),
          "fcvtzs z11.s, p3/m, z16.h");
  COMPARE(fcvtzs(z4.VnD(), p7.Merging(), z4.VnH()), "fcvtzs z4.d, p7/m, z4.h");
  COMPARE(fcvtzs(z24.VnS(), p1.Merging(), z4.VnS()),
          "fcvtzs z24.s, p1/m, z4.s");
  COMPARE(fcvtzs(z25.VnD(), p4.Merging(), z24.VnS()),
          "fcvtzs z25.d, p4/m, z24.s");
  COMPARE(fcvtzu(z16.VnS(), p7.Merging(), z14.VnD()),
          "fcvtzu z16.s, p7/m, z14.d");
  COMPARE(fcvtzu(z31.VnD(), p1.Merging(), z16.VnD()),
          "fcvtzu z31.d, p1/m, z16.d");
  COMPARE(fcvtzu(z12.VnH(), p2.Merging(), z27.VnH()),
          "fcvtzu z12.h, p2/m, z27.h");
  COMPARE(fcvtzu(z26.VnS(), p6.Merging(), z29.VnH()),
          "fcvtzu z26.s, p6/m, z29.h");
  COMPARE(fcvtzu(z29.VnD(), p5.Merging(), z27.VnH()),
          "fcvtzu z29.d, p5/m, z27.h");
  COMPARE(fcvtzu(z13.VnS(), p2.Merging(), z17.VnS()),
          "fcvtzu z13.s, p2/m, z17.s");
  COMPARE(fcvtzu(z25.VnD(), p7.Merging(), z28.VnS()),
          "fcvtzu z25.d, p7/m, z28.s");
  COMPARE(scvtf(z16.VnH(), p6.Merging(), z5.VnH()), "scvtf z16.h, p6/m, z5.h");
  COMPARE(scvtf(z31.VnD(), p5.Merging(), z26.VnS()),
          "scvtf z31.d, p5/m, z26.s");
  COMPARE(scvtf(z0.VnH(), p7.Merging(), z0.VnS()), "scvtf z0.h, p7/m, z0.s");
  COMPARE(scvtf(z12.VnS(), p7.Merging(), z0.VnS()), "scvtf z12.s, p7/m, z0.s");
  COMPARE(scvtf(z17.VnD(), p1.Merging(), z17.VnD()),
          "scvtf z17.d, p1/m, z17.d");
  COMPARE(scvtf(z2.VnH(), p0.Merging(), z9.VnD()), "scvtf z2.h, p0/m, z9.d");
  COMPARE(scvtf(z26.VnS(), p5.Merging(), z4.VnD()), "scvtf z26.s, p5/m, z4.d");
  COMPARE(ucvtf(z27.VnH(), p4.Merging(), z25.VnH()),
          "ucvtf z27.h, p4/m, z25.h");
  COMPARE(ucvtf(z3.VnD(), p4.Merging(), z3.VnS()), "ucvtf z3.d, p4/m, z3.s");
  COMPARE(ucvtf(z24.VnH(), p2.Merging(), z29.VnS()),
          "ucvtf z24.h, p2/m, z29.s");
  COMPARE(ucvtf(z29.VnS(), p5.Merging(), z14.VnS()),
          "ucvtf z29.s, p5/m, z14.s");
  COMPARE(ucvtf(z7.VnD(), p2.Merging(), z14.VnD()), "ucvtf z7.d, p2/m, z14.d");
  COMPARE(ucvtf(z20.VnH(), p2.Merging(), z14.VnD()),
          "ucvtf z20.h, p2/m, z14.d");
  COMPARE(ucvtf(z26.VnS(), p1.Merging(), z18.VnD()),
          "ucvtf z26.s, p1/m, z18.d");
  COMPARE(frinta(z11.VnH(), p0.Merging(), z3.VnH()),
          "frinta z11.h, p0/m, z3.h");
  COMPARE(frinta(z11.VnS(), p0.Merging(), z3.VnS()),
          "frinta z11.s, p0/m, z3.s");
  COMPARE(frinta(z11.VnD(), p0.Merging(), z3.VnD()),
          "frinta z11.d, p0/m, z3.d");
  COMPARE(frinti(z17.VnH(), p0.Merging(), z16.VnH()),
          "frinti z17.h, p0/m, z16.h");
  COMPARE(frinti(z17.VnS(), p0.Merging(), z16.VnS()),
          "frinti z17.s, p0/m, z16.s");
  COMPARE(frinti(z17.VnD(), p0.Merging(), z16.VnD()),
          "frinti z17.d, p0/m, z16.d");
  COMPARE(frintm(z2.VnH(), p7.Merging(), z15.VnH()),
          "frintm z2.h, p7/m, z15.h");
  COMPARE(frintm(z2.VnS(), p7.Merging(), z15.VnS()),
          "frintm z2.s, p7/m, z15.s");
  COMPARE(frintm(z2.VnD(), p7.Merging(), z15.VnD()),
          "frintm z2.d, p7/m, z15.d");
  COMPARE(frintn(z14.VnH(), p5.Merging(), z18.VnH()),
          "frintn z14.h, p5/m, z18.h");
  COMPARE(frintn(z14.VnS(), p5.Merging(), z18.VnS()),
          "frintn z14.s, p5/m, z18.s");
  COMPARE(frintn(z14.VnD(), p5.Merging(), z18.VnD()),
          "frintn z14.d, p5/m, z18.d");
  COMPARE(frintp(z20.VnH(), p6.Merging(), z23.VnH()),
          "frintp z20.h, p6/m, z23.h");
  COMPARE(frintp(z20.VnS(), p6.Merging(), z23.VnS()),
          "frintp z20.s, p6/m, z23.s");
  COMPARE(frintp(z20.VnD(), p6.Merging(), z23.VnD()),
          "frintp z20.d, p6/m, z23.d");
  COMPARE(frintx(z2.VnH(), p6.Merging(), z18.VnH()),
          "frintx z2.h, p6/m, z18.h");
  COMPARE(frintx(z2.VnS(), p6.Merging(), z18.VnS()),
          "frintx z2.s, p6/m, z18.s");
  COMPARE(frintx(z2.VnD(), p6.Merging(), z18.VnD()),
          "frintx z2.d, p6/m, z18.d");
  COMPARE(frintz(z26.VnH(), p7.Merging(), z25.VnH()),
          "frintz z26.h, p7/m, z25.h");
  COMPARE(frintz(z26.VnS(), p7.Merging(), z25.VnS()),
          "frintz z26.s, p7/m, z25.s");
  COMPARE(frintz(z26.VnD(), p7.Merging(), z25.VnD()),
          "frintz z26.d, p7/m, z25.d");
  COMPARE(fcvt(z5.VnH(), p2.Merging(), z11.VnD()), "fcvt z5.h, p2/m, z11.d");
  COMPARE(fcvt(z30.VnS(), p7.Merging(), z0.VnD()), "fcvt z30.s, p7/m, z0.d");
  COMPARE(fcvt(z10.VnD(), p0.Merging(), z17.VnH()), "fcvt z10.d, p0/m, z17.h");
  COMPARE(fcvt(z28.VnS(), p3.Merging(), z27.VnH()), "fcvt z28.s, p3/m, z27.h");
  COMPARE(fcvt(z9.VnD(), p7.Merging(), z0.VnS()), "fcvt z9.d, p7/m, z0.s");
  COMPARE(fcvt(z27.VnH(), p7.Merging(), z9.VnS()), "fcvt z27.h, p7/m, z9.s");
  COMPARE(frecpx(z16.VnH(), p1.Merging(), z29.VnH()),
          "frecpx z16.h, p1/m, z29.h");
  COMPARE(frecpx(z16.VnS(), p1.Merging(), z29.VnS()),
          "frecpx z16.s, p1/m, z29.s");
  COMPARE(frecpx(z16.VnD(), p1.Merging(), z29.VnD()),
          "frecpx z16.d, p1/m, z29.d");
  COMPARE(fsqrt(z30.VnH(), p3.Merging(), z13.VnH()),
          "fsqrt z30.h, p3/m, z13.h");
  COMPARE(fsqrt(z30.VnS(), p3.Merging(), z13.VnS()),
          "fsqrt z30.s, p3/m, z13.s");
  COMPARE(fsqrt(z30.VnD(), p3.Merging(), z13.VnD()),
          "fsqrt z30.d, p3/m, z13.d");

  CLEANUP();
}

TEST(sve_fp_unary_op_predicated_macro) {
  SETUP();

  COMPARE_MACRO(Fcvt(z5.VnH(), p2.Zeroing(), z11.VnD()),
                "movprfx z5.d, p2/z, z11.d\n"
                "fcvt z5.h, p2/m, z11.d");
  COMPARE_MACRO(Fcvt(z30.VnS(), p7.Zeroing(), z0.VnD()),
                "movprfx z30.d, p7/z, z0.d\n"
                "fcvt z30.s, p7/m, z0.d");
  COMPARE_MACRO(Fcvt(z10.VnD(), p0.Zeroing(), z17.VnH()),
                "movprfx z10.d, p0/z, z17.d\n"
                "fcvt z10.d, p0/m, z17.h");
  COMPARE_MACRO(Fcvt(z28.VnS(), p3.Zeroing(), z27.VnH()),
                "movprfx z28.s, p3/z, z27.s\n"
                "fcvt z28.s, p3/m, z27.h");
  COMPARE_MACRO(Fcvt(z9.VnD(), p7.Zeroing(), z0.VnS()),
                "movprfx z9.d, p7/z, z0.d\n"
                "fcvt z9.d, p7/m, z0.s");
  COMPARE_MACRO(Fcvt(z27.VnH(), p7.Zeroing(), z9.VnS()),
                "movprfx z27.s, p7/z, z9.s\n"
                "fcvt z27.h, p7/m, z9.s");
  COMPARE_MACRO(Frecpx(z16.VnH(), p1.Zeroing(), z29.VnH()),
                "movprfx z16.h, p1/z, z29.h\n"
                "frecpx z16.h, p1/m, z29.h");
  COMPARE_MACRO(Frecpx(z17.VnS(), p2.Zeroing(), z30.VnS()),
                "movprfx z17.s, p2/z, z30.s\n"
                "frecpx z17.s, p2/m, z30.s");
  COMPARE_MACRO(Frecpx(z18.VnD(), p3.Zeroing(), z31.VnD()),
                "movprfx z18.d, p3/z, z31.d\n"
                "frecpx z18.d, p3/m, z31.d");
  COMPARE_MACRO(Frinta(z6.VnD(), p3.Zeroing(), z12.VnD()),
                "movprfx z6.d, p3/z, z12.d\n"
                "frinta z6.d, p3/m, z12.d");
  COMPARE_MACRO(Frinti(z7.VnS(), p3.Zeroing(), z11.VnS()),
                "movprfx z7.s, p3/z, z11.s\n"
                "frinti z7.s, p3/m, z11.s");
  COMPARE_MACRO(Frintm(z8.VnH(), p3.Zeroing(), z10.VnH()),
                "movprfx z8.h, p3/z, z10.h\n"
                "frintm z8.h, p3/m, z10.h");
  COMPARE_MACRO(Frintn(z9.VnD(), p3.Zeroing(), z9.VnD()),
                "movprfx z9.d, p3/z, z9.d\n"
                "frintn z9.d, p3/m, z9.d");
  COMPARE_MACRO(Frintp(z10.VnS(), p3.Zeroing(), z8.VnS()),
                "movprfx z10.s, p3/z, z8.s\n"
                "frintp z10.s, p3/m, z8.s");
  COMPARE_MACRO(Frintx(z11.VnH(), p3.Zeroing(), z7.VnH()),
                "movprfx z11.h, p3/z, z7.h\n"
                "frintx z11.h, p3/m, z7.h");
  COMPARE_MACRO(Frintz(z12.VnD(), p3.Zeroing(), z6.VnD()),
                "movprfx z12.d, p3/z, z6.d\n"
                "frintz z12.d, p3/m, z6.d");
  COMPARE_MACRO(Fsqrt(z30.VnH(), p3.Zeroing(), z13.VnH()),
                "movprfx z30.h, p3/z, z13.h\n"
                "fsqrt z30.h, p3/m, z13.h");
  COMPARE_MACRO(Fsqrt(z29.VnS(), p3.Zeroing(), z14.VnS()),
                "movprfx z29.s, p3/z, z14.s\n"
                "fsqrt z29.s, p3/m, z14.s");
  COMPARE_MACRO(Fsqrt(z28.VnD(), p3.Zeroing(), z15.VnD()),
                "movprfx z28.d, p3/z, z15.d\n"
                "fsqrt z28.d, p3/m, z15.d");

  CLEANUP();
}

TEST(sve_fp_unary_op_unpredicated) {
  SETUP();

  COMPARE(frecpe(z0.VnH(), z2.VnH()), "frecpe z0.h, z2.h");
  COMPARE(frecpe(z0.VnS(), z2.VnS()), "frecpe z0.s, z2.s");
  COMPARE(frecpe(z0.VnD(), z2.VnD()), "frecpe z0.d, z2.d");
  COMPARE(frsqrte(z27.VnH(), z14.VnH()), "frsqrte z27.h, z14.h");
  COMPARE(frsqrte(z27.VnS(), z14.VnS()), "frsqrte z27.s, z14.s");
  COMPARE(frsqrte(z27.VnD(), z14.VnD()), "frsqrte z27.d, z14.d");

  CLEANUP();
}

TEST(sve_inc_dec_by_predicate_count) {
  SETUP();

  COMPARE(decp(x17, p0.VnB()), "decp x17, p0.b");
  COMPARE(decp(x17, p0.VnH()), "decp x17, p0.h");
  COMPARE(decp(x17, p0.VnS()), "decp x17, p0.s");
  COMPARE(decp(x17, p0.VnD()), "decp x17, p0.d");
  COMPARE(decp(z2.VnH(), p11), "decp z2.h, p11");
  COMPARE(decp(z2.VnS(), p11), "decp z2.s, p11");
  COMPARE(decp(z2.VnD(), p11), "decp z2.d, p11");
  COMPARE(incp(x26, p8.VnB()), "incp x26, p8.b");
  COMPARE(incp(x26, p8.VnH()), "incp x26, p8.h");
  COMPARE(incp(x26, p8.VnS()), "incp x26, p8.s");
  COMPARE(incp(x26, p8.VnD()), "incp x26, p8.d");
  COMPARE(incp(z27.VnH(), p9), "incp z27.h, p9");
  COMPARE(incp(z27.VnS(), p9), "incp z27.s, p9");
  COMPARE(incp(z27.VnD(), p9), "incp z27.d, p9");
  COMPARE(sqdecp(x12, p7.VnB(), w12), "sqdecp x12, p7.b, w12");
  COMPARE(sqdecp(x12, p7.VnH(), w12), "sqdecp x12, p7.h, w12");
  COMPARE(sqdecp(x12, p7.VnS(), w12), "sqdecp x12, p7.s, w12");
  COMPARE(sqdecp(x12, p7.VnD(), w12), "sqdecp x12, p7.d, w12");
  COMPARE(sqdecp(x30, p5.VnB()), "sqdecp x30, p5.b");
  COMPARE(sqdecp(x30, p5.VnH()), "sqdecp x30, p5.h");
  COMPARE(sqdecp(x30, p5.VnS()), "sqdecp x30, p5.s");
  COMPARE(sqdecp(x30, p5.VnD()), "sqdecp x30, p5.d");
  COMPARE(sqdecp(z13.VnH(), p1), "sqdecp z13.h, p1");
  COMPARE(sqdecp(z13.VnS(), p1), "sqdecp z13.s, p1");
  COMPARE(sqdecp(z13.VnD(), p1), "sqdecp z13.d, p1");
  COMPARE(sqincp(x26, p5.VnB(), w26), "sqincp x26, p5.b, w26");
  COMPARE(sqincp(x26, p5.VnH(), w26), "sqincp x26, p5.h, w26");
  COMPARE(sqincp(x26, p5.VnS(), w26), "sqincp x26, p5.s, w26");
  COMPARE(sqincp(x26, p5.VnD(), w26), "sqincp x26, p5.d, w26");
  COMPARE(sqincp(x5, p15.VnB()), "sqincp x5, p15.b");
  COMPARE(sqincp(x5, p15.VnH()), "sqincp x5, p15.h");
  COMPARE(sqincp(x5, p15.VnS()), "sqincp x5, p15.s");
  COMPARE(sqincp(x5, p15.VnD()), "sqincp x5, p15.d");
  COMPARE(sqincp(z14.VnH(), p4), "sqincp z14.h, p4");
  COMPARE(sqincp(z14.VnS(), p4), "sqincp z14.s, p4");
  COMPARE(sqincp(z14.VnD(), p4), "sqincp z14.d, p4");
  COMPARE(uqdecp(w3, p13.VnB()), "uqdecp w3, p13.b");
  COMPARE(uqdecp(w3, p13.VnH()), "uqdecp w3, p13.h");
  COMPARE(uqdecp(w3, p13.VnS()), "uqdecp w3, p13.s");
  COMPARE(uqdecp(w3, p13.VnD()), "uqdecp w3, p13.d");
  COMPARE(uqdecp(x19, p0.VnB()), "uqdecp x19, p0.b");
  COMPARE(uqdecp(x19, p0.VnH()), "uqdecp x19, p0.h");
  COMPARE(uqdecp(x19, p0.VnS()), "uqdecp x19, p0.s");
  COMPARE(uqdecp(x19, p0.VnD()), "uqdecp x19, p0.d");
  COMPARE(uqdecp(z15.VnH(), p9), "uqdecp z15.h, p9");
  COMPARE(uqdecp(z15.VnS(), p9), "uqdecp z15.s, p9");
  COMPARE(uqdecp(z15.VnD(), p9), "uqdecp z15.d, p9");
  COMPARE(uqincp(w18, p1.VnB()), "uqincp w18, p1.b");
  COMPARE(uqincp(w18, p1.VnH()), "uqincp w18, p1.h");
  COMPARE(uqincp(w18, p1.VnS()), "uqincp w18, p1.s");
  COMPARE(uqincp(w18, p1.VnD()), "uqincp w18, p1.d");
  COMPARE(uqincp(x17, p15.VnB()), "uqincp x17, p15.b");
  COMPARE(uqincp(x17, p15.VnH()), "uqincp x17, p15.h");
  COMPARE(uqincp(x17, p15.VnS()), "uqincp x17, p15.s");
  COMPARE(uqincp(x17, p15.VnD()), "uqincp x17, p15.d");
  COMPARE(uqincp(z4.VnH(), p3), "uqincp z4.h, p3");
  COMPARE(uqincp(z4.VnS(), p3), "uqincp z4.s, p3");
  COMPARE(uqincp(z4.VnD(), p3), "uqincp z4.d, p3");

  CLEANUP();
}

TEST(sve_inc_dec_by_predicate_count_macro) {
  SETUP();

  // The MacroAssembler automatically generates movprfx where it can.
  COMPARE_MACRO(Decp(z0.VnD(), p1), "decp z0.d, p1");
  COMPARE_MACRO(Decp(z2.VnS(), p3, z2.VnS()), "decp z2.s, p3");
  COMPARE_MACRO(Decp(z3.VnS(), p3, z3.VnS()), "decp z3.s, p3");
  COMPARE_MACRO(Decp(z4.VnH(), p5, z6.VnH()),
                "movprfx z4, z6\n"
                "decp z4.h, p5");
  COMPARE_MACRO(Incp(z7.VnD(), p8), "incp z7.d, p8");
  COMPARE_MACRO(Incp(z9.VnS(), p10, z9.VnS()), "incp z9.s, p10");
  COMPARE_MACRO(Incp(z10.VnS(), p10, z10.VnS()), "incp z10.s, p10");
  COMPARE_MACRO(Incp(z10.VnH(), p11, z12.VnH()),
                "movprfx z10, z12\n"
                "incp z10.h, p11");
  COMPARE_MACRO(Sqdecp(z0.VnD(), p1), "sqdecp z0.d, p1");
  COMPARE_MACRO(Sqdecp(z2.VnS(), p3, z2.VnS()), "sqdecp z2.s, p3");
  COMPARE_MACRO(Sqdecp(z3.VnS(), p3, z3.VnS()), "sqdecp z3.s, p3");
  COMPARE_MACRO(Sqdecp(z4.VnH(), p5, z6.VnH()),
                "movprfx z4, z6\n"
                "sqdecp z4.h, p5");
  COMPARE_MACRO(Sqincp(z7.VnD(), p8), "sqincp z7.d, p8");
  COMPARE_MACRO(Sqincp(z9.VnS(), p10, z9.VnS()), "sqincp z9.s, p10");
  COMPARE_MACRO(Sqincp(z10.VnS(), p10, z10.VnS()), "sqincp z10.s, p10");
  COMPARE_MACRO(Sqincp(z10.VnH(), p11, z12.VnH()),
                "movprfx z10, z12\n"
                "sqincp z10.h, p11");
  COMPARE_MACRO(Uqdecp(z0.VnD(), p1), "uqdecp z0.d, p1");
  COMPARE_MACRO(Uqdecp(z2.VnS(), p3, z2.VnS()), "uqdecp z2.s, p3");
  COMPARE_MACRO(Uqdecp(z3.VnS(), p3, z3.VnS()), "uqdecp z3.s, p3");
  COMPARE_MACRO(Uqdecp(z4.VnH(), p5, z6.VnH()),
                "movprfx z4, z6\n"
                "uqdecp z4.h, p5");
  COMPARE_MACRO(Uqincp(z7.VnD(), p8), "uqincp z7.d, p8");
  COMPARE_MACRO(Uqincp(z9.VnS(), p10, z9.VnS()), "uqincp z9.s, p10");
  COMPARE_MACRO(Uqincp(z10.VnS(), p10, z10.VnS()), "uqincp z10.s, p10");
  COMPARE_MACRO(Uqincp(z10.VnH(), p11, z12.VnH()),
                "movprfx z10, z12\n"
                "uqincp z10.h, p11");

  // Sqdecp cannot write into a W register, but Uqdecp can.
  COMPARE_MACRO(Uqdecp(w6, p7.VnD()), "uqdecp w6, p7.d");
  COMPARE_MACRO(Uqdecp(x10, p11.VnH()), "uqdecp x10, p11.h");
  COMPARE_MACRO(Uqdecp(x12, p13.VnS()), "uqdecp x12, p13.s");
  COMPARE_MACRO(Uqdecp(w14, p15.VnD()), "uqdecp w14, p15.d");

  CLEANUP();
}

TEST(sve_index_generation) {
  SETUP();

  COMPARE(index(z21.VnB(), -16, 15), "index z21.b, #-16, #15");
  COMPARE(index(z22.VnB(), -2, 1), "index z22.b, #-2, #1");
  COMPARE(index(z23.VnH(), -1, 0), "index z23.h, #-1, #0");
  COMPARE(index(z24.VnS(), 0, -1), "index z24.s, #0, #-1");
  COMPARE(index(z25.VnD(), 1, -2), "index z25.d, #1, #-2");
  COMPARE(index(z26.VnB(), 15, -16), "index z26.b, #15, #-16");
  COMPARE(index(z23.VnB(), -16, w8), "index z23.b, #-16, w8");
  COMPARE(index(z24.VnH(), -1, x9), "index z24.h, #-1, w9");
  COMPARE(index(z25.VnS(), 0, w10), "index z25.s, #0, w10");
  COMPARE(index(z26.VnD(), 15, x11), "index z26.d, #15, x11");
  COMPARE(index(z14.VnB(), w15, 15), "index z14.b, w15, #15");
  COMPARE(index(z15.VnH(), x16, 1), "index z15.h, w16, #1");
  COMPARE(index(z16.VnS(), w17, 0), "index z16.s, w17, #0");
  COMPARE(index(z17.VnD(), x18, -16), "index z17.d, x18, #-16");
  COMPARE(index(z20.VnB(), w23, w21), "index z20.b, w23, w21");
  COMPARE(index(z21.VnH(), x24, w22), "index z21.h, w24, w22");
  COMPARE(index(z22.VnS(), w25, x23), "index z22.s, w25, w23");
  COMPARE(index(z23.VnD(), x26, x24), "index z23.d, x26, x24");

  // Simple pass-through macros.
  COMPARE_MACRO(Index(z21.VnB(), -16, 15), "index z21.b, #-16, #15");
  COMPARE_MACRO(Index(z22.VnB(), -2, 1), "index z22.b, #-2, #1");
  COMPARE_MACRO(Index(z23.VnH(), -1, 0), "index z23.h, #-1, #0");
  COMPARE_MACRO(Index(z24.VnS(), 0, -1), "index z24.s, #0, #-1");
  COMPARE_MACRO(Index(z25.VnD(), 1, -2), "index z25.d, #1, #-2");
  COMPARE_MACRO(Index(z26.VnB(), 15, -16), "index z26.b, #15, #-16");
  COMPARE_MACRO(Index(z23.VnB(), -16, w8), "index z23.b, #-16, w8");
  COMPARE_MACRO(Index(z24.VnH(), -1, x9), "index z24.h, #-1, w9");
  COMPARE_MACRO(Index(z25.VnS(), 0, w10), "index z25.s, #0, w10");
  COMPARE_MACRO(Index(z26.VnD(), 15, x11), "index z26.d, #15, x11");
  COMPARE_MACRO(Index(z14.VnB(), w15, 15), "index z14.b, w15, #15");
  COMPARE_MACRO(Index(z15.VnH(), x16, 1), "index z15.h, w16, #1");
  COMPARE_MACRO(Index(z16.VnS(), w17, 0), "index z16.s, w17, #0");
  COMPARE_MACRO(Index(z17.VnD(), x18, -16), "index z17.d, x18, #-16");
  COMPARE_MACRO(Index(z20.VnB(), w23, w21), "index z20.b, w23, w21");
  COMPARE_MACRO(Index(z21.VnH(), x24, w22), "index z21.h, w24, w22");
  COMPARE_MACRO(Index(z22.VnS(), w25, x23), "index z22.s, w25, w23");
  COMPARE_MACRO(Index(z23.VnD(), x26, x24), "index z23.d, x26, x24");

  // Argument synthesis.
  COMPARE_MACRO(Index(z0.VnB(), 16, -17),
                "mov w16, #0x10\n"
                "mov w17, #0xffffffef\n"
                "index z0.b, w16, w17");
  COMPARE_MACRO(Index(z1.VnH(), x2, -17),
                "mov w16, #0xffffffef\n"
                "index z1.h, w2, w16");
  COMPARE_MACRO(Index(z3.VnS(), 16, w4),
                "mov w16, #0x10\n"
                "index z3.s, w16, w4");
  COMPARE_MACRO(Index(z4.VnD(), -17, 16),
                "mov x16, #0xffffffffffffffef\n"
                "mov x17, #0x10\n"
                "index z4.d, x16, x17");

  CLEANUP();
}

TEST(sve_int_arithmetic_unpredicated) {
  SETUP();

  COMPARE(add(z23.VnB(), z30.VnB(), z31.VnB()), "add z23.b, z30.b, z31.b");
  COMPARE(add(z24.VnH(), z29.VnH(), z30.VnH()), "add z24.h, z29.h, z30.h");
  COMPARE(add(z25.VnS(), z28.VnS(), z29.VnS()), "add z25.s, z28.s, z29.s");
  COMPARE(add(z26.VnD(), z27.VnD(), z28.VnD()), "add z26.d, z27.d, z28.d");
  COMPARE(sqadd(z26.VnB(), z21.VnB(), z1.VnB()), "sqadd z26.b, z21.b, z1.b");
  COMPARE(sqadd(z25.VnH(), z20.VnH(), z2.VnH()), "sqadd z25.h, z20.h, z2.h");
  COMPARE(sqadd(z24.VnS(), z19.VnS(), z3.VnS()), "sqadd z24.s, z19.s, z3.s");
  COMPARE(sqadd(z23.VnD(), z18.VnD(), z4.VnD()), "sqadd z23.d, z18.d, z4.d");
  COMPARE(sqsub(z1.VnB(), z10.VnB(), z0.VnB()), "sqsub z1.b, z10.b, z0.b");
  COMPARE(sqsub(z2.VnH(), z11.VnH(), z1.VnH()), "sqsub z2.h, z11.h, z1.h");
  COMPARE(sqsub(z3.VnS(), z12.VnS(), z2.VnS()), "sqsub z3.s, z12.s, z2.s");
  COMPARE(sqsub(z4.VnD(), z13.VnD(), z3.VnD()), "sqsub z4.d, z13.d, z3.d");
  COMPARE(sub(z9.VnB(), z7.VnB(), z25.VnB()), "sub z9.b, z7.b, z25.b");
  COMPARE(sub(z8.VnH(), z8.VnH(), z26.VnH()), "sub z8.h, z8.h, z26.h");
  COMPARE(sub(z7.VnS(), z9.VnS(), z27.VnS()), "sub z7.s, z9.s, z27.s");
  COMPARE(sub(z6.VnD(), z10.VnD(), z28.VnD()), "sub z6.d, z10.d, z28.d");
  COMPARE(uqadd(z13.VnB(), z15.VnB(), z3.VnB()), "uqadd z13.b, z15.b, z3.b");
  COMPARE(uqadd(z12.VnH(), z16.VnH(), z2.VnH()), "uqadd z12.h, z16.h, z2.h");
  COMPARE(uqadd(z11.VnS(), z17.VnS(), z1.VnS()), "uqadd z11.s, z17.s, z1.s");
  COMPARE(uqadd(z10.VnD(), z18.VnD(), z0.VnD()), "uqadd z10.d, z18.d, z0.d");
  COMPARE(uqsub(z9.VnB(), z13.VnB(), z13.VnB()), "uqsub z9.b, z13.b, z13.b");
  COMPARE(uqsub(z11.VnH(), z15.VnH(), z11.VnH()), "uqsub z11.h, z15.h, z11.h");
  COMPARE(uqsub(z13.VnS(), z17.VnS(), z13.VnS()), "uqsub z13.s, z17.s, z13.s");
  COMPARE(uqsub(z15.VnD(), z19.VnD(), z15.VnD()), "uqsub z15.d, z19.d, z15.d");

  CLEANUP();
}

TEST(sve_int_binary_arithmetic_predicated) {
  SETUP();

  COMPARE(add(z22.VnB(), p4.Merging(), z22.VnB(), z20.VnB()),
          "add z22.b, p4/m, z22.b, z20.b");
  COMPARE(add(z22.VnH(), p4.Merging(), z22.VnH(), z20.VnH()),
          "add z22.h, p4/m, z22.h, z20.h");
  COMPARE(add(z22.VnS(), p4.Merging(), z22.VnS(), z20.VnS()),
          "add z22.s, p4/m, z22.s, z20.s");
  COMPARE(add(z22.VnD(), p4.Merging(), z22.VnD(), z20.VnD()),
          "add z22.d, p4/m, z22.d, z20.d");
  COMPARE(and_(z22.VnB(), p3.Merging(), z22.VnB(), z3.VnB()),
          "and z22.b, p3/m, z22.b, z3.b");
  COMPARE(and_(z22.VnH(), p3.Merging(), z22.VnH(), z3.VnH()),
          "and z22.h, p3/m, z22.h, z3.h");
  COMPARE(and_(z22.VnS(), p3.Merging(), z22.VnS(), z3.VnS()),
          "and z22.s, p3/m, z22.s, z3.s");
  COMPARE(and_(z22.VnD(), p3.Merging(), z22.VnD(), z3.VnD()),
          "and z22.d, p3/m, z22.d, z3.d");
  COMPARE(bic(z17.VnB(), p7.Merging(), z17.VnB(), z10.VnB()),
          "bic z17.b, p7/m, z17.b, z10.b");
  COMPARE(bic(z17.VnH(), p7.Merging(), z17.VnH(), z10.VnH()),
          "bic z17.h, p7/m, z17.h, z10.h");
  COMPARE(bic(z17.VnS(), p7.Merging(), z17.VnS(), z10.VnS()),
          "bic z17.s, p7/m, z17.s, z10.s");
  COMPARE(bic(z17.VnD(), p7.Merging(), z17.VnD(), z10.VnD()),
          "bic z17.d, p7/m, z17.d, z10.d");
  COMPARE(eor(z23.VnB(), p4.Merging(), z23.VnB(), z15.VnB()),
          "eor z23.b, p4/m, z23.b, z15.b");
  COMPARE(eor(z23.VnH(), p4.Merging(), z23.VnH(), z15.VnH()),
          "eor z23.h, p4/m, z23.h, z15.h");
  COMPARE(eor(z23.VnS(), p4.Merging(), z23.VnS(), z15.VnS()),
          "eor z23.s, p4/m, z23.s, z15.s");
  COMPARE(eor(z23.VnD(), p4.Merging(), z23.VnD(), z15.VnD()),
          "eor z23.d, p4/m, z23.d, z15.d");
  COMPARE(mul(z15.VnB(), p5.Merging(), z15.VnB(), z15.VnB()),
          "mul z15.b, p5/m, z15.b, z15.b");
  COMPARE(mul(z15.VnH(), p5.Merging(), z15.VnH(), z15.VnH()),
          "mul z15.h, p5/m, z15.h, z15.h");
  COMPARE(mul(z15.VnS(), p5.Merging(), z15.VnS(), z15.VnS()),
          "mul z15.s, p5/m, z15.s, z15.s");
  COMPARE(mul(z15.VnD(), p5.Merging(), z15.VnD(), z15.VnD()),
          "mul z15.d, p5/m, z15.d, z15.d");
  COMPARE(orr(z9.VnB(), p1.Merging(), z9.VnB(), z28.VnB()),
          "orr z9.b, p1/m, z9.b, z28.b");
  COMPARE(orr(z9.VnH(), p1.Merging(), z9.VnH(), z28.VnH()),
          "orr z9.h, p1/m, z9.h, z28.h");
  COMPARE(orr(z9.VnS(), p1.Merging(), z9.VnS(), z28.VnS()),
          "orr z9.s, p1/m, z9.s, z28.s");
  COMPARE(orr(z9.VnD(), p1.Merging(), z9.VnD(), z28.VnD()),
          "orr z9.d, p1/m, z9.d, z28.d");
  COMPARE(sabd(z11.VnB(), p6.Merging(), z11.VnB(), z31.VnB()),
          "sabd z11.b, p6/m, z11.b, z31.b");
  COMPARE(sabd(z11.VnH(), p6.Merging(), z11.VnH(), z31.VnH()),
          "sabd z11.h, p6/m, z11.h, z31.h");
  COMPARE(sabd(z11.VnS(), p6.Merging(), z11.VnS(), z31.VnS()),
          "sabd z11.s, p6/m, z11.s, z31.s");
  COMPARE(sabd(z11.VnD(), p6.Merging(), z11.VnD(), z31.VnD()),
          "sabd z11.d, p6/m, z11.d, z31.d");
  COMPARE(sdivr(z20.VnS(), p5.Merging(), z20.VnS(), z23.VnS()),
          "sdivr z20.s, p5/m, z20.s, z23.s");
  COMPARE(sdiv(z15.VnD(), p6.Merging(), z15.VnD(), z8.VnD()),
          "sdiv z15.d, p6/m, z15.d, z8.d");
  COMPARE(smax(z30.VnB(), p4.Merging(), z30.VnB(), z30.VnB()),
          "smax z30.b, p4/m, z30.b, z30.b");
  COMPARE(smax(z30.VnH(), p4.Merging(), z30.VnH(), z30.VnH()),
          "smax z30.h, p4/m, z30.h, z30.h");
  COMPARE(smax(z30.VnS(), p4.Merging(), z30.VnS(), z30.VnS()),
          "smax z30.s, p4/m, z30.s, z30.s");
  COMPARE(smax(z30.VnD(), p4.Merging(), z30.VnD(), z30.VnD()),
          "smax z30.d, p4/m, z30.d, z30.d");
  COMPARE(smin(z20.VnB(), p7.Merging(), z20.VnB(), z19.VnB()),
          "smin z20.b, p7/m, z20.b, z19.b");
  COMPARE(smin(z20.VnH(), p7.Merging(), z20.VnH(), z19.VnH()),
          "smin z20.h, p7/m, z20.h, z19.h");
  COMPARE(smin(z20.VnS(), p7.Merging(), z20.VnS(), z19.VnS()),
          "smin z20.s, p7/m, z20.s, z19.s");
  COMPARE(smin(z20.VnD(), p7.Merging(), z20.VnD(), z19.VnD()),
          "smin z20.d, p7/m, z20.d, z19.d");
  COMPARE(smulh(z23.VnB(), p0.Merging(), z23.VnB(), z3.VnB()),
          "smulh z23.b, p0/m, z23.b, z3.b");
  COMPARE(smulh(z23.VnH(), p0.Merging(), z23.VnH(), z3.VnH()),
          "smulh z23.h, p0/m, z23.h, z3.h");
  COMPARE(smulh(z23.VnS(), p0.Merging(), z23.VnS(), z3.VnS()),
          "smulh z23.s, p0/m, z23.s, z3.s");
  COMPARE(smulh(z23.VnD(), p0.Merging(), z23.VnD(), z3.VnD()),
          "smulh z23.d, p0/m, z23.d, z3.d");
  COMPARE(subr(z1.VnB(), p6.Merging(), z1.VnB(), z1.VnB()),
          "subr z1.b, p6/m, z1.b, z1.b");
  COMPARE(subr(z1.VnH(), p6.Merging(), z1.VnH(), z1.VnH()),
          "subr z1.h, p6/m, z1.h, z1.h");
  COMPARE(subr(z1.VnS(), p6.Merging(), z1.VnS(), z1.VnS()),
          "subr z1.s, p6/m, z1.s, z1.s");
  COMPARE(subr(z1.VnD(), p6.Merging(), z1.VnD(), z1.VnD()),
          "subr z1.d, p6/m, z1.d, z1.d");
  COMPARE(sub(z28.VnB(), p2.Merging(), z28.VnB(), z0.VnB()),
          "sub z28.b, p2/m, z28.b, z0.b");
  COMPARE(sub(z28.VnH(), p2.Merging(), z28.VnH(), z0.VnH()),
          "sub z28.h, p2/m, z28.h, z0.h");
  COMPARE(sub(z28.VnS(), p2.Merging(), z28.VnS(), z0.VnS()),
          "sub z28.s, p2/m, z28.s, z0.s");
  COMPARE(sub(z28.VnD(), p2.Merging(), z28.VnD(), z0.VnD()),
          "sub z28.d, p2/m, z28.d, z0.d");
  COMPARE(uabd(z14.VnB(), p6.Merging(), z14.VnB(), z22.VnB()),
          "uabd z14.b, p6/m, z14.b, z22.b");
  COMPARE(uabd(z14.VnH(), p6.Merging(), z14.VnH(), z22.VnH()),
          "uabd z14.h, p6/m, z14.h, z22.h");
  COMPARE(uabd(z14.VnS(), p6.Merging(), z14.VnS(), z22.VnS()),
          "uabd z14.s, p6/m, z14.s, z22.s");
  COMPARE(uabd(z14.VnD(), p6.Merging(), z14.VnD(), z22.VnD()),
          "uabd z14.d, p6/m, z14.d, z22.d");
  COMPARE(udivr(z27.VnS(), p5.Merging(), z27.VnS(), z31.VnS()),
          "udivr z27.s, p5/m, z27.s, z31.s");
  COMPARE(udiv(z13.VnD(), p4.Merging(), z13.VnD(), z11.VnD()),
          "udiv z13.d, p4/m, z13.d, z11.d");
  COMPARE(umax(z0.VnB(), p5.Merging(), z0.VnB(), z14.VnB()),
          "umax z0.b, p5/m, z0.b, z14.b");
  COMPARE(umax(z0.VnH(), p5.Merging(), z0.VnH(), z14.VnH()),
          "umax z0.h, p5/m, z0.h, z14.h");
  COMPARE(umax(z0.VnS(), p5.Merging(), z0.VnS(), z14.VnS()),
          "umax z0.s, p5/m, z0.s, z14.s");
  COMPARE(umax(z0.VnD(), p5.Merging(), z0.VnD(), z14.VnD()),
          "umax z0.d, p5/m, z0.d, z14.d");
  COMPARE(umin(z26.VnB(), p5.Merging(), z26.VnB(), z12.VnB()),
          "umin z26.b, p5/m, z26.b, z12.b");
  COMPARE(umin(z26.VnH(), p5.Merging(), z26.VnH(), z12.VnH()),
          "umin z26.h, p5/m, z26.h, z12.h");
  COMPARE(umin(z26.VnS(), p5.Merging(), z26.VnS(), z12.VnS()),
          "umin z26.s, p5/m, z26.s, z12.s");
  COMPARE(umin(z26.VnD(), p5.Merging(), z26.VnD(), z12.VnD()),
          "umin z26.d, p5/m, z26.d, z12.d");
  COMPARE(umulh(z12.VnB(), p2.Merging(), z12.VnB(), z17.VnB()),
          "umulh z12.b, p2/m, z12.b, z17.b");
  COMPARE(umulh(z12.VnH(), p2.Merging(), z12.VnH(), z17.VnH()),
          "umulh z12.h, p2/m, z12.h, z17.h");
  COMPARE(umulh(z12.VnS(), p2.Merging(), z12.VnS(), z17.VnS()),
          "umulh z12.s, p2/m, z12.s, z17.s");
  COMPARE(umulh(z12.VnD(), p2.Merging(), z12.VnD(), z17.VnD()),
          "umulh z12.d, p2/m, z12.d, z17.d");
  CLEANUP();
}

TEST(sve_int_binary_arithmetic_predicated_macro) {
  SETUP();

  COMPARE_MACRO(Add(z22.VnB(), p4.Merging(), z22.VnB(), z20.VnB()),
                "add z22.b, p4/m, z22.b, z20.b");
  COMPARE_MACRO(Add(z22.VnH(), p4.Merging(), z20.VnH(), z22.VnH()),
                "add z22.h, p4/m, z22.h, z20.h");
  COMPARE_MACRO(Add(z22.VnS(), p4.Merging(), z21.VnS(), z20.VnS()),
                "movprfx z22.s, p4/m, z21.s\n"
                "add z22.s, p4/m, z22.s, z20.s");

  COMPARE_MACRO(And(z22.VnH(), p3.Merging(), z22.VnH(), z3.VnH()),
                "and z22.h, p3/m, z22.h, z3.h");
  COMPARE_MACRO(And(z22.VnS(), p3.Merging(), z3.VnS(), z22.VnS()),
                "and z22.s, p3/m, z22.s, z3.s");
  COMPARE_MACRO(And(z22.VnD(), p3.Merging(), z2.VnD(), z3.VnD()),
                "movprfx z22.d, p3/m, z2.d\n"
                "and z22.d, p3/m, z22.d, z3.d");

  COMPARE_MACRO(Bic(z17.VnB(), p7.Merging(), z17.VnB(), z10.VnB()),
                "bic z17.b, p7/m, z17.b, z10.b");
  COMPARE_MACRO(Bic(z17.VnS(), p7.Merging(), z10.VnS(), z17.VnS()),
                "mov z31.d, z17.d\n"
                "movprfx z17.s, p7/m, z10.s\n"
                "bic z17.s, p7/m, z17.s, z31.s");
  COMPARE_MACRO(Bic(z17.VnD(), p7.Merging(), z7.VnD(), z27.VnD()),
                "movprfx z17.d, p7/m, z7.d\n"
                "bic z17.d, p7/m, z17.d, z27.d");

  COMPARE_MACRO(Eor(z23.VnB(), p4.Merging(), z23.VnB(), z15.VnB()),
                "eor z23.b, p4/m, z23.b, z15.b");
  COMPARE_MACRO(Eor(z23.VnH(), p4.Merging(), z23.VnH(), z15.VnH()),
                "eor z23.h, p4/m, z23.h, z15.h");
  COMPARE_MACRO(Eor(z23.VnD(), p4.Merging(), z18.VnD(), z15.VnD()),
                "movprfx z23.d, p4/m, z18.d\n"
                "eor z23.d, p4/m, z23.d, z15.d");

  COMPARE_MACRO(Mul(z15.VnB(), p5.Merging(), z15.VnB(), z15.VnB()),
                "mul z15.b, p5/m, z15.b, z15.b");
  COMPARE_MACRO(Mul(z15.VnH(), p5.Merging(), z7.VnH(), z15.VnH()),
                "mul z15.h, p5/m, z15.h, z7.h");
  COMPARE_MACRO(Mul(z15.VnS(), p5.Merging(), z0.VnS(), z1.VnS()),
                "movprfx z15.s, p5/m, z0.s\n"
                "mul z15.s, p5/m, z15.s, z1.s");

  COMPARE_MACRO(Orr(z9.VnH(), p1.Merging(), z9.VnH(), z28.VnH()),
                "orr z9.h, p1/m, z9.h, z28.h");
  COMPARE_MACRO(Orr(z9.VnS(), p1.Merging(), z9.VnS(), z28.VnS()),
                "orr z9.s, p1/m, z9.s, z28.s");
  COMPARE_MACRO(Orr(z9.VnD(), p1.Merging(), z6.VnD(), z7.VnD()),
                "movprfx z9.d, p1/m, z6.d\n"
                "orr z9.d, p1/m, z9.d, z7.d");

  COMPARE_MACRO(Sabd(z11.VnB(), p6.Merging(), z11.VnB(), z31.VnB()),
                "sabd z11.b, p6/m, z11.b, z31.b");
  COMPARE_MACRO(Sabd(z11.VnH(), p6.Merging(), z31.VnH(), z11.VnH()),
                "sabd z11.h, p6/m, z11.h, z31.h");
  COMPARE_MACRO(Sabd(z11.VnS(), p6.Merging(), z21.VnS(), z31.VnS()),
                "movprfx z11.s, p6/m, z21.s\n"
                "sabd z11.s, p6/m, z11.s, z31.s");

  COMPARE_MACRO(Sdiv(z20.VnS(), p5.Merging(), z23.VnS(), z20.VnS()),
                "sdivr z20.s, p5/m, z20.s, z23.s");
  COMPARE_MACRO(Sdiv(z15.VnD(), p6.Merging(), z30.VnD(), z8.VnD()),
                "movprfx z15.d, p6/m, z30.d\n"
                "sdiv z15.d, p6/m, z15.d, z8.d");

  COMPARE_MACRO(Smax(z30.VnB(), p4.Merging(), z30.VnB(), z31.VnB()),
                "smax z30.b, p4/m, z30.b, z31.b");
  COMPARE_MACRO(Smax(z30.VnS(), p4.Merging(), z3.VnS(), z30.VnS()),
                "smax z30.s, p4/m, z30.s, z3.s");
  COMPARE_MACRO(Smax(z30.VnD(), p4.Merging(), z1.VnD(), z5.VnD()),
                "movprfx z30.d, p4/m, z1.d\n"
                "smax z30.d, p4/m, z30.d, z5.d");

  COMPARE_MACRO(Smin(z20.VnH(), p7.Merging(), z20.VnH(), z19.VnH()),
                "smin z20.h, p7/m, z20.h, z19.h");
  COMPARE_MACRO(Smin(z20.VnS(), p7.Merging(), z19.VnS(), z20.VnS()),
                "smin z20.s, p7/m, z20.s, z19.s");
  COMPARE_MACRO(Smin(z20.VnD(), p7.Merging(), z14.VnD(), z15.VnD()),
                "movprfx z20.d, p7/m, z14.d\n"
                "smin z20.d, p7/m, z20.d, z15.d");

  COMPARE_MACRO(Smulh(z23.VnB(), p0.Merging(), z23.VnB(), z3.VnB()),
                "smulh z23.b, p0/m, z23.b, z3.b");
  COMPARE_MACRO(Smulh(z23.VnH(), p0.Merging(), z13.VnH(), z23.VnH()),
                "smulh z23.h, p0/m, z23.h, z13.h");
  COMPARE_MACRO(Smulh(z23.VnD(), p0.Merging(), z30.VnD(), z31.VnD()),
                "movprfx z23.d, p0/m, z30.d\n"
                "smulh z23.d, p0/m, z23.d, z31.d");

  COMPARE_MACRO(Sub(z28.VnB(), p2.Merging(), z28.VnB(), z0.VnB()),
                "sub z28.b, p2/m, z28.b, z0.b");
  COMPARE_MACRO(Sub(z28.VnH(), p2.Merging(), z14.VnH(), z28.VnH()),
                "subr z28.h, p2/m, z28.h, z14.h");
  COMPARE_MACRO(Sub(z28.VnS(), p2.Merging(), z7.VnS(), z13.VnS()),
                "movprfx z28.s, p2/m, z7.s\n"
                "sub z28.s, p2/m, z28.s, z13.s");

  COMPARE_MACRO(Uabd(z14.VnH(), p6.Merging(), z14.VnH(), z22.VnH()),
                "uabd z14.h, p6/m, z14.h, z22.h");
  COMPARE_MACRO(Uabd(z14.VnS(), p6.Merging(), z14.VnS(), z22.VnS()),
                "uabd z14.s, p6/m, z14.s, z22.s");
  COMPARE_MACRO(Uabd(z14.VnD(), p6.Merging(), z13.VnD(), z22.VnD()),
                "movprfx z14.d, p6/m, z13.d\n"
                "uabd z14.d, p6/m, z14.d, z22.d");

  COMPARE_MACRO(Udiv(z27.VnS(), p5.Merging(), z16.VnS(), z27.VnS()),
                "udivr z27.s, p5/m, z27.s, z16.s");
  COMPARE_MACRO(Udiv(z13.VnD(), p4.Merging(), z22.VnD(), z11.VnD()),
                "movprfx z13.d, p4/m, z22.d\n"
                "udiv z13.d, p4/m, z13.d, z11.d");

  COMPARE_MACRO(Umax(z0.VnB(), p5.Merging(), z0.VnB(), z14.VnB()),
                "umax z0.b, p5/m, z0.b, z14.b");
  COMPARE_MACRO(Umax(z0.VnS(), p5.Merging(), z14.VnS(), z0.VnS()),
                "umax z0.s, p5/m, z0.s, z14.s");
  COMPARE_MACRO(Umax(z0.VnD(), p5.Merging(), z29.VnD(), z14.VnD()),
                "movprfx z0.d, p5/m, z29.d\n"
                "umax z0.d, p5/m, z0.d, z14.d");

  COMPARE_MACRO(Umin(z26.VnB(), p5.Merging(), z26.VnB(), z12.VnB()),
                "umin z26.b, p5/m, z26.b, z12.b");
  COMPARE_MACRO(Umin(z26.VnH(), p5.Merging(), z27.VnH(), z26.VnH()),
                "umin z26.h, p5/m, z26.h, z27.h");
  COMPARE_MACRO(Umin(z26.VnD(), p5.Merging(), z13.VnD(), z12.VnD()),
                "movprfx z26.d, p5/m, z13.d\n"
                "umin z26.d, p5/m, z26.d, z12.d");

  COMPARE_MACRO(Umulh(z12.VnB(), p2.Merging(), z12.VnB(), z17.VnB()),
                "umulh z12.b, p2/m, z12.b, z17.b");
  COMPARE_MACRO(Umulh(z12.VnH(), p2.Merging(), z0.VnH(), z12.VnH()),
                "umulh z12.h, p2/m, z12.h, z0.h");
  COMPARE_MACRO(Umulh(z12.VnS(), p2.Merging(), z25.VnS(), z17.VnS()),
                "movprfx z12.s, p2/m, z25.s\n"
                "umulh z12.s, p2/m, z12.s, z17.s");

  CLEANUP();
}

TEST(sve_int_compare_scalars) {
  SETUP();

  COMPARE(ctermeq(w30, w26), "ctermeq w30, w26");
  COMPARE(ctermne(x21, x18), "ctermne x21, x18");
  COMPARE(whilele(p10.VnB(), x11, x6), "whilele p10.b, x11, x6");
  COMPARE(whilele(p10.VnH(), w11, w6), "whilele p10.h, w11, w6");
  COMPARE(whilele(p10.VnH(), x11, x6), "whilele p10.h, x11, x6");
  COMPARE(whilele(p10.VnS(), w11, w6), "whilele p10.s, w11, w6");
  COMPARE(whilele(p10.VnD(), x11, x6), "whilele p10.d, x11, x6");
  COMPARE(whilelo(p4.VnB(), w3, w25), "whilelo p4.b, w3, w25");
  COMPARE(whilelo(p4.VnH(), x3, x25), "whilelo p4.h, x3, x25");
  COMPARE(whilelo(p4.VnS(), w3, w25), "whilelo p4.s, w3, w25");
  COMPARE(whilelo(p4.VnD(), x3, x25), "whilelo p4.d, x3, x25");
  COMPARE(whilels(p7.VnB(), w15, w15), "whilels p7.b, w15, w15");
  COMPARE(whilels(p7.VnH(), x15, x15), "whilels p7.h, x15, x15");
  COMPARE(whilels(p7.VnS(), w15, w15), "whilels p7.s, w15, w15");
  COMPARE(whilels(p7.VnD(), x15, x15), "whilels p7.d, x15, x15");
  COMPARE(whilelt(p14.VnB(), w11, w14), "whilelt p14.b, w11, w14");
  COMPARE(whilelt(p14.VnH(), x11, x14), "whilelt p14.h, x11, x14");
  COMPARE(whilelt(p14.VnS(), w11, w14), "whilelt p14.s, w11, w14");
  COMPARE(whilelt(p14.VnD(), x11, x14), "whilelt p14.d, x11, x14");

  CLEANUP();
}

TEST(sve_int_compare_signed_imm) {
  SETUP();

  COMPARE(cmpeq(p0.VnB(), p3.Zeroing(), z1.VnB(), 15),
          "cmpeq p0.b, p3/z, z1.b, #15");
  COMPARE(cmpeq(p0.VnH(), p3.Zeroing(), z1.VnH(), 7),
          "cmpeq p0.h, p3/z, z1.h, #7");
  COMPARE(cmpeq(p0.VnS(), p3.Zeroing(), z1.VnS(), -3),
          "cmpeq p0.s, p3/z, z1.s, #-3");
  COMPARE(cmpeq(p0.VnD(), p3.Zeroing(), z1.VnD(), -14),
          "cmpeq p0.d, p3/z, z1.d, #-14");
  COMPARE(cmpge(p9.VnB(), p6.Zeroing(), z12.VnB(), 14),
          "cmpge p9.b, p6/z, z12.b, #14");
  COMPARE(cmpge(p9.VnH(), p6.Zeroing(), z12.VnH(), 6),
          "cmpge p9.h, p6/z, z12.h, #6");
  COMPARE(cmpge(p9.VnS(), p6.Zeroing(), z12.VnS(), -4),
          "cmpge p9.s, p6/z, z12.s, #-4");
  COMPARE(cmpge(p9.VnD(), p6.Zeroing(), z12.VnD(), -13),
          "cmpge p9.d, p6/z, z12.d, #-13");
  COMPARE(cmpgt(p15.VnB(), p4.Zeroing(), z23.VnB(), 13),
          "cmpgt p15.b, p4/z, z23.b, #13");
  COMPARE(cmpgt(p15.VnH(), p4.Zeroing(), z23.VnH(), 5),
          "cmpgt p15.h, p4/z, z23.h, #5");
  COMPARE(cmpgt(p15.VnS(), p4.Zeroing(), z23.VnS(), -12),
          "cmpgt p15.s, p4/z, z23.s, #-12");
  COMPARE(cmpgt(p15.VnD(), p4.Zeroing(), z23.VnD(), -5),
          "cmpgt p15.d, p4/z, z23.d, #-5");
  COMPARE(cmple(p4.VnB(), p3.Zeroing(), z5.VnB(), 12),
          "cmple p4.b, p3/z, z5.b, #12");
  COMPARE(cmple(p4.VnH(), p3.Zeroing(), z5.VnH(), 4),
          "cmple p4.h, p3/z, z5.h, #4");
  COMPARE(cmple(p4.VnS(), p3.Zeroing(), z5.VnS(), -11),
          "cmple p4.s, p3/z, z5.s, #-11");
  COMPARE(cmple(p4.VnD(), p3.Zeroing(), z5.VnD(), -6),
          "cmple p4.d, p3/z, z5.d, #-6");
  COMPARE(cmplt(p3.VnB(), p7.Zeroing(), z15.VnB(), 11),
          "cmplt p3.b, p7/z, z15.b, #11");
  COMPARE(cmplt(p3.VnH(), p7.Zeroing(), z15.VnH(), 3),
          "cmplt p3.h, p7/z, z15.h, #3");
  COMPARE(cmplt(p3.VnS(), p7.Zeroing(), z15.VnS(), -10),
          "cmplt p3.s, p7/z, z15.s, #-10");
  COMPARE(cmplt(p3.VnD(), p7.Zeroing(), z15.VnD(), -7),
          "cmplt p3.d, p7/z, z15.d, #-7");
  COMPARE(cmpne(p13.VnB(), p5.Zeroing(), z20.VnB(), 10),
          "cmpne p13.b, p5/z, z20.b, #10");
  COMPARE(cmpne(p13.VnH(), p5.Zeroing(), z20.VnH(), 2),
          "cmpne p13.h, p5/z, z20.h, #2");
  COMPARE(cmpne(p13.VnS(), p5.Zeroing(), z20.VnS(), -9),
          "cmpne p13.s, p5/z, z20.s, #-9");
  COMPARE(cmpne(p13.VnD(), p5.Zeroing(), z20.VnD(), -8),
          "cmpne p13.d, p5/z, z20.d, #-8");

  CLEANUP();
}

TEST(sve_int_compare_unsigned_imm) {
  SETUP();

  COMPARE(cmphi(p8.VnB(), p6.Zeroing(), z1.VnB(), 127),
          "cmphi p8.b, p6/z, z1.b, #127");
  COMPARE(cmphi(p8.VnH(), p6.Zeroing(), z1.VnH(), 126),
          "cmphi p8.h, p6/z, z1.h, #126");
  COMPARE(cmphi(p8.VnS(), p6.Zeroing(), z1.VnS(), 99),
          "cmphi p8.s, p6/z, z1.s, #99");
  COMPARE(cmphi(p8.VnD(), p6.Zeroing(), z1.VnD(), 78),
          "cmphi p8.d, p6/z, z1.d, #78");
  COMPARE(cmphs(p11.VnB(), p2.Zeroing(), z8.VnB(), 67),
          "cmphs p11.b, p2/z, z8.b, #67");
  COMPARE(cmphs(p11.VnH(), p2.Zeroing(), z8.VnH(), 63),
          "cmphs p11.h, p2/z, z8.h, #63");
  COMPARE(cmphs(p11.VnS(), p2.Zeroing(), z8.VnS(), 51),
          "cmphs p11.s, p2/z, z8.s, #51");
  COMPARE(cmphs(p11.VnD(), p2.Zeroing(), z8.VnD(), 40),
          "cmphs p11.d, p2/z, z8.d, #40");
  COMPARE(cmplo(p9.VnB(), p4.Zeroing(), z4.VnB(), 32),
          "cmplo p9.b, p4/z, z4.b, #32");
  COMPARE(cmplo(p9.VnH(), p4.Zeroing(), z4.VnH(), 22),
          "cmplo p9.h, p4/z, z4.h, #22");
  COMPARE(cmplo(p9.VnS(), p4.Zeroing(), z4.VnS(), 15),
          "cmplo p9.s, p4/z, z4.s, #15");
  COMPARE(cmplo(p9.VnD(), p4.Zeroing(), z4.VnD(), 11),
          "cmplo p9.d, p4/z, z4.d, #11");
  COMPARE(cmpls(p14.VnB(), p5.Zeroing(), z9.VnB(), 7),
          "cmpls p14.b, p5/z, z9.b, #7");
  COMPARE(cmpls(p14.VnH(), p5.Zeroing(), z9.VnH(), 4),
          "cmpls p14.h, p5/z, z9.h, #4");
  COMPARE(cmpls(p14.VnS(), p5.Zeroing(), z9.VnS(), 3),
          "cmpls p14.s, p5/z, z9.s, #3");
  COMPARE(cmpls(p14.VnD(), p5.Zeroing(), z9.VnD(), 1),
          "cmpls p14.d, p5/z, z9.d, #1");

  CLEANUP();
}

TEST(sve_int_compare_vectors) {
  SETUP();

  COMPARE(cmpeq(p13.VnB(), p0.Zeroing(), z26.VnB(), z10.VnD()),
          "cmpeq p13.b, p0/z, z26.b, z10.d");
  COMPARE(cmpeq(p13.VnH(), p0.Zeroing(), z26.VnH(), z10.VnD()),
          "cmpeq p13.h, p0/z, z26.h, z10.d");
  COMPARE(cmpeq(p13.VnS(), p0.Zeroing(), z26.VnS(), z10.VnD()),
          "cmpeq p13.s, p0/z, z26.s, z10.d");
  COMPARE(cmpeq(p14.VnB(), p3.Zeroing(), z18.VnB(), z15.VnB()),
          "cmpeq p14.b, p3/z, z18.b, z15.b");
  COMPARE(cmpeq(p14.VnH(), p3.Zeroing(), z18.VnH(), z15.VnH()),
          "cmpeq p14.h, p3/z, z18.h, z15.h");
  COMPARE(cmpeq(p14.VnS(), p3.Zeroing(), z18.VnS(), z15.VnS()),
          "cmpeq p14.s, p3/z, z18.s, z15.s");
  COMPARE(cmpeq(p14.VnD(), p3.Zeroing(), z18.VnD(), z15.VnD()),
          "cmpeq p14.d, p3/z, z18.d, z15.d");
  COMPARE(cmpge(p8.VnB(), p3.Zeroing(), z13.VnB(), z0.VnD()),
          "cmpge p8.b, p3/z, z13.b, z0.d");
  COMPARE(cmpge(p8.VnH(), p3.Zeroing(), z13.VnH(), z0.VnD()),
          "cmpge p8.h, p3/z, z13.h, z0.d");
  COMPARE(cmpge(p8.VnS(), p3.Zeroing(), z13.VnS(), z0.VnD()),
          "cmpge p8.s, p3/z, z13.s, z0.d");
  COMPARE(cmpge(p3.VnB(), p4.Zeroing(), z6.VnB(), z1.VnB()),
          "cmpge p3.b, p4/z, z6.b, z1.b");
  COMPARE(cmpge(p3.VnH(), p4.Zeroing(), z6.VnH(), z1.VnH()),
          "cmpge p3.h, p4/z, z6.h, z1.h");
  COMPARE(cmpge(p3.VnS(), p4.Zeroing(), z6.VnS(), z1.VnS()),
          "cmpge p3.s, p4/z, z6.s, z1.s");
  COMPARE(cmpge(p3.VnD(), p4.Zeroing(), z6.VnD(), z1.VnD()),
          "cmpge p3.d, p4/z, z6.d, z1.d");
  COMPARE(cmpgt(p4.VnB(), p2.Zeroing(), z24.VnB(), z1.VnD()),
          "cmpgt p4.b, p2/z, z24.b, z1.d");
  COMPARE(cmpgt(p4.VnH(), p2.Zeroing(), z24.VnH(), z1.VnD()),
          "cmpgt p4.h, p2/z, z24.h, z1.d");
  COMPARE(cmpgt(p4.VnS(), p2.Zeroing(), z24.VnS(), z1.VnD()),
          "cmpgt p4.s, p2/z, z24.s, z1.d");
  COMPARE(cmpgt(p10.VnB(), p3.Zeroing(), z23.VnB(), z19.VnB()),
          "cmpgt p10.b, p3/z, z23.b, z19.b");
  COMPARE(cmpgt(p10.VnH(), p3.Zeroing(), z23.VnH(), z19.VnH()),
          "cmpgt p10.h, p3/z, z23.h, z19.h");
  COMPARE(cmpgt(p10.VnS(), p3.Zeroing(), z23.VnS(), z19.VnS()),
          "cmpgt p10.s, p3/z, z23.s, z19.s");
  COMPARE(cmpgt(p10.VnD(), p3.Zeroing(), z23.VnD(), z19.VnD()),
          "cmpgt p10.d, p3/z, z23.d, z19.d");
  COMPARE(cmphi(p10.VnB(), p6.Zeroing(), z6.VnB(), z11.VnD()),
          "cmphi p10.b, p6/z, z6.b, z11.d");
  COMPARE(cmphi(p10.VnH(), p6.Zeroing(), z6.VnH(), z11.VnD()),
          "cmphi p10.h, p6/z, z6.h, z11.d");
  COMPARE(cmphi(p10.VnS(), p6.Zeroing(), z6.VnS(), z11.VnD()),
          "cmphi p10.s, p6/z, z6.s, z11.d");
  COMPARE(cmphi(p1.VnB(), p0.Zeroing(), z4.VnB(), z2.VnB()),
          "cmphi p1.b, p0/z, z4.b, z2.b");
  COMPARE(cmphi(p1.VnH(), p0.Zeroing(), z4.VnH(), z2.VnH()),
          "cmphi p1.h, p0/z, z4.h, z2.h");
  COMPARE(cmphi(p1.VnS(), p0.Zeroing(), z4.VnS(), z2.VnS()),
          "cmphi p1.s, p0/z, z4.s, z2.s");
  COMPARE(cmphi(p1.VnD(), p0.Zeroing(), z4.VnD(), z2.VnD()),
          "cmphi p1.d, p0/z, z4.d, z2.d");
  COMPARE(cmphs(p10.VnB(), p5.Zeroing(), z22.VnB(), z5.VnD()),
          "cmphs p10.b, p5/z, z22.b, z5.d");
  COMPARE(cmphs(p10.VnH(), p5.Zeroing(), z22.VnH(), z5.VnD()),
          "cmphs p10.h, p5/z, z22.h, z5.d");
  COMPARE(cmphs(p10.VnS(), p5.Zeroing(), z22.VnS(), z5.VnD()),
          "cmphs p10.s, p5/z, z22.s, z5.d");
  COMPARE(cmphs(p12.VnB(), p6.Zeroing(), z20.VnB(), z24.VnB()),
          "cmphs p12.b, p6/z, z20.b, z24.b");
  COMPARE(cmphs(p12.VnH(), p6.Zeroing(), z20.VnH(), z24.VnH()),
          "cmphs p12.h, p6/z, z20.h, z24.h");
  COMPARE(cmphs(p12.VnS(), p6.Zeroing(), z20.VnS(), z24.VnS()),
          "cmphs p12.s, p6/z, z20.s, z24.s");
  COMPARE(cmphs(p12.VnD(), p6.Zeroing(), z20.VnD(), z24.VnD()),
          "cmphs p12.d, p6/z, z20.d, z24.d");
  COMPARE(cmple(p11.VnB(), p2.Zeroing(), z18.VnB(), z0.VnD()),
          "cmple p11.b, p2/z, z18.b, z0.d");
  COMPARE(cmple(p11.VnH(), p2.Zeroing(), z18.VnH(), z0.VnD()),
          "cmple p11.h, p2/z, z18.h, z0.d");
  COMPARE(cmple(p11.VnS(), p2.Zeroing(), z18.VnS(), z0.VnD()),
          "cmple p11.s, p2/z, z18.s, z0.d");
  COMPARE(cmplo(p12.VnB(), p6.Zeroing(), z21.VnB(), z10.VnD()),
          "cmplo p12.b, p6/z, z21.b, z10.d");
  COMPARE(cmplo(p12.VnH(), p6.Zeroing(), z21.VnH(), z10.VnD()),
          "cmplo p12.h, p6/z, z21.h, z10.d");
  COMPARE(cmplo(p12.VnS(), p6.Zeroing(), z21.VnS(), z10.VnD()),
          "cmplo p12.s, p6/z, z21.s, z10.d");
  COMPARE(cmpls(p8.VnB(), p4.Zeroing(), z9.VnB(), z15.VnD()),
          "cmpls p8.b, p4/z, z9.b, z15.d");
  COMPARE(cmpls(p8.VnH(), p4.Zeroing(), z9.VnH(), z15.VnD()),
          "cmpls p8.h, p4/z, z9.h, z15.d");
  COMPARE(cmpls(p8.VnS(), p4.Zeroing(), z9.VnS(), z15.VnD()),
          "cmpls p8.s, p4/z, z9.s, z15.d");
  COMPARE(cmplt(p6.VnB(), p6.Zeroing(), z4.VnB(), z8.VnD()),
          "cmplt p6.b, p6/z, z4.b, z8.d");
  COMPARE(cmplt(p6.VnH(), p6.Zeroing(), z4.VnH(), z8.VnD()),
          "cmplt p6.h, p6/z, z4.h, z8.d");
  COMPARE(cmplt(p6.VnS(), p6.Zeroing(), z4.VnS(), z8.VnD()),
          "cmplt p6.s, p6/z, z4.s, z8.d");
  COMPARE(cmpne(p1.VnB(), p6.Zeroing(), z31.VnB(), z16.VnD()),
          "cmpne p1.b, p6/z, z31.b, z16.d");
  COMPARE(cmpne(p1.VnH(), p6.Zeroing(), z31.VnH(), z16.VnD()),
          "cmpne p1.h, p6/z, z31.h, z16.d");
  COMPARE(cmpne(p1.VnS(), p6.Zeroing(), z31.VnS(), z16.VnD()),
          "cmpne p1.s, p6/z, z31.s, z16.d");
  COMPARE(cmpne(p11.VnB(), p1.Zeroing(), z3.VnB(), z24.VnB()),
          "cmpne p11.b, p1/z, z3.b, z24.b");
  COMPARE(cmpne(p11.VnH(), p1.Zeroing(), z3.VnH(), z24.VnH()),
          "cmpne p11.h, p1/z, z3.h, z24.h");
  COMPARE(cmpne(p11.VnS(), p1.Zeroing(), z3.VnS(), z24.VnS()),
          "cmpne p11.s, p1/z, z3.s, z24.s");
  COMPARE(cmpne(p11.VnD(), p1.Zeroing(), z3.VnD(), z24.VnD()),
          "cmpne p11.d, p1/z, z3.d, z24.d");
  COMPARE(cmpls(p8.VnB(), p4.Zeroing(), z9.VnB(), z15.VnB()),
          "cmphs p8.b, p4/z, z15.b, z9.b");
  COMPARE(cmpls(p8.VnH(), p4.Zeroing(), z9.VnH(), z15.VnH()),
          "cmphs p8.h, p4/z, z15.h, z9.h");
  COMPARE(cmpls(p8.VnS(), p4.Zeroing(), z9.VnS(), z15.VnS()),
          "cmphs p8.s, p4/z, z15.s, z9.s");
  COMPARE(cmpls(p8.VnD(), p4.Zeroing(), z9.VnD(), z15.VnD()),
          "cmphs p8.d, p4/z, z15.d, z9.d");
  COMPARE(cmplo(p10.VnB(), p3.Zeroing(), z14.VnB(), z20.VnB()),
          "cmphi p10.b, p3/z, z20.b, z14.b");
  COMPARE(cmplo(p10.VnH(), p3.Zeroing(), z14.VnH(), z20.VnH()),
          "cmphi p10.h, p3/z, z20.h, z14.h");
  COMPARE(cmplo(p10.VnS(), p3.Zeroing(), z14.VnS(), z20.VnS()),
          "cmphi p10.s, p3/z, z20.s, z14.s");
  COMPARE(cmplo(p10.VnD(), p3.Zeroing(), z14.VnD(), z20.VnD()),
          "cmphi p10.d, p3/z, z20.d, z14.d");
  COMPARE(cmple(p12.VnB(), p2.Zeroing(), z19.VnB(), z25.VnB()),
          "cmpge p12.b, p2/z, z25.b, z19.b");
  COMPARE(cmple(p12.VnH(), p2.Zeroing(), z19.VnH(), z25.VnH()),
          "cmpge p12.h, p2/z, z25.h, z19.h");
  COMPARE(cmple(p12.VnS(), p2.Zeroing(), z19.VnS(), z25.VnS()),
          "cmpge p12.s, p2/z, z25.s, z19.s");
  COMPARE(cmple(p12.VnD(), p2.Zeroing(), z19.VnD(), z25.VnD()),
          "cmpge p12.d, p2/z, z25.d, z19.d");
  COMPARE(cmplt(p14.VnB(), p1.Zeroing(), z24.VnB(), z30.VnB()),
          "cmpgt p14.b, p1/z, z30.b, z24.b");
  COMPARE(cmplt(p14.VnH(), p1.Zeroing(), z24.VnH(), z30.VnH()),
          "cmpgt p14.h, p1/z, z30.h, z24.h");
  COMPARE(cmplt(p14.VnS(), p1.Zeroing(), z24.VnS(), z30.VnS()),
          "cmpgt p14.s, p1/z, z30.s, z24.s");
  COMPARE(cmplt(p14.VnD(), p1.Zeroing(), z24.VnD(), z30.VnD()),
          "cmpgt p14.d, p1/z, z30.d, z24.d");

  CLEANUP();
}

TEST(sve_int_misc_unpredicated) {
  SETUP();

  COMPARE(fexpa(z5.VnH(), z9.VnH()), "fexpa z5.h, z9.h");
  COMPARE(fexpa(z5.VnS(), z9.VnS()), "fexpa z5.s, z9.s");
  COMPARE(fexpa(z5.VnD(), z9.VnD()), "fexpa z5.d, z9.d");
  COMPARE(ftssel(z17.VnH(), z24.VnH(), z14.VnH()),
          "ftssel z17.h, z24.h, z14.h");
  COMPARE(ftssel(z17.VnS(), z24.VnS(), z14.VnS()),
          "ftssel z17.s, z24.s, z14.s");
  COMPARE(ftssel(z17.VnD(), z24.VnD(), z14.VnD()),
          "ftssel z17.d, z24.d, z14.d");
  COMPARE(movprfx(z24, z1), "movprfx z24, z1");

  CLEANUP();
}

TEST(sve_int_mul_add_predicated) {
  SETUP();

  COMPARE(mad(z29.VnB(), p6.Merging(), z22.VnB(), z21.VnB()),
          "mad z29.b, p6/m, z22.b, z21.b");
  COMPARE(mad(z29.VnH(), p6.Merging(), z22.VnH(), z21.VnH()),
          "mad z29.h, p6/m, z22.h, z21.h");
  COMPARE(mad(z29.VnS(), p6.Merging(), z22.VnS(), z21.VnS()),
          "mad z29.s, p6/m, z22.s, z21.s");
  COMPARE(mad(z29.VnD(), p6.Merging(), z22.VnD(), z21.VnD()),
          "mad z29.d, p6/m, z22.d, z21.d");
  COMPARE(mla(z23.VnB(), p1.Merging(), z21.VnB(), z23.VnB()),
          "mla z23.b, p1/m, z21.b, z23.b");
  COMPARE(mla(z23.VnH(), p1.Merging(), z21.VnH(), z23.VnH()),
          "mla z23.h, p1/m, z21.h, z23.h");
  COMPARE(mla(z23.VnS(), p1.Merging(), z21.VnS(), z23.VnS()),
          "mla z23.s, p1/m, z21.s, z23.s");
  COMPARE(mla(z23.VnD(), p1.Merging(), z21.VnD(), z23.VnD()),
          "mla z23.d, p1/m, z21.d, z23.d");
  COMPARE(mls(z4.VnB(), p6.Merging(), z17.VnB(), z28.VnB()),
          "mls z4.b, p6/m, z17.b, z28.b");
  COMPARE(mls(z4.VnH(), p6.Merging(), z17.VnH(), z28.VnH()),
          "mls z4.h, p6/m, z17.h, z28.h");
  COMPARE(mls(z4.VnS(), p6.Merging(), z17.VnS(), z28.VnS()),
          "mls z4.s, p6/m, z17.s, z28.s");
  COMPARE(mls(z4.VnD(), p6.Merging(), z17.VnD(), z28.VnD()),
          "mls z4.d, p6/m, z17.d, z28.d");
  COMPARE(msb(z27.VnB(), p7.Merging(), z29.VnB(), z1.VnB()),
          "msb z27.b, p7/m, z29.b, z1.b");
  COMPARE(msb(z27.VnH(), p7.Merging(), z29.VnH(), z1.VnH()),
          "msb z27.h, p7/m, z29.h, z1.h");
  COMPARE(msb(z27.VnS(), p7.Merging(), z29.VnS(), z1.VnS()),
          "msb z27.s, p7/m, z29.s, z1.s");
  COMPARE(msb(z27.VnD(), p7.Merging(), z29.VnD(), z1.VnD()),
          "msb z27.d, p7/m, z29.d, z1.d");

  CLEANUP();
}

TEST(sve_int_mul_add_predicated_macro) {
  SETUP();

  COMPARE_MACRO(Mla(z0.VnB(), p1.Merging(), z0.VnB(), z2.VnB(), z4.VnB()),
                "mla z0.b, p1/m, z2.b, z4.b");
  COMPARE_MACRO(Mla(z3.VnH(), p2.Merging(), z4.VnH(), z3.VnH(), z5.VnH()),
                "mad z3.h, p2/m, z5.h, z4.h");
  COMPARE_MACRO(Mla(z4.VnS(), p3.Merging(), z5.VnS(), z6.VnS(), z4.VnS()),
                "mad z4.s, p3/m, z6.s, z5.s");
  COMPARE_MACRO(Mla(z5.VnD(), p4.Merging(), z6.VnD(), z7.VnD(), z8.VnD()),
                "movprfx z5.d, p4/m, z6.d\n"
                "mla z5.d, p4/m, z7.d, z8.d");

  COMPARE_MACRO(Mls(z0.VnD(), p1.Merging(), z0.VnD(), z2.VnD(), z4.VnD()),
                "mls z0.d, p1/m, z2.d, z4.d");
  COMPARE_MACRO(Mls(z3.VnS(), p2.Merging(), z4.VnS(), z3.VnS(), z5.VnS()),
                "msb z3.s, p2/m, z5.s, z4.s");
  COMPARE_MACRO(Mls(z4.VnH(), p3.Merging(), z5.VnH(), z6.VnH(), z4.VnH()),
                "msb z4.h, p3/m, z6.h, z5.h");
  COMPARE_MACRO(Mls(z5.VnB(), p4.Merging(), z6.VnB(), z7.VnB(), z8.VnB()),
                "movprfx z5.b, p4/m, z6.b\n"
                "mls z5.b, p4/m, z7.b, z8.b");

  CLEANUP();
}

TEST(sve_int_mul_add_unpredicated) {
  SETUP();

  COMPARE(sdot(z13.VnS(), z12.VnB(), z12.VnB()), "sdot z13.s, z12.b, z12.b");
  COMPARE(sdot(z18.VnD(), z27.VnH(), z22.VnH()), "sdot z18.d, z27.h, z22.h");
  COMPARE(udot(z23.VnS(), z22.VnB(), z11.VnB()), "udot z23.s, z22.b, z11.b");
  COMPARE(udot(z21.VnD(), z27.VnH(), z27.VnH()), "udot z21.d, z27.h, z27.h");

  CLEANUP();
}

TEST(sve_int_mul_add_unpredicated_macro) {
  SETUP();

  COMPARE_MACRO(Sdot(z0.VnS(), z0.VnS(), z2.VnB(), z4.VnB()),
                "sdot z0.s, z2.b, z4.b");
  COMPARE_MACRO(Sdot(z3.VnD(), z4.VnD(), z3.VnH(), z5.VnH()),
                "movprfx z31, z4\n"
                "sdot z31.d, z3.h, z5.h\n"
                "mov z3.d, z31.d");
  COMPARE_MACRO(Sdot(z4.VnS(), z5.VnS(), z6.VnB(), z4.VnB()),
                "movprfx z31, z5\n"
                "sdot z31.s, z6.b, z4.b\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Sdot(z6.VnD(), z7.VnD(), z8.VnH(), z9.VnH()),
                "movprfx z6, z7\n"
                "sdot z6.d, z8.h, z9.h");
  COMPARE_MACRO(Sdot(z5.VnD(), z5.VnD(), z5.VnH(), z5.VnH()),
                "sdot z5.d, z5.h, z5.h");

  COMPARE_MACRO(Udot(z0.VnD(), z0.VnD(), z2.VnH(), z4.VnH()),
                "udot z0.d, z2.h, z4.h");
  COMPARE_MACRO(Udot(z3.VnS(), z4.VnS(), z3.VnB(), z5.VnB()),
                "movprfx z31, z4\n"
                "udot z31.s, z3.b, z5.b\n"
                "mov z3.d, z31.d");
  COMPARE_MACRO(Udot(z4.VnD(), z5.VnD(), z6.VnH(), z4.VnH()),
                "movprfx z31, z5\n"
                "udot z31.d, z6.h, z4.h\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Udot(z6.VnS(), z7.VnS(), z8.VnB(), z9.VnB()),
                "movprfx z6, z7\n"
                "udot z6.s, z8.b, z9.b");
  COMPARE_MACRO(Udot(z5.VnS(), z5.VnS(), z5.VnB(), z5.VnB()),
                "udot z5.s, z5.b, z5.b");
  CLEANUP();
}

TEST(sve_int_reduction) {
  SETUP();

  COMPARE(andv(b15, p1, z4.VnB()), "andv b15, p1, z4.b");
  COMPARE(andv(h14, p2, z3.VnH()), "andv h14, p2, z3.h");
  COMPARE(andv(s13, p3, z2.VnS()), "andv s13, p3, z2.s");
  COMPARE(andv(d12, p4, z1.VnD()), "andv d12, p4, z1.d");
  COMPARE(eorv(b12, p0, z30.VnB()), "eorv b12, p0, z30.b");
  COMPARE(eorv(h11, p1, z29.VnH()), "eorv h11, p1, z29.h");
  COMPARE(eorv(s10, p2, z28.VnS()), "eorv s10, p2, z28.s");
  COMPARE(eorv(d9, p3, z27.VnD()), "eorv d9, p3, z27.d");
  COMPARE(movprfx(z30.VnB(), p2.Zeroing(), z23.VnB()),
          "movprfx z30.b, p2/z, z23.b");
  COMPARE(movprfx(z10.VnH(), p0.Merging(), z10.VnH()),
          "movprfx z10.h, p0/m, z10.h");
  COMPARE(movprfx(z0.VnS(), p2.Zeroing(), z23.VnS()),
          "movprfx z0.s, p2/z, z23.s");
  COMPARE(movprfx(z31.VnD(), p7.Merging(), z23.VnD()),
          "movprfx z31.d, p7/m, z23.d");
  COMPARE(orv(b4, p0, z16.VnB()), "orv b4, p0, z16.b");
  COMPARE(orv(h6, p2, z18.VnH()), "orv h6, p2, z18.h");
  COMPARE(orv(s8, p4, z20.VnS()), "orv s8, p4, z20.s");
  COMPARE(orv(d10, p6, z22.VnD()), "orv d10, p6, z22.d");
  COMPARE(saddv(d20, p1, z12.VnB()), "saddv d20, p1, z12.b");
  COMPARE(saddv(d22, p3, z15.VnH()), "saddv d22, p3, z15.h");
  COMPARE(saddv(d24, p5, z18.VnS()), "saddv d24, p5, z18.s");
  COMPARE(smaxv(b9, p3, z1.VnB()), "smaxv b9, p3, z1.b");
  COMPARE(smaxv(h19, p2, z1.VnH()), "smaxv h19, p2, z1.h");
  COMPARE(smaxv(s29, p1, z1.VnS()), "smaxv s29, p1, z1.s");
  COMPARE(smaxv(d9, p0, z1.VnD()), "smaxv d9, p0, z1.d");
  COMPARE(sminv(b8, p3, z14.VnB()), "sminv b8, p3, z14.b");
  COMPARE(sminv(h18, p2, z4.VnH()), "sminv h18, p2, z4.h");
  COMPARE(sminv(s28, p1, z4.VnS()), "sminv s28, p1, z4.s");
  COMPARE(sminv(d8, p0, z24.VnD()), "sminv d8, p0, z24.d");
  COMPARE(uaddv(d13, p0, z15.VnB()), "uaddv d13, p0, z15.b");
  COMPARE(uaddv(d15, p2, z20.VnH()), "uaddv d15, p2, z20.h");
  COMPARE(uaddv(d17, p4, z25.VnS()), "uaddv d17, p4, z25.s");
  COMPARE(uaddv(d19, p6, z30.VnD()), "uaddv d19, p6, z30.d");
  COMPARE(umaxv(b28, p3, z4.VnB()), "umaxv b28, p3, z4.b");
  COMPARE(umaxv(h18, p6, z2.VnH()), "umaxv h18, p6, z2.h");
  COMPARE(umaxv(s18, p4, z29.VnS()), "umaxv s18, p4, z29.s");
  COMPARE(umaxv(d28, p1, z24.VnD()), "umaxv d28, p1, z24.d");
  COMPARE(uminv(b16, p3, z0.VnB()), "uminv b16, p3, z0.b");
  COMPARE(uminv(h16, p2, z3.VnH()), "uminv h16, p2, z3.h");
  COMPARE(uminv(s16, p1, z5.VnS()), "uminv s16, p1, z5.s");
  COMPARE(uminv(d16, p7, z7.VnD()), "uminv d16, p7, z7.d");

  CLEANUP();
}

TEST(sve_int_unary_arithmetic_predicated) {
  SETUP();

  COMPARE(abs(z5.VnB(), p5.Merging(), z31.VnB()), "abs z5.b, p5/m, z31.b");
  COMPARE(abs(z29.VnH(), p5.Merging(), z17.VnH()), "abs z29.h, p5/m, z17.h");
  COMPARE(abs(z6.VnS(), p4.Merging(), z24.VnS()), "abs z6.s, p4/m, z24.s");
  COMPARE(abs(z19.VnD(), p3.Merging(), z25.VnD()), "abs z19.d, p3/m, z25.d");
  COMPARE(cls(z4.VnB(), p0.Merging(), z20.VnB()), "cls z4.b, p0/m, z20.b");
  COMPARE(cls(z11.VnH(), p0.Merging(), z26.VnH()), "cls z11.h, p0/m, z26.h");
  COMPARE(cls(z10.VnS(), p1.Merging(), z10.VnS()), "cls z10.s, p1/m, z10.s");
  COMPARE(cls(z5.VnD(), p1.Merging(), z4.VnD()), "cls z5.d, p1/m, z4.d");
  COMPARE(clz(z18.VnB(), p3.Merging(), z1.VnB()), "clz z18.b, p3/m, z1.b");
  COMPARE(clz(z13.VnH(), p4.Merging(), z18.VnH()), "clz z13.h, p4/m, z18.h");
  COMPARE(clz(z15.VnS(), p4.Merging(), z24.VnS()), "clz z15.s, p4/m, z24.s");
  COMPARE(clz(z29.VnD(), p2.Merging(), z22.VnD()), "clz z29.d, p2/m, z22.d");
  COMPARE(cnot(z16.VnB(), p6.Merging(), z20.VnB()), "cnot z16.b, p6/m, z20.b");
  COMPARE(cnot(z10.VnH(), p5.Merging(), z12.VnH()), "cnot z10.h, p5/m, z12.h");
  COMPARE(cnot(z8.VnS(), p5.Merging(), z21.VnS()), "cnot z8.s, p5/m, z21.s");
  COMPARE(cnot(z3.VnD(), p3.Merging(), z18.VnD()), "cnot z3.d, p3/m, z18.d");
  COMPARE(cnt(z29.VnB(), p3.Merging(), z7.VnB()), "cnt z29.b, p3/m, z7.b");
  COMPARE(cnt(z3.VnH(), p6.Merging(), z31.VnH()), "cnt z3.h, p6/m, z31.h");
  COMPARE(cnt(z2.VnS(), p4.Merging(), z16.VnS()), "cnt z2.s, p4/m, z16.s");
  COMPARE(cnt(z0.VnD(), p0.Merging(), z24.VnD()), "cnt z0.d, p0/m, z24.d");
  COMPARE(fabs(z17.VnH(), p7.Merging(), z15.VnH()), "fabs z17.h, p7/m, z15.h");
  COMPARE(fabs(z18.VnS(), p0.Merging(), z29.VnS()), "fabs z18.s, p0/m, z29.s");
  COMPARE(fabs(z17.VnD(), p1.Merging(), z9.VnD()), "fabs z17.d, p1/m, z9.d");
  COMPARE(fneg(z25.VnH(), p1.Merging(), z28.VnH()), "fneg z25.h, p1/m, z28.h");
  COMPARE(fneg(z5.VnS(), p1.Merging(), z25.VnS()), "fneg z5.s, p1/m, z25.s");
  COMPARE(fneg(z6.VnD(), p1.Merging(), z17.VnD()), "fneg z6.d, p1/m, z17.d");
  COMPARE(neg(z25.VnB(), p4.Merging(), z8.VnB()), "neg z25.b, p4/m, z8.b");
  COMPARE(neg(z30.VnH(), p3.Merging(), z23.VnH()), "neg z30.h, p3/m, z23.h");
  COMPARE(neg(z7.VnS(), p2.Merging(), z26.VnS()), "neg z7.s, p2/m, z26.s");
  COMPARE(neg(z21.VnD(), p3.Merging(), z5.VnD()), "neg z21.d, p3/m, z5.d");
  COMPARE(not_(z24.VnB(), p1.Merging(), z27.VnB()), "not z24.b, p1/m, z27.b");
  COMPARE(not_(z31.VnH(), p6.Merging(), z19.VnH()), "not z31.h, p6/m, z19.h");
  COMPARE(not_(z18.VnS(), p5.Merging(), z13.VnS()), "not z18.s, p5/m, z13.s");
  COMPARE(not_(z12.VnD(), p2.Merging(), z28.VnD()), "not z12.d, p2/m, z28.d");
  COMPARE(sxtb(z19.VnH(), p7.Merging(), z3.VnH()), "sxtb z19.h, p7/m, z3.h");
  COMPARE(sxtb(z3.VnS(), p1.Merging(), z17.VnS()), "sxtb z3.s, p1/m, z17.s");
  COMPARE(sxtb(z27.VnD(), p0.Merging(), z12.VnD()), "sxtb z27.d, p0/m, z12.d");
  COMPARE(sxth(z6.VnS(), p1.Merging(), z17.VnS()), "sxth z6.s, p1/m, z17.s");
  COMPARE(sxth(z8.VnD(), p6.Merging(), z2.VnD()), "sxth z8.d, p6/m, z2.d");
  COMPARE(sxtw(z13.VnD(), p3.Merging(), z27.VnD()), "sxtw z13.d, p3/m, z27.d");
  COMPARE(uxtb(z23.VnH(), p3.Merging(), z21.VnH()), "uxtb z23.h, p3/m, z21.h");
  COMPARE(uxtb(z0.VnS(), p2.Merging(), z13.VnS()), "uxtb z0.s, p2/m, z13.s");
  COMPARE(uxtb(z1.VnD(), p3.Merging(), z13.VnD()), "uxtb z1.d, p3/m, z13.d");
  COMPARE(uxth(z27.VnS(), p0.Merging(), z29.VnS()), "uxth z27.s, p0/m, z29.s");
  COMPARE(uxth(z22.VnD(), p4.Merging(), z20.VnD()), "uxth z22.d, p4/m, z20.d");
  COMPARE(uxtw(z14.VnD(), p1.Merging(), z13.VnD()), "uxtw z14.d, p1/m, z13.d");

  // Check related but undefined encodings.
  COMPARE(dci(0x0410a000), "unallocated (Unallocated)");  // sxtb b
  COMPARE(dci(0x0412a000), "unallocated (Unallocated)");  // sxth b
  COMPARE(dci(0x0452a000), "unallocated (Unallocated)");  // sxth h
  COMPARE(dci(0x0414a000), "unallocated (Unallocated)");  // sxtw b
  COMPARE(dci(0x0454a000), "unallocated (Unallocated)");  // sxtw h
  COMPARE(dci(0x0494a000), "unallocated (Unallocated)");  // sxtw s

  COMPARE(dci(0x0411a000), "unallocated (Unallocated)");  // uxtb b
  COMPARE(dci(0x0413a000), "unallocated (Unallocated)");  // uxth b
  COMPARE(dci(0x0453a000), "unallocated (Unallocated)");  // uxth h
  COMPARE(dci(0x0415a000), "unallocated (Unallocated)");  // uxtw b
  COMPARE(dci(0x0455a000), "unallocated (Unallocated)");  // uxtw h
  COMPARE(dci(0x0495a000), "unallocated (Unallocated)");  // uxtw s

  COMPARE(dci(0x041ca000), "unallocated (Unallocated)");  // fabs b
  COMPARE(dci(0x041da000), "unallocated (Unallocated)");  // fneg b

  CLEANUP();
}

TEST(sve_neg_macro) {
  SETUP();

  COMPARE_MACRO(Neg(z0.VnB(), z0.VnB()), "subr z0.b, z0.b, #0");
  COMPARE_MACRO(Neg(z1.VnH(), z2.VnH()),
                "movprfx z1, z2\n"
                "subr z1.h, z1.h, #0");
  COMPARE_MACRO(Neg(z29.VnS(), z29.VnS()), "subr z29.s, z29.s, #0");
  COMPARE_MACRO(Neg(z30.VnD(), z31.VnD()),
                "movprfx z30, z31\n"
                "subr z30.d, z30.d, #0");

  CLEANUP();
}

TEST(sve_cpy_fcpy_imm) {
  SETUP();

  COMPARE(cpy(z25.VnB(), p13.Zeroing(), -1), "mov z25.b, p13/z, #-1");
  COMPARE(cpy(z25.VnB(), p13.Merging(), -1), "mov z25.b, p13/m, #-1");
  COMPARE(cpy(z25.VnH(), p13.Merging(), 127), "mov z25.h, p13/m, #127");
  COMPARE(cpy(z25.VnS(), p13.Merging(), 10752),
          "mov z25.s, p13/m, #42, lsl #8");
  COMPARE(cpy(z25.VnD(), p13.Merging(), -10752),
          "mov z25.d, p13/m, #-42, lsl #8");
  COMPARE(mov(z25.VnD(), p13.Merging(), -10752),
          "mov z25.d, p13/m, #-42, lsl #8");

  COMPARE(fcpy(z20.VnH(), p11.Merging(), 29.0),
          "fmov z20.h, p11/m, #0x3d (29.0000)");
  COMPARE(fmov(z20.VnS(), p11.Merging(), -31.0),
          "fmov z20.s, p11/m, #0xbf (-31.0000)");
  COMPARE(fcpy(z20.VnD(), p11.Merging(), 1.0),
          "fmov z20.d, p11/m, #0x70 (1.0000)");

  CLEANUP();
}

TEST(sve_fmov_zero) {
  SETUP();

  // Predicated `fmov` is an alias for either `fcpy` or `cpy`.
  COMPARE(fmov(z13.VnS(), p0.Merging(), 1.0),
          "fmov z13.s, p0/m, #0x70 (1.0000)");
  COMPARE(fmov(z13.VnS(), p0.Merging(), 0.0), "mov z13.s, p0/m, #0");
  COMPARE_MACRO(Fmov(z13.VnD(), p0.Merging(), 1.0),
                "fmov z13.d, p0/m, #0x70 (1.0000)");
  COMPARE_MACRO(Fmov(z13.VnD(), p0.Merging(), 0.0), "mov z13.d, p0/m, #0");

  // Unpredicated `fmov` is an alias for either `fdup` or `dup`.
  COMPARE(fmov(z13.VnS(), 1.0), "fmov z13.s, #0x70 (1.0000)");
  COMPARE(fmov(z13.VnS(), 0.0), "mov z13.s, #0");
  COMPARE_MACRO(Fmov(z13.VnD(), 1.0), "fmov z13.d, #0x70 (1.0000)");
  COMPARE_MACRO(Fmov(z13.VnD(), 0.0), "mov z13.d, #0");

  // -0.0 cannot be encoded by this alias, but is handled by the MacroAssembler.
  COMPARE_MACRO(Fmov(z13.VnD(), p0.Merging(), -0.0),
                "mov x16, #0x8000000000000000\n"
                "mov z13.d, p0/m, x16");
  COMPARE_MACRO(Fmov(z13.VnD(), -0.0), "mov z13.d, #0x8000000000000000");

  CLEANUP();
}

TEST(sve_int_wide_imm_unpredicated) {
  SETUP();

  COMPARE(add(z12.VnB(), z12.VnB(), 0), "add z12.b, z12.b, #0");
  COMPARE(add(z13.VnH(), z13.VnH(), 255), "add z13.h, z13.h, #255");
  COMPARE(add(z14.VnS(), z14.VnS(), 256), "add z14.s, z14.s, #1, lsl #8");
  COMPARE(add(z15.VnD(), z15.VnD(), 255 * 256),
          "add z15.d, z15.d, #255, lsl #8");

  COMPARE(dup(z6.VnB(), -128), "mov z6.b, #-128");
  COMPARE(dup(z7.VnH(), 127), "mov z7.h, #127");
  COMPARE(dup(z8.VnS(), -128 * 256), "mov z8.s, #-128, lsl #8");
  COMPARE(dup(z9.VnD(), 127 * 256), "mov z9.d, #127, lsl #8");
  COMPARE(mov(z8.VnS(), -128 * 256, -1), "mov z8.s, #-128, lsl #8");
  COMPARE(mov(z9.VnD(), 127 * 256, -1), "mov z9.d, #127, lsl #8");

  COMPARE(sqadd(z7.VnB(), z7.VnB(), 124), "sqadd z7.b, z7.b, #124");
  COMPARE(sqadd(z8.VnH(), z8.VnH(), 131), "sqadd z8.h, z8.h, #131");
  COMPARE(sqadd(z9.VnS(), z9.VnS(), 252 * 256),
          "sqadd z9.s, z9.s, #252, lsl #8");
  COMPARE(sqadd(z10.VnD(), z10.VnD(), 20 * 256),
          "sqadd z10.d, z10.d, #20, lsl #8");

  COMPARE(sqsub(z31.VnB(), z31.VnB(), 132), "sqsub z31.b, z31.b, #132");
  COMPARE(sqsub(z30.VnH(), z30.VnH(), 251), "sqsub z30.h, z30.h, #251");
  COMPARE(sqsub(z29.VnS(), z29.VnS(), 21 * 256),
          "sqsub z29.s, z29.s, #21, lsl #8");
  COMPARE(sqsub(z28.VnD(), z28.VnD(), 123 * 256),
          "sqsub z28.d, z28.d, #123, lsl #8");

  COMPARE(subr(z20.VnB(), z20.VnB(), 250), "subr z20.b, z20.b, #250");
  COMPARE(subr(z21.VnH(), z21.VnH(), 22), "subr z21.h, z21.h, #22");
  COMPARE(subr(z22.VnS(), z22.VnS(), 122 * 256),
          "subr z22.s, z22.s, #122, lsl #8");
  COMPARE(subr(z23.VnD(), z23.VnD(), 133 * 256),
          "subr z23.d, z23.d, #133, lsl #8");

  COMPARE(sub(z18.VnB(), z18.VnB(), 23), "sub z18.b, z18.b, #23");
  COMPARE(sub(z19.VnH(), z19.VnH(), 121), "sub z19.h, z19.h, #121");
  COMPARE(sub(z20.VnS(), z20.VnS(), 134 * 256),
          "sub z20.s, z20.s, #134, lsl #8");
  COMPARE(sub(z21.VnD(), z21.VnD(), 249 * 256),
          "sub z21.d, z21.d, #249, lsl #8");

  COMPARE(uqadd(z21.VnB(), z21.VnB(), 246), "uqadd z21.b, z21.b, #246");
  COMPARE(uqadd(z22.VnH(), z22.VnH(), 26), "uqadd z22.h, z22.h, #26");
  COMPARE(uqadd(z23.VnS(), z23.VnS(), 118 * 256),
          "uqadd z23.s, z23.s, #118, lsl #8");
  COMPARE(uqadd(z24.VnD(), z24.VnD(), 137 * 256),
          "uqadd z24.d, z24.d, #137, lsl #8");

  COMPARE(uqsub(z10.VnB(), z10.VnB(), 27), "uqsub z10.b, z10.b, #27");
  COMPARE(uqsub(z11.VnH(), z11.VnH(), 117), "uqsub z11.h, z11.h, #117");
  COMPARE(uqsub(z12.VnS(), z12.VnS(), 138 * 256),
          "uqsub z12.s, z12.s, #138, lsl #8");
  COMPARE(uqsub(z13.VnD(), z13.VnD(), 245 * 256),
          "uqsub z13.d, z13.d, #245, lsl #8");

  COMPARE(fdup(z26.VnH(), Float16(-5.0f)), "fmov z26.h, #0x94 (-5.0000)");
  COMPARE(fdup(z27.VnS(), -13.0f), "fmov z27.s, #0xaa (-13.0000)");
  COMPARE(fdup(z28.VnD(), 1.0f), "fmov z28.d, #0x70 (1.0000)");
  COMPARE(fmov(z28.VnD(), 1.0f), "fmov z28.d, #0x70 (1.0000)");

  COMPARE(mul(z15.VnB(), z15.VnB(), -128), "mul z15.b, z15.b, #-128");
  COMPARE(mul(z16.VnH(), z16.VnH(), -1), "mul z16.h, z16.h, #-1");
  COMPARE(mul(z17.VnS(), z17.VnS(), 17), "mul z17.s, z17.s, #17");
  COMPARE(mul(z18.VnD(), z18.VnD(), 127), "mul z18.d, z18.d, #127");

  COMPARE(smax(z7.VnB(), z7.VnB(), -2), "smax z7.b, z7.b, #-2");
  COMPARE(smax(z8.VnH(), z8.VnH(), 18), "smax z8.h, z8.h, #18");
  COMPARE(smax(z9.VnS(), z9.VnS(), 126), "smax z9.s, z9.s, #126");
  COMPARE(smax(z10.VnD(), z10.VnD(), -127), "smax z10.d, z10.d, #-127");

  COMPARE(smin(z5.VnB(), z5.VnB(), 19), "smin z5.b, z5.b, #19");
  COMPARE(smin(z6.VnH(), z6.VnH(), 125), "smin z6.h, z6.h, #125");
  COMPARE(smin(z7.VnS(), z7.VnS(), -126), "smin z7.s, z7.s, #-126");
  COMPARE(smin(z8.VnD(), z8.VnD(), -3), "smin z8.d, z8.d, #-3");

  COMPARE(umax(z15.VnB(), z15.VnB(), 120), "umax z15.b, z15.b, #120");
  COMPARE(umax(z16.VnH(), z16.VnH(), 135), "umax z16.h, z16.h, #135");
  COMPARE(umax(z17.VnS(), z17.VnS(), 248), "umax z17.s, z17.s, #248");
  COMPARE(umax(z18.VnD(), z18.VnD(), 24), "umax z18.d, z18.d, #24");

  COMPARE(umin(z22.VnB(), z22.VnB(), 136), "umin z22.b, z22.b, #136");
  COMPARE(umin(z23.VnH(), z23.VnH(), 247), "umin z23.h, z23.h, #247");
  COMPARE(umin(z24.VnS(), z24.VnS(), 25), "umin z24.s, z24.s, #25");
  COMPARE(umin(z25.VnD(), z25.VnD(), 119), "umin z25.d, z25.d, #119");

  CLEANUP();
}

TEST(sve_add_sub_imm_macro) {
  SETUP();

  // The MacroAssembler automatically generates movprfx where necessary.
  COMPARE_MACRO(Add(z12.VnB(), z13.VnB(), 0),
                "movprfx z12, z13\n"
                "add z12.b, z12.b, #0");
  COMPARE_MACRO(Sub(z20.VnB(), 250, z2.VnB()),
                "movprfx z20, z2\n"
                "subr z20.b, z20.b, #250");
  COMPARE_MACRO(Sub(z19.VnH(), z4.VnH(), 121),
                "movprfx z19, z4\n"
                "sub z19.h, z19.h, #121");

  // Add and Sub can make use of two's complement equivalences.
  COMPARE_MACRO(Add(z13.VnH(), z13.VnH(), 0xffff), "sub z13.h, z13.h, #1");
  COMPARE_MACRO(Add(z15.VnD(), z15.VnD(), 0xffffffffffffffd6),
                "sub z15.d, z15.d, #42");
  COMPARE_MACRO(Add(z16.VnH(), z16.VnH(), 0xff00),
                "add z16.h, z16.h, #255, lsl #8");
  COMPARE_MACRO(Sub(z17.VnH(), z17.VnH(), 0xfffe), "add z17.h, z17.h, #2");
  COMPARE_MACRO(Sub(z14.VnB(), z14.VnB(), 0x80), "sub z14.b, z14.b, #128");

  // The MacroAssembler automatically generates dup if an immediate isn't
  // encodable.
  COMPARE_MACRO(Add(z15.VnD(), z20.VnD(), 1234567890),
                "mov x16, #0x2d2\n"
                "movk x16, #0x4996, lsl #16\n"
                "mov z31.d, x16\n"
                "add z15.d, z20.d, z31.d");
  COMPARE_MACRO(Sub(z22.VnS(), 256 * 256, z2.VnS()),
                "mov z31.s, #0x10000\n"
                "sub z22.s, z31.s, z2.s");
  COMPARE_MACRO(Sub(z21.VnD(), z11.VnD(), 111111111111),
                "mov x16, #0x1c7\n"
                "movk x16, #0xdebd, lsl #16\n"
                "movk x16, #0x19, lsl #32\n"
                "mov z31.d, x16\n"
                "sub z21.d, z11.d, z31.d");

  CLEANUP();
}

TEST(sve_uqadd_uqsub_imm_macro) {
  SETUP();

  // The MacroAssembler automatically generates movprfx where necessary.
  COMPARE_MACRO(Uqadd(z21.VnB(), z14.VnB(), 246),
                "movprfx z21, z14\n"
                "uqadd z21.b, z21.b, #246");
  COMPARE_MACRO(Uqsub(z10.VnB(), z27.VnB(), 27),
                "movprfx z10, z27\n"
                "uqsub z10.b, z10.b, #27");
  COMPARE_MACRO(Uqadd(z1.VnS(), z2.VnS(), 42 * 256),
                "movprfx z1, z2\n"
                "uqadd z1.s, z1.s, #42, lsl #8");

  COMPARE_MACRO(Uqsub(z3.VnB(), z3.VnB(), 0xff), "uqsub z3.b, z3.b, #255");
  COMPARE_MACRO(Uqadd(z8.VnS(), z8.VnS(), 0xff00),
                "uqadd z8.s, z8.s, #255, lsl #8");

  CLEANUP();
}

TEST(sve_sqadd_sqsub_imm_macro) {
  SETUP();

  // The MacroAssembler automatically generates movprfx where necessary.
  COMPARE_MACRO(Sqadd(z21.VnB(), z14.VnB(), 123),
                "movprfx z21, z14\n"
                "sqadd z21.b, z21.b, #123");
  COMPARE_MACRO(Sqsub(z10.VnB(), z27.VnB(), 27),
                "movprfx z10, z27\n"
                "sqsub z10.b, z10.b, #27");
  COMPARE_MACRO(Sqadd(z22.VnS(), z15.VnS(), 256),
                "movprfx z22, z15\n"
                "sqadd z22.s, z22.s, #1, lsl #8");

  COMPARE_MACRO(Sqsub(z3.VnB(), z3.VnB(), 0xff), "sqsub z3.b, z3.b, #255");
  COMPARE_MACRO(Sqadd(z4.VnH(), z4.VnH(), 0xff00),
                "sqadd z4.h, z4.h, #255, lsl #8");

  CLEANUP();
}

TEST(sve_int_wide_imm_unpredicated_macro) {
  SETUP();

  // The MacroAssembler automatically generates movprfx where it can.
  COMPARE_MACRO(Mul(z1.VnD(), z18.VnD(), 127),
                "movprfx z1, z18\n"
                "mul z1.d, z1.d, #127");
  COMPARE_MACRO(Smax(z3.VnS(), z9.VnS(), 126),
                "movprfx z3, z9\n"
                "smax z3.s, z3.s, #126");
  COMPARE_MACRO(Smin(z26.VnH(), z6.VnH(), 125),
                "movprfx z26, z6\n"
                "smin z26.h, z26.h, #125");
  COMPARE_MACRO(Umax(z25.VnB(), z15.VnB(), 120),
                "movprfx z25, z15\n"
                "umax z25.b, z25.b, #120");
  COMPARE_MACRO(Umin(z13.VnD(), z25.VnD(), 119),
                "movprfx z13, z25\n"
                "umin z13.d, z13.d, #119");
  COMPARE_MACRO(Dup(z8.VnS(), -7654321),
                "mov w16, #0x344f\n"
                "movk w16, #0xff8b, lsl #16\n"
                "mov z8.s, w16");

  // The MacroAssembler automatically generates dup if an immediate isn't
  // encodable, when it is out-of-range for example.
  COMPARE_MACRO(Dup(z9.VnD(), 0x80000000), "mov z9.d, #0x80000000");
  COMPARE_MACRO(Mov(z9.VnD(), 0x80000000), "mov z9.d, #0x80000000");
  COMPARE_MACRO(Fdup(z26.VnH(), Float16(0.0)), "mov z26.h, #0");
  COMPARE_MACRO(Fdup(z26.VnH(), Float16(0.0)), "mov z26.h, #0");
  COMPARE_MACRO(Fdup(z27.VnS(), 255.0f),
                "mov w16, #0x437f0000\n"
                "mov z27.s, w16");
  COMPARE_MACRO(Fdup(z28.VnD(), 12.3456),
                "mov x16, #0xfec5\n"
                "movk x16, #0x7bb2, lsl #16\n"
                "movk x16, #0xb0f2, lsl #32\n"
                "movk x16, #0x4028, lsl #48\n"
                "mov z28.d, x16");
  COMPARE_MACRO(Fmov(z26.VnH(), Float16(0.0)), "mov z26.h, #0");
  COMPARE_MACRO(Fmov(z26.VnH(), Float16(0.0)), "mov z26.h, #0");
  COMPARE_MACRO(Fmov(z27.VnS(), 255.0f),
                "mov w16, #0x437f0000\n"
                "mov z27.s, w16");
  COMPARE_MACRO(Fmov(z28.VnD(), 12.3456),
                "mov x16, #0xfec5\n"
                "movk x16, #0x7bb2, lsl #16\n"
                "movk x16, #0xb0f2, lsl #32\n"
                "movk x16, #0x4028, lsl #48\n"
                "mov z28.d, x16");

  // Only predicated version of instruction is supported for unencodable
  // immediate.
  {
    UseScratchRegisterScope temps(&masm);
    temps.Include(p7, p15);
    COMPARE_MACRO(Mul(z18.VnD(), z18.VnD(), -1270000000),
                  "ptrue p7.d\n"
                  "mov x16, #0xffffffffffff5680\n"
                  "movk x16, #0xb44d, lsl #16\n"
                  "mov z31.d, x16\n"
                  "mul z18.d, p7/m, z18.d, z31.d");
    COMPARE_MACRO(Smax(z9.VnS(), z11.VnS(), -0x70000001),
                  "ptrue p7.s\n"
                  "mov z9.s, #0x8fffffff\n"
                  "smax z9.s, p7/m, z9.s, z11.s");
    COMPARE_MACRO(Smin(z6.VnH(), z6.VnH(), -0x7eef),
                  "ptrue p7.h\n"
                  "mov w16, #0xffff8111\n"
                  "mov z31.h, w16\n"
                  "smin z6.h, p7/m, z6.h, z31.h");
    COMPARE_MACRO(Umax(z15.VnH(), z7.VnH(), 0xfeee),
                  "ptrue p7.h\n"
                  "mov w16, #0xfeee\n"
                  "mov z15.h, w16\n"
                  "umax z15.h, p7/m, z15.h, z7.h");
    COMPARE_MACRO(Umin(z25.VnD(), z25.VnD(), 123123123),
                  "ptrue p7.d\n"
                  "mov x16, #0xb5b3\n"
                  "movk x16, #0x756, lsl #16\n"
                  "mov z31.d, x16\n"
                  "umin z25.d, p7/m, z25.d, z31.d");
  }
}

TEST(sve_mem_32bit_gather_vector_plus_immediate_macro) {
  SETUP();

  // Simple cases.
  COMPARE_MACRO(Ld1b(z4.VnS(), p4.Zeroing(), SVEMemOperand(z12.VnS(), 31)),
                "ld1b {z4.s}, p4/z, [z12.s, #31]");
  COMPARE_MACRO(Ld1h(z10.VnS(), p6.Zeroing(), SVEMemOperand(z4.VnS(), 10)),
                "ld1h {z10.s}, p6/z, [z4.s, #10]");
  COMPARE_MACRO(Ld1w(z16.VnS(), p0.Zeroing(), SVEMemOperand(z26.VnS(), 124)),
                "ld1w {z16.s}, p0/z, [z26.s, #124]");
  COMPARE_MACRO(Ld1sb(z9.VnS(), p3.Zeroing(), SVEMemOperand(z22.VnS())),
                "ld1sb {z9.s}, p3/z, [z22.s]");
  COMPARE_MACRO(Ld1sh(z22.VnS(), p1.Zeroing(), SVEMemOperand(z9.VnS(), 62)),
                "ld1sh {z22.s}, p1/z, [z9.s, #62]");
  COMPARE_MACRO(Ldff1b(z17.VnS(), p2.Zeroing(), SVEMemOperand(z29.VnS(), 0)),
                "ldff1b {z17.s}, p2/z, [z29.s]");
  COMPARE_MACRO(Ldff1h(z16.VnS(), p3.Zeroing(), SVEMemOperand(z15.VnS())),
                "ldff1h {z16.s}, p3/z, [z15.s]");
  COMPARE_MACRO(Ldff1w(z7.VnS(), p3.Zeroing(), SVEMemOperand(z20.VnS(), 4)),
                "ldff1w {z7.s}, p3/z, [z20.s, #4]");
  COMPARE_MACRO(Ldff1sb(z7.VnS(), p1.Zeroing(), SVEMemOperand(z10.VnS(), 21)),
                "ldff1sb {z7.s}, p1/z, [z10.s, #21]");
  COMPARE_MACRO(Ldff1sh(z0.VnS(), p1.Zeroing(), SVEMemOperand(z23.VnS(), 42)),
                "ldff1sh {z0.s}, p1/z, [z23.s, #42]");

  // Unencodable cases use a scalar-plus-vector form.
  COMPARE_MACRO(Ld1b(z17.VnS(), p2.Zeroing(), SVEMemOperand(z11.VnS(), 32)),
                "mov x16, #0x20\n"
                "ld1b {z17.s}, p2/z, [x16, z11.s, uxtw]");
  COMPARE_MACRO(Ld1h(z11.VnS(), p1.Zeroing(), SVEMemOperand(z8.VnS(), -2)),
                "mov x16, #0xfffffffffffffffe\n"
                "ld1h {z11.s}, p1/z, [x16, z8.s, uxtw]");
  COMPARE_MACRO(Ld1w(z5.VnS(), p6.Zeroing(), SVEMemOperand(z9.VnS(), 42)),
                "mov x16, #0x2a\n"
                "ld1w {z5.s}, p6/z, [x16, z9.s, uxtw]");
  COMPARE_MACRO(Ld1sb(z28.VnS(), p5.Zeroing(), SVEMemOperand(z12.VnS(), -1)),
                "mov x16, #0xffffffffffffffff\n"
                "ld1sb {z28.s}, p5/z, [x16, z12.s, uxtw]");
  COMPARE_MACRO(Ld1sh(z30.VnS(), p5.Zeroing(), SVEMemOperand(z21.VnS(), 64)),
                "mov x16, #0x40\n"
                "ld1sh {z30.s}, p5/z, [x16, z21.s, uxtw]");
  COMPARE_MACRO(Ldff1b(z19.VnS(), p6.Zeroing(), SVEMemOperand(z24.VnS(), 32)),
                "mov x16, #0x20\n"
                "ldff1b {z19.s}, p6/z, [x16, z24.s, uxtw]");
  COMPARE_MACRO(Ldff1h(z5.VnS(), p1.Zeroing(), SVEMemOperand(z24.VnS(), -2)),
                "mov x16, #0xfffffffffffffffe\n"
                "ldff1h {z5.s}, p1/z, [x16, z24.s, uxtw]");
  COMPARE_MACRO(Ldff1w(z17.VnS(), p6.Zeroing(), SVEMemOperand(z18.VnS(), 42)),
                "mov x16, #0x2a\n"
                "ldff1w {z17.s}, p6/z, [x16, z18.s, uxtw]");
  COMPARE_MACRO(Ldff1sb(z31.VnS(), p5.Zeroing(), SVEMemOperand(z21.VnS(), -1)),
                "mov x16, #0xffffffffffffffff\n"
                "ldff1sb {z31.s}, p5/z, [x16, z21.s, uxtw]");
  COMPARE_MACRO(Ldff1sh(z6.VnS(), p5.Zeroing(), SVEMemOperand(z22.VnS(), 64)),
                "mov x16, #0x40\n"
                "ldff1sh {z6.s}, p5/z, [x16, z22.s, uxtw]");

  CLEANUP();
}

TEST(sve_mem_32bit_gather_and_unsized_contiguous) {
  SETUP();

  // 32-bit gather load in scalar-plus-vector vform with unscaled offset.
  COMPARE(ld1b(z9.VnS(), p5.Zeroing(), SVEMemOperand(x2, z1.VnS(), SXTW)),
          "ld1b {z9.s}, p5/z, [x2, z1.s, sxtw]");
  COMPARE(ld1b(z9.VnS(), p5.Zeroing(), SVEMemOperand(sp, z1.VnS(), UXTW)),
          "ld1b {z9.s}, p5/z, [sp, z1.s, uxtw]");
  COMPARE(ld1h(z17.VnS(), p2.Zeroing(), SVEMemOperand(x11, z24.VnS(), SXTW)),
          "ld1h {z17.s}, p2/z, [x11, z24.s, sxtw]");
  COMPARE(ld1w(z22.VnS(), p6.Zeroing(), SVEMemOperand(sp, z5.VnS(), UXTW)),
          "ld1w {z22.s}, p6/z, [sp, z5.s, uxtw]");
  COMPARE(ld1sb(z12.VnS(), p7.Zeroing(), SVEMemOperand(x17, z23.VnS(), UXTW)),
          "ld1sb {z12.s}, p7/z, [x17, z23.s, uxtw]");
  COMPARE(ld1sb(z22.VnS(), p3.Zeroing(), SVEMemOperand(x23, z23.VnS(), SXTW)),
          "ld1sb {z22.s}, p3/z, [x23, z23.s, sxtw]");
  COMPARE(ld1sh(z11.VnS(), p2.Zeroing(), SVEMemOperand(x18, z10.VnS(), UXTW)),
          "ld1sh {z11.s}, p2/z, [x18, z10.s, uxtw]");

  // 32-bit gather load in scalar-plus-vector vform with scaled offset.
  COMPARE(ld1h(z9.VnS(), p3.Zeroing(), SVEMemOperand(sp, z4.VnS(), UXTW, 1)),
          "ld1h {z9.s}, p3/z, [sp, z4.s, uxtw #1]");
  COMPARE(ld1w(z0.VnS(), p6.Zeroing(), SVEMemOperand(x28, z21.VnS(), SXTW, 2)),
          "ld1w {z0.s}, p6/z, [x28, z21.s, sxtw #2]");
  COMPARE(ld1sh(z11.VnS(), p4.Zeroing(), SVEMemOperand(sp, z0.VnS(), SXTW, 1)),
          "ld1sh {z11.s}, p4/z, [sp, z0.s, sxtw #1]");

  // 32-bit gather first-fault load in scalar-plus-vector vform with 32-bit
  // unpacked unscaled offset.
  COMPARE(ldff1b(z18.VnS(), p6.Zeroing(), SVEMemOperand(x27, z24.VnS(), UXTW)),
          "ldff1b {z18.s}, p6/z, [x27, z24.s, uxtw]");
  COMPARE(ldff1h(z28.VnS(), p6.Zeroing(), SVEMemOperand(x1, z30.VnS(), UXTW)),
          "ldff1h {z28.s}, p6/z, [x1, z30.s, uxtw]");
  COMPARE(ldff1w(z12.VnS(), p3.Zeroing(), SVEMemOperand(x25, z27.VnS(), SXTW)),
          "ldff1w {z12.s}, p3/z, [x25, z27.s, sxtw]");
  COMPARE(ldff1sb(z15.VnS(), p5.Zeroing(), SVEMemOperand(x5, z14.VnS(), SXTW)),
          "ldff1sb {z15.s}, p5/z, [x5, z14.s, sxtw]");
  COMPARE(ldff1sh(z18.VnS(), p4.Zeroing(), SVEMemOperand(x25, z25.VnS(), SXTW)),
          "ldff1sh {z18.s}, p4/z, [x25, z25.s, sxtw]");

  // 32-bit gather first-fault load in scalar-plus-vector vform with 32-bit
  // scaled offset.
  COMPARE(ldff1h(z25.VnS(),
                 p3.Zeroing(),
                 SVEMemOperand(x17, z15.VnS(), SXTW, 1)),
          "ldff1h {z25.s}, p3/z, [x17, z15.s, sxtw #1]");
  COMPARE(ldff1w(z5.VnS(),
                 p4.Zeroing(),
                 SVEMemOperand(x23, z31.VnS(), UXTW, 2)),
          "ldff1w {z5.s}, p4/z, [x23, z31.s, uxtw #2]");
  COMPARE(ldff1sh(z10.VnS(),
                  p0.Zeroing(),
                  SVEMemOperand(x19, z15.VnS(), UXTW, 1)),
          "ldff1sh {z10.s}, p0/z, [x19, z15.s, uxtw #1]");

  // Load and broadcast data to vector.
  COMPARE(ld1rb(z2.VnH(), p0.Zeroing(), SVEMemOperand(x30, 0)),
          "ld1rb {z2.h}, p0/z, [x30]");
  COMPARE(ld1rb(z14.VnS(), p2.Zeroing(), SVEMemOperand(x11, 63)),
          "ld1rb {z14.s}, p2/z, [x11, #63]");
  COMPARE(ld1rb(z27.VnD(), p1.Zeroing(), SVEMemOperand(x29, 2)),
          "ld1rb {z27.d}, p1/z, [x29, #2]");
  COMPARE(ld1rb(z0.VnB(), p3.Zeroing(), SVEMemOperand(sp, 59)),
          "ld1rb {z0.b}, p3/z, [sp, #59]");
  COMPARE(ld1rh(z19.VnH(), p5.Zeroing(), SVEMemOperand(x1, 0)),
          "ld1rh {z19.h}, p5/z, [x1]");
  COMPARE(ld1rh(z4.VnS(), p7.Zeroing(), SVEMemOperand(x29, 126)),
          "ld1rh {z4.s}, p7/z, [x29, #126]");
  COMPARE(ld1rh(z24.VnD(), p0.Zeroing(), SVEMemOperand(sp, 78)),
          "ld1rh {z24.d}, p0/z, [sp, #78]");
  COMPARE(ld1rw(z19.VnS(), p5.Zeroing(), SVEMemOperand(x4, 252)),
          "ld1rw {z19.s}, p5/z, [x4, #252]");
  COMPARE(ld1rw(z13.VnD(), p3.Zeroing(), SVEMemOperand(x2, 100)),
          "ld1rw {z13.d}, p3/z, [x2, #100]");
  COMPARE(ld1rd(z19.VnD(), p7.Zeroing(), SVEMemOperand(x14, 504)),
          "ld1rd {z19.d}, p7/z, [x14, #504]");
  COMPARE(ld1rsb(z16.VnH(), p1.Zeroing(), SVEMemOperand(x29, 0)),
          "ld1rsb {z16.h}, p1/z, [x29]");
  COMPARE(ld1rsb(z8.VnS(), p6.Zeroing(), SVEMemOperand(sp, 33)),
          "ld1rsb {z8.s}, p6/z, [sp, #33]");
  COMPARE(ld1rsb(z25.VnD(), p2.Zeroing(), SVEMemOperand(x18, 63)),
          "ld1rsb {z25.d}, p2/z, [x18, #63]");
  COMPARE(ld1rsh(z11.VnS(), p5.Zeroing(), SVEMemOperand(x14, 2)),
          "ld1rsh {z11.s}, p5/z, [x14, #2]");
  COMPARE(ld1rsh(z28.VnD(), p1.Zeroing(), SVEMemOperand(x19, 124)),
          "ld1rsh {z28.d}, p1/z, [x19, #124]");
  COMPARE(ld1rsw(z23.VnD(), p4.Zeroing(), SVEMemOperand(x10, 8)),
          "ld1rsw {z23.d}, p4/z, [x10, #8]");

  CLEANUP();
}

TEST(sve_mem_32bit_gather_and_unsized_contiguous_macro) {
  SETUP();

  COMPARE_MACRO(Ld1rb(z2.VnB(), p0.Zeroing(), SVEMemOperand(x30, 100)),
                "add x16, x30, #0x64 (100)\n"
                "ld1rb {z2.b}, p0/z, [x16]");
  COMPARE_MACRO(Ld1rh(z4.VnH(), p1.Zeroing(), SVEMemOperand(x21, 201)),
                "add x16, x21, #0xc9 (201)\n"
                "ld1rh {z4.h}, p1/z, [x16]");
  COMPARE_MACRO(Ld1rw(z6.VnS(), p2.Zeroing(), SVEMemOperand(x14, 512)),
                "add x16, x14, #0x200 (512)\n"
                "ld1rw {z6.s}, p2/z, [x16]");
  COMPARE_MACRO(Ld1rd(z8.VnD(), p3.Zeroing(), SVEMemOperand(x3, 1024)),
                "add x16, x3, #0x400 (1024)\n"
                "ld1rd {z8.d}, p3/z, [x16]");
  COMPARE_MACRO(Ld1rsb(z10.VnH(), p4.Zeroing(), SVEMemOperand(sp, -100)),
                "sub x16, sp, #0x64 (100)\n"
                "ld1rsb {z10.h}, p4/z, [x16]");
  COMPARE_MACRO(Ld1rsh(z12.VnS(), p5.Zeroing(), SVEMemOperand(x30, -255)),
                "sub x16, x30, #0xff (255)\n"
                "ld1rsh {z12.s}, p5/z, [x16]");
  COMPARE_MACRO(Ld1rsw(z14.VnD(), p6.Zeroing(), SVEMemOperand(x1, -1024)),
                "sub x16, x1, #0x400 (1024)\n"
                "ld1rsw {z14.d}, p6/z, [x16]");
}

TEST(sve_mem_64bit_gather_vector_plus_immediate) {
  SETUP();

  COMPARE(ld1b(z2.VnD(), p2.Zeroing(), SVEMemOperand(z12.VnD(), 31)),
          "ld1b {z2.d}, p2/z, [z12.d, #31]");
  COMPARE(ld1h(z30.VnD(), p7.Zeroing(), SVEMemOperand(z28.VnD(), 10)),
          "ld1h {z30.d}, p7/z, [z28.d, #10]");
  COMPARE(ld1w(z10.VnD(), p5.Zeroing(), SVEMemOperand(z4.VnD(), 124)),
          "ld1w {z10.d}, p5/z, [z4.d, #124]");
  COMPARE(ld1d(z13.VnD(), p3.Zeroing(), SVEMemOperand(z19.VnD(), 248)),
          "ld1d {z13.d}, p3/z, [z19.d, #248]");
  COMPARE(ld1sb(z16.VnD(), p7.Zeroing(), SVEMemOperand(z31.VnD())),
          "ld1sb {z16.d}, p7/z, [z31.d]");
  COMPARE(ld1sh(z20.VnD(), p2.Zeroing(), SVEMemOperand(z2.VnD(), 62)),
          "ld1sh {z20.d}, p2/z, [z2.d, #62]");
  COMPARE(ld1sw(z2.VnD(), p7.Zeroing(), SVEMemOperand(z25.VnD())),
          "ld1sw {z2.d}, p7/z, [z25.d]");
  COMPARE(ldff1b(z24.VnD(), p5.Zeroing(), SVEMemOperand(z8.VnD(), 0)),
          "ldff1b {z24.d}, p5/z, [z8.d]");
  COMPARE(ldff1h(z9.VnD(), p3.Zeroing(), SVEMemOperand(z19.VnD())),
          "ldff1h {z9.d}, p3/z, [z19.d]");
  COMPARE(ldff1w(z26.VnD(), p6.Zeroing(), SVEMemOperand(z15.VnD(), 4)),
          "ldff1w {z26.d}, p6/z, [z15.d, #4]");
  COMPARE(ldff1d(z19.VnD(), p1.Zeroing(), SVEMemOperand(z14.VnD())),
          "ldff1d {z19.d}, p1/z, [z14.d]");
  COMPARE(ldff1sb(z26.VnD(), p5.Zeroing(), SVEMemOperand(z14.VnD(), 21)),
          "ldff1sb {z26.d}, p5/z, [z14.d, #21]");
  COMPARE(ldff1sh(z6.VnD(), p3.Zeroing(), SVEMemOperand(z19.VnD(), 42)),
          "ldff1sh {z6.d}, p3/z, [z19.d, #42]");
  COMPARE(ldff1sw(z19.VnD(), p7.Zeroing(), SVEMemOperand(z14.VnD(), 84)),
          "ldff1sw {z19.d}, p7/z, [z14.d, #84]");

  CLEANUP();
}

TEST(sve_mem_64bit_gather_vector_plus_immediate_macro) {
  SETUP();

  // Simple cases.
  COMPARE_MACRO(Ld1b(z18.VnD(), p6.Zeroing(), SVEMemOperand(z31.VnD(), 31)),
                "ld1b {z18.d}, p6/z, [z31.d, #31]");
  COMPARE_MACRO(Ld1h(z5.VnD(), p3.Zeroing(), SVEMemOperand(z18.VnD(), 10)),
                "ld1h {z5.d}, p3/z, [z18.d, #10]");
  COMPARE_MACRO(Ld1w(z0.VnD(), p6.Zeroing(), SVEMemOperand(z22.VnD(), 124)),
                "ld1w {z0.d}, p6/z, [z22.d, #124]");
  COMPARE_MACRO(Ld1d(z18.VnD(), p3.Zeroing(), SVEMemOperand(z19.VnD(), 248)),
                "ld1d {z18.d}, p3/z, [z19.d, #248]");
  COMPARE_MACRO(Ld1sb(z18.VnD(), p6.Zeroing(), SVEMemOperand(z17.VnD())),
                "ld1sb {z18.d}, p6/z, [z17.d]");
  COMPARE_MACRO(Ld1sh(z14.VnD(), p3.Zeroing(), SVEMemOperand(z11.VnD(), 62)),
                "ld1sh {z14.d}, p3/z, [z11.d, #62]");
  COMPARE_MACRO(Ld1sw(z18.VnD(), p5.Zeroing(), SVEMemOperand(z14.VnD())),
                "ld1sw {z18.d}, p5/z, [z14.d]");
  COMPARE_MACRO(Ldff1b(z20.VnD(), p5.Zeroing(), SVEMemOperand(z17.VnD(), 0)),
                "ldff1b {z20.d}, p5/z, [z17.d]");
  COMPARE_MACRO(Ldff1h(z20.VnD(), p5.Zeroing(), SVEMemOperand(z16.VnD())),
                "ldff1h {z20.d}, p5/z, [z16.d]");
  COMPARE_MACRO(Ldff1w(z1.VnD(), p6.Zeroing(), SVEMemOperand(z16.VnD(), 4)),
                "ldff1w {z1.d}, p6/z, [z16.d, #4]");
  COMPARE_MACRO(Ldff1d(z16.VnD(), p1.Zeroing(), SVEMemOperand(z3.VnD())),
                "ldff1d {z16.d}, p1/z, [z3.d]");
  COMPARE_MACRO(Ldff1sb(z26.VnD(), p7.Zeroing(), SVEMemOperand(z3.VnD(), 21)),
                "ldff1sb {z26.d}, p7/z, [z3.d, #21]");
  COMPARE_MACRO(Ldff1sh(z1.VnD(), p7.Zeroing(), SVEMemOperand(z9.VnD(), 42)),
                "ldff1sh {z1.d}, p7/z, [z9.d, #42]");
  COMPARE_MACRO(Ldff1sw(z19.VnD(), p4.Zeroing(), SVEMemOperand(z3.VnD(), 84)),
                "ldff1sw {z19.d}, p4/z, [z3.d, #84]");

  // Unencodable cases use a scalar-plus-vector form.
  COMPARE_MACRO(Ld1b(z23.VnD(), p6.Zeroing(), SVEMemOperand(z16.VnD(), 32)),
                "mov x16, #0x20\n"
                "ld1b {z23.d}, p6/z, [x16, z16.d]");
  COMPARE_MACRO(Ld1h(z10.VnD(), p6.Zeroing(), SVEMemOperand(z11.VnD(), -2)),
                "mov x16, #0xfffffffffffffffe\n"
                "ld1h {z10.d}, p6/z, [x16, z11.d]");
  COMPARE_MACRO(Ld1w(z14.VnD(), p3.Zeroing(), SVEMemOperand(z11.VnD(), 42)),
                "mov x16, #0x2a\n"
                "ld1w {z14.d}, p3/z, [x16, z11.d]");
  COMPARE_MACRO(Ld1d(z10.VnD(), p4.Zeroing(), SVEMemOperand(z3.VnD(), 256)),
                "mov x16, #0x100\n"
                "ld1d {z10.d}, p4/z, [x16, z3.d]");
  COMPARE_MACRO(Ld1sb(z14.VnD(), p2.Zeroing(), SVEMemOperand(z11.VnD(), -1)),
                "mov x16, #0xffffffffffffffff\n"
                "ld1sb {z14.d}, p2/z, [x16, z11.d]");
  COMPARE_MACRO(Ld1sh(z20.VnD(), p7.Zeroing(), SVEMemOperand(z12.VnD(), 64)),
                "mov x16, #0x40\n"
                "ld1sh {z20.d}, p7/z, [x16, z12.d]");
  COMPARE_MACRO(Ld1sw(z15.VnD(), p6.Zeroing(), SVEMemOperand(z18.VnD(), 42)),
                "mov x16, #0x2a\n"
                "ld1sw {z15.d}, p6/z, [x16, z18.d]");
  COMPARE_MACRO(Ldff1b(z15.VnD(), p0.Zeroing(), SVEMemOperand(z0.VnD(), 32)),
                "mov x16, #0x20\n"
                "ldff1b {z15.d}, p0/z, [x16, z0.d]");
  COMPARE_MACRO(Ldff1h(z23.VnD(), p3.Zeroing(), SVEMemOperand(z31.VnD(), -2)),
                "mov x16, #0xfffffffffffffffe\n"
                "ldff1h {z23.d}, p3/z, [x16, z31.d]");
  COMPARE_MACRO(Ldff1w(z28.VnD(), p3.Zeroing(), SVEMemOperand(z17.VnD(), 42)),
                "mov x16, #0x2a\n"
                "ldff1w {z28.d}, p3/z, [x16, z17.d]");
  COMPARE_MACRO(Ldff1d(z18.VnD(), p3.Zeroing(), SVEMemOperand(z13.VnD(), 256)),
                "mov x16, #0x100\n"
                "ldff1d {z18.d}, p3/z, [x16, z13.d]");
  COMPARE_MACRO(Ldff1sb(z31.VnD(), p7.Zeroing(), SVEMemOperand(z3.VnD(), -1)),
                "mov x16, #0xffffffffffffffff\n"
                "ldff1sb {z31.d}, p7/z, [x16, z3.d]");
  COMPARE_MACRO(Ldff1sh(z13.VnD(), p0.Zeroing(), SVEMemOperand(z15.VnD(), 64)),
                "mov x16, #0x40\n"
                "ldff1sh {z13.d}, p0/z, [x16, z15.d]");
  COMPARE_MACRO(Ldff1sw(z30.VnD(), p7.Zeroing(), SVEMemOperand(z10.VnD(), 42)),
                "mov x16, #0x2a\n"
                "ldff1sw {z30.d}, p7/z, [x16, z10.d]");

  CLEANUP();
}

TEST(sve_mem_64bit_gather_scalar_plus_vector) {
  SETUP();

  COMPARE(ld1b(z30.VnD(), p6.Zeroing(), SVEMemOperand(sp, z24.VnD())),
          "ld1b {z30.d}, p6/z, [sp, z24.d]");
  COMPARE(ld1d(z18.VnD(), p5.Zeroing(), SVEMemOperand(x11, z11.VnD())),
          "ld1d {z18.d}, p5/z, [x11, z11.d]");
  COMPARE(ld1h(z2.VnD(), p3.Zeroing(), SVEMemOperand(x16, z18.VnD())),
          "ld1h {z2.d}, p3/z, [x16, z18.d]");
  COMPARE(ld1sb(z11.VnD(), p3.Zeroing(), SVEMemOperand(x24, z21.VnD())),
          "ld1sb {z11.d}, p3/z, [x24, z21.d]");
  COMPARE(ld1sh(z7.VnD(), p7.Zeroing(), SVEMemOperand(x28, z23.VnD())),
          "ld1sh {z7.d}, p7/z, [x28, z23.d]");
  COMPARE(ld1sw(z29.VnD(), p7.Zeroing(), SVEMemOperand(x27, z4.VnD())),
          "ld1sw {z29.d}, p7/z, [x27, z4.d]");
  COMPARE(ld1w(z19.VnD(), p1.Zeroing(), SVEMemOperand(x27, z4.VnD())),
          "ld1w {z19.d}, p1/z, [x27, z4.d]");

  COMPARE(ld1d(z20.VnD(), p3.Zeroing(), SVEMemOperand(x3, z15.VnD(), LSL, 3)),
          "ld1d {z20.d}, p3/z, [x3, z15.d, lsl #3]");
  COMPARE(ld1h(z24.VnD(), p4.Zeroing(), SVEMemOperand(x6, z11.VnD(), LSL, 1)),
          "ld1h {z24.d}, p4/z, [x6, z11.d, lsl #1]");
  COMPARE(ld1sh(z22.VnD(), p6.Zeroing(), SVEMemOperand(x7, z31.VnD(), LSL, 1)),
          "ld1sh {z22.d}, p6/z, [x7, z31.d, lsl #1]");
  COMPARE(ld1sw(z9.VnD(), p0.Zeroing(), SVEMemOperand(x2, z27.VnD(), LSL, 2)),
          "ld1sw {z9.d}, p0/z, [x2, z27.d, lsl #2]");
  COMPARE(ld1w(z9.VnD(), p2.Zeroing(), SVEMemOperand(x0, z0.VnD(), LSL, 2)),
          "ld1w {z9.d}, p2/z, [x0, z0.d, lsl #2]");

  COMPARE(ld1b(z19.VnD(), p5.Zeroing(), SVEMemOperand(x21, z29.VnD(), UXTW)),
          "ld1b {z19.d}, p5/z, [x21, z29.d, uxtw]");
  COMPARE(ld1d(z9.VnD(), p5.Zeroing(), SVEMemOperand(x5, z21.VnD(), SXTW)),
          "ld1d {z9.d}, p5/z, [x5, z21.d, sxtw]");
  COMPARE(ld1h(z26.VnD(), p3.Zeroing(), SVEMemOperand(x1, z10.VnD(), UXTW)),
          "ld1h {z26.d}, p3/z, [x1, z10.d, uxtw]");
  COMPARE(ld1sb(z4.VnD(), p1.Zeroing(), SVEMemOperand(x24, z15.VnD(), SXTW)),
          "ld1sb {z4.d}, p1/z, [x24, z15.d, sxtw]");
  COMPARE(ld1sh(z9.VnD(), p1.Zeroing(), SVEMemOperand(x0, z12.VnD(), UXTW)),
          "ld1sh {z9.d}, p1/z, [x0, z12.d, uxtw]");
  COMPARE(ld1sw(z19.VnD(), p2.Zeroing(), SVEMemOperand(x19, z16.VnD(), SXTW)),
          "ld1sw {z19.d}, p2/z, [x19, z16.d, sxtw]");
  COMPARE(ld1w(z13.VnD(), p3.Zeroing(), SVEMemOperand(x8, z10.VnD(), UXTW)),
          "ld1w {z13.d}, p3/z, [x8, z10.d, uxtw]");

  COMPARE(ld1d(z25.VnD(), p3.Zeroing(), SVEMemOperand(x14, z0.VnD(), UXTW, 3)),
          "ld1d {z25.d}, p3/z, [x14, z0.d, uxtw #3]");
  COMPARE(ld1h(z21.VnD(), p5.Zeroing(), SVEMemOperand(x13, z8.VnD(), SXTW, 1)),
          "ld1h {z21.d}, p5/z, [x13, z8.d, sxtw #1]");
  COMPARE(ld1sh(z29.VnD(), p0.Zeroing(), SVEMemOperand(x9, z10.VnD(), UXTW, 1)),
          "ld1sh {z29.d}, p0/z, [x9, z10.d, uxtw #1]");
  COMPARE(ld1sw(z5.VnD(), p2.Zeroing(), SVEMemOperand(x1, z23.VnD(), SXTW, 2)),
          "ld1sw {z5.d}, p2/z, [x1, z23.d, sxtw #2]");
  COMPARE(ld1w(z21.VnD(), p1.Zeroing(), SVEMemOperand(x7, z8.VnD(), UXTW, 2)),
          "ld1w {z21.d}, p1/z, [x7, z8.d, uxtw #2]");

  CLEANUP();
}

TEST(sve_mem_prefetch) {
  SETUP();

  // Test every encodable prefetch operation.
  const char* expected[] = {" pldl1keep",
                            " pldl1strm",
                            " pldl2keep",
                            " pldl2strm",
                            " pldl3keep",
                            " pldl3strm",
                            " pstl1keep",
                            " pstl1strm",
                            " pstl2keep",
                            " pstl2strm",
                            " pstl3keep",
                            " pstl3strm"};

  const PrefetchOperation kSVEPrfOperations[] = {PLDL1KEEP,
                                                 PLDL1STRM,
                                                 PLDL2KEEP,
                                                 PLDL2STRM,
                                                 PLDL3KEEP,
                                                 PLDL3STRM,
                                                 PSTL1KEEP,
                                                 PSTL1STRM,
                                                 PSTL2KEEP,
                                                 PSTL2STRM,
                                                 PSTL3KEEP,
                                                 PSTL3STRM};

  VIXL_STATIC_ASSERT(ArrayLength(expected) == ArrayLength(kSVEPrfOperations));


#define VIXL_DISAM_PREFETCH_TEST(INSN, NAME, SH)                               \
  do {                                                                         \
    for (size_t i = 0; i < ArrayLength(kSVEPrfOperations); i++) {              \
      PrefetchOperation op = kSVEPrfOperations[i];                             \
      std::string str(NAME);                                                   \
      str.append(expected[i]);                                                 \
      /* Vector plus immediate */                                              \
      COMPARE_PREFIX(INSN(op, p6, SVEMemOperand(z30.VnS(), 31)), str.c_str()); \
      COMPARE_PREFIX(INSN(op, p5, SVEMemOperand(z29.VnD(), 17)), str.c_str()); \
      /* Scalar plus immediate */                                              \
      COMPARE_PREFIX(INSN(op, p4, SVEMemOperand(x11, -32, SVE_MUL_VL)),        \
                     str.c_str());                                             \
      COMPARE_PREFIX(INSN(op, p4, SVEMemOperand(sp, 31, SVE_MUL_VL)),          \
                     str.c_str());                                             \
      /* Scalar plus vector */                                                 \
      COMPARE_PREFIX(INSN(op, p3, SVEMemOperand(x24, z22.VnS(), UXTW, SH)),    \
                     str.c_str());                                             \
      COMPARE_PREFIX(INSN(op, p2, SVEMemOperand(x24, z22.VnD(), SXTW, SH)),    \
                     str.c_str());                                             \
      COMPARE_PREFIX(INSN(op, p1, SVEMemOperand(x4, z2.VnD(), LSL, SH)),       \
                     str.c_str());                                             \
      /* Scalar plus scalar */                                                 \
      COMPARE_PREFIX(INSN(op, p1, SVEMemOperand(x8, x29, LSL, SH)),            \
                     str.c_str());                                             \
      COMPARE_PREFIX(INSN(op, p0, SVEMemOperand(sp, x6, LSL, SH)),             \
                     str.c_str());                                             \
    }                                                                          \
  } while (0)

  VIXL_DISAM_PREFETCH_TEST(prfh, "prfh", 1);
  VIXL_DISAM_PREFETCH_TEST(prfw, "prfw", 2);
  VIXL_DISAM_PREFETCH_TEST(prfd, "prfd", 3);
#undef VIXL_DISAM_PREFETCH_TEST

  COMPARE(prfb(PLDL1KEEP, p5, SVEMemOperand(z30.VnS(), 0)),
          "prfb pldl1keep, p5, [z30.s]");
  COMPARE(prfb(PLDL1STRM, p5, SVEMemOperand(x28, -11, SVE_MUL_VL)),
          "prfb pldl1strm, p5, [x28, #-11, mul vl]");
  COMPARE(prfb(PLDL2KEEP, p6, SVEMemOperand(x30, x29)),
          "prfb pldl2keep, p6, [x30, x29]");
  COMPARE(prfb(PLDL2STRM, p6, SVEMemOperand(x7, z12.VnD())),
          "prfb pldl2strm, p6, [x7, z12.d]");
  COMPARE(prfb(PLDL2STRM, p6, SVEMemOperand(x7, z12.VnS(), UXTW)),
          "prfb pldl2strm, p6, [x7, z12.s, uxtw]");
  COMPARE(prfd(PLDL3KEEP, p5, SVEMemOperand(z11.VnD(), 9)),
          "prfd pldl3keep, p5, [z11.d, #9]");
  COMPARE(prfd(PLDL3STRM, p3, SVEMemOperand(x0, 0, SVE_MUL_VL)),
          "prfd pldl3strm, p3, [x0]");
  COMPARE(prfd(PSTL1KEEP, p7, SVEMemOperand(x5, x5, LSL, 3)),
          "prfd pstl1keep, p7, [x5, x5, lsl #3]");
  COMPARE(prfd(PSTL1STRM, p1, SVEMemOperand(x19, z18.VnS(), SXTW, 3)),
          "prfd pstl1strm, p1, [x19, z18.s, sxtw #3]");
  COMPARE(prfh(PSTL2KEEP, p6, SVEMemOperand(z0.VnS(), 31)),
          "prfh pstl2keep, p6, [z0.s, #31]");
  COMPARE(prfh(PSTL2STRM, p4, SVEMemOperand(x17, -3, SVE_MUL_VL)),
          "prfh pstl2strm, p4, [x17, #-3, mul vl]");
  COMPARE(prfh(PSTL3KEEP, p3, SVEMemOperand(x0, x0, LSL, 1)),
          "prfh pstl3keep, p3, [x0, x0, lsl #1]");
  COMPARE(prfh(PSTL3STRM, p4, SVEMemOperand(x20, z0.VnD(), LSL, 1)),
          "prfh pstl3strm, p4, [x20, z0.d, lsl #1]");
  COMPARE(prfw(PLDL1KEEP, p3, SVEMemOperand(z23.VnD(), 5)),
          "prfw pldl1keep, p3, [z23.d, #5]");
  COMPARE(prfw(PLDL1STRM, p1, SVEMemOperand(x4, 31, SVE_MUL_VL)),
          "prfw pldl1strm, p1, [x4, #31, mul vl]");
  COMPARE(prfw(PLDL2KEEP, p2, SVEMemOperand(x22, x22, LSL, 2)),
          "prfw pldl2keep, p2, [x22, x22, lsl #2]");
  COMPARE(prfw(PLDL2STRM, p1, SVEMemOperand(x2, z6.VnS(), SXTW, 2)),
          "prfw pldl2strm, p1, [x2, z6.s, sxtw #2]");

  CLEANUP();
}

TEST(sve_mem_64bit_ff_gather_scalar_plus_vector) {
  SETUP();

  // 64-bit unscaled offset.
  COMPARE(ldff1b(z18.VnD(), p6.Zeroing(), SVEMemOperand(x27, z24.VnD())),
          "ldff1b {z18.d}, p6/z, [x27, z24.d]");
  COMPARE(ldff1h(z28.VnD(), p6.Zeroing(), SVEMemOperand(x1, z30.VnD())),
          "ldff1h {z28.d}, p6/z, [x1, z30.d]");
  COMPARE(ldff1w(z12.VnD(), p3.Zeroing(), SVEMemOperand(x25, z27.VnD())),
          "ldff1w {z12.d}, p3/z, [x25, z27.d]");
  COMPARE(ldff1d(z23.VnD(), p5.Zeroing(), SVEMemOperand(x29, z31.VnD())),
          "ldff1d {z23.d}, p5/z, [x29, z31.d]");
  COMPARE(ldff1sb(z15.VnD(), p5.Zeroing(), SVEMemOperand(x5, z14.VnD())),
          "ldff1sb {z15.d}, p5/z, [x5, z14.d]");
  COMPARE(ldff1sh(z18.VnD(), p4.Zeroing(), SVEMemOperand(x25, z25.VnD())),
          "ldff1sh {z18.d}, p4/z, [x25, z25.d]");
  COMPARE(ldff1sw(z12.VnD(), p3.Zeroing(), SVEMemOperand(x25, z27.VnD())),
          "ldff1sw {z12.d}, p3/z, [x25, z27.d]");

  // 64-bit scaled offset.
  COMPARE(ldff1h(z25.VnD(),
                 p3.Zeroing(),
                 SVEMemOperand(x17, z15.VnD(), LSL, 1)),
          "ldff1h {z25.d}, p3/z, [x17, z15.d, lsl #1]");
  COMPARE(ldff1w(z5.VnD(), p4.Zeroing(), SVEMemOperand(x23, z31.VnD(), LSL, 2)),
          "ldff1w {z5.d}, p4/z, [x23, z31.d, lsl #2]");
  COMPARE(ldff1d(z2.VnD(), p0.Zeroing(), SVEMemOperand(sp, z7.VnD(), LSL, 3)),
          "ldff1d {z2.d}, p0/z, [sp, z7.d, lsl #3]");
  COMPARE(ldff1sh(z10.VnD(),
                  p0.Zeroing(),
                  SVEMemOperand(x19, z15.VnD(), LSL, 1)),
          "ldff1sh {z10.d}, p0/z, [x19, z15.d, lsl #1]");
  COMPARE(ldff1sw(z5.VnD(),
                  p4.Zeroing(),
                  SVEMemOperand(x23, z31.VnD(), LSL, 2)),
          "ldff1sw {z5.d}, p4/z, [x23, z31.d, lsl #2]");

  // 32-bit unpacked unscaled offset
  COMPARE(ldff1b(z18.VnD(), p6.Zeroing(), SVEMemOperand(sp, z24.VnD(), UXTW)),
          "ldff1b {z18.d}, p6/z, [sp, z24.d, uxtw]");
  COMPARE(ldff1h(z20.VnD(), p5.Zeroing(), SVEMemOperand(x7, z14.VnD(), SXTW)),
          "ldff1h {z20.d}, p5/z, [x7, z14.d, sxtw]");
  COMPARE(ldff1w(z22.VnD(), p4.Zeroing(), SVEMemOperand(x17, z4.VnD(), UXTW)),
          "ldff1w {z22.d}, p4/z, [x17, z4.d, uxtw]");
  COMPARE(ldff1d(z24.VnD(), p3.Zeroing(), SVEMemOperand(x3, z24.VnD(), SXTW)),
          "ldff1d {z24.d}, p3/z, [x3, z24.d, sxtw]");
  COMPARE(ldff1sb(z26.VnD(), p2.Zeroing(), SVEMemOperand(x13, z14.VnD(), UXTW)),
          "ldff1sb {z26.d}, p2/z, [x13, z14.d, uxtw]");
  COMPARE(ldff1sh(z28.VnD(), p1.Zeroing(), SVEMemOperand(x23, z4.VnD(), SXTW)),
          "ldff1sh {z28.d}, p1/z, [x23, z4.d, sxtw]");
  COMPARE(ldff1sw(z30.VnD(), p0.Zeroing(), SVEMemOperand(x8, z24.VnD(), UXTW)),
          "ldff1sw {z30.d}, p0/z, [x8, z24.d, uxtw]");

  // 32-bit unpacked scaled offset
  COMPARE(ldff1h(z4.VnD(), p5.Zeroing(), SVEMemOperand(x7, z1.VnD(), SXTW, 1)),
          "ldff1h {z4.d}, p5/z, [x7, z1.d, sxtw #1]");
  COMPARE(ldff1w(z5.VnD(),
                 p4.Zeroing(),
                 SVEMemOperand(x17, z11.VnD(), UXTW, 2)),
          "ldff1w {z5.d}, p4/z, [x17, z11.d, uxtw #2]");
  COMPARE(ldff1d(z6.VnD(), p3.Zeroing(), SVEMemOperand(x3, z31.VnD(), SXTW, 3)),
          "ldff1d {z6.d}, p3/z, [x3, z31.d, sxtw #3]");
  COMPARE(ldff1sh(z7.VnD(),
                  p1.Zeroing(),
                  SVEMemOperand(x23, z7.VnD(), UXTW, 1)),
          "ldff1sh {z7.d}, p1/z, [x23, z7.d, uxtw #1]");
  COMPARE(ldff1sw(z8.VnD(),
                  p0.Zeroing(),
                  SVEMemOperand(x8, z17.VnD(), SXTW, 2)),
          "ldff1sw {z8.d}, p0/z, [x8, z17.d, sxtw #2]");

  CLEANUP();
}

TEST(sve_ld2_scalar_plus_immediate) {
  SETUP();

  COMPARE(ld2b(z31.VnB(), z0.VnB(), p6.Zeroing(), SVEMemOperand(x19)),
          "ld2b {z31.b, z0.b}, p6/z, [x19]");
  COMPARE(ld2b(z31.VnB(),
               z0.VnB(),
               p6.Zeroing(),
               SVEMemOperand(x19, 14, SVE_MUL_VL)),
          "ld2b {z31.b, z0.b}, p6/z, [x19, #14, mul vl]");
  COMPARE(ld2b(z15.VnB(),
               z16.VnB(),
               p6.Zeroing(),
               SVEMemOperand(x19, -16, SVE_MUL_VL)),
          "ld2b {z15.b, z16.b}, p6/z, [x19, #-16, mul vl]");

  COMPARE(ld2h(z15.VnH(), z16.VnH(), p6.Zeroing(), SVEMemOperand(x19)),
          "ld2h {z15.h, z16.h}, p6/z, [x19]");
  COMPARE(ld2h(z15.VnH(),
               z16.VnH(),
               p0.Zeroing(),
               SVEMemOperand(x19, 14, SVE_MUL_VL)),
          "ld2h {z15.h, z16.h}, p0/z, [x19, #14, mul vl]");
  COMPARE(ld2h(z15.VnH(),
               z16.VnH(),
               p0.Zeroing(),
               SVEMemOperand(x19, -16, SVE_MUL_VL)),
          "ld2h {z15.h, z16.h}, p0/z, [x19, #-16, mul vl]");

  COMPARE(ld2w(z0.VnS(), z1.VnS(), p0.Zeroing(), SVEMemOperand(x19)),
          "ld2w {z0.s, z1.s}, p0/z, [x19]");
  COMPARE(ld2w(z0.VnS(),
               z1.VnS(),
               p0.Zeroing(),
               SVEMemOperand(x19, 14, SVE_MUL_VL)),
          "ld2w {z0.s, z1.s}, p0/z, [x19, #14, mul vl]");
  COMPARE(ld2w(z0.VnS(),
               z1.VnS(),
               p7.Zeroing(),
               SVEMemOperand(x19, -16, SVE_MUL_VL)),
          "ld2w {z0.s, z1.s}, p7/z, [x19, #-16, mul vl]");

  COMPARE(ld2d(z0.VnD(), z1.VnD(), p7.Zeroing(), SVEMemOperand(x19)),
          "ld2d {z0.d, z1.d}, p7/z, [x19]");
  COMPARE(ld2d(z31.VnD(),
               z0.VnD(),
               p7.Zeroing(),
               SVEMemOperand(x19, 14, SVE_MUL_VL)),
          "ld2d {z31.d, z0.d}, p7/z, [x19, #14, mul vl]");
  COMPARE(ld2d(z31.VnD(),
               z0.VnD(),
               p7.Zeroing(),
               SVEMemOperand(x19, -16, SVE_MUL_VL)),
          "ld2d {z31.d, z0.d}, p7/z, [x19, #-16, mul vl]");

  CLEANUP();
}

TEST(sve_ld3_scalar_plus_immediate) {
  SETUP();

  COMPARE(ld3b(z30.VnB(),
               z31.VnB(),
               z0.VnB(),
               p7.Zeroing(),
               SVEMemOperand(x19)),
          "ld3b {z30.b, z31.b, z0.b}, p7/z, [x19]");
  COMPARE(ld3b(z30.VnB(),
               z31.VnB(),
               z0.VnB(),
               p6.Zeroing(),
               SVEMemOperand(x19, 21, SVE_MUL_VL)),
          "ld3b {z30.b, z31.b, z0.b}, p6/z, [x19, #21, mul vl]");
  COMPARE(ld3b(z30.VnB(),
               z31.VnB(),
               z0.VnB(),
               p6.Zeroing(),
               SVEMemOperand(x19, -24, SVE_MUL_VL)),
          "ld3b {z30.b, z31.b, z0.b}, p6/z, [x19, #-24, mul vl]");

  COMPARE(ld3h(z15.VnH(),
               z16.VnH(),
               z17.VnH(),
               p6.Zeroing(),
               SVEMemOperand(x19)),
          "ld3h {z15.h, z16.h, z17.h}, p6/z, [x19]");
  COMPARE(ld3h(z15.VnH(),
               z16.VnH(),
               z17.VnH(),
               p6.Zeroing(),
               SVEMemOperand(x19, 21, SVE_MUL_VL)),
          "ld3h {z15.h, z16.h, z17.h}, p6/z, [x19, #21, mul vl]");
  COMPARE(ld3h(z15.VnH(),
               z16.VnH(),
               z17.VnH(),
               p0.Zeroing(),
               SVEMemOperand(x19, -24, SVE_MUL_VL)),
          "ld3h {z15.h, z16.h, z17.h}, p0/z, [x19, #-24, mul vl]");

  COMPARE(ld3w(z15.VnS(),
               z16.VnS(),
               z17.VnS(),
               p0.Zeroing(),
               SVEMemOperand(x19)),
          "ld3w {z15.s, z16.s, z17.s}, p0/z, [x19]");
  COMPARE(ld3w(z0.VnS(),
               z1.VnS(),
               z2.VnS(),
               p0.Zeroing(),
               SVEMemOperand(x19, 21, SVE_MUL_VL)),
          "ld3w {z0.s, z1.s, z2.s}, p0/z, [x19, #21, mul vl]");
  COMPARE(ld3w(z0.VnS(),
               z1.VnS(),
               z2.VnS(),
               p0.Zeroing(),
               SVEMemOperand(x19, -24, SVE_MUL_VL)),
          "ld3w {z0.s, z1.s, z2.s}, p0/z, [x19, #-24, mul vl]");

  COMPARE(ld3d(z0.VnD(), z1.VnD(), z2.VnD(), p7.Zeroing(), SVEMemOperand(x19)),
          "ld3d {z0.d, z1.d, z2.d}, p7/z, [x19]");
  COMPARE(ld3d(z0.VnD(),
               z1.VnD(),
               z2.VnD(),
               p7.Zeroing(),
               SVEMemOperand(x19, 21, SVE_MUL_VL)),
          "ld3d {z0.d, z1.d, z2.d}, p7/z, [x19, #21, mul vl]");
  COMPARE(ld3d(z30.VnD(),
               z31.VnD(),
               z0.VnD(),
               p7.Zeroing(),
               SVEMemOperand(x19, -24, SVE_MUL_VL)),
          "ld3d {z30.d, z31.d, z0.d}, p7/z, [x19, #-24, mul vl]");

  CLEANUP();
}

TEST(sve_ld4_scalar_plus_immediate) {
  SETUP();

  COMPARE(ld4b(z31.VnB(),
               z0.VnB(),
               z1.VnB(),
               z2.VnB(),
               p7.Zeroing(),
               SVEMemOperand(x19)),
          "ld4b {z31.b, z0.b, z1.b, z2.b}, p7/z, [x19]");
  COMPARE(ld4b(z31.VnB(),
               z0.VnB(),
               z1.VnB(),
               z2.VnB(),
               p7.Zeroing(),
               SVEMemOperand(x19, 28, SVE_MUL_VL)),
          "ld4b {z31.b, z0.b, z1.b, z2.b}, p7/z, [x19, #28, mul vl]");
  COMPARE(ld4b(z31.VnB(),
               z0.VnB(),
               z1.VnB(),
               z2.VnB(),
               p6.Zeroing(),
               SVEMemOperand(x19, -32, SVE_MUL_VL)),
          "ld4b {z31.b, z0.b, z1.b, z2.b}, p6/z, [x19, #-32, mul vl]");

  COMPARE(ld4h(z31.VnH(),
               z0.VnH(),
               z1.VnH(),
               z2.VnH(),
               p6.Zeroing(),
               SVEMemOperand(x19)),
          "ld4h {z31.h, z0.h, z1.h, z2.h}, p6/z, [x19]");
  COMPARE(ld4h(z15.VnH(),
               z16.VnH(),
               z17.VnH(),
               z18.VnH(),
               p6.Zeroing(),
               SVEMemOperand(x19, 28, SVE_MUL_VL)),
          "ld4h {z15.h, z16.h, z17.h, z18.h}, p6/z, "
          "[x19, #28, mul vl]");
  COMPARE(ld4h(z15.VnH(),
               z16.VnH(),
               z17.VnH(),
               z18.VnH(),
               p6.Zeroing(),
               SVEMemOperand(x19, -32, SVE_MUL_VL)),
          "ld4h {z15.h, z16.h, z17.h, z18.h}, p6/z, "
          "[x19, #-32, mul vl]");

  COMPARE(ld4w(z15.VnS(),
               z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               p0.Zeroing(),
               SVEMemOperand(x19)),
          "ld4w {z15.s, z16.s, z17.s, z18.s}, p0/z, [x19]");
  COMPARE(ld4w(z15.VnS(),
               z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               p0.Zeroing(),
               SVEMemOperand(x19, 28, SVE_MUL_VL)),
          "ld4w {z15.s, z16.s, z17.s, z18.s}, p0/z, "
          "[x19, #28, mul vl]");
  COMPARE(ld4w(z0.VnS(),
               z1.VnS(),
               z2.VnS(),
               z3.VnS(),
               p0.Zeroing(),
               SVEMemOperand(x19, -32, SVE_MUL_VL)),
          "ld4w {z0.s, z1.s, z2.s, z3.s}, p0/z, [x19, #-32, mul vl]");

  COMPARE(ld4d(z0.VnD(),
               z1.VnD(),
               z2.VnD(),
               z3.VnD(),
               p0.Zeroing(),
               SVEMemOperand(x19)),
          "ld4d {z0.d, z1.d, z2.d, z3.d}, p0/z, [x19]");
  COMPARE(ld4d(z0.VnD(),
               z1.VnD(),
               z2.VnD(),
               z3.VnD(),
               p7.Zeroing(),
               SVEMemOperand(x19, 28, SVE_MUL_VL)),
          "ld4d {z0.d, z1.d, z2.d, z3.d}, p7/z, [x19, #28, mul vl]");
  COMPARE(ld4d(z0.VnD(),
               z1.VnD(),
               z2.VnD(),
               z3.VnD(),
               p7.Zeroing(),
               SVEMemOperand(x19, -32, SVE_MUL_VL)),
          "ld4d {z0.d, z1.d, z2.d, z3.d}, p7/z, [x19, #-32, mul vl]");

  CLEANUP();
}

TEST(sve_ld2_scalar_plus_scalar) {
  SETUP();

  COMPARE(ld2b(z25.VnB(), z26.VnB(), p1.Zeroing(), SVEMemOperand(x20, x19)),
          "ld2b {z25.b, z26.b}, p1/z, [x20, x19]");
  COMPARE(ld2b(z25.VnB(), z26.VnB(), p1.Zeroing(), SVEMemOperand(sp, x19)),
          "ld2b {z25.b, z26.b}, p1/z, [sp, x19]");
  COMPARE(ld2b(z31.VnB(), z0.VnB(), p1.Zeroing(), SVEMemOperand(sp, x19)),
          "ld2b {z31.b, z0.b}, p1/z, [sp, x19]");

  COMPARE(ld2h(z31.VnH(),
               z0.VnH(),
               p1.Zeroing(),
               SVEMemOperand(x20, x19, LSL, 1)),
          "ld2h {z31.h, z0.h}, p1/z, [x20, x19, lsl #1]");
  COMPARE(ld2h(z31.VnH(),
               z0.VnH(),
               p7.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 1)),
          "ld2h {z31.h, z0.h}, p7/z, [sp, x19, lsl #1]");
  COMPARE(ld2h(z31.VnH(),
               z0.VnH(),
               p7.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 1)),
          "ld2h {z31.h, z0.h}, p7/z, [sp, x19, lsl #1]");

  COMPARE(ld2w(z16.VnS(),
               z17.VnS(),
               p7.Zeroing(),
               SVEMemOperand(x20, x19, LSL, 2)),
          "ld2w {z16.s, z17.s}, p7/z, [x20, x19, lsl #2]");
  COMPARE(ld2w(z16.VnS(),
               z17.VnS(),
               p7.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 2)),
          "ld2w {z16.s, z17.s}, p7/z, [sp, x19, lsl #2]");
  COMPARE(ld2w(z16.VnS(),
               z17.VnS(),
               p0.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 2)),
          "ld2w {z16.s, z17.s}, p0/z, [sp, x19, lsl #2]");

  COMPARE(ld2d(z16.VnD(),
               z17.VnD(),
               p0.Zeroing(),
               SVEMemOperand(x20, x19, LSL, 3)),
          "ld2d {z16.d, z17.d}, p0/z, [x20, x19, lsl #3]");
  COMPARE(ld2d(z25.VnD(),
               z26.VnD(),
               p0.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 3)),
          "ld2d {z25.d, z26.d}, p0/z, [sp, x19, lsl #3]");
  COMPARE(ld2d(z25.VnD(),
               z26.VnD(),
               p0.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 3)),
          "ld2d {z25.d, z26.d}, p0/z, [sp, x19, lsl #3]");

  CLEANUP();
}

TEST(sve_ld3_scalar_plus_scalar) {
  SETUP();

  COMPARE(ld3b(z25.VnB(),
               z26.VnB(),
               z27.VnB(),
               p1.Zeroing(),
               SVEMemOperand(x20, x19)),
          "ld3b {z25.b, z26.b, z27.b}, p1/z, [x20, x19]");
  COMPARE(ld3b(z25.VnB(),
               z26.VnB(),
               z27.VnB(),
               p1.Zeroing(),
               SVEMemOperand(sp, x19)),
          "ld3b {z25.b, z26.b, z27.b}, p1/z, [sp, x19]");
  COMPARE(ld3b(z30.VnB(),
               z31.VnB(),
               z0.VnB(),
               p1.Zeroing(),
               SVEMemOperand(sp, x19)),
          "ld3b {z30.b, z31.b, z0.b}, p1/z, [sp, x19]");

  COMPARE(ld3h(z30.VnH(),
               z31.VnH(),
               z0.VnH(),
               p1.Zeroing(),
               SVEMemOperand(x20, x19, LSL, 1)),
          "ld3h {z30.h, z31.h, z0.h}, p1/z, [x20, x19, lsl #1]");
  COMPARE(ld3h(z30.VnH(),
               z31.VnH(),
               z0.VnH(),
               p7.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 1)),
          "ld3h {z30.h, z31.h, z0.h}, p7/z, [sp, x19, lsl #1]");
  COMPARE(ld3h(z30.VnH(),
               z31.VnH(),
               z0.VnH(),
               p7.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 1)),
          "ld3h {z30.h, z31.h, z0.h}, p7/z, [sp, x19, lsl #1]");

  COMPARE(ld3w(z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               p7.Zeroing(),
               SVEMemOperand(x20, x19, LSL, 2)),
          "ld3w {z16.s, z17.s, z18.s}, p7/z, [x20, x19, lsl #2]");
  COMPARE(ld3w(z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               p7.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 2)),
          "ld3w {z16.s, z17.s, z18.s}, p7/z, [sp, x19, lsl #2]");
  COMPARE(ld3w(z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               p0.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 2)),
          "ld3w {z16.s, z17.s, z18.s}, p0/z, [sp, x19, lsl #2]");

  COMPARE(ld3d(z16.VnD(),
               z17.VnD(),
               z18.VnD(),
               p0.Zeroing(),
               SVEMemOperand(x20, x19, LSL, 3)),
          "ld3d {z16.d, z17.d, z18.d}, p0/z, [x20, x19, lsl #3]");
  COMPARE(ld3d(z25.VnD(),
               z26.VnD(),
               z27.VnD(),
               p0.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 3)),
          "ld3d {z25.d, z26.d, z27.d}, p0/z, [sp, x19, lsl #3]");
  COMPARE(ld3d(z25.VnD(),
               z26.VnD(),
               z27.VnD(),
               p0.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 3)),
          "ld3d {z25.d, z26.d, z27.d}, p0/z, [sp, x19, lsl #3]");

  CLEANUP();
}

TEST(sve_ld4_scalar_plus_scalar) {
  SETUP();

  COMPARE(ld4b(z25.VnB(),
               z26.VnB(),
               z27.VnB(),
               z28.VnB(),
               p0.Zeroing(),
               SVEMemOperand(x20, x19)),
          "ld4b {z25.b, z26.b, z27.b, z28.b}, p0/z, [x20, x19]");
  COMPARE(ld4b(z25.VnB(),
               z26.VnB(),
               z27.VnB(),
               z28.VnB(),
               p1.Zeroing(),
               SVEMemOperand(sp, x19)),
          "ld4b {z25.b, z26.b, z27.b, z28.b}, p1/z, [sp, x19]");
  COMPARE(ld4b(z25.VnB(),
               z26.VnB(),
               z27.VnB(),
               z28.VnB(),
               p1.Zeroing(),
               SVEMemOperand(sp, x19)),
          "ld4b {z25.b, z26.b, z27.b, z28.b}, p1/z, [sp, x19]");

  COMPARE(ld4h(z31.VnH(),
               z0.VnH(),
               z1.VnH(),
               z2.VnH(),
               p1.Zeroing(),
               SVEMemOperand(x20, x19, LSL, 1)),
          "ld4h {z31.h, z0.h, z1.h, z2.h}, p1/z, [x20, x19, lsl #1]");
  COMPARE(ld4h(z31.VnH(),
               z0.VnH(),
               z1.VnH(),
               z2.VnH(),
               p1.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 1)),
          "ld4h {z31.h, z0.h, z1.h, z2.h}, p1/z, [sp, x19, lsl #1]");
  COMPARE(ld4h(z31.VnH(),
               z0.VnH(),
               z1.VnH(),
               z2.VnH(),
               p7.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 1)),
          "ld4h {z31.h, z0.h, z1.h, z2.h}, p7/z, [sp, x19, lsl #1]");

  COMPARE(ld4w(z31.VnS(),
               z0.VnS(),
               z1.VnS(),
               z2.VnS(),
               p7.Zeroing(),
               SVEMemOperand(x20, x19, LSL, 2)),
          "ld4w {z31.s, z0.s, z1.s, z2.s}, p7/z, [x20, x19, lsl #2]");
  COMPARE(ld4w(z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               z19.VnS(),
               p7.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 2)),
          "ld4w {z16.s, z17.s, z18.s, z19.s}, p7/z, "
          "[sp, x19, lsl #2]");
  COMPARE(ld4w(z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               z19.VnS(),
               p7.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 2)),
          "ld4w {z16.s, z17.s, z18.s, z19.s}, p7/z, "
          "[sp, x19, lsl #2]");

  COMPARE(ld4d(z16.VnD(),
               z17.VnD(),
               z18.VnD(),
               z19.VnD(),
               p0.Zeroing(),
               SVEMemOperand(x20, x19, LSL, 3)),
          "ld4d {z16.d, z17.d, z18.d, z19.d}, p0/z, "
          "[x20, x19, lsl #3]");
  COMPARE(ld4d(z16.VnD(),
               z17.VnD(),
               z18.VnD(),
               z19.VnD(),
               p0.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 3)),
          "ld4d {z16.d, z17.d, z18.d, z19.d}, p0/z, "
          "[sp, x19, lsl #3]");
  COMPARE(ld4d(z25.VnD(),
               z26.VnD(),
               z27.VnD(),
               z28.VnD(),
               p0.Zeroing(),
               SVEMemOperand(sp, x19, LSL, 3)),
          "ld4d {z25.d, z26.d, z27.d, z28.d}, p0/z, "
          "[sp, x19, lsl #3]");

  CLEANUP();
}

TEST(sve_ff_contiguous) {
  SETUP();

  COMPARE(ldff1b(z24.VnB(), p1.Zeroing(), SVEMemOperand(x21)),
          "ldff1b {z24.b}, p1/z, [x21]");
  COMPARE(ldff1b(z22.VnH(), p5.Zeroing(), SVEMemOperand(x5, x28)),
          "ldff1b {z22.h}, p5/z, [x5, x28]");
  COMPARE(ldff1b(z2.VnS(), p5.Zeroing(), SVEMemOperand(sp, x11)),
          "ldff1b {z2.s}, p5/z, [sp, x11]");
  COMPARE(ldff1b(z12.VnD(), p3.Zeroing(), SVEMemOperand(x26, xzr)),
          "ldff1b {z12.d}, p3/z, [x26]");
  COMPARE(ldff1h(z21.VnH(), p3.Zeroing(), SVEMemOperand(x27)),
          "ldff1h {z21.h}, p3/z, [x27]");
  COMPARE(ldff1h(z11.VnS(), p6.Zeroing(), SVEMemOperand(sp, x15, LSL, 1)),
          "ldff1h {z11.s}, p6/z, [sp, x15, lsl #1]");
  COMPARE(ldff1h(z6.VnD(), p7.Zeroing(), SVEMemOperand(x8, xzr, LSL, 1)),
          "ldff1h {z6.d}, p7/z, [x8]");
  COMPARE(ldff1w(z11.VnS(), p7.Zeroing(), SVEMemOperand(sp)),
          "ldff1w {z11.s}, p7/z, [sp]");
  COMPARE(ldff1w(z6.VnD(), p6.Zeroing(), SVEMemOperand(x5, x0, LSL, 2)),
          "ldff1w {z6.d}, p6/z, [x5, x0, lsl #2]");
  COMPARE(ldff1d(z0.VnD(), p3.Zeroing(), SVEMemOperand(x15, x1, LSL, 3)),
          "ldff1d {z0.d}, p3/z, [x15, x1, lsl #3]");

  COMPARE(ldff1sb(z31.VnH(), p4.Zeroing(), SVEMemOperand(x10, x25)),
          "ldff1sb {z31.h}, p4/z, [x10, x25]");
  COMPARE(ldff1sb(z25.VnS(), p7.Zeroing(), SVEMemOperand(sp, x20)),
          "ldff1sb {z25.s}, p7/z, [sp, x20]");
  COMPARE(ldff1sb(z20.VnD(), p3.Zeroing(), SVEMemOperand(x19, xzr)),
          "ldff1sb {z20.d}, p3/z, [x19]");
  COMPARE(ldff1sh(z18.VnS(), p3.Zeroing(), SVEMemOperand(sp, x0, LSL, 1)),
          "ldff1sh {z18.s}, p3/z, [sp, x0, lsl #1]");
  COMPARE(ldff1sh(z30.VnD(), p1.Zeroing(), SVEMemOperand(x28, xzr, LSL, 1)),
          "ldff1sh {z30.d}, p1/z, [x28]");
  COMPARE(ldff1sw(z3.VnD(), p4.Zeroing(), SVEMemOperand(x22, x18, LSL, 2)),
          "ldff1sw {z3.d}, p4/z, [x22, x18, lsl #2]");

  CLEANUP();
}

TEST(sve_mem_contiguous_load) {
  SETUP();

  COMPARE(ld1rqb(z3.VnB(), p2.Zeroing(), SVEMemOperand(x22, x18)),
          "ld1rqb {z3.b}, p2/z, [x22, x18]");
  COMPARE(ld1rqd(z6.VnD(), p0.Zeroing(), SVEMemOperand(x18, x9, LSL, 3)),
          "ld1rqd {z6.d}, p0/z, [x18, x9, lsl #3]");
  COMPARE(ld1rqh(z1.VnH(), p7.Zeroing(), SVEMemOperand(x9, x6, LSL, 1)),
          "ld1rqh {z1.h}, p7/z, [x9, x6, lsl #1]");
  COMPARE(ld1rqw(z12.VnS(), p4.Zeroing(), SVEMemOperand(sp, xzr, LSL, 2)),
          "ld1rqw {z12.s}, p4/z, [sp, xzr, lsl #2]");
  COMPARE(ld1rqb(z18.VnB(), p2.Zeroing(), SVEMemOperand(x18, 0)),
          "ld1rqb {z18.b}, p2/z, [x18]");
  COMPARE(ld1rqb(z18.VnB(), p2.Zeroing(), SVEMemOperand(x18, 16)),
          "ld1rqb {z18.b}, p2/z, [x18, #16]");
  COMPARE(ld1rqd(z11.VnD(), p1.Zeroing(), SVEMemOperand(x23, -16)),
          "ld1rqd {z11.d}, p1/z, [x23, #-16]");
  COMPARE(ld1rqh(z11.VnH(), p1.Zeroing(), SVEMemOperand(x0, 112)),
          "ld1rqh {z11.h}, p1/z, [x0, #112]");
  COMPARE(ld1rqw(z22.VnS(), p3.Zeroing(), SVEMemOperand(sp, -128)),
          "ld1rqw {z22.s}, p3/z, [sp, #-128]");

  COMPARE_MACRO(Ld1rqb(z0.VnB(), p0.Zeroing(), SVEMemOperand(x0, x1)),
                "ld1rqb {z0.b}, p0/z, [x0, x1]");
  COMPARE_MACRO(Ld1rqh(z0.VnH(), p0.Zeroing(), SVEMemOperand(x0, x1, LSL, 1)),
                "ld1rqh {z0.h}, p0/z, [x0, x1, lsl #1]");
  COMPARE_MACRO(Ld1rqw(z0.VnS(), p0.Zeroing(), SVEMemOperand(x0, x1, LSL, 2)),
                "ld1rqw {z0.s}, p0/z, [x0, x1, lsl #2]");
  COMPARE_MACRO(Ld1rqd(z0.VnD(), p0.Zeroing(), SVEMemOperand(x0, x1, LSL, 3)),
                "ld1rqd {z0.d}, p0/z, [x0, x1, lsl #3]");
  COMPARE_MACRO(Ld1rqh(z11.VnH(), p1.Zeroing(), SVEMemOperand(x0, 112)),
                "ld1rqh {z11.h}, p1/z, [x0, #112]");
  COMPARE_MACRO(Ld1rqw(z22.VnS(), p3.Zeroing(), SVEMemOperand(sp, -128)),
                "ld1rqw {z22.s}, p3/z, [sp, #-128]");

  COMPARE_MACRO(Ld1rqb(z0.VnB(), p0.Zeroing(), SVEMemOperand(x0, 2222)),
                "add x16, x0, #0x8ae (2222)\n"
                "ld1rqb {z0.b}, p0/z, [x16]");
  COMPARE_MACRO(Ld1rqw(z0.VnS(), p0.Zeroing(), SVEMemOperand(x0, x1)),
                "add x16, x0, x1\n"
                "ld1rqw {z0.s}, p0/z, [x16]");
  COMPARE_MACRO(Ld1rqd(z0.VnD(), p0.Zeroing(), SVEMemOperand(x0, x1, LSL, 1)),
                "add x16, x0, x1, lsl #1\n"
                "ld1rqd {z0.d}, p0/z, [x16]");

  COMPARE(ldnt1b(z21.VnB(), p5.Zeroing(), SVEMemOperand(x1, x23)),
          "ldnt1b {z21.b}, p5/z, [x1, x23]");
  COMPARE(ldnt1d(z10.VnD(), p0.Zeroing(), SVEMemOperand(x23, x6, LSL, 3)),
          "ldnt1d {z10.d}, p0/z, [x23, x6, lsl #3]");
  COMPARE(ldnt1h(z30.VnH(), p4.Zeroing(), SVEMemOperand(x6, x11, LSL, 1)),
          "ldnt1h {z30.h}, p4/z, [x6, x11, lsl #1]");
  COMPARE(ldnt1w(z0.VnS(), p4.Zeroing(), SVEMemOperand(x11, x1, LSL, 2)),
          "ldnt1w {z0.s}, p4/z, [x11, x1, lsl #2]");
  COMPARE(ldnt1w(z0.VnS(), p4.Zeroing(), SVEMemOperand(sp, xzr, LSL, 2)),
          "ldnt1w {z0.s}, p4/z, [sp, xzr, lsl #2]");

  COMPARE_MACRO(Ldnt1b(z0.VnB(), p0.Zeroing(), SVEMemOperand(x0, x1)),
                "ldnt1b {z0.b}, p0/z, [x0, x1]");
  COMPARE_MACRO(Ldnt1h(z0.VnH(), p0.Zeroing(), SVEMemOperand(x0, x1, LSL, 1)),
                "ldnt1h {z0.h}, p0/z, [x0, x1, lsl #1]");
  COMPARE_MACRO(Ldnt1w(z0.VnS(), p0.Zeroing(), SVEMemOperand(x0, x1, LSL, 2)),
                "ldnt1w {z0.s}, p0/z, [x0, x1, lsl #2]");
  COMPARE_MACRO(Ldnt1d(z0.VnD(), p0.Zeroing(), SVEMemOperand(x0, x1, LSL, 3)),
                "ldnt1d {z0.d}, p0/z, [x0, x1, lsl #3]");

  COMPARE_MACRO(Ldnt1w(z0.VnS(), p0.Zeroing(), SVEMemOperand(x0, x1, LSL, 3)),
                "add x16, x0, x1, lsl #3\n"
                "ldnt1w {z0.s}, p0/z, [x16]");
  COMPARE_MACRO(Ldnt1d(z0.VnD(), p0.Zeroing(), SVEMemOperand(x0, x1, LSL, 4)),
                "add x16, x0, x1, lsl #4\n"
                "ldnt1d {z0.d}, p0/z, [x16]");

  COMPARE(ldnt1b(z1.VnB(), p3.Zeroing(), SVEMemOperand(x11)),
          "ldnt1b {z1.b}, p3/z, [x11]");
  COMPARE(ldnt1b(z2.VnB(), p2.Zeroing(), SVEMemOperand(x12, -8, SVE_MUL_VL)),
          "ldnt1b {z2.b}, p2/z, [x12, #-8, mul vl]");
  COMPARE(ldnt1d(z2.VnD(), p7.Zeroing(), SVEMemOperand(x13, -2, SVE_MUL_VL)),
          "ldnt1d {z2.d}, p7/z, [x13, #-2, mul vl]");
  COMPARE(ldnt1h(z26.VnH(), p4.Zeroing(), SVEMemOperand(x16, 3, SVE_MUL_VL)),
          "ldnt1h {z26.h}, p4/z, [x16, #3, mul vl]");
  COMPARE(ldnt1w(z17.VnS(), p4.Zeroing(), SVEMemOperand(x15, 7, SVE_MUL_VL)),
          "ldnt1w {z17.s}, p4/z, [x15, #7, mul vl]");
  COMPARE(ldnt1w(z17.VnS(), p4.Zeroing(), SVEMemOperand(sp, 7, SVE_MUL_VL)),
          "ldnt1w {z17.s}, p4/z, [sp, #7, mul vl]");

  COMPARE_MACRO(Ldnt1b(z2.VnB(),
                       p0.Zeroing(),
                       SVEMemOperand(x10, 42, SVE_MUL_VL)),
                "mov x16, #0x2a\n"
                "rdvl x17, #1\n"
                "madd x16, x16, x17, x10\n"
                "ldnt1b {z2.b}, p0/z, [x16]");
  COMPARE_MACRO(Ldnt1h(z3.VnH(),
                       p1.Zeroing(),
                       SVEMemOperand(x11, 31, SVE_MUL_VL)),
                "addvl x16, x11, #31\n"
                "ldnt1h {z3.h}, p1/z, [x16]");
  COMPARE_MACRO(Ldnt1w(z4.VnS(),
                       p2.Zeroing(),
                       SVEMemOperand(x12, -35, SVE_MUL_VL)),
                "mov x16, #0xffffffffffffffdd\n"
                "rdvl x17, #1\n"
                "madd x16, x16, x17, x12\n"
                "ldnt1w {z4.s}, p2/z, [x16]");
  COMPARE_MACRO(Ldnt1d(z5.VnD(), p3.Zeroing(), SVEMemOperand(x13, 3)),
                "add x16, x13, #0x3 (3)\n"
                "ldnt1d {z5.d}, p3/z, [x16]");

  COMPARE(ldnf1b(z1.VnH(), p0.Zeroing(), SVEMemOperand(x25, -8, SVE_MUL_VL)),
          "ldnf1b {z1.h}, p0/z, [x25, #-8, mul vl]");
  COMPARE(ldnf1b(z0.VnS(), p0.Zeroing(), SVEMemOperand(x2, 7, SVE_MUL_VL)),
          "ldnf1b {z0.s}, p0/z, [x2, #7, mul vl]");
  COMPARE(ldnf1b(z31.VnD(), p6.Zeroing(), SVEMemOperand(x0, -7, SVE_MUL_VL)),
          "ldnf1b {z31.d}, p6/z, [x0, #-7, mul vl]");
  COMPARE(ldnf1b(z25.VnB(), p1.Zeroing(), SVEMemOperand(x5, 6, SVE_MUL_VL)),
          "ldnf1b {z25.b}, p1/z, [x5, #6, mul vl]");
  COMPARE(ldnf1d(z25.VnD(), p0.Zeroing(), SVEMemOperand(x11, -6, SVE_MUL_VL)),
          "ldnf1d {z25.d}, p0/z, [x11, #-6, mul vl]");
  COMPARE(ldnf1h(z22.VnH(), p4.Zeroing(), SVEMemOperand(x7, 5, SVE_MUL_VL)),
          "ldnf1h {z22.h}, p4/z, [x7, #5, mul vl]");
  COMPARE(ldnf1h(z7.VnS(), p2.Zeroing(), SVEMemOperand(x1, -5, SVE_MUL_VL)),
          "ldnf1h {z7.s}, p2/z, [x1, #-5, mul vl]");
  COMPARE(ldnf1h(z5.VnD(), p3.Zeroing(), SVEMemOperand(x29, 4, SVE_MUL_VL)),
          "ldnf1h {z5.d}, p3/z, [x29, #4, mul vl]");
  COMPARE(ldnf1sb(z12.VnH(), p5.Zeroing(), SVEMemOperand(x27, -4, SVE_MUL_VL)),
          "ldnf1sb {z12.h}, p5/z, [x27, #-4, mul vl]");
  COMPARE(ldnf1sb(z10.VnS(), p2.Zeroing(), SVEMemOperand(x13, 3, SVE_MUL_VL)),
          "ldnf1sb {z10.s}, p2/z, [x13, #3, mul vl]");
  COMPARE(ldnf1sb(z25.VnD(), p6.Zeroing(), SVEMemOperand(x26, -3, SVE_MUL_VL)),
          "ldnf1sb {z25.d}, p6/z, [x26, #-3, mul vl]");
  COMPARE(ldnf1sh(z3.VnS(), p5.Zeroing(), SVEMemOperand(x1, 2, SVE_MUL_VL)),
          "ldnf1sh {z3.s}, p5/z, [x1, #2, mul vl]");
  COMPARE(ldnf1sh(z8.VnD(), p6.Zeroing(), SVEMemOperand(x13, -2, SVE_MUL_VL)),
          "ldnf1sh {z8.d}, p6/z, [x13, #-2, mul vl]");
  COMPARE(ldnf1sw(z5.VnD(), p6.Zeroing(), SVEMemOperand(x2, 1, SVE_MUL_VL)),
          "ldnf1sw {z5.d}, p6/z, [x2, #1, mul vl]");
  COMPARE(ldnf1w(z11.VnS(), p3.Zeroing(), SVEMemOperand(sp, -1, SVE_MUL_VL)),
          "ldnf1w {z11.s}, p3/z, [sp, #-1, mul vl]");
  COMPARE(ldnf1w(z10.VnD(), p6.Zeroing(), SVEMemOperand(x12)),
          "ldnf1w {z10.d}, p6/z, [x12]");

  CLEANUP();
}

TEST(sve_mem_contiguous_store) {
  SETUP();

  COMPARE(stnt1b(z21.VnB(), p5.Zeroing(), SVEMemOperand(x1, x23)),
          "stnt1b {z21.b}, p5, [x1, x23]");
  COMPARE(stnt1d(z10.VnD(), p0.Zeroing(), SVEMemOperand(x23, x6, LSL, 3)),
          "stnt1d {z10.d}, p0, [x23, x6, lsl #3]");
  COMPARE(stnt1h(z30.VnH(), p4.Zeroing(), SVEMemOperand(x6, x11, LSL, 1)),
          "stnt1h {z30.h}, p4, [x6, x11, lsl #1]");
  COMPARE(stnt1w(z0.VnS(), p4.Zeroing(), SVEMemOperand(x11, x1, LSL, 2)),
          "stnt1w {z0.s}, p4, [x11, x1, lsl #2]");
  COMPARE(stnt1w(z0.VnS(), p4.Zeroing(), SVEMemOperand(sp, xzr, LSL, 2)),
          "stnt1w {z0.s}, p4, [sp, xzr, lsl #2]");

  COMPARE(stnt1b(z1.VnB(), p3.Zeroing(), SVEMemOperand(x11)),
          "stnt1b {z1.b}, p3, [x11]");
  COMPARE(stnt1b(z2.VnB(), p2.Zeroing(), SVEMemOperand(x12, -8, SVE_MUL_VL)),
          "stnt1b {z2.b}, p2, [x12, #-8, mul vl]");
  COMPARE(stnt1d(z2.VnD(), p7.Zeroing(), SVEMemOperand(x13, -2, SVE_MUL_VL)),
          "stnt1d {z2.d}, p7, [x13, #-2, mul vl]");
  COMPARE(stnt1h(z26.VnH(), p4.Zeroing(), SVEMemOperand(x16, 3, SVE_MUL_VL)),
          "stnt1h {z26.h}, p4, [x16, #3, mul vl]");
  COMPARE(stnt1w(z17.VnS(), p4.Zeroing(), SVEMemOperand(x15, 7, SVE_MUL_VL)),
          "stnt1w {z17.s}, p4, [x15, #7, mul vl]");
  COMPARE(stnt1w(z17.VnS(), p4.Zeroing(), SVEMemOperand(sp, 7, SVE_MUL_VL)),
          "stnt1w {z17.s}, p4, [sp, #7, mul vl]");

  COMPARE_MACRO(Stnt1b(z2.VnB(),
                       p0.Zeroing(),
                       SVEMemOperand(x10, 42, SVE_MUL_VL)),
                "mov x16, #0x2a\n"
                "rdvl x17, #1\n"
                "madd x16, x16, x17, x10\n"
                "stnt1b {z2.b}, p0, [x16]");
  COMPARE_MACRO(Stnt1h(z3.VnH(),
                       p1.Zeroing(),
                       SVEMemOperand(x11, 31, SVE_MUL_VL)),
                "addvl x16, x11, #31\n"
                "stnt1h {z3.h}, p1, [x16]");
  COMPARE_MACRO(Stnt1w(z4.VnS(),
                       p2.Zeroing(),
                       SVEMemOperand(x12, -35, SVE_MUL_VL)),
                "mov x16, #0xffffffffffffffdd\n"
                "rdvl x17, #1\n"
                "madd x16, x16, x17, x12\n"
                "stnt1w {z4.s}, p2, [x16]");

  COMPARE_MACRO(Stnt1b(z0.VnB(), p0.Zeroing(), SVEMemOperand(x0, x1)),
                "stnt1b {z0.b}, p0, [x0, x1]");
  COMPARE_MACRO(Stnt1h(z0.VnH(), p0.Zeroing(), SVEMemOperand(x0, x1, LSL, 1)),
                "stnt1h {z0.h}, p0, [x0, x1, lsl #1]");
  COMPARE_MACRO(Stnt1w(z0.VnS(), p0.Zeroing(), SVEMemOperand(x0, x1, LSL, 2)),
                "stnt1w {z0.s}, p0, [x0, x1, lsl #2]");
  COMPARE_MACRO(Stnt1d(z0.VnD(), p0.Zeroing(), SVEMemOperand(x0, x1, LSL, 3)),
                "stnt1d {z0.d}, p0, [x0, x1, lsl #3]");

  CLEANUP();
}

TEST(sve_load_broadcast_octo) {
  SETUP();

  COMPARE_MACRO(Ld1rob(z3.VnB(), p1.Zeroing(), SVEMemOperand(x0, x1)),
                "ld1rob {z3.b}, p1/z, [x0, x1]");
  COMPARE_MACRO(Ld1roh(z6.VnH(), p4.Zeroing(), SVEMemOperand(sp, x31, LSL, 1)),
                "ld1roh {z6.h}, p4/z, [sp]");
  COMPARE_MACRO(Ld1roh(z6.VnH(), p4.Zeroing(), SVEMemOperand(sp, x30, LSL, 1)),
                "ld1roh {z6.h}, p4/z, [sp, x30, lsl #1]");
  COMPARE_MACRO(Ld1row(z2.VnS(), p6.Zeroing(), SVEMemOperand(x30, x30, LSL, 2)),
                "ld1row {z2.s}, p6/z, [x30, x30, lsl #2]");
  COMPARE_MACRO(Ld1rod(z30.VnD(), p7.Zeroing(), SVEMemOperand(x21, x5, LSL, 3)),
                "ld1rod {z30.d}, p7/z, [x21, x5, lsl #3]");
  COMPARE_MACRO(Ld1rob(z9.VnB(), p0.Zeroing(), SVEMemOperand(sp, 32)),
                "ld1rob {z9.b}, p0/z, [sp, #32]");
  COMPARE_MACRO(Ld1roh(z19.VnH(), p3.Zeroing(), SVEMemOperand(x4)),
                "ld1roh {z19.h}, p3/z, [x4]");
  COMPARE_MACRO(Ld1row(z21.VnS(), p3.Zeroing(), SVEMemOperand(x11, 224)),
                "ld1row {z21.s}, p3/z, [x11, #224]");
  COMPARE_MACRO(Ld1rod(z0.VnD(), p2.Zeroing(), SVEMemOperand(x16, -256)),
                "ld1rod {z0.d}, p2/z, [x16, #-256]");

  CLEANUP();
}

TEST(sve_ldr_str_simple) {
  SETUP();

  COMPARE(str(p14, SVEMemOperand(x0)), "str p14, [x0]");
  COMPARE(str(z14, SVEMemOperand(sp)), "str z14, [sp]");
  COMPARE(ldr(p4, SVEMemOperand(x0)), "ldr p4, [x0]");
  COMPARE(ldr(z4, SVEMemOperand(sp)), "ldr z4, [sp]");
  COMPARE(str(p15, SVEMemOperand(sp, -256, SVE_MUL_VL)),
          "str p15, [sp, #-256, mul vl]");
  COMPARE(str(z16, SVEMemOperand(x13, 255, SVE_MUL_VL)),
          "str z16, [x13, #255, mul vl]");
  COMPARE(ldr(p5, SVEMemOperand(sp, -42, SVE_MUL_VL)),
          "ldr p5, [sp, #-42, mul vl]");
  COMPARE(ldr(z6, SVEMemOperand(x28, 42, SVE_MUL_VL)),
          "ldr z6, [x28, #42, mul vl]");

  COMPARE_MACRO(Str(p14, SVEMemOperand(x0)), "str p14, [x0]");
  COMPARE_MACRO(Str(z14, SVEMemOperand(sp)), "str z14, [sp]");
  COMPARE_MACRO(Ldr(p4, SVEMemOperand(x0)), "ldr p4, [x0]");
  COMPARE_MACRO(Ldr(z4, SVEMemOperand(sp)), "ldr z4, [sp]");
  COMPARE_MACRO(Str(p15, SVEMemOperand(sp, -256, SVE_MUL_VL)),
                "str p15, [sp, #-256, mul vl]");
  COMPARE_MACRO(Str(z16, SVEMemOperand(x13, 255, SVE_MUL_VL)),
                "str z16, [x13, #255, mul vl]");
  COMPARE_MACRO(Ldr(p5, SVEMemOperand(sp, -42, SVE_MUL_VL)),
                "ldr p5, [sp, #-42, mul vl]");
  COMPARE_MACRO(Ldr(z6, SVEMemOperand(x28, 42, SVE_MUL_VL)),
                "ldr z6, [x28, #42, mul vl]");

  COMPARE_MACRO(Ldr(z6, SVEMemOperand(x28, 42, SVE_MUL_VL)),
                "ldr z6, [x28, #42, mul vl]");

  // IsEquivalentToScalar
  COMPARE_MACRO(Str(p0, SVEMemOperand(x0, xzr)), "str p0, [x0]");
  COMPARE_MACRO(Ldr(p1, SVEMemOperand(sp, xzr)), "ldr p1, [sp]");
  COMPARE_MACRO(Str(z2, SVEMemOperand(x12, xzr)), "str z2, [x12]");
  COMPARE_MACRO(Ldr(z3, SVEMemOperand(x7, xzr)), "ldr z3, [x7]");

  // Other cases fall back on Adr. We test Adr separately, so here we just test
  // sequences that stress scratch register allocation.
  COMPARE_MACRO(Str(p4, SVEMemOperand(x5, 4242, SVE_MUL_VL)),
                "mov x16, #0x1092\n"
                "rdvl x17, #1\n"
                "mul x16, x16, x17\n"
                "add x16, x5, x16, asr #3\n"
                "str p4, [x16]");
  COMPARE_MACRO(Ldr(p6, SVEMemOperand(sp, 4242, SVE_MUL_VL)),
                "mov x16, #0x1092\n"
                "rdvl x17, #1\n"
                "mul x16, x16, x17\n"
                "asr x16, x16, #3\n"
                "add x16, sp, x16\n"
                "ldr p6, [x16]");
  COMPARE_MACRO(Str(z7, SVEMemOperand(sp, 4242, SVE_MUL_VL)),
                "mov x16, #0x1092\n"
                "rdvl x17, #1\n"
                "mul x16, x16, x17\n"
                "add x16, sp, x16\n"
                "str z7, [x16]");
  COMPARE_MACRO(Ldr(z8, SVEMemOperand(x9, 4242, SVE_MUL_VL)),
                "mov x16, #0x1092\n"
                "rdvl x17, #1\n"
                "madd x16, x16, x17, x9\n"
                "ldr z8, [x16]");

  CLEANUP();
}

TEST(sve_ld1_st1) {
  SETUP();

  COMPARE(st1b(z11.VnB(), p0, SVEMemOperand(x22)), "st1b {z11.b}, p0, [x22]");
  COMPARE(st1b(z15.VnH(), p1, SVEMemOperand(x15, 7, SVE_MUL_VL)),
          "st1b {z15.h}, p1, [x15, #7, mul vl]");
  COMPARE(st1b(z19.VnS(), p2, SVEMemOperand(sp, -8, SVE_MUL_VL)),
          "st1b {z19.s}, p2, [sp, #-8, mul vl]");
  COMPARE(st1b(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
          "st1b {z23.d}, p3, [x1]");
  COMPARE(st1b(z2.VnB(), p4, SVEMemOperand(x1, x2)),
          "st1b {z2.b}, p4, [x1, x2]");
  COMPARE(st1b(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 0)),
          "st1b {z31.d}, p7, [x9, x9]");
  COMPARE(st1b(z3.VnS(), p0, SVEMemOperand(z14.VnS(), 30)),
          "st1b {z3.s}, p0, [z14.s, #30]");
  COMPARE(st1b(z14.VnD(), p4, SVEMemOperand(z3.VnD(), 31)),
          "st1b {z14.d}, p4, [z3.d, #31]");
  COMPARE(st1b(z15.VnD(), p5, SVEMemOperand(x0, z5.VnD())),
          "st1b {z15.d}, p5, [x0, z5.d]");
  COMPARE(st1b(z15.VnS(), p5, SVEMemOperand(sp, z2.VnS(), UXTW)),
          "st1b {z15.s}, p5, [sp, z2.s, uxtw]");
  COMPARE(st1b(z15.VnD(), p5, SVEMemOperand(x0, z25.VnD(), SXTW)),
          "st1b {z15.d}, p5, [x0, z25.d, sxtw]");

  COMPARE(st1h(z15.VnH(), p1, SVEMemOperand(x15, 7, SVE_MUL_VL)),
          "st1h {z15.h}, p1, [x15, #7, mul vl]");
  COMPARE(st1h(z19.VnS(), p2, SVEMemOperand(sp, -8, SVE_MUL_VL)),
          "st1h {z19.s}, p2, [sp, #-8, mul vl]");
  COMPARE(st1h(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
          "st1h {z23.d}, p3, [x1]");
  COMPARE(st1h(z2.VnH(), p4, SVEMemOperand(x1, x2, LSL, 1)),
          "st1h {z2.h}, p4, [x1, x2, lsl #1]");
  COMPARE(st1h(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 1)),
          "st1h {z31.d}, p7, [x9, x9, lsl #1]");
  COMPARE(st1h(z3.VnS(), p0, SVEMemOperand(z14.VnS(), 30)),
          "st1h {z3.s}, p0, [z14.s, #30]");
  COMPARE(st1h(z14.VnD(), p4, SVEMemOperand(z3.VnD(), 62)),
          "st1h {z14.d}, p4, [z3.d, #62]");
  COMPARE(st1h(z15.VnD(), p6, SVEMemOperand(sp, z6.VnD())),
          "st1h {z15.d}, p6, [sp, z6.d]");
  COMPARE(st1h(z15.VnD(), p6, SVEMemOperand(sp, z6.VnD(), LSL, 1)),
          "st1h {z15.d}, p6, [sp, z6.d, lsl #1]");
  COMPARE(st1h(z15.VnS(), p3, SVEMemOperand(x25, z3.VnS(), SXTW)),
          "st1h {z15.s}, p3, [x25, z3.s, sxtw]");
  COMPARE(st1h(z15.VnS(), p6, SVEMemOperand(x7, z15.VnS(), SXTW, 1)),
          "st1h {z15.s}, p6, [x7, z15.s, sxtw #1]");
  COMPARE(st1h(z17.VnD(), p3, SVEMemOperand(sp, z26.VnD(), SXTW)),
          "st1h {z17.d}, p3, [sp, z26.d, sxtw]");
  COMPARE(st1h(z15.VnD(), p6, SVEMemOperand(x13, z9.VnD(), UXTW, 1)),
          "st1h {z15.d}, p6, [x13, z9.d, uxtw #1]");

  COMPARE(st1w(z19.VnS(), p2, SVEMemOperand(sp, -8, SVE_MUL_VL)),
          "st1w {z19.s}, p2, [sp, #-8, mul vl]");
  COMPARE(st1w(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
          "st1w {z23.d}, p3, [x1]");
  COMPARE(st1w(z2.VnS(), p4, SVEMemOperand(x1, x2, LSL, 2)),
          "st1w {z2.s}, p4, [x1, x2, lsl #2]");
  COMPARE(st1w(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 2)),
          "st1w {z31.d}, p7, [x9, x9, lsl #2]");
  COMPARE(st1w(z3.VnS(), p0, SVEMemOperand(z14.VnS(), 32)),
          "st1w {z3.s}, p0, [z14.s, #32]");
  COMPARE(st1w(z14.VnD(), p4, SVEMemOperand(z3.VnD(), 124)),
          "st1w {z14.d}, p4, [z3.d, #124]");
  COMPARE(st1w(z17.VnD(), p2, SVEMemOperand(x30, z5.VnD())),
          "st1w {z17.d}, p2, [x30, z5.d]");
  COMPARE(st1w(z17.VnD(), p2, SVEMemOperand(x30, z5.VnD(), LSL, 2)),
          "st1w {z17.d}, p2, [x30, z5.d, lsl #2]");
  COMPARE(st1w(z15.VnS(), p7, SVEMemOperand(x26, z4.VnS(), UXTW)),
          "st1w {z15.s}, p7, [x26, z4.s, uxtw]");
  COMPARE(st1w(z15.VnS(), p4, SVEMemOperand(x8, z16.VnS(), UXTW, 2)),
          "st1w {z15.s}, p4, [x8, z16.s, uxtw #2]");
  COMPARE(st1w(z19.VnD(), p7, SVEMemOperand(x1, z27.VnD(), UXTW)),
          "st1w {z19.d}, p7, [x1, z27.d, uxtw]");
  COMPARE(st1w(z15.VnD(), p4, SVEMemOperand(sp, z10.VnD(), SXTW, 2)),
          "st1w {z15.d}, p4, [sp, z10.d, sxtw #2]");

  COMPARE(st1d(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
          "st1d {z23.d}, p3, [x1]");
  COMPARE(st1d(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 3)),
          "st1d {z31.d}, p7, [x9, x9, lsl #3]");
  COMPARE(st1d(z14.VnD(), p4, SVEMemOperand(z3.VnD(), 32)),
          "st1d {z14.d}, p4, [z3.d, #32]");
  COMPARE(st1d(z14.VnD(), p4, SVEMemOperand(z3.VnD(), 248)),
          "st1d {z14.d}, p4, [z3.d, #248]");
  COMPARE(st1d(z19.VnD(), p2, SVEMemOperand(x29, z22.VnD())),
          "st1d {z19.d}, p2, [x29, z22.d]");
  COMPARE(st1d(z19.VnD(), p2, SVEMemOperand(x29, z22.VnD(), LSL, 3)),
          "st1d {z19.d}, p2, [x29, z22.d, lsl #3]");
  COMPARE(st1d(z21.VnD(), p1, SVEMemOperand(x2, z28.VnD(), SXTW)),
          "st1d {z21.d}, p1, [x2, z28.d, sxtw]");
  COMPARE(st1d(z15.VnD(), p2, SVEMemOperand(x14, z11.VnD(), UXTW, 3)),
          "st1d {z15.d}, p2, [x14, z11.d, uxtw #3]");

  COMPARE(ld1b(z11.VnB(), p0.Zeroing(), SVEMemOperand(x22)),
          "ld1b {z11.b}, p0/z, [x22]");
  COMPARE(ld1b(z15.VnH(), p1.Zeroing(), SVEMemOperand(x15, 7, SVE_MUL_VL)),
          "ld1b {z15.h}, p1/z, [x15, #7, mul vl]");
  COMPARE(ld1b(z19.VnS(), p2.Zeroing(), SVEMemOperand(sp, -8, SVE_MUL_VL)),
          "ld1b {z19.s}, p2/z, [sp, #-8, mul vl]");
  COMPARE(ld1b(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
          "ld1b {z23.d}, p3/z, [x1]");
  COMPARE(ld1b(z2.VnB(), p4.Zeroing(), SVEMemOperand(x1, x2)),
          "ld1b {z2.b}, p4/z, [x1, x2]");
  COMPARE(ld1b(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 0)),
          "ld1b {z31.d}, p7/z, [x9, x9]");

  COMPARE(ld1h(z15.VnH(), p1.Zeroing(), SVEMemOperand(x15, 7, SVE_MUL_VL)),
          "ld1h {z15.h}, p1/z, [x15, #7, mul vl]");
  COMPARE(ld1h(z19.VnS(), p2.Zeroing(), SVEMemOperand(sp, -8, SVE_MUL_VL)),
          "ld1h {z19.s}, p2/z, [sp, #-8, mul vl]");
  COMPARE(ld1h(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
          "ld1h {z23.d}, p3/z, [x1]");
  COMPARE(ld1h(z2.VnH(), p4.Zeroing(), SVEMemOperand(x1, x2, LSL, 1)),
          "ld1h {z2.h}, p4/z, [x1, x2, lsl #1]");
  COMPARE(ld1h(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 1)),
          "ld1h {z31.d}, p7/z, [x9, x9, lsl #1]");

  COMPARE(ld1w(z19.VnS(), p2.Zeroing(), SVEMemOperand(sp, -8, SVE_MUL_VL)),
          "ld1w {z19.s}, p2/z, [sp, #-8, mul vl]");
  COMPARE(ld1w(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
          "ld1w {z23.d}, p3/z, [x1]");
  COMPARE(ld1w(z2.VnS(), p4.Zeroing(), SVEMemOperand(x1, x2, LSL, 2)),
          "ld1w {z2.s}, p4/z, [x1, x2, lsl #2]");
  COMPARE(ld1w(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 2)),
          "ld1w {z31.d}, p7/z, [x9, x9, lsl #2]");

  COMPARE(ld1d(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
          "ld1d {z23.d}, p3/z, [x1]");
  COMPARE(ld1d(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 3)),
          "ld1d {z31.d}, p7/z, [x9, x9, lsl #3]");

  COMPARE(ld1sb(z15.VnH(), p1.Zeroing(), SVEMemOperand(x15, 7, SVE_MUL_VL)),
          "ld1sb {z15.h}, p1/z, [x15, #7, mul vl]");
  COMPARE(ld1sb(z19.VnS(), p2.Zeroing(), SVEMemOperand(sp, -8, SVE_MUL_VL)),
          "ld1sb {z19.s}, p2/z, [sp, #-8, mul vl]");
  COMPARE(ld1d(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
          "ld1d {z23.d}, p3/z, [x1]");
  COMPARE(ld1sb(z5.VnH(), p1.Zeroing(), SVEMemOperand(x15, x1, LSL, 0)),
          "ld1sb {z5.h}, p1/z, [x15, x1]");
  COMPARE(ld1sb(z9.VnS(), p2.Zeroing(), SVEMemOperand(x29, x3, LSL, 0)),
          "ld1sb {z9.s}, p2/z, [x29, x3]");
  COMPARE(ld1sb(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 0)),
          "ld1sb {z31.d}, p7/z, [x9, x9]");

  COMPARE(ld1sh(z19.VnS(), p2.Zeroing(), SVEMemOperand(sp, -8, SVE_MUL_VL)),
          "ld1sh {z19.s}, p2/z, [sp, #-8, mul vl]");
  COMPARE(ld1sh(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
          "ld1sh {z23.d}, p3/z, [x1]");
  COMPARE(ld1sh(z11.VnS(), p4.Zeroing(), SVEMemOperand(x22, x10, LSL, 1)),
          "ld1sh {z11.s}, p4/z, [x22, x10, lsl #1]");
  COMPARE(ld1sh(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 1)),
          "ld1sh {z31.d}, p7/z, [x9, x9, lsl #1]");

  COMPARE(ld1sw(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
          "ld1sw {z23.d}, p3/z, [x1]");
  COMPARE(ld1sw(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 2)),
          "ld1sw {z31.d}, p7/z, [x9, x9, lsl #2]");

  CLEANUP();
}

TEST(sve_ld1_st1_macro) {
  SETUP();

  // Pass-through cases.
  COMPARE_MACRO(St1b(z11.VnB(), p0, SVEMemOperand(x22)),
                "st1b {z11.b}, p0, [x22]");
  COMPARE_MACRO(St1b(z15.VnH(), p1, SVEMemOperand(x15, 7, SVE_MUL_VL)),
                "st1b {z15.h}, p1, [x15, #7, mul vl]");
  COMPARE_MACRO(St1b(z19.VnS(), p2, SVEMemOperand(sp, -8, SVE_MUL_VL)),
                "st1b {z19.s}, p2, [sp, #-8, mul vl]");
  COMPARE_MACRO(St1b(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "st1b {z23.d}, p3, [x1]");
  COMPARE_MACRO(St1b(z2.VnB(), p4, SVEMemOperand(x1, x2)),
                "st1b {z2.b}, p4, [x1, x2]");
  COMPARE_MACRO(St1b(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 0)),
                "st1b {z31.d}, p7, [x9, x9]");
  COMPARE_MACRO(St1b(z3.VnS(), p6, SVEMemOperand(z4.VnS(), 22)),
                "st1b {z3.s}, p6, [z4.s, #22]");

  COMPARE_MACRO(St1h(z15.VnH(), p1, SVEMemOperand(x15, 7, SVE_MUL_VL)),
                "st1h {z15.h}, p1, [x15, #7, mul vl]");
  COMPARE_MACRO(St1h(z19.VnS(), p2, SVEMemOperand(sp, -8, SVE_MUL_VL)),
                "st1h {z19.s}, p2, [sp, #-8, mul vl]");
  COMPARE_MACRO(St1h(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "st1h {z23.d}, p3, [x1]");
  COMPARE_MACRO(St1h(z2.VnH(), p4, SVEMemOperand(x1, x2, LSL, 1)),
                "st1h {z2.h}, p4, [x1, x2, lsl #1]");
  COMPARE_MACRO(St1h(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 1)),
                "st1h {z31.d}, p7, [x9, x9, lsl #1]");
  COMPARE_MACRO(St1h(z3.VnD(), p5, SVEMemOperand(z0.VnD())),
                "st1h {z3.d}, p5, [z0.d]");

  COMPARE_MACRO(St1w(z19.VnS(), p2, SVEMemOperand(sp, -8, SVE_MUL_VL)),
                "st1w {z19.s}, p2, [sp, #-8, mul vl]");
  COMPARE_MACRO(St1w(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "st1w {z23.d}, p3, [x1]");
  COMPARE_MACRO(St1w(z2.VnS(), p4, SVEMemOperand(x1, x2, LSL, 2)),
                "st1w {z2.s}, p4, [x1, x2, lsl #2]");
  COMPARE_MACRO(St1w(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 2)),
                "st1w {z31.d}, p7, [x9, x9, lsl #2]");
  COMPARE_MACRO(St1w(z12.VnS(), p2, SVEMemOperand(z13.VnS(), 124)),
                "st1w {z12.s}, p2, [z13.s, #124]");

  COMPARE_MACRO(St1d(z23.VnD(), p3, SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "st1d {z23.d}, p3, [x1]");
  COMPARE_MACRO(St1d(z31.VnD(), p7, SVEMemOperand(x9, x9, LSL, 3)),
                "st1d {z31.d}, p7, [x9, x9, lsl #3]");
  COMPARE_MACRO(St1d(z13.VnD(), p3, SVEMemOperand(z12.VnD(), 248)),
                "st1d {z13.d}, p3, [z12.d, #248]");

  // SVEMemOperand synthesis.
  // Check that the MacroAssembler falls back on `CalculateSVEAddress` at the
  // boundary conditions. We test this helper independently.
  COMPARE_MACRO(St1b(z10.VnB(), p7, SVEMemOperand(x0, 8, SVE_MUL_VL)),
                "addvl x16, x0, #8\n"
                "st1b {z10.b}, p7, [x16]");
  COMPARE_MACRO(St1h(z11.VnS(), p5, SVEMemOperand(sp, -9, SVE_MUL_VL)),
                "mov x16, #0xffffffffffffffdc\n"
                "rdvl x17, #1\n"
                "mul x16, x16, x17\n"
                "asr x16, x16, #3\n"
                "add x16, sp, x16\n"
                "st1h {z11.s}, p5, [x16]");
  COMPARE_MACRO(St1w(z22.VnS(), p3, SVEMemOperand(sp, 42)),
                "add x16, sp, #0x2a (42)\n"
                "st1w {z22.s}, p3, [x16]");
  COMPARE_MACRO(St1d(z22.VnD(), p1, SVEMemOperand(x3, x4)),
                "add x16, x3, x4\n"
                "st1d {z22.d}, p1, [x16]");
  COMPARE_MACRO(St1b(z30.VnD(), p0, SVEMemOperand(x9, xzr)),
                "st1b {z30.d}, p0, [x9]");

// TODO: Fix these - they need scatter-store-to-scalar-plus-vector support.
#if 0
  COMPARE_MACRO(St1b(z1.VnD(), p6, SVEMemOperand(z0.VnD(), 32)), "mov x16, #0x20\n" "st1b {z1.d}, p6/z, [x16, z0.d]");
  COMPARE_MACRO(St1h(z1.VnS(), p6, SVEMemOperand(z0.VnS(), -1)), "mov x16, #0xffffffffffffffff\n" "st1h {z1.s}, p6/z, [x16, z0.s]");
#endif

  COMPARE_MACRO(Ld1b(z11.VnB(), p0.Zeroing(), SVEMemOperand(x22)),
                "ld1b {z11.b}, p0/z, [x22]");
  COMPARE_MACRO(Ld1b(z15.VnH(),
                     p1.Zeroing(),
                     SVEMemOperand(x15, 7, SVE_MUL_VL)),
                "ld1b {z15.h}, p1/z, [x15, #7, mul vl]");
  COMPARE_MACRO(Ld1b(z19.VnS(),
                     p2.Zeroing(),
                     SVEMemOperand(sp, -8, SVE_MUL_VL)),
                "ld1b {z19.s}, p2/z, [sp, #-8, mul vl]");
  COMPARE_MACRO(Ld1b(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "ld1b {z23.d}, p3/z, [x1]");
  COMPARE_MACRO(Ld1b(z2.VnB(), p4.Zeroing(), SVEMemOperand(x1, x2)),
                "ld1b {z2.b}, p4/z, [x1, x2]");
  COMPARE_MACRO(Ld1b(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 0)),
                "ld1b {z31.d}, p7/z, [x9, x9]");

  COMPARE_MACRO(Ld1h(z15.VnH(),
                     p1.Zeroing(),
                     SVEMemOperand(x15, 7, SVE_MUL_VL)),
                "ld1h {z15.h}, p1/z, [x15, #7, mul vl]");
  COMPARE_MACRO(Ld1h(z19.VnS(),
                     p2.Zeroing(),
                     SVEMemOperand(sp, -8, SVE_MUL_VL)),
                "ld1h {z19.s}, p2/z, [sp, #-8, mul vl]");
  COMPARE_MACRO(Ld1h(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "ld1h {z23.d}, p3/z, [x1]");
  COMPARE_MACRO(Ld1h(z2.VnH(), p4.Zeroing(), SVEMemOperand(x1, x2, LSL, 1)),
                "ld1h {z2.h}, p4/z, [x1, x2, lsl #1]");
  COMPARE_MACRO(Ld1h(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 1)),
                "ld1h {z31.d}, p7/z, [x9, x9, lsl #1]");

  COMPARE_MACRO(Ld1w(z19.VnS(),
                     p2.Zeroing(),
                     SVEMemOperand(sp, -8, SVE_MUL_VL)),
                "ld1w {z19.s}, p2/z, [sp, #-8, mul vl]");
  COMPARE_MACRO(Ld1w(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "ld1w {z23.d}, p3/z, [x1]");
  COMPARE_MACRO(Ld1w(z2.VnS(), p4.Zeroing(), SVEMemOperand(x1, x2, LSL, 2)),
                "ld1w {z2.s}, p4/z, [x1, x2, lsl #2]");
  COMPARE_MACRO(Ld1w(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 2)),
                "ld1w {z31.d}, p7/z, [x9, x9, lsl #2]");

  COMPARE_MACRO(Ld1d(z23.VnD(), p3.Zeroing(), SVEMemOperand(x1, 0, SVE_MUL_VL)),
                "ld1d {z23.d}, p3/z, [x1]");
  COMPARE_MACRO(Ld1d(z31.VnD(), p7.Zeroing(), SVEMemOperand(x9, x9, LSL, 3)),
                "ld1d {z31.d}, p7/z, [x9, x9, lsl #3]");

  // SVEMemOperand synthesis.
  // Check that the MacroAssembler falls back on `CalculateSVEAddress` at the
  // boundary conditions. We test this helper independently.
  COMPARE_MACRO(Ld1b(z10.VnB(), p7.Zeroing(), SVEMemOperand(x0, 8, SVE_MUL_VL)),
                "addvl x16, x0, #8\n"
                "ld1b {z10.b}, p7/z, [x16]");
  COMPARE_MACRO(Ld1h(z11.VnS(),
                     p5.Zeroing(),
                     SVEMemOperand(sp, -9, SVE_MUL_VL)),
                "mov x16, #0xffffffffffffffdc\n"
                "rdvl x17, #1\n"
                "mul x16, x16, x17\n"
                "asr x16, x16, #3\n"
                "add x16, sp, x16\n"
                "ld1h {z11.s}, p5/z, [x16]");
  COMPARE_MACRO(Ld1w(z22.VnS(), p3.Zeroing(), SVEMemOperand(sp, 42)),
                "add x16, sp, #0x2a (42)\n"
                "ld1w {z22.s}, p3/z, [x16]");
  COMPARE_MACRO(Ld1d(z22.VnD(), p1.Zeroing(), SVEMemOperand(x3, x4)),
                "add x16, x3, x4\n"
                "ld1d {z22.d}, p1/z, [x16]");
  COMPARE_MACRO(Ld1b(z30.VnD(), p0.Zeroing(), SVEMemOperand(x9, xzr)),
                "ld1b {z30.d}, p0/z, [x9]");
  CLEANUP();
}

TEST(sve_st2_scalar_plus_immediate) {
  SETUP();

  COMPARE(st2b(z31.VnB(), z0.VnB(), p6, SVEMemOperand(x19)),
          "st2b {z31.b, z0.b}, p6, [x19]");
  COMPARE(st2b(z31.VnB(), z0.VnB(), p6, SVEMemOperand(x19, 14, SVE_MUL_VL)),
          "st2b {z31.b, z0.b}, p6, [x19, #14, mul vl]");
  COMPARE(st2b(z15.VnB(), z16.VnB(), p6, SVEMemOperand(x19, -16, SVE_MUL_VL)),
          "st2b {z15.b, z16.b}, p6, [x19, #-16, mul vl]");

  COMPARE(st2h(z15.VnH(), z16.VnH(), p6, SVEMemOperand(x19)),
          "st2h {z15.h, z16.h}, p6, [x19]");
  COMPARE(st2h(z15.VnH(), z16.VnH(), p0, SVEMemOperand(x19, 14, SVE_MUL_VL)),
          "st2h {z15.h, z16.h}, p0, [x19, #14, mul vl]");
  COMPARE(st2h(z15.VnH(), z16.VnH(), p0, SVEMemOperand(x19, -16, SVE_MUL_VL)),
          "st2h {z15.h, z16.h}, p0, [x19, #-16, mul vl]");

  COMPARE(st2w(z0.VnS(), z1.VnS(), p0, SVEMemOperand(x19)),
          "st2w {z0.s, z1.s}, p0, [x19]");
  COMPARE(st2w(z0.VnS(), z1.VnS(), p0, SVEMemOperand(x19, 14, SVE_MUL_VL)),
          "st2w {z0.s, z1.s}, p0, [x19, #14, mul vl]");
  COMPARE(st2w(z0.VnS(), z1.VnS(), p7, SVEMemOperand(x19, -16, SVE_MUL_VL)),
          "st2w {z0.s, z1.s}, p7, [x19, #-16, mul vl]");

  COMPARE(st2d(z0.VnD(), z1.VnD(), p7, SVEMemOperand(x19)),
          "st2d {z0.d, z1.d}, p7, [x19]");
  COMPARE(st2d(z31.VnD(), z0.VnD(), p7, SVEMemOperand(x19, 14, SVE_MUL_VL)),
          "st2d {z31.d, z0.d}, p7, [x19, #14, mul vl]");
  COMPARE(st2d(z31.VnD(), z0.VnD(), p7, SVEMemOperand(x19, -16, SVE_MUL_VL)),
          "st2d {z31.d, z0.d}, p7, [x19, #-16, mul vl]");

  CLEANUP();
}

TEST(sve_st3_scalar_plus_immediate) {
  SETUP();

  COMPARE(st3b(z30.VnB(), z31.VnB(), z0.VnB(), p7, SVEMemOperand(x19)),
          "st3b {z30.b, z31.b, z0.b}, p7, [x19]");
  COMPARE(st3b(z30.VnB(),
               z31.VnB(),
               z0.VnB(),
               p6,
               SVEMemOperand(x19, 21, SVE_MUL_VL)),
          "st3b {z30.b, z31.b, z0.b}, p6, [x19, #21, mul vl]");
  COMPARE(st3b(z30.VnB(),
               z31.VnB(),
               z0.VnB(),
               p6,
               SVEMemOperand(x19, -24, SVE_MUL_VL)),
          "st3b {z30.b, z31.b, z0.b}, p6, [x19, #-24, mul vl]");

  COMPARE(st3h(z15.VnH(), z16.VnH(), z17.VnH(), p6, SVEMemOperand(x19)),
          "st3h {z15.h, z16.h, z17.h}, p6, [x19]");
  COMPARE(st3h(z15.VnH(),
               z16.VnH(),
               z17.VnH(),
               p6,
               SVEMemOperand(x19, 21, SVE_MUL_VL)),
          "st3h {z15.h, z16.h, z17.h}, p6, [x19, #21, mul vl]");
  COMPARE(st3h(z15.VnH(),
               z16.VnH(),
               z17.VnH(),
               p0,
               SVEMemOperand(x19, -24, SVE_MUL_VL)),
          "st3h {z15.h, z16.h, z17.h}, p0, [x19, #-24, mul vl]");

  COMPARE(st3w(z15.VnS(), z16.VnS(), z17.VnS(), p0, SVEMemOperand(x19)),
          "st3w {z15.s, z16.s, z17.s}, p0, [x19]");
  COMPARE(st3w(z0.VnS(),
               z1.VnS(),
               z2.VnS(),
               p0,
               SVEMemOperand(x19, 21, SVE_MUL_VL)),
          "st3w {z0.s, z1.s, z2.s}, p0, [x19, #21, mul vl]");
  COMPARE(st3w(z0.VnS(),
               z1.VnS(),
               z2.VnS(),
               p0,
               SVEMemOperand(x19, -24, SVE_MUL_VL)),
          "st3w {z0.s, z1.s, z2.s}, p0, [x19, #-24, mul vl]");

  COMPARE(st3d(z0.VnD(), z1.VnD(), z2.VnD(), p7, SVEMemOperand(x19)),
          "st3d {z0.d, z1.d, z2.d}, p7, [x19]");
  COMPARE(st3d(z0.VnD(),
               z1.VnD(),
               z2.VnD(),
               p7,
               SVEMemOperand(x19, 21, SVE_MUL_VL)),
          "st3d {z0.d, z1.d, z2.d}, p7, [x19, #21, mul vl]");
  COMPARE(st3d(z30.VnD(),
               z31.VnD(),
               z0.VnD(),
               p7,
               SVEMemOperand(x19, -24, SVE_MUL_VL)),
          "st3d {z30.d, z31.d, z0.d}, p7, [x19, #-24, mul vl]");

  CLEANUP();
}

TEST(sve_st4_scalar_plus_immediate) {
  SETUP();

  COMPARE(st4b(z31.VnB(), z0.VnB(), z1.VnB(), z2.VnB(), p7, SVEMemOperand(x19)),
          "st4b {z31.b, z0.b, z1.b, z2.b}, p7, [x19]");
  COMPARE(st4b(z31.VnB(),
               z0.VnB(),
               z1.VnB(),
               z2.VnB(),
               p7,
               SVEMemOperand(x19, 28, SVE_MUL_VL)),
          "st4b {z31.b, z0.b, z1.b, z2.b}, p7, [x19, #28, mul vl]");
  COMPARE(st4b(z31.VnB(),
               z0.VnB(),
               z1.VnB(),
               z2.VnB(),
               p6,
               SVEMemOperand(x19, -32, SVE_MUL_VL)),
          "st4b {z31.b, z0.b, z1.b, z2.b}, p6, [x19, #-32, mul vl]");

  COMPARE(st4h(z31.VnH(), z0.VnH(), z1.VnH(), z2.VnH(), p6, SVEMemOperand(x19)),
          "st4h {z31.h, z0.h, z1.h, z2.h}, p6, [x19]");
  COMPARE(st4h(z15.VnH(),
               z16.VnH(),
               z17.VnH(),
               z18.VnH(),
               p6,
               SVEMemOperand(x19, 28, SVE_MUL_VL)),
          "st4h {z15.h, z16.h, z17.h, z18.h}, p6, [x19, #28, mul vl]");
  COMPARE(st4h(z15.VnH(),
               z16.VnH(),
               z17.VnH(),
               z18.VnH(),
               p6,
               SVEMemOperand(x19, -32, SVE_MUL_VL)),
          "st4h {z15.h, z16.h, z17.h, z18.h}, p6, "
          "[x19, #-32, mul vl]");

  COMPARE(st4w(z15.VnS(),
               z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               p0,
               SVEMemOperand(x19)),
          "st4w {z15.s, z16.s, z17.s, z18.s}, p0, [x19]");
  COMPARE(st4w(z15.VnS(),
               z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               p0,
               SVEMemOperand(x19, 28, SVE_MUL_VL)),
          "st4w {z15.s, z16.s, z17.s, z18.s}, p0, [x19, #28, mul vl]");
  COMPARE(st4w(z0.VnS(),
               z1.VnS(),
               z2.VnS(),
               z3.VnS(),
               p0,
               SVEMemOperand(x19, -32, SVE_MUL_VL)),
          "st4w {z0.s, z1.s, z2.s, z3.s}, p0, [x19, #-32, mul vl]");

  COMPARE(st4d(z0.VnD(), z1.VnD(), z2.VnD(), z3.VnD(), p0, SVEMemOperand(x19)),
          "st4d {z0.d, z1.d, z2.d, z3.d}, p0, [x19]");
  COMPARE(st4d(z0.VnD(),
               z1.VnD(),
               z2.VnD(),
               z3.VnD(),
               p7,
               SVEMemOperand(x19, 28, SVE_MUL_VL)),
          "st4d {z0.d, z1.d, z2.d, z3.d}, p7, [x19, #28, mul vl]");
  COMPARE(st4d(z0.VnD(),
               z1.VnD(),
               z2.VnD(),
               z3.VnD(),
               p7,
               SVEMemOperand(x19, -32, SVE_MUL_VL)),
          "st4d {z0.d, z1.d, z2.d, z3.d}, p7, [x19, #-32, mul vl]");

  CLEANUP();
}

TEST(sve_st2_scalar_plus_scalar) {
  SETUP();

  COMPARE(st2b(z25.VnB(), z26.VnB(), p1, SVEMemOperand(x20, x19)),
          "st2b {z25.b, z26.b}, p1, [x20, x19]");
  COMPARE(st2b(z25.VnB(), z26.VnB(), p1, SVEMemOperand(sp, x19)),
          "st2b {z25.b, z26.b}, p1, [sp, x19]");
  COMPARE(st2b(z31.VnB(), z0.VnB(), p1, SVEMemOperand(sp, x19)),
          "st2b {z31.b, z0.b}, p1, [sp, x19]");

  COMPARE(st2h(z31.VnH(), z0.VnH(), p1, SVEMemOperand(x20, x19, LSL, 1)),
          "st2h {z31.h, z0.h}, p1, [x20, x19, lsl #1]");
  COMPARE(st2h(z31.VnH(), z0.VnH(), p7, SVEMemOperand(sp, x19, LSL, 1)),
          "st2h {z31.h, z0.h}, p7, [sp, x19, lsl #1]");
  COMPARE(st2h(z31.VnH(), z0.VnH(), p7, SVEMemOperand(sp, x19, LSL, 1)),
          "st2h {z31.h, z0.h}, p7, [sp, x19, lsl #1]");

  COMPARE(st2w(z16.VnS(), z17.VnS(), p7, SVEMemOperand(x20, x19, LSL, 2)),
          "st2w {z16.s, z17.s}, p7, [x20, x19, lsl #2]");
  COMPARE(st2w(z16.VnS(), z17.VnS(), p7, SVEMemOperand(sp, x19, LSL, 2)),
          "st2w {z16.s, z17.s}, p7, [sp, x19, lsl #2]");
  COMPARE(st2w(z16.VnS(), z17.VnS(), p0, SVEMemOperand(sp, x19, LSL, 2)),
          "st2w {z16.s, z17.s}, p0, [sp, x19, lsl #2]");

  COMPARE(st2d(z16.VnD(), z17.VnD(), p0, SVEMemOperand(x20, x19, LSL, 3)),
          "st2d {z16.d, z17.d}, p0, [x20, x19, lsl #3]");
  COMPARE(st2d(z25.VnD(), z26.VnD(), p0, SVEMemOperand(sp, x19, LSL, 3)),
          "st2d {z25.d, z26.d}, p0, [sp, x19, lsl #3]");
  COMPARE(st2d(z25.VnD(), z26.VnD(), p0, SVEMemOperand(sp, x19, LSL, 3)),
          "st2d {z25.d, z26.d}, p0, [sp, x19, lsl #3]");

  CLEANUP();
}

TEST(sve_st3_scalar_plus_scalar) {
  SETUP();

  COMPARE(st3b(z25.VnB(), z26.VnB(), z27.VnB(), p1, SVEMemOperand(x20, x19)),
          "st3b {z25.b, z26.b, z27.b}, p1, [x20, x19]");
  COMPARE(st3b(z25.VnB(), z26.VnB(), z27.VnB(), p1, SVEMemOperand(sp, x19)),
          "st3b {z25.b, z26.b, z27.b}, p1, [sp, x19]");
  COMPARE(st3b(z30.VnB(), z31.VnB(), z0.VnB(), p1, SVEMemOperand(sp, x19)),
          "st3b {z30.b, z31.b, z0.b}, p1, [sp, x19]");

  COMPARE(st3h(z30.VnH(),
               z31.VnH(),
               z0.VnH(),
               p1,
               SVEMemOperand(x20, x19, LSL, 1)),
          "st3h {z30.h, z31.h, z0.h}, p1, [x20, x19, lsl #1]");
  COMPARE(st3h(z30.VnH(),
               z31.VnH(),
               z0.VnH(),
               p7,
               SVEMemOperand(sp, x19, LSL, 1)),
          "st3h {z30.h, z31.h, z0.h}, p7, [sp, x19, lsl #1]");
  COMPARE(st3h(z30.VnH(),
               z31.VnH(),
               z0.VnH(),
               p7,
               SVEMemOperand(sp, x19, LSL, 1)),
          "st3h {z30.h, z31.h, z0.h}, p7, [sp, x19, lsl #1]");

  COMPARE(st3w(z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               p7,
               SVEMemOperand(x20, x19, LSL, 2)),
          "st3w {z16.s, z17.s, z18.s}, p7, [x20, x19, lsl #2]");
  COMPARE(st3w(z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               p7,
               SVEMemOperand(sp, x19, LSL, 2)),
          "st3w {z16.s, z17.s, z18.s}, p7, [sp, x19, lsl #2]");
  COMPARE(st3w(z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               p0,
               SVEMemOperand(sp, x19, LSL, 2)),
          "st3w {z16.s, z17.s, z18.s}, p0, [sp, x19, lsl #2]");

  COMPARE(st3d(z16.VnD(),
               z17.VnD(),
               z18.VnD(),
               p0,
               SVEMemOperand(x20, x19, LSL, 3)),
          "st3d {z16.d, z17.d, z18.d}, p0, [x20, x19, lsl #3]");
  COMPARE(st3d(z25.VnD(),
               z26.VnD(),
               z27.VnD(),
               p0,
               SVEMemOperand(sp, x19, LSL, 3)),
          "st3d {z25.d, z26.d, z27.d}, p0, [sp, x19, lsl #3]");
  COMPARE(st3d(z25.VnD(),
               z26.VnD(),
               z27.VnD(),
               p0,
               SVEMemOperand(sp, x19, LSL, 3)),
          "st3d {z25.d, z26.d, z27.d}, p0, [sp, x19, lsl #3]");

  CLEANUP();
}

TEST(sve_st4_scalar_plus_scalar) {
  SETUP();

  COMPARE(st4b(z25.VnB(),
               z26.VnB(),
               z27.VnB(),
               z28.VnB(),
               p0,
               SVEMemOperand(x20, x19)),
          "st4b {z25.b, z26.b, z27.b, z28.b}, p0, [x20, x19]");
  COMPARE(st4b(z25.VnB(),
               z26.VnB(),
               z27.VnB(),
               z28.VnB(),
               p1,
               SVEMemOperand(sp, x19)),
          "st4b {z25.b, z26.b, z27.b, z28.b}, p1, [sp, x19]");
  COMPARE(st4b(z25.VnB(),
               z26.VnB(),
               z27.VnB(),
               z28.VnB(),
               p1,
               SVEMemOperand(sp, x19)),
          "st4b {z25.b, z26.b, z27.b, z28.b}, p1, [sp, x19]");

  COMPARE(st4h(z31.VnH(),
               z0.VnH(),
               z1.VnH(),
               z2.VnH(),
               p1,
               SVEMemOperand(x20, x19, LSL, 1)),
          "st4h {z31.h, z0.h, z1.h, z2.h}, p1, [x20, x19, lsl #1]");
  COMPARE(st4h(z31.VnH(),
               z0.VnH(),
               z1.VnH(),
               z2.VnH(),
               p1,
               SVEMemOperand(sp, x19, LSL, 1)),
          "st4h {z31.h, z0.h, z1.h, z2.h}, p1, [sp, x19, lsl #1]");
  COMPARE(st4h(z31.VnH(),
               z0.VnH(),
               z1.VnH(),
               z2.VnH(),
               p7,
               SVEMemOperand(sp, x19, LSL, 1)),
          "st4h {z31.h, z0.h, z1.h, z2.h}, p7, [sp, x19, lsl #1]");

  COMPARE(st4w(z31.VnS(),
               z0.VnS(),
               z1.VnS(),
               z2.VnS(),
               p7,
               SVEMemOperand(x20, x19, LSL, 2)),
          "st4w {z31.s, z0.s, z1.s, z2.s}, p7, [x20, x19, lsl #2]");
  COMPARE(st4w(z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               z19.VnS(),
               p7,
               SVEMemOperand(sp, x19, LSL, 2)),
          "st4w {z16.s, z17.s, z18.s, z19.s}, p7, [sp, x19, lsl #2]");
  COMPARE(st4w(z16.VnS(),
               z17.VnS(),
               z18.VnS(),
               z19.VnS(),
               p7,
               SVEMemOperand(sp, x19, LSL, 2)),
          "st4w {z16.s, z17.s, z18.s, z19.s}, p7, [sp, x19, lsl #2]");

  COMPARE(st4d(z16.VnD(),
               z17.VnD(),
               z18.VnD(),
               z19.VnD(),
               p0,
               SVEMemOperand(x20, x19, LSL, 3)),
          "st4d {z16.d, z17.d, z18.d, z19.d}, p0, [x20, x19, lsl #3]");
  COMPARE(st4d(z16.VnD(),
               z17.VnD(),
               z18.VnD(),
               z19.VnD(),
               p0,
               SVEMemOperand(sp, x19, LSL, 3)),
          "st4d {z16.d, z17.d, z18.d, z19.d}, p0, [sp, x19, lsl #3]");
  COMPARE(st4d(z25.VnD(),
               z26.VnD(),
               z27.VnD(),
               z28.VnD(),
               p0,
               SVEMemOperand(sp, x19, LSL, 3)),
          "st4d {z25.d, z26.d, z27.d, z28.d}, p0, [sp, x19, lsl #3]");

  CLEANUP();
}

TEST(sve_mul_index) {
  SETUP();

  COMPARE(sdot(z17.VnD(), z21.VnH(), z15.VnH(), 0),
          "sdot z17.d, z21.h, z15.h[0]");
  COMPARE(sdot(z28.VnS(), z9.VnB(), z7.VnB(), 1), "sdot z28.s, z9.b, z7.b[1]");
  COMPARE(udot(z26.VnD(), z15.VnH(), z1.VnH(), 1),
          "udot z26.d, z15.h, z1.h[1]");
  COMPARE(udot(z23.VnS(), z24.VnB(), z5.VnB(), 3),
          "udot z23.s, z24.b, z5.b[3]");

  CLEANUP();
}

TEST(sve_mul_index_macro) {
  SETUP();

  COMPARE_MACRO(Sdot(z0.VnS(), z0.VnS(), z2.VnB(), z4.VnB(), 0),
                "sdot z0.s, z2.b, z4.b[0]");
  COMPARE_MACRO(Sdot(z3.VnD(), z4.VnD(), z3.VnH(), z5.VnH(), 1),
                "movprfx z31, z4\n"
                "sdot z31.d, z3.h, z5.h[1]\n"
                "mov z3.d, z31.d");
  COMPARE_MACRO(Sdot(z4.VnS(), z5.VnS(), z6.VnB(), z4.VnB(), 2),
                "movprfx z31, z5\n"
                "sdot z31.s, z6.b, z4.b[2]\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Sdot(z6.VnD(), z7.VnD(), z8.VnH(), z9.VnH(), 0),
                "movprfx z6, z7\n"
                "sdot z6.d, z8.h, z9.h[0]");
  COMPARE_MACRO(Sdot(z5.VnD(), z5.VnD(), z5.VnH(), z5.VnH(), 1),
                "sdot z5.d, z5.h, z5.h[1]");

  COMPARE_MACRO(Udot(z0.VnD(), z0.VnD(), z2.VnH(), z4.VnH(), 1),
                "udot z0.d, z2.h, z4.h[1]");
  COMPARE_MACRO(Udot(z3.VnS(), z4.VnS(), z3.VnB(), z5.VnB(), 3),
                "movprfx z31, z4\n"
                "udot z31.s, z3.b, z5.b[3]\n"
                "mov z3.d, z31.d");
  COMPARE_MACRO(Udot(z4.VnD(), z5.VnD(), z6.VnH(), z4.VnH(), 0),
                "movprfx z31, z5\n"
                "udot z31.d, z6.h, z4.h[0]\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Udot(z9.VnS(), z8.VnS(), z7.VnB(), z6.VnB(), 2),
                "movprfx z9, z8\n"
                "udot z9.s, z7.b, z6.b[2]");
  COMPARE_MACRO(Udot(z5.VnS(), z5.VnS(), z5.VnB(), z5.VnB(), 1),
                "udot z5.s, z5.b, z5.b[1]");
  CLEANUP();
}

TEST(sve_partition_break) {
  SETUP();

  COMPARE(brkas(p8.VnB(), p5.Zeroing(), p4.VnB()), "brkas p8.b, p5/z, p4.b");
  COMPARE(brka(p11.VnB(), p7.Zeroing(), p15.VnB()), "brka p11.b, p7/z, p15.b");
  COMPARE(brka(p12.VnB(), p8.Merging(), p13.VnB()), "brka p12.b, p8/m, p13.b");
  COMPARE(brkbs(p6.VnB(), p9.Zeroing(), p14.VnB()), "brkbs p6.b, p9/z, p14.b");
  COMPARE(brkb(p11.VnB(), p6.Zeroing(), p4.VnB()), "brkb p11.b, p6/z, p4.b");
  COMPARE(brkb(p12.VnB(), p7.Merging(), p5.VnB()), "brkb p12.b, p7/m, p5.b");
  COMPARE(brkns(p2.VnB(), p11.Zeroing(), p0.VnB(), p2.VnB()),
          "brkns p2.b, p11/z, p0.b, p2.b");
  COMPARE(brkn(p4.VnB(), p3.Zeroing(), p1.VnB(), p4.VnB()),
          "brkn p4.b, p3/z, p1.b, p4.b");

  COMPARE_MACRO(Brkns(p3.VnB(), p10.Zeroing(), p2.VnB(), p5.VnB()),
                "mov p3.b, p5.b\n"
                "brkns p3.b, p10/z, p2.b, p3.b");
  COMPARE_MACRO(Brkn(p5.VnB(), p4.Zeroing(), p3.VnB(), p7.VnB()),
                "mov p5.b, p7.b\n"
                "brkn p5.b, p4/z, p3.b, p5.b");

  CLEANUP();
}

TEST(sve_permute_predicate) {
  SETUP();

  COMPARE(rev(p15.VnB(), p6.VnB()), "rev p15.b, p6.b");
  COMPARE(rev(p15.VnH(), p6.VnH()), "rev p15.h, p6.h");
  COMPARE(rev(p15.VnS(), p6.VnS()), "rev p15.s, p6.s");
  COMPARE(rev(p15.VnD(), p6.VnD()), "rev p15.d, p6.d");
  COMPARE(trn1(p13.VnB(), p15.VnB(), p12.VnB()), "trn1 p13.b, p15.b, p12.b");
  COMPARE(trn1(p13.VnH(), p15.VnH(), p12.VnH()), "trn1 p13.h, p15.h, p12.h");
  COMPARE(trn1(p13.VnS(), p15.VnS(), p12.VnS()), "trn1 p13.s, p15.s, p12.s");
  COMPARE(trn1(p13.VnD(), p15.VnD(), p12.VnD()), "trn1 p13.d, p15.d, p12.d");
  COMPARE(trn2(p5.VnB(), p5.VnB(), p6.VnB()), "trn2 p5.b, p5.b, p6.b");
  COMPARE(trn2(p5.VnH(), p5.VnH(), p6.VnH()), "trn2 p5.h, p5.h, p6.h");
  COMPARE(trn2(p5.VnS(), p5.VnS(), p6.VnS()), "trn2 p5.s, p5.s, p6.s");
  COMPARE(trn2(p5.VnD(), p5.VnD(), p6.VnD()), "trn2 p5.d, p5.d, p6.d");
  COMPARE(uzp1(p14.VnB(), p4.VnB(), p14.VnB()), "uzp1 p14.b, p4.b, p14.b");
  COMPARE(uzp1(p14.VnH(), p4.VnH(), p14.VnH()), "uzp1 p14.h, p4.h, p14.h");
  COMPARE(uzp1(p14.VnS(), p4.VnS(), p14.VnS()), "uzp1 p14.s, p4.s, p14.s");
  COMPARE(uzp1(p14.VnD(), p4.VnD(), p14.VnD()), "uzp1 p14.d, p4.d, p14.d");
  COMPARE(uzp2(p6.VnB(), p11.VnB(), p2.VnB()), "uzp2 p6.b, p11.b, p2.b");
  COMPARE(uzp2(p6.VnH(), p11.VnH(), p2.VnH()), "uzp2 p6.h, p11.h, p2.h");
  COMPARE(uzp2(p6.VnS(), p11.VnS(), p2.VnS()), "uzp2 p6.s, p11.s, p2.s");
  COMPARE(uzp2(p6.VnD(), p11.VnD(), p2.VnD()), "uzp2 p6.d, p11.d, p2.d");
  COMPARE(zip1(p13.VnB(), p4.VnB(), p12.VnB()), "zip1 p13.b, p4.b, p12.b");
  COMPARE(zip1(p13.VnH(), p4.VnH(), p12.VnH()), "zip1 p13.h, p4.h, p12.h");
  COMPARE(zip1(p13.VnS(), p4.VnS(), p12.VnS()), "zip1 p13.s, p4.s, p12.s");
  COMPARE(zip1(p13.VnD(), p4.VnD(), p12.VnD()), "zip1 p13.d, p4.d, p12.d");
  COMPARE(zip2(p1.VnB(), p15.VnB(), p2.VnB()), "zip2 p1.b, p15.b, p2.b");
  COMPARE(zip2(p1.VnH(), p15.VnH(), p2.VnH()), "zip2 p1.h, p15.h, p2.h");
  COMPARE(zip2(p1.VnS(), p15.VnS(), p2.VnS()), "zip2 p1.s, p15.s, p2.s");
  COMPARE(zip2(p1.VnD(), p15.VnD(), p2.VnD()), "zip2 p1.d, p15.d, p2.d");
  COMPARE(punpkhi(p12.VnH(), p6.VnB()), "punpkhi p12.h, p6.b");
  COMPARE(punpklo(p4.VnH(), p14.VnB()), "punpklo p4.h, p14.b");

  CLEANUP();
}

TEST(sve_permute_vector_extract) {
  SETUP();

  COMPARE_MACRO(Ext(z10.VnB(), z10.VnB(), z2.VnB(), 0),
                "ext z10.b, z10.b, z2.b, #0");
  COMPARE_MACRO(Ext(z10.VnB(), z10.VnB(), z2.VnB(), 1),
                "ext z10.b, z10.b, z2.b, #1");
  COMPARE_MACRO(Ext(z2.VnB(), z2.VnB(), z10.VnB(), 254),
                "ext z2.b, z2.b, z10.b, #254");
  COMPARE_MACRO(Ext(z2.VnB(), z2.VnB(), z10.VnB(), 255),
                "ext z2.b, z2.b, z10.b, #255");

  CLEANUP();
}

TEST(sve_permute_vector_interleaving) {
  SETUP();

  COMPARE(trn1(z25.VnB(), z31.VnB(), z17.VnB()), "trn1 z25.b, z31.b, z17.b");
  COMPARE(trn1(z25.VnH(), z31.VnH(), z17.VnH()), "trn1 z25.h, z31.h, z17.h");
  COMPARE(trn1(z25.VnS(), z31.VnS(), z17.VnS()), "trn1 z25.s, z31.s, z17.s");
  COMPARE(trn1(z25.VnD(), z31.VnD(), z17.VnD()), "trn1 z25.d, z31.d, z17.d");
  COMPARE(trn2(z23.VnB(), z19.VnB(), z5.VnB()), "trn2 z23.b, z19.b, z5.b");
  COMPARE(trn2(z23.VnH(), z19.VnH(), z5.VnH()), "trn2 z23.h, z19.h, z5.h");
  COMPARE(trn2(z23.VnS(), z19.VnS(), z5.VnS()), "trn2 z23.s, z19.s, z5.s");
  COMPARE(trn2(z23.VnD(), z19.VnD(), z5.VnD()), "trn2 z23.d, z19.d, z5.d");
  COMPARE(uzp1(z3.VnB(), z27.VnB(), z10.VnB()), "uzp1 z3.b, z27.b, z10.b");
  COMPARE(uzp1(z3.VnH(), z27.VnH(), z10.VnH()), "uzp1 z3.h, z27.h, z10.h");
  COMPARE(uzp1(z3.VnS(), z27.VnS(), z10.VnS()), "uzp1 z3.s, z27.s, z10.s");
  COMPARE(uzp1(z3.VnD(), z27.VnD(), z10.VnD()), "uzp1 z3.d, z27.d, z10.d");
  COMPARE(uzp2(z22.VnB(), z26.VnB(), z15.VnB()), "uzp2 z22.b, z26.b, z15.b");
  COMPARE(uzp2(z22.VnH(), z26.VnH(), z15.VnH()), "uzp2 z22.h, z26.h, z15.h");
  COMPARE(uzp2(z22.VnS(), z26.VnS(), z15.VnS()), "uzp2 z22.s, z26.s, z15.s");
  COMPARE(uzp2(z22.VnD(), z26.VnD(), z15.VnD()), "uzp2 z22.d, z26.d, z15.d");
  COMPARE(zip1(z31.VnB(), z2.VnB(), z20.VnB()), "zip1 z31.b, z2.b, z20.b");
  COMPARE(zip1(z31.VnH(), z2.VnH(), z20.VnH()), "zip1 z31.h, z2.h, z20.h");
  COMPARE(zip1(z31.VnS(), z2.VnS(), z20.VnS()), "zip1 z31.s, z2.s, z20.s");
  COMPARE(zip1(z31.VnD(), z2.VnD(), z20.VnD()), "zip1 z31.d, z2.d, z20.d");
  COMPARE(zip2(z15.VnB(), z23.VnB(), z12.VnB()), "zip2 z15.b, z23.b, z12.b");
  COMPARE(zip2(z15.VnH(), z23.VnH(), z12.VnH()), "zip2 z15.h, z23.h, z12.h");
  COMPARE(zip2(z15.VnS(), z23.VnS(), z12.VnS()), "zip2 z15.s, z23.s, z12.s");
  COMPARE(zip2(z15.VnD(), z23.VnD(), z12.VnD()), "zip2 z15.d, z23.d, z12.d");

  CLEANUP();
}

TEST(sve_cpy_reg) {
  SETUP();

  COMPARE(cpy(z1.VnB(), p2.Merging(), wsp), "mov z1.b, p2/m, wsp");
  COMPARE(cpy(z2.VnH(), p6.Merging(), w3), "mov z2.h, p6/m, w3");
  COMPARE(cpy(z3.VnS(), p7.Merging(), x5), "mov z3.s, p7/m, w5");
  COMPARE(cpy(z4.VnD(), p7.Merging(), x30), "mov z4.d, p7/m, x30");
  COMPARE(cpy(z5.VnD(), p7.Merging(), sp), "mov z5.d, p7/m, sp");

  COMPARE(cpy(z27.VnB(), p3.Merging(), b23), "mov z27.b, p3/m, b23");
  COMPARE(cpy(z27.VnH(), p3.Merging(), h23), "mov z27.h, p3/m, h23");
  COMPARE(cpy(z27.VnS(), p3.Merging(), s23), "mov z27.s, p3/m, s23");
  COMPARE(cpy(z27.VnD(), p3.Merging(), d23), "mov z27.d, p3/m, d23");

  COMPARE(mov(z1.VnB(), p2.Merging(), wsp), "mov z1.b, p2/m, wsp");
  COMPARE(mov(z4.VnD(), p7.Merging(), x30), "mov z4.d, p7/m, x30");
  COMPARE(mov(z5.VnD(), p7.Merging(), sp), "mov z5.d, p7/m, sp");
  COMPARE(mov(z27.VnB(), p3.Merging(), b23), "mov z27.b, p3/m, b23");
  COMPARE(mov(z27.VnD(), p3.Merging(), d23), "mov z27.d, p3/m, d23");

  COMPARE_MACRO(Mov(z1.VnB(), p2.Merging(), wsp), "mov z1.b, p2/m, wsp");
  COMPARE_MACRO(Mov(z4.VnD(), p7.Merging(), x30), "mov z4.d, p7/m, x30");
  COMPARE_MACRO(Mov(z5.VnD(), p7.Merging(), sp), "mov z5.d, p7/m, sp");
  COMPARE_MACRO(Mov(z27.VnB(), p3.Merging(), b23), "mov z27.b, p3/m, b23");
  COMPARE_MACRO(Mov(z27.VnD(), p3.Merging(), d23), "mov z27.d, p3/m, d23");

  CLEANUP();
}

TEST(sve_permute_vector_predicated) {
  SETUP();

  COMPARE(compact(z13.VnS(), p7, z1.VnS()), "compact z13.s, p7, z1.s");
  COMPARE(compact(z13.VnD(), p7, z1.VnD()), "compact z13.d, p7, z1.d");
  COMPARE(splice(z7.VnB(), p6, z7.VnB(), z2.VnB()),
          "splice z7.b, p6, z7.b, z2.b");
  COMPARE(splice(z7.VnH(), p6, z7.VnH(), z2.VnH()),
          "splice z7.h, p6, z7.h, z2.h");
  COMPARE(splice(z7.VnS(), p6, z7.VnS(), z2.VnS()),
          "splice z7.s, p6, z7.s, z2.s");
  COMPARE(splice(z7.VnD(), p6, z7.VnD(), z2.VnD()),
          "splice z7.d, p6, z7.d, z2.d");

  COMPARE_MACRO(Splice(z0.VnB(), p1, z2.VnB(), z4.VnB()),
                "movprfx z0, z2\n"
                "splice z0.b, p1, z0.b, z4.b");
  COMPARE_MACRO(Splice(z0.VnH(), p1, z2.VnH(), z0.VnH()),
                "movprfx z31, z2\n"
                "splice z31.h, p1, z31.h, z0.h\n"
                "mov z0.d, z31.d");

  COMPARE(clasta(z4.VnB(), p2, z4.VnB(), z12.VnB()),
          "clasta z4.b, p2, z4.b, z12.b");
  COMPARE(clasta(z4.VnH(), p2, z4.VnH(), z12.VnH()),
          "clasta z4.h, p2, z4.h, z12.h");
  COMPARE(clasta(z4.VnS(), p2, z4.VnS(), z12.VnS()),
          "clasta z4.s, p2, z4.s, z12.s");
  COMPARE(clasta(z4.VnD(), p2, z4.VnD(), z12.VnD()),
          "clasta z4.d, p2, z4.d, z12.d");
  COMPARE(clastb(z29.VnB(), p7, z29.VnB(), z26.VnB()),
          "clastb z29.b, p7, z29.b, z26.b");
  COMPARE(clastb(z29.VnH(), p7, z29.VnH(), z26.VnH()),
          "clastb z29.h, p7, z29.h, z26.h");
  COMPARE(clastb(z29.VnS(), p7, z29.VnS(), z26.VnS()),
          "clastb z29.s, p7, z29.s, z26.s");
  COMPARE(clastb(z29.VnD(), p7, z29.VnD(), z26.VnD()),
          "clastb z29.d, p7, z29.d, z26.d");

  COMPARE_MACRO(Clasta(z5.VnD(), p2, z4.VnD(), z12.VnD()),
                "movprfx z5, z4\n"
                "clasta z5.d, p2, z5.d, z12.d");
  COMPARE_MACRO(Clastb(z30.VnD(), p7, z29.VnD(), z26.VnD()),
                "movprfx z30, z29\n"
                "clastb z30.d, p7, z30.d, z26.d");
  COMPARE_MACRO(Clasta(z9.VnH(), p3, z8.VnH(), z9.VnH()),
                "movprfx z31, z8\n"
                "clasta z31.h, p3, z31.h, z9.h\n"
                "mov z9.d, z31.d");
  COMPARE_MACRO(Clastb(z1.VnS(), p1, z1.VnS(), z1.VnS()),
                "clastb z1.s, p1, z1.s, z1.s");

  COMPARE(clasta(w6, p0, w6, z13.VnB()), "clasta w6, p0, w6, z13.b");
  COMPARE(clasta(w6, p0, w6, z13.VnH()), "clasta w6, p0, w6, z13.h");
  COMPARE(clasta(w6, p0, w6, z13.VnS()), "clasta w6, p0, w6, z13.s");
  COMPARE(clasta(x6, p0, x6, z13.VnD()), "clasta x6, p0, x6, z13.d");
  COMPARE(clastb(w21, p2, w21, z27.VnB()), "clastb w21, p2, w21, z27.b");
  COMPARE(clastb(w21, p2, w21, z27.VnH()), "clastb w21, p2, w21, z27.h");
  COMPARE(clastb(w21, p2, w21, z27.VnS()), "clastb w21, p2, w21, z27.s");
  COMPARE(clastb(x21, p2, x21, z27.VnD()), "clastb x21, p2, x21, z27.d");

  COMPARE(clasta(b8, p6, b8, z7.VnB()), "clasta b8, p6, b8, z7.b");
  COMPARE(clasta(h8, p6, h8, z7.VnH()), "clasta h8, p6, h8, z7.h");
  COMPARE(clasta(s8, p6, s8, z7.VnS()), "clasta s8, p6, s8, z7.s");
  COMPARE(clasta(d8, p6, d8, z7.VnD()), "clasta d8, p6, d8, z7.d");
  COMPARE(clastb(b17, p0, b17, z19.VnB()), "clastb b17, p0, b17, z19.b");
  COMPARE(clastb(h17, p0, h17, z19.VnH()), "clastb h17, p0, h17, z19.h");
  COMPARE(clastb(s17, p0, s17, z19.VnS()), "clastb s17, p0, s17, z19.s");
  COMPARE(clastb(d17, p0, d17, z19.VnD()), "clastb d17, p0, d17, z19.d");

  COMPARE(lasta(w15, p3, z3.VnB()), "lasta w15, p3, z3.b");
  COMPARE(lasta(w15, p3, z3.VnH()), "lasta w15, p3, z3.h");
  COMPARE(lasta(w15, p3, z3.VnS()), "lasta w15, p3, z3.s");
  COMPARE(lasta(x15, p3, z3.VnD()), "lasta x15, p3, z3.d");
  COMPARE(lasta(b30, p4, z24.VnB()), "lasta b30, p4, z24.b");
  COMPARE(lasta(h30, p4, z24.VnH()), "lasta h30, p4, z24.h");
  COMPARE(lasta(s30, p4, z24.VnS()), "lasta s30, p4, z24.s");
  COMPARE(lasta(d30, p4, z24.VnD()), "lasta d30, p4, z24.d");

  COMPARE(lastb(w9, p2, z16.VnB()), "lastb w9, p2, z16.b");
  COMPARE(lastb(w9, p2, z16.VnH()), "lastb w9, p2, z16.h");
  COMPARE(lastb(w9, p2, z16.VnS()), "lastb w9, p2, z16.s");
  COMPARE(lastb(x9, p2, z16.VnD()), "lastb x9, p2, z16.d");
  COMPARE(lastb(b14, p5, z2.VnB()), "lastb b14, p5, z2.b");
  COMPARE(lastb(h14, p5, z2.VnH()), "lastb h14, p5, z2.h");
  COMPARE(lastb(s14, p5, z2.VnS()), "lastb s14, p5, z2.s");
  COMPARE(lastb(d14, p5, z2.VnD()), "lastb d14, p5, z2.d");

  CLEANUP();
}

TEST(sve_reverse) {
  SETUP();

  COMPARE_MACRO(Rbit(z22.VnB(), p2.Merging(), z24.VnB()),
                "rbit z22.b, p2/m, z24.b");
  COMPARE_MACRO(Rbit(z22.VnH(), p2.Merging(), z24.VnH()),
                "rbit z22.h, p2/m, z24.h");
  COMPARE_MACRO(Rbit(z22.VnS(), p2.Merging(), z24.VnS()),
                "rbit z22.s, p2/m, z24.s");
  COMPARE_MACRO(Rbit(z22.VnD(), p2.Merging(), z24.VnD()),
                "rbit z22.d, p2/m, z24.d");
  COMPARE_MACRO(Revb(z14.VnH(), p7.Merging(), z25.VnH()),
                "revb z14.h, p7/m, z25.h");
  COMPARE_MACRO(Revb(z14.VnS(), p7.Merging(), z25.VnS()),
                "revb z14.s, p7/m, z25.s");
  COMPARE_MACRO(Revb(z14.VnD(), p7.Merging(), z25.VnD()),
                "revb z14.d, p7/m, z25.d");
  COMPARE_MACRO(Revh(z16.VnS(), p2.Merging(), z4.VnS()),
                "revh z16.s, p2/m, z4.s");
  COMPARE_MACRO(Revh(z16.VnD(), p2.Merging(), z4.VnD()),
                "revh z16.d, p2/m, z4.d");
  COMPARE_MACRO(Revw(z26.VnD(), p5.Merging(), z10.VnD()),
                "revw z26.d, p5/m, z10.d");

  CLEANUP();
}

TEST(sve_permute_vector_unpredicated) {
  SETUP();

  COMPARE(dup(z4.VnB(), w7), "mov z4.b, w7");
  COMPARE(dup(z5.VnH(), w6), "mov z5.h, w6");
  COMPARE(dup(z6.VnS(), sp), "mov z6.s, wsp");
  COMPARE(dup(z7.VnD(), x4), "mov z7.d, x4");
  COMPARE(dup(z25.VnQ(), z28.VnQ(), 2), "mov z25.q, z28.q[2]");
  COMPARE(dup(z26.VnH(), z12.VnH(), 0), "mov z26.h, h12");

  COMPARE(mov(z4.VnB(), w7), "mov z4.b, w7");
  COMPARE(mov(z5.VnH(), w6), "mov z5.h, w6");
  COMPARE(mov(z6.VnS(), sp), "mov z6.s, wsp");
  COMPARE(mov(z7.VnD(), x4), "mov z7.d, x4");
  COMPARE(mov(z25.VnQ(), z28.VnQ(), 2), "mov z25.q, z28.q[2]");
  COMPARE(mov(z0.VnS(), s1), "mov z0.s, s1");

  COMPARE_MACRO(Mov(z7.VnD(), x4), "mov z7.d, x4");
  COMPARE_MACRO(Mov(z25.VnQ(), z28.VnQ(), 2), "mov z25.q, z28.q[2]");
  COMPARE_MACRO(Mov(z2.VnB(), b13), "mov z2.b, b13");
  COMPARE_MACRO(Mov(z31.VnQ(), q31), "mov z31.q, q31");

  // Test dup with reserved tsz field.
  COMPARE_PREFIX(dci(0x05202000), "unimplemented");

  COMPARE(insr(z15.VnB(), w13), "insr z15.b, w13");
  COMPARE(insr(z16.VnH(), w14), "insr z16.h, w14");
  COMPARE(insr(z17.VnS(), w15), "insr z17.s, w15");
  COMPARE(insr(z18.VnD(), x16), "insr z18.d, x16");
  COMPARE(insr(z5.VnB(), b3), "insr z5.b, b3");
  COMPARE(insr(z6.VnH(), h15), "insr z6.h, h15");
  COMPARE(insr(z7.VnS(), s22), "insr z7.s, s22");
  COMPARE(insr(z8.VnD(), d30), "insr z8.d, d30");
  COMPARE(rev(z13.VnB(), z10.VnB()), "rev z13.b, z10.b");
  COMPARE(rev(z14.VnH(), z10.VnH()), "rev z14.h, z10.h");
  COMPARE(rev(z15.VnS(), z10.VnS()), "rev z15.s, z10.s");
  COMPARE(rev(z16.VnD(), z10.VnD()), "rev z16.d, z10.d");
  COMPARE(sunpkhi(z10.VnH(), z11.VnB()), "sunpkhi z10.h, z11.b");
  COMPARE(sunpkhi(z11.VnS(), z11.VnH()), "sunpkhi z11.s, z11.h");
  COMPARE(sunpkhi(z12.VnD(), z11.VnS()), "sunpkhi z12.d, z11.s");
  COMPARE(sunpklo(z20.VnH(), z12.VnB()), "sunpklo z20.h, z12.b");
  COMPARE(sunpklo(z21.VnS(), z12.VnH()), "sunpklo z21.s, z12.h");
  COMPARE(sunpklo(z22.VnD(), z12.VnS()), "sunpklo z22.d, z12.s");
  COMPARE(uunpkhi(z17.VnH(), z14.VnB()), "uunpkhi z17.h, z14.b");
  COMPARE(uunpkhi(z18.VnS(), z14.VnH()), "uunpkhi z18.s, z14.h");
  COMPARE(uunpkhi(z19.VnD(), z14.VnS()), "uunpkhi z19.d, z14.s");
  COMPARE(uunpklo(z27.VnH(), z6.VnB()), "uunpklo z27.h, z6.b");
  COMPARE(uunpklo(z28.VnS(), z6.VnH()), "uunpklo z28.s, z6.h");
  COMPARE(uunpklo(z29.VnD(), z6.VnS()), "uunpklo z29.d, z6.s");
  COMPARE(tbl(z24.VnB(), z29.VnB(), z0.VnB()), "tbl z24.b, {z29.b}, z0.b");
  COMPARE(tbl(z25.VnH(), z29.VnH(), z1.VnH()), "tbl z25.h, {z29.h}, z1.h");
  COMPARE(tbl(z26.VnS(), z29.VnS(), z2.VnS()), "tbl z26.s, {z29.s}, z2.s");
  COMPARE(tbl(z27.VnD(), z29.VnD(), z3.VnD()), "tbl z27.d, {z29.d}, z3.d");

  CLEANUP();
}

TEST(sve_predicate_count) {
  SETUP();

  COMPARE(cntp(x9, p1, p0.VnB()), "cntp x9, p1, p0.b");
  COMPARE(cntp(x10, p12, p1.VnH()), "cntp x10, p12, p1.h");
  COMPARE(cntp(x11, p13, p14.VnS()), "cntp x11, p13, p14.s");
  COMPARE(cntp(x12, p4, p15.VnD()), "cntp x12, p4, p15.d");

  COMPARE_MACRO(Cntp(x0, p1, p2.VnB()), "cntp x0, p1, p2.b");
  COMPARE_MACRO(Cntp(w10, p11, p12.VnH()), "cntp x10, p11, p12.h");

  CLEANUP();
}

TEST(sve_predicate_logical_op) {
  SETUP();

  COMPARE(ands(p13.VnB(), p9.Zeroing(), p5.VnB(), p15.VnB()),
          "ands p13.b, p9/z, p5.b, p15.b");
  COMPARE(and_(p9.VnB(), p3.Zeroing(), p0.VnB(), p14.VnB()),
          "and p9.b, p3/z, p0.b, p14.b");
  COMPARE(bics(p8.VnB(), p5.Zeroing(), p3.VnB(), p1.VnB()),
          "bics p8.b, p5/z, p3.b, p1.b");
  COMPARE(bic(p5.VnB(), p5.Zeroing(), p9.VnB(), p9.VnB()),
          "bic p5.b, p5/z, p9.b, p9.b");
  COMPARE(eors(p11.VnB(), p1.Zeroing(), p1.VnB(), p2.VnB()),
          "eors p11.b, p1/z, p1.b, p2.b");
  COMPARE(eor(p8.VnB(), p6.Zeroing(), p1.VnB(), p11.VnB()),
          "eor p8.b, p6/z, p1.b, p11.b");
  COMPARE(nands(p13.VnB(), p0.Zeroing(), p9.VnB(), p4.VnB()),
          "nands p13.b, p0/z, p9.b, p4.b");
  COMPARE(nand(p7.VnB(), p7.Zeroing(), p15.VnB(), p2.VnB()),
          "nand p7.b, p7/z, p15.b, p2.b");
  COMPARE(nors(p8.VnB(), p8.Zeroing(), p12.VnB(), p11.VnB()),
          "nors p8.b, p8/z, p12.b, p11.b");
  COMPARE(nor(p3.VnB(), p6.Zeroing(), p15.VnB(), p12.VnB()),
          "nor p3.b, p6/z, p15.b, p12.b");
  COMPARE(orns(p10.VnB(), p11.Zeroing(), p0.VnB(), p15.VnB()),
          "orns p10.b, p11/z, p0.b, p15.b");
  COMPARE(orn(p0.VnB(), p1.Zeroing(), p7.VnB(), p4.VnB()),
          "orn p0.b, p1/z, p7.b, p4.b");
  COMPARE(orrs(p14.VnB(), p6.Zeroing(), p1.VnB(), p5.VnB()),
          "orrs p14.b, p6/z, p1.b, p5.b");
  COMPARE(orr(p13.VnB(), p7.Zeroing(), p10.VnB(), p4.VnB()),
          "orr p13.b, p7/z, p10.b, p4.b");
  COMPARE(sel(p9.VnB(), p15, p15.VnB(), p7.VnB()),
          "sel p9.b, p15, p15.b, p7.b");

  // Aliases.
  COMPARE(eor(p7.VnB(), p6.Zeroing(), p1.VnB(), p6.VnB()),
          "not p7.b, p6/z, p1.b");
  COMPARE(not_(p7.VnB(), p6.Zeroing(), p1.VnB()), "not p7.b, p6/z, p1.b");
  COMPARE(eors(p6.VnB(), p5.Zeroing(), p2.VnB(), p5.VnB()),
          "nots p6.b, p5/z, p2.b");
  COMPARE(nots(p6.VnB(), p5.Zeroing(), p2.VnB()), "nots p6.b, p5/z, p2.b");
  COMPARE(ands(p5.VnB(), p4.Zeroing(), p3.VnB(), p3.VnB()),
          "movs p5.b, p4/z, p3.b");
  COMPARE(movs(p5.VnB(), p4.Zeroing(), p3.VnB()), "movs p5.b, p4/z, p3.b");
  COMPARE(and_(p5.VnB(), p4.Zeroing(), p3.VnB(), p3.VnB()),
          "mov p5.b, p4/z, p3.b");
  COMPARE(mov(p5.VnB(), p4.Zeroing(), p3.VnB()), "mov p5.b, p4/z, p3.b");
  COMPARE(orrs(p4.VnB(), p3.Zeroing(), p3.VnB(), p3.VnB()), "movs p4.b, p3.b");
  COMPARE(movs(p4.VnB(), p3.VnB()), "movs p4.b, p3.b");
  COMPARE(orr(p4.VnB(), p3.Zeroing(), p3.VnB(), p3.VnB()), "mov p4.b, p3.b");
  COMPARE(mov(p4.VnB(), p3.VnB()), "mov p4.b, p3.b");
  COMPARE(sel(p3.VnB(), p2, p4.VnB(), p3.VnB()), "mov p3.b, p2/m, p4.b");
  COMPARE(mov(p3.VnB(), p2.Merging(), p4.VnB()), "mov p3.b, p2/m, p4.b");

  COMPARE_MACRO(Not(p7.VnB(), p6.Zeroing(), p1.VnB()), "not p7.b, p6/z, p1.b");
  COMPARE_MACRO(Nots(p6.VnB(), p5.Zeroing(), p2.VnB()),
                "nots p6.b, p5/z, p2.b");
  COMPARE_MACRO(Movs(p5.VnB(), p4.Zeroing(), p3.VnB()),
                "movs p5.b, p4/z, p3.b");
  COMPARE_MACRO(Mov(p5.VnB(), p4.Zeroing(), p3.VnB()), "mov p5.b, p4/z, p3.b");
  COMPARE_MACRO(Movs(p4.VnB(), p3.VnB()), "movs p4.b, p3.b");
  COMPARE_MACRO(Mov(p4, p3), "mov p4.b, p3.b");
  COMPARE_MACRO(Mov(p3.VnB(), p2.Merging(), p4.VnB()), "mov p3.b, p2/m, p4.b");

  CLEANUP();
}

TEST(sve_predicate_first_active) {
  SETUP();

  COMPARE(pfirst(p0.VnB(), p7, p0.VnB()), "pfirst p0.b, p7, p0.b");
  COMPARE(pfirst(p7.VnB(), p0, p7.VnB()), "pfirst p7.b, p0, p7.b");

  COMPARE_MACRO(Pfirst(p1.VnB(), p2, p1.VnB()), "pfirst p1.b, p2, p1.b");
  COMPARE_MACRO(Pfirst(p3.VnB(), p4, p5.VnB()),
                "mov p3.b, p5.b\n"
                "pfirst p3.b, p4, p3.b");

  {
    UseScratchRegisterScope temps(&masm);
    temps.Include(p7, p15);
    COMPARE_MACRO(Pfirst(p6.VnB(), p6, p0.VnB()),
                  "mov p15.b, p6.b\n"
                  "mov p6.b, p0.b\n"
                  "pfirst p6.b, p15, p6.b");
  }

  CLEANUP();
}

TEST(sve_predicate_next_active) {
  SETUP();

  COMPARE(pnext(p0.VnB(), p8, p0.VnB()), "pnext p0.b, p8, p0.b");
  COMPARE(pnext(p1.VnH(), p9, p1.VnH()), "pnext p1.h, p9, p1.h");
  COMPARE(pnext(p2.VnS(), p10, p2.VnS()), "pnext p2.s, p10, p2.s");
  COMPARE(pnext(p3.VnD(), p11, p3.VnD()), "pnext p3.d, p11, p3.d");

  COMPARE(pnext(p12.VnB(), p4, p12.VnB()), "pnext p12.b, p4, p12.b");
  COMPARE(pnext(p13.VnH(), p5, p13.VnH()), "pnext p13.h, p5, p13.h");
  COMPARE(pnext(p14.VnS(), p6, p14.VnS()), "pnext p14.s, p6, p14.s");
  COMPARE(pnext(p15.VnD(), p7, p15.VnD()), "pnext p15.d, p7, p15.d");

  COMPARE_MACRO(Pnext(p5.VnB(), p9, p5.VnB()), "pnext p5.b, p9, p5.b");
  COMPARE_MACRO(Pnext(p6.VnH(), p8, p6.VnH()), "pnext p6.h, p8, p6.h");
  COMPARE_MACRO(Pnext(p7.VnS(), p5, p7.VnS()), "pnext p7.s, p5, p7.s");
  COMPARE_MACRO(Pnext(p8.VnD(), p6, p8.VnD()), "pnext p8.d, p6, p8.d");

  COMPARE_MACRO(Pnext(p6.VnB(), p4, p5.VnB()),
                "mov p6.b, p5.b\n"
                "pnext p6.b, p4, p6.b");
  COMPARE_MACRO(Pnext(p7.VnH(), p3, p8.VnH()),
                "mov p7.b, p8.b\n"
                "pnext p7.h, p3, p7.h");
  COMPARE_MACRO(Pnext(p8.VnS(), p2, p9.VnS()),
                "mov p8.b, p9.b\n"
                "pnext p8.s, p2, p8.s");
  COMPARE_MACRO(Pnext(p9.VnD(), p1, p10.VnD()),
                "mov p9.b, p10.b\n"
                "pnext p9.d, p1, p9.d");

  {
    UseScratchRegisterScope temps(&masm);
    temps.Include(p7, p8);
    COMPARE_MACRO(Pnext(p6.VnB(), p6, p0.VnB()),
                  "mov p8.b, p6.b\n"
                  "mov p6.b, p0.b\n"
                  "pnext p6.b, p8, p6.b");
    COMPARE_MACRO(Pnext(p7.VnH(), p7, p1.VnH()),
                  "mov p8.b, p7.b\n"
                  "mov p7.b, p1.b\n"
                  "pnext p7.h, p8, p7.h");
    COMPARE_MACRO(Pnext(p10.VnS(), p10, p2.VnS()),
                  "mov p8.b, p10.b\n"
                  "mov p10.b, p2.b\n"
                  "pnext p10.s, p8, p10.s");
    COMPARE_MACRO(Pnext(p11.VnD(), p11, p3.VnD()),
                  "mov p8.b, p11.b\n"
                  "mov p11.b, p3.b\n"
                  "pnext p11.d, p8, p11.d");
  }

  CLEANUP();
}

TEST(sve_predicate_initialize) {
  SETUP();

  // Basic forms.
  COMPARE(ptrue(p0.VnB(), SVE_POW2), "ptrue p0.b, pow2");
  COMPARE(ptrue(p1.VnH(), SVE_VL1), "ptrue p1.h, vl1");
  COMPARE(ptrue(p2.VnS(), SVE_VL8), "ptrue p2.s, vl8");
  COMPARE(ptrue(p3.VnD(), SVE_VL16), "ptrue p3.d, vl16");
  COMPARE(ptrue(p4.VnB(), SVE_VL256), "ptrue p4.b, vl256");
  COMPARE(ptrue(p5.VnH(), SVE_MUL3), "ptrue p5.h, mul3");
  COMPARE(ptrue(p6.VnS(), SVE_MUL4), "ptrue p6.s, mul4");
  COMPARE(ptrue(p7.VnD(), SVE_ALL), "ptrue p7.d");

  COMPARE(ptrues(p8.VnB(), SVE_ALL), "ptrues p8.b");
  COMPARE(ptrues(p9.VnH(), SVE_MUL4), "ptrues p9.h, mul4");
  COMPARE(ptrues(p10.VnS(), SVE_MUL3), "ptrues p10.s, mul3");
  COMPARE(ptrues(p11.VnD(), SVE_VL256), "ptrues p11.d, vl256");
  COMPARE(ptrues(p12.VnB(), SVE_VL16), "ptrues p12.b, vl16");
  COMPARE(ptrues(p13.VnH(), SVE_VL8), "ptrues p13.h, vl8");
  COMPARE(ptrues(p14.VnS(), SVE_VL1), "ptrues p14.s, vl1");
  COMPARE(ptrues(p15.VnD(), SVE_POW2), "ptrues p15.d, pow2");

  // The Assembler supports arbitrary immediates.
  COMPARE(ptrue(p7.VnS(), 0xd), "ptrue p7.s, vl256");
  COMPARE(ptrue(p8.VnD(), 0xe), "ptrue p8.d, #0xe");
  COMPARE(ptrue(p9.VnB(), 0x15), "ptrue p9.b, #0x15");
  COMPARE(ptrue(p10.VnH(), 0x19), "ptrue p10.h, #0x19");
  COMPARE(ptrue(p11.VnS(), 0x1a), "ptrue p11.s, #0x1a");
  COMPARE(ptrue(p12.VnD(), 0x1c), "ptrue p12.d, #0x1c");
  COMPARE(ptrue(p13.VnB(), 0x1d), "ptrue p13.b, mul4");

  COMPARE(ptrues(p14.VnS(), 0xd), "ptrues p14.s, vl256");
  COMPARE(ptrues(p15.VnD(), 0xe), "ptrues p15.d, #0xe");
  COMPARE(ptrues(p0.VnB(), 0x15), "ptrues p0.b, #0x15");
  COMPARE(ptrues(p1.VnH(), 0x19), "ptrues p1.h, #0x19");
  COMPARE(ptrues(p2.VnS(), 0x1a), "ptrues p2.s, #0x1a");
  COMPARE(ptrues(p3.VnD(), 0x1c), "ptrues p3.d, #0x1c");
  COMPARE(ptrues(p4.VnB(), 0x1d), "ptrues p4.b, mul4");

  // SVE_ALL is the default.
  COMPARE(ptrue(p15.VnS()), "ptrue p15.s");
  COMPARE(ptrues(p0.VnS()), "ptrues p0.s");

  // The MacroAssembler provides a `FlagsUpdate` argument.
  COMPARE_MACRO(Ptrue(p0.VnB(), SVE_MUL3), "ptrue p0.b, mul3");
  COMPARE_MACRO(Ptrues(p1.VnH(), SVE_MUL4), "ptrues p1.h, mul4");
  COMPARE_MACRO(Ptrue(p2.VnS(), SVE_VL32, LeaveFlags), "ptrue p2.s, vl32");
  COMPARE_MACRO(Ptrue(p3.VnD(), SVE_VL64, SetFlags), "ptrues p3.d, vl64");
}

TEST(sve_pfalse) {
  SETUP();

  COMPARE(pfalse(p0.VnB()), "pfalse p0.b");
  COMPARE(pfalse(p15.VnB()), "pfalse p15.b");

  COMPARE_MACRO(Pfalse(p1.VnB()), "pfalse p1.b");
  COMPARE_MACRO(Pfalse(p4.VnH()), "pfalse p4.b");
  COMPARE_MACRO(Pfalse(p9.VnS()), "pfalse p9.b");
  COMPARE_MACRO(Pfalse(p14.VnD()), "pfalse p14.b");
}

TEST(sve_ptest) {
  SETUP();

  COMPARE(ptest(p15, p0.VnB()), "ptest p15, p0.b");
  COMPARE(ptest(p0, p15.VnB()), "ptest p0, p15.b");
  COMPARE(ptest(p6, p6.VnB()), "ptest p6, p6.b");

  COMPARE_MACRO(Ptest(p0, p1.VnB()), "ptest p0, p1.b");
}

TEST(sve_lane_size_relaxing) {
  SETUP();

  COMPARE_MACRO(Rdffr(p3), "rdffr p3.b");
  COMPARE_MACRO(Rdffr(p8.VnB()), "rdffr p8.b");

  COMPARE_MACRO(Wrffr(p9), "wrffr p9.b");
  COMPARE_MACRO(Wrffr(p8.VnB()), "wrffr p8.b");

  COMPARE_MACRO(And(z10, z1, z15), "and z10.d, z1.d, z15.d");
  COMPARE_MACRO(Bic(z11, z2, z16), "bic z11.d, z2.d, z16.d");
  COMPARE_MACRO(Eor(z12, z3, z17), "eor z12.d, z3.d, z17.d");
  COMPARE_MACRO(Orr(z13, z4, z18), "orr z13.d, z4.d, z18.d");

  COMPARE_MACRO(And(z10.VnD(), z1.VnD(), z15.VnD()), "and z10.d, z1.d, z15.d");
  COMPARE_MACRO(Bic(z11.VnS(), z2.VnS(), z16.VnS()), "bic z11.d, z2.d, z16.d");
  COMPARE_MACRO(Eor(z12.VnH(), z3.VnH(), z17.VnH()), "eor z12.d, z3.d, z17.d");
  COMPARE_MACRO(Orr(z13.VnB(), z4.VnB(), z18.VnB()), "orr z13.d, z4.d, z18.d");

  COMPARE_MACRO(Pfalse(p1), "pfalse p1.b");

  CLEANUP();
}

TEST(sve_read_ffr) {
  SETUP();

  COMPARE(rdffr(p13.VnB()), "rdffr p13.b");
  COMPARE(rdffrs(p14.VnB(), p9.Zeroing()), "rdffrs p14.b, p9/z");
  COMPARE(rdffr(p5.VnB(), p14.Zeroing()), "rdffr p5.b, p14/z");

  CLEANUP();
}

TEST(sve_propagate_break) {
  SETUP();

  COMPARE(brkpas(p12.VnB(), p0.Zeroing(), p12.VnB(), p11.VnB()),
          "brkpas p12.b, p0/z, p12.b, p11.b");
  COMPARE(brkpa(p1.VnB(), p2.Zeroing(), p13.VnB(), p8.VnB()),
          "brkpa p1.b, p2/z, p13.b, p8.b");
  COMPARE(brkpbs(p14.VnB(), p1.Zeroing(), p8.VnB(), p3.VnB()),
          "brkpbs p14.b, p1/z, p8.b, p3.b");
  COMPARE(brkpb(p2.VnB(), p5.Zeroing(), p0.VnB(), p14.VnB()),
          "brkpb p2.b, p5/z, p0.b, p14.b");

  CLEANUP();
}

TEST(sve_stack_allocation) {
  SETUP();

  COMPARE(rdvl(x26, 0), "rdvl x26, #0");
  COMPARE(rdvl(x27, 31), "rdvl x27, #31");
  COMPARE(rdvl(x28, -32), "rdvl x28, #-32");
  COMPARE(rdvl(xzr, 9), "rdvl xzr, #9");

  COMPARE(addvl(x6, x20, 0), "addvl x6, x20, #0");
  COMPARE(addvl(x7, x21, 31), "addvl x7, x21, #31");
  COMPARE(addvl(x8, x22, -32), "addvl x8, x22, #-32");
  COMPARE(addvl(sp, x1, 5), "addvl sp, x1, #5");
  COMPARE(addvl(x9, sp, -16), "addvl x9, sp, #-16");

  COMPARE(addpl(x20, x6, 0), "addpl x20, x6, #0");
  COMPARE(addpl(x21, x7, 31), "addpl x21, x7, #31");
  COMPARE(addpl(x22, x8, -32), "addpl x22, x8, #-32");
  COMPARE(addpl(sp, x1, 5), "addpl sp, x1, #5");
  COMPARE(addpl(x9, sp, -16), "addpl x9, sp, #-16");

  CLEANUP();
}

TEST(sve_rdvl_macro) {
  SETUP();

  // Encodable cases use rdvl directly.
  COMPARE_MACRO(Rdvl(x0, 3), "rdvl x0, #3");
  COMPARE_MACRO(Rdvl(x0, 31), "rdvl x0, #31");
  COMPARE_MACRO(Rdvl(x0, -32), "rdvl x0, #-32");

  // Unencodable cases fall back on `xn + (VL * multiplier)`.
  COMPARE_MACRO(Rdvl(x2, 0x1234),
                "mov x2, #0x1234\n"
                "rdvl x16, #1\n"
                "mul x2, x2, x16");

  CLEANUP();
}

TEST(sve_addvl_macro) {
  SETUP();

  // Encodable cases use addvl directly.
  COMPARE_MACRO(Addvl(sp, sp, -3), "addvl sp, sp, #-3");
  COMPARE_MACRO(Addvl(x0, x1, 8), "addvl x0, x1, #8");

  // If xn is xzr, `Addvl` behaves like `Rdvl`.
  COMPARE_MACRO(Addvl(x7, xzr, 8), "rdvl x7, #8");

  // Unencodable cases fall back on `xn + (VL * multiplier)`.
  COMPARE_MACRO(Addvl(x7, x8, 42),
                "mov x7, #0x2a\n"
                "rdvl x16, #1\n"
                "madd x7, x7, x16, x8");
  COMPARE_MACRO(Addvl(x10, x10, 42),
                "mov x16, #0x2a\n"
                "rdvl x17, #1\n"
                "madd x10, x16, x17, x10");
  COMPARE_MACRO(Addvl(x10, sp, 42),
                "mov x10, #0x2a\n"
                "rdvl x16, #1\n"
                "mul x10, x10, x16\n"
                "add x10, sp, x10");
  COMPARE_MACRO(Addvl(sp, x10, 42),
                "mov x16, #0x2a\n"
                "rdvl x17, #1\n"
                "mul x16, x16, x17\n"
                "add sp, x10, x16");

  CLEANUP();
}

TEST(sve_rdpl_macro) {
  SETUP();

  // There is no `rdpl` instruction. `Rdpl` is implemented as `Addpl` (with
  // xzr). However, since `addpl` operates on the stack pointer, some special
  // cases exist.

  // If the multiplier is a multiple of 8, `Rdpl` will pass through to `Rdvl`.
  COMPARE_MACRO(Rdpl(x0, 0), "rdvl x0, #0");
  COMPARE_MACRO(Rdpl(x0, 8), "rdvl x0, #1");

  // If the multiplier is encodable with `addpl`, we use that with an
  // explicitly-zeroed register.
  COMPARE_MACRO(Rdpl(x1, 7),
                "mov x1, #0x0\n"
                "addpl x1, x1, #7");
  COMPARE_MACRO(Rdpl(x1, -31),
                "mov x1, #0x0\n"
                "addpl x1, x1, #-31");

  // All other cases use `Rdvl`, and scale the result.
  COMPARE_MACRO(Rdpl(x2, 37),
                "mov x2, #0x25\n"
                "rdvl x16, #1\n"
                "mul x2, x2, x16\n"
                "asr x2, x2, #3");

  CLEANUP();
}

TEST(sve_addpl_macro) {
  SETUP();

  // Encodable cases use addpl directly.
  COMPARE_MACRO(Addpl(x22, x22, -3), "addpl x22, x22, #-3");
  COMPARE_MACRO(Addpl(x10, x11, 8), "addpl x10, x11, #8");
  COMPARE_MACRO(Addpl(x7, sp, 31), "addpl x7, sp, #31");

  // Otherwise, if the multiplier is a multiple of 8, `Addpl` will pass through
  // to `Addvl`.
  COMPARE_MACRO(Addpl(sp, x0, 48), "addvl sp, x0, #6");
  COMPARE_MACRO(Addpl(x2, sp, -48), "addvl x2, sp, #-6");

  // If xn is xzr, `Addpl` behaves like `Rdpl`.
  COMPARE_MACRO(Addpl(x7, xzr, 8), "rdvl x7, #1");
  COMPARE_MACRO(Addpl(x29, xzr, 13),
                "mov x29, #0x0\n"
                "addpl x29, x29, #13");

  // All other cases use `Rdvl`, and scale the result before adding it to `xn`.
  // Where possible, the scaling `asr` is merged with the `add`.
  COMPARE_MACRO(Addpl(x7, x8, 123),
                "mov x7, #0x7b\n"
                "rdvl x16, #1\n"
                "mul x7, x7, x16\n"
                "add x7, x8, x7, asr #3");
  COMPARE_MACRO(Addpl(x9, x9, 122),
                "mov x16, #0x7a\n"
                "rdvl x17, #1\n"
                "mul x16, x16, x17\n"
                "add x9, x9, x16, asr #3");
  // If the stack pointer is used, the `asr` and `add` must be separate.
  COMPARE_MACRO(Addpl(sp, x0, 33),
                "mov x16, #0x21\n"
                "rdvl x17, #1\n"
                "mul x16, x16, x17\n"
                "asr x16, x16, #3\n"
                "add sp, x0, x16");

  CLEANUP();
}

TEST(sve_vector_select) {
  SETUP();

  COMPARE_MACRO(Sel(z13.VnB(), p3, z3.VnB(), z25.VnB()),
                "sel z13.b, p3, z3.b, z25.b");
  COMPARE_MACRO(Sel(z13.VnH(), p3, z3.VnH(), z25.VnH()),
                "sel z13.h, p3, z3.h, z25.h");
  COMPARE_MACRO(Sel(z13.VnS(), p3, z3.VnS(), z25.VnS()),
                "sel z13.s, p3, z3.s, z25.s");
  COMPARE_MACRO(Sel(z13.VnD(), p3, z3.VnD(), z25.VnD()),
                "sel z13.d, p3, z3.d, z25.d");

  // Check sel() where zd == zm disassemble as predicated mov.
  COMPARE_MACRO(Sel(z1.VnB(), p4.Merging(), z30.VnB(), z1.VnB()),
                "mov z1.b, p4/m, z30.b");
  COMPARE_MACRO(Sel(z1.VnH(), p4.Merging(), z30.VnH(), z1.VnH()),
                "mov z1.h, p4/m, z30.h");
  COMPARE_MACRO(Sel(z1.VnS(), p4.Merging(), z30.VnS(), z1.VnS()),
                "mov z1.s, p4/m, z30.s");
  COMPARE_MACRO(Sel(z1.VnD(), p4.Merging(), z30.VnD(), z1.VnD()),
                "mov z1.d, p4/m, z30.d");

  // Check predicated mov() directly.
  COMPARE_MACRO(Mov(z2.VnB(), p10.Merging(), z22.VnB()),
                "mov z2.b, p10/m, z22.b");
  COMPARE_MACRO(Mov(z2.VnH(), p10.Merging(), z22.VnH()),
                "mov z2.h, p10/m, z22.h");
  COMPARE_MACRO(Mov(z2.VnS(), p10.Merging(), z22.VnS()),
                "mov z2.s, p10/m, z22.s");
  COMPARE_MACRO(Mov(z2.VnD(), p10.Merging(), z22.VnD()),
                "mov z2.d, p10/m, z22.d");

  CLEANUP();
}

TEST(sve_write_ffr) {
  SETUP();

  COMPARE_PREFIX(setffr(), "setffr");
  COMPARE(wrffr(p9.VnB()), "wrffr p9.b");

  CLEANUP();
}

TEST(sve2_match_nmatch) {
  SETUP();

  COMPARE(match(p15.VnB(), p1.Zeroing(), z18.VnB(), z5.VnB()),
          "match p15.b, p1/z, z18.b, z5.b");
  COMPARE(match(p15.VnH(), p1.Zeroing(), z18.VnH(), z5.VnH()),
          "match p15.h, p1/z, z18.h, z5.h");
  COMPARE(nmatch(p1.VnB(), p1.Zeroing(), z20.VnB(), z17.VnB()),
          "nmatch p1.b, p1/z, z20.b, z17.b");
  COMPARE(nmatch(p1.VnH(), p1.Zeroing(), z20.VnH(), z17.VnH()),
          "nmatch p1.h, p1/z, z20.h, z17.h");

  CLEANUP();
}

TEST(sve2_saba_uaba) {
  SETUP();

  COMPARE(saba(z13.VnB(), z2.VnB(), z31.VnB()), "saba z13.b, z2.b, z31.b");
  COMPARE(saba(z13.VnD(), z2.VnD(), z31.VnD()), "saba z13.d, z2.d, z31.d");
  COMPARE(saba(z13.VnH(), z2.VnH(), z31.VnH()), "saba z13.h, z2.h, z31.h");
  COMPARE(saba(z13.VnS(), z2.VnS(), z31.VnS()), "saba z13.s, z2.s, z31.s");
  COMPARE(uaba(z23.VnB(), z22.VnB(), z20.VnB()), "uaba z23.b, z22.b, z20.b");
  COMPARE(uaba(z23.VnD(), z22.VnD(), z20.VnD()), "uaba z23.d, z22.d, z20.d");
  COMPARE(uaba(z23.VnH(), z22.VnH(), z20.VnH()), "uaba z23.h, z22.h, z20.h");
  COMPARE(uaba(z23.VnS(), z22.VnS(), z20.VnS()), "uaba z23.s, z22.s, z20.s");

  COMPARE_MACRO(Saba(z12.VnB(), z12.VnB(), z3.VnB(), z30.VnB()),
                "saba z12.b, z3.b, z30.b");
  COMPARE_MACRO(Saba(z12.VnB(), z12.VnB(), z3.VnB(), z12.VnB()),
                "saba z12.b, z3.b, z12.b");
  COMPARE_MACRO(Saba(z12.VnB(), z12.VnB(), z12.VnB(), z30.VnB()),
                "saba z12.b, z12.b, z30.b");
  COMPARE_MACRO(Saba(z12.VnB(), z12.VnB(), z12.VnB(), z12.VnB()), "");
  COMPARE_MACRO(Saba(z12.VnB(), z13.VnB(), z3.VnB(), z30.VnB()),
                "movprfx z12, z13\n"
                "saba z12.b, z3.b, z30.b");
  COMPARE_MACRO(Saba(z12.VnB(), z3.VnB(), z3.VnB(), z30.VnB()),
                "movprfx z12, z3\n"
                "saba z12.b, z3.b, z30.b");
  COMPARE_MACRO(Saba(z12.VnB(), z30.VnB(), z3.VnB(), z30.VnB()),
                "movprfx z12, z30\n"
                "saba z12.b, z3.b, z30.b");
  COMPARE_MACRO(Saba(z12.VnB(), z3.VnB(), z3.VnB(), z3.VnB()),
                "mov z12.d, z3.d");
  COMPARE_MACRO(Saba(z12.VnB(), z3.VnB(), z12.VnB(), z3.VnB()),
                "mov z31.d, z12.d\n"
                "movprfx z12, z3\n"
                "saba z12.b, z31.b, z3.b");
  COMPARE_MACRO(Saba(z12.VnB(), z3.VnB(), z3.VnB(), z12.VnB()),
                "mov z31.d, z12.d\n"
                "movprfx z12, z3\n"
                "saba z12.b, z3.b, z31.b");

  COMPARE_MACRO(Uaba(z12.VnB(), z12.VnB(), z3.VnB(), z30.VnB()),
                "uaba z12.b, z3.b, z30.b");
  COMPARE_MACRO(Uaba(z12.VnB(), z12.VnB(), z3.VnB(), z12.VnB()),
                "uaba z12.b, z3.b, z12.b");
  COMPARE_MACRO(Uaba(z12.VnB(), z12.VnB(), z12.VnB(), z30.VnB()),
                "uaba z12.b, z12.b, z30.b");
  COMPARE_MACRO(Uaba(z12.VnB(), z12.VnB(), z12.VnB(), z12.VnB()), "");
  COMPARE_MACRO(Uaba(z12.VnB(), z13.VnB(), z3.VnB(), z30.VnB()),
                "movprfx z12, z13\n"
                "uaba z12.b, z3.b, z30.b");
  COMPARE_MACRO(Uaba(z12.VnB(), z3.VnB(), z3.VnB(), z30.VnB()),
                "movprfx z12, z3\n"
                "uaba z12.b, z3.b, z30.b");
  COMPARE_MACRO(Uaba(z12.VnB(), z30.VnB(), z3.VnB(), z30.VnB()),
                "movprfx z12, z30\n"
                "uaba z12.b, z3.b, z30.b");
  COMPARE_MACRO(Uaba(z12.VnB(), z3.VnB(), z3.VnB(), z3.VnB()),
                "mov z12.d, z3.d");
  COMPARE_MACRO(Uaba(z12.VnB(), z3.VnB(), z12.VnB(), z3.VnB()),
                "mov z31.d, z12.d\n"
                "movprfx z12, z3\n"
                "uaba z12.b, z31.b, z3.b");
  COMPARE_MACRO(Uaba(z12.VnB(), z3.VnB(), z3.VnB(), z12.VnB()),
                "mov z31.d, z12.d\n"
                "movprfx z12, z3\n"
                "uaba z12.b, z3.b, z31.b");

  CLEANUP();
}

TEST(sve2_halving_arithmetic) {
  SETUP();

  COMPARE(shadd(z20.VnB(), p3.Merging(), z20.VnB(), z7.VnB()),
          "shadd z20.b, p3/m, z20.b, z7.b");
  COMPARE(shadd(z20.VnD(), p3.Merging(), z20.VnD(), z7.VnD()),
          "shadd z20.d, p3/m, z20.d, z7.d");
  COMPARE(shadd(z20.VnH(), p3.Merging(), z20.VnH(), z7.VnH()),
          "shadd z20.h, p3/m, z20.h, z7.h");
  COMPARE(shadd(z20.VnS(), p3.Merging(), z20.VnS(), z7.VnS()),
          "shadd z20.s, p3/m, z20.s, z7.s");
  COMPARE(shsub(z21.VnB(), p0.Merging(), z21.VnB(), z0.VnB()),
          "shsub z21.b, p0/m, z21.b, z0.b");
  COMPARE(shsub(z21.VnD(), p0.Merging(), z21.VnD(), z0.VnD()),
          "shsub z21.d, p0/m, z21.d, z0.d");
  COMPARE(shsub(z21.VnH(), p0.Merging(), z21.VnH(), z0.VnH()),
          "shsub z21.h, p0/m, z21.h, z0.h");
  COMPARE(shsub(z21.VnS(), p0.Merging(), z21.VnS(), z0.VnS()),
          "shsub z21.s, p0/m, z21.s, z0.s");
  COMPARE(shsubr(z1.VnB(), p0.Merging(), z1.VnB(), z2.VnB()),
          "shsubr z1.b, p0/m, z1.b, z2.b");
  COMPARE(shsubr(z1.VnD(), p0.Merging(), z1.VnD(), z2.VnD()),
          "shsubr z1.d, p0/m, z1.d, z2.d");
  COMPARE(shsubr(z1.VnH(), p0.Merging(), z1.VnH(), z2.VnH()),
          "shsubr z1.h, p0/m, z1.h, z2.h");
  COMPARE(shsubr(z1.VnS(), p0.Merging(), z1.VnS(), z2.VnS()),
          "shsubr z1.s, p0/m, z1.s, z2.s");
  COMPARE(srhadd(z23.VnB(), p4.Merging(), z23.VnB(), z14.VnB()),
          "srhadd z23.b, p4/m, z23.b, z14.b");
  COMPARE(srhadd(z23.VnD(), p4.Merging(), z23.VnD(), z14.VnD()),
          "srhadd z23.d, p4/m, z23.d, z14.d");
  COMPARE(srhadd(z23.VnH(), p4.Merging(), z23.VnH(), z14.VnH()),
          "srhadd z23.h, p4/m, z23.h, z14.h");
  COMPARE(srhadd(z23.VnS(), p4.Merging(), z23.VnS(), z14.VnS()),
          "srhadd z23.s, p4/m, z23.s, z14.s");

  COMPARE(uhadd(z21.VnB(), p2.Merging(), z21.VnB(), z19.VnB()),
          "uhadd z21.b, p2/m, z21.b, z19.b");
  COMPARE(uhadd(z21.VnD(), p2.Merging(), z21.VnD(), z19.VnD()),
          "uhadd z21.d, p2/m, z21.d, z19.d");
  COMPARE(uhadd(z21.VnH(), p2.Merging(), z21.VnH(), z19.VnH()),
          "uhadd z21.h, p2/m, z21.h, z19.h");
  COMPARE(uhadd(z21.VnS(), p2.Merging(), z21.VnS(), z19.VnS()),
          "uhadd z21.s, p2/m, z21.s, z19.s");
  COMPARE(uhsub(z1.VnB(), p4.Merging(), z1.VnB(), z9.VnB()),
          "uhsub z1.b, p4/m, z1.b, z9.b");
  COMPARE(uhsub(z1.VnD(), p4.Merging(), z1.VnD(), z9.VnD()),
          "uhsub z1.d, p4/m, z1.d, z9.d");
  COMPARE(uhsub(z1.VnH(), p4.Merging(), z1.VnH(), z9.VnH()),
          "uhsub z1.h, p4/m, z1.h, z9.h");
  COMPARE(uhsub(z1.VnS(), p4.Merging(), z1.VnS(), z9.VnS()),
          "uhsub z1.s, p4/m, z1.s, z9.s");
  COMPARE(uhsubr(z18.VnB(), p0.Merging(), z18.VnB(), z1.VnB()),
          "uhsubr z18.b, p0/m, z18.b, z1.b");
  COMPARE(uhsubr(z18.VnD(), p0.Merging(), z18.VnD(), z1.VnD()),
          "uhsubr z18.d, p0/m, z18.d, z1.d");
  COMPARE(uhsubr(z18.VnH(), p0.Merging(), z18.VnH(), z1.VnH()),
          "uhsubr z18.h, p0/m, z18.h, z1.h");
  COMPARE(uhsubr(z18.VnS(), p0.Merging(), z18.VnS(), z1.VnS()),
          "uhsubr z18.s, p0/m, z18.s, z1.s");
  COMPARE(urhadd(z29.VnB(), p4.Merging(), z29.VnB(), z10.VnB()),
          "urhadd z29.b, p4/m, z29.b, z10.b");
  COMPARE(urhadd(z29.VnD(), p4.Merging(), z29.VnD(), z10.VnD()),
          "urhadd z29.d, p4/m, z29.d, z10.d");
  COMPARE(urhadd(z29.VnH(), p4.Merging(), z29.VnH(), z10.VnH()),
          "urhadd z29.h, p4/m, z29.h, z10.h");
  COMPARE(urhadd(z29.VnS(), p4.Merging(), z29.VnS(), z10.VnS()),
          "urhadd z29.s, p4/m, z29.s, z10.s");

  COMPARE_MACRO(Shadd(z0.VnB(), p0.Merging(), z1.VnB(), z2.VnB()),
                "movprfx z0.b, p0/m, z1.b\n"
                "shadd z0.b, p0/m, z0.b, z2.b");
  COMPARE_MACRO(Shadd(z0.VnB(), p0.Merging(), z1.VnB(), z0.VnB()),
                "shadd z0.b, p0/m, z0.b, z1.b");
  COMPARE_MACRO(Srhadd(z0.VnB(), p0.Merging(), z1.VnB(), z2.VnB()),
                "movprfx z0.b, p0/m, z1.b\n"
                "srhadd z0.b, p0/m, z0.b, z2.b");
  COMPARE_MACRO(Srhadd(z0.VnB(), p0.Merging(), z1.VnB(), z0.VnB()),
                "srhadd z0.b, p0/m, z0.b, z1.b");
  COMPARE_MACRO(Uhadd(z0.VnB(), p0.Merging(), z1.VnB(), z2.VnB()),
                "movprfx z0.b, p0/m, z1.b\n"
                "uhadd z0.b, p0/m, z0.b, z2.b");
  COMPARE_MACRO(Uhadd(z0.VnB(), p0.Merging(), z1.VnB(), z0.VnB()),
                "uhadd z0.b, p0/m, z0.b, z1.b");
  COMPARE_MACRO(Urhadd(z0.VnB(), p0.Merging(), z1.VnB(), z2.VnB()),
                "movprfx z0.b, p0/m, z1.b\n"
                "urhadd z0.b, p0/m, z0.b, z2.b");
  COMPARE_MACRO(Urhadd(z0.VnB(), p0.Merging(), z1.VnB(), z0.VnB()),
                "urhadd z0.b, p0/m, z0.b, z1.b");

  COMPARE_MACRO(Shsub(z0.VnB(), p0.Merging(), z0.VnB(), z1.VnB()),
                "shsub z0.b, p0/m, z0.b, z1.b");
  COMPARE_MACRO(Shsub(z0.VnB(), p0.Merging(), z1.VnB(), z0.VnB()),
                "shsubr z0.b, p0/m, z0.b, z1.b");
  COMPARE_MACRO(Uhsub(z0.VnB(), p0.Merging(), z0.VnB(), z1.VnB()),
                "uhsub z0.b, p0/m, z0.b, z1.b");
  COMPARE_MACRO(Uhsub(z0.VnB(), p0.Merging(), z1.VnB(), z0.VnB()),
                "uhsubr z0.b, p0/m, z0.b, z1.b");

  CLEANUP();
}

TEST(sve2_sra) {
  SETUP();

  COMPARE(ssra(z0.VnB(), z8.VnB(), 1), "ssra z0.b, z8.b, #1");
  COMPARE(ssra(z0.VnB(), z8.VnB(), 2), "ssra z0.b, z8.b, #2");
  COMPARE(ssra(z0.VnB(), z8.VnB(), 5), "ssra z0.b, z8.b, #5");
  COMPARE(ssra(z0.VnB(), z8.VnB(), 8), "ssra z0.b, z8.b, #8");
  COMPARE(ssra(z0.VnH(), z8.VnH(), 1), "ssra z0.h, z8.h, #1");
  COMPARE(ssra(z0.VnH(), z8.VnH(), 16), "ssra z0.h, z8.h, #16");
  COMPARE(ssra(z0.VnS(), z8.VnS(), 1), "ssra z0.s, z8.s, #1");
  COMPARE(ssra(z0.VnS(), z8.VnS(), 31), "ssra z0.s, z8.s, #31");
  COMPARE(ssra(z0.VnD(), z8.VnD(), 1), "ssra z0.d, z8.d, #1");
  COMPARE(ssra(z0.VnD(), z8.VnD(), 64), "ssra z0.d, z8.d, #64");

  COMPARE(srsra(z0.VnB(), z8.VnB(), 1), "srsra z0.b, z8.b, #1");
  COMPARE(srsra(z0.VnB(), z8.VnB(), 2), "srsra z0.b, z8.b, #2");
  COMPARE(srsra(z0.VnB(), z8.VnB(), 5), "srsra z0.b, z8.b, #5");
  COMPARE(srsra(z0.VnB(), z8.VnB(), 8), "srsra z0.b, z8.b, #8");
  COMPARE(srsra(z0.VnH(), z8.VnH(), 1), "srsra z0.h, z8.h, #1");
  COMPARE(srsra(z0.VnH(), z8.VnH(), 16), "srsra z0.h, z8.h, #16");
  COMPARE(srsra(z0.VnS(), z8.VnS(), 1), "srsra z0.s, z8.s, #1");
  COMPARE(srsra(z0.VnS(), z8.VnS(), 31), "srsra z0.s, z8.s, #31");
  COMPARE(srsra(z0.VnD(), z8.VnD(), 1), "srsra z0.d, z8.d, #1");
  COMPARE(srsra(z0.VnD(), z8.VnD(), 64), "srsra z0.d, z8.d, #64");

  COMPARE(usra(z0.VnB(), z8.VnB(), 1), "usra z0.b, z8.b, #1");
  COMPARE(usra(z0.VnB(), z8.VnB(), 2), "usra z0.b, z8.b, #2");
  COMPARE(usra(z0.VnB(), z8.VnB(), 5), "usra z0.b, z8.b, #5");
  COMPARE(usra(z0.VnB(), z8.VnB(), 8), "usra z0.b, z8.b, #8");
  COMPARE(usra(z0.VnH(), z8.VnH(), 1), "usra z0.h, z8.h, #1");
  COMPARE(usra(z0.VnH(), z8.VnH(), 16), "usra z0.h, z8.h, #16");
  COMPARE(usra(z0.VnS(), z8.VnS(), 1), "usra z0.s, z8.s, #1");
  COMPARE(usra(z0.VnS(), z8.VnS(), 31), "usra z0.s, z8.s, #31");
  COMPARE(usra(z0.VnD(), z8.VnD(), 1), "usra z0.d, z8.d, #1");
  COMPARE(usra(z0.VnD(), z8.VnD(), 64), "usra z0.d, z8.d, #64");

  COMPARE(ursra(z0.VnB(), z8.VnB(), 1), "ursra z0.b, z8.b, #1");
  COMPARE(ursra(z0.VnB(), z8.VnB(), 2), "ursra z0.b, z8.b, #2");
  COMPARE(ursra(z0.VnB(), z8.VnB(), 5), "ursra z0.b, z8.b, #5");
  COMPARE(ursra(z0.VnB(), z8.VnB(), 8), "ursra z0.b, z8.b, #8");
  COMPARE(ursra(z0.VnH(), z8.VnH(), 1), "ursra z0.h, z8.h, #1");
  COMPARE(ursra(z0.VnH(), z8.VnH(), 16), "ursra z0.h, z8.h, #16");
  COMPARE(ursra(z0.VnS(), z8.VnS(), 1), "ursra z0.s, z8.s, #1");
  COMPARE(ursra(z0.VnS(), z8.VnS(), 31), "ursra z0.s, z8.s, #31");
  COMPARE(ursra(z0.VnD(), z8.VnD(), 1), "ursra z0.d, z8.d, #1");
  COMPARE(ursra(z0.VnD(), z8.VnD(), 64), "ursra z0.d, z8.d, #64");

  COMPARE_MACRO(Ssra(z0.VnB(), z1.VnB(), z2.VnB(), 2),
                "movprfx z0, z1\n"
                "ssra z0.b, z2.b, #2");
  COMPARE_MACRO(Ssra(z0.VnB(), z0.VnB(), z2.VnB(), 2), "ssra z0.b, z2.b, #2");
  COMPARE_MACRO(Ssra(z0.VnB(), z1.VnB(), z1.VnB(), 2),
                "movprfx z0, z1\n"
                "ssra z0.b, z1.b, #2");
  COMPARE_MACRO(Ssra(z2.VnB(), z1.VnB(), z2.VnB(), 2),
                "mov z31.d, z2.d\n"
                "movprfx z2, z1\n"
                "ssra z2.b, z31.b, #2");
  COMPARE_MACRO(Ssra(z0.VnB(), z0.VnB(), z0.VnB(), 2), "ssra z0.b, z0.b, #2");

  COMPARE_MACRO(Srsra(z2.VnB(), z1.VnB(), z2.VnB(), 2),
                "mov z31.d, z2.d\n"
                "movprfx z2, z1\n"
                "srsra z2.b, z31.b, #2");
  COMPARE_MACRO(Usra(z2.VnB(), z1.VnB(), z2.VnB(), 2),
                "mov z31.d, z2.d\n"
                "movprfx z2, z1\n"
                "usra z2.b, z31.b, #2");
  COMPARE_MACRO(Ursra(z2.VnB(), z1.VnB(), z2.VnB(), 2),
                "mov z31.d, z2.d\n"
                "movprfx z2, z1\n"
                "ursra z2.b, z31.b, #2");
  CLEANUP();
}

TEST(sve2_sri_sli) {
  SETUP();

  COMPARE(sri(z6.VnB(), z9.VnB(), 1), "sri z6.b, z9.b, #1");
  COMPARE(sri(z6.VnB(), z9.VnB(), 2), "sri z6.b, z9.b, #2");
  COMPARE(sri(z6.VnB(), z9.VnB(), 5), "sri z6.b, z9.b, #5");
  COMPARE(sri(z6.VnB(), z9.VnB(), 8), "sri z6.b, z9.b, #8");
  COMPARE(sri(z6.VnH(), z9.VnH(), 1), "sri z6.h, z9.h, #1");
  COMPARE(sri(z6.VnH(), z9.VnH(), 16), "sri z6.h, z9.h, #16");
  COMPARE(sri(z6.VnS(), z9.VnS(), 1), "sri z6.s, z9.s, #1");
  COMPARE(sri(z6.VnS(), z9.VnS(), 31), "sri z6.s, z9.s, #31");
  COMPARE(sri(z6.VnD(), z9.VnD(), 1), "sri z6.d, z9.d, #1");
  COMPARE(sri(z6.VnD(), z9.VnD(), 64), "sri z6.d, z9.d, #64");

  COMPARE(sli(z29.VnB(), z7.VnB(), 0), "sli z29.b, z7.b, #0");
  COMPARE(sli(z29.VnB(), z7.VnB(), 2), "sli z29.b, z7.b, #2");
  COMPARE(sli(z29.VnB(), z7.VnB(), 5), "sli z29.b, z7.b, #5");
  COMPARE(sli(z29.VnB(), z7.VnB(), 7), "sli z29.b, z7.b, #7");
  COMPARE(sli(z29.VnH(), z7.VnH(), 0), "sli z29.h, z7.h, #0");
  COMPARE(sli(z29.VnH(), z7.VnH(), 15), "sli z29.h, z7.h, #15");
  COMPARE(sli(z29.VnS(), z7.VnS(), 0), "sli z29.s, z7.s, #0");
  COMPARE(sli(z29.VnS(), z7.VnS(), 31), "sli z29.s, z7.s, #31");
  COMPARE(sli(z29.VnD(), z7.VnD(), 0), "sli z29.d, z7.d, #0");
  COMPARE(sli(z29.VnD(), z7.VnD(), 63), "sli z29.d, z7.d, #63");

  CLEANUP();
}

TEST(sve2_shift_imm) {
  SETUP();

  COMPARE(sqshl(z0.VnB(), p5.Merging(), z0.VnB(), 0),
          "sqshl z0.b, p5/m, z0.b, #0");
  COMPARE(sqshl(z0.VnB(), p5.Merging(), z0.VnB(), 2),
          "sqshl z0.b, p5/m, z0.b, #2");
  COMPARE(sqshl(z0.VnB(), p5.Merging(), z0.VnB(), 5),
          "sqshl z0.b, p5/m, z0.b, #5");
  COMPARE(sqshl(z0.VnB(), p5.Merging(), z0.VnB(), 7),
          "sqshl z0.b, p5/m, z0.b, #7");
  COMPARE(sqshl(z0.VnH(), p5.Merging(), z0.VnH(), 0),
          "sqshl z0.h, p5/m, z0.h, #0");
  COMPARE(sqshl(z0.VnH(), p5.Merging(), z0.VnH(), 15),
          "sqshl z0.h, p5/m, z0.h, #15");
  COMPARE(sqshl(z0.VnS(), p5.Merging(), z0.VnS(), 0),
          "sqshl z0.s, p5/m, z0.s, #0");
  COMPARE(sqshl(z0.VnS(), p5.Merging(), z0.VnS(), 31),
          "sqshl z0.s, p5/m, z0.s, #31");
  COMPARE(sqshl(z0.VnD(), p5.Merging(), z0.VnD(), 0),
          "sqshl z0.d, p5/m, z0.d, #0");
  COMPARE(sqshl(z0.VnD(), p5.Merging(), z0.VnD(), 63),
          "sqshl z0.d, p5/m, z0.d, #63");

  COMPARE(sqshlu(z10.VnB(), p1.Merging(), z10.VnB(), 0),
          "sqshlu z10.b, p1/m, z10.b, #0");
  COMPARE(sqshlu(z10.VnB(), p1.Merging(), z10.VnB(), 2),
          "sqshlu z10.b, p1/m, z10.b, #2");
  COMPARE(sqshlu(z10.VnB(), p1.Merging(), z10.VnB(), 5),
          "sqshlu z10.b, p1/m, z10.b, #5");
  COMPARE(sqshlu(z10.VnB(), p1.Merging(), z10.VnB(), 7),
          "sqshlu z10.b, p1/m, z10.b, #7");
  COMPARE(sqshlu(z10.VnH(), p1.Merging(), z10.VnH(), 0),
          "sqshlu z10.h, p1/m, z10.h, #0");
  COMPARE(sqshlu(z10.VnH(), p1.Merging(), z10.VnH(), 15),
          "sqshlu z10.h, p1/m, z10.h, #15");
  COMPARE(sqshlu(z10.VnS(), p1.Merging(), z10.VnS(), 0),
          "sqshlu z10.s, p1/m, z10.s, #0");
  COMPARE(sqshlu(z10.VnS(), p1.Merging(), z10.VnS(), 31),
          "sqshlu z10.s, p1/m, z10.s, #31");
  COMPARE(sqshlu(z10.VnD(), p1.Merging(), z10.VnD(), 0),
          "sqshlu z10.d, p1/m, z10.d, #0");
  COMPARE(sqshlu(z10.VnD(), p1.Merging(), z10.VnD(), 63),
          "sqshlu z10.d, p1/m, z10.d, #63");

  COMPARE(srshr(z12.VnB(), p0.Merging(), z12.VnB(), 1),
          "srshr z12.b, p0/m, z12.b, #1");
  COMPARE(srshr(z12.VnB(), p0.Merging(), z12.VnB(), 2),
          "srshr z12.b, p0/m, z12.b, #2");
  COMPARE(srshr(z12.VnB(), p0.Merging(), z12.VnB(), 5),
          "srshr z12.b, p0/m, z12.b, #5");
  COMPARE(srshr(z12.VnB(), p0.Merging(), z12.VnB(), 8),
          "srshr z12.b, p0/m, z12.b, #8");
  COMPARE(srshr(z12.VnH(), p0.Merging(), z12.VnH(), 1),
          "srshr z12.h, p0/m, z12.h, #1");
  COMPARE(srshr(z12.VnH(), p0.Merging(), z12.VnH(), 16),
          "srshr z12.h, p0/m, z12.h, #16");
  COMPARE(srshr(z12.VnS(), p0.Merging(), z12.VnS(), 1),
          "srshr z12.s, p0/m, z12.s, #1");
  COMPARE(srshr(z12.VnS(), p0.Merging(), z12.VnS(), 32),
          "srshr z12.s, p0/m, z12.s, #32");
  COMPARE(srshr(z12.VnD(), p0.Merging(), z12.VnD(), 1),
          "srshr z12.d, p0/m, z12.d, #1");
  COMPARE(srshr(z12.VnD(), p0.Merging(), z12.VnD(), 64),
          "srshr z12.d, p0/m, z12.d, #64");

  COMPARE(uqshl(z29.VnB(), p7.Merging(), z29.VnB(), 0),
          "uqshl z29.b, p7/m, z29.b, #0");
  COMPARE(uqshl(z29.VnB(), p7.Merging(), z29.VnB(), 2),
          "uqshl z29.b, p7/m, z29.b, #2");
  COMPARE(uqshl(z29.VnB(), p7.Merging(), z29.VnB(), 5),
          "uqshl z29.b, p7/m, z29.b, #5");
  COMPARE(uqshl(z29.VnB(), p7.Merging(), z29.VnB(), 7),
          "uqshl z29.b, p7/m, z29.b, #7");
  COMPARE(uqshl(z29.VnH(), p7.Merging(), z29.VnH(), 0),
          "uqshl z29.h, p7/m, z29.h, #0");
  COMPARE(uqshl(z29.VnH(), p7.Merging(), z29.VnH(), 15),
          "uqshl z29.h, p7/m, z29.h, #15");
  COMPARE(uqshl(z29.VnS(), p7.Merging(), z29.VnS(), 0),
          "uqshl z29.s, p7/m, z29.s, #0");
  COMPARE(uqshl(z29.VnS(), p7.Merging(), z29.VnS(), 31),
          "uqshl z29.s, p7/m, z29.s, #31");
  COMPARE(uqshl(z29.VnD(), p7.Merging(), z29.VnD(), 0),
          "uqshl z29.d, p7/m, z29.d, #0");
  COMPARE(uqshl(z29.VnD(), p7.Merging(), z29.VnD(), 63),
          "uqshl z29.d, p7/m, z29.d, #63");

  COMPARE(urshr(z31.VnB(), p2.Merging(), z31.VnB(), 1),
          "urshr z31.b, p2/m, z31.b, #1");
  COMPARE(urshr(z31.VnB(), p2.Merging(), z31.VnB(), 2),
          "urshr z31.b, p2/m, z31.b, #2");
  COMPARE(urshr(z31.VnB(), p2.Merging(), z31.VnB(), 5),
          "urshr z31.b, p2/m, z31.b, #5");
  COMPARE(urshr(z31.VnB(), p2.Merging(), z31.VnB(), 8),
          "urshr z31.b, p2/m, z31.b, #8");
  COMPARE(urshr(z31.VnH(), p2.Merging(), z31.VnH(), 1),
          "urshr z31.h, p2/m, z31.h, #1");
  COMPARE(urshr(z31.VnH(), p2.Merging(), z31.VnH(), 16),
          "urshr z31.h, p2/m, z31.h, #16");
  COMPARE(urshr(z31.VnS(), p2.Merging(), z31.VnS(), 1),
          "urshr z31.s, p2/m, z31.s, #1");
  COMPARE(urshr(z31.VnS(), p2.Merging(), z31.VnS(), 32),
          "urshr z31.s, p2/m, z31.s, #32");
  COMPARE(urshr(z31.VnD(), p2.Merging(), z31.VnD(), 1),
          "urshr z31.d, p2/m, z31.d, #1");
  COMPARE(urshr(z31.VnD(), p2.Merging(), z31.VnD(), 64),
          "urshr z31.d, p2/m, z31.d, #64");

  CLEANUP();
}

TEST(sve2_shift_sat) {
  SETUP();

  COMPARE(srshl(z31.VnB(), p7.Merging(), z31.VnB(), z3.VnB()),
          "srshl z31.b, p7/m, z31.b, z3.b");
  COMPARE(srshl(z31.VnD(), p7.Merging(), z31.VnD(), z3.VnD()),
          "srshl z31.d, p7/m, z31.d, z3.d");
  COMPARE(srshl(z31.VnH(), p7.Merging(), z31.VnH(), z3.VnH()),
          "srshl z31.h, p7/m, z31.h, z3.h");
  COMPARE(srshl(z31.VnS(), p7.Merging(), z31.VnS(), z3.VnS()),
          "srshl z31.s, p7/m, z31.s, z3.s");

  COMPARE(srshlr(z16.VnB(), p7.Merging(), z16.VnB(), z29.VnB()),
          "srshlr z16.b, p7/m, z16.b, z29.b");
  COMPARE(srshlr(z16.VnD(), p7.Merging(), z16.VnD(), z29.VnD()),
          "srshlr z16.d, p7/m, z16.d, z29.d");
  COMPARE(srshlr(z16.VnH(), p7.Merging(), z16.VnH(), z29.VnH()),
          "srshlr z16.h, p7/m, z16.h, z29.h");
  COMPARE(srshlr(z16.VnS(), p7.Merging(), z16.VnS(), z29.VnS()),
          "srshlr z16.s, p7/m, z16.s, z29.s");

  COMPARE(urshl(z15.VnB(), p2.Merging(), z15.VnB(), z3.VnB()),
          "urshl z15.b, p2/m, z15.b, z3.b");
  COMPARE(urshl(z15.VnD(), p2.Merging(), z15.VnD(), z3.VnD()),
          "urshl z15.d, p2/m, z15.d, z3.d");
  COMPARE(urshl(z15.VnH(), p2.Merging(), z15.VnH(), z3.VnH()),
          "urshl z15.h, p2/m, z15.h, z3.h");
  COMPARE(urshl(z15.VnS(), p2.Merging(), z15.VnS(), z3.VnS()),
          "urshl z15.s, p2/m, z15.s, z3.s");

  COMPARE(urshlr(z27.VnB(), p1.Merging(), z27.VnB(), z30.VnB()),
          "urshlr z27.b, p1/m, z27.b, z30.b");
  COMPARE(urshlr(z27.VnD(), p1.Merging(), z27.VnD(), z30.VnD()),
          "urshlr z27.d, p1/m, z27.d, z30.d");
  COMPARE(urshlr(z27.VnH(), p1.Merging(), z27.VnH(), z30.VnH()),
          "urshlr z27.h, p1/m, z27.h, z30.h");
  COMPARE(urshlr(z27.VnS(), p1.Merging(), z27.VnS(), z30.VnS()),
          "urshlr z27.s, p1/m, z27.s, z30.s");

  COMPARE(sqshl(z22.VnB(), p4.Merging(), z22.VnB(), z21.VnB()),
          "sqshl z22.b, p4/m, z22.b, z21.b");
  COMPARE(sqshl(z22.VnD(), p4.Merging(), z22.VnD(), z21.VnD()),
          "sqshl z22.d, p4/m, z22.d, z21.d");
  COMPARE(sqshl(z22.VnH(), p4.Merging(), z22.VnH(), z21.VnH()),
          "sqshl z22.h, p4/m, z22.h, z21.h");
  COMPARE(sqshl(z22.VnS(), p4.Merging(), z22.VnS(), z21.VnS()),
          "sqshl z22.s, p4/m, z22.s, z21.s");

  COMPARE(sqshlr(z7.VnB(), p3.Merging(), z7.VnB(), z5.VnB()),
          "sqshlr z7.b, p3/m, z7.b, z5.b");
  COMPARE(sqshlr(z7.VnD(), p3.Merging(), z7.VnD(), z5.VnD()),
          "sqshlr z7.d, p3/m, z7.d, z5.d");
  COMPARE(sqshlr(z7.VnH(), p3.Merging(), z7.VnH(), z5.VnH()),
          "sqshlr z7.h, p3/m, z7.h, z5.h");
  COMPARE(sqshlr(z7.VnS(), p3.Merging(), z7.VnS(), z5.VnS()),
          "sqshlr z7.s, p3/m, z7.s, z5.s");

  COMPARE(uqshl(z10.VnB(), p0.Merging(), z10.VnB(), z21.VnB()),
          "uqshl z10.b, p0/m, z10.b, z21.b");
  COMPARE(uqshl(z10.VnD(), p0.Merging(), z10.VnD(), z21.VnD()),
          "uqshl z10.d, p0/m, z10.d, z21.d");
  COMPARE(uqshl(z10.VnH(), p0.Merging(), z10.VnH(), z21.VnH()),
          "uqshl z10.h, p0/m, z10.h, z21.h");
  COMPARE(uqshl(z10.VnS(), p0.Merging(), z10.VnS(), z21.VnS()),
          "uqshl z10.s, p0/m, z10.s, z21.s");

  COMPARE(uqshlr(z12.VnB(), p1.Merging(), z12.VnB(), z12.VnB()),
          "uqshlr z12.b, p1/m, z12.b, z12.b");
  COMPARE(uqshlr(z12.VnD(), p1.Merging(), z12.VnD(), z12.VnD()),
          "uqshlr z12.d, p1/m, z12.d, z12.d");
  COMPARE(uqshlr(z12.VnH(), p1.Merging(), z12.VnH(), z12.VnH()),
          "uqshlr z12.h, p1/m, z12.h, z12.h");
  COMPARE(uqshlr(z12.VnS(), p1.Merging(), z12.VnS(), z12.VnS()),
          "uqshlr z12.s, p1/m, z12.s, z12.s");

  COMPARE(sqrshl(z31.VnB(), p5.Merging(), z31.VnB(), z27.VnB()),
          "sqrshl z31.b, p5/m, z31.b, z27.b");
  COMPARE(sqrshl(z31.VnD(), p5.Merging(), z31.VnD(), z27.VnD()),
          "sqrshl z31.d, p5/m, z31.d, z27.d");
  COMPARE(sqrshl(z31.VnH(), p5.Merging(), z31.VnH(), z27.VnH()),
          "sqrshl z31.h, p5/m, z31.h, z27.h");
  COMPARE(sqrshl(z31.VnS(), p5.Merging(), z31.VnS(), z27.VnS()),
          "sqrshl z31.s, p5/m, z31.s, z27.s");

  COMPARE(sqrshlr(z25.VnB(), p6.Merging(), z25.VnB(), z7.VnB()),
          "sqrshlr z25.b, p6/m, z25.b, z7.b");
  COMPARE(sqrshlr(z25.VnD(), p6.Merging(), z25.VnD(), z7.VnD()),
          "sqrshlr z25.d, p6/m, z25.d, z7.d");
  COMPARE(sqrshlr(z25.VnH(), p6.Merging(), z25.VnH(), z7.VnH()),
          "sqrshlr z25.h, p6/m, z25.h, z7.h");
  COMPARE(sqrshlr(z25.VnS(), p6.Merging(), z25.VnS(), z7.VnS()),
          "sqrshlr z25.s, p6/m, z25.s, z7.s");

  COMPARE(uqrshl(z20.VnB(), p1.Merging(), z20.VnB(), z30.VnB()),
          "uqrshl z20.b, p1/m, z20.b, z30.b");
  COMPARE(uqrshl(z20.VnD(), p1.Merging(), z20.VnD(), z30.VnD()),
          "uqrshl z20.d, p1/m, z20.d, z30.d");
  COMPARE(uqrshl(z20.VnH(), p1.Merging(), z20.VnH(), z30.VnH()),
          "uqrshl z20.h, p1/m, z20.h, z30.h");
  COMPARE(uqrshl(z20.VnS(), p1.Merging(), z20.VnS(), z30.VnS()),
          "uqrshl z20.s, p1/m, z20.s, z30.s");

  COMPARE(uqrshlr(z8.VnB(), p5.Merging(), z8.VnB(), z9.VnB()),
          "uqrshlr z8.b, p5/m, z8.b, z9.b");
  COMPARE(uqrshlr(z8.VnD(), p5.Merging(), z8.VnD(), z9.VnD()),
          "uqrshlr z8.d, p5/m, z8.d, z9.d");
  COMPARE(uqrshlr(z8.VnH(), p5.Merging(), z8.VnH(), z9.VnH()),
          "uqrshlr z8.h, p5/m, z8.h, z9.h");
  COMPARE(uqrshlr(z8.VnS(), p5.Merging(), z8.VnS(), z9.VnS()),
          "uqrshlr z8.s, p5/m, z8.s, z9.s");

  CLEANUP();
}

TEST(sve2_sat_arith) {
  SETUP();

  COMPARE(sqadd(z28.VnB(), p0.Merging(), z28.VnB(), z3.VnB()),
          "sqadd z28.b, p0/m, z28.b, z3.b");
  COMPARE(sqadd(z28.VnD(), p0.Merging(), z28.VnD(), z3.VnD()),
          "sqadd z28.d, p0/m, z28.d, z3.d");
  COMPARE(sqadd(z28.VnH(), p0.Merging(), z28.VnH(), z3.VnH()),
          "sqadd z28.h, p0/m, z28.h, z3.h");
  COMPARE(sqadd(z28.VnS(), p0.Merging(), z28.VnS(), z3.VnS()),
          "sqadd z28.s, p0/m, z28.s, z3.s");
  COMPARE(sqsub(z6.VnB(), p0.Merging(), z6.VnB(), z12.VnB()),
          "sqsub z6.b, p0/m, z6.b, z12.b");
  COMPARE(sqsub(z6.VnD(), p0.Merging(), z6.VnD(), z12.VnD()),
          "sqsub z6.d, p0/m, z6.d, z12.d");
  COMPARE(sqsub(z6.VnH(), p0.Merging(), z6.VnH(), z12.VnH()),
          "sqsub z6.h, p0/m, z6.h, z12.h");
  COMPARE(sqsub(z6.VnS(), p0.Merging(), z6.VnS(), z12.VnS()),
          "sqsub z6.s, p0/m, z6.s, z12.s");
  COMPARE(sqsubr(z16.VnB(), p7.Merging(), z16.VnB(), z22.VnB()),
          "sqsubr z16.b, p7/m, z16.b, z22.b");
  COMPARE(sqsubr(z16.VnD(), p7.Merging(), z16.VnD(), z22.VnD()),
          "sqsubr z16.d, p7/m, z16.d, z22.d");
  COMPARE(sqsubr(z16.VnH(), p7.Merging(), z16.VnH(), z22.VnH()),
          "sqsubr z16.h, p7/m, z16.h, z22.h");
  COMPARE(sqsubr(z16.VnS(), p7.Merging(), z16.VnS(), z22.VnS()),
          "sqsubr z16.s, p7/m, z16.s, z22.s");
  COMPARE(suqadd(z26.VnB(), p2.Merging(), z26.VnB(), z28.VnB()),
          "suqadd z26.b, p2/m, z26.b, z28.b");
  COMPARE(suqadd(z26.VnD(), p2.Merging(), z26.VnD(), z28.VnD()),
          "suqadd z26.d, p2/m, z26.d, z28.d");
  COMPARE(suqadd(z26.VnH(), p2.Merging(), z26.VnH(), z28.VnH()),
          "suqadd z26.h, p2/m, z26.h, z28.h");
  COMPARE(suqadd(z26.VnS(), p2.Merging(), z26.VnS(), z28.VnS()),
          "suqadd z26.s, p2/m, z26.s, z28.s");
  COMPARE(usqadd(z25.VnB(), p4.Merging(), z25.VnB(), z6.VnB()),
          "usqadd z25.b, p4/m, z25.b, z6.b");
  COMPARE(usqadd(z25.VnD(), p4.Merging(), z25.VnD(), z6.VnD()),
          "usqadd z25.d, p4/m, z25.d, z6.d");
  COMPARE(usqadd(z25.VnH(), p4.Merging(), z25.VnH(), z6.VnH()),
          "usqadd z25.h, p4/m, z25.h, z6.h");
  COMPARE(usqadd(z25.VnS(), p4.Merging(), z25.VnS(), z6.VnS()),
          "usqadd z25.s, p4/m, z25.s, z6.s");
  COMPARE(uqadd(z24.VnB(), p7.Merging(), z24.VnB(), z1.VnB()),
          "uqadd z24.b, p7/m, z24.b, z1.b");
  COMPARE(uqadd(z24.VnD(), p7.Merging(), z24.VnD(), z1.VnD()),
          "uqadd z24.d, p7/m, z24.d, z1.d");
  COMPARE(uqadd(z24.VnH(), p7.Merging(), z24.VnH(), z1.VnH()),
          "uqadd z24.h, p7/m, z24.h, z1.h");
  COMPARE(uqadd(z24.VnS(), p7.Merging(), z24.VnS(), z1.VnS()),
          "uqadd z24.s, p7/m, z24.s, z1.s");
  COMPARE(uqsub(z10.VnB(), p3.Merging(), z10.VnB(), z1.VnB()),
          "uqsub z10.b, p3/m, z10.b, z1.b");
  COMPARE(uqsub(z10.VnD(), p3.Merging(), z10.VnD(), z1.VnD()),
          "uqsub z10.d, p3/m, z10.d, z1.d");
  COMPARE(uqsub(z10.VnH(), p3.Merging(), z10.VnH(), z1.VnH()),
          "uqsub z10.h, p3/m, z10.h, z1.h");
  COMPARE(uqsub(z10.VnS(), p3.Merging(), z10.VnS(), z1.VnS()),
          "uqsub z10.s, p3/m, z10.s, z1.s");
  COMPARE(uqsubr(z20.VnB(), p0.Merging(), z20.VnB(), z6.VnB()),
          "uqsubr z20.b, p0/m, z20.b, z6.b");
  COMPARE(uqsubr(z20.VnD(), p0.Merging(), z20.VnD(), z6.VnD()),
          "uqsubr z20.d, p0/m, z20.d, z6.d");
  COMPARE(uqsubr(z20.VnH(), p0.Merging(), z20.VnH(), z6.VnH()),
          "uqsubr z20.h, p0/m, z20.h, z6.h");
  COMPARE(uqsubr(z20.VnS(), p0.Merging(), z20.VnS(), z6.VnS()),
          "uqsubr z20.s, p0/m, z20.s, z6.s");

  COMPARE_MACRO(Sqadd(z28.VnB(), p0.Merging(), z28.VnB(), z28.VnB()),
                "sqadd z28.b, p0/m, z28.b, z28.b");
  COMPARE_MACRO(Sqadd(z29.VnB(), p0.Merging(), z28.VnB(), z3.VnB()),
                "movprfx z29.b, p0/m, z28.b\n"
                "sqadd z29.b, p0/m, z29.b, z3.b");
  COMPARE_MACRO(Sqadd(z29.VnB(), p0.Merging(), z28.VnB(), z29.VnB()),
                "sqadd z29.b, p0/m, z29.b, z28.b");
  COMPARE_MACRO(Uqadd(z28.VnB(), p0.Merging(), z28.VnB(), z28.VnB()),
                "uqadd z28.b, p0/m, z28.b, z28.b");
  COMPARE_MACRO(Uqadd(z29.VnB(), p0.Merging(), z28.VnB(), z3.VnB()),
                "movprfx z29.b, p0/m, z28.b\n"
                "uqadd z29.b, p0/m, z29.b, z3.b");
  COMPARE_MACRO(Uqadd(z29.VnB(), p0.Merging(), z28.VnB(), z29.VnB()),
                "uqadd z29.b, p0/m, z29.b, z28.b");

  COMPARE_MACRO(Sqsub(z28.VnB(), p0.Merging(), z28.VnB(), z28.VnB()),
                "sqsub z28.b, p0/m, z28.b, z28.b");
  COMPARE_MACRO(Sqsub(z29.VnB(), p0.Merging(), z28.VnB(), z3.VnB()),
                "movprfx z29.b, p0/m, z28.b\n"
                "sqsub z29.b, p0/m, z29.b, z3.b");
  COMPARE_MACRO(Sqsub(z29.VnB(), p0.Merging(), z28.VnB(), z29.VnB()),
                "sqsubr z29.b, p0/m, z29.b, z28.b");
  COMPARE_MACRO(Uqsub(z28.VnB(), p0.Merging(), z28.VnB(), z28.VnB()),
                "uqsub z28.b, p0/m, z28.b, z28.b");
  COMPARE_MACRO(Uqsub(z29.VnB(), p0.Merging(), z28.VnB(), z3.VnB()),
                "movprfx z29.b, p0/m, z28.b\n"
                "uqsub z29.b, p0/m, z29.b, z3.b");
  COMPARE_MACRO(Uqsub(z29.VnB(), p0.Merging(), z28.VnB(), z29.VnB()),
                "uqsubr z29.b, p0/m, z29.b, z28.b");

  COMPARE_MACRO(Suqadd(z28.VnB(), p0.Merging(), z28.VnB(), z28.VnB()),
                "suqadd z28.b, p0/m, z28.b, z28.b");
  COMPARE_MACRO(Suqadd(z29.VnB(), p0.Merging(), z28.VnB(), z3.VnB()),
                "movprfx z29.b, p0/m, z28.b\n"
                "suqadd z29.b, p0/m, z29.b, z3.b");
  COMPARE_MACRO(Suqadd(z29.VnB(), p0.Merging(), z28.VnB(), z29.VnB()),
                "mov z31.d, z29.d\n"
                "movprfx z29.b, p0/m, z28.b\n"
                "suqadd z29.b, p0/m, z29.b, z31.b");
  COMPARE_MACRO(Usqadd(z28.VnB(), p0.Merging(), z28.VnB(), z28.VnB()),
                "usqadd z28.b, p0/m, z28.b, z28.b");
  COMPARE_MACRO(Usqadd(z29.VnB(), p0.Merging(), z28.VnB(), z3.VnB()),
                "movprfx z29.b, p0/m, z28.b\n"
                "usqadd z29.b, p0/m, z29.b, z3.b");
  COMPARE_MACRO(Usqadd(z29.VnB(), p0.Merging(), z28.VnB(), z29.VnB()),
                "mov z31.d, z29.d\n"
                "movprfx z29.b, p0/m, z28.b\n"
                "usqadd z29.b, p0/m, z29.b, z31.b");

  CLEANUP();
}

TEST(sve2_pair_arith) {
  SETUP();

  COMPARE(addp(z3.VnB(), p1.Merging(), z3.VnB(), z0.VnB()),
          "addp z3.b, p1/m, z3.b, z0.b");
  COMPARE(addp(z3.VnD(), p1.Merging(), z3.VnD(), z0.VnD()),
          "addp z3.d, p1/m, z3.d, z0.d");
  COMPARE(addp(z3.VnH(), p1.Merging(), z3.VnH(), z0.VnH()),
          "addp z3.h, p1/m, z3.h, z0.h");
  COMPARE(addp(z3.VnS(), p1.Merging(), z3.VnS(), z0.VnS()),
          "addp z3.s, p1/m, z3.s, z0.s");
  COMPARE(smaxp(z5.VnB(), p4.Merging(), z5.VnB(), z10.VnB()),
          "smaxp z5.b, p4/m, z5.b, z10.b");
  COMPARE(smaxp(z5.VnD(), p4.Merging(), z5.VnD(), z10.VnD()),
          "smaxp z5.d, p4/m, z5.d, z10.d");
  COMPARE(smaxp(z5.VnH(), p4.Merging(), z5.VnH(), z10.VnH()),
          "smaxp z5.h, p4/m, z5.h, z10.h");
  COMPARE(smaxp(z5.VnS(), p4.Merging(), z5.VnS(), z10.VnS()),
          "smaxp z5.s, p4/m, z5.s, z10.s");
  COMPARE(sminp(z27.VnB(), p3.Merging(), z27.VnB(), z1.VnB()),
          "sminp z27.b, p3/m, z27.b, z1.b");
  COMPARE(sminp(z27.VnD(), p3.Merging(), z27.VnD(), z1.VnD()),
          "sminp z27.d, p3/m, z27.d, z1.d");
  COMPARE(sminp(z27.VnH(), p3.Merging(), z27.VnH(), z1.VnH()),
          "sminp z27.h, p3/m, z27.h, z1.h");
  COMPARE(sminp(z27.VnS(), p3.Merging(), z27.VnS(), z1.VnS()),
          "sminp z27.s, p3/m, z27.s, z1.s");
  COMPARE(umaxp(z7.VnB(), p2.Merging(), z7.VnB(), z23.VnB()),
          "umaxp z7.b, p2/m, z7.b, z23.b");
  COMPARE(umaxp(z7.VnD(), p2.Merging(), z7.VnD(), z23.VnD()),
          "umaxp z7.d, p2/m, z7.d, z23.d");
  COMPARE(umaxp(z7.VnH(), p2.Merging(), z7.VnH(), z23.VnH()),
          "umaxp z7.h, p2/m, z7.h, z23.h");
  COMPARE(umaxp(z7.VnS(), p2.Merging(), z7.VnS(), z23.VnS()),
          "umaxp z7.s, p2/m, z7.s, z23.s");
  COMPARE(uminp(z10.VnB(), p0.Merging(), z10.VnB(), z22.VnB()),
          "uminp z10.b, p0/m, z10.b, z22.b");
  COMPARE(uminp(z10.VnD(), p0.Merging(), z10.VnD(), z22.VnD()),
          "uminp z10.d, p0/m, z10.d, z22.d");
  COMPARE(uminp(z10.VnH(), p0.Merging(), z10.VnH(), z22.VnH()),
          "uminp z10.h, p0/m, z10.h, z22.h");
  COMPARE(uminp(z10.VnS(), p0.Merging(), z10.VnS(), z22.VnS()),
          "uminp z10.s, p0/m, z10.s, z22.s");

  COMPARE_MACRO(Addp(z3.VnB(), p1.Merging(), z3.VnB(), z3.VnB()),
                "addp z3.b, p1/m, z3.b, z3.b");
  COMPARE_MACRO(Addp(z4.VnB(), p1.Merging(), z3.VnB(), z3.VnB()),
                "movprfx z4.b, p1/m, z3.b\n"
                "addp z4.b, p1/m, z4.b, z3.b");
  COMPARE_MACRO(Addp(z4.VnB(), p1.Merging(), z3.VnB(), z4.VnB()),
                "mov z31.d, z4.d\n"
                "movprfx z4.b, p1/m, z3.b\n"
                "addp z4.b, p1/m, z4.b, z31.b");
  COMPARE_MACRO(Smaxp(z4.VnB(), p1.Merging(), z3.VnB(), z4.VnB()),
                "mov z31.d, z4.d\n"
                "movprfx z4.b, p1/m, z3.b\n"
                "smaxp z4.b, p1/m, z4.b, z31.b");
  COMPARE_MACRO(Sminp(z4.VnB(), p1.Merging(), z3.VnB(), z4.VnB()),
                "mov z31.d, z4.d\n"
                "movprfx z4.b, p1/m, z3.b\n"
                "sminp z4.b, p1/m, z4.b, z31.b");
  COMPARE_MACRO(Umaxp(z4.VnB(), p1.Merging(), z3.VnB(), z4.VnB()),
                "mov z31.d, z4.d\n"
                "movprfx z4.b, p1/m, z3.b\n"
                "umaxp z4.b, p1/m, z4.b, z31.b");
  COMPARE_MACRO(Uminp(z4.VnB(), p1.Merging(), z3.VnB(), z4.VnB()),
                "mov z31.d, z4.d\n"
                "movprfx z4.b, p1/m, z3.b\n"
                "uminp z4.b, p1/m, z4.b, z31.b");
  CLEANUP();
}

TEST(sve2_extract_narrow) {
  SETUP();

  COMPARE(sqxtnb(z2.VnB(), z0.VnH()), "sqxtnb z2.b, z0.h");
  COMPARE(sqxtnb(z2.VnH(), z0.VnS()), "sqxtnb z2.h, z0.s");
  COMPARE(sqxtnb(z2.VnS(), z0.VnD()), "sqxtnb z2.s, z0.d");
  COMPARE(sqxtnt(z31.VnB(), z18.VnH()), "sqxtnt z31.b, z18.h");
  COMPARE(sqxtnt(z31.VnH(), z18.VnS()), "sqxtnt z31.h, z18.s");
  COMPARE(sqxtnt(z31.VnS(), z18.VnD()), "sqxtnt z31.s, z18.d");
  COMPARE(sqxtunb(z28.VnB(), z6.VnH()), "sqxtunb z28.b, z6.h");
  COMPARE(sqxtunb(z28.VnH(), z6.VnS()), "sqxtunb z28.h, z6.s");
  COMPARE(sqxtunb(z28.VnS(), z6.VnD()), "sqxtunb z28.s, z6.d");
  COMPARE(sqxtunt(z14.VnB(), z31.VnH()), "sqxtunt z14.b, z31.h");
  COMPARE(sqxtunt(z14.VnH(), z31.VnS()), "sqxtunt z14.h, z31.s");
  COMPARE(sqxtunt(z14.VnS(), z31.VnD()), "sqxtunt z14.s, z31.d");
  COMPARE(uqxtnb(z28.VnB(), z4.VnH()), "uqxtnb z28.b, z4.h");
  COMPARE(uqxtnb(z28.VnH(), z4.VnS()), "uqxtnb z28.h, z4.s");
  COMPARE(uqxtnb(z28.VnS(), z4.VnD()), "uqxtnb z28.s, z4.d");
  COMPARE(uqxtnt(z19.VnB(), z7.VnH()), "uqxtnt z19.b, z7.h");
  COMPARE(uqxtnt(z19.VnH(), z7.VnS()), "uqxtnt z19.h, z7.s");
  COMPARE(uqxtnt(z19.VnS(), z7.VnD()), "uqxtnt z19.s, z7.d");

  CLEANUP();
}

TEST(sve2_eorbt_eortb) {
  SETUP();

  COMPARE(eorbt(z3.VnB(), z10.VnB(), z8.VnB()), "eorbt z3.b, z10.b, z8.b");
  COMPARE(eorbt(z3.VnD(), z10.VnD(), z8.VnD()), "eorbt z3.d, z10.d, z8.d");
  COMPARE(eorbt(z3.VnH(), z10.VnH(), z8.VnH()), "eorbt z3.h, z10.h, z8.h");
  COMPARE(eorbt(z3.VnS(), z10.VnS(), z8.VnS()), "eorbt z3.s, z10.s, z8.s");
  COMPARE(eortb(z21.VnB(), z21.VnB(), z15.VnB()), "eortb z21.b, z21.b, z15.b");
  COMPARE(eortb(z21.VnD(), z21.VnD(), z15.VnD()), "eortb z21.d, z21.d, z15.d");
  COMPARE(eortb(z21.VnH(), z21.VnH(), z15.VnH()), "eortb z21.h, z21.h, z15.h");
  COMPARE(eortb(z21.VnS(), z21.VnS(), z15.VnS()), "eortb z21.s, z21.s, z15.s");

  CLEANUP();
}

TEST(sve2_saturating_multiply_add_high) {
  SETUP();

  COMPARE(sqrdmlah(z27.VnB(), z28.VnB(), z19.VnB()),
          "sqrdmlah z27.b, z28.b, z19.b");
  COMPARE(sqrdmlah(z27.VnD(), z28.VnD(), z19.VnD()),
          "sqrdmlah z27.d, z28.d, z19.d");
  COMPARE(sqrdmlah(z27.VnH(), z28.VnH(), z19.VnH()),
          "sqrdmlah z27.h, z28.h, z19.h");
  COMPARE(sqrdmlah(z27.VnS(), z28.VnS(), z19.VnS()),
          "sqrdmlah z27.s, z28.s, z19.s");
  COMPARE(sqrdmlsh(z11.VnB(), z16.VnB(), z31.VnB()),
          "sqrdmlsh z11.b, z16.b, z31.b");
  COMPARE(sqrdmlsh(z11.VnD(), z16.VnD(), z31.VnD()),
          "sqrdmlsh z11.d, z16.d, z31.d");
  COMPARE(sqrdmlsh(z11.VnH(), z16.VnH(), z31.VnH()),
          "sqrdmlsh z11.h, z16.h, z31.h");
  COMPARE(sqrdmlsh(z11.VnS(), z16.VnS(), z31.VnS()),
          "sqrdmlsh z11.s, z16.s, z31.s");

  COMPARE_MACRO(Sqrdmlah(z29.VnD(), z0.VnD(), z29.VnD(), z26.VnD()),
                "movprfx z31, z0\n"
                "sqrdmlah z31.d, z29.d, z26.d\n"
                "mov z29.d, z31.d");
  COMPARE_MACRO(Sqrdmlah(z26.VnH(), z0.VnH(), z29.VnH(), z26.VnH()),
                "movprfx z31, z0\n"
                "sqrdmlah z31.h, z29.h, z26.h\n"
                "mov z26.d, z31.d");
  COMPARE_MACRO(Sqrdmlsh(z23.VnS(), z31.VnS(), z26.VnS(), z29.VnS()),
                "movprfx z23, z31\n"
                "sqrdmlsh z23.s, z26.s, z29.s");
  COMPARE_MACRO(Sqrdmlsh(z4.VnB(), z31.VnB(), z4.VnB(), z4.VnB()),
                "sqrdmlsh z31.b, z4.b, z4.b\n"
                "mov z4.d, z31.d");

  COMPARE(sqrdmlah(z10.VnD(), z30.VnD(), z11.VnD(), 1),
          "sqrdmlah z10.d, z30.d, z11.d[1]");
  COMPARE(sqrdmlah(z11.VnH(), z8.VnH(), z3.VnH(), 7),
          "sqrdmlah z11.h, z8.h, z3.h[7]");
  COMPARE(sqrdmlah(z21.VnS(), z29.VnS(), z7.VnS(), 3),
          "sqrdmlah z21.s, z29.s, z7.s[3]");
  COMPARE(sqrdmlsh(z2.VnD(), z16.VnD(), z14.VnD(), 0),
          "sqrdmlsh z2.d, z16.d, z14.d[0]");
  COMPARE(sqrdmlsh(z23.VnH(), z13.VnH(), z6.VnH(), 5),
          "sqrdmlsh z23.h, z13.h, z6.h[5]");
  COMPARE(sqrdmlsh(z27.VnS(), z8.VnS(), z4.VnS(), 2),
          "sqrdmlsh z27.s, z8.s, z4.s[2]");

  COMPARE_MACRO(Sqrdmlah(z24.VnD(), z0.VnD(), z24.VnD(), z13.VnD(), 0),
                "movprfx z31, z0\n"
                "sqrdmlah z31.d, z24.d, z13.d[0]\n"
                "mov z24.d, z31.d");
  COMPARE_MACRO(Sqrdmlah(z4.VnH(), z0.VnH(), z29.VnH(), z4.VnH(), 6),
                "movprfx z31, z0\n"
                "sqrdmlah z31.h, z29.h, z4.h[6]\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Sqrdmlsh(z12.VnS(), z31.VnS(), z26.VnS(), z2.VnS(), 2),
                "movprfx z12, z31\n"
                "sqrdmlsh z12.s, z26.s, z2.s[2]");
  COMPARE_MACRO(Sqrdmlsh(z0.VnD(), z31.VnD(), z0.VnD(), z0.VnD(), 1),
                "sqrdmlsh z31.d, z0.d, z0.d[1]\n"
                "mov z0.d, z31.d");

  CLEANUP();
}

TEST(sve2_integer_pairwise_add_accumulate_long) {
  SETUP();

  COMPARE(sadalp(z19.VnD(), p5.Merging(), z9.VnS()),
          "sadalp z19.d, p5/m, z9.s");
  COMPARE(sadalp(z19.VnH(), p5.Merging(), z9.VnB()),
          "sadalp z19.h, p5/m, z9.b");
  COMPARE(sadalp(z19.VnS(), p5.Merging(), z9.VnH()),
          "sadalp z19.s, p5/m, z9.h");
  COMPARE(uadalp(z20.VnD(), p4.Merging(), z5.VnS()),
          "uadalp z20.d, p4/m, z5.s");
  COMPARE(uadalp(z20.VnH(), p4.Merging(), z5.VnB()),
          "uadalp z20.h, p4/m, z5.b");
  COMPARE(uadalp(z20.VnS(), p4.Merging(), z5.VnH()),
          "uadalp z20.s, p4/m, z5.h");

  CLEANUP();
}

TEST(sve2_integer_multiply_vectors_unpredicated) {
  SETUP();

  COMPARE(mul(z23.VnB(), z0.VnB(), z12.VnB()), "mul z23.b, z0.b, z12.b");
  COMPARE(mul(z24.VnD(), z1.VnD(), z14.VnD()), "mul z24.d, z1.d, z14.d");
  COMPARE(mul(z25.VnH(), z2.VnH(), z16.VnH()), "mul z25.h, z2.h, z16.h");
  COMPARE(mul(z26.VnS(), z3.VnS(), z18.VnS()), "mul z26.s, z3.s, z18.s");

  COMPARE(pmul(z0.VnB(), z5.VnB(), z5.VnB()), "pmul z0.b, z5.b, z5.b");

  COMPARE(smulh(z11.VnB(), z9.VnB(), z1.VnB()), "smulh z11.b, z9.b, z1.b");
  COMPARE(smulh(z21.VnD(), z19.VnD(), z16.VnD()), "smulh z21.d, z19.d, z16.d");
  COMPARE(smulh(z11.VnH(), z9.VnH(), z1.VnH()), "smulh z11.h, z9.h, z1.h");
  COMPARE(smulh(z21.VnS(), z19.VnS(), z16.VnS()), "smulh z21.s, z19.s, z16.s");

  COMPARE(umulh(z5.VnB(), z9.VnB(), z5.VnB()), "umulh z5.b, z9.b, z5.b");
  COMPARE(umulh(z18.VnD(), z9.VnD(), z5.VnD()), "umulh z18.d, z9.d, z5.d");
  COMPARE(umulh(z18.VnH(), z9.VnH(), z9.VnH()), "umulh z18.h, z9.h, z9.h");
  COMPARE(umulh(z18.VnS(), z9.VnS(), z18.VnS()), "umulh z18.s, z9.s, z18.s");

  CLEANUP();
}

TEST(sve2_arith_interleaved_long) {
  SETUP();

  COMPARE(saddlbt(z15.VnD(), z6.VnS(), z18.VnS()),
          "saddlbt z15.d, z6.s, z18.s");
  COMPARE(saddlbt(z15.VnH(), z6.VnB(), z18.VnB()),
          "saddlbt z15.h, z6.b, z18.b");
  COMPARE(saddlbt(z15.VnS(), z6.VnH(), z18.VnH()),
          "saddlbt z15.s, z6.h, z18.h");
  COMPARE(ssublbt(z6.VnD(), z28.VnS(), z12.VnS()),
          "ssublbt z6.d, z28.s, z12.s");
  COMPARE(ssublbt(z6.VnH(), z28.VnB(), z12.VnB()),
          "ssublbt z6.h, z28.b, z12.b");
  COMPARE(ssublbt(z6.VnS(), z28.VnH(), z12.VnH()),
          "ssublbt z6.s, z28.h, z12.h");
  COMPARE(ssubltb(z11.VnD(), z18.VnS(), z19.VnS()),
          "ssubltb z11.d, z18.s, z19.s");
  COMPARE(ssubltb(z11.VnH(), z18.VnB(), z19.VnB()),
          "ssubltb z11.h, z18.b, z19.b");
  COMPARE(ssubltb(z11.VnS(), z18.VnH(), z19.VnH()),
          "ssubltb z11.s, z18.h, z19.h");

  CLEANUP();
}

TEST(sve2_int_unary_predicated) {
  SETUP();

  COMPARE_MACRO(Sqabs(z29.VnB(), p1.Merging(), z18.VnB()),
                "sqabs z29.b, p1/m, z18.b");
  COMPARE_MACRO(Sqabs(z29.VnD(), p1.Merging(), z18.VnD()),
                "sqabs z29.d, p1/m, z18.d");
  COMPARE_MACRO(Sqabs(z29.VnH(), p1.Merging(), z18.VnH()),
                "sqabs z29.h, p1/m, z18.h");
  COMPARE_MACRO(Sqabs(z29.VnS(), p1.Merging(), z18.VnS()),
                "sqabs z29.s, p1/m, z18.s");
  COMPARE_MACRO(Sqneg(z21.VnB(), p0.Merging(), z17.VnB()),
                "sqneg z21.b, p0/m, z17.b");
  COMPARE_MACRO(Sqneg(z21.VnD(), p0.Merging(), z17.VnD()),
                "sqneg z21.d, p0/m, z17.d");
  COMPARE_MACRO(Sqneg(z21.VnH(), p0.Merging(), z17.VnH()),
                "sqneg z21.h, p0/m, z17.h");
  COMPARE_MACRO(Sqneg(z21.VnS(), p0.Merging(), z17.VnS()),
                "sqneg z21.s, p0/m, z17.s");
  COMPARE_MACRO(Urecpe(z25.VnS(), p7.Merging(), z2.VnS()),
                "urecpe z25.s, p7/m, z2.s");
  COMPARE_MACRO(Ursqrte(z4.VnS(), p3.Merging(), z3.VnS()),
                "ursqrte z4.s, p3/m, z3.s");

  COMPARE_MACRO(Sqabs(z29.VnS(), p1.Zeroing(), z18.VnS()),
                "movprfx z29.s, p1/z, z29.s\n"
                "sqabs z29.s, p1/m, z18.s");
  COMPARE_MACRO(Sqneg(z21.VnB(), p0.Zeroing(), z17.VnB()),
                "movprfx z21.b, p0/z, z21.b\n"
                "sqneg z21.b, p0/m, z17.b");
  COMPARE_MACRO(Urecpe(z25.VnS(), p7.Zeroing(), z2.VnS()),
                "movprfx z25.s, p7/z, z25.s\n"
                "urecpe z25.s, p7/m, z2.s");
  COMPARE_MACRO(Ursqrte(z4.VnS(), p3.Zeroing(), z3.VnS()),
                "movprfx z4.s, p3/z, z4.s\n"
                "ursqrte z4.s, p3/m, z3.s");
  CLEANUP();
}

TEST(sve2_arith_long) {
  SETUP();

  COMPARE_MACRO(Sabdlb(z2.VnD(), z21.VnS(), z3.VnS()),
                "sabdlb z2.d, z21.s, z3.s");
  COMPARE_MACRO(Sabdlb(z2.VnH(), z21.VnB(), z3.VnB()),
                "sabdlb z2.h, z21.b, z3.b");
  COMPARE_MACRO(Sabdlb(z2.VnS(), z21.VnH(), z3.VnH()),
                "sabdlb z2.s, z21.h, z3.h");
  COMPARE_MACRO(Sabdlt(z25.VnD(), z23.VnS(), z17.VnS()),
                "sabdlt z25.d, z23.s, z17.s");
  COMPARE_MACRO(Sabdlt(z25.VnH(), z23.VnB(), z17.VnB()),
                "sabdlt z25.h, z23.b, z17.b");
  COMPARE_MACRO(Sabdlt(z25.VnS(), z23.VnH(), z17.VnH()),
                "sabdlt z25.s, z23.h, z17.h");
  COMPARE_MACRO(Saddlb(z24.VnD(), z30.VnS(), z16.VnS()),
                "saddlb z24.d, z30.s, z16.s");
  COMPARE_MACRO(Saddlb(z24.VnH(), z30.VnB(), z16.VnB()),
                "saddlb z24.h, z30.b, z16.b");
  COMPARE_MACRO(Saddlb(z24.VnS(), z30.VnH(), z16.VnH()),
                "saddlb z24.s, z30.h, z16.h");
  COMPARE_MACRO(Saddlt(z21.VnD(), z29.VnS(), z31.VnS()),
                "saddlt z21.d, z29.s, z31.s");
  COMPARE_MACRO(Saddlt(z21.VnH(), z29.VnB(), z31.VnB()),
                "saddlt z21.h, z29.b, z31.b");
  COMPARE_MACRO(Saddlt(z21.VnS(), z29.VnH(), z31.VnH()),
                "saddlt z21.s, z29.h, z31.h");
  COMPARE_MACRO(Ssublb(z4.VnD(), z23.VnS(), z7.VnS()),
                "ssublb z4.d, z23.s, z7.s");
  COMPARE_MACRO(Ssublb(z4.VnH(), z23.VnB(), z7.VnB()),
                "ssublb z4.h, z23.b, z7.b");
  COMPARE_MACRO(Ssublb(z4.VnS(), z23.VnH(), z7.VnH()),
                "ssublb z4.s, z23.h, z7.h");
  COMPARE_MACRO(Ssublt(z12.VnD(), z13.VnS(), z6.VnS()),
                "ssublt z12.d, z13.s, z6.s");
  COMPARE_MACRO(Ssublt(z12.VnH(), z13.VnB(), z6.VnB()),
                "ssublt z12.h, z13.b, z6.b");
  COMPARE_MACRO(Ssublt(z12.VnS(), z13.VnH(), z6.VnH()),
                "ssublt z12.s, z13.h, z6.h");
  COMPARE_MACRO(Uabdlb(z1.VnD(), z26.VnS(), z12.VnS()),
                "uabdlb z1.d, z26.s, z12.s");
  COMPARE_MACRO(Uabdlb(z1.VnH(), z26.VnB(), z12.VnB()),
                "uabdlb z1.h, z26.b, z12.b");
  COMPARE_MACRO(Uabdlb(z1.VnS(), z26.VnH(), z12.VnH()),
                "uabdlb z1.s, z26.h, z12.h");
  COMPARE_MACRO(Uabdlt(z25.VnD(), z29.VnS(), z14.VnS()),
                "uabdlt z25.d, z29.s, z14.s");
  COMPARE_MACRO(Uabdlt(z25.VnH(), z29.VnB(), z14.VnB()),
                "uabdlt z25.h, z29.b, z14.b");
  COMPARE_MACRO(Uabdlt(z25.VnS(), z29.VnH(), z14.VnH()),
                "uabdlt z25.s, z29.h, z14.h");
  COMPARE_MACRO(Uaddlb(z3.VnD(), z5.VnS(), z2.VnS()),
                "uaddlb z3.d, z5.s, z2.s");
  COMPARE_MACRO(Uaddlb(z3.VnH(), z5.VnB(), z2.VnB()),
                "uaddlb z3.h, z5.b, z2.b");
  COMPARE_MACRO(Uaddlb(z3.VnS(), z5.VnH(), z2.VnH()),
                "uaddlb z3.s, z5.h, z2.h");
  COMPARE_MACRO(Uaddlt(z15.VnD(), z28.VnS(), z20.VnS()),
                "uaddlt z15.d, z28.s, z20.s");
  COMPARE_MACRO(Uaddlt(z15.VnH(), z28.VnB(), z20.VnB()),
                "uaddlt z15.h, z28.b, z20.b");
  COMPARE_MACRO(Uaddlt(z15.VnS(), z28.VnH(), z20.VnH()),
                "uaddlt z15.s, z28.h, z20.h");
  COMPARE_MACRO(Usublb(z25.VnD(), z9.VnS(), z17.VnS()),
                "usublb z25.d, z9.s, z17.s");
  COMPARE_MACRO(Usublb(z25.VnH(), z9.VnB(), z17.VnB()),
                "usublb z25.h, z9.b, z17.b");
  COMPARE_MACRO(Usublb(z25.VnS(), z9.VnH(), z17.VnH()),
                "usublb z25.s, z9.h, z17.h");
  COMPARE_MACRO(Usublt(z5.VnD(), z11.VnS(), z15.VnS()),
                "usublt z5.d, z11.s, z15.s");
  COMPARE_MACRO(Usublt(z5.VnH(), z11.VnB(), z15.VnB()),
                "usublt z5.h, z11.b, z15.b");
  COMPARE_MACRO(Usublt(z5.VnS(), z11.VnH(), z15.VnH()),
                "usublt z5.s, z11.h, z15.h");

  CLEANUP();
}

TEST(sve2_arith_wide) {
  SETUP();

  COMPARE_MACRO(Saddwb(z12.VnD(), z8.VnD(), z8.VnS()),
                "saddwb z12.d, z8.d, z8.s");
  COMPARE_MACRO(Saddwb(z12.VnH(), z8.VnH(), z8.VnB()),
                "saddwb z12.h, z8.h, z8.b");
  COMPARE_MACRO(Saddwb(z12.VnS(), z8.VnS(), z8.VnH()),
                "saddwb z12.s, z8.s, z8.h");
  COMPARE_MACRO(Saddwt(z24.VnD(), z0.VnD(), z3.VnS()),
                "saddwt z24.d, z0.d, z3.s");
  COMPARE_MACRO(Saddwt(z24.VnH(), z0.VnH(), z3.VnB()),
                "saddwt z24.h, z0.h, z3.b");
  COMPARE_MACRO(Saddwt(z24.VnS(), z0.VnS(), z3.VnH()),
                "saddwt z24.s, z0.s, z3.h");
  COMPARE_MACRO(Ssubwb(z7.VnD(), z28.VnD(), z11.VnS()),
                "ssubwb z7.d, z28.d, z11.s");
  COMPARE_MACRO(Ssubwb(z7.VnH(), z28.VnH(), z11.VnB()),
                "ssubwb z7.h, z28.h, z11.b");
  COMPARE_MACRO(Ssubwb(z7.VnS(), z28.VnS(), z11.VnH()),
                "ssubwb z7.s, z28.s, z11.h");
  COMPARE_MACRO(Ssubwt(z29.VnD(), z25.VnD(), z20.VnS()),
                "ssubwt z29.d, z25.d, z20.s");
  COMPARE_MACRO(Ssubwt(z29.VnH(), z25.VnH(), z20.VnB()),
                "ssubwt z29.h, z25.h, z20.b");
  COMPARE_MACRO(Ssubwt(z29.VnS(), z25.VnS(), z20.VnH()),
                "ssubwt z29.s, z25.s, z20.h");
  COMPARE_MACRO(Uaddwb(z31.VnD(), z8.VnD(), z25.VnS()),
                "uaddwb z31.d, z8.d, z25.s");
  COMPARE_MACRO(Uaddwb(z31.VnH(), z8.VnH(), z25.VnB()),
                "uaddwb z31.h, z8.h, z25.b");
  COMPARE_MACRO(Uaddwb(z31.VnS(), z8.VnS(), z25.VnH()),
                "uaddwb z31.s, z8.s, z25.h");
  COMPARE_MACRO(Uaddwt(z17.VnD(), z15.VnD(), z2.VnS()),
                "uaddwt z17.d, z15.d, z2.s");
  COMPARE_MACRO(Uaddwt(z17.VnH(), z15.VnH(), z2.VnB()),
                "uaddwt z17.h, z15.h, z2.b");
  COMPARE_MACRO(Uaddwt(z17.VnS(), z15.VnS(), z2.VnH()),
                "uaddwt z17.s, z15.s, z2.h");
  COMPARE_MACRO(Usubwb(z10.VnD(), z13.VnD(), z20.VnS()),
                "usubwb z10.d, z13.d, z20.s");
  COMPARE_MACRO(Usubwb(z10.VnH(), z13.VnH(), z20.VnB()),
                "usubwb z10.h, z13.h, z20.b");
  COMPARE_MACRO(Usubwb(z10.VnS(), z13.VnS(), z20.VnH()),
                "usubwb z10.s, z13.s, z20.h");
  COMPARE_MACRO(Usubwt(z15.VnD(), z8.VnD(), z23.VnS()),
                "usubwt z15.d, z8.d, z23.s");
  COMPARE_MACRO(Usubwt(z15.VnH(), z8.VnH(), z23.VnB()),
                "usubwt z15.h, z8.h, z23.b");
  COMPARE_MACRO(Usubwt(z15.VnS(), z8.VnS(), z23.VnH()),
                "usubwt z15.s, z8.s, z23.h");

  CLEANUP();
}

TEST(sve2_shift_long) {
  SETUP();

  COMPARE_MACRO(Sshllb(z2.VnH(), z20.VnB(), 0), "sshllb z2.h, z20.b, #0");
  COMPARE_MACRO(Sshllb(z2.VnH(), z20.VnB(), 1), "sshllb z2.h, z20.b, #1");
  COMPARE_MACRO(Sshllb(z2.VnH(), z20.VnB(), 5), "sshllb z2.h, z20.b, #5");
  COMPARE_MACRO(Sshllb(z2.VnH(), z20.VnB(), 7), "sshllb z2.h, z20.b, #7");
  COMPARE_MACRO(Sshllb(z2.VnS(), z20.VnH(), 0), "sshllb z2.s, z20.h, #0");
  COMPARE_MACRO(Sshllb(z2.VnS(), z20.VnH(), 15), "sshllb z2.s, z20.h, #15");
  COMPARE_MACRO(Sshllb(z2.VnD(), z20.VnS(), 0), "sshllb z2.d, z20.s, #0");
  COMPARE_MACRO(Sshllb(z2.VnD(), z20.VnS(), 31), "sshllb z2.d, z20.s, #31");
  COMPARE_MACRO(Sshllt(z27.VnH(), z8.VnB(), 0), "sshllt z27.h, z8.b, #0");
  COMPARE_MACRO(Sshllt(z27.VnH(), z8.VnB(), 7), "sshllt z27.h, z8.b, #7");
  COMPARE_MACRO(Sshllt(z27.VnS(), z8.VnH(), 0), "sshllt z27.s, z8.h, #0");
  COMPARE_MACRO(Sshllt(z27.VnS(), z8.VnH(), 15), "sshllt z27.s, z8.h, #15");
  COMPARE_MACRO(Sshllt(z27.VnD(), z8.VnS(), 0), "sshllt z27.d, z8.s, #0");
  COMPARE_MACRO(Sshllt(z27.VnD(), z8.VnS(), 31), "sshllt z27.d, z8.s, #31");
  COMPARE_MACRO(Ushllb(z8.VnH(), z31.VnB(), 0), "ushllb z8.h, z31.b, #0");
  COMPARE_MACRO(Ushllb(z8.VnH(), z31.VnB(), 7), "ushllb z8.h, z31.b, #7");
  COMPARE_MACRO(Ushllb(z8.VnS(), z31.VnH(), 0), "ushllb z8.s, z31.h, #0");
  COMPARE_MACRO(Ushllb(z8.VnS(), z31.VnH(), 15), "ushllb z8.s, z31.h, #15");
  COMPARE_MACRO(Ushllb(z8.VnD(), z31.VnS(), 0), "ushllb z8.d, z31.s, #0");
  COMPARE_MACRO(Ushllb(z8.VnD(), z31.VnS(), 31), "ushllb z8.d, z31.s, #31");
  COMPARE_MACRO(Ushllt(z3.VnH(), z21.VnB(), 0), "ushllt z3.h, z21.b, #0");
  COMPARE_MACRO(Ushllt(z3.VnH(), z21.VnB(), 7), "ushllt z3.h, z21.b, #7");
  COMPARE_MACRO(Ushllt(z3.VnS(), z21.VnH(), 0), "ushllt z3.s, z21.h, #0");
  COMPARE_MACRO(Ushllt(z3.VnS(), z21.VnH(), 15), "ushllt z3.s, z21.h, #15");
  COMPARE_MACRO(Ushllt(z3.VnD(), z21.VnS(), 0), "ushllt z3.d, z21.s, #0");
  COMPARE_MACRO(Ushllt(z3.VnD(), z21.VnS(), 31), "ushllt z3.d, z21.s, #31");

  CLEANUP();
}

TEST(sve2_shift_narrow) {
  SETUP();

  COMPARE_MACRO(Shrnb(z7.VnB(), z4.VnH(), 1), "shrnb z7.b, z4.h, #1");
  COMPARE_MACRO(Shrnb(z7.VnB(), z4.VnH(), 2), "shrnb z7.b, z4.h, #2");
  COMPARE_MACRO(Shrnb(z7.VnB(), z4.VnH(), 5), "shrnb z7.b, z4.h, #5");
  COMPARE_MACRO(Shrnb(z7.VnB(), z4.VnH(), 8), "shrnb z7.b, z4.h, #8");
  COMPARE_MACRO(Shrnb(z7.VnH(), z4.VnS(), 1), "shrnb z7.h, z4.s, #1");
  COMPARE_MACRO(Shrnb(z7.VnH(), z4.VnS(), 16), "shrnb z7.h, z4.s, #16");
  COMPARE_MACRO(Shrnb(z7.VnS(), z4.VnD(), 1), "shrnb z7.s, z4.d, #1");
  COMPARE_MACRO(Shrnb(z7.VnS(), z4.VnD(), 32), "shrnb z7.s, z4.d, #32");
  COMPARE_MACRO(Shrnt(z21.VnB(), z29.VnH(), 1), "shrnt z21.b, z29.h, #1");
  COMPARE_MACRO(Shrnt(z21.VnB(), z29.VnH(), 8), "shrnt z21.b, z29.h, #8");
  COMPARE_MACRO(Shrnt(z21.VnH(), z29.VnS(), 1), "shrnt z21.h, z29.s, #1");
  COMPARE_MACRO(Shrnt(z21.VnH(), z29.VnS(), 16), "shrnt z21.h, z29.s, #16");
  COMPARE_MACRO(Shrnt(z21.VnS(), z29.VnD(), 1), "shrnt z21.s, z29.d, #1");
  COMPARE_MACRO(Shrnt(z21.VnS(), z29.VnD(), 32), "shrnt z21.s, z29.d, #32");

  COMPARE_MACRO(Rshrnb(z5.VnB(), z1.VnH(), 1), "rshrnb z5.b, z1.h, #1");
  COMPARE_MACRO(Rshrnb(z5.VnB(), z1.VnH(), 8), "rshrnb z5.b, z1.h, #8");
  COMPARE_MACRO(Rshrnb(z5.VnH(), z1.VnS(), 16), "rshrnb z5.h, z1.s, #16");
  COMPARE_MACRO(Rshrnb(z5.VnS(), z1.VnD(), 32), "rshrnb z5.s, z1.d, #32");
  COMPARE_MACRO(Rshrnt(z5.VnB(), z1.VnH(), 8), "rshrnt z5.b, z1.h, #8");
  COMPARE_MACRO(Rshrnt(z5.VnH(), z1.VnS(), 16), "rshrnt z5.h, z1.s, #16");
  COMPARE_MACRO(Rshrnt(z5.VnS(), z1.VnD(), 32), "rshrnt z5.s, z1.d, #32");

  COMPARE_MACRO(Sqrshrnb(z1.VnB(), z1.VnH(), 1), "sqrshrnb z1.b, z1.h, #1");
  COMPARE_MACRO(Sqrshrnb(z1.VnB(), z1.VnH(), 8), "sqrshrnb z1.b, z1.h, #8");
  COMPARE_MACRO(Sqrshrnb(z1.VnH(), z1.VnS(), 16), "sqrshrnb z1.h, z1.s, #16");
  COMPARE_MACRO(Sqrshrnb(z1.VnS(), z1.VnD(), 32), "sqrshrnb z1.s, z1.d, #32");
  COMPARE_MACRO(Sqrshrnt(z24.VnB(), z19.VnH(), 8), "sqrshrnt z24.b, z19.h, #8");
  COMPARE_MACRO(Sqrshrnt(z24.VnH(), z19.VnS(), 16),
                "sqrshrnt z24.h, z19.s, #16");
  COMPARE_MACRO(Sqrshrnt(z24.VnS(), z19.VnD(), 32),
                "sqrshrnt z24.s, z19.d, #32");

  COMPARE_MACRO(Sqshrnb(z25.VnB(), z1.VnH(), 1), "sqshrnb z25.b, z1.h, #1");
  COMPARE_MACRO(Sqshrnb(z25.VnB(), z1.VnH(), 8), "sqshrnb z25.b, z1.h, #8");
  COMPARE_MACRO(Sqshrnb(z25.VnH(), z1.VnS(), 16), "sqshrnb z25.h, z1.s, #16");
  COMPARE_MACRO(Sqshrnb(z25.VnS(), z1.VnD(), 32), "sqshrnb z25.s, z1.d, #32");
  COMPARE_MACRO(Sqshrnt(z0.VnB(), z25.VnH(), 8), "sqshrnt z0.b, z25.h, #8");
  COMPARE_MACRO(Sqshrnt(z0.VnH(), z25.VnS(), 16), "sqshrnt z0.h, z25.s, #16");
  COMPARE_MACRO(Sqshrnt(z0.VnS(), z25.VnD(), 32), "sqshrnt z0.s, z25.d, #32");

  COMPARE_MACRO(Uqrshrnb(z30.VnB(), z25.VnH(), 1), "uqrshrnb z30.b, z25.h, #1");
  COMPARE_MACRO(Uqrshrnb(z30.VnB(), z25.VnH(), 8), "uqrshrnb z30.b, z25.h, #8");
  COMPARE_MACRO(Uqrshrnb(z30.VnH(), z25.VnS(), 16),
                "uqrshrnb z30.h, z25.s, #16");
  COMPARE_MACRO(Uqrshrnb(z30.VnS(), z25.VnD(), 32),
                "uqrshrnb z30.s, z25.d, #32");
  COMPARE_MACRO(Uqrshrnt(z3.VnB(), z25.VnH(), 8), "uqrshrnt z3.b, z25.h, #8");
  COMPARE_MACRO(Uqrshrnt(z3.VnH(), z25.VnS(), 16), "uqrshrnt z3.h, z25.s, #16");
  COMPARE_MACRO(Uqrshrnt(z3.VnS(), z25.VnD(), 32), "uqrshrnt z3.s, z25.d, #32");

  COMPARE_MACRO(Uqshrnb(z17.VnB(), z4.VnH(), 1), "uqshrnb z17.b, z4.h, #1");
  COMPARE_MACRO(Uqshrnb(z17.VnB(), z4.VnH(), 8), "uqshrnb z17.b, z4.h, #8");
  COMPARE_MACRO(Uqshrnb(z17.VnH(), z4.VnS(), 16), "uqshrnb z17.h, z4.s, #16");
  COMPARE_MACRO(Uqshrnb(z17.VnS(), z4.VnD(), 32), "uqshrnb z17.s, z4.d, #32");
  COMPARE_MACRO(Uqshrnt(z28.VnB(), z18.VnH(), 8), "uqshrnt z28.b, z18.h, #8");
  COMPARE_MACRO(Uqshrnt(z28.VnH(), z18.VnS(), 16), "uqshrnt z28.h, z18.s, #16");
  COMPARE_MACRO(Uqshrnt(z28.VnS(), z18.VnD(), 32), "uqshrnt z28.s, z18.d, #32");

  COMPARE_MACRO(Sqrshrunb(z23.VnB(), z28.VnH(), 1),
                "sqrshrunb z23.b, z28.h, #1");
  COMPARE_MACRO(Sqrshrunb(z23.VnB(), z28.VnH(), 8),
                "sqrshrunb z23.b, z28.h, #8");
  COMPARE_MACRO(Sqrshrunb(z23.VnH(), z28.VnS(), 16),
                "sqrshrunb z23.h, z28.s, #16");
  COMPARE_MACRO(Sqrshrunb(z23.VnS(), z28.VnD(), 32),
                "sqrshrunb z23.s, z28.d, #32");
  COMPARE_MACRO(Sqrshrunt(z9.VnB(), z15.VnH(), 8), "sqrshrunt z9.b, z15.h, #8");
  COMPARE_MACRO(Sqrshrunt(z9.VnH(), z15.VnS(), 16),
                "sqrshrunt z9.h, z15.s, #16");
  COMPARE_MACRO(Sqrshrunt(z9.VnS(), z15.VnD(), 32),
                "sqrshrunt z9.s, z15.d, #32");

  COMPARE_MACRO(Sqshrunb(z25.VnB(), z10.VnH(), 1), "sqshrunb z25.b, z10.h, #1");
  COMPARE_MACRO(Sqshrunb(z25.VnB(), z10.VnH(), 8), "sqshrunb z25.b, z10.h, #8");
  COMPARE_MACRO(Sqshrunb(z25.VnH(), z10.VnS(), 16),
                "sqshrunb z25.h, z10.s, #16");
  COMPARE_MACRO(Sqshrunb(z25.VnS(), z10.VnD(), 32),
                "sqshrunb z25.s, z10.d, #32");
  COMPARE_MACRO(Sqshrunt(z20.VnB(), z3.VnH(), 8), "sqshrunt z20.b, z3.h, #8");
  COMPARE_MACRO(Sqshrunt(z20.VnH(), z3.VnS(), 16), "sqshrunt z20.h, z3.s, #16");
  COMPARE_MACRO(Sqshrunt(z20.VnS(), z3.VnD(), 32), "sqshrunt z20.s, z3.d, #32");

  CLEANUP();
}

TEST(sve2_aba_long) {
  SETUP();

  COMPARE(sabalb(z13.VnD(), z20.VnS(), z26.VnS()),
          "sabalb z13.d, z20.s, z26.s");
  COMPARE(sabalb(z13.VnH(), z20.VnB(), z26.VnB()),
          "sabalb z13.h, z20.b, z26.b");
  COMPARE(sabalb(z13.VnS(), z20.VnH(), z26.VnH()),
          "sabalb z13.s, z20.h, z26.h");
  COMPARE(sabalt(z14.VnD(), z19.VnS(), z10.VnS()),
          "sabalt z14.d, z19.s, z10.s");
  COMPARE(sabalt(z14.VnH(), z19.VnB(), z10.VnB()),
          "sabalt z14.h, z19.b, z10.b");
  COMPARE(sabalt(z14.VnS(), z19.VnH(), z10.VnH()),
          "sabalt z14.s, z19.h, z10.h");
  COMPARE(uabalb(z11.VnD(), z25.VnS(), z11.VnS()),
          "uabalb z11.d, z25.s, z11.s");
  COMPARE(uabalb(z11.VnH(), z25.VnB(), z11.VnB()),
          "uabalb z11.h, z25.b, z11.b");
  COMPARE(uabalb(z11.VnS(), z25.VnH(), z11.VnH()),
          "uabalb z11.s, z25.h, z11.h");
  COMPARE(uabalt(z4.VnD(), z2.VnS(), z31.VnS()), "uabalt z4.d, z2.s, z31.s");
  COMPARE(uabalt(z4.VnH(), z2.VnB(), z31.VnB()), "uabalt z4.h, z2.b, z31.b");
  COMPARE(uabalt(z4.VnS(), z2.VnH(), z31.VnH()), "uabalt z4.s, z2.h, z31.h");

  COMPARE_MACRO(Sabalb(z12.VnH(), z12.VnH(), z3.VnB(), z30.VnB()),
                "sabalb z12.h, z3.b, z30.b");
  COMPARE_MACRO(Sabalt(z12.VnH(), z12.VnH(), z3.VnB(), z12.VnB()),
                "sabalt z12.h, z3.b, z12.b");
  COMPARE_MACRO(Sabalb(z12.VnH(), z12.VnH(), z12.VnB(), z30.VnB()),
                "sabalb z12.h, z12.b, z30.b");
  COMPARE_MACRO(Sabalt(z12.VnH(), z12.VnH(), z12.VnB(), z12.VnB()), "");
  COMPARE_MACRO(Sabalb(z12.VnH(), z13.VnH(), z3.VnB(), z30.VnB()),
                "movprfx z12, z13\n"
                "sabalb z12.h, z3.b, z30.b");
  COMPARE_MACRO(Sabalt(z12.VnH(), z3.VnH(), z3.VnB(), z30.VnB()),
                "movprfx z12, z3\n"
                "sabalt z12.h, z3.b, z30.b");
  COMPARE_MACRO(Sabalb(z12.VnH(), z30.VnH(), z3.VnB(), z30.VnB()),
                "movprfx z12, z30\n"
                "sabalb z12.h, z3.b, z30.b");
  COMPARE_MACRO(Sabalt(z12.VnH(), z3.VnH(), z3.VnB(), z3.VnB()),
                "mov z12.d, z3.d");
  COMPARE_MACRO(Sabalb(z12.VnH(), z3.VnH(), z12.VnB(), z3.VnB()),
                "mov z31.d, z12.d\n"
                "movprfx z12, z3\n"
                "sabalb z12.h, z31.b, z3.b");
  COMPARE_MACRO(Sabalt(z12.VnH(), z3.VnH(), z3.VnB(), z12.VnB()),
                "mov z31.d, z12.d\n"
                "movprfx z12, z3\n"
                "sabalt z12.h, z3.b, z31.b");

  COMPARE_MACRO(Uabalt(z12.VnH(), z12.VnH(), z3.VnB(), z30.VnB()),
                "uabalt z12.h, z3.b, z30.b");
  COMPARE_MACRO(Uabalb(z12.VnH(), z12.VnH(), z3.VnB(), z12.VnB()),
                "uabalb z12.h, z3.b, z12.b");
  COMPARE_MACRO(Uabalt(z12.VnH(), z12.VnH(), z12.VnB(), z30.VnB()),
                "uabalt z12.h, z12.b, z30.b");
  COMPARE_MACRO(Uabalb(z12.VnH(), z12.VnH(), z12.VnB(), z12.VnB()), "");
  COMPARE_MACRO(Uabalt(z12.VnH(), z13.VnH(), z3.VnB(), z30.VnB()),
                "movprfx z12, z13\n"
                "uabalt z12.h, z3.b, z30.b");
  COMPARE_MACRO(Uabalb(z12.VnH(), z3.VnH(), z3.VnB(), z30.VnB()),
                "movprfx z12, z3\n"
                "uabalb z12.h, z3.b, z30.b");
  COMPARE_MACRO(Uabalt(z12.VnH(), z30.VnH(), z3.VnB(), z30.VnB()),
                "movprfx z12, z30\n"
                "uabalt z12.h, z3.b, z30.b");
  COMPARE_MACRO(Uabalb(z12.VnH(), z3.VnH(), z3.VnB(), z3.VnB()),
                "mov z12.d, z3.d");
  COMPARE_MACRO(Uabalt(z12.VnH(), z3.VnH(), z12.VnB(), z3.VnB()),
                "mov z31.d, z12.d\n"
                "movprfx z12, z3\n"
                "uabalt z12.h, z31.b, z3.b");
  COMPARE_MACRO(Uabalb(z12.VnH(), z3.VnH(), z3.VnB(), z12.VnB()),
                "mov z31.d, z12.d\n"
                "movprfx z12, z3\n"
                "uabalb z12.h, z3.b, z31.b");
  CLEANUP();
}

TEST(sve2_add_sub_carry) {
  SETUP();

  COMPARE(adclb(z25.VnS(), z17.VnS(), z24.VnS()), "adclb z25.s, z17.s, z24.s");
  COMPARE(adclb(z25.VnD(), z17.VnD(), z24.VnD()), "adclb z25.d, z17.d, z24.d");
  COMPARE(adclt(z0.VnS(), z2.VnS(), z15.VnS()), "adclt z0.s, z2.s, z15.s");
  COMPARE(adclt(z0.VnD(), z2.VnD(), z15.VnD()), "adclt z0.d, z2.d, z15.d");
  COMPARE(sbclb(z17.VnS(), z10.VnS(), z8.VnS()), "sbclb z17.s, z10.s, z8.s");
  COMPARE(sbclb(z17.VnD(), z10.VnD(), z8.VnD()), "sbclb z17.d, z10.d, z8.d");
  COMPARE(sbclt(z20.VnS(), z0.VnS(), z13.VnS()), "sbclt z20.s, z0.s, z13.s");
  COMPARE(sbclt(z20.VnD(), z0.VnD(), z13.VnD()), "sbclt z20.d, z0.d, z13.d");

  COMPARE_MACRO(Adclb(z25.VnS(), z25.VnS(), z17.VnS(), z24.VnS()),
                "adclb z25.s, z17.s, z24.s");
  COMPARE_MACRO(Adclb(z25.VnS(), z20.VnS(), z17.VnS(), z24.VnS()),
                "movprfx z25, z20\n"
                "adclb z25.s, z17.s, z24.s");
  COMPARE_MACRO(Adclb(z25.VnS(), z20.VnS(), z25.VnS(), z24.VnS()),
                "movprfx z31, z20\n"
                "adclb z31.s, z25.s, z24.s\n"
                "mov z25.d, z31.d");
  COMPARE_MACRO(Adclb(z25.VnS(), z20.VnS(), z24.VnS(), z25.VnS()),
                "movprfx z31, z20\n"
                "adclb z31.s, z24.s, z25.s\n"
                "mov z25.d, z31.d");
  COMPARE_MACRO(Adclb(z25.VnS(), z20.VnS(), z25.VnS(), z25.VnS()),
                "movprfx z31, z20\n"
                "adclb z31.s, z25.s, z25.s\n"
                "mov z25.d, z31.d");
  COMPARE_MACRO(Adclt(z25.VnS(), z20.VnS(), z25.VnS(), z25.VnS()),
                "movprfx z31, z20\n"
                "adclt z31.s, z25.s, z25.s\n"
                "mov z25.d, z31.d");

  COMPARE_MACRO(Sbclb(z30.VnS(), z30.VnS(), z7.VnS(), z29.VnS()),
                "sbclb z30.s, z7.s, z29.s");
  COMPARE_MACRO(Sbclb(z30.VnS(), z2.VnS(), z7.VnS(), z29.VnS()),
                "movprfx z30, z2\n"
                "sbclb z30.s, z7.s, z29.s");
  COMPARE_MACRO(Sbclb(z30.VnS(), z2.VnS(), z30.VnS(), z29.VnS()),
                "movprfx z31, z2\n"
                "sbclb z31.s, z30.s, z29.s\n"
                "mov z30.d, z31.d");
  COMPARE_MACRO(Sbclb(z30.VnS(), z2.VnS(), z29.VnS(), z30.VnS()),
                "movprfx z31, z2\n"
                "sbclb z31.s, z29.s, z30.s\n"
                "mov z30.d, z31.d");
  COMPARE_MACRO(Sbclb(z30.VnS(), z2.VnS(), z30.VnS(), z30.VnS()),
                "movprfx z31, z2\n"
                "sbclb z31.s, z30.s, z30.s\n"
                "mov z30.d, z31.d");
  COMPARE_MACRO(Sbclt(z30.VnS(), z2.VnS(), z30.VnS(), z30.VnS()),
                "movprfx z31, z2\n"
                "sbclt z31.s, z30.s, z30.s\n"
                "mov z30.d, z31.d");
  CLEANUP();
}

TEST(sve2_add_sub_high) {
  SETUP();

  COMPARE_MACRO(Addhnb(z29.VnS(), z19.VnD(), z2.VnD()),
                "addhnb z29.s, z19.d, z2.d");
  COMPARE_MACRO(Addhnb(z29.VnB(), z19.VnH(), z2.VnH()),
                "addhnb z29.b, z19.h, z2.h");
  COMPARE_MACRO(Addhnb(z29.VnH(), z19.VnS(), z2.VnS()),
                "addhnb z29.h, z19.s, z2.s");
  COMPARE_MACRO(Addhnt(z8.VnS(), z12.VnD(), z6.VnD()),
                "addhnt z8.s, z12.d, z6.d");
  COMPARE_MACRO(Addhnt(z8.VnB(), z12.VnH(), z6.VnH()),
                "addhnt z8.b, z12.h, z6.h");
  COMPARE_MACRO(Addhnt(z8.VnH(), z12.VnS(), z6.VnS()),
                "addhnt z8.h, z12.s, z6.s");
  COMPARE_MACRO(Raddhnb(z0.VnS(), z11.VnD(), z10.VnD()),
                "raddhnb z0.s, z11.d, z10.d");
  COMPARE_MACRO(Raddhnb(z0.VnB(), z11.VnH(), z10.VnH()),
                "raddhnb z0.b, z11.h, z10.h");
  COMPARE_MACRO(Raddhnb(z0.VnH(), z11.VnS(), z10.VnS()),
                "raddhnb z0.h, z11.s, z10.s");
  COMPARE_MACRO(Raddhnt(z23.VnS(), z27.VnD(), z9.VnD()),
                "raddhnt z23.s, z27.d, z9.d");
  COMPARE_MACRO(Raddhnt(z23.VnB(), z27.VnH(), z9.VnH()),
                "raddhnt z23.b, z27.h, z9.h");
  COMPARE_MACRO(Raddhnt(z23.VnH(), z27.VnS(), z9.VnS()),
                "raddhnt z23.h, z27.s, z9.s");
  COMPARE_MACRO(Rsubhnb(z30.VnS(), z29.VnD(), z11.VnD()),
                "rsubhnb z30.s, z29.d, z11.d");
  COMPARE_MACRO(Rsubhnb(z30.VnB(), z29.VnH(), z11.VnH()),
                "rsubhnb z30.b, z29.h, z11.h");
  COMPARE_MACRO(Rsubhnb(z30.VnH(), z29.VnS(), z11.VnS()),
                "rsubhnb z30.h, z29.s, z11.s");
  COMPARE_MACRO(Rsubhnt(z25.VnS(), z7.VnD(), z18.VnD()),
                "rsubhnt z25.s, z7.d, z18.d");
  COMPARE_MACRO(Rsubhnt(z25.VnB(), z7.VnH(), z18.VnH()),
                "rsubhnt z25.b, z7.h, z18.h");
  COMPARE_MACRO(Rsubhnt(z25.VnH(), z7.VnS(), z18.VnS()),
                "rsubhnt z25.h, z7.s, z18.s");
  COMPARE_MACRO(Subhnb(z31.VnS(), z31.VnD(), z7.VnD()),
                "subhnb z31.s, z31.d, z7.d");
  COMPARE_MACRO(Subhnb(z31.VnB(), z31.VnH(), z7.VnH()),
                "subhnb z31.b, z31.h, z7.h");
  COMPARE_MACRO(Subhnb(z31.VnH(), z31.VnS(), z7.VnS()),
                "subhnb z31.h, z31.s, z7.s");
  COMPARE_MACRO(Subhnt(z31.VnS(), z22.VnD(), z27.VnD()),
                "subhnt z31.s, z22.d, z27.d");
  COMPARE_MACRO(Subhnt(z31.VnB(), z22.VnH(), z27.VnH()),
                "subhnt z31.b, z22.h, z27.h");
  COMPARE_MACRO(Subhnt(z31.VnH(), z22.VnS(), z27.VnS()),
                "subhnt z31.h, z22.s, z27.s");

  CLEANUP();
}

TEST(sve2_complex_addition) {
  SETUP();

  COMPARE_MACRO(Cadd(z5.VnB(), z5.VnB(), z12.VnB(), 90),
                "cadd z5.b, z5.b, z12.b, #90");
  COMPARE_MACRO(Cadd(z5.VnD(), z5.VnD(), z12.VnD(), 90),
                "cadd z5.d, z5.d, z12.d, #90");
  COMPARE_MACRO(Cadd(z5.VnH(), z5.VnH(), z12.VnH(), 90),
                "cadd z5.h, z5.h, z12.h, #90");
  COMPARE_MACRO(Cadd(z5.VnS(), z5.VnS(), z12.VnS(), 90),
                "cadd z5.s, z5.s, z12.s, #90");
  COMPARE_MACRO(Cadd(z5.VnB(), z5.VnB(), z12.VnB(), 270),
                "cadd z5.b, z5.b, z12.b, #270");
  COMPARE_MACRO(Cadd(z5.VnD(), z5.VnD(), z12.VnD(), 270),
                "cadd z5.d, z5.d, z12.d, #270");
  COMPARE_MACRO(Cadd(z5.VnH(), z5.VnH(), z12.VnH(), 270),
                "cadd z5.h, z5.h, z12.h, #270");
  COMPARE_MACRO(Cadd(z5.VnS(), z5.VnS(), z12.VnS(), 270),
                "cadd z5.s, z5.s, z12.s, #270");
  COMPARE_MACRO(Cadd(z5.VnS(), z6.VnS(), z12.VnS(), 270),
                "movprfx z5, z6\n"
                "cadd z5.s, z5.s, z12.s, #270");
  COMPARE_MACRO(Cadd(z5.VnS(), z6.VnS(), z5.VnS(), 270),
                "mov z31.d, z5.d\n"
                "movprfx z5, z6\n"
                "cadd z5.s, z5.s, z31.s, #270");

  COMPARE_MACRO(Sqcadd(z20.VnB(), z20.VnB(), z23.VnB(), 90),
                "sqcadd z20.b, z20.b, z23.b, #90");
  COMPARE_MACRO(Sqcadd(z20.VnD(), z20.VnD(), z23.VnD(), 90),
                "sqcadd z20.d, z20.d, z23.d, #90");
  COMPARE_MACRO(Sqcadd(z20.VnH(), z20.VnH(), z23.VnH(), 90),
                "sqcadd z20.h, z20.h, z23.h, #90");
  COMPARE_MACRO(Sqcadd(z20.VnB(), z20.VnB(), z23.VnB(), 270),
                "sqcadd z20.b, z20.b, z23.b, #270");
  COMPARE_MACRO(Sqcadd(z20.VnD(), z20.VnD(), z23.VnD(), 270),
                "sqcadd z20.d, z20.d, z23.d, #270");
  COMPARE_MACRO(Sqcadd(z20.VnH(), z20.VnH(), z23.VnH(), 270),
                "sqcadd z20.h, z20.h, z23.h, #270");
  COMPARE_MACRO(Sqcadd(z20.VnS(), z20.VnS(), z23.VnS(), 270),
                "sqcadd z20.s, z20.s, z23.s, #270");
  COMPARE_MACRO(Sqcadd(z20.VnH(), z21.VnH(), z23.VnH(), 270),
                "movprfx z20, z21\n"
                "sqcadd z20.h, z20.h, z23.h, #270");
  COMPARE_MACRO(Sqcadd(z20.VnH(), z21.VnH(), z20.VnH(), 270),
                "mov z31.d, z20.d\n"
                "movprfx z20, z21\n"
                "sqcadd z20.h, z20.h, z31.h, #270");

  CLEANUP();
}

TEST(sve2_bit_permute) {
  SETUP();

  COMPARE_MACRO(Bdep(z18.VnB(), z10.VnB(), z0.VnB()),
                "bdep z18.b, z10.b, z0.b");
  COMPARE_MACRO(Bdep(z18.VnD(), z10.VnD(), z0.VnD()),
                "bdep z18.d, z10.d, z0.d");
  COMPARE_MACRO(Bdep(z18.VnH(), z10.VnH(), z0.VnH()),
                "bdep z18.h, z10.h, z0.h");
  COMPARE_MACRO(Bdep(z18.VnS(), z10.VnS(), z0.VnS()),
                "bdep z18.s, z10.s, z0.s");
  COMPARE_MACRO(Bext(z6.VnB(), z2.VnB(), z5.VnB()), "bext z6.b, z2.b, z5.b");
  COMPARE_MACRO(Bext(z6.VnD(), z2.VnD(), z5.VnD()), "bext z6.d, z2.d, z5.d");
  COMPARE_MACRO(Bext(z6.VnH(), z2.VnH(), z5.VnH()), "bext z6.h, z2.h, z5.h");
  COMPARE_MACRO(Bext(z6.VnS(), z2.VnS(), z5.VnS()), "bext z6.s, z2.s, z5.s");
  COMPARE_MACRO(Bgrp(z24.VnB(), z9.VnB(), z5.VnB()), "bgrp z24.b, z9.b, z5.b");
  COMPARE_MACRO(Bgrp(z24.VnD(), z9.VnD(), z5.VnD()), "bgrp z24.d, z9.d, z5.d");
  COMPARE_MACRO(Bgrp(z24.VnH(), z9.VnH(), z5.VnH()), "bgrp z24.h, z9.h, z5.h");
  COMPARE_MACRO(Bgrp(z24.VnS(), z9.VnS(), z5.VnS()), "bgrp z24.s, z9.s, z5.s");

  CLEANUP();
}

TEST(sve2_integer_multiply_long_vector) {
  SETUP();

  COMPARE(sqdmullb(z1.VnD(), z31.VnS(), z21.VnS()),
          "sqdmullb z1.d, z31.s, z21.s");
  COMPARE(sqdmullb(z2.VnH(), z30.VnB(), z22.VnB()),
          "sqdmullb z2.h, z30.b, z22.b");
  COMPARE(sqdmullb(z3.VnS(), z29.VnH(), z23.VnH()),
          "sqdmullb z3.s, z29.h, z23.h");
  COMPARE(sqdmullb(z1.VnS(), z27.VnH(), z3.VnH(), 7),
          "sqdmullb z1.s, z27.h, z3.h[7]");
  COMPARE(sqdmullb(z27.VnD(), z16.VnS(), z5.VnS(), 3),
          "sqdmullb z27.d, z16.s, z5.s[3]");

  COMPARE(sqdmullt(z2.VnD(), z1.VnS(), z5.VnS()), "sqdmullt z2.d, z1.s, z5.s");
  COMPARE(sqdmullt(z12.VnH(), z11.VnB(), z15.VnB()),
          "sqdmullt z12.h, z11.b, z15.b");
  COMPARE(sqdmullt(z20.VnS(), z21.VnH(), z25.VnH()),
          "sqdmullt z20.s, z21.h, z25.h");
  COMPARE(sqdmullt(z23.VnS(), z28.VnH(), z2.VnH(), 0),
          "sqdmullt z23.s, z28.h, z2.h[0]");
  COMPARE(sqdmullt(z7.VnD(), z4.VnS(), z0.VnS(), 0),
          "sqdmullt z7.d, z4.s, z0.s[0]");

  // Feature `SVEPmull128` is not supported.
  // COMPARE(pmullb(z12.VnQ(), z21.VnD(), z12.VnD()),
  //                "pmullb z12.q, z21.d, z12.d");
  COMPARE(pmullb(z12.VnH(), z21.VnB(), z12.VnB()),
          "pmullb z12.h, z21.b, z12.b");
  COMPARE(pmullt(z31.VnD(), z30.VnS(), z26.VnS()),
          "pmullt z31.d, z30.s, z26.s");

  COMPARE(smullb(z10.VnD(), z4.VnS(), z4.VnS()), "smullb z10.d, z4.s, z4.s");
  COMPARE(smullb(z11.VnH(), z14.VnB(), z14.VnB()),
          "smullb z11.h, z14.b, z14.b");
  COMPARE(smullb(z12.VnS(), z24.VnH(), z24.VnH()),
          "smullb z12.s, z24.h, z24.h");

  COMPARE(smullt(z31.VnD(), z26.VnS(), z5.VnS()), "smullt z31.d, z26.s, z5.s");
  COMPARE(smullt(z21.VnH(), z16.VnB(), z5.VnB()), "smullt z21.h, z16.b, z5.b");
  COMPARE(smullt(z11.VnS(), z6.VnH(), z5.VnH()), "smullt z11.s, z6.h, z5.h");

  COMPARE(umullb(z12.VnD(), z5.VnS(), z2.VnS()), "umullb z12.d, z5.s, z2.s");
  COMPARE(umullb(z12.VnH(), z15.VnB(), z12.VnB()),
          "umullb z12.h, z15.b, z12.b");
  COMPARE(umullb(z12.VnS(), z25.VnH(), z22.VnH()),
          "umullb z12.s, z25.h, z22.h");

  COMPARE(umullt(z24.VnD(), z6.VnS(), z6.VnS()), "umullt z24.d, z6.s, z6.s");
  COMPARE(umullt(z24.VnH(), z7.VnB(), z16.VnB()), "umullt z24.h, z7.b, z16.b");
  COMPARE(umullt(z24.VnS(), z8.VnH(), z26.VnH()), "umullt z24.s, z8.h, z26.h");

  CLEANUP();
}

TEST(sve2_xar) {
  SETUP();

  COMPARE_MACRO(Xar(z16.VnB(), z16.VnB(), z13.VnB(), 1),
                "xar z16.b, z16.b, z13.b, #1");
  COMPARE_MACRO(Xar(z16.VnB(), z16.VnB(), z13.VnB(), 8),
                "xar z16.b, z16.b, z13.b, #8");
  COMPARE_MACRO(Xar(z16.VnH(), z16.VnH(), z13.VnH(), 1),
                "xar z16.h, z16.h, z13.h, #1");
  COMPARE_MACRO(Xar(z16.VnH(), z16.VnH(), z13.VnH(), 16),
                "xar z16.h, z16.h, z13.h, #16");
  COMPARE_MACRO(Xar(z16.VnS(), z16.VnS(), z13.VnS(), 1),
                "xar z16.s, z16.s, z13.s, #1");
  COMPARE_MACRO(Xar(z16.VnS(), z16.VnS(), z13.VnS(), 32),
                "xar z16.s, z16.s, z13.s, #32");
  COMPARE_MACRO(Xar(z16.VnD(), z16.VnD(), z13.VnD(), 1),
                "xar z16.d, z16.d, z13.d, #1");
  COMPARE_MACRO(Xar(z16.VnD(), z16.VnD(), z13.VnD(), 64),
                "xar z16.d, z16.d, z13.d, #64");

  COMPARE_MACRO(Xar(z16.VnD(), z13.VnD(), z16.VnD(), 64),
                "xar z16.d, z16.d, z13.d, #64");
  COMPARE_MACRO(Xar(z16.VnD(), z13.VnD(), z12.VnD(), 64),
                "movprfx z16, z13\n"
                "xar z16.d, z16.d, z12.d, #64");
  COMPARE_MACRO(Xar(z16.VnD(), z16.VnD(), z16.VnD(), 64),
                "xar z16.d, z16.d, z16.d, #64");

  CLEANUP();
}

TEST(sve2_histogram) {
  SETUP();

  COMPARE_MACRO(Histcnt(z24.VnS(), p6.Zeroing(), z3.VnS(), z10.VnS()),
                "histcnt z24.s, p6/z, z3.s, z10.s");
  COMPARE_MACRO(Histcnt(z24.VnD(), p6.Zeroing(), z3.VnD(), z10.VnD()),
                "histcnt z24.d, p6/z, z3.d, z10.d");
  COMPARE_MACRO(Histseg(z22.VnB(), z14.VnB(), z8.VnB()),
                "histseg z22.b, z14.b, z8.b");

  CLEANUP();
}

TEST(sve2_table) {
  SETUP();

  COMPARE_MACRO(Tbl(z17.VnB(), z1.VnB(), z2.VnB(), z22.VnB()),
                "tbl z17.b, {z3.b, z4.b}, z22.b");
  COMPARE_MACRO(Tbl(z17.VnD(), z1.VnD(), z2.VnD(), z22.VnD()),
                "tbl z17.d, {z3.d, z4.d}, z22.d");
  COMPARE_MACRO(Tbl(z17.VnH(), z1.VnH(), z2.VnH(), z22.VnH()),
                "tbl z17.h, {z3.h, z4.h}, z22.h");
  COMPARE_MACRO(Tbl(z17.VnS(), z31.VnS(), z0.VnS(), z22.VnS()),
                "tbl z17.s, {z31.s, z0.s}, z22.s");
  COMPARE_MACRO(Tbx(z22.VnB(), z15.VnB(), z19.VnB()),
                "tbx z22.b, z15.b, z19.b");
  COMPARE_MACRO(Tbx(z22.VnD(), z15.VnD(), z19.VnD()),
                "tbx z22.d, z15.d, z19.d");
  COMPARE_MACRO(Tbx(z22.VnH(), z15.VnH(), z19.VnH()),
                "tbx z22.h, z15.h, z19.h");
  COMPARE_MACRO(Tbx(z22.VnS(), z15.VnS(), z19.VnS()),
                "tbx z22.s, z15.s, z19.s");

  CLEANUP();
}

TEST(sve2_cdot) {
  SETUP();

  COMPARE_MACRO(Cdot(z7.VnS(), z7.VnS(), z4.VnB(), z10.VnB(), 0),
                "cdot z7.s, z4.b, z10.b, #0");
  COMPARE_MACRO(Cdot(z7.VnD(), z7.VnD(), z4.VnH(), z10.VnH(), 0),
                "cdot z7.d, z4.h, z10.h, #0");
  COMPARE_MACRO(Cdot(z7.VnS(), z7.VnS(), z4.VnB(), z10.VnB(), 90),
                "cdot z7.s, z4.b, z10.b, #90");
  COMPARE_MACRO(Cdot(z7.VnD(), z7.VnD(), z4.VnH(), z10.VnH(), 90),
                "cdot z7.d, z4.h, z10.h, #90");
  COMPARE_MACRO(Cdot(z7.VnS(), z7.VnS(), z4.VnB(), z10.VnB(), 180),
                "cdot z7.s, z4.b, z10.b, #180");
  COMPARE_MACRO(Cdot(z7.VnD(), z7.VnD(), z4.VnH(), z10.VnH(), 180),
                "cdot z7.d, z4.h, z10.h, #180");
  COMPARE_MACRO(Cdot(z7.VnS(), z7.VnS(), z4.VnB(), z10.VnB(), 270),
                "cdot z7.s, z4.b, z10.b, #270");
  COMPARE_MACRO(Cdot(z7.VnD(), z7.VnD(), z4.VnH(), z10.VnH(), 270),
                "cdot z7.d, z4.h, z10.h, #270");

  COMPARE_MACRO(Cdot(z0.VnS(), z1.VnS(), z2.VnB(), z3.VnB(), 0),
                "movprfx z0, z1\n"
                "cdot z0.s, z2.b, z3.b, #0");
  COMPARE_MACRO(Cdot(z0.VnS(), z1.VnS(), z0.VnB(), z3.VnB(), 0),
                "mov z31.d, z0.d\n"
                "movprfx z0, z1\n"
                "cdot z0.s, z31.b, z3.b, #0");
  COMPARE_MACRO(Cdot(z0.VnS(), z1.VnS(), z2.VnB(), z0.VnB(), 0),
                "mov z31.d, z0.d\n"
                "movprfx z0, z1\n"
                "cdot z0.s, z2.b, z31.b, #0");
  COMPARE_MACRO(Cdot(z0.VnS(), z1.VnS(), z0.VnB(), z0.VnB(), 0),
                "mov z31.d, z0.d\n"
                "movprfx z0, z1\n"
                "cdot z0.s, z31.b, z31.b, #0");

  COMPARE_MACRO(Cdot(z18.VnS(), z18.VnS(), z26.VnB(), z7.VnB(), 0, 0),
                "cdot z18.s, z26.b, z7.b[0], #0");
  COMPARE_MACRO(Cdot(z18.VnS(), z18.VnS(), z26.VnB(), z7.VnB(), 1, 0),
                "cdot z18.s, z26.b, z7.b[1], #0");
  COMPARE_MACRO(Cdot(z18.VnS(), z18.VnS(), z26.VnB(), z7.VnB(), 2, 0),
                "cdot z18.s, z26.b, z7.b[2], #0");
  COMPARE_MACRO(Cdot(z18.VnS(), z18.VnS(), z26.VnB(), z7.VnB(), 3, 0),
                "cdot z18.s, z26.b, z7.b[3], #0");
  COMPARE_MACRO(Cdot(z18.VnS(), z18.VnS(), z26.VnB(), z7.VnB(), 2, 90),
                "cdot z18.s, z26.b, z7.b[2], #90");
  COMPARE_MACRO(Cdot(z18.VnS(), z18.VnS(), z26.VnB(), z7.VnB(), 2, 180),
                "cdot z18.s, z26.b, z7.b[2], #180");
  COMPARE_MACRO(Cdot(z18.VnS(), z18.VnS(), z26.VnB(), z7.VnB(), 2, 270),
                "cdot z18.s, z26.b, z7.b[2], #270");
  COMPARE_MACRO(Cdot(z5.VnD(), z5.VnD(), z7.VnH(), z1.VnH(), 0, 0),
                "cdot z5.d, z7.h, z1.h[0], #0");
  COMPARE_MACRO(Cdot(z5.VnD(), z5.VnD(), z7.VnH(), z1.VnH(), 1, 0),
                "cdot z5.d, z7.h, z1.h[1], #0");
  COMPARE_MACRO(Cdot(z5.VnD(), z5.VnD(), z7.VnH(), z1.VnH(), 1, 90),
                "cdot z5.d, z7.h, z1.h[1], #90");
  COMPARE_MACRO(Cdot(z5.VnD(), z5.VnD(), z7.VnH(), z1.VnH(), 1, 180),
                "cdot z5.d, z7.h, z1.h[1], #180");
  COMPARE_MACRO(Cdot(z5.VnD(), z5.VnD(), z7.VnH(), z1.VnH(), 1, 270),
                "cdot z5.d, z7.h, z1.h[1], #270");

  COMPARE_MACRO(Cdot(z0.VnS(), z1.VnS(), z2.VnB(), z3.VnB(), 0, 0),
                "movprfx z0, z1\n"
                "cdot z0.s, z2.b, z3.b[0], #0");
  COMPARE_MACRO(Cdot(z0.VnS(), z1.VnS(), z0.VnB(), z3.VnB(), 1, 90),
                "movprfx z31, z1\n"
                "cdot z31.s, z0.b, z3.b[1], #90\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Cdot(z0.VnS(), z1.VnS(), z2.VnB(), z0.VnB(), 2, 180),
                "movprfx z31, z1\n"
                "cdot z31.s, z2.b, z0.b[2], #180\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Cdot(z0.VnS(), z1.VnS(), z0.VnB(), z0.VnB(), 3, 270),
                "movprfx z31, z1\n"
                "cdot z31.s, z0.b, z0.b[3], #270\n"
                "mov z0.d, z31.d");

  CLEANUP();
}

TEST(sve2_ldnt1) {
  SETUP();

  COMPARE_MACRO(Ldnt1b(z24.VnS(), p4.Zeroing(), SVEMemOperand(z18.VnS(), x13)),
                "ldnt1b {z24.s}, p4/z, [z18.s, x13]");
  COMPARE_MACRO(Ldnt1h(z3.VnS(), p4.Zeroing(), SVEMemOperand(z15.VnS(), x14)),
                "ldnt1h {z3.s}, p4/z, [z15.s, x14]");
  COMPARE_MACRO(Ldnt1sb(z7.VnS(), p3.Zeroing(), SVEMemOperand(z18.VnS(), x11)),
                "ldnt1sb {z7.s}, p3/z, [z18.s, x11]");
  COMPARE_MACRO(Ldnt1sh(z17.VnS(), p5.Zeroing(), SVEMemOperand(z31.VnS(), x19)),
                "ldnt1sh {z17.s}, p5/z, [z31.s, x19]");
  COMPARE_MACRO(Ldnt1w(z18.VnS(), p5.Zeroing(), SVEMemOperand(z9.VnS(), x17)),
                "ldnt1w {z18.s}, p5/z, [z9.s, x17]");

  COMPARE_MACRO(Ldnt1b(z27.VnD(), p4.Zeroing(), SVEMemOperand(z27.VnD(), x24)),
                "ldnt1b {z27.d}, p4/z, [z27.d, x24]");
  COMPARE_MACRO(Ldnt1d(z25.VnD(), p0.Zeroing(), SVEMemOperand(z10.VnD(), x0)),
                "ldnt1d {z25.d}, p0/z, [z10.d, x0]");
  COMPARE_MACRO(Ldnt1h(z16.VnD(), p2.Zeroing(), SVEMemOperand(z10.VnD(), x9)),
                "ldnt1h {z16.d}, p2/z, [z10.d, x9]");
  COMPARE_MACRO(Ldnt1sb(z25.VnD(), p0.Zeroing(), SVEMemOperand(z0.VnD(), x3)),
                "ldnt1sb {z25.d}, p0/z, [z0.d, x3]");
  COMPARE_MACRO(Ldnt1sh(z4.VnD(), p1.Zeroing(), SVEMemOperand(z31.VnD(), x4)),
                "ldnt1sh {z4.d}, p1/z, [z31.d, x4]");
  COMPARE_MACRO(Ldnt1sw(z3.VnD(), p7.Zeroing(), SVEMemOperand(z1.VnD(), x10)),
                "ldnt1sw {z3.d}, p7/z, [z1.d, x10]");
  COMPARE_MACRO(Ldnt1w(z17.VnD(), p5.Zeroing(), SVEMemOperand(z8.VnD(), x12)),
                "ldnt1w {z17.d}, p5/z, [z8.d, x12]");

  CLEANUP();
}

TEST(sve2_stnt1) {
  SETUP();

  COMPARE_MACRO(Stnt1b(z29.VnD(), p7, SVEMemOperand(z29.VnD(), x21)),
                "stnt1b {z29.d}, p7, [z29.d, x21]");
  COMPARE_MACRO(Stnt1d(z19.VnD(), p4, SVEMemOperand(z3.VnD(), x16)),
                "stnt1d {z19.d}, p4, [z3.d, x16]");
  COMPARE_MACRO(Stnt1h(z11.VnS(), p3, SVEMemOperand(z2.VnS(), x16)),
                "stnt1h {z11.s}, p3, [z2.s, x16]");
  COMPARE_MACRO(Stnt1h(z3.VnD(), p3, SVEMemOperand(z10.VnD(), x16)),
                "stnt1h {z3.d}, p3, [z10.d, x16]");
  COMPARE_MACRO(Stnt1w(z11.VnS(), p4, SVEMemOperand(z14.VnS(), x15)),
                "stnt1w {z11.s}, p4, [z14.s, x15]");
  COMPARE_MACRO(Stnt1w(z7.VnD(), p0, SVEMemOperand(z11.VnD(), x10)),
                "stnt1w {z7.d}, p0, [z11.d, x10]");

  CLEANUP();
}

TEST(sve2_bitwise_ternary) {
  SETUP();

  COMPARE_MACRO(Bcax(z6.VnD(), z6.VnD(), z12.VnD(), z1.VnD()),
                "bcax z6.d, z6.d, z12.d, z1.d");
  COMPARE_MACRO(Bsl(z21.VnD(), z21.VnD(), z2.VnD(), z2.VnD()),
                "bsl z21.d, z21.d, z2.d, z2.d");
  COMPARE_MACRO(Bsl1n(z18.VnD(), z18.VnD(), z8.VnD(), z7.VnD()),
                "bsl1n z18.d, z18.d, z8.d, z7.d");
  COMPARE_MACRO(Bsl2n(z7.VnD(), z7.VnD(), z3.VnD(), z19.VnD()),
                "bsl2n z7.d, z7.d, z3.d, z19.d");
  COMPARE_MACRO(Eor3(z10.VnD(), z10.VnD(), z24.VnD(), z23.VnD()),
                "eor3 z10.d, z10.d, z24.d, z23.d");
  COMPARE_MACRO(Nbsl(z17.VnD(), z17.VnD(), z21.VnD(), z27.VnD()),
                "nbsl z17.d, z17.d, z21.d, z27.d");

  COMPARE_MACRO(Nbsl(z17.VnD(), z18.VnD(), z21.VnD(), z27.VnD()),
                "movprfx z17, z18\n"
                "nbsl z17.d, z17.d, z21.d, z27.d");
  COMPARE_MACRO(Nbsl(z17.VnD(), z18.VnD(), z17.VnD(), z27.VnD()),
                "movprfx z31, z18\n"
                "nbsl z31.d, z31.d, z17.d, z27.d\n"
                "mov z17.d, z31.d");
  COMPARE_MACRO(Nbsl(z17.VnD(), z18.VnD(), z21.VnD(), z17.VnD()),
                "movprfx z31, z18\n"
                "nbsl z31.d, z31.d, z21.d, z17.d\n"
                "mov z17.d, z31.d");
  COMPARE_MACRO(Nbsl(z17.VnD(), z18.VnD(), z17.VnD(), z17.VnD()),
                "movprfx z31, z18\n"
                "nbsl z31.d, z31.d, z17.d, z17.d\n"
                "mov z17.d, z31.d");

  CLEANUP();
}

TEST(sve2_int_compare_scalars) {
  SETUP();

  COMPARE_MACRO(Whilege(p0.VnB(), w20, w29), "whilege p0.b, w20, w29");
  COMPARE_MACRO(Whilege(p0.VnB(), x20, x29), "whilege p0.b, x20, x29");
  COMPARE_MACRO(Whilege(p0.VnD(), w20, w29), "whilege p0.d, w20, w29");
  COMPARE_MACRO(Whilege(p0.VnD(), x20, x29), "whilege p0.d, x20, x29");
  COMPARE_MACRO(Whilege(p0.VnH(), w20, w29), "whilege p0.h, w20, w29");
  COMPARE_MACRO(Whilege(p0.VnH(), x20, x29), "whilege p0.h, x20, x29");
  COMPARE_MACRO(Whilege(p0.VnS(), w20, w29), "whilege p0.s, w20, w29");
  COMPARE_MACRO(Whilege(p0.VnS(), x20, x29), "whilege p0.s, x20, x29");
  COMPARE_MACRO(Whilegt(p11.VnB(), w24, w3), "whilegt p11.b, w24, w3");
  COMPARE_MACRO(Whilegt(p11.VnD(), w24, w3), "whilegt p11.d, w24, w3");
  COMPARE_MACRO(Whilegt(p11.VnH(), x24, x3), "whilegt p11.h, x24, x3");
  COMPARE_MACRO(Whilegt(p11.VnS(), x24, x3), "whilegt p11.s, x24, x3");
  COMPARE_MACRO(Whilehi(p2.VnB(), x20, x8), "whilehi p2.b, x20, x8");
  COMPARE_MACRO(Whilehi(p2.VnD(), x20, x8), "whilehi p2.d, x20, x8");
  COMPARE_MACRO(Whilehi(p2.VnH(), w20, w8), "whilehi p2.h, w20, w8");
  COMPARE_MACRO(Whilehi(p2.VnS(), w20, w8), "whilehi p2.s, w20, w8");
  COMPARE_MACRO(Whilehs(p4.VnB(), w22, w9), "whilehs p4.b, w22, w9");
  COMPARE_MACRO(Whilehs(p4.VnD(), x22, x9), "whilehs p4.d, x22, x9");
  COMPARE_MACRO(Whilehs(p4.VnH(), w22, w9), "whilehs p4.h, w22, w9");
  COMPARE_MACRO(Whilehs(p4.VnS(), x22, x9), "whilehs p4.s, x22, x9");

  COMPARE_MACRO(Whilerw(p7.VnB(), x25, x27), "whilerw p7.b, x25, x27");
  COMPARE_MACRO(Whilerw(p7.VnD(), x25, x28), "whilerw p7.d, x25, x28");
  COMPARE_MACRO(Whilerw(p7.VnH(), x25, x29), "whilerw p7.h, x25, x29");
  COMPARE_MACRO(Whilerw(p7.VnS(), x25, x30), "whilerw p7.s, x25, x30");
  COMPARE_MACRO(Whilerw(p7.VnS(), x25, xzr), "whilerw p7.s, x25, xzr");
  COMPARE_MACRO(Whilewr(p8.VnB(), x14, x14), "whilewr p8.b, x14, x14");
  COMPARE_MACRO(Whilewr(p8.VnD(), x14, x13), "whilewr p8.d, x14, x13");
  COMPARE_MACRO(Whilewr(p8.VnH(), x14, x12), "whilewr p8.h, x14, x12");
  COMPARE_MACRO(Whilewr(p8.VnS(), x14, x11), "whilewr p8.s, x14, x11");
  COMPARE_MACRO(Whilewr(p8.VnS(), xzr, x11), "whilewr p8.s, xzr, x11");

  CLEANUP();
}

TEST(sve2_splice) {
  SETUP();

  COMPARE_MACRO(Splice(z31.VnB(), p0, z21.VnB(), z22.VnB()),
                "splice z31.b, p0, {z21.b, z22.b}");
  COMPARE_MACRO(Splice(z31.VnD(), p0, z21.VnD(), z22.VnD()),
                "splice z31.d, p0, {z21.d, z22.d}");
  COMPARE_MACRO(Splice(z31.VnH(), p0, z21.VnH(), z22.VnH()),
                "splice z31.h, p0, {z21.h, z22.h}");
  COMPARE_MACRO(Splice(z31.VnS(), p0, z31.VnS(), z0.VnS()),
                "splice z31.s, p0, z31.s, z0.s");
  COMPARE_MACRO(Splice(z30.VnS(), p0, z31.VnS(), z0.VnS()),
                "splice z30.s, p0, {z31.s, z0.s}");

  CLEANUP();
}

TEST(sve2_mul_index) {
  SETUP();

  COMPARE_MACRO(Mul(z18.VnH(), z5.VnH(), z7.VnH(), 0),
                "mul z18.h, z5.h, z7.h[0]");
  COMPARE_MACRO(Mul(z18.VnH(), z5.VnH(), z2.VnH(), 2),
                "mul z18.h, z5.h, z2.h[2]");
  COMPARE_MACRO(Mul(z18.VnH(), z5.VnH(), z2.VnH(), 6),
                "mul z18.h, z5.h, z2.h[6]");
  COMPARE_MACRO(Mul(z18.VnH(), z5.VnH(), z2.VnH(), 7),
                "mul z18.h, z5.h, z2.h[7]");
  COMPARE_MACRO(Mul(z8.VnS(), z15.VnS(), z7.VnS(), 0),
                "mul z8.s, z15.s, z7.s[0]");
  COMPARE_MACRO(Mul(z8.VnS(), z15.VnS(), z0.VnS(), 3),
                "mul z8.s, z15.s, z0.s[3]");
  COMPARE_MACRO(Mul(z8.VnD(), z15.VnD(), z15.VnD(), 0),
                "mul z8.d, z15.d, z15.d[0]");
  COMPARE_MACRO(Mul(z8.VnD(), z15.VnD(), z0.VnD(), 1),
                "mul z8.d, z15.d, z0.d[1]");

  CLEANUP();
}

TEST(sve2_mla_mls_index) {
  SETUP();

  COMPARE_MACRO(Mla(z1.VnH(), z1.VnH(), z9.VnH(), z0.VnH(), 0),
                "mla z1.h, z9.h, z0.h[0]");
  COMPARE_MACRO(Mla(z1.VnH(), z1.VnH(), z9.VnH(), z1.VnH(), 2),
                "mla z1.h, z9.h, z1.h[2]");
  COMPARE_MACRO(Mla(z1.VnH(), z1.VnH(), z9.VnH(), z2.VnH(), 6),
                "mla z1.h, z9.h, z2.h[6]");
  COMPARE_MACRO(Mla(z1.VnH(), z1.VnH(), z9.VnH(), z3.VnH(), 7),
                "mla z1.h, z9.h, z3.h[7]");
  COMPARE_MACRO(Mla(z10.VnS(), z10.VnS(), z22.VnS(), z7.VnS(), 0),
                "mla z10.s, z22.s, z7.s[0]");
  COMPARE_MACRO(Mla(z10.VnS(), z10.VnS(), z22.VnS(), z0.VnS(), 3),
                "mla z10.s, z22.s, z0.s[3]");
  COMPARE_MACRO(Mla(z4.VnD(), z4.VnD(), z0.VnD(), z15.VnD(), 0),
                "mla z4.d, z0.d, z15.d[0]");
  COMPARE_MACRO(Mla(z4.VnD(), z4.VnD(), z0.VnD(), z0.VnD(), 1),
                "mla z4.d, z0.d, z0.d[1]");

  COMPARE_MACRO(Mla(z4.VnH(), z5.VnH(), z0.VnH(), z1.VnH(), 0),
                "movprfx z4, z5\n"
                "mla z4.h, z0.h, z1.h[0]");
  COMPARE_MACRO(Mla(z4.VnH(), z5.VnH(), z4.VnH(), z1.VnH(), 0),
                "movprfx z31, z5\n"
                "mla z31.h, z4.h, z1.h[0]\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Mla(z4.VnH(), z5.VnH(), z0.VnH(), z4.VnH(), 0),
                "movprfx z31, z5\n"
                "mla z31.h, z0.h, z4.h[0]\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Mla(z4.VnH(), z5.VnH(), z4.VnH(), z4.VnH(), 0),
                "movprfx z31, z5\n"
                "mla z31.h, z4.h, z4.h[0]\n"
                "mov z4.d, z31.d");

  COMPARE_MACRO(Mls(z1.VnH(), z1.VnH(), z9.VnH(), z0.VnH(), 0),
                "mls z1.h, z9.h, z0.h[0]");
  COMPARE_MACRO(Mls(z1.VnH(), z1.VnH(), z9.VnH(), z1.VnH(), 2),
                "mls z1.h, z9.h, z1.h[2]");
  COMPARE_MACRO(Mls(z1.VnH(), z1.VnH(), z9.VnH(), z2.VnH(), 6),
                "mls z1.h, z9.h, z2.h[6]");
  COMPARE_MACRO(Mls(z1.VnH(), z1.VnH(), z9.VnH(), z3.VnH(), 7),
                "mls z1.h, z9.h, z3.h[7]");
  COMPARE_MACRO(Mls(z10.VnS(), z10.VnS(), z22.VnS(), z7.VnS(), 0),
                "mls z10.s, z22.s, z7.s[0]");
  COMPARE_MACRO(Mls(z10.VnS(), z10.VnS(), z22.VnS(), z0.VnS(), 3),
                "mls z10.s, z22.s, z0.s[3]");
  COMPARE_MACRO(Mls(z4.VnD(), z4.VnD(), z0.VnD(), z15.VnD(), 0),
                "mls z4.d, z0.d, z15.d[0]");
  COMPARE_MACRO(Mls(z4.VnD(), z4.VnD(), z0.VnD(), z0.VnD(), 1),
                "mls z4.d, z0.d, z0.d[1]");

  COMPARE_MACRO(Mls(z4.VnS(), z5.VnS(), z0.VnS(), z1.VnS(), 0),
                "movprfx z4, z5\n"
                "mls z4.s, z0.s, z1.s[0]");
  COMPARE_MACRO(Mls(z4.VnS(), z5.VnS(), z4.VnS(), z1.VnS(), 0),
                "movprfx z31, z5\n"
                "mls z31.s, z4.s, z1.s[0]\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Mls(z4.VnS(), z5.VnS(), z0.VnS(), z4.VnS(), 0),
                "movprfx z31, z5\n"
                "mls z31.s, z0.s, z4.s[0]\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Mls(z4.VnS(), z5.VnS(), z4.VnS(), z4.VnS(), 0),
                "movprfx z31, z5\n"
                "mls z31.s, z4.s, z4.s[0]\n"
                "mov z4.d, z31.d");

  CLEANUP();
}

TEST(sve2_mla_long) {
  SETUP();

  COMPARE_MACRO(Smlalb(z1.VnD(), z1.VnD(), z3.VnS(), z23.VnS()),
                "smlalb z1.d, z3.s, z23.s");
  COMPARE_MACRO(Smlalb(z1.VnH(), z1.VnH(), z3.VnB(), z23.VnB()),
                "smlalb z1.h, z3.b, z23.b");
  COMPARE_MACRO(Smlalb(z1.VnS(), z1.VnS(), z3.VnH(), z23.VnH()),
                "smlalb z1.s, z3.h, z23.h");
  COMPARE_MACRO(Smlalt(z31.VnD(), z31.VnD(), z24.VnS(), z29.VnS()),
                "smlalt z31.d, z24.s, z29.s");
  COMPARE_MACRO(Smlalt(z31.VnH(), z31.VnH(), z24.VnB(), z29.VnB()),
                "smlalt z31.h, z24.b, z29.b");
  COMPARE_MACRO(Smlalt(z31.VnS(), z31.VnS(), z24.VnH(), z29.VnH()),
                "smlalt z31.s, z24.h, z29.h");
  COMPARE_MACRO(Smlslb(z5.VnD(), z5.VnD(), z26.VnS(), z27.VnS()),
                "smlslb z5.d, z26.s, z27.s");
  COMPARE_MACRO(Smlslb(z5.VnH(), z5.VnH(), z26.VnB(), z27.VnB()),
                "smlslb z5.h, z26.b, z27.b");
  COMPARE_MACRO(Smlslb(z5.VnS(), z5.VnS(), z26.VnH(), z27.VnH()),
                "smlslb z5.s, z26.h, z27.h");
  COMPARE_MACRO(Smlslt(z23.VnD(), z23.VnD(), z24.VnS(), z25.VnS()),
                "smlslt z23.d, z24.s, z25.s");
  COMPARE_MACRO(Smlslt(z23.VnH(), z23.VnH(), z24.VnB(), z25.VnB()),
                "smlslt z23.h, z24.b, z25.b");
  COMPARE_MACRO(Smlslt(z23.VnS(), z23.VnS(), z24.VnH(), z25.VnH()),
                "smlslt z23.s, z24.h, z25.h");
  COMPARE_MACRO(Umlalb(z31.VnD(), z31.VnD(), z9.VnS(), z21.VnS()),
                "umlalb z31.d, z9.s, z21.s");
  COMPARE_MACRO(Umlalb(z31.VnH(), z31.VnH(), z9.VnB(), z21.VnB()),
                "umlalb z31.h, z9.b, z21.b");
  COMPARE_MACRO(Umlalb(z31.VnS(), z31.VnS(), z9.VnH(), z21.VnH()),
                "umlalb z31.s, z9.h, z21.h");
  COMPARE_MACRO(Umlalt(z11.VnD(), z11.VnD(), z5.VnS(), z22.VnS()),
                "umlalt z11.d, z5.s, z22.s");
  COMPARE_MACRO(Umlalt(z11.VnH(), z11.VnH(), z5.VnB(), z22.VnB()),
                "umlalt z11.h, z5.b, z22.b");
  COMPARE_MACRO(Umlalt(z11.VnS(), z11.VnS(), z5.VnH(), z22.VnH()),
                "umlalt z11.s, z5.h, z22.h");
  COMPARE_MACRO(Umlslb(z28.VnD(), z28.VnD(), z13.VnS(), z9.VnS()),
                "umlslb z28.d, z13.s, z9.s");
  COMPARE_MACRO(Umlslb(z28.VnH(), z28.VnH(), z13.VnB(), z9.VnB()),
                "umlslb z28.h, z13.b, z9.b");
  COMPARE_MACRO(Umlslb(z28.VnS(), z28.VnS(), z13.VnH(), z9.VnH()),
                "umlslb z28.s, z13.h, z9.h");
  COMPARE_MACRO(Umlslt(z9.VnD(), z9.VnD(), z12.VnS(), z30.VnS()),
                "umlslt z9.d, z12.s, z30.s");
  COMPARE_MACRO(Umlslt(z9.VnH(), z9.VnH(), z12.VnB(), z30.VnB()),
                "umlslt z9.h, z12.b, z30.b");
  COMPARE_MACRO(Umlslt(z9.VnS(), z9.VnS(), z12.VnH(), z30.VnH()),
                "umlslt z9.s, z12.h, z30.h");

  COMPARE_MACRO(Smlalt(z0.VnD(), z1.VnD(), z2.VnS(), z3.VnS()),
                "movprfx z0, z1\n"
                "smlalt z0.d, z2.s, z3.s");
  COMPARE_MACRO(Smlalt(z0.VnD(), z1.VnD(), z0.VnS(), z3.VnS()),
                "movprfx z31, z1\n"
                "smlalt z31.d, z0.s, z3.s\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Smlalt(z0.VnD(), z1.VnD(), z2.VnS(), z0.VnS()),
                "movprfx z31, z1\n"
                "smlalt z31.d, z2.s, z0.s\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Smlalt(z0.VnD(), z1.VnD(), z0.VnS(), z0.VnS()),
                "movprfx z31, z1\n"
                "smlalt z31.d, z0.s, z0.s\n"
                "mov z0.d, z31.d");

  CLEANUP();
}

TEST(sve2_complex_integer_multiply_add) {
  SETUP();

  COMPARE(sqrdcmlah(z31.VnB(), z15.VnB(), z20.VnB(), 0),
          "sqrdcmlah z31.b, z15.b, z20.b, #0");
  COMPARE(sqrdcmlah(z31.VnD(), z15.VnD(), z20.VnD(), 90),
          "sqrdcmlah z31.d, z15.d, z20.d, #90");
  COMPARE(sqrdcmlah(z31.VnH(), z15.VnH(), z20.VnH(), 180),
          "sqrdcmlah z31.h, z15.h, z20.h, #180");
  COMPARE(sqrdcmlah(z31.VnS(), z15.VnS(), z20.VnS(), 270),
          "sqrdcmlah z31.s, z15.s, z20.s, #270");

  COMPARE(sqrdcmlah(z14.VnS(), z11.VnS(), z8.VnS(), 1, 0),
          "sqrdcmlah z14.s, z11.s, z8.s[1], #0");
  COMPARE(sqrdcmlah(z31.VnH(), z2.VnH(), z3.VnH(), 2, 180),
          "sqrdcmlah z31.h, z2.h, z3.h[2], #180");

  COMPARE_MACRO(Sqrdcmlah(z0.VnB(), z1.VnB(), z0.VnB(), z3.VnB(), 0),
                "mov z31.d, z0.d\n"
                "movprfx z0, z1\n"
                "sqrdcmlah z0.b, z31.b, z3.b, #0");
  COMPARE_MACRO(Sqrdcmlah(z0.VnH(), z1.VnH(), z2.VnH(), z0.VnH(), 90),
                "mov z31.d, z0.d\n"
                "movprfx z0, z1\n"
                "sqrdcmlah z0.h, z2.h, z31.h, #90");
  COMPARE_MACRO(Sqrdcmlah(z0.VnS(), z1.VnS(), z0.VnS(), z0.VnS(), 0, 180),
                "movprfx z31, z1\n"
                "sqrdcmlah z31.s, z0.s, z0.s[0], #180\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Sqrdcmlah(z5.VnH(), z1.VnH(), z2.VnH(), z5.VnH(), 3, 270),
                "movprfx z31, z1\n"
                "sqrdcmlah z31.h, z2.h, z5.h[3], #270\n"
                "mov z5.d, z31.d");
  COMPARE_MACRO(Sqrdcmlah(z3.VnH(), z3.VnH(), z3.VnH(), z3.VnH(), 2, 90),
                "sqrdcmlah z3.h, z3.h, z3.h[2], #90");

  COMPARE(cmla(z19.VnB(), z7.VnB(), z2.VnB(), 0), "cmla z19.b, z7.b, z2.b, #0");
  COMPARE(cmla(z19.VnD(), z7.VnD(), z2.VnD(), 90),
          "cmla z19.d, z7.d, z2.d, #90");
  COMPARE(cmla(z19.VnH(), z7.VnH(), z2.VnH(), 180),
          "cmla z19.h, z7.h, z2.h, #180");
  COMPARE(cmla(z19.VnS(), z7.VnS(), z2.VnS(), 270),
          "cmla z19.s, z7.s, z2.s, #270");

  COMPARE_MACRO(Cmla(z0.VnB(), z1.VnB(), z0.VnB(), z3.VnB(), 0),
                "mov z31.d, z0.d\n"
                "movprfx z0, z1\n"
                "cmla z0.b, z31.b, z3.b, #0");
  COMPARE_MACRO(Cmla(z0.VnH(), z1.VnH(), z2.VnH(), z0.VnH(), 90),
                "mov z31.d, z0.d\n"
                "movprfx z0, z1\n"
                "cmla z0.h, z2.h, z31.h, #90");
  COMPARE_MACRO(Cmla(z0.VnS(), z1.VnS(), z0.VnS(), z0.VnS(), 180),
                "mov z31.d, z0.d\n"
                "movprfx z0, z1\n"
                "cmla z0.s, z31.s, z31.s, #180");
  COMPARE_MACRO(Cmla(z0.VnD(), z1.VnD(), z2.VnD(), z0.VnD(), 270),
                "mov z31.d, z0.d\n"
                "movprfx z0, z1\n"
                "cmla z0.d, z2.d, z31.d, #270");

  COMPARE_MACRO(Cmla(z17.VnS(), z17.VnS(), z29.VnS(), z0.VnS(), 1, 0),
                "cmla z17.s, z29.s, z0.s[1], #0");
  COMPARE_MACRO(Cmla(z17.VnS(), z17.VnS(), z29.VnS(), z1.VnS(), 0, 0),
                "cmla z17.s, z29.s, z1.s[0], #0");
  COMPARE_MACRO(Cmla(z17.VnS(), z17.VnS(), z29.VnS(), z8.VnS(), 1, 90),
                "cmla z17.s, z29.s, z8.s[1], #90");
  COMPARE_MACRO(Cmla(z17.VnS(), z17.VnS(), z29.VnS(), z15.VnS(), 0, 180),
                "cmla z17.s, z29.s, z15.s[0], #180");
  COMPARE_MACRO(Cmla(z18.VnH(), z18.VnH(), z22.VnH(), z0.VnH(), 3, 0),
                "cmla z18.h, z22.h, z0.h[3], #0");
  COMPARE_MACRO(Cmla(z18.VnH(), z18.VnH(), z22.VnH(), z1.VnH(), 2, 0),
                "cmla z18.h, z22.h, z1.h[2], #0");
  COMPARE_MACRO(Cmla(z18.VnH(), z18.VnH(), z22.VnH(), z4.VnH(), 1, 270),
                "cmla z18.h, z22.h, z4.h[1], #270");
  COMPARE_MACRO(Cmla(z18.VnH(), z18.VnH(), z22.VnH(), z7.VnH(), 0, 90),
                "cmla z18.h, z22.h, z7.h[0], #90");

  COMPARE_MACRO(Cmla(z1.VnH(), z19.VnH(), z22.VnH(), z7.VnH(), 0, 90),
                "movprfx z1, z19\n"
                "cmla z1.h, z22.h, z7.h[0], #90");
  COMPARE_MACRO(Cmla(z1.VnH(), z19.VnH(), z1.VnH(), z7.VnH(), 0, 90),
                "movprfx z31, z19\n"
                "cmla z31.h, z1.h, z7.h[0], #90\n"
                "mov z1.d, z31.d");
  COMPARE_MACRO(Cmla(z1.VnH(), z19.VnH(), z22.VnH(), z1.VnH(), 0, 90),
                "movprfx z31, z19\n"
                "cmla z31.h, z22.h, z1.h[0], #90\n"
                "mov z1.d, z31.d");
  COMPARE_MACRO(Cmla(z1.VnH(), z19.VnH(), z1.VnH(), z1.VnH(), 0, 90),
                "movprfx z31, z19\n"
                "cmla z31.h, z1.h, z1.h[0], #90\n"
                "mov z1.d, z31.d");

  CLEANUP();
}

TEST(sve2_saturating_multiply_add_long) {
  SETUP();

  COMPARE(sqdmlalb(z6.VnD(), z19.VnS(), z25.VnS()),
          "sqdmlalb z6.d, z19.s, z25.s");
  COMPARE(sqdmlalb(z6.VnH(), z19.VnB(), z25.VnB()),
          "sqdmlalb z6.h, z19.b, z25.b");
  COMPARE(sqdmlalb(z6.VnS(), z19.VnH(), z25.VnH()),
          "sqdmlalb z6.s, z19.h, z25.h");
  COMPARE(sqdmlalt(z11.VnD(), z0.VnS(), z10.VnS()),
          "sqdmlalt z11.d, z0.s, z10.s");
  COMPARE(sqdmlalt(z11.VnH(), z0.VnB(), z10.VnB()),
          "sqdmlalt z11.h, z0.b, z10.b");
  COMPARE(sqdmlalt(z11.VnS(), z0.VnH(), z10.VnH()),
          "sqdmlalt z11.s, z0.h, z10.h");
  COMPARE(sqdmlslb(z16.VnD(), z26.VnS(), z25.VnS()),
          "sqdmlslb z16.d, z26.s, z25.s");
  COMPARE(sqdmlslb(z16.VnH(), z26.VnB(), z25.VnB()),
          "sqdmlslb z16.h, z26.b, z25.b");
  COMPARE(sqdmlslb(z16.VnS(), z26.VnH(), z25.VnH()),
          "sqdmlslb z16.s, z26.h, z25.h");
  COMPARE(sqdmlslt(z21.VnD(), z23.VnS(), z9.VnS()),
          "sqdmlslt z21.d, z23.s, z9.s");
  COMPARE(sqdmlslt(z21.VnH(), z23.VnB(), z9.VnB()),
          "sqdmlslt z21.h, z23.b, z9.b");
  COMPARE(sqdmlslt(z21.VnS(), z23.VnH(), z9.VnH()),
          "sqdmlslt z21.s, z23.h, z9.h");

  COMPARE(sqdmlalb(z1.VnD(), z27.VnS(), z11.VnS(), 0),
          "sqdmlalb z1.d, z27.s, z11.s[0]");
  COMPARE(sqdmlalb(z30.VnS(), z6.VnH(), z3.VnH(), 0),
          "sqdmlalb z30.s, z6.h, z3.h[0]");
  COMPARE(sqdmlalt(z30.VnD(), z25.VnS(), z15.VnS(), 1),
          "sqdmlalt z30.d, z25.s, z15.s[1]");
  COMPARE(sqdmlalt(z10.VnS(), z1.VnH(), z1.VnH(), 3),
          "sqdmlalt z10.s, z1.h, z1.h[3]");
  COMPARE(sqdmlslb(z15.VnD(), z27.VnS(), z15.VnS(), 2),
          "sqdmlslb z15.d, z27.s, z15.s[2]");
  COMPARE(sqdmlslb(z5.VnS(), z5.VnH(), z7.VnH(), 6),
          "sqdmlslb z5.s, z5.h, z7.h[6]");
  COMPARE(sqdmlslt(z21.VnD(), z28.VnS(), z13.VnS(), 3),
          "sqdmlslt z21.d, z28.s, z13.s[3]");
  COMPARE(sqdmlslt(z5.VnS(), z3.VnH(), z1.VnH(), 7),
          "sqdmlslt z5.s, z3.h, z1.h[7]");

  COMPARE_MACRO(Sqdmlalb(z6.VnD(), z16.VnD(), z19.VnS(), z25.VnS()),
                "movprfx z6, z16\n"
                "sqdmlalb z6.d, z19.s, z25.s");
  COMPARE_MACRO(Sqdmlalt(z4.VnH(), z26.VnH(), z4.VnB(), z24.VnB()),
                "movprfx z31, z26\n"
                "sqdmlalt z31.h, z4.b, z24.b\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Sqdmlslb(z2.VnS(), z6.VnS(), z17.VnH(), z2.VnH()),
                "movprfx z31, z6\n"
                "sqdmlslb z31.s, z17.h, z2.h\n"
                "mov z2.d, z31.d");
  COMPARE_MACRO(Sqdmlslt(z0.VnD(), z1.VnD(), z0.VnS(), z0.VnS()),
                "movprfx z31, z1\n"
                "sqdmlslt z31.d, z0.s, z0.s\n"
                "mov z0.d, z31.d");

  COMPARE_MACRO(Sqdmlalb(z6.VnD(), z16.VnD(), z9.VnS(), z15.VnS(), 0),
                "movprfx z6, z16\n"
                "sqdmlalb z6.d, z9.s, z15.s[0]");
  COMPARE_MACRO(Sqdmlalt(z4.VnS(), z6.VnS(), z4.VnH(), z4.VnH(), 3),
                "movprfx z31, z6\n"
                "sqdmlalt z31.s, z4.h, z4.h[3]\n"
                "mov z4.d, z31.d");
  COMPARE_MACRO(Sqdmlslb(z2.VnS(), z16.VnS(), z17.VnH(), z2.VnH(), 6),
                "movprfx z31, z16\n"
                "sqdmlslb z31.s, z17.h, z2.h[6]\n"
                "mov z2.d, z31.d");
  COMPARE_MACRO(Sqdmlslt(z6.VnD(), z1.VnD(), z6.VnS(), z6.VnS(), 2),
                "movprfx z31, z1\n"
                "sqdmlslt z31.d, z6.s, z6.s[2]\n"
                "mov z6.d, z31.d");

  CLEANUP();
}

TEST(sve2_saturating_multiply_add_interleaved_long) {
  SETUP();

  COMPARE(sqdmlalbt(z23.VnD(), z29.VnS(), z26.VnS()),
          "sqdmlalbt z23.d, z29.s, z26.s");
  COMPARE(sqdmlalbt(z23.VnH(), z29.VnB(), z26.VnB()),
          "sqdmlalbt z23.h, z29.b, z26.b");
  COMPARE(sqdmlalbt(z23.VnS(), z29.VnH(), z26.VnH()),
          "sqdmlalbt z23.s, z29.h, z26.h");
  COMPARE(sqdmlslbt(z26.VnD(), z23.VnS(), z4.VnS()),
          "sqdmlslbt z26.d, z23.s, z4.s");
  COMPARE(sqdmlslbt(z26.VnH(), z23.VnB(), z4.VnB()),
          "sqdmlslbt z26.h, z23.b, z4.b");
  COMPARE(sqdmlslbt(z26.VnS(), z23.VnH(), z4.VnH()),
          "sqdmlslbt z26.s, z23.h, z4.h");

  COMPARE_MACRO(Sqdmlalbt(z29.VnD(), z0.VnD(), z29.VnS(), z26.VnS()),
                "movprfx z31, z0\n"
                "sqdmlalbt z31.d, z29.s, z26.s\n"
                "mov z29.d, z31.d");
  COMPARE_MACRO(Sqdmlalbt(z26.VnH(), z0.VnH(), z29.VnB(), z26.VnB()),
                "movprfx z31, z0\n"
                "sqdmlalbt z31.h, z29.b, z26.b\n"
                "mov z26.d, z31.d");
  COMPARE_MACRO(Sqdmlslbt(z23.VnS(), z31.VnS(), z26.VnH(), z29.VnH()),
                "movprfx z23, z31\n"
                "sqdmlslbt z23.s, z26.h, z29.h");
  COMPARE_MACRO(Sqdmlslbt(z4.VnD(), z31.VnD(), z4.VnS(), z4.VnS()),
                "sqdmlslbt z31.d, z4.s, z4.s\n"
                "mov z4.d, z31.d");

  CLEANUP();
}

TEST(sve2_floating_multiply_add_long_vector) {
  SETUP();

  COMPARE(fmlalb(z16.VnS(), z18.VnH(), z29.VnH()),
          "fmlalb z16.s, z18.h, z29.h");
  COMPARE(fmlalb(z3.VnS(), z8.VnH(), z7.VnH()), "fmlalb z3.s, z8.h, z7.h");
  COMPARE(fmlalt(z18.VnS(), z13.VnH(), z5.VnH()), "fmlalt z18.s, z13.h, z5.h");
  COMPARE(fmlalt(z18.VnS(), z7.VnH(), z16.VnH()), "fmlalt z18.s, z7.h, z16.h");
  COMPARE(fmlslb(z16.VnS(), z10.VnH(), z1.VnH()), "fmlslb z16.s, z10.h, z1.h");
  COMPARE(fmlslb(z25.VnS(), z11.VnH(), z0.VnH()), "fmlslb z25.s, z11.h, z0.h");
  COMPARE(fmlslt(z3.VnS(), z17.VnH(), z14.VnH()), "fmlslt z3.s, z17.h, z14.h");
  COMPARE(fmlslt(z5.VnS(), z1.VnH(), z7.VnH()), "fmlslt z5.s, z1.h, z7.h");

  CLEANUP();
}

TEST(sve2_mla_long_index) {
  SETUP();

  COMPARE_MACRO(Smlalb(z11.VnD(), z11.VnD(), z29.VnS(), z0.VnS(), 3),
                "smlalb z11.d, z29.s, z0.s[3]");
  COMPARE_MACRO(Smlalb(z18.VnS(), z18.VnS(), z17.VnH(), z0.VnH(), 7),
                "smlalb z18.s, z17.h, z0.h[7]");
  COMPARE_MACRO(Smlalt(z10.VnD(), z10.VnD(), z30.VnS(), z15.VnS(), 0),
                "smlalt z10.d, z30.s, z15.s[0]");
  COMPARE_MACRO(Smlalt(z23.VnS(), z23.VnS(), z31.VnH(), z7.VnH(), 0),
                "smlalt z23.s, z31.h, z7.h[0]");
  COMPARE_MACRO(Smlslb(z12.VnD(), z12.VnD(), z23.VnS(), z3.VnS(), 1),
                "smlslb z12.d, z23.s, z3.s[1]");
  COMPARE_MACRO(Smlslb(z5.VnS(), z5.VnS(), z4.VnH(), z4.VnH(), 2),
                "smlslb z5.s, z4.h, z4.h[2]");
  COMPARE_MACRO(Smlslt(z7.VnD(), z7.VnD(), z9.VnS(), z6.VnS(), 3),
                "smlslt z7.d, z9.s, z6.s[3]");
  COMPARE_MACRO(Smlslt(z9.VnS(), z9.VnS(), z21.VnH(), z3.VnH(), 4),
                "smlslt z9.s, z21.h, z3.h[4]");
  COMPARE_MACRO(Umlalb(z9.VnD(), z9.VnD(), z1.VnS(), z11.VnS(), 0),
                "umlalb z9.d, z1.s, z11.s[0]");
  COMPARE_MACRO(Umlalb(z9.VnS(), z9.VnS(), z5.VnH(), z1.VnH(), 6),
                "umlalb z9.s, z5.h, z1.h[6]");
  COMPARE_MACRO(Umlalt(z6.VnD(), z6.VnD(), z17.VnS(), z14.VnS(), 1),
                "umlalt z6.d, z17.s, z14.s[1]");
  COMPARE_MACRO(Umlalt(z9.VnS(), z9.VnS(), z11.VnH(), z3.VnH(), 7),
                "umlalt z9.s, z11.h, z3.h[7]");
  COMPARE_MACRO(Umlslb(z12.VnD(), z12.VnD(), z15.VnS(), z9.VnS(), 2),
                "umlslb z12.d, z15.s, z9.s[2]");
  COMPARE_MACRO(Umlslb(z14.VnS(), z14.VnS(), z10.VnH(), z2.VnH(), 0),
                "umlslb z14.s, z10.h, z2.h[0]");
  COMPARE_MACRO(Umlslt(z12.VnD(), z12.VnD(), z28.VnS(), z8.VnS(), 3),
                "umlslt z12.d, z28.s, z8.s[3]");
  COMPARE_MACRO(Umlslt(z24.VnS(), z24.VnS(), z12.VnH(), z6.VnH(), 1),
                "umlslt z24.s, z12.h, z6.h[1]");

  COMPARE_MACRO(Umlslt(z2.VnS(), z23.VnS(), z12.VnH(), z6.VnH(), 1),
                "movprfx z2, z23\n"
                "umlslt z2.s, z12.h, z6.h[1]");
  COMPARE_MACRO(Umlslt(z2.VnS(), z23.VnS(), z2.VnH(), z6.VnH(), 1),
                "movprfx z31, z23\n"
                "umlslt z31.s, z2.h, z6.h[1]\n"
                "mov z2.d, z31.d");
  COMPARE_MACRO(Umlslt(z2.VnS(), z23.VnS(), z12.VnH(), z2.VnH(), 1),
                "movprfx z31, z23\n"
                "umlslt z31.s, z12.h, z2.h[1]\n"
                "mov z2.d, z31.d");
  COMPARE_MACRO(Umlslt(z2.VnS(), z23.VnS(), z2.VnH(), z2.VnH(), 1),
                "movprfx z31, z23\n"
                "umlslt z31.s, z2.h, z2.h[1]\n"
                "mov z2.d, z31.d");

  CLEANUP();
}

TEST(sve2_mul_long_index) {
  SETUP();

  COMPARE_MACRO(Smullb(z13.VnS(), z31.VnH(), z0.VnH(), 0),
                "smullb z13.s, z31.h, z0.h[0]");
  COMPARE_MACRO(Smullb(z8.VnD(), z22.VnS(), z0.VnS(), 0),
                "smullb z8.d, z22.s, z0.s[0]");
  COMPARE_MACRO(Smullt(z14.VnS(), z30.VnH(), z7.VnH(), 7),
                "smullt z14.s, z30.h, z7.h[7]");
  COMPARE_MACRO(Smullt(z22.VnD(), z28.VnS(), z15.VnS(), 3),
                "smullt z22.d, z28.s, z15.s[3]");
  COMPARE_MACRO(Umullb(z24.VnD(), z20.VnS(), z5.VnS(), 1),
                "umullb z24.d, z20.s, z5.s[1]");
  COMPARE_MACRO(Umullb(z28.VnS(), z19.VnH(), z3.VnH(), 4),
                "umullb z28.s, z19.h, z3.h[4]");
  COMPARE_MACRO(Umullt(z0.VnD(), z31.VnS(), z8.VnS(), 2),
                "umullt z0.d, z31.s, z8.s[2]");
  COMPARE_MACRO(Umullt(z14.VnS(), z20.VnH(), z5.VnH(), 6),
                "umullt z14.s, z20.h, z5.h[6]");

  CLEANUP();
}

TEST(sve2_sat_double_mul_high) {
  SETUP();

  COMPARE_MACRO(Sqdmulh(z18.VnB(), z25.VnB(), z1.VnB()),
                "sqdmulh z18.b, z25.b, z1.b");
  COMPARE_MACRO(Sqdmulh(z18.VnD(), z25.VnD(), z1.VnD()),
                "sqdmulh z18.d, z25.d, z1.d");
  COMPARE_MACRO(Sqdmulh(z18.VnH(), z25.VnH(), z1.VnH()),
                "sqdmulh z18.h, z25.h, z1.h");
  COMPARE_MACRO(Sqdmulh(z18.VnS(), z25.VnS(), z1.VnS()),
                "sqdmulh z18.s, z25.s, z1.s");
  COMPARE_MACRO(Sqrdmulh(z21.VnB(), z21.VnB(), z27.VnB()),
                "sqrdmulh z21.b, z21.b, z27.b");
  COMPARE_MACRO(Sqrdmulh(z21.VnD(), z21.VnD(), z27.VnD()),
                "sqrdmulh z21.d, z21.d, z27.d");
  COMPARE_MACRO(Sqrdmulh(z21.VnH(), z21.VnH(), z27.VnH()),
                "sqrdmulh z21.h, z21.h, z27.h");
  COMPARE_MACRO(Sqrdmulh(z21.VnS(), z21.VnS(), z27.VnS()),
                "sqrdmulh z21.s, z21.s, z27.s");

  CLEANUP();
}

TEST(sve2_flogb) {
  SETUP();

  COMPARE_MACRO(Flogb(z15.VnH(), p0.Merging(), z3.VnH()),
                "flogb z15.h, p0/m, z3.h");
  COMPARE_MACRO(Flogb(z15.VnS(), p0.Merging(), z3.VnS()),
                "flogb z15.s, p0/m, z3.s");
  COMPARE_MACRO(Flogb(z15.VnD(), p0.Merging(), z3.VnD()),
                "flogb z15.d, p0/m, z3.d");
  COMPARE_MACRO(Flogb(z15.VnD(), p0.Zeroing(), z3.VnD()),
                "movprfx z15.d, p0/z, z15.d\n"
                "flogb z15.d, p0/m, z3.d");

  CLEANUP();
}

TEST(sve2_fp_pair) {
  SETUP();

  COMPARE_MACRO(Faddp(z14.VnD(), p1.Merging(), z14.VnD(), z26.VnD()),
                "faddp z14.d, p1/m, z14.d, z26.d");
  COMPARE_MACRO(Faddp(z14.VnH(), p1.Merging(), z14.VnH(), z26.VnH()),
                "faddp z14.h, p1/m, z14.h, z26.h");
  COMPARE_MACRO(Faddp(z14.VnS(), p1.Merging(), z14.VnS(), z26.VnS()),
                "faddp z14.s, p1/m, z14.s, z26.s");
  COMPARE_MACRO(Fmaxnmp(z2.VnD(), p1.Merging(), z2.VnD(), z14.VnD()),
                "fmaxnmp z2.d, p1/m, z2.d, z14.d");
  COMPARE_MACRO(Fmaxnmp(z2.VnH(), p1.Merging(), z2.VnH(), z14.VnH()),
                "fmaxnmp z2.h, p1/m, z2.h, z14.h");
  COMPARE_MACRO(Fmaxnmp(z2.VnS(), p1.Merging(), z2.VnS(), z14.VnS()),
                "fmaxnmp z2.s, p1/m, z2.s, z14.s");
  COMPARE_MACRO(Fmaxp(z22.VnD(), p1.Merging(), z22.VnD(), z3.VnD()),
                "fmaxp z22.d, p1/m, z22.d, z3.d");
  COMPARE_MACRO(Fmaxp(z22.VnH(), p1.Merging(), z22.VnH(), z3.VnH()),
                "fmaxp z22.h, p1/m, z22.h, z3.h");
  COMPARE_MACRO(Fmaxp(z22.VnS(), p1.Merging(), z22.VnS(), z3.VnS()),
                "fmaxp z22.s, p1/m, z22.s, z3.s");
  COMPARE_MACRO(Fminnmp(z1.VnD(), p0.Merging(), z1.VnD(), z14.VnD()),
                "fminnmp z1.d, p0/m, z1.d, z14.d");
  COMPARE_MACRO(Fminnmp(z1.VnH(), p0.Merging(), z1.VnH(), z14.VnH()),
                "fminnmp z1.h, p0/m, z1.h, z14.h");
  COMPARE_MACRO(Fminnmp(z1.VnS(), p0.Merging(), z1.VnS(), z14.VnS()),
                "fminnmp z1.s, p0/m, z1.s, z14.s");
  COMPARE_MACRO(Fminp(z16.VnD(), p3.Merging(), z16.VnD(), z11.VnD()),
                "fminp z16.d, p3/m, z16.d, z11.d");
  COMPARE_MACRO(Fminp(z16.VnH(), p3.Merging(), z16.VnH(), z11.VnH()),
                "fminp z16.h, p3/m, z16.h, z11.h");
  COMPARE_MACRO(Fminp(z16.VnS(), p3.Merging(), z16.VnS(), z11.VnS()),
                "fminp z16.s, p3/m, z16.s, z11.s");

  COMPARE_MACRO(Faddp(z14.VnD(), p1.Merging(), z13.VnD(), z26.VnD()),
                "movprfx z14.d, p1/m, z13.d\n"
                "faddp z14.d, p1/m, z14.d, z26.d");
  COMPARE_MACRO(Fmaxnmp(z2.VnD(), p1.Merging(), z3.VnD(), z2.VnD()),
                "mov z31.d, z2.d\n"
                "movprfx z2.d, p1/m, z3.d\n"
                "fmaxnmp z2.d, p1/m, z2.d, z31.d");
  COMPARE_MACRO(Fmaxp(z22.VnH(), p1.Merging(), z23.VnH(), z3.VnH()),
                "movprfx z22.h, p1/m, z23.h\n"
                "fmaxp z22.h, p1/m, z22.h, z3.h");
  COMPARE_MACRO(Fminnmp(z1.VnH(), p0.Merging(), z4.VnH(), z1.VnH()),
                "mov z31.d, z1.d\n"
                "movprfx z1.h, p0/m, z4.h\n"
                "fminnmp z1.h, p0/m, z1.h, z31.h");
  COMPARE_MACRO(Fminp(z16.VnS(), p3.Merging(), z11.VnS(), z11.VnS()),
                "movprfx z16.s, p3/m, z11.s\n"
                "fminp z16.s, p3/m, z16.s, z11.s");
  CLEANUP();
}

TEST(sve2_fmlal_fmlsl_index) {
  SETUP();

  COMPARE_MACRO(Fmlalb(z16.VnS(), z16.VnS(), z18.VnH(), z2.VnH(), 0),
                "fmlalb z16.s, z18.h, z2.h[0]");
  COMPARE_MACRO(Fmlalb(z3.VnS(), z3.VnS(), z8.VnH(), z7.VnH(), 7),
                "fmlalb z3.s, z8.h, z7.h[7]");
  COMPARE_MACRO(Fmlalt(z18.VnS(), z18.VnS(), z13.VnH(), z5.VnH(), 6),
                "fmlalt z18.s, z13.h, z5.h[6]");
  COMPARE_MACRO(Fmlalt(z18.VnS(), z18.VnS(), z7.VnH(), z6.VnH(), 5),
                "fmlalt z18.s, z7.h, z6.h[5]");
  COMPARE_MACRO(Fmlslb(z16.VnS(), z16.VnS(), z10.VnH(), z1.VnH(), 4),
                "fmlslb z16.s, z10.h, z1.h[4]");
  COMPARE_MACRO(Fmlslb(z25.VnS(), z25.VnS(), z11.VnH(), z0.VnH(), 3),
                "fmlslb z25.s, z11.h, z0.h[3]");
  COMPARE_MACRO(Fmlslt(z3.VnS(), z3.VnS(), z17.VnH(), z4.VnH(), 2),
                "fmlslt z3.s, z17.h, z4.h[2]");
  COMPARE_MACRO(Fmlslt(z5.VnS(), z5.VnS(), z1.VnH(), z7.VnH(), 1),
                "fmlslt z5.s, z1.h, z7.h[1]");

  COMPARE_MACRO(Fmlalb(z5.VnS(), z4.VnS(), z1.VnH(), z7.VnH(), 1),
                "movprfx z5, z4\n"
                "fmlalb z5.s, z1.h, z7.h[1]");
  COMPARE_MACRO(Fmlalt(z5.VnS(), z4.VnS(), z5.VnH(), z7.VnH(), 1),
                "movprfx z31, z4\n"
                "fmlalt z31.s, z5.h, z7.h[1]\n"
                "mov z5.d, z31.d");
  COMPARE_MACRO(Fmlslb(z5.VnS(), z4.VnS(), z1.VnH(), z5.VnH(), 1),
                "movprfx z31, z4\n"
                "fmlslb z31.s, z1.h, z5.h[1]\n"
                "mov z5.d, z31.d");
  COMPARE_MACRO(Fmlslt(z5.VnS(), z4.VnS(), z5.VnH(), z5.VnH(), 1),
                "movprfx z31, z4\n"
                "fmlslt z31.s, z5.h, z5.h[1]\n"
                "mov z5.d, z31.d");
  CLEANUP();
}

TEST(sve2_fp_convert) {
  SETUP();

  COMPARE_MACRO(Fcvtx(z14.VnS(), p4.Merging(), z0.VnD()),
                "fcvtx z14.s, p4/m, z0.d");
  COMPARE_MACRO(Fcvtx(z14.VnS(), p4.Zeroing(), z0.VnD()),
                "movprfx z14.d, p4/z, z14.d\n"
                "fcvtx z14.s, p4/m, z0.d");
  COMPARE_MACRO(Fcvtlt(z1.VnD(), p1.Merging(), z28.VnS()),
                "fcvtlt z1.d, p1/m, z28.s");
  COMPARE_MACRO(Fcvtlt(z10.VnS(), p5.Merging(), z0.VnH()),
                "fcvtlt z10.s, p5/m, z0.h");
  COMPARE_MACRO(Fcvtnt(z4.VnH(), p7.Merging(), z0.VnS()),
                "fcvtnt z4.h, p7/m, z0.s");
  COMPARE_MACRO(Fcvtnt(z8.VnS(), p0.Merging(), z4.VnD()),
                "fcvtnt z8.s, p0/m, z4.d");
  COMPARE_MACRO(Fcvtx(z14.VnS(), p4.Merging(), z0.VnD()),
                "fcvtx z14.s, p4/m, z0.d");
  COMPARE_MACRO(Fcvtxnt(z27.VnS(), p0.Merging(), z17.VnD()),
                "fcvtxnt z27.s, p0/m, z17.d");

  CLEANUP();
}

TEST(sve2_sat_double_mul_high_index) {
  SETUP();

  COMPARE_MACRO(Sqdmulh(z11.VnH(), z20.VnH(), z7.VnH(), 1),
                "sqdmulh z11.h, z20.h, z7.h[1]");
  COMPARE_MACRO(Sqdmulh(z11.VnH(), z20.VnH(), z2.VnH(), 7),
                "sqdmulh z11.h, z20.h, z2.h[7]");
  COMPARE_MACRO(Sqdmulh(z8.VnS(), z4.VnS(), z7.VnS(), 1),
                "sqdmulh z8.s, z4.s, z7.s[1]");
  COMPARE_MACRO(Sqdmulh(z8.VnS(), z4.VnS(), z3.VnS(), 3),
                "sqdmulh z8.s, z4.s, z3.s[3]");
  COMPARE_MACRO(Sqdmulh(z6.VnD(), z13.VnD(), z0.VnD(), 1),
                "sqdmulh z6.d, z13.d, z0.d[1]");
  COMPARE_MACRO(Sqdmulh(z6.VnD(), z13.VnD(), z15.VnD(), 0),
                "sqdmulh z6.d, z13.d, z15.d[0]");

  COMPARE_MACRO(Sqrdmulh(z3.VnH(), z29.VnH(), z7.VnH(), 2),
                "sqrdmulh z3.h, z29.h, z7.h[2]");
  COMPARE_MACRO(Sqrdmulh(z3.VnH(), z29.VnH(), z3.VnH(), 7),
                "sqrdmulh z3.h, z29.h, z3.h[7]");
  COMPARE_MACRO(Sqrdmulh(z19.VnS(), z15.VnS(), z7.VnS(), 0),
                "sqrdmulh z19.s, z15.s, z7.s[0]");
  COMPARE_MACRO(Sqrdmulh(z19.VnS(), z15.VnS(), z2.VnS(), 3),
                "sqrdmulh z19.s, z15.s, z2.s[3]");
  COMPARE_MACRO(Sqrdmulh(z29.VnD(), z13.VnD(), z0.VnD(), 1),
                "sqrdmulh z29.d, z13.d, z0.d[1]");
  COMPARE_MACRO(Sqrdmulh(z29.VnD(), z13.VnD(), z15.VnD(), 0),
                "sqrdmulh z29.d, z13.d, z15.d[0]");

  CLEANUP();
}

TEST(sve2_extract) {
  SETUP();

  COMPARE_MACRO(Ext(z0.VnB(), z1.VnB(), z2.VnB(), 2),
                "ext z0.b, {z1.b, z2.b}, #2");
  COMPARE_MACRO(Ext(z0.VnB(), z31.VnB(), z0.VnB(), 255),
                "ext z0.b, {z31.b, z0.b}, #255");
  COMPARE_MACRO(Ext(z0.VnB(), z31.VnB(), z0.VnB(), 0),
                "ext z0.b, {z31.b, z0.b}, #0");

  // Check destructive form is preferred over constructive.
  COMPARE_MACRO(Ext(z0.VnB(), z0.VnB(), z1.VnB(), 42),
                "ext z0.b, z0.b, z1.b, #42");

  CLEANUP();
}

TEST(sve_matmul) {
  SETUP();

  COMPARE_MACRO(Fmmla(z2.VnS(), z2.VnS(), z3.VnS(), z20.VnS()),
                "fmmla z2.s, z3.s, z20.s");
  COMPARE_MACRO(Fmmla(z21.VnD(), z21.VnD(), z30.VnD(), z2.VnD()),
                "fmmla z21.d, z30.d, z2.d");
  COMPARE_MACRO(Smmla(z31.VnS(), z31.VnS(), z7.VnB(), z19.VnB()),
                "smmla z31.s, z7.b, z19.b");
  COMPARE_MACRO(Ummla(z0.VnS(), z0.VnS(), z1.VnB(), z2.VnB()),
                "ummla z0.s, z1.b, z2.b");
  COMPARE_MACRO(Usmmla(z30.VnS(), z30.VnS(), z31.VnB(), z4.VnB()),
                "usmmla z30.s, z31.b, z4.b");

  COMPARE_MACRO(Fmmla(z0.VnS(), z1.VnS(), z2.VnS(), z3.VnS()),
                "movprfx z0, z1\n"
                "fmmla z0.s, z2.s, z3.s");
  COMPARE_MACRO(Smmla(z0.VnS(), z1.VnS(), z0.VnB(), z3.VnB()),
                "movprfx z31, z1\n"
                "smmla z31.s, z0.b, z3.b\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Ummla(z0.VnS(), z1.VnS(), z2.VnB(), z0.VnB()),
                "movprfx z31, z1\n"
                "ummla z31.s, z2.b, z0.b\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Usmmla(z0.VnS(), z1.VnS(), z0.VnB(), z0.VnB()),
                "movprfx z31, z1\n"
                "usmmla z31.s, z0.b, z0.b\n"
                "mov z0.d, z31.d");

  CLEANUP();
}

TEST(sve_usdot_sudot) {
  SETUP();

  COMPARE_MACRO(Usdot(z30.VnS(), z30.VnS(), z31.VnB(), z4.VnB()),
                "usdot z30.s, z31.b, z4.b");
  COMPARE_MACRO(Usdot(z30.VnS(), z30.VnS(), z31.VnB(), z4.VnB(), 0),
                "usdot z30.s, z31.b, z4.b[0]");
  COMPARE_MACRO(Sudot(z30.VnS(), z30.VnS(), z31.VnB(), z4.VnB(), 3),
                "sudot z30.s, z31.b, z4.b[3]");

  COMPARE_MACRO(Usdot(z0.VnS(), z30.VnS(), z29.VnB(), z28.VnB()),
                "movprfx z0, z30\n"
                "usdot z0.s, z29.b, z28.b");
  COMPARE_MACRO(Usdot(z0.VnS(), z30.VnS(), z29.VnB(), z0.VnB()),
                "movprfx z31, z30\n"
                "usdot z31.s, z29.b, z0.b\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Usdot(z0.VnS(), z30.VnS(), z0.VnB(), z28.VnB()),
                "movprfx z31, z30\n"
                "usdot z31.s, z0.b, z28.b\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Usdot(z0.VnS(), z30.VnS(), z0.VnB(), z0.VnB()),
                "movprfx z31, z30\n"
                "usdot z31.s, z0.b, z0.b\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Usdot(z0.VnS(), z30.VnS(), z29.VnB(), z4.VnB(), 0),
                "movprfx z0, z30\n"
                "usdot z0.s, z29.b, z4.b[0]");
  COMPARE_MACRO(Usdot(z0.VnS(), z30.VnS(), z0.VnB(), z4.VnB(), 0),
                "movprfx z31, z30\n"
                "usdot z31.s, z0.b, z4.b[0]\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Sudot(z0.VnS(), z30.VnS(), z29.VnB(), z0.VnB(), 0),
                "movprfx z31, z30\n"
                "sudot z31.s, z29.b, z0.b[0]\n"
                "mov z0.d, z31.d");
  COMPARE_MACRO(Usdot(z0.VnS(), z30.VnS(), z0.VnB(), z0.VnB(), 0),
                "movprfx z31, z30\n"
                "usdot z31.s, z0.b, z0.b[0]\n"
                "mov z0.d, z31.d");

  CLEANUP();
}
}  // namespace aarch64
}  // namespace vixl
