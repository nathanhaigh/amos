#ifndef INSERT_FIELD_HH_
#define INSERT_FIELD_HH_ 1

#include <qcanvas.h>

class DataStore;
class Insert;
class InsertCanvasItem;

class InsertField : public QCanvasView
{
  Q_OBJECT

public:
  InsertField(DataStore * datastore,
              int & hoffset,
              QCanvas * canvas, 
              QWidget * parent = 0, 
              const char * name = 0);

signals:
  void visibleRange(int, double);
  void setDetails(const QString & message);
  void setGindex(int gindex);

  void readIIDHighlighted(const QString &);
  void readEIDHighlighted(const QString &);
  void jumpToRead(int iid);


public slots:

  void highlightRead(int iid);
  void highlightIID(const QString & qeid);
  void highlightEID(const QString & qeid);

  void canvasCleared();

  void search(const QString & str);

protected:
  void contentsMousePressEvent(QMouseEvent* e);
  void viewportPaintEvent(QPaintEvent * e);

private:
  void processItemSelection(QString & s,
                            QCanvasItemList::Iterator & it, 
                            int & jumptoread,
                            bool & jump,
                            bool rightButtonDown,
                            bool jumpToBuddy,
                            int clickpos);
  
  void setFeatRect (QCanvasItem * item);
  void highlightInsert(InsertCanvasItem * iitem, bool highlight, bool highlightBuddy);
  void getInsertString(QString & s, int which, Insert * ins, int isSecond, InsertCanvasItem * iitem);
  DataStore * m_datastore;
  int & m_hoffset;

  QCanvasRectangle * m_visibleRect;
  QCanvasRectangle * m_featrect;
  QCanvasItem * m_feat;
  QCanvasItem * m_lastsearch;
};

#endif
