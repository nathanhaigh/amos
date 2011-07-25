#ifndef PERSISTENT_UTILS_HH
#define PERSISTENT_UTILS_HH 1

#include "foundation_AMOS.hh"

using namespace AMOS;
using namespace std;


class ReadPosInfo
{
public:
  ReadPosInfo(ID_t iid, ID_t contigiid, int offset, int gappedlen, int tileid, bool rc)
    : m_iid(iid), m_contigiid(contigiid), m_offset(offset), m_gappedlen(gappedlen), m_tileid(tileid), m_rc(rc) 
  {}

  ID_t m_iid;
  ID_t m_contigiid;
  int m_offset;
  int m_gappedlen;
  int m_tileid;
  bool m_rc;

  bool rc() const  { return m_rc; }
  ID_t iid() const { return m_iid; }
  ID_t contig() const { return m_contigiid; }

  int getRightOffset() const 
  { return m_offset + m_gappedlen - 1; }

  int pos(bool use3) const 
  {
    if (use3) { return pos3();}
    return pos5();
  }

  int pos5() const
  {
    int retval = m_offset;
    if (m_rc) { retval = getRightOffset(); }
    return retval;
  }

  int pos3() const
  {
    int retval = getRightOffset();
    if (m_rc) { retval = m_offset; }
    return retval;
  }
};


int computeDistance(Tile_t * lefttile, Tile_t * righttile)
{
  return righttile->offset - lefttile->offset;
}

void computeDelta(Tile_t * leftmtile,  ReadPosInfo * leftptile, 
                  Tile_t * rightmtile, ReadPosInfo * rightptile,
                  int & consistent, int & delta, int & mdist, int & pdist)
{
  bool mlrc = leftmtile->range.isReverse();
  bool mrrc = rightmtile->range.isReverse();

  bool plrc = leftptile->rc();
  bool prrc = rightptile->rc();

  bool masterinnie = mlrc ^ mrrc;
  bool patchinnie = plrc ^ prrc;

  consistent = (masterinnie == patchinnie);

  mdist = computeDistance(leftmtile, rightmtile);
  pdist = 0;

  int rpos = rightptile->pos(mrrc);
  int lpos = leftptile->pos(mlrc);

  if (consistent)
  {
    // -A-> -B->
    if (mlrc == plrc)
    {
      // -A-> -B->

      if (rpos < lpos) { consistent = 0; } // -B-> -A->
      pdist = rpos - lpos;
    }
    else
    {
      // <-B- <-A-
      if (lpos < rpos) { consistent = 0; } // <-A- <-B-
      pdist = lpos - rpos;
    }
  }

  // 1000 - 0 = 1000 bp collapse
  delta = pdist - mdist;
}


typedef map<ID_t, ReadPosInfo > ReadPosLookup;

int get5Offset(Tile_t tile)
{
  if (tile.range.isReverse()) { return tile.getRightOffset(); }
  return tile.offset;
}

void recordReadPositions(ID_t iid, vector<Tile_t> & tiling, ReadPosLookup & read2contigpos)
{
  sort(tiling.begin(), tiling.end(), TileOrderCmp());

  int tileid;
  vector<Tile_t>::iterator ti;
  for (ti = tiling.begin(), tileid = 0; ti != tiling.end(); ti++, tileid++)
  {
    read2contigpos.insert(make_pair(ti->source, 
                                    ReadPosInfo(ti->source, 
                                                iid, 
                                                ti->offset, 
                                                ti->getGappedLength(),
                                                tileid,
                                                ti->range.isReverse())));
  }
}

void recordContigPositions(Bank_t & contig_bank, ReadPosLookup & read2contigpos)
{
  // Record the position of every read in the patch bank
  IDMap_t::const_iterator c;
  const IDMap_t & patchmap = contig_bank.getIDMap();
  for (c = patchmap.begin(); c!= patchmap.end(); c++)
  {
    Contig_t contig;
    contig_bank.fetch(c->iid, contig);

    recordReadPositions(contig.getIID(), contig.getReadTiling(), read2contigpos);
  }
}






#endif
