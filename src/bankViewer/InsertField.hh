#ifndef INSERT_FIELD_HH_
#define INSERT_FIELD_HH_ 1

#include <qcanvas.h>
#include "DataStore.hh"

class Insert;

class InsertField : public QCanvasView
{
  Q_OBJECT

public:
  InsertField(DataStore * datastore,
              int & hoffset,
              QCanvas * canvas, 
              QWidget * parent = 0, 
              const char * name = 0);

signals:
  void visibleRange(int, double);
  void setStatus(const QString & message);
  void setGindex(int gindex);

protected:
  void contentsMousePressEvent(QMouseEvent* e);
  void viewportPaintEvent(QPaintEvent * e);

private:
  void getInsertString(QString & s, int which, Insert * ins);
  DataStore * m_datastore;
  int & m_hoffset;

};

#endif
