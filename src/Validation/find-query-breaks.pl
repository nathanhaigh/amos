#!/usr/bin/perl -w
use strict;

my $USAGE = "find-query-breaks.pl deltafile [trim]\n";
my $TRIMWIGGLE = 10;

my $deltafile = shift @ARGV or die $USAGE;
$TRIMWIGGLE = shift @ARGV if scalar @ARGV;


my $cmd = "show-coords -Hcql $deltafile";

open COORDS, "$cmd |" or die "Can't run $cmd ($!)\n";

sub printAlignment
{
  my $align = shift;
  my $rc = ($align->{qrc}) ? "[rc]" : "[]";
  print "$align->{rid} {1 [$align->{rstart},$align->{rend}] $align->{rlen}} | $align->{qid} {1 [$align->{qstart},$align->{qend}] $align->{qlen}} $rc ";
}

while (<COORDS>)
{
  #print $_;

  my @vals = split /\s+/, $_;

  my $align;

  $align->{rstart} = $vals[1];
  $align->{rend}   = $vals[2];

  $align->{qstart} = $vals[4];
  $align->{qend}   = $vals[5];

  $align->{ralen}   = $vals[7];
  $align->{qalen}   = $vals[8];

  $align->{pid}    = $vals[10];

  $align->{rlen}   = $vals[12];
  $align->{qlen}   = $vals[13];

  $align->{rid}    = $vals[18];
  $align->{qid}    = $vals[19];

  $align->{qrc}    = ($align->{qend} < $align->{qstart}) ? 1 : 0;

  if ($align->{qrc})
  {
    my $t = $align->{qstart};
    $align->{qstart} = $align->{qend};
    $align->{qend} = $t;
  }


  my $breakcount = 0;

  if (($align->{qstart} - $TRIMWIGGLE) > 1)
  {
    my $dist = $align->{qstart} - 1;
    printAlignment($align);
    print "Start Break: ($dist)\n";
    $breakcount++;
  }

  if (($align->{qend} + $TRIMWIGGLE) < $align->{qlen})
  {
    my $dist = $align->{qlen} - $align->{qend};
    printAlignment($align);
    print "End Break: ($dist)\n";
    $breakcount++;
  }

  #print $_ if $breakcount;
}
