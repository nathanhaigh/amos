///////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 11/12/2004
//!
//! \brief Transacts the operations described by the messages on a bank
//!
//! bank-transact reads assembly input data from an AMOS message file and
//! modifies the AMOS bank data as directed. All object links should reference
//! IIDs (internal IDs), but objects will be accessible from the bank by both
//! their IID and EID (external ID). Any conflict in the IID, EID mapping, e.g.
//! a duplicate id, will cause the violating message to be ignored and the
//! user will be warned of the inconsistency.
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include "amp.hh"
#include <iostream>
#include <vector>
#include <unistd.h>
using namespace AMOS;
using namespace std;
using namespace HASHMAP;





//=============================================================== Globals ====//
bool   OPT_Create      = false;      // create bank option
bool   OPT_ForceCreate = false;      // forcibly create bank option
bool   OPT_Compress    = false;      // SEQ and RED compression option
string OPT_BankName;                 // bank name parameter
string OPT_MessageName;              // message name parameter



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
  long int cnts = 0;                  // messages seen
  long int cnta = 0;                  // objects appended
  long int cntd = 0;                  // objects deleted
  long int cntr = 0;                  // objects replaced
  Message_t msg;                      // current message
  NCode_t ncode;                      // current NCode
  char act;                           // action enumeration
  ifstream msgfile;                   // the message file stream

  BankSet_t bnks;                     // all the banks
  UniversalSet_t objs;                // all the objects

  Bank_t * bp;                        // current bank
  Universal_t * op;                   // current object


  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- Output the current time and bank directory
  cerr << "START DATE: " << Date( ) << endl;
  cerr << "Bank is: " << OPT_BankName << endl;

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    //-- If OPT_ForceCreate, blast away existing banks
    if ( OPT_ForceCreate )
      for ( BankSet_t::iterator i = bnks . begin( ); i != bnks . end( ); ++ i )
        if ( i -> exists (OPT_BankName) )
          {
            i -> open (OPT_BankName);
            i -> destroy ( );
          }

    //-- Compress RED and SEQ if option is turned on
    if ( OPT_Compress )
      {
        ((Read_t &)objs [Read_t::NCODE]) . compress( );
        ((Sequence_t &)objs [Sequence_t::NCODE]) . compress( );
      }

    //-- Open the message file
    msgfile . open (OPT_MessageName . c_str( ));
    msgfile . seekg (0, ios::end);
    ProgressDots_t dots (msgfile . tellg( ), 50);
    msgfile . seekg (0, ios::beg);

    if ( ! msgfile )
      AMOS_THROW_IO ("Could not open message file " + OPT_MessageName);

    cerr << "    0%                                            100%" << endl
         << "AFG ";

    //-- Read the message file
    while ( msg . read (msgfile) )
      {
        cnts ++;
        dots . update (msgfile . tellg( ));

        ncode = msg . getMessageCode( );

        if ( ! objs . exists (ncode) )
          {
            cerr << "ERROR: Unrecognized message type" << endl
                 << "  unknown message type '" << Decode (ncode)
                 << "' ignored" << endl;
            exitcode = EXIT_FAILURE;
            continue;
          }

        bp = & (bnks [ncode]);
        op = & (objs [ncode]);
        if ( op -> isFlagA( ) )
          continue; // skip objects missing a bank

        //-- Parse the message
        try {
          op -> readMessage (msg);
        }
        catch (const Exception_t & e) {
          cerr << "ERROR: " << e . what( ) << endl
               << "  could not parse '" << Decode (ncode)
               << "' message with iid:"
               << (msg . exists (F_IID) ? msg . getField (F_IID) : "NULL")
               << ", message ignored" << endl;
          exitcode = EXIT_FAILURE;
          continue;
        }

        //-- Open the bank if necessary
        try {
          if ( ! bp -> isOpen( ) )
            {
              if ( ! bp -> exists (OPT_BankName) )
                bp -> create (OPT_BankName);
              else
                bp -> open (OPT_BankName);
            }
        }
        catch (const Exception_t & e) {
          cerr << "ERROR: " << e . what( ) << endl
               << "  could not open '" << Decode (ncode)
               << "' bank, all messages ignored" << endl;
          op -> setFlagA (true);
          exitcode = EXIT_FAILURE;
          continue;
        }

        //-- Get the message action code
        act = msg . exists (F_ACTION) ? msg [F_ACTION] [0] : E_ADD;

        //-- Perform the appropriate action on the bank
        try {
          switch (act)
            {
            case E_ADD:
              //-- Append a new object to the bank
              bp -> append (*op);
              cnta ++;
              break;

            case E_DELETE:
              //-- Flag an existing object for removal from the bank
              if ( op -> getIID( ) != NULL_ID )
                bp -> remove (op -> getIID( ));
              else if ( ! op -> getEID( ) . empty( ) )
                bp -> remove (op -> getEID( ) . c_str( ));
              else
                AMOS_THROW_ARGUMENT ("Cannot remove object w/o IID or EID");
              cntd ++;
              break;

            case E_REPLACE:
              //-- Replace an existing object in the bank
              if ( op -> getIID( ) != NULL_ID )
                bp -> replace (op -> getIID( ), *op);
              else if ( ! op -> getEID( ) . empty( ) )
                bp -> replace (op -> getEID( ) . c_str( ), *op);
              else
                AMOS_THROW_ARGUMENT ("Cannot replace object w/o IID or EID");
              cntr ++;
              break;

            default:
              AMOS_THROW_IO ((string)"Unrecognized action field " + act);
            }
        }
        catch (const IOException_t & e) {
          cerr << "ERROR: " << e . what( ) << endl
               << "  could not commit '" << Decode (ncode)
               << "' message with iid:"
               << (msg . exists (F_IID) ? msg . getField (F_IID) : "NULL")
               << " to bank, message ignored" << endl;
          exitcode = EXIT_FAILURE;
          continue;
        }
        catch (const ArgumentException_t & e) {
          cerr << "ERROR: " << e . what( ) << endl
               << "  ID conflict caused by '" << Decode (ncode)
               << "' message with iid:"
               << (msg . exists (F_IID) ? msg . getField (F_IID) : "NULL")
               << ", message ignored" << endl;
          exitcode = EXIT_FAILURE;
          continue;
        }
      }

    dots . end( );
    bnks . closeAll( );
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH

  msgfile . close( );

  //-- Output the end time
  cerr << "Messages read: " << cnts << endl
       << "Objects added: " << cnta << endl
       << "Objects deleted: " << cntd << endl
       << "Objects replaced: " << cntr << endl
       << "END DATE:   " << Date( ) << endl;

  return exitcode;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "b:cfhm:vz")) != EOF) )
    switch (ch)
      {
      case 'b':
        OPT_BankName = optarg;
        break;

      case 'c':
        OPT_Create = true;
        break;

      case 'f':
        OPT_Create = true;
        OPT_ForceCreate = true;
        break;

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'm':
        OPT_MessageName = optarg;
        break;

      case 'v':
        PrintBankVersion (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'z':
        OPT_Compress = true;
        break;

      default:
        errflg ++;
      }

  if ( OPT_BankName . empty( ) )
    {
      cerr << "ERROR: The -b option is mandatory" << endl;
      errflg ++;
    }

  if ( OPT_MessageName . empty( ) )
    {
      cerr << "ERROR: The -m option is mandatory" << endl;
      errflg ++;
    }

  if ( !OPT_Create && access (OPT_BankName . c_str( ), R_OK|W_OK|X_OK) )
    {
      cerr << "ERROR: Bank directory is not accessible, "
           << strerror (errno) << endl;
      errflg ++;
    }

  if ( OPT_Create && !OPT_ForceCreate && !access (OPT_BankName.c_str( ), F_OK) )
    {
      cerr << "ERROR: Bank path already exists" << endl;
      errflg ++;
    }

  if ( errflg > 0 || argc != optind )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information." << endl;
      exit (EXIT_FAILURE);
    }
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-b path       The directory path of the banks to open or create\n"
    << "-c            Create new bank directory if path does not exist\n"
    << "-f            Forcibly create new bank by destroying existing\n"
    << "-h            Display help information\n"
    << "-m path       The file path of the input message\n"
    << "-z            Compress sequence and quality values for SEQ and RED\n"
    << "              (only allows [ACGTN] sequence and [0,63] quality)\n"
    << "-v            Display the compatible bank version\n"
    << endl;
  cerr
    << "Takes an AMOS bank directory and message file as input. Alters the\n"
    << "banks as directed by the message file. Messages without an act field\n"
    << "will, by default, be added to the bank. All object links must\n"
    << "reference IIDs. If an object has a non-unique ID, the user will be\n"
    << "warned and the object will be ignored. To retrieve information from\n"
    << "a bank in message format, please use the bank-report utility.\n"
    << endl;
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  -b <bank path>  -m <message path>\n"
    << endl;
  return;
}
