#!/usr/bin/perl -w
use strict;
use Getopt::Long;

## Options
my $RENUM;
my $PREFIX = "";
my $SUFFIX = "";
my $TR_INPUT;
my $TR_OUTPUT = "_";

my $USAGE = "fastq_rename [options] orig.fq > new.fq\n";

my $help = 0;

my $res = GetOptions("help"      => \$help,
                     "renum"     => \$RENUM,
                     "prefix=s"  => \$PREFIX,
                     "suffix=s"  => \$SUFFIX,
                     "tr=s"      => \$TR_INPUT,
                     "tr_out=s"  => \$TR_OUTPUT,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "  Rename the reads in a fastq file to become <prefix> [number or name] <suffix>\n";
  print "  If not renumbering, use -tr to replace characters with -trc in name\n";
  print "\n";
  print "Options:\n";
  print "  -prefix <str> : add prefix to each readname\n";
  print "  -renum        : replace the readname as a sequential number 1,2,...,E,F,10,11...\n";
  print "  -tr <str>     : replace these characters in the readname\n";
  print "  -trc <str>    : when replacing, replace with this (default: $TR_OUTPUT)\n";
  print "  -suffix <str> : add suffix to each readname\n";
  exit 0;
}

my $c = 0;
while (<>)
{
  $c++;

  die "ERROR: Malformed fastq format, expected '\@' saw $_"
    if (substr($_, 0, 1) ne "\@");

  if ($RENUM)
  {
    printf "\@$PREFIX%X$SUFFIX\n", $c;
  }
  else
  {
    chomp;

    $_ = substr($_,1); ## skip the leading '@'
    $_ =~ s/$TR_INPUT/$TR_OUTPUT/g if (defined $TR_INPUT);
    print "\@$PREFIX$_$SUFFIX\n";
  }

  my $seq = <>; print $seq;
  my $h2  = <>; print "+\n";
  my $qv  = <>; print $qv;
}
