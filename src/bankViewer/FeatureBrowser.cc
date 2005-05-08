#include "FeatureBrowser.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qlineedit.h>

#include "foundation_AMOS.hh"

#include <vector>

using namespace std;



class FeatureListItem : public QListViewItem
{
public:
  FeatureListItem(QListView * parent, 
                  QString contigid,
                  QString eid,
                  QString type,
                  QString group,
                  QString offset,
                  QString end5,
                  QString end3,
                  QString comment)
               
    : QListViewItem(parent, contigid, eid, type, group, offset, end5, end3, comment)
    {
      
    }


  int compare(QListViewItem *i, int col,
              bool ascending ) const
  {
    if (col == 0 || col == 4 || col == 5 || col == 6)
    {
      return atoi(key(col,ascending)) - atoi(i->key(col,ascending));
    }
    else
    {
      return key(col,ascending).compare(i->key(col,ascending));
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

  m_table->addColumn("Contig ID");
  m_table->addColumn("EID");
  m_table->addColumn("Type");
  m_table->addColumn("Group");
  m_table->addColumn("Offset");
  m_table->addColumn("End5");
  m_table->addColumn("End3");
  m_table->addColumn("Comment");

  m_datastore = datastore;

  m_table->setShowSortIndicator(true);
  m_table->setRootIsDecorated(true);
  m_table->setAllColumnsShowFocus(true);

  QCursor orig = cursor();
  setCursor(Qt::waitCursor);

  try
  {
    int featcount = 0;
    int contigid = 1;
    datastore->contig_bank.seekg(1);
    AMOS::Contig_t contig;

    while (datastore->contig_bank >> contig)
    {
      vector<AMOS::Feature_t>::iterator fi;
      for (fi =  contig.getFeatures().begin();
           fi != contig.getFeatures().end();
           fi++)
      {
        featcount++;
        new FeatureListItem(m_table,
                            QString::number(contigid),
                            QString(fi->getEID()),
                            QString((QChar)(char)fi->type),
                            QString(fi->group),
                            QString::number(fi->range.isReverse() ? fi->range.end : fi->range.begin),
                            QString::number(fi->range.begin),
                            QString::number(fi->range.end),
                            QString(fi->getComment()));
      }
      contigid++;
    }

    QString status = "Select from " ;
    status += QString::number(featcount) + " features";
    statusBar()->message(status);
  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  setCursor(orig);
}

void FeatureBrowser::itemSelected(QListViewItem * item)
{
  int contigid = atoi(item->text(0));
  int gindex = atoi(item->text(4));

  if (contigid != m_datastore->m_contigId)
  {
    emit setContigId(contigid);
  }

  emit setGindex(gindex);
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


