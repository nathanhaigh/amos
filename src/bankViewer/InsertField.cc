#include "InsertField.hh"
#include <qwmatrix.h>
#include "InsertCanvasItem.hh"

#include <iostream>
using namespace std;

InsertField::InsertField(QCanvas * canvas, QWidget * parent, const char * name)
 : QCanvasView(canvas, parent, name)
{

   QWMatrix m = worldMatrix();
   m.translate(20, 0);
   m.scale( 0.05, 1.0 );
   setWorldMatrix( m );

}

void InsertField::contentsMousePressEvent( QMouseEvent* e )
{
  QPoint real = inverseWorldMatrix().map(e->pos());
  QCanvasItemList l = canvas()->collisions(real);

  if (l.empty())
  {
    emit setStatus(QString("Position is ") + QString::number(real.x()));
    emit setGindex(real.x());
  }
  else
  {
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
    {
      if ((*it)->rtti() == InsertCanvasItem::RTTI)
      {
        InsertCanvasItem * iitem = (InsertCanvasItem *) *it;
        QString s = "Insert ";

        if (iitem->m_insert->m_atile)
        {
          s += "atile: " + QString::number(iitem->m_insert->m_atile->source);
        }

        if (iitem->m_insert->m_btile)
        {
          s += "btile: " + QString::number(iitem->m_insert->m_btile->source);
        }

        emit setStatus(s);
      }
    }
  }
}

void InsertField::viewportPaintEvent(QPaintEvent * e)
{
  QRect rc = QRect(contentsX(),    contentsY(),
                   visibleWidth(), visibleHeight() );
  QRect canvasRect = inverseWorldMatrix().mapRect(rc);

  QCanvasView::viewportPaintEvent(e);

  emit visibleRange(canvasRect.x(), worldMatrix().m11());
}



