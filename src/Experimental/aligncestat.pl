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
my $help = 0;
my $numreads = 10000;
my $rc = 0;
my $QV_TRIM = 10;
my $HARD_TRIM = 0;

my $prefix;

my $USAGE = "aligncestat.pl <options> prefix ref.fa fq1 fq2 | -samfile <samfile>\n"; 

my $res = GetOptions("help"      => \$help,
                     "sam=s"     => \$samfile,
                     "reads=n"   => \$numreads,
                     "rc"        => \$rc,
                     "qv=n"      => \$QV_TRIM,
                     "trim=n"    => \$HARD_TRIM,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "  Align reads to a draft assembly and compute the ce statistic\n";
  print "\n";
  print "Required\n";
  print "  prefix          : prefix for output files\n";
  print "  ref.fa          : path to reference genome (must be indexed with bwa)\n";
  print "  fq1             : path to first read\n";
  print "  fq2             : path to second read\n";
  print "\n";
  print "Options\n";
  print "  -reads <n>      : align first n reads (default: $numreads)\n";
  print "  -rc             : reverse complement the reads before alignment\n";
  print "  -qv <n>         : bwa quality soft quality trim (default: $QV_TRIM)\n";
  print "  -trim <n>       : hard trim 3' value (default: $HARD_TRIM)\n";
  print "  -samfile <file> : Analyze previously aligned reads\n";

  exit 0;
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
    runCmd("prepare fq1",   "$prefix.1.fq",  "head -$nl $fq1 $TRIM_CMD | $FASTX_RC > $prefix.1.fq");
    runCmd("prepare fq2",   "$prefix.2.fq",  "head -$nl $fq2 $TRIM_CMD | $FASTX_RC > $prefix.2.fq");
  }
  else
  {
    runCmd("prepare fq1",   "$prefix.1.fq",  "head -$nl $fq1 $TRIM_CMD > $prefix.1.fq");
    runCmd("prepare fq2",   "$prefix.2.fq",  "head -$nl $fq2 $TRIM_CMD > $prefix.2.fq");
  }

 
  runCmd("bwa index",  "$ref.sa", "$BWA index $ref");
  runCmd("bwa aln 1",  "$prefix.1.sai",   "$BWA aln -I -q $QV_TRIM $ref $prefix.1.fq > $prefix.1.sai");
  runCmd("bwa aln 2",  "$prefix.2.sai",   "$BWA aln -I -q $QV_TRIM $ref $prefix.2.fq > $prefix.2.sai");
  runCmd("bwa sampe",  "$prefix.sam",   "$BWA sampe -f $prefix.sam $ref $prefix.1.sai $prefix.1.fq $prefix.2.sai $prefix.2.fq");

  #runCmd("sort sam",   "$samfile",      "grep -v '^\@' $prefix.sam | sort > $samfile");

  print STDERR "SAM file is: $samfile\n";
}
else
{
  $prefix = $samfile;
}


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
