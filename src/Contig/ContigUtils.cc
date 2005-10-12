#include "ContigUtils.hh"

using namespace std;
using namespace AMOS;

// Trim 'lefttrim' bases from a contig by trimming the consensus and then
// adjusting the offset and clear range of the tiling reads, or by 
// dropping the read entirely from the multiple alignment
void leftTrimContig(Contig_t & contig, int lefttrim)
{
  string cons = contig.getSeqString();
  string cqual = contig.getQualString();

  cerr << "Left Trimming " << lefttrim << " bases from consensus length of " << cons.length() << endl;

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


// Right trim the contig to the new length
void lengthTrimContig(Contig_t & contig, int newlength)
{
  string cons = contig.getSeqString();
  string cqual = contig.getQualString();

  int righttrim = cons.length() - newlength;

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

// Trim 'righttrim' bases from a contig by trimming the last righttrim bases
// from the consensus, and then adjusting the clear range of the tiling
// reads, or by dropping them entirely from the contig.
void rightTrimContig(Contig_t & contig, int righttrim)
{
  int clen = contig.getLength();

  lengthTrimContig(contig, clen-righttrim);
}


// Slide the tiling to the right by contigShiftDistance, insert - into
// the consensus
void shiftContigRight(Contig_t & contig, int contigShiftDistance)
{
  string cons = contig.getSeqString();
  string cqual = contig.getQualString();

  cons.insert(cons.begin(), contigShiftDistance, '-');
  cqual.insert(cqual.begin(), contigShiftDistance, 'X');

  contig.setSequence(cons, cqual);

  vector<Tile_t>::iterator ti;
  for (ti =  contig.getReadTiling().begin();
       ti != contig.getReadTiling().end();
       ti++)
  {
    ti->offset += contigShiftDistance;
  }
}

// Add contigGrowDistance gaps to the consensus
void widenContig(Contig_t & contig, int contigGrowDistance)
{
  string cons = contig.getSeqString();
  string cqual = contig.getQualString();

  cons.append(contigGrowDistance, '-');
  cqual.append(contigGrowDistance, 'X');

  contig.setSequence(cons, cqual);
}


