#include "NChartWidget.hh"
#include "InsertStats.hh"

#include <qstring.h>
#include <qpixmap.h>
#include <qpainter.h>

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
  setWFlags(Qt::WRepaintNoErase | Qt::WDestructiveClose);
}

NChartWidget::~NChartWidget()
{
  delete m_stats;
  m_stats = NULL;
}

void NChartWidget::setStats(InsertStats * stats)
{
  if (m_stats) { delete m_stats; }
  m_stats = stats;
  m_stats->nchart();
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

  p.drawText(0,10,width,30, Qt::AlignCenter,label);

  int gutter     = 20;
  int histtop    = 50;
  int bottomtext = 60;
  int histheight = height - histtop - bottomtext;
  int histbottom = histtop + histheight;

  int histleft   = 40;
  int histwidth  = width-histleft-gutter;

  p.setBrush(QColor(240,240,240));
  p.drawRect(histleft, histtop, histwidth, histheight);

  if (m_stats->count() == 0)
  {
    label = "Insufficient Count";
    p.drawText(center-400, histtop + histheight/2 - 10,
               800, 30, Qt::AlignCenter, label);
  }
  else
  {
    double xscale = (double)(histwidth)/ 100;
    double yscale = (double)(histheight-gutter) / m_stats->m_maxsize;

    // Xlabels
    pen.setStyle(Qt::DotLine);
    p.setPen(pen);

    int labelwidth = 100;
    int numbuckets = (int)(labelwidth / xscale);
    if (numbuckets == 0) { numbuckets = 1; }

    int prec = 0;

    for (double i = 12.5; i < 100; i+=12.5)
    {
      int xcoord = (int)(histleft+i*xscale);
      p.drawLine(xcoord, histtop, xcoord, histbottom+5);

      if (i == 25.0 || i == 50.0 || i == 75.0)
      {
        label = "  ";
        label += QString::number(i, 'f', prec);
        label += "%";
        p.drawText(xcoord-labelwidth, histbottom,
                   labelwidth*2, 30, Qt::AlignCenter, label);
      }
    }

    int yjump = (int)(100/yscale);
    yjump = (int)NiceNumber(yjump, true);
    if (yjump == 0) { yjump = 1; }

    for (int j = (int)NiceNumber(m_stats->m_maxsize, true); j > 0; j -= yjump)
    {
      if (j > m_stats->m_maxsize) { continue; }
      int ycoord = (int)(histbottom - j * yscale);

      if (m_grid) { p.drawLine(histleft-5, ycoord, histleft+histwidth, ycoord); }

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
      p.worldMatrix().invert().map(histleft-20, ycoord, &mapx, &mapy);
      p.drawPixmap(mapx-40, mapy-10, buffer);
      p.restore();
    }


    // NChart
    pen.setStyle(Qt::SolidLine);
    p.setPen(pen);
    p.setBrush(QColor(100,160,255));

    int l = m_stats->m_sizes.size();

    for (int i = 0; i+1 < l; i++)
    {
      int ystart = (int) (m_stats->m_sizes[i]   * yscale);
      int yend   = (int) (m_stats->m_sizes[i+1] * yscale);

      int xstart = (int) ((100-m_stats->m_bucketlow[i]) * xscale);
      int xend   = (int) ((100-m_stats->m_bucketlow[i+1]) * xscale);

      if (ystart > 1)
      {
        p.drawRect(histleft+xstart, histbottom-ystart,
                   xend-xstart+1,     ystart);
      }
      else
      {
        p.drawRect(histleft+xstart,    histbottom-2,
                   histwidth-xstart, 2);
        break;
      }
    }

    // text
    int textline1 = histbottom + 30;

    label = "Count: " + QString::number(m_stats->m_sizes.size(), 'f', prec);
    label += "   Max: " + QString::number(m_stats->m_sizes[0], 'f', prec) +
             "   N50: " + QString::number(m_stats->nvalue(50), 'f', prec);

    p.drawText(0, textline1,
               width, 30, Qt::AlignHCenter | Qt::AlignVCenter, label);
  }

  p.end();

  p.begin(this);
  p.drawPixmap(0,0,pix);
  p.end();
}



