#ifndef RENDERSEQ_H
#define RENDERSEQ_H

#include <algorithm>

#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

#include "ChromoField.hh"

class RenderSeq_t
{
public:
  RenderSeq_t(int vectorpos);
  ~RenderSeq_t();
  void load(AMOS::Bank_t & read_bank, AMOS::Tile_t * tile);
  char base(AMOS::Pos_t gindex) const;
  int qv(AMOS::Pos_t gindex) const;
  AMOS::Pos_t getGindex(AMOS::Pos_t seqpos) const;
  void loadTrace(const std::string & db);
  int getGSeqPos(int gindex);

  static bool hasOverlap(AMOS::Pos_t rangeStart, // 0-based exact offset of range
                         AMOS::Pos_t rangeEnd,   // 0-based exact end of range
                         AMOS::Pos_t seqOffset,  // 0-bases exact offset of seq
                         AMOS::Pos_t seqLen,     // count of bases of seq (seqend+1)
                         AMOS::Pos_t contigLen);  // count of bases in contig (contigend+1)

  struct TilingOrderCmp
  {
    bool operator() (const AMOS::Tile_t & a, const AMOS::Tile_t & b)
    {
      int offdiff = b.offset - a.offset;

      if (offdiff)
      {
        if (offdiff < 0) { return false; }
        return true;
      }

      int lendiff = (b.range.getLength() + b.gaps.size()) -
                    (a.range.getLength() + a.gaps.size());

      if (lendiff)
      {
        if (lendiff < 0) { return false; }
        return true;
      }

      return (a.source < b.source);
    }
  };


  int m_displaystart;
  int m_displayend;

  int m_loffset;
  int m_roffset;

  int m_vectorpos;
  AMOS::Read_t read;
  std::string m_nucs;
  std::string m_qual;
  bool m_rc;
  AMOS::Read_t m_read;
  AMOS::Tile_t * m_tile;

  Read * m_trace;
  std::vector<int> m_bcpos;
  bool m_displayTrace;
};

#endif
