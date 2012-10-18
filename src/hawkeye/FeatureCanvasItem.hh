#ifndef FEATURE_CANVAS_ITEM_HH_
#define FEATURE_CANVAS_ITEM_HH_ 1

#include <q3canvas.h>
#include "foundation_AMOS.hh"


class FeatureCanvasItem : public Q3CanvasRectangle
{
public:
  FeatureCanvasItem(int x, int y, int width, int height,
                    AMOS::Feature_t & feat, Q3Canvas * canvas);

  static const int RTTI;
  int rtti () const { return RTTI; }

  void drawShape(QPainter & p);
  AMOS::Feature_t & m_feat;

  static const char * getFeatureTypeStr(AMOS::FeatureType_t type);

private:
};


#endif
