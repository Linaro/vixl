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

#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include "vixl/utils.h"

namespace vixl {

// Each actual test is represented by a Test instance.
// Tests are appended to a static linked list upon creation.
class Test {
  typedef void (TestFunction)();

 public:
  Test(const char* name, TestFunction* callback);

  const char* name() { return name_; }
  TestFunction* callback() { return callback_; }
  static Test* first() { return first_; }
  static Test* last() { return last_; }
  Test* next() { return next_; }
  static bool debug() { return debug_; }
  static void set_debug(bool value) { debug_ = value; }
  static bool trace_sim() { return trace_sim_; }
  static void set_trace_sim(bool value) { trace_sim_ = value; }
  static bool trace_reg() { return trace_reg_; }
  static void set_trace_reg(bool value) { trace_reg_ = value; }
  static bool trace_write() { return trace_write_; }
  static void set_trace_write(bool value) { trace_write_ = value; }
  static bool coloured_trace() { return coloured_trace_; }
  static void set_coloured_trace(bool value) { coloured_trace_ = value; }
  static bool instruction_stats() { return instruction_stats_; }
  static void set_instruction_stats(bool value) { instruction_stats_ = value; }
  static bool sim_test_trace() { return sim_test_trace_; }
  static void set_sim_test_trace(bool value) { sim_test_trace_ = value; }

  // The debugger is needed to trace register values.
  static bool run_debugger() { return debug_; }

 private:
  const char* name_;
  TestFunction* callback_;

  static Test* first_;
  static Test* last_;
  Test* next_;
  static bool debug_;
  static bool trace_sim_;
  static bool trace_reg_;
  static bool trace_write_;
  static bool coloured_trace_;
  static bool instruction_stats_;
  static bool sim_test_trace_;
};

// Define helper macros for test files.

// Macro to register a test. It instantiates a Test and registers its
// callback function.
#define TEST_(Name)                                                            \
void Test##Name();                                                             \
Test test_##Name(#Name, &Test##Name);                                          \
void Test##Name()
}  // namespace vixl

#endif  // TEST_TEST_H_
