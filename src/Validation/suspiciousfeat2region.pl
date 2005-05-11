#!/usr/local/bin/perl -w
use strict;

my $contigid = -1;
my $rend;
my $laststart;
my @reasons;

my $buffer = 1000;

sub printEnd
{
  if (scalar @reasons)
  {
    print "$rend\t|\t";
    print join "\t", @reasons;
    print "\n";

    @reasons = ();
  }
}


while (<>)
{
  my @vals = split /\s+/, $_;

  my $cid = shift @vals;
  my $type = shift @vals;
  my $cstart = shift @vals;
  my $cend = shift @vals;

  if ($cid != $contigid)
  {
    printEnd();

    $laststart = -10000000;
    $rend      = -10000000;
    $contigid = $cid;
  }

  if ($cstart < $laststart)
  {
    die "Features are unsorted!";
  }

  if ($cstart-$buffer > $rend)
  {
    printEnd();

    ## new region 
    my $rstart = $cstart-$buffer;
    $rend = $cend+$buffer;

    print "$contigid\t$rstart\t";
  }
  elsif ($cend+$buffer > $rend)
  {
    $rend = $cend+$buffer;
  }

  $laststart = $cstart;

  push @reasons, "@vals\t$cstart\t$cend";
}

printEnd();
