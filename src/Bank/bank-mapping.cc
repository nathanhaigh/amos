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
#include "amp.hh"
#include <set>
#include <iostream>
#include <unistd.h>
using namespace std;
using namespace AMOS;




//=============================================================== Globals ====//
string  OPT_BankName;                        // bank name parameter
bool    OPT_BankSpy;                         // read or read-only spy
bool    OPT_IsExtractCodes = false;          // extract only certain NCodes
set<NCode_t> OPT_ExtractCodes;               // NCodes to extract



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
  long int cntc = 0;       // mappings output
  long int cnts = 0;       // mappings checked
  NCode_t ncode;           // current bank type
  Message_t msg;           // output message
  BankSet_t bnks;          // all the banks

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- Output the current time and bank directory
  cerr << "START DATE: " << Date( ) << endl;
  cerr << "Bank is: " << OPT_BankName << endl;

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {
    ProgressDots_t dots (OPT_IsExtractCodes ?
			 OPT_ExtractCodes . size( ) : bnks . getSize( ));

    //-- Iterate through each bank and dump its map
    for ( BankSet_t::iterator i = bnks . begin( ); i != bnks . end( ); ++ i )
      {
	ncode = i -> getType( );

	//-- Skip if we're not looking at this one or it doesn't exist
	if ((OPT_IsExtractCodes  &&
	     OPT_ExtractCodes . find (ncode) == OPT_ExtractCodes . end( ))
	    ||
	    (!OPT_IsExtractCodes  &&  !i -> exists (OPT_BankName)) )
	  continue;

	//-- Update seen mappings
	dots . update (cnts ++);

	//-- Try and open the bank
	try {
	  if ( OPT_BankSpy )
	    i -> open (OPT_BankName, B_SPY);
	  else
	    i -> open (OPT_BankName, B_READ);
	}
	catch (const Exception_t & e) {
          cerr << endl
	       << "WARNING: " << e . what( ) << endl
               << "  could not open '" << Decode (ncode)
               << "' bank, mapping ignored" << endl;
          continue;
	}


	//-- Write the IDMap message to cout
	i -> getIDMap( ) . writeMessage (msg);
	msg . write (cout);


	//-- Close the bank
	i -> close( );


	//-- Remove the code
	if ( OPT_IsExtractCodes )
	  OPT_ExtractCodes . erase (ncode);

	cntc ++;
      }

    dots . end( );
  }
  catch (Exception_t & e) {
    cerr << endl
	 << "FATAL: " << e . what( ) << endl
	 << "  could not report mapping, abort" << endl
	 << "Mappings checked: " << cnts << endl
	 << "Mappings reported: " << cntc << endl;
    return EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH


  //-- Any codes unrecognized?
  for ( set<NCode_t>::iterator i = OPT_ExtractCodes . begin( );
	i != OPT_ExtractCodes . end( ); ++ i )
    {
      cnts ++;
      cerr << endl
	   << "WARNING: Unrecognized bank type" << endl
	   << "  unknown bank type '" << Decode (*i)
	   << "', mapping ignored" << endl;
    }

  //-- Output the end time
  cerr << "Mappings checked: " << cnts << endl
       << "Mappings reported: " << cntc << endl
       << "END DATE:   " << Date( ) << endl;

  return EXIT_SUCCESS;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "b:hs")) != EOF) )
    switch (ch)
      {
      case 'b':
        OPT_BankName = optarg;
        break;

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 's':
	OPT_BankSpy = true;
	break;

      default:
        errflg ++;
      }

  if ( OPT_BankName . empty( ) )
    {
      cerr << "ERROR: The -b option is mandatory" << endl;
      errflg ++;
    }

  if ( ( OPT_BankSpy  &&  access (OPT_BankName . c_str( ), R_OK|X_OK))  ||
       (!OPT_BankSpy  &&  access (OPT_BankName . c_str( ), R_OK|W_OK|X_OK)) )
    {
      cerr << "ERROR: Bank directory is not accessible, "
	   << strerror (errno) << endl;
      errflg ++;
    }

  if ( errflg > 0 )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information." << endl;
      exit (EXIT_FAILURE);
    }

  if ( optind != argc )
    {
      OPT_IsExtractCodes = true;
      while ( optind != argc )
	OPT_ExtractCodes . insert (Encode (argv [optind ++]));
    }
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-b path       The directory path of the bank to report\n"
    << "-h            Display help information\n"
    << "-s            Disregard bank locks and write permissions (spy mode)\n"
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
    << "\nUSAGE: " << s << "  [options]  -b <bank path>  [NCodes]\n"
    << endl;
  return;
}
