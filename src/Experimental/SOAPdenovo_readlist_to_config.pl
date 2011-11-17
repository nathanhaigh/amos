#!/usr/bin/perl -w
use strict;
use Getopt::Long;

my $USAGE="SOAPdenovo_readlist_to_config.pl [options] readlist > prefix.config\n";

my $path;

my $help = 0;
my $rank = 1;

my $res = GetOptions("help"     => \$help,
                     "path=s"   => \$path,
                     "rank=s"   => \$rank
                     );

if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print " readlist: \"q\" or \"q1 q2 mea rc\"\n";
  print "\n";
  print "Options\n";
  print "-------\n";
  print " -path <path> : Prefix path to reads with <path>\n";
  print " -rank <rank> : Starting rank\n";
  exit 0;
}


if ($rank == 1)
{
  print "max_rd_len=200\n";
}


while (<>)
{
  next if /^$/;

  chomp;

  print "\n";
  print "[LIB]\n";

  my @vals = split /\s/, $_;

  if (scalar @vals == 1)
  {
    my $q = $vals[0];

    print "asm_flag=1\n";
    if (defined $path) { print "q=$path/$q\n"; }
    else               { print "q=$q\n"; }
  }
  else
  {
    my $q1  = $vals[0];
    my $q2  = $vals[1];
    my $mea = $vals[2];
    my $rc  = $vals[3];

    if (defined $rc && $rc)
    {
      print "reverse_seq=1\n";
    }

    print "rank=$rank\n";
    print "avg_ins=$mea\n";
    print "asm_flag=3\n";
    
    if (defined $path)
    {
      print "q1=$path/$q1\n";
      print "q2=$path/$q2\n";
    }
    else
    {
      print "q1=$q1\n";
      print "q2=$q2\n";
    }

    $rank++;
  }
}
