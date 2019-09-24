// Copyright 2014, VIXL authors
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

#include "instrument-aarch64.h"

namespace vixl {
namespace aarch64 {

Counter::Counter(const char* name, CounterType type)
    : count_(0), enabled_(false), type_(type) {
  VIXL_ASSERT(name != NULL);
  strncpy(name_, name, kCounterNameMaxLength);
  // Make sure `name_` is always NULL-terminated, even if the source's length is
  // higher.
  name_[kCounterNameMaxLength - 1] = '\0';
}


void Counter::Enable() { enabled_ = true; }


void Counter::Disable() { enabled_ = false; }


bool Counter::IsEnabled() { return enabled_; }


void Counter::Increment() {
  if (enabled_) {
    count_++;
  }
}


uint64_t Counter::GetCount() {
  uint64_t result = count_;
  if (type_ == Gauge) {
    // If the counter is a Gauge, reset the count after reading.
    count_ = 0;
  }
  return result;
}


const char* Counter::GetName() { return name_; }


CounterType Counter::GetType() { return type_; }


struct CounterDescriptor {
  const char* name;
  CounterType type;
};


static const CounterDescriptor kCounterList[] =
    {{"Instruction", Cumulative},

     {"Move Immediate", Gauge},
     {"Add/Sub DP", Gauge},
     {"Logical DP", Gauge},
     {"Other Int DP", Gauge},
     {"FP DP", Gauge},

     {"Conditional Select", Gauge},
     {"Conditional Compare", Gauge},

     {"Unconditional Branch", Gauge},
     {"Compare and Branch", Gauge},
     {"Test and Branch", Gauge},
     {"Conditional Branch", Gauge},

     {"Load Integer", Gauge},
     {"Load FP", Gauge},
     {"Load Pair", Gauge},
     {"Load Literal", Gauge},

     {"Store Integer", Gauge},
     {"Store FP", Gauge},
     {"Store Pair", Gauge},

     {"PC Addressing", Gauge},
     {"Other", Gauge},
     {"NEON", Gauge},
     {"SVE", Gauge},
     {"Crypto", Gauge}};


Instrument::Instrument(const char* datafile, uint64_t sample_period)
    : output_stream_(stdout), sample_period_(sample_period) {
  // Set up the output stream. If datafile is non-NULL, use that file. If it
  // can't be opened, or datafile is NULL, use stdout.
  if (datafile != NULL) {
    output_stream_ = fopen(datafile, "w");
    if (output_stream_ == NULL) {
      printf("Can't open output file %s. Using stdout.\n", datafile);
      output_stream_ = stdout;
    }
  }

  static const int num_counters =
      sizeof(kCounterList) / sizeof(CounterDescriptor);

  // Dump an instrumentation description comment at the top of the file.
  fprintf(output_stream_, "# counters=%d\n", num_counters);
  fprintf(output_stream_, "# sample_period=%" PRIu64 "\n", sample_period_);

  // Construct Counter objects from counter description array.
  for (int i = 0; i < num_counters; i++) {
    Counter* counter = new Counter(kCounterList[i].name, kCounterList[i].type);
    counters_.push_back(counter);
  }

  DumpCounterNames();
}


Instrument::~Instrument() {
  // Dump any remaining instruction data to the output file.
  DumpCounters();

  // Free all the counter objects.
  std::list<Counter*>::iterator it;
  for (it = counters_.begin(); it != counters_.end(); it++) {
    delete *it;
  }

  if (output_stream_ != stdout) {
    fclose(output_stream_);
  }
}


void Instrument::Update() {
  // Increment the instruction counter, and dump all counters if a sample period
  // has elapsed.
  static Counter* counter = GetCounter("Instruction");
  VIXL_ASSERT(counter->GetType() == Cumulative);
  counter->Increment();

  if ((sample_period_ != 0) && counter->IsEnabled() &&
      (counter->GetCount() % sample_period_) == 0) {
    DumpCounters();
  }
}


void Instrument::DumpCounters() {
  // Iterate through the counter objects, dumping their values to the output
  // stream.
  std::list<Counter*>::const_iterator it;
  for (it = counters_.begin(); it != counters_.end(); it++) {
    fprintf(output_stream_, "%" PRIu64 ",", (*it)->GetCount());
  }
  fprintf(output_stream_, "\n");
  fflush(output_stream_);
}


void Instrument::DumpCounterNames() {
  // Iterate through the counter objects, dumping the counter names to the
  // output stream.
  std::list<Counter*>::const_iterator it;
  for (it = counters_.begin(); it != counters_.end(); it++) {
    fprintf(output_stream_, "%s,", (*it)->GetName());
  }
  fprintf(output_stream_, "\n");
  fflush(output_stream_);
}


void Instrument::HandleInstrumentationEvent(unsigned event) {
  switch (event) {
    case InstrumentStateEnable:
      Enable();
      break;
    case InstrumentStateDisable:
      Disable();
      break;
    default:
      DumpEventMarker(event);
  }
}


void Instrument::DumpEventMarker(unsigned marker) {
  // Dumpan event marker to the output stream as a specially formatted comment
  // line.
  static Counter* counter = GetCounter("Instruction");

  fprintf(output_stream_,
          "# %c%c @ %" PRId64 "\n",
          marker & 0xff,
          (marker >> 8) & 0xff,
          counter->GetCount());
}


Counter* Instrument::GetCounter(const char* name) {
  // Get a Counter object by name from the counter list.
  std::list<Counter*>::const_iterator it;
  for (it = counters_.begin(); it != counters_.end(); it++) {
    if (strcmp((*it)->GetName(), name) == 0) {
      return *it;
    }
  }

  // A Counter by that name does not exist: print an error message to stderr
  // and the output file, and exit.
  static const char* error_message =
      "# Error: Unknown counter \"%s\". Exiting.\n";
  fprintf(stderr, error_message, name);
  fprintf(output_stream_, error_message, name);
  exit(1);
}


void Instrument::Enable() {
  std::list<Counter*>::iterator it;
  for (it = counters_.begin(); it != counters_.end(); it++) {
    (*it)->Enable();
  }
}


void Instrument::Disable() {
  std::list<Counter*>::iterator it;
  for (it = counters_.begin(); it != counters_.end(); it++) {
    (*it)->Disable();
  }
}

// Most visitors have exactly the same form, so use a macro list to define them.
#define VIXL_SIMPLE_INSTRUMENT_VISITOR_LIST(V)          \
  V("Add/Sub DP", AddSubExtended)                       \
  V("Add/Sub DP", AddSubImmediate)                      \
  V("Add/Sub DP", AddSubShifted)                        \
  V("Add/Sub DP", AddSubWithCarry)                      \
  V("Compare and Branch", CompareBranch)                \
  V("Conditional Branch", ConditionalBranch)            \
  V("Conditional Compare", ConditionalCompareImmediate) \
  V("Conditional Compare", ConditionalCompareRegister)  \
  V("Conditional Compare", FPConditionalCompare)        \
  V("Conditional Select", ConditionalSelect)            \
  V("Conditional Select", FPConditionalSelect)          \
  V("Crypto", Crypto2RegSHA)                            \
  V("Crypto", Crypto3RegSHA)                            \
  V("Crypto", CryptoAES)                                \
  V("FP DP", FPCompare)                                 \
  V("FP DP", FPDataProcessing1Source)                   \
  V("FP DP", FPDataProcessing2Source)                   \
  V("FP DP", FPDataProcessing3Source)                   \
  V("FP DP", FPFixedPointConvert)                       \
  V("FP DP", FPImmediate)                               \
  V("FP DP", FPIntegerConvert)                          \
  V("Load Integer", LoadStorePAC)                       \
  V("Load Literal", LoadLiteral)                        \
  V("Logical DP", LogicalImmediate)                     \
  V("Logical DP", LogicalShifted)                       \
  V("NEON", NEON2RegMisc)                               \
  V("NEON", NEON2RegMiscFP16)                           \
  V("NEON", NEON3Different)                             \
  V("NEON", NEON3Same)                                  \
  V("NEON", NEON3SameExtra)                             \
  V("NEON", NEON3SameFP16)                              \
  V("NEON", NEONAcrossLanes)                            \
  V("NEON", NEONByIndexedElement)                       \
  V("NEON", NEONCopy)                                   \
  V("NEON", NEONExtract)                                \
  V("NEON", NEONLoadStoreMultiStruct)                   \
  V("NEON", NEONLoadStoreMultiStructPostIndex)          \
  V("NEON", NEONLoadStoreSingleStruct)                  \
  V("NEON", NEONLoadStoreSingleStructPostIndex)         \
  V("NEON", NEONModifiedImmediate)                      \
  V("NEON", NEONPerm)                                   \
  V("NEON", NEONScalar2RegMisc)                         \
  V("NEON", NEONScalar2RegMiscFP16)                     \
  V("NEON", NEONScalar3Diff)                            \
  V("NEON", NEONScalar3Same)                            \
  V("NEON", NEONScalar3SameExtra)                       \
  V("NEON", NEONScalar3SameFP16)                        \
  V("NEON", NEONScalarByIndexedElement)                 \
  V("NEON", NEONScalarCopy)                             \
  V("NEON", NEONScalarPairwise)                         \
  V("NEON", NEONScalarShiftImmediate)                   \
  V("NEON", NEONShiftImmediate)                         \
  V("NEON", NEONTable)                                  \
  V("Other Int DP", Bitfield)                           \
  V("Other Int DP", DataProcessing1Source)              \
  V("Other Int DP", DataProcessing2Source)              \
  V("Other Int DP", DataProcessing3Source)              \
  V("Other Int DP", Extract)                            \
  V("Other", AtomicMemory)                              \
  V("Other", EvaluateIntoFlags)                         \
  V("Other", Exception)                                 \
  V("Other", LoadStoreExclusive)                        \
  V("Other", Reserved)                                  \
  V("Other", RotateRightIntoFlags)                      \
  V("Other", System)                                    \
  V("Other", Unallocated)                               \
  V("Other", Unimplemented)                             \
  V("PC Addressing", PCRelAddressing)                   \
  V("SVE", SVEAddressGeneration)                        \
  V("SVE", SVEBitwiseImm)                               \
  V("SVE", SVEBitwiseLogicalUnpredicated)               \
  V("SVE", SVEBitwiseShiftPredicated)                   \
  V("SVE", SVEBitwiseShiftUnpredicated)                 \
  V("SVE", SVEElementCount)                             \
  V("SVE", SVEFPAccumulatingReduction)                  \
  V("SVE", SVEFPArithmeticPredicated)                   \
  V("SVE", SVEFPArithmeticUnpredicated)                 \
  V("SVE", SVEFPCompareVectors)                         \
  V("SVE", SVEFPCompareWithZero)                        \
  V("SVE", SVEFPComplexAddition)                        \
  V("SVE", SVEFPComplexMulAdd)                          \
  V("SVE", SVEFPComplexMulAddIndex)                     \
  V("SVE", SVEFPFastReduction)                          \
  V("SVE", SVEFPMulAdd)                                 \
  V("SVE", SVEFPMulAddIndex)                            \
  V("SVE", SVEFPMulIndex)                               \
  V("SVE", SVEFPUnaryOpPredicated)                      \
  V("SVE", SVEFPUnaryOpUnpredicated)                    \
  V("SVE", SVEIncDecByPredicateCount)                   \
  V("SVE", SVEIndexGeneration)                          \
  V("SVE", SVEIntArithmeticUnpredicated)                \
  V("SVE", SVEIntBinaryArithmeticPredicated)            \
  V("SVE", SVEIntCompareScalars)                        \
  V("SVE", SVEIntCompareSignedImm)                      \
  V("SVE", SVEIntCompareUnsignedImm)                    \
  V("SVE", SVEIntCompareVectors)                        \
  V("SVE", SVEIntMiscUnpredicated)                      \
  V("SVE", SVEIntMulAddPredicated)                      \
  V("SVE", SVEIntMulAddUnpredicated)                    \
  V("SVE", SVEIntReduction)                             \
  V("SVE", SVEIntUnaryArithmeticPredicated)             \
  V("SVE", SVEIntWideImmPredicated)                     \
  V("SVE", SVEIntWideImmUnpredicated)                   \
  V("SVE", SVEMem32BitGatherAndUnsizedContiguous)       \
  V("SVE", SVEMem64BitGather)                           \
  V("SVE", SVEMemContiguousLoad)                        \
  V("SVE", SVEMemStore)                                 \
  V("SVE", SVEMulIndex)                                 \
  V("SVE", SVEPartitionBreak)                           \
  V("SVE", SVEPermutePredicate)                         \
  V("SVE", SVEPermuteVectorExtract)                     \
  V("SVE", SVEPermuteVectorInterleaving)                \
  V("SVE", SVEPermuteVectorPredicated)                  \
  V("SVE", SVEPermuteVectorUnpredicated)                \
  V("SVE", SVEPredicateCount)                           \
  V("SVE", SVEPredicateLogicalOp)                       \
  V("SVE", SVEPredicateFirstActive)                     \
  V("SVE", SVEPredicateInitialize)                      \
  V("SVE", SVEPredicateNextActive)                      \
  V("SVE", SVEPredicateReadFromFFR_Predicated)          \
  V("SVE", SVEPredicateReadFromFFR_Unpredicated)        \
  V("SVE", SVEPredicateTest)                            \
  V("SVE", SVEPredicateZero)                            \
  V("SVE", SVEPropagateBreak)                           \
  V("SVE", SVEStackAllocation)                          \
  V("SVE", SVEVectorSelect)                             \
  V("SVE", SVEWriteFFR)                                 \
  V("SVE", SVEContiguousLoad_ScalarPlusImm)             \
  V("SVE", SVEContiguousLoad_ScalarPlusScalar)          \
  V("Test and Branch", TestBranch)                      \
  V("Unconditional Branch", UnconditionalBranch)        \
  V("Unconditional Branch", UnconditionalBranchToRegister)

#define VIXL_DEFINE_SIMPLE_INSTRUMENT_VISITOR(COUNTER, NAME) \
  void Instrument::Visit##NAME(const Instruction* instr) {   \
    USE(instr);                                              \
    Update();                                                \
    GetCounter(COUNTER)->Increment();                        \
  }

VIXL_SIMPLE_INSTRUMENT_VISITOR_LIST(VIXL_DEFINE_SIMPLE_INSTRUMENT_VISITOR)

#undef VIXL_DEFINE_SIMPLE_INSTRUMENT_VISITOR
#undef VIXL_SIMPLE_INSTRUMENT_VISITOR_LIST

// Other visitors require more analysis.

void Instrument::VisitMoveWideImmediate(const Instruction* instr) {
  Update();
  static Counter* counter = GetCounter("Move Immediate");

  if (instr->IsMovn() && (instr->GetRd() == kZeroRegCode)) {
    unsigned imm = instr->GetImmMoveWide();
    HandleInstrumentationEvent(imm);
  } else {
    counter->Increment();
  }
}

void Instrument::InstrumentLoadStorePair(const Instruction* instr) {
  static Counter* load_pair_counter = GetCounter("Load Pair");
  static Counter* store_pair_counter = GetCounter("Store Pair");

  if (instr->Mask(LoadStorePairLBit) != 0) {
    load_pair_counter->Increment();
  } else {
    store_pair_counter->Increment();
  }
}

void Instrument::VisitLoadStorePairPostIndex(const Instruction* instr) {
  Update();
  InstrumentLoadStorePair(instr);
}

void Instrument::VisitLoadStorePairOffset(const Instruction* instr) {
  Update();
  InstrumentLoadStorePair(instr);
}

void Instrument::VisitLoadStorePairPreIndex(const Instruction* instr) {
  Update();
  InstrumentLoadStorePair(instr);
}

void Instrument::VisitLoadStorePairNonTemporal(const Instruction* instr) {
  Update();
  InstrumentLoadStorePair(instr);
}

void Instrument::InstrumentLoadStore(const Instruction* instr) {
  static Counter* load_int_counter = GetCounter("Load Integer");
  static Counter* store_int_counter = GetCounter("Store Integer");
  static Counter* load_fp_counter = GetCounter("Load FP");
  static Counter* store_fp_counter = GetCounter("Store FP");

  switch (instr->Mask(LoadStoreMask)) {
    case STRB_w:
    case STRH_w:
    case STR_w:
      VIXL_FALLTHROUGH();
    case STR_x:
      store_int_counter->Increment();
      break;
    case STR_s:
      VIXL_FALLTHROUGH();
    case STR_d:
      store_fp_counter->Increment();
      break;
    case LDRB_w:
    case LDRH_w:
    case LDR_w:
    case LDR_x:
    case LDRSB_x:
    case LDRSH_x:
    case LDRSW_x:
    case LDRSB_w:
      VIXL_FALLTHROUGH();
    case LDRSH_w:
      load_int_counter->Increment();
      break;
    case LDR_s:
      VIXL_FALLTHROUGH();
    case LDR_d:
      load_fp_counter->Increment();
      break;
  }
}

void Instrument::VisitLoadStoreUnscaledOffset(const Instruction* instr) {
  Update();
  InstrumentLoadStore(instr);
}

void Instrument::VisitLoadStorePostIndex(const Instruction* instr) {
  USE(instr);
  Update();
  InstrumentLoadStore(instr);
}

void Instrument::VisitLoadStorePreIndex(const Instruction* instr) {
  Update();
  InstrumentLoadStore(instr);
}

void Instrument::VisitLoadStoreRegisterOffset(const Instruction* instr) {
  Update();
  InstrumentLoadStore(instr);
}

void Instrument::VisitLoadStoreRCpcUnscaledOffset(const Instruction* instr) {
  Update();
  switch (instr->Mask(LoadStoreRCpcUnscaledOffsetMask)) {
    case STLURB:
    case STLURH:
    case STLUR_w:
    case STLUR_x: {
      static Counter* counter = GetCounter("Store Integer");
      counter->Increment();
      break;
    }
    case LDAPURB:
    case LDAPURSB_w:
    case LDAPURSB_x:
    case LDAPURH:
    case LDAPURSH_w:
    case LDAPURSH_x:
    case LDAPUR_w:
    case LDAPURSW:
    case LDAPUR_x: {
      static Counter* counter = GetCounter("Load Integer");
      counter->Increment();
      break;
    }
  }
}

void Instrument::VisitLoadStoreUnsignedOffset(const Instruction* instr) {
  Update();
  InstrumentLoadStore(instr);
}

}  // namespace aarch64
}  // namespace vixl
