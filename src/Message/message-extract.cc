////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 02/25/2004
//!
//! \brief Reads an AMOS message and extracts the requested message types
//!
//! message-extract reads an AMOS message and extracts the requested top-level
//! message types to stdout.
//!
////////////////////////////////////////////////////////////////////////////////

#include "IDMap_AMOS.hh"
#include "messages_AMOS.hh"
#include "alloc.hh"
#include <fstream>
#include <unistd.h>
using namespace AMOS;
using namespace Message_k;
using namespace std;


//=============================================================== Globals ====//
string OPT_MessageName;           // message name parameter
IDMap_t OPT_ExtractCodes(1000);   // extract message type map


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


//----------------------------------------------------- PringUsage -------------
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
  istream * msgstreamp;                    // the message file stream
  ifstream msgfile;                        // the message file, if applicable
  NCode_t msgcode;                         // current message NCode
  streampos spos;                          // begin of the curr record
  char * buff = NULL;                      // record buffer
  streamsize size;                         // size of curr record
  streamsize buff_size = 0;                // size of curr record buffer


  //-- Parse the command line arguments
  ParseArgs (argc, argv);
  

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

  //-- Open the message file
  if ( OPT_MessageName . empty( ) )
    msgstreamp = &cin;
  else
    {
      msgfile . open (OPT_MessageName . c_str( ));
      if ( !msgfile )
	AMOS_THROW_IO ("Could not open message file " + OPT_MessageName);
      msgstreamp = &msgfile;
    }


  //-- Parse the message file
  while ( msgstreamp -> good( ) )
    {
      while ( msgstreamp -> get( ) != '{' )
	if ( !msgstreamp -> good( ) )
	  break;
      msgstreamp -> putback ('{');

      spos = msgstreamp -> tellg( );
      msgcode = msg . skip (*msgstreamp);
      if ( OPT_ExtractCodes . exists (msgcode) )
	{
	  size = msgstreamp -> tellg( ) - spos;
	  if ( size > buff_size )
	    buff = (char *) SafeRealloc (buff, size);
	  msgstreamp -> seekg (-size, ifstream::cur);
	  msgstreamp -> read (buff, size);
	  cout . write (buff, size);
	}
    }

  }
  catch (Exception_t & e) {

  //-- On error, print debugging information
  cerr << "Last message: " << Decode (msgcode) << " eid:";
  if ( msg . exists (F_EID) )
    cerr << msg . getField (F_EID);
  cerr << "\nERROR: -- Fatal AMOS Exception --\n" << e;
  return EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH

  msgfile . close( );


  return EXIT_SUCCESS;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "hm:")) != EOF) )
    switch (ch)
      {
      case 'h':
	PrintHelp (argv[0]);
	exit (EXIT_SUCCESS);
	break;
      case 'm':
	OPT_MessageName = optarg;
	break;
      default:
	errflg ++;
      }

  if ( errflg > 0 )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
    }

  while ( optind != argc )
    {
      try {

	OPT_ExtractCodes . insert (Encode (argv [optind ++]), 1);
      }
      catch (Exception_t & e) {

	cerr << "WARNING: " << e . what( )
	     << " - NCode " << argv [optind - 1] << " ignored" << endl;
      }
    }
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-h            Display help information\n"
    << "-m path       The file path of the input message\n\n";

  cerr
    << "Takes an AMOS message file as input, either from the -m option or\n"
    << "from stdin. All top-level messages matching one of the NCodes\n"
    << "specified on the command line will be extracted and reported to\n"
    << "stdout. Each message will be lightly checked for correct AMOS\n"
    << "format, but their NCode and fields will not be validated.\n\n";
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  [NCodes]\n\n";
}
