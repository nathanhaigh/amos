#!/usr/bin/perl -w
use strict;
use Getopt::Long;

$|=1;

## Paths to binaries
my $FASTA_FROM_BED = "fastaFromBed";


my $USAGE = "extractFasta -x start -y end -i name ref.fa\n";
my $help;

my $start;
my $end;
my $name;

my $res = GetOptions("help"      => \$help,
                     "x=s"       => \$start,
                     "y=s"       => \$end,
                     "i=s"       => \$name);
 
if ($help)
{
  print $USAGE;
  print "\n";
  print "Extract a region from a fasta file\n";
  print "\n";
  print "Required\n";
  print "  ref.fa      : path to reference genome\n";
  print "  -x <pos>    : starting position\n";
  print "  -y <pos>    : ending position\n";
  print "  -i <name>   : name of sequence\n";
  exit 0;
}

my $ref = $ARGV[0] or die $USAGE;

if (!defined $start || !defined $end || !defined $name)
{
  die $USAGE;
}

if (! -r $ref)
{
  die "ERROR: Can't read $ref\n";
}

my $outname = "extractFasta.$$.fa";

open (FABED, "| $FASTA_FROM_BED -fi $ref -bed - -fo $outname")
 or die "Can't open pipe to $FASTA_FROM_BED ($!)\n";
print FABED "$name\t$start\t$end\n";
close FABED;

open FA, "< $outname" or die "Can't open $outname ($!)\n";
while (<FA>) { print $_; }
close FA;

unlink $outname;

