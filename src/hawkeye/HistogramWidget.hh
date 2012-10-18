#ifndef HISTOGRAMWIDGET_HH__
#define HISTOGRAMWIDGET_HH__ 1

#include <qwidget.h>
//Added by qt3to4:
#include <QPaintEvent>

class InsertStats;

class HistogramWidget : public QWidget
{
Q_OBJECT

public:
  HistogramWidget(InsertStats * stats, QWidget * parent, const char * name);
  ~HistogramWidget();

public slots:
  void setBucketCount(int);
  void setFitZero(bool);
  void setEnableGrid(bool);
  void setShadeMean(bool);
  void setShadeSD(double);

protected:
  void paintEvent(QPaintEvent *);

private:
  InsertStats * m_stats;
  bool m_fitzero;
  bool m_grid;
  bool m_shademean;
  double m_shadesd;
};



#endif
