#!/usr/bin/perl -w
# Copyright @ 2002 - 2010 The Institute for Genomic Research (TIGR).
# All rights reserved.
# 
# This software is provided "AS IS".  TIGR makes no warranties, express or
# implied, including no representation or warranty with respect to the
# performance of the software and derivatives or their safety,
# effectiveness, or commercial viability.  TIGR does not warrant the
# merchantability or fitness of the software and derivatives for any
# particular purpose, or that they may be exploited without infringing the
# copyrights, patent rights or property rights of others.
# 
# This software program may not be sold, leased, transferred, exported or
# otherwise disclaimed to anyone, in whole or in part, without the prior
# written consent of TIGR.

my $HELPTEXT = qq~
.USAGE.
  splitfasta [options] input_file.
    input_file
      the FASTA file to be split.

.DESCRIPTION.
  splitfasta - program that breaks a multi-FASTA file into separate single FASTA
  files which are named by their identifiers and have either the .seq or a user 
  defined suffix.

  Splitfasta takes in a multi-FASTA file as input and splits the FASTA file into
  multiple files, each file containing one record. The files are named by their
  identifiers followed by a suffix. If the suffix option is used, a user
  defined suffix is used, else the .seq suffix is used. If the id option is
  used, only the record which has the identifier specified in the id option is
  put into a file. Else all the records are put into separate files. When the
  verbose option is specified, the list of records which are put into files is
  printed to stdout. The program always prints the total number of records split
  to stdout.
  The program fails if the FASTA file does not parse, the command line options
  are incorrect or if the current directory is not writable.

.OPTIONS.
  -suffix<file_ext>
    specify a user defined suffix for naming the FASTA files.
 
  -id<identifier>
    specify the identifier of the FASTA file to be split.

  -verbose
    print the list of FASTA records being split.

.KEYWORDS.
  fasta, split, reads
~;


# =============================Pragmas and imports ======================
use strict;
use IO::File;
use TIGR::Foundation;
use TIGR::FASTAreader;
use TIGR::FASTArecord;

# Program stuff
my $VERSION_STRING = " Version 1.2 (Build " . (qw/$Revision$/ )[1] . ")";
my @DEPEND =
(
   "TIGR::Foundation",
   "TIGR::FASTAreader",
   "TIGR::FASTArecord",
);

MAIN:
{
   my $tf = new TIGR::Foundation;
   $tf->addDependInfo(@DEPEND);
   $tf->setVersionInfo($VERSION_STRING);
   $tf->setHelpInfo($HELPTEXT);

   my $file_ext = undef; #the suffix for the FASTA files
   my $identifier = undef;#a record identifier
   my $verbose = undef;#variable which is set if verbose messages are to be 
                       #printed
   my $input_file = "";#the FASTA file passed in.
   my $fr = undef;
   my @errors = ();
   my $extension = ".seq";
   
   $tf->logLocal("taking in the options from the command line",1);
   my $result = $tf->TIGR_GetOptions (
                                      'suffix=s',   \$file_ext,
				      'id=s',       \$identifier,
				      'verbose',    \$verbose,
				      );
   $tf->logLocal("performing the option dependent checks",1); 
   # incorrect execution command                               
   if ($result == 0) {
      $tf->bail("Command line parsing failed\n");
   }
   
   $tf->logLocal("reading the filename from the command line",1);
 
   if (defined $ARGV[0]) {
      $input_file = $ARGV[0]; #reading the FASTA filename.
      $tf->bail("Could not open \'$input_file\' ($!)") if (! -r $input_file);
   }
   else {
      $tf->bail("Supply a FASTA file\n");   
   }

   $tf->logLocal("Creating FASTAreader object",1); 
   
   $fr = new TIGR::FASTAreader($tf, \@errors, $input_file);

   if ((! defined  $fr)) {
      $tf->bail("Error creating FASTA Reader object for $input_file\n");
   }
  
   ### check if current directory is writable
   my $cwd = $tf->getProgramInfo('env_path');
   tf->bail("Cannot write to $cwd") unless (isWritableDir($cwd));


   if(defined ($file_ext)) { #if the user specified a suffix.
     $tf->logLocal("Setting the user defined suffix",2); 
     $extension = $file_ext; 
   }
   
   if(defined ($identifier)) {#if the user specified an identifier
      my $record = $fr->getRecordByIdentifier($identifier);
      $tf->logLocal("Forming the filename",2);
      my $filename = "$identifier"."$extension";
      
      if(defined $record) { 
         if ( ! open (FASTAOUT, ">$filename") ) {
            $tf->bail("Cannot open $filename for writing");
         }
         if(defined($verbose)) {
	      print("writing $identifier to $filename\n");
         }
         print FASTAOUT $record->toString();
         print("Split $input_file into 1 file\n");
      }
      else {
         $tf->bail("The record $identifier is not valid"); 
      } 
   }

   else {#if the entire file has to be split.
      my $count = 0;
      while($fr->hasNext()) {
         my $record = $fr->next();
         
         if(defined $record) {
            my $identifier = $record->getIdentifier();
	    $tf->logLocal("Forming the filename",4);
            my $filename = "$identifier"."$extension";
             
            if ( ! open (FASTAOUT, ">$filename") ) {
               $tf->bail("Cannot open $filename for writing");
            }
         
            if(defined($verbose)) {
	       print("writing $identifier to $filename\n");
            }
            $count++;
            print FASTAOUT $record->toString();
         }
      }
      print("Split $input_file into $count files\n");
   }
}   
