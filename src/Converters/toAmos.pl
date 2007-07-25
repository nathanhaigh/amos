#!/usr/bin/perl


use TIGR::Foundation;
use AMOS::AmosLib;
use AMOS::ParseFasta;
use XML::Parser;
use POSIX qw(tmpnam);
$ENV{TMPDIR} = ".";

use strict;

my $INCLUDE_SURROGATE = 0;
my $UTG_MESSAGES = 0;

# more to do
# 1. allow to specify clear ranges from a separate file
# 2. error if multiple sequence or multiple contig files provided

my $VERSION = '$Revision$ ';
my $HELP = q~
    toAmos (-m mates|-x traceinfo.xml|-f frg|-acc)
           (-c contig|-a asm [-S]|-ta tasm|-ace ace|-s fasta|-q qual) 
           (-arachne assembly.links|-scaff file.scaff)
           -o outfile 
           [-i insertfile | -map dstmap]
           [-gq goodqual] [-bq badqual]
           [-pos posfile] [-phd]

    toAmos is primarily designed for converting the output of an assembly
program into the AMOS format so that it can be stored in an AMOS bank.

    If you simply need a program to generate assembly inputs for one the 
AMOS-based assemblers (e.g. minimus or AMOS-cmp) use tarchive2amos. 

ASM File Options:
 -S Include Surrogate Unitigs as AMOS Contigs
 -utg Include all UTG Unitig messages as AMOS Contigs
~;

my $base = new TIGR::Foundation();
if (! defined $base) {
    die("A horrible death\n");
}


$base->setVersionInfo($VERSION);
$base->setHelpInfo($HELP);

my $matesfile;
my $traceinfofile;
my $ctgfile;
my $frgfile;
my $asmfile;
my $tasmfile;
my $acefile;
my $outfile;
my $fastafile;
my $qualfile;
my $insertfile;
my $libmap;
my $posfile;
my $GOODQUAL = 30;
my $BADQUAL = 10;
my $byaccession = undef;
my $phd_opt = undef;
my $arachne_scaff = undef;
my $scaffile = undef;

my $minSeqId = 1;  # where to start numbering reads
my $err = $base->TIGR_GetOptions("m=s"   => \$matesfile,
				 "x=s"   => \$traceinfofile,
				 "c=s"   => \$ctgfile,
				 "f=s"   => \$frgfile,
				 "a=s"   => \$asmfile,
				 "ta=s"  => \$tasmfile,
				 "ace=s" => \$acefile,
				 "o=s"   => \$outfile,
				 "i=s"   => \$insertfile,
				 "map=s" => \$libmap,
				 "arachne=s" => \$arachne_scaff,
				 "scaff=s"   => \$scaffile,
				 "gq=i"  => \$GOODQUAL,
				 "bq=i"  => \$BADQUAL,
				 "q=s"   => \$qualfile,
				 "s=s"   => \$fastafile,
				 "pos=s" => \$posfile,
				 "id=i"  => \$minSeqId,
				 "acc"   => \$byaccession,
				 "phd"   => \$phd_opt,
				 "S"     => \$INCLUDE_SURROGATE,
				 "utg"   => \$UTG_MESSAGES);

my $matesDone = 0;
my $readsDone = 0;

# this is where all my data live
my %contigs;    # contig ids to contig length map
my %seqids;     # seq_name to seq_id map
my %seqnames;   # seq id to seq name map
my %seqcontig;  # seq id to contig map
my %contigseq;  # contig id to sequence id list 
my %seq_range;  # seq id to clear range
my %asm_range;  # seq id to range within contig
my %contigcons; # contig id to consensus sequence

my %forw;       # insert to forw (rev resp) end mapping 
my %rev; 
my %libraries;  # libid to lib range (mean, stdev) mapping
my %insertlib;  # lib id to insert list
my %inserttype; # the type of insert, if undefined assume 'E'
my %seenlib;    # insert id to lib id map
my %seqinsert;  # sequence id to insert id map
my %libnames;   # lib id to lib name
my %ctgnames;   # ctg id to ctg name
my %ctgids;     # ctg name to ctg id
my %posidx;     # position of sequence in pos file

my $minCtgId = $minSeqId;  # where to start numbering contigs

my $outprefix;

if (! defined $outfile){
    die "You must specify an output file with option -o\n";
}
elsif ($outfile eq "-")
{
  open(OUT, ">&STDOUT") || $base->bail("Can't open stdout: $!\n");
}
else
{
  open(OUT, ">$outfile") || $base->bail("Cannot open $outfile: $!\n");
}


my $tmprefix = "tmp.$$"; #tmpnam();

open(TMPSEQ, ">$tmprefix.seq") 
    || $base->bail("Cannot open $tmprefix.seq: $!\n");
open(TMPCTG, ">$tmprefix.ctg") 
    || $base->bail("Cannot open $tmprefix.ctg: $!\n");
open(TMPSCF, ">$tmprefix.scf")
    || $base->bail("Cannot open $tmprefix.scf: $!\n");


#then figure out the mates
if (defined $frgfile){
    open(IN, $frgfile) || $base->bail("Cannot open $frgfile: $!\n");
    parseFrgFile(\*IN);
    close(IN);
    $matesDone = 1;
    $readsDone = 1;
}

if (defined $fastafile){
    open(IN, $fastafile) || $base->bail("Cannot open $fastafile: $!\n");
    if (defined $qualfile){
	open(QUAL, $qualfile) || $base->bail("Cannot open $qualfile: $!\n");
	parseFastaFile(\*IN, \*QUAL);
	close(QUAL);
    } else {
	parseFastaFile(\*IN);
    }
    close(IN);
    $readsDone = 1;
}

if (defined $posfile){
    open(IN, $posfile) || $base->bail("Cannot open $posfile: $!\n");
    parsePosFile(\*IN);
    close(IN);
}

if (defined $asmfile){
    $outprefix = $asmfile;
    open(IN, $asmfile) || $base->bail("Cannot open $asmfile: $!\n");
    parseAsmFile(\*IN);
    close(IN);
}

if (defined $ctgfile){
    $outprefix = $ctgfile;
    open(IN, $ctgfile) || $base->bail("Cannot open $ctgfile: $!\n");
    parseContigFile(\*IN);
    close(IN);
}

if (defined $tasmfile) {
    die("This option is not yet fully functional\n");
    
    $outprefix = $tasmfile;
    open(IN, $tasmfile) || $base->bail("Cannot open $tasmfile: $!\n");
    parseTAsmFile(\*IN);
    close(IN);
}

if (defined $acefile){
    $outprefix = $acefile;
    open(IN, $acefile) || $base->bail("Cannot open $acefile: $!\n");
    parseACEFile(\*IN);
    close(IN);
	if ($phd_opt) {
		open(IN, $acefile) || $base->bail("Cannot open $acefile: $!\n");
		parsePHDFiles(\*IN);
		close(IN);
	}


}

$outprefix =~ s/\.[^.]*$//;

# now it's time for library and mates information

if (defined $traceinfofile){
    $matesDone = 1;
    open(IN, $traceinfofile) || $base->bail("Cannot open $traceinfofile: $!\n");
    parseTraceInfoFile(\*IN);
    close(IN);
}

if (! $matesDone && defined $matesfile) { # the mate file contains either mates
    # or regular expressions defining them
    open(IN, $matesfile) || 
	$base->bail("Cannot open \"$matesfile\": $!\n");
    parseMatesFile(\*IN);
    close(IN);
} # if mates not done defined matesfile

if (defined $insertfile){
    open(IN, $insertfile) || $base->bail("Cannot open $insertfile: $!\n");
    parseInsertFile(\*IN);
    close(IN);
}

if (defined $libmap){
    open(IN, $libmap) || $base->bail("Cannot open $libmap: $!\n");
    parseLibMapFile(\*IN);
    close(IN);
}

if (defined $arachne_scaff){
    open(IN, $arachne_scaff) || $base->bail("Cannot open $arachne_scaff: $!\n");
    parseArachneScaff(\*IN);
    close(IN);
}

if (defined $scaffile){
    open(IN, $scaffile) || $base->bail("Cannot open $scaffile: $!\n");
    parseScaff(\*IN);
    close(IN);
}


close(TMPSEQ);
close(TMPCTG);
close(TMPSCF);

## make sure all reads have an insert
my %libid;
my %insid;
my $ll = $minSeqId++;
$libraries{$ll} = "0 0"; # dummy library for unmated guys
$libnames{$ll} = "unmated";
#$libid{$ll} = $ll;

while (my ($sid, $sname) = each %seqnames){
    if (! exists $seqinsert{$sid}){
	my $id = $minSeqId++;
	$seqinsert{$sid} = $id;
	$insid{$id} = $id;
	$seenlib{$id} = $ll;
#	$insertlib{$ll} .= "$id ";
	$forw{$id} = $sid;
    }
}

## here's where we output all the stuff

# first print out a pretty header message
my $date = localtime();

print OUT "{UNV\n";
print OUT "eid:afg\n";
print OUT "com:\n";
print OUT "generated by $0\n";
print OUT "$date\n";
print OUT ".\n";
print OUT "}\n";


# then print out one library at a time
while (my ($lib, $range) = each %libraries){
    my ($mean, $sd) = split(' ', $range);
    print OUT "{LIB\n";

    $libid{$lib} = $minSeqId++;
    print OUT "iid:$libid{$lib}\n";

    if (exists $libnames{$lib}){
	print OUT "eid:$libnames{$lib}\n";
    } else {
	print OUT "eid:$lib\n";
    }
    print OUT "{DST\n";
    print OUT "mea:$mean\n";
    print OUT "std:$sd\n";
    print OUT "}\n";
    print OUT "}\n";
}

# then all the inserts
while (my ($ins, $lib) = each %seenlib){
    print OUT "{FRG\n";
#    if ($ins =~ /^\d+$/){
#	print OUT "iid:$ins\n";
#	$insid{$ins} = $ins;
#    } else {
    $insid{$ins} = $minSeqId;
    print OUT "iid:", $minSeqId++, "\n";
    print OUT "eid:", $ins, "\n";
#    }
    if (! exists $libid{$lib}){ 
	die ("Have not seen library $lib yet: possible error in input\n");
    }
    print OUT "lib:$libid{$lib}\n";

    if ( exists $forw{$ins} && exists $rev{$ins} ) {
        print OUT "rds:$forw{$ins},$rev{$ins}\n";
    }

    if ( exists $inserttype{$ins} && $inserttype{$ins} eq "T") {
        print OUT "typ:T\n";
    } else {
        print OUT "typ:I\n";
    }
    print OUT "}\n";
}

# then all the reads
if (defined $posfile){
    open(POS, $posfile) || $base->bail("Cannot open $posfile: $!\n");
}
open(TMPSEQ, "$tmprefix.seq") 
    || $base->bail("Cannot open $tmprefix.seq: $!\n");

while (<TMPSEQ>){
    if (/^\#(\d+)/){
	my $rid = $1;
	print OUT "{RED\n";
	print OUT "iid:$rid\n";
	print OUT "eid:$seqnames{$rid}\n";
	print OUT "seq:\n";
	$_ = <TMPSEQ>;
	while ($_ !~ /^\#/){
	    print OUT;
	    $_ = <TMPSEQ>;
	}
	print OUT ".\n";
	print OUT "qlt:\n";
	$_ = <TMPSEQ>;
	while ($_ !~ /^\#/){
	    print OUT;
	    $_ = <TMPSEQ>;
	}
	print OUT ".\n";
	if (defined $posfile){
	    if (exists $posidx{$rid}){
		seek POS, $posidx{$rid}, 0;
		my $line = <POS>;
		chomp $line;
		my ($seqname, $ver, $poss) = split('\t', $line);
#		print "unpacking $poss\n";
		my @poss = unpack("(a4)*", $poss);
		@poss = map(hex, @poss);
#		print "got $#poss pieces\n";

		print OUT "bcp:\n";
		for (my $p = 0; $p <= $#poss; $p += 15){
		    print OUT join(" ", @poss[$p .. $p + 14]), "\n";
		}
		print OUT ".\n";
	    }# else {
#		print "What bcp: $rid $seqnames{$rid}\n";
#	    }
	} elsif (defined $phd_opt) {

	    if (exists $posidx{$rid}){
		my $pos = $posidx{$rid};

		print OUT "bcp:\n";
		my @posn = split(' ', $pos);
		for (my $i = 0; $i <= $#posn; $i+=15){
		    if ($i + 14 <= $#posn){
			print OUT join(" ", @posn[$i .. $i+14]), "\n";
		    } else {
			print OUT join(" ", @posn[$i .. $#posn]), "\n";
		    }
		}
		print OUT ".\n";
	    }
	}


	if  (! exists $seqinsert{$rid}){
	    die("Cannot find insert for $rid ($seqnames{$rid})\n");
	}
	if (! exists $insid{$seqinsert{$rid}}){
	    die ("cannot find insert id for insert $seqinsert{$rid}, sequence $rid, $seqnames{$rid}\n");
	}
	print OUT "frg:$insid{$seqinsert{$rid}}\n";
	my ($cll, $clr) = split(' ', $seq_range{$rid});
	print OUT "clr:$cll,$clr\n";
	print OUT "}\n";
    } else {
	$base->bail("Weird error at line $. in $tmprefix.seq");
    }
}
close(TMPSEQ);
if (defined $posfile){ close(POS);}

unlink("$tmprefix.seq") || $base->bail("Cannot remove $tmprefix.seq: $!\n");

# then all the contigs

open(TMPCTG, "$tmprefix.ctg") 
    || $base->bail("Cannot open $tmprefix.ctg: $!\n");

while (<TMPCTG>){
    if (/^\#(\d+) (.)/){
	my $cid = $1;
	my $sts = $2;
	print OUT "{CTG\n";
	print OUT "iid:$cid\n";
	print OUT "sts:$sts\n";
	if (exists $ctgnames{$cid}){
	    print OUT "eid:$ctgnames{$cid}\n";
	}
	print OUT "seq:\n";
	$_ = <TMPCTG>;
	while ($_ !~ /^\#/){
	    print OUT;
	    $_ = <TMPCTG>;
	}
	print OUT ".\n";
	print OUT "qlt:\n";
	$_ = <TMPCTG>;
	while ($_ !~ /^\#/){
	    print OUT;
	    $_ = <TMPCTG>;
	}
	print OUT ".\n";
	while (/^\#(\d+)/){
	    my $rid = $1;
	    my ($len, $ren) = split(' ', $asm_range{$rid});
	    my ($cl, $cr) = split(' ', $seq_range{$rid});
	    if (! exists $seq_range{$rid}){
		die ("No clear range for read $rid\n");
	    }
	    if (! exists $asm_range{$rid}){
		die ("No asm range for read $rid\n");
	    }
	    my $tmp;
	    if ($len > $ren){
		$tmp = $len;
		$len = $ren;
		$ren = $tmp;
		$tmp = $cr;
		$cr = $cl;
		$cl = $tmp;
	    }
	    print OUT "{TLE\n";
	    print OUT "src:$rid\n";
	    print OUT "off:$len\n";
	    print OUT "clr:$cl,$cr\n";
	    my $deltastring;
	    $_ = <TMPCTG>;
	    while ($_ !~ /^\#/){
		$deltastring .= $_;
		$_ = <TMPCTG>;
	    }
	    if ($deltastring !~ /^\s*$/){
		print OUT "gap:\n";
		print OUT $deltastring;
		print OUT ".\n";
	    }
	    print OUT "}\n";
	}
	print OUT "}\n";
    } else {
	$base->bail("Weird error at line $. in $tmprefix.ctg");
    }
}

close(TMPCTG);

unlink("$tmprefix.ctg") || $base->bail("Cannot remove $tmprefix.ctg: $!\n");

# all the contig links
# all the contig edges
# and all the scaffolds

if (-f "$tmprefix.scf"){
    open(TMPSCF, "$tmprefix.scf") || $base->bail("Cannot open $tmprefix.scf:$!\n");
    while (<TMPSCF>){
	print OUT;
    }
    close(TMPSCF);
    unlink("$tmprefix.scf") || $base->bail("Cannot remove $tmprefix.scf: $!\n");
}
close(OUT);

if ($minSeqId > $minCtgId){
    print "Max ID: $minSeqId\n";
} else {
    print "Max ID: $minCtgId\n";
}

exit(0);




###############################################################################


# LIBRARY NAME PARSING
sub parseInsertFile {
    my $IN = shift;

    while (<IN>){
	if (/GenomicLibrary Id=\"(\S+)\" acc=\"(\d+)\"/){
	    $libnames{$2} = $1;
	    print STDERR "lib-id = $2; lib-name = $1\n";
	}
    }
} # parseInsertFile

sub parseLibMapFile {
    my $IN = shift;

    while (<IN>){
	my ($id, $name) = split(' ', $_);
	$libnames{$id} = $name;
    }
}

# POSIION FILE PARSER

# parse TIGR-style .pos file
sub parsePosFile {
    my $IN = shift;
    my $pos = tell IN;
    while (<IN>){
	chomp;
	my ($seqname, $ver, $poss) = split('\t', $_);
	if ($seqname ne "SequenceName"){ # skip header
	    if (! exists $seqids{$seqname}){
		$base->bail("Have not seen sequence $seqname before processing pos file");
	    }
#	print "pos of $seqids{$seqname} $seqname is $pos\n";
	    $posidx{$seqids{$seqname}} = $pos;
	}
	$pos = tell IN;
    }
} # parse TIGR-style .pos file


# MATES PARSING FUNCTIONS

# parse Trace Archive style XML files
my $tag;
my $library;
my $template;
my $clipl;
my $clipr;
my $mean;
my $stdev;
my $end;
my $seqId;

sub parseTraceInfoFile {
    my $IN = shift;

    my $xml = new XML::Parser(Style => 'Stream');

    if (! defined $xml){
	$base->bail("Cannot create an XML parser");
    }

    # start parsing away.  The hashes will magically fill up

    $xml->parse($IN);

} # parseTraceInfoFile


# Celera .frg
# populates %seqids, %seqnames, and %seq_range, %libraries, %insertlib,
# %seenlib, %seqinsert
sub parseFrgFile {
    my $IN = shift;

    while (my $record = getRecord($IN)){
	my ($type, $fields, $recs) = parseRecord($record);
	if ($type eq "FRG") {
	    my $id = getCAId($$fields{acc});
	    my $iid = $minSeqId++;
	    my $nm = $$fields{src};
	    my @lines = split('\n', $nm);
	    $nm = $lines[0]; # join('', @lines);
	    if ($byaccession || $nm =~ /^\s*$/){
		$seqnames{$iid} = $id;
	    } else {
		$seqnames{$iid} = $nm;
		$seqids{$nm} = $iid;
	    }
	    $seqids{$id} = $iid;
	    my ($seql, $seqr) = split(',', $$fields{clr});
	    $seq_range{$iid} = "$seql $seqr";
	    print TMPSEQ "#$iid\n";
	    print TMPSEQ "$$fields{seq}";
	    print TMPSEQ "#\n";
	    print TMPSEQ "$$fields{qlt}";
	    print TMPSEQ "#\n";
	    next;
	}
	
	if ($type eq "DST"){
	    my $id = getCAId($$fields{acc});
	    $libraries{$id} = "$$fields{mea} $$fields{std}";
#	    $libid{$id} = $minSeqId++;
	    next;
	}
	
	if ($type eq "LKG"){
	    my $id = $minSeqId++;
#	    $insertlib{$$fields{dst}} .= "$id ";
	    $seenlib{$id} = $$fields{dst};
	    $seqinsert{$seqids{$$fields{fg1}}} = $id;
	    $seqinsert{$seqids{$$fields{fg2}}} = $id;
	    $forw{$id} = $seqids{$$fields{fg1}};
	    $rev{$id} = $seqids{$$fields{fg2}};
	    if ($$fields{ori} eq "O"){
		$inserttype{$id} = "T";
	    }
	    next;
	}
    }

} #parseFrgFile

# multi-fasta formatted file
# accepts one of the following headers:
# >seqname
# >seqname clearleft clearright
# >seqname \d+ \d+ \d+ clearleft clearright
sub parseFastaFile
{
    my $seqfile = shift;
    my $qualfile = shift;

    my $pf = new AMOS::ParseFasta($seqfile);
    my $qf;
    if (defined $qualfile){
	$qf = new AMOS::ParseFasta($qualfile, '>', ' ');
    }
    while (my ($head, $data) = $pf->getRecord()){
	my $seqname;
	my $cll;
	my $clr;
	if ($head =~ /^(\S+)\s+\d+\s+\d+\s+\d+\s+(\d+)\s+(\d+)/){
	    # TIGR format
	    $seqname = $1; $cll = $2; $clr = $3;
	} elsif ($head =~ /^(\S+)\s+(\d+)\s+(\d+)/){
	    # just name and clear range
	    $seqname = $1; $cll = $2; $clr = $3;
	} elsif ($head =~ /^(\S+)/){
	    # just name
	    $seqname = $1;
	}

	if (! defined $cll){
	    $cll = 0;
	    $clr = length($data);
	}

	my $id = $minSeqId++;
	$seqnames{$id} = $seqname;
#	print STDERR "got $seqname $id\n";
	$seqids{$seqname} = $id;

	# so we don't overwrite an externally provided clear range
	if (! exists $seq_range{$id}){
	    $seq_range{$id} = "$cll $clr";
	} else {
	    ($cll, $clr) = split(' ', $seq_range{$id});
	}
	
	print TMPSEQ "#$id\n";
	for (my $i = 0; $i <= length($data); $i+= 60){
	    print TMPSEQ substr($data, $i, 60), "\n";
	}
	print TMPSEQ "#\n";
	my $qualdata = "";
	if (defined $qualfile){
	    my ($qh, $qdata) = $qf->getRecord();
	    if ($qh !~ /^$seqname/){
		$base->bail("Sequence and quality records must agree: $seqname != $qh\n");
	    }
	    my @quals = split(' ', $qdata);
	    if ($#quals + 1 != length($data)){
		$base->bail(sprintf("Sequence and quality records must have same length for $seqname: %d vs %d\n", length($data), $#quals + 1));
	    }
	    for (my $i = 0; $i <= $#quals; $i++){
		if ($quals[$i] <= 0) {$quals[$i] = 1;}
		if ($quals[$i] > 60) {$quals[$i] = 60;}
		$qualdata .= chr(ord('0') + $quals[$i]);
	    }
	} else {
	    for (my $i = 0; $i < $cll; $i++){
		$qualdata .= chr(ord('0') + $BADQUAL);
	    }
	    for (my $i = $cll; $i < $clr; $i++){
		$qualdata .= chr(ord('0') + $GOODQUAL);
	    }
	    for (my $i = $clr; $i < length($data); $i++){
		$qualdata .= chr(ord('0') + $BADQUAL);
	    }
	}

	for (my $i = 0; $i <= length($qualdata); $i+= 60){
	    print TMPSEQ substr($qualdata, $i, 60), "\n";
	}
	print TMPSEQ "#\n";
    }
}

# parses BAMBUS style .mates file
# * expects %seqids to be populated
# * populates %libraries, %forw, %rev, %insertlib, %seenlib, %seqinsert
sub parseMatesFile {
    my $IN = shift;

	print "in mates";

    my @libregexp;
    my @libids;
    my @pairregexp;
    my $insname = 1;

    while (<$IN>){
	chomp;
	if (/^library/){
	    my @recs = split('\t', $_);
	    if ($#recs < 3 || $#recs > 4){
		print STDERR "Only ", $#recs + 1, " fields\n";
		$base->logError("Improperly formated line $. in \"$matesfile\".\nMaybe you didn't use TABs to separate fields\n", 1);
		next;
	    }
	    
	    if ($#recs == 4){
		$libregexp[++$#libregexp] = $recs[4];
		$libids[++$#libids] = $recs[1];
	    }
	    my $mean = ($recs[2] + $recs[3]) / 2;
	    my $stdev = ($recs[3] - $recs[2]) / 6;
	    $libraries{$recs[1]} = "$mean $stdev";
	    next;
	} # if library

	if (/^pair/){
	    my @recs = split('\t', $_);
	    if ($#recs != 2){
		$base->logError("Improperly formated line $. in \"$matesfile\".\nMaybe you didn't use TABs to separate fields\n");
		next;
	    }
	    @pairregexp[++$#pairregexp] = "$recs[1] $recs[2]";
	    next;
	}
	if (/^\#/) { # comment
	    next;
	}
	if (/^\s*$/) { # empty line
	    next;
	}
	
	# now we just deal with the pair lines
	my @recs = split('\t', $_);
	if ($#recs < 1 || $#recs > 2){
	    $base->logError("Improperly formated line $. in \"$matesfile\".\nMaybe you didn't use TABs to separate fields\n");
	    next;
	}
	
# make sure we've seen these sequences
	if (! defined $seqids{$recs[0]}){
	    $base->logError("Sequence $recs[0] has no ID at line $. in \"$matesfile\"");
	    next;
	}
	if (! defined $seqids{$recs[1]} ){
	    $base->logError("Sequence $recs[1] has no ID at line $. in \"$matesfile\"");
	    next;
	}
	
	if (defined $recs[2]){
#	    $insertlib{$recs[2]} .= "$insname ";
	    $seenlib{$insname} = $recs[2];
	} else {
	    $base->logError("$insname has no library\n");
	}
	
	$forw{$insname} = $seqids{$recs[0]};
	$rev{$insname} = $seqids{$recs[1]};
	
	$seqinsert{$seqids{$recs[0]}} = $insname;
	$seqinsert{$seqids{$recs[1]}} = $insname;
	
	$insname++;
    } # while <IN>

    # now we have to go through all the sequences and assign them to
    # inserts
    while (my ($nm, $sid) = each %seqids){
	for (my $r = 0; $r <= $#pairregexp; $r++){
	    my ($freg, $revreg) = split(' ', $pairregexp[$r]);
	    $base->logLocal("trying $freg and $revreg on $nm\n", 2);
	    if ($nm =~ /$freg/){
		$base->logLocal("got forw $1\n", 2);
		if (! exists $forw{$1}){
		    $forw{$1} = $sid;
		    $seqinsert{$sid} = $1;
		}
		last;
	    }
	    if ($nm =~ /$revreg/){
		$base->logLocal("got rev $1\n", 2);
		if (! exists $rev{$1}){
		    $rev{$1} = $sid;
		    $seqinsert{$sid} = $1;
		}
		last;
	    }
	} # for each pairreg
    } # while each %seqids
    
    while (my ($ins, $nm) = each %forw) {
	if (! exists $insid{$ins}){
	    $insid{$ins} = $minSeqId++;
	}
	if (! exists $seenlib{$ins}){
	    my $found = 0;
	    
	    $nm = $seqnames{$nm};

	    for (my $l = 0; $l <= $#libregexp; $l++){
		$base->logLocal("Trying $libregexp[$l] on $nm\n", 2);
		if ($nm =~ /$libregexp[$l]/){
		    $base->logLocal("found $libids[$l]\n", 2);
#		    $insertlib{$libids[$l]} .= "$ins ";
		    $seenlib{$ins} = $libids[$l];
		    $found = 1;
		    last;
		}
	    }
	    if ($found == 0){
		$base->logError("Cannot find library for \"$nm\"");
		next;
	    }
	}
    }
    while (my ($ins, $nm) = each %rev) {
	if (! exists $insid{$ins}){
	    $insid{$ins} = $minSeqId++;
	}
	if (! exists $seenlib{$ins}){
	    my $found = 0;
	    
	    $nm = $seqnames{$nm};

	    for (my $l = 0; $l <= $#libregexp; $l++){
		$base->logLocal("Trying $libregexp[$l] on $nm\n", 2);
		if ($nm =~ /$libregexp[$l]/){
		    $base->logLocal("found $libids[$l]\n", 2);
#		    $insertlib{$libids[$l]} .= "$ins ";
		    $seenlib{$ins} = $libids[$l];
		    $found = 1;
		    last;
		}
	    }
	    if ($found == 0){
		$base->logError("Cannot find library for \"$nm\"");
		next;
	    }
	}
    }
} # parseMateFile;


# CONTIG PARSING FUNCTIONS
#
# Each function parses either a file or a database table and
# fills in the following hashes:
# 
# %contigs - contig_ids and sizes
# %seqids - seq_name to seq_id
# %seqnames - seq_id to seq_name
# %seq_range - seq_id to seq_range 
# %asm_range - seq_id to asm_range as blank delimited string
# %seqcontig - seq_id to contig
# %contigcons - contig consensus for each contig



# Celera .asm
# populates %contigs, %asm_range, %seqcontig, %contigcons
# expects %seq_range to be populated
sub parseAsmFile 
{
  my $IN = shift;

  while (my $record = getRecord($IN))
  {
    my ($type, $fields, $recs) = parseRecord($record);

    if (($type eq "CCO") || 
        ($INCLUDE_SURROGATE && ($type eq "UTG") && ($$fields{sta} eq "S")) ||
        ($UTG_MESSAGES && ($type eq "UTG")))
    {
      my $sts = "C";
      $sts = $$fields{sta} if $type eq "UTG";
      $sts = $$fields{pla} if $type eq "CCO";

      my $id = getCAId($$fields{acc});
      my $iid = $minCtgId++;
      my $contiglen = $$fields{len};

      $ctgnames{$iid} = $id;
      $ctgids{$id} = $iid;

      my $coord;

      my $consensus = $$fields{cns};
      my @consensus = split('\n', $consensus);
      $consensus = join('', @consensus);

      print TMPCTG "#$iid $sts\n";
      print TMPCTG $$fields{cns};
      print TMPCTG "#\n";
      print TMPCTG $$fields{qlt};
      
      $contigs{$iid} = $contiglen;

      for (my $i = 0; $i <= $#$recs; $i++)
      {
        my ($sid, $sfs, $srecs) = parseRecord($$recs[$i]);
        if ($sid eq "MPS")
        {
            if (! exists $seqids{getCAId($$sfs{mid})}){
                die ("Have not seen sequence with id " . getCAId($$sfs{mid}));
            }

            my $fid = $seqids{getCAId($$sfs{mid})};
            print TMPCTG "#$fid\n";
            print TMPCTG $$sfs{del};
            $seqcontig{$fid} = $id;
            $contigseq{$id} .= "$fid ";
            
            my ($asml, $asmr) = split(',', $$sfs{pos});
            $asm_range{$fid} = "$asml $asmr";
        }
      }
      print TMPCTG "#\n";
    } 
    elsif ($type eq "SCF")
    {
      my $off = 0;
      print TMPSCF "{SCF\n";
      my $id = getCAId($$fields{acc});
      my $iid = $minCtgId++;
      $ctgids{$id} = $iid;
      $ctgnames{$iid} = $id;
      print TMPSCF "iid:$iid\n";
      print TMPSCF "eid:$id\n";
      for (my $i = 0; $i <= $#$recs; $i++)
      {
        my ($sid, $sfs, $srecs) = parseRecord($$recs[$i]);
        if ($sid eq "CTP")
        {
          print TMPSCF "{TLE\n";
          print TMPSCF "src:$ctgids{$$sfs{ct1}}\n";
          print TMPSCF "off:$off\n";

          if ($$sfs{ori} eq "N" ||
              $$sfs{ori} eq "I")
          {
            print TMPSCF "clr:0,$contigs{$ctgids{$$sfs{ct1}}}\n";
          } 
          else 
          {
            print TMPSCF "clr:$contigs{$ctgids{$$sfs{ct1}}},0\n";
          }

          print TMPSCF "}\n";
          $off += $contigs{$ctgids{$$sfs{ct1}}};
          $off += int($$sfs{mea});

          if ($i == $#$recs && $$sfs{ct1} != $$sfs{ct2})
          {
            print TMPSCF "{TLE\n";
            print TMPSCF "src:$ctgids{$$sfs{ct2}}\n";
            print TMPSCF "off:$off\n";

            if ($$sfs{ori} eq "N" ||
                $$sfs{ori} eq "O")
            {
              print TMPSCF "clr:0,$contigs{$ctgids{$$sfs{ct2}}}\n";
            } 
            else 
            {
              print TMPSCF "clr:$contigs{$ctgids{$$sfs{ct2}}},0\n";
            }

            print TMPSCF "}\n";
          }
        }
      }
      print TMPSCF "}\n";
    } 
    elsif ($type eq "CLK")
    {
      print TMPSCF "{CTE\n";
      print TMPSCF "nds:$ctgids{$$fields{co1}},$ctgids{$$fields{co2}}\n";
      print TMPSCF "adj:$$fields{ori}\n";
      print TMPSCF "sze:$$fields{mea}\n";
      print TMPSCF "std:$$fields{std}\n";
      print TMPSCF "typ:M\n";
      print TMPSCF "}\n";
    }
    elsif ($type eq "AFG")
    {
      ## Grab the clear range from the AFG records if present

      if (exists $$fields{clr})
      {
        my $acc = getCAId($$fields{acc});
        my $iid = $seqids{$acc};

        my ($seql, $seqr) = split(/,/, $$fields{clr});
        my $clrstr = "$seql $seqr";

        # if (!defined $seq_range{$iid} || $seq_range{$iid} ne $clrstr)
        # {
        #   my $origclr = $seq_range{$iid};
        #   print STDERR "Updating $acc clr range from $origclr to $clrstr\n";
        # }

        $seq_range{$iid} = $clrstr;
      }
    }
  }
} # parseAsmFile

# TIGR .asm
sub parseTAsmFile {
    my $IN = shift;

    my $ctg; 
    my $len;
    my $sname;
    my $alend;
    my $arend;
    my $slend;
    my $srend;
    my $sid;
    my $consensus;
    my $iid;
    while (<$IN>){
	if (/^sequence\s+(\w+)/){
	    $len = length($1);
	    $consensus = $1;
	    next;
	}
	if (/^asmbl_id\s+(\w+)/){
	    $ctg = $1;
	    $iid = $minCtgId++;
	    $ctgnames{$iid} = $ctg;
	    $ctgids{$ctg} = $iid;
	    $contigs{$iid} = $len;  # here we assume that length 
                                    # was already computed
	    next;
	}
	if (/^seq_name\s+(\S+)/){
	    $sname = $1;
	    if (! exists $seqids{$sname}){
		$sid = $minSeqId++;
		$seqids{$sname} = $sid;
		$seqnames{$sid} = $sname;
	    } else {
		$sid = $seqids{$sname};
	    }

	    $seqcontig{$sid} = $iid;
	    $contigseq{$iid} .= "$sid ";
	    next;
	}
	if (/^asm_lend\s+(\d+)/){
	    $alend = $1 - 1; # 0 based
	    next;
	}
	if (/^asm_rend\s+(\d+)/){
	    $arend = $1;
	    next;
	}
	if (/^seq_lend\s+(\d+)/){
	    $slend = $1 - 1;
	    next;
	}
	if (/^seq_rend\s+(\d+)/){
	    $srend = $1;
	    next;
	}
	if (/^offset/){
	    $seq_range{$sid} = "$slend $srend";
	    $asm_range{$sid} = "$alend $arend";
	    next;
	}
    }
} # parseTasmFile


sub parsePHDFiles {
    my $IN = shift;

    while(<$IN>) {
	if(m/^DS CHROMAT_FILE: (\S+) PHD_FILE: (\S+)/) {
	    open(PHD_FILE, "<../phd_dir/$2") || die("missing phd file $2");
	    
	    my $seqname = $1;
		my $pos_list;

	    while(<PHD_FILE>) {
		if(m/BEGIN_DNA/) {
		    my $seq;

		    
		    while(<PHD_FILE>) {
			if(m/END_DNA/) {
			    last;
			}
			chomp();
			my ($nuc, $qual, $p) =  split;
			$seq .= $nuc;
			$pos_list .= " " . $p;
		    }
		    
		    last;
		}
	    }
	    
	    if (! exists $seqids{$seqname}){
			print("Have not seen sequence $seqname before processing pos file");
	    } else {
			# put pos into posidx
			$posidx{$seqids{$seqname}} = $pos_list;
	    }
	}
	close(PHD_FILE);
    }
}


# New .ACE format
sub parseACEFile {
    my $IN = shift;
    
    my $ctg; 
    my $len;
    my $sname;
    my $alend;
    my $arend;
    my $slend;
    my $srend;
    my $sid;

    my $inContig = 0;
    my $inSequence = 0;
    my $inQual = 0;

    my $contigName;
    my $contigLen;
    my $contigSeqs;
    my $seqName;
    my %offset;
    my %rc;
    my $seq;
    my @gaps;
    my $iid;
    my $first = 1;
    my $qual = "";
    while (<$IN>){
	if (/^CO (\S+) (\d+) (\d+)/){
	    if ($first != 1){print TMPCTG "#\n";}
	    $first = 0;
	    $contigName = $1;
	    $iid = $minCtgId++;
	    $ctgnames{$iid} = $contigName;
	    $ctgids{$contigName} = $iid;
	    $contigLen = $2;
#	    $contigs{$iid} = $contigLen;
	    $contigSeqs = $3;
	    $inContig = 1;
	    $seq = "";
	    %offset = ();
	    next;
	}
	if ($inContig == 1 && /^\s*$/){ # end of contig
	    $inContig = 0;
	    $seq =~ s/\*/-/g;
	    @gaps = (); 
	    my $gap  = index($seq, "-");
	    while ($gap != -1){
		push(@gaps, $gap + 1);
		$gap = index($seq, "-", $gap + 1);
	    }
	    $contigs{$iid} = $contigLen;
	    # print consensus record
	    print TMPCTG "#$iid C\n";
	    for (my $c = 0; $c < length($seq); $c+= 60){
		print TMPCTG substr($seq, $c, 60), "\n";
	    }
	    print TMPCTG "#\n";
#	    my $qual = $seq;
#	    $qual =~ s/./X/;
#	    # print quality values (all Xs
#	    for (my $c = 0; $c < length($qual); $c += 60){
#		print TMPCTG substr($qual, $c, 60), "\n";
#	    }

	    next;
	}

	if ($inSequence == 1 && $_ =~ /^\s*$/){
	    $inSequence = 0;
	    next;
	}

	if ($inContig == 1 || $inSequence == 1) {
	    chomp;
	    $seq .= $_;
	    next;
	}

	if (/^BQ/){
	    $inQual = 1;
	    $qual = "";
	    next;
	}

	if ($inQual == 1 && $_ =~ /^\s*$/){
	    $inQual = 0;
	    my $tmpqual = $qual;
	    my $qual = "";
	    my $q = 0;
	    # create quality values for consensus gaps
	    for (my $c = 0; $c < length($seq); $c++){
		if (substr($seq, $c, 1) eq "-"){
		    if ($q >= length($tmpqual)){
			$q = length($tmpqual) - 1;
		    }
		    $qual .= substr($tmpqual, $q, 1); # next seen quality
		} else {
		    $qual .= substr($tmpqual, $q++, 1);
		}
	    }
	    for (my $c = 0; $c < length($qual); $c += 60){
		print TMPCTG substr($qual, $c, 60), "\n";
	    }
	}

	if ($inQual == 1){
	    chomp;
	    $_ =~ s/^\s+//;
	    my @quals = split(' ', $_);
	    for (my $q = 0; $q <= $#quals; $q++){
		if ($quals[$q] > 60) {$quals[$q] = 60;}
		$qual .= chr(ord("0")+ $quals[$q]);
	    }
	}
	
	
	if (/^AF (\S+) (\w) (-?\d+)/){
	    # AF record contains the offset of the sequence
	    # in the contig, 1-based.  Also indicates whether sequence
	    # is reverse complemented
	    $offset{$1} = $3 - 1;
	    $rc{$1} = $2;
	    next;
	}
	
	if (/^RD (\S+)/){
	    # indicates start of a sequence record
	    $inSequence = 1;
	    $seqName = $1;
	    $seq = "";
	    next;
	}

	if (/^QA -?(\d+) -?(\d+) (\d+) (\d+)/){
	    # at this point the sequence ended
	    my $offset = $offset{$seqName};
	    my $cll = $3 - 1;
	    my $clr = $4;
	    my $end5 = $1;
	    my $end3 = $2;

	    # ACE files contain *s instead of -s
	    $seq =~ s/\*/-/g;

	    # shift offset to beginning of clear range
	    # otherwise it points to the beginning of the entire sequence
	    # shown in the file
	    $len = length($seq);
	    $offset += $cll;
	    
	    # @sdels contains the positions of gaps in the sequence
	    my $ndel = 0;
	    my @sdels = ();

	    my $allseq = $seq; # all the sequence (will be chopped later)

	    # gap positions are wrt to the clear range
	    # in the aligned orientation
	    $seq = substr($seq, $cll, $clr - $cll);
	    for (my $s = 0; $s < length($seq); $s++){
                if (substr($seq, $s, 1) eq "-"){
                    push @sdels, $ndel;
                } else {
                    $ndel++;
                }
            }
	    if ($rc{$seqName} eq "C"){
		$seq = reverseComplement($seq);
		$allseq = reverseComplement($allseq);
	    }
	    

	    # if read is reversed swap cll and clr
	    if ($rc{$seqName} eq "C"){
		my $tmp = $len - $cll;
		$cll = $len - $clr;
		$clr = $tmp;
	    }

	    my $pref = substr($allseq, 0, $cll); # prefix of sequence

	    my $i = 0;
	    my $asml = $offset;
	    my $asmr = $asml + $clr - $cll;

	    while ($pref =~ /-/g) { # make $cll  ungapped
		$cll--;
		$clr--;
	    }
	    while ($seq =~ /-/g){ #make $clr ungapped
		$clr--;
	    }

	    # if reverse complemented, reverse the assembly range
	    if ($rc{$seqName} eq "C"){
		my $tmp = $asml;
		$asml = $asmr;
		$asmr = $tmp;
	    }
        
	    # assign sequence id and populate all necessary data-structures
	    my $seqId;
	    if (! exists $seqids{$seqName}){
#		print STDERR "Couldnt find id for $seqName\n";
		$seqId = $minSeqId++;
		$seqids{$seqName} = $seqId;
		$seqnames{$seqId} = $seqName;
	    } else {
		$seqId = $seqids{$seqName};
	    }
	    $seqcontig{$seqId} = $iid;
	    $contigseq{$iid} .= "$seqId ";
	    if ($cll > $clr) {
		$seq_range{$seqId} = "$clr $cll";
	    } else {
	        $seq_range{$seqId} = "$cll $clr";
	    }
	    $asm_range{$seqId} = "$asml $asmr";
	    if ($readsDone == 0){ # no read info, must generate
		my $qualdata = "";
		$allseq =~ s/-//g;
		print TMPSEQ "#$seqId\n";
		for (my $i = 0; $i <= length($allseq); $i+= 60){
		    print TMPSEQ substr($allseq, $i, 60), "\n";
		}
		print TMPSEQ "#\n";
		for (my $i = 0; $i < $cll; $i++){
		    $qualdata .= chr(ord('0') + $BADQUAL);
		}
		for (my $i = $cll; $i < $clr; $i++){
		    $qualdata .= chr(ord('0') + $GOODQUAL);
		}
		for (my $i = $clr; $i < length($allseq); $i++){
		    $qualdata .= chr(ord('0') + $BADQUAL);
		}
		for (my $i = 0; $i <= length($qualdata); $i+= 60){
		    print TMPSEQ substr($qualdata, $i, 60), "\n";
		}
		print TMPSEQ "#\n";
	    } # if $readsDone == 0
	    print TMPCTG "#$seqId\n";
	    print TMPCTG join (" ", @sdels), "\n";
	    next;
	}
    } # while <$IN>
    print TMPCTG "#\n";
} #parseAceFile



# TIGR .contig file
sub parseContigFile {
    my $IN = shift;

    my $ctg; 
    my $len;
    my $sname;
    my $alend;
    my $arend;
    my $slend;
    my $srend;
    my $sid;
    my $incontig = 0;
    my $consensus = "";
    my @sdels;
    my $ndel;
    my $slen;
    my $iid;

    my $first = 1;
    while (<$IN>){
	if (/^\#\#(\S+) \d+ (\d+)/ ){
	    if ($first != 1){
		print TMPCTG "#$sid\n";
		print TMPCTG join(" ", @sdels), "\n";
		print TMPCTG "#\n";
		$arend = $alend + $slen;
		$asm_range{$sid} = "$alend $arend";
#		print TMPCTG "#\n";
	    }
	    $first = 0;
	    $consensus = "";
	    $ctg = $1;
	    $iid = $minCtgId++;
	    $ctgids{$ctg} = $iid;
	    $ctgnames{$iid} = $ctg;
	    $contigs{$iid} = $2;
	    $incontig = 1;
	    $slen = 0;
	    next;
	}

	if (/^\#(\S+)\((\S+)\) .*\{(\S+) (\S+)\} <(\S+) (\S+)>/){
	    if ($incontig == 1){
		print TMPCTG "#$iid C\n";
		for (my $c = 0; $c < length($consensus); $c+=60){
		    print TMPCTG substr($consensus, $c, 60), "\n";
		}
#		print TMPCTG "$consensus\n";
		print TMPCTG "#\n";
		for (my $c = 0; $c < length($consensus); $c+=60){
		    for ($b = 0; $b < 60; $b++){
			if ($b + $c >= length($consensus)){last;}
			print TMPCTG "X";
		    }
		    print TMPCTG "\n";
		}
#		print TMPCTG "\n";
	    } else {
		print TMPCTG "#$sid\n";
		print TMPCTG join(" ", @sdels), "\n";
		$arend = $alend + $slen;
		$asm_range{$sid} = "$alend $arend";
		$slen = 0;
	    }
	    $incontig = 0;
	    $sname = $1;
	    @sdels = ();
	    $ndel = 0;
	    if (! exists $seqids{$sname}){
		die ("Cannot find ID for sequence $sname\n");
	    } else {
		$sid = $seqids{$sname};
	    }
	    
	    $seqcontig{$sid} = $iid;
	    $contigseq{$iid} .= "$sid ";
	    
	    $alend = $2;
	    
	    if ($3 < $4){
		$slend = $3 - 1;
		$srend = $4;
	    } else {
		$slend = $3;
		$srend = $4 - 1;
	    }
	    $seq_range{$sid} = "$slend $srend";
	    next;
	}

	if ($incontig){
	    chomp;
	    $consensus .= $_;
	} else { # sequence record
	    chomp;
	    $slen += length($_);
	    for (my $s = 0; $s < length($_); $s++){
		if (substr($_, $s, 1) eq "-"){
		    push @sdels, $ndel;
		} else {
		    $ndel++;
		}
	    }
	}
    } # while in
    # process last sequence
    print TMPCTG "#$sid\n";
    print TMPCTG join(" ", @sdels), "\n";
    print TMPCTG "#\n";
    $arend = $alend + $slen;
    $asm_range{$sid} = "$alend $arend";

} # parseContigFile

# Arachne .links scaffold file
# assumptions: all contigs are forward
# super ids are integers
sub parseArachneScaff()
{
    my $IN = shift;

   # Fields in TAB delimited file
   # 0 - super_id       
   # 1 - num_bases_in_super      
   # 2 - num_contigs_in_super    
   # 3 - ordinal_num_of_contig_in_super  
   # 4 - contig_id       
   # 5 - length_of_contig        
   # 6 - estimated_gap_before_contig     
   # 7 - estimated_gap_after_contig

    my $lastsuper = undef;
    my $offset = 0;
    while (<$IN>) {
	if (/^\#/) { next;}
	my @fields = split('\t', $_);
	if ($fields[0] != $lastsuper){
	    if  (defined $lastsuper){
		print TMPSCF "}\n"; # close scaffold
	    }
	    $lastsuper = $fields[0];
	    $offset = 0;
	    print TMPSCF "{SCF\n";
	    print TMPSCF "iid:$lastsuper\n";
	    print TMPSCF "eid:$lastsuper\n";
	}
	if (! exists $ctgids{$fields[4]}) {
	    $base->logError("Cannot find id for contig $fields[4]\n");
	    next;
	}
	
	# print contig tile
	print TMPSCF "{TLE\n";
	print TMPSCF "src:$ctgids{$fields[4]}\n";
	print TMPSCF "off:$offset\n";
	print TMPSCF "clr:0,$contigs{$ctgids{$fields[4]}}\n";
	print TMPSCF "}\n";

	# update offset
	$offset += $fields[5] + $fields[7];
    }
    print TMPSCF "}\n"; # done here
} #  parseArachneScaff


# Bambus/AMOS .scaff file
sub parseScaff()
{
    my $IN = shift;

    # format of scaff file
    #>scaffid numcontigs scaffbases scaffspan
    #contigid orientation contiglen gapsize

    # note gapsize is gap after contig

    my $ps = new AMOS::ParseFasta($IN, ">", "\n");
    while (my ($head, $data) = $ps->getRecord()){
	$head =~ /(\S+).*/;
	my $scfnam = $1;
	my $scfid = $minSeqId++;

	print TMPSCF "{SCF\n";
	print TMPSCF "iid:$scfid\n";
	print TMPSCF "eid:$scfnam\n";

	my $offset = 0;
	
	my @lines = split('\n', $data);
	for (my $i = 0; $i <= $#lines; $i++){
	    my @fields = split(/\s+/, $lines[$i]);
	    
	    if (! exists $ctgids{$fields[0]}) {
		$base->logError("Cannot find id for contig $fields[0]\n");
		next;
	    }

	    # print contig tile
	    print TMPSCF "{TLE\n";
	    print TMPSCF "src:$ctgids{$fields[0]}\n";
	    print TMPSCF "off:$offset\n";
	    if ($fields[1] eq "BE"){
		print TMPSCF "clr:0,$contigs{$ctgids{$fields[0]}}\n";
	    } else {
		print TMPSCF "clr:$contigs{$ctgids{$fields[0]}},0\n";
	    }
	    print TMPSCF "}\n";
	    $offset += $contigs{$ctgids{$fields[0]}} + $fields[3];
	}

	print TMPSCF "}\n";
    }

} # parseScaff

###############################################################
# XML parser functions
###############################################################
sub StartDocument
{
#    print "starting\n";
}

sub EndDocument
{
#    print "done\n";
}

sub StartTag
{
    $tag = lc($_[1]);
    
    if ($tag eq "trace"){
        $library = undef;
        $template = undef;
        $clipl = undef;
        $clipr = undef;
        $mean = undef;
        $stdev = undef;
        $end = undef;
        $seqId = undef;
    }
}


sub EndTag
{
    $tag = lc($_[1]);
    if ($tag eq "trace"){
        if (! defined $seqId){
            $base->logError("trace has no name???\n");
        }
        if (! defined $library){
            $base->logError("trace $seqId has no library\n");
        }
        if (! defined $mean){
            $base->logError("library $library has no mean\n");
        } 
        
        if (! defined $stdev){
            $base->logError("library $library has no stdev\n");
        }

	if (defined $mean and defined $stdev){
	    $libraries{$library} = "$mean $stdev";
        }

        if (! defined $template){
            $base->logError("trace $seqId has no template\n");
        } 
        
        if (! defined $end) {
            $base->logError("trace $seqId has no end\n");
        }
        
        if ($end eq "R"){
            if (! exists $rev{$template} ||
                $seqnames{$seqId} gt $seqnames{$rev{$template}}){
                $rev{$template} = $seqId;
            }
        }
	 
        if ($end eq "F"){
            if (! exists $forw{$template} ||
                $seqnames{$seqId} gt $seqnames{$forw{$template}}){
                $forw{$template} = $seqId;
            }
        }
	    
	$seqinsert{$seqId} = $template;
#	$insertlib{$library} .= "$template ";
	$seenlib{$template} = $library;
	
    
        if (defined $clipl && defined $clipr){
	    $seq_range{$seqId} = "$clipl $clipr";
        }
    }

    $tag = undef;
}


sub Text 
{
    if (defined $tag){
        if ($tag eq "insert_size"){
            $mean = $_;
        } elsif ($tag eq "insert_stdev"){
            $stdev = $_;
        } elsif ($tag eq "trace_name"){
            my $seqName = $_;
	    $seqId = $minSeqId++;
	    $seqids{$seqName} = $seqId;
	    $seqnames{$seqId} = $seqName;
        } elsif ($tag eq "library_id"){
            $library = $_;
        } elsif ($tag eq "seq_lib_id") {
            if (! defined $library) {
                $library = $_;
            }
        } elsif ($tag eq "template_id"){
            $template = $_;
        } elsif ($tag eq "trace_end"){
            $end = $_;
        } elsif ($tag eq "clip_quality_left" ||
                 $tag eq "clip_vector_left"){
            if (! defined $clipl || $_ > $clipl){
                $clipl = $_;
            }
        } elsif ($tag eq "clip_quality_right" ||
                 $tag eq "clip_vector_right"){
            if (! defined $clipr || $_ < $clipr){
                $clipr = $_;
            }
        }
    }
}

sub pi
{

}
