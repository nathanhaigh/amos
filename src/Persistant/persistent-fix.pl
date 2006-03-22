#!/usr/bin/perl -w
use strict;

my $run = 1;
my $FIX = "~/build/AMOS/src/Persistant/persistent-fix-contigs";

my $USAGE = "persistent-fix.pl file start end [flags]\n";

die $USAGE if scalar @ARGV < 3;

my $file  = shift @ARGV;
my $start = shift @ARGV;
my $end   = shift @ARGV;
my $flags = join " ", @ARGV;

$flags = "-G" if !defined $flags;

my $inc = 1;
if ($end < $start) { $inc = -1; }

while ($start != $end)
{
  my $mlevel = sprintf("%04d", $start+$inc);
  my $plevel = sprintf("%04d", $start);

  print "#### Fixing $mlevel with $plevel\n";
  my $cmd = "$FIX $flags $mlevel/AMOS/$file.bnk $plevel/AMOS/$file.bnk";
  print "cmd: $cmd\n";


  if ($run)
  {
    my $rc = system($cmd);
    die "Command failed: $!\n" if $rc;
  }

  $start += $inc;

  print "\n\n";
}
