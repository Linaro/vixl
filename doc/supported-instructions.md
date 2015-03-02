VIXL Supported Instruction List
===============================

This is a list of the AArch64 instructions supported by the VIXL assembler,
disassembler and simulator. The simulator may not support all floating point
operations to the precision required by AArch64 - please check the simulator
source code for details.

AArch64 integer instructions
----------------------------

### ADC ###

Add with carry bit.

    void adc(const Register& rd,
             const Register& rn,
             const Operand& operand)


### ADCS ###

Add with carry bit and update status flags.

    void adcs(const Register& rd,
              const Register& rn,
              const Operand& operand)


### ADD ###

Add.

    void add(const Register& rd,
             const Register& rn,
             const Operand& operand)


### ADDS ###

Add and update status flags.

    void adds(const Register& rd,
              const Register& rn,
              const Operand& operand)


### ADR ###

Calculate the address of a PC offset.

    void adr(const Register& rd, int imm21)


### ADR ###

Calculate the address of a label.

    void adr(const Register& rd, Label* label)


### ADRP ###

Calculate the page address of a PC offset.

    void adrp(const Register& rd, int imm21)


### ADRP ###

Calculate the page address of a label.

    void adrp(const Register& rd, Label* label)


### AND ###

Bitwise and (A & B).

    void and_(const Register& rd,
              const Register& rn,
              const Operand& operand)


### ANDS ###

Bitwise and (A & B) and update status flags.

    void ands(const Register& rd,
              const Register& rn,
              const Operand& operand)


### ASR ###

Arithmetic shift right.

    void asr(const Register& rd, const Register& rn, unsigned shift)


### ASRV ###

Arithmetic shift right by variable.

    void asrv(const Register& rd, const Register& rn, const Register& rm)


### B ###

Conditional branch to PC offset.

    void b(int imm19, Condition cond)


### B ###

Conditional branch to label.

    void b(Label* label, Condition cond)


### B ###

Unconditional branch to PC offset.

    void b(int imm26)


### B ###

Unconditional branch to label.

    void b(Label* label)


### BFI ###

Bitfield insert.

    void bfi(const Register& rd,
             const Register& rn,
             unsigned lsb,
             unsigned width)


### BFM ###

Bitfield move.

    void bfm(const Register& rd,
             const Register& rn,
             unsigned immr,
             unsigned imms)


### BFXIL ###

Bitfield extract and insert low.

    void bfxil(const Register& rd,
               const Register& rn,
               unsigned lsb,
               unsigned width)


### BIC ###

Bit clear (A & ~B).

    void bic(const Register& rd,
             const Register& rn,
             const Operand& operand)


### BICS ###

Bit clear (A & ~B) and update status flags.

    void bics(const Register& rd,
              const Register& rn,
              const Operand& operand)


### BL ###

Branch with link to PC offset.

    void bl(int imm26)


### BL ###

Branch with link to label.

    void bl(Label* label)


### BLR ###

Branch with link to register.

    void blr(const Register& xn)


### BR ###

Branch to register.

    void br(const Register& xn)


### BRK ###

Monitor debug-mode breakpoint.

    void brk(int code)


### CBNZ ###

Compare and branch to PC offset if not zero.

    void cbnz(const Register& rt, int imm19)


### CBNZ ###

Compare and branch to label if not zero.

    void cbnz(const Register& rt, Label* label)


### CBZ ###

Compare and branch to PC offset if zero.

    void cbz(const Register& rt, int imm19)


### CBZ ###

Compare and branch to label if zero.

    void cbz(const Register& rt, Label* label)


### CCMN ###

Conditional compare negative.

    void ccmn(const Register& rn,
              const Operand& operand,
              StatusFlags nzcv,
              Condition cond)


### CCMP ###

Conditional compare.

    void ccmp(const Register& rn,
              const Operand& operand,
              StatusFlags nzcv,
              Condition cond)


### CINC ###

Conditional increment: rd = cond ? rn + 1 : rn.

    void cinc(const Register& rd, const Register& rn, Condition cond)


### CINV ###

Conditional invert: rd = cond ? ~rn : rn.

    void cinv(const Register& rd, const Register& rn, Condition cond)


### CLREX ###

Clear exclusive monitor.

    void clrex(int imm4 = 0xf)


### CLS ###

Count leading sign bits.

    void cls(const Register& rd, const Register& rn)


### CLZ ###

Count leading zeroes.

    void clz(const Register& rd, const Register& rn)


### CMN ###

Compare negative.

    void cmn(const Register& rn, const Operand& operand)


### CMP ###

Compare.

    void cmp(const Register& rn, const Operand& operand)


### CNEG ###

Conditional negate: rd = cond ? -rn : rn.

    void cneg(const Register& rd, const Register& rn, Condition cond)


### CRC32B ###

CRC-32 checksum from byte.

    void crc32b(const Register& rd,
                const Register& rn,
                const Register& rm)


### CRC32CB ###

CRC-32 C checksum from byte.

    void crc32cb(const Register& rd,
                 const Register& rn,
                 const Register& rm)


### CRC32CH ###

CRC-32 C checksum from half-word.

    void crc32ch(const Register& rd,
                 const Register& rn,
                 const Register& rm)


### CRC32CW ###

CRC-32 C checksum from word.

    void crc32cw(const Register& rd,
                 const Register& rn,
                 const Register& rm)


### CRC32CX ###

CRC-32C checksum from double word.

    void crc32cx(const Register& rd,
                 const Register& rn,
                 const Register& rm)


### CRC32H ###

CRC-32 checksum from half-word.

    void crc32h(const Register& rd,
                const Register& rn,
                const Register& rm)


### CRC32W ###

CRC-32 checksum from word.

    void crc32w(const Register& rd,
                const Register& rn,
                const Register& rm)


### CRC32X ###

CRC-32 checksum from double word.

    void crc32x(const Register& rd,
                const Register& rn,
                const Register& rm)


### CSEL ###

Conditional select: rd = cond ? rn : rm.

    void csel(const Register& rd,
              const Register& rn,
              const Register& rm,
              Condition cond)


### CSET ###

Conditional set: rd = cond ? 1 : 0.

    void cset(const Register& rd, Condition cond)


### CSETM ###

Conditional set mask: rd = cond ? -1 : 0.

    void csetm(const Register& rd, Condition cond)


### CSINC ###

Conditional select increment: rd = cond ? rn : rm + 1.

    void csinc(const Register& rd,
               const Register& rn,
               const Register& rm,
               Condition cond)


### CSINV ###

Conditional select inversion: rd = cond ? rn : ~rm.

    void csinv(const Register& rd,
               const Register& rn,
               const Register& rm,
               Condition cond)


### CSNEG ###

Conditional select negation: rd = cond ? rn : -rm.

    void csneg(const Register& rd,
               const Register& rn,
               const Register& rm,
               Condition cond)


### DC ###

System data cache operation.

    void dc(DataCacheOp op, const Register& rt)


### DMB ###

Data memory barrier.

    void dmb(BarrierDomain domain, BarrierType type)


### DSB ###

Data synchronization barrier.

    void dsb(BarrierDomain domain, BarrierType type)


### EON ###

Bitwise enor/xnor (A ^ ~B).

    void eon(const Register& rd, const Register& rn, const Operand& operand)


### EOR ###

Bitwise eor/xor (A ^ B).

    void eor(const Register& rd, const Register& rn, const Operand& operand)


### EXTR ###

Extract.

    void extr(const Register& rd,
              const Register& rn,
              const Register& rm,
              unsigned lsb)


### HINT ###

System hint.

    void hint(SystemHint code)


### HLT ###

Halting debug-mode breakpoint.

    void hlt(int code)


### IC ###

System instruction cache operation.

    void ic(InstructionCacheOp op, const Register& rt)


### ISB ###

Instruction synchronization barrier.

    void isb()


### LDAR ###

Load-acquire register.

    void ldar(const Register& rt, const MemOperand& src)


### LDARB ###

Load-acquire byte.

    void ldarb(const Register& rt, const MemOperand& src)


### LDARH ###

Load-acquire half-word.

    void ldarh(const Register& rt, const MemOperand& src)


### LDAXP ###

Load-acquire exclusive register pair.

    void ldaxp(const Register& rt, const Register& rt2, const MemOperand& src)


### LDAXR ###

Load-acquire exclusive register.

    void ldaxr(const Register& rt, const MemOperand& src)


### LDAXRB ###

Load-acquire exclusive byte.

    void ldaxrb(const Register& rt, const MemOperand& src)


### LDAXRH ###

Load-acquire exclusive half-word.

    void ldaxrh(const Register& rt, const MemOperand& src)


### LDNP ###

Load integer or FP register pair, non-temporal.

    void ldnp(const CPURegister& rt, const CPURegister& rt2,
              const MemOperand& src)


### LDP ###

Load integer or FP register pair.

    void ldp(const CPURegister& rt, const CPURegister& rt2,
             const MemOperand& src)


### LDPSW ###

Load word pair with sign extension.

    void ldpsw(const Register& rt, const Register& rt2, const MemOperand& src)


### LDR ###

Load integer or FP register from literal pool.

    void ldr(const CPURegister& rt, RawLiteral* literal)


### LDR ###

Load integer or FP register from pc + imm19 << 2.

    void ldr(const CPURegister& rt, int imm19)


### LDR ###

Load integer or FP register.

    void ldr(const CPURegister& rt, const MemOperand& src,
             LoadStoreScalingOption option = PreferScaledOffset)


### LDRB ###

Load byte.

    void ldrb(const Register& rt, const MemOperand& src,
              LoadStoreScalingOption option = PreferScaledOffset)


### LDRH ###

Load half-word.

    void ldrh(const Register& rt, const MemOperand& src,
              LoadStoreScalingOption option = PreferScaledOffset)


### LDRSB ###

Load byte with sign extension.

    void ldrsb(const Register& rt, const MemOperand& src,
               LoadStoreScalingOption option = PreferScaledOffset)


### LDRSH ###

Load half-word with sign extension.

    void ldrsh(const Register& rt, const MemOperand& src,
               LoadStoreScalingOption option = PreferScaledOffset)


### LDRSW ###

Load word with sign extension from literal pool.

    void ldrsw(const Register& rt, RawLiteral* literal)


### LDRSW ###

Load word with sign extension from pc + imm19 << 2.

    void ldrsw(const Register& rt, int imm19)


### LDRSW ###

Load word with sign extension.

    void ldrsw(const Register& rt, const MemOperand& src,
               LoadStoreScalingOption option = PreferScaledOffset)


### LDUR ###

Load integer or FP register (with unscaled offset).

    void ldur(const CPURegister& rt, const MemOperand& src,
              LoadStoreScalingOption option = PreferUnscaledOffset)


### LDURB ###

Load byte (with unscaled offset).

    void ldurb(const Register& rt, const MemOperand& src,
               LoadStoreScalingOption option = PreferUnscaledOffset)


### LDURH ###

Load half-word (with unscaled offset).

    void ldurh(const Register& rt, const MemOperand& src,
               LoadStoreScalingOption option = PreferUnscaledOffset)


### LDURSB ###

Load byte with sign extension (and unscaled offset).

    void ldursb(const Register& rt, const MemOperand& src,
                LoadStoreScalingOption option = PreferUnscaledOffset)


### LDURSH ###

Load half-word with sign extension (and unscaled offset).

    void ldursh(const Register& rt, const MemOperand& src,
                LoadStoreScalingOption option = PreferUnscaledOffset)


### LDURSW ###

Load word with sign extension.

    void ldursw(const Register& rt, const MemOperand& src,
                LoadStoreScalingOption option = PreferUnscaledOffset)


### LDXP ###

Load exclusive register pair.

    void ldxp(const Register& rt, const Register& rt2, const MemOperand& src)


### LDXR ###

Load exclusive register.

    void ldxr(const Register& rt, const MemOperand& src)


### LDXRB ###

Load exclusive byte.

    void ldxrb(const Register& rt, const MemOperand& src)


### LDXRH ###

Load exclusive half-word.

    void ldxrh(const Register& rt, const MemOperand& src)


### LSL ###

Logical shift left.

    void lsl(const Register& rd, const Register& rn, unsigned shift)


### LSLV ###

Logical shift left by variable.

    void lslv(const Register& rd, const Register& rn, const Register& rm)


### LSR ###

Logical shift right.

    void lsr(const Register& rd, const Register& rn, unsigned shift)


### LSRV ###

Logical shift right by variable.

    void lsrv(const Register& rd, const Register& rn, const Register& rm)


### MADD ###

Multiply and accumulate.

    void madd(const Register& rd,
              const Register& rn,
              const Register& rm,
              const Register& ra)


### MNEG ###

Negated multiply.

    void mneg(const Register& rd, const Register& rn, const Register& rm)


### MOV ###

Move register to register.

    void mov(const Register& rd, const Register& rn)


### MOVK ###

Move immediate and keep.

    void movk(const Register& rd, uint64_t imm, int shift = -1)


### MOVN ###

Move inverted immediate.

    void movn(const Register& rd, uint64_t imm, int shift = -1)


### MOVZ ###

Move immediate.

    void movz(const Register& rd, uint64_t imm, int shift = -1)


### MRS ###

Move to register from system register.

    void mrs(const Register& rt, SystemRegister sysreg)


### MSR ###

Move from register to system register.

    void msr(SystemRegister sysreg, const Register& rt)


### MSUB ###

Multiply and subtract.

    void msub(const Register& rd,
              const Register& rn,
              const Register& rm,
              const Register& ra)


### MUL ###

Multiply.

    void mul(const Register& rd, const Register& rn, const Register& rm)


### MVN ###

Move inverted operand to register.

    void mvn(const Register& rd, const Operand& operand)


### NEG ###

Negate.

    void neg(const Register& rd,
             const Operand& operand)


### NEGS ###

Negate and update status flags.

    void negs(const Register& rd,
              const Operand& operand)


### NGC ###

Negate with carry bit.

    void ngc(const Register& rd,
             const Operand& operand)


### NGCS ###

Negate with carry bit and update status flags.

    void ngcs(const Register& rd,
              const Operand& operand)


### NOP ###

No-op.

    void nop()


### ORN ###

Bitwise nor (A | ~B).

    void orn(const Register& rd, const Register& rn, const Operand& operand)


### ORR ###

Bitwise or (A | B).

    void orr(const Register& rd, const Register& rn, const Operand& operand)


### PRFM ###

Prefetch from pc + imm19 << 2.

    void prfm(PrefetchOperation op, int imm19)


### PRFM ###

Prefetch memory in the literal pool.

    void prfm(PrefetchOperation op, RawLiteral* literal)


### PRFM ###

Prefetch memory.

    void prfm(PrefetchOperation op, const MemOperand& addr,
              LoadStoreScalingOption option = PreferScaledOffset)


### PRFUM ###

Prefetch memory (with unscaled offset).

    void prfum(PrefetchOperation op, const MemOperand& addr,
               LoadStoreScalingOption option = PreferUnscaledOffset)


### RBIT ###

Bit reverse.

    void rbit(const Register& rd, const Register& rn)


### RET ###

Branch to register with return hint.

    void ret(const Register& xn = lr)


### REV ###

Reverse bytes.

    void rev(const Register& rd, const Register& rn)


### REV16 ###

Reverse bytes in 16-bit half words.

    void rev16(const Register& rd, const Register& rn)


### REV32 ###

Reverse bytes in 32-bit words.

    void rev32(const Register& rd, const Register& rn)


### ROR ###

Rotate right.

    void ror(const Register& rd, const Register& rs, unsigned shift)


### RORV ###

Rotate right by variable.

    void rorv(const Register& rd, const Register& rn, const Register& rm)


### SBC ###

Subtract with carry bit.

    void sbc(const Register& rd,
             const Register& rn,
             const Operand& operand)


### SBCS ###

Subtract with carry bit and update status flags.

    void sbcs(const Register& rd,
              const Register& rn,
              const Operand& operand)


### SBFIZ ###

Signed bitfield insert with zero at right.

    void sbfiz(const Register& rd,
               const Register& rn,
               unsigned lsb,
               unsigned width)


### SBFM ###

Signed bitfield move.

    void sbfm(const Register& rd,
              const Register& rn,
              unsigned immr,
              unsigned imms)


### SBFX ###

Signed bitfield extract.

    void sbfx(const Register& rd,
              const Register& rn,
              unsigned lsb,
              unsigned width)


### SDIV ###

Signed integer divide.

    void sdiv(const Register& rd, const Register& rn, const Register& rm)


### SMADDL ###

Signed long multiply and accumulate: 32 x 32 + 64 -> 64-bit.

    void smaddl(const Register& rd,
                const Register& rn,
                const Register& rm,
                const Register& ra)


### SMSUBL ###

Signed long multiply and subtract: 64 - (32 x 32) -> 64-bit.

    void smsubl(const Register& rd,
                const Register& rn,
                const Register& rm,
                const Register& ra)


### SMULH ###

Signed multiply high: 64 x 64 -> 64-bit <127:64>.

    void smulh(const Register& xd, const Register& xn, const Register& xm)


### SMULL ###

Signed long multiply: 32 x 32 -> 64-bit.

    void smull(const Register& rd, const Register& rn, const Register& rm)


### STLR ###

Store-release register.

    void stlr(const Register& rt, const MemOperand& dst)


### STLRB ###

Store-release byte.

    void stlrb(const Register& rt, const MemOperand& dst)


### STLRH ###

Store-release half-word.

    void stlrh(const Register& rt, const MemOperand& dst)


### STLXP ###

Store-release exclusive register pair.

    void stlxp(const Register& rs,
               const Register& rt,
               const Register& rt2,
               const MemOperand& dst)


### STLXR ###

Store-release exclusive register.

    void stlxr(const Register& rs, const Register& rt, const MemOperand& dst)


### STLXRB ###

Store-release exclusive byte.

    void stlxrb(const Register& rs, const Register& rt, const MemOperand& dst)


### STLXRH ###

Store-release exclusive half-word.

    void stlxrh(const Register& rs, const Register& rt, const MemOperand& dst)


### STNP ###

Store integer or FP register pair, non-temporal.

    void stnp(const CPURegister& rt, const CPURegister& rt2,
              const MemOperand& dst)


### STP ###

Store integer or FP register pair.

    void stp(const CPURegister& rt, const CPURegister& rt2,
             const MemOperand& dst)


### STR ###

Store integer or FP register.

    void str(const CPURegister& rt, const MemOperand& dst,
             LoadStoreScalingOption option = PreferScaledOffset)


### STRB ###

Store byte.

    void strb(const Register& rt, const MemOperand& dst,
              LoadStoreScalingOption option = PreferScaledOffset)


### STRH ###

Store half-word.

    void strh(const Register& rt, const MemOperand& dst,
              LoadStoreScalingOption option = PreferScaledOffset)


### STUR ###

Store integer or FP register (with unscaled offset).

    void stur(const CPURegister& rt, const MemOperand& src,
              LoadStoreScalingOption option = PreferUnscaledOffset)


### STURB ###

Store byte (with unscaled offset).

    void sturb(const Register& rt, const MemOperand& dst,
               LoadStoreScalingOption option = PreferUnscaledOffset)


### STURH ###

Store half-word (with unscaled offset).

    void sturh(const Register& rt, const MemOperand& dst,
               LoadStoreScalingOption option = PreferUnscaledOffset)


### STXP ###

Store exclusive register pair.

    void stxp(const Register& rs,
              const Register& rt,
              const Register& rt2,
              const MemOperand& dst)


### STXR ###

Store exclusive register.

    void stxr(const Register& rs, const Register& rt, const MemOperand& dst)


### STXRB ###

Store exclusive byte.

    void stxrb(const Register& rs, const Register& rt, const MemOperand& dst)


### STXRH ###

Store exclusive half-word.

    void stxrh(const Register& rs, const Register& rt, const MemOperand& dst)


### SUB ###

Subtract.

    void sub(const Register& rd,
             const Register& rn,
             const Operand& operand)


### SUBS ###

Subtract and update status flags.

    void subs(const Register& rd,
              const Register& rn,
              const Operand& operand)


### SVC ###

Generate exception targeting EL1.

    void svc(int code)


### SXTB ###

Signed extend byte.

    void sxtb(const Register& rd, const Register& rn)


### SXTH ###

Signed extend halfword.

    void sxth(const Register& rd, const Register& rn)


### SXTW ###

Signed extend word.

    void sxtw(const Register& rd, const Register& rn)


### SYS ###

System instruction with pre-encoded op (op1:crn:crm:op2).

    void sys(int op, const Register& rt = xzr)


### SYS ###

System instruction.

    void sys(int op1, int crn, int crm, int op2, const Register& rt = xzr)


### TBNZ ###

Test bit and branch to PC offset if not zero.

    void tbnz(const Register& rt, unsigned bit_pos, int imm14)


### TBNZ ###

Test bit and branch to label if not zero.

    void tbnz(const Register& rt, unsigned bit_pos, Label* label)


### TBZ ###

Test bit and branch to PC offset if zero.

    void tbz(const Register& rt, unsigned bit_pos, int imm14)


### TBZ ###

Test bit and branch to label if zero.

    void tbz(const Register& rt, unsigned bit_pos, Label* label)


### TST ###

Bit test and set flags.

    void tst(const Register& rn, const Operand& operand)


### UBFIZ ###

Unsigned bitfield insert with zero at right.

    void ubfiz(const Register& rd,
               const Register& rn,
               unsigned lsb,
               unsigned width)


### UBFM ###

Unsigned bitfield move.

    void ubfm(const Register& rd,
              const Register& rn,
              unsigned immr,
              unsigned imms)


### UBFX ###

Unsigned bitfield extract.

    void ubfx(const Register& rd,
              const Register& rn,
              unsigned lsb,
              unsigned width)


### UDIV ###

Unsigned integer divide.

    void udiv(const Register& rd, const Register& rn, const Register& rm)


### UMADDL ###

Unsigned long multiply and accumulate: 32 x 32 + 64 -> 64-bit.

    void umaddl(const Register& rd,
                const Register& rn,
                const Register& rm,
                const Register& ra)


### UMSUBL ###

Unsigned long multiply and subtract: 64 - (32 x 32) -> 64-bit.

    void umsubl(const Register& rd,
                const Register& rn,
                const Register& rm,
                const Register& ra)


### UXTB ###

Unsigned extend byte.

    void uxtb(const Register& rd, const Register& rn)


### UXTH ###

Unsigned extend halfword.

    void uxth(const Register& rd, const Register& rn)


### UXTW ###

Unsigned extend word.

    void uxtw(const Register& rd, const Register& rn)



AArch64 floating point and NEON instructions
--------------------------------------------

### ABS ###

Absolute value.

    void abs(const VRegister& vd,
             const VRegister& vn)


### ADD ###

Add.

    void add(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### ADDHN ###

Add narrow returning high half.

    void addhn(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### ADDHN2 ###

Add narrow returning high half (second part).

    void addhn2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### ADDP ###

Add pair of elements scalar.

    void addp(const VRegister& vd,
              const VRegister& vn)


### ADDP ###

Add pairwise.

    void addp(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### ADDV ###

Add across vector.

    void addv(const VRegister& vd,
              const VRegister& vn)


### AND ###

Bitwise and.

    void and_(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### BIC ###

Bit clear immediate.

    void bic(const VRegister& vd,
             const int imm8,
             const int left_shift = 0)


### BIC ###

Bit clear.

    void bic(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### BIF ###

Bitwise insert if false.

    void bif(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### BIT ###

Bitwise insert if true.

    void bit(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### BSL ###

Bitwise select.

    void bsl(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### CLS ###

Count leading sign bits.

    void cls(const VRegister& vd,
             const VRegister& vn)


### CLZ ###

Count leading zero bits (vector).

    void clz(const VRegister& vd,
             const VRegister& vn)


### CMEQ ###

Compare bitwise to zero.

    void cmeq(const VRegister& vd,
              const VRegister& vn,
              int value)


### CMEQ ###

Compare equal.

    void cmeq(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### CMGE ###

Compare signed greater than or equal to zero.

    void cmge(const VRegister& vd,
              const VRegister& vn,
              int value)


### CMGE ###

Compare signed greater than or equal.

    void cmge(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### CMGT ###

Compare signed greater than zero.

    void cmgt(const VRegister& vd,
              const VRegister& vn,
              int value)


### CMGT ###

Compare signed greater than.

    void cmgt(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### CMHI ###

Compare unsigned higher.

    void cmhi(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### CMHS ###

Compare unsigned higher or same.

    void cmhs(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### CMLE ###

Compare signed less than or equal to zero.

    void cmle(const VRegister& vd,
              const VRegister& vn,
              int value)


### CMLT ###

Compare signed less than zero.

    void cmlt(const VRegister& vd,
              const VRegister& vn,
              int value)


### CMTST ###

Compare bitwise test bits nonzero.

    void cmtst(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### CNT ###

Population count per byte.

    void cnt(const VRegister& vd,
             const VRegister& vn)


### DUP ###

Duplicate general-purpose register to vector.

    void dup(const VRegister& vd,
             const Register& rn)


### DUP ###

Duplicate vector element to vector or scalar.

    void dup(const VRegister& vd,
             const VRegister& vn,
             int vn_index)


### EOR ###

Bitwise eor.

    void eor(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### EXT ###

Extract vector from pair of vectors.

    void ext(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm,
             int index)


### FABD ###

FP absolute difference.

    void fabd(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### FABS ###

FP absolute.

    void fabs(const VRegister& vd, const VRegister& vn)


### FACGE ###

FP absolute greater than or equal.

    void facge(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### FACGT ###

FP absolute greater than.

    void facgt(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### FADD ###

FP add.

    void fadd(const VRegister& vd, const VRegister& vn, const VRegister& vm)


### FADDP ###

FP pairwise add scalar.

    void faddp(const VRegister& vd,
               const VRegister& vn)


### FADDP ###

FP pairwise add vector.

    void faddp(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### FCCMP ###

FP conditional compare.

    void fccmp(const VRegister& vn,
               const VRegister& vm,
               StatusFlags nzcv,
               Condition cond)


### FCMEQ ###

FP compare equal to zero.

    void fcmeq(const VRegister& vd,
               const VRegister& vn,
               double imm)


### FCMEQ ###

FP compare equal.

    void fcmeq(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### FCMGE ###

FP greater than or equal to zero.

    void fcmge(const VRegister& vd,
               const VRegister& vn,
               double imm)


### FCMGE ###

FP greater than or equal.

    void fcmge(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### FCMGT ###

FP greater than zero.

    void fcmgt(const VRegister& vd,
               const VRegister& vn,
               double imm)


### FCMGT ###

FP greater than.

    void fcmgt(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### FCMLE ###

FP less than or equal to zero.

    void fcmle(const VRegister& vd,
               const VRegister& vn,
               double imm)


### FCMLT ###

FP less than to zero.

    void fcmlt(const VRegister& vd,
               const VRegister& vn,
               double imm)


### FCMP ###

FP compare immediate.

    void fcmp(const VRegister& vn, double value)


### FCMP ###

FP compare registers.

    void fcmp(const VRegister& vn, const VRegister& vm)


### FCSEL ###

FP conditional select.

    void fcsel(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm,
               Condition cond)


### FCVT ###

FP convert between precisions.

    void fcvt(const VRegister& vd, const VRegister& vn)


### FCVTAS ###

FP convert to signed integer, nearest with ties to away.

    void fcvtas(const Register& rd, const VRegister& vn)


### FCVTAS ###

FP convert to signed integer, nearest with ties to away.

    void fcvtas(const VRegister& vd, const VRegister& vn)


### FCVTAU ###

FP convert to unsigned integer, nearest with ties to away.

    void fcvtau(const Register& rd, const VRegister& vn)


### FCVTAU ###

FP convert to unsigned integer, nearest with ties to away.

    void fcvtau(const VRegister& vd, const VRegister& vn)


### FCVTL ###

FP convert to higher precision.

    void fcvtl(const VRegister& vd, const VRegister& vn)


### FCVTL2 ###

FP convert to higher precision (second part).

    void fcvtl2(const VRegister& vd, const VRegister& vn)


### FCVTMS ###

FP convert to signed integer, round towards -infinity.

    void fcvtms(const VRegister& vd, const VRegister& vn)


### FCVTMS ###

FP convert to signed integer, round towards -infinity.

    void fcvtms(const Register& rd, const VRegister& vn)


### FCVTMU ###

FP convert to unsigned integer, round towards -infinity.

    void fcvtmu(const VRegister& vd, const VRegister& vn)


### FCVTMU ###

FP convert to unsigned integer, round towards -infinity.

    void fcvtmu(const Register& rd, const VRegister& vn)


### FCVTN ###

FP convert to lower precision.

    void fcvtn(const VRegister& vd, const VRegister& vn)


### FCVTN2 ###

FP convert to lower prevision (second part).

    void fcvtn2(const VRegister& vd, const VRegister& vn)


### FCVTNS ###

FP convert to signed integer, nearest with ties to even.

    void fcvtns(const Register& rd, const VRegister& vn)


### FCVTNS ###

FP convert to signed integer, nearest with ties to even.

    void fcvtns(const VRegister& rd, const VRegister& vn)


### FCVTNU ###

FP convert to unsigned integer, nearest with ties to even.

    void fcvtnu(const Register& rd, const VRegister& vn)


### FCVTNU ###

FP convert to unsigned integer, nearest with ties to even.

    void fcvtnu(const VRegister& rd, const VRegister& vn)


### FCVTPS ###

FP convert to signed integer, round towards +infinity.

    void fcvtps(const VRegister& vd, const VRegister& vn)


### FCVTPS ###

FP convert to signed integer, round towards +infinity.

    void fcvtps(const Register& rd, const VRegister& vn)


### FCVTPU ###

FP convert to unsigned integer, round towards +infinity.

    void fcvtpu(const VRegister& vd, const VRegister& vn)


### FCVTPU ###

FP convert to unsigned integer, round towards +infinity.

    void fcvtpu(const Register& rd, const VRegister& vn)


### FCVTXN ###

FP convert to lower precision, rounding to odd.

    void fcvtxn(const VRegister& vd, const VRegister& vn)


### FCVTXN2 ###

FP convert to lower precision, rounding to odd (second part).

    void fcvtxn2(const VRegister& vd, const VRegister& vn)


### FCVTZS ###

FP convert to signed integer or fixed-point, round towards zero.

    void fcvtzs(const VRegister& vd, const VRegister& vn, int fbits = 0)


### FCVTZS ###

FP convert to signed integer or fixed-point, round towards zero.

    void fcvtzs(const Register& rd, const VRegister& vn, int fbits = 0)


### FCVTZU ###

FP convert to unsigned integer or fixed-point, round towards zero.

    void fcvtzu(const Register& rd, const VRegister& vn, int fbits = 0)


### FCVTZU ###

FP convert to unsigned integer or fixed-point, round towards zero.

    void fcvtzu(const VRegister& vd, const VRegister& vn, int fbits = 0)


### FDIV ###

FP divide.

    void fdiv(const VRegister& vd, const VRegister& fn, const VRegister& vm)


### FMADD ###

FP fused multiply-add.

    void fmadd(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm,
               const VRegister& va)


### FMAX ###

FP maximum.

    void fmax(const VRegister& vd, const VRegister& fn, const VRegister& vm)


### FMAXNM ###

FP maximum number.

    void fmaxnm(const VRegister& vd, const VRegister& fn, const VRegister& vm)


### FMAXNMP ###

FP pairwise maximum number scalar.

    void fmaxnmp(const VRegister& vd,
                 const VRegister& vn)


### FMAXNMP ###

FP pairwise maximum number vector.

    void fmaxnmp(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm)


### FMAXNMV ###

FP maximum number across vector.

    void fmaxnmv(const VRegister& vd,
                 const VRegister& vn)


### FMAXP ###

FP pairwise maximum scalar.

    void fmaxp(const VRegister& vd,
               const VRegister& vn)


### FMAXP ###

FP pairwise maximum vector.

    void fmaxp(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### FMAXV ###

FP maximum across vector.

    void fmaxv(const VRegister& vd,
               const VRegister& vn)


### FMIN ###

FP minimum.

    void fmin(const VRegister& vd, const VRegister& fn, const VRegister& vm)


### FMINNM ###

FP minimum number.

    void fminnm(const VRegister& vd, const VRegister& fn, const VRegister& vm)


### FMINNMP ###

FP pairwise minimum number scalar.

    void fminnmp(const VRegister& vd,
                 const VRegister& vn)


### FMINNMP ###

FP pairwise minimum number vector.

    void fminnmp(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm)


### FMINNMV ###

FP minimum number across vector.

    void fminnmv(const VRegister& vd,
                 const VRegister& vn)


### FMINP ###

FP pairwise minimum scalar.

    void fminp(const VRegister& vd,
               const VRegister& vn)


### FMINP ###

FP pairwise minimum vector.

    void fminp(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### FMINV ###

FP minimum across vector.

    void fminv(const VRegister& vd,
               const VRegister& vn)


### FMLA ###

FP fused multiply-add to accumulator by element.

    void fmla(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm,
              int vm_index)


### FMLA ###

FP vector multiply accumulate.

    void fmla(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### FMLS ###

FP fused multiply-sub from accumulator by element.

    void fmls(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm,
              int vm_index)


### FMLS ###

FP vector multiply subtract.

    void fmls(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### FMOV ###

Move 64-bit register to top half of 128-bit FP register.

    void fmov(const VRegister& vd, int index, const Register& rn)


### FMOV ###

Move FP register to FP register.

    void fmov(const VRegister& vd, const VRegister& fn)


### FMOV ###

Move FP register to register.

    void fmov(const Register& rd, const VRegister& fn)


### FMOV ###

Move double precision immediate to FP register.

    void fmov(const VRegister& vd, double imm)


### FMOV ###

Move register to FP register.

    void fmov(const VRegister& vd, const Register& rn)


### FMOV ###

Move single precision immediate to FP register.

    void fmov(const VRegister& vd, float imm)


### FMOV ###

Move top half of 128-bit FP register to 64-bit register.

    void fmov(const Register& rd, const VRegister& vn, int index)


### FMSUB ###

FP fused multiply-subtract.

    void fmsub(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm,
               const VRegister& va)


### FMUL ###

FP multiply by element.

    void fmul(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm,
              int vm_index)


### FMUL ###

FP multiply.

    void fmul(const VRegister& vd, const VRegister& vn, const VRegister& vm)


### FMULX ###

FP multiply extended by element.

    void fmulx(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm,
               int vm_index)


### FMULX ###

FP vector multiply extended.

    void fmulx(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### FNEG ###

FP negate.

    void fneg(const VRegister& vd, const VRegister& vn)


### FNMADD ###

FP fused multiply-add and negate.

    void fnmadd(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm,
                const VRegister& va)


### FNMSUB ###

FP fused multiply-subtract and negate.

    void fnmsub(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm,
                const VRegister& va)


### FNMUL ###

FP multiply-negate scalar.

    void fnmul(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### FRECPE ###

FP reciprocal estimate.

    void frecpe(const VRegister& vd,
                const VRegister& vn)


### FRECPS ###

FP reciprocal step.

    void frecps(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### FRECPX ###

FP reciprocal exponent scalar.

    void frecpx(const VRegister& vd,
                const VRegister& vn)


### FRINTA ###

FP round to integer, nearest with ties to away.

    void frinta(const VRegister& vd, const VRegister& vn)


### FRINTI ###

FP round to integer, implicit rounding.

    void frinti(const VRegister& vd, const VRegister& vn)


### FRINTM ###

FP round to integer, toward minus infinity.

    void frintm(const VRegister& vd, const VRegister& vn)


### FRINTN ###

FP round to integer, nearest with ties to even.

    void frintn(const VRegister& vd, const VRegister& vn)


### FRINTP ###

FP round to integer, toward plus infinity.

    void frintp(const VRegister& vd, const VRegister& vn)


### FRINTX ###

FP round to integer, exact, implicit rounding.

    void frintx(const VRegister& vd, const VRegister& vn)


### FRINTZ ###

FP round to integer, towards zero.

    void frintz(const VRegister& vd, const VRegister& vn)


### FRSQRTE ###

FP reciprocal square root estimate.

    void frsqrte(const VRegister& vd,
                 const VRegister& vn)


### FRSQRTS ###

FP reciprocal square root step.

    void frsqrts(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm)


### FSQRT ###

FP square root.

    void fsqrt(const VRegister& vd, const VRegister& vn)


### FSUB ###

FP subtract.

    void fsub(const VRegister& vd, const VRegister& vn, const VRegister& vm)


### INS ###

Insert vector element from another vector element.

    void ins(const VRegister& vd,
             int vd_index,
             const VRegister& vn,
             int vn_index)


### INS ###

Insert vector element from general-purpose register.

    void ins(const VRegister& vd,
             int vd_index,
             const Register& rn)


### LD1 ###

One-element single structure load to one lane.

    void ld1(const VRegister& vt,
             int lane,
             const MemOperand& src)


### LD1 ###

One-element structure load to four registers.

    void ld1(const VRegister& vt,
             const VRegister& vt2,
             const VRegister& vt3,
             const VRegister& vt4,
             const MemOperand& src)


### LD1 ###

One-element structure load to one register.

    void ld1(const VRegister& vt,
             const MemOperand& src)


### LD1 ###

One-element structure load to three registers.

    void ld1(const VRegister& vt,
             const VRegister& vt2,
             const VRegister& vt3,
             const MemOperand& src)


### LD1 ###

One-element structure load to two registers.

    void ld1(const VRegister& vt,
             const VRegister& vt2,
             const MemOperand& src)


### LD1R ###

One-element single structure load to all lanes.

    void ld1r(const VRegister& vt,
              const MemOperand& src)


### LD2 ###

Two-element single structure load to one lane.

    void ld2(const VRegister& vt,
             const VRegister& vt2,
             int lane,
             const MemOperand& src)


### LD2 ###

Two-element structure load.

    void ld2(const VRegister& vt,
             const VRegister& vt2,
             const MemOperand& src)


### LD2R ###

Two-element single structure load to all lanes.

    void ld2r(const VRegister& vt,
              const VRegister& vt2,
              const MemOperand& src)


### LD3 ###

Three-element single structure load to one lane.

    void ld3(const VRegister& vt,
             const VRegister& vt2,
             const VRegister& vt3,
             int lane,
             const MemOperand& src)


### LD3 ###

Three-element structure load.

    void ld3(const VRegister& vt,
             const VRegister& vt2,
             const VRegister& vt3,
             const MemOperand& src)


### LD3R ###

Three-element single structure load to all lanes.

    void ld3r(const VRegister& vt,
              const VRegister& vt2,
              const VRegister& vt3,
              const MemOperand& src)


### LD4 ###

Four-element single structure load to one lane.

    void ld4(const VRegister& vt,
             const VRegister& vt2,
             const VRegister& vt3,
             const VRegister& vt4,
             int lane,
             const MemOperand& src)


### LD4 ###

Four-element structure load.

    void ld4(const VRegister& vt,
             const VRegister& vt2,
             const VRegister& vt3,
             const VRegister& vt4,
             const MemOperand& src)


### LD4R ###

Four-element single structure load to all lanes.

    void ld4r(const VRegister& vt,
              const VRegister& vt2,
              const VRegister& vt3,
              const VRegister& vt4,
              const MemOperand& src)


### MLA ###

Multiply-add by scalar element.

    void mla(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm,
             int vm_index)


### MLA ###

Multiply-add to accumulator.

    void mla(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### MLS ###

Multiply-subtract by scalar element.

    void mls(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm,
             int vm_index)


### MLS ###

Multiply-subtract to accumulator.

    void mls(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### MOV ###

Move general-purpose register to a vector element.

    void mov(const VRegister& vd,
             int vd_index,
             const Register& rn)


### MOV ###

Move register to register.

    void mov(const VRegister& vd,
             const VRegister& vn)


### MOV ###

Move vector element to another vector element.

    void mov(const VRegister& vd,
             int vd_index,
             const VRegister& vn,
             int vn_index)


### MOV ###

Move vector element to general-purpose register.

    void mov(const Register& rd,
             const VRegister& vn,
             int vn_index)


### MOV ###

Move vector element to scalar.

    void mov(const VRegister& vd,
             const VRegister& vn,
             int vn_index)


### MOVI ###

Vector move immediate.

    void movi(const VRegister& vd,
              const uint64_t imm,
              Shift shift = LSL,
              const int shift_amount = 0)


### MUL ###

Multiply by scalar element.

    void mul(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm,
             int vm_index)


### MUL ###

Multiply.

    void mul(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### MVN ###

Bitwise not.

    void mvn(const VRegister& vd,
             const VRegister& vn)


### MVNI ###

Vector move inverted immediate.

    void mvni(const VRegister& vd,
              const int imm8,
              Shift shift = LSL,
              const int shift_amount = 0)


### NEG ###

Negate.

    void neg(const VRegister& vd,
             const VRegister& vn)


### NOT ###

Bitwise not.

    void not_(const VRegister& vd,
              const VRegister& vn)


### ORN ###

Bitwise orn.

    void orn(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### ORR ###

Bitwise or immediate.

    void orr(const VRegister& vd,
             const int imm8,
             const int left_shift = 0)


### ORR ###

Bitwise or.

    void orr(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### PMUL ###

Polynomial multiply.

    void pmul(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### PMULL ###

Polynomial multiply long.

    void pmull(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### PMULL2 ###

Polynomial multiply long (second part).

    void pmull2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### RADDHN ###

Rounding add narrow returning high half.

    void raddhn(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### RADDHN2 ###

Rounding add narrow returning high half (second part).

    void raddhn2(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm)


### RBIT ###

Reverse bit order.

    void rbit(const VRegister& vd,
              const VRegister& vn)


### REV16 ###

Reverse elements in 16-bit halfwords.

    void rev16(const VRegister& vd,
               const VRegister& vn)


### REV32 ###

Reverse elements in 32-bit words.

    void rev32(const VRegister& vd,
               const VRegister& vn)


### REV64 ###

Reverse elements in 64-bit doublewords.

    void rev64(const VRegister& vd,
               const VRegister& vn)


### RSHRN ###

Rounding shift right narrow by immediate.

    void rshrn(const VRegister& vd,
               const VRegister& vn,
               int shift)


### RSHRN2 ###

Rounding shift right narrow by immediate (second part).

    void rshrn2(const VRegister& vd,
                const VRegister& vn,
                int shift)


### RSUBHN ###

Rounding subtract narrow returning high half.

    void rsubhn(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### RSUBHN2 ###

Rounding subtract narrow returning high half (second part).

    void rsubhn2(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm)


### SABA ###

Signed absolute difference and accumulate.

    void saba(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### SABAL ###

Signed absolute difference and accumulate long.

    void sabal(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SABAL2 ###

Signed absolute difference and accumulate long (second part).

    void sabal2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### SABD ###

Signed absolute difference.

    void sabd(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### SABDL ###

Signed absolute difference long.

    void sabdl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SABDL2 ###

Signed absolute difference long (second part).

    void sabdl2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### SADALP ###

Signed pairwise long add and accumulate.

    void sadalp(const VRegister& vd,
                const VRegister& vn)


### SADDL ###

Signed add long.

    void saddl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SADDL2 ###

Signed add long (second part).

    void saddl2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### SADDLP ###

Signed pairwise long add.

    void saddlp(const VRegister& vd,
                const VRegister& vn)


### SADDLV ###

Signed add long across vector.

    void saddlv(const VRegister& vd,
                const VRegister& vn)


### SADDW ###

Signed add wide.

    void saddw(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SADDW2 ###

Signed add wide (second part).

    void saddw2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### SCVTF ###

Convert signed integer or fixed point to FP.

    void scvtf(const VRegister& fd, const Register& rn, int fbits = 0)


### SCVTF ###

Convert signed integer or fixed-point to FP.

    void scvtf(const VRegister& fd, const VRegister& vn, int fbits = 0)


### SHADD ###

Signed halving add.

    void shadd(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SHL ###

Shift left by immediate.

    void shl(const VRegister& vd,
             const VRegister& vn,
             int shift)


### SHLL ###

Shift left long by element size.

    void shll(const VRegister& vd,
              const VRegister& vn,
              int shift)


### SHLL2 ###

Shift left long by element size (second part).

    void shll2(const VRegister& vd,
               const VRegister& vn,
               int shift)


### SHRN ###

Shift right narrow by immediate.

    void shrn(const VRegister& vd,
              const VRegister& vn,
              int shift)


### SHRN2 ###

Shift right narrow by immediate (second part).

    void shrn2(const VRegister& vd,
               const VRegister& vn,
               int shift)


### SHSUB ###

Signed halving sub.

    void shsub(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SLI ###

Shift left by immediate and insert.

    void sli(const VRegister& vd,
             const VRegister& vn,
             int shift)


### SMAX ###

Signed maximum.

    void smax(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### SMAXP ###

Signed pairwise maximum.

    void smaxp(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SMAXV ###

Signed maximum across vector.

    void smaxv(const VRegister& vd,
               const VRegister& vn)


### SMIN ###

Signed minimum.

    void smin(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### SMINP ###

Signed minimum pairwise.

    void sminp(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SMINV ###

Signed minimum across vector.

    void sminv(const VRegister& vd,
               const VRegister& vn)


### SMLAL ###

Signed long multiply-add by scalar element.

    void smlal(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm,
               int vm_index)


### SMLAL ###

Signed long multiply-add.

    void smlal(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SMLAL2 ###

Signed long multiply-add (second part).

    void smlal2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### SMLAL2 ###

Signed long multiply-add by scalar element (second part).

    void smlal2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm,
                int vm_index)


### SMLSL ###

Signed long multiply-sub by scalar element.

    void smlsl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm,
               int vm_index)


### SMLSL ###

Signed long multiply-sub.

    void smlsl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SMLSL2 ###

Signed long multiply-sub (second part).

    void smlsl2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### SMLSL2 ###

Signed long multiply-sub by scalar element (second part).

    void smlsl2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm,
                int vm_index)


### SMOV ###

Signed move vector element to general-purpose register.

    void smov(const Register& rd,
              const VRegister& vn,
              int vn_index)


### SMULL ###

Signed long multiply by scalar element.

    void smull(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm,
               int vm_index)


### SMULL ###

Signed long multiply.

    void smull(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SMULL2 ###

Signed long multiply (second part).

    void smull2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### SMULL2 ###

Signed long multiply by scalar element (second part).

    void smull2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm,
                int vm_index)


### SQABS ###

Signed saturating absolute value.

    void sqabs(const VRegister& vd,
               const VRegister& vn)


### SQADD ###

Signed saturating add.

    void sqadd(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SQDMLAL ###

Signed saturating doubling long multiply-add by element.

    void sqdmlal(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm,
                 int vm_index)


### SQDMLAL ###

Signed saturating doubling long multiply-add.

    void sqdmlal(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm)


### SQDMLAL2 ###

Signed saturating doubling long multiply-add (second part).

    void sqdmlal2(const VRegister& vd,
                  const VRegister& vn,
                  const VRegister& vm)


### SQDMLAL2 ###

Signed saturating doubling long multiply-add by element (second part).

    void sqdmlal2(const VRegister& vd,
                  const VRegister& vn,
                  const VRegister& vm,
                  int vm_index)


### SQDMLSL ###

Signed saturating doubling long multiply-sub by element.

    void sqdmlsl(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm,
                 int vm_index)


### SQDMLSL ###

Signed saturating doubling long multiply-subtract.

    void sqdmlsl(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm)


### SQDMLSL2 ###

Signed saturating doubling long multiply-sub by element (second part).

    void sqdmlsl2(const VRegister& vd,
                  const VRegister& vn,
                  const VRegister& vm,
                  int vm_index)


### SQDMLSL2 ###

Signed saturating doubling long multiply-subtract (second part).

    void sqdmlsl2(const VRegister& vd,
                  const VRegister& vn,
                  const VRegister& vm)


### SQDMULH ###

Signed saturating doubling multiply element returning high half.

    void sqdmulh(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm,
                 int vm_index)


### SQDMULH ###

Signed saturating doubling multiply returning high half.

    void sqdmulh(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm)


### SQDMULL ###

Signed saturating double long multiply by element.

    void sqdmull(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm,
                 int vm_index)


### SQDMULL ###

Signed saturating doubling long multiply.

    void sqdmull(const VRegister& vd,
                 const VRegister& vn,
                 const VRegister& vm)


### SQDMULL2 ###

Signed saturating double long multiply by element (second part).

    void sqdmull2(const VRegister& vd,
                  const VRegister& vn,
                  const VRegister& vm,
                  int vm_index)


### SQDMULL2 ###

Signed saturating doubling long multiply (second part).

    void sqdmull2(const VRegister& vd,
                  const VRegister& vn,
                  const VRegister& vm)


### SQNEG ###

Signed saturating negate.

    void sqneg(const VRegister& vd,
               const VRegister& vn)


### SQRDMULH ###

Signed saturating rounding doubling multiply element returning high half.

    void sqrdmulh(const VRegister& vd,
                  const VRegister& vn,
                  const VRegister& vm,
                  int vm_index)


### SQRDMULH ###

Signed saturating rounding doubling multiply returning high half.

    void sqrdmulh(const VRegister& vd,
                  const VRegister& vn,
                  const VRegister& vm)


### SQRSHL ###

Signed saturating rounding shift left by register.

    void sqrshl(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### SQRSHRN ###

Signed saturating rounded shift right narrow by immediate.

    void sqrshrn(const VRegister& vd,
                 const VRegister& vn,
                 int shift)


### SQRSHRN2 ###

Signed saturating rounded shift right narrow by immediate (second part).

    void sqrshrn2(const VRegister& vd,
                  const VRegister& vn,
                  int shift)


### SQRSHRUN ###

Signed sat rounded shift right unsigned narrow by immediate.

    void sqrshrun(const VRegister& vd,
                  const VRegister& vn,
                  int shift)


### SQRSHRUN2 ###

Signed sat rounded shift right unsigned narrow by immediate (second part).

    void sqrshrun2(const VRegister& vd,
                   const VRegister& vn,
                   int shift)


### SQSHL ###

Signed saturating shift left by immediate.

    void sqshl(const VRegister& vd,
               const VRegister& vn,
               int shift)


### SQSHL ###

Signed saturating shift left by register.

    void sqshl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SQSHLU ###

Signed saturating shift left unsigned by immediate.

    void sqshlu(const VRegister& vd,
                const VRegister& vn,
                int shift)


### SQSHRN ###

Signed saturating shift right narrow by immediate.

    void sqshrn(const VRegister& vd,
                const VRegister& vn,
                int shift)


### SQSHRN2 ###

Signed saturating shift right narrow by immediate (second part).

    void sqshrn2(const VRegister& vd,
                 const VRegister& vn,
                 int shift)


### SQSHRUN ###

Signed saturating shift right unsigned narrow by immediate.

    void sqshrun(const VRegister& vd,
                 const VRegister& vn,
                 int shift)


### SQSHRUN2 ###

Signed saturating shift right unsigned narrow by immediate (second part).

    void sqshrun2(const VRegister& vd,
                  const VRegister& vn,
                  int shift)


### SQSUB ###

Signed saturating subtract.

    void sqsub(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SQXTN ###

Signed saturating extract narrow.

    void sqxtn(const VRegister& vd,
               const VRegister& vn)


### SQXTN2 ###

Signed saturating extract narrow (second part).

    void sqxtn2(const VRegister& vd,
                const VRegister& vn)


### SQXTUN ###

Signed saturating extract unsigned narrow.

    void sqxtun(const VRegister& vd,
                const VRegister& vn)


### SQXTUN2 ###

Signed saturating extract unsigned narrow (second part).

    void sqxtun2(const VRegister& vd,
                 const VRegister& vn)


### SRHADD ###

Signed rounding halving add.

    void srhadd(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### SRI ###

Shift right by immediate and insert.

    void sri(const VRegister& vd,
             const VRegister& vn,
             int shift)


### SRSHL ###

Signed rounding shift left by register.

    void srshl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SRSHR ###

Signed rounding shift right by immediate.

    void srshr(const VRegister& vd,
               const VRegister& vn,
               int shift)


### SRSRA ###

Signed rounding shift right by immediate and accumulate.

    void srsra(const VRegister& vd,
               const VRegister& vn,
               int shift)


### SSHL ###

Signed shift left by register.

    void sshl(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### SSHLL ###

Signed shift left long by immediate.

    void sshll(const VRegister& vd,
               const VRegister& vn,
               int shift)


### SSHLL2 ###

Signed shift left long by immediate (second part).

    void sshll2(const VRegister& vd,
                const VRegister& vn,
                int shift)


### SSHR ###

Signed shift right by immediate.

    void sshr(const VRegister& vd,
              const VRegister& vn,
              int shift)


### SSRA ###

Signed shift right by immediate and accumulate.

    void ssra(const VRegister& vd,
              const VRegister& vn,
              int shift)


### SSUBL ###

Signed subtract long.

    void ssubl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SSUBL2 ###

Signed subtract long (second part).

    void ssubl2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### SSUBW ###

Signed integer subtract wide.

    void ssubw(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SSUBW2 ###

Signed integer subtract wide (second part).

    void ssubw2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### ST1 ###

One-element single structure store from one lane.

    void st1(const VRegister& vt,
             int lane,
             const MemOperand& src)


### ST1 ###

One-element structure store from four registers.

    void st1(const VRegister& vt,
             const VRegister& vt2,
             const VRegister& vt3,
             const VRegister& vt4,
             const MemOperand& src)


### ST1 ###

One-element structure store from one register.

    void st1(const VRegister& vt,
             const MemOperand& src)


### ST1 ###

One-element structure store from three registers.

    void st1(const VRegister& vt,
             const VRegister& vt2,
             const VRegister& vt3,
             const MemOperand& src)


### ST1 ###

One-element structure store from two registers.

    void st1(const VRegister& vt,
             const VRegister& vt2,
             const MemOperand& src)


### ST2 ###

Two-element single structure store from two lanes.

    void st2(const VRegister& vt,
             const VRegister& vt2,
             int lane,
             const MemOperand& src)


### ST2 ###

Two-element structure store from two registers.

    void st2(const VRegister& vt,
             const VRegister& vt2,
             const MemOperand& src)


### ST3 ###

Three-element single structure store from three lanes.

    void st3(const VRegister& vt,
             const VRegister& vt2,
             const VRegister& vt3,
             int lane,
             const MemOperand& src)


### ST3 ###

Three-element structure store from three registers.

    void st3(const VRegister& vt,
             const VRegister& vt2,
             const VRegister& vt3,
             const MemOperand& src)


### ST4 ###

Four-element single structure store from four lanes.

    void st4(const VRegister& vt,
             const VRegister& vt2,
             const VRegister& vt3,
             const VRegister& vt4,
             int lane,
             const MemOperand& src)


### ST4 ###

Four-element structure store from four registers.

    void st4(const VRegister& vt,
             const VRegister& vt2,
             const VRegister& vt3,
             const VRegister& vt4,
             const MemOperand& src)


### SUB ###

Subtract.

    void sub(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### SUBHN ###

Subtract narrow returning high half.

    void subhn(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### SUBHN2 ###

Subtract narrow returning high half (second part).

    void subhn2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### SUQADD ###

Signed saturating accumulate of unsigned value.

    void suqadd(const VRegister& vd,
                const VRegister& vn)


### SXTL ###

Signed extend long.

    void sxtl(const VRegister& vd,
              const VRegister& vn)


### SXTL2 ###

Signed extend long (second part).

    void sxtl2(const VRegister& vd,
               const VRegister& vn)


### TBL ###

Table lookup from four registers.

    void tbl(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vn2,
             const VRegister& vn3,
             const VRegister& vn4,
             const VRegister& vm)


### TBL ###

Table lookup from one register.

    void tbl(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### TBL ###

Table lookup from three registers.

    void tbl(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vn2,
             const VRegister& vn3,
             const VRegister& vm)


### TBL ###

Table lookup from two registers.

    void tbl(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vn2,
             const VRegister& vm)


### TBX ###

Table lookup extension from four registers.

    void tbx(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vn2,
             const VRegister& vn3,
             const VRegister& vn4,
             const VRegister& vm)


### TBX ###

Table lookup extension from one register.

    void tbx(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vm)


### TBX ###

Table lookup extension from three registers.

    void tbx(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vn2,
             const VRegister& vn3,
             const VRegister& vm)


### TBX ###

Table lookup extension from two registers.

    void tbx(const VRegister& vd,
             const VRegister& vn,
             const VRegister& vn2,
             const VRegister& vm)


### TRN1 ###

Transpose vectors (primary).

    void trn1(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### TRN2 ###

Transpose vectors (secondary).

    void trn2(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### UABA ###

Unsigned absolute difference and accumulate.

    void uaba(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### UABAL ###

Unsigned absolute difference and accumulate long.

    void uabal(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UABAL2 ###

Unsigned absolute difference and accumulate long (second part).

    void uabal2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### UABD ###

Unsigned absolute difference.

    void uabd(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### UABDL ###

Unsigned absolute difference long.

    void uabdl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UABDL2 ###

Unsigned absolute difference long (second part).

    void uabdl2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### UADALP ###

Unsigned pairwise long add and accumulate.

    void uadalp(const VRegister& vd,
                const VRegister& vn)


### UADDL ###

Unsigned add long.

    void uaddl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UADDL2 ###

Unsigned add long (second part).

    void uaddl2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### UADDLP ###

Unsigned pairwise long add.

    void uaddlp(const VRegister& vd,
                const VRegister& vn)


### UADDLV ###

Unsigned add long across vector.

    void uaddlv(const VRegister& vd,
                const VRegister& vn)


### UADDW ###

Unsigned add wide.

    void uaddw(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UADDW2 ###

Unsigned add wide (second part).

    void uaddw2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### UCVTF ###

Convert unsigned integer or fixed point to FP.

    void ucvtf(const VRegister& fd, const Register& rn, int fbits = 0)


### UCVTF ###

Convert unsigned integer or fixed-point to FP.

    void ucvtf(const VRegister& fd, const VRegister& vn, int fbits = 0)


### UHADD ###

Unsigned halving add.

    void uhadd(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UHSUB ###

Unsigned halving sub.

    void uhsub(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UMAX ###

Unsigned maximum.

    void umax(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### UMAXP ###

Unsigned pairwise maximum.

    void umaxp(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UMAXV ###

Unsigned maximum across vector.

    void umaxv(const VRegister& vd,
               const VRegister& vn)


### UMIN ###

Unsigned minimum.

    void umin(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### UMINP ###

Unsigned pairwise minimum.

    void uminp(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UMINV ###

Unsigned minimum across vector.

    void uminv(const VRegister& vd,
               const VRegister& vn)


### UMLAL ###

Unsigned long multiply-add by scalar element.

    void umlal(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm,
               int vm_index)


### UMLAL ###

Unsigned long multiply-add.

    void umlal(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UMLAL2 ###

Unsigned long multiply-add (second part).

    void umlal2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### UMLAL2 ###

Unsigned long multiply-add by scalar element (second part).

    void umlal2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm,
                int vm_index)


### UMLSL ###

Unsigned long multiply-sub by scalar element.

    void umlsl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm,
               int vm_index)


### UMLSL ###

Unsigned long multiply-sub.

    void umlsl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UMLSL2 ###

Unsigned long multiply-sub (second part).

    void umlsl2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### UMLSL2 ###

Unsigned long multiply-sub by scalar element (second part).

    void umlsl2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm,
                int vm_index)


### UMOV ###

Unsigned move vector element to general-purpose register.

    void umov(const Register& rd,
              const VRegister& vn,
              int vn_index)


### UMULL ###

Unsigned long multiply by scalar element.

    void umull(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm,
               int vm_index)


### UMULL ###

Unsigned long multiply long.

    void umull(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UMULL2 ###

Unsigned long multiply (second part).

    void umull2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### UMULL2 ###

Unsigned long multiply by scalar element (second part).

    void umull2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm,
                int vm_index)


### UQADD ###

Unsigned saturating add.

    void uqadd(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UQRSHL ###

Unsigned saturating rounding shift left by register.

    void uqrshl(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### UQRSHRN ###

Unsigned saturating rounding shift right narrow by immediate.

    void uqrshrn(const VRegister& vd,
                 const VRegister& vn,
                 int shift)


### UQRSHRN2 ###

Unsigned saturating rounding shift right narrow by immediate (second part).

    void uqrshrn2(const VRegister& vd,
                  const VRegister& vn,
                  int shift)


### UQSHL ###

Unsigned saturating shift left by immediate.

    void uqshl(const VRegister& vd,
               const VRegister& vn,
               int shift)


### UQSHL ###

Unsigned saturating shift left by register.

    void uqshl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UQSHRN ###

Unsigned saturating shift right narrow by immediate.

    void uqshrn(const VRegister& vd,
                const VRegister& vn,
                int shift)


### UQSHRN2 ###

Unsigned saturating shift right narrow by immediate (second part).

    void uqshrn2(const VRegister& vd,
                 const VRegister& vn,
                 int shift)


### UQSUB ###

Unsigned saturating subtract.

    void uqsub(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### UQXTN ###

Unsigned saturating extract narrow.

    void uqxtn(const VRegister& vd,
               const VRegister& vn)


### UQXTN2 ###

Unsigned saturating extract narrow (second part).

    void uqxtn2(const VRegister& vd,
                const VRegister& vn)


### URECPE ###

Unsigned reciprocal estimate.

    void urecpe(const VRegister& vd,
                const VRegister& vn)


### URHADD ###

Unsigned rounding halving add.

    void urhadd(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### URSHL ###

Unsigned rounding shift left by register.

    void urshl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### URSHR ###

Unsigned rounding shift right by immediate.

    void urshr(const VRegister& vd,
               const VRegister& vn,
               int shift)


### URSQRTE ###

Unsigned reciprocal square root estimate.

    void ursqrte(const VRegister& vd,
                 const VRegister& vn)


### URSRA ###

Unsigned rounding shift right by immediate and accumulate.

    void ursra(const VRegister& vd,
               const VRegister& vn,
               int shift)


### USHL ###

Unsigned shift left by register.

    void ushl(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### USHLL ###

Unsigned shift left long by immediate.

    void ushll(const VRegister& vd,
               const VRegister& vn,
               int shift)


### USHLL2 ###

Unsigned shift left long by immediate (second part).

    void ushll2(const VRegister& vd,
                const VRegister& vn,
                int shift)


### USHR ###

Unsigned shift right by immediate.

    void ushr(const VRegister& vd,
              const VRegister& vn,
              int shift)


### USQADD ###

Unsigned saturating accumulate of signed value.

    void usqadd(const VRegister& vd,
                const VRegister& vn)


### USRA ###

Unsigned shift right by immediate and accumulate.

    void usra(const VRegister& vd,
              const VRegister& vn,
              int shift)


### USUBL ###

Unsigned subtract long.

    void usubl(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### USUBL2 ###

Unsigned subtract long (second part).

    void usubl2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### USUBW ###

Unsigned subtract wide.

    void usubw(const VRegister& vd,
               const VRegister& vn,
               const VRegister& vm)


### USUBW2 ###

Unsigned subtract wide (second part).

    void usubw2(const VRegister& vd,
                const VRegister& vn,
                const VRegister& vm)


### UXTL ###

Unsigned extend long.

    void uxtl(const VRegister& vd,
              const VRegister& vn)


### UXTL2 ###

Unsigned extend long (second part).

    void uxtl2(const VRegister& vd,
               const VRegister& vn)


### UZP1 ###

Unzip vectors (primary).

    void uzp1(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### UZP2 ###

Unzip vectors (secondary).

    void uzp2(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### XTN ###

Extract narrow.

    void xtn(const VRegister& vd,
             const VRegister& vn)


### XTN2 ###

Extract narrow (second part).

    void xtn2(const VRegister& vd,
              const VRegister& vn)


### ZIP1 ###

Zip vectors (primary).

    void zip1(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)


### ZIP2 ###

Zip vectors (secondary).

    void zip2(const VRegister& vd,
              const VRegister& vn,
              const VRegister& vm)



Additional or pseudo instructions
---------------------------------

### BIND ###

Bind a label to the current PC.

    void bind(Label* label)


### DC32 ###

Emit 32 bits of data into the instruction stream.

    void dc32(uint32_t data)


### DC64 ###

Emit 64 bits of data into the instruction stream.

    void dc64(uint64_t data)


### DCI ###

Emit raw instructions into the instruction stream.

    void dci(Instr raw_inst)


### PLACE ###

Place a literal at the current PC.

    void place(RawLiteral* literal)



