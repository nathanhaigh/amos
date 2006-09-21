#ifndef INSERT_HH_
#define INSERT_HH_ 1

#include "foundation_AMOS.hh"

class InsertCanvasItem;

class Insert
{
public:
  Insert(AMOS::ID_t aid, AMOS::ID_t acontig, AMOS::Tile_t * atile, 
         AMOS::ID_t bid, AMOS::ID_t bcontig, AMOS::Tile_t * btile, 
         AMOS::ID_t libid, AMOS::Distribution_t distribution,
         AMOS::FragmentType_t matetype);

  void setActive(int i, Insert * other, bool includeLibrary);
  int getProjectedPosition(AMOS::Tile_t * tile, AMOS::Distribution_t dist);
  void printInsert() const;
  bool reasonablyConnected() const;
  bool ceConnected() const;

  int m_loffset;
  int m_roffset;
  int m_length;
  int m_actual;

  static float MAXSTDEV;

  int m_arc;
  int m_brc;

  AMOS::FragmentType_t m_matetype;

  Insert * m_other;
  InsertCanvasItem * m_canvasItem;

  AMOS::Distribution_t m_dist;
  AMOS::ID_t m_libid;

  int m_active;

  enum MateState
  {
    Happy = 'H',
    ExpandedMate = 'E',
    CompressedMate = 'C',
    OrientationViolation = 'O',
    LinkingMate = 'L',
    SingletonMate = 'S',
    NoMate = 'N',
    Unknown = 'U',
  };

  MateState m_state;

  AMOS::Tile_t * m_atile;
  AMOS::ID_t     m_acontig;
  AMOS::ID_t     m_aid;

  AMOS::Tile_t * m_btile;
  AMOS::ID_t     m_bcontig;
  AMOS::ID_t     m_bid;


  struct TilingOrderCmp
  {
    bool operator() (Insert * const & a, Insert * const & b)
    {
      int offdiff = b->m_loffset - a->m_loffset;

      if (offdiff)
      {
        if (offdiff < 0) { return false; }
        return true;
      }

      int lendiff = b->m_roffset - a->m_roffset;

      if (lendiff)
      {
        if (lendiff < 0) { return false; }
        return true;
      }

      // stl requires a strict weak ordering. Return false if tie.
      return false;
    }
  };


  static const char * getInsertTypeStr(MateState state);
  static const char * allstates;

};

#endif
