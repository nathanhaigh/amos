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
Program that parses an AMOS message file (.afg) and prints out related message information in a TAB delimited format:
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
 
  OUTPUT:  6 column tab delimited file 
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

		if($id eq "SCF" or $id eq "CTG")
                {
                        foreach my $rec2 (@$recs)
                        {
                                my($id2, $fields2, $recs2) = parseCARecord($rec2);

                                if($id2 eq "TLE")
                                {
                                        my %fields2=%$fields2;
					$id2=($id eq "SCF")?"CTG":"RED";

                                        print join "\t",($id,$fields{iid},$fields{eid},$id2,$fields2{src},".");
                                        print "\n";
                                }
                        }
                }
		elsif($id eq "RED")
		{
			print join "\t",($id,$fields{iid},".","FRG",$fields{frg},".");
                        print "\n";
                }
		elsif($id eq "FRG" and $fields{rds})
                {
			my @rds=split ",",$fields{rds};

        		if(@rds)
			{
	                	print join "\t",($id,$fields{iid},".","RED",$rds[0],"."); print "\n";
				print join "\t",($id,$fields{iid},".","RED",$rds[1],"."); print "\n";
			}
                }
	}
	

	exit 0;
}
