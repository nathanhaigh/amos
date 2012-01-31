#!/usr/bin/perl -w
use strict;
use Getopt::Long;

my $USAGE = "fasta_trim <options> seq.fa\n"; 

## Options
my $help  = 0;

my $LEN = 100;

my $res = GetOptions("help"    => \$help,
                     "len=s"   => \$LEN,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print "  Trim sequences in a fasta file to a given length\n";
  print "\n";
  print "Required\n";
  print "  seq.fa          : path to multifasta file\n";
  print "\n";
  print "Options\n";
  print "  -len <l>        : length to trim (default: $LEN)\n";
  exit 0;
}



## Process a single contig
###############################################################################
sub processSeq
{
  my $seq = $_[0];
  return if !defined $seq;
  print substr($seq, 0, $LEN), "\n";
}



## Scan the multifasta file
###############################################################################
my $seq = undef;
my $seqid = undef;

while (<>)
{
  if (/^>/)
  {
    processSeq($seq);
    $seq = "";
    print $_;
  }
  else
  {
    chomp;
    $seq .= $_;
  }
}

processSeq($seq);
