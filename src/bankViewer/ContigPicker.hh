#ifndef CONTIG_PICKER_HH__
#define CONTIG_PICKER_HH__ 1

#include <string>
#include <qmainwindow.h>
#include <qlistview.h>

using std::string;

class ContigPicker : public QMainWindow
{
  Q_OBJECT

public:
  ContigPicker(const string & bankname, QWidget * parent, const char * name);

public slots:
  void itemSelected(QListViewItem * item);

signals:
  void contigSelected(int);

};


#endif
