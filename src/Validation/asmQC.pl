#!/usr/local/bin/perl

#asmQC.pl      - checks mate-pair satisfiability and produces both reports and
# optionally a .cam file

use TIGR::Foundation;

$base = new TIGR::Foundation;

if (! defined $base){
    print STDERR "Aye carramba\n";
    exit(1);
}

my $LIBLIMIT = 300;
my $MINSEQS = 0;   # minimum cluster size to be considered

my $ctgfile;
my $db;
my $libfile;
my $outfile;
my $matefile;
my $minCvg;
my $conservative;
my $camfile;
my $orffile;
my $libinfofile;

#  %report = ("LENS" => 1,
#  	   "LENH" => 1,
#  	   "ORI" => 1,
#  	   "OTHER" => 1,
#  	   "NOPAIR" => 1,
#  	   "MISC" => 1,
#  	   "MISL" => 1);

my %report = ("LENS" => 1,
	   "LENH" => 1,
	   "ORI" => 1,
	   "MISC" => 1,
	   "MISL" => 1,
	   "NOPAIR" => 1,
	   "OTHER" => 1);

my %stats = ("LENS" => 0,
	     "LENH" => 0,
	     "ORI" => 0,
	     "MISC" => 0,
	     "MISL" => 0,
	     "NOPAIR" => 0,
	     "OTHER" => 0);

my $helpText = qq~
    asmQC [opts]
     -i <file>  .contig input file (required)
     -L <lib>   use library data from <lib> file
     -o <outfile> name of output
     -cam <camfile> name of celamy output
     -orf <orffile> list of orfs
     -libout <libinfofile> library size summary
     -lens      do not cluster clones shorter than expected
     -lenh      do not cluster clones longer than expected
     -len       equivalent to both -lens and -lenh
     -ori       do not cluster clone orientation violations
     -misc      do not cluster primer reads in a different contig than their
                clone
     -misl      do not cluster primer reads outside the range of their clone
     -mis       both -misc and -misl
     -nopair    do not cluster unpaired reads
     -other     do not cluster reads with mates in other contigs
     -mates <file>  read pairing is specified in <file>
     -min <minseq> ignore all clusters smaller than <minseq> reads
     -cvg <mincvg> only show clusters with coverage deeper than <mincvg>
     -conservative expand clone size ranges in a conservative fashion
    ~;

$base->setHelpInfo($helpText);

$err = $base->TIGR_GetOptions("i=s" => \$ctgfile,
			      "L=s" => \$libfile,
			      "o=s" => \$outfile,
			      "cam=s" => \$camfile,
			      "orf=s" => \$orffile,
			      "lens" => \$nolens,
			      "lenh" => \$nolenh,
			      "len" => \$nolen,
			      "ori" => \$noori,
			      "misc" => \$nomisc,
			      "misl" => \$nomisl,
			      "mis" => \$nomis,
			      "nopair" => \$nopair,
			      "other" => \$noother,
			      "mates=s" => \$matefile,
			      "min=i" => \$MINSEQS,
			      "mincvg=i" => \$minCvg,
			      "conservative" => \$conservative,
			      "libout=s" => \$libinfofile
			      );

if (defined $nolens){
    delete $report{"LENS"};
}
if (defined $nolenh){
    delete $report{"LENH"};
}
if (defined $nolen){
    delete $report{"LENS"};
    delete $report{"LENH"};
}
if (defined $noori){
    delete $report{"ORI"};
}
if (defined $nomisc){
    delete $report{"MISC"};
}
if (defined $nomisl){
    delete $report{"MISL"};
}
if (defined $nomis){
    delete $report{"MISC"};
    delete $report{"MISL"};
}
if (defined $nopair){
    delete $report{"NOPAIR"};
}
if (defined $noother) {
    delete $report{"OTHER"};
}


if (! defined $ctgfile){
    $base->bail("You must provide a file with the -i option");
}

if (! defined $matefile){
    $base->bail("You must provide a mates file with the -mates option");
}

if (! defined $outfile){
    $base->bail("You must provide an output file name with the -o option");
}

my $camid = 0;
if (defined $camfile){
    open(CAM, ">$camfile") || $base->bail("Cannot open $camfile: $!\n");
    print CAM q~0goodColor: C00FF00 T1 S # good clone
0goodReadColor: C00FF00 T2 S # read_in_good clone
0shortColor: CFF0000 T1 S # short clone
0shortReadColor: CFF0000 T2 S # read_in_short clone
0longColor: CFF00FF T1 S # long clone
0longReadColor: CFF00FF T2 S # read_in_long clone
0noMateColor: C0000FF T2 S # no mate 
0MateExtColor: C0000FF T1 S # w/o mate extension
0orientColor: C800080 T1 D # misoriented mate clone
0orientReadColor: C800080 T2 S # read_in_misoriented clone
0backboneColor: C808080 T2 S # backbone
0orfColor: CFF8000 T2 S # orf 
~;
}


%libRange = ();
#my %libId = ();  # mapping between libIds and cat#
my %inserts;
my %forw;
my %rev;
my %seq2ins;
my %ins2lib;

my $insid = 1;

if (defined $matefile){
    open(MATES, $matefile) || $base->bail("Cannot open $matefile: $!\n");
    
    while (<MATES>){
        chomp;
        if (/^library/){
            my @recs = split('\t', $_);
	    
            if ($#recs < 3 || $#recs > 4){
                $base->logError("Improperly formated line $. in \"$matefile\".\nMaybe you didn't use TABs to separate fields\n", 1);
                next;
            }

	    $libRange{$recs[1]} = "$recs[2] $recs[3]"; 
            
            next;
        } # if library

	if (/^\#/) { # comment
            next;
        }

        if (/^\s*$/) { # empty line
            next;
        }

	if (/^pair/) { # regexp pair
	    # deal with this at a later time
	    next;
	}
        
        # now we just deal with the pair lines
        my @recs = split('\t', $_);
        if ($#recs < 1 || $#recs > 2){
            $base->logError("Improperly formated line $. in \"$matefile\".\nMaybe you didn't use TABs to separate fields\n");
            next;
        }

        my $insname = $insid++;

        if (defined $recs[2]){
            $ins2lib{$insname} = $recs[2];
        } else {
            $base->logError("$insname has no library\n");
        }
        
        $forw{$insname} = $recs[0];
        $rev{$insname} = $recs[1];
        
        $seq2ins{$recs[0]} = $insname;
        $seq2ins{$recs[1]} = $insname;
    } # while <MATES>

    close(MATES);

}

if (defined $libfile){
    open (LIB, $libfile) || $base->bail("Cannot open $libfile: $!");
    while (<LIB>){
	if (/^(\w+) (\d+) (\d+)/){
	    $libRange{$1} = "$2 $3";
	}
    }
    close(LIB);
}


if (defined $conservative){
    while (my ($lib, $range) = each %libRange){
	my ($low, $high) = split(' ', $range);
	print STDERR "Adjusting library $lib from [$low, $high] to ";
	my $mid = ($high + $low) / 2;
	$high = 2 * $high - $mid;
	if (2 * $low - $mid < 0){
	    $low /= 2;
	} else {
	    $low = 2 * $low - $mid;
	}
	print STDERR "[$low, $high]\n";
	$libRange{$lib} = "$low $high";
    }
}

open(IN, $ctgfile) || $base->bail("Cannot open $ctgfile: $!");

%len = ();
%seqCtg = ();
%cloneSeqs = ();
%range = ();

$nSeqs = 0;
$nContigs = 0;

if (defined $orffile && defined $camfile){
    open(ORF, "$orffile") || $base->bail("Cannot open $orffile: $!\n");
    while (<ORF>){
	chomp;
	my @flds = split('\t', $_);
	my $low;
	my $high;
	if ($flds[0] > $flds[1]){
	    $low = $flds[1];
	    $high = $flds[0];
	} else {
	    $low = $flds[0];
	    $high = $flds[1];
	}
	print CAM "${camid}orf: $low A0orfColor $high R1 \# $flds[2]\n";
	$camid++;
    }
}

while (<IN>){
    if (/^\#\#(\S+) \d+ (\d+)/){
	$contig = $1;
	$len{$contig} = $2;
	$nContigs++;
	print STDERR "doing contig $contig\n";
	next;
    }
    if (/^\#(\S+)\(\d+\).*\{(\d+ \d+)\} <(\d+ \d+)>/){
#	print STDERR "Got sequence $1\n";
	$seqCtg{$1} = $contig;
	$asmRange{$1} = $3;
	$seqRange{$1} = $2;
#	$cloneSeqs{get_clone($1)} .= "$1 ";
	$nSeqs++;
	next;
    }
}
close(IN);

%bad = ();

$allClones = 0;
$badPair = 0;
$badOther = 0;
$badOri = 0;
$badLenS = 0;
$badLenH = 0;
$badMisC = 0;
$badMisL = 0;

$libRange{"unmated"} = "100 1000";

while (my ($seq, $ctg) = each %seqCtg){
    if (! exists $seq2ins{$seq}){
	my $iid = $insid++;
	$seq2ins{$seq} = $iid;
	$forw{$iid} = $seq;
	$ins2lib{$iid} = "unmated";
    }
}



while (($ins, $lib) = each %ins2lib){
    my $forw = $forw{$ins};
    my $rev = $rev{$ins};
    my $clnseqs = "";
    if (defined $forw){$clnseqs .= "$forw ";}
    if (defined $rev) {$clnseqs .= "$rev ";}

    if ((defined $forw && ! exists $seqCtg{$forw}) && 
	(defined $rev && ! exists $seqCtg{$rev})){next;}

    $allClones++;

    if ($forw eq "" || $rev eq ""){ # unpaired ends
	if ($forw ne ""){
	    ($minCln, $maxCln) = split(' ', $asmRange{$forw});
	}
	if ($rev ne ""){
	    ($minCln, $maxCln) = split(' ', $asmRange{$rev});
	}
	if ( (($forw ne "" && exists $asmRange{$forw}) || 
	      ($rev ne "" && exists $asmRange{$rev})) && 
	    defined $camfile && 
	    exists $report{"NOPAIR"}){
	    my $l; my $r;
	    if ($rev ne ""){
		($l, $r) = split(' ', $seqRange{$rev});
		($libLow, $libHi) = split(' ', $libRange{$lib});
	    } else {
		($l, $r) = split(' ', $seqRange{$forw});
		($libLow, $libHi) = split(' ', $libRange{$lib});
	    }
	    my $libMed = int (($libLow + $libHi) / 2);
	    if ($l < $r) 
	    { #rest of clone is to the right
		my $rightend = $libMed + $minCln;
		if ($rightend <= $maxCln) {$rightend = $maxCln + 1;}
		print CAM "${camid}$cln:$minCln A0noMateColor $maxCln A0MateExtColor $rightend R2 \# no mate for $cln $forw $rev\n";
	    } else { # rest of clone is to the left
		my $leftend = $maxCln - $libMed;
		if ($leftend >= $minCln) {$leftend = $minCln - 1;}
		print CAM "${camid}$cln:$leftend A0MateExtColor $minCln A0noMateColor $maxCln R2 \# no mate for $cln $forw $rev\n";
	    }
	    $camid++;
	}
	markBad($clnseqs, "NOPAIR");
	next;
    }

    if (exists $seqCtg{$forw} && exists $seqCtg{$rev} && $seqCtg{$forw} != $seqCtg{$rev}){ # ends belong to different contigs
	markBad($clnseqs, "OTHER");
	next;
    }
	
    if (! exists $asmRange{$forw} || ! exists $asmRange{$rev}){
	next;
    }
    
    ($fal, $far) = split(' ', $asmRange{$forw});
    ($ral, $rar) = split(' ', $asmRange{$rev});

    print STDERR "CLONE $cln  $fal, $far - $ral, $rar\n";

    $minCln = 0; $maxCln = 0;
    if ($fal < $ral){ # forward is to the left of rev
	$minCln = $fal;	$maxCln = $rar;
	$lowCln = $far; $highCln = $ral;
	if ($lowCln > $highCln){
	   $lowCln = $highCln = int(($far + $ral) / 2);
           $highCln++;
	}
	if (getOri($forw) != 1 || getOri($rev) != -1){
	    markBad($clnseqs, "ORI");
	    my $forwori = (getOri($forw) == 1) ? ">" : "<";
	    my $revori = (getOri($rev) == 1) ? ">" : "<";
	    if (defined $camfile && exists $report{"ORI"}){
		print CAM "${camid}$cln: $minCln A0orientColor $maxCln C1 \# mis-oriented $cln ($forw $forwori $revori $rev)\n";
		$camid++;
	    }
	    next;
	}
    } else { # forward is to the right
	$minCln = $ral; $maxCln = $far;
	$lowCln = $rar; $highCln = $fal;
	if ($lowCln > $highCln){
	   $lowCln = $highCln = int(($rar + $fal) / 2);
           $highCln++;
	}
	if (getOri($forw) != -1 || getOri($rev) != 1){
	    markBad($clnseqs, "ORI");
	    my $forwori = (getOri($forw) == 1) ? ">" : "<";
	    my $revori = (getOri($rev) == 1) ? ">" : "<";
	    if (defined $camfile && exists $report{"ORI"}){
		print CAM "${camid}$cln: $minCln A0orientColor $maxCln C1 \# mis-oriented $cln ($rev $revori $forwori $forw)\n";
		$camid++;
	    }
	    next;
	}
    }

    ($libLow, $libHi) = split(' ', $libRange{$lib});

    if ($maxCln - $minCln < $libLow){ 
	markBad($clnseqs, "LENS");
	my $len = $maxCln - $minCln;
	print STDERR "$cln ", $lib, " $minCln, $maxCln < $libLow, $libHi\n";
	if (defined $camfile){
	    if ($highCln < $lowCln){
		print CAM "${camid}$cln: $minCln A0shortReadColor $maxCln R2 \# short $cln ($rev $forw) $len < $libLow\n";
	    } else {
		print CAM "${camid}$cln: $minCln A0shortReadColor $lowCln A0shortColor $highCln A0shortReadColor $maxCln R2 \# short $cln ($rev $forw) $len < $libLow\n";
	    }
	    $camid++;
	}
	next;
    }
    if ($maxCln - $minCln > $libHi) {
	my $len = $maxCln - $minCln;
	markBad($clnseqs, "LENH");
	print STDERR "$cln ", $lib, " $minCln, $maxCln > $libLow, $libHi\n";
	if (defined $camfile){
	    print CAM "${camid}$cln: $minCln A0longReadColor $lowCln A0longColor $highCln A0longReadColor $maxCln R3 \# long $cln ($rev $forw) $len > $libHi\n";
	    $camid++;
	}
	next;
    }

    if (defined $camfile){
	print CAM "${camid}$cln: $minCln A0goodReadColor $lowCln A0goodColor $highCln A0goodReadColor $maxCln R2 \# good $cln ($rev $forw)\n";
	$camid++;
    }

    @clns = split (' ', $clnseqs);
    $ctg = $seqCtg{$forw};
    for ($i = 0; $i <= $#clns; $i++){
	if ($seqCtg{$clns[$i]} != $ctg){
	    markBad($clnseqs, "MISC");
	    last;
	}
	($l, $r) = split(' ', $asmRange{$clns[$i]});
	if ($l < $minCln ||
	    $r > $maxCln){
	    markBad($clnseqs, "MISL");
	    last;
	}
    }
}

if (defined $camfile){
    close(CAM);
}

while (($bseq, $type) = each %bad){
    if (! exists $report{$type}){
	delete $stats{$type};
	print STDERR "skipping type $type\n";
	next;
    }
    
    print STDERR "detected baddie for contig ", $seqCtg{$bseq}, "\n";
    $badCtg{$seqCtg{$bseq}} .= "$bseq ";
    
    ($asml{$bseq}, $asmr{$bseq}) =  split(' ', $asmRange{$bseq});
}

$clustid = 0;

open(OUT, ">$outfile") || $base->bail("Cannot open $outfile: $!");
print OUT "graph G {\n";
print OUT "  rankdir = LR\n";

my %small;   # array of small clusters

while (($ctg, $seqs) = each %badCtg){
    print STDERR "examining baddies for contig $ctg\n";
    @seqs = split (' ', $seqs);
    @seqs = sort {$asml{$a} <=> $asml{$b}} @seqs;

    $range = -1;
    my $nseqs = 0;
    for ($i = 0; $i <= $#seqs; $i++){
	if ($asml{$seqs[$i]} > $range){
	    print STDERR "Cluster $clustid has $nseqs sequences\n";
	    if ($range != -1 && $nseqs < $MINSEQS){ # mark all small clusters
		# but only if it's not the first one.  That one gets chopped
		# by default
		print STDERR "Marking cluster $clustid as small\n";
		$small{$clustid} = 1;
	    }
	    $nseqs = 0;
	    $clustid++;
	    $clustContig{$clustid} = $ctg;
	    $cluster{$clustid} .= "$seqs[$i] ";
	    $clustleft{$clustid} = $asml{$seqs[$i]};
	    $clustright{$clustid} = $asmr{$seqs[$i]};
	} else {
	    $cluster{$clustid} .= "$seqs[$i] ";
	    if ($asmr{$seqs[$i]} > $clustright{$clustid}){
		$clustright{$clustid} = $asmr{$seqs[$i]};
	    }
	}
	$nseqs++;
	$range = $clustright{$clustid};
	$cloneClust{get_clone($seqs[$i])}{$clustid} = 1;
	$reasons{$clustid}{$bad{$seqs[$i]}}++;

    }
    if ($nseqs < $MINSEQS){
	print STDERR "Marking cluster $clustid as small\n";
	$small{$clustid} = 1;
    }
}

print STDERR "found $clustid clusters\n";

while (($clone, $clusts) = each %cloneClust){
    @cl = keys %$clusts;

    if ($#cl > 0){ # more than 2 clusters
	for ($i = 1; $i <= $#cl; $i++){
	    if ($cl[$i - 1] < $cl[$i]){
		$mate = $cl[$i - 1] . " " . $cl[$i];
	    } else {
		$mate = $cl[$i] . " " . $cl[$i - 1];
	    }
	    $edges{$mate}++;
	}
    }
}

$prevcontig = 0;
for ($c = 1; $c <= $clustid; $c++){
    if (exists $small{$c}){
	print STDERR "Skipping cluster $c\n";
	next; # skip small cluster
    }

    my $OK = 0;
    my @coverage;
    if (defined $minCvg){
	@coverage = getCoverage($cluster{$c});
	for (my $cv = 0; $cv <= $#coverage; $cv++){
	    my ($l, $r, @cvseqs) = split(' ', $coverage[$cv]);
	    if ($#cvseqs + 1 >= $minCvg){
		$OK = 1;
	    }
	}
    }
    if (! $OK){
	next;
    }
    print "Cluster $c $clustContig{$c} [$clustleft{$c}, $clustright{$c}] ";
    while (($reason, $n) = each %{$reasons{$c}}){
	print "$reason($n) ";
    }
    print "\n";
    @seqs = split(' ', $cluster{$c});
    @seqs = map {$_ .= "(" . $bad{$_} . ")"} @seqs;
    $SPAN=7;
    for ($j = 0; $j <= $#seqs; $j += $SPAN){
	print join(" ", @seqs[$j .. $j+ $SPAN]), "\n";
    }

    #$width = 1.0 / 72 * ($clustright{$c} - $clustleft{$c});
    $width = 1;
    if ($width < 0.5) {$width = 0.5};
    print OUT "  $c [label=\"$clustContig{$c}\\n$clustleft{$c} - $clustright{$c}\", shape=\"box\", height=\".1\", width=\"$width\"];\n";
    if ($c > 1 && $clustContig{$c} == $clustContig{$c - 1}){
	print OUT "  ", $c - 1, " -- $c [style=\"invis\", weight=2];\n";
    }

    @others = grep (/^$c | $c$/, keys %edges);
    @others = sort {$edges{$b} <=> $edges{$a}} @others;
    if ($#others >= 0){
	for ($j = 0; $j <= $#others; $j++){
	    $others[$j] =~ /^$c (\d+)|(\d+) $c$/;
	    if (defined $1){
		$other = $1;
	    } elsif (defined $2){
		$other = $2;
	    } else {
	        print STDERR "WIERD\n";
		next;
	    }
	    print "$edges{$others[$j]} links to $other $clustContig{$other} [$clustleft{$other}, $clustright{$other}]\n";
	}
    }
    print "\n";

    print "COVERAGE SUMMARY:\n";
    for (my $cv = 0; $cv <= $#coverage; $cv++){
	my ($l, $r, @cvseqs) = split(' ', $coverage[$cv]);
	if ($#cvseqs + 1 < $minCvg){
	    next;
	}
	print "Coverage region $cv: [$l, $r] ", $#cvseqs + 1, "x\n";
	@cvseqs = map {$_ .= "(" . $bad{$_} . ")"} @cvseqs;
	print join(" ", @cvseqs), "\n";
    }
}

while (($mates, $num) = each %edges){
    ($a, $b) = split(' ', $mates);
    if (exists $small{$a} || exists $small{$b}){
	next; # skip small clusters
    }
    print OUT "  $a -- $b [label=\"$num\"];\n";
}



print OUT "}\n";
close(OUT);

my $badPair = $stats{"NOPAIR"};
my $badOther = $stats{"OTHER"};
my $badOri = $stats{"ORI"};
my $badLenS = $stats{"LENS"};
my $badLenH = $stats{"LENH"};
my $badMisC = $stats{"MISC"};
my $badMisL = $stats{"MISL"};

print "Summary of findings\n";
print "-------------------\n";
print "# Contigs:  $nContigs\n";
print "# Seqs:     $nSeqs\n";
print "# Clones:   $allClones\n";
print "# unpaired: $badPair (", sprintf("%.2f", $badPair * 100.0 / $allClones),
    ")\n";
print "# linking:  $badOther (", sprintf("%.2f", $badOther * 100.0 / $allClones),
    ")\n";
print "# orient:   $badOri (", sprintf("%.2f", $badOri * 100.0 / $allClones),
    ")\n";
print "# <length:  $badLenS (", sprintf("%.2f", $badLenS * 100.0 / $allClones),
    ")\n";
print "# >length:  $badLenH (", sprintf("%.2f", $badLenH * 100.0 / $allClones),
    ")\n";
print "# mispl. ctg.: $badMisC (", sprintf("%.2f", $badMisC * 100.0 / $allClones),
    ")\n";
print "# mispl. rng.: $badMisL (", sprintf("%.2f", $badMisL * 100.0 / $allClones),
    ")\n";
$totBad = $badPair + $badOther + $badOri + $badLenS + $badLenH + $badMisC + $badMisL;
print "total bad:  $totBad (", sprintf("%.2f", $totBad * 100.0 / $allClones),
    ")\n";

exit(0);


#############################################################


# marks the sequences given as parameter as bad
sub markBad
{
    my $which = shift;
    my $why = shift;

    $stats{$why}++;
    my @seqs = split(' ', $which);
    for (my $i = 0; $i <= $#seqs; $i++){
	$bad{$seqs[$i]} = $why;
	print STDERR "marked baddie ($seqs[$i]<$asmRange{$seqs[$i]}>, $why) for contig ", 
	$seqCtg{$seqs[$i]}, "\n";
    }
}

# returns the orientation of a read in a contig as +1 or -1
sub getOri
{
    my $seq = shift;

    my ($l, $r) = split(' ', $seqRange{$seq});
    if ($l < $r) {
	return 1;
    } else {
	return -1;
    }
}

# returns the library for a sequence
sub get_lib
{
    my $seq = shift;

    return $libId{substr($seq, 0, 4)};
}

# returns the clone for a sequence
sub get_clone
{
    my $seq = shift;

    return substr($seq, 0, 7);
}

# takes a blank delimited string of names belonging to the
# same clone and returns the forward and reverse read names
# for this clone
sub process_clone
{
    my $seq = shift;
    my @seqs = split(' ', $seq);
    
    my $forw = "";
    my $rev = "";
    for (my $i = 0; $i <= $#seqs; $i++){
	if (length($seqs[$i]) > 10){
	   # print STDERR "$seqs[$i]  looks like a primer walk\n";
	    next;
	}
	if (substr($seqs[$i], 7, 1) eq "F" ||
	    substr($seqs[$i], 8, 1) eq "F"){
	    if ($forw eq "" || $seqs[$i] gt $forw){
		$forw = $seqs[$i];
	    }
	} elsif (substr($seqs[$i], 7, 1) eq "R" ||
		 substr($seqs[$i], 8, 1) eq "R") {
	    if ($rev eq "" || $seqs[$i] gt $rev){
		$rev = $seqs[$i];
	    }
	} else {
	   # print STDERR "$seqs[$i] is neither\n";
	}
    } # for $i in $#seqs

    return ($forw, $rev);
}

###############
# getCoverage
#
# takes as input a list of sequences, assuming that $asml, $asmr are defined
# The output contains an array of strings, one for each coverage region,
# containing the following information:
#
# "lend rend seq_1 seq_2 ... seq_cvg"
# where lend and rend are the coordinates within the assembly and are followed
# by the current set of sequences forming the coverage
#
# to parse this array you can do something like:
#
# ($lend, $rend, @seqs) = split(' ', $cvgarray[$i]);
# print "coverage is ", $#seqs + 1, "\n";
# 
sub getCoverage
{
    my $seqs = shift;
    my @seqs = split(' ', $seqs);
    my %events;

    print STDERR "Found ", $#seqs + 1, " sequences\n";
    for (my $i = 0; $i <= $#seqs; $i++){
	my $l = $asml{$seqs[$i]};
	my $r = $asmr{$seqs[$i]};
	while (exists $events{$l}){
	    $l += 0.01;
	}
	while (exists $events{$r}){
	    $r += 0.01;
	}
	$events{$l} = "$seqs[$i] B";
	$events{$r} = "$seqs[$i] E";
    } # now the events array contains the coordinates for the start
    # and end of all the sequences
    
    my $first = 1;
    my @outarray;
    my %currentlist;
    my $l; 
    my $r;
    foreach my $event (sort {$a <=> $b} keys %events){
	# process the events in order of their x coordinate
	my ($seqname, $where) = split(' ', $events{$event});
	if ($first && $where ne "B"){
	    $base->bail("Starting with an end, how wierd");
	}
	if ($first){
	    $first = 0;
	    $currentlist{$seqname} = 1;
	    $l = $asml{$seqname};
	    next;
	} 
	if ($where eq "B"){ # a new sequence begins here
	    $r = $asml{$seqname};
	    push(@outarray, "$l $r " . join(" ", keys %currentlist));
	    $l = $asml{$seqname};
	    $currentlist{$seqname} = 1;
	    next;
	}
	if ($where eq "E"){ # a sequence ends here
	    $r = $asmr{$seqname};
	    push(@outarray, "$l $r " . join(" ", keys %currentlist));
	    $l = $asmr{$seqname};
	    delete $currentlist{$seqname};
	}
    }
    return @outarray;
} # getCoverage

