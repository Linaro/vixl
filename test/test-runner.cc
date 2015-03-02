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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "test-runner.h"

// Initialize the list as empty.
vixl::Test* vixl::Test::first_ = NULL;
vixl::Test* vixl::Test::last_ = NULL;

// No debugger to start with.
bool vixl::Test::debug_ = false;

// No tracing to start with.
bool vixl::Test::trace_sim_ = false;
bool vixl::Test::trace_reg_ = false;
bool vixl::Test::trace_write_ = false;

// No colour highlight by default.
bool vixl::Test::coloured_trace_ = false;

// No instruction statistics by default.
bool vixl::Test::instruction_stats_ = false;

// Don't generate simulator test traces by default.
bool vixl::Test::sim_test_trace_ = false;

// Instantiate a Test and append it to the linked list.
vixl::Test::Test(const char* name, TestFunction* callback)
  : name_(name), callback_(callback), next_(NULL) {
  // Append this test to the linked list.
  if (first_ == NULL) {
    VIXL_ASSERT(last_ == NULL);
    first_ = this;
  } else {
    last_->next_ = this;
  }
  last_ = this;
}


// Look for 'search' in the arguments.
static bool IsInArgs(const char* search, int argc, char* argv[]) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(search, argv[i]) == 0) {
      return true;
    }
  }
  return false;
}


static bool IsOption(const char* arg) {
  // Any argument like "--option" is an option.
  return ((arg[0] == '-') && (arg[1] == '-'));
}


static void NormalizeOption(char * arg) {
  // Squash all '_' characters in options. This allows --trace_sim and
  // --trace-sim to be handled in the same way, for example.
  VIXL_ASSERT(IsOption(arg));
  for (char * c = arg; *c != '\0'; c++) {
    if (*c == '_') {
      *c = '-';
    }
  }
}


static void PrintHelpMessage() {
  printf("Usage:  ./test [options] [test names]\n"
      "Run all tests specified on the command line.\n"
      "--help              Print this help message.\n"
      "--list              List all available tests.\n"
      "--run_all           Run all available tests.\n"
      "--debugger          Run in the debugger.\n"
      "--trace_all         Enable all trace options, plus --coloured_trace.\n"
      "--trace_sim         Generate a trace of simulated instructions, as\n"
      "                    well as disassembly from the DISASM tests.\n"
      "--trace_reg         Generate a trace of simulated registers.\n"
      "--trace_write       Generate a trace of memory writes.\n"
      "--coloured_trace    Generate coloured trace.\n"
      "--instruction_stats Log instruction statistics to vixl_stats.csv.\n"
      "--sim_test_trace    Print result traces for SIM_* tests.\n");
}

int main(int argc, char* argv[]) {
  // Parse the arguments. Option flags must appear first, followed by an
  // optional list of tests to run.

  int test_specifiers = 0;
  for (int i = 1; i < argc; i++) {
    if (IsOption(argv[i])) {
      NormalizeOption(argv[i]);
    } else {
      // Anything that isn't an option is a test specifier.
      test_specifiers++;
    }
  }

  // Options controlling test conditions and debug output.

  if (IsInArgs("--trace-all", argc, argv)) {
    vixl::Test::set_trace_reg(true);
    vixl::Test::set_trace_write(true);
    vixl::Test::set_trace_sim(true);
    vixl::Test::set_coloured_trace(true);
  }

  if (IsInArgs("--coloured-trace", argc, argv)) {
    vixl::Test::set_coloured_trace(true);
  }

  if (IsInArgs("--debugger", argc, argv)) {
    vixl::Test::set_debug(true);
  }

  if (IsInArgs("--trace-write", argc, argv)) {
    vixl::Test::set_trace_write(true);
  }

  if (IsInArgs("--trace-reg", argc, argv)) {
    vixl::Test::set_trace_reg(true);
  }

  if (IsInArgs("--trace-sim", argc, argv)) {
    vixl::Test::set_trace_sim(true);
  }

  if (IsInArgs("--instruction-stats", argc, argv)) {
    vixl::Test::set_instruction_stats(true);
  }

  if (IsInArgs("--sim-test-trace", argc, argv)) {
    vixl::Test::set_sim_test_trace(true);
  }

  // Basic (mutually-exclusive) operations.

  if (IsInArgs("--help", argc, argv)) {
    PrintHelpMessage();

  } else if (IsInArgs("--list", argc, argv)) {
    // List all registered tests, then exit.
    for (vixl::Test* c = vixl::Test::first(); c != NULL; c = c->next()) {
      printf("%s\n", c->name());
    }

  } else if (IsInArgs("--run-all", argc, argv)) {
    // Run all registered tests.
    for (vixl::Test* c = vixl::Test::first(); c != NULL; c = c->next()) {
      printf("Running %s\n", c->name());
      c->callback()();
    }

  } else {
    // Run the specified tests.
    if (test_specifiers == 0) {
      printf("No tests specified.\n");
      PrintHelpMessage();
      return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
      if (!IsOption(argv[i])) {
        vixl::Test* c;
        for (c = vixl::Test::first(); c != NULL; c = c->next()) {
          if (strcmp(c->name(), argv[i]) == 0) {
            c->callback()();
            break;
          }
        }
        // Fail if we have not found a matching test to run.
        if (c == NULL) {
          printf("Test '%s' does not exist. Aborting...\n", argv[i]);
          abort();
        }
      }
    }
  }

  return EXIT_SUCCESS;
}

