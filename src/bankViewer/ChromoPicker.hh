#ifndef CHROMO_PICKER_HH__
#define CHROMO_PICKER_HH__ 1

#include <string>
#include <qmainwindow.h>
#include <qlistview.h>
#include <qcheckbox.h>
#include "DataStore.hh"

#include <qradiobutton.h>

using std::string;

class ChromoPicker : public QMainWindow
{
  Q_OBJECT

public:
  ChromoPicker(DataStore * datastore, QWidget * parent, const char * name);

public slots:
  void itemSelected(QListViewItem * item);
  void selectiid(const QString & iid);
  void selecteid(const QString & eid);
  void acceptSelected();

  void addNew();

signals:
  void highlightRead(int);

private:
  DataStore * m_datastore;
  QListView * m_table;

  QLineEdit * m_pathpick;
  QLineEdit * m_fetchpick;
  QLineEdit * m_fetchresultpick;
  QLineEdit * m_fetchcachepick;
  QLineEdit * m_fetchdbpick;
  QCheckBox * m_enabled;
};


#endif
