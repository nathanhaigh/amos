#!/usr/bin/perl -w
use strict;

my $redcnt = 0;
my $dupread = 0;
my %seenreads;
my $MAXREADS = 20000;

my $TOAMOS       = "./toAmos";
my $BANKTRANSACT = "./bank-transact";
my $ANALYZESNPS  = "./analyzeSNPs";

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

    my $cmd0 = "($TOAMOS -f $pid.frg -a $pid.asm -o - | "
             . "$BANKTRANSACT -m - -b $pid.bnk -c)";

    my $rc0 = system($cmd0);
    if ($rc0) 
    { 
      system("cat $pid.frg 1>&2");
      system("cat $pid.asm 1>&2");
      
      die "Can't run $cmd0 for $ctgid ($!)\n"; 
    }

    my $cmd1 = "($ANALYZESNPS  -b $pid.bnk/ -T -e -a | tr ' ' '\t') 2> $pid.err";
    my $rc1 = system($cmd1);
    if ($rc1) { die "Can't run $cmd1 for $ctgid ($!)\n"; }

    system("rm -rf $pid.bnk");

    open ERR, "< $pid.err" or die "Can't open $pid.err ($!)\n";
    while (<ERR>)
    {
      print STDERR $_;
      if (/(\d+) positions reported of (\d+) total SNPs found./)
      {
        my $pos  = $1;
        my $snps = $2;

        print STDERR "reporter:counter:tcov,positions,$pos\n";
        print STDERR "reporter:counter:tcov,snps,$snps\n";
      }
      elsif (/Searched \d+ positions in (\d+) contigs./)
      {
        my $ctgs = $1;
        print STDERR "reporter:counter:tcov,contigs,$ctgs\n";
      }
    }

    system("rm -f $pid.err");

    openFiles();

    $redcnt = 0;
    undef %seenreads;
  }
}


my $ctgid;
openFiles();

system("chmod +x $TOAMOS");
system("chmod +x $BANKTRANSACT");
system("chmod +x $ANALYZESNPS");

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

    if (exists $seenreads{$acc})
    {
      $dupread++;
    }
    else
    {
      $seenreads{$acc} = 1;
      $redcnt++;

      my $insrc = 0;

      print FRG "{FRG\n";
      foreach my $l (@vals) 
      { 
        if ($l =~ /^src:/)
        {
          #Strip out the src field
          $insrc = 1;
        }
        elsif ($insrc)
        {
          if ($l =~ /^\./)
          {
            $insrc = 0;
          }
        }
        else
        {
          print FRG "$l\n"; 
        }
      }
      print FRG "}\n";
    }
  }
  else 
  {
    die "Unknown message: $_\n";
  }

  
}

printTCov($ctgid, 1);

system("rm -f $pid.frg $pid.asm");

print STDERR "reporter:counter:tcov,dupread,$dupread\n";
