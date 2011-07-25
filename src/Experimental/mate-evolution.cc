#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include "PersistentUtils.hh"
#include "DataStore.hh"
#include "Insert.hh"

using namespace AMOS;
using namespace std;

int INDEXSCAFFOLDS = 0;
int WRITEFEATURES = 0;
int MINDELTA = 0;

struct RangeDelta
{
  RangeDelta(Range_t r, int d) : range(r), delta(d) {}
  Range_t range;
  int delta;
};

struct RangeCmp
{
  bool operator () (const RangeDelta & a, const RangeDelta & b)
  {
    int ldiff = b.range.getLo() - a.range.getLo();

    if (ldiff)
    {
      if (ldiff < 0) { return false; }
      return true;
    }

    int rdiff = b.range.getHi() - a.range.getHi();
    if (rdiff)
    {
      if (rdiff < 0) { return false; }
      return true;
    }

    return false;
  }
};


void computeAllInserts(DataStore & datastore,
                       vector<Insert *> & inserts)
{
  set<ID_t> seencontigs;
  vector<Tile_t> tiling;

  Contig_t contig;
  Scaffold_t scaff;
  int count = 0;

  if (INDEXSCAFFOLDS)
  {
    datastore.scaffold_bank.seekg(1);
    while (datastore.scaffold_bank >> scaff)
    {
      datastore.mapReadsToScaffold(scaff, tiling, 0);
      datastore.calculateInserts(tiling, inserts, 1, 0);
      tiling.clear();
      count++;

      vector<Tile_t>::const_iterator ci;
      for (ci =  scaff.getContigTiling().begin(); 
           ci != scaff.getContigTiling().end();
           ci++)
      {
        seencontigs.insert(ci->source);
      }
    }

    cerr << "Indexed " << count << " scaffolds with " << seencontigs.size() << " contigs" << endl;
  }

  count = 0;
  set<ID_t>::iterator si;

  const IDMap_t cmap = datastore.contig_bank.getIDMap();
  IDMap_t::const_iterator ci;
  for (ci = cmap.begin(); ci != cmap.end(); ci++)
  {
    si = seencontigs.find(ci->iid);

    if (si == seencontigs.end())
    {
      count++;

      datastore.contig_bank.seekg(ci->bid);
      datastore.contig_bank >> contig;
      datastore.calculateInserts(contig.getReadTiling(), inserts, 1, 0);
    }
  }

  cerr << "Indexed " << count << " contigs not in scaffolds" << endl;
}


ReadPosLookup read2contigpos;


int main (int argc, char ** argv)
{
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  int curcontigiid = 0;
  string curcontigeid;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Compare mate-sizes in one bank to sizes in another\n"
"\n"
"   Usage: mate-evolution [options] curbank otherbankn\n"
"\n"
"   Output:\n"
"   read1 read2 delta (curdist disttomean) (olddist olddisttomean) [mean sd] acontig bcontig olda oldb\n"
"\n"
"   Options\n"
"   -------\n"
"   -d <val> Min delta to display\n"
"   -f       Write features to bank\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("d=i", &MINDELTA);
    tf->getOptions()->addOptionResult("f",   &WRITEFEATURES);

    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 2)
    {
      cerr << "Usage: mate-evolution [options] curbank otherbank" << endl;
      return EXIT_FAILURE;
    }

    string curbankname = argvv.front(); argvv.pop_front();
    string otherbankname = argvv.front(); argvv.pop_front();

    DataStore otherstore;
    vector<Insert *> otherinserts;

    DataStore curstore;
    vector<Insert *> curinserts;

    if (otherstore.openBank(otherbankname) || curstore.openBank(curbankname))
    {
      cerr << "Can't open bank" << endl;
      exit(1);
    }

    typedef HASHMAP::hash_map<ID_t, Insert *> ReadInsertMap;
    ReadInsertMap otherread2insert;

    computeAllInserts(otherstore, otherinserts);

    vector<Insert *>::iterator o;
    for (o = otherinserts.begin(); o != otherinserts.end(); o++)
    {
      if ((*o)->m_aid != 0) { otherread2insert.insert(make_pair((*o)->m_aid, *o)); }
      if ((*o)->m_bid != 0) { otherread2insert.insert(make_pair((*o)->m_bid, *o)); }
    }

    computeAllInserts(curstore, curinserts);

    curstore.feat_bank.close();

    typedef vector<RangeDelta> RangeList;
    typedef map<ID_t, RangeList> RangeMap;
    RangeMap fixranges;
    RangeMap::iterator fi;


    for (o = curinserts.begin(); o != curinserts.end(); o++)
    {
      if ((*o)->m_active == 2 && (*o)->m_state != Insert::OrientationViolation)
      {
        int curdist = (*o)->m_actual;

        ReadInsertMap::iterator other1 = otherread2insert.find((*o)->m_aid);
        if (other1 != otherread2insert.end())
        {
          if (other1->second->m_active == 2 && other1->second->m_state != Insert::OrientationViolation)
          {
            int olddist = other1->second->m_actual;
            int delta = olddist - curdist;  //1000 - 0 = 1000 bp collapse

            double sd   = (*o)->m_dist.sd;
            double mean = (*o)->m_dist.mean;

            int disttomean    = (int)(curdist - mean);
            int olddisttomean = (int)(olddist - mean);

            int meandelta = abs(disttomean) - abs(olddisttomean);

            if (meandelta >= MINDELTA)
            {
              cout << otherstore.read_bank.lookupEID((*o)->m_aid) << "\t"
                   << otherstore.read_bank.lookupEID((*o)->m_bid) << "\t"
                   << meandelta << "\t" << delta   
                   <<  "\t(" << curdist << "\t" << disttomean    
                   << ")\t(" << olddist << "\t" << olddisttomean 
                   << ")\t[" << mean << " +/- " << sd << "]\t"
                   << (*o)->m_acontig << "\t" << (*o)->m_bcontig << "\t"
                   << other1->second->m_acontig << "\t" << other1->second->m_bcontig 
                   << endl;

              if ((*o)->m_acontig == (*o)->m_bcontig)
              {
                fi = fixranges.find((*o)->m_acontig);
                if (fi == fixranges.end())
                {
                  fi = fixranges.insert(make_pair((*o)->m_acontig, vector<RangeDelta> ())).first;
                }

                fi->second.push_back(RangeDelta(Range_t((*o)->m_loffset, (*o)->m_roffset), meandelta));
              }
            }
          }
        }
      }
    }


    for (fi = fixranges.begin(); fi != fixranges.end(); fi++)
    {
      RangeList & ranges = fi->second;
      RangeList::iterator r1, r2;

      sort(ranges.begin(), ranges.end(), RangeCmp());

      cout << ">" << fi->first << " " << ranges.size() << endl;

      for (r1 = ranges.begin(); r1 != ranges.end(); r1++)
      {
        r2 = r1+1;

        int count = 1;
        int sum = r1->delta;

        cout << r1->range.getLo() << " " << r1->range.getHi() << " " << r1->delta << endl;

        while ((r2 != ranges.end()) && (r2->range.getLo() < r1->range.getHi()))
        {
          cout << r2->range.getLo() << " " << r2->range.getHi() << " " << r2->delta << endl;

          count++;
          sum += r2->delta;

          r1->range.setEnd(max(r1->range.getHi(), r2->range.getHi()));
          ranges.erase(r2);
          r2 = r1+1;
        }

        double delta = double(sum)/count;
        cout << ":" << r1->range.getLo() << " " << r1->range.getHi() << " " << count << " " << delta << endl;

        if (WRITEFEATURES)
        {
          curstore.feat_bank.open(curbankname, B_WRITE);
          Feature_t feat;
          feat.setRange(r1->range);
          feat.setSource(make_pair(fi->first, Contig_t::NCODE));
          feat.setType('f');

          stringstream comment;
          comment << "MatePair improvement count: " << count << " delta: " << delta;
          feat.setComment(comment.str());

          curstore.feat_bank << feat;
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
