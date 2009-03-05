#ifndef HISTOGRAM_WINDOW_HH__
#define HISTOGRAM_WINDOW_HH__ 1

#include <q3mainwindow.h>

#include "InsertStats.hh"

class QLineEdit;

class HistogramWindow : public Q3MainWindow
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
