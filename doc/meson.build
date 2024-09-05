# SPDX-FileCopyrightText: 2021 VIXL authors
# SPDX-License-Identifier: BSD-3-Clause

doc_dir = get_option('datadir')/'doc'/meson.project_name()

custom_target(
  'doc',
  command: [markdown, '@INPUT@'],
  input: '..'/'README.md',
  output: '@BASENAME@.html',
  capture: true,
  install: true,
  install_dir: doc_dir
)

if build_a32 or build_t32
  subdir('aarch32')
endif
if build_a64
  subdir('aarch64')
endif
