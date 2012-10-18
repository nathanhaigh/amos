#!/usr/bin/perl -w

# Copyright (c) 2004, The Institute for Genomic Research (TIGR), Rockville,
# Maryland, U.S.A.  All rights reserved.

use strict;
use TIGR::FASTAreader;
use TIGR::FASTArecord;
use TIGR::FASTAgrammar;
use TIGR::Foundation;

our @DEPEND = ( "TIGR::Foundation", "TIGR::FASTAreader", "TIGR::FASTArecord",
                "TIGR::FASTAgrammar" );
our $VERSION = '2.00';
our $REVISION = (qw$Revision$)[-1];
our $VERSION_STRING = "$VERSION (Build $REVISION)";

my $__HELP_INFO =
q~
.USAGE.
  cutFasta [ options ] -i instruction_file fasta_file
  cutFasta [ options ] -f frag_id -x xcoord -y ycoord [ -s seq_id ] fasta_file

.DESCRIPTION.
  This program performs various excisions on FASTA records in fasta_file.  Each
  cut instruction is accompanied by a frag_id.  The frag_id becomes the sequence
  name for a new FASTA record consisting of the excised portion of the original
  record.  The new FASTA record is written to STDOUT (default) or output_file.
  A variety of cuts can be performed.  See later in this document for more
  information on the instruction_file and the cut options.

.OPTIONS.
    <fasta_file>
      This is the FASTA input file.  This file may contain either one sequence
      or many sequences.

   -f <frag_id>
      This option specifies a fragment id to use for a new FASTA record cut
      described by the -x and -y parameters.  This frag_id becomes the header
      of the new FASTA record.  Note, this parameter cannot be used with -i
      or without -x.

   -x <xcoord>
      Indicate a starting cut coordinate.  See later in this document regarding
      possible values for xcoord.  This parameter is used in conjunction with
      -f, and cannot be used with -i.

   -y <ycoord>
      Indicate a stopping cut coordinate.  See later in this document regarding
      possible values for ycoord.  This parameter is used in conjunction with
      -f and -x and cannot be used with -i.

   -s <seq_id>
      Indicate a template sequence for cutting.  This is sequence on which
      cuts are to be performed.  This parameter can be omitted ONLY IF
      there is only one sequence in fasta_file.  Note, a unique substring
      is sufficient for specifying any sequence.

   -i <instruction_file>
      Input a series of instructions (frag_id, xcoord, ycoord, seq_id sets)
      from a file.  This parameter cannot be used with -f, -x, -y, and -s.

   -0
      Indicates that all coordinates supplied, either by file or by argument
      or echoed in, are '0' based (starts at 0).  Default is '1' based 
      (starts at 1).

   -o <output_file>
      Indicate a file to receive the new fragments.  By default, output goes
      to STDOUT.

   The TIGR Foundation standard options (-h, -debug, -depend, etc.) are
   also supported.

   COORDINATE VALUES
   xcoord and ycoord can be specified in one of the following ways per record.

  1. an end5 and end3; if end5 is greater than end3, the resulting sequence 
  is reverse complemented.  Note, end5 and/or end3 can be negative, indicating
  a distance from the end of the sequence.  Whenever negative coordinates
  are given, they are converted to their positive counterparts before determining
  if the read is reversed: -100 to -50 would not be reverse complemented, whereas
  -50 to -100 would be reverse complemented.

  Examples: 'test 1 20', 'frag end 23', 'frag2 2311 -3', 'frag3 -2 12'

  2. an end5 and a length; the length must be prefixed with a '+'.  In this
  case as well, the end5 coordinate can be negative.
  Example: 'test 345043 +2000'

  INSTRUCTIONS FILE (and standard input)

  Input instructions must be supplied one record per line.  The instructions can
  be specified in the instruction file or echoed in (given on standard input).
  Please see earlier discussions for valid values for xcoord and ycoord.

  <frag_id> <xcoord> <ycoord> <seq_id>

  seq_id can be omitted if there is only one record in fasta_file.
  An example would be the following

  fragment1 1 10 12311

  .. extracts bases 1 through 10 of sequence named 12311 and outputs those
  10 bases to a FASTA record named fragment1.

  CONSTRAINT
  To maintain compliance of cutFasta output with SOP I005, cutFasta does not
  allow the use of a fragment id (frag_id) more than once for an output.
  If a fragment id is encountered more than once in the input, a warning is
  issued each time and a new name is assigned.  The new name is reported in
  the warning, along with the number of the fragment instance.

  The new fragment id will model <original_frag_id>__<xcoord>_<ycoord>_<seq_id> .
  If this new fragment id is not unique, an error is thrown and the fragment
  is skipped.

  EXAMPLE
  The above instruction example as applied to command line arguments would be
  as follows.  Output goes to standard output.  We take 12311 from my.fasta.

  cutFasta -f fragment1 -x 1 -y 10 -s 12311 my.fasta

  KNOWN ISSUES
  Large FASTA input files may fail on small memory machines.

  CONTACT
  Please forward all questions and comments to the Software Engineering team at
  bits.se@tigr.org.

  Daniel Kosack
  10/13/2004

.KEYWORDS.
  fasta
~;

my $__USAGE_INFO = 
q~cutFasta -x xcoord -y ycoord -f frag_id [ -s seq_id ] [ options ] fastafile
cutFasta -i instruction_file [ options ] fastafile
echo 'frag_id xcoord ycoord [ seq_id ]' | cutFasta [ options ] fastafile
-0         Indicate that coordinates are '0' based.  Default is '1' based.
-o fname   Specify an output file.  DEFAULT: STDOUT.
~;

# Set up TIGR Foundation.
my $tf_obj = new TIGR::Foundation;
$tf_obj->addDependInfo(@DEPEND);
$tf_obj->setVersionInfo($VERSION_STRING);
$tf_obj->setHelpInfo($__HELP_INFO);
$tf_obj->setUsageInfo($__USAGE_INFO);

# debugging scheme
#
#   Debugging via the TIGR Foundation uses increasing log levels based on
#   nesting.  'MAIN' starts at level 1.  Every nest increments the level by 1.
#   Subroutines always start nesting at level 2.  As debugging levels
#   increase, logging is more verbose.  This makes sense as you log at
#   greater depth (ie. deeper branching).


# Global variables.
my @FASTA_headers = ();                         # Store record headers here.
my %FASTA_idx = ();                             # Cache record indices.


## Functions


# This function processes the command line invocation, filling out the 
# appropriate values of the parameter hash (a reference to which is passed
# in).  It also checks that there are no input mode clashes.  On success,
# this function returns 1 (true).  On failure, it returns undef.
sub parseCommandLine($) {
   my $rh_parms = shift;
   my $modes_found = 0;                         # Check for the input modes. 
   my $instruction_file = undef;                # tmp. instruction file
   $tf_obj->logLocal("Now processing in \'parseCommandLine\'.", 2);
   my $result = $tf_obj->TIGR_GetOptions(
                                          "f=s" => \$rh_parms->{'frag_id'},
                                          "x=s" => \$rh_parms->{'xcoord'},
                                          "y=s" => \$rh_parms->{'ycoord'},
                                          "s=s" => \$rh_parms->{'seq_id'},
                                          "i=s" => \$instruction_file,
                                          "o=s" => \$rh_parms->{'output_file'},
                                          "0" => \$rh_parms->{'base_zero'},
                                        );
   if ( ! $result ) {
      $tf_obj->logError("Command line parsing failed!");
      $tf_obj->printUsageInfoAndExit();
   }
   $rh_parms->{'input_file'} = shift @ARGV;
   if ( ! defined ( $rh_parms->{'input_file'} ) ) {
#      $tf_obj->logError("An input FASTA file is not specified!", 3);
      $tf_obj->printUsageInfoAndExit();
   }
   # Check input modes.
   if ( defined ( $rh_parms->{'frag_id'} ) ||
        defined ( $rh_parms->{'xcoord'} ) || 
        defined ( $rh_parms->{'ycoord'} ) ||
        defined ( $rh_parms->{'seq_id'} ) ) {
      # Any of the above indicate a switch based input method.
      $modes_found++;
      # Verify that the fragment id and x-coordinate are specified.
      if ( ! defined ( $rh_parms->{'frag_id'} ) || 
           ! defined ( $rh_parms->{'xcoord'} ) ||
           ! defined ( $rh_parms->{'ycoord'} ) ) {
         $tf_obj->logError("Fragment id or x, y coordinates not specified!",4);
         $tf_obj->printUsageInfoAndExit();
      }
   }
   if ( defined ( $instruction_file ) ) {
      # We have an instruction file based input method.
      $modes_found++;
      $rh_parms->{'ins_file'} = $instruction_file;
   }
   # Check that only one input mode is specified.
   if ( $modes_found > 1 ) {
      $tf_obj->logError("Specified input mode is invalid!", 3);
      $tf_obj->printUsageInfoAndExit();
   }
   $tf_obj->logLocal("Finished processing command line inputs.", 2);
   $tf_obj->logLocal("Now leaving \'parseCommandLine\'.", 2);
   return 1;
}


# This function catalogs the FASTA records for regular expression parsing.
# It takes a reference to a FASTAreader object.  On success, it returns
# 1 (true).  On failure, it returns undef (false).
sub catalogFASTArecords($) {
   my $f_obj = shift;
   $tf_obj->logLocal("Now processing in \'catalogFASTArecords\'.", 2);
   while ( defined ( my $rec = $f_obj->next() ) ) {
      $tf_obj->logLocal("Got record \'" . $rec->getIdentifier() . "\'.", 3);
      my $header = $rec->getHeader();
      push @FASTA_headers, $header;
      $tf_obj->logLocal("Logged header \'$header\'.", 3);
   }
   $tf_obj->logLocal("Finished cataloging FASTA headers.", 2);
   $tf_obj->logLocal("Now leaving \'catalogFASTArecords\'.", 2);
   return 1;
}


# This function takes a FASTAreader object and an identifier.  It returns
# the according FASTA record.  It does a regular expression search for
# the identifier against the FASTA record set.  If there is no match, this
# function returns undef - otherwise the record is returned.
sub findFASTArecord($$) {
   my $f_obj = shift;
   my $str_id = shift;
   my $record = undef;
   $tf_obj->logLocal("Now processing in \'findFASTArecord\'.", 2);
   if ( ! defined ( $str_id ) ) {
      $tf_obj->logError("An internal error has occurred; the search id is " .
         "not passed!", 3);
      return undef;
   }
   # Check to see if the string id is a real identifier.
   if ( defined ( $f_obj->seekIdentifier($str_id) ) ) {
      $tf_obj->logLocal("Found \'$str_id\' in FASTA store.", 2);
   }
   elsif ( defined ( $FASTA_idx{$str_id} ) &&
           ( $FASTA_idx{$str_id} != -1 ) ) {
      # Check the cache to see if the id has already been resolved.
      $tf_obj->logLocal("Found a cache entry for \'$str_id\'.", 3);
      $f_obj->seekIndex($FASTA_idx{$str_id});
   }
   elsif ( ! defined ( $FASTA_idx{$str_id} ) ) {
      # We search for the record.
      my $num_matches = 0;
      my $match_header = undef;
      foreach my $header ( @FASTA_headers ) {
         if ( index($header, $str_id) != -1 ) {
            # We have a match!
            $tf_obj->logLocal("Id \'$str_id\' matches header \'$header\'.", 5);
            $num_matches++;
            $match_header = $header;
         }
      }
      if ( $num_matches > 1 ) {
         $tf_obj->logError("Sequence id \'$str_id\' is ambiguous.", 4);
         $FASTA_idx{$str_id} = -1;
         return undef;
      }
      if ( $num_matches == 0 ) {
         $tf_obj->logError("Sequence id \'$str_id\' not found.", 4);
         $FASTA_idx{$str_id} = -1;
         return undef;
      }
      $tf_obj->logLocal("Sequence id \'$str_id\' maps to \'$match_header\'.",3);
      # Store off index of sequence.
      my $identifier = TIGR::FASTAgrammar::_headerToIdentifier($match_header);
      if ( ! defined ( $identifier ) ) {
         $tf_obj->logError("Cannot get identifier for \'$match_header\'.", 4);
         return undef;
      }
      if ( ! defined ( $f_obj->seekIdentifier($identifier) ) ) {
         $tf_obj->logError("Cannot find record for id \'$identifier\'.", 4);
         return undef;
      }
      my $idx = $f_obj->index();
      $FASTA_idx{$str_id} = $idx;               # Cache the lookup string.
   }
   $record = $f_obj->get();
   if ( ! defined ( $record ) ) {
      $tf_obj->logError("Failed to return record object for \'$str_id\'!", 3);
      return undef;
   }
   $tf_obj->logLocal("Retrieved record for \'$str_id\'.", 2);
   $tf_obj->logLocal("Now leaving \'findFASTArecord\'.", 2);
   return $record;
}


# This function converts any of the possible inputs into one unified input
# for further processing.  That is, whether coordinates are passed as offsets
# or as end5-end3, one output results.  See the parameter list for inputs.
# This method returns results as output parameters.  Return values are 1 on
# success, undef on failure.  Outputs are given as 0 based offset / length
# coordinates with a strand indicated.
sub convertCoordinates($$$$$$$) {
   my $xcoord = shift;
   my $ycoord = shift;
   my $base_zero = shift;                       # 0 based coords? 0->no, 1->yes
   my $seq_len = shift;                         # length of substrate sequence
   my $start = shift;                           # output parameter
   my $frag_len = shift;                        # output parameter
   my $strand = shift;                          # 0 => forward, 1 => reverse.
   $tf_obj->logLocal("Now processing in \'convertCoordinates\'.", 2);
   # Check for proper variable definition.
   if ( ! defined ( $xcoord ) ) {
      $tf_obj->logError("No X coordinate is specified for the record!", 3);
      return undef;
   }
   if ( ! defined ( $ycoord ) ) {
      $tf_obj->logError("No Y coordinate is specified for the record!", 3);
      return undef;
   }
   # Store original values.
   my $xcoord_orig = $xcoord;
   my $ycoord_orig = $ycoord;
   # Convert inputs into something usable.
   # First, check for definition of 'end' or '-' in either x or y coordinates.
   if ( ( $xcoord eq 'end' ) || ( $xcoord eq '-' ) ) {
      $xcoord = $seq_len;
      if ( $base_zero ) {
         $xcoord--;
      }
   }
   if ( ( $ycoord eq 'end' ) || ( $ycoord eq '-' ) ) {
      $ycoord = $seq_len;
      if ( $base_zero ) {
         $ycoord--;
      }
   }
   # Now, check to see if the inputs are greater than (or equal to) zero.
   if ( $xcoord =~ /^-\d+$/ ) {
      $xcoord += $seq_len;
      if ( ! $base_zero ) {
         $xcoord++;
      }
   }
   if ( $ycoord =~ /^-\d+$/ ) {
      $ycoord += $seq_len;
      if ( ! $base_zero ) {
         $ycoord++;
      }
   }   
   # Check cases.
   if ( ( $xcoord =~ /^\d+$/ ) && ( $ycoord =~ /^\d+$/ ) ) {
      # We have an end5 and end3 scheme.
      $tf_obj->logLocal("End5/end3 use case: X=\'$xcoord\', Y=\'$ycoord\'.", 3);
      if ( ! $base_zero ) {
         $xcoord--;
         $ycoord--;
      }
      if ( $xcoord > $ycoord ) {
         # Flip as needed.
         my $tmp = $xcoord;
         $xcoord = $ycoord;
         $ycoord = $tmp;
         ${$strand} = 1;
      }
      else { 
         ${$strand} = 0;
      }
      ${$start} = $xcoord;
      ${$frag_len} = ( $ycoord - $xcoord ) + 1; 
   } 
   elsif ( ( $xcoord =~ /^\d+$/ ) && ( $ycoord =~ /^\+\d+$/ ) ) {
      # We have a start coordinate and a length.
      $tf_obj->logLocal("X plus Y offset: X=\'$xcoord\', Y=\'$ycoord\'.", 3);
      if ( ! $base_zero ) {
         $xcoord--;
      }
      ${$start} = $xcoord;
      ( ${$frag_len} ) = $ycoord =~ /^\+(\d+)$/;
      ${$strand} = 0;
   }
   else {
      $tf_obj->logError("Unknown use case: X=\'$xcoord_orig\', " .
                                          "Y=\'$ycoord_orig\'.", 3);
      return undef;
   }
   # Check coordinates for sanity.
   $tf_obj->logLocal("Start is \'${$start}\', length is \'${$frag_len}\' and " .
      "strand is \'${$strand}\'.", 2);
   if ( ${$start} < 0 ) {
      $tf_obj->logError("Start value \'${$start}\' is not allowed.", 3);
      return undef;
   }
   if ( ( ${$start} + ${$frag_len} ) > $seq_len ) {
      $tf_obj->logError("Fragment length \'${$frag_len}\' is too large.", 3);
      return undef;
   }
   $tf_obj->logLocal("Now leaving \'convertCoordinates\'.", 2);
   return 1;
}


# This function constructs a fragment FASTA record given the cut information
# and the parameter hash.  It returns the fragment FASTA record on
# success, and undef on failure.
sub makeFragment($$$$$) {
   my $rh_parms = shift;
   my $frag_id = shift;
   my $seq_id = shift;
   my $xcoord = shift;
   my $ycoord = shift;
   $tf_obj->logLocal("Now processing in \'makeFragment\'.", 2);
   # Check if the fragment is already used.
   if ( defined ( $rh_parms->{'frag_store'}->{$frag_id} ) ) {
      # The fragment appears multiple times.  Construct a new name.
      my $new_frag_id = $frag_id . "__" . $xcoord . "_" . $ycoord . "_" .
                        $seq_id;
      $rh_parms->{'frag_store'}->{$frag_id}++;
      my $instance = $rh_parms->{'frag_store'}->{$frag_id};
      $tf_obj->logError("Fragment \'$frag_id\' appears multiple times in " .
         "the input.  Using id \'$new_frag_id\' for instance $instance.", 3);
      if ( defined ( $rh_parms->{'frag_store'}->{$new_frag_id} ) ) {
         $tf_obj->logError("New fragment id \'$new_frag_id\' also used.  " .
            "Skipping.", 4);
         return undef;
      }
      $frag_id = $new_frag_id;
   }
   my $f_obj = $rh_parms->{'fasta_obj'};
   # Extract the substrate record object.
   my $subst_obj = findFASTArecord($f_obj, $seq_id);
   if ( ! defined ( $subst_obj ) ) {
      $tf_obj->logLocal("No record returned for \'$seq_id\'.", 3);
      return undef;
   }
   my $strand = undef;
   my $start = undef;
   my $frag_len = undef;
   if ( ! defined ( convertCoordinates($xcoord,$ycoord,$rh_parms->{'base_zero'},
                    $subst_obj->size(), \$start, \$frag_len, \$strand) ) ) {
      $tf_obj->logError("Cannot get coordinates for fragment \'$frag_id\'.", 3);
      return undef;
   }
   my $seq_data = $subst_obj->subSequence($start, $frag_len);
   if ( $strand == 1 ) {
      # We need to reverse complement.
      my $tmp_rec = new TIGR::FASTArecord($frag_id, $seq_data);
      $seq_data = $tmp_rec->reverseComplementData();
      if ( ! defined ( $seq_data ) ) {
         $tf_obj->logError("Fragment \'$frag_id\' cannot be reverse " .
            "complemented.", 4);
         return undef;
      }
   }
   # Create the result record.
   my $fasta_rec = new TIGR::FASTArecord($frag_id, $seq_data);
   if ( ! defined ( $fasta_rec ) ) {
      $tf_obj->logError("Cannot create FASTA record for \'$frag_id\'!", 3);
      return undef;
   }
   $tf_obj->logLocal("Created FASTA record for \'$frag_id\'.", 2);
   if ( ! defined ( $rh_parms->{'frag_store'}->{$frag_id} ) ) {
      $rh_parms->{'frag_store'}->{$frag_id} = 0;
   }
   $rh_parms->{'frag_store'}->{$frag_id}++;
   $tf_obj->logLocal("Now returning from \'makeFragment\'.", 2);
   return $fasta_rec;
}


MAIN: {
   my %ParmHash = (
                    "frag_id" => undef,
                    "frag_store" => {},
                    "xcoord" => undef,
                    "ycoord" => undef,
                    "ins_file" => "-",
                    "input_file" => undef,
                    "output_file" => "&STDOUT",
                    "base_zero" => undef,
                    "seq_id" => undef,
                    "unique" => undef,
                    "fasta_obj" => undef,
                  );
   my $default_seq_id = undef;
   my $err_flag = 0;

   # Parse the command line.
   if ( ! defined ( parseCommandLine(\%ParmHash) ) ) {
      exit 1;
   }
   # Open the output stream.
   if ( ! open ( OUTPUT, ">" . $ParmHash{'output_file'} ) ) {
      $tf_obj->bail("Cannot open output stream \'$ParmHash{'output_file'}\'.");
   }
   # Open the FASTA reader.
   $ParmHash{'fasta_obj'} = new TIGR::FASTAreader $tf_obj, 
                                                  $ParmHash{'input_file'};
   if ( ! defined ( $ParmHash{'fasta_obj'} ) ) {
      $tf_obj->bail("Cannot open FASTA file \'$ParmHash{'input_file'}\'.");
   }
   my $num_recs = $ParmHash{'fasta_obj'}->count();
   if ( $num_recs == 1 ) {
      $tf_obj->logLocal("Only one FASTA record, setting default sequence.", 2);
      my $seq_rec = $ParmHash{'fasta_obj'}->next();
      $default_seq_id = $seq_rec->getIdentifier();
      $ParmHash{'fasta_obj'}->seekIndex(-1);    # Return to the beginning.
   }
   # Catalog the FASTA file.
   if ( ! defined ( catalogFASTArecords($ParmHash{'fasta_obj'}) ) )  {
      $tf_obj->bail("Cannot catalog FASTA records!");
   }
   # Process the instructions.
   # First, look for the switch based input.
   if ( defined ( $ParmHash{'frag_id'} ) ) {
      my $seq_id = $ParmHash{'seq_id'};
      if ( ! defined ( $seq_id ) ) {
         $seq_id = $default_seq_id;
      }
      if ( ! defined ( $seq_id ) ) {
         # Still no sequence id...
         $tf_obj->bail("No sequence specified in input!  MultiFASTA (or " .
           "empty file) found.");
      }
      my $record = makeFragment(\%ParmHash, $ParmHash{'frag_id'}, $seq_id,
                                $ParmHash{'xcoord'}, $ParmHash{'ycoord'});
      if ( ! defined ( $record ) ) {
         $err_flag = 1;
      }
      else {
         print OUTPUT $record->toString();
      }
   }
   # Otherwise, go to a file.
   elsif ( defined ( $ParmHash{'ins_file'} ) ) {
      if ( ! open (INPUT, $ParmHash{'ins_file'}) ) {
         $tf_obj->bail("Cannot open input file \'$ParmHash{'ins_file'}\'.");
      }
      # Iterate through the instructions.
      while ( defined ( my $line = <INPUT> ) ) {
         chomp $line;
         if ( $line =~ /^\s*$/ || $line =~ /^\s*#/ ) {
            next;
         }
         # Trim all trailing ws.
         $line =~ s/\s*$//g;
         $tf_obj->logLocal("Got line \'$line\'.", 3);
         my ( $frag_id, $xcoord, $ycoord, $seq_id ) = split /\s+/, $line;
         if ( ! defined ( $frag_id ) || 
              ! defined ( $xcoord ) ||
              ! defined ( $ycoord ) ) {
            $tf_obj->logError("Improper input line \'$line\'.", 4);
            $err_flag = 1;
            next;
         }
         $tf_obj->logLocal("Frag id \'$frag_id\'.", 3);
         $tf_obj->logLocal("X \'$xcoord\'.", 3);
         $tf_obj->logLocal("Y \'$ycoord\'.", 3);
         if ( defined ( $seq_id ) ) {
            $tf_obj->logLocal("SeqId \'$seq_id\'.", 3);
         }
         elsif ( defined ( $default_seq_id ) ) {
            $tf_obj->logLocal("SeqId \'<undefined>', " .
               "using \'$default_seq_id.", 3);
            $seq_id = $default_seq_id;
         }
         else { # ! defined $default_seq_id
            $tf_obj->logError("No sequence specified in input!  MultiFASTA " .
               "(or empty file) found.", 3);
            next;
         }
         my $record = makeFragment(\%ParmHash, $frag_id, $seq_id, $xcoord,
                                   $ycoord);
         if ( ! defined ( $record ) ) {
            $err_flag = 1;
         }
         else {
            print OUTPUT $record->toString();
         }
      }
      close(INPUT);
   }
   # Close the output stream.
   if ( ! close(OUTPUT) ) {
      $tf_obj->bail("Cannot flush the output stream!");
   }
   if ( ! $err_flag ) {
      $tf_obj->logLocal("Program exiting successfully.", 2);
   }
   else {
      $tf_obj->logError("There are errors.", 2);
   }
   exit $err_flag;
}
