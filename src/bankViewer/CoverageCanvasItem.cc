#include "CoverageCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>

const int CoverageCanvasItem::RTTI = 132546;

using namespace std;

CoverageCanvasItem::CoverageCanvasItem(int x, int y, int width, int height,
                                       QPointArray & arr, QCanvas * canvas)
  : QCanvasRectangle(x, y, width, height, canvas),
    m_coveragePlot(arr)
{

}

void CoverageCanvasItem::drawShape (QPainter & p)
{
  p.drawPolyline(m_coveragePlot);
}
