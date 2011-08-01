#!/usr/bin/perl -w
use strict;
use Statistics::Descriptive;
use Getopt::Long;

## Paths to binaries
my $FASTX_RC     = "fastx_reverse_complement";
my $FASTX_TRIM   = "fastx_trimmer";
my $FASTQ_RENAME = "fastq_rename";
my $BWA          = "bwa";
my $SAMTOOLS     = "samtools";

## Options
my $samfile;
my $help = 0;
my $numreads = 10000;
my $RC = 0;
my $QV_TRIM = 10;
my $HARD_TRIM = 0;
my $QV_ILLUMINA = 0;
my $ADD_SUFFIX = 0;

my $prefix;

my $USAGE = "aligncestat.pl <options> prefix ref.fa fq1 fq2 | -samfile <samfile>\n"; 

my $res = GetOptions("help"      => \$help,
                     "sam=s"     => \$samfile,
                     "reads=n"   => \$numreads,
                     "rc"        => \$RC,
                     "I"         => \$QV_ILLUMINA,
                     "suffix"    => \$ADD_SUFFIX,
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
  print "  -suffix         : Add /1 and /2 suffix to reads\n";
  print "  -reads <n>      : align first n reads (default: $numreads)\n";
  print "  -rc             : reverse complement the reads before alignment\n";
  print "  -qv <n>         : bwa quality soft quality trim (default: $QV_TRIM)\n";
  print "  -I              : quality values are Illumina format\n";
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
  my $ref = $ARGV[1];
  my $fq1 = $ARGV[2];
  my $fq2 = $ARGV[3];

  $samfile = "$prefix.sam.s";

  my $nl = $numreads * 4; ## 4 lines per reads in a fastq

  my $TRIM_CMD = "";
  $TRIM_CMD  = " | $FASTX_TRIM -t $HARD_TRIM" if ($HARD_TRIM > 0);

  if ($RC)
  {
    $TRIM_CMD .= " | $FASTX_RC -Q ";
    if ($QV_ILLUMINA) { $TRIM_CMD .= "64"; } else { $TRIM_CMD .= "33"; }
  }

  my $TC1 = $TRIM_CMD;
  my $TC2 = $TRIM_CMD;

  if ($ADD_SUFFIX)
  {
    $TC1 .= " | $FASTQ_RENAME -suffix /1";
    $TC2 .= " | $FASTQ_RENAME -suffix /2";
  }

  $QV_ILLUMINA = ($QV_ILLUMINA) ? "-I" : "";

  runCmd("prepare fq1",   "$prefix.1.fq",  "head -$nl $fq1 $TC1 > $prefix.1.fq", 1);
  runCmd("prepare fq2",   "$prefix.2.fq",  "head -$nl $fq2 $TC2 > $prefix.2.fq", 1);

  runCmd("bwa index",  "$ref.sa", "$BWA index $ref");

  runCmd("bwa aln 1",  "$prefix.1.sai",   "$BWA aln $QV_ILLUMINA -q $QV_TRIM $ref $prefix.1.fq > $prefix.1.sai", 1);
  runCmd("bwa aln 2",  "$prefix.2.sai",   "$BWA aln $QV_ILLUMINA -q $QV_TRIM $ref $prefix.2.fq > $prefix.2.sai", 1);
  runCmd("bwa sampe",  "$prefix.sam",     "$BWA sampe -f $prefix.sam $ref $prefix.1.sai $prefix.2.sai $prefix.1.fq $prefix.2.fq", 1);

  runCmd("sam -> bam", "$prefix.bam",     "$SAMTOOLS view -bT $ref $prefix.sam > $prefix.bam", 1);
  runCmd("sort bam",   "$prefix.s.bam",   "$SAMTOOLS sort $prefix.bam $prefix.s", 1);  

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
  my $force = $_[3];

  if (! -r $outf || defined $force)
  {
    print STDERR "$desc [$cmd]...\n";
    my $rc = system($cmd);
    die $rc if $rc;
  }
}
