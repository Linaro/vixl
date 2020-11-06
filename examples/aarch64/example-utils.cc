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

#include <iostream>
#include "cpu-features.h"
#include "examples.h"

using namespace vixl;
using namespace vixl::aarch64;

#define __ masm->

bool CanRunNatively(vixl::CPUFeatures req) {
#if __aarch64__
#ifdef VIXL_CODE_BUFFER_MMAP
  if (vixl::CPUFeatures::InferFromOS().Has(req)) {
    return true;
  }
  std::cerr << "This example cannot run. Required features: { " << req
            << " }\n";
  return false;
#else
  USE(req);
  // `CodeBuffer::SetExecutable()` requires VIXL_CODE_BUFFER_MMAP.
  std::cerr << "Native examples currently require VIXL_CODE_BUFFER_MMAP.\n";
  std::cerr << "Consider rebuilding with `code_buffer_allocator=mmap`.\n";
  // TODO: Most examples generate position-independent code, so we could copy
  // the code out to some other buffer.
  VIXL_UNIMPLEMENTED();
  return false;
#endif
#else
  USE(req);
  std::cerr << "This example cannot run because this is not an AArch64 "
               "platform.\n";
  return false;
#endif
}

// Capinfo (*)(void* __capability cap)
void GenerateNewCapinfo(MacroAssembler* masm) {
  // AAPCS64:
  //  - The input (cap) is in c0.
  //  - The caller allocates space for the result and passes it in x8/c8.
  __ Str(c0, MemOperand(x8));

  __ Cfhi(x10, c0);
  __ Str(x10, MemOperand(x8, offsetof(Capinfo, high64)));
  __ Str(x0, MemOperand(x8, offsetof(Capinfo, low64)));

  __ Gcbase(x10, c0);
  __ Str(x10, MemOperand(x8, offsetof(Capinfo, gcbase)));
  __ Gcflgs(x10, c0);
  __ Str(x10, MemOperand(x8, offsetof(Capinfo, gcflgs)));
  __ Gclen(x10, c0);
  __ Str(x10, MemOperand(x8, offsetof(Capinfo, gclen)));
  __ Gclim(x10, c0);
  __ Str(x10, MemOperand(x8, offsetof(Capinfo, gclim)));
  __ Gcoff(x10, c0);
  __ Str(x10, MemOperand(x8, offsetof(Capinfo, gcoff)));
  __ Gcperm(x10, c0);
  __ Str(x10, MemOperand(x8, offsetof(Capinfo, gcperm)));
  __ Gcseal(x10, c0);
  __ Str(x10, MemOperand(x8, offsetof(Capinfo, gcseal)));
  __ Gctag(x10, c0);
  __ Str(x10, MemOperand(x8, offsetof(Capinfo, gctag)));
  __ Gctype(x10, c0);
  __ Str(x10, MemOperand(x8, offsetof(Capinfo, gctype)));
  __ Gcvalue(x10, c0);
  __ Str(x10, MemOperand(x8, offsetof(Capinfo, gcvalue)));

  __ Ret();
}

void Capinfo::Print() const {
  printf("Capability: 0x%" PRId64 "_%016" PRIx64 "_%016" PRIx64 "\n",
         (gctag & 1),
         high64,
         low64);
#ifdef __CHERI__
  // CHERI printf extension.
  printf("    printf: %#lp\n", reinterpret_cast<const void*>(&cap));
#endif

  printf("    gcbase: %#18" PRIx64 "\n", gcbase);
  printf("    gcflgs: %#18" PRIx64 "\n", gcflgs);
  printf("     gclen: %#18" PRIx64 "\n", gclen);
  printf("     gclim: %#18" PRIx64 "\n", gclim);
  printf("     gcoff: %#18" PRIx64 "\n", gcoff);
  printf("    gcperm: %#18" PRIx64 "\n", gcperm);
  printf("    gcseal: %#18" PRIx64 "\n", gcseal);
  printf("     gctag: %#18" PRIx64 "\n", gctag);
  printf("    gctype: %#18" PRIx64 "\n", gctype);
  printf("   gcvalue: %#18" PRIx64 "\n", gcvalue);
}
