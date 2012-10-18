#ifndef COVERAGE_RECT_CANVAS_ITEM_HH_
#define COVERAGE_RECT_CANVAS_ITEM_HH_ 1

#include <q3canvas.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <vector>

class CoverageRectCanvasItem : public Q3CanvasRectangle
{
public:
  CoverageRectCanvasItem(int x, int y,
                         int width, int height, 
                         int libid, double baseLevel,
                         Q3PointArray & arr, 
                         std::vector<double> & raw,
                         bool copyRaw,
                         Q3Canvas * canvas,
                         const QColor & color);

  static const int RTTI;
  int rtti () const { return RTTI; }

  Q3PointArray         m_points;
  std::vector<double> m_raw;

  int                 m_libid;
  double              m_baseLevel;
  QColor              m_color;
  bool                m_copyraw;

  double m_low;
  double m_high;

protected:
  void drawShape (QPainter & p);

};


#endif
