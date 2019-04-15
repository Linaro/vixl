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

#include <cmath>

#include "test-runner.h"
#include "test-utils-aarch64.h"

#include "aarch64/cpu-aarch64.h"
#include "aarch64/disasm-aarch64.h"
#include "aarch64/macro-assembler-aarch64.h"
#include "aarch64/simulator-aarch64.h"

#define __ masm->

namespace vixl {
namespace aarch64 {


// This value is a signalling NaN as both a double and as a float (taking the
// least-significant word).
const double kFP64SignallingNaN = RawbitsToDouble(UINT64_C(0x7ff000007f800001));
const float kFP32SignallingNaN = RawbitsToFloat(0x7f800001);
const Float16 kFP16SignallingNaN = RawbitsToFloat16(0x7c01);

// A similar value, but as a quiet NaN.
const double kFP64QuietNaN = RawbitsToDouble(UINT64_C(0x7ff800007fc00001));
const float kFP32QuietNaN = RawbitsToFloat(0x7fc00001);
const Float16 kFP16QuietNaN = RawbitsToFloat16(0x7e01);


bool Equal32(uint32_t expected, const RegisterDump*, uint32_t result) {
  if (result != expected) {
    printf("Expected 0x%08" PRIx32 "\t Found 0x%08" PRIx32 "\n",
           expected,
           result);
  }

  return expected == result;
}


bool Equal64(uint64_t reference,
             const RegisterDump*,
             uint64_t result,
             ExpectedResult option) {
  switch (option) {
    case kExpectEqual:
      if (result != reference) {
        printf("Expected 0x%016" PRIx64 "\t Found 0x%016" PRIx64 "\n",
               reference,
               result);
      }
      break;
    case kExpectNotEqual:
      if (result == reference) {
        printf("Expected a result not equal to 0x%016" PRIx64 "\n", reference);
      }
      break;
  }

  return reference == result;
}


bool Equal128(vec128_t expected, const RegisterDump*, vec128_t result) {
  if ((result.h != expected.h) || (result.l != expected.l)) {
    printf("Expected 0x%016" PRIx64 "%016" PRIx64
           "\t "
           "Found 0x%016" PRIx64 "%016" PRIx64 "\n",
           expected.h,
           expected.l,
           result.h,
           result.l);
  }

  return ((expected.h == result.h) && (expected.l == result.l));
}


bool EqualFP16(Float16 expected, const RegisterDump*, Float16 result) {
  uint16_t e_rawbits = Float16ToRawbits(expected);
  uint16_t r_rawbits = Float16ToRawbits(result);
  if (e_rawbits == r_rawbits) {
    return true;
  } else {
    if (IsNaN(expected) || IsZero(expected)) {
      printf("Expected 0x%04" PRIx16 "\t Found 0x%04" PRIx16 "\n",
             e_rawbits,
             r_rawbits);
    } else {
      printf("Expected %.6f (16 bit): (0x%04" PRIx16
             ")\t "
             "Found %.6f (0x%04" PRIx16 ")\n",
             FPToFloat(expected, kIgnoreDefaultNaN),
             e_rawbits,
             FPToFloat(result, kIgnoreDefaultNaN),
             r_rawbits);
    }
    return false;
  }
}


bool EqualFP32(float expected, const RegisterDump*, float result) {
  if (FloatToRawbits(expected) == FloatToRawbits(result)) {
    return true;
  } else {
    if (IsNaN(expected) || (expected == 0.0)) {
      printf("Expected 0x%08" PRIx32 "\t Found 0x%08" PRIx32 "\n",
             FloatToRawbits(expected),
             FloatToRawbits(result));
    } else {
      printf("Expected %.9f (0x%08" PRIx32
             ")\t "
             "Found %.9f (0x%08" PRIx32 ")\n",
             expected,
             FloatToRawbits(expected),
             result,
             FloatToRawbits(result));
    }
    return false;
  }
}


bool EqualFP64(double expected, const RegisterDump*, double result) {
  if (DoubleToRawbits(expected) == DoubleToRawbits(result)) {
    return true;
  }

  if (IsNaN(expected) || (expected == 0.0)) {
    printf("Expected 0x%016" PRIx64 "\t Found 0x%016" PRIx64 "\n",
           DoubleToRawbits(expected),
           DoubleToRawbits(result));
  } else {
    printf("Expected %.17f (0x%016" PRIx64
           ")\t "
           "Found %.17f (0x%016" PRIx64 ")\n",
           expected,
           DoubleToRawbits(expected),
           result,
           DoubleToRawbits(result));
  }
  return false;
}


bool Equal32(uint32_t expected, const RegisterDump* core, const Register& reg) {
  VIXL_ASSERT(reg.Is32Bits());
  // Retrieve the corresponding X register so we can check that the upper part
  // was properly cleared.
  int64_t result_x = core->xreg(reg.GetCode());
  if ((result_x & 0xffffffff00000000) != 0) {
    printf("Expected 0x%08" PRIx32 "\t Found 0x%016" PRIx64 "\n",
           expected,
           result_x);
    return false;
  }
  uint32_t result_w = core->wreg(reg.GetCode());
  return Equal32(expected, core, result_w);
}


bool Equal64(uint64_t reference,
             const RegisterDump* core,
             const Register& reg,
             ExpectedResult option) {
  VIXL_ASSERT(reg.Is64Bits());
  uint64_t result = core->xreg(reg.GetCode());
  return Equal64(reference, core, result, option);
}


bool NotEqual64(uint64_t reference,
                const RegisterDump* core,
                const Register& reg) {
  VIXL_ASSERT(reg.Is64Bits());
  uint64_t result = core->xreg(reg.GetCode());
  return NotEqual64(reference, core, result);
}


bool Equal128(uint64_t expected_h,
              uint64_t expected_l,
              const RegisterDump* core,
              const VRegister& vreg) {
  VIXL_ASSERT(vreg.Is128Bits());
  vec128_t expected = {expected_l, expected_h};
  vec128_t result = core->qreg(vreg.GetCode());
  return Equal128(expected, core, result);
}


bool EqualFP16(Float16 expected,
               const RegisterDump* core,
               const VRegister& fpreg) {
  VIXL_ASSERT(fpreg.Is16Bits());
  // Retrieve the corresponding D register so we can check that the upper part
  // was properly cleared.
  uint64_t result_64 = core->dreg_bits(fpreg.GetCode());
  if ((result_64 & 0xfffffffffff0000) != 0) {
    printf("Expected 0x%04" PRIx16 " (%f)\t Found 0x%016" PRIx64 "\n",
           Float16ToRawbits(expected),
           FPToFloat(expected, kIgnoreDefaultNaN),
           result_64);
    return false;
  }
  return EqualFP16(expected, core, core->hreg(fpreg.GetCode()));
}


bool EqualFP32(float expected,
               const RegisterDump* core,
               const VRegister& fpreg) {
  VIXL_ASSERT(fpreg.Is32Bits());
  // Retrieve the corresponding D register so we can check that the upper part
  // was properly cleared.
  uint64_t result_64 = core->dreg_bits(fpreg.GetCode());
  if ((result_64 & 0xffffffff00000000) != 0) {
    printf("Expected 0x%08" PRIx32 " (%f)\t Found 0x%016" PRIx64 "\n",
           FloatToRawbits(expected),
           expected,
           result_64);
    return false;
  }

  return EqualFP32(expected, core, core->sreg(fpreg.GetCode()));
}


bool EqualFP64(double expected,
               const RegisterDump* core,
               const VRegister& fpreg) {
  VIXL_ASSERT(fpreg.Is64Bits());
  return EqualFP64(expected, core, core->dreg(fpreg.GetCode()));
}


bool Equal64(const Register& reg0,
             const RegisterDump* core,
             const Register& reg1,
             ExpectedResult option) {
  VIXL_ASSERT(reg0.Is64Bits() && reg1.Is64Bits());
  int64_t reference = core->xreg(reg0.GetCode());
  int64_t result = core->xreg(reg1.GetCode());
  return Equal64(reference, core, result, option);
}


bool NotEqual64(const Register& reg0,
                const RegisterDump* core,
                const Register& reg1) {
  VIXL_ASSERT(reg0.Is64Bits() && reg1.Is64Bits());
  int64_t expected = core->xreg(reg0.GetCode());
  int64_t result = core->xreg(reg1.GetCode());
  return NotEqual64(expected, core, result);
}


bool Equal64(uint64_t expected,
             const RegisterDump* core,
             const VRegister& vreg) {
  VIXL_ASSERT(vreg.Is64Bits());
  uint64_t result = core->dreg_bits(vreg.GetCode());
  return Equal64(expected, core, result);
}


static char FlagN(uint32_t flags) { return (flags & NFlag) ? 'N' : 'n'; }


static char FlagZ(uint32_t flags) { return (flags & ZFlag) ? 'Z' : 'z'; }


static char FlagC(uint32_t flags) { return (flags & CFlag) ? 'C' : 'c'; }


static char FlagV(uint32_t flags) { return (flags & VFlag) ? 'V' : 'v'; }


bool EqualNzcv(uint32_t expected, uint32_t result) {
  VIXL_ASSERT((expected & ~NZCVFlag) == 0);
  VIXL_ASSERT((result & ~NZCVFlag) == 0);
  if (result != expected) {
    printf("Expected: %c%c%c%c\t Found: %c%c%c%c\n",
           FlagN(expected),
           FlagZ(expected),
           FlagC(expected),
           FlagV(expected),
           FlagN(result),
           FlagZ(result),
           FlagC(result),
           FlagV(result));
    return false;
  }

  return true;
}


bool EqualRegisters(const RegisterDump* a, const RegisterDump* b) {
  for (unsigned i = 0; i < kNumberOfRegisters; i++) {
    if (a->xreg(i) != b->xreg(i)) {
      printf("x%d\t Expected 0x%016" PRIx64 "\t Found 0x%016" PRIx64 "\n",
             i,
             a->xreg(i),
             b->xreg(i));
      return false;
    }
  }

  for (unsigned i = 0; i < kNumberOfVRegisters; i++) {
    uint64_t a_bits = a->dreg_bits(i);
    uint64_t b_bits = b->dreg_bits(i);
    if (a_bits != b_bits) {
      printf("d%d\t Expected 0x%016" PRIx64 "\t Found 0x%016" PRIx64 "\n",
             i,
             a_bits,
             b_bits);
      return false;
    }
  }

  return true;
}


RegList PopulateRegisterArray(Register* w,
                              Register* x,
                              Register* r,
                              int reg_size,
                              int reg_count,
                              RegList allowed) {
  RegList list = 0;
  int i = 0;
  for (unsigned n = 0; (n < kNumberOfRegisters) && (i < reg_count); n++) {
    if (((UINT64_C(1) << n) & allowed) != 0) {
      // Only assign allowed registers.
      if (r) {
        r[i] = Register(n, reg_size);
      }
      if (x) {
        x[i] = Register(n, kXRegSize);
      }
      if (w) {
        w[i] = Register(n, kWRegSize);
      }
      list |= (UINT64_C(1) << n);
      i++;
    }
  }
  // Check that we got enough registers.
  VIXL_ASSERT(CountSetBits(list, kNumberOfRegisters) == reg_count);

  return list;
}


RegList PopulateVRegisterArray(VRegister* s,
                               VRegister* d,
                               VRegister* v,
                               int reg_size,
                               int reg_count,
                               RegList allowed) {
  RegList list = 0;
  int i = 0;
  for (unsigned n = 0; (n < kNumberOfVRegisters) && (i < reg_count); n++) {
    if (((UINT64_C(1) << n) & allowed) != 0) {
      // Only assigned allowed registers.
      if (v) {
        v[i] = VRegister(n, reg_size);
      }
      if (d) {
        d[i] = VRegister(n, kDRegSize);
      }
      if (s) {
        s[i] = VRegister(n, kSRegSize);
      }
      list |= (UINT64_C(1) << n);
      i++;
    }
  }
  // Check that we got enough registers.
  VIXL_ASSERT(CountSetBits(list, kNumberOfVRegisters) == reg_count);

  return list;
}


void Clobber(MacroAssembler* masm, RegList reg_list, uint64_t const value) {
  Register first = NoReg;
  for (unsigned i = 0; i < kNumberOfRegisters; i++) {
    if (reg_list & (UINT64_C(1) << i)) {
      Register xn(i, kXRegSize);
      // We should never write into sp here.
      VIXL_ASSERT(!xn.Is(sp));
      if (!xn.IsZero()) {
        if (!first.IsValid()) {
          // This is the first register we've hit, so construct the literal.
          __ Mov(xn, value);
          first = xn;
        } else {
          // We've already loaded the literal, so re-use the value already
          // loaded into the first register we hit.
          __ Mov(xn, first);
        }
      }
    }
  }
}


void ClobberFP(MacroAssembler* masm, RegList reg_list, double const value) {
  VRegister first = NoVReg;
  for (unsigned i = 0; i < kNumberOfVRegisters; i++) {
    if (reg_list & (UINT64_C(1) << i)) {
      VRegister dn(i, kDRegSize);
      if (!first.IsValid()) {
        // This is the first register we've hit, so construct the literal.
        __ Fmov(dn, value);
        first = dn;
      } else {
        // We've already loaded the literal, so re-use the value already loaded
        // into the first register we hit.
        __ Fmov(dn, first);
      }
    }
  }
}


void Clobber(MacroAssembler* masm, CPURegList reg_list) {
  if (reg_list.GetType() == CPURegister::kRegister) {
    // This will always clobber X registers.
    Clobber(masm, reg_list.GetList());
  } else if (reg_list.GetType() == CPURegister::kVRegister) {
    // This will always clobber D registers.
    ClobberFP(masm, reg_list.GetList());
  } else {
    VIXL_UNREACHABLE();
  }
}


void RegisterDump::Dump(MacroAssembler* masm) {
  VIXL_ASSERT(__ StackPointer().Is(sp));

  // Ensure that we don't unintentionally clobber any registers.
  UseScratchRegisterScope temps(masm);
  temps.ExcludeAll();

  // Preserve some temporary registers.
  Register dump_base = x0;
  Register dump = x1;
  Register tmp = x2;
  Register dump_base_w = dump_base.W();
  Register dump_w = dump.W();
  Register tmp_w = tmp.W();

  // Offsets into the dump_ structure.
  const int x_offset = offsetof(dump_t, x_);
  const int w_offset = offsetof(dump_t, w_);
  const int d_offset = offsetof(dump_t, d_);
  const int s_offset = offsetof(dump_t, s_);
  const int h_offset = offsetof(dump_t, h_);
  const int q_offset = offsetof(dump_t, q_);
  const int sp_offset = offsetof(dump_t, sp_);
  const int wsp_offset = offsetof(dump_t, wsp_);
  const int flags_offset = offsetof(dump_t, flags_);

  __ Push(xzr, dump_base, dump, tmp);

  // Load the address where we will dump the state.
  __ Mov(dump_base, reinterpret_cast<uintptr_t>(&dump_));

  // Dump the stack pointer (sp and wsp).
  // The stack pointer cannot be stored directly; it needs to be moved into
  // another register first. Also, we pushed four X registers, so we need to
  // compensate here.
  __ Add(tmp, sp, 4 * kXRegSizeInBytes);
  __ Str(tmp, MemOperand(dump_base, sp_offset));
  __ Add(tmp_w, wsp, 4 * kXRegSizeInBytes);
  __ Str(tmp_w, MemOperand(dump_base, wsp_offset));

  // Dump X registers.
  __ Add(dump, dump_base, x_offset);
  for (unsigned i = 0; i < kNumberOfRegisters; i += 2) {
    __ Stp(Register::GetXRegFromCode(i),
           Register::GetXRegFromCode(i + 1),
           MemOperand(dump, i * kXRegSizeInBytes));
  }

  // Dump W registers.
  __ Add(dump, dump_base, w_offset);
  for (unsigned i = 0; i < kNumberOfRegisters; i += 2) {
    __ Stp(Register::GetWRegFromCode(i),
           Register::GetWRegFromCode(i + 1),
           MemOperand(dump, i * kWRegSizeInBytes));
  }

  // Dump D registers.
  __ Add(dump, dump_base, d_offset);
  for (unsigned i = 0; i < kNumberOfVRegisters; i += 2) {
    __ Stp(VRegister::GetDRegFromCode(i),
           VRegister::GetDRegFromCode(i + 1),
           MemOperand(dump, i * kDRegSizeInBytes));
  }

  // Dump S registers.
  __ Add(dump, dump_base, s_offset);
  for (unsigned i = 0; i < kNumberOfVRegisters; i += 2) {
    __ Stp(VRegister::GetSRegFromCode(i),
           VRegister::GetSRegFromCode(i + 1),
           MemOperand(dump, i * kSRegSizeInBytes));
  }

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64
  // Dump H registers. Note: Stp does not support 16 bits.
  __ Add(dump, dump_base, h_offset);
  for (unsigned i = 0; i < kNumberOfVRegisters; i++) {
    __ Str(VRegister::GetHRegFromCode(i),
           MemOperand(dump, i * kHRegSizeInBytes));
  }
#else
  USE(h_offset);
#endif

  // Dump Q registers.
  __ Add(dump, dump_base, q_offset);
  for (unsigned i = 0; i < kNumberOfVRegisters; i += 2) {
    __ Stp(VRegister::GetQRegFromCode(i),
           VRegister::GetQRegFromCode(i + 1),
           MemOperand(dump, i * kQRegSizeInBytes));
  }

  // Dump the flags.
  __ Mrs(tmp, NZCV);
  __ Str(tmp, MemOperand(dump_base, flags_offset));

  // To dump the values that were in tmp amd dump, we need a new scratch
  // register. We can use any of the already dumped registers since we can
  // easily restore them.
  Register dump2_base = x10;
  Register dump2 = x11;
  VIXL_ASSERT(!AreAliased(dump_base, dump, tmp, dump2_base, dump2));

  // Don't lose the dump_ address.
  __ Mov(dump2_base, dump_base);

  __ Pop(tmp, dump, dump_base, xzr);

  __ Add(dump2, dump2_base, w_offset);
  __ Str(dump_base_w,
         MemOperand(dump2, dump_base.GetCode() * kWRegSizeInBytes));
  __ Str(dump_w, MemOperand(dump2, dump.GetCode() * kWRegSizeInBytes));
  __ Str(tmp_w, MemOperand(dump2, tmp.GetCode() * kWRegSizeInBytes));

  __ Add(dump2, dump2_base, x_offset);
  __ Str(dump_base, MemOperand(dump2, dump_base.GetCode() * kXRegSizeInBytes));
  __ Str(dump, MemOperand(dump2, dump.GetCode() * kXRegSizeInBytes));
  __ Str(tmp, MemOperand(dump2, tmp.GetCode() * kXRegSizeInBytes));

  // Finally, restore dump2_base and dump2.
  __ Ldr(dump2_base,
         MemOperand(dump2, dump2_base.GetCode() * kXRegSizeInBytes));
  __ Ldr(dump2, MemOperand(dump2, dump2.GetCode() * kXRegSizeInBytes));

  completed_ = true;
}

}  // namespace aarch64
}  // namespace vixl
