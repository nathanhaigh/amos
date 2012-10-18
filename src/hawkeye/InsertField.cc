#include "InsertField.hh"
#include "UIElements.hh"
#include <qmatrix.h>
#include <qcursor.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qregexp.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPixmap>
#include <QMouseEvent>

#include "InsertCanvasItem.hh"
#include "FeatureCanvasItem.hh"
#include "ContigCanvasItem.hh"
#include "CoverageCanvasItem.hh"
#include "DataStore.hh"
#include "InsertCanvasItem.hh"
#include "RangeScrollBar.hh"

#include "icons/zoom_in.xpm"
#include "icons/zoom_out.xpm"


#include <iostream>
using namespace std;


InsertField::InsertField(DataStore * datastore,
                         int & hoffset,
                         Q3Canvas * canvas, 
                         QWidget * parent, 
                         const char * name)
 : Q3CanvasView(canvas, parent, name),
   m_datastore(datastore),
   m_hoffset(hoffset),
   m_featrect(NULL),
   m_feat(NULL),
   m_lastsearch(NULL),
   m_hrange(NULL),
   m_vrange(NULL)
{
  QMatrix m = worldMatrix();
  m.translate(0, 0);
  m.scale(0.05, 1.0);
  setWorldMatrix(m); // init

  setHScrollBarMode(Q3ScrollView::AlwaysOff);
  setVScrollBarMode(Q3ScrollView::AlwaysOff);

  m_toolstate = 0;
  setCursor(Qt::ArrowCursor);
}

void InsertField::setScrollBars(RangeScrollBar_t * hrange,
                                RangeScrollBar_t * vrange)
{
  m_hrange = hrange;
  m_vrange = vrange;
}

void InsertField::wheelEvent(QWheelEvent * e)
{
  if (m_vrange)
  {
    QApplication::sendEvent(m_vrange, e);
  }
}

void InsertField::viewportPaintEvent(QPaintEvent * e)	 
{	 
  Q3CanvasView::viewportPaintEvent(e);	 
 	 
  QRect rc = QRect(contentsX(),    contentsY(),	 
                   visibleWidth(), visibleHeight() );	 

  QRect real = inverseWorldMatrix().mapRect(rc);	 
 	 
  emit visibleRange(16*real.x()-m_hoffset, worldMatrix().m11()/16);	 

  return;	 
}


void InsertField::setSelectTool()
{
  m_toolstate = 0;
  setCursor(Qt::ArrowCursor);
}

void InsertField::setZoomInTool()
{
  m_toolstate = 1;
  setCursor(QCursor(QPixmap((const char ** )zoom_in)));
}

void InsertField::setZoomOutTool()
{
  m_toolstate = 2;
  setCursor(QCursor(QPixmap((const char ** )zoom_out)));
}


void InsertField::setFeatRect (Q3CanvasItem * item)
{
  int jump = 0;

  if (!m_featrect)
    {
      m_featrect = new Q3CanvasRectangle (0,0,0,0,canvas());
      m_featrect->setBrush(QColor(59,49,31));
      m_featrect->setPen(QColor(139,119,111));
      m_featrect->setZ(-2);
    }

  if ( item == NULL )
    {
      m_featrect->hide();
    }
  else
    {
      int width = item->boundingRect().width();
      canvas()->setChanged(m_featrect->boundingRect());
      m_featrect->setSize(width, canvas()->height());
      m_featrect->move(item->x(), 0);
      m_featrect->show();
      jump = 16*((int)item->x() + width/2) - m_hoffset;
    }
  
  m_feat = item;
  canvas()->setChanged(m_featrect->boundingRect());
  canvas()->update();

  if ( jump )
    emit setGindex(jump);
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

  Q3CanvasItemList all = canvas()->allItems();

  for (Q3CanvasItemList::Iterator it=all.begin(); it!=all.end(); ++it) 
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

void InsertField::processItemSelection(QString & s,
                                       Q3CanvasItemList::Iterator & it, 
                                       int & jumptoread,
                                       bool & jump,
                                       bool rightButtonDown,
                                       bool jumpToBuddy,
                                       int clickpos)
{
  if (((*it)->rtti() == FeatureCanvasItem::RTTI) || 
      ((*it)->rtti() == ContigCanvasItem::RTTI))
  {

    if (!m_featrect)
    {
      m_featrect = new Q3CanvasRectangle((int)(*it)->x(), 0, 
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
    bool highlightBuddy = rightButtonDown;

    if (jumpToBuddy)
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
      + QString(UIElements::getFeatureStr
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

    jump = false;
  }
  else if ((*it)->rtti() == CoverageCanvasItem::RTTI)
  {
    CoverageCanvasItem * citem = (CoverageCanvasItem *) * it;

    int i = 1;

    for (i = 1; i < citem->m_points.size(); i++)
    {
      int xval = citem->m_points[i].x();
      if (xval > clickpos)
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
                          citem->m_points[i].y()) + "</td></tr>";
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



void InsertField::contentsMousePressEvent( QMouseEvent* e )
{
  QPoint real = inverseWorldMatrix().map(e->pos());
  Q3CanvasItemList l = canvas()->collisions(real);

  int gindex = 16*real.x() - m_hoffset;
  int jumptoread = 0;

  bool jump = true;

  int xpos = real.x();
  int ypos = real.y();

  if ((m_toolstate == 1 && e->button() == Qt::LeftButton) || 
      (m_toolstate == 2 && e->button() == Qt::RightButton)) // zoomin
  {
    QMatrix m = worldMatrix();
    QMatrix newzoom(m.m11()*2, m.m12(), m.m21(), m.m22()*1.25, m.dx(), m.dy());
    setWorldMatrix(newzoom); 

    setContentsPos((int)(xpos*newzoom.m11() - visibleWidth()/2), 
                   (int)(ypos*newzoom.m22() - visibleHeight()/2));

    emit updateVisibleRange();
  }
  else if ((m_toolstate == 2 && e->button() == Qt::LeftButton) ||
           (m_toolstate == 1 && e->button() == Qt::RightButton))// zoomout
  {
    QMatrix m = worldMatrix();
    QMatrix newzoom(m.m11()/2, m.m12(), m.m21(), m.m22()/1.25, m.dx(), m.dy());

    if (canvas()->width() * newzoom.m11() < width())
    {
      double xf = ((double) width()) / canvas()->width();
      newzoom.setMatrix(xf, newzoom.m12(), newzoom.m21(), newzoom.m22(), newzoom.dx(), newzoom.dy());
    }

    if (canvas()->height() * newzoom.m22() < height())
    {
      double yf = (double) height() / canvas()->height();
      newzoom.setMatrix(newzoom.m11(), newzoom.m12(), newzoom.m21(), yf, newzoom.dx(), newzoom.dy());
    }

    setWorldMatrix(newzoom);

    setContentsPos((int)(xpos*newzoom.m11() - visibleWidth()/2), 
                   (int)(ypos*newzoom.m22() - visibleHeight()/2));

    emit updateVisibleRange();
  }
  else // center
  {
    QString s;

    for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
    {
      processItemSelection(s, it, jumptoread, jump,  
                           e->button() == Qt::RightButton,
                           e->state() == Qt::ControlButton,
                           real.x());
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
}

void InsertField::canvasCleared()
{
  m_featrect = NULL;
  m_feat = NULL;
  m_lastsearch = NULL;
}


void InsertField::search(const QString & str)
{
  if ( str.isEmpty() )
    {
      setFeatRect (NULL);
      m_lastsearch = NULL;
      return;
    }

  QRegExp exp (str);
  Q3CanvasItemList all = canvas()->allItems();
  Q3CanvasItemList::Iterator it;

  // pick up the search where we left off
  for ( it = all.begin(); it != all.end(); ++ it )
    if ( *it == m_lastsearch )
      {
        ++ it;
        break;
      }

  bool wrap = false;
  for ( ; !wrap || it != all.end(); ++ it ) 
    {
      // wrap around
      if ( it == all.end() )
        { 
          it = all.begin();
          wrap = true;
        }

      if ( (*it)->rtti() == InsertCanvasItem::RTTI )
        {
          InsertCanvasItem * iitem = (InsertCanvasItem *) *it;
          Insert * ins = iitem->m_insert;
          QString aiid (QString::number(ins->m_aid));
          QString biid (QString::number(ins->m_bid));
          QString aeid (m_datastore->read_bank.lookupEID(ins->m_aid).c_str());
          QString beid (m_datastore->read_bank.lookupEID(ins->m_bid).c_str());

          // search insert aiid, biid, aeid, beid
          if ( aiid.contains(exp) || biid.contains(exp) ||
               aeid.contains(exp) || beid.contains(exp) )
            break;
        }
      else if ( (*it)->rtti() == ContigCanvasItem::RTTI )
        {
          ContigCanvasItem * citem = (ContigCanvasItem *) *it;
          AMOS::Tile_t * tile = &(citem->m_tile);
          QString bid (QString::number(m_datastore->contig_bank.
                                       getIDMap().lookupBID(tile->source)));
          QString iid (QString::number(citem->m_tile.source));
          QString eid (m_datastore->contig_bank.lookupEID(tile->source).c_str());

          // search contig bid, iid, eid
          if ( bid.contains(exp) || iid.contains(exp) || eid.contains(exp) )
            break;
        }
      else if ( (*it)->rtti() == FeatureCanvasItem::RTTI )
        {
          FeatureCanvasItem * fitem = (FeatureCanvasItem *) *it;
          QString iid (QString::number(fitem->m_feat.getIID()));
          QString eid (fitem->m_feat.getEID().c_str());
          QString com (fitem->m_feat.getComment().c_str());

          // search feature iid, eid, and comment words
          if ( iid.contains(exp) || eid.contains(exp) || com.contains(exp) )
            break;
        }
    }

  if ( it != all.end() )
    {
      QString s;
      int jumptoread = 0;
      bool jump = false;

      processItemSelection(s, it, jumptoread, jump,  
                           false, false, 0);
      emit setDetails(s);

      setFeatRect (*it);
      m_lastsearch = *it;
    }
  else
    {
      setFeatRect (NULL);
      m_lastsearch = NULL;
    }
}

void InsertField::resizeEvent(QResizeEvent * e)
{
  emit fieldResized();
}
