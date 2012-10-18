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
Program that identifies and prints related records from two files 

Usage: $PRG map_file id_file [options]

	INPUT:   
		id_file:  3 column tab delimited: message type, internal id, external id
		map_file: 6 column tab delimited: message type, internal id, external id (parent), \
				  message type, internal id, external id (child)
	
	options:
	
	-h|help		- Print this help and exit;
	-V|version	- Print the version and exit;
	-depend		- Print the program and database dependency list;
	-debug <level>	- Set the debug <level> (0, non-debug by default); 
			
	OUTPUT:  
		all the records in id file and related(child) records from the map_file
~;

my $MOREHELP = qq~
Return Codes:   0 - on success, 1 - on failure.
~;


###############################################################################
#
# Main program
#
###############################################################################


	# ==> Ba.afg.info.tail <==
	# SCF     362     7180000001353   249103764       67
	# SCF     363     7180000001354   249103831       138
	# SCF     364     7180000001355   249103969       67
	# 
	# ==> Ba.afg.map <==
	# SCF     362     7180000001353   CTG     304     .
	# ...
	# CTG     304     7180000001295   RED     83836   .
	# CTG     304     7180000001295   RED     33896   .
	# CTG     304     7180000001295   RED     93344   .
	# CTG     304     7180000001295   RED     71388   .
	# ..
	# RED     23047   .       FRG     251797  .
	# ...
	# FRG     251797  .       RED     23047   .
	# FRG     251797  .       RED     23048   .


MAIN:
{
	my (%iid,%eid);
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
				
		print join "\t",@f[0..2]; print "\n";
	}
	close(IN);

	while($changed)
	{
		$changed=0;
		
		# read 1st file
		open(IN,$ARGV[0]) or die;
		while(<IN>)
		{
    			my @f=split;
						
                	if($iid{$f[0]}{$f[1]} or $eid{$f[0]}{$f[2]}) 
			{
				if($f[4] ne "." and !$iid{$f[3]}{$f[4]})
				{
					$iid{$f[3]}{$f[4]}=1;
					print join "\t",@f[3..5]; print "\n";
					$changed=1;
				}	
				elsif($f[5] ne "." and !$eid{$f[3]}{$f[5]})
				{
					$eid{$f[3]}{$f[5]}=1;
					print join "\t",@f[3..5]; print "\n";
					$changed=1;
				}	
                	}
		}
		close(IN);
	}
	
	
	
	exit 0;
}
