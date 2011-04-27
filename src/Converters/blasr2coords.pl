#!/usr/bin/perl -w
use strict;

while (<>)
{
  chomp;
  my @vals = split /\s/, $_;

  my $qname  = $vals[0];
  my $rname  = $vals[1];
  my $qoo    = $vals[2];
  my $score  = $vals[3];
  my $id     = $vals[4];

  my $rstart = $vals[5] + 1;
  my $rend   = $vals[6] + 1;
  my $rlen   = $vals[7];

  my $qstart = $vals[8] + 1;
  my $qend   = $vals[9] + 1;
  my $qlen   = $vals[10];

  printf("%8d %8d  | %8d %8d  | %8d %8d  | %6.02f  | %8d %8d | %6.02f %6.02f | $rname $qname\n",
         $rstart, $rend,
         (($qoo == 0) ? $qstart : $qend), (($qoo == 0) ? $qend : $qstart),
         $rend - $rstart, $qend - $qstart,
         $id,
         $rlen, $qlen,
         100 * (($rend - $rstart)/$rlen), 100 * (($qend-$qstart)/$qlen));
}
