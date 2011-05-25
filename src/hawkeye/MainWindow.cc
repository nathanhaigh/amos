#include "MainWindow.hh"

#include <qmenubar.h>
#include <qinputdialog.h>
#include <q3toolbar.h>
#include <qtoolbutton.h>
#include <q3accel.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3vbox.h>
#include <qsplitter.h>
#include <qapplication.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3PopupMenu>
#include <Q3VBoxLayout>
#include "DataStore.hh"
#include <QtGui>


#include "TilingFrame.hh"
#include "AlignmentInfo.hh"

#include "icons/fontdecrease.xpm"
#include "icons/fontincrease.xpm"

using namespace std;

MainWindow::MainWindow(DataStore * datastore, QWidget *parent, const char *name )
           : Q3MainWindow( NULL, name ),
             m_parent(parent), m_datastore(datastore)
{
  setCaption("Hawkeye : Contig View");
  m_gindex = 0;
  m_fontsize = 10;

  setGeometry(150,QApplication::desktop()->height()-450-parent->pos().y(),QApplication::desktop()->width()-200,400);

  m_outervbox = new Q3VBox(this, "mainvbox");
  setCentralWidget( m_outervbox);

  m_multiTilingWidget = new QWidget(m_outervbox, "multiTilingWidget");
  m_multiTilingWidget->show();

  Q3VBoxLayout * multiTilingLayout = new Q3VBoxLayout(m_multiTilingWidget, 0, -1, "mtl");

  AlignmentInfo * ai = new AlignmentInfo();
  m_tiling  = new TilingFrame(m_datastore, ai, m_multiTilingWidget, "origtilingframe");
  initializeTiling(m_tiling, true);
  m_tiling->show();

  multiTilingLayout->addWidget(m_tiling);


  m_slider = new QScrollBar(Qt::Horizontal, m_outervbox, "slider");
  m_slider->setTracking(1);
  m_slider->setPageStep(20);

  // Status Bar
  statusBar()->message("No Bank Loaded");

  // Status Tool Bar
  Q3ToolBar * status = new Q3ToolBar(this, "Status");
  status->setLabel("Navigation");

  m_options = new Q3PopupMenu(this);
  menuBar()->insertItem("&Options", m_options);
  m_packid         = m_options->insertItem("Pac&k Reads",              this, SLOT(togglePackReads())); m_packchecked = false;
  m_namewidthid    = m_options->insertItem("Set Readname &Width",      this, SLOT(setReadnameWidth()));
  m_basecolorid    = m_options->insertItem("Color &Bases",             this, SLOT(toggleBaseColors())); m_basecolorchecked = false;
  m_showfullid     = m_options->insertItem("Show &Full Range",         this, SLOT(toggleShowFullRange())); m_showfullchecked = false;
  m_ungappedid     = m_options->insertItem("Ungapped Coordinates",     this, SLOT(toggleUngapped())); m_ungappedchecked = false;
  m_options->insertSeparator();
  m_qvid           = m_options->insertItem("Show &Quality Values",     this, SLOT(toggleDisplayQV())); m_qvchecked = false;
  m_cqvid          = m_options->insertItem("Show &Consensus QV",       this, SLOT(toggleShowConsQV())); m_cqvchecked = false;
  m_lowquallowerid = m_options->insertItem("Lower Case &Low QV",       this, SLOT(toggleLowQualityLowerCase())); m_lowquallowerchecked = false;
  m_qvcolorid      = m_options->insertItem("Q&V Coloring",             this, SLOT(toggleQVColoring())); m_qvcolorchecked = false;
  m_options->insertSeparator();
  m_snpcoloringid  = m_options->insertItem("SN&P Coloring",            this, SLOT(toggleSNPColoring())); m_snpcoloringchecked = false;
  m_highid         = m_options->insertItem("&Highlight Discrepancies", this, SLOT(toggleHighlightDiscrepancy())); m_highchecked = false;
  m_polyid         = m_options->insertItem("&Polymorphism View",       this, SLOT(togglePolymorphismView())); m_polychecked = false;
  m_options->insertSeparator();
  m_prefetch       = m_options->insertItem("&Prefetch Chromatograms",  this, SLOT(toggleDisplayAllChromo())); m_prefetchchecked = false;

  m_options->setItemChecked(m_snpcoloringid, true); m_snpcoloringchecked = true;


  new QLabel("Contig", status, "contiglbl");
  m_contigspin = new QSpinBox(1,100,1,status, "contigspin");
  m_contigspin->setMinimumWidth(100);

  status->addSeparator();

  m_offsetlabel = new QLabel("Offset", status, "gindexlbl");
  m_gspin     = new QSpinBox(0,100, 1, status, "gindexspin");
  m_gspin->setMinimumWidth(100);


  QToolButton * bPrevDisc = new QToolButton(Qt::LeftArrow, status, "prev");
  bPrevDisc->setTextLabel("Previous Discrepancy");
  bPrevDisc->setAccel(Qt::CTRL + Qt::Key_Space + Qt::SHIFT);
  bPrevDisc->setMinimumWidth(15);
  bPrevDisc->setMaximumWidth(15);
  bPrevDisc->setMaximumHeight(25);

  QToolButton * bNextDisc = new QToolButton(Qt::RightArrow, status, "next");
  bNextDisc->setTextLabel("Next Discrepancy");
  bNextDisc->setAccel(Qt::CTRL + Qt::Key_Space);
  bNextDisc->setMinimumWidth(15);
  bNextDisc->setMaximumWidth(15);
  bNextDisc->setMaximumHeight(25);

  connect(bNextDisc, SIGNAL(clicked()),
          this,      SIGNAL(advanceNextDiscrepancy()));

  connect(bPrevDisc, SIGNAL(clicked()),
          this,      SIGNAL(advancePrevDiscrepancy()));

  connect(new QShortcut(QKeySequence(tr("Ctrl+Q")), this), SIGNAL(activated()),
          qApp, SLOT(quit()));

  connect(new QShortcut(QKeySequence(tr("Ctrl+W")), this), SIGNAL(activated()),
          this, SLOT(close()));

  connect(new QShortcut(QKeySequence(tr("Ctrl+Shift+=")), this), SIGNAL(activated()),
          this, SLOT(fontIncrease()));

  connect(new QShortcut(QKeySequence(tr("Ctrl+-")), this), SIGNAL(activated()),
          this, SLOT(fontDecrease()));

  connect(new QShortcut(QKeySequence(tr("Ctrl+Right")), this), SIGNAL(activated()),
          this, SLOT(jumpFGindex()));

  connect(new QShortcut(QKeySequence(tr("Ctrl+Left")), this), SIGNAL(activated()),
          this, SLOT(jumpPGindex()));

  connect(new QShortcut(QKeySequence(tr("Ctrl+Shift+.")), this), SIGNAL(activated()),
          bNextDisc, SLOT(animateClick()));

  connect(new QShortcut(QKeySequence(tr("Ctrl+Shift+,")), this), SIGNAL(activated()),
          this, SIGNAL(advancePrevDiscrepancy()));



  QIcon icon_fontminus(QPixmap((const char ** )fontdecrease_xpm));
  QIcon icon_fontplus(QPixmap((const char **)fontincrease_xpm));

  QToolButton * b = new QToolButton(icon_fontplus, "Font Increase", "Font Increase",
                                    this, SLOT(fontIncrease()), status);
  b->setMaximumWidth(25);
  b->setMaximumHeight(25);

  b = new QToolButton(icon_fontminus, "Font Decrease", "Font Decrease",
                      this, SLOT(fontDecrease()), status);
  b->setMaximumWidth(25);
  b->setMaximumHeight(25);

  Q3ToolBar * searchbar = new Q3ToolBar(this, "SearchBar");
  status->setLabel("Search");


  new QLabel("Search", searchbar, "findlbl");
  m_searchedit = new QLineEdit(searchbar, "searchbox");
  m_searchedit->setMinimumHeight(25);
  m_searchedit->setMinimumWidth(25);
  connect(m_searchedit, SIGNAL(returnPressed()), this, SLOT(findNext()));

  QToolButton * bFindPrev = new QToolButton(Qt::LeftArrow, searchbar, "rsearch");
  connect(bFindPrev, SIGNAL(clicked()), this, SLOT(findPrev()));
  bFindPrev->setTextLabel("Find Previous");
  bFindPrev->setAccel(Qt::SHIFT + Qt::CTRL + Qt::Key_F);
  bFindPrev->setMinimumWidth(15);
  bFindPrev->setMaximumWidth(15);
  bFindPrev->setMaximumHeight(25);

  QToolButton * bFindNext = new QToolButton(Qt::RightArrow, searchbar, "fsearch");
  connect(bFindNext, SIGNAL(clicked()), this, SLOT(findNext()));
  bFindNext->setTextLabel("Find Next");
  bFindNext->setAccel(Qt::CTRL + Qt::Key_F);
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

  connect(this,      SIGNAL(togglePackReads(bool)),
          tiling,    SIGNAL(togglePackReads(bool)));

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

    connect(tiling,   SIGNAL(highlightRead(int)),
            m_parent, SIGNAL(highlightRead(int)));
            
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

void MainWindow::togglePackReads()
{
  m_packchecked = !m_packchecked;
  m_options->setItemChecked(m_packid, m_packchecked);

  emit togglePackReads(m_packchecked);
}

void MainWindow::toggleShowFullRange()
{
  m_showfullchecked = !m_showfullchecked;
  m_options->setItemChecked(m_showfullid, m_showfullchecked);

  emit toggleShowFullRange(m_showfullchecked);
}

void MainWindow::toggleDisplayQV()
{
  m_qvchecked = !m_qvchecked;
  m_options->setItemChecked(m_qvid, m_qvchecked);

  emit toggleDisplayQV(m_qvchecked);
}

void MainWindow::toggleShowConsQV()
{
  m_cqvchecked = !m_cqvchecked;
  m_options->setItemChecked(m_cqvid, m_cqvchecked);

  emit toggleShowConsQV(m_cqvchecked);
}

void MainWindow::toggleLowQualityLowerCase()
{
  m_lowquallowerchecked = !m_lowquallowerchecked;
  m_options->setItemChecked(m_lowquallowerid, m_lowquallowerchecked);

  emit toggleLowQualityLowerCase(m_lowquallowerchecked);
}

void MainWindow::toggleQVColoring()
{
  m_qvcolorchecked = !m_qvcolorchecked;
  m_options->setItemChecked(m_qvcolorid, m_qvcolorchecked);

  emit toggleQVColoring(m_qvcolorchecked);
}

void MainWindow::toggleBaseColors()
{
  m_basecolorchecked = !m_basecolorchecked;
  m_options->setItemChecked(m_basecolorid, m_basecolorchecked);

  emit toggleBaseColors(m_basecolorchecked);
}

void MainWindow::toggleSNPColoring()
{
  m_snpcoloringchecked = !m_snpcoloringchecked;
  m_options->setItemChecked(m_snpcoloringid, m_snpcoloringchecked);

  emit toggleSNPColoring(m_snpcoloringchecked);
}

void MainWindow::toggleHighlightDiscrepancy()
{
  m_highchecked = !m_highchecked;
  m_options->setItemChecked(m_highid, m_highchecked);

  emit toggleHighlightDiscrepancy(m_highchecked);
}

void MainWindow::toggleDisplayAllChromo()
{
  m_prefetchchecked = !m_prefetchchecked;
  m_options->setItemChecked(m_prefetch, m_prefetchchecked);

  emit toggleDisplayAllChromo(m_prefetchchecked);
}

void MainWindow::togglePolymorphismView()
{
  m_polychecked = !m_polychecked;
  m_options->setItemChecked(m_polyid, m_polychecked);

  emit togglePolymorphismView(m_polychecked);
}

void MainWindow::toggleUngapped()
{
  m_ungappedchecked = !m_ungappedchecked;
  m_options->setItemChecked(m_ungappedid, m_ungappedchecked);

  int pos = m_gspin->value();

  if (m_ungappedchecked) 
  {
    m_offsetlabel->setText("Position"); 
    m_gspin->setRange(1, m_datastore->m_contig.gap2ungap(m_datastore->m_contig.getLength()));
  }
  else
  { 
    m_offsetlabel->setText("Offset"); 
    m_gspin->setRange(0, m_datastore->m_contig.getLength());
    pos = m_datastore->m_contig.ungap2gap(pos);
  }


  emit toggleShowUngapped(m_ungappedchecked);
  updateGSpin(pos);
}

void MainWindow::updateGSpin(int gindex)
{
  if (m_ungappedchecked)
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
  if (m_ungappedchecked)
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

void MainWindow::setReadnameWidth()
{
  bool ok;
  int res = QInputDialog::getInteger(
            "Hawkeye", "Enter readname width:", 
            m_tiling->getReadnameWidth(), 0, 1000, 2,
            &ok, this );
  if ( ok ) 
  {
    m_tiling->setReadnameWidth(res);
  } 
}
