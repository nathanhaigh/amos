#!/usr/bin/perl -w

use strict;
use TIGR::Foundation;
use File::Spec;
use Cwd;

my $FATTENRADIUS = 500;

my $MATCHLEN = 15;
my $CLUSTERLEN = 30;

my $MATCHLEN2 = 10;
my $CLUSTERLEN2 = 15;

my $USAGE = "Usage: aj_joinContigs.pl rid roo qid qoo [rex] [qex] [OPTIONS]\n";

my $HELPTEXT = qq~
Joins contigs together with optional contig extension

   $USAGE
   rid is the reference contig id
   roo is the orientation of the reference

   qid is the query contig id
   qoo is the orientation of the query

   {r,q}id.{contig,qual,seq} must be in cwd or contained in dataprefix

   [rex] specifies optional reference contig extension
   [qex] specifies optional query contig extension

   [rex] and [qex] format is:
   [r|q]ex seqname distance

   Options
   -------
   -[no]fatten    Fatten the join region 
                  [DEFAULT: -fatten]

   -id <prefix>   Prefix of output contig,seq,qual files
                  [DEFAULT: 'join']

   -dir <dir>     Directory to write results            
                  [DEFAULT: 'joincontigs']

   -data <prefix> Prefix to master contig,qual,seq input files

   -report <file> Filename of join report
                  [DEFAULT: 'join.report']
~;

my $VERSION = " Version 1.00 (Build " . (qw/$Revision$/ )[1] . ")";

my $platformbin = "/fs/sz-user-supported/Linux-x86_64/bin";
my $commonbin = "/fs/sz-user-supported/common/bin";

my $CAT           = "/bin/cat";
my $GETCOVERAGE   = "$platformbin/getCoverage";
my $AUGMENTSLICE  = "$platformbin/augmentSlice";
my $SLICE2FASTA   = "$platformbin/slice2fasta";
my $SLICE2CONTIG  = "$platformbin/slice2contig";
my $NUCMER        = "$platformbin/nucmer";
my $SHOWCOORDS    = "$platformbin/show-coords";
my $ZIPCLAP       = "$platformbin/zipclap";
my $FATTENSLICE   = "$platformbin/fattenSlice";

my $SIZEFASTA     = "$commonbin/sizeFasta";
my $FILTERSEQ     = "$commonbin/filter_seq";
my $FILTERCONTIG  = "$commonbin/filter_contig";

## TODO: Enable optimize slice
my $OPTIMIZESLICE = "optimizeSlice";

my @DEPENDS = 
(
  "TIGR::Foundation",
  $FILTERSEQ,
  $FILTERCONTIG,
  $GETCOVERAGE, 
  $AUGMENTSLICE,
  $FATTENSLICE, 
  $SLICE2FASTA, 
  $SLICE2CONTIG,
  $ZIPCLAP,     
  $NUCMER,      
  $SHOWCOORDS,  
  $SIZEFASTA,   
  $CAT,
);

my $tf = new TIGR::Foundation;
my $doverbose = 1;
my $reportsuffix = "report";

sub echo
{
  my $str = join " ", @_;

  $tf->logLocal($str, 4);
  if ($doverbose)
  {
    print @_;
  }
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

  if ($?)
  {
    echo "failed!\n";
    $tf->bail("$cmd failed ($?)") if ($?);
  }
  
  return (split /\s+/, $out)[1];
}

sub prepareInputs
{
  my $contig = shift;
  my $origdir = shift;
  my $data = shift;

  my $id     = $contig->{id};

  ## Contig must be first file!
  foreach my $suffix (qw/contig qual seq/)
  {
    if (! -r "$id.$suffix")
    {
      if (-r "$origdir/$id.$suffix")
      {
        symlink "$origdir/$id.$suffix", "$id.$suffix"
          or $tf->bail("Can't symlink $origdir/$id.$suffix");
      }
      elsif (defined $data)
      {
        if (-r "$data.$suffix")
        {
          if ($suffix eq "contig")
          {
            runCmd("$FILTERCONTIG $data.contig $id > $id.contig")
          }
          else
          {
            runCmd("$FILTERSEQ $id.contig $data.$suffix > $id.$suffix");
          }
        }
        else
        {
          $tf->bail("Can't read $data.$suffix");
        }
      }
      else
      {
        $tf->bail("Need input for $id.$suffix");
      }
    }
  }


  my $prefix = "$id.aug";

  runCmd("$GETCOVERAGE --silent $id -l -p | $AUGMENTSLICE $id - -o $prefix.slice")
    if (! -r "$prefix.slice");

  runCmd("ln -s $id.qual $prefix.qual")
    if (! -r "$prefix.qual");

  $contig->{prefix} = $prefix;
  $contig->{slice} = "$prefix.slice";
  $contig->{ex} = 0;
}

sub extendContig
{
  my $results = shift;
  my $contig = shift;
  my $type = shift;

  my $id = $contig->{id};
  my $prefix = "$id.extend";
  my $report = "$prefix.$reportsuffix";

  $results->{$type."exreport"} = $report;

  if (! -r "$prefix.slice")
  {
    my $cmd = "$FATTENSLICE -r $contig->{seqname} -d $contig->{dist} $contig->{slice} -o $prefix.slice 2>$report";
    my $info = "extending $id";
    runCmd($cmd, $info);
  }

  runCmd("ln -s $id.qual $prefix.qual")
    if (! -r "$prefix.qual");

  if (-r $report)
  {
    my $seqname = $contig->{seqname};
    open REPORT, "< $report"
      or $tf->bail("Can't open $report ($!)");

    while (<REPORT>)
    {
      next if (!/$seqname=/);
      my ($tag, $values) = split /=/, $_;

      my @vals = split /\s+/, $values;
      my $consex = $vals[2];

      if ($consex > $contig->{dist})
      {
        $tf->logError("Error: consex:$consex > dist:$contig->{dist}\n");
        $consex = $contig->{dist};
      }

      $contig->{ex} = $consex;
      last;
    }
    close REPORT;
  }

  $contig->{prefix} = $prefix;
  $contig->{slice} = "$prefix.slice";
}

sub createFasta
{
  my $contig = shift;
  my $prefix = $contig->{prefix};

  runCmd("$SLICE2FASTA -i $contig->{id} $contig->{slice} -o $prefix.fasta")
    if (! -r "$prefix.fasta");

  $contig->{size} = getSize("$prefix.fasta");
}

sub max
{
  $_[0] > $_[1] ? $_[0] : $_[1];
}

sub min
{
  $_[0] < $_[1] ? $_[0] : $_[1];
}

sub findChain
{
  my $rcontig = shift;
  my $qcontig = shift;
  my $coords = shift;
  my $results = shift;

  my $roo = $rcontig->{oo};
  my $qoo = $qcontig->{oo};

  my $type;

  my $str = "Finding chain roo: $roo qoo: $qoo type: ";
  if    (!$roo && !$qoo) { $type = 1; $str .= "$type rend:$rcontig->{size} qbegin:1"; }
  elsif (!$roo &&  $qoo) { $type = 2; $str .= "$type rend:$rcontig->{size} qbegin:$qcontig->{size}"; }
  elsif ( $roo &&  $qoo) { $type = 3; $str .= "$type rbegin:1 qend:$qcontig->{size}";}
  elsif ( $roo && !$qoo) { $type = 4; $str .= "$type rbegin:1 qend:1";}
  echo "$str\n";

  open COORDS, "< $coords" 
    or $tf->bail("Can't open $coords ($!)");

  my $inalignment = 0;
  my $chain = 0;

  my $rangebegin = -1;
  my $rangeend = -1;

  while (<COORDS>)
  {
    if (!$inalignment) { $inalignment = 1 if (/=========/); }
    else
    {
      my @values = split /\s+/, $_;

      my $rbegin  = $values[1];
      my $rend    = $values[2];

      my $qbegin  = $values[4];
      my $qend    = $values[5];
 
      my $rlen    = $values[7];
      my $qlen    = $values[8];

      my $idscore = $values[10];

      my $valid = 0;

      #echo "$chain: r:[$rbegin,$rend] q:[$qbegin,$qend]\n";

      if ($type == 1)
      {
        if ($rend == $rcontig->{size} && $qbegin == 1)
        {
          $rangebegin = min($rbegin, $rcontig->{size}-$rcontig->{ex});
          $rangeend = -($qcontig->{size} - max($qend, $qcontig->{ex}));

          $valid = 1;
        }
      }
      elsif ($type == 2)
      {
        if ($rend == $rcontig->{size} && $qbegin == $qcontig->{size})
        {
          $rangebegin = min($rbegin, $rcontig->{size}-$rcontig->{ex});
          $rangeend = -min($qend, $qcontig->{size}-$qcontig->{ex});

          $valid = 1;
        }
      }
      elsif ($type == 3)
      {
        if ($rbegin == 1 && $qend == $qcontig->{size})
        {
          $rangebegin = min($qbegin, $qcontig->{size}-$qcontig->{ex});
          $rangeend = -($rcontig->{size} - max($rend, $rcontig->{ex}));

          $valid = 1;
        }
      }
      elsif ($type == 4)
      {
        if ($rbegin == 1 && $qend == 1)
        {
          $rangebegin = $qcontig->{size} - max($qbegin, $qcontig->{ex});
          $rangeend = -($rcontig->{size} - max($rend, $rcontig->{ex}));

          $valid = 1;
        }
      }

      if ($valid)
      {
        echo "$chain: r:[$rbegin,$rend] q:[$qbegin,$qend]\n";

        $results->{rangebegin} = $rangebegin;
        $results->{rangeend}   = $rangeend;
        $results->{chain}      = $chain;
        $results->{idscore}    = $idscore;
        $results->{rex}        = $rcontig->{ex};
        $results->{qex}        = $qcontig->{ex};

        $results->{extension}  = $results->{rex} + $results->{qex};
        $results->{olen}       = ($rlen + $qlen) / 2;
        $results->{gapsize}    = $results->{extension} - $results->{olen};

        return $chain;
      }

      $chain++;
    }
  }

  echo "Can't find usable chain\n";
  return -1;
}



sub alignContigs
{
  my $rcontig = shift;
  my $qcontig = shift;
  my $results = shift;

  createFasta($rcontig);
  createFasta($qcontig);

  my $rprefix = $rcontig->{prefix};
  my $qprefix = $qcontig->{prefix};
 
  my $prefix = $results->{sliceprefix};
  my $deltafile = "$prefix.delta";
  my $coords = "$prefix.coords";

  if (! -r $deltafile)
  {
    my $cmd = "$NUCMER $rprefix.fasta $qprefix.fasta --nooptimize --maxmatch -l $MATCHLEN -c $CLUSTERLEN -p $prefix 2> /dev/null";
    runCmd($cmd, "nucmer");
  }

  runCmd("$SHOWCOORDS $deltafile > $coords")
    if (! -r $coords);

  my $chain = findChain($rcontig, $qcontig, $coords, $results);

  if ($chain == -1)
  {
    my $cmd = "$NUCMER $rprefix.fasta $qprefix.fasta --nooptimize --maxmatch -l $MATCHLEN2 -c $CLUSTERLEN2 -p $prefix 2> /dev/null";
    runCmd($cmd, "nucmer2");

    runCmd("$SHOWCOORDS $deltafile > $coords");

    $chain = findChain($rcontig, $qcontig, $coords, $results);

    $tf->bail("Can't find usable chain after two attempts\n", 2)
      if ($chain == -1);
  }

  return $chain;
}

sub joinContigs
{
  my $rcontig = shift;
  my $qcontig = shift;
  my $results = shift;

  my $rid = $rcontig->{id};
  my $qid = $qcontig->{id};

  $results->{reference} = $rid;
  $results->{query} = $qid;

  my $prefix = "aj-$rid-$qid";
  $results->{sliceprefix} = $prefix;
  
  my $chain = alignContigs($rcontig, $qcontig, $results);

  if (! -r "$prefix.slice")
  {
    ## Create a fasta so the size can be found, but this won't exactly be the fasta of the final contig
    my $cmd = "$ZIPCLAP $rcontig->{slice} $qcontig->{slice} -c $chain -o $prefix --nocontig --fasta --debug 4";
    runCmd($cmd, "zipclap");
  }

  $results->{joinlen}   = getSize("$prefix.fasta");
  $results->{joinbegin} = $results->{rangebegin};
  $results->{joinend}   = $results->{joinlen} + $results->{rangeend};

  $results->{finalbegin} = $results->{joinbegin};
  $results->{finalend}   = $results->{joinend};

}

##TODO: Make fattenslice smarter, 
##      so there won't be as much wiggle on the fatten region
sub fattenJoin
{
  my $results = shift;

  my $prefix = $results->{sliceprefix};
  my $fprefix = "$prefix.fatten";

  my $fattenreport = "fatten.$reportsuffix";
  $results->{fattenreport} = $fattenreport;

  $results->{finalbegin} = max($results->{joinbegin} - $FATTENRADIUS, 1);
  $results->{finalend}   = min($results->{joinend}   + $FATTENRADIUS, $results->{joinlen});

  if (! -r "$fprefix.slice")
  {
    my $cmd = "$FATTENSLICE $prefix.slice -o $fprefix.slice -U -x $results->{finalbegin} -y $results->{finalend} 2> $fattenreport";
    runCmd($cmd, "fattenSlice");
  }

  $results->{sliceprefix} = $fprefix;
}

##TODO: Enable optimizing
sub optimizeJoin
{
  my $results = shift;

  my $prefix = $results->{sliceprefix};
  my $oprefix = "$prefix.opt";

  if (! -r "$oprefix.slice")
  {
    my $cmd = "$OPTIMIZESLICE $prefix.slice -o $oprefix.slice";
    runCmd($cmd, "optimizeSlice");
  }

  $results->{sliceprefix} = $oprefix;
}

sub finalizeResults
{
  my $results = shift;
  my $dotcov = shift;

  my $prefix = $results->{joinid};
  my $slice = $results->{sliceprefix};

  runCmd("$SLICE2CONTIG $slice.slice -o $prefix.contig -i $prefix", "slice2contig")
    if (! -r "$prefix.contig");

  runCmd("$SLICE2FASTA $slice.slice -o $prefix.fasta -i $prefix")
    if (! -r "$prefix.fasta");

  my $qid = $results->{query};
  my $rid = $results->{reference};

  runCmd("$CAT $rid.qual $qid.qual > $prefix.qual")
    if (! -r "$prefix.qual");

  if (! -r "$prefix.seq")
  {
    open OSEQ, "> $prefix.seq"
      or $tf->bail("Can't open $prefix.seq for writing ($!)");

    my %contig;
    open CONTIG, "< $prefix.contig"
      or $tf->bail("Can't open $prefix.contig for reading ($!)");

    while (<CONTIG>)
    {
      next if ((! /^#/) || (/^##/));

      if (/^#(\S+)\(.*{(\d+) (\d+)}/)
      {
        my $seqname = $1;
        my $clr1 = $2;
        my $clr2 = $3;

        if ($clr2 < $clr1)
        {
          my $t = $clr1;
          $clr1 = $clr2;
          $clr2 = $t;
        }

        $contig{$seqname}->{clr1} = $clr1;
        $contig{$seqname}->{clr2} = $clr2;
      }
    }

    close CONTIG;

    foreach my $id ($rid, $qid)
    {
      open SEQ, "< $id.seq"
        or $tf->bail("Can't open $id.seq for reading ($!)");

      while (<SEQ>)
      {
        if (/>(\S+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)/)
        {
          if (exists $contig{$1})
          {
            my $clr1 = $contig{$1}->{clr1};
            my $clr2 = $contig{$1}->{clr2};

            if ($clr1 > $5) { $clr1 = $5 };
            if ($clr2 < $6) { $clr2 = $6 };

            $tf->logLocal("Extending clear range for $1", 4);
            print OSEQ ">$1 $2 $3 $4 $clr1 $clr2\n";
          }
          else
          {
            print OSEQ $_;
          }
        }
        else
        {
          print OSEQ $_;
        }
      }

      close SEQ;
    }

    close OSEQ;
  }

  runCmd("$GETCOVERAGE -t $prefix --silent")
    if ($dotcov && ! -r "$prefix.tcov");
}

sub printJoinReport
{
  my $results = shift;

  open JOINREPORT, "> $results->{joinreport}"
    or $tf->bail("Can't open $results->{joinreport} ($!)");

  delete $results->{joinreport};

  foreach my $key (sort keys %{$results})
  {
    print JOINREPORT "$key=$results->{$key}\n";
  }

  close JOINREPORT;
}



MAIN:
{
  my $dofatten = 1;
  my $outdir = "joincontigs";
  my $outprefix = "join";
  my $dotcov = 0;

  my $data = undef;
  my $report = "$outprefix.$reportsuffix";

  $tf->addDependInfo(@DEPENDS);
  $tf->setHelpInfo($HELPTEXT);
  $tf->setVersionInfo($VERSION);

  # now we handle the input options
  my $result = $tf->TIGR_GetOptions
               (
                 'v|verbose!',  \$doverbose,
                 'fatten!',     \$dofatten,
                 'tcov!',       \$dotcov,
                 'dir=s',       \$outdir,
                 'id=s',        \$outprefix,
                 'data=s',      \$data,
                 'report=s',    \$report,
                );

  $tf->bail("Command line parsing failed") if (!$result);

  my $results;
  $results->{joinid} = $outprefix;
  $results->{joinreport} = $report;

  my $rcontig;
  $rcontig->{id}     = shift @ARGV;
  $rcontig->{oo}     = shift @ARGV;
  $rcontig->{ex}     = 0;

  my $qcontig;
  $qcontig->{id}     = shift @ARGV;
  $qcontig->{oo}     = shift @ARGV;
  $qcontig->{ex}     = 0;

  die $USAGE if (!defined $qcontig->{oo});

  my $debug = $tf->getDebugLevel();
  $tf->setDebugLevel(4) if (!$debug);

  while (defined (my $ex = shift @ARGV))
  {
    my $seqname = shift @ARGV;
    my $dist = shift @ARGV;

    my $contig = undef;

    if    ($ex eq "rex") { $contig = $rcontig; }
    elsif ($ex eq "qex") { $contig = $qcontig; }
    else                 { die $USAGE;         }

    if (exists $contig->{seqname}) { die $USAGE; }

    $contig->{seqname} = $seqname;
    $contig->{dist} = $dist;
  }

  $data = File::Spec->rel2abs($data) if defined $data;

  my $origdir = cwd;

  mkdir $outdir if ! -r $outdir;
  chdir $outdir;

  ## origdir should just be ..
  $origdir = File::Spec->abs2rel($origdir);
  $data = File::Spec->abs2rel($data) if defined $data;

  echo "prepare inputs for joining... ";
  prepareInputs($rcontig, $origdir, $data);
  prepareInputs($qcontig, $origdir, $data);
  echo "ok.\n";

  extendContig($results, $rcontig, "r") if (defined $rcontig->{seqname});
  extendContig($results, $qcontig, "q") if (defined $qcontig->{seqname});

  joinContigs($rcontig, $qcontig, $results);

  fattenJoin($results) if ($dofatten);

  finalizeResults($results, $dotcov);

  printJoinReport($results);
}

