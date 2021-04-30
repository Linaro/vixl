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

#if __cplusplus < 201703L
#error "Text Assembler requires C++17 standard or above."
#else

#include "tasm-assembler.h"
#include "assembler-base-vixl.h"
#include "tasm-instructions.h"

namespace vixl {
namespace aarch64 {
namespace tasm {

// Use lambda expression in std::vector constructor to initialize it with keys
// extracted from 'prototypes_' map.
const std::vector<std::string> TextAssembler::str_prototypes_([]() {
  std::vector<std::string> keys;
  for (const auto& prototype : prototypes_) keys.push_back(prototype.first);

  return keys;
}());

void TextAssembler::Assemble(std::string instruction) {
  AssemblerFn fun_object;
  std::string prototype;
  std::string mnemonic;
  std::vector<Argument> args;
  LineType line_type;

  line_type = ip_.LoadInstruction(instruction, &prototype);
  mnemonic = ip_.GetMnemonic();
  args = ip_.GetArgs();

  // There are few cases where VIXL expects a raw (as-encoded) immediate.
  if (mnemonic.compare("adrp") == 0 && prototype.compare("ri64") == 0) {
    args[1] = std::get<int64_t>(args[1]) / 4096;
  } else if (mnemonic.compare("cbnz") == 0 && prototype.compare("ri64") == 0) {
    args[1] = std::get<int64_t>(args[1]) / 4;
  } else if (mnemonic.compare("cbz") == 0 && prototype.compare("ri64") == 0) {
    args[1] = std::get<int64_t>(args[1]) / 4;
  } else if (mnemonic.compare("ldrsw") == 0 && prototype.compare("ri64") == 0) {
    args[1] = std::get<int64_t>(args[1]) / 4;
  } else if (mnemonic.compare("ldr") == 0 &&
             std::regex_match(prototype, std::regex("[rv]i64"))) {
    args[1] = std::get<int64_t>(args[1]) / 4;
  } else if (mnemonic.compare("b") == 0 &&
             std::regex_match(prototype, std::regex("i64c?"))) {
    args[0] = std::get<int64_t>(args[0]) / 4;
  }

  // Don't try to assemble instruction line if there was an error in this line.
  if (eh_.GetLineErr()) return;

  if (line_type == kInstruction) {
    try {
      fun_object = prototypes_.at(prototype).at(mnemonic);
      std::visit(InstructionDispatcher(this, args, mnemonic), fun_object);
    } catch (std::out_of_range) {
      eh_.Err("Instruction with given mnemonic and arguments doesn't exist.");
    } catch (std::bad_variant_access) {
      eh_.Err("Wrong arguments for the given instruction.");
    } catch (std::runtime_error& ex) {
      eh_.Err("Wrong instruction semantics. Internal error message: " +
              std::string(ex.what()));
    }
  } else if (line_type == kUnmatched) {
    eh_.Err("Wrong input line format");
  }
}

bool TextAssembler::MnemonicExists(std::string prototype,
                                   std::string mnemonic) {
  size_t prototype_map = prototypes_.count(prototype);

  if (prototype_map == 0) return prototype_map;

  return prototypes_.at(prototype).count(mnemonic);
}

std::vector<std::string> TextAssembler::GetPrototypes() {
  return str_prototypes_;
}

bool TextAssembler::GetError() { return eh_.GetErr(); }
bool TextAssembler::GetLineError() { return eh_.GetLineErr(); }

vixl::internal::AssemblerBase* TextAssembler::AsAssemblerBase() { return this; }
}
}
}  // namespace vixl::aarch64::tasm

#endif  // __cplusplus >= 201703L