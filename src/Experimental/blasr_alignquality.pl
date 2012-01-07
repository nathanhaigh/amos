#!/usr/bin/perl -w
use strict;
use Getopt::Long;

## Options
my $help  = 0;
my $MIN_ALIGN_LEN = 1000;
my $MASK_BP = 0;
my $VERBOSE = 0;
my $reffile = undef;

my $USAGE = "blasr_alignquality <options> m0alignments\n"; 

my $res = GetOptions("help"      => \$help,
                     "min=s"     => \$MIN_ALIGN_LEN,
                     "mask=s"    => \$MASK_BP,
                     "verbose"   => \$VERBOSE,
                     "ref=s"     => \$reffile,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print "  Scan blasr alignments (blasr -m 0) and compute statistics on the quality.\n";
  print "\n";
  print "Options\n";
  print "  -min <len>         : minimum alignment length to process (default: $MIN_ALIGN_LEN)\n";
  print "  -mask <len>        : don't consider the first or last len bp (default: $MASK_BP)\n";
  print "  -ref <ref.len.txt> : compute accuracy with respect to the reference in the specified regions\n";
  print "  -verbose           : be verbose when parsing\n";
  exit 0;
}


## Load the reference ids of interest
#####################################################################

my %refinfo;
my @refseqs;

if (defined $reffile)
{
  open LENS, "< $reffile" or die "Can't open $reffile ($!)\n";

  while (<LENS>)
  {
    chomp;
    my ($ref, $len) = split /\s+/, $_;
    $refinfo{$ref}->{len} = $len;

    push @refseqs, $ref;
  }

  close LENS;
}


## Initialize the stats
#####################################################################

my %stats;
$stats{all}  = 0;
$stats{good} = 0;
$stats{len}  = 0 ;
$stats{m}    = 0;
$stats{s}    = 0;
$stats{i}    = 0;
$stats{d}    = 0;

my %dist;

my $curMatch;
my $curMis;
my $curIns;
my $curDel;
my $curLen;

my $astart;
my $aend;

my $tstart;
my $tend;
my $target;

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


## Process an alignment
#####################################################################

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

    my $idx = $MASK_BP;
    while (($idx < $ml - $MASK_BP) && ($mchr[$idx] eq "|")) 
    { 
      $alignedbases[$astart+$idx]++; 

      if ((defined $reffile) && exists $refinfo{$target})
      {
        $refinfo{$target}->{depth}->[$tstart+$idx]++;
        $refinfo{$target}->{cor}->[$tstart+$idx]++;
      }

      $idx++; 
    }

    my $d = -1;
    $idx++;

    while ($idx < $ml-$MASK_BP)
    {
      $d++;
      $alignedbases[$astart+$idx]++;

      if ((defined $reffile) && exists $refinfo{$target})
      {
        $refinfo{$target}->{depth}->[$tstart+$idx]++;
      }

      if ($mchr[$idx] eq " ") 
      { 
        $errorbases[$astart+$idx]++;
        print " $d" if ($VERBOSE); 
        $dist{$d}++; $d = -1; 
      }
      else
      {
        if ((defined $reffile) && exists $refinfo{$target})
        {
          $refinfo{$target}->{cor}->[$tstart+$idx]++;
        }
      }

      $idx++;
    }

    if ($d>-1) { $dist{$d}++; }

    print "\n" if ($VERBOSE);
  }

  print "======================================================================\n" if $VERBOSE;

  $qrystr = "";
  $matchstr = "";
  $refstr = "";
}



## Scan the alignments
#####################################################################

print STDERR "Scanning alignments... ";
my $aligncnt = 0;

while (<>)
{
  next if (/^\s*$/);

  if    (/nMatch: (\d+)/)      { processDist(); $curMatch = $1;}
  elsif (/nMisMatch: (\d+)/)   { $curMis = $1; }
  elsif (/nIns: (\d+)/)        { $curIns = $1; }
  elsif (/nDel: (\d+)/)        { $curDel = $1; }
  elsif (/Target: (\S+)/)      { $target = $1; }
  elsif (/QueryRange: (\d+) -> (\d+)/)  { $astart = $1; $aend = $2; $curLen = $aend - $astart;}
  elsif (/TargetRange: (\d+) -> (\d+)/) { $tstart = $1; $tend = $2; $inAlign = 1; }
  elsif ($inAlign)
  {
    $aligncnt++;
    if ($aligncnt % 1000 == 0) { print STDERR "."; }

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

print STDERR " $aligncnt total alignments\n";




## Process the overall stats
#####################################################################

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

my $mr = ($sum) ? $match / $sum : 0;
my $sr = ($sum) ? $mis / $sum   : 0;
my $ir = ($sum) ? $ins / $sum   : 0;
my $dr = ($sum) ? $del / $sum   : 0;

print "alignments:\t$good\t$all\n";
print "len:\t$len\t$sum\n";
print "match:\t$match\t$mr\n";
print "mis:\t$mis\t$sr\n";
print "ins:\t$ins\t$ir\n";
print "del:\t$del\t$dr\n";

print "\n";


## Histogram of seed lengths
#####################################################################

print "seed len\n";
foreach my $d (sort {$a <=> $b} keys %dist)
{
  my $cnt = $dist{$d};
  print "$d\t$cnt\n";
}

print "\n";



## Print the fraction of correct aligned bases
#####################################################################

print "erroneous aligned bases\n";

for (my $i = 0; $i < $maxreadlen; $i++)
{
  my $aligned = $alignedbases[$i];
  my $errors  = $errorbases[$i];

  my $perc = sprintf("%0.02f", ($aligned) ? $errors/$aligned : 0);

  print "$i\t$errors\t$aligned\t$perc\n";
}



## Print quality with respect to the reference
#####################################################################

if (defined $reffile)
{
  print "\n\n";
  print "reference coverage and accuracy\n";

  my $gpos = 0;

  foreach my $ref (@refseqs)
  {
    my $l = $refinfo{$ref}->{len};

    for (my $i = 1; $i <= $l; $i++)
    {
      my $depth = (exists $refinfo{$ref}->{depth}->[$i]) ? $refinfo{$ref}->{depth}->[$i] : 0;
      my $cor   = (exists $refinfo{$ref}->{cor}->[$i])   ? $refinfo{$ref}->{cor}->[$i]   : 0;

      print "$gpos\t$ref\t$depth\t$cor\n";
      $gpos++;
    }
  }
}

