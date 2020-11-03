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

#ifndef VIXL_AARCH64_ISA_AARCH64_H
#define VIXL_AARCH64_ISA_AARCH64_H

#include <functional>
#include <map>
#include <ostream>

#include "globals-vixl.h"

namespace vixl {
namespace aarch64 {

enum class ISA {
  A64,
  C64,
  Data,  // Used for literal pools, etc.
};

inline ISA ExchangeISA(ISA isa) {
  // Like `bx`.
  switch (isa) {
    case ISA::A64:
      return ISA::C64;
    case ISA::C64:
      return ISA::A64;
    default:
      // It doesn't make sense to exchange from Data mode.
      VIXL_UNIMPLEMENTED();
      return isa;
  }
}

inline const char* GetISAName(ISA isa) {
  switch (isa) {
    case ISA::A64:
      return "A64";
    case ISA::C64:
      return "C64";
    case ISA::Data:
      return "Data";
  }
  VIXL_UNREACHABLE();
  return "Unknown";
}

inline int GetInterworkOffset(ISA isa) {
  switch (isa) {
    case ISA::C64:
      return 1;
    case ISA::A64:
    case ISA::Data:
      return 0;
  }
  VIXL_UNREACHABLE();
  return 0;
}

// Map buffer addresses to ISAs.
// This is used to record ISA changes by the Assembler, for later passing to a
// Decoder for inspection (e.g. disassembly).
class ISAMap {
 public:
  explicit ISAMap(ISA default_isa)
      : default_isa_(default_isa), isas_(), cursor_(isas_.end()) {}

  // Set the ISA for a block starting at the specified offset, and ending at the
  // next `SetISAAt` (or the end of the buffer). The order in which ISA changes
  // are described has no effect on the result; adjacent blocks with the same
  // ISA are never merged.
  void SetISAAt(ptrdiff_t at, ISA isa) {
    auto cursor = isas_.insert(isas_.end(), std::make_pair(at, isa));
    // Make sure that if the position already existed, it takes the new value.
    cursor->second = isa;
    cursor_ = cursor;
  }

  // Get the ISA at the specified address.
  //
  // This is O(log(n)) in general (as per `std::map::upper_bound()`), but only
  // O(1) for repeated and sequential `GetISAAt`/`SetISAAt` accesses. This makes
  // Assembler::GetISA() fast whilst permitting arbitrary access where
  // necessary.
  ISA GetISAAt(ptrdiff_t at) const {
    if (isas_.empty()) return default_isa_;

    if (CursorContains(at)) {
      return cursor_->second;
    }
    // TODO: Before doing a full lookup, try advancing to the next consecutive
    // block.

    // If `at` is before the first block, return the default.
    if (isas_.begin()->first > at) {
      return default_isa_;
    }

    // `upper_bound` finds the first element where `element.first > at`. We want
    // the block just before that. This works even if `upper_bound` returns
    // `isas_.end()`, by returning the last element.
    // The cases where no block exists starting before `at` were already caught
    // above.
    cursor_ = isas_.upper_bound(at);
    --cursor_;
    return cursor_->second;
  }

  // Produce a new ISAMap containing only the blocks in the specified range.
  // `start` is inclusive, and `end` is exlusive. This is useful when processing
  // small parts of an existing buffer.
  ISAMap GetPart(ptrdiff_t start, ptrdiff_t end) const {
    ISAMap map(GetISAAt(start));
    for (auto it = isas_.upper_bound(start); it != isas_.end(); ++it) {
      if (it->first >= end) break;
      map.SetISAAt(it->first - start, it->second);
    }
    return map;
  }

  void Clear() {
    isas_.clear();
    cursor_ = isas_.end();
  }

 private:
  bool CursorContains(ptrdiff_t at) const {
    if (isas_.empty()) return false;
    if (cursor_ == isas_.end()) return false;
    if (cursor_->first > at) return false;
    // `cursor_` starts on or before `at`, so check that the next block starts
    // after it.
    auto next = cursor_;
    ++next;
    return ((next == isas_.end()) || (next->first > at));
  }

  ISA default_isa_;

  // Map the offset at the start of each block to the ISA of the block.
  std::map<ptrdiff_t, ISA> isas_;

  // At iterator to the most recently-accessed block, to avoid a full lookup on
  // consecutive accesses.
  mutable std::map<ptrdiff_t, ISA>::const_iterator cursor_;
};

// A convenience scope for temporarily setting the ISA. This allows Data regions
// to be conveniently embedded in A64 or C64, or for short C64 sequences to be
// embedded into A64, etc.
//
// Expected usage (literal pools):
//
// Label data;
// __ Adr(c0, &data);
// {
//   ISAScope isa(&masm, ISA::Data);
//   ExactAssemblyScope guard(&masm);
//   __ Bind(&data);
//   __ dc32(...);
//   ...
//
//   // At the end of the scope, the original ISA is restored.
// }
//
// TODO: Inline A64/C64 code blocks are awkward because this doesn't generate
// `bx` automatically. Should we do that? Maybe just in the MacroAssembler.
class ISAScope {
 public:
  // Start an ISAScope on any object that implements
  // `void SetISA(ISA isa)` and `ISA GetISA() const`.
  template <typename T>
  explicit ISAScope(T* object_with_isa, ISA isa) {
    ISA old_isa = object_with_isa->GetISA();
    restore_ = [object_with_isa, old_isa]() {
      object_with_isa->SetISA(old_isa);
    };
    object_with_isa->SetISA(isa);
  }

  ~ISAScope() { restore_(); }

 private:
  std::function<void()> restore_;
};

}  // namespace aarch64
}  // namespace vixl

#endif  // VIXL_AARCH64_ISA_AARCH64_H
