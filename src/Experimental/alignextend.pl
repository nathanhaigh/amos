#!/usr/bin/perl -w
use strict;
use Getopt::Long;
$| = 1;

## Paths to binaries
my $BWA            = "bwa"; # bwa
my $FASTX_RC       = "fastx_reverse_complement"; # fastx_toolkit
my $FASTA_FROM_BED = "fastaFromBed"; # bedtools
my $FASTQ_RENAME   = "fastq_rename"; # AMOS (src/utils/fastq_rename.pl)
my $FASTX_TRIM     = "fastx_trimmer"; # fastx_toolkit

## Options

my $help = 0;
my $stage = "prepare";

## prepare
my $ADD_SUFFIX = 0;
my $RC         = 0;
my $HARD_TRIM  = 0;
my $NUM_READS  = 0; # 0 means all reads
my $PAIR       = 0;

## align
my $THREADS     = 1;
my $QV_TRIM     = 10;
my $QV_ILLUMINA = 0;
my $NOINDEX     = 0;
my $MAXDIFF     = 0.04;

## filter
my $CHECK_DIST     = 0;
my $MIN_DIST       = 0;
my $MAX_DIST       = 0;
my $MAPQ_THRESHOLD = 0;
my $ALLOW_DIFF     = 0;
my $INVALID        = undef;
my $UNALIGNED      = 0;

## rmdup
my $SKIPRMDUP  = 0;
my $DUP_WIGGLE = 2;

## genreads
my $SKIPGEN = 0;
my $READLEN = 100;
my $QV_READ = 31;

my $USAGE = "alignextend [options] prefix ref.fa fq1 fq2\n";

my $arguments = join(" ", @ARGV);

my $res = GetOptions("help"      => \$help,
                     "stage=s"   => \$stage,

                     # prepare
                     "suffix"    => \$ADD_SUFFIX,
                     "rc"        => \$RC,
                     "trim=n"    => \$HARD_TRIM,
                     "reads=n"   => \$NUM_READS,
                     "pair"      => \$PAIR,

                     # align
                     "qv=n"      => \$QV_TRIM,
                     "threads=n" => \$THREADS,
                     "I"         => \$QV_ILLUMINA,
                     "noindex"   => \$NOINDEX,
                     "maxdiff=f" => \$MAXDIFF,

                     # filter
                     "mapq=n"    => \$MAPQ_THRESHOLD,
                     "check"     => \$CHECK_DIST,
                     "min=n"     => \$MIN_DIST,
                     "max=n"     => \$MAX_DIST,
                     "allow"     => \$ALLOW_DIFF,
                     "invalid=s" => \$INVALID,
                     "unaligned" => \$UNALIGNED,

                     # rmdup
                     "skiprmdup" => \$SKIPRMDUP,
                     "dup=n"     => \$DUP_WIGGLE,

                     # genreads
                     "skipgen"   => \$SKIPGEN,
                     "len=n"     => \$READLEN,
                     "q=n"       => \$QV_READ
                    );

if (!$res)
{
  die $USAGE;
}
 
if ($help)
{
  print $USAGE;
  print "\n";
  print "Align (very) short reads to a draft assembly, and extend reads on their 3' end with the sequence\n";
  print "to where they align. This is especially for extending short mate-pair reads into long reads for\n";
  print "scaffolding.\n";
  print "\n";
  print "Required\n";
  print "  prefix       : (out) prefix for output files\n";
  print "  ref.fa       : (in)  path to FASTA file for reference genome\n";
  print "  fq1          : (in)  path to FASTQ file for read 1 in pair\n";
  print "  fq2          : (in)  path to FASTQ file for read 2 in pair\n";
  print "\n";
  print "Options\n";
  print " -stage <s>    : stage to start at [prepare,align,filter,rmdup,genreads] (default: $stage)\n";
  print "\n";
  print "prepare: prepare the data for alignment\n";
  print "  -suffix      : Add /1 and /2 suffix to reads\n";
  print "  -rc          : reverse complement the reads\n";
  print "  -trim <n>    : hard trim 3' value (default: $HARD_TRIM)\n";
  print "  -reads <n>   : only use first n pairs (default: use all pairs)\n";
  print "  -pair        : pair the reads correctly (this normally should be done already)\n";
  print "\n";
  print "align: align reads to assembly\n";
  print "  -qv <n>      : bwa quality soft quality trim (default: $QV_TRIM)\n";
  print "  -threads <n> : number of threads for bwa (default: $THREADS)\n";
  print "  -I           : read quality values are Illumina format\n";
  print "  -maxdiff     : -n option to bwa (max differences per alignment)\n";
  print "  -noindex     : assembly is already indexed with bwa\n";
  print "\n";
  print "filter: select pairs that align and are correctly separated\n";
  print "  -mapq <n>    : Only trust alignment with at least this MAPQ (default: $MAPQ_THRESHOLD)\n";
  print "  -check       : Only accept alignments with the specified orientation and distance\n";
  print "  -min <n>     : Only accept alignments at least this far apart (default: $MIN_DIST)\n";
  print "  -max <n>     : Only accept alignments less than this far apart (default: $MAX_DIST)\n";
  print "  -allow       : Allow alignments to separate sequences (only useful with -check)\n";
  print "  -invalid <f> : Save aligned but invalid pairs to a BED file f (only useful with -check)\n";
  print "  -unaligned   : Save unaligned pairs to \${prefix}.unaligned.{1,2}.fq\n";
  print "\n";
  print "rmdup: remove duplicates\n";
  print "  -skiprmdup:  : Skip the rmdup stage (do not remote duplicates)\n";
  print "  -dup <n>     : Filter duplicates with coordinates within this distance (default: $DUP_WIGGLE)\n";
  print "\n";
  print "genreads: extract new reads\n";
  print "  -skipgen     : Skip the genreads stage (do not extract new reads)\n";
  print "  -len <n>     : extend the reads to this length (default: $READLEN)\n";
  print "  -q <n>       : quality value to assign to read bases (default: $QV_READ)\n";

  exit 0;
}

if ($MAX_DIST != 0 || $MIN_DIST != 0)
{
  $CHECK_DIST = 1;
}

if (($CHECK_DIST) && ($MAX_DIST == 0))
{
  die "ERROR: checking distance, but max=0\n";
}

my $prefix = $ARGV[0] or die $USAGE;
my $ref    = $ARGV[1] or die $USAGE;

die "ERROR: Can't find \"$ref\"\n" unless -r $ref;

if ("$ref" ne "$prefix.ref.fa")
{
  unlink "$prefix.ref.fa" if (-e "$prefix.ref.fa");
  symlink $ref, "$prefix.ref.fa"
      or die "Failed to create symlink \"$prefix.ref.fa\" => \"$ref\"\n";
}
$ref = "$prefix.ref.fa";

print "Running $0 $arguments\n";

my $start_time = time;
my $stage_start_time;

if    ($stage eq "prepare")  { goto PREPARE_STAGE;  }
elsif ($stage eq "align")    { goto ALIGN_STAGE;    }
elsif ($stage eq "filter")   { goto FILTER_STAGE;   }
elsif ($stage eq "rmdup")    { goto RMDUP_STAGE;    }
elsif ($stage eq "genreads") { goto GENREADS_STAGE; }
else                         { die "Unknown stage: $stage\n"; }

sub begin_stage
{
  my $stage = shift;
  my $hyphens = "-" x length($stage);
  $stage_start_time = time;
  print "\n";
  print "------------------------------$hyphens-\n";
  print "alignextend: beginning stage \"$stage\"\n";
  print "------------------------------$hyphens-\n";
}
sub end_stage
{
  my $stage = shift;
  printf "alignextend: ending stage \"$stage\" (%d seconds elapsed)\n",
          time - $stage_start_time;
}

## Prepare the reads
###############################################################################
PREPARE_STAGE:
begin_stage "prepare";
my $fq1 = $ARGV[2] or die $USAGE;
my $fq2 = $ARGV[3] or die $USAGE;
die "ERROR: Can't find \"$fq1\"\n" unless -r $fq1;
die "ERROR: Can't find \"$fq2\"\n" unless -r $fq2;
die "The -pair option is unimplemented\n" if ($PAIR);

foreach my $idx (1..2)
{
  my $fq = ($idx == 1) ? $fq1 : $fq2;

  print "Preparing \"$prefix.$idx.fq\" from \"$fq\"\n";

  my $PREPARE_CMD = "";

  if ($NUM_READS == 0)
  {
    $PREPARE_CMD .= "cat \"$fq\"";
  }
  else
  {
    my $nl = $NUM_READS * 4; ## 4 lines per reads in a fastq
    $PREPARE_CMD .= "head -$nl \"$fq\"";
  }

  if ($HARD_TRIM > 0)
  {
    $PREPARE_CMD .= " | $FASTX_TRIM -t $HARD_TRIM";
  }

  if ($RC)
  {
    my $q_offset = ($QV_ILLUMINA) ? "64" : "33";
    $PREPARE_CMD .= " | $FASTX_RC -Q $q_offset";
  }

  if ($ADD_SUFFIX) { $PREPARE_CMD .= " | $FASTQ_RENAME -clean -suffix _$idx"; }
  else             { $PREPARE_CMD .= " | $FASTQ_RENAME -clean -tr '/'"; }

  $PREPARE_CMD .= "> \"$prefix.$idx.fq\"";

  runCmd("prepare fq", "$prefix.$idx.fq", $PREPARE_CMD);
}
end_stage "prepare";

## Align the reads
###############################################################################
ALIGN_STAGE:
begin_stage "align";

foreach my $idx (1..2)
{
  die "ERROR: Can't find \"$prefix.$idx.fq\"\n" unless -r "$prefix.$idx.fq";
}

## Index genome
if (! $NOINDEX)
{
  runCmd("bwa index", "$ref.sa", "$BWA index \"$ref\"");
  print "\n";
}

die "ERROR: Can't find \"$ref.sa\"\n" unless -r "$ref.sa";

my $bwa_qv_flag = ($QV_ILLUMINA) ? "-I" : "";
foreach my $idx (1..2)
{
  print "Aligning reads from \"$prefix.$idx.fq\" to \"$ref\"\n";
  runCmd("bwa aln", "$prefix.$idx.sai",
         "$BWA aln -t $THREADS $bwa_qv_flag -q $QV_TRIM -f \"$prefix.$idx.sai\" "
         . "-n $MAXDIFF \"$ref\" \"$prefix.$idx.fq\" >& \"$prefix.$idx.sai.log\"");
  runCmd("bwa samse", "$prefix.$idx.sam",
         "$BWA samse -f \"$prefix.$idx.sam\" \"$ref\" \"$prefix.$idx.sai\" \"$prefix.$idx.fq\" "
         . ">& \"$prefix.$idx.sam.log\"");
}
end_stage "align";

FILTER_STAGE:
begin_stage "filter";
filter_by_alignment("$prefix.1.sam","$prefix.2.sam", "$prefix.bedpe",
                    "$prefix.1.fq", "$prefix.2.fq",
                    ($UNALIGNED) ? "$prefix.unaligned.1.fq" : undef,
                    ($UNALIGNED) ? "$prefix.unaligned.2.fq" : undef);
end_stage "filter";

RMDUP_STAGE:
if ($SKIPRMDUP)
{
  print "Skipping stage: rmdup\n";
}
else
{
  begin_stage "rmdup";
  die "ERROR: Can't find \"$prefix.bedpe\"\n" unless -r "$prefix.bedpe";
  filterDups("$prefix.bedpe", "$prefix.rmdup.bedpe");
  end_stage "rmdup";
}

GENREADS_STAGE:
if ($SKIPGEN)
{
  print "Skipping stage: genreads\n";
}
else
{
  begin_stage "genreads";
  my $bedpe = ($SKIPRMDUP) ? "$prefix.bedpe" : "$prefix.rmdup.bedpe";
  die "ERROR: Can't find \"$bedpe\"\n" unless -r $bedpe;
  genReads($bedpe, "$prefix.1e.fq", "$prefix.2e.fq");
  end_stage "genreads";
}

printf "alignextend: Done (%d seconds elapsed)\n", time - $start_time;
exit 0;

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

## read_sam_headers
###############################################################################

sub read_sam_headers
{
  my $samfilep = shift;
  my $reflenp = shift;
  my $refcntp = shift;
  my $line;

  while (1)
  {
    $line = readline $samfilep;
    if (! defined $line || $line !~ /^\@/)
    {
      last;
    }
    elsif ($line =~ /^\@SQ/)
    {
      my ($tag, $sn, $ln) = split /\s+/, $line;
      my ($snt, $refid)   = split /:/, $sn;
      my ($lnt, $len)  = split /:/, $ln;

      if (! defined $reflenp->{$refid})
      {
        $reflenp->{$refid} = $len;
        $$refcntp++;
      }
      elsif ($reflenp->{$refid} ne $len)
      {
        die "Inconsistent headers in SAM files\n";
      }
    }
    ## Ignore other SAM headers such as PG
  }
  # Return first line in the SAM file that is not a header.
  return $line;
}

## get_read_pos
###############################################################################

sub get_read_pos
{
  my $sam_pos    = shift;
  my $goal_rdlen = shift;
  my $cur_rdlen  = shift;
  my $reflen     = shift;
  my $dir        = shift;

  my $startpos;
  my $endpos;
  if ($dir eq "+")
  {
    ## Forward strand
    $startpos = $sam_pos - 1;  ## BED is 0-based
    $endpos = $startpos + $goal_rdlen;
    if ($endpos > $reflen)
    { 
      $endpos   = $reflen;
      $startpos = $reflen - $goal_rdlen;
      if ($startpos < 0)
      {
        $startpos = 0;
      }
    }
  }
  else
  {
    ## Reverse strand
    $endpos = ($sam_pos - 1) + $cur_rdlen; ## BED is 0-based
    $startpos = $endpos - $goal_rdlen;
    if ($startpos < 0) 
    { 
      $startpos = 0; 
      $endpos = $goal_rdlen;
    }
    if ($endpos > $reflen)
    {
      $endpos = $reflen;
    }
  }
  return ($startpos, $endpos);
}

## filter_by_alignment
##############################################################################

sub filter_by_alignment
{
  my $samfile1       = shift;
  my $samfile2       = shift;
  my $bedpe          = shift;
  my $prepared_1_fq  = shift;
  my $prepared_2_fq  = shift;
  my $unaligned_1_fq = shift;
  my $unaligned_2_fq = shift;

  open SAM1, "<", $samfile1 or die "Can't open \"$samfile1\": $!\n";
  open SAM2, "<", $samfile2 or die "Can't open \"$samfile2\": $!\n";

  print "Saving filtered reads to \"$bedpe\"\n";
  open BEDPE, ">", $bedpe or die "Can't open \"$bedpe\": $!\n";

  if (defined $unaligned_1_fq)
  {
    print "Saving unaligned reads to \"$unaligned_1_fq\"\n";
    open UNALIGNED_1_FQ, ">", $unaligned_1_fq or die "Can't open \"$unaligned_1_fq\": $!\n";
    open PREPARED_1_FQ, "<", $prepared_1_fq or die "Can't open \"$prepared_1_fq\": $!\n";
  }
  if (defined $unaligned_2_fq)
  {
    print "Saving unaligned reads to \"$unaligned_2_fq\"\n";
    open UNALIGNED_2_FQ, ">", $unaligned_2_fq or die "Can't open \"$unaligned_2_fq\": $!\n";
    open PREPARED_2_FQ, "<", $prepared_2_fq or die "Can't open \"$prepared_2_fq\": $!\n";
  }

  if (defined $INVALID)
  {
    print "Saving invalidly aligned pairs to \"$INVALID\"\n";
    open INVALID, ">", $INVALID or die "Can't open \"$INVALID\": $!\n";
  }

  my $pair_cnt = 0;
  my $unaligned_pair_cnt = 0;
  my $partially_aligned_pair_cnt = 0;
  my $aligned_pair_cnt = 0;

  my $same_scaffold_pair_cnt = 0;
  my $diff_scaffold_pair_cnt = 0;

  my $same_direction_pair_cnt = 0;
  my $negative_sep_pair_cnt = 0;
  my $invalid_sep_pair_cnt = 0;
  my $valid_sep_pair_cnt = 0;

  my $printed_pair_cnt = 0;

  my $fq_tag_1;
  my $fq_seq_1;
  my $fq_sep_1;
  my $fq_qual_1;

  my $fq_tag_2;
  my $fq_seq_2;
  my $fq_sep_2;
  my $fq_qual_2;

  my %reflen;
  my $refcnt = 0;

  my $samline1 = read_sam_headers(\*SAM1, \%reflen, \$refcnt);
  my $samline2 = read_sam_headers(\*SAM2, \%reflen, \$refcnt);

  while (defined $samline1 and defined $samline2)
  {
    $pair_cnt++;

    if (defined $unaligned_1_fq)
    {
      $fq_tag_1  = readline PREPARED_1_FQ;
      $fq_seq_1  = readline PREPARED_1_FQ;
      $fq_sep_1  = readline PREPARED_1_FQ;
      $fq_qual_1 = readline PREPARED_1_FQ;
    }

    if (defined $unaligned_2_fq)
    {
      $fq_tag_2  = readline PREPARED_2_FQ;
      $fq_seq_2  = readline PREPARED_2_FQ;
      $fq_sep_2  = readline PREPARED_2_FQ;
      $fq_qual_2 = readline PREPARED_2_FQ;
    }

    my ($name1, $flag1, $rname1, $pos1, $mapq1, $cigar1, $rnext1, $pnext1,
        $tlen1, $seq1) = split /\t/, $samline1;

    my ($name2, $flag2, $rname2, $pos2, $mapq2, $cigar2, $rnext2, $pnext2,
        $tlen2, $seq2) = split /\t/, $samline2;

    my $base1 = substr($name1, 0, -2); ## trim _1 or _2
    my $base2 = substr($name2, 0, -2); ## trim _1 or _2
    if ($base1 ne $base2)
    {
    
      die "ERROR: Reads \"$name1\" and \"$name2\"\n" 
           . "in \"$samfile1\" and \"$samfile2\" are not from the same pair.\n";
    }

    my $iidx1 = substr($name1, -1, 1);
    my $iidx2 = substr($name2, -1, 1);
    if (!(($iidx1 eq "1" && $iidx2 eq "2") || ($iidx1 eq "2" && $iidx2 eq "1")))
    {
      my $msg =  "ERROR: Reads \"$name1\" and \"$name2\"\n"
                  . "in \"$samfile1\" and \"$samfile2\" are not reads 1 and 2 of a pair.\n";
      if (!$ADD_SUFFIX && ($name1 !~ /[12]$/ || $name2 !~ /[12]$/))
      {
        $msg .= "Perhaps you need to specify '-suffix'?\n";
      }
      die $msg;
    }

    my $is_1_unaligned = (($flag1 & 0x4) || ($mapq1 < $MAPQ_THRESHOLD));
    my $is_2_unaligned = (($flag2 & 0x4) || ($mapq2 < $MAPQ_THRESHOLD));

    if ($is_1_unaligned || $is_2_unaligned)
    {
      ## At least one read could not be aligned with adequate quality.  Write
      ## the reads to the FASTQ files for unaligned pairs if -unaligned was
      ## specified.  Both reads in the pair are written, even if one was
      ## aligned.

      if ($is_1_unaligned && $is_2_unaligned)
      {
        $unaligned_pair_cnt++;
      }
      else
      {
        $partially_aligned_pair_cnt++;
      }
      print UNALIGNED_1_FQ "$fq_tag_1$fq_seq_1$fq_sep_1$fq_qual_1"
         or die "Error writing to \"$unaligned_1_fq\": $!\n"
         if defined $unaligned_1_fq;
      print UNALIGNED_2_FQ "$fq_tag_2$fq_seq_2$fq_sep_2$fq_qual_2"
         or die "Error writing to \"$unaligned_2_fq\": $!\n"
         if defined $unaligned_2_fq;
    }
    else
    {
      ## Both reads were aligned.

      $aligned_pair_cnt++;

      ## Direction of each read.
      my $dir1 = ($flag1 & 0x10) ? "-" : "+";
      my $dir2 = ($flag2 & 0x10) ? "-" : "+";

      my ($startpos1, $endpos1) = get_read_pos($pos1, $READLEN, length($seq1),
                                               $reflen{$rname1}, $dir1);

      my ($startpos2, $endpos2) = get_read_pos($pos2, $READLEN, length($seq2),
                                               $reflen{$rname2}, $dir2);

      my $doprint = 1;

      if ($CHECK_DIST)
      {
        if ($rname1 eq $rname2)
        {
          ## Same scaffold 

          $same_scaffold_pair_cnt++;

          if ($dir1 eq $dir2)
          {
            ## Reads align in same direction!  Don't include these reads.

            $same_direction_pair_cnt++;
            $doprint = 0;
          }
          else
          {
            my $dist;
            if ($dir1 eq "+")
            {
              $dist = $endpos2 - $startpos1;
            }
            else
            {
              $dist = $endpos1 - $startpos2;
            }
            if ($dist < 0)
            {
              $negative_sep_pair_cnt++;
            }
            if (($dist < $MIN_DIST) || ($dist > $MAX_DIST))
            {
              $invalid_sep_pair_cnt++;
              $doprint = 0;
            } else {
              $valid_sep_pair_cnt++;
            }
          }
        }
        else
        {
          ## Different scaffolds

          $diff_scaffold_pair_cnt++;
          $doprint = $ALLOW_DIFF;
        }
      }
      if ($doprint) 
      { 
        $printed_pair_cnt++;
        print BEDPE "$base1\t$rname1\t$startpos1\t$endpos1\t$dir1"
                    . "\t$rname2\t$startpos2\t$endpos2\t$dir2\n"
                 or die "Error writing to \"$bedpe\": $!\n";
      }
      elsif (defined $INVALID)
      {
        print INVALID "$base1\t$rname1\t$startpos1\t$endpos1\t$dir1"
                      . "\t$rname2\t$startpos2\t$endpos2\t$dir2\n"
                 or die "Error writing to \"$INVALID\": $!\n";
      }
    }
    $samline1 = readline SAM1;
    $samline2 = readline SAM2;
  }

  die "Error reading $samfile1: $!\n" unless (eof SAM1 || defined $samline1);
  die "Error reading $samfile2: $!\n" unless (eof SAM2 || defined $samline2);
  die "$samfile1 and $samfile2 do not have the same number of lines"
    if (defined $samline1 || defined $samline2);

  close SAM1;
  close SAM2;
  close UNALIGNED_1_FQ or die "Failed to close \"$unaligned_1_fq\": $!\n"
     if defined $unaligned_1_fq;
  close UNALIGNED_2_FQ or die "Failed to close \"$unaligned_2_fq\": $!\n"
     if defined $unaligned_2_fq;
  close PREPARED_1_FQ if defined $unaligned_1_fq;
  close PREPARED_2_FQ if defined $unaligned_2_fq;
  close BEDPE or die "Failed to close \"$bedpe\": $!\n";
  close INVALID or die "Failed to close \"$INVALID\": $!\n" if defined $INVALID;

  printf "Number of pairs...\n";
  printf "  total: $pair_cnt\n";

  printf "  where neither read aligned to a scaffold: $unaligned_pair_cnt (%.2f%%)\n",
        100 * $unaligned_pair_cnt / $pair_cnt;
  printf "  where only one read aligned to a scaffold: $partially_aligned_pair_cnt (%.2f%%)\n",
        100 * $partially_aligned_pair_cnt / $pair_cnt;
  printf "  where both reads aligned to a scaffold: $aligned_pair_cnt (%.2f%%)\n",
        100 * $aligned_pair_cnt / $pair_cnt;
  if ($CHECK_DIST)
  {
    printf "  where the reads aligned...\n";
    printf "    to different scaffolds: $diff_scaffold_pair_cnt (%.2f%% of total, %.2f%% of aligned)\n",
          100 * $diff_scaffold_pair_cnt / $pair_cnt,
          100 * $diff_scaffold_pair_cnt / $aligned_pair_cnt;
    printf "    to the same scaffold: $same_scaffold_pair_cnt (%.2f%% of total, %.2f%% of aligned)\n",
          100 * $same_scaffold_pair_cnt / $pair_cnt,
          100 * $same_scaffold_pair_cnt / $aligned_pair_cnt;
    printf "  where the reads aligned to the same scaffold...\n";
    printf "    in the same direction: $same_direction_pair_cnt (%.2f%% of total, %.2f%% of same-scaffold aligned)\n",
          100 * $same_direction_pair_cnt / $pair_cnt,
          100 * $same_direction_pair_cnt / $same_scaffold_pair_cnt;
    printf "    with negative separation: $negative_sep_pair_cnt (%.2f%% of total, %.2f%% of same-scaffold aligned)\n",
          100 * $negative_sep_pair_cnt / $pair_cnt,
          100 * $negative_sep_pair_cnt / $same_scaffold_pair_cnt;
    printf "    with invalid separation: $invalid_sep_pair_cnt (%.2f%% of total, %.2f%% of same-scaffold aligned)\n",
          100 * $invalid_sep_pair_cnt / $pair_cnt,
          100 * $invalid_sep_pair_cnt / $same_scaffold_pair_cnt;
    printf "    with valid separation: $valid_sep_pair_cnt (%.2f%% of total, %.2f%% of same-scaffold aligned)\n",
          100 * $valid_sep_pair_cnt / $pair_cnt,
          100 * $valid_sep_pair_cnt / $same_scaffold_pair_cnt;
  }
  printf "$printed_pair_cnt read pairs (%.2f%% of total) were written to \"$bedpe\"\n",
         100 * $printed_pair_cnt / $pair_cnt;
  if ($INVALID)
  {
    my $invalid_cnt = $aligned_pair_cnt - $printed_pair_cnt;
    printf "$invalid_cnt read pairs (%.2f%% of total) were written to \"$INVALID\"\n",
           100 * $invalid_cnt / $pair_cnt;
  }
  if (defined $unaligned_1_fq && defined $unaligned_2_fq)
  {
    my $unaligned_cnt = $unaligned_pair_cnt + $partially_aligned_pair_cnt;
    printf "$unaligned_cnt read pairs (%.2f%% of total) were written "
           . "to \"$unaligned_1_fq\" and \"$unaligned_2_fq\"\n",
           100 * $unaligned_cnt / $pair_cnt;
  }
}

## filterDups
##############################################################################

sub filterDups
{
  my $bedpe = shift;
  my $bedpeout = shift;

  open BEDPE, "<", $bedpe or die "Can't open \"$bedpe\": $!\n";
  open RMDUP, ">", $bedpeout or die "Can't open \"$bedpeout\": $!\n";

  print "Scanning \"$bedpe\" for duplicate aligned pairs\n";

  my %refalign;
  my $loaded = 0;

  while (<BEDPE>)
  {
    chomp;

    $loaded++;

    my ($base,$ref1,$s1,$e1,$d1,$ref2,$s2,$e2,$d2) = split /\s+/, $_;

    my $align;
    $align->[0] = $base;

    ## by convention, index alignments by the ref id with lesser value

    if ($ref1 le $ref2)
    {
      $align->[1] = $ref1; $align->[2] = $s1; $align->[3] = $e1; $align->[4] = $d1;
      $align->[5] = $ref2; $align->[6] = $s2; $align->[7] = $e2; $align->[8] = $d2;
    }
    else
    {
      $align->[5] = $ref1; $align->[6] = $s1; $align->[7] = $e1; $align->[8] = $d1; 
      $align->[1] = $ref2; $align->[2] = $s2; $align->[3] = $e2; $align->[4] = $d2; 
    }

    $align->[9] = 0; ## Im not a duplicate to start

    push @{$refalign{$align->[1]}}, $align;
  }

  die "Error reading \"$bedpe\": $!\n" unless eof BEDPE;

  print "  Loaded $loaded aligned pairs\n";

  my $printed = 0;

  ## for each reference sequence
  foreach my $ref (keys %refalign)
  {
    ## sort alignments by position along scaffold
    my @aligns = sort {$a->[2] <=> $b->[2]} @{$refalign{$ref}};

    for (my $i = 0; $i < scalar @aligns; $i++)
    {
      my $ai = $aligns[$i];

      next if $ai->[9];  ## already marked a dup
      
      for (my $j = $i+1; $j < scalar @aligns; $j++)
      {
        my $aj = $aligns[$j];

        if ($aj->[2] > $ai->[2] + $DUP_WIGGLE) { last; }

        next if $aj->[9]; ## already marked a dup

        if (($ai->[4] eq $aj->[4]) &&                   ## same d1
            ($ai->[5] eq $aj->[5]) &&                   ## same r2
            ($aj->[6] >= $ai->[6] - $DUP_WIGGLE) &&     ## same s2
            ($aj->[6] <= $ai->[6] + $DUP_WIGGLE) &&     ## same s2
            ($ai->[8] eq $aj->[8]))                     ## same e2
        {
          $aj->[9] = 1;

          #print STDERR "m: ", join("\t", @{$ai}), "\n";
          #print STDERR "d: ", join("\t", @{$aj}), "\n";
          #print STDERR "\n";
        }
      }
    }

    for (my $i = 0; $i < scalar @aligns; $i++)
    {
      my $ai = $aligns[$i];

      if (!$ai->[9])
      {
        $printed++;
        print RMDUP join("\t", @{$ai}[0..8]), "\n"
            or die "Error writing to \"$bedpeout\": $!\n";
      }
    }
  }

  my $percp = sprintf("%0.02f", 100*$printed / $loaded);
  print "  Kept $printed non-duplicates pairs ($percp%) in file \"$bedpeout\"\n";
  print "\n";

  close RMDUP or die "Failed to close \"$bedpeout\": $!\n";
}

## genReads
##############################################################################

sub genReads
{
  my $bedpe = shift;
  my $fq1 = shift;
  my $fq2 = shift;

  open BEDPE, "<", "$bedpe" or die "Can't open \"$bedpe\": $!\n";

  open BED1, ">", "$fq1.bed" or die "Can't open \"$fq1.bed\": $!\n";
  open BED2, ">", "$fq2.bed" or die "Can't open \"$fq2.bed\": $!\n";

  print "Generating reads from \"$bedpe\"... \n";

  my $loaded = 0;

  while (<BEDPE>)
  {
    $loaded++;

    chomp;
    my ($base,$ref1,$s1,$e1,$d1,$ref2,$s2,$e2,$d2) = split /\s+/, $_;

    print BED1 "$ref1\t$s1\t$e1\t$base/1\t1\t$d1\n"
       or die "Error writing to \"$fq1.bed\": $!\n";
    print BED2 "$ref2\t$s2\t$e2\t$base/2\t1\t$d2\n"
       or die "Error writing to \"$fq2.bed\": $!\n";
  }

  close BED1 or die "Failed to close \"$fq1.bed\": $!\n";
  close BED2 or die "Failed to close \"$fq2.bed\": $!\n";

  print "$loaded pairs read\n";

  runCmd("fastaFromBed 1", "$fq1.sfa",
         "$FASTA_FROM_BED -fi \"$ref\" -bed \"$fq1.bed\" -fo \"$fq1.sfa\" -name -tab -s");

  sfa2fq("$fq1.sfa", $fq1);

  runCmd("fastaFromBed 2", "$fq2.sfa",
         "$FASTA_FROM_BED -fi \"$ref\" -bed \"$fq2.bed\" -fo \"$fq2.sfa\" -name -tab -s");
  sfa2fq("$fq2.sfa", $fq2);
}


## sfa2fq
###############################################################################

sub sfa2fq
{
  my $sfa = shift;
  my $fq  = shift;

  open SFA, "<", "$sfa" or die "Can't open \"$sfa\": $!\n";
  open FQ,  ">", "$fq" or die "Can't open \"$fq\": $!\n";

  my $q_char = chr($QV_READ + ($QV_ILLUMINA) ? 64 : 33);

  my $q;

  print " printing \"$fq\"... \n";

  my $cnt = 0;

  while (<SFA>)
  {
    $cnt++;

    my ($name, $seq) = split /\s+/, $_;

    $q = $q_char x length($seq);

    print FQ "\@$name\n$seq\n+\n$q\n" or die "Error writing to \"$fq\": $!\n";
  }
  die "Error reading \"$sfa\": $!\n" unless eof SFA;

  close FQ or die "Failed to close \"$fq\": $!\n";

  print "$cnt reads printed\n";
}
