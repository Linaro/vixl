VIXL Change Log
===============

* 1.2
    + Added support for `fmadd`, `fnmadd`, `fnmsub`, `fminnm`, `fmaxnm`,
      `frinta`, `fcvtau` and `fcvtas`.
    + Added support for assembling and disassembling `isb`, `dsb` and `dmb`.
    + Added support for automatic inversion of compare instructions when using
      negative immediates.
    + Added support for using `movn` when generating immediates.
    + Added explicit flag-setting 'S' instructions, and removed
      `SetFlags` and `LeaveFlags` arguments.
    + Added support for `Movk` in macro assembler.
    + Added support for W register parameters to `Tbz` and `Tbnz`.
    + Added support for using immediate operands with `Csel`.
    + Added new debugger syntax for memory inspection.
    + Fixed `smull`, `fmsub` and `sdiv` simulation.
    + Fixed sign extension for W->X conversions using `sxtb`, `sxth` and `sxtw`.
    + Prevented code generation for certain side-effect free operations,
      such as `add r, r, #0`, in the macro assembler.
    + Other small bug fixes.

* 1.1
    + Improved robustness of instruction decoder and disassembler.
    + Added support for double-to-float conversions using `fcvt`.
    + Added support for more fixed-point to floating-point conversions (`ucvtf`
      and `scvtf`).
    + Added instruction statistics collection class `instrument-a64.cc`.

* 1.0
    + Initial release.
