#!/usr/bin/perl

use TIGR::Foundation;
use AMOS::ParseFasta;

$tf = new TIGR::Foundation;

if (!defined $tf){
    die ("Bad foundation\n");
}

if ((scalar @ARGV == 0) || ($ARGV[0] eq '-h')){
  die "Separate filtered and raw sequences\nUSAGE: $0 [fasta_file1] [fasta_file2] ... [fasta_filen] \n";
}


foreach my $file (@ARGV)
{
  open(IN, $file) || $tf->bail("Cannot open $file: $!\n");
  $fr = new AMOS::ParseFasta(\*IN);

  my $cnt_raw = 0;
  my $cnt_filter = 0;

  open(RAW,    "> $file.raw") || $tf->bail("Can't open $file.raw: $!\n");
  open(FILTER, "> $file.filter") || $tf->bail("Can't open $file.filter: $!\n");

  die ("Bad reader\n")
    if (!defined $fr);

  while (($head, $body) = $fr->getRecord())
  {
    if ($head =~ /.*\/.*\/.*/)
    {
      $cnt_filter++;
      print FILTER ">$head\n$body\n";
    }
    else
    {
      $cnt_raw++;
      print RAW ">$head\n$body\n";
    }
  }

  print STDERR "$file raw: $cnt_raw filter: $cnt_filter\n";
}

