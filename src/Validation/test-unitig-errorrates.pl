#!/usr/local/bin/perl -w
use strict;

my $USAGE = "test-unitiger.pl prefix [start=0 [end=20 [inc=1]]]\n";

sub runCmd
{
  my $cmd = shift;
  my $rc = system($cmd);
  if ($rc) { die "$cmd failed!\n"; }
}


my $prefix = shift @ARGV;
my $start  = shift @ARGV;
my $end    = shift @ARGV;
my $inc    = shift @ARGV;

die $USAGE if !defined $prefix;
$start = 0  if !defined $start;
$end   = 20 if !defined $end;
$inc   = 1  if !defined $inc;

for (my $i=$start; $i <= $end; $i+=$inc)
{
  my $val = sprintf("%0.03f", $i/1000);
  my $s = $val;
  $s =~ s/\.//;

  print "$val $s\n";

  if (-x $s)
  {
    print "Exists!\n";
    next;
  }

  mkdir $s;
  chdir $s;

    runCmd("ln -s ../$prefix.frg .");
    runCmd("sed 's/UNITIGERROR/$val/' ../runca.amos.opteron.tmpl > runca.amos.opteron");
    runCmd("runAmos -C runca.amos.opteron $prefix");

  chdir "..";
}
