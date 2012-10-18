#!/usr/bin/perl -w
use strict;
use AMOS::AmosLib;

## Find the longest sub-clear range without MAXNS Ns in a row
## if new clear range < MINFRGLEN, drop read and link
## if $PERMUTEN, change N's in clear range to random base

my $PERMUTEN = 1;
my $CHANGECLR = 1;

my $MAXNS = 6;
my $MINFRGLEN = 64;

my $VERBOSE = 0;

my %shortfrgs;
my @dnabases = qw/A C G T/;


while (my $rec = getRecord(\*STDIN))
{
  my ($id, $fields, $recs) = parseRecord($rec);

  if ($id eq "FRG")
  {
    my $acc = $fields->{acc};
    my $seqname = $fields->{src};
    chomp $seqname;

    my ($clrl, $clrr) = split ",", $fields->{clr};

    my $seq = $fields->{seq};
    $seq =~ s/\n//g;

    my @bases = split //, $seq;
    my $len = scalar @bases;

    
    print STDERR "$acc\t$seqname\t$clrl\t$clrr\t$len |" if $VERBOSE;

    my $beststart = -1;
    my $bestlen = 0;
    my $bestn = 0;

    my $curstart = -1;
    my $curn = 0;
    my $alln = 0;

    my $ncount = 0;

    my $i;
    for ($i = 0; $i < $len; $i++)
    {
      my $origb = $bases[$i];
      if ($origb eq "N")
      {
        $alln++;

        if ($PERMUTEN) { $bases[$i] = $dnabases[rand(4)]; }
      }

      next if ($i < $clrl || $i >= $clrr);

      if ($origb eq "N")
      {
        $ncount++;
        $curn++;

        print STDERR " $i" if $VERBOSE;

        if ($ncount == $MAXNS)
        {
          my $curend = $i - $ncount + 1;
          my $curlen = $curend - $curstart;
          $curn -= $ncount;

          print STDERR "\t[$curstart $curend]" if $VERBOSE;

          if ($curlen > $bestlen)
          {
            $bestlen = $curlen;
            $beststart = $curstart;
            $bestn = $curn;
          }

          $curstart = -1;
        }
      }
      else
      {
        if ($curstart == -1)
        {
          $curstart = $i;
          $curn = 0;
        }

        $ncount = 0;
      }
    }

    if ($curstart != -1)
    {
      my $curend = $clrr - $ncount;
      my $curlen = $curend - $curstart;
      $curn -= $ncount;

      print STDERR "\t[$curstart $curend $ncount]" if $VERBOSE;

      if ($curlen > $bestlen)
      {
        $bestlen = $curlen;
        $beststart = $curstart;
        $bestn = $curn;
      }
    }

    my $bestend = $beststart+$bestlen;

    if (!$CHANGECLR)
    {
      $beststart = $clrl;
      $bestend = $clrr;
      $bestlen = $bestend - $beststart;
    }

    my $diff = ($beststart != $clrl || $bestend != $clrr) ? 1 : 0;

    if ($diff || ($PERMUTEN && $alln) || $VERBOSE)
    {
      print STDERR "$acc\t$seqname\t$clrl\t$clrr\t$len |" if !$VERBOSE;
      print STDERR "\t$beststart\t$bestend\t|\t$bestn\t$alln";
      print STDERR " **" if $diff;
      print STDERR "\n";
    }
 
    if ($bestlen >= $MINFRGLEN)
    {
      my $skipline = 0;

      foreach (split /\n/, $rec)
      {
        if ($skipline) { $skipline = 0 if /^\.$/; }
        elsif (/^clr:/) { print "clr:$beststart,$bestend\n"; }
        elsif (/^seq:/)
        {
          print "seq:\n";
          for (my $i = 0; $i < $len; $i+=60)
          {
            my $j = ($i+59 < $len) ? $i+59 : $len-1;
            print join ("", @bases[$i .. $j]), "\n";
          }
          print ".\n";

          $skipline = 1;
        }
        else { print "$_\n"; }
      }
    }
    else
    {
      print STDERR "$seqname too short, dropping\n";
      $shortfrgs{$acc} = 1;
    }
  }
  elsif ($id eq "LKG")
  {
    my $fg1 = $fields->{fg1};
    my $fg2 = $fields->{fg2};

    if (exists $shortfrgs{$fg1} ||
        exists $shortfrgs{$fg2})
    {
      print STDERR "Skipping mate between $fg1 and $fg2\n";
    }
    else
    {
      print $rec;
    }
  }
  else
  {
    print $rec;
  }
}
