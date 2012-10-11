#!/usr/bin/perl -w
use strict;

my $oldstyle = 0;

while (<>)
{
  chomp;
  my @vals = split /\s/, $_;

  if ($oldstyle)
  {
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

    printf("%8d %8d  | %8d %8d  | %8d %8d  | %9.02f  | %8d %8d | %8.02f %9.02f | $rname\t$qname\n",
           $rstart, $rend,
           (($qoo == 0) ? $qstart : $qend), (($qoo == 0) ? $qend : $qstart),
           $rend - $rstart, $qend - $qstart,
           $id,
           $rlen, $qlen,
           100 * (($rend - $rstart)/$rlen), 100 * (($qend-$qstart)/$qlen));
  }
  else
  {
    ## see blasr -header

    my $qname = $vals[0];
    my $rname = $vals[1];

    my $qoo   = $vals[2];
    my $roo   = $vals[3];

    my $score = $vals[4];
    my $id    = $vals[5];

    my $rstart = $vals[6]+1;
    my $rend   = $vals[7]+1;
    my $rlen   = $vals[8];

    my $qstart = $vals[9]+1;
    my $qend   = $vals[10]+1;
    my $qlen   = $vals[11];

    my $ncells = $vals[12];

    printf("%8d %8d  | %8d %8d  | %8d %8d  | %9.02f  | %8d %8d | %8.02f %9.02f | $rname\t$qname\n",
           $rstart, $rend,
           (($roo == 0) ? $qstart : $qend), (($roo == 0) ? $qend : $qstart),
           $rend - $rstart, $qend - $qstart,
           $id,
           $rlen, $qlen,
           100 * (($rend - $rstart)/$rlen), 100 * (($qend-$qstart)/$qlen));

  }
}
