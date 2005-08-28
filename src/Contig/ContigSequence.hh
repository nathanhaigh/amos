#ifndef CONTIGSEQUENCE_HH 
#define CONTIGSEQUENCE_HH 1

#include "foundation_AMOS.hh"
#include <vector>
#include <ostream>

class ContigSequence
{
public:
  ContigSequence(std::vector<AMOS::Tile_t>::iterator & ti, AMOS::Read_t & read);
  ~ContigSequence();

  int m_readcount;
  std::vector<AMOS::Tile_t>::iterator m_tile;
  AMOS::Read_t m_read;


  const std::string & getSeqname() const;


  char & operator[](long gindex);

  int rightTwiddle(long gindex, int & twiddleDistance, char cons);
  int leftTwiddle(long gindex, int & twiddleDistance, char cons);

  void erase(long gindex);
  char base(long gindex);
  int qv(long gindex) const;

  float getAvgQV(long offset, int count) const;
  float getAvgQV3Trim(int count) const;

  void dump();
  void dumpRange(long gindexstart, long gindexend);
  void printReadData(std::ostream & out);
  void printContigSequence(std::ostream & out, long asm_l=1, long asm_r=1);

  void adjustOffset(long gindex, int delta);

  long getRightEnd() const;
  long getOffset() const;
  std::string getRightTrim();
  std::string getLeftTrim();
  int get3TrimLength();

  int extendRight(const std::string & extendRight);
  int extendLeft(const std::string & extendLeft);

  int getReadID() const;
  AMOS::ID_t getIID() const { return m_read.getIID(); }


  bool isReverseCompliment() const;

  long stripLeftGaps();
  long stripRightGaps();

private:
  void swap(long a, long b);

  void renderSequence();

  std::string m_nuc;
  std::string m_qual;
};

#endif
