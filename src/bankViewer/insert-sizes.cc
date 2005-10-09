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

#include "DataStore.hh"
#include "Insert.hh"
#include "CoverageStats.hh"

using namespace std;
using namespace AMOS;

typedef HASHMAP::hash_map<AMOS::ID_t, pair<AMOS::ID_t, AMOS::FragmentType_t> > MateLookupMap;
MateLookupMap m_readmatelookup;

typedef HASHMAP::hash_map<ID_t, Tile_t *> SeqTileMap_t;

DataStore * m_datastore;
int m_tilingwidth;
int m_connectMates = 1;
typedef std::vector<Insert *> InsertList_t;
InsertList_t m_inserts;

class InsertStats
{
public:
  InsertStats() {}
  void addSize(int size){ m_sizes.push_back(size); }
  int count() const {return m_sizes.size(); }
  double mean() const 
  { 
    long sum = 0; 
    for (int i = 0; i < m_sizes.size(); i++)
      { sum += m_sizes[i]; }

    return ((double)sum) / m_sizes.size();
  }

  double stdev() const
  {
    double mea = mean();

    double sumdiff = 0;

    for (int i = 0; i < m_sizes.size(); i++)
    {
      double diff = m_sizes[i] - mea;
      sumdiff += (diff *diff);
    }

    sumdiff /= m_sizes.size() -1;

    return sqrt(sumdiff);

  }


private:

  vector<int> m_sizes;
};


void computeInsertHappiness(vector<Tile_t> m_tiling)
{
  SeqTileMap_t seqtileLookup;

  int mated = 0;
  int unmated = 0;
  int matelisted = 0;

  vector<Tile_t>::iterator ti;

  // map iid -> tile * (within this contig)
  for (ti =  m_tiling.begin();
       ti != m_tiling.end();
       ti++)
  {
    seqtileLookup[ti->source] = &(*ti);
  }

  cerr << "Loading mates" << endl;
  Insert * insert;
  MateLookupMap::iterator mi;

  int count = 0;

  // For each read in the contig
  SeqTileMap_t::iterator ai;
  for (ai =  seqtileLookup.begin();
       ai != seqtileLookup.end();
       ai++)
  {
    count++;

    if (ai->second == NULL)
    {
      //cerr << "Skipping already seen read" << endl;
      continue;
    }

    ID_t aid = ai->first;
    ID_t acontig = m_datastore->lookupContigId(aid);
    Tile_t * atile = ai->second;

    AMOS::ID_t libid = m_datastore->getLibrary(aid);
    AMOS::Distribution_t dist = m_datastore->getLibrarySize(libid);

    // Does it have a mate
    mi = m_datastore->m_readmatelookup.find(aid);
    if (mi == m_datastore->m_readmatelookup.end())
    {
      unmated++;
      insert = new Insert(aid, acontig, atile,
                          AMOS::NULL_ID, AMOS::NULL_ID, NULL,
                          libid, dist, m_tilingwidth, 
                          AMOS::Fragment_t::NULL_FRAGMENT);
    }
    else
    {
      matelisted++;

      ID_t bid = mi->second.first;
      ID_t bcontig = AMOS::NULL_ID;
      Tile_t * btile = NULL;
      bcontig = m_datastore->lookupContigId(bid);

      SeqTileMap_t::iterator bi = seqtileLookup.find(bid);

      if (bi != seqtileLookup.end())
      {
        mated++;

        btile = bi->second;
        bi->second = NULL;
      }

      insert = new Insert(aid, acontig, atile, 
                          bid, bcontig, btile, 
                          libid, dist, m_tilingwidth, 
                          mi->second.second);


      if (m_connectMates && insert->reasonablyConnected())
      {
        // A and B are within this contig, and should be drawn together
        insert->m_active = 2;
      }
      else if (btile)
      {
        // A and B are within this contig, but not reasonably connected
        Insert * j = new Insert(*insert);
        j->setActive(1, insert, m_connectMates);
        m_inserts.push_back(j);

        insert->setActive(0, j, m_connectMates);
      }
      else 
      { 
        // Just A is valid
        insert->setActive(0, NULL, m_connectMates); 
      }
    }

    m_inserts.push_back(insert);

    // Mark that this read has been taken care of already
    ai->second = NULL;
  }

  sort(m_inserts.begin(), m_inserts.end(), Insert::TilingOrderCmp());

  cerr << "mated: "   << mated 
       << " matelisted: " << matelisted
       << " unmated: " << unmated << endl;
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

      m_tilingwidth = scaff.getSpan();
    

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
      computeInsertHappiness(rtiling);

      vector<Insert *>::iterator i;

      int FRAGMENTSTRLEN = 5;
      int m_verbose = 0;
      
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
            fi = fragstats.insert(make_pair(fragment, new InsertStats())).first;
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

  while ( !errflg && ((ch = getopt (argc, argv, "hsvExy")) != EOF) )
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
    << "-E contigeid  Contig eid of interest\n"
    << "-x start      Start of range\n"
    << "-y end        End of range\n"
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
