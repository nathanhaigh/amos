#include "foundation_AMOS.hh"
#include <iostream>
#include <unistd.h>
using namespace std;
using namespace AMOS;


//=============================================================== Globals ====//
string OPT_BankName;     
bool   OPT_BankSpy = false;
bool   OPT_UseEIDs = false;
bool   OPT_DumpContigs = false;
bool   OPT_UseRaw = false;


string OPT_IIDFile;  // Filename of IIDs to dump
string OPT_EIDFile;  // Filename of EIDs to dump

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


void dumpRead(Read_t & red)
{
  Range_t rng = red.getClearRange();

  if (OPT_UseRaw)
  {
    rng.begin = 0;
    rng.end = red.getLength();
  }

  if (OPT_UseEIDs)
  {
    printf("%s\t%0.06f\n", red.getEID().c_str(), red.getGCContent(rng));
  }
  else
  {
    printf("%d\t%0.06f\n", red.getIID(), red.getGCContent(rng));
  }
}

void dumpContig(Contig_t & ctg)
{
  if (OPT_UseEIDs)
  {
    printf("%s\t%0.06f\n", ctg.getEID().c_str(), ctg.getGCContent());
  }
  else
  {
    printf("%d\t%0.06f\n", ctg.getIID(), ctg.getGCContent());
  }
}




//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  int exitcode = EXIT_SUCCESS;
  BankStream_t red_bank(Read_t::NCODE);
  BankStream_t ctg_bank(Contig_t::NCODE);
  Read_t red;
  Contig_t ctg;

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try 
  {
    int mode = B_READ;
    if (OPT_BankSpy) { mode = B_SPY; }

    if (OPT_DumpContigs)
    {
      ctg_bank.open(OPT_BankName, mode);
    }
    else
    {
      red_bank.open (OPT_BankName, mode);
    }

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
        if (OPT_DumpContigs)
        {
          ctg_bank.fetch(id, ctg);
          dumpContig(ctg);
        }
        else
        {
          red_bank.fetch(id, red);
          dumpRead(red);
        }
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
        ID_t iid = atoi(id.c_str());
        if (OPT_DumpContigs)
        {
          ctg_bank.fetch(iid, ctg);
          dumpContig(ctg);
        }
        else
        {
          red_bank.fetch(iid, red);
          dumpRead(red);
        }
      }
    }
    else
    {
      //-- Iterate through each object in the bank
      if (OPT_DumpContigs)
      {
        while (ctg_bank >> ctg)
        {
          dumpContig(ctg);
        }
      }
      else
      {
        while (red_bank >> red)
        {
          dumpRead(red);
        }
      }
    }
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH

  return exitcode;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "hsverCE:I:")) != EOF) )
    switch (ch)
      {
      case 'e': OPT_UseEIDs = true;     break;
      case 'E': OPT_EIDFile = optarg;   break;
      case 'I': OPT_IIDFile = optarg;   break;
      case 'r': OPT_UseRaw = true;      break;
      case 'C': OPT_DumpContigs = true; break;

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 's': OPT_BankSpy = true; break;

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

   cerr << "\n.DESCRIPTION.\n"
        << "  Prints the GC Content of reads or contigs to stdout\n"
        << "\n.OPTIONS.\n"
        << "  -h          Display help information\n"
        << "  -s          Disregard bank locks and write permissions (spy mode)\n"
        << "  -v          Display the compatible bank version\n"
        << "  -e          Use EIDs instead of IIDs for identifiers\n"
        << "  -r          Ignore clear range and dump entire sequence\n"
        << "  -C          Dump Contigs instead of reads\n"
        << "  -E file     Dump just the eids listed in file\n"
        << "  -I file     Dump just the iids listed in file\n"
        << "\n.KEYWORDS.\n"
        << "  amos bank, GC content\n"
        << endl;

  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr << "\n.USAGE.\n" << "  " <<  s << "  [options]  -b <bank path>\n";
  return;
}
