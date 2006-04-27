#include "LaunchPad.hh"

#include <iostream>

#include <qapplication.h>
#include <qfiledialog.h>
#include <qspinbox.h>
#include <qstatusbar.h>


#include "InsertWindow.hh"
#include "DataStore.hh"
#include "ChromoPicker.hh"
#include "NetworkCom.hh"
#include "MainWindow.hh"


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

  connect(tilingButton, SIGNAL(clicked()), this, SLOT(showTiling()));
  connect(insertButton, SIGNAL(clicked()), this, SLOT(showInserts()));
  connect(contigIDSpin, SIGNAL(valueChanged(int)), this, SLOT(setContigId(int)));


  // Status Bar
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
    statsText->setContentsPos(0,0);
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
  emit gindexSelected(gpos);
}


