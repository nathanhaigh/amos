#include "FeatureBrowser.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qlineedit.h>
#include "DataStore.hh"

#include "foundation_AMOS.hh"

#include <vector>

using namespace std;
using namespace AMOS;



class FeatureListItem : public QListViewItem
{
public:
  FeatureListItem(QListView * parent, 
                  QString ncode,
                  QString siid,
                  QString type,
                  QString dir,
                  QString start,
                  QString end,
                  QString length,
                  QString comment)
               
    : QListViewItem(parent, ncode, siid, type, dir, start, end, length, comment)
    {
    }

  FeatureListItem(FeatureListItem * parent, 
                  QString ncode,
                  QString siid,
                  QString type,
                  QString dir,
                  QString start,
                  QString end,
                  QString length,
                  QString comment)
               
    : QListViewItem(parent, ncode, siid, type, dir, start, end, length, comment)
    {
    }


  int compare(QListViewItem *i, int col,
              bool ascending ) const
  {
    if (col == 2 || col == 3 || col == 5 || col == 6 || col == 7)
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



  connect(m_table, SIGNAL(doubleClicked(QListViewItem *)),
          this,  SLOT(itemSelected(QListViewItem *)));

  connect(m_table, SIGNAL(returnPressed(QListViewItem *)),
          this,  SLOT(itemSelected(QListViewItem *)));

#if 0
  QToolBar * tool = new QToolBar(this, "tools");
  new QLabel("EID:", tool, "eidlbl");
  QLineEdit * eidpick = new QLineEdit(tool, "eidpick");
  connect(eidpick, SIGNAL(textChanged(const QString &)),
          this,    SLOT(selecteid(const QString &)));

  connect(eidpick, SIGNAL(returnPressed()),
          this,    SLOT(acceptSelected()));
#endif

  m_table->addColumn("Source Type");
  m_table->addColumn("Source IID");
  m_table->addColumn("Type");
  m_table->addColumn("Dir");
  m_table->addColumn("Start");
  m_table->addColumn("End");
  m_table->addColumn("Length");
  m_table->addColumn("Comment");

  m_datastore = datastore;

  m_table->setShowSortIndicator(true);
  m_table->setRootIsDecorated(true);
  m_table->setAllColumnsShowFocus(true);

  loadTable();
}

void FeatureBrowser::loadTable()
{
  QCursor orig = cursor();
  setCursor(Qt::waitCursor);

  try
  {
    QString status = "Select from " ;
    status += QString::number(m_datastore->feat_bank.getSize()) + " features";
    statusBar()->message(status);

    Feature_t feat;
    m_datastore->feat_bank.seekg(1);

    map<string, FeatureListItem *> objectFeatureMap;
    map<string, FeatureListItem *>::iterator ofmi;

    char buffer[128];

    while (m_datastore->feat_bank >> feat)
    {
      sprintf(buffer, "%c:%d", (char)feat.getSource().second, feat.getSource().first);
      string s;
      s += buffer;

      ofmi = objectFeatureMap.find(s);
      FeatureListItem * base;

      if (ofmi == objectFeatureMap.end())
      {
        base = new FeatureListItem(m_table,
                                   QString((QChar)(char)feat.getSource().second),
                                   QString::number(feat.getSource().first),
                                   QString(""),
                                   QString(""),
                                   QString(""),
                                   QString(""),
                                   QString(""),
                                   QString(""));

        ofmi = objectFeatureMap.insert(make_pair(s, base)).first;
      }
      else
      {
        base = ofmi->second;
      }

      AMOS::Range_t range = feat.getRange();

      new FeatureListItem(base,
                          QString((QChar)(char)feat.getSource().second),
                          QString::number(feat.getSource().first),
                          QString((QChar)(char)feat.getType()),
                          QString((QChar)(range.isReverse()?'R':'F')),
                          QString::number(range.getLo()),
                          QString::number(range.getHi()),
                          QString::number(range.getLength()),
                          QString(feat.getComment().c_str()));

      base->setText(2, QString::number(base->childCount()));
    }

  }
  catch (AMOS::Exception_t & e)
  {
    statusBar()->message("Features not available");
  }

  setCursor(orig);
}

void FeatureBrowser::itemSelected(QListViewItem * item)
{
  int offset = atoi(item->text(4));
  ID_t iid = atoi(item->text(1));

  if (item->text(0)[0] == Contig_t::NCODE)
  {
    ID_t bid = m_datastore->contig_bank.lookupBID(iid);

    if (bid != m_datastore->m_contigId)
    {
      emit setContigId(bid);
    }

    emit setGindex(offset);
  }
  else if (item->text(0)[0] == Scaffold_t::NCODE)
  {
    AMOS::Scaffold_t scaffold;

    ID_t bid = m_datastore->scaffold_bank.lookupBID(iid);
    m_datastore->fetchScaffold(bid, scaffold);

    bid = 0;

    vector<Tile_t>::iterator ti;
    for (ti = scaffold.getContigTiling().begin();
         ti != scaffold.getContigTiling().end();
         ti++)
    {
      if ((ti->offset <= offset) && (offset <= ti->getRightOffset()))
      {
        bid = m_datastore->contig_bank.getIDMap().lookupBID(ti->source);
        offset -= ti->offset;

        if (ti->range.isReverse())
        {
          offset = ti->getGappedLength() - offset;
        }

        break;
      }
    }

    if (bid)
    {
      emit setContigId(bid);
      emit setGindex(offset);
    }
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

void FeatureBrowser::refreshTable()
{
  loadTable();
}
