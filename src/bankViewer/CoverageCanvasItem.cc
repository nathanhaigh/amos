#include "CoverageCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>

const int CoverageCanvasItem::RTTI = 132546;
const int CoverageBackgroundCanvasItem::RTTI = 132547;

using namespace std;

CoverageCanvasItem::CoverageCanvasItem(int x, int y, int width, int height,
                                       QPointArray & arr, QCanvas * canvas)
  : QCanvasRectangle(x, y, width, height, canvas),
    m_coveragePlot(arr)
{

}

void CoverageCanvasItem::drawShape (QPainter & p)
{
  p.setPen(QPen(Qt::red, 2));
  p.drawPolyline(m_coveragePlot);
}



CoverageBackgroundCanvasItem::CoverageBackgroundCanvasItem(int x, int y, int width, int height, QCanvas * canvas)
  : QCanvasRectangle(x, y, width, height, canvas)
{

}

void CoverageBackgroundCanvasItem::drawShape (QPainter & p)
{
  QColor bgcolor(60,60,60);
  p.setPen(bgcolor);
  p.setBrush(bgcolor);
  p.drawRect((int)(x()), (int)(y()), (int)(width()), (int)(height()));

  QColor linecolor(Qt::white);
  p.setPen(linecolor);
  p.drawLine((int)(x()), (int)(y()+height()), (int)(x()+width()), (int)(y()+height()));
}
