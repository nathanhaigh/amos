////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 01/07/2004
//!
//! \brief Reports the data contained in a bank to a message file
//!
//! bank-report reads the assembly data from an AMOS bank and writes it to an
//! AMOS message file. Will only extract the data of the objects it recognizes.
//! All links reference IIDs.
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
string  OPT_IIDExtractName;                  // extract by IID file
string  OPT_EIDExtractName;                  // extract by EID file
bool    OPT_IsBIDs = false;                  // include BIDs in the output
bool    OPT_IsExtractCodes = false;          // extract only certain NCodes
bool    OPT_IsExtractIDs = false;            // extract only certain IDs
bool    OPT_BankSpy = false;                 // read or read-only spy
set<NCode_t> OPT_ExtractCodes;               // NCodes to extract
vector<ID_t> OPT_ExtractIIDs;                // IIDs to extract
vector<string> OPT_ExtractEIDs;              // EIDs to extract



//========================================================== Fuction Decs ====//
//----------------------------------------------------- ParseExtract -----------
//! \brief Parses the extract file and loads the appropriate extract maps
//!
//! \return void
//!
void ParseExtract ( );


//----------------------------------------------------- PrintObject ------------
//! \brief Prints the object message to stdout
//!
//! \return void
//!
void PrintObject (const Universal_t * obj, ID_t bid);


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
  long int cntc = 0;       // objects reported
  NCode_t ncode;           // current object type
  UniversalSet_t objs;     // all the universal objects
  ID_t bid;                // bank index

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- Output the current time and bank directory
  cerr << "START DATE: " << Date( ) << endl;
  cerr << "Bank is: " << OPT_BankName << endl;

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    //-- Iterate through each known object and dump if its got bank
    for ( UniversalSet_t::iterator i = objs.begin( ); i != objs.end( ); ++ i )
      {
	ncode = i -> getNCode( );

	//-- Skip if we're not looking at this one or it doesn't exist
	if ( (OPT_IsExtractCodes  &&
	      OPT_ExtractCodes . find (ncode) == OPT_ExtractCodes . end( ))
	     ||
	     (!OPT_IsExtractCodes  &&  !BankExists (ncode, OPT_BankName)) )
	  continue;


	if ( OPT_IsExtractIDs )
	  {
	    Bank_t bank (ncode);

	    //-- Try and open the bank
	    try {
	      if ( OPT_BankSpy )
		bank . open (OPT_BankName, B_SPY);
	      else
		bank . open (OPT_BankName, B_READ);
	    }
	    catch (const Exception_t & e) {
	      cerr << "WARNING: " << e . what( ) << endl
		   << "  could not open '" << Decode (ncode)
		   << "' bank, all objects ignored" << endl;
	      continue;
	    }

	    //-- Get all of the requested IIDs
	    for ( vector<ID_t>::iterator ii = OPT_ExtractIIDs . begin( );
		  ii != OPT_ExtractIIDs . end( ); ++ ii )
	      {
		bid = bank . getIDMap( ) . lookupBID (*ii);
		if ( bid == NULL_ID )
		  continue;
		bank . fetch (*ii, *i);
		PrintObject (i, bid);
		cntc ++;
	      }

	    //-- Get all of the requested EIDs
	    for ( vector<string>::iterator ei = OPT_ExtractEIDs . begin( );
		  ei != OPT_ExtractEIDs . end( ); ++ ei )
	      {
		bid = bank . getIDMap( ) . lookupBID (ei -> c_str( ));
		if ( bid == NULL_ID )
		  continue;
		bank . fetch (ei -> c_str( ), *i);
		PrintObject (i, bid);
		cntc ++;
	      }

	    bank . close( );
	  }
	else
	  {
	    BankStream_t bankstream (ncode);
	    
	    //-- Open the bankstream if it exists
	    try {
	      if ( OPT_BankSpy )
		bankstream . open (OPT_BankName, B_SPY);
	      else
		bankstream . open (OPT_BankName, B_READ);
	    }
	    catch (const Exception_t & e) {
	      cerr << "WARNING: " << e . what( ) << endl
		   << "  could not open '" << Decode (ncode)
		   << "' bankstream, all objects ignored" << endl;
	      continue;
	    }
	    
	    //-- Get ALL of the objects
	    while ( bankstream >> *i )
	      {
		PrintObject (i, bankstream . tellg( ) - 1);
		cntc ++;
	      }

	    bankstream . close( );
	  }

	//-- Remove the code
	if ( OPT_IsExtractCodes )
	  OPT_ExtractCodes . erase (ncode);
      }
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
	 << "  could not perform report, abort" << endl
	 << "Objects reported: " << cntc << endl;
    return EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH


  //-- Any codes unrecognized?
  for ( set<NCode_t>::iterator i = OPT_ExtractCodes . begin( );
	i != OPT_ExtractCodes . end( ); ++ i )
    {
      cerr << "WARNING: Unrecognized bank type" << endl
	   << "  unknown bank type '" << Decode (*i)
	   << "',  bank ignored" << endl;
    }

  //-- Output the end time
  cerr << "Objects reported: " << cntc << endl
       << "END DATE:   " << Date( ) << endl;
  return EXIT_SUCCESS;
}




//----------------------------------------------------- ParseExtract -----------
void ParseExtract ( )
{
  ifstream infile;
  string eid;
  ID_t iid;

  if ( ! OPT_IIDExtractName . empty( ) )
    {
      infile . open (OPT_IIDExtractName . c_str( ));
      if ( !infile )
	cerr << "WARNING: Could not open " << OPT_IIDExtractName << endl;

      infile >> iid;
      while ( infile )
	{
	  OPT_ExtractIIDs . push_back (iid);
	  infile >> iid;
	}

      infile . close( );
    }

  if ( ! OPT_EIDExtractName . empty( ) )
    {
      infile . open (OPT_EIDExtractName . c_str( ));
      if ( !infile )
	cerr << "WARNING: Could not open " << OPT_EIDExtractName << endl;

      getline (infile, eid);
      while ( infile )
	{
	  OPT_ExtractEIDs . push_back (eid);
	  getline (infile, eid);
	}

      infile . close( );
    }
}




//----------------------------------------------------- PrintObject ------------
void PrintObject (const Universal_t * obj, ID_t bid)
{
  Message_t msg;
  ostringstream ss;

  obj -> writeMessage (msg);

  if ( OPT_IsBIDs )
    {
      ss << bid;
      msg . setField (F_BID, ss . str( ));
    }

  msg . write (cout);
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "b:BE:hI:sv")) != EOF) )
    switch (ch)
      {
      case 'b':
        OPT_BankName = optarg;
        break;

      case 'B':
	OPT_IsBIDs = true;
	break;

      case 'E':
	OPT_IsExtractIDs = true;
	OPT_EIDExtractName = optarg;
	break;

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'I':
	OPT_IsExtractIDs = true;
	OPT_IIDExtractName = optarg;
	break;

      case 's':
	OPT_BankSpy = true;
	break;

      case 'v':
	PrintVersion (argv[0]);
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
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
    }

  if ( optind != argc )
    {
      OPT_IsExtractCodes = true;
      while ( optind != argc )
	OPT_ExtractCodes . insert (Encode (argv [optind ++]));
    }

  if ( OPT_IsExtractIDs )
    ParseExtract( );
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-b path       The directory path of the bank to report\n"
    << "-B            Include BIDs in the output messages (for debugging)\n"
    << "-E file       Report only objects matching EIDs in file\n"
    << "-h            Display help information\n"
    << "-I file       Report only objects matching IIDs in file\n"
    << "-s            Disregard bank locks and write permissions (spy mode)\n"
    << "-v            Display the compatible bank version\n"
    << endl;
  cerr
    << "Takes an AMOS bank directory as input. Will output the information\n"
    << "contained in the bank in the form of an AMOS message to stdout. All\n"
    << "link information references IIDs. If no NCodes are listed on the\n"
    << "command line, all object types will be reported. The -E and -I files\n"
    << "should contain a list of IDs, one per line. Only objects matching an\n"
    << "entry in either file will be output when these options are used and\n"
    << "objects are reported as many times as they are listed. This utility\n"
    << "only outputs bank information. To alter the contents of a bank,\n"
    << "please use the bank-transact utility.\n\n";
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
