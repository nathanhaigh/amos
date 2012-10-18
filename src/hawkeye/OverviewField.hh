#ifndef OVERVIEW_FIELD_HH__
#define OVERVIEW_FIELD_HH__ 1

#include <q3canvas.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QResizeEvent>
#include <QMouseEvent>

class OverviewField : public Q3CanvasView
{
  Q_OBJECT

public:

  OverviewField(Q3Canvas * canvas,
                QWidget * parent = 0,
                const char * name = 0);

public slots:
  void resizeEvent(QResizeEvent * e);

signals:
  void centerView(int);
  void overviewResized();

protected:
  void wheelEvent(QWheelEvent *e);
  void contentsMousePressEvent( QMouseEvent* e );

private:

};


#endif
