////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 02/20/2004
//!
//! \brief Reads an AMOS message and displays summary information
//!
//! message-count reads an AMOS message and displays the number of each top-
//! level message, along with their total and average sizes.
//!
////////////////////////////////////////////////////////////////////////////////

#include "IDMap_AMOS.hh"
#include "messages_AMOS.hh"
#include <string>
#include <fstream>
#include <cstdio>
#include <vector>
#include <unistd.h>
using namespace AMOS;
using namespace Message_k;
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
  IDMap_t typemap(1000);                   // NCode to index mapping
  ID_t ti;                                 // current type index
  NCode_t msgcode;                         // current message NCode
  streampos lastpos;                       // last tellg pos
  vector< pair<uint32_t,uint32_t> > sums (1);  // message count and size sums
  uint64_t c1, c2;

  //-- Parse the command line arguments
  ParseArgs (argc, argv);
  

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

  //-- Open the message file
  if ( OPT_MessageName . size( ) == 0 )
    msgstreamp = &cin;
  else
    {
      msgfile . open (OPT_MessageName . c_str( ));
      if ( !msgfile )
	AMOS_THROW_IO ("Could not open message file " + OPT_MessageName);
      msgstreamp = &msgfile;
    }
  lastpos = msgstreamp -> tellg( );

  //-- Parse the message file
  while ( (msgcode = msg . skip (*msgstreamp)) )
    {
      if ( typemap . exists (msgcode) )
	ti = typemap . lookup (msgcode);
      else
	{
	  ti = sums . size( );
	  typemap . insert (msgcode, ti);
	  sums . push_back (pair<uint32_t, uint32_t> (0,0));
	}

      sums [ti] . first ++;
      sums [ti] . second += msgstreamp -> tellg( ) - lastpos;

      lastpos = msgstreamp -> tellg( );
    }

  typemap . invert( );

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

  printf ("%5s %9s %12s %12s\n", "NCODE", "COUNT", "SIZE", "AVG");
  printf ("-----------------------------------------\n");
  c1 = c2 = 0;
  for ( ID_t i = 1; i < sums . size( ); i ++ )
    {
      msgcode = typemap . lookup (i);
      printf ("%5s %9ld %12ld %12ld\n",
	      Decode (msgcode) . c_str( ),
	      (long)sums [i] . first,
	      (long)sums [i] . second,
	      (long)(sums [i] . second / sums [i] . first));
      c1 += sums [i] . first;
      c2 += sums [i] . second;
    }
  printf ("-----------------------------------------\n");
  printf ("      %9ld %12ld %12ld\n", (long)c1, (long)c2, (long)(c2 / c1));

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

  if ( errflg > 0 || (optind != argc && optind != argc - 1) )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
    }

  if ( optind == argc - 1 )
    OPT_MessageName = argv [optind ++];
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-h            Display help information\n\n";

  cerr
    << "Takes an AMOS message file as input, either from the command line\n"
    << "or from stdin. All messages will be lightly checked for correct AMOS\n"
    << "format, but their NCode and fields will not be validated. Number of\n"
    << "each top-level message type will be displayed, along with their total\n"
    << "and average sizes.\n\n";
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  [message path]\n\n";
}
