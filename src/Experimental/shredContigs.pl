#!/usr/bin/perl -w
use strict;
use Getopt::Long;

my $help    = 0;
my $READLEN = 3000;
my $COV     = 3;

my $USAGE = "shredContigs <options> ctg.fa > ctg.shred.fa\n";

my $res = GetOptions("help"  => \$help,
                     "len=s" => \$READLEN,
                     "cov=s" => \$COV);

if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print "  Shred a fasta file of contigs into overlapping sub-sequences\n";
  print "\n";
  print "Options\n";
  print "  -len <len> : length to shred sequences (default: $READLEN)\n";
  print "  -cov <cov> : coverage of overlapping shreded sequences (default: $COV)\n";
  exit 0;
}

die $USAGE if (scalar @ARGV == 0);


sub shredSeq
{
  my $seqid = shift;
  my $seq = shift;

  return if !defined $seq;

  my $delta = $READLEN / $COV;

  my $segment = 0;
  my $offset = 0;
  my $seqlen = length ($seq);

  while ($offset < $seqlen)
  {
    my $sseq = substr($seq, $offset, $READLEN);
    my $end = $offset + $READLEN;
    $end = $seqlen if ($end > $seqlen);

    print ">$seqid.$segment.$offset.$end\n";
    print "$sseq\n";

    last if ($end == $seqlen);

    $offset += $delta;
    $segment++;
  }
}


my $seqid = undef;
my $seq = undef;

while (<>)
{
  chomp;

  if (/^>/)
  {
    shredSeq($seqid, $seq);

    $seqid = $_;
    $seq = "";
  }
  else
  {
    $seq .= $_;
  }
}

shredSeq($seqid, $seq);
