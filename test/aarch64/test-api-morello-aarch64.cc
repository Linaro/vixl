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

#include <cstdio>
#include <cstring>
#include <string>

#include "test-runner.h"
#include "test-utils.h"
#include "aarch64/test-utils-aarch64.h"

#include "aarch64/macro-assembler-aarch64.h"
#include "aarch64/registers-aarch64.h"

#define __ masm.
#define TEST(name) TEST_(AARCH64_API_##name)


namespace vixl {
namespace aarch64 {

TEST(morello_c_registers) {
  VIXL_CHECK(c0.GetCode() == 0);
  VIXL_CHECK(c31.GetCode() == 31);

  VIXL_CHECK(c0.Is(c0));
  VIXL_CHECK(!c0.Is(c1));
  VIXL_CHECK(!c0.Is(x0));
  VIXL_CHECK(!c0.Is(w0));
  VIXL_CHECK(!c0.Is(v0));
  VIXL_CHECK(!c0.Is(q0));
  VIXL_CHECK(c0.Is(x0.C()));
  VIXL_CHECK(c0.Is(w0.C()));

  VIXL_CHECK(x0.WithSizeInBits(kCRegSize).Is(c0));
  VIXL_CHECK(x0.WithSizeInBytes(kCRegSizeInBytes).Is(c0));
  VIXL_CHECK(x0.WithSameSizeAs(c12).Is(c0));
  VIXL_CHECK(w0.WithSizeInBits(kCRegSize).Is(c0));
  VIXL_CHECK(w0.WithSizeInBytes(kCRegSizeInBytes).Is(c0));
  VIXL_CHECK(w0.WithSameSizeAs(c12).Is(c0));

  VIXL_CHECK(AreAliased(c5, c5));
  VIXL_CHECK(AreAliased(c5, w5));
  VIXL_CHECK(AreAliased(c5, x5));
  VIXL_CHECK(AreAliased(w5, c5));
  VIXL_CHECK(AreAliased(x5, c5));

  VIXL_CHECK(!AreAliased(c6, c7));
  VIXL_CHECK(!AreAliased(w6, c7));
  VIXL_CHECK(!AreAliased(v7, c7));
}

TEST(morello_isa) {
  class ObjectWithISA {
   public:
    explicit ObjectWithISA(ISA isa) : isa_(isa) {}
    void SetISA(ISA isa) { isa_ = isa; }
    ISA GetISA() const { return isa_; }

   private:
    ISA isa_;
  };

  ObjectWithISA obj(ISA::A64);
  VIXL_CHECK(obj.GetISA() == ISA::A64);
  {
    ISAScope outer(&obj, ISA::C64);
    VIXL_CHECK(obj.GetISA() == ISA::C64);

    {
      ISAScope inner(&obj, ISA::A64);
      VIXL_CHECK(obj.GetISA() == ISA::A64);
    }
    VIXL_CHECK(obj.GetISA() == ISA::C64);

    // The ISA can be manually changed within a scope.
    obj.SetISA(ISA::Data);

    {
      ISAScope inner(&obj, ISA::Data);
      // If the inner scope has the same ISA, it has no effect.
      VIXL_CHECK(obj.GetISA() == ISA::Data);
      // If the ISA is manually changed within the scope, the changes are
      // reverted on exit.
      obj.SetISA(ISA::A64);
      VIXL_CHECK(obj.GetISA() == ISA::A64);
    }
    VIXL_CHECK(obj.GetISA() == ISA::Data);
  }
  VIXL_CHECK(obj.GetISA() == ISA::A64);
}

TEST(morello_scratch_scope_basic) {
  MacroAssembler masm;
  // c16 and c17 are available as scratch registers by default.
  {
    UseScratchRegisterScope temps(&masm);
    CRegister temp1 = temps.AcquireC();
    CRegister temp2 = temps.AcquireC();
    VIXL_CHECK(temp1.Is(c16));
    VIXL_CHECK(temp2.Is(c17));
  }
  {
    UseScratchRegisterScope temps(&masm);
    CPURegister temp1 = temps.AcquireCPURegisterOfSize(kCRegSize);
    CPURegister temp2 = temps.AcquireCPURegisterOfSize(kCRegSize);
    VIXL_CHECK(temp1.Is(c16));
    VIXL_CHECK(temp2.Is(c17));
  }
}

TEST(morello_scratch_scope_overlap) {
  MacroAssembler masm;
  // c16 and c17 are aliased with x16 and x17. Check that we can't acquire
  // overlapping registers.
  {
    UseScratchRegisterScope temps(&masm);
    Register temp1 = temps.AcquireX();
    CRegister temp2 = temps.AcquireC();
    VIXL_CHECK(!AreAliased(temp1, temp2));
  }
  {
    UseScratchRegisterScope temps(&masm);
    CRegister temp1 = temps.AcquireC();
    Register temp2 = temps.AcquireX();
    VIXL_CHECK(!AreAliased(temp1, temp2));
  }
}

TEST(morello_memoperand_base) {
  VIXL_CHECK(MemOperand(c0).GetBase().Is(c0));
  VIXL_CHECK(MemOperand(x0).GetBase().Is(x0));
  VIXL_CHECK(!MemOperand(c0).GetBase().Is(x0));
  VIXL_CHECK(!MemOperand(x0).GetBase().Is(c0));

  VIXL_CHECK(MemOperand(c0).GetBaseCRegister().Is(c0));
  VIXL_CHECK(MemOperand(x0).GetBaseRegister().Is(x0));

  VIXL_CHECK(MemOperand(c0, 42).GetBase().Is(c0));
  VIXL_CHECK(MemOperand(x0, 42).GetBase().Is(x0));
  VIXL_CHECK(!MemOperand(c0, 42).GetBase().Is(x0));
  VIXL_CHECK(!MemOperand(x0, 42).GetBase().Is(c0));

  VIXL_CHECK(MemOperand(c0, 42).GetBaseCRegister().Is(c0));
  VIXL_CHECK(MemOperand(x0, 42).GetBaseRegister().Is(x0));
}

TEST(morello_memoperand_alt_base) {
  VIXL_CHECK(!MemOperand(x0).IsAltBase(ISA::A64));
  VIXL_CHECK(MemOperand(x0).IsAltBase(ISA::C64));
  VIXL_CHECK(MemOperand(c0).IsAltBase(ISA::A64));
  VIXL_CHECK(!MemOperand(c0).IsAltBase(ISA::C64));

  VIXL_CHECK(!MemOperand(x0, 42).IsAltBase(ISA::A64));
  VIXL_CHECK(MemOperand(x0, 42).IsAltBase(ISA::C64));
  VIXL_CHECK(MemOperand(c0, 42).IsAltBase(ISA::A64));
  VIXL_CHECK(!MemOperand(c0, 42).IsAltBase(ISA::C64));
}

}  // namespace aarch64
}  // namespace vixl
