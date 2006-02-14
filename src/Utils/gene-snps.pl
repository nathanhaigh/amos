#!/usr/bin/perl -w
use strict;

my $USAGE = "gene-snps.pl gene.tab snps gene.fasta tcovStats\n";


my $tabfile = shift @ARGV or die $USAGE;
my $snpsfile = shift @ARGV or die $USAGE;
my $fastafile = shift @ARGV or die $USAGE;
my $statsfile = shift @ARGV or die $USAGE;

open TABFILE, "< $tabfile" or die "Can't open $tabfile ($!)\n";
open SNPS,    "< $snpsfile" or die "can't open $snpsfile ($!)\n";
open FASTA,   "< $fastafile" or die "Can't open $fastafile ($!)\n";
open STATS,   "< $statsfile" or die "Can't open $statsfile ($!)\n";

my $consensus;
my @genes;
my %genegroups;
my $reflen = 0;
my %genestats;

my %codon =
(
  "TTT" => "F", "TTC" => "F", "TTA" => "L", "TTG" => "L",
  "TCT" => "S", "TCC" => "S", "TCA" => "S", "TCG" => "S",
  "TAT" => "Y", "TAC" => "Y", "TAA" => "*", "TAG" => "*",
  "TGT" => "C", "TGC" => "C", "TGA" => "*", "TGG" => "W",
  "CTT" => "L", "CTC" => "L", "CTA" => "L", "CTG" => "L",
  "CCT" => "P", "CCC" => "P", "CCA" => "P", "CCG" => "P",
  "CAT" => "H", "CAC" => "H", "CAA" => "Q", "CAG" => "Q",
  "CGT" => "R", "CGC" => "R", "CGA" => "R", "CGG" => "R",
  "ATT" => "I", "ATC" => "I", "ATA" => "I", "ATG" => "M",
  "ACT" => "T", "ACC" => "T", "ACA" => "T", "ACG" => "T",
  "AAT" => "N", "AAC" => "N", "AAA" => "K", "AAG" => "K",
  "AGT" => "S", "AGC" => "S", "AGA" => "R", "AGG" => "R",
  "GTT" => "V", "GTC" => "V", "GTA" => "V", "GTG" => "V",
  "GCT" => "A", "GCC" => "A", "GCA" => "A", "GCG" => "A",
  "GAT" => "D", "GAC" => "D", "GAA" => "E", "GAG" => "E",
  "GGT" => "G", "GGC" => "G", "GGA" => "G", "GGG" => "G",
);

my %rc = 
(
  "." => ".",
  "A" => "T", "T" => "A",
  "C" => "G", "G" => "C",
);

my %ambiguity=
(
  "A" => ["A"],
  "C" => ["C"],
  "G" => ["G"],
  "T" => ["T"],

  "M" => ["A", "C"],
  "R" => ["A", "G"],
  "W" => ["A", "T"],
  "S" => ["C", "G"],
  "Y" => ["C", "T"],
  "K" => ["G", "T"],

  "V" => ["A", "C", "G"],
  "H" => ["A", "C", "T"],
  "D" => ["A", "G", "T"],
  "B" => ["C", "G", "T"],

  "N" => ["A", "C", "G", "T"],
);



sub translate
{
  my $seq = shift;
  my $trans;

  my $len = length($seq);

  my @seq = split //, $seq;

  my $s = "";
  my $c = 0;
  for (my $i=0; $i < $len; $i++)
  {
    $s .= $seq[$i];
    ++$c;

    if ($c == 3)
    {
      $trans .= $codon{uc($s)};
      $s = "";
      $c = 0;
    }
  }
  
  if ($c != 0)
  {
    die "ERROR: c = $c, s = $s\n";
  }

  return $trans;
}

sub reverseCompliment
{
  my $seq = shift;
  $seq = reverse $seq;
  my $len = length $seq;

  my @seq = split //, $seq;

  for (my $i = 0; $i < $len; $i++)
  {
    $seq[$i] = $rc{uc($seq[$i])};
  }

  return join("", @seq);
}

sub printGene
{
  my $genegroup = shift;
  
  my $seq;
  foreach my $gene (sort {$a->{start} <=> $b->{start}} @$genegroup)
  {
    print "GENE: $gene->{label} $gene->{start} $gene->{end} $gene->{rc}\n";

    my $s = $gene->{start};
    my $e = $gene->{end};

    $seq .= substr($consensus, $s-1, $e-$s+1);
  }

  $seq = reverseCompliment($seq) if $genegroup->[0]->{rc};
  my $trans = translate($seq);

  print ">dna\n";
  print "$seq\n";

  print ">protein\n";
  print "$trans\n";

  print "\n\n";
}

sub printSNPDistance
{
  my $label = shift;
  my $len = shift;
  my $count = shift;

  my $density = sprintf("%.02f", ($count *1000) / $len);

  my $distance = ($count) ? sprintf("%.02f", $len / $count+1) : "*";
  print "$label density: $density snp/Kbp, distance: $distance bp/snp ($len bp $count snps)";
}


my %qdepth;
my $totalbases = 0;
my $totalpos = 0;
## Load the dcov
while (<STATS>)
{
  my @vals = split /\s+/, $_;
  my $cov = $vals[3];

  $qdepth{$vals[0]} = $cov;
  $totalbases += $cov;
  $totalpos++;
}

print "Loaded query contig coverage information totalpos: $totalpos\n";


## Load the consensus
while (<FASTA>)
{
  next if />/;
  chomp;
  $consensus .= $_;
}

print "Loaded reference consensus len=",length($consensus),"\n";




## Load the gene coordinates
while (<TABFILE>)
{
  next if /^#/;
  next if /^!/;

  if (/^%(\d+)/)
  {
    $reflen = $1;
    next;
  }

  chomp;

  my @val = split /\t+/, $_;

  my $strand = $val[0];
  my $start  = $val[2];
  my $end    = $val[3];
  my $label  = $val[5];

  my $exon;
  $exon->{start}    = $start;
  $exon->{end}      = $end;
  $exon->{len}      = $end - $start + 1;
  $exon->{label}    = $label;
  $exon->{rc}       = ($strand eq "reverse") ? 1 : 0;
  $exon->{snpcount} = 0;
  $exon->{synonymous} = 0;
  $exon->{nonsynonymous} = 0;
  $exon->{deletion} = 0;
  $exon->{insertion} = 0;
  $exon->{codonposition}->{1} = 0;
  $exon->{codonposition}->{2} = 0;
  $exon->{codonposition}->{3} = 0;

  $genestats{$label}->{label} = $label;
  $genestats{$label}->{len} += $end - $start + 1;
  $genestats{$label}->{snpcount} = 0;
  $genestats{$label}->{synonymous} = 0;
  $genestats{$label}->{nonsynonymous} = 0;
  $genestats{$label}->{deletion} = 0;
  $genestats{$label}->{insertion} = 0;
  $genestats{$label}->{codonposition}->{1} = 0;
  $genestats{$label}->{codonposition}->{2} = 0;
  $genestats{$label}->{codonposition}->{3} = 0;

  push @{$genegroups{$label}}, $exon;

  push @genes, $exon;
}

print "Loaded gene data for ", scalar keys %genegroups,
      " genes (", scalar @genes, " exons)\n";

foreach my $l (sort {$genegroups{$a}->[0]->{start} <=> $genegroups{$b}->[0]->{start}} keys %genegroups)
{
  #printGene($genegroups{$l});
}

## Load the snps

my %qsnppos;

my $snpcount = 0;
my $lastsnp = -1000;
my $snpcov = 0;
my $ambsnpcount = 0;

while (<SNPS>)
{
  ++$snpcount;

  my @vals = split /\s+/, $_;
  my $rpos = $vals[10];
  my $qpos = $vals[11];
  my $type = $vals[14];
  my $rseq = $vals[15];
  my $qseq = $vals[16];
  my $len  = length($rseq);

  my $f = "";

  if ($rpos - $lastsnp < 3) { $f = '*'; }
  $lastsnp = $rpos;

  my $cov = $qdepth{$qpos};
  $snpcov += $cov;

  $qsnppos{$qpos} = 1;
  
  print "$rpos$f\t$len\t$type\t$rseq\t$qseq\t$cov";

  foreach my $g (@genes)
  {
    if (($g->{start} <= $rpos) &&
        ($g->{end}   >= $rpos))
    {
      my $s = $g->{start};
      my $e = $g->{end};
      my $rc = $g->{rc};
      my $l = $g->{label};
      my $rcc = $rc ? "1" : "0";

      $g->{snpcount}++;
      $genestats{$l}->{snpcount}++;

      print "\t|\t$g->{label} ($s, $e) $rcc";

      if ($rseq =~ /\./)
      {
        $g->{insertion}++;
        $genestats{$l}->{insertion}++;
        last;
      }

      if ($qseq =~ /\./)
      {
        $g->{deletion}++;
        $genestats{$l}->{deletion}++;
        last;
      }

      last if ($len > 1);

      my @ambiguities = @{$ambiguity{uc($qseq)}};

      my $codonposition = 0;
      my $origdna = "";

      if ($rc)
      {
        my $exonpos = 0;
        my $last = 0;

        my $geneseq;

        foreach my $exon (sort {$b->{start} <=> $a->{start}} @{$genegroups{$g->{label}}})
        {
          my $raw = substr($consensus, $exon->{start}-1, $exon->{len});
          $geneseq .= reverseCompliment($raw);

          if ($exon->{start} eq $g->{start})
          {
            $last = 1;
          }

          if (!$last)
          {
            $exonpos += $exon->{len};
          }
        }

        die "WTF" if !$last;

        my $geneseqoffset = $e - $rpos + $exonpos;
        $codonposition = $geneseqoffset % 3;

        #my $conspos = $rpos-(2-$codonposition)-1;
        #$origdna = uc(substr($consensus, $conspos, 3));

        $geneseqoffset -= $codonposition;

        $origdna = uc(substr($geneseq, $geneseqoffset, 3));

        my $rseqrc = $rc{uc($rseq)};

        if (substr($origdna, $codonposition, 1) ne uc($rseqrc))
        {
          die " ERROR: orig[$codonposition]:$origdna != $rseqrc";
        }

        substr($origdna, $codonposition, 1) = lc(substr($origdna, $codonposition, 1));
      }
      else
      {
        my $exonpos = 0;
        my $last = 0;
        my $geneseq;
        foreach my $exon (sort {$a->{start} <=> $b->{start}} @{$genegroups{$g->{label}}})
        {
          $geneseq .= substr($consensus, $exon->{start}-1, $exon->{len});

          if ($exon->{start} eq $g->{start})
          {
            $last = 1;
          }

          if (!$last)
          {
            $exonpos += $exon->{len};
            print " $exonpos";
          }
        }

        die "WTF" if !$last;

        my $geneseqoffset = $rpos - $s + $exonpos; 
        $codonposition = $geneseqoffset % 3;

 #       my $conspos = $rpos-$codonposition-1;
        $geneseqoffset -= $codonposition;

        $origdna = uc(substr($geneseq, $geneseqoffset, 3));

        if (substr($origdna, $codonposition, 1) ne uc($rseq))
        {
          die " ERROR: orig[$codonposition]:$origdna != $rseq";
        }

        substr($origdna, $codonposition, 1) = lc(substr($origdna, $codonposition, 1));
      }

      my $cpos = $codonposition+1;
      print "\t| $cpos";

      $g->{codonposition}->{$cpos}++;
      $genestats{$l}->{codonposition}->{$cpos}++;

      foreach $qseq (@ambiguities)
      {
        if (uc($qseq) eq uc($rseq))
        {
          # print "| skip $qseq -> $rseq";
          next;
        }
        $ambsnpcount++;

        my $newdna = "";

        if ($rc)
        {
          $newdna = $origdna;
          substr($newdna, $codonposition, 1) = lc($rc{uc($qseq)});
        }
        else
        {
          $newdna = $origdna;
          substr($newdna, $codonposition, 1) = $qseq;
        }

        my $origaa = $codon{uc($origdna)};
        my $newaa = $codon{uc($newdna)};

        my $syn = ($origaa eq $newaa) ? 1 : 0;
        print " | $origaa ($origdna) -> $newaa ($newdna) $syn";

        $g->{synonymous}     += $syn;
        $g->{nonsynonymous} += 1-$syn;
        $genestats{$l}->{synonymous} += $syn;
        $genestats{$l}->{nonsynonymous} += 1-$syn;
      }

      if (scalar @ambiguities > 1)
      {
        print " | @@@";
      }

      last;
    }
  }

  print "\n";
}

print "\n\n";

print "gene stats:\n";

foreach my $genename (sort {$a cmp $b} keys %genestats)
{
  my $g = $genestats{$genename};
  my $gl = $g->{len};
  my $sc = $g->{snpcount};

  printSNPDistance("\"$g->{label}\"", $gl, $sc);

  print " |";

  for (my $i = 1; $i <= 3; $i++)
  {
    my $c = $g->{codonposition}->{$i};
    print " $c";
  }

  my $syn = $g->{synonymous};
  my $non = $g->{nonsynonymous};
  my $del = $g->{deletion};
  my $ins = $g->{insertion};
  print " S:$syn N:$non D:$del I:$ins\n";

  if ($sc != ($syn + $non + $del + $ins))
  {
    die "ERROR in $g->{label}: sc($sc) != syn ($syn) + non ($non) + del ($del) + ins ($ins)\n";
  }
}

print "\n\nexon stats:\n";

my %codonposition = (1=>0,2=>0,3=>0);
my $genelen = 0;
my $genesnps = 0;

my $synonymous = 0;
my $nonsynonymous = 0;
my $insertions = 0;
my $deletions = 0;
foreach my $g (sort {$a->{label} cmp $b->{label}} @genes)
{
  my $gl = $g->{len};
  my $sc = $g->{snpcount};

  $genelen  += $gl;
  $genesnps += $sc;
 
  printSNPDistance("\"$g->{label}\" start:$g->{start} end:$g->{end}", $gl, $sc);

  print " |";

  for (my $i = 1; $i <= 3; $i++)
  {
    my $c = $g->{codonposition}->{$i};
    $codonposition{$i} += $c;

    print " $c";
  }

  my $syn = $g->{synonymous};
  my $non = $g->{nonsynonymous};
  my $del = $g->{deletion};
  my $ins = $g->{insertion};
  print " S:$syn N:$non D:$del I:$ins\n";

  $synonymous += $syn;
  $nonsynonymous += $non;
  $deletions += $del;
  $insertions += $ins;

  if ($sc != ($syn + $non + $del + $ins))
  {
    die "ERROR in $g->{label}: sc($sc) != syn ($syn) + non ($non) + del ($del) + ins ($ins)\n";
  }
}

print "\n";

printSNPDistance("Coding", $genelen, $genesnps); print "\n";

my $noncodinglen = $reflen - $genelen;
my $noncodingsnps = $snpcount - $genesnps;

printSNPDistance("Non-Coding", $noncodinglen, $noncodingsnps); print "\n";
printSNPDistance("Overall", $reflen, $snpcount); print "\n";

print "SNP codon Position: $codonposition{1} $codonposition{2} $codonposition{3} S:$synonymous N:$nonsynonymous\n";

if ($codonposition{1} + $codonposition{2} + $codonposition{3} != ($synonymous + $nonsynonymous))
{
  die "ERROR with codon totals!!!\n";
}

print "\n\n";

{
  my $adepth = sprintf ("%.02f", $totalbases/$totalpos);
  my $sdepth = sprintf ("%.02f", $snpcov/$snpcount);

  print "All  (bases, positions, avgdepth): $totalbases $totalpos $adepth\n";
  print "Snps (bases, positions, avgdepth): $snpcov $snpcount $sdepth\n";
}


print "Wstart Wend WSize Adepth SNPs\n";

my $windowsize = 500;
my $windowshift = $windowsize/2;
my $wstart = 1;

while ($wstart < $totalpos)
{
  my $wend = $wstart + $windowsize;
  my $snpcount = 0;
  my $totalbases = 0;
  my $windowpos = 0;
  my $i;

  for ($i = $wstart; ($i < $wend) && ($i < $totalpos); $i++)
  {
    $windowpos++;
    $totalbases += $qdepth{$i};

    if (exists $qsnppos{$i})
    {
      $snpcount++;
    }
  }

  my $avgdepth = ($windowpos) ? sprintf("%.02f", $totalbases/$windowpos) : "0.0";

  print "#$wstart $i $windowpos $avgdepth $snpcount\n";
  $wstart += $windowshift;
}


