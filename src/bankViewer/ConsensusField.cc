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
                               int & fontsize,
                               QWidget * parent,
                               const char * name)
   :QWidget(parent, name),
    m_consensus(cons),
    m_gindex(gindex),
    m_fontsize(fontsize)
{
  setMinimumHeight(50);
  setPalette( QPalette( QColor( 200, 200, 200) ) );
}

void ConsensusField::paintEvent(QPaintEvent * event)
{
  cerr << "paintCons" << endl;
  if (m_consensus.empty()) { return; }

  QPixmap pix(width(), height());
  pix.fill(this, 0,0);

  QPainter p( &pix );
  QPen pen;
  pen.setColor(black);
  p.setPen(pen);

  int lineheight  = m_fontsize+5;
  int tilehoffset = m_fontsize*10;

  int width = this->width();
  int displaywidth = (width-tilehoffset)/m_fontsize;

  p.setFont(QFont("Courier", m_fontsize));

  int grangeStart = m_gindex;
  int grangeEnd = jmin(m_gindex + displaywidth, m_consensus.size()-1);

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
