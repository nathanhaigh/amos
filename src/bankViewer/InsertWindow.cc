#include "InsertWindow.hh"
#include "InsertWidget.hh"

#include <qstatusbar.h>
#include <qlabel.h>
#include <qslider.h>

using namespace std;

InsertWindow::InsertWindow(DataStore * datastore,
                           QWidget * parent,
                           const char * name)
  : QMainWindow(parent, name)
{
  resize(600, 300);

  QToolBar * options = new QToolBar(this, "options");
  options->setLabel("Options");

  new QLabel("Zoom", options, "zoomlbl");
  QSlider * zoom = new QSlider(1, 500, 10, 20, Qt::Horizontal, options, "zoom");

  InsertWidget * iw = new InsertWidget(datastore, this, "iw");
  setCentralWidget(iw);
  statusBar()->message("Ready", 2000);
  statusBar()->setSizeGripEnabled(false);


  connect(iw,          SIGNAL(setStatus(const QString &)),
          statusBar(), SLOT(message(const QString &)));

  connect(iw,   SIGNAL(setGindex(int)),
          this, SIGNAL(setGindex(int)));

  connect(this, SIGNAL(setTilingVisibleRange(int, int)),
          iw,   SLOT(setTilingVisibleRange(int, int)));

  connect(zoom, SIGNAL(valueChanged(int)),
          iw,   SLOT(setZoom(int)));

  connect(this, SIGNAL(refreshCanvas()),
          iw,   SLOT(refreshCanvas()));
}

void InsertWindow::contigChanged()
{
  emit refreshCanvas();

}
