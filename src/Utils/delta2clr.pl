#!/usr/bin/perl -w
 
use strict;
use warnings;

# TIGR Modules
use TIGR::Foundation;

my $tigr_tf = new TIGR::Foundation;
my $PRG = $tigr_tf->getProgramInfo('name');
my $REV="1.0";
my @DEPENDS=("TIGR::Foundation");

# help info
my $HELPTEXT = qq~

Usage: $PRG < delta_file [options]

	INPUT:   
		the mummer delta file
		
        options:

		-zero_cvg file	- File that contain zero coverage regions; 
                          reads ending in these	regions won't get trimmed

		-read_len <n>	- The untrimmed read lengths; Default 32 for Solexa reads
	
		-h|help		- Print this help and exit;
		-V|version	- Print the version and exit;
		-depend		- Print the program and database dependency list;
		-debug <level>	- Set the debug <level> (0, non-debug by default); 
 
	OUTPUT:  
		Delta at the console
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
	$options{read_len}=32;
	my $is_zero_cvg=0;

	# Configure TIGR Foundation
	$tigr_tf->setHelpInfo($HELPTEXT.$MOREHELP);
        $tigr_tf->setUsageInfo($HELPTEXT);
        $tigr_tf->setVersionInfo($REV);
        $tigr_tf->addDependInfo(@DEPENDS);
	
	# validate input parameters
	my $result = $tigr_tf->TIGR_GetOptions(
		"zero_cvg=s"	=>	\$options{zero_cvg},
		"read_len=s"	=>	\$options{read_len}
	);
	
	$tigr_tf->printUsageInfoAndExit() unless($result);

	########################################################################
		
	my %zero_cvg;		
	if(defined($options{zero_cvg}))
	{
		open(IN,$options{zero_cvg}) or die $!;
		while(<IN>)
		{
			my @f=split;
			next unless(@f);
			$zero_cvg{$f[0]}{$f[1]}=1;
			$zero_cvg{$f[0]}{$f[2]}=1; 
		}
		close(IN);
	}

	########################################################################
		
	my ($ref,$id,$min,$max);
	
	# read the Delta file
	while(<>)
	{	
		#>1 gnl|ti|185591439 3256683 909
		#3252989 3253871 16 909 19 19 0
		#-44
		#5
		#0
		#=> 3256683 16 909

		#>gi|116048575|ref|NC_008463.1| 52 6537648 33
		#287894 287914 21 1 0 0 0
		#0
		#1554749 1554781 1 33 1 1 0
		#0
		#4001147 4001167 1 21 0 0 0
		#0
		#4842167 4842192 6 31 0 0 0
		#0
		#=> 52  1 33

		my @f=split;

    		if(/^>/)
    		{
			if(defined($id))
			{
				print join " ",($id,$min,$max);
				print "\n";
			}

			$ref=$f[0];
			$ref=~s/>//;
			$id=$f[1];

			undef($min);
			undef($max);
    		}
		elsif(scalar(@f)==7)
		{
			if($zero_cvg{$ref}{$f[0]})
			{
				if($f[2]<$f[3]) 
				{ 	
					$f[2]=1;
				}
				else            
				{ 
					$f[2]=$options{read_len}; 
				}
			}

                        if($zero_cvg{$ref}{$f[1]})
                        {
                                if($f[2]<$f[3]) 
				{ 
					$f[3]=$options{read_len};
				} 
                                else            
				{ 
					$f[3]=1; 
				}
                        }  

			if($f[2]>$f[3]) { ($f[2],$f[3])=($f[3],$f[2]); }
			$f[2]--;

			if(!defined($min)) { ($min,$max)=($f[2],$f[3]); }
			else 
			{
				$min=$f[2] if($f[2]<$min);
				$max=$f[3] if($f[3]>$max);
			}
		}
	}

	if(defined($id))
	{                                
		print join " ",($id,$min,$max);
		print "\n"
	}


	exit 0;
}
