VIXL: AArch64 Runtime Code Generation Library Version 1.4
=========================================================

Contents:

 * Requirements
 * Overview
 * Known limitations
 * Usage


Requirements
============

To build VIXL the following software is required:

 1. Python 2.7
 2. SCons 2.0
 3. GCC 4.4+

A 64-bit host machine is required, implementing an LP64 data model. VIXL has
only been tested using GCC on Ubuntu systems.

To run the linter stage of the tests, the following software is also required:

 1. Git
 2. [Google's `cpplint.py`][cpplint]

Refer to the 'Usage' section for details.


Overview
========

VIXL is made of three components.

 1. A programmatic assembler to generate A64 code at runtime. The assembler
    abstracts some of the constraints of the A64 ISA, for example most
    instructions support any immediate.
 2. A disassembler which can print any instruction emitted by the assembler.
 3. A simulator which can simulate any instruction emitted by the assembler.
    The simulator allows generated code to be run on another architecture
    without the need for a full ISA model.

The VIXL git repository can be found [on GitHub][vixl]. Changes from previous
versions of VIXL can be found in the [Changelog](doc/changelog.md).


Known Limitations
=================

VIXL was developed to target JavaScript engines so a number of features from A64
were deemed unnecessary:

 * No Advanced SIMD support.
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
    executable) on the `PATH`. Only revision 104 has been tested with VIXL.

It is possible to tell `tools/presubmit.py` to skip the linter stage by passing
`--nolint`. This removes the dependency on `cpplint.py` and Git. The `--nolint`
option is implied if the VIXL project is a snapshot (with no `.git` directory).


Building and Running the Benchmarks
-----------------------------------

There are two very basic benchmarks provided with VIXL:

 1. bench\_dataop, emitting adds
 2. bench\_branch, emitting branches

To build one benchmark: `scons target=bench_xxx`, then run it as
`./bench_xxx_sim <number of iterations>`. The benchmarks do not report a
figure; they should be timed using the `time` command.


Getting Started
---------------

A short introduction to using VIXL can be found [here](doc/getting-started.md).
Example source code is provided in the `examples` directory. Build this using
`scons target=examples` from the root directory.



[cpplint]: https://google-styleguide.googlecode.com/svn-history/r104/trunk/cpplint/cpplint.py
           "Google's cpplint.py script."

[vixl]: https://github.com/armvixl/vixl
        "The VIXL repository on GitHub."
