#include "RenderSeq.hh"

RenderSeq_t::RenderSeq_t(int vectorpos)
{
  m_vectorpos = vectorpos;
}

RenderSeq_t::~RenderSeq_t()
{ }


char RenderSeq_t::base(Pos_t gindex) const
{
  if (gindex < m_offset || gindex >= m_offset + m_nucs.size())
  {
    return ' ';
  }
  else
  {
    return m_nucs[gindex - m_offset];
  }
}

void RenderSeq_t::load(Bank_t & read_bank, vector<Tile_t>::const_iterator tile)
{
  read_bank.fetch(tile->source, m_read);
  Range_t range = tile->range;

  m_offset = tile->offset;
  m_rc = 0;
  if (tile->range.begin > tile->range.end) { m_rc = 1; range.swap();} 

  m_nucs = m_read.getSeqString(range);
  m_qual = m_read.getQualString(range);

  if (m_rc) 
  { 
    Reverse_Complement(m_nucs); 
    reverse (m_qual.begin(), m_qual.end());
  }

  // Insert gaps
  Pos_t gapcount;
  vector<Pos_t>::const_iterator g;
  for (g =  tile->gaps.begin(), gapcount = 0; 
       g != tile->gaps.end(); 
       g++, gapcount++)
  {
    m_nucs.insert(*g+gapcount, 1, '-');

    // qv of a gap is the min of the flanking values
    char lqv = (*g+gapcount > 0) ? m_qual[*g+gapcount-1] : -1;
    char rqv = (*g+gapcount < m_qual.size()) ? m_qual[*g+gapcount] : -1;
    char gapqv = (lqv < rqv) 
                 ? (lqv != -1) ? lqv : rqv 
                 : (rqv != -1) ? rqv : lqv;

    m_qual.insert(*g+gapcount, 1, gapqv);
  }
}

