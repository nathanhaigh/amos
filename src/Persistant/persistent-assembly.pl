#!/usr/bin/perl -w
use strict;

my $USAGE = "persistant-assembly.pl prefix [start=0 [end=20 [inc=1]]]\n";

sub runCmd
{
  my $cmd = shift;

  my $echo = shift;
  print "Running: $cmd\n" if defined $echo;

  my $rc = system($cmd);
  if ($rc) { die "$cmd failed!\n"; }
}

my $uname = `uname -p`;
die "Must run on opteron" if (!($uname =~ /x86_64/));


my $prefix = shift @ARGV;
my $start  = shift @ARGV;
my $end    = shift @ARGV;
my $inc    = shift @ARGV;

die $USAGE if !defined $prefix;
$start = 0  if !defined $start;
$end   = 20 if !defined $end;
$inc   = 1  if !defined $inc;

foreach my $file ("$prefix.frg", 
                  "$prefix.frgStore", 
                  "$prefix.gkpStore", 
                  "$prefix.ofg", 
                  "$prefix.ovlStore", 
                  "runca.amos.opteron")
{
  die "Required file $file is missing\n" if (! -r $file);
}


for (my $i=$start; $i <= $end; $i+=$inc)
{
  my $eval = sprintf("%0.03f", $i/1000);
  $eval =~ s/\.//;

  print "Eval: $eval\n";

  mkdir $eval if (! -x $eval);
  chdir $eval;

  if (! -r "$prefix.asm")
  {
    runCmd("ln -s ../$prefix.frg .");
    runCmd("ln -s ../$prefix.frgStore .");
    runCmd("ln -s ../$prefix.gkpStore .");
    runCmd("ln -s ../$prefix.ofg .");
    runCmd("ln -s ../$prefix.ovlStore .");

    runCmd("../runca.amos.opteron $prefix -D UNITIGERROR=$eval -s 80", 1);
  }

  mkdir "AMOS" if (! -x "AMOS");
  chdir "AMOS";

  if (! -r "$prefix.bnk")
  {
    runCmd("ln -s ../$prefix.frg");
    runCmd("ln -s ../$prefix.asm");
    runCmd("cavalidate $prefix", 1);
  }

  chdir "..";
  chdir "..";
}
