#!/usr/bin/perl -w
use strict;

my $seenafg = 0;


while (<>)
{
  if (/^{AFG/)
  {
    $seenafg = 1;
    my $msg;
    my $acc;

    while (1)
    {
      my $line = <>;
      last if ($line =~ /\}/);

      chomp $line;

      if (!defined $acc && $line =~ /^acc:\((.*)\,.*\)/)
      {
        $acc = $1;
      }

      if (defined $msg) { $msg .= "\t"; }
      $msg .= $line;
    }

    print "$acc\tAFG\t$msg\n";
  }
  elsif ($seenafg)
  {
    exit 0;
  }
}
