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
  setFontSize(12);
  setPalette(QPalette(QColor(180, 180, 180)));
}

void ConsensusField::setFontSize(int fontsize)
{
  m_fontsize=fontsize;
  int lineheight  = m_fontsize+5;
  setMinimumHeight(lineheight*2+2); // 2 lineheight + 2
}

void ConsensusField::paintEvent(QPaintEvent * event)
{
  if (m_consensus.empty()) { cerr << "nocons" << endl; return; }

  QPixmap pix(width(), height());
  pix.fill(this, 0,0);

  QPainter p( &pix );
  QPen pen;
  pen.setColor(black);
  p.setPen(pen);

  int lineheight  = m_fontsize+5;
  int tilehoffset = m_fontsize*10+2;
  int seqnamehoffset = 10+2;

  int width = this->width();
  int displaywidth = (width-tilehoffset)/m_fontsize;

  p.setFont(QFont("Helvetica", m_fontsize));

  int grangeStart = m_gindex;
  int grangeEnd = jmin(m_gindex + displaywidth, m_consensus.size()-1);


  QString cname = "Consensus";
  QString pname = "Position";

  int consoffset = lineheight*2;
  int posoffset = lineheight*2;
  int lineoffset = lineheight;

 // p.drawText(seqnamehoffset, posoffset, pname);
  p.drawText(seqnamehoffset, consoffset, cname);

  //x-axis
  p.drawLine(tilehoffset, lineoffset, 
             tilehoffset+(grangeEnd-grangeStart+1)*m_fontsize, lineoffset);

//  cerr << "paintCons [" << grangeStart << "," << grangeEnd << "]" << endl;

  QString pos;
  QString cons;
  for (int j = grangeStart; j <= grangeEnd; j++)
  {
    p.setFont(QFont("Helvetica", m_fontsize));
    char b = m_consensus[j];

    switch (b)
    {
      case 'A': pen.setColor(red); break;
      case 'C': pen.setColor(green); break;
      case 'G': pen.setColor(blue); break;
      case 'T': pen.setColor(yellow); break;
      default: pen.setColor(black); break;
    };

    p.setPen(pen);
    int xcoord = tilehoffset + (j-grangeStart)*m_fontsize;

    QString s;
    s+= b;
    p.drawText(xcoord, consoffset, s);

    // Numbers
    int n = j%10;
    s = QString::number(n);
    pen.setColor(black);
    p.setPen(pen);
    //p.drawText(xcoord, posoffset, s);

    if (n==0)
    {
      int scaledfont = m_fontsize*.75;
      p.setFont(QFont("Helvetica", scaledfont));
      QString spos = QString::number(j);
      p.drawLine(xcoord+m_fontsize/3, lineoffset-5, xcoord+m_fontsize/3, lineoffset+5);
      p.drawText(xcoord+m_fontsize/3-20, lineoffset-5-m_fontsize,40,scaledfont, Qt::AlignHCenter, spos);
    }
    else if (n==5)
    {
      p.drawLine(xcoord+m_fontsize/3, lineoffset-2, xcoord+m_fontsize/3, lineoffset+2);
    }
  }

  p.end();
  p.begin(this);
  p.drawPixmap(0, 0, pix);
}
