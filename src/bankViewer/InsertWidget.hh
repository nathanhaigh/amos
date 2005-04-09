#ifndef INSERT_WIDGET_HH_
#define INSERT_WIDGET_HH_ 1

#include <qwidget.h>
#include <string>

#include "InsertField.hh"
#include "InsertPosition.hh"
#include "DataStore.hh"

#include <qslider.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qcanvas.h>
#include <qwidget.h>
#include <string>
#include "foundation_AMOS.hh"
#include "Insert.hh"
#include "DataStore.hh"

#include <map>

using std::map;
using std::pair;



class InsertWidget : public QWidget
{
  Q_OBJECT

public:

  InsertWidget(DataStore * datastore,
               map<char, pair<int, bool> > & types,
               QWidget * parent = 0, const char * name = 0);
  ~InsertWidget();

public slots:
  void initializeTiling();

  void setTilingVisibleRange(int, int, int);
  void setZoom(int);
  void paintCanvas();
  void setConnectMates(bool);
  void setPartitionTypes(bool);
  void setCoveragePlot(bool);
  void setPaintScaffold(bool);
  void setFeatures(bool);
  void setColorByLibrary(bool);
  void contigChanged();
  void computePos(int);

  void start();
  void stopbreak();
  void left();
  void right();
  void timeout();
  void autoplay();


signals:
  void setStatus(const QString & message);
  void setGindex(int gindex);
  void setContigId(int contigid);
  void highlightIID(const QString & qiid);
  void highlightEID(const QString & qeid);

private:
  void flushInserts();
  void computeInsertHappiness();
  void clearCanvas();
  void initializeVisibleRectangle();

  DataStore      * m_datastore;
  InsertField    * m_ifield;
  QCanvas        * m_icanvas;
  InsertPosition * m_iposition;

  QCanvasRectangle * m_tilingVisible;

  QCanvasRectangle * m_paddle;
  QCanvasEllipse   * m_ball;
  QTimer * m_timer;
  int m_xvel;
  int m_yvel;
  int m_autoplay;

  int m_connectMates;
  int m_partitionTypes;
  int m_coveragePlot;
  int m_showFeatures;
  int m_colorByLibrary;
  int m_paintScaffold;

  int m_tilingwidth;
  float m_hscale;

  AMOS::ID_t m_currentContig;
  AMOS::ID_t m_currentScaffold;

  // from insert canvas
  void drawTile(AMOS::Tile_t * tile, QCanvas * p, int hoffset, int vpos, Insert::MateState state);

  int m_seqheight;
  int m_hoffset;

  typedef std::vector<Insert *> InsertList_t;
  InsertList_t m_inserts;

  std::vector<AMOS::Tile_t> m_tiling;
  std::vector<AMOS::Tile_t> m_ctiling;
  std::vector<AMOS::Feature_t> m_features;

  std::map<char, std::pair<int, bool> > & m_types;
};

#endif
