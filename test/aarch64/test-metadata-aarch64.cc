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

#include <sys/mman.h>
#include <unistd.h>

#include "test-runner.h"
#include "test-utils.h"

#include "aarch64/cpu-aarch64.h"
#include "aarch64/disasm-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"
#include "aarch64/simulator-aarch64.h"
#include "aarch64/test-utils-aarch64.h"
#include "test-assembler-aarch64.h"

namespace vixl {
namespace aarch64 {

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64
TEST(test_metadata_mte) {
  SETUP_WITH_FEATURES(CPUFeatures::kMTE);

  size_t data_size = 320;
  void* tagged_address = simulator.Mmap(NULL,
                                        data_size,
                                        PROT_READ | PROT_WRITE | PROT_MTE,
                                        MAP_PRIVATE | MAP_ANONYMOUS,
                                        -1,
                                        0);

  START();

  Register tagged_heap_ptr = x20;
  __ Mov(tagged_heap_ptr, reinterpret_cast<uintptr_t>(tagged_address));
  for (int i = 0; i < 10; i++) {
    __ Ldr(w0, MemOperand(tagged_heap_ptr, i * 32));
    __ Str(w0, MemOperand(tagged_heap_ptr, i * 32));
  }
  __ Ldr(x2, MemOperand(tagged_heap_ptr, 8));
  __ Ldrb(w3, MemOperand(tagged_heap_ptr, 1));
  __ Ldrh(w4, MemOperand(tagged_heap_ptr, 67));

  __ Addg(x21, tagged_heap_ptr, 16, 2);

  END();

  if (CAN_RUN()) {
    RUN();
  }

  simulator.Munmap(tagged_address, data_size, PROT_MTE);
}

#ifdef VIXL_NEGATIVE_TESTING
TEST(test_metadata_mte_neg) {
  CPUFeatures features(CPUFeatures::kMTE);
  SETUP_WITH_FEATURES(features);
  size_t data_size = 320;
  void* tagged_address = simulator.Mmap(NULL,
                                        data_size,
                                        PROT_READ | PROT_WRITE | PROT_MTE,
                                        MAP_PRIVATE | MAP_ANONYMOUS,
                                        -1,
                                        0);

  START();

  Register tagged_heap_ptr = x20;
  __ Mov(tagged_heap_ptr, reinterpret_cast<uintptr_t>(tagged_address));
  __ Addg(x21, tagged_heap_ptr, 16, 2);

  // The memory tag has been changed and becomes invalid.
  __ Ldr(w0, MemOperand(x21));
  __ Str(w0, MemOperand(x21));

  // Out-of-bound access error.
  __ Ldr(w0, MemOperand(tagged_heap_ptr, 320));
  __ Str(w0, MemOperand(tagged_heap_ptr, 336));
  __ Ldr(w0, MemOperand(tagged_heap_ptr, -8));
  __ Str(w0, MemOperand(tagged_heap_ptr, -16));

  void* tagged_address_2 = simulator.Mmap(NULL,
                                          data_size,
                                          PROT_READ | PROT_WRITE | PROT_MTE,
                                          MAP_PRIVATE | MAP_ANONYMOUS,
                                          -1,
                                          0);

  __ Mov(x22, reinterpret_cast<uintptr_t>(tagged_address_2));
  simulator.Munmap(tagged_address_2, data_size, PROT_MTE);

  // Use-after-free error.
  __ Ldr(w0, MemOperand(x22));

  END();

  if (CAN_RUN()) {
    MUST_FAIL_WITH_MESSAGE(RUN(), "Tag mismatch.");
  }

  simulator.Munmap(tagged_address, data_size, PROT_MTE);
}
#endif  // VIXL_NEGATIVE_TESTING
#endif  // VIXL_INCLUDE_SIMULATOR_AARCH64
}  // namespace aarch64
}  // namespace vixl
