# SPDX-FileCopyrightText: 2021 VIXL authors
# SPDX-License-Identifier: BSD-3-Clause

custom_target(
  'doc_aarch32',
  command: [markdown, '@INPUT@'],
  input: 'getting-started-aarch32.md',
  output: '@BASENAME@.html',
  capture: true,
  install: true,
  install_dir: doc_dir/'aarch32'
)
