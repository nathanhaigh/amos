#!/usr/bin/perl -w
use strict;

my $USAGE = "plot-breaks.pl [-c] 1con listofdelta.q\n";

my $CHECKFIX = 0;
my $BREAKSCMD = "find-query-breaks.pl -B";
my $tic = 10;


my $onecon = shift @ARGV;

if (defined $onecon && $onecon eq "-c") 
{ 
  $CHECKFIX = 1; 
  $BREAKSCMD .= " -c"; 
  $onecon = shift @ARGV; 
}

die $USAGE if !defined $onecon;

open LENS, "getlengths $onecon |" 
  or die "Can't open pipe for getlengths ($!)\n";

my %refinfo;
my $refcum = 1;
while (<LENS>)
{
  my ($name, $len) = split /\s+/, $_;
  $refinfo{$name}->{len} = $len;
  $refinfo{$name}->{startx} = $refcum;
  $refcum += $len + 10000;
}

my %breaks;

foreach my $delta (@ARGV)
{
  my $num = $1 if ($delta =~ /^(\d+)/);
  print STDERR "Processing $num ($delta)\n";


  open BREAKS, "$BREAKSCMD $delta |" 
    or die "Can't open pipe for $BREAKSCMD ($!)\n";

  my $ycoord = $num * 100;

  ## Draw Horizontal Line representing genome
  foreach my $ref (keys %refinfo)
  {
    my $s = $refinfo{$ref}->{startx};
    my $e = $s + $refinfo{$ref}->{len};

    print "$s\t$ycoord\t$e\t$ycoord\t100.0\n";
  }

  ## Draw a tic for every breakpoint
  while (<BREAKS>)
  {
    my @vals = split /\s+/, $_;
    my $binfo;

    my $ref  = $vals[0];
    my $type = $vals[1];
    my $loc  = $vals[2];

    die "\"$ref\" not found\n" if (!exists $refinfo{$ref});

    my $refx = $refinfo{$ref}->{startx};
    my $x = $loc + $refx;

    my $t = $ycoord + $tic;
    my $b = $ycoord - $tic;

    my $col = 0.0;

    if ($CHECKFIX) { $col = $vals[3] * 100.0; }
    print "$x\t$t\t$x\t$b\t$col\n";
  }
}


