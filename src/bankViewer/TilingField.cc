#include "TilingField.hh"
#include <qpainter.h>
#include <qpixmap.h>
#include "ReadInfo.hh"
#include <qstring.h>
#include <qpointarray.h>
#include "UIElements.hh"

#define DEBUG 0

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
      #if DEBUG
      cerr << "Hit " << (*i)->m_read.getEID() << " [" << dcov << "]" << endl;
      #endif

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
  #if DEBUG
  cerr << "Timer fired! single click" << endl;
  #endif

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
  #if DEBUG
  cerr << "mouserelease state:" << m_clickstate << endl;
  #endif

  m_clickTimer->start(400, true);
  m_yclick = e->y();
}


void TilingField::mouseDoubleClickEvent( QMouseEvent *e )
{
  #if DEBUG
  cerr << "doubleclick, stop timer" << endl;
  #endif

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

  connect(this, SIGNAL(setTilingVisibleRange(int, int)),
          readinfo, SLOT(setTilingVisibleRange(int, int)));
}



void TilingField::paintEvent( QPaintEvent * )
{
  if (m_renderedSeqs.empty()) { resize(m_width, m_height); return; }

  int basespace      = 5;
  int gutter         = m_fontsize/2;
  int lineheight     = m_fontsize+gutter;
  int tilehoffset    = m_fontsize*12;
  int seqnamehoffset = gutter;
  int rchoffset      = m_fontsize*11;
  int basewidth      = m_fontsize+basespace;

  double tracevscale = 1500.0 / (m_traceheight - 10);

  int displaywidth = (m_width-tilehoffset)/basewidth;

  int height = 10000; // max height

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

  emit setTilingVisibleRange(grangeStart, grangeEnd);

  #if DEBUG
  cerr << "paintTField:" << m_renderedSeqs.size()
       << " [" << grangeStart << "," << grangeEnd << "]" << endl;
  #endif

  vector<RenderSeq_t>::iterator ri;
  int dcov = 0;
  int ldcov = 0;
  int clen = m_consensus.size();

  for (ri =  m_renderedSeqs.begin();
       ri != m_renderedSeqs.end(); 
       ri++)
  {
    int hasOverlap = RenderSeq_t::hasOverlap(grangeStart, grangeEnd, 
                                             ri->m_loffset, ri->gappedLen(),
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
        p.setPen(offsetColor);
        p.setBrush(offsetColor);
        p.drawRect(0, ldcov, m_width, readheight);
      }

      // black pen
      p.setPen(black);
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
        for (int gindex = grangeStart; gindex <= grangeEnd; gindex++)
        {
          int hoffset = tilehoffset + (gindex-grangeStart)*basewidth;

          char b = ri->base(gindex);
          s = b;

          if (m_highlightdiscrepancy && b != ' ' && b != m_consensus[gindex])
          {
            p.setBrush(UIElements::color_discrepancy);
            p.setPen(UIElements::color_discrepancy);
            p.drawRect(hoffset, ldcov, m_fontsize, lineheight);
          }

          // Bases
          p.setPen(UIElements::getBaseColor(b));
          p.setFont(QFont("Helvetica", m_fontsize));
          p.drawText(hoffset, ldcov, 
                     m_fontsize, lineheight,
                     Qt::AlignHCenter | Qt::AlignBottom, s);

          // QV
          if (m_displayqv && b != ' ')
          {
            int qv = ri->qv(gindex);
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
          
          if (ri->m_trace)
          {
            unsigned short * trace = NULL;

            for (int channel = 0; channel < 4; channel++)
            {
              if (!ri->m_rc)
              {
                switch (channel)
                {
                  case 0: trace = ri->m_trace->traceA; UIElements::setBasePen(pen, 'A'); break;
                  case 1: trace = ri->m_trace->traceC; UIElements::setBasePen(pen, 'C'); break;
                  case 2: trace = ri->m_trace->traceG; UIElements::setBasePen(pen, 'G'); break;
                  case 3: trace = ri->m_trace->traceT; UIElements::setBasePen(pen, 'T'); break;
                };
              }
              else
              {
                switch (channel)
                {
                  case 0: trace = ri->m_trace->traceA; UIElements::setBasePen(pen, 'T'); break;
                  case 1: trace = ri->m_trace->traceC; UIElements::setBasePen(pen, 'G'); break;
                  case 2: trace = ri->m_trace->traceG; UIElements::setBasePen(pen, 'C'); break;
                  case 3: trace = ri->m_trace->traceT; UIElements::setBasePen(pen, 'A'); break;
                };
              }

              p.setPen(pen);

              bool first = true;

              // go beyond the range so the entire peak will be drawn
              for (int gindex = grangeStart-1; gindex <= grangeEnd+1; gindex++)
              {
                int hoffset = tilehoffset + (gindex-grangeStart)*basewidth+m_fontsize/2;

                int peakposition     = ri->m_bcpos[ri->getGSeqPos(gindex)];
                int nextpeakposition = ri->m_bcpos[ri->getGSeqPos(gindex+1)];

                // in 1 basewidth worth of pixels, cover hdelta worth of trace
                int    hdelta = nextpeakposition - peakposition;
                double hscale = ((double)(basewidth))/hdelta; // rc negative
                      
                int tpos = peakposition;
                while ((!ri->m_rc && tpos < nextpeakposition) ||
                       ( ri->m_rc && tpos > nextpeakposition))
                {
                  int hval = hoffset + (int)((tpos-peakposition)*hscale); // rc negative
                  int tval = (int)(trace[tpos]/tracevscale);

                  if (tval > m_traceheight - 10)
                  {
                    // truncate very tall peaks
                    tval = m_traceheight - 10;
                  }

                  if (first)
                  {
                    p.moveTo(hval, baseline - tval);
                    first = false;
                  }

                  // draw trace
                  p.lineTo(hval, baseline-tval);

                  // ticks
                  if ((tpos % 5 == 0) && channel == 0)
                  {
                    QPoint current = p.pos();
                    p.setPen(Qt::black);

                    p.drawLine(hval, baseline-1, hval, baseline+1);

                    p.moveTo(current);
                    p.setPen(pen);
                  }

                  if (ri->m_rc) { tpos--; }
                  else          { tpos++; }
                }
              }
            }
          }

          p.setPen(black);
          p.drawLine(tilehoffset-basewidth, baseline, m_width, baseline);
          ldcov += m_traceheight;
        }
      }
      else
      {
        // just the seqname
        ldcov += lineheight;
      }

      ri->m_displayend = ldcov;

      dcov++;
      m_currentReads.push_back(&(*ri));
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
