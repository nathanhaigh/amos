#include "CoverageCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>

const int CoverageCanvasItem::RTTI = 132546;

using namespace std;

CoverageCanvasItem::CoverageCanvasItem(int x, int y, int width, int height, bool isClone,
                                       QPointArray & arr, QCanvas * canvas,
                                       double meanLevel)
  : QCanvasRectangle(x, y, width, height, canvas),
    m_coveragePlot(arr),
    m_isClone(isClone),
    m_meanLevel(meanLevel)
{
  show();
}

void CoverageCanvasItem::drawShape (QPainter & p)
{
  QPen pen(QPen((m_isClone ? UIElements::color_insertcoverage 
                           : UIElements::color_readcoverage), 1));
  p.setPen(pen);
  p.drawPolyline(m_coveragePlot);

  pen.setStyle(Qt::DotLine);
  p.setPen(pen);

  p.drawLine((int)(x()),           (int)(y() + height() - m_meanLevel),
             (int)(x() + width()), (int)(y() + height() - m_meanLevel));
}
