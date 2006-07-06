#!/usr/bin/perl -w
use strict;

my $run = 1;
my $FIX =  "~/build/AMOS/src/Persistant/persistent-fix-contigs";
my $MATE = "~/build/AMOS/src/Persistant/mate-evolution";

my $CMD = $MATE;

my $USAGE = "persistent-fix.pl file start end [flags]\n";

die $USAGE if scalar @ARGV < 3;

my $file  = shift @ARGV;
my $start = shift @ARGV;
my $end   = shift @ARGV;
my $flags = join " ", @ARGV;


my $base = sprintf("%04d", $start);
$base .= "/AMOS/$file.bnk";

die "$base not accessible\n" if (! -x $base);


$flags = "-G" if !defined $flags;

my $inc = 1;
if ($end < $start) { $inc = -1; }

while ($start != $end)
{
  my $mlevel = sprintf("%04d", $start+$inc);
  my $plevel = sprintf("%04d", $start);

  print STDERR "#### Fixing $mlevel with $plevel\n";
  my $cmd = "$CMD $flags $mlevel/AMOS/$file.bnk $plevel/AMOS/$file.bnk";
  print "cmd: $cmd\n";


  if ($run)
  {
    my $rc = system($cmd);
    die "Command failed: $!\n" if $rc;
  }

  $start += $inc;

  print "\n\n";
}

print STDERR "Done.\n\n";
