#!/usr/bin/perl
# $Id$ 
#
# contig2fasta - Convert TIGR .contig file to a fasta file containing 
# only the assembly records and omitting component sequences and singletons.
#
# Written by -  Martin Shumway
#
my $HELPTEXT = qq~
Convert TIGR .contig file to an ungapped fasta file containing only the 
assembly records and omitting component sequences and singletons.  

  contig2fasta <contigfile>  [options]
    contigfile   TIGR .contig file to extract assemblies from 
    options:
      -o <prefix>  Prefix for all index file. 
      -c <asm id>  Extract the specified contig only
      -merge  Create a monolithic fasta file for matching analysis, adding 
              filler characters (N's) between records.  Produces a .index 
              file with offsets (from 0) of the start of each record.
              The file that is created with -o option is <prefix>_merged .
      -[no]filler  Inject up to 60 filler characters (N's) in between 
              merged records (default on).  

~;
#
# (C) Copyright 2001-2003  The Institute for Genomic Research (TIGR)
#     All rights reserved.
#

# =============================== Pragmas and imports ======================
use strict;
use IO::File;
use File::Basename;
use TIGR::Foundation;
use TIGR::FASTAreader;
use TIGR::FASTArecord;

# Normally used only in testing
# use warnings;

# ================================ Globals ====================================

my $MY_APP = basename($0);
my $MY_LOG = "$MY_APP.log";
my $MY_VERSION = " Version 1.3 (Build " . (qw/$Revision$/ )[1] . ")";
my @MY_DEPENDS =
(
  "TIGR::Foundation",
  "TIGR::FASTAreader",
  "TIGR::FASTArecord",
);

my $NFILLER = 60;

# Reference to TF object
my $tf = new TIGR::Foundation;

# Operating modes
my $silent = 0;              # Do not print progress messages
my $list = 0;                # Produce list files of various sets
my $report = 1;              # Produce a report of the pull

# ================================= Procedures ===================================

# printTable - Print the content of a multiple column lookup table
# $_[0] - filename
# $_[1] - reference to table entry that may be an array
#
sub printTable($$)
{
  my $filename = shift;
  return if (! defined $filename);
  my $rh = shift;

  my $fh = new IO::File "> $filename" or $tf->bail("Failed to open $filename ($!)");
  foreach my $i (sort keys %$rh)
  {
    my @entries = @{$$rh{$i}};
    my $entry = join "\t",@entries;
    $fh->print("$i\t$entry\n") or $tf->bail("Failed to write to $filename ($!)");
  }
  $fh->close() or $tf->bail("Failed to close $filename ($!)");
}


# ==================================== Main ======================================
MAIN:
{
  my %CONTIGS = ();
  my $contigfile = undef;
  my $merge = 0;
  my $indexfile = undef;
  my %Contigs = ();
  my $asm_id = undef;
  my $isFiller = 1; 
  my $prefix = undef; 

  # ========================== Program Setup ==============================
  # Prepare logs
  $tf->addDependInfo(@MY_DEPENDS);
  $tf->setHelpInfo($HELPTEXT);
  $tf->setVersionInfo($MY_VERSION);
  $tf->setUsageInfo($HELPTEXT);

  # now we handle the input options
  my $result  = $tf->TIGR_GetOptions
                (
                  "merge", \$merge,
                  "o=s",   \$prefix,
                  "c=s",   \$asm_id,
                  "filler!",   \$isFiller,
                );

  $tf->bail("Command line parsing failed") if ($result == 0);

  $tf->printUsageInfoAndExit() if ($#ARGV != 0);
  $contigfile = $ARGV[0] if (defined $ARGV[0]);
  $tf->bail("Could not open input file \'$contigfile\' ($!)") if (! -r $contigfile);
  $indexfile = "$prefix" . "_merged.index" if ($merge);

  # =========================== Gather Inputs =============================
  $tf->logLocal("Filtering $contigfile", 4);
  my $ih = new IO::File("< $contigfile") 
    or $tf->bail("Failed to open $contigfile for input");
  my $contigtmpfile = $contigfile . ".tmp";
  my $oh = new IO::File("> $contigtmpfile") 
    or $tf->bail("Failed to open $contigtmpfile for output");
  while (my $line = $ih->getline())
  {
    if ($line =~ m/^##/)
    {
      my ($contig_id, @junk) = split /\s+/,$line;
      $contig_id =~ s/##//g; 
      $Contigs{$contig_id} = 1;
      $line =~ s/^##/>/o;
      $oh->print("$line") or $tf->bail("Failed to write $contigtmpfile for output");
    }
    else
    {
      $line =~ s/^#/>/o;
      $line =~ s/\(\d+\)//o;
      $oh->print("$line") or $tf->bail("Failed to write $contigtmpfile for output");
    }
  }
  $ih->close() or $tf->bail("Failed to close $contigfile");
  $oh->close() or $tf->bail("Failed to close $contigtmpfile");

  if (defined $asm_id  &&  ! exists $Contigs{$asm_id})
  {
    $tf->bail("Did not find contig \'$asm_id\' in $contigfile"); 
  }

  $tf->logLocal("Reading fasta records from $contigtmpfile", 9);
  my @errors = ();
  my $fr = new TIGR::FASTAreader($tf, \@errors, $contigtmpfile);
  $tf->bail("Error creating FASTA Reader object") if (! defined $fr);
  if ($#errors >= 0)
  {
    for my $e (@errors)
    {
      logerr("$e\n");
    }
    $tf->bail("Invalid FASTA on input from $contigtmpfile");
  }
  unlink $contigtmpfile or 
    $tf->bail("Failed to remove temporary file $contigtmpfile");

  my $merge_data = "";
  my %INDEX = ();
  while ( $fr->hasNext() )
  {
    my $r = $fr->next();
    my $a = $r->getIdentifier();
    if (exists $Contigs{$a})  # means it's an assembly id
    {
      next if (defined $asm_id  &&  $a ne $asm_id);
      $tf->logLocal("Found contig $a", 9);
      my $header = $r->getHeader();
      my $data = $r->getData();
      $data =~ s/-//g;               # remove gaps
      if ($merge)
      {
        # If we are merging, then flesh it out with 60 or more N's 
        my $len = length($data);
        if ($isFiller)
        {
          my $ns = ($len % 60) + 60;
          for (my $i=0; $i < $ns; $i++)
          {
            $data .= "N";
          }
        }
        my $start = length($merge_data);
        my $stop = $start + $len - 1;
        my @coords = ($start, $stop);
        $INDEX{$a} = \@coords;
        $merge_data .= $data;
      }
      else
      {
        my $r2 = new TIGR::FASTArecord($header, $data); 
        $tf->logLocal("Found assembly record $a", 9);
        $CONTIGS{$a} = $r2;
      }
    }
  }

  # =========================== write output ==============================

  # Combine data as required
  my $s = "";
  if ($merge)
  {
    printTable($indexfile, \%INDEX);
    my $r2 = new TIGR::FASTArecord($prefix . "_merged", $merge_data); 
    $s = $r2->toString();
  }
  else
  {
    foreach my $a (sort keys %CONTIGS)
    {
      # Emit header
      my $r = $CONTIGS{$a};
      $s .= $r->toString();
    }
  }

  # Emit data according to output format
  if (defined $prefix)
  {
    my $fh = undef;
    my $fn = "";
    if ($merge)
    {
      $fn = sprintf("%s_merged.fasta", $prefix);
    }
    else
    {
      $fn = sprintf("%s.fasta", $prefix);
    }
    $fh = new IO::File("> $fn") or $tf->bail("Could not open $fn for output");
    $fh->print($s);
    $fh->close();
  }
  else
  {
    print $s;
  }

  exit 0;
}
