#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"

using namespace AMOS;
using namespace std;


class ReadPosInfo
{
public:
  ReadPosInfo(ID_t contigiid, int offset, bool rc) 
    : m_contigiid(contigiid), m_offset(offset), m_rc(rc)
  {}

  ID_t m_contigiid;
  int  m_offset;
  bool m_rc;
};

typedef map<ID_t, ReadPosInfo> ReadPosLookup;
ReadPosLookup curread2contigpos;


class NeighborLink
{
public:
  NeighborLink(ID_t bcontig) : m_bcontigid(bcontig) {}
  ID_t m_bcontigid;

  set<ID_t> m_areads;
  set<ID_t> m_breads;
  set<ID_t> m_joins;
  map<ID_t, ID_t> m_read2contig;

  void addPair(ID_t join, ID_t aread, ID_t bread)
  {
    m_areads.insert(aread);
    m_read2contig.insert(make_pair(aread, join));

    m_breads.insert(bread);
    m_read2contig.insert(make_pair(bread, join));

    m_joins.insert(join);
  }
};


class Neighbors
{
public:
    Neighbors(ID_t contigid)
     : m_contigid(contigid)
    {}

    ID_t m_contigid;
    map<ID_t, NeighborLink> m_links;

    void addNeighbor(ID_t bcontig, 
                     ID_t join, 
                     ID_t aread,
                     ID_t bread)
    {
      map<ID_t, NeighborLink>::iterator n = m_links.find(bcontig);
      if (n == m_links.end())
      {
        n = m_links.insert(make_pair(bcontig, NeighborLink(bcontig))).first;
      }

      n->second.addPair(join, aread, bread);
    }
};


typedef map<ID_t, Neighbors> NeighborMap;
NeighborMap neighbormap;

void handlePair(ID_t acontig, 
                ID_t bcontig, 
                ID_t join, 
                ID_t aread, 
                ID_t bread)
{
  NeighborMap::iterator n = neighbormap.find(acontig);

  if (n == neighbormap.end())
  {
    n = neighbormap.insert(make_pair(acontig, Neighbors(acontig))).first;
  }

  n->second.addNeighbor(bcontig, join, aread, bread);
}


int main (int argc, char ** argv)
{
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  int curcontigiid = 0;
  string curcontigeid;

  int startpos = -1;
  int endpos = -1;

  int USETILENUM = 0;
  map<ID_t, int> curcontigreads;
  int HIDEDETAILS = 0;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Find reads that are in two contigs in one assembly, but in the same\n"
"contig in another assembly\n"
"\n"
"   Usage: gap-closure-reads [options] curbank otherbankn\n"
"\n"
"   Options\n"
"   -------------------\n"
"   -d   Hide details\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("d", &HIDEDETAILS, "Be verbose when reporting");

    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    int USETILENUM = 0;

    if (argvv.size() != 2)
    {
      cerr << "Usage: gap-closure-reads [options] curbank otherbank" << endl;
      return EXIT_FAILURE;
    }

    string curbankname = argvv.front(); argvv.pop_front();
    string otherbankname = argvv.front(); argvv.pop_front();

    Bank_t ccontig_bank(Contig_t::NCODE);
    Bank_t cread_bank(Read_t::NCODE);

    Bank_t ocontig_bank(Contig_t::NCODE);
    Bank_t oread_bank(Read_t::NCODE);

    ccontig_bank.open(curbankname, B_READ);
    cread_bank.open(curbankname, B_READ);

    ocontig_bank.open(otherbankname, B_READ);
    oread_bank.open(otherbankname, B_READ);

    IDMap_t::const_iterator c;
    const IDMap_t & crm = cread_bank.getIDMap();
    const IDMap_t & orm = oread_bank.getIDMap();

    cerr << "Indexing " << curbankname << "... ";
    const IDMap_t & ccm = ccontig_bank.getIDMap();
    for (c = ccm.begin(); c != ccm.end(); c++)
    {
      Contig_t contig;
      ccontig_bank.fetch(c->iid, contig);

      curcontigreads.insert(make_pair(c->iid, contig.getReadTiling().size()));

      sort(contig.getReadTiling().begin(), contig.getReadTiling().end(), TileOrderCmp());

      int tilenum = 0;
      vector<Tile_t>::iterator t;
      for (t = contig.getReadTiling().begin(); t != contig.getReadTiling().end(); t++)
      {
        int val = t->offset;
        if (USETILENUM) { val = tilenum; }

        curread2contigpos.insert(make_pair(t->source, ReadPosInfo(c->iid, val, t->range.isReverse())));
        tilenum++;
      }
    }

    cerr << curread2contigpos.size() << " reads in contigs" << endl;

    cerr << "Processing " << otherbankname << "... ";

    int cmp = 0;
    int diff = 0;
    int areadcount = 0;
    int same = 0;

    // consider all pairs of reads contigs from the other assembly,
    // if there is a pair that is in different assemblies, this pair
    // could be used for stitching the contigs
    const IDMap_t & ocm = ocontig_bank.getIDMap();
    for (c = ocm.begin(); c != ocm.end(); c++)
    {
      Contig_t contig;
      ocontig_bank.fetch(c->iid, contig);

      vector<Tile_t> & tiling = contig.getReadTiling();

      sort(tiling.begin(), tiling.end(), TileOrderCmp());

      int tilenum = 0;
      vector<Tile_t>::iterator t;
      vector<Tile_t>::iterator t2;
      for (t = tiling.begin(); t != tiling.end(); t++)
      {
        string aread = orm.lookupEID(t->source);
        ID_t t1iid = crm.lookupIID(aread);
        ReadPosLookup::iterator t1info = curread2contigpos.find(t1iid);
        areadcount++;

        if (t1info != curread2contigpos.end())
        {
          ID_t acontig = t1info->second.m_contigiid;

          for (t2 = t+1; t2 != tiling.end(); t2++)
          {
            cmp++;
            string bread = orm.lookupEID(t2->source);
            ID_t t2iid = crm.lookupIID(bread);
            ReadPosLookup::iterator t2info = curread2contigpos.find(t2iid);

            if (t2info != curread2contigpos.end())
            {
              ID_t bcontig = t2info->second.m_contigiid;

              if (acontig != bcontig)
              {
                diff++;

                if (acontig < bcontig)
                {
                  handlePair(acontig, bcontig, contig.getIID(), t1iid, t2iid);
                }
                else
                {
                  handlePair(bcontig, acontig, contig.getIID(), t2iid, t1iid);
                }
              }
              else
              {
                same++;
              }
            }
          }
        }
      }
    }

    cerr << areadcount << " reads in contigs." << endl;
    cerr << "Made " << cmp << " comparisons." << endl;
    cerr << same << " pairs were consistent." << endl;
    cerr << diff << " pairs were new." << endl;


    NeighborMap::iterator n;
    for (n = neighbormap.begin(); n != neighbormap.end(); n++)
    {
      ID_t acontig = n->second.m_contigid;
      string ac = ccm.lookupEID(acontig);

      map<ID_t, NeighborLink>::iterator l;
      for (l = n->second.m_links.begin(); l != n->second.m_links.end(); l++)
      {
        NeighborLink & nl = l->second;
        ID_t bcontig = nl.m_bcontigid;
        string bc = ccm.lookupEID(bcontig);

        cout << ">" << ac << " [" << nl.m_areads.size() << "/" 
             << curcontigreads[acontig] << "] <-> "
             << bc << " [" << nl.m_breads.size() << "/"
             << curcontigreads[bcontig] << "]";

        set<ID_t>::const_iterator si;

        for (si = nl.m_joins.begin(); si != nl.m_joins.end(); si++)
        {
          cout << "\t" << ocm.lookupEID(*si);
        }
        cout << endl;


        if (!HIDEDETAILS)
        {

          for (si = nl.m_areads.begin(); si != nl.m_areads.end(); si++)
          {
            cout << "a\t" << crm.lookupEID(*si) << "\t" 
                 << ocm.lookupEID(nl.m_read2contig[*si]) << endl;
          }

          for (si = nl.m_breads.begin(); si != nl.m_breads.end(); si++)
          {
            cout << "b\t" << crm.lookupEID(*si) << "\t"
                 << ocm.lookupEID(nl.m_read2contig[*si]) << endl;
          }
        }
      }
    }
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    retval = 1;
  }
  catch (const ExitProgramNormally & e)
  {
    retval = 0;
  }
  catch (const amosException & e)
  {
    cerr << e << endl;
    retval = 100;
  }

  try
  {
    if (tf) delete tf;
  }
  catch (const amosException & e)
  {
    cerr << "amosException while deleting tf: " << e << endl;
    retval = 105;
  }

  cerr << "End: " << Date() << endl;
  return retval;
}
