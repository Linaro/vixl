# MacroAssembler and Assembler Code Generation Behaviours

This design document describes the way that the Assembler and MacroAssembler
generate code. This has wide implications, including buffer management,
instruction-precise generation and related control over code generation.

**NOTE**: This document describes design decisions, but the code does not
implement or match everything that is described in this document.
TODO: Work on the code to achieve what is expressed in this document, and update
the documentation.

## Basic Use-Cases

### Simple Code Generation

For normal code generation, the MacroAssembler should be used. We recommend this
even if the caller doesn't require the macro behaviour, because it acts as a
(partial) fail-safe in case calling code accidentally passes out-of-range
immediates and suchlike. The MacroAssembler is also able to check and emit
literal pools.

Note that the MacroAssembler is allowed to emit an arbitrary amount of code in
order to achieve the requested effect. (Note that "arbitrary" includes no code,
for macros with no effect other than to advance the PC.) In practical terms, a
macro can effectively generate a huge amount of code if it needs to emit a
literal pool (for example).

### Precise Code Generation

Sometimes, the caller needs to generate a very precise code sequence. The
typical use-case is where code needs to be patched. In these cases, the
MacroAssembler must not be used, but the Assembler can be called directly. Since
the caller most likely has a MacroAssembler object, we provide
ExactAssemblyScope to restrict any MacroAssembler methods from being used for
its duration, and allow access to the Assembler.

### Fuzzy Cases: Approximate Size Limits

Sometimes the caller wants the convenience and fail-safe features of the
MacroAssembler, and isn't worried about the precise code sequence used, but
needs to ensure that the total code size does not exceed the range of a branch
(or similar PC-relative instruction). Veneers simplify many cases like this, but
not all (and not necessarily optimally). For example, VIXL32's Switch-Case
macros probably do not have sufficient range to cope with a literal pool in
the middle.

For these cases, we'd like to ensure that the MacroAssembler doesn't emit _too
much_ code. This is very fuzzy, and in practice means avoiding pools, but
allowing standard macros. The catch is that the caller must specify the upper
limit on the size of the generated code.

A corner-case that is relevant for VIXL32 (but mostly irrelevant for VIXL64) is
that the protected region could easily be larger than the range of some
load-literal instructions, so we should not actually _block_ the pool. For
example, `vldr` has a range of about 1KB, but `tbh` can easily exceed this
range. If one of the Cases generates an FP literal-load, the MacroAssembler
needs to put the pool in the middle of the Switch-Case sequence.

This case is currently accomodated by MacroAssembler::EnsureEmitFor.

## Proposal

These behaviours are similar to (or the same as) existing cases to avoid
breaking backwards compatibility. Several potentially-unsafe scopes have been
deprecated, and a few have been given more flexibility.

Each scope utility will behave in the same way for VIXL64 as for VIXL32, even if
the implementations differ.

### `CodeBufferCheckScope(Assembler* assm, size_t size, ...)`

- Allow code emission from the specified `Assembler`.
- Optionally reserve space in the `CodeBuffer` (if it is managed by VIXL).
- Optionally, on destruction, check the size of the generated code.
  (The size can be either exact or a maximum size.)

This scope exists so that callers can use an Assembler by itself, without even
instantiating a MacroAssembler.

### `CodeBufferCheckScope(MacroAssembler* masm, ...)`

- DEPRECATED

Otherwise, this is the same as `CodeBufferCheckScope(Assembler*)`.

It is unfortunate that this scope allows the Assembler and MacroAssembler to be
mixed freely; this can cause numerous problems. For example, the Assembler
doesn't know about the pools, so use of the Assembler can push the pools out of
range. This was acceptable in VIXL64, where the pool range is very large, but
not in VIXL32.

We should retain the existing functionality for a while, but mark the
`MacroAssembler*` form as DEPRECATED. A suitable replacement is
EmissionCheckScope, which allows the Assembler and MacroAssembler to be mixed,
but also blocks pools and therefore avoids the problems that
`CodeBufferCheckScope` has.

### `EmissionCheckScope(MacroAssembler* masm, size_t size, AssertPolicy ...)`

- Do the same as `CodeBufferCheckSCope`, but:
  - If managed by VIXL, always reserve space in the `CodeBuffer`.
  - Always check the (exact or maximum) size of the generated code on
    destruction.
- Emit pools if the specified size would push them out of range.
- Block pools emission for the duration of the scope.

This scope allows the `Assembler` and `MacroAssembler` to be freely and safely
mixed for its duration.

The MacroAssembler uses this to implement its own macros.

### `ExactAssemblyScope(MacroAssembler* masm, ...)`

- Do the same as `EmissionCheckScope`.
- Block access to the MacroAssemblerInterface (using run-time assertions).

This scope allows safely generating exact assembly code. Compared to
`CodeBufferCheckScope`, it disables the `MacroAssembler`, and guarantees that
no pools will be emitted during code generation.

This replaces VIXL64's InstructionAccurateScope.

### `BlockPoolsScope` (and variants)

- DEPRECATED
- Block the pools for the duration.

These scopes really shouldn't be used outside VIXL itself. Since uses inside
VIXL are minimal, we should mark it as DEPRECATED and replace our own uses with
EmissionCheckScope or manual `MacroAssembler::BlockPools()` calls.

Note that this scope made sense in VIXL64, where pool ranges are large and we
have a large contingency region built into the pool checks. In VIXL32, where the
ranges are tight, we can't generally afford to block the constant pools at
arbitrary points, even for short sequences of instructions.

### `InstructionAccurateScope`

- DEPRECATED
- Replaced by ExactAssemblyScope.

When generating T32, we need something like InstructionAccurateScope to check
the code _size_, rather than the instruction count, since the instruction size
is much more likely to vary in a way that matters. However, it's not safe to
just change `InstructionAccurateScope`'s behaviour because the constructor
prototype would be unchanged, so there would be no compile-time warning for
users.

### `MacroAssembler::EnsureEmitFor`

- Private to the MacroAssembler (but available, in a DEPRECATED form, to
  VIXL64 users).
- Ensure that there is space in the CodeBuffer so that `size` bytes can be
  emitted contiguously.

Pools are emitted if `size` bytes would push them out of range, but they are not
actually blocked; pools can still be emitted during the specified range if they
are used during the range.

    __ EnsureEmitFor(4096);   // Might dump pools.
    __ Add(...);
    __ Add(...);              // These macros will not dump pools. They might
    __ Add(...);              // emit multiple instructions.
    __ Add(...);
    __ Vldr(d0, 12345.0);     // Adds an entry to the literal pool (range ~1KB).
    __ Add(...);
    ...
    __ Add(...);
    __ Add(...);              // The pool containing 12345.0 will be dumped
    __ Add(...);              // before the end of the EnsureEmitFor range.
    __ Add(...);

This is a one-shot call, not a scope utility, so there is no size checking
available. For that reason, it is risky, but still useful in certain cases.
There are also tricky corner-cases to consider. Most notably, if literals are
added to the pool during the `EnsureEmitFor` range, a pool might still be
generated in that range. This can be avoided by including the size of new
literals in the size check, but because this is not a scope utility and has not
destruction checks, we cannot assert that the usage was safe.

Also note that this does not acquire the CodeBuffer, so it is not possible to
use the Assembler after using this utility alone.

## Usage Examples

### Basic Usage

    void fn(MacroAssembler* masm) {
      // - Uses delegates if necessary.
      // - Arbitrary length (including 0, potentially).
      // - Can automatically reserve space in the code buffer.
      // - Can automatically dump pools.
      masm->Add(...);
    }

    void fn(MacroAssembler* masm) {
      // - No delegates allowed.
      //   - If a delegate is called, it should crash even in release mode.
      //     (This helps to avoid security bugs derived from data-dependent code
      //     generation.)
      // - Always generates exactly one instruction.
      // - No automatic buffer growth, but does check that there is space. (In
      //   VIXL64, this is done by the CodeBuffer.)
      // - In VIXL64, this requires that the code buffer has been "acquired".
      //   Any of the EmissionCheckScopes can do this, as can ExactAssemblyScope
      //   and CodeBufferCheckScope.
      SingleEmissionCheckScope(masm);
      masm->add(...);
    }

    void fn(Assembler* assm) {
      // Identical to the MacroAssembler::add example, except that we must use
      // CodeBufferCheckScope to acquire the buffer.
      CodeBufferCheckScope(assm, ...);
      assm->add(...);
    }

### Macros: Simple Code Generation

    void MacroAssembler::Add(...) {
      // A macro no larger than
      // `MacroEmissionCheckScope::kTypicalMacroInstructionMaxSize`.
      MacroEmissionCheckScope scope(...);
      ...
    }

    void MacroAssembler::Printf(...) {
      // A macro larger than
      // `MacroEmissionCheckScope::kTypicalMacroInstructionMaxSize`.
      // We start no scope, but rely only on upper-case macros which create
      // their own MacroEmissionCheckScopes. Pools can be emitted during
      // this large macro.
      Add(...)
      Ldr(...)
      ...
    }

### Patchable Regions: Precise Code Generation

    void fn(MacroAssembler* masm) {
      __ Add(...);
      __ Add(...);
      __ Add(...);
      {
        // We want this sequence of instructions to be patched later, so we need
        // to use instruction-accurate code generation with a predictable size.
        // It is forbidden to use macros during this scope.
        ExactAssemblyScope(masm, 4 * kInstructionSize);
        __ bind(&patch_location);
        __ add(...);
        __ add(...);
        __ add(...);
        __ add(...);
      }
      __ Add(...);
      __ Add(...);
      __ Add(...);
    }
