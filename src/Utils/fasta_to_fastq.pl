#!/usr/bin/perl -w
use strict;

my $QV = "^";

my $header = undef;
my $seq    = undef;

sub printFastq
{
  return if !defined $header;

  print "\@$header\n";
  print "$seq\n";
  print "+\n";
  print $QV x length($seq);
  print "\n";
}

while (<>)
{
  chomp;

  if (/>(\S+)/)
  {
    printFastq();

    $header = $1;
    $seq = "";
  }
  else
  {
    $seq .= $_;
  }
}

printFastq();
