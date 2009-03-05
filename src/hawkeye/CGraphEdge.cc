#include "CGraphEdge.hh"

const int CGraphEdge::RTTI = 132547;

using namespace AMOS;

CGraphEdge::CGraphEdge(ContigEdge_t * edge, Q3Canvas * canvas)
  : Q3CanvasLine(canvas)
{
  m_edge = edge;
}
     
