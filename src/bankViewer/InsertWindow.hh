#ifndef INSERTWINDOW_HH_
#define INSERTWINDOW_HH_ 1

#include <qmainwindow.h>
#include <string>
#include "DataStore.hh"
#include <qpopupmenu.h>
#include <qlineedit.h>

#include <map>


class InsertWindow : public QMainWindow
{
  Q_OBJECT

public:
  InsertWindow(DataStore * datastore, QWidget* parent=0, const char* name=0);

public slots:
  void contigChanged();
  void toggleItem(int id);

  void toggleConnectMates();
  void togglePartitionTypes();
  void toggleTintHappiness();
  void toggleCoveragePlot();
  void toggleCEStatistic();
  void toggleFeatures();
  void toggleColorByLibrary();
  void togglePaintScaffold();

  void loadHappyDistance();

signals:
  void setGindex(int gindex);
  void setContigId(int contigid);
  void visibleRange(int, int);
  void setTilingVisibleRange(int, int, int);
  void paintCanvas();
  void newContig();
  void refreshInserts();
  void setHappyDistance(float);

  void setConnectMates(bool);
  void setPartitionTypes(bool);
  void setCoveragePlot(bool);
  void setCEStatistic(bool);
  void setFeatures(bool);
  void setColorByLibrary(bool);
  void setPaintScaffold(bool);
  void setTintHappiness(bool);

private:

  typedef std::map<char, std::pair<int, bool> > typemap;

  typemap m_types;

  QPopupMenu * m_typesmenu;
  QPopupMenu * m_optionsmenu;
  QLineEdit * m_happypick;

  int m_connectmatesid;
  int m_partitiontypesid;
  int m_coverageid;
  int m_featid;
  int m_libcolorid;
  int m_scaffid;
  int m_ceid;
  int m_tintid;
};

#endif
