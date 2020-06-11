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
    ExactAssemblyScope guard(&masm, 2 * kInstructionSize);
    Label a64, c64;

    VIXL_ASSERT(masm.GetISA() == ISA::A64);
    __ bx(&c64);
    __ SetISA(ISA::C64);
    __ bind(&c64);
    __ bx(&a64);
    __ SetISA(ISA::A64);
    __ bind(&a64);
  }

  END();

  if (CAN_RUN()) {
    VIXL_UNIMPLEMENTED();
    // TODO: Check that the ISA was actually switched.
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

}  // namespace aarch64
}  // namespace vixl
