#include "InsertCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>

const int InsertCanvasItem::RTTI = 132545;

InsertCanvasItem::InsertCanvasItem(int x, int y, int width, int height,
                                   Insert * insert, QCanvas * canvas)
  : QCanvasRectangle(x, y, width, height, canvas)
{
  m_insert = insert;
  m_highlight = false;

  m_insert->m_canvasItem = this;
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

  if (m_highlight)
  {
    p.setPen(QColor(255,255,255));
    p.setBrush(Qt::NoBrush);
    p.drawRect((int) x(),     (int) y()-1,
               (int) width(), (int) height()+3);
  }
}

void InsertCanvasItem::drawTile(AMOS::Tile_t * tile, QPainter & p)
{
  if (!tile) { return; }

  int hoffset = x() - m_insert->m_loffset;

  int readLength = tile->range.getLength() + tile->gaps.size();
  p.drawRect(tile->offset+hoffset, (int) y(), readLength, height());
}
