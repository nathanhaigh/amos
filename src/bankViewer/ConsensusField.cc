#include "ConsensusField.hh"
#include <qpixmap.h>
#include <qpen.h>
#include <qpainter.h>


int jmin (int a, int b)
{
  return a < b ? a : b;
}

ConsensusField::ConsensusField(string & cons,
                               int & gindex,
                               QWidget * parent,
                               const char * name)
   :QFrame(parent, name),
    m_consensus(cons),
    m_gindex(gindex)
{
  m_shownumbers = 1;
  setFontSize(12);
  setPalette(QPalette(QColor(160, 160, 160)));
}

void ConsensusField::setFontSize(int fontsize)
{
  m_fontsize=fontsize;
  int lineheight  = m_fontsize+5;

  if (m_shownumbers) { setMinimumHeight(3*lineheight+2); }
  else               { setMinimumHeight(2*lineheight+2); }
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

  int gutter         = 5;
  int framegutter    = 2;
  int lineheight     = m_fontsize + gutter;
  int tilehoffset    = m_fontsize*12 + framegutter;
  int seqnamehoffset = gutter + framegutter;
  int extraheight    = 3;

  int width = this->width();
  int displaywidth = (width-tilehoffset)/m_fontsize;

  p.setFont(QFont("Helvetica", m_fontsize));

  int grangeStart = m_gindex;
  int grangeEnd = jmin(m_gindex + displaywidth, m_consensus.size()-1);


  QString cname = "Consensus";
  QString pname = "Position";

  int consoffset = lineheight;
  int posoffset  = lineheight;
  int lineoffset = lineheight;

  if (m_shownumbers) { p.drawText(seqnamehoffset, posoffset+lineheight, pname); }

  if (m_shownumbers) { consoffset = 2*lineheight; }
  p.drawText(seqnamehoffset, consoffset+lineheight, cname);

  //x-axis
  p.drawLine(tilehoffset, lineoffset, 
             tilehoffset+(grangeEnd-grangeStart+1)*m_fontsize, lineoffset);

//  cerr << "paintCons [" << grangeStart << "," << grangeEnd << "]" << endl;

  QString pos;
  QString cons;
  QString s; 

  for (int j = grangeStart; j <= grangeEnd; j++)
  {
    p.setFont(QFont("Helvetica", m_fontsize));
    char b = m_consensus[j];

    switch (b)
    {
      case 'A': pen.setColor(darkGreen); break;
      case 'C': pen.setColor(blue); break;
      case 'G': pen.setColor(yellow); break;
      case 'T': pen.setColor(red); break;
      default:  pen.setColor(black); break;
    };

    p.setPen(pen);
    int xcoord = tilehoffset + (j-grangeStart)*m_fontsize;

    s = b;
    p.drawText(xcoord, consoffset, 
               m_fontsize, m_fontsize+gutter+extraheight, 
               Qt::AlignHCenter | Qt::AlignBottom, s);

    pen.setColor(black);
    p.setPen(pen);

    int n = j%10;

    if (m_shownumbers)
    {
      // Numbers
      s = QString::number(n);
      p.drawText(xcoord, posoffset, 
                 m_fontsize, m_fontsize+gutter+extraheight, 
                 Qt::AlignHCenter | Qt::AlignBottom, s);
    }

    // ticks and labels
    if (n==0)
    {
      int scaledfont = (int)(m_fontsize*.75);
      p.setFont(QFont("Helvetica", scaledfont));
      QString spos = QString::number(j);

      p.drawLine(xcoord+m_fontsize/2, lineoffset-5, 
                 xcoord+m_fontsize/2, lineoffset+5);

      p.drawText(xcoord+m_fontsize/2-30, lineoffset-5-m_fontsize,
                 60, m_fontsize, 
                 Qt::AlignHCenter, spos);
    }
    else if (n==5)
    {
      p.drawLine(xcoord+m_fontsize/2, lineoffset-2, 
                 xcoord+m_fontsize/2, lineoffset+2);
    }
  }

  p.end();
  p.begin(this);
  p.drawPixmap(0, 0, pix);
}
