# Copyright 2013, ARM Limited
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
import sys

# Global configuration.
PROJ_SRC_DIR   = 'src'
PROJ_SRC_FILES = '''
src/utils.cc
src/a64/assembler-a64.cc
src/a64/macro-assembler-a64.cc
src/a64/instructions-a64.cc
src/a64/decoder-a64.cc
src/a64/debugger-a64.cc
src/a64/disasm-a64.cc
src/a64/cpu-a64.cc
src/a64/simulator-a64.cc
src/a64/instrument-a64.cc
'''.split()
PROJ_EXAMPLES_DIR = 'examples'
PROJ_EXAMPLES_SRC_FILES = '''
examples/debugger.cc
examples/add3-double.cc
examples/add4-double.cc
examples/factorial-rec.cc
examples/factorial.cc
examples/sum-array.cc
examples/abs.cc
examples/swap4.cc
examples/swap-int32.cc
examples/check-bounds.cc
examples/getting-started.cc
'''.split()
# List target specific files.
# Target names are used as dictionary entries.
TARGET_SRC_DIR = {
  'cctest': 'test',
  'bench_dataop': 'benchmarks',
  'bench_branch': 'benchmarks',
  'examples': 'examples'
}
TARGET_SRC_FILES = {
  'cctest': '''
    test/cctest.cc
    test/test-utils-a64.cc
    test/test-assembler-a64.cc
    test/test-simulator-a64.cc
    test/test-disasm-a64.cc
    test/test-fuzz-a64.cc
    test/examples/test-examples.cc
    '''.split() + PROJ_EXAMPLES_SRC_FILES,
  'bench_dataop': '''
    benchmarks/bench-dataop.cc
    '''.split(),
  'bench_branch': '''
    benchmarks/bench-branch.cc
    '''.split()
}
RELEASE_OBJ_DIR  = 'obj/release'
DEBUG_OBJ_DIR    = 'obj/debug'
COVERAGE_OBJ_DIR = 'obj/coverage'


# Helper functions.
def abort(message):
  print('ABORTING: ' + message)
  sys.exit(1)


def list_target(obj_dir, src_files):
  return map(lambda x: os.path.join(obj_dir, x), src_files)


def create_variant(obj_dir, targets_dir):
  VariantDir(os.path.join(obj_dir, PROJ_SRC_DIR), PROJ_SRC_DIR)
  for directory in targets_dir.itervalues():
    VariantDir(os.path.join(obj_dir, directory), directory)


# Build arguments.
args = Variables()
args.Add(EnumVariable('mode', 'Build mode', 'release',
                      allowed_values = ['release', 'debug', 'coverage']))
args.Add(EnumVariable('target', 'Target to build', 'cctest',
                      allowed_values = ['cctest',
                                        'bench_dataop',
                                        'bench_branch',
                                        'examples']))
args.Add(EnumVariable('simulator', 'build for the simulator', 'on',
                      allowed_values = ['on', 'off']))

# Configure the environment.
create_variant(RELEASE_OBJ_DIR, TARGET_SRC_DIR)
create_variant(DEBUG_OBJ_DIR, TARGET_SRC_DIR)
create_variant(COVERAGE_OBJ_DIR, TARGET_SRC_DIR)
env = Environment(variables=args)

# Commandline help.
Help(args.GenerateHelpText(env))

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
env.Append(CPPPATH = [PROJ_SRC_DIR])
env.Append(CPPFLAGS = ['-Wall',
                       '-Werror',
                       '-fdiagnostics-show-option',
                       '-Wextra',
                       '-pedantic',
                       # Explicitly enable the write-strings warning. VIXL uses
                       # const correctly when handling string constants.
                       '-Wwrite-strings'])

target_program = env['target']
build_suffix = ''

if env['simulator'] == 'on':
  env.Append(CPPFLAGS = ['-DUSE_SIMULATOR'])
  build_suffix += '_sim'

if env['mode'] == 'debug':
  env.Append(CPPFLAGS = ['-g', '-DDEBUG'])
  # Append the debug mode suffix to the executable name.
  build_suffix += '_g'
  build_dir = DEBUG_OBJ_DIR
elif env['mode'] == 'coverage':
  env.Append(CPPFLAGS = ['-g', '-DDEBUG', '-fprofile-arcs', '-ftest-coverage'])
  env.Append(LINKFLAGS = ['-fprofile-arcs'])
  # Append the coverage mode suffix to the executable name.
  build_suffix += '_gcov'
  build_dir = COVERAGE_OBJ_DIR
else:
  # Release mode.
  env.Append(CPPFLAGS = ['-O3'])
  build_dir = RELEASE_OBJ_DIR
  # GCC 4.8 has a bug which produces a warning saying that an anonymous Operand
  # object might be used uninitialized:
  #   http://gcc.gnu.org/bugzilla/show_bug.cgi?id=57045
  # The bug does not seem to appear in GCC 4.7, or in debug builds with GCC 4.8.
  env.Append(CPPFLAGS = ['-Wno-maybe-uninitialized'])


if target_program == 'cctest':
  env.Append(CPPPATH = [PROJ_EXAMPLES_DIR])
  env.Append(CPPFLAGS = ['-DTEST_EXAMPLES'])

# Build the library.
proj_library = env.Library('vixl' + build_suffix, list_target(build_dir, PROJ_SRC_FILES))

if target_program == 'examples':
  # Build the examples.
  env.Append(CPPPATH = [PROJ_EXAMPLES_DIR])
  for example in PROJ_EXAMPLES_SRC_FILES:
    example_name = "example-" + os.path.splitext(os.path.basename(example))[0]
    env.Program(example_name, list_target(build_dir, [example]) + proj_library)
else:
  # Build the target program.
  program_target_files = list_target(build_dir, TARGET_SRC_FILES[env['target']])
  env.Program(target_program + build_suffix, program_target_files + proj_library)
