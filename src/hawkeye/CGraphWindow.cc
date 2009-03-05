#include "CGraphWindow.hh"
#include <q3canvas.h>
#include <qstatusbar.h>
#include <qsplitter.h>

#include "CGraphContig.hh"
#include "CGraphView.hh"
#include "CGraphEdge.hh"

#include "DataStore.hh"

using namespace AMOS;
using namespace std;

CGraphWindow::CGraphWindow(DataStore * datastore, 
                           QWidget * parent, 
                           const char * name)
  : Q3MainWindow(parent, name)
{
  setCaption("Contig Graph");
  m_contigHeight = 40;
  m_contigWidth  = 100;
  m_gutter = 10;

  m_datastore = datastore;
  m_canvas = new Q3Canvas(this, "cgcanvas");

  QSplitter * split = new QSplitter(this);
  split->setOrientation(Qt::Vertical);

  CGraphView * myview = new CGraphView(m_canvas, split, "cgview");

  m_edgeview = new Q3ListView(split, "edgeview");
  m_edgeview->setRootIsDecorated(true);
  m_edgeview->setAllColumnsShowFocus(true);
 

  m_edgeview->addColumn("Type");
  m_edgeview->addColumn("ID");
  m_edgeview->addColumn("Contig 1");
  m_edgeview->addColumn("Contig 2");
  m_edgeview->addColumn("Adjacency");
  m_edgeview->addColumn("Distance");
  m_edgeview->addColumn("SD");
  m_edgeview->addColumn("Weight");

  setCentralWidget(split);

  connect(myview, SIGNAL(setContigId(int)),
          this,   SIGNAL(setContigId(int)));

  connect(myview, SIGNAL(edgeSelected(AMOS::ContigEdge_t *)),
          this,   SLOT(edgeSelected(AMOS::ContigEdge_t *)));

  drawGraph();
}

void CGraphWindow::drawGraph()
{
  // clear and flush
  Q3CanvasItemList list = m_canvas->allItems();
  Q3CanvasItemList::Iterator it = list.begin();
  for (; it != list.end(); ++it) {
      if ( *it )
          delete *it;
  }

  m_edgeview->clear();

  int lineheight = m_contigHeight + m_gutter;

  int leftcount = 0;
  int rightcount = 0;

  ContigEdge_t edge;

  m_leftcontigs.clear();
  m_rightcontigs.clear();

  try
  {
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

        LinkAdjacency_t adj = edge.getAdjacency();

        QString side;

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
    
    // Draw myself
    CGraphContig * contig = new CGraphContig(m_datastore->m_contigId, NULL, 0,
                                             2*m_contigWidth+m_gutter, m_gutter,
                                             m_contigWidth, m_contigHeight,
                                             m_canvas);
    contig->show();

    // Draw Neighbors
    vector<ContigEdge_t>::iterator ci;
    for (ci =  m_leftcontigs.begin(), leftcount = 0;
         ci != m_leftcontigs.end();
         ci++, leftcount++)
    {
      drawNeighbor(&(*ci), 
                   m_gutter, 
                   leftcount*lineheight+m_gutter, 
                   false);
    }

    for (ci =  m_rightcontigs.begin(), rightcount = 0;
         ci != m_rightcontigs.end();
         ci++, rightcount++)
    {
      drawNeighbor(&(*ci), 
                   4*m_contigWidth+m_gutter, 
                   rightcount*lineheight+m_gutter, 
                   true);
    }

    int vmax = (rightcount > leftcount) ? rightcount : leftcount;
    vmax = (vmax ? vmax : 1);

    m_canvas->resize(5*m_contigWidth+2*m_gutter,vmax*lineheight+m_gutter);

    QString status = "Viewing contig graph for contig " + QString::number(m_datastore->m_contigId);
    statusBar()->message(status);
  }
  catch (Exception_t & e)
  {
    QString status = "Contig graph not available";
    statusBar()->message(status);
  }

  m_canvas->update();
}

void CGraphWindow::drawNeighbor(ContigEdge_t * edge, 
                                int xpos, 
                                int ypos,
                                bool onRight)
{
  bool rc = 0;
  LinkAdjacency_t adj = edge->getAdjacency();
  if (adj == 'I' || adj == 'O') { rc = 1; }

  CGraphEdge * line = new CGraphEdge(edge, m_canvas);
  CGraphContig * contig  = new CGraphContig(edge->getContigs().second, 
                                            edge, rc,
                                            xpos, ypos,
                                            m_contigWidth, m_contigHeight,
                                            m_canvas);

  QString side;
  if (onRight)
  {
    side = "Right";
    line->setPoints(xpos, ypos+m_contigHeight/2,
                    3*m_contigWidth+m_gutter, m_gutter+m_contigHeight/2);
  }
  else
  {
    side = "Left";
    line->setPoints(xpos+m_contigWidth, ypos+m_contigHeight/2,
                    2*m_contigWidth+m_gutter, m_gutter+m_contigHeight/2);
  }

  line->setPen(QPen(Qt::black, edge->getContigLinks().size()));
  line->show();
  contig->show();

  Q3ListViewItem * edgeitem = new Q3ListViewItem(m_edgeview);

  edgeitem->setText(0, QString("Edge"));
  edgeitem->setText(1, QString::number(edge->getIID()));
  edgeitem->setText(2, QString::number(edge->getContigs().first));
  edgeitem->setText(3, QString::number(edge->getContigs().second));
  edgeitem->setText(4, side);
  edgeitem->setText(5, QString::number(edge->getSize()));
  edgeitem->setText(6, QString::number(edge->getSD()));
  edgeitem->setText(7, QString::number(edge->getContigLinks().size()));

  vector<ID_t>::const_iterator ii;

  for (ii =  edge->getContigLinks().begin();
       ii != edge->getContigLinks().end();
       ii++)
  {
    ContigLink_t link;
    m_datastore->link_bank.fetch(*ii, link);

    if (link.getContigs().first != m_datastore->m_contigId)
    {
      link.flip();
    }

    Q3ListViewItem * linkitem = new Q3ListViewItem(edgeitem);

    linkitem->setText(0, QString((QChar)link.getType()));
    linkitem->setText(1, QString::number(link.getIID()));
    linkitem->setText(2, QString::number(link.getContigs().first));
    linkitem->setText(3, QString::number(link.getContigs().second));
    linkitem->setText(4, QString((QChar) link.getAdjacency()));
    linkitem->setText(5, QString::number(link.getSize()));
    linkitem->setText(6, QString::number(link.getSD()));
    linkitem->setText(7, QString::number(1));
  }
}

void CGraphWindow::contigChanged()
{
  drawGraph();
}

void CGraphWindow::edgeSelected(AMOS::ContigEdge_t *edge)
{
  QString id = QString::number(edge->getIID());
  QString status = "Viewing edge " + id;
  statusBar()->message(status);

  Q3ListViewItem * item = m_edgeview->firstChild();

  while (item)
  {
    if (item->text(1) == id)
    {
      item->setOpen(true);
      m_edgeview->setSelected(item, true);
      m_edgeview->ensureItemVisible(item);
      break;
    }

    item = item->nextSibling();
  }
}
