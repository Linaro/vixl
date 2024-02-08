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

#include "test-debugger-aarch64.h"

namespace vixl {
namespace aarch64 {

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64
// The debugger is only available to be tested in simulator builds.

TEST(breakpoints_invalid) {
  SETUP();

  // Test invalid strings instead of numbers.
  SETUP_CMD("break a", "Error: Use `break <address>` to set a breakpoint");
  SETUP_CMD("break abcdef", "Error: Use `break <address>` to set a breakpoint");
  SETUP_CMD("break A", "Error: Use `break <address>` to set a breakpoint");
  SETUP_CMD("break ABCDEF", "Error: Use `break <address>` to set a breakpoint");
  SETUP_CMD("break 0x", "Error: Use `break <address>` to set a breakpoint");
  SETUP_CMD("break 0xg", "Error: Use `break <address>` to set a breakpoint");

  // Test different amounts of parameters.
  SETUP_CMD("break", "Error: Use `break <address>` to set a breakpoint");
  SETUP_CMD("break 42 52", "Error: Use `break <address>` to set a breakpoint");

  // Test out of range addresses.
  SETUP_CMD("break 0xFFFFFFFFFFFFFFFF1",
            "Error: Use `break <address>` to set a breakpoint");
  SETUP_CMD("break 18446744073709551616",
            "Error: Use `break <address>` to set a breakpoint");

  // Continue to exit the debugger.
  SETUP_CMD("continue", "Continuing...");
  RUN();

  CHECK_OUTPUT();
}

TEST(breakpoints_valid) {
  SETUP();

  // Test bottom boundary of addresses.
  SETUP_CMD("break 0x0", "Breakpoint successfully added at: 0x0");
  SETUP_CMD("break 0", "Breakpoint successfully removed at: 0x0");
  SETUP_CMD("break 0x1", "Breakpoint successfully added at: 0x1");
  SETUP_CMD("break 1", "Breakpoint successfully removed at: 0x1");

  // Test top boundary of addresses.
  SETUP_CMD("break 0xFFFFFFFFFFFFFFFF",
            "Breakpoint successfully added at: 0xffffffffffffffff");
  SETUP_CMD("break 18446744073709551615",
            "Breakpoint successfully removed at: 0xffffffffffffffff");

  // Continue to exit the debugger.
  SETUP_CMD("continue", "Continuing...");
  RUN();

  CHECK_OUTPUT();
}

TEST(breakpoints_hit) {
  SETUP();

  // Test hitting a breakpoint.
  std::string mov_addr = GET_INSTRUCTION_ADDRESS("mov x2, #0x2");
  std::string break_cmd = "break ";
  break_cmd += mov_addr;
  std::string expected_trace = "Breakpoint successfully added at: ";
  expected_trace += mov_addr;
  SETUP_CMD(break_cmd, expected_trace);
  SETUP_CMD("continue",
            "Continuing...\n"
            "Debugger hit breakpoint, breaking...");

  // Continue to exit the debugger.
  SETUP_CMD("continue", "Continuing...");
  RUN();

  CHECK_OUTPUT();
}

TEST(cmd_aliases) {
  SETUP();

  // Test all short form commands, to ensure they correctly run their long form
  // counterparts.
  SETUP_CMD("b", "Error: Use `break <address>` to set a breakpoint");
  SETUP_CMD("s x",
            "Error: use `step \\[number\\]` to step an optional number of"
            " instructions");
  SETUP_CMD("p",
            "Error: use `print <register|all>` to print the contents of a"
            " specific register or all registers.");
  SETUP_CMD("t 1", "Error: use `trace` to toggle tracing of registers.");
  SETUP_CMD("g 1",
            "Error: use `gdb` to enter GDB from the simulator debugger.");

  // Continue to exit the debugger.
  SETUP_CMD("c", "Continuing...");
  RUN();

  CHECK_OUTPUT();
}

TEST(stepping_single) {
  SETUP();

  // Test single stepping through the whole program.
  SETUP_CMD("step", ".*mov x2, #0x2");
  SETUP_CMD("step", ".*sub x3, x1, x2");
  SETUP_CMD("step", ".*ret");
  SETUP_CMD("step",
            ".*Debugger at the end of simulation, leaving simulator...");

  RUN();

  CHECK_OUTPUT();
}

TEST(stepping_single_and_continue) {
  SETUP();

  // Test single stepping and then continuing.
  SETUP_CMD("step", ".*mov x2, #0x2");
  SETUP_CMD("continue", "Continuing...");

  RUN();

  CHECK_OUTPUT();
}

TEST(stepping_multi_1) {
  SETUP();

  // Test multi stepping a single instruction.
  SETUP_CMD("step 1", ".*mov x2, #0x2");

  // Continue to exit the debugger.
  SETUP_CMD("continue", "Continuing...");
  RUN();

  CHECK_OUTPUT();
}

TEST(stepping_multi_2) {
  SETUP();

  // Test multi stepping two instructions.
  SETUP_CMD("step 2",
            ".*mov x2, #0x2\n"
            ".*sub x3, x1, x2");

  // Continue to exit the debugger.
  SETUP_CMD("continue", "Continuing...");
  RUN();

  CHECK_OUTPUT();
}

TEST(stepping_multi_3) {
  SETUP();

  // Test multi stepping three instructions.
  SETUP_CMD("step 3",
            ".*mov x2, #0x2\n"
            ".*sub x3, x1, x2\n"
            ".*ret");

  // Continue to exit the debugger.
  SETUP_CMD("continue", "Continuing...");
  RUN();

  CHECK_OUTPUT();
}

TEST(stepping_multi_4) {
  SETUP();

  // Test stepping through the whole program in one go.
  SETUP_CMD("step 4",
            ".*mov x2, #0x2\n"
            ".*sub x3, x1, x2\n"
            ".*ret\n"
            "Debugger at the end of simulation, leaving simulator...");

  RUN();

  CHECK_OUTPUT();
}

TEST(stepping_multi_5) {
  SETUP();

  // Test multi stepping past the end of the program.
  SETUP_CMD("step 5",
            ".*mov x2, #0x2\n"
            ".*sub x3, x1, x2\n"
            ".*ret\n"
            "Debugger at the end of simulation, leaving simulator...");

  RUN();

  CHECK_OUTPUT();
}

TEST(stepping_invalid) {
  SETUP();

  // Test invalid arguments to step command.
  SETUP_CMD("step 1 2",
            "Error: use `step \\[number\\]` to step an optional number of"
            " instructions");

  // Continue to exit the debugger.
  SETUP_CMD("continue", "Continuing...");
  RUN();

  CHECK_OUTPUT();
}

TEST(print_invalid) {
  SETUP();

  // Test invalid amounts of arguments to the print command.
  SETUP_CMD("print",
            "Error: use `print <register|all>` to print the contents of a"
            " specific register or all registers.");
  SETUP_CMD("print all all",
            "Error: use `print <register|all>` to print the contents of a"
            " specific register or all registers.");

  // Test invalid types of registers.
  SETUP_CMD("print alls",
            "Error: incorrect register format, use e.g: X0, x0, etc...");
  SETUP_CMD("print a",
            "Error: incorrect register format, use e.g: X0, x0, etc...");
  SETUP_CMD("print x",
            "Error: incorrect register format, use e.g: X0, x0, etc...");
  SETUP_CMD("print 0",
            "Error: incorrect register format, use e.g: X0, x0, etc...");

  // Test registers that don't exist on AARCH64.
  SETUP_CMD("print w32",
            "Error: incorrect register format, use e.g: X0, x0, etc...");
  SETUP_CMD("print W32",
            "Error: incorrect register format, use e.g: X0, x0, etc...");
  SETUP_CMD("print x32",
            "Error: incorrect register format, use e.g: X0, x0, etc...");
  SETUP_CMD("print X32",
            "Error: incorrect register format, use e.g: X0, x0, etc...");
  SETUP_CMD("print v32",
            "Error: incorrect register format, use e.g: X0, x0, etc...");
  SETUP_CMD("print V32",
            "Error: incorrect register format, use e.g: X0, x0, etc...");

  // Continue to exit the debugger.
  SETUP_CMD("continue", "Continuing...");
  RUN();

  CHECK_OUTPUT();
}

TEST(trace_invalid) {
  SETUP();

  // Test invalid arguments to trace command.
  SETUP_CMD("trace 1 2", "Error: use `trace` to toggle tracing of registers.");

  // Continue to exit the debugger.
  SETUP_CMD("continue", "Continuing...");
  RUN();

  CHECK_OUTPUT();
}

TEST(trace_toggling) {
  SETUP();

  // Test toggling tracing.
  SETUP_CMD("trace",
            "Enabling disassembly, registers and memory write tracing");
  SETUP_CMD("trace",
            "Disabling disassembly, registers and memory write tracing");
  SETUP_CMD("trace",
            "Enabling disassembly, registers and memory write tracing");
  SETUP_CMD("trace",
            "Disabling disassembly, registers and memory write tracing");

  // Continue to exit the debugger.
  SETUP_CMD("continue", "Continuing...");
  RUN();

  CHECK_OUTPUT();
}

TEST(trace_full) {
  SETUP();

  // Test tracing the whole program.
  SETUP_CMD("trace",
            "Enabling disassembly, registers and memory write tracing");

  std::string expected_trace = "Continuing...\n";
  expected_trace += ".*add x1, x0, #0x5 \\(5\\)\n";
  expected_trace += "(" + x_register_trace + "\\n){32}";
  expected_trace += "(" + v_register_trace + "\\n){32}";
  expected_trace += ".*mov x2, #0x2\n";
  expected_trace += x_register_trace + "\n";
  expected_trace += ".*sub x3, x1, x2\n";
  expected_trace += x_register_trace + "\n";
  expected_trace += ".*ret\n";
  expected_trace += "# Branch to 0x0000000000000000.";
  SETUP_CMD("continue", expected_trace);

  RUN();

  CHECK_OUTPUT();
}

TEST(trace_partial) {
  SETUP();

  // Test tracing a single line.
  SETUP_CMD("trace",
            "Enabling disassembly, registers and memory write tracing");

  std::string expected_trace = ".*add x1, x0, #0x5 \\(5\\)\n";
  expected_trace += "(" + x_register_trace + "\\n){32}";
  expected_trace += "(" + v_register_trace + "\\n){32}";
  expected_trace += ".*mov x2, #0x2\n";
  SETUP_CMD("step", expected_trace);
  SETUP_CMD("trace",
            "Disabling disassembly, registers and memory write tracing");
  SETUP_CMD("continue", "Continuing...\n");

  RUN();

  CHECK_OUTPUT();
}

#endif  // VIXL_INCLUDE_SIMULATOR_AARCH64

}  // namespace aarch64
}  // namespace vixl
