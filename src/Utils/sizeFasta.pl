#!/usr/bin/perl
# $Id$ 
#
# sizeFasta - Return the size of records in a fasta file or set of files.
#
# Written by -  Martin Shumway
#
my $HELPTEXT = qq~
Return the size in bases of the records in a fasta file.  More than one
fasta file can be listed in the invocation perhaps following expansion
in the shell.

  sizeFasta <fasta files>  [options]
    options:
      -i <id>      Extract the specified id only

~;
#
# (C) Copyright 2001-2004  The Institute for Genomic Research (TIGR)
#     All rights reserved.
#

# =============================== Pragmas and imports ======================
use strict;
use TIGR::Foundation;
use TIGR::FASTAreader;
use TIGR::FASTArecord;

# Normally used only in testing
# use warnings;

# ================================ Globals ====================================

my $MY_VERSION = " Version 1.0 (Build " . (qw/$Revision$/ )[1] . ")";
my @MY_DEPENDS =
(
  "TIGR::Foundation",
  "TIGR::FASTAreader",
  "TIGR::FASTArecord",
);

# Reference to TF object
my $tf = new TIGR::Foundation;

# ================================= Procedures ===================================

MAIN:
{
  # ========================== Program Setup ==============================
  $tf->addDependInfo(@MY_DEPENDS);
  $tf->setHelpInfo($HELPTEXT);
  $tf->setVersionInfo($MY_VERSION);
  $tf->setUsageInfo($HELPTEXT);

  # now we handle the input options
  my $id = undef;
  my $result  = $tf->TIGR_GetOptions
                (
                  "i=s",   \$id,
                );

  $tf->bail("Command line parsing failed") if ($result == 0);

  $tf->printUsageInfoAndExit() if ($#ARGV < 0);

  # =========================== Gather Inputs =============================
  $tf->logLocal("Filtering fastafiles " . join(",",@ARGV), 4);

  my %SIZE = ();
  for (my $i=0; $i <= $#ARGV; $i++)
  { 
    my $fastafile = $ARGV[$i];
    $tf->bail("Could not open input file ($!)") if (! -r $fastafile);
    $tf->logLocal("Reading fasta records from $fastafile", 9);
    my @errors = ();
    my $fr = new TIGR::FASTAreader($tf, \@errors, $fastafile);
    $tf->bail("Error creating FASTA Reader object") if (! defined $fr);
    if ($#errors >= 0)
    {
      for my $e (@errors)
      {
        logerr("$e\n");
      }
      $tf->bail("Invalid FASTA on input from $fastafile");
    }

    while ( $fr->hasNext() )
    {
      my $r = $fr->next();
      my $a = $r->getIdentifier();
      next if (defined $id  &&  $a ne $id);
      $tf->logLocal("Found contig $a", 9);
      $SIZE{$a} = $r->size();
    }
  }

  # =========================== write output ==============================

  if (defined $id  &&  ! exists $SIZE{$id})
  {
    $tf->bail("Did not find id \'$id\'"); 
  }
  elsif (defined $id)
  {
    my $s = sprintf("%s\t%d\n", $id, $SIZE{$id});
    print $s;
  }
  else
  {
    foreach my $a (sort keys %SIZE)
    {
      # Emit header
      my $s = sprintf("%s\t%d\n", $a, $SIZE{$a});
      print $s;
    }
  }

  exit 0;
}
