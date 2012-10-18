#!/usr/bin/perl -w
 
use strict;
use warnings;

# TIGR Modules
use TIGR::Foundation;
use TIGR::AsmLib;

$| = 1;

my $tigr_tf = new TIGR::Foundation;
my $PRG = $tigr_tf->getProgramInfo('name');
my $VERSION = '$Revision$ ';
my @DEPENDS=("TIGR::Foundation","TIGR::AsmLib");

# help info
my $HELPTEXT = qq~
Program that ...

Usage: $PRG msg_file [options]
	  
  INPUT:  message file 
       
  options:
	-h|help		- Print this help and exit;
	-V|version	- Print the version and exit;
	-depend		- Print the program and database dependency list;
	-debug <level>	- Set the debug <level> (0, non-debug by default); 
 
  OUTPUT: 6 column tab delimited file 
~;

my $MOREHELP = qq~
Return Codes:   0 - on success, 1 - on failure.
~;

###############################################################################
#
# Main program
#
###############################################################################

MAIN:
{
	my %options;
	my (%eid,%iid);
	
	# Configure TIGR Foundation
	$tigr_tf->setHelpInfo($HELPTEXT.$MOREHELP);
        $tigr_tf->setUsageInfo($HELPTEXT);
        $tigr_tf->setVersionInfo($VERSION);
        $tigr_tf->addDependInfo(@DEPENDS);
	
	# validate input parameters
	my $result = $tigr_tf->TIGR_GetOptions(	);
	$tigr_tf->printUsageInfoAndExit() if (!$result);
	
	#########################################################################

        print join "\t",("#MSG","iid","eid","MSG","iid","eid");
        print "\n";

	while(<>)
	{
		#0	 1	 2		 3	 4	 5
		#SCF     0       7180000001304   CCO     .       7180000000992
		#CCO     6536    7180000001299   UTG     .       7180000000211
		#CCO     6       7180000000993   AFG     128841  .
		
		next if(/^#/);

		my @f=split;
		
		if($f[0] eq "SCF" and $f[3] eq "CCO")
		{
			if($f[4] ne ".") { $eid{$f[3]}{$f[4]}=join "\t",@f[3..5] }
			if($f[5] ne ".") { $iid{$f[3]}{$f[5]}=join "\t",@f[3..5] }     
		}
		elsif($f[0] eq "CCO" and $f[3] eq "AFG")
                {
			if($f[1] ne "." and $eid{$f[0]}{$f[1]})    { print join "\t",$eid{$f[0]}{$f[1]},@f[3..5]; print "\n";}
			elsif($f[2] ne "." and $iid{$f[0]}{$f[2]}) { print join "\t",$iid{$f[0]}{$f[2]},@f[3..5]; print "\n";}
                }
	}	

	exit 0;
}
