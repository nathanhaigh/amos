#include "ChromoPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <qpopupmenu.h>

#include "foundation_AMOS.hh"
#include "DataStore.hh"
#include "ChromoStore.hh"


#include <vector>

using namespace std;



class ChromoListItem : public QListViewItem
{
public:
  ChromoListItem(QListView * parent, 
                 QString path)
               
    : QListViewItem(parent, path) {}
};


ChromoPicker::ChromoPicker(QWidget * parent, const char * name)
  :QMainWindow(parent, name)
{
  m_table = new QListView(this, "ChromoPickertbl");
  setCentralWidget(m_table);
  setCaption("Chromatogram Information");
  resize(550,500);
  show();

  ChromoStore * chromostore = ChromoStore::Instance();

  QToolBar * fetchtool = new QToolBar(this, "cmdtools");
  new QLabel("Trace AutoFetch", fetchtool, "fetchlabel");
  m_enabled = new QCheckBox("Enable", fetchtool);
  m_enabled->setChecked(chromostore->m_tracecmdenabled);

  connect(m_enabled, SIGNAL(toggled(bool)),
          this, SLOT(setTraceCmdEnabled(bool)));
          

  QToolBar * cmdtool = new QToolBar(this, "cmdtools");
  new QLabel("Command:", cmdtool, "fetchlabel");
  m_fetchpick = new QLineEdit(cmdtool, "fetchpick");
  m_fetchpick->setText(chromostore->m_tracecmd);
  m_fetchpick->setMinimumWidth(800);
  m_fetchpick->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding));

  QToolBar * restool = new QToolBar(this, "restools");
  new QLabel("Result:", restool, "fetchresultlabel");
  m_fetchresultpick = new QLineEdit(restool, "fetchresultpick");
  m_fetchresultpick->setText(chromostore->m_tracecmdpath);
  m_fetchresultpick->setMinimumWidth(500);


  QToolBar * cachetool = new QToolBar(this, "cachetools");
  new QLabel("Cache:", cachetool, "fetchcachelbl");
  m_fetchcachepick = new QLineEdit(cachetool, "fetchresultpick");
  m_fetchcachepick->setText(chromostore->m_tracecache);
  m_fetchcachepick->setMinimumWidth(800);


  QToolBar * tool = new QToolBar(this, "dbtools");
  new QLabel("Add Trace Directory:", tool, "pathlbl");
  m_pathpick = new QLineEdit(tool, "pathpick");
  m_pathpick->setMinimumWidth(500);

  QToolButton* add = new QToolButton(QPixmap(), "Add", "Add new chromatogram location",
                                     this, SLOT(addNew()), tool);
  add->setText("Add");



  m_table->addColumn("Directory");

  m_table->setShowSortIndicator(true);
  m_table->setRootIsDecorated(true);
  m_table->setAllColumnsShowFocus(true);

  QCursor orig = cursor();

  vector<string>::iterator vi;

  for (vi =  chromostore->m_tracepaths.begin();
       vi != chromostore->m_tracepaths.end();
       vi++)
  {
    new ChromoListItem(m_table, QString(vi->c_str()));
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
  if (m_pathpick->text() != "")
  {
    ChromoStore * chromostore = ChromoStore::Instance();
    chromostore->m_tracepaths.push_back((std::string)m_pathpick->text().ascii());
    new ChromoListItem(m_table, m_pathpick->text());
    m_pathpick->setText("");
  }
}

void ChromoPicker::setTraceCmdEnabled(bool enabled)
{
  m_enabled->setChecked(enabled);
  ChromoStore::Instance()->m_tracecmdenabled = enabled;
}
