////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 03/08/2004
//!
//! \brief Dumps the reads in a bank by IID to stdout w/ clear range trimmed
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include <iostream>
#include <unistd.h>
using namespace std;
using namespace AMOS;


//=============================================================== Globals ====//
string OPT_BankName;                 // bank name parameter
bool   OPT_BankSpy = false;          // read or read-only spy
bool   OPT_UseEIDs = false;          // print EIDs instead of IIDs
bool   OPT_UseRaw  = false;          // pull entire seqlen
bool   OPT_DumpQual = false;         // dump qualities

string OPT_IIDFile;  // Filename of IIDs to dump
string OPT_EIDFile;  // Filename of EIDs to dump

long int cnts = 0;             // seen object count
long int cntw = 0;             // written object count

int OPT_basesperline = 70;



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


void DumpQuals(ostream & out, const string & quals, int lineLen)
{
  int len = quals.length();
  char buffer[16];

  for (int i = 0; i < len; i++)
  {
    // Print each number followed by a character
    // '\n' if last number in that row or final element
    // ' ' otherwise

    sprintf (buffer, "%.2d%c", quals[i] - AMOS::MIN_QUALITY,
             ( ( ( i % ( lineLen ) == ( lineLen - 1 ) ) ) || ( i == len - 1 ) ) ? '\n' : ' ' );

    out << buffer;
  }
}


void dumpRead(Read_t & red)
{
  cnts++;

  if ( red . getLength( ) <= 0 )
    {
      cerr << "WARNING: read with IID " << red . getIID( )
           << " has no sequence, skipped\n";
      return;
    }
  if ( ! OPT_UseRaw && red . getClearRange( ) . getLength( ) <= 0 )
    {
      cerr << "WARNING: read with IID " << red . getIID( )
           << " has no clear range sequence, skipped\n";
      return;
    }

  if ( OPT_UseEIDs )
    cout << ">" << red . getEID( ) << endl;
  else
    cout << ">" << red . getIID( ) << endl;

  if (OPT_DumpQual)
  {
    if (OPT_UseRaw) { DumpQuals(cout, red.getQualString(), OPT_basesperline); }
    else            { DumpQuals(cout, red.getQualString(red.getClearRange()), OPT_basesperline); }
  }
  else
  {
    if (OPT_UseRaw) { WrapString (cout, red . getSeqString( ), OPT_basesperline); }
    else            { WrapString (cout, red . getSeqString(red . getClearRange( )), OPT_basesperline); }
  }

  cntw++;
}



//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  int exitcode = EXIT_SUCCESS;
  BankStream_t red_bank (Read_t::NCODE);
  Read_t red;

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    if ( OPT_BankSpy )
      red_bank . open (OPT_BankName, B_SPY);
    else
      red_bank . open (OPT_BankName, B_READ);

    ifstream file;
    string id;

    if (!OPT_EIDFile.empty())
    {
      // Dump just the specified list of EIDs
      file.open(OPT_EIDFile.c_str());

      if (!file)
      {
        throw Exception_t("Couldn't open EID File",
                          __LINE__, __FILE__);
      }

      while (file >> id)
      {
        red_bank.seekg(red_bank.getIDMap().lookupBID(id));
        red_bank >> red;
        dumpRead(red);
      }
    }
    else if (!OPT_IIDFile.empty())
    {
      // Dump just the specified list of EIDs
      file.open(OPT_IIDFile.c_str());

      if (!file)
      {
        throw Exception_t("Couldn't open IID File",
                          __LINE__, __FILE__);
      }

      while (file >> id)
      {
        red_bank.seekg(red_bank.getIDMap().lookupBID(atoi(id.c_str())));
        red_bank >> red;
        dumpRead(red);
      }
    }
    else
    {
      //-- Iterate through each object in the bank
      while ( red_bank >> red )
      {
        dumpRead(red);
      }
    }
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH


  cerr << "Objects seen: " << cnts << endl
       << "Objects written: " << cntw << endl;

  return exitcode;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "ehrsvqE:I:L:")) != EOF) )
    switch (ch)
      {
      case 'e':
	OPT_UseEIDs = true;
	break;

      case 'E':
      OPT_EIDFile = optarg;
      break;

      case 'I':
      OPT_IIDFile = optarg;
      break;

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'r':
        OPT_UseRaw = true;
        break;

      case 'q':
        OPT_DumpQual = true;
        OPT_basesperline = 17;
        break;

      case 'L':
        OPT_basesperline = atoi(optarg);
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

  if ( errflg > 0 || optind != argc - 1 )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
    }

  OPT_BankName = argv [optind ++];
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-e            Use EIDs for FastA header instead of IIDs\n"
    << "-h            Display help information\n"
    << "-r            Ignore clear range and dump entire sequence\n"
    << "-q            Dump qualities in fasta format instead of sequence\n"
    << "-s            Disregard bank locks and write permissions (spy mode)\n"
    << "-v            Display the compatible bank version\n"
    << "-E file       Dump just the eids listed in file\n"
    << "-I file       Dump just the iids listed in file\n"
    << "-L num        Set the maximum number of bases per line (Default: 70)\n"
    << endl;
  
  cerr
    << "Takes an AMOS bank directory and dumps all contained reads listed by\n"
    << "IID to stdout (clear range sequence only)\n\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
  return;
}
