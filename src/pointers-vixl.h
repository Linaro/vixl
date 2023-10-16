// Copyright 2023, VIXL authors
// All rights reserved.
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

// Pointer abstractions, for CHERI compatibility.

#ifndef POINTERS_VIXL_H_
#define POINTERS_VIXL_H_

#include <type_traits>

#include "globals-vixl.h"

#if VIXL_HOST_HAS_CAPABILITIES
#include <cheriintrin.h>
#endif

namespace vixl {

// True for values that are capabilities (but not necessarily pointers). For
// example:
//
// On both hybrid and purecap:
// - void * __capability
// - intcap_t / uintcap_t
//
// On purecap only:
// - void *
// - intptr_t / uintptr_t
//
// Use like std::is_pointer and similar functions.
//
// If you need a capability that's also a semantic pointer then you may need to
// test `std::is_pointer<...> && vixl::is_capability<...>`.
template<class T>
struct is_capability : std::false_type {};

#if VIXL_HOST_HAS_CAPABILITIES
template<class T>
struct is_capability<T * __capability> : std::true_type {};
template<class T>
struct is_capability<T * const __capability> : std::true_type {};
template<class T>
struct is_capability<T * volatile __capability> : std::true_type {};
template<class T>
struct is_capability<T * const volatile __capability> : std::true_type {};
template<>
struct is_capability<intcap_t> : std::true_type {};
template<>
struct is_capability<uintcap_t> : std::true_type {};
#endif

}  // namespace vixl

#endif
