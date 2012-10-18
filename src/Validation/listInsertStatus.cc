#include "foundation_AMOS.hh"
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <map>
#include <cmath>
#include <set>

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

bool   OPT_PrintHappy       = false;
bool   OPT_PrintExpanded    = false;
bool   OPT_PrintCompressed  = false;
bool   OPT_PrintMisOriented = false;
bool   OPT_PrintLinking     = false;
bool   OPT_PrintSingleton   = false;
bool   OPT_PrintUnmated     = false;
bool   OPT_PrintUnknown     = false;



void ParseArgs (int argc, char ** argv);
void PrintHelp (const char * s);
void PrintUsage (const char * s);

set<char> printTypes;

void processInserts()
{
  map <string, InsertStats *>::iterator fi;
  vector<Insert *>::iterator i;
  
  for (i =  m_inserts.begin();
       i != m_inserts.end();
       i++)
  {
    if (printTypes.find((*i)->m_state) != printTypes.end())
    {
      float lo = (*i)->m_dist.mean - (Insert::MAXSTDEV*(*i)->m_dist.sd);
      float hi = (*i)->m_dist.mean + (Insert::MAXSTDEV*(*i)->m_dist.sd);

      cout << (char)(*i)->m_state << "\t"
           << m_datastore->read_bank.lookupEID((*i)->m_aid) << "\t"
           << m_datastore->read_bank.lookupEID((*i)->m_bid) << "\t"
           << (*i)->m_acontig << "\t"
           << (*i)->m_bcontig << "\t"
           << (*i)->m_length  << "\t"
           << (*i)->m_libid   << "\t"
           << lo << "\t"
           << hi << "\t" << endl;
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

  while ( !errflg && ((ch = getopt (argc, argv, "hsvHECOLSNU")) != EOF) )
    switch (ch)
      {
      case 'h': PrintHelp (argv[0]); exit (EXIT_SUCCESS); break;
      case 's': OPT_BankSpy = true; break;
      case 'v': PrintBankVersion (argv[0]); exit (EXIT_SUCCESS); break;

      case 'H': OPT_PrintHappy       = true; break;
      case 'E': OPT_PrintExpanded    = true; break;
      case 'C': OPT_PrintCompressed  = true; break;
      case 'O': OPT_PrintMisOriented = true; break;
      case 'L': OPT_PrintLinking     = true; break;
      case 'S': OPT_PrintSingleton   = true; break;
      case 'N': OPT_PrintUnmated     = true; break;
      case 'U': OPT_PrintUnknown     = true; break;

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

  if (!OPT_PrintHappy       &&
      !OPT_PrintExpanded    &&
      !OPT_PrintCompressed  &&
      !OPT_PrintMisOriented &&
      !OPT_PrintLinking     &&
      !OPT_PrintSingleton   &&
      !OPT_PrintUnmated     &&
      !OPT_PrintUnknown)
  {
    OPT_PrintHappy       = true;
    OPT_PrintExpanded    = true;
    OPT_PrintCompressed  = true;
    OPT_PrintMisOriented = true;
    OPT_PrintLinking     = true;
    OPT_PrintSingleton   = true;
    OPT_PrintUnmated     = true;
    OPT_PrintUnknown     = true;
  }

  if (OPT_PrintHappy)       { printTypes.insert(Insert::Happy); }
  if (OPT_PrintExpanded)    { printTypes.insert(Insert::ExpandedMate); } 
  if (OPT_PrintCompressed)  { printTypes.insert(Insert::CompressedMate); } 
  if (OPT_PrintMisOriented) { printTypes.insert(Insert::OrientationViolation); }  
  if (OPT_PrintLinking)     { printTypes.insert(Insert::LinkingMate); }
  if (OPT_PrintSingleton)   { printTypes.insert(Insert::SingletonMate); }
  if (OPT_PrintUnmated)     { printTypes.insert(Insert::NoMate); }
  if (OPT_PrintUnknown)     { printTypes.insert(Insert::Unknown); }
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "Print the status of inserts in contigs and scaffolds.\n"
    << "If no print types are selected, all insert types are printed\n"
    << "Otherwise, just the requested insert types are printed\n"
    << "Output:\n"
    << "Status read1 read2 contig1 contig2 insert_length libid lib_lo lib_hi\n"
    << "\n"
    << "Options:\n"
    << "-h  Display help information\n"
    << "-s  Disregard bank locks and write permissions (spy mode)\n"
    << "-v  Display the compatible bank version\n"
    << "-H  Print happy mates\n"
    << "-E  Print expanded mates\n"
    << "-C  Print compressed mates\n"
    << "-O  Print misoriented mates\n"
    << "-L  Print linking mates\n"
    << "-S  Print singleton mates\n"
    << "-N  Print unmated mates\n"
    << "-U  Print unknown mates\n"
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
