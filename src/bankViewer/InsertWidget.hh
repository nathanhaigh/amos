#ifndef INSERT_WIDGET_HH_
#define INSERT_WIDGET_HH_ 1

#include <qwidget.h>
#include <string>


#include <qslider.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qcanvas.h>
#include <qwidget.h>
#include <string>
#include "foundation_AMOS.hh"
#include "Insert.hh"

#include <map>

using std::map;
using std::pair;


class DataStore;
class InsertField;
class InsertPosition;


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

  void refreshWidget();

  void setTilingVisibleRange(int, int, int);
  void setZoom(int);
  void setVZoom(int);
  void paintCanvas();
  void setConnectMates(bool);
  void setPartitionTypes(bool);
  void setCoveragePlot(bool);
  void setCEStatistic(bool);
  void setPaintScaffold(bool);
  void setFeatures(bool);
  void setColorByLibrary(bool);
  void setColorByMate(bool);
  void setTintHappiness(bool);
  void setTintFeatures(bool);
  void contigChanged();
  void computePos(int);
  void setHappyDistance(float);

  void setPersistant(bool);
  void setErrorRate(int);

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

  void jumpToRead(int iid);

  void highlightRead(int iid);
  void highlightIID(const QString & qiid);
  void highlightEID(const QString & qeid);

  void readIIDHighlighted(const QString & qiid);
  void readEIDHighlighted(const QString & qeid);

private:
  void flushInserts();
  void computeInsertHappiness();
  void clearCanvas();
  void initializeVisibleRectangle();
  void paintCoverage(QPointArray & arr, 
                     std::vector<double> & rawvalues,
                     bool copyraw,
                     int arrLen, 
                     int voffset, int vheight, 
                     int libid, double baseLine,
                     QColor color);

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

  bool m_connectMates;
  bool m_partitionTypes;
  bool m_coveragePlot;
  bool m_cestats;
  bool m_showFeatures;
  bool m_colorByLibrary;
  bool m_colorByMate;
  bool m_paintScaffold;
  bool m_tintHappiness;
  bool m_tintFeatures;

  bool m_persistant;
  int m_error;

  int m_tilingwidth;
  float m_hscale;

  AMOS::ID_t m_currentContig;
  AMOS::ID_t m_currentScaffold;

  // from insert canvas
  void drawTile(AMOS::Tile_t * tile, QCanvas * p, int hoffset, int vpos, Insert::MateState state);

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
};

#endif
