#ifndef INSERTWINDOW_HH_
#define INSERTWINDOW_HH_ 1

#include <qmainwindow.h>
#include <string>


class InsertWindow : public QMainWindow
{
  Q_OBJECT

public:
  InsertWindow(const std::string & bankname, int contigId,
               QWidget* parent=0, const char* name=0);
};

#endif
