#include "NChartWindow.hh"
#include "NChartWidget.hh"

#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3buttongroup.h>

#include "NChartStats.hh"

NChartWindow::NChartWindow(NChartStats * stats, QWidget * parent, const char * name)
 : NChartWindowBase(parent, name)
{
  nChartWidget->setStats(stats);

  connect(orderingGroup,      SIGNAL(clicked(int)), nChartWidget, SLOT(setOrdering(int)));
  connect(colorGroup,         SIGNAL(clicked(int)), nChartWidget, SLOT(setColorStyle(int)));

  setCaption(stats->m_label.c_str());
  resize(800,500);
  show();
}
