#include "CGraphView.hh"
#include "CGraphContig.hh"
#include <qwmatrix.h>

CGraphView::CGraphView(QCanvas * canvas, QWidget * parent, const char * name)
  : QCanvasView(canvas, parent, name)
{

}

void CGraphView::contentsMousePressEvent(QMouseEvent * e)
{
  QPoint real = inverseWorldMatrix().map(e->pos());
  QCanvasItemList l = canvas()->collisions(real);

  for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
  {
    if ((*it)->rtti() == CGraphContig::RTTI)
    {
      CGraphContig * contig = (CGraphContig *) *it;
      cerr << contig->m_id << " selected" << endl;
      emit setContigId(contig->m_id);
    }
  }
}
