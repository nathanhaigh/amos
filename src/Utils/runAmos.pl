#!__PERL

use lib "__LOCAL_LIB";
use TIGR::Foundation;
use strict;

my $VERSION = '$Revision$ ';
my $USAGE = q~
  USAGE: runAmos -C config_file [-s start] [-e end] prefix

Try 'runAmos -h' for more information.
~;
my $HELP = q~
    runAmos -C config_file [-s start] [-e end] [-clean] prefix 

    if the config file is not specified we use environment variable AMOSCONF
    if a start step is specified (-s) starts with that command
    if an end step is specified (-e) ends with the command prior to the number
    if -clean is specified, all files except for those listed in the variables INPUTS
and OUTPUTS get removed from the current directory

    e.g.  runAmos -s 1 -e 5   will run steps 1, 2, 3, and 4.
    
   Special variable PREFIX gets set to the prefix specified in the command line
   Lines starting with \# are comments
   Lines starting with \#\# get displayed when next command is run

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
$base->logAppend(1);

$base->setVersionInfo($VERSION);
$base->setHelpInfo($HELP);
$base->setDebugLevel(1);

my $conffile;
my $prefix;
my $start;
my $end;
my $clean;

my $err = $base->TIGR_GetOptions("conf|C=s"   => \$conffile,
				 "start|s=i"  => \$start,
				 "end|e=i"    => \$end,
				 "clean"      => \$clean);

if ($err != 1){
    $base->bail("Command line processing failed");
}

$prefix = $ARGV[0];

if (! defined $prefix){
    $base->bail($USAGE);
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
my $message;
my $startime;
my $noop = 0;

$variables{PREFIX} = $prefix;

open(CONF, "$conffile") || $base->bail("Cannot open $conffile: $!\n");

while (<CONF>){
    chomp;
    if (/^\#\#(.*)$/){ # comment that will be displayed
	$message = $1;
	next;
    }
    if (/^\#/){ #comment
	next;
    }
    if (/^\s*$/){ # empty line
	next;
    }
    
    if ($multiLine){
	if (/^\.\s*$/) {
	    if ($noop == 0){
		my $elapsed = time() - $startime;
		printf ("Elapsed: %s\n", prettyTime($elapsed));
		$base->logLocal(sprintf ("Elapsed: %s\n", prettyTime($elapsed)), 1);
            } else {
                $noop = 0;
            }
            $multiLine = 0;
            next;
        }
        doCommand($_);
	next;
    }
    if (/^(\S+)\s*=\s*(\S.*)\s*$/){ # variable definition
	if (! exists $variables{$1}){
	    $variables{$1} = substituteVars($2);
	    $base->logLocal("Setting variable $1 to $2", 2);
	}
	next;
    }
    if (/^(\d+)\s*:\s*(\S.*)\s*$/){ # one-line command
	if ($1 < $step){
	    $base->bail("Steps appear out of order at line $.");
	}
	$step = $1;
	if ((defined $start && $step < $start) ||
	    (defined $end && $step >= $end)){
	    $noop = 1;
	} else {
	    print "Doing step $step: $message\n";
	    $base->logLocal("Doing step $step: $message", 1);
	}
	$startime = time();
	doCommand($2);
	if ($noop == 0){
	    my $elapsed = time() - $startime;
	    printf ("Elapsed: %s\n", prettyTime($elapsed));
	    $base->logLocal(sprintf ("Elapsed: %s\n", prettyTime($elapsed)), 1);
	} else {
	    $noop = 0;
	}
	$message = "";
	next;
    }
    if (/^(\S+)\s*:\s*$/){ # multi-line command
	if ($1 < $step){
	    $base->bail("Steps appear out of order at line $.");
	}
	$step = $1;
	if ((defined $start && $step < $start) ||
	    (defined $end && $step >= $end)){
	    $noop = 1;
	} else {
	    print "Doing step $step: $message\n";
	    $base->logLocal("Doing step $step: $message", 1);
	}
	$message = "";
	$multiLine = 1;
	$startime = time();
	next;
    }
    $base->logError("Don't understand line $.: $_\n");
}

if (defined $clean){
    my %pardoned;
    my @inputs = split(/\s+/, $variables{"INPUTS"});
    my @outputs = split(/\s+/, $variables{"OUTPUTS"});
    while (@inputs){
	print "pardonning input $inputs[$#inputs]\n";
	$pardoned{pop(@inputs)} = 1;
    }
    while (@outputs){
	print "pardonning output $outputs[$#outputs]\n";
	$pardoned{pop(@outputs)} = 1;
    }
    opendir(DOT, ".") || $base->bail("Cannot open .: $!");
    while (my $file = readdir(DOT)){
	if ($file =~ /^.$/    ||
	    $file =~ /^..$/   ||
	    $file =~ /^.nfs/  ||
	    $file =~ /\.log$/ ||
	    $file =~ /\.error$/){ # pardon the useful files
	    next;
	}
	if (exists $pardoned{$file}){
	    next;
	}
#	print "Removing: $file\n";
	if (-d $file){
	    system("rm -rf $file");
	} else {
	    unlink $file || $base->logError ("Could not remove $file: $!\n");
	}
    }
    closedir(DOT);
}

exit(0);

sub doCommand
{
    my $command = shift;
    my $checkRet = 1;
    return if (! defined $step);
    return if ($noop);

    if ($command =~ /^-.*/){
	$checkRet = 0;
	$command =~ s/^-//;
    }
    $command = substituteVars($command);
    $base->logLocal("Running: $command", 1);

#save STDOUT and STDERR
    open(OLDOUT, ">&STDOUT");
    open(OLDERR, ">&STDERR");
    open(STDOUT, '>>', $base->getLogFile()); # append to log file
    open(STDERR, ">&STDOUT");
    my $ret = system($command);
    close(STDOUT);close(STDERR);
#restore STDOUT and STDERR
    open(STDOUT, ">&OLDOUT");
    open(STDERR, ">&OLDERR");
    if ($ret == -1){
	$base->bail("Failed to spawn command $command: $!\n");
    }
    if ($checkRet == 1 && $ret != 0){
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
    my @tokens = split(/(\$\(\w+\))/, $string);
    for (my $i = 0; $i <= $#tokens; $i++){
	if ($tokens[$i] =~ /\$\((\w+)\)/){
	    if (! exists $variables{$1}){
		$base->bail("Don't recognize variable $1 at line $.");
	    }
	    $tokens[$i] = $variables{$1};
	}
	$outstring .= $tokens[$i];
    }
#    while ($string =~ /\G([^\$]*)\$\((\w+)\)/gc){
#	$outstring .= "$1";
#	print "before is $1 variable is $2 and subst is $variables{$2}\n";
#	if (! exists $variables{$2}){
#	    $base->bail("Don't recognize variable $2 at line $.");
#	}
#	$outstring .= $variables{$2};
#    }
#    $string =~ /\G(.*)$/;
#    $outstring .= $1;
#    print "Into $outstring\n";
    return $outstring;
}

sub prettyTime
{
    my $elapsed = shift;
    
    my $secs =     $elapsed          % 60;
    my $min  = int($elapsed / 60)    % 60;
    my $hr   = int($elapsed / 3600)  % 60;
    my $day  = int($elapsed / 86400) % 24;

    return "${day}d ${hr}h ${min}m ${secs}s";
}
