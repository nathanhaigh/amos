#!/usr/bin/perl -w
use strict;
use POSIX;
use Getopt::Long;

$| = 1;

my $USAGE = "ovlsimulate [options] [-numreads n] [-erate e] [-cov c] -readlen l\n";

my $help;

my $V = 0;
my $showseedhist = 0;
my $minovl       = 20;
my $readlen      = undef;
my $erate        = .15;
my $numreads     = 100;
my $covstart     = undef;
my $covend       = undef;
my $covdisp      = 10;

my $res = GetOptions("help"       => \$help,
                     "verbose"    => \$V,
                     "readlen=n"  => \$readlen,
                     "erate=s"    => \$erate,
                     "cov=n"      => \$covdisp,
                     "covstart=n" => \$covstart,
                     "covend=n"   => \$covend,
                     "minovl=n"   => \$minovl,
                     "seedhist"   => \$showseedhist);
 
if ($help)
{
  print $USAGE;
  print "\n";
  print "  Simulate a perfect multiple alignment of <numreads> reads, each <readlength> long. Introduce errors\n";
  print "  at the specified <erate>, to compute the consensus accuracy and the fraction of overlaps detected.\n";
  print "\n";
  print "Read Parameters\n";
  print "  -readlen <l>  : length of reads to simulate\n";
  print "  -numreads <n> : number of reads to simulate (default: $numreads)\n";
  print "  -erate <e>    : sequencing error rate to use (default: $erate)\n";
  print "  -minovl <len> : minimum exact seed length to detect overlap (default: $minovl)\n";
  print "\n";
  print "Options\n";
  print "  -verbose       : be verbose\n";
  print "  -seedhist      : display the histogram of max overlap seed length\n";
  print "  -cov <n>       : use this single coverage level (default: $covdisp)\n";
  print "  -covstart <s>  : compute range of coverage values with this lower bound\n";
  print "  -covend <e>    : compute range of coverage values with this upper bound\n";
  exit 0;
}

srand(12345);

die $USAGE if !defined $readlen;


sub min { return $_[0] < $_[1] ? $_[0] : $_[1]; }
sub max { return $_[0] > $_[1] ? $_[0] : $_[1]; }

if ((defined $covstart && !defined $covend) || (defined $covend && !defined $covstart))
{
  die "ERROR: Must define both cov start and end\n";
}

if (!defined $covstart)
{
  $covstart = $covdisp;
  $covend   = $covdisp;
}


for (my $cov = $covstart; $cov <= $covend; $cov++)
{
  my $arate = int($readlen / $cov);
  my $span = int(($numreads-1) * $arate + $readlen);

  if ($V)
  {
    print "Simulating $numreads reads rl=$readlen span=$span cov=$cov arate=$arate erate=$erate minovl=$minovl\n";
    print "pos : "; for (my $p = 0; $p < $span; $p++) { my $i = ($p/100) % 10; print $i; } print "\n";
    print "      "; for (my $p = 0; $p < $span; $p++) { my $i = ($p/10) % 10;  print $i; } print "\n";
    print "      "; for (my $p = 0; $p < $span; $p++) { my $i = $p % 10;       print $i; } print "\n";
    print "\n";
  }

  my @layout;
  my @matrix;
  my @cns; for (my $p = 0; $p < $span; $p++) { $cns[$p]->{X} = 0; $cns[$p]->{M} = 0; }

  my @maxseed;


  my $totalbp  = 0;
  my $totalerr = 0;

  for (my $r = 0; $r < $numreads; $r++)
  {
    my $start = $r*$arate;
    my $end   = $start + $readlen; 

    $layout[$r]->{s} = $start;
    $layout[$r]->{e} = $end;

    for (my $i = $start; $i < $end; $i++) 
    { 
      my $adderr = rand() <= $erate;
      $totalbp++;

      if ($adderr)
      {
        $totalerr++;

        $matrix[$r]->[$i-$start] = "X"; 
        $cns[$i]->{X}++;
      }
      else
      {
        $matrix[$r]->[$i-$start] = "."; 
        $cns[$i]->{M}++;
      }
    }

    if ($V)
    {
      printf ("% 4d: ", $r); print " " x $start; print join("", @{$matrix[$r]}); print "\n";
    }
  }


  my $cnsbad = 0;
  my $goodspan = 0;
  my $goodcovbad = 0;

  if ($V) { print "\ncns : ";  }

  for (my $p = 0; $p < $span; $p++) 
  { 
    my $c = "."; 

    my $hasgood = 0;

    if (($cns[$p]->{X} + $cns[$p]->{M}) == $cov)
    {
      $goodspan++;
      $hasgood = 1;
    }

    if ($cns[$p]->{X} > $cns[$p]->{M})
    {
      $c = "x"; $cnsbad++;
      if ($hasgood) { $c = "X"; $goodcovbad++; }
    }

    if ($V) { print $c; }
  } 

  if ($V) { print "\n"; }


  my $ovlshould = 0;
  my $ovlhas    = 0;

  for (my $i = 0; $i < $numreads; $i++)
  {
    my $li = $layout[$i];

    my $i_should = 0;
    my $i_has    = 0;

    if ($V) { printf ("% 4d: ", $i); }

    for (my $j = 0; $j < $numreads; $j++)
    {
      next if $i == $j;

      my $lj = $layout[$j];

      last if ($lj->{s} > $li->{e});

      my $os = max($li->{s}, $lj->{s});
      my $oe = min($li->{e}, $lj->{e});

      if ($oe - $os >= $minovl)
      {
        $ovlshould++;
        $i_should++;
        
        if ($V) { print "\n $j [$os $oe]: "; }

        my $ss = $os;
        my $sl = 0;
        my $maxsl = 0;

        for (my $oo = $os; $oo < $oe; $oo++)
        {
          if (($matrix[$i]->[$oo-$li->{s}] eq ".") &&
              ($matrix[$j]->[$oo-$lj->{s}] eq "."))
          {
            if ($sl == 0) { $ss = $oo; }
            $sl++;
          }
          else
          {
            if ($V) { if ($sl > 0) { print " [$ss $sl]"; } }

            if ($sl > $maxsl)
            {
              $maxsl = $sl;
            }

            $sl = 0;
          }
        }

        if ($V) { if ($sl > 0) { print " [$ss $sl]"; } }

        if ($sl > $maxsl)
        {
          $maxsl = $sl;
        }

        if ($maxsl >= $minovl)
        {
          if ($V) { print " +$maxsl"; }
          $i_has++; $ovlhas++;
        }
        else
        {
          if ($V) { print " -$maxsl"; }
        }

        $maxseed[$sl]++;
      }
    }

    if ($V) { print "\n ===> $i_should $i_has\n"; }
  }

  my $emperate     = sprintf("%.02f", 100*$totalerr / $totalbp);
  my $cnserate     = sprintf("%.02f", 100*$cnsbad / $span);
  my $goodcoverate = sprintf("%.02f", 100*$goodcovbad / $goodspan);
  my $ovlrate      = sprintf("%.02f", ($ovlshould) ? 100*$ovlhas/$ovlshould : 0);

  print "numread\t$numreads\tcov\t$cov\tspan\t$span\tarate\t$arate\terate\t$erate\temperate\t$emperate\tcnserr\t$goodcoverate\tminovl\t$minovl\tovlshould\t$ovlshould\tovlrate\t$ovlrate\n";

  if ($showseedhist)
  {
    foreach my $ms (0..scalar @maxseed)
    {
      my $cnt = (defined $maxseed[$ms]) ? $maxseed[$ms] : 0;
      print "$ms\t$cnt\n";
    }
  }
  
  if($V) { print "\n\n"; }
}
