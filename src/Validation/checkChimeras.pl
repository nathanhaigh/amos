#!/usr/local/bin/perl

## Assumes input is list sorted by reference from get_chimeras.pl

my $END = 10; # bp away from end
my $LEN = 10000; # minimum contig size
my $NEAR = 5; # bases within which supporting breakpoint

my @lines;
my @breakpoints;
my %read2contig;
my $last = undef;
my %cool; # keep track of "good" sequences
while (<>){
    chomp;
    my @fields = split('\t', $_);

    if (defined $last && $fields[11] != $last){
	process_contig();
	@lines = ();
    }

    if ($fields[0] < $END && 
	($fields[3] > $fields[8] - $END || $fields[3] < $END)){
	$cool{$fields[12]} = 1;
    } # sequence falls off 5' contig end
    if ($fields[1] > $fields[7] - $END &&
	($fields[2] > $fields[8] - $END || $fields[2] < $END)){
	$cool{$fields[12]} = 1;
    } # sequence falls off 3' contig end
    if ($fields[2] < $END && $fields[3] > $fields[8] - $END){
	$cool{$fields[12]} = 1;
    } # sequence is contained in forward direction
    if ($fields[2] > $fields[8] - $END && $fields[3] < $END){
	$cool{$fields[12]} = 1;
    } # sequence is contained in reverse direction

    if ($fields[7] < $LEN) {next;} # short contig

    if ($fields[0] > $END && 
	$fields[2] < $fields[8] - $END && 
	$fields[2] > $END){
	push @lines, "$fields[0] $fields[2] $fields[11] $fields[12]";
    } #first coord is a breakpoint
    if ($fields[1] < $fields[7] - $END &&
	$fields[3] < $fields[8] - $END &&
	$fields[3] > $END){ 
	push @lines, "$fields[1] $fields[3] $fields[11] $fields[12]";
    } # second coord is breakpoint

	
#    print "$fields[11]\n";

    $last = $fields[11];
#    push @lines, $_;
}
process_contig();

for (my $i = 0; $i <= $#breakpoints; $i++){
    my @fields = split(' ', $breakpoints[$i]);
    my @ctgs = split(' ', $read2contig{$fields[3]});
    if ($#ctgs < 1){next;}
    for ($j = 0; $j <= $#ctgs; $j++){
	$ctgs{$ctgs[$j]} = 1;
    }
    print join(" ", @fields), " ", join(" ", keys %ctgs), "\n";
    %ctgs = ();
}

exit(0);

sub process_contig
{
    my @newlines;
    for (my $i = 0; $i <= $#lines; $i++){
	my @fields = split(' ', $lines[$i]);
	if (exists $cool{$fields[3]}){
	    next;
	}
	push @newlines, $lines[$i];
    }

    @lines = @newlines;

    if ($#lines < 1){return;} # not enough chimeras

    my %breaks = ();
    my @lineidx = ();
    for (my $i = 0; $i <= $#lines; $i++){
	my @fields = split(' ', $lines[$i]);
	$breaks{$i} = $fields[0];
#	print "breaks of $i = $fields[0]\n";
	push @lineidx, $i;
    }

    @lineidx = sort {$breaks{$a} <=> $breaks{$b}} @lineidx;

    $lastbreak = 0;

    for (my $i = 0; $i <= $#lineidx; $i++){
#	print $lines[$lineidx[$i]], "\n";
	if (! exists $breaks{$lineidx[$i]}){
	    die ("Weird: $lineidx[$i] $i\n");
	}
#	print "$breaks{$lineidx[$i]}\n";
	if ($breaks{$lineidx[$i]} - $lastbreak < $NEAR){
#	    print "$breaks{$lineidx[$i]} near $lastbreak\n";
	    while ($i <= $#lineidx &&
		   $breaks{$lineidx[$i]} - $lastbreak < $NEAR){
		push @breakpoints, $lines[$lineidx[$i]];
		my @fields = split(' ', $lines[$lineidx[$i]]);
		$read2contig{$fields[3]} .= "$fields[2] ";
		$lastbreak = $breaks{$lineidx[$i]};
		$i++;
	    }
	    push @breakpoints, $lines[$lineidx[$i - 1]];
	    my @fields = split(' ', $lines[$lineidx[$i - 1]]);
	    $read2contig{$fields[3]} .= "$fields[2] ";
	    $i--;
	}
	$lastbreak = $breaks{$lineidx[$i]};
    }
}
