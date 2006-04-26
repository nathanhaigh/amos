#ifndef OVERVIEW_FIELD_HH__
#define OVERVIEW_FIELD_HH__ 1

#include <qcanvas.h>

class OverviewField : public QCanvasView
{
  Q_OBJECT

public:

  OverviewField(QCanvas * canvas,
                QWidget * parent = 0,
                const char * name = 0);

signals:
  void centerView(int);

protected:
  void wheelEvent(QWheelEvent *e);
  void contentsMousePressEvent( QMouseEvent* e );

private:

};


#endif
