#include "ChromoField.hh"
#include "RenderSeq.hh"
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpen.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <math.h>
#include <qstring.h>
#include <string>
#include <qpixmap.h>

#include <stdio.h>
#include "UIElements.hh"
#include <q3scrollview.h>

using namespace std;
using namespace AMOS;

extern "C"
{
#include <Read.h>
}

ChromoField::ChromoField(RenderSeq_t * read, 
                         const string & cons,
                         const string & cstatus,
                         QWidget *parent, 
                         const char *name)
  :QWidget(parent, name)
{
  setPalette(QPalette(UIElements::color_tiling));
  m_pix = NULL;
  m_hscale = 2.0;

  m_read = read;
  if (!m_read->m_trace) { return; }

  Read * m_trace = (Read *) m_read->m_trace;

  int vscale=24;
  int tickwidth = 2;
  int maxy = 2000;

  int diam          = 5;
  int fontsize      = 10;
  int gutter        = fontsize/2;
  int lineheight    = fontsize+gutter;

  int consposoffset = 5;
  int clineoffset   = consposoffset+lineheight+5;
  int discoffset    = clineoffset+5;
  int consoffset    = discoffset+fontsize;

  int seqoffset     = consoffset+lineheight;
  int qvoffset      = seqoffset+lineheight;
  int slineoffset   = qvoffset+lineheight;
  int sposoffset    = slineoffset+5;


  int baseline = 180;
  int tposoffset = baseline + 5;

  int lastTracePos = (int)(m_hscale*m_trace->NPoints);

  m_hoffset = 20;
  resize (lastTracePos+2*m_hoffset, tposoffset+lineheight+m_hoffset);

  m_pix = new QPixmap(width(), height());
  m_pix->fill(this, 0, 0);

  QPen pen;
  QPainter p(m_pix);

  p.translate(m_hoffset, 0);

  vector<int16_t> & m_pos = m_read->m_pos;
  string & bases = m_read->m_bases;
  string & quals = m_read->m_quals;
  
  // tint outside the assembled
  p.setPen(UIElements::color_tilingoffset);
  p.setBrush(UIElements::color_tilingoffset);
  
  if (m_read->m_rangebegin)
  {
    int rpos = m_pos[m_read->m_rangebegin];
    int bpos = m_pos[m_read->m_rangebegin-1];

    int width = (int)(((rpos+bpos)/2)*m_hscale);
    if (m_read->m_rc) { width = lastTracePos - width; }

    p.drawRect(-5, 2, width+5, height() -4);
  }

  if (m_read->m_rangeend != (int) bases.length()-1)
  {
    int rpos = m_pos[m_read->m_rangeend];
    int bpos = m_pos[m_read->m_rangeend+1];

    int width = (int)(((rpos+bpos)/2)*m_hscale);
    if (m_read->m_rc) { width = lastTracePos - width; }

    p.drawRect(width-2, 2,
               lastTracePos-width+3, height()-4);
  }

  // Draw the bases and consensus
  int startgindex = -1;
  int endgindex = -1;

  QString s;

  int i;
  for (i = 0; i < (int) m_pos.size(); i++)
  {
    int gseqpos = i;
    if (m_read->m_rc) { gseqpos = bases.size() - i -1; }

    char b  = bases[i]; s = b;
    char qv = quals[i] - AMOS::MIN_QUALITY;

    int hpos = (int)(m_hscale*m_pos[i]);
    if (m_read->m_rc) { hpos = lastTracePos - hpos; }

    // base
    p.setPen(UIElements::getBaseColor(b));
    p.setFont(QFont("Helvetica", fontsize));
    p.drawText(hpos-20, seqoffset,
               40, 20, Qt::AlignHCenter,s);

    // qv
    p.setFont(QFont("Helvetica", 8));
    p.drawText(hpos-20, qvoffset,
               40, 20, Qt::AlignHCenter,QString::number(qv));


    // gseqpos
    p.setPen(Qt::black);
    if (b != '-' && (gseqpos % 5 == 0))
    {
      p.setFont(QFont("Helvetica", 8));
      p.drawText(hpos-20, sposoffset,
                 40, 20, Qt::AlignHCenter, QString::number(gseqpos));

      p.drawLine(hpos, slineoffset-2,
                 hpos, slineoffset+2);
    }


    int gindex = m_read->getGindex(gseqpos);
    if (gindex >= 0 && gindex < (int) cons.length())
    {
      if (startgindex == -1) { startgindex = hpos-fontsize; }
      else                   { endgindex   = hpos+fontsize; }

      char c = cons[gindex]; s=c;

      // consensus
      p.setFont(QFont("Helvetica", fontsize));
      p.setPen(UIElements::getBaseColor(c));
      p.drawText(hpos-20, consoffset,
                 40, 20, Qt::AlignHCenter,s);

      p.setPen(Qt::black);

      // consensus dot
      if (cstatus[gindex] == 'X')
      {
        p.setBrush(Qt::SolidPattern);
        p.setBrush(Qt::black);
        p.drawEllipse(hpos-diam/2-1, discoffset, diam, diam);
      }

      // consensus position
      if (gindex % 5 == 0)
      {
        p.setFont(QFont("Helvetica", 8));
        p.drawText(hpos-20, consposoffset, 
                   40, 20, Qt::AlignHCenter, QString::number(gindex));
        p.drawLine(hpos, clineoffset-2, 
                   hpos, clineoffset+2);
      }
    }
  }

  
  p.setPen(Qt::black);

  // y-axis and ticks
  int hpos = 0;
  if (m_read->m_rc) { hpos = lastTracePos; }

  p.drawLine(hpos, baseline, hpos, baseline-(maxy/vscale));
  for (i = 0; i < maxy; i+=100)
  {
    
    int y = baseline-(i/vscale);
    p.drawLine(hpos-tickwidth, y, hpos+tickwidth, y);
  }

  // x-axis, ticks, and labels
  p.drawLine(0, baseline, lastTracePos, baseline);
  for (i = 0; i < m_trace->NPoints; i+=10)
  {
    hpos = (int)(m_hscale*i);
    if (m_read->m_rc)
    {
      hpos = lastTracePos - hpos;
    }

    // chromo ticks and labels
    if (i % 50 == 0)
    {
      p.setFont(QFont("Helvetica", 8));
      p.drawText(hpos-20, baseline+10,
                 40,20,Qt::AlignHCenter,QString::number(i));

      p.drawLine(hpos, baseline+tickwidth,
                 hpos, baseline-tickwidth);
    }
    else
    {
      p.drawLine(hpos, baseline+1,
                 hpos, baseline-1);
    }
  }

  // trace
  for (int channel = 0; channel < 4; channel++)
  {
    unsigned short * trace = NULL;
    if (m_read->m_rc)
    {
      switch (channel)
      {
        case 0: trace = m_trace->traceA; UIElements::setBasePen(pen, 'T'); break;
        case 1: trace = m_trace->traceC; UIElements::setBasePen(pen, 'G'); break;
        case 2: trace = m_trace->traceG; UIElements::setBasePen(pen, 'C'); break;
        case 3: trace = m_trace->traceT; UIElements::setBasePen(pen, 'A'); break;
      };
    }
    else
    {
      switch (channel)
      {
        case 0: trace = m_trace->traceA; UIElements::setBasePen(pen, 'A'); break;
        case 1: trace = m_trace->traceC; UIElements::setBasePen(pen, 'C'); break;
        case 2: trace = m_trace->traceG; UIElements::setBasePen(pen, 'G'); break;
        case 3: trace = m_trace->traceT; UIElements::setBasePen(pen, 'T'); break;
      };
    }

    p.setPen(pen);

    QPainterPath path;
    path.moveTo(0,baseline);

    if (m_read->m_rc)
    {
      path.moveTo(lastTracePos, baseline);
    }

    for (i = 0; i < m_trace->NPoints; i++)
    {
      hpos = (int)(m_hscale * i);
      if (m_read->m_rc) { hpos = lastTracePos - hpos; }

      path.lineTo(hpos,baseline-(trace[i])/vscale);
    }

    p.drawPath(path);
  }

  p.setPen(Qt::black);
  p.drawLine(startgindex, clineoffset, endgindex, clineoffset);
  p.drawLine(0, slineoffset, lastTracePos, slineoffset);

  p.end();
}

void ChromoField::paintEvent(QPaintEvent * event)
{
  if (m_pix)
  {
    QPainter painter(this);
    painter.drawPixmap(0, 0, *m_pix);
  }
}


int ChromoField::getWindowPos(int gindex)
{
  int retval = 1; 

  if (m_pix)
  {
    int gseqpos = m_read->getGSeqPos(gindex);

    if (m_read->m_rc)
    {
      gseqpos = m_read->m_pos.size() - gseqpos;
      Read * m_trace = (Read *) m_read->m_trace;
      retval = (int)((m_trace->NPoints-m_read->m_pos[gseqpos])* m_hscale);
    }
    else
    {
      retval = (int)(m_read->m_pos[gseqpos]*m_hscale);
    }

    retval += m_hoffset;
  }

  return retval;
}
