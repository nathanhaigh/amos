#include "LibraryPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qlineedit.h>

#include "foundation_AMOS.hh"

#include <vector>

using namespace std;



class LibListItem : public QListViewItem
{
public:
  LibListItem(QListView * parent, 
              QString iid,
              QString eid,
              QString mean,
              QString sd)
               
    : QListViewItem(parent, iid, eid, mean, sd) { }


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


LibraryPicker::LibraryPicker(DataStore * datastore,
                       QWidget * parent, 
                       const char * name)
  :QMainWindow(parent, name)
{
  m_table = new QListView(this, "LibraryPickertbl");
  setCentralWidget(m_table);
  setCaption("Library Information");
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
  m_table->addColumn("Mean");
  m_table->addColumn("Stdev");

  m_table->setShowSortIndicator(true);
  m_table->setRootIsDecorated(true);
  m_table->setAllColumnsShowFocus(true);

  m_datastore = datastore;

  loadTable();
}

void LibraryPicker::loadTable()
{
  m_table->clear();
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

        new LibListItem(m_table,
                        QString::number(lib.getIID()),
                        lib.getEID().c_str(),
                        QString::number(dist.mean),
                        QString::number(dist.sd));
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

void LibraryPicker::itemSelected(QListViewItem * item)
{
  //emit highlightRead(atoi(item->text(0)));
}

void LibraryPicker::selectiid(const QString & iid)
{
  cerr << iid << endl;
  QListViewItem * item = m_table->findItem(iid, 0);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void LibraryPicker::selecteid(const QString & eid)
{
  cerr << eid << endl;
  QListViewItem * item = m_table->findItem(eid, 1);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void LibraryPicker::acceptSelected()
{
  QListViewItem * item = m_table->selectedItem();
  if (item)
  {
    itemSelected(item);
  }
}


void LibraryPicker::refreshTable()
{
  loadTable();
}
