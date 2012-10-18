#include "HistogramWindow.hh"
#include "HistogramWidget.hh"
#include "InsertStats.hh"

#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>

HistogramWindow::HistogramWindow(InsertStats * stats, QWidget * parent, const char * name)
 : Q3MainWindow(parent, name),
   m_stats(stats)
{
  //setWindowFlags(Qt::WDestructiveClose);

  HistogramWidget * widget = new HistogramWidget(stats, this, "HistoWidget");
  m_stats->histogram(100,true);

  Q3ToolBar * options = new Q3ToolBar(this, "options");
  options->setLabel("Options");

  new QLabel("Buckets", options, "bucketlbl");
  QSpinBox * spin = new QSpinBox(50,1000,50, options, "bucketlbl");
  spin->setValue(200);

  connect(spin,   SIGNAL(valueChanged(int)),
          widget, SLOT(setBucketCount(int)));
          
#if 0
  new QLabel(" Fit Zero", options, "fitlbl");
  QCheckBox * fit = new QCheckBox(options, "fitcheck");
  fit->setChecked(true);

  connect(fit,  SIGNAL(toggled(bool)),
          widget, SLOT(setFitZero(bool)));

  new QLabel(" Gridlines", options, "gridlbl");
  QCheckBox * grid = new QCheckBox(options, "gridcheck");
  grid->setChecked(true);

  connect(grid,  SIGNAL(toggled(bool)),
          widget, SLOT(setEnableGrid(bool)));

#endif


  new QLabel(" Shade Mean", options, "meanlbl");
  QCheckBox * mean = new QCheckBox(options, "meancheck");
  mean->setChecked(true);

  connect(mean,   SIGNAL(toggled(bool)),
          widget, SLOT(setShadeMean(bool)));


  new QLabel(" SD:", options, "bucketlbl");
  m_sdpick = new QLineEdit(options, "sdpick");
  m_sdpick->setText(QString::number(2));

  connect(m_sdpick,   SIGNAL(returnPressed()),
          this,       SLOT(loadShadeSD()));

  connect(this,       SIGNAL(setShadeSD(double)),
          widget,     SLOT(setShadeSD(double)));

  setCentralWidget(widget);
  setCaption(stats->m_label.c_str());
  resize(550,500);
  show();
}

void HistogramWindow::loadShadeSD()
{
  emit setShadeSD(atof(m_sdpick->text().ascii()));
}

