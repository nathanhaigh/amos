#!/usr/bin/perl -w
use strict;
use Getopt::Long;
#use Math::Random;

my $USAGE = "mutate [options] fasta #events\n";

my $randseed = 123456;
my $help = 0;
my $prefix = "out";
my $eventmean = 10000;
my $eventstdev = 5000;

my $res = GetOptions("help"         => \$help,
                     "prefix=s"     => \$prefix,
                     "seed=s"       => \$randseed,
                     "mean=s"       => \$eventmean,
                     "stdev=s"      => \$eventstdev,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print "  Mutate a reference genome to include structural variations.\n";
  print "\n";
  print "Required\n";
  print " ref.fa         : path to reference genome\n";
  print " numevents      : number of structural variations to simulate\n";
  print "\n";
  print "Options\n";
  print " -prefix <name> : prefix to output genome, bedfile, and mutation report (default: $prefix)\n";
  print " -seed <val>    : seed for random number generator (default: $randseed)\n";
  print " -mean <val>    : mean length for events (default: $eventmean)\n";
  print " -stdev <val>   : stdev for events (default: $eventstdev)\n";
  
  exit 0;
}

my $fastafile = shift @ARGV or die $USAGE;
my $numevents = shift @ARGV or die $USAGE;

my @rr = split //, $randseed;
my $rrr = 0; foreach (@rr) { $rrr += ord($_); } 
srand($rrr);
#random_set_seed_from_phrase($randseed);

open REPORT, "> $prefix.report"
 or die "Can't open $prefix.report";

open BED, "> $prefix.bed"
  or die "Can't open $prefix.bed ($!)\n";

sub logmsg
{
  print @_;
  print REPORT @_;
}

logmsg "Simulating $numevents events into $fastafile\n";
logmsg "randseed=$randseed\n";
logmsg "mean=$eventmean\n";
logmsg "stdev=$eventstdev\n";

## Set up event types
###############################################################################

my %types;
$types{0}->{name}      = "del";
$types{0}->{copysrc}   = 0;
$types{0}->{maxcopies} = 1;
$types{0}->{delsource} = 0;
$types{1}->{inv}       = 0;

$types{1}->{name}       = "inv";
$types{1}->{copysrc}    = 0;
$types{1}->{maxcopies}  = 1;
$types{1}->{delsource}  = 0;
$types{1}->{inv}        = 1;

$types{2}->{name}       = "tan-dup";
$types{2}->{copysrc}    = 0;
$types{2}->{maxcopies}  = 3;
$types{2}->{delsource}  = 0;
$types{2}->{inv}        = 0;

$types{3}->{name}       = "tan-inv";
$types{3}->{copysrc}    = 0;
$types{3}->{maxcopies}  = 3;
$types{3}->{delsource}  = 0;
$types{3}->{inv}        = 1;

$types{4}->{name}       = "dup";
$types{4}->{copysrc}    = 1;
$types{4}->{maxcopies}  = 3;
$types{4}->{delsource}  = 0;
$types{4}->{inv}        = 0;

$types{5}->{name}       = "dup-inv";
$types{5}->{copysrc}    = 1;
$types{5}->{maxcopies}  = 3;
$types{5}->{delsource}  = 0;
$types{5}->{inv}        = 1;

$types{6}->{name}       = "trans";
$types{6}->{copysrc}    = 1;
$types{6}->{maxcopies}  = 3;
$types{6}->{delsource}  = 1;
$types{6}->{inv}        = 0;

$types{7}->{name}       = "trans-inv";
$types{7}->{copysrc}    = 1;
$types{7}->{maxcopies}  = 3;
$types{7}->{delsource}  = 1;
$types{7}->{inv}        = 1;


my $numtypes = scalar keys %types;

sub printevent
{
  my $e = $_[0];

  my $idx      = $e->{idx};
  my $parent   = $e->{parent};
  my $type     = $e->{type};
  my $typename = $types{$type}->{name};
  my $pos      = $e->{pos};
  my $len      = $e->{len};
  my $src      = $e->{src};
  my $copies   = $e->{copies};

  logmsg "Event $idx [$parent]:\t$typename\tpos:\t$pos\tlen:\t$len\tsrc:\t$src\tcopies:\t$copies\n";
}


## Load the sequence lengths
###############################################################################

open LENS, "getlengths $fastafile |"
  or die "Can't execute getlengths $fastafile ($!)\n";

my $totalspan = 0;

my @seqs;
my %seqinfo;
while (<LENS>)
{
  chomp;
  my ($id, $len) = split /\s+/, $_;

  logmsg "original len: $id => $len\n";

  $seqinfo{$id}->{len} = $len;
  $totalspan += $len;
  push @seqs, $id;

  last;
}

close LENS;


## Generate Events
###############################################################################

logmsg "\n";
logmsg "Generating Events\n";

my @events;

for (my $i = 0; $i < $numevents; $i++)
{
  my $type = $i % $numtypes;
  my $pos  = int(rand($totalspan));
  #my $len  = int(random_normal(1, $eventmean, $eventstdev));
  my $len = $eventmean + int(((rand()+rand()+rand()+rand()+rand()+rand()) - 3) * $eventstdev);
  my $copies = 1+int(rand($types{$type}->{maxcopies}));

  my $src = -1;

  if ($types{$type}->{copysrc})
  {
    $src = int(rand($totalspan));
  }

  my $e;
  $e->{idx}    = $i;
  $e->{parent} = "";
  $e->{type}   = $type;
  $e->{pos}    = $pos;
  $e->{len}    = $len;
  $e->{src}    = $src;
  $e->{copies} = $copies;

  printevent($e);
  push @events, $e;

  if ($types{$type}->{delsource})
  {
    my $s;
    $s->{idx}     = "$i.1"; 
    $s->{parent}  = $i;
    $s->{type}    = 0; # deletion
    $s->{pos}     = $src;
    $s->{len}     = $len;
    $s->{src}     = -1;
    $s->{copies}  = 1;

    printevent($s);
    push @events, $s
  }
}

## Sort events
###############################################################################

logmsg "\nsorting events\n";
@events = sort {$a->{pos} <=> $b->{pos}} @events;

foreach my $e (@events)
{
  printevent($e);
}


## Generate bed
###############################################################################

logmsg "\ngenerating bed\n";

my $segcount = 0;


sub printbed
{
  my $ref   = $_[0];
  my $start = $_[1];
  my $end   = $_[2];
  my $dir   = $_[3];

  $segcount++;
  my $segname = sprintf("seg%05d", $segcount); 

  print BED "$ref\t$start\t$end\t$segname\t$segcount\t$dir\n";
  logmsg    "$ref\t$start\t$end\t$segname\t$segcount\t$dir\n";
}

my $refname = $seqs[0];
my $reflen  = $seqinfo{$refname}->{len};

my $lastpos = 0;

foreach my $e (@events)
{
  printevent($e);

  my $type = $e->{type};
  my $typename = $types{$type}->{name};

  my $pos = $e->{pos};
  my $len = $e->{len};
  my $dir = ($types{$type}->{inv}) ? "-" : "+";

  if ($pos > $lastpos)
  {
    ## print up to event position
    printbed($refname, $lastpos, $pos, "+"); 
  }
  else
  {
    logmsg "WARNING: Overlapping events\n";
  }

  if ($types{$type}->{copysrc})
  {
    my $src    = $e->{src};
    my $endpos = $src + $len;

    for (my $c = 0; $c < $e->{copies}; $c++)
    {
      printbed($refname, $src, $endpos, $dir);
    }

    $lastpos = $e->{pos};
  }
  else
  {
    my $endpos = $pos+$len;

    if ($typename eq "del")
    {
      ## deletion, nothing to print
    }
    elsif ($typename eq "inv")
    {
      printbed($refname, $pos, $endpos, $dir);
    }    
    else
    {
      ## tandem-dup or tandem-inversion
      for (my $c = 0; $c < $e->{copies}+1; $c++) ## note copies+1 to get the original segment first
      {
        printbed($refname, $pos, $endpos, $dir);
      }
    }

    $lastpos = $endpos;
  }

  logmsg "\n";
}

## get the end of the sequence
printbed($refname, $lastpos, $reflen, "+");
