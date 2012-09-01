#!/usr/bin/perl -w

my $goodfile = shift @ARGV;

if (!defined $goodfile || ! -r $goodfile)
{
  die "Usage: rearrangeSeqs.pl good.{seq,qual,contig} [copy.{seq,qual}]\n";
}

my @seqlist;
my $seqname = undef;
my %copysequences;

## Load the original
open GOOD, "< $goodfile" or die "Couldn't open $goodfile";
while (<GOOD>)
{
  push @seqlist, $1 if (/^>(\S+)/);
  push @seqlist, $1 if (/^#(\S+)\(/);
}
close GOOD;

## Load the copy
while (<>)
{
  if (/>(\S+)/)
  {
    $seqname = $1;
    $copysequences{$seqname} = $_;
  }
  elsif (defined $seqname) 
  {
    $copysequences{$seqname} .= $_;
  }
  else
  {
    die "Malformed input in copy";
  } 
}

## Write the copied sequences in the same order as orig
foreach $seqname (@seqlist)
{
  if (defined $copysequences{$seqname})
  {
    print $copysequences{$seqname};
    $copysequences{$seqname} = undef;
  }
  else
  {
    ## Make sure each sequence in orig was in copy
    print STDERR "Sequence $seqname was in $goodfile but not in copy!\n";
  }
}

## Make sure each sequence in copy was in orig
foreach $seqname (keys %copysequences)
{
  if (defined $copysequences{$seqname})
  {
#    print STDERR "Printing $seqname from copy but not in $goodfile!\n";
#    print $copysequences{$seqname};
  }
}
