#include "InsertWidget.hh"

#include <qlayout.h>
#include <qlabel.h>
#include <qwmatrix.h>

using namespace std;

InsertWidget::InsertWidget(DataStore * datastore,
                           QWidget * parent, const char * name)
 : QWidget(parent, name)
{
  QBoxLayout * vbox = new QVBoxLayout(this);

  m_iposition = new InsertPosition(this, "insertposition");
  m_icanvas   = new InsertCanvas(datastore, this, "icanvas");
  m_ifield    = new InsertField(m_icanvas, this, "qcv");
  m_ifield->show();

  vbox->addWidget(m_iposition);
  vbox->addWidget(m_ifield, 10);
  vbox->activate();

  m_tilingVisible = new QCanvasRectangle(m_icanvas);
  m_tilingVisible->setZ(-1);
  m_tilingVisible->setBrush(QColor(100,100,100));
  m_tilingVisible->setPen(QColor(100,100,100));

  connect(m_ifield, SIGNAL(visibleRange(int, double)),
          m_iposition, SLOT(setVisibleRange(int,double)));

  connect(m_ifield, SIGNAL(setStatus(const QString &)),
          this,     SIGNAL(setStatus(const QString &)));

  connect(m_ifield, SIGNAL(setGindex(int)),
          this,     SIGNAL(setGindex(int)));
}

void InsertWidget::setTilingVisibleRange(int gstart, int gend)
{
  QRect rc = QRect(m_ifield->contentsX(), m_ifield->contentsY(),
                   m_ifield->visibleWidth(), m_ifield->visibleHeight());
  QRect canvasRect = m_ifield->inverseWorldMatrix().mapRect(rc);

  m_tilingVisible->setSize(gend - gstart +1, m_icanvas->height());
  m_tilingVisible->move(gstart, 0);
  m_tilingVisible->show();


  int mapx;
  int mapy;

  m_ifield->worldMatrix().map((gstart + gend)/2, canvasRect.y() + canvasRect.height()/2,
                              &mapx, &mapy);

  m_ifield->ensureVisible(mapx, mapy);

  m_icanvas->update();
}

void InsertWidget::setZoom(int zoom)
{
  double xfactor = 1.00/zoom;

  QWMatrix matrix = m_ifield->worldMatrix();
  QWMatrix imatrix = m_ifield->inverseWorldMatrix();

  // reset to identity
  m_ifield->setWorldMatrix(imatrix);

  // set zoom
  QWMatrix newzoom(xfactor, matrix.m12(), matrix.m21(), matrix.m22(),
                   matrix.dx(), matrix.dy());
  m_ifield->setWorldMatrix(newzoom);
}

void InsertWidget::refreshCanvas()
{
  m_icanvas->drawCanvas();
}
