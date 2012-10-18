#include "CoverageCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>

const int CoverageCanvasItem::RTTI = 132546;

using namespace std;

CoverageCanvasItem::CoverageCanvasItem(int x, int y, int width, int height, 
                                       int libid, double baseLevel,
                                       Q3PointArray & arr, 
                                       vector<double> & raw,
                                       bool copyRaw,
                                       Q3Canvas * canvas,
                                       const QColor & color)
  : Q3CanvasRectangle(x, y, width, height, canvas),
    m_points(arr),
    m_raw(raw),
    m_libid(libid),
    m_baseLevel(baseLevel),
    m_color(color),
    m_copyraw(copyRaw)
{
  show();
}

void CoverageCanvasItem::drawShape (QPainter & p)
{
  p.save();

  QPen pen(QPen(m_color,1));
  pen.setWidth(1);
  p.setPen(pen);
  p.drawPolyline(m_points);

  if (m_copyraw)
  {
    pen.setColor(Qt::gray);
  }

  pen.setStyle(Qt::DotLine);
  p.setPen(pen);

  p.drawLine((int)(x()),           (int)(y() + height() - m_baseLevel),
             (int)(x() + width()), (int)(y() + height() - m_baseLevel));

  p.restore();
}
