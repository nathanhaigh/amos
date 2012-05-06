#!/usr/bin/perl -w
use strict;

## /base/path/to/<lib>.<set>/seqname.READ1.txt.gz

my %libs;

print "group_name, library_name, file_name\n";

while (<>)
{
  chomp;
  my $path = $_;

  my @dirs = split /\//, $path;

  my $set = $dirs[-2];
  my $lib = (split/\./, $set)[0];

  next if /D2/;
  $path =~ s/D1/D\?/g;

  $libs{$lib}++;
  my $libid = $libs{$lib};

  print "$lib.$libid, $lib, $path\n";

}
