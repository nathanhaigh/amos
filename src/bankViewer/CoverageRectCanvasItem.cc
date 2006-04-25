#include "CoverageRectCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>
#include <limits.h>

const int CoverageRectCanvasItem::RTTI = 123456;

#include <iostream>
using namespace std;

CoverageRectCanvasItem::CoverageRectCanvasItem(int x, int y,
                                               int width, int height, 
                                               int libid, double baseLevel,
                                               QPointArray & arr, 
                                               vector<double> & raw,
                                               bool copyRaw,
                                               QCanvas * canvas,
                                               const QColor & color)
  : QCanvasRectangle(x, y, width, height, canvas),
    m_points(arr),
    m_raw(raw),
    m_libid(libid),
    m_baseLevel(baseLevel),
    m_color(color),
    m_copyraw(copyRaw)
{
  show();
  m_low = (int)m_baseLevel;
  m_high = (int)m_baseLevel;
}

void CoverageRectCanvasItem::drawShape (QPainter & p)
{
  QBrush lowBrush  (UIElements::color_StretchedMate);
  QBrush highBrush (UIElements::color_CompressedMate);
  QPen lowPen  (UIElements::color_StretchedMate);
  QPen highPen (UIElements::color_CompressedMate);

  int px = (int)x();
  int py = 0;
  for ( QPointArray::Iterator i = m_points.begin(); i != m_points.end(); ++ i )
    {
      if ( abs(i->y()) != py )
        {
          if ( py < m_low )
            {
              p.setBrush (lowBrush);
              p.setPen (lowPen);
              p.drawRect (px, (int)y(), (int)i->x() - px + 1, height());
            }
          else if ( py > m_high )
            {
              p.setBrush (highBrush);
              p.setPen (highPen);
              p.drawRect (px, (int)y(), (int)i->x() - px + 1, height());
            }

          px = i->x();
          py = abs(i->y());
        }
    }

  if ( py < m_low )
    {
      p.setBrush (lowBrush);
      p.setPen (lowPen);
      p.drawRect (px, (int)y(), (int)(x()+width()) - px + 1, height());
    }
  else if ( py > m_high )
    {
      p.setBrush (highBrush);
      p.setPen (highPen);
      p.drawRect (px, (int)y(), (int)(x()+width()) - px + 1, height());
    }
}
