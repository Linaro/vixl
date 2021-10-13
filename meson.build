# SPDX-FileCopyrightText: 2021 VIXL authors
# SPDX-License-Identifier: BSD-3-Clause

project(
  'vixl',
  'cpp',
  default_options: [
    'cpp_std=c++17',
    'buildtype=release',
    'warning_level=3',
    'werror=true',
    'd_ndebug=if-release',
    'b_lto=true'
  ],
  license: 'BSD-3-Clause',
  meson_version: '>=0.50.0',
  version: '7.0.0',
)

extra_args = []

if get_option('debug')
  extra_args += '-DVIXL_DEBUG'
endif

hosts_32bit = ['arc', 'arm', 'c2000', 'csky', 'mips', 'ppc', 'riscv32', 'rx', 'sparc', 'wasm32', 'x86']
can_target_aarch64 = not (host_machine.cpu_family() in hosts_32bit)

build_a32 = false
build_t32 = false
build_a64 = false

targets = get_option('target')
if 'auto' in targets or 'all' in targets
  if can_target_aarch64 or 'all' in targets
    extra_args += [
      '-DVIXL_INCLUDE_TARGET_A32',
      '-DVIXL_INCLUDE_TARGET_T32',
      '-DVIXL_INCLUDE_TARGET_A64'
    ]
    build_a32 = true
    build_t32 = true
    build_a64 = true
  else
    extra_args += [
      '-DVIXL_INCLUDE_TARGET_A32',
      '-DVIXL_INCLUDE_TARGET_T32'
    ]
    build_a32 = true
    build_t32 = true
  endif
else
  if 'a32' in targets or 'aarch32' in targets
    extra_args += [
      '-DVIXL_INCLUDE_TARGET_A32'
    ]
    build_a32 = true
  endif
  if 't32' in targets or 'aarch32' in targets
    extra_args += [
      '-DVIXL_INCLUDE_TARGET_T32'
    ]
    build_t32 = true
  endif
  if 'a64' in targets or 'aarch64' in targets
    extra_args += [
      '-DVIXL_INCLUDE_TARGET_A64'
    ]
    build_a64 = true
  endif
endif

target_sources = []
if build_a32 or build_t32
  subdir('src'/'aarch32')
endif
if build_a64
  subdir('src'/'aarch64')
endif

if get_option('simulator') == 'auto'
  if not (host_machine.cpu_family() == 'aarch64') and can_target_aarch64
    extra_args += '-DVIXL_INCLUDE_SIMULATOR_AARCH64'
  endif
elif get_option('simulator') == 'aarch64'
  if can_target_aarch64 and build_a64
    extra_args += '-DVIXL_INCLUDE_SIMULATOR_AARCH64'
  else
    error('Building an AArch64 simulator implies that VIXL targets AArch64. Set `target` to include `aarch64` or `a64`.')
  endif
endif

allocator = get_option('code_buffer_allocator')
if (allocator == 'auto' and host_machine.system() == 'linux') or allocator == 'mmap'
  extra_args += '-DVIXL_CODE_BUFFER_MMAP'
else
  extra_args += '-DVIXL_CODE_BUFFER_MALLOC'
endif

markdown = find_program('markdown', required: get_option('doc'))
if markdown.found()
  subdir('doc')
endif

libvixl = library(
  'vixl',
  'src'/'code-buffer-vixl.cc',
  'src'/'compiler-intrinsics-vixl.cc',
  'src'/'cpu-features.cc',
  'src'/'utils-vixl.cc',
  cpp_args: extra_args,
  include_directories: 'src',
  install: true,
  sources: target_sources,
  version: meson.project_version()
)

vixl_dep = declare_dependency(
  compile_args: extra_args,
  include_directories: 'src',
  link_with: libvixl
)

if meson.version().version_compare('>=0.54.0')
  meson.override_dependency('vixl', vixl_dep)
endif

install_headers(
  'src'/'assembler-base-vixl.h',
  'src'/'code-buffer-vixl.h',
  'src'/'code-generation-scopes-vixl.h',
  'src'/'compiler-intrinsics-vixl.h',
  'src'/'cpu-features.h',
  'src'/'globals-vixl.h',
  'src'/'invalset-vixl.h',
  'src'/'macro-assembler-interface.h',
  'src'/'platform-vixl.h',
  'src'/'pool-manager-impl.h',
  'src'/'pool-manager.h',
  'src'/'utils-vixl.h',
  subdir: 'vixl'
)

import('pkgconfig').generate(
  libvixl,
  description: 'ARMv8 Runtime Code Generation Library',
  extra_cflags: extra_args,
  subdirs: 'vixl',
  url: 'https://github.com/Linaro/vixl'
)
