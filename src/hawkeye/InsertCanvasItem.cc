#include "InsertCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>

#include "Insert.hh"

const int InsertCanvasItem::RTTI = 132545;

using namespace std;

InsertCanvasItem::InsertCanvasItem(int x, int y, int width, int height,
                                   Insert * insert, Q3Canvas * canvas)
  : Q3CanvasRectangle(x, y, width, height, canvas),
  m_insert(insert), m_highlight(false), m_contigcolor(false),
  m_alinked(0), m_alinkedread(0), m_blinked(0), m_blinkedread(0)
{
  m_insert->m_canvasItem = this;
}

void InsertCanvasItem::drawShape (QPainter & p)
{
  //cerr << "ICI::drawShape" << endl;
  p.drawLine((int)x(),                   (int) (y()) + (int)height()-1,
             (int)x() + (int) width()-1, (int) (y()) + (int)height()-1);

  if (m_insert->m_active == 2)
  {
    drawTile(m_insert->m_atile, p, 'A');
    drawTile(m_insert->m_btile, p, 'B');
  }
  else if (m_insert->m_active == 0)
  {
    drawTile(m_insert->m_atile, p, 'A');
  }
  else
  {
    drawTile(m_insert->m_btile, p, 'B');
  }

  if (m_highlight)
  {
    p.setPen(QColor(255,255,255));
    p.setBrush(Qt::NoBrush);
    p.drawRect((int) x(),     (int) y()-1,
               (int) width(), (int) height()+3);
  }
}

void InsertCanvasItem::drawTile(AMOS::Tile_t * tile, QPainter & p, char type)
{
  if (!tile) { return; }

  float hscale = .06250;
  int hoffset = (int)((1/hscale)*x() - m_insert->m_loffset);

  int readLength = tile->getGappedLength();

  if (m_contigcolor)
  {
    p.save();

    if (type == 'A')
    {
      p.setPen(m_acolor);
      p.setBrush(m_acolor);
    }
    else
    {
      p.setPen(m_bcolor);
      p.setBrush(m_bcolor);
    }
  }

  p.drawRect((int)(hscale*(tile->offset+hoffset)), (int) y(), 
             (int)(hscale*(tile->offset+hoffset+readLength-1))-(int)(hscale*(tile->offset+hoffset)), (int)height());

  if (m_contigcolor)
  {
    p.restore();
  }

  if (tile->offset+hoffset < x())
  {
    cerr << type << " " << tile->source << " tile->offset: " << tile->offset << " hoffset: " << hoffset << " x:" << x() << endl;
  }
}
