#ifndef CGRAPHWINDOW_HH_
#define CGRAPHWINDOW_HH_ 1

#include <q3mainwindow.h>
#include <q3canvas.h>
#include <q3listview.h>

#include <vector>
#include "foundation_AMOS.hh"


using std::vector;

class DataStore;

class CGraphWindow : public Q3MainWindow
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
  Q3Canvas * m_canvas;
  Q3ListView * m_edgeview;

  int m_contigWidth;
  int m_contigHeight;
  int m_gutter;

  vector<AMOS::ContigEdge_t> m_leftcontigs, m_rightcontigs;
};

#endif
