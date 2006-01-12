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
#include <qsplitter.h>

#include "TilingFrame.hh"
#include "InsertWindow.hh"
#include "AssemblyStats.hh"

#include "icons/fontdecrease.xpm"
#include "icons/fontincrease.xpm"

using namespace std;

MainWindow::MainWindow( QWidget *parent, const char *name )
           : QMainWindow( parent, name )
{
  setCaption("Assembly Investigator");
  m_gindex = 0;

  m_contigPicker = NULL;
  m_readPicker = NULL;
  m_libPicker = NULL;
  m_chromoPicker = NULL;
  m_featPicker = NULL;
  m_fontsize = 10;
  m_insertWindow = NULL;
  m_cgraphWindow = NULL;

  m_outervbox = new QVBox(this, "mainvbox");
  setCentralWidget( m_outervbox);

  m_multiTilingWidget = new QWidget(m_outervbox, "multiTilingWidget");
  m_multiTilingWidget->show();

  QVBoxLayout * multiTilingLayout = new QVBoxLayout(m_multiTilingWidget, 0, -1, "mtl");

  AlignmentInfo * ai = new AlignmentInfo();
  m_tiling  = new TilingFrame(&m_datastore, ai, m_multiTilingWidget, "origtilingframe");
  initializeTiling(m_tiling, true);
  m_tiling->show();

  multiTilingLayout->addWidget(m_tiling);


  m_slider = new QScrollBar(Horizontal, m_outervbox, "slider");
  m_slider->setTracking(1);
  m_slider->setPageStep(20);


  // Menubar
  QPopupMenu* file = new QPopupMenu(this);
  menuBar()->insertItem("&File", file);
  file->insertItem("&Open Bank...",     this,  SLOT(chooseBank()));
  file->insertItem("Chromatogram P&aths...",   this,  SLOT(showChromoPicker()));
  file->insertSeparator();
  file->insertItem("&Assembly Statistics...", this, SLOT(showAssemblyStats()));
  file->insertItem("&Contig Picker...", this,  SLOT(chooseContig()));
  file->insertItem("&Scaffold Information...", this,  SLOT(showScaffoldPicker()));
  file->insertItem("&Read Information...",   this,  SLOT(showReadPicker()));
  file->insertItem("&Feature Browser...",   this,  SLOT(showFeatureBrowser()));
  file->insertItem("&Library Information...",   this,  SLOT(showLibPicker()));
  file->insertSeparator();
  file->insertItem("&Quit", qApp,  SLOT(quit()), CTRL+Key_Q );

  //m_indicatorid    = m_options->insertItem("Show &Indicator",          this, SLOT(toggleShowIndicator()));
  //m_posid          = m_options->insertItem("&Show Positions",          this, SLOT(toggleShowPositions()));
  m_indicatorid = -1;
  m_posid = -1;

  m_options = new QPopupMenu(this);
  menuBar()->insertItem("&Options", m_options);
  m_basecolorid    = m_options->insertItem("Color &Bases",             this, SLOT(toggleBaseColors()));
  m_showfullid     = m_options->insertItem("Show &Full Range",         this, SLOT(toggleShowFullRange()));
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
  bPrevDisc->setAccel(CTRL + Key_Space + SHIFT);
  bPrevDisc->setMinimumWidth(20);

  QToolButton * bNextDisc = new QToolButton(Qt::RightArrow, status, "next");
  bNextDisc->setTextLabel("Next Discrepancy");
  bNextDisc->setAccel(CTRL + Key_Space);
  bNextDisc->setMinimumWidth(20);

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


  new QLabel("Find", searchbar, "findlbl");
  m_searchedit = new QLineEdit(searchbar, "searchbox");
  connect(m_searchedit, SIGNAL(returnPressed()), this, SLOT(findNext()));

  QToolButton * bFindPrev = new QToolButton(Qt::LeftArrow, searchbar, "rsearch");
  connect(bFindPrev, SIGNAL(clicked()), this, SLOT(findPrev()));
  bFindPrev->setTextLabel("Find Previous");
  bFindPrev->setAccel(SHIFT + CTRL + Key_F);
  bFindPrev->setMinimumWidth(20);

  QToolButton * bFindNext = new QToolButton(Qt::RightArrow, searchbar, "fsearch");
  connect(bFindNext, SIGNAL(clicked()), this, SLOT(findNext()));
  bFindNext->setTextLabel("Find Next");
  bFindNext->setAccel(CTRL + Key_F);
  bFindNext->setMinimumWidth(20);


  // gindex

  connect(this,     SIGNAL(gindexChanged(int)),
          m_slider, SLOT(setValue(int)));

  connect(m_slider, SIGNAL(valueChanged(int)),
          this,     SLOT(setGindex(int)));

  connect(this,     SIGNAL(gindexChanged(int)),
          m_gspin, SLOT(setValue(int)));

  connect(m_gspin, SIGNAL(valueChanged(int)),
          this,     SLOT(setGindex(int)));
  



  // contigid
  connect(m_contigid, SIGNAL(valueChanged(int)),
          this,       SLOT(setContigId(int)));

  connect(this,       SIGNAL(contigIdSelected(int)),
          m_contigid, SLOT(setValue(int)));


  // dbpick <-> tiling
  connect(dbpick, SIGNAL(textChanged(const QString &)),
          this,   SLOT(setChromoDB(const QString &)));

  connect(this, SIGNAL(chromoDBSet(const QString &)),
          dbpick, SLOT(setText(const QString &)));


  // Set defaults
  m_gspin->setValue(0);
  m_slider->setFocus();
  dbpick->setText("GB6");

  // If not mpop, enable
  //if (strcmp(getenv("USER"), "mpop") && strcmp(getenv("USER"), "mschatz"))
  if (strcmp(getenv("USER"), "mpop"))
  {
    toggleBaseColors();
  }

  toggleShowIndicator();
  toggleHighlightDiscrepancy();

#if 0
  ReferenceAlignment ra0;
  ra0.m_reference = "1047283846736";
  ra0.m_query = "1";
  ra0.m_rstart = 1;
  ra0.m_rend = 1834;
  ra0.m_qstart = 1;
  ra0.m_qend = 1834;
  ra0.m_percentid = 100.0;
  ra0.m_chain = 0;
  m_referenceAlignments.insert(make_pair(ra0.m_reference, ra0));

  ReferenceAlignment ra1;
  ra1.m_reference = "1047283846736";
  ra1.m_query = "2";
  ra1.m_rstart = 2338;
  ra1.m_rend = 14443;
  ra1.m_qstart = 1;
  ra1.m_qend = 12106;
  ra1.m_percentid = 100.0;
  ra0.m_chain = 1;
  m_referenceAlignments.insert(make_pair(ra1.m_reference, ra1));

  ReferenceAlignment ra2;
  ra2.m_reference = "1047283846736";
  ra2.m_query = "3";
  ra2.m_rstart = 15340;
  ra2.m_rend = 22333;
  ra2.m_qstart = 1;
  ra2.m_qend = 6994;
  ra2.m_percentid = 100.0;
  ra0.m_chain = 2;
  m_referenceAlignments.insert(make_pair(ra2.m_reference, ra2));

  ReferenceAlignment ra3;
  ra3.m_reference = "1047283846736";
  ra3.m_query = "4";
  ra3.m_rstart = 22609;
  ra3.m_rend = 540338;
  ra3.m_qstart = 1;
  ra3.m_qend = 517730;
  ra3.m_percentid = 100.0;
  ra0.m_chain = 3;
  m_referenceAlignments.insert(make_pair(ra3.m_reference, ra3));
#endif
}

void MainWindow::initializeTiling(TilingFrame * tiling, bool isReference)
{
  connect(this,      SIGNAL(gindexChanged(int)),
          tiling,      SLOT(setGindex(int)));

  connect(this,      SIGNAL(highlightRead(int)),
          tiling,      SLOT(highlightRead(int)));

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

  connect(this,      SIGNAL(toggleShowIndicator(bool)),
          tiling,    SIGNAL(toggleShowIndicator(bool)));

  connect(this,      SIGNAL(toggleSNPColoring(bool)),
          tiling,    SIGNAL(toggleSNPColoring(bool)));

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
  }
}

void MainWindow::initializeSimpleServer(int port)
{
  SimpleServer * server = new SimpleServer(this, port);
  connect(server, SIGNAL(newConnect(ClientSocket*)),
          SLOT(newConnect(ClientSocket*)));
}

void MainWindow::newConnect(ClientSocket * s)
{
  statusBar()->message("New Connection");

  connect(s,           SIGNAL(logText(const QString &)),
          statusBar(), SLOT(message(const QString &)));

  connect(s,           SIGNAL(setContigLocation(QString, int)),
          this,        SLOT(setContigLocation(QString, int)));
}

void MainWindow::setContigLocation(const QString contigid, int pos)
{
  int bid;
  cerr << "set contig location " << contigid << ":" << pos << endl;

  if (contigid[0] == 'E')
  {
    cerr << "Lookup: " << contigid.ascii()+1 << endl;
    bid = m_datastore.contig_bank.getIDMap().lookupBID(contigid.ascii()+1);
  }
  else if (contigid[0] == 'I')
  {
    int iid = atoi(contigid.ascii()+1);

    cerr << "Lookup " << iid << endl;
    bid = m_datastore.contig_bank.getIDMap().lookupBID(iid);
  }
  else
  {
    bid = atoi(contigid.ascii());
  }

  setContigId(bid);
  setGindex(pos);
}

void MainWindow::jumpToRead(int iid)
{
  DataStore::IdLookup_t::iterator rcl = m_datastore.m_readcontiglookup.find(iid);
  if (rcl != m_datastore.m_readcontiglookup.end())
  {
    setContigId(rcl->second);
    vector<AMOS::Tile_t>::iterator ti;
    for (ti = m_datastore.m_contig.getReadTiling().begin();
         ti != m_datastore.m_contig.getReadTiling().end();
         ti++)
    {
      if (ti->source == iid)
      {
        emit setGindex(ti->offset);
        emit highlightRead(ti->source);
        break;
      }
    }
  }
}



void MainWindow::chooseBank()
{
  QString s = QFileDialog::getExistingDirectory("", this,
                                                "Open a Bank",
                                                "Open a Bank",
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
    m_insertWindow = new InsertWindow(&m_datastore, this, "insertWindow");

    connect(m_insertWindow, SIGNAL(setGindex(int)),
            this,           SLOT(setGindex(int)));

    connect(m_insertWindow, SIGNAL(setContigId(int)),
            this,           SLOT(setContigId(int)));

    connect(m_tiling,       SIGNAL(setTilingVisibleRange(int, int, int)),
            m_insertWindow, SIGNAL(setTilingVisibleRange(int, int, int)));

    connect(this,           SIGNAL(contigIdSelected(int)),
            m_insertWindow, SLOT(contigChanged()));

    connect(this,           SIGNAL(bankSelected()),
            m_insertWindow, SLOT(bankChanged()));

    connect(this,           SIGNAL(highlightRead(int)),
            m_insertWindow, SIGNAL(highlightRead(int)));
  }

  m_insertWindow->show();
  m_tiling->update();
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

  connect(this,           SIGNAL(bankSelected()),
          m_contigPicker, SLOT(refreshTable()));
}


void MainWindow::showScaffoldPicker()
{
  ScaffoldPicker * scaffoldPicker = new ScaffoldPicker(&m_datastore, this, "scaffoldpicker");

  connect(scaffoldPicker, SIGNAL(contigSelected(int)),
          this,           SLOT(setContigId(int)));
  connect(scaffoldPicker, SIGNAL(setGindex(int)),
          this,           SLOT(setGindex(int)));

  connect(this,           SIGNAL(bankSelected()),
          scaffoldPicker, SLOT(refreshTable()));
}

void MainWindow::setContigId(int contigId)
{
  if ((contigId != 0) &&
      ((AMOS::ID_t)contigId != m_datastore.m_contigId) &&
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
    s += QString::number(m_datastore.m_contig.getLength());
    s += " Reads: ";
    s += QString::number(m_datastore.m_contig.getReadTiling().size());



    int m_showalignments = 0;
    if (m_showalignments)
    {
      vector<ReferenceAlignment> alignments;
      ReferenceAlignments::iterator ri = m_referenceAlignments.find(m_datastore.m_contig.getEID());

      while (ri != m_referenceAlignments.end() && ri->first == m_datastore.m_contig.getEID())
      {
        cerr << "Found alignment r:" << ri->second.m_reference << " q:" << ri->second.m_query << endl;
        alignments.push_back(ri->second);
        ri++;
      }

      delete m_multiTilingWidget->layout();
      QVBoxLayout * multiTilingLayout = new QVBoxLayout(m_multiTilingWidget, 0, -1,  "mtl2");
      QSplitter * splitter = new QSplitter(Qt::Vertical, m_multiTilingWidget, "splitter");
      splitter->show();
      multiTilingLayout->addWidget(splitter);

      vector<ReferenceAlignment>::iterator ai;
      cerr << "r";
      AlignmentInfo * rai = new AlignmentInfo();

      if (!alignments.empty())
      {
        rai->m_gaps.push_back(143);
        rai->m_gaps.push_back(3790);
      }
      
      m_tiling  = new TilingFrame(&m_datastore, rai, splitter, "reftiling");
      initializeTiling(m_tiling, true);
      m_tiling->show();

      for (ai =  alignments.begin();
           ai != alignments.end();
           ai++)
      {
        cerr << "q";
        DataStore * qs = new DataStore;
        qs->openBank("amoscmp.bnk");
        qs->setContigId(qs->contig_bank.getIDMap().lookupBID(ai->m_query.c_str()));

        AlignmentInfo * qai = new AlignmentInfo();

        int count = 0;
        for (int i = 0; i < rai->m_gaps.size(); i++)
        {
          if (rai->m_gaps[i] < ai->m_rstart) { count++; }
        }

        qai->m_startshift = ai->m_rstart - ai->m_qstart + count;
        qai->m_endshift = m_datastore.m_contig.getLength() - ai->m_rend;

        TilingFrame * tiling = new TilingFrame(qs, qai, splitter, "querytiling");
        tiling->setContigId(qs->m_contigId);
        initializeTiling(tiling, false);
        tiling->show();
      }
    }

    emit contigIdSelected(m_datastore.m_contigId);
    statusBar()->message(s);
  }
}

void MainWindow::setBankname(std::string bankname)
{
  if (bankname != "")
  {
    if (!m_datastore.openBank(bankname))
    {
      m_contigid->setRange(1, m_datastore.contig_bank.getSize());
      setContigId(1);
      emit bankSelected();
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
  if (m_datastore.m_tracedb != db.ascii())
  {
    m_datastore.m_tracedb = db.ascii();
    emit chromoDBSet(db);
  }
}

void MainWindow::showReadPicker()
{
  if (m_readPicker) { m_readPicker->close(); }

  m_readPicker = new ReadPicker(&m_datastore, this, "readPicker");
  connect(m_readPicker, SIGNAL(highlightRead(int)),
          this,         SIGNAL(highlightRead(int)));

  connect(this,         SIGNAL(contigIdSelected(int)),
          m_readPicker,   SLOT(contigIdSelected(int)));
}

void MainWindow::showLibPicker()
{
  if (m_libPicker) { m_libPicker->close(); }

  m_libPicker = new LibraryPicker(&m_datastore, this, "libPicker");
  connect(this, SIGNAL(bankSelected()),
          m_libPicker, SLOT(refreshTable()));

}

void MainWindow::showAssemblyStats()
{
  AssemblyStats * asmStats = new AssemblyStats(&m_datastore, this, "libPicker");
  connect(this, SIGNAL(bankSelected()),
          asmStats, SLOT(refreshTable()));

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

  connect(this,           SIGNAL(bankSelected()),
          m_featPicker,   SLOT(refreshTable()));
}

void MainWindow::addChromoPath(const QString & path)
{
  m_datastore.m_tracepaths.push_back(path.ascii());
}

void MainWindow::addChromoDB(const QString & db)
{
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

void MainWindow::enableTraceFetch(bool dofetch)
{
  m_datastore.m_tracecmdenabled = dofetch;
}
