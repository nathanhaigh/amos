////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 03/08/2004
//!
//! \brief Dumps a bambus .mates file from an AMOS bank
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include <iostream>
#include <cassert>
#include <unistd.h>
using namespace std;
using namespace AMOS;


//=============================================================== Globals ====//
string OPT_BankName;                 // bank name parameter
bool   OPT_BankSpy = false;          // read or read-only spy



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
  int exitcode = EXIT_SUCCESS;
  Fragment_t frg;
  Library_t lib;
  pair<ID_t, ID_t> mtp;

  BankStream_t frg_bank (Fragment_t::NCODE);
  BankStream_t lib_bank (Library_t::NCODE);

  long int cntw = 0;             // written object count

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    BankMode_t bm = OPT_BankSpy ? B_SPY : B_READ;
    frg_bank . open (OPT_BankName, bm);
    lib_bank . open (OPT_BankName, bm);

    //-- Iterate through each library in the bank
    while ( lib_bank >> lib )
      {
	cout << "library\t" << lib . getIID( )
             << '\t'
	     << lib.getDistribution( ).mean - (lib.getDistribution( ).sd * 3)
             << '\t'
	     << lib.getDistribution( ).mean + (lib.getDistribution( ).sd * 3)
             << endl;
      }

    //-- Iterate through each fragment in the bank
    while ( frg_bank >> frg )
      {
        mtp = frg . getMatePair( );
        if ( mtp . first != NULL_ID && mtp . second != NULL_ID )
          {
            cout << mtp . first << '\t' << mtp . second << '\t'
                 << frg . getLibrary( )
                 << endl;
            cntw ++;
          }
      }

    frg_bank . close( );
    lib_bank . close( );
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH


  cerr << "Objects written: " << cntw << endl;

  return exitcode;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "hsv")) != EOF) )
    switch (ch)
      {
      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 's':
	OPT_BankSpy = true;
	break;

      case 'v':
	PrintBankVersion (argv[0]);
	exit (EXIT_SUCCESS);
	break;

      default:
        errflg ++;
      }

  if ( errflg > 0 || optind != argc - 1 )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
    }

  OPT_BankName = argv [optind ++];
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-h            Display help information\n"
    << "-s            Disregard bank locks and write permissions (spy mode)\n"
    << "-v            Display the compatible bank version\n"
    << endl;
  cerr
    << "Takes an AMOS bank directory and dumps a bambus .mates files to\n"
    << "stdout\n\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
  return;
}
