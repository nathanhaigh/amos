#!/usr/local/bin/perl

# $Id$
#
# Converts from an AMOS assembly file to a new .ace file
#

use strict;
use TIGR::Foundation;
use AMOS::AmosLib;

my $base = new TIGR::Foundation;
my $GREP = "/bin/grep";

if (! defined $base){
    die ("Foundation cannot be created.  FATAL!\n");
}

my $VERSION = '$Revision$ ';
$base->setVersionInfo($VERSION);

my $HELPTEXT = q~
   amos2ace [opts] [infile1] [infile2] ...
       
   Options:   
        -o <outfile>    Output file, by default <infile1>.ace
        -c <chromat_dir> Location of the chromatograms
        -p <phd_dir>    Location of the PHD directory

    The multiple input files must be presented in "proper" order - each message needs to
be defined before being referenced.
    ~;

$base->setHelpInfo($HELPTEXT);

my $infile;
my $outfile;
my $chromodir = "../chromat_dir";
my $phddir = "../phd_dir";
my $frgfile;

my $err = $base->TIGR_GetOptions(
				 "o=s" => \$outfile,
				 "c=s" => \$chromodir,
				 "p=s" => \$phddir
				 );

if (! $err){
    $base->bail("Error processing options!");
}

if ($#ARGV < 0){
    $base->bail("At least one input file must be specified");
}

my $prefix;

$ARGV[0] =~ /(.*)\.[^.]*$/;
$prefix = $1;

if (! defined $outfile){
    $outfile = "$prefix.ace";
}

# get rid of trailing slashes
$chromodir =~ s/\/+$//;
$phddir =~ s/\/+$//;


# Here's the process
#
# * get contig information and generate contig record in contig temp file
# * for each of the sequences generate sequence information and store coords in
# contig writing sequence records in another sequence temp file
# * generate tiling info and concatenate both temp files into an output temp
# file
#
# * when all contigs are done generate output file header and concatenate
# output temp file to output. 


# counters for the useful information
my $nContigs = 0;
my $nReads = 0;

my $fr;


my $TMP = $$ . time();
my $ctgTmp = "$TMP.CTG.TMP";
my $seqTmp = "$TMP.SEQ.TMP";
my $outTmp = "$TMP.OUT.TMP";

open(OUT, ">$outTmp") ||
    $base->bail("Cannot open output temp \"$outTmp\" : $!");

my $nseqs;     # number of sequences in this contig
my $seenseqs;  # number of sequences we've actually seen
my $contigid;  # contigId
my $seqName;
my %seqClr;    # clear range from seq file
my %seqAlnClr; # clear range from .asm file
my %seqAlnRng; # assembly range from .asm file
my %seqOff;    # sequence offset
my %rend;      # right end of sequences
my $al;        # aligned coordinates (asm_lend, rend)
my $ar;
my $end5;      # clear range
my $end3; 
my $contigLen; # # bases in the contig
my $contigSeq;

my %seqpos;
my %seqnames;
my %seqfile;

my @files;

my %seqclr;
for (my $f = 0; $f <= $#ARGV; $f++){
    open($files[$f], $ARGV[$f]) || $base->bail("Cannot open $ARGV[$f]: $!");
    
    my $seekpos = tell $files[$f];

    while (my $record = getRecord($files[$f])){
	my ($rec, $fields, $recs) = parseRecord($record);
	my $nseqs;
	my $id = $$fields{iid};
	$contigid = $$fields{iid};
	
	if ($rec eq "RED"){
	    $seqpos{$$fields{iid}} = $seekpos;
	    $seqfile{$$fields{iid}} = $f;
	    if (! exists $$fields{eid} || $$fields{eid} =~ /^\s*$/){
		$seqnames{$$fields{iid}} = $$fields{iid};
	    } else {
		$seqnames{$$fields{iid}}= $$fields{eid};
		$seqnames{$$fields{iid}} =~ s/^\s+//; # clean up any initial spaces
		if ($seqnames{$$fields{iid}} =~ /^(\S+)\s+.*$/) {
		    # if the eid is a space-separated name, use just the first token
		    $seqnames{$$fields{iid}} = $1;
		}
	    }
	    $seqclr{$$fields{iid}} = $$fields{clr};
#	    print STDERR "setting clear range for $$fields{iid} to $$fields{clr}\n";
	} elsif ($rec eq "CTG"){
	    my $seq = $$fields{seq};
	    my @lines = split('\n', $seq);
	    $seq = join ('', @lines);
	    $seq =~ s/-/*/g;
	    $contigLen = length($seq);
	    my @offsets;
	    my $coord;
	    $#offsets = length($seq) - 1;
	    for (my $i = 0; $i < length($seq); $i++){
		if (substr($seq, $i, 1) ne "*"){
		    $coord++;
		}
		$offsets[$i] = $coord;
	    }

	    my $qual = $$fields{qlt};
	    @lines = split('\n', $qual);
	    $qual = join('', @lines);
	    open(CTGOUT, ">$ctgTmp")||
		$base->bail("Cannot open contig temp $ctgTmp: $!\n");

	    for (my $i = 0; $i < length($seq); $i += 50){
		print CTGOUT substr($seq, $i, 50), "\n";
	    }
	    print CTGOUT "\n";
	    $nContigs++;
	    
	    print CTGOUT "BQ\n";
	    my @qualvals;
	    # .ace qualities are only assigned to unpadded bases
	    for (my $i = 0; $i < length($qual); $i++){
		if (substr($seq, $i, 1) ne "*"){
		    push(@qualvals, ord(substr($qual, $i, 1)) - ord('0'));
		}
	    }
	    for (my $i = 0; $i <= $#qualvals; $i+=50){
		my $end = $i + 49;
		if ($end > $#qualvals){
		    $end = $#qualvals;
		}
		print CTGOUT " ", join(" ", @qualvals[$i .. $end]), "\n";
	    }
	    
	    print CTGOUT "\n";

	    %seqClr = ();
	    %seqAlnClr = ();
	    %seqAlnRng = ();
	    %seqOff = ();
	    %rend = ();
	    open(SEQOUT, ">$seqTmp") ||
		$base->bail("Cannot open seq temp $seqTmp: $!");
	    
	    for (my $r = 0; $r <= $#$recs; $r++){
		my ($srec, $sfields, $srecs) = parseRecord($$recs[$r]);
		my $seql;
		my $seqr;
		my $sequence;
		if ($srec eq "TLE"){
		    $nReads++;
		    $nseqs++;
		    if (! exists $$sfields{src}){
			$base->bail("Error: TLE record contains no src: field\n");
		    }
		    $seqName = $seqnames{$$sfields{src}};
		    $sequence = get_seq($seqfile{$$sfields{src}}, $$sfields{src});
		    @lines = split('\n', $sequence);
		    $sequence = join('', @lines);
		    ($seql, $seqr) = split(',', $seqclr{$$sfields{src}});
#		    print STDERR "sequence $$sfields{src} has range $seql, $seqr\n";
		    my @gaps = split(/\s+/, $$sfields{gap});
		    my ($asml, $asmr) = split(',', $$sfields{clr});

#		    print STDERR "asml and asmr are $asml $asmr\n";
		    if ($asml < $asmr){
			$asmr -= $asml;
			$asml = 0;
		    } else {
			$asml -= $asmr;
			$asmr = 0;
		    }
#		    print STDERR "asml and asmr are $asml $asmr\n";
		    $asml += $$sfields{off};
		    $asmr += $$sfields{off};
#		    print STDERR "asml and asmr are $asml $asmr\n";

		    my $left = $seql;
		    if ($asml > $asmr){
			$sequence  = reverseComplement($sequence);
			my $tmp = $asmr;
			$asmr = $asml;
			$asml = $tmp;
			
			$tmp = $seqr;
			$seqr = $seql;
			$seql = $tmp;
			$left = length($sequence) - $seql;
		    }
#		    print STDERR "asml and asmr are $asml $asmr\n";
		    # now we add gaps to the sequence
		    my $outseq = "";
		    my $gapindex = 0;
#		    print STDERR "have ", $#gaps + 1, " gaps and ", length($sequence), " bases\n";
		    for (my $j = 0; $j < length($sequence); $j++){
			my $seqj = $j - $left;# + $seql{$id} - 1; # index in untrimmed sequence
#			my $seqj = $j; # if index in trimmed sequence
			if ($gapindex <= $#gaps && $seqj > $gaps[$gapindex]){
			    print STDERR "Weird $seqnames{$id}, $seqj > $gaps[$gapindex]\n";
			}
			while ($gapindex <= $#gaps && $seqj == $gaps[$gapindex]){
			    $outseq .= "*";
			    $asmr++;
			    $gapindex++;
			}
			$outseq .= substr($sequence, $j, 1);
		    }
		    $seqAlnRng{$seqName} = sprintf("%d %d", $asml + 1, $asmr);
		    $seqAlnClr{$seqName} = sprintf("%d %d", 
						   (($seql < $seqr)?$seql + 1:$seql), 
						   (($seql < $seqr)?$seqr : $seqr + 1));
		    my $off = $$sfields{off};
		    my $ori = ($seql > $seqr) ? "C" : "U";

		    $seqOff{$seqName} = $asml;
		    $rend{$seqName} = $asmr;
		    
		    $seqOff{$seqName}++;
		    if ($ori eq "C"){
			$off -= length($sequence) - $seql;
		    } else {
			$off -= $seql;
		    }
		    
		    $off++;
		    
		    print CTGOUT "AF $seqName $ori $off\n";
		    print SEQOUT "RD $seqName ", length($outseq), " 0 0\n";
		    for (my $i = 0; $i <= length($outseq); $i += 50){
			print SEQOUT substr($outseq, $i, 50), "\n";
		    }
		    print SEQOUT "\n";

		    my $end5;
		    my $end3;
		    if ($ori eq "C"){
			$end5 = length($sequence) - $seql;
			$end3 = length($outseq) - $seqr;
		    } else {
			$end5 = $seql;
			$end3 = length($outseq) - length($sequence) + $seqr;
		    }
		    $end5++; #all coordinates are 1 based
		    print SEQOUT sprintf("QA %d %d %d %d\n", 
					 $end5, $end3, $end5, $end3);
		    my $chrmfile = $chromodir . "/$seqName";
		    my $phdfile = $phddir . "/$seqName.phd.1";
		    my $time;
		    
		    if (-r $phdfile){
			$time = `$GREP TIME $phdfile`;
			$time =~ s/TIME: //;
		    }
		    
		    if (! defined $time){
			if (-e $phddir) {
			    $base->logError("Cannot stat phd file \"$phdfile\"", 1);
			}
			$time = localtime;
		    } 
		    
		    my $dir = ($ori eq "C") ? "rev" : "forw";
		    
		    print SEQOUT "DS CHROMAT_FILE: $chrmfile PHD_FILE: $phdfile TIME: $time\n"; # CHEM: unknown DYE: big TEMPLATE: $seqName DIRECTION: $dir\n";
		} # if TILE
	    } # for each tile
	    close(SEQOUT);
	    my $prev;
	    my $nBS = 0;

	    my @offsets = keys  %seqOff;
#	    print STDERR " I have ", $#offsets + 1, " offsets\n";
 	    foreach my $sequence ( sort {
		($seqOff{$a} == $seqOff{$b}) ? 
		    ($rend{$b} <=> $rend{$a}) : 
		    ($seqOff{$a} <=> $seqOff{$b})
		} (keys %seqOff)) {
#		print STDERR "${sequence}($seqOff{$sequence}), ";
 		if (defined $prev) {
 		    if ($seqOff{$sequence} - 1 < $seqOff{$prev} ||
			$rend{$sequence} < $rend{$prev}){
 			next;
 		    }
 		    $nBS++;
 		    print CTGOUT "BS $seqOff{$prev} ", $seqOff{$sequence} - 1, " $prev\n";
 		}
 		$prev = $sequence;
	    }
#	    print STDERR "\n";
 	    $nBS++;
 	    print CTGOUT "BS $seqOff{$prev} $contigLen $prev\n";
	    close(CTGOUT);
	    
	    print OUT "CO $contigid $contigLen $nseqs $nBS U\n";
	    open(CTGOUT, "$ctgTmp") ||
		$base->bail("Cannot open \"$ctgTmp\": $!");
	    while (<CTGOUT>){
		print OUT;
	    }
	    close(CTGOUT);
	    print OUT "\n";
	    open(SEQOUT, "$seqTmp") ||
		$base->bail("Cannot open \"$seqTmp\": $!");
	    while (<SEQOUT>){
		print OUT;
	    }
	    close(SEQOUT);
	    print OUT "\n";
	} # if CTG
	$seekpos = tell $files[$f];
    } ## while getrecord
}
close(OUT);
	
# now we need to create the output file and attach to it the temp file.
open(OUT, ">$outfile") ||
    $base->bail("Cannot open output file \"$outfile\": $!");

print OUT "AS $nContigs $nReads\n\n";

open(TMP, "$outTmp") ||
    $base->bail("Cannot open temp file \"$outTmp\" : $!");
while (<TMP>){
    print OUT;
}
close(TMP);

# now print some info
print OUT "WA{\n";
print OUT "amos2ace $VERSION\n";
my $time = `date`;
chomp $time;
print OUT "Run by $ENV{USER} on $time\n";
print OUT "}\n";
close(OUT);

# then remove all the temp files
unlink($outTmp, $seqTmp, $ctgTmp);

exit(0);

# gets a sequence by Id from a file;
sub get_seq
{
    my $file = shift;
    my $id = shift;

    seek $files[$file], $seqpos{$id}, 0; # seek set
    my $record = getRecord($files[$file]);
    if (! defined $record){
        print "weird error\n";
        return;
    }

    my ($rec, $fields, $recs) = parseRecord($record);
    
    if ($rec ne "RED"){
        print STDERR "weird error in get_seq for read $id, expecting RED not $rec at position $seqpos{$id} in file $file\n";
        return;
    }
    if ($$fields{iid} != $id){
        print STDERR "weird error in get_seq, expecting $id, got $$fields{acc}\n
";
        return;
    }
    return $$fields{seq};
}
