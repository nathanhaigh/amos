#!/usr/bin/perl -w
use strict;

my $INTERVAL_WIGGLE = 1000;
my $CE_RANGE = 10;

my $mean = 2244;
my $stdev = 250;

my $USAGE = "check_ce.pl regions ce > region.ce\n";

my $regions = shift @ARGV or die $USAGE;
my $cefile  = shift @ARGV or die $USAGE;



## Load regions of interest
###############################################################################

my %regions;

open REGIONS, "< $regions" or die "Can't open $regions ($!)\n";

my $regioncnt = 0;

while (<REGIONS>)
{
  chomp;

  my @vals = split /\s+/, $_;

  my $seq   = $vals[0];
  my $start = $vals[1];
  my $end   = $vals[2];
  my $delta = $vals[11];

  my $interval;
  $interval->{s}       = $start;
  $interval->{e}       = $end;
  $interval->{delta}   = $delta;
  $interval->{details} = $_;

  $regioncnt++;

  push @{$regions{$seq}}, $interval;
}

my $seqs = scalar keys %regions;

print STDERR "Loaded $regioncnt regions on $seqs sequences\n";



## Load the ce stats for these regions
###############################################################################

open CE, "< $cefile" or die "Can't open $cefile ($!)\n";

while (<CE>)
{
  next if (/^>/);

  chomp;

  my ($seq, $pos, $ce, $depth) = split /\s+/, $_;

  if (exists $regions{$seq})
  {
    foreach my $i (@{$regions{$seq}})
    {
      if ((($i->{s} - $INTERVAL_WIGGLE) <= $pos) && 
           ($pos <= ($i->{e} + $INTERVAL_WIGGLE)))
      {
        push @{$i->{ce}}, $_
      }
    }
  }
}



## Now print the regions and ce stats
###############################################################################

foreach my $seq (keys %regions)
{
  foreach my $i (@{$regions{$seq}})
  {
    my $details = $i->{details};

    my $ce = 0;
    my $depth = 0;
    my $ceidx = 0;
    my $nce = 0;

    if (exists $i->{ce})
    {
      #print ">$details\n";

      $nce = scalar @{$i->{ce}};

      while ($ceidx < $nce)
      {
        my ($seq, $pos, $pce, $pdepth) = split /\s+/, $i->{ce}->[$ceidx];
        #print " - $ceidx ",$i->{ce}->[$ceidx], "\n";;

        if ($pos > $i->{s}) ## first one after start
        {
          $ce = $pce;
          $depth = $pdepth;
          last;
        }

        $ceidx++;
      }
    }

    my $newce = ($depth > 0) ? $ce + ($i->{delta}/($stdev/sqrt($depth))) : $ce;
    $newce = sprintf("%0.03f", $newce);
    print ">$details\t|\t$ce\t$depth\t|\t$newce\n";

    if ($nce)
    {
      my $ii = $ceidx - $CE_RANGE;
      if ($ii < 0) { $ii = 0; }

      my $jj = $ceidx + $CE_RANGE;
      if ($jj > $nce) { $jj = $nce; }

      for (my $x = $ii; $x < $jj; $x++)
      {
        my $details = $i->{ce}->[$x];
        print " $details\n";
      }
    }
  }
}




