#!/usr/bin/perl -w
use strict;


my %best;

while (<>)
{
  my @vals = split /\s+/, $_;

  my $qlen   = $vals[8];
  my $percid = $vals[10];
  my $rseq   = $vals[18];
  my $qseq   = $vals[19];

  my $match = $percid * $qlen / 100;

  if ((!exists $best{$qseq}) || ($match > $best{$qseq}->{score}))
  {
    $best{$qseq}->{rseq}  = $rseq;
    $best{$qseq}->{score} = $match;
    $best{$qseq}->{align} = $_;
  }
}

foreach my $q (keys %best)
{
  my $rseq  = $best{$q}->{rseq};
  my $score = $best{$q}->{score};
  my $align = $best{$q}->{align};

  #print "$q\t$rseq\t$score\t$align";
  print $align;
}
