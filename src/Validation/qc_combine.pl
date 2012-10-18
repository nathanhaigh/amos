#!/usr/bin/perl -w
use strict;

if (scalar @ARGV < 1)
{
  die "Usage: qc-combine file1.qc [file2.qc ... filen.qc]\n";
}
elsif ($ARGV[0] eq "-h")
{
my $HELP_INFO = q~
  USAGE: qc_combine  [options]  <qcfile1>  <qcfile2>  . . .  <qcfileN>

  DESCRIPTION:
    qc_combine merges multiple CA qc files into a single side-by-side
    comparison qc file. Useful for viewing multiple qc files from different
    assemblies of the same data. Any number of qc files will be accepted
    as arguments on the command line, and the resulting merge will be output
    to stdout.

    Values missing from files that are present in other files will have their
    value reported as '???'. Sections present in files other than the first are
    fatal errors.

  OPTIONS:
    -h  Get this help.
~;

  print $HELP_INFO;
  exit (0);
}

my %files;
my @tagsequence;
my %seentags;
my %seensections;

my $first = 1;
foreach my $filename (@ARGV)
{
  open FILE, "< $filename" or die "Can't open $filename ($!)\n";
  my $section = "";
  my $line = 0;
  my $lastline;

  while (<FILE>)
  {
    chomp;

    $_ =~ s/,avgContig,avgGap//;
    $_ =~ s/,EUID//;

    if (/^\[(.+)\]$/)
    {
      $section=$1;

      if ($first)
      {
        push @tagsequence, $_;
        $seensections{$section} = 1;
      }
      else
      {
        if (! exists $seensections{$section})
        {
          die "ERROR: Unexpected section: $section in $filename line $line\n";
        }
      }
    }
    elsif (/=/)
    {
      my @vals = split /=/, $_;
      my $tag = $vals[0];
      my $val = $vals[1];

      if ($val =~ /,/)
      {
        if ($tag =~ /total/)
        {

        }
        else
        {
          my @vals = split /,/, $val;
          pop @vals;
          if ($section =~ /Scaffold/)
          {
            pop @vals;
            pop @vals;
          }

          $val = join ",", @vals;
        }
      }

      $files{$filename}->{"$section.$tag"} = $val;

      #print "$filename.$section.$tag: $val\n";

      if ($first)
      {
        push @tagsequence, $tag;
        $seentags{"$section.$tag"} = $line;
      }
      else
      {
        if (! exists $seentags{"$section.$tag"})
        {
          print STDERR "WARNING: $section.$tag is new in $filename line $line\n";
          
          $lastline++;
          splice @tagsequence, $lastline, 0, ($tag);

          foreach my $k (keys %seentags)
          {
            $seentags{$k}++ if ($seentags{$k} >= $lastline);
          }

          $seentags{"$section.$tag"} = $lastline;
        }
        else
        {
          $lastline = $seentags{"$section.$tag"};
        }
      }
    }
    elsif (/^$/)
    {
      push @tagsequence, $_ if $first;
    }
    else
    {
      print STDERR "Don't understand line $line in $filename: \"$_\"\n";
    }

    $line++;
  }

  $first = 0;
}

printf "%25s", "";
foreach my $filename (@ARGV)
{
  printf " %15s", substr $filename, 0, 15;
}
print "\n";

my $section="";
my $list = 0;
foreach my $tag (@tagsequence)
{
  ##print "TAG: \"$tag\"\n";
  if ($tag =~ /^\[(.+)\]$/)
  {
    $section = $1;
    print $tag;

    ## If the section has a comma, assume list values follow
    $list = ($section =~ /,/);
  }
  elsif ($tag =~ /^$/)
  {

  }
  else
  {
    if ($list) { printf "%-15s", $tag; }
    else       { printf "%-25s", $tag; }

    foreach my $filename (@ARGV)
    {
      my $data = $files{$filename}->{"$section.$tag"};
      $data = "???" if (!defined $data);

      if ($list) { printf " %25s", $data; }
      else       { printf " %15s", $data; }
    }
  }

  print "\n";
}
