#ifndef CGRAPHWINDOW_HH_
#define CGRAPHWINDOW_HH_ 1

#include <qmainwindow.h>
#include <qcanvas.h>

#include "DataStore.hh"

class CGraphWindow : public QMainWindow
{
  Q_OBJECT

public:
  CGraphWindow(DataStore * datastore, QWidget * parent=0, const char * name=0);

signals:
  void setContigId(int);

public slots:
  void contigChanged();

private:
  void drawGraph();

  DataStore * m_datastore;
  QCanvas * m_canvas;

  vector<AMOS::ContigEdge_t> m_leftcontigs, m_rightcontigs;
};

#endif
