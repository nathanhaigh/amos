#include "MainWindow.hh"

#include <qapplication.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qfiledialog.h>
#include <qaccel.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qvbox.h>

#include "TilingFrame.hh"
#include "InsertWindow.hh"

#include "icons/fontdecrease.xpm"
#include "icons/fontincrease.xpm"

using namespace std;

MainWindow::MainWindow( QWidget *parent, const char *name )
           : QMainWindow( parent, name )
{
  setCaption("Assembly Investigator");
  m_gindex = -1;

  m_contigPicker = NULL;
  m_readPicker = NULL;
  m_libPicker = NULL;
  m_chromoPicker = NULL;
  m_featPicker = NULL;
  m_fontsize = 10;
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
  file->insertItem("&Open Bank...",     this,  SLOT(chooseBank()));
  file->insertItem("Chromatogram P&aths...",   this,  SLOT(showChromoPicker()));
  file->insertSeparator();
  file->insertItem("&Contig Picker...", this,  SLOT(chooseContig()));
  file->insertItem("&Read Information...",   this,  SLOT(showReadPicker()));
  file->insertItem("&Feature Browser...",   this,  SLOT(showFeatureBrowser()));
  file->insertItem("&Library Information...",   this,  SLOT(showLibPicker()));
  file->insertSeparator();
  file->insertItem("&Quit", qApp,  SLOT(quit()), CTRL+Key_Q );

  m_options = new QPopupMenu(this);
  menuBar()->insertItem("&Options", m_options);
  m_basecolorid    = m_options->insertItem("Color &Bases",             this, SLOT(toggleBaseColors()));
  m_snpcoloringid  = m_options->insertItem("SN&P Coloring",            this, SLOT(toggleSNPColoring()));
  m_showfullid     = m_options->insertItem("Show &Full Range",         this, SLOT(toggleShowFullRange()));
  m_posid          = m_options->insertItem("&Show Positions",          this, SLOT(toggleShowPositions()));
  m_indicatorid    = m_options->insertItem("Show &Indicator",          this, SLOT(toggleShowIndicator()));
  m_qvid           = m_options->insertItem("Show &Quality Values",     this, SLOT(toggleDisplayQV()));
  m_lowquallowerid = m_options->insertItem("Lower Case &Low QV", this, SLOT(toggleLowQualityLowerCase()));
  m_highid         = m_options->insertItem("&Highlight Discrepancies", this, SLOT(toggleHighlightDiscrepancy()));
  m_prefetch       = m_options->insertItem("&Prefetch Chromatograms",  this, SLOT(toggleDisplayAllChromo()));

  m_options->setItemChecked(m_snpcoloringid, true);

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
  bPrevDisc->setAccel(Key_PageDown);
  bPrevDisc->setMinimumWidth(20);

  QToolButton * bNextDisc = new QToolButton(Qt::RightArrow, status, "next");
  bNextDisc->setTextLabel("Next Discrepancy");
  bNextDisc->setAccel(Key_PageUp);
  bNextDisc->setMinimumWidth(20);

  QAccel *a = new QAccel( this );
  a->connectItem(a->insertItem(Key_PageUp+SHIFT), this, SLOT(jumpFGindex()) );
  a->connectItem(a->insertItem(Key_Up+SHIFT),     this, SLOT(jumpFGindex()) );

  a->connectItem(a->insertItem(Key_PageDown+SHIFT), this, SLOT(jumpPGindex()) );
  a->connectItem(a->insertItem(Key_Down+SHIFT),     this, SLOT(jumpPGindex()) );

  a->connectItem(a->insertItem(CTRL+Key_Plus), this, SLOT(fontIncrease()));
  a->connectItem(a->insertItem(CTRL+Key_Minus), this, SLOT(fontDecrease()));

  new QLabel("   Contig ID", status, "contiglbl");
  m_contigid  = new QSpinBox(1, 1, 1, status, "contigid");

  new QLabel("   Chromo DB", status, "dblbl");
  QLineEdit *  dbpick  = new QLineEdit(status, "dbpick");

  QToolButton * bShowInserts = new QToolButton(QPixmap(), "Show Inserts", "Show Inserts", 
                                               this, SLOT(showInserts()), status );
  bShowInserts->setText("Inserts");
  bShowInserts->setAccel(CTRL+Key_I);

  QToolButton * bShowCGraph = new QToolButton(QPixmap(), "Show CGraph", "Show CGraph", 
                                               this, SLOT(showCGraph()), status );
  bShowCGraph->setText("Contig Graph");

  QIconSet icon_fontminus(QPixmap((const char ** )fontdecrease_xpm));
  QIconSet icon_fontplus(QPixmap((const char **)fontincrease_xpm));

  new QToolButton(icon_fontplus, "Font Increase", "Font Increase",
                  this, SLOT(fontIncrease()), status);

  new QToolButton(icon_fontminus, "Font Decrease", "Font Decrease",
                  this, SLOT(fontDecrease()), status);

  QToolBar * searchbar = new QToolBar(this, "SearchBar");

  m_searchedit = new QLineEdit(searchbar, "searchbox");
  QToolButton * bfind = new QToolButton(QPixmap(), "Find", "Find", this, SLOT(findString()), searchbar);
  bfind->setMinimumWidth(20);
  bfind->setText("Find");
  bfind->setAccel(CTRL+Key_F);

  connect(m_searchedit, SIGNAL(returnPressed()),
          this,         SLOT(findString()));
        


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

  connect(this,     SIGNAL(highlightRead(int)),
          m_tiling, SLOT(highlightRead(int)));

  // dbpick <-> tiling
  connect(dbpick, SIGNAL(textChanged(const QString &)),
          this,   SLOT(setChromoDB(const QString &)));

  connect(this, SIGNAL(chromoDBSet(const QString &)),
          dbpick, SLOT(setText(const QString &)));



  connect(this,        SIGNAL(toggleDisplayAllChromo(bool)),
          m_tiling,    SLOT(toggleDisplayAllChromo(bool)));

  connect(this,        SIGNAL(setFontSize(int)),
          m_tiling,    SLOT(setFontSize(int)));

  connect(this,        SIGNAL(toggleDisplayQV(bool)),
          m_tiling,    SIGNAL(toggleDisplayQV(bool)));

  connect(this,        SIGNAL(toggleLowQualityLowerCase(bool)),
          m_tiling,    SIGNAL(toggleLowQualityLowerCase(bool)));

  connect(this,        SIGNAL(toggleShowFullRange(bool)),
          m_tiling,    SIGNAL(toggleShowFullRange(bool)));

  connect(this,        SIGNAL(toggleHighlightDiscrepancy(bool)),
          m_tiling,    SIGNAL(toggleHighlightDiscrepancy(bool)));

  connect(this,        SIGNAL(toggleShowNumbers(bool)),
          m_tiling,    SIGNAL(toggleShowNumbers(bool)));

  connect(this,        SIGNAL(toggleBaseColors(bool)),
          m_tiling,    SIGNAL(toggleBaseColors(bool)));

  connect(this,        SIGNAL(toggleShowIndicator(bool)),
          m_tiling,    SIGNAL(toggleShowIndicator(bool)));

  connect(this,        SIGNAL(toggleSNPColoring(bool)),
          m_tiling,    SIGNAL(toggleSNPColoring(bool)));

  connect(this,        SIGNAL(searchString(const QString &)),
          m_tiling,    SLOT(searchString(const QString &)));


  // Set defaults
  m_gspin->setValue(0);
  m_slider->setFocus();
  dbpick->setText("DMG");

  // If not mpop, enable
  if (strcmp(getenv("USER"), "mpop") && strcmp(getenv("USER"), "mschatz"))
  {
    toggleBaseColors();
  }

  toggleShowIndicator();
  toggleHighlightDiscrepancy();
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

  m_contigPicker = new ContigPicker(&m_datastore, this, "contigpicker");
  connect(m_contigPicker, SIGNAL(contigSelected(int)),
          this,           SLOT(setContigId(int)));
  connect(m_contigPicker, SIGNAL(setGindex(int)),
          this,           SLOT(setGindex(int)));
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

    if (m_readPicker)
    {
      m_readPicker->close();
      m_readPicker = NULL;
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

void MainWindow::toggleShowFullRange()
{
  bool b = !m_options->isItemChecked(m_showfullid);
  m_options->setItemChecked(m_showfullid, b);

  emit toggleShowFullRange(b);
}


void MainWindow::toggleShowPositions()
{
  bool b = !m_options->isItemChecked(m_posid);
  m_options->setItemChecked(m_posid, b);

  emit toggleShowNumbers(b);
}

void MainWindow::toggleShowIndicator()
{
  bool b = !m_options->isItemChecked(m_indicatorid);
  m_options->setItemChecked(m_indicatorid, b);

  emit toggleShowIndicator(b);
}

void MainWindow::toggleDisplayQV()
{
  bool b = !m_options->isItemChecked(m_qvid);
  m_options->setItemChecked(m_qvid, b);

  emit toggleDisplayQV(b);
}

void MainWindow::toggleLowQualityLowerCase()
{
  bool b = !m_options->isItemChecked(m_lowquallowerid);
  m_options->setItemChecked(m_lowquallowerid, b);

  emit toggleLowQualityLowerCase(b);
}

void MainWindow::toggleBaseColors()
{
  bool b = !m_options->isItemChecked(m_basecolorid);
  m_options->setItemChecked(m_basecolorid, b);

  emit toggleBaseColors(b);
}

void MainWindow::toggleSNPColoring()
{
  bool b = !m_options->isItemChecked(m_snpcoloringid);
  m_options->setItemChecked(m_snpcoloringid, b);

  emit toggleSNPColoring(b);
}

void MainWindow::toggleHighlightDiscrepancy()
{
  bool b = !m_options->isItemChecked(m_highid);
  m_options->setItemChecked(m_highid, b);

  emit toggleHighlightDiscrepancy(b);
}

void MainWindow::toggleDisplayAllChromo()
{
  bool b = !m_options->isItemChecked(m_prefetch);
  m_options->setItemChecked(m_prefetch, b);

  emit toggleDisplayAllChromo(b);
}

void MainWindow::fontIncrease()
{
  m_fontsize++;
  emit setFontSize(m_fontsize);
}

void MainWindow::fontDecrease()
{
  if (m_fontsize <= 6) { return; }
  m_fontsize--;
  emit setFontSize(m_fontsize);
}

void MainWindow::setChromoDB(const QString & db)
{
  if (m_datastore.m_db != db.ascii())
  {
    m_datastore.m_db = db.ascii();
    emit chromoDBSet(db);
  }
}

void MainWindow::showReadPicker()
{
  if (m_readPicker) { m_readPicker->close(); }

  m_readPicker = new ReadPicker(&m_datastore, this, "readPicker");
  connect(m_readPicker, SIGNAL(highlightRead(int)),
          this,         SIGNAL(highlightRead(int)));
}

void MainWindow::showLibPicker()
{
  if (m_libPicker) { m_libPicker->close(); }

  m_libPicker = new LibraryPicker(&m_datastore, this, "libPicker");
}

void MainWindow::showChromoPicker()
{
  if (m_chromoPicker) { m_chromoPicker->close(); }

  m_chromoPicker = new ChromoPicker(&m_datastore, this, "chromoPicker");
}

void MainWindow::showFeatureBrowser()
{
  if (m_featPicker) { m_featPicker->close(); }

  m_featPicker = new FeatureBrowser(&m_datastore, this, "featPicker");
  connect(m_featPicker,   SIGNAL(setGindex(int)),
          this,           SLOT(setGindex(int)));

  connect(m_featPicker,   SIGNAL(setContigId(int)),
          this,           SLOT(setContigId(int)));
}

void MainWindow::addChromoPath(const QString & path)
{
  m_datastore.m_chromopaths.push_back(path.ascii());
}

void MainWindow::addChromoDB(const QString & db)
{
  m_datastore.m_chromodbs.push_back(db.ascii());
}

void MainWindow::jumpFGindex()
{
  setGindex(m_gindex+20);
}

void MainWindow::jumpPGindex()
{
  setGindex(m_gindex-20);
}

void MainWindow::findString()
{
  const QString & str = m_searchedit->text();
  emit searchString(str);
}
