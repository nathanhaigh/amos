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
ReadPosLookup read2contigpos;


int main (int argc, char ** argv)
{
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  int curcontigiid = 0;
  string curcontigeid;

  int startpos = -1;
  int endpos = -1;

  int USETILENUM = 0;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Computes the read order evolution\n"
"\n"
"   Usage: read-evolution [options] curbank otherbankn\n"
"\n"
"   Options\n"
"   -------------------\n"
"   -c <iid>  Examine reads in current contig iid\n"
"   -C <eid>  Examine reads in current contig eid\n"
"   -x <val>  Examine starting at this position\n"
"   -y <val>  Examine stopping at this position\n"
"\n"
"   -t        Use tiling index instead of offset\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("c=i", &curcontigiid, "Be verbose when reporting");
    tf->getOptions()->addOptionResult("C=s", &curcontigeid, "Be verbose when reporting");

    tf->getOptions()->addOptionResult("x=i", &startpos, "Be verbose when reporting");
    tf->getOptions()->addOptionResult("y=i", &endpos, "Be verbose when reporting");

    tf->getOptions()->addOptionResult("t",   &USETILENUM, "Be verbose when reporting");

    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (curcontigiid == 0 && curcontigeid.empty())
    {
      cerr << "You must specify a contig to examine" << endl;
      return EXIT_FAILURE;
    }


    if (argvv.size() != 2)
    {
      cerr << "Usage: read-evolution [options] curbank otherbank" << endl;
      return EXIT_FAILURE;
    }

    string curbankname = argvv.front(); argvv.pop_front();
    string otherbankname = argvv.front(); argvv.pop_front();

    Bank_t ocontig_bank(Contig_t::NCODE);
    Bank_t oread_bank(Read_t::NCODE);

    Bank_t ccontig_bank(Contig_t::NCODE);
    Bank_t cread_bank(Read_t::NCODE);


    map<ID_t, int> oldcontigreadcount;


    ocontig_bank.open(otherbankname, B_READ);
    oread_bank.open(otherbankname, B_READ);

    ccontig_bank.open(curbankname, B_READ);
    cread_bank.open(curbankname, B_READ);

    IDMap_t::const_iterator c;
    const IDMap_t & crm = cread_bank.getIDMap();
    const IDMap_t & orm = oread_bank.getIDMap();

    // Record the contig and position of each read in the old bank
    const IDMap_t & ocm = ocontig_bank.getIDMap();
    for (c = ocm.begin(); c != ocm.end(); c++)
    {
      Contig_t contig;
      ocontig_bank.fetch(c->iid, contig);

      sort(contig.getReadTiling().begin(), contig.getReadTiling().end(), TileOrderCmp());

      oldcontigreadcount.insert(make_pair(contig.getIID(), contig.getReadTiling().size()));

      int tilenum = 0;
      vector<Tile_t>::iterator t;
      for (t = contig.getReadTiling().begin(); t != contig.getReadTiling().end(); t++)
      {
        ID_t curiid = crm.lookupIID(orm.lookupEID(t->source));

        int val = t->offset;
        if (USETILENUM) { val = tilenum; }

        read2contigpos.insert(make_pair(curiid, ReadPosInfo(c->iid, val, t->range.isReverse())));
        tilenum++;
      }
    }

    const IDMap_t & ccm = ccontig_bank.getIDMap();
    if (!curcontigeid.empty()) { curcontigiid = ccm.lookupIID(curcontigeid); }


    // Figure out which old contigs had reads in the current contig
    map<ID_t, int> oldcontigsandminoffset;
    map<ID_t, int>::iterator oco;

    Contig_t contig;
    ccontig_bank.fetch(curcontigiid, contig);
    sort(contig.getReadTiling().begin(), contig.getReadTiling().end(), TileOrderCmp());
    ReadPosLookup::iterator rpl;

    bool check = false;
    if (startpos == -1){check = true; }

    vector<Tile_t>::iterator t;
    for (t = contig.getReadTiling().begin(); t != contig.getReadTiling().end(); t++)
    {
      if (!check && t->offset > startpos)
      {
        check = true;
      }
      
      if (check && endpos != -1 && t->offset > endpos)
      {
        check = false;
      }

      if (check)
      {
        rpl = read2contigpos.find(t->source);

        if (rpl !=read2contigpos.end())
        {
          oco = oldcontigsandminoffset.find(rpl->second.m_contigiid);
          if (oco == oldcontigsandminoffset.end())
          {
            oco = oldcontigsandminoffset.insert(make_pair(rpl->second.m_contigiid, t->offset)).first;
          }
        }
      }
    }

    vector<ID_t> oldcontigs;
    vector<ID_t>::iterator old;

    while (!oldcontigsandminoffset.empty())
    {
      map<ID_t, int>::iterator minoco = oldcontigsandminoffset.begin();

      for (oco = oldcontigsandminoffset.begin();
           oco != oldcontigsandminoffset.end();
           oco++)
      {
        if (oco->second < minoco->second)
        {
          minoco = oco;
        }
      }

      oldcontigs.push_back(minoco->first);
      oldcontigsandminoffset.erase(minoco);
    }




    // Print the header line of old contigs
    printf("%-15s% 10s ", "== CONTIGS == ", contig.getEID().c_str());
    for (old =  oldcontigs.begin();
         old != oldcontigs.end();
         old++)
    {
      printf("% 10s ", ocm.lookupEID(*old).c_str());
    }
    cout << endl;

    printf("%-15s% 10d ", "== READS ==", contig.getReadTiling().size());
    for (old =  oldcontigs.begin();
         old != oldcontigs.end();
         old++)
    {
      printf("% 10d ", oldcontigreadcount[*old]);
    }
    cout << endl;




    // Print where the read was in the other contigs
    check = false;
    if (startpos == -1){check = true; }

    for (t = contig.getReadTiling().begin(); t != contig.getReadTiling().end(); t++)
    {
      if (!check && t->offset > startpos)
      {
        check = true;
      }
      
      if (check && endpos != -1 && t->offset > endpos)
      {
        check = false;
      }

      if (check)
      {
        printf("%-15s%10d%c", 
               crm.lookupEID(t->source).c_str(),
               t->offset,
               (t->range.isReverse() ? '-' : '+'));

        rpl = read2contigpos.find(t->source);

        if (rpl != read2contigpos.end())
        {
          for (old =  oldcontigs.begin();
               old != oldcontigs.end();
               old++)
          {
            if (*old == rpl->second.m_contigiid)
            {
              printf("%10d%c", rpl->second.m_offset, (rpl->second.m_rc ? '-' : '+'));
            }
            else
            {
              printf("%10s ", " ");
            }
          }
        }
        cout << endl;
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
