#!/usr/bin/perl -w
use strict;

while (<>)
{
  if (/^{FRG/)
  {
    my $msg;
    my $acc;

    while (1)
    {
      my $line = <>;
      last if ($line =~ /\}/);

      chomp $line;

      if (!defined $acc && $line =~ /^acc:(.*)/)
      {
        $acc = $1;
      }

      if (defined $msg) { $msg .= "\t"; }
      $msg .= $line;
    }

    print "$acc\tFRG\t$msg\n";
  }
}
