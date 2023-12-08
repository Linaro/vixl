// Copyright 2023, VIXL authors
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

// Tests for the simulator debugger.

#include <fstream>
#include <regex>

#include "test-runner.h"
#include "test-utils.h"

#include "aarch64/macro-assembler-aarch64.h"
#include "aarch64/simulator-aarch64.h"
#include "aarch64/test-utils-aarch64.h"

namespace vixl {
namespace aarch64 {

#define __ masm->
#define TEST(name) TEST_(AARCH64_DBG_##name)

//
// Regex for various types of printing/tracing output.
//

// Matches traced/printed general purpose register output from the simulator,
// e.g:
//   "#             x0: 0x000000000badbeef"
const std::string x_register_trace = "#[\\s]+(x\\d{1,2}|lr|sp): 0x[0-9a-f]+";
// Matches traced/printed vector register output from the simulator, e.g:
//   "#             v0: 0x7ff0f0007f80f0017ff0f0007f80f000"
const std::string v_register_trace = "#[\\s]+(v\\d{1,2}): 0x[0-9a-f]+";

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64
// Run tests with the simulator.

// Generate some basic code which immediately breaks into the debugger.
// This serves as a sandbox for all debugger tests to run in.
void GenerateDebuggerAsm(MacroAssembler* masm) {
  // Create a breakpoint here to break into the debugger.
  __ Brk(0);

  // Do some arithmetic.
  __ Add(x1, x0, 5);
  __ Mov(x2, 2);
  __ Sub(x3, x1, x2);

  __ Ret();
}

// Setup the test environment with the debugger assembler and simulator.
#define SETUP()                                                           \
  MacroAssembler masm;                                                    \
  masm.SetCPUFeatures(CPUFeatures::None());                               \
  masm.SetGenerateSimulatorCode(true);                                    \
  GenerateDebuggerAsm(&masm);                                             \
  masm.FinalizeCode();                                                    \
  Instruction* start = masm.GetBuffer()->GetStartAddress<Instruction*>(); \
  Decoder decoder;                                                        \
  std::istringstream input_stream;                                        \
  char ostream_filename[] = "/tmp/vixl-test-debugger-XXXXXX";             \
  FILE* output_stream = fdopen(mkstemp(ostream_filename), "w");           \
  /* Disassemble the generated code so we can use the addresses later. */ \
  PrintDisassembler disassembler(output_stream);                          \
  disassembler.DisassembleBuffer(start, masm.GetSizeOfCodeGenerated());   \
  fflush(output_stream);                                                  \
  Simulator simulator(&decoder, output_stream);                           \
  simulator.GetDebugger()->SetInputStream(&input_stream);                 \
  simulator.SetColouredTrace(Test::coloured_trace());                     \
  simulator.SetCPUFeatures(CPUFeatures::None());                          \
  simulator.SetDebuggerEnabled(true);                                     \
  /* Setup a map so that commands and their output can be checked. */     \
  std::unordered_map<std::string, std::string> command_map

// Add a command to the input stream queue and map its expected output so that
// it can be checked at the end of simulation.
#define SETUP_CMD(cmd, expected_output)             \
  {                                                 \
    std::string cmd_str(cmd);                       \
    cmd_str += "\n";                                \
    std::string exp_out(expected_output);           \
    input_stream.str(input_stream.str() + cmd_str); \
    command_map.insert({cmd_str, exp_out});         \
  }

// Run the simulator.
#define RUN()               \
  simulator.RunFrom(start); \
  fclose(output_stream)

// Read the output file stream and check that the expected output from each
// command is found directly following it.
#define CHECK_OUTPUT()                                                       \
  std::ifstream file_stream(ostream_filename);                               \
  std::ostringstream ostream;                                                \
  ostream << file_stream.rdbuf();                                            \
  for (const auto& iter : command_map) {                                     \
    std::string cmd = iter.first;                                            \
    std::string expected = iter.second;                                      \
    /* We assume the expected output follows the command that was issued. */ \
    std::regex regex(cmd + expected);                                        \
    if (!std::regex_search(ostream.str(), regex)) {                          \
      printf("output = \n\"%s\"\n", ostream.str().c_str());                  \
      /* Remove the newlines. */                                             \
      cmd.erase(cmd.size() - 1, 1);                                          \
      std::string err =                                                      \
          cmd + " - failed: \"" + expected + "\" not found in output ";      \
      VIXL_ABORT_WITH_MSG(err.c_str());                                      \
    }                                                                        \
  }                                                                          \
  std::remove(ostream_filename)

#define GET_INSTRUCTION_ADDRESS(instruction) \
  GetInstructionAddress(ostream_filename, instruction)

// Get the address of an instruction from the given filename.
std::string GetInstructionAddress(std::string filename,
                                  std::string instruction) {
  std::ifstream file_stream(filename);
  std::ostringstream ostream;
  ostream << file_stream.rdbuf();

  // Match the instruction string and capture the address of that instruction.
  // Note: leading 0's are matched but not captured.
  std::smatch sub_matches;
  std::string str = ostream.str();
  std::string regex_str = "(0x)0*([0-9a-f]+)  [0-9a-f]+\t\t";
  regex_str += instruction;
  std::regex regex(regex_str);
  if (std::regex_search(str, sub_matches, regex) && sub_matches.size() == 3) {
    return sub_matches[1].str() + sub_matches[2].str();
  } else {
    std::string err = regex_str + " not found in output ";
    VIXL_ABORT_WITH_MSG(err.c_str());
  }
}

#endif  // VIXL_INCLUDE_SIMULATOR_AARCH64

}  // namespace aarch64
}  // namespace vixl
