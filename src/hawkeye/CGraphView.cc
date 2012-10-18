#include "CGraphView.hh"
#include "CGraphContig.hh"
#include <qmatrix.h>
//Added by qt3to4:
#include <QMouseEvent>

using namespace std;

CGraphView::CGraphView(Q3Canvas * canvas, QWidget * parent, const char * name)
  : Q3CanvasView(canvas, parent, name)
{

}

void CGraphView::contentsMousePressEvent(QMouseEvent * e)
{
  QPoint real = inverseWorldMatrix().map(e->pos());
  Q3CanvasItemList l = canvas()->collisions(real);

  for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
  {
    if ((*it)->rtti() == CGraphContig::RTTI)
    {
      CGraphContig * contig = (CGraphContig *) *it;
      cerr << contig->m_id << " selected" << endl;
      emit setContigId(contig->m_id);
    }
    else if ((*it)->rtti() == CGraphEdge::RTTI)
    {
      CGraphEdge * edge = (CGraphEdge *) *it;
      emit edgeSelected(edge->m_edge);
    }
  }
}
