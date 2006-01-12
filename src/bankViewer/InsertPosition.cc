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

  update();
}

void InsertPosition::paintEvent(QPaintEvent * e)
{
  int linepos = 21;

  QPainter p(this);
  p.setBrush(Qt::black);
  p.drawRect(rect());
  p.setPen(Qt::white);

  int distance = (int)(100/m_scale); // 50 pixels between tick marks

  double sd = distance;
  int num = 0;

  while (sd > 10)
  {
    sd /= 10;
    num++;
  }

  distance = (int) sd;
  while (num)
  {
    distance *= 10;
    num--;
  }

  p.translate(2, 0); // account for scroll view frame

  p.drawLine(0, linepos, width(), linepos);

  QString pos;

  for (int i = m_start; i < m_start + (width() / m_scale); i++)
  {
    if (i % distance == 0)
    {
      p.drawLine((i-m_start) * m_scale, linepos-2,
                 (i-m_start) * m_scale, linepos+2);

      if (abs(i) > 1000000)
      {
        pos = QString::number(i/1000000.0) + "M";
      }
      else if (abs(i) > 1000)
      {
        pos = QString::number(i/1000.0) + "K";
      }
      else
      {
        pos = QString::number(i);
      }

      p.drawText((i-m_start) * m_scale - 50, 4, 100, 15,  
                 Qt::AlignHCenter | Qt::AlignBottom, pos);
    }
  }
}

