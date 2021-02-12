Text Assembler C++ code generator
=================================

Overview
========
Functionality of the code in this directory is to generate Text Assembler C++
header files with

 1. **parse-xml.py**: Script that takes xml file from reference manual as an
    input. Produces list of instructions in syntax suitable for the code
    generating script.
 2. **instruction-add.py**: Code generating script. Takes list of instructions
    as an input and compares it with instructions supported by VIXL. Produces
    header files with instructions that are in both sets.
 3. **./tasm-template/*.jinja2**: Template files that are used to generate
    proper header files.


Requirements
============

To run both scripts the following dependencies are required:

 1. Python 3.6 or later
 2. Jinja2

Last dependency is a python library that can be installed with pip.
For example:

    * $ python3 -m pip install jinja2


Usage
=====

**parse-xml.py** should be used to generate a list of instructions. By default
it takes content of the [xml] file from stdin and prints instructions to stdout.
There are flags that allow to use files. For example:

    * $ cat onebigfile.xml | ./parse-xml.py > onebigfile.parsed
    * $ ./parse-xml.py --in-file onebigfile.xml --out-file onebigfile.parsed

**instruction-add.py** should be used to generate Text Assembler headers.
The only argument is a path to the file with the list of instructions.
Standard usage:

    * ./instruction-add.py onebigfile.parsed

Output header files are created in directory ./tasm-template/.

Verbosity can be increased with -v and -vv. On each level additional logs are
printed on standard output.

[xml]: https://developer.arm.com/architectures/cpu-architecture/a-profile/exploration-tools