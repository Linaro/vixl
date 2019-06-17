// Copyright 2019, VIXL authors
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

#ifndef VIXL_AARCH64_BENCH_UTILS_H_
#define VIXL_AARCH64_BENCH_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "globals-vixl.h"

class BenchTimer {
 public:
  BenchTimer() { gettimeofday(&start_, NULL); }

  double GetElapsedSeconds() const {
    timeval elapsed = GetElapsed();
    double sec = elapsed.tv_sec;
    double usec = elapsed.tv_usec;
    return sec + (usec / 1000000.0);
  }

  bool HasRunFor(uint32_t seconds) {
    timeval elapsed = GetElapsed();
    VIXL_ASSERT(elapsed.tv_sec >= 0);
    return static_cast<uint64_t>(elapsed.tv_sec) >= seconds;
  }

 private:
  timeval GetElapsed() const {
    VIXL_ASSERT(timerisset(&start_));
    timeval now, elapsed;
    gettimeofday(&now, NULL);
    timersub(&now, &start_, &elapsed);
    return elapsed;
  }

  timeval start_;
};

// Provide a standard command-line interface for all benchmarks.
class BenchCLI {
 public:
  // Set default values.
  BenchCLI(int argc, char* argv[])
      : run_time_(kDefaultRunTime), status_(kRunBenchmark) {
    for (int i = 1; i < argc; i++) {
      if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
        PrintUsage(argv[0]);
        status_ = kExitSuccess;
        return;
      }
    }

    // Use the default run time.
    if (argc == 1) return;

    if (argc != 2) {
      if (argc > 0) PrintUsage(argv[0]);
      status_ = kExitFailure;
      return;
    }

    char* end;
    unsigned long run_time = strtoul(argv[1], &end, 0);  // NOLINT(runtime/int)
    if ((end == argv[1]) || (run_time > UINT32_MAX)) {
      PrintUsage(argv[0]);
      status_ = kExitFailure;
      return;
    }
    run_time_ = static_cast<uint32_t>(run_time);
  }

  void PrintUsage(char* name) {
    printf("USAGE: %s [OPTIONS]... [RUN_TIME]\n", name);
    printf("\n");
    printf("Run a single VIXL benchmark for approximately RUN_TIME seconds,\n");
    printf("or %" PRIu32 " seconds if unspecified.\n", kDefaultRunTime);
    printf("\n");
    printf("OPTIONS:\n");
    printf("\n");
    printf("    -h, --help\n");
    printf("        Print this help message.\n");
  }

  void PrintResults(uint64_t iterations, double elapsed_seconds) {
    double score = iterations / elapsed_seconds;
    printf("%g iteration%s per second (%" PRIu64 " / %g)\n",
           score,
           (score == 1.0) ? "" : "s",
           iterations,
           elapsed_seconds);
  }

  bool ShouldExitEarly() const {
    switch (status_) {
      case kRunBenchmark:
        return false;
      case kExitFailure:
      case kExitSuccess:
        return true;
    }
    VIXL_UNREACHABLE();
    return true;
  }

  int GetExitCode() const {
    switch (status_) {
      case kExitFailure:
        return EXIT_FAILURE;
      case kExitSuccess:
      case kRunBenchmark:
        return EXIT_SUCCESS;
    }
    VIXL_UNREACHABLE();
    return EXIT_FAILURE;
  }

  uint32_t GetRunTimeInSeconds() const { return run_time_; }

 private:
  static const uint32_t kDefaultRunTime = 5;

  uint32_t run_time_;

  enum { kRunBenchmark, kExitSuccess, kExitFailure } status_;
};

#endif  // VIXL_AARCH64_BENCH_UTILS_H_
