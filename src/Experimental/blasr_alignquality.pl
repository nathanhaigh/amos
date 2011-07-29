#!/usr/bin/perl -w
use strict;

my %stats;
my %dist;

my $curMatch;
my $curMis;
my $curIns;
my $curDel;
my $curLen;

my $qrystr;
my $matchstr;
my $refstr;

my $inAlign = 0;

my $MIN_ALIGN_LEN = 100;


sub processDist
{
  return if !$inAlign;
  $inAlign = 0;

  return if $curLen < $MIN_ALIGN_LEN;

  $stats{len} += $curLen;
  $stats{m}   += $curMatch;
  $stats{s}   += $curMis;
  $stats{i}   += $curIns;
  $stats{d}   += $curDel;

  my @mchr = split //, $matchstr;
  my $ml = scalar @mchr;

  print "l: $curLen m: $curMatch s: $curMis i: $curIns d: $curDel\n";
  print "$matchstr\n";

  print "d: ";

  my $idx = 0;
  while (($idx < $ml) && ($mchr[$idx] eq "|")) { $idx++; }

  my $d = -1;
  $idx++;
  while ($idx < $ml)
  {
    $d++;
    if ($mchr[$idx] eq " ") 
    { print " $d"; $dist{$d}++; $d = -1; }
    $idx++;
  }

  print "\n";

  #print "$qrystr\n";
  #print "$refstr\n";

  $qrystr = "";
  $matchstr = "";
  $refstr = "";
}

while (<>)
{
  next if (/^\s*$/);

  if    (/nMatch: (\d+)/)    { processDist(); $curMatch = $1;}
  elsif (/nMisMatch: (\d+)/) { $curMis = $1; }
  elsif (/nIns: (\d+)/)      { $curIns = $1; }
  elsif (/nDel: (\d+)/)      { $curDel = $1; }
  elsif (/QueryRange: (\d+) -> (\d+)/) { $curLen = $2 - $1;}
  elsif (/TargetRange:/)     { $inAlign = 1; }
  elsif ($inAlign)
  {
    my $qrystrs = $_;
    my $markers = <>;
    my $refstrs = <>; 
    my $blank = <>;
    
    print $qrystrs;
    print $markers;
    print $refstrs;
    print $blank;

    chomp $qrystrs;
    chomp $markers;
    chomp $refstrs;

    $qrystr   .= substr($qrystrs, 8);
    $matchstr .= substr($markers, 8);
    $refstr   .= substr($refstrs, 8);
  }

  print $_ if (!$inAlign);
}

processDist();


my $len   = $stats{len};
my $match = $stats{m};
my $mis   = $stats{s};
my $ins   = $stats{i};
my $del   = $stats{d};

my $sum = $match + $mis + $ins + $del;

my $mr = $match / $sum;
my $sr = $mis / $sum;
my $ir = $ins / $sum;
my $dr = $del / $sum;

print "len:\t$len\t$sum\n";
print "match:\t$match\t$mr\n";
print "mis:\t$mis\t$sr\n";
print "ins:\t$ins\t$ir\n";
print "del:\t$del\t$dr\n";


print "seed len\n";
foreach my $d (sort {$a <=> $b} keys %dist)
{
  my $cnt = $dist{$d};
  print "$d\t$cnt\n";
}



