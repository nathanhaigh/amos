#ifndef INSERTWINDOW_HH_
#define INSERTWINDOW_HH_ 1

#include <qmainwindow.h>
#include <qscrollview.h>
#include <string>

#include "InsertField.hh"


class InsertWindow : public QMainWindow
{
  Q_OBJECT

public:
  InsertWindow(const string & bankname,
               int contigId,
               QWidget* parent=0, const char* name=0);

private:
  QScrollView * m_sv;
  InsertField * m_insertfield;

  string m_bankname;
  int m_contigId;
};

#endif
