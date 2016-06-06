The proposed implementation’s objectives:
 * Avoid any new API at the assembler level
 * Consider the literal pool simply as a container of literals
 * Keep the literal pool at the macro-assembler level
 * The literal pool is public (can be used by the users)
 * The macro-assembler will have its own literal pool, and will manage
   it automatically
 * The macro-assembler is responsible for emitting its managed literal
   pool when needed
 * The assembler does not manage literals, but can place a literal in the
   code buffer

Contrary to AArch64, every single instruction handling labels has a
specific range which also depends on the ISA and the variant.
For example, ldrd's range is [-1020, 1020] for 32bit T32 and [-255, 255] for
A32.
But for ldr the range is [-4095, 4095] for A32, [0, 1020] for 16bit T32 and
[-4095, 4095] for 32bit T32.
So, the macro-assembler will have to wait for the instruction to be emitted
at the assembler level, as the assembler may choose the variant to emit, to
specify the maximum range the literal has to be placed.

The checkpoint is also dependent on an offset, specific to the ISA,
applied to the PC (r15). That offset is 4 in the T32 case and 8 in the A32
case. ARM ARM describes it as the "Architecture State Offset".
But also, in some cases, the PC is automatically aligned, referenced as
Align(PC, alignment) in the ARM ARM.

When a literal is added to the literal pool, it’s appended to the list of
literals and the pool’s size is updated.
When an instruction using a literal is emitted, the literal's checkpoint
is set, and depends on the variant that was used to emit the instruction.
The macro assembler then manages the checkpoint which defines when the
macro-assembler’s internal pool has to be emitted.
Broadly speaking, when that checkpoint is reached, the macro-assmbler's
literal pool is emitted. The emission will include an optional branch to let the
code execute around the pool:
        b after_pool
        .int 0xabcdef123
        .float 1.0
        ...
    after_pool:
The branch itself can be avoided if the previous instruction is an uncondition
branch (like b, tbb/tbh, ...)

We will emit the literal pool when one of the following conditions is met:
 1. The offset in the code-buffer has reached the checkpoint (including enough
    space for a branch). Every instruction has to perform the check, and only
    via the macro-assembler because the assembler has no knowledge of the literal
    pool,
 2. A literal is added, and the instruction range does not allow the
    placement at the end of the pool.
 3. The assembler emits an unconditional branch, a return from function or
    possibly any instruction which modifies pc ('mov pc, lr' or 'pop pc'
    which is popular in the T32 world). This will need to be evaluated
    (consider the size of the pool for example) to avoid trashing the
    I-cache by emitting too often, but some instructions have a very small
    range (ldrd for example), that this will be hard to avoid.

At the application level, one can use a literal pool, but such pool will
not be managed by the macro-assembler, and it’s the responsibility of the
application to emit it.
Which means that there will be no automatic mechanism or callback to
tell the application when it needs to be emitted. Whereas, the built-in
literal pool will be fully managed by the macro-assembler, and
automatically emitted when the macro-assembler detects that it can/should.
We believe it’s too complex for vixl to manage multiple literal pools
considering the sometimes small latitude given to emit the pools. But
that’s negotiable. For example, we could have one literal pool for small
ranges like ldrd and one other for bigger ranges.
Literals can also be created at the appplication level, and be placed in
the code at the assembler level, then used as labels for ldr* and all
the variants.

What still needs to be done with this version of the literal pools:
 • Have a notion of shared literal so that the literal can be reused
   even when the literal has been emitted

In the current implementation for AArch64 the literal pool is associated to a
macro-assembler, the literal may be associated to a literal pool, and the
assembler places the literal. If the literal is linked to literal pool, the
assembler will have callbacks in the macro-assembler (as sub class of the
assembler)... So, it’s pretty hard to manage all this. Here's a gdb trace:


    vixl::aarch64::MacroAssembler::Ldr (this, rt, imm=1311768467294899695)
    1488          literal = new Literal<uint64_t>(imm,
    1489                                          &literal_pool_,
    1490                                          RawLiteral::kDeletedOnPlacementByPool);

    1498        ldr(rt, literal);

      vixl::aarch64::Assembler::ldr (this, rt=..., literal)
      1693      ldr(rt, static_cast<int>(LinkAndGetWordOffsetTo(literal)));

        vixl::aarch64::Assembler::LinkAndGetWordOffsetTo (this=0x7fffffffd550, literal)
        637   literal->SetLastUse(GetCursorOffset());

          vixl::aarch64::RawLiteral::SetLastUse (this, offset=40) at src/vixl/aarch64/assembler-aarch64.h:1151
          1154        offset_ = -offset - 1;

        vixl::aarch64::Assembler::LinkAndGetWordOffsetTo (this, literal)
        640     literal->GetLiteralPool()->AddEntry(literal);

          vixl::aarch64::LiteralPool::AddEntry
          129   UpdateFirstUse(masm_->GetCursorOffset());

            vixl::aarch64::LiteralPool::UpdateFirstUse(this, use_position=40)
            140     SetNextRecommendedCheckpoint(NextRecommendedCheckpoint());

              vixl::aarch64::LiteralPool::NextRecommendedCheckpoint (this) at src/vixl/aarch64/macro-assembler-aarch64.h:155
              155   return first_use_ + kRecommendedLiteralPoolRange;

            vixl::aarch64::LiteralPool::SetNextRecommendedCheckpoint (this, offset=131112) at src/vixl/aarch64/macro-assembler-aarch64.h:3130
            3129      masm_->recommended_checkpoint_ =
            3130          std::min(masm_->recommended_checkpoint_, offset);
** Note that we've modified a *private* member of the MacroAssembler through a call to the assembler **
            3131      recommended_checkpoint_ = offset;

          vixl::aarch64::LiteralPool::AddEntry (this=0x7fffffffd598, literal=0x28d7f40)
          131   entries_.push_back(literal);
          132   size_ += literal->GetSize();
