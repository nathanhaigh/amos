#!/usr/bin/perl
use strict;

my $VERBOSE = 0;

my $USAGE = "scale_ce_plot [options] coords ce > scaled.ce\n";

my $coordsfile = shift @ARGV or die $USAGE;
my $cefile     = shift @ARGV or die $USAGE;

open COORDS, "$coordsfile" or die "Can't open $coordsfile ($!)\n";
open CEFILE, "$cefile" or die "Can't open $cefile\n";

my %rinfo;
my %qinfo;

my %qalignments;
my %ralignments;

while (<COORDS>)
{
  s/^\s+//;

  my @fields = split /\s+/, $_;

  my $a;
  $a->{rstart} = $fields[0];
  $a->{rend}   = $fields[1];

  $a->{qstart} = $fields[3];
  $a->{qend}   = $fields[4];
  $a->{rc}     = 0;

  if ($a->{qstart} > $a->{qend})
  {
    $a->{qstart} = $fields[4];
    $a->{qend}   = $fields[3];
    $a->{rc}     = 1;
  }

  my $rid      = $fields[17];
  my $qid      = $fields[18];

  $a->{rid}    = $rid;
  $a->{qid}    = $qid;

  $rinfo{$rid}->{len} = $fields[11];
  $qinfo{$qid}->{len} = $fields[12];

  push @{$qalignments{$qid}}, $a;
  push @{$ralignments{$rid}->{$qid}}, $a;
}

sub translate
{
  my $qpos = shift @_;
  my $a    = shift @_;

  if ($VERBOSE) { print "translate $qpos: [$a->{rstart} $a->{rend}] [$a->{qstart} $a->{qend} $a->{rc}]\n"; }

  my $qalen = $a->{qend} - $a->{qstart};
  my $ralen = $a->{rend} - $a->{rstart}; 

  my $qdist = 0;

  if ($a->{rc} == 0)
  {
    $qdist = $qpos - $a->{qstart};
  }
  else
  {
    $qdist = $a->{qend} - $qpos;
  }

  return $a->{rstart} + $ralen * $qdist / $qalen;
}

sub interpolate
{
  my $qpos   = shift @_;
  my $aprior = shift @_;
  my $apost  = shift @_;

  if ($VERBOSE) 
  { 
    print "interpolate $qpos: [$aprior->{rstart} $aprior->{rend}]"; 
    print " [$aprior->{qstart} $aprior->{qend} $aprior->{rc}]";
    print " -- [$apost->{rstart} $apost->{rend}]";
    print " [$apost->{qstart} $apost->{qend} $apost->{rc}]\n";
  }

  my $qalen = 0;
  my $ralen = 0;
  my $qdist = 0;
  my $rpos  = 0;

  if (($aprior->{rc} == 0) && ($apost->{rc} == 0))
  {
    $qalen = $apost->{qstart} - $aprior->{qend};
    $ralen = $apost->{rstart} - $aprior->{rend};
    $qdist = $qpos - $apost->{qstart};
    $rpos  = $aprior->{qend};
  }
  elsif (($aprior->{rc} == 1) && ($apost->{rc} == 1))
  {
    $qalen = $apost->{qstart} - $aprior->{qend};
    $ralen = $apost->{rstart} - $aprior->{rend};
    $qdist = $qpos - $apost->{qstart};
    $rpos  = $apost->{qstart};

  }
  elsif ($aprior->{rc} == 0)
  {
    ## extend off the end of the prior alignment
    $qalen = $aprior->{qend} - $aprior->{qstart};
    $ralen = $aprior->{rend} - $aprior->{rstart};
    $qdist = $qpos-$aprior->{qend};

    $rpos  = $aprior->{qend};
  }
  elsif ($apost->{rc} == 0)
  {
    ## extend from the beginning of the post alignment
    $qalen = $aprior->{qend} - $aprior->{qstart};
    $ralen = $aprior->{rend} - $aprior->{rstart};
    $qdist = $qpos-$aprior->{qend};

    $rpos  = $aprior->{qend};
  }

  return $rpos + $ralen * $qdist / $qalen;
}



while (<CEFILE>)
{
  if (/^>(\S+)/)
  {
    print STDERR "Processing $1\n";
    next;
  }

  my @vals = split /\s+/, $_;

  my $qid   = $vals[0];
  my $qpos  = $vals[1];
  my $ce    = $vals[2];
  my $depth = $vals[3];

  next if (!exists $qalignments{$qid});

  my $spos = undef;

  my $aprior = undef;
  my $apost  = undef;

  foreach my $a (@{$qalignments{$qid}})
  {
    if (($qpos >= $a->{qstart}) && ($qpos <= $a->{qend}))
    {
      $spos = translate($qpos, $a);
      last;
    }

    if ($a->{qend} < $qpos)
    {
      if (!defined $aprior) { $aprior = $a; }
      else
      {
        my $adist = $qpos-$a->{qend};
        my $pdist = $qpos-$aprior->{qend};

        if ($adist < $pdist) { $aprior = $a; }
      }
    }

    if ($qpos < $a->{qstart})
    {
      if (!defined $apost) { $apost = $a; }
      else
      {
        my $adist = $a->{qstart} - $qpos;
        my $pdist = $apost->{qstart} - $qpos;

        if ($adist < $pdist) { $apost = $a; }
      }
    }
  }

  if (!defined $spos)
  {
    if ((defined $aprior) && (defined $apost))
    {
      $spos = interpolate($qpos, $aprior, $apost);
    }
  }

  if (defined $spos) { print "$spos\t$ce\t$depth\t$qid\t$qpos\n"; }
}









