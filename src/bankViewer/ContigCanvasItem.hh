#ifndef CONTIG_CANVAS_ITEM_HH_
#define CONTIG_CANVAS_ITEM_HH_ 1

#include <qcanvas.h>
#include "foundation_AMOS.hh"


class ContigCanvasItem : public QCanvasRectangle
{
public:
  ContigCanvasItem(int x, int y, int width, int height, AMOS::Tile_t tile, QCanvas * canvas);

  static const int RTTI;
  int rtti () const { return RTTI; }

  AMOS::Tile_t m_tile;
};


#endif
