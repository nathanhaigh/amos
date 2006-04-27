#include "LaunchPad.hh"
#include <qcursor.h>
#include <qstatusbar.h>
#include <qmenubar.h>

#include <qlabel.h>
#include <qlineedit.h>

#include "foundation_AMOS.hh"
#include "HistogramWindow.hh"


#include <vector>

using namespace std;



class ReadListItem : public QListViewItem
{
public:
  ReadListItem(QListView * parent, 
                 QString iid,
                 QString eid,
                 QString type,
                 QString matetype,
                 QString offset,
                 QString endoffset,
                 QString len,
                 QString dir,
                 QString seql,
                 QString seqr,
                 QString libid,
                 QString gccontent)
               
    : QListViewItem(parent, iid, eid, type, matetype, offset, endoffset, len, dir) 
    {
      setText(8, seql);
      setText(9, seqr);
      setText(10, libid);
      setText(11, gccontent);
    }


  int compare(QListViewItem *i, int col,
              bool ascending ) const
  {
    if (col == 11)
    {
      double diff = atof(key(col,ascending)) - atof(i->key(col,ascending));
      if      (diff < 0) { return -1; }
      else if (diff > 0) { return 1;}
      return 0;
    }
    else if (col == 1 || col == 2 || col==3 || col == 7)
    {
      return key(col,ascending).compare(i->key(col,ascending));
    }

    return atoi(key(col,ascending)) - atoi(i->key(col,ascending));
  }
};


void LaunchPad::initReads()
{
  connect(readList, SIGNAL(doubleClicked(QListViewItem *)),
          this,  SLOT(readSelected(QListViewItem *)));

  connect(readList, SIGNAL(returnPressed(QListViewItem *)),
          this,  SLOT(readSelected(QListViewItem *)));

  connect(readIIDEdit, SIGNAL(textChanged(const QString &)),
          this,        SLOT(readSelectIID(const QString &)));

  connect(readEIDEdit, SIGNAL(textChanged(const QString &)),
          this,        SLOT(readSelectEID(const QString &)));

  connect(readEIDEdit, SIGNAL(returnPressed()),
          this,        SLOT(readViewSelected()));

  connect(readIIDEdit, SIGNAL(returnPressed()),
          this,        SLOT(readViewSelected()));

  connect(readLengthButton, SIGNAL(clicked()),
          this,             SLOT(readLengthHistogram()));

  connect(readGCButton,     SIGNAL(clicked()),
          this,             SLOT(readGCHistogram()));



  readList->addColumn("IID");
  readList->addColumn("EID");
  readList->addColumn("Type");
  readList->addColumn("MateType");
  readList->addColumn("Offset");
  readList->addColumn("End Offset");
  readList->addColumn("Length");
  readList->addColumn("Dir");
  readList->addColumn("CLR Begin");
  readList->addColumn("CLR End");
  readList->addColumn("Lib ID");
  readList->addColumn("GC Content");
}

void LaunchPad::loadReads()
{
  readList->clear();

  try
  {
    QCursor orig = cursor();
    setCursor(Qt::waitCursor);

    QString status = "Select from " ;
    status += QString::number(m_datastore->m_contig.getReadTiling().size()) 
           + " reads";
    statusBar()->message(status);

    vector<AMOS::Tile_t>::iterator ti;
    for (ti =  m_datastore->m_contig.getReadTiling().begin();
         ti != m_datastore->m_contig.getReadTiling().end();
         ti++)
    {
      AMOS::ID_t libid = m_datastore->getLibrary(ti->source);
      AMOS::Distribution_t dist = m_datastore->getLibrarySize(libid);

      AMOS::Read_t red;
      m_datastore->fetchRead(ti->source, red);

      double gccontent = red.getGCContent(red.getClearRange());

      char type = red.getType();
      if (type == 0) { type = '?'; }

      DataStore::MateLookupMap::iterator mi = m_datastore->m_readmatelookup.find(ti->source);
      char mateType = '?';

      if (mi != m_datastore->m_readmatelookup.end())
      {
        mateType = mi->second.second;
        if (mateType == 0) { mateType = '?'; }
      }


      int len = ti->range.getLength() + ti->gaps.size();
      new ReadListItem(readList,
                       QString::number(ti->source),
                       red.getEID().c_str(),
                       QString(QChar(type)),
                       QString(QChar(mateType)),
                       QString::number(ti->offset),
                       QString::number(ti->offset + len - 1),
                       QString::number(len),
                       ((ti->range.isReverse())?"R":"F"),
                       QString::number(ti->range.begin),
                       QString::number(ti->range.end),
                       QString::number(libid),
                       QString::number(gccontent, 'f', 4));
    }

    setCursor(orig);
  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }
}

void LaunchPad::readSelected(QListViewItem * item)
{
  emit highlightRead(atoi(item->text(0)));
  setGindex(atoi(item->text(4)));
}

void LaunchPad::readSelectIID(const QString & iid)
{
  QListViewItem * item = readList->findItem(iid, 0);
  if (item)
  {
    readList->setSelected(item, true);
    readList->ensureItemVisible(item);
  }
}

void LaunchPad::readSelectEID(const QString & eid)
{
  QListViewItem * item = readList->findItem(eid, 1);
  if (item)
  {
    readList->setSelected(item, true);
    readList->ensureItemVisible(item);
  }
}

void LaunchPad::readViewSelected()
{
  QListViewItem * item = readList->selectedItem();
  if (item)
  {
    readSelected(item);
  }
}


void LaunchPad::readLengthHistogram()
{
  char buffer[16];
  sprintf(buffer, "%d", m_datastore->m_contigId);

  InsertStats * stats = new InsertStats((string)"Clear Range Length Histogram for Contig " + buffer);

  vector<AMOS::Tile_t>::iterator ti;
  for (ti =  m_datastore->m_contig.getReadTiling().begin();
       ti != m_datastore->m_contig.getReadTiling().end();
       ti++)
  {
    stats->addSize(ti->range.getLength());
  }

  new HistogramWindow(stats, this, "hist");
}

void LaunchPad::readGCHistogram()
{
  char buffer[16];
  sprintf(buffer, "%d", m_datastore->m_contigId);

  InsertStats * stats = new InsertStats((string)"Read GC Content Histogram for Contig " + buffer);

  vector<AMOS::Tile_t>::iterator ti;
  for (ti =  m_datastore->m_contig.getReadTiling().begin();
       ti != m_datastore->m_contig.getReadTiling().end();
       ti++)
  {
    AMOS::Read_t red;
    m_datastore->fetchRead(ti->source, red);

    stats->addSize(red.getGCContent(red.getClearRange()));
  }

  new HistogramWindow(stats, this, "hist");
}

