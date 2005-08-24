
#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"

using namespace AMOS;
using namespace std;

// Trim 'lefttrim' bases from a contig by trimming the consensus and then
// adjusting the offset and clear range of the tiling reads, or by 
// dropping the read entirely from the multiple alignment
void leftTrimContig(Contig_t & contig, int lefttrim)
{
  string cons = contig.getSeqString();
  string cqual = contig.getQualString();

  cerr << "Left Trimming " << lefttrim << " bases from " << cons.length() << endl;

  cons.erase(0, lefttrim);
  cqual.erase(0, lefttrim);

  contig.setSequence(cons,cqual);

  int curread = 0;
  int tilecount = 0;
  vector<Tile_t> & tiling = contig.getReadTiling();

  for (curread = 0; curread < tiling.size(); curread++)
  {
    if (tiling[curread].getRightOffset() >= lefttrim)
    {
      // This read should exist in the new tiling
      tiling[curread].offset -= lefttrim;
      
      if (tiling[curread].offset < 0)
      {
        // fix range and gaps
        int trim = -tiling[curread].offset;
        tiling[curread].offset = 0;

        vector<Pos_t> & gaps = tiling[curread].gaps;
        int curgap = 0;
        int gapstodrop = 0;
        int gapstokeep = 0;

        for (curgap = 0; curgap < gaps.size(); curgap++)
        {
          if (gaps[curgap] + curgap < trim) { gapstodrop++; }
          else
          {
            gaps[gapstokeep] = gaps[curgap]-(trim-gapstodrop);
            gapstokeep++;
          }
        }

        gaps.resize(gapstokeep);

        if (tiling[curread].range.isReverse())
        {
          tiling[curread].range.begin -= (trim - gapstodrop);
        }
        else
        {
          tiling[curread].range.begin += (trim - gapstodrop);
        }
      }

      tiling[tilecount] = tiling[curread];
      tilecount++;
    }
  }

  tiling.resize(tilecount);
}

// Trim 'righttrim' bases from a contig by trimming the last righttrim bases
// from the consensus, and then adjusting the clear range of the tiling
// reads, or by dropping them entirely from the contig.
void rightTrimContig(Contig_t & contig, int righttrim)
{
  string cons = contig.getSeqString();
  string cqual = contig.getQualString();

  int newlength = cons.length() - righttrim;

  cerr << "Right Trimming " << righttrim << " bases from " << cons.length() << endl;

  cons.resize(newlength);
  cqual.resize(newlength);

  contig.setSequence(cons,cqual);

  int curread = 0;
  int tilecount = 0;
  vector<Tile_t> & tiling = contig.getReadTiling();

  for (curread = 0; curread < tiling.size(); curread++)
  {
    if (tiling[curread].offset < newlength)
    {
      // This read should exist in the new tiling

      if (tiling[curread].getRightOffset() >= newlength)
      {
        // This read extends beyond new width
        // fix range and gaps
        int trim = tiling[curread].getRightOffset() - newlength + 1;
        int last = newlength - 1 - tiling[curread].offset;

        vector<Pos_t> & gaps = tiling[curread].gaps;
        int curgap = 0;
        int gapstodrop = 0;
        int gapstokeep = 0;

        for (curgap = 0; curgap < gaps.size(); curgap++)
        {
          if (gaps[curgap] + curgap > last) { gapstodrop++; }
          else
          {
            gaps[gapstokeep] = gaps[curgap];
            gapstokeep++;
          }
        }

        gaps.resize(gapstokeep);

        if (tiling[curread].range.isReverse())
        {
          tiling[curread].range.end += (trim - gapstodrop);
        }
        else
        {
          tiling[curread].range.end -= (trim - gapstodrop);
        }
      }

      tiling[tilecount] = tiling[curread];
      tilecount++;
    }
  }

  tiling.resize(tilecount);
}

int main (int argc, char ** argv)
{
  int lefttrim(0);
  int righttrim(0);
  int contigiid(0);
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
"   -E id    EID of contig to trim\n"
"   -I id    IID of contig to trim\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("L=i", &lefttrim, "Left trim amount");
    tf->getOptions()->addOptionResult("R=i", &righttrim, "Right trim amount");

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

    if (!contigeid.empty()) { contig_bank.fetch(contigeid, contig); }
    else                    { contig_bank.fetch(contigiid, contig); }

    // trim right first so we don't have to shift trim after left trim
    if (righttrim) { rightTrimContig(contig, righttrim); }
    if (lefttrim)  { leftTrimContig(contig, lefttrim); }

    contig_bank.replace(contig.getIID(), contig);
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
