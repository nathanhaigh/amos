#!/usr/bin/perl

## this program retrieves a collection of reads from the bank
## identifies those that belong to the same fragment, then builds
## layouts (using nucmer).

use lib "/usr/local/lib";
use AMOS::AmosLib;

my $END = 10; # end wobble allowed

if ($#ARGV < 0 || $#ARGV == 0 && $ARGV[0] eq "-h"){
    print "Usage: preassembleFrgs.pl <bank>\n";
    exit(0);
}

my $bank = $ARGV[0];
my $temp = "TEMP";

my %frgrds;
my %rdidx;
my %clears;

open(TMP, "bank-report -b $bank RED |") || die ("Cannot report bank: $!\n");

while (my $record = getRecord(\*TMP)){
    my ($rec, $fields, $recs) = parseRecord($record);
    if ($rec eq "RED"){
	my $id = $$fields{iid};
	my $frg = $$fields{frg};
	if (! defined $frg) {next;} # only want reads in fragments
	$frgrds{$frg} .= "$id ";
    } # RED records
} # for each input record

close(TMP);

while (my ($frg, $seqs) = each %frgrds){
    my @seqs = split(" ", $seqs);
    if ($#seqs < 1){ next;} # fragments with at least two reads
    
    open(TMPSEQ, ">$temp.list") || die ("Cannot open $temp.list: $!\n");

    for ($i = 0; $i <= $#seqs; $i++){
	print TMPSEQ $seqs[$i], "\n";
    }
    
    close(TMPSEQ);
    
    system ("/local/asmg/scratch/mihai/AMOS/Linux/bin/hash-overlap -B $bank -I $temp.list");

} # for each fragment

system("tigger -b $bank");
system("make-consensus -B -b $bank");
