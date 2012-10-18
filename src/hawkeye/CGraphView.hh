#ifndef CGRAPHVIEW_HH_
#define CGRAPHVIEW_HH_ 1

#include <q3canvas.h>
//Added by qt3to4:
#include <QMouseEvent>
#include "CGraphEdge.hh"

class CGraphView : public Q3CanvasView
{
  Q_OBJECT

public:
  CGraphView(Q3Canvas * canvas,
             QWidget * parent = 0,
             const char * name = 0);

signals:
  void setContigId(int);
  void edgeSelected(AMOS::ContigEdge_t *);

protected:
  void contentsMousePressEvent(QMouseEvent * e);

};

#endif
