VIXL Supported Instruction List
===============================

This is a list of the AArch64 instructions supported by the VIXL assembler,
disassembler and simulator. The simulator may not support all floating point
operations to the precision required by AArch64 - please check the simulator
source code for details.

AArch64 integer instructions
----------------------------

### adc ###

Add with carry bit.

    void adc(const Register& rd,
             const Register& rn,
             const Operand& operand)


### adcs ###

Add with carry bit and update status flags.

    void adcs(const Register& rd,
              const Register& rn,
              const Operand& operand)


### add ###

Add.

    void add(const Register& rd,
             const Register& rn,
             const Operand& operand)


### adds ###

Add and update status flags.

    void adds(const Register& rd,
              const Register& rn,
              const Operand& operand)


### adr ###

Calculate the address of a PC offset.

    void adr(const Register& rd, int imm21)


### adr ###

Calculate the address of a label.

    void adr(const Register& rd, Label* label)


### and ###

Bitwise and (A & B).

    void and_(const Register& rd,
              const Register& rn,
              const Operand& operand)


### ands ###

Bitwise and (A & B) and update status flags.

    void ands(const Register& rd,
              const Register& rn,
              const Operand& operand)


### asr ###

Arithmetic shift right.

    inline void asr(const Register& rd, const Register& rn, unsigned shift)


### asrv ###

Arithmetic shift right by variable.

    void asrv(const Register& rd, const Register& rn, const Register& rm)


### b ###

Conditional branch to PC offset.

    void b(int imm19, Condition cond)


### b ###

Conditional branch to label.

    void b(Label* label, Condition cond)


### b ###

Unconditional branch to PC offset.

    void b(int imm26)


### b ###

Unconditional branch to label.

    void b(Label* label)


### bfi ###

Bitfield insert.

    inline void bfi(const Register& rd,
                    const Register& rn,
                    unsigned lsb,
                    unsigned width)


### bfm ###

Bitfield move.

    void bfm(const Register& rd,
             const Register& rn,
             unsigned immr,
             unsigned imms)


### bfxil ###

Bitfield extract and insert low.

    inline void bfxil(const Register& rd,
                      const Register& rn,
                      unsigned lsb,
                      unsigned width)


### bic ###

Bit clear (A & ~B).

    void bic(const Register& rd,
             const Register& rn,
             const Operand& operand)


### bics ###

Bit clear (A & ~B) and update status flags.

    void bics(const Register& rd,
              const Register& rn,
              const Operand& operand)


### bl ###

Branch with link to PC offset.

    void bl(int imm26)


### bl ###

Branch with link to label.

    void bl(Label* label)


### blr ###

Branch with link to register.

    void blr(const Register& xn)


### br ###

Branch to register.

    void br(const Register& xn)


### brk ###

Monitor debug-mode breakpoint.

    void brk(int code)


### cbnz ###

Compare and branch to PC offset if not zero.

    void cbnz(const Register& rt, int imm19)


### cbnz ###

Compare and branch to label if not zero.

    void cbnz(const Register& rt, Label* label)


### cbz ###

Compare and branch to PC offset if zero.

    void cbz(const Register& rt, int imm19)


### cbz ###

Compare and branch to label if zero.

    void cbz(const Register& rt, Label* label)


### ccmn ###

Conditional compare negative.

    void ccmn(const Register& rn,
              const Operand& operand,
              StatusFlags nzcv,
              Condition cond)


### ccmp ###

Conditional compare.

    void ccmp(const Register& rn,
              const Operand& operand,
              StatusFlags nzcv,
              Condition cond)


### cinc ###

Conditional increment: rd = cond ? rn + 1 : rn.

    void cinc(const Register& rd, const Register& rn, Condition cond)


### cinv ###

Conditional invert: rd = cond ? ~rn : rn.

    void cinv(const Register& rd, const Register& rn, Condition cond)


### cls ###

Count leading sign bits.

    void cls(const Register& rd, const Register& rn)


### clz ###

Count leading zeroes.

    void clz(const Register& rd, const Register& rn)


### cmn ###

Compare negative.

    void cmn(const Register& rn, const Operand& operand)


### cmp ###

Compare.

    void cmp(const Register& rn, const Operand& operand)


### cneg ###

Conditional negate: rd = cond ? -rn : rn.

    void cneg(const Register& rd, const Register& rn, Condition cond)


### csel ###

Conditional select: rd = cond ? rn : rm.

    void csel(const Register& rd,
              const Register& rn,
              const Register& rm,
              Condition cond)


### cset ###

Conditional set: rd = cond ? 1 : 0.

    void cset(const Register& rd, Condition cond)


### csetm ###

Conditional set mask: rd = cond ? -1 : 0.

    void csetm(const Register& rd, Condition cond)


### csinc ###

Conditional select increment: rd = cond ? rn : rm + 1.

    void csinc(const Register& rd,
               const Register& rn,
               const Register& rm,
               Condition cond)


### csinv ###

Conditional select inversion: rd = cond ? rn : ~rm.

    void csinv(const Register& rd,
               const Register& rn,
               const Register& rm,
               Condition cond)


### csneg ###

Conditional select negation: rd = cond ? rn : -rm.

    void csneg(const Register& rd,
               const Register& rn,
               const Register& rm,
               Condition cond)


### dmb ###

Data memory barrier.

    void dmb(BarrierDomain domain, BarrierType type)


### dsb ###

Data synchronization barrier.

    void dsb(BarrierDomain domain, BarrierType type)


### eon ###

Bitwise enor/xnor (A ^ ~B).

    void eon(const Register& rd, const Register& rn, const Operand& operand)


### eor ###

Bitwise eor/xor (A ^ B).

    void eor(const Register& rd, const Register& rn, const Operand& operand)


### extr ###

Extract.

    void extr(const Register& rd,
              const Register& rn,
              const Register& rm,
              unsigned lsb)


### hint ###

System hint.

    void hint(SystemHint code)


### hlt ###

Halting debug-mode breakpoint.

    void hlt(int code)


### isb ###

Instruction synchronization barrier.

    void isb()


### ldnp ###

Load integer or FP register pair, non-temporal.

    void ldnp(const CPURegister& rt, const CPURegister& rt2,
              const MemOperand& src)


### ldp ###

Load integer or FP register pair.

    void ldp(const CPURegister& rt, const CPURegister& rt2,
             const MemOperand& src)


### ldpsw ###

Load word pair with sign extension.

    void ldpsw(const Register& rt, const Register& rt2, const MemOperand& src)


### ldr ###

Load double precision floating point literal to FP register.

    void ldr(const FPRegister& ft, double imm)


### ldr ###

Load integer or FP register.

    void ldr(const CPURegister& rt, const MemOperand& src)


### ldr ###

Load literal to register.

    void ldr(const Register& rt, uint64_t imm)


### ldr ###

Load single precision floating point literal to FP register.

    void ldr(const FPRegister& ft, float imm)


### ldrb ###

Load byte.

    void ldrb(const Register& rt, const MemOperand& src)


### ldrh ###

Load half-word.

    void ldrh(const Register& rt, const MemOperand& src)


### ldrsb ###

Load byte with sign extension.

    void ldrsb(const Register& rt, const MemOperand& src)


### ldrsh ###

Load half-word with sign extension.

    void ldrsh(const Register& rt, const MemOperand& src)


### ldrsw ###

Load word with sign extension.

    void ldrsw(const Register& rt, const MemOperand& src)


### lsl ###

Logical shift left.

    inline void lsl(const Register& rd, const Register& rn, unsigned shift)


### lslv ###

Logical shift left by variable.

    void lslv(const Register& rd, const Register& rn, const Register& rm)


### lsr ###

Logical shift right.

    inline void lsr(const Register& rd, const Register& rn, unsigned shift)


### lsrv ###

Logical shift right by variable.

    void lsrv(const Register& rd, const Register& rn, const Register& rm)


### madd ###

Multiply and accumulate.

    void madd(const Register& rd,
              const Register& rn,
              const Register& rm,
              const Register& ra)


### mneg ###

Negated multiply.

    void mneg(const Register& rd, const Register& rn, const Register& rm)


### mov ###

Move register to register.

    void mov(const Register& rd, const Register& rn)


### movk ###

Move immediate and keep.

    void movk(const Register& rd, uint64_t imm, int shift = -1)


### movn ###

Move inverted immediate.

    void movn(const Register& rd, uint64_t imm, int shift = -1)


### movz ###

Move immediate.

    void movz(const Register& rd, uint64_t imm, int shift = -1)


### mrs ###

Move to register from system register.

    void mrs(const Register& rt, SystemRegister sysreg)


### msr ###

Move from register to system register.

    void msr(SystemRegister sysreg, const Register& rt)


### msub ###

Multiply and subtract.

    void msub(const Register& rd,
              const Register& rn,
              const Register& rm,
              const Register& ra)


### mul ###

Multiply.

    void mul(const Register& rd, const Register& rn, const Register& rm)


### mvn ###

Move inverted operand to register.

    void mvn(const Register& rd, const Operand& operand)


### neg ###

Negate.

    void neg(const Register& rd,
             const Operand& operand)


### negs ###

Negate and update status flags.

    void negs(const Register& rd,
              const Operand& operand)


### ngc ###

Negate with carry bit.

    void ngc(const Register& rd,
             const Operand& operand)


### ngcs ###

Negate with carry bit and update status flags.

    void ngcs(const Register& rd,
              const Operand& operand)


### nop ###

No-op.

    void nop()


### orn ###

Bitwise nor (A | ~B).

    void orn(const Register& rd, const Register& rn, const Operand& operand)


### orr ###

Bitwise or (A | B).

    void orr(const Register& rd, const Register& rn, const Operand& operand)


### rbit ###

Bit reverse.

    void rbit(const Register& rd, const Register& rn)


### ret ###

Branch to register with return hint.

    void ret(const Register& xn = lr)


### rev ###

Reverse bytes.

    void rev(const Register& rd, const Register& rn)


### rev16 ###

Reverse bytes in 16-bit half words.

    void rev16(const Register& rd, const Register& rn)


### rev32 ###

Reverse bytes in 32-bit words.

    void rev32(const Register& rd, const Register& rn)


### ror ###

Rotate right.

    inline void ror(const Register& rd, const Register& rs, unsigned shift)


### rorv ###

Rotate right by variable.

    void rorv(const Register& rd, const Register& rn, const Register& rm)


### sbc ###

Subtract with carry bit.

    void sbc(const Register& rd,
             const Register& rn,
             const Operand& operand)


### sbcs ###

Subtract with carry bit and update status flags.

    void sbcs(const Register& rd,
              const Register& rn,
              const Operand& operand)


### sbfiz ###

Signed bitfield insert with zero at right.

    inline void sbfiz(const Register& rd,
                      const Register& rn,
                      unsigned lsb,
                      unsigned width)


### sbfm ###

Signed bitfield move.

    void sbfm(const Register& rd,
              const Register& rn,
              unsigned immr,
              unsigned imms)


### sbfx ###

Signed bitfield extract.

    inline void sbfx(const Register& rd,
                     const Register& rn,
                     unsigned lsb,
                     unsigned width)


### scvtf ###

Convert signed integer or fixed point to FP.

    void scvtf(const FPRegister& fd, const Register& rn, unsigned fbits = 0)


### sdiv ###

Signed integer divide.

    void sdiv(const Register& rd, const Register& rn, const Register& rm)


### smaddl ###

Signed long multiply and accumulate: 32 x 32 + 64 -> 64-bit.

    void smaddl(const Register& rd,
                const Register& rn,
                const Register& rm,
                const Register& ra)


### smsubl ###

Signed long multiply and subtract: 64 - (32 x 32) -> 64-bit.

    void smsubl(const Register& rd,
                const Register& rn,
                const Register& rm,
                const Register& ra)


### smulh ###

Signed multiply high: 64 x 64 -> 64-bit <127:64>.

    void smulh(const Register& xd, const Register& xn, const Register& xm)


### smull ###

Signed long multiply: 32 x 32 -> 64-bit.

    void smull(const Register& rd, const Register& rn, const Register& rm)


### stnp ###

Store integer or FP register pair, non-temporal.

    void stnp(const CPURegister& rt, const CPURegister& rt2,
              const MemOperand& dst)


### stp ###

Store integer or FP register pair.

    void stp(const CPURegister& rt, const CPURegister& rt2,
             const MemOperand& dst)


### str ###

Store integer or FP register.

    void str(const CPURegister& rt, const MemOperand& dst)


### strb ###

Store byte.

    void strb(const Register& rt, const MemOperand& dst)


### strh ###

Store half-word.

    void strh(const Register& rt, const MemOperand& dst)


### sub ###

Subtract.

    void sub(const Register& rd,
             const Register& rn,
             const Operand& operand)


### subs ###

Subtract and update status flags.

    void subs(const Register& rd,
              const Register& rn,
              const Operand& operand)


### sxtb ###

Signed extend byte.

    inline void sxtb(const Register& rd, const Register& rn)


### sxth ###

Signed extend halfword.

    inline void sxth(const Register& rd, const Register& rn)


### sxtw ###

Signed extend word.

    inline void sxtw(const Register& rd, const Register& rn)


### tbnz ###

Test bit and branch to PC offset if not zero.

    void tbnz(const Register& rt, unsigned bit_pos, int imm14)


### tbnz ###

Test bit and branch to label if not zero.

    void tbnz(const Register& rt, unsigned bit_pos, Label* label)


### tbz ###

Test bit and branch to PC offset if zero.

    void tbz(const Register& rt, unsigned bit_pos, int imm14)


### tbz ###

Test bit and branch to label if zero.

    void tbz(const Register& rt, unsigned bit_pos, Label* label)


### tst ###

Bit test and set flags.

    void tst(const Register& rn, const Operand& operand)


### ubfiz ###

Unsigned bitfield insert with zero at right.

    inline void ubfiz(const Register& rd,
                      const Register& rn,
                      unsigned lsb,
                      unsigned width)


### ubfm ###

Unsigned bitfield move.

    void ubfm(const Register& rd,
              const Register& rn,
              unsigned immr,
              unsigned imms)


### ubfx ###

Unsigned bitfield extract.

    inline void ubfx(const Register& rd,
                     const Register& rn,
                     unsigned lsb,
                     unsigned width)


### ucvtf ###

Convert unsigned integer or fixed point to FP.

    void ucvtf(const FPRegister& fd, const Register& rn, unsigned fbits = 0)


### udiv ###

Unsigned integer divide.

    void udiv(const Register& rd, const Register& rn, const Register& rm)


### umaddl ###

Unsigned long multiply and accumulate: 32 x 32 + 64 -> 64-bit.

    void umaddl(const Register& rd,
                const Register& rn,
                const Register& rm,
                const Register& ra)


### umsubl ###

Unsigned long multiply and subtract: 64 - (32 x 32) -> 64-bit.

    void umsubl(const Register& rd,
                const Register& rn,
                const Register& rm,
                const Register& ra)


### uxtb ###

Unsigned extend byte.

    inline void uxtb(const Register& rd, const Register& rn)


### uxth ###

Unsigned extend halfword.

    inline void uxth(const Register& rd, const Register& rn)


### uxtw ###

Unsigned extend word.

    inline void uxtw(const Register& rd, const Register& rn)



AArch64 floating point instructions
-----------------------------------

### fabs ###

FP absolute.

    void fabs(const FPRegister& fd, const FPRegister& fn)


### fadd ###

FP add.

    void fadd(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm)


### fccmp ###

FP conditional compare.

    void fccmp(const FPRegister& fn,
               const FPRegister& fm,
               StatusFlags nzcv,
               Condition cond)


### fcmp ###

FP compare immediate.

    void fcmp(const FPRegister& fn, double value)


### fcmp ###

FP compare registers.

    void fcmp(const FPRegister& fn, const FPRegister& fm)


### fcsel ###

FP conditional select.

    void fcsel(const FPRegister& fd,
               const FPRegister& fn,
               const FPRegister& fm,
               Condition cond)


### fcvt ###

FP convert between single and double precision.

    void fcvt(const FPRegister& fd, const FPRegister& fn)


### fcvtas ###

Convert FP to signed integer (nearest with ties to away).

    void fcvtas(const Register& rd, const FPRegister& fn)


### fcvtau ###

Convert FP to unsigned integer (nearest with ties to away).

    void fcvtau(const Register& rd, const FPRegister& fn)


### fcvtms ###

Convert FP to signed integer (round towards -infinity).

    void fcvtms(const Register& rd, const FPRegister& fn)


### fcvtmu ###

Convert FP to unsigned integer (round towards -infinity).

    void fcvtmu(const Register& rd, const FPRegister& fn)


### fcvtns ###

Convert FP to signed integer (nearest with ties to even).

    void fcvtns(const Register& rd, const FPRegister& fn)


### fcvtnu ###

Convert FP to unsigned integer (nearest with ties to even).

    void fcvtnu(const Register& rd, const FPRegister& fn)


### fcvtzs ###

Convert FP to signed integer (round towards zero).

    void fcvtzs(const Register& rd, const FPRegister& fn)


### fcvtzu ###

Convert FP to unsigned integer (round towards zero).

    void fcvtzu(const Register& rd, const FPRegister& fn)


### fdiv ###

FP divide.

    void fdiv(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm)


### fmadd ###

FP fused multiply and add.

    void fmadd(const FPRegister& fd,
               const FPRegister& fn,
               const FPRegister& fm,
               const FPRegister& fa)


### fmax ###

FP maximum.

    void fmax(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm)


### fmaxnm ###

FP maximum number.

    void fmaxnm(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm)


### fmin ###

FP minimum.

    void fmin(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm)


### fminnm ###

FP minimum number.

    void fminnm(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm)


### fmov ###

Move FP register to FP register.

    void fmov(const FPRegister& fd, const FPRegister& fn)


### fmov ###

Move FP register to register.

    void fmov(const Register& rd, const FPRegister& fn)


### fmov ###

Move double precision immediate to FP register.

    void fmov(const FPRegister& fd, double imm)


### fmov ###

Move register to FP register.

    void fmov(const FPRegister& fd, const Register& rn)


### fmov ###

Move single precision immediate to FP register.

    void fmov(const FPRegister& fd, float imm)


### fmsub ###

FP fused multiply and subtract.

    void fmsub(const FPRegister& fd,
               const FPRegister& fn,
               const FPRegister& fm,
               const FPRegister& fa)


### fmul ###

FP multiply.

    void fmul(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm)


### fneg ###

FP negate.

    void fneg(const FPRegister& fd, const FPRegister& fn)


### fnmadd ###

FP fused multiply, add and negate.

    void fnmadd(const FPRegister& fd,
                const FPRegister& fn,
                const FPRegister& fm,
                const FPRegister& fa)


### fnmsub ###

FP fused multiply, subtract and negate.

    void fnmsub(const FPRegister& fd,
                const FPRegister& fn,
                const FPRegister& fm,
                const FPRegister& fa)


### frinta ###

FP round to integer (nearest with ties to away).

    void frinta(const FPRegister& fd, const FPRegister& fn)


### frintm ###

FP round to integer (toward minus infinity).

    void frintm(const FPRegister& fd, const FPRegister& fn)


### frintn ###

FP round to integer (nearest with ties to even).

    void frintn(const FPRegister& fd, const FPRegister& fn)


### frintz ###

FP round to integer (towards zero).

    void frintz(const FPRegister& fd, const FPRegister& fn)


### fsqrt ###

FP square root.

    void fsqrt(const FPRegister& fd, const FPRegister& fn)


### fsub ###

FP subtract.

    void fsub(const FPRegister& fd, const FPRegister& fn, const FPRegister& fm)



Additional or pseudo instructions
---------------------------------

### bind ###

Bind a label to the current PC.

    void bind(Label* label)


### dc32 ###

Emit 32 bits of data into the instruction stream.

    inline void dc32(uint32_t data)


### dc64 ###

Emit 64 bits of data into the instruction stream.

    inline void dc64(uint64_t data)


### dci ###

Emit raw instructions into the instruction stream.

    inline void dci(Instr raw_inst)



