// Copyright 2022, VIXL authors
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
#include "aarch64/io-reporter-aarch64.h"

using namespace std;
using namespace vixl;
using namespace vixl::aarch64;

// Example of using the IOReporter component to print the source and destination
// registers used by individual instructions.

#ifndef TEST_EXAMPLES
int main(void) {
  MacroAssembler m;
  Decoder d;
  Disassembler dis;
  IOReporter ior;
  d.AppendVisitor(&dis);
  d.AppendVisitor(&ior);

  CPUFeatures f;
  f.Combine(CPUFeatures::kPAuth, CPUFeatures::kNEON);
  m.SetCPUFeatures(f);

  // Sample code that uses a range of input and output registers - not
  // something that should be executed.
  Label fn;
  m.Bind(&fn);
  m.Paciasp();
  m.Mov(x0, 42);
  m.Mov(x1, x10);
  m.Add(x0, x0, 42);
  m.Str(x9, MemOperand(x10, 8, PostIndex));
  m.Ld4(v30.V8B(),
        v31.V8B(),
        v0.V8B(),
        v1.V8B(),
        MemOperand(x0, x2, PostIndex));
  m.FinalizeCode();

  Instruction* c = m.GetLabelAddress<Instruction*>(&fn);
  d.Decode(c);  // paciasp
  cout << dis.GetOutput() << " reads: ";
  unordered_set<CPURegister> r = ior.GetSourceRegisters();
  for (CPURegister reg : r) {
    cout << reg.GetArchitecturalName() << " ";
  }
  cout << endl;

  ior.Reset();
  d.Decode(c + 4);  // movz x0, #42
  cout << dis.GetOutput() << " writes: ";
  unordered_set<CPURegister> w = ior.GetDestinationRegisters();
  for (CPURegister reg : w) {
    cout << reg.GetArchitecturalName() << " ";
  }
  cout << endl;

  ior.Reset();
  d.Decode(c + 8);  // mov x1, x10
  cout << dis.GetOutput() << "; ";
  d.Decode(c + 12);  // add x0, x0, #42
  cout << dis.GetOutput() << " reads: ";
  r = ior.GetSourceRegisters();
  for (CPURegister reg : r) {
    cout << reg.GetArchitecturalName() << " ";
  }
  cout << endl;

  ior.Reset();
  d.Decode(c + 16);  // str x9, [x10], #8
  cout << dis.GetOutput() << " reads: ";
  r = ior.GetSourceRegisters();
  for (CPURegister reg : r) {
    cout << reg.GetArchitecturalName() << " ";
  }
  cout << ", writes: ";
  w = ior.GetDestinationRegisters();
  for (CPURegister reg : w) {
    cout << reg.GetArchitecturalName() << " ";
  }
  cout << endl;

  ior.Reset();
  d.Decode(c + 20);  // ld4 {v30.8b, v31.8b, v0.8b, v1.8b}, [x0], x2
  cout << dis.GetOutput() << " reads: ";
  r = ior.GetSourceRegisters();
  for (CPURegister reg : r) {
    cout << reg.GetArchitecturalName() << " ";
  }
  cout << ", writes: ";
  w = ior.GetDestinationRegisters();
  for (CPURegister reg : w) {
    cout << reg.GetArchitecturalName() << " ";
  }
  cout << endl;

  return 0;
}
#endif  // TEST_EXAMPLES
