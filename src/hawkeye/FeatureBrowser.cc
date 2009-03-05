#include "FeatureBrowser.hh"
#include <qcursor.h>
#include <qstatusbar.h>
#include <q3listview.h>

#include <qlabel.h>
#include <qlineedit.h>
#include "DataStore.hh"
#include "UIElements.hh"

#include "foundation_AMOS.hh"

#include <vector>
#include "amp.hh"

using namespace std;
using namespace AMOS;



class FeatureListItem : public Q3ListViewItem
{
public:

  FeatureListItem(Q3ListView * parent, 
                  QString ncode,
                  QString siid,
                  QString count,
                  QString type,
                  QString dir,
                  QString start,
                  QString end,
                  QString length,
                  QString comment)
               
    : Q3ListViewItem(parent, ncode, siid, count, type, dir, start, end, length)
    {
      setText(8, comment);
    }

  FeatureListItem(FeatureListItem * parent, 
                  QString ncode,
                  QString siid,
                  QString count,
                  QString type,
                  QString dir,
                  QString start,
                  QString end,
                  QString length,
                  QString comment)
               
    : Q3ListViewItem(parent, ncode, siid, count, type, dir, start, end, length)
    {
      setText(8, comment);
    }


  int compare(Q3ListViewItem *i, int col,
              bool ascending ) const
  {
    if (col == 8)
    {
      return key(col,ascending).compare(i->key(col,ascending));
    }

    return atoi(key(col,ascending)) - atoi(i->key(col,ascending));
  }
};



void LaunchPad::initFeatures()
{
  connect(featureList, SIGNAL(doubleClicked(Q3ListViewItem *)),
          this,  SLOT(featureSelected(Q3ListViewItem *)));

  connect(featureList, SIGNAL(returnPressed(Q3ListViewItem *)),
          this,  SLOT(featureSelected(Q3ListViewItem *)));

  connect(groupGroup, SIGNAL(clicked(int)),
          this,  SLOT(featureGroupChanged(int)));
}

void LaunchPad::featureGroupChanged(int id)
{
  loadFeatures();
}

void LaunchPad::loadFeatures()
{
  EventTime_t timer;
  if (m_verbose) { cerr << "Loading Features..."; }

  QCursor orig = cursor();
  setCursor(Qt::waitCursor);

  featureList->clear();

  int c = featureList->columns();
  for (int i = 0; i < c; i++)
  {
    featureList->removeColumn(0);
  }

  if (featureGroupContigButton->isChecked())
  {
    featureList->addColumn("Source Type");
    featureList->addColumn("Source IID");
    featureList->addColumn("Count");
    featureList->addColumn("Type");
    featureList->addColumn("Dir");
    featureList->addColumn("Start");
    featureList->addColumn("End");
    featureList->addColumn("Length");
    featureList->addColumn("Comment");
  }
  else
  {
    featureList->addColumn("Type");
    featureList->addColumn("Count");
    featureList->addColumn("Source Type");
    featureList->addColumn("Source IID");
    featureList->addColumn("Dir");
    featureList->addColumn("Start");
    featureList->addColumn("End");
    featureList->addColumn("Length");
    featureList->addColumn("Comment");
  }


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
      AMOS::Range_t range = feat.getRange();

      string s;

      if (featureGroupContigButton->isChecked())
      {
        sprintf(buffer, "%c:%d", (char)feat.getSource().second, feat.getSource().first);
        s += buffer;
      }
      else
      {
        s += feat.getType();
      }

      ofmi = objectFeatureMap.find(s);
      FeatureListItem * base;

      if (ofmi == objectFeatureMap.end())
      {
        if (featureGroupContigButton->isChecked())
        {
          base = new FeatureListItem(featureList,
                                     QString(Decode(feat.getSource().second).c_str()),
                                     QString::number(feat.getSource().first),
                                     QString("1"),
                                     QString(""),
                                     QString(""),
                                     QString(""),
                                     QString(""),
                                     QString(""),
                                     QString(""));
        }
        else
        {
          base = new FeatureListItem(featureList,
                                     QString(UIElements::getFeatureStr((char)feat.getType())),
                                     QString("1"),
                                     QString(""),
                                     QString(""),
                                     QString(""),
                                     QString(""),
                                     QString(""),
                                     QString(""),
                                     QString(""));
        }

        ofmi = objectFeatureMap.insert(make_pair(s, base)).first;
      }
      else
      {
        base = ofmi->second;
      }

      
      if (featureGroupContigButton->isChecked())
      {
        new FeatureListItem(base,
                            QString(Decode(feat.getSource().second).c_str()),
                            QString::number(feat.getSource().first),
                            QString::number(1),
                            QString(UIElements::getFeatureStr((char)feat.getType())),
                            QString((QChar)(range.isReverse()?'R':'F')),
                            QString::number(range.getLo()),
                            QString::number(range.getHi()),
                            QString::number(range.getLength()),
                            QString(feat.getComment().c_str()));

        base->setText(2, QString::number(base->childCount()));
      }
      else
      {
        new FeatureListItem(base,
                            QString(UIElements::getFeatureStr((char)feat.getType())),
                            QString::number(1),
                            QString(Decode(feat.getSource().second).c_str()),
                            QString::number(feat.getSource().first),
                            QString((QChar)(range.isReverse()?'R':'F')),
                            QString::number(range.getLo()),
                            QString::number(range.getHi()),
                            QString::number(range.getLength()),
                            QString(feat.getComment().c_str()));

        base->setText(1, QString::number(base->childCount()));
      }
    }
  }
  catch (AMOS::Exception_t & e)
  {
    statusBar()->message("Features not available");
  }

  if (featureGroupContigButton->isChecked())
  {
    featureList->setSorting(2,false);
  }
  else
  {
    featureList->setSorting(1,false);
  }

  setCursor(orig);

  if (m_verbose) { cerr << "      " << timer.str() << endl; }
}

void LaunchPad::featureViewButtonSelected()
{
  Q3ListViewItem * item = featureList->selectedItem();
  if (item)
  {
    featureSelected(item);
  }
}

void LaunchPad::featureSelected(Q3ListViewItem * item)
{
  int offset = atoi(item->text(5));
  ID_t iid   = atoi(item->text(3));
  char ncode = item->text(2).ascii()[0];

  if (featureGroupContigButton->isChecked())
  {
    iid   = atoi(item->text(1));
    ncode = item->text(0).ascii()[0];
  }

  if (ncode == (char)Contig_t::NCODE)
  {
    ID_t bid = m_datastore->contig_bank.lookupBID(iid);
    setContigId(bid);
    setGindex(offset);
  }
  else if (ncode == Scaffold_t::NCODE)
  {
    AMOS::Scaffold_t scaffold;

    m_datastore->fetchScaffoldIID(iid, scaffold);

    ID_t contigbid = 0;

    vector<Tile_t>::iterator ti;
    for (ti = scaffold.getContigTiling().begin();
         ti != scaffold.getContigTiling().end();
         ti++)
    {
      if ((ti->offset <= offset) && (offset <= ti->getRightOffset()))
      {
        contigbid = m_datastore->contig_bank.getIDMap().lookupBID(ti->source);
        offset -= ti->offset;

        if (ti->range.isReverse())
        {
          offset = ti->getGappedLength() - offset;
        }

        break;
      }
    }

    if (contigbid)
    {
      setContigId(contigbid);
      setGindex(offset);
    }
  }
}
