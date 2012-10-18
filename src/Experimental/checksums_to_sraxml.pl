#!/usr/bin/perl -w
use strict;

my $CENTER_NAME = "CSHL";
my $EXPERIMENT  = "SRX151684";
my $FILETYPE    = "PacBio_HDF5";

my $year;
my $month;
my $day;
my $hour;
my $min;
my $sec;

print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
print "<RUN_SET xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://www.ncbi.nlm.nih.gov/viewvc/v1/trunk/sra/doc/SRA_1-4/SRA.run.xsd?view=co\">\n";

while (<>)
{
  chomp;
  my ($checksum, $filename) = split /\s+/, $_;
 
  my ($basename) = split /\./, $filename;
  my ($date, $time) = split /_/, $basename;

  #print STDERR "$filename $basename\n";
 
  if ($date =~ /m(\d\d)(\d\d)(\d\d)/)
  {
    $year  = $1;
    $month = $2;
    $day   = $3;
  }
  else
  { 
    die "Can't parse date: $date $filename\n";
  }

  if ($time =~ /(\d\d)(\d\d)(\d\d)/)
  {
    $hour = $1;
    $min  = $2;
    $sec  = $3;
  }
  else
  {
    die "Cant parse time: $time $filename\n";
  }
 
 
  print "  <RUN alias=\"$basename\" center_name=\"$CENTER_NAME\" run_center=\"$CENTER_NAME\" run_date=\"20$year-$month-${day}T$hour:$min:$sec\">\n";
  print "    <EXPERIMENT_REF refname=\"SRX151684\"/>\n";
  print "    <DATA_BLOCK>\n";
  print "      <FILES>\n";
  print "        <FILE filename=\"$filename\" filetype=\"$FILETYPE\" checksum_method=\"MD5\" checksum=\"$checksum\" />\n";
  print "      </FILES>\n";
  print "    </DATA_BLOCK>\n";
  print "  </RUN>\n";
}

print "</RUN_SET>\n";
