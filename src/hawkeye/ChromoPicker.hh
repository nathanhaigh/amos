#ifndef CHROMO_PICKER_HH__
#define CHROMO_PICKER_HH__ 1

#include <string>
#include <q3mainwindow.h>
#include <q3listview.h>
#include <qcheckbox.h>

#include <qradiobutton.h>

using std::string;

class ChromoPicker : public Q3MainWindow
{
  Q_OBJECT

public:
  ChromoPicker(QWidget * parent, const char * name);

public slots:
  void itemSelected(Q3ListViewItem * item);
  void selectiid(const QString & iid);
  void selecteid(const QString & eid);
  void acceptSelected();
  void setTraceCmdEnabled(bool enabled);


  void addNew();

signals:
  void highlightRead(int);

private:
  Q3ListView * m_table;

  QLineEdit * m_pathpick;
  QLineEdit * m_fetchpick;
  QLineEdit * m_fetchresultpick;
  QLineEdit * m_fetchcachepick;
  QLineEdit * m_fetchdbpick;
  QCheckBox * m_enabled;
};


#endif
