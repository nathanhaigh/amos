#include "InsertField.hh"
#include <qwmatrix.h>
#include "InsertCanvasItem.hh"
#include "FeatureCanvasItem.hh"
#include "ContigCanvasItem.hh"

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

void InsertField::highlightInsert(InsertCanvasItem * iitem, 
                                  bool highlight,
                                  bool highlightBuddy)
{
  iitem->m_highlight = highlight;

  Insert * ins = iitem->m_insert;

  QString s = "Insert";

  getInsertString(s, ins->m_active, ins, 0);
  getInsertString(s, !ins->m_active, ins, 1);

  s += QString(" [") + (char)ins->m_state + "] ";

  s += " Actual: "   + QString::number(ins->m_actual);
  s += " Expected: " + QString::number(ins->m_dist.mean - Insert::MAXSTDEV*ins->m_dist.sd) 
    +  " - "         + QString::number(ins->m_dist.mean + Insert::MAXSTDEV*ins->m_dist.sd);

  emit setStatus(s);

  canvas()->setChanged(iitem->boundingRect());

  if (highlightBuddy &&
      ins->m_other && 
      ins->m_other->m_canvasItem)
  {
    ins->m_other->m_canvasItem->m_highlight = iitem->m_highlight;
    canvas()->setChanged(ins->m_other->m_canvasItem->boundingRect());
  }

  canvas()->update();
}

void InsertField::highlightEID(const QString & qeid)
{
  AMOS::ID_t iid = m_datastore->read_bank.lookupIID(qeid.ascii());
  highlightIID(iid);
}

void InsertField::highlightIID(const QString & qiid)
{
  AMOS::ID_t iid = (AMOS::ID_t) (qiid.toUInt());
  highlightIID(iid);
}

void InsertField::highlightIID(AMOS::ID_t iid)
{
  if (iid == AMOS::NULL_ID)
  {
    return;
  }

  QCanvasItemList all = canvas()->allItems();

  for (QCanvasItemList::Iterator it=all.begin(); it!=all.end(); ++it) 
  {
    if ((*it)->rtti() == InsertCanvasItem::RTTI)
    {
      InsertCanvasItem * iitem = (InsertCanvasItem *) *it;
      Insert * ins = iitem->m_insert;

      if (ins->m_aid == iid || ins->m_bid == iid)
      {
        highlightInsert(iitem, true, true);
        break;
      }
    }
  }
}

void InsertField::contentsMousePressEvent( QMouseEvent* e )
{
  QPoint real = inverseWorldMatrix().map(e->pos());
  QCanvasItemList l = canvas()->collisions(real);

  bool found = false;

  for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
  {
    if ((*it)->rtti() == InsertCanvasItem::RTTI)
    {
      InsertCanvasItem * iitem = (InsertCanvasItem *) *it;
      bool highlight = !iitem->m_highlight;
      bool highlightBuddy = e->button() == RightButton;

      highlightInsert(iitem, highlight, highlightBuddy);
      found = true;
    }
    else if ((*it)->rtti() == FeatureCanvasItem::RTTI)
    {
      FeatureCanvasItem * fitem = (FeatureCanvasItem *) * it;
      QString s = "EID: ";
      s += fitem->m_feat.eid + " Group:";
      s += fitem->m_feat.group + " Comment:";
      s += fitem->m_feat.comment + " Type:";
      s += (char)fitem->m_feat.type;
      s += " [" +  QString::number(fitem->m_feat.range.begin) + ",";
      s += QString::number(fitem->m_feat.range.end) + "]";

      emit setStatus(s);
      found = true;
    }
    else if ((*it)->rtti() == ContigCanvasItem::RTTI)
    {
      ContigCanvasItem * citem = (ContigCanvasItem *) * it;

      QString s = "Contig IID: " + QString::number(citem->m_tile.source);
      s += " [" + QString::number(citem->m_tile.offset) +
           ","  + QString::number(citem->m_tile.offset + citem->m_tile.range.getLength()) +
           "]";

      emit setStatus(s);

      found = true;
    }
  }

  if (!found)
  {
    emit setGindex(16*real.x()-m_hoffset);
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



void InsertField::getInsertString(QString & s, int selectb, Insert * ins, int second)
{
  AMOS::ID_t iid = ins->m_aid;
  AMOS::ID_t contigiid = ins->m_acontig;

  if (selectb)
  {
    iid = ins->m_bid;
    contigiid = ins->m_bcontig;
  }

  if (iid != AMOS::NULL_ID)
  {
    if (second)
    {
      s += " <=>";
    }

    s += " ";
    if (ins->m_active == selectb)
    {
      s += "*";
    }

    s += "[";
    s += "e:" + QString(m_datastore->read_bank.lookupEID(iid));
    s += " i:" + QString::number(iid);
    s += " c:" + QString::number(contigiid);
    s += "]";
  }
}
