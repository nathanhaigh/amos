#ifndef INSERT_CANVAS_HH_ 
#define INSERT_CANVAS_HH_ 1

#include <qpixmap.h>
#include <qcanvas.h>
#include <qwidget.h>
#include <string>
#include "foundation_AMOS.hh"
#include "Insert.hh"

class InsertCanvas : public QCanvas
{
public:
  InsertCanvas(const std::string & bankname,
              int contigId,
              QWidget * parent=0,
              const char * name = 0);

  ~InsertCanvas();


private:
  void drawTile(AMOS::Tile_t * tile, QCanvas * p, int hoffset, int vpos, Insert::MateState state);
  AMOS::Distribution_t getLibrarySize(AMOS::ID_t readid);

  int m_seqheight;
  int m_hoffset;

  AMOS::Bank_t read_bank;
  AMOS::Bank_t contig_bank;
  AMOS::Bank_t frag_bank;
  AMOS::Bank_t lib_bank;
  AMOS::BankStream_t mate_bank;

  std::string m_bankname;
  int m_contigId;

  std::string m_consensus;
  std::vector<AMOS::Tile_t> m_tiling;
  std::vector<Insert *> m_inserts;
};


#endif
