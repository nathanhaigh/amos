#include "RenderSeq.hh"

using namespace std;
using namespace AMOS;

RenderSeq_t::RenderSeq_t()
{
  m_trace = NULL;
  m_displayTrace = false;
  m_displaystart = 0;
  m_displayend = 0;
  bgcolor = ' ';
}

RenderSeq_t::~RenderSeq_t()
{ }


char RenderSeq_t::base(Pos_t gindex, bool outsideclr) const
{
  int leftboundary = m_loffset;
  int rightboundary = m_roffset;

  if (outsideclr)
  {
    leftboundary = m_lfoffset;
    rightboundary = m_rfoffset;
  }

  if (gindex < leftboundary || gindex > rightboundary)
  {
    return ' ';
  }
  else
  {
    return m_bases[gindex - m_loffset + m_rangebegin];
  }
}

void RenderSeq_t::getBases(string & alignedBases, Pos_t grangeStart, Pos_t grangeEnd)
{
  alignedBases.clear();

  int gindex;

  // Initialize string with spaces
  if (grangeStart < m_loffset)
  {
    for (gindex = grangeStart; 
         gindex < m_loffset && gindex <= grangeEnd; 
         gindex++)
    {
      alignedBases += ' ';
    }

    grangeStart = m_loffset;
  }


  // Now incorporate the bases
  for (gindex = grangeStart; 
       gindex <= m_roffset && gindex <= grangeEnd; 
       gindex++)
  {
    alignedBases += m_bases[gindex-m_loffset + m_rangebegin];
  }

  // Trailing spaces
  for (gindex = m_roffset+1; gindex <= grangeEnd; gindex++)
  {
    alignedBases += ' ';
  }

}

int RenderSeq_t::qv(Pos_t gindex, bool outsideclr) const
{
  int leftboundary = m_loffset;
  int rightboundary = m_roffset;

  if (outsideclr)
  {
    leftboundary = m_lfoffset;
    rightboundary = m_rfoffset;
  }

  if (gindex < leftboundary || gindex > rightboundary)
  {
    return -1;
  }
  else
  {
    return m_quals[gindex - m_loffset + m_rangebegin] - AMOS::MIN_QUALITY;
  }
}

int RenderSeq_t::pos(Pos_t gindex, bool outsideclr) const
{
  int leftboundary = m_loffset;
  int rightboundary = m_roffset;

  if (outsideclr)
  {
    leftboundary = m_lfoffset;
    rightboundary = m_rfoffset;
  }


  if (gindex < leftboundary)
  {
    return m_pos[m_rangebegin];
  }
  else if (gindex > rightboundary)
  {
    return m_pos[m_rangeend];
  }
  else
  {
    return m_pos[gindex - m_loffset + m_rangebegin];
  }
}

Pos_t RenderSeq_t::getGindex(Pos_t gseqpos) const
{
  Pos_t gindex = 0;
  int seqoffset;

  if (m_rc)
  {
    // compute distance to right aligned position
    seqoffset = gseqpos - m_tile->range.end;
    gindex = m_roffset-seqoffset;  
  }
  else
  {
    // compute distance to left aligned position
    seqoffset = gseqpos - m_tile->range.begin;
    gindex = m_loffset + seqoffset;
  }

  return gindex;
}

void RenderSeq_t::load(DataStore * datastore, Tile_t * tile)
{
  m_tile = tile;
  datastore->fetchRead(tile->source, m_read);
  Range_t range = tile->range;

  m_loffset = tile->offset;
  m_roffset = tile->offset + tile->range.getLength() + tile->gaps.size() - 1; 

  m_rc = 0;
  if (tile->range.begin > tile->range.end) { m_rc = 1; range.swap();} 

  m_rangebegin = m_tile->range.begin;

  // Render the full gapped sequence
  m_bases = m_read.getSeqString();
  m_quals = m_read.getQualString();

  if (m_rc)
  {
    m_rangebegin = m_bases.length() - m_tile->range.begin;

    Reverse_Complement(m_bases);
    reverse(m_quals.begin(), m_quals.end());
  }

  // last aligned base is the first plus the gapped aligned range
  m_rangeend = m_rangebegin + m_tile->range.getLength() + m_tile->gaps.size() -1;

  // Insert gaps
  Pos_t gapcount;
  vector<Pos_t>::const_iterator g;
  for (g =  m_tile->gaps.begin(), gapcount = 0; 
       g != m_tile->gaps.end(); 
       g++, gapcount++)
  {
    unsigned int gappos = *g+gapcount+m_rangebegin;
    m_bases.insert(gappos, 1, '-');

    // qv of a gap is the min of the flanking values
    char lqv = (gappos > 0)              ? m_quals[gappos-1] : -1;
    char rqv = (gappos < m_quals.size()) ? m_quals[gappos]   : -1;
    char gapqv = (lqv < rqv) 
                 ? (lqv != -1) ? lqv : rqv 
                 : (rqv != -1) ? rqv : lqv;

    m_quals.insert(gappos, 1, gapqv);
  }

  m_lfoffset = m_loffset - m_rangebegin;
  m_rfoffset = m_lfoffset + m_bases.size() - 1;
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

void RenderSeq_t::loadTrace(DataStore * datastore)
{
  if (m_trace) { return; }

  m_pos = m_read.getBasePositions();
  cerr << "Load Positions [" << m_pos.size() << "] ";
  if (m_pos.empty()) { cerr << endl; return; }
  
  cerr << "and trace" << endl;
  m_trace = datastore->fetchTrace(m_read);
  if (!m_trace) { cerr << "=NULL" << endl; return; }

  if (m_rc)
  {
    reverse(m_pos.begin(), m_pos.end());

#if RC_TRACE
    vector<int16_t>::iterator p;
    for (p = m_pos.begin(); p != m_pos.end(); p++)
    {
      *p = m_trace->NPoints - *p;
    }
#endif
  }

  // Insert gaps
  Pos_t gapcount;
  vector<Pos_t>::const_iterator g;
  for (g =  m_tile->gaps.begin(), gapcount = 0; 
       g != m_tile->gaps.end(); 
       g++, gapcount++)
  {
    int gappos = *g+gapcount+m_rangebegin;

    int left  = m_pos[gappos-1];
    int right = m_pos[gappos];

    m_pos.insert(m_pos.begin()+gappos, 1, (left+right)/2);
  }
}

int RenderSeq_t::getGSeqPos(int gindex)
{
  int gseqpos;

  if (m_rc)
  {
    int distance = m_roffset - gindex;
    gseqpos = m_tile->range.end + distance;
  }
  else
  {
    int distance = gindex - m_loffset;
    gseqpos = m_tile->range.begin + distance;
  }

  if      (gseqpos < 0)                   { gseqpos = 0; }
  else if (gseqpos >= (int) m_pos.size()) { gseqpos = m_pos.size()-1; }

  return gseqpos;
}

Pos_t RenderSeq_t::gappedLen() const
{
  if (!m_tile) { cerr << "m_tile is NULL!" << endl; return -1;}
  return m_tile->range.getLength() + m_tile->gaps.size();
}

Pos_t RenderSeq_t::fullLen() const
{
  return m_bases.size();
}
