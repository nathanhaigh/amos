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
bool   OPT_Reassign    = false;      // Reassign IIDs
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


int exitcode = EXIT_SUCCESS;
long int cnts = 0;                  // messages seen
long int cnta = 0;                  // objects appended
long int cntd = 0;                  // objects deleted
long int cntr = 0;                  // objects replaced
NCode_t ncode;                      // current NCode
char act;                           // action enumeration

BankStreamSet_t bnks;               // all the banks
UniversalSet_t objs;                // all the objects

BankStream_t * bp;                  // current bank
Universal_t * op;                   // current object

typedef HASHMAP::hash_map<ID_t, ID_t> ReassignMap;
ReassignMap reassignLIB;
ReassignMap reassignFRG;
ReassignMap reassignRED;


typedef HASHMAP::hash_map<NCode_t, ID_t> ReassignStart;
ReassignStart ncodemaxiid;


ID_t getNewIID(NCode_t ncode)
{
  ReassignStart::iterator ni = ncodemaxiid.find(ncode);

  if (ni == ncodemaxiid.end())
  {
    BankStream_t * b = & (bnks[ncode]);

    if (!b)
    {
      cerr << "No Bank!" << endl;
      exit (1);
    }

    if (!b->isOpen())
    {
      if (!b->exists(OPT_BankName))
      {
        cerr << "Bank does not exist, starting at 0" << endl;
        ni = ncodemaxiid.insert(make_pair(ncode, 0)).first;
      }
      else
      {
        b->open(OPT_BankName);
        ni = ncodemaxiid.insert(make_pair(ncode, b->getMaxIID())).first;
        cerr << "Bank exists but closed, starting at " << ni->second << endl;
      }
    }
    else
    {
      ni = ncodemaxiid.insert(make_pair(ncode, b->getMaxIID())).first;
      cerr << "Bank exists and open, starting at " << ni->second << endl;
    }
  }

  ni->second++;
  return ni->second;
}

ReassignMap::iterator findOrCreate(ReassignMap & map, NCode_t ncode, ID_t oldiid)
{
  ReassignMap::iterator retval = map.find(oldiid);

  if (retval == map.end())
  {
    ID_t newiid = 0;
    if (oldiid != 0) { newiid = getNewIID(ncode); }

    cerr << "Map " << Decode(ncode) << " from " << oldiid << " to " << newiid << endl;
    retval = reassignFRG.insert(make_pair(oldiid, newiid)).first;
  }

  return retval;
}





void HandleMessage(Message_t & msg)
{
        cnts ++;
        ncode = msg . getMessageCode( );

        if ( ! objs . exists (ncode) )
          {
            cerr << "ERROR: Unrecognized message type" << endl
                 << "  unknown message type '" << Decode (ncode)
                 << "' ignored" << endl;
            exitcode = EXIT_FAILURE;
            return;
          }

        bp = & (bnks [ncode]);
        op = & (objs [ncode]);
        if ( bp -> getStatus( ) )
          return; // skip objects missing a bank

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
          return;
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
          bp -> setStatus (1);
          exitcode = EXIT_FAILURE;
          return;
        }

        //-- Get the message action code
        act = msg . exists (F_ACTION) ? msg [F_ACTION] [0] : E_ADD;

        //-- Perform the appropriate action on the bank
        try {
          switch (act)
            {
            case E_ADD:

              if (OPT_Reassign)
              {
                if (ncode == Fragment_t::NCODE)
                {
                  Fragment_t * frg = (Fragment_t *) op;

                  // Update the iid for the fragment
                  ReassignMap::iterator fi = findOrCreate(reassignFRG, Fragment_t::NCODE, frg->getIID());
                  frg->setIID(fi->second);

                  // Now, update the iids for the matepairs
                  ReassignMap::iterator r1 = findOrCreate(reassignRED, Read_t::NCODE, frg->getMatePair().first);
                  ReassignMap::iterator r2 = findOrCreate(reassignRED, Read_t::NCODE, frg->getMatePair().second);
                  frg->setReads(make_pair(r1->second, r2->second));
                }
                else if (ncode == Read_t::NCODE)
                {
                  Read_t * red = (Read_t *) op;

                  // Update the iid of the read
                  ReassignMap::iterator ri = findOrCreate(reassignRED, Read_t::NCODE, red->getIID());
                  red->setIID(ri->second);

                  // Now, update the fragment
                  ReassignMap::iterator fi = findOrCreate(reassignFRG, Fragment_t::NCODE, red->getFragment());
                  red->setFragment(fi->second);
                }
              }

              //-- Append a new object to the bank
              bp -> append (*op);
              cnta ++;
              break;

            case E_DELETE:
              //-- Flag an existing object for removal from the bank
              if ( op -> getIID( ) != NULL_ID )
                bp -> remove (op -> getIID( ));
              else if ( ! op -> getEID( ) . empty( ) )
                bp -> remove (op -> getEID( ));
              else
                AMOS_THROW_ARGUMENT ("Cannot remove object w/o IID or EID");
              cntd ++;
              break;

            case E_REPLACE:
              //-- Replace an existing object in the bank
              if ( op -> getIID( ) != NULL_ID )
                bp -> replace (op -> getIID( ), *op);
              else if ( ! op -> getEID( ) . empty( ) )
                bp -> replace (op -> getEID( ), *op);
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
          return;
        }
        catch (const ArgumentException_t & e) {
          cerr << "ERROR: " << e . what( ) << endl
               << "  ID conflict caused by '" << Decode (ncode)
               << "' message with iid:"
               << (msg . exists (F_IID) ? msg . getField (F_IID) : "NULL")
               << ", message ignored" << endl;
          exitcode = EXIT_FAILURE;
          return;
        }
      }



//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{

  Message_t msg;                      // current message
  ifstream  msgfile;                   // the message file stream

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- Output the current time and bank directory
  cerr << "START DATE: " << Date( ) << endl;
  cerr << "Bank is: " << OPT_BankName << endl;

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    //-- If OPT_ForceCreate, blast away existing banks
    if ( OPT_ForceCreate )
      for ( BankStreamSet_t::iterator i = bnks.begin(); i != bnks.end(); ++ i )
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



    //-- Read the Messages
    if (OPT_MessageName == "-")
    {
      cerr << "Reading messages from standard in" << endl;
      while ( msg . read (cin) )
      {
        HandleMessage(msg);
      }
    }
    else
    {
      //-- Open the message file
      msgfile . open (OPT_MessageName . c_str( ));
      msgfile . seekg (0, ios::end);
      ProgressDots_t dots (msgfile.tellg(), 50);

      msgfile . seekg (0, ios::beg);

      if ( ! msgfile )
        AMOS_THROW_IO ("Could not open message file " + OPT_MessageName);

      cerr << "    0%                                            100%" << endl
           << "AFG ";

      //-- Read the message file
      while ( msg . read (msgfile) )
        {
          dots . update (msgfile . tellg( ));
          HandleMessage(msg);
        }

      dots . end( );
      msgfile . close( );
    }

    bnks . closeAll( );
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
	 << "at offset: " << msgfile.tellg() << " in message file" << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH


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

  while ( !errflg && ((ch = getopt (argc, argv, "Rb:cfhm:vz")) != EOF) )
    switch (ch)
      {
      case 'R':
        OPT_Reassign = true;
        break;

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

   cerr << "\n.DESCRIPTION.\n"
        << "  Takes an AMOS bank directory and message file as input. Alters the\n"
        << "  banks as directed by the message file. Messages without an act field\n"
        << "  will, by default, be added to the bank. All object links must\n"
        << "  reference IIDs. If an object has a non-unique ID, the user will be\n"
        << "  warned and the object will be ignored. To retrieve information from\n"
        << "  a bank in message format, please use the bank-report utility.\n"
        << "\n.OPTIONS.\n"
        << "  -b path       The directory path of the banks to open or create\n"
        << "  -c            Create new bank directory if path does not exist\n"
        << "  -f            Forcibly create new bank by destroying existing\n"
        << "  -h            Display help information\n"
        << "  -m path       The file path of the input message\n"
        << "  -z            Compress sequence and quality values for SEQ and RED\n"
        << "                (only allows [ACGTN] sequence and [0,63] quality)\n"
        << "  -v            Display the compatible bank version\n"
        << "\n.KEYWORDS.\n"
        << "  amos bank"
        << endl;

  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{ 
  cerr << "\n.USAGE.\n" << "  " <<  s << "  [options]  -b <bank path> -m <message path>\n";
  return;
}
