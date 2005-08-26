#include "ContigSequence.hh"
#include "AMOS_Foundation.hh"

using namespace AMOS;
using namespace std;


ContigSequence::ContigSequence(vector<Tile_t>::iterator & tile, Read_t & read)
 : m_tile(tile),
   m_read(read)
{
  renderSequence();
}

void ContigSequence::renderSequence()
{
  m_nuc = m_read.getSeqString(m_tile->range);
  m_qual = m_read.getQualString(m_tile->range);

  //cerr << "range: " << m_tile->range.begin << " " << m_tile->range.end << endl;

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
  }
}

ContigSequence::~ContigSequence()
{

}

const string & ContigSequence::getSeqname() const
{
  return m_read.getEID();
}

bool ContigSequence::isReverseCompliment() const
{
  return m_tile->range.isReverse();
}

int ContigSequence::getReadID() const
{
  return m_readcount;
//  return m_read.getIID();
}

char & ContigSequence::operator[](long gindex)
{
  long position = gindex - m_tile->offset;

  return m_nuc[position];
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


void ContigSequence::adjustOffset(long gindex, int delta)
{
  if (m_tile->offset >= gindex)
  {
    m_tile->offset += delta;
  }
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

void ContigSequence::erase(long gindex)
{
  long position = gindex - m_tile->offset;

  if (position >= 0 && position < m_nuc.length())
  {
    if (m_nuc[position] != '-')
    {
      cerr << "Error: erasing " << m_nuc[position] << " at " << position 
           << " in r" << getReadID() << endl;

      throw amosException("erasing non-gap element from read!",
                          "ContigSequence::erase");
    }

    m_nuc.erase(m_nuc.begin() + position);
    if (!m_qual.empty()) { m_qual.erase(m_qual.begin() + position); }
  }
  else if (position < 0)
  {
    // A slice was erased to the left of this read
    m_tile->offset--;
  }
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

long ContigSequence::getRightEnd() const
{
  return m_tile->getRightOffset();
}

long ContigSequence::getOffset() const
{
  return m_tile->offset;
}

long ContigSequence::stripRightGaps()
{
  cerr << "TODO: stripRightGaps()" << endl;

  int rightGaps = 0;
  while (m_nuc[m_nuc.length()-1-rightGaps] == '-')
  {
    rightGaps++;
  }

  if (rightGaps)
  {
    cerr << "Stripping " << rightGaps << 
            " rightGaps from r" << getReadID() << endl;

    m_nuc.resize(m_nuc.size()-rightGaps);
    if (!m_qual.empty()) { m_qual.resize(m_qual.size()-rightGaps); }
  }

  return getRightEnd();
}

long ContigSequence::stripLeftGaps()
{
  cerr << "TODO: stripLeftGaps()" << endl;

  int leftGaps = 0;

  while (m_nuc[leftGaps] == '-')
  {
    leftGaps++;
  }

  if (leftGaps)
  {
    cerr << "Stripping " << leftGaps << 
            " leftGaps from r" << getReadID() << endl;

    m_nuc.erase(0, leftGaps);
    if (!m_qual.empty()) { m_qual.erase(m_qual.begin(), m_qual.begin()+leftGaps); }
    
    m_tile->offset += leftGaps;
  }

  return m_tile->offset;
}

int ContigSequence::get3TrimLength()
{
  return m_read.getLength() - m_tile->range.getHi();
}

string ContigSequence::getLeftTrim()
{
  // TODO
  return "TODOLEFTTRIM";

#if 0
  string reversed;

  // We are getting the right trim, but reverse complimenting it
  m_leftTrim = m_readData->getRawNucData(false);
  m_leftTrimQual = m_readData->getRawQualData(false);

  m_leftTrimReversed = "";
  for (int i = m_leftTrim.size()-1; i >= 0; i--)
  {
    char base = m_leftTrim[i];
    m_leftTrimReversed.push_back(libSlice_getCompliment(base));
  }

  return m_leftTrimReversed;
#endif
}

string ContigSequence::getRightTrim()
{
  Range_t r(m_tile->range.getHi(), m_read.getLength());
  return m_read.getSeqString(r);
}

int ContigSequence::extractFirstQuality(string & qualstring, bool doErase)
{
  int qualval = 0;

  string::size_type endpos = qualstring.find(' ');

  if (endpos == qualstring.npos)
  {
    qualval = atoi(qualstring.c_str());
    if (doErase) { qualstring = ""; }
  }
  else
  {
    const string & qualstr = qualstring.substr(0, endpos);
    qualval = atoi(qualstr.c_str());
    if (doErase) { qualstring.erase(0, endpos+1); }
  }

  return qualval;
}

int ContigSequence::extendLeft(const string & extendLeft)
{
  int basesExtended = 0;

  if (!extendLeft.empty())
  {
    //cerr << "Left Extended r" << getReadID()
    //     << ": " << extendLeft << endl;
    
    unsigned int addedBases = extendLeft.size();
    m_tile->offset -= addedBases;

    m_nuc = extendLeft + m_nuc;
    int qualval = m_qual[0];

    for (int i = addedBases-1; i >= 0; i--)
    {
      if (extendLeft[i] != '-')
      {
        basesExtended++;
        qualval = extractFirstQuality(m_leftTrimQual, true);
        m_leftTrim.erase(0, 1);
      }
      else
      {
        if (!m_leftTrimQual.empty())
        {
          int nextqv = extractFirstQuality(m_leftTrimQual, false);
          if (nextqv < qualval) { qualval = nextqv; }
        }
      }

      m_qual.insert(m_qual.begin(), qualval);
    }

    cerr << "TODO extendLeft()" << endl;

#if 0

    m_readData->flushTrimmedSeqSide(false);

    m_readData->addNucData(m_leftTrim.c_str(), false);
    m_readData->addQualData(m_leftTrimQual.c_str(), false);

    long clrleft = m_readData->getClrleft();
    long clrright = m_readData->getClrright();

    m_readData->setClrData(clrleft, clrright + basesExtended);
#endif

    /*
    cerr << "Left Extension of r" << getReadID()
         << " by " << basesExtended 
         << " (" << addedBases << ")" << endl;
    */
  }

  return basesExtended;
}

int ContigSequence::extendRight(const string & extendRight)
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
