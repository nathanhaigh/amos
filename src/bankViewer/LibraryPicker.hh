#ifndef LIBRARY_PICKER_HH__
#define LIBRARY_PICKER_HH__ 1

#include <string>
#include <qmainwindow.h>
#include <qlistview.h>
#include "DataStore.hh"

using std::string;

class LibraryPicker : public QMainWindow
{
  Q_OBJECT

public:
  LibraryPicker(DataStore * datastore, QWidget * parent, const char * name);

public slots:
  void itemSelected(QListViewItem * item);
  void selectiid(const QString & iid);
  void selecteid(const QString & eid);
  void acceptSelected();

signals:
  void highlightRead(int);

private:
  DataStore * m_datastore;
  QListView * m_table;
};


#endif
