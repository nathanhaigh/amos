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
Program that parses a CA message file (.frg or .asm) and prints out related message information in a TAB delimited format:
  1. type  (parent)
  2. internal identifier
  3. external identifier
  
  4. type  (child)
  5. internal identifier
  6. external identifier
  
Missing identifiers are replaced with "." (to keep the number of columns==6)

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

	# parse input
	while(my $rec = getCARecord(\*STDIN))
	{
		my($id, $fields, $recs) = parseCARecord($rec);
		my %fields=%$fields;

		if($id eq "LKG")	# frg.LKG
		{
			print join "\t",("FRG",$fields{fg1},".","LKG",$fields{fg1},"."); print "\n";
			print join "\t",("FRG",$fields{fg2},".","LKG",$fields{fg2},"."); print "\n";
			
			print join "\t",("FRG",$fields{fg1},".","FRG",$fields{fg2},"."); print "\n";
			print join "\t",("FRG",$fields{fg2},".","FRG",$fields{fg1},"."); print "\n";
                }
		elsif($id eq "SCF")	# asm.SCF
                {
			$fields{acc}=~/(\d+),(\d+)/ or die "ERROR: $rec";
			($fields{iid},$fields{eid})=($2,$1);

                        foreach my $rec2 (@$recs)
                        {
                                my($id2, $fields2, $recs2) = parseCARecord($rec2);

                                if($id2 eq "CTP")
                                {
                                        my %fields2=%$fields2;
					
                                        print join "\t",($id,$fields{iid},$fields{eid},"CCO",".",$fields2{ct1}); print "\n";
					if($fields2{ct1} != $fields2{ct2})
					{
						print join "\t",($id,$fields{iid},$fields{eid},"CCO",".",$fields2{ct2}); print "\n";
					}
                                }
                        }
                }
		elsif($id eq "CCO" or $id eq "UTG")	# asm.CCO,UTG
                {
			$fields{acc}=~/(\d+),(\d+)/ or die "ERROR: $rec";
			($fields{iid},$fields{eid})=($2,$1);

                        foreach my $rec2 (@$recs)
                        {
                                my($id2, $fields2, $recs2) = parseCARecord($rec2);

                                if($id2 eq "MPS")
                                {
                                        my %fields2=%$fields2;
				
                                         print join "\t",($id,$fields{iid},$fields{eid},"AFG",$fields2{mid},"."); print "\n";
                                }
				elsif($id2 eq "UPS")
                                {
                                        my %fields2=%$fields2;
				
                                         print join "\t",($id,$fields{iid},$fields{eid},"UTG",".",$fields2{lid}); print "\n";
                                }
                        }
                }
	}
	

	exit 0;
}
