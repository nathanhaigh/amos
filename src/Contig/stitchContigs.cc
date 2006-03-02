
#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include "ContigUtils.hh"

using namespace AMOS;
using namespace std;

void checkClr(vector<Tile_t>::iterator & mti, vector<Tile_t>::iterator & pti, const string & side)
{
  if (mti->range != pti->range)
  {
    cout << "Clear range of " << side << " stitch read differs!!!" << endl;
    cout << "master: " << mti->range.begin << "," << mti->range.end << endl;
    cout << "patch: "  << pti->range.begin << "," << pti->range.end << endl;

    throw "ERROR";
  }
}

void checkCons(const string & mc, const string & pc, const string & side)
{
  if (mc != pc)
  {
    cout << side << " stitch consensus mismatch!" << endl;
    cout << "pc: " << pc << endl;
    cout << "mc: " << mc << endl;

    throw "ERROR";
  }

  cout << "  " << side << " stitch consensus match (len = " << mc.length() << ")" << endl;
}


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

  Bank_t master_bank(Contig_t::NCODE);
  Bank_t master_reads(Read_t::NCODE);
  Bank_t patch_bank(Contig_t::NCODE);
  Bank_t patch_reads(Read_t::NCODE);

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

    Contig_t master, master2, patch;

    master_reads.open(masterbank, B_READ);
    master_bank.open(masterbank, B_READ);

    patch_reads.open(patchbank, B_READ);
    patch_bank.open(patchbank, B_READ);

    if (!m.empty())  { masteriid = master_bank.lookupIID(m); }
    if (!m2.empty()) { master2iid = master_bank.lookupIID(m2); }
    if (!p.empty())  { patchiid  = patch_bank.lookupIID(p); }

    master_bank.fetch(masteriid, master);
    if (master2iid) { master_bank.fetch(master2iid, master2); }
    patch_bank.fetch(patchiid, patch);

    // master
    string mcons = master.getSeqString();
    string mqual = master.getQualString();

    // patch
    string pcons = patch.getSeqString();
    string pqual = patch.getQualString();

    vector<Tile_t> & ptiling = patch.getReadTiling();
    sort(ptiling.begin(), ptiling.end(), TileOrderCmp());
    vector<Tile_t>::iterator pti = ptiling.begin();

    ID_t mleftiid = 0, pleftiid = 0; 
    ID_t mrightiid = 0, prightiid = 0;

    if (!leftstitchread.empty())
    {
      mleftiid = master_reads.lookupIID(leftstitchread);
      pleftiid = patch_reads.lookupIID(leftstitchread);
    }

    if (!rightstitchread.empty())
    {
      mrightiid = master_reads.lookupIID(rightstitchread);
      prightiid = patch_reads.lookupIID(rightstitchread);
    }

    // new
    string newcons;
    string newqual;
    vector<Tile_t> newtiling;

    int masterbaseoffset = 0;
    int patchbaseoffset = 0;

    int leftfeatoffset = 0;
    int rightfeatoffset = 0;


    sort(master.getReadTiling().begin(), master.getReadTiling().end(), TileOrderCmp());
    vector<Tile_t>::iterator mti = master.getReadTiling().begin();

    set<ID_t> patchreads;
    set<ID_t> masterreads;

    if (!leftstitchread.empty())
    {
      // Find the left read in the master
      vector<Tile_t> & mtiling = master.getReadTiling();

      cout << "Handling Left" << endl;

      int rightmost = 0;
      while (mti != mtiling.end())
      {
        if (mti->offset + mti->getGappedLength() > rightmost)
        {
          rightmost = mti->offset + mti->getGappedLength();
        }

        if (mti->source == mleftiid)
        {
          cout << "  Found left stitch read in master, offset:" << mti->offset << endl;
          masterbaseoffset = mti->offset;
          break;
        }

        newtiling.push_back(*mti);
        mti++;
      }

      if (mti == mtiling.end())
      {
        cout << "ERROR: Didn't find left stitch read in master!!!" << endl;
        throw "Vadim sucks!";
      }

      cout << "  Left Consensus = mcons[0, " << masterbaseoffset - 1 << "]" << endl;

      newcons = mcons.substr(0, masterbaseoffset);
      newqual = mqual.substr(0, masterbaseoffset);

      leftfeatoffset = masterbaseoffset;

      // Find the left read in the patch
      while (pti != ptiling.end())
      {
        if (pti->source == pleftiid)
        {
          patchbaseoffset = pti->offset;
          cout << "  Found left stitch read in patch, offset: " << pti->offset << endl;
          checkClr(mti, pti, "left");
          break;
        }

        pti++;
      }

      if (pti == ptiling.end())
      {
        cout << "Didn't find left read in patch!" << endl;
        throw "ERROR";
      }

      // Check that at least the consensi across the left stitch read are identical
      string pc = pcons.substr(patchbaseoffset,  rightmost-masterbaseoffset);
      string mc = mcons.substr(masterbaseoffset, rightmost-masterbaseoffset);
      checkCons(mc, pc, "Left");
    }

    cout << endl;
    cout << "Handling Patch Region" << endl;

    // pti points at first read to patch
    // stitch the reads into the master

    int porigoffset = 0;
    int rightmost = 0;
    int lentocheck = 0;
    while (pti != ptiling.end())
    {
      if (pti->offset + pti->getGappedLength() > rightmost)
      {
        rightmost = pti->offset + pti->getGappedLength();
      }

      porigoffset = pti->offset;
      pti->offset = pti->offset - patchbaseoffset + masterbaseoffset;
      pti->source = master_reads.lookupIID(patch_reads.lookupEID(pti->source));
      newtiling.push_back(*pti);

      if (pti->source == mrightiid) // right read was converted, so use mrightiid
      {
        cout << "  Found right stitch read in patch, offset: " << porigoffset << endl;

        if (rightmost > porigoffset + pti->getGappedLength())
        {
          cout << "  WARNING: Read in patch extends beyond right stitch read" << endl;
        }

        lentocheck = rightmost - porigoffset;
        rightmost = porigoffset + pti->getGappedLength();

        break;
      }

      patchreads.insert(pti->source);
      pti++;
    }

    if (!rightstitchread.empty() && (pti == ptiling.end()))
    {
      cout << "Didn't find right read in patch" << endl;
      throw "ERROR";
    }

    int len = rightmost - patchbaseoffset;
    cout << "  Patch Consensus[" << newcons.length() 
         << "].append pcons[" << patchbaseoffset << "," << len << "]" << endl;

    newcons.append(pcons, patchbaseoffset, len);
    newqual.append(pqual, patchbaseoffset, len);

    rightfeatoffset = newcons.size() - 1;


    if (!rightstitchread.empty())
    {
      cout << endl;
      cout << "Handling Right" << endl;

      // handle right flank from master
      bool stitchreads = false;
      patchbaseoffset = pti->offset; // pti must exist

      vector<Tile_t> & mtiling = (master2iid) ? master2.getReadTiling() : master.getReadTiling();

      if (master2iid)
      {
        while (mti != master.getReadTiling().end())
        {
          masterreads.insert(mti->source);
          mti++;
        }

        sort(mtiling.begin(), mtiling.end(), TileOrderCmp());
        mti = mtiling.begin();

        mcons = master2.getSeqString();
        mqual = master2.getQualString();
      }

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

          int roffset = mti->offset + mti->getGappedLength();
          cout << "  Found right stitch read in master, offset: " << mti->offset << endl;

          checkClr(mti, pti, "right");

          // Ensure the consensi across the right stitch read are identical
          string mc = mcons.substr(masterbaseoffset, lentocheck);
          string pc = pcons.substr(porigoffset, lentocheck);
          checkCons(mc, pc, "Right");

          len = mcons.length() - roffset;
          cout << "  Right Consensus [" << newcons.length() << "].append mcons[" << roffset << "," << len << "]" << endl;
          newcons.append(mcons, roffset, len);
          newqual.append(mcons, roffset, len);
        }

        if (!stitchreads)
        {
          masterreads.insert(mti->source);
        }

        mti++;
      }

      if (!stitchreads)
      {
        cout << "ERROR: Didn't find right stitch read in master" << endl;
        throw "Help me!";
      }
    }

    master.setReadTiling(newtiling);
    master.setSequence(newcons, newqual);



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
      const IDMap_t & cm = master_bank.getIDMap();
      IDMap_t::const_iterator c;
      for (c = cm.begin(); c != cm.end(); c++)
      {
        Contig_t contig;
        master_bank.fetch(c->iid, contig);

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

    master_bank.close();

    cout << endl;

    // save the new contig
    try
    {
      master_bank.open(masterbank, B_READ|B_WRITE);

      if (savetonew)
      {
        master.setIID(master_bank.getMaxIID()+1);
        master.setEID(master.getEID() + "_stitched");
        master_bank.append(master);

        cout << "Saved new contig i:" << master.getIID() << " e:" << master.getEID() << endl;
      }
      else
      {
        master_bank.replace(master.getIID(), master);
        cout << "Updated contig i:" << master.getIID() << " e:" << master.getEID() << endl;

        if (master2iid) 
        { 
          cout << "Removed contig i:" << master2iid << " e:" << master_bank.lookupEID(master2iid) << endl;
          master_bank.remove(master2iid); 
        }
      }

      master_bank.close();

      BankStream_t feat_bank(Feature_t::NCODE);
      feat_bank.open(masterbank, B_READ|B_WRITE);

      Feature_t feat;
      feat.setRange(Range_t(leftfeatoffset, rightfeatoffset));
      feat.setSource(make_pair(master.getIID(), Contig_t::NCODE));
      feat.setType(Feature_t::JOIN);

      char buffer[16];
      snprintf(buffer, 16, "%d", patchiid);
      feat.setComment((string) "Left: " + leftstitchread + " right: " + rightstitchread + " patch: " + buffer);

      feat_bank << feat;

      feat_bank.close();
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
