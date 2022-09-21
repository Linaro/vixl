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
// registers used by a code sequence executed on the simulator.

#ifndef TEST_EXAMPLES
#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64
int main(void) {
  MacroAssembler m;
  Decoder d;
  Simulator s(&d);
  IOReporter ior;
  d.AppendVisitor(&ior);

  Label fn, loop;
  m.Bind(&fn);
  m.Mov(x0, xzr);
  m.Mov(x7, 10);
  m.Bind(&loop);
  m.Add(x0, x0, 42);
  m.Sub(x7, x7, 1);
  m.Cbnz(x7, &loop);
  m.Ret();
  m.FinalizeCode();
  m.GetBuffer()->SetExecutable();
  s.RunFrom(m.GetLabelAddress<Instruction*>(&fn));

  unordered_set<CPURegister> r = ior.GetSourceRegisters();
  cout << "Sequence reads: ";
  for (CPURegister reg : r) {
    cout << reg.GetArchitecturalName() << " ";
  }
  unordered_set<CPURegister> w = ior.GetDestinationRegisters();
  cout << ", writes: ";
  for (CPURegister reg : w) {
    cout << reg.GetArchitecturalName() << " ";
  }
  cout << endl;

  return 0;
}
#else
// Without the simulator there is nothing to run.
int main(void) { return 0; }
#endif  // VIXL_INCLUDE_SIMULATOR_AARCH64
#endif  // TEST_EXAMPLES
