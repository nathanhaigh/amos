#include "TilingField.hh"
#include <qpainter.h>
#include <qpixmap.h>
#include "ReadInfo.hh"
#include <qstring.h>
#include <qpointarray.h>
#include "UIElements.hh"

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
                         const string & consensus,
                         const string & cstatus,
                         const string & db,
                         int & gindex,
                         int & fontsize,
                         QWidget *parent, const char *name )
        : QWidget( parent, name ),
          m_fontsize(fontsize),
          m_gindex(gindex),
          m_db(db),
          m_consensus(consensus),
          m_cstatus(cstatus),
          m_renderedSeqs(renderedSeqs)
{
  m_width=600;
  m_height=0;
  m_stabletiling = 0;
  m_highlightdiscrepancy = 0;
  m_traceheight = 50;
  m_displayqv = 0;

  m_clickTimer = new QTimer(this, 0);
  connect (m_clickTimer, SIGNAL(timeout()),
           this, SLOT(singleClick()));

  m_clickstate = 0;

  setMinimumSize(m_width, m_height);
  setPalette(QPalette(QColor(170, 170, 170)));
}

void TilingField::toggleStable(bool stable)
{
  m_stabletiling = stable;
  repaint();
}

int TilingField::getReadCov(int y)
{
  int dcov;
  vector<RenderSeq_t *>::iterator i;
  for (i =  m_currentReads.begin(), dcov = 0;
       i != m_currentReads.end();
       i++, dcov++)
  {
    if (y >= (*i)->m_displaystart && y < (*i)->m_displayend)
    {
      //cerr << "Hit " << (*i)->m_read.getEID() << " [" << dcov << "]" << endl;
      return dcov;
    }
  }

  return -1;
}

void TilingField::mousePressEvent(QMouseEvent *e)
{
}

void TilingField::singleClick()
{
  //cerr << "Timer fired! single click" << endl;

  int dcov = getReadCov(m_yclick);
  if (dcov == -1) { return; }

  m_currentReads[dcov]->m_displayTrace = !m_currentReads[dcov]->m_displayTrace;

  if (m_currentReads[dcov]->m_displayTrace)
  {
    m_currentReads[dcov]->loadTrace(m_db);
  }

  repaint();
}

void TilingField::mouseReleaseEvent( QMouseEvent * e)
{
  //cerr << "mouserelease state:" << m_clickstate << endl;
  m_clickTimer->start(400, true);
  m_yclick = e->y();
}


void TilingField::mouseDoubleClickEvent( QMouseEvent *e )
{
  //cerr << "doubleclick, stop timer" << endl;
  m_clickTimer->stop();

  int dcov = getReadCov(e->y());
  if (dcov == -1) { return; }
  ReadInfo * readinfo = new ReadInfo(m_currentReads[dcov], 
                                     m_db, 
                                     m_consensus,
                                     m_cstatus,
                                     this, 
                                     "readinfo");
  readinfo->show();
}



void TilingField::paintEvent( QPaintEvent * )
{
  // cerr << "paintTField:" << m_renderedSeqs.size() << endl;
  if (m_renderedSeqs.empty()) { resize(m_width, m_height); return; }

  int basespace      = 5;
  int gutter         = m_fontsize/2;
  int lineheight     = m_fontsize+gutter;
  int tilehoffset    = m_fontsize*12;
  int seqnamehoffset = gutter;
  int rchoffset      = m_fontsize*11;
  int basewidth      = m_fontsize+basespace;

  int displaywidth = (m_width-tilehoffset)/basewidth;

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
      ri->m_displaystart = ldcov;

      int readheight = lineheight; // seqname
      if (m_displayqv)        { readheight += lineheight; }
      if (ri->m_displayTrace) { readheight += m_traceheight; }
    
      // offset rectangle
      if (dcov % 2)
      {
        pen.setColor(offsetColor);
        p.setPen(pen);
        p.setBrush(offsetColor);
        p.drawRect(0, ldcov, m_width, readheight);
      }

      // black pen
      pen.setColor(black);
      p.setPen(pen);
      p.setBrush(black);
      p.setFont(QFont("Helvetica", m_fontsize));

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

      if (hasOverlap)
      {
        for (int j = grangeStart; j <= grangeEnd; j++)
        {
          int hoffset = tilehoffset + (j-grangeStart)*basewidth;

          // Bases
          char b = ri->base(j);
          s = b;

          p.setPen(UIElements::getBaseColor(b));
          p.setFont(QFont("Helvetica", m_fontsize));
          p.drawText(hoffset, ldcov, 
                     m_fontsize, lineheight,
                     Qt::AlignHCenter | Qt::AlignBottom, s);

          // QV
          if (m_displayqv)
          {
            int qv = ri->qv(j);
            if (qv != -1)
            {
              s = QString::number(qv);

              p.setFont(QFont("Helvetica", (int)(m_fontsize*.75)));
              p.setPen(black);
              p.drawText(hoffset, ldcov+lineheight,
                         m_fontsize, m_fontsize,
                         Qt::AlignHCenter | Qt::AlignBottom, s);
            }
          }
        }

        ldcov += lineheight;
        if (m_displayqv) { ldcov += lineheight; }

        if (ri->m_displayTrace) 
        { 
          int baseline = ldcov + m_traceheight - 10;
          ldcov += m_traceheight;
          
          if (ri->m_trace)
          {
            if (!ri->m_rc)
            {
              for (int channel = 0; channel < 4; channel++)
              {
                unsigned short * trace = NULL;
                switch (channel)
                {
                  case 0: trace = ri->m_trace->traceA; UIElements::setBasePen(pen, 'A'); break;
                  case 1: trace = ri->m_trace->traceC; UIElements::setBasePen(pen, 'C'); break;
                  case 2: trace = ri->m_trace->traceG; UIElements::setBasePen(pen, 'G'); break;
                  case 3: trace = ri->m_trace->traceT; UIElements::setBasePen(pen, 'T'); break;
                };

                p.setPen(pen);

                bool first = true;

                for (int j = grangeStart-1; j <= grangeEnd+1; j++)
                {
                  int peakposition = ri->m_bcpos[ri->getGSeqPos(j)];
                  int nextpeakposition = ri->m_bcpos[ri->getGSeqPos(j+1)];

                  int hdelta = nextpeakposition - peakposition;
                  double hscale = ((double)(basewidth))/hdelta;
                  double vscale = 25;

                  int hoffset = tilehoffset + (j-grangeStart)*basewidth+m_fontsize/2;
                        
                  for (int t = peakposition; t < nextpeakposition; t++)
                  {
                    if ((t % 5 == 0) && channel == 0)
                    {
                      QPoint current = p.pos();
                      p.setPen(Qt::black);

                      p.drawLine(hoffset + (t-peakposition) * hscale, baseline-1,
                                 hoffset + (t-peakposition) * hscale, baseline+1);

                      p.moveTo(current);
                      p.setPen(pen);
                    }

                    int tval = trace[t]/vscale;
                    int hval = hoffset + (t-peakposition)*hscale; 

                    if (first)
                    {
                      p.moveTo(hval, baseline - tval);
                      first = false;
                    }

                    p.lineTo(hval, baseline-tval);
                  }
                }
              }
            }
            else
            {
              for (int channel = 0; channel < 4; channel++)
              {
                unsigned short * trace = NULL;
                switch (channel)
                {
                  case 0: trace = ri->m_trace->traceA; UIElements::setBasePen(pen, 'T'); break;
                  case 1: trace = ri->m_trace->traceC; UIElements::setBasePen(pen, 'G'); break;
                  case 2: trace = ri->m_trace->traceG; UIElements::setBasePen(pen, 'C'); break;
                  case 3: trace = ri->m_trace->traceT; UIElements::setBasePen(pen, 'A'); break;
                };

                p.setPen(pen);

                bool first = true;

                for (int j = grangeStart-1; j <= grangeEnd+1; j++)
                {
                  int peakposition = ri->m_bcpos[ri->getGSeqPos(j)];
                  int nextpeakposition = ri->m_bcpos[ri->getGSeqPos(j+1)];

                  int hdelta = peakposition - nextpeakposition;
                  double hscale = ((double)(basewidth))/hdelta;
                  double vscale = 25;

                  int hoffset = tilehoffset + (j-grangeStart)*basewidth+m_fontsize/2;
                        
                  for (int t = peakposition; t > nextpeakposition; t--)
                  {
                    if ((t % 5 == 0) && channel == 0)
                    {
                      QPoint current = p.pos();
                      p.setPen(Qt::black);

                      p.drawLine(hoffset + (t-peakposition) * hscale, baseline-1,
                                 hoffset + (t-peakposition) * hscale, baseline+1);

                      p.moveTo(current);
                      p.setPen(pen);
                    }

                    int tval = trace[t]/vscale;
                    int hval = hoffset + (peakposition-t)*hscale;

                    if (first)
                    {
                      p.moveTo(hval, baseline - tval);
                      first = false;
                    }

                    p.lineTo(hval, baseline-tval);
                  }
                }
              }
            }
          }

          p.setPen(black);
          p.drawLine(tilehoffset, baseline, m_width, baseline);
        }
      }
      else
      {
        // just the seqname
        ldcov += lineheight;
      }

      ri->m_displayend = ldcov;

      dcov++;
      m_currentReads.push_back(ri);
    }
  }

  if (m_highlightdiscrepancy)
  {
    for (int j = grangeStart; j <= grangeEnd; j++)
    {
      int q = j-grangeStart;
      if (m_cstatus[j] == 'X')
      {
        p.setBrush(Qt::NoBrush);
        p.setPen(UIElements::color_discrepancy);
        p.drawRect(tilehoffset + q*basewidth, 0,
                   m_fontsize, ldcov);
      }
    }
  }

  p.end();
  p.begin(this);

  pix.resize(m_width, ldcov);
  p.drawPixmap(0, 0, pix);
  resize(m_width, ldcov);
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

void TilingField::toggleHighlightDiscrepancy(bool show)
{
  m_highlightdiscrepancy = show;
  repaint();
}

void TilingField::toggleDisplayQV(bool show)
{
  m_displayqv = show;
  repaint();
}
