// $Id$

#include "ContigIterator.hh"

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
  for (int i = 0; i < tile.gaps.size(); i++)
  {
    int gappos = tile.gaps[i] + i;
    m_seq.insert(gappos, 1,  '-');

    char lqv = (gappos > 0) ? m_qual[gappos-1] : -1;
    char rqv = (gappos < m_qual.size()) ? m_qual[gappos] : -1;

    char gapqv = (lqv < rqv)
                 ? (lqv != -1) ? lqv : rqv
                 : (rqv != -1) ? rqv : lqv;

    m_qual.insert(gappos, 1, gapqv);
  }

  m_loffset = tile.offset;
  m_roffset = tile.offset + tile.getGappedLength() - 1;
  m_isRC = tile.range.isReverse();
}

int ContigIterator_t::s_tilingreadsoffset(0);

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
  sort(tiling.begin(), tiling.end(), cmpTile());
  m_currenttile = 0;
  m_tilingreads = tiling.size();
  s_tilingreadsoffset += m_tilingreads;
}

int ContigIterator_t::uindex() const
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

  ReadList_t::iterator ins = m_tiled.insert(m_tiled.end(), trd);

  m_ends.push(ins);
}


// Move to the next position
bool ContigIterator_t::advanceNext()
{
  if (m_gindex != -1 && m_consensus[m_gindex] != '-') { m_uindex++; }
  m_gindex++;

  TileVector_t & tiling = m_contig.getReadTiling();

  while (m_currenttile < m_tilingreads && tiling[m_currenttile].offset <= m_gindex)
  { 
    renderTile(tiling[m_currenttile], m_currenttile);
    m_currenttile++;
  }

  while (!m_ends.empty() && m_ends.top()->m_roffset < m_gindex)
  { 
    m_tiled.erase(m_ends.top());
    m_ends.pop();
  }

  return !(m_gindex == m_consensus.size());
}

// Seek to a random position in the contig
bool ContigIterator_t::seek(Pos_t gindex)
{
  if (gindex >= m_consensus.size()) { return false; }

  bool reuseexistingtiling = (gindex >= m_gindex);

  if (!reuseexistingtiling)
  {
    // Can't go backwards, so flush the tiling and reload from scratch
    m_tiled.clear();
    while (!m_ends.empty()) { m_ends.pop(); }
    m_currenttile = 0;
  }

  m_gindex = gindex;
  m_uindex = m_contig.gap2ungap(gindex)+1;

  // load the reads that overlap this position
  TileVector_t & tiling = m_contig.getReadTiling();

  while (m_currenttile < m_tilingreads && tiling[m_currenttile].offset <= m_gindex)
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
      m_tiled.erase(m_ends.top());
      m_ends.pop();
    }
  }

  return true;
}


bool ContigIterator_t::hasSNP() const
{
  ReadList_t::const_iterator tle;
  ReadList_t::const_iterator end;

  char lastbase = ' ';
  for (tle = m_tiled.begin(); tle != m_tiled.end(); tle++)
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
 : m_base(base), 
   m_count(0),
   m_cumqv(0),
   m_maxqv(0)
{ }

void BaseStats_t::addRead(ReadList_t::iterator & tile, Pos_t gindex)
{
  m_count++;

  int qv = tile->qv(gindex);
  m_cumqv += qv;

  if (qv > m_maxqv) { m_maxqv = qv; }

  m_reads.push_back(tile);
}

Column_t::Column_t()
 : m_depth(0)
{ }

Column_t ContigIterator_t::getColumn()
{
  Column_t column;
  ReadList_t::iterator tle;
  StatsMap::iterator si;

  for (tle = m_tiled.begin(); tle != m_tiled.end(); tle++)
  { 
    assert(m_gindex >= tle->m_loffset &&
           m_gindex <= tle->m_roffset);

    column.m_depth++;

    char base = tle->base(m_gindex);

    si = column.m_baseinfo.find(base);
    if (si == column.m_baseinfo.end())
    {
      BaseStats_t stats(base);
      si = column.m_baseinfo.insert(make_pair(base, stats)).first;
    }

    si->second.addRead(tle, m_gindex);
  }

  for (si =  column.m_baseinfo.begin();
       si != column.m_baseinfo.end();
       si++)
  {
    column.m_basefrequencies.push_back(&si->second);
    column.m_baseqv.push_back(&si->second);
  }

  sort(column.m_basefrequencies.begin(), column.m_basefrequencies.end(), BaseStatsFreqCmp());
  sort(column.m_baseqv.begin(),          column.m_baseqv.end(),          BaseStatsQVCmp());

  return column;
}

