////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 01/07/2004
//!
//! \brief Reports the data contained in a bank to a message file
//!
//! bank-report reads the assembly data from an AMOS bank and writes it to an
//! AMOS message file. The bank's internal ID (iid) links are translated back
//! their external IDs (eid's).
//!
//! \todo allow the reporting of one or more specific objects
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
string OPT_BankName;                 // bank name parameter
string OPT_MessageName;              // message name parameter
bool   OPT_Concat = false;           // concat to existing message
bool   OPT_Truncate = false;         // truncate message file before write


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

  Message_t msg;                 // the current message
  ofstream msgfile;              // the message file stream
  IDMap_t typemap(1000);         // NCode to index mapping
  ID_t ti;                       // type index
  Universal_t * typep;           // type pointer
  Bank_t * bankp = NULL;         // bank pointer

  long int cnts = 0;             // seen object count
  long int cntw = 0;             // written object count

  ID_t id;                       // id holder
  pair<ID_t, ID_t> idp;          // id pair
  pair<ID_t, NCode_t> scp;       // source pair
  vector<Tile_t>::iterator tvi;  // tile vector iterator
  vector<ID_t>::iterator ivi;    // ID vector iterator

  //-- Indices of the types in the bank array
  //   order of this list is important to maintain def before ref linking rule
  enum
    {
      I_NULL = 0,
      I_UNIVERSAL,
      I_SEQUENCE,
      I_LIBRARY,
      I_FRAGMENT,
      I_READ,
      I_MATEPAIR,
      I_OVERLAP,
      I_KMER,
      I_CONTIG,
      I_CONTIGLINK,
      I_CONTIGEDGE,
      I_SCAFFOLD,
      I_MAX,
    };

  //-- The known universal types
  Universal_t * types [I_MAX] =
    {
      NULL,
      new Universal_t( ),
      new Sequence_t( ),
      new Library_t( ),
      new Fragment_t( ),
      new Read_t( ),
      new Matepair_t( ),
      new Overlap_t( ),
      new Kmer_t( ),
      new Contig_t( ),
      new ContigLink_t( ),
      new ContigEdge_t( ),
      new Scaffold_t( ),
    };

  //-- Inverted ID maps (IID->EID)
  IDMap_t * invmaps [I_MAX] =
    {
      NULL,
      new IDMap_t( ),
      new IDMap_t( ),
      new IDMap_t( ),
      new IDMap_t( ),
      new IDMap_t( ),
      new IDMap_t( ),
      new IDMap_t( ),
      new IDMap_t( ),
      new IDMap_t( ),
      new IDMap_t( ),
      new IDMap_t( ),
      new IDMap_t( ),
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
  typemap . insert (Sequence_t::NCode( ),   I_SEQUENCE);
  typemap . insert (Library_t::NCode( ),    I_LIBRARY);
  typemap . insert (Fragment_t::NCode( ),   I_FRAGMENT);
  typemap . insert (Read_t::NCode( ),       I_READ);
  typemap . insert (Matepair_t::NCode( ),   I_MATEPAIR);
  typemap . insert (Overlap_t::NCode( ),    I_OVERLAP);
  typemap . insert (Kmer_t::NCode( ),       I_KMER);
  typemap . insert (Contig_t::NCode( ),     I_CONTIG);
  typemap . insert (ContigLink_t::NCode( ), I_CONTIGLINK);
  typemap . insert (ContigEdge_t::NCode( ), I_CONTIGEDGE);
  typemap . insert (Scaffold_t::NCode( ),   I_SCAFFOLD);


  //-- Open the message file
  msgfile . open (OPT_MessageName . c_str( ),
		  OPT_Truncate ? ios::out|ios::trunc : ios::out);
  if ( !msgfile )
    AMOS_THROW_IO ("Could not open message file " + OPT_MessageName);


  //-- Iterate through each bank and add objects to the message file
  for ( ti = 1; ti < I_MAX; ti ++ )
    {
      //-- Convienence pointers to the current NCode type
      delete bankp;
      typep = types [ti];
      bankp = new Bank_t (typep -> getNCode( ));

      //-- Open the bank if it exists
      try {
	if ( ! bankp -> exists (OPT_BankName) )
	  continue;
	else
	  bankp -> open (OPT_BankName);
      }
      catch (Exception_t & e) {
	cerr << "WARNING: " << e . what( ) << endl
	     << "  could not open "
	     << Decode (typep -> getNCode( )) << " bank, types ignored\n";
	continue;
      }

      //-- Invert the bank's ID map so it is now an IID -> EID map
      (*(invmaps [ti])) = bankp -> map( );
      invmaps [ti] -> invert( );

      //-- Iterate through each object in the bank
      for ( id = 1; id <= bankp -> getLastIID( ); id ++ )
	{
	  //-- Fetch the next object
	  typep -> setIID (id);
	  bankp -> fetch (*typep);
	  cnts ++;

	  //-- Translate the ID pointers from IID to EID
	  try {
	    switch ( ti )
	      {
	      case I_UNIVERSAL:
	      case I_LIBRARY:
	      case I_SEQUENCE:
		//-- No pointers to translate
		break;
	      case I_FRAGMENT:
		if ( ((Fragment_t *)typep) -> getLibrary( ) != NULL_ID )
		  ((Fragment_t *)typep) -> setLibrary (invmaps [I_LIBRARY] ->
		    lookup (((Fragment_t *)typep) -> getLibrary( )));
		if ( ((Fragment_t *)typep) -> getSource( ) != NULL_ID )
		  ((Fragment_t *)typep) -> setSource (invmaps [I_FRAGMENT] ->
		    lookup (((Fragment_t *)typep) -> getSource( )));
		break;
	      case I_READ:
		if ( ((Read_t *)typep) -> getFragment( ) != NULL_ID )
		  ((Read_t *)typep) -> setFragment (invmaps [I_FRAGMENT] ->
                    lookup (((Read_t *)typep) -> getFragment( )));
		break;
	      case I_MATEPAIR:
		idp = ((Matepair_t *)typep) -> getReads( );
		if ( idp . first != NULL_ID  ||  idp . second != NULL_ID )
		  {
		    idp . first = invmaps [I_READ] -> lookup (idp . first);
		    idp . second = invmaps [I_READ] -> lookup (idp . second);
		    ((Matepair_t *)typep) -> setReads (idp);
		  }
		break;
	      case I_OVERLAP:
		idp = ((Overlap_t *)typep) -> getReads( );
		if ( idp . first != NULL_ID  ||  idp . second != NULL_ID )
		  {
		    idp . first = invmaps [I_READ] -> lookup (idp . first);
		    idp . second = invmaps [I_READ] -> lookup (idp . second);
		    ((Overlap_t *)typep) -> setReads (idp);
		  }
		break;
	      case I_KMER:
		for ( ivi  = ((Kmer_t *)typep) -> getReads( ) . begin( );
		      ivi != ((Kmer_t *)typep) -> getReads( ) . end( );
		      ivi ++ )
		  (*ivi) = invmaps [I_READ] -> lookup (*ivi);
		break;
	      case I_CONTIG:
		for ( tvi  = ((Contig_t *)typep) -> getReadTiling( ) . begin( );
		      tvi != ((Contig_t *)typep) -> getReadTiling( ) . end( );
		      tvi ++ )
		  tvi -> id = invmaps [I_READ] -> lookup (tvi -> id);
		break;
	      case I_CONTIGLINK:
		idp = ((ContigLink_t *)typep) -> getContigs( );
		if ( idp . first != NULL_ID  ||  idp . second != NULL_ID )
		  {
		    idp . first = invmaps [I_CONTIG] -> lookup (idp . first);
		    idp . second = invmaps [I_CONTIG] -> lookup (idp . second);
		    ((ContigLink_t *)typep) -> setContigs (idp);
		  }
		scp = ((ContigLink_t *)typep) -> getSource( );
		if ( scp . second != NULL_NCODE )
		  {
		    scp . first =
		      invmaps [typemap . lookup (scp . second)] ->
		      lookup (scp . first);
		    ((ContigLink_t *)typep) -> setSource (scp);
		  }
		break;
	      case I_CONTIGEDGE:
		for ( ivi  = ((ContigEdge_t *)typep) ->
			getContigLinks( ) . begin( );
		      ivi != ((ContigEdge_t *)typep) ->
			getContigLinks( ) . end( );
		      ivi ++ )
		  (*ivi) = invmaps [I_CONTIGLINK] -> lookup (*ivi);
		break;
	      case I_SCAFFOLD:
		for ( tvi  = ((Scaffold_t *)typep) ->
			getContigTiling( ) . begin( );
		      tvi != ((Scaffold_t *)typep) ->
			getContigTiling( ) . end( );
		      tvi ++ )
		  tvi -> id = invmaps [I_CONTIG] -> lookup (tvi -> id);
		for ( ivi  = ((Scaffold_t *)typep) ->
			getContigEdges( ) . begin( );
		      ivi != ((Scaffold_t *)typep) ->
			getContigEdges( ) . end( );
		      ivi ++ )
		  (*ivi) = invmaps [I_CONTIGEDGE] -> lookup (*ivi);
		break;
	      default:
		assert (false);
	      }
	  }
	  catch (Exception_t & e) {
	    //-- If failed to resolve a link, ignore object
	    cerr << "WARNING: " << e . what( ) << endl
		 << "  could not resolve iid links in "
		 << Decode (typep -> getNCode( )) << " with iid:"
		 << typep -> getIID( ) << ", object ignored\n";
	    continue;
	  }

	  clocka = clock( );
	  typep -> writeMessage (msg);
	  clockb = clock( );
	  loopa += (double)(clockb - clocka);
	  clocka = clock( );
	  msg . write (msgfile);
	  clockb = clock( );
	  loopb += (double)(clockb - clocka);
	  cntw ++;
	}

      //-- Close the bank, only its inverted ID map are needed now
      bankp -> close( );
    }


  //-- Close and free the objects
  msgfile . close( );
  for ( ID_t i = 1; i < I_MAX; i ++ )
    {
      delete invmaps [i];
      delete types [i];
    }
  delete bankp;
  }
  catch (Exception_t & e) {

  //-- On error, print debugging information
  cerr << "Objects seen: " << cnts << endl
       << "Objects written: " << cntw << endl
       << "ERROR: -- Fatal AMOS Exception --\n" << e;
  return EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH


  //-- Output the end time
  cerr << "Objects seen: " << cnts << endl
       << "Objects written: " << cntw << endl
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

  while ( !errflg && ((ch = getopt (argc, argv, "b:cfhm:")) != EOF) )
    switch (ch)
      {
      case 'b':
        OPT_BankName = optarg;
        break;
      case 'c':
	OPT_Concat = true;
	break;
      case 'f':
	OPT_Truncate = true;
	break;
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

  if (OPT_Concat  &&  OPT_Truncate)
    {
      cerr << "ERROR: The -c and -f options exclude each other\n";
      errflg ++;
    }

  if (access (OPT_BankName . c_str( ), R_OK|W_OK|X_OK) )
    {
      cerr << "ERROR: Bank path is not accessible\n";
      errflg ++;
    }

  if (!OPT_Concat  &&  !OPT_Truncate  &&
      !access (OPT_MessageName . c_str( ), F_OK) )
    {
      cerr << "ERROR: Message path already exists\n";
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
    << "-b path       The directory path of the banks to report\n"
    << "-c            Concat report to an existing message\n"
    << "-f            Force new report by truncating existing file\n"
    << "-h            Display help information\n"
    << "-m path       The file path of the message to generate\n\n";

  cerr
    << "Takes an AMOS bank directory and message file path as input. Reports\n"
    << "the information contained in the bank to a message file. IID link\n"
    << "information in the banks will be automatically translated to EID link\n"
    << "information, so all links in the message will refer to EIDs. To alter\n"
    << "the contents of a bank, please use the bank-transact utility.\n\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  -b <bank path>  -m <message path>\n\n";
  return;
}
