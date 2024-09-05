# SPDX-FileCopyrightText: 2021 VIXL authors
# SPDX-License-Identifier: BSD-3-Clause

doc_aarch64_files = [
  'getting-started-aarch64',
  'supported-instructions-aarch64'
]

foreach file : doc_aarch64_files
  custom_target(
    'doc_aarch64_' + file,
    command: [markdown, '@INPUT@'],
    input: file + '.md',
    output: file + '.html',
    capture: true,
    install: true,
    install_dir: doc_dir/'aarch64'
  )
endforeach

subdir('topics')
