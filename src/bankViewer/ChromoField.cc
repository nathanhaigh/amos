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




ChromoField::ChromoField(RenderSeq_t * read, 
                         const string & db, 
                         const string & cons,
                         const string & cstatus,
                         QWidget *parent, 
                         const char *name)
  :QWidget(parent, name)
{
  setPalette(QPalette(QColor(180, 180, 180)));
  m_pix = NULL;
  int hscale = 2;

  string readname = read->m_read.getEID();

  string path = "/local/chromo/Chromatograms/";
  path += db + "/ABISSed/" + readname[0]+readname[1]+readname[2] + "/"
                           + readname[0]+readname[1]+readname[2]+readname[3] + "/"
                           + readname[0]+readname[1]+readname[2]+readname[3]+readname[4]+ "/" 
                           + readname;

  m_render = read;
  m_trace = read_reading((char *)path.c_str(), TT_ANY);

  if (!m_trace)
  {
    return;
  }

  resize (hscale*m_trace->NPoints+100,210);

  vector<int> pos;

  char name[100];
  int version;

  char hex[5];
  hex[4]='\0';
  string curseq;
  FILE * fpos = fopen("dmg.pos", "r");
  char c;
  int i;
  while ((i = fscanf ( fpos, "%s\t%d\t", name, &version )) != 0)
  {
    if (readname == name)
    {
      while ( fgets ( hex, 5, fpos ) && hex[0] != '\n' )
      {
          int x;

          sscanf ( hex, "%04x", &x );
          pos.push_back(x);
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

  cerr << "loaded " << pos.size() << "positions" << i << endl;

  m_pix = new QPixmap(width(), height());
  m_pix->fill(this, 0, 0);

  QPen pen;
  QPainter painter(m_pix);

  int baseline = 100;
  int offset = 20;

  int vscale=24;
  int tickwidth = 5;
  int maxy = 2000;

  int diam          = 5;
  int fontsize      = 8;
  int gutter        = fontsize/2;
  int lineheight    = fontsize+gutter;

  int sposoffset    = baseline+30;
  int slineoffset   = sposoffset+lineheight+5;
  int seqoffset     = slineoffset+5;
  int consoffset    = seqoffset+lineheight;
  int discoffset    = consoffset+lineheight;
  int clineoffset   = discoffset+lineheight;
  int consposoffset = clineoffset+4;

  int curpos = 0;
  int startgindex = -1;
  int endgindex = -1;


  // y-ticks
  for (i = 0; i < maxy; i+=100)
  {
    int y = baseline-(i/vscale);
    painter.drawLine(offset,y,offset+tickwidth,y);
  }

  // x-ticks
  for (i = 0; i < m_trace->NPoints; i+=10)
  {
    painter.drawLine(hscale*i+offset,baseline,hscale*i+offset,baseline-tickwidth);
    if (!(i % 50))
    {
      painter.drawLine(hscale*i+offset, baseline+tickwidth, hscale*i+offset, baseline);
    }
  }

  // x-labels
  painter.setFont(QFont("Helvetica", 8));
  for (i=0; i < m_trace->NPoints; i += 50)
  {
    QString s = QString::number(i);
    painter.drawText(hscale*i-20+offset,baseline+10,40,20,Qt::AlignHCenter,s);
  }

  painter.setFont(QFont("Helvetica", 8));

  for (int channel = 0; channel < 4; channel++)
  {
    unsigned short * trace = NULL;
    switch (channel)
    {
      case 0: trace = m_trace->traceA; UIElements::setBasePen(pen, 'A'); break;
      case 1: trace = m_trace->traceC; UIElements::setBasePen(pen, 'C'); break;
      case 2: trace = m_trace->traceG; UIElements::setBasePen(pen, 'G'); break;
      case 3: trace = m_trace->traceT; UIElements::setBasePen(pen, 'T'); break;
    };

    painter.setPen(pen);

    painter.moveTo(offset,baseline);
    for (i = 0; i < m_trace->NPoints; i++)
    {
      if (trace[i])
      {
        painter.lineTo(hscale*i+offset,baseline-(trace[i])/vscale);
      }
      else
      {
        painter.moveTo(hscale*i+offset,baseline);
      }
    }
  }

  painter.setPen(black);
  
  // axis
  painter.drawLine(offset, baseline, hscale*m_trace->NPoints+offset, baseline);
  painter.drawLine(offset, baseline, offset, baseline-(maxy/vscale));

  painter.drawLine(offset, slineoffset, hscale*m_trace->NPoints+offset, slineoffset);

  string bases = read->m_read.getSeqString();

  int rangebegin = read->m_tile->range.begin;

  if (read->m_rc)
  {
    rangebegin = bases.length() - read->m_tile->range.begin;
    reverse(bases.begin(), bases.end());
    reverse(pos.begin(), pos.end());
  }

  // Insert gaps
  Pos_t gapcount;
  vector<Pos_t>::const_iterator g;
  for (g =  read->m_tile->gaps.begin(), gapcount = 0; 
       g != read->m_tile->gaps.end(); 
       g++, gapcount++)
  {
    bases.insert(*g+gapcount+rangebegin, 1, '-');

    int left  = pos[*g+gapcount+rangebegin-1];
    int right = pos[*g+gapcount+rangebegin];
    pos.insert(pos.begin()+*g+gapcount+rangebegin, 1, (left+right)/2);
  }

  if (read->m_rc)
  {
    reverse(bases.begin(), bases.end());
    reverse(pos.begin(), pos.end());
  }

  for (i = 0; i < pos.size(); i++)
  {
    char b = bases[i];
    painter.setPen(UIElements::getBaseColor(b));

    int coordinate = curpos;
    curpos++;
    //if (b == '-') { coordinate--; }
    //else          { curpos++; }

    QString s;
    s = b;

    painter.drawText(hscale*pos[i]-20+offset, seqoffset,
                     40, 20, Qt::AlignHCenter,s);

    if (b != '-' && (coordinate % 5 == 0))
    {
      painter.setPen(black);
      painter.drawText(hscale*pos[i]-20+offset, sposoffset,
                       40, 20, Qt::AlignHCenter,QString::number(coordinate));
      painter.drawLine(hscale*pos[i]+offset, slineoffset-2,
                       hscale*pos[i]+offset, slineoffset+2);
    }

    int gindex = read->getGindex(i);
    if (gindex >= read->m_offset && gindex < read->m_offset + read->m_nucs.size())
    {
      char c = cons[gindex];
      if (read->m_rc)
      {
        c = Complement(c);
      }

      s=c;
      painter.setPen(UIElements::getBaseColor(c));

      if (startgindex == -1) { startgindex = hscale*pos[i]+offset-fontsize; }
      else                   { endgindex   = hscale*pos[i]+offset+fontsize; }

      painter.drawText(hscale*pos[i]-20+offset, consoffset,
                       40, 20, Qt::AlignHCenter,s);

      if (cstatus[gindex] == 'X')
      {
        painter.setBrush(Qt::SolidPattern);
        painter.setPen(Qt::black);
        painter.setBrush(Qt::black);
        painter.drawEllipse(hscale*pos[i]+offset-diam/2-1, discoffset,
                            diam, diam);
      }

      if (gindex % 5 == 0)
      {
        painter.setPen(black);
        painter.drawText(hscale*pos[i]-20+offset, consposoffset,
                         40, 20, Qt::AlignHCenter,QString::number(gindex));
        painter.drawLine(hscale*pos[i]+offset, clineoffset-2,
                         hscale*pos[i]+offset, clineoffset+2);
      }
    }
  }

  painter.setPen(black);
  painter.drawLine(startgindex, clineoffset, endgindex, clineoffset);

  painter.end();
}

void ChromoField::paintEvent(QPaintEvent * event)
{
  if (m_pix)
  {
    QPainter painter(this);
    painter.drawPixmap(0, 0, *m_pix);
  }
}

