#ifndef RENDERSEQ_H
#define RENDERSEQ_H

#include <algorithm>

#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

using namespace AMOS;

class RenderSeq_t
{
public:
  RenderSeq_t(int vectorpos);
  ~RenderSeq_t();
  int m_offset;
  void load(AMOS::Bank_t & read_bank, vector<AMOS::Tile_t>::const_iterator tile);

  char base(Pos_t gindex) const;

  int m_vectorpos;
  AMOS::Read_t read;
  string m_nucs;
  string m_qual;
  bool m_rc;
  AMOS::Read_t m_read;
};



#endif
