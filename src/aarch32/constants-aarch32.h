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

#ifndef VIXL_CONSTANTS_AARCH32_H_
#define VIXL_CONSTANTS_AARCH32_H_

namespace vixl {
namespace aarch32 {

const unsigned kRegSizeInBits = 32;
const unsigned kRegSizeInBytes = kRegSizeInBits / 8;
const unsigned kSRegSizeInBits = 32;
const unsigned kSRegSizeInBytes = kSRegSizeInBits / 8;
const unsigned kDRegSizeInBits = 64;
const unsigned kDRegSizeInBytes = kDRegSizeInBits / 8;
const unsigned kQRegSizeInBits = 128;
const unsigned kQRegSizeInBytes = kQRegSizeInBits / 8;

const unsigned kNumberOfRegisters = 16;
const unsigned kNumberOfSRegisters = 32;
const unsigned kMaxNumberOfDRegisters = 32;
const unsigned kNumberOfQRegisters = 16;
const unsigned kNumberOfT32LowRegisters = 8;

const unsigned kIpCode = 12;
const unsigned kSpCode = 13;
const unsigned kLrCode = 14;
const unsigned kPcCode = 15;

const unsigned kT32PcDelta = 4;
const unsigned kA32PcDelta = 8;

const unsigned kRRXEncodedValue = 3;

const unsigned kCoprocMask = 0xe;
const unsigned kInvalidCoprocMask = 0xa;

const unsigned kLowestT32_32Opcode = 0xe8000000;

const uint32_t kUnknownValue = 0xdeadbeef;

const uint32_t kMaxInstructionSizeInBytes = 4;
const uint32_t kA32InstructionSizeInBytes = 4;
const uint32_t k32BitT32InstructionSizeInBytes = 4;
const uint32_t k16BitT32InstructionSizeInBytes = 2;

// Maximum size emitted by a single T32 unconditional macro-instruction.
const uint32_t kMaxT32MacroInstructionSizeInBytes = 32;

const uint32_t kCallerSavedRegistersMask = 0x500f;

enum SystemFunctionsOpcodes { kPrintfCode };

}  // namespace aarch32
}  // namespace vixl

#endif  // VIXL_CONSTANTS_AARCH32_H_
