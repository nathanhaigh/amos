#ifndef INSERT_POSITION_HH_
#define INSERT_POSITION_HH_ 1

#include <qwidget.h>

class InsertPosition : public QWidget
{
  Q_OBJECT

public:
  InsertPosition(QWidget * parent, const char * name);

  void paintEvent(QPaintEvent * e);

public slots:
  void setVisibleRange(int, double);

private:
  double m_scale;
  int m_start;

};

#endif
