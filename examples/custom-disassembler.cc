// Copyright 2014, ARM Limited
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

#include "examples.h"
#include "custom-disassembler.h"


#define BUF_SIZE (4096)
#define __ masm->


void CustomDisassembler::AppendRegisterNameToOutput(
    const Instruction* instr,
    const CPURegister& reg) {
  USE(instr);
  if (reg.IsRegister()) {
    switch (reg.code()) {
      case 16:
        AppendToOutput(reg.Is64Bits() ? "ip0" : "wip0");
        return;
      case 17:
        AppendToOutput(reg.Is64Bits() ? "ip1" : "wip1");
        return;
      case 30:
        AppendToOutput(reg.Is64Bits() ? "lr" : "w30");
        return;
      case kSPRegInternalCode:
        AppendToOutput(reg.Is64Bits() ? "x_stack_pointer" : "w_stack_pointer");
        return;
      case 31:
        AppendToOutput(reg.Is64Bits() ? "x_zero_reg" : "w_zero-reg");
        return;
      default:
        // Fall through.
        break;
    }
  }
  // Print other register names as usual.
  Disassembler::AppendRegisterNameToOutput(instr, reg);
}


static const char* FakeLookupAddressDescription(const void* address) {
  USE(address);
  // We fake looking up the address in a table. We behave as if the first and
  // third address we are asked about were function entries.
  static int i = 0;
  const char* desc = NULL;
  if (i == 0) {
    desc = "function: foo";
  } else if (i == 2) {
    desc = "function: bar";
  }
  i++;
  return desc;
}


void CustomDisassembler::AppendCodeAddressToOutput(
    const Instruction* instr, const void* addr) {
  USE(instr);
  const char* address_desc = FakeLookupAddressDescription(addr);
  // Print the raw address and - if available - its description.
  AppendToOutput("(addr %p", addr);
  if (address_desc != NULL) {
    Disassembler::AppendToOutput(" ; %s", address_desc);
  }
  AppendToOutput(")");
}


void CustomDisassembler::VisitAddSubShifted(const Instruction* instr) {
  vixl::Disassembler::VisitAddSubShifted(instr);
  if (instr->Rd() == vixl::x10.code()) {
    AppendToOutput(" // add/sub to x10");
  }
  ProcessOutput(instr);
}


void GenerateCustomDisassemblerTestCode(MacroAssembler* masm) {
  // Generate some code to illustrate how the modified disassembler changes the
  // disassembly output.
  Label begin, end;
  __ Bind(&begin);
  __ Add(x10, x16, x17);
  __ Cbz(x10, &end);
  __ Add(x11, ip0, ip1);
  __ Add(w5, w6, w30);
  __ Tbz(x10, 2, &begin);
  __ Tbnz(x10, 3, &begin);
  __ Br(x30);
  __ Br(lr);
  __ Fadd(d30, d16, d17);
  __ Push(xzr, xzr);
  __ Pop(x16, x20);
  __ Bind(&end);
}


void TestCustomDisassembler() {
  // Create and initialize the assembler.
  byte assm_buf[BUF_SIZE];
  MacroAssembler masm(assm_buf, BUF_SIZE);

  // Generate the code.
  Label code_start, code_end;
  masm.Bind(&code_start);
  GenerateCustomDisassemblerTestCode(&masm);
  masm.Bind(&code_end);
  masm.FinalizeCode();
  Instruction* instr_start = masm.GetLabelAddress<Instruction*>(&code_start);
  Instruction* instr_end = masm.GetLabelAddress<Instruction*>(&code_end);

  // Instantiate a standard disassembler, our custom disassembler, and register
  // them with a decoder.
  Decoder decoder;
  Disassembler disasm;
  CustomDisassembler custom_disasm;
  decoder.AppendVisitor(&disasm);
  decoder.AppendVisitor(&custom_disasm);

  // Iterate through the instructions to show the difference in the disassembly.
  Instruction* instr;
  for (instr = instr_start; instr < instr_end; instr += kInstructionSize) {
    decoder.Decode(instr);
    printf("\n");
    printf("VIXL disasm:   %s\n", disasm.GetOutput());
    printf("custom disasm: %s\n", custom_disasm.GetOutput());
  }
}


#ifndef TEST_EXAMPLES
int main() {
  TestCustomDisassembler();
  return 0;
}
#endif
