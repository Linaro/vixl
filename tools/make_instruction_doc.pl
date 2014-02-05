#!/usr/bin/perl

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

# Assembler header file.
my $hfile = "src/a64/assembler-a64.h";

# Extra pseudo instructions added to AArch64.
my @extras = qw/bind debug dci dc32 dc64/;

my %inst = ();  # Global hash of instructions.

$/ = '';
open(IN, "<$hfile") or die("Can't open header file $header.\n");
while(<IN>)
{
  # Find a function formatted like an instruction.
  if(my($t) = /^  ((?:void|inline void) [a-z0-9]{1,6}_?)\(/mgp)
  {
    my $before = ${^PREMATCH};
    my $after = ${^POSTMATCH};

    # Extract the instruction.
    my($i) = $t =~ /(?:void|inline void) ([a-z0-9]{1,6})/;

    # Extract the comment from before the function.
    my($d) = $before =~ /.*  \/\/ ([A-Z].+?\.)$/;

    # Extract and tidy up the function prototype.
    my($p) = $after =~ /(.*?\))/ms;
    $p =~ s/\n/\n  /g;
    $p = "$t(".$p;

    # Establish the type of the instruction.
    my $type = 'integer';
    ($i =~ /^f/) and $type = 'float';
    ($i ~~ @extras) and $type = 'pseudo';

    # Push the results into a hash keyed by prototype string.
    $inst{$p}->{'type'} = $type;
    $inst{$p}->{'mnemonic'} = $i;
    $inst{$p}->{'description'} = $d;
  }
}
close(IN);

print <<HEADER;
VIXL Supported Instruction List
===============================

This is a list of the AArch64 instructions supported by the VIXL assembler,
disassembler and simulator. The simulator may not support all floating point
operations to the precision required by AArch64 - please check the simulator
source code for details.

HEADER

print describe_insts('AArch64 integer instructions', 'integer');
print describe_insts('AArch64 floating point instructions', 'float');
print describe_insts('Additional or pseudo instructions', 'pseudo');

# Sort instructions by mnemonic and then description.
sub inst_sort
{
  $inst{$a}->{'mnemonic'} cmp $inst{$b}->{'mnemonic'} ||
  $inst{$a}->{'description'} cmp $inst{$b}->{'description'};
}

# Return a Markdown formatted list of instructions of a particular type.
sub describe_insts
{
  my($title, $type) = @_;
  my $result = '';
  $result .= "$title\n";
  $result .= '-' x length($title);
  $result .= "\n\n";

  foreach my $i (sort inst_sort keys(%inst))
  {
    next if($inst{$i}->{'type'} ne $type);
    $result .= sprintf("### %s ###\n\n%s\n\n", $inst{$i}->{'mnemonic'}, $inst{$i}->{'description'});
    $result .= "    $i\n\n\n";
  }
  $result .= "\n";
  return $result
}


