#!/usr/bin/perl -w
use strict;
use Getopt::Long;

## Dependencies
my $SFF_EXTRACT     = "sff_extract.py";      ## from http://bioinf.comav.upv.es/sff_extract/

## Options
my $help  = 0;
my $prefix = undef;

my $MIN_LEN = 30;
my $MAX_LEN = 60;

my $USAGE = "USAGE: sff2fq [OPTION...] SFF_FILE\n"; 

my $LINKER_FILE = "linkerall.fa";

my $OVERWRITE = 0;

my $res = GetOptions("help"          => \$help,
                     "prefix=s"      => \$prefix,
                     "min-len=s"     => \$MIN_LEN,
                     "max-len=s"     => \$MAX_LEN,
                     "linker-file=s" => \$LINKER_FILE,
                     "overwrite"     => \$OVERWRITE,
                     );
 
if ($help || !$res)
{
  print $USAGE;
  print "\n";
  print "Convert a SFF file containing paired-end 454 reads to a pair of FASTQ files.\n";
  print "\n";
  print "The 454 reads are split around the linker sequences from the specified\n";
  print "linker file, and the left and right reads are written to separate\n";
  print "FASTQ files.  The reads are re-named suffixed with /1 and /2, and one\n";
  print "of the reads in each pair is reverse complemented.  Reads that do not\n";
  print "contain a linker sequence are discarded.  Reads are trimmed to the length\n";
  print "specified by --max-len, and by default read pairs containing a read with\n";
  print "length below the --min-len will be discarded (this will happen when the\n";
  print "linker sequence is very close to the end of the original read.)\n";
  print "\n";
  print "Required\n";
  print "  SFF_FILE        : input 454 paired-end reads\n";
  print "\n";
  print "Options\n";
  print "  --prefix <name> : prefix for output fq files (default: prefix of .sff file)\n";
  print "  --min-len <len>     : minimum length reads to keep (default: $MIN_LEN)\n";
  print "  --max-len <len>     : length to trim reads to (default: $MAX_LEN)\n";
  print "  --linker-file <file> : file containing 454 linker sequences\n";
  print "                         (default: $LINKER_FILE)\n";
  print "  --overwrite     : overwrite existing files\n";
  print "  --help          :  show this help\n";
  exit 0;
}

die $USAGE if (scalar @ARGV != 1);

my $sfffile = $ARGV[0];

if (!defined $prefix)
{
  $prefix = $sfffile;
  $prefix =~ s/\.sff$//;
}

# AWK filter to get the first reads in the read pairs from the FASTQ file
# produced by sff_extract.py:
#
#   - keep only reads with ".f" in the tag, but replace with "/1"
#   - trim sequences to $MAX_LEN
#   - keep all original quality scores (no intermediate conversion to FASTA...)
my $awk_f_filter = <<EOS;
{
    if (NR % 4 == 1) {
        # FASTQ tag line.
        if (/\\.f/) {
            sub(/\\.f/, "/1");
            do_print = 1;
            print;
        } else {
            do_print = 0;
        }
    } else {
        if (do_print) {
            if (NR % 4 == 3) {
                print;
            } else {
                # FASTQ sequence or quality line.  Trim to MAX_LEN bases.
                print substr(\$0, 1, $MAX_LEN);
            }
        }
    }
}
EOS

# Tweak the AWK program to get the second reads from the pairs.
my $awk_r_filter = $awk_f_filter;
$awk_r_filter =~ s/\\.f/\\.r/g;
$awk_r_filter =~ s/"\/1"/"\/2"/g;

runCmd("sff_extract", "$prefix.raw.fastq",
       "$SFF_EXTRACT --linker_file=\"$LINKER_FILE\" --clip --compat "
       . "\"$sfffile\" -o \"$prefix.raw\"");

runCmd("convert fq1", "$prefix.raw1.fq",
       "awk '$awk_f_filter' \"$prefix.raw.fastq\" > \"$prefix.raw1.fq\"");

runCmd("convert fq2", "$prefix.raw2.fq",
       "awk '$awk_r_filter' \"$prefix.raw.fastq\" > \"$prefix.raw2.fq\"");

print "\n";
print "Generating \"$prefix.1.fq\" and \"$prefix.2.fq\"\n";
print "  from \"$prefix.raw1.fq\" and \"$prefix.raw2.fq\".\n";
print "Read pairs containing a read less than $MIN_LEN in length will be discarded.\n";

# Go through the FASTQ files we made and filter out read pairs that have one or
# both reads less than $MIN_LEN bp long.
open RAW1, "<", "$prefix.raw1.fq" or die "ERROR: Cannot open \"$prefix.raw1.fq\": $!\n";
open RAW2, "<", "$prefix.raw2.fq" or die "ERROR: Cannot open \"$prefix.raw2.fq\": $!\n";
open OUT1, ">", "$prefix.1.fq" or die "ERROR: Cannot open \"$prefix.1.fq\": $!\n";
open OUT2, ">", "$prefix.2.fq" or die "ERROR: Cannot open \"$prefix.2.fq\": $!\n";

my $num_discarded_pairs = 0;
my $num_kept_pairs = 0;

while (1) {
  my $fq_tag_1  = <RAW1>;
  my $fq_seq_1  = <RAW1>;
  my $fq_sep_1  = <RAW1>;
  my $fq_qual_1 = <RAW1>;

  my $fq_tag_2  = <RAW2>;
  my $fq_seq_2  = <RAW2>;
  my $fq_sep_2  = <RAW2>;
  my $fq_qual_2 = <RAW2>;

  if (!defined $fq_tag_1 || !defined $fq_tag_2) {
    if (defined $fq_tag_1 || defined $fq_tag_2) {
      die "ERROR: Unexpected EOF\n";
    }
    last;
  }

  chomp $fq_tag_1;
  chomp $fq_tag_2;

  if (substr($fq_tag_1, 0, -2) ne substr($fq_tag_2, 0, -2)) {
    die "ERROR: Reads do not match up correctly\n";
  }
  if (substr($fq_tag_1, -2) ne "/1" || substr($fq_tag_2, -2) ne "/2") {
    die "ERROR: Reads not named correctly ($fq_tag_1, $fq_tag_2)\n";
  }
  chomp $fq_seq_1;
  chomp $fq_seq_2;
  if (length $fq_seq_1 >= $MIN_LEN && length $fq_seq_2 >= $MIN_LEN) {
    print OUT1 "$fq_tag_1\n$fq_seq_1\n$fq_sep_1$fq_qual_1"
        or die "ERROR: Error writing to \"$prefix.1.fq\": $!\n";
    print OUT2 "$fq_tag_2\n$fq_seq_2\n$fq_sep_2$fq_qual_2"
        or die "ERROR: Error writing to \"$prefix.2.fq\": $!\n";
    $num_kept_pairs++;
  } else {
    $num_discarded_pairs++;
  }
}
close OUT1 or die "ERROR: Cannot close \"$prefix.1.fq\": $!\n";
close OUT2 or die "ERROR: Cannot close \"$prefix.2.fq\": $!\n";

my $num_pairs = $num_kept_pairs + $num_discarded_pairs;
print "Discarded $num_discarded_pairs of $num_pairs pairs "
    . "due to read length less than $MIN_LEN bp.\n";
print "There are $num_kept_pairs pairs remaining.\n";
print "Done generating \"$prefix.1.fq\" and \"$prefix.2.fq\".\n";


## runCmd
###############################################################################

sub runCmd
{
  my $desc = $_[0];
  my $outf = $_[1];
  my $cmd  = $_[2];

  if (! -r $outf || $OVERWRITE)
  {
    print "[$desc] running command to generate file \"$outf\":\n";
    print "$cmd\n";
    print "\n";
    my $rc = system($cmd);
    die "ERROR: Command failed rc:$rc ($!)\n" if $rc;
  } else {
    print "Skipping command to generate \"$outf\"\n";
  }
}
