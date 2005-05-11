#!/usr/local/bin/perl -w
use strict;

my $WINDOWSNPS = 3;
my $WINDOWSIZE = 100;
my $MINCORRELATED = 2;

my @lastsnps;
my $contigid = undef;

my $lastsnppos;
my $firstsnppos;
my $snpcount = 0;

sub printResults
{
  if ($snpcount)
  {
    my $dist = $lastsnppos - $firstsnppos;
    my $density = sprintf("%.02f", $dist / $snpcount);

    print "$contigid P $firstsnppos $lastsnppos HIGH_SNP $snpcount $density\n";
  }
}

while (<>)
{
  chomp;
  my @vals = split /\s+/, $_;

  if (!defined $contigid || $contigid != $vals[0])
  {
    printResults();

    @lastsnps = ();
    $lastsnppos = undef;
    $snpcount = 0;

    $contigid = $vals[0];
  }

  my $curpos = $vals[1];
  my $cursnps = $vals[5];

  @lastsnps = grep {($curpos - $_) < $WINDOWSIZE} @lastsnps;

  if ($cursnps >= $MINCORRELATED)
  {
    push @lastsnps, $curpos;
  }

  my $count = scalar @lastsnps;

  if ($snpcount && $count < $WINDOWSNPS)
  {
    printResults();

    @lastsnps = ();
    $lastsnppos = undef;
    $snpcount = 0;
  }

  if ($cursnps >= $MINCORRELATED)
  {
    if ($count >= $WINDOWSNPS)
    {
      if ($snpcount == 0)
      {
        my $first = $lastsnps[0];
        $firstsnppos = $first;
        $snpcount = $count;
      }
      else
      {
        $snpcount++;
      }

      $lastsnppos = $curpos;
    }
  }
}

printResults();

exit 0;
