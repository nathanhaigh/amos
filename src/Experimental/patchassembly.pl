#!/usr/bin/perl -w
use strict;

my $USAGE = "patchassembly prefix regions ref.fa patches.fa\n";
my $EXTRACT_FASTA = "/bluearc/data/schatz/mschatz/devel/amos/bin/extractFasta";
my $FASTA_FORMAT = "/data/schatz/software/bin/fasta_formatter";

my $prefix  = shift @ARGV or die $USAGE;
my $regions = shift @ARGV or die $USAGE;
my $reffa   = shift @ARGV or die $USAGE;
my $patches = shift @ARGV or die $USAGE;


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


## Load reference lengths
#####################################################################
my %lengths;

open LENS, "getlengths $reffa |"
  or die "Can't open pipe to getlengths ($!)\n";

while (<LENS>)
{
  chomp;

  my @vals = split /\s+/, $_;
  $lengths{$vals[0]} = $vals[1];
}

close LENS;


## scan the region files
#####################################################################

#scaffold_110    316053  316238  185     |       scaffold794.272 1011    1013    1       2       |       -183    | 2.357   

open REGIONS, "< $regions" or die "Can't open $regions ($!)\n";

my $lastseq = "";
my $lastpos = 1;

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
    $lastpos = 1;
  }

  extractRegion($lastseq, $lastpos, $rstart+1, "+", $reffa);

  if ($pend < $pstart)
  {
    ## this should be the patch itself, but not enough info in the region file ==> punt
    extractRegion($lastseq, $rstart, $rend+1, "+", $reffa);
  }
  else
  {
    if ($poo) { extractRegion($patch, $pstart, $pend+1, "-", $patches); }
    else      { extractRegion($patch, $pstart, $pend+1, "+", $patches); }
  }

  $lastpos = $rend;
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
