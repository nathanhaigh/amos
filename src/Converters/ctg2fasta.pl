#!/usr/local/bin/perl

use TIGR::ParseFasta;
use TIGR::AmosLib;

my $parser = new ParseFasta(\*STDIN, '#', '');

while (my ($head, $data) = $parser->getRecord()){

    if ($head =~ /^\#(\S+)/){
	my $name = $1;
	$data =~ s/-//g;
	printFastaSequence(\*STDOUT, $name, $data);
    }

}

exit(0);
