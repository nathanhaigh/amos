#ifndef INSERTWINDOW_HH_
#define INSERTWINDOW_HH_ 1

#include <qmainwindow.h>
#include <string>
#include "DataStore.hh"
#include <qpopupmenu.h>

#include <map>


class InsertWindow : public QMainWindow
{
  Q_OBJECT

public:
  InsertWindow(DataStore * datastore, QWidget* parent=0, const char* name=0);

public slots:
  void contigChanged();

  void toggleHappy();
  void toggleUnknown();
  void toggleStretched();
  void toggleOrientation();
  void toggleMissing();
  void toggleLinking();
  void toggleNone();

signals:
  void setGindex(int gindex);
  void visibleRange(int, int);
  void setTilingVisibleRange(int, int);
  void refreshCanvas();

private:
  void toggleItem(char c);
  std::map<char, std::pair<int, bool> > m_types;

  QPopupMenu * m_display;
};

#endif
