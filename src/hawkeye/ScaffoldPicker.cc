#include "ScaffoldPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <q3toolbar.h>
#include <qlineedit.h>
#include <qmenubar.h>

#include "foundation_AMOS.hh"
#include "RenderSeq.hh"
#include "DataStore.hh"

#include "InsertStats.hh"
#include "NChartStats.hh"
#include "HistogramWindow.hh"
#include "NChartWindow.hh"
#include "amp.hh"


#include <vector>

using namespace std;
using namespace AMOS;



class ScaffoldListItem : public Q3ListViewItem
{
public:

  ScaffoldListItem(Q3ListView * parent, 
                   QString id,
                   QString iid,
                   QString eid,
                   QString span,
                   QString bases,
                   QString offset)
               
    : Q3ListViewItem(parent, id, iid, eid, span, bases, offset) {}

  ScaffoldListItem(ScaffoldListItem * parent, 
                   QString id,
                   QString iid,
                   QString eid,
                   QString span,
                   QString bases,
                   QString offset)
               
    : Q3ListViewItem(parent, id, iid, eid, span, bases, offset) {}

  int compare(Q3ListViewItem *i, int col,
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
  connect(scaffoldList,    SIGNAL(doubleClicked(Q3ListViewItem *)),
          this,            SLOT(scaffoldSelected(Q3ListViewItem *)));

  connect(scaffoldList,    SIGNAL(returnPressed(Q3ListViewItem *)),
          this,            SLOT(scaffoldSelected(Q3ListViewItem *)));

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
  EventTime_t timer;
  if (m_verbose) { cerr << "Loading Scaffolds..."; }

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
  scaffoldList->addColumn("Bases/Offset");
  scaffoldList->addColumn("Contigs/Orientation");

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
      int bases = 0;

      vector<AMOS::Tile_t> & ctiling = scaffold.getContigTiling();

      vector<AMOS::Tile_t>::iterator ti; 	 
      for (ti =  ctiling.begin();
           ti != ctiling.end();
           ti++) 	 
      { 	 
        bases += ti->range.getLength();
      }

      ScaffoldListItem * scaffolditem;

      scaffolditem = new ScaffoldListItem(scaffoldList,  
                                          QString::number(scaffid), 
                                          QString::number(scaffold.getIID()),
                                          QString(scaffold.getEID().c_str()), 
                                          QString::number(span), 
                                          QString::number(bases), 
                                          QString::number(numcontigs));

      scaffid++;

      for (ti =  ctiling.begin();
           ti != ctiling.end();
           ti++) 	 
      { 	 
        QString oo = ti->range.isReverse() ? "EB" : "BE";
        new ScaffoldListItem(scaffolditem, 	 
                             QString::number(m_datastore->contig_bank.getIDMap().lookupBID(ti->source)),
                             QString::number(ti->source), 	 
                             QString(m_datastore->contig_bank.lookupEID(ti->source).c_str()),
                             QString::number(ti->range.getLength()),
                             QString::number(ti->offset),
                             oo);
      }
    }
  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  scaffoldList->setSorting(3, false);

  scaffoldList->setColumnWidth(2, (int)(scaffoldList->columnWidth(2)*1.2));

  setCursor(orig);
  if (m_verbose) { cerr << "     " << timer.str() << endl; }
}

void LaunchPad::scaffoldSelected(Q3ListViewItem * item)
{
  if (item->parent())
  {
    setContigId(atoi(item->text(0)));
  }
  else
  {
    setScaffoldId(atoi(item->text(0)));
  }
}

void LaunchPad::scaffoldSelectIID(const QString & iid)
{
  Q3ListViewItem * item = scaffoldList->findItem(iid, 1);
  if (item)
  {
    scaffoldList->setSelected(item, true);
    scaffoldList->ensureItemVisible(item);
  }
}

void LaunchPad::scaffoldSelectEID(const QString & eid)
{
  Q3ListViewItem * item = scaffoldList->findItem(eid, 2);
  if (item)
  {
    scaffoldList->setSelected(item, true);
    scaffoldList->ensureItemVisible(item);
  }
}

void LaunchPad::scaffoldViewSelected()
{
  Q3ListViewItem * item = scaffoldList->selectedItem();
  if (item)
  {
    scaffoldSelected(item);
  }
}


void LaunchPad::scaffoldSpanHistogram()
{
  NChartStats * stats = new NChartStats(*m_scaffstats);
  new NChartWindow(stats, this, "hist");
}

void LaunchPad::scaffoldContigHistogram()
{
  InsertStats * stats = new InsertStats((string)"Scaffold Contig Count Histogram");

  AMOS::Scaffold_t scaffold;
  m_datastore->scaffold_bank.seekg(1);
  while (m_datastore->scaffold_bank >> scaffold)
  {
    stats->addSize(scaffold.getContigTiling().size());
  }

  new HistogramWindow(stats, this, "hist");
}

