#include "MainWindow.hh"

#include <qapplication.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qspinbox.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qscrollview.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <qlayout.h>

#include <qmainwindow.h>
#include <qlistview.h>
#include <qcursor.h>

#include "TilingFrame.hh"

using namespace std;

MainWindow::MainWindow( QWidget *parent, const char *name )
           : QMainWindow( parent, name )
{

  m_contigPicker = NULL;
  m_fontsize = 12;

  QVBox * vbox = new QVBox(this, "vbox");

  m_tiling = new TilingFrame(vbox, "tilingframe");

  m_slider = new QScrollBar(Horizontal, vbox, "slider");
  m_slider->setTracking(0);
  m_slider->setPageStep(20);

  setCentralWidget(vbox);

  // Menubar
  QPopupMenu* file = new QPopupMenu(this);
  menuBar()->insertItem("&File", file);
  file->insertItem("&Open Bank...",     this,  SLOT(chooseBank()) );
  file->insertItem("&Contig Picker...", this, SLOT(chooseContig()));
  file->insertItem("&Quit", qApp,  SLOT(quit()), CTRL+Key_Q );

  m_options = new QPopupMenu(this);
  menuBar()->insertItem("&Options", m_options);
  m_posid    = m_options->insertItem("&Show Positions",          this, SLOT(toggleShowPositions()));
  m_qvid     = m_options->insertItem("Show &Quality Values",     this, SLOT(toggleShowQV()));
  m_highid   = m_options->insertItem("&Highlight Discrepancies", this, SLOT(toggleHighlightDiscrepancy()));
  m_prefetch = m_options->insertItem("&Prefetch Chromatograms",  this, SLOT(togglePrefetchChromatograms()));

  // Status Bar
  statusBar()->message("No Bank Loaded");

  // Status Tool Bar
  QToolBar * status = new QToolBar(this, "Status");
  status->setLabel("Status");

  QLabel * lbl = new QLabel("Position", status, "gindexlbl");
  m_gindex     = new QSpinBox(0,100, 1, status, "gindexspin");
  m_gindex->setMinimumWidth(100);

  QToolButton * bPrevDisc = new QToolButton(Qt::LeftArrow, status, "prev");
  bPrevDisc->setTextLabel("Previous Discrepancy");

  QToolButton * bNextDisc = new QToolButton(Qt::RightArrow, status, "next");
  bNextDisc->setTextLabel("Next Discrepancy");

  new QLabel("   Contig ID", status, "contiglbl");
  m_contigid  = new QSpinBox(1, 1, 1, status, "contigid");

  new QLabel("   Database", status, "dblbl");
  QLineEdit *  dbpick  = new QLineEdit("DMG", status, "dbpick");

  QToolButton * bShowInserts = new QToolButton(QPixmap(), "Show Inserts", "Show Inserts", 
                                               m_tiling, SLOT(showInserts()), status );
  bShowInserts->setText("Show Inserts");

  QIconSet icon_fontminus(QPixmap("icons/fontdecrease.xpm"));
  QIconSet icon_fontplus(QPixmap("icons/fontincrease.xpm"));

  QToolButton * bFontIncrease = new QToolButton(icon_fontplus, "Font Increase", "Font Increase",
                                                this, SLOT(fontIncrease()), status);

  QToolButton * bFontDecrease = new QToolButton(icon_fontminus, "Font Decrease", "Font Decrease",
                                                this, SLOT(fontDecrease()), status);

  // slider <-> tiling
  connect(m_slider, SIGNAL(valueChanged(int)),
          m_tiling, SLOT(setGindex(int)) );

  connect(m_slider, SIGNAL(sliderMoved(int)),
          m_tiling, SLOT(trackGindex(int)));

  connect(m_tiling, SIGNAL(gindexChanged(int)),
          m_slider, SLOT(setValue(int)) );

  connect(m_slider, SIGNAL(sliderReleased()),
          m_tiling,   SLOT(trackGindexDone()) );

  // m_gindex <-> tiling
  connect(m_tiling, SIGNAL(gindexChanged(int)),
          m_gindex, SLOT(setValue(int)));

  connect(m_gindex, SIGNAL(valueChanged(int)),
          m_tiling, SLOT(setGindex(int)));

  // checkboxes <-> tiling
//  connect(stable, SIGNAL(toggled(bool)),
//          tiling, SLOT(toggleStable(bool)));

  connect(bNextDisc, SIGNAL(clicked()),
          m_tiling,    SLOT(advanceNextDiscrepancy()));
  connect(bPrevDisc, SIGNAL(clicked()),
          m_tiling,    SLOT(advancePrevDiscrepancy()));


  // contigid <-> tiling
  connect(m_contigid, SIGNAL(valueChanged(int)),
          m_tiling,     SLOT(setContigId(int)));

  connect(m_tiling,     SIGNAL(contigLoaded(int)),
          m_contigid, SLOT(setValue(int)));

  // mainwindow <-> tiling
  connect(this,   SIGNAL(bankSelected(std::string)),
          m_tiling, SLOT(setBankname(std::string)));
  
  connect(m_tiling,   SIGNAL(contigRange(int, int)),
          this,     SLOT(setContigRange(int, int)));

  connect(m_tiling, SIGNAL(setGindexRange(int, int)),
          this,   SLOT(setGindexRange(int, int)));

  connect(this, SIGNAL(contigIdSelected(int)),
          m_tiling, SLOT(setContigId(int)));

  connect(this, SIGNAL(gindexChanged(int)),
          m_tiling, SLOT(setGindex(int)));


  // dbpick <-> tiling
  connect(dbpick, SIGNAL(textChanged(const QString &)),
          m_tiling, SLOT(setDB(const QString &)));

  
  // statusbar <-> tiling
  connect(m_tiling,      SIGNAL(setStatus(const QString &)),
          statusBar(), SLOT(message(const QString &)));
  

  // Set defaults
  m_gindex->setValue(0);
  m_slider->setFocus();
}

void MainWindow::setBankname(std::string bankname)
{
  m_bankname = bankname;
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

void MainWindow::chooseBank()
{
  QString s = QFileDialog::getExistingDirectory(
                   "/local/asmg/work/mschatz/AMOS",
                   this,
                   "get existing directory",
                   "Choose a Bank",
                   TRUE );

  if (s != "")
  {
    m_bankname = s.ascii();
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

void MainWindow::chooseContig()
{
  if (m_contigPicker) { m_contigPicker->close(); }

  m_contigPicker = new ContigPicker(m_bankname, this, "contigpicker");
  connect(m_contigPicker, SIGNAL(contigSelected(int)),
          this,           SLOT(contigSelected(int)));
}

void MainWindow::contigSelected(int contigid)
{
  emit contigIdSelected(contigid);

  if (m_contigPicker)
  {
    m_contigPicker->close();
    m_contigPicker = NULL;
  }
}

void MainWindow::toggleShowPositions()
{
  bool b = !m_options->isItemChecked(m_posid);
  m_options->setItemChecked(m_posid, b);

  m_tiling->toggleShowNumbers(b);
}

void MainWindow::toggleShowQV()
{
  bool b = !m_options->isItemChecked(m_qvid);
  m_options->setItemChecked(m_qvid, b);

  m_tiling->toggleDisplayQV(b);
}

void MainWindow::toggleHighlightDiscrepancy()
{
  bool b = !m_options->isItemChecked(m_highid);
  m_options->setItemChecked(m_highid, b);

  m_tiling->toggleHighlightDiscrepancy(b);
}

void MainWindow::togglePrefetchChromatograms()
{
  bool b = !m_options->isItemChecked(m_prefetch);
  m_options->setItemChecked(m_prefetch, b);

  m_tiling->toggleDisplayAllChromo(b);
}

void MainWindow::fontIncrease()
{
  m_fontsize++;
  m_tiling->setFontSize(m_fontsize);
}

void MainWindow::fontDecrease()
{
  if (m_fontsize <= 6) { return; }
  m_fontsize--;
  m_tiling->setFontSize(m_fontsize);
}

