#!/usr/bin/perl -w

#
# Calculate and print "average" quality values for each position in a
# Solexa FASTQ file.
# 

use strict;
use warnings;
use Getopt::Long;

my $USAGE = "fastqqc.pl [-c numreads] [-r readlen] fq\n";

my $readlen = 75;
my $cutoff = undef;
my $printqvstats = 0;

my $result = GetOptions(
 "c=s"    => \$cutoff,
 "r=s"    => \$readlen,
 "qv"     => \$printqvstats,
);


my $filename = shift @ARGV or die $USAGE;
open(FQ, $filename) || die "Could not open .fq";

print STDERR "Analyzing $cutoff reads\n";

my $lines = 0;
my @mins;
my @maxs;
my @tots;

for (my $i = 0; $i < $readlen; $i++)
{
  $mins[$i] = 255;
  $maxs[$i] = 0;
  $tots[$i] = 0;
}

my %nsPerRead = ();
my %posCnts = ();


my $i = 0;
while(<FQ>) {
	$i++;
	next if ($i % 2) != 0;
	if(($i % 4) == 2) {
		# Sequence line
		my ($as, $cs, $gs, $ts, $ns) = (0, 0, 0, 0, 0);
		for(my $i = 0; $i < length($_) && $i < $readlen; $i++) {
			my $c = substr($_, $i, 1);
			if($c eq 'A') {
				$posCnts{"A$i"}++;
			} elsif($c eq 'C') {
				$posCnts{"C$i"}++;
			} elsif($c eq 'G') {
				$posCnts{"G$i"}++;
			} elsif($c eq 'T') {
				$posCnts{"T$i"}++;
			} elsif($c eq 'N') {
				$posCnts{"N$i"}++;
				$ns++;
			}
		}
		$nsPerRead{$ns}++;
	} else {
		# Quality line
		for(my $i = 0; $i < length($_) && $i < $readlen; $i++) {
			my $oi = (ord(substr($_, $i, 1)));
			$tots[$i] += $oi;
			if($oi < $mins[$i]) {
				$mins[$i] = $oi;
			}
			if($oi > $maxs[$i]) {
				$maxs[$i] = $oi;
			}
		}
		$lines++;
	}
	last if (defined $cutoff) && ($lines >= $cutoff);
}

print "Analyzed $lines reads\n";


if ($printqvstats)
{
# Print averages
  print "Average:\n";
  my $istr = "";
  my $i33str = "";
  my $i64str = "";
  for(my $i = 0; $i < $readlen; $i++) {
      last if $tots[$i] == 0;
      my $q = $tots[$i] * 1.0 / $lines;
      my $rq = int($q + 0.5);
      print chr($rq);
      $istr .= "$rq ";
      $i33str .= ($rq-33)." ";
      $i64str .= ($rq-64)." ";
  }
  print "\n$istr\n\n$i33str\n\n$i64str\n";
  print "\n";

# Print mins
  print "Min:\n";
  $istr = "";
  $i33str = "";
  $i64str = "";
  for(my $i = 0; $i < $readlen; $i++) {
      print chr($mins[$i]);
      my $rq = $mins[$i];
      $istr .= "$rq ";
      $i33str .= ($rq-33)." ";
      $i64str .= ($rq-64)." ";
  }
  print "\n$istr\n\n$i33str\n\n$i64str\n";
  print "\n";

# Print maxs
  print "Max:\n";
  $istr = "";
  $i33str = "";
  $i64str = "";
  for(my $i = 0; $i < $readlen; $i++) {
      print chr($maxs[$i]);
      my $rq = $maxs[$i];
      $istr .= "$rq ";
      $i33str .= ($rq-33)." ";
      $i64str .= ($rq-64)." ";
  }
  print "\n$istr\n$i33str\n$i64str\n";
  print "\n";
}

print "Ns per read:\n";
for my $k (sort {$a <=> $b} keys %nsPerRead) {
	print "  $k: $nsPerRead{$k}\n";
}

print "Base composition:\n";
print "pos       A      C      G      T      N      Q\n";
for(my $i = 0; $i < $readlen; $i++) {
    printf "%4d  ", $i+1;
	for my $k ('A', 'C', 'G', 'T', 'N') {
		printf "%02.3f  ", 100*$posCnts{"$k$i"}/$lines;
	}
	my $q = $tots[$i] * 1.0 / $lines;
	my $rq = int($q + 0.5) - 64;
    printf " %4d\n", $rq;
}
