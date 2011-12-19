#!/usr/bin/perl -w
use strict;
use Getopt::Long;

## Options
my $help  = 0;
my $MIN_ALIGN_LEN = 1000;
my $VERBOSE = 0;

my $USAGE = "blasr_alignquality <options> m0alignments\n"; 

my $res = GetOptions("help"      => \$help,
                     "min=s"     => \$MIN_ALIGN_LEN,
                     "verbose"   => \$VERBOSE,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print "  Scan blasr alignments (-m 0) and compute statistics on the quality.\n";
  print "\n";
  print "Options\n";
  print "  -min <len> : minimum alignment length to process (default: $MIN_ALIGN_LEN)\n";
  print "  -verbose   : be verbose when parsing\n";
  exit 0;
}


my %stats;
my %dist;

my $curMatch;
my $curMis;
my $curIns;
my $curDel;
my $curLen;

my $astart;
my $aend;

my $qrystr;
my $matchstr;
my $refstr;

my $inAlign = 0;

my @alignedbases;
my @errorbases;

my $maxreadlen = 10000;

for (my $i = 0; $i < $maxreadlen; $i++)
{
  $alignedbases[$i] = 0;
  $errorbases[$i] = 0;
}



sub processDist
{
  return if !$inAlign;
  $inAlign = 0;

  $stats{all}++;

  if ($curLen < $MIN_ALIGN_LEN)
  {
    print "too short\n" if $VERBOSE;
  }
  else
  {
    print "evaluating\n" if $VERBOSE;

    $stats{good}++;

    $stats{len} += $curLen;
    $stats{m}   += $curMatch;
    $stats{s}   += $curMis;
    $stats{i}   += $curIns;
    $stats{d}   += $curDel;

    my @mchr = split //, $matchstr;
    my $ml = scalar @mchr;


    if ($VERBOSE)
    {
      print "l: $curLen m: $curMatch s: $curMis i: $curIns d: $curDel\n";
      print "$qrystr\n";
      print "$matchstr\n";
      print "$refstr\n";
      print "d: ";
    }

    my $idx = 0;
    while (($idx < $ml) && ($mchr[$idx] eq "|")) { $idx++; $alignedbases[$astart+$idx]++; }

    my $d = -1;
    $idx++;

    while ($idx < $ml)
    {
      $d++;
      $alignedbases[$astart+$idx]++;

      if ($mchr[$idx] eq " ") 
      { 
        $errorbases[$astart+$idx]++;
        print " $d" if ($VERBOSE); 
        $dist{$d}++; $d = -1; 
      }
      $idx++;
    }

    print "\n" if ($VERBOSE);
  }

  print "======================================================================\n" if $VERBOSE;

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
  elsif (/QueryRange: (\d+) -> (\d+)/) { $astart = $1; $aend = $2; $curLen = $aend - $astart;}
  elsif (/TargetRange:/)     { $inAlign = 1; }
  elsif ($inAlign)
  {
    my $qrystrs = $_;
    my $markers = <>;
    my $refstrs = <>; 
    my $blank = <>;
    
    if ($VERBOSE)
    {
      print $qrystrs;
      print $markers;
      print $refstrs;
      print $blank;
    }

    chomp $qrystrs;
    chomp $markers;
    chomp $refstrs;

    my @qq = split //, $qrystrs;

    my $offset = 0;
    
    my $state=1;

    for (my $i = 0; $i < scalar(@qq); $i++)
    {
    #  print "$state $i: \"$qq[$i]\"\n";

      if    ($state == 1) { if ($qq[$i] ne " ") { $state = 2; } }
      elsif ($state == 2) { if ($qq[$i] eq " ") { $state = 3; } }
      elsif ($state == 3)
      {
        if ($qq[$i] ne " ")
        {
          $offset = $i;
          $state = 4;
          last;
        }
      }
    }

    die "weird: $state \"$qrystrs\"" if ($state != 4);

    #print "offset: $offset\n";
    #print "$qrystrs\n";
    #print "$markers\n";
    #print "$refstrs\n";

    $qrystr   .= substr($qrystrs, $offset);
    $matchstr .= substr($markers, $offset);
    $refstr   .= substr($refstrs, $offset);

    #print "$qrystr\n";
    #print "$matchstr\n";
    #print "$refstr\n";
  }

  print $_ if ($VERBOSE && !$inAlign);
}

processDist();

print "========================================================================\n" if $VERBOSE;
print "========================================================================\n" if $VERBOSE;
print "\n" if $VERBOSE;


my $all   = $stats{all};
my $good  = $stats{good};
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

print "alignments:\t$good\t$all\n";
print "len:\t$len\t$sum\n";
print "match:\t$match\t$mr\n";
print "mis:\t$mis\t$sr\n";
print "ins:\t$ins\t$ir\n";
print "del:\t$del\t$dr\n";

print "\n";


print "seed len\n";
foreach my $d (sort {$a <=> $b} keys %dist)
{
  my $cnt = $dist{$d};
  print "$d\t$cnt\n";
}

print "\n";

print "aligned bases\n";

for (my $i = 0; $i < $maxreadlen; $i++)
{
  my $aligned = $alignedbases[$i];
  my $errors  = $errorbases[$i];

  my $perc = sprintf("%0.02f", ($aligned) ? $errors/$aligned : 0);

  print "$i\t$errors\t$aligned\t$perc\n";
}






