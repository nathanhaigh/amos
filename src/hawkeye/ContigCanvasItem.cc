#include "ContigCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>

const int ContigCanvasItem::RTTI = 132556;

ContigCanvasItem::ContigCanvasItem(int x, int y, int width, int height, 
                                   AMOS::Tile_t tile,
                                   Q3Canvas * canvas)
  : Q3CanvasRectangle(x, y, width, height, canvas), m_tile(tile)
{
  if (tile.range.isReverse())
  {
    setPen(UIElements::color_RContig);
    setBrush(UIElements::color_RContig);
  }
  else
  {
    setPen(UIElements::color_FContig);
    setBrush(UIElements::color_FContig);
  }
}
