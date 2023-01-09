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

#if VIXL_HOST_HAS_CAPABILITIES
Capinfo::Capinfo(void* __capability c) : name("Capability") {
  uint64_t tmp;
  asm(  // Store the capability verbatim.
      "str %[c], [%[self], %[cap]]\n\t"
      // Store raw high/low halves.
      "cfhi %[tmp], %[c]\n\t"
      "str %[tmp], [%[self], %[high64]]\n\t"
      // TODO: Can we just read %[c] as an X register?
      "gcvalue %[tmp], %[c]\n\t"
      "str %[tmp], [%[self], %[low64]]\n\t"
      // Store properties.
      "gcbase %[tmp], %[c]\n\t"
      "str %[tmp], [%[self], %[gcbase]]\n\t"
      "gcflgs %[tmp], %[c]\n\t"
      "str %[tmp], [%[self], %[gcflgs]]\n\t"
      "gclen %[tmp], %[c]\n\t"
      "str %[tmp], [%[self], %[gclen]]\n\t"
      "gclim %[tmp], %[c]\n\t"
      "str %[tmp], [%[self], %[gclim]]\n\t"
      "gcoff %[tmp], %[c]\n\t"
      "str %[tmp], [%[self], %[gcoff]]\n\t"
      "gcperm %[tmp], %[c]\n\t"
      "str %[tmp], [%[self], %[gcperm]]\n\t"
      "gcseal %[tmp], %[c]\n\t"
      "str %[tmp], [%[self], %[gcseal]]\n\t"
      "gctag %[tmp], %[c]\n\t"
      "str %[tmp], [%[self], %[gctag]]\n\t"
      "gctype %[tmp], %[c]\n\t"
      "str %[tmp], [%[self], %[gctype]]\n\t"
      "gcvalue %[tmp], %[c]\n\t"
      "str %[tmp], [%[self], %[gcvalue]]\n\t"
      : [tmp] "=&r"(tmp)
#if VIXL_HOST_CHERI_PURECAP
      : [self] "C"(this),
#else
      : [self] "r"(this),
#endif
        [c] "C"(c),
        [cap] "i"(offsetof(Capinfo, cap)),
        [high64] "i"(offsetof(Capinfo, high64)),
        [low64] "i"(offsetof(Capinfo, low64)),
        [gcbase] "i"(offsetof(Capinfo, gcbase)),
        [gcflgs] "i"(offsetof(Capinfo, gcflgs)),
        [gclen] "i"(offsetof(Capinfo, gclen)),
        [gclim] "i"(offsetof(Capinfo, gclim)),
        [gcoff] "i"(offsetof(Capinfo, gcoff)),
        [gcperm] "i"(offsetof(Capinfo, gcperm)),
        [gcseal] "i"(offsetof(Capinfo, gcseal)),
        [gctag] "i"(offsetof(Capinfo, gctag)),
        [gctype] "i"(offsetof(Capinfo, gctype)),
        [gcvalue] "i"(offsetof(Capinfo, gcvalue))
      : "memory");
}
#endif

// Capinfo (*)(void* __capability cap)
void GenerateNewCapinfo(MacroAssembler* masm, const char* name) {
  // AAPCS64(-cap):
  //  - The input ('cap') is in c0.
  //  - The caller allocates space for the result and passes it in x8/c8.

#if VIXL_HOST_CHERI_PURECAP
  CRegister result = c8;

  // Capabilities can't be constructed piecewise; load it from a literal pool.
  __ Ldr(c10, reinterpret_cast<uintptr_t>(name));
  __ Str(c10, MemOperand(result, offsetof(Capinfo, name)));
#else
  Register result = x8;

  __ Mov(x10, reinterpret_cast<uintptr_t>(name));
  __ Str(x10, MemOperand(result, offsetof(Capinfo, name)));
#endif

  __ Str(c0, MemOperand(result, offsetof(Capinfo, cap)));

  __ Cfhi(x10, c0);
  __ Str(x10, MemOperand(result, offsetof(Capinfo, high64)));
  __ Str(x0, MemOperand(result, offsetof(Capinfo, low64)));

  __ Gcbase(x10, c0);
  __ Str(x10, MemOperand(result, offsetof(Capinfo, gcbase)));
  __ Gcflgs(x10, c0);
  __ Str(x10, MemOperand(result, offsetof(Capinfo, gcflgs)));
  __ Gclen(x10, c0);
  __ Str(x10, MemOperand(result, offsetof(Capinfo, gclen)));
  __ Gclim(x10, c0);
  __ Str(x10, MemOperand(result, offsetof(Capinfo, gclim)));
  __ Gcoff(x10, c0);
  __ Str(x10, MemOperand(result, offsetof(Capinfo, gcoff)));
  __ Gcperm(x10, c0);
  __ Str(x10, MemOperand(result, offsetof(Capinfo, gcperm)));
  __ Gcseal(x10, c0);
  __ Str(x10, MemOperand(result, offsetof(Capinfo, gcseal)));
  __ Gctag(x10, c0);
  __ Str(x10, MemOperand(result, offsetof(Capinfo, gctag)));
  __ Gctype(x10, c0);
  __ Str(x10, MemOperand(result, offsetof(Capinfo, gctype)));
  __ Gcvalue(x10, c0);
  __ Str(x10, MemOperand(result, offsetof(Capinfo, gcvalue)));

  __ Ret();
}

void Capinfo::Print() const {
  printf("%s: 0x%" PRId64 "_%016" PRIx64 "_%016" PRIx64 "\n",
         name,
         (gctag & 1),
         high64,
         low64);
#if VIXL_HOST_HAS_CAPABILITIES
  // CHERI printf extension.
  printf("    printf: %#lp\n", reinterpret_cast<const void* __capability>(cap));
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

void Capinfo::PrintOneLine() const {
  std::string perms;
#if VIXL_HOST_HAS_CAPABILITIES
  if (gcperm & CHERI_PERM_LOAD) perms += 'r';
  if (gcperm & CHERI_PERM_STORE) perms += 'w';
  if (gcperm & CHERI_PERM_EXECUTE) perms += 'x';
  if (gcperm & CHERI_PERM_LOAD_CAP) perms += 'R';
  if (gcperm & CHERI_PERM_STORE_CAP) perms += 'W';
#if VIXL_HOST_IS_MORELLO
  if (gcperm & (1 << 1)) perms += 'E'; // EXECUTIVE
#endif
  if (!perms.empty()) perms += ',';
#else
  perms = "unknown,";
#endif

  printf("%s: 0x%" PRIx64 " [%s,0x%" PRIx64 "-0x%" PRIx64 "]%s\n",
         name,
         gcvalue,
         perms.c_str(),
         gcbase,
         gclim,
         gcseal ? " (sealed)" : "");
}
