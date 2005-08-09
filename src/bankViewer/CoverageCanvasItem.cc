#include "CoverageCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>

const int CoverageCanvasItem::RTTI = 132546;

using namespace std;

CoverageCanvasItem::CoverageCanvasItem(int x, int y, int width, int height, 
                                       int libid, double baseLevel,
                                       QPointArray & arr, QCanvas * canvas,
                                       const QColor & color)
  : QCanvasRectangle(x, y, width, height, canvas),
    m_points(arr),
    m_libid(libid),
    m_baseLevel(baseLevel),
    m_color(color)
{
  show();
}

void CoverageCanvasItem::drawShape (QPainter & p)
{
  QPen pen(QPen(m_color,1));
  p.setPen(pen);
  p.drawPolyline(m_points);

  pen.setStyle(Qt::DotLine);
  p.setPen(pen);

  p.drawLine((int)(x()),           (int)(y() + height() - m_baseLevel),
             (int)(x() + width()), (int)(y() + height() - m_baseLevel));
}
