#include "ChromoField.hh"
#include <qpainter.h>
#include <qpen.h>
#include <math.h>
#include <qstring.h>
#include <string>


ChromoField::ChromoField(RenderSeq_t * read, string & db, QWidget *parent, const char *name)
  :QWidget(parent, name)
{
  setPalette(QPalette(QColor(200, 200, 200)));


  string readname = read->m_read.getEID();

  string path = "/local/chromo/Chromatograms/";
  path += db + "/ABISSed/" + readname[0]+readname[1]+readname[2] + "/"
                           + readname[0]+readname[1]+readname[2]+readname[3] + "/"
                           + readname[0]+readname[1]+readname[2]+readname[3]+readname[4]+ "/" 
                           + readname;

  //cerr << readname << " from " << db << endl;
  //cerr << "path: " << path << endl;

  m_render = read;
  m_rawread = read_reading((char *)path.c_str(), TT_ANY);

  resize (m_rawread->NPoints+100,160);
}

void ChromoField::paintEvent(QPaintEvent * event)
{
  QPainter painter(this);

  int baseline = 100;
  int offset = 20;

  int i=0;
  int vscale=24;

  // x-axis
  painter.drawLine(offset,baseline,m_rawread->NPoints+offset,baseline);

  // x-ticks
  for (i = 0; i < m_rawread->NPoints; i+=10)
  {
    painter.drawLine(i+offset,baseline,i+offset,baseline-5);
    if (!(i % 50))
    {
      painter.drawLine(i+offset, baseline+5, i+offset, baseline);
    }
  }

  // x-labels
  for (i=0; i < m_rawread->NPoints; i += 50)
  {
    QString s = QString::number(i);
    painter.drawText(i-20+offset,baseline+10,40,20,Qt::AlignHCenter,s);
  }

  for (int channel = 0; channel < 4; channel++)
  {
    QPen pen;

    unsigned short * trace = NULL;
    switch (channel)
    {
      case 0: trace = m_rawread->traceA; pen.setColor(red); break;
      case 1: trace = m_rawread->traceC; pen.setColor(green); break;
      case 2: trace = m_rawread->traceG; pen.setColor(blue); break;
      case 3: trace = m_rawread->traceT; pen.setColor(yellow); break;
    };

    painter.setPen(pen);

    painter.moveTo(offset,baseline);
    for (i = 0; i < m_rawread->NPoints; i++)
    {
      if (trace[i])
      {
        painter.lineTo(i+offset,baseline-(trace[i])/vscale);
      }
      else
      {
        painter.moveTo(i+offset,baseline);
      }
    }
  }

  painter.end();
}

