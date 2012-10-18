#include "Insert.hh"
#include <algorithm>

using namespace AMOS;
using namespace std;

const int MAXTRANSPOSONDIST = 100;
const int MIN3PRIMETRIM = 250;
const int REASONABLY_CONNECTED_SD = 10;
const int CE_CONNECTED_SD = 8;
const int READLEN = 500;

float Insert::MAXSTDEV = 2;
const char * Insert::allstates = "HECOLSNU";

Insert::Insert(ID_t     aid,
               ID_t     acontig,
               Tile_t * atile, 
               ID_t     bid,
               ID_t     bcontig,
               Tile_t * btile,
               ID_t     libid,
               Distribution_t dist,
               AMOS::FragmentType_t matetype)
{
  m_atile   = atile;
  m_aid     = aid;
  m_acontig = acontig;

  m_btile   = btile;
  m_bid     = bid;
  m_bcontig = bcontig;

  m_matetype = matetype;
  m_dist = dist;
  m_libid = libid;

  m_state = Unknown;
  m_active = -1;
  m_actual = -1;

  m_length = m_dist.mean;

  m_arc = 0;
  m_brc = 0;

  // Gapped ranges on the contig
  Range_t arange, brange;

  if (atile) 
  { 
    m_arc = atile->range.isReverse(); 
    arange.setBegin(atile->offset);
    arange.setEnd(atile->offset + atile->getGappedLength() - 1);
  }

  if (btile) 
  { 
    m_brc = btile->range.isReverse(); 
    brange.setBegin(btile->offset);
    brange.setEnd(btile->offset + btile->getGappedLength() - 1);
  }

  if (atile && btile)
  {
    m_loffset = min(arange.begin, brange.begin);
    m_roffset = max(arange.end, brange.end);

    m_length = m_roffset - m_loffset + 1;

    m_state = Happy;
    m_active = 2;

    if (m_matetype == Fragment_t::TRANSPOSON)
    {
      if (m_arc) { m_actual = brange.begin - arange.end; }
      else       { m_actual = arange.begin - brange.end; }

      if (m_actual > MAXTRANSPOSONDIST)
      {
        m_state = ExpandedMate;
      }

      if ((m_arc + m_brc != 1) ||
          (m_arc && arange.end > brange.end) ||
          (m_arc && brange.begin < arange.begin) ||
          (m_brc && brange.end > arange.end) ||
          (m_brc && arange.begin < brange.begin))
      {
        m_state = OrientationViolation;
      }
    }
    else
    {
      if ((unsigned int)m_length > (unsigned int)(dist.mean + MAXSTDEV*dist.sd))
      {
        m_state = ExpandedMate;
      }
      else if ((int)(m_length + MAXSTDEV*dist.sd) < dist.mean)
      {
        m_state = CompressedMate;
      }

      // Orientation violation if the reads point in the same direction, or
      // there is at least one 3' base not covered:
      //   <--------
      //     -------->
      //    ^       ^

      if ((m_arc + m_brc != 1) ||
          (m_arc && arange.begin < brange.begin) ||
          (m_arc && brange.end > arange.end) ||
          (m_brc && brange.begin < arange.begin) ||
          (m_brc && arange.end > brange.end))
      {
        m_state = OrientationViolation;
      }
    }
  }
  else if (atile)
  {
    m_state = LinkingMate;
    m_loffset = arange.begin;
    m_roffset = arange.end;

    if (bid == NULL_ID)
    {
      m_state = NoMate;
      m_length = m_roffset - m_loffset + 1;
      m_active = 0;
    }
    else if (bcontig == NULL_ID)
    {
      m_state = SingletonMate;
    }
  }
  else
  {
    if (btile)
    {
      throw "btile, but no atile!";
    }

    throw "No Tile!";
  }

  if (m_actual == -1)
  {
    m_actual = m_roffset - m_loffset + 1;
  }
  m_other = NULL;
  m_canvasItem = NULL;
}

void Insert::printInsert() const
{
  cerr << m_aid << "," << m_bid << " l:" << m_loffset << " r:" << m_roffset << " s:" << (char)m_state << endl;
}

int Insert::getProjectedPosition(Tile_t * tile, Distribution_t dist)
{
  if (tile->range.isReverse())
  {
    return (int)(tile->offset + tile->getGappedLength() - 1 - dist.mean - MAXSTDEV*dist.sd - READLEN);
  }
  else
  {
    return (int)(tile->offset + dist.mean + MAXSTDEV*dist.sd + READLEN);
  }
}


bool Insert::reasonablyConnected() const
{
  if (!m_atile || !m_btile) { return false; }

  if (m_matetype == Fragment_t::TRANSPOSON)
  {
    return true;
  }

  return (m_state == Happy) ||
         (m_state == CompressedMate) ||
         (m_state == ExpandedMate && (m_actual <= m_dist.mean + REASONABLY_CONNECTED_SD * m_dist.sd));


  /*  //This would force reads that overlap to be disconnected 
         (m_state == CompressedMate && (m_actual > (m_atile->getGappedLength() + m_btile->getGappedLength())));
  */
}

bool Insert::ceConnected() const
{
 // if (m_acontig != m_bcontig) {return false;}
  if (m_matetype == Fragment_t::TRANSPOSON)
  {
    return false;
  }

  return ((m_state == Happy) || 
          (m_state == CompressedMate) || 
          (m_state == ExpandedMate)) && 
         (abs(m_actual - m_dist.mean) < (CE_CONNECTED_SD * m_dist.sd));
}

void Insert::setActive(int i, Insert * other, bool includeLibrary)
{
  Tile_t * tile;
  m_other = other;

  m_active = i;

  if (i == 0) { tile = m_atile; }
  else        { tile = m_btile; }

  int len = tile->getGappedLength();

  if (includeLibrary)
  {
    if (tile->range.isReverse())
    {
      m_roffset = tile->offset + len - 1;

      if (m_dist.mean > len)
      {
        m_loffset = m_roffset - m_dist.mean;
      }
      else
      {
        m_loffset = tile->offset - MIN3PRIMETRIM;
      }
    }
    else
    {
      m_loffset = tile->offset;

      if (m_dist.mean > len)
      {
        m_roffset = m_loffset + m_dist.mean;
      }
      else
      {
        m_roffset = m_loffset + len + MIN3PRIMETRIM - 1;
      }
    }
  }
  else
  {
    m_loffset = tile->offset;
    m_roffset = tile->offset + len - 1;
  }

  m_length = m_roffset - m_loffset + 1;
}

static const char * happystr       = "Happy";
static const char * compressedstr  = "Compressed Mate";
static const char * expandedstr    = "Stretched Mate";
static const char * orientationstr = "Orientation Violation";
static const char * linkingstr     = "Linking Mate";
static const char * singletonstr   = "Singleton Mate";
static const char * unmatedstr     = "Unmated";
static const char * unknownstr     = "Unknown";

const char * Insert::getInsertTypeStr(MateState state)
{
  switch (state)
  {
    case Happy:                return happystr;
    case ExpandedMate:         return expandedstr;
    case CompressedMate:       return compressedstr;
    case OrientationViolation: return orientationstr;
    case LinkingMate:          return linkingstr;
    case SingletonMate:        return singletonstr;
    case NoMate:               return unmatedstr;
    default:                   return unknownstr;
  }
}

