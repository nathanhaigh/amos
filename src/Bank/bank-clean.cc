////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 01/10/2005
//!
//! \brief Cleans a bank of it's deleted records and stale file locks
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include "amp.hh"
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
using namespace AMOS;
using namespace std;




//=============================================================== Globals ====//
string  OPT_BankName;                        // bank name parameter
bool    OPT_ForceClean = false;              // force clean
bool    OPT_IsCleanCodes = false;            // clean certain codes
set<NCode_t> OPT_CleanCodes;                 // NCodes to clean



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


//----------------------------------------------------- PrintVersion -----------
//! \brief Prints version information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintVersion (const char * s);



//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  long int cntc = 0;       // banks cleand
  NCode_t ncode;           // current bank type
  BankSet_t bnks;          // all the banks

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- Output the current time and bank directory
  cerr << "START DATE: " << Date( ) << endl;
  cerr << "Bank is: " << OPT_BankName << endl;

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    //-- Iterate through each known bank and clean
    for ( BankSet_t::iterator i = bnks.begin( ); i != bnks.end( ); ++ i )
      {
        ncode = i -> getType( );

	//-- Skip if we're not looking at this one or it doesn't exist
	if ( (OPT_IsCleanCodes  &&
	      OPT_CleanCodes . find (ncode) == OPT_CleanCodes . end( ))
	     ||
             (!OPT_IsCleanCodes  &&  !i -> exists (OPT_BankName)) )
	  continue;

        cerr << "Cleaning '" << Decode(ncode) << "' bank ... ";

        //-- Try and open the bank
        try {
          if ( OPT_ForceClean )
            i -> open (OPT_BankName, B_READ | B_WRITE | B_FORCE);
          else
            i -> open (OPT_BankName, B_READ | B_WRITE);
        }
        catch (const Exception_t & e) {
          cerr << "WARNING: " << e . what( ) << endl
               << "  could not open '" << Decode (ncode)
               << "' bank, bank ignored" << endl;
          continue;
        }

        //-- Clean and close the bank
        i -> clean( );
        i -> close( );

	//-- Remove the code
	if ( OPT_IsCleanCodes )
	  OPT_CleanCodes . erase (ncode);

        cerr << "done.\n";

        cntc ++;
      }
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
	 << "  could not perform clean, abort" << endl
	 << "Banks cleaned: " << cntc << endl;
    return EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH


  //-- Any codes unrecognized?
  for ( set<NCode_t>::iterator i = OPT_CleanCodes . begin( );
	i != OPT_CleanCodes . end( ); ++ i )
    {
      cerr << "WARNING: Unrecognized bank type" << endl
	   << "  unknown bank type '" << Decode (*i)
	   << "',  bank ignored" << endl;
    }

  //-- Output the end time
  cerr << "Banks cleaned: " << cntc << endl
       << "END DATE:   " << Date( ) << endl;
  return EXIT_SUCCESS;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "b:fh")) != EOF) )
    switch (ch)
      {
      case 'b':
        OPT_BankName = optarg;
        break;

      case 'f':
        OPT_ForceClean = true;
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
      cerr << "ERROR: Bank directory is not accessible, "
	   << strerror (errno) << endl;
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
      OPT_IsCleanCodes = true;
      while ( optind != argc )
	OPT_CleanCodes . insert (Encode (argv [optind ++]));
    }
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-b path       The directory path of the bank to clean\n"
    << "-f            Forcibly clean the banks by removing all file locks\n"
    << "-h            Display help information\n"
    << endl;
  cerr
    << "Takes an AMOS bank directory as input. If no NCodes are listed on the\n"
    << "command line, all bank types will be cleaned of deleted records.\n"
    << "Otherwise, only the listed bank types will be cleaned. Cleaning the\n"
    << "deleted records may dramatically reduce the size of the bank if\n"
    << "numerous remove or replace operations have been performed. The -f\n"
    << "option will cause the program to remove any active bank locks\n"
    << "preventing the clean, but this option should be used with great\n"
    << "caution as it will interfere with any user currently accessing the\n"
    << "bank. In addition, a stale write lock may indicate that the bank\n"
    << "has been corrupted by an uncompleted write operation.\n"
    << endl;
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  -b <bank path>  [NCodes]\n\n";
  return;
}




//---------------------------------------------------------- PrintVersion ----//
void PrintVersion (const char * s)
{
  cerr << endl << s << " for bank version " << Bank_t::BANK_VERSION << endl;
  return;
}
