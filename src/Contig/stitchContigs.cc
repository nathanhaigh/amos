
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
  int masteriid;
  int patchiid;

  int retval = 0;
  AMOS_Foundation * tf = NULL;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Stitch together 2 contigs to replace the middle of one contig with a patch\n"
"\n"
"   Usage: stitchContigs [options] master.bnk patch.bnk\n"
"   -L <seqname>  Leftmost read to start patch\n"
"   -R <seqname>  Rightmost read to end patch\n"
"   -M <iid>      IID of master contig\n"
"   -P <iid>      IID of patch contig\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("L=s", &leftstitchread, "Left");
    tf->getOptions()->addOptionResult("R=s", &rightstitchread, "Right");
    tf->getOptions()->addOptionResult("M=i", &masteriid, "Contig IID");
    tf->getOptions()->addOptionResult("P=i", &patchiid, "Contig IID");

    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 2)
    {
      cerr << "Usage: trimContig [options] master.bnk patch.bnk" << endl;
      return EXIT_FAILURE;
    }

    string masterbank = argvv.front(); argvv.pop_front();
    string patchbank  = argvv.front(); argvv.pop_front();

    Bank_t master_bank(Contig_t::NCODE);
    Bank_t patch_bank(Contig_t::NCODE);

    Bank_t master_reads(Read_t::NCODE);
    Bank_t patch_reads(Read_t::NCODE);

    cerr << "Starting stitch at " << Date() << endl;

    master_bank.open(masterbank, B_READ);
    master_reads.open(masterbank, B_READ);

    patch_bank.open(patchbank, B_READ);
    patch_reads.open(patchbank, B_READ);

    Contig_t master;
    Contig_t patch;
    master_bank.fetch(masteriid, master);
    patch_bank.fetch(patchiid, patch);

    string mcons = master.getSeqString();
    string mqual = master.getQualString();

    string pcons = patch.getSeqString();
    string pqual = patch.getQualString();

    vector<Tile_t> newtiling;

    ID_t mleftiid = master_reads.lookupIID(leftstitchread);
    ID_t mrightiid = master_reads.lookupIID(rightstitchread);

    ID_t pleftiid = patch_reads.lookupIID(leftstitchread);
    ID_t prightiid = patch_reads.lookupIID(rightstitchread);

    vector<Tile_t>::iterator mti;
    vector<Tile_t>::iterator pti;

    vector<Tile_t> & mtiling = master.getReadTiling();
    sort(mtiling.begin(), mtiling.end(), TileOrderCmp());

    vector<Tile_t> & ptiling = patch.getReadTiling();
    sort(ptiling.begin(), ptiling.end(), TileOrderCmp());


    // Find the left read in the master
    int mcount = 0;
    int rightmost = 0;

    for (mti = mtiling.begin();
         mti != mtiling.end();
         mti++, mcount++)
    {
      if (mti->offset + mti->getGappedLength() > rightmost)
      {
        rightmost = mti->offset + mti->getGappedLength();
      }

      if (mti->source == mleftiid)
      {
        cout << "Found left stitch read in master (" << mcount << ")" << endl;
        break;
      }

      newtiling.push_back(*mti);
    }

    if (mti == mtiling.end())
    {
      cout << "ERROR: Didn't find left stitch read in master!!!" << endl;
      throw "Vadim sucks!";
    }


    int masterbaseoffset = mti->offset;

    cout << "Left Consensus = mcons[0, " << masterbaseoffset - 1 << "]" << endl;

    string cons = mcons.substr(0, masterbaseoffset);
    string qual = mqual.substr(0, masterbaseoffset);



    // find the left read in the patch, copy those reads into the new vector untit right read
    int pcount = 0;

    int origoffset = 0;
    int patchbaseoffset = -1;
    bool stitchreads = false;
    for (pti =  ptiling.begin();
         pti != ptiling.end();
         pti++, pcount++)
    {
      if (pti->source == pleftiid)
      {
        patchbaseoffset = pti->offset;
        stitchreads = true;
        cout << "Found left stitch read in patch (" << pcount << ")" << endl;
      }

      if (stitchreads)
      {
        origoffset = pti->offset;
        pti->offset = pti->offset - patchbaseoffset + masterbaseoffset;

        pti->source = master_reads.lookupIID(patch_reads.lookupEID(pti->source));
        newtiling.push_back(*pti);
      }

      if (pti->source == mrightiid) // right read was converted, so use mrightiid
      {
        if (!stitchreads)
        {
          cout << "ERROR: Didn't find left stitch read in patch!!!" << endl;
          throw "ERROR";
        }

        cout << "Found right stitch read in patch (" << pcount << ")" << endl;
        break;
      }
    }

    if (patchbaseoffset == -1)
    {
      cout << "Didn't find left or right read in patch!" << endl;
      throw "ERROR";
    }

    if (pti == ptiling.end())
    {
      cout << "Didn't find right read in patch" << endl;
      throw "ERROR";
    }

    int len = origoffset + pti->getGappedLength() - patchbaseoffset;
    cout << "Patch Consensus[" << cons.length() << "].append pcons[" << patchbaseoffset << "," << len << "]" << endl;

    if (len < rightmost-masterbaseoffset)
    {
      cout << "Patch region too short" << endl;
      throw "Save me";
    }

    string pc = pcons.substr(patchbaseoffset,  rightmost-masterbaseoffset);
    string mc = mcons.substr(masterbaseoffset, rightmost-masterbaseoffset);

    if (mc != pc)
    {
      cout << "Left stitch consensus mismatch!" << endl;
      cout << "pc: " << pc << endl;
      cout << "mc: " << mc << endl;

      throw "Save me!";
    }
    else
    {
      cout << "Left stitch consensus match (len = " << mc.length() << ")" << endl;
    }

    cons.append(pcons, patchbaseoffset, len);
    qual.append(pqual, patchbaseoffset, len);

    stitchreads = false;
    patchbaseoffset = pti->offset;

    int startroffset;

    while (mti != mtiling.end())
    {
      if (stitchreads)
      {
        mti->offset = mti->offset - masterbaseoffset + patchbaseoffset;
        newtiling.push_back(*mti);
      }

      // mrightiid is the last read from patch, don't stitch it
      if (mti->source == mrightiid)
      {
        stitchreads = true;
        masterbaseoffset = mti->offset;
        startroffset = mti->offset + mti->getGappedLength();
        cout << "Found right stitch read in master (" << mcount << ")" << endl;
      }

      mcount++; mti++;
    }

    if (!stitchreads)
    {
      cout << "ERROR: Didn't find right stitch read in master" << endl;
      throw "Help me!";
    }

    mc = mcons.substr(masterbaseoffset, startroffset - masterbaseoffset);
    pc = pcons.substr(origoffset, pti->getGappedLength());

    if (mc != pc)
    {
      cout << "right stitch consensus mismatch!" << endl;
      cout << "pc: " << pc << endl;
      cout << "mc: " << mc << endl;

      throw "Save me!";
    }
    else
    {
      cout << "right stitch consensus match (len = " << mc.length() << ")" << endl;
    }

    len = mcons.length() - startroffset;
    cout << "Right Consensus [" << cons.length() << "].append mcons[" << startroffset << "," << len << "]" << endl;
    cons.append(mcons, startroffset, len);
    qual.append(mcons, startroffset, len);

    master.setReadTiling(newtiling);
    master.setSequence(cons, qual);

    master_bank.close();

    try
    {
      master_bank.open(masterbank, B_READ|B_WRITE);
      master_bank.replace(master.getIID(), master);
      master_bank.close();
    }
    catch (Exception_t & e)
    {
      cerr << "ERROR Saving results!!!\n" << e;
      return 1;
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
