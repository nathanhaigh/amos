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
  m_qvid     = m_options->insertItem("&Show Quality Values",     this, SLOT(toggleShowQV()));
  m_highid   = m_options->insertItem("&Highlight Discrepancies", this, SLOT(toggleHighlightDiscrepancy()));
  m_prefetch = m_options->insertItem("&Prefetch Chromatograms",  this, SLOT(togglePrefetchChromatograms()));

  // Status Bar
  statusBar()->message("No Bank Loaded");

  // Status Tool Bar
  QToolBar * status = new QToolBar(this, "Status");
  status->setLabel("Status");

  QLabel * lbl = new QLabel("Position", status, "gindexlbl");
  m_gindex     = new QSpinBox(0,100, 1, status, "gindexspin");
  m_gindex->setMinimumWidth(50);

  QToolButton * bNextDisc = new QToolButton(Qt::RightArrow, status, "next");

  new QLabel("   Contig ID", status, "contiglbl");
  m_contigid  = new QSpinBox(1, 1, 1, status, "contigid");

  new QLabel("   Database", status, "dblbl");
  QLineEdit *  dbpick  = new QLineEdit("DMG", status, "dbpick");

  QToolButton * bShowInserts = new QToolButton(QPixmap(), "Show Inserts", "Show Inserts", 
                                               m_tiling, SLOT(showInserts()), status );
  bShowInserts->setText("Show Inserts");

  new QLabel("   Font Size", status, "fontlbl");
  QSpinBox * fontsize  = new QSpinBox(6, 24, 1, status, "fontsize");


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

  // fontsize <-> tiling
  connect(fontsize, SIGNAL(valueChanged(int)), 
          m_tiling,   SLOT(setFontSize(int)));

  // checkboxes <-> tiling
//  connect(stable, SIGNAL(toggled(bool)),
//          tiling, SLOT(toggleStable(bool)));

  connect(bNextDisc, SIGNAL(clicked()),
          m_tiling,    SLOT(advanceNextDiscrepancy()));


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
  fontsize->setValue(12);
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

class ContigListItem : public QListViewItem
{
public:
  ContigListItem(QListView * parent, 
                 QString id,
                 QString type,
                 QString offset,
                 QString length,
                 QString elements)
               
    : QListViewItem(parent, id, type, offset, length, elements) {}

  ContigListItem(QListViewItem * parent, 
                 QString id,
                 QString type,
                 QString offset,
                 QString length,
                 QString elements)
               
    : QListViewItem(parent, id, type, offset, length, elements) {}

  int compare(QListViewItem *i, int col,
              bool ascending ) const
  {
    return atoi(key(col,ascending)) - atoi(i->key(col,ascending));
  }
};

void MainWindow::chooseContig()
{
  if (m_contigPicker) { delete m_contigPicker; }

  m_contigPicker = new QMainWindow(this);

  QListView * table = new QListView(m_contigPicker, "contigpicker");
  table->resize(500,500);
  m_contigPicker->setCentralWidget(table);
  m_contigPicker->resize(300,300);
  m_contigPicker->show();

  connect(table, SIGNAL(doubleClicked(QListViewItem *)),
          this,  SLOT(contigSelected(QListViewItem *)));

  table->addColumn("Id");
  table->addColumn("Type");
  table->addColumn("Offset");
  table->addColumn("Length");
  table->addColumn("Elements");
  table->setShowSortIndicator(true);
  table->setRootIsDecorated(true);

  try
  {
    QCursor orig = m_contigPicker->cursor();
    m_contigPicker->setCursor(Qt::waitCursor);
    AMOS::Contig_t contig;
    AMOS::BankStream_t contig_bank(AMOS::Contig_t::NCODE);

    contig_bank.open(m_bankname);

    QString status = "Select from " + 
                     QString::number(contig_bank.getSize()) + 
                     " contigs in " + m_bankname.c_str();
    m_contigPicker->statusBar()->message(status);

    vector<ContigListItem> contigs;

    int contigid = 1;
    while (contig_bank >> contig)
    {
      int contiglen = contig.getSeqString().length();
      int numreads = contig.getReadTiling().size();

      //ContigListItem * contigitem = new ContigListItem(table,  
      new ContigListItem(table,  
                         QString::number(contigid), 
                         "Contig",
                         "0",
                         QString::number(contiglen), 
                         QString::number(numreads));

      /*
      vector<AMOS::Tile_t>::iterator ti;
      for (ti =  contig.getReadTiling().begin();
           ti != contig.getReadTiling().end();
           ti++)
      {
        new ContigListItem(contigitem,
                           QString::number(ti->source),
                           "Read",
                           QString::number(ti->offset),
                           QString::number(ti->range.getLength() + ti->gaps.size()),
                           " ");
      }
      */
      
      contigid++;
    }
    m_contigPicker->setCursor(orig);

  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }
}

void MainWindow::contigSelected(QListViewItem * item)
{
  emit contigIdSelected(atoi(item->text(0)));
  m_contigPicker->close();
  m_contigPicker = NULL;
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

