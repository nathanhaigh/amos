#!/usr/bin/perl -w
use strict;
use POSIX;
use FileHandle;

#++> $rid1 [$rlen1] -- $bestoo $bestcnt $bestmed $bestdist $beststd -- $rid2 [$rlen2] :
#++> scaffold89 [44907] -- RF 3 1582.00 1361.00 1118.00 -- C9969 [502] : RF 3 1582.00 1361.00 +/- 1118.00

# fastaFromBed -fi ctgs.fa -bed lib.1.bed -s -name -fo lib.1.fa
# fastaFromBed -fi ctgs.fa -bed lib.2.bed -s -name -fo lib.2.fa

my $readlen  = 100;
my $libbin   = 250;
my $baserank = 10;
my $librange = 200;
my $numreads = 20;

my $USAGE = "link2reads.pl prefix links ctg.fa\n";

my $prefix = shift @ARGV or die $USAGE;
my $linkf  = shift @ARGV or die $USAGE;
my $ctgf   = shift @ARGV or die $USAGE;

my %libs;

die "Can't read $ctgf\n"
  if (! -r $ctgf);

open LINKF, "< $linkf"
  or die "Can't open $linkf ($!)\n";

open CONFIG, "> $prefix.scaff.config"
  or die "Can't open $prefix.scaff.config ($!)\n";

while (<LINKF>)
{
  chomp;

  my @fields = split /\s+/, $_;

  my $ctg1 = $fields[1];
  my $len1 = $fields[2]; $len1 =~ s/^\[//; $len1 =~ s/\]$//;

  my $oo   = $fields[4]; my ($o1, $o2) = split //, $oo;
  my $cnt  = $fields[5];
  my $dst  = int($fields[6]);

  my $ctg2 = $fields[10];
  my $len2 = $fields[11]; $len2 =~ s/^\[//; $len2 =~ s/\]$//;

  my $lib  = $dst+$readlen+$readlen+$librange;
  my $rlib = $libbin*ceil(($lib) / $libbin);
  my $margin = int(($rlib-$lib)/2);

  ##       |- rl -|- lr -|-m-|                   |-m-|- rl -|
  ##       |-------------------- $rlib ---------------------|
  ##                         |------- dst -------|
  ##   ---------------------->                   ------------------------->
  ##       |----->                                   <------|

  #print "$ctg1 $len1 - $o1 $o2 $dst $rlib $lib $margin - $ctg2 $len2\n";

  if (!exists $libs{$rlib})
  {
     $libs{$rlib}->{1} = FileHandle->new("> $prefix.lib$rlib.1.bed");
     $libs{$rlib}->{2} = FileHandle->new("> $prefix.lib$rlib.2.bed");

     my $rank = $baserank + $rlib / $libbin;

     my $q1 = File::Spec->rel2abs("$prefix.lib$rlib.1.fq");
     my $q2 = File::Spec->rel2abs("$prefix.lib$rlib.2.fq");

     print CONFIG "\n";
     print CONFIG "[LIB]\n";
     print CONFIG "avg_ins=$rlib\n";
     print CONFIG "asm_flag=2\n";
     print CONFIG "reverse_seq=0\n";
     print CONFIG "rank=$rank\n";
     print CONFIG "q1=$q1\n";
     print CONFIG "q2=$q2\n";
  }

  my $fh1 = $libs{$rlib}->{1};
  my $fh2 = $libs{$rlib}->{2};

  print $fh1 "#$ctg1 $len1 - $o1 $o2 $dst $rlib $lib $margin - $ctg2 $len2\n";
  print $fh2 "#$ctg1 $len1 - $o1 $o2 $dst $rlib $lib $margin - $ctg2 $len2\n";

  for (my $ridx = 0; $ridx < $numreads; $ridx++)
  {
    my $shift1 = int($librange - $ridx*$librange/($numreads-1));
    my $shift2 = $librange - $shift1;
    #print $fh1 "#s1: $shift1 $shift2\n";
    #print $fh2 "#s2: $shift1 $shift2\n";

    if ($o1 eq "F")
    {
      my $s = $len1 - $readlen - $margin - $shift1; 
      my $e = $len1 - $margin - $shift1;

      print $fh1 "$ctg1\t$s\t$e\t$ctg1:$ctg2:$oo:$ridx/1\t$lib\t+\n";
    }
    else
    {
      my $s = 0 + $margin + $shift1;
      my $e = $readlen + $margin + $shift1;

      print $fh1 "$ctg1\t$s\t$e\t$ctg1:$ctg2:$oo:$ridx/1\t$lib\t-\n";
    }

    if ($o2 eq "F")
    {
      my $s = 0 + $margin + $shift2;
      my $e = $readlen + $margin + $shift2;

      print $fh2 "$ctg2\t$s\t$e\t$ctg1:$ctg2:$oo:$ridx/2\t$lib\t+\n";
    }
    else
    {
      my $s = $len2 - $readlen - $margin - $shift2;
      my $e = $len2 - $margin - $shift2;

      print $fh2 "$ctg2\t$s\t$e\t$ctg1:$ctg2:$oo:$ridx/2\t$lib\t-\n";
    }
  }
}

foreach my $rlib (sort {$a <=> $b} keys %libs)
{
# fastaFromBed -fi ctgs.fa -bed lib.1.bed -s -name -fo lib.1.fa
  print STDERR "Converting $rlib\n";

  $libs{$rlib}->{1}->close;
  $libs{$rlib}->{2}->close;

  my $cmd1 = "fastaFromBed -fi $ctgf -bed $prefix.lib$rlib.1.bed -s -name -fo $prefix.lib$rlib.1.fa";
  system($cmd1) and die "ERROR: $cmd1 ($!)\n";

  $cmd1 = "fasta_to_fastq.pl $prefix.lib$rlib.1.fa > $prefix.lib$rlib.1.fq";
  system($cmd1) and die "ERROR: $cmd1 ($!)\n";
  
  my $cmd2 = "fastaFromBed -fi $ctgf -bed $prefix.lib$rlib.2.bed -s -name -fo $prefix.lib$rlib.2.fa";
  system($cmd2) and die "ERROR: $cmd2 ($!)\n";

  $cmd2 = "fasta_to_fastq.pl $prefix.lib$rlib.2.fa > $prefix.lib$rlib.2.fq";
  system($cmd2) and die "ERROR: $cmd2 ($!)\n";
}
