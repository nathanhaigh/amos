#include "ConsensusField.hh"
#include <qpixmap.h>
#include <qpen.h>
#include <qpainter.h>
#include "UIElements.hh"

using namespace std;


int jmin (int a, int b)
{
  return a < b ? a : b;
}
int jmax (int a, int b)
{
  return a > b ? a : b;
}

ConsensusField::ConsensusField(const string & cons,
                               const string & cstatus,
                               int & gindex,
                               QWidget * parent,
                               const char * name)
   :QFrame(parent, name),
    m_consensus(cons),
    m_cstatus(cstatus),
    m_gindex(gindex)
{
  m_shownumbers = 0;
  m_highlightdiscrepancy = 0;

  m_diam = 5;
  m_basespace = 5;

  setFontSize(12);
  setPalette(QPalette(UIElements::color_tiling));
}

void ConsensusField::setFontSize(int fontsize)
{
  m_fontsize=fontsize;
  int gutter         = m_fontsize/2;
  int framegutter    = 2;

  m_lineheight     = m_fontsize + gutter;

  m_tilehoffset    = m_fontsize*12 + framegutter;
  m_seqnamehoffset = gutter + framegutter;
  m_basewidth      = m_fontsize + m_basespace;

  m_lineoffset = m_lineheight;
  m_posoffset  = m_lineheight + gutter; // hidden by default
  m_discoffset = m_lineheight + m_diam + m_diam;
  m_consoffset = m_discoffset + m_diam;

  if (m_shownumbers) 
  { 
    m_consoffset += m_lineheight;
    m_discoffset += m_lineheight;
  }

  setMinimumHeight(m_consoffset+m_lineheight+2);
}

void ConsensusField::setShowNumbers(bool doShow)
{
  m_shownumbers = doShow;
  setFontSize(m_fontsize);
}

void ConsensusField::paintEvent(QPaintEvent * event)
{
  if (m_consensus.empty()) 
  { 
    QPainter p (this);
    p.drawText(20, m_fontsize+m_fontsize/2, "No Contig Loaded");
    return;
  }

  QPixmap pix(width(), height());
  pix.fill(this, 0,0);

  QPainter p( &pix );
  QPen pen;
  pen.setColor(black);
  p.setPen(pen);
  p.setFont(QFont("Helvetica", m_fontsize));

  int width = this->width();
  int displaywidth = (width-m_tilehoffset)/m_basewidth;

  p.drawText(m_seqnamehoffset, m_consoffset,
             200, m_lineheight,
             Qt::AlignLeft | Qt::AlignBottom, "Consensus");

  int grangeStart = m_gindex;
  int grangeEnd = jmin(m_gindex + displaywidth, m_consensus.size()-1);

  //x-axis
  p.drawLine(m_tilehoffset, m_lineoffset, 
             m_tilehoffset+(grangeEnd-grangeStart+1)*m_basewidth, m_lineoffset);
  QString s;

  for (int gindex = grangeStart; gindex <= grangeEnd; gindex++)
  {
    char b = m_consensus[gindex];
    s = b;

    p.setFont(QFont("Helvetica", m_fontsize));
    UIElements::setBasePen(pen, b);
    p.setPen(pen);

    int xcoord = m_tilehoffset + (gindex-grangeStart)*m_basewidth;

    p.drawText(xcoord, m_consoffset, 
               m_fontsize, m_lineheight, 
               Qt::AlignHCenter | Qt::AlignBottom, s);

    if (m_cstatus[gindex] == 'X')
    {
      p.setBrush(Qt::SolidPattern);
      p.setPen(Qt::black);
      p.setBrush(Qt::black);
      p.drawEllipse(xcoord+m_fontsize/2-m_diam/2-1, m_discoffset,
                    m_diam, m_diam);
                    
      if (m_highlightdiscrepancy)
      {
        p.setBrush(Qt::NoBrush);
        p.setPen(UIElements::color_discrepancy);
        p.drawRect(xcoord, m_consoffset, m_fontsize, m_lineheight-1);
      }
    }

    p.setPen(Qt::black);

    int n = gindex%10;
    int scaledfont = (int)jmax((int)(m_fontsize*.6), 6);
    p.setFont(QFont("Helvetica", scaledfont));

    if (m_shownumbers)
    {
      // Numbers
      s = QString::number(n);
      p.drawText(xcoord, m_posoffset, 
                 m_fontsize, m_fontsize,
                 Qt::AlignHCenter | Qt::AlignBottom, s);
    }

    // ticks and labels
    if (n==0)
    {
      s = QString::number(gindex);

      p.drawLine(xcoord+m_fontsize/2, m_lineoffset-2, 
                 xcoord+m_fontsize/2, m_lineoffset+2);

      p.drawText(xcoord+m_fontsize/2-30, 2,
                 60, m_fontsize, 
                 Qt::AlignHCenter | Qt::AlignBottom, s);
    }
    else if (n==5)
    {
      p.drawLine(xcoord+m_fontsize/2, m_lineoffset-2, 
                 xcoord+m_fontsize/2, m_lineoffset+2);
    }
  }

  p.end();
  p.begin(this);
  p.drawPixmap(0, 0, pix);
}

void ConsensusField::toggleHighlightDiscrepancy(bool show)
{
  //m_highlightdiscrepancy = show;
  //repaint();
}
