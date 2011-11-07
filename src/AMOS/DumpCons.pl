#!/usr/bin/env perl

use strict;
use warnings;
use AMOS;

my $bankpath = shift @ARGV;
my $type = shift @ARGV;
my $id = shift @ARGV;

die "Usage: AmosTest bankpath [-e|-i] id\n"
  if (!defined $id);

my $ctg = AMOS::Contig_t->new();
my $cbank = AMOS::Bank_t->new($ctg->getNCode());

print STDERR "Opening Bank $bankpath\n";
$cbank->open($bankpath);

print STDERR "Fetching contig $id\n";
if ($type =~ /e/)
{
  $cbank->fetch($id, $ctg);
}
else
{
  $cbank->fetch(int($id), $ctg);
}

my $cons = $ctg->getSeqString();
print "Cons: $cons\n";

print "Consensus Length: ", length($cons), "\n";





