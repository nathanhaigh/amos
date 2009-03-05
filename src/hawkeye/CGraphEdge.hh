#ifndef CGRAPHEDGE_HH_
#define CGRAPHEDGE_HH_ 1

#include <q3canvas.h>
#include "foundation_AMOS.hh"

class CGraphEdge : public Q3CanvasLine
{
public:
  CGraphEdge(AMOS::ContigEdge_t * edge, Q3Canvas * canvas);

  static const int RTTI;
  int rtti () const { return RTTI; }

  AMOS::ContigEdge_t * m_edge;
};

#endif
