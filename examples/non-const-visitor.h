// Copyright 2014, ARM Limited
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

#ifndef VIXL_EXAMPLES_NON_CONST_VISITOR_H_
#define VIXL_EXAMPLES_NON_CONST_VISITOR_H_

using namespace vixl;

class SwitchAddSubRegisterSources : public DecoderVisitor {
 public:
  SwitchAddSubRegisterSources()
      : DecoderVisitor(DecoderVisitor::kNonConstVisitor) {}

  // Our visitor switches the register sources for some add and sub instructions
  // (not all add and sub instructions). Visitors are listed by the macro
  // `VISITOR_LIST` in a64/decoder-a64.h.
  virtual void VisitAddSubShifted(const Instruction* instr) {
    int rn = instr->Rn();
    int rm = instr->Rm();
    // Only non-const visitors are allowed to discard constness of the visited
    // instruction.
    Instruction* mutable_instr = MutableInstruction(instr);
    Instr instr_bits = mutable_instr->InstructionBits();

    // Switch the bitfields for the `rn` and `rm` registers.
    instr_bits &= ~(Rn_mask | Rm_mask);
    instr_bits |= (rn << Rm_offset) | (rm << Rn_offset);

    // Rewrite the instruction.
    mutable_instr->SetInstructionBits(instr_bits);
  }

  // Define the remaining visitors to do nothing.
#define UNUSED_VISITOR_LIST(V)      \
  V(PCRelAddressing)                \
  V(AddSubImmediate)                \
  V(LogicalImmediate)               \
  V(MoveWideImmediate)              \
  V(Bitfield)                       \
  V(Extract)                        \
  V(UnconditionalBranch)            \
  V(UnconditionalBranchToRegister)  \
  V(CompareBranch)                  \
  V(TestBranch)                     \
  V(ConditionalBranch)              \
  V(System)                         \
  V(Exception)                      \
  V(LoadStorePairPostIndex)         \
  V(LoadStorePairOffset)            \
  V(LoadStorePairPreIndex)          \
  V(LoadStorePairNonTemporal)       \
  V(LoadLiteral)                    \
  V(LoadStoreUnscaledOffset)        \
  V(LoadStorePostIndex)             \
  V(LoadStorePreIndex)              \
  V(LoadStoreRegisterOffset)        \
  V(LoadStoreUnsignedOffset)        \
  V(LoadStoreExclusive)             \
  V(LogicalShifted)                 \
  V(AddSubExtended)                 \
  V(AddSubWithCarry)                \
  V(ConditionalCompareRegister)     \
  V(ConditionalCompareImmediate)    \
  V(ConditionalSelect)              \
  V(DataProcessing1Source)          \
  V(DataProcessing2Source)          \
  V(DataProcessing3Source)          \
  V(FPCompare)                      \
  V(FPConditionalCompare)           \
  V(FPConditionalSelect)            \
  V(FPImmediate)                    \
  V(FPDataProcessing1Source)        \
  V(FPDataProcessing2Source)        \
  V(FPDataProcessing3Source)        \
  V(FPIntegerConvert)               \
  V(FPFixedPointConvert)            \
  V(Crypto2RegSHA)                  \
  V(Crypto3RegSHA)                  \
  V(CryptoAES)                      \
  V(NEON2RegMisc)                   \
  V(NEON3Different)                 \
  V(NEON3Same)                      \
  V(NEONAcrossLanes)                \
  V(NEONByIndexedElement)           \
  V(NEONCopy)                       \
  V(NEONExtract)                    \
  V(NEONLoadStoreMultiStruct)       \
  V(NEONLoadStoreMultiStructPostIndex)  \
  V(NEONLoadStoreSingleStruct)      \
  V(NEONLoadStoreSingleStructPostIndex) \
  V(NEONModifiedImmediate)          \
  V(NEONScalar2RegMisc)             \
  V(NEONScalar3Diff)                \
  V(NEONScalar3Same)                \
  V(NEONScalarByIndexedElement)     \
  V(NEONScalarCopy)                 \
  V(NEONScalarPairwise)             \
  V(NEONScalarShiftImmediate)       \
  V(NEONShiftImmediate)             \
  V(NEONTable)                      \
  V(NEONPerm)                       \
  V(Unallocated)                    \
  V(Unimplemented)
#define DEFINE_UNUSED_VISITOR(Name)                                  \
  virtual void Visit##Name(const Instruction* i) {                   \
    USE(i); /* Prevents compiler warnings about unused variables. */ \
  }
  UNUSED_VISITOR_LIST(DEFINE_UNUSED_VISITOR)
#undef DEFINE_UNUSED_VISITOR
#undef UNUSED_VISITOR_LIST
};


void GenerateNonConstVisitorTestCode(MacroAssembler* masm);

int64_t RunNonConstVisitorTestGeneratedCode(const Instruction* start_instr);

void ModifyNonConstVisitorTestGeneratedCode(Instruction* start,
                                            Instruction* end);


#endif
