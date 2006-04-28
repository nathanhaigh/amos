#ifndef NCHARTWIDGET_HH__
#define NCHARTWIDGET_HH__ 1

#include <qwidget.h>

class InsertStats;

class NChartWidget : public QWidget
{
Q_OBJECT

public:
  NChartWidget(QWidget * parent, const char * name);
  ~NChartWidget();

public slots:
  void setStats(InsertStats * stats);

protected:
  void paintEvent(QPaintEvent *);

private:
  InsertStats * m_stats;
  bool m_grid;
};



#endif
