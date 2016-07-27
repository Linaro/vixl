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

#include "examples.h"

#define __ masm->

void GenerateGradeComment(MacroAssembler* masm) {
  // Given a grade 'A', 'B', 'C', 'D' or 'F', print a relevant
  // comment.
  // void GradeComment(char grade)
  Label invalid_input;

  __ Subs(r1, r0, 'A');
  __ B(lt, &invalid_input);

  Label break_switch;
  // Manage the cases from 'A' to 'F' included.
  // Switch/Case handles inputs from [0, n[
  // and here n is 'F' - 'A' + 1 which is 6
  // 'E' will be handled in default case.
  JumpTable8bitOffset grade_comment('F' - 'A' + 1);
  __ Switch(r1, &grade_comment);

  __ Case(&grade_comment, 'A' - 'A');
  __ Printf("Excellent!!\n");
  __ Break(&grade_comment);

  __ Case(&grade_comment, 'B' - 'A');
  __ Printf("Good job!\n");
  __ Break(&grade_comment);

  __ Case(&grade_comment, 'C' - 'A');
  __ Printf("You passed\n");
  __ Break(&grade_comment);

  __ Case(&grade_comment, 'D' - 'A');
  __ Printf("Could do better\n");
  __ Break(&grade_comment);

  __ Case(&grade_comment, 'F' - 'A');
  __ Printf("You failed!\n");
  __ Break(&grade_comment);

  __ Default(&grade_comment);
  __ Bind(&invalid_input);
  __ Printf("Invalid input: %c\n", r0);
  __ EndSwitch(&grade_comment);

  __ Bx(lr);
}

#ifndef TEST_EXAMPLES
int main() {
  MacroAssembler masm(T32);
  // Generate the code for the example function.
  Label entry_point;
  masm.Bind(&entry_point);
  GenerateGradeComment(&masm);
  masm.FinalizeCode();
#ifdef VIXL_INCLUDE_SIMULATOR
  // There is no simulator defined for VIXL AArch32.
  printf("This example cannot be simulated\n");
#else
  byte* code = masm.GetBuffer().GetBuffer();
  uint32_t code_size = masm.GetBuffer().GetSizeInBytes();
  ExecutableMemory memory(code, code_size);
  // Run the example function.
  void (*comment_function)(char grade) =
      memory.GetEntryPoint<void (*)(char)>(entry_point);
  (*comment_function)('A');
  (*comment_function)('B');
  (*comment_function)('F');
  (*comment_function)('E');
  (*comment_function)('a');
  (*comment_function)('G');
#endif
  return 0;
}
#endif  // TEST_EXAMPLES
