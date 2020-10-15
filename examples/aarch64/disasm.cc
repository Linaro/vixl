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

#include "aarch64/decoder-aarch64.h"
#include "aarch64/disasm-aarch64.h"

// This example is interactive, and isn't tested systematically.
#ifndef TEST_EXAMPLES

using namespace vixl;
using namespace vixl::aarch64;

void PrintUsage(char const* name) {
  printf("Usage: %s [<address>:]<instruction> ...\n", name);
  printf("\n");
  printf("Disassemble ad-hoc A64 instructions.\n");
  printf("\n");
  printf(
      "<address>\n"
      "  The address of the instruction. Any signed 64-bit value accepted by\n"
      "  strtoll can be specified. The address is printed alongside each\n"
      "  instruction, and it is also used to decode PC-relative offsets.\n"
      "\n"
      "  If no address is specified, it is derived from the previously-\n"
      "  disassembled instruction. The default address for the first\n"
      "  instruction is 0.\n"
      "\n");
  printf(
      "<instruction>\n"
      "  A hexadecimal representation of an A64 instruction. The leading '0x'\n"
      "  (or '0X') is optional.\n"
      "\n"
      "  Multiple instructions can be provided; unless qualified with an\n"
      "  address, they will be disassembled as if they were read sequentially\n"
      "  from memory.\n"
      "\n");
  printf("Examples:\n");
  printf("  $ %s d2824685\n", name);
  printf("   0x0000000000000000:  d2824685  movz x5, #0x1234\n");
  printf("\n");
  printf("  $ %s -4:0x10fffe85 0xd61f00a0\n", name);
  printf("  -0x0000000000000004:  10fffe85  adr x5, #-0x30 (addr -0x34)\n");
  printf("   0x0000000000000000:  d61f00a0  br x5\n");
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
  if (argc < 2) {
    PrintUsage(argv[0]);
    return 1;
  }

  for (int i = 1; i < argc; i++) {
    char const* arg = argv[i];
    if ((strcmp(arg, "--help") == 0) || (strcmp(arg, "-h") == 0)) {
      PrintUsage(argv[0]);
      return 0;
    }
  }

  // Assume an address of 0, unless otherwise specified.
  int64_t address = 0;

  vixl::aarch64::Decoder decoder;
  vixl::aarch64::Disassembler disasm;
  decoder.AppendVisitor(&disasm);

  for (int i = 1; i < argc; i++) {
    char* arg = argv[i];

    // If there is a ':' in the input, the first part indicates an address.
    char* split = strchr(arg, ':');
    if (split) {
      *split = '\0';
      int64_t new_address = ParseInt64(arg);
      if ((new_address != address) && (i != 1)) {
        // Print a short gap if the address range is broken.
        printf("\n");
      }
      address = new_address;
      arg = split + 1;
    }

    // We always assume that this succeeds, but print the encoding with the
    // output so the user can see if the instruction was misinterpreted.
    Instr instruction_raw = ParseInstr(arg);
    vixl::aarch64::Instruction* instruction =
        reinterpret_cast<vixl::aarch64::Instruction*>(&instruction_raw);
    disasm.MapCodeAddress(address, instruction);
    decoder.Decode(instruction);

    char sign_char = (address < 0) ? '-' : ' ';
    uint64_t abs_address =
        (address < 0) ? -static_cast<uint64_t>(address) : address;

    printf("%c0x%016" PRIx64 ":\t%08" PRIx32 "\t%s\n",
           sign_char,
           abs_address,
           instruction_raw,
           disasm.GetOutput());

    address += vixl::aarch64::kInstructionSize;
  }

  return 0;
}

#endif  // TEST_EXAMPLES
