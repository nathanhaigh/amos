////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 03/08/2004
//!
//! \brief Dumps the reads in a bank by IID to stdout w/ clear range trimmed
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include <iostream>
#include <unistd.h>
using namespace std;
using namespace AMOS;
using namespace Bank_k;


//=============================================================== Globals ====//
string OPT_BankName;                 // bank name parameter


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
  BankStream_t red_bank (READ);
  Read_t red;

  long int cnts = 0;             // seen object count
  long int cntw = 0;             // written object count

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    red_bank . open (OPT_BankName);

    //-- Iterate through each object in the bank
    while ( red_bank >> red )
      {
	cnts ++;

	if ( red . getClearRange( ) . getLength( ) <= 0 )
	  {
	    cerr << "WARNING: read with IID " << red . getIID( )
		 << " has no clear range sequence, skipped\n";
	    continue;
	  }
	cout << ">" << red . getIID( ) << endl;
	WrapString (cout, red . getSeqString(red . getClearRange( )), 70);

	cntw ++;
      }
  }
  catch (Exception_t & e) {

  //-- On error, print debugging information
  cerr << "Objects seen: " << cnts << endl
       << "Objects written: " << cntw << endl
       << "ERROR: -- Fatal AMOS Exception --\n" << e;
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

  while ( !errflg && ((ch = getopt (argc, argv, "h")) != EOF) )
    switch (ch)
      {
      case 'h':
        PrintHelp (argv[0]);
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
    << "-h            Display help information\n\n";

  cerr
    << "Takes an AMOS bank directory and dumps all contained reads listed by\n"
    << "IID to stdout (clear range sequence only)\n\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
  return;
}
