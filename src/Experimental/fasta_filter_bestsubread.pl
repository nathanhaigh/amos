#!/usr/bin/perl
use strict;
use FileHandle;
use Getopt::Long;

## Options
my $help  = 0;
my $USAGE = "fasta_filter_bestsubread <options> subreads.fa\n"; 

my $VERBOSE = 0;

my $res = GetOptions("help"      => \$help,
                     "verbose"   => \$VERBOSE,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print " Given a file of subreads, print just the longest subread for each molecule\n";
  print "\n";
  print "Required\n";
  print "  reads.fa  : path to reference genome (must be indexed with bwa)\n";
  print "\n";
  print "Options\n";
  print "  -verbose  : be verbose with output\n";
  exit 0;
}


# >m110425_194810_00124_c100105422556400000315040006221190_s1_p0/10/34_688

my $allreads = 0;
my $printedreads = 0;

my $bestfull   = "";
my $bestbase   = "";
my $bestreadid = "";
my $bestseq    = "";

my $fullname = "";
my $basename = "";
my $readid   = "";
my $seq      = "";

while (<>)
{
  if (/>(\S+)/)
  {
    $allreads++;

    if (($allreads % 100000) == 0)
    {
      print STDERR "$allreads\n";
    }

    if (($basename eq $bestbase) && ($readid eq $bestreadid))
    {
      if (length($seq) > length($bestseq))
      {
        ## New best
        $bestfull = $fullname;
        $bestseq  = $seq;
      }
    }


    $fullname = $1;
    ($basename, $readid) = split /\//, $fullname;

    $seq = "";

    if (($basename ne $bestbase) || ($readid ne $bestreadid))
    {
      if ($bestfull ne "")
      {
        print ">$bestfull\n";
        print "$bestseq";

        $printedreads++;
      }

      $bestfull   = $fullname;
      $bestbase   = $basename;
      $bestreadid = $readid;
      $bestseq    = "";
    }
  }
  else
  {
    $seq .= $_;
  }
}

if ($bestfull ne "")
{
  print ">$bestfull\n";
  print "$bestseq";

  $printedreads++;
}



print STDERR "Printed $printedreads of $allreads\n";

