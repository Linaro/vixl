# SPDX-FileCopyrightText: 2021 VIXL authors
# SPDX-License-Identifier: BSD-3-Clause

# Need to wrap the filenames in files() otherwise this array would be treated
# as a simple array of strings, and when used in the master meson.build they
# would refer to nonexistent paths. Wrapping in files() ensures that the
# filenames will be always relative to this directory, even if referenced in
# a different one. As a general rule, when I need to refer to a file from a
# different build directory I should wrap it in files().

target_sources += files(
  'assembler-aarch32.cc',
  'constants-aarch32.cc',
  'disasm-aarch32.cc',
  'instructions-aarch32.cc',
  'location-aarch32.cc',
  'macro-assembler-aarch32.cc',
  'operands-aarch32.cc'
)

install_headers(
  'assembler-aarch32.h',
  'constants-aarch32.h',
  'disasm-aarch32.h',
  'instructions-aarch32.h',
  'location-aarch32.h',
  'macro-assembler-aarch32.h',
  'operands-aarch32.h',
  subdir: 'vixl'/'aarch32'
)
