#!/usr/bin/perl -w
use strict;
use Getopt::Long;
use Statistics::Descriptive;

my $stats = Statistics::Descriptive::Full->new();

my $CMDLINE = join(" ", @ARGV);

my $MIN_MATCH_LEN = 50;
my $MAX_OVERHANG  = 100;
my $MIN_READS     = 2;
my $MIN_REF_LEN   = 500;
my $MIN_IDENT     = 80;

my $HELP = 0;

my $result = GetOptions ("match=i"    => \$MIN_MATCH_LEN,
                         "overhang=i" => \$MAX_OVERHANG,
                         "reads=i"    => \$MIN_READS,
                         "reflen=i"   => \$MIN_REF_LEN,
                         "help"       => \$HELP);

if ($HELP || !$result)
{
  print STDERR "\n";
  print STDERR "coords2scafflinks.pl [options] coords\n";
  print STDERR "\n";
  print STDERR "  -match <len>    : min match length (default: $MIN_MATCH_LEN)\n";
  print STDERR "  -overhang <len> : max overhang (default: $MAX_OVERHANG)\n";
  print STDERR "  -reads <num>    : min number of supporting reads (default: $MIN_READS)\n";
  print STDERR "  -reflen <len>   : min reference length to consider (default: $MIN_REF_LEN)\n";
  print STDERR "  -minid <\%id>   : min percent id of alignment (default: $MIN_IDENT)\n";
  print STDERR "\n";
  print STDERR "Bundles with good links will be printed as:\n";
  print STDERR "++> rid1 [rlen1] -- bestoo bestcnt bestmed bestdist beststd -- rid2 [rlen2] :"; 
  print STDERR "++ dist\too\t|\tsep\thang\t||\tprid\tpoo\tphang\t||\tcrid\tcoo\tchang\t||\tqid\n";

  exit 0;
}

print "## $0 $CMDLINE\n";
print "## MIN_MATCH_LEN: $MIN_MATCH_LEN\n";
print "## MAX_OVERHANG: $MAX_OVERHANG\n";
print "## MIN_READS: $MIN_READS\n";
print "## MIN_REF_LEN: $MIN_REF_LEN\n";
print "## MIN_IDENT: $MIN_IDENT\n";
print "##\n";

## TODO: repeat checks <- read aligns equally well to multiple locations or kmer screening


## Scan alignments, load promising ones at the ends of the sequences into memory
## This should be a small fraction of the alignments, since most will be interior hits
###################################################################################################

my %alignments;
my %edges;
my %rinfo;

my $alncnt = 0;
my $alnedgecnt = 0;

my $inheader = 0;

while (<>)
{
  if (/^\//)
  {
    $inheader = 1;
    next;
  }

  if (/^================/)
  {
    $inheader = 0;
    next;
  }

  next if $inheader;

  $alncnt++;

  chomp;

  my $info;
  $info->{full}   = $_;

  $_ =~ s/^\s+//;

  my @vals = split /\s+/, $_;

  my $rid    = $vals[17];
  my $qid    = $vals[18];

  my $rstart = $vals[0];
  my $rend   = $vals[1];
  my $rlen   = $vals[11];
  my $ident  = $vals[9];

  $rinfo{$rid}->{len} = $rlen;

  my $matchlen = $rend - $rstart;

  if ($matchlen < $MIN_MATCH_LEN)
  {
    next;
  }

  if ($ident < $MIN_IDENT)
  {
    next;
  }
  
  if (($rstart < $MAX_OVERHANG) || ($rlen - $rend < $MAX_OVERHANG))
  {
    $alnedgecnt++;

    $info->{rstart} = $rstart;
    $info->{rend}   = $rend;

    $info->{qstart} = $vals[3];
    $info->{qend}   = $vals[4];

    $info->{qoo}    = 0;
    if ($info->{qstart} > $info->{qend}) { $info->{qoo} = 1; }

    $info->{ident}  = $ident;

    $info->{rlen}   = $rlen;
    $info->{qlen}   = $vals[12];

    $info->{rid}    = $rid;
    $info->{qid}    = $qid;

    push @{$alignments{$qid}->{$rid}}, $info;
  }
}

print  "## Loaded $alnedgecnt of $alncnt alignments\n";
print  STDERR "Loaded $alnedgecnt of $alncnt alignments\n";



## Now scan the alignments in memory, and look for alignments that span between at least two sequences
#######################################################################################################

my $readcnt = 0;
my $singletoncnt = 0;
my $paircnt = 0;
my $multicnt = 0;
my $edgecnt = 0;

foreach my $qid (keys %alignments)
{
  my @matches = keys %{$alignments{$qid}};
  my $matchcnt = scalar @matches;
  $readcnt++;

  if ($matchcnt > 1)
  {
    ## Found a spanner

    if    ($matchcnt == 2) { $paircnt++; }
    elsif ($matchcnt  > 2) { $multicnt++; }

    for (my $i = 0; $i < $matchcnt; $i++)
    {
      for (my $j = $i+1; $j < $matchcnt; $j++)
      {
        $edgecnt++;

        push @{$edges{$matches[$i]}->{$matches[$j]}}, $qid; 
        push @{$edges{$matches[$j]}->{$matches[$i]}}, $qid; 
      }
    }
  }
  elsif ($matchcnt == 1)
  {
    $singletoncnt++;
  }
}

print "## Loaded $edgecnt edges from $readcnt reads, $singletoncnt singletons alignments, $paircnt pairs $multicnt multis\n";
print STDERR "Loaded $edgecnt edges from $readcnt reads, $singletoncnt singletons alignments, $paircnt pairs $multicnt multis\n";
print "\n";


## Now scan the edges between reference sequences to evalute the strength of the link
#############################################################################################################

my $ctgpaircnt = 0;
my $candidate_links = 0;
my $valid_links = 0;
my $potential_bundles = 0;
my $good_bundles = 0;

foreach my $rid1 (sort {$rinfo{$b}->{len} <=> $rinfo{$a}->{len}} keys %edges)
{
  my @neighbors = keys %{$edges{$rid1}};

  foreach my $rid2 (sort {$rinfo{$b}->{len} <=> $rinfo{$a}->{len}} @neighbors)
  {
    my @qids = sort @{$edges{$rid1}->{$rid2}};
    my $qidcnt = scalar @qids;

    if ($qidcnt >= $MIN_READS)
    {
      $ctgpaircnt++;

      my $rlen1 = $rinfo{$rid1}->{len};
      my $rlen2 = $rinfo{$rid2}->{len};

      if (($rlen1 >= $MIN_REF_LEN) && ($rlen2 >= $MIN_REF_LEN))
      {
        my @goodspans;

        if (($rlen1 > $rlen2) ||
            (($rlen1 == $rlen2) && ($rid1 lt $rid2)))
        {
          print ">$rid1 [$rlen1] -- $qidcnt -- $rid2 [$rlen2]\n";
          $candidate_links++;

          my $info;
          
          my $sep = 0;

          foreach my $qid (@qids)
          {
            print " -\n" if $sep;
            
            my @alns;

            foreach my $ri (@{$alignments{$qid}->{$rid1}})
            {
              # print "r ", $ri->{full}, "\n";
              push @alns, $ri;
            }

            foreach my $qi (@{$alignments{$qid}->{$rid2}})
            {
              # print "q ", $qi->{full}, "\n";
              push @alns, $qi;
            }

            @alns = sort {$b->{qstart} < $a->{qstart}} @alns;

            for (my $i = 0; $i < scalar @alns; $i++)
            {
              my $cai = $alns[$i];

              if ($i > 0)
              {
                my $pai = $alns[$i-1];

                if ($cai->{rid} ne $pai->{rid})
                {
                  ## Spans sequences

                  my $prid  = $pai->{rid};
                  my $poo   = $pai->{qoo};
                  my $phang = 0;

                  my $crid  = $cai->{rid};
                  my $coo   = $cai->{qoo};
                  my $chang = 0;

                  my $sep  = 0;
                  my $oo   = "???";

                  if ($poo == 0 && $coo == 0)
                  {
                    ##      pai                       cai
                    ##  ----------s--e-->          --s--e------------>
                    ##          --s--e---------------s--e->

                    $oo   = "FF";

                    $phang = $pai->{rlen} - $pai->{rend};
                    $chang = $cai->{rstart};
                    $sep   = $cai->{qstart} - $pai->{qend};
                  }
                  elsif ($poo == 1 && $coo == 1)
                  {
                    ##      cai                       pai
                    ##  <---------e--s---         <--e--s-------------
                    ##          <-s--e---------------s--e--

                    $oo   = "RR";

                    $phang = $pai->{rlen} - $pai->{rstart};
                    $chang = $cai->{rend};
                    $sep   = $cai->{qend} - $pai->{qstart}; 
                  }
                  elsif ($poo == 0 && $coo == 1)
                  {
                    ##      pai                       cai
                    ##  ----------s--e-->         <--e--s-------------
                    ##          --s--e---------------e--s->

                    $oo   = "FR";

                    $phang = $pai->{rlen} - $pai->{rend};
                    $chang = $cai->{rlen} - $cai->{rend};
                    $sep   = $cai->{qend} - $pai->{qend};
                  }
                  elsif ($poo == 1 && $coo == 0)
                  {
                    ##      pai                       cai
                    ##  <---------e--s---         ---s--e------------>
                    ##          --e--s---------------s--e->

                    $oo   = "RF";

                    $phang = $pai->{rstart};
                    $chang = $cai->{rstart};
                    $sep   = $cai->{qstart} - $pai->{qstart};
                  }

                  my $hang = $phang + $chang;
                  my $dist = $sep - $hang; 

                  my $status = "OK";
                  if ($hang > $MAX_OVERHANG)
                  {
                    $status = "FAIL";
                  }

                  print "   == $status\t$dist\t$oo\t|\t$sep\t$hang\t||\t$prid\t$poo\t$phang\t||\t$crid\t$coo\t$chang\t||\t$qid\n";

                  if ($status eq "OK")
                  {
                    my $span;
                    $span->{dist} = $dist;
                    $span->{oo}   = $oo;
                    $span->{sep}  = $sep;
                    $span->{hang} = $hang;
                    $span->{prid} = $prid;
                    $span->{poo}  = $poo;
                    $span->{phang} = $phang;
                    $span->{crid}  = $crid;
                    $span->{coo}   = $coo;
                    $span->{chang} = $chang;
                    $span->{qid}   = $qid;

                    push @goodspans, $span;
                    $valid_links++;
                  }
                }
              }

              print $cai->{full}, "\n";
            }

            $sep = 1;
          }

          print "\n";
        }

        if (scalar @goodspans)
        {
          my $numspans = scalar @goodspans;
          $potential_bundles++;

          my %bundle;
          foreach my $s (@goodspans)
          {
            if (!exists $bundle{$s->{oo}})
            {
              $bundle{$s->{oo}}->{stats} = Statistics::Descriptive::Full->new();
            }

            $bundle{$s->{oo}}->{stats}->add_data($s->{dist});

            $bundle{$s->{oo}}->{cnt}++;
            $bundle{$s->{oo}}->{distsum} += $s->{dist};
            push @{$bundle{$s->{oo}}->{spans}}, $s;
          }

          my @oos = sort {$bundle{$b}->{cnt} <=> $bundle{$a}->{cnt}} keys %bundle;

          my $STATUS = "WEAK";

          if (scalar @goodspans >= $MIN_READS)
          {
            $STATUS = "GOOD";
            $good_bundles++;
          }

          my $bestoo  = $oos[0];
          my $bestcnt = $bundle{$oos[0]}->{cnt};
          my $bestdist = sprintf("%0.02f", $bundle{$oos[0]}->{distsum} / $bestcnt);
          my $beststd  = sprintf("%0.02f", $bestcnt > 1 ? $bundle{$oos[0]}->{stats}->standard_deviation() : 0);
          my $bestmed  = sprintf("%0.02f", $bundle{$oos[0]}->{stats}->median());

          print "++> $STATUS $rid1 [$rlen1] -- $bestoo $bestcnt $bestmed $bestdist $beststd -- $rid2 [$rlen2] :"; 

          foreach my $oo (@oos)
          {
            my $cnt = $bundle{$oo}->{cnt};
            my $avg = sprintf("%0.02f", $bundle{$oo}->{distsum} / $bundle{$oo}->{cnt});
            my $med = sprintf("%0.02f", $bundle{$oo}->{stats}->median());
            my $std = sprintf("%0.02f", $cnt > 1 ? $bundle{$oo}->{stats}->standard_deviation() : 0);
            print " $oo $cnt $med $avg +/- $std";
          }

          print "\n";

          foreach my $oo (@oos)
          {
            foreach my $s (@{$bundle{$oo}->{spans}})
            {
              print "++ $s->{dist}\t$s->{oo}\t|\t$s->{sep}\t$s->{hang}\t||\t$s->{prid}\t$s->{poo}\t$s->{phang}\t||\t$s->{crid}\t$s->{coo}\t$s->{chang}\t||\t$s->{qid}\n";
            }
          }

          print "\n"
        }
      }
    }
  }
}

print STDERR "Examined $ctgpaircnt contig pairs, $candidate_links candidate links, $valid_links valid links, $potential_bundles potential bundles, $good_bundles good bundles\n";

