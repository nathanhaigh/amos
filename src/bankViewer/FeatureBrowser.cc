#include "FeatureBrowser.hh"
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
                 QString eid,
                 QString type,
                 QString group,
                 QString end5,
                 QString end3,
                 QString comment)
               
    : QListViewItem(parent, eid, type, group, end5, end3, comment)
    {
      
    }


  int compare(QListViewItem *i, int col,
              bool ascending ) const
  {
    if (col == 3 || col == 4)
    {
      return atoi(key(col,ascending)) - atoi(i->key(col,ascending));
    }
    else
    {
      return strcmp(key(col,ascending), i->key(col,ascending));
    }
  }
};


FeatureBrowser::FeatureBrowser(DataStore * datastore,
                       QWidget * parent, 
                       const char * name)
  :QMainWindow(parent, name)
{
  m_table = new QListView(this, "FeatureBrowsertbl");
  setCentralWidget(m_table);
  setCaption("Feature Browser");
  resize(800,500);
  show();

  QToolBar * tool = new QToolBar(this, "tools");

  new QLabel("EID:", tool, "eidlbl");
  QLineEdit * eidpick = new QLineEdit(tool, "eidpick");

  connect(m_table, SIGNAL(doubleClicked(QListViewItem *)),
          this,  SLOT(itemSelected(QListViewItem *)));

  connect(m_table, SIGNAL(returnPressed(QListViewItem *)),
          this,  SLOT(itemSelected(QListViewItem *)));

  connect(eidpick, SIGNAL(textChanged(const QString &)),
          this,    SLOT(selecteid(const QString &)));

  connect(eidpick, SIGNAL(returnPressed()),
          this,    SLOT(acceptSelected()));

  m_table->addColumn("EID");
  m_table->addColumn("Type");
  m_table->addColumn("Group");
  m_table->addColumn("End5");
  m_table->addColumn("End3");
  m_table->addColumn("Comment");

  m_table->setShowSortIndicator(true);
  m_table->setRootIsDecorated(true);
  m_table->setAllColumnsShowFocus(true);

  try
  {
    QCursor orig = cursor();
    setCursor(Qt::waitCursor);

    QString status = "Select from " ;
    status += QString::number(datastore->m_contig.getFeatures().size()) 
           + " features";
    statusBar()->message(status);

    vector<AMOS::Feature_t>::iterator ti;
    for (ti =  datastore->m_contig.getFeatures().begin();
         ti != datastore->m_contig.getFeatures().end();
         ti++)
    {
      new ContigListItem(m_table,
                         QString(ti->eid),
                         QString((QChar)(char)ti->type),
                         QString(ti->group),
                         QString::number(ti->range.begin),
                         QString::number(ti->range.end),
                         QString(ti->comment));
    }

    setCursor(orig);
  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }
}

void FeatureBrowser::itemSelected(QListViewItem * item)
{
  emit setGindex(atoi(item->text(3)));
}

void FeatureBrowser::selecteid(const QString & eid)
{
  cerr << eid << endl;
  QListViewItem * item = m_table->findItem(eid, 1);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void FeatureBrowser::acceptSelected()
{
  QListViewItem * item = m_table->selectedItem();
  if (item)
  {
    itemSelected(item);
  }
}


