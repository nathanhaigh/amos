#!/usr/local/bin/perl

use strict;
use TIGR::AmosLib;
use TIGR::ParseFasta;

my $parser = new TIGR::ParseFasta(\*STDIN, '#', '');

while (my ($head, $data) = $parser->getRecord()){

    if ($head =~ /^\#(\S+)/){
	my $name = $1;
	$data =~ s/-//g;
	printFastaSequence(\*STDOUT, $name, $data);
    }

}

exit(0);
