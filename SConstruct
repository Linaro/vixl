# Copyright 2015, ARM Limited
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

import os
import os.path
import platform
import subprocess
import sys

root_dir = os.path.dirname(File('SConstruct').rfile().abspath)
sys.path.insert(0, os.path.join(root_dir, 'tools'))
import util


Help('''
Build system for the VIXL project.
See README.md for documentation and details about the build system.
Some common build targets are:
    scons            # Build the VIXL library and test utility.
    scons examples   # Build all the examples.
    scons all        # Build everything.

''')


# Global configuration.
PROJ_SRC_DIR   = 'src'
PROJ_SRC_FILES = '''
src/vixl/a64/assembler-a64.cc
src/vixl/a64/cpu-a64.cc
src/vixl/a64/debugger-a64.cc
src/vixl/a64/decoder-a64.cc
src/vixl/a64/disasm-a64.cc
src/vixl/a64/instructions-a64.cc
src/vixl/a64/instrument-a64.cc
src/vixl/a64/logic-a64.cc
src/vixl/a64/macro-assembler-a64.cc
src/vixl/a64/simulator-a64.cc
src/vixl/code-buffer.cc
src/vixl/compiler-intrinsics.cc
src/vixl/utils.cc
'''.split()
PROJ_EXAMPLES_DIR = 'examples'
PROJ_EXAMPLES_SRC_FILES = '''
examples/abs.cc
examples/add2-vectors.cc
examples/add3-double.cc
examples/add4-double.cc
examples/check-bounds.cc
examples/crc-checksums.cc
examples/custom-disassembler.cc
examples/debugger.cc
examples/factorial-rec.cc
examples/factorial.cc
examples/neon-matrix-multiply.cc
examples/getting-started.cc
examples/non-const-visitor.cc
examples/sum-array.cc
examples/swap-int32.cc
examples/swap4.cc
'''.split()
# List target specific files.
# Target names are used as dictionary entries.
TARGET_SRC_DIR = {
  'test': 'test',
  'bench-dataop': 'benchmarks',
  'bench-branch': 'benchmarks',
  'bench-branch-link': 'benchmarks',
  'bench-branch-masm': 'benchmarks',
  'bench-branch-link-masm': 'benchmarks',
  'examples': 'examples'
}
TARGET_SRC_FILES = {
  'test': '''
    test/test-runner.cc
    test/examples/test-examples.cc
    test/test-assembler-a64.cc
    test/test-disasm-a64.cc
    test/test-fuzz-a64.cc
    test/test-invalset.cc
    test/test-simulator-a64.cc
    test/test-utils-a64.cc
    '''.split(),
  'bench-dataop': '''
    benchmarks/bench-dataop.cc
    '''.split(),
  'bench-branch': '''
    benchmarks/bench-branch.cc
    '''.split(),
  'bench-branch-link': '''
    benchmarks/bench-branch-link.cc
    '''.split(),
  'bench-branch-masm': '''
    benchmarks/bench-branch-masm.cc
    '''.split(),
  'bench-branch-link-masm': '''
    benchmarks/bench-branch-link-masm.cc
    '''.split()
}
OBJ_DIR  = 'obj'

# Helper functions.
def abort(message):
  print('ABORTING: ' + message)
  sys.exit(1)


def list_target(obj_dir, src_files):
  return map(lambda x: os.path.join(obj_dir, x), src_files)


def is_compiler(compiler):
  return env['CXX'].find(compiler) == 0


def create_variant(obj_dir, targets_dir):
  VariantDir(os.path.join(obj_dir, PROJ_SRC_DIR), PROJ_SRC_DIR)
  for directory in targets_dir.itervalues():
    VariantDir(os.path.join(obj_dir, directory), directory)


# Build arguments.
args = Variables()
args.Add(EnumVariable('mode', 'Build mode', 'release',
                      allowed_values = ['release', 'debug']))
sim_default = 'off' if platform.machine() == 'aarch64' else 'on'
args.Add(EnumVariable('simulator', 'build for the simulator', sim_default,
                      allowed_values = ['on', 'off']))
args.Add('std', 'c++ standard')

# Configure the environment.
env = Environment(variables=args)

# Commandline help.
Help(args.GenerateHelpText(env) + '\n')

# Abort if any invalid argument was passed.
# This check must happened after an environment is created.
unknown_arg = args.UnknownVariables()
if unknown_arg:
  abort('Unknown variable(s): ' + str(unknown_arg.keys()))

# Setup tools.
# This is necessary for cross-compilation.
env['CXX'] = os.environ.get('CXX', env.get('CXX'))
env['AR'] = os.environ.get('AR', env.get('AR'))
env['RANLIB'] = os.environ.get('RANLIB', env.get('RANLIB'))
env['CC'] = os.environ.get('CC', env.get('CC'))
env['LD'] = os.environ.get('LD', env.get('LD'))

if os.environ.get('CPPFLAGS'):
  env.Append(CPPFLAGS = os.environ.get('CPPFLAGS').split())
if os.environ.get('LINKFLAGS'):
  env.Append(LINKFLAGS = os.environ.get('LINKFLAGS').split())

# Always look in 'src' for include files.
# TODO: Restore the '-Wunreachable-code' flag. This flag breaks builds for clang
# 3.4 with std=c++98. So we need to re-enable this conditionally when clang is at
# version 3.5 or later.
env.Append(CPPPATH = [PROJ_SRC_DIR])
env.Append(CPPFLAGS = ['-Wall',
                       '-Werror',
                       '-fdiagnostics-show-option',
                       '-Wextra',
                       '-Wredundant-decls',
                       '-pedantic',
                       # Explicitly enable the write-strings warning. VIXL uses
                       # const correctly when handling string constants.
                       '-Wwrite-strings'])

build_suffix = ''
std_path = 'default-std'

if 'std' in env:
  env.Append(CPPFLAGS = ['-std=' + env['std']])
  std_path = env['std']

if is_compiler('clang++'):
  # This warning only works for Clang, when compiling the code base as C++11
  # or newer. The compiler does not complain if the option is passed when
  # compiling earlier C++ standards.
  env.Append(CPPFLAGS = ['-Wimplicit-fallthrough'])

if env['simulator'] == 'on':
  env.Append(CPPFLAGS = ['-DUSE_SIMULATOR'])
  build_suffix += '_sim'

if env['mode'] == 'debug':
  env.Append(CPPFLAGS = ['-g', '-DVIXL_DEBUG'])
  # Append the debug mode suffix to the executable name.
  build_suffix += '_g'
else:
  # Release mode.
  env.Append(CPPFLAGS = ['-O3'])
  process = subprocess.Popen(env['CXX'] + ' --version | grep "gnu.*4\.8"',
                             shell = True,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  stdout, stderr = process.communicate()
  using_gcc48 = stdout != ''
  if using_gcc48:
    # GCC 4.8 has a bug which produces a warning saying that an anonymous
    # Operand object might be used uninitialized:
    #   http://gcc.gnu.org/bugzilla/show_bug.cgi?id=57045
    # The bug does not seem to appear in GCC 4.7, or in debug builds with
    # GCC 4.8.
    env.Append(CPPFLAGS = ['-Wno-maybe-uninitialized'])

# Configure build directory
build_dir = os.path.join(OBJ_DIR, env['mode'], env['CXX'], std_path, '')
create_variant(build_dir, TARGET_SRC_DIR)

# The lists of available targets and target names.
targets = []
target_alias_names = []
# Helper to create aliases.
def create_alias(name, target):
  env.Alias(name, target)
  targets.append(target)
  target_alias_names.append(name)


# The vixl library.
libvixl = env.Library(build_dir + 'vixl' + build_suffix,
                      list_target(build_dir, PROJ_SRC_FILES))
create_alias('libvixl', libvixl)


# The test executable.
# The test requires building the example files with specific options, so we
# create a separate variant dir for the example objects built this way.
test_ex_vdir = os.path.join(build_dir, 'test_examples')
VariantDir(test_ex_vdir, '.')
test_ex_obj = env.Object(list_target(test_ex_vdir, PROJ_EXAMPLES_SRC_FILES),
                         CPPFLAGS = env['CPPFLAGS'] + ['-DTEST_EXAMPLES'])
test = env.Program(build_dir + 'test-runner' + build_suffix,
                   list_target(build_dir, TARGET_SRC_FILES['test']) +
                   test_ex_obj + libvixl,
                   CPPPATH = env['CPPPATH'] + [PROJ_EXAMPLES_DIR])
create_alias('test', test)

# The benchmarks.
benchmarks = ['bench-dataop', 'bench-branch', 'bench-branch-link',
              'bench-branch-masm', 'bench-branch-link-masm']
for bench in benchmarks:
  prog = env.Program(build_dir + bench + build_suffix,
                     list_target(build_dir, TARGET_SRC_FILES[bench]) + libvixl)
  create_alias(bench, prog)
# Alias to build all benchmarks.
create_alias('benchmarks', benchmarks)

# The examples.
examples = []
for example in PROJ_EXAMPLES_SRC_FILES:
  example_name = "example-" + os.path.splitext(os.path.basename(example))[0]
  prog = env.Program(build_dir + example_name,
                     [os.path.join(build_dir, example)] + libvixl,
                     CPPPATH = env['CPPPATH'] + [PROJ_EXAMPLES_DIR])
  create_alias(example_name, prog)
  examples.append(prog)
# Alias to build all examples.
create_alias('examples', examples)


# Create a simple alias to build everything with the current options.
create_alias('all', targets)

Help('Available top level targets:\n' + '\t' + '\n\t'.join(target_alias_names) + '\n')

# By default, only build the tests.
Default(libvixl, test)
