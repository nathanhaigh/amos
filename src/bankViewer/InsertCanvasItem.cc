#include "InsertCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>

const int InsertCanvasItem::RTTI = 132545;

InsertCanvasItem::InsertCanvasItem(int x, int y, int width, int height,
                                   Insert * insert, QCanvas * canvas)
  : QCanvasRectangle(x, y, width, height, canvas)
{
  m_insert = insert;
}

void InsertCanvasItem::drawShape (QPainter & p)
{
  p.setPen(UIElements::getInsertColor(m_insert->m_state));
  p.setBrush(UIElements::getInsertColor(m_insert->m_state));

  if (m_insert->m_state == Insert::Happy)
  {
    drawTile(m_insert->m_atile, p);
    drawTile(m_insert->m_btile, p);
  }
  else if (m_insert->m_active == 0)
  {
    drawTile(m_insert->m_atile, p);
  }
  else
  {
    drawTile(m_insert->m_btile, p);
  }

  p.drawLine((int) x(),            (int) (y() + height()),
             (int)(x() + width()), (int) (y() + height()));

#if 0

      //cerr << endl;
      QString s = "Actual: " + QString::number(actuallength) +
                  "\nExpected: " + QString::number(ii->m_dist.mean-3*ii->m_dist.sd) +
                  " - "          + QString::number(ii->m_dist.mean+3*ii->m_dist.sd);
      QToolTip::add(this, QRect(inserthpos, vpos,
                                insertlength, m_seqheight), s);

#endif                               
}

void InsertCanvasItem::drawTile(AMOS::Tile_t * tile, QPainter & p)
{
  if (!tile) { return; }

  int readLength = tile->range.getLength() + tile->gaps.size();
  p.drawRect(tile->offset, (int) y(), readLength, height());
}

#if 0


  const char * seqname = read_bank.lookupEID(tile->source);
  QString tip = seqname;

  tip += " [" + QString::number(tile->offset) 
       +  "," + QString::number(tile->offset + tile->range.getLength() + tile->gaps.size() -1)
       + "]";

  if (tile->range.end < tile->range.begin)
  {
    tip += " [RC]";
  }

 // QToolTip::add(this, QRect(hpos, vpos, 
 //               readLength, m_seqheight), tip);
}

#endif
