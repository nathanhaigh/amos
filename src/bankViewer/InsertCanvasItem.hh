#ifndef INSERT_CANVAS_ITEM_HH_
#define INSERT_CANVAS_ITEM_HH_ 1

#include <qcanvas.h>
#include "Insert.hh"


class InsertCanvasItem : public QCanvasRectangle
{
public:
  InsertCanvasItem(int x, int y, int width, int height,
                   Insert * insert, QCanvas * canvas);

  static const int RTTI;
  int rtti () const { return RTTI; }
  Insert * m_insert;

protected:
  void drawShape (QPainter & p);

private:
  void drawTile(AMOS::Tile_t * tile, QPainter & p);

private:
};


#endif
