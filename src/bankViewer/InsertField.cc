#include "InsertField.hh"
#include <qwmatrix.h>

#include <iostream>
using namespace std;

InsertField::InsertField(QCanvas * canvas, QWidget * parent, const char * name)
 : QCanvasView(canvas, parent, name)
{

   QWMatrix m = worldMatrix();
   m.translate(20, 0);
   m.scale( 0.05, 1.0 );
   setWorldMatrix( m );

}

void InsertField::contentsMousePressEvent( QMouseEvent* e )
{
  QPoint real = inverseWorldMatrix().map(e->pos());
  cerr << "position is " << real.x() << endl;

  QCanvasItemList l = canvas()->collisions(real);

  for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
  {
    if ( (*it)->rtti() == QCanvasRectangle::RTTI )
    qDebug("A QCanvasRectangle lies somewhere at this point");
  }
}

void InsertField::viewportPaintEvent(QPaintEvent * e)
{
  QRect rc = QRect(contentsX(),    contentsY(),
                   visibleWidth(), visibleHeight() );
  QRect canvasRect = inverseWorldMatrix().mapRect(rc);

  QCanvasView::viewportPaintEvent(e);

  emit visibleRange(canvasRect.x(), worldMatrix().m11());
}


