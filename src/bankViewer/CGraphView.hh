#ifndef CGRAPHVIEW_HH_
#define CGRAPHVIEW_HH_ 1

#include <qcanvas.h>

class CGraphView : public QCanvasView
{
  Q_OBJECT

public:
  CGraphView(QCanvas * canvas,
             QWidget * parent = 0,
             const char * name = 0);

signals:
  void setContigId(int);

protected:
  void contentsMousePressEvent(QMouseEvent * e);

};

#endif
