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
Program that parses an AMOS message file and resassigns internal ids to scaffolds and contigs

Usage: $PRG -f id_file msg_file [options]
	  
  INPUT:  
  
  	id_file:  tab delimnited file : message type, old internal id, new internal id
	msg_file: AMOS msg file
   
  options:
  	-f id_file
	
	-h|help		- Print this help and exit;
	-V|version	- Print the version and exit;
	-depend		- Print the program and database dependency list;
	-debug <level>	- Set the debug <level> (0, non-debug by default); 
 
  OUTPUT:  new msg_file
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
	my %h;
	
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
	
	open(IN,$options{f}) or $tigr_tf->bail("Cannot open input file".$!) ;
	while(<IN>)
	{
		#CTG     99      1
		
    		my @f=split;
		next unless(@f);

    		$h{$f[0]}{$f[1]}=$f[2];		
	}
	close(IN);
	
	#########################################################################

        # parse input
	while(my $rec = getCARecord(\*STDIN))
	{
		my($id, $fields, $recs) = parseCARecord($rec);
		my %fields=%$fields;

		if($id eq "SCF")
                {
                        my @rec=split /\n/,$rec;
			foreach (@rec)
			{
				
				if(/^iid:(\d+)/)    
				{ 
					defined($h{"SCF"}{$1}) or die "ERROR: unknown SCF $1\n";
					print "iid:",$h{"SCF"}{$1}; 
				}
				elsif(/^src:(\d+)/) 
				{ 
					defined($h{"CTG"}{$1}) or die "ERROR: unknown CTG $1\n";
					print "src:",$h{"CTG"}{$1}; 
				}
				else                
				{ 
					print $_;
				}
				
				print "\n";
			}
				
                }		
		elsif($id eq "CTG")
                {
                        my @rec=split /\n/,$rec;
			foreach (@rec)
			{
				if(/^iid:(\d+)/)    
				{ 
					defined($h{"CTG"}{$1}) or die "ERROR: unknown CTG $1\n";
					print "iid:",$h{"CTG"}{$1}; 
				}
				else                
				{ 
					print $_;
				}
				
				print "\n";
			}
                }
		elsif($id eq "CTE")
		{
			my @rec=split /\n/,$rec;
			foreach (@rec)
			{
				if(/^nds:(\d+),(\d+)/)    
				{ 
					defined($h{"CTG"}{$1}) or die "ERROR: unknown CTG $1\n";
					defined($h{"CTG"}{$2}) or die "ERROR: unknown CTG $2\n";
					
					print "nds:",$h{"CTG"}{$1},",",$h{"CTG"}{$2}; 
				}
				else                      
				{ 
					print $_;
				}
				
				print "\n";
			}
                }        
		else 
		{ 
	 		print $rec 
		}
	}

	exit 0;
}
