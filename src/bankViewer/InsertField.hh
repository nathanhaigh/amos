#ifndef INSERT_FIELD_HH_
#define INSERT_FIELD_HH_ 1

#include <qcanvas.h>

class InsertField : public QCanvasView
{
  Q_OBJECT

public:
  InsertField(QCanvas * canvas, QWidget * parent = 0, const char * name = 0);

signals:
  void visibleRange(int, double);
  void setStatus(const QString & message);
  void setGindex(int gindex);

protected:
  void contentsMousePressEvent(QMouseEvent* e);
  void viewportPaintEvent(QPaintEvent * e);

};

#endif
