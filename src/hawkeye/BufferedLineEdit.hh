#ifndef BUFFERED_LINE_EDIT
#define BUFFERED_LINE_EDIT 1

#include <qlineedit.h>

class BufferedLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  
  BufferedLineEdit(QWidget * parent, const char * name=0);

public slots:
   void handleReturnPressed();

signals:
  void newValue(const QString & string);

};

#endif
