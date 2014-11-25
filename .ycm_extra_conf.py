# Vim YouCompleteMe completion configuration.
#
# See doc/topics/ycm.md for details.

import os
import platform

repo_root = os.path.dirname(os.path.abspath(__file__))

# Paths in the compilation flags must be absolute to allow ycm to find them from
# any working directory.
def AbsolutePath(path):
  return os.path.join(repo_root, path)

flags = [
  '-I', AbsolutePath('src'),
  '-Wall',
  '-Werror',
  '-Wextra',
  '-pedantic',
  '-Wno-newline-eof',
  '-Wwrite-strings',
  '-std=c++',
  '-x', 'c++'
]

if platform.machine() != 'aarch64':
  flags.append('-DUSE_SIMULATOR')


def FlagsForFile(filename, **kwargs):
  return {
    'flags': flags,
    'do_cache': True
  }

