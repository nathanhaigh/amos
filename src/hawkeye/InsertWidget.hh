#ifndef INSERT_WIDGET_HH_
#define INSERT_WIDGET_HH_ 1

#include <qwidget.h>
#include <string>


#include <qslider.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <q3canvas.h>
#include <qwidget.h>
#include <string>
#include "foundation_AMOS.hh"
#include "Insert.hh"
#include "RangeScrollBar.hh"
#include "CoverageStats.hh"

#include <map>

using std::map;
using std::pair;


class DataStore;
class InsertField;
class InsertPosition;
class CoverageStats;
class OverviewField;


class InsertWidget : public QWidget
{
  Q_OBJECT

public:

  InsertWidget(DataStore * datastore,
               map<char, pair<int, bool> > & types,
               QWidget * parent = 0, const char * name = 0);
  ~InsertWidget();

public slots:
  void resizeOverview();
  void resizeField();

  void initializeTiling();
  void updateVisibleRange();
  void centerView(int);

  void refreshWidget();

  void showAll();
  void setTilingVisibleRange(int, int, int);
  void paintCanvas();
  void setConnectMates(bool);
  void setPartitionTypes(bool);
  void setCoveragePlot(bool);
  void setCEStatistic(bool);
  void setPaintScaffold(bool);
  void setTintHappiness(bool);
  void contigChanged();
  void computePos(int);
  void setShowScaffold(bool);
  void setSyncWithTiling(bool);
  void setColorStyle(int);

  void setHappyDistance(float);

  void setVisibleHRange(int, int);
  void setHPos(int);

  void setVisibleVRange(int, int);
  void setVPos(int);

  void setInsertCovTol(int);
  void setReadCovTol(int);
//   void setSNPTol(int);
//   void setUnitigTol(int);
//   void setQCTol(int);
//   void setBreakTol(int);

  void setInsertCovFeatures(bool);
  void setReadCovFeatures(bool);
//   void setSNPFeatures(bool);
//   void setUnitigFeatures(bool);
//   void setQCFeatures(bool);
//   void setBreakFeatures(bool);
  void setMisasmFeatures(bool);
  void setOtherFeatures(bool);

  void start();
  void stopbreak();
  void left();
  void right();
  void timeout();
  void autoplay();


signals:
  void setDetails(const QString & message);
  void setGindex(int gindex);
  void setContigId(int contigid);

  void canvasCleared();

  void jumpToRead(int iid);

  void search(const QString & str);

  void highlightRead(int iid);
  void highlightIID(const QString & qiid);
  void highlightEID(const QString & qeid);

  void readIIDHighlighted(const QString & qiid);
  void readEIDHighlighted(const QString & qeid);

  void currentScaffoldCoordinate(int);

  void newMaxInsertCovTol(int);
  void newMaxReadCovTol(int);
//   void newMaxSNPTol(int);
//   void newMaxUnitigTol(int);
//   void newMaxQCTol(int);
//   void newMaxBreakTol(int);

  void setZoomInTool();
  void setZoomOutTool();
  void setSelectTool();


private:
  void flushInserts();
  void computeInsertHappiness();
  void computeCoverage();
  void disconnectMates();
  void clearCanvas();
  void setInsertCanvasSize(int left, int right);
  void initializeVisibleRectangle();
  void paintCoverage(const PointArray_t & arr, 
                     const std::vector<double> & rawvalues,
                     bool copyraw,
                     int arrLen, 
                     int voffset, int vheight, 
                     int libid, double baseLine,
                     QColor color,
                     bool compressed = false);

  DataStore      * m_datastore;
  InsertField    * m_ifield;
  Q3Canvas        * m_icanvas;
  InsertPosition * m_iposition;

  Q3CanvasRectangle * m_tilingVisible;

  Q3CanvasRectangle * m_paddle;
  Q3CanvasEllipse   * m_ball;
  QTimer * m_timer;
  int m_xvel;
  int m_yvel;
  int m_autoplay;

  bool m_connectMates;
  bool m_partitionTypes;
  bool m_coveragePlot;
  bool m_cestats;
  bool m_colorByLibrary;
  bool m_colorByMate;
  bool m_colorByStretchiness;
  bool m_paintScaffold;
  bool m_tintHappiness;
  bool m_showscaffold;
  bool m_kmercoverageplot;
  bool m_syncWithTiling;

  bool m_insertCovFeatures;
  bool m_readCovFeatures;
//   bool m_snpFeatures;
//   bool m_unitigFeatures;
//   bool m_qcFeatures;
//   bool m_breakFeatures;
  bool m_misasmFeatures;
  bool m_otherFeatures;

  int m_tilingwidth;
  float m_hscale;

  AMOS::ID_t m_currentContig;
  AMOS::ID_t m_currentScaffold;

  // from insert canvas
  void drawTile(AMOS::Tile_t * tile, Q3Canvas * p, int hoffset, int vpos, Insert::MateState state);

  int m_seqheight;
  int m_hoffset;

  int m_contigid;
  int m_gstart;
  int m_gend;

  typedef std::vector<Insert *> InsertList_t;
  InsertList_t m_inserts;

  std::vector<AMOS::Tile_t> m_tiling;
  std::vector<AMOS::Tile_t> m_ctiling;
  std::vector<AMOS::Feature_t> m_features;

  std::map<char, std::pair<int, bool> > & m_types;
  CoverageStats * m_kmerstats;
  CoverageStats * m_insertCL;
  CoverageStats * m_readCL;

  int m_width;

  double m_meaninsertcoverage;
  double m_meanreadcoverage;

  typedef map<AMOS::ID_t, CoverageStats> LibStats;
  LibStats m_libStats;


  RangeScrollBar_t * hrange;
  RangeScrollBar_t * vrange;
  OverviewField * m_overview;

  int m_overviewtop, m_overviewbottom;
  bool m_updatingScrollBars;

  int m_scaffoldId;
  QString m_scaffoldName;
};

#endif
