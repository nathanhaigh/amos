#!/usr/local/bin/perl

use strict;

use TIGR::Foundation;

my $base = new TIGR::Foundation;

if (! defined $base){
    die("Walk do not run to the nearest EXIT!\n");
}

my $VERSION = '${Revision}$ ';
my $HELP = "findChimeras -o ovlp_file -l len_file";

$base->setHelpInfo($HELP);
$base->setVersionInfo($VERSION);


my $ovlpfile;
my $lenfile;

my $ret = $base->TIGR_GetOptions("o=s" => \$ovlpfile,
				 "l=s" => \$lenfile);


my %seqnames;
my %seqlens;
my %olaps;
my %ovl;       # quick lookup if two fragments overlap
my %insert;    # insert of each sequence
my %exts;      # extensions of each sequence

if (! defined $lenfile) {
    $base->bail("Must provide a length file (see -h)");
}
open(LEN, "$lenfile") || $base->bail("Cannot open \"$lenfile\": $!");
my @fields;
my $insertname;
while (<LEN>){
    chomp;
    @fields = split('\t');
    $seqnames{$fields[0]} = $fields[1];
    $seqlens{$fields[0]} = $fields[2];
    $insertname = substr($fields[1], 0, 7);
    $insert{$fields[0]} = $insertname;
}

close(LEN);

if (! defined $ovlpfile) {
    $base->bail("Must provide an overlap file (see -h)");
}
open(OVLP, "$ovlpfile") || $base->bail("Cannot open \"$ovlpfile\": $!");

my $last;
my $this;
my $that;
my $ori;
my $start;
my $end;
while (<OVLP>){
    chomp;
    @fields = split(/\s+/);

    $this = $fields[1];
    $that = $fields[2];
    $ori = $fields[3];
    $start = $fields[4];
    $end = $fields[5];

    print STDERR "$this\r";
    
    if ($insert{$this} eq $insert{$that}) {
	next;
    } # skip same insert overlaps

    $last = $this;

    $ovl{"$this $that"} = 1;


    # first check the beginning
    if ($start <= 0) { 
	$start = 0;
	push(@{$exts{"$this B"}}, $that);  # that extends this at the beginning
    } 

    # then check the end
    if ($end >= 0) {
	$end = $seqlens{$this};
	push(@{$exts{"$this E"}}, $that); # that extends this at the end
    } else { # $end < 0
	$end = $seqlens{$this} + $end;
    }

    push(@{$olaps{$this}}, "$that $ori $start $end");
    
    print "$this $that $seqnames{$this} $seqnames{$that} $start $end $seqlens{$this}\n";
}

close(OVLP);

# now we look for "break points" - that is points through which no sequence
# extends
select STDOUT;
$| = 1;

my $end;
my $nconflicts = 0;
my $other;
my $j;
my $b;
my $bori;
my $bs;
my $be;
my $i;
my $a;
my $ovls;

while (($a, $ovls) = each %olaps){
    print STDERR "$a\r";
    for ($i = 0; $i <= $#{$ovls}; $i++){
	($b, $bori, $bs, $be) = split(' ', $$ovls[$i]);
	print "checking $a $seqnames{$a} $b $seqnames{$b} $bori $bs $be\n";
	if ($be < $seqlens{$a}) { # we check extension to the right
	    $end = ($bori eq 'N') ? "E" : "B";
	    for ($j = 0; $j <= $#{$exts{"$b $end"}}; $j++){
		# check all reads extending $b
		$other = ${$exts{"$b $end"}}[$j];
		print "end looking at $other\n";
		if ($insert{$other} eq $insert{$a}) {
		    # skip same insert reads
		    next;
		}
		if (exists $ovl{"$a $other"} ||
		    exists $ovl{"$other $a"}) {
		    $nconflicts = 0;
		    last; # we found an extension, this is not a conflict
		} else {
		    $nconflicts++;
		}
	    } # for my $j = ...
	    if ($nconflicts != 0){
		print "$a $seqnames{$a} has $nconflicts conflicts  with $b $seqnames{$b} at end $be\n";
	    }
	} # if $be < $seqlens
	$nconflicts = 0;
	if ($bs > 0 ) { # we check extension to the left
	    $end = ($bori eq 'N') ? "B" : "E";
	    for ($j = 0; $j <= $#{$exts{"$b $end"}}; $j++){
		# check all reads extending $b
		$other = ${$exts{"$b $end"}}[$j];
		print "beginning looking at $other\n";
		if ($insert{$other} eq $insert{$a}) {
		    # skip same insert reads
		    next;
		}
		if (exists $ovl{"$a $other"} ||
		    exists $ovl{"$other $a"}) {
		    $nconflicts = 0;
		    last; # we found an extension, this is not a conflict
		} else {
		    $nconflicts++;
		}
	    } # for $j <= $#exts
	    if ($nconflicts != 0){
		print "$a $seqnames{$a} has $nconflicts conflicts with $b $seqnames{$b} at beginning $bs\n";
	    }
	} # if $bs > 0
    } # for my $i...
} # while each olaps
print "DONE\n";
exit(0);  #Main ends here
