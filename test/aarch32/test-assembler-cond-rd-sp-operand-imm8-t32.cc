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
// test/aarch32/config/template-assembler-aarch32.cc.in template file using
// tools/generate_tests.py.
//
// PLEASE DO NOT EDIT.
// -----------------------------------------------------------------------------

#include "test-runner.h"

#include "test-utils.h"
#include "test-utils-aarch32.h"

#include "aarch32/assembler-aarch32.h"
#include "aarch32/macro-assembler-aarch32.h"

#define BUF_SIZE (4096)

namespace vixl {
namespace aarch32 {

// List of instruction mnemonics.
#define FOREACH_INSTRUCTION(M) M(Add)

// Values to be passed to the assembler to produce the instruction under test.
struct Operands {
  Condition cond;
  Register rd;
  Register rn;
  uint32_t immediate;
};

// This structure contains all data needed to test one specific
// instruction.
struct TestData {
  // The `operands` field represents what to pass to the assembler to
  // produce the instruction.
  Operands operands;
  // Optionally expect the MacroAssembler to have generated an extra
  // instruction. This is used when the instruction needs to be in an IT block.
  const char* expect_instruction_before;
  // Description of the operands, used for error reporting.
  const char* operands_description;
  // Unique identifier, used for generating traces.
  const char* identifier;
};

struct TestResult {
  size_t size;
  const byte* encoding;
};

// Each element of this array produce one instruction encoding.
static const TestData kTests[] = {
    {{al, r0, sp, 0x0}, "", "al r0 sp 0x0", "al_r0_sp_0x0"},
    {{al, r0, sp, 0x4}, "", "al r0 sp 0x4", "al_r0_sp_0x4"},
    {{al, r0, sp, 0x8}, "", "al r0 sp 0x8", "al_r0_sp_0x8"},
    {{al, r0, sp, 0xc}, "", "al r0 sp 0xc", "al_r0_sp_0xc"},
    {{al, r0, sp, 0x10}, "", "al r0 sp 0x10", "al_r0_sp_0x10"},
    {{al, r0, sp, 0x14}, "", "al r0 sp 0x14", "al_r0_sp_0x14"},
    {{al, r0, sp, 0x18}, "", "al r0 sp 0x18", "al_r0_sp_0x18"},
    {{al, r0, sp, 0x1c}, "", "al r0 sp 0x1c", "al_r0_sp_0x1c"},
    {{al, r0, sp, 0x20}, "", "al r0 sp 0x20", "al_r0_sp_0x20"},
    {{al, r0, sp, 0x24}, "", "al r0 sp 0x24", "al_r0_sp_0x24"},
    {{al, r0, sp, 0x28}, "", "al r0 sp 0x28", "al_r0_sp_0x28"},
    {{al, r0, sp, 0x2c}, "", "al r0 sp 0x2c", "al_r0_sp_0x2c"},
    {{al, r0, sp, 0x30}, "", "al r0 sp 0x30", "al_r0_sp_0x30"},
    {{al, r0, sp, 0x34}, "", "al r0 sp 0x34", "al_r0_sp_0x34"},
    {{al, r0, sp, 0x38}, "", "al r0 sp 0x38", "al_r0_sp_0x38"},
    {{al, r0, sp, 0x3c}, "", "al r0 sp 0x3c", "al_r0_sp_0x3c"},
    {{al, r0, sp, 0x40}, "", "al r0 sp 0x40", "al_r0_sp_0x40"},
    {{al, r0, sp, 0x44}, "", "al r0 sp 0x44", "al_r0_sp_0x44"},
    {{al, r0, sp, 0x48}, "", "al r0 sp 0x48", "al_r0_sp_0x48"},
    {{al, r0, sp, 0x4c}, "", "al r0 sp 0x4c", "al_r0_sp_0x4c"},
    {{al, r0, sp, 0x50}, "", "al r0 sp 0x50", "al_r0_sp_0x50"},
    {{al, r0, sp, 0x54}, "", "al r0 sp 0x54", "al_r0_sp_0x54"},
    {{al, r0, sp, 0x58}, "", "al r0 sp 0x58", "al_r0_sp_0x58"},
    {{al, r0, sp, 0x5c}, "", "al r0 sp 0x5c", "al_r0_sp_0x5c"},
    {{al, r0, sp, 0x60}, "", "al r0 sp 0x60", "al_r0_sp_0x60"},
    {{al, r0, sp, 0x64}, "", "al r0 sp 0x64", "al_r0_sp_0x64"},
    {{al, r0, sp, 0x68}, "", "al r0 sp 0x68", "al_r0_sp_0x68"},
    {{al, r0, sp, 0x6c}, "", "al r0 sp 0x6c", "al_r0_sp_0x6c"},
    {{al, r0, sp, 0x70}, "", "al r0 sp 0x70", "al_r0_sp_0x70"},
    {{al, r0, sp, 0x74}, "", "al r0 sp 0x74", "al_r0_sp_0x74"},
    {{al, r0, sp, 0x78}, "", "al r0 sp 0x78", "al_r0_sp_0x78"},
    {{al, r0, sp, 0x7c}, "", "al r0 sp 0x7c", "al_r0_sp_0x7c"},
    {{al, r0, sp, 0x80}, "", "al r0 sp 0x80", "al_r0_sp_0x80"},
    {{al, r0, sp, 0x84}, "", "al r0 sp 0x84", "al_r0_sp_0x84"},
    {{al, r0, sp, 0x88}, "", "al r0 sp 0x88", "al_r0_sp_0x88"},
    {{al, r0, sp, 0x8c}, "", "al r0 sp 0x8c", "al_r0_sp_0x8c"},
    {{al, r0, sp, 0x90}, "", "al r0 sp 0x90", "al_r0_sp_0x90"},
    {{al, r0, sp, 0x94}, "", "al r0 sp 0x94", "al_r0_sp_0x94"},
    {{al, r0, sp, 0x98}, "", "al r0 sp 0x98", "al_r0_sp_0x98"},
    {{al, r0, sp, 0x9c}, "", "al r0 sp 0x9c", "al_r0_sp_0x9c"},
    {{al, r0, sp, 0xa0}, "", "al r0 sp 0xa0", "al_r0_sp_0xa0"},
    {{al, r0, sp, 0xa4}, "", "al r0 sp 0xa4", "al_r0_sp_0xa4"},
    {{al, r0, sp, 0xa8}, "", "al r0 sp 0xa8", "al_r0_sp_0xa8"},
    {{al, r0, sp, 0xac}, "", "al r0 sp 0xac", "al_r0_sp_0xac"},
    {{al, r0, sp, 0xb0}, "", "al r0 sp 0xb0", "al_r0_sp_0xb0"},
    {{al, r0, sp, 0xb4}, "", "al r0 sp 0xb4", "al_r0_sp_0xb4"},
    {{al, r0, sp, 0xb8}, "", "al r0 sp 0xb8", "al_r0_sp_0xb8"},
    {{al, r0, sp, 0xbc}, "", "al r0 sp 0xbc", "al_r0_sp_0xbc"},
    {{al, r0, sp, 0xc0}, "", "al r0 sp 0xc0", "al_r0_sp_0xc0"},
    {{al, r0, sp, 0xc4}, "", "al r0 sp 0xc4", "al_r0_sp_0xc4"},
    {{al, r0, sp, 0xc8}, "", "al r0 sp 0xc8", "al_r0_sp_0xc8"},
    {{al, r0, sp, 0xcc}, "", "al r0 sp 0xcc", "al_r0_sp_0xcc"},
    {{al, r0, sp, 0xd0}, "", "al r0 sp 0xd0", "al_r0_sp_0xd0"},
    {{al, r0, sp, 0xd4}, "", "al r0 sp 0xd4", "al_r0_sp_0xd4"},
    {{al, r0, sp, 0xd8}, "", "al r0 sp 0xd8", "al_r0_sp_0xd8"},
    {{al, r0, sp, 0xdc}, "", "al r0 sp 0xdc", "al_r0_sp_0xdc"},
    {{al, r0, sp, 0xe0}, "", "al r0 sp 0xe0", "al_r0_sp_0xe0"},
    {{al, r0, sp, 0xe4}, "", "al r0 sp 0xe4", "al_r0_sp_0xe4"},
    {{al, r0, sp, 0xe8}, "", "al r0 sp 0xe8", "al_r0_sp_0xe8"},
    {{al, r0, sp, 0xec}, "", "al r0 sp 0xec", "al_r0_sp_0xec"},
    {{al, r0, sp, 0xf0}, "", "al r0 sp 0xf0", "al_r0_sp_0xf0"},
    {{al, r0, sp, 0xf4}, "", "al r0 sp 0xf4", "al_r0_sp_0xf4"},
    {{al, r0, sp, 0xf8}, "", "al r0 sp 0xf8", "al_r0_sp_0xf8"},
    {{al, r0, sp, 0xfc}, "", "al r0 sp 0xfc", "al_r0_sp_0xfc"},
    {{al, r0, sp, 0x100}, "", "al r0 sp 0x100", "al_r0_sp_0x100"},
    {{al, r0, sp, 0x104}, "", "al r0 sp 0x104", "al_r0_sp_0x104"},
    {{al, r0, sp, 0x108}, "", "al r0 sp 0x108", "al_r0_sp_0x108"},
    {{al, r0, sp, 0x10c}, "", "al r0 sp 0x10c", "al_r0_sp_0x10c"},
    {{al, r0, sp, 0x110}, "", "al r0 sp 0x110", "al_r0_sp_0x110"},
    {{al, r0, sp, 0x114}, "", "al r0 sp 0x114", "al_r0_sp_0x114"},
    {{al, r0, sp, 0x118}, "", "al r0 sp 0x118", "al_r0_sp_0x118"},
    {{al, r0, sp, 0x11c}, "", "al r0 sp 0x11c", "al_r0_sp_0x11c"},
    {{al, r0, sp, 0x120}, "", "al r0 sp 0x120", "al_r0_sp_0x120"},
    {{al, r0, sp, 0x124}, "", "al r0 sp 0x124", "al_r0_sp_0x124"},
    {{al, r0, sp, 0x128}, "", "al r0 sp 0x128", "al_r0_sp_0x128"},
    {{al, r0, sp, 0x12c}, "", "al r0 sp 0x12c", "al_r0_sp_0x12c"},
    {{al, r0, sp, 0x130}, "", "al r0 sp 0x130", "al_r0_sp_0x130"},
    {{al, r0, sp, 0x134}, "", "al r0 sp 0x134", "al_r0_sp_0x134"},
    {{al, r0, sp, 0x138}, "", "al r0 sp 0x138", "al_r0_sp_0x138"},
    {{al, r0, sp, 0x13c}, "", "al r0 sp 0x13c", "al_r0_sp_0x13c"},
    {{al, r0, sp, 0x140}, "", "al r0 sp 0x140", "al_r0_sp_0x140"},
    {{al, r0, sp, 0x144}, "", "al r0 sp 0x144", "al_r0_sp_0x144"},
    {{al, r0, sp, 0x148}, "", "al r0 sp 0x148", "al_r0_sp_0x148"},
    {{al, r0, sp, 0x14c}, "", "al r0 sp 0x14c", "al_r0_sp_0x14c"},
    {{al, r0, sp, 0x150}, "", "al r0 sp 0x150", "al_r0_sp_0x150"},
    {{al, r0, sp, 0x154}, "", "al r0 sp 0x154", "al_r0_sp_0x154"},
    {{al, r0, sp, 0x158}, "", "al r0 sp 0x158", "al_r0_sp_0x158"},
    {{al, r0, sp, 0x15c}, "", "al r0 sp 0x15c", "al_r0_sp_0x15c"},
    {{al, r0, sp, 0x160}, "", "al r0 sp 0x160", "al_r0_sp_0x160"},
    {{al, r0, sp, 0x164}, "", "al r0 sp 0x164", "al_r0_sp_0x164"},
    {{al, r0, sp, 0x168}, "", "al r0 sp 0x168", "al_r0_sp_0x168"},
    {{al, r0, sp, 0x16c}, "", "al r0 sp 0x16c", "al_r0_sp_0x16c"},
    {{al, r0, sp, 0x170}, "", "al r0 sp 0x170", "al_r0_sp_0x170"},
    {{al, r0, sp, 0x174}, "", "al r0 sp 0x174", "al_r0_sp_0x174"},
    {{al, r0, sp, 0x178}, "", "al r0 sp 0x178", "al_r0_sp_0x178"},
    {{al, r0, sp, 0x17c}, "", "al r0 sp 0x17c", "al_r0_sp_0x17c"},
    {{al, r0, sp, 0x180}, "", "al r0 sp 0x180", "al_r0_sp_0x180"},
    {{al, r0, sp, 0x184}, "", "al r0 sp 0x184", "al_r0_sp_0x184"},
    {{al, r0, sp, 0x188}, "", "al r0 sp 0x188", "al_r0_sp_0x188"},
    {{al, r0, sp, 0x18c}, "", "al r0 sp 0x18c", "al_r0_sp_0x18c"},
    {{al, r0, sp, 0x190}, "", "al r0 sp 0x190", "al_r0_sp_0x190"},
    {{al, r0, sp, 0x194}, "", "al r0 sp 0x194", "al_r0_sp_0x194"},
    {{al, r0, sp, 0x198}, "", "al r0 sp 0x198", "al_r0_sp_0x198"},
    {{al, r0, sp, 0x19c}, "", "al r0 sp 0x19c", "al_r0_sp_0x19c"},
    {{al, r0, sp, 0x1a0}, "", "al r0 sp 0x1a0", "al_r0_sp_0x1a0"},
    {{al, r0, sp, 0x1a4}, "", "al r0 sp 0x1a4", "al_r0_sp_0x1a4"},
    {{al, r0, sp, 0x1a8}, "", "al r0 sp 0x1a8", "al_r0_sp_0x1a8"},
    {{al, r0, sp, 0x1ac}, "", "al r0 sp 0x1ac", "al_r0_sp_0x1ac"},
    {{al, r0, sp, 0x1b0}, "", "al r0 sp 0x1b0", "al_r0_sp_0x1b0"},
    {{al, r0, sp, 0x1b4}, "", "al r0 sp 0x1b4", "al_r0_sp_0x1b4"},
    {{al, r0, sp, 0x1b8}, "", "al r0 sp 0x1b8", "al_r0_sp_0x1b8"},
    {{al, r0, sp, 0x1bc}, "", "al r0 sp 0x1bc", "al_r0_sp_0x1bc"},
    {{al, r0, sp, 0x1c0}, "", "al r0 sp 0x1c0", "al_r0_sp_0x1c0"},
    {{al, r0, sp, 0x1c4}, "", "al r0 sp 0x1c4", "al_r0_sp_0x1c4"},
    {{al, r0, sp, 0x1c8}, "", "al r0 sp 0x1c8", "al_r0_sp_0x1c8"},
    {{al, r0, sp, 0x1cc}, "", "al r0 sp 0x1cc", "al_r0_sp_0x1cc"},
    {{al, r0, sp, 0x1d0}, "", "al r0 sp 0x1d0", "al_r0_sp_0x1d0"},
    {{al, r0, sp, 0x1d4}, "", "al r0 sp 0x1d4", "al_r0_sp_0x1d4"},
    {{al, r0, sp, 0x1d8}, "", "al r0 sp 0x1d8", "al_r0_sp_0x1d8"},
    {{al, r0, sp, 0x1dc}, "", "al r0 sp 0x1dc", "al_r0_sp_0x1dc"},
    {{al, r0, sp, 0x1e0}, "", "al r0 sp 0x1e0", "al_r0_sp_0x1e0"},
    {{al, r0, sp, 0x1e4}, "", "al r0 sp 0x1e4", "al_r0_sp_0x1e4"},
    {{al, r0, sp, 0x1e8}, "", "al r0 sp 0x1e8", "al_r0_sp_0x1e8"},
    {{al, r0, sp, 0x1ec}, "", "al r0 sp 0x1ec", "al_r0_sp_0x1ec"},
    {{al, r0, sp, 0x1f0}, "", "al r0 sp 0x1f0", "al_r0_sp_0x1f0"},
    {{al, r0, sp, 0x1f4}, "", "al r0 sp 0x1f4", "al_r0_sp_0x1f4"},
    {{al, r0, sp, 0x1f8}, "", "al r0 sp 0x1f8", "al_r0_sp_0x1f8"},
    {{al, r0, sp, 0x1fc}, "", "al r0 sp 0x1fc", "al_r0_sp_0x1fc"},
    {{al, r0, sp, 0x200}, "", "al r0 sp 0x200", "al_r0_sp_0x200"},
    {{al, r0, sp, 0x204}, "", "al r0 sp 0x204", "al_r0_sp_0x204"},
    {{al, r0, sp, 0x208}, "", "al r0 sp 0x208", "al_r0_sp_0x208"},
    {{al, r0, sp, 0x20c}, "", "al r0 sp 0x20c", "al_r0_sp_0x20c"},
    {{al, r0, sp, 0x210}, "", "al r0 sp 0x210", "al_r0_sp_0x210"},
    {{al, r0, sp, 0x214}, "", "al r0 sp 0x214", "al_r0_sp_0x214"},
    {{al, r0, sp, 0x218}, "", "al r0 sp 0x218", "al_r0_sp_0x218"},
    {{al, r0, sp, 0x21c}, "", "al r0 sp 0x21c", "al_r0_sp_0x21c"},
    {{al, r0, sp, 0x220}, "", "al r0 sp 0x220", "al_r0_sp_0x220"},
    {{al, r0, sp, 0x224}, "", "al r0 sp 0x224", "al_r0_sp_0x224"},
    {{al, r0, sp, 0x228}, "", "al r0 sp 0x228", "al_r0_sp_0x228"},
    {{al, r0, sp, 0x22c}, "", "al r0 sp 0x22c", "al_r0_sp_0x22c"},
    {{al, r0, sp, 0x230}, "", "al r0 sp 0x230", "al_r0_sp_0x230"},
    {{al, r0, sp, 0x234}, "", "al r0 sp 0x234", "al_r0_sp_0x234"},
    {{al, r0, sp, 0x238}, "", "al r0 sp 0x238", "al_r0_sp_0x238"},
    {{al, r0, sp, 0x23c}, "", "al r0 sp 0x23c", "al_r0_sp_0x23c"},
    {{al, r0, sp, 0x240}, "", "al r0 sp 0x240", "al_r0_sp_0x240"},
    {{al, r0, sp, 0x244}, "", "al r0 sp 0x244", "al_r0_sp_0x244"},
    {{al, r0, sp, 0x248}, "", "al r0 sp 0x248", "al_r0_sp_0x248"},
    {{al, r0, sp, 0x24c}, "", "al r0 sp 0x24c", "al_r0_sp_0x24c"},
    {{al, r0, sp, 0x250}, "", "al r0 sp 0x250", "al_r0_sp_0x250"},
    {{al, r0, sp, 0x254}, "", "al r0 sp 0x254", "al_r0_sp_0x254"},
    {{al, r0, sp, 0x258}, "", "al r0 sp 0x258", "al_r0_sp_0x258"},
    {{al, r0, sp, 0x25c}, "", "al r0 sp 0x25c", "al_r0_sp_0x25c"},
    {{al, r0, sp, 0x260}, "", "al r0 sp 0x260", "al_r0_sp_0x260"},
    {{al, r0, sp, 0x264}, "", "al r0 sp 0x264", "al_r0_sp_0x264"},
    {{al, r0, sp, 0x268}, "", "al r0 sp 0x268", "al_r0_sp_0x268"},
    {{al, r0, sp, 0x26c}, "", "al r0 sp 0x26c", "al_r0_sp_0x26c"},
    {{al, r0, sp, 0x270}, "", "al r0 sp 0x270", "al_r0_sp_0x270"},
    {{al, r0, sp, 0x274}, "", "al r0 sp 0x274", "al_r0_sp_0x274"},
    {{al, r0, sp, 0x278}, "", "al r0 sp 0x278", "al_r0_sp_0x278"},
    {{al, r0, sp, 0x27c}, "", "al r0 sp 0x27c", "al_r0_sp_0x27c"},
    {{al, r0, sp, 0x280}, "", "al r0 sp 0x280", "al_r0_sp_0x280"},
    {{al, r0, sp, 0x284}, "", "al r0 sp 0x284", "al_r0_sp_0x284"},
    {{al, r0, sp, 0x288}, "", "al r0 sp 0x288", "al_r0_sp_0x288"},
    {{al, r0, sp, 0x28c}, "", "al r0 sp 0x28c", "al_r0_sp_0x28c"},
    {{al, r0, sp, 0x290}, "", "al r0 sp 0x290", "al_r0_sp_0x290"},
    {{al, r0, sp, 0x294}, "", "al r0 sp 0x294", "al_r0_sp_0x294"},
    {{al, r0, sp, 0x298}, "", "al r0 sp 0x298", "al_r0_sp_0x298"},
    {{al, r0, sp, 0x29c}, "", "al r0 sp 0x29c", "al_r0_sp_0x29c"},
    {{al, r0, sp, 0x2a0}, "", "al r0 sp 0x2a0", "al_r0_sp_0x2a0"},
    {{al, r0, sp, 0x2a4}, "", "al r0 sp 0x2a4", "al_r0_sp_0x2a4"},
    {{al, r0, sp, 0x2a8}, "", "al r0 sp 0x2a8", "al_r0_sp_0x2a8"},
    {{al, r0, sp, 0x2ac}, "", "al r0 sp 0x2ac", "al_r0_sp_0x2ac"},
    {{al, r0, sp, 0x2b0}, "", "al r0 sp 0x2b0", "al_r0_sp_0x2b0"},
    {{al, r0, sp, 0x2b4}, "", "al r0 sp 0x2b4", "al_r0_sp_0x2b4"},
    {{al, r0, sp, 0x2b8}, "", "al r0 sp 0x2b8", "al_r0_sp_0x2b8"},
    {{al, r0, sp, 0x2bc}, "", "al r0 sp 0x2bc", "al_r0_sp_0x2bc"},
    {{al, r0, sp, 0x2c0}, "", "al r0 sp 0x2c0", "al_r0_sp_0x2c0"},
    {{al, r0, sp, 0x2c4}, "", "al r0 sp 0x2c4", "al_r0_sp_0x2c4"},
    {{al, r0, sp, 0x2c8}, "", "al r0 sp 0x2c8", "al_r0_sp_0x2c8"},
    {{al, r0, sp, 0x2cc}, "", "al r0 sp 0x2cc", "al_r0_sp_0x2cc"},
    {{al, r0, sp, 0x2d0}, "", "al r0 sp 0x2d0", "al_r0_sp_0x2d0"},
    {{al, r0, sp, 0x2d4}, "", "al r0 sp 0x2d4", "al_r0_sp_0x2d4"},
    {{al, r0, sp, 0x2d8}, "", "al r0 sp 0x2d8", "al_r0_sp_0x2d8"},
    {{al, r0, sp, 0x2dc}, "", "al r0 sp 0x2dc", "al_r0_sp_0x2dc"},
    {{al, r0, sp, 0x2e0}, "", "al r0 sp 0x2e0", "al_r0_sp_0x2e0"},
    {{al, r0, sp, 0x2e4}, "", "al r0 sp 0x2e4", "al_r0_sp_0x2e4"},
    {{al, r0, sp, 0x2e8}, "", "al r0 sp 0x2e8", "al_r0_sp_0x2e8"},
    {{al, r0, sp, 0x2ec}, "", "al r0 sp 0x2ec", "al_r0_sp_0x2ec"},
    {{al, r0, sp, 0x2f0}, "", "al r0 sp 0x2f0", "al_r0_sp_0x2f0"},
    {{al, r0, sp, 0x2f4}, "", "al r0 sp 0x2f4", "al_r0_sp_0x2f4"},
    {{al, r0, sp, 0x2f8}, "", "al r0 sp 0x2f8", "al_r0_sp_0x2f8"},
    {{al, r0, sp, 0x2fc}, "", "al r0 sp 0x2fc", "al_r0_sp_0x2fc"},
    {{al, r0, sp, 0x300}, "", "al r0 sp 0x300", "al_r0_sp_0x300"},
    {{al, r0, sp, 0x304}, "", "al r0 sp 0x304", "al_r0_sp_0x304"},
    {{al, r0, sp, 0x308}, "", "al r0 sp 0x308", "al_r0_sp_0x308"},
    {{al, r0, sp, 0x30c}, "", "al r0 sp 0x30c", "al_r0_sp_0x30c"},
    {{al, r0, sp, 0x310}, "", "al r0 sp 0x310", "al_r0_sp_0x310"},
    {{al, r0, sp, 0x314}, "", "al r0 sp 0x314", "al_r0_sp_0x314"},
    {{al, r0, sp, 0x318}, "", "al r0 sp 0x318", "al_r0_sp_0x318"},
    {{al, r0, sp, 0x31c}, "", "al r0 sp 0x31c", "al_r0_sp_0x31c"},
    {{al, r0, sp, 0x320}, "", "al r0 sp 0x320", "al_r0_sp_0x320"},
    {{al, r0, sp, 0x324}, "", "al r0 sp 0x324", "al_r0_sp_0x324"},
    {{al, r0, sp, 0x328}, "", "al r0 sp 0x328", "al_r0_sp_0x328"},
    {{al, r0, sp, 0x32c}, "", "al r0 sp 0x32c", "al_r0_sp_0x32c"},
    {{al, r0, sp, 0x330}, "", "al r0 sp 0x330", "al_r0_sp_0x330"},
    {{al, r0, sp, 0x334}, "", "al r0 sp 0x334", "al_r0_sp_0x334"},
    {{al, r0, sp, 0x338}, "", "al r0 sp 0x338", "al_r0_sp_0x338"},
    {{al, r0, sp, 0x33c}, "", "al r0 sp 0x33c", "al_r0_sp_0x33c"},
    {{al, r0, sp, 0x340}, "", "al r0 sp 0x340", "al_r0_sp_0x340"},
    {{al, r0, sp, 0x344}, "", "al r0 sp 0x344", "al_r0_sp_0x344"},
    {{al, r0, sp, 0x348}, "", "al r0 sp 0x348", "al_r0_sp_0x348"},
    {{al, r0, sp, 0x34c}, "", "al r0 sp 0x34c", "al_r0_sp_0x34c"},
    {{al, r0, sp, 0x350}, "", "al r0 sp 0x350", "al_r0_sp_0x350"},
    {{al, r0, sp, 0x354}, "", "al r0 sp 0x354", "al_r0_sp_0x354"},
    {{al, r0, sp, 0x358}, "", "al r0 sp 0x358", "al_r0_sp_0x358"},
    {{al, r0, sp, 0x35c}, "", "al r0 sp 0x35c", "al_r0_sp_0x35c"},
    {{al, r0, sp, 0x360}, "", "al r0 sp 0x360", "al_r0_sp_0x360"},
    {{al, r0, sp, 0x364}, "", "al r0 sp 0x364", "al_r0_sp_0x364"},
    {{al, r0, sp, 0x368}, "", "al r0 sp 0x368", "al_r0_sp_0x368"},
    {{al, r0, sp, 0x36c}, "", "al r0 sp 0x36c", "al_r0_sp_0x36c"},
    {{al, r0, sp, 0x370}, "", "al r0 sp 0x370", "al_r0_sp_0x370"},
    {{al, r0, sp, 0x374}, "", "al r0 sp 0x374", "al_r0_sp_0x374"},
    {{al, r0, sp, 0x378}, "", "al r0 sp 0x378", "al_r0_sp_0x378"},
    {{al, r0, sp, 0x37c}, "", "al r0 sp 0x37c", "al_r0_sp_0x37c"},
    {{al, r0, sp, 0x380}, "", "al r0 sp 0x380", "al_r0_sp_0x380"},
    {{al, r0, sp, 0x384}, "", "al r0 sp 0x384", "al_r0_sp_0x384"},
    {{al, r0, sp, 0x388}, "", "al r0 sp 0x388", "al_r0_sp_0x388"},
    {{al, r0, sp, 0x38c}, "", "al r0 sp 0x38c", "al_r0_sp_0x38c"},
    {{al, r0, sp, 0x390}, "", "al r0 sp 0x390", "al_r0_sp_0x390"},
    {{al, r0, sp, 0x394}, "", "al r0 sp 0x394", "al_r0_sp_0x394"},
    {{al, r0, sp, 0x398}, "", "al r0 sp 0x398", "al_r0_sp_0x398"},
    {{al, r0, sp, 0x39c}, "", "al r0 sp 0x39c", "al_r0_sp_0x39c"},
    {{al, r0, sp, 0x3a0}, "", "al r0 sp 0x3a0", "al_r0_sp_0x3a0"},
    {{al, r0, sp, 0x3a4}, "", "al r0 sp 0x3a4", "al_r0_sp_0x3a4"},
    {{al, r0, sp, 0x3a8}, "", "al r0 sp 0x3a8", "al_r0_sp_0x3a8"},
    {{al, r0, sp, 0x3ac}, "", "al r0 sp 0x3ac", "al_r0_sp_0x3ac"},
    {{al, r0, sp, 0x3b0}, "", "al r0 sp 0x3b0", "al_r0_sp_0x3b0"},
    {{al, r0, sp, 0x3b4}, "", "al r0 sp 0x3b4", "al_r0_sp_0x3b4"},
    {{al, r0, sp, 0x3b8}, "", "al r0 sp 0x3b8", "al_r0_sp_0x3b8"},
    {{al, r0, sp, 0x3bc}, "", "al r0 sp 0x3bc", "al_r0_sp_0x3bc"},
    {{al, r0, sp, 0x3c0}, "", "al r0 sp 0x3c0", "al_r0_sp_0x3c0"},
    {{al, r0, sp, 0x3c4}, "", "al r0 sp 0x3c4", "al_r0_sp_0x3c4"},
    {{al, r0, sp, 0x3c8}, "", "al r0 sp 0x3c8", "al_r0_sp_0x3c8"},
    {{al, r0, sp, 0x3cc}, "", "al r0 sp 0x3cc", "al_r0_sp_0x3cc"},
    {{al, r0, sp, 0x3d0}, "", "al r0 sp 0x3d0", "al_r0_sp_0x3d0"},
    {{al, r0, sp, 0x3d4}, "", "al r0 sp 0x3d4", "al_r0_sp_0x3d4"},
    {{al, r0, sp, 0x3d8}, "", "al r0 sp 0x3d8", "al_r0_sp_0x3d8"},
    {{al, r0, sp, 0x3dc}, "", "al r0 sp 0x3dc", "al_r0_sp_0x3dc"},
    {{al, r0, sp, 0x3e0}, "", "al r0 sp 0x3e0", "al_r0_sp_0x3e0"},
    {{al, r0, sp, 0x3e4}, "", "al r0 sp 0x3e4", "al_r0_sp_0x3e4"},
    {{al, r0, sp, 0x3e8}, "", "al r0 sp 0x3e8", "al_r0_sp_0x3e8"},
    {{al, r0, sp, 0x3ec}, "", "al r0 sp 0x3ec", "al_r0_sp_0x3ec"},
    {{al, r0, sp, 0x3f0}, "", "al r0 sp 0x3f0", "al_r0_sp_0x3f0"},
    {{al, r0, sp, 0x3f4}, "", "al r0 sp 0x3f4", "al_r0_sp_0x3f4"},
    {{al, r0, sp, 0x3f8}, "", "al r0 sp 0x3f8", "al_r0_sp_0x3f8"},
    {{al, r0, sp, 0x3fc}, "", "al r0 sp 0x3fc", "al_r0_sp_0x3fc"}};

// These headers each contain an array of `TestResult` with the reference output
// values. The reference arrays are names `kReference{mnemonic}`.
#include "aarch32/traces/assembler-cond-rd-sp-operand-imm8-t32-add.h"

// The maximum number of errors to report in detail for each test.
static const unsigned kErrorReportLimit = 8;

typedef void (MacroAssembler::*Fn)(Condition cond, Register rd, Register rn,
                                   const Operand& op);

static void TestHelper(Fn instruction, const char* mnemonic,
                       const TestResult reference[]) {
  unsigned total_error_count = 0;
  MacroAssembler masm(BUF_SIZE);

  masm.SetT32(true);

  for (unsigned i = 0; i < ARRAY_SIZE(kTests); i++) {
    // Values to pass to the macro-assembler.
    Condition cond = kTests[i].operands.cond;
    Register rd = kTests[i].operands.rd;
    Register rn = kTests[i].operands.rn;
    uint32_t immediate = kTests[i].operands.immediate;
    Operand op(immediate);

    uint32_t start = masm.GetCursorOffset();
    (masm.*instruction)(cond, rd, rn, op);
    uint32_t end = masm.GetCursorOffset();

    const byte* result_ptr =
        masm.GetBuffer().GetOffsetAddress<const byte*>(start);
    uint32_t result_size = end - start;

    if (Test::generate_test_trace()) {
      // Print the result bytes.
      printf("static const byte kInstruction_%s_%s[] = {\n", mnemonic,
             kTests[i].identifier);
      for (uint32_t j = 0; j < result_size; j++) {
        if (j == 0) {
          printf("  0x%02" PRIx8, result_ptr[j]);
        } else {
          printf(", 0x%02" PRIx8, result_ptr[j]);
        }
      }
      // This comment is meant to be used by external tools to validate
      // the encoding. We can parse the comment to figure out what
      // instruction this corresponds to.
      printf(" // %s %s %s\n};\n", kTests[i].expect_instruction_before,
             mnemonic, kTests[i].operands_description);
    } else {
      // Check we've emitted the exact same encoding as present in the
      // trace file. Only print up to `kErrorReportLimit` errors.
      if (((result_size != reference[i].size) ||
           (memcmp(result_ptr, reference[i].encoding, reference[i].size) !=
            0)) &&
          (++total_error_count <= kErrorReportLimit)) {
        printf("Error when testing \"%s\" with operands \"%s\":\n", mnemonic,
               kTests[i].operands_description);
        printf("  Expected: ");
        for (uint32_t j = 0; j < reference[i].size; j++) {
          if (j == 0) {
            printf("0x%02" PRIx8, reference[i].encoding[j]);
          } else {
            printf(", 0x%02" PRIx8, reference[i].encoding[j]);
          }
        }
        printf("\n");
        printf("  Found:    ");
        for (uint32_t j = 0; j < result_size; j++) {
          if (j == 0) {
            printf("0x%02" PRIx8, result_ptr[j]);
          } else {
            printf(", 0x%02" PRIx8, result_ptr[j]);
          }
        }
        printf("\n");
      }
    }
  }

  masm.FinalizeCode();

  if (Test::generate_test_trace()) {
    // Finalize the trace file by writing the final `TestResult` array
    // which links all generated instruction encodings.
    printf("static const TestResult kReference%s[] = {\n", mnemonic);
    for (unsigned i = 0; i < ARRAY_SIZE(kTests); i++) {
      printf("  {\n");
      printf("    ARRAY_SIZE(kInstruction_%s_%s),\n", mnemonic,
             kTests[i].identifier);
      printf("    kInstruction_%s_%s,\n", mnemonic, kTests[i].identifier);
      printf("  },\n");
    }
    printf("};\n");
  } else {
    if (total_error_count > kErrorReportLimit) {
      printf("%u other errors follow.\n",
             total_error_count - kErrorReportLimit);
    }
  }
}

// Instantiate tests for each instruction in the list.
#define TEST(mnemonic)                                                      \
  static void Test_##mnemonic() {                                           \
    TestHelper(&MacroAssembler::mnemonic, #mnemonic, kReference##mnemonic); \
  }                                                                         \
  static Test test_##mnemonic(                                              \
      "AARCH32_ASSEMBLER_COND_RD_SP_OPERAND_IMM8_T32_" #mnemonic,           \
      &Test_##mnemonic);
FOREACH_INSTRUCTION(TEST)
#undef TEST

}  // aarch32
}  // vixl
