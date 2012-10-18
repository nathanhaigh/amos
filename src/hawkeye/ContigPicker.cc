#include "LaunchPad.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <q3toolbar.h>
#include <qlineedit.h>
#include <qmenubar.h>
#include <qcheckbox.h>
#include "DataStore.hh"

#include "NChartStats.hh"
#include "HistogramWindow.hh"
#include "NChartWindow.hh"

#include "foundation_AMOS.hh"

#include <vector>
#include "amp.hh"

using namespace std;
using namespace AMOS;

int FIELD_OFFSET=4;

class ContigListItem : public Q3ListViewItem
{
public:
  ContigListItem(Q3ListView * parent, 
                 QString id,
                 QString iid,
                 QString eid,
                 QString status,
                 QString length,
                 QString reads,
                 QString coverage,
                 QString gccontent)
               
    : Q3ListViewItem(parent, id, iid, eid, status, length, reads, coverage, gccontent) {}

  ContigListItem(Q3ListView * parent, 
                 QString id,
                 QString iid,
                 QString eid,
                 QString status,
                 QString offset,
                 QString length,
                 QString reads,
                 QString coverage,
                 QString gccontent)
               
    : Q3ListViewItem(parent, id, iid, eid, status, offset, length, reads, coverage) 
  { setText(8, gccontent); }

  ContigListItem(ContigListItem * parent, 
                 QString id,
                 QString iid,
                 QString eid,
                 QString status,
                 QString offset,
                 QString length,
                 QString reads,
                 QString coverage,
                 QString gccontent)
               
    : Q3ListViewItem(parent, id, iid, eid, status, offset, length, reads, coverage)
  { setText(8, gccontent); }

  int compare(Q3ListViewItem *i, int col,
              bool ascending ) const
  {
    if (col == 2 || col == 3)
    {
      return key(col,ascending).compare(i->key(col,ascending));
    }

    if ((col == 7) || (col == 8))
    {
      double diff = atof(key(col,ascending)) - atof(i->key(col,ascending));
      if      (diff < 0) { return -1; }
      else if (diff > 0) { return 1;}
      return 0;
    }

    return atoi(key(col,ascending)) - atoi(i->key(col,ascending));
  }
};



void LaunchPad::initContigs()
{
  connect(contigList,    SIGNAL(doubleClicked(Q3ListViewItem *)),
          this,          SLOT(contigSelected(Q3ListViewItem *)));

  connect(contigList,    SIGNAL(returnPressed(Q3ListViewItem *)),
          this,          SLOT(contigSelected(Q3ListViewItem *)));

  connect(contigIIDEdit, SIGNAL(textChanged(const QString &)),
          this,          SLOT(contigSelectIID(const QString &)));

  connect(contigEIDEdit, SIGNAL(textChanged(const QString &)),
          this,          SLOT(contigSelectEID(const QString &)));

  connect(contigEIDEdit, SIGNAL(returnPressed()),
          this,          SLOT(contigViewSelected()));

  connect(contigIIDEdit, SIGNAL(returnPressed()),
          this,          SLOT(contigViewSelected()));

  connect(contigLengthButton, SIGNAL(clicked()),
          this,               SLOT(contigLengthHistogram()));

  connect(contigReadsButton,  SIGNAL(clicked()),
          this,               SLOT(contigReadCountHistogram()));

  connect(contigCoverageButton, SIGNAL(clicked()),
          this,                 SLOT(contigCoverageHistogram()));

  connect(contigGCButton,     SIGNAL(clicked()),
          this,               SLOT(contigGCHistogram()));

  connect(readsCheck,         SIGNAL(clicked()),
          this,               SLOT(loadContigs()));

  contigList->addColumn("Id");
  contigList->addColumn("IID");
  contigList->addColumn("EID");
  contigList->addColumn("Status");
  contigList->addColumn("Offset");
  contigList->addColumn("Length");
  contigList->addColumn("Reads");
  contigList->addColumn("Coverage");
  contigList->addColumn("GC Content");
}


void LaunchPad::loadContigs()
{
  EventTime_t timer;
  if (m_verbose) { cerr << "Loading Contigs..."; }

  QCursor orig = cursor();
  setCursor(Qt::waitCursor);

  contigList->clear();

  ContigListItem * curItem = NULL;

  try
  {
    AMOS::Contig_t contig;
    QString status = "Select from " + 
                     QString::number(m_datastore->contig_bank.getSize()) + 
                     " contigs in " + m_datastore->m_bankname.c_str();
    statusBar()->message(status);

    int contigid = 1;
    m_datastore->contig_bank.seekg(1);
    while (m_datastore->contig_bank >> contig)
    {
      int contiglen = contig.getLength();
      int numreads = contig.getReadTiling().size();
      double coverage = contig.getFoldCov();
      double gccontent = contig.getGCContent();

      ContigListItem * contigitem;

      contigitem = new ContigListItem(contigList,  
                                      QString::number(contigid), 
                                      QString::number(contig.getIID()),
                                      QString(contig.getEID().c_str()), 
                                      QString(QChar(contig.getStatus())),
                                      QString(""),
                                      QString::number(contiglen), 
                                      QString::number(numreads),
                                      QString::number(coverage, 'f', 2),
                                      QString::number(gccontent, 'f', 4));

      if (contigid == m_datastore->m_contigId)
      {
        curItem = contigitem;
      }

      contigid++;

      if (readsCheck->isChecked())
      {
        vector<AMOS::Tile_t>::iterator ti; 	 
        for (ti =  contig.getReadTiling().begin(); 	 
             ti != contig.getReadTiling().end(); 	 
             ti++) 	 
        { 	 
          new ContigListItem(contigitem, 	 
                             QString("Read"),
                             QString::number(ti->source), 	 
                             QString(m_datastore->read_bank.lookupEID(ti->source).c_str()),
                             QString(""),
                             QString::number(ti->offset), 	 
                             QString::number(ti->range.getLength() + ti->gaps.size()),
                             QString(""),
                             QString(""),
                             QString("")); 	 
        }
      }
    }

    contigList->setSorting(5, false);
  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  contigList->setColumnWidth(2, (int)(contigList->columnWidth(2)*1.2));

  setCursor(orig);
  if (m_verbose) { cerr << "       " << timer.str() << endl; }
}

void LaunchPad::contigSelected(Q3ListViewItem * item)
{
  int offset = 0;
  if (item->parent())
  {
    offset = atoi(item->text(FIELD_OFFSET));
    item = item->parent();
  }

  setContigId(atoi(item->text(0)));
  setGindex(offset);
}

void LaunchPad::contigSelectIID(const QString & iid)
{
  Q3ListViewItem * item = contigList->findItem(iid, 1);
  if (item)
  {
    contigList->setSelected(item, true);
    contigList->ensureItemVisible(item);
  }
}

void LaunchPad::contigSelectEID(const QString & eid)
{
  Q3ListViewItem * item = contigList->findItem(eid, 2);
  if (item)
  {
    contigList->setSelected(item, true);
    contigList->ensureItemVisible(item);
  }
}

void LaunchPad::contigViewSelected()
{
  Q3ListViewItem * item = contigList->selectedItem();
  if (item)
  {
    contigSelected(item);
  }
}

void LaunchPad::contigLengthHistogram()
{
  NChartStats * stats = new NChartStats(*m_contigstats);
  new NChartWindow(stats, this, "hist");
}

void LaunchPad::contigReadCountHistogram()
{
  InsertStats * stats = new InsertStats((string)"Contig Read Count Histogram");

  AMOS::Contig_t contig;
  m_datastore->contig_bank.seekg(1);
  while (m_datastore->contig_bank >> contig)
  {
    stats->addSize(contig.getReadTiling().size());
  }

  new HistogramWindow(stats, this, "hist");
}

void LaunchPad::contigCoverageHistogram()
{
  InsertStats * stats = new InsertStats((string)"Contig Coverage Histogram");

  AMOS::Contig_t contig;
  m_datastore->contig_bank.seekg(1);
  while (m_datastore->contig_bank >> contig)
  {
    stats->addSize(contig.getFoldCov());
  }

  new HistogramWindow(stats, this, "hist");
}

void LaunchPad::contigGCHistogram()
{
  InsertStats * stats = new InsertStats((string)"Contig GC Content Histogram");

  AMOS::Contig_t contig;
  m_datastore->contig_bank.seekg(1);
  while (m_datastore->contig_bank >> contig)
  {
    stats->addSize(contig.getGCContent());
  }

  new HistogramWindow(stats, this, "hist");
}

