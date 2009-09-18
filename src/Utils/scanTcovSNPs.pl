#!/usr/bin/perl -w
use strict;

## Display Parameters
my $CNS_FLANK = 50;
my $TILING_FLANK = 10;
my $BUFFERSIZE = 6*$CNS_FLANK;

## SNP Parameters
my $MIN_DEPTH = 2;
my $MIN_ALLELE_DEPTH = 2;

my $MIN_SNP_AVG_QV = 21;
my $MAX_SNP_HOMO = 2;

my $MIN_INDEL_SUM_QV = 60;
my $MAX_INDEL_HOMO = 1; 

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

  for (my $pos = $line + 1; $pos <= $blen; $pos++)
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

  return if (!defined $info->{basestr}); ## 0-coverage
  return if (length($info->{basestr})) < $MIN_DEPTH; ## low coverage

  my @bases = split //,  $info->{basestr};
  my @quals = split /:/, $info->{qualstr};
  my @reads = split /:/, $info->{readstr};

  my $depth = scalar @bases;

  my %slice;

  for (my $i = 0; $i < $depth; $i++)
  {
    my $b = $bases[$i];
    $slice{$b}->{cnt}++;
    $slice{$b}->{cqv} += $quals[$i];
    push @{$slice{$b}->{idx}}, $i;
  }

  return if scalar keys %slice < 2; ## homogenous slice

  my $cns = $info->{cns};

  my $issnp = 0;

  my @snpbases;
  push @snpbases, $cns;

  my $chomolen = computeHomoLen($line, $cns);

  my @snphomolen;
  push @snphomolen, $chomolen;

  my @bases_qv = sort {$slice{$b}->{cqv} <=> $slice{$a}->{cqv}} keys %slice;
 
  foreach my $base (@bases_qv)
  {
    if (($base ne $cns) && ($slice{$base}->{cnt} >= $MIN_ALLELE_DEPTH))
    {
      my $bhomolen = computeHomoLen($line, $base); 

      if (($base eq '-') || ($cns eq '-'))
      {
        if (($bhomolen <= $MAX_INDEL_HOMO) &&
            ($chomolen <= $MAX_INDEL_HOMO))
        {
          my @sqv = sort {$b <=> $a}
                    map {$quals[$_]}
                    @{$slice{$base}->{idx}};

          if (($sqv[0] + $sqv[1]) >= $MIN_INDEL_SUM_QV)
          {
            $issnp = 1;
            push @snpbases, $base;
            push @snphomolen, $bhomolen;
          }
        }
      }
      else
      {
        if (($bhomolen <= $MAX_SNP_HOMO) &&
            ($chomolen <= $MAX_SNP_HOMO))
        {
          if (($slice{$base}->{cqv} / $slice{$base}->{cnt}) >= $MIN_SNP_AVG_QV)
          {
            $issnp = 1;
            push @snpbases, $base;
            push @snphomolen, $bhomolen;
          }
        }
      }
    }
  }

  if ($issnp)
  {
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

    my $snpstr .= "[" . join("/", @snpbases) . "]";

    print ">$info->{ctg}\t$info->{gpos}\t$info->{upos}\t$cns\t$depth\t",
          join("/", @snpbases), "\t",
          join("/", map {$slice{$_}->{cnt}} @snpbases), "\t",
          join("/", map {$slice{$_}->{cqv}} @snpbases), "\t",
          join("/", @snphomolen), "\t",
          $lflank, $snpstr, $rflank, "\n";

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




my $contigid;
my $line = 0;
while (<>)
{
  chomp;

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
    $info->{readstr} = $vals[7];
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

  $contigid = $info->{ctg};

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

