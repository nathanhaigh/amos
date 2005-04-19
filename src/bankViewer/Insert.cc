#include "Insert.hh"

using namespace AMOS;
using namespace std;

const int MAXTRANSPOSONDIST = 100;
const int MIN3PRIMETRIM = 250;
const int REASONABLY_CONNECTED_SD = 10;
const int READLEN = 500;

float Insert::MAXSTDEV = 2;
const char * Insert::allstates = "HSCOMLNU";

static int min (int a, int b)
{
  return (a < b) ? a : b;
}

static int max (int a, int b)
{
  return (a > b) ? a : b;
}

Insert::Insert(ID_t     aid,
               ID_t     acontig,
               Tile_t * atile, 
               ID_t     bid,
               ID_t     bcontig,
               Tile_t * btile,
               ID_t     libid,
               Distribution_t dist,
               int conslen,
               AMOS::MateType_t matetype)
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
    arange.setEnd(atile->offset + atile->range.getLength() + atile->gaps.size()-1);
  }

  if (btile) 
  { 
    m_brc = btile->range.isReverse(); 
    brange.setBegin(btile->offset);
    brange.setEnd(btile->offset + btile->range.getLength() + btile->gaps.size()-1);
  }

  if (atile && btile)
  {
    m_loffset = min(arange.begin, brange.begin);
    m_roffset = max(arange.end, brange.end);

    m_length = m_roffset - m_loffset + 1;

    m_state = Happy;
    m_active = 2;

    if (m_matetype == Matepair_t::TRANSPOSON)
    {
      if (m_arc) { m_actual = brange.begin - arange.end; }
      else       { m_actual = arange.begin - brange.end; }

      if (m_actual > MAXTRANSPOSONDIST)
      {
        m_state = StretchedMate;
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
        m_state = StretchedMate;
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
    else
    {
      int projected = getProjectedPosition(atile, m_dist);

      if (m_arc)
      { if (projected > 0)       { m_state = MissingMate; } }
      else
      { if (projected < conslen) { m_state = MissingMate; } }
    }
  }
  else if (btile)
  {
    m_state = LinkingMate;
    m_loffset = brange.begin;
    m_roffset = brange.end;

    int projected = getProjectedPosition(btile, m_dist);

    if (m_brc)
    { if (projected > 0)       { m_state = MissingMate; } }
    else
    { if (projected < conslen) { m_state = MissingMate; } }
  }
  else
  {
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
    return (int)(tile->offset + tile->range.getLength() + tile->gaps.size() - 1 - dist.mean - MAXSTDEV*dist.sd - READLEN);
  }
  else
  {
    return (int)(tile->offset + dist.mean + MAXSTDEV*dist.sd + READLEN);
  }
}


bool Insert::reasonablyConnected() const
{
  if (!m_atile || !m_btile) { return false; }

  if (m_matetype == Matepair_t::TRANSPOSON)
  {
    return true;
  }

  return (m_state == Happy) ||
         (m_state == CompressedMate) ||
         (m_state == StretchedMate && (m_actual <= m_dist.mean + REASONABLY_CONNECTED_SD * m_dist.sd));


  /*  //This would force reads that overlap to be disconnected 
         (m_state == CompressedMate && (m_actual > (m_atile->range.getLength() + m_atile->gaps.size() +
                                                    m_btile->range.getLength() + m_btile->gaps.size())));
  */
}

void Insert::setActive(int i, Insert * other, bool includeLibrary)
{
  Tile_t * tile;
  m_other = other;

  m_active = i;

  if (i == 0) { tile = m_atile; }
  else        { tile = m_btile; }

  int len = tile->range.getLength() + tile->gaps.size()-1;

  if (includeLibrary)
  {
    if (tile->range.isReverse())
    {
      m_roffset = tile->offset + len;

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
        m_roffset = m_loffset + len + MIN3PRIMETRIM;
      }
    }
  }
  else
  {
    m_loffset = tile->offset;
    m_roffset = tile->offset + len;
  }

  m_length = (m_roffset - m_loffset);
}

static const char * happystr       = "Happy";
static const char * stretchedstr   = "Stretched Mate";
static const char * compressedstr  = "Compressed Mate";
static const char * orientationstr = "Orientation Violation";
static const char * missingstr     = "Missing Mate";
static const char * linkingstr     = "Linking Mate";
static const char * unmatedstr     = "No Mate";
static const char * unknownstr     = "Unknown";

const char * Insert::getInsertTypeStr(MateState state)
{
  switch (state)
  {
    case Happy:                return happystr;
    case StretchedMate:        return stretchedstr;
    case CompressedMate:       return compressedstr;
    case OrientationViolation: return orientationstr;
    case MissingMate:          return missingstr;
    case LinkingMate:          return linkingstr;
    case NoMate:               return unmatedstr;
    default:                   return unknownstr;
  }
}

