#include "InsertWindow.hh"

InsertWindow::InsertWindow(const string & bankname,
                           int contigId,
                           QWidget * parent,
                           const char * name)
  : QMainWindow(parent, name)
{
  resize(600, 300);
  m_bankname = bankname;
  m_contigId = contigId;

  m_sv = new QScrollView(this, "insertscroll");
  m_insertfield = new InsertField(m_bankname, 
                                  m_contigId,
                                  m_sv->viewport(),
                                  "inserts");
  m_sv->addChild(m_insertfield);

  setCentralWidget(m_sv);




}
