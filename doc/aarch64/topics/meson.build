# SPDX-FileCopyrightText: 2021 VIXL authors
# SPDX-License-Identifier: BSD-3-Clause

doc_aarch64_topics_files = [
  'extending-the-disassembler',
  'index',
  'state-trace',
  'ycm'
]

foreach file : doc_aarch64_topics_files
  custom_target(
    'doc_aarch64_topics_' + file,
    command: [markdown, '@INPUT@'],
    input: file + '.md',
    output: file + '.html',
    capture: true,
    install: true,
    install_dir: doc_dir/'aarch64'/'topics'
  )
endforeach
