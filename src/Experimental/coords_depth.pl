#!/usr/bin/perl -w
use strict;

my $PERC_ALEN=.95;

my $depth;
my $seq = "";

sub printDepth
{
  return if !defined $seq || $seq eq "";

  print "$seq\t", join(",",@$depth), "\n";
}

while (<>)
{
  chomp;
  s/^\s+//;

  my @fields = split /\s+/, $_;

  # for (my $i = 0; $i < scalar @fields; $i++) { print "$i: $fields[$i]\n"; }

  my $start  = $fields[0];
  my $end    = $fields[1];

  my $qstart = $fields[3];
  my $qend   = $fields[4];

  my $qalen  = $fields[7];

  my $curlen = $fields[11];
  my $qlen   = $fields[12];

  my $cur    = $fields[17];

  if ($seq ne $cur)
  {
    printDepth();

    $seq = $cur;
    $depth = undef;

    for (my $i = 0; $i < $curlen; $i++)
    {
      $depth->[$i] = 0;
    }
  }

  my $dir = ($qstart < $qend) ? "f" : "r";

  if ((($dir eq "r") && ($start <= 5)) || 
      (($dir eq "f") && ($curlen-$end <= 5)) ||
      ($qalen >= $qlen*$PERC_ALEN))
  {
    # print "$_\n";

    for (my $i = $start; $i <= $end; $i++)
    {
      $depth->[$i-1]++;
    }
  }
}

printDepth();
