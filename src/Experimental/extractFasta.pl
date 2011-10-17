#!/usr/bin/perl

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
my $rc = 0;
my $SKIP_HEADER = 0;

my $res = GetOptions("help"      => \$help,
                     "x=s"       => \$start,
                     "y=s"       => \$end,
                     "i=s"       => \$name,
                     "skip"      => \$SKIP_HEADER,
                     "rc"        => \$rc);
 
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
  print "\n";
  print "Options\n";
  print "  -rc         : reverse complement sequence\n";
  print "  -skip       : dont print fasta header\n";
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

open (FABED, "| $FASTA_FROM_BED -fi $ref -bed - -fo $outname -s")
 or die "Can't open pipe to $FASTA_FROM_BED ($!)\n";

if ($rc) { print FABED "$name\t$start\t$end\treg\t1\t-\n"; }
else     { print FABED "$name\t$start\t$end\treg\t1\t+\n"; }

close FABED;

open FA, "< $outname" or die "Can't open $outname ($!)\n";
while (<FA>) 
{ 
  next if ($SKIP_HEADER && />/);
  print $_; 
}
close FA;

unlink $outname;

