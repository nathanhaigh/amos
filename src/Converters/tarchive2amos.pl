#!/usr/local/bin/perl

use strict;

use TIGR::AmosLib;
use TIGR::ParseFasta;
use TIGR::Foundation;
use XML::Parser;
use IO::Handle;

my $ENDTRIM = 40;
my $MINSEQ = 100;
my $MAXSEQ = 2048;

my $tag; # XML tag
my $library;
my $template;
my $clipl;
my $clipr;
my $mean;
my $stdev;
my $end;
my $seqId;

my %end5;       # the reads at the 5' resp 3' end of an insert
my %end3;
my %means;      # mean and standard deviation for libraries
my %stdevs;
my %clr;        # clear range for each sequence
my %seq2ins;    # insert name for each sequence
my %ins2lib;    # library name for each insert
my %lib2id;     # mapping from library name to eid
my %ins2id;     # mapping from insert name to eid
my %seq2id;     # mapping from read name to eid

my $gzip = "gzip";

my $base = new TIGR::Foundation;

if (! defined $base) {
    die ("Walk, do not run, to the nearest exit!\n");
}

my $VERSION = '1.0 ($Revision$)';
$base->setVersionInfo($VERSION);

my $HELPTEXT = qq~
    tarchive2ca -o <out_prefix> [-c <clear_ranges>] fasta1 ... fastan

   <out_prefix>   - prefix for the output files
   <clear_ranges> - file containing clear ranges for the reads.  If this file
           is provided, any sequence that does not appear in it is excluded
           from the output.
   fasta1 ... fastan - list of files to be converted.
           The program assumes that for each program called <file>.seq there
           is a <file>.qual and a <file>.xml.  
    ~;

$base->setHelpInfo($HELPTEXT);

my $outprefix;
my $clears;
my $ID = 1;
my $silent;
my $err = $base->TIGR_GetOptions("o=s"    => \$outprefix,
				 "c=s"    => \$clears,
                                 "i=i"    => \$ID,
				 "silent" => \$silent);
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
my %seqId;
my %lib ;

my $fragname = "$outprefix.afg";

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
open(FRAG, ">$fragname") || die ("Cannot open $fragname: $!");
printFragHeader(\*FRAG);


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
	$base->bail("Cannot find the xml file corresponding to $seqname");
    }

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


# now we are ready to print the library information
while (my ($lib, $mean) = each %means){
    my $libid = getId();
    $lib2id{$lib} = $libid;

    print FRAG "{LIB\n";
    print FRAG "act:A\n";
    print FRAG "eid:$libid\n";
    print FRAG "com:\n";
    print FRAG "$lib\n";
    print FRAG ".\n";
    print FRAG "{DST\n";
    print FRAG "mea:$mean\n";
    print FRAG "std:$stdevs{$lib}\n";
    print FRAG "}\n";
    print FRAG "}\n";
}

# and the fragment information
while (my ($frg, $lib) = each %ins2lib){
    my $insid = getId();
    $ins2id{$frg} = $insid;
    print FRAG "{FRG\n";
    print FRAG "act:A\n";              # ADD
    print FRAG "eid:$insid\n";     
    print FRAG "com:\n";
    print FRAG "$frg\n";               # original fragment id
    print FRAG ".\n";
    print FRAG "lib:$lib2id{$lib}\n";  
    print FRAG "typ:I\n";              # INSERT
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
    } elsif (-e "qual.$prefix.gz") {
	$qualname = "qual.$prefix.gz";
    } elsif (-e "$prefix.qual"){
	$qualname = "$prefix.qual";
    } elsif ( -e "$prefix.qual.gz") {
	$qualname = "$prefix.qual.gz";
    } else {
	$base->bail("Cannot find the quality file corresponding to $seqname");
    }


    if ($seqname =~ /\.gz$/){
	open(SEQ, "$gzip -dc $seqname |") ||
	    $base->bail("Cannot open $seqname: $!\n");
    } else {
	open(SEQ, "$seqname") ||
	    $base->bail("Cannot open $seqname: $!\n");
    }
    
    if ($qualname =~ /\.gz$/){
	open(QUAL, "$gzip -dc $qualname |") ||
	    $base->bail("Cannot open $qualname: $!\n");
    } else {
	open(QUAL, "$qualname") ||
	    $base->bail("Cannot open $qualname: $!\n");
    }
    
    if (! defined $silent){
	print STDERR "Parsing $seqname and $qualname\n";
    }

    my $seqparse = new ParseFasta(\*SEQ);
    my $qualparse = new ParseFasta(\*QUAL, ">", " ");

    my ($fhead, $frec) = $seqparse->getRecord(); 
    my ($qhead, $qrec) = $qualparse->getRecord();

    do {
	$fhead =~ /^(\S+) ?(\S+)?/;
	my $fid = $1;
	my $fidname = $2;

	if (! defined $fidname || $fidname eq "bases"){
	    $fidname = $fid;
	}
	
	$qhead =~ /^(\S+)/;
	my $qid = $1;
	
	if ($fid != $qid){
	    die ("fasta and qual records have different IDs: $fid vs $qid\n");
	}

	# if TIGR formatted, fetch the clear range
	if ($fhead =~ /^(\S+) \d+ \d+ \d+ (\d+) (\d+)/){
	    my $l = $2;
	    my $r = $3;
	    $l--;
	    $fidname = $1;
	    $clr{$fidname} = "$l,$r";
	}

	my $recId = getId();

	my $seqlen = length($frec);
	my @quals = split(' ', $qrec);
	if ($#quals + 1 != $seqlen) {
	    die ("Fasta and quality disagree: $seqlen vs " . $#quals + 1 . "\n");
	}

	my $caqual = "";
	for (my $q = 0; $q <= $#quals; $q++){
	    my $qv = $quals[$q];
	    if ($qv > 60) {
		$qv = 60;
	    }
	    
	    $caqual .= chr(ord('0') + $qv);
	}
	
	if (! defined $silent){
	    print STDERR "$recId $fidname\r";
	}
	
	if (! exists $clr{$fidname}){
	    if (defined $clears){
		next;
	    }
	    if (! defined $silent){
		print "$fid has no clear range\n";
	    }
	    $clr{$fidname} = "0,$seqlen";
	} 

	$seqId{$fidname} = $recId;

	my $seq_lend;
	my $seq_rend;
	if (! defined $clears){
	    ($seq_lend, $seq_rend) = split(',', $clr{$fidname});
	} else {
	    if (! exists $clr{$fid}) {
		if (! defined $silent) {
		    print "sequence $fidname has no clear range\n";
		}
		delete $seqId{$fidname};
		next;
	    }
	    ($seq_lend, $seq_rend) = split(',', $clr{$fid});
	}

	if ($seqlen > $MAXSEQ){
	    $frec = substr($frec, 0, $seq_rend + 1);
	    $caqual = substr($caqual, 0, $seq_rend + 1);
	    $seqlen = length($frec);
	    if ($seqlen > $MAXSEQ){
		if (! defined $silent){
		    print "skipping sequence $fidname due to length $seqlen\n";
		}
		delete $seqId{$fidname};
		next;
	    }
	}
	    if ($seq_rend - $seq_lend < $MINSEQ){
		if (! defined $silent){
		print "skipping sequence $fidname since it's short\n";
	    }
	    delete $seqId{$fidname};
	    next;
	}

	print FRAG "{RED\n";                # read
	print FRAG "act:A\n";               # ADD
	print FRAG "eid:$recId\n";          
	print FRAG "com:\n$fidname\n.\n";
	print FRAG "seq:\n";
	$frec =~ s/[^actgnACTGN]/N/g;
	for (my $s = 0; $s < $seqlen; $s += 60){
	    print FRAG substr($frec, $s, 60), "\n";
	}
	print FRAG ".\n";
	print FRAG "qlt:\n";
	for (my $s = 0; $s < $seqlen; $s += 60){
	    print FRAG substr($caqual, $s, 60), "\n";
	}
	if ($seq_rend > $seqlen){
	    print "right end of clear range $seq_rend > $seqlen - shrinking it\n";
	    $seq_rend = $seqlen;
	}
	print FRAG ".\n";
	print FRAG "frg:$ins2id{$seq2ins{$fidname}}\n";
	print FRAG "clr:$seq_lend,$seq_rend\n";
	print FRAG "}\n";

    } while (($fhead, $frec) = $seqparse->getRecord(), ($qhead, $qrec) = $qualparse->getRecord());
    
    if (! defined $silent){
	print STDERR "done\n";
    }
    close(SEQ);
    close(QUAL);
}

if (! defined $silent){
    print STDERR "doing mates\n";
}

while (my ($ins, $lib) = each %ins2lib){
    if (exists $end5{$ins} && exists $end3{$ins}){
	if (! exists $seqId{$end5{$ins}} ||
	    ! exists $seqId{$end3{$ins}}){
	    next;
	}
	my $id = getId();
	print FRAG "{MTP\n";
	print FRAG "eid:$id\n";
	print FRAG "rd1:$seqId{$end5{$ins}}\n";
	print FRAG "rd2:$seqId{$end3{$ins}}\n";
	print FRAG "com:\n";
	print FRAG "$ins\n";
	print FRAG ".\n";
	print FRAG "typ:E\n"; # END READS
	print FRAG "}\n";
    }
}

if (! defined $silent){
    print STDERR "done\n";
}
close(FRAG);


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
	    if (! defined $silent){
		print "trace $seqId has no template\n";
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
	    $clr{$seqId} = "$clipl,$clipr";
	}
	$seq2ins{$seqId} = $template;
	$ins2lib{$template} = $library;
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


## Some other useful functions

sub printFragHeader
{
    my $file = shift;

    my $date = localtime();

    print $file "{UNV\n";
    print $file "act:A\n";
    print $file "eid:1\n";
    print $file "com:\nGenerated by tarchive2amos on $date\n.\n";
    print $file "}\n";
}

sub getId
{
    return $ID++;
}

