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

#include "messages_AMOS.hh"
#include <string>
#include <fstream>
#include <cstdio>
#include <unistd.h>
using namespace std;
using namespace AMOS;
using namespace HASHMAP;


//=============================================================== Globals ====//
string OPT_MessageName;           // message name parameter

struct LongPair_t
{
  long int first, second;
  LongPair_t( ) { first = second = 0; }
};

//========================================================== Fuction Decs ====//
long int SafeIntDiv (long int a, long int b)
{ return (long int)(b == 0 ? 0 : (double)a / (double)b); }


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
  ifstream msgfile;                        // the message file, if applicable
  Message_t msg;                           // the current message
  NCode_t msgcode = NULL_NCODE;            // current message NCode
  streampos lastpos;                       // last tellg pos
  hash_map<NCode_t, LongPair_t> sums;           // message count and size sums
  long int c1, c2;


  //-- Parse the command line arguments
  ParseArgs (argc, argv);


  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

  //-- Open the message file
  msgfile . open (OPT_MessageName . c_str( ));
  if ( !msgfile )
    AMOS_THROW_IO ("Could not open message file " + OPT_MessageName);
  lastpos = msgfile . tellg( );

  //-- Parse the message file
  while ( (msgcode = msg . skip (msgfile)) != NULL_NCODE )
    {
      sums [msgcode] . first ++;
      sums [msgcode] . second += msgfile . tellg( ) - lastpos;
      lastpos = msgfile . tellg( );
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

  printf ("%5s %9s %12s %12s\n", "NCODE", "COUNT", "SIZE", "AVG");
  printf ("-----------------------------------------\n");
  c1 = c2 = 0;
  hash_map<NCode_t, LongPair_t>::iterator mi;
  for ( mi = sums . begin( ); mi != sums . end( ); mi ++ )
    {
      printf ("%5s %9ld %12ld %12ld\n",
	      Decode (mi -> first) . c_str( ),
	      mi -> second . first,
	      mi -> second . second,
	      SafeIntDiv (mi -> second . second, mi -> second . first));
      c1 += mi -> second . first;
      c2 += mi -> second . second;
    }
  printf ("-----------------------------------------\n");
  printf ("      %9ld %12ld %12ld\n",
	  (long int)c1, (long int)c2, SafeIntDiv (c2, c1));

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
    << "Takes an AMOS message file as input on the command line. Number of\n"
    << "each top-level message type will be displayed, along with their total\n"
    << "and average sizes. All messages must have balanced braces and valid\n"
    << "headers, however their fields will not be checked for correctness.\n"
    << endl;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  [message path]\n"
    << endl;
}
