#!/usr/bin/perl
use strict;
use FileHandle;
use Getopt::Long;

## Options
my $help  = 0;
my $USAGE = "fasta_filter_dups <options> reads.fa\n"; 

my $CHECK_DUPS = 0;
my $VERBOSE = 0;

my $res = GetOptions("help"      => \$help,
                     "dups"      => \$CHECK_DUPS,
                     "verbose"   => \$VERBOSE,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print " Scans a fasta file of pacbio reads and splits them SMRTcell specific files\n";
  print " Also can remove duplicated reads while printing\n";
  print "\n";
  print "Required\n";
  print "  reads.fa  : path to reference genome (must be indexed with bwa)\n";
  print "\n";
  print "Options\n";
  print "  -dups     : check for duplicated reads\n";
  print "  -verbose  : be verbose with output\n";
  exit 0;
}

my %reads;
my %files;

my $curfh = undef;
my $curbase = "";
my $doprint = 0;

# >m110425_194810_00124_c100105422556400000315040006221190_s1_p0/10/34_688

my $allreads = 0;
my $printedreads = 0;


while (<>)
{
  if (/>(\S+)/)
  {
    $allreads++;

    if (($allreads % 100000) == 0)
    {
      print STDERR "$allreads\n";
    }

    my $fullname = $1;
    my ($basename, $readid, $range) = split /\//, $fullname;


    if ($basename ne $curbase)
    {
      if (!exists $files{$basename})
      {
        $files{$basename}->{fh} = FileHandle->new("> $basename.fa");
      }

      $curfh = $files{$basename}->{fh};
      $curbase = $basename;
    }

    $doprint = 1;

    if ($CHECK_DUPS)
    {
      $doprint = 0;
      $reads{$basename}->{$readid}->{$range}++;

      if ($reads{$basename}->{$readid}->{$range} == 1)
      {
        $doprint = 1;
      }
    }

    print "$basename $readid $range $doprint\n" if $VERBOSE;

    $files{$basename}->{all}++;
    if ($doprint)
    {
      $printedreads++;
      $files{$basename}->{printed}++;
    }
  }

  if ($doprint) { print $curfh $_; }
}

my $numfiles = scalar keys %files;

print STDERR "Printed $printedreads of $allreads into $numfiles files\n";

foreach my $fn (sort keys %files)
{
  my $all = $files{$fn}->{all};
  my $p   = $files{$fn}->{printed};

  my $pp = sprintf("%0.02f", 100*$p/$all);

  print "$fn\t$all\t$p\t$pp\n";
}


