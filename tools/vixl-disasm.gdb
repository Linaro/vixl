# Copyright 2022, VIXL authors
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   * Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#   * Neither the name of ARM Limited nor the names of its contributors may be
#     used to endorse or promote products derived from this software without
#     specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


# GDB commands for conveniently invoking VIXL's `disasm` example from within a
# debugging session. This is particularly helpful if your GDB doesn't support
# the ISA you want to examine (but VIXL does).
#
#   - Build VIXL's `examples_aarch64` target. Only the disassembler is required,
#     and the example only supports AArch64, so build with something like this:
#
#       $ scons simulator=none target=aarch64 ...
#
#   - Then, either:
#       - Copy or symlink the `disasm` example onto your PATH as `vixl-disasm`,
#       - ... or update $vixl_disasm_path to point to it.
#
#   - Start GDB, then source this script:
#
#       (gdb) source <path>/vixl-disasm.gdb
#       (gdb) help vixl-disasm-a64
#
# WARNING
#
# This script has numerous caveats and issues, and is not production quality.
# See help text for details. However, it's better than nothing in a constrained
# environment. A Python implementation could allow for a nicer interface, but is
# not supported by CheriBSD's GDB.

set $vixl_disasm_path = "vixl-disasm"

define vixl-disasm-impl
  set logging off
  set logging file /tmp/vixl-disasm-gdb
  set logging overwrite on
  set logging redirect on
  set $isa = $arg0
  # Cast away capability metadata.
  set $address = (unsigned long)$arg1
  set $count = $arg2
  if $count < 0
    set $count = -$count
    set $address = $address - $count * 4
  end
  set logging on
  # Double quotes will be logged, but we'll strip them later.
  output $vixl_disasm_path
  output " --"
  output/s $isa
  output " --start-at "
  output/x $address
  set $n = 0
  while $n < $count
    output " "
    eval "output/z ((unsigned int*)($address))[%d]", $n
    set $n = $n + 1
  end
  set logging off
  set logging overwrite off
  set logging redirect off
  set logging file gdb.txt
  shell eval "$(cat /tmp/vixl-disasm-gdb | tr -d '\n' | sed -e 's/\"//g')"
end


define vixl-disasm-a64
  if ($argc == 0)
    # (no args)
    vixl-disasm-impl "a64" $pc 1
  end
  if ($argc == 1)
    # COUNT
    vixl-disasm-impl "a64" $arg0 1
  end
  if ($argc == 2)
    # COUNT ADDRESS
    vixl-disasm-impl "a64" $arg0 $arg1
  end
  if ($argc > 2)
    echo Too many arguments.\n
  end
end

document vixl-disasm-a64
Disassemble A64 instructions using VIXL: vixl-disasm-a64 [ADDRESS [COUNT]]

  - ADDRESS is the start point (or the end point, for negative COUNTs). The
    default is $pc.

  - COUNT is a number of instructions to disassemble, and defaults to 1. If
    negative, instructions up to (but not including) ADDRESS are disassembled.

CAVEATS

  - This relies upon (and resets) GDB's "logging" feature.

  - If there are any errors, for example if the memory is inaccessible, the
    command will abort. This both fails silently, and leaves the "logging" hack
    enabled. It does not seem possible to recover from this automatically from
    GDB script. The easiest way to return to normal is:

      (gdb) vixl-disasm-a64 ...    # Failing command, no output.
      (gdb) set logging off
      ...
end


define vixl-disasm-c64
  if ($argc == 0)
    # (no args)
    vixl-disasm-impl "c64" $pc 1
  end
  if ($argc == 1)
    # COUNT
    vixl-disasm-impl "c64" $arg0 1
  end
  if ($argc == 2)
    # COUNT ADDRESS
    vixl-disasm-impl "c64" $arg0 $arg1
  end
  if ($argc > 2)
    echo Too many arguments.\n
  end
end

document vixl-disasm-c64
Disassemble C64 instructions using VIXL: vixl-disasm-c64 [ADDRESS [COUNT]]

  - ADDRESS is the start point (or the end point, for negative COUNTs). The
    default is $pc.

  - COUNT is a number of instructions to disassemble, and defaults to 1. If
    negative, instructions up to (but not including) ADDRESS are disassembled.

CAVEATS

  - This relies upon (and resets) GDB's "logging" feature.

  - If there are any errors, for example if the memory is inaccessible, the
    command will abort. This both fails silently, and leaves the "logging" hack
    enabled. It does not seem possible to recover from this automatically from
    GDB script. The easiest way to return to normal is:

      (gdb) vixl-disasm-c64 ...    # Failing command, no output.
      (gdb) set logging off
      ...
end
