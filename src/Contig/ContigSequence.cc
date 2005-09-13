#include "ContigSequence.hh"
#include "AMOS_Foundation.hh"

using namespace AMOS;
using namespace std;


ContigSequence::ContigSequence(vector<Tile_t>::iterator & tile, Read_t & read, int readcount)
 : m_tile(tile),
   m_read(read),
   m_readcount(readcount)
{
  renderSequence();
}

void ContigSequence::renderSequence()
{
#ifdef DEBUGRENDER
  cerr << "range: [" << m_tile->range.begin << " " << m_tile->range.end;
#endif

  m_nuc = m_read.getSeqString(m_tile->range);
  m_qual = m_read.getQualString(m_tile->range);

#ifdef DEBUGRENDER
  cerr << ") gaps:";
#endif


  for (int i = 0; i < m_tile->gaps.size(); i++)
  {
    int gappos = m_tile->gaps[i] + i;

    m_nuc.insert(gappos, 1,  '-');

    char lqv = (gappos > 0) ? m_qual[gappos-1] : -1;
    char rqv = (gappos < m_qual.size()) ? m_qual[gappos] : -1;

    char gapqv = (lqv < rqv)
                 ? (lqv != -1) ? lqv : rqv
                 : (rqv != -1) ? rqv : lqv;

    m_qual.insert(gappos, 1, gapqv);

#ifdef DEBUGRENDER
    cerr << " " << gappos;
#endif
  }

#ifdef DEBUGRENDER
  cerr << "." << endl;
#endif
}

ContigSequence::~ContigSequence()
{

}

char ContigSequence::base(long gindex)
{
  if (gindex < m_tile->offset || gindex > m_tile->getRightOffset())
  {
    cerr << "Invalid position requested: "<< gindex 
         << " in r" << getReadID() << endl;
    throw amosException("Invalid position requested", "ContigSequence::base");
  }

  long position = gindex - m_tile->offset;
  return m_nuc[position];
}

int ContigSequence::qv(long gindex) const
{
  if (gindex < m_tile->offset || gindex > m_tile->getRightOffset())
  {
    cerr << "Invalid position requested: "<< gindex 
         << " in r" << getReadID() << endl;
    throw amosException("Invalid position requested", "ContigSequence::qv");
  }

  long position = gindex - m_tile->offset;
  return m_qual[position] - MIN_QUALITY;
}

float ContigSequence::getAvgQV3Trim(int count) const
{
  int i = 0;
  int sum = 0;
  Range_t r(m_tile->range.getHi(), m_read.getLength());
  string trimquals(m_read.getQualString(r));

  for (i = 0; i < trimquals.size() && i < count; i++)
  {
    sum += trimquals[i] - MIN_QUALITY;
  }

  return (i) ? ((float)sum)/i : 0.0;
}

float ContigSequence::getAvgQV(long offset, int count) const
{
  int qvsum = 0;
  int i;

  if (isReverseCompliment())
  {
    for (i = 0; i < count && offset+i <= m_tile->getRightOffset(); i++) 
      { qvsum += qv(offset+i); }
  }
  else
  {
    for (i = 0; i < count && offset-i > m_tile->offset; i++) 
      { qvsum += qv(offset-i); }
  }

  return (i) ? ((float) qvsum) / i : 0.0;
}


void ContigSequence::swap(long a, long b)
{
  char baseswap = m_nuc[a];
  m_nuc[a] = m_nuc[b];
  m_nuc[b] = baseswap;

  if (!m_qual.empty())
  {
    char qualswap = m_qual[a];
    m_qual[a] = m_qual[b];
    m_qual[b] = qualswap;
  }
}

int ContigSequence::leftTwiddle(long gindex, int & twiddleDistance, char cons)
{
  int retval = 0;
  long position = gindex - m_tile->offset;
  if (position > 0 && position < m_nuc.length())
  {
    if (twiddleDistance == 0)
    {
      char base;
      twiddleDistance = 1;
      while (position-twiddleDistance >= 0 &&
             ((base = m_nuc[position-twiddleDistance]) == '-'))
      {
        twiddleDistance++;
      }

      if (position-twiddleDistance >= 0)
      {
        if (cons != '?' && base != cons)
        {
          twiddleDistance = 0;
        }
      }
    }

    if (twiddleDistance && position - twiddleDistance >= 0)
    {
      swap(position-twiddleDistance, position);
      retval = 1;
    }
  }

  return retval;
}

int ContigSequence::rightTwiddle(long gindex, int & twiddleDistance, char cons)
{
  int retval = 0;
  long position = gindex - m_tile->offset;

  if ((position >= 0) && (position+1 < m_nuc.length()))
  {
    if (twiddleDistance == 0)
    {
      char base;
      twiddleDistance = 1;
      while (position+twiddleDistance < m_nuc.length() &&
             ((base = m_nuc[position+twiddleDistance]) == '-'))
      {
        twiddleDistance++;
      }

      if (position+twiddleDistance < m_nuc.length())
      {
        if (cons != '?' && base != cons)
        {
          twiddleDistance = 0;
        }
      }
    }

    if (twiddleDistance && position + twiddleDistance < m_nuc.length())
    {
      swap(position, position+twiddleDistance);
      retval = 1;
    }
  }

  return retval;
}


void ContigSequence::dumpRange(long gindexstart, long gindexend)
{
  if (!(m_tile->offset > gindexend || m_tile->getRightOffset() < gindexstart))
  {
    cerr << "r[" << getReadID() << "]:\t";

    for (int i = gindexstart; i < m_tile->offset; i++)
    {
      cerr << '*';
    }

    long startpos = gindexstart - m_tile->offset;
    long endpos = gindexend - m_tile->offset;

    if (startpos < 0)             { startpos = 0; }
    if (endpos >= m_nuc.length()) { endpos = m_nuc.length()-1; }

    cerr << m_nuc.substr(startpos, endpos-startpos+1) << endl;
  }
}

void ContigSequence::dump()
{
  cerr << "r[" << getReadID() << "]:\t";
  for (int i = 0; i < m_tile->offset; i++)
  {
    cerr << '*';
  }

  cerr << m_nuc << endl;
}

int ContigSequence::get3TrimLength()
{
  return m_read.getLength() - m_tile->range.getHi();
}

string ContigSequence::get3Trim()
{
  Range_t r(m_tile->range.getHi(), m_read.getLength());

  if (m_tile->range.isReverse())
  {
    r.swap();
  }

  return m_read.getSeqString(r);
}

int ContigSequence::extend3Left(const string & extendLeft)
{
  int basesExtended = 0;

  if (!extendLeft.empty())
  {
    //cerr << "Left Extended r" << getReadID()
    //     << ": " << extendLeft << endl;
    
    unsigned int addedBases = extendLeft.size();
    m_tile->offset -= addedBases;

    vector<Pos_t> newgaps;
    int gapcount = 0;

    for (int i = 0; i < addedBases; i++)
    {
      if (extendLeft[i] != '-')
      {
        basesExtended++;
      }
      else
      {
        int gseqpos = i;
        int seqpos = gseqpos - gapcount;

        newgaps.push_back(seqpos);
        gapcount++;
      }
    }

    for (int g = 0; g < m_tile->gaps.size(); g++)
    {
      newgaps.push_back(m_tile->gaps[g] + basesExtended);
    }

    m_tile->gaps = newgaps;

    m_tile->range.begin += basesExtended;

    /*
    cerr << "Left Extension of r" << getReadID()
         << " by " << basesExtended 
         << " (" << addedBases << ")" << endl;
    */

    renderSequence();
  }

  return basesExtended;
}

int ContigSequence::extend3Right(const string & extendRight)
{
  int basesExtended = 0;

  if (!extendRight.empty())
  {
    //cerr << "extendRight sequence: " << extendRight << " in r" << getReadID()
    
    unsigned int addedBases = extendRight.length();
    int glen = m_tile->getGappedLength();
    int gapcount = m_tile->gaps.size();

    //cerr << "new gaps:";

    for (unsigned int i = 0; i < addedBases; i++)
    {
      if (extendRight[i] != '-')
      {
        basesExtended++;
      }
      else
      {
        int gseqpos = glen + i;
        int seqpos = gseqpos - gapcount;

        //cerr << " " << seqpos;

        m_tile->gaps.push_back(seqpos);
        gapcount++;
      }
    }

    //cerr << endl;

    m_tile->range.end += basesExtended;


    //cerr << "Right Extension of r" << getReadID()
    //     << " by " << basesExtended 
    //     << " (" << addedBases << ")" << endl;

    renderSequence();
  }

  return basesExtended;
}
