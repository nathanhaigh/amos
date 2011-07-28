#!/usr/bin/perl

## chops the seq and qv lines to be the minimum of the two

my $trims = 0;
my $trimq  = 0;

my $trimsb = 0;
my $trimqb = 0;

while (<>)
{
  print $_;
  my $s = <>;  chomp $s;
  my $h2 = <>; chomp $h2;
  my $q = <>;  chomp $q;
  
  my $sl = length($s);
  my $ql = length($q);

  if ($sl < $ql)
  {
    $trimq++;
    $trimqb += $ql-$sl;
    $q = substr($q,0,$sl);
  }
  elsif ($ql < $sl)
  {
    $trims++;
    $trimsb += $sl-$ql;
    $s = substr($s,0,$ql);
  }

  print "$s\n$h2\n$q\n";
}

print STDERR "trimseq: $trims $trimsb bp  trimq: $trimq $trimqb bp\n";
