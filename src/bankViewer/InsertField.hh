#ifndef INSERT_FIELD_HH_ 
#define INSERT_FIELD_HH_ 1

#include <qpixmap.h>
#include <qwidget.h>
#include <string>
#include "foundation_AMOS.hh"
#include "Insert.hh"

class InsertField : public QWidget
{
  Q_OBJECT

public:
  InsertField(const string & bankname,
              int contigId,
              QWidget * parent=0,
              const char * name = 0);

protected:
  void paintEvent(QPaintEvent * e);


private:
  void drawTile(AMOS::Tile_t * tile, QPainter & p, int hoffset, int vpos);
  AMOS::Distribution_t getLibrarySize(ID_t readid);

  double m_scale;
  int m_seqheight;

  AMOS::Bank_t read_bank;
  AMOS::Bank_t contig_bank;
  AMOS::Bank_t frag_bank;
  AMOS::Bank_t lib_bank;
  AMOS::BankStream_t mate_bank;

  string m_bankname;
  int m_contigId;

  string m_consensus;
  std::vector<AMOS::Tile_t> m_tiling;
  QPixmap * m_pix;
};


#endif
