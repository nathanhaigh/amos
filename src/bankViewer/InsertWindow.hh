#ifndef INSERTWINDOW_HH_
#define INSERTWINDOW_HH_ 1

#include <qmainwindow.h>
#include <string>
#include "DataStore.hh"


class InsertWindow : public QMainWindow
{
  Q_OBJECT

public:
  InsertWindow(DataStore * datastore, QWidget* parent=0, const char* name=0);

public slots:
  void contigChanged();

signals:
  void setGindex(int gindex);
  void visibleRange(int, int);
  void setTilingVisibleRange(int, int);
  void refreshCanvas();
};

#endif
