#!/usr/bin/perl -w
use strict;

use AMOS::AmosFoundation;

my $TRIMWIGGLE = 10;
my $CIRCLEWIGGLE = 0;
my $MINLENGTH = 0;

my $HELPTEXT = qq~
Find alignment breaks in query sequences

  find-query-breaks.pl [options] deltafile

  -t <val> Minimum length of alignment break to report (Default: 10)
  -c <val> Minimum distance to edge of reference sequence to report (Default: 0)
  -l <val> Minimum length of query sequence to report (Default: 0)
~;

sub printAlignment
{
  my $align = shift;
  my $rc = ($align->{qrc}) ? "[rc]" : "[]";
  print "$align->{rid} {1 [$align->{rstart},$align->{rend}] $align->{rlen}} | $align->{qid} {1 [$align->{qstart},$align->{qend}] $align->{qlen}} $rc ";
}

my $base = new AMOS::AmosFoundation;
$base->setHelpText($HELPTEXT);
$base->setUsage("find-query-breaks.pl [options] deltafile");

my $err = $base->getOptions("t=i" => \$TRIMWIGGLE,
                            "c=i" => \$CIRCLEWIGGLE,
                            "l=i" => \$MINLENGTH);

if (!$err || 0 == scalar @ARGV) { $base->bail("Command line parsing failed. See -h option"); }

my $multidelta = (scalar @ARGV) > 1;

foreach my $deltafile (@ARGV)
{
  print ">$deltafile\n" if $multidelta;

  my $cmd = "show-coords -Hcrl $deltafile";
  open COORDS, "$cmd |" or die "Can't run $cmd ($!)\n";

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

    next if ($align->{qlen} < $MINLENGTH);


    my $breakcount = 0;


    if (( $align->{qrc} && ($align->{rlen} - $align->{rend} <= $CIRCLEWIGGLE)) ||
        (!$align->{qrc} && ($align->{rstart} - 1            <= $CIRCLEWIGGLE)))

    {

    }
    elsif (($align->{qstart} - 1) > $TRIMWIGGLE)
    {
      my $dist = $align->{qstart} - 1;
      printAlignment($align);
      print "Start Break: $dist\n";
      $breakcount++;
    }

    if ((!$align->{qrc} && ($align->{rlen} - $align->{rend} <= $CIRCLEWIGGLE)) ||
        ( $align->{qrc} && ($align->{rstart} - 1            <= $CIRCLEWIGGLE)))

    {

    }
    elsif (($align->{qlen} - $align->{qend}) > $TRIMWIGGLE)
    {
      my $dist = $align->{qlen} - $align->{qend};
      printAlignment($align);
      print "End Break: $dist\n";
      $breakcount++;
    }

    #print $_ if $breakcount;
  }
}
