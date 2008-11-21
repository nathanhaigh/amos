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
Program that parses an AMOS message file (.afg) and prints out the following information in a TAB delimited format:
  1. type 
  2. internal identifier
  3. external identifier
  4. offset from the beginning of the file
  5. length
  
Missing identifiers are replaced with "." (to keep the number of columns==5)

Usage: $PRG msg_file [options]
	  
  INPUT:  message file 
     
  options:
	-h|help		- Print this help and exit;
	-V|version	- Print the version and exit;
	-depend		- Print the program and database dependency list;
	-debug <level>	- Set the debug <level> (0, non-debug by default); 
 
  OUTPUT:  5 column tab delimited file 
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
	my $offset=0;
	my $len=0;
	my $n=0;
	my %offset;
	my %len;
	
	# Configure TIGR Foundation
	$tigr_tf->setHelpInfo($HELPTEXT.$MOREHELP);
        $tigr_tf->setUsageInfo($HELPTEXT);
        $tigr_tf->setVersionInfo($VERSION);
        $tigr_tf->addDependInfo(@DEPENDS);
	
	# validate input parameters
	my $result = $tigr_tf->TIGR_GetOptions();
	$tigr_tf->printUsageInfoAndExit() if (!$result);
	
	#########################################################################

        print join "\t",("#MSG","iid","eid","offset","len");
        print "\n";
	
	# parse input
	while(my $rec = getCARecord(\*STDIN))
	{
		my($id, $fields, $recs) = parseCARecord($rec);
		my %fields=%$fields;

		my $len=length($rec);

		if($fields{iid} and $fields{eid})	## afg,LIB,FRG,RED,CTG,SCF
		{
			print join "\t",($id,$fields{iid},$fields{eid},$offset,$len);print "\n"; 
		}		
		elsif($fields{eid})			## afg.UNV
		{
			print join "\t",($id,".",$fields{eid},$offset,$len);print "\n"; 
		}		
		else					## afg.CTE
		{
			print join "\t",($id,".",".",$offset,$len);print "\n";
		}

		$offset+=$len;
	}

	exit 0;
}
