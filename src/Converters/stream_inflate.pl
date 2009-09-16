#!/usr/bin/perl -w
use strict;

while (<>)
{
  chomp;
  my @vals = split /\t/, $_;

  my $acc = shift @vals;
  my $type = shift @vals;

  print "{$type\n";
  foreach my $line (@vals)
  {
    print "$line\n";
  }
  print "}\n";
}

