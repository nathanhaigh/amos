#include "ChromoPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qtoolbutton.h>

#include "foundation_AMOS.hh"

#include <vector>

using namespace std;



class ChromoListItem : public QListViewItem
{
public:
  ChromoListItem(QListView * parent, 
                 QString type,
                 QString path)
               
    : QListViewItem(parent, type, path) {}
};


ChromoPicker::ChromoPicker(DataStore * datastore,
                       QWidget * parent, 
                       const char * name)
  :QMainWindow(parent, name), m_datastore(datastore)
{
  m_table = new QListView(this, "ChromoPickertbl");
  setCentralWidget(m_table);
  setCaption("Chromatogram Information");
  resize(550,500);
  show();

  QToolBar * tool = new QToolBar(this, "tools");
  new QLabel("Type:", tool, "typelbl");
  m_typepick = new QLineEdit(tool, "typepick");

  new QLabel("  Path:", tool, "pathlbl");
  m_pathpick = new QLineEdit(tool, "pathpick");

  QToolButton* add = new QToolButton(QPixmap(), "Add", "Add new chromatogram location",
                                     this, SLOT(addNew()), tool);
  add->setText("Add");

  m_table->addColumn("Type");
  m_table->addColumn("Path");

  m_table->setShowSortIndicator(true);
  m_table->setRootIsDecorated(true);
  m_table->setAllColumnsShowFocus(true);

  QCursor orig = cursor();

  vector<string>::iterator vi;

  for (vi = m_datastore->m_chromopaths.begin();
       vi != m_datastore->m_chromopaths.end();
       vi++)
  {
    new ChromoListItem(m_table, QString("Path"), QString(vi->c_str()));
  }

  for (vi = m_datastore->m_chromodbs.begin();
       vi != m_datastore->m_chromodbs.end();
       vi++)
  {
    new ChromoListItem(m_table, QString("DB"), QString(vi->c_str()));
  }


  setCursor(orig);
}

void ChromoPicker::itemSelected(QListViewItem * item)
{
  //emit highlightRead(atoi(item->text(0)));
}

void ChromoPicker::selectiid(const QString & iid)
{
  cerr << iid << endl;
  QListViewItem * item = m_table->findItem(iid, 0);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void ChromoPicker::selecteid(const QString & eid)
{
  cerr << eid << endl;
  QListViewItem * item = m_table->findItem(eid, 1);
  if (item)
  {
    m_table->setSelected(item, true);
    m_table->ensureItemVisible(item);
  }
}

void ChromoPicker::acceptSelected()
{
  QListViewItem * item = m_table->selectedItem();
  if (item)
  {
    itemSelected(item);
  }
}

void ChromoPicker::addNew()
{
  QString type = m_typepick->text();
  QString path = m_pathpick->text();

  if (type == QString("Path"))
  {
    m_datastore->m_chromopaths.push_back((std::string)path.ascii());
    new ChromoListItem(m_table, type, path);
  }
  else if (type == QString("DB"))
  {
    m_datastore->m_chromodbs.push_back((std::string)path.ascii());
    new ChromoListItem(m_table, type, path);
  }

}
