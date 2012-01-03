#!/usr/bin/perl -w
use strict;

my %cov;

my $MAX_COV = 1000;

for (my $c = 0; $c < $MAX_COV; $c++)
{
  $cov{$c}->{all} = 0;
  $cov{$c}->{cor} = 0;
}

while (<>)
{
  chomp;

  my ($gpos, $seq, $cov, $acc) = split /\s+/, $_;

  my $cnsok = (2*$acc > $cov) ? 1 : 0;

  $cov{$cov}->{all}++;
  $cov{$cov}->{cor} += $cnsok;
}

for (my $c = 1; $c < $MAX_COV; $c++)
{
  my $all = $cov{$c}->{all};
  my $cor = $cov{$c}->{cor};
  
  my $r = sprintf("%0.05f", ($all>0) ? 1-$cor/$all : 0);

  print "$c\t$all\t$cor\t$r\n";
}

