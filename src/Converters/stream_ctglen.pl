#!/usr/bin/perl -w
use strict;

my $contigid;
my $len = 0;
while (<>)
{
  my ($ctg, $rest) = split /\s+/, $_;

  if (defined $contigid && $contigid ne $ctg)
  {
    print "$contigid\t$len\n";
    $len = 0;
  }

  $contigid = $ctg;
  $len++;
}

print "$contigid\t$len\n";
