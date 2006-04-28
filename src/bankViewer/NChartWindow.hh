#ifndef NCHART_WINDOW_HH__
#define NCHART_WINDOW_HH__ 1

#include <qmainwindow.h>

#include "InsertStats.hh"

class QLineEdit;

class NChartWindow : public QMainWindow
{
  Q_OBJECT

public:
  NChartWindow(InsertStats * stats, QWidget * parent, const char * name);
 
private:
  InsertStats * m_stats;
};

#endif
