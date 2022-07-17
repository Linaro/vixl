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


// -----------------------------------------------------------------------------
// This file is auto generated from the
// test/aarch32/config/template-simulator-aarch32.cc.in template file using
// tools/generate_tests.py.
//
// PLEASE DO NOT EDIT.
// -----------------------------------------------------------------------------


#include "test-runner.h"

#include "test-utils.h"
#include "test-utils-aarch32.h"

#include "aarch32/assembler-aarch32.h"
#include "aarch32/disasm-aarch32.h"
#include "aarch32/macro-assembler-aarch32.h"

#define __ masm.
#define BUF_SIZE (4096)

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH32
// Run tests with the simulator.

#define SETUP() MacroAssembler masm(BUF_SIZE)

#define START() masm.GetBuffer()->Reset()

#define END() \
  __ Hlt(0);  \
  __ FinalizeCode();

// TODO: Run the tests in the simulator.
#define RUN()

#else  // ifdef VIXL_INCLUDE_SIMULATOR_AARCH32.

#define SETUP()                  \
  MacroAssembler masm(BUF_SIZE); \
  UseScratchRegisterScope harness_scratch;

#define START()                 \
  harness_scratch.Open(&masm);  \
  harness_scratch.ExcludeAll(); \
  masm.GetBuffer()->Reset();    \
  __ Push(r4);                  \
  __ Push(r5);                  \
  __ Push(r6);                  \
  __ Push(r7);                  \
  __ Push(r8);                  \
  __ Push(r9);                  \
  __ Push(r10);                 \
  __ Push(r11);                 \
  __ Push(lr);                  \
  harness_scratch.Include(ip);

#define END()                  \
  harness_scratch.Exclude(ip); \
  __ Pop(lr);                  \
  __ Pop(r11);                 \
  __ Pop(r10);                 \
  __ Pop(r9);                  \
  __ Pop(r8);                  \
  __ Pop(r7);                  \
  __ Pop(r6);                  \
  __ Pop(r5);                  \
  __ Pop(r4);                  \
  __ Bx(lr);                   \
  __ FinalizeCode();           \
  harness_scratch.Close();

#define RUN()                                                 \
  {                                                           \
    int pcs_offset = masm.IsUsingT32() ? 1 : 0;               \
    masm.GetBuffer()->SetExecutable();                        \
    ExecuteMemory(masm.GetBuffer()->GetStartAddress<byte*>(), \
                  masm.GetSizeOfCodeGenerated(),              \
                  pcs_offset);                                \
    masm.GetBuffer()->SetWritable();                          \
  }

#endif  // ifdef VIXL_INCLUDE_SIMULATOR_AARCH32

namespace vixl {
namespace aarch32 {

// List of instruction encodings:
#define FOREACH_INSTRUCTION(M) \
  M(Mul)                       \
  M(Qadd16)                    \
  M(Qadd8)                     \
  M(Qasx)                      \
  M(Qsax)                      \
  M(Qsub16)                    \
  M(Qsub8)                     \
  M(Sdiv)                      \
  M(Shadd16)                   \
  M(Shadd8)                    \
  M(Shasx)                     \
  M(Shsax)                     \
  M(Shsub16)                   \
  M(Shsub8)                    \
  M(Smmul)                     \
  M(Smmulr)                    \
  M(Smuad)                     \
  M(Smuadx)                    \
  M(Smulbb)                    \
  M(Smulbt)                    \
  M(Smultb)                    \
  M(Smultt)                    \
  M(Smulwb)                    \
  M(Smulwt)                    \
  M(Smusd)                     \
  M(Smusdx)                    \
  M(Udiv)                      \
  M(Uhadd16)                   \
  M(Uhadd8)                    \
  M(Uhasx)                     \
  M(Uhsax)                     \
  M(Uhsub16)                   \
  M(Uhsub8)                    \
  M(Uqadd16)                   \
  M(Uqadd8)                    \
  M(Uqasx)                     \
  M(Uqsax)                     \
  M(Uqsub16)                   \
  M(Uqsub8)                    \
  M(Usad8)                     \
  M(Sadd16)                    \
  M(Sadd8)                     \
  M(Sasx)                      \
  M(Sel)                       \
  M(Ssax)                      \
  M(Ssub16)                    \
  M(Ssub8)                     \
  M(Uadd16)                    \
  M(Uadd8)                     \
  M(Uasx)                      \
  M(Usax)                      \
  M(Usub16)                    \
  M(Usub8)                     \
  M(Qadd)                      \
  M(Qdadd)                     \
  M(Qdsub)                     \
  M(Qsub)


// The following definitions are defined again in each generated test, therefore
// we need to place them in an anonymous namespace. It expresses that they are
// local to this file only, and the compiler is not allowed to share these types
// across test files during template instantiation. Specifically, `Operands` and
// `Inputs` have various layouts across generated tests so they absolutely
// cannot be shared.

#ifdef VIXL_INCLUDE_TARGET_T32
namespace {

// Values to be passed to the assembler to produce the instruction under test.
struct Operands {
  Condition cond;
  Register rd;
  Register rn;
  Register rm;
};

// Input data to feed to the instruction.
struct Inputs {
  uint32_t apsr;
  uint32_t qbit;
  uint32_t ge;
  uint32_t rd;
  uint32_t rn;
  uint32_t rm;
};

// This structure contains all input data needed to test one specific encoding.
// It used to generate a loop over an instruction.
struct TestLoopData {
  // The `operands` fields represents the values to pass to the assembler to
  // produce the instruction.
  Operands operands;
  // Description of the operands, used for error reporting.
  const char* operands_description;
  // Unique identifier, used for generating traces.
  const char* identifier;
  // Array of values to be fed to the instruction.
  size_t input_size;
  const Inputs* inputs;
};

static const Inputs kCondition[] =
    {{NFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {ZFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {CFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {VFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {NZFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {NCFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {NVFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {ZCFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {ZVFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {CVFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {NZCFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {NZVFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {NCVFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {ZCVFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab},
     {NZCVFlag, NoFlag, NoFlag, 0xabababab, 0xabababab, 0xabababab}};

static const Inputs kRdIsRnIsRm[] =
    {{NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000000, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x00000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x00000002, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00000020, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x0000007d, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x0000007e, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x0000007f, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x00007ffd, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00007ffe, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007fff, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x33333333, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x55555555, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x7ffffffd, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x7ffffffe, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x7fffffff, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x80000000, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x80000001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xcccccccc, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0xffff8000, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xffff8001, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xffff8002, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xffff8003, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffffff80, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0xffffff81, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xffffff83, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffffffe0, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xfffffffd, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xfffffffe, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0xffffffff, 0xffffffff}};

static const Inputs kRdIsRn[] =
    {{NoFlag, NoFlag, NoFlag, 0xffff8003, 0xffff8003, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x00000001, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xffff8002, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xfffffffe, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xaaaaaaaa, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x00007ffd, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007fff, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00000020, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x7ffffffe, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x7ffffffe, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x0000007d, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x7ffffffe, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00000020, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x0000007d, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xffffff83, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffffffe0, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x55555555, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0xffff8000, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x00000001, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00000020, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x80000000, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xffff8003, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x80000000, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00000020, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x00000001, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x7ffffffe, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x7ffffffd, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00000020, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xffff8003, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xaaaaaaaa, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x00007ffd, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x80000000, 0xffffffff},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007fff, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffffff80, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x00000001, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x7ffffffd, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xffff8003, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007fff, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x80000000, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xfffffffe, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xfffffffe, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007fff, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x80000000, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x0000007f, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffffffe0, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x00007ffd, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xffff8002, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x80000001, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0xffffff81, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x00000002, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xaaaaaaaa, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00007ffe, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x7ffffffd, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffffff80, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffffff80, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xaaaaaaaa, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x0000007f, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x33333333, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xaaaaaaaa, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0xffff8000, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xfffffffd, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x80000001, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x00007ffd, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x7ffffffd, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x80000000, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0xffff8000, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x33333333, 0xffffffff},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0xffffffff, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x55555555, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000000, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00000020, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007fff, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x7fffffff, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0xffffff81, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xfffffffd, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x7ffffffe, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x00000002, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0xffff8000, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00007ffe, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x0000007d, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x55555555, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0xffffffff, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x0000007e, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000000, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xffff8002, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x7ffffffe, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xffffff83, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00007ffe, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x33333333, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x0000007e, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xcccccccc, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x55555555, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x0000007f, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x7ffffffe, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xfffffffe, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x33333333, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x00000001, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x0000007d, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0xffff8000, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xfffffffd, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0xffffffff, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xfffffffe, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffffffe0, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00007ffe, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0xffffff81, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xfffffffe, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xcccccccc, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x80000001, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xfffffffe, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x7fffffff, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x7fffffff, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xffff8001, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xffffff83, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000000, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x7ffffffe, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000000, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x7ffffffd, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x00000001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0xffffff81, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x80000001, 0xffffffff},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x7fffffff, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0xffffffff, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0xffff8000, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x80000001, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xffffff83, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0xffffff81, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffffff80, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x33333333, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x7ffffffd, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xffff8001, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x80000001, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x0000007d, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x55555555, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x0000007f, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xffff8001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xffff8001, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0xffffffff, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007fff, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xffff8001, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00000020, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x7fffffff, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x7ffffffd, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x80000000, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x55555555, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xffff8001, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x80000000, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x00000002, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffffff80, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0xffffffff, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x00000002, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xcccccccc, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xffff8002, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0xffff8000, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x00000001, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffffffe0, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00007ffe, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x00007ffd, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x0000007f, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000000, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xfffffffe, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000000, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x33333333, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffffffe0, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xfffffffd, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffffffe0, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xffffff83, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x55555555, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xfffffffd, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00007ffe, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x80000000, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00000020, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x0000007d, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007fff, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x80000001, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x00007ffd, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffffff80, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x0000007d, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xffffff83, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x00007ffd, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x33333333, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x0000007d, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x80000001, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xaaaaaaaa, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xffff8003, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffffffe0, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x80000000, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x0000007e, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000000, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xffff8002, 0xffffff81}};

static const Inputs kRdIsRm[] =
    {{NoFlag, NoFlag, NoFlag, 0x00000020, 0x0000007f, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x00000020, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00007ffd, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0x7ffffffd, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xffff8000, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0xfffffffd, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0x80000000, 0xffffffff},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0xffffff82, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0xfffffffe, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x55555555, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0xfffffffe, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0x80000000, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffff8003, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x7ffffffe, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007fff, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0xffffff80, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0x00000020, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0xffff8002, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0xffffff80, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x00000020, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0xffffff81, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0xffffff82, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0xffffff82, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0xffffff83, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x55555555, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0x00000001, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x7ffffffe, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0xffff8001, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x00000002, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0xaaaaaaaa, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0x00000020, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xfffffffe, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xfffffffd, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x00007ffe, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xcccccccc, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0x7ffffffe, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0xffffffff, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0xaaaaaaaa, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x0000007d, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xaaaaaaaa, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0xaaaaaaaa, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x55555555, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x0000007e, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x33333333, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xffffff80, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00007ffe, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x55555555, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffff8001, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0x00000002, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0xaaaaaaaa, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0x0000007f, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0x7fffffff, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0xffff8001, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0xffff8003, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0xaaaaaaaa, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0xaaaaaaaa, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xcccccccc, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xffffff81, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xfffffffe, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0xcccccccc, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00000020, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0x00007ffe, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0x0000007d, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xfffffffe, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xaaaaaaaa, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x0000007f, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x00000001, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0xffff8000, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xffffffff, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0xffffff83, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0xffff8000, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0xffff8000, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0xffffffff, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xffff8000, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0xffffffe0, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0x7ffffffe, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0xfffffffe, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x00000000, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x0000007f, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0x7ffffffd, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x80000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x00007ffd, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xaaaaaaaa, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0x00000000, 0xffffffff},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffffff82, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xaaaaaaaa, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffff8000, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xffffffe0, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x00007ffd, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x00007fff, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xfffffffd, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x7ffffffd, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x00000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xfffffffe, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0x00007fff, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0x00000020, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x0000007d, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0xffff8002, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0x00000000, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0x0000007f, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0x00007ffe, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xffff8003, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x80000001, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0x00007ffd, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0x00007ffe, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0x80000001, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x00007ffe, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x7ffffffd, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xfffffffd, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x7ffffffd, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xffffff82, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x80000000, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xffffffff, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0xffffff81, 0xffffffff},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00000020, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0x00000002, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xffffffe0, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0xffffff83, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x33333333, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0xffffff82, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0x55555555, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x00007ffd, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0x00007ffd, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xffffff80, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x00000001, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00007ffe, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xffffffff, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0x00000002, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x7fffffff, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007ffd, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x80000001, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0xffffff82, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x00000020, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x00007fff, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0xffffff81, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x7ffffffe, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xaaaaaaaa, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0x00000001, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000001, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xfffffffd, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xffff8002, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x00000020, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x0000007f, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x0000007f, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xffffff83, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xcccccccc, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xffffffff, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xcccccccc, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0xffffff83, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0x00007ffe, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x00007ffe, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x0000007f, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00007ffd, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x00000001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffff8003, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0xffff8003, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0xffffff82, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x7ffffffe, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0x7ffffffd, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xffff8001, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0x7fffffff, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x80000001, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xffffffff, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x0000007e, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xffffff80, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0x7ffffffd, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffffe0, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0xaaaaaaaa, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0x0000007e, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0x0000007f, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x0000007f, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x80000000, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x00000000, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0xffffff80, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0x33333333, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0xffffff81, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xaaaaaaaa, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x0000007f, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x7ffffffd, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x33333333, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0xfffffffe, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0x0000007d, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0x80000000, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0xffffff81, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x7ffffffe, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0xffffff80, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x00000001, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000000, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xffffff82, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0xffff8001, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007ffe, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0x00007fff, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0xffff8003, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x80000001, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0x7fffffff, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffff8000, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0xffffff82, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0xcccccccc, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0x7fffffff, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0x00007fff, 0xffff8003}};

static const Inputs kRnIsRm[] =
    {{NoFlag, NoFlag, NoFlag, 0xffffff82, 0x00000002, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x0000007f, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x0000007d, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x00000020, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0xcccccccc, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xffffffe0, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x00000000, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0xffffffff, 0xffffffff},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0xffffffe0, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x00000002, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xcccccccc, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x80000000, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0x33333333, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00007ffd, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xaaaaaaaa, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0x7ffffffd, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0xfffffffd, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0x00000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000002, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0xffff8001, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xfffffffd, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xffff8000, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x7fffffff, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0xffffff83, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x00000002, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x0000007f, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0x00000020, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffff8001, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0xffffff83, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0xffffff81, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x00000002, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xffff8000, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x00000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0x7ffffffe, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x00000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x7ffffffd, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x00000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x55555555, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x80000001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff82, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xffff8001, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xffffff80, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x7ffffffe, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffffff83, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x7ffffffe, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xffff8002, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x80000000, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0x00007ffe, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0xaaaaaaaa, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x00000020, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x7ffffffe, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x7ffffffe, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xffff8000, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x7fffffff, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0x00000020, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0xffffffe0, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0xcccccccc, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0xaaaaaaaa, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x33333333, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0xffff8002, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0xffffffe0, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0xffff8000, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xffffff80, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x00000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x80000001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00000002, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x0000007d, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0xfffffffd, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xaaaaaaaa, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0xffffff83, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x00000002, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x0000007f, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xfffffffe, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xffffffff, 0xffffffff},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0xffff8002, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x55555555, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0xcccccccc, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0x00000020, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x0000007e, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0xfffffffe, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x55555555, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0x0000007f, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0x7fffffff, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0x7ffffffe, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0x7fffffff, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x7ffffffd, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x33333333, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xfffffffd, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x0000007e, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffffff82, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x0000007f, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x7ffffffd, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffffe0, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0xffff8001, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0xffffff82, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0x0000007f, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x00000020, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xffff8002, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0x00007fff, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x7ffffffe, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0x00007ffe, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0xffffff82, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xffff8002, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x00007ffd, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0xffff8003, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xffff8003, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xffffff83, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x33333333, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007ffe, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0x00007ffd, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0xffffff80, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xffffffe0, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x7ffffffd, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0x00000000, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00007fff, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x00000002, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x00007ffe, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0xfffffffe, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xffffff83, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0xffffff80, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xffff8002, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0xffffffe0, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0x0000007f, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x80000001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x00000000, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0x7ffffffe, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0xffff8002, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0x7fffffff, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0x0000007e, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0x00007fff, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0x33333333, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xaaaaaaaa, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xaaaaaaaa, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x7ffffffd, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x0000007f, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xffff8001, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0xfffffffd, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0x0000007d, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x0000007d, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0xaaaaaaaa, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0x0000007d, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0x00000020, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00000000, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0x0000007d, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xfffffffe, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0xffff8001, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0xffffff80, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x80000001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0x0000007f, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0xffffff81, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0xfffffffe, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0xcccccccc, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0x80000001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0xffff8002, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x7fffffff, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffff8003, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x00007ffe, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x80000001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0x00007ffe, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xfffffffd, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xffff8003, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x00000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x33333333, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xffffff81, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0x00007fff, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xfffffffe, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0x80000001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xaaaaaaaa, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0x55555555, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0xffffff83, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0x00000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x00000000, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x0000007d, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xffffff83, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0x00007ffd, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0xffffff81, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0xffff8000, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0xffffffff, 0xffffffff},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x80000000, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffff8001, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0x00007fff, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0xffffff81, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xfffffffd, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x33333333, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xffff8002, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0x0000007e, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0xffffff80, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0xffffff80, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x00000020, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0xffffff81, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x80000001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x7fffffff, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00000002, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x80000001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0xfffffffd, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x80000000, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x00000020, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffff8002, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0x0000007e, 0x0000007e}};

static const Inputs kRdIsNotRnIsNotRm[] =
    {{NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xffff8001, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xffff8003, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x00007fff, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0xffffff80, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0x33333333, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x0000007f, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0xffffffff, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0xfffffffe, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffff8001, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0x0000007e, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x0000007e, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xffffff81, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0x00007ffd, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0xcccccccc, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x00000020, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x00000002, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0x0000007f, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00007ffd, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00007ffd, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0xfffffffe, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x00007fff, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x00007fff, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x80000000, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0x00000001, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x55555555, 0xffffffff},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0xffffffff, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xffff8003, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0xffffff82, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x00000002, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0xffffff80, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0x00007ffd, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xffff8002, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x00000000, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x7ffffffe, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x00007ffd, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x80000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0x00000001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffff8001, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x33333333, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0xffffffe0, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0x7ffffffe, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xcccccccc, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0x33333333, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x7ffffffe, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffffff80, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x0000007e, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x00000020, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x00000001, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xcccccccc, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xaaaaaaaa, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0x0000007f, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xffffffff, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0xffffff81, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0x0000007d, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xaaaaaaaa, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x00000002, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0x00000001, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00000000, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xffffff80, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x0000007e, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0xaaaaaaaa, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0x80000000, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x00007ffd, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0xffffff80, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xffff8000, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xaaaaaaaa, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x7ffffffe, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x00000002, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xfffffffd, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0xffffff83, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0xffff8001, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00007fff, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xfffffffe, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x80000000, 0xffffffff},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0xfffffffd, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x7fffffff, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xffffffe0, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x00000001, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0xffffff82, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0xffff8003, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0x33333333, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0xffffffff, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x0000007f, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0xffff8003, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0xfffffffd, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0xfffffffe, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0xffffffe0, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xffffff82, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffffff81, 0x00007ffd, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0x00000002, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x00007ffd, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x7fffffff, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xfffffffe, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xffff8003, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0xaaaaaaaa, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x00000020, 0xffffffe0},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0x0000007d, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x80000001, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0xffffffff, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x00000000, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x80000000, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0x55555555, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xffff8003, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0x80000001, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xffffff82, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0x00000000, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0x0000007e, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0x7fffffff, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xaaaaaaaa, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xffffff80, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0x00000001, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x00007ffe, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0x00000000, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xffffff82, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0xaaaaaaaa, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x80000000, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x55555555, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0xffffff81, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffffe0, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0x00000001, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffffff80, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xcccccccc, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0xffff8001, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xffff8001, 0xaaaaaaaa},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0xfffffffe, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0xcccccccc, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0x00007ffd, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xfffffffd, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0x00000001, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0x7ffffffd, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0xffff8001, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x00007ffe, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x00000001, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0x33333333, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffff8002, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0xffffff82, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0x0000007f, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x00000001, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x00007fff, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0xffff8000, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0xcccccccc, 0xffffff81, 0x7ffffffd},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xffff8003, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0x7fffffff, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x00000020, 0x80000000, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x7ffffffe, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x00000020, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xffffff81, 0x80000000},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0xffff8001, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x7ffffffe, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x80000000, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xaaaaaaaa, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xffffffff, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0xffff8001, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffd, 0x0000007f, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0x0000007f, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0xaaaaaaaa, 0xffff8000, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xfffffffe, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0x7fffffff, 0xfffffffd, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0xffffffff, 0x00007fff},
     {NoFlag, NoFlag, NoFlag, 0x0000007d, 0xcccccccc, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0xffffff80, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0x0000007e, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0xffffff82, 0xffffff80, 0xffff8000},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x0000007d, 0xfffffffe},
     {NoFlag, NoFlag, NoFlag, 0xfffffffe, 0xffffff82, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x55555555, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0xffff8001, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0xffffffe0, 0x33333333},
     {NoFlag, NoFlag, NoFlag, 0x55555555, 0x00000001, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0xffffffff, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x00007fff, 0x00000000, 0x7fffffff},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0x0000007d, 0x7ffffffe},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0x0000007e, 0xffff8001},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0x00007fff, 0xffffff83},
     {NoFlag, NoFlag, NoFlag, 0x0000007e, 0x0000007d, 0x00000020},
     {NoFlag, NoFlag, NoFlag, 0x80000000, 0xffffff81, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0xffffffff, 0xffff8002, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0x0000007f, 0x55555555, 0x0000007f},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x00000020, 0x00007ffd},
     {NoFlag, NoFlag, NoFlag, 0xffff8001, 0xffffff80, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x7ffffffe, 0x00007ffd, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0xffffff83, 0x7ffffffe, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0xffff8001, 0x80000001},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0xffff8003, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0x80000001, 0xffff8003, 0xffffff81},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0x0000007d, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x00007ffd, 0x0000007f, 0xffff8003},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0xffffffff, 0x00000000},
     {NoFlag, NoFlag, NoFlag, 0xffff8000, 0x00007fff, 0x55555555},
     {NoFlag, NoFlag, NoFlag, 0x33333333, 0xffffff82, 0x00007ffe},
     {NoFlag, NoFlag, NoFlag, 0xffffff80, 0x7ffffffe, 0x0000007e},
     {NoFlag, NoFlag, NoFlag, 0xffff8002, 0xffffffff, 0x00000002},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0x00007ffd, 0xffffff80},
     {NoFlag, NoFlag, NoFlag, 0x00007ffe, 0x00000002, 0xcccccccc},
     {NoFlag, NoFlag, NoFlag, 0x00000002, 0xffffff81, 0xfffffffd},
     {NoFlag, NoFlag, NoFlag, 0xffffffe0, 0x0000007d, 0x00000001},
     {NoFlag, NoFlag, NoFlag, 0x00000000, 0xffffff82, 0xffffff82},
     {NoFlag, NoFlag, NoFlag, 0xfffffffd, 0x00000020, 0xffff8002},
     {NoFlag, NoFlag, NoFlag, 0xffff8003, 0xffffff81, 0x0000007d},
     {NoFlag, NoFlag, NoFlag, 0x00000001, 0x7fffffff, 0x55555555}};


// A loop will be generated for each element of this array.
const TestLoopData kTests[] = {{{eq, r0, r0, r0},
                                "eq r0 r0 r0",
                                "Condition_eq_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{ne, r0, r0, r0},
                                "ne r0 r0 r0",
                                "Condition_ne_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{cs, r0, r0, r0},
                                "cs r0 r0 r0",
                                "Condition_cs_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{cc, r0, r0, r0},
                                "cc r0 r0 r0",
                                "Condition_cc_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{mi, r0, r0, r0},
                                "mi r0 r0 r0",
                                "Condition_mi_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{pl, r0, r0, r0},
                                "pl r0 r0 r0",
                                "Condition_pl_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{vs, r0, r0, r0},
                                "vs r0 r0 r0",
                                "Condition_vs_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{vc, r0, r0, r0},
                                "vc r0 r0 r0",
                                "Condition_vc_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{hi, r0, r0, r0},
                                "hi r0 r0 r0",
                                "Condition_hi_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{ls, r0, r0, r0},
                                "ls r0 r0 r0",
                                "Condition_ls_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{ge, r0, r0, r0},
                                "ge r0 r0 r0",
                                "Condition_ge_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{lt, r0, r0, r0},
                                "lt r0 r0 r0",
                                "Condition_lt_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{gt, r0, r0, r0},
                                "gt r0 r0 r0",
                                "Condition_gt_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{le, r0, r0, r0},
                                "le r0 r0 r0",
                                "Condition_le_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{al, r0, r0, r0},
                                "al r0 r0 r0",
                                "Condition_al_r0_r0_r0",
                                ARRAY_SIZE(kCondition),
                                kCondition},
                               {{al, r0, r0, r0},
                                "al r0 r0 r0",
                                "RdIsRnIsRm_al_r0_r0_r0",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r1, r1, r1},
                                "al r1 r1 r1",
                                "RdIsRnIsRm_al_r1_r1_r1",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r2, r2, r2},
                                "al r2 r2 r2",
                                "RdIsRnIsRm_al_r2_r2_r2",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r3, r3, r3},
                                "al r3 r3 r3",
                                "RdIsRnIsRm_al_r3_r3_r3",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r4, r4, r4},
                                "al r4 r4 r4",
                                "RdIsRnIsRm_al_r4_r4_r4",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r5, r5, r5},
                                "al r5 r5 r5",
                                "RdIsRnIsRm_al_r5_r5_r5",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r6, r6, r6},
                                "al r6 r6 r6",
                                "RdIsRnIsRm_al_r6_r6_r6",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r7, r7, r7},
                                "al r7 r7 r7",
                                "RdIsRnIsRm_al_r7_r7_r7",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r8, r8, r8},
                                "al r8 r8 r8",
                                "RdIsRnIsRm_al_r8_r8_r8",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r9, r9, r9},
                                "al r9 r9 r9",
                                "RdIsRnIsRm_al_r9_r9_r9",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r10, r10, r10},
                                "al r10 r10 r10",
                                "RdIsRnIsRm_al_r10_r10_r10",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r11, r11, r11},
                                "al r11 r11 r11",
                                "RdIsRnIsRm_al_r11_r11_r11",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r12, r12, r12},
                                "al r12 r12 r12",
                                "RdIsRnIsRm_al_r12_r12_r12",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r14, r14, r14},
                                "al r14 r14 r14",
                                "RdIsRnIsRm_al_r14_r14_r14",
                                ARRAY_SIZE(kRdIsRnIsRm),
                                kRdIsRnIsRm},
                               {{al, r14, r14, r3},
                                "al r14 r14 r3",
                                "RdIsRn_al_r14_r14_r3",
                                ARRAY_SIZE(kRdIsRn),
                                kRdIsRn},
                               {{al, r1, r1, r2},
                                "al r1 r1 r2",
                                "RdIsRn_al_r1_r1_r2",
                                ARRAY_SIZE(kRdIsRn),
                                kRdIsRn},
                               {{al, r12, r12, r7},
                                "al r12 r12 r7",
                                "RdIsRn_al_r12_r12_r7",
                                ARRAY_SIZE(kRdIsRn),
                                kRdIsRn},
                               {{al, r10, r10, r6},
                                "al r10 r10 r6",
                                "RdIsRn_al_r10_r10_r6",
                                ARRAY_SIZE(kRdIsRn),
                                kRdIsRn},
                               {{al, r4, r4, r9},
                                "al r4 r4 r9",
                                "RdIsRn_al_r4_r4_r9",
                                ARRAY_SIZE(kRdIsRn),
                                kRdIsRn},
                               {{al, r5, r5, r6},
                                "al r5 r5 r6",
                                "RdIsRn_al_r5_r5_r6",
                                ARRAY_SIZE(kRdIsRn),
                                kRdIsRn},
                               {{al, r2, r2, r4},
                                "al r2 r2 r4",
                                "RdIsRn_al_r2_r2_r4",
                                ARRAY_SIZE(kRdIsRn),
                                kRdIsRn},
                               {{al, r7, r7, r11},
                                "al r7 r7 r11",
                                "RdIsRn_al_r7_r7_r11",
                                ARRAY_SIZE(kRdIsRn),
                                kRdIsRn},
                               {{al, r7, r7, r12},
                                "al r7 r7 r12",
                                "RdIsRn_al_r7_r7_r12",
                                ARRAY_SIZE(kRdIsRn),
                                kRdIsRn},
                               {{al, r2, r2, r10},
                                "al r2 r2 r10",
                                "RdIsRn_al_r2_r2_r10",
                                ARRAY_SIZE(kRdIsRn),
                                kRdIsRn},
                               {{al, r1, r12, r1},
                                "al r1 r12 r1",
                                "RdIsRm_al_r1_r12_r1",
                                ARRAY_SIZE(kRdIsRm),
                                kRdIsRm},
                               {{al, r4, r3, r4},
                                "al r4 r3 r4",
                                "RdIsRm_al_r4_r3_r4",
                                ARRAY_SIZE(kRdIsRm),
                                kRdIsRm},
                               {{al, r0, r2, r0},
                                "al r0 r2 r0",
                                "RdIsRm_al_r0_r2_r0",
                                ARRAY_SIZE(kRdIsRm),
                                kRdIsRm},
                               {{al, r10, r4, r10},
                                "al r10 r4 r10",
                                "RdIsRm_al_r10_r4_r10",
                                ARRAY_SIZE(kRdIsRm),
                                kRdIsRm},
                               {{al, r4, r8, r4},
                                "al r4 r8 r4",
                                "RdIsRm_al_r4_r8_r4",
                                ARRAY_SIZE(kRdIsRm),
                                kRdIsRm},
                               {{al, r4, r1, r4},
                                "al r4 r1 r4",
                                "RdIsRm_al_r4_r1_r4",
                                ARRAY_SIZE(kRdIsRm),
                                kRdIsRm},
                               {{al, r8, r11, r8},
                                "al r8 r11 r8",
                                "RdIsRm_al_r8_r11_r8",
                                ARRAY_SIZE(kRdIsRm),
                                kRdIsRm},
                               {{al, r1, r10, r1},
                                "al r1 r10 r1",
                                "RdIsRm_al_r1_r10_r1",
                                ARRAY_SIZE(kRdIsRm),
                                kRdIsRm},
                               {{al, r2, r0, r2},
                                "al r2 r0 r2",
                                "RdIsRm_al_r2_r0_r2",
                                ARRAY_SIZE(kRdIsRm),
                                kRdIsRm},
                               {{al, r6, r7, r6},
                                "al r6 r7 r6",
                                "RdIsRm_al_r6_r7_r6",
                                ARRAY_SIZE(kRdIsRm),
                                kRdIsRm},
                               {{al, r1, r12, r12},
                                "al r1 r12 r12",
                                "RnIsRm_al_r1_r12_r12",
                                ARRAY_SIZE(kRnIsRm),
                                kRnIsRm},
                               {{al, r6, r9, r9},
                                "al r6 r9 r9",
                                "RnIsRm_al_r6_r9_r9",
                                ARRAY_SIZE(kRnIsRm),
                                kRnIsRm},
                               {{al, r12, r7, r7},
                                "al r12 r7 r7",
                                "RnIsRm_al_r12_r7_r7",
                                ARRAY_SIZE(kRnIsRm),
                                kRnIsRm},
                               {{al, r11, r8, r8},
                                "al r11 r8 r8",
                                "RnIsRm_al_r11_r8_r8",
                                ARRAY_SIZE(kRnIsRm),
                                kRnIsRm},
                               {{al, r14, r0, r0},
                                "al r14 r0 r0",
                                "RnIsRm_al_r14_r0_r0",
                                ARRAY_SIZE(kRnIsRm),
                                kRnIsRm},
                               {{al, r8, r4, r4},
                                "al r8 r4 r4",
                                "RnIsRm_al_r8_r4_r4",
                                ARRAY_SIZE(kRnIsRm),
                                kRnIsRm},
                               {{al, r0, r8, r8},
                                "al r0 r8 r8",
                                "RnIsRm_al_r0_r8_r8",
                                ARRAY_SIZE(kRnIsRm),
                                kRnIsRm},
                               {{al, r5, r3, r3},
                                "al r5 r3 r3",
                                "RnIsRm_al_r5_r3_r3",
                                ARRAY_SIZE(kRnIsRm),
                                kRnIsRm},
                               {{al, r8, r5, r5},
                                "al r8 r5 r5",
                                "RnIsRm_al_r8_r5_r5",
                                ARRAY_SIZE(kRnIsRm),
                                kRnIsRm},
                               {{al, r5, r10, r10},
                                "al r5 r10 r10",
                                "RnIsRm_al_r5_r10_r10",
                                ARRAY_SIZE(kRnIsRm),
                                kRnIsRm},
                               {{al, r14, r3, r4},
                                "al r14 r3 r4",
                                "RdIsNotRnIsNotRm_al_r14_r3_r4",
                                ARRAY_SIZE(kRdIsNotRnIsNotRm),
                                kRdIsNotRnIsNotRm},
                               {{al, r3, r11, r1},
                                "al r3 r11 r1",
                                "RdIsNotRnIsNotRm_al_r3_r11_r1",
                                ARRAY_SIZE(kRdIsNotRnIsNotRm),
                                kRdIsNotRnIsNotRm},
                               {{al, r11, r2, r6},
                                "al r11 r2 r6",
                                "RdIsNotRnIsNotRm_al_r11_r2_r6",
                                ARRAY_SIZE(kRdIsNotRnIsNotRm),
                                kRdIsNotRnIsNotRm},
                               {{al, r14, r0, r5},
                                "al r14 r0 r5",
                                "RdIsNotRnIsNotRm_al_r14_r0_r5",
                                ARRAY_SIZE(kRdIsNotRnIsNotRm),
                                kRdIsNotRnIsNotRm},
                               {{al, r11, r6, r12},
                                "al r11 r6 r12",
                                "RdIsNotRnIsNotRm_al_r11_r6_r12",
                                ARRAY_SIZE(kRdIsNotRnIsNotRm),
                                kRdIsNotRnIsNotRm},
                               {{al, r10, r8, r4},
                                "al r10 r8 r4",
                                "RdIsNotRnIsNotRm_al_r10_r8_r4",
                                ARRAY_SIZE(kRdIsNotRnIsNotRm),
                                kRdIsNotRnIsNotRm},
                               {{al, r4, r3, r1},
                                "al r4 r3 r1",
                                "RdIsNotRnIsNotRm_al_r4_r3_r1",
                                ARRAY_SIZE(kRdIsNotRnIsNotRm),
                                kRdIsNotRnIsNotRm},
                               {{al, r12, r11, r14},
                                "al r12 r11 r14",
                                "RdIsNotRnIsNotRm_al_r12_r11_r14",
                                ARRAY_SIZE(kRdIsNotRnIsNotRm),
                                kRdIsNotRnIsNotRm},
                               {{al, r9, r4, r11},
                                "al r9 r4 r11",
                                "RdIsNotRnIsNotRm_al_r9_r4_r11",
                                ARRAY_SIZE(kRdIsNotRnIsNotRm),
                                kRdIsNotRnIsNotRm},
                               {{al, r14, r4, r6},
                                "al r14 r4 r6",
                                "RdIsNotRnIsNotRm_al_r14_r4_r6",
                                ARRAY_SIZE(kRdIsNotRnIsNotRm),
                                kRdIsNotRnIsNotRm}};

// We record all inputs to the instructions as outputs. This way, we also check
// that what shouldn't change didn't change.
struct TestResult {
  size_t output_size;
  const Inputs* outputs;
};

// These headers each contain an array of `TestResult` with the reference output
// values. The reference arrays are names `kReference{mnemonic}`.
#include "aarch32/traces/simulator-cond-rd-rn-rm-mul-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-qadd-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-qadd16-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-qadd8-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-qasx-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-qdadd-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-qdsub-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-qsax-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-qsub-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-qsub16-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-qsub8-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-sadd16-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-sadd8-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-sasx-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-sdiv-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-sel-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-shadd16-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-shadd8-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-shasx-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-shsax-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-shsub16-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-shsub8-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-smmul-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-smmulr-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-smuad-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-smuadx-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-smulbb-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-smulbt-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-smultb-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-smultt-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-smulwb-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-smulwt-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-smusd-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-smusdx-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-ssax-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-ssub16-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-ssub8-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uadd16-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uadd8-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uasx-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-udiv-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uhadd16-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uhadd8-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uhasx-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uhsax-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uhsub16-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uhsub8-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uqadd16-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uqadd8-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uqasx-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uqsax-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uqsub16-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-uqsub8-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-usad8-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-usax-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-usub16-t32.h"
#include "aarch32/traces/simulator-cond-rd-rn-rm-usub8-t32.h"


// The maximum number of errors to report in detail for each test.
const unsigned kErrorReportLimit = 8;

typedef void (MacroAssembler::*Fn)(Condition cond,
                                   Register rd,
                                   Register rn,
                                   Register rm);

void TestHelper(Fn instruction,
                const char* mnemonic,
                const TestResult reference[]) {
  SETUP();
  masm.UseT32();
  START();

  // Data to compare to `reference`.
  TestResult* results[ARRAY_SIZE(kTests)];

  // Test cases for memory bound instructions may allocate a buffer and save its
  // address in this array.
  byte* scratch_memory_buffers[ARRAY_SIZE(kTests)];

  // Generate a loop for each element in `kTests`. Each loop tests one specific
  // instruction.
  for (unsigned i = 0; i < ARRAY_SIZE(kTests); i++) {
    // Allocate results on the heap for this test.
    results[i] = new TestResult;
    results[i]->outputs = new Inputs[kTests[i].input_size];
    results[i]->output_size = kTests[i].input_size;

    size_t input_stride = sizeof(kTests[i].inputs[0]) * kTests[i].input_size;
    VIXL_ASSERT(IsUint32(input_stride));

    scratch_memory_buffers[i] = NULL;

    Label loop;
    UseScratchRegisterScope scratch_registers(&masm);
    // Include all registers from r0 ro r12.
    scratch_registers.Include(RegisterList(0x1fff));

    // Values to pass to the macro-assembler.
    Condition cond = kTests[i].operands.cond;
    Register rd = kTests[i].operands.rd;
    Register rn = kTests[i].operands.rn;
    Register rm = kTests[i].operands.rm;
    scratch_registers.Exclude(rd);
    scratch_registers.Exclude(rn);
    scratch_registers.Exclude(rm);

    // Allocate reserved registers for our own use.
    Register input_ptr = scratch_registers.Acquire();
    Register input_end = scratch_registers.Acquire();
    Register result_ptr = scratch_registers.Acquire();

    // Initialize `input_ptr` to the first element and `input_end` the address
    // after the array.
    __ Mov(input_ptr, Operand::From(kTests[i].inputs));
    __ Add(input_end, input_ptr, static_cast<uint32_t>(input_stride));
    __ Mov(result_ptr, Operand::From(results[i]->outputs));
    __ Bind(&loop);

    {
      UseScratchRegisterScope temp_registers(&masm);
      Register nzcv_bits = temp_registers.Acquire();
      Register saved_q_bit = temp_registers.Acquire();
      // Save the `Q` bit flag.
      __ Mrs(saved_q_bit, APSR);
      __ And(saved_q_bit, saved_q_bit, QFlag);
      // Set the `NZCV` and `Q` flags together.
      __ Ldr(nzcv_bits, MemOperand(input_ptr, offsetof(Inputs, apsr)));
      __ Orr(nzcv_bits, nzcv_bits, saved_q_bit);
      __ Msr(APSR_nzcvq, nzcv_bits);
    }
    {
      UseScratchRegisterScope temp_registers(&masm);
      Register q_bit = temp_registers.Acquire();
      Register saved_nzcv_bits = temp_registers.Acquire();
      // Save the `NZCV` flags.
      __ Mrs(saved_nzcv_bits, APSR);
      __ And(saved_nzcv_bits, saved_nzcv_bits, NZCVFlag);
      // Set the `NZCV` and `Q` flags together.
      __ Ldr(q_bit, MemOperand(input_ptr, offsetof(Inputs, qbit)));
      __ Orr(q_bit, q_bit, saved_nzcv_bits);
      __ Msr(APSR_nzcvq, q_bit);
    }
    {
      UseScratchRegisterScope temp_registers(&masm);
      Register ge_bits = temp_registers.Acquire();
      __ Ldr(ge_bits, MemOperand(input_ptr, offsetof(Inputs, ge)));
      __ Msr(APSR_g, ge_bits);
    }
    __ Ldr(rd, MemOperand(input_ptr, offsetof(Inputs, rd)));
    __ Ldr(rn, MemOperand(input_ptr, offsetof(Inputs, rn)));
    __ Ldr(rm, MemOperand(input_ptr, offsetof(Inputs, rm)));

    (masm.*instruction)(cond, rd, rn, rm);

    {
      UseScratchRegisterScope temp_registers(&masm);
      Register nzcv_bits = temp_registers.Acquire();
      __ Mrs(nzcv_bits, APSR);
      // Only record the NZCV bits.
      __ And(nzcv_bits, nzcv_bits, NZCVFlag);
      __ Str(nzcv_bits, MemOperand(result_ptr, offsetof(Inputs, apsr)));
    }
    {
      UseScratchRegisterScope temp_registers(&masm);
      Register q_bit = temp_registers.Acquire();
      __ Mrs(q_bit, APSR);
      // Only record the Q bit.
      __ And(q_bit, q_bit, QFlag);
      __ Str(q_bit, MemOperand(result_ptr, offsetof(Inputs, qbit)));
    }
    {
      UseScratchRegisterScope temp_registers(&masm);
      Register ge_bits = temp_registers.Acquire();
      __ Mrs(ge_bits, APSR);
      // Only record the GE bits.
      __ And(ge_bits, ge_bits, GEFlags);
      __ Str(ge_bits, MemOperand(result_ptr, offsetof(Inputs, ge)));
    }
    __ Str(rd, MemOperand(result_ptr, offsetof(Inputs, rd)));
    __ Str(rn, MemOperand(result_ptr, offsetof(Inputs, rn)));
    __ Str(rm, MemOperand(result_ptr, offsetof(Inputs, rm)));

    // Advance the result pointer.
    __ Add(result_ptr, result_ptr, Operand::From(sizeof(kTests[i].inputs[0])));
    // Loop back until `input_ptr` is lower than `input_base`.
    __ Add(input_ptr, input_ptr, Operand::From(sizeof(kTests[i].inputs[0])));
    __ Cmp(input_ptr, input_end);
    __ B(ne, &loop);
  }

  END();

  RUN();

  if (Test::generate_test_trace()) {
    // Print the results.
    for (size_t i = 0; i < ARRAY_SIZE(kTests); i++) {
      printf("const Inputs kOutputs_%s_%s[] = {\n",
             mnemonic,
             kTests[i].identifier);
      for (size_t j = 0; j < results[i]->output_size; j++) {
        printf("  { ");
        printf("0x%08" PRIx32, results[i]->outputs[j].apsr);
        printf(", ");
        printf("0x%08" PRIx32, results[i]->outputs[j].qbit);
        printf(", ");
        printf("0x%08" PRIx32, results[i]->outputs[j].ge);
        printf(", ");
        printf("0x%08" PRIx32, results[i]->outputs[j].rd);
        printf(", ");
        printf("0x%08" PRIx32, results[i]->outputs[j].rn);
        printf(", ");
        printf("0x%08" PRIx32, results[i]->outputs[j].rm);
        printf(" },\n");
      }
      printf("};\n");
    }
    printf("const TestResult kReference%s[] = {\n", mnemonic);
    for (size_t i = 0; i < ARRAY_SIZE(kTests); i++) {
      printf("  {\n");
      printf("    ARRAY_SIZE(kOutputs_%s_%s),\n",
             mnemonic,
             kTests[i].identifier);
      printf("    kOutputs_%s_%s,\n", mnemonic, kTests[i].identifier);
      printf("  },\n");
    }
    printf("};\n");
  } else if (kCheckSimulatorTestResults) {
    // Check the results.
    unsigned total_error_count = 0;
    for (size_t i = 0; i < ARRAY_SIZE(kTests); i++) {
      bool instruction_has_errors = false;
      for (size_t j = 0; j < kTests[i].input_size; j++) {
        uint32_t apsr = results[i]->outputs[j].apsr;
        uint32_t qbit = results[i]->outputs[j].qbit;
        uint32_t ge = results[i]->outputs[j].ge;
        uint32_t rd = results[i]->outputs[j].rd;
        uint32_t rn = results[i]->outputs[j].rn;
        uint32_t rm = results[i]->outputs[j].rm;
        uint32_t apsr_input = kTests[i].inputs[j].apsr;
        uint32_t qbit_input = kTests[i].inputs[j].qbit;
        uint32_t ge_input = kTests[i].inputs[j].ge;
        uint32_t rd_input = kTests[i].inputs[j].rd;
        uint32_t rn_input = kTests[i].inputs[j].rn;
        uint32_t rm_input = kTests[i].inputs[j].rm;
        uint32_t apsr_ref = reference[i].outputs[j].apsr;
        uint32_t qbit_ref = reference[i].outputs[j].qbit;
        uint32_t ge_ref = reference[i].outputs[j].ge;
        uint32_t rd_ref = reference[i].outputs[j].rd;
        uint32_t rn_ref = reference[i].outputs[j].rn;
        uint32_t rm_ref = reference[i].outputs[j].rm;

        if (((apsr != apsr_ref) || (qbit != qbit_ref) || (ge != ge_ref) ||
             (rd != rd_ref) || (rn != rn_ref) || (rm != rm_ref)) &&
            (++total_error_count <= kErrorReportLimit)) {
          // Print the instruction once even if it triggered multiple failures.
          if (!instruction_has_errors) {
            printf("Error(s) when testing \"%s %s\":\n",
                   mnemonic,
                   kTests[i].operands_description);
            instruction_has_errors = true;
          }
          // Print subsequent errors.
          printf("  Input:    ");
          printf("0x%08" PRIx32, apsr_input);
          printf(", ");
          printf("0x%08" PRIx32, qbit_input);
          printf(", ");
          printf("0x%08" PRIx32, ge_input);
          printf(", ");
          printf("0x%08" PRIx32, rd_input);
          printf(", ");
          printf("0x%08" PRIx32, rn_input);
          printf(", ");
          printf("0x%08" PRIx32, rm_input);
          printf("\n");
          printf("  Expected: ");
          printf("0x%08" PRIx32, apsr_ref);
          printf(", ");
          printf("0x%08" PRIx32, qbit_ref);
          printf(", ");
          printf("0x%08" PRIx32, ge_ref);
          printf(", ");
          printf("0x%08" PRIx32, rd_ref);
          printf(", ");
          printf("0x%08" PRIx32, rn_ref);
          printf(", ");
          printf("0x%08" PRIx32, rm_ref);
          printf("\n");
          printf("  Found:    ");
          printf("0x%08" PRIx32, apsr);
          printf(", ");
          printf("0x%08" PRIx32, qbit);
          printf(", ");
          printf("0x%08" PRIx32, ge);
          printf(", ");
          printf("0x%08" PRIx32, rd);
          printf(", ");
          printf("0x%08" PRIx32, rn);
          printf(", ");
          printf("0x%08" PRIx32, rm);
          printf("\n\n");
        }
      }
    }

    if (total_error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n",
             total_error_count - kErrorReportLimit);
    }
    VIXL_CHECK(total_error_count == 0);
  } else {
    VIXL_WARNING("Assembled the code, but did not run anything.\n");
  }

  for (size_t i = 0; i < ARRAY_SIZE(kTests); i++) {
    delete[] results[i]->outputs;
    delete results[i];
    delete[] scratch_memory_buffers[i];
  }
}

// Instantiate tests for each instruction in the list.
// TODO: Remove this limitation by having a sandboxing mechanism.
#if defined(VIXL_HOST_POINTER_32)
#define TEST(mnemonic)                                                      \
  void Test_##mnemonic() {                                                  \
    TestHelper(&MacroAssembler::mnemonic, #mnemonic, kReference##mnemonic); \
  }                                                                         \
  Test test_##mnemonic("AARCH32_SIMULATOR_COND_RD_RN_RM_" #mnemonic "_T32", \
                       &Test_##mnemonic);
#else
#define TEST(mnemonic)                                                      \
  void Test_##mnemonic() {                                                  \
    VIXL_WARNING("This test can only run on a 32-bit host.\n");             \
    USE(TestHelper);                                                        \
  }                                                                         \
  Test test_##mnemonic("AARCH32_SIMULATOR_COND_RD_RN_RM_" #mnemonic "_T32", \
                       &Test_##mnemonic);
#endif

FOREACH_INSTRUCTION(TEST)
#undef TEST

}  // namespace
#endif

}  // namespace aarch32
}  // namespace vixl
