#ifndef COVERAGE_CANVAS_ITEM_HH_
#define COVERAGE_CANVAS_ITEM_HH_ 1

#include <qcanvas.h>

class CoverageCanvasItem : public QCanvasRectangle
{
public:
  CoverageCanvasItem(int x, int y, int width, int height,
                     QPointArray & arr, QCanvas * canvas);

  static const int RTTI;
  int rtti () const { return RTTI; }

protected:
  void drawShape (QPainter & p);

private:
  QPointArray m_coveragePlot;
};


#endif
