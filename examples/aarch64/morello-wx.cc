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

#include <sys/mman.h>

#include <iostream>
#include "cpu-features.h"
#include "examples.h"

using namespace vixl;
using namespace vixl::aarch64;

#define __ masm.

#if defined(__CHERI__) && defined(__aarch64__)
#include <cheriintrin.h>

// This example demonstrates a 'Compiler' component, which generates code into a
// writable buffer, but returns compiled functions as sealed, executable
// capabilities.
//
// Suballocations from the buffer are made in a bump-pointer style, but the
// pointer is only bumped when executable capabilities are generated. This
// enables functions to be tightly packed, implementing W^X permissions on a
// much finer granularity than page permissions allow.
//
// In a real design, the writeable and executable capabilities should be split
// across two separate compartments.

class Compiler {
  // Alternative views of the same physical memory.
  // The virtual addresses may differ, but don't have to.
  void* __capability rw_;
  void* __capability rx_;
  size_t finalised_;

 public:
  explicit Compiler(size_t len) : finalised_(0) {
    // To keep the example simple, we will never extend or unmap this buffer.
    // Note that in hybrid mode, the cast (with `__cheri_tocap`) derives a
    // capability from DDC.
    auto rwx = (__cheri_tocap Instr * __capability)
        mmap(nullptr,
             len,
             PROT_READ | PROT_WRITE | PROT_EXEC,
             MAP_PRIVATE | MAP_ANONYMOUS,
             -1,
             0);
    VIXL_CHECK(rwx != nullptr);
#ifdef __CHERI_PURE_CAPABILITY__
    // `mmap` returns a capability in this case.
    VIXL_UNIMPLEMENTED();
#else
    rwx = reinterpret_cast<Instr * __capability>(cheri_bounds_set(rwx, len));
    rw_ = rwx;

    // On CheriBSD, DDC does not have the "Execute" permission, so derive `rx_`
    // from PCC instead. We assume that the PCC's bounds are sufficient.
    // TODO: Use `cheri_codeptr` once it is fixed.
    ptraddr_t va = cheri_base_get(rwx);
    asm("cvtp %[rx_], %[va]\n\t"
        "scbndse %[rx_], %[rx_], %[len]\n\r"
        : [rx_] "=&C"(rx_)
        : [va] "r"(va), [len] "r"(cheri_length_get(rw_)));
#endif
  }

  // Return a writable capability, guaranteed not to overlap with any finalised
  // region previously returned by `GetExecutable`. The result will span at
  // least `min_len` bytes and will be at least instruction-aligned.
  //
  // Repeated calls without an intermediate call to `GetExecutable` usually
  // return overlapping results, or the same result if the same `min_len` is
  // passed.
  //
  // Aborts if a suitable capability cannot be represented in the remaining part
  // of the buffer.
  byte* __capability GetWritable(size_t min_len) const {
    uint64_t instr_mask = ~((UINT64_C(1) << kInstructionSizeLog2) - 1);
    // These use Morello's `rrlen` and `rrmask` respectively.
    uint64_t mask = cheri_representable_alignment_mask(min_len) & instr_mask;
    uint64_t len = cheri_representable_length(min_len);

    // Round the base up, not down, so we don't return executable memory.
    ptraddr_t base = (cheri_base_get(rw_) + finalised_ + ~mask) & mask;

    return reinterpret_cast<byte * __capability>(
        cheri_bounds_set_exact(cheri_address_set(rw_, base), len));
  }

  // Return a sealed, executable capability to the start of the
  // previously-allocated writable region (`rw`). This capability could be
  // handed to a less-trusted compartment, which would then be able to execute
  // the code, but only from the entry point we select here.
  //
  // The base of `rw` and `min_len` must both be aligned to `kInstructionSize`.
  void* __capability GetExecutable(byte* __capability rw,
                                   size_t min_len,
                                   ISA entry_isa) {
    VIXL_CHECK(cheri_is_aligned(rw, kInstructionSize));
    VIXL_CHECK(IsMultiple<kInstructionSize>(min_len));
    size_t len = cheri_representable_length(min_len);
    finalised_ += len;
    // [base(rw), base(rw) + len] should always be representable, since
    // with valid usage, min_len < length(rw).
    void* __capability fn =
        cheri_bounds_set_exact(cheri_address_set(rx_, cheri_address_get(rw)), len);

    // If we rounded up the length, pad with invalid instructions.
    if (len > min_len) {
      size_t pad = len - min_len;
      MacroAssembler masm((__cheri_fromcap byte*)rw + min_len, pad);
      ExactAssemblyScope guard(&masm, pad);
      while (masm.GetSizeOfCodeGenerated() < pad) {
        __ udf(kUnreachableOpcode);
      }
      masm.FinalizeCode();
      guard.Close();
    }

    switch (entry_isa) {
      case ISA::A64:
        return cheri_sentry_create(fn);
      case ISA::C64: {
        // Set the bottom bit, so that `blr` enters (or remains in) C64.
        auto fn_c64 = reinterpret_cast<uintcap_t>(fn) + 1;
        return cheri_sentry_create(reinterpret_cast<void * __capability>(fn_c64));
      }
      case ISA::Data:
        VIXL_ABORT_WITH_MSG("Data should not be executable.");
    }
  }

  typedef int64_t (*__capability AbsFn)(int64_t);
  AbsFn __capability GenerateAbs(ISA isa) {
    // TODO: Give MacroAssembler the ability to accept a bounded capability,
    // rather than (pointer) base + length.
    size_t size = 128;
    byte* __capability rw = GetWritable(size);
    MacroAssembler masm((__cheri_fromcap byte*)rw, size);
    CPUFeatures cpu_req(CPUFeatures::kMorello);
    masm.SetCPUFeatures(cpu_req);
    masm.SetISA(isa);

    __ Cmp(x0, 0);
    __ Cneg(x0, x0, mi);
    __ Ret(clr);  // Defaults to `lr` in A64, so make `clr` explicit.

    masm.FinalizeCode();
    return reinterpret_cast<AbsFn>(
        GetExecutable(rw, masm.GetBuffer()->GetSizeInBytes(), isa));
  }
};

#ifndef TEST_EXAMPLES
int main(void) {
  Compiler compiler(4096);

  auto abs_a64 = compiler.GenerateAbs(ISA::A64);
  Capinfo("abs_a64", (void* __capability)abs_a64).PrintOneLine();
  std::cout << "abs_a64(42) = " << abs_a64(42) << "\n";
  std::cout << "abs_a64(-42) = " << abs_a64(-42) << "\n";

  auto abs_c64 = compiler.GenerateAbs(ISA::C64);
  Capinfo("abs_c64", (void* __capability)abs_c64).PrintOneLine();
  std::cout << "abs_c64(42) = " << abs_c64(42) << "\n";
  std::cout << "abs_c64(-42) = " << abs_c64(-42) << "\n";
}
#endif

#else  // __CHERI__ && __aarch64__
#ifndef TEST_EXAMPLES
int main(void) { std::cerr << "This example requires Morello.\n"; }
#endif
#endif  // __CHERI__ && __aarch64__
