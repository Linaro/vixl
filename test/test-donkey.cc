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

#include <regex>
#include <set>

#include "aarch64/test-utils-aarch64.h"

using namespace vixl;
using namespace vixl::aarch64;

#define __ masm->

class InstructionReporter : public DecoderVisitor {
 public:
  InstructionReporter() : DecoderVisitor(kNonConstVisitor) {}

  void Visit(Metadata *metadata, const Instruction *instr) VIXL_OVERRIDE {
    USE(instr);
    instr_form_ = (*metadata)["form"];
  }

  std::string MoveForm() { return std::move(instr_form_); }

 private:
  std::string instr_form_;
};

Instr Mutate(Instr base) {
  Instr result = base;
  while ((result == base) || (result == 0)) {
    // Flip two bits somewhere in the most-significant 27.
    for (int i = 0; i < 2; i++) {
      uint32_t pos = 5 + ((lrand48() >> 20) % 27);
      result = result ^ (1 << pos);
    }

    // Always flip one of the low five bits, as that's where the destination
    // register is often encoded.
    uint32_t dst_pos = (lrand48() >> 20) % 5;
    result = result ^ (1 << dst_pos);
  }
  return result;
}

#ifndef VIXL_INCLUDE_SIMULATOR_AARCH64
int main(void) {
  printf("Test donkey requires a simulator build to be useful.\n");
  return 0;
}
#else
int main(int argc, char **argv) {
  if ((argc < 3) || (argc > 5)) {
    printf(
        "Usage: test-donkey <instruction form regex> <number of instructions "
        "to emit in test> <encoding generation manner> <input data type>\n"
        "  regex - ECMAScript (C++11) regular expression to match instruction "
        "form\n"
        "  encoding=random - use rng only to select new instructions\n"
        "    (can take longer, but gives better coverage for disparate "
        "encodings)\n"
        "  encoding=`initial hex` - hex encoding of first instruction in test, "
        "eg. 1234abcd\n"
        "  input data type - used to specify the data type of generating "
        "input, e.g. input=fp, default set to integer type\n"
        "  command examples :\n"
        "  ./test-donkey \"fml[as]l[bt]\" 50 encoding=random input=fp\n"
        "  ./test-donkey \"fml[as]l[bt]\" 30 input=int\n");
    exit(1);
  }

  // Use LC-RNG only to select instructions.
  bool random_only = false;

  std::string target_re = argv[1];
  uint32_t count = static_cast<uint32_t>(strtoul(argv[2], NULL, 10));
  uint32_t cmdline_encoding = 0;
  InputSet input_set = kIntInputSet;
  if (argc > 3) {
    // The arguments of instruction pattern and the number of generating
    // instructions are processed.
    int32_t i = 3;
    std::string argv_s(argv[i]);
    if (argv_s.find("encoding=") != std::string::npos) {
      char *c = argv[i];
      c += 9;
      if (strcmp(c, "random") == 0) {
        random_only = true;
      } else {
        cmdline_encoding = static_cast<uint32_t>(strtoul(c, NULL, 16));
      }
      i++;
    }

    if ((argc > 4) || (i == 3)) {
      argv_s = std::string(argv[i]);
      if (argv_s.find("input=") != std::string::npos) {
        char *c = argv[i];
        c += 6;
        if (strcmp(c, "fp") == 0) {
          input_set = kFpInputSet;
        } else {
          VIXL_ASSERT(strcmp(c, "int") == 0);
        }
        i++;
      }
    }

    // Ensure all arguments have been processed.
    VIXL_ASSERT(argc == i);
  }

  srand48(42);

  MacroAssembler masm;
  masm.GetCPUFeatures()->Combine(CPUFeatures::kSVE);

  std::map<int, Simulator *> sim_vl;
  for (int i = 128; i <= 2048; i += 128) {
    sim_vl[i] = new Simulator(new Decoder());
    sim_vl[i]->SetVectorLengthInBits(i);
  }

  char buffer[256];
  Decoder trial_decoder;
  Disassembler disasm(buffer, sizeof(buffer));
  InstructionReporter reporter;
  trial_decoder.AppendVisitor(&reporter);
  trial_decoder.AppendVisitor(&disasm);

  using InstrData = struct {
    Instr inst;
    std::string disasm;
    uint32_t state_hash;
  };
  std::vector<InstrData> useful_insts;

  // Seen states are only considered for vl128. It's assumed that a new state
  // for vl128 implies a new state for all other vls.
  std::set<uint32_t> seen_states;
  uint32_t state_hash;

  std::map<int, uint32_t> initial_state_vl;
  std::map<int, uint32_t> state_hash_vl;

  // Compute hash of the initial state of the machine.
  Label test;
  masm.Bind(&test);
  masm.PushCalleeSavedRegisters();
  SetInitialMachineState(&masm, input_set);
  ComputeMachineStateHash(&masm, &state_hash);
  masm.PopCalleeSavedRegisters();
  masm.Ret();
  masm.FinalizeCode();
  masm.GetBuffer()->SetExecutable();

  for (std::pair<int, Simulator *> s : sim_vl) {
    s.second->RunFrom(masm.GetLabelAddress<Instruction *>(&test));
    initial_state_vl[s.first] = state_hash;
    if (s.first == 128) seen_states.insert(state_hash);
  }

  masm.GetBuffer()->SetWritable();
  masm.Reset();

  // Count number of failed instructions, in order to allow changing instruction
  // candidate strategy.
  int miss_count = 0;

  while (useful_insts.size() < count) {
    miss_count++;

    Instr inst;
    if (cmdline_encoding != 0) {
      // Initial instruction encoding supplied on the command line.
      inst = cmdline_encoding;
      cmdline_encoding = 0;
    } else if (useful_insts.empty() || random_only || (miss_count > 10000)) {
      // LCG-random instruction.
      inst = static_cast<Instr>(mrand48());
    } else {
      // Instruction based on mutation of last successful instruction.
      inst = Mutate(useful_insts.back().inst);
    }

    trial_decoder.Decode(reinterpret_cast<Instruction *>(&inst));
    if (std::regex_search(reporter.MoveForm(), std::regex(target_re))) {
      // Disallow "unimplemented" instructions.
      std::string buffer_s(buffer);
      if (buffer_s.find("unimplemented") != std::string::npos) continue;

      // Disallow instructions with "sp" in their arguments, as we don't support
      // instructions operating on memory, and the OS expects sp to be valid for
      // signal handlers, etc.
      size_t space = buffer_s.find(' ');
      if ((space != std::string::npos) &&
          (buffer_s.substr(space).find("sp") != std::string::npos))
        continue;

      fprintf(stderr, "Trying 0x%08x (%s)\n", inst, buffer);

      // TODO: factorise this code into a CalculateState helper function.

      // Initialise the machine to a known state.
      masm.PushCalleeSavedRegisters();
      SetInitialMachineState(&masm, input_set);

      {
        ExactAssemblyScope scope(&masm,
                                 (useful_insts.size() + 1) * kInstructionSize);

        // Emit any instructions already found to move the state to somewhere
        // new.
        for (const InstrData &i : useful_insts) {
          masm.dci(i.inst);
        }

        // Try a new instruction.
        masm.dci(inst);
      }

      // Compute the new state of the machine.
      ComputeMachineStateHash(&masm, &state_hash);
      masm.PopCalleeSavedRegisters();
      masm.Ret();
      masm.FinalizeCode();
      masm.GetBuffer()->SetExecutable();

      // Try the new instruction for VL128.
      sim_vl[128]->RunFrom(masm.GetLabelAddress<Instruction *>(&test));
      state_hash_vl[128] = state_hash;

      if (seen_states.count(state_hash_vl[128]) == 0) {
        // A new state! Run for all VLs, record it, add the instruction to the
        // list of useful ones.

        for (std::pair<int, Simulator *> s : sim_vl) {
          if (s.first == 128) continue;
          s.second->RunFrom(masm.GetLabelAddress<Instruction *>(&test));
          state_hash_vl[s.first] = state_hash;
        }

        seen_states.insert(state_hash_vl[128]);
        useful_insts.push_back({inst, buffer, state_hash_vl[128]});
        miss_count = 0;
      } else {
        // Machine already reached here. Probably not an interesting
        // instruction. NB. it's possible for an instruction to reach the same
        // machine state as two or more others, but for these purposes, let's
        // call that not useful.
        fprintf(stderr,
                "Already reached state 0x%08x, skipping 0x%08x, miss_count "
                "%d\n",
                state_hash_vl[128],
                inst,
                miss_count);
      }

      // Restart generation.
      masm.GetBuffer()->SetWritable();
      masm.Reset();
    }
  }

  // Emit test case based on identified instructions and associated hashes.
  printf("TEST_SVE(sve2_%s) {\n", target_re.c_str());
  printf(
      "  SVE_SETUP_WITH_FEATURES(CPUFeatures::kSVE, CPUFeatures::kSVE2, "
      "CPUFeatures::kNEON, "
      "CPUFeatures::kCRC32);\n");
  printf("  START();\n\n");
  printf((input_set == kFpInputSet)
             ? "  SetInitialMachineState(&masm, kFpInputSet);\n"
             : "  SetInitialMachineState(&masm);\n");
  printf("  // state = 0x%08x\n\n", initial_state_vl[128]);

  printf("  {\n");
  printf("    ExactAssemblyScope scope(&masm, %lu * kInstructionSize);\n",
         useful_insts.size());
  for (InstrData &i : useful_insts) {
    printf("    __ dci(0x%08x);  // %s\n", i.inst, i.disasm.c_str());
    printf("    // vl128 state = 0x%08x\n", i.state_hash);
  }
  printf("  }\n\n");
  printf("  uint32_t state;\n");
  printf("  ComputeMachineStateHash(&masm, &state);\n");
  printf("  __ Mov(x0, reinterpret_cast<uint64_t>(&state));\n");
  printf("  __ Ldr(w0, MemOperand(x0));\n\n");
  printf("  END();\n");
  printf("  if (CAN_RUN()) {\n");
  printf("    RUN();\n");
  printf("    uint32_t expected_hashes[] = {\n");
  for (std::pair<int, uint32_t> h : state_hash_vl) {
    printf("    0x%08x,\n", h.second);
  }
  printf("    };\n");
  printf(
      "    ASSERT_EQUAL_64(expected_hashes[core.GetSVELaneCount(kQRegSize) - "
      "1], x0);\n");
  printf("  }\n}\n");

  return 0;
}
#endif
