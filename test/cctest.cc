// Copyright 2013, ARM Limited
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
#include "cctest.h"

// Initialize the list as empty.
vixl::Cctest* vixl::Cctest::first_ = NULL;
vixl::Cctest* vixl::Cctest::last_ = NULL;

// No debugger to start with.
bool vixl::Cctest::debug_ = false;

// No tracing to start with.
bool vixl::Cctest::trace_sim_ = false;
bool vixl::Cctest::trace_reg_ = false;

// No colour highlight by default.
bool vixl::Cctest::coloured_trace_ = false;

// No instruction statistics by default.
bool vixl::Cctest::instruction_stats_ = false;

// Don't generate simulator test traces by default.
bool vixl::Cctest::sim_test_trace_ = false;

// Instantiate a Cctest and append it to the linked list.
vixl::Cctest::Cctest(const char* name, CctestFunction* callback)
  : name_(name), callback_(callback), next_(NULL) {
  // Append this cctest to the linked list.
  if (first_ == NULL) {
    VIXL_ASSERT(last_ == NULL);
    first_ = this;
  } else {
    last_->next_ = this;
  }
  last_ = this;
}


// Look for 'search' in the arguments.
bool IsInArgs(const char* search, int argc, char* argv[]) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(search, argv[i]) == 0) {
      return true;
    }
  }
  return false;
}


// Special keywords used as arguments must be registered here.
bool IsSpecialArgument(const char* arg) {
  return (strcmp(arg, "--help") == 0) ||
         (strcmp(arg, "--list") == 0) ||
         (strcmp(arg, "--run_all") == 0) ||
         (strcmp(arg, "--debugger") == 0) ||
         (strcmp(arg, "--trace_sim") == 0) ||
         (strcmp(arg, "--trace_reg") == 0) ||
         (strcmp(arg, "--coloured_trace") == 0) ||
         (strcmp(arg, "--instruction_stats") == 0) ||
         (strcmp(arg, "--sim_test_trace") == 0);
}


void PrintHelpMessage() {
  printf("Usage:  ./cctest [options] [test names]\n"
         "Run all tests specified on the command line.\n"
         "--help              print this help message.\n"
         "--list              list all available tests.\n"
         "--run_all           run all available tests.\n"
         "--debugger          run in the debugger.\n"
         "--trace_sim         generate a trace of simulated instructions.\n"
         "--trace_reg         generate a trace of simulated registers. "
                             "Implies --debugger.\n"
         "--coloured_trace    generate coloured trace.\n"
         "--instruction_stats log instruction statistics to vixl_stats.csv.\n"
         "--sim_test_trace    Print result traces for SIM_* tests.\n");
}

int main(int argc, char* argv[]) {
  // Parse the arguments. Option flags must appear first, followed by an
  // optional list of tests to run.

  if (IsInArgs("--coloured_trace", argc, argv)) {
    vixl::Cctest::set_coloured_trace(true);
  }

  if (IsInArgs("--debugger", argc, argv)) {
    vixl::Cctest::set_debug(true);
  }

  if (IsInArgs("--trace_reg", argc, argv)) {
    vixl::Cctest::set_trace_reg(true);
  }

  if (IsInArgs("--trace_sim", argc, argv)) {
    vixl::Cctest::set_trace_sim(true);
  }

  if (IsInArgs("--instruction_stats", argc, argv)) {
    vixl::Cctest::set_instruction_stats(true);
  }

  if (IsInArgs("--sim_test_trace", argc, argv)) {
    vixl::Cctest::set_sim_test_trace(true);
  }

  if (IsInArgs("--help", argc, argv)) {
    PrintHelpMessage();

  } else if (IsInArgs("--list", argc, argv)) {
    // List all registered cctests.
    for (vixl::Cctest* c = vixl::Cctest::first(); c != NULL; c = c->next()) {
      printf("%s\n", c->name());
    }

  } else if (IsInArgs("--run_all", argc, argv)) {
    // Run all registered cctests.
    for (vixl::Cctest* c = vixl::Cctest::first(); c != NULL; c = c->next()) {
      printf("Running %s\n", c->name());
      c->callback()();
    }

  } else {
    if (argc <= 1)
      PrintHelpMessage();
    // Other arguments must be tests to run.
    int i = 1;
    for (i = 1; i < argc; i++) {
      if (!IsSpecialArgument(argv[i])) {
        vixl::Cctest* c;
        for (c = vixl::Cctest::first(); c != NULL; c = c->next()) {
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

