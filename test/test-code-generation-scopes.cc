// Copyright 2016, VIXL authors
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

#include "test-runner.h"

#ifdef VIXL_INCLUDE_TARGET_AARCH64
#include "aarch64/macro-assembler-aarch64.h"
#endif

#define TEST(name)  TEST_(SCOPES_##name)
#define __ masm.

namespace vixl {

// This file contains tests for code generation scopes.

#ifdef VIXL_INCLUDE_TARGET_AARCH64
TEST(CodeBufferCheckScope_basic) {
  aarch64::MacroAssembler masm;

  {
    CodeBufferCheckScope scope(&masm, aarch64::kInstructionSize);
    __ Mov(aarch64::x0, 0);
  }

  masm.FinalizeCode();
}


TEST(CodeBufferCheckScope_Open) {
  aarch64::MacroAssembler masm;

  {
    CodeBufferCheckScope scope;
    __ Mov(aarch64::x0, 0);
    scope.Open(&masm, aarch64::kInstructionSize);
    __ Mov(aarch64::x1, 1);
  }

  masm.FinalizeCode();
}


TEST(CodeBufferCheckScope_Close) {
  aarch64::MacroAssembler masm;

  {
    CodeBufferCheckScope scope(&masm, aarch64::kInstructionSize);
    __ Mov(aarch64::x0, 0);
    scope.Close();
    __ Mov(aarch64::x1, 1);
  }

  masm.FinalizeCode();
}


TEST(CodeBufferCheckScope_Open_Close) {
  aarch64::MacroAssembler masm;

  {
    CodeBufferCheckScope scope;
    __ Mov(aarch64::x0, 0);
    scope.Open(&masm, aarch64::kInstructionSize);
    __ Mov(aarch64::x1, 1);
    scope.Close();
    __ Mov(aarch64::x2, 2);
  }

  masm.FinalizeCode();
}


TEST(EmissionCheckScope_basic) {
  aarch64::MacroAssembler masm;

  {
    EmissionCheckScope scope(&masm, aarch64::kInstructionSize);
    __ Mov(aarch64::x0, 0);
  }

  masm.FinalizeCode();
}


TEST(EmissionCheckScope_Open) {
  aarch64::MacroAssembler masm;

  {
    EmissionCheckScope scope;
    __ Mov(aarch64::x0, 0);
    scope.Open(&masm, aarch64::kInstructionSize);
    __ Mov(aarch64::x1, 1);
  }

  masm.FinalizeCode();
}


TEST(EmissionCheckScope_Close) {
  aarch64::MacroAssembler masm;

  {
    EmissionCheckScope scope(&masm, aarch64::kInstructionSize);
    __ Mov(aarch64::x0, 0);
    scope.Close();
    __ Mov(aarch64::x1, 1);
  }

  masm.FinalizeCode();
}


TEST(EmissionCheckScope_Open_Close) {
  aarch64::MacroAssembler masm;

  {
    EmissionCheckScope scope;
    __ Mov(aarch64::x0, 0);
    scope.Open(&masm, aarch64::kInstructionSize);
    __ Mov(aarch64::x1, 1);
    scope.Close();
    __ Mov(aarch64::x2, 2);
  }

  masm.FinalizeCode();
}


#define ASSERT_LITERAL_POOL_SIZE(expected)                                     \
  VIXL_CHECK(                                                                  \
      (expected + aarch64::kInstructionSize) == (masm.GetLiteralPoolSize()))

TEST(EmissionCheckScope_emit_pool) {
  aarch64::MacroAssembler masm;

  // Make sure the pool is empty;
  masm.EmitLiteralPool(aarch64::LiteralPool::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  __ Ldr(aarch64::x0, 0x1234567890abcdef);
  ASSERT_LITERAL_POOL_SIZE(8);

  {
    // Check that opening the scope with a reserved space well below the limit
    // at which can generate the literal pool does not force the emission of
    // the pool.
    EmissionCheckScope scope(&masm,
                             10 * aarch64::kInstructionSize,
                             EmissionCheckScope::kMaximumSize);
    ASSERT_LITERAL_POOL_SIZE(8);
  }

  {
    // Check that the scope forces emission of the pool if necessary.
    EmissionCheckScope scope(&masm,
                             aarch64::kMaxLoadLiteralRange + 1,
                             EmissionCheckScope::kMaximumSize);
    ASSERT_LITERAL_POOL_SIZE(0);
  }

  masm.FinalizeCode();
}


TEST(EmissionCheckScope_emit_pool_on_Open) {
  aarch64::MacroAssembler masm;

  // Make sure the pool is empty;
  masm.EmitLiteralPool(aarch64::LiteralPool::kBranchRequired);
  ASSERT_LITERAL_POOL_SIZE(0);

  __ Ldr(aarch64::x0, 0x1234567890abcdef);
  ASSERT_LITERAL_POOL_SIZE(8);

  {
    // Check that opening the scope with a reserved space well below the limit
    // at which can generate the literal pool does not force the emission of
    // the pool.
    EmissionCheckScope scope;
    scope.Open(&masm,
               10 * aarch64::kInstructionSize,
               EmissionCheckScope::kMaximumSize);
    ASSERT_LITERAL_POOL_SIZE(8);
  }

  {
    // Check that the scope forces emission of the pool if necessary.
    EmissionCheckScope scope;
    scope.Open(&masm,
               aarch64::kMaxLoadLiteralRange + 1,
               EmissionCheckScope::kMaximumSize);
    ASSERT_LITERAL_POOL_SIZE(0);
  }

  masm.FinalizeCode();
}
#endif  // VIXL_INCLUDE_TARGET_AARCH64


}  // namespace vixl

