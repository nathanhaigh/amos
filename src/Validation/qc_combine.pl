#!/usr/bin/perl -w

#-------------------------------------------------------------------------------
#   Programmer: Adam M Phillippy
#         File: qc_combine
#         Date: 08 / 01 / 03
#
#        Usage:
#    qc_combine  [options]  <qcfile1>  <qcfile2>  . . .  <qcfileN>
#
#                Try 'qc_combine -h' for more information.
#
#      Purpose: To combine multiple CA qc files into a single side-by-side
#               comparison qc file.
#
#-------------------------------------------------------------------------------

use TIGR::Foundation;
use strict;


my $VERSION_INFO = " Version 1.00";


my $HELP_INFO = q~
  USAGE: qc_combine  [options]  <qcfile1>  <qcfile2>  . . .  <qcfileN>

  DESCRIPTION:
    qc_combine merges multiple CA qc files into a single side-by-side
    comparison qc file. Useful for viewing multiple qc files from different
    assemblies of the same data. Any number of qc files will be accepted
    as arguments on the command line, and the resulting merge will be output
    to stdout.

  OPTIONS:
    --depend        Print the dependency information and exit
    -h
    --help          Display help information and exit
    -V
    --version       Display the version information and exit
    ~;


my $USAGE_INFO = q~
  USAGE: qc_combine  [options]  <qcfile>  <qcfile>  . . .  <qcfileN>
    ~;


my @DEPEND_INFO =
    (
     "TIGR::Foundation"
     );


sub main ( )
{
    my $tigr;             # TIGR::Foundation object
    my @err;              # Error variable

    my @filehandles;      # list of single qc files
    my $filename;         # single filename
    my $filehandle;       # single filehandle
    my $currtag;          # current line tag
    my $tag;              # line tag
    my $line;             # line
    my $data;             # line data

    #-- Initialize TIGR::Foundation
    $tigr = new TIGR::Foundation;
    if ( !defined ($tigr) ) {
	die "ERROR: TIGR::Foundation could not be initialized.\n";
    }
    
    #-- Set help and usage information
    $tigr->setHelpInfo ($HELP_INFO);
    $tigr->setUsageInfo ($USAGE_INFO);
    $tigr->setVersionInfo ($VERSION_INFO);
    $tigr->addDependInfo (@DEPEND_INFO);

    #-- Get command line parameters
    $err[0] = $tigr->TIGR_GetOptions
	(
 
	 );

    #-- Check if the parsing was successful
    if ( $err[0] == 0 ) {
	$tigr->printUsageInfo( );
	die "Try '$0 -h' for more information.\n";
    }

    #-- If 0 qc files on command line, nothing to do
    if ( scalar(@ARGV) == 0 ) {
	die "No qc files were specified, nothing to do.\n";
    }

    #-- Open the qc files and print the header as we go
    printf "%25s", "";
    foreach $filename (@ARGV) {
	printf " %15s", substr $filename, 0, 15;

	undef $filehandle;
	open $filehandle, "<$filename" or
	    die "ERROR: Could not open qc file \"$filename\": $!\n";
	push @filehandles, $filehandle;
    }
    print "\n\n";


    #-- Print the body
    my @finished = ( );
    while ( ! @finished ) {
	undef $currtag;

	foreach $filehandle (@filehandles) {

	    if ( ! ($line = <$filehandle>) ) {
		#-- EOF
		push @finished, $filehandle;
		next;
	    }

	    my $spacer;
	    my $header;
	    my $datatag;
	    my $data;
	    ($spacer, $header, $datatag, $data) = $line =~
		/^\s*(\n)$|^(\[.+\])\s*$|^(\w+)(?:\s+|=)(\S+)\s*$/;
	    
	    #-- Switch on the type of line we got
	    if ( defined($spacer) ) {
		#-- Empty line
		$tag = $spacer;
		
	    } elsif ( defined($header) ) {
		#-- [] header
		$tag = $header;

	    } elsif ( defined($datatag)  &&  defined($data) ) {
		#-- Data line
		$tag = $datatag;

	    } else {
		die "ERROR: Malformed line: \"$line\"\n";
	    }
	    chomp $tag;

	    #-- Dump the merged data
	    if ( defined($currtag) ) {
		if ( $currtag ne $tag ) {
		    die "ERROR: Disagreeing tags:\n$currtag\n$tag\n";
		}
	    } else {
		#-- 1st column is the tag
		$currtag = $tag;
		if ( $tag =~ /^\d+$/ ) {
		    #-- List data
		    printf "%-10s", $tag;
		} else {
		    #-- If it's not a list tag
		    $tag =~ s/,avgContig,avgGap/""/e;
		    printf "%-25s", $tag;
		}
	    }

	    if ( defined($data) ) {
		#-- If we got data, append a column to the line
		if ( $currtag =~ /^\d+$/ ) {
		    #-- List data
		    $data =~ s/,[0-9.\-]+,[0-9.\-]+\s*$/""/e;
		    printf " %-25s", $data;
		} else {
		    #-- Normal data
		    printf " %15s", $data;
		}
	    }
	}

	print "\n";
    }

    #-- Check that all were exhausted
    if ( scalar(@finished) != scalar(@filehandles) ) {
	warn "WARNING: not all input files were the same length.\n";
    }

    #-- Close em down
    foreach $filehandle (@filehandles) {
	close $filehandle;
    }
    
    #-- Return success
    return (0);
}

exit ( main ( ) );

#-- END OF SCRIPT
