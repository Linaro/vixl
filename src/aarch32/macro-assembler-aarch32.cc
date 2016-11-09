// Copyright 2015, VIXL authors
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of ARM Limited nor the names of its contributors may
//     be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "aarch32/macro-assembler-aarch32.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define CONTEXT_SCOPE \
  ContextScope context(this, __FILE__ ":" TOSTRING(__LINE__))

namespace vixl {
namespace aarch32 {

void UseScratchRegisterScope::Open(MacroAssembler* masm) {
  VIXL_ASSERT((available_ == NULL) && (available_vfp_ == NULL));
  available_ = masm->GetScratchRegisterList();
  old_available_ = available_->GetList();
  available_vfp_ = masm->GetScratchVRegisterList();
  old_available_vfp_ = available_vfp_->GetList();
}


void UseScratchRegisterScope::Close() {
  if (available_ != NULL) {
    available_->SetList(old_available_);
    available_ = NULL;
  }
  if (available_vfp_ != NULL) {
    available_vfp_->SetList(old_available_vfp_);
    available_vfp_ = NULL;
  }
}


bool UseScratchRegisterScope::IsAvailable(const Register& reg) const {
  VIXL_ASSERT(available_ != NULL);
  VIXL_ASSERT(reg.IsValid());
  return available_->Includes(reg);
}


bool UseScratchRegisterScope::IsAvailable(const VRegister& reg) const {
  VIXL_ASSERT(available_vfp_ != NULL);
  VIXL_ASSERT(reg.IsValid());
  return available_vfp_->IncludesAllOf(reg);
}


Register UseScratchRegisterScope::Acquire() {
  VIXL_ASSERT(available_ != NULL);
  VIXL_CHECK(!available_->IsEmpty());
  Register reg = available_->GetFirstAvailableRegister();
  available_->Remove(reg);
  return reg;
}


VRegister UseScratchRegisterScope::AcquireV(unsigned size_in_bits) {
  switch (size_in_bits) {
    case kSRegSizeInBits:
      return AcquireS();
    case kDRegSizeInBits:
      return AcquireD();
    case kQRegSizeInBits:
      return AcquireQ();
    default:
      VIXL_UNREACHABLE();
      return NoVReg;
  }
}


QRegister UseScratchRegisterScope::AcquireQ() {
  VIXL_ASSERT(available_vfp_ != NULL);
  VIXL_CHECK(!available_vfp_->IsEmpty());
  QRegister reg = available_vfp_->GetFirstAvailableQRegister();
  available_vfp_->Remove(reg);
  return reg;
}


DRegister UseScratchRegisterScope::AcquireD() {
  VIXL_ASSERT(available_vfp_ != NULL);
  VIXL_CHECK(!available_vfp_->IsEmpty());
  DRegister reg = available_vfp_->GetFirstAvailableDRegister();
  available_vfp_->Remove(reg);
  return reg;
}


SRegister UseScratchRegisterScope::AcquireS() {
  VIXL_ASSERT(available_vfp_ != NULL);
  VIXL_CHECK(!available_vfp_->IsEmpty());
  SRegister reg = available_vfp_->GetFirstAvailableSRegister();
  available_vfp_->Remove(reg);
  return reg;
}


void UseScratchRegisterScope::Release(const Register& reg) {
  VIXL_ASSERT(available_ != NULL);
  VIXL_ASSERT(reg.IsValid());
  VIXL_ASSERT(!available_->Includes(reg));
  available_->Combine(reg);
}


void UseScratchRegisterScope::Release(const VRegister& reg) {
  VIXL_ASSERT(available_vfp_ != NULL);
  VIXL_ASSERT(reg.IsValid());
  VIXL_ASSERT(!available_vfp_->IncludesAliasOf(reg));
  available_vfp_->Combine(reg);
}


void UseScratchRegisterScope::Include(const RegisterList& list) {
  VIXL_ASSERT(available_ != NULL);
  RegisterList excluded_registers(sp, lr, pc);
  uint32_t mask = list.GetList() & ~excluded_registers.GetList();
  available_->SetList(available_->GetList() | mask);
}


void UseScratchRegisterScope::Include(const VRegisterList& list) {
  VIXL_ASSERT(available_vfp_ != NULL);
  available_vfp_->SetList(available_vfp_->GetList() | list.GetList());
}


void UseScratchRegisterScope::Exclude(const RegisterList& list) {
  VIXL_ASSERT(available_ != NULL);
  available_->SetList(available_->GetList() & ~list.GetList());
}


void UseScratchRegisterScope::Exclude(const VRegisterList& list) {
  VIXL_ASSERT(available_vfp_ != NULL);
  available_vfp_->SetList(available_->GetList() & ~list.GetList());
}


void UseScratchRegisterScope::ExcludeAll() {
  if (available_ != NULL) {
    available_->SetList(0);
  }
  if (available_vfp_ != NULL) {
    available_vfp_->SetList(0);
  }
}


void VeneerPoolManager::AddLabel(Label* label) {
  if (!label->IsInVeneerPool()) {
    label->SetVeneerPoolManager(this);
    labels_.push_back(label);
  }
  Label::ForwardReference& back = label->GetBackForwardRef();
  back.SetIsBranch();
  label->UpdateCheckpoint();
  Label::Offset tmp = label->GetCheckpoint();
  if (checkpoint_ > tmp) {
    checkpoint_ = tmp;
    masm_->ComputeCheckpoint();
  }
}


void VeneerPoolManager::RemoveLabel(Label* label) {
  label->ClearVeneerPoolManager();
  if (label->GetCheckpoint() == checkpoint_) {
    // We have to compute checkpoint again.
    checkpoint_ = Label::kMaxOffset;
    for (std::list<Label*>::iterator it = labels_.begin();
         it != labels_.end();) {
      if (*it == label) {
        it = labels_.erase(it);
      } else {
        checkpoint_ = std::min(checkpoint_, (*it)->GetCheckpoint());
        ++it;
      }
    }
    masm_->ComputeCheckpoint();
  } else {
    // We only have to remove the label from the list.
    for (std::list<Label*>::iterator it = labels_.begin();; ++it) {
      VIXL_ASSERT(it != labels_.end());
      if (*it == label) {
        labels_.erase(it);
        break;
      }
    }
  }
}


void VeneerPoolManager::Emit(Label::Offset target) {
  checkpoint_ = Label::kMaxOffset;
  // Sort labels (regarding their checkpoint) to avoid that a veneer
  // becomes out of range.
  labels_.sort(Label::CompareLabels);
  // To avoid too many veneers, generate veneers which will be necessary soon.
  static const size_t kVeneerEmissionMargin = 1 * KBytes;
  // To avoid too many veneers, use generated veneers for other not too far
  // uses.
  static const size_t kVeneerEmittedMargin = 2 * KBytes;
  Label::Offset emitted_target = target + kVeneerEmittedMargin;
  target += kVeneerEmissionMargin;
  // Reset the checkpoint. It will be computed again in the loop.
  checkpoint_ = Label::kMaxOffset;
  for (std::list<Label*>::iterator it = labels_.begin(); it != labels_.end();) {
    // The labels are sorted. As soon as a veneer is not needed, we can stop.
    if ((*it)->GetCheckpoint() > target) {
      checkpoint_ = std::min(checkpoint_, (*it)->GetCheckpoint());
      break;
    }
    // Define the veneer.
    Label veneer;
    masm_->Bind(&veneer);
    Label::Offset label_checkpoint = Label::kMaxOffset;
    // Check all uses of this label.
    for (Label::ForwardRefList::iterator ref = (*it)->GetFirstForwardRef();
         ref != (*it)->GetEndForwardRef();) {
      if (ref->IsBranch()) {
        if (ref->GetCheckpoint() <= emitted_target) {
          // Use the veneer.
          masm_->EncodeLabelFor(*ref, &veneer);
          ref = (*it)->Erase(ref);
        } else {
          // Don't use the veneer => update checkpoint.
          label_checkpoint = std::min(label_checkpoint, ref->GetCheckpoint());
          ++ref;
        }
      } else {
        ++ref;
      }
    }
    // Even if we no longer have use of this label, we can keep it in the list
    // as the next "B" would add it back.
    (*it)->SetCheckpoint(label_checkpoint);
    checkpoint_ = std::min(checkpoint_, label_checkpoint);
    // Generate the veneer.
    masm_->B(*it);
    ++it;
  }
#ifdef VIXL_DEBUG
  for (std::list<Label*>::iterator it = labels_.begin(); it != labels_.end();
       ++it) {
    VIXL_ASSERT((*it)->GetCheckpoint() >= checkpoint_);
  }
#endif
  masm_->ComputeCheckpoint();
}


void MacroAssembler::PerformEnsureEmit(Label::Offset target, uint32_t size) {
  EmitOption option = kBranchRequired;
  Label after_pools;
  if (target >= veneer_pool_manager_.GetCheckpoint()) {
#ifdef VIXL_DEBUG
    // Here, we can't use an AssemblerAccurateScope as it would call
    // PerformEnsureEmit in an infinite loop.
    bool save_assembler_state = AllowAssembler();
    SetAllowAssembler(true);
#endif
    b(&after_pools);
#ifdef VIXL_DEBUG
    SetAllowAssembler(false);
#endif
    veneer_pool_manager_.Emit(target);
    option = kNoBranchRequired;
#ifdef VIXL_DEBUG
    SetAllowAssembler(save_assembler_state);
#endif
  }
  // Check if the macro-assembler's internal literal pool should be emitted
  // to avoid any overflow. If we already generated the veneers, we can
  // emit the pool (the branch is already done).
  VIXL_ASSERT(GetCursorOffset() <= literal_pool_manager_.GetCheckpoint());
  if ((target > literal_pool_manager_.GetCheckpoint()) ||
      (option == kNoBranchRequired)) {
    // We will generate the literal pool. Generate all the veneers which
    // would become out of range.
    size_t literal_pool_size = literal_pool_manager_.GetLiteralPoolSize();
    VIXL_ASSERT(IsInt32(literal_pool_size));
    Label::Offset veneers_target =
        target + static_cast<Label::Offset>(literal_pool_size);
    VIXL_ASSERT(veneers_target >= 0);
    if (veneers_target >= veneer_pool_manager_.GetCheckpoint()) {
      veneer_pool_manager_.Emit(veneers_target);
    }
    EmitLiteralPool(option);
  }
  BindHelper(&after_pools);
  if (GetBuffer()->IsManaged()) {
    bool grow_requested;
    GetBuffer()->EnsureSpaceFor(size, &grow_requested);
    if (grow_requested) ComputeCheckpoint();
  }
}


void MacroAssembler::ComputeCheckpoint() {
  checkpoint_ = veneer_pool_manager_.GetCheckpoint();
  if (literal_pool_manager_.GetCheckpoint() != Label::kMaxOffset) {
    size_t veneer_max_size = veneer_pool_manager_.GetMaxSize();
    VIXL_ASSERT(IsInt32(veneer_max_size));
    // We must be able to generate the pool and a branch over the pool.
    Label::Offset tmp = literal_pool_manager_.GetCheckpoint() -
                        static_cast<Label::Offset>(veneer_max_size +
                                                   kMaxInstructionSizeInBytes);
    VIXL_ASSERT(tmp >= 0);
    checkpoint_ = std::min(checkpoint_, tmp);
  }
  size_t buffer_size = GetBuffer()->GetCapacity();
  VIXL_ASSERT(IsInt32(buffer_size));
  Label::Offset buffer_checkpoint = static_cast<Label::Offset>(buffer_size);
  checkpoint_ = std::min(checkpoint_, buffer_checkpoint);
}


void MacroAssembler::Switch(Register reg, JumpTableBase* table) {
  // 32-bit table A32:
  // adr ip, table
  // add ip, r1, lsl 2
  // ldr ip, [ip]
  // jmp: add pc, pc, ip, lsl 2
  // table:
  // .int (case_0 - (jmp + 8)) >> 2
  // .int (case_1 - (jmp + 8)) >> 2
  // .int (case_2 - (jmp + 8)) >> 2

  // 16-bit table T32:
  // adr ip, table
  // jmp: tbh ip, r1
  // table:
  // .short (case_0 - (jmp + 4)) >> 1
  // .short (case_1 - (jmp + 4)) >> 1
  // .short (case_2 - (jmp + 4)) >> 1
  // case_0:
  //   ...
  //   b end_switch
  // case_1:
  //   ...
  //   b end_switch
  // ...
  // end_switch:
  Label jump_table;
  UseScratchRegisterScope temps(this);
  Register scratch = temps.Acquire();
  int table_size = AlignUp(table->GetTableSizeInBytes(), 4);

  // Jumpt to default if reg is not in [0, table->GetLength()[
  Cmp(reg, table->GetLength());
  B(ge, table->GetDefaultLabel());

  Adr(scratch, &jump_table);
  if (IsUsingA32()) {
    Add(scratch, scratch, Operand(reg, LSL, table->GetOffsetShift()));
    switch (table->GetOffsetShift()) {
      case 0:
        Ldrb(scratch, MemOperand(scratch));
        break;
      case 1:
        Ldrh(scratch, MemOperand(scratch));
        break;
      case 2:
        Ldr(scratch, MemOperand(scratch));
        break;
      default:
        VIXL_ABORT_WITH_MSG("Unsupported jump table size.\n");
    }
    // Emit whatever needs to be emitted if we want to
    // correctly rescord the position of the branch instruction
    uint32_t branch_location = GetCursorOffset();
    table->SetBranchLocation(branch_location + GetArchitectureStatePCOffset());
    AssemblerAccurateScope scope(this,
                                 table_size + kA32InstructionSizeInBytes,
                                 CodeBufferCheckScope::kMaximumSize);
    add(pc, pc, Operand(scratch, LSL, 2));
    VIXL_ASSERT((GetCursorOffset() - branch_location) == 4);
    bind(&jump_table);
    GenerateSwitchTable(table, table_size);
  } else {
    // Thumb mode - We have tbb and tbh to do this for 8 or 16bit offsets.
    //  But for 32bit offsets, we use the same coding as for A32
    if (table->GetOffsetShift() == 2) {
      // 32bit offsets
      Add(scratch, scratch, Operand(reg, LSL, 2));
      Ldr(scratch, MemOperand(scratch));
      // Cannot use add pc, pc, r lsl 1 as this is unpredictable in T32,
      // so let's do the shift before
      Lsl(scratch, scratch, 1);
      // Emit whatever needs to be emitted if we want to
      // correctly rescord the position of the branch instruction
      uint32_t branch_location = GetCursorOffset();
      table->SetBranchLocation(branch_location +
                               GetArchitectureStatePCOffset());
      AssemblerAccurateScope scope(this,
                                   table_size + kMaxInstructionSizeInBytes,
                                   CodeBufferCheckScope::kMaximumSize);
      add(pc, pc, scratch);
      // add pc, pc, rm fits in 16bit T2 (except for rm = sp)
      VIXL_ASSERT((GetCursorOffset() - branch_location) == 2);
      bind(&jump_table);
      GenerateSwitchTable(table, table_size);
    } else {
      VIXL_ASSERT((table->GetOffsetShift() == 0) ||
                  (table->GetOffsetShift() == 1));
      // Emit whatever needs to be emitted if we want to
      // correctly rescord the position of the branch instruction
      uint32_t branch_location = GetCursorOffset();
      table->SetBranchLocation(branch_location +
                               GetArchitectureStatePCOffset());
      AssemblerAccurateScope scope(this,
                                   table_size + kMaxInstructionSizeInBytes,
                                   CodeBufferCheckScope::kMaximumSize);
      if (table->GetOffsetShift() == 0) {
        // 8bit offsets
        tbb(scratch, reg);
      } else {
        // 16bit offsets
        tbh(scratch, reg);
      }
      // tbb/tbh is a 32bit instruction
      VIXL_ASSERT((GetCursorOffset() - branch_location) == 4);
      bind(&jump_table);
      GenerateSwitchTable(table, table_size);
    }
  }
}


void MacroAssembler::GenerateSwitchTable(JumpTableBase* table, int table_size) {
  table->BindTable(GetCursorOffset());
  for (int i = 0; i < table_size / 4; i++) {
    GetBuffer()->Emit32(0);
  }
}


// switch/case/default : case
// case_index is assumed to be < table->GetLength()
// which is checked in JumpTable::Link and Table::SetPresenceBit
void MacroAssembler::Case(JumpTableBase* table, int case_index) {
  table->Link(this, case_index, GetCursorOffset());
  table->SetPresenceBitForCase(case_index);
}

// switch/case/default : default
void MacroAssembler::Default(JumpTableBase* table) {
  Bind(table->GetDefaultLabel());
}

// switch/case/default : break
void MacroAssembler::Break(JumpTableBase* table) { B(table->GetEndLabel()); }

// switch/case/default : finalize
// Manage the default path, mosstly. All empty offsets in the jumptable
// will point to default.
// All values not in [0, table->GetLength()[ are already pointing here anyway.
void MacroAssembler::EndSwitch(JumpTableBase* table) { table->Finalize(this); }

void MacroAssembler::HandleOutOfBoundsImmediate(Condition cond,
                                                Register tmp,
                                                uint32_t imm) {
  if (IsUintN(16, imm)) {
    CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
    mov(cond, tmp, imm & 0xffff);
    return;
  }
  if (IsUsingT32()) {
    if (ImmediateT32::IsImmediateT32(~imm)) {
      CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
      mvn(cond, tmp, ~imm);
      return;
    }
  } else {
    if (ImmediateA32::IsImmediateA32(~imm)) {
      CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
      mvn(cond, tmp, ~imm);
      return;
    }
  }
  CodeBufferCheckScope scope(this, 2 * kMaxInstructionSizeInBytes);
  mov(cond, tmp, imm & 0xffff);
  movt(cond, tmp, imm >> 16);
}


void MacroAssembler::PadToMinimumBranchRange(Label* label) {
  const Label::ForwardReference* last_reference = label->GetForwardRefBack();
  if ((last_reference != NULL) && last_reference->IsUsingT32()) {
    uint32_t location = last_reference->GetLocation();
    if (location + k16BitT32InstructionSizeInBytes ==
        static_cast<uint32_t>(GetCursorOffset())) {
      uint16_t* instr_ptr = buffer_.GetOffsetAddress<uint16_t*>(location);
      if ((instr_ptr[0] & kCbzCbnzMask) == kCbzCbnzValue) {
        VIXL_ASSERT(!InITBlock());
        // A Cbz or a Cbnz can't jump immediately after the instruction. If the
        // target is immediately after the Cbz or Cbnz, we insert a nop to
        // avoid that.
        EmitT32_16(k16BitT32NopOpcode);
      }
    }
  }
}


HARDFLOAT void PrintfTrampolineRRRR(
    const char* format, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineRRRD(
    const char* format, uint32_t a, uint32_t b, uint32_t c, double d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineRRDR(
    const char* format, uint32_t a, uint32_t b, double c, uint32_t d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineRRDD(
    const char* format, uint32_t a, uint32_t b, double c, double d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineRDRR(
    const char* format, uint32_t a, double b, uint32_t c, uint32_t d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineRDRD(
    const char* format, uint32_t a, double b, uint32_t c, double d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineRDDR(
    const char* format, uint32_t a, double b, double c, uint32_t d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineRDDD(
    const char* format, uint32_t a, double b, double c, double d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineDRRR(
    const char* format, double a, uint32_t b, uint32_t c, uint32_t d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineDRRD(
    const char* format, double a, uint32_t b, uint32_t c, double d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineDRDR(
    const char* format, double a, uint32_t b, double c, uint32_t d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineDRDD(
    const char* format, double a, uint32_t b, double c, double d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineDDRR(
    const char* format, double a, double b, uint32_t c, uint32_t d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineDDRD(
    const char* format, double a, double b, uint32_t c, double d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineDDDR(
    const char* format, double a, double b, double c, uint32_t d) {
  printf(format, a, b, c, d);
}


HARDFLOAT void PrintfTrampolineDDDD(
    const char* format, double a, double b, double c, double d) {
  printf(format, a, b, c, d);
}


void MacroAssembler::Printf(const char* format,
                            CPURegister reg1,
                            CPURegister reg2,
                            CPURegister reg3,
                            CPURegister reg4) {
  if (generate_simulator_code_) {
    PushRegister(reg4);
    PushRegister(reg3);
    PushRegister(reg2);
    PushRegister(reg1);
    Push(RegisterList(r0, r1));
    StringLiteral* format_literal =
        new StringLiteral(format, RawLiteral::kDeletedOnPlacementByPool);
    Adr(r0, format_literal);
    uint32_t args = (reg4.GetType() << 12) | (reg3.GetType() << 8) |
                    (reg2.GetType() << 4) | reg1.GetType();
    Mov(r1, args);
    Hvc(kPrintfCode);
    Pop(RegisterList(r0, r1));
    int size = reg4.GetRegSizeInBytes() + reg3.GetRegSizeInBytes() +
               reg2.GetRegSizeInBytes() + reg1.GetRegSizeInBytes();
    Drop(size);
  } else {
    // Generate on a native platform => 32 bit environment.
    // Preserve core registers r0-r3, r12, r14
    const uint32_t saved_registers_mask =
        kCallerSavedRegistersMask | (1 << r5.GetCode());
    Push(RegisterList(saved_registers_mask));
    // Push VFP registers.
    Vpush(Untyped64, DRegisterList(d0, 8));
    if (Has32DRegs()) Vpush(Untyped64, DRegisterList(d16, 16));
    // Search one register which has been saved and which doesn't need to be
    // printed.
    RegisterList available_registers(kCallerSavedRegistersMask);
    if (reg1.GetType() == CPURegister::kRRegister) {
      available_registers.Remove(Register(reg1.GetCode()));
    }
    if (reg2.GetType() == CPURegister::kRRegister) {
      available_registers.Remove(Register(reg2.GetCode()));
    }
    if (reg3.GetType() == CPURegister::kRRegister) {
      available_registers.Remove(Register(reg3.GetCode()));
    }
    if (reg4.GetType() == CPURegister::kRRegister) {
      available_registers.Remove(Register(reg4.GetCode()));
    }
    Register tmp = available_registers.GetFirstAvailableRegister();
    VIXL_ASSERT(tmp.GetType() == CPURegister::kRRegister);
    // Push the flags.
    Mrs(tmp, APSR);
    Push(tmp);
    Vmrs(RegisterOrAPSR_nzcv(tmp.GetCode()), FPSCR);
    Push(tmp);
    // Push the registers to print on the stack.
    PushRegister(reg4);
    PushRegister(reg3);
    PushRegister(reg2);
    PushRegister(reg1);
    int core_count = 1;
    int vfp_count = 0;
    uint32_t printf_type = 0;
    // Pop the registers to print and store them into r1-r3 and/or d0-d3.
    // Reg4 may stay into the stack if all the register to print are core
    // registers.
    PreparePrintfArgument(reg1, &core_count, &vfp_count, &printf_type);
    PreparePrintfArgument(reg2, &core_count, &vfp_count, &printf_type);
    PreparePrintfArgument(reg3, &core_count, &vfp_count, &printf_type);
    PreparePrintfArgument(reg4, &core_count, &vfp_count, &printf_type);
    // Ensure that the stack is aligned on 8 bytes.
    And(r5, sp, 0x7);
    if (core_count == 5) {
      // One 32 bit argument (reg4) has been left on the stack =>  align the
      // stack
      // before the argument.
      Pop(r0);
      Sub(sp, sp, r5);
      Push(r0);
    } else {
      Sub(sp, sp, r5);
    }
    // Select the right trampoline depending on the arguments.
    uintptr_t address;
    switch (printf_type) {
      case 0:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineRRRR);
        break;
      case 1:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineDRRR);
        break;
      case 2:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineRDRR);
        break;
      case 3:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineDDRR);
        break;
      case 4:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineRRDR);
        break;
      case 5:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineDRDR);
        break;
      case 6:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineRDDR);
        break;
      case 7:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineDDDR);
        break;
      case 8:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineRRRD);
        break;
      case 9:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineDRRD);
        break;
      case 10:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineRDRD);
        break;
      case 11:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineDDRD);
        break;
      case 12:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineRRDD);
        break;
      case 13:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineDRDD);
        break;
      case 14:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineRDDD);
        break;
      case 15:
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineDDDD);
        break;
      default:
        VIXL_UNREACHABLE();
        address = reinterpret_cast<uintptr_t>(PrintfTrampolineRRRR);
        break;
    }
    StringLiteral* format_literal =
        new StringLiteral(format, RawLiteral::kDeletedOnPlacementByPool);
    Adr(r0, format_literal);
    Mov(ip, Operand::From(address));
    Blx(ip);
    // If register reg4 was left on the stack => skip it.
    if (core_count == 5) Drop(kRegSizeInBytes);
    // Restore the stack as it was before alignment.
    Add(sp, sp, r5);
    // Restore the flags.
    Pop(tmp);
    Vmsr(FPSCR, tmp);
    Pop(tmp);
    Msr(APSR_nzcvqg, tmp);
    // Restore the regsisters.
    if (Has32DRegs()) Vpop(Untyped64, DRegisterList(d16, 16));
    Vpop(Untyped64, DRegisterList(d0, 8));
    Pop(RegisterList(saved_registers_mask));
  }
}


void MacroAssembler::PushRegister(CPURegister reg) {
  switch (reg.GetType()) {
    case CPURegister::kNoRegister:
      break;
    case CPURegister::kRRegister:
      Push(Register(reg.GetCode()));
      break;
    case CPURegister::kSRegister:
      Vpush(Untyped32, SRegisterList(SRegister(reg.GetCode())));
      break;
    case CPURegister::kDRegister:
      Vpush(Untyped64, DRegisterList(DRegister(reg.GetCode())));
      break;
    case CPURegister::kQRegister:
      VIXL_UNIMPLEMENTED();
      break;
  }
}


void MacroAssembler::PreparePrintfArgument(CPURegister reg,
                                           int* core_count,
                                           int* vfp_count,
                                           uint32_t* printf_type) {
  switch (reg.GetType()) {
    case CPURegister::kNoRegister:
      break;
    case CPURegister::kRRegister:
      VIXL_ASSERT(*core_count <= 4);
      if (*core_count < 4) Pop(Register(*core_count));
      *core_count += 1;
      break;
    case CPURegister::kSRegister:
      VIXL_ASSERT(*vfp_count < 4);
      *printf_type |= 1 << (*core_count + *vfp_count - 1);
      Vpop(Untyped32, SRegisterList(SRegister(*vfp_count * 2)));
      Vcvt(F64, F32, DRegister(*vfp_count), SRegister(*vfp_count * 2));
      *vfp_count += 1;
      break;
    case CPURegister::kDRegister:
      VIXL_ASSERT(*vfp_count < 4);
      *printf_type |= 1 << (*core_count + *vfp_count - 1);
      Vpop(Untyped64, DRegisterList(DRegister(*vfp_count)));
      *vfp_count += 1;
      break;
    case CPURegister::kQRegister:
      VIXL_UNIMPLEMENTED();
      break;
  }
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondROp instruction,
                              Condition cond,
                              Register rn,
                              const Operand& operand) {
  // movt, sxtb16, teq, uxtb16
  VIXL_ASSERT((type == kMovt) || (type == kSxtb16) || (type == kTeq) ||
              (type == kUxtb16));

  if (type == kMovt) {
    VIXL_ABORT_WITH_MSG("`Movt` expects a 16-bit immediate.");
  }

  // This delegate only supports teq with immediates.
  CONTEXT_SCOPE;
  if ((type == kTeq) && operand.IsImmediate()) {
    UseScratchRegisterScope temps(this);
    Register scratch = temps.Acquire();
    HandleOutOfBoundsImmediate(cond, scratch, operand.GetImmediate());
    CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
    teq(cond, rn, scratch);
    return;
  }
  Assembler::Delegate(type, instruction, cond, rn, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondSizeROp instruction,
                              Condition cond,
                              EncodingSize size,
                              Register rn,
                              const Operand& operand) {
  // cmn cmp mov movs mvn mvns sxtb sxth tst uxtb uxth
  CONTEXT_SCOPE;
  VIXL_ASSERT(size.IsBest());
  VIXL_ASSERT((type == kCmn) || (type == kCmp) || (type == kMov) ||
              (type == kMovs) || (type == kMvn) || (type == kMvns) ||
              (type == kSxtb) || (type == kSxth) || (type == kTst) ||
              (type == kUxtb) || (type == kUxth));
  if (IsUsingT32() && operand.IsRegisterShiftedRegister()) {
    VIXL_ASSERT((type != kMov) || (type != kMovs));
    InstructionCondRROp shiftop = NULL;
    switch (operand.GetShift().GetType()) {
      case LSL:
        shiftop = &Assembler::lsl;
        break;
      case LSR:
        shiftop = &Assembler::lsr;
        break;
      case ASR:
        shiftop = &Assembler::asr;
        break;
      case RRX:
        // A RegisterShiftedRegister operand cannot have a shift of type RRX.
        VIXL_UNREACHABLE();
        break;
      case ROR:
        shiftop = &Assembler::ror;
        break;
      default:
        VIXL_UNREACHABLE();
    }
    if (shiftop != NULL) {
      UseScratchRegisterScope temps(this);
      Register scratch = temps.Acquire();
      CodeBufferCheckScope scope(this, 2 * kMaxInstructionSizeInBytes);
      (this->*shiftop)(cond,
                       scratch,
                       operand.GetBaseRegister(),
                       operand.GetShiftRegister());
      return (this->*instruction)(cond, size, rn, scratch);
    }
  }
  if (operand.IsImmediate()) {
    uint32_t imm = operand.GetImmediate();
    switch (type) {
      case kMov:
      case kMovs:
        if (!rn.IsPC()) {
          // Immediate is too large, but not using PC, so handle with mov{t}.
          HandleOutOfBoundsImmediate(cond, rn, imm);
          if (type == kMovs) {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            tst(cond, rn, rn);
          }
          return;
        } else if (type == kMov) {
          VIXL_ASSERT(IsUsingA32() || cond.Is(al));
          // Immediate is too large and using PC, so handle using a temporary
          // register.
          UseScratchRegisterScope temps(this);
          Register scratch = temps.Acquire();
          HandleOutOfBoundsImmediate(al, scratch, imm);
          EnsureEmitFor(kMaxInstructionSizeInBytes);
          return bx(cond, scratch);
        }
        break;
      case kCmn:
      case kCmp:
        if (IsUsingA32() || !rn.IsPC()) {
          UseScratchRegisterScope temps(this);
          Register scratch = temps.Acquire();
          HandleOutOfBoundsImmediate(cond, scratch, imm);
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          return (this->*instruction)(cond, size, rn, scratch);
        }
        break;
      case kMvn:
      case kMvns:
        if (!rn.IsPC()) {
          UseScratchRegisterScope temps(this);
          Register scratch = temps.Acquire();
          HandleOutOfBoundsImmediate(cond, scratch, imm);
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          return (this->*instruction)(cond, size, rn, scratch);
        }
        break;
      case kTst:
        if (IsUsingA32() || !rn.IsPC()) {
          UseScratchRegisterScope temps(this);
          Register scratch = temps.Acquire();
          HandleOutOfBoundsImmediate(cond, scratch, imm);
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          return (this->*instruction)(cond, size, rn, scratch);
        }
        break;
      default:  // kSxtb, Sxth, Uxtb, Uxth
        break;
    }
  }
  Assembler::Delegate(type, instruction, cond, size, rn, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondRROp instruction,
                              Condition cond,
                              Register rd,
                              Register rn,
                              const Operand& operand) {
  // orn orns pkhbt pkhtb rsc rscs sxtab sxtab16 sxtah uxtab uxtab16 uxtah

  if ((type == kSxtab) || (type == kSxtab16) || (type == kSxtah) ||
      (type == kUxtab) || (type == kUxtab16) || (type == kUxtah) ||
      (type == kPkhbt) || (type == kPkhtb)) {
    // TODO: It would be good to be able to convert "type" to a string, so we
    // can tell the user which instruction is not valid.
    VIXL_ABORT_WITH_MSG(
        "The MacroAssembler does not convert "
        "Sxtab, Sxtab16, Sxtah, Uxtab, Uxtab16, Uxtah, Pkhbt and Pkhtb "
        "instructions.");
  }

  // This delegate only handles the following instructions.
  VIXL_ASSERT((type == kOrn) || (type == kOrns) || (type == kRsc) ||
              (type == kRscs));
  CONTEXT_SCOPE;
  if (IsUsingT32() && operand.IsRegisterShiftedRegister()) {
    InstructionCondRROp shiftop = NULL;
    switch (operand.GetShift().GetType()) {
      case LSL:
        shiftop = &Assembler::lsl;
        break;
      case LSR:
        shiftop = &Assembler::lsr;
        break;
      case ASR:
        shiftop = &Assembler::asr;
        break;
      case RRX:
        // A RegisterShiftedRegister operand cannot have a shift of type RRX.
        VIXL_UNREACHABLE();
        break;
      case ROR:
        shiftop = &Assembler::ror;
        break;
      default:
        VIXL_UNREACHABLE();
    }
    if (shiftop != NULL) {
      UseScratchRegisterScope temps(this);
      Register rm = operand.GetBaseRegister();
      Register rs = operand.GetShiftRegister();
      // If different from `rn`, we can make use of either `rd`, `rm` or `rs` as
      // a scratch register.
      if (!rd.Is(rn)) temps.Include(rd);
      if (!rm.Is(rn)) temps.Include(rm);
      if (!rs.Is(rn)) temps.Include(rs);
      Register scratch = temps.Acquire();
      // TODO: The scope length was measured empirically. We should analyse the
      // worst-case size and add targetted tests.
      CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
      (this->*shiftop)(cond, scratch, rm, rs);
      return (this->*instruction)(cond, rd, rn, scratch);
    }
  }
  if (IsUsingT32() && ((type == kRsc) || (type == kRscs))) {
    // The RegisterShiftRegister case should have been handled above.
    VIXL_ASSERT(!operand.IsRegisterShiftedRegister());
    UseScratchRegisterScope temps(this);
    Register negated_rn;
    if (operand.IsImmediate() || !operand.GetBaseRegister().Is(rn)) {
      // In this case, we can just negate `rn` instead of using a temporary
      // register.
      negated_rn = rn;
    } else {
      if (!rd.Is(rn)) temps.Include(rd);
      negated_rn = temps.Acquire();
    }
    {
      CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
      mvn(cond, negated_rn, rn);
    }
    if (type == kRsc) {
      CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
      return adc(cond, rd, negated_rn, operand);
    }
    // TODO: We shouldn't have to specify how much space the next instruction
    // needs.
    CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
    return adcs(cond, rd, negated_rn, operand);
  }
  if (IsUsingA32() && ((type == kOrn) || (type == kOrns))) {
    // TODO: orn r0, r1, imm -> orr r0, r1, neg(imm) if doable
    //  mvn r0, r2
    //  orr r0, r1, r0
    Register scratch;
    UseScratchRegisterScope temps(this);
    // If different from `rn`, we can make use of source and destination
    // registers as a scratch register.
    if (!rd.Is(rn)) temps.Include(rd);
    if (!operand.IsImmediate() && !operand.GetBaseRegister().Is(rn)) {
      temps.Include(operand.GetBaseRegister());
    }
    if (operand.IsRegisterShiftedRegister() &&
        !operand.GetShiftRegister().Is(rn)) {
      temps.Include(operand.GetShiftRegister());
    }
    scratch = temps.Acquire();
    {
      // TODO: We shouldn't have to specify how much space the next instruction
      // needs.
      CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
      mvn(cond, scratch, operand);
    }
    if (type == kOrns) {
      CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
      return orrs(cond, rd, rn, scratch);
    }
    CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
    return orr(cond, rd, rn, scratch);
  }
  if (operand.IsImmediate()) {
    int32_t imm = operand.GetSignedImmediate();
    if (ImmediateT32::IsImmediateT32(~imm)) {
      CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
      if (IsUsingT32()) {
        switch (type) {
          case kOrn:
            return orr(cond, rd, rn, ~imm);
          case kOrns:
            return orrs(cond, rd, rn, ~imm);
          default:
            break;
        }
      }
    }
    UseScratchRegisterScope temps(this);
    // Allow using the destination as a scratch register if possible.
    if (!rd.Is(rn)) temps.Include(rd);
    Register scratch = temps.Acquire();
    HandleOutOfBoundsImmediate(cond, scratch, imm);
    CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
    return (this->*instruction)(cond, rd, rn, scratch);
  }
  Assembler::Delegate(type, instruction, cond, rd, rn, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondSizeRROp instruction,
                              Condition cond,
                              EncodingSize size,
                              Register rd,
                              Register rn,
                              const Operand& operand) {
  // adc adcs add adds and_ ands asr asrs bic bics eor eors lsl lsls lsr lsrs
  // orr orrs ror rors rsb rsbs sbc sbcs sub subs

  VIXL_ASSERT(
      (type == kAdc) || (type == kAdcs) || (type == kAdd) || (type == kAdds) ||
      (type == kAnd) || (type == kAnds) || (type == kAsr) || (type == kAsrs) ||
      (type == kBic) || (type == kBics) || (type == kEor) || (type == kEors) ||
      (type == kLsl) || (type == kLsls) || (type == kLsr) || (type == kLsrs) ||
      (type == kOrr) || (type == kOrrs) || (type == kRor) || (type == kRors) ||
      (type == kRsb) || (type == kRsbs) || (type == kSbc) || (type == kSbcs) ||
      (type == kSub) || (type == kSubs));

  CONTEXT_SCOPE;
  VIXL_ASSERT(size.IsBest());
  if (IsUsingT32() && operand.IsRegisterShiftedRegister()) {
    InstructionCondRROp shiftop = NULL;
    switch (operand.GetShift().GetType()) {
      case LSL:
        shiftop = &Assembler::lsl;
        break;
      case LSR:
        shiftop = &Assembler::lsr;
        break;
      case ASR:
        shiftop = &Assembler::asr;
        break;
      case RRX:
        // A RegisterShiftedRegister operand cannot have a shift of type RRX.
        VIXL_UNREACHABLE();
        break;
      case ROR:
        shiftop = &Assembler::ror;
        break;
      default:
        VIXL_UNREACHABLE();
    }
    if (shiftop != NULL) {
      UseScratchRegisterScope temps(this);
      Register rm = operand.GetBaseRegister();
      Register rs = operand.GetShiftRegister();
      // If different from `rn`, we can make use of either `rd`, `rm` or `rs` as
      // a scratch register.
      if (!rd.Is(rn)) temps.Include(rd);
      if (!rm.Is(rn)) temps.Include(rm);
      if (!rs.Is(rn)) temps.Include(rs);
      Register scratch = temps.Acquire();
      CodeBufferCheckScope scope(this, 2 * kMaxInstructionSizeInBytes);
      (this->*shiftop)(cond, scratch, rm, rs);
      return (this->*instruction)(cond, size, rd, rn, scratch);
    }
  }
  if (operand.IsImmediate()) {
    int32_t imm = operand.GetSignedImmediate();
    if (ImmediateT32::IsImmediateT32(~imm)) {
      if (IsUsingT32()) {
        switch (type) {
          case kOrr:
            return orn(cond, rd, rn, ~imm);
          case kOrrs:
            return orns(cond, rd, rn, ~imm);
          default:
            break;
        }
      }
    }
    if (imm < 0) {
      InstructionCondSizeRROp asmcb = NULL;
      // Add and sub are equivalent using an arithmetic negation:
      //   add rd, rn, #imm <-> sub rd, rn, - #imm
      // Add and sub with carry are equivalent using a bitwise NOT:
      //   adc rd, rn, #imm <-> sbc rd, rn, NOT #imm
      switch (type) {
        case kAdd:
          asmcb = &Assembler::sub;
          imm = -imm;
          break;
        case kAdds:
          asmcb = &Assembler::subs;
          imm = -imm;
          break;
        case kSub:
          asmcb = &Assembler::add;
          imm = -imm;
          break;
        case kSubs:
          asmcb = &Assembler::adds;
          imm = -imm;
          break;
        case kAdc:
          asmcb = &Assembler::sbc;
          imm = ~imm;
          break;
        case kAdcs:
          asmcb = &Assembler::sbcs;
          imm = ~imm;
          break;
        case kSbc:
          asmcb = &Assembler::adc;
          imm = ~imm;
          break;
        case kSbcs:
          asmcb = &Assembler::adcs;
          imm = ~imm;
          break;
        default:
          break;
      }
      if (asmcb != NULL) {
        CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
        return (this->*asmcb)(cond, size, rd, rn, Operand(imm));
      }
    }
    UseScratchRegisterScope temps(this);
    // Allow using the destination as a scratch register if possible.
    if (!rd.Is(rn)) temps.Include(rd);
    Register scratch = temps.Acquire();
    // TODO: The scope length was measured empirically. We should analyse the
    // worst-case size and add targetted tests.
    CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
    mov(cond, scratch, operand.GetImmediate());
    return (this->*instruction)(cond, size, rd, rn, scratch);
  }
  Assembler::Delegate(type, instruction, cond, size, rd, rn, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionRL instruction,
                              Register rn,
                              Label* label) {
  // cbz cbnz
  VIXL_ASSERT((type == kCbz) || (type == kCbnz));

  CONTEXT_SCOPE;
  CodeBufferCheckScope scope(this, 2 * kMaxInstructionSizeInBytes);
  if (IsUsingA32()) {
    if (type == kCbz) {
      VIXL_ABORT_WITH_MSG("Cbz is only available for T32.\n");
    } else {
      VIXL_ABORT_WITH_MSG("Cbnz is only available for T32.\n");
    }
  } else if (rn.IsLow()) {
    switch (type) {
      case kCbnz: {
        Label done;
        cbz(rn, &done);
        b(label);
        Bind(&done);
        return;
      }
      case kCbz: {
        Label done;
        cbnz(rn, &done);
        b(label);
        Bind(&done);
        return;
      }
      default:
        break;
    }
  }
  Assembler::Delegate(type, instruction, rn, label);
}


template <typename T>
static inline bool IsI64BitPattern(T imm) {
  for (T mask = 0xff << ((sizeof(T) - 1) * 8); mask != 0; mask >>= 8) {
    if (((imm & mask) != mask) && ((imm & mask) != 0)) return false;
  }
  return true;
}


template <typename T>
static inline bool IsI8BitPattern(T imm) {
  uint8_t imm8 = imm & 0xff;
  for (unsigned rep = sizeof(T) - 1; rep > 0; rep--) {
    imm >>= 8;
    if ((imm & 0xff) != imm8) return false;
  }
  return true;
}


static inline bool CanBeInverted(uint32_t imm32) {
  uint32_t fill8 = 0;

  if ((imm32 & 0xffffff00) == 0xffffff00) {
    //    11111111 11111111 11111111 abcdefgh
    return true;
  }
  if (((imm32 & 0xff) == 0) || ((imm32 & 0xff) == 0xff)) {
    fill8 = imm32 & 0xff;
    imm32 >>= 8;
    if ((imm32 >> 8) == 0xffff) {
      //    11111111 11111111 abcdefgh 00000000
      // or 11111111 11111111 abcdefgh 11111111
      return true;
    }
    if ((imm32 & 0xff) == fill8) {
      imm32 >>= 8;
      if ((imm32 >> 8) == 0xff) {
        //    11111111 abcdefgh 00000000 00000000
        // or 11111111 abcdefgh 11111111 11111111
        return true;
      }
      if ((fill8 == 0xff) && ((imm32 & 0xff) == 0xff)) {
        //    abcdefgh 11111111 11111111 11111111
        return true;
      }
    }
  }
  return false;
}


template <typename RES, typename T>
static inline RES replicate(T imm) {
  VIXL_ASSERT((sizeof(RES) > sizeof(T)) &&
              (((sizeof(RES) / sizeof(T)) * sizeof(T)) == sizeof(RES)));
  RES res = imm;
  for (unsigned i = sizeof(RES) / sizeof(T) - 1; i > 0; i--) {
    res = (res << (sizeof(T) * 8)) | imm;
  }
  return res;
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondDtSSop instruction,
                              Condition cond,
                              DataType dt,
                              SRegister rd,
                              const SOperand& operand) {
  CONTEXT_SCOPE;
  if (type == kVmov) {
    if (operand.IsImmediate() && dt.Is(F32)) {
      const NeonImmediate& neon_imm = operand.GetNeonImmediate();
      if (neon_imm.CanConvert<float>()) {
        // movw ip, imm16
        // movk ip, imm16
        // vmov s0, ip
        UseScratchRegisterScope temps(this);
        Register scratch = temps.Acquire();
        float f = neon_imm.GetImmediate<float>();
        // TODO: The scope length was measured empirically. We should analyse
        // the
        // worst-case size and add targetted tests.
        CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
        mov(cond, scratch, FloatToRawbits(f));
        return vmov(cond, rd, scratch);
      }
    }
  }
  Assembler::Delegate(type, instruction, cond, dt, rd, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondDtDDop instruction,
                              Condition cond,
                              DataType dt,
                              DRegister rd,
                              const DOperand& operand) {
  CONTEXT_SCOPE;
  if (type == kVmov) {
    if (operand.IsImmediate()) {
      const NeonImmediate& neon_imm = operand.GetNeonImmediate();
      switch (dt.GetValue()) {
        case I32:
          if (neon_imm.CanConvert<uint32_t>()) {
            uint32_t imm = neon_imm.GetImmediate<uint32_t>();
            // vmov.i32 d0, 0xabababab will translate into vmov.i8 d0, 0xab
            if (IsI8BitPattern(imm)) {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              return vmov(cond, I8, rd, imm & 0xff);
            }
            // vmov.i32 d0, 0xff0000ff will translate into
            // vmov.i64 d0, 0xff0000ffff0000ff
            if (IsI64BitPattern(imm)) {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              return vmov(cond, I64, rd, replicate<uint64_t>(imm));
            }
            // vmov.i32 d0, 0xffab0000 will translate into
            // vmvn.i32 d0, 0x0054ffff
            if (cond.Is(al) && CanBeInverted(imm)) {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              return vmvn(I32, rd, ~imm);
            }
          }
          break;
        case I16:
          if (neon_imm.CanConvert<uint16_t>()) {
            uint16_t imm = neon_imm.GetImmediate<uint16_t>();
            // vmov.i16 d0, 0xabab will translate into vmov.i8 d0, 0xab
            if (IsI8BitPattern(imm)) {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              return vmov(cond, I8, rd, imm & 0xff);
            }
          }
          break;
        case I64:
          if (neon_imm.CanConvert<uint64_t>()) {
            uint64_t imm = neon_imm.GetImmediate<uint64_t>();
            // vmov.i64 d0, -1 will translate into vmov.i8 d0, 0xff
            if (IsI8BitPattern(imm)) {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              return vmov(cond, I8, rd, imm & 0xff);
            }
            // mov ip, lo(imm64)
            // vdup d0, ip
            // vdup is prefered to 'vmov d0[0]' as d0[1] does not need to be
            // preserved
            {
              UseScratchRegisterScope temps(this);
              Register scratch = temps.Acquire();
              {
                // TODO: The scope length was measured empirically. We should
                // analyse the
                // worst-case size and add targetted tests.
                CodeBufferCheckScope scope(this,
                                           2 * kMaxInstructionSizeInBytes);
                mov(cond, scratch, static_cast<uint32_t>(imm & 0xffffffff));
              }
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              vdup(cond, Untyped32, rd, scratch);
            }
            // mov ip, hi(imm64)
            // vmov d0[1], ip
            {
              UseScratchRegisterScope temps(this);
              Register scratch = temps.Acquire();
              {
                // TODO: The scope length was measured empirically. We should
                // analyse the
                // worst-case size and add targetted tests.
                CodeBufferCheckScope scope(this,
                                           2 * kMaxInstructionSizeInBytes);
                mov(cond, scratch, static_cast<uint32_t>(imm >> 32));
              }
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              vmov(cond, Untyped32, DRegisterLane(rd, 1), scratch);
            }
            return;
          }
          break;
        default:
          break;
      }
      if ((dt.Is(I8) || dt.Is(I16) || dt.Is(I32)) &&
          neon_imm.CanConvert<uint32_t>()) {
        // mov ip, imm32
        // vdup.8 d0, ip
        UseScratchRegisterScope temps(this);
        Register scratch = temps.Acquire();
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          mov(cond, scratch, neon_imm.GetImmediate<uint32_t>());
        }
        DataTypeValue vdup_dt = Untyped32;
        switch (dt.GetValue()) {
          case I8:
            vdup_dt = Untyped8;
            break;
          case I16:
            vdup_dt = Untyped16;
            break;
          case I32:
            vdup_dt = Untyped32;
            break;
          default:
            VIXL_UNREACHABLE();
        }
        CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
        return vdup(cond, vdup_dt, rd, scratch);
      }
      if (dt.Is(F32) && neon_imm.CanConvert<float>()) {
        float f = neon_imm.GetImmediate<float>();
        // Punt to vmov.i32
        // TODO: The scope length was guessed based on the double case below. We
        // should analyse the worst-case size and add targetted tests.
        CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
        return vmov(cond, I32, rd, FloatToRawbits(f));
      }
      if (dt.Is(F64) && neon_imm.CanConvert<double>()) {
        // Punt to vmov.i64
        double d = neon_imm.GetImmediate<double>();
        // TODO: The scope length was measured empirically. We should analyse
        // the
        // worst-case size and add targetted tests.
        CodeBufferCheckScope scope(this, 6 * kMaxInstructionSizeInBytes);
        return vmov(cond, I64, rd, DoubleToRawbits(d));
      }
    }
  }
  Assembler::Delegate(type, instruction, cond, dt, rd, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondDtQQop instruction,
                              Condition cond,
                              DataType dt,
                              QRegister rd,
                              const QOperand& operand) {
  CONTEXT_SCOPE;
  if (type == kVmov) {
    if (operand.IsImmediate()) {
      const NeonImmediate& neon_imm = operand.GetNeonImmediate();
      switch (dt.GetValue()) {
        case I32:
          if (neon_imm.CanConvert<uint32_t>()) {
            uint32_t imm = neon_imm.GetImmediate<uint32_t>();
            // vmov.i32 d0, 0xabababab will translate into vmov.i8 d0, 0xab
            if (IsI8BitPattern(imm)) {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              return vmov(cond, I8, rd, imm & 0xff);
            }
            // vmov.i32 d0, 0xff0000ff will translate into
            // vmov.i64 d0, 0xff0000ffff0000ff
            if (IsI64BitPattern(imm)) {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              return vmov(cond, I64, rd, replicate<uint64_t>(imm));
            }
            // vmov.i32 d0, 0xffab0000 will translate into
            // vmvn.i32 d0, 0x0054ffff
            if (CanBeInverted(imm)) {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              return vmvn(cond, I32, rd, ~imm);
            }
          }
          break;
        case I16:
          if (neon_imm.CanConvert<uint16_t>()) {
            uint16_t imm = neon_imm.GetImmediate<uint16_t>();
            // vmov.i16 d0, 0xabab will translate into vmov.i8 d0, 0xab
            if (IsI8BitPattern(imm)) {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              return vmov(cond, I8, rd, imm & 0xff);
            }
          }
          break;
        case I64:
          if (neon_imm.CanConvert<uint64_t>()) {
            uint64_t imm = neon_imm.GetImmediate<uint64_t>();
            // vmov.i64 d0, -1 will translate into vmov.i8 d0, 0xff
            if (IsI8BitPattern(imm)) {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              return vmov(cond, I8, rd, imm & 0xff);
            }
            // mov ip, lo(imm64)
            // vdup q0, ip
            // vdup is prefered to 'vmov d0[0]' as d0[1-3] don't need to be
            // preserved
            {
              UseScratchRegisterScope temps(this);
              Register scratch = temps.Acquire();
              {
                CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
                mov(cond, scratch, static_cast<uint32_t>(imm & 0xffffffff));
              }
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              vdup(cond, Untyped32, rd, scratch);
            }
            // mov ip, hi(imm64)
            // vmov.i32 d0[1], ip
            // vmov d1, d0
            {
              UseScratchRegisterScope temps(this);
              Register scratch = temps.Acquire();
              {
                CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
                mov(cond, scratch, static_cast<uint32_t>(imm >> 32));
              }
              {
                CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
                vmov(cond,
                     Untyped32,
                     DRegisterLane(rd.GetLowDRegister(), 1),
                     scratch);
              }
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              vmov(cond, F64, rd.GetHighDRegister(), rd.GetLowDRegister());
            }
            return;
          }
          break;
        default:
          break;
      }
      if ((dt.Is(I8) || dt.Is(I16) || dt.Is(I32)) &&
          neon_imm.CanConvert<uint32_t>()) {
        // mov ip, imm32
        // vdup.8 d0, ip
        UseScratchRegisterScope temps(this);
        Register scratch = temps.Acquire();
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          mov(cond, scratch, neon_imm.GetImmediate<uint32_t>());
        }
        DataTypeValue vdup_dt = Untyped32;
        switch (dt.GetValue()) {
          case I8:
            vdup_dt = Untyped8;
            break;
          case I16:
            vdup_dt = Untyped16;
            break;
          case I32:
            vdup_dt = Untyped32;
            break;
          default:
            VIXL_UNREACHABLE();
        }
        CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
        return vdup(cond, vdup_dt, rd, scratch);
      }
      if (dt.Is(F32) && neon_imm.CanConvert<float>()) {
        // Punt to vmov.i64
        float f = neon_imm.GetImmediate<float>();
        CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
        return vmov(cond, I32, rd, FloatToRawbits(f));
      }
      if (dt.Is(F64) && neon_imm.CanConvert<double>()) {
        // Punt to vmov.i64
        double d = neon_imm.GetImmediate<double>();
        CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
        return vmov(cond, I64, rd, DoubleToRawbits(d));
      }
    }
  }
  Assembler::Delegate(type, instruction, cond, dt, rd, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondMop instruction,
                              Condition cond,
                              const MemOperand& operand) {
  // pld pldw pli
  CONTEXT_SCOPE;
  if (operand.IsImmediate()) {
    const Register& rn = operand.GetBaseRegister();
    AddrMode addrmode = operand.GetAddrMode();
    int32_t offset = operand.GetOffsetImmediate();
    switch (addrmode) {
      case PreIndex:
        // Pre-Indexed case:
        // pld [r1, 12345]! will translate into
        //   add r1, r1, 12345
        //   pld [r1]
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          if (operand.GetSign().IsPlus()) {
            add(cond, rn, rn, offset);
          } else {
            sub(cond, rn, rn, offset);
          }
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, MemOperand(rn, Offset));
        }
        return;
      case Offset: {
        UseScratchRegisterScope temps(this);
        Register scratch = temps.Acquire();
        // Offset case:
        // pld [r1, 12345] will translate into
        //   add ip, r1, 12345
        //   pld [ip]
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          if (operand.GetSign().IsPlus()) {
            add(cond, scratch, rn, offset);
          } else {
            sub(cond, scratch, rn, offset);
          }
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, MemOperand(scratch, Offset));
        }
        return;
      }
      case PostIndex:
        // Post-indexed case:
        // pld [r1], imm32 will translate into
        //   pld [r1]
        //   movw ip. imm32 & 0xffffffff
        //   movt ip, imm32 >> 16
        //   add r1, r1, ip
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, MemOperand(rn, Offset));
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          if (operand.GetSign().IsPlus()) {
            add(cond, rn, rn, offset);
          } else {
            sub(cond, rn, rn, offset);
          }
        }
        return;
    }
  }
  if (operand.IsPlainRegister()) {
    const Register& rn = operand.GetBaseRegister();
    AddrMode addrmode = operand.GetAddrMode();
    const Register& rm = operand.GetOffsetRegister();
    switch (addrmode) {
      case PreIndex:
        // Pre-Indexed case:
        // pld [r1, r2]! will translate into
        //   add r1, r1, r2
        //   pld [r1]
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          if (operand.GetSign().IsPlus()) {
            add(cond, rn, rn, rm);
          } else {
            sub(cond, rn, rn, rm);
          }
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, MemOperand(rn, Offset));
        }
        return;
      case Offset: {
        UseScratchRegisterScope temps(this);
        Register scratch = temps.Acquire();
        // Offset case:
        // pld [r1, r2] will translate into
        //   add ip, r1, r2
        //   pld [ip]
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          if (operand.GetSign().IsPlus()) {
            add(cond, scratch, rn, rm);
          } else {
            sub(cond, scratch, rn, rm);
          }
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, MemOperand(scratch, Offset));
        }
        return;
      }
      case PostIndex:
        // Post-indexed case:
        // pld [r1], r2 will translate into
        //   pld [r1]
        //   add r1, r1, r2
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, MemOperand(rn, Offset));
          if (operand.GetSign().IsPlus()) {
            add(cond, rn, rn, rm);
          } else {
            sub(cond, rn, rn, rm);
          }
        }
        return;
    }
  }
  Assembler::Delegate(type, instruction, cond, operand);
}

void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondRMop instruction,
                              Condition cond,
                              Register rd,
                              const MemOperand& operand) {
  // lda ldab ldaex ldaexb ldaexh ldah ldrbt ldrex ldrexb ldrexh ldrht ldrsbt
  // ldrsht ldrt stl stlb stlh strbt strht strt
  CONTEXT_SCOPE;
  if (operand.IsImmediate()) {
    const Register& rn = operand.GetBaseRegister();
    AddrMode addrmode = operand.GetAddrMode();
    int32_t offset = operand.GetOffsetImmediate();
    switch (addrmode) {
      case PreIndex:
        // Pre-Indexed case:
        // lda r0, [r1, 12345]! will translate into
        //   add r1, r1, 12345
        //   lda r0, [r1]
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          if (operand.GetSign().IsPlus()) {
            add(cond, rn, rn, offset);
          } else {
            sub(cond, rn, rn, offset);
          }
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, rd, MemOperand(rn, Offset));
        }
        return;
      case Offset: {
        UseScratchRegisterScope temps(this);
        // Allow using the destination as a scratch register if possible.
        if ((type != kStl) && (type != kStlb) && (type != kStlh) &&
            !rd.Is(rn)) {
          temps.Include(rd);
        }
        Register scratch = temps.Acquire();
        // Offset case:
        // lda r0, [r1, 12345] will translate into
        //   add r0, r1, 12345
        //   lda r0, [r0]
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          if (operand.GetSign().IsPlus()) {
            add(cond, scratch, rn, offset);
          } else {
            sub(cond, scratch, rn, offset);
          }
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, rd, MemOperand(scratch, Offset));
        }
        return;
      }
      case PostIndex:
        // Avoid the unpredictable case 'ldr r0, [r0], imm'
        if (!rn.Is(rd)) {
          // Post-indexed case:
          // lda r0. [r1], imm32 will translate into
          //   lda r0, [r1]
          //   movw ip. imm32 & 0xffffffff
          //   movt ip, imm32 >> 16
          //   add r1, r1, ip
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            (this->*instruction)(cond, rd, MemOperand(rn, Offset));
          }
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            if (operand.GetSign().IsPlus()) {
              add(cond, rn, rn, offset);
            } else {
              sub(cond, rn, rn, offset);
            }
          }
          return;
        }
        break;
    }
  }
  if (operand.IsPlainRegister()) {
    const Register& rn = operand.GetBaseRegister();
    AddrMode addrmode = operand.GetAddrMode();
    const Register& rm = operand.GetOffsetRegister();
    switch (addrmode) {
      case PreIndex:
        // Pre-Indexed case:
        // lda r0, [r1, r2]! will translate into
        //   add r1, r1, 12345
        //   lda r0, [r1]
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          if (operand.GetSign().IsPlus()) {
            add(cond, rn, rn, rm);
          } else {
            sub(cond, rn, rn, rm);
          }
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, rd, MemOperand(rn, Offset));
        }
        return;
      case Offset: {
        UseScratchRegisterScope temps(this);
        // Allow using the destination as a scratch register if possible.
        if ((type != kStl) && (type != kStlb) && (type != kStlh) &&
            !rd.Is(rn)) {
          temps.Include(rd);
        }
        Register scratch = temps.Acquire();
        // Offset case:
        // lda r0, [r1, r2] will translate into
        //   add r0, r1, r2
        //   lda r0, [r0]
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          if (operand.GetSign().IsPlus()) {
            add(cond, scratch, rn, rm);
          } else {
            sub(cond, scratch, rn, rm);
          }
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, rd, MemOperand(scratch, Offset));
        }
        return;
      }
      case PostIndex:
        // Avoid the unpredictable case 'lda r0, [r0], r1'
        if (!rn.Is(rd)) {
          // Post-indexed case:
          // lda r0, [r1], r2 translate into
          //   lda r0, [r1]
          //   add r1, r1, r2
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            (this->*instruction)(cond, rd, MemOperand(rn, Offset));
          }
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            if (operand.GetSign().IsPlus()) {
              add(cond, rn, rn, rm);
            } else {
              sub(cond, rn, rn, rm);
            }
          }
          return;
        }
        break;
    }
  }
  Assembler::Delegate(type, instruction, cond, rd, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondSizeRMop instruction,
                              Condition cond,
                              EncodingSize size,
                              Register rd,
                              const MemOperand& operand) {
  // ldr ldrb ldrh ldrsb ldrsh str strb strh
  CONTEXT_SCOPE;
  VIXL_ASSERT(size.IsBest());
  if (operand.IsImmediate()) {
    const Register& rn = operand.GetBaseRegister();
    AddrMode addrmode = operand.GetAddrMode();
    int32_t offset = operand.GetOffsetImmediate();
    bool ok = true;
    uint32_t mask = 0;
    switch (type) {
      case kLdr:
      case kLdrb:
      case kStr:
      case kStrb:
        if (IsUsingA32() || (addrmode == Offset)) {
          mask = 0xfff;
        } else {
          mask = 0xff;
        }
        break;
      case kLdrsb:
      case kLdrh:
      case kLdrsh:
      case kStrh:
        if (IsUsingT32() && (addrmode == Offset)) {
          mask = 0xfff;
        } else {
          mask = 0xff;
        }
        break;
      default:
        ok = false;
        break;
    }
    if (ok) {
      bool negative;
      // Try to maximize the offset use by the MemOperand (load_store_offset).
      // Add or subtract the part which can't be used by the MemOperand
      // (add_sub_offset).
      int32_t add_sub_offset;
      int32_t load_store_offset;
      load_store_offset = offset & mask;
      if (offset >= 0) {
        negative = false;
        add_sub_offset = offset & ~mask;
      } else {
        negative = true;
        add_sub_offset = -offset & ~mask;
        if (load_store_offset > 0) add_sub_offset += mask + 1;
      }
      switch (addrmode) {
        case PreIndex:
          // Pre-Indexed case:
          // ldr r0, [r1, 12345]! will translate into
          //   add r1, r1, 12345
          //   ldr r0, [r1]
          {
            CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
            if (negative) {
              sub(cond, rn, rn, add_sub_offset);
            } else {
              add(cond, rn, rn, add_sub_offset);
            }
          }
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            (this->*instruction)(cond,
                                 size,
                                 rd,
                                 MemOperand(rn, load_store_offset, PreIndex));
          }
          return;
        case Offset: {
          UseScratchRegisterScope temps(this);
          // Allow using the destination as a scratch register if possible.
          if ((type != kStr) && (type != kStrb) && (type != kStrh) &&
              !rd.Is(rn)) {
            temps.Include(rd);
          }
          Register scratch = temps.Acquire();
          // Offset case:
          // ldr r0, [r1, 12345] will translate into
          //   add r0, r1, 12345
          //   ldr r0, [r0]
          {
            CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
            if (negative) {
              sub(cond, scratch, rn, add_sub_offset);
            } else {
              add(cond, scratch, rn, add_sub_offset);
            }
          }
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            (this->*instruction)(cond,
                                 size,
                                 rd,
                                 MemOperand(scratch, load_store_offset));
          }
          return;
        }
        case PostIndex:
          // Avoid the unpredictable case 'ldr r0, [r0], imm'
          if (!rn.Is(rd)) {
            // Post-indexed case:
            // ldr r0. [r1], imm32 will translate into
            //   ldr r0, [r1]
            //   movw ip. imm32 & 0xffffffff
            //   movt ip, imm32 >> 16
            //   add r1, r1, ip
            {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              (this->*instruction)(cond,
                                   size,
                                   rd,
                                   MemOperand(rn,
                                              load_store_offset,
                                              PostIndex));
            }
            {
              CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
              if (negative) {
                sub(cond, rn, rn, add_sub_offset);
              } else {
                add(cond, rn, rn, add_sub_offset);
              }
            }
            return;
          }
          break;
      }
    }
  }
  if (operand.IsPlainRegister()) {
    const Register& rn = operand.GetBaseRegister();
    AddrMode addrmode = operand.GetAddrMode();
    const Register& rm = operand.GetOffsetRegister();
    switch (addrmode) {
      case PreIndex:
        // Pre-Indexed case:
        // ldr r0, [r1, r2]! will translate into
        //   add r1, r1, r2
        //   ldr r0, [r1]
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          if (operand.GetSign().IsPlus()) {
            add(cond, rn, rn, rm);
          } else {
            sub(cond, rn, rn, rm);
          }
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, size, rd, MemOperand(rn, Offset));
        }
        return;
      case Offset: {
        UseScratchRegisterScope temps(this);
        // Allow using the destination as a scratch register if possible.
        if ((type != kStr) && (type != kStrb) && (type != kStrh) &&
            !rd.Is(rn)) {
          temps.Include(rd);
        }
        Register scratch = temps.Acquire();
        // Offset case:
        // ldr r0, [r1, r2] will translate into
        //   add r0, r1, r2
        //   ldr r0, [r0]
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          if (operand.GetSign().IsPlus()) {
            add(cond, scratch, rn, rm);
          } else {
            sub(cond, scratch, rn, rm);
          }
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, size, rd, MemOperand(scratch, Offset));
        }
        return;
      }
      case PostIndex:
        // Avoid the unpredictable case 'ldr r0, [r0], imm'
        if (!rn.Is(rd)) {
          // Post-indexed case:
          // ldr r0. [r1], r2 will translate into
          //   ldr r0, [r1]
          //   add r1, r1, r2
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            (this->*instruction)(cond, size, rd, MemOperand(rn, Offset));
          }
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            if (operand.GetSign().IsPlus()) {
              add(cond, rn, rn, rm);
            } else {
              sub(cond, rn, rn, rm);
            }
          }
          return;
        }
        break;
    }
  }
  Assembler::Delegate(type, instruction, cond, size, rd, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondRRMop instruction,
                              Condition cond,
                              Register rt,
                              Register rt2,
                              const MemOperand& operand) {
  // ldaexd, ldrd, ldrexd, stlex, stlexb, stlexh, strd, strex, strexb, strexh
  CONTEXT_SCOPE;

  bool can_delegate = true;
  if (((type == kLdrd) || (type == kStrd) || (type == kLdaexd) ||
       (type == kLdrexd)) &&
      IsUsingA32()) {
    can_delegate =
        (((rt.GetCode() & 1) == 0) && !rt.Is(lr) &&
         (((rt.GetCode() + 1) % kNumberOfRegisters) == rt2.GetCode()));
  }

  if (can_delegate) {
    if (operand.IsImmediate()) {
      const Register& rn = operand.GetBaseRegister();
      AddrMode addrmode = operand.GetAddrMode();
      int32_t offset = operand.GetOffsetImmediate();
      switch (addrmode) {
        case PreIndex:
          // Pre-Indexed case:
          // ldrd r0, r1, [r2, 12345]! will translate into
          //   add r2, 12345
          //   ldrd r0, r1, [r2]
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            if (operand.GetSign().IsPlus()) {
              add(cond, rn, rn, offset);
            } else {
              sub(cond, rn, rn, offset);
            }
          }
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            (this->*instruction)(cond, rt, rt2, MemOperand(rn, Offset));
          }
          return;
        case Offset: {
          UseScratchRegisterScope temps(this);
          // Allow using the destinations as a scratch registers if possible.
          if ((type != kStlex) && (type != kStlexb) && (type != kStlexh) &&
              (type != kStrd) && (type != kStrex) && (type != kStrexb) &&
              (type != kStrexh)) {
            if (!rt.Is(rn)) {
              temps.Include(rt);
            }
            if (!rt2.Is(rn)) {
              temps.Include(rt2);
            }
          }
          Register scratch = temps.Acquire();
          // Offset case:
          // ldrd r0, r1, [r2, 12345] will translate into
          //   add r0, r2, 12345
          //   ldrd r0, r1, [r0]
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            if (operand.GetSign().IsPlus()) {
              add(cond, scratch, rn, offset);
            } else {
              sub(cond, scratch, rn, offset);
            }
          }
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            (this->*instruction)(cond, rt, rt2, MemOperand(scratch, Offset));
          }
          return;
        }
        case PostIndex:
          // Avoid the unpredictable case 'ldr r0, r1, [r0], imm'
          if (!rn.Is(rt) && !rn.Is(rt2)) {
            // Post-indexed case:
            // ldrd r0, r1, [r2], imm32 will translate into
            //   ldrd r0, r1, [r2]
            //   movw ip. imm32 & 0xffffffff
            //   movt ip, imm32 >> 16
            //   add r2, ip
            {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              (this->*instruction)(cond, rt, rt2, MemOperand(rn, Offset));
            }
            {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              if (operand.GetSign().IsPlus()) {
                add(cond, rn, rn, offset);
              } else {
                sub(cond, rn, rn, offset);
              }
            }
            return;
          }
          break;
      }
    }
    if (operand.IsPlainRegister()) {
      const Register& rn = operand.GetBaseRegister();
      const Register& rm = operand.GetOffsetRegister();
      AddrMode addrmode = operand.GetAddrMode();
      switch (addrmode) {
        case PreIndex:
          // ldrd r0, r1, [r2, r3]! will translate into
          //   add r2, r3
          //   ldrd r0, r1, [r2]
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            if (operand.GetSign().IsPlus()) {
              add(cond, rn, rn, rm);
            } else {
              sub(cond, rn, rn, rm);
            }
          }
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            (this->*instruction)(cond, rt, rt2, MemOperand(rn, Offset));
          }
          return;
        case PostIndex:
          // ldrd r0, r1, [r2], r3 will translate into
          //   ldrd r0, r1, [r2]
          //   add r2, r3
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            (this->*instruction)(cond, rt, rt2, MemOperand(rn, Offset));
          }
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            if (operand.GetSign().IsPlus()) {
              add(cond, rn, rn, rm);
            } else {
              sub(cond, rn, rn, rm);
            }
          }
          return;
        case Offset: {
          UseScratchRegisterScope temps(this);
          // Allow using the destinations as a scratch registers if possible.
          if ((type != kStlex) && (type != kStlexb) && (type != kStlexh) &&
              (type != kStrd) && (type != kStrex) && (type != kStrexb) &&
              (type != kStrexh)) {
            if (!rt.Is(rn)) {
              temps.Include(rt);
            }
            if (!rt2.Is(rn)) {
              temps.Include(rt2);
            }
          }
          Register scratch = temps.Acquire();
          // Offset case:
          // ldrd r0, r1, [r2, r3] will translate into
          //   add r0, r2, r3
          //   ldrd r0, r1, [r0]
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            if (operand.GetSign().IsPlus()) {
              add(cond, scratch, rn, rm);
            } else {
              sub(cond, scratch, rn, rm);
            }
          }
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            (this->*instruction)(cond, rt, rt2, MemOperand(scratch, Offset));
          }
          return;
        }
      }
    }
  }
  Assembler::Delegate(type, instruction, cond, rt, rt2, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondDtSMop instruction,
                              Condition cond,
                              DataType dt,
                              SRegister rd,
                              const MemOperand& operand) {
  // vldr.32 vstr.32
  CONTEXT_SCOPE;
  if (operand.IsImmediate()) {
    const Register& rn = operand.GetBaseRegister();
    AddrMode addrmode = operand.GetAddrMode();
    int32_t offset = operand.GetOffsetImmediate();
    VIXL_ASSERT(((offset > 0) && operand.GetSign().IsPlus()) ||
                ((offset < 0) && operand.GetSign().IsMinus()) || (offset == 0));
    if (rn.IsPC()) {
      VIXL_ABORT_WITH_MSG(
          "The MacroAssembler does not convert vldr or vstr with a PC base "
          "register.\n");
    }
    switch (addrmode) {
      case PreIndex:
        // Pre-Indexed case:
        // vldr.32 s0, [r1, 12345]! will translate into
        //   add r1, 12345
        //   vldr.32 s0, [r1]
        if (offset != 0) {
          CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
          add(cond, rn, rn, offset);
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, dt, rd, MemOperand(rn, Offset));
        }
        return;
      case Offset: {
        UseScratchRegisterScope temps(this);
        Register scratch = temps.Acquire();
        // Offset case:
        // vldr.32 s0, [r1, 12345] will translate into
        //   add ip, r1, 12345
        //   vldr.32 s0, [ip]
        {
          VIXL_ASSERT(offset != 0);
          CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
          add(cond, scratch, rn, offset);
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, dt, rd, MemOperand(scratch, Offset));
        }
        return;
      }
      case PostIndex:
        // Post-indexed case:
        // vldr.32 s0, [r1], imm32 will translate into
        //   vldr.32 s0, [r1]
        //   movw ip. imm32 & 0xffffffff
        //   movt ip, imm32 >> 16
        //   add r1, ip
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, dt, rd, MemOperand(rn, Offset));
        }
        if (offset != 0) {
          CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
          add(cond, rn, rn, offset);
        }
        return;
    }
  }
  Assembler::Delegate(type, instruction, cond, dt, rd, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondRRRMop instruction,
                              Condition cond,
                              Register rd,
                              Register rt,
                              Register rt2,
                              const MemOperand& operand) {
  // stlexd strexd
  CONTEXT_SCOPE;
  if (IsUsingT32() ||
      (((rt.GetCode() & 1) == 0) && !rt.Is(lr) &&
       (((rt.GetCode() + 1) % kNumberOfRegisters) == rt2.GetCode()))) {
    if (operand.IsImmediate()) {
      const Register& rn = operand.GetBaseRegister();
      AddrMode addrmode = operand.GetAddrMode();
      int32_t offset = operand.GetOffsetImmediate();
      switch (addrmode) {
        case PreIndex:
          // Pre-Indexed case:
          // strexd r5, r0, r1, [r2, 12345]! will translate into
          //   add r2, 12345
          //   strexd r5,  r0, r1, [r2]
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            if (operand.GetSign().IsPlus()) {
              add(cond, rn, rn, offset);
            } else {
              sub(cond, rn, rn, offset);
            }
          }
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            (this->*instruction)(cond, rd, rt, rt2, MemOperand(rn, Offset));
          }
          return;
        case Offset: {
          UseScratchRegisterScope temps(this);
          // Allow using the destination as a scratch register if possible.
          if (!rd.Is(rn) && !rd.Is(rt) && !rd.Is(rt2)) temps.Include(rd);
          Register scratch = temps.Acquire();
          // Offset case:
          // strexd r5, r0, r1, [r2, 12345] will translate into
          //   add r5, r2, 12345
          //   strexd r5, r0, r1, [r5]
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            if (operand.GetSign().IsPlus()) {
              add(cond, scratch, rn, offset);
            } else {
              sub(cond, scratch, rn, offset);
            }
          }
          {
            CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
            (this->*instruction)(cond,
                                 rd,
                                 rt,
                                 rt2,
                                 MemOperand(scratch, Offset));
          }
          return;
        }
        case PostIndex:
          // Avoid the unpredictable case 'ldr r0, r1, [r0], imm'
          if (!rn.Is(rt) && !rn.Is(rt2)) {
            // Post-indexed case:
            // strexd r5, r0, r1, [r2], imm32 will translate into
            //   strexd r5, r0, r1, [r2]
            //   movw ip. imm32 & 0xffffffff
            //   movt ip, imm32 >> 16
            //   add r2, ip
            {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              (this->*instruction)(cond, rd, rt, rt2, MemOperand(rn, Offset));
            }
            {
              CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
              if (operand.GetSign().IsPlus()) {
                add(cond, rn, rn, offset);
              } else {
                sub(cond, rn, rn, offset);
              }
            }
            return;
          }
          break;
      }
    }
  }
  Assembler::Delegate(type, instruction, cond, rd, rt, rt2, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondDtDMop instruction,
                              Condition cond,
                              DataType dt,
                              DRegister rd,
                              const MemOperand& operand) {
  // vldr.64 vstr.64
  CONTEXT_SCOPE;
  if (operand.IsImmediate()) {
    const Register& rn = operand.GetBaseRegister();
    AddrMode addrmode = operand.GetAddrMode();
    int32_t offset = operand.GetOffsetImmediate();
    VIXL_ASSERT(((offset > 0) && operand.GetSign().IsPlus()) ||
                ((offset < 0) && operand.GetSign().IsMinus()) || (offset == 0));
    if (rn.IsPC()) {
      VIXL_ABORT_WITH_MSG(
          "The MacroAssembler does not convert vldr or vstr with a PC base "
          "register.\n");
    }
    switch (addrmode) {
      case PreIndex:
        // Pre-Indexed case:
        // vldr.64 d0, [r1, 12345]! will translate into
        //   add r1, 12345
        //   vldr.64 d0, [r1]
        if (offset != 0) {
          CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
          add(cond, rn, rn, offset);
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, dt, rd, MemOperand(rn, Offset));
        }
        return;
      case Offset: {
        UseScratchRegisterScope temps(this);
        Register scratch = temps.Acquire();
        // Offset case:
        // vldr.64 d0, [r1, 12345] will translate into
        //   add ip, r1, 12345
        //   vldr.32 s0, [ip]
        {
          VIXL_ASSERT(offset != 0);
          CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
          add(cond, scratch, rn, offset);
        }
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, dt, rd, MemOperand(scratch, Offset));
        }
        return;
      }
      case PostIndex:
        // Post-indexed case:
        // vldr.64 d0. [r1], imm32 will translate into
        //   vldr.64 d0, [r1]
        //   movw ip. imm32 & 0xffffffff
        //   movt ip, imm32 >> 16
        //   add r1, ip
        {
          CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
          (this->*instruction)(cond, dt, rd, MemOperand(rn, Offset));
        }
        if (offset != 0) {
          CodeBufferCheckScope scope(this, 3 * kMaxInstructionSizeInBytes);
          add(cond, rn, rn, offset);
        }
        return;
    }
  }
  Assembler::Delegate(type, instruction, cond, dt, rd, operand);
}


void MacroAssembler::Delegate(InstructionType type,
                              InstructionCondMsrOp instruction,
                              Condition cond,
                              MaskedSpecialRegister spec_reg,
                              const Operand& operand) {
  USE(type);
  VIXL_ASSERT(type == kMsr);
  if (operand.IsImmediate()) {
    UseScratchRegisterScope temps(this);
    Register scratch = temps.Acquire();
    {
      CodeBufferCheckScope scope(this, 2 * kMaxInstructionSizeInBytes);
      mov(cond, scratch, operand);
    }
    CodeBufferCheckScope scope(this, kMaxInstructionSizeInBytes);
    msr(cond, spec_reg, scratch);
    return;
  }
  Assembler::Delegate(type, instruction, cond, spec_reg, operand);
}

#undef CONTEXT_SCOPE
#undef TOSTRING
#undef STRINGIFY

// Start of generated code.
// End of generated code.
}  // namespace aarch32
}  // namespace vixl
