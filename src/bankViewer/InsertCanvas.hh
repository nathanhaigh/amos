#ifndef INSERT_CANVAS_HH_ 
#define INSERT_CANVAS_HH_ 1

#include <qpixmap.h>
#include <qcanvas.h>
#include <qwidget.h>
#include <string>
#include "foundation_AMOS.hh"
#include "Insert.hh"
#include "DataStore.hh"

class InsertCanvas : public QCanvas
{
public:
  InsertCanvas(DataStore * datastore,
               QWidget * parent=0,
               const char * name = 0);

  ~InsertCanvas();


public:
  void drawCanvas();

private:
  void drawTile(AMOS::Tile_t * tile, QCanvas * p, int hoffset, int vpos, Insert::MateState state);
  AMOS::Distribution_t getLibrarySize(AMOS::ID_t readid);

  int m_seqheight;
  int m_hoffset;

  std::string m_consensus;
  std::vector<AMOS::Tile_t> m_tiling;
  std::vector<Insert *> m_inserts;

  DataStore * m_datastore;
};


#endif
