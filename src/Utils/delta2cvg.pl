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
Program that computes alignment coverage from a Mummer delta file

Usage: $PRG < delta_file [options]
	
  INPUT:
	delta_file	

	#>Streptococcus_suis 2_14_26_F3 2007491 46
	#1282180 1282217 39 2 0 0 0
	#0
	#>Streptococcus_suis 2_14_233_F3 2007491 46
	#1082721 1082752 33 2 0 0 0
	#0


  options:
	-m <n>		- Min coverage to display
	-M <n>		- Max coverage to display
	-merge		- Merge coverage intervals 

	-h|help		- Print this help and exit;
	-V|version	- Print the version and exit;
	-depend		- Print the program and database dependency list;
	-debug <level>	- Set the debug <level> (0, non-debug by default); 

  OUTPUT:
	Example:
		J28690Ab07.q1k	1	9	0
		J28690Ab07.q1k 	9	112	1
		J28690Ab07.q1k  112	951	2
		....	
~;
                                                                                                                                                             
my $MOREHELP = qq~
Return Codes:   0 - on success, 1 - on failure.
~;

sub getCoverage($$)
{
	my ($contig_name,$count_ref,$options_ref)=@_;
	my %count=%$count_ref;
	my %options=%$options_ref;
	
	#display clone coverage
	my $count=0;
	
	my @keys=sort {$a <=> $b} keys %count;	
	my $n=scalar(@keys);
	 
	foreach my $i (1..$n-1)
	{
		$count{$keys[$i]}+=$count{$keys[$i-1]}
	}

	if($options{merge})
	{
		my @del;
		
		if($options{m})
		{
			foreach my $i (1..$n-2)
			{
				if($count{$keys[$i-1]}>=$options{m} and $count{$keys[$i]}>=$options{m})
				{
					push @del,$keys[$i];
				}
			}
		}

                if($options{M})
                {
                        foreach my $i (1..$n-2)
                        {
				if($count{$keys[$i-1]}<=$options{M} and $count{$keys[$i]}<=$options{M})
				{
					push @del,$keys[$i];
				}
                        }
                }

		foreach (@del)
		{
			delete $count{$_};
		}
	}

        @keys=sort {$a <=> $b} keys %count;
        $n=scalar(@keys);

        foreach my $i (1..$n-1)
        {
		next if(defined($options{m}) and $count{$keys[$i-1]}<$options{m});
		next if(defined($options{M}) and $count{$keys[$i-1]}>$options{M});

                print join "\t",($contig_name,$keys[$i-1],$keys[$i],$keys[$i]-$keys[$i-1],$count{$keys[$i-1]});
		print "\n";
        }
}


	
###############################################################################
#
# Main program
#
###############################################################################

MAIN:
{
	# Configure TIGR Foundation
	$tigr_tf->setHelpInfo($HELPTEXT.$MOREHELP);
        $tigr_tf->setUsageInfo($HELPTEXT);
        $tigr_tf->setVersionInfo($REV);
        $tigr_tf->addDependInfo(@DEPENDS);
	
	# validate input parameters
	my %options;

	my $result = $tigr_tf->TIGR_GetOptions(
		"m=s" => \$options{m},
		"M=s" => \$options{M},
		"merge"	=> \$options{merge}
		);
	$tigr_tf->printUsageInfoAndExit() if (!$result);

	# parse the .contig file
	my %count;
	my $contig_name;

	while(<>)
	{

	        #>Streptococcus_suis 2_14_26_F3 2007491 46
	        #1282180 1282217 39 2 0 0 0
	        #0
        	#>Streptococcus_suis 2_14_233_F3 2007491 46
	        #1082721 1082752 33 2 0 0 0
        	#0

		my @f=split;

		if(/^\// or /^NUCMER/) {}
		elsif(/^>/)
		{
			my $id=$f[0];
			$id=~s/>//;

			if(defined($contig_name) and $id ne $contig_name)
			{
				&getCoverage($contig_name,\%count,\%options);
				%count=();	
			}	
	
			$contig_name=$id;
			$count{1}+=0;
                	$count{$f[2]}+=0;
		}
		elsif(scalar(@f)==7)
		{
			$count{$f[0]}++;
			$count{$f[1]}--;
		}
	}

	if ($contig_name)
	{	
		&getCoverage($contig_name,\%count,\%options);
	}

	exit 0;
}
