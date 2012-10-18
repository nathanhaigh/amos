#!/usr/bin/perl -w
use strict;

# sanitize first:
# tr ':#/' '___' < all.fa

my $libraryname = "paired";
my $mean = 200;
my $stdev = 20;

print "library\t$libraryname\t$mean\t$stdev\n";

my %seen;
my $sep = '_';

while (<>)
{
  if (/^>(\S+)/)
  {
    my $readname = $1;
    if ($readname =~ /(\S+)$sep(\d)$/)
    {
      my $base = $1;
      my $v=$2;

      if (!exists $seen{$base})
      {
        print $base.$sep."1\t".$base.$sep."2\t$libraryname\n";
      }

      $seen{$base}++;
    }
  }
}
