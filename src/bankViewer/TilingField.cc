#include "TilingField.hh"
#include <qpainter.h>
#include <qpixmap.h>
#include "ReadInfo.hh"
#include <qstring.h>
#include <qpointarray.h>

int imin (int a, int b)
{
  return a < b ? a : b;
}
int imax (int a, int b)
{
  return a > b ? a : b;
}

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
  m_width=600;
  m_height=198;
  m_stabletiling = 0;

  setMinimumSize(m_width, m_height);
  setPalette(QPalette(QColor(170, 170, 170)));
}

void TilingField::toggleStable(bool stable)
{
  m_stabletiling = stable;
  repaint();
}


void TilingField::mouseDoubleClickEvent( QMouseEvent *e )
{
  int gutter      = m_fontsize/2;
  int lineheight  = m_fontsize+gutter;

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



void TilingField::paintEvent( QPaintEvent * )
{
  // cerr << "paintTField:" << m_renderedSeqs.size() << endl;
  if (m_renderedSeqs.empty()) { resize(m_width, m_height); return; }

  int gutter         = m_fontsize/2;
  int lineheight     = m_fontsize+gutter;
  int tilehoffset    = m_fontsize*12;
  int seqnamehoffset = gutter;
  int rchoffset      = m_fontsize*11;

  int displaywidth = (m_width-tilehoffset)/m_fontsize;

  int height = imax(m_renderedSeqs.size() * lineheight, 10000);

  QPixmap pix(m_width, height);
  pix.fill(this, 0,0);

  QPainter p(&pix);
  QPen pen;
  pen.setColor(black);
  p.setPen(pen);
  p.setFont(QFont("Helvetica", m_fontsize));
  p.setBrush(Qt::SolidPattern);

  QColor offsetColor(190,190,190);

  QPointArray rcflag(3);
  int tridim = m_fontsize/2;
  int trioffset = m_fontsize/3;

  // Figure out which reads tile this range
  m_currentReads.clear();

  Pos_t grangeStart = m_gindex;
  Pos_t grangeEnd = imin(m_gindex + displaywidth, m_consensus.size()-1);

  vector<RenderSeq_t>::iterator ri;
  int dcov = 0;
  int ldcov = 0;
  int clen = m_consensus.size();

  for (ri =  m_renderedSeqs.begin();
       ri != m_renderedSeqs.end(); 
       ri++)
  {
    int hasOverlap = RenderSeq_t::hasOverlap(grangeStart, grangeEnd, 
                                             ri->m_offset, ri->m_nucs.size(), 
                                             clen);

    if (hasOverlap || m_stabletiling)
    {
      // offset rectangle
      if (dcov % 2)
      {
        pen.setColor(offsetColor);
        p.setPen(pen);
        p.setBrush(offsetColor);
        p.drawRect(0, ldcov, m_width, lineheight);
      }

      // black pen
      pen.setColor(black);
      p.setPen(pen);
      p.setBrush(black);

      // RC Flag
      if (ri->m_rc)
      {
        rcflag[0]=QPoint(rchoffset+tridim, ldcov+trioffset);
        rcflag[1]=QPoint(rchoffset+tridim, ldcov+trioffset+tridim);
        rcflag[2]=QPoint(rchoffset,        ldcov+trioffset+tridim/2);
      }
      else
      {
        rcflag[0]=QPoint(rchoffset,        ldcov+trioffset);
        rcflag[1]=QPoint(rchoffset,        ldcov+trioffset+tridim);
        rcflag[2]=QPoint(rchoffset+tridim, ldcov+trioffset+tridim/2);
      }
      p.drawPolygon(rcflag);

      // Seqname
      QString s = ri->m_read.getEID().c_str();
      p.drawText(seqnamehoffset, ldcov,
                 rchoffset-seqnamehoffset, lineheight,
                 Qt::AlignLeft | Qt::AlignBottom, s);

      // Bases
      if (hasOverlap)
      {
        for (int j = grangeStart; j <= grangeEnd; j++)
        {
          char b = ri->base(j);

          switch (b)
          {
            case 'A': pen.setColor(darkGreen); break;
            case 'C': pen.setColor(blue); break;
            case 'G': pen.setColor(yellow); break;
            case 'T': pen.setColor(red); break;
            default:  pen.setColor(black); break;
          };

          p.setPen(pen);

          s = b;
          p.drawText(tilehoffset + (j-grangeStart)*m_fontsize, ldcov, 
                     m_fontsize, lineheight,
                     Qt::AlignHCenter | Qt::AlignBottom, s);
        }
      }

      dcov++;
      ldcov = lineheight * dcov;
      m_currentReads.push_back(ri);
    }
  }

  p.end();
  p.begin(this);

  pix.resize(m_width, ldcov);
  p.drawPixmap(0, 0, pix);
  resize(m_width, imax(ldcov, m_height));
}


QSizePolicy TilingField::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void TilingField::setSize(int width, int height)
{
  m_width = width;
  m_height = height;
}
