///////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 12/20/2003
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


struct obpair
{
  Universal_t * obj;
  Bank_t * bank;
  bool warned;

  obpair ( )
    : obj(NULL), bank(NULL), warned(false)
  {}

  obpair (Bank_t * bank_p, Universal_t * obj_p)
    : obj(obj_p), bank(bank_p), warned(false)
  {}
};


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
  long int cnts = 0;
  long int cntc = 0;
  Message_t msg;
  NCode_t ncode;
  hash_map<NCode_t, obpair> obps;   // object,bank pair NCode-keyed map
  ifstream msgfile;                 // the message file stream
  char act;                         // action enumeration
  obpair * obp;                     // object,bank pair pointer

  //-- The KNOWN types to put in the banks
  //   note: node constants are part of the AMOS namespace
  //    e.g. AMOS::M_UNIVERSAL == Encode ("UNV")
  obps[M_UNIVERSAL]   =obpair(new Bank_t (M_UNIVERSAL),   new Universal_t);
  obps[M_SEQUENCE]    =obpair(new Bank_t (M_SEQUENCE),    new Sequence_t);
  obps[M_LIBRARY]     =obpair(new Bank_t (M_LIBRARY),     new Library_t);
  obps[M_FRAGMENT]    =obpair(new Bank_t (M_FRAGMENT),    new Fragment_t);
  obps[M_READ]        =obpair(new Bank_t (M_READ),        new Read_t);
  obps[M_MATEPAIR]    =obpair(new Bank_t (M_MATEPAIR),    new Matepair_t);
  obps[M_OVERLAP]     =obpair(new Bank_t (M_OVERLAP),     new Overlap_t);
  obps[M_KMER]        =obpair(new Bank_t (M_KMER),        new Kmer_t);
  obps[M_LAYOUT]      =obpair(new Bank_t (M_LAYOUT),      new Layout_t);
  obps[M_CONTIG]      =obpair(new Bank_t (M_CONTIG),      new Contig_t);
  obps[M_CONTIGLINK]  =obpair(new Bank_t (M_CONTIGLINK),  new ContigLink_t);
  obps[M_CONTIGEDGE]  =obpair(new Bank_t (M_CONTIGEDGE),  new ContigEdge_t);
  obps[M_SCAFFOLD]    =obpair(new Bank_t (M_SCAFFOLD),    new Scaffold_t);
  obps[M_SCAFFOLDLINK]=obpair(new Bank_t (M_SCAFFOLDLINK),new ScaffoldLink_t);
  obps[M_SCAFFOLDEDGE]=obpair(new Bank_t (M_SCAFFOLDEDGE),new ScaffoldEdge_t);


  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- Output the current time and bank directory
  cerr << "START DATE: " << Date( ) << endl;
  cerr << "Bank is: " << OPT_BankName << endl;

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    //-- Compress RED and SEQ if option is turned on
    if ( OPT_Compress )
      {
	((Read_t *)(obps [M_READ] . obj)) -> compress( );
	((Sequence_t *)(obps [M_SEQUENCE] . obj)) -> compress( );
      }

    //-- Open the message file
    msgfile . open (OPT_MessageName . c_str( ));
    if ( !msgfile )
      AMOS_THROW_IO ("Could not open message file " + OPT_MessageName);

    //-- Read the message file
    while ( msg . read (msgfile) )
      {
	//-- Increment the message counter
	++ cnts;

	//-- Get the message NCode and figure out the type array index
	ncode = msg . getMessageCode( );
	obp = &(obps [ncode]);

	if ( obp -> bank == NULL )
	  {
	    cerr << "WARNING: Unrecognized message type "
		 << Decode (ncode) << " ignored\n";
	    continue;
	  }

	//-- Parse the message
	try {
	  obp -> obj -> readMessage (msg);
	}
	catch (Exception_t & e) {
	  cerr << "WARNING: " << e . what( ) << endl
	       << "  could not parse fields in " << Decode (ncode)
	       << " with iid:" << msg . getField (F_IID)
	       << ", message ignored\n";
	  continue;
	}

	//-- Open the bank if necessary
	try {
	  if ( ! obp -> bank -> isOpen( ) )
	    {
	      if ( OPT_Create )
		{
		  if ( !OPT_ForceCreate  &&  obp->bank->exists (OPT_BankName) )
		    AMOS_THROW_IO ("Bank already exists");
		  obp -> bank -> create (OPT_BankName);
		}
	      else
		obp -> bank -> open (OPT_BankName);
	    }
	}
	catch (Exception_t & e) {
	  if ( ! obp -> warned )
	    {
	      cerr << "WARNING: " << e . what( ) << endl
		   << "  could not open " << Decode (ncode)
		   << " bank, all messages ignored\n";
	      obp -> warned = true;
	    }
	  continue;
	}

	//-- Get the message action code
	if ( msg . exists (F_ACTION) )
	  act = (msg . getField (F_ACTION)) [0];
	else
	  act = E_ADD;

	//-- Perform the appropriate action on the bank
	try {
	  switch (act)
	    {
	    case E_ADD:
	      //-- Append a new object to the bank
	      obp -> bank -> append (*(obp -> obj));
	      break;

	    case E_DELETE:
	      //-- Flag an existing object for removal from the bank
	      if ( obp -> obj -> getIID( ) != NULL_ID )
		obp -> bank -> remove (obp -> obj -> getIID( ));
	      else if ( ! obp -> obj -> getEID( ) . empty( ) )
		obp -> bank -> remove (obp -> obj -> getEID( ) . c_str( ));
	      else
		AMOS_THROW_ARGUMENT ("Cannot remove object w/o IID or EID");
	      break;

	    case E_REPLACE:
	      //-- Replace an existing object in the bank
	      if ( obp -> obj -> getIID( ) != NULL_ID )
		obp -> bank -> replace (obp -> obj -> getIID( ), *(obp -> obj));
	      else if ( ! obp -> obj -> getEID( ) . empty( ) )
		obp -> bank -> replace
		  (obp -> obj -> getEID( ) . c_str( ), *(obp -> obj));
	      else
		AMOS_THROW_ARGUMENT ("Cannot edit object w/o IID or EID");
	      break;

	    default:
	      AMOS_THROW_IO ("Unrecognized action field");
	    }
	}
	catch (IOException_t & e) {
	  cerr << "WARNING: " << e . what( ) << endl
	       << "  could not commit " << Decode (ncode)
	       << " with iid:" << msg . getField (F_IID)
	       << " to bank, message ignored\n";
	  continue;
	}
	catch (ArgumentException_t & e) {
	  cerr << "WARNING: " << e . what( ) << endl
	       << "  ID conflict caused by " << Decode (ncode)
	       << " with iid:" << msg . getField (F_IID)
	       << ", message ignored\n";
	  continue;
	}

	cntc ++;
      }

    //-- Close all the banks and free the objects
    msgfile . close( );

    hash_map<NCode_t, obpair>::iterator mi;
    for ( mi = obps . begin( ); mi != obps . end( ); ++ mi )
      {
	if ( mi -> second . bank != NULL )
	  mi -> second . bank -> close( );
	delete mi -> second . bank;
	delete mi -> second . obj;
      }
  }
  catch (Exception_t & e) {

    //-- On error, print debugging information
    cerr << "Current message: " << Decode (ncode) << " iid:";
    if ( msg . exists (F_IID) )
      cerr << msg . getField (F_IID);
    cerr << "\nMessages seen: " << cnts
	 << "\nMessages committed: " << cntc
      	 << "\nERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH
  
  
  //-- Output the end time
  cerr << "Messages seen: " << cnts << endl
       << "Messages committed: " << cntc << endl
       << "END DATE:   " << Date( ) << endl;

  return EXIT_SUCCESS;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "b:cfhm:z")) != EOF) )
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
      case 'z':
	OPT_Compress = true;
	break;
      default:
	errflg ++;
      }

  if ( OPT_BankName . empty( ) )
    {
      cerr << "ERROR: The -b option is mandatory\n";
      errflg ++;
    }

  if ( OPT_MessageName . empty( ) )
    {
      cerr << "ERROR: The -m option is mandatory\n";
      errflg ++;
    }

  if ( !OPT_Create  &&  access (OPT_BankName . c_str( ), R_OK|W_OK|X_OK) )
    {
      cerr << "ERROR: Bank directory is not accessible\n";
      errflg ++;
    }

  if ( OPT_Create  &&
       !OPT_ForceCreate  &&
       !access (OPT_BankName . c_str( ), F_OK) )
    {
      cerr << "ERROR: Bank path already exists\n";
      errflg ++;
    }

  if ( errflg > 0 || argc != optind )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
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
    << "-f            Forcibly create new banks by overwriting existing\n"
    << "-h            Display help information\n"
    << "-m path       The file path of the input message\n"
    << "-z            Compress sequence and quality values for SEQ and RED\n"
    << "              (only allows [ACGTN] sequence and [0,63] quality)\n\n";
  cerr
    << "Takes an AMOS bank directory and message file as input. Alters the\n"
    << "banks as directed by the message file. Messages without an act field\n"
    << "will, by default, be added to the bank. All object links must\n"
    << "reference IIDs. If an object has a non-unique ID, the user will be\n"
    << "warned and the object will be ignored. To retrieve information from\n"
    << "a bank in message format, please use the bank-report utility.\n\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  -b <bank path>  -m <message path>\n\n";
  return;
}
