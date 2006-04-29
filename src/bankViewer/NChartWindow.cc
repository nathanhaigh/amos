#include "NChartWindow.hh"
#include "NChartWidget.hh"

#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>

#include "NChartStats.hh"

NChartWindow::NChartWindow(NChartStats * stats, QWidget * parent, const char * name)
 : QMainWindow(parent, name)
{
  setWFlags(Qt::WDestructiveClose);

  NChartWidget * widget = new NChartWidget(this, "HistoWidget");
  widget->setStats(stats);

  setCentralWidget(widget);
  setCaption(stats->m_label);
  resize(550,500);
  show();
}
