#include "OverviewField.hh"
#include <qcanvas.h>
#include <qpainter.h>
#include <qcursor.h>

#include <vector>
#include <iostream>

using namespace std;


OverviewField::OverviewField(QCanvas * canvas,
                             QWidget * parent, 
                             const char * name)
  :QCanvasView(canvas, parent, name)
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

  emit centerView(real.x());
}
