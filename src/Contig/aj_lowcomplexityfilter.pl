#!/usr/bin/perl -w
use strict;
use TIGR::Foundation;

my $USAGE = "Usage: aj_lowcomplexityfilter infile outfile\n";

my $HELPTEXT = qq~
Strip low complexity sequence from a multifasta file.

  $USAGE
~;

my $VERSION = " Version 1.00 (Build " . (qw/$Revision$/ )[1] . ")";

my $DUST = "dust";

my @DEPENDS = 
(
  "TIGR::Foundation",
  "dust",
);

my $DOVERBOSE = 1;

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




MAIN:
{
  $tf->addDependInfo(@DEPENDS);
  $tf->setHelpInfo($HELPTEXT);
  $tf->setVersionInfo($VERSION);
  my $result = $tf->TIGR_GetOptions();
  $tf->bail("Command line parsing failed") if (!$result);

  my $infile = shift @ARGV or die $USAGE;
  my $outfile = shift @ARGV or die $USAGE;

  runCmd("$DUST $infile > $outfile");

  ## TODO: After running dust, scan the sequence, and after some number of
  ## consequtive N's in a row, do a hard trim of the sequence. This may help
  ## in the case where nucmer finds an alignment after the low complexity 
  ## sequence
}
