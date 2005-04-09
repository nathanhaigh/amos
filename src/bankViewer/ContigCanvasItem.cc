#include "ContigCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>

const int ContigCanvasItem::RTTI = 132556;

ContigCanvasItem::ContigCanvasItem(int x, int y, int width, int height, bool rc,
                                   AMOS::ID_t bid, QCanvas * canvas)
  : QCanvasRectangle(x, y, width, height, canvas), m_contigid(bid)
{
  if (rc)
  {
    setPen(Qt::red);
    setBrush(Qt::red);
  }
  else
  {
    setPen(Qt::blue);
    setBrush(Qt::blue);
  }
}
