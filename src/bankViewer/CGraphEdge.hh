#ifndef CGRAPHEDGE_HH_
#define CGRAPHEDGE_HH_ 1

#include <qcanvas.h>
#include "foundation_AMOS.hh"

class CGraphEdge : public QCanvasLine
{
public:
  CGraphEdge(AMOS::ContigEdge_t * edge, QCanvas * canvas);

  static const int RTTI;
  int rtti () const { return RTTI; }

  AMOS::ContigEdge_t * m_edge;
};

#endif
