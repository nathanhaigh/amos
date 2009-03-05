#include "BufferedLineEdit.hh"

BufferedLineEdit::BufferedLineEdit(QWidget * parent, const char * name)
   : QLineEdit(parent, name)
{ 
    connect (this, SIGNAL(returnPressed()),
             this, SLOT(handleReturnPressed()));
}

void BufferedLineEdit::handleReturnPressed()
{
  emit newValue(text());
}

