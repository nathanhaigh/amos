#include "ReadInfo.hh"
#include <qlabel.h>
#include <qscrollview.h>
#include <qlayout.h>
#include "ChromoField.hh"

using namespace std;

ReadInfo::ReadInfo(RenderSeq_t * read, 
                   const string & db, 
                   const string & cons,
                   const string & cstatus,
                   QWidget*parent, 
                   const char * name)
  : QDialog(parent, name)
{
  resize(1000,250);

  QBoxLayout * layout = new QVBoxLayout(this);

  QString s = QString::number(read->m_vectorpos) + " \""
            + read->m_read.getEID().c_str() + "\" {"
            + QString::number(read->m_read.getIID()) + "}";

  QLabel *label = new QLabel( this );
  label->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  label->setMaximumHeight(20);
  label->setText(s);
  label->setAlignment( AlignTop | AlignLeft );

  QScrollView * sv = new QScrollView(this, "scroll");
  layout->addWidget(label);
  layout->addWidget(sv);

  ChromoField* child1 = new ChromoField(read, db, cons, cstatus, sv->viewport(), "chromo");
  sv->addChild(child1);
}
