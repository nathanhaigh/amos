
#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include "ContigUtils.hh"

using namespace AMOS;
using namespace std;


int main (int argc, char ** argv)
{

  string leftstitchread;
  string rightstitchread;
  int masteriid = 0;
  int master2iid = 0;
  int patchiid = 0;
  int breakcontig = 0;
  int savetonew = 0;

  int retval = 0;
  AMOS_Foundation * tf = NULL;

  Bank_t master_contig(Contig_t::NCODE);
  Bank_t master_reads(Read_t::NCODE);
  Bank_t master_scaff(Scaffold_t::NCODE);
  BankStream_t master_feat(Feature_t::NCODE);

  Bank_t patch_contig(Contig_t::NCODE);
  Bank_t patch_reads(Read_t::NCODE);
  BankStream_t patch_feat(Feature_t::NCODE);

  try
  {
    string m, m2, p;


    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Replace a region of a master contig with a region of a patch contig\n"
"\n"
"   Usage: stitchContigs [options] master.bnk patch.bnk\n"
"   -b            Break master contig with patch\n"
"   -n            Save results to a new contig\n\n"
"   -L <seqname>  Leftmost read to start patch\n"
"   -R <seqname>  Rightmost read to end patch\n\n"
"   -M <iid>      IID of master contig\n"
"   -m <eid>      EID of master contig\n"
"   -G <iid>      IID of right master contig (when closing gaps)\n"
"   -g <eid>      EID of right master contig (when closing gaps)\n"
"   -P <iid>      IID of patch contig\n"
"   -p <eid>      EID of patch contig\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("L=s", &leftstitchread,  "Left");
    tf->getOptions()->addOptionResult("R=s", &rightstitchread, "Right");
    tf->getOptions()->addOptionResult("M=i", &masteriid,       "Contig IID");
    tf->getOptions()->addOptionResult("G=i", &master2iid,      "Contig IID");
    tf->getOptions()->addOptionResult("P=i", &patchiid,        "Contig IID");

    tf->getOptions()->addOptionResult("m=s", &m,  "Contig EID");
    tf->getOptions()->addOptionResult("g=s", &m2, "Contig EID");
    tf->getOptions()->addOptionResult("p=s", &p,  "Contig EID");

    tf->getOptions()->addOptionResult("b",   &breakcontig, "BreakContig");
    tf->getOptions()->addOptionResult("n",   &savetonew,   "BreakContig");

    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 2)
    {
      cerr << "Usage: stitchContigs [options] master.bnk patch.bnk" << endl;
      while (argvv.size() != 0)
      {
        cerr << "argv: " << argvv.front() << endl;
        argvv.pop_front();
      }
      return 0;
    }

    int reads = (rightstitchread.empty() ? 0 : 1) + (leftstitchread.empty() ? 0 : 1);
    if ((reads == 0) || (reads == 1 && !breakcontig) || (reads==2 && breakcontig))
    {
      cerr << "You must specify both stitch reads, or 1 stitch read and -b" << endl;
      return 1;
    }

    string masterbank = argvv.front(); argvv.pop_front();
    string patchbank  = argvv.front(); argvv.pop_front();

    cerr << "Starting stitch at " << Date() << endl;

    // Initialize

    master_reads.open(masterbank, B_READ|B_WRITE);
    master_contig.open(masterbank, B_READ|B_WRITE);
    master_feat.open(masterbank, B_READ|B_WRITE);
    master_scaff.open(masterbank, B_READ|B_WRITE);

    patch_reads.open(patchbank, B_READ);
    patch_contig.open(patchbank, B_READ);
    patch_feat.open(patchbank, B_READ);

    if (!m.empty())  { masteriid  = lookupIID(master_contig, m);  }
    if (!m2.empty()) { master2iid = lookupIID(master_contig, m2); }
    if (!p.empty())  { patchiid   = lookupIID(patch_contig, p);   }

    set<ID_t> patchreads;
    set<ID_t> masterreads;
    Range_t stitchRegion;

    stitchContigs(master_contig, master_reads, master_scaff, master_feat,
                  patch_contig,  patch_reads,  patch_feat,
                  leftstitchread, rightstitchread,
                  masterreads, patchreads,
                  stitchRegion,
                  masteriid, master2iid, patchiid,
                  savetonew, true);



    cout << endl;
    cout << "Replaced " << masterreads.size() 
         << " reads from master with " << patchreads.size()
         << " reads from patch" << endl;

    int dup = 0;
    map<ID_t, ID_t> newpatchreads;
    map<ID_t, ID_t>::iterator npi;

    set<ID_t>::iterator pi;
    set<ID_t>::iterator mi;

    for (pi = patchreads.begin(); pi != patchreads.end(); pi++)
    {
      mi = masterreads.find(*pi);
      if (mi == masterreads.end())
      {
        newpatchreads.insert(make_pair(*pi, 0));
      }
      else
      {
        dup++;
      }
    }

    cout << "Duplicated patch reads: " << dup << endl;
    cout << "New patch reads: " << newpatchreads.size() << endl;

    if (!newpatchreads.empty())
    {
      const IDMap_t & cm = master_contig.getIDMap();
      IDMap_t::const_iterator c;
      for (c = cm.begin(); c != cm.end(); c++)
      {
        Contig_t contig;
        master_contig.fetch(c->iid, contig);

        vector<Tile_t>::iterator ti;

        int patchcount = 0;
        for (ti = contig.getReadTiling().begin(); ti != contig.getReadTiling().end(); ti++)
        {
          npi = newpatchreads.find(ti->source);

          if (npi != newpatchreads.end())
          {
            npi->second = contig.getIID();
            cout << "    " << master_reads.lookupEID(ti->source) << " " << contig.getEID() << endl;
            patchcount++;
          }
        }

        if (patchcount)
        {
          cout << patchcount << " of " << contig.getReadTiling().size() 
               << " reads in contig " << contig.getEID() << " were used in patch!\n";
        }
      }

      for (npi = newpatchreads.begin(); npi != newpatchreads.end(); npi++)
      {
        if (npi->second == 0)
        {
          cout << "    " << master_reads.lookupEID(npi->first) << " NULL" << endl;
        }
      }
    }

    cout << endl;



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
