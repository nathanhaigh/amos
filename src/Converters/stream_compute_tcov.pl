#!/usr/bin/perl -w
use strict;

my $redcnt = 0;
my $MAXREADS = 40;

my $pid = $$;

sub openFiles
{
  open FRG, "> $pid.frg" or die "Can't open $pid.frg";
  open ASM, "> $pid.asm" or die "Can't open $pid.asm";
}


sub printTCov
{
  my $ctgid = shift;
  my $final = shift;

  print STDERR "Printing $ctgid\n";

  if ($redcnt >= $MAXREADS || $final)
  {
    close FRG;
    close ASM;

    my $cmd0 = "(toAmos -f $pid.frg -a $pid.asm -o - | "
             . "bank-transact -m - -b $pid.bnk -c) >& /dev/null";

    my $rc0 = system($cmd0);
    if ($rc0) { die "Can't run $cmd0 for $ctgid ($!)\n"; }

    my $cmd1 = "analyzeSNPs  -b $pid.bnk/ -T -e -a | tr ' ' '\t'";
    my $rc1 = system($cmd1);
    if ($rc1) { die "Can't run $cmd1 for $ctgid ($!)\n"; }

    system("rm -rf $pid.bnk");

    openFiles();

    $redcnt = 0;
  }
}


my $ctgid;
openFiles();

while (<>)
{
  chomp;
  my @vals = split /\t/, $_;

  my $curctg = shift @vals;
  my $type   = shift @vals;

  if (defined $ctgid && $ctgid ne $curctg)
  {
    printTCov($ctgid,0);
  }

  $ctgid = $curctg;

  if ($type eq "CCO") 
  {
    print ASM "{CCO\n";
    foreach my $l (@vals) { print ASM "$l\n"; }
    print ASM "}\n";
  }
  elsif (($type eq "CTGFRG") || ($type eq "DEGFRG"))
  {
    my $acc = shift @vals;
    my $ft  = shift @vals;

    $redcnt++;

    print FRG "{FRG\n";
    foreach my $l (@vals) { print FRG "$l\n"; }
    print FRG "}\n";
  }
  else 
  {
    die "Unknown message: $_\n";
  }

  
}

printTCov($ctgid, 1);

system("rm -f $pid.frg $pid.asm");
