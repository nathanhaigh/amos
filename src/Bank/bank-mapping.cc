////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/24/04
//!
//! \brief Reports the IDMaps for the requested bank[s]
//!
//! bank-mapping reads the IDMap from one or more AMOS banks and writes the
//! information to an AMOS message file. Columns in the map fields are
//! BID,IID,EID respectively. With 0=NULL for BID,IID and '\n'=NULL for EID.
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include <iostream>
#include <unistd.h>
using namespace std;
using namespace AMOS;




//=============================================================== Globals ====//
string  OPT_BankName;                        // bank name parameter
bool    OPT_IsExtractCodes = false;          // extract only certain NCodes
vector<NCode_t> OPT_ExtractCodes;            // NCodes to extract


//========================================================== Fuction Decs ====//
//----------------------------------------------------- ParseArgs --------------
//! \brief Sets the global OPT_% values from the command line arguments
//!
//! \return void
//!
void ParseArgs (int argc, char ** argv);


//----------------------------------------------------- PrintHelp --------------
//! \brief Prints help information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintHelp (const char * s);


//----------------------------------------------------- PrintUsage -------------
//! \brief Prints usage information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintUsage (const char * s);


//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    if ( ! OPT_IsExtractCodes )
      {
	//-- The KNOWN types to pull from the banks
	OPT_ExtractCodes . push_back (Universal_t::NCODE);
	OPT_ExtractCodes . push_back (Sequence_t::NCODE);
	OPT_ExtractCodes . push_back (Library_t::NCODE);
	OPT_ExtractCodes . push_back (Fragment_t::NCODE);
	OPT_ExtractCodes . push_back (Read_t::NCODE);
	OPT_ExtractCodes . push_back (Matepair_t::NCODE);
	OPT_ExtractCodes . push_back (Overlap_t::NCODE);
	OPT_ExtractCodes . push_back (Kmer_t::NCODE);
	OPT_ExtractCodes . push_back (Contig_t::NCODE);
	OPT_ExtractCodes . push_back (ContigLink_t::NCODE);
	OPT_ExtractCodes . push_back (ContigEdge_t::NCODE);
	OPT_ExtractCodes . push_back (Scaffold_t::NCODE);
      }

    //-- Iterate through each bank and dump its map
    vector<NCode_t>::iterator ni;
    for ( ni  = OPT_ExtractCodes . begin( );
	  ni != OPT_ExtractCodes . end( ); ++ ni )
      {
	Bank_t bank (*ni);

	try {
	  if ( bank . exists (OPT_BankName) )
	    bank . open (OPT_BankName);
          else
            continue;
        }
        catch (Exception_t & e) {
          cerr << "WARNING: " << e . what( ) << endl
               << "  could not open " << Decode (*ni)
               << " bank, mapping ignored\n";
          continue;
        }

	Message_t msg;
	bank . getIDMap( ) . writeMessage (msg);
	msg . write (cout);

	bank . close( );
      }
  }
  catch (Exception_t & e) {

  //-- On error, print debugging information
  cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  return EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH

  return EXIT_SUCCESS;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "b:h")) != EOF) )
    switch (ch)
      {
      case 'b':
        OPT_BankName = optarg;
        break;

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      default:
        errflg ++;
      }

  if ( OPT_BankName . empty( ) )
    {
      cerr << "ERROR: The -b option is mandatory\n";
      errflg ++;
    }

  if ( access (OPT_BankName . c_str( ), R_OK|W_OK|X_OK) )
    {
      cerr << "ERROR: Bank directory is not accessible\n";
      errflg ++;
    }

  if ( errflg > 0 )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
    }

  if ( optind != argc )
    {
      OPT_IsExtractCodes = true;
      while ( optind != argc )
	OPT_ExtractCodes . push_back (Encode (argv [optind ++]));
    }
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-b path       The directory path of the bank to report\n"
    << "-h            Display help information\n"
    << endl;
  cerr
    << "Takes an AMOS bank directory as input. Will output the ID map\n"
    << "information contained in the bank in the form of an AMOS message\n"
    << "to stdout. If no NCodes are listed on the command line, all known\n"
    << "object types will be reported. Output columns in the map field are\n"
    << "BID,IID,EID respectively, with 0 being NULL for BID,IID and newline\n"
    << "being NULL for EID.\n"
    << endl;
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s
    << "  [options]  -b <bank path>  [NCodes]\n\n";
  return;
}
