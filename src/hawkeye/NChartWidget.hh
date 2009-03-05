#ifndef NCHARTWIDGET_HH__
#define NCHARTWIDGET_HH__ 1

#include <qwidget.h>
#include <qpoint.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QMouseEvent>

class NChartStats;

class NChartWidget : public QWidget
{
Q_OBJECT

public:
  NChartWidget(QWidget * parent, const char * name);
  ~NChartWidget();

public slots:
  void setStats(NChartStats * stats);
  void mouseDoubleClickEvent(QMouseEvent * e);
  void mouseMoveEvent(QMouseEvent * e);
  void setOrdering(int ordering);
  void setColorStyle(int style);

signals:
  void idSelected(int id);

protected:
  void paintEvent(QPaintEvent *);
  void resizeEvent();

private:
  NChartStats * m_stats;
  bool m_grid;

  int m_histleft;
  int m_histwidth;
  int m_histtop;
  int m_histheight;
  int m_histbottom;

  int m_ordering;
  int m_colorstyle;

  double m_xscale;
  double m_yscale;

  QPoint m_clickpoint;
};



#endif
