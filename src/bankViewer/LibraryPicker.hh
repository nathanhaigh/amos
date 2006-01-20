#ifndef LIBRARY_PICKER_HH__
#define LIBRARY_PICKER_HH__ 1

#include <qmainwindow.h>
#include <qlistview.h>


class DataStore;

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
  void refreshTable();
  void readLengthSelected();

signals:
  void highlightRead(int);

private:
  void loadTable();

  DataStore * m_datastore;
  QListView * m_table;
};


#endif
