#include "MainWindow.hh"

#include <qmenubar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qaccel.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qvbox.h>
#include <qsplitter.h>
#include <qapplication.h>
#include "DataStore.hh"


#include "TilingFrame.hh"
#include "AlignmentInfo.hh"

#include "icons/fontdecrease.xpm"
#include "icons/fontincrease.xpm"

using namespace std;

MainWindow::MainWindow(DataStore * datastore, QWidget *parent, const char *name )
           : QMainWindow( parent, name ),
             m_datastore(datastore)
{
  setCaption("Hawkeye : Contig View");
  m_gindex = 0;
  m_fontsize = 10;

  setGeometry(150,QApplication::desktop()->height()-450-parent->pos().y(),QApplication::desktop()->width()-200,400);

  m_outervbox = new QVBox(this, "mainvbox");
  setCentralWidget( m_outervbox);

  m_multiTilingWidget = new QWidget(m_outervbox, "multiTilingWidget");
  m_multiTilingWidget->show();

  QVBoxLayout * multiTilingLayout = new QVBoxLayout(m_multiTilingWidget, 0, -1, "mtl");

  AlignmentInfo * ai = new AlignmentInfo();
  m_tiling  = new TilingFrame(m_datastore, ai, m_multiTilingWidget, "origtilingframe");
  initializeTiling(m_tiling, true);
  m_tiling->show();

  multiTilingLayout->addWidget(m_tiling);


  m_slider = new QScrollBar(Horizontal, m_outervbox, "slider");
  m_slider->setTracking(1);
  m_slider->setPageStep(20);

  // Status Bar
  statusBar()->message("No Bank Loaded");

  // Status Tool Bar
  QToolBar * status = new QToolBar(this, "Status");
  status->setLabel("Navigation");

  m_options = new QPopupMenu(this);
  menuBar()->insertItem("&Options", m_options);
  m_basecolorid    = m_options->insertItem("Color &Bases",             this, SLOT(toggleBaseColors()));
  m_showfullid     = m_options->insertItem("Show &Full Range",         this, SLOT(toggleShowFullRange()));
  m_ungappedid     = m_options->insertItem("Ungapped Coordinates",     this, SLOT(toggleUngapped()));
  m_options->insertSeparator();
  m_qvid           = m_options->insertItem("Show &Quality Values",     this, SLOT(toggleDisplayQV()));
  m_cqvid          = m_options->insertItem("Show &Consensus QV",       this, SLOT(toggleShowConsQV()));
  m_lowquallowerid = m_options->insertItem("Lower Case &Low QV",       this, SLOT(toggleLowQualityLowerCase()));
  m_qvcolorid      = m_options->insertItem("Q&V Coloring",             this, SLOT(toggleQVColoring()));
  m_options->insertSeparator();
  m_snpcoloringid  = m_options->insertItem("SN&P Coloring",            this, SLOT(toggleSNPColoring()));
  m_highid         = m_options->insertItem("&Highlight Discrepancies", this, SLOT(toggleHighlightDiscrepancy()));
  m_polyid         = m_options->insertItem("&Polymorphism View",       this, SLOT(togglePolymorphismView()));
  m_options->insertSeparator();
  m_prefetch       = m_options->insertItem("&Prefetch Chromatograms",  this, SLOT(toggleDisplayAllChromo()));

  m_options->setItemChecked(m_snpcoloringid, true);


  new QLabel("Contig", status, "contiglbl");
  m_contigspin = new QSpinBox(1,100,1,status, "contigspin");
  m_contigspin->setMinimumWidth(100);

  status->addSeparator();

  m_offsetlabel = new QLabel("Offset", status, "gindexlbl");
  m_gspin     = new QSpinBox(0,100, 1, status, "gindexspin");
  m_gspin->setMinimumWidth(100);


  QToolButton * bPrevDisc = new QToolButton(Qt::LeftArrow, status, "prev");
  bPrevDisc->setTextLabel("Previous Discrepancy");
  bPrevDisc->setAccel(CTRL + Key_Space + SHIFT);
  bPrevDisc->setMinimumWidth(15);
  bPrevDisc->setMaximumWidth(15);
  bPrevDisc->setMaximumHeight(25);

  QToolButton * bNextDisc = new QToolButton(Qt::RightArrow, status, "next");
  bNextDisc->setTextLabel("Next Discrepancy");
  bNextDisc->setAccel(CTRL + Key_Space);
  bNextDisc->setMinimumWidth(15);
  bNextDisc->setMaximumWidth(15);
  bNextDisc->setMaximumHeight(25);

  connect(bNextDisc, SIGNAL(clicked()),
          this,      SIGNAL(advanceNextDiscrepancy()));

  connect(bPrevDisc, SIGNAL(clicked()),
          this,      SIGNAL(advancePrevDiscrepancy()));

  QAccel *a = new QAccel( this );
  a->connectItem(a->insertItem(Key_PageUp),   this, SLOT(jumpFGindex()) );
  a->connectItem(a->insertItem(Key_PageDown), this, SLOT(jumpPGindex()) );

  a->connectItem(a->insertItem(CTRL+Key_Plus),  this, SLOT(fontIncrease()));
  a->connectItem(a->insertItem(CTRL+Key_Minus), this, SLOT(fontDecrease()));

  a->connectItem(a->insertItem(CTRL + Key_PageUp),   bNextDisc, SLOT(animateClick()));
  a->connectItem(a->insertItem(CTRL + Key_PageDown), bPrevDisc, SLOT(animateClick()));

  a->connectItem(a->insertItem(CTRL+Key_Q),       qApp, SLOT(quit()));

  QIconSet icon_fontminus(QPixmap((const char ** )fontdecrease_xpm));
  QIconSet icon_fontplus(QPixmap((const char **)fontincrease_xpm));

  QToolButton * b = new QToolButton(icon_fontplus, "Font Increase", "Font Increase",
                                    this, SLOT(fontIncrease()), status);
  b->setMaximumWidth(25);

  b = new QToolButton(icon_fontminus, "Font Decrease", "Font Decrease",
                      this, SLOT(fontDecrease()), status);
  b->setMaximumWidth(25);

  QToolBar * searchbar = new QToolBar(this, "SearchBar");
  status->setLabel("Search");


  new QLabel("Search", searchbar, "findlbl");
  m_searchedit = new QLineEdit(searchbar, "searchbox");
  m_searchedit->setMinimumHeight(25);
  m_searchedit->setMinimumWidth(25);
  connect(m_searchedit, SIGNAL(returnPressed()), this, SLOT(findNext()));

  QToolButton * bFindPrev = new QToolButton(Qt::LeftArrow, searchbar, "rsearch");
  connect(bFindPrev, SIGNAL(clicked()), this, SLOT(findPrev()));
  bFindPrev->setTextLabel("Find Previous");
  bFindPrev->setAccel(SHIFT + CTRL + Key_F);
  bFindPrev->setMinimumWidth(15);
  bFindPrev->setMaximumWidth(15);
  bFindPrev->setMaximumHeight(25);

  QToolButton * bFindNext = new QToolButton(Qt::RightArrow, searchbar, "fsearch");
  connect(bFindNext, SIGNAL(clicked()), this, SLOT(findNext()));
  bFindNext->setTextLabel("Find Next");
  bFindNext->setAccel(CTRL + Key_F);
  bFindNext->setMinimumWidth(15);
  bFindNext->setMaximumWidth(15);
  bFindNext->setMaximumHeight(25);


  // gindex

  connect(this,     SIGNAL(gindexChanged(int)),
          m_slider, SLOT(setValue(int)));

  connect(m_slider, SIGNAL(valueChanged(int)),
          this,     SLOT(setGindex(int)));


  connect(this,     SIGNAL(gindexChanged(int)),
          this,     SLOT(updateGSpin(int)));

  connect(m_gspin,  SIGNAL(valueChanged(int)),
          this,     SLOT(loadGSpin(int)));

  connect(m_contigspin, SIGNAL(valueChanged(int)),
          parent,   SLOT(setContigId(int)));


  // Set defaults
  m_gspin->setValue(0);
  m_slider->setFocus();

  // If not mpop, enable
  //if (strcmp(getenv("USER"), "mpop") && strcmp(getenv("USER"), "mschatz"))
  if (strcmp(getenv("USER"), "mpop"))
  {
    toggleBaseColors();
  }

  toggleHighlightDiscrepancy();

  emit setFontSize(m_fontsize);
}

void MainWindow::initializeTiling(TilingFrame * tiling, bool isReference)
{
  connect(tiling,    SIGNAL(setTilingVisibleRange(int, int, int)),
          this,      SIGNAL(setTilingVisibleRange(int, int, int)));

  connect(this,      SIGNAL(gindexChanged(int)),
          tiling,      SLOT(setGindex(int)));

  connect(this,      SIGNAL(toggleDisplayAllChromo(bool)),
          tiling,      SLOT(toggleDisplayAllChromo(bool)));

  connect(this,      SIGNAL(setFontSize(int)),
          tiling,      SLOT(setFontSize(int)));

  connect(this,      SIGNAL(toggleDisplayQV(bool)),
          tiling,    SIGNAL(toggleDisplayQV(bool)));

  connect(this,      SIGNAL(toggleShowConsQV(bool)),
          tiling,    SIGNAL(toggleShowConsQV(bool)));

  connect(this,      SIGNAL(toggleQVColoring(bool)),
          tiling,    SIGNAL(toggleQVColoring(bool)));

  connect(this,      SIGNAL(toggleLowQualityLowerCase(bool)),
          tiling,    SIGNAL(toggleLowQualityLowerCase(bool)));

  connect(this,      SIGNAL(toggleShowFullRange(bool)),
          tiling,    SIGNAL(toggleShowFullRange(bool)));

  connect(this,      SIGNAL(toggleHighlightDiscrepancy(bool)),
          tiling,    SIGNAL(toggleHighlightDiscrepancy(bool)));

  connect(this,      SIGNAL(toggleShowNumbers(bool)),
          tiling,    SIGNAL(toggleShowNumbers(bool)));

  connect(this,      SIGNAL(togglePolymorphismView(bool)),
          tiling,    SIGNAL(togglePolymorphismView(bool)));

  connect(this,      SIGNAL(toggleBaseColors(bool)),
          tiling,    SIGNAL(toggleBaseColors(bool)));

  connect(this,      SIGNAL(toggleSNPColoring(bool)),
          tiling,    SIGNAL(toggleSNPColoring(bool)));

  connect(this,      SIGNAL(toggleShowUngapped(bool)),
          tiling,    SIGNAL(toggleShowUngapped(bool)));

  if (isReference)
  {
    connect(tiling,  SIGNAL(gindexChanged(int)),
            this,      SLOT(setGindex(int)));

    connect(tiling,  SIGNAL(setGindexRange(int, int)),
            this,      SLOT(setGindexRange(int, int)));

    connect(this,    SIGNAL(contigIdSelected(int)),
            tiling,    SLOT(setContigId(int)));
    
    connect(this,    SIGNAL(searchString(const QString &, bool)),
            tiling,    SLOT(searchString(const QString &, bool)));

    connect(this,    SIGNAL(advanceNextDiscrepancy()),
            tiling,    SLOT(advanceNextDiscrepancy()));

    connect(this,    SIGNAL(advancePrevDiscrepancy()),
            tiling,    SLOT(advancePrevDiscrepancy()));

    connect(tiling,         SIGNAL(highlightRead(int)),
            parentWidget(), SIGNAL(highlightRead(int)));
            
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



void MainWindow::toggleDisplayQV()
{
  bool b = !m_options->isItemChecked(m_qvid);
  m_options->setItemChecked(m_qvid, b);

  emit toggleDisplayQV(b);
}

void MainWindow::toggleShowConsQV()
{
  bool b = !m_options->isItemChecked(m_cqvid);
  m_options->setItemChecked(m_cqvid, b);

  emit toggleShowConsQV(b);
}

void MainWindow::toggleLowQualityLowerCase()
{
  bool b = !m_options->isItemChecked(m_lowquallowerid);
  m_options->setItemChecked(m_lowquallowerid, b);

  emit toggleLowQualityLowerCase(b);
}

void MainWindow::toggleQVColoring()
{
  bool b = !m_options->isItemChecked(m_qvcolorid);
  m_options->setItemChecked(m_qvcolorid, b);

  emit toggleQVColoring(b);
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

void MainWindow::togglePolymorphismView()
{
  bool b = !m_options->isItemChecked(m_polyid);
  m_options->setItemChecked(m_polyid, b);

  emit togglePolymorphismView(b);
}

void MainWindow::toggleUngapped()
{
  bool ungapped = m_options->isItemChecked(m_ungappedid);
  m_options->setItemChecked(m_ungappedid, !ungapped);

  int pos = m_gspin->value();

  if (ungapped) 
  { 
    m_offsetlabel->setText("Offset"); 
    m_gspin->setRange(0, m_datastore->m_contig.getLength());
    pos = m_datastore->m_contig.ungap2gap(pos);
  }
  else
  {
    m_offsetlabel->setText("Position"); 
    m_gspin->setRange(1, m_datastore->m_contig.gap2ungap(m_datastore->m_contig.getLength()));
  }

  emit toggleShowUngapped(!ungapped);
  updateGSpin(pos);
}

void MainWindow::updateGSpin(int gindex)
{
  if (m_options->isItemChecked(m_ungappedid))
  {
    m_gspin->setValue(m_datastore->m_contig.gap2ungap(gindex));
  }
  else
  {
    m_gspin->setValue(gindex);
  }
}

void MainWindow::loadGSpin(int pos)
{
  if (m_options->isItemChecked(m_ungappedid))
  {
    setGindex(m_datastore->m_contig.ungap2gap(pos));
  }
  else
  {
    setGindex(pos);
  }
}



void MainWindow::fontIncrease()
{
  m_fontsize++;
  emit setFontSize(m_fontsize);
}

void MainWindow::fontDecrease()
{
  if (m_fontsize <= -8) { return; }
  m_fontsize--;
  emit setFontSize(m_fontsize);
}



void MainWindow::jumpFGindex()
{
  setGindex(m_gindex+20);
}

void MainWindow::jumpPGindex()
{
  setGindex(m_gindex-20);
}

void MainWindow::findNext()
{
  const QString & str = m_searchedit->text();
  emit searchString(str, true);
}

void MainWindow::findPrev()
{
  const QString & str = m_searchedit->text();
  emit searchString(str, false);
}

void MainWindow::setContigId(int id)
{
  statusBar()->message(QString("Viewing Contig ") + QString::number(id));
  m_contigid = id;
  emit contigIdSelected(id);
  m_contigspin->setValue(id);
}

void MainWindow::bankChanged()
{
  m_contigspin->setMaxValue(m_datastore->contig_bank.getSize());
}
