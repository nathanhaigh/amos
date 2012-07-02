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

  my $doprint = 0;
  my $skipmate = 0;

  if    (/_R1_/)   { $doprint = 1; $path =~ s/_R1_/_R\?_/g; }
  elsif (/_R2_/)   { $skipmate = 1; }
  elsif (/_R1\./)  { $doprint = 1; $path =~ s/_R1\./_R\?\./g; }
  elsif (/_R2\./)  { $skipmate = 1; }
  elsif (/_RD1\./) { $doprint = 1; $path =~ s/_RD1/_RD\?/g; }
  elsif (/_RD2\./) { $skipmate = 1; }
  elsif (/_READ1/) { $doprint = 1; $path =~ s/_READ1/_READ\?/g; }
  elsif (/_READ2/) { $skipmate = 1; }
  else
  {
    die "Unknown pairing $_\n";
  }


  if ($doprint)
  {
    $libs{$lib}++;
    my $libid = $libs{$lib};

    print "$lib.$libid, $lib, $path\n";
  }
}
