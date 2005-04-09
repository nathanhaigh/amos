#include "ContigCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>

const int ContigCanvasItem::RTTI = 132556;

ContigCanvasItem::ContigCanvasItem(int x, int y, int width, int height, 
                                   AMOS::Tile_t tile,
                                   QCanvas * canvas)
  : QCanvasRectangle(x, y, width, height, canvas), m_tile(tile)
{
  if (tile.range.isReverse())
  {
    setPen(Qt::yellow);
    setBrush(Qt::yellow);
  }
  else
  {
    setPen(Qt::blue);
    setBrush(Qt::blue);
  }
}
