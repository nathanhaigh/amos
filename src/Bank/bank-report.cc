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
////////////////////////////////////////////////////////////////////////////////

#include "amp.hh"
#include "IDMap_AMOS.hh"
#include "messages_AMOS.hh"
#include "universals_AMOS.hh"
#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
#include <vector>
#include <map>
#include <sstream>
#include <unistd.h>
using namespace AMOS;
using namespace Message_k;
using namespace std;


//=============================================================== Globals ====//
string  OPT_BankName;                        // bank name parameter
string  OPT_ExtractName;                     // extract by tag file
bool    OPT_UseIIDs = false;                 // include IIDs in the output
bool    OPT_IsExtractCodes = false;          // extract codes or not
bool    OPT_IsExtractIIDs = false;           // extract iids or not
bool    OPT_IsExtractEIDs = false;           // extract eids or not
bool    OPT_IsExtractComments = false;       // extract comments or not

IDMap_t OPT_ExtractCodes(1000);              // extract message type map
IDMap_t OPT_ExtractIDs (1000);               // extract message iid map
map<string, bool> OPT_ExtractComments;       // extract message comment map



//========================================================== Fuction Decs ====//
//----------------------------------------------------- ParseExtract -----------
//! \brief Parses the extract file and loads the appropriate extract map
//!
//! \return void
//!
void ParseExtract ( );


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
  Message_t msg;                 // the current message
  IDMap_t typemap(1000);         // NCode to index mapping
  ID_t ti;                       // type index
  Universal_t * typep;           // type pointer
  Bank_t * bankp = NULL;         // bank pointer

  long int cnts = 0;             // seen object count
  long int cntw = 0;             // written object count

  ostringstream ss;
  unsigned int nl;
  bool showrecord;
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

      //-- If this Bank was not requested, skip it
      if ( OPT_IsExtractCodes  &&
	   ! OPT_ExtractCodes . exists (bankp -> getBankCode( )) )
	continue;

      //-- Iterate through each object in the bank, if requested
      if ( ! OPT_IsExtractCodes  ||
	   OPT_ExtractCodes . exists (bankp -> getBankCode( )) )
      for ( id = 1; id <= bankp -> getLastIID( ); id ++ )
	{
	  //-- If extracting by ID, skip if not selected
	  if ( (OPT_IsExtractIIDs  &&
		! OPT_ExtractIDs . exists (id))
	       ||
	       (OPT_IsExtractEIDs  &&
		(! invmaps [ti] -> exists (id)  ||
		 ! OPT_ExtractIDs . exists (invmaps [ti] -> lookup (id)))) )
	    continue;

	  //-- Fetch the next object
	  typep -> setIID (id);
	  bankp -> fetch (*typep);
	  if ( typep -> isRemoved( ) )
	    continue;
	  cnts ++;

	  //-- Translate the ID pointers from IID to EID
	  if ( !OPT_UseIIDs )
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

	  showrecord = true;
	  if ( OPT_IsExtractComments )
	    {
	      nl = typep -> getComment( ) . find ('\n');
	      string temps (typep -> getComment( ), 0, nl);
	      if ( OPT_ExtractComments . find (temps) ==
		   OPT_ExtractComments . end( ) )
		showrecord = false;
	    }

	  if ( showrecord )
	    {
	      typep -> writeMessage (msg);
	      if ( OPT_UseIIDs )
		{
		  ss . str("");
		  ss << typep -> getIID( );
		  msg . setField (F_IID, ss . str( ));
		  msg . removeField (F_EID);
		}
	      msg . write (cout);
	      cntw ++;
	    }
	}

      //-- Close the bank, only its inverted ID map are needed now
      bankp -> close( );
    }


  //-- Close and free the objects
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

  return EXIT_SUCCESS;
}




//----------------------------------------------------- ParseExtract -----------
void ParseExtract ( )
{
  ifstream efile (OPT_ExtractName . c_str( ));
  string str;
  ID_t id;

  if ( !efile . good( ) )
    {
      cerr << "WARNING: Could not open " << OPT_ExtractName << endl;
      return;
    }

  if ( OPT_IsExtractComments )
    {
      while ( true )
	{
	  getline (efile, str);
	  if ( !efile . good( ) )
	    break;

	  if ( !(OPT_ExtractComments .
		 insert (map<string,bool>::value_type(str,true))).second )
	    cerr << "WARNING: Cannot insert multiple map keys - Comment "
		 << str << " ignored" << endl;
	}
    }
  else if ( OPT_IsExtractIIDs )
    {
      while ( true )
	{
	  efile >> id;
	  if ( !efile . good( ) )
	    break;

	  try {
	    OPT_ExtractIDs . insert (id, 1);
	  }
	  catch (Exception_t & e) {
	    cerr << "WARNING: " << e . what( )
		 << " - IID " << id << " ignored" << endl;
	  }
	}
    }
  else if ( OPT_IsExtractEIDs )
    {
      while ( true )
	{
	  efile >> id;
	  if ( !efile . good( ) )
	    break;

	  try {
	    OPT_ExtractIDs . insert (id, 1);
	  }
	  catch (Exception_t & e) {
	    cerr << "WARNING: " << e . what( )
		 << " - EID " << id << " ignored" << endl;
	  }
	}
    }
  else
    assert (false);

  efile . close( );
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "b:c:e:hi:I")) != EOF) )
    switch (ch)
      {
      case 'b':
        OPT_BankName = optarg;
        break;

      case 'c':
	OPT_ExtractName = optarg;
	OPT_IsExtractComments = true;
	break;

      case 'e':
	OPT_ExtractName = optarg;
	OPT_IsExtractEIDs = true;
	break;

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'i':
	OPT_ExtractName = optarg;
	OPT_IsExtractIIDs = true;
	break;

      case 'I':
	OPT_UseIIDs = true;
	break;

      default:
        errflg ++;
      }

  if ( OPT_BankName . empty( ) )
    {
      cerr << "ERROR: The -b option is mandatory\n";
      errflg ++;
    }

  if ( access (OPT_BankName . c_str( ), R_OK|W_OK|X_OK) )
    {
      cerr << "ERROR: Bank path is not accessible\n";
      errflg ++;
    }

  if (
      (OPT_IsExtractComments  &&  OPT_IsExtractEIDs)  ||
      (OPT_IsExtractComments  &&  OPT_IsExtractIIDs)  ||
      (OPT_IsExtractEIDs      &&  OPT_IsExtractIIDs)
      )
    {
      cerr << "ERROR: -c -e -i are mutually exlusive\n";
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
	{
	  try {
	    OPT_ExtractCodes . insert (Encode (argv [optind ++]), 1);
	  }
	  catch (Exception_t & e) {  
	    cerr << "WARNING: " << e . what( )
		 << " - NCode " << argv [optind - 1] << " ignored" << endl;
	  }
	}
    }

  if ( ! OPT_ExtractName . empty( ) )
    ParseExtract( );
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-b path       The directory path of the bank to report\n"
    << "-c file       Report objects matching comments listed in file\n"
    << "-e file       Report objects matching EIDs listed in file\n"
    << "-h            Display help information\n"
    << "-i file       Report objects matching IIDs listed in file\n"
    << "-I            Use IIDs for links and IDs instead of EIDs\n"
    << endl;

  cerr
    << "Takes an AMOS bank directory as input. Will output the information\n"
    << "contained in the bank in the form of an AMOS message to stdout. IID\n"
    << "link information in the banks will be automatically translated to EID\n"
    << "link information, so all links in the message will refer to EIDs. If\n"
    << "no NCodes are listed on the command line, all object types will be\n"
    << "reported. The -c -e -i options are mutually exlusive; their files\n"
    << "should contain a list of tags, one per line. This utility only\n"
    << "outputs bank information; To alter the contents of a bank, please use\n"
    << "the bank-transact utility.\n\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s
    << "  [options]  -b <bank path>  [NCodes]\n\n";
  return;
}
