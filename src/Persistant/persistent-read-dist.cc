#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"

using namespace AMOS;
using namespace std;

int MINDELTA(0);
int PRINTDIFFERENT(0);
int USESCAFFOLD(0);


typedef map<ID_t, pair<ID_t,int> > ReadPosLookup;
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


void printReadDistance(ID_t curiid,
                       vector<Tile_t> & tiling,
                       Bank_t & read_bank)
{
  vector<Tile_t>::const_iterator t1;
  vector<Tile_t>::const_iterator t2;

  int tilingsize = tiling.size();

  for (t1 = tiling.begin(); t1 != tiling.end(); t1++)
  {
    int t1pos = t1->offset;
    if (t1->range.isReverse()) { t1pos = t1pos + t1->getGappedLength() - 1; }

    for (t2 = t1+1; t2 != tiling.end(); t2++)
    {
      int t2pos = t2->offset;
      if (t2->range.isReverse()) { t2pos = t2pos + t2->getGappedLength() - 1; }

      int dist = abs(t2pos - t1pos);
      int olddist = 0;
      int delta = 0;
      int absdelta = 0;

      ReadPosLookup::const_iterator r1 = read2contigpos.find(t1->source);
      ReadPosLookup::const_iterator r2 = read2contigpos.find(t2->source);

      // Distance is only defined if the two reads existed in the same contig before
      if (r1 != read2contigpos.end() &&
          r2 != read2contigpos.end() &&
          r1->second.first == r2->second.first)
      {
        olddist = abs(r1->second.second - r2->second.second);
        delta = dist-olddist;
        absdelta = abs(delta);

        if (absdelta >= MINDELTA)
        {
          cout << curiid << "\t" 
               << lastcontigmap.lookupEID(r1->second.first) << "\t"
               << read_bank.lookupEID(t1->source) << "\t" 
               << read_bank.lookupEID(t2->source) << "\t" 
               << dist << "\t"
               << olddist << "\t"
               << delta << endl;
        }
      }
      else if (PRINTDIFFERENT)
      {
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



void recordReadPosition(const vector<Tile_t> & tiling, ID_t iid)
{
  vector<Tile_t>::const_iterator ti;
  for (ti = tiling.begin(); ti != tiling.end(); ti++)
  {
    int t1pos = ti->offset;
    if (ti->range.isReverse()) { t1pos = t1pos + ti->getGappedLength() - 1; }

    read2contigpos.insert(make_pair(ti->source, make_pair(iid, t1pos)));
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
"curcontigeid lastcontigeid read1 read2 dist olddist delta\n"
"\n"
"   Options\n"
"   -------------------\n"
"   -d <val>  Only display abs(delta) >= val\n"
"   -p        Display distances between reads not in same lastbank contig\n"
"   -s        Use scaffolds when computing distances\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("d=i", &MINDELTA,       "Be verbose when reporting");
    tf->getOptions()->addOptionResult("p",   &PRINTDIFFERENT, "Be verbose when reporting");
    tf->getOptions()->addOptionResult("s",   &USESCAFFOLD,    "Be verbose when reporting");
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
            printReadDistance(c->iid, scaffreads, read_bank);
          }

          read2contigpos.clear();
        }

        for (c = scaffoldmap.begin(); c != scaffoldmap.end(); c++)
        {
          Scaffold_t scaffold;
          scaffold_bank.fetch(c->iid, scaffold);
          vector <Tile_t> scaffreads = getScaffoldReadTiling(scaffold, contig_bank);
          recordReadPosition(scaffreads, c->iid);
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
            printReadDistance(c->iid, contig.getReadTiling(), read_bank);
          }
          
          read2contigpos.clear();
        }

        for (c = contigmap.begin(); c!= contigmap.end(); c++)
        {
          Contig_t contig;
          contig_bank.fetch(c->iid, contig);
          recordReadPosition(contig.getReadTiling(), c->iid);
        }

        lastcontigmap = contigmap;
      }

      first = false;
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
