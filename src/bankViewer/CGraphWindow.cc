#include "CGraphWindow.hh"
#include <qcanvas.h>
#include <qstatusbar.h>

#include "CGraphContig.hh"
#include "CGraphView.hh"

using namespace AMOS;

CGraphWindow::CGraphWindow(DataStore * datastore, 
                           QWidget * parent, 
                           const char * name)
  : QMainWindow(parent, name)
{
  cerr << "CGraphWindow" << endl;

  m_datastore = datastore;
  m_canvas = new QCanvas(this, "cgcanvas");
  CGraphView * myview = new CGraphView(m_canvas, this, "cgview");
  setCentralWidget(myview);

  connect(myview, SIGNAL(setContigId(int)),
          this,   SIGNAL(setContigId(int)));

  drawGraph();
}

void CGraphWindow::drawGraph()
{
  // clear and flush
  QCanvasItemList list = m_canvas->allItems();
  QCanvasItemList::Iterator it = list.begin();
  for (; it != list.end(); ++it) {
      if ( *it )
          delete *it;
  }

  int contigheight = 40;
  int contigwidth  = 100;
  int gutter = 10;
  int lineheight = contigheight + gutter;

  CGraphContig * contig;
  QCanvasLine * line;

  // Draw myself
  contig  = new CGraphContig(m_datastore->m_contigId, NULL, 0,
                             2*contigwidth+gutter, gutter,
                             contigwidth, contigheight,
                             m_canvas);
  contig->show();

  int leftcount = 0;
  int rightcount = 0;

  ContigEdge_t edge;

  m_leftcontigs.clear();
  m_rightcontigs.clear();

  m_datastore->edge_bank.seekg(1);
  while (m_datastore->edge_bank >> edge)
  {
    if ((edge.getContigs().first  == m_datastore->m_contigId) ||
        (edge.getContigs().second == m_datastore->m_contigId))
    {
      if (edge.getContigs().second == m_datastore->m_contigId)
      {
        edge.flip();
      }

      cerr << "Edge: "
           << edge.getContigs().second
           << "\tgs:" << edge.getSize()
           << "\tsd:" << edge.getSD()
           << "\tw:"  << edge.getContigLinks().size() 
           << endl;

      LinkAdjacency_t adj = edge.getAdjacency();

      if (adj == 'N' || adj == 'I')
      {
        m_rightcontigs.push_back(edge);
      }
      else
      {
        m_leftcontigs.push_back(edge);
      }
    }
  }

  vector<ContigEdge_t>::iterator ci;
  for (ci =  m_leftcontigs.begin(), leftcount = 0;
       ci != m_leftcontigs.end();
       ci++, leftcount++)
  {
    bool rc = 0;
    LinkAdjacency_t adj = ci->getAdjacency();
    if (adj == 'O') { rc = 1; }

    line = new QCanvasLine(m_canvas);
    contig  = new CGraphContig(ci->getContigs().second, &(*ci), rc,
                               gutter, leftcount*lineheight+gutter,
                               contigwidth, contigheight,
                               m_canvas);
    line->setPoints(contigwidth+gutter, leftcount*lineheight+gutter+contigheight/2,
                    2*contigwidth+gutter, gutter+contigheight/2);
    line->setPen(Qt::black);
    line->show();
    contig->show();
  }

  for (ci =  m_rightcontigs.begin(), rightcount = 0;
       ci != m_rightcontigs.end();
       ci++, rightcount++)
  {
    bool rc = 0;
    LinkAdjacency_t adj = ci->getAdjacency();
    if (adj == 'I') { rc = 1; }

    line = new QCanvasLine(m_canvas);
    contig  = new CGraphContig(ci->getContigs().second, &(*ci), rc,
                               4*contigwidth+gutter, rightcount*lineheight+gutter,
                               contigwidth, contigheight,
                               m_canvas);
    line->setPoints(4*contigwidth+gutter, rightcount*lineheight+gutter+contigheight/2,
                    3*contigwidth+gutter, gutter+contigheight/2);
    line->setPen(Qt::black);
    line->show();
    contig->show();
  }

  int vmax = (rightcount > leftcount) ? rightcount : leftcount;
  vmax = (vmax ? vmax : 1);

  m_canvas->resize(5*contigwidth+2*gutter,vmax*lineheight+gutter);
  m_canvas->update();

  statusBar()->message("Painted");
}

void CGraphWindow::contigChanged()
{
  drawGraph();
}
