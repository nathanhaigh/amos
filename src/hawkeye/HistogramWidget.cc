#include "HistogramWidget.hh"
#include "InsertStats.hh"

#include <qstring.h>
#include <qpixmap.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>

#include <iostream>
#include <cmath>

using namespace std;

HistogramWidget::HistogramWidget(InsertStats * stats, QWidget * parent, const char * name)
 : QWidget(parent, name),
   m_stats(stats), m_fitzero(true), m_grid(true), m_shademean(true), m_shadesd(2.0)
{
  setMinimumSize(550, 500);
  setWindowFlags(Qt::WRepaintNoErase | Qt::WDestructiveClose | Qt::WResizeNoErase);
  m_stats->histogram(200, m_fitzero);
}

HistogramWidget::~HistogramWidget()
{
  delete m_stats;
  m_stats = NULL;
}

static void drawNormal(QPainter & p, 
                int histleft, int histbottom,
                int histwidth, int histheight,
                double xvalscale,
                double mean, double sd,
                double low, double high)
{
  double cnst = 1/(sqrt(2*3.14159*sd*sd));
  double expval = -(mean-mean)*(mean-mean)/(2*sd*sd);
  double maxval = cnst * exp(expval);
  int lastx = histleft;
  int lasty = histbottom;

  double yvalscale = (histheight) / maxval;

  for (int i = low; i < high; i+=10)
  {
    expval = -(i-mean)*(i-mean)/(2*sd*sd);
    double val = cnst * exp(expval);

    int xval = histleft + i * xvalscale;
    int yval = histbottom - val * yvalscale;

    p.drawLine(lastx, lasty, xval, yval);

    lastx = xval;
    lasty = yval;
  }
}

void HistogramWidget::paintEvent(QPaintEvent * event)
{
  int width = this->width();
  int height = this->height();

  QPixmap pix(width, height);
  pix.fill(Qt::white);

  QPainter p(&pix);
  QPen pen(Qt::black);


  p.setPen(pen);
  p.setFont(QFont("Helvetica", 12));

  int center = width/2;

  QString label = m_stats->m_label.c_str();

  p.drawText(center-400,10,800,30, Qt::AlignCenter,label);

  int gutter     = 20;
  int histtop    = 50;
  int bottomtext = 100;
  int histheight = height - histtop - bottomtext;
  int histbottom = histtop + histheight;

  int histleft   = 60;
  int histwidth  = width-histleft-gutter;

  p.setBrush(QColor(240,240,240));
  p.drawRect(histleft, histtop, histwidth, histheight);

  int buckets = m_stats->m_buckets.size();

  double xscale = (double)(histwidth)/ buckets;
  double yscale = (double)(histheight-gutter) / m_stats->m_maxcount;

  if (m_stats->count() == 0)
  {
    label = "Insufficient Count";
    p.drawText(center-400, histtop + histheight/2 - 10,
               800, 30, Qt::AlignCenter, label);
  }
  else
  {
    // Shading
    if (m_shademean)
    {
      p.setBrush(Qt::yellow);

      double mean = m_stats->mean();
      double leftsd  = mean - m_shadesd*m_stats->stdev();
      double rightsd = mean + m_shadesd*m_stats->stdev();

      if (leftsd < 0) { leftsd = 0; }
      if (rightsd > m_stats->m_high) { rightsd = m_stats->m_high; }

      leftsd /= m_stats->m_bucketsize;
      rightsd /= m_stats->m_bucketsize;

      p.drawRect((int)(histleft + leftsd * xscale),      histtop,
                 (int)((rightsd - leftsd + 1) * xscale), histheight);

    }

    // Xlabels
    pen.setStyle(Qt::DotLine);
    p.setPen(pen);

    int labelwidth = 100;
    int numbuckets = (int)(labelwidth / xscale);
    if (numbuckets == 0) { numbuckets = 1; }

    int prec = 0;
    if (m_stats->m_high <= 1) { prec = 4; }

    for (int i = 0; i < buckets; i++)
    {
      if (i % numbuckets == 0)
      {
        int xcoord = (int)(histleft+i*xscale);

        if (m_grid) { p.drawLine(xcoord, histtop, xcoord, histbottom+5); }

        label = QString::number(m_stats->m_bucketlow[i], 'f', prec);

        p.drawText(xcoord-labelwidth, histbottom,
                   labelwidth*2, 30, Qt::AlignCenter, label);
      }
    }


    // ylables
    int yjump = (int)(100/yscale);
    if (yjump == 0) { yjump = 1; }
    for (int j = m_stats->m_maxcount; j >= 0; j -= yjump)
    {
      int ycoord = (int)(histbottom - j * yscale);

      if (m_grid) { p.drawLine(histleft-5, ycoord, histleft+histwidth, ycoord); }

      label = QString::number(j);

      p.drawText(histleft-100, ycoord-15,
                 90, 30, Qt::AlignRight | Qt::AlignVCenter, label);
    }


    // histogram
    pen.setStyle(Qt::SolidLine);
    p.setPen(pen);
    p.setBrush(QColor(100,160,255));

    for (int i = 0; i < buckets; i++)
    {
      int xwidth = (int)xscale;
      if (xwidth == 0) { xwidth = 1; }

      int yheight = (int)(m_stats->m_buckets[i]*yscale);
      if (m_stats->m_buckets[i] && yheight < 5) { yheight = 5; }

      p.drawRect((int)(histleft+i*xscale), 
                 (int)(histbottom - yheight),
                 (int)(xwidth)+1, 
                 (int)(yheight));
    }

    if (m_shademean)
    {
      double mean = m_stats->mean();
      mean /= m_stats->m_bucketsize;
      p.setPen(Qt::red);
      p.drawLine((int)(histleft + mean*xscale),  histtop,
                 (int)(histleft + mean*xscale), histbottom);
    }

    int doDrawNormal = 1;
    if (doDrawNormal)
    {
      double sd = m_stats->stdev();
      double mean = m_stats->mean();

      p.setPen(Qt::red);

      drawNormal(p, histleft, histbottom, histwidth, histheight-gutter, 
                 xscale/m_stats->m_bucketsize, mean, sd, 
                 m_stats->m_low, m_stats->m_high);
    }

    p.setPen(Qt::black);

    // text
    int textline1 = histbottom + 30;
    int textline2 = textline1 + 20;
    int textline3 = textline2 + 20;

    prec = 2;
    if (m_stats->m_high <= 1) { prec = 4; }

    label = "Sample Range: " + QString::number(m_stats->m_low, 'f', prec) +
            " - " + QString::number(m_stats->m_high, 'f', prec);

    p.drawText(histleft, textline1,
               histwidth, 30, Qt::AlignLeft | Qt::AlignVCenter, label);

    label = "Sample Mean: " + QString::number(m_stats->mean(), 'f', prec) +
            "   SD: " + QString::number(m_stats->stdev(), 'f', prec) +
            "   Count: " + QString::number(m_stats->count());

    p.drawText(histleft, textline2,
               histwidth, 30, Qt::AlignLeft | Qt::AlignVCenter, label);


    int withincount = m_stats->withinSD(m_shadesd);
    double withinperc = (double)(withincount*100.0)/m_stats->count();

    label = "Samples within " + QString::number(m_shadesd) +
            " SD: " + QString::number(withincount) +
            " (" + QString::number(withinperc, 'f', 2) + "%)";

    p.drawText(histleft, textline3,
               histwidth, 30, Qt::AlignLeft | Qt::AlignVCenter, label);
  }

  p.end();

  p.begin(this);
  p.drawPixmap(0,0,pix);
  p.end();
}


void HistogramWidget::setBucketCount(int count)
{
  m_stats->histogram(count, m_fitzero);
  update();
}

void HistogramWidget::setFitZero(bool fit)
{
  m_fitzero = fit;
  m_stats->histogram(m_stats->m_buckets.size(), m_fitzero);
  update();
}

void HistogramWidget::setEnableGrid(bool enable)
{
  m_grid = enable;
  update();
}

void HistogramWidget::setShadeMean(bool enable)
{
  m_shademean = enable;
  update();
}

void HistogramWidget::setShadeSD(double sd)
{
  m_shadesd = sd;
  update();
}
