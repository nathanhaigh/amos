#!/usr/bin/perl -w
use strict;
use Getopt::Long;

$|=1;

## Paths to binaries
my $BWA            = "bwa";
my $FASTX_RC       = "fastx_reverse_complement";
my $FASTA_FROM_BED = "fastaFromBed";
my $FASTQ_RENAME   = "fastq_rename";
my $FASTX_TRIM     = "fastx_trimmer";

## Options
my $RC = 0;
my $READLEN = 100;
my $MAPQ_THRESHOLD = 0;
my $numreads = 0;
my $THREADS = 1;

my $HARD_TRIM = 0;
my $QV_TRIM = 10;
my $QV_ILLUMINA = 0;
my $QV_READ = 30;
my $ADD_SUFFIX = 0;

my $CHECK_DIST = 0;
my $MIN_DIST = 0;
my $MAX_DIST = 0;

my $DUP_WIGGLE = 2;

my $stage = "all";

my $USAGE = "alignextend [options] prefix ref.fa fq1 fq2\n";
my $help;

my $arguments = join(" ", @ARGV);

my $res = GetOptions("help"      => \$help,
                     "reads=n"   => \$numreads,
                     "suffix"    => \$ADD_SUFFIX,
                     "rc"        => \$RC,
                     "qv=n"      => \$QV_TRIM,
                     "trim=n"    => \$HARD_TRIM,
                     "threads=n" => \$THREADS,
                     "len=n"     => \$READLEN,
                     "mapq=n"    => \$MAPQ_THRESHOLD,
                     "I"         => \$QV_ILLUMINA,
                     "q=n"       => \$QV_READ,
                     "check"     => \$CHECK_DIST,
                     "max=n"     => \$MAX_DIST,
                     "min=n"     => \$MIN_DIST,
                     "dup=n"     => \$DUP_WIGGLE,
                     "stage=s"   => \$stage);
 
if ($help)
{
  print $USAGE;
  print "\n";
  print "Align (very) short reads to a draft assembly, and extend reads on their 3' end with the sequence\n";
  print "to where they align. This is especially for extending short mate-pair reads into long reads for\n";
  print "scaffolding.\n";
  print "\n";
  print "Required\n";
  print "  prefix      : prefix for output files\n";
  print "  ref.fa      : path to reference genome\n";
  print "  fq1         : path to first read\n";
  print "  fq2         : path to second read\n";
  print "\n";
  print "Options\n";
  print " -stage <s>   : stage to start at [align,match,rmdup,genreads] (default: $stage)\n";
  print "\n";
  print "align: align reads to assembly\n";
  print "  -reads <n>  : align first n pairs (default: $numreads)\n";
  print "  -suffix     : Add /1 and /2 suffix to reads\n";
  print "  -rc         : reverse complement the reads before alignment\n";
  print "  -qv <n>     : bwa quality soft quality trim (default: $QV_TRIM)\n";
  print "  -thread <n> : number of threads for bwa (default: $THREADS)\n";
  print "  -trim <n>   : hard trim 3' value (default: $HARD_TRIM)\n";
  print "  -I          : read quality values are Illumina format\n";
  print "\n";
  print "match: select pairs that align and are correctly separated\n";
  print "  -mapq <n>   : Only trust alignment with at least this MAPQ (default: $MAPQ_THRESHOLD)\n";
  print "  -check      : Only accept alignments with proper orientation and distance\n";
  print "  -min <n>    : Only accept alignments at least this far apart (default: $MIN_DIST)\n";
  print "  -max <n>    : Only accept alignments less than this far apart (default: $MAX_DIST)\n";
  print "\n";
  print "rmdup: remove duplicates\n";
  print "  -dup <n>    : Filter duplicates with coordinates within this distance (default: $DUP_WIGGLE)\n";
  print "\n";
  print "genreads: extract new reads\n";
  print "  -q <n>      : quality value to assign to read bases (default: $QV_READ)\n";
  print "  -len <n>    : extend to this length (default: $READLEN)\n";

  exit 0;
}

my %validstages;
$validstages{align} = 1;
$validstages{match} = 1;
$validstages{rmdup} = 1;
$validstages{genreads} = 1;
$validstages{all} = 1;


if ($MAX_DIST > 0) { $CHECK_DIST = 1; }
if (($CHECK_DIST) && ($MAX_DIST == 0))
{
  die "ERROR: checking distance, but max=0\n";
}

if (!exists $validstages{$stage})
{
  die "Unknown stage: $stage\n";
}

my $prefix = $ARGV[0] or die $USAGE;
my $ref    = $ARGV[1] or die $USAGE;

$QV_ILLUMINA = (defined $QV_ILLUMINA) ? "-I" : "";

print "Running $0 $arguments\n";

## Align the reads
###############################################################################

if (($stage eq "all") || ($stage eq "align"))
{
  my $fq1    = $ARGV[2] or die $USAGE;
  my $fq2    = $ARGV[3] or die $USAGE;

  die "Cant read $fq1" if (! -r $fq1);
  die "Cant read $fq2" if (! -r $fq2);

  if (! -r "$ref.sa")
  {
    ## Index genome
    runCmd("bwa index", "$ref.sa", "bwa index $ref");
    print "\n";
  }

  foreach my $idx (1..2)
  {
    print "aligning reads: $idx\n";

    my $fq = ($idx == 1) ? $fq1 : $fq2;
    my $samfile = "$prefix.$idx.sam";
    my $bedfile = "$prefix.$idx.bed";

    my $nl = $numreads * 4; ## 4 lines per reads in a fastq
    my $headcmd = ($numreads>0) ?  "head -$nl" : "cat";

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

    runCmd("prepare fq",   "$prefix.$idx.fq",  "$headcmd $fq $TRIM_CMD > $prefix.$idx.fq");

    runCmd("bwa aln",    "$prefix.$idx.sai",   "$BWA aln -t $THREADS $QV_ILLUMINA -q $QV_TRIM $ref $prefix.$idx.fq -f $prefix.$idx.sai >& $prefix.$idx.sai.log");
    runCmd("bwa samse",  "$prefix.$idx.sam",   "$BWA samse -f $samfile $ref $prefix.$idx.sai $prefix.$idx.fq >& $prefix.idx.sam.log");

    sam2bed($samfile, $bedfile);

    print "\n";
  }
}


if (($stage eq "all") || ($stage eq "match"))
{
  $stage="all";
  matchBed("$prefix.1.bed", "$prefix.2.bed", "$prefix.bedpe");
}

if (($stage eq "all") || ($stage eq "rmdup"))
{
  $stage = "all";
  filterDups("$prefix.bedpe", "$prefix.rmdup.bedpe");
}

if (($stage eq "all") || ($stage eq "genreads"))
{
  $stage = "all";
  genReads("$prefix.rmdup.bedpe", "$prefix.1e.fq", "$prefix.2e.fq");
}

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


## sam2bed
###############################################################################

sub sam2bed
{
  my $samfile = shift;
  my $bedfile = shift;

  open SAM, "< $samfile" or die "Can't open $samfile ($!)\n";
  open BED, "> $bedfile" or die "Can't open $bedfile ($!)\n";

  my %reflen;

  print "\nsam2bed $samfile...\n";

  my $rdcnt = 0;
  my $goodrdcnt = 0;
  my $refcnt = 0;

  while (<SAM>)
  {
    chomp;

    if (/^\@SQ/)
    {
      my ($tag, $sn, $ln) = split /\s+/, $_;
      my ($snt, $refid)   = split /:/, $sn;
      my ($lnt, $reflen)  = split /:/, $ln;

      $reflen{$refid} = $reflen;
      $refcnt++;

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
    my $iidx = substr($name, -1, 1);

    next if $rname eq '*';
    next if $mapq < $MAPQ_THRESHOLD;

    my $reflen = $reflen{$rname};

    $goodrdcnt++;

    if ($flag == 0)
    {
      ## Forward strand

      my $startpos = $pos-1;  ## BED is 0-based
      my $endpos = $startpos + $READLEN - 1;

      if ($endpos > $reflen) 
      { 
        $endpos   = $reflen;
        $startpos = $reflen-$READLEN;
        if ($startpos < 0) { $startpos = 0; }
      }

      print BED "$rname\t$pos\t$endpos\t$base/$iidx\t1\t+\n"; 
    }
    elsif ($flag == 16)
    {
      ## Reverse strand

      my $endpos = $pos+length($seq)-1-1; ## BED is 0-based
      my $startpos = $endpos - $READLEN+1;

      if ($startpos <= 0) 
      { 
        $startpos = 0; 
        $endpos = $READLEN;
      }

      if ($endpos > $reflen)
      {
        $endpos = $reflen;
      }

      print BED "$rname\t$startpos\t$endpos\t$base/$iidx\t1\t-\n";
    }
  }

  close BED;

  my $percgood = sprintf("%0.02f", 100*$goodrdcnt / $rdcnt);

  print " Loaded: $refcnt ref sequences\n";
  print " Printed $goodrdcnt of $rdcnt reads ($percgood%)\n";
}


## matchBed
###############################################################################

sub matchBed
{
  my $bed1 = shift;
  my $bed2 = shift;
  my $bedpe = shift;

  open BED1, "$bed1" or die "Can't open $bed1 ($!)\n";
  open BED2, "$bed2" or die "Can't open $bed2 ($!)\n";
  open BEDPE, "> $bedpe" or die "Can't open $bedpe ($!)\n";

  print "matchBed\n";
  print " scanning $bed1... ";

  my %coords;

  my $cnt1 = 0;

  while (<BED1>)
  {
    $cnt1++;
    chomp;
    my ($ref, $s, $e, $name, $j, $d) = split /\s+/, $_;

    my $base = substr($name, 0, -2); ## trim /1 or /2

    my $rec;
    $rec->[0] = $ref;
    $rec->[1] = $s;
    $rec->[2] = $e;
    $rec->[3] = $d;

    $coords{$base} = $rec;
  }

  print "$cnt1 reads\n";

  my $cnt2 = 0;

  my $printed = 0;
  my $matched = 0;

  print " scanning $bed2... ";

  while (<BED2>)
  {
    $cnt2++;

    chomp;
    my ($ref2, $s2, $e2, $name, $j, $d2) = split /\s+/, $_;

    my $base = substr($name, 0, -2); ## trim /1 or /2

    if (exists $coords{$base})
    {
      $matched++;

      my $a1 = $coords{$base};
      my $ref1 = $a1->[0];
      my $s1   = $a1->[1];
      my $e1   = $a1->[2];
      my $d1   = $a1->[3];

      my $doprint = 1;

      if ($CHECK_DIST)
      {
        if ($ref1 eq $ref2)
        {
          if ($d1 eq $d2)
          {
            $doprint = 0;
          }
          else
          {
            my $dist = 0;

            if ($d1 eq "+") { $dist = $e2-$s1; }
            else            { $dist = $e1-$s2; }

            if (($dist < $MIN_DIST) || ($dist > $MAX_DIST))
            {
              $doprint = 0;
            }
          }
        }
      }

      if ($doprint) 
      { 
        $printed++;
        print BEDPE "$base\t$ref1\t$s1\t$e1\t$d1\t$ref2\t$s2\t$e2\t$d2\n";
      }
    }
  }

  close BEDPE;

  my $perc1 = sprintf("%0.02f", 100*$matched / $cnt1);
  my $perc2 = sprintf("%0.02f", 100*$matched / $cnt2);

  print "$cnt2 reads\n";
  print " matched: $matched ($perc1% of fq1 $perc2% of fq2\n";

  if ($CHECK_DIST)
  {
    my $percp = sprintf("%0.02f", 100*$printed / $matched);
    print " passed: $printed of $matched ($percp%)\n";
  }

  print "\n";
}




## filterDups
##############################################################################

sub filterDups
{
  my $bedpe = shift;
  my $bedpeout = shift;

  open BEDPE, "$bedpe" or die "Can't open $bedpe ($!)\n";
  open CLEAN, ">$bedpeout" or die "Can't open $bedpeout ($!)\n";

  print "scanning $bedpe for duplicates\n";

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

  print " loaded $loaded alignments\n";

  my $printed = 0;

  ## for each reference sequence
  foreach my $ref (keys %refalign)
  {
    ## sort alignments by position along contig
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
        print CLEAN join("\t", @{$ai}[0..8]), "\n";
      }
    }
  }

  my $percp = sprintf("%0.02f", 100*$printed / $loaded);
  print " kept $printed non-duplicates ($percp%)\n";
  print "\n";

  close CLEAN;
}

## genReads
##############################################################################

sub genReads
{
  my $bedpe = shift;
  my $fq1 = shift;
  my $fq2 = shift;

  open BEDPE, "$bedpe" or die "Can't open $bedpe ($!)\n";

  open BED1, ">$fq1.bed" or die "Can't open $fq1.bed ($!)\n";
  open BED2, ">$fq2.bed" or die "Can't open $fq2.bed ($!)\n";

  print "generating reads from $bedpe... ";

  my $loaded = 0;

  while (<BEDPE>)
  {
    $loaded++;

    chomp;
    my ($base,$ref1,$s1,$e1,$d1,$ref2,$s2,$e2,$d2) = split /\s+/, $_;

    print BED1 "$ref1\t$s1\t$e1\t$base/1\t1\t$d1\n"; 
    print BED2 "$ref2\t$s2\t$e2\t$base/2\t1\t$d2\n"; 
  }

  close BED1;
  close BED2;

  print "$loaded pairs read\n";

  runCmd("fastaFromBed 1", "$fq1.sfa", "$FASTA_FROM_BED -fi $ref -bed $fq1.bed -fo $fq1.sfa -name -tab -s");
  sfa2fq("$fq1.sfa", $fq1);

  runCmd("fastaFromBed 2", "$fq2.sfa", "$FASTA_FROM_BED -fi $ref -bed $fq2.bed -fo $fq2.sfa -name -tab -s");
  sfa2fq("$fq2.sfa", $fq2);
}


## sfa2fq
###############################################################################

sub sfa2fq
{
  my $sfa = shift;
  my $fq  = shift;

  open SFA, "$sfa" or die "Can't open $sfa ($!)\n";
  open FQ,  ">$fq" or die "Can't open $fq ($!)\n";

  my $q;

  print " printing $fq... ";

  my $cnt = 0;

  while (<SFA>)
  {
    $cnt++;

    my ($name, $seq) = split /\s+/, $_;

    if ($QV_ILLUMINA) { $q = 'a' x length($seq); }
    else              { $q = 'A' x length($seq); }

    print FQ "\@$name\n$seq\n+\n$q\n";
  }

  close FQ;

  print "$cnt reads printed\n";
}
