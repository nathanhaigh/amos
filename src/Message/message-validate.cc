////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 05/13/2004
//!
//! \brief Validates the format of an AMOS message.
//!
//! message-validate reads an AMOS message and returns 0 if the AMOS message
//! parsed successfully or non-zero otherwise. Does not validate links.
//!
////////////////////////////////////////////////////////////////////////////////

#include "messages_AMOS.hh"
#include <string>
#include <fstream>
#include <unistd.h>
using namespace AMOS;
using namespace std;


//=============================================================== Globals ====//
string OPT_MessageName;           // message name parameter


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
  Message_t msg;                           // the current message
  ifstream msgfile;                        // the message file, if applicable

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

  //-- Open the message file
  msgfile . open (OPT_MessageName . c_str( ));
  if ( !msgfile )
    AMOS_THROW_IO ("Could not open message file " + OPT_MessageName);

  //-- Parse the message file
  while ( msgfile . good( ) )
    msg . read (msgfile);
  }
  catch (Exception_t & e) {

  //-- On error, print debugging information
  cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  cout << "FAILURE\n";
  return EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH

  msgfile . close( );

  cout << "SUCCESS\n";
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

  OPT_MessageName = argv [optind ++];
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-h            Display help information\n"
    << endl;

  cerr
    << "Takes an AMOS message file as input on the command line. Validates\n"
    << "the format of the AMOS message and returns 0 if the message parsed\n"
    << "successfully, else returns non-zero and prints a description of the\n"
    << "failure to stderr. Links are not validated.\n"
    << endl;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  [message path]\n"
    << endl;
}
