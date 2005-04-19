#include "CoverageCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>

const int CoverageCanvasItem::RTTI = 132546;

using namespace std;

CoverageCanvasItem::CoverageCanvasItem(int x, int y, int width, int height, bool isClone,
                                       QPointArray & arr, QCanvas * canvas)
  : QCanvasRectangle(x, y, width, height, canvas),
    m_coveragePlot(arr),
    m_isClone(isClone)
{
  if (isClone)
  {
    setPen(QPen(Qt::magenta, 1));
  }
  else
  {
    setPen(QPen(Qt::green, 1));
  }

  show();
}

void CoverageCanvasItem::drawShape (QPainter & p)
{
  p.drawPolyline(m_coveragePlot);
}
