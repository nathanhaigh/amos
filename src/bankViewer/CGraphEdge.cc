#include "CGraphEdge.hh"

const int CGraphEdge::RTTI = 132547;

using namespace AMOS;

CGraphEdge::CGraphEdge(ContigEdge_t * edge, QCanvas * canvas)
  : QCanvasLine(canvas)
{
  m_edge = edge;
}
     
