#include "MainWindow.hh"

#include <qapplication.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qscrollview.h>
#include <qlineedit.h>
#include <qcheckbox.h>

#include "TilingFrame.hh"

MainWindow::MainWindow( QWidget *parent, const char *name )
           : QWidget( parent, name )
{
  // Menubar
  QMenuBar* menubar = new QMenuBar(this);
  Q_CHECK_PTR( menubar );

  QPopupMenu* file = new QPopupMenu( menubar );
  Q_CHECK_PTR( file );
  menubar->insertItem( "&File", file );
  file->insertItem( "&Open Bank...", this,  SLOT(openBank()) );
  file->insertItem( "&Quit", qApp,  SLOT(quit()) );

  // Statusbar
  QLabel * statusbar = new QLabel(this);
  statusbar->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  statusbar->setText("No Bank Loaded");

  // Widgets
  m_contigid = new QSpinBox(1, 1, 1, this, "contigid");
  m_gindex   = new QSpinBox(0,100, 1, this, "gindexspin");

  QSpinBox * fontsize  = new QSpinBox(6, 24, 1, this, "fontsize");
  QLineEdit * dbpick   = new QLineEdit("DMG", this, "dbpick");
  TilingFrame * tiling = new TilingFrame(this, "tilingframe");

  QCheckBox * stable       = new QCheckBox("Stable Tiling", this, "stable");
  QCheckBox * shownumbers  = new QCheckBox("Show Position", this, "consnumbers");
  QCheckBox * hldisc       = new QCheckBox("Highlight", this, "highlightconflicts");
  QCheckBox * showqv       = new QCheckBox("Show QV", this, "showqvchkbx");

  QLabel * contig_lbl   = new QLabel(m_contigid, "Contig ID", this, "contiglbl");
  QLabel * db_lbl       = new QLabel(dbpick, "Database", this, "dblbl");
  QLabel * gindex_lbl   = new QLabel(m_gindex, "Position", this, "gindexlbl");
  QLabel * fontsize_lbl = new QLabel(fontsize, "Font Size", this, "fontlbl");

  int gutter = 5;

  m_slider = new QSlider(Horizontal, this, "slider");
  m_slider->setTracking(0);

  // slider <-> tiling
  connect(m_slider, SIGNAL(valueChanged(int)),
          tiling, SLOT(setGindex(int)) );

  connect(tiling, SIGNAL(gindexChanged(int)),
          m_slider, SLOT(setValue(int)) );

  connect(m_slider, SIGNAL(sliderMoved(int)),
          tiling,   SLOT(trackGindex(int)));

  connect(m_slider, SIGNAL(sliderReleased()),
          tiling,   SLOT(trackGindexDone()) );

  // m_gindex <-> tiling
  connect(tiling, SIGNAL(gindexChanged(int)),
          m_gindex, SLOT(setValue(int)));

  connect(m_gindex, SIGNAL(valueChanged(int)),
          tiling, SLOT(setGindex(int)));

  // fontsize <-> tiling
  connect(fontsize, SIGNAL(valueChanged(int)), 
          tiling,   SLOT(setFontSize(int)));

  // checkboxes <-> tiling
  connect(stable, SIGNAL(toggled(bool)),
          tiling, SLOT(toggleStable(bool)));
  connect(shownumbers, SIGNAL(toggled(bool)),
          tiling, SLOT(toggleShowNumbers(bool)));
  connect(hldisc, SIGNAL(toggled(bool)),
          tiling, SLOT(toggleHighlightDiscrepancy(bool)));
  connect(showqv, SIGNAL(toggled(bool)),
          tiling, SLOT(toggleDisplayQV(bool)));

  // contigid <-> tiling
  connect(m_contigid, SIGNAL(valueChanged(int)),
          tiling,     SLOT(setContigId(int)));

  connect(tiling,     SIGNAL(contigLoaded(int)),
          m_contigid, SLOT(setValue(int)));

  // mainwindow <-> tiling
  connect(this,   SIGNAL(bankSelected(string)),
          tiling, SLOT(setBankname(string)));
  
  connect(tiling,   SIGNAL(contigRange(int, int)),
          this,     SLOT(setContigRange(int, int)));

  connect(tiling, SIGNAL(setGindexRange(int, int)),
          this,   SLOT(setGindexRange(int, int)));

  connect(this, SIGNAL(contigIdSelected(int)),
          tiling, SLOT(setContigId(int)));

  connect(this, SIGNAL(gindexChanged(int)),
          tiling, SLOT(setGindex(int)));

  // statusbar <-> tiling
  connect(tiling,    SIGNAL(setStatus(const QString &)),
          statusbar, SLOT(setText(const QString &)));

  // dbpick <-> tiling
  connect(dbpick, SIGNAL(textChanged(const QString &)),
          tiling, SLOT(setDB(const QString &)));

  
  // outmost
  QBoxLayout * outergrid = new QHBoxLayout(0);
  QVBoxLayout* vbox = new QVBoxLayout(this);
  vbox->setMenuBar(menubar);
  menubar->setSeparator(QMenuBar::InWindowsStyle);
  vbox->addLayout(outergrid);
  vbox->addSpacing(gutter);
  vbox->addWidget(statusbar);
  vbox->activate();
  
  // outer
  QBoxLayout * leftgrid  = new QVBoxLayout(0);
  QBoxLayout * rightgrid = new QVBoxLayout(0);
 
  outergrid->addSpacing(gutter);
  outergrid->addLayout(leftgrid);
  outergrid->addSpacing(gutter);

  outergrid->addLayout(rightgrid, 10);

  //left
  leftgrid->addSpacing(gutter);
  leftgrid->addWidget(db_lbl);
  leftgrid->addWidget(dbpick);
  leftgrid->addSpacing(gutter);

  leftgrid->addWidget(contig_lbl);
  leftgrid->addWidget(m_contigid);
  leftgrid->addSpacing(gutter);

  leftgrid->addWidget(gindex_lbl);
  leftgrid->addWidget(m_gindex);
  leftgrid->addSpacing(gutter);

  leftgrid->addWidget(fontsize_lbl);
  leftgrid->addWidget(fontsize);
  leftgrid->addSpacing(gutter);

  leftgrid->addWidget(stable);
  leftgrid->addWidget(shownumbers);
  leftgrid->addWidget(hldisc);
  leftgrid->addWidget(showqv);
  leftgrid->addStretch(gutter);

  //right
  rightgrid->addWidget(tiling, gutter);
  rightgrid->addWidget(m_slider);

  // Set defaults
  fontsize->setValue(12);
  m_gindex->setValue(0);
  m_slider->setFocus();
}

void MainWindow::setBankname(string bankname)
{
  emit bankSelected(bankname);
}

void MainWindow::setGindex(int gindex)
{
  emit gindexChanged(gindex);
}

void MainWindow::setContigId(int contigId)
{
  emit contigIdSelected(contigId);
}

void MainWindow::openBank()
{
  QString s = QFileDialog::getExistingDirectory(
                   "/local/asmg/work/mschatz/AMOS",
                   this,
                   "get existing directory",
                   "Choose a Bank",
                   TRUE );

  if (s != "")
  {
    emit bankSelected(s.ascii());
  }
}

void MainWindow::setContigRange(int a, int b)
{
  m_contigid->setRange(a,b);
}

void MainWindow::setGindexRange(int a, int b)
{
  m_gindex->setRange(a,b);
  m_slider->setRange(a,b);
}

