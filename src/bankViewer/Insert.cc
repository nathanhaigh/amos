#include "Insert.hh"

using namespace AMOS;

int omin (int a, int b)
{
  return (a < b) ? a : b;
}

int omax (int a, int b)
{
  return (a > b) ? a : b;
}

Insert::Insert(Tile_t * atile, 
               ID_t     acontig,
               Tile_t * btile,
               ID_t     bcontig,
               Distribution_t dist,
               int conslen)
{
  m_atile = atile;
  m_acontig = acontig;

  m_btile = btile;
  m_bcontig = bcontig;

  m_state = Unknown;
  m_dist = dist;
  m_active = -1;

  m_length = m_dist.mean;

  int READLEN = 500;

  m_arc = 0;
  m_brc = 0;
  if (atile) { m_arc = (atile->range.end < atile->range.begin); }
  if (btile) { m_brc = (btile->range.end < btile->range.begin); }
  
  if (atile && btile)
  {
    m_loffset = omin(atile->offset, btile->offset);

    m_roffset = omax(atile->offset + atile->range.getLength() + atile->gaps.size() -1,
                     btile->offset + btile->range.getLength() + btile->gaps.size() -1);

    m_length = m_roffset - m_loffset;

    if (abs(m_length - dist.mean) <= 3*dist.sd)
    {
      m_state = Happy;
      m_active = 2;
    }
    else
    {
      m_state = SizeViolation;
    }

    if (m_arc + m_brc != 1)
    {
      m_state = OrientationViolation;
    }
  }
  else if (atile)
  {
    m_state = LinkingMate;
    m_loffset = atile->offset;
    m_roffset = atile->offset + atile->range.getLength() + atile->gaps.size() -1;

    if (bcontig == AMOS::NULL_ID)
    {
      m_state = NoMate;
      m_length = m_roffset - m_loffset + 1;
      m_active = 0;
    }
    else
    {
      if (atile->range.end < atile->range.begin)
      {
        // rc
        
        int projected = m_roffset - m_dist.mean - 3*m_dist.sd - READLEN;
        
        if (projected > 0)
        {
          m_state = MissingMate;
        }
      }
      else
      {
        //forward
        if (m_loffset + m_dist.mean + 3*m_dist.sd + READLEN < conslen)
        {
          m_state = MissingMate;
        }
      }
    }
  }
  else if (btile)
  {
    m_state = LinkingMate;
    m_loffset = btile->offset;
    m_roffset = btile->offset + btile->range.getLength() + btile->gaps.size() -1;

    if (btile->range.end < btile->range.begin)
    {
      // rc
      
      int projected = m_roffset - m_dist.mean - 3*m_dist.sd - READLEN;
      
      if (projected > 0)
      {
        m_state = MissingMate;
      }
    }
    else
    {
      //forward
      if (m_loffset + m_dist.mean + 3*m_dist.sd + READLEN < conslen)
      {
        m_state = MissingMate;
      }
    }
  }
  else
  {
    throw "No Tile!";
  }

  m_actual = m_roffset - m_loffset + 1;
  m_other = NULL;
  m_canvasItem = NULL;
}

void Insert::setActive(int i, Insert * other)
{
  Tile_t * tile;
  m_other = other;

  m_active = i;

  if (i == 0) { tile = m_atile; }
  else        { tile = m_btile; }

  if (tile->range.end < tile->range.begin)
  {
    //rc
    m_loffset = tile->offset - (m_dist.mean - tile->range.getLength() - tile->gaps.size());
  }
  else
  {
    //forward
    m_loffset = tile->offset;
  }

  m_roffset = tile->offset + m_dist.mean;
  m_length = m_dist.mean;
}
