#!/usr/bin/perl -w
use strict;

my $USAGE = "persistant-assembly.pl prefix [start=0 [end=20 [inc=1]]]\n";
my $RUNCA = "~mschatz/bin/runca.amos.opteron";
my $EXEC = 1;


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

die "$prefix.frg is missing" if (! -r "$prefix.frg");

my $BASEDIR    = "base";
my $AMOSDIR    = "AMOS";
my $PERSISTDIR = "persist";

sub runCmd
{
  my $cmd = shift;

  my $echo = shift;
  print "Running: $cmd\n" if defined $echo;

  if ($EXEC)
  {
    my $rc = system($cmd);
    if ($rc) { die "$cmd failed!\n"; }
  }
}


if (! -r "$BASEDIR/$AMOSDIR/$prefix.bnk")
{
  if (! -x $BASEDIR){ mkdir $BASEDIR; }
  chdir $BASEDIR;

  if (! -r "$prefix.asm")
  {
    print "Base Assembly\n";
    runCmd("ln -s ../$prefix.frg .");
    runCmd("$RUNCA $prefix", 1);
  }

  if (! -x $AMOSDIR) { mkdir $AMOSDIR; }
  chdir $AMOSDIR;

  if (! -r "$prefix.bnk")
  {
    runCmd("ln -s ../$prefix.frg");
    runCmd("ln -s ../$prefix.asm");
    runCmd("cavalidate $prefix", 1);
  }

  chdir "../..";
}

if (! -x $PERSISTDIR) { mkdir $PERSISTDIR; }
chdir $PERSISTDIR;


foreach my $file ("$prefix.frg", 
                  "$prefix.frgStore", 
                  "$prefix.gkpStore", 
                  "$prefix.ofg", 
                  "$prefix.ovlStore")
{
  runCmd("ln -s ../$BASEDIR/$file $file") if (! -r $file);
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

    runCmd("$RUNCA $prefix -D UNITIGERROR=$eval -s 80", 1);
  }

  mkdir "AMOS" if (! -x "AMOS");
  chdir "AMOS";

  if (! -r "$prefix.bnk")
  {
    runCmd("ln -s ../$prefix.frg");
    runCmd("ln -s ../$prefix.asm");
    runCmd("cavalidate $prefix", 1);
  }

  if (-r "../../../$prefix.1con" && ! -r "refalign.delta.q")
  {
    runCmd("nucmer ../../../$prefix.1con $prefix.fasta -p refalign", 1);
    runCmd("delta-filter -q refalign.delta > refalign.delta.q", 1);
  }

  chdir "..";
  chdir "..";
}
