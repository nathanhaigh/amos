#!/usr/bin/perl -w

use strict;
use TIGR::Foundation;

my $USAGE = "Usage: zipclap reference query [options]\n";
my $HELPTEXT = qq~
Zips together a query assembly onto a reference assembly. 

  $USAGE
  reference and query can be either .contig or .slice files.

  Options:
  --------
    -c <chain> specify alignment chain to use          (DEFAULT 0)
    -r <refshift> specify reference shift              (DEFAULT 0)
    -C Assume contig to be circular when converting to contig

    -[no]contig [Don't] Create a contig file of result (DEFAULT: -contig)
    -[no]tcov   [Don't] Create a tcov file of result   (DEFAULT: -tcov)
    -[no]tasm   [Don't] Create a tasm file of result   (DEFAULT: -notasm)
    -[no]fasta  [Don't] Create a fasta file of result  (DEFAULT: -nofasta)
    -[no]recall [Don't] Recall the zipped consensus    (DEFAULT: -norecall)'

    -o <prefix>  Specify prefix for out files          (DEFAULT: zip)

    -q <filename> Override snps file query fasta filename (DEPRECATED)

    -v Toggle being more verbose

The reference assembly is held "in place" and the query may be reverse 
complemented or shifted until it aligns well. Alignment gaps are determined
by the snps file (ultimately via nucmer) and ensure the assemblies are aligned
properly. If there are multiple alignments between the two contigs, select
the desired alignment chain with '-c' (starting at 0).

Alignment information is searched in the following order so that if you 
provide a snps file, then nothing else will be searched.
  1) outprefix.snps 
  2) outprefix.aligns
  3) outprefix.delta
  4) nucmer
~;

my $VERSION = "Version 1.30 (Build " . (qw/$Revision$/ )[1] . ")";

#my $platformbin = "/fs/sz-user-supported/Linux-i686/bin";
my $platformbin = "/fs/sz-user-supported/Linux-x86_64/bin";
my $commonbin   = "/fs/sz-user-supported/common/bin";

my $CAT          = "/bin/cat";

my $FINDSNPS     = "$commonbin/findSNPs";
my $SIZEFASTA    = "$commonbin/sizeFasta";

my $NUCMER       = "$platformbin/nucmer";
my $SHOWALIGNS   = "$platformbin/show-aligns";

my $BANK2CONTIG  = "$platformbin/bank2contig";
my $BANK2FASTA   = "$platformbin/bank2fasta";
my $ZIPCONTIGS   = "$platformbin/zipContigs";


my @DEPENDS = 
(
  "TIGR::Foundation",
  $FINDSNPS,     
  $NUCMER,       
  $SHOWALIGNS,
  $SIZEFASTA,    
  $CAT,
  $BANK2CONTIG,
  $BANK2FASTA,
  $ZIPCONTIGS,
);

my $tf = new TIGR::Foundation;
my $doverbose = 0;

sub echo
{
  my $str = join " ", @_;

  $tf->logLocal($str, 4);
  print @_ if ($doverbose);
}

sub runCmd
{
  my $cmd = shift;
  my $info = shift;

  echo "$info... " if defined $info;

  my $exitcode = $tf->runCommand($cmd);

  if ($exitcode)
  {
    echo "failed! ($!)\n" if defined $info;
    $tf->bail("Error with $cmd ($exitcode)");
  }

  echo "ok.\n" if defined $info;
}

sub getSize
{
  my $id = shift;
  my $cmd = "$SIZEFASTA $id";

  $tf->logLocal("Running $cmd... ", 4);

  my $out = `$cmd`;

  $tf->bail("$cmd failed ($?)") if ($?);
  $tf->logLocal("ok.", 4);

  return (split /\s+/, $out)[1];
}

sub getFastaId
{
  my $fasta = shift;

  my $id = undef;

  echo "Getting fasta id from $fasta... ";

  open FASTA, "< $fasta"
    or $tf->bail("Couldn't open $fasta ($!)");

  while (<FASTA>)
  {
    if (/^\>(\S+)/)
    {
      $id = $1;
      last;
    }
  }

  close FASTA;

  $tf->bail("Couldn't find id in $fasta")
    if (!defined $id);

  echo "$id\n";

  return $id;
}



sub loadSNPs
{
  my $snpfile = shift;
  my $chain = shift;
  my $refshift = shift;
  my $queryfasta = shift;
  my $docircular = shift;

  my $reference = shift;
  my $query = shift;
  my $outprefix = shift;

  open SNPFILE, "< $snpfile"
    or $tf->bail("Couldn't open $snpfile ($!)");

  my $foundchain = 0;
  my $aligndata;
  $aligndata->{offset} = 0;

  ## fasta files are guaranteed to exist by slice2fasta
  my $qsize = getSize("$query.fasta");
  my $rsize = getSize("$reference.fasta"); 

  echo "Loading $snpfile\n";
  while (<SNPFILE>)
  {
    chomp;
    my @values = split /, /, $_;

    my $referencepath = $values[0];
    my $referenceid   = $values[1];
    my $querypath     = $values[2];
    my $queryid       = $values[3];
    my $snpchain      = $values[4];

    next if ($snpchain ne $chain);

    $querypath = $queryfasta if (defined $queryfasta);

    $foundchain = 1;

    my $diff = "Unknown Diff";

    if (!exists $aligndata->{direction})
    {
      my $ralignstart = $values[5] - $refshift;
      my $ralignend   = $values[6] - $refshift;
      my $qalignstart = $values[7];
      my $qalignend   = $values[8];
      my $qdirection  = $values[9];

      $aligndata->{direction} = $qdirection;
      $aligndata->{ralignend} = $ralignend;

      if ($qdirection eq "R")
      {
        if ($ralignstart == 1)
        {
          $aligndata->{offset} = $qsize - $qalignstart; 

          my $o = $aligndata->{offset} + 1;
          echo "Type I. qsize $qsize qalignstart $qalignstart ";
          $diff = "RC Query, vi $outprefix.tcov. Jump to index $o (1U)";

          if (!($ralignend == $rsize || $qalignend == 1))
          {
            $tf->bail("Type I Alignment chain does not fully specify alignment!");
          }
        }
        elsif ($qalignstart == $qsize)
        {
          $aligndata->{offset} = 1-$ralignstart;

          echo "Type II. ralignstart $ralignstart ";
          $diff = "xxdiff $reference.tcov $outprefix.tcov";

          if (!($ralignend == $rsize || $qalignend == 1))
          {
            $tf->bail("Type II Alignment chain does not fully specify alignment!");
          }
        }
        else
        {
          $tf->bail("Unusable chain specified (expected rstart=1 or qstart=qsize for reversed query)");
        }
      }
      else
      {
        if (($qalignstart == 1) || ($ralignstart == 1))
        {
          $aligndata->{offset} = $qalignstart - $ralignstart;

          my $o = $aligndata->{offset};

          $diff = ($o<0) ? $reference : $query;
          $diff = "xxdiff $diff.tcov $outprefix.tcov";

          echo "Type III. qalignstart $qalignstart ralignstart $ralignstart ";

          if (!($ralignend == $rsize || $qalignend == $qsize))
          {
            $tf->bail("Type III Alignment chain does not fully specify alignment!");
          }
        }
        else
        {
          $tf->bail("Unusable chain specified (expected rstart=1 or qstart=1 for forward query)");
        }
      }

      echo "offset $aligndata->{offset} ($diff)\n";
    }

    my $rpos  = $values[10]-$refshift;
    my $qpos  = $values[11];
    my $poly  = $values[12];
    my $ptype = $values[14];

    my $polylen = length($poly);

    if ($ptype eq "I")
    {
      echo "Insert $polylen gaps in reference at $rpos\n";
      $aligndata->{reference}->{$rpos} += $polylen;
    }
    elsif ($ptype eq "D")
    {
      echo "Insert $polylen gaps in query at $qpos\n";
      $aligndata->{query}->{$qpos} += $polylen;
    }
    else
    {
      ## Allow snps to zip together
      ## Nothing to do.
    }
  }

  $tf->bail("Can't find requested chain ($chain)\n") if (!$foundchain);

  ## Strip leading and trailing gaps (if not circular)
  if (!$docircular)
  {
    if (exists $aligndata->{query})
    {
      my %qgaps = %{$aligndata->{query}};

      if ($aligndata->{direction} eq 'F')
      {
        if (exists $qgaps{0})
        {
          ## leading gaps in the forward query
          $aligndata->{offset} -= $qgaps{0};
          $aligndata->{query}->{0} = 0;
        }

        if (exists $qgaps{$qsize})
        {
          ## trailing gaps in the forward query
          $aligndata->{query}->{$qsize} = 0;
        }
      }
      else
      {
        if (exists $qgaps{$qsize})
        {
          ## leading gaps in the rc query
          $aligndata->{offset} -= $qgaps{$qsize};
          $aligndata->{query}->{$qsize} = 0;
        }

        if (exists $qgaps{0})
        {
          ## trailing gaps in the rc query
          $aligndata->{query}->{0} = 0;
        }
      }
    }

    if (exists $aligndata->{reference})
    {
      my %rgaps = %{$aligndata->{reference}};

      ## Reference is always forward
      if (exists $rgaps{0})
      {
        ## leading gaps in the reference
        $aligndata->{offset} += $rgaps{0};
        $aligndata->{reference}->{0} = 0;
      }

      if (exists $rgaps{$aligndata->{ralignend}})
      {
        ## trailing gaps in reference
        $aligndata->{reference}->{$aligndata->{ralignend}} = 0;
      }
    }
  }

  return $aligndata;
}

sub createGapList
{
  my $gaptable = shift;
  my $gaplist = undef;

  foreach my $pos (sort keys %$gaptable)
  {
    my $num = $gaptable->{$pos};

    for (my $i = 0; $i < $num; $i++)
    {
      if (defined $gaplist) { $gaplist .= ","; }
      $gaplist .= $pos;
    }
  }

  return $gaplist;
}


MAIN:
{
  my $chain      = 0;
  my $refshift   = 0;

  my $docircular = 0;
  my $docontig   = 1;
  my $dorecall   = 0;
  my $dofasta    = 0;
  my $dotasm     = 0;

  my $queryfasta = undef;

  my $outprefix = "zip";

  ## Become $outprefix.suffix
  my $deltafile = ".delta";
  my $alignfile = ".aligns";
  my $snpfile   = ".snps";

  $tf->addDependInfo(@DEPENDS);
  $tf->setHelpInfo($HELPTEXT);
  $tf->setVersionInfo($VERSION);

  # now we handle the input options
  my $result = $tf->TIGR_GetOptions
               (
                 'c=i',         \$chain,
                 'r=i',         \$refshift,
                 'o=s',         \$outprefix,
                 'q=s',         \$queryfasta,

                 'C|circular!', \$docircular,
                 'contig!',     \$docontig,
                 'fasta!',      \$dofasta,
                 'tasm!',       \$dotasm,
                 'recall!',     \$dorecall,

                 'v|verbose!',  \$doverbose,
                );

  $tf->bail("Command line parsing failed") if (!$result);

  $tf->bail("Converting circular contigs to tasm is unsupported")
    if ($dotasm && $docircular);

  my $bank      = shift; die $USAGE if !defined $bank;
  my $reference = shift; die $USAGE if !defined $reference;
  my $query     = shift; die $USAGE if !defined $query;

  my $debug = $tf->getDebugLevel();
  $tf->setDebugLevel(1) if (!$debug);

  foreach my $s (\$alignfile, \$snpfile, \$deltafile)
  {
    $$s = $outprefix.$$s;
  }

  my %ids;

  ## Strip .contig or .qual
  foreach my $p (\$reference, \$query)
  {
    $ids{$$p} = $$p;
  }

  echo "Reference prefix is \"$reference\"\n";
  echo "Query prefix is \"$query\"\n";

  $tf->bail("Can't access bank $bank")
    if (! -r "$bank");

  foreach my $prefix ($reference, $query)
  {
    if (-r "$prefix.fasta")
    {
      $ids{$prefix} = getFastaId("$prefix.fasta")
    }
    else
    {
      runCmd("echo $prefix > eidlist");
      runCmd("$BANK2FASTA -b $bank -E eidlist -e");
      runCmd("rm eidlist");
    }
  }

  if (! -r $snpfile)
  {
    if (! -r $alignfile)
    {
      if (! -r $deltafile)
      {
        runCmd("$NUCMER $reference.fasta $query.fasta --nooptimize --maxmatch -p $outprefix 2> /dev/null",
               "nucmer $reference $query");
      }

      runCmd("$SHOWALIGNS $deltafile $ids{$reference} $ids{$query} > $alignfile");
    }

    runCmd("$FINDSNPS -flank 0 $alignfile -chain $chain > $snpfile");
  }

  my $aligndata = loadSNPs($snpfile, $chain, $refshift, $queryfasta, $docircular, 
                           $reference, $query, $outprefix);

  if (1)
  {
    my $recall = "";
    $recall = "-c -a 3" if $dorecall; ## Recall conic

    my $o = $aligndata->{offset};

    ## everything from nucmer is 1-based ungapped
    my $cmd = "$ZIPCONTIGS --refcons $bank $query $reference $outprefix";
    $cmd   .= " --debug 4 $recall -1 -U -O $o"; 
    $cmd   .= " -R" if ($aligndata->{direction} eq "R");
    
    my $rgaps = createGapList($aligndata->{reference});
    $cmd .= " -r $rgaps" if defined $rgaps;

    my $qgaps = createGapList($aligndata->{query});
    $cmd .= " -q $qgaps" if defined $qgaps;

    runCmd($cmd, "zipContigs");
  }

  runCmd("echo $outprefix > eidlist");

  runCmd("$BANK2CONTIG $bank -E eidlist > $outprefix.contig")
    if ($docontig);

  runCmd("$BANK2FASTA $bank -E eidlist > $outprefix.fasta")
    if ($dofasta);

  runCmd("rm eidlist");
}

