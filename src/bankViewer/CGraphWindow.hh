#ifndef CGRAPHWINDOW_HH_
#define CGRAPHWINDOW_HH_ 1

#include <qmainwindow.h>
#include <qcanvas.h>
#include <qlistview.h>

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
  void edgeSelected(AMOS::ContigEdge_t *);

private:
  void drawGraph();
  void drawNeighbor(AMOS::ContigEdge_t * edge,
                    int xpos, int ypos, bool onRight);

  DataStore * m_datastore;
  QCanvas * m_canvas;
  QListView * m_edgeview;

  int m_contigWidth;
  int m_contigHeight;
  int m_gutter;

  vector<AMOS::ContigEdge_t> m_leftcontigs, m_rightcontigs;
};

#endif
