#!/usr/bin/perl -w
use strict;

my $ERROR_THRESHOLD = 1.0;
my $TRIMREADS = 1;

my $USAGE = "ovl2lay.pl RED.map idx.iid.eid.left.right < dump-olaps-store > lay.afg\n";

my $readmap = shift @ARGV or die $USAGE;
my $readinfofile = shift @ARGV or die $USAGE;

open MAP, "< $readmap" or die "Can't open $readmap ($!)\n";

my %eid2iid;

my $count = 0;
print STDERR "Loading AMOS readids from $readmap... ";

while (<MAP>)
{
  next if /^RED/;
  my @vals = split /\s+/, $_;

  $eid2iid{$vals[2]} = $vals[1];
  $count++;
}
print STDERR "$count loaded.\n";


$count = 0;
print STDERR "Loading readinfo from $readinfofile... ";

my %readinfo;
open INFO, "< $readinfofile" or die "Can't open $readinfofile ($!)\n";
while (<INFO>)
{
  my @vals = split /\s+/, $_;
  my $idx = $vals[0];

  my $iid   = $vals[1];
  my $eid   = $vals[2];
  my $start = $vals[3];
  my $end   = $vals[4];

  $readinfo{$idx}->{iid}   = $iid;
  $readinfo{$idx}->{eid}   = $eid;

  $readinfo{$idx}->{start} = $start;
  $readinfo{$idx}->{end}   = $end;
  $count++;
}

print STDERR " $count loaded\n";

sub printLay
{
  return if $TRIMREADS;

  my $maxahang = undef;
  foreach my $ovl (sort {$a->{ahang} <=> $b->{ahang}} @_)
  {
    if (!defined $maxahang) { $maxahang = $ovl->{ahang}; } 

    my $bread = $ovl->{bread};
    my $oo    = $ovl->{oo};

    my $eid = $readinfo{$bread}->{eid};
    my $iid = $eid2iid{$eid};
    my $start = $readinfo{$bread}->{start};
    my $end   = $readinfo{$bread}->{end};

    if ($oo ne "N")
    {
      my $t = $start; $start = $end; $end = $t;
    }

    my $offset = $ovl->{ahang} - $maxahang;

    print "{TLE\n";
    print "clr:$start,$end\n";
    print "off:$offset\n";
    print "src:$iid\n";
    print "}\n";
  }
}


my @overlaps;

my $num = 1;
my $read = undef;
while (<>)
{
  my @vals = split /\s+/, $_;

  my $aread = $vals[1];

  my $ovl;
  $ovl->{bread} = $vals[2];
  $ovl->{oo}    = $vals[3];
  $ovl->{ahang} = $vals[4];
  $ovl->{bhang} = $vals[5];
  $ovl->{err}   = $vals[6];
  $ovl->{cor}   = $vals[7];

  next if ($ovl->{err} > $ERROR_THRESHOLD);

  if (!defined $read || $aread ne $read)
  {
    if (defined $read) { printLay(@overlaps); @overlaps = (); print "}\n"; }

    $read = $aread;

    my $start = $readinfo{$aread}->{start};
    my $end   = $readinfo{$aread}->{end};
    my $eid   = $readinfo{$aread}->{eid};
    my $iid   = $eid2iid{$eid};

    print "{LAY\n";
    print "iid:$num.$aread\n"; $num++;
    print "eid:$eid.lay\n";

    if ($TRIMREADS)
    {
      print "{TLE\n";
      print "clr:$start,$end\n";
      print "off:0\n";
      print "src:$iid\n";
      print "}\n";
    }
    else
    {
      my $aovl;
      $aovl->{bread} = $aread;
      $aovl->{oo} = "N";
      $aovl->{ahang} = 0;
      $aovl->{err} = 0;
      $aovl->{cor} = 0;

      push @overlaps, $aovl;
    }
  }

  if ($TRIMREADS)
  {
    my $bread = $ovl->{bread};
    my $oo    = $ovl->{oo};
    my $ahang = $ovl->{ahang};
    my $bhang = $ovl->{bhang};

    my $eid = $readinfo{$bread}->{eid};
    my $iid = $eid2iid{$eid};
    my $start = $readinfo{$bread}->{start};
    my $end   = $readinfo{$bread}->{end};

   # print "> $eid $iid $start $end $_";

    if ($oo eq "N")
    {
      if ($ahang < 0) { $start += -$ahang; }
      if ($bhang > 0) { $end   -= $bhang;  }
    }
    else
    {
      if ($ahang < 0) { $end   -= -$ahang; }
      if ($bhang > 0) { $start +=  $bhang; }

      my $t = $start; $start = $end; $end = $t;
    }

    my $offset = 0;
    if ($ahang > 0) { $offset = $ahang; }

    print "{TLE\n";
    print "clr:$start,$end\n";
    print "off:$offset\n";
    print "src:$iid\n";
    print "}\n";
  }
  else
  {
    push @overlaps, $ovl;
  }
}

if (defined $read) { printLay(@overlaps); @overlaps = (); print "}\n"; }
