#include "ScaffoldPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qtoolbar.h>
#include <qlineedit.h>
#include <qmenubar.h>

#include "foundation_AMOS.hh"
#include "RenderSeq.hh"

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
                   QString contigs)
               
    : QListViewItem(parent, id, iid, eid, length, contigs) {}

  ScaffoldListItem(QListView * parent, 
                   QString id,
                   QString iid,
                   QString eid,
                   QString offset,
                   QString length,
                   QString contigs)
               
    : QListViewItem(parent, id, iid, eid, offset, length, contigs) {}

  ScaffoldListItem(ScaffoldListItem * parent, 
                   QString id,
                   QString iid,
                   QString eid,
                   QString offset,
                   QString length,
                   QString contigs)
               
    : QListViewItem(parent, id, iid, eid, offset, length, contigs) {}

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


ScaffoldPicker::ScaffoldPicker(DataStore * datastore,
                               QWidget * parent, 
                               const char * name)
  :QMainWindow(parent, name)
{
  m_showContigs = true;
  m_datastore = datastore;

  m_table = new QListView(this, "scaffpickertbl");
  setCentralWidget(m_table);
  setCaption("Scaffold Information");
  resize(500,500);
  show();

  m_options = new QPopupMenu(this);
  menuBar()->insertItem("&Options", m_options);
  m_showcontigsid  = m_options->insertItem("Show &Contigs", this, SLOT(toggleShowContigs()));
  m_options->setItemChecked(m_showcontigsid, true);

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


  m_table->setShowSortIndicator(true);
  m_table->setRootIsDecorated(true);
  m_table->setAllColumnsShowFocus(true);


  loadTable(true);
}

void ScaffoldPicker::loadTable(bool jumpToCurrent)
{
  m_table->clear();
  int c = m_table->columns();
  for (int i = 0; i < c; i++)
  {
    m_table->removeColumn(0);
  }

  if (m_showContigs)
  {
    m_table->addColumn("Id");
    m_table->addColumn("IID");
    m_table->addColumn("EID");
    m_table->addColumn("Offset");
    m_table->addColumn("Span");
    m_table->addColumn("Contigs");
  }
  else
  {
    m_table->addColumn("Id");
    m_table->addColumn("IID");
    m_table->addColumn("EID");
    m_table->addColumn("Span");
    m_table->addColumn("Contigs");
  }

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

      if (m_showContigs)
      {
        scaffolditem = new ScaffoldListItem(m_table,  
                                        QString::number(scaffid), 
                                        QString::number(contig.getIID()),
                                        QString(contig.getEID().c_str()), 
                                        QString(""),
                                        QString::number(span), 
                                        QString::number(numcontigs));
      }
      else
      {
        scaffolditem = new ScaffoldListItem(m_table,  
                                        QString::number(scaffid), 
                                        QString::number(scaffold.getIID()),
                                        QString(scaffold.getEID().c_str()), 
                                        QString::number(span), 
                                        QString::number(numcontigs));
      }

      scaffid++;

      if (m_showContigs)
      {
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
                             QString(m_datastore->contig_bank.lookupEID(ti->source)),
                             QString::number(ti->offset), 	 
                             QString::number(ti->range.getLength() + ti->gaps.size()),
                             oo);
        }
      }
    }
  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  setCursor(orig);
}

void ScaffoldPicker::itemSelected(QListViewItem * item)
{
  int offset = 0;
  if (item->parent())
  {
    offset = 0;
    emit contigSelected(atoi(item->text(0)));
    emit setGindex(offset);
  }
}

void ScaffoldPicker::selectiid(const QString & iid)
{
  QListViewItem * item = m_table->findItem(iid, 1);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void ScaffoldPicker::selecteid(const QString & eid)
{
  QListViewItem * item = m_table->findItem(eid, 2);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void ScaffoldPicker::acceptSelected()
{
  QListViewItem * item = m_table->selectedItem();
  if (item)
  {
    itemSelected(item);
  }
}

void ScaffoldPicker::toggleShowContigs()
{
  bool b = !m_options->isItemChecked(m_showcontigsid);
  m_options->setItemChecked(m_showcontigsid, b);

  m_showContigs = b;

  loadTable(false);
}

