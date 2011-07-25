#!/usr/bin/perl -w
use strict;
use Statistics::Descriptive;
use Getopt::Long;

## Paths to binaries
my $FASTX_RC   = "fastx_reverse_complement";
my $FASTX_TRIM = "fastx_trimmer";
my $BWA        = "bwa";

## Options
my $samfile;
my $showdetails  = 0;
my $showdist = 0;
my $help  = 0;
my $numreads = 10000;
my $rc = 0;
my $trimmean = ".1";  ## trim 10% outliers
my $QV_TRIM = 10;
my $HARD_TRIM = 0;

my $prefix;

my $USAGE = "pairDist.pl <options> prefix ref.fa fq1 fq2 | -samfile <samfile>\n"; 

my $res = GetOptions("help"      => \$help,
                     "sam=s"     => \$samfile,
                     "reads=n"   => \$numreads,
                     "dist=n"    => \$showdist,
                     "details=n" => \$showdetails,
                     "rc"        => \$rc,
                     "qv=n"      => \$QV_TRIM,
                     "trim=n"    => \$HARD_TRIM,
                     );
 
if ($help)
{
  print $USAGE;
  print "\n";
  print "  Compute the distance between paired end reads by individually aligning them\n";
  print "  to a reference genome or draft assembly. Aligns just the first <numreads>\n";
  print "  and compute summary statistics on the unambiguous alignments to the same\n";
  print "  contig/scaffold/chromosome. Use -samfile if reads are already aligned.\n";
  print "\n";
  print "Options\n";
  print "  prefix          : prefix for output files\n";
  print "  ref.fa          : path to reference genome (must be indexed with bwa)\n";
  print "  fq1             : path to first read\n";
  print "  fq2             : path to second read\n";
  print "  -reads <n>      : align first n reads (default: $numreads)\n";
  print "  -dist <n>       : show the distance for the first n pairs (default: $showdist)\n";
  print "  -details <n>    : show details for first n pairs (default: $showdetails)\n";
  print "  -rc             : reverse complement the reads before alignment\n";
  print "  -qv <n>         : bwa quality soft quality trim (default: $QV_TRIM)\n";
  print "  -trim <n>       : hard trim 3' value (default: $HARD_TRIM)\n";
  print "\n";
  print "  -samfile <file> : Analyze previously aligned reads\n";

  exit 0;
}

if (($showdetails > 0) && ($showdist > 0))
{
  die "ERROR: Can't show details and distances at the same time\n";
}

## Align Reads
###############################################################################

if (!defined $samfile)
{
  die $USAGE if (scalar @ARGV != 4);

  $prefix = $ARGV[0];
  my $ref    = $ARGV[1];
  my $fq1    = $ARGV[2];
  my $fq2    = $ARGV[3];

  $samfile = "$prefix.sam.s";

  my $nl = $numreads * 4; ## 4 lines per reads in a fastq

  my $TRIM_CMD = "";

  if ($HARD_TRIM > 0)
  {
    $TRIM_CMD = " | $FASTX_TRIM -t $HARD_TRIM";
  }

  if ($rc)
  {
    runCmd("prepare fq1",   "$prefix.1.fq",  "head -$nl $fq1 $TRIM_CMD | $FASTX_RC | tr '/' '_' > $prefix.1.fq");
    runCmd("prepare fq2",   "$prefix.2.fq",  "head -$nl $fq2 $TRIM_CMD | $FASTX_RC | tr '/' '_' > $prefix.2.fq");
  }
  else
  {
    runCmd("prepare fq1",   "$prefix.1.fq",  "head -$nl $fq1 $TRIM_CMD | tr '/' '_' > $prefix.1.fq");
    runCmd("prepare fq2",   "$prefix.2.fq",  "head -$nl $fq2 $TRIM_CMD | tr '/' '_' > $prefix.2.fq");
  }

  runCmd("cat fq",     "$prefix.fq",    "cat $prefix.1.fq $prefix.2.fq > $prefix.fq");
  runCmd("bwa aln",    "$prefix.sai",   "$BWA aln -I -q $QV_TRIM $ref $prefix.fq > $prefix.sai");
  runCmd("bwa samse",  "$prefix.sam",   "$BWA samse -f $prefix.sam $ref $prefix.sai $prefix.fq");
  runCmd("sort sam",   "$samfile",      "grep -v '^\@' $prefix.sam | sort > $samfile");

  print STDERR "SAM file is: $samfile\n";
}
else
{
  $prefix = $samfile;
}


## Load Alignments
###############################################################################

open SAM, $samfile or die "Can't open $samfile ($!)\n";

my %match;

while (<SAM>)
{
  chomp;

  my ($name, $flag, $rname, $pos, $mapq, $cigar, $rnext, $pnext, $tlen, $seq, $qual) = split /\s+/, $_;

  my $base = substr($name, 0, -2); ## trim _1 or _2
  my $idx  = substr($name, -1, 1);

  my $info;
  $info->{idx}  = $idx;
  $info->{rid}  = $rname;
  $info->{p}    = $pos;
  $info->{l}    = length($seq);
  $info->{f}    = $flag;
  $info->{full} = $_;

  push @{$match{$base}}, $info;

  #print "$base\t$idx\t$rid\t$s\t$e\t$f\t||\t$_";
}


## Compute Distances
###############################################################################

my $stats = Statistics::Descriptive::Full->new();

if ($showdetails > 0)
{
  print "#base\t|\trid\t|\ts1\te1\tf1\t|\ts2\te2\tf2\t||\td\t||\tfull1\t||\tfull2\n";
}

foreach my $base (keys %match)
{
  if (scalar @{$match{$base}} == 2)
  {
    my $i1 = $match{$base}->[0];
    my $i2 = $match{$base}->[1];
    my $rid = $i1->{rid};

    if (($i1->{idx} ne $i2->{idx}) &&
        ($i1->{rid} eq $i2->{rid}) &&
        ($i1->{rid} ne '*'))
    {
      my $s1 = $i1->{p}; my $e1 = $s1 + $i1->{l}; my $f1 = $i1->{f}; my $full1 = $i1->{full}; 
      my $s2 = $i2->{p}; my $e2 = $s2 + $i2->{l}; my $f2 = $i2->{f}; my $full2 = $i2->{full};

      my $d = undef;

      if    (($f1 == 0) && ($f2 == 16)) { $d = $e2 - $s1; }
      elsif (($f1 == 16) && ($f2 == 0)) { $d = $e1 - $s2; }

      if (defined $d)
      {
        if ($showdetails != 0)
        {
          print "$base\t|\t$rid\t|\t$s1\t$e1\t$f1\t|\t$s2\t$e2\t$f2\t||\t$d\t||\t$full1\t||\t$full2\n";
          $showdetails--;
        }
        elsif ($showdist != 0)
        {
          print "$d\n";
          $showdist--;
        }

        $stats->add_data($d);
      }
    }
  }
}


## Print Stats
###############################################################################

my $num    = $stats->count();
my $min    = $stats->min();
my $max    = $stats->max();
my $mean   = sprintf("%0.01f", $stats->mean());
my $stdev  = sprintf("%0.01f", $stats->standard_deviation());
my $median=$stats->median();
my $trim = sprintf("%0.01f", $stats->trimmed_mean($trimmean));

print STDERR "## $prefix aligned pairs=$num dist: [$min, $max] $mean +/- $stdev";
print STDERR " median=$median";
print STDERR " trimmean=$trim";
print STDERR "\n";


## runCmd
###############################################################################

sub runCmd
{
  my $desc = $_[0];
  my $outf = $_[1];
  my $cmd  = $_[2];

  if (! -r $outf)
  {
    print STDERR "$desc [$cmd]...\n";
    my $rc = system($cmd);
    die $rc if $rc;
  }
}
