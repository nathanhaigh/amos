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
               Distribution_t dist)
{
  m_atile = atile;
  m_acontig = acontig;

  m_btile = btile;
  m_bcontig = bcontig;

  m_state = 'U';
  m_dist = dist;
  

  if (atile && btile)
  {
    m_loffset = omin(atile->offset, btile->offset);

    m_roffset = omax(atile->offset + atile->range.getLength() + atile->gaps.size() -1,
                     btile->offset + btile->range.getLength() + btile->gaps.size() -1);


    m_length = m_roffset - m_loffset;

    if (abs(m_length - dist.mean) <= 3*dist.sd)
    {
      m_state = 'H';
    }
    else
    {
      m_state = 'S';
    }

    int arc = (atile->range.end < atile->range.begin);
    int brc = (btile->range.end < btile->range.begin);

    if (arc + brc != 1)
    {
      cerr << "Orientation violation detected!" << endl;
      m_state = 'O';
    }
  }
  else if (atile)
  {
    m_state = 'L';
    m_loffset = atile->offset;
    m_roffset = atile->offset + atile->range.getLength() + atile->gaps.size() -1;
  }
  else
  {
    m_state = 'L';
    m_loffset = btile->offset;
    m_roffset = btile->offset + btile->range.getLength() + btile->gaps.size() -1;
  }


}
