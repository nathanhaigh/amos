#ifndef INSERT_CANVAS_ITEM_HH_
#define INSERT_CANVAS_ITEM_HH_ 1

#include <q3canvas.h>
#include "Insert.hh"

class Insert;


class InsertCanvasItem : public Q3CanvasRectangle
{
public:
  InsertCanvasItem(int x, int y, int width, int height,
                   Insert * insert, Q3Canvas * canvas);

  static const int RTTI;
  int rtti () const { return RTTI; }
  Insert * m_insert;
  bool m_highlight;

  bool m_contigcolor;
  QColor m_acolor;
  int m_alinked;
  int m_alinkedread;

  QColor m_bcolor;
  int m_blinked;
  int m_blinkedread;

protected:
  void drawShape (QPainter & p);

private:
  void drawTile(AMOS::Tile_t * tile, QPainter & p, char type);

private:
};


#endif
