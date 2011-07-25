#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include "PersistentUtils.hh"

using namespace AMOS;
using namespace std;


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
  int PRINTIID = 0;
  int PRINTLEN = 0;

  int GNUPLOT = 0;

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
"\n"
"   -x <val>  Examine starting at this position\n"
"   -y <val>  Examine stopping at this position\n"
"\n"
"   -t        Use tiling index instead of offset\n"
"   -i        Print contig IIDs instead of EIDs\n"
"   -l        Print length as well\n"
"\n"
"   -G        Print in gnuplot format\n"
"             Use: plot \"filename\" with lp\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("c=i", &curcontigiid);
    tf->getOptions()->addOptionResult("C=s", &curcontigeid);

    tf->getOptions()->addOptionResult("x=i", &startpos);
    tf->getOptions()->addOptionResult("y=i", &endpos);

    tf->getOptions()->addOptionResult("t",   &USETILENUM);
    tf->getOptions()->addOptionResult("i",   &PRINTIID);
    tf->getOptions()->addOptionResult("l",   &PRINTLEN);

    tf->getOptions()->addOptionResult("G",   &GNUPLOT);

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
      oldcontigreadcount.insert(make_pair(contig.getIID(), contig.getReadTiling().size()));

      recordReadPositions(contig.getIID(), contig.getReadTiling(), read2contigpos);
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

    int oldcontigsize = oldcontigs.size();



    int width = (PRINTLEN) ? 20 : 10;

    if (!GNUPLOT)
    {
      // Print the header line of old contigs
      if (PRINTIID)
      {
        printf("%-15s% *d ", "== CONTIGS == ", width, contig.getIID());
      }
      else
      {
        printf("%-15s% *s ", "== CONTIGS == ", width, contig.getEID().c_str());
      }

      for (old = oldcontigs.begin(); old != oldcontigs.end(); old++)
      {
        if (PRINTIID) { printf("% *d ", width, *old); }
        else          { printf("% *s ", width, ocm.lookupEID(*old).c_str()); }
      }
      cout << endl;

      printf("%-15s% *d ", "== READS ==", width, contig.getReadTiling().size());
      for (old = oldcontigs.begin(); old != oldcontigs.end(); old++)
      {
        printf("% *d ", width, oldcontigreadcount[*old]);
      }
      cout << endl;
    }


    // Print where the read are in the other assembly
    check = false;
    if (startpos == -1){check = true; }

    for (t = contig.getReadTiling().begin(); t != contig.getReadTiling().end(); t++)
    {
      if (!check && t->offset >= startpos)
      {
        check = true;
      }
      
      if (check && endpos != -1 && t->offset > endpos)
      {
        check = false;
      }

      if (check)
      {
        int val = t->offset;

        if (!GNUPLOT)
        {
          if (PRINTLEN)
          {
            char buffer[30];
            sprintf(buffer, "%d,%d", val, t->getRightOffset());
            printf("%-15s%20s%c", 
                   crm.lookupEID(t->source).c_str(),
                   buffer,
                   (t->range.isReverse() ? '-' : '+'));
          }
          else
          {
            printf("%-15s%10d%c", 
                   crm.lookupEID(t->source).c_str(),
                   val,
                   (t->range.isReverse() ? '-' : '+'));
          }
        }

        rpl = read2contigpos.find(t->source);

        if (rpl != read2contigpos.end())
        {
          for (int o = 0; o < oldcontigsize; o++)
          {
            if (oldcontigs[o] == rpl->second.m_contigiid)
            {
              int val = rpl->second.m_offset;

              if (GNUPLOT)
              {
                cout << 0         << " " << t->offset << endl;
                cout << (o+1)*100 << " " << rpl->second.pos(t->range.isReverse()) << endl;
                cout << endl;
                cout << endl;
              }
              else
              {
                if (USETILENUM) { val = rpl->second.m_tileid; }

                if (PRINTLEN)
                {
                  char buffer[30];
                  sprintf(buffer, "%d,%d", val, rpl->second.getRightOffset());
                  printf("%20s%c", buffer, (rpl->second.m_rc ? '-' : '+'));
                }
                else
                {
                  printf("%10d%c", val, (rpl->second.m_rc ? '-' : '+'));
                }
              }
            }
            else
            {
              if (!GNUPLOT) { printf("%*s ", width, " "); }
            }
          }
        }

        if (!GNUPLOT) { cout << endl; }
      }
    }

    if (GNUPLOT)
    {
      cout << (oldcontigsize*100)+10 << " " << 0 << endl
           << (oldcontigsize*100)+10 << " " << 0 << endl;

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
