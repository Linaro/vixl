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

import glob
import os
from os.path import join
import platform
import subprocess
import sys

root_dir = os.path.dirname(File('SConstruct').rfile().abspath)
sys.path.insert(0, join(root_dir, 'tools'))
import config
import util


Help('''
Build system for the VIXL project.
See README.md for documentation and details about the build system.
''')


# We track top-level targets to automatically generate help and alias them.
class TopLevelTargets:
  def __init__(self):
    self.targets = []
    self.help_messages = []
  def Add(self, target, help_message):
    self.targets.append(target)
    self.help_messages.append(help_message)
  def Help(self):
    res = ""
    for i in range(len(self.targets)):
      res += '\t{0:<{1}}{2:<{3}}\n'.format(
        'scons ' + self.targets[i],
        len('scons ') + max(map(len, self.targets)),
        ' : ' + self.help_messages[i],
        len(' : ') + max(map(len, self.help_messages)))
    return res

top_level_targets = TopLevelTargets()



# Build options ----------------------------------------------------------------

# Store all the options in a dictionary.
# The SConstruct will check the build variables and construct the build
# environment as appropriate.
options = {
    'all' : { # Unconditionally processed.
      'CCFLAGS' : ['-Wall',
                   '-Werror',
                   '-fdiagnostics-show-option',
                   '-Wextra',
                   '-Wredundant-decls',
                   '-pedantic',
                   '-Wmissing-noreturn',
                   '-Wwrite-strings'],
      'CPPPATH' : [config.dir_src_vixl]
      },
#   'build_option:value' : {
#     'environment_key' : 'values to append'
#     },
    'mode:debug' : {
      'CCFLAGS' : ['-DVIXL_DEBUG', '-O0']
      },
    'mode:release' : {
      'CCFLAGS' : ['-O3']
      },
    'simulator:on' : {
      'CCFLAGS' : ['-DVIXL_INCLUDE_SIMULATOR'],
      },
    'symbols:on' : {
      'CCFLAGS' : ['-g'],
      'LINKFLAGS' : ['-g']
      },
    }


# A `DefaultVariable` has a default value that depends on elements not known
# when variables are first evaluated.
# Each `DefaultVariable` has a handler that will compute the default value for
# the given environment.
def modifiable_flags_handler(env):
  env['modifiable_flags'] = \
      'on' if 'mode' in env and env['mode'] == 'debug' else 'off'


def symbols_handler(env):
  env['symbols'] = 'on' if 'mode' in env and env['mode'] == 'debug' else 'off'


vars_default_handlers = {
    # variable_name    : [ 'default val', 'handler'                ]
    'symbols'          : [ 'mode==debug', symbols_handler          ],
    'modifiable_flags' : [ 'mode==debug', modifiable_flags_handler ]
    }


def DefaultVariable(name, help, allowed):
  default_value = vars_default_handlers[name][0]
  allowed.append(default_value)
  return EnumVariable(name, help, default_value, allowed)


vars = Variables()
# Define command line build options.
sim_default = 'off' if platform.machine() == 'aarch64' else 'on'
vars.AddVariables(
    EnumVariable('mode', 'Build mode',
                 'release', allowed_values=config.build_options_modes),
    DefaultVariable('symbols', 'Include debugging symbols in the binaries',
                    ['on', 'off']),
    EnumVariable('simulator', 'Build for the simulator',
                 sim_default, allowed_values=['on', 'off']),
    ('std', 'C++ standard. The standards tested are: %s.' % \
                                         ', '.join(config.tested_cpp_standards))
    )

# Abort the build if any command line option is unknown or invalid.
unknown_build_options = vars.UnknownVariables()
if unknown_build_options:
  print 'Unknown build options:',  unknown_build_options.keys()
  Exit(1)

# We use 'variant directories' to avoid recompiling multiple times when build
# options are changed, different build paths are used depending on the options
# set. These are the options that should be reflected in the build directory
# path.
options_influencing_build_path = ['mode', 'symbols', 'CXX', 'std', 'simulator']



# Build helpers ----------------------------------------------------------------

def RetrieveEnvironmentVariables(env):
  for key in ['CC', 'CXX', 'CCFLAGS', 'CXXFLAGS', 'AR', 'RANLIB', 'LD']:
    if os.getenv(key): env[key] = os.getenv(key)
  if os.getenv('LD_LIBRARY_PATH'): env['LIBPATH'] = os.getenv('LD_LIBRARY_PATH')
  if os.getenv('CXXFLAGS'):
    env.Append(CXXFLAGS = os.getenv('CXXFLAGS').split())
  if os.getenv('LINKFLAGS'):
    env.Append(LINKFLAGS = os.getenv('LINKFLAGS').split())
  # This allows colors to be displayed when using with clang.
  env['ENV']['TERM'] = os.getenv('TERM')


def ProcessBuildOptions(env):
  # 'all' is unconditionally processed.
  if 'all' in options:
    for var in options['all']:
      if var in env and env[var]:
        env[var] += options['all'][var]
      else:
        env[var] = options['all'][var]
  # Other build options must match 'option:value'
  env_dict = env.Dictionary()
  for key in env_dict.keys():
    # First apply the default variables handlers.
    if key in vars_default_handlers and \
        env_dict[key] == vars_default_handlers[key][0]:
      vars_default_handlers[key][1](env_dict)
    # Then update the environment according to the value of the variable.
    key_val_couple = key + ':%s' % env_dict[key]
    if key_val_couple in options:
      for var in options[key_val_couple]:
        env[var] += options[key_val_couple][var]


def ConfigureEnvironmentForCompiler(env):
  def is_compiler(compiler):
    return env['CXX'].find(compiler) == 0
  if is_compiler('clang++'):
    # These warnings only work for Clang.
    # -Wimplicit-fallthrough only works when compiling the code base as C++11 or
    # newer. The compiler does not complain if the option is passed when
    # compiling earlier C++ standards.
    env.Append(CPPFLAGS = ['-Wimplicit-fallthrough', '-Wshorten-64-to-32'])

    # The '-Wunreachable-code' flag breaks builds for clang 3.4.
    process = subprocess.Popen(env['CXX'] + ' --version | grep "clang.*3\.4"',
                               shell = True,
                               stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    stdout, stderr = process.communicate()
    using_clang3_4 = stdout != ''
    if not using_clang3_4:
      env.Append(CPPFLAGS = ['-Wunreachable-code'])

  # GCC 4.8 has a bug which produces a warning saying that an anonymous Operand
  # object might be used uninitialized:
  #   http://gcc.gnu.org/bugzilla/show_bug.cgi?id=57045
  # The bug does not seem to appear in GCC 4.7, or in debug builds with GCC 4.8.
  if env['mode'] == 'release':
    process = subprocess.Popen(env['CXX'] + ' --version | grep "g++.*4\.8"',
                               shell = True,
                               stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    stdout, stderr = process.communicate()
    using_gcc48 = stdout != ''
    if using_gcc48:
      env.Append(CPPFLAGS = ['-Wno-maybe-uninitialized'])


def ConfigureEnvironment(env):
  RetrieveEnvironmentVariables(env)
  ProcessBuildOptions(env)
  if 'std' in env:
    env.Append(CPPFLAGS = ['-std=' + env['std']])
    std_path = env['std']
  ConfigureEnvironmentForCompiler(env)


def TargetBuildDir(env):
  # Build-time option values are embedded in the build path to avoid requiring a
  # full build when an option changes.
  build_dir = config.dir_build
  for option in options_influencing_build_path:
    option_value = env[option] if option in env else ''
    build_dir = join(build_dir, option + '_'+ option_value)
  return build_dir


def PrepareVariantDir(location, build_dir):
  location_build_dir = join(build_dir, location)
  VariantDir(location_build_dir, location)
  return location_build_dir


def VIXLLibraryTarget(env):
  build_dir = TargetBuildDir(env)
  # Create a link to the latest build directory.
  subprocess.check_call(["rm", "-f", config.dir_build_latest])
  util.ensure_dir(build_dir)
  subprocess.check_call(["ln", "-s", build_dir, config.dir_build_latest])
  # Source files are in `src/vixl` and in `src/vixl/a64/`.
  variant_dir_vixl = PrepareVariantDir(join('src', 'vixl'), build_dir)
  variant_dir_a64 = PrepareVariantDir(join('src', 'vixl', 'a64'), build_dir)
  sources = [Glob(join(variant_dir_vixl, '*.cc')),
             Glob(join(variant_dir_a64, '*.cc'))]
  return env.Library(join(build_dir, 'vixl'), sources)



# Build ------------------------------------------------------------------------

# The VIXL library, built by default.
env = Environment(variables = vars)
ConfigureEnvironment(env)
Help(vars.GenerateHelpText(env))
libvixl = VIXLLibraryTarget(env)
Default(libvixl)
env.Alias('libvixl', libvixl)
top_level_targets.Add('', 'Build the VIXL library.')


# The benchmarks.
benchmark_names = util.ListCCFilesWithoutExt(config.dir_benchmarks)
benchmarks_build_dir = PrepareVariantDir('benchmarks', TargetBuildDir(env))
benchmark_targets = []
for bench in benchmark_names:
  prog = env.Program(join(benchmarks_build_dir, bench),
                     join(benchmarks_build_dir, bench + '.cc'),
                     LIBS=[libvixl])
  benchmark_targets.append(prog)
env.Alias('benchmarks', benchmark_targets)
top_level_targets.Add('benchmarks', 'Build the benchmarks.')


# The examples.
example_names = util.ListCCFilesWithoutExt(config.dir_examples)
examples_build_dir = PrepareVariantDir('examples', TargetBuildDir(env))
example_targets = []
for example in example_names:
  prog = env.Program(join(examples_build_dir, example),
                     join(examples_build_dir, example + '.cc'),
                     LIBS=[libvixl])
  example_targets.append(prog)
env.Alias('examples', example_targets)
top_level_targets.Add('examples', 'Build the examples.')


# The tests.
test_build_dir = PrepareVariantDir('test', TargetBuildDir(env))
# The test requires building the example files with specific options, so we
# create a separate variant dir for the example objects built this way.
test_examples_vdir = join(TargetBuildDir(env), 'test', 'test_examples')
VariantDir(test_examples_vdir, '.')
test_examples_obj = env.Object(
    [Glob(join(test_examples_vdir, join('test', 'examples', '*.cc'))),
             Glob(join(test_examples_vdir, join('examples', '*.cc')))],
    CCFLAGS = env['CCFLAGS'] + ['-DTEST_EXAMPLES'],
    CPPPATH = env['CPPPATH'] + [config.dir_examples])
test = env.Program(join(test_build_dir, 'test-runner'),
                   [Glob(join(test_build_dir, '*.cc')), test_examples_obj],
                   CPPPATH = env['CPPPATH'] + [config.dir_examples],
                   LIBS=[libvixl])
env.Alias('tests', test)
top_level_targets.Add('tests', 'Build the tests.')


env.Alias('all', top_level_targets.targets)
top_level_targets.Add('all', 'Build all the targets above.')

Help('\n\nAvailable top level targets:\n' + top_level_targets.Help())
