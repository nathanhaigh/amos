#include "ReadInfo.hh"
#include <qlabel.h>
#include <qlayout.h>

using namespace std;

ReadInfo::ReadInfo(RenderSeq_t * read, 
                   const string & db, 
                   const string & cons,
                   const string & cstatus,
                   QWidget*parent, 
                   const char * name)
  : QDialog(parent, name)
{
  resize(1000,256);

  QBoxLayout * layout = new QVBoxLayout(this);

  QString s = QString::number(read->m_vectorpos) + " \""
            + read->m_read.getEID().c_str() + "\" {"
            + QString::number(read->m_read.getIID()) + "}";

  QLabel *label = new QLabel( this );
  label->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  label->setMaximumHeight(20);
  label->setText(s);
  label->setAlignment( AlignTop | AlignLeft );

  m_sv = new QScrollView(this, "scroll");
  layout->addWidget(label);
  layout->addWidget(m_sv);

  m_chromo = new ChromoField(read, db, cons, cstatus, 
                             m_sv->viewport(), "chromo");
  m_sv->addChild(m_chromo);
}

void ReadInfo::setTilingVisibleRange(int grangeStart, int grangeEnd)
{
  int pos = m_chromo->getWindowPos((int) ((grangeStart+grangeEnd)/2));
  m_sv->ensureVisible(pos, 0);
}

