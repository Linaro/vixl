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

class Label {
 public:
  typedef int32_t Offset;
  static const Offset kMaxOffset = 0x7fffffff;

  class LabelEmitOperator {
    Label::Offset max_backward_;
    Label::Offset max_forward_;

   public:
    LabelEmitOperator(Label::Offset max_backward, Label::Offset max_forward)
        : max_backward_(max_backward), max_forward_(max_forward) {}
    virtual ~LabelEmitOperator() {}
    virtual uint32_t Encode(uint32_t /*instr*/,
                            Label::Offset /*pc*/,
                            const Label* /*label*/) const {
      return 0;
    }
    Label::Offset GetMaxForwardDistance() const { return max_forward_; }
    Label::Offset GetMaxBackwardDistance() const { return max_backward_; }
  };

  class ForwardReference {
   public:
    ForwardReference(int32_t location,
                     const LabelEmitOperator& op,
                     InstructionSet isa)
        : location_(location), op_(op), isa_(isa), is_branch_(false) {
#if defined(VIXL_INCLUDE_TARGET_A32_ONLY)
      USE(isa_);
      VIXL_ASSERT(isa_ == A32);
#elif defined(VIXL_INCLUDE_TARGET_T32_ONLY)
      USE(isa_);
      VIXL_ASSERT(isa == T32);
#endif
    }
    Offset GetMaxForwardDistance() const { return op_.GetMaxForwardDistance(); }
    int32_t GetLocation() const { return location_; }
    uint32_t GetStatePCOffset() const {
      return IsUsingT32() ? kT32PcDelta : kA32PcDelta;
    }

#if defined(VIXL_INCLUDE_TARGET_A32_ONLY)
    bool IsUsingT32() const { return false; }
#elif defined(VIXL_INCLUDE_TARGET_T32_ONLY)
    bool IsUsingT32() const { return true; }
#else
    bool IsUsingT32() const { return isa_ == T32; }
#endif

    bool IsBranch() const { return is_branch_; }
    void SetIsBranch() { is_branch_ = true; }
    const LabelEmitOperator& GetEmitOperator() const { return op_; }
    Offset GetCheckpoint() const {
      // The load instructions align down PC before adding the offset.
      // The alignment is only needed for T32 as A32 instructions are always
      // 4 byte aligned.
      int32_t pc = GetLocation() + GetStatePCOffset();
      return GetMaxForwardDistance() +
             ((IsUsingT32() && !IsBranch()) ? AlignDown(pc, 4) : pc);
    }

   private:
    int32_t location_;
    const LabelEmitOperator& op_;
    InstructionSet isa_;
    bool is_branch_;
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
  Label()
      : imm_offset_(kMaxOffset),
        pc_offset_(0),
        is_bound_(false),
        minus_zero_(false),
        isa_(kDefaultISA),
        referenced_(false),
        veneer_pool_manager_(NULL),
        is_near_(false),
        checkpoint_(kMaxOffset) {}
  explicit Label(Offset offset, uint32_t pc_offset, bool minus_zero = false)
      : imm_offset_(offset),
        pc_offset_(pc_offset),
        is_bound_(true),
        minus_zero_(minus_zero),
        isa_(kDefaultISA),
        referenced_(false),
        veneer_pool_manager_(NULL),
        is_near_(false),
        checkpoint_(kMaxOffset) {}
  ~Label() VIXL_THROW_IN_NEGATIVE_TESTING_MODE(std::runtime_error) {
#ifdef VIXL_DEBUG
    if (referenced_ && !is_bound_) {
      VIXL_ABORT_WITH_MSG("Label used but not bound.\n");
    }
#endif
  }

#undef DEFAULT_IS_T32

  bool IsBound() const { return is_bound_; }
  bool HasForwardReference() const { return !forward_.empty(); }
  void Bind(Offset offset, InstructionSet isa) {
    VIXL_ASSERT(!IsBound());
    USE(isa);
    USE(isa_);
    imm_offset_ = offset;
    is_bound_ = true;
#if defined(VIXL_INCLUDE_TARGET_A32_ONLY)
    VIXL_ASSERT(isa == A32);
#elif defined(VIXL_INCLUDE_TARGET_T32_ONLY)
    VIXL_ASSERT(isa == T32);
#else
    isa_ = isa;
#endif
  }
  uint32_t GetPcOffset() const { return pc_offset_; }
  Offset GetLocation() const {
    VIXL_ASSERT(IsBound());
    return imm_offset_ + static_cast<Offset>(pc_offset_);
  }
  bool IsUsingT32() const {
    VIXL_ASSERT(IsBound());  // Must be bound to know its ISA.
#if defined(VIXL_INCLUDE_TARGET_A32_ONLY)
    return false;
#elif defined(VIXL_INCLUDE_TARGET_T32_ONLY)
    return true;
#else
    return isa_ == T32;
#endif
  }
  bool IsMinusZero() const {
    VIXL_ASSERT(IsBound());
    return minus_zero_;
  }
  void SetReferenced() { referenced_ = true; }
  bool IsReferenced() const { return referenced_; }
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
  void SetCheckpoint(Offset checkpoint) { checkpoint_ = checkpoint; }
  Offset GetCheckpoint() const { return checkpoint_; }
  Offset GetAlignedCheckpoint(int byte_align) const {
    return AlignDown(GetCheckpoint(), byte_align);
  }
  void AddForwardRef(int32_t instr_location,
                     InstructionSet isa,
                     const LabelEmitOperator& op) {
    VIXL_ASSERT(referenced_);
    forward_.push_back(ForwardReference(instr_location, op, isa));
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
                 ((checkpoint_ == Label::kMaxOffset) && forward_.empty())));
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

  static bool CompareLabels(Label* a, Label* b) {
    return a->GetCheckpoint() < b->GetCheckpoint();
  }

 private:
  // Once bound, location of this label in the code buffer.
  Offset imm_offset_;
  uint32_t pc_offset_;
  // Is the label bound.
  bool is_bound_;
  // Special flag for 'pc - 0'.
  bool minus_zero_;
  // Which ISA is the label in.
  InstructionSet isa_;
  // True if the label has been used at least once.
  bool referenced_;
  // Not null if the label is currently inserted in the veneer pool.
  VeneerPoolManager* veneer_pool_manager_;
  // True if the label is inserted in the near_labels_ list.
  bool is_near_;
  // Contains the references to the unbound label
  ForwardRefList forward_;
  // Max offset in the code buffer. Must be emitted before this checkpoint.
  Offset checkpoint_;
};

class VeneerPoolManager {
 public:
  explicit VeneerPoolManager(MacroAssembler* masm)
      : masm_(masm),
        near_checkpoint_(Label::kMaxOffset),
        far_checkpoint_(Label::kMaxOffset),
        max_near_checkpoint_(0),
        near_checkpoint_margin_(0),
        last_label_reference_offset_(0),
        monitor_(0) {}
  bool IsEmpty() const {
    return (near_labels_.size() + far_labels_.size()) == 0;
  }
  Label::Offset GetCheckpoint() const {
    // For the far labels, we subtract the veneer size. This way avoids problems
    // when two label have the same checkpoint. In the usual case, we lose some
    // range but, as the minimum range for far labels is 1 mega byte, it's not
    // very important.
    size_t veneer_max_size = GetMaxSize();
    VIXL_ASSERT(IsInt32(veneer_max_size));
    Label::Offset tmp =
        far_checkpoint_ - static_cast<Label::Offset>(veneer_max_size);
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
  void EmitLabel(Label* label, Label::Offset emitted_target);
  void Emit(Label::Offset target);

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
  // A default value of Label::kMaxOffset means that the checkpoint is
  // invalid (no entry in the list).
  Label::Offset near_checkpoint_;
  Label::Offset far_checkpoint_;
  // Highest checkpoint value for the near list.
  Label::Offset max_near_checkpoint_;
  // Margin we have to take to ensure that 16 bit branch instructions will be
  // able to generate 32 bit veneers.
  uint32_t near_checkpoint_margin_;
  // Offset where the last reference to a label has been added to the pool.
  Label::Offset last_label_reference_offset_;
  // Indicates whether the emission of this pool is blocked.
  int monitor_;
};

}  // namespace aarch32
}  // namespace vixl

#endif  // VIXL_AARCH32_LABEL_AARCH32_H_
