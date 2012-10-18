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

#include "messages_AMOS.hh"
#include "utility_AMOS.hh"
#include <set>
#include <fstream>
#include <unistd.h>
using namespace AMOS;
using namespace std;


//=============================================================== Globals ====//
bool OPT_Invert = false;          // invert match
string OPT_MessageName;           // message name parameter
set<NCode_t> OPT_ExtractCodes;    // extract message type set


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
  NCode_t msgcode;                         // current message NCode
  streampos spos;                          // begin of the curr record
  char * buff = NULL;                      // record buffer
  streamsize size;                         // size of curr record
  streamsize buff_size = 0;                // size of curr record buffer
  set<NCode_t>::iterator endcode;          // end of the NCode set


  //-- Parse the command line arguments
  ParseArgs (argc, argv);
  

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

  //-- Open the message file
  msgfile . open (OPT_MessageName . c_str( ));
  if ( !msgfile )
    AMOS_THROW_IO ("Could not open message file " + OPT_MessageName);
  spos = msgfile . tellg( );

  //-- Parse the message file
  endcode = OPT_ExtractCodes . end( );
  while ( (msgcode = msg . skip (msgfile)) != NULL_NCODE )
    {
      if ( (OPT_Invert ?
            OPT_ExtractCodes . find (msgcode) == endcode :
            OPT_ExtractCodes . find (msgcode) != endcode) )
	{
	  size = msgfile . tellg( ) - spos;
	  if ( size > buff_size )
	    {
	      buff_size = size << 2;
	      buff = (char *) SafeRealloc (buff, buff_size);
	    }
	  msgfile . seekg (-size, ifstream::cur);
	  msgfile . read (buff, size);
	  cout . write (buff, size);
	}
      spos = msgfile . tellg( );
    }
  }
  catch (Exception_t & e) {

  //-- On error, print debugging information
  cerr << "Last message: " << Decode (msgcode) << " iid:";
  if ( msg . exists (F_IID) )
    cerr << msg . getField (F_IID);
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

  while ( !errflg && ((ch = getopt (argc, argv, "hm:v")) != EOF) )
    switch (ch)
      {
      case 'h':
	PrintHelp (argv[0]);
	exit (EXIT_SUCCESS);
	break;
      case 'm':
	OPT_MessageName = optarg;
	break;
      case 'v':
        OPT_Invert = true;
        break;
      default:
	errflg ++;
      }

  if ( OPT_MessageName . empty( ) )
    {
      cerr << "ERROR: The -m option is mandatory\n";
      errflg ++;
    }

  if ( errflg > 0 )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
    }

  while ( optind != argc )
    OPT_ExtractCodes . insert (Encode (argv [optind ++]));
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-h            Display help information\n"
    << "-m path       The file path of the input message\n"
    << "-v            Invert match. Filter instead of extract given NCodes\n"
    << endl;

  cerr
    << "Takes an AMOS message file and AMOS NCodes as input. All top-level\n"
    << "messages matching one of the specified NCodes will be extracted and\n"
    << "reported to stdout. All messages must have balanced braces and valid\n"
    << "headers, however their fields will not be checked for correctness.\n"
    << endl;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  -m <message path>  [NCodes]\n"
    << endl;
}
