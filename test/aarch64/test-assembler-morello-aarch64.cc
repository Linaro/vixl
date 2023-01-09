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

#include <sys/mman.h>
#include <unistd.h>

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "test-runner.h"
#include "test-utils.h"
#include "aarch64/test-utils-aarch64.h"

#include "aarch64/cpu-aarch64.h"
#include "aarch64/disasm-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"
#include "aarch64/simulator-aarch64.h"
#include "test-assembler-aarch64.h"

namespace vixl {
namespace aarch64 {

TEST(morello_isa_labels) {
  SETUP_WITH_FEATURES(CPUFeatures::kMorello);
  START();

  // We assemble for A64 by default.
  VIXL_ASSERT(masm.GetISA() == ISA::A64);

  // Labels to be bound in A64 code.
  Label a64_0, a64_1, a64_2;
  // Labels to be bound in C64 code.
  Label c64_0, c64_1, c64_2;
  // Labels to be bound in data.
  Label data_0, data_1;

  // Labels are initialised with an unknown target ISA.
  VIXL_ASSERT(!a64_0.HasKnownISA());
  VIXL_ASSERT(!a64_1.HasKnownISA());
  VIXL_ASSERT(!a64_2.HasKnownISA());
  VIXL_ASSERT(!c64_0.HasKnownISA());
  VIXL_ASSERT(!c64_1.HasKnownISA());
  VIXL_ASSERT(!c64_2.HasKnownISA());
  VIXL_ASSERT(!data_0.HasKnownISA());
  VIXL_ASSERT(!data_1.HasKnownISA());

  __ B(&a64_0);
  __ Bind(&a64_2);
  __ B(&a64_1, eq);
  // Conventional branches don't interwork.
  VIXL_ASSERT(a64_0.GetISA() == ISA::A64);
  VIXL_ASSERT(a64_1.GetISA() == ISA::A64);
  // Binding a label always sets its ISA.
  VIXL_ASSERT(a64_2.GetISA() == ISA::A64);

  // `adr` can address both code and data, so it doesn't affect the target ISA.
  __ Adr(x0, &a64_2);
  __ Adr(x1, &data_0);
  VIXL_ASSERT(!data_0.HasKnownISA());

  // `bx` interworks unconditionally.
  {
    ExactAssemblyScope guard(&masm, kInstructionSize);
    __ bx(&c64_0);
    VIXL_ASSERT(c64_0.GetISA() == ISA::C64);
    masm.SetISA(ISA::C64);
    __ bind(&c64_0);
  }

  // Adr works irrespective of the target ISA.
  __ Adr(c2, &a64_0);
  __ Adr(c3, &a64_1);
  __ Adr(c4, &a64_2);
  __ Adr(c5, &c64_0);
  __ Adr(c6, &c64_1);
  __ Adr(c7, &c64_2);

  __ Bind(&c64_2);
  __ B(&c64_0, eq);
  __ B(&c64_1);

  // Only `adr` can address Data, but it can address the same data from both A64
  // and C64.
  __ Adr(c8, &data_0);
  {
    ISAScope isa(&masm, ISA::Data);
    ExactAssemblyScope guard(&masm, sizeof(uint32_t) * 2);
    __ bind(&data_0);
    __ dc32(42);
    __ bind(&data_1);
    __ dc32(43);
  }
  __ Adr(c9, &data_0);
  __ Adr(c10, &data_1);
  __ Bind(&c64_1);

  // `bx` interworks unconditionally.
  {
    ExactAssemblyScope guard(&masm, kInstructionSize);
    __ bx(&a64_0);
    VIXL_ASSERT(a64_0.GetISA() == ISA::A64);
    masm.SetISA(ISA::A64);
    __ bind(&a64_0);
  }

  // We can jump over a C64 region.
  __ B(&a64_2);

  __ Bind(&a64_1);

  END();
  DISASSEMBLE();

  // TODO: Organise this test so we can execute it.
}

TEST(morello_asm_bx) {
  SETUP_WITH_FEATURES(CPUFeatures::kMorello);
  START();

  {
    ExactAssemblyScope guard(&masm, 8 * kInstructionSize);
    Label a64, c64;

    VIXL_ASSERT(masm.GetISA() == ISA::A64);

    // In A64, this is: `adr x0, pc+0`
    // In C64, this is: `adr c0, pcc+0`
    __ dci(ADR | c0.GetCode());
    __ gctag(x0, c0);  // Set x0 = 0 for A64, or 1 for C64.

    __ bx(&c64);
    __ SetISA(ISA::C64);
    __ bind(&c64);

    __ dci(ADR | c1.GetCode());
    __ gctag(x1, c1);

    __ bx(&a64);
    __ SetISA(ISA::A64);
    __ bind(&a64);

    __ dci(ADR | c2.GetCode());
    __ gctag(x2, c2);
  }

  END();

  if (CAN_RUN()) {
    RUN();
    ASSERT_EQUAL_64(0, x0);
    ASSERT_EQUAL_64(1, x1);
    ASSERT_EQUAL_64(0, x2);
  } else {
    DISASSEMBLE();
  }
}

#ifdef VIXL_NEGATIVE_TESTING
TEST(morello_asm_bx_same_isa) {
  auto test = [](ISA isa) -> bool {
    SETUP_WITH_FEATURES(CPUFeatures::kMorello);
    START();
    bool caught_error = false;
    {
      ISAScope isa_scope(&masm, isa);
      ExactAssemblyScope guard(&masm, kInstructionSize);
      Label target;

      __ bx(&target);
      try {
        __ bind(&target);  // This should fail, since the ISA is the same.
      } catch (const std::runtime_error&) {
        // Correctly link the label, to avoid the `!IsLinked()` assertion on
        // destruction.
        masm.SetISA(ExchangeISA(isa));
        __ bind(&target);
        caught_error = true;
      }
    }
    END();
    DISASSEMBLE();
    return caught_error;
  };
  VIXL_CHECK(test(ISA::A64));
  VIXL_CHECK(test(ISA::C64));
}
#endif

#if VIXL_HOST_HAS_CAPABILITIES
TEST(morello_ldr_literal_cap) {
  SETUP_WITH_FEATURES(CPUFeatures::kMorello);
  START();

  __ SetFixedCodeAddressBits(kCRegSizeInBytesLog2);

  // Load several capabilities, to test alignment logic.
  uint8_t data[8];
  auto cap = [&data](size_t i) -> uintcap_t {
    i %= sizeof(data);
#if VIXL_HOST_CHERI_PURECAP
    return reinterpret_cast<uintcap_t>(cheri_bounds_set(&data[i], 1));
#else
    auto addr = reinterpret_cast<ptraddr_t>(&data[i]);
    auto cap = cheri_address_set(cheri_ddc_get(), addr);
    return reinterpret_cast<uintcap_t>(cheri_bounds_set(cap, 1));
#endif
  };

  for (unsigned i = 0; i < sizeof(data); i++) {
    __ Ldr(CRegister(i), cap(i));
  }

  END();

  if (CAN_RUN()) {
    RUN();

    // TODO: Check capabilities, once they're supported by RegisterDump.
    uint64_t base = cheri_address_get(cap(0));
    for (unsigned i = 0; i < sizeof(data); i++) {
      ASSERT_EQUAL_64(base + i, XRegister(i));
    }
  }
}
#endif

// Test that we can use csp as a stack pointer.
TEST(morello_csp) {
  SETUP_WITH_FEATURES(CPUFeatures::kMorello);
  START();

  Label csp_is_valid;
  // Purecap hosts can just use csp directly. Otherwise, we'll need to derive a
  // csp from DDC.
  __ Chktgd(csp);
  __ B(cs, &csp_is_valid);
  __ Mrs(c0, DDC);
  __ Mov(x1, sp);
  __ Scvalue(csp, c0, x1);
  __ Bind(&csp_is_valid);

  VIXL_ASSERT(__ StackPointer().Is(sp));
  __ SetStackPointer(csp);
  __ Mov(c10, csp);

  // We can only push/pop with a capability stack pointer from C64, since
  // Morello doesn't have alt-base pre- or post-index accesses.
  VIXL_ASSERT(masm.GetISA() == ISA::A64);
  __ Bx();
  {
    ISAScope isa(&masm, ISA::C64);

    // Now we can use any VIXL stack helper, and it will use csp.
    __ Mov(x1, 42);
    __ Mov(x2, -42);
    __ Mov(x3, 420);
    __ Mov(x4, -420);
    __ Push(c0);
    __ Push(x1, x2);
    __ Push(w1, w2, w3, w4);
    __ Claim(64);

    // Measure the stack we've used, so we can check it later.
    __ Mov(c11, csp);
    __ Subs(x11, c10, c11);

    __ Drop(64);
    __ Pop(w24, w23, w22, w21);
    __ Pop(x26, x25);
    __ Pop(c27);
    __ Chkeq(c0, c27);
    __ Mrs(x0, NZCV);
  }
  __ Bx();

  __ Chkeq(csp, c10);
  __ SetStackPointer(sp);

  END();

  if (CAN_RUN()) {
    RUN();
    ASSERT_EQUAL_64(0x00000000fffffe5c, x24); // -420
    ASSERT_EQUAL_64(0x00000000000001a4, x23); //  420
    ASSERT_EQUAL_64(0x00000000ffffffd6, x22); //  -42
    ASSERT_EQUAL_64(0x000000000000002a, x21); //   42
    ASSERT_EQUAL_64(0xffffffffffffffd6, x26); //  -42
    ASSERT_EQUAL_64(0x000000000000002a, x25); //   42
    ASSERT_EQUAL_64(ZFlag, x0); // c27 === c0 (DDC) after push/pop.
    ASSERT_EQUAL_NZCV(ZFlag);   // csp === c10 after the sequence.
    // Check the space used (including Claim/Drop).
    ASSERT_EQUAL_64(112, x11);
  } else {
    DISASSEMBLE();
  }
}

// Test that we can use a capability as a stack pointer, and that csp gets
// bumped to preserve AAPCS64-cap invariants.
TEST(morello_csp_bump) {
  SETUP_WITH_FEATURES(CPUFeatures::kMorello);
  START();

  Label csp_is_valid;
  // Purecap hosts can just use csp directly. Otherwise, we'll need to derive a
  // csp from DDC.
  __ Chktgd(csp);
  __ B(cs, &csp_is_valid);
  __ Mrs(c0, DDC);
  __ Mov(x1, sp);
  __ Scvalue(csp, c0, x1);
  __ Bind(&csp_is_valid);

  VIXL_ASSERT(__ StackPointer().Is(sp));
  __ SetStackPointer(c29);
  __ Mov(c29, csp);
  __ Mov(c10, csp); // For comparison later.

  Register csp_bump_check = x28;
  __ Mov(csp_bump_check, 0);

  // We can only push/pop with a capability stack pointer from C64, since
  // Morello doesn't have "alt-base" pre- or post-index accesses.
  VIXL_ASSERT(masm.GetISA() == ISA::A64);
  __ Bx();
  {
    ISAScope isa(&masm, ISA::C64);

    // Now we can use any VIXL stack helper, and it will use c29 (and bump csp).
    __ Mov(x1, 42);
    __ Mov(x2, -42);
    __ Mov(x3, 420);
    __ Mov(x4, -420);
    __ Push(c0);

    // Check that csp was bumped.
    __ Mov(c12, csp);
    __ Cmp(c29, c12);
    __ Cset(x12, lo);
    __ Orr(csp_bump_check, csp_bump_check, Operand(x12, LSL, 0));

    __ Push(x1, x2);

    // Check that csp was bumped.
    __ Mov(c12, csp);
    __ Cmp(c29, c12);
    __ Cset(x12, lo);
    __ Orr(csp_bump_check, csp_bump_check, Operand(x12, LSL, 1));

    __ Push(w1, w2, w3, w4);

    // Check that csp was bumped.
    __ Mov(c12, csp);
    __ Cmp(c29, c12);
    __ Cset(x12, lo);
    __ Orr(csp_bump_check, csp_bump_check, Operand(x12, LSL, 2));

    __ Claim(64);

    // Check that csp was bumped.
    __ Mov(c12, csp);
    __ Cmp(c29, c12);
    __ Cset(x12, lo);
    __ Orr(csp_bump_check, csp_bump_check, Operand(x12, LSL, 3));

    // Measure the stack we've used, so we can check it later.
    __ Subs(x11, c10, c29);

    __ Drop(64);
    __ Pop(w24, w23, w22, w21);
    __ Pop(x26, x25);
    __ Pop(c27);
    __ Chkeq(c0, c27);
    __ Mrs(x0, NZCV);
  }
  __ Bx();

  __ Chkeq(c29, c10);
  __ Mov(sp, x29);
  __ SetStackPointer(sp);

  END();

  if (CAN_RUN()) {
    RUN();
    ASSERT_EQUAL_64(0x00000000fffffe5c, x24); // -420
    ASSERT_EQUAL_64(0x00000000000001a4, x23); //  420
    ASSERT_EQUAL_64(0x00000000ffffffd6, x22); //  -42
    ASSERT_EQUAL_64(0x000000000000002a, x21); //   42
    ASSERT_EQUAL_64(0xffffffffffffffd6, x26); //  -42
    ASSERT_EQUAL_64(0x000000000000002a, x25); //   42
    ASSERT_EQUAL_64(ZFlag, x0); // c27 === c0 (DDC) after push/pop.
    ASSERT_EQUAL_NZCV(ZFlag);   // csp === c10 after the sequence.
    ASSERT_EQUAL_64(0, csp_bump_check);
    // Check the space used (including Claim/Drop).
    ASSERT_EQUAL_64(112, x11);
  } else {
    DISASSEMBLE();
  }
}

}  // namespace aarch64
}  // namespace vixl
