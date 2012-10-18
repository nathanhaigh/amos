#include "ConsensusField.hh"
#include <qpixmap.h>
#include <qpen.h>
#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <QPaintEvent>
#include <Q3Frame>
#include <QMouseEvent>
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
                               int & contigid,
                               QString & contigname,
                               QWidget * parent,
                               const char * name)
   :Q3Frame(parent, name),
    m_consensus(cons),
    m_cstatus(cstatus),
    m_consqual(consqual),
    m_ugpos(ugpos),
    m_alignment(ai),
    m_gindex(gindex),
    m_contigId(contigid),
    m_contigName(contigname),
    m_readnamewidth(11)
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

  m_tilehoffset    = theight*(m_readnamewidth+1) + framegutter;
  m_seqnamehoffset = gutter + framegutter;
  m_basewidth      = m_fontsize + m_basespace;

  if (m_packreads)
  {
    m_tilehoffset = 2+framegutter;
  }

  if (m_basewidth <= 0)
  {
    m_basewidth = 1/(-m_basewidth+2);
  }

  m_lineoffset = m_lineheight*2;
  m_posoffset  = m_lineheight*2 + gutter; // hidden by default
  m_discoffset = m_lineheight*2 + m_diam + m_diam;
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

#include <QtGui>

void ConsensusField::drawContents(QPainter * p)
{
  int theight = max(m_fontsize, m_minheight);

  if (m_consensus.empty()) 
  { 
    p->drawText(20, theight+theight/2, "No Contig Loaded");
    return;
  }

  int width = this->width();

  QColor bgcolor = UIElements::color_tilingoffset;
  bgcolor = bgcolor.light(150); 
  
  p->fillRect(rect(), bgcolor);

  p->setRenderHint(QPainter::Antialiasing);
  p->setRenderHint(QPainter::TextAntialiasing); 

  QPen pen;
  pen.setColor(Qt::black);
  p->setPen(pen);
  p->setFont(QFont("Helvetica", theight));

  int displaywidth = (int)((width-m_tilehoffset)/m_basewidth);


  if (!m_packreads)
  {
    p->drawText(m_seqnamehoffset, m_consoffset,
               m_tilehoffset - m_seqnamehoffset, m_lineheight,
               Qt::AlignLeft | Qt::AlignBottom, "Consensus");
  }

  int grangeStart = m_gindex;
  int grangeEnd = min(m_gindex + displaywidth, m_consensus.size()-1);

  QString s = "Viewing Contig ";
  if (!m_contigName.isEmpty())
  {
    s += m_contigName + " [" + QString::number(m_contigId) + "]";
  }
  else
  {
    s += QString::number(m_contigId);
  }
    
  s += " from ";

  if (m_showUngapped)
  {
    s += QString::number(m_ugpos[grangeStart]) + " to " + QString::number(m_ugpos[grangeEnd]) + " of " + QString::number(m_ugpos[m_consensus.size()-1]);
  }
  else
  {
    s += QString::number(grangeStart)          + " to " + QString::number(grangeEnd)          + " of " + QString::number(m_consensus.size());
  }

  p->drawText(width/2 - 400, 2, 800, m_lineheight, Qt::AlignHCenter | Qt::AlignBottom, s);

  //x-axis
  p->drawLine(m_tilehoffset, m_lineoffset, 
             (int)(m_tilehoffset+(grangeEnd-grangeStart+1)*m_basewidth), m_lineoffset);

  if (!(m_rangeend < grangeStart || m_rangestart > grangeEnd))
  {
    int drawStart = max(m_rangestart, grangeStart);
    int drawEnd = min(m_rangeend, grangeEnd);

    p->setPen(Qt::red);

    p->drawRect(m_tilehoffset + (int)((drawStart - grangeStart)*m_basewidth - 1), 
               m_consoffset,
               (int)((drawEnd - drawStart + 1) * m_basewidth - m_basespace + 3),
               m_lineheight);

    p->setPen(Qt::black);
  }


  int fudge = 0;
  if (m_displayQV) { fudge = (int) (theight*.25); }

  for (int gindex = grangeStart; gindex <= grangeEnd; gindex++)
  {
    int shifted = gindex;
    if (m_alignment)
    {
      shifted = m_alignment->getContigPos(gindex);
    }

    char b = m_consensus[gindex];
    s = b;

    p->setFont(QFont("Helvetica", theight));
    if (m_basecolors)
    {
      UIElements::setBasePen(pen, b);
      p->setPen(pen);
    }

    int xcoord = (int)(m_tilehoffset + (gindex-grangeStart)*m_basewidth);
    int bwidth = max((int) m_basewidth, 1);

    if (m_fontsize < m_minheight)
    {
      if (m_cstatus[gindex] == 'X')
      {
        p->setBrush(UIElements::getBaseColor(b));
        p->drawRect(xcoord, m_consoffset, bwidth, m_lineheight-1);
      }
    }
    else
    {
      p->drawText(xcoord, m_consoffset, 
                 theight, m_lineheight, 
                 Qt::AlignHCenter | Qt::AlignBottom, s);

      if (m_displayQV)
      {
        p->setPen(Qt::black);
        p->setFont(QFont("Helvetica", (int)(theight*.6)));

        b = m_consqual[gindex];
        s = QString::number(b-AMOS::MIN_QUALITY);

        p->drawText(xcoord, m_consoffset-theight-5,
                   theight, m_lineheight,
                   Qt::AlignHCenter | Qt::AlignBottom, s);
      }

      if (m_cstatus[gindex] == 'X' || m_cstatus[gindex] == '*')
      {
        p->setBrush(Qt::SolidPattern);
        if (m_cstatus[gindex] == '*')
        {
          p->setPen(Qt::red);
          p->setBrush(Qt::red);
        }
        else
        {
          p->setPen(Qt::black);
          p->setBrush(Qt::black);
        }


        qreal diam = m_diam;
        p->drawEllipse(xcoord+theight/2-m_diam/2-1, m_discoffset-fudge,
                      diam, diam);
                      
        if (m_highlightdiscrepancy)
        {
          p->setBrush(Qt::NoBrush);
          p->setPen(UIElements::color_discrepancy);
          p->drawRect(xcoord, m_consoffset, theight, m_lineheight-1);
        }
      }
    }

    p->setPen(Qt::black);
    int scaledfont = (int)max((int)(theight*.8), 8);
    p->setFont(QFont("Helvetica", scaledfont));

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
      p->drawText(xcoord, m_posoffset, 
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

      p->drawLine(xpos, m_lineoffset-2, 
                 xpos, m_lineoffset+2);

      p->drawText(xpos-50, m_lineheight,
                 100, theight*2, 
                 Qt::AlignHCenter | Qt::AlignCenter, s);
    }
    else if (j==0 && m_consensus[gindex] != '*' && (!m_showUngapped || m_consensus[gindex] != '-'))
    {
      int xpos = xcoord + m_fontsize/2;
      if (m_fontsize <= 0) { xpos = xcoord; }

      p->drawLine(xpos, m_lineoffset-1, 
                 xpos, m_lineoffset+1);
    }


    if (m_showIndicator && m_fontsize >= m_minheight)
    {
      Q3PointArray indicator (3);

      int hbase = (int)(m_tilehoffset + 10*m_basewidth + .25*m_fontsize);
      int hstep = (int)(.25*m_fontsize);

      indicator[0] = QPoint(hbase, 1);
      indicator[1] = QPoint(hbase + 2*hstep+1, 1);
      indicator[2] = QPoint(hbase + hstep, 5);

      p->setPen(Qt::black);
      p->setBrush(Qt::black);
      p->drawPolygon(indicator);
    }
  }
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

void ConsensusField::togglePackReads(bool pack)
{
  m_packreads = pack;
  setFontSize(m_fontsize);
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
  if (e->x() < (m_tilehoffset - m_basewidth))
  {
    emit sortColumns(-1);
  }
  else
  {
    int gindex = (int)(m_gindex + (e->x() - m_tilehoffset)/m_basewidth);
    emit sortColumns(gindex);
  }
}

void ConsensusField::setHighlightRange(int start, int end)
{
  m_rangestart = start;
  m_rangeend = end;
}

void ConsensusField::setReadnameWidth(int width)
{
  m_readnamewidth = width;
  setFontSize(m_fontsize);
  update();
}



