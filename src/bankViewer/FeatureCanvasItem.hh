#ifndef FEATURE_CANVAS_ITEM_HH_
#define FEATURE_CANVAS_ITEM_HH_ 1

#include <qcanvas.h>
#include "foundation_AMOS.hh"


class FeatureCanvasItem : public QCanvasRectangle
{
public:
  FeatureCanvasItem(int x, int y, int width, int height,
                    AMOS::Feature_t & feat, QCanvas * canvas);

  static const int RTTI;
  int rtti () const { return RTTI; }
  AMOS::Feature_t & m_feat;

protected:
  void drawShape (QPainter & p);

private:
};


#endif
