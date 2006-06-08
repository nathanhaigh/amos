#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"

#include "PersistentUtils.hh"

using namespace AMOS;
using namespace std;

int MINDELTA(0);
int PRINTDIFFERENT(0);
int USESCAFFOLD(0);
int USEEID(0);
int SHOWBEST(0);




ReadPosLookup read2contigpos;

IDMap_t lastcontigmap;

vector<Tile_t> getScaffoldReadTiling(Scaffold_t & scaffold, 
                                     Bank_t & contig_bank)
{
  vector<Tile_t> scaffreads;
  vector <Tile_t>::iterator ci;

  for (ci = scaffold.getContigTiling().begin();
       ci != scaffold.getContigTiling().end();
       ci++)
  {
    Contig_t contig;
    contig_bank.fetch(ci->source, contig);

    if (ci->range.isReverse())
    {
      contig.reverseComplement();
    }

    int clen = contig.getLength(); 

    vector <Tile_t>::iterator ri;
    for (ri = contig.getReadTiling().begin();
         ri != contig.getReadTiling().end();
         ri++)
    {
      Tile_t mappedTile;
      mappedTile.source = ri->source;
      mappedTile.gaps   = ri->gaps;
      mappedTile.range  = ri->range;
      mappedTile.offset = ci->offset + ri->offset;
      scaffreads.push_back(mappedTile);
    }
  }

  return scaffreads;
}



void printReadPair(Bank_t & read_bank,
                   ID_t curiid, 
                   Tile_t * cur1,
                   Tile_t * cur2,
                   ReadPosInfo * other1,
                   ReadPosInfo * other2)
{
  int delta, mdist, pdist;
  int consistent;

  computeDelta(cur1, other1, cur2, other2,
               consistent, delta, mdist, pdist);


  int absdelta = abs(delta);

  if (absdelta >= MINDELTA)
  {
    cout << curiid << "\t";

    if (USEEID)
    {
      cout << lastcontigmap.lookupEID(other1->contig()) << "\t";
    }
    else
    {
      cout << other1->contig() << "\t";
    }

    cout << read_bank.lookupEID(cur1->source) << "\t" 
         << read_bank.lookupEID(cur2->source) << "\t" 
         << mdist << "\t"
         << pdist << "\t"
         << delta << "\t"
         << consistent << endl;
  }
}

class Neighbor_t
{
public:
  Neighbor_t(ID_t contigid) : m_contigid(contigid) {}
  ID_t m_contigid;
  vector<ReadPosInfo *> m_reads;

  void addRead(ReadPosInfo * r)
  {
    m_reads.push_back(r);
  }
};

int contigcount(0);
int neighborcount(0);



void printBestReadDistance(ID_t curiid,
                          vector<Tile_t> & tiling,
                          Bank_t & read_bank)
{
  map<ID_t, Neighbor_t> neighbormap;
  map<ID_t, Neighbor_t>::iterator ni;

  sort(tiling.begin(), tiling.end(), TileOrderCmp());

  contigcount += (tiling.size() * (tiling.size()-1))/2;

  map<ID_t, Tile_t *> tilelookup;

  vector<Tile_t>::iterator t1;
  for (t1 = tiling.begin(); t1 != tiling.end(); t1++)
  {
    ReadPosLookup::iterator r1 = read2contigpos.find(t1->source);

    if (r1 != read2contigpos.end())
    {
      ni = neighbormap.find(r1->second.contig());

      if (ni == neighbormap.end())
      {
        ni = neighbormap.insert(make_pair(r1->second.contig(), Neighbor_t(r1->second.contig()))).first;
      }

      ni->second.addRead(&r1->second);
      tilelookup.insert(make_pair(t1->source, &(*t1)));
    }
  }

  for (ni = neighbormap.begin(); ni != neighbormap.end(); ni++)
  {
    int l = ni->second.m_reads.size();
    int max = 0;
    int maxi;
    int maxj;

    for (int i = 0; i < l; i++)
    {
      Tile_t * ti = tilelookup[ni->second.m_reads[i]->iid()];

      for (int j = i+1; j < l; j++)
      {
        neighborcount++;

        int delta, mdist, pdist;
        int consistent;

        computeDelta(&(*ti), 
                     ni->second.m_reads[i],
                     tilelookup[ni->second.m_reads[j]->iid()],
                     ni->second.m_reads[j],
                     consistent, delta, mdist, pdist);

        if (consistent && delta > max)
        {
          max = delta; maxi = i; maxj = j;
        }
      }
    }

    if (max)
    {
      printReadPair(read_bank, curiid, 
                    tilelookup[ni->second.m_reads[maxi]->iid()],
                    tilelookup[ni->second.m_reads[maxj]->iid()],
                    ni->second.m_reads[maxi],
                    ni->second.m_reads[maxj]);
    }
  }
}


void printAllReadDistance(ID_t curiid,
                          vector<Tile_t> & tiling,
                          Bank_t & read_bank)
{
  vector<Tile_t>::iterator t1;
  vector<Tile_t>::iterator t2;

  int tilingsize = tiling.size();

  for (t1 = tiling.begin(); t1 != tiling.end(); t1++)
  {
    ReadPosLookup::iterator r1 = read2contigpos.find(t1->source);
    if (r1 == read2contigpos.end()) { continue; }

    for (t2 = t1+1; t2 != tiling.end(); t2++)
    {
      ReadPosLookup::iterator r2 = read2contigpos.find(t2->source);
      if (r2 == read2contigpos.end()) { continue; }

      // Distance is only defined if the two reads existed in the same contig before
      if (r1->second.contig() == r2->second.contig())
      {
        contigcount++;
        printReadPair(read_bank, curiid, &(*t1), &(*t2), &r1->second, &r2->second);
      }
      else if (PRINTDIFFERENT)
      {
        int dist = computeDistance(&*t1, &*t2);

        cout << curiid << "\t" 
             << "*\t"
             << read_bank.lookupEID(t1->source) << "\t" 
             << read_bank.lookupEID(t2->source) << "\t" 
             << dist << "\t"
             << "*\t*" << endl;
      }
    }
  }
}


int main (int argc, char ** argv)
{
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Computes the read distance evolution\n"
"\n"
"   Usage: persistent-read-dist [options] bank1 ... bankn\n"
"\n"
"delta = curdist - olddist (negative means collapse in current\n"
"Output format:\n"
">currentbank-lastbank\n"
"curcontigiid lastcontigiid read1 read2 dist olddist delta\n"
"\n"
"   Options\n"
"   -------------------\n"
"   -d <val>  Only display abs(delta) >= val\n"
"   -p        Display distances between reads not in same lastbank contig\n"
"   -s        Use scaffolds when computing distances\n"
"   -e        Display contigeid's instead of iids\n"
"   -b        Only show the biggest different for a given cur,old pair\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("d=i", &MINDELTA,       "Be verbose when reporting");
    tf->getOptions()->addOptionResult("p",   &PRINTDIFFERENT, "Be verbose when reporting");
    tf->getOptions()->addOptionResult("s",   &USESCAFFOLD,    "Be verbose when reporting");
    tf->getOptions()->addOptionResult("e",   &USEEID,         "Be verbose when reporting");
    tf->getOptions()->addOptionResult("b",   &SHOWBEST,       "Only show best");
    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() == 0)
    {
      cerr << "Usage: persistent-read-dist [options] bank1 ... bankn" << endl;
      return EXIT_FAILURE;
    }

    string lastbank;
    string bankname = argvv.front();
    bool first = true;

    cerr << "Loading read id map" << endl;
    Bank_t read_bank(Read_t::NCODE);
    read_bank.open(bankname, B_READ); 

    while (!argvv.empty())
    {
      bankname = argvv.front(); argvv.pop_front();
      cerr << "Processing " << bankname << " at " << Date() << endl;

      Bank_t scaffold_bank(Scaffold_t::NCODE);
      Bank_t contig_bank(Contig_t::NCODE);
      contig_bank.open(bankname, B_READ);

      if (USESCAFFOLD)
      {
        scaffold_bank.open(bankname, B_READ);
      }

      bankname = bankname.substr(0, bankname.find_first_of("/ "));

      if (!first)
      {
        cout << ">" << bankname << "-" << lastbank << endl;
      }

      lastbank = bankname;

      IDMap_t::const_iterator c;
      if (USESCAFFOLD)
      {
        const IDMap_t & scaffoldmap = scaffold_bank.getIDMap();

        if (!first)
        {
          for (c = scaffoldmap.begin(); c != scaffoldmap.end(); c++)
          {
            Scaffold_t scaffold;
            scaffold_bank.fetch(c->iid, scaffold);
            vector <Tile_t> scaffreads = getScaffoldReadTiling(scaffold, contig_bank);

            if (SHOWBEST)
            {
              printBestReadDistance(c->iid, scaffreads, read_bank);
            }
            else
            {
              printAllReadDistance(c->iid, scaffreads, read_bank);
            }
          }

          read2contigpos.clear();
        }

        for (c = scaffoldmap.begin(); c != scaffoldmap.end(); c++)
        {
          Scaffold_t scaffold;
          scaffold_bank.fetch(c->iid, scaffold);
          vector <Tile_t> scaffreads = getScaffoldReadTiling(scaffold, contig_bank);
          recordReadPositions(c->iid, scaffreads, read2contigpos);
        }

        lastcontigmap = scaffoldmap;
      }
      else
      {
        const IDMap_t & contigmap = contig_bank.getIDMap();

        if (!first)
        {
          for (c = contigmap.begin(); c!= contigmap.end(); c++)
          {
            Contig_t contig;
            contig_bank.fetch(c->iid, contig);

            if (SHOWBEST)
            {
              printBestReadDistance(c->iid, contig.getReadTiling(), read_bank);
            }
            else
            {
              printAllReadDistance(c->iid, contig.getReadTiling(), read_bank);
            }
          }
          
          read2contigpos.clear();
        }

        for (c = contigmap.begin(); c!= contigmap.end(); c++)
        {
          Contig_t contig;
          contig_bank.fetch(c->iid, contig);
          recordReadPositions(contig.getIID(), contig.getReadTiling(), read2contigpos);
        }

        lastcontigmap = contigmap;
      }

      first = false;
    }

    cerr << "Would have made " << contigcount << " comparisons" << endl;
    cerr << "Actually made " << neighborcount << " comparisons" << endl;
    cerr << "Saved: " << contigcount - neighborcount << endl;
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
