#!/usr/bin/perl -w
use strict;

my $seencco = 0;

while (<>)
{
  if (/^{CCO/)
  {
    $seencco = 1;

    my $msg;
    my $acc;

    my $inread = 0;

    while (1)
    {
      my $line = <>;

      if ($line =~ /\}/)
      {
        if ($inread) { $inread = 0; }
        else         { last; }
      }

      chomp $line;

      if (!defined $acc && $line =~ /^acc:\((.*)\,.*\)/)
      {
        $acc = $1;
      }
      elsif ($line =~ /^\{/)
      {
        $inread = 1;
      }

      if (defined $msg) { $msg .= "\t"; }
      $msg .= $line;
    }

    print "$acc\tCCO\t$msg\n";
  }
}

