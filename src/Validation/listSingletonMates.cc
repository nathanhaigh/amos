#include "foundation_AMOS.hh"
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <map>
#include <cmath>

#include "DataStore.hh"
#include "Insert.hh"
#include "InsertStats.hh"

using namespace std;
using namespace AMOS;

typedef HASHMAP::hash_map<ID_t, Tile_t *> SeqTileMap_t;

int FRAGMENTSTRLEN = 3;
int m_verbose = 0;

DataStore * m_datastore;
int m_connectMates = 1;
typedef std::vector<Insert *> InsertList_t;
InsertList_t m_inserts;

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

void processInserts()
{
  map <string, InsertStats *>::iterator fi;
  vector<Insert *>::iterator i;
  
  for (i =  m_inserts.begin();
       i != m_inserts.end();
       i++)
  {
    if ((*i)->m_state == Insert::SingletonMate || (*i)->m_state == Insert::LinkingMate)
    {
      cout << (*i)->m_acontig << " "
           << m_datastore->read_bank.lookupEID((*i)->m_aid) << " "
           << m_datastore->read_bank.lookupEID((*i)->m_bid) << " "
           << (char)(*i)->m_state << endl;
    }

    delete (*i);
  }

  m_inserts.clear();
}




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


    map <string, InsertStats *>::iterator fi;

    if (m_datastore->scaffold_bank.isOpen())
    {
      cerr << "Processing scaffolds... ";

      m_datastore->scaffold_bank.seekg(1);
      while (m_datastore->scaffold_bank >> scaff)
      {
        vector <Tile_t> rtiling;
        m_datastore->mapReadsToScaffold(scaff, rtiling, 1);
        m_datastore->calculateInserts(rtiling, m_inserts, m_connectMates, 1);
        processInserts();
      }
    }
    else
    {
      cerr << "Processing contigs... ";

      m_datastore->contig_bank.seekg(1);
      while (m_datastore->contig_bank >> contig)
      {
        m_datastore->calculateInserts(contig.getReadTiling(), m_inserts, m_connectMates, 1);
        processInserts();
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

  while ( !errflg && ((ch = getopt (argc, argv, "hsdvf:")) != EOF) )
    switch (ch)
      {
      case 'h': PrintHelp (argv[0]); exit (EXIT_SUCCESS); break;

      case 's': OPT_BankSpy = true; break;

      case 'v': PrintBankVersion (argv[0]); exit (EXIT_SUCCESS); break;

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
    << "Output:\n"
    << "contigiid placedread missingmate type\n"
    << "\n"
    << "Options:\n"
    << "-h            Display help information\n"
    << "-s            Disregard bank locks and write permissions (spy mode)\n"
    << "-v            Display the compatible bank version\n"
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
