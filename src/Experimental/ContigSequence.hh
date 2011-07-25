#ifndef CONTIGSEQUENCE_HH 
#define CONTIGSEQUENCE_HH 1

#include "foundation_AMOS.hh"
#include <vector>
#include <ostream>

class ContigSequence
{
public:
  ContigSequence(std::vector<AMOS::Tile_t>::iterator & ti, 
                 AMOS::Read_t & read,
                 int readcount=-1);

  ~ContigSequence();

  int m_readcount;
  std::vector<AMOS::Tile_t>::iterator m_tile;
  AMOS::Read_t m_read;

  const std::string & getSeqname() const { return m_read.getEID();           }
  long getRightEnd() const               { return m_tile->getRightOffset();  }
  long getOffset() const                 { return m_tile->offset;            }
  bool isReverseCompliment() const       { return m_tile->range.isReverse(); }
  int getReadID() const                  { return m_readcount;               }
  AMOS::ID_t getIID() const              { return m_read.getIID();           }

  int rightTwiddle(long gindex, int & twiddleDistance, char cons);
  int leftTwiddle(long gindex, int & twiddleDistance, char cons);

  char base(long gindex);
  int qv(long gindex) const;

  float getAvgQV(long offset, int count) const;
  float getAvgQV3Trim(int count) const;

  void dump();
  void dumpRange(long gindexstart, long gindexend);


  std::string get3Trim();
  int get3TrimLength();

  int extend3Right(const std::string & extendRight);
  int extend3Left(const std::string & extendLeft);

private:
  void swap(long a, long b);

  void renderSequence();

  std::string m_nuc;
  std::string m_qual;
};

#endif
