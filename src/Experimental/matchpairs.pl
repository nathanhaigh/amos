#!/usr/bin/perl -w
use strict;
use Getopt::Long;

## Given 2 fq files 
## extract the subset of paired reads that occur in both => f1.matched, f2.matched
## and also report the orphans reads => f1.orphans, f2.orphans

## Options
my $help  = 0;

my $USAGE = "matchpairs.pl <options> fq1 fq2\n";
my $VERBOSE = 0;

my $res = GetOptions("help"      => \$help,
                     "verbose"   => \$VERBOSE,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print "  Make sure the pairs from fq1 and fq2 are paired up\n";
  print "\n";
  print "Required\n";
  print "  fq1             : path to first read\n";
  print "  fq2             : path to second read\n";
  print "\n";
  print "Options\n";
  print "  -verbose        : be verbose\n";
  exit 0;
}


my $fq1 = shift @ARGV or die $USAGE;
my $fq2 = shift @ARGV or die $USAGE;

my %basenames;

if ($fq1 =~ /\.gz$/) { open FQ1, "gzip -dc $fq1 |" or die "Can't gzip -dc $fq1 ($!)\n"; }
else                 { open FQ1, "< $fq1"          or die "Can't open $fq1 ($!)\n"; }

if ($fq2 =~ /\.gz$/) { open FQ2, "gzip -dc $fq2 |" or die "Can't gzip -dc $fq2 ($!)\n"; }
else                 { open FQ2, "< $fq2"          or die "Can't open $fq2 ($!)\n"; }


open FQ1M, "> $fq1.matched" or die "Can't open $fq1.matched ($!)\n";
open FQ1O, "> $fq1.orphans" or die "Can't open $fq1.orphans ($!)\n";

open FQ2M, "> $fq2.matched" or die "Can't open $fq2.matched ($!)\n";
open FQ2O, "> $fq2.orphans" or die "Can't open $fq2.orphans ($!)\n";


## index reads in fq1
###############################################################################
print STDERR "Indexing $fq1... ";

my $fq1cnt = 0;

while (<FQ1>)
{
  next if /^$/;

  if (/^@(\S+)/)
  {
    my $readname = $1;
    my $base = substr($readname, 0, -2);
    $basenames{$base} = 1;

    print "saw $base $readname\n" if $VERBOSE;
    $fq1cnt++;

    $_ = <FQ1>; ## seq
    $_ = <FQ1>; ## h2
    $_ = <FQ1>; ## qv
  }
  else
  {
    die "Malformed fastq $_\n";
  }
}

close FQ1;
print STDERR "$fq1cnt loaded\n";



## Report reads in fq2 that were in fq1
###############################################################################
print STDERR "Scanning $fq2... ";

my $fq2saw  = 0;
my $fq2keep = 0;
my $fq2orphans = 0;

while (<FQ2>)
{
  next if (/^$/);

  if (/^@(\S+)/)
  {
    my $readname = $1;
    my $base = substr($readname, 0, -2);
    $fq2saw++;

    print "saw $base $readname\n" if $VERBOSE;

    if (exists $basenames{$base})
    {
      $fq2keep++;

      $basenames{$base} = 2;

      print FQ2M "\@$readname\n";

      $_ = <FQ2>; print FQ2M $_; ## seq
      $_ = <FQ2>; print FQ2M "+\n";
      $_ = <FQ2>; print FQ2M $_; ## qv
    }
    else
    {
      $fq2orphans++;

      print FQ2O "\@$readname\n";

      $_ = <FQ2>; print FQ2O $_; ## seq
      $_ = <FQ2>; print FQ2O "+\n";
      $_ = <FQ2>; print FQ2O $_; ## qv
    }
  }
  else
  {
    die "Malformed fastq $_\n";
  }
}

print STDERR "saw: $fq2saw keep: $fq2keep orphans: $fq2orphans\n";


## Now report reads in fq1 that were in fq2
###############################################################################

if ($fq1 =~ /\.gz$/) { open FQ1, "gzip -dc $fq1 |" or die "Can't gzip -dc $fq1 ($!)\n"; }
else                 { open FQ1, "< $fq1"          or die "Can't open $fq1 ($!)\n"; }

print STDERR "Printing $fq1... ";

my $fq1saw  = 0;
my $fq1keep = 0;
my $fq1orphans = 0;

while (<FQ1>)
{
  next if (/^$/);

  if (/^@(\S+)/)
  {
    my $readname = $1;
    my $base = substr($readname, 0, -2);
    $fq1saw++;

    if ($basenames{$base} == 2)
    {
      $fq1keep++;

      print FQ1M "\@$readname\n";

      $_ = <FQ1>; print FQ1M $_; ## seq
      $_ = <FQ1>; print FQ1M "+\n";
      $_ = <FQ1>; print FQ1M $_; ## qv
    }
    else
    {
      $fq1orphans++;

      print FQ1O "\@$readname\n";

      $_ = <FQ1>; print FQ1O $_;  ## seq
      $_ = <FQ1>; print FQ1O "+\n";
      $_ = <FQ1>; print FQ1O $_;  ## qv
    }
  }
  else
  {
    die "Malformed fastq $_\n";
  }
}

print STDERR "saw: $fq1saw keep: $fq1keep orphans: $fq1orphans\n";
