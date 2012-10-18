#ifndef CONTIG_CANVAS_ITEM_HH_
#define CONTIG_CANVAS_ITEM_HH_ 1

#include <q3canvas.h>
#include "foundation_AMOS.hh"


class ContigCanvasItem : public Q3CanvasRectangle
{
public:
  ContigCanvasItem(int x, int y, int width, int height, AMOS::Tile_t tile, Q3Canvas * canvas);

  static const int RTTI;
  int rtti () const { return RTTI; }

  AMOS::Tile_t m_tile;
};


#endif
