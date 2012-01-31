#!/usr/bin/perl -w
use strict;
use Getopt::Long;


## Options
my $help  = 0;
my $prefix = undef;

my $MAX_LEN = 40;

my $USAGE = "USAGE: sff2fq <options> sff\n"; 

my $res = GetOptions("help"      => \$help,
                     "prefix=s"  => \$prefix,
                     "len=s"     => \$MAX_LEN,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print "  Convert sff file to a pair of fq files\n";
  print "\n";
  print "Required\n";
  print "  sff             : input reads\n";
  print "\n";
  print "Options\n";
  print "  -prefix <name> : prefix for fq files (default: prefix of .sff file)\n";
  print "  -len <len>     : max length of reads (default: $MAX_LEN)\n";
  exit 0;
}

die $USAGE if (scalar @ARGV != 1);

my $sfffile = $ARGV[0];

if (!defined $prefix)
{
  $prefix = $sfffile;
  $prefix =~ s/\.sff$//;
}

print STDERR "creating $prefix.1.fq and $prefix.2.fq using $MAX_LEN bp reads\n\n";

runCmd("sff_extract", "$prefix.raw.fastq", "sff_extract.py -l linkerall.fa -c -Q $sfffile -o $prefix.raw");
runCmd("convert fq1", "$prefix.1.fq",      "fastq_to_fasta_fast $prefix.raw.fastq | fasta_trim -l $MAX_LEN | fasta_formatter -t | grep '\.f' | awk '{print \">\"substr(\$1,1,length(\$1)-2)\"/1\"; print \$2}' | fasta_to_fastq > $prefix.1.fq");
runCmd("convert fq2", "$prefix.2.fq",      "fastq_to_fasta_fast $prefix.raw.fastq | fasta_trim -l $MAX_LEN | fasta_formatter -t | grep '\.r' | awk '{print \">\"substr(\$1,1,length(\$1)-2)\"/2\"; print \$2}' | fasta_to_fastq > $prefix.2.fq");


## runCmd
###############################################################################

sub runCmd
{
  my $desc = $_[0];
  my $outf = $_[1];
  my $cmd  = $_[2];
  my $force = $_[3];

  if (! -r $outf || defined $force)
  {
    print STDERR "$desc [$cmd]...\n";
    my $rc = system($cmd);
    die "Command failed rc:$rc ($!)\n" if $rc;
  }
}
