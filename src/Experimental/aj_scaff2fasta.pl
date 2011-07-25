#!/usr/bin/perl
use strict;
use TIGR::Foundation;
use TIGR::AsmLib;

my $USAGE = "Usage: aj_scaff2fasta.pl prefix.scaff prefix.fasta -o out.scaffolds.fasta\n";

my $MINGAPSIZE = 100;

my $HELPTEXT = qq~
Convert a scaff file and fasta of contigs to scaffolds.fasta with 
$MINGAPSIZE 'N' spacers. Alternatively separate contigs by the expected gap 
size.

  $USAGE
  Options
  -------
  -e Space contigs in scaffold by expected gap size

  -min <MIN> Specify minimum gapsize when using expected gap size
     (Gaps smaller than MIN bp get MIN Ns Default: $MINGAPSIZE)
~;

my $VERSION = " Version 1.00 (Build " . (qw/$Revision$/ )[1] . ")";

my @DEPENDS = 
(
  "TIGR::Foundation",
);

my $DOVERBOSE = 0;

my $tf = new TIGR::Foundation;

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

sub printScaffold
{
  my $outfile    = shift;
  my $scaffid    = shift;
  my $numcontigs = shift;
  my $sequence   = shift;

  printFastaSequence($outfile, 
                     sprintf("%s %d bp %d contigs", $scaffid, length($sequence), $numcontigs), 
                     $sequence);
}


MAIN:
{
  my $USEEXPECTED = 0;
  my $outfile = undef;

  $tf->addDependInfo(@DEPENDS);
  $tf->setHelpInfo($HELPTEXT);
  $tf->setVersionInfo($VERSION);
  my $result = $tf->TIGR_GetOptions("o=s",   \$outfile,
                                    "min=i", \$MINGAPSIZE,
                                    "e",     \$USEEXPECTED);


  $tf->bail("Command line parsing failed") if (!$result);

  my $USEDEFAULTGAPSIZE = !$USEEXPECTED;

  my $scaff = shift or die $USAGE;
  my $fasta = shift or die $USAGE;
  die $USAGE if !defined $outfile;

  open SCAFF,   "< $scaff" or $tf->bail("Can't open $scaff ($!)\n");
  open FASTA,   "< $fasta" or $tf->bail("Can't open $fasta ($!)\n");
  open OUTFILE, "> $outfile" or $tf->bail("Can't open $outfile ($!)\n");

  my %contigs;
  my $contigid;

  echo "Loading sequences... ";

  while (<FASTA>)
  {
    if (/>(\S+)/)
    {
      $contigid = $1;
    }
    else
    {
      chomp;
      $contigs{$contigid} .= $_;
    }
  }

  close FASTA;

  echo "ok.\n";

  my $scaffid = undef;
  my $numcontigs;
  my $sequence;
  my $contigcount;

  echo "Printing scaffolds.fasta... ";

  while (<SCAFF>)
  {
    if (/^>(\S+)\s+(\S+)\s+(\S+)\s+(\S+)/)
    {
      printScaffold(\*OUTFILE, $scaffid, $numcontigs, $sequence)
        if defined $scaffid;

      $contigcount = 0;
      $sequence    = "";
      $scaffid     = $1;
      $numcontigs  = $2;
      my $bases    = $3;
      my $span     = $4;
    }
    elsif (/(\S+)\s+(\S+)\s+(\S+)\s+(\S+)/)
    {
      my $contigid = $1;
      my $oo       = $2;
      my $size     = $3;
      my $gapsize  = $4;

      $contigcount++;

      $tf->bail("Scaffold references missing contig $contigid!\n")
        if !exists $contigs{$contigid};

      my $cons = $contigs{$contigid};
      $cons = reverseComplement($cons) if ($oo eq "EB");
      $sequence .= $cons;

      if ($contigcount != $numcontigs)
      {
        my $gapcount = int($gapsize);
        $gapcount = $MINGAPSIZE if ($gapcount < $MINGAPSIZE || $USEDEFAULTGAPSIZE);

        my $gaps = "N"x$gapcount;
        $sequence .= $gaps;
      }
    }
  }
  close SCAFF;

  printScaffold(\*OUTFILE, $scaffid, $numcontigs, $sequence)
   if defined $scaffid;

  echo "ok.\n";

 close OUTFILE;
}
