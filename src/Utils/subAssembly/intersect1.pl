#!/usr/bin/perl -w
 
use strict;
use warnings;

# TIGR Modules
use TIGR::Foundation;

my $tigr_tf = new TIGR::Foundation;
my $PRG = $tigr_tf->getProgramInfo('name');
my $VERSION = '$Revision$ ';
my @DEPENDS=("TIGR::Foundation");

# help info
my $HELPTEXT = qq~
Program that filters records from one file based on record information from a second file

Usage: $PRG id_file1 id_file2 [options]

	INPUT:   
		id_file1:  3+ column tab delimited: message type, internal id, external id
		id_file2:  3  column tab delimited: message type, internal id, external id
	
	  options:
			
	OUTPUT:  
		all the records in file1 that have a correspondent in file2
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
	my (%iid,%eid,%offset);
	my $changed=1;
	
	# Configure TIGR Foundation
	$tigr_tf->setHelpInfo($HELPTEXT.$MOREHELP);
        $tigr_tf->setUsageInfo($HELPTEXT);
        $tigr_tf->setVersionInfo($VERSION);
        $tigr_tf->addDependInfo(@DEPENDS);
	
	# validate input parameters
	my $result = $tigr_tf->TIGR_GetOptions(	);
	$tigr_tf->printUsageInfoAndExit() if (!$result);
	
	scalar(@ARGV)==2 or 
		 $tigr_tf->bail("Incorrect number of parameters");	
		 
	# read 2nd file
	open(IN,$ARGV[1]) or $tigr_tf->bail("Cannot open input file".$!) ;
	while(<IN>)
	{
    		my @f=split;
		next unless(@f);

    		$iid{$f[0]}{$f[1]}=1 if($f[1] ne ".");
		$eid{$f[0]}{$f[2]}=1 if($f[2] ne ".");
	}
	close(IN);

	# read 1st file
	open(IN,$ARGV[0]) or die;
	while(<IN>)
	{
		#0	 1	 2	    	 3	 4
		#id      iid     eid             offset  len
		#FRG     67526   198392091       4356    1727
		
		my @f=split;
                if(($iid{$f[0]}{$f[1]} or $eid{$f[0]}{$f[2]}) and ! $offset{$f[3]})
		{
			print $_;
			$offset{$f[3]}=1;
		}
	}
	close(IN);	
	
	
	exit 0;
}
