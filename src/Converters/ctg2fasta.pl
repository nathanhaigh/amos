#!/usr/local/bin/perl

use strict;
use AMOS::AmosLib;
use AMOS::ParseFasta;

my $parser = new AMOS::ParseFasta(\*STDIN, '#', '');

while (my ($head, $data) = $parser->getRecord()){

    if ($head =~ /^\#(\S+)/){
	my $name = $1;
	$data =~ s/-//g;
	printFastaSequence(\*STDOUT, $name, $data);
    }

}

exit(0);
