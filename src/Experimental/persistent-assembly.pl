#!/usr/bin/perl -w
use strict;

my $USAGE = "persistant-assembly.pl prefix [start=0 [end=20 [inc=1]]]\n";
my $RUNCA = "~/build/wgs-assembler-4.2/Linux-amd64/bin/runCA";
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

if (! -x $PERSISTDIR) { mkdir $PERSISTDIR; }
chdir $PERSISTDIR;

runCmd("ln -s ../$prefix.frg") if ! -r "$prefix.frg";

for (my $i=$start; $i <= $end; $i+=$inc)
{
  my $eval = sprintf("%0.03f", $i/1000);
  $eval =~ s/\.//;

  print "Eval: $eval\n";

  if (! -x "$eval" || 
      ! -r "$eval/9-terminator/asm.asm" || 
        -r "$eval/0-mercounts")
  {
    runCmd("$RUNCA $prefix.frg -d $eval -s noVec utgErrorRate=$eval cleanup=aggressive 2>&1 | tee -a runCA.$eval.out", 1);
  }

  chdir $eval;

  mkdir "AMOS" if (! -x "AMOS");
  chdir "AMOS";

  if (! -r "$prefix.bnk")
  {
    runCmd("ln -s ../9-terminator/asm.frg");
    runCmd("ln -s ../9-terminator/asm.asm");
    runCmd("cavalidate asm", 1);
  }

  if (-r "../../../$prefix.1con" && ! -r "refalign.delta.q")
  {
    runCmd("nucmer ../../../$prefix.1con asm.fasta -p refalign", 1);
    runCmd("delta-filter -q refalign.delta > refalign.delta.q", 1);
  }

  chdir "..";
  chdir "..";
}
