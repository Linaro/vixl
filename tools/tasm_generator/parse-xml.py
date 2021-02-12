#!/usr/bin/env python3

# Copyright 2021, VIXL authors
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

from xml.dom import minidom
import os
import sys
import argparse

def GetInstructionList(in_file, out_file):
  str_input = in_file.read()
  xmldoc = minidom.parseString(str_input)
  mainlist = xmldoc.getElementsByTagName('asmtemplate')
  aliaslist = xmldoc.getElementsByTagName('equivalent_to')

  # Find instruction aliases and remove them from the main list.
  for al in aliaslist:
    mainlist.remove(al.getElementsByTagName('asmtemplate')[0])

  for m in mainlist:
    instruction = []

    for i in m.childNodes:
      for t in i.childNodes:
        instruction.append(t.data)

    if len(instruction) > 0:
      print(''.join(instruction), file=out_file)


if __name__ == '__main__':
  parser = argparse.ArgumentParser(
    description='Generate instructions list from reference manual xml file.')
  parser.add_argument('--out-file', metavar='out_file', default=sys.stdout,
                       type=argparse.FileType('w'),  nargs='?',
                       help='path to an output file')
  parser.add_argument('--in-file', metavar='in_file', nargs='?',
                      type=argparse.FileType('r'), default=sys.stdin,
                      help='path to the xml file')
  args = parser.parse_args()
  GetInstructionList(args.in_file, args.out_file)
