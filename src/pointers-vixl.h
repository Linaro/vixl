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

// Address "tag". This is the top byte in "type byte ignore" (TBI), not the
// capability tag. This is roughly equivalent to Morello's flags.
const unsigned kAddressTagOffset = 56;
const unsigned kAddressTagWidth = 8;
const uint64_t kAddressTagMask = ((UINT64_C(1) << kAddressTagWidth) - 1)
                                 << kAddressTagOffset;
VIXL_STATIC_ASSERT(kAddressTagMask == UINT64_C(0xff00000000000000));

// Extend CHERI's ptraddr_t to all hosts (as vixl::ptraddr_t).
#if VIXL_HOST_HAS_CAPABILITIES
using ::ptraddr_t;
#else
typedef uintptr_t ptraddr_t;
#endif

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

// Map pointer types onto a corresponding uintptr_t-like type. Use like other
// standard `::type` helpers. For example:
//
//     typename vixl::UintReprOf<void *>::type                (= uintptr_t)
//     typename vixl::UintReprOf<void * __capability>::type   (= uintcap_t)
template<typename P, typename Enable = void> struct UintReprOf;

template<typename P>
struct UintReprOf<P,
                  typename std::enable_if<std::is_pointer<P>::value &&
                                          !vixl::is_capability<P>::value>::type> {
  typedef uintptr_t type;
};

#if VIXL_HOST_HAS_CAPABILITIES
template<typename P>
struct UintReprOf<P,
                  typename std::enable_if<std::is_pointer<P>::value &&
                                          vixl::is_capability<P>::value>::type> {
  typedef uintcap_t type;
};

template<> struct UintReprOf<uintcap_t> { typedef uintcap_t type; };
template<> struct UintReprOf<intcap_t> { typedef uintcap_t type; };
#endif  // VIXL_HOST_HAS_CAPABILITIES

#if VIXL_HOST_CHERI_HYBRID || !VIXL_HOST_HAS_CAPABILITIES
template<> struct UintReprOf<uintptr_t> { typedef uintptr_t type; };
template<> struct UintReprOf<intptr_t> { typedef uintptr_t type; };
#endif

// A simple wrapper class around pointer types (or their integer
// representations), providing simple accessors for capability properties, with
// reasonable behaviour for non-capabilities.
//
// A full pointer is required; partial representations (such as `ptraddr_t` on
// purecap hosts) are not supported. However, `ptraddr_t` can be combined with a
// full pointer using `SetAddress()` and similar methods.
template<typename P, typename Uint = typename UintReprOf<P>::type>
class Pointer {
 public:
  // Implicit conversions can make `Pointer` transparent.
  Pointer(P ptr) : ptr_(ptr) {}  // NOLINT(runtime/explicit)
  operator P() const {
    return Unwrap();
  }

  P Unwrap() const {
    return ptr_;
  }

  // Cast to uintptr_t or uintcap_t, as appropriate.
  Uint GetUintRepr() const {
    // Use C-style casts to get static_cast behaviour for integral types (e.g.
    // intptr_t), and reinterpret_cast behaviour for other types.
    return (Uint)(ptr_);
  }

  template<typename U>
  U ReinterpretCast() const {
    return Pointer<U>(reinterpret_cast<U>(Unwrap()));
  }

  ptraddr_t GetAddress() const {
    return GetAddressImpl(GetUintRepr());
  }

  void SetAddress(ptraddr_t address) const {
    ptr_ = (P)(SetAddressImpl(GetUintRepr(), address));
  }

  Pointer<P, Uint> WithAddress(ptraddr_t address) const {
    return Pointer<P, Uint>((P)(SetAddressImpl(GetUintRepr(), address)));
  }

  uint64_t GetFlags() const {
    return GetFlagsImpl(GetUintRepr());
  }

  void SetFlags(uint64_t flags) const {
    ptr_ = (P)(SetFlagsImpl(GetUintRepr(), flags));
  }

  Pointer<P, Uint> WithFlags(uint64_t flags) const {
    return Pointer<P, Uint>((P)(SetFlagsImpl(GetUintRepr(), flags)));
  }

  Pointer<P, Uint> WithoutFlags() const {
    return Pointer<P, Uint>((P)(SetFlagsImpl(GetUintRepr(), 0)));
  }

 private:
#if VIXL_HOST_HAS_CAPABILITIES
  // Overloaded implementations for:
  //   - CHERI (Morello) hybrid,
  //   - CHERI (Morello) purecap.

  static inline ptraddr_t GetAddressImpl(uintcap_t ptr) {
    return cheri_address_get(ptr);
  }

  static inline uintcap_t SetAddressImpl(uintcap_t ptr, ptraddr_t address) {
    return cheri_address_set(ptr, address);
  }

  static inline uint64_t GetFlagsImpl(uintcap_t ptr) {
#if VIXL_HOST_IS_MORELLO
    uint64_t flags;
    asm("gcflgs %[flags], %[ptr]\n\r"
        : [flags] "=r"(flags)
        : [ptr] "C"(ptr)
    );
    return (flags & kAddressTagMask) >> kAddressTagOffset;
#else
#error "Unimplemented: flags accessors for non-Morello CHERI hosts."
#endif // VIXL_HOST_IS_MORELLO
  }

  static inline uintcap_t SetFlagsImpl(uintcap_t ptr, uint64_t flags) {
    VIXL_ASSERT(flags < (1 << kAddressTagWidth));
#ifdef __aarch64__
    uintcap_t result;
    asm("scflgs %[result], %[ptr], %[flags]\n\r"
        : [result] "=C"(result)
        : [ptr] "C"(ptr), [flags] "r"(flags << kAddressTagOffset)
    );
    return result;
#else
#error "Unimplemented: flags accessors for non-Morello CHERI hosts."
#endif
  }
#endif

#if !VIXL_HOST_CHERI_PURECAP
  // Overloaded implementations for:
  //   - non-CHERI hosts (e.g. traditional AArch64),
  //   - CHERI (Morello) hybrid.

  static inline ptraddr_t GetAddressImpl(uintptr_t ptr) {
    return reinterpret_cast<ptraddr_t>(ptr);
  }

  static inline uintptr_t SetAddressImpl(uintptr_t ptr, ptraddr_t address) {
    VIXL_STATIC_ASSERT(sizeof(ptr) == sizeof(address));
    return static_cast<uintptr_t>(address);
  }

  static inline uint64_t GetFlagsImpl(uintptr_t ptr) {
    return (ptr & kAddressTagMask) >> kAddressTagOffset;
  }

  static inline uintptr_t SetFlagsImpl(uintptr_t ptr, uint64_t flags) {
    VIXL_STATIC_ASSERT(sizeof(ptr) == sizeof(flags));
    VIXL_ASSERT(flags < (1 << kAddressTagWidth));
    return (ptr & ~kAddressTagMask) | (flags << kAddressTagOffset);
  }
#endif

  P ptr_;
};

// A convenience for Pointer<P>(...), to avoid having to make P explicit.
template<typename P>
Pointer<P> WrapPointer(P ptr) {
  return Pointer<P>(ptr);
}

}  // namespace vixl

#endif
