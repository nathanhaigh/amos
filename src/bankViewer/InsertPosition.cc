#include "InsertPosition.hh"
#include <iostream>
#include <qpainter.h>

using namespace std;

InsertPosition::InsertPosition(QWidget * parent, const char * name)
 : QWidget(parent, name)
{
  setMinimumHeight(30);

  m_start = 0;
  m_scale = 1;
}


void InsertPosition::setVisibleRange(int start, double scale)
{
  m_start = start;
  m_scale = scale;

  repaint();
}

void InsertPosition::paintEvent(QPaintEvent * e)
{
  int linepos = 25;

  QPainter p(this);
  p.setBrush(Qt::black);
  p.drawRect(rect());
  p.setPen(Qt::white);

  p.translate(2, 0); // account for scroll view frame

  p.drawLine(0, linepos, width(), linepos);

  QString pos;

  for (int i = m_start; i < m_start + (width() / m_scale); i++)
  {
    if (i % 500 == 0)
    {
      p.drawLine((i-m_start) * m_scale, linepos-2,
                 (i-m_start) * m_scale, linepos+2);

      if (i % 1000 == 0)
      {
        pos = QString::number(i/1000);
        pos += "k";
        p.drawText((i-m_start) * m_scale - 20, 8, 40, 15,  
                   Qt::AlignHCenter | Qt::AlignBottom, pos);
      }
    }
  }
}

