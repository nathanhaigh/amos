#!/usr/local/bin/perl

# $Id$
#
# Converts from a TIGR .asm file to a new .ace file
#
#  Copyright @ 2002, The Institute for Genomic Research (TIGR). 

use strict;
use TIGR::AsmLib;
use TIGR::Foundation;

my $base = new TIGR::Foundation;
if (! defined $base){
    die("Wierd problem!\n");
}

my $VERSION = '1.0 $Revision$ ';

my $acefile;
my $contigfile;

my $HELP_INFO = q~
   ace2contig [-o <contigfile>] [-i] <acefile>
    ~;

$base->setHelpInfo($HELP_INFO);

my $err = $base->TIGR_GetOptions("i=s" => \$acefile,
				 "o=s" => \$contigfile
				 );

if (! defined $acefile){
    $acefile = $ARGV[0];
    if (! defined $acefile) {
	$base->bail("You must specify an .ACE file\n");
    }
}

if ($acefile !~ /\.ace$/){
    $base->bail("Ace file \"$acefile\" must end in .ace\n");
}

if (! defined $contigfile){
    my $prefix;
    $acefile =~ /(.*)\.ace$/;
    $prefix = $1;
    $contigfile = $prefix . ".contig";
}

open(ACE, $acefile) || $base->bail("Cannot open \"$acefile\": $!\n");
open(CONTIG, ">$contigfile") || $base->bail("Cannot open \"$contigfile\": $!\n");

my $contigName;
my $contigLen;
my $contigSeqs;
my $inContig = 0;
my $inSequence = 0;
my $seqName;
my $seq;
my %offset;
my %rc;
my @gaps;
my $contigSequence;
while (<ACE>){
    if (/^CO (\S+) (\d+) (\d+)/){
	$contigName = $1;
	$contigLen = $2;
	$contigSeqs = $3;
	$inContig = 1;
	$seq = "";
	%offset = ();
	next;
    }
    if ($inContig && /^\s*$/){
	$inContig = 0;
	$seq =~ s/\*/-/g;
	@gaps = (); 
	my $gap  = index($seq, "-");
        while ($gap != -1){
            push(@gaps, $gap + 1);
            $gap = index($seq, "-", $gap + 1);
        }

	$contigSequence = $seq;
#	printContigRecord(\*CONTIG, $contigName, $contigLen, $contigSeqs, $seq, "contig");
	next;
    }
    if ($inSequence && $_ =~ /^\s*$/){
	$inSequence = 0;
	next;
    }
    if ($inContig || $inSequence) {
	chomp;
	$seq .= $_;
	next;
    }
    if (/^AF (\S+) (\w) (-?\d+)/){
	$offset{$1} = $3;
	$rc{$1} = $2;
	next;
    }
    if (/^RD (\S+)/){
	$inSequence = 1;
	$seqName = $1;
	$seq = "";
	next;
    }
    if (/^QA -?(\d+) -?(\d+) (\d+) (\d+)/){
	my $offset = $offset{$seqName};
	my $cll = $3;
	my $clr = $4;
	my $end5 = $1;
	my $end3 = $2;
	$seq =~ s/\*/-/g;
	my $len = length($seq);
	$offset += $cll - 2;
	$seq = substr($seq, $cll - 1, $clr - $cll + 1);
	
	my $i = 0;
	my $asml = $offset + 1;
	my $asmr = $asml + $clr - $cll + 1;
	while ($i <= $#gaps && $offset > $gaps[$i]){
	    $asml--; $asmr--; $i++;
	} # get rid of gaps from offset here
	while ($i <= $#gaps && $offset + $clr - $cll + 1 > $gaps[$i]){
	    $asmr--; $i++;
	}

	if ($rc{$seqName} eq "C"){ # make coordinates with respect to forw strand
	    $cll = $len - $cll + 1;
	    $clr = $len - $clr + 1;
	    my $tmp = $cll;
	    $cll = $clr;
	    $clr = $tmp;
	}

	while ($seq =~ /-/g){ #make $clr ungapped
	    $clr--;
	}

	if ($rc{$seqName} eq "C"){
	    my $tmp = $cll;
	    $cll = $clr;
	    $clr = $tmp;
	}
	
	printSequenceRecord(\*CONTIG, $seqName, $seq, $offset, (($rc{$seqName} eq "U")?"" : "RC"), $cll, $clr, $asml, $asmr, "contig");
	next;
    }
}

close(CONTIG);
close(ACE);
exit(0);
