#include "MainWindow.hh"
#include <qapplication.h>

#include <qpushbutton.h>
#include <qlcdnumber.h>
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

#include "lcdrange.hh"
#include "TilingField.hh"

MainWindow::MainWindow(string bankname, int contigID,
                       QWidget *parent, const char *name )
           : QWidget( parent, name )
{
  // Menubar
  QMenuBar* menubar = new QMenuBar(this);
  Q_CHECK_PTR( menubar );

  QPopupMenu* file = new QPopupMenu( menubar );
  Q_CHECK_PTR( file );
  menubar->insertItem( "&File", file );
  file->insertItem( "&Open", this,  SLOT(openBank()) );
  file->insertItem( "&Quit", qApp,  SLOT(quit()) );

  // Statusbar
  QStatusBar * statusbar = new QStatusBar(this);
  statusbar->message("No Bank Loaded");

  // Widgets
  m_contigid = new QSpinBox(1, 1, 1, this, "contigid");
  QSpinBox * fontsize = new QSpinBox(6, 24, 1, this, "fontsize");

  LCDRange * gindex = new LCDRange( this, "gindex" );

  QLineEdit * dbpick = new QLineEdit("DMG", this, "dbpick");

  QScrollView * sv = new QScrollView(this, "tilingscroll");
  TilingField * tiling = new TilingField( sv->viewport(), "tiling" );
  sv->addChild(tiling);
  sv->setHScrollBarMode(QScrollView::AlwaysOff);
  sv->setResizePolicy(QScrollView::AutoOneFit);

  // slider <-> tiling
  connect(gindex, SIGNAL(valueChanged(int)),
          tiling, SLOT(setGindex(int)) );

  connect(tiling, SIGNAL(gindexChanged(int)),
          gindex, SLOT(setValue(int)) );

  connect(tiling, SIGNAL(setRange(int, int)),
          gindex, SLOT(setRange(int, int)));

  // fontsize <-> tiling
  connect(fontsize, SIGNAL(valueChanged(int)), 
          tiling,   SLOT(setFontSize(int)));

  // contigid <-> tiling
  connect(m_contigid, SIGNAL(valueChanged(int)),
          tiling,   SLOT(setContigId(int)));

  connect(tiling,   SIGNAL(contigLoaded(int)),
          m_contigid, SLOT(setValue(int)));

  // mainwindow <-> tiling
  connect(this,   SIGNAL(bankSelected(string)),
          tiling, SLOT(setBankname(string)));
  
  connect(tiling,   SIGNAL(contigRange(int, int)),
          this,     SLOT(setContigRange(int, int)));

  // statusbar <-> tiling
  connect(tiling,    SIGNAL(setStatus(const QString &)),
          statusbar, SLOT(message(const QString &)));

  // dbpick <-> tiling
  connect(dbpick, SIGNAL(textChanged(const QString &)),
          tiling, SLOT(setDB(const QString &)));


  QGridLayout *outergrid = new QGridLayout(NULL, 1, 2, 10, -1, "outerg"); // 1 row, 2 col
  
  // outmost
  QVBoxLayout* vbox = new QVBoxLayout(this);
  vbox->setMenuBar(menubar);
  menubar->setSeparator(QMenuBar::InWindowsStyle);
  vbox->addLayout(outergrid);
  vbox->addWidget(statusbar);
  vbox->activate();
  
  // outer
  QGridLayout *leftgrid = new QGridLayout(NULL, 9, 1, 1, -1, "leftg");
  QGridLayout *rightgrid = new QGridLayout(NULL, 2, 1, 10, -1, "rightg");
  outergrid->addMultiCellLayout(leftgrid, 0, 0, 0, 0);
  outergrid->addMultiCellLayout(rightgrid, 0, 0, 1, 1);
  outergrid->setColStretch( 1, 10 );

  //left
  QLabel * db_lbl = new QLabel(dbpick, "Database", this, "dblbl");
  leftgrid->addWidget(db_lbl,0,0);
  leftgrid->addWidget(dbpick,1,0);
  leftgrid->addRowSpacing(2,10);

  QLabel * contig_lbl = new QLabel(m_contigid, "Contig ID", this, "contiglbl");
  leftgrid->addWidget(contig_lbl,3,0);
  leftgrid->addWidget(m_contigid,4,0);
  leftgrid->addRowSpacing(5,10);

  QLabel * fontsize_lbl = new QLabel(fontsize, "Font Size", this, "fontlbl");
  leftgrid->addWidget(fontsize_lbl,6,0);
  leftgrid->addWidget(fontsize, 7,0);
  leftgrid->setRowStretch(8,10);


  //right
  rightgrid->addWidget( gindex, 0, 0);
  rightgrid->addWidget( sv, 1, 0 );
  rightgrid->setRowStretch(1, 10);

  // Set defaults
  fontsize->setValue(12);
  tiling->setBankname(bankname);
  tiling->setContigId(contigID);
  gindex->setValue( 0 );
  gindex->setFocus();
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
