#!/usr/bin/perl
use strict;
use FileHandle;

my %reads;
my %files;

my $curfh = undef;
my $curbase = "";
my $doprint = 0;

# >m110425_194810_00124_c100105422556400000315040006221190_s1_p0/10/34_688

my $allreads = 0;
my $printedreads = 0;

while (<>)
{
  if (/>(\S+)/)
  {
    $allreads++;

    if (($allreads % 100000) == 0)
    {
      print STDERR "$allreads\n";
    }

    my $fullname = $1;
    my @fields = split /\//, $fullname;

    my $basename = $fields[0];
    my $readid   = $fields[1];
    my $range    = $fields[2];
    #print "$basename $readid $range\n";

    if ($basename ne $curbase)
    {
      if (!exists $files{$basename})
      {
        $files{$basename} = FileHandle->new("> $basename.fa");
      }

      $curfh = $files{$basename};
      $curbase = $basename;
    }

    $doprint = 0;
    $reads{$basename}->{$readid}->{$range}++;

    if ($reads{$basename}->{$readid}->{$range} == 1)
    {
      $doprint = 1;
      $printedreads++;
    }
  }

  if ($doprint) { print $curfh $_; }
}

my $numfiles = scalar keys %files;

print STDERR "Printed $printedreads of $allreads into $numfiles files\n";
