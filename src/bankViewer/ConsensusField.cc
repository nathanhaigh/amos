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
  setMinimumHeight(fontsize*2+12); // 2 lineheight + 2
  m_fontsize=fontsize;
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

  p.drawText(seqnamehoffset, lineheight, pname);
  p.drawText(seqnamehoffset, lineheight*2, cname);

//  cerr << "paintCons [" << grangeStart << "," << grangeEnd << "]" << endl;

  QString pos;
  QString cons;
  for (int j = grangeStart; j <= grangeEnd; j++)
  {
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

    QString s;
    s+= b;
    p.drawText(tilehoffset + (j-grangeStart)*m_fontsize, lineheight*2, s);

    // Numbers
    s = QString::number(j%10);
    pen.setColor(black);
    p.setPen(pen);
    p.drawText(tilehoffset + (j-grangeStart)*m_fontsize, lineheight, s);
  }

  p.end();
  p.begin(this);
  p.drawPixmap(0, 0, pix);
}
