#include "ReadPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

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
                 QString dir)
               
    : QListViewItem(parent, iid, eid, offset, endoffset, len, dir) {}


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
  QListView * table = new QListView(this, "readpickertbl");
  table->resize(500,500);
  setCentralWidget(table);
  resize(500,500);
  show();

  connect(table, SIGNAL(doubleClicked(QListViewItem *)),
          this,  SLOT(itemSelected(QListViewItem *)));

  table->addColumn("IID");
  table->addColumn("EID");
  table->addColumn("Offset");
  table->addColumn("End Offset");
  table->addColumn("Length");
  table->addColumn("Dir");
  table->setShowSortIndicator(true);
  table->setRootIsDecorated(true);

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
      int len = ti->range.getLength() + ti->gaps.size();
      new ContigListItem(table,
                         QString::number(ti->source),
                         datastore->read_bank.lookupEID(ti->source),
                         QString::number(ti->offset),
                         QString::number(ti->offset + len - 1),
                         QString::number(len),
                         ((ti->range.isReverse())?"R":"F"));
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

