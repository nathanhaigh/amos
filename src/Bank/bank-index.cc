////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 01/10/2005
//!
//! \brief Cleans a bank of it's deleted records and stale file locks
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include "amp.hh"
using namespace AMOS;
using namespace std;




//=============================================================== Globals ====//
string  OPT_BankName;                        // bank name parameter


//========================================================== Fuction Decs ====//
//----------------------------------------------------- BuildContigToFeature ---
//! \brief Builds the contig to feature index
//!
//! \return Success or failure
//!
bool BuildContigToFeature (Bank_t & idx_bank);


//----------------------------------------------------- BuildContigToScaffold --
//! \brief Builds the contig to scaffold index
//!
//! \return Success or failure
//!
bool BuildContigToScaffold (Bank_t & idx_bank);


//----------------------------------------------------- BuildReadToContig ------
//! \brief Builds the read to contig index
//!
//! \return Success or failure
//!
bool BuildReadToContig (Bank_t & idx_bank);


//----------------------------------------------------- BuildReadToLibrary -----
//! \brief Builds the read to library index
//!
//! \return Success or failure
//!
bool BuildReadToLibrary (Bank_t & idx_bank);


//----------------------------------------------------- BuildReadToMate --------
//! \brief Builds the read to mate index
//!
//! \return Success or failure
//!
bool BuildReadToMate (Bank_t & idx_bank);


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
  Bank_t idx_bank (Index_t::NCODE);
  int exitcode = EXIT_SUCCESS;

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- Output the current time and bank directory
  cerr << "START DATE: " << Date( ) << endl;
  cerr << "Bank is: " << OPT_BankName << endl;

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {
    if ( ! idx_bank . exists (OPT_BankName) )
      idx_bank . create (OPT_BankName);
    else
      idx_bank . open (OPT_BankName);
    
    //-- Build the indices
    exitcode = BuildContigToFeature (idx_bank) && exitcode;
    exitcode = BuildContigToScaffold (idx_bank) && exitcode;
    exitcode = BuildReadToContig (idx_bank) && exitcode;
    exitcode = BuildReadToLibrary (idx_bank) && exitcode;
    exitcode = BuildReadToMate (idx_bank) && exitcode;
   
    idx_bank . close( );
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH


  //-- Output the end time
  cerr << "END DATE:   " << Date( ) << endl;

  return exitcode;
}




//----------------------------------------------------- BuildContigToFeature ---
bool BuildContigToFeature (Bank_t & idx_bank)
{
  Index_t idx (Contig_t::NCODE, Feature_t::NCODE);
  idx . setEID (Index_t::CONTIG_TO_FEATURE);

  Feature_t fea;

  BankStream_t fea_bank (Feature_t::NCODE);

  try {
    fea_bank . open (OPT_BankName, B_READ);

    while ( fea_bank >> fea )
      if ( fea . getSource( ) . second == Contig_t::NCODE )
        idx . insert (fea . getSource( ) . first, fea . getIID( ));

    fea_bank . close( );

    idx_bank . append (idx);
  }
  catch (const Exception_t & e) {
    cerr << "ERROR: " << e . what( ) << endl
         << "  could not create contig-to-feature index" << endl;
    return false;
  }
  return true;
}




//----------------------------------------------------- BuildContigToScaffold --
bool BuildContigToScaffold (Bank_t & idx_bank)
{
  Index_t idx (Contig_t::NCODE, Scaffold_t::NCODE);
  idx . setEID (Index_t::CONTIG_TO_SCAFFOLD);

  Scaffold_t scf;
  vector<Tile_t>::const_iterator ti;

  BankStream_t scf_bank (Scaffold_t::NCODE);

  try {
    scf_bank . open (OPT_BankName, B_READ);

    while ( scf_bank >> scf )
      for ( ti  = scf . getContigTiling( ) . begin( );
            ti != scf . getContigTiling( ) . end( ); ++ ti )
        idx . insert (ti -> source, scf . getIID( ));

    scf_bank . close( );

    idx_bank . append (idx);
  }
  catch (const Exception_t & e) {
    cerr << "ERROR: " << e . what( ) << endl
         << "  could not create contig-to-scaffold index" << endl;
    return false;
  }
  return true;
}




//----------------------------------------------------- BuildReadToContig ------
bool BuildReadToContig (Bank_t & idx_bank)
{
  Index_t idx (Read_t::NCODE, Contig_t::NCODE);
  idx . setEID (Index_t::READ_TO_CONTIG);

  Contig_t ctg;
  vector<Tile_t>::const_iterator ti;

  BankStream_t ctg_bank (Contig_t::NCODE);

  try {
    ctg_bank . open (OPT_BankName, B_READ);

    while ( ctg_bank >> ctg )
      for ( ti  = ctg . getReadTiling( ) . begin( );
            ti != ctg . getReadTiling( ) . end( ); ++ ti )
        idx . insert (ti -> source, ctg . getIID( ));

    ctg_bank . close( );

    idx_bank . append (idx);
  }
  catch (const Exception_t & e) {
    cerr << "ERROR: " << e . what( ) << endl
         << "  could not create read-to-contig index" << endl;
    return false;
  }
  return true;
}




//----------------------------------------------------- BuildReadToLibrary -----
bool BuildReadToLibrary (Bank_t & idx_bank)
{
  Index_t idx (Read_t::NCODE, Library_t::NCODE);
  idx . setEID (Index_t::READ_TO_LIBRARY);

  Read_t red;
  Fragment_t frg;

  BankStream_t red_bank (Read_t::NCODE);
  Bank_t frg_bank (Fragment_t::NCODE);

  try {
    red_bank . open (OPT_BankName, B_READ);
    frg_bank . open (OPT_BankName, B_READ);

    while ( red_bank >> red )
      {
        frg_bank . fetch (red . getFragment( ), frg);
        idx . insert (red . getIID( ), frg . getLibrary( ));
      }

    red_bank . close( );
    frg_bank . close( );

    idx_bank . append (idx);
  }
  catch (const Exception_t & e) {
    cerr << "ERROR: " << e . what( ) << endl
         << "  could not create read-to-library index" << endl;
    return false;
  }
  return true;
}




//----------------------------------------------------- BuildReadToMate --------
bool BuildReadToMate (Bank_t & idx_bank)
{
  Index_t idx (Read_t::NCODE, Read_t::NCODE);
  idx . setEID (Index_t::READ_TO_MATE);

  Matepair_t mtp;
  pair<ID_t, ID_t> reads;

  BankStream_t mtp_bank (Matepair_t::NCODE);

  try {
    mtp_bank . open (OPT_BankName, B_READ);

    while ( mtp_bank >> mtp )
      {
        reads = mtp . getReads( );
        idx . insert (reads . first, reads . second);
        idx . insert (reads . second, reads . first);
      }

    mtp_bank . close( );

    idx_bank . append (idx);
  }
  catch (const Exception_t & e) {
    cerr << "ERROR: " << e . what( ) << endl
         << "  could not create read-to-mate index" << endl;
    return false;
  }
  return true;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "hv")) != EOF) )
    switch (ch)
      {
      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'v':
        PrintBankVersion (argv[0]);
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

  OPT_BankName = argv [optind ++];

  if ( access (OPT_BankName . c_str( ), R_OK|W_OK|X_OK) )
    {
      cerr << "ERROR: Bank directory is not accessible, "
	   << strerror (errno) << endl;
      exit (EXIT_FAILURE);
    }
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-h            Display help information\n"
    << "-v            Display the compatible bank version\n"
    << endl;
  cerr
    << "Takes an AMOS bank directory as input and builds an expected set of\n"
    << "bank indices. These Index_t objects are used to efficiently jump\n"
    << "between objects with inter-bank relationships, such as Read_t to\n"
    << "containing Contig_t or Read_t to parent Library_t.\n"
    << "  Built indices are RED->CTG, CTG->SCF, RED->LIB, RED->RED for read\n"
    << "to containing contig, contig to containing scaffold, read to parent\n"
    << "library, and read to mated read respectively. One-to-many mappings\n"
    << "are permitted, and indices can be accessed directly from the IDX bank\n"
    << "via EIDs of the form shown above, e.g. \"RED->CTG\", or the static\n"
    << "strings defined in the Index_t type.\n"
    << endl;
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
  return;
}
