////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 12/20/2003
//!
//! \brief Transacts the operations described by the messages on a bank
//!
//! bank-transact reads assembly input data from an AMOS message file and
//! modifies the AMOS bank data as directed. External IDs (eid's) and their
//! their links are translated to internal IDs (iid's), but will be returned
//! to eid's on a bank-report operation. Any conflict in this mapping, e.g. a
//! duplicate eid or an invalid link will cause the violating message to be
//! ignored.
//!
////////////////////////////////////////////////////////////////////////////////

#include "amp.hh"
#include "IDMap_AMOS.hh"
#include "messages_AMOS.hh"
#include "universals_AMOS.hh"
#include <iostream>
#include <cassert>
#include <ctime>
#include <vector>
#include <unistd.h>
using namespace AMOS;
using namespace Message_k;
using namespace std;


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
  //***************
  double loopa = 0;
  double loopb = 0;
  clock_t clocka, clockb;
  //***************

  char act;                      // action enumeration
  Message_t msg;                 // the current message
  ifstream msgfile;              // the message file stream
  NCode_t msgcode;               // current message NCode
  IDMap_t typemap(1000);         // NCode to index mapping
  ID_t ti;                       // type index
  Universal_t * typep;           // type pointer
  Bank_t * bankp;                // bank pointer
  long int cnts = 0;             // seen message count
  long int cntc = 0;             // committed message count

  ID_t id;                       // id holder
  pair<ID_t, ID_t> idp;          // id pair
  pair<ID_t, NCode_t> scp;       // source pair
  vector<Tile_t>::iterator tvi;  // tile vector iterator
  vector<ID_t>::iterator ivi;    // ID vector iterator

  //-- Indices of the types in the bank and type arrays
  enum
    {
      I_NULL = 0,
      I_UNIVERSAL,
      I_CONTIG,
      I_CONTIGEDGE,
      I_CONTIGLINK,
      I_FRAGMENT,
      I_KMER,
      I_LIBRARY,
      I_MATEPAIR,
      I_OVERLAP,
      I_READ,
      I_SCAFFOLD,
      I_SEQUENCE,
      I_MAX,
    };

  //-- The known universal types
  Universal_t * types [I_MAX] =
    {
      NULL,
      new Universal_t( ),
      new Contig_t( ),
      new ContigEdge_t( ),
      new ContigLink_t( ),
      new Fragment_t( ),
      new Kmer_t( ),
      new Library_t( ),
      new Matepair_t( ),
      new Overlap_t( ),
      new Read_t( ),
      new Scaffold_t( ),
      new Sequence_t( ),
    };

  //-- The known banks
  Bank_t * banks [I_MAX] =
    {
      NULL,
      new Bank_t (Universal_t::NCode( )),
      new Bank_t (Contig_t::NCode( )),
      new Bank_t (ContigEdge_t::NCode( )),
      new Bank_t (ContigLink_t::NCode( )),
      new Bank_t (Fragment_t::NCode( )),
      new Bank_t (Kmer_t::NCode( )),
      new Bank_t (Library_t::NCode( )),
      new Bank_t (Matepair_t::NCode( )),
      new Bank_t (Overlap_t::NCode( )),
      new Bank_t (Read_t::NCode( )),
      new Bank_t (Scaffold_t::NCode( )),
      new Bank_t (Sequence_t::NCode( )),
    };


  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- Output the current time and bank directory
  cerr << "START DATE: " << Date( ) << endl;
  cerr << "Bank is: " << OPT_BankName << endl;
  cerr << "Message is: " << OPT_MessageName << endl;


  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

  //-- Set up the ncode to index mapping
  typemap . insert (Universal_t::NCode( ),  I_UNIVERSAL);
  typemap . insert (Contig_t::NCode( ),     I_CONTIG);
  typemap . insert (ContigEdge_t::NCode( ), I_CONTIGEDGE);
  typemap . insert (ContigLink_t::NCode( ), I_CONTIGLINK);
  typemap . insert (Fragment_t::NCode( ),   I_FRAGMENT);
  typemap . insert (Kmer_t::NCode( ),       I_KMER);
  typemap . insert (Library_t::NCode( ),    I_LIBRARY);
  typemap . insert (Matepair_t::NCode( ),   I_MATEPAIR);
  typemap . insert (Overlap_t::NCode( ),    I_OVERLAP);
  typemap . insert (Read_t::NCode( ),       I_READ);
  typemap . insert (Scaffold_t::NCode( ),   I_SCAFFOLD);
  typemap . insert (Sequence_t::NCode( ),   I_SEQUENCE);

  //-- Compress RED and SEQ if option is turned on
  if ( OPT_Compress )
    {
      ((Read_t *)(types [I_READ])) -> compress( );
      ((Sequence_t *)(types [I_SEQUENCE])) -> compress( );
    }

  //-- Open the message file
  msgfile . open (OPT_MessageName . c_str( ));
  if ( !msgfile )
    AMOS_THROW_IO ("Could not open message file " + OPT_MessageName);
  
  //-- Read the message file
  while ( 1 )
    {
      clocka = clock( );
      if ( ! msg . read (msgfile) )
	break;

      //-- Increment the message counter
      cnts ++;

      //-- Get the message NCode and figure out the type array index
      msgcode = msg . getMessageCode( );
      try {
	ti = typemap . lookup (msgcode);
      }
      catch (Exception_t & e) {
	cerr << "WARNING: " << e . what( ) << endl
	     << "  unrecognized NCode message type ("
	     << Decode (msgcode) << "), message ignored\n";
	continue;
      }

      //-- Convienence pointers to the current NCode type
      typep = types [ti];
      bankp = banks [ti];

      //-- Parse the message
      try {
	typep -> readMessage (msg);
      }
      catch (Exception_t & e) {
	cerr << "WARNING: " << e . what( ) << endl
	     << "  could not parse fields in "
	     << Decode (msgcode) << " with eid:"
	     << typep -> getEID( ) << ", message ignored\n";
	continue;
      }
      clockb = clock( );
      loopa += (double)(clockb - clocka);

      //-- Open the bank if necessary
      try {
	if ( ! bankp -> isOpen( ) )
	  {
	    if ( OPT_Create )
	      bankp -> create (OPT_BankName);
	    else
	      bankp -> open (OPT_BankName);
	  }
      }
      catch (Exception_t & e) {
	cerr << "WARNING: " << e . what( ) << endl
	     << "  could not open "
	     << Decode (msgcode) << " bank, message ignored\n";
	continue;
      }
      
      //-- Translate the ID pointers from EID to IID
      try {
	switch ( ti )
	  {
	  case I_UNIVERSAL:
	  case I_LIBRARY:
	  case I_SEQUENCE:
	    //-- No pointers to translate
	    break;
	  case I_CONTIG:
	    for ( tvi  = ((Contig_t *)typep) -> getReadTilingItr( );
		  tvi != ((Contig_t *)typep) -> getReadTiling( ) . end( );
		  tvi ++ )
	      tvi -> id = banks [I_READ] -> map( ) . lookup (tvi -> id);
	    break;
	  case I_CONTIGEDGE:
	    for ( ivi  = ((ContigEdge_t *)typep) -> getContigLinksItr( );
		  ivi != ((ContigEdge_t *)typep) -> getContigLinks( ) . end( );
		  ivi ++ )
	      (*ivi) = banks [I_CONTIGLINK] -> map( ) . lookup (*ivi);
	    break;
	  case I_CONTIGLINK:
	    idp = ((ContigLink_t *)typep) -> getContigs( );
	    if ( idp . first != NULL_ID  ||  idp . second != NULL_ID )
	      {
		idp . first =
		  banks [I_CONTIG] -> map( ) . lookup (idp . first);
		idp . second =
		  banks [I_CONTIG] -> map( ) . lookup (idp . second);
		((ContigLink_t *)typep) -> setContigs (idp);
	      }
	    scp = ((ContigLink_t *)typep) -> getSource( );
	    if ( scp . second != NULL_NCODE )
	      {
		scp . first =
		  banks [typemap . lookup (scp . second)] ->
		  map( ) . lookup (scp . first);
		((ContigLink_t *)typep) -> setSource (scp);
	      }
	    break;
	  case I_FRAGMENT:
	    if ( ((Fragment_t *)typep) -> getLibrary( ) != NULL_ID )
	      ((Fragment_t *)typep) -> setLibrary (banks [I_LIBRARY] ->
	        map( ) . lookup (((Fragment_t *)typep) -> getLibrary( )));
	    if ( ((Fragment_t *)typep) -> getSource( ) != NULL_ID )
	      ((Fragment_t *)typep) -> setSource (banks [I_FRAGMENT] ->
                map( ) . lookup (((Fragment_t *)typep) -> getSource( )));
	    break;
	  case I_KMER:
	    for ( ivi  = ((Kmer_t *)typep) -> getReadsItr( );
		  ivi != ((Kmer_t *)typep) -> getReads( ) . end( );
		  ivi ++ )
	      (*ivi) = banks [I_READ] -> map( ) . lookup (*ivi);
	    break;
	  case I_MATEPAIR:
	    idp = ((Matepair_t *)typep) -> getReads( );
	    if ( idp . first != NULL_ID  ||  idp . second != NULL_ID )
	      {
		idp . first = banks [I_READ] -> map( ) . lookup (idp . first);
		idp . second = banks [I_READ] -> map( ) . lookup (idp . second);
		((Matepair_t *)typep) -> setReads (idp);
	      }
	    break;
	  case I_OVERLAP:
	    idp = ((Overlap_t *)typep) -> getReads( );
	    if ( idp . first != NULL_ID  ||  idp . second != NULL_ID )
	      {
		idp . first = banks [I_READ] -> map( ) . lookup (idp . first);
		idp . second = banks [I_READ] -> map( ) . lookup (idp . second);
		((Overlap_t *)typep) -> setReads (idp);
	      }
	    break;
	  case I_READ:
	    if ( ((Read_t *)typep) -> getFragment( ) != NULL_ID )
	      ((Read_t *)typep) -> setFragment (banks [I_FRAGMENT] ->
                map( ) . lookup (((Read_t *)typep) -> getFragment( )));
	    break;
	  case I_SCAFFOLD:
	    for ( tvi  = ((Scaffold_t *)typep) -> getContigTilingItr( );
		  tvi != ((Scaffold_t *)typep) -> getContigTiling( ) . end( );
		  tvi ++ )
	      tvi -> id = banks [I_CONTIG] -> map( ) . lookup (tvi -> id);
	    for ( ivi  = ((Scaffold_t *)typep) -> getContigEdgesItr( );
		  ivi != ((Scaffold_t *)typep) -> getContigEdges( ) . end( );
		  ivi ++ )
	      (*ivi) = banks [I_CONTIGEDGE] -> map( ) . lookup (*ivi);
	    break;
	  default:
	    assert (false);
	  }
      }
      catch (Exception_t & e) {
	//-- If failed to resolve a link, ignore message
	cerr << "WARNING: " << e . what( ) << endl
	     << "  could not resolve eid links in "
	     << Decode (msgcode) << " with eid:"
	     << typep -> getEID( ) << ", message ignored\n";
	continue;
      }

      //-- Get the message action code
      if ( msg . exists (F_ACTION) )
	act = (msg . getField (F_ACTION)) [0];
      else
	act = E_ADD;

      //-- Perform the appropriate action on the bank
      clocka = clock( );
      try {
	switch (act)
	  {
	  case E_ADD:
	    //-- Append a new object to the bank
	    id = bankp -> getLastIID( ) + 1;
	    if ( typep -> getEID( ) != NULL_ID )
	      bankp -> map( ) . insert (typep -> getEID( ), id);
	    bankp -> append (*typep);
	    assert (id == typep -> getIID( ));
	    break;
	  case E_DELETE:
	    //-- Flag an existing object for deletion from the bank
	    typep -> setIID (bankp -> map( ) . lookup (typep -> getEID( )));
	    bankp -> remove (*typep);
	    break;
	  case E_EDIT:
	    //-- Replace an existing object in the bank
	    typep -> setIID (bankp -> map( ) . lookup (typep -> getEID( )));
	    bankp -> replace (*typep);
	    break;
	  default:
	    AMOS_THROW_IO ("Unrecognized action field");
	  }
      }
      catch (IOException_t & e) {
	cerr << "WARNING: " << e . what( ) << endl
	     << "  could not commit "
	     << Decode (msgcode) << " with eid:"
	     << typep -> getEID( ) << " to bank, message ignored\n";
	continue;
      }
      catch (Exception_t & e) {
	cerr << "WARNING: " << e . what( ) << endl
	     << "  ID conflict caused by "
	     << Decode (msgcode) << " with eid:"
	     << typep -> getEID( ) << ", message ignored\n";
	continue;
      }
      clockb = clock( );
      loopb += (double)(clockb - clocka);

      cntc ++;
    }
  

  //-- Close all the banks and free the object
  msgfile . close( );
  for ( ID_t i = 1; i < I_MAX; i ++ )
    {
      banks [i] -> close( );
      delete banks [i];
      delete types [i];
    }
  }
  catch (Exception_t & e) {

  //-- On error, print debugging information
  cerr << "Current message: " << Decode (msgcode)
       << " eid:" << typep -> getEID( ) << endl
       << "Messages seen: " << cnts << endl
       << "Messages committed: " << cntc << endl
       << "ERROR: -- Fatal AMOS Exception --\n" << e;
  return EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH


  //-- Output the end time
  cerr << "Messages seen: " << cnts << endl
       << "Messages committed: " << cntc << endl
       << "END DATE:   " << Date( ) << endl;

  cerr << endl
       << "loopa: " << (double)loopa / CLOCKS_PER_SEC << " sec.\n"
       << "loopb: " << (double)loopb / CLOCKS_PER_SEC << " sec.\n"
       << "granu: " << CLOCKS_PER_SEC << " of a sec.\n";

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

  if (OPT_BankName == "")
    {
      cerr << "ERROR: The -b option is mandatory\n";
      errflg ++;
    }

  if (OPT_MessageName == "")
    {
      cerr << "ERROR: The -m option is mandatory\n";
      errflg ++;
    }

  if (OPT_Create  && !OPT_ForceCreate  &&
      !access (OPT_BankName . c_str( ), F_OK))
    {
      cerr << "ERROR: Bank path already exists\n";
      errflg ++;
    }

  if (!OPT_Create  &&
      access (OPT_BankName . c_str( ), R_OK|W_OK|X_OK))
    {
      cerr << "ERROR: Bank path is not accessible\n";
      errflg ++;
    }

  if (access (OPT_MessageName . c_str( ), R_OK))
    {
      cerr << "ERROR: Message path is not accessible\n";
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
    << "will, by default, be added to the bank. EID link information in the\n"
    << "messages will be automatically translated to IID link information, so\n"
    << "all links in a bank will refer to IIDs. To retrieve information from\n"
    << "a bank in message format, please use the bank-report utility which\n"
    << "will return all linking information back to EIDs.\n\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  -b <bank path>  -m <message path>\n\n";
  return;
}
