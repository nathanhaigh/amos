#include "NChartWindow.hh"
#include "NChartWidget.hh"

#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qbuttongroup.h>

#include "NChartStats.hh"

NChartWindow::NChartWindow(NChartStats * stats, QWidget * parent, const char * name)
 : NChartWindowBase(parent, name)
{
  setWFlags(Qt::WDestructiveClose);

  nChartWidget->setStats(stats);

  connect(normalizationGroup, SIGNAL(clicked(int)), nChartWidget, SLOT(setNormalization(int)));
  connect(orderingGroup,      SIGNAL(clicked(int)), nChartWidget, SLOT(setOrdering(int)));

  setCaption(stats->m_label);
  resize(800,500);
  show();
}
