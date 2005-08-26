#ifndef CONTIG_FATTENER_HH
#define CONTIG_FATTENER_HH 1

#include <vector>
#include "foundation_AMOS.hh"
#include "ContigSequence.hh"

typedef std::vector<long int> DeltaVector;
typedef std::vector<ContigSequence> ContigSequenceSlice;

class ContigFattener
{
public:
  ContigFattener::ContigFattener(AMOS::Contig_t & contig, 
                                 AMOS::Bank_t & read_bank, 
                                 bool verbose);

  ContigFattener::~ContigFattener();

  void extendRead(const std::string & seqname, int distance);
  void growContigLeft(bool requireConfirmation);
  void growContigRight(bool requireConfirmation);

  void fattenContig(int fattenRangeStart, int fattenRangeEnd,
                    int doLeftExtension, int doRightExtension);

private:

  void extendRead(ContigSequence * read, int distance);

  ContigSequence * getContigSequence(const std::string & seqname);
  ContigSequenceSlice getTilingSlice(int gindex);

  void recordExtension(const ContigSequence * read, int distanceExtended,
                       int trimLength, float origAvg, float origTrimAvg, 
                       int origUContigWidth);

  int extendLeft(ContigSequence * read, int distance);
  int extendRight(ContigSequence * read, int distance);

  int fattenLeft(ContigSequence * read, int distance);
  int fattenRight(ContigSequence * read, int distance);

  long int getBasesToSkip(DeltaVector & delta,
                          DeltaVector::const_iterator & deltaiter,
                          bool & consGap,
                          unsigned int cpos,
                          unsigned int rpos,
                          const std::string & trimmed,
                          const std::string & consensus,
                          bool autoPromoteConsGaps);

  void mergeExtension(unsigned int & cpos, 
                      unsigned int & lastConsPos,
                      unsigned int & rpos, 
                      std::string & trimmedSeq, 
                      std::string & consensus,
                      ContigSequence * read,
                      DeltaVector & delta,
                      bool autoPromoteConsGaps,
                      int consMovementDelta);


  AMOS::Contig_t & m_contig;
  AMOS::Bank_t & m_read_bank;
  bool m_verbose;

  long m_gindex;


  void stripSequenceEdgeGaps();

  void printPos(long gindexstart, long gindexend);
  void dumpRange(long gindexstart, long gindexend, ContigSequence * read);
  void dumpAlignment();
  void recallCons();
  int  calculateCQV(long gindex, char & cons);
  void insertGapSlice(long gindex, ContigSequence * read);
};

#endif
