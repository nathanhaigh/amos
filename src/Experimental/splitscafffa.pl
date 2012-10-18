#good!/usr/bin/perl -w
use strict;
use Getopt::Long;

## Split a scaffold into contigs whenever there are $NTHRESH N's in a row

my $help = 0;
my $NTHRESH = 20;
my $AGPFILENAME = undef;
my $FASTA_LEN = 60;
my $MIN_CONTIG_LEN = 200;

my $USAGE = "splitscafffa [-agp agpfile] [-n nlimit] scaff > scaff.ctg.fa\n";

my $res = GetOptions("help"  => \$help,
                     "n=s"   => \$NTHRESH,
                     "agp=s" => \$AGPFILENAME,
                     "fl=s"  => \$FASTA_LEN,
                     "l=s"   => \$MIN_CONTIG_LEN);

if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print "  Split a scaffold file of contigs separated by N's back into the\n";
  print "  individual contigs. Optionally write the scaffold information as\n";
  print "  an agp file describing how each contig is assigned to each scaffold\n";
  print "  If the output format is agp, then the contigs will be renumbered\n";
  print "\n";
  print "Options\n";
  print " -n nlimit : Identify contig boundaries by runs of <nlimit> n's (default: $NTHRESH)\n";
  print " -l len    : minimum contig length to report (default: $MIN_CONTIG_LEN)\n";
  print " -agp file : Output the scaffolding information to the specified file in agp format\n";
  print "\n";

  exit 0;
}


die $USAGE if (scalar @ARGV == 0);

if (defined $AGPFILENAME)
{
  open AGP, "> $AGPFILENAME\n"
    or die "Can't open $AGPFILENAME\n";

  print AGP "## AGP-version 2.0\n";
  print AGP "## AGP constructed by AMOS/splitscaff.pl $ARGV[0]\n";
}

my $nscaff  = 0;
my $ncontig = 0;
my $ngoodcontig = 0;

sub processScaff
{
  my $scaffid = $_[0];
  my $seq     = $_[1];

  return if !defined $scaffid;

  $nscaff++;

  if (defined $AGPFILENAME)
  {
     $scaffid = "scaffold_$nscaff";
  }

  my $prevoffset = 0;
  my $partnumber = 1;
  my $start_n = -1;

  my @sections = split /N{$NTHRESH,}/, $seq;

  for (my $i = 0; $i < scalar @sections; $i++)
  {
    $ncontig++;

    my $offset = index($seq, $sections[$i], $prevoffset);

    die "ERROR finding substring with section $i\n"  
      if $offset == -1;

    my $contig_len = length ($sections[$i]);
    my $end = $offset + $contig_len - 1; ## inclusive coords

    if ($contig_len < $MIN_CONTIG_LEN)
    {
      ## skip over short contigs
      next;
    }

    $ngoodcontig++;

    if ($start_n == -1)
    {
      ## we are about to print out the first acceptable contig. 
      ## Hopefully this is at position 1, but shift just in case.

      $start_n = $offset;
    }

    if (defined $AGPFILENAME)
    {
       ## print the info for the preceeding gap
       if ($prevoffset > 0)
       {
         my $gapstart       = $prevoffset + 1 + 1 - $start_n;   ## prevoffset was last base in contig + 1-based
         my $gapend         = $offset - 1 + 1 - $start_n;       ## offset is first base in contig + 1-based
         my $component_type = "N"; ## Gap of known size
         my $gaplen         = $gapend - $gapstart + 1;
         my $gaptype        = "scaffold";
         my $linkage        = "yes"; ## gaps linked by mate pairs
         my $linkage_type   = "paired-ends";

         print AGP "$scaffid\t$gapstart\t$gapend\t$partnumber\t$component_type\t$gaplen\t$gaptype\t$linkage\t$linkage_type\n";
         
         $partnumber++;
       }

       ## now print the contig record
       my $component_type = "W"; ## WGS Contig
       my $orientation    = "+"; ## By construction, contigs are forward
       my $contig_start   = $offset + 1 - $start_n;  ## 1-based coords
       my $contig_end     = $end + 1 - $start_n;     ## 1-based coords

       print AGP "$scaffid\t$contig_start\t$contig_end\t$partnumber\t$component_type\tcontig_$ngoodcontig\t1\t$contig_len\t$orientation\n";

       $partnumber++;

       print ">contig_$ngoodcontig\n";
    }
    else
    {
       print ">$scaffid\.$i\.$offset\.$end\n";
    }

    for (my $j = 0; $j < $contig_len; $j += $FASTA_LEN)
    {
      print substr($sections[$i], $j, $FASTA_LEN), "\n";
    }

    $prevoffset = $offset + $contig_len - 1; ## last base in contig
  }
}


my $scaffid = undef;
my $seq     = undef;

while (<>)
{
  chomp;

  if (/^>(\S+)/)
  {
    processScaff($scaffid, $seq);
    $scaffid = $1;
    $seq = "";
  }
  else
  {
    $seq .= $_;
  }
}

processScaff($scaffid, $seq);

print STDERR "Processed $nscaff scaffolds and $ncontig contigs, printed $ngoodcontig at least $MIN_CONTIG_LEN bp long\n";
