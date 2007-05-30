#!/usr/bin/perl -w

use strict;
use TIGR::Foundation;

my $USAGE = "Usage: aj_evaluateSequenceOverlaps.pl roo qoo out.coords\n";

my $HELPTEXT = qq~
Evaluate alignments to find the best candidate for autoJoining

   $USAGE
   roo is the reference orientation
   qoo is the query orientation
   out.coords contains the nucmer results

   Options
   -------
   -gapsize <size>     Expected gap size
   -gapstdev <stdev>   Standard deviation of gap size
   -alignthresh <perc> Minimum acceptable alignment percent id
~;

my $VERSION = " Version 1.01 (Build " . (qw/$Revision$/ )[1] . ")";
my $SIZEFASTA = "/fs/sz-user-supported/common/bin/sizeFasta";

my @DEPENDS = 
(
  "TIGR::Foundation",
  $SIZEFASTA,
);

my $tf = new TIGR::Foundation;


my $ALIGNTHRESH = 94.00;
my $GAPSIZE_PARTITION = 20;
my $MAXOVERHANG = 10000;
my $MAXGAPSIZE  = 10000;
my $MINIDSCORE = 85.00;
my $MINMATCHLEN = 40;
my $MINOVERLAP = 30;

my $WORKDIR = ".";

my $doverbose = 0;
my $PRINTALL = 0;

my %qcontig;
my %rcontig;
my %qseqs;
my %rseqs;

my %rlayout;
my %qlayout;
my $layoutcount = 0;

sub echo
{
  my $str = join " ", @_;

  $tf->logLocal($str, 4);
  print @_ if ($doverbose);
}

sub getSize
{
  my $seq = shift;
  my $id = shift;

  my $cmd = "$SIZEFASTA -i $id $seq";

  $tf->logLocal("Running $cmd", 4);
  my $out = `$cmd`;

  $tf->bail("$cmd failed ($!)") if ($?);

  return (split /\s+/, $out)[1];
}

sub loadSeqSizes
{
  my $file = shift;
  my $retval = {};

  my $cmd = "$SIZEFASTA $file";

  $tf->logLocal("Running $cmd", 4);
  my @out = `$cmd`;

  $tf->bail("$cmd failed ($!)") if ($?);

  foreach (@out)
  {
    my @vals = split;
    $retval->{$vals[0]} = $vals[1];
  }

  return $retval;
}

sub loadLayout
{
  my $cref = shift;
  my $layout = shift;
  my $layoutfile = shift;

  my $info = undef;

  open LAYOUT, "< $layoutfile"
    or $tf->bail("Can't open $layoutfile ($!)");

  while (<LAYOUT>)
  {
    if (/^##(\S+)/)
    {
      $cref->{name} = $1;
      $info = $_;
    }
    elsif (/^#(\S+)\(/)
    {
      my $seqname = $1;
      chomp;

      $layout->{$seqname} = $_;
    }
  }

  close LAYOUT;

  $tf->bail("Contig $layoutfile malformed") if !defined $info;

  return $info;
}

sub getContigInfo
{
  my ($cref, $layout, $type, $orientation) = @_;

  my $info;

  if ( -r "$WORKDIR/$cref->{prefix}.layout")
  {
    $info = loadLayout($cref, $layout, "$WORKDIR/$cref->{prefix}.layout");
  }
  else
  {
    $info = loadLayout($cref, $layout, "$WORKDIR/$cref->{prefix}.contig");
  }

  chomp $info;
  echo $info;

  my @values = split /\s/, $info;
  $cref->{bases} = $values[2];
  $cref->{info} = $info;

  echo " $orientation\n";
  $orientation = ($orientation eq "BE") ? 0 : 1;
  $cref->{oo} = $orientation;

  my $len = getSize("$WORKDIR/$cref->{prefix}.fasta", $cref->{prefix});
  $cref->{len} = $len;
    
  my $overlapside = ($orientation) ?  ($type eq "r") ? "B" : "E"
                                   :  ($type eq "r") ? "E" : "B";

  $cref->{overlapside} = $overlapside;

  $cref->{seqsizes} = loadSeqSizes("$WORKDIR/$cref->{prefix}.seq");
}



sub getReadInfo
{
  my ($cref, $layout, $seqname) = @_;
  my $retval;

  $retval->{seqname} = $seqname;
  $retval->{contig}  = $cref;

  if ($seqname eq $cref->{name})
  {
    ## This alignment is with the consensus
    $retval->{cons}   = 1;

    $retval->{offset} = 0;
    $retval->{rc}     = 0;
    $retval->{len}    = $cref->{len};
    $retval->{size}   = $cref->{len};

    $retval->{seql}   = 1;
    $retval->{asml}   = 1;

    $retval->{seqr}   = $cref->{len};
    $retval->{asmr}   = $cref->{len};

    $retval->{seqmax} = $cref->{len};
  }
  else
  {
    $retval->{cons} = 0;

    $tf->bail("Sequence $seqname not in layout") 
      if !exists $layout->{$seqname};

    my $info = $layout->{$seqname};

    $tf->bail("Sequence $seqname not in seq file") 
      if !exists $cref->{seqsizes}->{$seqname};

    my $size = $cref->{seqsizes}->{$seqname};

    my $offset = $1 if ($info =~ /^\#\S+\((\d+)\)/);
    my $rc = ($info =~ /\[RC\]/);
    my $len = $1 if ($info =~ /(\d+) bases/);

    my ($seql, $seqr, $asml, $asmr);

    if ($info =~ /{(\S+) (\S+)}/) { $seql = $1; $seqr = $2; }
    if ($info =~ /<(\S+) (\S+)>/) { $asml = $1; $asmr = $2; }

    $retval->{offset} = $offset;
    $retval->{rc}     = $rc ? "1" : "0";
    $retval->{len}    = $len;
    $retval->{size}   = $size;

    $retval->{seql}   = $seql;
    $retval->{seqr}   = $seqr;

    $retval->{asml}   = $asml;
    $retval->{asmr}   = $asmr;

    $retval->{seqmax} = ($rc) ? $seql : $seqr;
  }

  ## Tedge is the projected sequence position of the overlaping contig edge
  my $tedge;

  if ($cref->{overlapside} eq "B")
  {
    if ($retval->{rc}) { $tedge = $retval->{asml} + $retval->{seql}; }
    else               { $tedge = $retval->{seql} - $retval->{asml}; }
  }
  else
  {
    if ($retval->{rc}) { $tedge = $retval->{seqr} - ($cref->{len} - $retval->{asmr}); }
    else               { $tedge = $retval->{seqr} + ($cref->{len} - $retval->{asmr}); }
  }

  $retval->{tedge} = $tedge;
  $retval->{dir} = $cref->{oo} ^ $retval->{rc};

  return $retval;
}

sub getReadRef
{
  my ($seq, $seqref, $contigref, $layout) = @_;

  my %seqs = %$seqref;

  my $retval;

  if (exists($seqs{$seq}))
  {
    $retval = $seqs{$seq};
  }
  else
  {
    $retval = getReadInfo($contigref, $layout, $seq);
    $seqref->{$seq} = $retval; ## Save it away for fast lookup
  }

  return $retval;
}

sub parseOverlap
{
  my ($id, $alignment) = @_;

  my @values = split /\s+/, $alignment;

  my $rseq = $values[12];
  my $qseq = $values[13];

  my $rlen = $values[7];
  my $qlen = $values[8];

  my $idscore = $values[10];
  $idscore =~ s/%//g;

  my $o;
  $o->{id} = $id;
  $o->{dist} = ($qlen < $rlen) ? $qlen : $rlen;
  $o->{idscore} = $idscore;
  $o->{iddist} = sprintf("%.02f", ($o->{dist} * $idscore /100));

  $o->{rref}    = getReadRef($rseq, \%rseqs, \%rcontig, \%rlayout);
  $o->{rstart}  = $values[1];
  $o->{rend}    = $values[2];
  $o->{rrc}     = 0;  ## nucmer always aligns with reference forward
  $o->{rmax}    = $o->{rend};
  $o->{rasml}   = "?";
  $o->{rasmr}   = "?";

  $o->{qref}    = getReadRef($qseq, \%qseqs, \%qcontig, \%qlayout);
  $o->{qstart}  = $values[4];
  $o->{qend}    = $values[5];
  $o->{qrc}     = ($o->{qstart} > $o->{qend}) ? 1 : 0;
  $o->{qmax}    = ($o->{qrc}) ? $o->{qstart} : $o->{qend};
  $o->{qasml}   = "?";
  $o->{qasmr}   = "?";

  ## rrc and qrc mark in the sequence alignment is reversed
  ## rdir and qdir mark if the implied contig direction is reversed
  $o->{rdir} = ($o->{rref}->{rc}); ## rrc == 0
  $o->{qdir} = ($o->{qref}->{rc} ^ $o->{qrc});

  $o->{cons} = $o->{rref}->{cons} + $o->{qref}->{cons};
  
  my @consdisplay = ("","+","*");
  $o->{consd} = $consdisplay[$o->{cons}];

  return $o;
}


sub printOverlap
{
  my $o = shift;

  my $rref = $o->{rref};
  my $qref = $o->{qref};

  my $rrc = ($rref->{rc}) ? "RC" : "";
  my $qrc = ($qref->{rc}) ? "RC" : "";

  my $roo = $rref->{contig}->{oo} ? "EB" : "BE";
  my $qoo = $qref->{contig}->{oo} ? "EB" : "BE";

  my $rcons = ($rref->{cons}) ? "#" : "";
  my $qcons = ($qref->{cons}) ? "#" : "";

  print "$o->{id}: $rref->{seqname} <-> $qref->{seqname} ".
        "$o->{idscore}% $o->{dist}bp\n";

  print "#$rcons$rref->{seqname}($rref->{offset}) [$rrc] $rref->{len}. <$rref->{asml} $rref->{asmr}> {$rref->{seql} $rref->{seqr}} ($o->{rstart} $o->{rend}) $rref->{tedge} |$o->{rasml} $o->{rasmr}|\n";
  print "#$qcons$qref->{seqname}($qref->{offset}) [$qrc] $qref->{len}. <$qref->{asml} $qref->{asmr}> {$qref->{seql} $qref->{seqr}} ($o->{qstart} $o->{qend}) $qref->{tedge} |$o->{qasml} $o->{qasmr}|\n";
  print "\n";

  foreach my $ref ($rref, $qref)
  {
    my $type = $ref eq $rref ? "r" : "q";
    my $rc = $ref->{rc};

    my $unit = 50;

    my $tedge = $ref->{tedge};


    my %display = ("a" => ["(",")"],
                   "t" => [$ref->{contig}->{overlapside}, "+"]);

    my %values = ("astart" => $o->{$type."start"},
                  "aend"   => $o->{$type."end"});

    if ($ref->{cons})
    {
      $values{"fstart"} = 1;
      $values{"fend"}   = $ref->{size};

      $values{"tstart"} = $tedge;

      $display{"f"} = ["[","]"];
      $unit = $ref->{len} / 40;
    }
    else
    {
      ## Cap the distance to edge by about 1 readlen
      $tedge = 2000  if ($tedge > 2000);
      $tedge = -1000 if ($tedge < -1000);

      $display{"c"} = ["{","}"];
      $display{"s"} = ["|=","=>"];

      $values{"cstart"} = $ref->{seql};
      $values{"cend"}   = $ref->{seqr};

      $values{"sstart"} = 1;
      $values{"send"}   = $ref->{size};

      $values{"tstart"} = $tedge;
    }

    my $pos = undef;

    foreach my $key (sort {$values{$ref->{dir} ? $b : $a} <=> 
                           $values{$ref->{dir} ? $a : $b}}
                        keys %values)
    {
      my $char = $1 if ($key =~ /^(\S)/);

      if (defined $pos)
      {
        my $space = int(abs($values{$key} - $pos) / $unit);
        print " "x$space;
      }

      $pos = $values{$key};

      if (!defined $display{$char}->[2])
      {
        $display{$char}->[2] = 1;

        if    ($key eq "sstart") { print "|="; }
        elsif ($key eq "send")   { print "<=" }
        else                     { print $display{$char}->[0]; }
      }
      else
      {
        if    ($key eq "sstart") { print "=|"; }
        elsif ($key eq "send")   { print "=>" }
        else                     { print $display{$char}->[1]; }
      }
    }

    print "\n";
  }
}


sub evaluateOverlap
{
  my ($o) = @_;

  my $rref = $o->{rref};
  my $qref = $o->{qref};

  echo "Eval:";
  my $bad = "!\n\n";

  echo " id";
  my $idscore = $o->{idscore};
  $idscore =~ s/[^\d\.]//g;
  if (0 && $idscore < $MINIDSCORE)
  {
    echo $bad; return 0;
  }

  echo " len";
  if (0 && $o->{iddist} < $MINMATCHLEN)
  {
    echo $bad; return 0;
  }

  echo " dir";
  if (($rref->{contig}->{oo} ^ $qref->{contig}->{oo}) ^ 
      ($o->{rdir} ^ $o->{qdir})) 
  {
    echo $bad; return 0;
  }

  my ($rhang, $qhang);   ## Amount of hanging consensus created by alignment
  my ($rdist, $qdist);   ## Gap distance measures
  my ($r3, $q3);         ## Alignment requires 3' extension (bad)
  my ($r5, $q5);         ## Alignment requires 5' extension
  my ($rex, $qex);       ## Alignment requires contig extension

  my $d = 0;
  my $cpos = 0;

  my ($rasml, $rasmr);
  my ($qasml, $qasmr);

  echo " t";
  if (!$rref->{contig}->{oo})
  {
    if (!$rref->{rc})
    {
      echo "1";

      $d = $rref->{seqr} - $o->{rend};
      $rasmr = $rref->{asmr} - $d;

      $d = $rref->{seqr} - $o->{rstart};
      $rasml = $rref->{asmr} - $d;

      $rhang = $rref->{contig}->{len} - $rasmr;
      $rdist = -$rhang;

      $r3  = ($o->{rend} - $MINOVERLAP) < $rref->{seql};
      $r5  = ($o->{rstart} + $MINOVERLAP) > $rref->{seqr};
      $rex = ($o->{rstart} + $MINOVERLAP) > $rref->{tedge};
    }
    else
    {
      echo "2";

      $d = $o->{rstart} - $rref->{seqr};
      $rasmr = $rref->{asmr} - $d;

      $d = $o->{rend} - $rref->{seqr};
      $rasml = $rref->{asmr} - $d;

      $rhang = $rref->{contig}->{len} - $rasmr;
      $rdist = -$rhang;

      $r3  = ($o->{rend} - $MINOVERLAP) < $rref->{seqr};
      $r5  = ($o->{rstart} + $MINOVERLAP) > $rref->{seql};
      $rex = 0;
    } 
  }
  else
  {
    if (!$rref->{rc})
    {
      echo "3";

      $d = $o->{rstart} - $rref->{seql};
      $rasml = $rref->{asml} + $d;

      $d = $o->{rend} - $rref->{seql};
      $rasmr = $rref->{asml} + $d;

      $rhang = $rasml;
      $rdist = -$rhang;

      $r3  = ($o->{rend} - $MINOVERLAP) < $rref->{seql};
      $r5  = ($o->{rstart} + $MINOVERLAP) > $rref->{seqr};
      $rex = 0;
    }
    else
    {
      echo "4";

      $d = $rref->{seql} - $o->{rend};
      $rasml = $rref->{asml} + $d;

      $d = $rref->{seql} - $o->{rstart};
      $rasmr = $rref->{asml} + $d;

      $rhang = $rasml;
      $rdist = -$rhang;

      $r3  = ($o->{rend} - $MINOVERLAP) < $rref->{seqr};
      $r5  = ($o->{rstart} + $MINOVERLAP) > $rref->{seql};
      $rex = ($o->{rstart} + $MINOVERLAP) > $rref->{tedge};
    } 
  }


  if (!$qref->{contig}->{oo})
  {
    if (!$o->{qrc})
    {
      if (!$qref->{rc})
      {
        echo "5";

        $d = $o->{qstart} - $qref->{seql};
        $qasml = $qref->{asml} + $d;
        $qhang = $qasml;

        $d = $o->{qend} - $qref->{seql};
        $qasmr = $qref->{asml} + $d;
        $qdist = -$qasmr;

        $q3  = ($o->{qend} - $MINOVERLAP) < $qref->{seql};
        $q5  = ($o->{qstart} + $MINOVERLAP) > $qref->{seqr};
        $qex = 0;
      }
      else
      {
        echo "6";

        $d = $qref->{seql} - $o->{qend};
        $qasml = $qref->{asml} + $d;
        $qhang = $qasml;

        $d = $qref->{seql} - $o->{qstart};
        $qasmr = $qref->{asml} + $d;
        $qdist = -$qasmr;

        $q3  = ($o->{qend} - $MINOVERLAP) < $qref->{seqr};
        $q5  = ($o->{qend} + $MINOVERLAP) > $qref->{seql};
        $qex = ($o->{qend} + $MINOVERLAP) > $qref->{tedge};
      }
    }
    else
    {
      if (!$qref->{rc})
      {
        echo "7";

        $d = $o->{qend} - $qref->{seql};
        $qasml = $qref->{asml} + $d;
        $qhang = $qasml;

        $d = $o->{qstart} - $qref->{seql};
        $qasmr = $qref->{asml} + $d;
        $qdist = -$qasmr;

        $q3  = ($o->{qstart} - $MINOVERLAP) < $qref->{seql};
        $q5  = ($o->{qend} + $MINOVERLAP) > $qref->{seqr};
        $qex = 0;
      }
      else
      {
        echo "8";

        $d = $qref->{seql} - $o->{qstart};
        $qasml = $qref->{asml} + $d;
        $qhang = $qasml;

        $d = $qref->{seql} - $o->{qend};
        $qasmr = $qref->{asml} + $d;
        $qdist = -$qasmr;

        $q3  = ($o->{qstart} - $MINOVERLAP) < $qref->{seqr};
        $q5  = ($o->{qend} + $MINOVERLAP) > $qref->{seql};
        $qex = ($o->{qend} + $MINOVERLAP) > $qref->{tedge};
      }
    } 
  }
  else
  {
    if (!$o->{qrc})
    {
      if (!$qref->{rc})
      {
        echo "9";
        
        $d = $qref->{seqr} - $o->{qend};
        $qasmr = $qref->{asmr} - $d;
        $qhang = $qref->{contig}->{len} - $qasmr;
        
        $d = $qref->{seqr} - $o->{qstart};
        $qasml = $qref->{asmr} - $d;
        $qdist = $qasml - $qref->{contig}->{len};

        $q3  = ($o->{qend} - $MINOVERLAP) < $qref->{seql};
        $q5  = ($o->{qstart} + $MINOVERLAP) > $qref->{seqr};
        $qex = ($o->{qstart} + $MINOVERLAP) > $qref->{tedge};
      }
      else
      {
        echo "10";
        
        $d = $o->{qstart} - $qref->{seqr};
        $qasmr = $qref->{asmr} - $d;
        $qhang = $qref->{contig}->{len} - $qasmr;
        
        $d = $o->{qend} - $qref->{seqr};
        $qasml = $qref->{asmr} - $d;
        $qdist = $qasml - $qref->{contig}->{len};

        $q3  = ($o->{qend} - $MINOVERLAP) < $qref->{seqr};
        $q5  = ($o->{qstart} + $MINOVERLAP) > $qref->{seql};
        $qex = 0;
      }
    }
    else
    {
      if (!$qref->{rc})
      {
        echo "11";
        
        $d = $qref->{seqr} - $o->{qstart};
        $qasmr = $qref->{asmr} - $d;
        $qhang = $qref->{contig}->{len} - $qasmr;
        
        $d = $qref->{seqr} - $o->{qend};
        $qasml = $qref->{asmr} - $d;
        $qdist = $qasml - $qref->{contig}->{len};

        $q3  = ($o->{qstart} - $MINOVERLAP) < $qref->{seql};
        $q5  = ($o->{qend} + $MINOVERLAP) > $qref->{seqr};
        $qex = ($o->{qend} + $MINOVERLAP) > $qref->{tedge};
      }
      else
      {
        echo "12";
        
        $d = $o->{qend} - $qref->{seqr};
        $qasmr = $qref->{asmr} - $d;
        $qhang = $qref->{contig}->{len} - $qasmr;
        
        $d = $o->{qstart} - $qref->{seqr};
        $qasml = $qref->{asmr} - $d;
        $qdist = $qasml - $qref->{contig}->{len};

        $q3  = ($o->{qstart} - $MINOVERLAP) < $qref->{seqr};
        $q5  = ($o->{qend} + $MINOVERLAP) > $qref->{seql};
        $qex = 0;
      }
    } 
  }

  $o->{rasml} = $rasml;
  $o->{rasmr} = $rasmr;
  $o->{qasml} = $qasml;
  $o->{qasmr} = $qasmr;

  echo "($rhang $qhang)";

  if ($rhang < 0) { $rhang = 0; }
  if ($qhang < 0) { $qhang = 0; }

  $rhang += $qhang;
  $o->{overhang} = $rhang;

  if ($rhang > $MAXOVERHANG)
  {
    echo $bad; return 0;
  }

  #echo " oh";
  #if ($o->{overhang} > $o->{iddist})
  #{
  #  echo $bad; return 0;
  #}

  my $gapsize = $rdist + $qdist;
  echo " gap($rdist,$qdist,$gapsize)";
  $o->{gapsize} = $gapsize;
  if ($gapsize > $MAXGAPSIZE)
  {
    echo $bad; return 0;
  }

  $r3 = ($r3) ? "r" : "";
  $q3 = ($q3) ? "q" : "";

  echo " 3'($r3$q3)";
  if ($r3 || $q3)
  {
    echo $bad; return 0;
  }

  ## Can't extend just the consensus
  $r5 &= !$rref->{cons};
  $q5 &= !$qref->{cons};

  $o->{r5} = $r5;
  $o->{q5} = $q5;


  $r5 = ($r5) ? "r" : "";
  $q5 = ($q5) ? "q" : "";

  $rex = ($rex) ? "r" : "";
  $qex = ($qex) ? "q" : "";

  $o->{rex} = $rex;
  $o->{qex} = $qex;

  echo " 5'($r5$q5)";
  echo " ex($rex$qex)";

  echo " success.\n\n"; return 1;
}

sub filterOverlaps
{
  my $goodoverlaps = shift;
  my $alloverlaps = shift;

  ## Detect multiple overlaps between pairs of reads
  for (my $i = 0; $i < scalar @$goodoverlaps; $i++)
  {
    my $o = $goodoverlaps->[$i];
    next if $o->{dup};
    next if ($o->{cons} == 2);

    for (my $j = $i+1; $j < scalar @$goodoverlaps; $j++)
    {
      my $p = $goodoverlaps->[$j];

      next if ($p->{cons} == 2);

      if (($o->{rref} eq $p->{rref}) && 
          ($o->{qref} eq $p->{qref}))
      {
        $o->{dup} = 1;
        $p->{dup} = 1;
      }
    }
  }

  ## Detect multiple overlaps between pairs of reads
  for (my $i = 0; $i < scalar @$goodoverlaps; $i++)
  {
    my $o = $goodoverlaps->[$i];
    next if $o->{dup};
    next if ($o->{cons} == 2);

    for (my $j = 0; $j < scalar @$alloverlaps; $j++)
    {
      my $p = $alloverlaps->[$j];

      next if ($p->{cons} == 2);

      if (($o->{rref} eq $p->{rref}) && 
          ($o->{qref} eq $p->{qref}) &&
          ($o->{id} != $p->{id}))
      {
        $o->{dup} = 1;
        $p->{dup} = 1;
      }
    }
  }
}

sub groupOverlapsGapSize
{
  my $goodoverlaps = shift;

  my $groups = undef;

  foreach my $o (@$goodoverlaps)
  {
    my $mygroup = undef;

    foreach my $group (@$groups)
    {
      if (abs($o->{gapsize} - $group->{gap}) < $GAPSIZE_PARTITION)
      {
        $mygroup = $group;
        last;
      }
    }

    if (!defined $mygroup)
    {
      ## Force $mygroup to become a hash reference
      $mygroup->{count} = 0;
      push @$groups, $mygroup;
    }

    push @{$mygroup->{overlaps}}, $o;

    $mygroup->{count}++;
    $mygroup->{gapsum} += $o->{gapsize};
    $mygroup->{gap} = sprintf("%.02f", ($mygroup->{gapsum} / $mygroup->{count}));
  }

  return $groups;
}

sub hasOverlap
{
  my ($o, $p) = @_;

  my $retval = 1;

  if (($p->{rasmr} < $o->{rasml}) || ## p_r left flanks
      ($o->{rasmr} < $p->{rasml}))   ## o_r left flanks
  {
    $retval = 0;
  }

  if (($p->{qasmr} < $o->{qasml}) || 
      ($o->{qasmr} < $p->{qasml}))
  {
    $retval = 0;
  }

  return $retval;
}

sub groupOverlapsOverlap
{
  my $goodoverlaps = shift;

  my $groups = undef;

  for (my $i=0; $i < scalar @$goodoverlaps; $i++)
  {
#    print "i: $i\n";
    next if !defined $goodoverlaps->[$i];

    my $goverlaps = [ $goodoverlaps->[$i] ];
    $goodoverlaps->[$i] = undef;

    my $foundoverlap = 1;
    while ($foundoverlap)
    {
      $foundoverlap = 0;

      for (my $j=$i+1; $j<scalar @$goodoverlaps; $j++)
      {
        my $p = $goodoverlaps->[$j];
        next if !defined $p;

        foreach my $o (@$goverlaps)
        {
          if (hasOverlap($o, $p))
          {
            push @$goverlaps, $p;

            $goodoverlaps->[$j] = undef;
            $foundoverlap++;
            last;
          }
        }
      }
    }

    my $curgroup;
    $curgroup->{overlaps} = $goverlaps;
    push @$groups, $curgroup;
  }

  return $groups
}

sub evaluateGroup
{
  my $curgroup = shift;

  my $bad = 0;

  ## 60% of overlaps are dups
  my $dupcount = grep {$_->{dup}} @{$curgroup->{overlaps}};
  $bad ||= ($dupcount >= .60*$curgroup->{count});

  ## Overlap entirely based on consensus (NUU)
  $bad ||= !(grep {$_->{cons} != 2} @{$curgroup->{overlaps}});

  ## More bases against join than supporting it
  $bad ||= (2*$curgroup->{overhang} > $curgroup->{iddist});

  ## Not enough matching bases in absolute sense.
  $bad ||= ($curgroup->{best}->{iddist} < $MINMATCHLEN);

  ## Poor match score
  $bad ||= ($curgroup->{best}->{idscore} < $ALIGNTHRESH);

  return $bad;
}

sub evaluateGroups
{
  my $groups = shift;

  my $numgood = 0;
  foreach my $g (@$groups)
  {
    my @overlaps = sort {     $b->{cons} <=> $a->{cons} ||
                          $a->{overhang} <=> $b->{overhang} ||
                            $b->{iddist} <=> $a->{iddist}      } @{$g->{overlaps}};


    ## The best overlap defines the attributes for the entire group
    my $best = $overlaps[0]; ## There is at least one member
    $g->{best}     = $best;
    $g->{overhang} = $best->{overhang};
    $g->{iddist}   = $best->{iddist};
    $g->{gap}      = $best->{gapsize};
    $g->{count}    = scalar @overlaps;
    $g->{idscore}  = $best->{idscore};

    $g->{support}  = $g->{iddist} - 2*$g->{overhang};

    @{$g->{overlaps}} = @overlaps;

    my $bad = evaluateGroup($g);
    $g->{bad} = $bad;
    $numgood += !$bad;
  }

#  print STDERR "Numgood: $numgood\n" if ($numgood > 1);
}

sub printExecution
{
  my $o = shift;
  my $rref = $o->{rref};
  my $qref = $o->{qref};

  my $rdelta = $o->{rmax} - $rref->{seqmax};
  my $qdelta = $o->{qmax} - $qref->{seqmax};

  my $cmd = "$rref->{contig}->{prefix} $rref->{contig}->{oo} ".
            "$qref->{contig}->{prefix} $qref->{contig}->{oo}";

  if ($rdelta > 0)
  {
    $cmd .= " rex $rref->{seqname} $rdelta";
  }

  if ($qdelta > 0)
  {
    $cmd .= " qex $qref->{seqname} $qdelta";
  }

  print "@=$cmd\n";
}

sub printGroups
{
  my $rcontig = shift;
  my $qcontig = shift;

  my $groups  = shift; 
  my $gapsize = shift;
  my $gapstdev = shift;

  return if !defined $groups;

  my $r   = $rcontig->{prefix};
  my $roo = ($rcontig->{oo}) ? "EB" : "BE";

  my $q   = $qcontig->{prefix};
  my $qoo = $qcontig->{oo} ? "EB" : "BE";

  $doverbose = 1;

  print "\@$r($roo) <-> $q($qoo) $gapsize \[$gapstdev\]\n";
  print "\@-------------------\n";

  my $num = 0;
  foreach my $g (sort {      $a->{bad} <=> $b->{bad} || 
                         $b->{support} <=> $a->{support} ||
                        $a->{overhang} <=> $b->{overhang} ||
                          $b->{iddist} <=> $a->{iddist}      } @$groups)
  {
    my $bad = ($g->{bad}) ? "x" : "";
    $num++;

    print "\@$bad$num: Gap: $g->{gap} Id*Dist: $g->{iddist} ($g->{idscore}%) Overhang: $g->{overhang} (";
    print join(",", map{my $cons = $_->{consd}; 
                        my $dup  = ($_->{dup}) ? "x" : "";
                        "$dup$_->{gapsize}$cons"} @{$g->{overlaps}}), ")\n";

    if (!$bad)
    {
      my $best = $g->{best};
      printOverlap($best);
      evaluateOverlap($best);
      printExecution($best);

      if (0)
      {
        foreach my $o (@{$g->{overlaps}})
        {
          printOverlap($o);
          evaluateOverlap($o);
        }
      }
    }
  }

  print "\@\n\@\n";
}



MAIN:
{
  my $gapsize = "";
  my $gapstdev = "";

  $tf->addDependInfo(@DEPENDS);
  $tf->setHelpInfo($HELPTEXT);
  $tf->setVersionInfo($VERSION);


  # now we handle the input options
  my $result = $tf->TIGR_GetOptions
               (
                 'v|verbose!',    \$PRINTALL,
                 'gapsize=s',     \$gapsize,
                 'gapstdev=s',    \$gapstdev,
                 'alignthresh=s', \$ALIGNTHRESH,
                );

  $tf->bail("Command line parsing failed") if (!$result);

  my $roo = shift @ARGV;
  my $qoo = shift @ARGV;
  my $coords = shift @ARGV;

  die $USAGE if !defined $coords;

  my $id = 1;

  my $goodoverlaps = undef;
  my $alloverlaps = undef;
  my $inalignment=0;

  open COORDS, "< $coords" 
    or $tf->bail("Can't open $coords ($!)\n");

  while (<COORDS>)
  {
    chomp;

    if (!$inalignment)
    {
      if (/=====/) { $inalignment = 1; }
      elsif (!exists $rcontig{prefix})
      {
        my ($r, $q) = split / /, $_;

        $rcontig{prefix} = $1 if ($r =~ /\/(\w+)\.\w+$/);
        $qcontig{prefix} = $1 if ($q =~ /\/(\w+)\.\w+$/);

        getContigInfo(\%rcontig, \%rlayout, "r", $roo);
        getContigInfo(\%qcontig, \%qlayout, "q", $qoo);
      }
    }
    else
    {
      my $o = parseOverlap($id, $_);

      $doverbose = 0 if $PRINTALL;
      my $goodoverlap = evaluateOverlap($o);

      if ($PRINTALL)
      {
        $doverbose = 1;
        printOverlap($o);
        evaluateOverlap($o);
      }

      push @$alloverlaps, $o;
      push @$goodoverlaps, $o if ($goodoverlap);

      $id++;
    }
  }
  close COORDS;

  if (defined $goodoverlaps)
  {
    filterOverlaps($goodoverlaps, $alloverlaps);

    my $groups = groupOverlapsOverlap($goodoverlaps);
    #my $groups = groupOverlapsGapSize($goodoverlaps);

    evaluateGroups($groups);
    printGroups(\%rcontig, \%qcontig, $groups, $gapsize, $gapstdev);
  }
}
