#ifndef CONTIGUTILS_HH_
#define CONTIGUTILS_HH_ 1

#include "foundation_AMOS.hh"
#include <string>
#include <set>

void leftTrimContig(AMOS::Contig_t & contig, int lefttrim);
void rightTrimContig(AMOS::Contig_t & contig, int righttrim);
void lengthTrimContig(AMOS::Contig_t & contig, int righttrim);


void shiftContigRight(AMOS::Contig_t & contig, int contigShiftDistance);
void widenContig(AMOS::Contig_t & contig, int contigGrowDistance);



AMOS::ID_t lookupIID(AMOS::Bank_t & bnk, const std::string & eid);

void reverseContig(AMOS::Bank_t & contig_bank,
                   AMOS::Bank_t & scaffold_bank,
                   AMOS::BankStream_t & feat_bnk,
                   AMOS::ID_t contigiid);

void stitchContigs(AMOS::Bank_t & master_contig, 
                   AMOS::Bank_t & master_reads,
                   AMOS::Bank_t & master_scaff,
                   AMOS::BankStream_t & master_feat,
                   AMOS::Bank_t & patch_contig,
                   AMOS::Bank_t & patch_reads,
                   AMOS::BankStream_t & patch_feat,
                   const std::string & leftstitchread,
                   const std::string & rightstitchread,
                   std::set<AMOS::ID_t> & masterreads,
                   std::set<AMOS::ID_t> & patchreads,
                   AMOS::Range_t & stitchRegion,
                   AMOS::ID_t masteriid,
                   AMOS::ID_t master2iid,
                   AMOS::ID_t patchiid,
                   bool savetonew,
                   bool perfectovl);




#endif
