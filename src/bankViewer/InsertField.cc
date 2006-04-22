#include "InsertField.hh"
#include <qwmatrix.h>
#include "InsertCanvasItem.hh"
#include "FeatureCanvasItem.hh"
#include "ContigCanvasItem.hh"
#include "CoverageCanvasItem.hh"
#include "DataStore.hh"
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
   m_hoffset(hoffset),
   m_featrect(NULL),
   m_feat(NULL)
{
  QWMatrix m = worldMatrix();
  m.translate(20, 0);
  m.scale( 0.05, 1.0 );
  setWorldMatrix( m );

  setHScrollBarMode(QScrollView::AlwaysOff);
  setVScrollBarMode(QScrollView::AlwaysOff);

  m_visibleRect = new QCanvasRectangle(0,0,100,100, canvas);
  m_visibleRect->setPen(Qt::black);
  m_visibleRect->setBrush(Qt::black);
  m_visibleRect->setZ(-1000);
  m_visibleRect->show();
}

void InsertField::highlightInsert(InsertCanvasItem * iitem, 
                                  bool highlight,
                                  bool highlightBuddy)
{
  iitem->m_highlight = highlight;

  Insert * ins = iitem->m_insert;
  AMOS::ID_t iida = ins->m_aid;
  QString eida = m_datastore->read_bank.lookupEID(iida).c_str();

  emit readIIDHighlighted(QString::number(iida));
  emit readEIDHighlighted(eida);

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
  highlightRead(iid);
}

void InsertField::highlightIID(const QString & qiid)
{
  AMOS::ID_t iid = (AMOS::ID_t) (qiid.toUInt());
  highlightRead(iid);
}

void InsertField::highlightRead(int iid)
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

  int gindex = 16*real.x() - m_hoffset;
  int jumptoread = 0;

  bool jump = true;

  QString s;

  for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
  {
    if (((*it)->rtti() == FeatureCanvasItem::RTTI) || 
        ((*it)->rtti() == ContigCanvasItem::RTTI))
    {

      if (!m_featrect)
      {
        m_featrect = new QCanvasRectangle((*it)->x(), 0, 
                                          (*it)->boundingRect().width(), canvas()->height(), 
                                          canvas());
        m_featrect->setBrush(QColor(59,49,31));
        m_featrect->setPen(QColor(139,119,111));
        m_featrect->setZ(-2);
        m_featrect->show();
      }
      else
      {
        if (m_feat == *it)
        {
          if (m_featrect->isVisible()) { m_featrect->hide(); }
          else                         { m_featrect->show(); }
        }
        else
        {
          canvas()->setChanged(m_featrect->boundingRect());
          m_featrect->setSize((*it)->boundingRect().width(), canvas()->height());
          m_featrect->move((*it)->x(), 0);
          m_featrect->show();
        }
      }

      m_feat = *it;
      canvas()->setChanged(m_featrect->boundingRect());
      canvas()->update();

    }

    // insert selected
    if ((*it)->rtti() == InsertCanvasItem::RTTI)
    {
      InsertCanvasItem * iitem = (InsertCanvasItem *) *it;
      bool highlight = !iitem->m_highlight;
      bool highlightBuddy = e->button() == RightButton;

      if (e->state() == Qt::ControlButton)
      {
        if (iitem->m_contigcolor)
        {
          jumptoread = iitem->m_alinkedread;
        }
        else
        {
          jumptoread = iitem->m_insert->m_bid;
        }
      }

      highlightInsert(iitem, highlight, highlightBuddy);
      jump = false;

      // emit details
      Insert * ins = iitem->m_insert;
      AMOS::ID_t iida = ins->m_aid;
      AMOS::ID_t iidb = ins->m_bid;
      AMOS::ID_t ctga = ins->m_acontig;
      AMOS::ID_t ctgb = ins->m_bcontig;
      QString eida = m_datastore->read_bank.lookupEID(iida).c_str();
      QString eidb = m_datastore->read_bank.lookupEID(iidb).c_str();
      
      s += "<b><em>Insert</em></b><br>";
      s += "<table>";
      s += "<tr><td><b>Status</b></td><td>"
        + QString(Insert::getInsertTypeStr(ins->m_state)) + "</td></tr>";
      s += "<tr><td><b>Size</b></td><td>"
        + QString::number(ins->m_actual) + "</td></tr>";
      s += "<tr><td><b>Expected</b></td><td>"
        + QString::number(ins->m_dist.mean - Insert::MAXSTDEV*ins->m_dist.sd)
        + "-"
        + QString::number(ins->m_dist.mean + Insert::MAXSTDEV*ins->m_dist.sd)
        + "</td></tr>";
      s += "<tr><td><b>Range</b></td><td>"
        + QString::number(ins->m_loffset) + ","
        + QString::number(ins->m_roffset) + "</td></tr>";
      s += "</table>";

      s += "<hr>";

      s += "<b><em>Reads</em></b><br>";
      s += "<table>";
      s += "<tr><td><b>EID</b></td><td>"
        + eida + "</td><td>"
        + eidb + "</td></tr>";
      s += "<tr><td><b>IID</b></td><td>"
        + QString::number(iida) + "</td><td>"
        + QString::number(iidb) + "</td></tr>";
      s += "<tr><td><b>Contig</b></td><td>"
        + QString::number(ctga) + "</td><td>"
        + QString::number(ctgb) + "</td></tr>";
      s += "</table>";

      s += "<hr>";
    }
    // feature selected
    else if ((*it)->rtti() == FeatureCanvasItem::RTTI)
    {
      jump = false;

      // emit details
      FeatureCanvasItem * fitem = (FeatureCanvasItem *) * it;

      s += "<b><em>Feature</em></b><br>";
      s += "<table>";
      s += "<tr><td><b>Type</b></td><td>"
        + QString(FeatureCanvasItem::getFeatureTypeStr
                  (fitem->m_feat.getType())) + "</td></tr>";
      s += "<tr><td><b>Size</b></td><td>"
        + QString::number(fitem->m_feat.getRange().getLength()) + "</td></tr>";
      s += "<tr><td><b>Range</b></td><td>"
        + QString::number(fitem->m_feat.getRange().begin) + ","
        + QString::number(fitem->m_feat.getRange().end) + "</td></tr>";
      s += "<tr><td><b>Comment</b></td><td>"
        + QString(fitem->m_feat.getComment().c_str()) + "</td></tr>";
      s += "<tr><td><b>EID</b></td><td>"
        + QString(fitem->m_feat.getEID().c_str()) + "</td></tr>";
      s += "<tr><td><b>IID</b></td><td>"
        + QString::number(fitem->m_feat.getIID()) + "</td></tr>";
      s += "</table>";

      s += "<hr>";
    }
    // contig selected
    else if ((*it)->rtti() == ContigCanvasItem::RTTI)
    {
      // emit details
      ContigCanvasItem * citem = (ContigCanvasItem *) * it;
      long int len = citem->m_tile.range.getLength();

      s += "<b><em>Contig</em></b><br>";
      s += "<table>";
      s += "<tr><td><b>ID</b></td><td>"
        + QString::number(m_datastore->contig_bank.getIDMap().lookupBID
                          (citem->m_tile.source)) + "</td></tr>";
      s += "<tr><td><b>Size</b></td><td>"
        + QString::number(len) + "</td></tr>";
      s += "<tr><td><b>Range</b></td><td>"
        + QString::number(citem->m_tile.offset) + ","
        + QString::number(citem->m_tile.offset + len) + "</td></tr>";
      s += "<tr><td><b>EID</b></td><td>"
        + QString(m_datastore->contig_bank.lookupEID
                  (citem->m_tile.source).c_str()) + "</td></tr>";
      s += "<tr><td><b>IID</b></td><td>"
        + QString::number(citem->m_tile.source) + "</td></tr>";
      s += "</table>";

      s += "<hr>";
    }
    else if ((*it)->rtti() == CoverageCanvasItem::RTTI)
    {
      CoverageCanvasItem * citem = (CoverageCanvasItem *) * it;

      int i = 1;
      int click = real.x();

      for (i = 1; i < citem->m_points.size(); i++)
      {
        int xval = citem->m_points[i].x();
        if (xval > click)
        {
          break;
        }
      }

      i--;

      if (0)
      {
        int xval = citem->m_points[i].x();
        xval = 16*xval - m_hoffset;
        cerr << "i: " << i << " xval: " << xval << endl;
      }

      if (citem->m_libid < 0)
      {
        s += "<b><em>";
        
        if (citem->m_libid == -1) { s += "Clone"; }
        else if (citem->m_libid == -2) { s += "Read";  }
        else if (citem->m_libid == -3) { s += "Kmer";  }
        
        s += " Coverage</em></b>";
        s += "<table>";
        s += "<tr><td><b>Value</b></td><td>"
          + QString::number(citem->y() + citem->height() -
                            citem->m_points[i].y()); + "</td></tr>";
        s += "<tr><td><b>Average</b></td><td>"
          + QString::number(citem->m_baseLevel, 'f', 2) + "</td></tr>";
        s += "</table>";

        s += "<hr>";
      }
      else
      {
        s += "<b><em>CE Stat</em></b>";
        s += "<table>";
        s += "<tr><td><b>Library</b></td><td>"
          + QString::number(citem->m_libid) + "</td></tr>";
        s += "<tr><td><b>Value</b></td><td>"
          + QString::number(citem->m_raw[i], 'f', 3) + "</td></tr>";
        s += "</table>";

        s += "<hr>";
      }

      jump = true;
    }
  }

  s += "<b><em>Click</em></b><br>";
  s += "<table>";
  s += "<tr><td><b>Position</b></td><td>"
    + QString::number(gindex) + "</td></tr>";
  s += "</table>";

  emit setDetails(s);

  if (jump)
    emit setGindex(gindex);
  
  if (jumptoread)
    emit jumpToRead(jumptoread);
}

void InsertField::viewportPaintEvent(QPaintEvent * e)
{
  QRect rc = QRect(contentsX(),    contentsY(),
                   visibleWidth(), visibleHeight() );
  QRect real = inverseWorldMatrix().mapRect(rc);

#if 0
  if (m_visibleRect)
  {
    m_visibleRect->setPen(Qt::black);
    m_visibleRect->setBrush(Qt::black);
    m_visibleRect->setSize(real.width(), real.height());
    m_visibleRect->move(real.x(), real.y());
    canvas()->setChanged(m_visibleRect->boundingRect());
    canvas()->update();
  }
  else
  {
    cerr << "WTF???" << endl;
  }
#endif

  QCanvasView::viewportPaintEvent(e);

  emit visibleRange(16*real.x()-m_hoffset, worldMatrix().m11()/16);
}

void InsertField::canvasCleared()
{
  m_featrect = NULL;
  m_feat = NULL;
}
