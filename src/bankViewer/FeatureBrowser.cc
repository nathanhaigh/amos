#include "FeatureBrowser.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qlineedit.h>

#include "foundation_AMOS.hh"

#include <vector>

using namespace std;
using namespace AMOS;



class FeatureListItem : public QListViewItem
{
public:
  FeatureListItem(QListView * parent, 
                  QString eid,
                  QString type,
                  QString ncode,
                  QString siid,
                  QString dir,
                  QString start,
                  QString end,
                  QString comment)
               
    : QListViewItem(parent, eid, type, ncode, siid, dir, start, end, comment)
    {
      
    }


  int compare(QListViewItem *i, int col,
              bool ascending ) const
  {
    if (col == 2 || col == 3 || col == 5 || col == 6)
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

  m_table->addColumn("EID");
  m_table->addColumn("Type");
  m_table->addColumn("Source Type");
  m_table->addColumn("Source IID");
  m_table->addColumn("Dir");
  m_table->addColumn("Start");
  m_table->addColumn("End");
  m_table->addColumn("Comment");

  m_datastore = datastore;

  m_table->setShowSortIndicator(true);
  m_table->setRootIsDecorated(true);
  m_table->setAllColumnsShowFocus(true);

  QCursor orig = cursor();
  setCursor(Qt::waitCursor);

  try
  {
    QString status = "Select from " ;
    status += QString::number(datastore->feat_bank.getSize()) + " features";
    statusBar()->message(status);

    Feature_t feat;
    datastore->feat_bank.seekg(1);

    while (datastore->feat_bank >> feat)
    {
      AMOS::Range_t range = feat.getRange();

      new FeatureListItem(m_table,
                          QString(feat.getEID().c_str()),
                          QString((QChar)(char)feat.getType()),
                          QString::number(feat.getSource().second),
                          QString::number(feat.getSource().first),
                          QString((QChar)(range.isReverse()?'R':'F')),
                          QString::number(range.getLo()),
                          QString::number(range.getHi()),
                          QString(feat.getComment().c_str()));
    }

  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  setCursor(orig);
}

void FeatureBrowser::itemSelected(QListViewItem * item)
{
  if (atoi(item->text(2)) == Contig_t::NCODE)
  {
    ID_t iid = atoi(item->text(3));
    ID_t bid = m_datastore->contig_bank.lookupBID(iid);

    int offset = atoi(item->text(5));

    if (bid != m_datastore->m_contigId)
    {
      emit setContigId(bid);
    }

    emit setGindex(offset);
  }
}

void FeatureBrowser::selecteid(const QString & eid)
{
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


