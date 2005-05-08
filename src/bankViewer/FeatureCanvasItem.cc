#include "FeatureCanvasItem.hh"
#include "UIElements.hh"
#include <qpainter.h>

const int FeatureCanvasItem::RTTI = 132548;

FeatureCanvasItem::FeatureCanvasItem(int x, int y, int width, int height,
                                     AMOS::Feature_t & feat, QCanvas * canvas)
  : QCanvasRectangle(x, y, width, height, canvas), m_feat(feat)
{
  setPen(UIElements::getFeatureColor(feat.getType()));
  setBrush(UIElements::getFeatureColor(feat.getType()));
}
