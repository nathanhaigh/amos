#!/usr/bin/perl -w

use strict;
use TIGR::Foundation;
use File::Spec;
use File::Copy;
use File::Basename;
use Cwd;

$|++; # no buffering on STDOUT

my $platformbin = "/fs/sz-user-supported/Linux-x86_64/bin";
my $commonbin   = "/fs/sz-user-supported/common/bin";

my $VERSION             = " Version 1.20 (Build " . (qw/$Revision$/ )[1] . ")";

my $CAT                 = "/bin/cat";
my $RM                  = "/bin/rm";

my $AEEXEC              = "/usr/local/common/aeexec";
my $REMAPFEATURES       = "/usr/local/common/remapFeatures";

my $CONTIG2FASTA        = "$commonbin/contig2fasta";
my $SPLITFASTA          = "$commonbin/splitFasta";
my $SIZEFASTA           = "$commonbin/sizeFasta";
my $CUTFASTA            = "$commonbin/cutFasta";
my $CA2AJSCAFF          = "$commonbin/ca2ajscaff";
my $FILTERCONTIG        = "$commonbin/filter_contig";
my $FILTERSEQ           = "$commonbin/filter_seq";
my $EVALOVERLAP         = "$commonbin/aj_evaluateSequenceOverlaps";

my $NUCMER              = "$platformbin/nucmer";
my $SHOWCOORDS          = "$platformbin/show-coords";
my $JOINCONTIGS         = "$platformbin/aj_joinContigs";
my $LOWCOMPLEXITYFILTER = "$platformbin/aj_lowcomplexityfilter";
my $TOAMOS              = "$platformbin/toAmos";
my $AJSCAFF2AMOS        = "$platformbin/aj_scaff2amos";
my $BANKTRANSACT        = "$platformbin/bank-transact";

my $BANK2CONTIG  = "bank2contig";
my $DUMPREADS    = "dumpreads";
my $BANK2SCAFF   = "bank2scaff";
my $AMOS2MATES   = "amos2mates";
my $BANKREPORT   = "bank-report";
my $LOADFEATURES = "loadFeatures";
my $DUMPFEATURES = "dumpFeatures";


my $SELF          = $0;
my $USAGE         = "Usage: autoJoin <prefix.bnk> [OPTIONS]\n";

my $tf            = new TIGR::Foundation;

my $DOVERBOSE     = 1;
my $DOJOIN        = 0;
my $DOVALIDATE    = 0;
my $DOAEVALIDATE  = 0;

my $MATCHLEN      = 15;
my $CLUSTERLEN    = 30;
my $RANGEWINDOW   = 100;  ## FATTENRADIUS is already ~500
my $FSEQOVERLAP   = 10000;
my $DEFAULT_STDEV = 100;

my $ALIGNTHRESHOLD = 94.0; ## Minimum alignment similiarity (%ID) for joining contigs

my $MAXGAPSIZE    = 1000;    ## Max Gap size before giving up
my $MINGAPSTDEV   = 100.00;  ## Hard floor on gap standard deviation
my $GAPSTDTHRESH  = 4;       ## Max acceptable number of standard deviations

my $JOINPREFIX    = "aj_";
my $JOINSUFFIX    = "_joined";
my $ALIGNMENTDIR  = "alignment";
my $RESULTSDIR    = "results";
my $JOINDIR       = "joincontigs";
my $OVERLAPREPORT = "overlap.report";


my $HELPTEXT = qq~
AutoJoin contigs and close sequencing gaps in an assembly through contig
extension. The input is an AMOS bank (prefix.bnk) containing contigs and
scaffold. Within the autoJoin work directory (-dir), there will be an alignment
directory which contains all of the alignment information and intermediate
files for the join process. Final results are stored in a results directory.

The main result is a prefix.bnk in the results directory containing a
new AMOS bank with the AutoJoined assembly. In addition, in the results
directory are the prefix.{contig,qual,seq,pos} files which contain the entire
assembly after autoJoiner. (prefix_joined contains just those contigs that
autoJoiner sucessful on.) A report on the results of autoJoiner are stored
in prefix.joinreport.

In addition, a prefix.unjoined file is created which lists every contig id
that autoJoiner did not perform any join actions on, prefix.feat which has
all original features remapped plus new AUTOJOIN features, and a prefix.scaff
file containing the updated scaffold.

  $USAGE
    prefix.bnk is the path to the AMOS bank.

  Options
  -------
  -dir <dir>    Output directory                 [ Default: autoJoin ]
  -prefix <pre> Prefix for joined contig names   [ Default: aj_ ]
  -join         Actually do the joins            [ Default: nojoins ]

  -alignment <threshold> Specify Minimum percent identity for an alignment 
                         to be used for joining contigs together.
                         [ Default: 94.0 ]
~;

my $EXTRAHELP = qq~
Within the results directory, autoEditor can be run and creates its own output 
in the autoEditor directory, and a upload directory which contains all of the
files necessary to load the AutoJoined+AutoEdited assembly into the database 
(via aloader). In addition the prefix.sq.contigs file is present which has
been updated for the autoEditor results.

  -D <db>         Assembly project (for autoEditor & validation)
  -[no]join       Toggle between eval & eval+join    [ Default: -join ]
  -[no]validate   Validate the join against a 1con   [ Default: -novalidate ]
  -[no]edit       Edit the resultant assembly        [ Default: -noedit ]
  -[no]aevalidate Validate against AE contigs        [ Default: -noaevalidate ]
~;

my @DEPENDS = 
(
  "TIGR::Foundation",
  $CA2AJSCAFF,
  $SPLITFASTA,
  $CONTIG2FASTA,
  $FILTERCONTIG,
  $FILTERSEQ,
  $NUCMER,
  $SHOWCOORDS,
  $SIZEFASTA,
  $CAT,
);

sub echo
{
  my $str = join " ", @_;

  $tf->logLocal($str, 4);
  print @_ if ($DOVERBOSE);
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

sub tfmkdir
{
  runCmd("mkdir $_[0]") if (! -r $_[0]);
}

sub tfchdir
{
  $tf->logLocal("chdir $_[0]",4);
  chdir $_[0];
}


{
  my $joinid = 0;
  sub getJoinId
  {
    $joinid++;
    return "$JOINPREFIX$joinid";
  }
}

sub getSize
{
  my $file = shift;
  my $cmd = "$SIZEFASTA $file";

  $tf->logLocal("Running $cmd", 4);
  my $out = `$cmd`;

  if ($?)
  {
    echo "failed!\n";
    $tf->bail("$cmd failed ($?)") if ($?);
  }

  return (split /\s+/, $out)[1];
}

sub getGappedSize
{
  my $contig = shift;

  my $gsize = -1;

  open CONTIG, "< $contig"
    or $tf->bail("Can't open $contig");

  while (<CONTIG>)
  {
    if (/^\#\#(\S+) \d+ (\d+) bases/)
    {
      $gsize = $2;
      last;
    }
  }

  close CONTIG;

  $tf->bail("Invalid contig file $contig") 
    if $gsize == -1;

  return $gsize;
}


sub tfsymlink
{
  $tf->bail("Can't read $_[0]")
    if (! -r $_[0]);

  symlink ($_[0], $_[1])
    or $tf->bail("Can't symlink $_[0] to $_[1]");
}

{
  my %features;

  sub loadFeatures
  {
    my $featfile = shift;
    echo " loading existing features from $featfile\n";

    open FEAT, "< $featfile"
      or $tf->bail("Can't read $featfile ($!)\n");

    while (<FEAT>)
    {
      my $feature;

      chomp;
      my @fields = split /\s+/, $_;

      my $contigid        = shift @fields;
      $feature->{type}    = shift @fields;
      $feature->{end5}    = shift @fields;
      $feature->{end5g}   = $feature->{end5};
      $feature->{end3}    = shift @fields;
      $feature->{end3g}   = $feature->{end3};
      $feature->{comment} = join(" ", @fields);

      push @{$features{$contigid}}, $feature;
    }
    
    close FEAT;
  }

  sub printFeatures
  {
    my $filename = shift;

    open NEWFEAT, "> $filename"
      or $tf->bail("Can't read $filename ($!)\n");

    foreach my $contigid (sort keys %features)
    {
      next if !defined $features{$contigid};
      #next if !getContigInfo($contigid)->{isjoin};

      foreach my $feature (@{$features{$contigid}})
      {
        my $end5    = $feature->{end5};
        my $end3    = $feature->{end3};
        my $type    = $feature->{type};
        my $comment = $feature->{comment};

        print NEWFEAT "$contigid\t$type\t$end5\t$end3\t$comment\n";
      }
    }
  }

  sub createJoinFeature
  {
    my $gapinfo = shift;
    my $joinid = $gapinfo->{autojoin}->{id};

    my $left   = $gapinfo->{left}->{id};
    my $right  = $gapinfo->{right}->{id};

    my $gapsize = $gapinfo->{autojoin}->{gapsize};
    my $idscore = $gapinfo->{autojoin}->{idscore};
    my $olen    = $gapinfo->{autojoin}->{olen};

    my $joinfeature;
    $joinfeature->{isjoin} = 1;

    $joinfeature->{type}    = "J";
    $joinfeature->{comment} = qq~AutoJoin $left $right gapsize:$gapsize overlap:$olen bp @ $idscore%\n~;

    ## TODO: These are ungapped coordinates
    $joinfeature->{end5} = $gapinfo->{autojoin}->{begin};
    $joinfeature->{end3} = $gapinfo->{autojoin}->{end};

    return $joinfeature;
  }

  sub remapJoinedFeatures
  {
    my $gapinfo = shift;

    my $left       = $gapinfo->{left}->{id};
    my $leftsize   = $gapinfo->{left}->{size};
    my $leftgsize  = $gapinfo->{left}->{gsize};

    my $right      = $gapinfo->{right}->{id};
    my $rightsize  = $gapinfo->{right}->{size};
    my $rightgsize = $gapinfo->{right}->{gsize};

    my $joinid     = $gapinfo->{autojoin}->{id};
    my $joinsize   = $gapinfo->{autojoin}->{size};
    my $joingsize  = $gapinfo->{autojoin}->{gsize};

    if (defined $features{$left} || defined $features{$right})
    {
      my $jointype;

      if ($gapinfo->{left}->{oo} eq "BE")
      {
        ## left features are the same
        push @{$features{$joinid}}, @{$features{$left}}
          if defined $features{$left};

        if ($gapinfo->{right}->{oo} eq "BE")
        {
          $jointype = "I";

          ## remap right features based on distance to rightmost edge
          foreach my $f (@{$features{$right}})
          {
            $f->{end5} = $joinsize - ($rightsize - $f->{end5});
            $f->{end3} = $joinsize - ($rightsize - $f->{end3});

            $f->{end5g} = $joingsize - ($rightgsize - $f->{end5g});
            $f->{end3g} = $joingsize - ($rightgsize - $f->{end3g});

            push @{$features{$joinid}}, $f;
          }
        }
        else
        {
          $jointype = "II";

          ## remap right features based on distance to rightmost edge (reverse)

          foreach my $f (@{$features{$right}})
          {
            my $swap = $f->{end5};

            $f->{end5} = $joinsize - $f->{end3};
            $f->{end3} = $joinsize - $swap;

            $swap = $f->{end5g};

            $f->{end5g} = $joinsize - $f->{end3g};
            $f->{end3f} = $joinsize - $swap;

            push @{$features{$joinid}}, $f;
          }
        }
      }
      else
      {
        ## remap left features based on distance to rightmost edge
        foreach my $f (@{$features{$left}})
        {
          $f->{end5} = $joinsize - ($leftsize - $f->{end5});
          $f->{end3} = $joinsize - ($leftsize - $f->{end3});

          $f->{end5g} = $joingsize - ($leftgsize - $f->{end5g});
          $f->{end3g} = $joingsize - ($leftgsize - $f->{end3g});

          push @{$features{$joinid}}, $f;
        }

        if ($gapinfo->{right}->{oo} eq "EB")
        {
          $jointype = "III";

          ## Right Features are the same (EB-EB)
          push @{$features{$joinid}}, @{$features{$right}}
            if defined $features{$right};
        }
        else
        {
          $jointype = "IV";

          ##EB-BE
          foreach my $f (@{$features{$right}})
          {
            my $swap = $f->{end5};
            $f->{end5} = $rightsize - $f->{end3};
            $f->{end3} = $rightsize - $swap;

            $swap = $f->{end5g};
            $f->{end5g} = $rightsize - $f->{end3g};
            $f->{end3g} = $rightsize - $swap;

            push @{$features{$joinid}}, $f;
          }
        }
      }

      delete $features{$left};
      delete $features{$right};

      echo "Remapping $left and $right features into $joinid (Type $jointype)\n";
    }

    my $joinfeature = createJoinFeature($gapinfo);
    push @{$features{$joinid}}, $joinfeature;
  }
}



sub prepareAssembly
{
  my $prefix = shift;  ## dmg
  my $bankdir = shift; ## path/to/dmg.bnk

  echo "prepare assembly...\n";

  runCmd("$RM -rf $RESULTSDIR", "Warning: deleting old results directory ($RESULTSDIR)")
    if (-r $RESULTSDIR);

  tfmkdir($RESULTSDIR);

  runCmd("($DUMPREADS -e -r -c $bankdir > $prefix.seq) >& /dev/null", " dumpreads seq")
    if (! -r "$prefix.seq");

  runCmd("($DUMPREADS -e -q -r $bankdir > $prefix.qual) >& /dev/null", " dumpreads qual")
    if (! -r "$prefix.qual");

  foreach my $suffix (qw/seq qual/)
  {
    if (! -r "$prefix.$suffix.idx")
    {
      runCmd("$FILTERSEQ -index $prefix.$suffix",
             " Creating $suffix idx");
    }
  }

  runCmd("($BANK2CONTIG -e $bankdir > $prefix.contig) >& /dev/null", " bank2contig")
    if (! -r "$prefix.contig");

  runCmd("($BANK2SCAFF -e $bankdir > $prefix.scaff) >& /dev/null", " bank2scaff")
    if (! -r "$prefix.scaff");

  runCmd("($BANKREPORT -b $bankdir | $AMOS2MATES > $prefix.mates) >& /dev/null", " bank2mates")
    if (! -r "$prefix.mates");

  runCmd("($DUMPFEATURES $bankdir > $prefix.feat) >& /dev/null", " dumping original features")
    if (! -r "$prefix.feat");

  loadFeatures("$prefix.feat");

  loadLayout($prefix);

  if (! -r "fasta")
  {
    tfmkdir("fasta");
    
    my $cmd = "$CONTIG2FASTA $prefix.contig > fasta/$prefix.fasta";
    my $info = " contig2fasta $prefix";
    runCmd($cmd, $info);

    tfchdir("fasta");
    runCmd("$SPLITFASTA $prefix.fasta > /dev/null");
    tfchdir("..");
  }
    
  loadOrigContigSizes("fasta/$prefix.fasta");
}

{
  my %contigstatus;
  my %origcontiginfo;

  my %gapsizes;

  sub getExpectedGapSize
  {
    return ($gapsizes{$_[0]}->{size}, $gapsizes{$_[0]}->{stdev});
  }

  sub gapTooLarge
  {
    my $gapinfo = shift;

    my $gapsize = $gapinfo->{expectedgap};
    my $gapstdev  = $gapinfo->{gapstdev};

    if ($gapstdev < $MINGAPSTDEV) { $gapstdev = $MINGAPSTDEV; }

    ## Minimum size the gap may be as indicated by links
    my $mingapsize = $gapsize - ($gapstdev * $GAPSTDTHRESH); 
    if ($mingapsize < 0) { $mingapsize = 0; }

    my $retval = ($mingapsize >= $MAXGAPSIZE);

    if ($retval)
    {
      print "Gap too large: $mingapsize $gapsize $gapstdev\n";
    }

    return $retval;
  }

  sub loadLayout
  {
    my $prefix = shift;

    my $suffix = "contig";
    my $createlayout = 0;

    if (! -r "$prefix.layout")
    {
      open LAYOUT, "> $prefix.layout"
        or $tf->bail("Can't open $prefix.layout to create layout ($!)");

      $createlayout = 1;
    }
    else
    {
      $suffix = "layout";
    }

    open CONTIG, "< $prefix.$suffix"
      or $tf->bail("Can't open $prefix.$suffix to load layout ($!)");

    while (<CONTIG>)
    {
      if (/^\#\#(\S+) \d+ (\d+) bases/)
      {
        $origcontiginfo{$1}->{gsize} = $2;
      }

      if ($createlayout)
      {
        print LAYOUT $_ if (/^\#/);
      }
    }

    close CONTIG;
    close LAYOUT if ($createlayout);
  }

  sub loadOrigContigSizes
  {
    my $file = shift;
    my $cmd = "$SIZEFASTA $file";

    echo " loading original contig sizes\n";

    $tf->logLocal("Running $cmd", 4);
    my @out = `$cmd`;

    if ($?)
    {
      echo "failed!\n";
      $tf->bail("$cmd failed ($?)") if ($?);
    }

    foreach (@out)
    {
      my @vals = split;
      $origcontiginfo{$vals[0]}->{size} = $vals[1];
    }
  }

  sub addOrigContig
  {
    my $id = shift;
    my $oo = shift;
    my $contiglen = shift;
    my $gapsize   = shift;
    my $gapstdev = shift;

    $gapsizes{$id}->{size} = $gapsize;
    $gapsizes{$id}->{stdev} = $gapstdev;

    $contigstatus{$id}->{id} = $id;
    $contigstatus{$id}->{oo} = $oo;
    $contigstatus{$id}->{size}  = $origcontiginfo{$id}->{size};
    $contigstatus{$id}->{gsize} = $origcontiginfo{$id}->{gsize};

    $contigstatus{$id}->{comment} = "CA_CONTIG: $id"
      if (!exists $contigstatus{$id}->{comment});
  }

  sub printUnjoinedContigs
  {
    my $filename = shift;

    open UNJOINED, "> $filename"
      or $tf->bail("Can't open $filename");

    foreach my $id (keys %contigstatus)
    {
      if (!defined($contigstatus{$id}->{newid}) && 
          !defined($contigstatus{$id}->{isjoin}))
      {
        print UNJOINED "$id\n";
      }
    }

    close UNJOINED;
  }

  sub getContigInfo
  {
    my $id = shift;
    my $norecurse = shift;

    my $cs = $contigstatus{$id};

    if (! defined $norecurse)
    {
      while (defined $cs && defined $cs->{newid})
      {
        $cs = $contigstatus{$cs->{newid}};
      }
    }

    return $cs;
  }

  sub createJoinContigInfo
  {
    my $gapinfo = shift;

    my $joinid = $gapinfo->{autojoin}->{id};

    ## Add the joined contig to the contigstatus db
    $contigstatus{$joinid}->{id}     = $gapinfo->{autojoin}->{id};
    $contigstatus{$joinid}->{size}   = $gapinfo->{autojoin}->{size};
    $contigstatus{$joinid}->{gsize}  = $gapinfo->{autojoin}->{gsize};
    $contigstatus{$joinid}->{prefix} = $gapinfo->{autojoin}->{prefix};

    ## New contig has same orientation as left contig, right contig still has old oo
    $contigstatus{$joinid}->{oo}     = $gapinfo->{left}->{oo};

    $contigstatus{$joinid}->{isjoin} = 1;

    my $leftid  = $gapinfo->{left}->{id};
    my $rightid = $gapinfo->{right}->{id};

    ## Remember the join chain explicitly
    if (defined $contigstatus{$leftid}->{joinchain})
    {
      $contigstatus{$joinid}->{joinchain} = $contigstatus{$leftid}->{joinchain};
    }
    else
    {
      $contigstatus{$joinid}->{joinchain} = "$leftid";
    }

    $contigstatus{$joinid}->{joinchain} .= " $rightid";
  }
}

sub nucmerSeqs
{
  my $info    = shift;
  my $ref     = shift;
  my $query   = shift;
  my $options = shift;

  my $out  = shift;
  $out = "out" if (!defined $out);

  if (! -r "$out.delta")
  {
    my $cmd = "$NUCMER -p $out $options $ref $query 2> /dev/null";
    runCmd($cmd, $info);
  }

  runCmd("$SHOWCOORDS $out.delta > $out.coords")
    if (! -r "$out.coords");
}


sub parseGap
{
  my $stats = shift;
  my $prefix = shift;

  my $left  = shift;
  my $right = shift;

  $stats->{SequenceGaps}++;

  my $gapinfo;
  $gapinfo->{left}  = getContigInfo($left);
  $gapinfo->{right} = getContigInfo($right);
  $gapinfo->{iprefix} = $prefix;

  my $workdir = "alignment-$left-$right";
  $gapinfo->{workdir} = $workdir;

  my ($expectedgap, $gapstdev) = getExpectedGapSize($left);
  $gapinfo->{expectedgap} = $expectedgap;
  $gapinfo->{gapstdev} = $gapstdev;

  $gapinfo->{autojoin}->{status} = 0;

  echo "\n>$workdir $expectedgap \[$gapstdev\] $gapinfo->{left}->{id} $gapinfo->{right}->{id} $gapinfo->{left}->{oo}-$gapinfo->{right}->{oo}\n";

  return $gapinfo;
}

## Create a full sequence file (fseq) which contains the consensus and the
## full range sequence of those sequence which are within FSEQOVERLAP (10kb) 
## of the overlapping edge of the contig
sub createFseqFile
{
  my $id = shift;
  my $size = shift;
  my $side = shift;

  my $fseq = "$id.fseq";
  my $fseqorig = "$fseq.0";

  return if -r $fseq;

  my %edgesequences;

  my $suffix = "contig";
  $suffix = "layout" if (-r "$id.layout");

  open LAYOUT, "< $id.$suffix"
    or $tf->bail("Couldn't open $id.$suffix ($!)");

  my $savecount = 0;

  while (<LAYOUT>)
  {
    if (/^\#(\S+)\((\d+)\)/)
    {
      my $seqname = $1;
      my $offset = $2;
      my $len = $1 if /(\d+) bases/;

      my $left = $offset+0;
      my $right = $offset+$len;

      my $savesequence = 0;

      if ($side eq "B")
      {
        $savesequence = ($left < $FSEQOVERLAP);
      }
      else
      {
        $savesequence = (($size - $right) < $FSEQOVERLAP);
      }

      $edgesequences{$seqname} = 1 if ($savesequence);
      $savecount += $savesequence;
    }
  }
  close LAYOUT;

  ## It is possible that no sequences will be picked up, in the case of
  ## a NUU on the flanking edge
  if ($savecount)
  {
    open SEQ, "< $id.seq"
      or $tf->bail("Couldn't open $id.seq ($!)");

    open FSEQ, ">> $fseqorig"
      or $tf->bail("Couldn't open $id.fseq ($!)");

    my $printid = 0;

    while (<SEQ>)
    {
      $printid = exists $edgesequences{$1} if (/>(\S+)/);
      print FSEQ $_ if $printid;
    }

    close SEQ;
    close FSEQ;

    runCmd("$LOWCOMPLEXITYFILTER $fseqorig $fseq");
  }

  ## Copy the consensus last so we don't have to worry about running the
  ## complexity filter on it.

  runCmd("$CAT $id.fasta >> $fseq");
}


sub alignContigs
{
  my $gapinfo = shift;
  my $ajdir = shift;

  my $iprefix = $gapinfo->{iprefix};

  echo "prepare contigs for alignment... ";

  foreach my $type (qw/left right/)
  {
    my $id = $gapinfo->{$type}->{id};

    if (defined $gapinfo->{$type}->{prefix})
    {
      my $fprefix = $gapinfo->{$type}->{prefix};
      $fprefix = "$ajdir/$fprefix";

      foreach my $suffix (qw/contig fasta seq qual/)
      {
        if (! -r "$id.$suffix")
        {
          tfsymlink("$fprefix.$suffix", "$id.$suffix");
        }
      }
    }
    else
    {
      if (! -r "$id.fasta")
      {
        my $suffix = "seq";
        $suffix = "fasta" if (-r "$ajdir/fasta/$id.fasta");

        tfsymlink("$ajdir/fasta/$id.$suffix", "$id.fasta");
      }

      runCmd("$FILTERCONTIG $ajdir/$iprefix.layout $id > $id.layout")
        if (! -r "$id.layout");

      runCmd("$FILTERSEQ $id.layout $ajdir/$iprefix.seq > $id.seq")
        if (! -r "$id.seq");
    }

    $gapinfo->{$type}->{size} = getSize("$id.fasta")
      if !defined $gapinfo->{$type}->{size};
  }

  createFseqFile($gapinfo->{left}->{id}, 
                 $gapinfo->{left}->{size},
                 $gapinfo->{left}->{oo}  eq "BE" ? "E" : "B");

  createFseqFile($gapinfo->{right}->{id}, 
                 $gapinfo->{right}->{size},
                 $gapinfo->{right}->{oo} eq "BE" ? "B" : "E");

  echo "ok.\n";

  nucmerSeqs("align extended contigs", 
             "$gapinfo->{left}->{id}.fseq", 
             "$gapinfo->{right}->{id}.fseq", 
             "--maxmatch -l $MATCHLEN -c $CLUSTERLEN");
}

sub hasPotentialJoin
{
  my $gapinfo = shift;
  my $stats = shift;

  my $retval = 0;

  if (! -r $OVERLAPREPORT)
  {
    my $cmd = "$EVALOVERLAP $gapinfo->{left}->{oo} $gapinfo->{right}->{oo} out.coords ".
              "-gapsize $gapinfo->{expectedgap} -gapstdev $gapinfo->{gapstdev} ".
              "-alignthresh $ALIGNTHRESHOLD -debug 1 > $OVERLAPREPORT";
    runCmd($cmd, "evaluate sequence overlaps");
  }

  if (-r $OVERLAPREPORT)
  {
    my $cmd = undef;

    open EVAL, "< $OVERLAPREPORT"
      or $tf->bail("Can't open $OVERLAPREPORT ($!)");

    while (<EVAL>)
    {
      next if (!/^\@/);

      if (/Gap/)
      {
        echo $_;
      }
      elsif ((/^\@\=/) && (!defined $cmd))
      {
        echo $_;
        chomp;

        $cmd = $_;
        $cmd =~ s/^\@\=//;

        $stats->{Overlap}++;
        $gapinfo->{autojoin}->{cmd} = $cmd;
        $retval = 1;
      }
    }

    close EVAL;
  }

  return $retval;
}

sub loadJoinReport
{
  my $gapinfo = shift;
  my $joinreport = $gapinfo->{autojoin}->{report};

  open JOINREPORT, "< $joinreport"
    or $tf->bail("Can't open joinreport $joinreport ($!)");

  while (<JOINREPORT>)
  {
    chomp;
    my ($tag,$value) = split /=/, $_;

    if    ($tag eq "finalbegin") { $gapinfo->{autojoin}->{begin}     = $value; }
    elsif ($tag eq "finalend")   { $gapinfo->{autojoin}->{end}       = $value; }
    elsif ($tag eq "gapsize")    { $gapinfo->{autojoin}->{gapsize}   = $value; }
    elsif ($tag eq "idscore")    { $gapinfo->{autojoin}->{idscore}   = $value; }
    elsif ($tag eq "olen")       { $gapinfo->{autojoin}->{olen}      = $value; }
    elsif ($tag eq "extension")  { $gapinfo->{autojoin}->{extension} = $value; }
    elsif ($tag eq "rex")        { $gapinfo->{autojoin}->{rex}       = $value; }
    elsif ($tag eq "qex")        { $gapinfo->{autojoin}->{qex}       = $value; }
    elsif ($tag eq "joinid")     { $gapinfo->{autojoin}->{id}        = $value; }
  }

  close JOINREPORT;

#  foreach my $t (sort keys %{$gapinfo->{autojoin}}) { print "$t=$gapinfo->{autojoin}->{$t}\n"; }
}

sub joinContigs
{
  my $gapinfo = shift;
  my $stats = shift;
  my $ajdir = shift; ## ../..

  my $retval = 0;

  my $joinreport = "join.report";
  $gapinfo->{autojoin}->{report} = "$JOINDIR/$joinreport";

  my $joinid = getJoinId();

  if (! -r $gapinfo->{autojoin}->{report})
  {
    $retval = 1;

    if (! -r $JOINDIR)
    {
      ## Specify the data prefix, but there may be files to work on in the cwd
      $gapinfo->{autojoin}->{cmd} = "$JOINCONTIGS $gapinfo->{autojoin}->{cmd} ".
                                    "-dir $JOINDIR ".
                                    "-id $joinid ".
                                    "-data $ajdir/$gapinfo->{iprefix} ".
                                    "-report $joinreport";

#      echo "Joincmd: $gapinfo->{autojoin}->{cmd}\n";

      ## Join not attempted previously
      $retval = $tf->runCommand($gapinfo->{autojoin}->{cmd});
    }

    if ($retval)
    {
      my $reason = ($retval == 2) ? "Nucmer2" : "Unknown";
      echo "Status: Error! ($reason)\n";
      $stats->{Error}++;
    }
  }

  if (!$retval)
  {
    $stats->{Joined}++;
    loadJoinReport($gapinfo);

    ## Joinid may be different if we are using euids
    $joinid = $gapinfo->{autojoin}->{id};

    $gapinfo->{left}->{newid}  = $joinid;
    $gapinfo->{right}->{newid} = $joinid;

    $gapinfo->{autojoin}->{status} = 1;
    $gapinfo->{autojoin}->{size}   = getSize("$JOINDIR/$joinid.fasta");
    $gapinfo->{autojoin}->{gsize}  = getGappedSize("$JOINDIR/$joinid.contig");
    $gapinfo->{autojoin}->{prefix} = "$ALIGNMENTDIR/$gapinfo->{workdir}/$JOINDIR/$joinid";

    createJoinContigInfo($gapinfo);
  }

  return $retval;
}



sub countGoodAlignments
{
  my $coords     = shift;
  my $minsize    = shift;
  my $minpercent = shift;
  my $percidref  = shift;

  $$percidref = 0 if defined $percidref;

  my $good = 0;
  my $count = 0;

  open COORDS, "< $coords"
    or $tf->bail("Can't open $coords ($!)");

  my $inalignment = 0;

  while (<COORDS>)
  {
    if (!$inalignment) { $inalignment = 1 if (/=====/); }
    else
    {
      $count++;

      my @val = split /\s+/, $_;

      next if (($val[8] < $minsize) || ($val[10] < $minpercent));

      echo $_;
      $good++;

      ## Remember the max percent id of good matches
      $$percidref = $val[10] if (defined $percidref && $val[10] > $$percidref);
    }
  }
  close COORDS;

  return $good;
}


sub isValidJoinRegion
{
  my $project = shift;
  my $gapinfo = shift;
  my $percid  = shift;

  my $joinid = $gapinfo->{autojoin}->{id};

  my $rangebegin = $gapinfo->{autojoin}->{begin} - $RANGEWINDOW;
  $rangebegin = 1 if ($rangebegin <= 1);
  
  my $rangeend = $gapinfo->{autojoin}->{end} + $RANGEWINDOW;
  $rangeend = $gapinfo->{autojoin}->{size} if ($rangeend > $gapinfo->{autojoin}->{size});

  echo "Checking extended join region...\n";

  runCmd("echo '$joinid $rangebegin $rangeend' | $CUTFASTA $joinid.fasta > $joinid.join.fasta")
    if (! -r "$joinid.join.fasta");

  my $minpercent = 90;
  my $joinsize = getSize("$joinid.join.fasta");

  nucmerSeqs("nucmer cutfasta", "$project.1con", "$joinid.join.fasta", 
             "--nooptimize --maxmatch -c $CLUSTERLEN -l $MATCHLEN", "out.join");

  return (countGoodAlignments("out.join.coords", 
                              $joinsize, 
                              $minpercent,
                              $percid));
}


sub countValidOrigContigs
{
  my $project = shift;
  my $gapinfo = shift;

  ## Lets check to see if the original contigs had a match
  echo "Checking original contigs...\n";
  my $goodcount = 0;

  foreach my $type (qw/left right/)
  {
    my $id = $gapinfo->{$type}->{id};

    ## Check the pre-extension fasta
    tfsymlink("../../$id.fasta", "$id.fasta")
      if (! -r "$id.fasta");

    my $minpercent = 99;
    my $perclen = .90;

    nucmerSeqs("nucmer $id", "$project.1con", "$id.fasta", 
               "--maxmatch", "out.$id");

    $goodcount++ if (countGoodAlignments("out.$id.coords", 
                                         $gapinfo->{$type}->{size} * $perclen, 
                                         $minpercent));
  }

  return $goodcount;
}


sub validateJoin
{
  my $project = shift;
  my $gapinfo = shift;
  my $stats = shift;
  my $prefix = shift;

  my $joinid = $gapinfo->{autojoin}->{id};

  my $origsize = $gapinfo->{autojoin}->{size}; 

  echo "Checking join...\n";

  if (! -r $JOINDIR)
  {
    echo "No joindir!\n";
    return 0;
  }

  tfchdir($JOINDIR);

  tfmkdir("validate");
  tfchdir("validate");
  
  if ($DOAEVALIDATE)
  {
    if (! -r "$joinid.fasta")
    {
      if (! -r "autoEditor")
      {
        foreach my $suffix (qw/contig qual seq/)
        {
          tfsymlink("../$joinid.$suffix", "$joinid.$suffix")
            if (! -r "$joinid.$suffix");
        }
 
        my $aeproject = $project;
        $aeproject = "crypt" if ($aeproject =~ /crypt/);
        runCmd("autoEditor $joinid -d $aeproject -o autoEditor", "autoEditor join");
        runCmd("contig2fasta autoEditor/$joinid.contig > $joinid.fasta");
      }
    }

    $gapinfo->{autojoin}->{size} = getSize("$joinid.fasta");
 
    my $joinfeat = "$joinid.feat";
 
    my $oend5 = $gapinfo->{autojoin}->{begin};
    my $oend3 = $gapinfo->{autojoin}->{end};
 
    if (! -r $joinfeat)
    {
      open JOINFEAT, "> $joinfeat"
        or $tf->bail("Can't open $joinfeat");

      print JOINFEAT "Contig Id=\"$joinid\"\n";
      print JOINFEAT "Location End5=\"$oend5\" End5_gapped=\"$oend5\" End3=\"$oend3\" End3_gapped=\"$oend3\"\n";
      close JOINFEAT;
    }
 
    runCmd("$REMAPFEATURES autoEditor/$joinid.contigEdits $joinfeat -o autoEditor/$joinid.feat")
      if (! -r "autoEditor/$joinid.feat");
 
    open FEAT, "< autoEditor/$joinid.feat";
    while (<FEAT>)
    {
      next if ! /Location/;
      my $end3 = $1 if (/End3=\"(\d+)\"/);
      my $end5 = $1 if (/End5=\"(\d+)\"/);
 
      echo "Remapping $oend5->$end5, $oend3->$end3\n";
      $gapinfo->{autojoin}->{begin} = $end5;
      $gapinfo->{autojoin}->{end} = $end3;
 
      last;
    }
    close FEAT;
  }
  else
  {
    tfsymlink("../$joinid.fasta", "$joinid.fasta")
      if (! -r "$joinid.fasta");
  }

  ## Link the 1con
  if (! -r "$project.1con")
  {
    my $base;

    # WOULD LIKE TO REMOVE THESE BACK DOORS FROM PRODUCTION CODE -- Jason Dec 2004
    if ($project =~ /crypt(\d+)/)
    {
      $base = "/usr/local/db/euk/private/c_neoformans/whole_genome_sequencing/chr$1.final";
    }
    else
    {
      $base = `getdb -f 1con $project`;
    }

    symlink($base, "$project.1con");
  }

  my $retval = 1;
  my $minpercent = 99;

  nucmerSeqs("nucmer 1con", "$project.1con", "$joinid.fasta", 
             "--nooptimize --maxmatch"); 

  my $good = countGoodAlignments("out.coords", 
                                 $gapinfo->{autojoin}->{size}, 
                                 $minpercent);

  if ($good == 1)
  {
    $stats->{Valid}++;
    echo "Status: Valid\n";
  }

  {
    my $percid;
    if (isValidJoinRegion($project, $gapinfo, \$percid))
    {
      echo "Status: ValidJoin\n";
      $stats->{ValidJoin}++;
      push @{$stats->{validjoinid}}, $percid;
    }
    else
    {
      my $goodcount = countValidOrigContigs($project, $gapinfo);

      if ($goodcount == 0)
      {
        ## Neither orig was in the 1con
        echo "Status: Degenerate\n";
        $stats->{Degenerate}++;
      }
      elsif ($goodcount == 1)
      {
        ## One orig was in the 1con
        echo "Status: HalfDegenerate\n";
        $stats->{HalfDegenerate}++;
      }
      elsif ($goodcount == 2)
      {
        ## Both flanking contigs are present, but their join was not in the 1con
        echo "Status: Invalid!\n";
        $stats->{Invalid}++;

        $retval = 0;
      }
    }
  }

  tfchdir("..");
  tfchdir("..");

  $gapinfo->{autojoin}->{size} = $origsize;

  return $retval;
}




## Scaffold Management
{
  my %newscaffold;
  my $lastgap = undef;
  my $curscaffid = undef;

  sub addContigToScaffold
  {
    my $id = shift;
    my $gap = shift;
    my $gapstdev = shift;

    my $contig = getContigInfo($id);

    my $scaffcontig;
    $scaffcontig->{id}       = $contig->{id};
    $scaffcontig->{oo}       = $contig->{oo};
    $scaffcontig->{size}     = $contig->{size};
    $scaffcontig->{gap}      = $gap;
    $scaffcontig->{gapstdev} = $gapstdev;

    push @{$newscaffold{$curscaffid}}, $scaffcontig;
  }


  sub handleNewScaffold
  {
    my $prefix = shift;
    my $ajdir = shift;
    my $scaffid = shift;

    if (defined $lastgap)
    {
      ## End the previous scaffold, remember lastgap.right info. 
      ## If lastgap was undef then this was a singleton

      addJoinToResultsFile($lastgap, $ajdir, $prefix)
        if ($lastgap->{autojoin}->{status});

      ## Add lastgap.right to the scaffold (may or may not be join)
      addContigToScaffold($lastgap->{right}->{id}, 0, 0);
    }

    if (defined $scaffid)
    {
      $curscaffid = $scaffid;
      $newscaffold{$scaffid} = [];
    }

    $lastgap = undef;
  }

  sub handleGapChain
  {
    my $prefix = shift;
    my $ajdir = shift;
    my $gapinfo = shift;

    if (!$gapinfo->{autojoin}->{status})
    {
      ## The current gap is still open, and it is safe to remember left contig info

      if (defined $lastgap && $lastgap->{autojoin}->{status})
      {
        ## gapinfo.left contig was a join contig, 
        ## and now we know for sure that no more will be added
        addJoinToResultsFile($lastgap, $ajdir, $prefix);
      }

      ## Add gapinfo.left to the scaffold
      addContigToScaffold($gapinfo->{left}->{id}, 
                          $gapinfo->{expectedgap}, 
                          $gapinfo->{gapstdev});
    }

    $lastgap = $gapinfo;
  }

  ## For each contig that had a join, print which contigs went into that join,
  ## in their original scaffold order
  sub printJoinSummary
  {
    my $filename = shift;

    open SUMMARY, "> $filename"
      or $tf->bail("Can't open $filename ($!)");

    foreach my $scaffid (sort {$a <=> $b} keys %newscaffold)
    {
      foreach my $contig (@{$newscaffold{$scaffid}})
      {
        my $id = $contig->{id};
        my $info = getContigInfo($id);

        if ($info->{isjoin})
        {
          my $chain = $info->{joinchain};
          print SUMMARY "$id: $chain\n";
        }
      }
    }

    close SUMMARY;
  }

  sub printScaffold
  {
    my $filename = shift;

    open SCAFF, "> $filename"
      or $tf->bail("Can't open $filename ($!)");

    foreach my $scaffid (sort {$a <=> $b} keys %newscaffold)
    {
      my $scafflen = 0;
      my $scaffspan = 0;
      my $numctg = 0;

      ## Quick pass to compute size
      foreach my $contig (@{$newscaffold{$scaffid}})
      {
        $numctg++;
        $scafflen  += $contig->{size};
        $scaffspan += $contig->{size} + int($contig->{gap});
      }

      ## Print scaffold
      print SCAFF ">$scaffid $numctg $scafflen $scaffspan\n";
      foreach my $contig (@{$newscaffold{$scaffid}})
      {
        ## For consistency with sequencetiling (which requires integer offsets and gap sizes)
        my $gapsize = int($contig->{gap});
        print SCAFF "$contig->{id} $contig->{oo} $contig->{size} $gapsize $contig->{gapstdev}\n"
      }
    }

    close SCAFF;
  }
}


## Stats Management
{
  sub initializeStats
  {
    my $stats;

    foreach my $key (qw/SequenceGaps Overlap Valid Invalid 
                        Degenerate HalfDegenerate 
                        Error ValidJoin Joined 
                        DoubleExtend SingleExtend NoExtend/)
    {
      $stats->{$key} = 0;
    }

    $stats->{expectedgap} = [];
    $stats->{actualgap}   = [];
    $stats->{idscore}     = [];
    $stats->{olen}        = [];
    $stats->{extension}   = [];
    $stats->{validjoinid} = [];

    return $stats;
  }

  sub updateJoinStatistics
  {
    my $gapinfo = shift;
    my $stats = shift;

    push @{$stats->{expectedgap}}, $gapinfo->{expectedgap};
    push @{$stats->{actualgap}},   $gapinfo->{autojoin}->{gapsize};
    push @{$stats->{idscore}},     $gapinfo->{autojoin}->{idscore};
    push @{$stats->{olen}},        $gapinfo->{autojoin}->{olen};
    push @{$stats->{extension}},   $gapinfo->{autojoin}->{extension};

    my $rex = ($gapinfo->{autojoin}->{rex} > 0);
    my $qex = ($gapinfo->{autojoin}->{qex} > 0);

    if    ($rex && $qex) { $stats->{DoubleExtend}++; }
    elsif ($rex || $qex) { $stats->{SingleExtend}++; }
    else                 { $stats->{NoExtend}++;     }
  }


  sub median
  {
    my $num = scalar @_;

    return 0 if $num==0;

    if ($num % 2)
    {
      ## Odd number of elements, just the middle
      return sprintf("%.02f", $_[int($num/2)]);
    }
    else
    {
      ## Even number, return the average of the middles
      return sprintf("%.02f", ($_[int(($num/2)-1)] + $_[int($num/2)]) / 2);
    }
  }

  sub medianabsolutedeviation
  {
    my $median = median(@_);
    my @absdiff = sort {$a <=> $b} 
                  map {abs($_ - $median)} @_;

    my $madconstant = 1.4826; ## From R
    return sprintf("%.02f", $madconstant * median(@absdiff));
  }

  sub mean
  {
    my $sum = 0;
    my $count = 0;

    foreach my $i (@_) { $sum += $i; $count++; }
    return sprintf("%.02f", ($count) ? $sum /$count : 0);
  }

  sub standarddeviation
  {
    my $mean = mean(@_);

    my $sum = 0;
    my $count = 0;

    foreach my $i (@_)
    {
      $sum += (($i - $mean)**2);
      $count++;
    }

    $sum = ($count>1) ? $sum / ($count-1) : 0;
    $sum = sqrt($sum);

    return sprintf("%.02f", $sum);
  }

  sub range
  {
    return "0,0" if !defined $_[0];
    my $last = scalar(@_) -1;
    return "$_[0],$_[$last]";
  }

  sub printArrayStats
  {
    my $prefix = shift;
    my $array = shift;

    my @sorted = sort {$a <=> $b} @{$array};

    my $Range  = range(@sorted);
    my $Mean   = mean(@sorted);
    my $SD     = standarddeviation(@sorted);
    my $Median = median(@sorted);
    my $Mad    = medianabsolutedeviation(@sorted);

    print "$prefix.Raw=", join(",", @{$array}), "\n" if $DOVALIDATE;
    print "$prefix.Range=$Range\n";
    print "$prefix.Mean=$Mean\n";
    print "$prefix.SD=$SD\n";
    print "$prefix.Median=$Median\n";
    print "$prefix.Mad=$Mad\n";
  }


  sub printStats
  {
    my $prefix = shift;
    my $stats = shift;

    my $PercJoin       = sprintf("%.03f", ($stats->{SequenceGaps}) ? $stats->{Joined}/$stats->{SequenceGaps} : 0);
    my $PercInvalid    = sprintf("%.03f", ($stats->{Joined}) ? $stats->{Invalid}/$stats->{Joined} : 0);

    my $title = "autoJoin";
    $title = "$prefix\_$title" if $DOVALIDATE;

    print "[$title]\n";
    print "NumSequenceGaps=$stats->{SequenceGaps}\n";
    print "NumJoined=$stats->{Joined}\n";
    print "NumDoubleExtend=$stats->{DoubleExtend}\n";
    print "NumSingleExtend=$stats->{SingleExtend}\n";
    print "NumNoExtend=$stats->{NoExtend}\n";

    print "PercJoin=$PercJoin\n";

    if ($DOVALIDATE)
    {
      print "NumOverlap=$stats->{Overlap}\n";
      print "NumJoinError=$stats->{Error}\n";
      print "PercInvalidJoin=$PercInvalid\n";
      print "NumInvalid=$stats->{Invalid}\n";
      print "NumVerified=$stats->{Valid}\n";
      print "NumJoinVerified=$stats->{ValidJoin}\n";
      print "NumDegenerate=$stats->{Degenerate}\n";
      print "NumHalfDegenerate=$stats->{HalfDegenerate}\n";
    }

    printArrayStats("ExpectedGap", $stats->{expectedgap});
    printArrayStats("ActualGap",   $stats->{actualgap});

    if ($DOVALIDATE)
    {
      printArrayStats("IDScore",     $stats->{idscore});
      printArrayStats("Olen",        $stats->{olen});
      printArrayStats("Extend",      $stats->{extension});
      printArrayStats("JoinID",      $stats->{validjoinid});
    }

    print "\n";
  }
}

sub addJoinToResultsFile
{
  my $gapinfo = shift;
  my $ajdir = shift;
  my $prefix = shift;

  if ($gapinfo->{autojoin}->{status})
  {
    echo "save $gapinfo->{autojoin}->{id} results... ";

    foreach my $suffix (qw/contig qual seq/)
    {
      runCmd("$CAT $ajdir/$gapinfo->{autojoin}->{prefix}.$suffix >> $ajdir/$RESULTSDIR/$prefix$JOINSUFFIX.$suffix");
    }

    echo "ok";
  }
}

sub finalizeAssembly
{
  my $prefix = shift;
  my $ajdir = shift;
  my $stats = shift;
  my $db = shift;

  echo "\n\nFinalizing assembly\n";

  tfchdir("$ajdir/$RESULTSDIR"); $ajdir = File::Spec->abs2rel($ajdir);

  my %joinsequences;
  my $printid = 0;

  open ORIGCONTIG, "< $ajdir/$prefix.contig"
    or $tf->bail("Can't open orig contig ($ajdir/$prefix.contig) ($!)");

  open ALLCONTIG, "> $prefix.contig"
    or $tf->bail("Can't open $prefix.contig ($!)");

  echo " creating results contig file... ";

  while (<ORIGCONTIG>)
  {
    if (/^##(\S+)\s/)
    {
      my $contigid = $1;
      my $info = getContigInfo($contigid, 1);

      $printid = !defined $info->{newid};
    }
    elsif (/^#(\S+)\(/)
    {
      $joinsequences{$1} = 1 if (!$printid);
    }

    print ALLCONTIG $_ if $printid;
  }

  close ORIGCONTIG;
  close ALLCONTIG;

  echo "ok.\n";

  runCmd("$CAT $prefix$JOINSUFFIX.contig >> $prefix.contig", " concat contig")
    if -r "$prefix$JOINSUFFIX.contig";

  foreach my $suffix (qw/qual seq/)
  {
    open FASTA, "< $ajdir/$prefix.$suffix"
      or $tf->bail("Can't open orig $suffix ($ajdir/$prefix.$suffix) ($!)");

    open OUT, ">$prefix.$suffix"
      or $tf->bail("Can't open $prefix.$suffix ($!)");

    $printid = 0;

    while (<FASTA>)
    {
      $printid = !exists $joinsequences{$1} if (/^>(\S+)/);
      print OUT $_ if $printid;
    }

    close FASTA;
    close OUT;

    runCmd("$CAT $prefix$JOINSUFFIX.$suffix >> $prefix.$suffix", " concat $suffix")
      if -r "$prefix$JOINSUFFIX.$suffix";
  }

  printFeatures("$prefix.feat");
  printUnjoinedContigs("$prefix.unjoined");
  printScaffold("$prefix.scaff");
  printJoinSummary("$prefix.joinsummary");

  ## Create AMOS Output
  runCmd("(($TOAMOS -m ../$prefix.mates -c $prefix.contig -s $prefix.seq -q $prefix.qual -o - | $BANKTRANSACT -m - -b $prefix.bnk -c) > /dev/null) >& /dev/null", " creating bank");
  runCmd("(($AJSCAFF2AMOS $prefix.scaff $prefix.bnk/CTG.map | $BANKTRANSACT -m - -b $prefix.bnk) > /dev/null) >& /dev/null", "  loading updated scaffold");
  runCmd("(($LOADFEATURES $prefix.bnk $prefix.feat) > /dev/null ) >& /dev/null", "  loading features");

  open REPORT, "> $prefix.joinreport"
    or $tf->bail("Can't open $prefix.joinreport");

  select REPORT;
  printStats($prefix, $stats);

  select STDOUT;
  close REPORT;

  echo "\n";
  printStats($prefix, $stats);
}


MAIN:
{
  my $db = undef;

  my $ajdir = "autoJoin";
  my $curajpath;

  $tf->addDependInfo(@DEPENDS);
  $tf->setHelpInfo($HELPTEXT);
  $tf->setVersionInfo($VERSION);
  $tf->setLogFile(getcwd()."/autoJoin.log"); ## Don't follow chdir

  # now we handle the input options
  my $result = $tf->TIGR_GetOptions
               (
                 'D=s',         \$db,
                 "prefix=s",    \$JOINPREFIX,
                 'o|dir=s',     \$ajdir,
                 'verbose!',    \$DOVERBOSE,
                 'validate!',   \$DOVALIDATE,
                 'join!',       \$DOJOIN,
                 'aevalidate!', \$DOAEVALIDATE,
                 'alignment=s', \$ALIGNTHRESHOLD,
                );

  $tf->bail("Command line parsing failed") if (!$result);

  if ($ALIGNTHRESHOLD < 75.00 || $ALIGNTHRESHOLD > 100.00)
  {
    $tf->bail("Alignment threshold must be between 75.0 and 100.0 percent");
  }

  my $bankpath = shift @ARGV;  ## path/to/dmg.bnk
  die $USAGE if !defined $bankpath;
  $bankpath =~ s/\/$//;

  my $debug = $tf->getDebugLevel();
  $tf->setDebugLevel(4) if (!$debug);

  my $prefix = basename($bankpath);  ## dmg.bnk
  $prefix =~ s/\.bnk//;              ## dmg

  $bankpath = File::Spec->rel2abs($bankpath);

  $tf->logLocal("prefix: $prefix\n", 4);
  $tf->logLocal("bankpath: $bankpath\n", 4);

  tfmkdir($ajdir); tfchdir($ajdir); $ajdir = cwd; 

  prepareAssembly($prefix, $bankpath);

  my $scafffile = "$prefix.scaff";

  open SCAFFFILE, "< $scafffile"
    or $tf->bail("Can't open scaff file $scafffile for reading ($!)");

  tfmkdir($ALIGNMENTDIR); tfchdir($ALIGNMENTDIR);

  my $stats = initializeStats();

  my $leftcontig = undef;
  my $issingleton = 0;

  while (<SCAFFFILE>)
  {
    if (/^\>(\S+)/) 
    { 
      ## In the start of a scaffold

      if ($issingleton && defined $leftcontig)
      {
        # The last scaffold was a singleton
        addContigToScaffold($leftcontig, 0, 0)
      }

      handleNewScaffold($prefix, File::Spec->abs2rel($ajdir), $1); 

      $leftcontig = undef;
      $issingleton = 1;
    }
    else
    {
      chomp;
      ## Notice the gap between contigs

      my @vals = split /\s+/, $_;

      my $gapstdev  = $DEFAULT_STDEV;

      my $contigid  = $vals[0];
      my $oo        = $vals[1];
      my $contiglen = $vals[2];
      my $gapsize   = $vals[3];

      $gapstdev = $vals[4] if defined $vals[4];

      addOrigContig($contigid, $oo, $contiglen, $gapsize, $gapstdev);

      if (!defined $leftcontig) { $leftcontig = $contigid; }
      else
      {
        $issingleton = 0;
        my $gapinfo = parseGap($stats, $prefix, $leftcontig, $contigid);

        if (!gapTooLarge($gapinfo))
        {
          tfmkdir($gapinfo->{workdir});
          tfchdir($gapinfo->{workdir});
          $curajpath = File::Spec->abs2rel($ajdir);

          alignContigs($gapinfo, $curajpath);
          if (hasPotentialJoin($gapinfo, $stats) && $DOJOIN)
          {
            if (!joinContigs($gapinfo, $stats, $curajpath))
            {
              ## The join was successful
              remapJoinedFeatures($gapinfo);
              updateJoinStatistics($gapinfo, $stats);

              if (defined $db && $DOVALIDATE)
              {
                validateJoin($db, $gapinfo, $stats, $prefix)
              }
            }
          }

          tfchdir("..");
        }

        handleGapChain($prefix, File::Spec->abs2rel($ajdir), $gapinfo);

        ## The rightcontig becomes the leftcontig for the next gap
        $leftcontig = $contigid;
      }
    }
  }

  ## Make sure the end of the gap file is noticed
  addContigToScaffold($leftcontig, 0, 0)
    if ($issingleton && defined $leftcontig);

  handleNewScaffold($prefix, File::Spec->abs2rel($ajdir), undef);

  finalizeAssembly($prefix, $ajdir, $stats, $db);
}

