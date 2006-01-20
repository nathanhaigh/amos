#ifndef FEATURE_BROWSER_HH__
#define FEATURE_BROWSER_HH__ 1

#include <string>
#include <qmainwindow.h>
#include <qlistview.h>

using std::string;

class DataStore;

class FeatureBrowser : public QMainWindow
{
  Q_OBJECT

public:
  FeatureBrowser(DataStore * datastore, QWidget * parent, const char * name);

public slots:
  void itemSelected(QListViewItem * item);
  void selecteid(const QString & eid);
  void acceptSelected();
  void refreshTable();

signals:
  void setGindex(int);
  void setContigId(int);

private:
  void loadTable();

  DataStore * m_datastore;
  QListView * m_table;
};


#endif
