#ifndef INSERT_WIDGET_HH_
#define INSERT_WIDGET_HH_ 1

#include <qwidget.h>
#include <string>

#include "InsertField.hh"
#include "InsertCanvas.hh"
#include "InsertPosition.hh"


class InsertWidget : public QWidget
{
  Q_OBJECT

public:

  InsertWidget(const std::string & bankname, int contigId,
               QWidget * parent = 0, const char * name = 0);


private:
  InsertField    * m_ifield;
  InsertCanvas   * m_icanvas;
  InsertPosition * m_iposition;

  std::string m_bankname;
  int m_contigId;
};

#endif
