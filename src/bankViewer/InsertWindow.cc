#include "InsertWindow.hh"
#include "InsertWidget.hh"

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
}

