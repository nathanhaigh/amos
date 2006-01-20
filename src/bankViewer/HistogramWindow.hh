#ifndef HISTOGRAM_WINDOW_HH__
#define HISTOGRAM_WINDOW_HH__ 1

#include <qmainwindow.h>

class InsertStats;
class QLineEdit;

class HistogramWindow : public QMainWindow
{
  Q_OBJECT

public:
  HistogramWindow(InsertStats * stats, QWidget * parent, const char * name);
 
public slots:
  void loadShadeSD();

signals:
  void setShadeSD(double);

private:
  InsertStats * m_stats;
  QLineEdit * m_sdpick;
};

#endif
