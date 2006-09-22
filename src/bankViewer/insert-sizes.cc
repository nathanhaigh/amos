#include "foundation_AMOS.hh"
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <map>
#include <set>
#include <cmath>

#include "DataStore.hh"
#include "Insert.hh"
#include "InsertStats.hh"

using namespace std;
using namespace AMOS;


string OPT_BankName;
bool   OPT_BankSpy = false; 
int    OPT_SUBLIBRARYLEN = 0;
int    OPT_DETAILS = false;
int    OPT_PRINTSUMMARY = true;


DataStore * m_datastore;
int m_connectMates = 1;
typedef std::vector<Insert *> InsertList_t;
InsertList_t m_inserts;
map <string, InsertStats *> libstats;


void ParseArgs (int argc, char ** argv);
void PrintHelp (const char * s);
void PrintUsage (const char * s);

void processInserts()
{
  map <string, InsertStats *>::iterator fi;
  vector<Insert *>::iterator i;
  
  for (i =  m_inserts.begin();
       i != m_inserts.end();
       i++)
  {
    if ((*i)->m_active == 2)
    {
      string fragment;

      if (OPT_SUBLIBRARYLEN == 0)
      {
        char buffer[16];
        sprintf(buffer, "%d", (*i)->m_libid);
        fragment=buffer;
      }
      else
      {
        fragment = m_datastore->read_bank.lookupEID((*i)->m_aid).substr(0,OPT_SUBLIBRARYLEN);
      }

      if (OPT_DETAILS)
      {
        ID_t fragid = m_datastore->lookupFragId((*i)->m_aid);

        cout << (*i)->m_libid << "\t"
             << fragid << "\t"
             << fragment << "\t"
             << m_datastore->read_bank.lookupEID((*i)->m_aid) << "\t" 
             << m_datastore->read_bank.lookupEID((*i)->m_bid) << "\t"
             << (*i)->m_actual << endl;
      }

      if (OPT_PRINTSUMMARY)
      {
        fi = libstats.find(fragment);
        if (fi == libstats.end())
        {
          char buffer[16];
          sprintf(buffer, "%d", (*i)->m_libid);
          fi = libstats.insert(make_pair(fragment, new InsertStats(buffer))).first;
        }

        fi->second->addSize((*i)->m_actual);
      }
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
      cerr << "Processing scaffolds..." << endl;

      set<ID_t> seencontigs;

      m_datastore->scaffold_bank.seekg(1);
      while (m_datastore->scaffold_bank >> scaff)
      {
        vector <Tile_t> rtiling;
        m_datastore->mapReadsToScaffold(scaff, rtiling, 1);
        m_datastore->calculateInserts(rtiling, m_inserts, m_connectMates, 0);
        processInserts();

        vector<Tile_t>::iterator ci;
        for (ci = scaff.getContigTiling().begin();
             ci != scaff.getContigTiling().end();
             ci++)
        {
          seencontigs.insert(ci->source);
        }
      }

      cerr << "Processing unplaced contigs..." << endl;
      m_datastore->contig_bank.seekg(1);

      set<ID_t>::iterator si;

      while (m_datastore->contig_bank >> contig)
      {
        si = seencontigs.find(contig.getIID());

        if (si == seencontigs.end())
        {
          m_datastore->calculateInserts(contig.getReadTiling(), m_inserts, m_connectMates, 0);
          processInserts();
        }
      }
    }
    else
    {
      cerr << "Processing contigs..." << endl;

      m_datastore->contig_bank.seekg(1);
      while (m_datastore->contig_bank >> contig)
      {
        m_datastore->calculateInserts(contig.getReadTiling(), m_inserts, m_connectMates, 0);
        processInserts();
      }
    }

    if (OPT_PRINTSUMMARY)
    {
      cout << "SubLibrary (k=" << OPT_SUBLIBRARYLEN <<") Summaries" << endl;
      for (fi = libstats.begin(); fi != libstats.end(); fi++)
      {
        cout << fi->second->m_label << "\t"
             << fi->first << "\t"
             << fi->second->count() << "\t"
             << fi->second->mean() << "\t"
             << fi->second->stdev() << endl;
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

  while ( !errflg && ((ch = getopt (argc, argv, "Shsdvf:")) != EOF) )
  {
    switch (ch)
    {
      case 'h': PrintHelp (argv[0]); exit (EXIT_SUCCESS); break;
      case 'v': PrintBankVersion (argv[0]); exit (EXIT_SUCCESS); break;
      case 's': OPT_BankSpy = true; break;
      case 'd': OPT_DETAILS = true; break;
      case 'S': OPT_PRINTSUMMARY = false; break;
      case 'f': OPT_SUBLIBRARYLEN = atoi(optarg); break;

      default:
        errflg ++;
    };
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
    << "Options:\n"
    << "-h            Display help information\n"
    << "-s            Disregard bank locks and write permissions (spy mode)\n"
    << "-v            Display the compatible bank version\n"
    << "-f len        Number of characters of seqname to use as sublibrary (default=0)\n"
    << "-d            Show details for each insert\n"
    << "-S            Don't print library summaries\n"
    << "\n"
    << "Print information on the observed sizes of inserts grouped by library\n"
    << "or sublibrary. Both reads have to be in the same scaffold (or unplaced\n"
    << "contig) for the insert to be reported. A sublibrary is defined by the\n"
    << "first k characters of the sequence name. By default, k=0 and it\n"
    << "reports on the whole library, use -f to set a new length.\n"
    << "\n"
    << "By default, only print the SubLibrary summaries:\n"
    << "Libid SubLibrary Count Mean StDev\n"
    << "\n"
    << "Use details mode (-d) to also print the size for all inserts:\n"
    << "Libid FragmentID SubLibrary Read1 Read2 Size\n"
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
