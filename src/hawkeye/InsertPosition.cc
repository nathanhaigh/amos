#include "InsertPosition.hh"
#include <iostream>
#include <qpainter.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QPaintEvent>
#include "DataStore.hh"

using namespace std;

InsertPosition::InsertPosition(DataStore * datastore, int & scaffoldId,
                               QString & scaffoldName, QWidget * parent, 
                               const char * name)
 : m_scaffoldId(scaffoldId), m_scaffoldName(scaffoldName), QWidget(parent, name), m_datastore(datastore)
{
  setMinimumHeight(60);
  setWindowFlags(Qt::WRepaintNoErase);

  m_start = 0;
  m_scale = 1;
  m_pos = 0;
}


void InsertPosition::setVisibleRange(int start, double scale)
{
  m_start = start;
  m_scale = scale;

  update();
}

void InsertPosition::setScaffoldCoordinate(int pos)
{
  m_pos = pos;
  update();
}

void InsertPosition::paintEvent(QPaintEvent * e)
{
  int linepos = 55;

  QPainter p(this);

  p.setBrush(Qt::black);
  p.drawRect(rect());

  p.setPen(Qt::white);
  p.setFont(QFont("Helvetica", 12));

  int center = (int) width()/2;

  QString label = "Scaffold: ";
  
  if (m_scaffoldName.isEmpty())
  {
    label += QString::number(m_scaffoldId);
  }
  else
  {
    label += m_scaffoldName + " [" + 
             QString::number(m_scaffoldId) + "]";
  }

  label += "  Contig: ";
  QString contigname = m_datastore->contig_bank.lookupEID(m_datastore->m_contigId).c_str();

  if (contigname.isEmpty())
  {
    label += QString::number(m_datastore->m_contigId);
  }
  else
  {
    label += contigname + " [" + 
             QString::number(m_datastore->m_contigId) + "]";
  }

  label += "  Position: " + QString::number(m_pos);

  label += "  Viewing: " + QString::number(m_start, 'f', 0)
        +  " - " + QString::number((int)m_start+(width() / m_scale), 'f', 0);

  p.drawText(center-400, 2, 800, 25, Qt::AlignCenter, label);

  int distance = (int)(100/m_scale); // 100 pixels between tick marks

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

      p.drawText((i-m_start) * m_scale - 50, linepos-20, 100, 20,  
                 Qt::AlignHCenter | Qt::AlignBottom, pos);
    }
  }
}

