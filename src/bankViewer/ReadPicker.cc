#include "ReadPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qlineedit.h>

#include "foundation_AMOS.hh"

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
                 QString mean)
               
    : QListViewItem(parent, iid, eid, type, matetype, offset, endoffset, len, dir) 
    {
      setText(8, seql);
      setText(9, seqr);
      setText(10, libid);
      setText(11, mean);
    }


  int compare(QListViewItem *i, int col,
              bool ascending ) const
  {
    if (col == 1 || col == 2 || col==3 || col == 7)
    {
      return key(col,ascending).compare(i->key(col,ascending));
    }
    else
    {
      return atoi(key(col,ascending)) - atoi(i->key(col,ascending));
    }
  }
};


ReadPicker::ReadPicker(DataStore * datastore,
                       QWidget * parent, 
                       const char * name)
  :QMainWindow(parent, name)
{
  m_table = new QListView(this, "readpickertbl");
  setCentralWidget(m_table);
  setCaption("Read Chooser");
  resize(800,500);
  show();

  m_datastore = datastore;

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
  m_table->addColumn("Type");
  m_table->addColumn("MateType");
  m_table->addColumn("Offset");
  m_table->addColumn("End Offset");
  m_table->addColumn("Length");
  m_table->addColumn("Dir");
  m_table->addColumn("CLR Begin");
  m_table->addColumn("CLR End");
  m_table->addColumn("Lib ID");
  m_table->addColumn("Lib Mean");

  m_table->setShowSortIndicator(true);
  m_table->setRootIsDecorated(true);
  m_table->setAllColumnsShowFocus(true);

  loadTable();
}

void ReadPicker::loadTable()
{
  m_table->clear();

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
      new ReadListItem(m_table,
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
                       QString::number(dist.mean));
    }

    setCursor(orig);
  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }
}

void ReadPicker::itemSelected(QListViewItem * item)
{
  emit highlightRead(atoi(item->text(0)));
}

void ReadPicker::selectiid(const QString & iid)
{
  QListViewItem * item = m_table->findItem(iid, 0);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void ReadPicker::selecteid(const QString & eid)
{
  QListViewItem * item = m_table->findItem(eid, 1);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void ReadPicker::acceptSelected()
{
  QListViewItem * item = m_table->selectedItem();
  if (item)
  {
    itemSelected(item);
  }
}

void ReadPicker::contigIdSelected(int contigid)
{
  loadTable();
}


