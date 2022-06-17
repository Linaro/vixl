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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "code-buffer-vixl.h"
#include "aarch64/decoder-aarch64.h"
#include "aarch64/disasm-aarch64.h"

// This example is interactive, and isn't tested systematically.
#ifndef TEST_EXAMPLES

using namespace vixl;
using namespace vixl::aarch64;

class PrintDisassemblerWithPC : public PrintDisassembler {
 public:
  explicit PrintDisassemblerWithPC(FILE* stream)
      : PrintDisassembler(stream), show_pc_(false), pc_(0) { }

  void ShowPC(int64_t pc) {
    SetDefaultLinePrefix("   ");
    show_pc_ = true;
    pc_ = pc;
  }

 protected:
  virtual void ProcessOutput(const Instruction* instr) VIXL_OVERRIDE {
    // Align, to make sure that C64 bottom-bit-set addresses point to the
    // right place.
    uint64_t pc_cmp = AlignDown(pc_, sizeof(Instr));
    uint64_t addr_cmp = AlignDown(CodeRelativeAddress(instr), sizeof(Instr));
    if (show_pc_ && (pc_cmp == addr_cmp)) {
      SetNextDisassemblyPrefix("=> ");
    }
    PrintDisassembler::ProcessOutput(instr);
  }

 private:
  bool show_pc_;
  int64_t pc_;
};

void PrintUsage(char const* name) {
  printf("Usage: %s [OPTION]... <INSTRUCTION>...\n", name);
  printf("\n");
  printf("Disassemble ad-hoc A64 or C64 instructions.\n");
  printf("\n");
  printf(
      "Options:\n"
      "  --start-at <address>\n"
      "    Start disassembling from <address> Any signed 64-bit value\n"
      "    accepted by strtoll can be specified. The address is printed\n"
      "    alongside each instruction, and it is also used to decode\n"
      "    PC-relative offsets.\n"
      "\n"
      "  --a64\n"
      "    Disassemble as A64. This is the default.\n"
      "\n"
      "  --c64\n"
      "    Disassemble as C64.\n"
      "\n"
      "  --pc <address>\n"
      "    If included in the output, point to the PC's address. Useful\n"
      "    for GDB integration with VIXL's `tools/vixl-disasm.gdb`.\n"
      "\n");
  printf(
      "<instruction>\n"
      "  A hexadecimal representation of an A64 instruction. The leading '0x'\n"
      "  (or '0X') is optional.\n"
      "\n"
      "  Multiple instructions can be provided; they will be disassembled as\n"
      "  if they were read sequentially from memory.\n"
      "\n");
  printf("Examples:\n");
  printf("  $ %s d2824685\n", name);
  printf("  # ISA: A64\n");
  printf("   0x0000000000000000: d2824685  movz x5, #0x1234\n");
  printf("\n");
  printf("  $ %s --start-at -4 0x10fffe85 0xd61f00a0\n", name);
  printf("  # ISA: A64\n");
  printf("  -0x0000000000000004: 10fffe85  adr x5, #-0x30 (addr -0x34)\n");
  printf("   0x0000000000000000: d61f00a0  br x5\n");
  printf("\n");
  printf("  $ %s --c64 0x10fffe85 0xc2c273e0 --start-at 0x420\n", name);
  printf("  # ISA: C64\n");
  printf("   0x0000000000000420: 10fffe85  adr c5, #-0x30 (addr 0x3f0)\n");
  printf("   0x0000000000000424: c2c273e0  bx #4\n");
  printf("\n");
  printf("  $ %s --c64 0x10fffe85 0xc2c273e0 --start-at 0x420 --pc 0x424\n", name);
  printf("  # ISA: C64\n");
  printf("      0x0000000000000420: 10fffe85  adr c5, #-0x30 (addr 0x3f0)\n");
  printf("   => 0x0000000000000424: c2c273e0  bx #4\n");
}

Instr ParseInstr(char const* arg) {
  // TODO: Error handling for out-of-range inputs.
  return (Instr)strtoul(arg, NULL, 16);
}

int64_t ParseInt64(char const* arg) {
  // TODO: Error handling for out-of-range inputs.
  return (int64_t)strtoll(arg, NULL, 0);
}

int main(int argc, char* argv[]) {
  for (int i = 1; i < argc; i++) {
    char const* arg = argv[i];
    if ((strcmp(arg, "--help") == 0) || (strcmp(arg, "-h") == 0)) {
      PrintUsage(argv[0]);
      return 0;
    }
  }

  // Assume an address of 0, unless otherwise specified.
  int64_t start_address = 0;
  // Default to A64.
  ISA isa = ISA::A64;
  // Allocate space for one instruction per argument.
  CodeBuffer buffer((argc - 1) * kInstructionSize);

  bool show_pc = false;
  int64_t pc = 0;

  bool expect_start_at = false;
  bool expect_pc = false;
  for (int i = 1; i < argc; i++) {
    char* arg = argv[i];
    if (expect_start_at) {
      start_address = ParseInt64(arg);
      expect_start_at = false;
    } else if (expect_pc) {
      pc = ParseInt64(arg);
      expect_pc = false;
      show_pc = true;
    } else if (strcmp(arg, "--start-at") == 0) {
      expect_start_at = true;
    } else if (strcmp(arg, "--pc") == 0) {
      expect_pc = true;
    } else if (strcmp(arg, "--a64") == 0) {
      isa = ISA::A64;
    } else if (strcmp(arg, "--c64") == 0) {
      isa = ISA::C64;
    } else {
      // Assume that everything else is an instruction.
      buffer.Emit(ParseInstr(arg));
    }
  }
  buffer.SetClean();

  if (expect_start_at) {
    printf("No address given. Use: --start-at <address>\n");
    return 1;
  }

  if (expect_pc) {
    printf("No address given. Use: --pc <address>\n");
    return 1;
  }

  if (buffer.GetSizeInBytes() == 0) {
    printf("Nothing to disassemble.\n");
    return 0;
  }

  // Disassemble the buffer.
  const Instruction* start = buffer.GetStartAddress<Instruction*>();
  const Instruction* end = buffer.GetEndAddress<Instruction*>();
  PrintDisassemblerWithPC disasm(stdout);
  if (show_pc) disasm.ShowPC(pc);
  disasm.PrintSignedAddresses(true);
  disasm.MapCodeAddress(start_address, start);
  ISAMap map(isa);
  disasm.DisassembleBuffer(start, end, &map);

  return 0;
}

#endif  // TEST_EXAMPLES
