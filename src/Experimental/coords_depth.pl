#!/usr/bin/perl -w
use strict;

my $USAGE = "coords_depth.pl lens <coords> > seq.depth";

my %depth;
my %lens;
my @seqs;

my $lensfile = shift @ARGV or die $USAGE;

open LENS, "< $lensfile" or die "Can't open $lensfile ($!)\n";

my $gpos = 0;

while (<LENS>)
{
  chomp;
  my ($seqid, $len) = split /\s+/, $_;

  push @seqs, $seqid;
  $lens{$seqid} = $len;

  print "#$gpos\t$seqid\t$len\n";
  $gpos+=$len;
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

  my $dir = ($qstart < $qend) ? "f" : "r";

  for (my $i = $start; $i <= $end; $i++)
  {
    $depth{$cur}->[$i]++;
  }
}

$gpos = 0;

foreach my $seqid (@seqs)
{
  my $l = $lens{$seqid};

  for (my $p = 1; $p < $l; $p++)
  {
    my $d = 0;
    $d = $depth{$seqid}->[$p] if (exists $depth{$seqid}->[$p]);

    print "$gpos\t$seqid\t$p\t$d\n";

    $gpos++;
  }
}

