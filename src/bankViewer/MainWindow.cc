#include "MainWindow.hh"

#include <qapplication.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qvbox.h>

#include "TilingFrame.hh"
#include "InsertWindow.hh"

using namespace std;

MainWindow::MainWindow( QWidget *parent, const char *name )
           : QMainWindow( parent, name )
{
  m_gindex = -1;

  m_contigPicker = NULL;
  m_fontsize = 12;
  m_insertWindow = NULL;
  m_cgraphWindow = NULL;

  QVBox * vbox = new QVBox(this, "vbox");

  m_tiling = new TilingFrame(&m_datastore, vbox, "tilingframe");

  m_slider = new QScrollBar(Horizontal, vbox, "slider");
  m_slider->setTracking(1);
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

  new QLabel("Position", status, "gindexlbl");
  m_gspin     = new QSpinBox(0,100, 1, status, "gindexspin");
  m_gspin->setMinimumWidth(100);

  QToolButton * bPrevDisc = new QToolButton(Qt::LeftArrow, status, "prev");
  bPrevDisc->setTextLabel("Previous Discrepancy");

  QToolButton * bNextDisc = new QToolButton(Qt::RightArrow, status, "next");
  bNextDisc->setTextLabel("Next Discrepancy");

  new QLabel("   Contig ID", status, "contiglbl");
  m_contigid  = new QSpinBox(1, 1, 1, status, "contigid");

  new QLabel("   Database", status, "dblbl");
  QLineEdit *  dbpick  = new QLineEdit(status, "dbpick");

  QToolButton * bShowInserts = new QToolButton(QPixmap(), "Show Inserts", "Show Inserts", 
                                               this, SLOT(showInserts()), status );
  bShowInserts->setText("Inserts");

  QToolButton * bShowCGraph = new QToolButton(QPixmap(), "Show CGraph", "Show CGraph", 
                                               this, SLOT(showCGraph()), status );
  bShowCGraph->setText("Contig Graph");

  QIconSet icon_fontminus(QPixmap("icons/fontdecrease.xpm"));
  QIconSet icon_fontplus(QPixmap("icons/fontincrease.xpm"));

  new QToolButton(icon_fontplus, "Font Increase", "Font Increase",
                  this, SLOT(fontIncrease()), status);

  new QToolButton(icon_fontminus, "Font Decrease", "Font Decrease",
                  this, SLOT(fontDecrease()), status);

  // gindex
  connect(this,     SIGNAL(gindexChanged(int)),
          m_tiling, SLOT(setGindex(int)));

  connect(this,     SIGNAL(gindexChanged(int)),
          m_slider, SLOT(setValue(int)));

  connect(this,     SIGNAL(gindexChanged(int)),
          m_gspin, SLOT(setValue(int)));

  connect(m_tiling, SIGNAL(gindexChanged(int)),
          this,     SLOT(setGindex(int)));
          
  connect(m_gspin, SIGNAL(valueChanged(int)),
          this,     SLOT(setGindex(int)));
  
  connect(m_slider, SIGNAL(valueChanged(int)),
          this,     SLOT(setGindex(int)));


  // discrepancy advance
  connect(bNextDisc, SIGNAL(clicked()),
          m_tiling,    SLOT(advanceNextDiscrepancy()));
  connect(bPrevDisc, SIGNAL(clicked()),
          m_tiling,    SLOT(advancePrevDiscrepancy()));

  // contigid
  connect(m_contigid, SIGNAL(valueChanged(int)),
          this,       SLOT(setContigId(int)));

  connect(this,       SIGNAL(contigIdSelected(int)),
          m_contigid, SLOT(setValue(int)));

  connect(this,       SIGNAL(contigIdSelected(int)),
          m_tiling,   SLOT(setContigId(int)));

  // mainwindow <-> tiling
  connect(m_tiling, SIGNAL(setGindexRange(int, int)),
          this,     SLOT(setGindexRange(int, int)));

  // dbpick <-> tiling
  connect(dbpick, SIGNAL(textChanged(const QString &)),
          this,   SLOT(setDB(const QString &)));

  // Set defaults
  m_gspin->setValue(0);
  m_slider->setFocus();
  dbpick->setText("DMG");
}


void MainWindow::chooseBank()
{
  QString s = QFileDialog::getExistingDirectory(
                   "/local/asmg/work/mschatz/AMOS",
                   this,
                   "get existing directory",
                   "Choose a Bank",
                   TRUE );

  if (!s.isEmpty())
  {
    setBankname(s.ascii());
  }
}

void MainWindow::showInserts()
{
  if (!m_insertWindow)
  {

    m_insertWindow = new InsertWindow(&m_datastore,
                                      this, 
                                      "insertWindow");

    connect(m_insertWindow, SIGNAL(setGindex(int)),
            this,         SLOT(setGindex(int)));

    connect(m_tiling,      SIGNAL(setTilingVisibleRange(int, int)),
            m_insertWindow,  SIGNAL(setTilingVisibleRange(int, int)));

    connect(this, SIGNAL(contigIdSelected(int)),
            m_insertWindow, SLOT(contigChanged()));
  }

  m_insertWindow->show();
  m_tiling->repaint();
}

void MainWindow::showCGraph()
{
  if (!m_cgraphWindow)
  {
    m_cgraphWindow = new CGraphWindow(&m_datastore, this, "CGraphWindow");

    connect(this, SIGNAL(contigIdSelected(int)),
            m_cgraphWindow, SLOT(contigChanged()));

    connect(m_cgraphWindow, SIGNAL(setContigId(int)),
            this,           SLOT(setContigId(int)));
  }

  m_cgraphWindow->show();
}

void MainWindow::chooseContig()
{
  if (m_contigPicker) { m_contigPicker->close(); }

  m_contigPicker = new ContigPicker(m_datastore.m_bankname, this, "contigpicker");
  connect(m_contigPicker, SIGNAL(contigSelected(int)),
          this,           SLOT(setContigId(int)));
}

void MainWindow::setContigId(int contigId)
{
  if ((contigId != m_datastore.m_contigId) &&
      !m_datastore.setContigId(contigId))
  {
    QString s = "Viewing ";
    s += m_datastore.m_bankname.c_str();
    s += " with ";
    s += QString::number(m_datastore.contig_bank.getSize());
    s += " contigs";

    s += " Contig Id:";
    s += QString::number(contigId);
    s += " Size: ";
    s += QString::number(m_datastore.m_contig.getSeqString().length());
    s += " Reads: ";
    s += QString::number(m_datastore.m_contig.getReadTiling().size());

    statusBar()->message(s);

    emit contigIdSelected(m_datastore.m_contigId);

    if (m_contigPicker)
    {
      m_contigPicker->close();
      m_contigPicker = NULL;
    }
  }
}

void MainWindow::setBankname(std::string bankname)
{
  if (bankname != "")
  {
    if (!m_datastore.openBank(bankname))
    {
      emit bankSelected(bankname);
      m_contigid->setRange(1, m_datastore.contig_bank.getSize());
      setContigId(1);
    }
  }
}

void MainWindow::setGindex(int gindex)
{
  if (gindex == m_gindex) { return; }

  m_gindex = gindex;
  emit gindexChanged(gindex);
}


void MainWindow::setGindexRange(int a, int b)
{
  m_gspin->setRange(a,b);
  m_slider->setRange(a,b);
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

void MainWindow::setDB(const QString & db)
{
  m_datastore.m_db = db.ascii();
}
