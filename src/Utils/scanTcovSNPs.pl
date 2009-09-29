#!/usr/bin/perl -w
use strict;

my $strictsnps = 0;
my $weaksnps = 0;
my $scannedbases = 0;
my $numcontigs = 0;

## Display Parameters
my $CNS_FLANK = 50;
my $TILING_FLANK = 10;
my $BUFFERSIZE = 6*$CNS_FLANK;
my $PRINT_VERBOSE = 1;

## SNP Parameters

my $TYPE_STRICT = 'S';
my $TYPE_WEAK   = 'W';

my $MIN_ALLELE_DEPTH = 2;
my $MIN_CQV = 45;
my $MIN_ILLUMINA = 1;

my $STRICT_MAX_COVERAGE = 30;
my $STRICT_MIN_ALLELE_DEPTH = 3;
my $STRICT_MIN_CQV = 60;

my $MIN_DEPTH = $MIN_ALLELE_DEPTH*2;

my @buffer;

sub computeHomoLen
{
  my $line = shift;
  my $b = shift;

  return 0 if $b eq '-';

  my $retval = 1;

  for (my $pos = $line - 1; $pos >= 0; $pos--)
  {
    if ($buffer[$pos]->{basestr} !~ /$b/)
    {
      last;
    }

    $retval++;
  }

  my $blen = scalar @buffer;

  for (my $pos = $line + 1; $pos < $blen; $pos++)
  {
    if ($buffer[$pos]->{basestr} !~ /$b/)
    {
      last;
    }

    $retval++;
  }

  return $retval;
}

sub checkSNP
{
  my $line = shift;

  my $info = $buffer[$line];

  ## Short circuit 0 coverage or low coverage 
  return if (!defined $info->{basestr}); 
  return if (length($info->{basestr})) < $MIN_DEPTH;

  my @bases = split //,  $info->{basestr};
  my @quals = split /:/, $info->{qualstr};
  my @reads = split //,  $info->{readstr};

  my $depth = scalar @bases;

  my %slice;

  for (my $i = 0; $i < $depth; $i++)
  {
    my $b = $bases[$i];

    if ($b ne 'N')
    {
      $slice{$b}->{cnt}++;
      $slice{$b}->{cqv} += $quals[$i];
      push @{$slice{$b}->{idx}}, $i;
    }
  }

  ## Short circuit homogeneous slices 
  return if scalar keys %slice < 2; 

  my @bases_qv = sort {$slice{$b}->{cqv} <=> $slice{$a}->{cqv}} keys %slice;

  my @snpbases;
  my @strictsnpbases;

  foreach my $base (@bases_qv)
  {
    if ($slice{$base}->{cnt} >= $MIN_ALLELE_DEPTH)
    {
      my @sqv = sort {$b <=> $a}
                map {$quals[$_]}
                @{$slice{$base}->{idx}};

      $slice{$base}->{$TYPE_WEAK}->{cqv} = $sqv[0]+$sqv[1];

      $slice{$base}->{homo} = computeHomoLen($line, $base);

      my $illumina = 0;
      foreach my $i (@{$slice{$base}->{idx}})
      {
        $illumina++ if ($reads[$i] =~ /H/);
      }

      $slice{$base}->{illumina} = $illumina;

      if (($depth <= $STRICT_MAX_COVERAGE) &&
          ($slice{$base}->{cnt} >= $STRICT_MIN_ALLELE_DEPTH) &&
          (($sqv[0]+$sqv[1]+$sqv[2]) >= $STRICT_MIN_CQV))
      {
        $slice{$base}->{$TYPE_STRICT}->{cqv} = $sqv[0]+$sqv[1]+$sqv[2];
        push @strictsnpbases, $base;
        push @snpbases, $base;
      }
      elsif (($slice{$base}->{cnt} >= $MIN_ALLELE_DEPTH) &&
             (($sqv[0]+$sqv[1]) >= $MIN_CQV))
      {
        push @snpbases, $base;
      }
    }
  }

  my $snptype;

  if (scalar @strictsnpbases >= 2)
  {
    my $isolated = 0;
    my $confirmed = 0;
    foreach my $base (@strictsnpbases)
    {
      if ($slice{$base}->{homo} == 1)
      {
        $isolated++;
        last;
      }
      elsif($slice{$base}->{illumina} >= $MIN_ILLUMINA)
      {
        $confirmed++;
      }
    }

    if ($isolated || $confirmed >= 2)
    {
      $snptype = $TYPE_STRICT;
      @snpbases = @strictsnpbases;
    }
  }

  if (!defined $snptype && (scalar @snpbases >= 2))
  {
    $snptype = $TYPE_WEAK;

    if ((scalar @snpbases == 2) &&
        (($snpbases[0] eq '-') || ($snpbases[1] eq '-')))
    {
      ## Pure Indel
      my $confirmed = 0;
      foreach my $base (@snpbases)
      {
        if($slice{$base}->{illumina} >= $MIN_ILLUMINA)
        {
          $confirmed++;
        }
      }

      if ($confirmed != 2)
      {
        $snptype = undef;
      }
    }
  }

  if (defined $snptype)
  {
    if ($snptype eq $TYPE_STRICT) { $strictsnps++; }
    else                          { $weaksnps++; }
    my $blen = scalar @buffer;

    my $lflank = "";

    for(my $pos = $line-1;
        ($pos >= 0) && (length $lflank < $CNS_FLANK);
        $pos--)
    {
      my $c = $buffer[$pos]->{cns};
      if ($c ne "-")
      {
        $lflank .= $c;
      }
    }

    $lflank = reverse $lflank;

    my $rflank = "";

    for(my $pos = $line+1;
        ($pos < $blen) && (length $rflank < $CNS_FLANK);
        $pos++)
    {
      my $c = $buffer[$pos]->{cns};

      if ($c ne "-")
      {
        $rflank .= $c;
      }
    }

    print "$info->{ctg}\t$info->{gpos}\t$info->{upos}\t$info->{cns}\t$snptype\t$depth\t",
          join("/", @snpbases), "\t",
          join("/", map {$slice{$_}->{cnt}}  @snpbases), "\t",
          join("/", map {$slice{$_}->{cqv}}  @snpbases), "\t",
          join("/", map {$slice{$_}->{$snptype}->{cqv}}  @snpbases), "\t",
          join("/", map {$slice{$_}->{homo}} @snpbases), "\t",
          join("/", map {$slice{$_}->{illumina}} @snpbases), "\t",
          $lflank, "[" , join("/", @snpbases), "]", $rflank, "\n";

    if ($PRINT_VERBOSE)
    {
      my $pos = $line - $TILING_FLANK;
      my $end = $line + $TILING_FLANK + 1;

      if ($pos < 0) { $pos = 0; }
      if ($end > scalar @buffer) { $end = scalar @buffer; }

      while ($pos < $end)
      {
        my $i = $buffer[$pos];
        print "= ",
              $i->{gpos}, "\t",
              $i->{upos}, "\t",
              $i->{cns},  "\t",
              $i->{basestr}, "\t",
              $i->{qualstr}, "\t", 
              $i->{readstr}, "\n";

        $pos++;
      }

      print "\n\n";
    }
  }
}




my $contigid;
my $line = 0;
my $prev = "";
while (<>)
{
  chomp;

  $scannedbases++;

  my @vals = split /\s+/, $_;

  my $info;
  $info->{ctg}   = $vals[0];
  $info->{gpos}  = $vals[1];
  $info->{upos}  = $vals[2];
  $info->{cns}   = $vals[3];
  $info->{cqual} = $vals[4];

  if (scalar @vals > 5)
  {
    $info->{basestr} = $vals[5];
    $info->{qualstr} = $vals[6];

    $info->{readstr} = join "", map {substr($_,0,1)} split /:/, $vals[7];
  }

  if (defined $contigid && ($info->{ctg} ne $contigid))
  {
    my $blen = scalar @buffer;

    while ($line < $blen)
    {
      checkSNP($line);
      $line++;
    }

    undef @buffer;
    $line = 0;
  }

  if (!defined $contigid || ($info->{ctg} ne $contigid))
  {
    if ($info->{gpos} != 0)
    {
      print STDERR "Didn't see beginning of contig $info->{ctg}\n$prev\n$_\n";
    }
    
    $contigid = $info->{ctg};
    #print STDERR "Scanning $contigid\n";
    $numcontigs++;
  }
  else
  {
    my $len = scalar @buffer;
    my $lastpos = $buffer[$len-1]->{gpos};

    if ($info->{gpos} != $lastpos+1)
    {
      print STDERR "File is not sorted\n$prev\n$_\n";
    }
  }

  $prev = $_;

  push @buffer, $info;
  my $blen = scalar @buffer;

  if (($blen - $line) > $BUFFERSIZE)
  {
    checkSNP($line);
    $line++;
  }

  if ($line > $BUFFERSIZE)
  {
    shift @buffer;
    $line--;
  }
}

my $blen = scalar @buffer;
while ($line < $blen)
{
  checkSNP($line);
  $line++;
}

print STDERR "reporter:counter:asm,scannedbases,$scannedbases\n";
print STDERR "reporter:counter:asm,strictsnps,$strictsnps\n";
print STDERR "reporter:counter:asm,weaksnps,$weaksnps\n";
print STDERR "reporter:counter:asm,numcontigs,$numcontigs\n";


