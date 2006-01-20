#ifndef SCAFFOLD_PICKER_HH__
#define SCAFFOLD_PICKER_HH__ 1

#include <string>
#include <qmainwindow.h>
#include <qlistview.h>
#include <qpopupmenu.h>

using std::string;

class DataStore;

class ScaffoldPicker : public QMainWindow
{
  Q_OBJECT

public:
  ScaffoldPicker(DataStore * datastore, QWidget * parent, const char * name);

public slots:
  void itemSelected(QListViewItem * item);
  void selectiid(const QString & iid);
  void selecteid(const QString & eid);
  void acceptSelected();
  void toggleShowContigs();
  void refreshTable();

signals:
  void contigSelected(int);
  void setGindex(int);

private:
  QListView * m_table;

  void loadTable(bool jumpToCurrent);
  bool m_showContigs;
  int m_showcontigsid;
  QPopupMenu * m_options;
  DataStore * m_datastore;

};


#endif
