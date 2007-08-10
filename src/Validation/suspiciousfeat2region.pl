#!/usr/bin/perl
use lib "/fs/szdevel/amp/AMOS/Linux-x86_64/lib";

use strict;

my $BUFFER = 2000;
my $MIN_TYPES = 2;

my $contigid = -1;
my $rstart;
my $rend;
my $laststart;
my @reasons;
my %reasonshash;


sub printEnd
{
  my $count = scalar @reasons;

  if ( scalar(keys %reasonshash) >= $MIN_TYPES )
  {
    print "$contigid\t$rstart\t$rend\t$count\t|\t";
    print join "\t|\t", @reasons;
    print "\n";
  }

  @reasons = ();
  %reasonshash = ();
}


while (<>)
{
  my @vals = split /\s+/, $_;

  my $cid    = shift @vals;
  my $type   = shift @vals;
  my $cstart = shift @vals;
  my $cend   = shift @vals;
  my $desc   = shift @vals;

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

  if ( $cstart > $rend + $BUFFER )
  {
    printEnd();

    ## new region 
    $rstart = $cstart;
    $rend = $cend;
  }
  elsif ( $cend > $rend )
  {
    $rend = $cend;
  }

  $laststart = $cstart;

  push @reasons, "$cstart\t$cend\t$type\t@vals";
  $reasonshash{$desc}++;
}

printEnd();
