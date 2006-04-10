#include "OverviewWindow.hh"
#include "OverviewWidget.hh"
#include <qcanvas.h>

#include "DataStore.hh"
#include "foundation_AMOS.hh"

#include <vector>

using namespace std;
using namespace AMOS;


OverviewWindow::OverviewWindow(DataStore * datastore,
                               QWidget * parent, 
                               const char * name)
  :QMainWindow(parent, name)
{
  resize (800,600);
  m_datastore = datastore;

  OverviewWidget * ow = new OverviewWidget (datastore, this, "ow");
  setCentralWidget(ow);
}

