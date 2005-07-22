#ifndef READ_PICKER_HH__
#define READ_PICKER_HH__ 1

#include <string>
#include <qmainwindow.h>
#include <qlistview.h>
#include "DataStore.hh"

using std::string;

class ReadPicker : public QMainWindow
{
  Q_OBJECT

public:
  ReadPicker(DataStore * datastore, QWidget * parent, const char * name);

public slots:
  void itemSelected(QListViewItem * item);
  void selectiid(const QString & iid);
  void selecteid(const QString & eid);
  void acceptSelected();
  void contigIdSelected(int contigid);

signals:
  void highlightRead(int);

private:
  void loadTable();

  DataStore * m_datastore;
  QListView * m_table;
};


#endif
