#!/usr/bin/perl -w
use strict;

my $WINDOW = 25;

sub countgc
{
  my $str = $_[0];
  my $g = ($str =~ tr/gGcC//);

  return $g;
}

sub processContig
{
  my $seq = $_[0];

  return if !defined $seq;

  if (length($seq) > $WINDOW*10)
  {
    my $gc = countgc(substr($seq, 0, $WINDOW));
    print "E\t$gc\n";

    for (my $pos = $WINDOW; $pos+1.5*$WINDOW < length($seq); $pos+=$WINDOW)
    {
      my $wstr = substr($seq, $pos, $WINDOW);
      $gc = countgc($wstr);
      print "I\t$gc\n";
    }

    $gc = countgc(substr($seq, -$WINDOW));
    print "E\t$gc\n";
  }
}


my $seq = undef;

while (<>)
{
  if (/^>/)
  {
    processContig($seq);
    $seq = "";
  }
  else
  {
    chomp;
    $seq .= $_;
  }
}

processContig($seq);
