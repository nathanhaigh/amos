#ifndef INSERT_FIELD_HH_
#define INSERT_FIELD_HH_ 1

#include <q3canvas.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPaintEvent>

class DataStore;
class Insert;
class InsertCanvasItem;
class RangeScrollBar_t;

class InsertField : public Q3CanvasView
{
  Q_OBJECT

public:
  InsertField(DataStore * datastore,
              int & hoffset,
              Q3Canvas * canvas, 
              QWidget * parent = 0, 
              const char * name = 0);

  void setScrollBars(RangeScrollBar_t * hrange, RangeScrollBar_t * vrange);

signals:
  void visibleRange(int, double);
  void setDetails(const QString & message);
  void setGindex(int gindex);

  void readIIDHighlighted(const QString &);
  void readEIDHighlighted(const QString &);
  void jumpToRead(int iid);
  void updateVisibleRange();
  void fieldResized();

public slots:
  void resizeEvent(QResizeEvent * e);

  void setZoomInTool();
  void setZoomOutTool();
  void setSelectTool();


  void highlightRead(int iid);
  void highlightIID(const QString & qeid);
  void highlightEID(const QString & qeid);

  void canvasCleared();

  void search(const QString & str);


protected:
  void contentsMousePressEvent(QMouseEvent* e);
  void wheelEvent(QWheelEvent *e);
  void viewportPaintEvent(QPaintEvent *e);

private:
  void processItemSelection(QString & s,
                            Q3CanvasItemList::Iterator & it, 
                            int & jumptoread,
                            bool & jump,
                            bool rightButtonDown,
                            bool jumpToBuddy,
                            int clickpos);
  
  void setFeatRect (Q3CanvasItem * item);
  void highlightInsert(InsertCanvasItem * iitem, bool highlight, bool highlightBuddy);
  void getInsertString(QString & s, int which, Insert * ins, int isSecond, InsertCanvasItem * iitem);
  DataStore * m_datastore;
  int & m_hoffset;

  Q3CanvasRectangle * m_featrect;
  Q3CanvasItem * m_feat;
  Q3CanvasItem * m_lastsearch;

  RangeScrollBar_t * m_hrange;
  RangeScrollBar_t * m_vrange;

  int m_toolstate;
};

#endif
