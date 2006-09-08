#include "ReadInfo.hh"
#include <qlabel.h>
#include <qlayout.h>
#include "ChromoField.hh"
#include "RenderSeq.hh"



using namespace std;

ReadInfo::ReadInfo(RenderSeq_t * read, 
                   const string & cons,
                   const string & cstatus,
                   QWidget*parent, 
                   const char * name)
  : QDialog(parent, name),
    m_read(*read)
{
  resize(1000,256);

  QBoxLayout * layout = new QVBoxLayout(this);

  QString s = QString(" \"")
            + m_read.m_read.getEID().c_str() + "\" {"
            + QString::number(m_read.m_read.getIID()) + "}";

  QLabel *label = new QLabel( this );
  label->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  label->setMaximumHeight(20);
  label->setText(s);
  label->setAlignment( AlignTop | AlignLeft );

  m_sv = new QScrollView(this, "scroll");
  layout->addWidget(label);
  layout->addWidget(m_sv);

  m_chromo = new ChromoField(&m_read, cons, cstatus, m_sv->viewport(), "chromo");
  m_sv->addChild(m_chromo);
}

void ReadInfo::setTilingVisibleRange(int contigid, int grangeStart, int grangeEnd)
{
  // TODO:: Assume contigid is current
  int pos = m_chromo->getWindowPos((int) ((grangeStart+grangeEnd)/2));
  m_sv->center(pos, 0);
}

