#include "TilingField.hh"
#include <qpainter.h>
#include <qpixmap.h>
#include "ReadInfo.hh"
#include <qstring.h>
#include <qpointarray.h>
#include "UIElements.hh"

#define DEBUG 0

static int min (int a, int b)
{
  return a < b ? a : b;
}
static int max (int a, int b)
{
  return a > b ? a : b;
}

using namespace AMOS;
using namespace std;

TilingField::TilingField(DataStore * datastore,
                         vector<RenderSeq_t> & renderedSeqs,
                         const string & consensus,
                         const string & cstatus,
                         int & gindex,
                         int & fontsize,
                         QWidget *parent, const char *name )
        : QWidget( parent, name ),
          m_fontsize(fontsize),
          m_gindex(gindex),
          m_consensus(consensus),
          m_cstatus(cstatus),
          m_renderedSeqs(renderedSeqs)
{
  m_datastore = datastore;
  m_width=600;
  m_height=0;
  m_stabletiling = 0;
  m_highlightdiscrepancy = 0;
  m_tracespace = 50;
  m_traceheight = m_tracespace - 10;
  m_displayqv = 0;
  m_lowquallower = false;
  m_fullseq = false;
  m_snpcoloring = true;
  m_basecolors = false;

  m_clickTimer = new QTimer(this, 0);
  connect (m_clickTimer, SIGNAL(timeout()),
           this, SLOT(singleClick()));

  m_clickstate = 0;

  setMinimumSize(m_width, m_height);
  setPalette(QPalette(UIElements::color_tiling));
}

int TilingField::getReadCov(int y)
{
  int dcov;
  vector<RenderSeq_t>::iterator ri;

  for (ri =  m_renderedSeqs.begin(), dcov = 0;
       ri != m_renderedSeqs.end();
       ri++, dcov++)
  {
    if (y >= ri->m_displaystart && y < ri->m_displayend)
    {
      #if DEBUG
      cerr << "Hit " << ri->m_read.getEID() << " [" << dcov << "]" << endl;
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

  m_renderedSeqs[dcov].m_displayTrace = !m_renderedSeqs[dcov].m_displayTrace;

  if (m_renderedSeqs[dcov].m_displayTrace)
  {
    m_renderedSeqs[dcov].loadTrace(m_datastore);
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

  m_renderedSeqs[dcov].loadTrace(m_datastore);
  ReadInfo * readinfo = new ReadInfo(&m_renderedSeqs[dcov], 
                                     m_datastore->m_db, 
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
  int gutter         = m_fontsize;
  int lineheight     = m_fontsize+gutter;
  int tilehoffset    = m_fontsize*12;
  int seqnamehoffset = gutter;
  int rchoffset      = m_fontsize*11;
  int basewidth      = m_fontsize+basespace;

  double tracevscale = 1500.0 / m_traceheight;
  int displaywidth = (m_width-tilehoffset)/basewidth;

  Pos_t grangeStart = m_gindex;
  Pos_t grangeEnd = min(m_gindex + displaywidth, m_consensus.size()-1);



  // Compute the exact height we want
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

    if (m_fullseq)
    {
      hasOverlap = RenderSeq_t::hasOverlap(grangeStart, grangeEnd, 
                                           ri->m_lfoffset, ri->fullLen(),
                                           clen);
    }

    if (hasOverlap || m_stabletiling)
    {
      ri->m_displaystart = ldcov;

      int readheight = lineheight; // seqname
      if (m_displayqv)        { readheight += lineheight; }
      if (ri->m_displayTrace) { readheight += m_tracespace; }

      ldcov += readheight;
      ri->m_displayend = ldcov;
    }
    else
    {
      ri->m_displaystart = -1;
      ri->m_displayend = -1;
    }
  }

  // +1 takes care of 0dcov regions
  int height = ldcov + 1;
  QString s;

  QPixmap pix(m_width, height);
  pix.fill(this, 0,0);

  QPainter p(&pix);
  QPen pen;
  pen.setColor(black);
  p.setPen(pen);
  p.setFont(QFont("Helvetica", m_fontsize));
  p.setBrush(Qt::SolidPattern);

  QPointArray rcflag(3);
  int tridim = m_fontsize/2;
  int trioffset = m_fontsize/2;

  emit setTilingVisibleRange(grangeStart, grangeEnd);

  #if DEBUG
  cerr << "paintTField:" << m_renderedSeqs.size()
       << " [" << grangeStart << "," << grangeEnd << "]" << endl;
  #endif

  for (ri =  m_renderedSeqs.begin();
       ri != m_renderedSeqs.end(); 
       ri++)
  {
    if (ri->m_displaystart != -1)
    {
      int hasOverlap = RenderSeq_t::hasOverlap(grangeStart, grangeEnd, 
                                               ri->m_loffset, ri->gappedLen(),
                                               clen);

      if (m_fullseq)
      {
        hasOverlap = RenderSeq_t::hasOverlap(grangeStart, grangeEnd, 
                                             ri->m_lfoffset, ri->fullLen(),
                                             clen);
      }

      ldcov = ri->m_displaystart;

      int readheight = lineheight; // seqname
      if (m_displayqv)        { readheight += lineheight; }
      if (ri->m_displayTrace) { readheight += m_tracespace; }


      QColor bgcolor;
      if (m_snpcoloring)
      {
        bgcolor = UIElements::getSNPColoring(ri->bgcolor);
      }
      else
      {
        bgcolor = UIElements::color_tilingoffset;
      }
    
      // background rectangle
      if (dcov % 2)
      {
        bgcolor = bgcolor.light(150);
      }
      else
      {
        bgcolor = bgcolor.light(175);
      }

      p.setPen(bgcolor);
      p.setBrush(bgcolor);

      p.drawRect(0, ldcov, m_width, readheight);
      dcov++;

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
      p.drawText(seqnamehoffset, ldcov,
                 rchoffset-seqnamehoffset, lineheight,
                 Qt::AlignLeft | Qt::AlignBottom, 
                 QString(ri->m_read.getEID().c_str()));

      if (hasOverlap)
      {

        if (m_fullseq)
        {
          p.setPen(UIElements::color_tilingtrim);
          p.setBrush(UIElements::color_tilingtrim);

          if (grangeStart < ri->m_loffset)
          {
            int start = (ri->m_lfoffset - grangeStart) * basewidth;
            if (grangeStart > ri->m_lfoffset)
            {
              start = 0;
            }

            p.drawRect(tilehoffset + start, ldcov + 2, 
                       (ri->m_loffset - grangeStart) * basewidth, readheight-4);
          }

          if (grangeEnd > ri->m_roffset)
          {
            int start = (ri->m_roffset - grangeStart + 1) * basewidth;

            if (grangeStart > ri->m_roffset)
            {
              start = 0;
            }

            int rightdistance = (min(grangeEnd, ri->m_rfoffset) - max(ri->m_roffset+1, grangeStart) + 1 ) * basewidth;

            p.drawRect(tilehoffset + start, ldcov + 2, 
                       rightdistance,readheight - 4);
          }
        }


        for (int gindex = grangeStart, alignedPos = 0;
             gindex <= grangeEnd; 
             gindex++, alignedPos++)
        {
          int hoffset = tilehoffset + (gindex-grangeStart)*basewidth;

          int qv = ri->qv(gindex, m_fullseq);
          char b = ri->base(gindex, m_fullseq);

          if (qv < 30 && m_lowquallower) { b = tolower(b); }
          else                           { b = toupper(b); }

          s = b;

          if (m_highlightdiscrepancy && b != ' ' && toupper(b) != toupper(m_consensus[gindex]))
          {
            p.setBrush(UIElements::color_discrepancy);
            p.setPen(UIElements::color_discrepancy);
            p.drawRect(hoffset, ldcov, m_fontsize, lineheight);
          }

          // Bases
          p.setPen((m_basecolors) ? UIElements::getBaseColor(b) : black);
          p.setFont(QFont("Helvetica", m_fontsize));
          p.drawText(hoffset, ldcov, 
                     m_fontsize, lineheight,
                     Qt::AlignHCenter | Qt::AlignBottom, s);

          // QV
          if (m_displayqv && b != ' ')
          {
            if (qv != -1)
            {
              s = QString::number(qv);

              p.setFont(QFont("Helvetica", (int)(m_fontsize*.75)));
              p.setPen(black);
              p.drawText(hoffset, ldcov+lineheight,
                         m_fontsize, 2*m_fontsize,
                         Qt::AlignHCenter | Qt::AlignVCenter, s);
            }
          }
        }

        ldcov += lineheight;
        if (m_displayqv) { ldcov += lineheight; }

        if (ri->m_displayTrace) 
        { 
          int baseline = ldcov + m_traceheight;
          
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

              int leftboundary = ri->m_loffset;
              int rightboundary = ri->m_roffset;

              if (m_fullseq)
              {
                leftboundary = ri->m_lfoffset;
                rightboundary = ri->m_rfoffset;
              }

              // go beyond the range so the entire peak will be drawn
              for (int gindex =  max(leftboundary, grangeStart-1); 
                       gindex <= min(rightboundary, grangeEnd+1); 
                       gindex++)
              {
                int hoffset = tilehoffset + (gindex-grangeStart)*basewidth+m_fontsize/2;

                int peakposition     = ri->pos(gindex, m_fullseq);
                int nextpeakposition = ri->pos(gindex+1, m_fullseq);

                // in 1 basewidth worth of pixels, cover hdelta worth of trace
                int    hdelta = nextpeakposition - peakposition;
                double hscale = ((double)(basewidth))/hdelta; // rc negative
                      
                int tpos = peakposition;
                while ((!ri->m_rc && tpos < nextpeakposition) ||
                       ( ri->m_rc && tpos > nextpeakposition))
                {
                  int hval = hoffset + (int)((tpos-peakposition)*hscale); // rc negative
                  int tval = (int)(trace[tpos]/tracevscale);

                  // truncate very tall peaks
                  if (tval > m_traceheight) { tval = m_traceheight; }

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
        }
      }
    }
  }

  p.end();

  p.begin(this);
  p.drawPixmap(0, 0, pix);
  p.end();

  if (m_width != this->width() || height != this->height())
  {
    resize(m_width, height);
  }
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

void TilingField::toggleBaseColors(bool showColors)
{
  m_basecolors = showColors;
  repaint();
}

void TilingField::toggleShowFullRange(bool showFull)
{
  m_fullseq = showFull;
  repaint();
}

void TilingField::toggleDisplayQV(bool show)
{
  m_displayqv = show;
  repaint();
}

void TilingField::toggleLowQualityLowerCase(bool dolower)
{
  m_lowquallower = dolower;
  repaint();
}

void TilingField::toggleSNPColoring(bool doColor)
{
  m_snpcoloring = doColor;
  repaint();
}
