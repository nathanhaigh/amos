#include "ChromoField.hh"
#include "RenderSeq.hh"
#include <qpainter.h>
#include <qpen.h>
#include <math.h>
#include <qstring.h>
#include <string>
#include <qpixmap.h>

#include <stdio.h>
#include "UIElements.hh"
#include <qscrollview.h>

using namespace std;
using namespace AMOS;




ChromoField::ChromoField(RenderSeq_t * render, 
                         const string & db, 
                         const string & cons,
                         const string & cstatus,
                         QWidget *parent, 
                         const char *name)
  :QWidget(parent, name)
{
  setPalette(QPalette(QColor(180, 180, 180)));
  m_pix = NULL;
  m_hscale = 1.5;
  m_contigView = true;

  string readname = render->m_read.getEID();

  string path = "/local/chromo/Chromatograms/";
  path += db + "/ABISSed/" + readname[0]+readname[1]+readname[2] + "/"
                           + readname[0]+readname[1]+readname[2]+readname[3] + "/"
                           + readname[0]+readname[1]+readname[2]+readname[3]+readname[4]+ "/" 
                           + readname;

  m_render = render;
  m_trace = read_reading((char *)path.c_str(), TT_ANY);
  if (!m_trace) { return; }

  char seqname[100];
  int version;

  char hex[5];
  hex[4]='\0';
  string curseq;
  FILE * fpos = fopen("dmg.pos", "r");
  char c;
  int i;
  int x;
  while ((i = fscanf ( fpos, "%s\t%d\t", seqname, &version )) != 0)
  {
    if (readname == seqname)
    {
      while ( fgets ( hex, 5, fpos ) && hex[0] != '\n' )
      {
        sscanf ( hex, "%04x", &x );
        m_pos.push_back(x);
      }

      break;
    }
    else
    {
      do {
        c = fgetc(fpos);
      } while (c != '\n');
    }
  }

  cerr << "loaded " << m_pos.size() << " positions" << endl;


  int vscale=24;
  int tickwidth = 2;
  int maxy = 2000;

  int diam          = 5;
  int fontsize      = 8;
  int gutter        = fontsize/2;
  int lineheight    = fontsize+gutter;

  int consposoffset = 5;
  int clineoffset   = consposoffset+lineheight+5;
  int discoffset    = clineoffset+5;
  int consoffset    = discoffset+fontsize;

  int seqoffset     = consoffset+lineheight;
  int slineoffset   = seqoffset+lineheight+5;
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
  p.setFont(QFont("Helvetica", 8));

  int rangebegin = render->m_tile->range.begin;
  int rangeend   = render->m_tile->range.end + render->m_tile->gaps.size() - 1;
  
  // Render the full gapped sequence
  string bases = render->m_read.getSeqString();

  if (render->m_rc)
  {
    rangebegin = bases.length() - render->m_tile->range.begin;
    reverse(bases.begin(), bases.end());
    reverse(m_pos.begin(), m_pos.end());
  }

  // Insert gaps
  Pos_t gapcount;
  vector<Pos_t>::const_iterator g;
  for (g =  render->m_tile->gaps.begin(), gapcount = 0; 
       g != render->m_tile->gaps.end(); 
       g++, gapcount++)
  {
    bases.insert(*g+gapcount+rangebegin, 1, '-');

    int left  = m_pos[*g+gapcount+rangebegin-1];
    int right = m_pos[*g+gapcount+rangebegin];
    m_pos.insert(m_pos.begin()+*g+gapcount+rangebegin, 1, (left+right)/2);
  }

  if (render->m_rc)
  {
    reverse(bases.begin(), bases.end());
    reverse(m_pos.begin(), m_pos.end());
  }

  
  // tint outside the assembled
  p.setPen(QColor(140,140,140));
  p.setBrush(QColor(140,140,140));
  
  if (!render->m_rc || m_contigView)
  {
    if (render->m_rc)
    {
      rangebegin = render->m_tile->range.end;
      rangeend   = render->m_tile->range.begin + render->m_tile->gaps.size() - 1;
    }

    if (rangebegin)
    {
      int rpos = m_pos[rangebegin];
      int bpos = m_pos[rangebegin-1];

      int width = (int)(((rpos+bpos)/2)*m_hscale);

      if (render->m_rc)
      {
        p.drawRect(lastTracePos - width, 2, width+2, height()-4);
      }
      else
      {
        p.drawRect(-5, 2, width+5, height() -4);
      }
    }

    if (rangeend != (int) bases.length()-1)
    {
      int rpos = m_pos[rangeend];
      int bpos = m_pos[rangeend+1];
      int width = (int)(((rpos+bpos)/2)*m_hscale);

      if (render->m_rc)
      {
        p.drawRect(-5, 2, (lastTracePos-width)+5, height()-4);
      }
      else
      {
        p.drawRect(width-2, 2,
                   lastTracePos-width+3, height()-4);
      }
    }
  }

  // Draw the bases and consensus
  int startgindex = -1;
  int endgindex = -1;

  QString s;

  for (i = 0; i < (int) m_pos.size(); i++)
  {
    char b = bases[i];

    int hpos = (int)(m_hscale*m_pos[i]);

    if (render->m_rc && m_contigView)
    {
      hpos = lastTracePos - hpos;
      b = Complement(b);
    }

    s = b;

    p.setPen(UIElements::getBaseColor(b));
    p.drawText(hpos-20, seqoffset,
               40, 20, Qt::AlignHCenter,s);

    if (b != '-' && (i % 5 == 0))
    {
      p.setPen(black);
      p.drawText(hpos-20, sposoffset,
                 40, 20, Qt::AlignHCenter, QString::number(i));

      p.drawLine(hpos, slineoffset-2,
                 hpos, slineoffset+2);
    }

    int gindex = render->getGindex(i);
    //if (gindex >= render->m_loffset && gindex <= render->m_roffset)
    if (gindex >= 0 && gindex < (int) cons.length())
    {
      char c = cons[gindex];
      if (render->m_rc && !m_contigView) { c = Complement(c); }

      s=c;
      p.setPen(UIElements::getBaseColor(c));

      if (startgindex == -1) { startgindex = hpos-fontsize; }
      else                   { endgindex   = hpos+fontsize; }

      p.drawText(hpos-20, consoffset,
                 40, 20, Qt::AlignHCenter,s);

      p.setPen(black);

      if (cstatus[gindex] == 'X')
      {
        p.setBrush(Qt::SolidPattern);
        p.setBrush(Qt::black);
        p.drawEllipse(hpos-diam/2-1, discoffset, diam, diam);
      }

      if (gindex % 5 == 0)
      {
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
  if (m_render->m_rc && m_contigView) { hpos = lastTracePos; }

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
    if (m_render->m_rc && m_contigView)
    {
      hpos = lastTracePos - hpos;
    }

    if (i % 50 == 0)
    {
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

  for (int channel = 0; channel < 4; channel++)
  {
    unsigned short * trace = NULL;
    if (render->m_rc && m_contigView)
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

    p.moveTo(0,baseline);

    if (render->m_rc && m_contigView)
    {
      p.moveTo(lastTracePos, baseline);
    }

    for (i = 0; i < m_trace->NPoints; i++)
    {
      hpos = (int)(m_hscale * i);
      if (render->m_rc && m_contigView) { hpos = lastTracePos - hpos; }

      p.lineTo(hpos,baseline-(trace[i])/vscale);
    }
  }

  p.setPen(black);
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
  int gseqpos = m_render->getGSeqPos(gindex);
  int retval = (int)(m_pos[gseqpos]*m_hscale);

  if (m_contigView && m_render->m_rc)
  {
    retval = (int)(m_trace->NPoints * m_hscale) - retval;
  }

  retval += m_hoffset;

  return retval;
}
