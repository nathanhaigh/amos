#include "CGraphContig.hh"

#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>

const int CGraphContig::RTTI = 132546;

using namespace AMOS;
using namespace std;

CGraphContig::CGraphContig(ID_t id, ContigEdge_t * edge, bool rc,
                           int x, int y, int width, int height,
                           Q3Canvas * canvas)
  : Q3CanvasRectangle(x, y, width, height, canvas)
{
  m_id = id;
  m_edge = edge;
  m_rc = rc;

  if (edge)
  {
    if (m_id != edge->getContigs().second)
    {
      cerr << "edge corruption detected! m_id:" << m_id
           << " second: " << m_edge->getContigs().second
           << endl;

    }
  }
}

void CGraphContig::drawShape(QPainter & p)
{
  p.setPen(Qt::black);
  p.setBrush(Qt::yellow);

  int contigheight = 40;
  int contigwidth  = 100;

  int x = (int) (this->x());
  int y = (int) (this->y());

  Q3PointArray forward(5);
  forward[0] = QPoint(x+0,                          y+0);
  forward[1] = QPoint(x+contigwidth-contigheight/2, y+0);
  forward[2] = QPoint(x+contigwidth,                y+contigheight/2);
  forward[3] = QPoint(x+contigwidth-contigheight/2, y+contigheight);
  forward[4] = QPoint(x+0,                          y+contigheight);

  Q3PointArray reverse(5);
  reverse[0] = QPoint(x+0,              y+contigheight/2);
  reverse[1] = QPoint(x+contigheight/2, y+0);
  reverse[2] = QPoint(x+contigwidth,    y+0);
  reverse[3] = QPoint(x+contigwidth,    y+contigheight);
  reverse[4] = QPoint(x+contigheight/2, y+contigheight);

  if (m_rc)
  {
    p.drawPolygon(reverse);
  }
  else
  {
    p.drawPolygon(forward);
  }

  QString s = QString::number(m_id);

  if (m_edge)
  {
    s += " {" + QString::number(m_edge->getContigLinks().size()) + "}";
    s += "\n [" + QString::number(m_edge->getSize()) + "]";
  }

  p.drawText(x, y, width(), height(), 
             Qt::AlignHCenter | Qt::AlignVCenter, s);

  if (m_edge)
  {
    if (m_id != m_edge->getContigs().second)
    {
      cerr << "m_edge corruption detected! m_id:" << m_id
           << " second: " << m_edge->getContigs().second
           << endl;

    }
  }
}


