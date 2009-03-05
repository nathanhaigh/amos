#include "FeatureCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>
#include "foundation_AMOS.hh"

const int FeatureCanvasItem::RTTI = 132548;

FeatureCanvasItem::FeatureCanvasItem(int x, int y, int width, int height,
                                     AMOS::Feature_t & feat, Q3Canvas * canvas)
  : Q3CanvasRectangle(x, y, width, height, canvas), m_feat(feat)
{
  setPen(UIElements::getFeatureColor(feat.getType()));
  setBrush(UIElements::getFeatureColor(feat.getType()));
}

void FeatureCanvasItem::drawShape(QPainter & p)
{
  if (m_feat.getType() == 'B')
  {
    p.drawLine((int) x(),                 (int) y(),
               (int) (x() + width() - 1), (int) y());

    if (m_feat.getRange().isReverse())
    {
      p.drawRect((int) (x() + width() - width()/2),   (int) y(),
                 (int) (width()/2),                   (int) height());
    }
    else
    {
      p.drawRect((int) x(),                 (int) y(),
                 (int) (width()/2),         (int) height());
    }
  }
  else
  {
    p.drawRect((int) x(),      (int) y(),
               (int) width(),  (int) height());
  }
}
