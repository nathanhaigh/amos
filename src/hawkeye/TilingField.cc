#include "TilingField.hh"
#include <qpainter.h>
#include <qpixmap.h>
#include <q3pointarray.h>
#include <qstring.h>
#include <qapplication.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPaintEvent>
#include <QtGui>

#include "ReadInfo.hh"
#include "UIElements.hh"
#include "DataStore.hh"
#include "AlignmentInfo.hh"

#define DEBUGQV 0

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

extern "C"
{
#include <Read.h>
}

static int m_minheight = 8;

TilingField::TilingField(DataStore * datastore,
                         vector<RenderSeq_t> & renderedSeqs,
                         const string & consensus,
                         const string & cstatus,
                         AlignmentInfo * ai,
                         int & gindex,
                         int & fontsize,
                         QWidget *parent, const char *name )
        : QWidget( parent, name ),
          m_consensus(consensus),
          m_cstatus(cstatus),
          m_alignment(ai),
          m_renderedSeqs(renderedSeqs),
          m_fontsize(fontsize),
          m_gindex(gindex)
{
  m_datastore = datastore;
  m_readnamewidth = 11;
  m_width=100;
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
  m_polymorphismView = false;
  m_qvcoloring = false;
  m_doubleclick = false;
  m_packreads = false;

  m_clickTimer = new QTimer(this, 0);
  connect (m_clickTimer, SIGNAL(timeout()),
           this, SLOT(singleClick()));

  m_clickstate = 0;

  setMinimumSize(m_width, m_height);

  m_width=QApplication::desktop()->width();
  resize(m_width, 100);
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
      #if DEBUGQV
      cerr << "Hit " << ri->m_read.getEID() << " [" << dcov << "]" << endl;
      #endif

      return dcov;
    }
  }

  return -1;
}

void TilingField::singleClick()
{
  #if DEBUGQV
  cerr << "Timer fired! single click" << endl;
  #endif

  int dcov = getReadCov(m_yclick);
  if (dcov == -1) { return; }

  m_renderedSeqs[dcov].m_displayTrace = !m_renderedSeqs[dcov].m_displayTrace;

  if (m_renderedSeqs[dcov].m_displayTrace)
  {
    m_renderedSeqs[dcov].loadTrace();
  }

  update();
}

void TilingField::mouseReleaseEvent( QMouseEvent * e)
{
  #if DEBUGQV
  cerr << "mouserelease state:" << m_clickstate << endl;
  #endif

  int theight         = max(m_fontsize, m_minheight);
  int tilehoffset    = theight*12;

  if (e->x() < tilehoffset)
  {
    int dcov = getReadCov(e->y());
    emit highlightRead(m_renderedSeqs[dcov].m_read.getIID());
  }
  else if (m_doubleclick)
  {
    // Eat the release from the doubleclick
    m_doubleclick = false;
  }
  else
  {
    m_clickTimer->start(400, true);
    m_yclick = e->y();
  }
}


void TilingField::mouseDoubleClickEvent( QMouseEvent *e )
{
  #if DEBUGQV
  cerr << "doubleclick, stop timer" << endl;
  #endif

  m_clickTimer->stop();
  m_doubleclick = true;

  int dcov = getReadCov(e->y());
  if (dcov == -1) { return; }

  m_renderedSeqs[dcov].loadTrace();
  ReadInfo * readinfo = new ReadInfo(&m_renderedSeqs[dcov], 
                                     m_consensus,
                                     m_cstatus,
                                     this, 
                                     "readinfo");
  readinfo->show();

  connect(this, SIGNAL(setTilingVisibleRange(int, int, int)),
          readinfo, SLOT(setTilingVisibleRange(int, int, int)));
}



void TilingField::paintEvent( QPaintEvent * paintevent )
{
  if (m_renderedSeqs.empty()) { resize(m_width, m_height); return; }

  int theight         = max(m_fontsize, m_minheight);

  int basespace      = 5;
  int gutter         = theight;
  int lineheight     = theight+gutter;
  int tilehoffset    = theight*(m_readnamewidth+1);
  int seqnamehoffset = gutter;
  int rchoffset      = theight*m_readnamewidth;
  double basewidth   = m_fontsize+basespace;

  if (m_packreads) { tilehoffset = 2; }

  int  packgutter = m_readnamewidth; // separate reads on same row by this much

  if (basewidth <= 0)
  {
    basewidth = 1/((-basewidth+2));
  }

  int bwidth = max(basewidth, 1);

  double tracevscale = 1500.0 / m_traceheight;
  int displaywidth = (int) ((m_width-tilehoffset)/basewidth);

  Pos_t grangeStart = m_gindex;
  Pos_t grangeEnd = min(m_gindex + displaywidth, m_consensus.size()-1);

  Pos_t srangeStart = m_alignment->getContigPos(grangeStart);
  Pos_t srangeEnd = m_alignment->getContigPos(grangeEnd);

  // Compute the exact height we want
  vector<RenderSeq_t>::iterator ri;
  int dcov = 0;
  int ldcov = 0;
  int clen = m_consensus.size();

  vector<int> rows;

  int rowheight = lineheight;
  if (m_displayqv) rowheight += lineheight;

  for (ri =  m_renderedSeqs.begin();
       ri != m_renderedSeqs.end(); 
       ri++)
  {
    int hasOverlap = RenderSeq_t::hasOverlap(srangeStart, srangeEnd, 
                                             ri->m_loffset, ri->gappedLen(),
                                             clen);

    if (m_fullseq)
    {
      hasOverlap = RenderSeq_t::hasOverlap(srangeStart, srangeEnd, 
                                           ri->m_lfoffset, ri->fullLen(),
                                           clen);
    }

    if (hasOverlap || m_stabletiling)
    {

      if (m_packreads)
      {
        int r;

        for (r = 0; r < (int) rows.size(); r++)
        {
          if (ri->m_loffset > rows[r])
          {
            break;
          }
        }

        if (r >= (int) rows.size())
        {
          rows.push_back(0);
        }

        rows[r] = ri->m_loffset+ri->gappedLen()+packgutter;

        ri->m_displaystart = r*rowheight;
        ri->m_displayend   = (r+1)*rowheight;
      }
      else
      {
        ri->m_displaystart = ldcov;

        int readheight = lineheight; // seqname
        if (m_displayqv)        { readheight += lineheight; }
        if (ri->m_displayTrace) { readheight += m_tracespace; }

        dcov++;
        ldcov += readheight;
        ri->m_displayend = ldcov;
      }
    }
    else
    {
      ri->m_displaystart = -1;
      ri->m_displayend = -1;
    }
  }

  if (m_packreads)
  {
    ldcov = rows.size() * lineheight;
  }

  // +1 takes care of 0dcov regions
  int height = ldcov + 1;
  if (height > 32000) { height = 32000; }

  if (m_width != this->width() || height != this->height())
  {
    //cerr << "Resize: " << m_width << "x" << height << endl;
    resize(m_width, height);
    update();
  }

  int drawtop = paintevent->rect().top();
  int drawbottom = paintevent->rect().bottom();

  QString s;

  QPainter p(this);

  QPen pen;
  pen.setColor(Qt::black);
  p.setPen(pen);
  p.setFont(QFont("Helvetica", theight));
  p.setBrush(Qt::SolidPattern);

  Q3PointArray rcflag(3);
  int tridim = theight/2;
  int trioffset = lineheight-.75*theight-2;


  #if 0
  cerr << "paintTField:" << m_renderedSeqs.size() << " dcov: " << dcov << " height: " << height
       << " [" << srangeStart << "," << srangeEnd << "]" << endl;
  cerr << "Draw top: " << drawtop << " bottom: " << drawbottom << endl;
  #endif

  if (m_packreads)
  {
    for (int r = 0; r < rows.size(); r++)
    {
      QColor bgcolor = UIElements::color_tilingoffset;
    
      // background rectangle
      if (r % 2) { bgcolor = bgcolor.light(175); }
      else          { bgcolor = bgcolor.light(150); }

      p.setPen(bgcolor);
      p.setBrush(bgcolor);

      p.drawRect(0, r*lineheight, m_width, lineheight);
    }
  }

  dcov = 0;

  for (ri =  m_renderedSeqs.begin();
       ri != m_renderedSeqs.end(); 
       ri++)
  {
    if (ri->m_displaystart != -1)
    {
      int hasOverlap = RenderSeq_t::hasOverlap(srangeStart, srangeEnd, 
                                               ri->m_loffset, ri->gappedLen(),
                                               clen);

      if (m_fullseq)
      {
        hasOverlap = RenderSeq_t::hasOverlap(srangeStart, srangeEnd, 
                                             ri->m_lfoffset, ri->fullLen(),
                                             clen);
      }

      if (!hasOverlap)
      {
        cerr << "wtf?" << endl;
      }

      ldcov = ri->m_displaystart;
      dcov++;

      int readheight = lineheight; // seqname
      if (m_displayqv)        { readheight += lineheight; }
      if (ri->m_displayTrace) { readheight += m_tracespace; }

      if ((ldcov > height) || (ldcov > drawbottom) || ((ldcov+readheight) < drawtop))
      {
        continue;
      }

      if (!m_packreads)
      {
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
        if (dcov % 2) { bgcolor = bgcolor.light(175); }
        else          { bgcolor = bgcolor.light(150); }

        p.setPen(bgcolor);
        p.setBrush(bgcolor);

        p.drawRect(0, ldcov, m_width, readheight);

        // black pen
        p.setPen(QPen(Qt::black));
        p.setBrush(QBrush(Qt::black));
        p.setFont(QFont("Helvetica", theight));

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
        int PrintIID = 0;
        if (PrintIID)
        {
          p.drawText(seqnamehoffset, ldcov,
                     rchoffset-seqnamehoffset, lineheight-2,
                     Qt::AlignLeft | Qt::AlignBottom, 
                     QString::number(ri->m_read.getIID()));
        }
        else
        {
          p.drawText(seqnamehoffset, ldcov,
                     rchoffset-seqnamehoffset, lineheight-2,
                     Qt::AlignLeft | Qt::AlignBottom, 
                     QString(ri->m_read.getEID().c_str()));
        }
      }

      if (hasOverlap)
      {

        if (m_fullseq)
        {
          p.setPen(UIElements::color_tilingtrim);
          p.setBrush(UIElements::color_tilingtrim);

          if (srangeStart < ri->m_loffset)
          {
            int left  = max(srangeStart, ri->m_lfoffset);
            int right = min(ri->m_loffset-1, srangeEnd);

            int start = (int) ((left - srangeStart) * basewidth);

            p.drawRect(tilehoffset + start, ldcov + 2, 
                       (int)((right - left + 1) * basewidth), readheight - 4);
          }

          if (srangeEnd > ri->m_roffset)
          {
            int left = max(srangeStart, ri->m_roffset+1);
            int right = min(ri->m_rfoffset, srangeEnd);

            int start = (int)((left - srangeStart) * basewidth);

            p.drawRect(tilehoffset + start, ldcov + 2, 
                       (int)((right - left + 1 ) * basewidth), readheight - 4);
          }
        }

        if (m_fontsize < m_minheight)
        {
          p.setPen(Qt::black);
          p.setBrush(QColor(200,200,200));

          int left = max(srangeStart, ri->m_loffset);
          int right = min(srangeEnd,  ri->m_roffset);

          int start = (int)((left - srangeStart) * basewidth);

          p.drawRect(tilehoffset + start-1, ldcov + 2, 
                    (int)((right - left + 1 ) * basewidth+2), readheight - 4);

        }

        for (int gindex = grangeStart, alignedPos = 0;
             gindex <= grangeEnd; 
             gindex++, alignedPos++)
        {
          int hoffset = (int)(tilehoffset + (gindex-grangeStart)*basewidth);

          int shifted = m_alignment->getContigPos(gindex);

          if (m_consensus[gindex] == '*')
          {
            continue;
          }

          int qv = ri->qv(shifted, m_fullseq, m_consensus.length());
          char b = ri->base(shifted, m_fullseq, m_consensus.length());

          if (qv < 30 && m_lowquallower) { b = tolower(b); }
          else                           { b = toupper(b); }

          s = b;

          {
            bool bad = m_highlightdiscrepancy && (toupper(b) != toupper(m_consensus[gindex]));

            if ((bad || m_qvcoloring) && b != ' ')
            {
              QColor bg;

              if (bad && !m_qvcoloring)
              {
                bg = UIElements::color_discrepancy;
              }
              else
              {
                int h = 0, s = 0, v = 60 + qv*3;

                if (bad && m_qvcoloring)
                {
                  h = 300; s = 220 ; v = (int)(160 + 1.5*qv);
                }

                bg.setHsv(h,s,v);
              }

              p.setBrush(bg);
              p.setPen(bg);
              if (m_fontsize >= m_minheight)
              {
                p.drawRect(hoffset-basespace/2, ldcov+3, bwidth, lineheight-6);
              }
              else
              {
                int baseheight = lineheight - 6;
                if (m_displayqv) { baseheight = lineheight-3; }
                p.drawRect(hoffset, ldcov+3, bwidth, baseheight);
              }
            }
          }

          // Bases
          p.setPen((m_basecolors) ? UIElements::getBaseColor(b) : QPen(Qt::black));
          p.setFont(QFont("Helvetica", m_fontsize));

          if (m_fontsize < m_minheight)
          {
            if ((b != ' ') && ((m_polymorphismView) || (toupper(b) != toupper(m_consensus[gindex]))))
            {
              int baseheight = lineheight-6;
              if (m_displayqv) { baseheight = lineheight-3; }

              p.setBrush((m_basecolors) ? QBrush(UIElements::getBaseColor(b)) : QBrush(Qt::black));
              p.drawRect(hoffset, ldcov+3, bwidth, baseheight);
            }
          }
          else
          {
            if (m_polymorphismView && toupper(b) == toupper(m_consensus[gindex]))
            {
              s = '.';
            }

            p.drawText(hoffset, ldcov, 
                       m_fontsize, lineheight-2,
                       Qt::AlignHCenter | Qt::AlignBottom, s);
          }

          // QV
          if (m_displayqv && b != ' ' && qv != -1)
          {
            if (m_fontsize < m_minheight)
            {
              QColor qvcolor;
              int h = 0, s = 0, v = 60 + qv*3;
              qvcolor.setHsv(h,s,v);
              p.setBrush(qvcolor);
              p.setPen(qvcolor);

              p.drawRect(hoffset, ldcov+lineheight, bwidth, lineheight-3);
            }
            else
            {
              s = QString::number(qv);

              p.setFont(QFont("Helvetica", (int)(m_fontsize*.75)));
              p.setPen(QPen(Qt::black));
              p.drawText(hoffset-m_fontsize, ldcov+lineheight,
                         3*m_fontsize, m_fontsize,
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
            Read * chromotrace = (Read *) ri->m_trace;

            for (int channel = 0; channel < 4; channel++)
            {
              if (!ri->m_rc)
              {
                switch (channel)
                {
                  case 0: trace = chromotrace->traceA; UIElements::setBasePen(pen, 'A'); break;
                  case 1: trace = chromotrace->traceC; UIElements::setBasePen(pen, 'C'); break;
                  case 2: trace = chromotrace->traceG; UIElements::setBasePen(pen, 'G'); break;
                  case 3: trace = chromotrace->traceT; UIElements::setBasePen(pen, 'T'); break;
                };
              }
              else
              {
                switch (channel)
                {
                  case 0: trace = chromotrace->traceA; UIElements::setBasePen(pen, 'T'); break;
                  case 1: trace = chromotrace->traceC; UIElements::setBasePen(pen, 'G'); break;
                  case 2: trace = chromotrace->traceG; UIElements::setBasePen(pen, 'C'); break;
                  case 3: trace = chromotrace->traceT; UIElements::setBasePen(pen, 'A'); break;
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

              leftboundary = m_alignment->getGlobalPos(leftboundary);
              rightboundary = m_alignment->getGlobalPos(rightboundary);

              // go beyond the range so the entire peak will be drawn
              for (int gindex =  max(leftboundary, grangeStart-1); 
                       gindex <= min(rightboundary, grangeEnd+1); 
                       gindex++)
              {
                if (m_consensus[gindex] == '*')
                {
                  continue;
                }

                int hoffset = (int)(tilehoffset + (gindex-grangeStart)*basewidth+m_fontsize/2);
                int shifted = m_alignment->getContigPos(gindex);

                int peakposition     = ri->pos(shifted, m_fullseq, m_consensus.length());
                int nextpeakposition = ri->pos(shifted+1, m_fullseq, m_consensus.length());

                int count = 1;
                while (m_consensus[gindex+count] == '*')
                {
                  count++;
                }

                QPainterPath path;

                // in 1 basewidth worth of pixels, cover hdelta worth of trace
                int    hdelta = nextpeakposition - peakposition;
                double hscale = ((double)(count*basewidth))/hdelta; // rc negative
                      
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
                    path.moveTo(hval, baseline - tval);
                    first = false;
                  }

                  // draw trace
                  path.lineTo(hval, baseline-tval);

                  // ticks
                  if ((tpos % 5 == 0) && channel == 0)
                  {
                    p.setPen(Qt::black);

                    p.drawLine(hval, baseline-1, hval, baseline+1);

                    p.setPen(pen);
                  }

                  if (ri->m_rc) { tpos--; }
                  else          { tpos++; }
                }

                p.drawPath(path);
              }
            }
          }

          p.setPen(QPen(Qt::black));
          p.drawLine((int)(tilehoffset-basewidth), baseline, m_width, baseline);
        }
      }
    }
  }
  
  emit setTilingVisibleRange(m_datastore->m_contigId, srangeStart, srangeEnd);
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

void TilingField::togglePackReads(bool pack)
{
  m_packreads = pack;
  update();
}

void TilingField::toggleHighlightDiscrepancy(bool show)
{
  m_highlightdiscrepancy = show;
  update();
}

void TilingField::toggleBaseColors(bool showColors)
{
  m_basecolors = showColors;
  update();
}

void TilingField::toggleShowFullRange(bool showFull)
{
  m_fullseq = showFull;
  update();
}

void TilingField::toggleDisplayQV(bool show)
{
  m_displayqv = show;
  update();
}

void TilingField::toggleLowQualityLowerCase(bool dolower)
{
  m_lowquallower = dolower;
  update();
}

void TilingField::toggleSNPColoring(bool doColor)
{
  m_snpcoloring = doColor;
  update();
}

void TilingField::togglePolymorphismView(bool doPV)
{
  m_polymorphismView = doPV;
  update();
}

void TilingField::toggleQVColoring(bool doqvc)
{
  m_qvcoloring = doqvc;
  update();
}

void TilingField::setReadnameWidth(int width)
{
  m_readnamewidth = width;
  update();
}

int TilingField::getReadnameWidth()
{
  return m_readnamewidth;
}
