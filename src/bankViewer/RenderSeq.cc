#include "RenderSeq.hh"

using namespace std;
using namespace AMOS;

RenderSeq_t::RenderSeq_t(int vectorpos)
{
  m_vectorpos = vectorpos;
  m_trace = NULL;
  m_displayTrace = false;
  m_displaystart = 0;
  m_displayend = 0;
}

RenderSeq_t::~RenderSeq_t()
{ }


char RenderSeq_t::base(Pos_t gindex) const
{
  if (gindex < m_loffset || gindex > m_roffset)
  {
    return ' ';
  }
  else
  {
    return m_nucs[gindex - m_loffset];
  }
}

int RenderSeq_t::qv(Pos_t gindex) const
{
  if (gindex < m_loffset || gindex > m_roffset)
  {
    return -1;
  }
  else
  {
    return m_qual[gindex - m_loffset] - AMOS::MIN_QUALITY;
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

void RenderSeq_t::load(Bank_t & read_bank, Tile_t * tile)
{
  m_tile = tile;
  read_bank.fetch(tile->source, m_read);
  Range_t range = tile->range;

  m_loffset = tile->offset;
  m_roffset = tile->offset + tile->range.getLength() + tile->gaps.size() - 1; 

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

void RenderSeq_t::loadTrace(const string & db)
{
  if (m_trace) { return; }

  string readname = m_read.getEID();

  string path = "/local/chromo/Chromatograms/";
  path += db + "/ABISSed/" + readname[0]+readname[1]+readname[2] + "/"
                           + readname[0]+readname[1]+readname[2]+readname[3] + "/"
                           + readname[0]+readname[1]+readname[2]+readname[3]+readname[4]+ "/" 
                           + readname;

  m_trace = read_reading((char *)path.c_str(), TT_ANY);
  if (!m_trace) { return; }

  char name[100];
  int version;

  char hex[5];
  hex[4]='\0';
  string curseq;
  FILE * fpos = fopen("dmg.pos", "r");
  char c;
  int i;
  int x;
  while ((i = fscanf ( fpos, "%s\t%d\t", name, &version )) != 0)
  {
    if (readname == name)
    {
      while ( fgets ( hex, 5, fpos ) && hex[0] != '\n' )
      {
        sscanf ( hex, "%04x", &x );
        m_bcpos.push_back(x);
      }

      break;
    }
    else
    {
      do {
        c = fgetc(fpos);
      } while (c != '\n');
    }
  }

  cerr << "loaded " << m_bcpos.size() << "positions" << endl;

  string bases = m_read.getSeqString();
  int rangebegin = m_tile->range.begin;

  if (m_rc)
  {
    rangebegin = bases.length() - m_tile->range.begin;
//    reverse(bases.begin(), bases.end());
    reverse(m_bcpos.begin(), m_bcpos.end());
  }

  // Insert gaps
  Pos_t gapcount;
  vector<Pos_t>::const_iterator g;
  for (g =  m_tile->gaps.begin(), gapcount = 0; 
       g != m_tile->gaps.end(); 
       g++, gapcount++)
  {
//    bases.insert(*g+gapcount+rangebegin, 1, '-');

    int left  = m_bcpos[*g+gapcount+rangebegin-1];
    int right = m_bcpos[*g+gapcount+rangebegin];
    m_bcpos.insert(m_bcpos.begin()+*g+gapcount+rangebegin, 1, (left+right)/2);
  }

  if (m_rc)
  {
//    reverse(bases.begin(), bases.end());
    reverse(m_bcpos.begin(), m_bcpos.end());
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

  return gseqpos;
}
