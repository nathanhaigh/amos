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
void PrintObject (const Universal_t & obj, ID_t bid);


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
  long int cnto = 0;       // objects reported
  long int cntc = 0;       // banks reported
  long int cnts = 0;       // banks seen
  NCode_t ncode;           // current object type
  UniversalSet_t objs;     // all the universal objects
  ID_t bid;                // bank index

  UniversalSet_t::iterator ui;
  set<NCode_t>::iterator ci;

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- Output the current time and bank directory
  cerr << "START DATE: " << Date( ) << endl;
  cerr << "Bank is: " << OPT_BankName << endl;

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    cerr << "    0%                                            100%" << endl;

    //-- Iterate through each known object and dump if its got bank
    for ( ui = objs.begin( ); ui != objs.end( ); ++ ui )
      {
	ncode = ui -> getNCode( );

	//-- Skip if we're not looking at this one or it doesn't exist
	if ( (OPT_IsExtractCodes  &&
	      OPT_ExtractCodes . find (ncode) == OPT_ExtractCodes . end( ))
	     ||
	     (!OPT_IsExtractCodes  &&
              !BankExists (ncode, OPT_BankName)) )
	  continue;

        cnts ++;
        OPT_ExtractCodes . erase (ncode);

        //-- Report the bank
        try {
          cerr << Decode (ncode) << " ";

          if ( OPT_IsExtractIDs )
            {
              vector<ID_t>::iterator ii = OPT_ExtractIIDs . begin( );
              vector<string>::iterator ei = OPT_ExtractEIDs . begin( );
              Bank_t bank (ncode);
              
              if ( OPT_BankSpy )
                bank . open (OPT_BankName, B_SPY);
              else
                bank . open (OPT_BankName, B_READ);

              ProgressDots_t dots (OPT_ExtractIIDs . size( ) +
                                   OPT_ExtractEIDs . size( ), 50);

              //-- Get the requested IIDs
              for ( ; ii != OPT_ExtractIIDs . end( ); ++ ii )
                {
                  dots . update (ii - OPT_ExtractIIDs . begin( ));
                  bid = bank . getIDMap( ) . lookupBID (*ii);
                  if ( bid == NULL_ID )
                    {
                      cerr << "ERROR: IID '" << *ii << "' not found in '"
                           << Decode (ncode) << "' bank" << endl;
                      exitcode = EXIT_FAILURE;
                      continue;
                    }
                  bank . fetch (*ii, *ui);
                  PrintObject (*ui, bid);
                  cnto ++;
                }

              //-- Get the requested EIDs
              for ( ; ei != OPT_ExtractEIDs . end( ); ++ ei )
                {
                  dots . update ((ei - OPT_ExtractEIDs . begin( )) +
                                 (ii - OPT_ExtractIIDs . begin( )));
                  bid = bank . getIDMap( ) . lookupBID (ei -> c_str( ));
                  if ( bid == NULL_ID )
                    {
                      cerr << "ERROR: EID '" << *ei << "' not found in '"
                           << Decode (ncode) << "' bank" << endl;
                      exitcode = EXIT_FAILURE;
                      continue;
                    }
                  bank . fetch (ei -> c_str( ), *ui);
                  PrintObject (*ui, bid);
                  cnto ++;
                }

              dots . end( );
              bank . close( );
            }
          else
            {
              BankStream_t bankstream (ncode);

	      if ( OPT_BankSpy )
		bankstream . open (OPT_BankName, B_SPY);
	      else
		bankstream . open (OPT_BankName, B_READ);

              long int cntd = 0;
              ProgressDots_t dots (bankstream . getSize( ), 50);

              //-- Get all objects
              while ( bankstream >> *ui )
                {
                  dots . update (++ cntd);
                  PrintObject (*ui, bankstream . tellg( ) - 1);
                  cnto ++;
                }

              dots . end( );
              bankstream . close( );
            }
        }
        catch (const Exception_t & e) {
          cerr << "ERROR: " << e . what( ) << endl
               << "  failed to report '" << Decode (ncode) << "' bank" << endl;
          exitcode = EXIT_FAILURE;
          continue;
        }

        cntc ++;
      }

    //-- Any codes unrecognized?
    for ( ci = OPT_ExtractCodes.begin( ); ci != OPT_ExtractCodes.end( ); ++ ci )
      {
        cnts ++;
        cerr << "ERROR: Unrecognized bank type" << endl
             << "  unknown bank type '" << Decode (*ci) << "' ignored" << endl;
        exitcode = EXIT_FAILURE;
      }
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH


  //-- Output the end time
  cerr << "Report attempts: " << cnts << endl
       << "Report successes: " << cntc << endl
       << "Objects reported: " << cnto << endl
       << "END DATE:   " << Date( ) << endl;

  return exitcode;
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
	cerr << "ERROR: Could not open " << OPT_IIDExtractName << endl;

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
	cerr << "ERROR: Could not open " << OPT_EIDExtractName << endl;

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
void PrintObject (const Universal_t & obj, ID_t bid)
{
  Message_t msg;
  ostringstream ss;

  obj . writeMessage (msg);

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
	PrintBankVersion (argv[0]);
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
