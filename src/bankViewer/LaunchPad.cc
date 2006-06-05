#include "LaunchPad.hh"

#include <iostream>

#include <qapplication.h>
#include <qfiledialog.h>
#include <qspinbox.h>
#include <qstatusbar.h>
#include <qaccel.h>


#include "InsertWindow.hh"
#include "DataStore.hh"
#include "ChromoPicker.hh"
#include "NetworkCom.hh"
#include "MainWindow.hh"
#include "NChartStats.hh"
#include "NChartWidget.hh"


using namespace AMOS;
using namespace std;


LaunchPad::LaunchPad(QWidget* parent, const char* name, WFlags fl)
 : LaunchPadBase(parent, name, fl)
{
  m_tilingWindow = NULL;
  m_insertWindow = NULL;
  m_chromoPicker = NULL;
  m_gindex = 0;

  m_datastore = new DataStore();

  initFeatures();
  initLibraries();
  initScaffolds();
  initContigs();
  initReads();

  connect(viewButton,    SIGNAL(clicked()), this, SLOT(showAll()));
  connect(tilingButton,  SIGNAL(clicked()), this, SLOT(showTiling()));
  connect(insertButton,  SIGNAL(clicked()), this, SLOT(showInserts()));
  connect(contigIDSpin,  SIGNAL(valueChanged(int)), this, SLOT(setContigId(int)));

  connect(contigSizes,   SIGNAL(idSelected(int)),   this, SLOT(setContigId(int)));
  connect(scaffoldSizes, SIGNAL(idSelected(int)),   this, SLOT(setScaffoldId(int)));
  connect(contigSizes,   SIGNAL(idSelected(int)),   this, SLOT(showAll()));
  connect(scaffoldSizes, SIGNAL(idSelected(int)),   this, SLOT(showAll()));

  connect(orderRadioGroup, SIGNAL(clicked(int)), scaffoldSizes, SLOT(setOrdering(int)));
  connect(orderRadioGroup, SIGNAL(clicked(int)), contigSizes,   SLOT(setOrdering(int)));
  connect(colorRadioGroup, SIGNAL(clicked(int)), scaffoldSizes, SLOT(setColorStyle(int)));
  connect(colorRadioGroup, SIGNAL(clicked(int)), contigSizes,   SLOT(setColorStyle(int)));

  statusBar()->message("No Bank Loaded");
}


LaunchPad::~LaunchPad()
{
  delete m_datastore;
}

void LaunchPad::fileChromoPaths()
{
  if (m_chromoPicker) { m_chromoPicker->close(); }

  m_chromoPicker = new ChromoPicker(m_datastore, this, "chromoPicker");
}

void LaunchPad::addChromoPath(const QString & path)
{
  m_datastore->m_tracepaths.push_back(path.ascii());
}

void LaunchPad::setChromoDB(const QString & db)
{
  if (m_datastore->m_tracedb != db.ascii())
  {
    m_datastore->m_tracedb = db.ascii();
  }
}



void LaunchPad::fileOpen()
{
  QString s = QFileDialog::getExistingDirectory("", this, "Open a Bank", "Open a Bank", TRUE );

  if (!s.isEmpty()) { setBankname(s.ascii()); }
}

void LaunchPad::fileExit()
{
  qApp->quit();
}


void LaunchPad::setBankname(std::string bankname)
{
  if (bankname != "")
  {
    if (!m_datastore->openBank(bankname))
    {
      loadBank();
      contigIDSpin->setRange(1, m_datastore->contig_bank.getSize());
      setContigId(1);
      emit bankSelected();
    }
  }
}


void LaunchPad::loadBank()
{
  loadAssemblyStatistics();
  loadFeatures();
  loadLibraries();
  loadScaffolds();
  loadContigs();
  loadReads();


  NChartStats * scaffstats = new NChartStats((string)"Scaffold Span Distribution");

  if (m_datastore->scaffold_bank.isOpen())
  {
    AMOS::Scaffold_t scaffold;
    m_datastore->scaffold_bank.seekg(1);
    while (m_datastore->scaffold_bank >> scaffold)
    {
      int bid = m_datastore->scaffold_bank.tellg() - 1;
      scaffstats->addSize(bid, scaffold.getSpan());
    }
  }



  NChartStats * contigstats = new NChartStats((string)"Contig Length Distribution");

  if (m_datastore->contig_bank.isOpen())
  {
    AMOS::Contig_t contig;
    m_datastore->contig_bank.seekg(1);
    while (m_datastore->contig_bank >> contig)
    {
      int bid = m_datastore->contig_bank.tellg() - 1;
      contigstats->addSize(bid, contig.getLength());
    }
  }


  if (m_datastore->feat_bank.isOpen())
  {
    try
    {
      Feature_t feat;
      m_datastore->feat_bank.seekg(1);

      while (m_datastore->feat_bank >> feat)
      {
        ID_t iid = feat.getSource().first;
        NCode_t nc = feat.getSource().second;

        if (nc == Contig_t::NCODE)
        {
          try
          {
            contigstats->addScore(m_datastore->contig_bank.lookupBID(iid), 1.0);

            int scaffid = m_datastore->lookupScaffoldId(iid);
            if (scaffid) { scaffstats->addScore(scaffid, 1.0); }
          }
          catch (AMOS::Exception_t & e)
          {
            cerr << "error: " << e << endl;
          }
        }
        else if (nc == Scaffold_t::NCODE)
        {
          try
          {
            scaffstats->addScore(m_datastore->scaffold_bank.lookupBID(iid), 1.0);
          }
          catch (AMOS::Exception_t & e)
          {
            cerr << "error: " << e << endl;
          }
        }
      }
    }
    catch (AMOS::Exception_t & e)
    {
      cerr << "error: " << e << endl;
    }
  }

  scaffoldSizes->setStats(scaffstats);
  contigSizes->setStats(contigstats);
}

void LaunchPad::initDisplay()
{
  statsText->verticalScrollBar()->setValue(0);
}

void LaunchPad::showAll()
{
  showInserts();
  showTiling();
}


void LaunchPad::showInserts()
{
  if (!m_insertWindow)
  {
    m_insertWindow = new InsertWindow(m_datastore, this, "insertWindow");

    connect(m_insertWindow, SIGNAL(setGindex(int)),
            this,           SLOT(setGindex(int)));

    connect(m_insertWindow, SIGNAL(setContigId(int)),
            this,           SLOT(setContigId(int)));

    connect(this,           SIGNAL(setTilingVisibleRange(int, int, int)),
            m_insertWindow, SIGNAL(setTilingVisibleRange(int, int, int)));

    connect(this,           SIGNAL(contigIdSelected(int)),
            m_insertWindow, SLOT(contigChanged()));

    connect(this,           SIGNAL(bankSelected()),
            m_insertWindow, SLOT(bankChanged()));

    connect(this,           SIGNAL(highlightRead(int)),
            m_insertWindow, SIGNAL(highlightRead(int)));
  }

  m_insertWindow->show();
  emit setTilingVisibleRange(m_datastore->m_contigId, m_gindex, m_gindex+100);
  if (m_tilingWindow) { m_tilingWindow->update(); }
}

void LaunchPad::showTiling()
{
  if (!m_tilingWindow)
  {
    m_tilingWindow = new MainWindow(m_datastore, this, "insertWindow");

    connect(this,           SIGNAL(contigIdSelected(int)),
            m_tilingWindow, SLOT(setContigId(int)));

    connect(this,           SIGNAL(gindexSelected(int)),
            m_tilingWindow, SLOT(setGindex(int)));

    connect(m_tilingWindow, SIGNAL(setTilingVisibleRange(int,int,int)),
            this,           SIGNAL(setTilingVisibleRange(int,int,int)));

    connect(this,           SIGNAL(bankSelected()),
            m_tilingWindow, SLOT(bankChanged()));

    connect(m_tilingWindow, SIGNAL(gindexChanged(int)),
            this,           SLOT(setGindex(int)));

    m_tilingWindow->bankChanged();
  }

  m_tilingWindow->setContigId(contigIDSpin->value());
  m_tilingWindow->setGindex(m_gindex);

  m_tilingWindow->show();
}


void LaunchPad::setContigId(int contigId)
{
  if ((contigId != 0) &&
      ((AMOS::ID_t)contigId != m_datastore->m_contigId) &&
      !m_datastore->setContigId(contigId))
  {
    QString s = "Viewing ";
    s += m_datastore->m_bankname.c_str();
    s += " with ";
    s += QString::number(m_datastore->contig_bank.getSize());
    s += " contigs";

    s += " Contig Id:";
    s += QString::number(contigId);
    s += " Size: ";
    s += QString::number(m_datastore->m_contig.getLength());
    s += " Reads: ";
    s += QString::number(m_datastore->m_contig.getReadTiling().size());

    if (contigIDSpin->value() != contigId)
    {
      contigIDSpin->setValue(contigId);
    }

    loadReads();

    emit contigIdSelected(m_datastore->m_contigId);
    emit gindexSelected(0);
    statusBar()->message(s);
  }
}

void LaunchPad::setScaffoldId(int scaffId)
{
  if ((scaffId != 0) &&
      (scaffId != m_datastore->m_scaffoldId) &&
      (m_datastore->scaffold_bank.isOpen()))
  {
    AMOS::Scaffold_t scaffold;
    m_datastore->fetchScaffold(scaffId, scaffold);

    AMOS::ID_t contigiid = scaffold.getContigTiling().begin()->source;
    AMOS::ID_t bid = m_datastore->contig_bank.getIDMap().lookupBID(contigiid);

    setContigId(bid);
  }
}

void LaunchPad::initializeSimpleServer(int port)
{
  SimpleServer * server = new SimpleServer(this, port);
  connect(server, SIGNAL(newConnect(ClientSocket*)),
          SLOT(newConnect(ClientSocket*)));
}

void LaunchPad::newConnect(ClientSocket * s)
{
  statusBar()->message("New Connection");

  connect(s,           SIGNAL(logText(const QString &)),
          statusBar(), SLOT(message(const QString &)));

  connect(s,           SIGNAL(setContigLocation(QString, int)),
          this,        SLOT(setContigLocation(QString, int)));
}

void LaunchPad::setContigLocation(const QString contigid, int pos)
{
  int bid;
  cerr << "set contig location " << contigid << ":" << pos << endl;

  if (contigid[0] == 'E')
  {
    cerr << "Lookup: " << contigid.ascii()+1 << endl;
    bid = m_datastore->contig_bank.getIDMap().lookupBID(contigid.ascii()+1);
  }
  else if (contigid[0] == 'I')
  {
    int iid = atoi(contigid.ascii()+1);

    cerr << "Lookup " << iid << endl;
    bid = m_datastore->contig_bank.getIDMap().lookupBID(iid);
  }
  else
  {
    bid = atoi(contigid.ascii());
  }

  setContigId(bid);
  setGindex(pos);
}


void LaunchPad::enableTraceFetch(bool dofetch)
{
  m_datastore->m_tracecmdenabled = dofetch;
}

void LaunchPad::loadKmers(std::string file)
{
  m_datastore->Read_Mers(file.c_str());
}

void LaunchPad::jumpToRead(int iid)
{
  DataStore::IdLookup_t::iterator rcl = m_datastore->m_readcontiglookup.find(iid);
  if (rcl != m_datastore->m_readcontiglookup.end())
  {
    setContigId(rcl->second);
    vector<AMOS::Tile_t>::iterator ti;
    for (ti = m_datastore->m_contig.getReadTiling().begin();
         ti != m_datastore->m_contig.getReadTiling().end();
         ti++)
    {
      if (ti->source == iid)
      {
        setGindex(ti->offset);
        emit highlightRead(ti->source);
        break;
      }
    }
  }
}

void LaunchPad::setGindex(int gpos)
{
  m_gindex = gpos;

  if (!m_tilingWindow) { emit setTilingVisibleRange(m_datastore->m_contigId, gpos, gpos+100); }
  emit gindexSelected(gpos);
}


