#include "NChartWidget.hh"
#include "NChartStats.hh"

#include <qstring.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <cmath>

#include <iostream>

using namespace std;

double NiceNumber (double x, bool round)
{
  // adapted from "Nice numbers for graph labels"
  // by Paul S. Heckbert published in Graphics Gems pp. 61-63
  // finds a "nice" number approximately equal to x.
  // Round the number if round=1, take a ceiling if round=0

  double nf; // nice, rounded fraction
  int exp = (int)floor ( log10 ( x ) ); // exponent of x
  double f = x / pow ( 10.0, exp ); // fractional part of x; between 1 and 10

  if ( round )
    {
      if ( f < 1.5 )
        nf = 1.0;
      else if ( f < 3.0 )
        nf = 2.0;
      else if ( f < 7.0 )
        nf = 5.0;
      else
        nf = 10.0;
    }
  else
    {
      if ( f < 1.0 )
        nf = 1.0;
      else if ( f < 2.0 )
        nf = 2.0;
      else if ( f < 5.0 )
        nf = 5.0;
      else
        nf = 10.0;
    }
  
  return nf * pow ( 10.0, exp );
}


NChartWidget::NChartWidget(QWidget * parent, const char * name)
 : QWidget(parent, name),
   m_stats(NULL), m_grid(true)
{
  setMinimumSize(250, 250);
  setWFlags(Qt::WRepaintNoErase | Qt::WDestructiveClose | Qt::WResizeNoErase);

  m_ordering = 0;
  m_colorstyle = 0;

  m_highlightsize = -1;
  setMouseTracking(true);
}

NChartWidget::~NChartWidget()
{
  delete m_stats;
  m_stats = NULL;
}

void NChartWidget::setStats(NChartStats * stats)
{
  if (m_stats) { delete m_stats; }
  m_stats = stats;
  m_stats->nchart(m_ordering);
}

void NChartWidget::setOrdering(int ordering)
{
  m_ordering = ordering;
  m_stats->nchart(m_ordering);
  update();
}

void NChartWidget::setColorStyle(int colorstyle)
{
  m_colorstyle = colorstyle;
  update();
}

void NChartWidget::paintEvent(QPaintEvent * event)
{
  if (!m_stats) { return; }

  int width = this->width();
  int height = this->height();

  QPixmap pix(width, height);
  pix.fill(Qt::white);

  QPainter p(&pix);
  QPen pen(Qt::black);


  p.setPen(pen);
  p.setFont(QFont("Helvetica", 12));

  int center = width/2;

  QString label = m_stats->m_label;

  p.drawText(0,5,width,25, Qt::AlignCenter | Qt::AlignVCenter,label);

  int gutter     = 20;
  int bottomtext = 80;
  m_histtop    = 40;
  m_histleft   = 40;
  m_histheight = height - m_histtop - bottomtext;
  m_histbottom = m_histtop + m_histheight;

  m_histwidth  = width-m_histleft-gutter;

  if (m_stats->m_maxscore)
  {
    m_histwidth -= 30;
  }

  int textline1 = m_histbottom + 25;
  int textline2 = textline1 + 20;

  QColor baserectcolor(100,160,255);

  if (m_colorstyle == 1)
  {
    baserectcolor.setHsv(0,1,255);
  }
  else if (m_colorstyle == 0)
  {
    baserectcolor.setHsv(105,160,255);
  }

  p.setBrush(QColor(240,240,240));
  p.drawRect(m_histleft, m_histtop, m_histwidth, m_histheight);

  if (m_stats->m_maxscore)
  {
    int thermoleft = m_histleft+m_histwidth+10;

    int h,s,v;
    baserectcolor.hsv(&h,&s,&v);

    QColor tcolor(baserectcolor);

    if (m_colorstyle == 1) 
    { 
      for (int i = 0; i < 255; i+=5)
      {
        if (i+5 > 255) { i = 255-5; }

        tcolor.setHsv(h,i,v);
        p.setPen(tcolor);
        p.setBrush(tcolor);

        double top = m_histheight * (i+5) / (255);
        double theight = m_histheight * 7.0 / (255.0);

        p.drawRect(thermoleft+1, m_histbottom-1-top, 8, theight);
      }
    }
    else if (m_colorstyle == 0)
    {
      double yscale = (double)(m_histheight) / (h+5);

      for (int i = h; i >= 0; i-=5)
      {
        tcolor.setHsv(i,s,v);
        p.setPen(tcolor);
        p.setBrush(tcolor);

        double top = yscale * (h-i+5);
        double theight = yscale * 6;

        p.drawRect(thermoleft+1, m_histbottom-1-top, 8, theight);
      }
    }

    p.setPen(Qt::black);
    p.setBrush(Qt::NoBrush);
    p.drawRect(thermoleft, m_histtop, 10, m_histheight);
    p.setBrush(Qt::SolidPattern);

    QPixmap buffer0(80,15);
    buffer0.fill();
    QPainter lp0(&buffer0);
    lp0.setPen(Qt::black);
    lp0.setPen(Qt::black);
    lp0.setFont(QFont("Helvetica", 12));
    lp0.drawText(0,0,80,15, Qt::AlignHCenter|Qt::AlignVCenter, "0");
    lp0.end();

    QPixmap buffermax(80,15);
    buffermax.fill();
    QPainter lp1(&buffermax);
    lp1.setPen(Qt::black);
    lp1.setPen(Qt::black);
    lp1.setFont(QFont("Helvetica", 12));
    lp1.drawText(0,0,80,15, Qt::AlignHCenter|Qt::AlignVCenter, QString::number(m_stats->m_maxscore));
    lp1.end();

    QPixmap bufferfeat(80,15);
    bufferfeat.fill();
    QPainter lp2(&bufferfeat);
    lp2.setPen(Qt::black);
    lp2.setPen(Qt::black);
    lp2.setFont(QFont("Helvetica", 12));
    lp2.drawText(0,0,80,15, Qt::AlignHCenter|Qt::AlignVCenter, "Features");
    lp2.end();

    p.save();
    p.rotate(-90);
    int mapx, mapy;
    p.worldMatrix().invert().map(thermoleft+25, m_histbottom, &mapx, &mapy);
    p.drawPixmap(mapx-40, mapy-10, buffer0);

    p.worldMatrix().invert().map(thermoleft+25, m_histtop + m_histheight/2, &mapx, &mapy);
    p.drawPixmap(mapx-40, mapy-10, bufferfeat);

    p.worldMatrix().invert().map(thermoleft+25, m_histtop, &mapx, &mapy);
    p.drawPixmap(mapx-40, mapy-10, buffermax);

    p.restore();
  } 
    

  if (m_stats->count() == 0)
  {
    label = "Insufficient Count";
    p.drawText(center-400, m_histtop + m_histheight/2 - 10,
               800, 30, Qt::AlignCenter, label);
  }
  else
  {
    m_xscale = (double)(m_histwidth)/ 100;
    m_yscale = (double)(m_histheight-gutter) / m_stats->m_maxsize;

    // Xlabels
    pen.setStyle(Qt::DotLine);
    p.setPen(pen);

    int labelwidth = 100;
    int numbuckets = (int)(labelwidth / m_xscale);
    if (numbuckets == 0) { numbuckets = 1; }

    int prec = 0;

    for (double i = 12.5; i < 100; i+=12.5)
    {
      int xcoord = (int)(m_histleft+i*m_xscale);
      p.drawLine(xcoord, m_histtop, xcoord, m_histbottom+5);

      if (i == 25.0 || i == 50.0 || i == 75.0)
      {
        label = "  ";
        label += QString::number(i, 'f', prec);
        label += "%";
        p.drawText(xcoord-labelwidth, m_histbottom,
                   labelwidth*2, 30, Qt::AlignCenter, label);
      }
    }

    int yjump = (int)(100/m_yscale);
    yjump = (int)NiceNumber(yjump, true);
    if (yjump == 0) { yjump = 1; }

    for (int j = (int)NiceNumber(m_stats->m_maxsize, true); j > 0; j -= yjump)
    {
      if (j > m_stats->m_maxsize) { continue; }
      int ycoord = (int)(m_histbottom - j * m_yscale);

      if (m_grid) { p.drawLine(m_histleft-5, ycoord, m_histleft+m_histwidth, ycoord); }

      if (j >= 1000000)
      {
        double q = j /1000000.0;
        
        label = QString::number(q, 'f', 1);
        label += "M";
      }
      else if (j >= 1000)
      {
        double q = j /1000.0;

        label = QString::number(q, 'f', 1);
        label += "K";
      }
      else
      {
        label = QString::number(j);
      }

      QPixmap buffer(80,15);
      buffer.fill();
      QPainter lp(&buffer);
      lp.setPen(Qt::black);
      lp.setFont(QFont("Helvetica", 12));
      lp.drawText(0,0,80,15, Qt::AlignHCenter|Qt::AlignVCenter, label);
      lp.end();

      p.save();
      p.rotate(-90);
      int mapx, mapy;
      p.worldMatrix().invert().map(m_histleft-20, ycoord, &mapx, &mapy);
      p.drawPixmap(mapx-40, mapy-10, buffer);
      p.restore();
    }


    // NChart
    pen.setStyle(Qt::SolidLine);
    p.setPen(pen);

    int l = m_stats->m_sizes.size();

    for (int i = 0; i+1 < l; i++)
    {
      QColor rectcolor(baserectcolor);
      
      if (m_stats->m_maxscore)
      {
        int h,s,v;
        rectcolor.hsv(&h,&s,&v);

        double badness = ((double)m_stats->m_sizes[i].m_score) / m_stats->m_maxscore;

        if (m_colorstyle == 1)
        {
          s = (int)(badness * 255);
        }
        else if (m_colorstyle == 0)
        {
          h -= badness * (h);
          h %= 360;
        }

        rectcolor.setHsv(h,s,v);
      }

      p.setBrush(rectcolor);

      int ystart = (int) (m_stats->m_sizes[i].m_size * m_yscale);
      int yend   = (int) (m_stats->m_sizes[i+1].m_size * m_yscale);


      double left = 100-m_stats->m_sizes[i].m_perc;
      double right = 100-m_stats->m_sizes[i+1].m_perc;

      int xstart = (int) (left * m_xscale);
      int xend   = (int) (right * m_xscale);

      if (left <= m_highlightsize && m_highlightsize <= right)
      {
        p.setPen(Qt::white);
        p.setBrush(Qt::blue);

        QString info = "Selected: " + QString::number(m_stats->m_sizes[i].m_id, 'f', 0) +
                       "  Size: " + QString::number(m_stats->m_sizes[i].m_size, 'f', 0) +
                       "  (" + QString::number(100-m_stats->m_sizes[i+1].m_perc, 'f', 2) +
                       "%) " + QString::number(m_stats->m_sizes[i].m_score) +
                       " Features";

        p.setPen(Qt::black);
        p.drawText(0, textline2,
                   width, 30, Qt::AlignHCenter | Qt::AlignVCenter, info);
      }

      if (ystart > 1)
      {
        p.drawRect(m_histleft+xstart, m_histbottom-ystart,
                   xend-xstart+1,     ystart);
      }
      else
      {
        p.drawRect(m_histleft+xstart,    m_histbottom-2,
                   m_histwidth-xstart, 2);
      }
    }

    // text

    label = "Count: " + QString::number(m_stats->m_sizes.size(), 'f', prec);
    label += "   Max: " + QString::number(m_stats->m_sizes[0].m_size, 'f', prec);
    label += "   N50: " + QString::number(m_stats->nvalue(50).m_size, 'f', prec);
    label += "   Total: " + QString::number(m_stats->m_sum, 'f', prec);

    p.drawText(0, textline1,
               width, 30, Qt::AlignHCenter | Qt::AlignVCenter, label);

  }

  p.end();

  p.begin(this);
  p.drawPixmap(0,0,pix);
  p.end();
}


void NChartWidget::mouseMoveEvent(QMouseEvent * e)
{
  if (e->y() >= m_histtop && e->y() <= m_histbottom)
  {
    double xpos = (e->x() - m_histleft) / m_xscale;
    m_highlightsize = xpos;
  }
  else
  {
    m_highlightsize = -1;
  }

  update();
}

void NChartWidget::mouseDoubleClickEvent(QMouseEvent * e)
{
  mouseMoveEvent(e);

  int l = m_stats->m_sizes.size();
  for (int i = 0; i+1 < l; i++)
  {
    double left = 100-m_stats->m_sizes[i].m_perc;
    double right = 100-m_stats->m_sizes[i+1].m_perc;

    if (left <= m_highlightsize && m_highlightsize <= right)
    {
      cerr << "Click: " << m_stats->m_sizes[i].m_perc
           << " " << m_stats->m_sizes[i].m_id << endl;

      emit idSelected(m_stats->m_sizes[i].m_id);
    }
  }
}
