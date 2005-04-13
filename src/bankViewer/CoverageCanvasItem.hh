#ifndef COVERAGE_CANVAS_ITEM_HH_
#define COVERAGE_CANVAS_ITEM_HH_ 1

#include <qcanvas.h>

class CoverageCanvasItem : public QCanvasRectangle
{
public:
  CoverageCanvasItem(int x, int y, int width, int height, bool isClone,
                     QPointArray & arr, QCanvas * canvas);

  static const int RTTI;
  int rtti () const { return RTTI; }
  QPointArray m_coveragePlot;
  bool m_isClone;

protected:
  void drawShape (QPainter & p);

};


#endif
