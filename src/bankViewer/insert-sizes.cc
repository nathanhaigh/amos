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
#include <cmath>

#include "DataStore.hh"
#include "Insert.hh"
#include "InsertStats.hh"

using namespace std;
using namespace AMOS;

typedef HASHMAP::hash_map<ID_t, Tile_t *> SeqTileMap_t;

int FRAGMENTSTRLEN = 3;
int m_verbose = 1;

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


    map <string, InsertStats *> fragstats;
    map <string, InsertStats *>::iterator fi;


    cerr << "Processing scaffolds... ";
    int scaffcount = 0;
    m_datastore->scaffold_bank.seekg(1);
    while (m_datastore->scaffold_bank >> scaff)
    {
      scaffcount++;
      vector <Tile_t> rtiling;
      SeqTileMap_t seqtileLookup;

      vector<Tile_t> & ctiling = scaff.getContigTiling();
      vector<Tile_t>::const_iterator ci;

      cerr << "Mapping reads to scaffold " << scaff.getEID() << "... ";

      for (ci = ctiling.begin(); ci != ctiling.end(); ci++)
      {
        m_datastore->fetchContig(ci->source, contig);

        int clen = contig.getLength();

        vector<Tile_t> & crtiling = contig.getReadTiling();
        vector<Tile_t>::const_iterator ri;
        for (ri = crtiling.begin(); ri != crtiling.end(); ri++)
        {
          Tile_t mappedTile;
          mappedTile.source = ri->source;
          mappedTile.gaps   = ri->gaps;
          mappedTile.range  = ri->range;

          int offset = ri->offset;
          if (ci->range.isReverse())
          {
            mappedTile.range.swap();
            offset = clen - (offset + ri->range.getLength() + ri->gaps.size());
          }

          mappedTile.offset = ci->offset + offset;

          rtiling.push_back(mappedTile);
        }
      }

      cerr << rtiling.size() << " reads mapped" << endl;

      m_datastore->calculateInserts(rtiling, m_inserts, m_connectMates, 1);

      vector<Insert *>::iterator i;

      
      for (i =  m_inserts.begin();
           i != m_inserts.end();
           i++)
      {
        if ((*i)->m_active == 2)
        {
          string fragment = m_datastore->read_bank.lookupEID((*i)->m_aid).substr(0,FRAGMENTSTRLEN);

          if (m_verbose)
          {
            cout << (*i)->m_libid << "\t"
                 << fragment << "\t"
                 << m_datastore->read_bank.lookupEID((*i)->m_aid) << "\t" 
                 << m_datastore->read_bank.lookupEID((*i)->m_bid) << "\t"
                 << (*i)->m_actual << endl;
          }

          fi = fragstats.find(fragment);
          if (fi == fragstats.end())
          {
            fi = fragstats.insert(make_pair(fragment, new InsertStats(""))).first;
          }

          fi->second->addSize((*i)->m_actual);
        }

        delete (*i);
      }

      m_inserts.clear();
    }

    cout << endl;
    cout << "SubLibrary Summaries" << endl;
    for (fi = fragstats.begin(); fi != fragstats.end(); fi++)
    {
      cout << fi->first << "\t"
           << fi->second->count() << "\t"
           << fi->second->mean() << "\t"
           << fi->second->stdev() << endl;
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

  while ( !errflg && ((ch = getopt (argc, argv, "hsvf:")) != EOF) )
    switch (ch)
      {
      case 'h': PrintHelp (argv[0]); exit (EXIT_SUCCESS); break;

      case 's': OPT_BankSpy = true; break;

      case 'v': PrintBankVersion (argv[0]); exit (EXIT_SUCCESS); break;

      case 'f': FRAGMENTSTRLEN = atoi(optarg); break;


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
    << "-f len        Number of characters of seqname to use as sublibrary (default=3)\n"
    << endl;
  cerr
    << "Finds all reads that should overlap a given contig range. Includes reads that\n"
    << "should be present by the virtue of their mate and the scaffold\n\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
  return;
}
