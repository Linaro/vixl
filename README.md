VIXL: AArch64 Runtime Code Generation Library Version 1.9
=========================================================

Contents:

 * Overview
 * Requirements
 * Known limitations
 * Usage


Overview
========

VIXL contains three components.

 1. A programmatic **assembler** to generate A64 code at runtime. The assembler
    abstracts some of the constraints of the A64 ISA; for example, most
    instructions support any immediate.
 2. A **disassembler** that can print any instruction emitted by the assembler.
 3. A **simulator** that can simulate any instruction emitted by the assembler.
    The simulator allows generated code to be run on another architecture
    without the need for a full ISA model.

The VIXL git repository can be found [on GitHub][vixl].

Changes from previous versions of VIXL can be found in the
[Changelog](doc/changelog.md).


Requirements
============

To build VIXL the following software is required:

 1. Python 2.7
 2. SCons 2.0
 3. GCC 4.8+ or Clang 3.4+

A 64-bit host machine is required, implementing an LP64 data model. VIXL has
been tested using GCC on AArch64 Debian, GCC and Clang on amd64 Ubuntu
systems.

To run the linter stage of the tests, the following software is also required:

 1. Git
 2. [Google's `cpplint.py`][cpplint]

Refer to the 'Usage' section for details.


Known Limitations
=================

VIXL was developed for JavaScript engines so a number of features from A64 were
deemed unnecessary:

 * Limited rounding mode support for floating point.
 * Limited support for synchronisation instructions.
 * Limited support for system instructions.
 * A few miscellaneous integer and floating point instructions are missing.

The VIXL simulator supports only those instructions that the VIXL assembler can
generate. The `doc` directory contains a
[list of supported instructions](doc/supported-instructions.md).

The VIXL simulator was developed to run on 64-bit amd64 platforms. Whilst it
builds and mostly works for 32-bit x86 platforms, there are a number of
floating-point operations which do not work correctly, and a number of tests
fail as a result.

Debug Builds
------------

Your project's build system must define `VIXL_DEBUG` (eg. `-DVIXL_DEBUG`)
when using a VIXL library that has been built with debug enabled.

Some classes defined in VIXL header files contain fields that are only present
in debug builds, so if `VIXL_DEBUG` is defined when the library is built, but
not defined for the header files included in your project, you will see runtime
failures.

Exclusive-Access Instructions
-----------------------------

All exclusive-access instructions are supported, but the simulator cannot
accurately simulate their behaviour as described in the ARMv8 Architecture
Reference Manual.

 * A local monitor is simulated, so simulated exclusive loads and stores execute
   as expected in a single-threaded environment.
 * The global monitor is simulated by occasionally causing exclusive-access
   instructions to fail regardless of the local monitor state.
 * Load-acquire, store-release semantics are approximated by issuing a host
   memory barrier after loads or before stores. The built-in
   `__sync_synchronize()` is used for this purpose.

The simulator tries to be strict, and implements the following restrictions that
the ARMv8 ARM allows:

 * A pair of load-/store-exclusive instructions will only succeed if they have
   the same address and access size.
 * Most of the time, cache-maintenance operations or explicit memory accesses
   will clear the exclusive monitor.
    * To ensure that simulated code does not depend on this behaviour, the
      exclusive monitor will sometimes be left intact after these instructions.

Instructions affected by these limitations:
  `stxrb`, `stxrh`, `stxr`, `ldxrb`, `ldxrh`, `ldxr`, `stxp`, `ldxp`, `stlxrb`,
  `stlxrh`, `stlxr`, `ldaxrb`, `ldaxrh`, `ldaxr`, `stlxp`, `ldaxp`, `stlrb`,
  `stlrh`, `stlr`, `ldarb`, `ldarh`, `ldar`, `clrex`.


Usage
=====

Running all Tests
-----------------

The helper script `tools/presubmit.py` will build and run every test that is
provided with VIXL, in both release and debug mode. It is a useful script for
verifying that all of VIXL's dependencies are in place and that VIXL is working
as it should.

By default, the `tools/presubmit.py` script runs a linter to check that the
source code conforms with the code style guide, and to detect several common
errors that the compiler may not warn about. This is most useful for VIXL
developers. The linter has the following dependencies:

 1. Git must be installed, and the VIXL project must be in a valid Git
    repository, such as one produced using `git clone`.
 2. `cpplint.py`, [as provided by Google][cpplint], must be available (and
    executable) on the `PATH`.

It is possible to tell `tools/presubmit.py` to skip the linter stage by passing
`--nolint`. This removes the dependency on `cpplint.py` and Git. The `--nolint`
option is implied if the VIXL project is a snapshot (with no `.git` directory).


Building and Running the Benchmarks
-----------------------------------

There are three very basic benchmarks provided with VIXL:

 1. bench-dataop, emitting adds
 2. bench-branch, emitting branches
 3. bench-branch-link, emitting branch-links

Build these benchmarks using `scons bench-dataop`, `scons bench-branch` and
`scons bench-branch-link`. This will produce binaries called
`bench-dataop_sim`, `bench-branch_sim` and `bench-branch-link_sim`. Run these
with an iteration count argument, for example `./bench-dataop_sim 10000000`. The
benchmarks do not report a result; time them using the UNIX `time` command.

Build the benchmarks natively for execution on an AArch64 target using `scons
<benchmark name> simulator=off`. This will produce binaries called
`bench-dataop`, `bench-branch` and `bench-branch-link`. Run and time these in
the same way as the simulator versions.


Getting Started
---------------

A short introduction to using VIXL can be found [here](doc/getting-started.md).
Example source code is provided in the [examples](examples) directory. You can
build all the examples with `scons examples` from the root directory, or use
`scons --help` to get a detailed list of available build targets.


Using VIXL
----------

In addition to [getting started](doc/getting-started.md) and the
[examples](examples), you can find documentation and guides on various topics
that may be helpful [here](doc/topics/index.md).





[cpplint]: http://google-styleguide.googlecode.com/svn/trunk/cpplint/cpplint.py
           "Google's cpplint.py script."

[vixl]: https://github.com/armvixl/vixl
        "The VIXL repository on GitHub."
