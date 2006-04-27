#include "LaunchPad.hh"
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

void LaunchPad::initLibraries()
{
  connect(libraryList, SIGNAL(doubleClicked(QListViewItem *)),
          this,        SLOT(librarySelected(QListViewItem *)));

  connect(libraryList, SIGNAL(returnPressed(QListViewItem *)),
          this,        SLOT(librarySelected(QListViewItem *)));

  connect(libraryInsertButton, SIGNAL(clicked()),
          this,                SLOT(libraryViewSelected()));

  connect(libraryClearLengthButton, SIGNAL(clicked()),
          this,                     SLOT(libraryClearLengthSelected()));

  connect(libraryReadLengthButton, SIGNAL(clicked()),
          this,                    SLOT(libraryReadLengthSelected()));

  connect(libraryGCButton,         SIGNAL(clicked()),
          this,                    SLOT(libraryGCSelected()));

  libraryList->addColumn("IID");
  libraryList->addColumn("EID");
  libraryList->addColumn("Mean");
  libraryList->addColumn("Stdev");
  libraryList->addColumn("Inserts");

  libraryList->setShowSortIndicator(true);
  libraryList->setRootIsDecorated(true);
  libraryList->setAllColumnsShowFocus(true);
}


void LaunchPad::loadLibraries()
{
  libraryList->clear();
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

        new LibListItem(libraryList,
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

void LaunchPad::librarySelected(QListViewItem * item)
{
  ID_t libid = atoi(item->text(0));

  char buffer[32];
  sprintf(buffer, "%d", libid);
  InsertStats * stats = new InsertStats((string)"Insert Size Histogram for Library " + buffer + " (In Contigs)");

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

void LaunchPad::libraryViewSelected()
{
  QListViewItem * item = libraryList->selectedItem();
  if (item)
  {
    librarySelected(item);
  }
}



void LaunchPad::libraryGCSelected()
{
  QListViewItem * item = libraryList->selectedItem();
  if (item)
  {
    QCursor orig = cursor();
    setCursor(Qt::waitCursor);

    ID_t libid = atoi(item->text(0));

    char buffer[32];
    sprintf(buffer, "%d", libid);
    InsertStats * stats = new InsertStats((string)"GC Content Histogram for Library " + buffer + " (All Reads)");

    Read_t red;
    m_datastore->read_bank.seekg(1);

    while (m_datastore->read_bank >> red)
    {
      if (m_datastore->getLibrary(red.getIID()) == libid)
      {
        stats->addSize(red.getGCContent(red.getClearRange()));
      }
    }

    new HistogramWindow(stats, this, "hist");
    setCursor(orig);
  }
}

void LaunchPad::libraryClearLengthSelected()
{
  QListViewItem * item = libraryList->selectedItem();
  if (item)
  {
    QCursor orig = cursor();
    setCursor(Qt::waitCursor);

    ID_t libid = atoi(item->text(0));

    char buffer[32];
    sprintf(buffer, "%d", libid);
    InsertStats * stats = new InsertStats((string)"Clear Range Length Histogram for Library " + buffer + " (All Reads)");

    Read_t red;
    m_datastore->read_bank.seekg(1);

    while (m_datastore->read_bank >> red)
    {
      if (m_datastore->getLibrary(red.getIID()) == libid)
      {
        stats->addSize(red.getClearRange().getLength());
      }
    }

    new HistogramWindow(stats, this, "hist");
    setCursor(orig);
  }
}

void LaunchPad::libraryReadLengthSelected()
{
  QListViewItem * item = libraryList->selectedItem();
  if (item)
  {
    QCursor orig = cursor();
    setCursor(Qt::waitCursor);

    ID_t libid = atoi(item->text(0));

    char buffer[32];
    sprintf(buffer, "%d", libid);
    InsertStats * stats = new InsertStats((string)"Full Range Length Histogram for Library " + buffer + " (All Reads)");

    Read_t red;
    m_datastore->read_bank.seekg(1);

    while (m_datastore->read_bank >> red)
    {
      if (m_datastore->getLibrary(red.getIID()) == libid)
      {
        stats->addSize(red.getLength());
      }
    }

    new HistogramWindow(stats, this, "hist");
    setCursor(orig);
  }
}
