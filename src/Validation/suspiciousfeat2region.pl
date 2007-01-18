#!/usr/local/bin/perl -w
use strict;

my $BUFFER = 1000;
my $MIN_SIGNATURES = 2;

my $contigid = -1;
my $rstart;
my $rend;
my $laststart;
my @reasons;


sub printEnd
{
  my $count = scalar @reasons;
  if ($count >= $MIN_SIGNATURES)
  {
    print "$contigid\t$rstart\t$rend\t$count\t|\t";
    print join "\t|\t", @reasons;
    print "\n";
  }

  @reasons = ();
}


while (<>)
{
  my @vals = split /\s+/, $_;

  my $cid    = shift @vals;
  my $type   = shift @vals;
  my $cstart = shift @vals;
  my $cend   = shift @vals;

  if ($cid != $contigid)
  {
    printEnd();

    ## new contig
    $contigid  = $cid;
    $laststart = -10000000;
    $rstart    = -10000000;
    $rend      = -10000000;
  }

  if ($cstart < $laststart)
  {
    die "Features are unsorted!";
  }

  if ($cstart > $rend + $BUFFER)
  {
    printEnd();

    ## new region 
    $rstart = $cstart;
    $rend = $cend;
  }
  elsif ($cend > $rend)
  {
    $rend = $cend;
  }

  $laststart = $cstart;

  push @reasons, "$cstart\t$cend\t$type\t@vals";
}

printEnd();
