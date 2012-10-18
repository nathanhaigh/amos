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
Program that filters a message file (CA,AMOS) based on the message ids

Usage: $PRG -f id_file msg_file [options]
	  

  INPUT:  
  	id_file		: 3 column tab delimited: message type, internal id, external id
	msg_file 	: CA or AMOS file
   
  options:
    	-f id_file	: 3 column tab delimited: message type, internal id, external id
	
	-h|help		- Print this help and exit;
	-V|version	- Print the version and exit;
	-depend		- Print the program and database dependency list;
	-debug <level>	- Set the debug <level> (0, non-debug by default); 
 
  OUTPUT: new msg_file
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
	my %offset;
	my %len;
	
	# Configure TIGR Foundation
	$tigr_tf->setHelpInfo($HELPTEXT.$MOREHELP);
        $tigr_tf->setUsageInfo($HELPTEXT);
        $tigr_tf->setVersionInfo($VERSION);
        $tigr_tf->addDependInfo(@DEPENDS);
	
	# validate input parameters
	my $result = $tigr_tf->TIGR_GetOptions(
		"f=s"	=>	\$options{f}
	);
	$tigr_tf->printUsageInfoAndExit() if (!$result);
	
	#########################################################################

	open(FILTER,$options{f}) or die $!; 
	binmode STDIN;

	while(<FILTER>)
	{
		#0	 1 	 2	 3	 4
		#FRG     200600  178946  1612    46
		
		next if(/^#/);
		my ($id,$iid,$eid,$offset,$len,$buffer)=split;

		next unless($len);
		seek STDIN,$offset,0;
		die "ERROR: seek $offset\n" unless($offset==tell(STDIN));		

		read (STDIN, $buffer, $len); 
		chomp $buffer;

		die "ERROR: $buffer " unless($buffer=~/^{/ and $buffer=~/}$/);
		print $buffer,"\n";
	}
	close(FILTER);

	exit 0;
}
