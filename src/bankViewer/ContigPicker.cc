#include "ContigPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qtoolbar.h>
#include <qlineedit.h>
#include <qmenubar.h>
#include "DataStore.hh"


#include "foundation_AMOS.hh"

#include <vector>

using namespace std;

int FIELD_OFFSET=4;

class ContigListItem : public QListViewItem
{
public:
  ContigListItem(QListView * parent, 
                 QString id,
                 QString iid,
                 QString eid,
                 QString status,
                 QString length,
                 QString reads)
               
    : QListViewItem(parent, id, iid, eid, status, length, reads) {}

  ContigListItem(QListView * parent, 
                 QString id,
                 QString iid,
                 QString eid,
                 QString status,
                 QString offset,
                 QString length,
                 QString reads)
               
    : QListViewItem(parent, id, iid, eid, status, offset, length, reads) {}

  ContigListItem(ContigListItem * parent, 
                 QString id,
                 QString iid,
                 QString eid,
                 QString status,
                 QString offset,
                 QString length,
                 QString reads)
               
    : QListViewItem(parent, id, iid, eid, status, offset, length, reads) {}

  int compare(QListViewItem *i, int col,
              bool ascending ) const
  {
    if (col == 2 || col == 3)
    {
      return key(col,ascending).compare(i->key(col,ascending));
    }

    return atoi(key(col,ascending)) - atoi(i->key(col,ascending));
  }
};

#include "BufferedLineEdit.hh"


ContigPicker::ContigPicker(DataStore * datastore,
                           QWidget * parent, 
                           const char * name)
  :QMainWindow(parent, name)
{
  m_showReads = false;
  m_datastore = datastore;

  m_table = new QListView(this, "contigpickertbl");
  setCentralWidget(m_table);
  setCaption("Contig Chooser");
  resize(500,500);
  show();

  m_options = new QPopupMenu(this);
  menuBar()->insertItem("&Options", m_options);
  m_showreadsid  = m_options->insertItem("Show &Reads", this, SLOT(toggleShowReads()));

  QToolBar * tool = new QToolBar(this, "tools");
  new QLabel("IID:", tool, "iidlbl");
  BufferedLineEdit * iidpick = new BufferedLineEdit(tool, "iidpick");

  new QLabel("EID:", tool, "eidlbl");
  BufferedLineEdit * eidpick = new BufferedLineEdit(tool, "eidpick");

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


  m_table->setShowSortIndicator(true);
  m_table->setRootIsDecorated(true);
  m_table->setAllColumnsShowFocus(true);


  loadTable(true);
}

void ContigPicker::loadTable(bool jumpToCurrent)
{
  m_table->clear();
  int c = m_table->columns();
  for (int i = 0; i < c; i++)
  {
    m_table->removeColumn(0);
  }

  QCursor orig = cursor();
  setCursor(Qt::waitCursor);

  ContigListItem * curItem = NULL;

  if (m_showReads)
  {
    m_table->addColumn("Id");
    m_table->addColumn("IID");
    m_table->addColumn("EID");
    m_table->addColumn("Status");
    m_table->addColumn("Offset");
    m_table->addColumn("Length");
    m_table->addColumn("Reads");
  }
  else
  {
    m_table->addColumn("Id");
    m_table->addColumn("IID");
    m_table->addColumn("EID");
    m_table->addColumn("Status");
    m_table->addColumn("Length");
    m_table->addColumn("Reads");
  }

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

      ContigListItem * contigitem;

      if (m_showReads)
      {
        contigitem = new ContigListItem(m_table,  
                                        QString::number(contigid), 
                                        QString::number(contig.getIID()),
                                        QString(contig.getEID().c_str()), 
                                        QString(QChar(contig.getStatus())),
                                        QString(""),
                                        QString::number(contiglen), 
                                        QString::number(numreads));
      }
      else
      {
        contigitem = new ContigListItem(m_table,  
                                        QString::number(contigid), 
                                        QString::number(contig.getIID()),
                                        QString(contig.getEID().c_str()), 
                                        QString(QChar(contig.getStatus())),
                                        QString::number(contiglen), 
                                        QString::number(numreads));
      }

      if (contigid == m_datastore->m_contigId)
      {
        curItem = contigitem;
      }

      contigid++;

      if (m_showReads)
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
                             QString("")); 	 
        }
      }
    }
  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  if (jumpToCurrent && curItem)
  {
    m_table->setSelected(curItem, true);
    m_table->ensureItemVisible(curItem);
  }

  setCursor(orig);
}

void ContigPicker::itemSelected(QListViewItem * item)
{
  int offset = 0;
  if (item->parent())
  {
    offset = atoi(item->text(FIELD_OFFSET));
    item = item->parent();
  }

  emit contigSelected(atoi(item->text(0)));
  emit setGindex(offset);
}

void ContigPicker::selectiid(const QString & iid)
{
  QListViewItem * item = m_table->findItem(iid, 1);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void ContigPicker::selecteid(const QString & eid)
{
  QListViewItem * item = m_table->findItem(eid, 2);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void ContigPicker::acceptSelected()
{
  QListViewItem * item = m_table->selectedItem();
  if (item)
  {
    itemSelected(item);
  }
}

void ContigPicker::toggleShowReads()
{
  bool b = !m_options->isItemChecked(m_showreadsid);
  m_options->setItemChecked(m_showreadsid, b);

  m_showReads = b;

  loadTable(false);
}


void ContigPicker::refreshTable()
{
  loadTable(false);
}
