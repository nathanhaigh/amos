#!/usr/local/bin/perl

use strict;
use TIGR::Foundation;

my $VERSION = '$Revision$ ';
my $HELP = q~
    runAmos -C config_file [-s start] [-e end] prefix

    if the config file is not specified uses the value in the
 environment variable AMOSCONF
    if a start step is specified (-s) starts with that command
    if an end step is specified (-e) ends with the command prior to the number

    e.g.  runAmos -s 1 -e 5   will run steps 1, 2, 3, and 4.
    
   Special variable PREFIX gets set to the prefix specified in the command line

  Example config file:

    PATH = /usr/local/bin
    PERL = $(PATH)/perl

    1: $(PERL) $(PREFIX).pl
    2:
    $(PERL) $(PREFIX)-1.pl
    $(PERL) $(PREFIX)-2.pl
    .

~;

my $base = new TIGR::Foundation();
if (! defined $base) {
    die("A horrible death\n");
}

$base->setVersionInfo($VERSION);
$base->setHelpInfo($HELP);
$base->setDebugLevel(1);

my $conffile;
my $prefix;
my $start;
my $end;

my $err = $base->TIGR_GetOptions("C=s"  => \$conffile,
				 "s=i"  => \$start,
				 "e=i"  => \$end);

if ($err != 1){
    $base->bail("Command line processing failed");
}

$prefix = $ARGV[0];

if (! defined $prefix){
    $base->bail("You must specify a prefix");
}

if (! defined $conffile){
    $conffile = $ENV{AMOSCONF};
}

if (! defined $conffile){
    $base->bail("You must specify a configuration file");
}

if (defined $start && defined $end && $end < $start){
    $base->bail("Start must be less than end!");
}

my %variables;
my $multiLine = 0;
my $step = 0;

$variables{PREFIX} = $prefix;

open(CONF, "$conffile") || $base->bail("Cannot open $conffile: $!\n");

while (<CONF>){
    chomp;
    if (/^\#/){ #comment
	next;
    }
    if (/^\s*$/){ # empty line
	next;
    }
    if ($multiLine){
	if (/^\.$/){
	    $multiLine = 0;
	    next;
	}
	doCommand($_);
    }
    if (/^(\S+)\s*=\s*(\S+)\s*$/){ # variable definition
	$variables{$1} = substituteVars($2);
	$base->logLocal("Setting variable $1 to $2", 2);
	next;
    }
    if (/^(\d+)\s*:\s*(\S.*)\s*$/){ # one-line command
	if ($1 < $step){
	    $base->bail("Steps appear out of order at line $.");
	}
	$step = $1;
	$base->logLocal("Doing step $step", 1);
	doCommand($2);
	next;
    }
    if (/^(\S+)\s*:\s*$/){ # multi-line command
	if ($1 < $step){
	    $base->bail("Steps appear out of order at line $.");
	}
	$step = $1;
	$base->logLocal("Doing step $step", 1);
	$multiLine = 1;
	next;
    }
    $base->logError("Don't understand line $.: $_\n");
}


sub doCommand
{
    my $command = shift;

    return if (! defined $step);
    return if (defined $start && $step < $start);
    return if (defined $end && $step >= $end);

    $command = substituteVars($command);
    $base->logLocal("Running: $command", 1);
    my $ret = system($command);
    if ($ret == -1){
	$base->bail("Failed to spawn command $command: $!\n");
    }
    if ($ret != 0){
	$base->bail(
		    sprintf("Command $command failed with exit code %d", 
			    $ret / 256)
		    );
    } 
}

sub substituteVars
{
    my $string = shift;
    my $outstring;

#    print "Parsing $string\n";
    while ($string =~ /\G([^\$]*)\$\((\w+)\)/gc){
	$outstring .= "$1";
#	print "before is $1 variable is $2 and subst is $variables{$2}\n";
	if (! exists $variables{$2}){
	    $base->bail("Don't recognize variable $2 at line $.");
	}
	$outstring .= $variables{$2};
    }
    $string =~ /\G(.*)$/;
    $outstring .= $1;
#    print "Into $outstring\n";
    return $outstring;
}
