// Copyright 2015, VIXL authors
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

#ifndef VIXL_AARCH32_LABEL_AARCH32_H_
#define VIXL_AARCH32_LABEL_AARCH32_H_

extern "C" {
#include <stdint.h>
}

#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <list>

#include "utils-vixl.h"

#include "constants-aarch32.h"

namespace vixl {
namespace aarch32 {

class VeneerPoolManager;
class MacroAssembler;

class Location {
 public:
  typedef int32_t Offset;
  static const Offset kMaxOffset = 0x7fffffff;

  class EmitOperator {
    InstructionSet isa_;

   public:
    explicit EmitOperator(InstructionSet isa) : isa_(isa) {
#if defined(VIXL_INCLUDE_TARGET_A32_ONLY)
      USE(isa_);
      VIXL_ASSERT(isa == A32);
#elif defined(VIXL_INCLUDE_TARGET_T32_ONLY)
      USE(isa_);
      VIXL_ASSERT(isa == T32);
#endif
    }
    virtual ~EmitOperator() {}
    virtual uint32_t Encode(uint32_t /*instr*/,
                            Location::Offset /*pc*/,
                            const Location* /*label*/) const {
      return 0;
    }
#if defined(VIXL_INCLUDE_TARGET_A32_ONLY)
    bool IsUsingT32() const { return false; }
#elif defined(VIXL_INCLUDE_TARGET_T32_ONLY)
    bool IsUsingT32() const { return true; }
#else
    bool IsUsingT32() const { return isa_ == T32; }
#endif
  };

  class ForwardReference {
   public:
    ForwardReference(int32_t location,
                     const EmitOperator& op,
                     Offset max_offset)
        : location_(location),
          op_(op),
          is_branch_(false),
          max_forward_(max_offset) {}
    Offset GetMaxForwardDistance() const { return max_forward_; }
    int32_t GetLocation() const { return location_; }
    uint32_t GetStatePCOffset() const {
      return op_.IsUsingT32() ? kT32PcDelta : kA32PcDelta;
    }

    bool IsUsingT32() const { return op_.IsUsingT32(); }
    bool IsBranch() const { return is_branch_; }
    void SetIsBranch() { is_branch_ = true; }
    const EmitOperator& GetEmitOperator() const { return op_; }
    Offset GetCheckpoint() const {
      // The load instructions align down PC before adding the offset.
      // The alignment is only needed for T32 as A32 instructions are always
      // 4 byte aligned.
      int32_t pc = GetLocation() + GetStatePCOffset();
      return GetMaxForwardDistance() +
             ((op_.IsUsingT32() && !IsBranch()) ? AlignDown(pc, 4) : pc);
    }

   private:
    int32_t location_;
    const EmitOperator& op_;
    bool is_branch_;
    Location::Offset max_forward_;
  };

  typedef std::list<ForwardReference> ForwardRefList;

  enum UpdateCheckpointOption { kNoUpdateNecessary, kRecomputeCheckpoint };

  static bool CompareCheckpoints(const ForwardReference& a,
                                 const ForwardReference& b) {
    return a.GetCheckpoint() < b.GetCheckpoint();
  }

  Offset GetNextCheckpoint() {
    if (HasForwardReference()) {
      ForwardRefList::iterator min_checkpoint =
          std::min_element(forward_.begin(),
                           forward_.end(),
                           CompareCheckpoints);
      return (*min_checkpoint).GetCheckpoint();
    }
    return kMaxOffset;
  }

 public:
  Location()
      : location_(kMaxOffset),
        is_bound_(false),
        referenced_(false),
        checkpoint_(kMaxOffset) {}
  explicit Location(Offset location)
      : location_(location),
        is_bound_(true),
        referenced_(false),
        checkpoint_(kMaxOffset) {}
  ~Location() VIXL_THROW_IN_NEGATIVE_TESTING_MODE(std::runtime_error) {
#ifdef VIXL_DEBUG
    if (referenced_ && !is_bound_) {
      VIXL_ABORT_WITH_MSG("Location, label or literal used but not bound.\n");
    }
#endif
  }

#undef DEFAULT_IS_T32

  bool IsBound() const { return is_bound_; }
  bool HasForwardReference() const { return !forward_.empty(); }
  void Bind(Offset offset) {
    VIXL_ASSERT(!IsBound());
    location_ = offset;
    is_bound_ = true;
  }
  Offset GetLocation() const {
    VIXL_ASSERT(IsBound());
    return location_;
  }
  void SetReferenced() { referenced_ = true; }
  bool IsReferenced() const { return referenced_; }
  void SetCheckpoint(Offset checkpoint) { checkpoint_ = checkpoint; }
  Offset GetCheckpoint() const { return checkpoint_; }
  Offset GetAlignedCheckpoint(int byte_align) const {
    return AlignDown(GetCheckpoint(), byte_align);
  }
  void AddForwardRef(int32_t instr_location,
                     const EmitOperator& op,
                     Offset max_offset) {
    VIXL_ASSERT(referenced_);
    forward_.push_back(ForwardReference(instr_location, op, max_offset));
  }

  ForwardRefList::iterator GetFirstForwardRef() { return forward_.begin(); }
  ForwardRefList::iterator GetEndForwardRef() { return forward_.end(); }
  const ForwardReference* GetForwardRefBack() const {
    if (forward_.empty()) return NULL;
    return &forward_.back();
  }
  // Erase an item in the list. We don't have to recompute the checkpoint as
  // the caller does it.
  ForwardRefList::iterator Erase(ForwardRefList::iterator ref) {
    return forward_.erase(ref);
  }
  ForwardReference& GetBackForwardRef() { return forward_.back(); }

  void ClearForwardRef() { forward_.clear(); }

  // Only used by the literal pool.
  // Removes the last forward reference, in particular because of a rewind.
  // TODO(all): This is hard to test as the checkpoint could be affected only
  //   if the literal has multiple forward references. So, the literal has to be
  //   shared between multiple instructions and part of the literal pool which
  //   is not yet supperted.
  void InvalidateLastForwardReference(
      UpdateCheckpointOption update_checkpoint = kRecomputeCheckpoint) {
    if (!IsBound()) {
      VIXL_ASSERT(HasForwardReference());
      forward_.pop_back();
    }
    VIXL_ASSERT((update_checkpoint == kNoUpdateNecessary) &&
                ((checkpoint_ == GetNextCheckpoint()) ||
                 ((checkpoint_ == Location::kMaxOffset) && forward_.empty())));
    if (update_checkpoint == kRecomputeCheckpoint) {
      checkpoint_ = GetNextCheckpoint();
    }
  }

  // Only used by the literal pool.
  // Update the checkpoint as the shorter distance from the last
  // literal in the pool's reference location to the point
  // where the forward reference will fail.
  // The last forward reference is assumed to be the one freshly
  // added regarding this literal.
  void UpdateCheckpoint() {
    if (HasForwardReference()) {
      const ForwardReference& ref = forward_.back();
      checkpoint_ = std::min(checkpoint_, ref.GetCheckpoint());
    }
    VIXL_ASSERT(GetNextCheckpoint() == checkpoint_);
  }

 private:
  // Once bound, location of this label in the code buffer.
  Offset location_;
  // Is the label bound.
  bool is_bound_;
  // True if the label has been used at least once.
  bool referenced_;
  // Contains the references to the unbound label
  ForwardRefList forward_;
  // Max offset in the code buffer. Must be emitted before this checkpoint.
  Offset checkpoint_;
};

class Label : public Location {
 public:
  Label() : Location(), veneer_pool_manager_(NULL), is_near_(false) {}
  explicit Label(Offset location)
      : Location(location), veneer_pool_manager_(NULL), is_near_(false) {}
  static bool CompareLabels(Label* a, Label* b) {
    return a->GetCheckpoint() < b->GetCheckpoint();
  }
  bool IsInVeneerPool() const { return veneer_pool_manager_ != NULL; }
  VeneerPoolManager* GetVeneerPoolManager() const {
    return veneer_pool_manager_;
  }
  void SetVeneerPoolManager(VeneerPoolManager* veneer_pool_manager,
                            bool is_near) {
    veneer_pool_manager_ = veneer_pool_manager;
    is_near_ = is_near;
  }
  void ClearVeneerPoolManager() { veneer_pool_manager_ = NULL; }
  bool IsNear() const { return is_near_; }

 private:
  // Not null if the label is currently in the veneer pool.
  VeneerPoolManager* veneer_pool_manager_;
  // True if the label is in the near_labels_ list.
  bool is_near_;
};

class VeneerPoolManager {
 public:
  explicit VeneerPoolManager(MacroAssembler* masm)
      : masm_(masm),
        near_checkpoint_(Location::kMaxOffset),
        far_checkpoint_(Location::kMaxOffset),
        max_near_checkpoint_(0),
        near_checkpoint_margin_(0),
        last_label_reference_offset_(0),
        monitor_(0) {}
  bool IsEmpty() const {
    return (near_labels_.size() + far_labels_.size()) == 0;
  }
  Location::Offset GetCheckpoint() const {
    // For the far labels, we subtract the veneer size. This way avoids problems
    // when two label have the same checkpoint. In the usual case, we lose some
    // range but, as the minimum range for far labels is 1 mega byte, it's not
    // very important.
    size_t veneer_max_size = GetMaxSize();
    VIXL_ASSERT(IsInt32(veneer_max_size));
    Location::Offset tmp =
        far_checkpoint_ - static_cast<Location::Offset>(veneer_max_size);
    // Make room for a branch over the pools.
    return std::min(near_checkpoint_, tmp) - kMaxInstructionSizeInBytes -
           near_checkpoint_margin_;
  }
  size_t GetMaxSize() const {
    return (near_labels_.size() + far_labels_.size()) *
           kMaxInstructionSizeInBytes;
  }
  void AddLabel(Label* label);
  void RemoveLabel(Label* label);
  void EmitLabel(Label* label, Location::Offset emitted_target);
  void Emit(Location::Offset target);

  void Block() { monitor_++; }
  void Release();
  bool IsBlocked() const { return monitor_ != 0; }

 private:
  MacroAssembler* masm_;
  // Lists of all unbound labels which are used by a branch instruction.
  std::list<Label*> near_labels_;
  std::list<Label*> far_labels_;
  // Offset in the code buffer after which the veneer needs to be emitted.
  // It's the lowest checkpoint value in the associated list.
  // A default value of Location::kMaxOffset means that the checkpoint is
  // invalid (no entry in the list).
  Location::Offset near_checkpoint_;
  Location::Offset far_checkpoint_;
  // Highest checkpoint value for the near list.
  Location::Offset max_near_checkpoint_;
  // Margin we have to take to ensure that 16 bit branch instructions will be
  // able to generate 32 bit veneers.
  uint32_t near_checkpoint_margin_;
  // Offset where the last reference to a label has been added to the pool.
  Location::Offset last_label_reference_offset_;
  // Indicates whether the emission of this pool is blocked.
  int monitor_;
};

}  // namespace aarch32
}  // namespace vixl

#endif  // VIXL_AARCH32_LABEL_AARCH32_H_
