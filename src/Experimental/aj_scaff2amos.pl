#!/usr/bin/perl -w
use strict;

my $USAGE = "aj_scaff2amos.pl scaff bnk/CTG.map > scf.afg\n";

my $scaff = shift @ARGV or die $USAGE;
my $map = shift @ARGV or die $USAGE;

open SCAFF, "< $scaff" or die "Can't open $scaff ($!)\n";
open MAP, "< $map" or die "Can't open $map ($!)\n";

my %eid2iid;

while (<MAP>)
{
  if (/(\d+)\s+(\d+)\s+(\S+)/)
  {
    $eid2iid{$3} = $2;
  }
}

my $scaffid = undef;
my $iid = 1;
my $offset = 0;

sub endScaff
{
  print "}\n" if (defined $scaffid);
}


while (<SCAFF>)
{
  if (/>(\S+)/)
  {
    endScaff();

    $scaffid = $1;
    print "{SCF\n";
    print "eid:$scaffid\n";
    print "iid:$iid\n";
    $iid++;
    $offset = 0;
  }
  else
  {
    my @vals = split /\s+/, $_;
    my $ceid = $vals[0];
    my $oo   = $vals[1];
    my $len  = $vals[2];
    my $gap  = int($vals[3]);

    my $ciid = $eid2iid{$ceid};

    print "{TLE\n";
    if ($oo eq "BE") { print "clr:0,$len\n"; }
    else             { print "clr:$len,0\n"; }
    print "off:$offset\n";
    print "src:$ciid\n";
    print "}\n";

    $offset += $len;
    $offset += $gap;
  }
}

endScaff();


