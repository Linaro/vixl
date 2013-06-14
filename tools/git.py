# Copyright 2013 ARM Limited
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
# THIS SOFTWARE IS PROVIDED BY ARM LIMITED AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ARM LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import re
import util
import os.path

def is_git_repository_root():
  return os.path.isdir('.git')

def get_current_branch():
  status, branches = util.getstatusoutput('git branch')
  if status != 0: util.abort('Failed to run git branch.')
  match = re.search("^\* (.*)$", branches, re.MULTILINE)
  if not match: util.abort('Failed to find the current branch.')

  branch = match.group(1);

  # If we are not on a named branch, return the hash of the HEAD commit.
  # This can occur (for example) if a specific revision is checked out by
  # commit hash, or during a rebase.
  if branch == '(no branch)':
    status, commit = util.getstatusoutput('git log -1 --pretty=format:"%H"')
    if status != 0: util.abort('Failed to run git log.')
    match = re.search('^[0-9a-fA-F]{40}$', commit, re.MULTILINE)
    if not match: util.abort('Failed to find the current revision.')
    branch = match.group(0)

  return branch


def get_tracked_files():
  command = 'git ls-tree '
  branch = get_current_branch()
  options = ' -r --full-tree --name-only'

  status, tracked = util.getstatusoutput(command + branch + options)
  if status != 0: util.abort('Failed to list tracked files.')

  return tracked


# Get untracked files in src/, test/, and tools/.
def get_untracked_files():
  status, output = util.getstatusoutput('git status -s')
  if status != 0: util.abort('Failed to get git status.')

  untracked_regexp = re.compile('\?\?.*(src/|test/|tools/).*(.cc$|.h$)')
  files_in_watched_folder = lambda n: untracked_regexp.search(n) != None
  untracked_files = filter(files_in_watched_folder, output.split('\n'))

  return untracked_files
