
#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include "ContigUtils.hh"

using namespace AMOS;
using namespace std;

bool trimContig(int cleancontig, int lengthtrim, int lefttrim, int righttrim, Contig_t & contig)
{
  bool retval = false;

  if (cleancontig)
  {
    vector<Tile_t> & tiling = contig.getReadTiling();

    if (tiling.size() > 1)
    {
      int leftmost = contig.getLength(), secondleft = contig.getLength();
      int rightmost = 0, secondright = 0;

      vector<Tile_t>::iterator ti;
      for (ti = tiling.begin(); ti != tiling.end(); ti++)
      {
        int loffset = ti->offset;
        int roffset = ti->getRightOffset();

        if (loffset < leftmost)         { secondleft = leftmost; leftmost = loffset; }
        else if (loffset < secondleft)  { secondleft = loffset; }

        if (roffset > rightmost)        { secondright = rightmost; rightmost = roffset; }
        else if (roffset > secondright) { secondright = roffset; }
      }

      lefttrim = secondleft - leftmost;
      righttrim = rightmost - secondright;
    }
  }

  if (lengthtrim || righttrim || lefttrim)
  {
    // trim right first so we don't have to shift trim after left trim
    if (lengthtrim) { lengthTrimContig(contig, lengthtrim); }
    if (righttrim)  { rightTrimContig(contig, righttrim); }
    if (lefttrim)   { leftTrimContig(contig, lefttrim); }

    retval = true;
  }

  return retval;
}


int main (int argc, char ** argv)
{
  int lefttrim(0);
  int righttrim(0);
  int lengthtrim(0);
  int contigiid(0);
  int cleancontig(0);
  int cleanall(0);
  string contigeid;

  int retval = 0;
  AMOS_Foundation * tf = NULL;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Trim a contig by removing a specified number of bases from either the left\n"
"or right end of the contig. Reads that span the trim regions will either\n"
"have their clear range adjusted or be entirely removed from the tiling as\n"
"appropriate.\n"
"\n"
"   Usage: trimContig [options] bnk\n"
"\n"
"   -L <val> Trim val bases from left side\n"
"   -R <val> Trim val bases from right side\n"
"   -N <val> Right trim contig to new length\n"
"   -c       Clean contigs by trimming 1x coverage at ends\n"
"   -E id    EID of contig to trim\n"
"   -I id    IID of contig to trim\n"
"   -C       Clean all contigs\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("L=i", &lefttrim,    "Left trim amount");
    tf->getOptions()->addOptionResult("R=i", &righttrim,   "Right trim amount");
    tf->getOptions()->addOptionResult("N=i", &lengthtrim,  "New Contig Length");
    tf->getOptions()->addOptionResult("c",   &cleancontig, "Clean Contig");
    tf->getOptions()->addOptionResult("C",   &cleanall,    "Clean All");

    tf->getOptions()->addOptionResult("I=i", &contigiid, "Contig IID");
    tf->getOptions()->addOptionResult("E=s", &contigeid, "Contig EID");
    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 1)
    {
      cerr << "Usage: trimContig [options] bankname" << endl;
      return EXIT_FAILURE;
    }

    string bankname = argvv.front(); argvv.pop_front();
    Bank_t contig_bank(Contig_t::NCODE);

    cerr << "Processing " << bankname << " at " << Date() << endl;

    contig_bank.open(bankname, B_READ|B_WRITE);
    Contig_t contig;

    if (cleanall)
    {
      const IDMap_t & map = contig_bank.getIDMap();
      IDMap_t::const_iterator ci;
      for (ci = map.begin(); ci != map.end(); ci++)
      {
        contig_bank.fetch(ci->iid, contig);
        if (trimContig(1,0,0,0,contig))
        {
          contig_bank.replace(ci->iid, contig);
        }
      }
    }
    else
    {
      if (!contigeid.empty()) { contig_bank.fetch(contigeid, contig); }
      else                    { contig_bank.fetch(contigiid, contig); }

      if (trimContig(cleancontig, lengthtrim, lefttrim, righttrim, contig))
      {
        contig_bank.replace(contig.getIID(), contig);
      }
    }

    contig_bank.close();
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
