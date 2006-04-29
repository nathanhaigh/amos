#ifndef NCHART_WINDOW_HH__
#define NCHART_WINDOW_HH__ 1

#include <qmainwindow.h>


class NChartStats;

class NChartWindow : public QMainWindow
{
  Q_OBJECT

public:
  NChartWindow(NChartStats * stats, QWidget * parent, const char * name);
};

#endif
