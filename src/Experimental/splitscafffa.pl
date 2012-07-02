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

  my $offset = 0;

  for (my $i = 0; $i < scalar @sections; $i++)
  {
    $offset = index($seq, $sections[$i], $offset);

    die "ERROR finding substring with section $i\n"  
      if $offset == -1;

    my $len = length ($sections[$i]);
    my $end = $offset + $len;

    print ">$scaffid\.$i\.$offset\.$end\n";
    print $sections[$i],"\n";

    $offset += $len;
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
