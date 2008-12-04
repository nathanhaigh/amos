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
Program that parses a CA message file (.frg or .asm) and prints out the following information in a TAB delimited format:
  1. message type 
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
 
  OUTPUT:   5 column tab delimited file 
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
	my $len;
	my $n=0;
	my %offset;
	my %len;
	
	# Configure TIGR Foundation
	$tigr_tf->setHelpInfo($HELPTEXT.$MOREHELP);
        $tigr_tf->setUsageInfo($HELPTEXT);
        $tigr_tf->setVersionInfo($VERSION);
        $tigr_tf->addDependInfo(@DEPENDS);
	
	# validate input parameters
	my $result = $tigr_tf->TIGR_GetOptions(	);
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

                if($id eq "BAT" or $id eq "DST")			## frg.BAT,DST
                {
                        print join "\t",($id,$fields{acc},".",$offset,$len);print "\n"; 
                }
		elsif($id eq "FRG")					## frg.FRG 
                {
                        $fields{src}="." unless $fields{src};
			chomp $fields{src};

                        print join "\t",($id,$fields{acc},$fields{src},$offset,$len);print "\n"; 
                }
		elsif($id eq "LKG")					## frg.LKG
		{
			print join "\t",($id,$fields{fg1},".",$offset,$len); print "\n"; 
			print join "\t",($id,$fields{fg2},".",$offset,$len); print "\n"; 
		}
		elsif($id eq "AFG")					## asm.AFG
                {
			$fields{acc}=~/(\w+),(\w+)/;

                        print join "\t",($id,$1,".",$offset,$len);print "\n"; 
                }
		elsif($id eq "UTG" or $id eq "CCO" or $id eq "SCF")	## asm.CCO,SCF,UTG
                {
			$fields{acc}=~/(\d+),(\d+)/;

                        print join "\t",($id,$2,$1,$offset,$len);print "\n"; 
                }
		else							## frg.ADL ; asm.AMP,CLK,MDI,ULK
		{
			print join "\t",($id,".",".",$offset,$len);print "\n";
		}

		$offset+=$len;
	}

	exit 0;
}
