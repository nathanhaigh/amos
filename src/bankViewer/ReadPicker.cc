#include "ReadPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qlineedit.h>

#include "foundation_AMOS.hh"

#include <vector>

using namespace std;



class ContigListItem : public QListViewItem
{
public:
  ContigListItem(QListView * parent, 
                 QString iid,
                 QString eid,
                 QString offset,
                 QString endoffset,
                 QString len,
                 QString dir,
                 QString seql,
                 QString seqr,
                 QString mean)
               
    : QListViewItem(parent, iid, eid, offset, endoffset, len, dir, seql, seqr) 
    {
      setText(8, mean);
      
    }


  int compare(QListViewItem *i, int col,
              bool ascending ) const
  {
    if (col == 1 || col == 5)
    {
      return strcmp(key(col,ascending), i->key(col,ascending));
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
  resize(550,500);
  show();

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
  m_table->addColumn("Offset");
  m_table->addColumn("End Offset");
  m_table->addColumn("Length");
  m_table->addColumn("Dir");
  m_table->addColumn("CLR Begin");
  m_table->addColumn("CLR End");
  m_table->addColumn("Lib Mean");

  m_table->setShowSortIndicator(true);
  m_table->setRootIsDecorated(true);
  m_table->setAllColumnsShowFocus(true);

  try
  {
    QCursor orig = cursor();
    setCursor(Qt::waitCursor);

    QString status = "Select from " ;
    status += QString::number(datastore->m_contig.getReadTiling().size()) 
           + " reads";
    statusBar()->message(status);

    vector<AMOS::Tile_t>::iterator ti;
    for (ti =  datastore->m_contig.getReadTiling().begin();
         ti != datastore->m_contig.getReadTiling().end();
         ti++)
    {
      AMOS::Distribution_t dist = datastore->getLibrarySize(ti->source);

      int len = ti->range.getLength() + ti->gaps.size();
      new ContigListItem(m_table,
                         QString::number(ti->source),
                         datastore->read_bank.lookupEID(ti->source),
                         QString::number(ti->offset),
                         QString::number(ti->offset + len - 1),
                         QString::number(len),
                         ((ti->range.isReverse())?"R":"F"),
                         QString::number(ti->range.begin),
                         QString::number(ti->range.end),
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
  cerr << iid << endl;
  QListViewItem * item = m_table->findItem(iid, 0);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void ReadPicker::selecteid(const QString & eid)
{
  cerr << eid << endl;
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


