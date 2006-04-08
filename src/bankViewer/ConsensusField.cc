#include "ConsensusField.hh"
#include <qpixmap.h>
#include <qpen.h>
#include <qpainter.h>
#include "UIElements.hh"
#include "AlignmentInfo.hh"

using namespace std;


static int min (int a, int b)
{
  return a < b ? a : b;
}
static int max (int a, int b)
{
  return a > b ? a : b;
}

static int m_minheight = 8;

ConsensusField::ConsensusField(const string & cons,
                               const string & cstatus,
                               const string & consqual,
                               const vector<int> & ugpos,
                               AlignmentInfo * ai,
                               int & gindex,
                               QWidget * parent,
                               const char * name)
   :QFrame(parent, name),
    m_consensus(cons),
    m_cstatus(cstatus),
    m_consqual(consqual),
    m_ugpos(ugpos),
    m_alignment(ai),
    m_gindex(gindex)
{
  m_shownumbers = 0;
  m_highlightdiscrepancy = 0;
  m_showIndicator = true;
  m_basecolors = false;

  m_diam = 5;
  m_basespace = 5;

  setFontSize(10);
  setPalette(QPalette(UIElements::color_tiling));

  m_rangestart = -1;
  m_rangeend = -1;
  m_displayQV = false;
  m_showUngapped = false;
}

void ConsensusField::setFontSize(int fontsize)
{
  m_fontsize=fontsize;

  int theight = max(m_fontsize, m_minheight);


  int gutter         = theight;
  int framegutter    = 2;

  m_lineheight     = theight + gutter;

  m_tilehoffset    = theight*12 + framegutter;
  m_seqnamehoffset = gutter + framegutter;
  m_basewidth      = m_fontsize + m_basespace;

  if (m_basewidth <= 0)
  {
    m_basewidth = 1/(-m_basewidth+2);
  }

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

void ConsensusField::toggleShowNumbers(bool doShow)
{
  m_shownumbers = doShow;
  setFontSize(m_fontsize);
}

void ConsensusField::paintEvent(QPaintEvent * event)
{
  int theight = max(m_fontsize, m_minheight);

  if (m_consensus.empty()) 
  { 
    QPainter p (this);
    p.drawText(20, theight+theight/2, "No Contig Loaded");
    return;
  }

  QPixmap pix(width(), height());
  pix.fill(this, 0,0);

  QPainter p( &pix );
  QPen pen;
  pen.setColor(black);
  p.setPen(pen);
  p.setFont(QFont("Helvetica", theight));

  int width = this->width();
  int displaywidth = (int)((width-m_tilehoffset)/m_basewidth);

  p.drawText(m_seqnamehoffset, m_consoffset,
             m_tilehoffset - m_seqnamehoffset, m_lineheight,
             Qt::AlignLeft | Qt::AlignBottom, "Consensus");

  int grangeStart = m_gindex;
  int grangeEnd = min(m_gindex + displaywidth, m_consensus.size()-1);

  //x-axis
  p.drawLine(m_tilehoffset, m_lineoffset, 
             (int)(m_tilehoffset+(grangeEnd-grangeStart+1)*m_basewidth), m_lineoffset);

  QString s;

  if (!(m_rangeend < grangeStart || m_rangestart > grangeEnd))
  {
    int drawStart = max(m_rangestart, grangeStart);
    int drawEnd = min(m_rangeend, grangeEnd);

    p.setPen(Qt::red);

    p.drawRect(m_tilehoffset + (int)((drawStart - grangeStart)*m_basewidth - 1), 
               m_consoffset,
               (int)((drawEnd - drawStart + 1) * m_basewidth - m_basespace + 3),
               m_lineheight);

    p.setPen(Qt::black);
  }


  int fudge = 0;
  if (m_displayQV) { fudge = theight*.25; }

  for (int gindex = grangeStart; gindex <= grangeEnd; gindex++)
  {
    int shifted = gindex;
    if (m_alignment)
    {
      shifted = m_alignment->getContigPos(gindex);
    }

    char b = m_consensus[gindex];
    s = b;

    p.setFont(QFont("Helvetica", theight));
    if (m_basecolors)
    {
      UIElements::setBasePen(pen, b);
      p.setPen(pen);
    }

    int xcoord = (int)(m_tilehoffset + (gindex-grangeStart)*m_basewidth);
    int bwidth = max(m_basewidth, 1);

    if (m_fontsize < m_minheight)
    {
      if (m_cstatus[gindex] == 'X')
      {
        p.setBrush(UIElements::getBaseColor(b));
        p.drawRect(xcoord, m_consoffset, bwidth, m_lineheight-1);
      }
    }
    else
    {
      p.drawText(xcoord, m_consoffset, 
                 theight, m_lineheight, 
                 Qt::AlignHCenter | Qt::AlignBottom, s);

      if (m_displayQV)
      {
        p.setPen(Qt::black);
        p.setFont(QFont("Helvetica", theight*.6));

        b = m_consqual[gindex];
        s = QString::number(b-AMOS::MIN_QUALITY);

        p.drawText(xcoord, m_consoffset-theight-5,
                   theight, m_lineheight,
                   Qt::AlignHCenter | Qt::AlignBottom, s);
      }

      if (m_cstatus[gindex] == 'X' || m_cstatus[gindex] == '*')
      {
        p.setBrush(Qt::SolidPattern);
        if (m_cstatus[gindex] == '*')
        {
          p.setPen(Qt::red);
          p.setBrush(Qt::red);
        }
        else
        {
          p.setPen(Qt::black);
          p.setBrush(Qt::black);
        }


        p.drawEllipse(xcoord+theight/2-m_diam/2-1, m_discoffset-fudge,
                      m_diam, m_diam);
                      
        if (m_highlightdiscrepancy)
        {
          p.setBrush(Qt::NoBrush);
          p.setPen(UIElements::color_discrepancy);
          p.drawRect(xcoord, m_consoffset, theight, m_lineheight-1);
        }
      }
    }

    p.setPen(Qt::black);
    int scaledfont = (int)max((int)(theight*.6), 6);
    p.setFont(QFont("Helvetica", scaledfont));

    int nbreak = 10;
    int jbreak = 5;

    if (m_fontsize < 1)
    {
      nbreak = 100;
      jbreak = 25;

      if (m_fontsize < -5) { nbreak = 500; jbreak = 100; }
    }

    int n = shifted%nbreak;
    int j = shifted%jbreak;
    if (m_showUngapped) { n = m_ugpos[shifted] % nbreak; j = m_ugpos[shifted] % jbreak; }

    if (m_shownumbers && m_fontsize >= m_minheight)
    {
      // Numbers
      s = QString::number(n);
      p.drawText(xcoord, m_posoffset, 
                 theight, 2*theight,
                 Qt::AlignHCenter | Qt::AlignCenter, s);
    }

    if (n==0 && m_consensus[gindex] != '*' && (!m_showUngapped || m_consensus[gindex] != '-'))
    {
      if (m_showUngapped)
      {
        s = QString::number(m_ugpos[gindex]);
      }
      else
      {
        s = QString::number(m_alignment->getContigPos(gindex));
      }

      int xpos = xcoord + m_fontsize/2;
      if (m_fontsize <= 0) { xpos = xcoord; }

      p.drawLine(xpos, m_lineoffset-2, 
                 xpos, m_lineoffset+2);

      p.drawText(xpos-50, 2,
                 100, theight*2, 
                 Qt::AlignHCenter | Qt::AlignCenter, s);
    }
    else if (j==0 && m_consensus[gindex] != '*' && (!m_showUngapped || m_consensus[gindex] != '-'))
    {
      int xpos = xcoord + m_fontsize/2;
      if (m_fontsize <= 0) { xpos = xcoord; }

      p.drawLine(xpos, m_lineoffset-1, 
                 xpos, m_lineoffset+1);
    }


    if (m_showIndicator && m_fontsize >= m_minheight)
    {
      QPointArray indicator (3);

      int hbase = (int)(m_tilehoffset + 10*m_basewidth + .25*m_fontsize);
      int hstep = (int)(.25*m_fontsize);

      indicator[0] = QPoint(hbase, 1);
      indicator[1] = QPoint(hbase + 2*hstep+1, 1);
      indicator[2] = QPoint(hbase + hstep, 5);

      p.setPen(Qt::black);
      p.setBrush(Qt::black);
      p.drawPolygon(indicator);
    }
  }

  p.end();
  p.begin(this);
  p.drawPixmap(0, 0, pix);
}

void ConsensusField::toggleHighlightDiscrepancy(bool show)
{
  //m_highlightdiscrepancy = show;
  //update();
}

void ConsensusField::toggleBaseColors(bool show)
{
  m_basecolors = show;
  update();
}

void ConsensusField::toggleShowIndicator(bool show)
{
  m_showIndicator = show;
  update();
}

void ConsensusField::toggleShowConsQV(bool show)
{
  m_displayQV = show;
  update();
}

void ConsensusField::toggleShowUngapped(bool use)
{
  m_showUngapped = use;
  update();
}

void ConsensusField::mouseReleaseEvent( QMouseEvent * e)
{
  int gindex = (int)(m_gindex + (e->x() - m_tilehoffset)/m_basewidth);
  emit sortColumns(gindex);
}

void ConsensusField::setHighlightRange(int start, int end)
{
  m_rangestart = start;
  m_rangeend = end;
}



