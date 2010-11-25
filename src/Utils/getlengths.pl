#!/usr/local/bin/perl

use TIGR::Foundation;
use AMOS::ParseFasta;

$tf = new TIGR::Foundation;

if (!defined $tf){
    die ("Bad foundation\n");
}

if ($ARGV[0] eq '-h') {
  die "Report the length of the sequences in a FASTA file. If no file is provided, standard input is used\nUSAGE: $0 [fasta_file]\n";
}

if (scalar @ARGV == 0)
{
  $fr = new AMOS::ParseFasta(\*STDIN);
}
else
{
open(IN, $ARGV[0]) || $tf->bail("Cannot open $ARGV[0]: $!\n");
$fr = new AMOS::ParseFasta(\*IN);
}

if (!defined $fr){
    die ("Bad reader\n");
}

while (($head, $body) = $fr->getRecord()){
    $head =~ /(\S+)/;
    $id = $1;
    print "$id ", length($body), "\n";
}
