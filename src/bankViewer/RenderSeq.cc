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

Pos_t RenderSeq_t::getGindex(Pos_t gseqpos) const
{
  Pos_t gindex = 0;
  int seqoffset;

  if (m_rc)
  {
    seqoffset = gseqpos - m_tile->range.end;
    gindex = m_offset+m_nucs.size()-1-seqoffset;  // -1 -> size based
  }
  else
  {
    seqoffset = gseqpos - m_tile->range.begin;
    gindex = m_offset + seqoffset;
  }


  return gindex;
}

void RenderSeq_t::load(Bank_t & read_bank, Tile_t * tile)
{
  m_tile = tile;
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

bool RenderSeq_t::hasOverlap(Pos_t rangeStart, // 0-based exact offset of range
                             Pos_t rangeEnd,   // 0-based exact end of range
                             Pos_t seqOffset,  // 0-bases exact offset of seq
                             Pos_t seqLen,     // count of bases of seq (seqend+1)
                             Pos_t contigLen)  // count of bases in contig (contigend+1)
{
  int retval = 1;

  if (seqOffset >= (Pos_t) 0)
  {
    if ((seqOffset > rangeEnd)                  // sequence right flanks
        || (seqOffset + seqLen-1 < rangeStart)) // sequence left flanks
    {
      retval = 0;
    }
  }
  else
  {
    // Negative Offset, test left and right separately
    retval = hasOverlap(rangeStart, rangeEnd, 
                        0, seqLen+seqOffset, contigLen) ||
             hasOverlap(rangeStart, rangeEnd, 
                        contigLen+seqOffset, -seqOffset, contigLen);
  }

  return retval;
}


