#include "InsertField.hh"
#include <qwmatrix.h>
#include "InsertCanvasItem.hh"

#include <iostream>
using namespace std;

InsertField::InsertField(DataStore * datastore,
                         int & hoffset,
                         QCanvas * canvas, 
                         QWidget * parent, 
                         const char * name)
 : QCanvasView(canvas, parent, name),
   m_datastore(datastore),
   m_hoffset(hoffset)
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
    emit setGindex(real.x()-m_hoffset);
  }
  else
  {
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
    {
      if ((*it)->rtti() == InsertCanvasItem::RTTI)
      {
        InsertCanvasItem * iitem = (InsertCanvasItem *) *it;
        Insert * ins = iitem->m_insert;

        QString s = "Insert " + QString::number(ins->m_active);

        getInsertString(s, ins->m_active, ins);
        getInsertString(s, !ins->m_active, ins);

        s += " Actual: "   + QString::number(ins->m_actual);
        s += " Expected: " + QString::number(ins->m_dist.mean - 3*ins->m_dist.sd) 
          +  " - "         + QString::number(ins->m_dist.mean + 3*ins->m_dist.sd);

        emit setStatus(s);

        iitem->m_highlight = !iitem->m_highlight;
        canvas()->setChanged(iitem->boundingRect());

        if (e->button() == RightButton && 
            ins->m_other && 
            ins->m_other->m_canvasItem)
        {
          ins->m_other->m_canvasItem->m_highlight = iitem->m_highlight;
          canvas()->setChanged(ins->m_other->m_canvasItem->boundingRect());
        }

        canvas()->update();
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

  emit visibleRange(canvasRect.x()-m_hoffset, worldMatrix().m11());
}



void InsertField::getInsertString(QString & s, int which, Insert * ins)
{
  if (which == 0)
  {
    if (ins->m_aid != AMOS::NULL_ID)
    {
      s += " a: ";
      s += m_datastore->read_bank.lookupEID(ins->m_aid);
      s += " {" + QString::number(ins->m_aid)     + "}";
      s += " [" + QString::number(ins->m_acontig) + "]";
    }
  }
  else
  {
    if (ins->m_bid != AMOS::NULL_ID)
    {
      s += " b: ";
      s += m_datastore->read_bank.lookupEID(ins->m_bid);
      s += " {" + QString::number(ins->m_bid)     + "}";
      s += " [" + QString::number(ins->m_bcontig) + "]";
    }
  }
}



