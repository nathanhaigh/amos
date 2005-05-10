#!/usr/local/bin/perl

use strict;

use AMOS::AmosLib;
use AMOS::ParseFasta;
use TIGR::Foundation;
use XML::Parser;
use IO::Handle;
use POSIX qw(tmpnam);
$ENV{TMPDIR} = ".";

my $MINSEQ = 0;
my $MAXSEQ = undef;
my $DEFAULT_QUAL = 20;

my $tmprefix = "tmp.$$"; #tmpnam();

my $tag; # XML tag
my $library;
my $template;
my $clipl;
my $clipr;
my $mean;
my $stdev;
my $end;
my $seqId;

my %end5;       # the reads at the 5', resp. 3', end of an insert
my %end3;
my %means;      # mean and standard deviation for libraries
my %stdevs;
my %clr;        # clear range for each sequence
my %seq2ins;    # insert name for each sequence
my %ins2lib;    # library name for each insert
my %lib2id;     # mapping from library name to iid
my %ins2id;     # mapping from insert name to iid
my %seq2id;     # mapping from read name to iid

my @libregexp;  # when using mates files contains list of libraries regular expressions
my @libnames;   # library names corresponding to the regular expressions
my @pairregexp; # read mating regular expressions

my $gzip = "gzip";

my $base = new TIGR::Foundation;

if (! defined $base) {
    die ("Walk, do not run, to the nearest exit!\n");
}

my $VERSION = '$Revision$ ';
$base->setVersionInfo($VERSION);

my $HELPTEXT = qq~
    tarchive2amos -o <prefix> [-c <clip>] [-m <mates>] [options] fasta1 ... fastan

   <prefix>   - prefix for the output files
   <clip>     - file containing clear ranges for the reads.  If this file
           is provided, any sequence that does not appear in it is excluded
           from the output.
   <mates>    - file containing mate-pair information as specified in the BAMBUS
           documentation.  This file replaces information provided in .xml files
   fasta1 ... fastan - list of files to be converted.
           The program assumes that for each program called <file>.seq there
           is a <file>.qual and a <file>.xml.  If no .xml file is present 
           the program will only produce a set of RED (read) records.  

Options

    -i <id> - start numbering messages with id <id> (useful when appending
to a bank)
    -min <minlen> - reads shorter than <minlen> are rejected (default $MINSEQ)
    -max <maxlen> - reads longer than <maxlen> are rejected (default no limit)
    -qual <qval> - default quality value assigned when no quality file is 
provided (default $DEFAULT_QUAL)
~;

$base->setHelpInfo($HELPTEXT);

my $outprefix;
my $clears;
my $mates ;     # name of file containing mate pairs
my $ID = 1;
my $TEM_ID = 1;  # generic identifier for reads with no template
my $silent;
my $err = $base->TIGR_GetOptions("o=s"    => \$outprefix,
				 "c=s"    => \$clears,
				 "m=s"    => \$mates,
                                 "i=i"    => \$ID,
				 "silent" => \$silent,
				 "min=i"    => \$MINSEQ,
				 "max=i"    => \$MAXSEQ,
				 "qual=i"   => \$DEFAULT_QUAL);

if ($err == 0) {
    $base->bail("Command line processing failed\n");
}

if (! defined $outprefix) {
    $base->bail("A prefix must be set with option -o\n");
}

my $xml = new XML::Parser(Style => 'Stream');
if (!defined $xml) {
    $base->bail("Cannot create an XML parser");
}


my %clones;
my %seqs;
my %lib ;

my $fragname;
if ($outprefix =~ /\.afg$/){
    $fragname = $outprefix;
} else {
    $fragname = "$outprefix.afg";
}

# get the clear ranges if externally determined
if (defined $clears){
    open(CLR, $clears) || $base->bail("Cannot open $clears: $!\n");
    while (<CLR>){
	chomp;
	my @fields = split(' ', $_);
	$clr{$fields[0]} = "$fields[1],$fields[2]";
    }
    close(CLR);
}

# get ready to produce the output
open(FRAG, ">$fragname") || $base->bail("Cannot open $fragname: $!");
printFragHeader(\*FRAG);

open(TMPRED, ">$tmprefix.red") || $base->bail("Cannot open $tmprefix.red: $!\n");

my $numxml = 0;
my $hasxml;
my $hasqual;

for (my $f = 0; $f <= $#ARGV; $f++){
# for each file
#   read anciliary XML data
    my $seqname = $ARGV[$f];
    my $qualname;
    my $xmlname;
    my $prefix;

    if ($seqname =~ /fasta\.(.*)(\.gz)?/){
	$prefix = $1;
    } elsif ($seqname =~ /(.*)\.seq(\.gz)?/){
	$prefix = $1;
    } else {
	$base->bail("Fasta file ($seqname) must be called either fasta.<file> or <file>.seq\n");
    }

    if (-e "xml.$prefix"){
	$xmlname = "xml.$prefix";
    } elsif (-e "xml.$prefix.gz") {
	$xmlname = "xml.$prefix.gz";
    } elsif (-e "$prefix.xml"){
	$xmlname = "$prefix.xml";
    } elsif ( -e "$prefix.xml.gz") {
	$xmlname = "$prefix.xml.gz";
    } else {
	next;
	$base->bail("Cannot find the xml file corresponding to $seqname");
    }
    $numxml++;

    my $XML = new IO::Handle;

    if ($xmlname =~ /\.gz$/){
	open($XML, "$gzip -dc $xmlname |") ||
	    $base->bail("Cannot open $xmlname: $!\n");
    } else {
	open($XML, "$xmlname") ||
	    $base->bail("Cannot open $xmlname: $!\n");
    }

    $xml->parse($XML);
    
    close($XML);
}


if ($numxml <= $#ARGV) { # fewer XML files than inputs
    $hasxml = 0;
} else {
    $hasxml = 1;
}


if (defined $mates){
    open(MATES, $mates) || $base->bail("Cannot open $mates: $!\n");
    
    while (<MATES>){
	chomp;
	if (/^library/){
	    my $libId = getId(); # get a new identifier
	    my @recs = split('\t', $_);

	    if ($#recs < 3 || $#recs > 4){
		$base->logError("Improperly formated line $. in \"$mates\".\nMaybe you didn't use TABs to separate fields\n", 1);
		next;
	    }
	    
	    if ($#recs == 4){
		push(@libregexp, $recs[4]);
		push(@libnames, $recs[1]);
	    }

	    my $mean = ($recs[2] + $recs[3]) / 2;
	    my $stdev = ($recs[3] - $recs[2]) / 6;

	    $means{$recs[1]} = $mean;
	    $stdevs{$recs[1]} = $stdev;
	    
	    next;
	} # if library

	if (/^pair/){
	    my @recs = split('\t', $_);
	    if ($#recs != 2){
		$base->logError("Improperly formated line $. in \"$mates\".\nMaybe you didn't use TABs to separate fields\n");
		next;
	    }
	    push(@pairregexp, "$recs[1] $recs[2]");
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
	    $base->logError("Improperly formated line $. in \"$mates\".\nMaybe you didn't use TABs to separate fields\n");
	    next;
	}

	my $insname = getId();
	$ins2id{$insname} = $insname;

	if (defined $recs[2]){
	    $ins2lib{$insname} = $recs[2];
	} else {
	    $base->logError("$insname has no library\n");
	}
	
	$end5{$insname} = $recs[0];
	$end3{$insname} = $recs[1];
	
	$seq2ins{$recs[0]} = $insname;
	$seq2ins{$recs[1]} = $insname;
    } # while <MATES>

    close(MATES);
} 

# now we are ready to print the library information
while (my ($lib, $mean) = each %means){
    my $libid = getId();
    $lib2id{$lib} = $libid;

    print FRAG "{LIB\n";
    print FRAG "iid:$libid\n";
    print FRAG "eid:$lib\n";
    print FRAG "{DST\n";
    print FRAG "mea:$mean\n";
    print FRAG "std:$stdevs{$lib}\n";
    print FRAG "}\n";
    print FRAG "}\n";
}


for (my $f = 0; $f <= $#ARGV; $f++){
# for each file
#   read the seq and qual files
    my $seqname = $ARGV[$f];
    my $qualname;
    my $prefix;

    if ($seqname =~ /fasta\.(.*)(\.gz)?/){
	$prefix = $1;
    } elsif ($seqname =~ /(.*)\.seq(\.gz)?/){
	$prefix = $1;
    } else {
	$base->bail("Fasta file ($seqname) must be called either fasta.<file> or <file>.seq\n");
    }
    
    if (-e "qual.$prefix"){
	$qualname = "qual.$prefix";
	$hasqual = 1;
    } elsif (-e "qual.$prefix.gz") {
	$qualname = "qual.$prefix.gz";
	$hasqual = 1;
    } elsif (-e "$prefix.qual"){
	$qualname = "$prefix.qual";
	$hasqual = 1;
    } elsif ( -e "$prefix.qual.gz") {
	$qualname = "$prefix.qual.gz";
	$hasqual = 1;
    } else {
	$hasqual = 0;
#	$base->bail("Cannot find the quality file corresponding to $seqname");
    }

    if ($seqname =~ /\.gz$/){
	open(SEQ, "$gzip -dc $seqname |") ||
	    $base->bail("Cannot open $seqname: $!\n");
    } else {
	open(SEQ, "$seqname") ||
	    $base->bail("Cannot open $seqname: $!\n");
   }
    
    if ($hasqual == 1){
	if ($qualname =~ /\.gz$/){
	    open(QUAL, "$gzip -dc $qualname |") ||
		$base->bail("Cannot open $qualname: $!\n");
	} else {
	    open(QUAL, "$qualname") ||
		$base->bail("Cannot open $qualname: $!\n");
	}
    }
    
    if (! defined $silent){
	print STDERR "Parsing $seqname and $qualname\n";
    }

    my $seqparse = new AMOS::ParseFasta(\*SEQ);
    my $qualparse;
    if ($hasqual == 1) {
	$qualparse = new AMOS::ParseFasta(\*QUAL, ">", " ");
    }

    my $fhead; my $frec;
    my $qhead; my $qrec;

    while (($fhead, $frec) = $seqparse->getRecord()) {
	my $qid;
	my $fid; 
	my $fidname;
	
	if ($hasqual == 1){
	    ($qhead, $qrec) = $qualparse->getRecord();
	    $qhead =~ /^(\S+)/;
	    my $qid = $1;
	    $fhead =~ /^(\S+)/;
	    my $fid = $1;
	    
	    if ($fid != $qid){
		$base->bail("fasta and qual records have different IDs: $fid vs $qid\n");
	    }
	}

	if ($fhead =~ /^(\S+)\s+\d+\s+\d+\s+\d+\s+(\d+)\s+(\d+)/){
# if TIGR formatted, fetch the clear range
#	    print STDERR "got TIGR: $1 $2 $3\n";
	    my $l = $2;
	    my $r = $3;
	    $l--;
	    $fidname = $1;
	    $fid = $1;
	    if (defined $clears && ! exists $clr{$fid}){
		# clear range file decides which sequences stay and which go
		next;
	    }
	    # allow XML or clear file to override the clear range info
	    if (! exists $clr{$fid}) {$clr{$fid} = "$l,$r";}
	} elsif ($fhead =~ /^ ?(\S+) ?(\S+)?/){
#	    print STDERR "got ncbi: $1 $2\n";
# NCBI formatted, first is the trace id then the trace name
	    $fid = $1;
	    $fidname = $2;

	    if (! defined $fidname || $fidname eq "bases"){
		$fidname = $fid;
	    }

	    if (defined $clears && ! exists $clr{$fid} && ! exists $clr{$fidname}) {
		# clear range file decides which sequences stay and which go
		next;
	    }
	    if (exists $clr{$fidname} && ! exists $clr{$fid}) {$clr{$fid} = $clr{$fidname};}
	}


	my $recId = getId();

	if (defined $mates){
	    for (my $r = 0; $r <= $#pairregexp; $r++){
		my ($freg, $revreg) = split(' ', $pairregexp[$r]);
		my $insertname = undef;
		if ($fidname =~ /$freg/){
		    $insertname = $1;
		    $seq2ins{$fidname} = $insertname;
		    if (! exists $end5{$insertname} || 
			$end5{$insertname} lt $fidname){
			$end5{$insertname} = $fidname;
		    }
		} elsif ($fidname =~ /$revreg/){
		    $insertname = $1;
		    $seq2ins{$fidname} = $insertname;
		    if (! exists $end3{$insertname} ||
			$end3{$insertname} lt $fidname){
			$end3{$insertname} = $fidname;
		    }
		} elsif ($fid =~ /$freg/){
		    $insertname = $1;
		    $seq2ins{$fid} = $insertname;
		    if (! exists $end5{$insertname} ||
			$end5{$insertname} lt $fid){
			$end5{$insertname} = $fid;
		    }
		} elsif ($fid =~ /$revreg/){
		    $insertname = $1;
		    $seq2ins{$fid} = $insertname;
		    if (! exists $end3{$insertname} ||
			$end3{$insertname} lt $fid){
			$end3{$insertname} = $fid;
		    }
		} # if forw or rev regexp match

		if (defined $insertname){
		    my $found = 0;

		    if (! exists $ins2id{$insertname}){
			$ins2id{$insertname} = getId();
		    }

		    for (my $l = 0; $l <= $#libregexp; $l++){
			if ($fidname =~ /$libregexp[$l]/){
			    $ins2lib{$insertname} = $libnames[$l];
			    $found = 1;
			    last;
			} elsif ($fid =~ /$libregexp[$l]/){
			    $ins2lib{$insertname} = $libnames[$l];
			    $found = 1;
			    last;
			}
		    }
		    if ($found == 0){
			$base->logError("Cannot find library for \"$insertname\"");
		    }

		    last;
		} # if found insert
	    } # for each pairreg
	} # if defined mates

	my $seqlen = length($frec);
	my @quals;
	if ($hasqual == 1){
	    @quals = split(' ', $qrec);
	    if ($#quals + 1 != $seqlen) {
		$base->bail("Fasta and quality disagree: $seqlen vs " . $#quals + 1 . "\n");
	    }
	} else {
	    for (my $q = 0; $q < $seqlen; $q++){ # fill in qualities with 20
		$quals[$q] = $DEFAULT_QUAL;
	    }
	}

	my $caqual = "";
	for (my $q = 0; $q <= $#quals; $q++){
	    my $qv = $quals[$q];
	    if ($qv > 60) {
		$qv = 60;
	    }
	    
	    $caqual .= chr(ord('0') + $qv);
	}
	
	if (! defined $silent && ($recId % 100 == 0)){
	    print STDERR "$recId\r";
	}
	
	if (! exists $clr{$fid}){
	    $clr{$fid} = "0,$seqlen";
	}

	$seq2id{$fidname} = $recId;
	$seq2id{$fid} = $recId;

	my $seq_lend;
	my $seq_rend;
	
	($seq_lend, $seq_rend) = split(',', $clr{$fid});
	
	if (defined $MAXSEQ && $seqlen > $MAXSEQ){
	    $frec = substr($frec, 0, $seq_rend + 1);
	    $caqual = substr($caqual, 0, $seq_rend + 1);
	    $seqlen = length($frec);
	    if (defined $MAXSEQ && $seqlen > $MAXSEQ){
		if (! defined $silent){
		    $base->logError("skipping sequence $fidname due to length $seqlen\n");
		}
		delete $seq2id{$fidname};
		delete $seq2id{$fid};
		next;
	    }
	}
	if ($seq_rend - $seq_lend < $MINSEQ){
	    if (! defined $silent){
		$base->logError("skipping sequence $fidname since it's short\n");
	    }
	    delete $seq2id{$fidname};
	    delete $seq2id{$fid};	
	    next;
	}

	if (! exists $seq2ins{$fid} && exists $seq2ins{$fidname}){
	    $seq2ins{$fid} = $seq2ins{$fidname};
	}

	if (! exists $seq2ins{$fid} ||
	    ! exists $ins2id{$seq2ins{$fid}}){
	    if ($hasxml || defined $mates){
		$base->logError("Found a sequence without a template - probably not in XML or mates file: $fidname\n");
		next;
	    } #else {
	#	$seq2ins{$fid} = 0; #insid
	#	$ins2id{0} = 0; #$insid;
	#    }
	}

	print TMPRED "{RED\n";                # read
	print TMPRED "iid:$recId\n";          
	print TMPRED "eid:$fid\n";
	print TMPRED "seq:\n";
	$frec =~ s/[^actgnACTGN]/N/g;
	for (my $s = 0; $s < $seqlen; $s += 60){
	    print TMPRED substr($frec, $s, 60), "\n";
	}
	print TMPRED ".\n";
	print TMPRED "qlt:\n";
	for (my $s = 0; $s < $seqlen; $s += 60){
	    print TMPRED substr($caqual, $s, 60), "\n";
	}
	if ($seq_rend > $seqlen){
	    $base->logError("right end of clear range $seq_rend > $seqlen - shrinking it\n");
	    $seq_rend = $seqlen;
	}
	print TMPRED ".\n";
	if (exists $ins2id{$seq2ins{$fid}}){
	    print TMPRED "frg:$ins2id{$seq2ins{$fid}}\n";
	}
	print TMPRED "clr:$seq_lend,$seq_rend\n";
	print TMPRED "}\n";
    } # while each read
    
    if (! defined $silent){
	print STDERR "done\n";
    }
    close(SEQ);
    close(QUAL);
}
close(TMPRED);

if (! defined $silent){
    print STDERR "doing fragments\n";
}

while (my ($ins, $id) = each %ins2id){
    print FRAG "{FRG\n";
    print FRAG "iid:$id\n";
    print FRAG "eid:$ins\n";
    print FRAG "lib:$lib2id{$ins2lib{$ins}}\n";
    print FRAG "typ:I\n";

    if ( exists $end5{$ins} && exists $end3{$ins} &&
         exists $seq2id{$end5{$ins}} && exists $seq2id{$end3{$ins}}){
        print FRAG "rds:$seq2id{$end5{$ins}},$seq2id{$end3{$ins}}\n";
    }

    print FRAG "}\n";
}

if (! defined $silent){
    print STDERR "putting it together\n";
}

if (-f "$tmprefix.red"){
    open(TMPRED, "$tmprefix.red") || $base->bail("Cannot open $tmprefix.red:$!\n");
    while (<TMPRED>){
        print FRAG;
    }
    close(TMPRED);
    unlink("$tmprefix.red") || $base->bail("Cannot remove $tmprefix.red: $!\n");
}
close(FRAG);

if (! defined $silent){
    print STDERR "done\n";
}

exit(0);


########
#
# The XML parser functions for parsing Trace Archive formatted .xml files

sub StartDocument
{
    if (! defined $silent){
	print "starting\n";
    }
}

sub EndDocument
{
    if (! defined $silent){
	print "done\n";
    }
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
	    if (! defined $silent){
		print "trace has no name???\n";
	    }
	}
	if (defined $clears && ! defined $clr{$seqId}){
	    return; # only handle reads with a clear range
	}
	if (! defined $library){
	    if (! defined $silent){
		print "trace $seqId has no library\n";
	    }
	}
	if (! defined $mean){
	    if (! defined $silent){
		print "library $library has no mean - replacing with 33333\n";
	    }
	    $means{$library} = 33333;
	    $mean = 33333;
	} else {
	    $means{$library} = $mean;
	}
	
	if (! defined $stdev){
	    if (! defined $silent){
		print "library $library has no stdev - replacing with 10% of $mean\n";
	    }
	    $stdevs{$library} = $mean * 0.1;
	} else {
	    $stdevs{$library} = $stdev;
	}
	
	if (! defined $template){
	    $template = "TEM_" . $TEM_ID++;
	    if (! defined $silent){
		print "trace $seqId has no template.  Setting to $template\n";
	    }
	} 
	
	if (! defined $end) {
	    if (! defined $silent){
		print "trace $seqId has no end\n";
	    }
	}
	
	if ($end eq "R"){
	    if (! exists $end3{$template} ||
		$seqId gt $end3{$template}){
		$end3{$template} = $seqId;
	    }
	}

	if ($end eq "F"){
	    if (! exists $end5{$template} ||
		$seqId gt $end5{$template}){
		$end5{$template} = $seqId;
	    }
	}
	
	
	if (defined $clipl && defined $clipr){
	    $clipr--;
	    # we don't care about clear ranges if defined elsewhere
	    if (! defined $clears) {$clr{$seqId} = "$clipl,$clipr";}
	}
	$seq2ins{$seqId} = $template;
	$ins2lib{$template} = $library;
	if (! defined $ins2id{$template}){
	    $ins2id{$template} = getId();
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
	    $seqId = $_;
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
		 $tag eq "clip_vector_left" ||
		 $tag eq "clip_left"){
	    if (! defined $clipl || $_ > $clipl){
		$clipl = $_;
	    }
	} elsif ($tag eq "clip_quality_right" ||
		 $tag eq "clip_vector_right" ||
		 $tag eq "clip_right"){
	    if (! defined $clipr || $_ < $clipr){
		$clipr = $_;
	    }
	}
    }
}

sub pi
{

}


## Some other useful functions

sub printFragHeader
{
    my $file = shift;

    my $date = localtime();

    print $file "{UNV\n";
    print $file "iid:1\n";
    print $file "com:\nGenerated by $ENV{USER} with tarchive2amos on $date\n.\n";
    print $file "}\n";
}

sub getId
{
    return $ID++;
}

