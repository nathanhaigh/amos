#include "CoverageRectCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <limits.h>
#include <math.h>

const int CoverageRectCanvasItem::RTTI = 123456;

#include <iostream>
using namespace std;

CoverageRectCanvasItem::CoverageRectCanvasItem(int x, int y,
                                               int width, int height, 
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
  m_low = m_baseLevel;
  m_high = m_baseLevel;
}

void CoverageRectCanvasItem::drawShape (QPainter & p)
{
  p.save();
  QColor shade;
  double px = x();
  double py = m_baseLevel;
  for ( Q3PointArray::Iterator i = m_points.begin(); i != m_points.end(); ++ i )
    {
      int cy = abs(i->y());
      if ( cy != py )
        {
          if ( py < m_low || py > m_high )
            {
              double df = fabs(m_baseLevel - py) / m_baseLevel * 100.0;
              if ( df < 1 ) df = 1;
              shade = m_color.light(int(df));
              p.setBrush (shade);
              p.setPen (shade);
              p.drawRect (int(px), int(y()), int(i->x()-px+1), height());
            }
              
          px = i->x();
          py = cy;
        }
    }
  p.restore();
}
