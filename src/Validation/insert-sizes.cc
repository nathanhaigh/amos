#include "foundation_AMOS.hh"
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <map>
#include <set>
#include <cmath>
#include "amp.hh"

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
int    OPT_PROCESSSCAFFOLDS = true;
int    OPT_COVERAGE = false;
int    OPT_UNPLACED = false;


DataStore * m_datastore;
int m_connectMates = 1;
typedef std::vector<Insert *> InsertList_t;
map <string, InsertStats *> libstats;


void ParseArgs (int argc, char ** argv);
void PrintHelp (const char * s);
void PrintUsage (const char * s);






void processInserts(const string & eid, 
                    InsertList_t & m_inserts, 
                    vector<Tile_t> & tiling)
{
  vector<Insert *>::iterator i = m_inserts.begin();
    
  if (OPT_COVERAGE)
  {
    multimap<int, int> read_endpoints;
    multimap<int, int>::iterator ep;

    multimap<int, Insert *> insert_endpoints;
    multimap<int, Insert *>::iterator iep;
    
    sort(tiling.begin(), tiling.end(), TileOrderCmp());

    int max = 0;
    vector<Tile_t>::iterator ti;
    for (ti = tiling.begin(); ti != tiling.end(); ti++)
    {
      if (ti->getRightOffset() > max) { max = ti->getRightOffset(); }
    }

    cerr << "Printing coverage for " << max << " bases" << endl;
    ProgressDots_t dots(max, 50);

    ti = tiling.begin();
    for (int gindex = 0; gindex <= max; gindex++)
    {
      ep = read_endpoints.begin();
      while ((ep != read_endpoints.end()) && (ep->first < gindex))
      {
        read_endpoints.erase(ep); ep = read_endpoints.begin();
      }

      while (ti != tiling.end() && ti->offset <= gindex)
      {
        read_endpoints.insert(make_pair(ti->getRightOffset(), ti->source));
        ti++;
      }

      iep = insert_endpoints.begin();
      while ((iep != insert_endpoints.end()) && (iep->first < gindex))
      {
        insert_endpoints.erase(iep); iep = insert_endpoints.begin();
      }

      while (i != m_inserts.end() && (*i)->m_loffset <= gindex)
      {
        if ((*i)->reasonablyConnected())
        {
          insert_endpoints.insert(make_pair((*i)->m_roffset, *i));
        }
        i++;
      }

      {
        int rcov = read_endpoints.size();
        int icov = insert_endpoints.size();

        cout << eid << "\t" << gindex << "\t" << rcov << "\t" << icov << "\t";

        if (0)
        {
          for (ep = read_endpoints.begin();  ep != read_endpoints.end(); ep++)
          {
            cout << ":" << m_datastore->read_bank.lookupEID(ep->second);
          }
        }

        for (iep = insert_endpoints.begin();  iep != insert_endpoints.end(); iep++)
        {
          cout << " (" << iep->second->m_actual << "," << iep->second->m_libid << ")";
        }

        cout << endl;
      }

      dots.update(gindex);
    }
    dots.end();
  }
  else
  {
    map <string, InsertStats *>::iterator fi;
    
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

  InsertList_t m_inserts;


  Scaffold_t scaff;
  Contig_t contig;

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    m_datastore = new DataStore();
    m_datastore->openBank(OPT_BankName);


    map <string, InsertStats *>::iterator fi;

    if (OPT_PROCESSSCAFFOLDS && m_datastore->scaffold_bank.isOpen())
    {
      cerr << "Processing scaffolds..." << endl;

      set<ID_t> seencontigs;

      m_datastore->scaffold_bank.seekg(1);
      while (m_datastore->scaffold_bank >> scaff)
      {
        vector <Tile_t> rtiling;
        m_datastore->mapReadsToScaffold(scaff, rtiling, 1);
        m_datastore->calculateInserts(rtiling, m_inserts, m_connectMates, 0);
        processInserts(scaff.getEID(), m_inserts, rtiling);

        vector<Tile_t>::iterator ci;
        for (ci = scaff.getContigTiling().begin();
             ci != scaff.getContigTiling().end();
             ci++)
        {
          seencontigs.insert(ci->source);
        }
      }

      if (OPT_UNPLACED)
      {
        cerr << "Processing unplaced contigs..." << endl;
        m_datastore->contig_bank.seekg(1);

        set<ID_t>::iterator si;

        while (m_datastore->contig_bank >> contig)
        {
          si = seencontigs.find(contig.getIID());

          if (si == seencontigs.end())
          {
            m_datastore->calculateInserts(contig.getReadTiling(), m_inserts, m_connectMates, 0);
            processInserts(contig.getEID(), m_inserts, contig.getReadTiling());
          }
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
        processInserts(contig.getEID(), m_inserts, contig.getReadTiling());
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

  while ( !errflg && ((ch = getopt (argc, argv, "Shsdvf:cCU")) != EOF) )
  {
    switch (ch)
    {
      case 'h': PrintHelp (argv[0]); exit (EXIT_SUCCESS); break;
      case 'v': PrintBankVersion (argv[0]); exit (EXIT_SUCCESS); break;
      case 's': OPT_BankSpy = true; break;
      case 'd': OPT_DETAILS = true; break;
      case 'S': OPT_PRINTSUMMARY = false; break;
      case 'f': OPT_SUBLIBRARYLEN = atoi(optarg); break;
      case 'c': OPT_PROCESSSCAFFOLDS = false; break;
      case 'C': OPT_COVERAGE = true; OPT_PRINTSUMMARY = false; break;
      case 'U': OPT_UNPLACED = true; break;

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
    << "-c            Don't use scaffold information\n"
    << "-C            Print coverage information\n"
    << "-U            Also process contigs not placed into a scaffold\n"
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
    << "\n"
    << "In Coverage mode (-C) also print read, insert coverage:\n"
    << "eid, pos, read_coverage, insert_coverage, list of (insert size, library) pairs\n"
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
