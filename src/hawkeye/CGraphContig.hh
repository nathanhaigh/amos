#ifndef CGRAPHCONTIG_HH_
#define CGRAPHCONTIG_HH_ 1

#include <q3canvas.h>
#include "foundation_AMOS.hh"

class CGraphContig : public Q3CanvasRectangle
{
public:
  CGraphContig(AMOS::ID_t id, AMOS::ContigEdge_t * edge, bool rc,
               int x, int y, int width, int height,
               Q3Canvas * canvas);

  static const int RTTI;
  int rtti () const { return RTTI; }

  AMOS::ContigEdge_t * m_edge;
  AMOS::ID_t m_id;
  bool m_rc;

protected:
  void drawShape(QPainter & p);
};

#endif
