#include "InsertWidget.hh"

#include <qlayout.h>
#include <qlabel.h>
#include <qwmatrix.h>

using namespace std;

InsertWidget::InsertWidget(const string & bankname, int contigId,
                           QWidget * parent, const char * name)
 : QWidget(parent, name)
{
  m_bankname = bankname;
  m_contigId = contigId;

  QBoxLayout * outer = new QHBoxLayout(this);

  QBoxLayout * vbox = new QVBoxLayout(0);
  QBoxLayout * controlbox = new QVBoxLayout(0);

  m_zoom = new QSlider(1, 500, 10, 20, Qt::Horizontal, this, "m_zoom");
  QLabel * z_lbl = new QLabel(m_zoom, "Zoom", this, "z_lbl");
  z_lbl->setMinimumWidth(100);

  m_iposition = new InsertPosition(this, "insertposition");

  m_icanvas = new InsertCanvas(m_bankname, m_contigId,
                               this, "insertcanvas");

  m_ifield = new InsertField(m_icanvas, this, "qcv");
  m_ifield->show();

  vbox->addWidget(m_iposition);
  vbox->addWidget(m_ifield, 10);
  vbox->activate();

  controlbox->addWidget(z_lbl);
  controlbox->addWidget(m_zoom);
  controlbox->addStretch(10);

  outer->addSpacing(5);
  outer->addLayout(controlbox);
  outer->addSpacing(5);
  outer->addLayout(vbox);

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

  connect(m_zoom,   SIGNAL(valueChanged(int)),
          this,     SLOT(setZoom(int)));
}

void InsertWidget::setTilingVisibleRange(int gstart, int gend)
{
  m_tilingVisible->setSize(gend - gstart +1, m_icanvas->height());
  m_tilingVisible->move(gstart, 0);
  m_tilingVisible->show();

  m_ifield->ensureVisible(((int) (gstart + gend)/2), 0);

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
