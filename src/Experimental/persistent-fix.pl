#!/usr/bin/perl -w
use strict;

my $run = 1;
my $FIX =  "~/build/AMOS/src/Persistant/persistent-fix-contigs";
my $MATE = "~/build/AMOS/src/Persistant/mate-evolution";
my $AUTO = "~/build/AMOS/src/Persistant/auto-fix-contigs";

my $NUCMER = "nucmer";
my $FILTER = "delta-filter";

my $CMD = $AUTO;

my $USAGE = "persistent-fix.pl file val start end [flags]\n";

die $USAGE if scalar @ARGV < 4;

my $file  = shift @ARGV;
my $val   = shift @ARGV;
my $start = shift @ARGV;
my $end   = shift @ARGV;
my $flags = join " ", @ARGV;

my $base = sprintf("%04d", $start);
$base .= "/AMOS/$file.bnk";

die "$base not accessible\n" if (! -x $base);

my $ALLVALL = 1;



sub runCMD
{
  my $cmd = shift;
  my $desc = shift;
  if (defined $desc) { print "$desc\n"; }
  else               { print "cmd: $cmd\n"; }

  if ($run)
  {
    my $rc = system($cmd);
    die "Command failed: $!\n" if $rc;
  }
}

sub fix
{
  my $mlevel = shift;
  my $plevel = shift;

  $mlevel = sprintf("%04d", $mlevel);
  $plevel = sprintf("%04d", $plevel);

  print STDERR "#### Fixing $mlevel with $plevel\n";

  if (! -r "$mlevel/AMOS/$mlevel-$plevel.delta")
  {
    runCMD("($NUCMER $mlevel/AMOS/$file.fasta $plevel/AMOS/$file.fasta -p $mlevel/AMOS/$mlevel-$plevel) > /dev/null", "align");
    unlink "$mlevel/AMOS/$mlevel-$plevel.cluster";
  }

  runCMD("$FILTER -rq $mlevel/AMOS/$mlevel-$plevel.delta > $mlevel/AMOS/$mlevel-$plevel.delta.rq", "delta-filter")
    if (! -r "$mlevel/AMOS/$mlevel-$plevel.delta.rq");

  runCMD("$CMD $flags $mlevel/AMOS/$file.bnk $plevel/AMOS/$file.bnk $mlevel/AMOS/$mlevel-$plevel.delta.rq");

  print "\n\n";
}


my $starttime = time;

if ($ALLVALL)
{
    for (my $j = $start; $j <= $end; $j++)
    {
      next if ($val == $j);
      fix($val, $j);
    }
}
else
{
  my $inc = 1;
  if ($end < $start) { $inc = -1; }

  while ($start != $end)
  {
    fix($start, $start+$inc);
    $start += $inc;
  }
}

my $endtime = time;
my $delta = $endtime - $starttime;

print STDERR "Done in $delta s\n";
