#include "OverviewField.hh"
#include <q3canvas.h>
#include <qpainter.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QResizeEvent>
#include <QMouseEvent>

#include <vector>
#include <iostream>

using namespace std;


OverviewField::OverviewField(Q3Canvas * canvas,
                             QWidget * parent, 
                             const char * name)
  :Q3CanvasView(canvas, parent, name, Qt::WNoAutoErase)
{
  setCursor(Qt::CrossCursor);
}

void OverviewField::wheelEvent(QWheelEvent * e)
{
  // just eat the event
}


void OverviewField::contentsMousePressEvent( QMouseEvent* e )
{
  QPoint real = inverseWorldMatrix().map(e->pos());

  cerr << "center on: " << real.x() << endl;

  emit centerView(real.x());
}

void OverviewField::resizeEvent(QResizeEvent * e)
{
  emit overviewResized();
}
