#ifndef INSERT_HH_
#define INSERT_HH_ 1

#include "foundation_AMOS.hh"


class Insert
{
public:
  Insert(AMOS::Tile_t * atile, ID_t acontig,
         AMOS::Tile_t * btile, ID_t bcontig,
         AMOS::Distribution_t distribution,
         int conslen);

  void setActive(int i);

  int m_loffset;
  int m_roffset;
  int m_length;
  int m_actual;

  int m_arc;
  int m_brc;

  AMOS::Distribution_t m_dist;

  int m_active;

  enum MateState
  {
    Happy = 'H',
    Unknown = 'U',
    SizeViolation = 'S',
    OrientationViolation = 'O',
    MissingMate = 'M',
    LinkingMate = 'L',
    NoMate = 'N',
  };

  MateState m_state;

  AMOS::Tile_t * m_atile;
  ID_t           m_acontig;

  AMOS::Tile_t * m_btile;
  ID_t           m_bcontig;


  struct TilingOrderCmp
  {
    bool operator() (const Insert & a, const Insert & b)
    {
      int offdiff = b.m_loffset - a.m_loffset;

      if (offdiff)
      {
        if (offdiff < 0) { return false; }
        return true;
      }

      int lendiff = b.m_roffset - a.m_roffset;

      if (lendiff)
      {
        if (lendiff < 0) { return false; }
        return true;
      }

      return true;
    }
  };

};

#endif
