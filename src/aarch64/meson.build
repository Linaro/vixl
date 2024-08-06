# SPDX-FileCopyrightText: 2021 VIXL authors
# SPDX-License-Identifier: BSD-3-Clause

# Need to wrap the filenames in files() otherwise this array would be treated
# as a simple array of strings, and when used in the master meson.build they
# would refer to nonexistent paths. Wrapping in files() ensures that the
# filenames will be always relative to this directory, even if referenced in
# a different one. As a general rule, when I need to refer to a file from a
# different build directory I should wrap it in files().

target_sources += files(
  'assembler-aarch64.cc',
  'assembler-sve-aarch64.cc',
  'cpu-aarch64.cc',
  'cpu-features-auditor-aarch64.cc',
  'debugger-aarch64.cc',
  'decoder-aarch64.cc',
  'disasm-aarch64.cc',
  'instructions-aarch64.cc',
  'logic-aarch64.cc',
  'macro-assembler-aarch64.cc',
  'macro-assembler-sve-aarch64.cc',
  'operands-aarch64.cc',
  'pointer-auth-aarch64.cc',
  'registers-aarch64.cc',
  'simulator-aarch64.cc'
)

install_headers(
  'abi-aarch64.h',
  'assembler-aarch64.h',
  'constants-aarch64.h',
  'cpu-aarch64.h',
  'cpu-features-auditor-aarch64.h',
  'debugger-aarch64.h',
  'decoder-aarch64.h',
  'decoder-constants-aarch64.h',
  'decoder-visitor-map-aarch64.h',
  'disasm-aarch64.h',
  'instructions-aarch64.h',
  'macro-assembler-aarch64.h',
  'operands-aarch64.h',
  'registers-aarch64.h',
  'simulator-aarch64.h',
  'simulator-constants-aarch64.h',
  subdir: 'vixl'/'aarch64'
)
