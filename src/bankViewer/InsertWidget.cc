#include "InsertWidget.hh"

#include <qlayout.h>

using namespace std;

InsertWidget::InsertWidget(const string & bankname, int contigId,
                           QWidget * parent, const char * name)
 : QWidget(parent, name)
{

  m_bankname = bankname;
  m_contigId = contigId;

  QBoxLayout * vbox = new QVBoxLayout(this);

  m_iposition = new InsertPosition(this, "insertposition");



  m_icanvas = new InsertCanvas(m_bankname, 
                               m_contigId,
                               this, "insertcanvas");

  m_ifield = new InsertField(m_icanvas, this, "qcv");
  m_ifield->show();

  vbox->addWidget(m_iposition);
  vbox->addWidget(m_ifield, 10);
  vbox->activate();

  connect(m_ifield, SIGNAL(visibleRange(int, double)),
          m_iposition, SLOT(setVisibleRange(int,double)));


  //connect(m_insertfield, SIGNAL(setGindex(int)),
  //       parent, SLOT(setGindex(int)));

}
