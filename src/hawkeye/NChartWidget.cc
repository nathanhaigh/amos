#include "NChartWidget.hh"
#include "NChartStats.hh"

#include <qstring.h>
#include <qpixmap.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QMouseEvent>
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
   m_stats(NULL), m_grid(true), m_clickpoint(-1,-1)
{
  setMinimumSize(250, 250);
 // setWFlags(Qt::WRepaintNoErase | Qt::WDestructiveClose | Qt::WResizeNoErase);

  m_ordering = 0;
  m_colorstyle = 0;

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

class DRect
{
public:
  DRect(double _x, double _y, double _w, double _h) : x(_x), y(_y), w(_w), h(_h) { }

  double x,y,w,h;
};

static int pivotlevel = 1;


static void layoutTreemap(NChartStats * stats, int start, int end, DRect space)
{
  if (start > end) { return; }

  if (start == end)
  {
    stats->m_sizes[start].m_rect = QRect(floor(space.x-.5), floor(space.y-.5), floor(space.w+.5)+2, floor(space.h+.5)+2);
    return;
  }

  // start ... r1pivot .. r2pivot ... end
  // r1: [start, r1pivot)   r1size
  // rp: [r1pivot, r1pivot] rpsize
  // r2: (r1pivot, r2pivot] r2size - rpsize
  // r3: (r2pivot, end]     r3size

  int r1pivot = (int)((start+end) / 2);
  double rpsize = stats->m_sizes[r1pivot].m_size;

  stats->m_sizes[r1pivot].m_pivotlevel = pivotlevel;
  pivotlevel++;

  double r1size = 0; double r3size = 0; double allsize = 0;
  for (int i = start; i <= end;    i++) { allsize += stats->m_sizes[i].m_size; }
  for (int i = start; i < r1pivot; i++) { r1size  += stats->m_sizes[i].m_size; }

  int    r2pivot = r1pivot;
  double bestratio = -1;
  double cumsize = 0;
  double r2size = 0;

  if (space.h < space.w)
  {
    //layout regions into vertical stripes
    for (int r2 = r1pivot; r2 <= end; r2++)
    {
      cumsize += stats->m_sizes[r2].m_size;
      double r2width  = space.w * cumsize / allsize;
      double rpheight = space.h * rpsize / cumsize;

      double rpratio = (rpheight > r2width) ? rpheight / r2width : r2width / rpheight;

      if (bestratio == -1 || rpratio < bestratio)
      {
        r2pivot = r2; r2size = cumsize; bestratio = rpratio;
      }
    }

    for (int i = r2pivot+1; i <= end; i++) { r3size += stats->m_sizes[i].m_size; }

    double r1width = space.w * r1size / allsize;
    double r2width = space.w * r2size / allsize;
    double r3width = space.w * r3size / allsize;

    double rpheight = space.h * rpsize / r2size;

    layoutTreemap(stats, start,     r1pivot-1, DRect(space.x,                 space.y,          r1width, space.h));          // r1
    layoutTreemap(stats, r1pivot,   r1pivot,   DRect(space.x+r1width,         space.y,          r2width, rpheight));         // rp
    layoutTreemap(stats, r1pivot+1, r2pivot,   DRect(space.x+r1width,         space.y+rpheight, r2width, space.h-rpheight)); // r2
    layoutTreemap(stats, r2pivot+1, end,       DRect(space.x+r1width+r2width, space.y,          r3width, space.h));          // r3
  }
  else
  {
    //layout regions into horizontal stripes
    for (int r2 = r1pivot; r2 <= end; r2++)
    {
      cumsize += stats->m_sizes[r2].m_size;
      double r2height = space.h * cumsize / allsize;
      double rpwidth  = space.w * rpsize / cumsize;

      double rpratio = (r2height > rpwidth) ? r2height / rpwidth : rpwidth / r2height;

      if (bestratio == -1 || rpratio < bestratio)
      {
        r2pivot = r2; r2size = cumsize; bestratio = rpratio;
      }
    }

    for (int i = r2pivot+1; i <= end; i++) { r3size += stats->m_sizes[i].m_size; }

    double r1height = space.h * r1size / allsize;
    double r2height = space.h * r2size / allsize;
    double r3height = space.h * r3size / allsize;

    double rpwidth = space.w * rpsize / r2size;

    layoutTreemap(stats, start,     r1pivot-1,   DRect(space.x,         space.y,                   space.w,         r1height)); // r1
    layoutTreemap(stats, r1pivot,   r1pivot,     DRect(space.x,         space.y+r1height,          rpwidth,         r2height)); // rp
    layoutTreemap(stats, r1pivot+1, r2pivot,     DRect(space.x+rpwidth, space.y+r1height,          space.w-rpwidth, r2height)); // r2
    layoutTreemap(stats, r2pivot+1, end,         DRect(space.x,         space.y+r1height+r2height, space.w,         r3height)); // r3

  }
}



static void setFeatRectColor(NChartStats * stats, int i, QColor & rectcolor, int colorstyle)
{
  if (stats->m_maxscore)
  {
    int h,s,v;
    rectcolor.hsv(&h,&s,&v);

    double badness = ((double)stats->m_sizes[i].m_score) / stats->m_maxscore;

    if (colorstyle == 1)
    {
      s = (int)(badness * 255);
    }
    else if (colorstyle == 0)
    {
      h -= badness * (h);
      h %= 360;
    }

    rectcolor.setHsv(h,s,v);
  }
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

  QString label = m_stats->m_label.c_str();

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
    // feature color gradient
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

        p.drawRect((int)(thermoleft+1), (int)(m_histbottom-1-top), (int)8, (int)theight);
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

        p.drawRect(thermoleft+1, (int)(m_histbottom-1-top), 8, (int)(theight));
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

    if (m_ordering < 2)
    {
      // Xlabels
      pen.setStyle(Qt::DotLine);
      p.setPen(pen);

      int labelwidth = 100;
      int numbuckets = (int)(labelwidth / m_xscale);
      if (numbuckets == 0) { numbuckets = 1; }

      for (double i = 12.5; i < 100; i+=12.5)
      {
        int xcoord = (int)(m_histleft+i*m_xscale);
        p.drawLine(xcoord, m_histtop, xcoord, m_histbottom+5);

        if (i == 25.0 || i == 50.0 || i == 75.0)
        {
          label = "  ";
          label += QString::number(i, 'f', 0);
          label += "%";
          p.drawText(xcoord-labelwidth, m_histbottom,
                     labelwidth*2, 30, Qt::AlignCenter, label);
        }
      }

      // Y-labels
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
    }


    // NChart
    pen.setStyle(Qt::SolidLine);
    p.setPen(pen);

    int l = m_stats->m_sizes.size();

    //layout the rectangles

    if (m_ordering == 0 || m_ordering == 1)
    {
      // layout by size, feature density
      double xsizescale = ((double) m_histwidth) / m_stats->m_sum;
      double cumsize = 0;

      for (int i = 0; i < l; i++)
      {
        int xcoord = (int) (cumsize * xsizescale);
        int width  = (int) (m_stats->m_sizes[i].m_size * xsizescale) + 1;
        int height = (int) (m_stats->m_sizes[i].m_size * m_yscale)+1;

        m_stats->m_sizes[i].m_rect.setRect(m_histleft+xcoord, m_histbottom-height, width, height);
        cumsize += m_stats->m_sizes[i].m_size;
      }
    }
    else if (m_ordering == 2)
    {
      pivotlevel = 1;
      layoutTreemap(m_stats, 0, l-1, DRect(0,0,m_histwidth,m_histheight));

      for (int i = 0; i < l; i++)
      {
        m_stats->m_sizes[i].m_rect.moveBy(m_histleft, m_histtop);
      }
    }

    // draw the rectangles
    bool first = true;
    for (int i = 0; i < l; i++)
    {
      QColor rectcolor(baserectcolor);
      setFeatRectColor(m_stats, i, rectcolor, m_colorstyle);
      p.setBrush(rectcolor);

      if (first && m_stats->m_sizes[i].m_rect.contains(m_clickpoint))
      {
        first = false;
        p.setBrush(QColor(99,175,252));

        double perc = ((double)m_stats->m_sizes[i].m_size*100) / m_stats->m_sum;

        QString info = "Selected: " + QString::number(m_stats->m_sizes[i].m_id, 'f', 0) +
                       "  Size: " + QString::number(m_stats->m_sizes[i].m_size, 'f', 0) +
                       "  (" + QString::number(perc, 'f', 2) +
                       "%) " + QString::number(m_stats->m_sizes[i].m_score) +
                       " Features";

        p.drawText(0, textline2,
                   width, 30, Qt::AlignHCenter | Qt::AlignVCenter, info);
      }

      p.drawRect(m_stats->m_sizes[i].m_rect);

      /*
      if (m_ordering == 2 && m_stats->m_sizes[i].m_pivotlevel)
      {
        p.setFont(QFont("Helvetica", 8));
        p.drawText(m_stats->m_sizes[i].m_rect, 
                   Qt::AlignHCenter | Qt::AlignVCenter, 
                   QString::number(m_stats->m_sizes[i].m_pivotlevel));
        p.setFont(QFont("Helvetica", 12));
      }
      */
    }


    // text

    label = "Count: " + QString::number(m_stats->m_sizes.size(), 'f', 0);
    label += "   Max: " + QString::number(m_stats->m_maxsize, 'f', 0);
    label += "   N50: " + QString::number(m_stats->nvalue(50).m_size, 'f', 0);
    label += "   Total: " + QString::number(m_stats->m_sum, 'f', 0);

    p.drawText(0, textline1,
               width, 30, Qt::AlignHCenter | Qt::AlignVCenter, label);

  }

  p.end();

  p.begin(this);
  p.drawPixmap(0,0,pix);
  p.end();
}

void NChartWidget::resizeEvent()
{
  m_clickpoint = QPoint(-1,-1);
}


void NChartWidget::mouseMoveEvent(QMouseEvent * e)
{
  m_clickpoint = e->pos();
  update();
}

void NChartWidget::mouseDoubleClickEvent(QMouseEvent * e)
{
  mouseMoveEvent(e);

  int l = m_stats->m_sizes.size();
  for (int i = 0; i < l; i++)
  {
    if (m_stats->m_sizes[i].m_rect.contains(m_clickpoint))
    {
      emit idSelected(m_stats->m_sizes[i].m_id);
      break;
    }
  }
}
