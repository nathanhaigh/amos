#include "ScaffoldPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qtoolbar.h>
#include <qlineedit.h>
#include <qmenubar.h>

#include "foundation_AMOS.hh"
#include "RenderSeq.hh"
#include "DataStore.hh"

#include "InsertStats.hh"
#include "HistogramWindow.hh"


#include <vector>

using namespace std;



class ScaffoldListItem : public QListViewItem
{
public:

  ScaffoldListItem(QListView * parent, 
                   QString id,
                   QString iid,
                   QString eid,
                   QString length,
                   QString contigs,
                   QString offset)
               
    : QListViewItem(parent, id, iid, eid, length, contigs, offset) {}

  ScaffoldListItem(ScaffoldListItem * parent, 
                   QString id,
                   QString iid,
                   QString eid,
                   QString length,
                   QString contigs,
                   QString offset)
               
    : QListViewItem(parent, id, iid, eid, length, contigs, offset) {}

  int compare(QListViewItem *i, int col,
              bool ascending ) const
  {
    if (col == 2)
    {
      return key(col,ascending).compare(i->key(col,ascending));
    }

    return atoi(key(col,ascending)) - atoi(i->key(col,ascending));
  }
};


void LaunchPad::initScaffolds()
{
  connect(scaffoldList,    SIGNAL(doubleClicked(QListViewItem *)),
          this,            SLOT(scaffoldSelected(QListViewItem *)));

  connect(scaffoldList,    SIGNAL(returnPressed(QListViewItem *)),
          this,            SLOT(scaffoldSelected(QListViewItem *)));

  connect(scaffoldIIDEdit, SIGNAL(textChanged(const QString &)),
          this,            SLOT(scaffoldSelectIID(const QString &)));

  connect(scaffoldEIDEdit, SIGNAL(textChanged(const QString &)),
          this,            SLOT(scaffoldSelectEID(const QString &)));

  connect(scaffoldIIDEdit, SIGNAL(returnPressed()),
          this,            SLOT(scaffoldViewSelected()));

  connect(scaffoldEIDEdit, SIGNAL(returnPressed()),
          this,            SLOT(scaffoldViewSelected()));

  connect(scaffoldSpanButton, SIGNAL(clicked()),
          this,               SLOT(scaffoldSpanHistogram()));
  
  connect(scaffoldContigsButton, SIGNAL(clicked()),
          this,                  SLOT(scaffoldContigHistogram()));
}

void LaunchPad::loadScaffolds()
{
  scaffoldList->clear();
  int c = scaffoldList->columns();
  for (int i = 0; i < c; i++)
  {
    scaffoldList->removeColumn(0);
  }

  scaffoldList->addColumn("Id");
  scaffoldList->addColumn("IID");
  scaffoldList->addColumn("EID");
  scaffoldList->addColumn("Span");
  scaffoldList->addColumn("Contigs");
  scaffoldList->addColumn("Offset");

  if (!m_datastore->scaffold_bank.isOpen())
  {
    QString status = "Scaffold Information not available";
    statusBar()->message(status);
    return;
  }

  QCursor orig = cursor();
  setCursor(Qt::waitCursor);

  try
  {
    AMOS::Contig_t contig;
    AMOS::Scaffold_t scaffold;

    QString status = "Select from " + 
                     QString::number(m_datastore->scaffold_bank.getSize()) + 
                     " scaffolds in " + m_datastore->m_bankname.c_str();
    statusBar()->message(status);

    int scaffid = 1;
    m_datastore->scaffold_bank.seekg(1);
    while (m_datastore->scaffold_bank >> scaffold)
    {
      int span = scaffold.getSpan();
      int numcontigs = scaffold.getContigTiling().size();

      ScaffoldListItem * scaffolditem;

      scaffolditem = new ScaffoldListItem(scaffoldList,  
                                          QString::number(scaffid), 
                                          QString::number(scaffold.getIID()),
                                          QString(scaffold.getEID().c_str()), 
                                          QString::number(span), 
                                          QString::number(numcontigs),
                                          "");

      scaffid++;

      vector<AMOS::Tile_t> & ctiling = scaffold.getContigTiling();

      vector<AMOS::Tile_t>::iterator ti; 	 
      for (ti =  ctiling.begin();
           ti != ctiling.end();
           ti++) 	 
      { 	 
        QString oo = ti->range.isReverse() ? "EB" : "BE";
        new ScaffoldListItem(scaffolditem, 	 
                             QString::number(m_datastore->contig_bank.getIDMap().lookupBID(ti->source)),
                             QString::number(ti->source), 	 
                             QString(m_datastore->contig_bank.lookupEID(ti->source).c_str()),
                             QString::number(ti->range.getLength() + ti->gaps.size()),
                             oo,
                             QString::number(ti->offset));
      }
    }
  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  scaffoldList->setSorting(3, false);

  setCursor(orig);
}

void LaunchPad::scaffoldSelected(QListViewItem * item)
{
  int offset = 0;
  if (item->parent())
  {
    offset = 0;
    setContigId(atoi(item->text(0)));
    setGindex(offset);
  }
  else
  {
    AMOS::Scaffold_t scaffold;
    m_datastore->fetchScaffold(atoi(item->text(0)), scaffold);

    AMOS::ID_t contigiid = scaffold.getContigTiling().begin()->source;
    AMOS::ID_t bid = m_datastore->contig_bank.getIDMap().lookupBID(contigiid);

    setContigId(bid);
    setGindex(offset);
  }
}

void LaunchPad::scaffoldSelectIID(const QString & iid)
{
  QListViewItem * item = scaffoldList->findItem(iid, 1);
  if (item)
  {
    scaffoldList->setSelected(item, true);
    scaffoldList->ensureItemVisible(item);
  }
}

void LaunchPad::scaffoldSelectEID(const QString & eid)
{
  QListViewItem * item = scaffoldList->findItem(eid, 2);
  if (item)
  {
    scaffoldList->setSelected(item, true);
    scaffoldList->ensureItemVisible(item);
  }
}

void LaunchPad::scaffoldViewSelected()
{
  QListViewItem * item = scaffoldList->selectedItem();
  if (item)
  {
    scaffoldSelected(item);
  }
}


void LaunchPad::scaffoldSpanHistogram()
{
  InsertStats * stats = new InsertStats((string)"Scaffold Span Histogram");

  AMOS::Scaffold_t scaffold;
  m_datastore->scaffold_bank.seekg(1);
  while (m_datastore->scaffold_bank >> scaffold)
  {
    stats->addSize(scaffold.getSpan());
  }

  new HistogramWindow(stats, this, "hist");
}

void LaunchPad::scaffoldContigHistogram()
{
  InsertStats * stats = new InsertStats((string)"Scaffold Contig Count Histogram");

  AMOS::Scaffold_t scaffold;
  m_datastore->scaffold_bank.seekg(1);
  while (m_datastore->scaffold_bank >> scaffold)
  {
    int span = scaffold.getSpan();
    stats->addSize(scaffold.getContigTiling().size());
  }

  new HistogramWindow(stats, this, "hist");
}

