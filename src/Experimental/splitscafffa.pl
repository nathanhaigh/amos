#!/usr/bin/perl -w
use strict;

## Split a scaffold into contigs whenever there are $NTHRESH N's in a row

my $NTHRESH = 20;

sub processScaff
{
  my $scaffid = $_[0];
  my $seq     = $_[1];

  return if !defined $scaffid;

  my @sections = split /N{$NTHRESH,}/, $seq;

  for (my $i = 0; $i < scalar @sections; $i++)
  {
    print ">$scaffid\.$i\n";
    print $sections[$i],"\n";
  }
}


my $scaffid = undef;
my $seq     = undef;

while (<>)
{
  chomp;

  if (/^>(\S+)/)
  {
    processScaff($scaffid, $seq);
    $scaffid = $1;
    $seq = "";
  }
  else
  {
    $seq .= $_;
  }
}

processScaff($scaffid, $seq);
