#include "InsertWindow.hh"
#include "InsertWidget.hh"

#include <qstatusbar.h>

using namespace std;

InsertWindow::InsertWindow(const std::string & bankname,
                           int contigId,
                           QWidget * parent,
                           const char * name)
  : QMainWindow(parent, name)
{
  resize(600, 300);

  InsertWidget * iw = new InsertWidget(bankname, contigId, this, "iw");
  setCentralWidget(iw);
  statusBar()->message("Ready", 2000);
  statusBar()->setSizeGripEnabled(false);


  connect(iw,          SIGNAL(setStatus(const QString &)),
          statusBar(), SLOT(message(const QString &)));

  connect(iw,   SIGNAL(setGindex(int)),
          this, SIGNAL(setGindex(int)));

  connect(this, SIGNAL(setTilingVisibleRange(int, int)),
          iw,   SLOT(setTilingVisibleRange(int, int)));
}
