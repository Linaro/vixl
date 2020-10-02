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

#if __cplusplus < 201703L
#error "Text Assembler requires C++17 standard or above."
#else

#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

#include "tasm/tasm-assembler.h"
#include "aarch64/simulator-aarch64.h"

using namespace vixl;
using namespace vixl::aarch64;
using namespace vixl::aarch64::tasm;

int const kMemSize = 0x8000000;

struct ControlFlags {
  bool disassemble;
  bool simulate;
  bool coloured_trace;
  int log_output;

  ControlFlags()
      : disassemble(false),
        simulate(true),
        coloured_trace(false),
        log_output(0) {}
};

void ExitWithUsage(const char* exe_name, int code) {
  std::cout << exe_name << ": tool executing given assembly program\n";
  std::cout << "USAGE: " << exe_name << " --file <filepath> <options>\n";
  std::cout << "OPTIONS:\n";
  std::cout << "  --help               print program usage\n";
  std::cout << "  --file <filepath>    path to file with assembly code\n";
  std::cout << "  --trace-output       show general purpose registers, sve"
            << " registers and disassembly ouput from simulator\n";
  std::cout << "  --coloured-trace     print coloured output\n";
  std::cout << "  --disassemble        run program through disassembler as a"
            << " separate phase and print the output\n";
  std::cout << "  --no-simulate        omit executing program in simulator\n";
  exit(code);
}

// By default this program simulates assembly code in simulator and prints no
// trace. User can modify output and behaviour by suitable flags.
ControlFlags ParseArguments(int args_num,
                            char** args_str,
                            std::string* filename) {
  ControlFlags cf;

  for (int i = 1; i < args_num; i++) {
    if (!std::strcmp(args_str[i], "--file") && i + 1 < args_num) {
      *filename = args_str[i + 1];
      i++;
    } else if (!std::strcmp(args_str[i], "--help")) {
      ExitWithUsage(args_str[0], 0);
    } else if (!std::strcmp(args_str[i], "--trace-output")) {
      cf.log_output = LOG_ALL;
    } else if (!std::strcmp(args_str[i], "--coloured-trace")) {
      cf.coloured_trace = true;
    } else if (!std::strcmp(args_str[i], "--disassemble")) {
      cf.disassemble = true;
    } else if (!std::strcmp(args_str[i], "--no-simulate")) {
      cf.simulate = false;
    } else {
      std::cout << "Error: Unrecognized command line argument: " << args_str[i]
                << "\n\n";
      ExitWithUsage(args_str[0], EXIT_FAILURE);
    }
  }
  if (filename->empty()) {
    std::cout << "Error: No path to assembly program proivided\n\n";
    ExitWithUsage(args_str[0], EXIT_FAILURE);
  }
  return cf;
}

// Read content of a file with assembly source code and pass each line to
// TextAssembler.
bool ReadFile(TextAssembler* tasm, std::string filename) {
  std::ifstream asm_file(filename);
  std::string line;

  if (asm_file.is_open()) {
    while (std::getline(asm_file, line)) tasm->Assemble(line);
    asm_file.close();
    tasm->FinalizeCode();
  } else {
    std::cout << "Error: Couldn't open file " << filename << std::endl;
    return false;
  }
  return true;
}

int main(int argc, char** argv) {
  TextAssembler tasm;
  CodeBuffer* cb = tasm.GetBuffer();
  std::string asm_file;

  ControlFlags cf = ParseArguments(argc, argv, &asm_file);

  if (!ReadFile(&tasm, asm_file)) return EXIT_FAILURE;

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64
  if (cf.simulate) {
    Decoder dec;
    Simulator sim(&dec);
    uint8_t* data = new uint8_t[kMemSize];

    memset(data, 0, kMemSize);
    // By default there is allocated memory block of 128MB size. Its address is
    // written to x0 register.
    sim.WriteXRegister(0, reinterpret_cast<uintptr_t>(&data[kMemSize]));
    sim.SetColouredTrace(cf.coloured_trace);
    sim.SetTraceParameters(cf.log_output);

    std::cout << "Simulator trace: " << std::endl;
    sim.RunFrom(cb->GetStartAddress<Instruction*>());
  }
#endif
  if (cf.disassemble) {
    PrintDisassembler disasm(stdout);
    std::cout << "Disassembler trace: " << std::endl;
    disasm.DisassembleBuffer(cb->GetStartAddress<Instruction*>(),
                             cb->GetEndAddress<Instruction*>());
  }
  return 0;
}

#endif  // __cplusplus >= 201703L
