#!/usr/bin/perl -w
use strict;
use Getopt::Long;

my $USAGE = "ctg_gc <options> ctg.fa\n"; 

## Options
my $help  = 0;

my $WINDOW = 25;
my $MIN_CTG_LEN = 500;
my $INTERIOR="I";
my $END="E";

my $res = GetOptions("help"       => \$help,
                     "window=s"   => \$WINDOW,
                     "minctg=s"   => \$MIN_CTG_LEN,
                     "interior=s" => \$INTERIOR,
                     "end=s"      => \$END,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print "  Compute the GC content of contig interiors versus contig ends\n";
  print "  Output format: <code> \\t <window gc>\n";
  print "\n";
  print "Required\n";
  print "  ctg.fa          : path to draft assembly\n";
  print "\n";
  print "Options\n";
  print "  -window <len>  : window length (default: $WINDOW)\n";
  print "  -minctg <len>  : minimum contig length to examine (default: $MIN_CTG_LEN)\n";
  print "  -interior <c>  : Code for ctg interior (default: $INTERIOR)\n";
  print "  -end <c>       : code for ctg end (default: $END)\n";
  exit 0;
}


## Count GC of a sequence
###############################################################################
sub countgc
{
  my $str = $_[0];
  my $g = ($str =~ tr/gGcC//);

  return $g;
}



## Process a single contig
###############################################################################
sub processContig
{
  my $seq = $_[0];

  return if !defined $seq;

  if (length($seq) >= $MIN_CTG_LEN)
  {
    my $gc = countgc(substr($seq, 0, $WINDOW));
    print "$END\t$gc\n";

    for (my $pos = $WINDOW; $pos+1.5*$WINDOW < length($seq); $pos+=$WINDOW)
    {
      my $wstr = substr($seq, $pos, $WINDOW);
      $gc = countgc($wstr);
      print "$INTERIOR\t$gc\n";
    }

    $gc = countgc(substr($seq, -$WINDOW));
    print "$END\t$gc\n";
  }
}



## Scan the multifasta file
###############################################################################
my $seq = undef;

while (<>)
{
  if (/^>/)
  {
    processContig($seq);
    $seq = "";
  }
  else
  {
    chomp;
    $seq .= $_;
  }
}

processContig($seq);
