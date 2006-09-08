#ifndef RENDERSEQ_H
#define RENDERSEQ_H

#include <algorithm>

#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

class DataStore;

class RenderSeq_t
{

public:
  RenderSeq_t();
  ~RenderSeq_t();

  void load(DataStore * datastore, AMOS::Tile_t * tile);
  void loadTrace();

  char base(AMOS::Pos_t gindex, bool outsideclr = false, AMOS::Pos_t conslen=-1) const;
  int    qv(AMOS::Pos_t gindex, bool outsideclr = false, AMOS::Pos_t conslen=-1) const;
  int   pos(AMOS::Pos_t gindex, bool outsideclr = false, AMOS::Pos_t conslen=-1) const;

  void getBases(std::string & alignedBases, 
                AMOS::Pos_t grangeStart, 
                AMOS::Pos_t grangeEnd);

  int getGSeqPos(int gindex);
  AMOS::Pos_t getGindex(AMOS::Pos_t seqpos) const;
  AMOS::Pos_t gappedLen() const;
  AMOS::Pos_t fullLen() const;

  // Tiling Specific
  int m_displaystart;
  int m_displayend;

  int m_loffset;
  int m_roffset;

  char bgcolor;

  // Projected left and right offsets to full range sequence
  int m_lfoffset;
  int m_rfoffset;

  // full gapped sequence contig oriented
  std::string          m_bases;     
  std::string          m_quals;
  std::vector<int16_t> m_pos;

  int m_rangebegin;  // first aligned base of m_bases
  int m_rangeend;    // last  aligned base of m_bases

  bool m_rc;
  AMOS::Read_t m_read;
  AMOS::Tile_t * m_tile;

  char * m_trace;
  bool m_displayTrace;

  static bool hasOverlap(AMOS::Pos_t rangeStart, // 0-based exact offset of range
                         AMOS::Pos_t rangeEnd,   // 0-based exact end of range
                         AMOS::Pos_t seqOffset,  // 0-bases exact offset of seq
                         AMOS::Pos_t seqLen,     // count of bases of seq (seqend+1)
                         AMOS::Pos_t contigLen);  // count of bases in contig (contigend+1)

};

#endif
