#!/usr/bin/perl -w
use strict;

my $USAGE = "patchassembly prefix regions ref.fa patches.fa coords\n";
my $EXTRACT_FASTA = "/bluearc/data/schatz/mschatz/devel/amos/bin/extractFasta";
my $FASTA_FORMAT = "/data/schatz/software/bin/fasta_formatter";

my $prefix  = shift @ARGV or die $USAGE;
my $regions = shift @ARGV or die $USAGE;
my $reffa   = shift @ARGV or die $USAGE;
my $patches = shift @ARGV or die $USAGE;
my $coords  = shift @ARGV or die $USAGE;


## Patch management
#####################################################################

my $PATCH_ID;

sub startPatch
{
  my $id = shift;

  $PATCH_ID = $id;

  open BED, "> $prefix.$id.bed" or die "Can't open $prefix.$id.bed\n";
  runCmd("start $id", "echo \">$id.patched\" > $prefix.$id.fa.raw");
}

sub extractRegion
{
  my $id    = shift;
  my $start = shift;
  my $end   = shift;
  my $oo    = shift;
  my $fa    = shift;

  print BED "$id\t$start\t$end\t1\t1\t$oo\n";

  my $rc = ($oo eq "-") ? "-rc" : "";

  runCmd("extract", "$EXTRACT_FASTA -skip -x $start -y $end -i $id $rc $fa >> $prefix.$PATCH_ID.fa.raw");
}

sub endPatch
{
  close BED;

  runCmd("format", "$FASTA_FORMAT -w 70 -i $prefix.$PATCH_ID.fa.raw -o $prefix.$PATCH_ID.fa");
}


## Load coords data
#####################################################################

my %coords;

my $coordcnt = 0;

print "Loading alignment coordinates from $coords... ";

open COORDS, "< $coords"
  or die "Can't open $coords ($!)\n";

#   36948    37958  |        1     1011  |     1011     1011  |   100.00  | 25479815     1011  |     0.00   100.00  |   scaffold_0 scaffold7781.4

while (<COORDS>)
{
  $coordcnt++;

  chomp;
  s/^\s+//;

  my @vals = split /\s+/, $_;

  my $c;
  $c->{rstart} = $vals[0];
  $c->{rend}   = $vals[1];
  $c->{qstart} = $vals[3];
  $c->{qend}   = $vals[4];
  $c->{details} = $_;
  
  my $ref   = $vals[17];
  my $patch = $vals[18];

  push @{$coords{$ref}->{$patch}}, $c;

 # print "$ref\t$patch\t$c->{rstart}\t$c->{rend}\t$c->{qstart}\t$c->{qend}\t||$_\n";   

 #  die if $coordcnt > 10;
}

close COORDS;

print "loaded $coordcnt coordinates\n";


## Load reference lengths
#####################################################################
my %lengths;

my $lencnt = 0;

if (-r "$reffa.lens")
{
  print "Loading reference lengths from $reffa.lens... ";

  open LENS, "$reffa.lens"
    or die "Can't open $reffa.lens ($!)\n";
}
else
{
  print "Loading reference lengths from $reffa... ";

  open LENS, "getlengths $reffa |"
    or die "Can't open pipe to getlengths ($!)\n";
}

while (<LENS>)
{
  chomp;

  my @vals = split /\s+/, $_;
  $lengths{$vals[0]} = $vals[1];
  $lencnt++;
}

close LENS;

print "loaded $lencnt reference sequence lengths\n";




## scan the region files
#####################################################################

#scaffold_110    316053  316238  185     |       scaffold794.272 1011    1013    1       2       |       -183    | 2.357   

open REGIONS, "< $regions" or die "Can't open $regions ($!)\n";

my $lastseq = "";
my $lastpos = 0;

while (<REGIONS>)
{
  chomp;

  my @vals = split /\s+/, $_;
  my $refseq = $vals[0];
  my $rstart = $vals[1];
  my $rend   = $vals[2];

  my $patch  = $vals[5];
  my $pstart = $vals[6];
  my $pend   = $vals[7];
  my $poo    = $vals[8];

  if ($lastseq ne $refseq)
  {
    if ($lastseq ne "")
    {
      my $seqlen = $lengths{$lastseq};
      extractRegion($lastseq, $lastpos, $seqlen, "+", $reffa);
      endPatch();
    }

    startPatch($refseq);
    $lastseq = $refseq;
    $lastpos = 0;
  }


  if ($pend < $pstart)
  {
    print  "Negative gap: $_\n";

    my $coordref = $coords{$refseq}->{$patch};

    foreach my $c (@$coordref)
    {
       print "$refseq\t$patch\t$c->{rstart}\t$c->{rend}\t$c->{qstart}\t$c->{qend}\t||$_\n";   
    }

    if (scalar @$coordref != 2)
    {
      print "complex negative gap! puntting!!!\n";

      extractRegion($lastseq, $lastpos, $rend, "+", $reffa);
      $lastpos = $rend;
    }
    else
    {
      my $c1 = $coordref->[0];
      my $c2 = $coordref->[1];

      my $rs = $c1->{rstart};
      my $re = $c2->{rend};

      my $qs = $c1->{qstart};
      my $qe = $c2->{qend};
      my $qoo = "+";

      if ($qs > $qe)
      {
        my $t = $qs; $qs = $qe; $qe = $t;
        $qoo = "-";
      }

      die if ($poo && ($qoo eq "+"));
      die if (!$poo && ($qoo eq "-"));

      print "replacing  $refseq:[$rs - $re] with $patch:[$qs - $qe]:$qoo\n";

      extractRegion($lastseq, $lastpos, $rs, "+", $reffa);
      extractRegion($patch, $qs-1, $qe, $qoo, $patches);

      $lastpos = $re;
    }
  }
  else
  {
    extractRegion($lastseq, $lastpos, $rstart, "+", $reffa);
    if ($poo) { extractRegion($patch, $pstart-1, $pend, "-", $patches); }
    else      { extractRegion($patch, $pstart-1, $pend, "+", $patches); }
    $lastpos = $rend;
  }
}


if ($lastseq ne "")
{
  my $seqlen = $lengths{$lastseq};
  extractRegion($lastseq, $lastpos, $seqlen, "+", $reffa);
  endPatch();
}


## runCmd
###############################################################################

sub runCmd
{
  my $desc = $_[0];
  my $cmd  = $_[1];

  print "$desc: $cmd...\n";
  my $rc = system($cmd);
  die $rc if $rc;
}
