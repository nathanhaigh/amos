#include "LibraryPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qmenubar.h>
#include <qpopupmenu.h>

#include "foundation_AMOS.hh"

#include <vector>

#include "Insert.hh"
#include "InsertStats.hh"
#include "DataStore.hh"
#include "HistogramWindow.hh"


using namespace std;
using namespace AMOS;



class LibListItem : public QListViewItem
{
public:
  LibListItem(QListView * parent, 
              QString iid,
              QString eid,
              QString mean,
              QString sd,
              QString count)
               
    : QListViewItem(parent, iid, eid, mean, sd, count) { }


  int compare(QListViewItem *i, int col,
              bool ascending ) const
  {
    if (col == 1)
    {
      return key(col,ascending).compare(i->key(col,ascending));
    }
    else
    {
      return atoi(key(col,ascending)) - atoi(i->key(col,ascending));
    }
  }
};


LibraryPicker::LibraryPicker(DataStore * datastore,
                       QWidget * parent, 
                       const char * name)
  :QMainWindow(parent, name)
{
  m_table = new QListView(this, "LibraryPickertbl");
  setCentralWidget(m_table);
  setCaption("Library Information");
  resize(550,500);
  show();

  QPopupMenu * menu = new QPopupMenu(this);
  menuBar()->insertItem("&Display", menu);
  menu->insertItem("&Insert Size Histogram", this, SLOT(acceptSelected()));
  menu->insertItem("&Clear Range Length Histogram", this, SLOT(readLengthSelected()));

  QToolBar * tool = new QToolBar(this, "tools");
  new QLabel("IID:", tool, "iidlbl");
  QLineEdit * iidpick = new QLineEdit(tool, "iidpick");

  new QLabel("EID:", tool, "eidlbl");
  QLineEdit * eidpick = new QLineEdit(tool, "eidpick");

  connect(m_table, SIGNAL(doubleClicked(QListViewItem *)),
          this,  SLOT(itemSelected(QListViewItem *)));

  connect(m_table, SIGNAL(returnPressed(QListViewItem *)),
          this,  SLOT(itemSelected(QListViewItem *)));

  connect(iidpick, SIGNAL(textChanged(const QString &)),
          this,    SLOT(selectiid(const QString &)));

  connect(eidpick, SIGNAL(textChanged(const QString &)),
          this,    SLOT(selecteid(const QString &)));

  connect(eidpick, SIGNAL(returnPressed()),
          this,    SLOT(acceptSelected()));

  connect(iidpick, SIGNAL(returnPressed()),
          this,    SLOT(acceptSelected()));


  m_table->addColumn("IID");
  m_table->addColumn("EID");
  m_table->addColumn("Mean");
  m_table->addColumn("Stdev");
  m_table->addColumn("Inserts");

  m_table->setShowSortIndicator(true);
  m_table->setRootIsDecorated(true);
  m_table->setAllColumnsShowFocus(true);

  m_datastore = datastore;

  loadTable();
}

void LibraryPicker::loadTable()
{
  m_table->clear();
  QCursor orig = cursor();

  if (!m_datastore->m_libdistributionlookup.empty())
  {
    setCursor(Qt::waitCursor);

    try
    {
      m_datastore->lib_bank.seekg(1);

      AMOS::Library_t lib;
      int count = 0;

      while (m_datastore->lib_bank >> lib)
      {
        AMOS::Distribution_t dist = lib.getDistribution();

        ID_t libid = lib.getIID();
        int frag = 0;

        DataStore::IdLookup_t::iterator li;
        for (li = m_datastore->m_fragliblookup.begin();
             li != m_datastore->m_fragliblookup.end();
             li++)
        {
          if (li->second == libid)
          {
            frag++;
          }
        }

        new LibListItem(m_table,
                        QString::number(libid),
                        lib.getEID().c_str(),
                        QString::number(dist.mean),
                        QString::number(dist.sd),
                        QString::number(frag));
        count++;
      }

      QString status = QString::number(count) + " libraries";
      statusBar()->message(status);
    }
    catch (AMOS::Exception_t & e)
    {
      cerr << "Library Information not available " << e;
    }

    setCursor(orig);
  }
  else
  {
    statusBar()->message("Library Information not available");
  }
}

void LibraryPicker::itemSelected(QListViewItem * item)
{
  ID_t libid = atoi(item->text(0));

  char buffer[32];
  sprintf(buffer, "%d", libid);
  InsertStats * stats = new InsertStats((string)"Insert Size Histogram for Library " + buffer);

  Contig_t contig;
  m_datastore->contig_bank.seekg(1);
  while (m_datastore->contig_bank >> contig)
  {
    vector<Insert *> inserts;
    vector<Insert *>::iterator vi;
    m_datastore->calculateInserts(contig.getReadTiling(), inserts, 1, 0);

    for (vi = inserts.begin(); vi != inserts.end(); vi++)
    {
      if (((*vi)->m_libid == libid) && ((*vi)->ceConnected()))
      {
        stats->addSize((*vi)->m_actual);
      }

      delete *vi;
    }
  }

  new HistogramWindow(stats, this, "hist");
}

void LibraryPicker::selectiid(const QString & iid)
{
  QListViewItem * item = m_table->findItem(iid, 0);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void LibraryPicker::selecteid(const QString & eid)
{
  QListViewItem * item = m_table->findItem(eid, 1);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void LibraryPicker::acceptSelected()
{
  QListViewItem * item = m_table->selectedItem();
  if (item)
  {
    itemSelected(item);
  }
}


void LibraryPicker::refreshTable()
{
  loadTable();
}

void LibraryPicker::readLengthSelected()
{
  QListViewItem * item = m_table->selectedItem();
  if (item)
  {
    ID_t libid = atoi(item->text(0));

    char buffer[32];
    sprintf(buffer, "%d", libid);
    InsertStats * stats = new InsertStats((string)"Reads in Contigs Clear Range Length Histogram for Library " + buffer);

    Contig_t contig;
    m_datastore->contig_bank.seekg(1);
    while (m_datastore->contig_bank >> contig)
    {
      vector<Tile_t>::iterator ti;
      for (ti = contig.getReadTiling().begin(); 
           ti != contig.getReadTiling().end();
           ti++)
      {
        if (m_datastore->getLibrary(ti->source) == libid)
        {
          stats->addSize(ti->range.getLength());
        }
      }
    }

    new HistogramWindow(stats, this, "hist");
  }
}
