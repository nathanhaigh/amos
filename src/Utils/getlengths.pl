#!/usr/local/bin/perl

use TIGR::Foundation;
use AMOS::ParseFasta;

$tf = new TIGR::Foundation;

if (!defined $tf){
    die ("Bad foundation\n");
}

open(IN, $ARGV[0]) || $tf->bail("Cannot open $ARGV[0]: $!\n");
$fr = new AMOS::ParseFasta(\*IN);

if (!defined $fr){
    die ("Bad reader\n");
}

while (($head, $body) = $fr->getRecord()){
    $head =~ /(\S+)/;
    $id = $1;
    print "$id ", length($body), "\n";
}
