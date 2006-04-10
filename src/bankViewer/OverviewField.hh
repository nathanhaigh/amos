#ifndef OVERVIEW_FIELD_HH__
#define OVERVIEW_FIELD_HH__ 1

#include <qcanvas.h>

class DataStore;

class OverviewField : public QCanvasView
{
  Q_OBJECT

public:

  OverviewField(DataStore * datastore,
                QCanvas * canvas,
                QWidget * parent = 0,
                const char * name = 0);

private:

  DataStore * m_datastore;
};


#endif
