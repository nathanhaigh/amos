#include "TilingField.hh"
#include <qpainter.h>
#include <qpixmap.h>
#include "ReadInfo.hh"
#include <qstring.h>
#include <qpointarray.h>

using namespace AMOS;
using namespace std;

TilingField::TilingField(vector<RenderSeq_t> & renderedSeqs,
                         string & consensus,
                         string & db,
                         int & gindex,
                         int & fontsize,
                         QWidget *parent, const char *name )
        : QWidget( parent, name ),
          m_fontsize(fontsize),
          m_gindex(gindex),
          m_db(db),
          m_consensus(consensus),
          m_renderedSeqs(renderedSeqs)
{
  setMinimumSize(width(), 200);
  setPalette( QPalette( QColor( 200, 200, 200) ) );
}


void TilingField::mouseDoubleClickEvent( QMouseEvent *e )
{
  int lineheight  = m_fontsize+5;

  int dcov = e->y();
  dcov /= lineheight;

  if (dcov >= (int) m_currentReads.size()) { return; }
  ReadInfo * readinfo = new ReadInfo(m_currentReads[dcov], 
                                     m_db, 
                                     m_consensus,
                                     this, 
                                     "readinfo");
  readinfo->show();
}

int imin (int a, int b)
{
  return a < b ? a : b;
}

void TilingField::paintEvent( QPaintEvent * )
{
  cerr << "paintTF" << endl;
  if (m_renderedSeqs.empty()) { return; }

  int height = 1000;

  QPixmap pix(this->width(), height);
  pix.fill(this, 0,0);

  QPainter p( &pix );
  QPen pen;
  pen.setColor(black);
  p.setPen(pen);

  int lineheight  = m_fontsize+5;
  int tilehoffset = m_fontsize*10;
  int seqnamehoffset = 10;
  int rchoffset = m_fontsize*8;

  int width = this->width();
  int displaywidth = (width-tilehoffset)/m_fontsize;

  p.setFont(QFont("Courier", m_fontsize));

  Pos_t grangeStart = m_gindex;
  Pos_t grangeEnd = imin(m_gindex + displaywidth, m_consensus.size()-1);

  // Figure out which reads tile this range
  vector<RenderSeq_t>::iterator ri;
  int dcov = 1;
  
  int clen = m_consensus.size();

  m_currentReads.clear();

  for (ri =  m_renderedSeqs.begin();
       ri != m_renderedSeqs.end(); 
       ri++)
  {
    if (RenderSeq_t::hasOverlap(grangeStart, grangeEnd, 
                                ri->m_offset, ri->m_nucs.size(), 
                                clen))
    {
      pen.setColor(black);
      p.setPen(pen);
      p.setBrush(Qt::SolidPattern);

//      QString s = QString::number(ri->m_vectorpos);
      QString s = ri->m_read.getEID().c_str();

      QPointArray a(3);
      if (ri->m_rc)
      {
        a[0]=QPoint(rchoffset+5,lineheight*dcov-1);
        a[1]=QPoint(rchoffset+5,lineheight*dcov-7);
        a[2]=QPoint(rchoffset,lineheight*dcov-4);
      }
      else
      {
        a[0]=QPoint(rchoffset,lineheight*dcov-1);
        a[1]=QPoint(rchoffset,lineheight*dcov-7);
        a[2]=QPoint(rchoffset+5,lineheight*dcov-4);
      }

      p.drawPolygon(a);
      p.drawText(seqnamehoffset, lineheight*dcov, s);

      s = "";
      for (int j = grangeStart; j <= grangeEnd; j++)
      {
        char b = ri->base(j);

        switch (b)
        {
          case 'A': pen.setColor(red); break;
          case 'C': pen.setColor(green); break;
          case 'G': pen.setColor(blue); break;
          case 'T': pen.setColor(yellow); break;
          default: pen.setColor(black); break;
        };

        p.setPen(pen);

        QString s;
        s+= b;
        p.drawText (tilehoffset + (j-grangeStart)*m_fontsize, lineheight*dcov, s);
      }

      dcov++;

      m_currentReads.push_back(ri);
    }
  }

  height = lineheight*dcov;

  p.end();
  p.begin(this);
  pix.resize(this->width(), height);

  p.drawPixmap(0, 0, pix);
}


QSizePolicy TilingField::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}
