#ifndef OVERVIEW_WINDOW_HH__
#define OVERVIEW_WINDOW_HH__ 1

#include <qmainwindow.h>

using std::string;

class DataStore;

class OverviewWindow : public QMainWindow
{
  Q_OBJECT

public:
  OverviewWindow(DataStore * datastore,
                 QWidget * parent = 0,
                 const char * name = 0);

private:

  DataStore * m_datastore;
};


#endif
