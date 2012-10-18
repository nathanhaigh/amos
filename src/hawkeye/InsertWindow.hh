#ifndef INSERTWINDOW_HH_
#define INSERTWINDOW_HH_ 1

#include <q3mainwindow.h>
#include <string>
#include <q3popupmenu.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qpixmap.h>
class QueryWidget;
class DetailWidget;
class InsertWidget;
class SelectionWidget;

#include <map>


class DataStore;

class InsertWindow : public Q3MainWindow
{
  Q_OBJECT

public:
  InsertWindow(DataStore * datastore, QWidget* parent=0, const char* name=0);

  static int s_persistant;

public slots:
  void bankChanged();
  void contigChanged();
  void loadSearch();
  void loadHappyDistance();

  void toggleItem();

  void setMaxInsertCovTol(int);
  void setMaxReadCovTol(int);
//   void setMaxSNPTol(int);
//   void setMaxUnitigTol(int);
//   void setMaxQCTol(int);
//   void setMaxBreakTol(int);

  void toggleSyncWithTiling();

  void clickSelectTool();
  void clickZoomInTool();
  void clickZoomOutTool();

signals:
  void setGindex(int gindex);
  void setContigId(int contigid);
  void visibleRange(int, int);
  void setTilingVisibleRange(int, int, int);
  void paintCanvas();
  void newContig();
  void refreshInserts();

  void setHappyDistance(float);
  void search(const QString &);
  void highlightRead(int);
  void showAll();

private:

  QPixmap mateIcon (const QColor & color, int mode);
  void buildLibraryBox();

  typedef std::map<char, std::pair<int, bool> > typemap;

  typemap m_types;

  Q3PopupMenu * m_libmenu;
  Q3PopupMenu * m_typesmenu;
  Q3PopupMenu * m_optionsmenu;

  DataStore * m_datastore;

  InsertWidget * m_inserts;
  QueryWidget * m_query;
  DetailWidget * m_detail;
  SelectionWidget * m_select;

  int m_connectmatesid;
  int m_partitiontypesid;
  int m_coverageid;
  int m_libcolorid;
  int m_scaffid;
  int m_ceid;
  int m_tintid;
  int m_tintfeatid;
  int m_matecolorid;
  int m_showscaffid;
  QWidget * m_libLegend;
};

#endif
