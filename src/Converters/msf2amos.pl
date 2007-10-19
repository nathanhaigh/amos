#!/usr/local/bin/perl

use TIGR::Foundation;
use AMOS::AmosLib;

my $base = new TIGR::Foundation;
my $MULT = 3;  # multiplicity for accepting a conflict

if (! defined $base) {
    die ("Walk, do not run, to the nearest exit!\n");
}

my $VERSION = '$Revision$ ';
$base->setVersionInfo($VERSION);

my $HELPTEXT = qq~
.USAGE.
msf2amos [-i <msffile> | <msffile>] [-o <dfile>]

.DESCRIPTION.
Converts a multi-alignment .msf file into a DIMACS style graph file where
each node is a read and an edge is created if the reads disagree in the
multiple alignment.

.OPTIONS.
  -i <msffile>  input file in MSF format.
  -o <dfile>    output file in DIMACS graph format.  If not provided defaults to
                STDOUT
  if <msffile> not provided, input defaults to STDIN

.KEYWORDS.
  converter, multi-alignment 
   ~;

my $outfile;
my $infile;

my $err = $base->TIGR_GetOptions("o=s"    => \$outfile,
				 "i=s"    => \$infile);
if ($err == 0) {
    $base->bail("Command line processing failed\n");
}

if (! defined $infile && $#ARGV >= 0){
    $infile = $ARGV[0];
}

if (defined $infile){
    open(STDIN, $infile) || die ("Cannot open $infile: $!\n");
}

if (defined $outfile) {
    open(STDOUT, ">$outfile") || die ("Cannot open $outfile: $!\n");
}

my $started = undef;
my $off = undef;
my %sequences;
my %ids;
my $ID = 0;
while (<STDIN>){
    if (/^\/\//){
	$started = 1;  # started the multiple alignment
	next;
    }
    if (! defined $started){next;}
    if (/^$/){ # next block
#	if (! defined $off){
#	    $off = 0;
#	} else {
#	    $off += 50;
#	}
	next;
    }
    
    my @fields = split(/\s+/, $_);
    if ($fields[0] eq ""){
	@fields = shift @fields;
    }
    if ($fields[1] !~ /[actgACTG-]+/){
	next; # probably empty line or position headers
    }
    my $name = $fields[0];
    my $seq = join("", @fields[1..$#fields]);

    if (! exists $ids{$name}){
	$ids{$name} = ++$ID;
    }

    $sequences{$ids{$name}} .= $seq;
}

#print "READ MSF\n";

my %seqoff; #where each sequence starts
my %ends;
my @seqnames;
while (my ($name, $seq) = each %sequences){
    $sequences{$name} =~ s/-*$//;
    $ends{$name} = length($sequences{$name});
    $sequences{$name} =~ s/^(-*)([^-].*)$/$2/;
    $seqoff{$name} = length($1);
    push (@seqnames, $name);
#    print "sequence $name is $seqoff{$name}: $sequences{$name}\n";
}

if ($#seqnames + 1 < 2 * $MULT) { # not enough seqs for conflict
    exit(0);
}

#print "PROCESSED SEQS\n";
my @ends = @seqnames;
@ends = sort {$ends{$a} <=> $ends{$b}} @ends;   # seq names sorted by end
@seqnames = sort {$seqoff{$a} <=> $seqoff{$b}} @seqnames; # sorted by start

#my $next = $seqoff{$seqnames[1]};

#print "DONE SoRTIN\n";

my $nextstart = 0;
my $nextend = 0;
my %active;

my %conflicts;
#$active{$seqnames[0]} = 1;
for (my $b = 0; $b < $ends{$ends[$#ends]}; $b++){
    # remove any reads that end
    while ($b == $ends{$ends[$nextend]}){
#	print "removing $ends[$nextend]\n";
	delete $active{$ends[$nextend]};
	$nextend++;
    }
    # add any reads that begin
    while (exists $seqoff{$seqnames[$nextstart]} && $b == $seqoff{$seqnames[$nextstart]}){
#	print "adding $seqnames[$nextstart]\n";
	$active{$seqnames[$nextstart]} = 1;
	$nextstart++;
    }

    my %bases = (); # bases at this position
    my %rds = ();

#    print "column $b is: ";
    while (my ($rd, $junk) = each %active){
#	print "$rd,", substr($sequences{$rd}, $b - $seqoff{$rd}, 1), " ";
	$bases{substr($sequences{$rd}, $b - $seqoff{$rd}, 1)}++;
	$rds{substr($sequences{$rd}, $b - $seqoff{$rd}, 1)} .= "$rd ";
    }
#    print "\n";
    my $nbases = 0;
    while (my ($base, $numocc) = each %bases){
	if ($numocc < $MULT) {
	    delete $bases{$base};
	    delete $rds{$base};
	} else {
	    $nbases++;
	}
    }
    if ($nbases > 1){ # conflict exists
	print STDERR "got conflict at base $b: $nbases\n";
	my @bases = keys %rds;
	for (my $i = 0; $i <  $#bases; $i++){
	    my @rdsa = split(" ", $rds{$bases[$i]});
	    for (my $j = $i + 1; $j <= $#bases; $j++){
		my @rdsb = split(" ", $rds{$bases[$j]});

		for (my $k = 0; $k <= $#rdsa; $k++){
		    for (my $l = 0; $l <= $#rdsb; $l++){
			if ($rdsa[$k] lt $rdsb[$l]){
			    $conflicts{"$rdsa[$k] $rdsb[$l]"}++;
			} else {
			    $conflicts{"$rdsb[$l] $rdsa[$k]"}++;
			}
		    }
		}
	    }
	}
    }
}

my @conflicts = keys %conflicts;

my $nseqs = $#seqnames + 1;
my $nedges = $#conflicts + 1;

print "p edge $nseqs $nedges\n";
for (my $i = 1; $i <= $nseqs; $i++){
    print "n $i\n";
}

for (my $i = 0; $i <= $#conflicts; $i++){
    print "e $conflicts[$i]\n";
}
