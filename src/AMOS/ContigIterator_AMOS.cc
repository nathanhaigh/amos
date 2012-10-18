// $Id$

#include "ContigIterator_AMOS.hh"

using namespace std;
using namespace AMOS;


TiledRead_t::TiledRead_t(Tile_t tile, Read_t red, int readidx)
    : m_seq(red.getSeqString(tile.range)), 
      m_qual(red.getQualString(tile.range)), 
      m_readidx(readidx),
      m_fragid(red.getFragment()),
      m_iid(red.getIID()),
      m_eid(red.getEID())
{
  int gappos;

  try
  {
    for (int i = 0; i < tile.gaps.size(); i++)
    {
      gappos = tile.gaps[i] + i;
      m_seq.insert(gappos, 1,  '-');

      char lqv = (gappos > 0) ? m_qual[gappos-1] : -1;
      char rqv = (gappos < m_qual.size()) ? m_qual[gappos] : -1;

      char gapqv = (lqv < rqv)
                   ? (lqv != -1) ? lqv : rqv
                   : (rqv != -1) ? rqv : lqv;

      m_qual.insert(gappos, 1, gapqv);
    }
  }
  catch(...)
  {
    AMOS_THROW_ARGUMENT((string)"Error inserting gaps into read: " + red.getEID());
  }

  m_loffset = tile.offset;
  m_roffset = tile.offset + tile.getGappedLength() - 1;
  m_isRC = tile.range.isReverse();

  int l = m_seq.size();

  for (int i = 0; i < l; i++)
  {
    m_seq[i] = toupper(m_seq[i]);
  }
}

int32_t ContigIterator_t::s_tilingreadsoffset(0);

ContigIterator_t::ContigIterator_t(Contig_t & ctg, Bank_t * rdbank)
 : m_tilingreadsoffset(s_tilingreadsoffset)
{
  m_gindex = -1;
  m_uindex = 1;

  m_readBank = rdbank;
  m_contig = ctg;

  m_consensus = m_contig.getSeqString();
  m_consqual = m_contig.getQualString();

  vector<Tile_t> & tiling = m_contig.getReadTiling();
  sort(tiling.begin(), tiling.end(), TileOrderCmp());
  m_currenttile = 0;
  m_numreads = tiling.size();
  s_tilingreadsoffset += m_numreads;
}

Pos_t ContigIterator_t::uindex() const
{
  if (m_consensus[m_gindex] == '-' && m_uindex > 1)
  {
    return m_uindex-1;
  }

  return m_uindex;
}

void ContigIterator_t::renderTile(Tile_t & tile, int tilingindex)
{
  Read_t rd;

  m_readBank->fetch(tile.source, rd);
  TiledRead_t trd(tile, rd, tilingindex+m_tilingreadsoffset);

  TiledReadList_t::iterator ins = m_tilingreads.insert(m_tilingreads.end(), trd);

  m_ends.push(ins);
}


// Move to the next position
bool ContigIterator_t::advanceNext()
{
  int len = m_consensus.size();
  if (m_gindex >= len) { return false; }

  if (m_gindex != -1 && m_consensus[m_gindex] != '-') { m_uindex++; }
  m_gindex++;

  vector<Tile_t> & tiling = m_contig.getReadTiling();

  while ((m_currenttile < m_numreads) && (tiling[m_currenttile].offset <= m_gindex))
  { 
    renderTile(tiling[m_currenttile], m_currenttile);
    m_currenttile++;
  }

  while (!m_ends.empty() && (m_ends.top()->m_roffset < m_gindex))
  { 
    m_tilingreads.erase(m_ends.top());
    m_ends.pop();
  }

  return (m_gindex < len);
}

// Seek to a random position in the contig
bool ContigIterator_t::seek(Pos_t gindex)
{
  if (gindex >= m_consensus.size()) { return false; }

  bool reuseexistingtiling = (gindex >= m_gindex);

  if (!reuseexistingtiling)
  {
    // Can't go backwards, so flush the tiling and reload from scratch
    m_tilingreads.clear();
    while (!m_ends.empty()) { m_ends.pop(); }
    m_currenttile = 0;
  }

  m_gindex = gindex;
  m_uindex = m_contig.gap2ungap(gindex);

  // load the reads that overlap this position
  vector<Tile_t> & tiling = m_contig.getReadTiling();

  while (m_currenttile < m_numreads && tiling[m_currenttile].offset <= m_gindex)
  {
    Pos_t roffset = tiling[m_currenttile].offset + tiling[m_currenttile].getGappedLength() - 1;
    if (roffset >= m_gindex)
    {
      // This read covers the seek position
      renderTile(tiling[m_currenttile], m_currenttile);
    }

    m_currenttile++;
  }

  if (reuseexistingtiling)
  {
    while (!m_ends.empty() && m_ends.top()->m_roffset < m_gindex)
    { 
      m_tilingreads.erase(m_ends.top());
      m_ends.pop();
    }
  }

  return true;
}


bool ContigIterator_t::hasSNP() const
{
  TiledReadList_t::const_iterator tle;

  char lastbase = ' ';
  for (tle = m_tilingreads.begin(); tle != m_tilingreads.end(); tle++)
  {
    char base = tle->base(m_gindex);

    if (lastbase != ' ' && lastbase != base)
    {
      return true;
    }

    lastbase = base;
  }

  return false;
}

BaseStats_t::BaseStats_t(char base)
 : m_base(base), m_cumqv(0), m_maxqv(0)
{ }

void BaseStats_t::addRead(TiledReadList_t::const_iterator tile, Pos_t gindex)
{
  int qv = tile->qv(gindex);
  m_cumqv += qv;

  if (qv > m_maxqv) { m_maxqv = qv; }

  m_reads.push_back(tile);
}

Column_t::Column_t(ContigIterator_t & ci)
 : m_gindex(ci.gindex()),
   m_uindex(ci.uindex()),
   m_cons(ci.cons()),
   m_cqv(ci.cqv()),
   m_depth(0)
{
  TiledReadList_t::const_iterator tle;
  map<char, BaseStats_t>::iterator si;

  for (tle = ci.getTilingReads().begin(); tle != ci.getTilingReads().end(); tle++)
  { 
    assert(m_gindex >= tle->m_loffset &&
           m_gindex <= tle->m_roffset);

    m_depth++;

    char base = tle->base(m_gindex);

    si = m_baseinfo.find(base);
    if (si == m_baseinfo.end())
    {
      si = m_baseinfo.insert(make_pair(base, BaseStats_t(base))).first;
    }

    si->second.addRead(tle, m_gindex);
  }
}

Column_t ContigIterator_t::getColumn()
{
  return Column_t(*this);
}

vector<BaseStats_t *> Column_t::getBaseInfo()
{
  vector<BaseStats_t *> retval;

  for (map<char, BaseStats_t>::iterator si = m_baseinfo.begin();
       si != m_baseinfo.end();
       si++)
  {
    retval.push_back(&si->second);
  }

  sort(retval.begin(), retval.end(), BaseStatsCmp());

  return retval;
}

