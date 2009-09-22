#!/usr/bin/perl -w
use strict;

my $singletons = 0;
my $ctgcnt = 0;
my $degcnt = 0;
my $totalreads = 0;
my $dupplaced  = 0;


sub printRead
{
  my $readid = shift;
  my $record = shift;

  $totalreads++;

  if (!defined $record->{FRGCTG} &&
      !defined $record->{FRGDEG})
  {
    $singletons++;
    return;
  }

  my $frg = $record->{FRG};

  ## update clear
  if (defined $record->{AFG})
  {
    my @frgfields = split /\t/, $frg;
    for (my $i = 0; $i < scalar @frgfields; $i++)
    {
      my $d = $frgfields[$i];

      if ($d =~ /^clr:/)
      {
     #   print STDERR "Reseting clr from $frgfields[$i] to $record->{AFG}\n";
        $frgfields[$i] = $record->{AFG};
      }
    }

    $frg = join("\t", @frgfields);
  }

  my $placed = 0;

  ## print frg for each ctg
  if (defined $record->{FRGCTG})
  {
    foreach my $ctgid (@{$record->{FRGCTG}})
    {
      print "$ctgid\tCTGFRG\t$frg\n";
      $ctgcnt++;
      $placed++;
    }
  }

  ## print frg for each deg
  if (defined $record->{FRGDEG})
  {
    foreach my $degid (@{$record->{FRGDEG}})
    {
      print "$degid\tDEGFRG\t$frg\n";
      $degcnt++;
      $placed++;
    }
  }

  if ($placed > 1)
  {
    $dupplaced++;
  }
}


my $record;
my $readid = undef;

while (<>)
{
  chomp;

  my @vals = split /[\t]/, $_;

  my $curid = shift @vals;
  my $type  = shift @vals;

  if (defined $readid && $readid ne $curid)
  {
    printRead($readid, $record);
    $record = undef;
  }

  $readid = $curid;

  if ($type eq "FRG")
  {
    $record->{FRG} = $_;
  }
  elsif ($type eq "AFG")
  {
    foreach my $i (@vals)
    {
      if ($i =~ /^clr:/)
      {
        $record->{AFG} = $i;
      }
    }
  }
  elsif ($type eq "FRGCTG")
  {
    push @{$record->{FRGCTG}}, $vals[0];
  }
  elsif ($type eq "FRGDEG")
  {
    push @{$record->{FRGDEG}}, $vals[0];
  }
  else
  {
    die "Unknown type: $_\n";
  }
}

printRead($readid, $record);

print STDERR "reporter:counter:prepare,totalreads,$totalreads\n";
print STDERR "reporter:counter:prepare,singletons,$singletons\n";
print STDERR "reporter:counter:prepare,ctgreads,$ctgcnt\n";
print STDERR "reporter:counter:prepare,degreads,$degcnt\n";
print STDERR "reporter:counter:prepare,dupplaced,$dupplaced\n";
