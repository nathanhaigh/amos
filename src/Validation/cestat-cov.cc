////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 03/08/2004
//!
//! \brief Dumps a bambus .mates file from an AMOS bank
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <map>
#include <iomanip>

#include "DataStore.hh"
#include "Insert.hh"
#include "CoverageStats.hh"

using namespace std;
using namespace AMOS;

typedef HASHMAP::hash_map<ID_t, Tile_t *> SeqTileMap_t;

DataStore * m_datastore;
int m_connectMates = 1;
typedef std::vector<Insert *> InsertList_t;
InsertList_t m_inserts;



int m_coveragePlot = 0;
int m_cestats = 1;

void printCEStats(Scaffold_t & scaff)
{
  typedef map<ID_t, CoverageStats> LibStats;
  LibStats::iterator li;

  LibStats libStats = m_datastore->computeCEStats(m_inserts);


  for (li = libStats.begin(); li != libStats.end(); li++)
  {
    cout << ">" << scaff.getEID() << " lib:" << li->first << endl;
    // This are stored as oldvalue newvalue so skip every other one
    for (int i = 1; i < li->second.m_curpos; i+=2)
    {
      cout << setprecision(10) << li->second.m_coverage[i].x() << " "
           << setprecision(6)  << li->second.m_cestat[i] << endl;
    }
  }
}


//=============================================================== Globals ====//
string OPT_BankName;                 // bank name parameter
bool   OPT_BankSpy = false;          // read or read-only spy


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
  Read_t red;
  Fragment_t frg;
  Library_t lib;


  Scaffold_t scaff;
  Contig_t contig;

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    m_datastore = new DataStore();
    m_datastore->openBank(OPT_BankName);

    cerr << "Processing scaffolds... ";
    int scaffcount = 0;
    m_datastore->scaffold_bank.seekg(1);
    while (m_datastore->scaffold_bank >> scaff)
    {
      scaffcount++;
      vector <Tile_t> rtiling;

      m_datastore->mapReadsToScaffold(scaff, rtiling, 1);
      m_datastore->calculateInserts(rtiling, m_inserts, m_connectMates, 1);

      printCEStats(scaff);

      vector<Insert *>::iterator i;
      
      for (i =  m_inserts.begin();
           i != m_inserts.end();
           i++)
      {
        delete (*i);
      }

      m_inserts.clear();
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

  while ( !errflg && ((ch = getopt (argc, argv, "hsv")) != EOF) )
    switch (ch)
      {
      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
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

  if (errflg > 0 || optind != argc - 1)
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
    << "-h            Display help information\n"
    << "-s            Disregard bank locks and write permissions (spy mode)\n"
    << "-v            Display the compatible bank version\n"
    << endl;
  cerr
    << "Print the CE-statistic value at the beginning and end of each"
    << "insert across each scaffold separated by library\n\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
  return;
}
