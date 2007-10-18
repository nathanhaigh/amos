#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include "Feature_AMOS.hh"

using namespace AMOS;
using namespace std;

int OPT_Scaffold(0);
int OPT_Stride(1);

void processContig(Contig_t & contig, int base_offset)
{
  vector<Tile_t> & tiling = contig.getReadTiling();
  vector<Tile_t>::const_iterator ti;
  sort(tiling.begin(), tiling.end(), TileOrderCmp());

  multiset<int> endpoints;
  multiset<int>::iterator ep, ep2;

  int conslen = contig.getLength();

  ti = tiling.begin();

  for (int offset = 0; offset < conslen; offset++)
  {
    ep = endpoints.begin();
    while ((ep != endpoints.end()) && (*ep < offset))
    {
      ep2 = ep;
      ep++;
      endpoints.erase(ep2);
    }

    while ((ti != tiling.end()) && (ti->offset == offset))
    {
      int roffset = ti->getRightOffset();
      endpoints.insert(roffset);
      ti++;
    }

    int gpos = offset+base_offset;

    if (gpos % OPT_Stride == 0)
    {
      cout << gpos << " " << endpoints.size() << endl;
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
"Plot the read coverage across contigs/scaffolds\n"
"Offset is the 0-based gapped coordinate\n"
"Output:\n"
">id\n"
"offset0 depth0\n"
"offset1 depth1\n"
"\n"
"   Usage: read-cov-plot [options] bank\n"
"\n"
"Options\n"
"-------------------\n"
"-S Print scafold coordinates instead of contig coordinates\n"
"-n <val> Only print every n-th line\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("S", &OPT_Scaffold);
    tf->getOptions()->addOptionResult("n=i", &OPT_Stride);
    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 1)
    {
      cerr << "Usage: read-cov-plot [options] bank" << endl;
      return EXIT_FAILURE;
    }

    string bankname = argvv.front(); argvv.pop_front();
    cerr << "Processing " << bankname << " at " << Date() << endl;

    Bank_t contig_bank(Contig_t::NCODE);
    contig_bank.open(bankname, B_READ|B_SPY);

    const IDMap_t & contigmap = contig_bank.getIDMap();
    IDMap_t::const_iterator c;

    if (OPT_Scaffold)
    {
      Bank_t scaff_bank(Scaffold_t::NCODE);
      scaff_bank.open(bankname, B_READ|B_SPY);

      const IDMap_t & scaffmap = scaff_bank.getIDMap();
      IDMap_t::const_iterator s;

      for (s = scaffmap.begin(); s != scaffmap.end(); s++)
      {
        Scaffold_t scaff;
        scaff_bank.fetch(s->iid, scaff);

        cout << ">Scaffold " << s->iid << endl;

        vector<Tile_t> & ctiling = scaff.getContigTiling();
        vector<Tile_t>::const_iterator ci;
        for (ci = ctiling.begin(); ci != ctiling.end(); ci++)
        {
          Contig_t contig;
          contig_bank.fetch(ci->source, contig);

          cout << ">Contig " << ci->source << endl;

          if (ci->range.isReverse())
          {
            contig.reverseComplement();
          }

          processContig(contig, ci->offset);
        }
      }
    }
    else
    {
      for (c = contigmap.begin(); c!=contigmap.end(); c++)
      {
        Contig_t contig;
        contig_bank.fetch(c->iid, contig);
        cout << ">Contig " << c->iid << endl;
        processContig(contig, 0);
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
