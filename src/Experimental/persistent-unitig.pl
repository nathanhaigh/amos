#!/usr/bin/perl -w
use strict;

my $USAGE = "persistant-assembly.pl prefix persist.spec start=0 [end=20 [inc=1]]]\n";
my $RUNCA = "~/build/packages/wgs-6.1/Linux-amd64/bin/runCA";
my $EXEC = 1;

my $prefix = shift @ARGV;
my $spec   = shift @ARGV;
my $start  = shift @ARGV;
my $end    = shift @ARGV;
my $inc    = shift @ARGV;

die $USAGE if !defined $spec;
$start = 0  if !defined $start;
$end   = 20 if !defined $end;
$inc   = 1  if !defined $inc;

my $BASEDIR    = "asm";
my $PERSISTDIR = "persist";

########################################

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

########################################

if (! -x $PERSISTDIR) { mkdir $PERSISTDIR; }
chdir $PERSISTDIR;

if (! -r "foo.frg") { runCmd("touch foo.frg"); }

for (my $i=$start; $i <= $end; $i+=$inc)
{
  my $enum = sprintf("%0.03f", $i/1000);
  my $eval = $enum;
  $eval =~ s/\.//;

  print "Eval: $eval $enum\n";

  mkdir $eval if (! -x $eval);

  foreach my $file ("0-mercounts",
                    "0-overlaptrim",
                    "0-overlaptrim-overlap",
                    "1-overlapper",
                    "3-overlapcorrection",
                    "$prefix.ovlStore")
  {
    runCmd("ln -s ../../$BASEDIR/$file $eval/$file") if (! -r "$eval/$file");
  }

  if (! -r "$eval/$prefix.gkpStore")
  {
    mkdir "$eval/$prefix.gkpStore";

    foreach my $f (`/bin/ls ../$BASEDIR/$prefix.gkpStore/`)
    {
      chomp $f;
      if ($f =~ "clr.*ECR") { print " skip: $f\n"; }
      else                  { runCmd("ln -s ../../../$BASEDIR/$prefix.gkpStore/$f $eval/$prefix.gkpStore/"); }
    }
  }

  if (! -r "$eval/4-unitigger")
  {
    runCmd("$RUNCA -d $eval -p asm -s ../$spec foo.frg utgErrorRate=$enum stopAfter=unitigger >& $eval.log", 1);
  }
}
