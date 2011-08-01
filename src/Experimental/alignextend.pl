#!/usr/bin/perl -w
use strict;
use Getopt::Long;

$|=1;
$ENV{LC_ALL}="C";

## Paths to binaries
my $BWA            = "bwa";
my $FASTX_RC       = "fastx_reverse_complement";
my $FASTA_FROM_BED = "fastaFromBed";
my $FASTQ_RENAME   = "fastq_rename";
my $FASTX_TRIM     = "fastx_trimmer";

## Options
my $RC = 0;
my $READLEN = 100;
my $MAPQ_THRESHOLD = 20;
my $numreads = 1000000;

my $HARD_TRIM = 0;
my $QV_TRIM = 10;
my $QV_ILLUMINA;
my $ADD_SUFFIX = 0;

my $USAGE = "alignextend [options] prefix ref.fa fq1 fq2\n";
my $help;

my $res = GetOptions("help"      => \$help,
                     "reads=n"   => \$numreads,
                     "suffix"    => \$ADD_SUFFIX,
                     "rc"        => \$RC,
                     "qv=n"      => \$QV_TRIM,
                     "trim=n"    => \$HARD_TRIM,
                     "len=n"     => \$READLEN,
                     "mapq=n"    => \$MAPQ_THRESHOLD,
                     "I"         => \$QV_ILLUMINA);
 
if ($help)
{
  print $USAGE;
  print "\n";
  print "Align (very) short reads to a draft assembly, and extend reads that map unambiguously\n";
  print "with the sequence of the contig on the 3' end\n";
  print "\n";
  print "Required\n";
  print "  prefix     : prefix for output files\n";
  print "  ref.fa     : path to reference genome (must be indexed with bwa)\n";
  print "  fq1        : path to first read\n";
  print "  fq2        : path to second read\n";
  print "\n";
  print "Options\n";
  print "  -reads <n> : align first m reads (default: $numreads)\n";
  print "  -suffix    : Add /1 and /2 suffix to reads\n";
  print "  -rc        : reverse complement the reads before alignment\n";
  print "  -qv <n>    : bwa quality soft quality trim (default: $QV_TRIM)\n";
  print "  -trim <n>  : hard trim 3' value (default: $HARD_TRIM)\n";
  print "  -I         : read quality values are Illumina format\n";
  print "  -len <n>   : extend to this length (default: $READLEN)\n";
  print "  -mapq <n>  : Only trust alignment with at least this MAPQ (default: $MAPQ_THRESHOLD)\n";
  exit 0;
 }


my $prefix = $ARGV[0] or die $USAGE;
my $ref    = $ARGV[1] or die $USAGE;
my $fq1    = $ARGV[2] or die $USAGE;
my $fq2    = $ARGV[3] or die $USAGE;


$QV_ILLUMINA = (defined $QV_ILLUMINA) ? "-I" : "";



## runCmd
###############################################################################

sub runCmd
{
  my $desc = $_[0];
  my $outf = $_[1];
  my $cmd  = $_[2];

  #if (! -r $outf)
  {
    print "$desc: $cmd...\n";
    my $rc = system($cmd);
    die $rc if $rc;
  }
}


## sam2bed
###############################################################################

sub sam2bed
{
  my $samfile = shift;
  my $bedfile = shift;

  return if -r $bedfile;

  open SAM, "< $samfile" or die "Can't open $samfile ($!)\n";
  open BED, "> $bedfile" or die "Can't open $bedfile ($!)\n";

  my %scafflen;

  print  "Running sam2bed $samfile...\n";
  my $rdcnt = 0;
  my $goodrdcnt = 0;
  my $scaffcnt = 0;

  while (<SAM>)
  {
    chomp;

    if (/^\@SQ/)
    {
      my ($tag, $sn, $ln) = split /\s+/, $_;
      my ($snt,$scaffid) = split /:/, $sn;
      my ($lnt,$scafflen) = split /:/, $ln;

      $scafflen{$scaffid} = $scafflen;
      $scaffcnt++;

      next;
    }
    elsif (/^\@/)
    {
      ## other headers, pg, etc
      next;
    }

    $rdcnt++;

    my ($name, $flag, $rname, $pos, $mapq, $cigar, $rnext, $pnext, $tlen, $seq, $qual, $attr) = split /\s+/, $_;

    my $base = substr($name, 0, -2); ## trim _1 or _2
    my $iidx  = substr($name, -1, 1);

    next if $rname eq '*';
    next if $mapq < $MAPQ_THRESHOLD;

    my $scafflen = $scafflen{$rname};

    if ($flag == 0)
    {
      my $startpos = $pos-1;  ## BED is 0-based
      my $endpos = $startpos + $READLEN - 1;

      if ($endpos >= $scafflen) { $endpos = $scafflen-1;}

      print BED "$rname\t$pos\t$endpos\t$base/$iidx\t1\t+\n"; ## Forward Strand
      $goodrdcnt++;
    }
    elsif ($flag == 16)
    {
      my $endpos = $pos+length($seq)-1-1; ## BED is 0-based
      my $startpos = $endpos - $READLEN+1;

      if ($startpos <= 0) { $startpos = 0; }

      print BED "$rname\t$startpos\t$endpos\t$base/$iidx\t1\t-\n"; ## Reverse Strand
      $goodrdcnt++;
    }
  }

  my $percgood = sprintf("%0.02f", 100*$goodrdcnt / $rdcnt);

  print "Loaded: $scaffcnt scaffolds\n";
  print "Printed $goodrdcnt of $rdcnt reads ($percgood%)\n";
}


## matchSFA
###############################################################################

sub matchSFA
{
  my $sfa1 = $_[0];
  my $sfa2 = $_[1];
  my $fq1  = $_[2];
  my $fq2  = $_[3];

  open SFA1, "< $sfa1" or die "Can't open $sfa1 ($!)\n";
  open SFA2, "< $sfa2" or die "Can't open $sfa2 ($!)\n";

  open FQ1, "> $fq1" or die "Can't open $fq1 ($!)\n";
  open FQ2, "> $fq2" or die "Can't open $fq2 ($!)\n";

  my %sfa1;
  my %sfa2;

  print "matchSFA\n";
  print " scanning $sfa1... ";

  my $cnt = 0;

  while (<SFA1>)
  {
    chomp;
    my ($name, $seq) = split /\t/, $_;

    my $base = substr($name, 0, -2); ## trim /1 or /2
    my $iidx = substr($name, -1, 1);

    $sfa1{$base}++;
    $cnt++;
  }

  close SFA1;

  print "$cnt reads\n";

  $cnt = 0;
  my $printed = 0;

  print " printing $fq2... ";

  while (<SFA2>)
  {
    chomp;
    my ($name, $seq) = split /\t/, $_;

    my $base = substr($name, 0, -2); ## trim /1 or /2
    my $iidx  = substr($name, -1, 1);

    $cnt++;

    if (exists $sfa1{$base})
    {
      $printed++;
      $sfa2{$base}++;

      my $q;

      if ($QV_ILLUMINA) { $q = 'a' x length($seq); }
      else              { $q = 'A' x length($seq); }

      print FQ2 "\@$name\n$seq\n+\n$q\n";
    }
  }

  print "printed $printed of $cnt\n";

  open SFA1, "< $sfa1" or die "Can't open $sfa1 ($!)\n";

  $cnt = 0;
  $printed = 0;

  print " printing $fq1... ";
  
  while (<SFA1>)
  {
    chomp;
    my ($name, $seq) = split /\t/, $_;

    my $base = substr($name, 0, -2); ## trim /1 or /2
    my $iidx  = substr($name, -1, 1);

    $cnt++;

    if (exists $sfa2{$base})
    {
      $printed++;

      my $q;

      if ($QV_ILLUMINA) { $q = 'a' x length($seq); }
      else              { $q = 'A' x length($seq); }

      print FQ1 "\@$name\n$seq\n+\n$q\n";
    }
  }

  print "printed $printed of $cnt\n";
}


## Align the reads
###############################################################################

foreach my $idx (1..2)
{
  print "idx: $idx\n";

  my $fq = ($idx == 1) ? $fq1 : $fq2;
  my $samfile = "$prefix.$idx.sam";
  my $bedfile = "$prefix.$idx.bed";

  my $nl = $numreads * 4; ## 4 lines per reads in a fastq

  my $TRIM_CMD = "";
  $TRIM_CMD  = " | $FASTX_TRIM -t $HARD_TRIM" if ($HARD_TRIM > 0);

  if ($RC)
  {
    $TRIM_CMD .= " | $FASTX_RC -Q ";
    if ($QV_ILLUMINA) { $TRIM_CMD .= "64"; } else { $TRIM_CMD .= "33"; }
  }

  if ($ADD_SUFFIX) { $TRIM_CMD .= " | $FASTQ_RENAME -suffix _$idx"; }
  else             { $TRIM_CMD .= " | $FASTQ_RENAME -tr '/'"; }

  $QV_ILLUMINA = ($QV_ILLUMINA) ? "-I" : "";

  runCmd("prepare fq",   "$prefix.$idx.fq",  "head -$nl $fq $TRIM_CMD > $prefix.$idx.fq");

  runCmd("bwa aln",    "$prefix.$idx.sai",   "$BWA aln $QV_ILLUMINA -q $QV_TRIM $ref $prefix.$idx.fq > $prefix.$idx.sai");
  runCmd("bwa samse",  "$prefix.$idx.sam",   "$BWA samse -f $samfile $ref $prefix.$idx.sai $prefix.$idx.fq");

  sam2bed($samfile, $bedfile);

  runCmd("fastaFromBed", "$prefix.$idx.sfa",  "$FASTA_FROM_BED -s -name -tab -fi $ref -bed $bedfile -fo $prefix.$idx.sfa");
  runCmd("sort sfa",     "$prefix.$idx.sfa.s", "sort -k1,1 $prefix.$idx.sfa > $prefix.$idx.sfa.s");

  print "\n";
}


## Create the matched extended pairs
###############################################################################

matchSFA("$prefix.1.sfa.s", "$prefix.2.sfa.s", "$prefix.1e.fq", "$prefix.2e.fq");

