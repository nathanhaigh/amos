#!/usr/bin/perl -w
use strict;
use Getopt::Long;

## Paths to binaries
my $BWA  = "bwa";
my $SAMTOOLS = "samtools";

## Options
my $help  = 0;
my $prefix = "out";

my $make_bam     = 0;
my $make_sbam    = 0;
my $use_illumina = 0;
my $QV_TRIM = 10;
my $threads = 1;

my $USAGE = "bwape <options> ref.fa fq1 fq2\n"; 

my $res = GetOptions("help"      => \$help,
                     "prefix=s"  => \$prefix,
                     "bam"       => \$make_bam,
                     "sbam"      => \$make_sbam,
                     "I"         => \$use_illumina,
                     "qv=s"      => \$QV_TRIM,
                     "t=s"       => \$threads,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print "  Align paired-end reads to a genome using bwa. If the genome has not been\n";
  print "  indexed, it will first index the genome\n";
  print "\n";
  print "Required\n";
  print "  ref.fa          : path to reference genome (must be indexed with bwa)\n";
  print "  fq1             : path to first read\n";
  print "  fq2             : path to second read\n";
  print "\n";
  print "Options\n";
  print "  -prefix <name> : prefix for sam/bam file (default: $prefix)\n";
  print "  -bam           : convert results to bam\n";
  print "  -sbam          : make sorted bam\n";
  print "  -I             : reads use Illumina Fastq format (Q64)\n";
  print "  -qv <val>      : bwa quality value trim level (defailt: $QV_TRIM)\n";
  print "  -t <threads>   : threads for bwa aln (default $threads)\n";
  exit 0;
}

die $USAGE if (scalar @ARGV != 3);

my $ref = $ARGV[0];
my $fq1 = $ARGV[1];
my $fq2 = $ARGV[2];

my $samfile = "$prefix.sam";

my $QV_ILLUMINA = ($use_illumina) ? "-I" : "";

runCmd("bwa index",  "$ref.sa",       "$BWA index $ref");
runCmd("bwa aln 1",  "$prefix.1.sai", "$BWA aln $QV_ILLUMINA -t $threads -q $QV_TRIM $ref $fq1 > $prefix.1.sai", 1);
runCmd("bwa aln 2",  "$prefix.2.sai", "$BWA aln $QV_ILLUMINA -t $threads -q $QV_TRIM $ref $fq2 > $prefix.2.sai", 1);
runCmd("bwa sampe",  "$prefix.sam",   "$BWA sampe -f $prefix.sam $ref $prefix.1.sai $prefix.2.sai $fq1 $fq2", 1);

if ($make_bam || $make_sbam)
{
  runCmd("sam2bam",   "$prefix.bam",     "$SAMTOOLS view -Sb $prefix.sam -o $prefix.bam", 1);

  if ($make_sbam)
  {
    runCmd("sam sort",  "$prefix.s.bam",     "$SAMTOOLS sort $prefix.bam $prefix.s", 1);
    runCmd("sam index", "$prefix.s.bam.bai", "$SAMTOOLS index $prefix.s.bam", 1);
  }
  else
  {
    runCmd("sam index", "$prefix.bam.bai", "$SAMTOOLS index $prefix.bam", 1);
  }
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
